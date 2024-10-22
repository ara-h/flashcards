#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

void print_window_title(WINDOW *win, int starty, int startx, int width,
                        char *str, int str_len);

WINDOW *make_main_menu_win(MENU *mm, WINDOW *win);

void placeholder_func(char *name, WINDOW *win);

int main(int argc, char *argv[]) {
  /*
  char *choices[] = {"Study deck                                 s",
                     "List decks                                 l",
                     "Edit decks                                 e",
                     "Load new deck from file                    o",
                     "Delete deck                                d",
                     "Exit                                       q"};
  */
  char *choices[] = {"Study deck",  "List decks",
                     "Edit decks",  "Load new deck from file",
                     "Delete deck", "Exit"};
  int n_choices = sizeof(choices) / sizeof(choices[0]);

  // Initialize curses
  initscr();
  use_default_colors();

  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  // Set menu items
  ITEM **mm_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
  int i;
  for (i = 0; i < n_choices; ++i) {
    mm_items[i] = new_item(choices[i], NULL);
    set_item_userptr(mm_items[i], placeholder_func);
  }
  mm_items[n_choices] = (ITEM *)NULL;

  // Make main menu
  MENU *main_menu = new_menu((ITEM **)mm_items);
  // Set menu to not show description
  menu_opts_off(main_menu, O_SHOWDESC);
  // Make main menu window
  WINDOW *main_menu_win = make_main_menu_win(main_menu, NULL);

  ITEM *cur;
  void (*p)(char *, WINDOW *);
  int c;
  while ((c = wgetch(main_menu_win)) != 'q') {
    switch (c) {
    case 'j':
    case KEY_DOWN:
      menu_driver(main_menu, REQ_DOWN_ITEM);
      break;
    case 'k':
    case KEY_UP:
      menu_driver(main_menu, REQ_UP_ITEM);
      break;
    case 10: // 'Enter'
      cur = current_item(main_menu);
      p = item_userptr(cur);
      p((char *)item_name(cur), main_menu_win);
      pos_menu_cursor(main_menu);
      break;
      // Do something on pressing Enter
    }

    // but if wgetch returns the window resize char, then move the window to new
    // center
    wrefresh(main_menu_win);
  }

  // Unpost menu and free
  unpost_menu(main_menu);
  free_menu(main_menu);
  for (i = 0; i < n_choices; i++)
    free_item(mm_items[i]);
  endwin();
  exit(EXIT_SUCCESS);
}

void print_window_title(WINDOW *win, int starty, int startx, int width,
                        char *str, int str_len) {
  int x, y;
  if (win == NULL) {
    win = stdscr;
  }
  getyx(win, y, x);
  if (startx != 0) {
    x = startx;
  }
  if (starty != 0) {
    y = starty;
  }
  if (width == 0) {
    width = 80;
  }

  int temp = (width - str_len) / 2;
  x = startx + temp;
  mvwprintw(win, y, x, "%s", str);
  refresh();
}

WINDOW *make_main_menu_win(MENU *mm, WINDOW *win) {
  endwin();

  if (win == NULL) {
    win = stdscr;
  }

  int start_col = (getmaxx(win) / 2) - 24; // maxx betta be >= 48
  int start_row = (getmaxy(win) / 2) - 5;

  WINDOW *main_menu_win = newwin(10, 48, start_row, start_col);
  keypad(main_menu_win, TRUE);

  set_menu_win(mm, main_menu_win);
  WINDOW *main_menu_derwin = derwin(main_menu_win, 8, 46, 1, 1);
  set_menu_sub(mm, main_menu_derwin);

  box(main_menu_win, 0, 0);
  char title[] = "FLASHCARDS";
  int title_len = sizeof(title) / sizeof(title[0]);
  print_window_title(main_menu_win, 0, 1, 48, title, title_len);
  refresh();
  post_menu(mm);
  wrefresh(main_menu_win);
  // TODO: does this leak memory? the WINDOWs?
  return main_menu_win;
}

void placeholder_func(char *name, WINDOW *win) {
  wmove(win, 8, 1);
  clrtoeol(); // This doesn't really clear to end of line
  mvwprintw(win, 8, 1, "Item selected is : %s", name);
}
