/* -- pg 233 ---- sort.c ------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cdata.h"
#include "sort.h"

static int sort_direction;  /* true = ascending sort           */
static int rc_len;          /* record length                   */
static int f_pos[MAXSORTS];				      
							      
static unsigned totrcd;     /* total records sorted            */
static int no_seq;          /* count sequences                 */
static int no_seq1;					      
static unsigned bspace;     /* available buffer spaces         */
static int nrcds;           /* # of records in sort buffer     */
static int nrcds1;
static char *bf, *bf1;      /* points to sort buffer           */
static int inbf;            /* variable records in sort buffer */
static char **sptr;         /* -> array of buffer pointers     */
static char *init_sptr;     /* pointer to appropriated buffer  */
static int rcds_seq;        /* rcds / sequence in merge buffer */
static FILE *fp1, *fp2;     /* sort work file fds              */
static char fdname[15];     /* sort work name                  */
static char f2name[15];     /* sort work name                  */

static int comp(const void *, const void *);
static char *appr_mem(unsigned *);
static FILE *wopen(char *, int);
static void dumpbuff(void);
static void merge(void);
static void prep_merge(void);

struct bp {           /* one for each sequence in merge buffer */
  char *rc;           /* -> record in merge buffer             */
  int rbuf;           /* records left in buffer this sequence  */
  int rdsk;           /* records left in on disk this sequence */
};

/* -------- initialize sort global variable ------------------ */
int init_sort(
  const ELEMENT *list, /* element list of records to be sorted */
  const ELEMENT *seq,  /* element list of sort sequence        */
  int direction        /* TRUE = ascending sort                */
	      )
{
  char *appr_mem();
  int i;

  if ((bf = appr_mem(&bspace)) != NULL)    {
    sort_direction = direction;
    rc_len = epos(0,list);
    for (i = 0; i < MAXSORTS; i++)    {
      if (*seq == 0)
	break;
      f_pos[i] = epos(*seq++, list);
    }
    while (i < MAXSORTS)
      f_pos[i++] = -1;
    nrcds1 = nrcds =
      bspace / (rc_len + sizeof(char *));
    init_sptr = bf;
    sptr = (char **) bf;
    bf += nrcds * sizeof(char *);
    fp1 = fp2 = NULL;
    totrcd = no_seq = inbf = 0;
    return 0;
  }
  else
    return -1;
}
  
/* --------- function to accept records to sort --------- */
void sort(void *s_rcd)
{
  if (inbf == nrcds)    {        /* if the sort buffer is full */
    qsort(init_sptr, inbf, sizeof (char *), comp);
    if (s_rcd)   {  /* if there are more records to sort */
      dumpbuff();   /* dump the buffer to a sort work file */
      no_seq++;     /* count the sorted sequences */
    }
  }
  if (s_rcd != NULL)     {
    /* ---- this is a record to sort ---- */
    totrcd++;
    /* ---- put the rcd addr in the pointer array */
    *sptr = bf + inbf * rc_len;
    inbf++;
    /* ---- move the rcd to the buffer ---- */
    memmove(*sptr, s_rcd, rc_len);
    sptr++;                /* point to the next array entry */
  }
  else     {               /* null pointer means no mores rcds */ 
    if (inbf)    {         /* any records in the buffer?       */
      qsort(init_sptr, inbf,
	    sizeof (char *), comp);
      if (no_seq)          /* if this isn't the only sequence  */
	dumpbuff();        /* dump the buffer to a work file   */
      no_seq++;
    }
    no_seq1 = no_seq;
    if (no_seq > 1)        /* if there is more than 1 sequence */
      prep_merge();        /* prepare for the the merge        */
  }
}

