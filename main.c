#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FRONT_MAX 512
#define BACK_MAX 512

typedef struct {
	char front[FRONT_MAX];
	char back[BACK_MAX];
} Card;


typedef struct Card_node {
	Card* data;
	struct Card_node* next;
} Card_node;


typedef struct {
	char name[32];
	size_t deck_size;
	Card_node* head;	
} Deck;


void card_prompt(Card card);

size_t card_ll_length(Card_node* head);

void free_card_node(Card_node* head);

void free_deck(Deck* deck);

void lrtrimncpy(char* dst, char* src, size_t n);

void print_card_node(Card_node* node);

Deck* load_deck(FILE* f);

int main(void) {
	// load from text file
	FILE* fp = fopen("deck0.txt", "r");
	if (fp == NULL) {
		printf("The file was not opened. The program will "
				"now exit.\n");
		exit(0);
	}
	Deck* current_deck = load_deck(fp);
	fclose(fp);
	printf("Loaded deck.\n");

	print_card_node(current_deck->head);

	free_deck(current_deck);

	return 0;
}


void card_prompt(Card card) {
	printf("\n");
	printf("%s\n", card.front);
	while( getchar() != '\n' );
	printf("%s\n", card.back);
	return;
}


size_t card_ll_length(Card_node* head) {
	size_t l = 0;
	Card_node* tmp = head;
	while (tmp) {
		l++;
		tmp = tmp->next;
	}
	return l;
}


void free_card_node(Card_node* head) {
	Card_node* node = head;
	Card_node* tmp;
	while(node) {
		tmp = node;
		node = node->next;
		free(tmp->data);
		free(tmp);
	}
	return;
}


void free_deck(Deck* deck) {
	free_card_node(deck->head);
	free(deck);
	return;
}


void lrtrimncpy(char* dst, char* src, size_t n) {
  // Trim whitespace and copy n characters
  // dst must have room for n characters and a null terminator
  char* srcStart = src;
  while(*srcStart == ' ') srcStart++;
  char* srcEnd = srcStart;
  while(*srcEnd != '\0' && *srcEnd != '\n' && *srcEnd != '\r') srcEnd++;
  while(*srcEnd == ' ') srcEnd--;
  int b = (srcEnd - srcStart);
  int i = 0;
  while(i < n && i < b) {
    *(dst + i) = *(srcStart + i);
    i++;
  }
  *(dst + i) = '\0';
  return;
}


void print_card_node(Card_node* node) {
	Card_node* temp = node;
  char fr[12];
	char ba[12];
	while (temp != NULL) {
		lrtrimncpy(fr, temp->data->front, 11);
		lrtrimncpy(ba, temp->data->back, 11);
		printf("< %s...| %s... >\n", fr, ba);
		temp = temp->next;
	}
	return;
}


Deck* load_deck(FILE* f) {
	// get deck name
	char deck_name[32];
	if (fgets(deck_name, sizeof(deck_name), f) == NULL) return NULL;
	Deck* d = malloc(sizeof(Deck));
	strcpy(d->name, deck_name);
	// printf("deck name: %s\n",d->name); // testing
	d->deck_size = 0;
	d->head = NULL;
	
	char buff[FRONT_MAX];
	// make a new card linked list
	Card_node* head = malloc(sizeof(Card_node));
	head->next = NULL;

	Card* new_card = NULL;
	Card_node* node = head;
	size_t size = 0;
	int front = 1;
	while (fgets(buff, sizeof(buff), f) != NULL) {
		if (front) {
      if (size > 0) {
        // Create and advance node here
        node->next = malloc(sizeof(Card_node));
        node = node->next;
        node->next = NULL;
      }
      new_card = malloc(sizeof(Card));
			strcpy(new_card->front, buff);
			// printf("Read card front: %s\n", (new_card->front));
			front = 0;
		} else {
			strcpy(new_card->back, buff);
			// printf("Read card back: %s\n", (new_card->back));
      // We are putting the data in here, but not advancing to the next node
			node->data = new_card;
			size++;
			front = 1;
		}
	}
	
	// handle when deck doesn't end in a card added to deck
	if (front == 0) {
		if (new_card == NULL) {
			// no cards were read from file
			head = NULL;
		} else {
			// front was read but no back
			// insert message as back of card
			char blank_msg[] = "flashcards: the back of this card wasn't found\n";
			// printf("Read card back: %s\n", blank_msg);
			strcpy(new_card->back, blank_msg);
			node->data = new_card;
			size++;
		}
	} 
	
	d->deck_size = size;
	d->head = head;
	return d;
}

