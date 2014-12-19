#include <queue>
#include <string>
#include <gst/gst.h>
#include <mutex>
#include <thread>

namespace ytmusic {
namespace player {

class Audio {
  public:
    Audio();
    void Enqueue(std::string url);
    void Play();
    void Pause();
    void ClearQueue();
    ~Audio();
  private:
    void player();
    GstElement* pipeline;
    std::mutex pipeline_lock;
    std::queue<std::string> song_queue;
    std::thread player_thread;
    
};

void pad_added_handler(GstElement *src, GstPad *new_pad, GstElement* convert);
}  // namespace player
}  // namespace ytmusic
