h64992
s 00007/00011/00361
d D 1.18 08/04/01 15:22:22 boiarino 19 18
c *** empty log message ***
e
s 00006/00007/00366
d D 1.17 08/04/01 14:49:06 boiarino 18 17
c *** empty log message ***
e
s 00000/00132/00373
d D 1.16 08/04/01 14:05:52 boiarino 17 16
c remove obsoleted functions
c 
e
s 00004/00000/00501
d D 1.15 07/10/12 13:09:21 boiarino 16 15
c *** empty log message ***
e
s 00023/00004/00478
d D 1.14 07/03/20 16:38:30 boiarino 15 14
c *** empty log message ***
e
s 00017/00012/00465
d D 1.13 04/02/03 20:56:03 boiarino 14 13
c *** empty log message ***
e
s 00109/00007/00368
d D 1.12 02/07/06 23:44:47 boiarino 13 12
c add BIGDATA_* staff
c 
e
s 00140/00126/00235
d D 1.11 02/06/29 18:05:40 boiarino 12 11
c cosmetic
c 
e
s 00004/00002/00357
d D 1.10 02/03/28 23:15:47 boiarino 11 10
c minor
c 
e
s 00019/00007/00340
d D 1.9 02/03/28 16:49:14 boiarino 10 9
c *** empty log message ***
e
s 00000/00002/00347
d D 1.8 01/10/25 12:36:46 boiarino 9 8
c printf("Write to %x ok..\n",(int)mem);
c not in #ifdef DEBUG ... #endif any more
c 
e
s 00002/00001/00347
d D 1.7 01/10/25 12:05:58 boiarino 8 7
c print just number instead of string in get_tigris_status()
c 
e
s 00007/00005/00341
d D 1.6 01/10/24 14:45:21 boiarino 7 6
c play around tigris_download_status again
c 
e
s 00002/00008/00344
d D 1.5 01/10/24 14:10:12 boiarino 6 5
c play around tigris_download_status
e
s 00027/00011/00325
d D 1.4 01/10/24 13:56:29 boiarino 5 4
c trying to make 'tigris_download_status' useable
c 
e
s 00266/00200/00070
d D 1.3 01/10/24 12:19:47 boiarino 4 3
c some printfs
c 
e
s 00023/00005/00247
d D 1.2 01/10/22 13:14:20 boiarino 3 1
c new version from /home/trigger/10-15-2001src
c 
e
s 00000/00000/00000
d R 1.2 01/10/22 12:58:18 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/vme_server.c
e
s 00252/00000/00000
d D 1.1 01/10/22 12:58:17 boiarino 1 0
c date and time created 01/10/22 12:58:17 by boiarino
e
u
U
f e 0
t
T
I 10

I 15
#ifdef VXWORKS

E 15
E 10
I 1
/* vme_server.c - TCP server code that runs on VxWorks */

/* use VxWorks header files */ 
D 14
#include <vxWorks.h> 
E 14
#include <stdio.h>
I 14
#include <string.h>

#include <vxWorks.h> 
E 14
#include <taskLib.h>
#include <sockLib.h> 
#include <fioLib.h>
D 13
#include <in.h> 
E 13
I 13
#include <in.h>
E 13
#include <inetLib.h> 
D 14
#include <string.h>
E 14
#include <vxLib.h>
#include <semLib.h>
D 12
#include <sysLib.h> //change 8
#include <ioLib.h>  //change 9
#include <usrLib.h> //change 16
E 12
I 12
#include <sysLib.h>
#include <ioLib.h>
#include <usrLib.h>
I 14

E 14
E 12
#include "vme_tcpInfo.h"
#include "vme_server.h"
#include "vme_error.h"
I 13
#include "vme_download.h"
#include "level1.h"
E 13

I 10
D 11
int sysBusToLocalAdrs();

E 11
E 10
D 4
#define DEBUG
E 4
I 4
D 7
#define DEBUG 1
E 7
I 7
/*#define DEBUG 1*/
E 7
E 4

I 14
D 18
int sysBusToLocalAdrs();
E 14
I 11
/* in sysLib.h (!!!???)
extern STATUS sysBusToLocalAdrs(int adrsSpace, char *busAdrs, char **pLocalAdrs);
*/

E 18
E 11
I 3
D 5
//Global variable 
E 5
I 5
D 7
/* downloading status variable */ 
E 5
int tigris_download_status; 
E 7
I 7
/* downloading status variable */
static int tigris_download_status; 
E 7

