#include "ClientDatastore.h"

#include <fstream>
#include <iostream>
#include <streambuf>

#include "rapidjson/document.h"
#include "SongFilter.h"

namespace YTMClient {

ClientDatastore::ClientDatastore(YTMClient* client) {
  this->client = client;
}

void ClientDatastore::Refresh() {
  std::string directory_json;
  if (!this->GetDirectory(&directory_json)) {
    return;
  }

  this->title_index.clear();
  this->titles.clear();
  this->album_index.clear();
  this->albums.clear();
  this->artist_index.clear();
  this->artists.clear();
  this->playlist_name_index.clear();
  this->playlists.clear();
  this->song_list.clear();
  this->playlist_list.clear();

  rapidjson::Document directory;
  directory.Parse(directory_json.c_str());
  for (int i = 0; i < directory["songs"].Size(); ++i) {
    int key;
    std::string title, artist, album, yt_hash;
    key = directory["songs"][i]["primary_key"].GetInt();
    title = directory["songs"][i]["title"].GetString();
    yt_hash = directory["songs"][i]["yt_hash"].GetString();
    if (directory["songs"][i].HasMember("artist")) {
      artist = directory["songs"][i]["artist"].GetString();
    } else {
      artist = "";
    }
    if (directory["songs"][i].HasMember("album")) {
      album = directory["songs"][i]["album"].GetString();
    } else {
      album = "";
    }
    this->RegisterSong(key, title, yt_hash, artist, album);
  }
  for (int i = 0; i < directory["playlists"].Size(); ++i) {
    std::vector<int> songs;
    std::string name = directory["playlists"][i]["name"].GetString();
    int key = directory["playlists"][i]["primary_key"].GetInt();
    for (int j = 0; j < directory["playlists"][i]["songs"].Size(); ++j) {
      songs.push_back(directory["playlists"][i]["songs"][j].GetInt());
    }
    this->playlist_name_index[name] = this->playlist_list.size();
    this->playlist_list.push_back({name, songs, key});
  }
  for (auto i : title_index) {
    this->titles.push_back(i.first);
  }
  for (auto i : artist_index) {
    this->artists.push_back(i.first);
  }
  for (auto i : album_index) {
    this->albums.push_back(i.first);
  }
  for (auto i : playlist_name_index) {
    this->playlists.push_back(i.first);
  }
}

int ClientDatastore::GetNumSongs() { return this->titles.size(); }
std::vector<std::string> ClientDatastore::GetTitles() { return this->titles; }
std::vector<std::string> ClientDatastore::GetTitles(SongFilter& f) {
  std::vector<std::string> out;
  for (auto& i : this->titles) {
    if (f.Test(this->song_list[this->title_index[i]])) {
      out.push_back(i);
    }
  }
  return out;
}
std::vector<std::string> ClientDatastore::GetArtists() { return this->artists; }
std::vector<std::string> ClientDatastore::GetAlbums() { return this->albums; }
int ClientDatastore::GetNumPlaylists() { return this->playlists.size(); }
std::vector<std::string> ClientDatastore::GetPlaylists() {
  return this->playlists;
}
int ClientDatastore::SongFirst(SongFilter& f) {
  return this->SongAfter(-1, f);
}
int ClientDatastore::SongBefore(int index, SongFilter& f) {
  for (int i = index - 1; i >= 0; --i) {
    if (f.Test(this->song_list[this->title_index[this->titles[i]]])) {
      return i;
    }
  }
  return -1;
}
int ClientDatastore::SongAfter(int index, SongFilter& f) {
  for (int i = index + 1; i < this->titles.size(); ++i) {
    if (f.Test(this->song_list[this->title_index[this->titles[i]]])) {
      return i;
    }
  }
  return -1;
}


std::vector<std::string> ClientDatastore::GetTitlesByPlaylist(
    std::string name) {
  std::vector<std::string> out;
  for (int key : playlist_list[playlist_name_index[name]].songs) {
    out.push_back(this->song_list[key_index[key]].title);
  }
  return out;
}
std::vector<std::string> ClientDatastore::GetTitlesByArtist(
    std::string artist) {
  std::vector<std::string> out;
  for (int i : artist_index[artist]) {
    out.push_back(this->song_list[i].title);
  }
  return out;
}
std::vector<std::string> ClientDatastore::GetTitlesByAlbum(std::string album) {
  std::vector<std::string> out;
  for (int i : album_index[album]) {
    out.push_back(this->song_list[i].title);
  }
  return out;
}
bool ClientDatastore::HasSong(std::string title) {
  return this->title_index.find(title) != title_index.end();
}
song_entry ClientDatastore::GetSongInfo(std::string title) {
  return this->song_list[this->title_index[title]];
}
song_entry ClientDatastore::GetSongInfo(int key) {
  return this->song_list[this->key_index[key]];
}
playlist_entry ClientDatastore::GetPlaylistInfo(std::string name) {
  return this->playlist_list[this->playlist_name_index[name]];
}
bool ClientDatastore::GetDirectory(std::string* result) {
  return this->client->GetDirectory(result);
}

void ClientDatastore::RegisterSong(int key, std::string title, std::string yt_hash,
                                   std::string artist, std::string album) {
  this->title_index[title] = this->song_list.size();
  this->album_index[album].push_back(this->song_list.size());
  this->artist_index[artist].push_back(this->song_list.size());
  this->key_index[key] = this->song_list.size();
  this->song_list.emplace_back();
  this->song_list.back().key = key;
  this->song_list.back().title = title;
  this->song_list.back().yt_hash = yt_hash;
  this->song_list.back().artist = artist;
  this->song_list.back().album = album;
}
}
