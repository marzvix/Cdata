/* -- pg 149 ------ btree.c ------------------------------- */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "cdata.h"
#include "btree.h"

#define KLEN bheader[trx].keylength
#define ENTLN (KLEN+ADR)

HEADER bheader[MXTREES];
BTREE trnode;

static FILE *fp[MXTREES];         /* file pointers to indexes */
static RPTR currnode[MXTREES]; /* node number of current key */
static int currkno[MXTREES];      /* key number of current key */
static int trx;                   /* current tree */

/* ---------------- local prototypes ----------------------- */
static int btreescan(RPTR *, char *, char **);
static int nodescan(char *, char **);
static int compare_keys(char *, char *);
static RPTR fileaddr(RPTR, char *);
static RPTR leaflevel(RPTR *, char **, int *);
static void implode(BTREE *, BTREE *);
static void redist(BTREE *, BTREE *);
static void adopt(void *, int, RPTR);
static RPTR nextnode(void);
static RPTR scannext(RPTR *, char **);
static RPTR scanprev(RPTR *, char **);
static char *childptr(RPTR, RPTR, BTREE*);
static void read_node(RPTR, void *);
static void write_node(RPTR, void *);
static void bseek(RPTR);
static void memerr(void);

/* -------- initiate b-tree processing ------------------- */
int btree_init(char *ndx_name)
{
  for (trx = 0; trx < MXTREES; trx++)
    if ( fp [trx] == NULL )
      break;
  if (trx == MXTREES)
    return ERROR;
  if ((fp[trx] = fopen(ndx_name, "rb+")) == NULL)
    return ERROR;
  fread(&bheader[trx], sizeof(HEADER), 1, fp[trx]);
  /* --- if this btree is locked, something is miss --- */
  if (bheader[trx].locked)   {
    fclose(fp[trx]);
    fp[trx] = NULL;
    return ERROR;
  }
  /* ------- lock the btree ---------------- */
  bheader[trx].locked = TRUE;
  fseek(fp[trx], 0L, SEEK_SET);
  fwrite(&bheader[trx], sizeof(HEADER), 1, fp[trx]);
  currnode[trx] = 0;
  currkno[trx] = 0;
  return trx;
}

/* -------- terminate b-tree processing ------------------ */
int btree_close(int tree)
{
  if (tree >= MXTREES || fp[tree] == 0)
    return ERROR;
  bheader[tree].locked = FALSE;
  fseek(fp[tree], 0L, SEEK_SET);
  fwrite(&bheader[tree], sizeof(HEADER),  1, fp[tree]);
  fclose(fp[tree]);
  fp[tree] = NULL;
  return OK;
}

/* -------- build a new b-tree ------------------------------ */
void build_b(char *name, int len)
{
  HEADER *bhdp;
  FILE *fp;

  if ((bhdp = malloc(NODE)) == NULL)
    memerr();
  memset(bhdp, '\0', NODE);
  bhdp->keylength = len;
  bhdp->m = ((NODE-((sizeof(int)*2)+(ADR*4)))/(len+ADR));
  bhdp->endnode = 1;
  remove(name);
  fp = fopen(name, "wb");
  fwrite(bhdp, NODE, 1, fp);
  fclose(fp);
  free(bhdp);
}

/* -------------- locate key in the b-tree ------------------ */
RPTR locate(int tree, char *k)
{
    int i, fnd = FALSE;
    RPTR t, ad;
    char *a;

    trx = tree;
    t = bheader[trx].rootnode;
    if (t) {
      read_node(t, &trnode);
      fnd = btreescan(&t, k, &a);
      ad = leaflevel(&t, &a, &i);
      if (i == trnode.keyct + 1) {
	i = 0;
	t = trnode.rtsib;
      }
      currnode[trx] = t;
      currkno[trx] = i;
    }
    return fnd ? ad : 0;
}

