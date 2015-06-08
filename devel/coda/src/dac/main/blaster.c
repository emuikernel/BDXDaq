/* blaster.c

  ROC:
blaster("129.57.68.21",1234,65536)
  clon10:
blastee 1234 65536
*/

#ifndef VXWORKS

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <sys/file.h>

#ifndef Linux
#include <sys/lock.h>
#endif

#include <netinet/tcp.h>

#else

#include "vxWorks.h"
#include "wdLib.h"
#include "socket.h"
#include "in.h"

#include "netinet/tcp.h"

#endif

#include "blaster.h"

int blasterStop;
extern int errno;

#ifndef VXWORKS
main(int argc, char *argv[])
#else
blaster(char *targetAddr, int port, int sockbufsize)
#endif
{
  struct sockaddr_in sin;
  int s, ix = 0;
  char *buffer;
  int on = 1;
  int y, optval;
  int sendsize;
#ifndef VXWORKS
  int sockbufsize;
  struct hostent *hp, *gethostbyname ();

  struct in_addr **pptr;
  struct sockaddr_in  servaddr;
  char txt[INET_ADDRSTRLEN];

  if(argc != 4)
  {
	printf ("usage: %s targetname port sockbufsize\n", argv [0]);
	exit(1);
  }
#endif

  bzero ((char *)&sin, sizeof (sin));

  s = socket (AF_INET, SOCK_STREAM, 0);
  if(s < 0) printf ("cannot open socket\n");

#ifndef VXWORKS
  hp = gethostbyname (argv[1]);



  /*some printfs*/
  printf(">>> hostname >%s<\n",hp->h_name);
  pptr = (struct in_addr **) hp->h_addr_list;
  for( ; *pptr != NULL; pptr++)
  {

    memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
    printf(">>> sin_port=%u\n",servaddr.sin_port);
    printf(">>> sin_addr=%u\n",servaddr.sin_addr);
    inet_ntop(AF_INET,*pptr,txt,INET_ADDRSTRLEN);
    printf(">>> txt >%s<\n",txt);

	/*

	 */

	/*
    if ((err = connect(sockfd, (SA *) &servaddr, sizeof(servaddr))) < 0) {
      if (debug) fprintf(stderr, "tcp_connect: error attempting to connect to server\n");
    }
    else {
      fprintf(stderr, "tcp_connect: connected to server\n");
      break;
    }
	*/
  }






  if (hp == 0 && (sin.sin_addr.s_addr = inet_addr (argv [1])) == -1)
  {
	fprintf (stderr, "%s: unkown host\n", argv [1]);
	exit(2);
  }

  if(hp != 0) bcopy (hp->h_addr, &sin.sin_addr, hp->h_length);

  sin.sin_port = htons (atoi (argv [2]));
  sockbufsize  = atoi (argv [3]);

#else
  sin.sin_addr.s_addr	= inet_addr (targetAddr);
  sin.sin_port	= htons (port);
#endif
  sin.sin_family 	= AF_INET;

  sendsize = SENDSIZE;
  printf("sendsize=%d\n",sendsize);

  if((buffer = (char *) malloc (sendsize)) == NULL)
  {
	printf ("cannot allocate buffer of sendsize %d\n", sendsize);
	exit (1);
  }

  if(setsockopt(s, SOL_SOCKET, SO_SNDBUF, &sockbufsize, sizeof(sockbufsize))<0)
  {
	printf("setsockopt SO_SNDBUF failed\n");
    free(buffer);
	exit(1);
  }

  {
    int nbytes, lbytes;

    nbytes = 0;
    lbytes = 4;

    getsockopt(s, SOL_SOCKET, SO_SNDBUF, (int *) &nbytes, &lbytes); 
    printf("socket buffer size is %d(0x%08x) bytes\n",nbytes,nbytes);
  }

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

  if(connect (s, (const struct sockaddr *)&sin, sizeof (sin)) < 0)
  {
#ifndef VXWORKS 
	perror("connect");
#endif
    printf("connect failed: host %s port %d\n", inet_ntoa (sin.sin_addr),
	       ntohs (sin.sin_port));

	free(buffer);
	exit(1);
  }
    
  printf("connected and preparing to send...\n");
    
  blasterStop = 0;

  /*if((y = send(s, buffer, sendsize, 0)) < 0)*/
  if((y = write(s, buffer, sendsize)) < 0)
  {
	perror ("blaster write error: ");
  }

#ifdef VXWORKS
  taskDelay(sysClkRateGet());
  printf("entering infinite loop\n");
  taskDelay(sysClkRateGet());
#endif

  {
    int nbytes, lbytes;

    nbytes = 0;
    lbytes = 4;

    getsockopt(s, SOL_SOCKET, SO_SNDBUF, (int *) &nbytes, &lbytes); 
    printf("socket buffer size is %d(0x%08x) bytes\n",nbytes,nbytes);
  }

  for (;;)
  {
	if(blasterStop == 1) break;

    if ((y = send(s, buffer, sendsize, 0)) < 0)
	/*if ((y = write(s, buffer, sendsize)) < 0)*/
    {
      perror ("blaster write error: ");
      blasterStop = 1;
      break;
    }
    else
    {
      /*printf("sent %d bytes\n",y)*/;
    }
#ifdef VXWORKS
    /* taskDelay(sysClkRateGet()); */
#endif
  }
    
  close (s);
    
  free (buffer);
  printf ("blaster exit.\n");
}

