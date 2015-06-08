h46294
s 00737/00000/00000
d D 1.1 10/03/09 12:45:21 boiarino 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1

/*
#define USE_ALTRO
*/

/* urol1.c - UNIX first readout list (polling mode) */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifndef VXWORKS
#include <sys/types.h>
#include <time.h>
#endif

#include "circbuf.h"

void davetrig(unsigned long, unsigned long);
void davetrig_done();

/*****************************/
/* former 'crl' control keys */

/* readout list UROL1 */
#define ROL_NAME__ "UROL1_PCI"

/* if use TS */
#define TRIG_SUPV

/* polling */
#define POLLING_MODE

/*???*/
/* 0: External Mode   1: Trigger Supervisor Mode 
#define TS_MODE  0
#define IRQ_SOURCE  0x10
*/

#ifdef TEST_SETUP
#define INIT_NAME urol1_pci_testsetup__init
#else
#define INIT_NAME urol1_pci__init
#endif

#include "rol.h"

/* PCI readout */
#include "PCI_source.h"

/************************/
/************************/

#include "coda.h"
#include "tt.h"

extern TTSPtr ttp; /* for tests only */

static char *rcname = "RC00";

long decrement;     /* local decrement counter */
extern long nevent; /* local event counter */
long mynev;

static int scan_flag; /* word to accumulate errors, will be reported at physics sync event */
static int evnum0, evnum0prev;;
static int evnum1, evnum1prev;
static int missed0, missed1;

#ifdef USE_ALTRO
#define NBINS 100
#define TIME_BIN_WIDTH 100 /* time bin width in nanosec */
#define NWCHANNEL  100000 /* must be bigger then # of bins + time + .. */
#define NOUTPUTBUF 1000000/* buffer length we will give to the rorc to be filled up (words) */
static int channel = 0;
#endif

#define SYNC 0x20000000

static void
__download()
{
  int i, poolsize;

#ifdef POLLING_MODE
  rol->poll = 1;
#endif

  /*PCI stuff*/
  pcitirInit();
  /*PCI stuff*/


  printf(">>>>>>>>>>>>>>>>>>>>>>>>>> ROCID = %d <<<<<<<<<<<<<<<<\n",rol->pid);

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");

  printf("INFO: User Download 1 Executed\n");

  return;
}

static void
__prestart()
{
  unsigned long jj, adc_id, sl;
  char *env;

  printf("User Prestart 1: ttp1=0x%08x\n",ttp);fflush(stdout);

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  printf("User Prestart 1: ttp11=0x%08x\n",ttp);fflush(stdout);

  /* init trig source PCI */
  PCI_INIT;

  /* Register a sync trigger source (up to 32 sources) */
  CTRIGRSS(PCI, 1, davetrig, davetrig_done); /* second arg=1 - what is that ? */


  printf("11\n");

  printf("User Prestart 1: ttp12=0x%08x\n",ttp);fflush(stdout);

  /*PCI stuff*/
  TIRPCI_SetL1AWidth(hDev, 10, 1);
  printf("12\n");
  TIRPCI_ClearScaler(hDev);
  printf("13\n");
  TIRPCI_PrintStatus(hDev,2);
  /*PCI stuff*/

  printf("14\n");


  rol->poll = 1;


  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);


#ifdef USE_ALTRO


#ifdef TEST_SETUP
  printf("User Prestart 1: ttp13=0x%08x\n",ttp);fflush(stdout);
  rorcDownload();
  printf("User Prestart 1: ttp14=0x%08x\n",ttp);fflush(stdout);
  rorcReadTable(rol->pid);
  printf("User Prestart 1: ttp15=0x%08x\n",ttp);fflush(stdout);
  if(rol->pid==30)
  {
    /*left upper, bonuspc0, crate 0*/
    int fecs[14] = {3,22};
	rorcPrestart(2010, channel, 2, fecs, 50, 0);
  }