/* ----------- search tree -------------------------- */
static int btreescan(RPTR *t, char *k, char **a)
{
    int nl;
    do {
      if (nodescan(k, a)) {
	while (compare_keys(*a, k) == FALSE)
	  if (scanprev(t, a) == 0)
	    break;
	if (compare_keys(*a, k))
	  scannext(t, a);
	return TRUE;
      }
      nl = trnode.nonleaf;
      if (nl) {
	*t = *((RPTR *) (*a - ADR));
	read_node(*t, &trnode);
      }
    } while (nl);
    return FALSE;
}
    
/* ----------- search node -------------------------- */
static int nodescan(char *keyvalue, char **nodeadr)
{
  int i;
  int result;
  
  *nodeadr = trnode.keyspace;
  for (i = 0; i < trnode.keyct; i++)   {
    result = compare_keys(keyvalue, *nodeadr);
    if (result == FALSE)
      return TRUE;
    if (result < 0)
      return FALSE;
    *nodeadr += ENTLN;
  }
  return FALSE;
}

/* ----------- compare keys ----------------------- */    
static int compare_keys(char *a, char *b)
{
  int len = KLEN, cm;
  
    while (len--)
      if ((cm = (int) *a++  - (int) *b++) != 0)
	break;
    return cm;
}

/* ----------- compute current file address-------- */    
static RPTR fileaddr(RPTR t, char *a)
{
  RPTR cn, ti;
  int i;

  ti = t;
  cn  = leaflevel(&ti, &a, &i);
  read_node(t, &trnode);
  return cn;
}

/* ----------- Navigate down to leaf level -------- */        
static RPTR leaflevel(RPTR *t, char **a, int *p)
{
  if (trnode.nonleaf == FALSE)   { /* already at a leaf */
    *p = (*a - trnode.keyspace) / ENTLN + 1;
    return *((RPTR *) (*a + KLEN));
  }
  *p = 0;
  *t = *((RPTR *) (*a + KLEN));
  read_node(*t, &trnode); // <<< --- debug 2
  *a = trnode.keyspace;
  while (trnode.nonleaf)    {
    *t = trnode.key0;
    read_node(*t, &trnode);
  }
  return trnode.key0;
}
    
