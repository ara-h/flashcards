#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

void print_window_title(WINDOW *win, int starty, int startx, int width,
                        char *str, int str_len);

int main(int argc, char *argv[]) {

  char *choices[] = {"Study deck",  "List decks",
                     "Edit decks",  "Load new deck from file",
                     "Delete deck", "Exit"};

  int n_choices = sizeof(choices) / sizeof(choices[0]);
  // n_choices must be less than 99
  char **choice_indexes = (char **)malloc(n_choices * sizeof(char *));
  int i;
  for (i = 0; i < n_choices; i++) {
    choice_indexes[i] = (char *)malloc(4 * sizeof(char));
  }
  for (i = 0; i < n_choices - 1; i++) {
    sprintf(choice_indexes[i], "%d.", i + 1);
  }
  sprintf(choice_indexes[n_choices - 1], "0.");

  initscr();

  start_color();

  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  ITEM **mm_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
  for (i = 0; i < n_choices; ++i) {
    mm_items[i] = new_item(choice_indexes[i], choices[i]);
  }
  mm_items[n_choices] = (ITEM *)NULL;

  MENU *main_menu = new_menu((ITEM **)mm_items);

  WINDOW *main_menu_win = newwin(10, 40, 4, 4);
  keypad(main_menu_win, TRUE);

  set_menu_win(main_menu, main_menu_win);
  set_menu_sub(main_menu, derwin(main_menu_win, 6, 38, 3, 1));

  set_menu_mark(main_menu, " * ");

  box(main_menu_win, 0, 0);
  char title[] = "FLASHCARDS";
  int title_len = sizeof(title) / sizeof(title[0]);
  print_window_title(main_menu_win, 1, 0, 40, title, title_len);
  mvwaddch(main_menu_win, 2, 0, ACS_LTEE);
  mvwhline(main_menu_win, 2, 1, ACS_HLINE, 38);
  mvwaddch(main_menu_win, 2, 39, ACS_RTEE);
  mvprintw(LINES - 3, 0, "Press Up and Down arrow keys to navigate,");
  mvprintw(LINES - 2, 0, "<ENTER> to select, or q to exit");
  refresh();

  post_menu(main_menu);
  wrefresh(main_menu_win);

  int c;
  while ((c = wgetch(main_menu_win)) != 'q') {
    switch (c) {
    case KEY_DOWN:
      menu_driver(main_menu, REQ_DOWN_ITEM);
      break;
    case KEY_UP:
      menu_driver(main_menu, REQ_UP_ITEM);
      break;
    }
    wrefresh(main_menu_win);
  }

  unpost_menu(main_menu);
  free_menu(main_menu);
  free(choice_indexes);
  for (i = 0; i < n_choices; i++)
    free_item(mm_items[i]);
  endwin();
  exit(EXIT_SUCCESS);
}

void print_window_title(WINDOW *win, int starty, int startx, int width,
                        char *str, int str_len) {
  int x, y;
  if (win == NULL)
    win = stdscr;
  getyx(win, y, x);
  if (startx != 0)
    x = startx;
  if (starty != 0)
    y = starty;
  if (width == 0)
    width = 80;

  int temp = (width - str_len) / 2;
  x = startx + temp;
  mvwprintw(win, y, x, "%s", str);
  refresh();
}
