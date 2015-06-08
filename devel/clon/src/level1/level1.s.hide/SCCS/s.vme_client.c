h64165
s 00012/00000/00281
d D 1.5 07/03/20 16:41:07 boiarino 6 5
c *** empty log message ***
e
s 00180/00016/00101
d D 1.4 02/07/06 23:45:30 boiarino 5 4
c add vme_bigsend
c 
e
s 00003/00002/00114
d D 1.3 02/06/25 18:23:56 boiarino 4 3
c add "close (sFd);"
c 
e
s 00085/00073/00031
d D 1.2 02/06/25 16:42:44 boiarino 3 1
c cosmetic
c 
e
s 00000/00000/00000
d R 1.2 01/10/22 12:58:17 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/vme_client.c
e
s 00104/00000/00000
d D 1.1 01/10/22 12:58:16 boiarino 1 0
c date and time created 01/10/22 12:58:16 by boiarino
e
u
U
f e 0
t
T
I 3

I 6
#ifdef VXWORKS

void
vme_client_dummy()
{
  return;
}

#else

E 6
E 3
I 1
/* vme_client.c - TCP client code that runs on UNIX host */ 
 
/* UNIX header files */ 
#include <stdio.h> 
I 5
#include <errno.h> 
E 5
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h> 
I 5

E 5
#include "vme_tcpInfo.h"
#include "vme_client.h"
#include "vme_error.h"
I 5
#include "vme_download.h"
E 5
 

D 3
int vme_send(clientRequest) 
    struct request *clientRequest; 
