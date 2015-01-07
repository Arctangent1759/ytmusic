#include <gst/gst.h>
#include <queue>
#include <string>
#include <thread>
#include <chrono>

#include "Audio.h"
#include "URLDecoder.h"

namespace ytmusic {
namespace player {

GstElement* init_song(std::string);
void play_song(GstElement* );

Audio::Audio() {
  this->running = true;
  this->started = false;
  this->player_thread = std::thread(&Audio::player, this);
  this->offset = 0;
}

void Audio::Enqueue(std::string url) {
  this->song_queue.push(url);
}

void Audio::Play() {
  if (this->started) {
    this->pipeline_lock.lock();
    GstStateChangeReturn ret = gst_element_set_state (this->pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
      g_printerr ("Unable to set the pipeline to the playing state.\n");
      gst_object_unref (pipeline);
    }
    this->pipeline_lock.unlock();
  }
}

void Audio::Pause() {
  this->pipeline_lock.lock();
  GstStateChangeReturn ret = gst_element_set_state (this->pipeline, GST_STATE_PAUSED);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (pipeline);
  }
  this->pipeline_lock.unlock();
}

void Audio::Skip() {
  this->pipeline_lock.lock();
  GstBus* bus = gst_element_get_bus(this->pipeline);
  gst_bus_post(bus, gst_message_new_eos((GstObject*)bus));
  this->pipeline_lock.unlock();
}

void Audio::ClearQueue() {
  while(!this->song_queue.empty()) {
    this->song_queue.pop();
  }
  this->offset = 0;
}

Audio::~Audio() {
  this->ClearQueue();
  GstBus* bus = gst_element_get_bus(this->pipeline);
  gst_bus_post(bus, gst_message_new_eos((GstObject*)bus));
  this->running = false;
  this->player_thread.join();
}

int Audio::GetOffset() {
  return this->offset;
}

void Audio::player() {
  std::string song_url;
  ytmusic::URLDecoder decoder;
  this->started = true;
  while(this->running) {
    if (!this->song_queue.empty()) {
      song_url = song_queue.front();
      song_url = decoder.DecodeURL(song_url);
      song_queue.pop();
      this->pipeline_lock.lock();
      this->pipeline = init_song(song_url);
      this->pipeline_lock.unlock();
      play_song(this->pipeline);
      this->offset += 1;
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
  }
}

void play_song(GstElement* pipeline) {
  GstBus* bus = gst_element_get_bus (pipeline);

  gboolean terminate = FALSE;
  do {
    GstMessage* msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
        (GstMessageType)(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
   
    if (msg != NULL) {
      GError *err;
      gchar *debug_info;
      switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR:
          gst_message_parse_error (msg, &err, &debug_info);
          g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
          g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
          g_clear_error (&err);
          g_free (debug_info);
          terminate = TRUE;
          break;
        case GST_MESSAGE_EOS:
          g_print ("End-Of-Stream reached.\n");
          terminate = TRUE;
          break;
        case GST_MESSAGE_STATE_CHANGED:
          if (GST_MESSAGE_SRC (msg) == GST_OBJECT (pipeline)) {
            GstState old_state, new_state, pending_state;
            gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
            g_print ("Pipeline state changed from %s to %s:\n",
                gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
          }
          break;
        default:
          g_printerr ("This should not happen.\n");
          break;
      }
      gst_message_unref (msg);
    }
  } while (!terminate);
   
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
}

GstElement* init_song(std::string song_url) {
  gst_init (0, NULL);
    
  GstElement* source = gst_element_factory_make ("uridecodebin", "source");
  GstElement* convert = gst_element_factory_make ("audioconvert", "convert");
  GstElement* sink = gst_element_factory_make ("autoaudiosink", "sink");
  GstElement* pipeline = gst_pipeline_new ("test-pipeline");
  if (!pipeline || !source || !convert || !sink) {
    g_printerr ("Not all elements could be created.\n");
  }
   
  gst_bin_add_many (GST_BIN (pipeline), source, convert , sink, NULL);
  if (!gst_element_link (convert, sink)) {
    g_printerr ("Elements could not be linked.\n");
    gst_object_unref (pipeline);
  }

  g_object_set (source, "uri", song_url.c_str(), NULL);
  g_signal_connect (source, "pad-added", G_CALLBACK (pad_added_handler), convert);

  GstStateChangeReturn ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (pipeline);
  }
  return pipeline;
}

void pad_added_handler (GstElement *src, GstPad *new_pad, GstElement* convert) {
  GstPad *sink_pad = gst_element_get_static_pad (convert, "sink");
  GstPadLinkReturn ret;
  GstCaps *new_pad_caps = NULL;
  GstStructure *new_pad_struct = NULL;
  const gchar *new_pad_type = NULL;
  if (gst_pad_is_linked (sink_pad)) {
    gst_object_unref (sink_pad);
    return;
  }
  new_pad_caps = gst_pad_get_caps (new_pad);
  new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
  new_pad_type = gst_structure_get_name (new_pad_struct);
  if (!g_str_has_prefix (new_pad_type, "audio/x-raw")) {
    g_print ("  It has type '%s' which is not raw audio. Ignoring.\n", new_pad_type);
    gst_object_unref (sink_pad);
    gst_caps_unref (new_pad_caps);
    return;
  }
  ret = gst_pad_link (new_pad, sink_pad);
  if (GST_PAD_LINK_FAILED (ret)) {
    g_print ("  Type is '%s' but link failed.\n", new_pad_type);
    gst_object_unref (sink_pad);
    gst_caps_unref (new_pad_caps);
    return;
  }
}

}  // namespace player
}  // namespace ytmusic
