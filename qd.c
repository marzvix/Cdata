/* -- pg 191 ------ qd.c --------------------------------- */
/* A query program.  Enter the name of the data base file and
 * (optionally) a list of data elements. A screen is buit
 * and the file is update based upon the data entered.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cdata.h"
#include "screen.h"
#include "keys.h"

static int file;
static int existing_record;
static char *rb;                 /* record buffer     */
static char *hb;                 /* hold buffer       */
static char *sc;                 /* screen buffer     */
static char len;                 /* record length     */
static int fl[] = {0, -1};
static int iplist [MXELE+1];
static const int *els;

/* ---------------- prototypes -------------------- */
static void query(void);
static void clear_record(void);
static void rcdin(void);
static void rcdout(void);
static int empty(char *, int);
static int same(void);
static void set_trap(void);
static int key_entry(char *);
#pragma GCC diagnostic ignored "-Wmain"
void main(int argc, char *argv[])
{
  if (argc > 1)    {
    if ((file = filename(argv[1])) != ERROR)    {
      if (argc == 2)    {
	len = rlen(file);
	els = (int *) file_ele [file];
      }
      else if (ellist(argc-2, argv+2, iplist) == OK) {
	len = epos(0, iplist);
	els = iplist;
      }
      else
	exit(1);
      sc = malloc(len);        /* screen buffer */
      rb = malloc(rlen(file)); /* record buffer */
      hb = malloc(rlen(file)); /* hold buffer   */
      if (sc == NULL || rb == NULL || hb == NULL) {
	printf("\nOut of memory");
	exit(1);
      }
      init_rcd(file, rb);
      init_rcd(file, hb);
      init_keys(); init_screen(argv[1], els, sc); 
      query(); 
      free(hb);
      free(rb);
      free(sc);
    }
  }
}

/* ------------------- process the query ----------------- */
static void query(void)
{
  int term = 0;

  *fl = file;
  db_open("", fl);
  clrrcd(sc, els);
  set_trap();
  while (term != F2) {
    term = data_entry();
    switch (term)    {
    /* ------------ GO ---------------- */
    case F1: rcdout();
      break;
    /* ------- First record ----------- */
    case HOME:  rcdout();
      if (first_rcd(file, 1, rb) == ERROR)
      	post_notice("Empty file");
      else
      	rcdin();
      break;
    /* ------- Last record ------------ */
    case END: rcdout();
      if (last_rcd(file, 1, rb) == ERROR)
	post_notice("Empty file");
      else
	rcdin();
      break;
    /* ------- previous record -------- */
    case PGUP:  rcdout();
      if (prev_rcd(file, 1, rb) == ERROR) {
	post_notice("Beginning of file");
	if (first_rcd(file, 1, rb) ==
	    ERROR)   {
	  post_notice("Empty file");
	  break;
	}
      }
      rcdin();
      break;
    /* ------- next record ------------ */
    case PGDN:  rcdout();
      if (next_rcd(file, 1, rb) == ERROR)    {
	post_notice("At end of file");
	if (last_rcd(file, 1, rb) ==
	    ERROR)    {
	  post_notice("Empty file");
	  break;
	}
      }
      rcdin();
      break;
      /* ------- delete record ------------ */
    case F4:  if (spaces(rb)==0) {
	post_notice("Verify w/F4");
	if (get_char() == F4)    {
	  del_rcd(file);
	  clear_record();
	}
	clear_notice();
      }
      break;
    case F3: if (spaces(sc))
	break;
      clear_record();
      term = 0;
      break;
    default: break;
    }
  }
 clear_screen();
 db_cls();
}

/* ------------- clear the record area ------------------  */
static void clear_record(void)
{
  int i = 0;
  while (index_ele [file] [0] [i])
    protect(index_ele[file][0][i++],FALSE);
  clrrcd(sc, els);
  existing_record = FALSE;
}

/* --------- get the data base file record -------------- */
static void rcdin(void)
{
  int i = 0;

  if (empty(rb, rlen(file)) == 0) {
    rcd_fill(rb, sc, file_ele[file], els);
    memmove(hb, rb, rlen(file));
    existing_record = TRUE;
    while (index_ele [file] [0] [i])
      protect(index_ele[file] [0] [i++],TRUE);
  }
}

/* --------- add update the data base file record ------- */
static void rcdout(void)
{
  if (empty(sc, len) == 0)    {

    rcd_fill(sc, rb, els, file_ele[file]);
    if (existing_record)    {
      if (same() == 0)    {
	post_notice("Returning record");
	rtn_rcd(file, rb);
      }
    }
    else    {
      post_notice("New record added");
      if (add_rcd(file, rb) == ERROR)
	dberror();
    }
    clear_record();

  }
}

/* ----------- test or an empty record buffer ----------- */
static int empty(char *b, int l)
{
  while (l--)
    if (*b && *b != ' ')
      return FALSE;
    else
      b++;
  return TRUE;
}

/* ------------ teste two buffer for equality ----------- */
static int same(void)
{
  int ln = rlen(file);

  while (--ln)
    if (*(rb + ln) != *(hb + ln))
      break;
  return (*(rb + ln) == *(hb + ln));
}

/* ---------- set the query screen's key element trap --- */
static void set_trap(void)
{
  int i = 0;

  while (index_ele [file] [0] [i])
    i++;
  edit(index_ele [file] [0] [i-1], key_entry);
}

/* --- come here when the primary key has been entered ---- */
static int key_entry(char *s)
{
  char key [MXKEYLEN];
  int i;

  if (spaces(s))
    return OK;
  *key = '\0';
  i = 0;
  while (index_ele [file] [0] [i])    {
    protect(index_ele[file][0][i],TRUE);
    strcat(key, sc + epos(index_ele[file][0][i++], els));
  }
  if (find_rcd(file, 1, key, rb) == ERROR)    {
    post_notice("New record");
    existing_record = FALSE;
    return OK;
  }
  rcdin();
  tally();
  return OK;
}
