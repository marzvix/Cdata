/* ----------------- database.c ---------------------- */

#include <stdio.h>
#include "cdata.h"
#include "keys.h"

void build_b();			/* Builds a new B-tree				*/
RPTR locate();        	/* Searches a B-tree for a key      */
RPTR nextkey();        	/* Gets address of the next key     */
RPTR prevkey();        	/* Gets address of the previous key */
RPTR firstkey();      	/* Gets address of first key        */
RPTR lastkey();       	/* Gets address of last key         */
RPTR currkey();	    	/* Gets address of current key	  	*/
void file_close();      /* Closes a data file               */
RPTR new_record();      /* Adds a record to a file          */
char *malloc();
void init_index(), cls_index(), del_indexes();

int db_opened = FALSE;	/* data base opened indicator       */
int curr_fd [MXFILS];	/* current file descriptor	    	*/
RPTR curr_a [MXFILS];	/* current record file address	    */
char *bfs [MXFILS];		/* file i/o buffers */
int bfd [MXFILS] [MXINDEX];
char dbpath [64];
int notice_posted = 0;
int prev_col = 0, prev_row = 0;

/* ------------------ open the data base ----------------- */
void db_open(path, fl)
char *path;
int *fl;
{
	char fnm [64];
	int i;

	if (!db_opened)	{
		for (i = 0; i < MXFILS; i++)
			curr_fd [i] = -1;
		db_opened = TRUE;
	}
	strcpy(dbpath, path);
	while (*fl != -1)	{
		sprintf(fnm, "%s%.8s.dat", path, dbfiles [*fl]);
		curr_fd [*fl] = file_open(fnm);
		init_index(path, *fl);
		if ((bfs [*fl] = malloc(rlen(*fl))) == NULL)	{
			errno = D_OM;
			dberror();
		}
		fl++;
	}
}

/* ------------- add a record to a file --------------- */
int add_rcd(f, bf)
int f;
char *bf;
{
	RPTR ad;
	extern RPTR new_record();
	int rtn;

	if ((rtn = relate_rcd(f, bf)) != ERROR)	{
		ad = new_record(curr_fd [f], bf);
		if ((rtn = add_indexes(f, bf, ad)) == ERROR)	{
			errno = D_DUPL;
			delete_record(curr_fd [f], ad);
		}
	}
	return rtn;
}

/* ------------- find a record in a file -------------- */
int find_rcd(f,k,key,bf)
int f;			/* file number			*/
int k;			/* key number			*/
char *key;		/* key value			*/
char *bf;		/* buffer for record	*/
{
	RPTR ad;
	
	if ((ad = locate(treeno(f,k), key)) == 0)	{
		errno = D_NF;
		return ERROR;
	}
	return getrcd(f, ad, bf);
}

/* -------- verify that a record is in a file --------- */
int verify_rcd(f,k,key)
int f;			/* file number			*/
int k;			/* key number			*/
char *key;		/* key value			*/
{
	if (locate(treeno(f,k), key) == (RPTR) 0)	{
		errno = D_NF;
		return ERROR;
	}
	return OK;
}

/* ------- find the first indexed record in a file -------- */
int first_rcd(f,k,bf)
int f;			/* file number			*/
int k;			/* key number			*/
char *bf;		/* buffer for record	*/
{
	RPTR ad;
	
	if ((ad = firstkey(treeno(f,k))) == 0)	{
		errno = D_EOF;
		return ERROR;
	}
	return getrcd(f, ad, bf);
}

/* ------- find the last indexed record in a file -------- */
int last_rcd(f,k,bf)
int f;			/* file number			*/
int k;			/* key number			*/
char *bf;		/* buffer for record	*/
{
	RPTR ad;
	
	if ((ad = lastkey(treeno(f,k))) == 0)	{
		errno = D_BOF;
		return ERROR;
	}
	return getrcd(f, ad, bf);
}

/* ------------- find the next record in a file ----------- */
int next_rcd(f,k,bf)
int f;			/* file number			*/
int k;			/* key number			*/
char *bf;		/* buffer for record	*/
{
	RPTR ad;
	
	if ((ad = nextkey(treeno(f, k))) == 0)	{
		errno = D_EOF;
		return ERROR;
	}
	return getrcd(f, ad, bf);
}

/* -------- find the previous record in a file --------- */
int prev_rcd(f,k,bf)
int f;			/* file number			*/
int k;			/* key number			*/
char *bf;		/* buffer for record	*/
{
	RPTR ad;
	
	if ((ad = prevkey(treeno(f,k))) == 0)	{
		errno = D_BOF;
		return ERROR;
	}
	return getrcd(f, ad, bf);
}

/* ----- return the current record to the data base ------- */
int rtn_rcd(f, bf)
int f;
char *bf;
{
	int rtn;

	if (curr_a [f] == 0)	{
		errno = D_PRIOR;
		return ERROR;
	}
	if ((rtn = relate_rcd(f, bf)) != ERROR)	{
		del_indexes(f, curr_a [f]);
		if ((rtn = add_indexes(f, bf, curr_a [f])) == OK)
			put_record(curr_fd [f], curr_a [f], bf);
		else
			errno = D_DUPL;
	}
	return rtn;
}

