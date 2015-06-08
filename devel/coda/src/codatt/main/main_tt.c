
#include "cinclude/ttreadraw.c"

/* main_tt.c - main program to test tt*.c files */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "coda.h"
#include "tt.h"

/* to get NWBOS 
#ifndef VXWORKS
#include <pthread.h>
#endif
#include "circbuf.h"
*/
#include "ttbosio.h"

unsigned long user_flag3;
unsigned long user_flag4;
char *        user_flag1;
char *        user_flag2; 

int *iw,*BOSCommon;
BOSIO *BOSIOdescriptor = NULL;
TTSPtr *TTP;
TTSPtr  ttp;
char *TT_DataFile="./rawdata.evt";
double *timers;

THist *h1, *h2, *h3;

#define BUFSIZE  64000
#define MAXEVENT 100000
#define ROCNUM   24


main(int Argc, char** Argv)
{
  int sl,i,status,nskip=0,len,fflag, *bufp, nev;
  long *inb, *outb;

printf("=========== argc=%d ===========\n",Argc);

  MSG1(">>>>>>>>>>>> ALLOCATE   BOS COMMON of %d words <<<<<<<<<<<<\n",NBCS+5);
  BOSCommon = (int *)malloc((NBCS+5)*4);
  if(BOSCommon==NULL)
  {
	MSG(">>>>>>>>>>>> FAIL to ALLOCATE BOS COMMON <<<<<<<<<<<<\n");
	exit(1);
  }	
  iw = BOSCommon+5;
  MSG2("BOSCommon=0x%8.8X iw=0x%8.8X\n",BOSCommon,iw);
  MSG (">>>>>>>>>>>> INITIALIZE BOS COMMON <<<<<<<<<<<<\n");
  MSG3(">>>>>>>>>>>> bosInit(0x%8.8X,%d) returns %d \n",iw,NBCS,bosInit(iw,NBCS));

  if(Argc>2)
  {
    TT_DataFile = Argv[2];
  } 
  MSG1("Use '%s' file for raw data\n",TT_DataFile);

  
  if(Argc>3)
  {
	nskip = atol(Argv[3]);
	MSG1("Skip %i Records\n",nskip);
	if( (status = bosOpen(TT_DataFile,"r",&BOSIOdescriptor)) !=0)
    {
	  printf("bosOpen status %d \n",status);
	  fflush(stdout);
	  exit(1);
	}
	/*
	for(i=0; i<nskip; i++)
    {
	  if((status = ddGetLogRec(BOSIOdescriptor, &bufp, &len, &fflag))!=0)
      {
	    printf("\n\nError reading %s : %i\n",TT_DataFile,status);fflush(stdout);
	    exit(1);
	  }
	  if(fflag) free(bufp);
	}
	*/
  } 
  

  /* Allocate Output buffer */
  if( (outb = (long*)malloc(BUFSIZE)) == NULL )
  {
    MSG1("Can't malloc %i bytes for output buffer ==> ABORT\n",BUFSIZE);
    exit(1);
  }
  /* Allocate Input buffer */
  if ( (inb  = (long*)malloc(BUFSIZE)) == NULL ) {
	MSG1("Can't malloc %i bytes for input buffer ==> ABORT\n",BUFSIZE);
	exit(1);
  }

  printf("rol2: downloading DDL table ...\n");
  clonbanks();
  printf("rol2: ... done.\n");

  /* Load translation tables */
  TTP = (TTSPtr *)malloc((MAXROC+1)*sizeof(TTSPtr));
  bzero (TTP, (MAXROC+1)*sizeof(TTSPtr));
  TTP[ROCNUM]=ttp=TT_LoadROCTT(ROCNUM,NULL);
  ttp->wannaraw = 1;
  ttp->notrans = 0;

TT_Prestart(ROCNUM);
TT_Go();
 
  for(nev = 0; nev < MAXEVENT; nev++)
  {
	len = TT_ReadRaw(&inb[2], ROCNUM);
	/*printf("TT_ReadRaw returns %d\n",len);*/
    if(len==0) continue;

    inb[0] = len + 2; /* CODA header - 1st word */
    inb[1] = 0x10101; /* CODA header - 2nd word */
	outb[0]=NWBOS;
    /*printf("call TT_Main\n");fflush(stdout);*/
    len = TT_Main(inb, outb, ROCNUM);
    /*printf("TT_Main returns %d\n",len);*/
	
  }
  
  exit(0);
}

