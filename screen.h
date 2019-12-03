/* -- pg 207 ---- screen.h ------------------------------- */

#ifndef SCREEN_H
#define SCREEN_H

#include "sys.h"

/* ---------- screen driver function definitions --------- */
void init_screen(char *, const ELEMENT *, char *);
void protect(ELEMENT, int);
void edit(ELEMENT, int (*)());
void display_template(void);
int data_entry(void);
int spaces(char *);
void tally(void);
void put_field(ELEMENT);
void error_message(char *);
void clear_notice(void);
void post_notice(char *);

#endif
