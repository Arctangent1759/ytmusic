#ifndef YTMUSD_H
#define YTMUSD_H

#include <memory>

#include "Audio.h"
#include "SocketServer.h"
#include "RequestDispatcher.h"

namespace ytmusic {
namespace ytmusd {

class Ytmusd {
  public:
    Ytmusd();
    void Play(int index);
    void Play(std::vector<int> indices);
    void Enqueue(int index);
    void Next();
    void Prev();
  private:
    void ClearQueue();
    std::vector<int> queue;
    int now_playing;
    ytmusic::player::Audio audio;
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
