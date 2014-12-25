#ifndef YTMUSD_H
#define YTMUSD_H

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
    void Play(int key);
    void Play(std::vector<int> keys);
    void PlayPlaylist(int key);
    void Stop();
    void Enqueue(int key);
    void Next();
    void Prev();
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
};

}  // namespace ytmusd
}  // namespace ytmusic

#endif  // YTMUSD_H