#else
  rorcDownload();
  rorcReadTable(rol->pid);

  /* parameters: rorc channel, the number of fecs, fec slot list */

  if(rol->pid==29)
  {
    /*left upper, bonuspc0, crate 0*/
    int fecs[14] = {1,3,5,17,18,20};
	rorcPrestart(9043, channel, 6, fecs, 50, 0);
  }
  else if(rol->pid==31)
  {
    /*left bottom, bonuspc1, crate 1*/
    int fecs[14] = {1,3,5,16,18,20};
	rorcPrestart(7165, channel, 6, fecs, 50, 0);
  }
  else if(rol->pid==0)    
  { 
    /*right upper, bonuspc2, crate 2*/  
    int fecs[14] = {0,3,4,16,18,20};    
    rorcPrestart(6038, channel, 6, fecs, 50, 0);     
  }   
  else if(rol->pid==28)    
  { 
    /*right bottom, bonuspc3, crate 3*/  
    int fecs[14] = {0,4,6,16,19,22};
    rorcPrestart(9044, channel, 6, fecs, 50, 0);
  }
#endif
 
#endif


  printf("INFO: User Prestart 1 executed\n");

  printf("User Prestart 1: ttp2=0x%08x\n",ttp);fflush(stdout);

  /* from parser (do we need that in rol2 ???) */
  *(rol->nevents) = 0;
  rol->recNb = 0;

  printf("User Prestart 1: ttp3=0x%08x\n",ttp);fflush(stdout);

  return;
}

static void
__pause()
{
  CDODISABLE(PCI,1,0);

  printf("INFO: User Pause 1 Executed\n");

  return;
}

static void
__go()
{
  char *env;

  scan_flag = 0;
  evnum0prev=0;
  evnum1prev=0;
  missed0 = 0;
  missed1 = 0;

#ifdef USE_ALTRO
  rorcGo(channel);
#endif

  CDOENABLE(PCI,1,0);

  mynev = 0;

  printf("INFO: User Go 1 Executed\n");

  return;
}

static void
__end()
{
  int ii, total_count, rem_count;

  CDODISABLE(PCI,1,0);

#ifdef USE_ALTRO
  rorcEnd(channel);
  /*rorcExit();*/
#endif

  /*PCI stuff*/
  TIRPCI_PrintStatus(hDev,2);
  /*PCI stuff*/

  printf("INFO: User End 1 Executed\n");

  return;
}




#ifdef USE_ALTRO

