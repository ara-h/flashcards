#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
  // This should be defined in a main_menu function
  printf("FLASHCARDS\n"); // Stylize this, taking into account terminal width
  printf("Type the number corresponding to the option you want to select:\n");
  printf("  1. Study deck\n");
  printf("  2. List decks\n");
  printf("  3. Edit decks\n");
  printf("  4. Load deck from text file\n");
  printf("  5. Delete deck\n");
  printf("  6. Print configuration file path\n");
  printf("  0. Exit\n");

  int c;
  while ((c = getchar()) != EOF) {
    if (c == ' ') {
      printf("Ignore space\n");
      continue;
    } else {
      printf("Got 0x%02hhx\n", c);
    }

    switch(c) {
      case '1':
        printf("You chose menu option 1.\n");
      case '2':
        printf("You chose menu option 2.\n");
      case '3':
        printf("You chose menu option 3.\n");
      case '4':
        printf("You chose menu option 4.\n");
      case '5':
      case '6':
      case '0':
        {
          printf("You chose to exit. Exiting with zero.\n");
          break;
        }
    }
  }
  exit(EXIT_SUCCESS);
}
