#include <ctype.h>
#include <dirent.h>
#include <fnmatch.h>
#include <menu.h>
#include <ncurses.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "study.h"

# define VERSION_MAJOR 0
# define VERSION_MINOR 0
# define VERSION_MICRO 1

# define DECK_NAME_MAX 256

void show_help(char *ex_name);
int parse_config(FILE *config,
                 char *reg_buf, unsigned long reg_buf_sz,
                 char *path_buf, unsigned long path_buf_sz);
int deck_chooser(void);
int deck_selector(const struct dirent *e);
int enter_deck_study(int item_index, struct dirent **deck_namelist);

/*
* Parse command line arguments. Depending on inputs, either add path to search
* path, show help text, show search paths, show version, or enter study mode.
*/
int main(int argc, char** argv) {
    int version_flag, help_flag, search_paths_flag;
    version_flag = help_flag = search_paths_flag = 0;
    char new_path[1024];
    memset(new_path, 0, 1024);
    int c;
    while ((c = getopt(argc, argv, "a:hpV")) != -1) {
        switch(c) {
            case 0:
                break;
            case 'a':
                strncpy(new_path, optarg, 1024);
                break;
            case 'h':
                help_flag = 1;
                break;
            case 'p':
                search_paths_flag = 1;
                break;
            case 'V':
                version_flag = 1;
                break;
            case '?':
                if (optopt == 'a') {
                    fprintf(stderr, "Option -%c requires an argument.", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c`.\n", optopt);
                } else {
                    fprintf(
                        stderr,
                        "Unkown option character `\\x%x`.\n",
                        optopt
                    );
                }
                show_help(argv[0]);
                return 1;
            default:
                abort();
        }
    }

    if (help_flag) {
        show_help(argv[0]);
        return 0;
    }

    if (version_flag) {
        printf(
            "%s: %d.%d.%d\n",
            argv[0],
            VERSION_MAJOR,
            VERSION_MINOR,
            VERSION_MICRO
        );
        return 0;
    }

    FILE *config;
    if ((config = fopen("./flashcards.conf", "r")) == NULL) {
        fprintf(stderr, "Could not open ./flashcards.conf\n");
        printf("Using default search path \"./\"\n");
    }

    if (config != NULL) {
        // Parse configuration file.
        char reg_buf[4096];
        memset(reg_buf, 0, 4096);
        char path_buf[4096];
        memset(path_buf, 0, 4096);
        int res;
        res = parse_config(config, reg_buf, 4096, path_buf, 4096);
        fclose(config);
        if (res < 0) {
            return 1;
        }
    }

    // Get search paths.
    //      Paths included are expected to be separated by ':'.

    // Do the showing of the search paths and the exiting, if desired.
    if (search_paths_flag) {
        // Do something here.
        return 0;
    }

    // Do the adding of search paths and the exiting, if desired.
    if (new_path[0] != '\0') {
        // Do something here.
        return 0;
    }

    // Scan search paths for decks. Pass the number of decks and namelist to
    //      deck chooser.

    return deck_chooser();
}


void show_help(char *ex_name) {
    printf(
        "Usage: %s [option...] [-a path]\n"
                "Options:\n"
                "   -a path     add path(s) to the search paths, :-separated\n"
                "   -h          show this help text\n"
                "   -p          show search paths\n"
                "   -V          show version\n",
        ex_name
    );
    return;
}


int parse_config(FILE *config,
                 char *reg_buf, unsigned long reg_buf_sz,
                 char *path_buf, unsigned long path_buf_sz) {
    regex_t regex;
    int reti;
    reti = regcomp(
        &regex,
        "^SEARCH_PATH=(.*)$",
        REG_EXTENDED | REG_NEWLINE
    );
    if (reti != 0) {
        fprintf(
            stderr,
            "parse_config: could not compile regex\n"
        );
        regerror(reti, &regex, reg_buf, reg_buf_sz);
        fprintf(stderr, "%s\n", reg_buf);
        regfree(&regex);
        return -1;
    }
    if (fgets(reg_buf, reg_buf_sz, config) == NULL) {
        fprintf(
            stderr,
            "parse_config: there was a problem reading the config file.\n"
        );
        regfree(&regex);
        return -1;
    }
    regmatch_t regmatch[2];
    reti = regexec(&regex, reg_buf, 2, regmatch, 0);
    while (reti == REG_NOMATCH
           && fgets(reg_buf, reg_buf_sz, config) != NULL) {
        reti = regexec(&regex, reg_buf, 2, regmatch, 0);
    }
    if (!reti) {
        int final = regmatch[1].rm_eo;
        int initial = regmatch[1].rm_so;
        strncpy(path_buf, reg_buf + initial, final - initial);
    } else {
        if (reti == REG_NOMATCH) {
            fprintf(
                stderr,
                "parse_config: no match was found in the file.\n"
            );
        } else {
            regerror(reti, &regex, reg_buf, reg_buf_sz);
            fprintf(
                stderr,
                "parse_config: regex match failed: %s\n",
                reg_buf
            );
        }
        regfree(&regex);
        return -1;
    }
    regfree(&regex);
    return 1;
}


int deck_chooser(void) {
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
            case KEY_ENTER:
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


int deck_selector(const struct dirent *e) {
    return !fnmatch("*.deck.txt", e->d_name, 0);
}

int enter_deck_study(int item_index, struct dirent **deck_namelist) {
    return study(deck_namelist[item_index]->d_name);
}