int
rorcPrintEvent(int nwords, unsigned int *bufin)
{
  int i, j, k, ii, jj;
  FILE *fd;
  int nsamples, adc, tdc;
  int crate, slot, channel;

  float ww=1.;
  float tmpx;
  int idn;

  int ntmp;
  unsigned short tmp[NBINS+2];

  int type;
  unsigned int *b32;


  if(rol->pid==29) fd = fopen("/home/clasrun/tpc0.out","w");
  if(rol->pid==31) fd = fopen("/home/clasrun/tpc1.out","w");
  if(rol->pid==0)  fd = fopen("/home/clasrun/tpc2.out","w");
  if(rol->pid==28) fd = fopen("/home/clasrun/tpc3.out","w");
  fd = stdout;

  /* process buffer */

  /* set crate number as expected minus one */
  if(rol->pid==29) crate = 0;
  else if(rol->pid==31) crate = 1;
  else if(rol->pid==0) crate = 2;
  else if(rol->pid==28) crate = 3;
  else printf("error in tttpc: rocid=%d\n",rol->pid);

  for(i=0; i<nwords; i++)
  {
    type = (bufin[i]>>30)&0x3;

    if(bufin[i]==0xffffffff) /*global header (8 words)*/
	{
      fprintf(fd,"[%5d] 0x%08x -> global header [0]\n",i,bufin[i]);
      fprintf(fd,"[%5d] 0x%08x -> global header [1]\n",i+1,bufin[i+1]);
      fprintf(fd,"[%5d] 0x%08x -> global header [2] (event %d)\n",i+2,bufin[i+2],bufin[i+2]&0xffffff);
      fprintf(fd,"[%5d] 0x%08x -> global header [3]\n",i+3,bufin[i+3]);
      fprintf(fd,"[%5d] 0x%08x -> global header [4]\n",i+4,bufin[i+4]);
      fprintf(fd,"[%5d] 0x%08x -> global header [5]\n",i+5,bufin[i+5]);
      fprintf(fd,"[%5d] 0x%08x -> global header [6]\n",i+6,bufin[i+6]);
      fprintf(fd,"[%5d] 0x%08x -> global header [7]\n",  i+7,bufin[i+7]);
      i += 7;
      k = 0;
      crate ++;
	}
    else if(type==0) /*ALTRO channel payload ([29:20] [19:10] [9:0])*/
	{
      /* print all payload */
      fprintf(fd,"  from channel header: nsamples=%d (nwords=%d)\n",nsamples,(nsamples+2)/3);
      ntmp = 0;
      for(ii=0; ii<(nsamples+2)/3; ii++)
	  {
        fprintf(fd,"[%5d] 0x%08x -> payload: [%3d] %4d    [%3d] %4d    [%3d] %4d\n",i,bufin[i],
          j,(bufin[i]>>20)&0x3ff,j+1,(bufin[i]>>10)&0x3ff,j+2,bufin[i]&0x3ff);
        tmp[ntmp++] = (bufin[i]>>20)&0x3ff;
        tmp[ntmp++] = (bufin[i]>>10)&0x3ff;
        tmp[ntmp++] = bufin[i]&0x3ff;
        j+=3;
        k++;
        i++;
  	  }
      i--;

      /* decode and print */
      ii=0;
      while(ii<ntmp)
      {
        nsamples = tmp[ii++];
        tdc = tmp[ii++];
        if(nsamples>0) fprintf(fd,"     Nsamples=%d, tdc=%d\n",nsamples,tdc);
        for(jj=0; jj<(nsamples-2); jj++)
        {
          adc = tmp[ii++];

		  /*
          ww = ((float)adc)/10.;
          tmpx = tdc-jj;
          idn = slot*1000+channel;
          hf1_(&idn,&tmpx,&ww);
		  */

          fprintf(fd,"         [%3d] adc=%d\n",jj,adc);
		}
      }
	}
    else if(type==1) /*ALTRO channel header (block length=[25:16] channel address=[11:0])*/
	{
      slot = (bufin[i]>>7)&0x1f;
      channel = bufin[i]&0x7f;
      nsamples = (bufin[i]>>16)&0x3ff;
      fprintf(fd,"[%5d] 0x%08x -> channel header: slot %2d, channel %3d, # of 10bit samples=%6d\n",
        i,bufin[i],slot,channel,nsamples);
      j=0;
      k++;
	}
    else if(type==2) /*RCU trailer word*/
	{
      fprintf(fd,"[%5d] 0x%08x -> trailer word: 0x%08x (possible len = %d, k=%d)\n",i,bufin[i],bufin[i],bufin[i]&0xffffff,k);
	}
    else if(type==3) /*last RCU trailer word*/
	{
      fprintf(fd,"[%5d] 0x%08x -> last trailer word: 0x%08x (%d)\n",i,bufin[i],bufin[i],bufin[i]);
	}
    else
	{
      fprintf(fd,"ERRORRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR !!!\n");
      exit(0);
	}

  }

  /*fclose(fd);*/

  return(0);
}