/* ----------- delete a key ----------------------- */        
int deletekey(int tree, char *x, RPTR ad)
{
  BTREE *qp, *yp;
  int rt_len, comb;
  RPTR p, adr, q, *b, y, z;
  char *a;

  trx = tree;
  if (trx >= MXTREES || fp[trx] == 0)
    return ERROR;
  p = bheader[trx].rootnode;
  if (p == 0)
    return OK;
  read_node(p, &trnode);
  if (btreescan(&p, x, &a) == FALSE)
    return OK;
  adr = fileaddr(p, a);
  while (adr != ad)    {
    adr = scannext(&p, &a);
    if (compare_keys(a, x))
      return OK;
  }
  if (trnode.nonleaf)    {
    b = (RPTR *) (a + KLEN);
    q = *b;
    if ((qp = malloc(NODE)) == NULL)
      memerr();
    read_node(q, qp);
    while (qp->nonleaf)    {
      q = qp->key0;
      read_node(q, qp);
    }
    /* move the left-most key from the leaf
       to where the deleted key is */
    memmove(a, qp->keyspace, KLEN);
    write_node(p, &trnode);
    p = q;
    trnode.key0 = *b;
    a = trnode.keyspace;
    b = (RPTR *) (a + KLEN);
    trnode.key0 = *b;
    free(qp);
  }
  currnode[trx] = p;
  currkno[trx] = (a - trnode.keyspace) / ENTLN;
  rt_len = (trnode.keyspace + (bheader[trx].m * ENTLN)) - a;
  memmove(a, a+ENTLN, rt_len);
  memset(a+rt_len, '\0', ENTLN);
  trnode.keyct--;
  if (currkno[trx] > trnode.keyct)    {
    if (trnode.rtsib)    {
      currnode[trx] = trnode.rtsib;
      currkno[trx] = 0;
    }
    else
      currkno[trx]--;
  }
  while (trnode.keyct <= bheader[trx].m / 2 &&
	 p != bheader[trx].rootnode)    {
    comb = FALSE;
    z = trnode.prntnode;
    if ((yp = malloc(NODE)) == NULL)
      memerr();
    if (trnode.rtsib)    {
      y = trnode.rtsib;
      read_node(y, yp);
      if (yp->keyct + trnode.keyct <
	  bheader[trx].m && yp->prntnode == z)    {
	comb = TRUE;
	implode(&trnode, yp);
      }
    }
    if (comb == FALSE && trnode.lfsib)    {
      y = trnode.lfsib;
      read_node(y, yp);
      if (yp->prntnode == z)    {
	if (yp->keyct + trnode.keyct <
	    bheader[trx].m)    {
	  comb = TRUE;
	  implode(yp, &trnode);
	}
	else    {
	  redist(yp, &trnode);
	  write_node(p, &trnode);
	  write_node(p, yp);
	  free(yp);
	  return OK;
	}
      }
    }
    if (comb == FALSE)    {
      y = trnode.rtsib;
      read_node(y, yp);
      redist(&trnode, yp);
      write_node(y, yp);
      write_node(p, &trnode);
      free(yp);
      return OK;
    }
    free(yp);
    p = z;
    read_node(p, &trnode);
  }
  if (trnode.keyct == 0)    {
    bheader[trx].rootnode = trnode.key0;
    trnode.nonleaf = FALSE;
    trnode.key0 = 0;
    trnode.prntnode = bheader[trx].rised_node;
    bheader[trx].rised_node = p;
  }
  if (bheader[trx].rootnode == 0)
    bheader[trx].rightmost = bheader[trx].leftmost = 0;
  write_node(p, &trnode);
  return OK;
}

/* ----------- Combine two sibiling nodes -------------- */        
static void implode(BTREE *left, BTREE *right)
{
  RPTR lf, rt, p;
  int rt_len, lf_len;
  char *a;
  RPTR *b;
  BTREE *par;
  RPTR c;
  char *j;

  lf = right->lfsib;
  rt = left->rtsib;
  p = left->prntnode;
  if ((par = malloc(NODE)) == NULL)
    memerr();
  j = childptr(lf, p, par);
  /* --- move key from parent to end of left sibiling --- */
  lf_len = left->keyct * ENTLN;
  a = left->keyspace + lf_len;
  memmove(a, j, KLEN);
  memmove(j, '\0', ENTLN);
  /* --- move keys from right sibling to left --- */
  b = (RPTR *) (a + KLEN);
  *b = right->key0;
  rt_len = right->keyct * ENTLN;
  a = (char *) (b + 1);
  memmove(a, right->keyspace, rt_len);
  /* --- point lower node to their new parent --- */
  if (left->nonleaf)
    adopt(b, right->keyct + 1, lf);
  /* --- if global key poiter -> to the right sibiling
     change to -> left --- */
  if (currnode[trx] == left->rtsib)    {
    currnode[trx] = right->lfsib;
    currkno[trx] += left->keyct + 1;
  }
  /* --- update control values in left sibiling mode --- */
  left->keyct += right->keyct + 1;
  c = bheader[trx].rised_node;
  bheader[trx].rised_node = left->rtsib;
  if (bheader[trx].rightmost == left->rtsib)
    bheader[trx].rightmost = right->lfsib;
  left->rtsib = right->rtsib;
  /* --- point the deleted node's right brother
     to this left brother --- */
  if (left->rtsib)    {
    read_node(left->rtsib, right);
    right->lfsib = lf;
    write_node(left->rtsib, right);
  }
  memset(right, '\0', NODE);
  right->prntnode = c;
  /* --- remove key from parent node --- */
  par->keyct--;
  if(par->keyct == 0)
    left->prntnode = 0;
  else   {
    rt_len = par->keyspace + (par->keyct * ENTLN) - j;
    memmove(j, j+ENTLN, rt_len);
  }
  write_node(lf, left);
  write_node(rt, right);
  write_node(p, par);
  free(par);
}

