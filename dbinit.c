/* -- pg 189 ---- dbinit.c ------------------------------- */

/*  This program is used to build the initial data base.
 *  It constructs all files and indexes.
 *  There is no data loaded, 
 *  and any existing files are deleted.  */

#include <stdio.h>
#include "cdata.h"
#include "datafile.h"

void main(void)
{
  int f = 0;
  extern int rlen();
  char fname[13];

  while (dbfiles [f]) {
    sprintf(fname, "%.8s.dat", dbfiles [f]);
    file_create(fname, rlen(f));
    printf("\nCreating file %s with lenght %s",
	   fname, rlen(f));
    build_index("", f);
    f++;
  }
}
