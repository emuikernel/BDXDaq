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
 
