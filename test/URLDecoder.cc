#include <iostream>

#include "URLDecoder.h"

int main() {
  std::cout << "Testing URLDecoder." << std::endl;
  ytmusic::URLDecoder decoder = ytmusic::URLDecoder();
  std::cout << decoder.DecodeURL("https://www.youtube.com/watch?v=keLtVvmkoXM") << std::endl;
}
