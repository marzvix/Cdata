/* -- pg 229 ----- filename.c --------------------------- */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cdata.h"

/*
 * Convert a filename into its file token.
 * Return the token,
 * or ERROR if the file name is not in the scheme.
 */
DBFILE filename(char *fn)
{
  char fname[32];
  DBFILE f;
  void name_cvt();

  name_cvt(fname, fn);
  for (f = 0; dbfiles [f]; f++)
    if (strcmp(fname, dbfiles [f]) == 0)
      break;
  if (dbfiles [f] == 0)   {
    fprintf(stderr, "\nNo such file as %s", fname);
    return ERROR;
  }
  return f;
}

/* ---------- convert a name to upper case -------------- */
void name_cvt(char *c2, char *c1)
{
  while (*c1) {
    *c2 = toupper(*c1);
    c1++;
    c2++;
  }
  *c2 = '\0';
}
