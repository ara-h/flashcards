#UNAME=:$(shell uname)

main.o: main.c
	gcc -o main.o main.c -lncurses -lmenu -lpanel -Wall
