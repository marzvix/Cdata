/* ----------------- screen.c ------------------------- */
#include <stdio.h>
#include "cdata.h"
#include "keys.h"

int insert_mode = FALSE;	   /* insert mode, TRUE/FALSE */
extern int prev_col, prev_row; /* current cursor location */
extern int screen_displayed;   /* template displayed flag */
#define FIELDCHAR '_'		   /* field filler character  */

struct {
	int prot;
	int (*edits)();
} sb [MXELE];

int *elist;
char *bf;
char *tname;
void right_justify(),
     right_justify_zero_fill(), left_justify_zero_fill(),
	 disp_element(), insert_status(),
	 put_fchar(), data_coord();

/* ----------- initialize the screen process ------------- */
void init_screen(name, els, bfr)
char *name;
int *els;
char *bfr;
{
	tname = name;
	elist = els;
	bf = bfr;
}

/* ------- set the protect flag for a screen field ------- */
void protect(el, tf)
int el, tf;
{
	sb[elp(el)].prot = tf;
}

/* ---- set the field edit function for a screen field --- */
void edit(el, func)
int el, (*func)();
{
	sb[elp(el)].edits = func;
}

/* ---- compute the relative position
			of an element on a screen ------ */
static int elp(el)
int el;
{
	int i;

	for (i = 0; *(elist + i); i++)
		if (el == *(elist + i))
			break;
	return i;
}

/* ----------- Display the crt template ----------- */
void display_template()
{
	int i, el, ct;
	char detag[16], *cp1, *cp2;

	clear_screen();
	screen_displayed = TRUE;
	ct = no_flds();
	printf("\n                   --- %s ---\n", tname);
	for (i = 0; i < ct; i++)	{
		el = *(elist + i) - 1;
		cp1 = denames[el];
		cp2 = detag;
		while (*cp1 && cp2 < detag + sizeof detag - 1)	{
			*cp2++ = *cp1 == '_' ? ' ' : *cp1;
			cp1++;
		}
		*cp2 = '\0';
		printf("\n%-16.16s %s",detag, elmask[el]);
	}
	printf("\n");
	insert_status();
}

/* ----- Process data entry for a screen template. ---- */
int data_entry()
{
	int (*validfunct)();
	int field_ctr, exitcode, el;
	int field_ptr = 0, done = FALSE, isvalid;
	char *bfptr;

	if (screen_displayed == 0)
		display_template();
	tally();
	field_ctr = no_flds();
	/* ---- collect data from keyboard into screen ---- */
	while (done == FALSE)	{
		bfptr = bf + epos(elist[field_ptr], elist);;
		el = *(elist + field_ptr) - 1;
		validfunct = sb[field_ptr].edits;
		data_coord(el + 1);
		if (sb[field_ptr].prot == FALSE)	{
			exitcode =
				read_element(eltype[el], elmask[el], bfptr);
			isvalid = (exitcode != ESC && validfunct) ? 
						(*validfunct)(bfptr,exitcode) : OK;
		}
		else	{
			exitcode = FWD;
			isvalid = OK;
		}
		if (isvalid == OK)
			switch (exitcode)	{		/* passed edit */
				case DN:				/* cursor down key */
				case '\r':				/* enter/return */
				case '\t':				/* horizontal tab */
				case FWD:				/* -> */
					if (field_ptr+1 == field_ctr)
						field_ptr = 0;
					else
						field_ptr++;
					break;
				case UP:				/* cursor up key */
				case BS:				/* back space */
					if (field_ptr == 0)
						field_ptr = field_ctr - 1;
					else
						field_ptr--;
					break;
				default:
					done = endstroke(exitcode);
					break;
			}
	}
	return (exitcode);
}

/* ----- Compute the number of fields on a template ------ */
static int no_flds()
{
	int ct = 0;

	while (*(elist + ct))
		ct++;
	return ct;
}

/* ------ compute data element field coordinates --------- */
static void data_coord(el)
int el;
{
	prev_col = 17;
	prev_row = elp(el) + 3;
}



