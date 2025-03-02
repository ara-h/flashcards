CC=clang
main.o: main.c
	$(CC) -o main.o main.c

study.o: study.c
	$(CC) -o study.o study.c -lncurses -Wall

choose_deck.o: choose_deck.c
	$(CC) -o choose_deck.o choose_deck.c -lncurses -lmenu -Wall