/* ------- delete the current record from the file -------- */
int del_rcd(f)
int f;
{
	if (curr_a [f])	{
		del_indexes(f, curr_a [f]); 
		delete_record(curr_fd [f], curr_a [f]);
		curr_a [f] = 0;
		return OK;
	}
	errno = D_PRIOR;
	return ERROR;
}





/* ---------- find the current record in a file ----------- */
int curr_rcd(f,k,bf)
int f;			/* file number			*/
int k;			/* key number			*/
char *bf;		/* buffer for record	*/
{
	RPTR ad;
	extern RPTR currkey();

	if ((ad = currkey(treeno(f,k))) == 0)	{
		errno = D_NF;
		return ERROR;
	}
	getrcd(f, ad, bf);
	return OK;
}

/* --------- get the next physical
				sequential record from the file ------- */
int seqrcd(f, bf)
int f;
RPTR *bf;
{
	RPTR ad;
	int rtn;

	do	{
		ad = ++curr_a [f];
		if ((rtn = (rel_rcd(f,ad,bf)))==ERROR && errno!=D_NF)
			break;
	}	while (errno == D_NF);
	return rtn;
}

/* ----------------- close the data base ------------------ */
void db_cls()
{
	int f;

	for (f = 0; f < MXFILS; f++)
		if (curr_fd [f] != -1)	{
			file_close(curr_fd [f]);
			cls_index(f);
			free(bfs[f]);
			curr_fd [f] = -1;
		}
	db_opened = FALSE;
}

/* -------------------- data base error routine ----------- */
void dberror()
{
	static char *ers [] = {
		"Record not found",
		"No prior record",
		"End of file",
		"Beginning of file",
		"Record already exists",
		"Not enough memory",
		"Index corrupted",
		"Disk i/o error"
	};
	static int fat [] = {0,1,0,0,0,1,1,1};

	error_message(ers [errno-1]);
	if (fat [errno-1])
		exit(1);
}

/* ----------- compute file record length ----------------- */
int rlen(f)
int f;
{
	return epos(0, file_ele [f]);
}





/* ---------- initialize a file record buffer ------------ */
void init_rcd(f, bf)
int f;			/* file number */
char *bf;		/* buffer */
{
	clrrcd(bf, file_ele[f]);
}

/* -------- set a generic record buffer to blanks --------- */
void clrrcd(bf, els)
char *bf;		/* buffer */
int *els;		/* data element list */
{
	int ln, i = 0, el;
	char *rb;

	while (*(els + i))	{
		el = *(els + i);
		rb = bf + epos(el, els);
		ln = ellen [el - 1];
		while (ln--)
			*rb++ = ' ';
		*rb = '\0';
		i++;
	}
}


/* ------- move data from one record to another ------- */
void rcd_fill(s, d, slist, dlist)
char *s;		/* source record buffer */
char *d;		/* destination record buffer */
int *slist;		/* source data element list */
int *dlist;		/* destination data element list */
{
	int *s1, *d1;

	s1 = slist;
	while (*s1)	{
		d1 = dlist;
		while (*d1)	{
			if (*s1 == *d1)
				strcpy(d+epos(*d1,dlist), s+epos(*s1,slist));
			d1++;
		}
		s1++;
	}
}

/* -------- compute relative position of
				a data element within a record -----*/
int epos(el, list)
int el;			/* element number */
int *list;		/* record element list */
{
	int len = 0;

	while (el != *list)
		len += ellen [(*list++)-1] + 1;
	return len;
}

/* ------------- index management functions ------------ */
/* ---- initialize the indices for a file ---- */
static void init_index(path, f)
char *path;		/* where the data base is */
int f;			/* file number */
{
	char xname [64];
	int x = 0;

	while (*(index_ele [f] + x))	{
		sprintf(xname, "%s%.8s.x%02d",path,dbfiles[f],x+1);
		if ((bfd [f] [x++] = btree_init(xname)) == ERROR)	{
			printf("\n%s", xname);
			errno = D_INDXC;
			dberror();
		}
	}
}

/* ---- build the indices for a file ---- */
void build_index(path, f)
char *path;
int f;			/* file number */
{
	char xname [64];
	int x = 0, x1;
	int len;

	while (*(index_ele [f] + x))	{
		sprintf(xname, "%s%.8s.x%02d", path, dbfiles[f], x+1);
		len = 0;
		x1 = 0;
		while (*(*(index_ele [f] + x) + x1))
			len += ellen [*(*(index_ele [f] + x) + (x1++))-1];
		build_b(xname, len);
		x++;
	}
}

/* ----- close the indices for a file ------ */
static void cls_index(f)
int f;
{
	int x = 0;

	while (*(index_ele [f] + x))	{
		if (bfd [f] [x] != ERROR)
			btree_close(bfd [f] [x]);
		x++;
	}
}







