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

void free_card_node(Card_node* node);

void free_deck(Deck* deck);

void print_card_node(Card_node* node);

Deck* load_deck(FILE* f);

int main(void) {
	// load from text file
	//FILE* fp = fopen("deck0.txt", "r");
	//if (fp == NULL) {
	//	printf("The file was not opened. The program will "
	//			"now exit.\n");
	//	exit(0);
	//}
	//Deck* current_deck = load_deck(fp);
	//fclose(fp);
	//printf("Loaded deck.\n");


	//free_deck(current_deck);

	// test card node
	Card* card0 = malloc(sizeof(Card));
	strcpy(card0->front, "Algeria");
	strcpy(card0->back, "Algiers");

	Card_node* cnode = malloc(sizeof(Card_node));
	cnode->data = card0;

	Deck* d = malloc(sizeof(Deck));
	strcpy(d->name, "Test");
	d->deck_size = card_ll_length(cnode);
	d->head = cnode;

	print_card_node(d->head);
	free_deck(d);

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


void free_card_node(Card_node* node) {
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
	char deck_name[32];
	if (fgets(deck_name, sizeof(deck_name), f) == NULL) return NULL;
	Deck* d = malloc(sizeof(Deck));
	strcpy(d->name, deck_name);
	d->deck_size = 0;
	d->head = NULL;
	
	char buff[FRONT_MAX];
	if (fgets(buff, sizeof(buff), f) == NULL) return d;

	Card_node* head = malloc(sizeof(Card_node));
	Card* new_card = malloc(sizeof(Card));
	strcpy(new_card->front, buff);
	head->data = new_card;
	head->next = NULL;

	Card_node* node = head;
	size_t size = 0;
	int front = 0;
	while (fgets(buff, sizeof(buff), f) != NULL) {
		if (front) {
			new_card = malloc(sizeof(Card));
			strcpy(new_card->front, buff);
			front = 0;
		} else {
			strcpy(new_card->back, buff);
			node->next = malloc(sizeof(Card_node));
			node = node->next;
			node->data = new_card;
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

