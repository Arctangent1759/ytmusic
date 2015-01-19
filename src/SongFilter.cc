#include "SongFilter.h"
#include <re2/re2.h>

namespace YTMClient {
void SongFilter::Clear() {
  this->valid_filter = true;
  this->filter = ".*";
  this->title = ".*";
  this->artist = ".*";
  this->album = ".*";
}
void SongFilter::SetFilter(const std::string &filter) {
  this->valid_filter = true;
  this->filter = filter;
}
void SongFilter::SetTitle(const std::string &filter) {
  this->valid_filter = false;
  this->title = filter;
}
void SongFilter::SetArtist(const std::string &filter) {
  this->valid_filter = false;
  this->artist = filter;
}
void SongFilter::SetAlbum(const std::string &filter) {
  this->valid_filter = false;
  this->album = filter;
}
void SongFilter::ParseFrom(const std::string &filter) {
  this->Clear();
  this->filter = filter;
  if (RE2::PartialMatch(filter, "\\\\t<([^>]+)>", &this->title)) {
    this->valid_filter = false;
  }
  if (RE2::PartialMatch(filter, "\\\\a<([^>]+)>", &this->artist)) {
    this->valid_filter = false;
  }
  if (RE2::PartialMatch(filter, "\\\\l<([^>]+)>", &this->album)) {
    this->valid_filter = false;
  }
}
bool SongFilter::Test(const song_entry &s) {
  if (this->valid_filter) {
    return RE2::PartialMatch(s.title, this->filter) ||
           RE2::PartialMatch(s.artist, this->filter) ||
           RE2::PartialMatch(s.album, this->filter);
  } else {
    return RE2::PartialMatch(s.title, this->title) &&
           RE2::PartialMatch(s.artist, this->artist) &&
           RE2::PartialMatch(s.album, this->album);
  }
}

}  // namespace YTMClient
