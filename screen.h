/* -- pg 207 ---- screen.h ------------------------------- */

#ifndef SCREEN_H
#define SCREEN_H

#include "sys.h"

/* ---------- screen driver function definitions --------- */
void init_screen(char *, const ELEMENT *, char *); /* ok */
void protect(ELEMENT, int);  /* dg */
void edit(ELEMENT, int (*)()); /* dg */
void display_template(void); 
int data_entry(void);
int spaces(char *);
void tally(void);
void put_field(ELEMENT);
void error_message(char *);  /* ok */
void clear_notice(void);     /* ok */
void post_notice(char *);    /* ok */

#endif