/* ---- add index values from a record to the indices ---- */
int add_indexes(f, bf, ad)
int f;
char *bf;
RPTR ad;
{
	int x = 0;
	int i;
	char key [MXKEYLEN];

	while (*(index_ele [f] + x))	{
		*key = '\0';
		i = 0;
		while(*(*(index_ele [f] + x) + i))
			strcat(key,
				bf +
			epos(*(*(index_ele[f]+x)+(i++)),file_ele [f]));
		if (insertkey(bfd [f] [x], key, ad, !x) == ERROR)
  			return ERROR;
		x++;
	}
  	return OK;
}

/* --- delete index values in a record from the indices --- */
static void del_indexes(f, ad)
int f;
RPTR ad;
{
	char *bf;
	int x = 0;
	int i;
	char key [MXKEYLEN];

	if ((bf = malloc(rlen(f))) == NULL)	{
		errno = D_OM;
		dberror();
	}
	get_record(curr_fd [f], ad, bf);
	while (*(index_ele [f] + x))	{
		*key = '\0';
		i = 0;
		while (*(*(index_ele [f] + x) + i))
			strcat(key,
				bf +
			epos(*(*(index_ele[f]+x)+(i++)), file_ele [f]));
		deletekey(bfd [f] [x++], key, ad);
	}
	free(bf);
}


/* ---- compute tree number from file and key number ---- */
static int treeno(f, k)
int f, k;
{
	return bfd [f] [k - 1];
}


/* ---- validate the contents of a record where its file is
        related to another file in the data base ---------- */
static int relate_rcd(f, bf)
int f;
char *bf;
{
	int fx = 0, mx, *fp;
	static int ff[] = {0, -1};
	char *cp;

	while (dbfiles [fx])	{
		if (fx != f && *(*(index_ele [fx]) + 1) == 0)	{
			mx = *(*(index_ele [fx]));
			fp = file_ele [f];
			while (*fp)	{
				if (*fp == mx)	{
					cp = bf + epos(mx, file_ele [f]);
					if (data_in(cp))	{
						if (curr_fd[fx] == -1)	{
							*ff = fx;
							db_open(dbpath, ff);
						}
						if (verify_rcd(fx, 1, cp) == ERROR)
							return ERROR;
					}
					break;
				}
				fp++;
			}
		}
		fx++;
	}
	return OK;
}

/* ----- test a string for data. return TRUE if any ---- */
static int data_in(c)
char *c;
{
	while (*c == ' ')		
		c++;
	return (*c != '\0');
}

/* ------------- get a record from a file -------------- */
static int getrcd(f, ad, bf)
int f;
RPTR ad;
char *bf;
{
	get_record(curr_fd [f], ad, bf);
	curr_a [f] = ad;
	return OK;
}

extern FHEADER fh [];




/* ----- find a record by relative record number ------ */
static int rel_rcd(f, ad, bf)
int f;			/* file number			*/
RPTR ad;
int *bf;
{
	errno = 0;
	if (ad >= fh [curr_fd [f]].next_record)	{
		errno = D_EOF;
		return ERROR;
	}
	getrcd(f, ad, bf);
	if (*bf == -1)	{
		errno = D_NF;
		return ERROR;
	}
	return OK;
}

/* -------------- error message -------- */
void error_message(s)
char *s;
{
	put_char(BELL);
	post_notice(s);
}

/* --------- clear notice line ------------- */
void clear_notice()
{
	int i;

	if (notice_posted)	{
		cursor(0,24);
		for (i = 0; i < 50; i++)
			put_char(' ');
		notice_posted = FALSE;
		cursor(prev_col, prev_row);
	}
}

/* ----------- post a notice ----------------- */
void post_notice(s)
char *s;
{
	clear_notice();
	cursor(0,24);
	while (*s)	{
		put_char(isprint(*s) ? *s : '.');
		s++;
	}
	cursor(prev_col, prev_row);
	notice_posted = TRUE;
}

/* ---------- Move a block -------- */
void mov_mem(s, d, l)
char *s, *d;
int l;
{
	if (d > s)
		while (l--)
			*(d + l) = *(s + l);
	else
		while (l--)
			*d++ = *s++;
}


/* --------- Set a block to a character value ----- */
void set_mem(s, l, n)
char *s, n;
int l;
{
	while (l--)
		*s++ = n;
}




/*
 * Convert a file name into its file token.
 * Return the token,
 * or ERROR if the file name is not in the schema.
 */
int filename(fn)
char *fn;
{
	char fname[32];
	int f;
	void name_cvt();

	name_cvt(fname, fn);
	for (f = 0; dbfiles [f]; f++)
		if (strcmp(fname, dbfiles [f]) == 0)
			break;
	if (dbfiles [f] == 0)	{
		fprintf(stderr, "\nNo such file as %s", fname);
		return ERROR;
	}
	return f;
}

/* ----------- convert a name to upper case ---------- */
void name_cvt(c2, c1)
char *c1, *c2;
{
	while (*c1)	{
		*c2 = toupper(*c1);
		c1++;
		c2++;
	}
	*c2 = '\0';
}
