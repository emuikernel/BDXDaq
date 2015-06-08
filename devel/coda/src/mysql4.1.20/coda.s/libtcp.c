/*************************/
/* trying emulate DP_cmd */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define TRUE 1
#define FALSE 0
#define OK 0
#define ERROR -1
#define STD_IN stdin


#include "libtcp.h"
#include "libdb.h"

int
tcpClientCmd(char *target, char *command, char *result)
{
  struct hostent *hptr;

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


  /* create client's socket */ 
  if((sFd = socket (AF_INET, SOCK_STREAM, 0)) == ERROR)
  {
    perror ("socket"); 
    return (ERROR); 
  } 

  /* bind not required - port number is dynamic */ 
  /* build server socket address */ 
  sockAddrSize = sizeof (struct sockaddr_in); 
  memset ((char *) &serverAddr, 0, sockAddrSize); 
  serverAddr.sin_family = AF_INET; 







  /* get port and host from DB; if no record in DB for <target name> - exit */

  dbsock = dbConnect(getenv("MYSQL_HOST"), "daq");

  /* use 'port' field */
  sprintf(tmp,"SELECT tcpClient_tcp FROM Ports WHERE name='%s'",target);
  /*
  printf("DB select: >%s<\n",tmp);
  */
  if(dbGetInt(dbsock, tmp, &portnum)==ERROR) return(ERROR);

  sprintf(tmp,"SELECT Host FROM Ports WHERE name='%s'",target);
  /*
  printf("DB select: >%s<\n",tmp);
  */
  if(dbGetStr(dbsock, tmp, hostname)==ERROR) return(ERROR);

  dbDisconnect(dbsock);
  
  /*
  printf("hostname=>%s< portnum=%d\n",hostname,portnum);
  */

  serverAddr.sin_port = htons(portnum/*SERVER_PORT_NUM*/); 

  hptr = gethostbyname(hostname);
  if(hptr == NULL)
  {
    printf("unknown hostname %s \n",hostname); 
    close(sFd);
    exit(1);
  }
  else
  {
    memcpy(&serverAddr.sin_addr,*(hptr->h_addr_list),sizeof(sizeof(struct in_addr)));
  }


  /* connect to server */ 
  if (connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR)
  {
    perror ("connect"); 
    close (sFd); 
    return (ERROR); 
  } 

  /* build request, prompting user for message */ 

  myRequest.msgLen = strlen(command);
  sprintf(myRequest.message,"%s\0",command);
  myRequest.reply = TRUE;

  /*
  printf(" Sending %d bytes: %s (len=%d)\n",
    myRequest.msgLen, myRequest.message, sizeof (myRequest));
  fflush(stdout);
  */


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
    while( (rBytes = read(sFd, (char *)&replyBuf[remB], REPLY_MSG_SIZE)) > 0)
    { 
      remB += rBytes;
    }
	/*
    printf("MESSAGE FROM SERVER (%d bytes):\n",remB);
    for(i=0; i<remB; i++)
    {
      printf("%c",replyBuf[i]);
    }
    printf("\n");
	fflush(stdout);
	*/

    /*Sergey: DP_ask was returning 'value = ...', we are NOT !!!*/
    /*mlen = remB;*/
    ch = strstr(replyBuf,"value = ");
    if(ch == NULL) mlen = remB;
    else           mlen = (int)ch - (int)replyBuf;

	/*
    printf("buf starts at 0x%08x, strstr returns 0x%08x, mlen=%d\n",
      replyBuf,ch,mlen);
	fflush(stdout);
	*/


	/* print 
    printf("print ---=================\n");
    for(i=0; i<mlen; i++)
    {
      printf("%c",replyBuf[i]);
    }
    printf("\n");
    printf("======---=================\n");
	*/

    /* return */

	/* POTENTIAL PROBLEM HERE: WE DO NOT KNOW THE SIZE OF 'result', MAYBE LESS THEN 'mlen' !!!
    solution: first 4 bytes of 'result' must contails result's length in bytes, then we can adjust
    'mlen' by following:
    {
      int *ptr = (int *)result;        
      mlen = MIN(mlen,ptr[0]);
    }
	*/

    strncpy(result,replyBuf,mlen);
    /*result[mlen] = '\0';*/
    result[mlen] = '\n';
	/*
    printf("result====================\n");
    printf("%s",result);
    printf("==========================\n");
	*/
  } 
  close(sFd);
 
  return(OK);
}



