#include "Ytmusd.h"

#include "URLDecoder.h"
#include "Ytmusd.h"

namespace ytmusic {
namespace ytmusd {

Ytmusd::Ytmusd() {
}
void Ytmusd::Play(int index) {
  this->ClearQueue();
  this->Enqueue(index);
  this->now_playing = -1;
}
void Ytmusd::Play(std::vector<int> indices) {
  this->ClearQueue();
  for (int index : indices) {
    this->Enqueue(index);
  }
}
void Ytmusd::Enqueue(int index) {
}
void Ytmusd::Next() {
  this->audio.Skip();
}
void Ytmusd::Prev() {
}
void Ytmusd::ClearQueue() {
  this->queue.clear();
  this->audio.ClearQueue();
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
