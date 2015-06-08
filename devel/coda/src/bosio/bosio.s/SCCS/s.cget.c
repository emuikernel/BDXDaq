h46582
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/cget.c
e
s 00118/00000/00000
d D 1.1 00/08/10 11:10:08 boiarino 1 0
c date and time created 00/08/10 11:10:08 by boiarino
e
u
U
f e 0
t
T
I 1
/*DECK ID>, CGET. */
 
#include <sys/types.h>
#include <unistd.h>
#include <sys/uio.h>
#include <errno.h>
#include "bosio.h"
#include "ioincl.h"
 
/* read from local file */
/* external int errno; */

int cget(int *lunptr, int *ircn, int *medium, int *ntot, int *buffer)
{
  size_t nbytes;
  int err;

  nbytes = *ntot * sizeof(int);

  if(*ircn > 0)
  {
    long offset, current, request;
    int noff;
    int whence;
		
    request = (*ircn - 1) * nbytes;
    current = ftell( filepointer[*lunptr] );
		
    offset = request - current;
    whence = SEEK_CUR;
    if( current < 0 || offset < 0)
    {
      whence = SEEK_SET;
      offset = request;
    }
		
    if(*medium==2)
    {
      if( lseek((int)filepointer[*lunptr], offset, whence) == -1 )
      {
        err=errno;
        printf("cget1: errno %d\n",errno);
        perror("cget");
        clearerr(filepointer[*lunptr]);
        errno = 0;
        fprintf(stderr,"cget. Cannot set record %d\n",*ircn);
        return(err);
      }
    }
    else
    {
      if( fseek(filepointer[*lunptr], offset, whence) != 0 )
      {
        err=errno;
        printf("cget2: errno %d\n",errno);
        perror("cget");
        clearerr(filepointer[*lunptr]);
        fprintf(stderr,"cget. Cannot set record %d\n",*ircn);
        return(err);
      }
    }
  }

  if(*medium==2)
  {
    int nread,icur,iend,fd;
    off_t tmp;

    fd = (int)filepointer[*lunptr];
    errno = 0;

    if ( nbytes == (nread=read(fd, buffer, nbytes)) ) return(0);
    if(errno) printf("cget3: nbytes=%d(%u) nread=%d errno=%d\n",nbytes,nbytes,nread,errno);
    icur = lseek(fd,0L,SEEK_CUR); /* set pointer to current location plus 0L */
    if(icur == -1) printf("cget3: error in lseek 1 error = %d\n",errno);
    iend = lseek(fd,0L,SEEK_END); /* set pointer to size of file plus 0L */
    if(iend == -1) printf("cget3: error in lseek 2 error = %d\n",errno);
    tmp = lseek(fd,icur,SEEK_SET); /* set pointer to 'icur' bytes */
    if(tmp == -1) printf("cget3: error in lseek 3 error = %d\n",errno);
    if (icur == iend) return(EBIO_EOF);
    if (errno != 0)
    {
      /*
      printf("cget3: possible 'read' errors:\n");
      printf("   EAGAIN=%d EBADF=%d EBADMSG=%d EDEADLK=%d EFAULT=%d"
           " EINTR=%d EINVAL=%d EIO=%d EISDIR=%d ENOLCK=%d ENOLINK=%d ENXIO=%d EOVERFLOW=%d"
           " EFAULT=%d ESPIPE=%d\n",
           EAGAIN,EBADF,EBADMSG,EDEADLK,EFAULT,EINTR,EINVAL,EIO,EISDIR,ENOLCK,ENOLINK,ENXIO,
           EOVERFLOW,EFAULT,ESPIPE);
      */
      err=errno;
      printf("cget3: we have error %d\n",errno);
      perror("cget");
      errno = 0;
      return (err);
    }
    return EBIO_INTERNAL;
  }
  else
  {
    if( nbytes == fread(buffer, 1, nbytes, filepointer[*lunptr]) ) return(0);
    if( feof( filepointer[*lunptr] ) )
    {
      return(EBIO_EOF);
    }
		
    if( ferror( filepointer[*lunptr] ) )
    {
      err=errno;
      printf("cget4: errno %d\n",errno);
      perror("cget");
      clearerr( filepointer[*lunptr] );
      return(err);
    }
  }

}
 
E 1
