#include "Datastore.h"

#include <fstream>
#include <iostream>
#include <stdlib.h>

namespace ytmusic {
namespace ytmusd {

int kErrorCouldNotOpenFile = 1;
int kErrorCouldNotParseFileToProto = 2;

Datastore::Datastore(std::string path) {
  this->path = path;
  this->Restore();
}

void Datastore::AddSong(std::string title, std::string yt_hash,
                        std::string artist, std::string album) {
  this->lock.lock();
  ::ytmusic::Datastore_proto_Song* song = this->datastore.add_song();
  song->set_title(title);
  song->set_yt_hash(yt_hash);
  if (!artist.empty()) {
    song->set_artist(artist);
  }
  if (!album.empty()) {
    song->set_album(album);
  }
  this->Commit();
  this->lock.unlock();
}

void Datastore::AddPlaylist(std::string name, std::vector<int> song_keys) {
  this->lock.lock();
  ::ytmusic::Datastore_proto_Playlist* playlist =
      this->datastore.add_playlist();
  playlist->set_name(name);
  for (int song_key : song_keys) {
    playlist->add_song_key(song_key);
  }
  this->Commit();
  this->lock.unlock();
}

int Datastore::Restore() {
  std::ifstream file(this->path, std::ios::in | std::ios::binary);
  if (!file) {
    return kErrorCouldNotOpenFile;
  }
  if (!this->datastore.ParseFromIstream(&file)) {
    return kErrorCouldNotParseFileToProto;
  }
  file.close();
  return 0;
}

int Datastore::Commit() {
  std::ofstream file(this->path,
                     std::ios::out | std::ios::trunc | std::ios::binary);
  if (!file) {
    return kErrorCouldNotOpenFile;
  }
  file << this->datastore.SerializeToOstream(&file);
  file.close();
  return 0;
}

}  // namespace ytmusd
}  // namespace ytmusic
