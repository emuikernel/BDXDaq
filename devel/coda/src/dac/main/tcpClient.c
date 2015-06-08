
/* tcpClient.c - TCP client example */ 
/* DESCRIPTION This file contains the client-side of the VxWorks TCP example code. 
   The example code demonstrates the usage of several BSD 4.4-style socket routine calls. */ 

#ifdef VXWORKS

#include <vxWorks.h>
#include <sockLib.h>
#include <inetLib.h>
#include <stdioLib.h>
#include <strLib.h>
#include <hostLib.h>
#include <ioLib.h>

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>

#define TRUE 1
#define FALSE 0
#define OK 0
#define ERROR -1
#define STD_IN stdin

#endif

#include "libtcp.h"
#include "libdb.h"

#define INSTANT_PRINT /* will print instantly; if undefined, will print after whole buffer received */

static void
alarmHandler(int sig)
{
  printf("alarmHandler reached\n");fflush(stdout);
  /*signal(sig, alarmHandler);*/
}


/**************************************************************************** 
* * tcpClient - send requests to server over a TCP socket 
* * This routine connects over a TCP socket to a server, and sends a 
* user-provided message to the server. Optionally, this routine 
* waits for the server's reply message. 
* * This routine may be invoked as follows: 
* -> tcpClient "remoteSystem" 
* Message to send: 
* Hello out there 
* Would you like a reply (Y or N): 
* y 
* value =0 =0x0 
* -> MESSAGE FROM SERVER: 
* Server received your message 
* * RETURNS: OK, or ERROR if the message could not be sent to the server. */ 

#ifdef VXWORKS

