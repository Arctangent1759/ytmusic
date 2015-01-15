#include "Datastore.h"

#include <fstream>
#include <iostream>
#include <stdlib.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace ytmusic {
namespace ytmusd {

const int kErrorCouldNotOpenFile = 1;
const int kErrorCouldNotParseFileToProto = 2;

Datastore::Datastore(std::string path) {
  this->path = path;
  util::Status status = this->Restore();
  if (!status) {
    switch (status.GetCode()) {
      case kErrorCouldNotOpenFile:
        this->datastore.set_next_song_primary_key(0);
        this->datastore.set_next_playlist_primary_key(0);
        break;
      case kErrorCouldNotParseFileToProto:
        fprintf(stderr,
                "File %s is not a valid ytmusic datastore. Please fix or "
                "delete the file and try again.\n",
                path.c_str());
        exit(-1);
        break;
      default:
        fprintf(stderr,
                "An unknown error has occurred on datastore restore.\n");
        exit(-1);
    }
  }
}

::ytmusic::util::Status Datastore::AddSong(std::string title,
                                           std::string yt_hash,
                                           std::string artist,
                                           std::string album) {
  this->lock.lock();

  ::ytmusic::util::Status status =
      this->ValidateSong(-1, title, yt_hash, artist, album);
  if (!status) {
    this->lock.unlock();
    return status;
  }

  int next_primary_key = this->datastore.next_song_primary_key();
  this->datastore.set_next_song_primary_key(next_primary_key + 1);

  ::ytmusic::Datastore_proto_Song* song = this->datastore.add_song();
  song->set_primary_key(next_primary_key);
  song->set_title(title);
  song->set_yt_hash(yt_hash);
  if (!artist.empty()) {
    song->set_artist(artist);
  }
  if (!album.empty()) {
    song->set_album(album);
  }

  if (!this->Commit()) {
    fprintf(stderr, "Failed to commit changes to datastore.\n");
  }
  this->lock.unlock();
  return ::ytmusic::util::Status();
}

int Datastore::NumSongs() { return this->datastore.song_size(); }

::ytmusic::util::Status Datastore::SetSongTitle(int key, std::string title) {
  this->lock.lock();

  ::ytmusic::util::Status status = this->ValidateSong(key, title, "", "", "");
  if (!status) {
    this->lock.unlock();
    return status;
  }

  this->GetSong(key)->set_title(title);

  if (!this->Commit()) {
    fprintf(stderr, "Failed to commit changes to datastore.\n");
  }
  this->lock.unlock();
  return ::ytmusic::util::Status();
}

::ytmusic::util::Status Datastore::SetSongYTHash(int key, std::string yt_hash) {
  this->lock.lock();

  ::ytmusic::util::Status status = this->ValidateSong(key, "", yt_hash, "", "");
  if (!status) {
    this->lock.unlock();
    return status;
  }

  this->GetSong(key)->set_yt_hash(yt_hash);

  if (!this->Commit()) {
    fprintf(stderr, "Failed to commit changes to datastore.\n");
  }
  this->lock.unlock();
  return ::ytmusic::util::Status();
}
::ytmusic::util::Status Datastore::SetSongArtist(int key, std::string artist) {
  this->lock.lock();

  ::ytmusic::util::Status status = this->ValidateSong(key, "", "", artist, "");
  if (!status) {
    this->lock.unlock();
    return status;
  }

  if (artist.empty()) {
    this->GetSong(key)->clear_artist();
  } else {
    this->GetSong(key)->set_artist(artist);
  }

  if (!this->Commit()) {
    fprintf(stderr, "Failed to commit changes to datastore.\n");
  }
  this->lock.unlock();
  return ::ytmusic::util::Status();
}
::ytmusic::util::Status Datastore::SetSongAlbum(int key, std::string album) {
  this->lock.lock();

  ::ytmusic::util::Status status = this->ValidateSong(key, "", "", "", album);
  if (!status) {
    this->lock.unlock();
    return status;
  }

  if (album.empty()) {
    this->GetSong(key)->clear_album();
  } else {
    this->GetSong(key)->set_album(album);
  }

  if (!this->Commit()) {
    fprintf(stderr, "Failed to commit changes to datastore.\n");
  }
  this->lock.unlock();
  return ::ytmusic::util::Status();
}

::ytmusic::util::Status Datastore::DelSong(int key) {
  this->lock.lock();

  int index = this->SongIndexOfKey(key);
  if (index == -1) {
    this->lock.unlock();
    return ::ytmusic::util::Status(
        "Key " + std::to_string(key) +
        " does not refer to a song in the database.");
  }
  this->datastore.mutable_song()->DeleteSubrange(index, 1);

  if (!this->Commit()) {
    fprintf(stderr, "Failed to commit changes to datastore.\n");
  }
  this->lock.unlock();
  return ::ytmusic::util::Status();
}

::google::protobuf::RepeatedPtrField<ytmusic::Datastore_proto_Song>
Datastore::GetSongs() {
  return this->datastore.song();
}

::ytmusic::Datastore_proto_Song* Datastore::GetSong(int key) {
  int index = this->SongIndexOfKey(key);
  if (index == -1) {
    return NULL;
  } else {
    return this->datastore.mutable_song(index);
  }
}

::ytmusic::util::Status Datastore::AddPlaylist(std::string name,
                                               std::vector<int> song_keys) {
  this->lock.lock();

  ::ytmusic::util::Status status = this->ValidatePlaylist(-1, name, song_keys);
  if (!status) {
    this->lock.unlock();
    return status;
  }

  int next_primary_key = this->datastore.next_playlist_primary_key();
  this->datastore.set_next_playlist_primary_key(next_primary_key + 1);

  ::ytmusic::Datastore_proto_Playlist* playlist =
      this->datastore.add_playlist();
  playlist->set_primary_key(next_primary_key);
  playlist->set_name(name);
  for (int song_key : song_keys) {
    playlist->add_song_key(song_key);
  }
  if (!this->Commit()) {
    fprintf(stderr, "Failed to commit changes to datastore.\n");
  }
  this->lock.unlock();
  return ::ytmusic::util::Status();
}

int Datastore::NumPlaylists() { return this->datastore.playlist_size(); }

::google::protobuf::RepeatedPtrField<ytmusic::Datastore_proto_Playlist>
Datastore::GetPlaylists() {
  return this->datastore.playlist();
}

::ytmusic::Datastore_proto_Playlist* Datastore::GetPlaylist(int key) {
  int index = this->PlaylistIndexOfKey(key);
  if (index == -1) {
    return NULL;
  } else {
    return this->datastore.mutable_playlist(index);
  }
}

::ytmusic::util::Status Datastore::SetPlaylistName(int key, std::string name) {
  this->lock.lock();

  ::ytmusic::util::Status status =
      this->ValidatePlaylist(key, name, std::vector<int>());
  if (!status) {
    this->lock.unlock();
    return status;
  }

  this->GetPlaylist(key)->set_name(name);

  this->lock.unlock();

  return ::ytmusic::util::Status();
}

::ytmusic::util::Status Datastore::SetPlaylistSongKeys(
    int key, std::vector<int> song_keys) {
  this->lock.lock();

  ::ytmusic::util::Status status = this->ValidatePlaylist(key, "", song_keys);
  if (!status) {
    this->lock.unlock();
    return status;
  }

  ::ytmusic::Datastore_proto_Playlist* playlist = this->GetPlaylist(key);
  playlist->clear_song_key();
  for (int song_key : song_keys) {
    playlist->add_song_key(song_key);
  }

  this->lock.unlock();

  return ::ytmusic::util::Status();
}

::ytmusic::util::Status Datastore::DelPlaylist(int key) {
  this->lock.lock();

  int index = this->PlaylistIndexOfKey(key);
  if (index == -1) {
    this->lock.unlock();
    return ::ytmusic::util::Status(
        "Key " + std::to_string(key) +
        " does not refer to a playlist in the database.");
  }
  this->datastore.mutable_playlist()->DeleteSubrange(index, 1);

  if (!this->Commit()) {
    fprintf(stderr, "Failed to commit changes to datastore.\n");
  }
  this->lock.unlock();
  return ::ytmusic::util::Status();
}

std::string Datastore::ToJSON() {
  this->lock.lock();
  std::string out;
  rapidjson::Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
  rapidjson::Value song_list(rapidjson::kArrayType);
  for (auto song : this->datastore.song()) {
    rapidjson::Value val, title, yt_hash, artist, album;
    val.SetObject();
    val.AddMember("primary_key", song.primary_key(), allocator);
    title.SetString(song.title().c_str(), song.title().length(), allocator);
    val.AddMember("title", title, allocator);
    yt_hash.SetString(song.yt_hash().c_str(), song.yt_hash().length(),
                      allocator);
    val.AddMember("yt_hash", yt_hash, allocator);
    if (song.has_artist()) {
      artist.SetString(song.artist().c_str(), song.artist().size(), allocator);
      val.AddMember("artist", artist, allocator);
    }
    if (song.has_album()) {
      album.SetString(song.album().c_str(), song.album().size(), allocator);
      val.AddMember("album", album, allocator);
    }
    song_list.PushBack(val, allocator);
  }
  d.AddMember("songs", song_list, allocator);
  rapidjson::Value playlist_list(rapidjson::kArrayType);
  for (auto playlist : this->datastore.playlist()) {
    rapidjson::Value val, name;
    val.SetObject();
    val.AddMember("primary_key", playlist.primary_key(), allocator);
    name.SetString(playlist.name().c_str(), playlist.name().length(),
                   allocator);
    val.AddMember("name", name, allocator);
    rapidjson::Value song_keys(rapidjson::kArrayType);
    for (auto key : playlist.song_key()) {
      song_keys.PushBack(key, allocator);
    }
    val.AddMember("songs", song_keys, allocator);
    playlist_list.PushBack(val, allocator);
  }
  d.AddMember("playlists", playlist_list, allocator);
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  d.Accept(writer);
  out = buffer.GetString();
  lock.unlock();
  return out;
}

int Datastore::SongIndexOfKey(int key) {
  for (int i = 0; i < this->datastore.song_size(); ++i) {
    if (this->datastore.song(i).primary_key() == key) {
      return i;
    }
  }
  return -1;
}
int Datastore::PlaylistIndexOfKey(int key) {
  for (int i = 0; i < this->datastore.playlist_size(); ++i) {
    if (this->datastore.playlist(i).primary_key() == key) {
      return i;
    }
  }
  return -1;
}

::ytmusic::util::Status Datastore::ValidateSong(int key, std::string title,
                                                std::string yt_hash,
                                                std::string artist,
                                                std::string album) {
  for (auto song : this->datastore.song()) {
    if (song.primary_key() != key && (song.yt_hash() == yt_hash)) {
      return ::ytmusic::util::Status("Song with youtube hash " + yt_hash +
                                     " already exists.");
    }
  }
  return ::ytmusic::util::Status();
}
::ytmusic::util::Status Datastore::ValidatePlaylist(
    int key, std::string name, std::vector<int> song_keys) {
  for (auto playlist : this->datastore.playlist()) {
    if (playlist.primary_key() != key && (playlist.name() == name)) {
      return ::ytmusic::util::Status("Playlist with name " + name +
                                     " already exists.");
    }
  }
  for (auto song_key : song_keys) {
    if (this->SongIndexOfKey(song_key) == -1) {
      return ::ytmusic::util::Status("Invalid song key detected.");
    }
  }
  return ::ytmusic::util::Status();
}
::ytmusic::util::Status Datastore::Restore() {
  std::ifstream file;
  file.open(this->path, std::ios::in | std::ios::binary);
  if (!file) {
    return ::ytmusic::util::Status("Could not open file.",
                                   kErrorCouldNotOpenFile);
  }
  if (!this->datastore.ParseFromIstream(&file)) {
    return ::ytmusic::util::Status(
        "Could not parse ytmusic datastore from file.",
        kErrorCouldNotParseFileToProto);
  }
  file.close();
  return ::ytmusic::util::Status();
}

::ytmusic::util::Status Datastore::Commit() {
  std::ofstream file;
  file.open(this->path, std::ios::trunc | std::ios::binary);
  if (!file) {
    return ::ytmusic::util::Status("Could not open file.",
                                   kErrorCouldNotOpenFile);
  }
  this->datastore.SerializeToOstream(&file);
  file.close();
  return ::ytmusic::util::Status();
}

}  // namespace ytmusd
}  // namespace ytmusic
