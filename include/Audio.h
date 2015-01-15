#ifndef AUDIO_H
#define AUDIO_H

#include <queue>
#include <string>
#include <gst/gst.h>
#include <mutex>
#include <thread>

#include "URLDecoder.h"

namespace ytmusic {
namespace player {

class Audio {
  public:
    Audio();
    void Enqueue(std::string url);
    void Play();
    void Pause();
    void Skip();
    void Flush();
    void ClearQueue();
    int GetOffset();
    bool IsPlaying();
    ~Audio();
  private:
    int offset;
    void player();
    GstElement* pipeline;
    bool running;
    bool started;
    std::mutex pipeline_lock;
    std::queue<std::string> song_queue;
    std::thread player_thread;
    bool playing;
    std::mutex player_lock;
    bool idle;
};

void pad_added_handler(GstElement *src, GstPad *new_pad, GstElement* convert);

}  // namespace player
}  // namespace ytmusic

#endif  // AUDIO_H
