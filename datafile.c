/* -- pg 142 ---- datafile.c ----------------------------- */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "cdata.h"
#include "datafile.h"

#define flocate(r,l) ((long)(sizeof(FHEADER)+(((r)-1)*(l))))

static FILE *fp[MXFILS];
FHEADER fh[MXFILS];

/* ---------- crete a file ------------------------- */
void file_create(char *name /* file name */, int len /* record lenght */)
{
  FILE *fp;         
  FHEADER hd;       

    remove(name);
    fp = fopen(name, "wb");
    hd.next_record = 1;
    hd.first_record = 0;
    hd.record_length = len;
    fwrite((char *) &h, sizeof hd, 1, fp);
    fclose(fp);
}

/* ---------- open a file ------------------------- */
int file_open(char *name /* filename */)
{
    int fno;

    for (fno = 0; fno < MXFILS; fno++)
        if (fp[fno] == NULL)
            break;
    if (fno == MXFILS)
        return ERROR;
    if ((fp[fno] = fopen(name, "rb+")) == NULL)
        return ERROR;
    fseek(fp[fno], 0L, SEEK_SET);
    fread((char*) &fh[fno], sizeof(FHEADER), 1, fp[fno]);
    return fno;
}

/* ---------- close a file ------------------------- */
void file_close(int fno /* logical file handle */)
{
    fseek(fp[fno], 0L, SEEK_SET);
    fwrite((char *) &fh[fno], sizeof(FHEADER), 1, fp[fno]);
    fclose(fp[fno]);
    fp[fno] = NULL;
}

/* ---------- create a new record -------------------- */
RPTR new_record(int fno /* logical file handle */, void *bf /* record buffer */)
{
    RPTR rcdno;
    FHEAER *c;

    if (fh[fno].first_record)   {
        rcdno = fh[fno].first_record;
        if ((c = malloc(fh[fno].record_length)) == NULL)    {
            errno = D_OM;
            dberror();
        }
        get_record(fno, rcdno, c);
        fh[fno].first_record = c->next_record;
        free(c);
    }
    else
        rcdno = fh[fno].next_record++;
    put_record(fno, rcdno, bf);
    return rcdno;
}

/* ---------- retrieve a record -------------------- */
int get_record(int fno /* logical file handle */, RPTR rcdno /* logical record number */, void *bf /* record buffer */)
{
    if (rcdno >= fh[fno].next_record)
        return ERROR;
    fseek(fp[fno],
          flocate(rcdno, fh[fno].record_length), SEEK_SET);
    fread(bf, fh[fno].record_length, 1, fp[fno]);
    return OK;
}

/* ---------- retrieve a record -------------------- */   
int put_record(int fno /* logical file handle */, RPTR rcdno /* logical record number */, void *bf /* record buffer */)
{
    if (rcdno >= fh[fno].next_record)
        return ERROR;
    fseek(fp[fno],
          flocate(rcdno, fh[fno].record_length), SEEK_SET);
    fwrite(bf, fh[fno].record_length, 1, fp[fno]);
    return OK;
}

int delete_record(int fno /* logical file handle */, RPTR rcdno /* logical record number */)
{
    FHEADR *bf;

    if (rcdno >= fh[fno].next_record)
        return ERROR;
    if ((bf = (FHEADER *)
         malloc(fh[fno].record_length)) == NULL)   {
        errno = D_OM;
        dberror();
    }
    memset(bf, '\0', fh[fno].record_length);
    bf->next_record = fh[fno].first_record;
    bf->first_record = -1;
    fh[fno].first_record = rcdno;
    put_record(fno, rcdno, bf);
    free(bf);
    return OK;
}
