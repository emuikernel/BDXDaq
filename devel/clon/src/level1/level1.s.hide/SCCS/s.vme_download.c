h13972
s 00009/00013/00301
d D 1.8 08/04/01 14:48:54 boiarino 9 8
c *** empty log message ***
e
s 00014/00009/00300
d D 1.7 08/03/29 13:18:24 boiarino 8 7
c define LEVEL1BASE differently for different cpu boards; not sure if
c still need it ..
c 
e
s 00000/00002/00309
d D 1.6 07/03/20 16:41:28 boiarino 7 6
c *** empty log message ***
e
s 00010/00002/00301
d D 1.5 05/02/25 14:40:28 boiarino 6 5
c *** empty log message ***
e
s 00086/00021/00217
d D 1.4 02/07/06 23:45:09 boiarino 5 4
c add BIGDATA staff
c 
e
s 00158/00167/00080
d D 1.3 02/06/29 18:02:07 boiarino 4 3
c cosmetic
c 
e
s 00016/00009/00231
d D 1.2 02/03/28 16:49:28 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/10/22 12:58:18 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/vme_download.c
e
s 00240/00000/00000
d D 1.1 01/10/22 12:58:17 boiarino 1 0
c date and time created 01/10/22 12:58:17 by boiarino
e
u
U
f e 0
t
T
I 4

E 4
I 1
D 6
/* vme_download.c */
E 6
I 6
/* vme_download.c - run on UNIX */
E 6

I 6
D 7
#undef TEST_SETUP

E 7
#ifdef TEST_SETUP
#define NROUTERS 1
#else
#define NROUTERS 6
#endif

I 8
D 9
/* BADBADBAD !!!!!!!! */
/*#define LEVEL1BASE 0xfa000000*/ /*mv2400*/
#define LEVEL1BASE 0xeff00000 /*mv5100*/
E 9

D 9

E 9
E 8
E 6
D 5
/* UNIX header files */
E 5
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "vme_tcpInfo.h"
#include "vme_client.h"
#include "vme_error.h"
I 5
#include "vme_download.h"
#include "level1.h"
E 5

I 3
D 4
/*int sysBusToLocalAdrs();*/

E 3
int zero_routers(hostname)
    char *hostname;
E 4
I 4
void
zero_routers(char *hostname)
E 4
{
D 4
    struct request clientRequest;
    unsigned short data = 0;
    unsigned int base,i;
E 4
I 4
  struct request clientRequest;
  unsigned short data = 0;
  unsigned int base, i;
E 4

I 3
D 4
	/*
  unsigned long ppc_offset;
  sysBusToLocalAdrs(0x39,0,&ppc_offset);
	*/
E 4
I 4
  clientRequest.command = VME_DOWNLOAD;
  clientRequest.size = sizeof(unsigned short);
  clientRequest.length = 1;
  clientRequest.data = (char*) &data;
  clientRequest.hostname = hostname;
  clientRequest.checksum = 0;
E 4

E 3
D 4
    clientRequest.command = VME_DOWNLOAD;
    clientRequest.size = sizeof(unsigned short);
    clientRequest.length = 1;
    clientRequest.data = (char*) &data;  //change 2
    clientRequest.hostname = hostname;
    clientRequest.checksum = 0;
E 4
I 4
D 6
  for(i=0; i<6; i++)
E 6
I 6
  for(i=0; i<NROUTERS; i++)
E 6
  {
D 8
    base = 0xfa000000 + i * 0x80000;
E 8
I 8
D 9
    base = LEVEL1BASE + i * 0x80000;
E 9
I 9
    base = i * 0x80000;
E 9
E 8
    clientRequest.addr = base + 0x1fffe;
    vme_send(&clientRequest);
    clientRequest.addr = base + 0x3fffe;
    vme_send(&clientRequest);
    clientRequest.addr = base + 0x5fffe;
    vme_send(&clientRequest);
    router_init(i+1,hostname);
  }
  evt_proc_init(hostname);
E 4

D 4
    for(i=0;i<6;i++) {
D 3
	base = 0xe0000000 + i * 0x80000;
E 3
I 3
	base = 0xfa000000 + i * 0x80000;
E 3
	clientRequest.addr = base + 0x1fffe;
	vme_send(&clientRequest);
	clientRequest.addr = base + 0x3fffe;
	vme_send(&clientRequest);
	clientRequest.addr = base + 0x5fffe;
	vme_send(&clientRequest);
	router_init(i+1,hostname);
    }
    evt_proc_init(hostname);
E 4
I 4
  return;
E 4
}
D 4
    
