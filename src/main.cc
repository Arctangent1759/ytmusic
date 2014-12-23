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

std::string echo(std::vector<std::string> request){
  return request[0];
}

using ytmusic::player::Audio;

int main() {
  Datastore ds("/tmp/ytmus.dat");
  ds.AddSong("title", "yt_hash", "artist", "album");
  std::vector<int> songs;
  songs.push_back(1);
  songs.push_back(2);
  songs.push_back(3);
  songs.push_back(4);
  songs.push_back(5);
  ds.AddPlaylist("playlist", songs);
  /*
  Audio songs;
  songs.Enqueue("https://www.youtube.com/watch?v=hHkKJfcBXcw");
  songs.Enqueue("https://www.youtube.com/watch?v=DazKKUShotM");
  songs.Play();
  std::string line;
  std::getline(std::cin, line);
  while(line.compare("q") != 0) {
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
  */
}
