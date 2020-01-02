#include <stdio.h>
#include "keys.h"
#include "ttcon.h"

static int buf[8];

void init_keys(void)
{
  set_conio_terminal_mode(); 
};

int getch_t(void)  /* wrapper to getch: get key translated */
{
  int i, c;
  
  i = 0;
  while(c = getch()) {
    if (c == CTRL_C)
      return c;
    
    if (c == ESC)
      buf[i++] = c;
    else if ( i > 0)
      buf[i++] = c;

    switch (i) {
      
    case 3:
      if (buf[0] == ESC) {
	switch (buf[1]) {
	case 91: /* setas */	  
	  switch (buf[2]) {
	  case 65: return UP;
	  case 66: return DN;
	  case 67: return BWD;
	  case 68: return FWD;
	  case 70: return END;
	  case 72: return HOME;
    //	  default: printf(". %d . %d . %d\n", buf[0], buf[1], c); break;
	  }
	  break;
	  
	case 79:  /* F1~F4 */
	  //	  i = 0;
	  switch (buf[2]) {
	  case 80: return F1;
	  case 81: return F2;
	  case 82: return F3;
	  case 83: return F4;
	  }
	  break;
	}
      }	  
      /* else */
      /* 	  printf("- %d - \n - %d -\n", buf[1], c);  /\* seq de 3 not id *\/ */
      /* //      i = 0; */
	break;

    case 4:
      if (buf[0] == ESC && buf[3] == 126) {
	switch (buf[1]) {
	case 91:
	  switch (buf[2]) {
	  case 50:  return INS;
	  case 51:  return DEL;
	  case 53:  return PGUP;
	  case 54:  return PGDN;
	  default:  printf("+ %d + \n + %d +\n", buf[1], buf[2]); 
	    break;
	  }
	}
      }
      /* else */
      /* 	  printf("# %d # \n # %d #\n", buf[3], c);  /\* seq de 5 not id *\/ */
      //      i = 0;
      break;
      
    case 5:
      if (buf[0] == 27 && buf[1] == 91 && buf[2] == 49 && buf[4] == 126) {
	  switch (buf[3]) {
	  case 53:  printf("achou F5 %d %d %d %d %d",
			   buf[0],buf[1],buf[2],buf[3],buf[4]
			   ); i = 0; break;
	  case 55:  printf("achou F6 %d %d %d %d %d",
			   buf[0],buf[1],buf[2],buf[3],buf[4]
			   ); i = 0; break;
	  case 56:  return F7;/*  printf("achou F7 %d %d %d %d %d", */
			   /* buf[0],buf[1],buf[2],buf[3],buf[4] */
			   /* ); i = 0; break; */
	  case 57:  printf("achou F8 %d %d %d %d %d",
			   buf[0],buf[1],buf[2],buf[3],buf[4]
			   ); i = 0; break;
	  default:  printf("; %d ; \n", buf[3]); 
	    break;
	  }
      }
      else if (buf[0] == 27 && buf[1] == 91 && buf[2] == 50 && buf[4] == 126){
	  switch (buf[3]) {
	  case 48:  printf("achou F9 %d %d %d %d %d",
			   buf[0],buf[1],buf[2],buf[3],buf[4]
			   ); i = 0; break;
	  case 49:  printf("achou F10 %d %d %d %d %d",
			   buf[0],buf[1],buf[2],buf[3],buf[4]
			   ); i = 0; break;
	  case 51:  printf("achou F11 %d %d %d %d %d",
			   buf[0],buf[1],buf[2],buf[3],buf[4]
			   ); i = 0; break;
	  case 52:  printf("achou F12 %d %d %d %d %d",
			   buf[0],buf[1],buf[2],buf[3],buf[4]
			   ); i = 0; break;
	  default:  printf("x %d x \n", buf[3]); 
	    break;
	  }
      }
      else
	printf("(5) %d %d %d %d %d",
	       buf[0],buf[1],buf[2],buf[3],buf[4]);  /* seq de 5 not id */
      //      i = 0;
      break;

    case 6:
      if (buf[0] == 27 && buf[1] == 91 && buf[2] == 49 && buf[3] == 59 && buf[4] == 50) {
      	  switch (buf[5]) {
      	  case 80:  printf("achou S-F1 %d %d %d %d %d",
      			   buf[0],buf[1],buf[2],buf[3],buf[4]
      			   ); i = 0; break;
      	  case 81:  printf("achou S-F2 %d %d %d %d %d",
      			   buf[0],buf[1],buf[2],buf[3],buf[4]
      			   ); i = 0; break;
      	  case 82:  printf("achou S-F3 %d %d %d %d %d",
      			   buf[0],buf[1],buf[2],buf[3],buf[4]
      			   ); i = 0; break;
      	  case 83:  printf("achou S-F4 %d %d %d %d %d",
      			   buf[0],buf[1],buf[2],buf[3],buf[4]
      			   ); i = 0; break;
      	  default:  printf("; %d ; \n", buf[3]);
      	    break;
      	  }
      }
      if (buf[0] == 27 && buf[1] == 91 && buf[2] == 49 && buf[3] == 59 && buf[4] == 53) {
      	  switch (buf[5]) {
      	  case 80:  printf("achou C-F1 %d %d %d %d %d",
      			   buf[0],buf[1],buf[2],buf[3],buf[4]
      			   ); i = 0; break;
      	  case 81:  printf("achou C-F2 %d %d %d %d %d",
      			   buf[0],buf[1],buf[2],buf[3],buf[4]
      			   ); i = 0; break;
      	  case 82:  printf("achou C-F3 %d %d %d %d %d",
      			   buf[0],buf[1],buf[2],buf[3],buf[4]
      			   ); i = 0; break;
      	  case 83:  printf("achou C-F4 %d %d %d %d %d",
      			   buf[0],buf[1],buf[2],buf[3],buf[4]
      			   ); i = 0; break;
      	  default:  printf("; %d ; \n", buf[3]);
      	    break;
      	  }
      }
      break; /* case 5 */

    case 0:
      switch (c) {
      case 127: return BS;
      default:
	//	printf("\r-- %d --\n", c); /* consume the character */
	return c;
      }
      
    default:
      if (i >= 5)
	i = 0;
    };
  }
  return c;
}
