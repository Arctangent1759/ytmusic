#include "ClientApp.h"

#include <ncurses.h>
#include <algorithm>

#include <iostream>
using std::cout;
using std::endl;

namespace YTMClient {

const int kHeaderHeight = 1;
const int kFooterHeight = 1;
const int kQueueLeftPadding = 2;
const int kNumQueueEntries = 7;
const std::string k25Spaces = "                         ";

void fill_line(WINDOW* w);

enum YtmusColor : short int {
  NORMAL = 1,
  HEADER = 2,
  SELECTED = 3,
  DIVIDER = 4,
  ENTRY_BOLD = 5,
  TITLE = 6,
  PLAYING = 7,
  PAUSED = 8,
};
enum Menu : int {
  SONGS_MENU = 0,
  PLAYLISTS_MENU = 1,
};

ClientApp::ClientApp(YTMClient* client) {
  this->datastore.reset(new ClientDatastore(client));
  this->client.reset(client);
  for (int i = 0; i < this->max_menu; ++i) {
    this->selection[i] = 0;
    this->selection_base[i] = 0;
    this->state[i] = 0;
  }
  this->menu_num = SONGS_MENU;
  this->action = YtmusAction::NOP;
}

void ClientApp::Run() {
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
  init_pair(YtmusColor::TITLE, COLOR_RED, COLOR_BLACK);
  init_pair(YtmusColor::PLAYING, COLOR_WHITE, COLOR_GREEN);
  init_pair(YtmusColor::PAUSED, COLOR_WHITE, COLOR_RED);
  timeout(1000);
  while (true) {
    datastore->Refresh();
    this->max_x = getmaxx(stdscr);
    this->max_y = getmaxy(stdscr);
    this->ParamsUpdate();
    this->ControlsUpdate(getch());
    this->Header();
    this->Footer();
    this->Content();
  }
  endwin();
}

void ClientApp::ParamsUpdate() {
  this->max_selection[SONGS_MENU] = this->datastore->GetNumSongs();
  this->max_selection[PLAYLISTS_MENU] = this->datastore->GetNumPlaylists();
}

void ClientApp::ControlsUpdate(char ch) {
  static YtmusAction curr_action;
  int num_rows = this->max_y - (kHeaderHeight + kFooterHeight);
  YtmusAction last_action = curr_action;
  curr_action = YtmusAction::NOP;
  switch (ch) {
    case 'j':
      if (this->selection[this->menu_num] <
          this->max_selection[this->menu_num] - 1) {
        this->selection[this->menu_num] += 1;
      }
      break;
    case 'k':
      if (this->selection[this->menu_num] > 0) {
        this->selection[this->menu_num] -= 1;
      }
      break;
    case 'l':
      this->menu_num = (this->menu_num + 1) % this->max_menu;
      break;
    case 'h':
      this->menu_num = (this->max_menu + this->menu_num - 1) % this->max_menu;
      break;
    case 'o':
      curr_action = YtmusAction::OPEN;
      break;
    case 'p':
      curr_action = YtmusAction::PLAY;
      break;
    case 'e':
      curr_action = YtmusAction::ENQUEUE;
      break;
    case 'n':
      curr_action = YtmusAction::NEW;
      break;
    case 'H':
      curr_action = YtmusAction::PREV;
      break;
    case 'L':
      curr_action = YtmusAction::NEXT;
      break;
    case 'P':
      curr_action = YtmusAction::TOGGLE_PAUSE;
      break;
    case 's':
      curr_action = YtmusAction::STOP;
      break;
    case 'q':
      exit(0);
      break;
  }
  if (curr_action != last_action) {
    this->action = curr_action;
  } else {
    this->action = YtmusAction::NOP;
  }
  if (this->selection[this->menu_num] >
      this->selection_base[this->menu_num] + num_rows - 2) {
    this->selection_base[this->menu_num] =
        this->selection[this->menu_num] - num_rows + 2;
  }
  if (this->selection[this->menu_num] < this->selection_base[this->menu_num]) {
    this->selection_base[this->menu_num] = this->selection[this->menu_num];
  }
}

void ClientApp::Header() {
  WINDOW* header = newwin(kHeaderHeight, this->max_x, 0, 0);
  wattrset(header, COLOR_PAIR(YtmusColor::HEADER) | A_BOLD | A_UNDERLINE);
  waddstr(header, "YTMClient");
  wattrset(header, COLOR_PAIR(YtmusColor::HEADER));
  fill_line(header);
  wrefresh(header);
  delwin(header);
}
void ClientApp::Footer() {
  WINDOW* footer =
      newwin(kFooterHeight, this->max_x, this->max_y - kFooterHeight, 0);
  YtmStatus status;
  if (!this->client->GetStatus(&status)) {
    wprintw(footer, "ERROR.");
    delwin(footer);
    return;
  }
  int queue_entry_width = getmaxx(footer) / kNumQueueEntries;
  int queue_right_padding = kNumQueueEntries - kQueueLeftPadding - 1;
  for (int i = 0; i < kNumQueueEntries; ++i) {
    int queue_index;
    if (status.now_playing < kQueueLeftPadding) {
      queue_index = 0 + i;
    } else if (status.now_playing + queue_right_padding >= status.queue.size()) {
      queue_index = status.queue.size() - kNumQueueEntries + i;
    } else {
      queue_index = status.now_playing + i - kQueueLeftPadding;
    }
    if (queue_index < 0 || queue_index >=  status.queue.size()) {
      continue;
    }
    if (queue_index == status.now_playing) {
      if (status.is_playing) {
        wattrset(footer, COLOR_PAIR(YtmusColor::PLAYING));
      } else {
        wattrset(footer, COLOR_PAIR(YtmusColor::PAUSED));
      }
    } else {
      wattrset(footer, COLOR_PAIR(YtmusColor::NORMAL));
    }
    wprintw(footer, "%s >",
            (this->datastore->GetSongInfo(status.queue[queue_index]).title + k25Spaces)
                .substr(0, queue_entry_width - 2)
                .c_str());
  }
  wrefresh(footer);
  delwin(footer);
}
void ClientApp::Content() {
  WINDOW* content = newwin(this->max_y - (kHeaderHeight + kFooterHeight),
                           this->max_x - 2, kHeaderHeight, 1);
  int num_rows = this->max_y - (kHeaderHeight + kFooterHeight);
  switch (this->menu_num) {
    case SONGS_MENU: {
      std::vector<std::string> titles = this->datastore->GetTitles();
      wattrset(content, COLOR_PAIR(YtmusColor::TITLE) | A_BOLD | A_UNDERLINE);
      ::mvwprintw(content, 0, 0, "Title");
      ::mvwprintw(content, 0, floor(0.5 * getmaxx(content)), "Artist");
      ::mvwprintw(content, 0, floor(0.75 * getmaxx(content)), "Album");
      song_entry selected;
      int row = 1;
      for (
          int i = this->selection_base[this->menu_num];
          i < std::min<int>(this->selection_base[this->menu_num] + num_rows - 1,
                            titles.size());
          ++i) {
        if (i == this->selection[this->menu_num]) {
          wattrset(content, COLOR_PAIR(YtmusColor::SELECTED));
          ::wmove(content, row, 0);
          fill_line(content);
          selected = this->datastore->GetSongInfo(titles[i]);
        } else {
          wattrset(content, COLOR_PAIR(YtmusColor::NORMAL));
        }
        ::mvwprintw(content, row, 0, "%s", titles[i].c_str());
        ::mvwprintw(content, row, floor(0.5 * getmaxx(content)), "%s",
                    datastore->GetSongInfo(titles[i]).artist.c_str());
        ::mvwprintw(content, row, floor(0.75 * getmaxx(content)), "%s",
                    datastore->GetSongInfo(titles[i]).album.c_str());
        ++row;
      }
      switch (this->action) {
        case YtmusAction::OPEN: {
        } break;
        case YtmusAction::PLAY: {
          this->client->PlaySong(selected.key);
        } break;
        case YtmusAction::NEW: {
        } break;
        case YtmusAction::ENQUEUE: {
          this->client->Enqueue(selected.key);
        } break;
        case YtmusAction::NEXT: {
          this->client->Next();
        } break;
        case YtmusAction::PREV: {
          this->client->Prev();
        } break;
        case YtmusAction::TOGGLE_PAUSE: {
          this->client->TogglePause();
        } break;
        case YtmusAction::STOP: {
          this->client->Stop();
        } break;
        case YtmusAction::NOP: {
        } break;
      }
    } break;
    case PLAYLISTS_MENU: {
      std::vector<std::string> playlist_names = this->datastore->GetPlaylists();
      wattrset(content, COLOR_PAIR(YtmusColor::TITLE) | A_BOLD | A_UNDERLINE);
      ::mvwprintw(content, 0, 0, "Name");
      playlist_entry selected;
      int row = 1;
      for (
          int i = this->selection_base[this->menu_num];
          i < std::min<int>(this->selection_base[this->menu_num] + num_rows - 1,
                            playlist_names.size());
          ++i) {
        if (i == this->selection[this->menu_num]) {
          wattrset(content, COLOR_PAIR(YtmusColor::SELECTED));
          ::wmove(content, row, 0);
          fill_line(content);
          selected = this->datastore->GetPlaylistInfo(playlist_names[i]);
        } else {
          wattrset(content, COLOR_PAIR(YtmusColor::NORMAL));
        }
        ::mvwprintw(content, row, 0, "%s", playlist_names[i].c_str());
        ++row;
      }
      switch (this->action) {
        case YtmusAction::OPEN: {
        } break;
        case YtmusAction::PLAY: {
          this->client->PlayPlaylist(selected.key);
        } break;
        case YtmusAction::NEW: {
        } break;
        case YtmusAction::ENQUEUE: {
          this->client->PlayPlaylist(selected.key);
        } break;
        case YtmusAction::NEXT: {
          this->client->Next();
        } break;
        case YtmusAction::PREV: {
          this->client->Prev();
        } break;
        case YtmusAction::TOGGLE_PAUSE: {
          this->client->TogglePause();
        } break;
        case YtmusAction::STOP: {
          this->client->Stop();
        } break;
        case YtmusAction::NOP: {
        } break;
      }
    } break;
  }
  wrefresh(content);
  delwin(content);
}

void fill_line(WINDOW* w) {
  int x = getcurx(w);
  int max_x = getmaxx(w);
  for (int i = 0; i < max_x - x; ++i) {
    waddch(w, ' ');
  }
}

}  // namespace YTMClient
