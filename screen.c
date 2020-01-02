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
static void right_justify_zero_fill(char *);
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
{
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

  for (i = 0; *(elist + i); i++)
    if (el == *(elist + i))
      break;
  return i;
}

/* ----------- display the crt template -------------------*/
void display_template(void)
{
  int i, ct;                      /* NOTA: \n\r adicionado */
  ELEMENT el;
  char detag[16], *cp2;
  const char *cp1;

  clear_screen();
  screen_displayed = TRUE;
  ct = no_flds();
  printf("\n\r                    --- %s ---\n\r", tname);
  for (i = 0; i < ct; i++) {
    el = *(elist +  i) - 1;
    cp1 = denames[el];
    cp2 = detag;
    while (*cp1 && cp2 < detag + sizeof detag - 1)   {
      *cp2++ = *cp1 == '_' ? ' ' : *cp1;
      cp1++;
    }
    *cp2 = '\0';
    printf("\n\r%-16.16s %s", detag, elmask[el]);
  }
  printf("\n\r");
  insert_status();
}

/* ---- process data entry for a screen template ----------*/
int data_entry(void)
{
  int (*validfunct)(char *, int);
  int field_ctr, exitcode;
  ELEMENT el;
  int field_ptr = 0, done = FALSE, isvalid;
  char *bfptr;

  if (screen_displayed == 0)
    display_template();  
  tally();
  field_ctr = no_flds();
  /* ---- collect data from keyboard into screen ---- */
  while (done == FALSE)    {
    bfptr = bf + epos(elist[field_ptr], elist);
    el = *(elist + field_ptr) - 1;
    validfunct = sb[field_ptr].edits;
    data_coord(el + 1);
    if (sb[field_ptr].prot == FALSE)     {
      exitcode =
	read_element(eltype[el], elmask[el], bfptr);
      isvalid = (exitcode != ESC && validfunct) ?
	(*validfunct)(bfptr, exitcode) : OK;
    }
    else    {
      exitcode = FWD;
      isvalid = OK;
    }
    if (isvalid == OK)
      switch (exitcode)    { /* passed edit */
      case DN:               /* cursor down key */
      case '\r':             /* enter/return */
      case '\t':             /* horizontal tab */
      case FWD:              /* -> */
	if (field_ptr+1 == field_ctr)
	  field_ptr = 0;
	else
	  field_ptr++;
	break;
      case UP:               /* cursor up key */
      case BS:               /* back space */
	if (field_ptr == 0)
	  field_ptr = field_ctr - 1;
	else
	  field_ptr--;
	break;
      default:
	done = endstroke(exitcode);
	break;
      }
  }
  return (exitcode);
}

/* ---- compute the number of fields on a template  -------*/
static int no_flds(void)
{
  int ct = 0;

  while (*(elist + ct))
    ct++;
  return ct;
}

/* ------ compute data element field coordinates ----------*/
static void data_coord(ELEMENT el)
{
  prev_col = 17;
  prev_row = elp(el) + 3;
}

/* ------ read data element from keyboard -----------------*/
static int read_element
               (const char type, const char *msk, char *bfr)
{
  const char *mask = msk;
  char *buff = bfr;
  int done = FALSE, c, column = prev_col;

  while (*mask != FieldChar)    {
    prev_col++;
    mask++;
  }
  while (TRUE)    {
    cursor(prev_col, prev_row);
    c = get_char();
    clear_notice();
    switch(c)     {
    case '\b':
    case BS:
      if (buff == bfr)    {
	done = c == BS;
	break;
      }
      --buff;
      do    {
	--mask;
	--prev_col;
      } while (*mask != FieldChar);
      /* if (c == BS) */
	break;
    case DEL:
      memmove(buff, buff+1, strlen(buff));
      *(buff+strlen(buff))  = ' ';
      cursor(prev_col, prev_row);
      disp_element(buff, mask);
      break;
    case FWD:
      do {
	prev_col++;
	mask++;
      } while (*mask && *mask != FieldChar);
      buff++;
      break;
    case F4: /* INS */
      insert_mode ^= TRUE;
      insert_status();
      break;
    case '.':
      if (type == 'C')    {
	if (*mask++ && *buff == ' ')    {
	  *buff++ = '0';
	  if(*mask++ && *buff == ' ')
	    *buff++ = '0';
	}
	right_justify(bfr);
	cursor(column, prev_row);
	disp_element(bfr, msk);
	prev_col = column + strlen(mask)-2;
	mask = msk + strlen(msk)-2;
	buff = bfr + strlen(bfr)-2;
	break;
      }
    default:
      if (endstroke(c))    {
	done = TRUE;
	break;
      }
      if (type != 'A' && !isdigit(c))    {
	error_message("Numbers only");
	break;
      }
      if (insert_mode)    {
	memmove(buff+1, buff, strlen(buff)-1);
	disp_element(buff,mask);
      }
      *buff++ = c;
      put_char(c);
      do {
	prev_col++;
	mask++;
      } while (*mask && *mask != FieldChar);
      if (!*mask)
	c = FWD;
      break;
    }
    if (!*mask)
      done = TRUE;
    if (done)    {
      if (type == 'D' &&
	  c != ESC &&
	  validate_date(bfr) != OK)
	return -1;
      break;
    }
  }
  if (c != ESC && type != 'A')    {
    if (type == 'C')    {
      if (*mask++ && *buff == ' ')     {
	*buff++ = '0';
	if (*mask++ && *buff == ' ')
	  *buff++ = '0';
      }
    }
    if (type == 'Z' || type == 'D')
      right_justify_zero_fill(bfr);
    else
      right_justify(bfr);
    cursor(column, prev_row);
    disp_element(bfr, msk);
  }
  return (c);
}

