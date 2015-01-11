#ifndef CLIENTAPP_H
#define CLIENTAPP_H

#include <memory>

#include "ClientDatastore.h"
#include "YTMClient.h"

namespace YTMClient {

enum class YtmusAction {
  NOP,
  OPEN,
  ENQUEUE,
  PLAY,
  NEW,
  NEXT,
  PREV,
  PAUSE,
  RESUME,
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
};

}  // namespace YTMClient
#endif  // CLIENTAPP_H
