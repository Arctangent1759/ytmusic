#include <iostream>

#include "URLDecoder.h"

int main() {
  ytmusic::URLDecoder decoder = ytmusic::URLDecoder();
  std::cout << decoder.DecodeURL("https://www.youtube.com/watch?v=kfchvCyHmsc") << std::endl;
}
