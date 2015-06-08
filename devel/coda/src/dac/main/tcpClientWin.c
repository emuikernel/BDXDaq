
/* tcpClient.c - TCP client for Windows */ 

#ifndef VXWORKS

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






/* defines */ 
#define SERVER_PORT_NUM 6001      /* server's port number for bind() */ 
#define REQUEST_MSG_SIZE 1024     /* max size of request message */ 
#define REPLY_MSG_SIZE 4096        /* max size of reply message */ 

/* structure for requests from clients to server */ 
typedef struct trequest {  
  char message[REQUEST_MSG_SIZE]; /* message buffer */ 
} TREQUEST;



#define SEND_AND_RECEIVE \
  /* send request to server */ \
  if(write(sFd, (char *) &myRequest, sizeof(TREQUEST)) == ERROR) \
  { \
    perror("write"); \
    close(sFd); \
    return(ERROR); \
  } \
  /* always expecting reply */ \
  rBytes = read(sFd, (char *)&replyBuf[0], REPLY_MSG_SIZE); \
  printf("rBytes=%d\n",rBytes); \
  printf("Respond from command: 0x%02x\n",replyBuf[0]); \
  if(replyBuf[0]&0x80) \
  { \
    mlen = rBytes; \
    printf("Error: "); \
    for(i=1; i<mlen; i++) printf("%c",replyBuf[i]); \
    printf("\n"); \
  } \
  else if(rBytes > 1) \
  { \
    mlen = rBytes/2; \
    ptr16 = (unsigned short *)&replyBuf[1]; \
    printf("Received data:\n"); \
    for(i=0; i<mlen; i++) \
    { \
      printf("[%3d] %6d (0x%04x)\n",i,ptr16[i],ptr16[i]); \
    } \
    printf("\n"); \
  }


static void
alarmHandler(int sig)
{
  printf("alarmHandler reached\n");fflush(stdout);
}


/**************************************************************************** 
* * tcpClient - send requests to server over a TCP socket 
* * This routine connects over a TCP socket to a server, and sends a 
* user-provided message to the server. Then routine 
* waits for the server's reply message. 
* * This routine may be invoked as follows: 
* -> tcpClient <remoteSystem> <message>
* for example:
* -> tcpClient croctest2 i
****************************************************************************/


main(int argc, char *argv[])
{
  struct hostent *hptr;
 
  TREQUEST myRequest; /* request to send to server */ 
  struct sockaddr_in serverAddr; /* server's socket address */ 
  unsigned char replyBuf[REPLY_MSG_SIZE*100]; /* buffer for reply */ 
  char reply; /* if TRUE, expect reply back */ 
  int sockAddrSize; /* size of socket address structure */ 
  int sFd; /* socket file descriptor */ 
  int mlen; /* length of message */
  int rBytes;
  int i, portnum = 0;
  char hostname[128];
  int dbsock;
  char tmp[1000], temp[100];
  char *ch;
  unsigned int *ptr32;
  unsigned short *ptr16;

  /* make sure all arguments are there */
  if (argc<2)
  {
    printf("Usage: tcpClientWin <target name>\n");
    exit(1);
  }


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





  /* normally port number is 5002, but not guaranteed .. */
  strcpy(hostname,argv[1]);
  portnum = SERVER_PORT_NUM;
  printf("hostname=>%s< portnum=%d\n",hostname,portnum);




  serverAddr.sin_port = htons(portnum); 

  hptr = gethostbyname(hostname);
  if(hptr == NULL) {
      printf("unknown hostname %s \n",hostname); 
      close(sFd);
      exit(1);
    } else {
      memcpy(&serverAddr.sin_addr,*(hptr->h_addr_list),sizeof(sizeof(struct in_addr)));
    }


  /*
    If the connection cannot be established immediately and O_NONBLOCK is set for
    the file descriptor for the socket, connect() shall fail and set errno to [EINPROGRESS],
    but the connection request shall not be aborted, and the connection shall be established
    asynchronously. Subsequent calls to connect() for the same socket, before the connection
    is established, shall fail and set errno to [EALREADY].

    When the connection has been established asynchronously, select() and poll() shall
    indicate that the file descriptor for the socket is ready for writing.
  */

  signal(SIGALRM,alarmHandler);
  alarm(3); /* in 3 seconds connect call will be interrupted if did not finished */


  /* connect to server */ 
  if (connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR)
  {
    /*perror ("connect");*/ 
    close (sFd); 
	return (ERROR);
  }

  alarm(0); /* clear alarm so it will not interrupt us */




  printf("\n\n test1 (unknown command)\n");
  myRequest.message[0] = 0x0E;
  SEND_AND_RECEIVE;

  printf("\n\n test2 (set base)\n");
  myRequest.message[0] = 0x00;
  ptr32 = (unsigned int *)&myRequest.message[1];
  *ptr32 = 0x08510000;
  SEND_AND_RECEIVE;

  printf("\n\n test3 (get status)\n");
  myRequest.message[0] = 0x05;
  SEND_AND_RECEIVE;



  close(sFd);
 
  return(OK);
 }


#else

/* dummy VXWORKS version */

void
tcpClientWin()
{
  printf("tcpClientWin is dummy for VXWORKS\n");
}

#endif
