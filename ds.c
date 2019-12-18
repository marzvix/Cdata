/* ------------------------- ds.c ------------------------ */

/*  Display the data of a file on stdout.  Type the filename 
 *  and a list of data element names.  The query response is 
 *  sent to the standard output device.  If no element list 
 *  is typed, the query uses the format of the file record.
 */

#include <stdio.h>
#include "cdata.h"

main(argc, argv)
int argc;
char *argv[];
{
	int f, iplist [MXELE+1];
	static int fl [] = {0, -1};
	extern void dblist();

	if (argc > 1)	{
		if ((f = filename(argv[1])) != ERROR)	{
			*fl = f;
			db_open("", fl);
			if (argc == 2)
				dblist(stdout, TRUE, f, 1, file_ele [f]);
			else if (ellist(argc - 2, argv + 2, iplist) == OK)
				dblist(stdout, TRUE, f, 0, iplist);
			db_cls();
		}
	}
}