E 4


D 4
int evt_proc_init(hostname)
    char *hostname;
E 4
I 4
D 5
int
E 5
I 5
void
E 5
evt_proc_init(char *hostname)
E 4
{
D 4
    struct request clientRequest;
    unsigned short data = 0;
    
    clientRequest.command = VME_DOWNLOAD;
    clientRequest.size = sizeof(unsigned short);
    clientRequest.length = 1;
    clientRequest.data = (char *) &data;   //change 3
    clientRequest.hostname = hostname;
    clientRequest.checksum = 0;
    
D 3
    clientRequest.addr = 0xe0320000; /* CREG 1 */
E 3
I 3
    clientRequest.addr = 0xfa320000; /* CREG 1 */
E 3
    vme_send(&clientRequest);
E 4
I 4
  struct request clientRequest;
  unsigned short data = 0;
E 4

D 3
    clientRequest.addr = 0xe0340000; /* CREG 2 */
E 3
I 3
D 4
    clientRequest.addr = 0xfa340000; /* CREG 2 */
E 3
    vme_send(&clientRequest);
E 4
I 4
  clientRequest.command = VME_DOWNLOAD;
  clientRequest.size = sizeof(unsigned short);
  clientRequest.length = 1;
  clientRequest.data = (char *) &data;
  clientRequest.hostname = hostname;
  clientRequest.checksum = 0;
E 4

D 3
    clientRequest.addr = 0xe0360000; /* SREG */
E 3
I 3
D 4
    clientRequest.addr = 0xfa360000; /* SREG */
E 3
    vme_send(&clientRequest);
E 4
I 4
D 8
  clientRequest.addr = 0xfa320000; /* CREG 1 */
E 8
I 8
D 9
  clientRequest.addr = LEVEL1BASE + 0x320000; /* CREG 1 */
E 9
I 9
  clientRequest.addr = 0x320000; /* CREG 1 */
E 9
E 8
  vme_send(&clientRequest);

D 8
  clientRequest.addr = 0xfa340000; /* CREG 2 */
E 8
I 8
D 9
  clientRequest.addr = LEVEL1BASE + 0x340000; /* CREG 2 */
E 9
I 9
  clientRequest.addr = 0x340000; /* CREG 2 */
E 9
E 8
  vme_send(&clientRequest);

D 8
  clientRequest.addr = 0xfa360000; /* SREG */
E 8
I 8
D 9
  clientRequest.addr = LEVEL1BASE + 0x360000; /* SREG */
E 9
I 9
  clientRequest.addr = 0x360000; /* SREG */
E 9
E 8
  vme_send(&clientRequest);
I 5

  return;
E 5
E 4
}

D 4
int router_init(router_id, hostname)
    int router_id;
    char *hostname;
E 4
I 4

D 5
int
E 5
I 5
void
E 5
router_init(int router_id, char *hostname)
E 4
{
D 4
    struct request clientRequest;
    unsigned short data = 0;
E 4
I 4
  struct request clientRequest;
  unsigned short data = 0;
E 4
    
D 4
    clientRequest.command = VME_DOWNLOAD;
    clientRequest.size = sizeof(unsigned short);
    clientRequest.length = 1;
    clientRequest.data = (char *) &data;   //change 4
    clientRequest.hostname = hostname;
    clientRequest.checksum = 0;
E 4
I 4
  clientRequest.command = VME_DOWNLOAD;
  clientRequest.size = sizeof(unsigned short);
  clientRequest.length = 1;
  clientRequest.data = (char *) &data;
  clientRequest.hostname = hostname;
  clientRequest.checksum = 0;
E 4

D 3
    clientRequest.addr = 0xe0060000 + ((router_id-1) * 0x80000); /* CREG */
E 3
I 3
D 4
    clientRequest.addr = 0xfa060000 + ((router_id-1) * 0x80000); /* CREG */
E 3
    vme_send(&clientRequest);
E 4
I 4
D 8
  clientRequest.addr = 0xfa060000 + ((router_id-1) * 0x80000); /* CREG */
E 8
I 8
D 9
  clientRequest.addr = LEVEL1BASE + 0x060000 + ((router_id-1) * 0x80000); /* CREG */
E 9
I 9
  clientRequest.addr = 0x060000 + ((router_id-1) * 0x80000); /* CREG */
E 9
E 8
  vme_send(&clientRequest);
E 4

D 3
    clientRequest.addr = 0xe0070000 + ((router_id-1) * 0x80000); /* SREG */
E 3
I 3
D 4
    clientRequest.addr = 0xfa070000 + ((router_id-1) * 0x80000); /* SREG */
E 3
    vme_send(&clientRequest);
E 4
I 4
D 8
  clientRequest.addr = 0xfa070000 + ((router_id-1) * 0x80000); /* SREG */
E 8
I 8
D 9
  clientRequest.addr = LEVEL1BASE + 0x070000 + ((router_id-1) * 0x80000); /* SREG */
E 9
I 9
  clientRequest.addr = 0x070000 + ((router_id-1) * 0x80000); /* SREG */
E 9
E 8
  vme_send(&clientRequest);
I 5

  return;
E 5
E 4
}

