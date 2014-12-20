#include "SocketServer.h"
#include "RequestDispatcher.h"

#ifndef YTMUSD_H
#define YTMUSD_H

namespace ytmusic {
namespace ytmusd {

class Ytmusd {
  public:
    Ytmusd();
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
