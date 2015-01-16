#include "RequestDispatcher.h"

#include <iostream>
#include <ctype.h>
#include <re2/re2.h>

namespace ytmusic {
namespace util {

std::string RequestDispatcher::HandleRequest(std::string request) {
  for (auto pattern_handler_pair : this->handlers) {
    if (RE2::FullMatch(request, pattern_handler_pair.first)) {
      return pattern_handler_pair.second(pattern_handler_pair.first, request);
    }
  }
  return "{\"error\":true, \"message\":\"ERROR: Invalid request.\"}";
}
void RequestDispatcher::RegisterHandler(std::string pattern, HandlerFunc handler) {
  this->handlers.emplace_back(pattern + "[\\r\\n]*", handler);
}

}  // namespace util
}  // namespace ytmusic