/*************************/
/* exact copy of previous but returning 'value = ...' (a la dp-ask)
/*************************/
int
tcpClientDP(char *target, char *command, char *result)
{
  struct hostent *hptr;

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


  /* create client's socket */ 
  if((sFd = socket (AF_INET, SOCK_STREAM, 0)) == ERROR)
  {
    perror ("socket"); 
    return (ERROR); 
  } 

  /* bind not required - port number is dynamic */ 
  /* build server socket address */ 
  sockAddrSize = sizeof (struct sockaddr_in); 
  memset ((char *) &serverAddr, 0, sockAddrSize); 
  serverAddr.sin_family = AF_INET; 


  /* get port and host from DB; if no record in DB for <target name> - exit */

  dbsock = dbConnect(getenv("MYSQL_HOST"), "daq"/*getenv("EXPID")*/);

  /* use 'port' field */
  sprintf(tmp,"SELECT tcpClient_tcp FROM Ports WHERE name='%s'",target);
  /*
  printf("DB select: >%s<\n",tmp);
  */
  if(dbGetInt(dbsock, tmp, &portnum)==ERROR) return(ERROR);

  sprintf(tmp,"SELECT Host FROM Ports WHERE name='%s'",target);
  /*
  printf("DB select: >%s<\n",tmp);
  */
  if(dbGetStr(dbsock, tmp, hostname)==ERROR) return(ERROR);

  dbDisconnect(dbsock);
  /*
  printf("hostname=>%s< portnum=%d\n",hostname,portnum);
  */

  serverAddr.sin_port = htons(portnum/*SERVER_PORT_NUM*/); 

  hptr = gethostbyname(hostname);
  if(hptr == NULL) {
      printf("unknown hostname %s \n",hostname); 
      close(sFd);
      exit(1);
    } else {
      memcpy(&serverAddr.sin_addr,*(hptr->h_addr_list),sizeof(sizeof(struct in_addr)));
    }


  /* connect to server */ 
  if (connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR) { 
    perror ("connect"); 
    close (sFd); 
    return (ERROR); 
  } 

  /* build request, prompting user for message */ 

  myRequest.msgLen = strlen(command);
  sprintf(myRequest.message,"%s\0",command);
  myRequest.reply = TRUE;

  /*
  printf(" Sending %d bytes: %s (len=%d)\n",
    myRequest.msgLen, myRequest.message, sizeof (myRequest));
  fflush(stdout);
  */


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
    while( (rBytes = read(sFd, (char *)&replyBuf[remB], REPLY_MSG_SIZE)) > 0)
    { 
      remB += rBytes;
    }
	/*
    printf("MESSAGE FROM SERVER (%d bytes):\n",remB);
    for(i=0; i<remB; i++)
    {
      printf("%c",replyBuf[i]);
    }
    printf("\n");
	fflush(stdout);
	*/

    /*Sergey: DP_ask was returning 'value = ...', we are NOT !!!*/
    mlen = remB;
	/*
    ch = strstr(replyBuf,"value = ");
    if(ch == NULL) mlen = remB;
    else           mlen = (int)ch - (int)replyBuf;
	*/

	/*
    printf("buf starts at 0x%08x, strstr returns 0x%08x, mlen=%d\n",
      replyBuf,ch,mlen);
	fflush(stdout);
	*/


	/* print 
    printf("print ---=================\n");
    for(i=0; i<mlen; i++)
    {
      printf("%c",replyBuf[i]);
    }
    printf("\n");
    printf("======---=================\n");
	*/

    /* return */
    strncpy(result,replyBuf,mlen);

    result[mlen] = '\0';
    /*result[mlen] = '\n';*/

	/*
    printf("result====================\n");
    printf("%s",result);
    printf("==========================\n");
	*/
  } 
  close(sFd);
 
  return(OK);
}

/*************************/
/*************************/
