#include "RequestDispatcher.h"

#include <iostream>

using std::cout;
using std::endl;

namespace ytmusic {
namespace util {

std::string RequestDispatcher::HandleRequest(std::string request) {
  for (auto regex_handler_pair : this->handlers) {
    std::smatch matches;
    if (std::regex_match(request, matches, regex_handler_pair.first)) {
      for(size_t i=0; i<matches.size(); ++i) {
          cout << matches[i] << endl;
      }
      return regex_handler_pair.second(std::vector<std::string>());
    }
  }
  return "ERROR: Could not find a handle request " + request;
}
void RequestDispatcher::RegisterHandler(std::string pattern, HandlerFunc handler) {
  this->handlers.emplace_back(std::regex(pattern), handler);
}

}  // namespace util
}  // namespace ytmusic
