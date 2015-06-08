
/* roc_network.c - ROC networking service routines; can be run at the same
   CPU as coda_roc or on secondary CPU */

#undef ROC_DOES_NOT_SEND

#include "da.h"

#ifdef VXWORKS

#include <stdio.h>
#include <sockLib.h>
#include <errno.h>
#include <errnoLib.h>

/*needheaderfiles
#include <sys/socket.h>
#include <netinet/tcp.h>
*/

extern long     vxTicks;

#define MYCLOCK 25

#else

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <dlfcn.h>
#include <sys/mman.h>

#ifdef Linux
#include <linux/prctl.h>
#endif

#define MYCLOCK NANOMICRO

#endif

#include "circbuf.h"
#include "bigbuf.h"

#define CODA_ERROR 1
#define CODA_OK 0

/****************************************************************************/
/*************************** roc_network functions **************************/

/* shutdown socket connection */
int
LINK_close(int socket)
{
  int retsocket;

  if(socket != 0)
  {
    if(shutdown(socket, 2)==0)
    {
      printf("LINK_close: socket #%d connection closed\n",socket);
      close(socket);
      retsocket = 0;
    }
    else
    {
      printf("LINK_close: ERROR in socket #%d connection closing\n",
        socket);
      retsocket = socket;
    }
  }

  return(retsocket);
}


/* create a connection to given host and port; returns socket number
or 0 if connection was not established */
int
LINK_establish(char *host, int port)
{
  struct hostent *hp, *gethostbyname();
  struct sockaddr_in sin;
  int s, slen, ret;
  int socketnum = 0;

  /* */
  bzero((char *)&sin, sizeof(sin));

  /* for vxworks - in libtklite.a; should we move it somewhere ??? */
  hp = gethostbyname(host);
  if(hp == 0 && (sin.sin_addr.s_addr = inet_addr(host)) == -1)
  {
	printf("LINK_establish: unknown host >%s<\n",host);
	return(0);
  }
  if(hp != 0) bcopy(hp->h_addr, &sin.sin_addr, hp->h_length);
  sin.sin_port = htons(port);
  sin.sin_family = AF_INET;

  /* create a socket */
  s = socket(AF_INET, SOCK_STREAM, 0); /* tcl: PF_INET !!?? */
  if(s < 0)
  {
    printf("LINK_establish: cannot open socket\n");
    return(0);
  }
  else
  {
    socketnum = s;
    printf("LINK_establish: socket # %d\n",socketnum);
  }


  /**********************/
  /* set socket options */
  /**********************/

  /* socket buffer size */

  slen = 48000/*29200*/;
  if(setsockopt (s, SOL_SOCKET, SO_SNDBUF, &slen, sizeof(slen)) < 0)
  {
	printf("LINK_establish: setsockopt SO_SNDBUF failed\n");
	return(0);
  }

  {
    int nbytes, lbytes;

    nbytes = 0;
    lbytes = 4;

    getsockopt(s, SOL_SOCKET, SO_SNDBUF, (int *) &nbytes, &lbytes); 
    printf("LINK_establish: socket buffer size is %d(0x%08x) bytes\n",
      nbytes,nbytes);
  }

  {
    int optval, lbytes;

	optval = 1; /* 1-yes, 0-no */
    if(setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) < 0)
	{
	  printf("LINK_establish: setsockopt SO_KEEPALIVE failed\n");
	  return(0);
	}

    optval = 0;
    lbytes = 4;
    getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (int *) &optval, &lbytes);
    printf("LINK_establish: keepAlive is %d\n",optval);
  }

#ifdef VXWORKS_needheaderfiles
  {
    int optval = 0;
    if(setsockopt (s, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof (optval)) < 0)
    {
	  printf("LINK_establish: setsockopt TCP_NODELAY failed\n");
      return(0);
    }
  }
