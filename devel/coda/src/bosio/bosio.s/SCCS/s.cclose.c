h03714
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/cclose.c
e
s 00036/00000/00000
d D 1.1 00/08/10 11:10:08 boiarino 1 0
c date and time created 00/08/10 11:10:08 by boiarino
e
u
U
f e 0
t
T
I 1
/*DECK ID>, CCLOSE. */
 
#include <unistd.h>
#include "ioincl.h"
/* #define COND_DEBUG */
/* #define DEBUG  */
#include "dbgpr.h"
 
/* close local file */
 
void
cclose(int lunptr, int bufptr, int medium)
{
  int status;


  if(filepointer[lunptr])
  {
    if(medium==2) /* low-level IO */
    {
      DPR1("cclose : handler -> %x\n",filepointer[lunptr]);
      status = close((int)filepointer[lunptr]);
      if(status) printf("cclose: ERROR status = %d\n",status);
    }
    else
    {
      fclose(filepointer[lunptr]);
    }

    filepointer[lunptr] = (FILE *) NULL;
    if(bufptr) FREE((char *)bufptr);
  }

  return;
}
 
E 1
