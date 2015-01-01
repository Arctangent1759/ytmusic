#include "ClientDatastore.h"

#include <fstream>
#include <iostream>
#include <streambuf>
using std::cout;
using std::endl;

#include "rapidjson/document.h"

namespace YTMClient {

ClientDatastore::ClientDatastore(std::string server_url) {
  this->server_url = server_url;
}

void ClientDatastore::Refresh() {
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
  directory.Parse(this->GetDirectory().c_str());
  for (int i = 0; i < directory["songs"].Size(); ++i) {
    int key;
    std::string title, artist, album;
    song_list.emplace_back();
    key = directory["songs"][i]["primary_key"].GetInt();
    title = directory["songs"][i]["title"].GetString();
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
    this->RegisterSong(key, title, artist, album);
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

std::vector<std::string> ClientDatastore::GetTitles() { return this->titles; }
std::vector<std::string> ClientDatastore::GetArtists() { return this->artists; }
std::vector<std::string> ClientDatastore::GetAlbums() { return this->albums; }
std::vector<std::string> ClientDatastore::GetPlaylists() {
  return this->playlists;
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

std::string ClientDatastore::GetDirectory() {
  // TODO(arctangent1759): implement.
  std::ifstream f(this->server_url);
  std::string out((std::istreambuf_iterator<char>(f)),
                  std::istreambuf_iterator<char>());
  return out;
}

void ClientDatastore::RegisterSong(int key, std::string title,
                                   std::string artist, std::string album) {
  this->title_index[title] = this->song_list.size();
  this->album_index[album].push_back(this->song_list.size());
  this->artist_index[artist].push_back(this->song_list.size());
  this->key_index[key] = this->song_list.size();
  this->song_list.emplace_back();
  this->song_list.back().key = key;
  this->song_list.back().title = title;
  this->song_list.back().artist = artist;
  this->song_list.back().album = album;
}
}
