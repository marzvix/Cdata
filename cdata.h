/*-- pg 82 --------- cdata.h ----------------*/

#include <stdio.h>

#ifndef CDATA_H
#define CDATA_H

#define MXFILS    11     /* maximum files in a data base */
#define MXELE    100     /* maximum data elements in a file */
#define MXINDEX    5     /* maximum index per file */
#define MXKEYLEN  80     /* maximum keylengh for index */
#define MXCAT      3     /* maximum elments perindex */
#define NAMELEN   31     /* data element name length */

/* initialize tyhos to call your function for data base errors */
extern void (*database_message)(void);
#define ERROR -1
#ifndef OK
#define OK 0
#endif
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif  /* TRUE */

typedef int RPTR;        /* B-tree node and file address   */

#ifndef APPLICATION_H
typedef int DBFILE;
typedef int ELEMENT;
#endif  /* APPLICATION_H */

/* ---------- schema as built for the application --------- */
extern const char *dbfiles[];       /* file names           */
extern const char *denames[];       /* data element names   */
extern const char *elmask[];        /* data element masks   */
extern const char eltype[];         /* data element types   */
extern const int ellen[];           /* data element lengths */
extern const ELEMENT *file_ele[];   /* file data elements   */
extern const ELEMENT **index_ele[]; /* index data elements  */

/* --------- data base prototype --------------- */

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
void clrrcd(void *, const ELEMENT *);
int epos(ELEMENT, const ELEMENT *);
void rcd_fill(const void *, void *, const ELEMENT *,
              const ELEMENT *);

/* ------ functions used by Cdata utility programs ------ */
void build_index(char *, DBFILE);
int add_indexes(DBFILE, void *, RPTR);
DBFILE filename(char *);
void name_cvt(char *, char *);
int ellist(int, char **, ELEMENT *);
void clist(FILE *, const ELEMENT *,  const ELEMENT *,
            void *, const char *);
void test_eop(FILE *, const char *, const ELEMENT *);
void dblist(FILE *, DBFILE, int, const ELEMENT *);

/* ------------ dbms error codes for errors returns ----- */
enum dberrors {
    D_NF=1,   /* record not found */
    D_PRIOR,  /* no prior record for this request */
    D_EOF,    /* end of file */
    D_BOF,    /* beggining of file */
    D_DUPL,   /* primary key already exists */
    D_OM,     /* out of memory */
    D_INDXC,  /* index corrupted */
    D_IOERR   /* io error */
};

#endif /* CDATA_H */
