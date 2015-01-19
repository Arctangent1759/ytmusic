#ifndef CLIENTAPP_H
#define CLIENTAPP_H

#include <ncurses.h>
#include <memory>

#include "ClientDatastore.h"
#include "YTMClient.h"
#include "SongFilter.h"

namespace YTMClient {

enum class YtmusAction {
  NOP,
  OPEN,
  ENQUEUE,
  PLAY,
  NEW,
  NEXT,
  PREV,
  TOGGLE_PAUSE,
  STOP,
};

class ClientApp {
 public:
  ClientApp(YTMClient* client);
  void Run();

 private:
  void ControlsUpdate(char ch);
  void ParamsUpdate();
  void Header();
  void Footer();
  void Content();
  std::string ReadFilter();
  song_entry EditMenu(WINDOW* edit_window, song_entry e);
  std::unique_ptr<ClientDatastore> datastore;
  std::unique_ptr<YTMClient> client;
  int max_x;
  int max_y;
  static constexpr int max_menu = 2;
  int menu_num;
  int selection_base[max_menu];
  int selection[max_menu];
  int state[max_menu];
  int max_selection[max_menu];
  YtmusAction action;
  int counter;
  SongFilter filter;
};

}  // namespace YTMClient
#endif  // CLIENTAPP_H
