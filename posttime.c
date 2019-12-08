/* -- pg 253 ---- postime.c ------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cbs.h"
#include "screen.h"
#include "keys.h"

static char *sc;
static DBFILE fl[] =
  {CLIENTS, CONSULTANTS, PROJECTS, ASSIGNMENTS, -1};
static ELEMENT els[] =
  {CONSULTANT_NO, CONSULTANT_NAME,
   PROJECT_NO, PROJECT_NAME, HOURS, EXPENSE, 0};
struct clients cl;
struct consultants cs;
struct projects pr;
struct assignments as;

void main(void)
{
  int term = '\0';
  int edcons(), edproj();
  long atol();
  long exp, rt, time_chg;
  int hrs;

  db_open("", fl);
  sc = malloc(epos(0,els));
  clrrcd(sc , els);
  init_screen("Time & Expenses", els, sc);
  edit(CONSULTANT_NO, edcons);    /* consultant number    */
  protect(CONSULTANT_NAME, TRUE); /* consultant name      */
  edit(PROJECT_NO, edproj);       /* project number       */
  protect(PROJECT_NAME, TRUE);    /* project name         */
  while (term != ESC)    {
    term = data_entry();
    switch (term) {
      /* ------------ GO ------------ */
    case F1:
      hrs = atoi(sc + epos(HOURS, els));
      exp = atol(sc + epos(EXPENSE, els));
      rt = atol(as.rate);
      time_chg = rt * hrs;
      clrrcd(sc, els);
      sprintf(cl.amt_due, "%ld",
	      atol(cl.amt_due)+time_chg+exp);
      rtn_rcd(CLIENTS, &cl);
      sprintf(pr.amt_expended, "%9ld",
	      atol(pr.amt_expended)+time_chg+exp);
      rtn_rcd(PROJECTS, &pr);
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

/* -------- consultant number -------- */
int edcons(char *s)
{
  if (find_rcd(CONSULTANTS, 1, s, &cs) == ERROR)     {
    dberror();
    return ERROR;
  }
  rcd_fill(&cs, sc, file_ele[CONSULTANTS], els);
  put_field(CONSULTANT_NAME);
  return OK;
}

/* -------- project number -------- */
int edproj(char *s)
{
  char consproj[11];
  
  if (find_rcd(PROJECTS, 1, s, &pr) == ERROR)     {
    dberror();
    return ERROR;
  }
  rcd_fill(&pr, sc, file_ele[PROJECTS], els);
  put_field(PROJECT_NAME);
  strcpy(consproj, cs.consultant_no);
  strcat(consproj, pr.project_no);
  if (find_rcd(ASSIGNMENTS, 1, consproj, &as) == ERROR)    {
    error_message("Consultant not assigned to project");
    return ERROR;
  }
  find_rcd(CLIENTS, 1, pr.client_no, &cl);
  return OK;
}
