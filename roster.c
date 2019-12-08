/* -- pg 244 ---- roster.c ------------------------- */

/*
 * Sort clients an print a roster
 */

#include <stdio.h>
#include <stdlib.h>
#include "cbs.h"
#include "sort.h"

struct clients cl;

void main(void)
{
  static DBFILE fl[] = {CLIENTS, -1};

  static const ELEMENT els[] = {CLIENT_NAME, 0};
  static const ELEMENT elp[] = {CLIENT_NAME, PHONE, 0};
  struct clients *cls;

  db_open("", fl);
  init_sort(file_ele[CLIENTS], els, TRUE);
  while(TRUE)    {
    if (next_rcd(CLIENTS, 1, &cl) == ERROR)
      break;
    sort(&cl);
  }
  sort(NULL);
  while ((cls = sort_op()) != NULL)
    clist(stdout, file_ele[CLIENTS], elp, cls, "Client Roster");
  db_cls();
}
