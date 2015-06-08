
/* LINK_support.c (used by UNIX programs only, no vxWorks !!!) */

#ifndef VXWORKS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>

#include "da.h"
#include "circbuf.h"
#include "bosio.h"
#include "etbosio.h"
#include "libdb.h"


#undef DEBUG

#define CODA_ERROR 1
#define CODA_OK 0

/*event builder complains:
-- Got all fragments of go
handle_link(): rocid=3: all buffers are full - wait ..
handle_link(): rocid=3: all buffers are full - wait ..
PUT: wait while icb=4 == cbp->read=4 -------
handle_link(): rocid=3: all buffers are full - wait ..
*/


/* external data */


extern char *mysql_host;

extern unsigned long *dataSent; /* see coda_component.c */

int deflt; /* 1 for CODA format, 0 for BOS format (see coda_eb_inc.c) */
unsigned int roc_linked; /* see deb_component.c */
CIRCBUF *roc_queues[MAX_ROCS]; /* see deb_component.c */
int      roc_queue_ix; /* see deb_component.c */
unsigned int *bufpool[MAX_ROCS][QSIZE]; /* see deb_component.c */
int roc_queue_evsize[MAX_ROCS]; /* see deb_component.c */

int nddl; /* see deb_component.c and BOS_format.c */
DDL ddl[NDDL]; /* see deb_component.c and BOS_format.c */

typedef struct thread_args *trArg;
typedef struct thread_args
{
  objClass object;
  DATA_LINK link;
} TRARGS;


static int ending_for_recv;

/********************************/
/* called from coda_component.c */
/*
int
LINK_support_Init(Tcl_Interp *interp)
{
  char tmp[400];

  sprintf(tmp,"%s {%s} %s {%s}",
	  __FILE__,
		  DAYTIME,
			CODA_USER,
	  "$Id: LINK_support.c,v 2.49 1999/10/28 14:49:57 abbottd Exp $");
  Tcl_SetVar(interp,
    "tcl_modules",tmp,TCL_LIST_ELEMENT|TCL_APPEND_VALUE|TCL_GLOBAL_ONLY);

  return(TCL_OK);
}
*/
/*****************************************************************************/

/* swap big buffer (includes BOS banks (2-word header format)) */
/* called by network thread on receiving buffer from ROC */

