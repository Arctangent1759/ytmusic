#include <ncurses.h>
#include <fstream>
#include <map>
#include <vector>
#include <cmath>
#include <iostream>

#include "ClientDatastore.h"
#include "VMenu.h"
#include "YTMClient.h"

using std::cout;
using std::endl;

const int kHeaderHeight = 1;
const int kFooterHeight = 1;

const float kTopMenuSize = 0.25;
const float kSelectMenuSize = 0.25;
// const float kDetailMenuSize = 0.5;

enum YtmusColor : short int {
  NORMAL = 1,
  HEADER = 2,
  SELECTED = 3,
  DIVIDER = 4,
  ENTRY_BOLD = 5,
  TITLE = 6,
};

void init();
void header(int max_x, int max_y, int ch);
void footer(int max_x, int max_y, int ch);
void content(int max_x, int max_y, int ch);
void fill_line(WINDOW* w);
void DisplayDetailPage(std::string song_name, int x, int y, int width,
                       int height);
void ClearDetailPage(int x, int y, int width, int height);

YTMClient::ClientDatastore datastore("test.json");

int main() {
  init();
  while (true) {
    datastore.Refresh();
    int max_x = getmaxx(stdscr);
    int max_y = getmaxy(stdscr);
    int ch = getch();
    header(max_x, max_y, ch);
    footer(max_x, max_y, ch);
    content(max_x, max_y, ch);
  }
  endwin();
}

void init() {
  initscr();
  curs_set(0);
  cbreak();
  noecho();
  intrflush(stdscr, false);
  keypad(stdscr, true);
  atexit([]() { endwin(); });
  start_color();
  init_pair(YtmusColor::NORMAL, COLOR_WHITE, COLOR_BLACK);
  init_pair(YtmusColor::HEADER, COLOR_WHITE, COLOR_BLUE);
  init_pair(YtmusColor::SELECTED, COLOR_WHITE, COLOR_RED);
  init_pair(YtmusColor::DIVIDER, COLOR_BLUE, COLOR_BLUE);
  init_pair(YtmusColor::ENTRY_BOLD, COLOR_GREEN, COLOR_BLACK);
  init_pair(YtmusColor::TITLE, COLOR_RED, COLOR_BLUE);
  timeout(25);
}

void header(int max_x, int max_y, int ch) {
  WINDOW* header = newwin(kHeaderHeight, max_x, 0, 0);
  wattrset(header, COLOR_PAIR(YtmusColor::HEADER) | A_BOLD | A_UNDERLINE);
  waddstr(header, "YTMClient");
  wattrset(header, COLOR_PAIR(YtmusColor::HEADER));
  fill_line(header);
  wrefresh(header);
  delwin(header);
}

void footer(int max_x, int max_y, int ch) {
  WINDOW* footer = newwin(kFooterHeight, max_x, max_y - kFooterHeight, 0);
  wrefresh(footer);
  delwin(footer);
}

