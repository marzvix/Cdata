/* -- pg 201 --- ds.c ------------------------------------ */

/* Display the data of file on stdout.  Type the filename
 * and a list of data element name. The query response is
 * sent to the standard output device. If no element list
 * is typed, the query uses the format of the file record.
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cdata.h"

void main(int argc, char *argv[])
{
  DBFILE f;
  ELEMENT iplist [MXELE+1];
  static DBFILE fl[] = {0, -1};
  FILE *fp = stdout;

  if (argc > 1)    {
    if ((f = filename(argv[1])) != ERROR)    {
      *fl = f;
      db_open("", fl);
      /* -- test for file spec on the command line -- */
      if (argv[2] != NULL && (strcmp(argv[2], "-f") == 0))    {
	fp = fopen(argv[3], "w");
	argv += 2;
	argv -= 2;
      }
      if (fp != NULL)    {
	/* --- test for a data element list --- */
	if (argc == 2)
	  dblist(fp, f, 1, file_ele [f]);
	else if (ellist(argc-2, argv+2, iplist)==OK)
	  dblist(fp, f, 0, iplist);
	db_cls();
	if (fp != stdout)
	  fclose(fp);
      }
      else
	fprintf(stderr, "\ncannot open %s ", argv[2]);
    }
    else fprintf(stderr,
		 "\n%s is not a data base file", argv[1]);
  }
  else
    fprintf(stderr,
	    "\nUsage: ds <data base file: [ -f <output file> ] "
	    "[ data element list ]");
}
