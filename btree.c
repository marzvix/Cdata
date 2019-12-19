/* --------------------- btree.c ----------------------- */
#include <stdio.h>
#include "cdata.h"

#define MXTREES 20
#define ADR sizeof(RPTR)
#define KLEN bheader[trx].keylength
#define ENTLN (KLEN+ADR)

/* --------- the btree node structure -------------- */
typedef struct treenode	{
	int nonleaf;	/* 0 if leaf, 1 if non-leaf  		  */
	RPTR prntnode;	/* parent node 						  */
	RPTR lfsib;		/* left sibling node  				  */			  	
	RPTR rtsib;		/* right sibling node  				  */
	int keyct;		/* number of keys 					  */
	RPTR key0;		/* node # of keys < 1st key this node */
	char keyspace [NODE - ((sizeof(int) * 2) + (ADR * 4))];
	char spil [MXKEYLEN];  /* for insertion excess */
} BTREE;

/* ---- the structure of the btree header node --------- */
typedef struct treehdr	{
	RPTR rootnode;			/* root node number */
	int keylength;
	int m;					/* max keys/node  */
	RPTR rlsed_node;		/* next released node */
	RPTR endnode;			/* next unassigned node  */
	int locked;				/* if btree is locked  */
	RPTR leftmost;			/* left-most node */
	RPTR rightmost;			/* right-most node */
} HEADER;

HEADER bheader [MXTREES];
BTREE trnode;

int handle  	[MXTREES];	/* handle of each index in use */
RPTR currnode 	[MXTREES];	/* node number of current key  */
int currkno 	[MXTREES];	/* key number of current key   */
int trx;					/* current tree */

/* char *malloc(), */
static char *childptr();
static void redist(),adopt(),implode(),
	 read_node(),write_node(),bseek();

RPTR firstkey();
RPTR lastkey();

static RPTR leaflevel();
static int btreescan();
static int nodescan();
static int compare_keys();
static RPTR fileaddr();
static void implode();
static void adopt();
static RPTR nextnode();
static RPTR scannext();
static RPTR scanprev();
static char *childptr();
static void read_node();
static void write_node();
static void bseek();
  
/* -------- initiate b-tree processing ---------- */
int btree_init(ndx_name)
char *ndx_name;
{
#if COMPILER == MICROSOFT
	extern int _iomode;
	_iomode = 0x8000;
#endif
#if COMPILER == LATTICE
	extern int _iomode;
	_iomode = 0x8000;
#endif
	for (trx = 0; trx < MXTREES; trx++)
		if (handle [trx] == 0)
			break;
	if (trx == MXTREES)
		return ERROR;
	if ((handle [trx] = open(ndx_name, OPENMODE)) == ERROR)
		return ERROR;
	lseek(handle [trx], 0L, 0);
	read(handle [trx],(char *) &bheader[trx],sizeof(HEADER));
	if (bheader[trx].locked)	{
		close(handle [trx]);
		handle [trx] = 0;
		return ERROR;
	}
	bheader[trx].locked = TRUE;
	lseek(handle [trx], 0L, 0);
	write(handle [trx],(char *) &bheader[trx],sizeof(HEADER));
	currnode [trx] = 0;
	currkno [trx] = 0;
	return trx;
}
/* ----------- terminate b tree processing ------------- */
int btree_close(tree)
int tree;
{
	if (tree >= MXTREES || handle [tree] == 0)
		return ERROR;
	bheader[tree].locked = FALSE;
	lseek(handle[tree], 0L, 0);
	write(handle[tree],(char *)&bheader[tree],sizeof(HEADER));
	close(handle[tree]);
	handle[tree] = 0;
	return OK;
}

