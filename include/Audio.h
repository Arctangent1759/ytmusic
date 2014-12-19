#include <string>

namespace ytmusic {
namespace player {

class Audio {
  Audio();
  void Enqueue(std::string url);
  void Play();
  void Pause();
  void ClearQueue();
  ~Audio();
};

}  // namespace player
}  // namespace ytmusic