int
bufferSwap(unsigned int *cbuf, int nlongs)
{
  unsigned int lwd, t1, t2;
  int ii, jj, ix;
  int tlen, blen, dtype, typ, num;
  short shd;
  char cd;
  char *cp;
  short *sp;
  unsigned int *lp;


  ii = 0;

  /* swap buffer header: BBHEAD words */
  lp = (unsigned int *)&cbuf[ii];
  for(jj=0; jj<BBHEAD; jj++)
  {
	lwd = LSWAP(*lp);
	*lp++ = lwd;
  }
  ii += BBHEAD;

#ifdef DEBUG
  printf("\nbuffer header: length=%d words, buffer#=%d, rocid=%d, #events=%d, fd/magic=0x%08x, end=%d\n",
		 cbuf[BBIWORDS],cbuf[BBIBUFNUM],cbuf[BBIROCID],cbuf[BBIEVENTS],cbuf[BBIFD],cbuf[BBIEND]);
#endif

  /* swap CODA fragments and BOS banks inside every fragment */
  while(ii<nlongs)
  {
    lp = (unsigned int *)&cbuf[ii];

    lwd = LSWAP(*lp);    /* Swap the CODA fragment length */
    *lp++ = lwd;
    blen = lwd - 1;
	t1=lwd;
	
#ifdef DEBUG
	printf("CODA fragment: length = %d, ",blen+1);
#endif
	
    lwd = LSWAP(*lp);    /* Swap the CODA fragment header */
    *lp++ = lwd;
    num = lwd&0xff;
    dtype = (lwd>>8)&0x3f/*0xff*/;
    typ = (lwd>>16)&0xff;
	t2=lwd;
	
#ifdef DEBUG
	printf("2nd word(0x%08x): tag=%d, dtype=%d, num=%d\n",lwd,typ,dtype,num);
#endif
	
    ii += 2;

    if(blen == 0) continue; /* nothing to do with empty fragment */

    if(dtype != DT_BANK)
    {
	  switch(dtswap[dtype])
      {
        case 0:
		  /*
		  printf("case 0: no swap\n");
		  */		/*
		  printf("ii=%d nlongs=%d 0x%08x 0x%08x)\n",
            ii,nlongs,t1,t2);
		  {
            FILE *fd;
            int iii;
            fd = fopen("/home/boiarino/abc.txt","w");
            for(iii=0; iii<ii; iii++) fprintf(fd,"[%6d] 0x%08x\n",iii,cbuf[iii]);
            fclose(fd);
		  }
		  exit(0);
					*/
	      /* No swap */
	      ii += blen;
	    break;

        case 1:
		  /*
		  printf("case 1: short swap\n");
		  */
	      /* short swap */
	      sp = (short *)&cbuf[ii];
	      for(jj=0; jj<(blen<<1); jj++)
          {
	        shd = SSWAP(*sp);
	        *sp++ = shd;
	      }
	      ii += blen;
	      break;

        case 2:
		  /*
		  printf("case 2: int swap, deflt=%d\n",deflt);
		  */
          /* int swap */
          if(deflt || ((typ > 15) && (typ < 32)) || blen < 2) /* CODA fragment contains CODA data */
          {
            lp = (unsigned int *)&cbuf[ii];
            for(jj=0; jj<blen; jj++)
            {
              lwd = LSWAP(*lp);
              *lp++ = lwd;
            }
          }
          else /* CODA fragment contains BOS banks */
          {
            unsigned int bankid, banknum, ncol, nrow, lfmt, fmtlen, nwords, bb;

            lp = (unsigned int *)&cbuf[ii];

            bb = 0;
			
            /*printf("=== fragment contains BOS banks, blen=%d\n",blen);
            for(jj=0; jj<blen; jj++) printf("=== [%3d] 0x%08x\n",jj,LSWAP(lp[jj]));*/
			
            while(bb < blen)
            {
              /* swap BOS bank header */
              for(jj=0; jj<2; jj++)
              {
                lwd = LSWAP(*lp);
                *lp++ = lwd;
              }
              bb += 2;
			  if(bb >= blen)
              {
                printf("warn: early break: bb=%d, blen=%d\n",bb,blen);
                break;
              }
              bankid  = ((*(lp-2))>>16)&0xFFFF;
              banknum = (*(lp-2))&0xFFFF;
              nrow    = *(lp-1);
              ncol = ddl[bankid].ncol;
              lfmt = ddl[bankid].lfmt;

if(lfmt <= 0)
{
  printf("ERROR: lfmt=%d bankid=%d\n",lfmt,bankid);fflush(stdout);

		  {
            FILE *fd;
            int iii;
            fd = fopen("/home/clasrun/coda_eb/abc.txt","w");
            fprintf(fd,"Beginning of fragment: ii=%d, cbuf[ii]=0x%08x, bb=%d\n",ii,cbuf[ii],bb);
            fprintf(fd,"Whole buffer (nlongs=%d):\n",nlongs);
            for(iii=0; iii<nlongs; iii++) fprintf(fd,"[%6d] 0x%08x\n",iii,cbuf[iii]);
            fclose(fd);
		  }

  exit(0);
}

              fmtlen = 4 / lfmt;
              nwords = (ncol * nrow * fmtlen + 3) / 4;

              /* if BOS bank length inconsistent with CODA fragment length,
              print error message and try to recover */
              if((bb+nwords) > blen)
              {
                printf("ERROR: bb=%d nwords=%d -> (bb+nwords)=%d > blen=%d\n",
                  bb,nwords,(bb+nwords),blen);
                printf("ERROR: bankid=0x%04x (%d)\n",bankid,bankid);
                printf("ERROR: banknum=%d\n",banknum);
                printf("ERROR: nrow=%d ncol=%d lfmt=%d\n",nrow,ncol,lfmt);
                nwords = blen - bb;
                printf("Trying to recover: set nwords=%d, nrow=%d (%d %d)\n",
					   nwords,(nwords*lfmt)/ncol,lfmt,ncol);
                nrow = (nwords*lfmt)/ncol;
                *(lp-1) = nrow;
              }

              /*printf("=== bb=%d, bankid=%d, banknum=%d, nwords=%d, nrow=%d, ncol=%d, fmtlen=%d\n",
                bb, bankid, banknum, nwords, nrow, ncol, lfmt);*/
			  
              /* convert BOS data */
              if(lfmt == 1) /* long */
              {
                /*printf("=== swap %6d long words\n",nwords);*/
                for(jj=0; jj<nwords; jj++)
                {
                  lwd = LSWAP(*lp);
                  *lp++ = lwd;
                }
              }
              else if(lfmt == 2) /* short */
              {
                /*printf("=== swap %6d short words\n",nwords*2);*/
                sp = (short *)lp;
                for(jj=0; jj<(nwords*2); jj++)
                {
                  shd = SSWAP(*sp);
                  *sp++ = shd;
                }
                lp += nwords;
              }
              /* do nothing for char */

              bb += nwords;
              /*printf("=== bb = %d\n",bb);*/
            }

          }

	      ii += blen;
	      break;

        case 3:
		  /*
		  printf("case 3: double swap\n");
		  */
	      /* double swap */
	      lp = (unsigned int *)&cbuf[ii];
	      for(jj=0; jj<blen; jj++)
          {
	        lwd = LSWAP(*lp);
	        *lp++ = lwd;
	      }
	      ii += blen;
	      break;

        case 4:
		  /*
		  printf("case 4: composite swap\n");fflush(stdout);
		  */
		  lp = (unsigned int *)&cbuf[ii];
          swap_composite_t(lp, 1, NULL);
		  /*
          printf("case 4: composite swap done\n");fflush(stdout);
		  */
	      ii += blen;
		  break;

        case 5:
		  /*
		  printf("case 5: bank of banks swap - do nothing (header swapped already)\n");
		  */
		  break;

        default:
		  printf("default: no swap\n");
	      /* No swap */
	      ii += blen;
      }
    }
    else
    {
      printf("dtype=0x%08x\n",dtype);
    }
  }

  return(0);
}



#define NPROFMAX 10
#define NPROF1 10


/* called from 'handle_link'; reads one 'big' buffer from 'fd' */
/* returns number of bytes read or -1 if error (i.e. EOF) */

