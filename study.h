#include <sys/queue.h>
// To do: change to conditional include?
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define CARD_MAX_CHAR 512

enum study_state { SHOW_FRONT, SHOW_BACK, DECK_END };

struct card {
    char front[CARD_MAX_CHAR];
    char back[CARD_MAX_CHAR];
    CIRCLEQ_ENTRY(card) cards;
};

CIRCLEQ_HEAD(deck_head, card);

void wind_down_study(struct deck_head *head_ptr);
int study(char* deck_fn);
