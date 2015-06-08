/* if event rate goes higher then 10kHz, with randon triggers we have wrong
slot number reported in GLOBAL HEADER and/or GLOBAL TRAILER words; to work
around that problem temporary patches were applied - until fixed (Sergey) */
#define SLOTWORKAROUND

/* fanuc1.c - first readout list for VME crates */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>




typedef      long long       hrtime_t;

/*temporary here: for time profiling */

#define TIMERL_VAR \
  static hrtime_t startTim, stopTim, dTim; \
  static int nTim; \
  static hrtime_t Tim, rmsTim, minTim=10000000, maxTim, normTim=1

#define TIMERL_START \
{ \
  startTim = gethrtime(); \
}

#define TIMERL_STOP(whentoprint_macros,histid_macros) \
{ \
  stopTim = gethrtime(); \
  if(stopTim > startTim) \
  { \
    nTim ++; \
    dTim = stopTim - startTim; \
    /*if(histid_macros >= 0)   \
    { \
      uthfill(histi, histid_macros, (int)(dTim/normTim), 0, 1); \
    }*/														\
    Tim += dTim; \
    rmsTim += dTim*dTim; \
    minTim = minTim < dTim ? minTim : dTim; \
    maxTim = maxTim > dTim ? maxTim : dTim; \
    /*logMsg("good: %d %ud %ud -> %d\n",nTim,startTim,stopTim,Tim,5,6);*/ \
    if(nTim == whentoprint_macros) \
    { \
      logMsg("timer: %7llu microsec (min=%7llu max=%7llu rms**2=%7llu)\n", \
                Tim/nTim/normTim,minTim/normTim,maxTim/normTim, \
                ABS(rmsTim/nTim-Tim*Tim/nTim/nTim)/normTim/normTim,5,6); \
      nTim = Tim = 0; \
    } \
  } \
  else \
  { \
    /*logMsg("bad:  %d %ud %ud -> %d\n",nTim,startTim,stopTim,Tim,5,6);*/ \
  } \
}








#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

/* readout list VMEROL1 */
#define ROL_NAME__ "FANUC1"

/* ts control */
#define TRIG_SUPV

/* polling mode if needed */
#define POLLING_MODE

/* name used by loader */
#define INIT_NAME fanuc1__init

/* main TI board */
#define TIR_ADDR   0x0ed0

#include "rol.h"

void usrtrig(unsigned int EVTYPE, unsigned int EVSOURCE);
void usrtrig_done();

/* vme readout */
#include "FANUC_source.h"


/* user code */

#include "uthbook.h"
#include "coda.h"
#include "tt.h"
#include "scaler7201.h"
#include "adc792.h"
#include "tdc1190.h"

/* from fputil.h */
#define SYNC 0x20000000


#undef DEBUG

/* time profiling: 0-OFF, 1-ON */
static int timeprofile = 1;

static char rcname[5];

/* pointer to TTS structures for current roc */
extern TTSPtr ttp;


#define NBOARDS 22    /* maximum number of VME boards: we have 21 boards, but numbering starts from 1 */

/* v1190 start */

#define MY_MAX_EVENT_LENGTH 3000/*3200*/ /* max words per v1190 board */
#define TDC_OFFSET 0
#define CH_OFFSET  0

static int nboards;
static int maxbytes = 0;
static int error_flag[NBOARDS];
#ifdef SLOTWORKAROUND
static int slotnums[NBOARDS];
#endif
static unsigned int tdcbuftmp[NBOARDS*MY_MAX_EVENT_LENGTH+16];
static unsigned int *tdcbuf;
/*
static int tmpgood[MY_MAX_EVENT_LENGTH];
static int tmpbad[MY_MAX_EVENT_LENGTH];
*/
#define NTICKS 1000 /* the number of ticks per second */

void
tsleep(int n)
{
#ifdef VXWORKS
  taskDelay ((sysClkRateGet() / NTICKS) * n);
#else
#endif
}

/* v1190 end */

/**********************/
/* standart functions */

static void
__download()
{
  unsigned int offset, ii;

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  printf(">>>>>>>>>>>>>>>>>>>>>>>>>> ROCID = %d <<<<<<<<<<<<<<<<\n",rol->pid);

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");


  logMsg("INFO: User Download Executed\n",1,2,3,4,5,6);
}


