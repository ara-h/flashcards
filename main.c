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

void print_card_node(Card_node* node);

Deck* load_deck(FILE* f);

int main(void) {
	// load from text file
	FILE* fp = fopen("deck1.txt", "r");
	if (fp == NULL) {
		printf("The file was not opened. The program will "
				"now exit.\n");
		exit(0);
	}
	Deck* current_deck = load_deck(fp);
	fclose(fp);
	printf("Loaded deck.\n");


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


void print_card_node(Card_node* node) {
	char fr[12];
	char ba[12];
	while (node) {
		strncpy(fr, node->data->front, 11);
		strncpy(ba, node->data->back, 11);
		printf("< %s...| %s... >\n", fr, ba);
		node = node->next;
	}
	return;
}


Deck* load_deck(FILE* f) {
	// get deck name
	char deck_name[32];
	if (fgets(deck_name, sizeof(deck_name), f) == NULL) return NULL;
	Deck* d = malloc(sizeof(Deck));
	strcpy(d->name, deck_name);
	printf("deck name: %s\n",d->name); // testing
	d->deck_size = 0;
	d->head = NULL;
	
	// get front of first card
	char buff[FRONT_MAX];
	// if (fgets(buff, sizeof(buff), f) == NULL) return d;
	// make a new card linked list
	Card_node* head = malloc(sizeof(Card_node));
	head->next = NULL;

	Card* new_card;
	Card_node* node = head;
	size_t size = 0;
	int front = 1;
	while (fgets(buff, sizeof(buff), f) != NULL) {
		if (front) {
			new_card = malloc(sizeof(Card));
			strcpy(new_card->front, buff);
			printf("Read card front: %s\n", (new_card->front));
			front = 0;
		} else {
			strcpy(new_card->back, buff);
			printf("Read card back: %s\n", (new_card->back));
			node->data = new_card;
			node->next = malloc(sizeof(Card_node));
			node = node->next;
			node->next = NULL;
			size++;
			front = 1;
		}
	}

	// TODO: handle when deck doesn't end in a card added to deck
	d->deck_size = size;
	d->head = head;
	return d;
}

