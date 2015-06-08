h49727
s 00008/00114/00111
d D 1.6 08/04/01 14:36:29 boiarino 7 6
c remove obsoleted functions
c 
e
s 00011/00000/00214
d D 1.5 07/03/20 16:38:47 boiarino 6 5
c *** empty log message ***
e
s 00021/00005/00193
d D 1.4 02/07/14 14:51:07 boiarino 5 4
c write file to the CLON_PARMS/ts/ area
c 
e
s 00146/00090/00052
d D 1.3 02/07/06 23:46:40 boiarino 4 3
c add get_bigdata, use it as default
c 
e
s 00014/00014/00128
d D 1.2 02/03/28 16:42:32 boiarino 3 1
c e0 -> fa
c 
e
s 00000/00000/00000
d R 1.2 01/10/22 13:45:27 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/vmememget/vmememget.c
e
s 00142/00000/00000
d D 1.1 01/10/22 13:45:26 boiarino 1 0
c date and time created 01/10/22 13:45:26 by boiarino
e
u
U
f e 0
t
T
I 4

/* vmememget.c */

I 6
#ifndef VXWORKS

E 6
E 4
I 1
#include <stdio.h>
#include <stdlib.h>
#include "vme_tcpInfo.h"
#include "vme_client.h"
#include "vme_error.h"
#include "vme_download.h" 
I 4
#include "level1.h" 
E 4

D 4
void usage ()
E 4
I 4

void
usage()
E 4
{
  printf ("Usage: vmememget <target host> <7 bit code> <destination file>\n");
  printf ("\tTarget host = the host name to get date from.\n");
  printf ("\t7 bit code = code to know what to get from board.\n");
D 7
  printf ("\tTestination file = where the info is going to be put.\n");
E 7
I 7
  printf ("\tDestination file = where the info is going to be put.\n");
E 7
  printf ("\tCode = #######  0/1 \n");
  printf ("\t       |||||||\n");
D 3
  printf ("\t       ||||||-  0xe0000000 - 0xe007ffff\n");
  printf ("\t       |||||--  0xe0080000 - 0xe00fffff\n");
  printf ("\t       ||||---  0xe0100000 - 0xe017ffff\n");
  printf ("\t       |||----  0xe0180000 - 0xe01fffff\n");
  printf ("\t       ||-----  0xe0200000 - 0xe027ffff\n");
  printf ("\t       |------  0xe0280000 - 0xe02fffff\n");
  printf ("\t       -------  0xe0300000 - 0xe037ffff\n");
E 3
I 3
D 7
  printf ("\t       ||||||-  0xfa000000 - 0xfa07ffff\n");
  printf ("\t       |||||--  0xfa080000 - 0xfa0fffff\n");
  printf ("\t       ||||---  0xfa100000 - 0xfa17ffff\n");
  printf ("\t       |||----  0xfa180000 - 0xfa1fffff\n");
  printf ("\t       ||-----  0xfa200000 - 0xfa27ffff\n");
  printf ("\t       |------  0xfa280000 - 0xfa2fffff\n");
  printf ("\t       -------  0xfa300000 - 0xfa37ffff\n");
E 7
I 7
  printf ("\t       ||||||-  0x000000 - 0x07ffff\n");
  printf ("\t       |||||--  0x080000 - 0x0fffff\n");
  printf ("\t       ||||---  0x100000 - 0x17ffff\n");
  printf ("\t       |||----  0x180000 - 0x1fffff\n");
  printf ("\t       ||-----  0x200000 - 0x27ffff\n");
  printf ("\t       |------  0x280000 - 0x2fffff\n");
  printf ("\t       -------  0x300000 - 0x37ffff\n");
E 7
E 3
D 4
  exit (-1);
E 4
I 4

  exit(-1);
E 4
}

D 4
FILE *f1;
E 4