STATUS 
tcpClient(char *serverName) 
{
#else

main(int argc, char *argv[])
{
  struct hostent *hptr;
#endif
 
  TREQUEST myRequest; /* request to send to server */ 
  struct sockaddr_in serverAddr; /* server's socket address */ 
  char replyBuf[REPLY_MSG_SIZE*100]; /* buffer for reply */ 
  char reply; /* if TRUE, expect reply back */ 
  int sockAddrSize; /* size of socket address structure */ 
  int sFd; /* socket file descriptor */ 
  int mlen; /* length of message */
  int rBytes;
  int i, portnum = 0;
  char hostname[128];
  MYSQL *dbsock;
  char tmp[1000], temp[100];
  char *ch;

#ifndef VXWORKS  /* make sure all arguments are there */
  if (argc<3) {
    printf("Usage: tcpClient <target name> [command]\n");
    exit(1);
  }
#endif

  /* create client's socket */ 
  if((sFd = socket (AF_INET, SOCK_STREAM, 0)) == ERROR)
  {
    perror ("socket"); 
    return (ERROR); 
  } 

  /* bind not required - port number is dynamic */ 
  /* build server socket address */ 
  sockAddrSize = sizeof (struct sockaddr_in); 
  bzero ((char *) &serverAddr, sockAddrSize); 
  serverAddr.sin_family = AF_INET; 
#ifdef VXWORKS
  /*  serverAddr.sin_len = (u_char) sockAddrSize; */
#endif


  /* Sergey: vxworks is not ready !!!!! */
#ifndef VXWORKS

  /* get port and host from DB; if no record in DB for <target name> - exit */
  dbsock = dbConnect(getenv("MYSQL_HOST"), "daq");

  /* use 'port' field */
  sprintf(tmp,"SELECT tcpClient_tcp FROM Ports WHERE name='%s'",argv[1]);
  /*
  printf("DB select: >%s<\n",tmp);
  */
  if(dbGetInt(dbsock, tmp, &portnum)==ERROR) return(ERROR);

  sprintf(tmp,"SELECT Host FROM Ports WHERE name='%s'",argv[1]);
  /*
  printf("DB select: >%s<\n",tmp);
  */
  if(dbGetStr(dbsock, tmp, hostname)==ERROR) return(ERROR);

  dbDisconnect(dbsock);
  /*  
  printf("hostname=>%s< portnum=%d\n",hostname,portnum);
  */

#endif

  serverAddr.sin_port = htons(portnum/*SERVER_PORT_NUM*/); 

#ifdef VXWORKS
  if (((serverAddr.sin_addr.s_addr = inet_addr (serverName)) == ERROR) && 
      ((serverAddr.sin_addr.s_addr = hostGetByName (serverName)) == ERROR))
  {
    perror ("unknown server name"); 
    close (sFd); 
    return (ERROR); 
  } 
#else
  hptr = gethostbyname(hostname);
  if(hptr == NULL) {
      printf("unknown hostname %s \n",hostname); 
      close(sFd);
      exit(1);
    } else {
      memcpy(&serverAddr.sin_addr,*(hptr->h_addr_list),sizeof(sizeof(struct in_addr)));
    }
#endif


  /*
    If the connection cannot be established immediately and O_NONBLOCK is set for
    the file descriptor for the socket, connect() shall fail and set errno to [EINPROGRESS],
    but the connection request shall not be aborted, and the connection shall be established
    asynchronously. Subsequent calls to connect() for the same socket, before the connection
    is established, shall fail and set errno to [EALREADY].

    When the connection has been established asynchronously, select() and poll() shall
    indicate that the file descriptor for the socket is ready for writing.
  */


goto a123;
  {
    int itmp, orig_flags;
    int on = 1; /* blocking */
    int off = 0; /* nonblocking */

    orig_flags = fcntl(sFd, F_GETFL, 0);
    printf("orig_flags=0x%08x\n",orig_flags);

	
    if(fcntl(sFd, F_SETFL, orig_flags | O_NONBLOCK) < 0)
    {
      perror("fcntl(O_NONBLOCK)");
    }
	
	/*
    if(fcntl(sFd, F_SETFL, orig_flags | O_NDELAY) < 0)
    {
      perror("fcntl(O_NDELAY)");
	}
	*/

	/*ioctl: No such file or directory (O_NDELAY or O_NONBLOCK ?)
    itmp = ioctl(sFd,O_NDELAY,(int) &on);
    if(itmp < 0) perror("ioctl");
	*/

  }
a123:

    /* see dpS/dpS.s/dplite.c how DP_cmd works !!! 
    {
      int cval = 1;
      int linger[2];

      linger[0] = 0;
      linger[1] = 0;

      setsockopt(sFd, SOL_SOCKET, SO_LINGER,
		 (char *)linger, sizeof(linger));
      
      setsockopt(sFd, IPPROTO_TCP, TCP_NODELAY, (char *)&cval, sizeof(int));
    }
	*/

  /*
  3.5.  How can I set the timeout for the connect() system call?

  From Richard Stevens (rstevens@noao.edu):

  Normally you cannot change this.  Solaris does let you do this, on a
  per-kernel basis with the ndd tcp_ip_abort_cinterval parameter.

  The easiest way to shorten the connect time is with an alarm() around
  the call to connect().  A harder way is to use select(), after setting
  the socket nonblocking.  Also notice that you can only shorten the
  connect time, there's normally no way to lengthen it.

  From Andrew Gierth (andrew@erlenstar.demon.co.uk):

  First, create the socket and put it into non-blocking mode, then call
  connect(). There are three possibilities:

  o  connect succeeds: the connection has been successfully made (this
     usually only happens when connecting to the same machine)

  o  connect fails: obvious

  o  connect returns -1/EINPROGRESS. The connection attempt has begun,
     but not yet completed.

  If the connection succeeds:

  o  the socket will select() as writable (and will also select as
     readable if data arrives)

  If the connection fails:

  o  the socket will select as readable *and* writable, but either a
     read or write will return the error code from the connection
     attempt. Also, you can use getsockopt(SO_ERROR) to get the error
     status - but be careful; some systems return the error code in the
     result parameter of getsockopt, but others (incorrectly) cause the
     getsockopt call *itself* to fail with the stored value as the
     error.
*/



#ifndef VXWORKS
  signal(SIGALRM,alarmHandler);
  alarm(10); /* in 3 seconds connect call will be interrupted if did not finished */
#endif

  /* connect to server */ 
  /*printf("connecting to server ...\n");*/ 
  if (connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR)
  {
    /*perror ("connect");*/ 
    close (sFd); 
	return (ERROR);
  }
  /*printf("connected !!!\n");*/ 

#ifndef VXWORKS
  alarm(0); /* clear alarm so it will not interrupt us */
#endif



  /* build request, prompting user for message */ 
#ifdef VXWORKS
  printf ("Message to send: \n"); 
  mlen = read (STD_IN, myRequest.message, REQUEST_MSG_SIZE); 
  myRequest.msgLen = mlen; 
  myRequest.message[mlen - 1] = '\0'; 
  printf ("Would you like a reply (Y or N): \n"); 
  read (STD_IN, &reply, 1); 
  switch (reply) { 
  case 'y': 
  case 'Y': 
    myRequest.reply = TRUE; 
    break; 
  default: 
    myRequest.reply = FALSE; 
    break; 
  } 
#else

  myRequest.msgLen = strlen(argv[2]);
  sprintf(myRequest.message,"%s\0",argv[2]);
  myRequest.reply = TRUE;
  /*  
printf("1: 0x%08x 0x%08x\n", myRequest.msgLen, myRequest.reply);
  */
  /* convert integers to network byte order */
  myRequest.msgLen = htonl(myRequest.msgLen);
  myRequest.reply = htonl(myRequest.reply);
  /*
printf("2: 0x%08x 0x%08x\n", myRequest.msgLen, myRequest.reply);
  */
  /*
  printf(" Sending %d bytes: %s (len=%d)\n",
    myRequest.msgLen, myRequest.message, sizeof (myRequest));
  fflush(stdout);
  */
#endif

  /* send request to server */ 
  if(write(sFd, (char *) &myRequest, sizeof(TREQUEST)) == ERROR)
  {
    perror("write"); 
    close(sFd); 
    return(ERROR);
  } 

  if(myRequest.reply) /* if expecting reply, read and display it */
  { 
    int remB = 0;
	/*
    printf("MESSAGE FROM SERVER:\n");
	*/
    while( (rBytes = read(sFd, (char *)&replyBuf[remB], REPLY_MSG_SIZE)) > 0)
    {
#ifdef INSTANT_PRINT
      for(i=0;i<rBytes;i++) printf("%c",replyBuf[remB+i]);fflush(stdout);
#endif
      remB += rBytes;
    }
#ifdef INSTANT_PRINT
    printf("\n");
	fflush(stdout);
#else
    ch = strstr(replyBuf,"value = ");
    if(ch == NULL) mlen = remB;
    else           mlen = (int)ch - (int)replyBuf;
	/*
    printf("buf starts at 0x%08x, strstr returns 0x%08x, mlen=%d\n",
      replyBuf,ch,mlen);
	fflush(stdout);
	*/
    for(i=0; i<mlen; i++)
    {
      printf("%c",replyBuf[i]);
    }
    printf("\n");
#endif
  } 
  close(sFd);
 
  return(OK);
 }