/* --------- right justify, space fill --------------------*/
static void right_justify(char *s) 
{
  int len;

  len = strlen(s);
  while ((*s == ' ' || *s == '0') && len)     {
    len--;
    *s++ = ' ';
  }
  if (len)
    while (*(s+(len-1)) == ' ') {
      memmove(s+1, s, len-1);
      *s = ' ';
    }
}

/* --------- right justify, zero fill ---------------------*/
static void right_justify_zero_fill(char *s)
{
  int len;

  if (spaces(s))
    return;
  len = strlen(s);
  while (*(s + len - 1) == ' ')    {
    memmove(s+1, s, len-1);
    *s = '0';
  }
}

/* --------- test for spaces ------------------------------*/
int spaces(char *c)
{
  while (*c == ' ')
    c++;
  return !*c;
}

/* ------------ validate a date (DDMMYY) ------------------*/
static int validate_date(char *s)
{
  static int days_in_month[] =
    { 31,28,31,30,31,30,31,31,30,31,30,31 };
  char date [7];
  int day, month, year;

  strcpy(date, s);
  if (spaces(date))
    return OK;
  /* ------ extract the year ------ */
  year = atoi(date+4);
  /* ------ extract the month ------ */
  *(date+4) = '\0';
  month = atoi(date+2)-1;
  /* ------ extract the day ------ */
  *(date+2) = '\0';
  day = atoi(date)-1;
  /* ------ leap year adjustment ------ */
  if ((year % 4) == 0)
    days_in_month[1] = 29;
  else
    days_in_month[1] = 28;
  if (month < 12)
    if (day < days_in_month [month])
      return 0L;
  error_message("Invalid date");
    return ERROR;
}

/* ---- display all the fields on a screen ----------------*/
void tally(void)
{
  const ELEMENT *els = elist;

  while (*els)
    put_field(*els++);
}

/* ------- writed a data element on the screen ----------- */
void put_field(ELEMENT el)
{
  data_coord(el);
  cursor(prev_col, prev_row);
  disp_element(bf + epos(el, elist), elmask[el - 1]);
}

/* ------------ display a data element ------------------- */
static void disp_element(char *b, const char *msk)
{
  while (*msk)    {
    int c = *msk != FieldChar ? *msk : *b++;
    put_char(c);
    msk++;
  }
  cursor(prev_col,prev_row);
}

/* ------------ dispaly insert mode status ----------------*/
static void insert_status(void)
{
  cursor(65,24);
  printf(insert_mode ? "[INS]" : "     ");
  cursor(prev_col,prev_row);
}

/* ----------- error messages -----------------------------*/
void error_message(char *s)
{
  putchar('\a');
  post_notice(s);
}

/* ----------- clear notice line --------------------------*/
void clear_notice(void)
{
  int i;

  if (notice_posted)   {
    cursor(0, 24);
    for ( i = 0; i < 50; i++)
      putchar(' ');
    notice_posted = FALSE;
    cursor(prev_col, prev_row);
  }
}

/* ----------- post a notice ------------------------------*/
void post_notice(char *s)
{ 
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
{
  static char *ers [] = {
     "Record not found",
     "No prior record",
     "End of file",
     "Beginning of file",
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
  switch (c) {
    /* ------ implementation-dependent values -------- */

  case F1:         /* function keys */
  case F2:
  case F3:
  case F4:
  case F5:
  case F6:
  case F7:
  case F8:
  case F9:
  case F10:

  case PGUP:      /* page scrolling keys */
  case PGDN:
  case HOME:
  case END:

  case UP:        /* cursor movmente keys */
  case DN:
    
  case '\r':      /* standard ASCII vales */
  case '\n':
  case '\t':
  case ESC:
    return TRUE;
  default:
    return FALSE;
  }
}
