/* -- pg 204 ---- index.c -------------------------- */
#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>
#include "cdata.h"

extern RPTR curr_a[];   /* current record file address */

void main(int argc, char *argv[])
{
  int f = 1, i;
  static int fs [MXFILS+1];
  char *bf, *path;

  if (argc < 2)    {
    printf("\nfile name or 'all' required");
    exit(1);
  }
  if (strcmp("all", argv [1]) == 0)  /* index all */
    for (f = 0; dbfiles [f]; f++) /* put files in list */
      fs [f] = f;
  else if ((*fs = filename(argv[1])) == ERROR)
    exit(1);
  fs [f] = (-1);  /* terminator (file, file, ..., -1) */
  path = argc > 2 ? argv[2] : "";
  /* delete and rebuild indexes
     in the data base files being indexed */
  for (i = 0; (f = fs [i]) != (-1); i++)
    build_index(path, f);
  /* Opend the data base files. */
  db_open(path, fs);
  for (i=0; (f = fs [i]) != (-1); i++)    {
    printf("\nIndexing %s", dbfiles [f]);
    if ((bf = malloc(rlen(f))) != NULL)   {
      while (seqrcd(f, bf) != ERROR)
	add_indexes(f, bf, curr_a [f]);
      free(bf);
    }
  }
  db_cls();
}