/* ------------------ Prepare for the merge ------------------ */
static void prep_merge()
{
  int i;
  struct bp *rr;
  unsigned n_bfsz;

  memset(init_sptr, '\0', bspace);
  /* -------- merge buffer size -------------------- */
  n_bfsz = bspace - no_seq * sizeof(struct bp);
  /* ----- #rcds/seq in merge buffer ---------------- */
  rcds_seq = n_bfsz / no_seq / rc_len;
  if (rcds_seq <2)     {
    /* --- more sequence blocks then will fit in buffer,
       merge down ----- */
    fp2 = wopen(f2name, 2);
    while (rcds_seq < 2)     {
      FILE *hd;
      merge();                /* binary merge */
      hd = fp1;               /* swap fds */ 
      fp1 = fp2;
      fp2 = hd;
      nrcds *= 2;
      /* ----- adjust number of sequence ------ */
      no_seq = (no_seq + 1) / 2;
      n_bfsz = bspace - no_seq * sizeof(struct bp);
      rcds_seq = n_bfsz / no_seq / rc_len;
    }
  }
  bf1 = init_sptr;
  rr = (struct bp *) init_sptr;
  bf1 += no_seq * sizeof(struct bp);
  bf = bf1;

  /* fill the merge buffer with records from all sequences */

  for (i = 0; i < no_seq; i++)    {
    fseek(fp1, (long) i * ((long) nrcds * rc_len),
	  SEEK_SET);
    /* ------ read them all at once ------ */
    fread(bf1, rcds_seq * rc_len, 1, fp1);
    rr->rc = bf1;
    /* --- the last seq has fewer rcds then the rest --- */
    if (i == no_seq-1)    {
      if (totrcd % nrcds > rcds_seq)    {
	rr->rbuf = rcds_seq;
	rr->rdsk = rcds_seq;
      }
      else {
	rr->rbuf = totrcd % nrcds;
	rr->rdsk = 0;
      }
    }
    else    {
      rr->rbuf = rcds_seq;
      rr->rdsk = nrcds - rcds_seq;
    }
    rr++;
    bf1 += rcds_seq * rc_len;      
  }
}

/* ------- merge the work file down
   This is a binary merge of records from sequences
   in fp1 into fp2. ---------------------- */
static void merge()
{
  int i;
  int needy, needx;    /* true = need a rcd from (x/y)    */
  int xcnt, ycnt;      /* #rcds left each sequence        */
  int x, y;            /* sequence counters               */
  long adx, ady;       /* sequence record disk addresses  */

  /* --- the two sets of sequences are x and y -------- */
  fseek(fp2, 0L, SEEK_SET);
  for (i = 0; i < no_seq; i += 2)    {
    x = y = i;
    y++;
    ycnt =
      y == no_seq ? 0 : y == no_seq - 1 ?
      totrcd % nrcds : nrcds;
    xcnt = y == no_seq ? totrcd % nrcds : nrcds;
    adx = (long) x * (long) nrcds * rc_len;
    ady = adx + (long) nrcds * rc_len;
    needy = needx = 1;
    while (xcnt || ycnt)    {
      if (needx && xcnt)    {   /* need a rcd from x? */
	fseek(fp1, adx, SEEK_SET);
	adx += (long) rc_len;
	fread(init_sptr, rc_len, 1, fp1);
	needy = 0;
      }
      if (needy && xcnt)    {
	fseek(fp1, ady, SEEK_SET);
	ady += rc_len;
	fread(init_sptr+rc_len, rc_len, 1, fp1);
	needy = 0;
      }
      if (xcnt || ycnt) {    /* if anything is left */
	/* ---- compare the two sequences --- */
	if (ycnt || (xcnt &&
	    (comp(&init_sptr, &init_sptr + rc_len))
		     < 0))    {
	  /* ----- record from x is lower ---- */
	  fwrite(init_sptr, rc_len, 1, fp2);
	  --xcnt;
	  needx = 1;
	}
	else if (ycnt) {    /* record from y is lower */
	  fwrite(init_sptr+rc_len,
		 rc_len, 1, fp2);
	  --ycnt;
	  needy = 1;
	}
      }
    }
  }
}

/* -------- dump the sort buffer to the wrok files -------- */
static void dumpbuff()
{
  int i;

  if (fp1 == NULL)
    fp1 = wopen(fdname, 1);
  sptr = (char **) init_sptr;
  for (i = 0; i < inbf; i++) {
    fwrite(*(sptr +i), rc_len, 1, fp1);
    *(sptr + i) = 0;
  }
  inbf = 0;
}

