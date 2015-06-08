h30328
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/crewnd.c
e
s 00077/00000/00000
d D 1.1 00/08/10 11:10:10 boiarino 1 0
c date and time created 00/08/10 11:10:10 by boiarino
e
u
U
f e 0
t
T
I 1
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
 
E 1
