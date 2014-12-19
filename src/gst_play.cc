#include <gst/gst.h>
#include <string>
   
static void pad_added_handler (GstElement *src, GstPad *pad, GstElement *convert);
int play(std::string url);
   
int play(std::string url) {
  gst_init (0, NULL);
    
  GstElement* source = gst_element_factory_make ("uridecodebin", "source");
  GstElement* convert = gst_element_factory_make ("audioconvert", "convert");
  GstElement* sink = gst_element_factory_make ("autoaudiosink", "sink");
  GstElement* pipeline = gst_pipeline_new ("test-pipeline");
  if (!pipeline || !source || !convert || !sink) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }
   
  gst_bin_add_many (GST_BIN (pipeline), source, convert , sink, NULL);
  if (!gst_element_link (convert, sink)) {
    g_printerr ("Elements could not be linked.\n");
    gst_object_unref (pipeline);
    return -1;
  }

  g_object_set (source, "uri", url.c_str(), NULL);
  g_signal_connect (source, "pad-added", G_CALLBACK (pad_added_handler), convert);

  GstStateChangeReturn ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (pipeline);
    return -1;
  }
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
  gst_object_unref (pipeline);
  return 0;
}
   
static void pad_added_handler (GstElement *src, GstPad *new_pad, GstElement* convert) {
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