/* ---------------- Open a sort work file ---------------- */
static FILE *wopen(char *name, int n)
{
  FILE * fp;
  strcpy(name, "sortwork.000");
  name[strlen(name) - 1] += n;
  if ((fp = fopen(name, "wb+")) == NULL)    {
    fprintf(stderr, "\nFile error");
    exit(1);
  }
  return fp;
}

/* -------- Function to get sorted records ----------------
 This is called to get sorted records after the sort is done.
 It returns pointers to each sorted record.
 Each call to it return one record.
 When there are no more records, it returns NULL. ------ */

void *sort_op(void)
{
  int j = 0;
  int nrd, i, k, l;
  struct bp *rr;
  static int r1 = 0;
  char *rtn;
  long ad, tr;

  sptr = (char **) init_sptr;
  if (no_seq < 2)    {
    /* -- with only 1 sequence, no merge has been done -- */
    if (r1 == totrcd)    {
      free(init_sptr);
      fp1 = fp2 = NULL;
      r1 = 0;
      return NULL;
    }
    return *(sptr + r1++);
  }
  rr = (struct bp *) init_sptr;
  for (i = 0; i < no_seq; i++)
    j |= (rr + i)->rbuf | (rr + i)->rdsk;

  /* -- j will be true if any sequence still has no records - */
  if (!j)    {
    fclose(fp1);        /* none left */
    remove(fdname);
    if (fp2)    {
      fclose(fp2);
      remove(f2name);
    }
    free(init_sptr);
    fp1 = fp2 = NULL;
    r1 = 0;
    return NULL;
  }
  k = 0;

  /* --- find the sequence in the merge buffer
     with the lowest record --- */
  for (i=0; i < no_seq; i ++)
    k = ((comp( &(rr + k)->rc, &(rr +i)->rc) < 0) ? k : i);

  /* --- k is an integer sequence number that ofsets to the 
     sequence with the lowest record */

  (rr + k)->rbuf--;          /* decremente the rcd counter */
  rtn = (rr + k)->rc;        /* set the return pointer */
  (rr + k)->rc += rc_len;
  if ((rr + k)->rbuf == 0)     {
    /* ---- the sequence got empty ---- */
    /* --- so get some more if there are any */
    rtn = bf + k * rcds_seq * rc_len;
    memmove(rtn, (rr + k)->rc - rc_len, rc_len);
    (rr + k)->rc = rtn + rc_len;
    if ((rr + k)->rdsk != 0)    {
      l = ((rcds_seq-1) < (rr+k)->rdsk) ?
	rcds_seq-1 : (rr+k)->rdsk;
      nrd = k == no_seq - 1 ? totrcd % nrcds : nrcds;
      tr = (long) ((k * nrcds + (nrd - (rr + k)->rdsk)));
      ad = tr * rc_len;
      fseek(fp1, ad, SEEK_SET);
      fread(rtn + rc_len, l * rc_len, 1, fp1);
      (rr + k)->rbuf = l;
      (rr + k)->rdsk = l;
    }
    else
      memset((rr + k)->rc, 127, rc_len);
  }
  return rtn;
}

/* ----- appropriate availabel memory ----- */
static char *appr_mem(unsigned *h)
{
  char *buff = NULL;

  *h = (unsigned) MOSTMEM + 1024;
  while (buff == NULL && *h > LEASTMEM)    {
    *h -= 1024;
    buff = malloc(*h);
  }
  return buff;
}

/* ------ compare function for sorting, merging ------ */
static int comp(const void *a, const void *b)
{
  int i, k;

  if (**(char **)a == 127 || **(char **)b == 127)
    return (int) **(char **)a - (int) **(char **)b;
  for (i = 0; i < MAXSORTS; i++)    {
    if (f_pos[i] == -1)
      break;
    if ((k = strcasecmp((*(char **)a)+f_pos[i],
			(*(char **)b)+f_pos[i])) != 0)
      return (!sort_direction) ? -k : k;
  }
  return 0;
}