/* --------Build a new b-tree ------------------ */
void build_b(name, len)
char *name;
int len;
{
	HEADER *bhdp;
	int fd;
#if COMPILER == MICROSOFT
	extern int _iomode;
	_iomode = 0x8000;
#endif
#if COMPILER == LATTICE
	extern int _iomode;
	_iomode = 0x8000;
#endif

	if ((bhdp = (HEADER *)malloc(NODE))==(HEADER *)NULL)	{
		errno = D_OM;
		dberror();
	}
	set_mem(bhdp, NODE, '\0');
	bhdp->keylength = len;
	bhdp->m = ((NODE-((sizeof(int)*2)+(ADR*4)))/(len+ADR));
	bhdp->endnode = 1;
	unlink(name);
	fd = creat(name, CMODE);
	close(fd);
	fd = open(name, OPENMODE);
	write(fd, (char *) bhdp, NODE);
	close(fd);
	free(bhdp);
}

/* --------------- Locate key in the b-tree -------------- */
RPTR locate(tree, k)
int tree;
char *k;
{
	int i, fnd = FALSE;
	RPTR t, ad;
	char *a;

	trx = tree;
	t = bheader[trx].rootnode;
	if (t)	{
		read_node(t, &trnode);
		fnd = btreescan(&t, k, &a);
		ad = leaflevel(&t, &a, &i);
		if (i == trnode.keyct + 1)	{
			i = 0;
			t = trnode.rtsib;
		}
		currnode [trx] = t;
		currkno [trx] = i;
	}
	return fnd ? ad : (RPTR) 0;
}

/* ----------- Search tree ------------- */
static int btreescan(t, k, a)
RPTR *t;
char *k, **a;
{
	int nl;
	do	{
		if (nodescan(k, a))	{
			while (compare_keys(*a, k) == FALSE)
				if (scanprev(t, a) == 0)
					break;
			if (compare_keys(*a, k))
				scannext(t, a);
			return TRUE;
		}
		nl = trnode.nonleaf;
		if (nl)	{
 			*t = *((RPTR *) (*a - ADR));
			read_node(*t, &trnode);
		}
	}	while (nl);
	return FALSE;
}

/* ------------------ Search node ------------ */
static int nodescan(keyvalue, nodeadr)
char *keyvalue, **nodeadr;
{
	int i;
	int result;

	*nodeadr = trnode.keyspace;
	for (i = 0; i < trnode.keyct; i++)	{
		result = compare_keys(keyvalue, *nodeadr);
		if (result == FALSE) return TRUE;
		if (result < 0) return FALSE;
		*nodeadr += ENTLN;
	}
	return FALSE;
}

/* ------------- Compare keys ----------- */
static int compare_keys(a, b)
char *a, *b;
{
	int len = KLEN, cm;

	while (len--)
		if ((cm = (int) *a++ - (int) *b++) != 0)
			break;
	return cm;
}

/* ------------ Compute current file address  ------------ */
static RPTR fileaddr(t, a)
RPTR t;
char *a;
{
	RPTR cn, ti;
	int i;

	ti = t;
	cn = leaflevel(&ti, &a, &i);
	read_node(t, &trnode);
	return cn;
}

/* ---------------- Navigate down to leaf level ----------- */
static RPTR leaflevel(t, a, p)
RPTR *t;
char **a;
int *p;
{
	if (trnode.nonleaf == FALSE)	{ /* already at a leaf? */
		*p = (*a - trnode.keyspace) / ENTLN + 1;
		return *((RPTR *) (*a + KLEN));
	}
	*p = 0;
	*t = *((RPTR *) (*a + KLEN));
	read_node(*t, &trnode);
	*a = trnode.keyspace;
	while (trnode.nonleaf)	{
		*t = trnode.key0;
		read_node(*t, &trnode);
	}
	return trnode.key0;
}

