
#ifdef VXWORKS

/* vme_server.c - TCP server code that runs on VxWorks */

/* use VxWorks header files */ 
#include <stdio.h>
#include <string.h>

#include <vxWorks.h> 
#include <taskLib.h>
#include <sockLib.h> 
#include <fioLib.h>
#include <in.h>
#include <inetLib.h> 
#include <vxLib.h>
#include <semLib.h>
#include <sysLib.h>
#include <ioLib.h>
#include <usrLib.h>

#include "vme_tcpInfo.h"
#include "vme_server.h"
#include "vme_error.h"
#include "vme_download.h"
#include "level1.h"

/*#define DEBUG 1*/

/* downloading status variable */
static int tigris_download_status; 

SEM_ID trigSem;
static struct request clientRequest;
static unsigned short data[0x10000];
static unsigned short bigdata[BIGDATA];

/* global offset */
static unsigned long offset;

void
get_tigris_status()
{
  /*printf("tigris_download_status = %d\n",tigris_download_status);*/
  printf("%d\n",tigris_download_status);
  return;
}


void
init_tigris_status()
{
  tigris_download_status = 0;
  printf("set tigris_download_status to %d\n",tigris_download_status);
  return;
}


STATUS
vme_server(void)
{
  struct sockaddr_in myAddr;
  struct sockaddr_in clientAddr;
  int sFd; 
  int newFd; 
  int sockAddrSize = sizeof (struct sockaddr_in);

  int i,j;
  char inetAddr[INET_ADDR_LEN]; 
  volatile unsigned short *mem;
  unsigned short checksum_received;
  char sendBuf, *chPtr;
  unsigned short test_val;

  /* set up local address */
  bzero ((char *) &myAddr, sockAddrSize);
  myAddr.sin_family = PF_INET;
  myAddr.sin_port = htons (SERVER_PORT_NUM);
  myAddr.sin_addr.s_addr = htonl (INADDR_ANY);

  /* create socket */ 
  if((sFd = socket (PF_INET, SOCK_STREAM, 0)) == ERROR)
  {
    perror ("socket");
    return (ERROR); 
  } 

  /* bind socket to local address */ 
  if(bind (sFd, (struct sockaddr *) &myAddr, sockAddrSize) == ERROR)
  { 
    perror ("bind"); 
    close (sFd); 
    return (ERROR); 
  } 

  /* create queue for client connection requests */ 
  if (listen (sFd, MAX_QUEUED_CONNECTIONS) == ERROR)
  { 
    perror ("listen"); 
    close (sFd); 
    return (ERROR); 
  } 

  sysBusToLocalAdrs(0x39,0,&offset);
  printf("vme_server: global address offset is 0x%08x\n",offset);

  /* accept net connect requests */   
  FOREVER
  {

#ifdef DEBUG
    printf ("FOREVER loop is reached, value=%d\n",tigris_download_status);
#endif

    if( (newFd = accept (sFd,(struct sockaddr *) &clientAddr, &sockAddrSize))
        == ERROR )
    { 
      perror ("accept");
      close (sFd);
      tigris_download_status = 5;
#ifdef DEBUG
        printf ("return from FOREVER loop, value=%d\n",tigris_download_status);
#endif
      return (ERROR);
    }
#ifdef DEBUG
    printf ("newFd accepted, value=%d\n",tigris_download_status);
#endif

    /* block other vme access to boards while doing a read or write */
    taskLock();
    semTake (trigSem, WAIT_FOREVER);
	
    fioRead(newFd,(char *)&clientRequest, sizeof(struct request));

    /* convert Internet address to dot notation */ 
    inet_ntoa_b(clientAddr.sin_addr, inetAddr); 
	    

    /* DO TRANSACTION */

    if(clientRequest.command == BIGDATA_DOWNLOAD)
    {
      printf("BIGDATA_DOWNLOAD transfer\n");
      /* let client know that server is ready for data */
      clientRequest.command = SERVER_READING;
      printf("Write to %x ok ...\n",(int)bigdata);
      if(write(newFd, (char *)(&clientRequest), sizeof (struct request))
            == ERROR)
      {
        perror ("write");
        close (newFd);
        tigris_download_status = 15;
#ifdef DEBUG
        printf ("return from FOREVER loop, value=%d\n",tigris_download_status);
#endif
        return(ERROR);
      }

      /* read data from client */
      j = fioRead(newFd, (char *)(bigdata),
                    (clientRequest.length * clientRequest.size));

      /* caculate checksum */
      checksum_received = 0;
      for(i=0; i<clientRequest.length; i++)
      {
        checksum_received ^= bigdata[i];
      }
      if(checksum_received != clientRequest.checksum)
      {
        printf ("Checksum does not match data received.\n"); 
        clientRequest.command = NETWORK_CHECKSUM_ERROR;
        tigris_download_status = 1;
      }

      /* hardware download */
      if(level1_download(bigdata) != 0)
      {
        printf("vme_server: ERROR in level1_download()\n");
      }
      else
      {
        printf("vme_server: level1_download() passed\n");
      }

      /* report any checksum errors back to the client */
      /* sending that command means that level1 download is completed */
      write(newFd, (char *)(&clientRequest), sizeof (struct request));
    }
    else if(clientRequest.command == BIGDATA_UPLOAD)
    {
      printf("BIGDATA_UPLOAD transfer\n");

      /* hardware upload */
      if(level1_upload(bigdata) != 0)
      {
        printf("vme_server: ERROR in level1_upload()\n");
      }
      else
      {
        printf("vme_server: level1_upload() passed\n");
      }

      printf("0x%08x===>0x%04x\n",&bigdata[0x000000],bigdata[0x000000]);
      printf("0x%08x===>0x%04x\n",&bigdata[0x000001],bigdata[0x000001]);
      printf("0x%08x===>0x%04x\n",&bigdata[0x080000/2],bigdata[0x080000/2]);
      printf("0x%08x===>0x%04x\n",&bigdata[0x080002/2],bigdata[0x080002/2]);

      /* calculate checksum to send to client */
      clientRequest.checksum = 0;
      for(i=0; i<clientRequest.length; i++)
      {
        clientRequest.checksum ^= bigdata[i];
      }
      printf("vme_server: checksum=%d\n",clientRequest.checksum);

      /* let client know that server is ready to send data */
      clientRequest.command = SERVER_WRITING;
      if((i = write(newFd, (char *)(&clientRequest),
                      sizeof(struct request))) == ERROR)
      {
        perror("write");
        tigris_download_status = 25;
        close(newFd);
#ifdef DEBUG
        printf("return from FOREVER loop, value=%d\n",tigris_download_status);
#endif
        return(ERROR);
      }
      printf("vme_server: sending %d bytes ...\n",
                 clientRequest.length*clientRequest.size);
      write(newFd, (char *)bigdata, clientRequest.length*clientRequest.size);
      printf("vme_server: done.\n");
    }
    else if(clientRequest.command == VME_DOWNLOAD)
    {
      /* check to make sure that this memory space is available */
      mem = (volatile unsigned short *) (offset + clientRequest.addr&0xFFFFFF);
      test_val = 0xa5a5;
      if((i=vxMemProbe((char*)(mem),VX_WRITE,clientRequest.size,
               (char*)(&test_val))) == ERROR)
      {
        /* there was an error to report it to the client and print it */
#ifdef DEBUG
        printf("Error writing value 0xa5a5 to address %x.\n", (int)mem);
#endif
        tigris_download_status = 10;
        clientRequest.command = MEM_TEST_ERROR;
      }
      else
      { 
        /* let client know that server is ready for data */
        clientRequest.command = SERVER_READING;
        printf("Write to %x ok..\n",(int)mem);
      }

      if( write(newFd, (char *)(&clientRequest), sizeof (struct request))
            == ERROR)
      {
        perror ("write");
        close (newFd);
        tigris_download_status = 15;
#ifdef DEBUG
        printf ("return from FOREVER loop, value=%d\n",tigris_download_status);
#endif
        return(ERROR);
      }

      if(clientRequest.command == SERVER_READING)
      {
        /* read data from client */
        j = fioRead(newFd, (char *)(data),
                      (clientRequest.length * clientRequest.size));

        /* caculate checksum */
        checksum_received = 0;
        for(i=0; i<clientRequest.length; i++)
        {
          mem[i] = data[i];
          checksum_received ^= mem[i]; /* calculate checksum from hardware */
        }

        if(checksum_received != clientRequest.checksum)
        {
          printf ("Checksum does not match data received.\n"); 
          clientRequest.command = NETWORK_CHECKSUM_ERROR;
          tigris_download_status = 1;
        }

        /* report any checksum errors back to the client */
        write(newFd, (char *)(&clientRequest), sizeof (struct request));
      }
    }
    else if(clientRequest.command == VME_UPLOAD)
    {
      /* check to make sure that this memory space is available */
      mem = (volatile unsigned short *) (offset + clientRequest.addr&0xFFFFFF);
      if(( i=vxMemProbe ((char*)(mem),VX_READ, sizeof(test_val),
               (char*)(&test_val))) == ERROR )
      {
        tigris_download_status = 20;
        printf ("Error reading from address %x.\n",(int) mem);
      }
      else
      {
        /* calculate checksum to send to client */
        clientRequest.checksum = 0;
        for(i=0; i<clientRequest.length; i++)
        {
          data[i] = mem[i];
          clientRequest.checksum ^= data[i];
        }

        /* let client know that server is ready to send data */
        clientRequest.command = SERVER_WRITING;
        if((i = write(newFd, (char *)(&clientRequest),
                        sizeof(struct request))) == ERROR)
        {
          perror ("write");
          tigris_download_status = 25;
          close (newFd);
#ifdef DEBUG
          printf("return from FOREVER loop, value=%d\n",tigris_download_status);
#endif
          return(ERROR);
        }

        /* send data to client */
        chPtr = (char *)data;
        for(i=0; i<clientRequest.length*clientRequest.size; i++)
        {
          sendBuf = chPtr[i];
          write(newFd, (char *)(&sendBuf), 1);
        }
      }
    }


#ifdef DEBUG
    printf ("End of FOREVER loop, value=%d\n",tigris_download_status);
#endif
    close(newFd);  
    taskUnlock ();

    semGive(trigSem); 

  } /* end FOREVER */
  close(sFd);

  return(OK);
} /* end myServer */



#else

/*
void
vme_server_dummy()
*/
int
main()
{
  return;
}

#endif
