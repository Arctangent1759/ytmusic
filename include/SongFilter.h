#ifndef SONGFILTER_H
#define SONGFILTER_H

#include <string>
#include <vector>

#include "ClientDatastore.h"

namespace YTMClient {
class SongFilter {
 public:
  void Clear();
  void SetFilter(const std::string &filter);
  void SetTitle(const std::string &filter);
  void SetArtist(const std::string &filter);
  void SetAlbum(const std::string &filter);
  void ParseFrom(const std::string &filter);
  bool Test(const song_entry &s);

 private:
  std::string filter;
  std::string title;
  std::string artist;
  std::string album;
  bool valid_filter;
};
}  // namespace YTMClient

#endif  // SONGFILTER_H
