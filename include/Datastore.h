#ifndef DATASTORE_H
#define DATASTORE_H

#include <mutex>
#include <string>
#include <vector>

#include "proto/Datastore.pb.h"
#include "Status.h"

namespace ytmusic {
namespace ytmusd {

class Datastore {
 public:
  Datastore(std::string path);
  ::ytmusic::util::Status AddSong(std::string title, std::string yt_hash,
                                  std::string artist, std::string album);
  int NumSongs();
  ::ytmusic::Datastore_proto_Song* GetSong(int key);
  ::google::protobuf::RepeatedPtrField<ytmusic::Datastore_proto_Song>
      GetSongs();
  ::ytmusic::util::Status SetSongTitle(int key, std::string title);
  ::ytmusic::util::Status SetSongYTHash(int key, std::string yt_hash);
  ::ytmusic::util::Status SetSongArtist(int key, std::string artist);
  ::ytmusic::util::Status SetSongAlbum(int key, std::string album);
  ::ytmusic::util::Status DelSong(int key);
  ::ytmusic::util::Status AddPlaylist(std::string name,
                                      std::vector<int> song_keys);
  int NumPlaylists();
  ::ytmusic::Datastore_proto_Playlist* GetPlaylist(int key);
  ::google::protobuf::RepeatedPtrField<ytmusic::Datastore_proto_Playlist>
      GetPlaylists();
  ::ytmusic::util::Status SetPlaylistName(int key, std::string name);
  ::ytmusic::util::Status SetPlaylistSongKeys(int key,
                                              std::vector<int> song_keys);
  ::ytmusic::util::Status DelPlaylist(int key);

 private:
  std::mutex lock;
  int SongIndexOfKey(int key);
  int PlaylistIndexOfKey(int key);
  ::ytmusic::util::Status ValidateSong(int key, std::string title,
                                       std::string yt_hash, std::string artist,
                                       std::string album);
  ::ytmusic::util::Status ValidatePlaylist(int key, std::string name,
                                           std::vector<int> song_keys);
  ::ytmusic::util::Status Restore();
  ::ytmusic::util::Status Commit();
  std::string path;
  ::ytmusic::Datastore_proto datastore;
};

}  // namespace ytmusd
}  // namespace ytmusic

#endif  // YTMUSDATASTORE_H
