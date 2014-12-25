#include "Ytmusd.h"

#include <re2/re2.h>

#include "URLDecoder.h"
#include "Ytmusd.h"

namespace ytmusic {
namespace ytmusd {

const std::string kYoutubeUrlTemplate = "https://www.youtube.com/watch?v=";

Ytmusd::Ytmusd(std::string datastore_path) {
  this->datastore = std::unique_ptr<Datastore>(new Datastore(datastore_path));
}
void Ytmusd::Play(int key) {
  this->Stop();
  this->queue.push_back(key);
  this->now_playing = 0;
  this->Update();
}
void Ytmusd::Play(std::vector<int> keys) {
  this->Stop();
  for (int key : keys) {
    this->queue.push_back(key);
  }
  this->now_playing = 0;
  this->Update();
}
void Ytmusd::Enqueue(int key) {
  this->queue.push_back(key);
  if (this->now_playing == -1) {
    this->now_playing = 0;
  }
  std::string song_url =
      kYoutubeUrlTemplate + this->datastore->GetSong(key)->yt_hash();
  this->audio.Enqueue(song_url);
}
void Ytmusd::Next() { this->audio.Skip(); }
void Ytmusd::Prev() {
  if (this->now_playing > 0) {
    this->now_playing = this->GetNowPlayingIndex() - 1;
  }
  this->Update();
}
void Ytmusd::Stop() {
  this->queue.clear();
  this->audio.ClearQueue();
  this->audio.Skip();
  this->now_playing = -1;
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

std::string Error(std::string message) { return "ERROR: " + message; }
std::string Success(std::string message) { return "SUCCESS: " + message; }

void YtmusdServer::InitHandlers() {
  dispatcher->RegisterHandler(
      "AddSong\\( *\"(.+)\" *, *\"(.+)\" *, *\"(.*)\" *, *(.*) *\\)",
      [this](std::string pattern, std::string request) {
        std::string title, yt_hash, artist, album;
        if (!RE2::FullMatch(request, pattern, &title, &yt_hash, &artist,
                            &album)) {
          return Error("Invalid request format.");
        }
        ::ytmusic::util::Status status = this->ytmusd->GetDatastore()->AddSong(
            title, yt_hash, artist, album);
        if (!status) {
          return status.GetMessage();
        }
        return Success("");
      });
  dispatcher->RegisterHandler("PlaySong\\((\\d+)\\)",
                              [this](std::string pattern, std::string request) {
    int key;
    if (!RE2::FullMatch(request, pattern, &key)) {
      return Error("Invalid request format.");
    }
    this->ytmusd->Play(key);
    return Success("");
  });
}

}  // namespace ytmusd
}  // namespace ytmusic
