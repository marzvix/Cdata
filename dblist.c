/* -- pg 224 ----- dblist.c ----------- */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "cdata.h"

void dblist(FILE *fd, DBFILE f, int k, const ELEMENT *list)
{
  char *bf;
  int rcdct = 0;
  if ((bf = malloc(rlen(f))) != NULL) {
    errno = 0;
    if (k)
      first_rcd(f, k, bf);
    while (errno != D_EOF)    {
      if (k)    {
	clist(fd,file_ele[f],list,bf,dbfiles[f]);
	rcdct++;
	next_rcd(f, k, bf);
      }
      else if (seqrcd(f, bf) != ERROR)    {
	clist(fd,file_ele[f],list,bf,dbfiles[f]);
	rcdct++;
      }
    }
    test_eop(fd, dbfiles [f], list);
    fprintf(fd, "\nRecords: %s\n", rcdct);
    free(bf);
  }
}
