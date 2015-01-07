#include <VMenu.h>
#include <ncurses.h>
#include <iostream>
using std::cout;
using std::endl;

namespace YTMClient {

void fill_line(WINDOW* w);

VMenu::VMenu() {}
VMenu::VMenu(int x, int y, int width, int height,
             std::vector<std::string> options) {
  this->SetPos(x, y, width, height);
  this->SetOptions(options);
}
void VMenu::SetOptions(std::vector<std::string> options) {
  this->options = options;
}
void VMenu::SetPos(int x, int y, int width, int height) {
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
}
void VMenu::SetHasNextMenu(bool v) {
  this->has_next_menu = v;
}
void VMenu::Draw(bool active, int selection) {
  WINDOW* win = newwin(this->height, this->width, this->y, this->x);

  if (active) {
    wattrset(win, COLOR_PAIR(YtmusColor::TITLE) | A_BOLD | A_UNDERLINE);
  } else {
    wattrset(win, COLOR_PAIR(YtmusColor::TITLE | A_BOLD | A_UNDERLINE));
  }
  mvwaddstr(win, 0, 0, this->title.c_str());
  wattrset(win, COLOR_PAIR(YtmusColor::TITLE));
  fill_line(win);

  for (int i = 0; i < this->options.size(); ++i) {
    wattrset(win, COLOR_PAIR(YtmusColor::NORMAL));
    if (i == selection) {
      if (active) {
        wattrset(win, COLOR_PAIR(YtmusColor::SELECTED) | A_BOLD);
      } else {
        wattrset(win, COLOR_PAIR(YtmusColor::SELECTED));
      }
    }
    if (this->options[i].empty()) {
      mvwaddstr(win, i+1, 0, "<Unknown>");
    } else {
      mvwaddstr(win, i+1, 0, this->options[i].c_str());
    }
    fill_line(win);
  }
  wattrset(win, COLOR_PAIR(YtmusColor::DIVIDER));
  mvwvline(win, 0, width - 1, '|', height);
  wrefresh(win);
  delwin(win);
  this->is_clear = false;
}
void VMenu::Clear() {
  if (!this->is_clear) {
    WINDOW* win = newwin(this->height, this->width, this->y, this->x);
    wclear(win);
    wrefresh(win);
    delwin(win);
    this->is_clear = true;
  }
}

void fill_line(WINDOW* w) {
  int x = getcurx(w);
  int max_x = getmaxx(w);
  for (int i = 0; i < max_x - x; ++i) {
    waddch(w, ' ');
  }
}

int VMenu::GetMaxSelection() {
  return this->options.size();
}

std::string VMenu::GetOption(int selection) {
  if (selection < 0 || selection >= this->options.size() || this->options[selection].empty()) {
    return "";
  }
  return this->options[selection];
}

bool VMenu::HasNextMenu() {
  return this->has_next_menu;
}

void VMenu::SetTitle(std::string v) {
  this->title = v;
}

}  // namespace YTMClient