void content(int max_x, int max_y, int ch) {
  static std::vector<std::string> top_options{"Songs",   "Playlists",
                                              "Artists", "Albums"};
  static int active_menu = 0;
  static std::vector<int> selection_stack{0};
  static std::vector<YTMClient::VMenu> menus(4);

  menus[0].SetTitle("Collections");
  menus[0].SetPos(0, kHeaderHeight, floor(max_x * kTopMenuSize),
                  max_y - kHeaderHeight - kFooterHeight);
  menus[0].SetOptions(top_options);
  menus[0].Draw(active_menu == 0, selection_stack[0]);
  menus[0].SetHasNextMenu(true);

  switch (selection_stack[0]) {
    case 0:
      menus[1].SetTitle("Songs");
      menus[1].SetPos(floor(max_x * kTopMenuSize), kHeaderHeight,
                      floor(max_x * kSelectMenuSize),
                      max_y - kHeaderHeight - kFooterHeight);
      menus[1].SetOptions(datastore.GetTitles());
      menus[1]
          .Draw(active_menu == 1, active_menu >= 1 ? selection_stack.at(1) : 0);
      menus[1].SetHasNextMenu(false);
      if (active_menu >= 1) {
        DisplayDetailPage(
            menus[1].GetOption(selection_stack[1]),
            max_x * kTopMenuSize + max_x * kSelectMenuSize, kHeaderHeight,
            max_x - (max_x * kTopMenuSize + max_x * kSelectMenuSize),
            max_y - kHeaderHeight - kFooterHeight);
      } else {
        ClearDetailPage(
            max_x * kTopMenuSize + max_x * kSelectMenuSize, kHeaderHeight,
            max_x - (max_x * kTopMenuSize + max_x * kSelectMenuSize),
            max_y - kHeaderHeight - kFooterHeight);
      }

      break;
    case 1:
      menus[1].SetTitle("Playlists");
      menus[1].SetPos(floor(max_x * kTopMenuSize), kHeaderHeight,
                      floor(max_x * kSelectMenuSize),
                      max_y - kHeaderHeight - kFooterHeight);
      menus[1].SetOptions(datastore.GetPlaylists());
      menus[1]
          .Draw(active_menu == 1, active_menu >= 1 ? selection_stack.at(1) : 0);
      menus[1].SetHasNextMenu(true);
      menus[2]
          .SetPos(floor(max_x * kTopMenuSize) + floor(max_x * kSelectMenuSize),
                  kHeaderHeight, max_x * kSelectMenuSize,
                  max_y - kHeaderHeight - kFooterHeight);
      if (active_menu >= 1) {
        menus[2].SetTitle("Songs in " +
                          (menus[1].GetOption(selection_stack[1]).empty()
                               ? "<Unknown>"
                               : menus[1].GetOption(selection_stack[1])));
        menus[2].SetOptions(datastore.GetTitlesByPlaylist(
            menus[1].GetOption(selection_stack[1])));
        menus[2].Draw(active_menu == 2,
                      active_menu >= 2 ? selection_stack.at(2) : 0);
        menus[2].SetHasNextMenu(false);
        if (active_menu >= 2) {
          DisplayDetailPage(
              menus[2].GetOption(selection_stack[2]),
              max_x * kTopMenuSize + 2 * max_x * kSelectMenuSize, kHeaderHeight,
              max_x - (max_x * kTopMenuSize + 2 * max_x * kSelectMenuSize),
              max_y - kHeaderHeight - kFooterHeight);
        } else {
          ClearDetailPage(
              max_x * kTopMenuSize + 2 * max_x * kSelectMenuSize, kHeaderHeight,
              max_x - (max_x * kTopMenuSize + 2 * max_x * kSelectMenuSize),
              max_y - kHeaderHeight - kFooterHeight);
        }
      } else {
        menus[2].Clear();
      }
      break;
    case 2:
      menus[1].SetTitle("Artists");
      menus[1].SetPos(floor(max_x * kTopMenuSize), kHeaderHeight,
                      floor(max_x * kSelectMenuSize),
                      max_y - kHeaderHeight - kFooterHeight);
      menus[1].SetOptions(datastore.GetArtists());
      menus[1]
          .Draw(active_menu == 1, active_menu >= 1 ? selection_stack.at(1) : 0);
      menus[1].SetHasNextMenu(true);
      menus[2]
          .SetPos(floor(max_x * kTopMenuSize) + floor(max_x * kSelectMenuSize),
                  kHeaderHeight, floor(max_x * kSelectMenuSize),
                  max_y - kHeaderHeight - kFooterHeight);
      if (active_menu >= 1) {
        menus[2].SetTitle("Songs in " +
                          (menus[1].GetOption(selection_stack[1]).empty()
                               ? "<Unknown>"
                               : menus[1].GetOption(selection_stack[1])));
        menus[2].SetOptions(datastore.GetTitlesByArtist(
            menus[1].GetOption(selection_stack[1])));
        menus[2].Draw(active_menu == 2,
                      active_menu >= 2 ? selection_stack.at(2) : 0);
        menus[2].SetHasNextMenu(false);
        if (active_menu >= 2) {
          DisplayDetailPage(
              menus[2].GetOption(selection_stack[2]),
              max_x * kTopMenuSize + 2 * max_x * kSelectMenuSize, kHeaderHeight,
              max_x - (max_x * kTopMenuSize + 2 * max_x * kSelectMenuSize),
              max_y - kHeaderHeight - kFooterHeight);
        } else {
          ClearDetailPage(
              max_x * kTopMenuSize + 2 * max_x * kSelectMenuSize, kHeaderHeight,
              max_x - (max_x * kTopMenuSize + 2 * max_x * kSelectMenuSize),
              max_y - kHeaderHeight - kFooterHeight);
        }
      } else {
        menus[2].Clear();
      }
      break;
    case 3:
      menus[1].SetTitle("Albums");
      menus[1].SetPos(floor(max_x * kTopMenuSize), kHeaderHeight,
                      floor(max_x * kSelectMenuSize),
                      max_y - kHeaderHeight - kFooterHeight);
      menus[1].SetOptions(datastore.GetAlbums());
      menus[1]
          .Draw(active_menu == 1, active_menu >= 1 ? selection_stack.at(1) : 0);
      menus[1].SetHasNextMenu(true);
      if (active_menu >= 1) {
        menus[2].SetTitle("Songs in " +
                          (menus[1].GetOption(selection_stack[1]).empty()
                               ? "<Unknown>"
                               : menus[1].GetOption(selection_stack[1])));
        menus[2].SetPos(
            floor(max_x * kTopMenuSize) + floor(max_x * kSelectMenuSize),
            kHeaderHeight, floor(max_x * kSelectMenuSize),
            max_y - kHeaderHeight - kFooterHeight);
        menus[2].SetOptions(
            datastore.GetTitlesByAlbum(menus[1].GetOption(selection_stack[1])));
        menus[2].Draw(active_menu == 2,
                      active_menu >= 2 ? selection_stack.at(2) : 0);
        menus[2].SetHasNextMenu(false);
        if (active_menu >= 2) {
          DisplayDetailPage(
              menus[2].GetOption(selection_stack[2]),
              max_x * kTopMenuSize + 2 * max_x * kSelectMenuSize, kHeaderHeight,
              max_x - (max_x * kTopMenuSize + 2 * max_x * kSelectMenuSize),
              max_y - kHeaderHeight - kFooterHeight);
        } else {
          ClearDetailPage(
              max_x * kTopMenuSize + 2 * max_x * kSelectMenuSize, kHeaderHeight,
              max_x - (max_x * kTopMenuSize + 2 * max_x * kSelectMenuSize),
              max_y - kHeaderHeight - kFooterHeight);
        }
      } else {
        menus[2].Clear();
      }
      break;
  }

  switch (ch) {
    case 'j':
      if (selection_stack.back() < menus[active_menu].GetMaxSelection() - 1) {
        ++selection_stack.back();
      }
      break;
    case 'k':
      if (selection_stack.back() > 0) {
        --selection_stack.back();
      }
      break;
    case 'l':
      if (active_menu < menus.size() - 1 && menus[active_menu].HasNextMenu()) {
        active_menu += 1;
        selection_stack.push_back(0);
      }
      break;
    case 'h':
      if (active_menu > 0) {
        active_menu -= 1;
        selection_stack.pop_back();
      }
      break;
    case 'q':
      exit(0);
      break;
  }
}

