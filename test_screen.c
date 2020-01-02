/* -------- test screen --------------------------------- */
/* #include <stdio.h> */
/* #include <termios.h> */
/* #include <unistd.h> */
/* #include <errno.h> */
#include <stdlib.h>
#include "cdata.h"
#include "screen.h"
#include "keys.h"    /* linkar com keys.o */
#include "ttcon.h"   /* linkar com ttcon.o */

static const int *els; /* supostament elementos */
static char *sc;       /* screen buffer     */

void (*database_message)(void);

void test_msg(void)
{
  puts("Testing screen...");  
}

int main(void)
{
  int c;
  int term = 0;
  clear_screen();

  init_keys();
  init_screen("TesteScreen", els, sc);

  while ((c = getch_t()) != CTRL_C)
    switch (c) {
    case HOME:
      puts("HOME ok");
      break;
      
    case F1:
      puts("F1 ok");
      break;
      
    default:
      printf ("[2;1Hdoskey = %d", c);
    }

  /* exit(0); */
  /* return 0; */
}
