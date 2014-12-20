#include "Ytmusd.h"

#include "URLDecoder.h"
#include "Ytmusd.h"

namespace ytmusic {
namespace ytmusd {

Ytmusd::Ytmusd() {
}

YtmusdServer::YtmusdServer(int port, int backlog, Ytmusd* ytmusd) {
  this->dispatcher = new ::ytmusic::util::RequestDispatcher;
  this->ytmusd = std::unique_ptr<Ytmusd>(ytmusd);
  this->server = std::unique_ptr<::ytmusic::util::SocketServer>(
      new ::ytmusic::util::SocketServer(port, backlog, this->dispatcher));
  this->InitHandlers();
}
void YtmusdServer::Start() {
  this->server->Start();
}
void YtmusdServer::InitHandlers() {
  dispatcher->RegisterHandler("/test/.*", [](std::string request) {
      return request;
  });
}

}  // namespace ytmusd
}  // namespace ytmusic
