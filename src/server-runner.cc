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
  Ytmusd* ytmusd = new Ytmusd("/tmp/ytmus.dat");
  YtmusdServer server(1759, 5, ytmusd);
  server.Start();

  /*
  Datastore ds("/tmp/ytmus.dat");
  cout << ds.AddSong("Satsuki Kiryuin Theme", "wSAp991JIVs", "Kill la Kill", "Kill la Kill").GetMessage() << endl;
  cout << ds.AddSong("Mine Turtle", "DI5_sQ8O-7Y", "Tom Ska", "asdfmovie").GetMessage() << endl;
  cout << ds.AddSong("I Like Trains", "hHkKJfcBXcw", "Tom Ska", "asdfmovie").GetMessage() << endl;
  cout << ds.AddSong("Do the Flop", "L5inD4XWz4U", "Tom Ska", "asdfmovie").GetMessage() << endl;
  cout << ds.AddSong("Hug Every Cat", "sP4NMoJcFd4", "", "").GetMessage() << endl;
  std::vector<int> songs;
  songs.push_back(1);
  songs.push_back(2);
  songs.push_back(3);
  cout << ds.AddPlaylist("asdfmovie", songs).GetMessage() << endl;
  for (int i = 0; i < ds.NumSongs(); ++i) {
    cout << ds.GetSong(i)->title() << " " << ds.GetSong(i)->yt_hash() << " " << ds.GetSong(i)->artist() << " " << ds.GetSong(i)->album() << endl;
  }
  for (int i = 0; i < ds.NumPlaylists(); ++i) {
    cout << ds.GetPlaylist(i)->name() << ": ";
    for (auto song_key : ds.GetPlaylist(i)->song_key()) {
      cout << ds.GetSong(song_key)->title() << " ";
    }
    cout << endl;
  }
  */
  /*
  cout << ds.GetSong(0)->title() << endl;
  cout << ds.GetSong(1)->title() << endl;
  cout << ds.GetSong(2)->title() << endl;
  cout << ds.GetSong(3)->title() << endl;
  cout << ds.GetSong(4)->title() << endl;
  cout << ds.GetSong(5)->title() << endl;
  cout << ds.GetSong(6)->title() << endl;
  cout << ds.GetSong(7)->title() << endl;
  cout << ds.GetSong(8)->title() << endl;
  cout << ds.GetSong(9)->title() << endl;
  cout << ds.GetSong(10)->title() << endl;
  cout << (ds.GetSong(4) == NULL) << endl;
  cout << ds.GetPlaylist(0)->name() << endl;
  cout << (ds.GetPlaylist(4) == NULL) << endl;
  */

  /*
  Audio songs;
  //songs.Enqueue("https://www.youtube.com/watch?v=hHkKJfcBXcw");
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
      if (!line.empty()) {
        songs.Enqueue(line);
      }
    }
    std::getline(std::cin, line);
  }
  */
}
