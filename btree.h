/* ---------------- btree.h ------------------------------- */

#define MXTREES 20
#define NODE 51                  /* length of a B-tree node */
#define ADR sizeof(RPTR)

/* ---------------- btree prototype ----------------------- */
int btree_init(char *);
int btree_close(int);
void build_b(char *, int);
RPTR locate(int, char *);
int deletekey(int, char *, RPTR);
int insertkey(int, char *, RPTR, int);
RPTR nextkey(int);
RPTR prevkey(int);
RPTR firstkey(int);
RPTR lastkey(int);
void keyval(int, char *);
RPTR currkey(int);

/* ---------------- the btree node structure -------------- */
typedef struct treenode {
    int nonleaf;    /* 0 if leav, 1 if non-leaf */
    RPTR prntnode;  /* parent node */
    RPTR lfsib;     /* left sibling node */
    RPTR rtsib;     /* right sibling node */
    int keyct;      /* number of keys */
    RPTR key0;      /* node # of keys < 1st key this node   */
    char keyspace [NODE - ((sizeof(int) * 2) + (ADR * 4))];
    char spil [MXKEYLEN]; /* for insertion excess */
} BTREE;

/* ---- the structure of the btree header node ------------ */
typedef struct treehdr {
    RPTR rootnode;           /* root node number     */
    int keylength;           /* length of a key      */
    int m;                   /* max keys/node        */
    RPTR rlsed_node;         /* nexe released node   */
    RPTR endnode;            /* next unassigned node */
    int locked;              /* if btree is locked   */
    RPTR leftmost;           /* left-most node       */
    RPTR rightmost;          /* right-most node      */
} HEADER;