#endif

  /*
  if (setsockopt (s, SOL_SOCKET, SO_SND_COPYAVOID, &on, sizeof (on)) < 0)
  {
	printf ("LINK_establish: setsockopt SO_RCVBUF failed\n");
	return(0);
  }
  */
  /*
    dp_socketOption $server noblock no
    dp_socketOption $server autoClose yes
    dp_socketOption $server keepAlive yes
	# Explicitly set nodelay off for data link (VxWorks slows down)
    #bugbug		dp_socketOption $server nodelay 1
*/

  /* connect */
  if((ret=connect(s, (const struct sockaddr *)&sin, sizeof (sin))) < 0)
  {
    printf("\n\nERRORRRRRRRRRRRRRRRRRRRRRRRRRRRRRR !!!!!!!!!!!!!!\n");
    printf("ERRORRRRRRRRRRRRRRRRRRRRRRRRRRRRRR !!!!!!!!!!!!!!\n");
    printf("ERRORRRRRRRRRRRRRRRRRRRRRRRRRRRRRR !!!!!!!!!!!!!!\n");
    printf("LINK_establish: connect failed: host %s port %d, ret=%d\n",
      inet_ntoa(sin.sin_addr), ntohs(sin.sin_port), ret);
    printf("ERRORRRRRRRRRRRRRRRRRRRRRRRRRRRRRR !!!!!!!!!!!!!!\n");
    printf("ERRORRRRRRRRRRRRRRRRRRRRRRRRRRRRRR !!!!!!!!!!!!!!\n");
    printf("ERRORRRRRRRRRRRRRRRRRRRRRRRRRRRRRR !!!!!!!!!!!!!!\n\n\n");
	return(0);
  }

  printf("LINK_establish: socket %d is ready: host %s port %d\n",
      socketnum, inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));

  return(socketnum);
}


/* sized_write() from dpnetwork.c */
/* returns number of data bytes written or -1 if error */
int
LINK_sized_write(int fd, unsigned int *buffer, unsigned long nbytes)
{
  int cc;
  int rembytes;
  char *buffer2 = (char *) buffer;
  unsigned long netlong;	/* network byte ordered length */


/* timing */
#ifdef VXWORKS
unsigned long start, end, time1, time2;
#else
#ifndef Darwin
hrtime_t start, end, time1, time2;
int i, cycle = 10;
static int nev;
static hrtime_t sum;
#endif
#endif

  if(nbytes <= 0)
  {
    printf("WARN: LINK_sized_write called with nbytes=%d - return(0)\n",nbytes);
    return(0);
  }

  /* write header */
  netlong = htonl(nbytes);
  rembytes = nbytes;

#ifdef VXWORKS
start = sysTimeBaseLGet();
#else
start = gethrtime();
#endif

#ifndef FIXEDBUFS

retry2:

/*printf("1: fd=%d -> %d %d\n",fd,sizeof(netlong),cc);fflush(stdout);*/
  cc = write(fd,(char *)&netlong,sizeof(netlong));
/*printf("2: fd=%d -> %d %d\n",fd,sizeof(netlong),cc);fflush(stdout);*/
/*printf("INFO: LINK_sized_write() returns errno=%d (cc=%d, sizeof(nbytes)=%d(%u), netlong=%u)\n",
  errno,cc,sizeof(nbytes),nbytes,netlong);*/

  if(cc != sizeof(nbytes))
  {
    if(errno == EWOULDBLOCK)
    {
      printf("Operation would block 1: retry ...\n");
      goto retry2;
    }
	/*
    if(errno == ENOBUFS)
    {
      printf("No buffer space available 1: retry ...\n");
      goto retry2;
    }
	*/
    printf("ERROR1: LINK_sized_write() returns errno=%d (cc=%d, sizeof(nbytes)=%d(%u), netlong=%u)\n",
      errno,cc,sizeof(nbytes),nbytes,netlong);
    return(-1);
  }

/*printf("3: fd=%d -> %d %d\n",fd,sizeof(netlong),cc);fflush(stdout);*/

#endif

#ifdef VXWORKS
end = sysTimeBaseLGet();
time1 = (end-start)/MYCLOCK;
#else
end = gethrtime();
time1 = (end-start)/MYCLOCK;
#endif

  /* write data */
  if(nbytes == 0)
  {
    printf("WARN: LINK_sized_write: no data - return\n");
    return(0);
  }

#ifdef VXWORKS
start = sysTimeBaseLGet();
#else
start = gethrtime();
#endif

/*
printf("SEND3: %d %d %d %d 0x%08x %d - 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],
buffer[6],buffer[7],buffer[8],buffer[9],buffer[10],buffer[11]);
*/

  while(rembytes)
  {
retry3:

    cc = write(fd,buffer2,rembytes);
    /*cc = send(fd,buffer2,rembytes,NULL);*/
/*printf("2: %d %d\n",rembytes,cc);*/

    if(cc == -1)
    {
      if(errno == EWOULDBLOCK)
      {
        printf("Operation would block 2: retry ...\n");
        goto retry3;
      }
	  /*
      if(errno == ENOBUFS)
      {
        printf("No buffer space available 2: retry ...\n");
        goto retry3;
      }
	  */
      printf("ERROR2: LINK_sized_write() returns errno=%d (cc=%d, buffer2=%d rembytes=%d nbytes=%d)\n",
        errno,cc,buffer2,rembytes,nbytes);
      if(errno==ENOBUFS) printf("No buffer space available\n");
      else if(errno==EPIPE) printf("Broken pipe\n");
      else printf("Unknown error\n");
/*
netStackDataPoolShow - not enough buffers !!!
netStackSysPoolShow
*/
      return(-1);
    }

    buffer2 += cc;
    rembytes -= cc;
    if(rembytes!=0) printf("WARN: LINK_sized_write: rembytes=%d\n",rembytes);
  }


/*timing */
#ifdef VXWORKS
end = sysTimeBaseLGet();
time2 = (end-start)/MYCLOCK;
/*
printf("sized_write: %7lu  %7lu microsec (buf %d)\n",time1,time2,buffer[0]);
*/
#else
end = gethrtime();
time2 = (end-start)/MYCLOCK;
/*
printf("sized_write: %7llu  %7llu microsec (buf %d)\n",time1,time2,buffer[0]);
*/
#endif

  return(nbytes);
}





