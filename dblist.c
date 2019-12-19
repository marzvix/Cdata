/* --------------------- dblist.c ------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include "cdata.h"
/* extern char *malloc(); */
/* extern int free(); */

void dblist(fd, inter, f, k, list)
FILE *fd;	/* output file */
int inter;	/* true for user interaction and page breaks */
int f;		/* file number */
int k;		/* key number  */
int *list;	/* list of elements */
{
	char *bf;
	int rcdct = 0;
	extern void clist(), test_eop(), oflow();
	bf = malloc(rlen(f));
	errno = 0;
	if (k)
		first_rcd(f, k, bf);
	if (inter == 0)
		oflow(fd, FALSE, dbfiles [f], list);
	while (errno != D_EOF)	{
		if (k)	{
			clist(fd,inter,file_ele[f],list,bf,dbfiles[f]);
			rcdct++;
			next_rcd(f, k, bf);
		}
		else if (seqrcd(f, bf) != ERROR)	{
			clist(fd,inter,file_ele[f],list,bf,dbfiles[f]);
			rcdct++;
		}
	}
	if (inter)	{
		test_eop(fd, dbfiles [f], list);
		fprintf(fd, "\nRecords: %d\n", rcdct);
	}
	free(bf);
}
