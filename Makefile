CC=clang
main.o: main.c
	$(CC) -o main.o main.c -lncurses -lmenu -lpanel -Wall