int
LINK_sized_read(int fd, char **buf, hrtime_t tprof[NPROFMAX])
{
  int size;	/* size of incoming packet */
  int cc, jj, llenw, *tmp;
  int rembytes;	/* remaining bytes */
  int n_retries = 0;
  int n_retry2;
  unsigned long netlong;	/* network byte ordered length */
  char *bufferp = 0;
  unsigned int lwd, magic;
  unsigned int *bigbuf;
  int n_ending;

static int nev;

/* timing */
hrtime_t start1, end1, start2, end2, start3, end3;



  /* Wait for all the data requested */
  int recv_flags = MSG_WAITALL;

  n_ending = 0;

  /* read header off socket */
  rembytes = sizeof(netlong);
  bufferp = (char *) &netlong;

start1 = start3 = gethrtime();

#ifdef FIXEDBUFS
  while(0)
#else
  while(rembytes)
#endif
  {


    {
      int nbytes, lbytes;

      nbytes = 65536;
      lbytes=4;
	  /*
      setsockopt(fd, SOL_SOCKET, SO_RCVBUF, 
                 (int *) &nbytes, lbytes); 
	  */
      getsockopt(fd, SOL_SOCKET, SO_RCVBUF, 
                 (int *) &nbytes, &lbytes); 
	  /*
      printf("socket buffer size is %d(0x%08x) bytes\n",nbytes,nbytes);
	  */
    }



/*printf("0: rembytes=%d [%d]\n",rembytes,fd);*/
    /*printf("processing 1 >%d<\n",fd);*/
    cc = recv(fd, bufferp, rembytes, /*MSG_DONTWAIT*/recv_flags);
    /*printf("processing 2 >%d<\n",fd);*/
/*printf("1: %d %d [%d]\n",rembytes,cc,fd);*/
    if(cc == -1)
    {
      if(errno == EWOULDBLOCK)
      {

        if(ending_for_recv)
        {
          n_ending ++;
          printf("fd=%d ending_for_recv=%d n_ending=%d - wait for ROC\n",
            fd,ending_for_recv,n_ending);
          if(n_ending >= 10)
		  {
            printf("fd=%d - ROC is not reporting -> force ending\n",fd);
            return(0);
		  }
        }

        /* retry */
		sleep(1); /* is 1 sec too big ??? */
        /* we do not know if ROC still alive and will send more data later,
        or ROC is dead ... */
		/*
        printf("LINK_sized_read(): recv would block, retrying ...");
        fflush(stdout);
		*/
      }
      else
      {
        printf("LINK_sized_read() ERROR1\n");
        fflush(stdout);
        if(errno != ECONNRESET)
        {
          perror("read ");
        }
        return(-1);
      }
    }
    else
    {
      /* It is OK for a socket to return with wrong number of bytes. */
      if(cc != rembytes)
      {
        if(cc > rembytes)
        {
          /* read() returned more bytes than requested!?!?!?! */
          /* this can't happen, but appears to anyway */
          printf("ERROR: LINK_sized_read(,,%d) = read(,,%d) = %d!?!?!\n",
                  size,rembytes,cc);
          printf("ERROR: recv() returned more chars than requested - exit.\n");
          fflush(stdout);
          exit(0);
        }
        else if(cc == 0) /* we are here if ROC closed connection ?! */
        {

          /* let other LINK threads know we are ending, in case if some ROC
          crashed and cannot send buffer with 'End' transition */
          ending_for_recv = 1;

          printf("LINK_sized_read(): fd=%d closed\n",fd);
          fflush(stdout);
          return(0);


        }
      }
#ifdef DEBUG
      printf("LINK_sized_read(): recv(,,%d) returned %d\n",rembytes,cc);
      fflush(stdout);
#endif
      /* Always adjust these to get out of the while. */
      bufferp += cc;
      rembytes -= cc;
    }

  } /* first 'recv' loop */


end1 = gethrtime();

#ifdef FIXEDBUFS
  size = TOTAL_RECEIVE_BUF_SIZE;
#else
  size = (int) ntohl(netlong); /* ntohl() return the argument value
                                  converted from network to host byte order */
#endif



  /* read data */
  if(size == 0)
  {
  	/* GHGHGH */
  	unsigned long *p;
  	printf("LINK_sized_read(): WARNING: zero length block from ROC\n");
    fflush(stdout);
#ifndef NOALLOC
  	*buf = (char *) calloc(24,1);
    if((*buf) == NULL)
    {
      printf("LINK_sized_read(): ERROR1: calloc(%d) returns zero !!!\n",size+6);
      fflush(stdout);
    }
#endif
    p = (unsigned long *) *buf;
    p[BBIWORDS]  = -1;
    p[BBIBUFNUM] = -1;
    p[BBIROCID]  = -1;
    p[BBIEVENTS] = -1; /* setting this to -1 makes handle_link ignore the buffer */
    p[BBHEAD]    = -1;
    p[BBHEAD+1]  = -1;

    return(24);
  }

#ifdef NOALLOC
  if(size > TOTAL_RECEIVE_BUF_SIZE)
  {
    printf("LINK_sized_read(): ERROR2: buffer size=%d too big - exit.\n",size+6);
    fflush(stdout);
    exit(0);
  }
#else
  /*printf("size=0x%08x\n",size);*/
  *buf = (char *) calloc(size+6,1); /* two bytes extra for null term strings */
  if((*buf) == NULL)
  {
    printf("LINK_sized_read(): ERROR2: calloc(%d) returns zero !!!\n",size+6);
    fflush(stdout);
  }
#endif

#ifdef DEBUG
  printf("LINK_sized_read(): have %d bytes buffer at 0x%08x\n",size+6,(int)(*buf));
  fflush(stdout);
#endif

  /* Sergey: CHANGE THIS IF BIGBUFS CHANGED IN roc_component.c */
  *((unsigned long *) *buf) = (size >> 2); /* put buffer size in 1st word */
  /*printf("12345: %d (%d 0x%08x)\n",*((unsigned long *) *buf),size,*buf);fflush(stdout);*/
  bufferp = *buf/* + sizeof(size)*/;           /* set pointer to 2nd word */



  rembytes = size;
  n_retry2 = 0;

start2 = gethrtime();

  while(rembytes)
  {

retry1:
/*
printf("fd=%d, bufferp=0x%08x, rembytes=0x%08x, recv_flags=%d\n",
fd, bufferp, rembytes, recv_flags);fflush(stdout);
*/


    cc = recv(fd, bufferp, rembytes, recv_flags);


/*printf("2: %d %d [%d]\n",rembytes,cc,fd);*/
/*
printf("cc=0x%08x\n",cc);fflush(stdout);
*/
    if(cc == -1)
    {
      if(errno == EWOULDBLOCK) goto retry1;
      if(errno != ECONNRESET) perror("read2");
      puts("Error 2.");
      printf("LINK_sized_read(): cc = %d, Errno is %d.\n", cc, errno);
      fflush(stdout);
      return(-1);
    }

    if(cc == 0)
    { /* EOF - process died */
      /* GHGHGH */
      printf("process died - return\n");
      fflush(stdout);
      return(0);
    }
    else if(cc > rembytes)
    {
      /* read() returned more bytes than requested!?!?!?! */
      /* this can't happen, but appears to anyway */

      printf("LINK_sized_read(): returned more bytes than requested!?!?!?!\n");
      printf("LINK_sized_read(,,%d) = read(,,%d) = %d!?!?!\n",
              size,rembytes,cc);
      printf("LINK_sized_read(): recv() returned more chars than requested - exit.\n");
      fflush(stdout);
      exit(0);
    }
    else if(cc != rembytes)
    {
      /* (cc > 0) && (cc < rembytes) */
      printf("LINK_sized_read(): cc=%d != rembytes=%d -> retry ...\n",cc,rembytes);
      fflush(stdout);
      n_retry2++;
    }
#ifdef DEBUG
    printf("LINK_sized_read(): recv(,,%d) returned %d\n",rembytes,cc);
    fflush(stdout);
#endif
    /* Always adjust these to get out of the while loop */
    bufferp += cc;
    rembytes -= cc;
  }







  /* we received buffer, lets swap it if necessary */
  bigbuf = (unsigned int *) *buf;
  /*
printf("RECV3: %d %d %d %d 0x%08x %d - 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x (%d)\n",
bigbuf[0],bigbuf[1],bigbuf[2],bigbuf[3],bigbuf[4],bigbuf[5],
bigbuf[6],bigbuf[7],bigbuf[8],bigbuf[9],bigbuf[10],bigbuf[11],size);
  */
  magic = bigbuf[BBIFD];
  if(magic == 0x01020304)
  {
#ifdef DEBUG
    printf("SWAP (0x%08x)\n",magic);
#endif
    llenw = size >> 2;
	/*
    if(llenw>200000)
    {
      printf("WARN: llenw=%d, size=%d\n",llenw,size);
      printf("RECV4: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x - 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x (%d)\n",
      bigbuf[0],bigbuf[1],bigbuf[2],bigbuf[3],bigbuf[4],bigbuf[5],
      bigbuf[6],bigbuf[7],bigbuf[8],bigbuf[9],bigbuf[10],bigbuf[11],size);
	}
	*/
    bufferSwap(bigbuf,llenw);
  }
#ifdef DEBUG
  else
  {
    printf("DO NOT SWAP (0x%08x)\n",magic);
  }
#endif







/* to simulate delay 
if(!(nev++%10)) {printf("sleep\n");sleep(1);}
*/

  tmp = (int *)(*buf);




end2 = end3 = gethrtime();
/*printf("67890: %d\n",tmp[BBIWORDS]);fflush(stdout);*/

tprof[3] += (end1-start1)/NANOMICRO;
tprof[4] += (end2-start2)/NANOMICRO;
tprof[5] += (end3-start3)/NANOMICRO;

tprof[1] += ((hrtime_t)tmp[BBIEVENTS]); /* the number of events */
tprof[2] += ((hrtime_t)tmp[BBIWORDS]); /* the number of bytes */
if(++tprof[0] == NPROF1)
{
/*
  printf("1: average buf: %4lld events, %6lld words,",
    tprof[1]/tprof[0],tprof[2]/tprof[0]);
  if(tprof[1] > 0)
  {
    printf("  recv1=%5lld recv2=%5lld tot=%5lld\n",
      tprof[3]/tprof[1],tprof[4]/tprof[1],tprof[5]/tprof[1]);
  }
  else
  {
    printf("\n");
  }
*/
  tprof[0] = 0;
  tprof[1] = 0;
  tprof[2] = 0;
  tprof[3] = 0;
  tprof[4] = 0;
  tprof[5] = 0;
}




/*
if(time3 > 3000000)
{
  printf("%7lld %7lld %7lld microsec (buf %d), rocid=%2d\n",
  time1,time2,time3,tmp[1],tmp[2]);
}
*/

  /* set appropriate bit letting building thread know we are ready */
roc_linked |= (1<<tmp[2]); /* tmp[2] contains rocid */

  /*
  printf("LINK_sized_read(): set roc_linked for rocid=%d (0x%08x)\n",
    tmp[2],roc_linked);
  fflush(stdout);
  */

  return(size);
}