/* -------------- Delete a key  ------------- */
int deletekey(tree, x, ad)
int tree;
char *x;
RPTR ad;
{
	BTREE *qp, *yp;
	int rt_len, comb;
	RPTR p, adr, q, *b, y, z;
	char *a;

	trx = tree;
	if (trx >= MXTREES || handle [trx] == 0)
		return ERROR;
	p = bheader[trx].rootnode;
	if (p == 0)
		return OK;
	read_node(p, &trnode);
	if (btreescan(&p, x, &a) == FALSE)
		return OK;
	adr = fileaddr(p, a);
	while (adr != ad)	{
		adr = scannext(&p, &a);
		if (compare_keys(a, x))
			return OK;
	}
	if (trnode.nonleaf)	{
		b = (RPTR *) (a + KLEN);
		q = *b;
		if ((qp=(BTREE *) malloc(NODE))==(BTREE *) NULL)	{
			errno = D_OM;
			dberror();
		}
		read_node(q, qp);
		while (qp->nonleaf)	{
			q = qp->key0;
			read_node(q, qp);
		}
	/* Move the left-most key from the leaf
						to where the deleted key is */
		mov_mem(qp->keyspace, a, KLEN);
		write_node(p, &trnode);
		p = q;
		mov_mem(qp, &trnode, sizeof trnode);
		a = trnode.keyspace;
		b = (RPTR *) (a + KLEN);
		trnode.key0 = *b;
		free(qp);
	}
	currnode [trx] = p;
	currkno [trx] = (a - trnode.keyspace) / ENTLN;
	rt_len = (trnode.keyspace + (bheader[trx].m * ENTLN)) - a;
	mov_mem(a + ENTLN, a, rt_len);
	set_mem(a + rt_len, ENTLN, '\0');
	trnode.keyct--;
	if (currkno [trx] > trnode.keyct)	{
		if (trnode.rtsib)	{
			currnode [trx] = trnode.rtsib;
			currkno [trx] = 0;
		}
		else
			currkno [trx]--;
	}
	while (trnode.keyct <= bheader[trx].m / 2 &&
								p != bheader[trx].rootnode)	{
		comb = FALSE;
		z = trnode.prntnode;
		if ((yp=(BTREE *) malloc(NODE))==(BTREE *) NULL)	{
			errno = D_OM;
			dberror();
		}
		if (trnode.rtsib)	{
			y = trnode.rtsib;
			read_node(y, yp);
			if (yp->keyct + trnode.keyct <
					bheader[trx].m && yp->prntnode == z)	{
				comb = TRUE;
				implode(&trnode, yp);
			}
		}
		if (comb == FALSE && trnode.lfsib)	{
			y = trnode.lfsib;
			read_node(y, yp);
			if (yp->prntnode == z)	{
				if (yp->keyct + trnode.keyct <
									bheader[trx].m) {
					comb = TRUE;
					implode(yp, &trnode);
				}
				else	{
					redist(yp, &trnode);
					write_node(p, &trnode);
					write_node(y, yp);
					free(yp);
					return OK;
				}
			}
		}
		if (comb == FALSE)	{
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
	if (trnode.keyct == 0)	{
		bheader[trx].rootnode = trnode.key0;
		trnode.nonleaf = FALSE;
		trnode.key0 = 0;
		trnode.prntnode = bheader[trx].rlsed_node;
		bheader[trx].rlsed_node = p;
	}
	if (bheader[trx].rootnode == 0)
		bheader[trx].rightmost = bheader[trx].leftmost = 0;
	write_node(p, &trnode);
	return OK;
}

/* ------------ Combine two sibling nodes. ------------- */
static void implode(left, right)
BTREE *left, *right;
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
	if ((par = (BTREE *) malloc(NODE)) == (BTREE *) NULL)	{
		errno = D_OM;
		dberror();
	}
	j = childptr(lf, p, par);
/* --- move key from parent to end of left sibling --- */ 
	lf_len = left->keyct * ENTLN;
	a = left->keyspace + lf_len;
	mov_mem(j, a, KLEN);
	set_mem(j, ENTLN, '\0');
/* --- move keys from right sibling to left --- */
	b = (RPTR *) (a + KLEN);
	*b = right->key0;
	rt_len = right->keyct * ENTLN;
	a = (char *) (b + 1);
	mov_mem(right->keyspace, a, rt_len);
/* --- point lower nodes to their new parent --- */
	if (left->nonleaf)
		adopt(b, right->keyct + 1, lf);
/* --- if global key pointers -> to the right sibling,
									change to -> left --- */
	if (currnode [trx] == left->rtsib)	{
		currnode [trx] = right->lfsib;
		currkno [trx] += left->keyct + 1;
	}
/* --- update control values in left sibling node --- */	
	left->keyct += right->keyct + 1;
	c = bheader[trx].rlsed_node;
	bheader[trx].rlsed_node = left->rtsib;
	if (bheader[trx].rightmost == left->rtsib)
		bheader[trx].rightmost = right->lfsib;
	left->rtsib = right->rtsib;
	set_mem(right, NODE, '\0');
	right->prntnode = c;
/* --- point the deleted node's right brother
								to this left brother --- */
	if (left->rtsib)	{
		read_node(left->rtsib, right);
		right->lfsib = lf;
		write_node(left->rtsib, right);
	}
/* --- remove key from parent node --- */
	par->keyct--;
	if (par->keyct == 0)
		left->prntnode = 0;
	else	{
		rt_len = par->keyspace + (par->keyct * ENTLN) - j;
		mov_mem(j + ENTLN, j, rt_len);
	}
	write_node(lf, left);
	write_node(rt, right);
	write_node(p, par);
	free(par);
}







/* ------------------ Insert key  ------------------- */
int insertkey(tree, x, ad, unique)
int tree;
char *x;
RPTR ad;
int unique;
{
	char k [MXKEYLEN + 1], *a;
	BTREE *yp;
	BTREE *bp;
	int nl_flag, rt_len, j;
	RPTR t, p, sv;
	RPTR *b;
	int lshft, rshft;

	trx = tree;
	if (trx >= MXTREES || handle [trx] == 0)
		return ERROR;
	p = 0;
	sv = 0;
	nl_flag = 0;
	mov_mem(x, k, KLEN);
	t = bheader[trx].rootnode;
	/* --------------- Find insertion point ------- */
	if (t)	{
		read_node(t, &trnode);
		if (btreescan(&t, k, &a))	{
			if (unique)
				return ERROR;
			else	{
				leaflevel(&t, &a, &j);
				currkno [trx] = j;
			}
		}
		else
			currkno [trx] = ((a - trnode.keyspace) / ENTLN)+1;
		currnode [trx] = t;
	}
	/* --------- Insert key into leaf node -------------- */
	while (t)	{
		nl_flag = 1;
		rt_len = (trnode.keyspace+(bheader[trx].m*ENTLN))-a;
		mov_mem(a, a + ENTLN, rt_len);
		mov_mem(k, a, KLEN);
		b = (RPTR *) (a + KLEN);
		*b = ad;
		if (trnode.nonleaf == FALSE)	{
			currnode [trx] = t;
			currkno [trx] = ((a - trnode.keyspace) / ENTLN)+1;
		}
		trnode.keyct++;
		if (trnode.keyct <= bheader[trx].m)	{
			write_node(t, &trnode);
			return OK;
		}
		/* --- Redistribute keys between sibling nodes ---*/
		lshft = FALSE;
		rshft = FALSE;
		if ((yp=(BTREE *) malloc(NODE))==(BTREE *) NULL)	{
			errno = D_OM;
			dberror();
		}
		if (trnode.lfsib)	{
			read_node(trnode.lfsib, yp);
			if (yp->keyct < bheader[trx].m &&
						yp->prntnode == trnode.prntnode)	{
				lshft = TRUE;
				redist(yp, &trnode);
				write_node(trnode.lfsib, yp);
			}
		}
		if (lshft == FALSE && trnode.rtsib)	{
			read_node(trnode.rtsib, yp);
			if (yp->keyct < bheader[trx].m &&
						yp->prntnode == trnode.prntnode)	{
				rshft = TRUE;
				redist(&trnode, yp);
				write_node(trnode.rtsib, yp);
			}
		}
		free(yp);
		if (lshft || rshft)	{
			write_node(t, &trnode);
			return OK;
		}
		p = nextnode();
		/* ----------- Split node -------------------- */
		if ((bp = (BTREE *) malloc(NODE))==(BTREE *) NULL)	{
			errno = D_OM;
			dberror();
		}
		set_mem(bp, NODE, '\0');
		trnode.keyct = (bheader[trx].m + 1) / 2;
		b = (RPTR *)
			  (trnode.keyspace+((trnode.keyct+1)*ENTLN)-ADR);
		bp->key0 = *b;
		bp->keyct = bheader[trx].m - trnode.keyct;
		rt_len = bp->keyct * ENTLN;
		a = (char *) (b + 1);
		mov_mem(a, bp->keyspace, rt_len);
		bp->rtsib = trnode.rtsib;
		trnode.rtsib = p;
		bp->lfsib = t;
		bp->nonleaf = trnode.nonleaf;
		a -= ENTLN;
		mov_mem(a, k, KLEN);
		set_mem(a, rt_len + ENTLN, '\0');
		if (bheader[trx].rightmost == t)
			bheader[trx].rightmost = p;
		if (t == currnode [trx] &&
						currkno [trx]>trnode.keyct)	{
			currnode [trx] = p;
			currkno [trx] -= trnode.keyct + 1;
		}
		ad = p;
		sv = t;
		t = trnode.prntnode;
		if (t)
			bp->prntnode = t;
		else	{
			p = nextnode();
			trnode.prntnode = p;
			bp->prntnode = p;
		}
		write_node(ad, bp);
		if (bp->rtsib)	{
			if ((yp=(BTREE *)malloc(NODE))==(BTREE *) NULL)	{
				errno = D_OM;
				dberror();
			}
			read_node(bp->rtsib, yp);
			yp->lfsib = ad;
			write_node(bp->rtsib, yp);
			free(yp);
		}
		if (bp->nonleaf)
			adopt(&bp->key0, bp->keyct + 1, ad);
		write_node(sv, &trnode);
		if (t)	{
			read_node(t, &trnode);
			a = trnode.keyspace;
			b = &trnode.key0;
			while (*b != bp->lfsib)	{
				a += ENTLN;
				b = (RPTR *) (a - ADR);
			}
		}
		free(bp);
	}
	/* --------------------- new root -------------------- */
	if (p == 0)
		p = nextnode();
	if ((bp = (BTREE *) malloc(NODE)) == (BTREE *) NULL)	{
		errno = D_OM;
		dberror();
	}
	set_mem(bp, NODE, '\0');
	bp->nonleaf = nl_flag;
	bp->prntnode = 0;
	bp->rtsib = 0;
	bp->lfsib = 0;
	bp->keyct = 1;
	bp->key0 = sv;
	*((RPTR *) (bp->keyspace + KLEN)) = ad;
	mov_mem(k, bp->keyspace, KLEN);
	write_node(p, bp);
	free(bp);
	bheader[trx].rootnode = p;
	if (nl_flag == FALSE)	{
		bheader[trx].rightmost = p;
		bheader[trx].leftmost = p;
		currnode [trx] = p;
		currkno [trx] = 1;
	}
	return OK;
}

/* ----- redistribute keys in sibling nodes ------ */
static void redist(left, right)
BTREE *left, *right;
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
	if ((zp = (BTREE *) malloc(NODE)) == FALSE)	{
		errno = D_OM;
		dberror();
	}
	c = childptr(right->lfsib, z, zp);
	if (left->keyct < right->keyct)	{
		d = left->keyspace + (left->keyct * ENTLN);
		mov_mem(c, d, KLEN);
		d += KLEN;
		e = right->keyspace - ADR;
		len = ((right->keyct - n2 - 1) * ENTLN) + ADR;
		mov_mem(e, d, len);
		if (left->nonleaf)
			adopt(d, right->keyct - n2, right->lfsib);	
		e += len;
		mov_mem(e, c, KLEN);
		e += KLEN;
		d = right->keyspace - ADR;
		len = (n2 * ENTLN) + ADR;
		mov_mem(e, d, len);
		set_mem(d + len, e - d, '\0');
		if (right->nonleaf == 0 &&
							left->rtsib == currnode [trx])
			if (currkno [trx] < right->keyct - n2)	{
				currnode [trx] = right->lfsib;
				currkno [trx] += n1 + 1;
			}
			else
				currkno [trx] -= right->keyct - n2;
		}
	else	{
		e = right->keyspace+((n2-right->keyct)*ENTLN)-ADR;
		mov_mem(right->keyspace - ADR, e,
							(right->keyct * ENTLN) + ADR);
		e -= KLEN;
		mov_mem(c, e, KLEN);
		d = left->keyspace + (n1 * ENTLN);
		mov_mem(d, c, KLEN);
		set_mem(d, KLEN, '\0');
		d += KLEN;
		len = ((left->keyct - n1 - 1) * ENTLN) + ADR;
		mov_mem(d, right->keyspace - ADR, len);
		set_mem(d, len, '\0');
		if (right->nonleaf)
			adopt(right->keyspace - ADR,
							left->keyct - n1, left->rtsib);
		if (left->nonleaf == FALSE)
			if (right->lfsib == currnode [trx] &&
									currkno [trx] > n1)	{
				currnode [trx] = left->rtsib;
				currkno [trx] -= n1 + 1;
			}
			else if (left->rtsib == currnode [trx])
				currkno [trx] += left->keyct - n1;
	}
	right->keyct = n2;
	left ->keyct = n1;
	write_node(z, zp);
	free(zp);
}

