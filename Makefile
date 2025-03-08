CC=clang
main.o: main.c
	$(CC) -o main.o main.c


choose_deck.o: choose_deck.c study.c
	$(CC) -o choose_deck.o choose_deck.c study.c -lncurses -lmenu -Wall
