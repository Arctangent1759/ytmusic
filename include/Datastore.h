#ifndef DATASTORE_H
#define DATASTORE_H

#include <mutex>
#include <string>
#include <vector>

#include "proto/Datastore.pb.h"

namespace ytmusic {
namespace ytmusd {

class Datastore {
 public:
  Datastore(std::string path);
  void AddSong(std::string title, std::string yt_hash, std::string artist,
               std::string album);
  void ModifySong(int key, std::string title, std::string yt_hash,
                  std::string artist, std::string album);
  ::ytmusic::Datastore_proto_Song GetSong(int key);
  void AddPlaylist(std::string name, std::vector<int> song_keys);
  void ModifyPlaylist(int key, std::string name, std::vector<int> songs);
  ::ytmusic::Datastore_proto_Playlist GetPlaylist(int key);
  std::string ToString();

 private:
  std::mutex lock;
  int Restore();
  int Commit();
  std::string path;
  ::ytmusic::Datastore_proto datastore;
};

}  // namespace ytmusd
}  // namespace ytmusic

#endif  // YTMUSDATASTORE_H
