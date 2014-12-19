#include "RequestDispatcher.h"

namespace ytmusic {
namespace util {

std::string RequestDispatcher::HandleRequest(std::string request) {
  for (auto regex_handler_pair : this->handlers) {
    if (std::regex_search(request, regex_handler_pair.first)) {
      return regex_handler_pair.second(std::vector<std::string>());
    }
  }
  return "ERROR: Could not find a handle request " + request;
}
void RequestDispatcher::RegisterHandler(std::string pattern, HandlerFunc handler) {
  this->handlers.emplace_back(std::regex(pattern, std::regex_constants::ECMAScript), handler);
}

}  // namespace util
}  // namespace ytmusic
