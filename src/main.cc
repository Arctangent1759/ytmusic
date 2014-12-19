#include <iostream>
#include <string>
#include "Audio.h"
#include "stdio.h"

#include "URLDecoder.h"

using ytmusic::player::Audio;

int main() {
  ytmusic::URLDecoder decoder = ytmusic::URLDecoder();
  std::string song_url = decoder.DecodeURL("https://www.youtube.com/watch?v=kfchvCyHmsc");
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
    else {
      songs.Enqueue(line);
    }
    std::getline(std::cin, line);
  }
}
