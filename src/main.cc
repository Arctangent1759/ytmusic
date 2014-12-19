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
  std::string song_url = "https://www.youtube.com/watch?v=kfchvCyHmsc";
  Audio songs;
  songs.Enqueue(song_url);
  songs.Play();
  std::string line;
  std::getline(std::cin, line);
  while(line.compare("q") != 0)
  {
    if (line.compare("play") == 0) {
      songs.Play();
    }
    else if (line.compare("pause") == 0) {
      songs.Pause();
    }
    else if (line.compare("skip") == 0) {
      songs.Skip();
    }
    else {
      songs.Enqueue(line);
    }
    std::getline(std::cin, line);
  }
}
