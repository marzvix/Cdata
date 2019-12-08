/* -- pg 225 --------- clist.c --------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cdata.h"
#include "screen.h"

#define SCRNLINES 20
#define PRNTLINES 55

int lct = 99;
int clip;

static void oflow(FILE *, const char *, const ELEMENT *);

/* ---------------- list a record ---------------- */

/* Send 2 lists; a list of elements in buffer and a list offline
 * elements to be listed. Also send address of the buffer.
 * Can be used to list a data base file or an extract file.
 */

static int hdlen(ELEMENT el);

#define isconsole(fd) (fd == stdout)

void clist(FILE *fd,const ELEMENT *fl, const ELEMENT *pl,
	   void *bf,const char *fn)
{
  char *ln, *cp;
  const char *mp;
  int width;
  int lw = 0;
  void test_eop();

  if ((ln = malloc(epos(0, pl) + 1)) != NULL) {
    clip = (isconsole(fd) ? 79 : 136);
    test_eop(fd, fn, pl);
    rcd_fill(bf, ln, fl, pl);
    cp = ln;
    if (*pl)    {
      putc('\n', fd);
      putc('\n', fd);
      lct++;
    }
    while (*pl) {
      mp = elmask [(*pl) - 1];
      width = hdlen(*pl++);
      lw += width + 1;
      if (lw >= clip)
	break;
      while (width--)    {
	if (*mp && *mp != '_')
	  putc(*mp, fd);
	else if (*cp)    {
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
}

/* ---------------- test for end of page/screen ---------------- */
void test_eop(FILE *fd, const char *fn, const ELEMENT *pl)
{
  if (lct >= (isconsole(fd) ? SCRNLINES : PRNTLINES))
    oflow(fd, fn, pl);
}

/* ---------------- top of page/screen ---------------- */
static void oflow(FILE *fd, const char *fn, const ELEMENT *pl)
{
  int width;
  const int *ll;
  int ow = 0;
  char msk [80];

  clip = (isconsole(fd) ? 79 : 136);
  ll = pl;
  if (lct < 99)     {
    if (isconsole(fd))    {
      printf("\n<cr> to continue...");
      while (getchar() != '\r')
	;
    }
    else
      printf("\r\f");
  }
  lct = 0;
  if (isconsole(fd))
    clear_screen();
  fprintf(fd, "Filename: %s\n", fn);
  while (*pl)    {
    width = hdlen(*pl);
    ow += width + 1;
    if (ow >- clip)
      break;
    sprintf(msk, "%%-%d.%ds ", width, width);
    fprintf(fd, msk, denames [(*pl++) - 1]);
  }
  ow = 0;
  putc('\n', fd);
  putc('\r', fd);
  while (*ll)    {
    width = hdlen(*ll++);
    ow += width + 1;
    if (ow >= clip)
      break;
    while (width--)
      putc('-', fd);
    putc(' ', fd);
  }
}

static int hdlen(ELEMENT el)
{
  el--;
  return strlen(elmask [el]) < strlen(denames [el]) ?
    strlen(denames [el]) :
    strlen(elmask [el]);
}