static void
__prestart()
{
  unsigned short value, value0, value1, array0[32], array1[32], data16;
  int i, ii, ifull, total_bytes, i1, i2, i3;
  short buf;
  unsigned short slot, channel, pattern[8];
  GEF_STATUS status;
  GEF_VME_DMA_HDL dma_hdl;
  GEF_MAP_PTR mapPtr;

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  /* initialize OS windows and TI board */
  CDOINIT(FANUC,TIR_SOURCE);

  /* renamed to CDOINIT
  FANUC_INIT;
  */

#ifdef POLLING_MODE
  CTRIGRSS(FANUC, TIR_SOURCE, usrtrig, usrtrig_done);
#else
  CTRIGRSA(FANUC, TIR_SOURCE, usrtrig, usrtrig_done);
#endif

  sprintf(rcname,"RC%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);




  /* USER code here */

  /* DMA setup */
  usrVmeDmaInit();
  /*usrVmeDmaSetConfig(2,3,0);*/ /*A32,MBLT*/
  usrVmeDmaSetConfig(2,5,0); /*A32,2eSST,160MB/s*/

/* v1190 start - was in Download() for vxWorks, for Linux must be after CDOINIT where memory
mapping initialized */

  /* have to call tdc1190Init() before any other calls */
  nboards = tdc1190Init(0x08210000,0x10000,20,0);
  if(nboards>0)
  {
    /* window parameters */
    if(rol->pid ==      29) {tdc1190SetDefaultWindowWidth(1450); tdc1190SetDefaultWindowOffset(-4050);}
    else if(rol->pid == 28) {tdc1190SetDefaultWindowWidth(1450); tdc1190SetDefaultWindowOffset(-4050);}
    /*else if(rol->pid == 17) {tdc1190SetDefaultWindowWidth(1450); tdc1190SetDefaultWindowOffset(-4550);}*/
    /*else if(rol->pid == 17) {tdc1190SetDefaultWindowWidth(3000); tdc1190SetDefaultWindowOffset(-5000);}*/
    else if(rol->pid == 17) {tdc1190SetDefaultWindowWidth(1950); tdc1190SetDefaultWindowOffset(-4550);}
    else if(rol->pid == 20) {tdc1190SetDefaultWindowWidth(1200); tdc1190SetDefaultWindowOffset(-3800);}
    else if(rol->pid == 21) {tdc1190SetDefaultWindowWidth(1000); tdc1190SetDefaultWindowOffset(-3600);}
    else if(rol->pid == 22) {tdc1190SetDefaultWindowWidth(1100); tdc1190SetDefaultWindowOffset(-3600);}
    else if(rol->pid == 27) {tdc1190SetDefaultWindowWidth(1500); tdc1190SetDefaultWindowOffset(-4500);}
    else if(rol->pid ==  0) {tdc1190SetDefaultWindowWidth(1500); tdc1190SetDefaultWindowOffset(-3600);}
    else                    {tdc1190SetDefaultWindowWidth(1500); tdc1190SetDefaultWindowOffset(-3900);}

    tdc1190Config(1); /*0-berr,1-fifo*/
  }


  /* check 'tdcbuftmp' alignment and set 'tdcbuf' to 16-byte boundary */
  /*
  tdcbuf = &tdcbuftmp[0];
  */
  usrVmeDmaMemory(&i1, &i2, &i3);
  tdcbuf = (unsigned int *)i2;
  printf("alignment0: tdcbuftmp=0x%08x, tdcbuf=0x%08x\n",tdcbuftmp,tdcbuf);
  if( (((int)tdcbuf)&7) == 0xc )
  {
    tdcbuf += 1;
    printf("alignment1: tdcbuftmp=0x%08x, tdcbuf=0x%08x\n",tdcbuftmp,tdcbuf);
  }
  else if( (((int)tdcbuf)&7) == 0x8 )
  {
    tdcbuf += 2;
    printf("alignment2: tdcbuftmp=0x%08x, tdcbuf=0x%08x\n",tdcbuftmp,tdcbuf);
  }
  else if( (((int)tdcbuf)&7) == 0x4 )
  {
    tdcbuf += 3;
    printf("alignment3: tdcbuftmp=0x%08x, tdcbuf=0x%08x\n",tdcbuftmp,tdcbuf);
  }
  printf("alignment: tdcbuftmp=0x%08x, tdcbuf=0x%08x\n",tdcbuftmp,tdcbuf);


#ifdef SLOTWORKAROUND
  for(ii=0; ii<nboards; ii++)
  {
    slot = tdc1190GetGeoAddress(ii);
	slotnums[ii] = slot;
    printf("[%d] slot %d\n",ii,slotnums[ii]);
  }
#endif


if((ttp != NULL) && (rol->pid != 28) && (rol->pid != 29) && (rol->pid != 0))
{

  /* disable v1190 channels undescribed in TT */
  for(ii=0; ii<nboards; ii++)
  {
    slot = tdc1190GetGeoAddress(ii);
    printf("[%2d] v1190 at slot %2d\n",ii,slot);

    tdc1190EnableAllChannels(ii);
    for(channel=1; channel<128; channel++) /* always keep channel 0 enabled */
    {
      if(ttp->idnr[slot][channel] <= 0)
      {
        /*printf("disable slot# %2d channel# %3d\n",slot,channel);*/
        tdc1190DisableChannel(ii, channel);
      }
    }

    tdc1190GetChannels(ii, pattern);

    printf("TDC connectors:    DL   DR   CL   CR   BL   BR   AL   AR\n");
    printf("channels pattern: %04x %04x %04x %04x %04x %04x %04x %04x\n",
      pattern[7],pattern[6],pattern[5],pattern[4],
      pattern[3],pattern[2],pattern[1],pattern[0]);
  }

}


/* v1190 end */



  printf("INFO: User Prestart Executed\n");fflush(stdout);


  printf("INFO: Write Event Executed\n");fflush(stdout);

  *(rol->nevents) = 0;
  rol->recNb = 0;

  return;
}       

static void
__pause()
{
  CDODISABLE(FANUC,TIR_SOURCE,0);

  logMsg("INFO: User Pause Executed\n",1,2,3,4,5,6);

  return;
}

static void
__go()
{  
  unsigned short value, value0, value1, array0[32], array1[32];
  int i, ii;
  int extra;

  logMsg("INFO: User Go ...\n",1,2,3,4,5,6);

  CDOENABLE(FANUC,TIR_SOURCE,0); /* bryan has (,1,1) ... */

  return;
}

static void
__end()
{
  CDODISABLE(FANUC,TIR_SOURCE,0);

  logMsg("INFO: User End Executed\n",1,2,3,4,5,6);

  return;
}


void
usrtrig(unsigned int EVTYPE, unsigned int EVSOURCE)
{
  int EVENT_LENGTH;
  int *jw;
  int *adrlen, *bufin, *bufout, i, ind, ind2, ncol, nrow, len, len1, len2;
  unsigned int tmp, *buf, *dabufp1, *dabufp2;
  unsigned short *buf2;
  unsigned short value;
  unsigned int nwords, nevent, nbcount, nbsubtract, buff[32];
  unsigned short level;
  TIMERL_VAR;

  int ii, iii, njjloops, blen, jj, nev, rlen, rlenbuf[NBOARDS], nevts, nwrds;
  unsigned int res, datascan, mymask=0xfff0;
  unsigned int tdcslot, tdcchan, tdcval, tdc14, tdcedge, tdceventcount;
  unsigned int tdceventid, tdcbunchid, tdcwordcount, tdcerrorflags;
  unsigned int *tdc, *tdchead, itdcbuf;
  int nheaders, ntrailers;
#ifdef SLOTWORKAROUND
  unsigned int tdcslot_h, tdcslot_t, remember_h;
#endif

  /*
  printf("EVTYPE=%d syncFlag=%d\n",EVTYPE,syncFlag);
  */

  rol->dabufp = (int *) 0;

  /*open event type EVTYPE of BT_UI4*/
  CEOPEN(EVTYPE, BT_UI4);

  jw = rol->dabufp; 

  /***/

  /* at that moment only second CODA world defined  */
  /* first CODA world (length) undefined, so set it */
  /*jw[ILEN] = 1;*/ jw[-2] = 1;


  /* for sync events do nothing          */
  /* for physics events send HLS scalers */

/* if want physics_sync events in external mode
if( (*(rol->nevents))%1000)
{
  syncFlag=0;
}
else
{
  syncFlag=1;
  rol->dabufp += bosMmsg_(jw,"INFO",rol->pid,"physics_sync event");
}
*/

  if((syncFlag<0)||(syncFlag>1))         /* illegal */
  {
    printf("Illegal1: syncFlag=%d EVTYPE=%d\n",syncFlag,EVTYPE);
  }
  else if((syncFlag==0)&&(EVTYPE==0))    /* illegal */
  {
    printf("Illegal2: syncFlag=%d EVTYPE=%d\n",syncFlag,EVTYPE);
  }
  else if((syncFlag==1)&&(EVTYPE==0))    /* force_sync (scaler) events */
  {

/*
!!! we are geting here on End transition: syncFlag=1 EVTYPE=0 !!!
*/
  }
  else if((syncFlag==0)&&(EVTYPE==15)) /* helicity strob events */
  {
    ;
  }
  else           /* physics and physics_sync events */
  {

/*
!!! very first event in test setup: syncFlag=0 EVTYPE=14
even before pulser is started !!!
all following events are: syncFlag=0 EVTYPE=11
*/

/*	
    logMsg("physics or physics_sync: syncFlag=%d EVTYPE=%d\n",
      syncFlag,EVTYPE,3,4,5,6);
*/




    if(timeprofile)
    {
      /*TIMER_NORMALIZE;*/
	  TIMERL_START;
    }





    /*board-by-board DMA readout
	tdc1190ReadStart(tdcbuf, rlenbuf);
*/


    /*linked list DMA readout*/
    tdc1190ReadListStart(tdcbuf, rlenbuf);
    res = tdc1190ReadDone();
#ifdef DEBUG
    logMsg("tdc1190ReadDone returns %d\n",res,2,3,4,5,6);
#endif
    if(res == -1)
    {
      logMsg("BAD%d\n",res,2,3,4,5,6);
      for(jj=0; jj<nboards; jj++) rlenbuf[jj] = 0;
    }
	









    /************************/
    /************************/
    /************************/
	/* start of v1190/v1290 */

    /* open data bank */
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
    {
      logMsg("bosMopen_ Error: %d\n",ind,0,0,0,0,0);
    }
    rol->dabufp += NHEAD;

    /* analysis */
    itdcbuf = 0;
    nheaders = ntrailers = 0;

    njjloops = nboards;
	/*
for(i=0; i<200; i++) tmpgood[i] = tdcbuf[i];
	*/
    for(jj=0; jj<njjloops; jj++)
    {
      rlen = rlenbuf[jj];
	  
#ifdef DEBUG
	  
      level = tdc1190GetAlmostFullLevel(jj);
      ii = tdc1190StatusAlmostFull(jj);
      logMsg("jj=%d, rlen=%d, almostfull=%d level=%d\n",jj,rlen,ii,level,5,6);
	  
#endif
	  
      if(rlen <= 0) continue;

      tdc = &tdcbuf[itdcbuf];
      for(ii=0; ii<rlen; ii++) tdc[ii] = LSWAP(tdc[ii]);
      itdcbuf += rlen;

      for(ii=0; ii<rlen; ii++)
      {
		
        if( ((tdc[ii]>>27)&0x1F) == 0)
        {
          nbcount ++;
          *rol->dabufp ++ = (tdc[ii] | (tdcslot<<27));

#ifdef DEBUG
          tdcedge = ((tdc[ii]>>26)&0x1);
          tdcchan = ((tdc[ii]>>19)&0x7F);
          tdcval = (tdc[ii]&0x7FFFF);
		  if(1/*tdcchan<2*/)
          {
            logMsg(" DATA: tdc=%02d ch=%03d edge=%01d value=%08d\n",
              (int)tdc14,(int)tdcchan,(int)tdcedge,(int)tdcval,5,6);
		  }
#endif
        }
        else if( ((tdc[ii]>>27)&0x1F) == 1)
        {
          tdc14 = ((tdc[ii]>>24)&0x3);
          tdceventid = ((tdc[ii]>12)&0xFFF);
          tdcbunchid = (tdc[ii]&0xFFF);
#ifdef DEBUG
          logMsg(" HEAD: tdc=%02d event_id=%05d bunch_id=%05d\n",
            (int)tdc14,(int)tdceventid,(int)tdcbunchid,4,5,6);
#endif
        }
        else if( ((tdc[ii]>>27)&0x1F) == 3)
        {
          tdc14 = ((tdc[ii]>>24)&0x3);
          tdceventid = ((tdc[ii]>12)&0xFFF);
          tdcwordcount = (tdc[ii]&0xFFF);
#ifdef DEBUG
          logMsg(" EOB:  tdc=%02d event_id=%05d word_count=%05d\n",
            (int)tdc14,(int)tdceventid,(int)tdcwordcount,4,5,6);
#endif
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x18)
        {
#ifdef DEBUG
          logMsg(" FILLER\n",1,2,3,4,5,6);
#endif
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x11)
        {
          nbsubtract ++;
#ifdef DEBUG
          logMsg(" TIME TAG: %08x\n",(int)(tdc[ii]&0x7ffffff),2,3,4,5,6);
#endif
        }
        else if( ((tdc[ii]>>27)&0x1F) == 8)
        {
          nbcount = 1; /* counter for the number of output words from board */
          nbsubtract = 9; /* # words to subtract including errors */
          tdcslot = tdc[ii]&0x1F;
          tdceventcount = (tdc[ii]>>5)&0x3FFFFF;
          tdchead = (unsigned int *) rol->dabufp; /* remember pointer */
#ifdef SLOTWORKAROUND
          tdcslot_h = tdcslot;
	      remember_h = tdc[ii];
          if(slotnums[nheaders] != tdcslot) /* correct slot number */
		  {
            logMsg("WARN: [%2d] slotnums=%d, tdcslot=%d -> use slotnums\n",
				   nheaders,slotnums[nheaders],tdcslot,4,5,6);
            tdcslot = slotnums[nheaders];
		  }
#endif
          *rol->dabufp ++ = tdcslot;
          nheaders++;

		  
#ifdef DEBUG
		  
          logMsg("GLOBAL HEADER: %d words, 0x%08x (slot=%d nevent=%d) -> header=0x%08x\n",
            rlen,tdc[ii],tdc[ii]&0x1F,(tdc[ii]>>5)&0xFFFFFF,*tdchead,6);
		  
#endif
		  
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x10)
        {
#ifdef SLOTWORKAROUND
          /* double check for slot number */
          tdcslot_t = tdc[ii]&0x1F;
          if(tdcslot_h>21||tdcslot_t>21||tdcslot_h!=tdcslot_t)
          {
            /*logMsg("WARN: slot from header=%d (0x%08x), from trailer=%d (0x%08x), must be %d\n",
			  tdcslot_h,remember_h,tdcslot_t,tdc[ii],tdcslot,6)*/;
          }
#endif
          /* put nwords in header; substract 4 TDC headers, 4 TDC EOBs,
          global trailer and error words if any */
          /**tdchead = (*tdchead) | (((tdc[ii]>>5)&0x3FFF) - nbsubtract);*/
          *tdchead |= ((((tdc[ii]>>5)&0x3FFF) - nbsubtract)<<5);
          if((((tdc[ii]>>5)&0x3FFF) - nbsubtract) != nbcount)
          {			
			
            logMsg("ERROR: word counters: %d != %d (subtract=%d)\n",
			  (((tdc[ii]>>5)&0x3FFF) - nbsubtract),nbcount,nbsubtract,4,5,6);
			
			/*
for(i=0; i<200; i++) tmpbad[i] = tdcbuf[i];
logMsg("tmpgood=0x%08x tmpbad=0x%08x\n",tmpgood,tmpbad,3,4,5,6);
			*/
          }
          ntrailers ++;

#ifdef DEBUG
          logMsg("GLOBAL TRAILER: 0x%08x (slot=%d nw=%d stat=%d) -> header=0x%08x\n",
            tdc[ii],tdc[ii]&0x1F,(tdc[ii]>>5)&0x3FFF,
            (tdc[ii]>>23)&0xF,*tdchead,6);
#endif
        }
        else if( ((tdc[ii]>>27)&0x1F) == 4)
        {
          nbsubtract ++;
          tdc14 = ((tdc[ii]>>24)&0x3);
          tdcerrorflags = (tdc[ii]&0x7FFF);
		  /*
#ifdef DEBUG
		  */
		  if(error_flag[tdcslot] == 0) /* print only once */
          {
            unsigned int ddd, lock, ntdc;
			
            logMsg(" ERR: event# %7d, slot# %2d, tdc# %02d, error_flags=0x%08x, err=0x%04x, lock=0x%04x\n",
              tdceventcount,tdcslot,(int)tdc14,(int)tdcerrorflags,ddd,lock);
			
		  }
          error_flag[tdcslot] = 1;
		  /*
#endif
		  */
        }
        else
        {
#ifdef DEBUG
          logMsg("ERROR: in TDC data format 0x%08x\n",
            (int)tdc[ii],2,3,4,5,6);
#endif
        }

      } /* loop over 'rlen' words */

    }

	/*
    if(nheaders!=ntrailers || nboards!=nheaders)
      logMsg("ERROR: nboards=%d, nheaders=%d, ntrailers=%d\n",nboards,nheaders,ntrailers,4,5,6);
	*/


/*TEST: add some junk to increase event size*/
    /*rol->dabufp += 200;*/
/*TEST*/


    blen = rol->dabufp - (int *)&jw[ind+1];
    if(blen == 0) /* no data - return pointer to the initial position */
    {
      rol->dabufp -= NHEAD;
	  /* NEED TO CLOSE BANK !!?? */
    }
    else if(blen >= (MAX_EVENT_LENGTH/4))
    {
      logMsg("1ERROR: event too long, blen=%d, ind=%d\n",blen,ind,0,0,0,0);
      logMsg(": %d %d 0x%x 0x%x\n",blen,ind,dabufp1,dabufp2,0,0);
      tsleep(1); /* 1 = 0.01 sec */
    }
    else if(bosMclose_(jw,ind,1,blen) <= 0)
    {
      logMsg("2ERROR in bosMclose_ - space is not enough !!!\n",1,2,3,4,5,6);
    }

	/* end of v1190/v1290 */
    /**********************/
    /**********************/
    /**********************/



    if(timeprofile)
    {
      TIMERL_STOP(10000,1000+rol->pid);
    }








goto a123;
    /* open data bank */
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
    {
      printf("bosMopen_ Error: %d\n",ind);
    }
    rol->dabufp += NHEAD;

    for(iii=0; iii<200; iii++) *rol->dabufp ++ = (0x1000+iii);

	/* close bank */
    blen = rol->dabufp - (int *)&jw[ind+1];
    if(blen == 0) /* no data - return pointer to the initial position */
    {
      rol->dabufp -= NHEAD;
	  /* NEED TO CLOSE BANK !!?? */
    }
    else if(blen >= (MAX_EVENT_LENGTH/4))
    {
      printf("ERROR: event too long, blen=%d, ind=%d\n",blen,ind);
      printf(": %d %d 0x%x 0x%x\n",blen,ind,dabufp1,dabufp2);
      sleep(1);
    }
    else if(bosMclose_(jw,ind,1,blen) <= 0)
    {
      printf("ERROR in bosMclose_ - space is not enough !!!\n");
    }
a123:



    /* for physics sync event, make sure all board buffers are empty */
    if(syncFlag==1)
    {
      int scan_flag = 0;
      unsigned short slot, nev16;

	  /*
	  if(scan_flag) logMsg("SYNC: scan_flag=0x%08x\n",scan_flag,2,3,4,5,6);
	  */
	  /*logMsg("PTRN ..\n",1,2,3,4,5,6);*/
      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = SYNC + scan_flag;
      rol->dabufp += bosMclose_(jw,ind2,1,1);
	  /*logMsg("PTRN done.\n",1,2,3,4,5,6);*/
    }

  }

  /* close event */
  CECLOSE;

  return;
}

void
usrtrig_done()
{
  return;
}


void
__done()
{
  /* from parser */
  poolEmpty = 0; /* global Done, Buffers have been freed */

  /* Acknowledge tir register */
  CDOACK(FANUC,TIR_SOURCE,0);

  return;
}  

static void
__status()
{
  return;
}  