/* ------- read data element from keyboard ------------- */
static int read_element(type, msk, bfr)
char type, *msk, *bfr;
{
	char *mask = msk, *buff = bfr;
	int done = FALSE, c, column = prev_col;

	while (*mask != FIELDCHAR)	{
		prev_col++;
		mask++;
	}
	while (TRUE)	{
		cursor(prev_col, prev_row);
		c = get_char();
		clear_notice();
		switch (c)	{
			case '\b':
			case BS:
				if (buff == bfr)	{
					done = c == BS;
					break;
				}
				--buff;
				do	{
					--mask;
					--prev_col;
				} while (*mask != FIELDCHAR);
				if (c == BS)
					break;
			case DEL:
				mov_mem(buff+1, buff, strlen(buff));
				*(buff+strlen(buff)) = ' ';
				cursor(prev_col, prev_row);
				disp_element(buff,mask);
				break;
			case FWD:
				do	{
					prev_col++;
					mask++;
				} while (*mask && *mask != FIELDCHAR);
				buff++;
				break;
			case INS:
				insert_mode ^= TRUE;
				insert_status();
				break;
			case '.':
				if (type == 'C')	{
					if (*mask++ && *buff == ' ')	{
						*buff++ = '0';
						if (*mask++ && *buff == ' ')
							*buff++ = '0';
					}
					right_justify(bfr);
					cursor(column, prev_row);
					disp_element(bfr, msk);
					prev_col = column + strlen(msk)-2;
					mask = msk + strlen(msk)-2;
					buff = bfr + strlen(bfr)-2;
					break;
				}
			default:
				if (endstroke(c))	{
					done = TRUE;
					break;
				}
				if (type != 'A' && !isdigit(c))	{
					error_message("Numbers only");
					break;
				}
				if (insert_mode)	{
					mov_mem(buff, buff+1, strlen(buff)-1);
					disp_element(buff,mask);
				}
				*buff++ = c;
				put_fchar(c);
				do	{
					prev_col++;
					mask++;
				} while (*mask && *mask != FIELDCHAR);
				if (!*mask)
					c = FWD;
				break;
		}
		if (!*mask)
			done = TRUE;
		if (done)	{
			if (type == 'D' &&
				c != ESC &&
					validate_date(bfr) != OK)
				return -1;
			break;
		}
	}
	if (c != ESC && type != 'A')	{
		if (type == 'C')	{
			if (*mask++ && *buff == ' ')	{
				*buff++ = '0';
				if (*mask++ && *buff == ' ')
					*buff++ = '0';
			}
		}
		if (type == 'Z' || type == 'D')
			right_justify_zero_fill(bfr);
		else
			right_justify(bfr);
		cursor(column, prev_row);
		disp_element(bfr,msk);
	}
	return (c);
}

/* ---------- test c for an ending keystroke ----------- */
endstroke(c)
{
	switch (c)	{
		case '\r':
		case '\n':
		case '\t':
		case ESC:
		case F1:
		case F2:
		case F3:
		case F4:
		case F5:
		case F6:
		case F7:
		case F8:
		case F9:
		case F10:
		case PGUP:
		case PGDN:
		case HOME:
		case END:
		case UP:
		case DN:
			return TRUE;
		default:
			return FALSE;
	}
}

/* ------- right justify, space fill -------- */
static void right_justify(s)
char *s;
{
	int len;

	len = strlen(s);
	while (*s == ' ' || *s == '0' && len)	{
		len--;
		*s++ = ' ';
	}
	if (len)
		while (*(s+(len-1)) == ' ')	{
			mov_mem(s, s+1, len-1);
			*s = ' ';
		}
}

/* ---------- right justify, zero fill --------------- */
static void right_justify_zero_fill(s)
char *s;
{
	int len;

	if (spaces(s))
		return;
	len = strlen(s);
	while (*(s + len - 1) == ' ')	{
		mov_mem(s, s + 1, len-1);
		*s = '0';
	}
}

/* ----------- test for spaces -------- */
int spaces(c)
char *c;
{
	while (*c == ' ')
		c++;
	return !*c;
}

/* -------------- validate a date ----------------- */
static int validate_date(s)
char *s;
{
	static int days [] =
		{ 31,28,31,30,31,30,31,31,30,31,30,31 };
	char date [7];
	int mo;

	strcpy(date, s);
	if (spaces(date))
		return OK;
	if (!atoi(date + 4))
		days[1]++;
	*(date + 4) = '\0';
	mo = atoi(date+2);
	*(date+2) = '\0';
	if (mo && mo < 13 && atoi(date) &&
				atoi(date) <= days [mo - 1])
		return OK;
	error_message("Invalid date");
	return ERROR;
}

/* ---- display all the fields on a screen ------ */
void tally()
{
	int *els = elist;

	while (*els)
		put_field(*els++);
}

/* ------- write a data element on the screen --------- */
void put_field(el)
int el;
{
	data_coord(el);
	cursor(prev_col, prev_row);
	disp_element(bf + epos(el, elist), elmask[el - 1]);
}

/* ---------- display a data element -------- */
static void disp_element(b, msk)
char *b, *msk;
{
	while (*msk)	{
		put_fchar(*msk != FIELDCHAR ? *msk : *b++);
		msk++;
	}
	cursor(prev_col,prev_row);
}




/* ---------- display insert mode status ------------------ */
static void insert_status()
{
	cursor(65,24);
	printf(insert_mode ? "[INS]" : "     ");
	cursor(prev_col,prev_row);
}

/* --------- write a field character --------- */
static void put_fchar(c)
int c;
{
	put_char(c == ' ' ? '_' : c);
}
