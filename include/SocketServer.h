#include <memory>

#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

namespace ytmusic {
namespace util {

class RequestHandler {
  public:
    virtual std::string HandleRequest(std::string request) = 0;
};

class SocketServer {
  public:
    SocketServer(int port, int backlog, RequestHandler* handler);
    void Start();
    void ServeRequest(int sockfd);
  private:
    int sockfd;
    int backlog;
    std::unique_ptr<RequestHandler> handler;
};

class DummyRequestHandler : public RequestHandler {
  public:
    std::string HandleRequest(std::string request) {
      printf("%s\n",request.c_str());
      return request;
    }
};

}  // namespace util
}  // namespace ytmusic

#endif  // REQUESTHANDLER_H