/* ----------- insert a key ----------------------- */
int insertkey(int tree, char *x, RPTR ad, int unique)
{
  char k[MXKEYLEN + 1], *a;
  BTREE *yp;
  BTREE *bp;
  int nl_flag, rt_len, j;
  RPTR t, p, sv;
  RPTR *b;
  int lshft, rshft;

  trx = tree;
  if (trx >= MXTREES || fp[trx] == 0)
    return ERROR;
  p = 0;
  sv = 0;
  nl_flag = 0;
  memmove(k, x, KLEN);
  t = bheader[trx].rootnode;
  /* ---- find insertion point --------------------------- */
  if (t)    {
    read_node(t, &trnode);
    if (btreescan(&t, k, &a))    {
      if (unique)
	return ERROR;
      else   {
	leaflevel(&t, &a, &j);  /// <<<<-----debug
	currkno[trx] = j;
      }
    }
    else
      currkno[trx] = ((a - trnode.keyspace) / ENTLN)+1;
    currnode[trx] = t;
  }
  /* ---- insert key into leaf node ---------------------- */  
  while (t) {
    nl_flag = 1;
    rt_len = (trnode.keyspace+(bheader[trx].m*ENTLN))-a;
    memmove(a+ENTLN, a, rt_len);
    memmove(a, k, KLEN);
    b = (RPTR *) (a + KLEN);
    *b = ad;
    if (trnode.nonleaf == FALSE)    {
      currnode[trx] = t;
      currkno[trx] = ((a - trnode.keyspace) / ENTLN)+1;
    }
    trnode.keyct++;
    if (trnode.keyct <= bheader[trx].m) {
      write_node(t, &trnode);
      return OK;
    }
    /* ---- redistribute keys between sibiling nodes ----- */
    lshft = FALSE;
    rshft = FALSE;
    if ((yp = malloc(NODE)) == NULL)
      memerr();
    if (trnode.lfsib)    {
      read_node(trnode.lfsib, yp);
      if (yp->keyct < bheader[trx].m &&
	  yp->prntnode == trnode.prntnode)    {
	lshft = TRUE;
	redist(yp, &trnode);
	write_node(trnode.lfsib, yp);
      }
    }
    if (lshft == FALSE && trnode.rtsib)    {
      read_node(trnode.rtsib, yp);
      if (yp->keyct < bheader[trx].m &&
	  yp->prntnode == trnode.prntnode)    {
	rshft = TRUE;
	redist(&trnode, yp);
	write_node(trnode.rtsib, yp);
      }
    }
    free(yp);
    if (lshft || rshft)    {
      write_node(t, &trnode);
      return OK;
    }
    p = nextnode();
    /* ---- split node ----------------------------------- */
    if((bp = malloc(NODE)) == NULL)
      memerr();
    memset(bp, '\0', NODE);
    trnode.keyct = (bheader[trx].m + 1) / 2;
    b = (RPTR*)
      (trnode.keyspace+((trnode.keyct+1)*ENTLN)-ADR);
    bp->key0 = *b;
    bp->keyct = bheader[trx].m - trnode.keyct;
    rt_len = bp->keyct * ENTLN;
    a = (char *) ( b + 1);
    memmove(bp->keyspace, a, rt_len);
    bp->rtsib = trnode.rtsib;
    trnode.rtsib = p;
    bp->lfsib = t;
    bp->nonleaf = trnode.nonleaf;
    a -= ENTLN;
    memmove(k, a, KLEN);
    memset(a, '\0', rt_len+ENTLN);
    if (bheader[trx].rightmost == t)
      bheader[trx].rightmost = p;
    if (t == currnode[trx] &&
	currkno[trx]>trnode.keyct) {
      currnode[trx] = p;
      currkno[trx] -= trnode.keyct + 1;
    }
    ad = p;
    sv = t;
    t = trnode.prntnode;
    if (t)
      bp->prntnode = t;
    else    {
      p = nextnode();
      trnode.prntnode = p;
      bp->prntnode = p;
    }
    write_node(ad, bp);
    if (bp->rtsib)    {
      if ((yp = malloc(NODE)) == NULL)
	memerr();
      read_node(bp->rtsib, yp);
      yp->lfsib = ad;
      write_node(bp->rtsib, yp);
      free(yp);
    }
    if (bp->nonleaf)
      adopt(&bp->key0, bp->keyct + 1, ad);
    write_node(sv, &trnode);
    if (t)    {
      read_node(t, &trnode);
      a = trnode.keyspace;
      b = &trnode.key0;
      while (*b != bp->lfsib)    {
	a += ENTLN;
	b = (RPTR *) (a - ADR);
      }
    }
    free(bp);
  }
  /* ----------------- new root --------------------- */
  if (p == 0)
    p = nextnode();
  if ((bp = malloc(NODE)) == NULL)
    memerr();
  memset(bp, '\0', NODE);
  bp->nonleaf = nl_flag;
  bp->prntnode = 0;
  bp->rtsib = 0;
  bp->lfsib = 0;
  bp->keyct = 1;
  bp->key0 = sv;
  *((RPTR *) (bp->keyspace + KLEN)) = ad;
  memmove(bp->keyspace, k, KLEN);
  write_node(p, bp);
  free(bp); /* <<<<<<<<<<<------- quebra aqui */
  bheader[trx].rootnode = p;
  if (nl_flag == FALSE)    {
    bheader[trx].rightmost = p;
    bheader[trx].leftmost = p;
    currnode[trx] = p;
    currkno[trx] = 1;
  }
  return OK;
}