D 4
int vme_download(addr, data, hostname)
    unsigned int addr;
    unsigned short *data;
    char *hostname;
E 4
I 4

int
I 5
bigdata_download(unsigned short *data, char *hostname)
{
  struct request clientRequest;
  int i;

  /* set up request to send to server */
  clientRequest.command = BIGDATA_DOWNLOAD;
  clientRequest.addr = 0; /* arbitrary - not used by vme_server */
  clientRequest.size = sizeof (unsigned short);
  clientRequest.length = BIGDATA/2;
  clientRequest.data = (char *) data;
  clientRequest.hostname = hostname;
  /* calculate checksum to send to server */
  data = (unsigned short *)clientRequest.data;
  clientRequest.checksum = 0;
  for(i=0; i<clientRequest.length; i++)
  {
    clientRequest.checksum ^= data[i];
  }

  /* send data to server */
  return(vme_bigsend(&clientRequest));
}

int
bigdata_upload(unsigned short *data, char *hostname)
{
  struct request clientRequest;
  int i;
  unsigned short checksum;

printf("bigdata_upload reached\n");fflush(stdout);

  /* set up request to send to server */
  clientRequest.command = BIGDATA_UPLOAD;
  clientRequest.addr = 0; /* arbitrary - not used by vme_server */
  clientRequest.size = sizeof (unsigned short);
  clientRequest.length = BIGDATA/2;
  clientRequest.data = (char *) data;
  clientRequest.hostname = hostname;

  /* send request to server */
printf("1: len=%d\n",clientRequest.length);
  vme_bigsend(&clientRequest);
printf("2\n");

  /* calculate checksum of data recived */
  checksum = 0;
  for(i=0;i<clientRequest.length;i++) checksum ^= data[i];

  if(checksum != clientRequest.checksum)
  {
	printf("Checksum of data received does not match checksum from server.\n");
  }

  printf("bigdata_upload done.\n");
}


int
E 5
vme_download(unsigned int addr, unsigned short *data, char *hostname)
E 4
{
D 4
    struct request clientRequest;
    int i;
    /* set up request to send to server */
    clientRequest.command=VME_DOWNLOAD;
    clientRequest.addr=addr;
    clientRequest.size=sizeof (unsigned short);
    clientRequest.length = 0x10000;
    clientRequest.data = (char *) data;   //change 5
    clientRequest.hostname=hostname;
    /* calculate checksum to send to server */
    data = (unsigned short *)clientRequest.data;  //change 7
    clientRequest.checksum = 0;
    for(i=0;i<clientRequest.length;i++)
	clientRequest.checksum ^= data[i];
    /* send data to server */
    return(vme_send(&clientRequest));
E 4
I 4
  struct request clientRequest;
  int i;

  /* set up request to send to server */
D 5
  clientRequest.command=VME_DOWNLOAD;
  clientRequest.addr=addr;
  clientRequest.size=sizeof (unsigned short);
E 5
I 5
  clientRequest.command = VME_DOWNLOAD;
  clientRequest.addr = addr;
  clientRequest.size = sizeof (unsigned short);
E 5
  clientRequest.length = 0x10000;
  clientRequest.data = (char *) data;
D 5
  clientRequest.hostname=hostname;
E 5
I 5
  clientRequest.hostname = hostname;
E 5
  /* calculate checksum to send to server */
  data = (unsigned short *)clientRequest.data;
  clientRequest.checksum = 0;
  for(i=0; i<clientRequest.length; i++)
  {
    clientRequest.checksum ^= data[i];
  }

  /* send data to server */
  return(vme_send(&clientRequest));
E 4
}

