/* ---------------------- dbsize.c ------------------------- */
/*	Compute the character size of a database */

#include <stdio.h>
#include "cdata.h"

void index_m();

main()
{
	int f, x;
	long rct [MXFILS];			/* number of records/file 	*/
	long fsize [MXFILS];		/* file sizes 				*/
	long dsize = 0;				/* data base sizes 			*/
	int m [MXINDEX];			/* btree m values 			*/
	long xsize [MXFILS] [MXINDEX];	/* index sizes 			*/

	for (f = 0; dbfiles [f]; f++)	{
		printf("\nEnter record count for %-10s: ",dbfiles[f]);
		fflush(stdout);
		scanf("%ld", &rct [f]);
		fsize [f] = rct [f] * rlen(f) + sizeof(FHEADER);
		printf("File size:                       %10ld",
							fsize [f]);
		dsize += fsize [f];
		index_m(f, m);
		for (x = 0; m [x]; x++)	{
			xsize [f] [x] = (2 + (rct [f] / m [x])) * NODE;
			dsize += xsize [f] [x];
			printf(
				"\nIndex %d (m=%2d) size:             %10ld",
					x+1,m[x],xsize[f][x]);
		}
	}
	printf("\n                                 ----------");
	printf("\nData base size:                  %10ld", dsize);
}



/* --- compute the btree m values
			for the indices of a data base file --- */
static void index_m(f, m)
int f;			/* file number */
int *m;			/* array of m values */
{
	int x, x1;
	int len;

	for (x = 0; x < MXINDEX; x++)	{
		*m = 0;
		if (index_ele [f] [x] [0] == 0)
			break;
		len = 0;
		for (x1 = 0; index_ele [f] [x] [x1]; x1++)
			len += ellen [index_ele [f] [x] [x1] - 1];
		*m++ = ((NODE-(sizeof(int)*2)
				-sizeof(RPTR)*4))/(len+sizeof(RPTR));
	}
}