E 3
I 3
int
I 5
vme_bigsend(struct request *clientRequest) 
{
  int sFd; 
  int mlen; 
  struct request myRequest; 
  struct sockaddr_in serverSockAddr; 
  struct hostent *hostInfo; 
  unsigned char returnVal;
  int i, j, cc, rembytes, n_retry2;
  char sendBuf, recBuf, *look;
  char *bufferp = 0;
  int recv_flags = MSG_WAITALL; /* wait for all the data requested */

  /************************/
  /* establish connection */
  /************************/
 
  /* create socket */ 
  if((sFd = socket(PF_INET, SOCK_STREAM, 0)) == ERROR)
  { 
    vme_seterror(SOCKET_CREATE_FAILED,"vme_client: socket: ");
    return(SOCKET_CREATE_FAILED);
  } 
 
  /* bind not required - port number is dynamic */ 

  /* build server socket address */ 
  bzero (&serverSockAddr, SOCK_ADDR_SIZE); 
  serverSockAddr.sin_family = PF_INET; 
  serverSockAddr.sin_port = htons (SERVER_PORT_NUM); 
  /* serverSockAddr.sin_addr.s_addr = inet_addr (SERVER_INET_ADDR); */ 
  if((hostInfo = gethostbyname(clientRequest->hostname)) != NULL)
  {
    serverSockAddr.sin_addr.s_addr = *(int *)hostInfo->h_addr;
  }
  else
  { 
    vme_seterror(GETHOSTBYNAME_FAILED,clientRequest->hostname);
    close (sFd);
    return(GETHOSTBYNAME_FAILED);
  }

  /* connect to server */ 
  if(connect(sFd, (struct sockaddr *) &serverSockAddr, SOCK_ADDR_SIZE)
          == ERROR)
  {
    vme_seterror(CONNECT_FAILED,"vme_client: connect: ");
    close(sFd);
	return(CONNECT_FAILED); 
  }
 
  /* send request to server */ 
  if(write (sFd, (char *)clientRequest, sizeof (struct request)) == ERROR)
  { 
    vme_seterror(SOCKET_WRITE_FAILED,"vme_client: write: "); 
    close(sFd); 
    return(SOCKET_WRITE_FAILED); 
  }

  /* wait until server is ready */
  if(read(sFd, (char *)clientRequest, sizeof (struct request)) == ERROR)
  {
    vme_seterror(SOCKET_READ_FAILED,"vme_client: read: ");
    close(sFd);
    return(SOCKET_READ_FAILED);
  }

  /* check message that server sent back to see if server is really ready */
  if(clientRequest->command == MEM_TEST_ERROR)
  {
    vme_seterror(MEM_TEST_ERROR,"vme_client: addr space unavail: ");
  }


  /*****************/
  /* transfer data */
  /*****************/

  printf("vme_client: command from vme_server = %d\n",clientRequest->command);

  /* write data to server for a BIGDATA_DOWNLOAD */
  if(clientRequest->command == SERVER_READING)
  {
    printf("vme_client: SERVER_READING\n");
	j = write(sFd, (char *)(clientRequest->data),
                                  (clientRequest->size*clientRequest->length));
	j = read(sFd, (char *)clientRequest, sizeof (struct request));
    if(clientRequest->command == NETWORK_CHECKSUM_ERROR)
	   vme_seterror(NETWORK_CHECKSUM_ERROR,"vme_client: vme_server reports: ");
	clientRequest->command = BIGDATA_DOWNLOAD;
  }

  /* receive data from server if doing a BIGDATA_UPLOAD */
  if(clientRequest->command == SERVER_WRITING)
  {
    printf("vme_client: SERVER_WRITING\n");
    printf("len=0x%08x(%d)\n",clientRequest->length*clientRequest->size,
                              clientRequest->length*clientRequest->size);

    rembytes = clientRequest->length*clientRequest->size;
    n_retry2 = 0;
    bufferp = clientRequest->data;
    while(rembytes)
    {

retry1:

      if((cc = recv(sFd, (char *)bufferp, rembytes, recv_flags)) == -1)
      {
        if(errno == EWOULDBLOCK) goto retry1;
        if(errno != ECONNRESET)
        {
          printf("cc = %d - exit\n", cc);
          exit(0);
        }
      }

      if(cc == 0)
      {
        printf("process died - exit\n");
        exit(0);
      }
      else if(cc > rembytes)
      {
        printf("returned more bytes than requested!?!?!?! - exit\n");
        exit(0);
      }
      else if(cc != rembytes)
      {
        printf("retry ...\n");
        n_retry2++;
      }
    
      bufferp += cc;
      rembytes -= cc;
    }

    clientRequest->command = BIGDATA_UPLOAD;
  }

  returnVal = 0;
    
  /* close all open files before exiting */ 
  close(sFd); 

  return(vme_checkerror()); 
}


