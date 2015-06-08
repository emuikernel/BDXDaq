
#include "../code.s/ppcTimer.c"

#define BONUS
#define DO_TPC_BANK


#ifdef TEST_SETUP
#define RUN_WITH_TIMEOUT	
#endif


/* uvmerol1.c - first readout list for VME crates for UNIX */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#ifndef VXWORKS
#include <pthread.h>
#endif

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

/* readout list UVMEROL1 */
#define ROL_NAME__ "UVMEROL1"




/* polling */
#define POLLING_MODE




/* need in one place in rol.h; must be undefined in rol1
IT IS UNDEFINED BY DEFAULT, NO NEED TO DO IT
#undef EVENT_MODE
*/


/* ts control */
#define TRIG_SUPV


/* name used by loader */
#ifdef TEST_SETUP
#define INIT_NAME uvmerol1_test__init
#else
#define INIT_NAME uvmerol1__init
#endif

#include "rol.h"

/* vme readout */
#include "VME_source.h" /* POLLING_MODE for tir_triglib.h undefined here */


/* user code */

#include "uthbook.h"
#include "coda.h"
#include "tt.h"


/* from fputil.h */
#define SYNC 0x20000000


#undef DEBUG


static char *rcname = "RC00";

/* user buffer pointers - all NULL at initialization */
extern int *userbuffers[NUSERBUFFERS];
/* pointer to TTS structures for current roc */
extern TTSPtr ttp;



#ifdef BONUS


/* Headers for RCU libraries and support */
#include <tools/get_input.h>
#include <altro/altro.h>
#include <tools/rcc_time_stamp.h>
#include <u2f/u2f.h>
#include <u2f/byte_swap.h>

extern unsigned int packageId;
extern unsigned int traceLevel;



/* Put any global user defined variables needed here for GEN readout */

#define RCU_MAX_CHANNEL 3328 /* may be useful .. */

#define TIME_BIN_WIDTH 114 /* time bin width in nanosec */
#define RCU_BUFFER_SIZE (1*1024*1024)
#define NWCHANNEL 200000 /* must be bigger then # of bins + time + .. */
#define MAXEVENTS 100  /* max number of events we'll report on one CLAS event */


unsigned int *rcuDataInBuf = 0; /* pointer to the input data buffer */

unsigned int SkipFlag=0, rcuDataCount=0, rcuDataMaxWords=2;

static unsigned int rcode = 0, rdata = 0, tdata = 0, fdata[2048] = {0};
static unsigned int fsize = 100, ccode = 0, cdata = 0;
static unsigned int offset = 0, rsize = 10, memdata[2048] = {0};
static unsigned int instructions[2048] = {0};
static char fname[200] =
"/usr/src/JLab/PPCLinux/kernel/modules/altro_usb/init_files/ginit2_15-85_zs_fpd"; 
static unsigned int chnum=0, point=0, tick=0, initonly=0, len1=0, len2=0;
static unsigned int mdsize = /*1024*8*/RCU_BUFFER_SIZE; /* cannot exceed RCU_BUFFER_SIZE */
static errst_t errst_s;
static trcfg_t trcfg_s;
static trcnt_t trcnt_s;
static int write_to_temp = 0;
unsigned int loop, osize;
long ievent,cont;
FILE *file;
unsigned int retBytes;
static int bonusevents;
static int nphysevents;
static int oldevnb;
static int ntpc, ntpch;
static int banknumber;
static int begin;
static double delta_t_average;
static float delta_n_average;
static float delta_nt_average;


/* BONUS service functions */

unsigned short
swap16(unsigned short word16)
{
  unsigned short res;
  unsigned char tmp;

  res = ((word16>>8)&0xFF) | ((word16&0xFF)<<8);

  return(res);
}

unsigned int
swap32(unsigned int word32)
{
  unsigned int res;
  unsigned char tmp;

  res = ((word32>>24)&0xFF) | (((word32>>16)&0xFF)<<8) | 
       (((word32>>8)&0xFF)<<16) | ((word32&0xFF)<<24);

  return(res);
}


/* tpcGetEvent() - search for first event in buffer

  inputs:

    rcuDataInBuf - data buffer
    beg - index of start of data in rcuDataInBuf
    end - index of end of data in rcuDataInBuf

  outputs:

    evBegin - index of the first word of event (the one before 0xffffffff)
    evEnd - index of the last word of event (the one after previous 0xffffffff)
    timestamp - event timestamp

  return value:

    event number, if found
    -1, if no more events
    -2, if incomplete event
    other negative, if error

 */

