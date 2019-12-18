/* ----------------- sys.c -------------------- */
#include <stdio.h>
#include "cdata.h"
#include "keys.h"

#if COMPILER == AZTEC
#define ci() scr_getc()
#endif
#if COMPILER == DATALIGHT
#define ci() getch()
#endif
#if COMPILER == ECOC
#define ci() getch()
#endif
#if COMPILER == LATTICE
#define ci() getch()
#endif
#if COMPILER == LETSC
#define ci() getcnb()
#endif
#if COMPILER == MICROSOFT
#define ci() getch()
#endif
#if COMPILER == TURBOC
#define ci() getch()
#endif
#if COMPILER == GCC
#include <stdlib.h>
/*
   nesse caso, deve-se definir a disciplina do 
   terminal para "raw" e retornar depois 
   para "cooked" após "pegar" o caractere"
   Tem forma mais chique de fazer isso.
   Mas fica para melhorias. (TODO)
*/

int getch()
{
  int ch;
  system("stty raw");
  ch = getchar();
  system("stty cooked");
  return ch;
}

#define ci()  getch()

#endif

/* ------------- get a keyboard character ----------------- */
int get_char(void)
{
 	int c;

#if COMPILER == CI_C86
	c = key_getc();
	if ((c & 255) == 0)
		c = (c >> 8) | 128;
#else
#if COMPILER == WIZARD
	c = bioskey(0);
	if ((c & 255) == 0)
		c = (c >> 8) | 128;
#else
	if (!(c = ci()))
		c = ci() | 128;
#endif
#endif
	return c & 255;
}

/* -------- write a character to the screen ------------ */
void put_char(int c)
{
	switch (c)	{
		case FWD:	printf("\033[C");
					break;
		case UP:	printf("\033[A");
					break;
		default:	putchar(c);
	}
	fflush(stdout);
}

/* ------------- set the cursor position -------------- */
void cursor(int x, int y)
{
	printf("\033[%02d;%02dH",y+1, x+1);
	fflush(stdout);
}

/* ------------------- clear the screen ------------------- */
int screen_displayed = 0;
void clear_screen(void)
{
	screen_displayed = 0;
	printf("\033[2J");
	fflush(stdout);
}