/* ----- redistribute key in sibiling nodes ------- */
static void redist(BTREE *left, BTREE *right)
{
  int n1, n2, len;
  RPTR z;
  char *c, *d, *e;
  BTREE *zp;

  n1 = (left->keyct + right->keyct) / 2;
  if (n1 == left->keyct)
    return;
  n2 = (left->keyct + right->keyct) - n1;
  z = left->prntnode;
  if ((zp = malloc(NODE)) == NULL)
    memerr();
  c = childptr(right->lfsib, z, zp);
  if (left->keyct < right->keyct)    {
    d = left->keyspace + (left->keyct * ENTLN);
    memmove(d, c, KLEN);
    d += KLEN;
    e = right->keyspace - ADR;
    len = ((right->keyct - n2 - 1) * ENTLN) + ADR;
    memmove(d, e, len);
    if (left->nonleaf)
      adopt(d, right->keyct - n2, right->lfsib);
    e += len;
    memmove(c, e, KLEN);
    e += KLEN;
    d = right->keyspace - ADR;
    len = (n2 * ENTLN) + ADR;
    memmove(d, e, len);
    memset(d+len, '\0', e-d);
    if(right->nonleaf == 0 &&
       left->rtsib == currnode[trx])
      if (currkno[trx] < right->keyct - n2)     {
	currnode[trx] = right->lfsib;
	currkno[trx] += n1 + 1;
      }
      else
	currkno[trx] -= right->keyct - n2;
  }
  else {
    e = right->keyspace+((n2-right->keyct)*ENTLN)-ADR;
    memmove(e, right->keyspace-ADR,
	    (right->keyct * ENTLN) + ADR);
    e -= KLEN;
    memmove(e, c, KLEN);
    d = left->keyspace + (n1 * ENTLN);
    memmove(c, d, KLEN);
    memset(d, '\0', KLEN);
    d += KLEN;
    len = ((left->keyct - n1 - 1) * ENTLN  + ADR);
    memmove(right->keyspace-ADR, d, len);
    memset(d, '\0', len);
    if (right->nonleaf)
      adopt(right->keyspace - ADR,
	    left->keyct - n1 , left->rtsib);
	if (left->nonleaf == FALSE)
	  if (right->lfsib == currnode[trx] &&
	      currkno[trx] > n1) {
	    currnode[trx] = left->rtsib;
	    currkno[trx] -= n1 + 1;
	  }
	  else if (left->rtsib == currnode[trx])
	    currkno[trx] += left->keyct - n1;
  }
  right->keyct = n2;
  left->keyct = n1;
  write_node(z, zp);
  free(zp);
}

