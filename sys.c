/* ----------------- sys.c -------------------- */
#include <stdio.h>
#include <string.h>

#include "keys.h"
#include "sys.h"
#include "cdata.h"

#define ci() getch()

int getch()
{
  int ch;
  /* system("stty raw"); */
  ch = getchar();
  /* system("stty cooked"); */
  return ch;
}

/* ------------- get a keyboard character ----------------- */
int get_char(void)
{
  int c;

  if (!(c = ci()))
  	c = ci() | 128;
   return c & 255;
}

/* -------- write a character to the screen ------------ */
void put_char(int c)
{
  switch (c)	{
  case DN:
    printf(ASC_DN);
    break;
  case UP:
    printf(ASC_UP);
    break;
  case BWD:
    printf(ASC_BWD);
    break;
  case FWD:
    printf(ASC_FWD);
    break;
  default:
    putchar(c);
  }
  fflush(stdout);
}

/* ------------- set the cursor position -------------- */
void cursor(int x, int y)
{
  /* printf("\033[%02d;%02dH",y+1, x+1); */
  /* fflush(stdout); */
  ASC_CMD_MOV(y, x);
}

/* ------------------- clear the screen ------------------- */
int screen_displayed = 0;
void clear_screen(void)
{
  screen_displayed = 0;
  printf("\033[2J");
  fflush(stdout);
}
