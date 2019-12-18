/* -------------------------- qd.c ----------------------- */
/* A query program.  Enter the name of the data base file and
 * (optionally) a list of data elements.  A screen is built
 * and the file is updated based upon the data entered.
 */
#include <stdio.h>
#include "keys.h"
#include "cdata.h"

static int file;
int existing_record;
char *rb;				/* record buffer */
char *hb;				/* hold buffer */
char *sc;				/* screen buffer   */
int len;				/* record length   */
char *malloc();
int fl[] = {0, -1};
int iplist [MXELE+1];
int *els;
void query(), rcdin(), rcdout(), set_trap(), clear_record();
int key_entry();

main(argc, argv)
int argc;
char *argv[];
{
	if (argc > 1)	{
		if ((file = filename(argv[1])) != ERROR)	{
			if (argc == 2)	{
				len = rlen(file);
				els = file_ele [file];
			}
			else if (ellist(argc-2, argv+2, iplist) == OK)	{
				len = epos(0, iplist);
				els = iplist;
			}
			else
				exit(1);
			sc = malloc(len);			/* screen buffer */
			rb = malloc(rlen(file));	/* record buffer */
			hb = malloc(rlen(file));	/* hold buffer   */
			if (sc == (char *) 0 ||
					rb == (char *) 0 ||
						hb == (char *) 0)	{
				printf("\nOut of memory");
				exit(1);
			}
			init_rcd(file, rb);
			init_rcd(file, hb);
			init_screen(argv[1], els, sc);
			query();
			free(hb);
			free(rb);
			free(sc);
		}
	}
}



/* -------------- process the query ------------ */
static void query()
{
	int term = 0;
	
	*fl = file;
	db_open("", fl);
	clrrcd(sc, els);
	set_trap();
	while (term != ESC)	{
		term = data_entry();
		switch (term)	{
			/* ----------- GO --------------- */
			case F1:	rcdout();
						break;
			/* ------------- First record ---------- */
			case HOME:	rcdout();
						if (first_rcd(file, 1, rb) == ERROR)
							post_notice("Empty file");
						else
							rcdin();
						break;
			/* ------------- First record ---------- */
			case END:	rcdout();
						if (last_rcd(file, 1, rb) == ERROR)
							post_notice("Empty file");
						else
							rcdin();
						break;
			/* ------------- Previous record -------- */
			case PGUP:	rcdout();
						if (prev_rcd(file, 1, rb) == ERROR)	{
							post_notice("Beginning of file");
							if (first_rcd(file, 1, rb) ==
													ERROR)	{
								post_notice("Empty file");
								break;
							}
						}
						rcdin();
						break;
			/* ------------- Next record ------------- */
			case PGDN:	rcdout();
						if (next_rcd(file, 1, rb) == ERROR)	{
							post_notice("At end of file");
							if (last_rcd(file, 1, rb) ==
													ERROR)	{
								post_notice("Empty file");
								break;
							}
						}
						rcdin();
						break;
			/* -------------- Delete record ------------- */
			case F7:	if (spaces(rb)== 0)	{
							post_notice("Verify w/F7");
							if (get_char() == F7)	{
								del_rcd(file);
								clear_record();
							}
							clear_notice();
						}
						break;
			case ESC:	if (spaces(sc))
							break;
						clear_record();
						term = 0;
						break;
			default:	break;
		}
	}
	clear_screen();
	db_cls();
}

/* ----------- clear out the record area -------------- */
static void clear_record()
{
	int i = 0;
	while (index_ele [file] [0] [i])
		protect(index_ele[file][0][i++],FALSE);
	clrrcd(sc, els);
	existing_record = FALSE;
}

/* ----------- get the data base file record ------------- */
static void rcdin()
{
	int i = 0;

	if (empty(rb, rlen(file)) == 0)	{
		rcd_fill(rb, sc, file_ele [file], els);
		mov_mem(rb, hb, rlen(file));
		existing_record = TRUE;
		while (index_ele [file] [0] [i])
			protect(index_ele[file][0][i++],TRUE);
	}
}


/* ------- add or update the data base file record ------ */
static void rcdout()
{
	if (empty(sc, len) == 0)	{
		rcd_fill(sc, rb, els, file_ele[file]);
		if (existing_record)	{
			if (same() == 0)	{
				post_notice("Returning record");
				rtn_rcd(file, rb);
			}
		}
		else	{
			post_notice("New record added");
			if (add_rcd(file, rb) == ERROR)
				dberror();
		}
		clear_record();
	}
}

/* -------- test for an empty record buffer ---------- */
static int empty(b, l)
char *b;
int l;
{
	while (l--)
		if (*b && *b != ' ')
			return FALSE;
		else
		 	b++;
	return TRUE;
}

/* ---------- test two record buffers for equality ------- */
static int same()
{
	int ln = rlen(file);

	while (--ln)
		if (*(rb + ln) != *(hb + ln))
			break;
	return (*(rb + ln) == *(hb + ln));
}

/* ------ set the query screen's key element trap --------- */
static void set_trap()
{
	int i = 0;

	while (index_ele [file] [0] [i])
		i++;
	edit(index_ele [file] [0] [i-1], key_entry);
}

/* --- come here when the primary key has been entered ---- */
static int key_entry(s)
char *s;
{
	char key [MXKEYLEN];
	int i;

	if (spaces(s))
		return OK;
	*key = '\0';
	i = 0;
	while (index_ele [file] [0] [i])	{
		protect(index_ele[file][0][i],TRUE);
		strcat(key, sc + epos(index_ele[file][0][i++], els));
	}
	if (find_rcd(file, 1, key, rb) == ERROR)	{
		post_notice("New record");
		existing_record = FALSE;
		return OK;
	}
	rcdin();
	tally();
	return OK;
}