/* ----------- assign new parents to child nodes ---------- */
static void adopt(ad, kct, newp)
RPTR *ad;
int kct;
RPTR newp;
{
	char *cp;
	BTREE *tmp;

	if ((tmp = (BTREE *) malloc(NODE)) == (BTREE *) NULL)	{
		errno = D_OM;
		dberror();
	}
	while (kct--)	{
		read_node(*ad, tmp);
		tmp->prntnode = newp;
		write_node(*ad, tmp);
		cp = (char *) ad;
		cp += ENTLN;
		ad = (RPTR *) cp;
	}
	free(tmp);
}





/* ----- compute node address for a new node -----*/
static RPTR nextnode()
{
	RPTR p;
	BTREE *nb;

	if (bheader[trx].rlsed_node)	{
		if ((nb = (BTREE *) malloc(NODE))==(BTREE *) NULL)	{
			errno = D_OM;
			dberror();
		}
		p = bheader[trx].rlsed_node;
		read_node(p, nb);
		bheader[trx].rlsed_node = nb->prntnode;
		free(nb);
	}
	else
		p = bheader[trx].endnode++;
	return p;
}

/* ----- next sequential key ------- */
RPTR nextkey(tree)
int tree;
{
	trx = tree;
	if (currnode [trx] == 0)
		return firstkey(trx);
	read_node(currnode [trx], &trnode);
	if (currkno [trx] == trnode.keyct)	{
		if (trnode.rtsib == 0)	{
			return (RPTR) 0;
		}
		currnode [trx] = trnode.rtsib;
		currkno [trx] = 0;
		read_node(trnode.rtsib, &trnode);
	}
	else
		currkno [trx]++;
	return *((RPTR *)
			(trnode.keyspace+(currkno[trx]*ENTLN)-ADR));
}