/* ----- assign new parents to child nodes ------- */
static void adopt(void *ad, int kct, RPTR newp)
{
    char *cp;
    BTREE *tmp;

    if ((tmp = malloc(NODE)) == NULL)
        memerr();
    while (kct--)   {
        read_node(*(RPTR *)ad, tmp);
        tmp->prntnode = newp;
        write_node(*(RPTR *)ad, tmp);
        cp = ad;
        cp += ENTLN;
        ad = cp;
    }
    free(tmp);
}

/* ----- compute node address for a new node ------ */
static RPTR nextnode(void)
{
    RPTR p;
    BTREE *nb;

    if (bheader[trx].rised_node)   {
        if ((nb = malloc(NODE)) == NULL)
            memerr();
        p = bheader[trx].rised_node;
        read_node(p, nb);
        bheader[trx].rised_node = nb->prntnode;
        free(nb);
    }
    else
        p = bheader[trx].endnode++;
    return p;
}

/* ----- next sequential key --------------- ------ */
RPTR nextkey(int tree)
{
    trx = tree;
    if (currnode[trx] == 0)
      return firstkey(trx);
    read_node(currnode[trx], &trnode);
    if (currkno[trx] == trnode.keyct)    {
      if (trnode.rtsib == 0)    {
	return 0;
      }
      currnode[trx] = trnode.rtsib;
      currkno[trx] = 0;
      read_node(trnode.rtsib, &trnode);
    }
    else
      currkno[trx]++;
    return *((RPTR *)
	     (trnode.keyspace+(currkno[trx]*ENTLN)-ADR));
}

/* ----- prev sequential key -------------- ------ */
RPTR prevkey(int tree)
{
    trx = tree;
    if (currnode[trx] == 0)
      return lastkey(trx);
    read_node(currnode[trx], &trnode);
    if (currkno[trx] == 0)    {
      if (trnode.lfsib == 0)
	return 0;
      currnode[trx] = trnode.lfsib;
      read_node(trnode.lfsib, &trnode);
      currkno[trx] = trnode.keyct;
    }
    else
      currkno[trx]--;
    return *((RPTR *)
	     (trnode.keyspace + (currkno[trx] * ENTLN) - ADR));
}

/* ----- first key ---------------------- */
RPTR firstkey(int tree)
{
    trx = tree;
    if (bheader[trx].leftmost == 0)
      return 0;
    read_node(bheader[trx].leftmost, &trnode);
    currnode[trx] = bheader[trx].leftmost;
    currkno[trx] = 1;
    return *((RPTR *) (trnode.keyspace + KLEN));
}

/* ----- last key ---------------------- */
RPTR lastkey(int tree)
{
    trx = tree;
    if (bheader[trx].rightmost == 0)
      return 0;
    read_node(bheader[trx].rightmost, &trnode);
    currnode[trx] = bheader[trx].rightmost;
    currkno[trx] = trnode.keyct;
    return *((RPTR *)
	     (trnode.keyspace + (trnode.keyct * ENTLN) - ADR));
}

