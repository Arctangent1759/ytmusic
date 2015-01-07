#include <iostream>
#include <string>
#include <vector>

#include "URLDecoder.h"
#include "Ytmusd.h"
#include "SocketServer.h"
#include "RequestDispatcher.h"
#include "Audio.h"
#include "Datastore.h"

using ytmusic::util::RequestDispatcher;
using ytmusic::ytmusd::Datastore;
using std::cout;
using std::endl;
using ytmusic::ytmusd::Ytmusd;
using ytmusic::ytmusd::YtmusdServer;

std::string echo(std::vector<std::string> request){
  return request[0];
}

using ytmusic::player::Audio;

int main() {
  Ytmusd* ytmusd = new Ytmusd("/home/arctangent/.ytmus");
  YtmusdServer server(1759, 5, ytmusd);
  server.Start();
}