/****************************************************************************/
/******************************* net_thread *******************************/

#ifdef VXWORKS
void 
net_thread(BIGNET *bignetptrin, unsigned int offsetin)
{
#else
void 
net_thread(BIGNET *bignetptrin)
{
  unsigned int offsetin = 0;
#endif
  static int length, status, fd, ifend;
  int i, jj, llen, llenw, evsz, res, nevent;
  unsigned long lwd;
  unsigned int *bigbuf;
  static BIGNET *bignetptr;
  static unsigned int offset;

/* timing */
#ifdef VXWORKS
  unsigned long start, end, time1, time2, icycle, cycle = 20;
#else
#ifndef Darwin
  hrtime_t start, end, time1, time2, icycle, cycle = 20;
  static int nev;
  static hrtime_t sum;
#endif
#endif

#ifdef Linux
  prctl(PR_SET_NAME,"coda_net");
#endif

  /*
  printf("input: bignetptrin=0x%08x offsetin=0x%08x\n",
    bignetptrin,offsetin);
  */
  bignetptr = bignetptrin;
  offset = offsetin;

  printf("bignetptr=0x%08x offset=0x%08x\n",bignetptr,offset);
#ifdef VXWORKS
  taskDelay(100);
#else
  sleep(1);
#endif
  printf("bignetptr=0x%08x offset=0x%08x\n",bignetptr,offset);
#ifdef VXWORKS
  taskDelay(100);
#else
  sleep(1);
#endif
  printf("bignetptr=0x%08x offset=0x%08x\n",bignetptr,offset);
#ifdef VXWORKS
  taskDelay(100);
#else
  sleep(1);
#endif
  printf("bignetptr=0x%08x offset=0x%08x\n",bignetptr,offset);fflush(stdout);
#ifdef VXWORKS
  taskDelay(100);
#else
  sleep(1);
#endif
  printf("bignet at 0x%08x, bignet.gbigBuffer at 0x%08x -> 0x%08x\n",
          bignetptr, &(bignetptr->gbigBuffer),
          (&(bignetptr->gbigBuffer))+offset);fflush(stdout);
#ifdef VXWORKS
  taskDelay(100);
#else
  sleep(1);
#endif

  /*printf("net_thread reached\n");fflush(stdout);*/
  nevent = 0;
#ifndef Darwin
  icycle = time1 = time2 = 0;
#endif
  do
  {
#ifdef VXWORKS
    icycle ++;
    start = sysTimeBaseLGet();
#else
    icycle ++;
    start = gethrtime();
#endif

	/*
    printf("bb_read(0x%08x)\n",&(bignetptr->gbigBuffer));
	*/
#ifdef VXWORKS

#ifdef PMCOFFSET
    /* if we are on host board, or we are on pmc AND proc is
      on pmc as well - get buffer from local memory */
    if(offset == 0)
    {
      bigbuf = bb_read(&(bignetptr->gbigBuffer));
    }
    else /* we are on pmc board AND proc on host - get buffer over pci bus */
    {
      bigbuf = bb_read_pci(&(bignetptr->gbigBuffer));
    }
#else
    bigbuf = bb_read(&(bignetptr->gbigBuffer));
#endif

#else

    bigbuf = bb_read(&(bignetptr->gbigBuffer));

#endif

	/*
printf("coda_net: bb_read(0x%08x) return 0x%08x\n",bignetptr->gbigBuffer,bigbuf);fflush(stdout);
printf("coda_net: bb_read: %d %d %d %d %d %d\n",
  bigbuf[0],bigbuf[1],bigbuf[2],bigbuf[3],bigbuf[4],bigbuf[5]);
printf("coda_net: bb_read: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
  bigbuf[6],bigbuf[7],bigbuf[8],bigbuf[9],bigbuf[10],bigbuf[11]);
fflush(stdout);
	*/

    if(bigbuf == NULL)
    {
      printf("net_thread: ERROR: bigbuf==NULL\n");fflush(stdout);
      break;
    }

#ifdef VXWORKS
    end = sysTimeBaseLGet();
    time1 += (end-start)/MYCLOCK;
#else
    end = gethrtime();
    time1 += (end-start)/MYCLOCK;
#endif

    /*********************/
    /* Output to Network */
    /*********************/

#ifdef VXWORKS
    start = sysTimeBaseLGet();
#else
    start = gethrtime();
#endif

    bignetptr->failure = 0;


{
    int nbytes, lbytes;

    nbytes = 65536;
    lbytes=4;
	/*
    setsockopt(bigbuf[BBIFD], SOL_SOCKET, SO_SNDBUF, 
               (int *) &nbytes, lbytes); 
	*/
    getsockopt(bigbuf[BBIFD], SOL_SOCKET, SO_SNDBUF, 
               (int *) &nbytes, &lbytes); 
	/*
    printf("socket buffer size is %d(0x%08x) bytes\n",nbytes,nbytes);
	*/
}


	/*******************************************************************/
	/*******************************************************************/
    /* geting event sizes from EB - NOT IN USE, JUST FOR BACKWARD COMP 
    {
      int itmp, buf[128];
      itmp=recv(bigbuf[BBIFD], (void *)buf, 128, 0);
printf("itmp=%d\n",itmp);
      if(itmp <= 0 )
      {
        bignet.failure = 1;
        printf("ERROR: net_thread failed (in recv).\n");
        return;
      }
      if(!bigendian_out)
      {
        for(i=0; i<32; i++)
        {
          lwd = LSWAP(buf[i]);
          buf[i] = lwd;
        }
	  }
	}*/
	/*******************************************************************/
	/*******************************************************************/

    /* remember some values (do not need all of them ..) */
    nevent += bigbuf[BBIEVENTS];
    llenw = bigbuf[BBIWORDS];
    llen = bigbuf[BBIWORDS] << 2;
	/*printf("net_thread: llen=%d\n",llen);*/
    fd = bigbuf[BBIFD];
    ifend = bigbuf[BBIEND];

    /* set 'magic' word */
    bigbuf[BBIFD] = 0x04030201;

#ifdef ROC_DOES_NOT_SEND

    if(bigbuf[BBIBUFNUM] == -1) /* send special events only */
	{
#ifdef FIXEDBUFS
printf("LINK_sized_write 11\n");fflush(stdout);
      if(LINK_sized_write(fd, bigbuf, SEND_BUF_SIZE) < 0)
printf("LINK_sized_write 12\n");fflush(stdout);
#else
printf("LINK_sized_write 13\n");fflush(stdout);
      if(LINK_sized_write(fd, bigbuf, llen) < 0)
printf("LINK_sized_write 14\n");fflush(stdout);
#endif
      {
        bignetptr->failure = 1;
        printf("ERROR: net_thread failed (in LINK_sized_write).\n");
        return;
      }
	}
    else
	{
      printf("Output to 'none': dumping %d words\n",bigbuf[BBIWORDS]);
	}

#else /* ROC does send */

#ifdef VXWORKS
    /* check for free net buffers */
    usrNetStackSysPoolStatus("net_thread",0);
    usrNetStackDataPoolStatus("net_thread",0);
#endif
/*
usrNetStackSysPoolStatus(1);
usrNetStackDataPoolStatus(1);
*/
    /* send data */
#ifdef FIXEDBUFS
printf("LINK_sized_write 1\n");fflush(stdout);
    if(LINK_sized_write(fd, bigbuf, SEND_BUF_SIZE) < 0)
#else
	if(llen>=SEND_BUF_SIZE) printf("ERROR: llen=%d >= SEND_BUF_SIZE=%d\n",llen,SEND_BUF_SIZE);
    /*printf("bigbuf[BBIFD]=%d\n",bigbuf[BBIFD]);*/
/*printf("LINK_sized_write 2\n");fflush(stdout);*/
    if(LINK_sized_write(fd, bigbuf, llen) < 0)
#endif
    {
      bignetptr->failure = 1;
      printf("ERROR: net_thread failed (in LINK_sized_write).\n");
      return;
    }
/*printf("LINK_sized_write 3\n");fflush(stdout);*/
/*
usrNetStackSysPoolStatus(2);
usrNetStackDataPoolStatus(2);
*/

#endif /*ROC_DOES_NOT_SEND*/

    /*print_output_buffers_are_full = 1; reenable warning message */


/*timing */
#ifdef VXWORKS
    end = sysTimeBaseLGet();
    time2 += (end-start)/MYCLOCK;
#else
    end = gethrtime();
    time2 += (end-start)/MYCLOCK;
#endif

if(nevent != 0 && icycle >= cycle)
{
  /*
printf("net_thread: about to print\n");
printf("net_thread: about to print\n");
printf("net_thread: about to print\n");
printf("net_thread: about to print\n");
printf("net_thread: about to print\n");
printf("net_thread: about to print: %d %d\n",nevent,icycle);
printf("net_thread: about to print: %d %d\n",nevent,icycle);
printf("net_thread: about to print: %d %d\n",nevent,icycle);
  */
#ifdef VXWORKS
printf("net_thread:  waiting=%7lu    sending=%7lu microsec per event (nev=%d)\n",
time1/nevent,time2/nevent,nevent/icycle);
#else
printf("net_thread:  waiting=%7llu    sending=%7llu microsec per event (nev=%d)\n",
time1/nevent,time2/nevent,nevent/icycle);
#endif
nevent = icycle = time1 = time2 = 0;
}


    /* exit the loop if 'End' condition was received */
    if(ifend == 1)
    {
      printf("net_thread: END condition received\n");fflush(stdout);
      break;
    }

  } while(1);

printf("roc_network +++++++++++++++++++++++++++++++++++++ 1\n");fflush(stdout);


  /* force 'big' buffer read/write methods to exit */
#ifdef VXWORKS

#ifdef PMCOFFSET
    if(offset == 0) /* we are on host board */
    {
      bb_cleanup(&(bignetptr->gbigBuffer));
    }
    else            /* we are on pmc board */
    {
      bb_cleanup_pci(&(bignetptr->gbigBuffer));
    }
#else
    bb_cleanup(&(bignetptr->gbigBuffer));
#endif

#else

    bb_cleanup(&(bignetptr->gbigBuffer));

#endif



printf("roc_network +++++++++++++++++++++++++++++++++++++ 2\n");fflush(stdout);
  /* close links */
  bignetptr->socket = LINK_close(bignetptr->socket);

printf("roc_network +++++++++++++++++++++++++++++++++++++ 3\n");fflush(stdout);
  bignetptr->doclose = 1;

  printf("WRITE THREAD EXIT\n");fflush(stdout);
}




/*********************************************/
/* functions to handle TCP connections to EB */

#include "libdb.h"

/* fromname: for example 'dc1', toname for example 'EB1' */
/* returns: our host name, EB's port number, our socket number */
int
rocOpenLink(char *fromname, char *toname, char host_return[128], int *port_return, int *socketnum_return)
{
  int socketnum_local;
  int i, len, itmp, numRows, nwaits;
  MYSQL *dbsock;
  MYSQL_RES *result;
  MYSQL_ROW row;
  char hostmp[100], name[100], type[100], state[100], chport[100];
  char tmp[1000], tmpp[1000], *ch, host[128];
  int port = 0;

  printf("rocOpenLinks reached\n");

  /* get host name (actually target name) */
  if(gethostname(tmp,999) < 0)
  {
    printf("rocOpenLink: ERROR in gethostname\n");
    return(-1);
  }
  else
  {
    strncpy(host,tmp,30);
    printf("rocOpenLink: set host to >%s<\n",host);
  }

  /* construct database table row name */
  strncpy(name,fromname,98);
  len = strlen(name);
  strcpy((char *)&name[len],"->");
  strncpy((char *)&name[len+2],toname,(100-(len+2)));
  printf("rocOpenLink: set name to >%s<\n",name);

  /* connect to database */
  dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));
  printf("3121: dbsock=%d\n",dbsock);
  if(dbsock==NULL)
  {
    printf("rocOpenLink: cannot connect to the database 1 - exit\n");
    exit(0);
  }

  nwaits = 0;