#ifdef VXWORKS
blaster1()
{
  blaster("129.57.68.20",1231,SENDSIZE);
}
blaster2()
{
  blaster("129.57.68.20",1232,SENDSIZE);
}
blaster3()
{
  blaster("129.57.68.20",1233,SENDSIZE);
}
blaster4()
{
  blaster("129.57.68.20",1234,SENDSIZE);
}
blaster5()
{
  blaster("129.57.68.20",1235,SENDSIZE);
}
blaster6()
{
  blaster("129.57.68.20",1236,SENDSIZE);
}
blaster7()
{
  blaster("129.57.68.20",1237,SENDSIZE);
}
blaster8()
{
  blaster("129.57.68.20",1238,SENDSIZE);
}
blaster9()
{
  blaster("129.57.68.20",1239,SENDSIZE);
}
blaster10()
{
  blaster("129.57.68.20",1240,SENDSIZE);
}
blaster11()
{
  blaster("129.57.68.20",1241,SENDSIZE);
}
blaster12()
{
  blaster("129.57.68.1",1242,SENDSIZE);
}
blaster13()
{
  blaster("129.57.68.1",1243,SENDSIZE);
}
blaster14()
{
  blaster("129.57.68.1",1244,SENDSIZE);
}
blaster15()
{
  blaster("129.57.68.1",1245,SENDSIZE);
}
blaster16()
{
  blaster("129.57.68.1",1246,SENDSIZE);
}
blaster17()
{
  blaster("129.57.68.1",1247,SENDSIZE);
}
blaster18()
{
  blaster("129.57.68.1",1248,SENDSIZE);
}
blaster19()
{
  blaster("129.57.68.1",1249,SENDSIZE);
}
blaster20()
{
  blaster("129.57.68.1",1250,SENDSIZE);
}
blaster21()
{
  blaster("129.57.68.1",1251,SENDSIZE);
}
blaster22()
{
  blaster("129.57.68.1",1252,SENDSIZE);
}
blaster23()
{
  blaster("129.57.68.1",1253,SENDSIZE);
}
blaster24()
{
  blaster("129.57.68.1",1254,SENDSIZE);
}
blaster25()
{
  blaster("129.57.68.1",1255,SENDSIZE);
}
blaster26()
{
  blaster("129.57.68.1",1256,SENDSIZE);
}
blaster27()
{
  blaster("129.57.68.1",1257,SENDSIZE);
}
blaster28()
{
  blaster("129.57.68.1",1258,SENDSIZE);
}
blaster29()
{
  blaster("129.57.68.1",1259,SENDSIZE);
}
blaster30()
{
  blaster("129.57.68.1",1260,SENDSIZE);
}
#endif