int
rorcFindReference(int nwords, unsigned int *bufin)
{
  int i, j, k, ii, jj;
  FILE *fd;
  int nsamples, adc, tdc;
  int crate, slot, channel;

  float ww=1.;
  float tmpx;
  int idn;

  int ntmp;
  unsigned short tmp[NBINS+2];

  int type;
  unsigned int *b32;


  fd = stdout;

  /* process buffer */

  /* set crate number as expected minus one */
  if(rol->pid==29) crate = 0;
  else if(rol->pid==31) crate = 1;
  else if(rol->pid==0) crate = 2;
  else if(rol->pid==28) crate = 3;
  else printf("error in tttpc: rocid=%d\n",rol->pid);

  for(i=0; i<nwords; i++)
  {
    type = (bufin[i]>>30)&0x3;

    if(bufin[i]==0xffffffff) /*global header (8 words)*/
	{
	  /*
      fprintf(fd,"[%5d] 0x%08x -> global header [0]\n",i,bufin[i]);
      fprintf(fd,"[%5d] 0x%08x -> global header [1]\n",i+1,bufin[i+1]);
      fprintf(fd,"[%5d] 0x%08x -> global header [2] (event %d)\n",i+2,bufin[i+2],bufin[i+2]&0xffffff);
      fprintf(fd,"[%5d] 0x%08x -> global header [3]\n",i+3,bufin[i+3]);
      fprintf(fd,"[%5d] 0x%08x -> global header [4]\n",i+4,bufin[i+4]);
      fprintf(fd,"[%5d] 0x%08x -> global header [5]\n",i+5,bufin[i+5]);
      fprintf(fd,"[%5d] 0x%08x -> global header [6]\n",i+6,bufin[i+6]);
      fprintf(fd,"[%5d] 0x%08x -> global header [7]\n",  i+7,bufin[i+7]);
	  */
      i += 7;
      k = 0;
      crate ++;
	}
    else if(type==0) /*ALTRO channel payload ([29:20] [19:10] [9:0])*/
	{
      /* print all payload */
      ntmp = 0;
      /*
      fprintf(fd,"  from channel header: nsamples=%d (nwords=%d)\n",nsamples,(nsamples+2)/3);
	  */
      for(ii=0; ii<(nsamples+2)/3; ii++)
	  {
        /*
        fprintf(fd,"[%5d] 0x%08x -> payload: [%3d] %4d    [%3d] %4d    [%3d] %4d\n",i,bufin[i],
          j,(bufin[i]>>20)&0x3ff,j+1,(bufin[i]>>10)&0x3ff,j+2,bufin[i]&0x3ff);
		*/
        tmp[ntmp++] = (bufin[i]>>20)&0x3ff;
        tmp[ntmp++] = (bufin[i]>>10)&0x3ff;
        tmp[ntmp++] = bufin[i]&0x3ff;
        j+=3;
        k++;
        i++;
  	  }
      i--;

      /* decode and print */
      ii=0;
      while(ii<ntmp)
      {
        nsamples = tmp[ii++];
        tdc = tmp[ii++];
if(slot==22&&channel==119)
        if(nsamples>0) fprintf(fd,"     Nsamples=%d, tdc=%d\n",nsamples,tdc);
        for(jj=0; jj<(nsamples-2); jj++)
        {
          adc = tmp[ii++];

		  /*
          ww = ((float)adc)/10.;
          tmpx = tdc-jj;
          idn = slot*1000+channel;
          hf1_(&idn,&tmpx,&ww);
		  */

if(slot==22&&channel==119)
          fprintf(fd,"         [%3d] adc=%d\n",jj,adc);
		}
      }
	}
    else if(type==1) /*ALTRO channel header (block length=[25:16] channel address=[11:0])*/
	{
      slot = (bufin[i]>>7)&0x1f;
      channel = bufin[i]&0x7f;
      nsamples = (bufin[i]>>16)&0x3ff;
	  /*
      fprintf(fd,"[%5d] 0x%08x -> channel header: slot %2d, channel %3d, # of 10bit samples=%6d\n",
        i,bufin[i],slot,channel,nsamples);
	  */
      j=0;
      k++;
	}
    else if(type==2) /*RCU trailer word*/
	{
	  /*
      fprintf(fd,"[%5d] 0x%08x -> trailer word: 0x%08x (possible len = %d, k=%d)\n",i,bufin[i],bufin[i],bufin[i]&0xffffff,k);
	  */
	}
    else if(type==3) /*last RCU trailer word*/
	{
	  /*
      fprintf(fd,"[%5d] 0x%08x -> last trailer word: 0x%08x (%d)\n",i,bufin[i],bufin[i],bufin[i]);
	  */
	}
    else
	{
      fprintf(fd,"ERRORRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR !!!\n");
      exit(0);
	}

  }

  /*fclose(fd);*/

  return(0);
}
#endif



