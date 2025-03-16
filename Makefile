CC=clang
main.o: main.c study.c
	$(CC) -o main.o main.c study.c -lncurses -lmenu -Wall


choose_deck.o: choose_deck.c study.c
	$(CC) -o choose_deck.o choose_deck.c study.c -lncurses -lmenu -Wall
