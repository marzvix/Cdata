/* ---------------- datafile.c -------------------------- */

#include <stdio.h>
#include "cdata.h"

#define flocate(r,l) ((long)(sizeof(FHEADER)+(((r)-1)*(l))))

static int handle [MXFILS];
FHEADER fh [MXFILS];

/* --------- create a file ----------- */
void file_create(name, len)
char *name;
int len;
{
	int fp;
	FHEADER hd;
#if COMPILER == MICROSOFT
	extern int _iomode;
	_iomode = 0x8000;
#endif
#if COMPILER == LATTICE
	extern int _iomode;
	_iomode = 0x8000;
#endif

	unlink(name);
	fp = creat(name, CMODE);
	close(fp);
	fp = open(name, OPENMODE);
	hd.next_record = 1;
	hd.first_record = 0;
	hd.record_length = len;
	write(fp, (char *) &hd, sizeof hd);
	close(fp);
}




/* --------------  open a file ---------------- */
int file_open(name)
char *name;
{
	int fp;
#if COMPILER == MICROSOFT
	extern int _iomode;
	_iomode = 0x8000;
#endif
#if COMPILER == LATTICE
	extern int _iomode;
	_iomode = 0x8000;
#endif

	for (fp = 0; fp < MXFILS; fp++)
		if (handle [fp] == 0)
			break;
	if (fp == MXFILS)
		return ERROR;
	if ((handle [fp] = open(name, OPENMODE)) == ERROR)
		return ERROR;
	lseek(handle [fp], 0L, 0);
	read(handle [fp], (char *) &fh [fp], sizeof(FHEADER));
	return fp;
}

/* --------------- close a file ----------------- */
void file_close(fp)
int fp;
{
	lseek(handle [fp], 0L, 0);
	write(handle [fp], (char *) &fh [fp], sizeof(FHEADER));
	close(handle [fp]);
	handle [fp] = 0;
}





/* -------------- create a new record ------------- */
RPTR new_record(fp, bf)
int fp;
char *bf;
{
	RPTR rcdno;
	extern char *malloc();
	FHEADER *c;
	extern int free();

	if (fh [fp].first_record)	{
		rcdno = fh [fp].first_record;
		if ((c = (FHEADER *)
				malloc(fh [fp].record_length)) == NULL)	{
			errno = D_OM;
			dberror();
		}
		get_record(fp, rcdno, c);
		fh [fp].first_record = c->next_record;
		free(c);
	}
	else
		rcdno = fh [fp].next_record++;
	put_record(fp, rcdno, bf);
	return rcdno;
}

/* ---------------- retrieve a record -------------- */
int get_record(fp, rcdno, bf)
int fp;
RPTR rcdno;
char *bf;
{
	if (rcdno >= fh [fp].next_record)
		return ERROR;
	lseek(handle [fp],
			flocate(rcdno, fh [fp].record_length), 0);
	read(handle [fp], bf, fh [fp].record_length);
	return OK;
}

/* ---------------- rewrite a record -------------- */
int put_record(fp, rcdno, bf)
int fp;
RPTR rcdno;
char *bf;
{
	if (rcdno > fh [fp].next_record)
		return ERROR;
	lseek(handle [fp],
				flocate(rcdno, fh [fp].record_length), 0);
	write(handle [fp], bf, fh [fp].record_length);
	return OK;
}

/* -------------- delete a record ---------------- */
int delete_record(fp, rcdno)
int fp;
RPTR rcdno;
{
	FHEADER *bf;
	extern char *malloc();

	if (rcdno > fh [fp].next_record)
		return ERROR;
	if ((bf = (FHEADER *)
			malloc(fh [fp].record_length)) == NULL)	{
		errno = D_OM;
		dberror();
	}
	set_mem(bf, fh [fp].record_length, '\0');
	bf->next_record = fh [fp].first_record;
	bf->first_record = -1;
	fh [fp].first_record = rcdno;
	put_record(fp, rcdno, bf);
	free(bf);
	return OK;
}