void
davetrig(unsigned long EVTYPE, unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
  int ii, len, len1, len2, type, lock_key, *tmp;
  int *adrlen, *bufin, *bufout, i, ind, ind2, ncol, nrow;
  unsigned long *secondword, *jw, *buf, *dabufp1, *dabufp2;
  DANODE *outEvent;
  unsigned int data;
  int ret;
  int max_words_from_channel;
#ifdef USE_ALTRO
  int roldabufp1[NOUTPUTBUF];
#endif

  
  /*printf("PCI trig reached\n");fflush(stdout);*/
/*
sleep(1);
  */

  /*PCI stuff*/
  TIRPCI_SetOutput(hDev, 3);
  /*PCI stuff*/

  rol->dabufp = (long *) 0;
  /*open event type EVTYPE of BT_UI4*/
  CEOPEN(EVTYPE, BT_UI4);

  /*PCI stuff*/
  ii = TIRPCI_Read_tdr(hDev);
  /*PCI stuff*/

  jw = rol->dabufp;
  jw[-2] = 1;
  secondword = rol->dabufp - 1; /* pointer to the second CODA header word */

  /*
  printf("syncFlag=%d EVTYPE=%d\n",syncFlag,EVTYPE);fflush(stdout);
  */

#ifdef TRIG_SUPV
  if((syncFlag<0)||(syncFlag>1)) /*illegal */
  {
    printf("111\n");
  }
  else if((syncFlag==0)&&(EVTYPE==0)) /*illegal */
  {
    printf("112\n");
  }
  else if((syncFlag==1)&&(EVTYPE==0)) /* force_sync (scaler) events */
  {
    printf("scaler event\n");fflush(stdout);
  }
  else if((syncFlag==0)&&(EVTYPE==15)) /* helicity strob events */
  {
    ;
  }
  else /* physics and physics_sync events */
#endif
  {

#ifdef USE_ALTRO
    /* open data bank */
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
    {
      printf("bosMopen_ Error: %d\n",ind);
    }
    rol->dabufp += NHEAD;

    len = rorcGetEvent(channel,NOUTPUTBUF,roldabufp1);
	if(len<=0)
	{
      missed0 ++;
      scan_flag |= 0x1;
      printf("ERROR: length ch0=%6d\n",len);
	  /*while(1) ;*/
      evnum0prev = 0;
	}
    else
	{
      evnum0 = roldabufp1[2];
      if(evnum0==(evnum0prev+2))
	  {
		;
		/*
        printf("WARN ch0: evnums are %6d(prev %6d)\n",evnum0,evnum0prev);
while(1) ;
		*/
	  }
      evnum0prev = evnum0;
	}

    max_words_from_channel = (((MAX_EVENT_LENGTH-1000)/2)/4);

    if(len > 0)
	{
      if(len > max_words_from_channel)
	  {
        printf("WARN: ch 0 event too big (%d words), will copy only %d words\n",
			   len,max_words_from_channel); fflush(stdout);
        len = 0/*max_words_from_channel*/;
	  }
	}

    if(len>0) for(i=0; i<len; i++) *rol->dabufp++ =  roldabufp1[i];
	
	printf("rol1 11\n");fflush(stdout);
    if(bosMclose_(jw,ind,1,len) == 0)
    {
      printf("ERROR in bosMclose_ - space is not enough !!!\n");
    }
	printf("rol1 12\n");fflush(stdout);

#else

    /* open data bank */
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
    {
      printf("bosMopen_ Error: %d\n",ind);
    }
    rol->dabufp += NHEAD;
    len = 1000;
    for(ii=0; ii<len; ii++)
    {
      *rol->dabufp++ = ii;
    }
    if(bosMclose_(jw,ind,1,len) == 0)
    {
      printf("ERROR in bosMclose_ - space is not enough !!!\n");
    }

#endif




    if(syncFlag==1)
    {
#ifdef USE_ALTRO
      printf("PHYSICS SYNC EVENT REACHED, recent event numbers are %d and %d (missed %d and %d)\n",
        evnum0,evnum1,missed0,missed1);fflush(stdout);

      if(scan_flag!=0)
      {
        printf("PHYSICS SYNC EVENT ERROR: scan_flag=0x%08x\n",scan_flag);
	  }
      
      while( (len=rorcGetEvent(channel,NOUTPUTBUF,roldabufp1)) != 0)
      {
        printf("SYNC ERROR: len=%d\n",len);
        scan_flag |= 0x10;
      }
#endif

      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = SYNC + scan_flag;
      rol->dabufp += bosMclose_(jw,ind2,1,1);

      scan_flag = 0;
    }



  }


  CECLOSE;

  /*PCI stuff*/
  TIRPCI_ClearOutput(hDev, 1);
  /*PCI stuff*/
  
  /*printf("rol1: PCI trig done, EVTYPE=%d\n",EVTYPE);fflush(stdout);*/
  
  return;
}

  
void
davetrig_done()
{
  /*PCI stuff*/
  TIRPCI_ClearOutput(hDev, 2);
  /*PCI stuff*/

  return;
}

void
__done()
{
  /* from parser */
  poolEmpty = 0; /* global Done, Buffers have been freed */

  CDOACK(PCI,1,0);

  return;
}
  
static void
__status()
{
  return;
}  
E 1
