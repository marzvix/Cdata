#ifndef _TERMCTRL_H
#define _TERMCTRL_H

void init_termview(void);
void enable_rawtty(void);
void disable_rawtty(void);
void get_keypress(void);
int get_tv_size(int *, int *);
int get_cursor_pos(int *, int *);
  
#endif
