/*------------------ cdata.c ----------------*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "cdata.h"
#include "datafile.h"
#include "btree.h"
#include "keys.h"

void (*database_message)(void);
static void init_index(const char *, const DBFILE);
static void cls_index(DBFILE);
static void del_indexes(DBFILE, RPTR);
static int relate_rcd(DBFILE, void *);
static int data_in(char *);
static int getrcd(DBFILE, RPTR, void *);
static int rel_rcd(DBFILE, RPTR, void *);

static int db_opened = FALSE;
static int curr_fd [MXFILS];
static char *bfs [MXFILS];
static int bfd [MXFILS] [MXINDEX];
static char dbpath [64];

RPTR curr_a [MXFILS];
/* - macro to compute tree number from file and key number - */
#define treeno(f,k) (bfd[f][(k)-1])


/* -------- Cdata API functions ------------- */
void db_open(const char *, const DBFILE *);
int add_rcd(DBFILE, void *);
int find_rcd(DBFILE, int, char *, void *);
int verify_rcd(DBFILE, int, char *);
int first_rcd(DBFILE, int, void *);
int last_rcd(DBFILE, int, void *);
int next_rcd(DBFILE, int, void *);
int prev_rcd(DBFILE, int, void *);
int rtn_rcd(DBFILE, void *);
int del_rcd(DBFILE);
int curr_rcd(DBFILE, int, void *);
int seqrcd(DBFILE, void *);
void db_cls(void);
void dberror(void);
int rlen(DBFILE);
void init_rcd(DBFILE, void *);
void cirrcd(void *, const ELEMENT *);
int epos(ELEMENT, const ELEMENT *);
void rcd_fill(const void *, void *, const ELEMENT *,
              const ELEMENT *);

/* ------ functions user by Cdata utility programs ------ */
void build_index(char *, DBFILE);
int add_indexes(DBFILE, void *, RPTR);
DBFILE filename(char *);
void name_cvt(char *, char *);
int ellist(int, char **, ELEMENT *);
void clist(FILE *, const char *, const ELEMENT *,
            void *, const char *);
void test_eop(FILE *, const char *, const ELEMENT *);
void dblist(FILE *, DBFILE, int, const ELEMENT *);
