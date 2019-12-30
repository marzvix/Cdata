/* -- pg 183 ---- sys.c ---------------------------- */

#include <stdio.h>
/* #include <conio.h> */#include <stdlib.h>
#include "keys.h"
#include "sys.h"
#include "cdata.h"

/* =======================================================
   The following functions are keyboard and screen drivers
   for the pc with ANSI.SYS installed
   ======================================================= */

extern int FieldChar, screen_displayed;

/* -------- write a character to the screen -------------- */
void put_char(int c)
{
  switch (c) {
  case FWD: printf("[C");
    break;
  case UP: printf("[A");
    break;
  default: putchar(c == ' ' ? FieldChar : c);
  }
  fflush(stdout);
}

/* ------------ set the cursor position ------------------ */
void cursor(int x, int y)
{
  printf("[%02d;%02dH",y+1, x+1);
  fflush(stdout);
}

/* -------------- clear the screen ----------------------- */
void clear_screen(void)
{
  screen_displayed = 0;
  printf("[0H;[2J"); 
  fflush(stdout);
}

/* ------- get a keyboard character ---------------------- */

int get_char(void)
{
  int c;
  if ((c = getchar()) == 0)
    c = getchar() | 128;

  return c & 255;
}