D 4
int vme_upload(addr, data, hostname)
    unsigned int addr;
    unsigned short *data;
    char *hostname;
E 4
I 4

int
vme_upload(unsigned int addr, unsigned short *data, char *hostname)
E 4
{
D 4
    struct request clientRequest;
    int i;
    unsigned short checksum;
    
    /*  zero out data buffer
    for(i=0;i<clientRequest.length;i++)
	data[i] = 0;
    Fix removed */
E 4
I 4
  struct request clientRequest;
  int i;
  unsigned short checksum;
E 4

D 4
    /* set up request to send to server */
    clientRequest.command=VME_UPLOAD;
    clientRequest.addr=addr;
    clientRequest.size = sizeof (unsigned short);
    clientRequest.length = 0x10000;
    clientRequest.data = (char *) data;  //change 6
    clientRequest.hostname=hostname;
    
    /* send request to server */
    vme_send(&clientRequest);
    
    /* calculate checksum of data recived */
    checksum = 0;
    for(i=0;i<clientRequest.length;i++)
	checksum ^= data[i];
    
E 4
I 4
  /* zero out data buffer
  for(i=0;i<clientRequest.length;i++) data[i] = 0; */
E 4

D 4
    if (checksum != clientRequest.checksum)
	printf ("Checksum of data received does not match checksum from server.\n");
E 4
I 4
  /* set up request to send to server */
D 5
  clientRequest.command=VME_UPLOAD;
  clientRequest.addr=addr;
E 5
I 5
  clientRequest.command = VME_UPLOAD;
  clientRequest.addr = addr;
E 5
  clientRequest.size = sizeof (unsigned short);
  clientRequest.length = 0x10000;
  clientRequest.data = (char *) data;
D 5
  clientRequest.hostname=hostname;
E 5
I 5
  clientRequest.hostname = hostname;
E 5
E 4

D 4
   /*    
    for(i=0;i<10;i++)
	printf("Data %i from %s = %x\n",i,hostname,data[i]);
   */
E 4
I 4
  /* send request to server */
  vme_send(&clientRequest);

  /* calculate checksum of data recived */
  checksum = 0;
  for(i=0;i<clientRequest.length;i++) checksum ^= data[i];

  if(checksum != clientRequest.checksum)
  {
	printf("Checksum of data received does not match checksum from server.\n");
  }

 /* for(i=0;i<10;i++) printf("Data %i from %s = %x\n",i,hostname,data[i]); */
E 4
}

D 4
int router_creg_download(routerNum, data, hostname)
    unsigned int routerNum;
    unsigned short data;
    char *hostname;
E 4
I 4
D 5

E 5
int
D 5
router_creg_download(unsigned int routerNum,unsigned short data,char *hostname)
E 5
I 5
ep_creg1_download(unsigned short data, char *hostname)
E 5
E 4
{
D 4
    struct request clientRequest;
    int i;
E 4
I 4
  struct request clientRequest;
  int i;
D 5

E 5
I 5
    
E 5
  /* set up request to send to server */
  clientRequest.command = VME_DOWNLOAD;
D 5
  clientRequest.addr = ((routerNum-1)*0x00080000) + 0xfa060000;
E 5
I 5
D 8
  clientRequest.addr = 0xfa320000;
E 8
I 8
D 9
  clientRequest.addr = LEVEL1BASE + 0x320000;
E 9
I 9
  clientRequest.addr = 0x320000;
E 9
E 8
E 5
  clientRequest.size = sizeof (unsigned short);
  clientRequest.length = 1;
  clientRequest.data = (char*)&data;
  clientRequest.hostname = hostname;
D 5

E 5
I 5
    
E 5
  /* calculate checksum to send to server */
  clientRequest.checksum = data;
E 4
    
D 4
    /* set up request to send to server */
    clientRequest.command = VME_DOWNLOAD;
D 3
    clientRequest.addr = ((routerNum-1)*0x00080000) + 0xe0060000;
E 3
I 3
    clientRequest.addr = ((routerNum-1)*0x00080000) + 0xfa060000;
E 3
    clientRequest.size = sizeof (unsigned short);
    clientRequest.length = 1;
    clientRequest.data = (char*)&data;
    clientRequest.hostname = hostname;
    
    /* calculate checksum to send to server */
    clientRequest.checksum = data;
    
    /* send data to server */
    return(vme_send(&clientRequest));
E 4
I 4
  /* send data to server */
  return(vme_send(&clientRequest));
E 4
}


