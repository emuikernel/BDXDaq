
/* vme_download.c - run on UNIX */

#ifdef TEST_SETUP
#define NROUTERS 1
#else
#define NROUTERS 6
#endif


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
#include "vme_download.h"
#include "level1.h"

void
zero_routers(char *hostname)
{
  struct request clientRequest;
  unsigned short data = 0;
  unsigned int base, i;

  clientRequest.command = VME_DOWNLOAD;
  clientRequest.size = sizeof(unsigned short);
  clientRequest.length = 1;
  clientRequest.data = (char*) &data;
  clientRequest.hostname = hostname;
  clientRequest.checksum = 0;

  for(i=0; i<NROUTERS; i++)
  {
    base = i * 0x80000;
    clientRequest.addr = base + 0x1fffe;
    vme_send(&clientRequest);
    clientRequest.addr = base + 0x3fffe;
    vme_send(&clientRequest);
    clientRequest.addr = base + 0x5fffe;
    vme_send(&clientRequest);
    router_init(i+1,hostname);
  }
  evt_proc_init(hostname);

  return;
}


void
evt_proc_init(char *hostname)
{
  struct request clientRequest;
  unsigned short data = 0;

  clientRequest.command = VME_DOWNLOAD;
  clientRequest.size = sizeof(unsigned short);
  clientRequest.length = 1;
  clientRequest.data = (char *) &data;
  clientRequest.hostname = hostname;
  clientRequest.checksum = 0;

  clientRequest.addr = 0x320000; /* CREG 1 */
  vme_send(&clientRequest);

  clientRequest.addr = 0x340000; /* CREG 2 */
  vme_send(&clientRequest);

  clientRequest.addr = 0x360000; /* SREG */
  vme_send(&clientRequest);

  return;
}


void
router_init(int router_id, char *hostname)
{
  struct request clientRequest;
  unsigned short data = 0;
    
  clientRequest.command = VME_DOWNLOAD;
  clientRequest.size = sizeof(unsigned short);
  clientRequest.length = 1;
  clientRequest.data = (char *) &data;
  clientRequest.hostname = hostname;
  clientRequest.checksum = 0;

  clientRequest.addr = 0x060000 + ((router_id-1) * 0x80000); /* CREG */
  vme_send(&clientRequest);

  clientRequest.addr = 0x070000 + ((router_id-1) * 0x80000); /* SREG */
  vme_send(&clientRequest);

  return;
}


int
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
vme_download(unsigned int addr, unsigned short *data, char *hostname)
{
  struct request clientRequest;
  int i;

  /* set up request to send to server */
  clientRequest.command = VME_DOWNLOAD;
  clientRequest.addr = addr;
  clientRequest.size = sizeof (unsigned short);
  clientRequest.length = 0x10000;
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
  return(vme_send(&clientRequest));
}


int
vme_upload(unsigned int addr, unsigned short *data, char *hostname)
{
  struct request clientRequest;
  int i;
  unsigned short checksum;

  /* zero out data buffer
  for(i=0;i<clientRequest.length;i++) data[i] = 0; */

  /* set up request to send to server */
  clientRequest.command = VME_UPLOAD;
  clientRequest.addr = addr;
  clientRequest.size = sizeof (unsigned short);
  clientRequest.length = 0x10000;
  clientRequest.data = (char *) data;
  clientRequest.hostname = hostname;

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
}

int
ep_creg1_download(unsigned short data, char *hostname)
{
  struct request clientRequest;
  int i;
    
  /* set up request to send to server */
  clientRequest.command = VME_DOWNLOAD;
  clientRequest.addr = 0x320000;
  clientRequest.size = sizeof (unsigned short);
  clientRequest.length = 1;
  clientRequest.data = (char*)&data;
  clientRequest.hostname = hostname;
    
  /* calculate checksum to send to server */
  clientRequest.checksum = data;
    
  /* send data to server */
  return(vme_send(&clientRequest));
}


int
ep_creg2_download(unsigned short data, char *hostname)
{
  struct request clientRequest;
  int i;
    
  /* set up request to send to server */
  clientRequest.command = VME_DOWNLOAD;
  clientRequest.addr = 0x340000;
  clientRequest.size = sizeof (unsigned short);
  clientRequest.length = 1;
  clientRequest.data = (char*)&data;
  clientRequest.hostname = hostname;
    
  /* calculate checksum to send to server */
  clientRequest.checksum = data;
    
  /* send data to server */
  return(vme_send(&clientRequest));
}


/* obsolete ? */
int
router_creg_download(unsigned int routerNum,unsigned short data,char *hostname)
{
  struct request clientRequest;
  int i;

  /* set up request to send to server */
  clientRequest.command = VME_DOWNLOAD;
  clientRequest.addr = 0x060000 + ((routerNum-1)*0x00080000);
  clientRequest.size = sizeof (unsigned short);
  clientRequest.length = 1;
  clientRequest.data = (char*)&data;
  clientRequest.hostname = hostname;

  /* calculate checksum to send to server */
  clientRequest.checksum = data;
    
  /* send data to server */
  return(vme_send(&clientRequest));
}























