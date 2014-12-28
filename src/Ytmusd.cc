#include "Ytmusd.h"

#include <sstream>
#include <re2/re2.h>
#include <iostream>
using namespace std;

#include "URLDecoder.h"
#include "Ytmusd.h"

namespace ytmusic {
namespace ytmusd {

const std::string kYoutubeUrlTemplate = "https://www.youtube.com/watch?v=";

Ytmusd::Ytmusd(std::string datastore_path) {
  this->datastore = std::unique_ptr<Datastore>(new Datastore(datastore_path));
}
::ytmusic::util::Status Ytmusd::Play(int key) {
  if (!this->datastore->GetSong(key)) {
    return util::Status("Song key " + std::to_string(key) + " does not exist.");
  }
  this->Stop();
  this->queue.push_back(key);
  this->now_playing = 0;
  this->Update();
  return util::Status();
}
::ytmusic::util::Status Ytmusd::Play(std::vector<int> keys) {
  for (auto key : keys) {
    if (!this->datastore->GetSong(key)) {
      return util::Status("Song key " + std::to_string(key) +
                          " does not exist.");
    }
  }
  this->Stop();
  for (int key : keys) {
    this->queue.push_back(key);
  }
  this->now_playing = 0;
  this->Update();
  return util::Status();
}
::ytmusic::util::Status Ytmusd::PlayPlaylist(int key) {
  auto playlist = this->datastore->GetPlaylist(key);
  if (!playlist) {
    return ::ytmusic::util::Status("Playlist does not exist.");
  }
  std::vector<int> keys;
  for (auto key : playlist->song_key()) {
    keys.push_back(key);
  }
  return this->Play(keys);
}
::ytmusic::util::Status Ytmusd::Enqueue(int key) {
  if (!this->datastore->GetSong(key)) {
    return util::Status("Song key " + std::to_string(key) + " does not exist.");
  }
  this->queue.push_back(key);
  if (this->now_playing == -1) {
    this->now_playing = 0;
  }
  std::string song_url =
      kYoutubeUrlTemplate + this->datastore->GetSong(key)->yt_hash();
  this->audio.Enqueue(song_url);
  return util::Status();
}
::ytmusic::util::Status Ytmusd::Next() {
  this->audio.Skip();
  return util::Status();
}
::ytmusic::util::Status Ytmusd::Prev() {
  if (this->now_playing > 0) {
    this->now_playing = this->GetNowPlayingIndex() - 1;
  }
  this->Update();
  return util::Status();
}
::ytmusic::util::Status Ytmusd::Pause() {
  this->audio.Pause();
  return util::Status();
}
::ytmusic::util::Status Ytmusd::Stop() {
  this->queue.clear();
  this->audio.ClearQueue();
  this->audio.Skip();
  this->now_playing = -1;
  return util::Status();
}
std::string Ytmusd::GetDirectory() {
  std::stringstream directory;
  directory << "{\"songs\":[";
  for (auto song : this->datastore->GetSongs()) {
    std::stringstream song_entry;
    song_entry << "{\"primary_key\":" << song.primary_key()
               << ",\"title\":\"" << song.title() << "\",\"yt_hash\":\""
               << song.yt_hash() << "\"";
    if (song.has_artist()) {
      song_entry << ",\"artist\":\"" << song.artist() << "\"";
    }
    if (song.has_album()) {
      song_entry << ",album:\"" << song.album() << "\"";
    }
    song_entry << "}";
    directory << song_entry.str() << ",";
  }
  directory << "],";
  directory << "\"playlists\":[";
  for (auto playlist : this->datastore->GetPlaylists()) {
    std::stringstream playlist_entry;
    playlist_entry << "{";
    playlist_entry << "\"primary_key\":\"" << playlist.primary_key() << "\"";
    playlist_entry << "\"name\":\"" << playlist.name() << "\"";
    playlist_entry << "\"songs\":[";
    for (int i : playlist.song_key()) {
      playlist_entry << i << ",";
    }
    playlist_entry << "]";
    playlist_entry << "},";
    directory << playlist_entry.str() << ",";
  }
  directory << "]}";
  return directory.str();
}
Datastore* Ytmusd::GetDatastore() { return this->datastore.get(); }
int Ytmusd::GetNowPlayingIndex() {
  return this->now_playing + this->audio.GetOffset();
}
void Ytmusd::Update() {
  for (int i = this->now_playing; i < this->queue.size(); ++i) {
    auto song = this->datastore->GetSong(this->queue[i]);
    std::string song_url = kYoutubeUrlTemplate + song->yt_hash();
    this->audio.Enqueue(song_url);
  }
}

YtmusdServer::YtmusdServer(int port, int backlog, Ytmusd* ytmusd) {
  this->dispatcher = new ::ytmusic::util::RequestDispatcher;
  this->ytmusd = std::unique_ptr<Ytmusd>(ytmusd);
  this->server = std::unique_ptr<::ytmusic::util::SocketServer>(
      new ::ytmusic::util::SocketServer(port, backlog, this->dispatcher));
  this->InitHandlers();
}
void YtmusdServer::Start() { this->server->Start(); }

std::string Error(std::string message) {
  return "{error:true, message:\"" + message + "\"}";
}
std::string Ack(std::string message) {
  return "{error:false, message:\"" + message + "\"}";
}

void YtmusdServer::InitHandlers() {
  dispatcher->RegisterHandler("PlaySong\\((\\d+)\\)",
                              [this](std::string pattern, std::string request) {
    int key;
    if (!RE2::FullMatch(request, pattern, &key)) {
      return Error("Invalid request format.");
    }
    ::ytmusic::util::Status status = this->ytmusd->Play(key);
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("PlaySongs\\(([0-9 ,]+)\\)",
                              [this](std::string pattern, std::string request) {
    re2::StringPiece arg_string;
    if (!RE2::FullMatch(request, pattern, &arg_string)) {
      return Error("Invalid request format.");
    }
    int arg;
    std::vector<int> keys;
    while (RE2::Consume(&arg_string, " *(\\d+),? *", &arg)) {
      keys.push_back(arg);
    }
    ::ytmusic::util::Status status = this->ytmusd->Play(keys);
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("PlayPlaylist\\((\\d+)\\)",
                              [this](std::string pattern, std::string request) {
    int key;
    if (!RE2::FullMatch(request, pattern, &key)) {
      return Error("Invalid request format.");
    }
    ::ytmusic::util::Status status = this->ytmusd->PlayPlaylist(key);
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("Pause\\(\\)",
                              [this](std::string pattern, std::string request) {
    ::ytmusic::util::Status status = this->ytmusd->Pause();
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("Stop\\(\\)",
                              [this](std::string pattern, std::string request) {
    ::ytmusic::util::Status status = this->ytmusd->Stop();
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("Next\\(\\)",
                              [this](std::string pattern, std::string request) {
    ::ytmusic::util::Status status = this->ytmusd->Next();
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("Prev\\(\\)",
                              [this](std::string pattern, std::string request) {
    ::ytmusic::util::Status status = this->ytmusd->Prev();
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("Enqueue\\((\\d+)\\)",
                              [this](std::string pattern, std::string request) {
    int key;
    if (!RE2::FullMatch(request, pattern, &key)) {
      return Error("Invalid request format.");
    }
    ::ytmusic::util::Status status = this->ytmusd->Enqueue(key);
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler(
      "AddSong\\( *\"(.+)\" *, *\"(.+)\" *, *\"(.*)\" *, *\"(.*)\" *\\)",
      [this](std::string pattern, std::string request) {
        std::string title, yt_hash, artist, album;
        if (!RE2::FullMatch(request, pattern, &title, &yt_hash, &artist,
                            &album)) {
          return Error("Invalid request format.");
        }
        ::ytmusic::util::Status status = this->ytmusd->GetDatastore()->AddSong(
            title, yt_hash, artist, album);
        if (!status) {
          return Error(status.GetMessage());
        }
        return Ack("");
      });
  dispatcher->RegisterHandler("SetSongTitle\\((\\d+), ?\"(.+)\"\\)",
                              [this](std::string pattern, std::string request) {
    int key;
    std::string value;
    if (!RE2::FullMatch(request, pattern, &key, &value)) {
      return Error("Invalid request format.");
    }
    ::ytmusic::util::Status status =
        this->ytmusd->GetDatastore()->SetSongTitle(key, value);
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("SetSongYTHash\\((\\d+), ?\"(.+)\"\\)",
                              [this](std::string pattern, std::string request) {
    int key;
    std::string value;
    if (!RE2::FullMatch(request, pattern, &key, &value)) {
      return Error("Invalid request format.");
    }
    ::ytmusic::util::Status status =
        this->ytmusd->GetDatastore()->SetSongYTHash(key, value);
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("SetSongArtist\\((\\d+), ?\"(.+)\"\\)",
                              [this](std::string pattern, std::string request) {
    int key;
    std::string value;
    if (!RE2::FullMatch(request, pattern, &key, &value)) {
      return Error("Invalid request format.");
    }
    ::ytmusic::util::Status status =
        this->ytmusd->GetDatastore()->SetSongArtist(key, value);
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("SetSongAlbum\\((\\d+), ?\"(.+)\"\\)",
                              [this](std::string pattern, std::string request) {
    int key;
    std::string value;
    if (!RE2::FullMatch(request, pattern, &key, &value)) {
      return Error("Invalid request format.");
    }
    ::ytmusic::util::Status status =
        this->ytmusd->GetDatastore()->SetSongAlbum(key, value);
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("DelSong\\((\\d+)\\)",
                              [this](std::string pattern, std::string request) {
    int key;
    if (!RE2::FullMatch(request, pattern, &key)) {
      return Error("Invalid request format.");
    }
    ::ytmusic::util::Status status = this->ytmusd->GetDatastore()->DelSong(key);
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("AddPlaylist\\(\"(.+)\", *([0-9 ,]+)\\)",
                              [this](std::string pattern, std::string request) {
    std::string name;
    re2::StringPiece arg_string;
    if (!RE2::FullMatch(request, pattern, &name, &arg_string)) {
      return Error("Invalid request format.");
    }
    int arg;
    std::vector<int> keys;
    while (RE2::Consume(&arg_string, " *(\\d+),? *", &arg)) {
      keys.push_back(arg);
    }
    ::ytmusic::util::Status status =
        this->ytmusd->GetDatastore()->AddPlaylist(name, keys);
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("SetPlaylistName\\((\\d+), ?(.+)\\)",
                              [this](std::string pattern, std::string request) {
    int key;
    std::string value;
    if (!RE2::FullMatch(request, pattern, &key, &value)) {
      return Error("Invalid request format.");
    }
    ::ytmusic::util::Status status =
        this->ytmusd->GetDatastore()->SetPlaylistName(key, value);
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("SetPlaylistSongKeys\\((\\d+), *([0-9 ,]+)\\)",
                              [this](std::string pattern, std::string request) {
    int key;
    re2::StringPiece arg_string;
    if (!RE2::FullMatch(request, pattern, &key, &arg_string)) {
      return Error("Invalid request format.");
    }
    int arg;
    std::vector<int> keys;
    while (RE2::Consume(&arg_string, " *(\\d+),? *", &arg)) {
      keys.push_back(arg);
    }
    ::ytmusic::util::Status status =
        this->ytmusd->GetDatastore()->SetPlaylistSongKeys(key, keys);
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("DelPlaylist\\((\\d+)\\)",
                              [this](std::string pattern, std::string request) {
    int key;
    if (!RE2::FullMatch(request, pattern, &key)) {
      return Error("Invalid request format.");
    }
    ::ytmusic::util::Status status =
        this->ytmusd->GetDatastore()->DelPlaylist(key);
    if (!status) {
      return Error(status.GetMessage());
    }
    return Ack("");
  });
  dispatcher->RegisterHandler("GetDirectory\\(\\)",
                              [this](std::string pattern, std::string request) {
    return this->ytmusd->GetDirectory();
  });


  /*
  ::ytmusic::util::Status DelPlaylist(int key);
  */
}

}  // namespace ytmusd
}  // namespace ytmusic
