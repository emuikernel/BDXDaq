/*DECK ID>, CREWND. */
 
#include <sys/types.h>
#include <unistd.h>
#include "ioincl.h"
 
/* rewind local file */
 
void crewnd(int lunptr, int medium)
{
#if defined(IRIX) || defined(ALLIANT) || defined(SunOS) || defined(HPUX)
  struct mtop top;
#endif
#ifdef AIX
  struct stop top;
#endif
#if defined(IRIX) || defined(ALLIANT) || defined(SunOS) || defined(HPUX) \
                  || defined(AIX) || defined(ULTRIX) || defined(ALPHA)
  FILE  *fp;
#endif
 
  if(medium == 1)	/* for tapes rewind=backspace */
  {
#if defined(IRIX) || defined(ALLIANT) || defined(SunOS) || defined(HPUX)
    top.mt_op = MTBSR;
    top.mt_count = 1;
    fp = filepointer[lunptr];
    if(ioctl(fileno(fp),MTIOCTOP,(void *)&top))
    {
      fprintf(stderr,"Cannot backspace tape \n");
      fflush(stderr);
    }
#endif
#ifdef AIX
    top.st_op = STRSR;
    top.st_count = 1;
    fp = filepointer[lunptr];
    if(ioctl(fileno(fp),STIOCTOP,(void *)&top))
    {
      fprintf(stderr,"Cannot backspace tape \n");
      fflush(stderr);
    }
#endif
#if defined(ULTRIX) || defined(ALPHA)
    if( fseek(filepointer[lunptr], 0L, SEEK_SET) )
    {
      fprintf(stderr,"Cannot backspace tape \n");
      fflush(stderr);
    }
#endif
#if defined(IRIX) || defined(ALLIANT) || defined(SunOS) || defined(HPUX) \
                  || defined(AIX) || defined(ULTRIX) || defined(ALPHA)
    fflush(filepointer[lunptr]);  /* necessary to discard the unread but
                                 already buffered data!!! */
#endif
#ifdef APOLLO
    printf(" Exabyte I/O not yet implemented \n");
    fflush(stdout);
#endif
  }
  else if (medium==2)
  {
    lseek((int)filepointer[lunptr], 0L, SEEK_SET);
    if(errno)
    {
      printf("crewnd: errno = %d\n",errno);
      errno = 0;
    }
  }
  else
  {
    rewind(filepointer[lunptr]);
  }
 
 return;
}
 
