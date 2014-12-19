#include <iostream>
#include <string>
#include <vector>

#include "URLDecoder.h"
#include "Ytmusd.h"
#include "SocketServer.h"
#include "RequestDispatcher.h"

using ytmusic::util::RequestDispatcher;
using std::cout;
using std::endl;

std::string echo(std::vector<std::string> request){
  return request[0];
}

using ytmusic::player::Audio;

int main() {
  RequestDispatcher d;
  d.RegisterHandler("^.*$",echo);
  cout << d.HandleRequest("asdf") << endl;
}
