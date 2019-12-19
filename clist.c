/* ----------------------- clist.c --------------------- */

#include <stdio.h>
#include <string.h>
#include "cdata.h"

#define SCRNLINES 20
#define PRNTLINES 55

static int hdlen();


int lct = 99;
int clip;

/* ------------ list a record ----------- */

/* Send 2 lists; a list of elements in buffer and a list of
 * elements to be listed. Also send address of the buffer.
 * Can be used to list a data base file or an extract file.
 */

void clist(fd, inter, fl, pl, bf, fn)
     FILE *fd;		/* output file */
     int inter;		/* true for user interaction & page breaks */
     int *fl;		/* file list  */
     int *pl;		/* print list */
     char *bf;		/* buffer     */
     char *fn;		/* file name  */
{
  char *ln, *cp, *mp; /* , *malloc(); */
  int width;
  int lw = 0;
  void test_eop();

  ln = malloc(epos(0, pl) + 1);
  clip = ((!inter || isatty(fileno(fd))) ? 79 : 136);
  if (inter)
    test_eop(fd, fn, pl);
  rcd_fill(bf, ln, fl, pl);
  cp = ln;
  if (*pl)	{
    putc('\n', fd);
    putc('\r', fd);
    lct++;
  }
  while (*pl)	{
    mp = elmask [(*pl) - 1];
    width = hdlen(*pl++);
    lw += width + 1;
    if (lw >= clip)
      break;
    while (width--)		{
      if (*mp && *mp != '_')
	putc(*mp, fd);
      else if (*cp)	{
	putc(isprint(*cp) ? *cp : '?', fd);
	cp++;
      }
      else 
	putc(' ', fd);
      if (*mp)
	mp++;
    }
    if (*pl)
      putc(' ', fd);
    cp++;
  }
  free(ln);
}

/* ----------- test for end of page/screen --------- */
void test_eop(fd, fn, pl)
     FILE *fd;
     char *fn;		/* file name */
     int *pl;		/* element list */
{
  void oflow();

  if (lct >= (isatty(fileno(fd)) ? SCRNLINES : PRNTLINES))
    oflow(fd, TRUE, fn, pl);
}


/* -------------- top of page/screen ------------ */
void oflow(fd, inter, fn, pl)
     FILE *fd;
     char *fn;		/* file name */
     int *pl;		/* element list */
{
  int width;
  int *ll;
  int ow = 0;
  char msk [80];

  clip = ((!inter || isatty(fileno(fd))) ? 79 : 136);
  ll = pl;
  if (inter && lct < 99)	{
    if (isatty(fileno(fd)))	{
      printf("\n<cr> to continue...");
      while (get_char() != '\r')
	;
    }
    else
      printf("\r\f");
  }
  lct = 0;
  if (inter && isatty(fileno(fd)))
    clear_screen();
  if (inter)
    fprintf(fd, "Filename: %s\n", fn);
  while (*pl)	{
    width = hdlen(*pl);
    ow += width + 1;
    if (ow >= clip)
      break;
    sprintf(msk, "%%-%d.%ds ", width, width);
    fprintf(fd, msk, denames [(*pl++) - 1]);
  }
  ow = 0;
  putc('\n', fd);
  putc('\r', fd);
  while (*ll)	{
    width = hdlen(*ll++);
    ow += width + 1;
    if (ow >= clip)
      break;
    while (width--)
      putc('-', fd);
    putc(' ', fd);
  }
}

static int hdlen(el)
     int el;
{
  el--;
  return strlen(elmask [el]) < strlen(denames [el]) ? 
    strlen(denames [el]) : 
    strlen(elmask [el]);
}

/* ---------- isatty function
   for the compilers that do not have it ------ */
#if COMPILER == ECOC
isatty(fd)
{
  int dev;

  dev = ioctl(fd, 0, 0);
  if ((dev & 0x80) == 0)
    return FALSE;
  return (dev & 3);
}
#endif

#if COMPILER == CI_C86
#include <dos.h>
struct regval rg;

isatty(fd)
{
  rg.ax = 0x4400;
  rg.bx = fd;
  sysint(0x21, &rg, &rg);
  if ((rg.dx & 0x80) == 0)
    return FALSE;
  return (rg.dx & 3);
}
#endif

#if COMPILER == DATALIGHT
#include <dos.h>
REGS rg;

int isatty(fd)
     int fd;
{
  rg.ax = 0x4400;
  rg.bx = fd;
  int86(0x21, &rg, &rg);
  if ((rg.dx & 0x80) == 0)
    return FALSE;
  return (rg.dx & 3);
}
#endif

#if COMPILER == LETSC
#include <dos.h>
struct reg rg;

int isatty(fd)
     int fd;
{
  rg.r_ax = 0x4400;
  rg.r_bx = fd;
  intcall(&rg, &rg, 0x21);
  if ((rg.r_dx & 0x80) == 0)
    return FALSE;
  return (rg.r_dx & 3);
}
#endif


#if COMPILER == WIZARD
int isatty(fd)
     int fd;
{
  int dev;

  dev = ioctl(fd, 0, 0, 0);
  if ((dev & 0x80) == 0)
    return FALSE;
  return (dev & 3);
}
#endif
