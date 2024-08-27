#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <menu.h>

void print_in_middle(WINDOW *win, int starty, int startx, int width,
                     char *str, int str_len, chtype color);

int main(int argc, char *argv[]) {

  char *choices[] = {
    "Study deck",
    "List decks",
    "Edit decks",
    "Load new deck from file",
    "Delete deck",
    "Exit"
  };

  int n_choices = sizeof(choices)/sizeof(choices[0]);
  // n_choices must be less than 99
  char **choice_indexes = (char **)malloc(n_choices*sizeof(char*));
  int i;
  for (i = 0; i < n_choices; i++) {
    choice_indexes[i] = (char *)malloc(4*sizeof(char));
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
  init_pair(1, COLOR_WHITE, COLOR_BLACK);

  ITEM **mm_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
  for (i = 0; i < n_choices; ++i) {
    mm_items[i] = new_item(choice_indexes[i], choices[i]);
  }
  mm_items[n_choices] = (ITEM *)NULL;

  MENU *main_menu = new_menu((ITEM **)mm_items);

  WINDOW *main_menu_win = newwin(10,40,4,4);
  keypad(main_menu_win, TRUE);

  set_menu_win(main_menu, main_menu_win);
  set_menu_sub(main_menu, derwin(main_menu_win, 6, 38, 3, 1));

  set_menu_mark(main_menu, " * ");

  box(main_menu_win, 0, 0);
  char title[] = "FLASHCARDS";
  int title_len = sizeof(title)/sizeof(title[0]);
  print_in_middle(main_menu_win, 1, 0, 40, title, title_len, COLOR_PAIR(1));
  mvwaddch(main_menu_win, 2, 0, ACS_LTEE);
  mvwhline(main_menu_win, 2, 1, ACS_HLINE, 38);
  mvwaddch(main_menu_win, 2, 39, ACS_RTEE);
  mvprintw(LINES - 2, 0, "q to Exit");
  refresh();

  post_menu(main_menu);
  wrefresh(main_menu_win);

  int c;
  while((c = wgetch(main_menu_win)) != 'q') {
    switch(c) {
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
  for (i = 0; i < n_choices; i++) free_item(mm_items[i]);
  endwin();
  exit(EXIT_SUCCESS);
}


void print_in_middle(WINDOW *win, int starty, int startx, int width,
                     char *str, int str_len, chtype color) {
  int x, y;
  float temp;

  if (win == NULL) win = stdscr;
  getyx(win, y, x);
  if(startx != 0) x = startx;
  if(starty != 0) y = starty;
  if (width == 0) width = 80;

  temp = (width - str_len) / 2;
  x = startx + (int)temp;
  wattron(win, color);
  mvwprintw(win, y, x, "%s", str);
  wattroff(win, color);
  refresh();
}
