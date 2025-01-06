CC=clang
main.o: main.c
	$(CC) -o main.o main.c -lncurses -lmenu -lpanel -Wall

study.o: study.c
	$(CC) -o study.o study.c -lncurses -Wall
