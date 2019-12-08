/* -- pg 256 ---- payments.c ----------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "cbs.h"
#include "screen.h"
#include "keys.h"

static char *sc;
static DBFILE fl[] = {CLIENTS, -1};
static ELEMENT els[] =
  {CLIENT_NO, CLIENT_NAME, AMT_DUE, PAYMENT, DATE_PAID, 0};
struct clients cl;
int len;

void main(void)
{
  int term = '\0';
  int edcl();
  long atol();

  db_open("", fl);
  len = epos(0, els);
  sc = malloc(len);
  clrrcd(sc, els);
  init_screen("Payments", els, sc);
  edit(CLIENT_NO, edcl);
  protect(CLIENT_NAME, TRUE);
  protect(AMT_DUE, TRUE);
  while (term != ESC)     {
    term = data_entry();
    switch (term)    {
    case F1:
      if (atol(cl.amt_due) <
	  atol(sc + epos(PAYMENT, els)))    {
	error_message("Overpayment");
	continue;
      }
      sprintf(cl.amt_due, "%8ld",
	      atol(cl.amt_due)
	      -atol(sc+epos(PAYMENT,els)));
      clrrcd(sc,els);
      rtn_rcd(CLIENTS, &cl);
      break;
    case ESC:
      if (spaces(sc))
	break;
      clrrcd(sc, els);
      term = '\0';
      break;
    default:
      break;
    }
  }
  clear_screen();
  free(sc);
  db_cls();
}

/* ------ client number -------- */
int edcl(char *s)
{
  if (find_rcd(CLIENTS, 1, s, &cl) == ERROR)     {
    dberror();
    return ERROR;
  }
  rcd_fill(&cl, sc, file_ele[CLIENTS], els);
  tally();
  return OK;
}
