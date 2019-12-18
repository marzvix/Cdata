/* -- pg 182 ----- sys.h -------------- */
#ifndef SYS_H
#define SYS_H

#define STDIN_FILENO stdin
#define STDOUT_FILENO stdout


#define WRT_STR(s) write(STDOUT_FILENO, s, strlen(s));
#define WRT_CHR(c) {				\
    int _c=c;					\
    write(STDOUT_FILENO, &_c, sizeof(int));	\
  }

#define ASC_CLS "\033[2J"  /* clear screen */ 
#define ASC_UP "\033[A"    /* up arrow */
#define ASC_DWN "\033[B"   /* down arrow */
#define ASC_FWD "\033[C"   /* right arrow */
#define ASC_BWD "\033[D"   /* left arrow */

#define CTRL_KEY(_c) (_c & 0x1F)  /* and 00011111 */

/* move cursor to y,x coord */
#define ASC_CMD_MOV(y, x)			\
  {						\
    char _pad[32];				\
    sprintf(_pad, "\033[%02d;%02dH", y+1, x+1);	\
    WRT_STR(_pad);				\
  }

void put_char(int);
void cursor(int x, int y);
void clear_screen(void);
int get_char(void);

#endif