/* main thread function */

void *
handle_link(trArg arg)
{
  DATA_LINK theLink = (DATA_LINK) arg->link;
  int fd;
  int numRead;
  int headerSize;
  char *errMsg;
  signed long *buf_long_p;
  int count, i, itmp;
  char *buf;
  char ipaddress[20];

  hrtime_t tprof[NPROFMAX];
  hrtime_t start1, end1, time1=NULL;
  hrtime_t start2, end2, time2=NULL;
  hrtime_t start4, end4, time4=NULL;
  hrtime_t nevtime1=NULL, nevchun=NULL, avgsize=NULL;

#ifdef NOALLOC
  unsigned int *bufptr[QSIZE];

  printf("cleanup pool of buffers for roc=%d (rocid=%d) ..\n",
    theLink->roc_queue->roc,theLink->roc_queue->rocid);
  fflush(stdout);
  for(i=0; i<QSIZE; i++)
  {
    bufptr[i] = bufpool[theLink->roc_queue->roc][i];
    bufptr[i][0] = 0; /* mark buffer as free */
  }
  printf(".. done.\n");
  fflush(stdout);
#endif


  /* accept socket connection (listen() must be called already) */

acceptagain:

{
  struct sockaddr_in from;
  int len;
  char *address;

  bzero((char *)&from, sizeof(from));
  len = sizeof (from);

printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
printf("Wait on 'accept(%d,0x%08x,%d)' ..\n",theLink->sock,&from,len);
printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");


	{
      FILE *fd;
      char fname[80];
      sprintf(fname,"/home/clasrun/ccscans/good_waiting_%s",theLink->name);
      fd = fopen(fname,"w");
	  if(fd > 0)
	  {
        chmod(fname,777);
        fprintf(fd,"waiting on accept\n");
        fclose(fd);
	  }
	}


fflush(stdout);

  /* NOTE: the original socket theLink->sock remains open for accepting further connections */
  while((theLink->fd = accept(theLink->sock, (struct sockaddr *)&from, &len)) == -1)
  {
    printf("accept: wait for connection\n");
    sleep(1);
  }



  /* sergey: accept only from specified address (trying to block CC scans and other junk) */
  address = inet_ntoa(from.sin_addr);
  printf("accept() returns Ok, we expected it from >%s<, received from>%s<\n",theLink->name,address);
  if(dacgethostbyname(theLink->name, ipaddress) != 0)
  {
    printf("ERROR in dacgethostbyname() !!!!!\n");
  }
  else if(strcmp(address,ipaddress))
  {
    printf("ERRORRRRRRRRRRRRRRRRRRRRRR: UNAUTORIZED ACCESS FROM >%s<, goto accept() again\n",address);
    printf("ERRORRRRRRRRRRRRRRRRRRRRRR: UNAUTORIZED ACCESS FROM >%s<, goto accept() again\n",address);
    printf("ERRORRRRRRRRRRRRRRRRRRRRRR: UNAUTORIZED ACCESS FROM >%s<, goto accept() again\n",address);
    fflush(stdout);

	{
      FILE *fd;
      char fname[80];
      sprintf(fname,"/home/clasrun/ccscans/bad_%s",theLink->name);
      fd = fopen(fname,"w");
	  if(fd > 0)
	  {
        chmod(fname,777);
        fprintf(fd,"UNAUTORIZED ACCESS FROM >%s<\n",address);
        fclose(fd);
	  }
	}

    close(theLink->fd);
    goto acceptagain;
  }

  {
    FILE *fd;
    char fname[80];
    sprintf(fname,"/home/clasrun/ccscans/good_%s",theLink->name);
    fd = fopen(fname,"w");
	if(fd > 0)
    {
      chmod(fname,0777);
      fprintf(fd,"accepted link from >%s<\n",address);
      fclose(fd);
    }
  }

  fd = theLink->fd;
  printf("connection accepted, fd=%d\n",fd);
}

  /* Sergey ??? */
  printf("call cfree ...\n"); fflush(stdout);
  cfree(arg);
  printf("... cfree done.\n"); fflush(stdout);

  /* main loop */
  while(1)
  {


start1 = gethrtime();






/* MAY NEED IT TO CHECK IF CONNECTION STILL ALIVE - CHECK IT LATER !!! */

	/****************************************************************/
	/****************************************************************/
    /*** ROCs do not need that - just for backward compartibility ***/
/* NOTE: even if ROC does not recv() that message, numRead still =128 ! * 
    /* sending event sizes to rocs */
    /* puts("Sending fragment info to rocs."); */
/*
    numRead = send(fd, (const void *)roc_queue_evsize, 128, 0);
printf("numRead=%d\n",numRead);
    if(numRead <= 0)
    {
      printf("handle_link(): send() returns %d\n",numRead);
      fflush(stdout);
      put_cb_data(&theLink->roc_queue, (void *) -1);
      break;
    }
*/
	/****************************************************************/
	/****************************************************************/

/* do not forget to uncomment it on other end !!!
    numRead = send(fd, (const void *)roc_queue_evsize, 128, 0);
    if(numRead <= 0)
    {
      printf("handle_link(): send() returns %d\n",numRead);
      fflush(stdout);
      put_cb_data(&theLink->roc_queue, (void *) -1);
      break;
    }
*/









#ifdef NOALLOC
    /* get free buffer from pool; will wait if nothing is available */
tryagain1:
    buf = NULL;
#ifdef DO_NOT_PUT
    bufptr[0][0] = 0;
#endif
    for(i=0; i<QSIZE; i++)
    {
      if(bufptr[i][0] == 0)
      {
        buf = (char *) bufptr[i];
#ifdef DEBUG
        printf("handle_link(): rocid=%d: got free buffer %d at 0x%08x\n",theLink->roc_queue->rocid,i,buf);
        fflush(stdout);
#endif
        break;
      }
    }
    if(buf == NULL)
    {
      printf("handle_link(): rocid=%d: all buffers are full - wait ..\n",
             theLink->roc_queue->rocid);
      fflush(stdout);
      sleep(1);
      goto tryagain1;
    }
#endif


    /* reading data from roc */
start4 = gethrtime();
/*printf("901 %d\n",fd);fflush(stdout);*/
    numRead = LINK_sized_read(fd,&buf,tprof);
/*printf("902 %d\n",fd);fflush(stdout);*/
end4 = gethrtime();

#ifdef DEBUG
    printf("handle_link(): got %d bytes\n",numRead); fflush(stdout);
    fflush(stdout);
#endif
    if(numRead <= 0)
    {
      printf("handle_link(): LINK_sized_read() returns %d\n",numRead);
      fflush(stdout);
      put_cb_data(&theLink->roc_queue, (void *) -1);
      break;
    }

    /* count total amount of data words */
    *dataSent += (numRead>>2);

    buf_long_p = (signed long *) buf;
#ifdef DEBUG
    printf("buffer from >%s< (%08x %08x %08x %08x %08x %08x %08x %08x)\n",
           theLink->name,
           buf_long_p[0],buf_long_p[1],buf_long_p[2],buf_long_p[3],
           buf_long_p[4],buf_long_p[5],buf_long_p[6],buf_long_p[7]);
#endif
    if(buf_long_p[BBIWORDS] == BBHEAD)
    {
      printf("handle_link(): WARNING got empty buffer from ROC !\n");
      fflush(stdout);
#ifndef NOALLOC
      free(buf);
#endif
      continue;
    }

    /* Check for test_link data */
    if(buf_long_p[BBIEVENTS] < 0)
    {
      printf("WARNING - handle_link discarding buffer. count = %d.\n",
        buf_long_p[BBIEVENTS]);
#ifndef NOALLOC
      free(buf);
#endif
      continue;
    }

#define NSLEP (QSIZE/2)

/* sleep here if our fifo 'almost' full; that suppose
to give CPU to another thread who probably need it more */
/*
if(theLink->roc_queue->num_full > NSLEP)
{
  printf("rocid=%2d: sleep %2d seconds\n",
  theLink->roc_queue->rocid,theLink->roc_queue->num_full-NSLEP);
  sleep(theLink->roc_queue->num_full-NSLEP);
}
*/

    /* put buffer to the circular buffer manager */
start2 = gethrtime();

#ifndef DO_NOT_PUT

#ifdef DEBUG
{
unsigned int *bigbuf;
bigbuf = (unsigned int *) buf;
printf("PUTV3: %d %d %d %d %d %d - 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
bigbuf[0],bigbuf[1],bigbuf[2],bigbuf[3],bigbuf[4],bigbuf[5],
bigbuf[6],bigbuf[7],bigbuf[8],bigbuf[9],bigbuf[10],bigbuf[11]);
}
#endif
    if(put_cb_data(&theLink->roc_queue, (void *) buf) < 0)
    {
      printf("handle_link(): put_cb_data returns < 0 - break.\n");
      fflush(stdout);
      break;
    }
#endif


end2 = gethrtime();
end1 = gethrtime();
time1 += ((end1-start1)/NANOMICRO);
time2 += ((end2-start2)/NANOMICRO);
time4 += ((end4-start4)/NANOMICRO);
nevchun += ((hrtime_t)buf_long_p[3]); /* the number of events */
avgsize += ((hrtime_t)buf_long_p[0]);
/*
printf("--- %ld %ld %ld %ld\n",buf_long_p[0],buf_long_p[1],buf_long_p[2],
buf_long_p[3]);
*/
if(++nevtime1 == NPROF1)
{
/*
  printf("2: average buf: %4lld events, %6lld words,",
    nevchun/nevtime1,avgsize/nevtime1);
  if(nevchun > 0)
  {
    printf("  tot=%5lld recv=%5lld put=%5lld\n",
      time1/nevchun,time4/nevchun,time2/nevchun);
  }
  else
  {
    printf("\n");
  }
*/
  nevtime1 = 0;
  time1 = 0;
  time2 = 0;
  time4 = 0;
  nevchun = 0;
  avgsize = 0;
}


  } /* end of main loop while(1) */








/*PIECE MOVED TO debCloseLink*/







/*printf("handle_link(): thread exit for %9.9s\n",theLink->name); fflush(stdout);*/
/*NOTE: segm fault printing 'theLink->name', probably pointer is not good any more ???*/
  printf("handle_link(): thread exit\n"); fflush(stdout);


printf("907\n"); fflush(stdout);
  pthread_exit(0);
}


  /*               croctest1    EB5        clon10-daq1
DATA_LINK           argv[1]   argv[2]       argv[3]
debOpenLink(char *fromname, char *toname, char *tohost)
  */

