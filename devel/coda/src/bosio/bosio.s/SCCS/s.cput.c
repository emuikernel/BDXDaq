h10912
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/cput.c
e
s 00211/00000/00000
d D 1.1 00/08/10 11:10:10 boiarino 1 0
c date and time created 00/08/10 11:10:10 by boiarino
e
u
U
f e 0
t
T
I 1

/* cput.c - write buffer to the local file */
 
#include <sys/types.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

#ifdef SunOS
#include <sys/asynch.h>
#endif

#include "bosio.h"
#include "ioincl.h"




/*
static int nevent = 0;
static struct timeval tv1, tv2;
static unsigned int time1, time2;
void
start_microtimer()
{
  nevent++;
  gettimeofday(&tv1,NULL);
  time1 = tv1.tv_sec*1000000 + tv1.tv_usec;
  return;
}
void
stop_microtimer(int cycle)
{
  gettimeofday(&tv2,NULL);
  time2 += (tv2.tv_sec*1000000 + tv2.tv_usec) - time1;
  if (nevent%(cycle) == 0)
  {
    printf(">time = %u microseconds\n",time2/nevent);
    nevent = 0;
    time2 = 0;
  }
  return;
}
*/







/* #define COND_DEBUG */
/* #define DEBUG   */
#include "dbgpr.h"

/* external int errno; */
 
int cput(int stream, int ircn, int ntot, int *buffer, int medium)
{
  size_t nbytes, nbytes_return;
  int error;
  
  nbytes = ntot * sizeof(int);

  if(ircn > 0)		/* direct access only */
  {
	long offset, current, request;
    int whence;
	  
	request = (ircn - 1) * nbytes;
    current = ftell(filepointer[stream]);
	  
	offset = request - current;
    whence = SEEK_CUR;
    if(current < 0 || offset < 0)
    {
      offset = request;
      whence = SEEK_SET;
    }
	  
	if (medium==2)
    {
      if( lseek((int)filepointer[stream], offset, whence) == -1 )
      {
		perror("cput"); 
	    error = errno;
	    errno = 0;
	    fprintf(stderr,"cput1: Cannot set record %d\n",ircn);
        fflush(stderr);
	    return(EBIO_SEEKERROR);
	  }
	}
    else
    {
      if(fseek(filepointer[stream], offset, whence) != 0)
      {
	    perror("cput");
	    error = errno;
	    clearerr(filepointer[stream]);
	    fprintf(stderr,"cput2: Cannot set record %d\n", ircn);
        fflush(stderr);
	    return(EBIO_SEEKERROR);
	  }
	}
  }

  /*printf("cput : WRITING to disk %i bytes\n",nbytes);*/
  if (medium==2)
  {
    static int buf[1000000], *buf1;
#ifdef SunOS
    aio_result_t resultp, *resultp1;
#endif
    static off_t offset = 0;
    static int whence = SEEK_END;

    /* COULD BE                           */
    /*static int whence = SEEK_SET;       */
    /* BUT AFTER aiowrite() AND aiowait() */
    /*offset += resultp.aio_return;       */
    /* HAVE TO BE SET !!!!!!!!!!!!!!!     */

	errno = 0;

/*
nbytes_return = nbytes;
*/




/*start_microtimer();*/

	if(nbytes != (nbytes_return = write((int)filepointer[stream], buffer, nbytes)))
    {
	  printf("cput: medium==2, ERROR WRITING to disk %i\n",errno); fflush(stdout);
	  printf("cput: stream = %d\n",stream); fflush(stdout);
	  printf("cput: file number = %d\n",(int)filepointer[stream]); fflush(stdout);
	  printf("cput: nbytes = %i nbytes_return = %i\n",nbytes,nbytes_return); fflush(stdout);
	  perror("cput");
	  error = errno;
	  errno = 0;
	  printf("cput: Write error %d\n",error);
	  return(EBIO_WRITEERROR);
	}

/*stop_microtimer(1000);*/








/*
start_microtimer();
resultp1 = aiowait(NULL);
stop_microtimer(1000);
*/

/*
printf("coping ...\n");
bcopy((char *)buffer, (char *)buf, nbytes);
printf("buffer=%08x buf=%08x\n",buffer,buf);
nbytes_return = aiowrite((int)filepointer[stream], (char *)buf, nbytes, offset, whence, &resultp);
*/

/*
buf1 = buffer;
printf("buffer=%08x buf1=%08x\n",buffer,buf1);
nbytes_return = aiowrite((int)filepointer[stream], (char *)buf1, nbytes, offset, whence, &resultp);
*/

/*
nbytes_return = aiowrite((int)filepointer[stream], (char *)buffer, nbytes, offset, whence, &resultp);
*/

/*
if(nbytes_return)
{
  printf("cput: error in aiowrite: nbytes_return=%d\n",nbytes_return);
  printf("  aio_return=%d\n",resultp.aio_return);
  printf("  aio_errno=%d\n",resultp.aio_errno);
  exit(1);
}
*/





  }
  else
  {
	clearerr(filepointer[stream]);
	if(nbytes > fwrite(buffer, 1, nbytes, filepointer[stream]) || errno != 0) {
	  DPR1("cput : ERROR WRITING to disk %i\n",errno);
	  perror("cput");
	  error = errno;
	  clearerr(filepointer[stream]);
	  printf("cput: Write error.\n");
	  return(EBIO_WRITEERROR);
	}
  }
  
  return(0);
}

 
E 1
