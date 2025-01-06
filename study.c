#include <sys/queue.h>
//#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>

#define CARD_MAX_CHAR 512

enum study_state {
    SHOW_FRONT,
    SHOW_BACK
};

struct card {
    char front[CARD_MAX_CHAR];
    char back[CARD_MAX_CHAR];
    CIRCLEQ_ENTRY(card) cards;
};

CIRCLEQ_HEAD(deck_head, card);

int
wind_down(struct deck_head *head_ptr)
{
   struct card *tmp1, *tmp2;
   tmp1 = CIRCLEQ_FIRST(head_ptr);
   while (tmp1 != (void *)head_ptr) {
       tmp2 = CIRCLEQ_NEXT(tmp1, cards);
       free(tmp1);
       tmp1 = tmp2;
   }
   CIRCLEQ_INIT(head_ptr);

   return 0;
}

// The main function will do the following.
// Load search paths.
// Search for decks. Load them into something.
// Show menu to select deck to study. Associate base names of decks with
// menu items.
// Enter into study mode.
// In study mode, shuffle the cards and insert them in a queue.
// Show the cards from the queue. User will press enter or space key to show
// the answer, then press 1 to put it at the end of the queue or 2 to
// discard it.

int
main(void)
{
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

    // Prepare curses interface to the program.
    int row, col;

    initscr();
    use_default_colors();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, row, col);

    // Set the state to decide how to advance.
    enum study_state state = SHOW_FRONT;
    mvprintw(row - 1, 0, "Press q to exit.");
    mvprintw(row - 2, 0, "Press ENTER or SPACE to show answer.");

    // Get the text on the front of the first card.
    char card_str[CARD_MAX_CHAR]; // Declare an array to store text from cards.
    c1 = CIRCLEQ_FIRST(&head);
    strncpy(card_str, c1->front, CARD_MAX_CHAR);

    // Print the text.
    mvprintw(0, 0, "%s", card_str);
    refresh();

    // Get user input character and respond accordingly.
    int c;
    while ((c = getch()) != 'q') {
        // Show the answer if the input key is enter (10) or space (32).
        if (c == 10 || c == 32) {
            if (state == SHOW_FRONT) {
                state = SHOW_BACK;
                strncpy(card_str, c1->back, CARD_MAX_CHAR);
                move(0, 0);
                clrtobot();
                mvprintw(0, 0, "%s", card_str);
                mvprintw(row - 3, 0,
                         "Press 1 to review again or 2 to discard.");
                mvprintw(row - 2, 0, "Press ENTER or SPACE to show prompt.");
                mvprintw(row - 1, 0, "Press q to exit.");
                refresh();
                continue;
            } else {
                state = SHOW_FRONT;
                strncpy(card_str, c1->front, CARD_MAX_CHAR);
                move(0, 0);
                clrtobot();
                mvprintw(0, 0, "%s", card_str);
                mvprintw(row - 2, 0, "Press ENTER or SPACE to show answer.");
                mvprintw(row - 1, 0, "Press q to exit.");
                refresh();
                continue;
            }
        }
        if (state == SHOW_BACK && c == '1') {
            // Advance the deck, or if this is the last card, offer to study
            // deck again or exit.
            continue;
        }
        if (state == SHOW_BACK && c == '2') {
            // Remove the current card from the deck and advance, or if this is
            // the last card, offer to study deck again or exit.
            continue;
        }
    }


    endwin();

    return wind_down(&head);
}