/* ----------- previous sequential key ----------- */
RPTR prevkey(tree)
int tree;
{
	trx = tree;
	if (currnode [trx] == 0)
		return lastkey(trx);
	read_node(currnode [trx], &trnode);
	if (currkno [trx] == 0)	{
		if (trnode.lfsib == 0)
			return (RPTR) 0;
		currnode [trx] = trnode.lfsib;
		read_node(trnode.lfsib, &trnode);
		currkno [trx] = trnode.keyct;
	}
	else
		currkno [trx]--;
	return *((RPTR *)
		(trnode.keyspace + (currkno [trx] * ENTLN) - ADR));
}

/* ------------- first key ------------- */
RPTR firstkey(tree)
int tree;
{
	trx = tree;
	if (bheader[trx].leftmost == 0)
		return (RPTR) 0;
	read_node(bheader[trx].leftmost, &trnode);
	currnode [trx] = bheader[trx].leftmost;
	currkno [trx] = 1;
	return *((RPTR *) (trnode.keyspace + KLEN));
}




/* ------------- last key ----------------- */
RPTR lastkey(tree)
int tree;
{
	trx = tree;
	if (bheader[trx].rightmost == 0)
		return (RPTR) 0;
	read_node(bheader[trx].rightmost, &trnode);
	currnode [trx] = bheader[trx].rightmost;
	currkno [trx] = trnode.keyct;
	return *((RPTR *)
		(trnode.keyspace + (trnode.keyct * ENTLN) - ADR));
}

