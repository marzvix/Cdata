/* -- pg 231 ---- sort.h ------------------------------------ */

#define MOSTMEM 50000U  /* most memory for sort buffer        */
#define LEASTMEM 10240U /* most memory for sort buffer        */
#define MAXSORTS 3      /* maximum fields in a sort sequence  */

int init_sort(
    const ELEMENT *,    /* element list of record to be sorted */
    const ELEMENT *,    /* element list of sort sequence       */
    int                 /* TRUE = ascending sort               */
);

void sort(void *);      /* sort records            */
void *sort_op(void);    /* retrieve sorted records */
