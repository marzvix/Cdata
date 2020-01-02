#include <stdio.h>
#include "keys.h"
#include "ttcon.h"

int main(int argc, char *argv[])
{
  int c;
  int buf[12];
  int i;
  
  set_conio_terminal_mode();

    /* while (!kbhit()) { */
    /*     /\* do some work *\/ */
    /* } */
  i = 0;
  while(c = getch()) {
    if (c == 3)
      break;
    
    if (c == 27)
      buf[i++] = c;
    else if ( i > 0)
      buf[i++] = c;

    switch (i) {
      
    case 3:
      if (buf[0] == 27) {
	switch (buf[1]) {
	case 91: /* setas */	  
	  switch (buf[2]) {
	  case 65: puts("achou UP"); i = 0; break;
	  case 66: puts("achou DN"); i = 0; break;
	  case 67: puts("achou RG"); i = 0; break;
	  case 68: puts("achou LF"); i = 0; break;
	  case 70: puts("achou END"); i = 0; break;
	  case 72: puts("achou HOME"); i = 0; break;
    //	  default: printf(". %d . %d . %d\n", buf[0], buf[1], c); break;
	  }
	  break;
	  
	case 79:  /* F1~F4 */
	  //	  i = 0;
	  switch (buf[2]) {
	  case 80: puts("achou F1"); i = 0; break;
	  case 81: puts("achou F2"); i = 0; break;
	  case 82: puts("achou F3"); i = 0; break;
	  case 83: puts("achou F4"); i = 0; break;
	  }
	  break;
	}
      }	  
      /* else */
      /* 	  printf("- %d - \n - %d -\n", buf[1], c);  /\* seq de 3 not id *\/ */
      /* //      i = 0; */
	break;

    case 4:
      if (buf[0] == 27 && buf[3] == 126) {
	switch (buf[1]) {
	case 91:
	  switch (buf[2]) {
	  case 50:  puts("achou INS"); i = 0; break;
	  case 51:  puts("achou DEL"); i = 0; break;
	  case 53:  puts("achou PGUP"); i = 0; break;
	  case 54:  puts("achou PGDN"); i = 0; break;
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
	  case 56:  printf("achou F7 %d %d %d %d %d",
			   buf[0],buf[1],buf[2],buf[3],buf[4]
			   ); i = 0; break;
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

    default:
      if (i >= 5)
	i = 0;
      if (i == 0)
	printf("\r-- %d --\n", c); /* consume the character */
      
    };
  }
}
