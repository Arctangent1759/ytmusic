#ifndef YTMUSD_H
#define YTMUSD_H

#include <mutex>
#include <memory>

#include "Audio.h"
#include "SocketServer.h"
#include "RequestDispatcher.h"
#include "Datastore.h"

namespace ytmusic {
namespace ytmusd {

class Ytmusd {
  public:
    Ytmusd(std::string datastore_path);
    ::ytmusic::util::Status Play(int key);
    ::ytmusic::util::Status Play(std::vector<int> keys);
    ::ytmusic::util::Status PlayPlaylist(int key);
    ::ytmusic::util::Status Pause();
    ::ytmusic::util::Status Continue();
    ::ytmusic::util::Status TogglePause();
    ::ytmusic::util::Status Stop();
    ::ytmusic::util::Status Enqueue(int key);
    ::ytmusic::util::Status Next();
    ::ytmusic::util::Status Prev();
    std::string GetDirectory();
    std::string GetStatus();
    Datastore* GetDatastore();

  private:
    int GetNowPlayingIndex();
    void Update();
    std::vector<int> queue;
    int now_playing;
    ytmusic::player::Audio audio;
    std::unique_ptr<Datastore> datastore;
};

class YtmusdServer {
  public:
    YtmusdServer(int port, int backlog, Ytmusd* ytmusd);
    void Start();
  private:
    void InitHandlers();
    std::unique_ptr<::ytmusic::util::SocketServer> server;
    ::ytmusic::util::RequestDispatcher* dispatcher;
    std::unique_ptr<Ytmusd> ytmusd;
    std::mutex mut;
};

}  // namespace ytmusd
}  // namespace ytmusic

#endif  // YTMUSD_H