DATA_LINK
debOpenLink(char *fromname, char *toname, char *tohost,  MYSQL *dbsock)
{
  DATA_LINK theLink;
  int i, len, itmp, res, numRows;
  char host[100], hostmp[100], type[100], state[100], chport[100];
  char inhost[100];
  char name[100];
  char tmp[256], tmpp[256], *ch;
  int port = 0;

  MYSQL_RES *result;

  struct hostent *hp, *gethostbyname();
  struct sockaddr_in sin, from;
  int s, slen;

  static trArg args;

printf("++++++1+ 0x%08x 0x%08x 0x%08x\n",roc_queues[0],roc_queues[1],roc_queues[2]);

  /* set 'theLink' */
  theLink = (DATA_LINK) calloc(sizeof(DATA_LINK_S),1);
  bzero((char *) theLink, sizeof(DATA_LINK_S));

  theLink->name = (char *) calloc(strlen(fromname)+1,1);
  strcpy(theLink->name, fromname); /* croctest1 etc */





  /* ROCs will use DB host name to send data to */
  /* 'inhost' will be set to 'links' table to let ROCs know where to send data */
  strncpy(inhost,tohost,99);
  printf("debOpenLink: set inhost to >%s<\n",inhost);
  strncpy(host,inhost,99);
  strncpy(theLink->host,host,99);


  /* construct database table row name */
  strncpy(name,fromname,98);
  len = strlen(name);
  strcpy((char *)&name[len],"->");
  strncpy((char *)&name[len+2],toname,(100-(len+2)));
  theLink->linkname = strdup(name); /* croctest1->EB5 etc */
  printf("debOpenLink: theLink->linkname is >%s<\n",theLink->linkname);

  /* set connection type to TCP */
  strcpy(type,"TCP");




printf("++++++2+ 0x%08x 0x%08x 0x%08x\n",roc_queues[0],roc_queues[1],roc_queues[2]);
  /* ... */

  /* */
  bzero((char *)&sin, sizeof(sin));
  hp = gethostbyname(host);
  if(hp == 0 && (sin.sin_addr.s_addr = inet_addr(host)) == -1)
  {
	printf("debOpenLink: unkown host >%s<\n",host);
	return(1);
  }
  if(hp != 0) bcopy(hp->h_addr, &sin.sin_addr, hp->h_length);
  sin.sin_port = htons(port);
  sin.sin_family = AF_INET;

  /* create a socket */
  s = socket(AF_INET, SOCK_STREAM, 0); /* tcl: PF_INET !!?? */
  if(s < 0)
  {
    printf("debOpenLink: cannot open socket\n");
    return(1);
  }
  else
  {
    theLink->sock = s;
    printf("debOpenLink: listening socket # %d\n",theLink->sock);
  }

  /* set socket options */
  /* was in dp
  if(reuseAddr)
  {
	int one = 1;
	res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
			    (char *)&one, sizeof(int));
  }
  {
    int cval = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&cval, 4);
  }
  */



  slen = 65535;
  if(setsockopt (s, SOL_SOCKET, SO_SNDBUF, &slen, sizeof(slen)) < 0)
  {
	printf("debOpenLink: setsockopt SO_SNDBUF failed\n");
	return(1);
  }

  {
    int nbytes, lbytes;

    nbytes = 0;
    lbytes = 4;

    getsockopt(s, SOL_SOCKET, SO_SNDBUF, (int *) &nbytes, &lbytes); 
    printf("debOpenLink: socket buffer size is %d(0x%08x) bytes\n",
      nbytes,nbytes);
  }
printf("++++++3+ 0x%08x 0x%08x 0x%08x\n",roc_queues[0],roc_queues[1],roc_queues[2]);

  /*
  optval = 0;
  if (setsockopt (s, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof (optval)) < 0)
  {
	printf("setsockopt TCP_NODELAY failed\n");
	free(buffer);
	exit(1);
  }
  */

  /*
  if (setsockopt (s, SOL_SOCKET, SO_SND_COPYAVOID, &on, sizeof (on)) < 0)
  {
	printf ("setsockopt SO_RCVBUF failed\n");
	free (buffer);
	exit (1);
  }
  */
  /*
    dp_socketOption $server noblock no
    dp_socketOption $server autoClose yes
    dp_socketOption $server keepAlive yes
	# Explicitly set nodelay off for data link (VxWorks slows down)
    #bugbug		dp_socketOption $server nodelay 1
*/

  /* connect for client only (ROC)
  if(connect(s, (const struct sockaddr *)&sin, sizeof (sin)) < 0)
  {
    printf("debOpenLink: connect failed: host %s port %d\n",
      inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
    close(s);
	return(1);
  }
  */

  /* bind and listen for server only (EB) */
  if(bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    printf("debOpenLink: bind failed: host %s port %d\n",
      inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
    close(s);
	return(1);
  }

  if(listen(s, 5) < 0)
  {
    printf("debOpenLink: listen failed\n");
    close(s);
	return(1);
  }


  /* get the port number */
  len = sizeof(sin);
  if(getsockname (s, (struct sockaddr *) &sin, &len) < 0)
  {
    printf("debOpenLink: getsockname failed\n");
    close(s);
	return(1);
  }

  port = ntohs(sin.sin_port);
  printf("debOpenLink: socket is listening: host %s port %d\n",
      inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));

printf("++++++4+ 0x%08x 0x%08x 0x%08x\n",roc_queues[0],roc_queues[1],roc_queues[2]);




  /* create 'links' table if it does not exist (database must be opened in calling function) */
  sprintf(tmpp,"SELECT * FROM links");
  if(mysql_query(dbsock, tmpp) != 0)
  {
    /*need to check it !!!*/
    printf("No 'links' table -> we will create it (%s)\n",mysql_error(dbsock));
    sprintf(tmp,"create table links (name char(100) not null, type char(4) not null,host char(30),state char(10),port int)");
    if(mysql_query(dbsock, tmp) != 0)
    {
	  printf("ERROR: cannot create table 'links' (%s)\n",mysql_error(dbsock));
      return(CODA_ERROR);
    }
    else
    {
      printf("table 'links' created\n");
    }
  }
  else
  {
    MYSQL_RES *res;
    printf("Table 'links' exist\n");

    /*store and free results, otherwise mysql gives error on following mysql_query ..*/
    if(!(res = mysql_store_result (dbsock) ))
    {
      printf("ERROR in mysql_store_result (%s)\n",mysql_error(dbsock));
      return(CODA_ERROR);
    }
    else
    {
      mysql_free_result(res);
    }
  }


  /* trying to select our link from 'links' table */
  sprintf(tmp,"SELECT name FROM links WHERE name='%s'",name);
  if(mysql_query(dbsock, tmp) != 0)
  {
	printf("debOpenLink: mysql error (%s)\n",mysql_error(dbsock));
    return(CODA_ERROR);
  }

  /* gets results from previous query */
  /* we assume that numRows=0 if our link does not exist,
     or numRows=1 if it does exist */
  if( !(result = mysql_store_result(dbsock)) )
  {
    printf("ERROR in mysql_store_result (%)\n",mysql_error(dbsock));
    return(CODA_ERROR);
  }
  else
  {
    numRows = mysql_num_rows(result);
    mysql_free_result(result);

    printf("nrow=%d\n",numRows);
    /* insert/update with state='down' */
    if(numRows == 0)
    {
      sprintf(tmp,"INSERT INTO links (name,type,host,port,state) VALUES ('%s','%s','%s',%d,'down')",name,type,host,port);
      printf("!!=1=> >%s<\n",tmp);
    }
    else if(numRows == 1)
    {
      sprintf(tmp,"UPDATE links SET host='%s',type='%s', port=%d, state='down' WHERE name='%s'",host,type,port,name);
      printf("!!=2=> >%s<\n",tmp);
    }
    else
    {
      printf("debOpenLink: ERROR: unknown nrow=%d",numRows);
      return(CODA_ERROR);
    }

    if(mysql_query(dbsock, tmp) != 0)
    {
	  printf("debOpenLink: ERROR 20-2\n");
      return(CODA_ERROR);
    }
    else
    {
      printf("Query >%s< succeeded\n",tmp);
    }
  }

  /* set state 'waiting' (MAYBE THIS MUST BE LAST ACTION, AFTER SETTING port etc ???!!!) */
  sprintf(tmp,"UPDATE links SET state='waiting' WHERE name='%s'",name);
  if(mysql_query(dbsock, tmp) != 0)
  {
	printf("debOpenLink: ERROR 22-2\n");
    return(CODA_ERROR);
  }

  /* database must be closed in calling function */

  /* ================ end of database update =================== */








printf("++++++5+ 0x%08x 0x%08x 0x%08x\n",roc_queues[0],roc_queues[1],roc_queues[2]);


/* cleanup .. */
ending_for_recv = 0;


  /* allocate and fill thread parameter structure */
 args = (trArg) calloc(sizeof(TRARGS),1);
  args->link = theLink;
  theLink->roc_queue = roc_queues[roc_queue_ix++];
  /*theLink->roc_queue->parent = theLink->name;donotneedit???*/
  printf("theLink->name=%s\n",theLink->name);
  printf("LINK_thread_init(): creating thread ..\n"); fflush(stdout);

printf("++++++6+ 0x%08x 0x%08x 0x%08x\n",roc_queues[0],roc_queues[1],roc_queues[2]);


 {
   /*Sergey: better be detached !!??*/
    pthread_attr_t detached_attr;

    pthread_attr_init(&detached_attr);
    pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setscope(&detached_attr, PTHREAD_SCOPE_SYSTEM);


    /* start thread */
    if(pthread_create( &theLink->thread, /*NULL*/&detached_attr,
                 (void *(*)(void *)) handle_link, (void *) args) != 0)
    {
      printf("LINK_thread_init(): ERROR in thread creating\n"); fflush(stdout);
      perror("pthread_create: ");
      return(CODA_ERROR);
    }
    printf("LINK_thread_init(): thread is created\n"); fflush(stdout);
 }

  return(theLink);
}