D 4
void get_data(char* hostname, unsigned int addr, char* filename)
{ 
E 4
I 4
void
get_bigdata(char *hostname, char *filename)
{
  FILE *fd;
E 4
  struct request clientRequest;
I 4
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

D 7
void
get_data(char *hostname, unsigned int addr, char *filename)
{
  FILE *fd;
  struct request clientRequest;
E 4
  unsigned short data[0x80000];
  unsigned short checksum;
  unsigned int localaddr[4];
D 4
  int i,j;
E 4
I 4
  int i, j;
E 7
E 4

D 7
  localaddr[0]=0x0;
  localaddr[1]=0x20000;
  localaddr[2]=0x40000;
  localaddr[3]=0x60000;
E 7
D 4
  
  f1 = fopen (filename, "a+b");
E 4

D 4
  
  clientRequest.command=VME_UPLOAD;
E 4
I 4
D 7
  fd = fopen(filename, "a+b");
E 7

D 7
  clientRequest.command = VME_UPLOAD;
E 4
  clientRequest.size = sizeof (unsigned short);      
  clientRequest.length = 0x10000;
  clientRequest.data = (char *) data;
D 4
  clientRequest.hostname=hostname;
  
  for(j=0;j<4;j++)
E 4
I 4
  clientRequest.hostname = hostname;

  for(j=0; j<4; j++)
  {
    clientRequest.addr = addr + localaddr[j];
    vme_send(&clientRequest);
    checksum = 0;

    for(i=0; i<clientRequest.length; i++) checksum ^= data[i];
    if(checksum != clientRequest.checksum)
E 4
    {
D 4
      clientRequest.addr=localaddr[j] + addr;
      vme_send(&clientRequest);
      checksum = 0;
      
      for(i=0;i<clientRequest.length;i++)
	checksum ^= data[i];
      
      if (checksum != clientRequest.checksum){
	printf ("Checksum of data received does not match checksum from server.\n");
	exit(1);
      }
      
      for(i=0;i<clientRequest.length;i++)
	fwrite(&data[i],sizeof(data[i]),1,f1);
    } 
  fclose (f1);
E 4
I 4
      printf ("Checksum of data received does not match checksum from server.\n");
      exit(1);
    }

    for(i=0; i<clientRequest.length; i++)
    {
      fwrite(&data[i],sizeof(data[i]),1,fd); /* one 'short' per write ! */
    }
  }

  fclose(fd);
  return;
E 4
}
D 4
  
int main(int argc, char* argv[])
E 4
I 4


I 5



E 7
/* *coda and *filename are ignored !!! */

E 5
int
D 5
main(int argc, char* argv[])
E 5
I 5
main(int argc, char *argv[])
E 5
E 4
{
D 5
  char *hostname,*code,*filename;
E 5
I 5
  char *hostname, *code, *filename;
E 5
  unsigned int addr=0x0;
I 5
  char *parm, fname[1000];
E 5
  int i;

D 4
  if (argc != 4)
    usage();
E 4
I 4
  if(argc != 4) usage();
E 4

  hostname = argv[1];
  code = argv[2];
  filename = argv[3];
D 4
  
  for(i=6;i>=0;i--)
    {
      if (code[i]=='1')
	{
	  if (i==6)
	    {
D 3
	      addr = 0xe0000000;
E 3
I 3
	      addr = 0xfa000000;
E 3
	      printf("Reading from: %s at memory address: 0x%X\n",hostname,addr);
	      get_data(hostname,addr,filename);
	      printf("Done..\n");
	    }
	  if (i==5)
	    {
D 3
	      addr = 0xe0080000;
E 3
I 3
	      addr = 0xfa080000;
E 3
	      printf("Reading from: %s at memory address: 0x%X\n",hostname,addr);
	      get_data(hostname,addr,filename);
	      printf("Done..\n");
	    }
	  if (i==4)
	    {
D 3
	      addr = 0xe0100000;
E 3
I 3
	      addr = 0xfa100000;
E 3
	      printf("Reading from: %s at memory address: 0x%X\n",hostname,addr);
	      get_data(hostname,addr,filename);
	      printf("Done..\n");
	    }
E 4

D 4
 	  if (i==3)
	    {
D 3
	      addr = 0xe0180000;
E 3
I 3
	      addr = 0xfa180000;
E 3
	      printf("Reading from: %s at memory address: 0x%X\n",hostname,addr);
	      get_data(hostname,addr,filename);
	      printf("Done..\n");
	    }
 	  if (i==2)
	    {
D 3
	      addr = 0xe0200000;
E 3
I 3
	      addr = 0xfa200000;
E 3
	      printf("Reading from: %s at memory address: 0x%X\n",hostname,addr);
	      get_data(hostname,addr,filename);
	      printf("Done..\n");
	    }

 	  if (i==1)
	    {
D 3
	      addr = 0xe0280000;
E 3
I 3
	      addr = 0xfa280000;
E 3
	      printf("Reading from: %s at memory address: 0x%X\n",hostname,addr);
	      get_data(hostname,addr,filename);
	      printf("Done..\n");
	    }
	  if (i==0)
	    {
D 3
	      addr = 0xe0300000;
E 3
I 3
	      addr = 0xfa300000;
E 3
	      printf("Reading from: %s at memory address: 0x%X\n",hostname,addr);
	      get_data(hostname,addr,filename);
	      printf("Done..\n");
	    }
	}
      
E 4
I 4
D 7
  /*
  for(i=6; i>=0; i--)
  {
    if(code[i]=='1')
    {
      if(i==6)
      {
        addr = 0xfa000000;
        printf("Reading from: %s at memory address: 0x%X\n",hostname,addr);
        get_data(hostname,addr,filename);
        printf("Done..\n");
      }
	  if(i==5)
      {
        addr = 0xfa080000;
        printf("Reading from: %s at memory address: 0x%X\n",hostname,addr);
        get_data(hostname,addr,filename);
        printf("Done..\n");
      }
      if(i==4)
      {
        addr = 0xfa100000;
        printf("Reading from: %s at memory address: 0x%X\n",hostname,addr);
        get_data(hostname,addr,filename);
        printf("Done..\n");
      }
 	  if(i==3)
      {
        addr = 0xfa180000;
        printf("Reading from: %s at memory address: 0x%X\n",hostname,addr);
        get_data(hostname,addr,filename);
        printf("Done..\n");
      }
      if(i==2)
      {
        addr = 0xfa200000;
        printf("Reading from: %s at memory address: 0x%X\n",hostname,addr);
        get_data(hostname,addr,filename);
        printf("Done..\n");
      }
      if(i==1)
      {
        addr = 0xfa280000;
        printf("Reading from: %s at memory address: 0x%X\n",hostname,addr);
        get_data(hostname,addr,filename);
        printf("Done..\n");
      }
      if(i==0)
      {
        addr = 0xfa300000;
        printf("Reading from: %s at memory address: 0x%X\n",hostname,addr);
        get_data(hostname,addr,filename);
        printf("Done..\n");
      }
E 4
    }
I 4
      
  }
  */
E 4
D 5
  printf("All data in %s\n",filename);
I 4
  get_bigdata(hostname,"zzz1.dat");
  printf("All data in %s\n","zzz1.dat");
E 5

E 7
I 5
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

E 5
  exit(0);
E 4
}
I 4

I 6
#else

void
vmememget_dummy()
{
  return;
}

#endif
E 6
E 4
E 1
