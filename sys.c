/* -------------- sys.c ---------------------------- */
#include <stdio.h>

#ifdef __CYGWIN__
#include <curses.h>
#else
#ifdef __DOS__
#include <conio.h>
#endif
#endif

#include "keys.h"
#include "sys.h"
/* #include "cdata.h" */

/* =======================================================
   The following functions are keyboard and screen drivers
   for the pc with ANSI.SYS installed
   /* ======================================================= */

extern int FieldChar, screen_displayed;

/* -------- write a character to the screen -------------- */
void put_char(int c)
{
    switch (c) {
    case FWD: printf("\033[C");
        break;
    case UP: printf("\033[A");
        break;
    /* default:  putchar(c == ' ' ? FieldChar : c); */
    }        
    fflush(stdout);
}

void cursor(int x, int y)
{
    printf("\033[%%02;0%dH",y+1, x+1);
    fflush(stdout);
}

void clear_screen(void)
{
    /* screen_displayed = 0; */
    printf("\033[2J");
    fflush(stdout);
}

int get_char(void)
{
    int c;

    fpurge(stdin);

    if ((c = getch()) == 0)
        c = getch() | 128;
    return c & 255;
}


/* ---- test ---- */
int main(void)
{
#ifdef __CYGWIN__
    initscr();
	clear();
    noecho();
	cbreak();	/* Line buffering disabled. pass on everything */
#endif

    clear_screen();
    printf("%x\n",get_char());

#ifdef __CYGWIN__
    //    endwin();
#endif

    return 0;
}
