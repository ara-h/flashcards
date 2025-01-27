CC=clang
main.o: main.c
	$(CC) -o main.o main.c

study.o: study.c
	$(CC) -o study.o study.c -lncurses -Wall
