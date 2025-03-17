#include <stdio.h>
#include <string.h>
#include <sys/queue.h>
#include "study.h"

void wind_down_study(struct deck_head *head_ptr) {
    struct card *tmp1, *tmp2;
    tmp1 = CIRCLEQ_FIRST(head_ptr);
    while (tmp1 != (void *)head_ptr) {
        tmp2 = CIRCLEQ_NEXT(tmp1, cards);
        free(tmp1);
        tmp1 = tmp2;
    }
    CIRCLEQ_INIT(head_ptr);
    return;
}

int load_deck(struct deck_head *hp, FILE *fp, char *buf) {
    unsigned int front, n;
    front = 1;
    n = 0;
    struct card *c = NULL;
    while (fgets(buf, CARD_MAX_CHAR, fp) != NULL) {
        if (front) {
            c = malloc(sizeof(struct card));
            strncpy(c->front, buf, CARD_MAX_CHAR);
            front = 0;
        } else {
            strncpy(c->back, buf, CARD_MAX_CHAR);
            CIRCLEQ_INSERT_TAIL(hp, c, cards);
            front = 1;
            n++;
        }
    }

    int retv = 0;
    if (c == NULL) {
        retv = 1; // Nothing was read from the file.
    }
    if (!front) {
        if (n > 1) {
            sprintf(c->back, "\n");
            CIRCLEQ_INSERT_TAIL(hp, c, cards);
        } else {
            free(c);
            retv = 1;
        }
    }
    return retv;
}

int study(char *deck_fn) {
    // struct card *c1, *c2, *c3;
    FILE *fp;
    if ((fp = fopen(deck_fn, "r")) == NULL) {
        return 1;
    }

    struct deck_head head;
    CIRCLEQ_INIT(&head);

    int exit_i = 0;
    char card_str[CARD_MAX_CHAR];
    if (fgets(card_str, CARD_MAX_CHAR, fp) == NULL) return 1;
    exit_i = load_deck(&head, fp, card_str);
    fclose(fp);

    int row, col;
    getmaxyx(stdscr, row, col);

    // Clear what was on the screen from the previous menu.
    erase();

    // Declare int to hold user input.
    int c;
    // Inform user if there was only one dangling card front read.
    if (exit_i) {
        c = 0;
        mvprintw(0, 0,
                 "There were no complete prompt/answer pairs in the chosen "
                 "deck.");
        mvprintw(row - 1, 0, "Press q to exit.");
        refresh();
        do { c = getch(); } while (c != 'q');
        return exit_i;
    }

    mvprintw(row - 1, 0, "Press q to exit.");
    mvprintw(row - 2, 0, "Press ENTER or SPACE to show answer.");

    struct card *c1, *c2;
    c1 = CIRCLEQ_FIRST(&head);
    strncpy(card_str, c1->front, CARD_MAX_CHAR);

    mvprintw(0, 0, "Prompt:");
    mvprintw(1, 0, "%s", card_str);
    refresh();

    // Get user input character and respond accordingly.
    // To do:
    //      Provide two ways to exit: (1) exit the entire program or (2) exit
    //      the current deck and go back to the deck chooser.

    // Set the state to decide how to advance.
    enum study_state state = SHOW_FRONT;

    while (!exit_i) {
        c = getch();
        if (c == 'q') exit_i = 1;
        // Show the answer if the input key is enter (10) or space (32).
        if (state != DECK_END && (c == 10 || c == 32 || c == KEY_ENTER)) {
            if (state == SHOW_FRONT) {
                state = SHOW_BACK;
                strncpy(card_str, c1->back, CARD_MAX_CHAR);
                erase();
                mvprintw(0, 0, "Answer:");
                mvprintw(1, 0, "%s", card_str);
                mvprintw(row - 3, 0,
                         "Press a to review again or d to discard.");
                mvprintw(row - 2, 0, "Press ENTER or SPACE to show prompt.");
                mvprintw(row - 1, 0, "Press q to exit.");
            } else {
                state = SHOW_FRONT;
                strncpy(card_str, c1->front, CARD_MAX_CHAR);
                erase();
                mvprintw(0, 0, "Prompt:");
                mvprintw(1, 0, "%s", card_str);
                mvprintw(row - 2, 0, "Press ENTER or SPACE to show answer.");
                mvprintw(row - 1, 0, "Press q to exit.");
            }
        }
        // Advance the deck if the input character was 'a'.
        if (state == SHOW_BACK && c == 'a') {
            c1 = CIRCLEQ_NEXT(c1, cards);
            if (c1 == (void *)&head) {
                c1 = CIRCLEQ_FIRST(&head);
            }
            state = SHOW_FRONT;
            strncpy(card_str, c1->front, CARD_MAX_CHAR);
            erase();
            mvprintw(0, 0, "Prompt:");
            mvprintw(1, 0, "%s", card_str);
            mvprintw(row - 2, 0, "Press ENTER or SPACE to show answer.");
            mvprintw(row - 1, 0, "Press q to exit.");
        }
        if (state == SHOW_BACK && c == 'd') {
            // If the input character was 'd', remove the current card from the
            // deck and advance, or if this is the last card, offer to study
            // deck again or exit.
            c2 = CIRCLEQ_NEXT(c1, cards);
            CIRCLEQ_REMOVE(&head, c1, cards);
            c1 = c2;
            if (CIRCLEQ_EMPTY(&head)) {
                state = DECK_END;
                erase();
                mvprintw(0, 0, "You have reached the end of the deck.");
                mvprintw(row - 2, 0, "Press a to study the deck again.");
                mvprintw(row - 1, 0, "Press q to exit.");
            } else {
                if (c1 == (void *)&head) {
                    c1 = CIRCLEQ_FIRST(&head);
                }
                state = SHOW_FRONT;
                strncpy(card_str, c1->front, CARD_MAX_CHAR);
                erase();
                mvprintw(0, 0, "Prompt:");
                mvprintw(1, 0, "%s", card_str);
                mvprintw(row - 2, 0, "Press ENTER or SPACE to show answer.");
                mvprintw(row - 1, 0, "Press q to exit.");
            }
        }
        if (state == DECK_END && c == 'a') {
            // Load the current deck again.
            erase();
            mvprintw(0, 0, "Ok, imagine that we loaded the deck again.");
            mvprintw(row - 1, 0, "Press q to exit.");
        }
        refresh();
    }

    wind_down_study(&head);
    return exit_i;
}