E 3
SEM_ID trigSem;
D 13
static struct request clientRequest; 
E 13
I 13
static struct request clientRequest;
E 13
static unsigned short data[0x10000];
I 13
static unsigned short bigdata[BIGDATA];
E 13

I 18
/* global offset */
static unsigned long offset;
E 18
I 5

void
get_tigris_status()
{
D 7
  printf("%d\n",tigris_download_status);
E 7
I 7
D 8
  printf("tigris_download_status = %d\n",tigris_download_status);
E 8
I 8
  /*printf("tigris_download_status = %d\n",tigris_download_status);*/
  printf("%d\n",tigris_download_status);
E 8
  return;
E 7
}


void
init_tigris_status()
{
D 6
  tigris_download_status = -1;
E 6
I 6
  tigris_download_status = 0;
E 6
D 7
  printf("%d\n",tigris_download_status);
E 7
I 7
  printf("set tigris_download_status to %d\n",tigris_download_status);
  return;
E 7
}


E 5
D 4
STATUS mem_set (void) {
E 4
I 4
STATUS
D 12
mem_set(void)
{
E 4
  unsigned short *mem, test_val = 0xa5a5;
  int i;
I 10

  unsigned long ppc_offset;
  sysBusToLocalAdrs(0x39,0,&ppc_offset);
E 10
  
D 4
  for (i=0;i<7;i++) {
E 4
I 4
  for(i=0;i<7;i++)
  {
E 4
D 10
    mem = (unsigned short *)(0xe0000000 + i * 0x80000);
E 10
I 10
    mem = (unsigned short *)(ppc_offset + i * 0x80000);
E 10
D 4
    if ((vxMemProbe ((char*)(mem),VX_WRITE, sizeof(unsigned short), (char*)(&test_val))) != ERROR ){
E 4
I 4
    if((vxMemProbe ((char*)(mem),VX_WRITE, sizeof(unsigned short), (char*)(&test_val))) != ERROR )
    {
E 4
      memset((char *)mem, 0xff, 0x80000);
D 10
      mem = (unsigned short *)(0xe0000000 + i * 0x80000 + 0x60000);
E 10
I 10
      mem = (unsigned short *)(ppc_offset + i * 0x80000 + 0x60000);
E 10
      bzero ((char *)mem, 2);
D 4
      if (i==6) {
	mem = (unsigned short *)(0xe0320000);
	bzero ((char *)mem, 1);
	mem = (unsigned short *)(0xe0340000);
	bzero ((char *)mem, 1);
E 4
I 4
      if(i==6)
      {
D 10
        mem = (unsigned short *)(0xe0320000);
E 10
I 10
        mem = (unsigned short *)(ppc_offset + 0x320000);
E 10
        bzero ((char *)mem, 1);
D 10
        mem = (unsigned short *)(0xe0340000);
E 10
I 10
        mem = (unsigned short *)(ppc_offset + 0x340000);
E 10
        bzero ((char *)mem, 1);
E 4
      }
    }
  }
  return(OK);
}


D 4
STATUS mem_zero (void) {
    unsigned short *mem, test_val = 0xa5a5;
    int i;
E 4
I 4
STATUS
mem_zero(void)
{
  unsigned short *mem, test_val = 0xa5a5;
  int i;
E 4

I 10
  unsigned long ppc_offset;
  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  
E 10
D 4
    for (i=0;i<7;i++) {
	mem = (unsigned short *)(0xe0000000 + i * 0x80000);
	if ((vxMemProbe ((char*)(mem),VX_WRITE, sizeof(unsigned short), (char*)(&test_val))) !=ERROR )
	    bzero((char *)mem, 0x80000);
E 4
I 4
  for(i=0;i<7;i++)
  {
D 10
    mem = (unsigned short *)(0xe0000000 + i * 0x80000);
E 10
I 10
    mem = (unsigned short *)(ppc_offset + i * 0x80000);
E 10
	if((vxMemProbe ((char*)(mem),VX_WRITE, sizeof(unsigned short), (char*)(&test_val))) !=ERROR )
    {
      bzero((char *)mem, 0x80000);
E 4
    }
D 4
    return(OK);
E 4
I 4
  }

  return(OK);
E 4
}

D 4
STATUS sreg_d (void) { 
    int i; 
    unsigned short *mem;
    unsigned short sreg_val;
E 4

D 4
    /* accept net connect requests */   
    FOREVER { 	
	taskLock ();
  	semTake (trigSem, WAIT_FOREVER); 
	for (i=0;i<6;i++) {
	    mem = (unsigned short *)(0xe0070000 + i * 0x80000);
	    if (vxMemProbe ((char*)(mem),VX_READ, sizeof(sreg_val), (char*)(&sreg_val)) != ERROR) {
		if (sreg_val != 0) {
		    printf ("WARNING: S-reg on router %i = %04x\n", i+1, sreg_val);
		    mem[0] = 0;
		} /* if s_reg */
	    } /* if vxMemProbe */
	} /* for i */
	mem = (unsigned short *)(0xe0360000);
	if (vxMemProbe ((char*)(mem),VX_READ, sizeof(sreg_val), (char*)(&sreg_val)) != ERROR) {
	    if (sreg_val != 0) {
		printf ("WARNING: S-reg on event processor = %04x\n", sreg_val);
		mem[0] = 0;
	    } /* if sreg_val */
	} /* if vxMemProbe */
 	semGive (trigSem);
	taskUnlock ();
	taskDelay (sysClkRateGet () / 2);
    } /* end FOREVER */
    return(OK);
E 4
I 4
STATUS
sreg_d(void)
{ 
  int i; 
  unsigned short *mem;
  unsigned short sreg_val;

I 10
  unsigned long ppc_offset;
  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  
E 10
  /* accept net connect requests */   
  FOREVER
  {
    taskLock();
    semTake(trigSem, WAIT_FOREVER); 
    for(i=0;i<6;i++)
    {
D 10
      mem = (unsigned short *)(0xe0070000 + i * 0x80000);
E 10
I 10
      mem = (unsigned short *)(ppc_offset + 0x070000 + i * 0x80000);
E 10
      if(vxMemProbe ((char*)(mem),VX_READ, sizeof(sreg_val), (char*)(&sreg_val)) != ERROR)
      {
        if(sreg_val != 0)
        {
          printf ("WARNING: S-reg on router %i = %04x\n", i+1, sreg_val);
		  mem[0] = 0;
	    } /* if s_reg */
      } /* if vxMemProbe */
    } /* for i */
D 10
    mem = (unsigned short *)(0xe0360000);
E 10
I 10
    mem = (unsigned short *)(ppc_offset + 0x360000);
E 10
    if(vxMemProbe ((char*)(mem),VX_READ, sizeof(sreg_val), (char*)(&sreg_val)) != ERROR)
    {
      if(sreg_val != 0)
      {
        printf ("WARNING: S-reg on event processor = %04x\n", sreg_val);
        mem[0] = 0;
      } /* if sreg_val */
    } /* if vxMemProbe */
    semGive (trigSem);
    taskUnlock ();
    taskDelay (sysClkRateGet () / 2);
  } /* end FOREVER */

  return(OK);
E 4
} /* end myServer */

D 4
STATUS vme_server (void) { 
    struct sockaddr_in myAddr; 
    struct sockaddr_in clientAddr; 
    int sFd; 
    int newFd; 
    int sockAddrSize = sizeof (struct sockaddr_in); 
     //change 15
    
    int i,j; //change 14 
    char inetAddr[INET_ADDR_LEN]; 
    unsigned short *mem;
    unsigned short checksum_received; //change 13
    char sendBuf, *chPtr;   //change 12
    unsigned short test_val;
    
    /* set up local address */ 
    bzero ((char *) &myAddr, sockAddrSize); //change 10
    myAddr.sin_family = PF_INET; 
    myAddr.sin_port = htons (SERVER_PORT_NUM); 
    myAddr.sin_addr.s_addr = htonl (INADDR_ANY);  
    
    /* create socket */ 
    if ((sFd = socket (PF_INET, SOCK_STREAM, 0)) == ERROR) { 
D 3
	perror ("socket"); 
E 3
I 3
	perror ("socket");
E 3
	return (ERROR); 
    } 
    
    /* bind socket to local address */ 
    if (bind (sFd, (struct sockaddr *) &myAddr, sockAddrSize) == ERROR) { 
	perror ("bind"); 
	close (sFd); 
	return (ERROR); 
    } 
    
    /* create queue for client connection requests */ 
    if (listen (sFd, MAX_QUEUED_CONNECTIONS) == ERROR) { 
	perror ("listen"); 
	close (sFd); 
	return (ERROR); 
    } 
    
    
    /* accept net connect requests */   
    FOREVER {	//change 11
I 3
      tigris_download_status = -1;
E 4

E 3
D 4
	if ((newFd = accept (sFd,(struct sockaddr *) &clientAddr, &sockAddrSize)) == ERROR) { 
	    perror ("accept");
	    close (sFd);
I 3
	    tigris_download_status = 5;
E 3
	    return (ERROR);
	} 
E 4
I 4
STATUS
E 12
vme_server(void)
{
  struct sockaddr_in myAddr;
  struct sockaddr_in clientAddr;
  int sFd; 
  int newFd; 
D 12
  int sockAddrSize = sizeof (struct sockaddr_in); 
  //change 15
E 12
I 12
  int sockAddrSize = sizeof (struct sockaddr_in);
E 12
E 4

D 4
	/* block other vme access to boards while doing a read or write */
	taskLock();
	semTake (trigSem, WAIT_FOREVER);
E 4
I 4
D 12
  int i,j; //change 14 
E 12
I 12
  int i,j;
E 12
  char inetAddr[INET_ADDR_LEN]; 
D 19
  unsigned short *mem;
E 19
I 19
  volatile unsigned short *mem;
E 19
D 12
  unsigned short checksum_received; //change 13
  char sendBuf, *chPtr;   //change 12
E 12
I 12
  unsigned short checksum_received;
  char sendBuf, *chPtr;
E 12
  unsigned short test_val;

D 13
  /* set up local address */ 
E 13
I 13
  /* set up local address */
E 13
D 12
  bzero ((char *) &myAddr, sockAddrSize); //change 10
E 12
I 12
  bzero ((char *) &myAddr, sockAddrSize);
E 12
D 13
  myAddr.sin_family = PF_INET; 
  myAddr.sin_port = htons (SERVER_PORT_NUM); 
  myAddr.sin_addr.s_addr = htonl (INADDR_ANY);  
E 13
I 13
  myAddr.sin_family = PF_INET;
  myAddr.sin_port = htons (SERVER_PORT_NUM);
  myAddr.sin_addr.s_addr = htonl (INADDR_ANY);
E 13

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

I 18
  sysBusToLocalAdrs(0x39,0,&offset);
  printf("vme_server: global address offset is 0x%08x\n",offset);
E 18

  /* accept net connect requests */   
  FOREVER
D 12
  { //change 11
E 12
I 12
  {
E 12
D 5
    tigris_download_status = -1;
E 5
I 5

E 5
#ifdef DEBUG
D 5
        printf ("\nFOREVER loop is reached, address 0x%08x, value=%d\n",
           (int)&tigris_download_status,tigris_download_status);
E 5
I 5
    printf ("FOREVER loop is reached, value=%d\n",tigris_download_status);
E 5
#endif

D 12
    if((newFd = accept (sFd,(struct sockaddr *) &clientAddr, &sockAddrSize)) == ERROR)
E 12
I 12
    if( (newFd = accept (sFd,(struct sockaddr *) &clientAddr, &sockAddrSize))
        == ERROR )
E 12
    { 
      perror ("accept");
      close (sFd);
      tigris_download_status = 5;
#ifdef DEBUG
D 5
        printf ("\nreturn from FOREVER loop, value=%d\n",tigris_download_status);
E 5
I 5
        printf ("return from FOREVER loop, value=%d\n",tigris_download_status);
E 5
#endif
      return (ERROR);
    }
I 5
D 6
    tigris_download_status = -1;
E 6
E 5
#ifdef DEBUG
D 5
        printf ("\nnewFd accepted, value=%d\n",tigris_download_status);
E 5
I 5
    printf ("newFd accepted, value=%d\n",tigris_download_status);
E 5
#endif

    /* block other vme access to boards while doing a read or write */
    taskLock();
    semTake (trigSem, WAIT_FOREVER);
E 4
	
D 4
	fioRead(newFd,(char *)&clientRequest, sizeof(struct request));
	    /* convert Internet address to dot notation */ 
	    inet_ntoa_b (clientAddr.sin_addr, inetAddr); 
E 4
I 4
    fioRead(newFd,(char *)&clientRequest, sizeof(struct request));

    /* convert Internet address to dot notation */ 
D 12
    inet_ntoa_b (clientAddr.sin_addr, inetAddr); 
E 12
I 12
    inet_ntoa_b(clientAddr.sin_addr, inetAddr); 
E 12
E 4
	    
D 4
	    if (clientRequest.command == VME_DOWNLOAD) { 
		
		/* check to make sure that this memory space is available */
		mem=(unsigned short *)clientRequest.addr;  
		test_val = 0xa5a5; //change 12
  		if (( i=vxMemProbe ((char*)(mem),VX_WRITE, clientRequest.size, (char*)(&test_val))) == ERROR ){
		    /* there was an error to report it to the client and write it to the console */
E 4
I 4
D 13
    if(clientRequest.command == VME_DOWNLOAD)
E 13
I 13

    /* DO TRANSACTION */

    if(clientRequest.command == BIGDATA_DOWNLOAD)
E 13
    {
I 13
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
E 13
      /* check to make sure that this memory space is available */
D 18
      mem=(unsigned short *)clientRequest.addr;  
E 18
I 18
D 19
      mem = (unsigned short *) (offset + clientRequest.addr&0xFFFFFF);  
E 19
I 19
      mem = (volatile unsigned short *) (offset + clientRequest.addr&0xFFFFFF);
E 19
E 18
D 12
      test_val = 0xa5a5; //change 12
      if(( i=vxMemProbe ((char*)(mem),VX_WRITE, clientRequest.size, (char*)(&test_val))) == ERROR )
E 12
I 12
      test_val = 0xa5a5;
      if((i=vxMemProbe((char*)(mem),VX_WRITE,clientRequest.size,
               (char*)(&test_val))) == ERROR)
E 12
      {
D 12
        /* there was an error to report it to the client and write it to the console */
E 12
I 12
        /* there was an error to report it to the client and print it */
E 12
E 4
#ifdef DEBUG
D 4
		  printf ("\nError writing value 0xa5a5 to address %x. \n", (int)mem); //change LAST
E 4
I 4
D 5
        printf ("\nError writing value 0xa5a5 to address %x. \n", (int)mem); //change LAST
E 5
I 5
D 12
        printf ("Error writing value 0xa5a5 to address %x. \n", (int)mem); //change LAST
E 12
I 12
        printf("Error writing value 0xa5a5 to address %x.\n", (int)mem);
E 12
E 5
E 4
#endif
I 3
D 4
		    tigris_download_status = 10;
E 3
		    clientRequest.command = MEM_TEST_ERROR;
		}
		else { 
		    /* let client know that server is ready for data */
		    clientRequest.command = SERVER_READING;
E 4
I 4
        tigris_download_status = 10;
        clientRequest.command = MEM_TEST_ERROR;
      }
      else
      { 
        /* let client know that server is ready for data */
        clientRequest.command = SERVER_READING;
E 4
D 9
#ifdef DEBUG
E 9
D 4
		    printf("Write to %x ok..\n",(int)mem);
E 4
I 4
        printf("Write to %x ok..\n",(int)mem);
E 4
D 9
#endif
E 9
D 4
		}
		
		if (write(newFd, (char *)(&clientRequest), sizeof (struct request)) == ERROR) {
		    perror ("write");
		    close (newFd);
I 3
		    tigris_download_status = 15;
E 3
		    return(ERROR);
E 4
I 4
      }
E 4
I 3

E 3
D 4
		}
		
		if (clientRequest.command == SERVER_READING) {
		    /* read data from client */
		    j = fioRead(newFd, (char *)(data), (clientRequest.length * clientRequest.size));
		    
		    /* caculate checksum */
		    checksum_received = 0;
		    for (i=0;i<clientRequest.length;i++) {
			mem[i] = data[i];
			/*	checksum_received ^= data[i];*/
			checksum_received ^= mem[i];
		    }
		    
		    if (checksum_received != clientRequest.checksum) {
			printf ("Checksum does not match data received.\n"); 
			clientRequest.command = NETWORK_CHECKSUM_ERROR;
I 3
			tigris_download_status = 1;
E 3
		    }
E 4
I 4
D 12
      if(write(newFd, (char *)(&clientRequest), sizeof (struct request)) == ERROR)
E 12
I 12
      if( write(newFd, (char *)(&clientRequest), sizeof (struct request))
            == ERROR)
E 12
      {
        perror ("write");
        close (newFd);
        tigris_download_status = 15;
#ifdef DEBUG
D 5
        printf ("\nreturn from FOREVER loop, value=%d\n",tigris_download_status);
E 5
I 5
        printf ("return from FOREVER loop, value=%d\n",tigris_download_status);
E 5
#endif
        return(ERROR);
      }
E 4

D 4
		    /* report any checksum errors back to the client */
		    write(newFd, (char *)(&clientRequest), sizeof (struct request));
E 4
I 4
      if(clientRequest.command == SERVER_READING)
      {
        /* read data from client */
D 12
        j = fioRead(newFd, (char *)(data), (clientRequest.length * clientRequest.size));
E 12
I 12
        j = fioRead(newFd, (char *)(data),
                      (clientRequest.length * clientRequest.size));
E 12
E 4

D 4
		    /* calculate checksum from hardware */
		    
E 4
I 4
        /* caculate checksum */
        checksum_received = 0;
D 19
        for(i=0;i<clientRequest.length;i++)
E 19
I 19
        for(i=0; i<clientRequest.length; i++)
E 19
        {
          mem[i] = data[i];
E 4
D 19

D 4
		}
	    } else  
		
		if (clientRequest.command == VME_UPLOAD) {
		    
		    /* check to make sure that this memory space is available */
		  mem=(unsigned short *)clientRequest.addr;  
D 3
		    if (( i=vxMemProbe ((char*)(mem),VX_READ, sizeof(test_val), (char*)(&test_val))) == ERROR )  
		      printf ("Error reading from address %x.\n",(int) mem); //change LAST
E 3
I 3
		    if (( i=vxMemProbe ((char*)(mem),VX_READ, sizeof(test_val), (char*)(&test_val))) == ERROR )
		      {
			tigris_download_status = 20;
			printf ("Error reading from address %x.\n",(int) mem); //change LAST
		      }
E 3
		    else {
			
			/* calculate checksum to send to client */
			clientRequest.checksum = 0;
			for (i=0;i<clientRequest.length;i++) 
			    clientRequest.checksum ^= data[i] = mem[i];
			
			/* let client know that server is ready to send data */
			clientRequest.command = SERVER_WRITING;
			if ((i = write(newFd, (char *)(&clientRequest), sizeof(struct request))) == ERROR) {
			    perror ("write");
I 3
			    tigris_download_status = 25;
E 3
			    close (newFd);
			    return(ERROR);
			}
E 4
I 4
          /* checksum_received ^= data[i];*/
          checksum_received ^= mem[i];
E 19
I 19
          checksum_received ^= mem[i]; /* calculate checksum from hardware */
E 19
        }
E 4

D 4
			/* send data to client */
			chPtr = (char *)data;
			for (i = 0;i<clientRequest.length*clientRequest.size;i++) {
			    sendBuf = chPtr[i];
			    write(newFd, (char *)(&sendBuf),1);
			}
		    }
		}
E 4
I 4
        if(checksum_received != clientRequest.checksum)
        {
          printf ("Checksum does not match data received.\n"); 
          clientRequest.command = NETWORK_CHECKSUM_ERROR;
          tigris_download_status = 1;
        }
E 4
D 3
	close (newFd);  
	taskUnlock ();
E 3
I 3

D 4
	    if (tigris_download_status == -1)
	      tigris_download_status = 0;
	    close (newFd);  
	    taskUnlock ();
E 4
I 4
        /* report any checksum errors back to the client */
        write(newFd, (char *)(&clientRequest), sizeof (struct request));
E 4
D 19

E 3
D 4
 	semGive (trigSem); 
    } /* end FOREVER */
    close (sFd);
    return(OK);
E 4
I 4
        /* calculate checksum from hardware */

E 19
      }
    }
    else if(clientRequest.command == VME_UPLOAD)
    {
      /* check to make sure that this memory space is available */
D 18
      mem=(unsigned short *)clientRequest.addr;  
E 18
I 18
D 19
      mem = (unsigned short *) (offset + clientRequest.addr&0xFFFFFF);  
E 19
I 19
      mem = (volatile unsigned short *) (offset + clientRequest.addr&0xFFFFFF);
E 19
E 18
D 12
      if(( i=vxMemProbe ((char*)(mem),VX_READ, sizeof(test_val), (char*)(&test_val))) == ERROR )
E 12
I 12
      if(( i=vxMemProbe ((char*)(mem),VX_READ, sizeof(test_val),
               (char*)(&test_val))) == ERROR )
E 12
      {
        tigris_download_status = 20;
D 12
        printf ("Error reading from address %x.\n",(int) mem); //change LAST
E 12
I 12
        printf ("Error reading from address %x.\n",(int) mem);
E 12
      }
      else
      {
        /* calculate checksum to send to client */
        clientRequest.checksum = 0;
D 12
        for(i=0;i<clientRequest.length;i++) clientRequest.checksum ^= data[i] = mem[i];
E 12
I 12
        for(i=0; i<clientRequest.length; i++)
        {
D 19
          clientRequest.checksum ^= data[i] = mem[i];
E 19
I 19
          data[i] = mem[i];
          clientRequest.checksum ^= data[i];
E 19
        }
E 12

        /* let client know that server is ready to send data */
        clientRequest.command = SERVER_WRITING;
D 12
        if((i = write(newFd, (char *)(&clientRequest), sizeof(struct request))) == ERROR)
E 12
I 12
        if((i = write(newFd, (char *)(&clientRequest),
                        sizeof(struct request))) == ERROR)
E 12
        {
          perror ("write");
          tigris_download_status = 25;
          close (newFd);
#ifdef DEBUG
D 5
        printf ("\nreturn from FOREVER loop, value=%d\n",tigris_download_status);
E 5
I 5
D 12
        printf ("return from FOREVER loop, value=%d\n",tigris_download_status);
E 12
I 12
          printf("return from FOREVER loop, value=%d\n",tigris_download_status);
E 12
E 5
#endif
          return(ERROR);
        }

        /* send data to client */
        chPtr = (char *)data;
D 12
        for(i = 0;i<clientRequest.length*clientRequest.size;i++)
E 12
I 12
        for(i=0; i<clientRequest.length*clientRequest.size; i++)
E 12
        {
          sendBuf = chPtr[i];
D 12
          write(newFd, (char *)(&sendBuf),1);
E 12
I 12
          write(newFd, (char *)(&sendBuf), 1);
E 12
        }
      }
    }
I 13


E 13
D 6

E 6
#ifdef DEBUG
D 5
        printf ("\nEnd of FOREVER loop (1), value=%d\n",tigris_download_status);
E 5
I 5
D 6
        printf ("End of FOREVER loop (1), value=%d\n",tigris_download_status);
E 6
I 6
    printf ("End of FOREVER loop, value=%d\n",tigris_download_status);
E 6
E 5
#endif
D 6
    if(tigris_download_status == -1) tigris_download_status = 0;
#ifdef DEBUG
D 5
        printf ("\nEnd of FOREVER loop (2), value=%d\n",tigris_download_status);
E 5
I 5
        printf ("End of FOREVER loop (2), value=%d\n",tigris_download_status);
E 5
#endif
E 6
D 12
    close (newFd);  
E 12
I 12
    close(newFd);  
E 12
    taskUnlock ();

D 12
    semGive (trigSem); 
E 12
I 12
    semGive(trigSem); 
E 12

  } /* end FOREVER */
D 12
  close (sFd);
E 12
I 12
  close(sFd);
E 12

  return(OK);
E 4
} /* end myServer */

D 4
STATUS trig_sem_init (void) {
    trigSem = semBCreate (SEM_Q_FIFO, SEM_FULL);
E 4

I 12

D 17





/********************************************************/
/* ????????????????????? OBSOLETE ????????????????????? */


E 12
D 4
    sp (vme_server,0,0,0,0,0,0,0,0,0);
    sp (sreg_d,0,0,0,0,0,0,0,0,0);
    return(OK);
E 4
I 4
STATUS
I 12
mem_set(void)
{
D 14
  unsigned short *mem, test_val = 0xa5a5;
E 14
I 14
  volatile unsigned short *mem;
  unsigned short test_val = 0xa5a5;
E 14
  int i;

  unsigned long ppc_offset;
  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  
  for(i=0;i<7;i++)
  {
D 14
    mem = (unsigned short *)(ppc_offset + i * 0x80000);
E 14
I 14
    mem = (volatile unsigned short *)(ppc_offset + i * 0x80000);
E 14
    if((vxMemProbe ((char*)(mem),VX_WRITE, sizeof(unsigned short), (char*)(&test_val))) != ERROR )
    {
      memset((char *)mem, 0xff, 0x80000);
D 14
      mem = (unsigned short *)(ppc_offset + i * 0x80000 + 0x60000);
E 14
I 14
      mem = (volatile unsigned short *)(ppc_offset + i * 0x80000 + 0x60000);
E 14
      bzero ((char *)mem, 2);
      if(i==6)
      {
D 14
        mem = (unsigned short *)(ppc_offset + 0x320000);
E 14
I 14
        mem = (volatile unsigned short *)(ppc_offset + 0x320000);
E 14
        bzero ((char *)mem, 1);
D 14
        mem = (unsigned short *)(ppc_offset + 0x340000);
E 14
I 14
        mem = (volatile unsigned short *)(ppc_offset + 0x340000);
E 14
        bzero ((char *)mem, 1);
      }
    }
  }
  return(OK);
}

STATUS
mem_zero(void)
{
D 14
  unsigned short *mem, test_val = 0xa5a5;
E 14
I 14
  unsigned short *mem;
  unsigned short test_val = 0xa5a5;
E 14
  int i;

  unsigned long ppc_offset;
  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  
  for(i=0;i<7;i++)
  {
D 14
    mem = (unsigned short *)(ppc_offset + i * 0x80000);
E 14
I 14
    mem = (volatile unsigned short *)(ppc_offset + i * 0x80000);
E 14
	if((vxMemProbe ((char*)(mem),VX_WRITE, sizeof(unsigned short), (char*)(&test_val))) !=ERROR )
    {
      bzero((char *)mem, 0x80000);
    }
  }

  return(OK);
}

I 15



E 15
STATUS
D 15
sreg_d(void)
E 15
I 15
sreg_d_1(void)
E 15
{ 
  int i; 
D 14
  unsigned short *mem;
E 14
I 14
  volatile unsigned short *mem;
E 14
  unsigned short sreg_val;

  unsigned long ppc_offset;
  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  
  /* accept net connect requests */   
  FOREVER
  {
    taskLock();
    semTake(trigSem, WAIT_FOREVER); 
    for(i=0;i<6;i++)
    {
D 14
      mem = (unsigned short *)(ppc_offset + 0x070000 + i * 0x80000);
E 14
I 14
      mem = (volatile unsigned short *)(ppc_offset + 0x070000 + i * 0x80000);
E 14
      if(vxMemProbe ((char*)(mem),VX_READ, sizeof(sreg_val), (char*)(&sreg_val)) != ERROR)
      {
        if(sreg_val != 0)
        {
          printf ("WARNING: S-reg on router %i = %04x\n", i+1, sreg_val);
		  mem[0] = 0;
	    } /* if s_reg */
      } /* if vxMemProbe */
    } /* for i */
D 14
    mem = (unsigned short *)(ppc_offset + 0x360000);
E 14
I 14
    mem = (volatile unsigned short *)(ppc_offset + 0x360000);
E 14
    if(vxMemProbe ((char*)(mem),VX_READ, sizeof(sreg_val), (char*)(&sreg_val)) != ERROR)
    {
      if(sreg_val != 0)
      {
        printf ("WARNING: S-reg on event processor = %04x\n", sreg_val);
        mem[0] = 0;
      } /* if sreg_val */
    } /* if vxMemProbe */
    semGive (trigSem);
    taskUnlock ();
    taskDelay (sysClkRateGet () / 2);
  } /* end FOREVER */

  return(OK);
} /* end myServer */

STATUS
E 12
D 15
trig_sem_init(void)
E 15
I 15
trig_sem_init_1(void)
E 15
{
  trigSem = semBCreate (SEM_Q_FIFO, SEM_FULL);

  sp (vme_server,0,0,0,0,0,0,0,0,0);
D 15
  sp (sreg_d,0,0,0,0,0,0,0,0,0);
E 15
I 15
  sp (sreg_d_1,0,0,0,0,0,0,0,0,0);
E 15

  return(OK);
E 4
}

I 15




E 15
D 4
STATUS trig_init (void) {
    sp (mem_zero,0,0,0,0,0,0,0,0,0);
    sp (trig_sem_init,0,0,0,0,0,0,0,0,0);
    return(OK);
E 4
I 4
D 12

E 12
STATUS
trig_init(void)
{
  sp (mem_zero,0,0,0,0,0,0,0,0,0);
D 15
  sp (trig_sem_init,0,0,0,0,0,0,0,0,0);
E 15
I 15
  sp (trig_sem_init_1,0,0,0,0,0,0,0,0,0);
E 15

  return(OK);
E 4
}
I 15

E 17
#else

I 16
/*
E 16
void
vme_server_dummy()
I 16
*/
int
main()
E 16
{
  return;
}

#endif
E 15
E 1
