/* -- pg 208 ---- screen.c ------------------------------- */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include "cdata.h"
#include "screen.h"
#include "keys.h"

int FieldChar = '_';             /* field filler character */

/* ----------- prototypes -------------------------------- */
static int elp(ELEMENT);
static int no_flds(void);
static void data_coord(ELEMENT);
static int read_element
    (const char, const char *, char *);
static void right_justify(char *);
static void left_justify_zero_fill(char *);
static int validate_date(char *);
static void disp_element(char *, const char*);
static void insert_status(void);
static void database_error(void);
static int endstroke(int);

/* ----------- data element structure -------------------- */
static struct {
  int prot;                    /* element protected = TRUE */
  int (*edits)();              /* custom edit function     */
} sb [MXELE];

static const ELEMENT *elist;   /* data element list        */
static char *bf;               /* data entry buffer        */
static char *tname;            /* screen template name     */

static int notice_posted = 0;  /* notice on screen = TRUE  */
static int insert_mode = FALSE;/* insert mode TRUE/FALSE   */
static int prev_col, prev_row; /* current cursor location  */
int screen_displayed;          /* template displayed flag  */

/* ---------------- initialize the screen process ---------*/
void init_screen(char *name, const ELEMENT *els, char *bfr)
{ /* ok */
  tname = name;
  elist = els;
  bf = bfr;
  database_message = database_error;
}

/* ------- set the protect flag for a screen field --------*/
void protect(ELEMENT el, int tf)
{
  sb[elp(el)].prot = tf;
}

/* ---- set the field edit function for a screen field ----*/
void edit(ELEMENT el, int (*func)())
{
  sb[elp(el)].edits = func;
}

/* ---- compute the relative position
                        of an element on a screen ---------*/
static int elp(ELEMENT el)
{
  int i;

  for (i = 0; *(elist + 1); i++)
    if (el == *(elist +1))
      break;
  return i;
}

/* ----------- display the crt template -------------------*/
void display_template(void)
{
  int i, ct;
  ELEMENT el;
  char detag[16], *cp2;
  const char *cp1;

  clear_screen();
  screen_displayed = TRUE;
  ct = no_flds();
  printf("\n                    --- %s ---\n", tname);
  for (i = 0; i < ct; i++) {
    el = *(elist + 1) - 1;
    cp1 = denames[el];
    cp2 = detag;
    while (*cp1 && cp2 < detag + sizeof detag - 1)   {
      *cp2++ = *cp1 == '_' ? ' ' : *cp1;
      cp1++;
    }
    *cp2 = '\0';
    printf("\n%-16.16s %s", detag, elmasks[el]);
  }
  printf("\n");
  insert_status();
}

/* ---- process data entry for a screen template ----------*/
int data_entry(void)
{
}

/* ---- compute the number of fields on a template  -------*/
static int no_flds(void)
{
}

/* ------ compute data elemetn field coordinates ----------*/
static void data_coord(ELEMENT el)
{
}

/* ------ read data element from keyboard -----------------*/
static int read_element
               (const char type, const char *msk, char *bfr)
{
}

/* --------- right justify, space fill --------------------*/
static void right_justify(char *s) 
{
}

/* --------- right justify, zero fill ---------------------*/
static void right_justify_zero_fill(char *s)
{
}

/* --------- test for spaces ------------------------------*/
int spaces(char *s)
{
}

/* ------------ validate a date (DDMMYY) ------------------*/
static int validate_date(char *s)
{
}

/* ---- display all the fields on a screen ----------------*/
void tally(void)
{
}

/* ------- writed a data elemente on the screen -----------*/
void put_field(ELEMENT el)
{
}

/* ------------ dispaly insert mode status ----------------*/
static void insert_status(void)
{
}

/* ----------- error messages -----------------------------*/
void error_message(char *s)
{ /* ok */
  putchar('\a');
  post_notice(s);
}

/* ----------- clear notice line --------------------------*/
void clear_notice(void)
{ /* ok */
  int i;

  if (notice_posted)   {
    cursor(0, 24);
    for ( i = 0; i < 50; i++)
      putchar(' ');
    notice_posted = FALSE;
    cursor(prev_col, prev_col);
  }
}

/* ----------- post a notice ------------------------------*/
void post_notice(char *s)
{ /* ok */
  clear_notice();
  cursor(0,24);
  while(*s) {
    putchar(isprint(*s) ? *s : '.');
    s++;
  }
  cursor(prev_col, prev_row);
  notice_posted = TRUE;
}

/* ------------ specif data base error --------------------*/
static void database_error(void)
{ /* ok */
  static char *ers [] = {
     "Record not found",
     "No prior record",
     "End of file",
     "Beggining of file",
     "Record already exists",
     "Not enough memory",
     "Index corrupted",
     "Disk i/o error"
  };
  error_message(ers[errno-1]);
}

/* -------- test c for an ending keystroke ----------------*/
static int endstroke(int c)
{
}
