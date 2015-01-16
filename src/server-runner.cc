#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <pwd.h>

#include "URLDecoder.h"
#include "Ytmusd.h"
#include "SocketServer.h"
#include "RequestDispatcher.h"
#include "Audio.h"
#include "Datastore.h"

int main() {
  std::string datastore_path = strcat(getpwuid(getuid())->pw_dir, "/.ytmus");
  ytmusic::ytmusd::Ytmusd* ytmusd = new ytmusic::ytmusd::Ytmusd(datastore_path);
  ytmusic::ytmusd::YtmusdServer server(1759, 5, ytmusd);
  server.Start();
}
