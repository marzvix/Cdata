/* --------------- cdata.h ---------------------- */

#define AZTEC		1	/* Manx Aztec C86		*/
#define CI_C86		2	/* Computer Innovations C86	*/
#define DATALIGHT	3	/* Datalight C			*/
#define ECOC		4	/* Eco-C88			*/
#define LATTICE		5	/* Lattice C			*/
#define LETSC		6	/* Mark Williams Let's C	*/
#define MICROSOFT	7	/* Microsoft C			*/
#define TURBOC		8	/* Turbo C			*/
#define WIZARD		9	/* Wizard C			*/
#define GCC		10	/* GCC				*/

#define ERROR -1
#define OK 0

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#if COMPILER != LATTICE
#if COMPILER != LETSC
#if COMPILER != CI_C86
#include <errno.h>
#endif
#endif
#endif

#if COMPILER != LETSC
#if COMPILER != DATALIGHT
#if COMPILER != CI_C86
#include <fcntl.h>
#endif
#endif
#endif

#include <ctype.h>

extern int errno;

#if COMPILER == MICROSOFT
#include <sys\types.h>
#include <sys\stat.h>
#define OPENMODE O_RDWR+O_BINARY
#undef CMODE
#define CMODE S_IWRITE
#endif

#if COMPILER == TURBOC
#include <sys\stat.h>
#define OPENMODE O_RDWR+O_BINARY
#undef CMODE
#define CMODE S_IWRITE
#endif

#if COMPILER == CI_C86
#define void int
#define atol atoi
#undef CMODE
#define CMODE BUPDATE
#define OPENMODE BUPDATE
#endif

#if COMPILER == LETSC
#define CMODE 0
#define OPENMODE 2
#endif

#if COMPILER == DATALIGHT
#define CMODE 0
#define OPENMODE 2
#endif

#if COMPILER == WIZARD
#define CMODE 0x8080
#define OPENMODE O_RDWR+O_BINARY
#endif

#if COMPILER == LATTICE
#define OPENMODE O_RDWR
#define CMODE 0666
#endif

#if COMPILER == AZTEC
#define OPENMODE O_RDWR
#define CMODE 0666
#endif

#if COMPILER == ECOC
#define OPENMODE O_RDWR
#define CMODE 0
#endif

long lseek();

#define NODE 512	/* length of a B-tree node		*/
#define RPTR long	/* B-tree node and file address 	*/

#define MXFILS	 11	/* maximum files in a data base		*/
#define MXELE	100	/* maximum data elements in a file	*/
#define MXINDEX	  5	/* maximum indexes per file		*/

/* ----------- dbms error codes for errno return ------ */
#define D_NF	  1	/* record not found			*/
#define D_PRIOR	  2	/* no prior record for this request	*/
#define D_EOF	  3	/* end of file				*/
#define D_BOF	  4	/* beginning of file			*/
#define D_DUPL	  5	/* primary key already exists		*/
#define D_OM	  6	/* out of memory			*/
#define D_INDXC	  7	/* index corrupted			*/
#define D_IOERR	  8	/* i/o error				*/

#define MXKEYLEN 80	/* maximum key length for indexes */

#ifndef SCHEMA
/* --------- schema as built for the application --------- */
extern char *dbfiles [];		/* file names		*/
extern char *denames [];		/* data element names	*/
extern char *elmask  [];		/* data element masks	*/
extern char eltype [];			/* data element types	*/
extern int ellen [];			/* data element lengths	*/
extern int *file_ele [];		/* file data elements	*/
extern int **index_ele [];		/* index data elements	*/

void mov_mem(), set_mem(), fatal();
void cls_file();
void build_b();
void put_char(), clear_screen(), cursor();
void error_message(), clear_notice(), post_notice();

/* ------------- data base function definitions ---------- */
void db_open(),db_cls(),dberror(),init_rcd(),
	 clrrcd(),rcd_fill(),build_index();

/* ---------- screen driver function definitions --------- */
void init_screen(),protect(),edit(),
	 display_template(),tally(),put_field();
#endif

/* ---------- file header --------------- */
typedef struct fhdr	{
	RPTR first_record;
	RPTR next_record;
	int record_length;
} FHEADER;