int
debCloseLink(DATA_LINK theLink, MYSQL *dbsock)
{
  void *status;
  char tmp[1000];

  /* */
  if(theLink == NULL)
  {
    printf("debCloseLink: theLink=NULL -> return\n");
    return(CODA_OK);
  }
  else
  {
    printf("debCloseLink: theLink=0x%08x -> closing\n",theLink);









/* PIECE MOVED FROM handle_link() */

/* SERGEY: SHOULD IT BE IN DESTRUCTOR ??? */

/* SERGEY: FOLLOWING PIECE DOES NOT HANDLE BROKEN CONNECTIONS PROPERLY, 
   FOR EXAMPLE IF coda_net DELETED IN ROC BEFORE 'GO', NEED TO FIX !!!!!!!!*/

  /*
   *
   * If we're in non-blocking mode, and this would block, return.
   * If the connection is closed (numRead == 0), don't return an
   * error message.  Otherwise, return one.
   *
   * In either case, we close the file, delete the file handler, and
   * return a null string.
   */
  theLink->thread = 0;

  printf("debCloseLink reached, fd=%d sock=%d\n",theLink->fd,theLink->sock);
  fflush(stdout);
  /* shutdown socket connection */
/* sergey: seg fault was happening, noticed that fd was big value, did check
   for <1000, need to investigate ..*/
  if(theLink->fd != 0 || theLink->fd < 1000)
  {
	printf("11: shutdown fd=%d\n",theLink->fd);fflush(stdout);
    if(shutdown(theLink->fd, SHUT_RDWR)==0) /*SHUT_RD,SHUT_WR,SHUT_RDWR*/
    {
	  printf("12\n");fflush(stdout);
      printf("debCloseLink: socket fd=%d sock=%d connection closed\n",
        theLink->fd,theLink->sock);

printf("903\n"); fflush(stdout);
printf("903-1 %d\n",theLink->fd); fflush(stdout);
close(theLink->fd);/*??????????hungs here!!!!!!!!!!*/
printf("904\n"); fflush(stdout);
printf("904-1 %d\n",theLink->fd); fflush(stdout);
close(theLink->sock);/*??????????hungs here!!!!!!!!!!*/
printf("905\n"); fflush(stdout);
printf("905-1 %d\n",theLink->fd); fflush(stdout);

    }
    else
    {
	  printf("13\n");fflush(stdout);
      printf("debCloseLink: ERROR in socket fd=%d sock=%d connection closing\n",
        theLink->fd,theLink->sock);
    }
  }

printf("906\n"); fflush(stdout);











  }






  /* !!!!!?????
    if { "$direction" == "in" } {
	database query "DELETE FROM links WHERE name='$name'"
    }
  */



  /* shut down a connection by telling the other end to shutdown (database must be opened in calling function) */
  /* set state 'down' */
  sprintf(tmp,"UPDATE links SET state='down' WHERE name='%s'",theLink->linkname);
  if(mysql_query(dbsock, tmp) != 0)
  {
    printf("debCloseLink: ERROR in database query {UPDATE ..}\n");
    return(CODA_ERROR);
  }
  else
  {
    printf("debCloseLink: link is down\n");
  }

  /* database must be closed in calling function */

  /* ================ end of database update =================== */



  /* cancel thread if exist */
  if(theLink->thread)
  {
    pthread_t thread = theLink->thread;
    printf("debCloseLink: canceling thread .\n");
    pthread_cancel(thread);
    printf("debCloseLink: canceling thread ..\n");
    /*pthread_join(thread,&status); stuck here if one of the ROCs crashed */
    printf("debCloseLink: canceling thread !\n");
    theLink->thread = 0;
  }

  /* release memory */
  printf("debCloseLink: free memory\n");
  cfree((char *) theLink->name);
  /*cfree((char *) theLink->parent);donotneedit???*/
  cfree((char *) theLink);
  printf("debCloseLink: done.\n");
  
  return(CODA_OK);
}

#else /* ifndef VXWORKS */

/* just to resolve */

/*
#include "etbosio.h"
int nddl;
DDL ddl[NDDL];
*/

void
LINK_support_vxworks_dummy()
{
  return;
}

#endif











