
#ifdef VXWORKS

void
vme_client_dummy()
{
  return;
}

#else

/* vme_client.c - TCP client code that runs on UNIX host */ 
 
/* UNIX header files */ 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h> 

#include "vme_tcpInfo.h"
#include "vme_client.h"
#include "vme_error.h"
#include "vme_download.h"
 

int
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
vme_send(struct request *clientRequest) 
{
  int sFd; 
  int mlen; 
  struct request myRequest; 
  struct sockaddr_in serverSockAddr; 
  struct hostent *hostInfo; 
  unsigned char returnVal;
  int i,j;
  char sendBuf, recBuf, *look;
  char buf[100];

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

  /* write data to server for a VME_DOWNLOAD */
  if(clientRequest->command == SERVER_READING)
  {
	j = write(sFd, (char *)(clientRequest->data),
                                  (clientRequest->size*clientRequest->length));
	j = read(sFd, (char *)clientRequest, sizeof (struct request));
    if(clientRequest->command == NETWORK_CHECKSUM_ERROR)
	   vme_seterror(NETWORK_CHECKSUM_ERROR,"vme_client: vme_server reports: ");
	clientRequest->command = VME_DOWNLOAD;
  }

  /* receive data from server if doing a VME_UPLOAD */
  if(clientRequest->command == SERVER_WRITING)
  {
    for(i=0; i<clientRequest->length*clientRequest->size; i++)
    {
      read(sFd, (char *)(&recBuf),1);
      clientRequest->data[i] = recBuf;
    }

    clientRequest->command = VME_UPLOAD;
  }

  returnVal = 0;
    
  /* close all open files before exiting */ 
  close(sFd); 

  return(vme_checkerror()); 
}

#endif



