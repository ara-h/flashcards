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

// struct deck_head* load_deck(FILE* deck_fp);

int study(char *deck_fn) {
    struct card *c1, *c2, *c3;
    struct deck_head head;

    CIRCLEQ_INIT(&head);

    c1 = malloc(sizeof(struct card));
    strncpy(c1->front, "This is the front of the first card.", CARD_MAX_CHAR);
    strncpy(c1->back, "This is the back of the first card.", CARD_MAX_CHAR);
    CIRCLEQ_INSERT_HEAD(&head, c1, cards);

    c2 = malloc(sizeof(struct card));
    strncpy(c2->front, "This is the front of the second card.", CARD_MAX_CHAR);
    strncpy(c2->back, "This is the back of the second card.", CARD_MAX_CHAR);
    CIRCLEQ_INSERT_TAIL(&head, c2, cards);

    c3 = malloc(sizeof(struct card));
    strncpy(c3->front, "This is the front of the third card.", CARD_MAX_CHAR);
    strncpy(c3->back, "This is the back of the third card.", CARD_MAX_CHAR);
    CIRCLEQ_INSERT_AFTER(&head, c2, c3, cards);

    int row, col;
    getmaxyx(stdscr, row, col);

    // Clear what was on the screen from the previous menu.
    erase();

    // Set the state to decide how to advance.
    enum study_state state = SHOW_FRONT;
    mvprintw(row - 1, 0, "Press q to exit.");
    mvprintw(row - 2, 0, "Press ENTER or SPACE to show answer.");

    // Get the text on the front of the first card.
    char card_str[CARD_MAX_CHAR]; // Declare an array to store text from cards.
    c1 = CIRCLEQ_FIRST(&head);
    strncpy(card_str, c1->front, CARD_MAX_CHAR);

    // Print the text.
    mvprintw(0, 0, "Prompt:");
    mvprintw(1, 0, "%s", card_str);
    refresh();

    // Get user input character and respond accordingly.
    // To do:
    //      Provide two ways to exit: (1) exit the entire program or (2) exit
    //      the current deck and go back to the deck chooser.

    int exit_i = 0;
    int c;
    while (!exit_i) {
        c = getch();
        if (c == 'q') exit_i = 1;
        // Show the answer if the input key is enter (10) or space (32).
        if (state != DECK_END && (c == 10 || c == 32)) {
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
                refresh();
            } else {
                state = SHOW_FRONT;
                strncpy(card_str, c1->front, CARD_MAX_CHAR);
                erase();
                mvprintw(0, 0, "Prompt:");
                mvprintw(1, 0, "%s", card_str);
                mvprintw(row - 2, 0, "Press ENTER or SPACE to show answer.");
                mvprintw(row - 1, 0, "Press q to exit.");
                refresh();
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
                refresh();
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
                refresh();
            }
        }
        if (state == DECK_END && c == '1') {
            // Load the current deck again.
            erase();
            mvprintw(0, 0, "Ok, imagine that we loaded the deck again.");
            mvprintw(row - 1, 0, "Press q to exit.");
            refresh();
        }
    }

    wind_down_study(&head);
    return exit_i;
}