int
tpcGetEvent(unsigned int *rcuDataInBuf, int begin, int end,
            int *evBegin, int *evEnd, int *timestamp)
{
  int i, j, ii, iev;
  unsigned int data;

  /* inpt parameters check */
  if(begin >= end)
  {
    printf("\ntpcGetEvent: ERROR: begin=%d >= end=%d\n",begin,end);
    return(-3);
  }

  /* skip leading 0xffffffff's if any (may remain from previous event */
  j=begin;
  while((rcuDataInBuf[j]==0xffffffff) && (j<end)) j++;
  if(j>=end) return(-1); /* data contains 0xffffffff's only - no more events */

  /* search for 0xffffffff */
  for(i=j; i<=end; i++)
  {
    ii = i;
    if(rcuDataInBuf[i]==0xffffffff)
    {
      *evBegin = i - 1;
      *evEnd = j;

      /* decode event number and timestamp */
      data = swap32(rcuDataInBuf[*evBegin]);
      iev = (data>>22)&0x3FF;
      *timestamp = data&0x3FFFFF; /* time between events in .. */
	  /*	  
printf("\ntpcGetEvent: iev=%6d timestamp=%7d (0x%08x)\n",
iev,*timestamp,data);
	  */
      return(iev);
	}
  }

  /* incomplete event */
  if(ii>=end)
  {
    printf("\ntpcGetEvent: incomplete event\n");
    *evBegin = end;
    *evEnd = j;

    return(-2);
  }

  printf("\ntpcGetEvent: unknown error\n");
  return(-4); /* some error */
}


/* tpcDecodeEvent() - decode one event

  inputs:

    rcuDataInBuf - data buffer
    evBegin - index of the first word of event (the one before 0xffffffff)
    evEnd - index of the last word of event (the one after previous 0xffffffff)
      NOTE: we are going backward, so evBegin bigger then evEnd !!!

  outputs:

    *ntpc - the number of records in 'tpc' array
    tpc - output array (will goto TPC bank)

  return value:

    event number, or negative value if error

*/

int
tpcDecodeEvent(unsigned int *rcuDataInBuf, int evBegin, int evEnd,
               int *ntpc, unsigned short tpc[3][NWCHANNEL], int *timestamp)
{
  int iev, i, j, k, evIndex;
  unsigned int data, datal, datar;
  int ich, nw, nwch, n10ch, n10f, nhits;
  int clusternw, clustertime, nclusters;
  unsigned short sd1, sd2, chbuf[NWCHANNEL];

  nhits = *ntpc; /* TPC bank counter; it can be not zero if previous event(s)
                    was not moved to TPC bank yet */

  /* swap data buffer from evEnd to evBegin */
  for(i=evEnd; i<=evBegin; i++)
  {
    rcuDataInBuf[i] = swap32(rcuDataInBuf[i]);
  }

  /* decode event number and timestamp */
  data = rcuDataInBuf[evBegin];
  iev = (data>>22)&0x3FF;
  *timestamp = data&0x3FFFFF; /* time between events in .. */
  /*
  printf("\n===>iev=%6d timestamp=%7d (0x%08x)\n",iev,*timestamp,data);
  */

  /* channels */
  evIndex = evBegin - 1;
  while(evIndex > evEnd) /* we are going backward ! */
  {
    unsigned int ad1, ad2, ad3;

    /* decode channel trailer */
    datar = rcuDataInBuf[evIndex];
    datal = rcuDataInBuf[evIndex-1];
    ich = datal & 0xFFF;
    nw = ((datal>>16)&0xF) | ((datar&0x3F)<<4);

    evIndex -=2;
    nwch = ((nw+3)/4)*2;
    n10ch = nwch * 2;
    n10f = n10ch - nw;
    /*
    printf("=[%5d] 0x%08x 0x%08x -> ich=%4d nw=%4d -> ",
          evIndex,datal,datar,ich,nw);
    printf("nwch=%4d n10ch=%4d n10f=%4d\n",nwch,n10ch,n10f);
	*/
    /* copy one channel to intermediate buffer for processing */
    j = 0;
    for(i=0; i<nwch; i++)
    {
      data = rcuDataInBuf[evIndex-i];
	  /*
      printf("[%5d] 0x%08x -> ",evIndex-i,data);
	  */
      sd1 = (data>>10)&0x3FF;
      n10f --;
      if(n10f<0) chbuf[j++] = sd1;
	  /*
      printf("%4d(0x%03x) [%3d][%3d] - ",sd1,sd1,n10f,j);
	  */
      sd2 = data&0x3FF;
      n10f --;
      if(n10f<0) chbuf[j++] = sd2;
	  /*
      printf("%4d(0x%03x) [%3d][%3d]\n",sd2,sd2,n10f,j);
	  */
    }

    /* print intermediate buffer 
    printf("-> %d %d\n",j,nw);
    for(j=0; j<nw; j++) printf("=====[%3d]===== %5d\n",j,chbuf[j]);
	*/

    /* process intermediate buffer (info for TPC bank) */
    j = 0; /* intermediate buffer counter */
    nclusters = 0;
    while(j < nw)
    {
      clusternw = chbuf[j++];
      clustertime = chbuf[j++];
	  /*
      printf("clusternw=%d clustertime=%d\n",clusternw,clustertime);
	  */
      nclusters ++;
      for(k=0; k<(clusternw-2); k++)
      {
        if(chbuf[j]>0) /* CUT OFF ZERO ADCs !!!!! */
        {
          tpc[0][nhits] = ich;
          tpc[1][nhits] = (clustertime-k) * TIME_BIN_WIDTH;
          tpc[2][nhits] = chbuf[j];
		  /*
          printf(">>> tpc: %5d %5d %5d\n",
            tpc[0][nhits],tpc[1][nhits],tpc[2][nhits]);
		  */
          nhits++;
if(nhits >= NWCHANNEL) printf("ERRRRRRRRRRRRRROR 1\n");
        }
        j++;
      }
    }
	/*
    printf(">>>>> nclusters=%d nhits=%d\n",nclusters,nhits);
	*/
    evIndex -= nwch;
  }

  *ntpc = nhits;

  return(iev);
}





