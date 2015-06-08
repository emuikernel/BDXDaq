
/* vmememget.c */

#ifndef VXWORKS

#include <stdio.h>
#include <stdlib.h>
#include "vme_tcpInfo.h"
#include "vme_client.h"
#include "vme_error.h"
#include "vme_download.h" 
#include "level1.h" 


void
usage()
{
  printf ("Usage: vmememget <target host> <7 bit code> <destination file>\n");
  printf ("\tTarget host = the host name to get date from.\n");
  printf ("\t7 bit code = code to know what to get from board.\n");
  printf ("\tDestination file = where the info is going to be put.\n");
  printf ("\tCode = #######  0/1 \n");
  printf ("\t       |||||||\n");
  printf ("\t       ||||||-  0x000000 - 0x07ffff\n");
  printf ("\t       |||||--  0x080000 - 0x0fffff\n");
  printf ("\t       ||||---  0x100000 - 0x17ffff\n");
  printf ("\t       |||----  0x180000 - 0x1fffff\n");
  printf ("\t       ||-----  0x200000 - 0x27ffff\n");
  printf ("\t       |------  0x280000 - 0x2fffff\n");
  printf ("\t       -------  0x300000 - 0x37ffff\n");

  exit(-1);
}


void
get_bigdata(char *hostname, char *filename)
{
  FILE *fd;
  struct request clientRequest;
  unsigned short bigdata[BIGDATA];
  unsigned short checksum;
  int i;

  fd = fopen(filename, "w");

  clientRequest.command = BIGDATA_UPLOAD;
  clientRequest.addr = 0; /* arbitrary */
  clientRequest.size = sizeof (unsigned short);      
  clientRequest.length = BIGDATA/2;
  clientRequest.data = (char *) bigdata;
  clientRequest.hostname = hostname;


printf("1: len=%d\n",clientRequest.length);
  vme_bigsend(&clientRequest);
printf("2\n");
  checksum = 0;

  for(i=0; i<clientRequest.length; i++) checksum ^= bigdata[i];
  if(checksum != clientRequest.checksum)
  {
    printf ("Checksum of data received does not match checksum from server.\n");
    exit(1);
  }

  for(i=0; i<clientRequest.length; i++)
  {
    fwrite(&bigdata[i],sizeof(bigdata[i]),1,fd); /* one 'short' per write ! */
  }

  fclose(fd);
  return;
}




/* *coda and *filename are ignored !!! */

int
main(int argc, char *argv[])
{
  char *hostname, *code, *filename;
  unsigned int addr=0x0;
  char *parm, fname[1000];
  int i;

  if(argc != 4) usage();

  hostname = argv[1];
  code = argv[2];
  filename = argv[3];

  /* create reference file */
  if((parm = getenv("CLON_PARMS")) == NULL)
  {
    printf("vmememget: ERROR: CLON_PARMS is not set - cannot create level1 file\n");
  }
  else
  {
    sprintf(fname,"%s/ts/level1_tmp.dat",parm);
    printf("All data in %s\n",fname);
    get_bigdata(hostname,fname);
    printf("All data in %s\n",fname);
  }

  exit(0);
}

#else

void
vmememget_dummy()
{
  return;
}

#endif