/* ----- scan to the next sequential key ----- */
static RPTR scannext(RPTR *p, char **a)
{
  RPTR cn;

  if (trnode.nonleaf)    {
    *p = *((RPTR *) (*a + KLEN));
    read_node(*p, &trnode);
    while (trnode.nonleaf)    {
      *p = trnode.key0;
      read_node(*p, &trnode);
    }
    *a = trnode.keyspace;
    return *((RPTR *) (*a + KLEN));
  }
  *a += ENTLN;
  while (-1)    {
    if ((trnode.keyspace + (trnode.keyct)
	 * ENTLN) != *a)
      return fileaddr(*p, *a);
    if (trnode.prntnode == 0 || trnode.rtsib == 0)
      break;
    cn = *p;
    *p = trnode.prntnode;
    read_node(*p, &trnode);
    *a = trnode.keyspace;
    while (*((RPTR *) (*a - ADR)) != cn)
      *a += ENTLN;
  }
  return 0;
}

/* ----- scan to the next sequential key ----- */
static RPTR scanprev(RPTR *p, char **a)
{
  RPTR cn;

  if (trnode.nonleaf)    {
    *p = *((RPTR *) (*a - ADR));
    read_node(*p, &trnode);
    while (trnode.nonleaf)    {
      *p = *((RPTR *)
	     (trnode.keyspace+(trnode.keyct)*ENTLN-ADR));
      read_node(*p, &trnode);
    }
    *a = trnode.keyspace + (trnode.keyct - 1) * ENTLN;
    return *((RPTR *) (*a + KLEN));
  }
  while (-1)    {
    if (trnode.keyspace != *a) {
      *a -= ENTLN;
      return fileaddr(*p, *a);
    }
    if (trnode.prntnode == 0 || trnode.lfsib == 0)
      break;
    cn = *p;
    *p = trnode.prntnode;
    read_node(*p, &trnode);
    *a = trnode.keyspace;
    while(*((RPTR *) (*a - ADR)) != cn)
      *a += ENTLN;
  }
  return 0;
}

/* -------- locate pointer to child ---------- */
static char *childptr(RPTR left, RPTR parent, BTREE *btp)
{
    char *c;

    read_node(parent, btp);
    c = btp->keyspace;
    while (*((RPTR *) (c - ADR)) != left)
        c += ENTLN;
    return c;
}

/* -------- current key value ---------------- */
void keyval(int tree, char *ky)
{
    RPTR b, p;
    char *k;
    int i;
    
    trx = tree;
    b = currnode[trx];
    if (b)    {
      read_node(b, &trnode);
      i = currkno[trx];
      k = trnode.keyspace + ((i - 1) * ENTLN);
      while(i == 0)    {
	p = b;
	b = trnode.prntnode;
	read_node(b, &trnode);
	for (; i <= trnode.keyct; i++)    {
	  k = trnode.keyspace + ((i - 1) * ENTLN);
	  if (*((RPTR *) (k + KLEN)) == p)
	    break;
	}
      }
      memmove(ky, k, KLEN);
    }
}
    
/* -------- current key ---------------------- */
RPTR currkey(int tree)
{
    RPTR f = 0;

    trx = tree;
    if (currnode[trx])    {
        read_node(currnode[trx], &trnode);
        f = *( (RPTR *)
               (trnode.keyspace+(currkno[trx]*ENTLN)-ADR));
    }
    return f;
}

/* -------- read a b-tree node --------------- */
static void read_node(RPTR nd, void *bf)
{
    bseek(nd);
    fread(bf, NODE, 1, fp[trx]);
    if (ferror(fp[trx]))    {
        errno = D_IOERR;
        dberror();
    }
}

/* -------- write a b-tree node --------------- */
static void write_node(RPTR nd, void *bf)
{
    bseek(nd);
    fwrite(bf, NODE, 1, fp[trx]);
    if (ferror(fp[trx]))    {
        errno = D_IOERR;
        dberror();
    }
}

/* -------- seek to the  b-tree node --------------- */
static void bseek(RPTR nd)
{
    if (fseek(fp[trx],
              (long) (NODE+((nd-1)*NODE)), SEEK_SET) == ERROR) {
        errno = D_IOERR;
        dberror();
    }
}

/* -------- out of memory error -------------------- */
static void memerr(void)
{
    errno = D_OM;
    dberror();
}