void fill_line(WINDOW* w) {
  int x = getcurx(w);
  int max_x = getmaxx(w);
  for (int i = 0; i < max_x - x; ++i) {
    waddch(w, ' ');
  }
}

bool is_clear = true;

void DisplayDetailPage(std::string song_name, int x, int y, int width,
                       int height) {
  if (song_name.empty() || !datastore.HasSong(song_name)) {
    return;
  }
  YTMClient::song_entry song = datastore.GetSongInfo(song_name);
  is_clear = false;
  WINDOW* win = newwin(height, width, y, x);
  wattrset(win, COLOR_PAIR(YtmusColor::ENTRY_BOLD) | A_BOLD | A_UNDERLINE);
  mvwaddstr(win, 0, 0, "Title:");
  wattrset(win, COLOR_PAIR(YtmusColor::NORMAL));
  wprintw(win, " %s", song.title.c_str());
  wattrset(win, COLOR_PAIR(YtmusColor::ENTRY_BOLD) | A_BOLD | A_UNDERLINE);
  mvwaddstr(win, 1, 0, "Artist:");
  wattrset(win, COLOR_PAIR(YtmusColor::NORMAL));
  wprintw(win, " %s", song.artist.c_str());
  wattrset(win, COLOR_PAIR(YtmusColor::ENTRY_BOLD) | A_BOLD | A_UNDERLINE);
  mvwaddstr(win, 2, 0, "Album:");
  wattrset(win, COLOR_PAIR(YtmusColor::NORMAL));
  wprintw(win, " %s", song.album.c_str());
  wrefresh(win);
  delwin(win);
}

void ClearDetailPage(int x, int y, int width, int height) {
  if (!is_clear) {
    WINDOW* win = newwin(height, width, y, x);
    wclear(win);
    wrefresh(win);
    delwin(win);
    is_clear = true;
  }
}
