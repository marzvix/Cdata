/* -- pg 206 ------- ellist .c -------------------------- */

/*
 * Construct list of ELEMENTs from list of names
 * such as might be entered on a command line.
 * Return OK, or ERROR of one of the names is not in the
 * dictionary.
 */

#include <stdio.h>
#include <string.h>
#include "cdata.h"

int ellist(int count, char *names[], ELEMENT *list)
{
  char elname[31];
  ELEMENT el, el1;

  for (el = 0; el < count; el++)    {
    for (el1 = 0; ; el1++)    {
      if (denames[el1] == NULL)    {
	fprintf(stderr,
		"\nNo such data element as %s", elname);
	return ERROR;
      }
      name_cvt(elname, names[el]);
      if (strcmp(elname,  denames[el1]) == 0)
	break;
    }
    *list++ = el1 + 1;
  }
  *list = 0;
  return OK;
}