waiting1:

  /**************************************************************/
  /* extract information from database table 'links' row 'name' */
  /**************************************************************/
  sprintf(tmpp,"SELECT type,host,state,port FROM links WHERE name='%s'",name);
  if(mysql_query(dbsock, tmpp) != 0)
  {
    printf("rocOpenLink ERROR: cannot select name from %s\n",name);
    return(-1);
  }
  else
  {
    printf("rocOpenLink: name selected\n");
  }


  /* gets results from previous query */
  if( !(result = mysql_store_result(dbsock)) )
  {
    printf("ERROR in mysql_store_result()\n");
    return(CODA_ERROR);
  }
  else
  {
    numRows = mysql_num_rows(result);
    printf("nrow=%d\n",numRows);

    if(numRows == 1)
    {
      row = mysql_fetch_row(result);
      for(i=0; i<4; i++)
      {
        printf("fields [%1d] >>>%s<<<\n",i,row[i]);
      }
    }
    else
    {
      printf("rocOpenLinks: ERROR: unknown nrow=%d",numRows);
      return(-1);
    }

    mysql_free_result(result);
  }

  strcpy(type,row[0]);
  strcpy(host,row[1]);
  strcpy(state,row[2]);
  strcpy(chport,row[3]);
  port = atoi(chport);

  printf("parsing results: type=>%s< host=>%s< state=>%s< port=>%s< -> %d\n",
    type,host,state,chport,port);
  

  /*sergey: wait for state='waiting'*/
  if( strncmp(state,"waiting",7) )
  {
    printf(">>> In database link state is >%s< - we need 'waiting'\n",state);
#ifdef VXWORKS
    taskDelay(sysClkRateGet());
#else
    sleep(1);
#endif

    if(nwaits++ > 10)   /* after N attempts, disconnect from database and return */
	{
      dbDisconnect(dbsock);
      return(-1);
	}

	goto waiting1;
  }


  /* create a connection to given host and port */
  socketnum_local = LINK_establish(host, port);
  if(socketnum_local>0)
  {
    printf("host >%s<\n",host);
    printf("port=%d\n",port);

    /* set state 'up' */
    sprintf(tmpp,"UPDATE links SET state='up' WHERE name='%s'",name);
    printf("DB command >%s<\n",tmpp);
    if(mysql_query(dbsock,tmpp) != 0)
    {
      printf("ERROR: cannot UPDATE links SET state='downloaded'\n");
      return(-1);
    }
    else
    {
      printf("UPDATE process success\n");
    }
  }
  else
  {
    printf("ERROR: LINK_establish returns %d\n",socketnum_local);
    return(-1);
  }

  /* disconnect from database */
  dbDisconnect(dbsock);

  *port_return = port;
  *socketnum_return = socketnum_local;
  strcpy(host_return,host);

  return(0);
}

int
rocCloseLinkxxx()
{
  /* ?????
    if { "$direction" == "in" } {
	database query "DELETE FROM links WHERE name='$name'"
    }
  */

  return(0);
}
