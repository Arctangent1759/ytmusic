#include <iostream>
#include <string>
#include "Audio.h"
#include "stdio.h"

#include "URLDecoder.h"

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
