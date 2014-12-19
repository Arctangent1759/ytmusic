#include <functional>
#include <string>
#include <vector>
#include <utility>
#include <regex>

#include "SocketServer.h"

#ifndef REQUESTDISPATCHER_H
#define REQUESTDISPATCHER_H

namespace ytmusic {
namespace util {

typedef std::function<std::string(std::vector<std::string>)> HandlerFunc;

class RequestDispatcher : public ytmusic::util::RequestHandler {
  public:
    std::string HandleRequest(std::string request);
    void RegisterHandler(std::string pattern, HandlerFunc handler);
  private:
    std::vector<std::pair<std::regex, HandlerFunc> > handlers;
};

}  // namespace util
}  // namespace ytmusic

#endif  // REQUESTDISPATCHER_H
