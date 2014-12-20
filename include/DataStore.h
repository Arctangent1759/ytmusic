#include <string>

#ifndef DATASTORE_H
#define DATASTORE_H

namespace ytmusic {
namespace ytmusd {

class SongEntry {
  public:
    SongEntry();
    void SetTitle(std::string title);
    void SetYTHash(std::string title);
    void SetArtist(std::string title);
    void SetAlbum(std::string title);
    std::string GetTitle(std::string title);
    std::string GetYTHash(std::string title);
    std::string GetYTHash(std::string title);
  private:
    std::string title;
    std::string yt_hash;
    std::string artist;
    std::string album;
}

class DataStore {
  public:
    DataStore(std::string config_dir);
    PutSong(SongEntry* song);
  private:
    std::string config_dir;
};

}  // namespace ytmusd
}  // namespace ytmusic


#endif  // YTMUSDATASTORE_H