int
E 5
vme_send(struct request *clientRequest) 
E 3
D 5
{ 
E 5
I 5
{
E 5
D 3
    int sFd; 
    int mlen; 
    struct request myRequest; 
    struct sockaddr_in serverSockAddr; 
    struct hostent *hostInfo; 
    unsigned char returnVal;
    int i,j;
    char sendBuf, recBuf, *look;
    char buf[100];
E 3
I 3
  int sFd; 
  int mlen; 
  struct request myRequest; 
  struct sockaddr_in serverSockAddr; 
  struct hostent *hostInfo; 
  unsigned char returnVal;
  int i,j;
  char sendBuf, recBuf, *look;
  char buf[100];
I 5

  /************************/
  /* establish connection */
  /************************/
E 5
E 3
 
D 3
    /* create socket */ 
    if ((sFd = socket(PF_INET, SOCK_STREAM, 0)) == ERROR) { 
	vme_seterror(SOCKET_CREATE_FAILED,"vme_client: socket: ");
	return(SOCKET_CREATE_FAILED);
    } 
E 3
I 3
  /* create socket */ 
  if((sFd = socket(PF_INET, SOCK_STREAM, 0)) == ERROR)
  { 
    vme_seterror(SOCKET_CREATE_FAILED,"vme_client: socket: ");
    return(SOCKET_CREATE_FAILED);
  } 
E 3
 
D 3
    /* bind not required - port number is dynamic */ 
E 3
I 3
  /* bind not required - port number is dynamic */ 
E 3
D 4
     
E 4
I 4

E 4
D 3
    /* build server socket address */ 
    bzero (&serverSockAddr, SOCK_ADDR_SIZE); 
    serverSockAddr.sin_family = PF_INET; 
    serverSockAddr.sin_port = htons (SERVER_PORT_NUM); 
    /* serverSockAddr.sin_addr.s_addr = inet_addr (SERVER_INET_ADDR); */ 
    if ((hostInfo=gethostbyname(clientRequest->hostname)) != NULL){ 
                        serverSockAddr.sin_addr.s_addr = *(int *)hostInfo->h_addr; } 
        else { 
            vme_seterror(GETHOSTBYNAME_FAILED,clientRequest->hostname);
	    return(GETHOSTBYNAME_FAILED);
        } 
    //CHANGE1
    /* connect to server */ 
    if (connect (sFd, (struct sockaddr *) &serverSockAddr, SOCK_ADDR_SIZE) == ERROR) { 
	vme_seterror(CONNECT_FAILED,"vme_client: connect: "); 
	close (sFd); 
	return (CONNECT_FAILED); 
    } 
    /* send request to server */ 
    if (write (sFd, (char *)clientRequest, sizeof (struct request)) == ERROR) { 
        vme_seterror(SOCKET_WRITE_FAILED,"vme_client: write: "); 
        close (sFd); 
        return (SOCKET_WRITE_FAILED); 
    }
    /* wait until server is ready */
    if (read (sFd, (char *)clientRequest, sizeof (struct request)) == ERROR) {
	vme_seterror(SOCKET_READ_FAILED,"vme_client: read: ");
E 3
I 3
  /* build server socket address */ 
  bzero (&serverSockAddr, SOCK_ADDR_SIZE); 
  serverSockAddr.sin_family = PF_INET; 
  serverSockAddr.sin_port = htons (SERVER_PORT_NUM); 
  /* serverSockAddr.sin_addr.s_addr = inet_addr (SERVER_INET_ADDR); */ 
D 5
  if((hostInfo=gethostbyname(clientRequest->hostname)) != NULL)
E 5
I 5
  if((hostInfo = gethostbyname(clientRequest->hostname)) != NULL)
E 5
  {
    serverSockAddr.sin_addr.s_addr = *(int *)hostInfo->h_addr;
  }
  else
  { 
    vme_seterror(GETHOSTBYNAME_FAILED,clientRequest->hostname);
I 4
    close (sFd);
E 4
    return(GETHOSTBYNAME_FAILED);
  }

D 5
  //CHANGE1
E 5
  /* connect to server */ 
D 5
  if(connect(sFd, (struct sockaddr *) &serverSockAddr, SOCK_ADDR_SIZE) == ERROR) { 
	vme_seterror(CONNECT_FAILED,"vme_client: connect: ");
E 3
D 4
	close (sFd);
E 4
I 4
    close (sFd);
E 4
D 3
	return (SOCKET_READ_FAILED);
    }
    /* check message that server sent back to see if server is really ready */
    if (clientRequest->command == MEM_TEST_ERROR) {
	vme_seterror(MEM_TEST_ERROR,"vme_client: addr space unavail: ");
    }
    /* write data to server for a VME_DOWNLOAD */
    if (clientRequest->command == SERVER_READING) {
	j = write (sFd, (char *)(clientRequest->data), (clientRequest->size * clientRequest->length));
	j = read (sFd, (char *)clientRequest, sizeof (struct request));
	if (clientRequest->command == NETWORK_CHECKSUM_ERROR)
	    vme_seterror(NETWORK_CHECKSUM_ERROR,"vme_client: vme_server reports: ");
E 3
I 3
	return (CONNECT_FAILED); 
E 5
I 5
  if(connect(sFd, (struct sockaddr *) &serverSockAddr, SOCK_ADDR_SIZE)
          == ERROR)
  {
    vme_seterror(CONNECT_FAILED,"vme_client: connect: ");
    close(sFd);
	return(CONNECT_FAILED); 
E 5
  }
 
  /* send request to server */ 
  if(write (sFd, (char *)clientRequest, sizeof (struct request)) == ERROR)
  { 
    vme_seterror(SOCKET_WRITE_FAILED,"vme_client: write: "); 
D 5
    close (sFd); 
    return (SOCKET_WRITE_FAILED); 
E 5
I 5
    close(sFd); 
    return(SOCKET_WRITE_FAILED); 
E 5
  }

  /* wait until server is ready */
D 5
  if(read (sFd, (char *)clientRequest, sizeof (struct request)) == ERROR)
E 5
I 5
  if(read(sFd, (char *)clientRequest, sizeof (struct request)) == ERROR)
E 5
  {
    vme_seterror(SOCKET_READ_FAILED,"vme_client: read: ");
D 5
    close (sFd);
    return (SOCKET_READ_FAILED);
E 5
I 5
    close(sFd);
    return(SOCKET_READ_FAILED);
E 5
  }

  /* check message that server sent back to see if server is really ready */
  if(clientRequest->command == MEM_TEST_ERROR)
  {
    vme_seterror(MEM_TEST_ERROR,"vme_client: addr space unavail: ");
  }

I 5

  /*****************/
  /* transfer data */
  /*****************/

E 5
  /* write data to server for a VME_DOWNLOAD */
  if(clientRequest->command == SERVER_READING)
  {
D 5
	j = write(sFd, (char *)(clientRequest->data), (clientRequest->size * clientRequest->length));
E 5
I 5
	j = write(sFd, (char *)(clientRequest->data),
                                  (clientRequest->size*clientRequest->length));
E 5
	j = read(sFd, (char *)clientRequest, sizeof (struct request));
    if(clientRequest->command == NETWORK_CHECKSUM_ERROR)
	   vme_seterror(NETWORK_CHECKSUM_ERROR,"vme_client: vme_server reports: ");
E 3
	clientRequest->command = VME_DOWNLOAD;
D 3
    }
    /* receive data from server if doing a VME_UPLOAD */
    if (clientRequest->command == SERVER_WRITING) {
/* 	if ((j = read (sFd, (char *)(clientRequest->data), (clientRequest->length * clientRequest->size))) == ERROR) { */
/* 	    perror("read"); */
/* 	    close (sFd); */
/* 	    return (ERROR); */
/* 	} */
	for (i = 0;i < clientRequest->length * clientRequest->size;i++) {
	    read (sFd, (char *)(&recBuf),1);
	    clientRequest->data[i] = recBuf;
	}
E 3
I 3
  }
E 3

D 3
 	clientRequest->command = VME_UPLOAD;
E 3
I 3
  /* receive data from server if doing a VME_UPLOAD */
  if(clientRequest->command == SERVER_WRITING)
  {
D 5
    for(i = 0; i < clientRequest->length * clientRequest->size; i++)
E 5
I 5
    for(i=0; i<clientRequest->length*clientRequest->size; i++)
E 5
    {
D 5
      read (sFd, (char *)(&recBuf),1);
E 5
I 5
      read(sFd, (char *)(&recBuf),1);
E 5
      clientRequest->data[i] = recBuf;
E 3
    }

D 3
    returnVal=0; 
E 3
I 3
    clientRequest->command = VME_UPLOAD;
  }

D 5
  returnVal=0; 
E 5
I 5
  returnVal = 0;
E 5
E 3
    
D 3
    /* close all open files before exiting */ 
    close(sFd); 
E 3
I 3
  /* close all open files before exiting */ 
  close(sFd); 
E 3

D 3
    return(vme_checkerror()); 
E 3
I 3
  return(vme_checkerror()); 
E 3
}

I 6
#endif
E 6

I 6

E 6

E 1