D 4
int ep_creg1_download(data, hostname)
    unsigned short data;
    char *hostname;
E 4
I 4
int
D 5
ep_creg1_download(unsigned short data, char *hostname)
E 5
I 5
ep_creg2_download(unsigned short data, char *hostname)
E 5
E 4
{
D 4
    struct request clientRequest;
    int i;
E 4
I 4
  struct request clientRequest;
  int i;
E 4
    
D 4
    /* set up request to send to server */
    clientRequest.command = VME_DOWNLOAD;
D 3
    clientRequest.addr = 0xe0320000;
E 3
I 3
    clientRequest.addr = 0xfa320000;
E 3
    clientRequest.size = sizeof (unsigned short);
    clientRequest.length = 1;
    clientRequest.data = (char*)&data;
    clientRequest.hostname = hostname;
E 4
I 4
  /* set up request to send to server */
  clientRequest.command = VME_DOWNLOAD;
D 5
  clientRequest.addr = 0xfa320000;
E 5
I 5
D 8
  clientRequest.addr = 0xfa340000;
E 8
I 8
D 9
  clientRequest.addr = LEVEL1BASE + 0x340000;
E 9
I 9
  clientRequest.addr = 0x340000;
E 9
E 8
E 5
  clientRequest.size = sizeof (unsigned short);
  clientRequest.length = 1;
  clientRequest.data = (char*)&data;
  clientRequest.hostname = hostname;
E 4
    
D 4
    /* calculate checksum to send to server */
    clientRequest.checksum = data;
E 4
I 4
  /* calculate checksum to send to server */
  clientRequest.checksum = data;
E 4
    
D 4
    /* send data to server */
    return(vme_send(&clientRequest));
E 4
I 4
  /* send data to server */
  return(vme_send(&clientRequest));
E 4
}

D 4
int ep_creg2_download(data, hostname)
    unsigned short data;
    char *hostname;
E 4
I 4

I 5
/* obsolete ? */
E 5
int
D 5
ep_creg2_download(unsigned short data, char *hostname)
E 5
I 5
router_creg_download(unsigned int routerNum,unsigned short data,char *hostname)
E 5
E 4
{
D 4
    struct request clientRequest;
    int i;
E 4
I 4
  struct request clientRequest;
  int i;
E 4
D 5
    
E 5
I 5

E 5
D 4
    /* set up request to send to server */
    clientRequest.command = VME_DOWNLOAD;
D 3
    clientRequest.addr = 0xe0340000;
E 3
I 3
    clientRequest.addr = 0xfa340000;
E 3
    clientRequest.size = sizeof (unsigned short);
    clientRequest.length = 1;
    clientRequest.data = (char*)&data;
    clientRequest.hostname = hostname;
E 4
I 4
  /* set up request to send to server */
  clientRequest.command = VME_DOWNLOAD;
D 5
  clientRequest.addr = 0xfa340000;
E 5
I 5
D 8
  clientRequest.addr = ((routerNum-1)*0x00080000) + 0xfa060000;
E 8
I 8
D 9
  clientRequest.addr = ((routerNum-1)*0x00080000) + LEVEL1BASE + 0x060000;
E 9
I 9
  clientRequest.addr = 0x060000 + ((routerNum-1)*0x00080000);
E 9
E 8
E 5
  clientRequest.size = sizeof (unsigned short);
  clientRequest.length = 1;
  clientRequest.data = (char*)&data;
  clientRequest.hostname = hostname;
E 4
D 5
    
E 5
I 5

E 5
D 4
    /* calculate checksum to send to server */
    clientRequest.checksum = data;
E 4
I 4
  /* calculate checksum to send to server */
  clientRequest.checksum = data;
E 4
    
D 4
    /* send data to server */
    return(vme_send(&clientRequest));
E 4
I 4
  /* send data to server */
  return(vme_send(&clientRequest));
E 4
}























E 1
