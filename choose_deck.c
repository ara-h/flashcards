/*
 * This file is for testing out the deck chooser menu -> study mode feature.
 */
#include <curses.h>
#include <dirent.h>
#include <fnmatch.h>
#include <menu.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "study.h"

#define DECK_NAME_MAX 256

int deck_selector(const struct dirent *e) {
    return !fnmatch("*.deck.txt", e->d_name, 0);
}


int enter_deck_study(int item_index, struct dirent **deck_namelist) {
    return study(deck_namelist[item_index]->d_name);
}


int main(void) {
    struct dirent **namelist;
    int n, i;
    n = scandir(".", &namelist, deck_selector, alphasort);
    char **choices = NULL;
    if (n >= 0) {
        choices = malloc(
            (n * sizeof(char *)) + (n * DECK_NAME_MAX * sizeof(char))
        );
        for (i = 0; i < n; i++) {
            choices[i] = (char *)(choices + n) + (i * DECK_NAME_MAX);
        }
        FILE *fp;
        char *sp;
        char deck_name[DECK_NAME_MAX];
        for (i = 0; i < n; i++) {
            fp = fopen(namelist[i]->d_name, "r");
            sp = fgets(deck_name, DECK_NAME_MAX, fp);
            fclose(fp);
            if (sp == NULL) {
                perror("Could not read from a file");
                free(choices);
                return 1;
            }
            deck_name[strcspn(deck_name, "\r\n")] = 0; // Remove ending CR, LF.
            strncpy(choices[i], deck_name, DECK_NAME_MAX);
        }
    } else {
        printf("No decks found in search path.\n");
        return 0;
    }

    // Initialize curses
    initscr();
    use_default_colors();

    int row, col;
    getmaxyx(stdscr, row, col);

    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    ITEM **dc_menu_items = (ITEM **)calloc(n + 1, sizeof(ITEM *));
    for (i = 0; i < n; i++) {
        dc_menu_items[i] = new_item(choices[i], NULL);
        set_item_userptr(dc_menu_items[i], enter_deck_study);
    }
    dc_menu_items[n] = (ITEM *)NULL;

    MENU *dc_menu = new_menu((ITEM **)dc_menu_items);
    menu_opts_off(dc_menu, O_SHOWDESC);

    mvprintw(row - 1, 0, "Press q to exit.");

    post_menu(dc_menu);
    refresh();

    int exit_i = 0;
    ITEM* cur;
    int (*p)(int item_index, struct dirent **deck_namelist);
    int c;
    while (!exit_i) {
        c = getch();
        switch(c) {
            case 'j':
            case KEY_DOWN:
                menu_driver(dc_menu, REQ_DOWN_ITEM);
                break;
            case 'k':
            case KEY_UP:
                menu_driver(dc_menu, REQ_UP_ITEM);
                break;
            case 10:
                cur = current_item(dc_menu);
                p = item_userptr(cur);
                unpost_menu(dc_menu);
                p(item_index(cur), namelist);
                erase();
                post_menu(dc_menu);
                mvprintw(row - 1, 0, "Press q to exit.");
                break;
            case KEY_RESIZE:
                getmaxyx(stdscr, row, col);
                mvprintw(
                    row - 1, 0,
                    "This window is now size %d x %d", row, col
                );
                break;
            case 'q':
                exit_i = 1;
                break;
        }
        refresh();
    }

    unpost_menu(dc_menu);
    free(dc_menu);
    for (i = 0; i < n + 1; i++) {
        free_item(dc_menu_items[i]);
    }
    free(choices);
    endwin();
    return 0;
}
