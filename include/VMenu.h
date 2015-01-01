#ifndef VMENU_H
#define VMENU_H

#include <vector>
#include <string>

namespace YTMClient {

enum YtmusColor : short int {
  NORMAL = 1,
  HEADER = 2,
  SELECTED = 3,
  DIVIDER = 4,
  ENTRY_BOLD = 5,
  TITLE = 6,
};

class VMenu {
 public:
  VMenu();
  VMenu(int x, int y, int width, int height, std::vector<std::string> options);
  void SetOptions(std::vector<std::string> options);
  void SetPos(int x, int y, int width, int height);
  void SetHasNextMenu(bool v);
  void SetTitle(std::string v);
  void Draw(bool active, int selection);
  void Clear();
  int GetMaxSelection();
  std::string GetOption(int selection);
  bool HasNextMenu();

 private:
  std::string title;
  std::vector<std::string> options;
  int width;
  int height;
  int x;
  int y;
  bool has_next_menu;
  bool is_clear;
};

}  // namespace YTMClient

#endif  // VMENU_H