#endif


/***********************/
/* user trig functions */
/***********************/

void
usrtrig(unsigned long EVTYPE, unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
  int *adrlen, *bufin, *bufout, i, j, ind, ind2, ncol, nrow, len, len1, len2;
  int *jw, *secondword;
  unsigned long tmp, *buf, *dabufp1, *dabufp2;
  unsigned short *buf2;
  unsigned short value;
  unsigned int nwords, nbcount, nbsubtract, buff[32];
#ifdef BONUS
  int ret, evBegin, evEnd, iev, timestamp;
  int ii, end;
  unsigned short tpc[3][NWCHANNEL], *buf16;
  unsigned int tpch[3][MAXEVENTS], *buf32;
  int newevnb;
  PPCTB tb1, tb2;
  double delta_t;
#endif

  //printf("usrtrig reached\n");fflush(stdout);

#ifdef POLLING_MODE

  /*printf("usrtrig reached\n");*/

#else

  /*printf("usrtrig reached\n");*/

#endif




  rol->dabufp = (long *) 0;



/*
if( (*(rol->nevents))%2 )
{
  printf("do skip\n");
  return;
}
printf("do NOT skip\n");
*/



  /*open event type EVTYPE of BT_UI4*/
  CEOPEN(EVTYPE, BT_UI4);

  jw = (int *)(rol->dabufp);


  /***/

  /* at that moment only second CODA world defined  */
  /* first CODA world (length) undefined, so set it */
  /*jw[ILEN] = 1;*/ jw[-2] = 1;

  secondword = (int *)(rol->dabufp - 1); /* pointer to the second CODA header word */

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




    /*logMsg("Force Sync: syncFlag=%d EVTYPE=%d\n",syncFlag,EVTYPE,3,4,5,6);*/
    /*
    ncol=32;
    nrow=1;
    if( (ind = bosMopen_(jw,"HLS+",0,ncol,nrow)) > 0)
    {
      rol->dabufp += bosMclose_(jw,ind,ncol,nrow);
    }
    if( (ind = bosMopen_(jw,"HLS+",1,ncol,nrow)) > 0)
    {
      rol->dabufp += bosMclose_(jw,ind,ncol,nrow);
    }
    */
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


#ifdef BONUS

  nphysevents ++;
  /*
printf("GOT EVENT: nphysevents=%d (ntpc=%d)\n",nphysevents,ntpc);
  */
  /*if( ((*(rol->nevents))%1) == 0  )
  {*/
  if(ntpc == 0)
  {
    /*bzero(rcuDataInBuf,RCU_BUFFER_SIZE);*/
	/*
    printf("\n\n\ncall U2F_ReadOut, begin=%d\n",begin);
	*/
#ifdef RUN_WITH_TIMEOUT	
    begin = 0;
    ret = U2F_ReadOut(mdsize, &retBytes, (u_char *)&rcuDataInBuf[begin], 0);
printf("after U2F_ReadOut: mdsize=%d, ret=%d, retBytes=%d\n",mdsize,ret,retBytes);
#else
ppcTimeBaseGet(&tb1);
    for(i=0; i<1024/*the number of 1024s chunks in buffer*/; i++)
	{
      ret = U2F_ReadOut(1024, &retBytes, (u_char *)&rcuDataInBuf[begin], 0);
	  /*
printf("XXX ret=%d retBytes=%d\n",ret,retBytes);
	  */
      ii = (i+1)*(1024/4);
      if( (retBytes != 1024) || /* last buffer was not full */
          (ret == 0) ||         /* we had timeout */
          ((rcuDataInBuf[ii-1]==0xffffffff)&&(rcuDataInBuf[ii-2]==0xffffffff)) )
               /* event ended exactly on 1024-boundary */
      {
if(ret==0)
{
  printf("YYY ret=%d retBytes=%d ii=%d lasts=0x%08x 0x%08x\n",
  ret,retBytes,ii,rcuDataInBuf[ii-1],rcuDataInBuf[ii-2]);
}
        break;
      }
      begin += 256;
	}

ppcTimeBaseGet(&tb2);
delta_t = ppcTimeBaseDuration(&tb1,&tb2);

delta_n_average += (((float)(begin + (retBytes/4)-1))*4);
delta_nt_average += ((float)(begin + 256))*4;
delta_t_average += delta_t;

if( ((*(rol->nevents))%1000) == 0 )
{
  delta_n_average = delta_n_average / 1000.;
  delta_nt_average = delta_nt_average / 1000.;
  /* delta_t_average must /1000 and then *1000 so do nothing */
  printf("> event size = %7.1f Bytes, time = %.8f millisec, data rate = %6.3f(%6.3f) MBytes/sec\n",
    delta_n_average,delta_t_average,(delta_n_average/delta_t_average)/1000.,
   (delta_nt_average/delta_t_average)/1000.);
  delta_t_average = 0.;
  delta_n_average = 0.;
  delta_nt_average = 0.;
}

#endif

	/*if(ret) rcc_error_print(stdout, ret);*/
	/*
    printf("[%5d] ret=%d, retBytes=%d\n",*(rol->nevents),ret,retBytes);
	*/

    /* check retBytes (maybe do not need it any more) 
    for(i=0; i<(mdsize/4); i++)
    {
      if(rcuDataInBuf[i]==0)
      {		
        printf("first zero word is %d, retBytes/4=%d\n",i,retBytes/4);
        break;
	  }
	}
	*/
	/*	
for(i=begin; i<(begin+(retBytes/4)); i++)
{
  printf("222 [%4d] 0x%08x\n",i,rcuDataInBuf[i]);
}
*/
    if(retBytes>0)
    {
      end = begin + (retBytes/4)-1; /* use 'begin' from previous event */
      begin = 0; /* now set it to zero */
#ifdef TEST_SETUP
	  /*
printf("befor analysis: begin=%d, end=%d, retBytes=%d\n",begin,end,retBytes);
	  */
#endif
	  /* SCAN FOR EVENTS */
      while(end>begin)
      {
        /* get first event */
        iev=tpcGetEvent(rcuDataInBuf,begin,end,&evBegin,&evEnd,&timestamp);
        if(iev >= 0)
        {
          iev=tpcDecodeEvent(rcuDataInBuf,evBegin,evEnd,&ntpc,tpc,&timestamp);
		  /*
          printf("+> iev=%d evBegin=%d evEnd=%d\n",iev,evBegin,evEnd);
		  */
          /* update tpc event numbers */
          newevnb = iev;
		  /*
          printf("iev=%d newevnb=%d oldevnb=%d\n",iev,newevnb,oldevnb);
		  */
          if(newevnb < oldevnb) newevnb += 1024;
          if(newevnb - oldevnb != 1)
          {
            printf("ERROR: bonusevents=%d newevnb=%d(%d) old=%d\n",
              bonusevents,newevnb,(newevnb-1024),oldevnb);
          }
          oldevnb = iev;
          bonusevents ++;

          /* fill tpch array (should move it to tpcDecodeEvent ??? */
          tpch[0][ntpch] = bonusevents;
          tpch[1][ntpch] = timestamp;
          tpch[2][ntpch] = ntpc;
          ntpch ++;
#ifdef TEST_SETUP
		  if(ntpch > 1)
		  {
            FILE *ff;
            ff = fopen("zzz","w");
            for(ii=0; ii<=end; ii++) fprintf(ff,"0x%08x\n",rcuDataInBuf[ii]);
            fclose(ff);
		  }
#endif
          begin = evBegin+1; /* first 0xffffffff after previous event */
		  /*
          printf("!!! new begin=%d bonusevents=%d\n",begin,bonusevents);
		  */
        }
        else if(iev==-1) /* there is no more events */
	    {
          /*printf("++> iev=%d (no more events)\n\n",iev);*/
          begin = 0;
          break;
	    }
        else if(iev==-2) /* incomplete event */
	    {
          printf("++> iev=%d\n\n",iev);
          memcpy((char *)rcuDataInBuf, (char *)&rcuDataInBuf[evEnd],
            (evBegin-evEnd+1)<<2);
          begin = (evBegin-evEnd+1);
          break;
	    }
        else /* unknown error */
	    {
          printf("++> iev=%d\n\n",iev);
          begin = 0;
          break;
	    }

	  }

	}

    if(nphysevents != bonusevents)
    {
      printf("---> SYNC INFO: clas=%6d bonus=%6d\n",
        nphysevents,bonusevents);
#ifdef TEST_SETUP
      exit(0);
#endif
    }

  }
  /*}*/


  /*
    event number, if found
    -1, if no more events
    -2, if incomplete event
    other negative, if error
  */


  if(ntpch>0) /* have some data, put them into TPC bank */
  {
	/*	
    printf("!!!!!!!!! Got event of size %d rows\n",ntpc);
	*/
#ifdef DO_TPC_BANK

    /* TPCH bank */
    if((ind = bosMopen_(jw, "TPCH", banknumber, 3, 0)) <=0)
    {
      printf("bosMopen_ Error opening TPCH bank: %d\n",ind);
    }

    buf32 = (unsigned int *) &jw[ind+1];
    for(ii=0; ii<ntpch; ii++)
    {
      *buf32++ = tpch[0][ii];
      *buf32++ = tpch[1][ii];
      *buf32++ = tpch[2][ii];
	}

    if((len = bosMclose_(jw,ind,3,ntpch)) <= 0)
    {
      printf("ERROR in bosMclose_ - space is not enough !!!\n");
    }
    else
    {
      /*printf("TPCH bank closed, nrow=%d, len=%d\n",ntpch,len)*/;
    }
    ntpch = 0;
    rol->dabufp += len;


    /* create TPC bank only if tpc[] contains data */
    /* it is possible that ntpch != 0 but ntpc == 0 */
    if(ntpc > 10000)
	{
      printf("WARN: ntpc=%d, will be truncated to 10000, last hit will contains 0xffffs\n",ntpc); 
      ntpc = 10000;
      tpc[0][ntpc-1] = 0xffff;
      tpc[1][ntpc-1] = 0xffff;
      tpc[2][ntpc-1] = 0xffff;
	}

    if(ntpc > 0)
    {
      /* TPC bank */
      if((ind = bosMopen_(jw, "TPC ", banknumber, 3, 0)) <=0)
      {
        printf("bosMopen_ Error opening TPC bank: %d\n",ind);
      }

      buf16 = (unsigned short *) &jw[ind+1];
      for(ii=0; ii<ntpc; ii++)
      {
        *buf16++ = tpc[0][ii];
        *buf16++ = tpc[1][ii];
        *buf16++ = tpc[2][ii];
	  }

      if((len = bosMclose_(jw,ind,3,ntpc)) <= 0)
      {
        printf("ERROR in bosMclose_ - space is not enough !!!\n");
      }
      else
      {
        /*printf("TPC bank closed, nrow=%d, len=%d\n",ntpc,len)*/;
      }
      ntpc = 0;
      rol->dabufp += len;
	}

#endif

  }
  /*else
  {
    printf(".. no new events: clas=%6d bonus=%6d\n",nphysevents,bonusevents);
  }*/

#endif


	

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


/**********************/
/* standart functions */
/**********************/

static void
__download()
{
  unsigned long offset, ii;

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif


  printf(">>>>>>>>>>>>>>>>>>>>>>>>>> ROCID = %d <<<<<<<<<<<<<<<<\n",rol->pid);

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");

  printf("INFO: User Download Executed\n");
}


static void
__prestart()
{
  char *env;
  unsigned short value, value0, value1, array0[32], array1[32], data16;
  int i, ii, ifull;
  short buf;
  unsigned short slot, channel, pattern[8];
#ifdef BONUS
  unsigned long ret;
#endif

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  VME_INIT; /* init trig source VME */

#ifdef POLLING_MODE
  CTRIGRSS(VME, 1, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */
#else
  /* Register an async trigger source, (up to 32 sources) */
  /* CTRIGRSA(source,code,handler,done)*/
  /* link async trig source VME 1 to usrtrig and usrtrig_done */
  CTRIGRSA(VME, 1, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */
#endif

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);


#ifdef BONUS

  packageId = 0;
#ifdef TEST_SETUP
  traceLevel = 0; /*100 for debug messages*/
#else
  traceLevel = 0; /*100 for debug messages*/
#endif

/****************/ 
/* Open U2F_LIB */
/****************/

  ret = U2F_Open(HW, DEFAULT_NODE);
  if(ret) rcc_error_print(stdout, ret);

/***********************************/
/* Open the rcc_time_stamp library */
/***********************************/

  /*ret = ts_open(1, TS_DUMMY);*/
  if(ret) rcc_error_print(stdout, ret);

/********************/
/* delay 1000000 us */
/********************/
/*
  ts_delay(1000000);
*/
  usleep(1000000);

/*****************/
/* Switch on FEC */
/*****************/

  rcode = O_ACTFEC;
#ifdef TEST_SETUP
  rdata = 0x0001;
#else
  rdata = 0x1fff;
#endif
  ret = U2F_Reg_Write(rcode, rdata);
  if(ret) rcc_error_print(stdout, ret);
    
/********************/
/* delay 1000000 us */
/********************/
  /*ts_delay(1000000);*/
  usleep(1000000);

  /* Reset FEC */

  ret = U2F_Exec_FECRST();
  if(ret) printf("ret from FECRST is %d\n",ret);


/********************/
/* delay 1000000 us */
/********************/

  /*ts_delay(1000000);*/
  usleep(1000000);

/*************************************/  
/* Reset Trigger Counter Instruction */
/*************************************/

  ret = U2F_Exec_RS_TRCNT();
  printf("ret from RS_TRCNT is %d\n",ret);

/********************/
/* delay 1000000 us */
/********************/

  /*ts_delay(1000000);*/
  usleep(1000000);

/*************************/  
/* Clear Trigger Counter */
/* (Reset EVWORD Register) */
/*************************/

  ret = U2F_Exec_TRG_CLR();
  printf("ret from TRG_CLR command is %d\n",ret);


/***************************/  
/* EVWORD in The Beginning */
/***************************/

  /*** Read EVWORD ***/ 
  ret = U2F_EVWORD_Read(&rdata); 
  printf("EVWORD in the Beginning = %x\n", rdata);

/*************************************/  
/* Reset Status Register Instruction */
/*************************************/
 
  ret = U2F_Exec_RS_STATUS();
  printf("ret from RS_STATUS is %d\n",ret);

/****************/
/* ERROR STATUS */
/****************/

  rcode = O_ERRST;
  ret = U2F_Reg_Read(rcode, &rdata); 
  ret = U2F_ERRST_decode(rdata, &errst_s);
  if(rdata & 0x1F) 
  {
    printf("\n***********************************************************\n");
    printf("ERROR INFO = %x\n", rdata);
    printf("\nDecoding ERRST register\n");
    printf("Pattern error is : %s\n", errst_s.pattern_error ? "True" : "False");
    printf("Abort is	 : %s\n", errst_s.abort ? "True" : "False");
    printf("Time-out is	 : %s\n", errst_s.timeout ? "True" : "False");
    printf("Altro error is   : %s\n", errst_s.altro_error ? "True" : "False");
    printf("\n***********************************************************\n");
  }


/************************************/
/* Load the Instruction Memory data */
/************************************/

  for(ii=0; ii<2048; ii++) fdata[ii] = 0;

#ifdef TEST_SETUP
  char fname1[200] = "/usr/src/JLab/PPCLinux/kernel/modules/altro_usb/init_files/ginit2_15-85_zs_fpd_testsetup";
  printf("Reading instructions from file >%s<\n", fname1);
  ret = U2F_File_Read(fname1, fsize, fdata);
#else
  printf("Reading instructions from file >%s<\n", fname);
  ret = U2F_File_Read(fname, fsize, fdata);
#endif

  if(ret) rcc_error_print(stdout, ret);

/*************************************/    
/* write into the Instruction Memory */
/*************************************/

  printf("\n***********************************************************\n");

  /* find how many lines we've got */
  rsize = 0;
  for(ii=0; ii<fsize; ii++)
  {
    if(fdata[ii]==0) break;
    rsize ++;
  }

  offset = 0;
  printf("Initializing instruction memory: fsize=%d, rsize=%d\n",fsize,rsize);
  for(loop=0; loop<rsize; loop++)
  {
    memdata[loop] = fdata[loop];
    instructions[loop]=fdata[loop];
    printf("Current Instructions Are: %d = 0x%08x\n", loop, memdata[loop]);
  }
  ret = U2F_IMEM_Write(rsize, offset, memdata);
  if(ret) rcc_error_print(stdout, ret);

/****************************************/
/* initialize the Channel Readout List  */
/****************************************/

  for(loop=0; loop<2048; loop++) memdata[loop] = 0x0;
 
#ifdef TEST_SETUP
  rsize = 8;
#else
  rsize = 104;
#endif
  offset = 0;
  {
    FILE *fd = NULL;
    int iloop, nread, i1, i2, i3;

    /* bonuspc1 */
    if(rol->pid==31) fd = fopen("/usr/src/JLab/PPCLinux/kernel/modules/altro_usb/init_files/ActChanList1.txt","r");
    /* bonuspc2 */
    if(rol->pid==27) fd = fopen("/usr/src/JLab/PPCLinux/kernel/modules/altro_usb/init_files/ActChanList2.txt","r");
    
    if(fd != NULL)
	{
      iloop = 0;
      while(1)
      {
        nread = fscanf(fd,"%d %d %x\n", &i1, &i2, &i3);
        printf("nread=%d -> %d %d 0x%04x\n",nread,i1,i2,i3);
        if(nread == EOF)
        {
          printf("==> CHANNEL MASK: end-of-file\n");
          break;
        }
        if(nread != 3)
        {
          printf("==> CHANNEL MASK: ERROR reading file: nread=%d\n",nread);
          for(loop = 0; loop < rsize; loop++) memdata[loop] = 0xffff;
          break;
        }

        memdata[iloop++] = i3;
	  }

      if(iloop != rsize)
      {
        printf("==> CHANNEL MASK ERROR: cannot open .. file, use defaults\n");
        for(loop = 0; loop < rsize; loop++) memdata[loop] = 0xffff;
      }

      fclose(fd);
	}
    else
	{
      printf("==> CHANNEL MASK ERROR: iloop=%d, use defaults\n",iloop);
      for(loop = 0; loop < rsize; loop++) memdata[loop] = 0xffff;
	}

    printf("\n\n==> CHANNEL MASK: iloop = %d\n",iloop);
    for(loop = 0; loop < rsize; loop++)
    {
      printf("==> CHANNEL MASK [%3d] 0x%04x\n",loop,memdata[loop]);
    }
    printf("\n\n");

  }

  ret = U2F_ACL_Write(rsize, offset, memdata);
  if(ret) rcc_error_print(stdout, ret);

/**********************************/
/* Execute the Instruction Memory */
/**********************************/

  ccode = C_EXEC;
  cdata = 0;
  ret = U2F_Exec_Command(ccode, cdata);
  if(ret) rcc_error_print(stdout, ret);
    
/*****************/
/* delay 1000 us */
/*****************/

  /*ts_delay(1000);*/
  usleep(1000);

/**********************/
/* COUNT THE TRIGGERS */
/**********************/
 
  printf("\nN TRIGGERS IN THE BEGINNING\n");
  rcode = O_TRCNT;
  ret = U2F_Reg_Read(rcode, &rdata); 
  ret = U2F_TRCNT_decode(rdata, &trcnt_s);
  printf("Number of L1 triggers received: %d\n", trcnt_s.ntr); 
  printf("Number of L1 triggers accepted: %d\n", trcnt_s.nta); 
  printf("\n***********************************************************\n");


/************************************************/	
/* Configure the Trigger Configuration Register */
/***********************************************/
/* 0x50800 : MODE = 2, 4 buffers, TW = 2048 sampling clock cycles   */
/* 0x54800 : MODE = 2, 8 buffers, TW = 2048 sampling clock cycles   */
/* 0x54400 : MODE = 2, 8 buffers, TW = 1024 sampling clock cycles   */
/* 0x74400 : MODE = 2, 8 buffers, TW = 1024 sampling clock cycles, Sparsified Data   */

  rcode = O_TRCFG;
  rdata = 0x74400;
  ret = U2F_Reg_Write(rcode, rdata);
  if(ret) rcc_error_print(stdout, ret);


/*************************/
/* TRIGGER CONFIGURATION */
/*************************/

  ret = U2F_Reg_Read(O_TRCFG, &rdata); 
  ret = U2F_TRCFG_decode(rdata, &trcfg_s);
  printf("TRIGGER CONFIGURATION TYPE = %x\n", rdata);
  printf("\nDecoding TRCFG register\n");
  printf("The system waits %d clock cycles after arrival of L1 trigger and issuing of L2 trigger\n", trcfg_s.tw);
  printf("Buffer mode		     : %s\n", trcfg_s.bmd ? "8 buffers" : "4 buffers");
  printf("Trigger mode		     : ");
  if (trcfg_s.mode == 0) printf("Software trigger\n");	  
  if (trcfg_s.mode == 1) printf("Out of range\n");
  if (trcfg_s.mode == 2) printf("External L1 and automatic L2 after TW\n");
  if (trcfg_s.mode == 3) printf("External L1 and L2\n");
  printf("There are %d buffers remaining in the multi event buffer\n",trcfg_s.remb);
  printf("Multi event buffer empty     : %s\n",trcfg_s.empty ? "True" : "False");
  printf("Multi event buffer full      : %s\n",trcfg_s.full ? "True" : "False");
  printf("The read pointer position is : %d\n",trcfg_s.rd_pt); 
  printf("The write pointer position is: %d\n",trcfg_s.wr_pt); 

/****************/
/* ERROR STATUS */
/****************/

  printf("\n***********************************************************\n");
  rcode = O_ERRST;
  ret = U2F_Reg_Read(rcode, &rdata); 
  ret = U2F_ERRST_decode(rdata, &errst_s);

  if(rdata & 0x1F) 
  {
    printf("ERROR INFO = %x\n", rdata);
    printf("\nDecoding ERRST register\n");
    printf("Pattern error is: %s\n",errst_s.pattern_error ? "True" : "False");
    printf("Abort is	 : %s\n", errst_s.abort ? "True" : "False");
    printf("Time-out is	 : %s\n", errst_s.timeout ? "True" : "False");
    printf("Altro error is   : %s\n", errst_s.altro_error ? "True" : "False");
    printf("\n***********************************************************\n");
  }

  rcode = O_EVWORD;
  ret = U2F_Reg_Read(rcode, &rdata); 
  printf("EVWORD in the Beginning = %x\n", rdata);






/* NEW */

  /* allocate input buffer */
  rcuDataInBuf = (unsigned int *) malloc(RCU_BUFFER_SIZE);
  if(rcuDataInBuf <= 0)
  {
    printf(" Error allocating RCU Buffer\n");
    exit(0);
  }
  bzero(rcuDataInBuf,RCU_BUFFER_SIZE);

  rcuDataCount = 0;
  rcuDataMaxWords = 2;

  bonusevents = 0;
  nphysevents = 0;
  oldevnb = 0;
  ntpc = 0;
  ntpch = 0;
  begin = 0;
  delta_t_average = 0.;
  delta_n_average = 0.;
  delta_nt_average = 0.;

  banknumber = 0;
#ifdef TEST_SETUP
  banknumber = 3;
#else
  if(rol->pid==31) banknumber = 1;
  if(rol->pid==27) banknumber = 2;
#endif

#endif



  printf("INFO: User Prestart Executed\n");

  /* from parser */
  if(__the_event__) WRITE_EVENT_;
  *(rol->nevents) = 0;
  rol->recNb = 0;

  return;
}       

static void
__pause()
{
  CDODISABLE(VME,1,0);

  printf("INFO: User Pause Executed\n");

  /* from parser */
  if(__the_event__) WRITE_EVENT_;

  return;
}

static void
__go()
{  
  unsigned short value, value0, value1, array0[32], array1[32];
  int i, ii;
#ifdef BONUS
  int ret;
#endif

  printf("INFO: User Go ...\n");

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

#ifdef BONUS
  ret = U2F_ReadOut(mdsize, &retBytes, (u_char *)rcuDataInBuf,M_FIRST);
  printf("first[%5d] ret=%d, retBytes=%d\n",*(rol->nevents),ret,retBytes);
#endif

  CDOENABLE(VME,1,0);
  printf("INFO: User Go: enabled\n");fflush(stdout);

  /* from parser */
  if(__the_event__) WRITE_EVENT_;

  return;
}

static void
__end()
{
#ifdef BONUS
  unsigned long ret;
#endif

printf("0\n");fflush(stdout);
  CDODISABLE(VME,1,0);
printf("1\n");fflush(stdout);


#ifdef BONUS

  /* =NULL happened if 'end' or 'exit' transition was initiated
  right after 'download' */
 if(rcuDataInBuf!=NULL)
 {
  /* tells system it is last buffer ??? we did not do it in June .. */
printf("2: mdsize=%d, rcuDataInBuf=0x%08x\n",mdsize,rcuDataInBuf);fflush(stdout);
  memset(rcuDataInBuf, 0, mdsize);
printf("3\n");fflush(stdout);
  printf("call U2F_ReadOut(..,M_LAST) ..\n");
  ret = U2F_ReadOut(mdsize, &osize, (u_char *)rcuDataInBuf, M_LAST);
  printf("U2F_ReadOut(..,M_LAST) returns %d\n",ret);

  /* do not need it any more ??
  rcode = O_TRCFG;
  rdata = 0x00000;
  ret = U2F_Reg_Write(rcode, rdata);
  */
 
/********************/
/* delay 1000000 us */
/********************/

  /*ts_delay(1000000);*/
  usleep(1000000);



/* If there is another event pending in the FECs, clear it out */

  rcode = O_EVWORD;
  ret = U2F_Reg_Read(rcode, &rdata); 
  printf("ending... evword= %x\n",rdata);

  printf("111\n");
  ret = U2F_Close();
  printf("222\n");
  /*
  ret = ts_close(TS_DUMMY);
  */

  if(rcuDataInBuf)
  {
    printf("Freeing RCU Buffer\n");
    free(rcuDataInBuf);
    printf("RCU Buffer freed\n");
    rcuDataInBuf = 0;
  }
 }
#endif


  printf("INFO: User End Executed\n");fflush(stdout);


  /* from parser */
  if(__the_event__) WRITE_EVENT_;

  return;
}

void
__done()
{
  /* from parser */
  poolEmpty = 0; /* global Done, Buffers have been freed */

  /* Acknowledge tir register */
  CDOACK(VME,1,0);

  return;
}

static void
__status()
{
  return;
}  




