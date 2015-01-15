#include "YTMClient.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>

#include "rapidjson/document.h"

namespace YTMClient {

YTMClient::YTMClient(std::string hostname, int port)
    : hostname(hostname), port(port) {}
ytmusic::util::Status YTMClient::PlaySong(int key) {
  return this->SendString("PlaySong(" + std::to_string(key) + ")");
}
ytmusic::util::Status YTMClient::PlaySongs(std::vector<int> keys) {
  std::string request = "PlaySongs(";
  for (int i = 0; i < keys.size(); ++i) {
    request += std::to_string(keys[i]);
    if (i != keys.size() - 1) {
      request += ", ";
    }
  }
  request += ")";
  return this->SendString(request);
}
ytmusic::util::Status YTMClient::PlayPlaylist(int key) {
  return this->SendString("PlayPlaylist(" + std::to_string(key) + ")");
}
ytmusic::util::Status YTMClient::Pause() { return this->SendString("Pause()"); }
ytmusic::util::Status YTMClient::Continue() { return this->SendString("Continue()"); }
ytmusic::util::Status YTMClient::TogglePause() { return this->SendString("TogglePause()"); }
ytmusic::util::Status YTMClient::Stop() { return this->SendString("Stop()"); }
ytmusic::util::Status YTMClient::Next() { return this->SendString("Next()"); }
ytmusic::util::Status YTMClient::Prev() { return this->SendString("Prev()"); }
ytmusic::util::Status YTMClient::Enqueue(int key) {
  return this->SendString("Enqueue(" + std::to_string(key) + ")");
}
ytmusic::util::Status YTMClient::AddSong(std::string title, std::string yt_hash,
                                         std::string artist,
                                         std::string album) {
  return this->SendString("AddSong(\"" + title + "\", \"" + yt_hash + "\", \"" +
                          artist + "\" , \"" + album + "\")");
}
ytmusic::util::Status YTMClient::SetSongTitle(int key, std::string val) {
  return this->SendString("SetSongTitle(" + std::to_string(key) + ", \"" +
                          val + "\")");
}
ytmusic::util::Status YTMClient::SetSongYTHash(int key, std::string val) {
  return this->SendString("SetSongYTHash(" + std::to_string(key) + ", \"" +
                          val + "\")");
}
ytmusic::util::Status YTMClient::SetSongArtist(int key, std::string val) {
  return this->SendString("SetSongArtist(" + std::to_string(key) + ", \"" +
                          val + "\")");
}
ytmusic::util::Status YTMClient::SetSongAlbum(int key, std::string val) {
  return this->SendString("SetSongAlbum(" + std::to_string(key) + ", \"" +
                          val + "\")");
}
ytmusic::util::Status YTMClient::DelSong(int key) {
  return this->SendString("DelSong(" + std::to_string(key) + ")");
}
ytmusic::util::Status YTMClient::AddPlaylist(std::string name,
                                             std::vector<int> song_keys) {
  std::string request = "AddPlaylist(\"" + name + "\"";
  for (auto key : song_keys) {
    request += ", " + std::to_string(key);
  }
  request += ")";
  return this->SendString(request);
}
ytmusic::util::Status YTMClient::SetPlaylistName(int key, std::string val) {
  return this->SendString("SetPlaylistName(" + std::to_string(key) +
                          ", \"" + val + "\")");
}
ytmusic::util::Status YTMClient::SetPlaylistSongKeys(
    int key, std::vector<int> song_keys) {
  std::string request = "SetPlaylistSongKeys(" + std::to_string(key);
  for (auto key : song_keys) {
    request += ", " + std::to_string(key);
  }
  request += ")";
  return this->SendString(request);
}
ytmusic::util::Status YTMClient::DelPlaylist(int key) {
  return this->SendString("DelPlaylist(" + std::to_string(key) + ")");
}
ytmusic::util::Status YTMClient::GetDirectory(std::string* response) {
  return this->SendString("GetDirectory()", response);
}
ytmusic::util::Status YTMClient::GetStatus(YtmStatus* response) {
  std::string response_json;
  ytmusic::util::Status req_status = this->SendString("GetStatus()", &response_json);
  if (!req_status) {
    return req_status;
  }
  rapidjson::Document d;
  d.Parse(response_json.c_str());
  if (!(d.HasMember("queue") && d["queue"].IsArray())) {
    return ytmusic::util::Status("Expected array \"queue\" in response json.");
  }
  for (int i = 0; i < d["queue"].Size(); ++i) {
    response->queue.push_back(d["queue"][i].GetInt());
  }
  if (!(d.HasMember("now_playing") && d["now_playing"].IsInt())) {
    return ytmusic::util::Status("Expected int \"now_playing\" in response json.");
  }
  response->now_playing = d["now_playing"].GetInt();
  if (!(d.HasMember("is_playing") && d["is_playing"].IsBool())) {
    return ytmusic::util::Status("Expected bool \"is_playing\" in response json.");
  }
  response->is_playing = d["is_playing"].GetBool();
  return req_status;
}
ytmusic::util::Status YTMClient::SendString(std::string s) {
  std::string result;
  return this->SendString(s, &result);
}

ytmusic::util::Status ResponseToStatus(std::string response) {
  rapidjson::Document doc;
  doc.Parse(response.c_str());
  if (doc.HasMember("error") && doc["error"].GetBool()) {
    if (doc.HasMember("message")) {
      return ytmusic::util::Status(doc["message"].GetString());
    } else {
      return ytmusic::util::Status(1);
    }
  }
  return ytmusic::util::Status();
}

ytmusic::util::Status YTMClient::SendString(std::string s,
                                            std::string* response) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    return ytmusic::util::Status("Failed to allocate socket.");
  }

  struct sockaddr_in serv_addr;
  struct hostent* server = gethostbyname(this->hostname.c_str());
  if (server == NULL) {
    return ytmusic::util::Status("No such host.");
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, 
       (char *)&serv_addr.sin_addr.s_addr,
       server->h_length);
  serv_addr.sin_port = htons(this->port);
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    return ytmusic::util::Status("Could not connect.");
  }
  int n = write(sockfd, (s + "\n").c_str(), s.size() + 1);
  if (n < 0) {
    return ytmusic::util::Status("Error writing to socket.");
  }
  *response = "";
  char buf = 0;
  while (true) {
    if (read(sockfd, &buf, 1) < 0) {
      return ytmusic::util::Status("Error reading from socket.");
    }
    if (buf == '\n') {
      break;
    }
    *response += &buf;
  }
  close(sockfd);
  return ResponseToStatus(*response);
}

}  // namespace YTMClient
