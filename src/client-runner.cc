#include <ncurses.h>
#include <fstream>
#include <map>
#include <vector>
#include <cmath>
#include <iostream>
using std::cout;
using std::endl;

#include "ClientApp.h"
#include "YTMClient.h"

int main() {
  /*
  YTMClient::YTMClient ytm("localhost", 1759);

  ytm.AddSong("Champion Cynthia Battle Remix", "tuADSGLjLB8", "GlitchxCity", "");
  ytm.AddSong("Hoenn Victory Road Remix", "iuHehsaC-MM", "GlitchxCity", "");
  ytm.AddSong("Mystery Dungeon Aegis Cave Remix", "kLWDM4KEErE", "GlitchxCity", "");
  ytm.AddSong("Lugia's Song Remix", "tW82Ftg_5iw", "GlitchxCity", "");
  ytm.AddSong("Sinnoh Trainer Battle Remix", "ml6ajlfDyLE", "GlitchxCity", "");
  ytm.AddPlaylist("GlitchxCity", {0,1,2,3,4});

  std::string directory;
  cout << ytm.GetDirectory(&directory) << endl;
  cout << "OUTPUT: " << directory << endl;
  */

  /*
  YTMClient::YTMClient ytm("localhost", 1759);
  for (int i = 0; i < 1000; ++i) {
    ytm.AddSong(std::to_string(i),std::to_string(i),std::to_string(i),std::to_string(i));
  }
  */

  YTMClient::ClientApp app(new YTMClient::YTMClient("localhost",1759));
  app.Run();
}
