#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include "sys.h"
#include "termctrl.h"

static struct termview
{
  int rows;
  int cols;
  struct termios orig_termios;
} TV;

void init_termview(void)
{
  if( get_tv_size(&TV.rows, &TV.cols) == -1)
      exit(0);
}

/* ---- set tty terminal to mode "cooked" ----- */
void disable_rawtty(void)
{
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &TV.orig_termios) == -1)
    exit(1);
}

/* ---- set tty terminal to mode not "cooked" ----- */
void enable_rawtty(void)
{
  if(tcgetattr(STDIN_FILENO, &TV.orig_termios) == -1)
    exit(1);
  atexit(disable_rawtty);
  struct termios raw = TV.orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    exit(0);
}

/* ---- get keys pressed from system input (stdin) and parse it ---- */
void get_keypress(void)
{
  int c = get_char();

  switch (c) {
  case CTRL_KEY('q'):
    clear_screen();
    exit(0);
    break;
  }
}

/* ----- get terminal view size ----- */
int get_tv_size(int *rows, int *cols)
{
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)    {
    return -1;
  } else     {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

/* ----- get cursor position ----- */
int get_cursor_pos(int *rows, int *cols) {
}

/* int get_char(void) */
/* { */
/*   int c; */
/*   auto int i; */

/* #ifdef __DOS__ */
/*   if ((c = getch()) == 0) */
/*     c = getch() | 128; */
/*   return c & 255; */
/* #endif */

/* #ifdef __CYGWIN__ */

/*   enable_rawtty(); */

/*   i = 0; */
/*   do { */
/*     c = getchar(); */
/*     printf("\n\r%d: '%c' %d ", i, c, c); */
/*   } while(c != 3); */

/*   return c; */
/* #endif */
/* } */
  


/*     c = getchar(); */

/*     if (isprint(c)) */
/*       fprintf(stdout, "Received normal ch '%c', code %d = 0%03o = 0x%02x\n", c, c, c, c); */
/*     else if (c == ESC) { */
/*       c = getchar(); */
/*       if ( c == '[' ) { */
/* 	c = getchar(); */
/* 	switch (c) { */
/* 	case 'A':  */
/* 	  fprintf(stdout, "Received UP ('%c'), code %d = 0%03o = 0x%02x\n", c, c, c, c); */
/* 	  c = UP; */
/* 	  break; */
	  
/* 	case 'B':  */
/* 	  fprintf(stdout, "Received DOWN ('%c'), code %d = 0%03o = 0x%02x\n", c, c, c, c); */
/* 	  c = DN; */
/* 	  break; */
	  
/* 	case 'C':  */
/* 	  fprintf(stdout, "Received RIGHT ('%c'), code %d = 0%03o = 0x%02x\n", c, c, c, c); */
/* 	  c = DN; */
/* 	  break; */
	  
/* 	case 'D':  */
/* 	  fprintf(stdout, "Received LEFT ('%c'), code %d = 0%03o = 0x%02x\n", c, c, c, c); */
/* 	  break; */
	  
/* 	case 'H':  */
/* 	  fprintf(stdout, "Received HOME ('%c'), code %d = 0%03o = 0x%02x\n", c, c, c, c); */
/* 	  c = HOME; */
/* 	  break; */
	  
/* 	case 'F':  */
/* 	  fprintf(stdout, "Received END ('%c'), code %d = 0%03o = 0x%02x\n", c, c, c, c); */
/* 	  c = END; */
/* 	  break; */
          
/* 	case '2':  */
/* 	  fprintf(stdout, "Received INS ('%c'), code %d = 0%03o = 0x%02x\n", c, c, c, c); */
/* 	  c = INS; */
/* 	  break; */
	  
/* 	case '3':  */
/* 	  fprintf(stdout, "Received DEL ('%c'), code %d = 0%03o = 0x%02x\n", c, c, c, c); */
/* 	  c = DEL; */
/* 	  break; */
	  
/* 	case '5':  */
/* 	  fprintf(stdout, "Received PGUP ('%c'), code %d = 0%03o = 0x%02x\n", c, c, c, c); */
/* 	  c = PGUP; */
/* 	  break; */
	  
/* 	case '6':  */
/* 	  fprintf(stdout, "Received PGDN ('%c'), code %d = 0%03o = 0x%02x\n", c, c, c, c); */
/* 	  c = PGDN; */
/* 	  break; */
	  
/* 	default: */
/* 	  ungetc(c, stdin); */
/* 	} */
/*       }	 */
/*     } */
      
/*     /\* Restore terminal to original state. *\/ */
/*     if (stream_restore(stdin, &saved)) { */
/*         fprintf(stderr, "Cannot restore standard input state: %s.\n", strerror(errno)); */
/*         return EXIT_FAILURE; */
/*     } */
    
/*     /\* i = 0; *\/ */
/*     /\* do { *\/ */
/*     /\*   c = getchar(); /\\* Or c = getc(stdin); *\\/ *\/ */
/*     /\*   if (isprint(c)) { *\/ */
/*     /\* 	cursor(24,24); /\\* posiciona o cursor para imprimir msgs de depuracao *\\/ *\/ */
/*     /\* 	fprintf(stdout, "Received character '%c', code %d = 0%03o = 0x%02x\n", c, c, c, c); *\/ */
/*     /\* 	cursor(x, y); /\\* retorna o cursor para o lugar depois de imprimir msg de depuracao *\\/ *\/ */
/*     /\* 	i = 0; *\/ */
/*     /\*   } *\/ */
/*     /\*   else { *\/ */
/*     /\* 	cursor(24,24+i); /\\* posiciona o cursor para imprimir msgs de depuracao *\\/ *\/ */
/*     /\* 	fprintf(stdout, "Received code %d = 0%03o = 0x%02x\n", c, c, c); *\/ */
/*     /\* 	i++; *\/ */
/*     /\*   } *\/ */
/*     /\*   fflush(stdout); *\/ */
/*     /\* } while (c != 'Q'); *\/ */
    
/*     /\* codigo que usei inicialmente  *\/ */
/*     /\* system("stty -echo raw"); *\/ */
/*     /\* c = getchar(); *\/ */
/*     /\* printf("* %x -  %d - %c  *", c, c, c); *\/ */
/*     /\* system("stty echo cooked"); *\/ */

