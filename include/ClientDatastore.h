#ifndef CLIENTDATASTORE_H
#define CLIENTDATASTORE_H

#include <map>
#include <string>
#include <vector>

#include "YTMClient.h"

namespace YTMClient {

typedef struct song_entry_t {
  std::string title;
  std::string artist;
  std::string album;
  int key;
} song_entry;

typedef struct playlist_entry_t {
  std::string name;
  std::vector<int> songs;
  int key;
} playlist_entry;

class ClientDatastore {
 public:
  ClientDatastore(YTMClient* client);
  void Refresh();
  int GetNumSongs();
  std::vector<std::string> GetTitles();
  std::vector<std::string> GetArtists();
  std::vector<std::string> GetAlbums();
  int GetNumPlaylists();
  std::vector<std::string> GetPlaylists();
  std::vector<std::string> GetTitlesByPlaylist(std::string name);
  std::vector<std::string> GetTitlesByArtist(std::string artist);
  std::vector<std::string> GetTitlesByAlbum(std::string album);
  song_entry GetSongInfo(std::string title);
  playlist_entry GetPlaylistInfo(std::string name);
  bool HasSong(std::string title);

 private:
  bool GetDirectory(std::string* result);
  void RegisterSong(int key, std::string title, std::string artist,
                    std::string album);
  YTMClient* client;
  std::vector<song_entry> song_list;
  std::vector<playlist_entry> playlist_list;
  std::map<std::string, int> title_index;
  std::map<std::string, std::vector<int>> artist_index;
  std::map<std::string, std::vector<int>> album_index;
  std::map<std::string, int> playlist_name_index;
  std::map<int, int> key_index;
  std::vector<std::string> titles;
  std::vector<std::string> artists;
  std::vector<std::string> albums;
  std::vector<std::string> playlists;
};

}  // namespace YTMClient

#endif  // CLIENTDATASTORE_H
