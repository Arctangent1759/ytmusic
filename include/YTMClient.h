#ifndef YTMCLIENT_H
#define YTMCLIENT_H

#include "Status.h"
#include <vector>

namespace YTMClient {

class YTMClient {
 public:
  YTMClient(std::string hostname, int port);
  ytmusic::util::Status PlayPlaylist(int key);
  ytmusic::util::Status Pause();
  ytmusic::util::Status Stop();
  ytmusic::util::Status Next();
  ytmusic::util::Status Prev();
  ytmusic::util::Status Enqueue(int key);
  ytmusic::util::Status AddSong(std::string title, std::string yt_hash,
                                std::string artist, std::string album);
  ytmusic::util::Status SetSongTitle(int key, std::string val);
  ytmusic::util::Status SetSongYTHash(int key, std::string val);
  ytmusic::util::Status SetSongArtist(int key, std::string val);
  ytmusic::util::Status SetSongAlbum(int key, std::string val);
  ytmusic::util::Status DelSong(int key);
  ytmusic::util::Status AddPlaylist(std::string name,
                                    std::vector<int> song_keys);
  ytmusic::util::Status SetPlaylistName(int key, std::string val);
  ytmusic::util::Status SetPlaylistSongKeys(int key,
                                            std::vector<int> song_keys);
  ytmusic::util::Status DelPlaylist(int key);
  std::string GetDirectory();

 private:
  ytmusic::util::Status SendString(std::string s);
  ytmusic::util::Status SendString(std::string s, std::string* response);
  std::string hostname;
  int port;
};

}  // namespace YTMClient

#endif  // YTMCLIENT_H