/* -------- scan to the next sequential key ------ */
static RPTR scannext(p, a)
RPTR *p;
char **a;
{
	RPTR cn;

	if (trnode.nonleaf)	{
		*p = *((RPTR *) (*a + KLEN));
		read_node(*p, &trnode);
		while (trnode.nonleaf)	{
			*p = trnode.key0;
			read_node(*p, &trnode);
		}
		*a = trnode.keyspace;
		return *((RPTR *) (*a + KLEN));
	}
	*a += ENTLN;
	while (-1)	{
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
	return (RPTR) 0;
}

/* ---- scan to the previous sequential key ---- */
static RPTR scanprev(p, a)
RPTR *p;
char **a;
{
	RPTR cn;

	if (trnode.nonleaf)	{
		*p = *((RPTR *) (*a - ADR));
		read_node(*p, &trnode);
		while (trnode.nonleaf)	{
			*p = *((RPTR *)
				(trnode.keyspace+(trnode.keyct)*ENTLN-ADR));
			read_node(*p, &trnode);
		}
		*a = trnode.keyspace + (trnode.keyct - 1) * ENTLN;
		return *((RPTR *) (*a + KLEN));
	}
	while (-1)	{
		if (trnode.keyspace != *a)	{
			*a -= ENTLN;
			return fileaddr(*p, *a);
		}
		if (trnode.prntnode == 0 || trnode.lfsib == 0)
			break;
		cn = *p;
		*p = trnode.prntnode;
		read_node(*p, &trnode);
		*a = trnode.keyspace;
		while (*((RPTR *) (*a - ADR)) != cn)
			*a += ENTLN;
	}
	return (RPTR) 0;
}

/* ------ locate pointer to child ---- */
static char *childptr(left, parent, btp)
RPTR left;
RPTR parent;
BTREE *btp;
{
	char *c;

	read_node(parent, btp);
	c = btp->keyspace;
	while (*((RPTR *) (c - ADR)) != left)
		c += ENTLN;
	return c;
}

/* -------------- current key value ---------- */
void keyval(tree, ky)
int tree;
char *ky;
{
	RPTR b, p;
	char *k;
	int i;

	trx = tree;
	b = currnode [trx];
	if (b)	{
		read_node(b, &trnode);
		i = currkno [trx];
		k = trnode.keyspace + ((i - 1) * ENTLN);
		while (i == 0)	{
			p = b;
			b = trnode.prntnode;
			read_node(b, &trnode);
			for (; i <= trnode.keyct; i++)	{
				k = trnode.keyspace + ((i - 1) * ENTLN);
				if (*((RPTR *) (k + KLEN)) == p)
					break;
			}
		}
		mov_mem(k, ky, KLEN);
	}
}

/* --------------- current key ---------- */
RPTR currkey(tree)
int tree;
{
	RPTR f = 0;

	trx = tree;
	if (currnode [trx])	{
		read_node(currnode [trx], &trnode);
		f = *( (RPTR *)
			(trnode.keyspace+(currkno[trx]*ENTLN)-ADR));
	}
	return f;
}

/* ---------- read a btree node ----------- */
static void read_node(nd, bf)
RPTR nd;
BTREE *bf;
{
	bseek(nd);
	read(handle [trx], (char *) bf, NODE);
}

/* ---------- write a btree node ----------- */
static void write_node(nd, bf)
RPTR nd;
BTREE *bf;
{
	bseek(nd);
	write(handle [trx], (char *) bf, NODE);
}

/* ----------- seek to the b-tree node ---------- */
static void bseek(nd)
RPTR nd;
{
	if (lseek(handle [trx],
			(long) (NODE+((nd-1)*NODE)),0) == ERROR)	{
		errno = D_IOERR;
		dberror();
	}
}
