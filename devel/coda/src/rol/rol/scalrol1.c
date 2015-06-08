
/* scalrol1.c - first readout list for scaler VME crates */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

#define MIN(x,y) ((x) < (y) ? (x) : (y))

/*****************************/
/* former 'crl' control keys */

/* readout list SCALROL1 */
#define ROL_NAME__ "SCALROL1"

/* ts control */
#define TRIG_SUPV

/* name used by loader */
#ifdef SCALER1
#define INIT_NAME scaler1__init
#endif
#ifdef SCALER2
#define INIT_NAME scaler2__init
#endif
#ifdef SCALER3
#define INIT_NAME scaler3__init
#endif
#ifdef SCALER4
#define INIT_NAME scaler4__init
#endif
#ifdef POLAR1
#define INIT_NAME moller__init

#define USE_INTERRUPTS
#endif


#include "rol.h"

/* vme readout */
#include "VME_source.h" /* POLLING_MODE for tir_triglib.h undefined here */


/* user code */

#include "uthbook.h"
#include "coda.h"
#include "tt.h"
#include "scaler7201.h"
#include "adc792.h"
#include "tdc890.h"



#ifdef SCALER1
#define TIRADR  0x0ea0  /* main TI board address */
/* Run_number and event_number */
#define RUN_NUMBER rol->runNumber
#define EVENT_NUMBER *(rol->nevents)
#endif
#ifdef SCALER2
#define TIRADR  0x0ec0  /* main TI board address */
#endif
#ifdef SCALER3
#define TIRADR  0x0ec0  /* main TI board address */
#endif
#ifdef SCALER4
#define TIRADR  0x0ec0  /* main TI board address */
#endif
#ifdef POLAR1
#define TIRADR  0x0ed0  /* main TI board address */

/*LeCroy ADCs*/
#define ADCADR1 0x140000

/* interrupt vector for Struck scaler (f0) */
#define INTVECTOR  0xf0

/* interrupt vector for second TI board */
#define INTVECTOR2 0xf8

/* interrupt level (TI board has level 5) */
/* lets use higher level then TI; in scaler7201handler(void) we'll disable
that level in the begining and enable in the end; when level 5 was used
polar seems missing some events on high rate ... */ 
#define INTLEVEL  6

/* interrupt level for seconf TI board */
#define INTLEVEL2  6

#define LSCALER0 0x508000
#define LSCALER1 0x303000
#define MASK    0x00000000   /* unmask all 32 channels (0-enable,1-disable) */
#define NCHN            32
#define NBUFHLS      72000

#define MYBUFSIZE (MAX_EVENT_LENGTH/2) /* set it to the half of MAX_EVENT_LENGTH */
#endif

/* DO NOT FORGET TO CHANGE THIS IF CHANGING 'maximum #####,###' ABOVE !!!
#define BUFSIZE_IN_BYTES1 MAX_EVENT_LENGTH
#define NUM_BUFFERS1  MAX_EVENT_POOL
*/


/* prototypes */
int sysBusToLocalAdrs();

/* global board offset */
static unsigned long offset;

/* standard software scalers (RCST) */
static unsigned long down_count=0;
static unsigned long pre_count=0;
static unsigned long go_count=0;

static unsigned long trig_count=0;
static unsigned long event_count=0;

static unsigned long run_event_count=0;
static unsigned long run_trig_count=0;

static unsigned long phys_count=0;
static unsigned long phys_sync_count=0;
static unsigned long run_phys_sync_count=0;
static unsigned long run_phys_count=0;
static unsigned long sync_count=0;
static unsigned long run_sync_count=0;
static unsigned long illegal_count=0;
static unsigned long run_illegal_count=0;
static unsigned long pause_count=0;
static unsigned long end_count=0;

/* for controlling scaler reset */
static unsigned long run_go_count=0;

#ifdef SCALER1

/* livetime calculations */
static unsigned int ungated1, gated1, ungated1_old, gated1_old;
static unsigned int ungated2, gated2, ungated2_old, gated2_old;


/* sync status */
static unsigned long sync_status=0;

/* defined in scalers.c downloaded at boot time...
   eventually should be table-driven */
extern int trscal[];
extern int ntrscal;

/* for trgs overflow counts */
/*
unsigned long *last_trgs;
unsigned long *trgs_overflow;
*/
unsigned long last_trgs[480];
unsigned long trgs_overflow[480];

#endif

#ifdef SCALER2
/* defined in scalers.c downloaded at boot time...eventually
   should be table-driven */
extern int ecscal[];
extern int necscal;
extern int scscal[];
extern int nscscal;
#endif

#ifdef SCALER3
/* defined in scalers.c downloaded at boot time...eventually
   should be table-driven */
extern int ccscal[];
extern int nccscal;
extern int stscal[];
extern int nstscal;
#endif

#ifdef SCALER4
/* defined in scalers.c downloaded at boot time...eventually
   should be table-driven */
extern int tgscal[];
extern int ntgscal;
#endif

#ifdef POLAR1
/* global addresses for boards */
volatile static unsigned long scaler0, scaler1;

/* user buffer pointers - all NULL at initialization */
extern int *userbuffers[NUSERBUFFERS];

static int *ring0, *ring1;
static int nHLS;
static int lenHLS0 = ((((MYBUFSIZE/4) - 1024) / 2) / NCHN) * NCHN;
static int lenHLS1 = ((((MYBUFSIZE/4) - 1024) / 2) / NCHN) * NCHN;
static int printRingFull = 1;
#endif

static char *rcname = "RC00";

/* pointer to TTS structures for current roc */
extern TTSPtr ttp;


/* time profiling: 0-OFF, 1-ON */
static int timeprofile = 1;

/* histogram area */
static UThisti histi[NHIST];
static neventh = 0;


#ifdef POLAR1
#ifdef USE_INTERRUPTS


int
scaler7201readHLS111(int addr, int *ring, int counter)
{
  int k, dataword, ret;

  /*logMsg("scaler7201readHLS111 reached, addr=0x%08x\n",(int)addr,0,0,0,0,0);*/
  ret = 0;
  k = 0;
  while( !(scaler7201status(addr) & FIFO_EMPTY))
  {
    dataword = scaler7201readfifo(addr);
    k++;
    if(!(k%16)) /* put nHLS in every 16th channel */
    {
      dataword = ( dataword & 0xff000000 ) + (counter & 0xffffff);
    }
    /* write dataword to the ring buffer */
    if(utRingWriteWord(ring, dataword) < 0)
    {
      /*logMsg("scaler7201readHLS111: ERROR: ring0 full\n",0,0,0,0,0,0);*/
      ret = -1;
    }
  }

if(k!=64) logMsg("scaler7201readHLS111: addr=0x%08x k=%d\n",addr,k,3,4,5,6);

  return(ret);
}



void
scaler7201handler(void)
{
  unsigned int mask0, ret;

  sysIntDisable(INTLEVEL);
  /*logMsg("scaler7201handler: reached\n",0,0,0,0,0,0);*/

  scaler7201intSave(scaler0, &mask0);





  nHLS ++; /* increment interrupt counter */




  /* read scaler0 */
  if(scaler7201status(scaler0) & FIFO_FULL)
  {
    scaler7201restore(scaler0, MASK);
    scaler7201restore(scaler1, MASK);
    scaler7201control(scaler0, ENABLE_EXT_NEXT);
    scaler7201control(scaler1, ENABLE_EXT_NEXT);
  }
  else
  {
    ret = scaler7201readHLS111(scaler0, ring0, nHLS);
    if(ret==0) printRingFull = 1;
    else if(ret==-1 && printRingFull==1)
    {
      logMsg("scaler7201handler: ring0 is full\n",0,0,0,0,0,0);
      printRingFull = 0;
    }
  }




  /* read scaler1 */
  if(scaler7201status(scaler1) & FIFO_FULL)
  {
    scaler7201restore(scaler0, MASK);
    scaler7201restore(scaler1, MASK);
    scaler7201control(scaler0, ENABLE_EXT_NEXT);
    scaler7201control(scaler1, ENABLE_EXT_NEXT);
  }
  else
  {
    ret = scaler7201readHLS111(scaler1, ring1, nHLS);
    if(ret==0) printRingFull = 1;
    else if(ret==-1 && printRingFull==1)
    {
      logMsg("scaler7201handler: ring1 is full\n",0,0,0,0,0,0);
      printRingFull = 0;
    }
  }


  scaler7201intRestore(scaler0, mask0);

  sysIntEnable(INTLEVEL);
  return;
}
#endif
#endif

/* standart functions */

static void
__download()
{
  unsigned long tir_offset, ii;

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");
  

  down_count++;

  /* Must check address in TI module */
  /* TI uses A16 (0x29, 4 digits), not A24 */

  /* get global offset for A16 */
  sysBusToLocalAdrs(0x29,0,&tir_offset);
  tir[1] = (struct vme_tir *)(tir_offset+TIRADR);

  /*  get global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&offset);

#ifdef POLAR1
  scaler0 = offset + LSCALER0;
  scaler1 = offset + LSCALER1;
  printf("\npolar rol1: lenHLS0 = %d,   lenHLS1 = %d\n",lenHLS0,lenHLS1);
  printf("\npolar rol1: scaler0 = 0x%08x  scaler1 = 0x%08x\n",scaler0,scaler1);

  scaler7201control(scaler0, DISABLE_EXT_NEXT);
  scaler7201control(scaler1, DISABLE_EXT_NEXT);

  if(userbuffers[0] == NULL)
  {
    printf("allocate %d bytes for userbuffers[0]\n",NBUFHLS);
    userbuffers[0] = utRingAlloc(NBUFHLS);
  }
  ring0 = userbuffers[0];

  if(userbuffers[1] == NULL)
  {
    printf("allocate %d bytes for userbuffers[1]\n",NBUFHLS);
    userbuffers[1] = utRingAlloc(NBUFHLS);
  }
  ring1 = userbuffers[1];
#endif

  logMsg("INFO: User Download Executed\n",1,2,3,4,5,6);
}

      
static void
__prestart()
{
  char *env;
  char *histname = "ROL1 RC00";
  unsigned short value, value0, value1, array0[32], array1[32], data16;
  int i, ii, ifull;
  short buf;
  unsigned short slot, channel, pattern[8];

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  /* init trig source VME */
  VME_INIT;

  /* Register an async trigger source, (up to 32 sources) */
  /* CTRIGRSA(source,code,handler,done)*/
  /* link async trig source VME 1 to usrtrig and usrtrig_done */
  CTRIGRSA(VME, 1, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */

  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);

  pre_count++;

  /* can't reset run_go_count in GO, so must do it here */
  run_go_count=0;

#ifdef POLAR1
  scaler7201control(scaler0, DISABLE_EXT_NEXT);
  scaler7201control(scaler1, DISABLE_EXT_NEXT);

  printf("\npolar rol1: lenHLS0 = %d,   lenHLS1 = %d\n",lenHLS0,lenHLS1);
  tttest("\npolar rol1:");
#endif

  if( (env=getenv("PROFILE")) != NULL )
  {
    if(*env == 'T')
    {
      timeprofile = 1;
      logMsg("rol1: time profiling is ON\n",1,2,3,4,5,6);
    }
    else
    {
      timeprofile = 0;
      logMsg("rol1: time profiling is OFF\n",1,2,3,4,5,6);
    }
  }
  if(timeprofile)
  {
    sprintf((char *)&histname[7],"%02d",rol->pid);
    uthbook1(histi, 1000+rol->pid, histname, 200, 0, 200);
  }

  printf("INFO: User Prestart Executed\n");

  /* from parser */
  *(rol->nevents) = 0;
  rol->recNb = 0;

  return;
}       

static void
__pause()
{
#ifdef POLAR1
  scaler7201control(scaler0, DISABLE_EXT_NEXT);
  scaler7201control(scaler1, DISABLE_EXT_NEXT);
#ifdef USE_INTERRUPTS
  scaler7201intdisable(scaler0);
#endif
#endif
  CDODISABLE(VME,1,0);

  pause_count++;

  logMsg("INFO: User Pause Executed\n",1,2,3,4,5,6);

  return;
}


static void
__go()
{  
  unsigned short value, value0, value1, array0[32], array1[32];
  int i, ii;
  volatile unsigned short *p;

  logMsg("INFO: User Go ...\n",1,2,3,4,5,6);

  go_count++;
  run_go_count++;

#ifdef SCALER1

  gated1 = 0;
  ungated1 = 0;
  gated1_old = 0;
  ungated1_old = 0;
  gated2 = 0;
  ungated2 = 0;
  gated2_old = 0;
  ungated2_old = 0;

  /* setup trigger latch card, hardware and software scalers
	 on first go transition only */
  if(run_go_count==1)
  {
    run_trig_count=0;	
    run_phys_count=0;	
    run_sync_count=0;	
    run_illegal_count=0;
    run_phys_sync_count=0;


    for(i=0; i<ntrscal; i++) reset_caen_scaler(offset+trscal[i]);

    /* allocate storage for trgs overflow banks */
/*
    last_trgs     = (unsigned long *)malloc(ntrscal*16*sizeof(unsigned long));
    trgs_overflow = (unsigned long *)malloc(ntrscal*16*sizeof(unsigned long));
*/
    for(i=0; i<ntrscal*16; i++) last_trgs[i]=0;
    for(i=0; i<ntrscal*16; i++) trgs_overflow[i]=0;
  }
#endif

#ifdef SCALER2
  /* reset hardware and software scalers on first go transition only */
  if (run_go_count==1)
  {
    run_trig_count=0;	
    run_event_count=0;	
    run_sync_count=0;	
    run_illegal_count=0;
    run_phys_sync_count=0;
    for(i=0; i<necscal; i++) reset_caen_scaler(offset+ecscal[i]);
    for(i=0; i<nscscal; i++) reset_caen_scaler(offset+scscal[i]);
  }
#endif

#ifdef SCALER3
  /* reset hardware and software scalers on first go transition only */
  if (run_go_count==1)
  {
    run_trig_count=0;	
    run_event_count=0;	
    run_sync_count=0;	
    run_illegal_count=0;
    run_phys_sync_count=0;
    for(i=0; i<nstscal; i++) reset_caen_scaler(offset+stscal[i]);
  } 
#endif

#ifdef SCALER4
  /* reset hardware and software scalers on first go transition only */
  if (run_go_count==1)
  {
    run_trig_count=0;	
    run_event_count=0;	
    run_sync_count=0;	
    run_illegal_count=0;
    run_phys_sync_count=0;
    for(i=0; i<ntgscal; i++) reset_caen_scaler(offset+tgscal[i]);
  } 
#endif

#ifdef POLAR1
  run_trig_count=0;	

  scaler7201control(scaler0, DISABLE_EXT_NEXT);
  scaler7201reset(scaler0);
  scaler7201clear(scaler0);
  scaler7201mask(scaler0, MASK);
  scaler7201enablenextlogic(scaler0);
  scaler7201control(scaler0, ENABLE_MODE2);
  scaler7201control(scaler0, ENABLE_EXT_DIS);

  scaler7201control(scaler1, DISABLE_EXT_NEXT);
  scaler7201reset(scaler1);
  scaler7201clear(scaler1);
  scaler7201mask(scaler1, MASK);
  scaler7201enablenextlogic(scaler1);
  scaler7201control(scaler1, ENABLE_MODE2);
  scaler7201control(scaler1, ENABLE_EXT_DIS);

  /*sergey 14-may-2009 moved to the trigger routine trying to avoid crashes
	on the very first event
  scaler7201control(scaler0, ENABLE_EXT_NEXT);
  scaler7201control(scaler1, ENABLE_EXT_NEXT);
  */



  utRingInit(ring0); /* cleanup HLS0 buffer */
  utRingInit(ring1); /* cleanup HLS1 buffer */
  nHLS    = 0;


#ifdef USE_INTERRUPTS
  utIntInit((VOIDFUNCPTR)scaler7201handler,0,INTLEVEL,INTVECTOR);
#endif

  /*scaler7201intenable(scaler0,0x4,INTLEVEL,INTVECTOR);*/
  /* if SIS scaler, use "ALMOST FULL" condition */
  /*scaler7201intenable(scaler0, 0x8,INTLEVEL,INTVECTOR);*/
#endif

  CDOENABLE(VME,1,0);

#ifdef POLAR1
#ifdef USE_INTERRUPTS
  scaler7201intenable(scaler0,0x1,INTLEVEL,INTVECTOR);
#endif
#endif

  return;
}



void
usrtrig(unsigned long EVTYPE, unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
  TIMER_VAR;
  int *adrlen, *bufin, *bufout, ind, ind2, ncol, nrow, len, len1, len2, len12;
  int ncol1, ncol2;
  volatile unsigned short *p,*p1,*p2;
  unsigned short latch1=0;
  unsigned short latch2=0;
  long *jw, *buf, *buf1, *secondword;	
  long i, ii;
  int word_count;
  long psync;
  unsigned long *spt;
  unsigned short *buf2;
  unsigned int livetime1, gated1_delta, ungated1_delta;
  unsigned int livetime2, gated2_delta, ungated2_delta;
  unsigned int ret;

  trig_count++;	
  run_trig_count++;	


#ifdef POLAR1
  if(run_trig_count==1)
  {
    scaler7201control(scaler0, ENABLE_EXT_NEXT);
    scaler7201control(scaler1, ENABLE_EXT_NEXT);
    logMsg("Enables 7201s\n",1,2,3,4,5,6);
  }
#endif


  rol->dabufp = (long *) 0;

  /*open event type EVTYPE of BT_UI4*/
  CEOPEN(EVTYPE, BT_UI4);

  jw = rol->dabufp;

  /* at that moment only second CODA world defined  */
  /* first CODA world (length) undefined, so set it */
  jw[ILEN] = 1;

  secondword = rol->dabufp - 1; /* pointer to the second CODA header word */

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

  /* for normal events send latch words, count l1 bits, etc.              */
  /* for sync events send latches and all scalers (hardware and software) */

  if((syncFlag<0)||(syncFlag>1))         /* illegal */
  {
    illegal_count++;
    run_illegal_count++;
    psync=0x0;
  }
  else if((EVTYPE==0)&&(syncFlag==0))    /* illegal */
  {
    illegal_count++;
    run_illegal_count++;
    psync=0x0;
  }
  else if((EVTYPE==0)&&(syncFlag==1))    /* force_sync events */
  {
    sync_count++;
    run_sync_count++;
    psync=0x0;

#ifdef SCALER1
    /* CAEN hardware scaler bank, no reset */
    ind2 = bosMopen_(jw,"TRGS",0,ntrscal*16,1);
    buf = (long *)&jw[ind2+1];
    spt = (long *)&jw[ind2+1];
    word_count=0;
    for(i=0; i<ntrscal; i++)
    {
	  /*logMsg("befor: 0x%08x\n",buf,2,3,4,5,6);*/
      read_caen_scaler(offset+trscal[i],buf);
	  /*logMsg("after: 0x%08x\n",buf,2,3,4,5,6);*/

	  /* save scalers for livetime calculations */
      if(i==1)
      {
        ungated1_old = ungated1;
        ungated1 = buf[0];
        ungated2_old = ungated2;
        ungated2 = buf[1];
	  }
      if(i==2)
      {
        gated1_old = gated1;
        gated1 = buf[0];
        gated2_old = gated2;
        gated2 = buf[1];
	  }

      word_count += 16;
      buf += 16;
    }
    rol->dabufp += bosMclose_(jw,ind2,word_count,1);

	
    ungated1_delta = ungated1 - ungated1_old;
    gated1_delta = gated1 - gated1_old;
    ungated2_delta = ungated2 - ungated2_old;
    gated2_delta = gated2 - gated2_old;
    if(ungated1_delta > 0) livetime1 = (gated1_delta*100)/ungated1_delta;
    else            livetime1 = 200;
    if(ungated2_delta > 0) livetime2 = (gated2_delta*100)/ungated2_delta;
    else            livetime2 = 200;
    logMsg("live time: clock_based = %d percent FC_based = %d percent\n",
      livetime1,livetime2,3,4,5,6);
	/*
    logMsg("  (gated=%d, gated_old=%d, gated_delta=%d, ungated=%d, ungated_old=%d, ungated_delta=%d)\n\n",
      gated,gated_old,gated_delta,ungated,ungated_old,ungated_delta);
	*/


    /* count trgs overflows */
    for(i=0; i<ntrscal*16; i++)
    {
      if(*(spt+i)<last_trgs[i]) trgs_overflow[i]++;
      last_trgs[i]=*(spt+i);
    }


    /* inject trgs overflow bank */
    ind2 = bosMopen_(jw,"TRGS",1,ntrscal*16,1);
    buf = (long *)&jw[ind2+1];
    for(i=0; i<ntrscal*16; i++) *(buf+i)=trgs_overflow[i];
    word_count=ntrscal*16;
    rol->dabufp += bosMclose_(jw,ind2,word_count,1);


    /* roc status bank */
    ind2 = bosMopen_(jw,"RCST",rol->pid,15,1);
    buf = (long *)&jw[ind2+1];

    *buf++=down_count;
    *buf++=pre_count;
    *buf++=go_count;
    *buf++=trig_count;
    *buf++=phys_count;
    *buf++=sync_count;
    *buf++=run_trig_count;
    *buf++=run_phys_count;
    *buf++=run_sync_count;
    *buf++=pause_count;
    *buf++=end_count;
    *buf++=illegal_count;
    *buf++=run_illegal_count;
    *buf++=phys_sync_count;
    *buf++=run_phys_sync_count;

    rol->dabufp += bosMclose_(jw,ind2,15,1);
#endif

#ifdef SCALER2
    /* scaler banks */

    ind2 = bosMopen_(jw,"ECS ",0,96,1);
    buf = (long *)&jw[ind2+1];
    word_count=0;
    for(i=0; i<necscal; i++)
    {
      read_caen_scaler(offset+ecscal[i],buf);
      word_count += 16;
      buf += 16;
    }
    rol->dabufp += bosMclose_(jw,ind2,word_count,1);


    /* scaler banks */
    ind2 = bosMopen_(jw,"SCS ",0,192,1);
    buf = (long *)&jw[ind2+1];
    word_count=0;
    for(i=0; i<nscscal; i++)
    {
      read_caen_scaler(offset+scscal[i],buf);
      word_count+=16;
      buf += 16;
    }
    rol->dabufp += bosMclose_(jw,ind2,word_count,1);


    /* reset scalers */
    for(i=0; i<necscal; i++) reset_caen_scaler(offset+ecscal[i]);
    for(i=0; i<nscscal; i++) reset_caen_scaler(offset+scscal[i]);


    /* roc status bank */
    ind2 = bosMopen_(jw,"RCST",rol->pid,15,1);
    buf = (long *)&jw[ind2+1];

    *buf++=down_count;
    *buf++=pre_count;
    *buf++=go_count;
    *buf++=trig_count;
    *buf++=event_count;
    *buf++=sync_count;
    *buf++=run_trig_count;
    *buf++=run_event_count;
    *buf++=run_sync_count;
    *buf++=pause_count;
    *buf++=end_count;
    *buf++=illegal_count;
    *buf++=run_illegal_count;
    *buf++=phys_sync_count;
    *buf++=run_phys_sync_count;

    rol->dabufp += bosMclose_(jw,ind2,15,1);
#endif

#ifdef SCALER3
    /* STS scaler bank */
    ind2 = bosMopen_(jw,"STSN",0,16,1);
    buf = (long *)&jw[ind2+1];
    word_count=0;
    for(i=0; i<nstscal; i++)
    {
      read_caen_scaler(offset+stscal[i],buf);
      word_count+=16;
      buf += 16;
    }
    rol->dabufp += bosMclose_(jw,ind2,word_count,1);



    /* reset scalers */
    for(i=0; i<nstscal; i++) reset_caen_scaler(offset+stscal[i]);


    /* roc status bank */
    ind2 = bosMopen_(jw,"RCST",rol->pid,15,1);
    buf = (long *)&jw[ind2+1];

    *buf++=down_count;
    *buf++=pre_count;
    *buf++=go_count;
    *buf++=trig_count;
    *buf++=event_count;
    *buf++=sync_count;
    *buf++=run_trig_count;
    *buf++=run_event_count;
    *buf++=run_sync_count;
    *buf++=pause_count;
    *buf++=end_count;
    *buf++=illegal_count;
    *buf++=run_illegal_count;
    *buf++=phys_sync_count;
    *buf++=run_phys_sync_count;

    rol->dabufp += bosMclose_(jw,ind2,15,1);
#endif

#ifdef SCALER4
    /* TGS scaler bank */
    ind2 = bosMopen_(jw,"TGS ",0,192,1);
    buf = (long *)&jw[ind2+1];
    word_count=0;
    for(i=0; i<ntgscal; i++)
    {
      read_caen_scaler(offset+tgscal[i],buf);
      word_count+=16;
      buf += 16;
    }
    rol->dabufp += bosMclose_(jw,ind2,word_count,1);

    /* reset scalers */
    for(i=0; i<ntgscal; i++) reset_caen_scaler(offset+tgscal[i]);


    /* stat bank */
    ind2 = bosMopen_(jw,"RCST",rol->pid,15,1);
    buf = (long *)&jw[ind2+1];

    *buf++=down_count;
    *buf++=pre_count;
    *buf++=go_count;
    *buf++=trig_count;
    *buf++=event_count;
    *buf++=sync_count;
    *buf++=run_trig_count;
    *buf++=run_event_count;
    *buf++=run_sync_count;
    *buf++=pause_count;
    *buf++=end_count;
    *buf++=illegal_count;
    *buf++=run_illegal_count;
    *buf++=phys_sync_count;
    *buf++=run_phys_sync_count;

    rol->dabufp += bosMclose_(jw,ind2,15,1);
#endif

#ifdef POLAR1
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
#endif

  }
  else if((syncFlag==0)&&(EVTYPE==15)) /* helicity strob events */
  {
    ;
#ifdef POLAR1

    /******************/
    /* scaler readout */
    /******************/

#ifdef USE_INTERRUPTS  /* interrupt readout*/

    len1 = utRingGetNwords(ring0);
    len2 = utRingGetNwords(ring1);
    if(len1!=len2) logMsg("ERROR: len1=%d != len2=%d\n",len1,len2,0,0,0,0);
    len12 = MIN(len1,len2);
	/*
    if(len1!=64||len2!=64) logMsg("WARN: len1=%d len2=%d (expect both 64 !!??)\n",len1,len2,0,0,0,0);
	*/
    if(len12 > lenHLS0)
    {
	  logMsg("WARN: len12=%d > lenHLS0=%d\n",len12,lenHLS0,0,0,0,0);
len12 = lenHLS0 - 64;
logMsg("INFO: set len12=%d\n",len12,0,0,0,0,0);
goto a321;
      rol->dabufp += bosMmsg_(jw,"ERRO",rol->pid,"len1 > lenHLS0 -> cleanup buffers.");
      utRingInit(ring0); /* cleanup HLS0 buffer */
      utRingInit(ring1); /* cleanup HLS1 buffer */
	}
    else if(len12 < 0)
    {
      logMsg("ERROR: rol1: len1=%d len2=%d\n",len1,len2,3,4,5,6);
    }
    else if(len12 == 0)
    {
      logMsg("ERROR: rol1: len1=%d len2=%d\n",len1,len2,3,4,5,6);
    }
    else
	{

a321:

      ncol = NCHN; /* the number of columns in both HLS and HLB banks */
      ncol1 = NCHN/2; /* the number of columns in HLS bank */
      ncol2 = NCHN/2; /* the number of columns in HLB bank */
      nrow = len12/ncol;
	  /*logMsg("ncol=%d nrow=%d\n",ncol,nrow,0,0,0,0);*/




      /* create HLS #0 bank */
      if( (ind = bosMopen_(jw,"HLS ",0,ncol1,nrow)) > 0)
      {
        buf = (long *)&jw[ind+1]; /* save data area address */
        rol->dabufp += bosMclose_(jw,ind,ncol,nrow);
      }
      else
      {
        rol->dabufp += bosMmsg_(jw,"ERRO",rol->pid,"bosMopen_() returns error1.");
      }

      /* create HLB #0 bank */
      if( (ind = bosMopen_(jw,"HLB ",0,ncol2,nrow)) > 0)
      {
        buf1 = (long *)&jw[ind+1]; /* save data area address */
        rol->dabufp += bosMclose_(jw,ind,ncol,nrow);
      }
      else
      {
        rol->dabufp += bosMmsg_(jw,"ERRO",rol->pid,"bosMopen_() returns error1.");
      }




      /* fill both banks */
      for(i=0; i<(len12/NCHN); i++)
      {
        for(ii=0; ii<ncol1; ii++) *buf++ = utRingReadWord(ring0);
        for(ii=0; ii<ncol2; ii++) *buf1++ = utRingReadWord(ring0);
	  }





      /* create HLS #1 bank */
      if( (ind = bosMopen_(jw,"HLS ",1,ncol1,nrow)) > 0)
      {
        buf = (long *)&jw[ind+1]; /* save data area address */
        rol->dabufp += bosMclose_(jw,ind,ncol,nrow);
      }
      else
      {
        rol->dabufp += bosMmsg_(jw,"ERRO",rol->pid,"bosMopen_() returns error1.");
      }

      /* create HLB #1 bank */
      if( (ind = bosMopen_(jw,"HLB ",1,ncol2,nrow)) > 0)
      {
        buf1 = (long *)&jw[ind+1]; /* save data area address */
        rol->dabufp += bosMclose_(jw,ind,ncol,nrow);
      }
      else
      {
        rol->dabufp += bosMmsg_(jw,"ERRO",rol->pid,"bosMopen_() returns error1.");
      }




      /* fill both banks */
      for(i=0; i<(len12/NCHN); i++)
      {
        for(ii=0; ii<ncol1; ii++) *buf++ = utRingReadWord(ring1);
        for(ii=0; ii<ncol2; ii++) *buf1++ = utRingReadWord(ring1);
	  }

    }



#else  /* non-interrupt readout*/




	/*sergey*/

    nHLS ++; /* increment readout counter */

    /* read scaler0 */
    if(scaler7201status(scaler0) & FIFO_FULL)
    {
      scaler7201restore(scaler0, MASK);
      scaler7201restore(scaler1, MASK);
      scaler7201control(scaler0, ENABLE_EXT_NEXT);
      scaler7201control(scaler1, ENABLE_EXT_NEXT);
    }
    else
    {
      ret = scaler7201readHLS111(scaler0, ring0, nHLS);
      if(ret==0) printRingFull = 1;
      else if(ret==-1 && printRingFull==1)
      {
        logMsg("scaler7201handler: ring0 is full\n",0,0,0,0,0,0);
        printRingFull = 0;
      }
    }

    /* read scaler1 */
    if(scaler7201status(scaler1) & FIFO_FULL)
    {
      scaler7201restore(scaler0, MASK);
      scaler7201restore(scaler1, MASK);
      scaler7201control(scaler0, ENABLE_EXT_NEXT);
      scaler7201control(scaler1, ENABLE_EXT_NEXT);
    }
    else
    {
      ret = scaler7201readHLS111(scaler1, ring1, nHLS);
      if(ret==0) printRingFull = 1;
      else if(ret==-1 && printRingFull==1)
      {
        logMsg("scaler7201handler: ring1 is full\n",0,0,0,0,0,0);
        printRingFull = 0;
      }
    }






    len1 = utRingGetNwords(ring0);
    len2 = utRingGetNwords(ring1);
    len12 = MIN(len1,len2);
	/*logMsg("len1=%d len2=%d\n",len1,len2,0,0,0,0);*/

    if(len12 > lenHLS0)
    {
	  logMsg("WARN: len12=%d > lenHLS0=%d\n",len12,lenHLS0,0,0,0,0);
      rol->dabufp += bosMmsg_(jw,"ERRO",rol->pid,"len1 > lenHLS0 -> cleanup buffers.");
      utRingInit(ring0); /* cleanup HLS0 buffer */
      utRingInit(ring1); /* cleanup HLS1 buffer */
	}
    else if(len12 < 0)
    {
      logMsg("ERROR: rol1: len1=%d len2=%d\n",len1,len2,3,4,5,6);
    }
    else if(len12 == 0)
    {
      /*logMsg("ERROR: rol1: len1=%d len2=%d\n",len1,len2,3,4,5,6)*/;
    }
    else
	{

      ncol = NCHN; /* the number of columns in both HLS and HLB banks */
      ncol1 = NCHN/2; /* the number of columns in HLS bank */
      ncol2 = NCHN/2; /* the number of columns in HLB bank */
      nrow = len12/ncol;
	  /*logMsg("ncol=%d nrow=%d\n",ncol,nrow,0,0,0,0);*/




      /* create HLS #0 bank */
      if( (ind = bosMopen_(jw,"HLS ",0,ncol1,nrow)) > 0)
      {
        buf = (long *)&jw[ind+1]; /* save data area address */
        rol->dabufp += bosMclose_(jw,ind,ncol,nrow);
      }
      else
      {
        rol->dabufp += bosMmsg_(jw,"ERRO",rol->pid,"bosMopen_() returns error1.");
      }

      /* create HLB #0 bank */
      if( (ind = bosMopen_(jw,"HLB ",0,ncol2,nrow)) > 0)
      {
        buf1 = (long *)&jw[ind+1]; /* save data area address */
        rol->dabufp += bosMclose_(jw,ind,ncol,nrow);
      }
      else
      {
        rol->dabufp += bosMmsg_(jw,"ERRO",rol->pid,"bosMopen_() returns error1.");
      }




      /* fill both banks */
      for(i=0; i<(len12/NCHN); i++)
      {
        for(ii=0; ii<ncol1; ii++) *buf++ = utRingReadWord(ring0);
        for(ii=0; ii<ncol2; ii++) *buf1++ = utRingReadWord(ring0);
	  }





      /* create HLS #1 bank */
      if( (ind = bosMopen_(jw,"HLS ",1,ncol1,nrow)) > 0)
      {
        buf = (long *)&jw[ind+1]; /* save data area address */
        rol->dabufp += bosMclose_(jw,ind,ncol,nrow);
      }
      else
      {
        rol->dabufp += bosMmsg_(jw,"ERRO",rol->pid,"bosMopen_() returns error1.");
      }

      /* create HLB #1 bank */
      if( (ind = bosMopen_(jw,"HLB ",1,ncol2,nrow)) > 0)
      {
        buf1 = (long *)&jw[ind+1]; /* save data area address */
        rol->dabufp += bosMclose_(jw,ind,ncol,nrow);
      }
      else
      {
        rol->dabufp += bosMmsg_(jw,"ERRO",rol->pid,"bosMopen_() returns error1.");
      }




      /* fill both banks */
      for(i=0; i<(len12/NCHN); i++)
      {
        for(ii=0; ii<ncol1; ii++) *buf++ = utRingReadWord(ring1);
        for(ii=0; ii<ncol2; ii++) *buf1++ = utRingReadWord(ring1);
	  }

    }


#endif


  }
  else           /* physics and physics-sync events */
  {
    /* clean up 'psync', will use it to accumulate errors */
    psync = 0;

#ifdef SCALER1




	

    /* check latch cards empty on physics sync event, resync if not */
    if(syncFlag==1)
    {


      psync|=sync_status;
      sync_status=0;




      /* create PTRN bank */
      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = 0x20000000 + psync;
      rol->dabufp += bosMclose_(jw,ind2,1,1);

      phys_sync_count++;
      run_phys_sync_count++;
    }
    else
    {
/*
      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = 0x10000000 + psync;
      rol->dabufp += bosMclose_(jw,ind2,1,1);
*/

      phys_count++;
      run_phys_count++;
    }
#endif

#ifdef SCALER2
    if(syncFlag==1)
    {
      phys_sync_count++;
      run_phys_sync_count++;
      psync=0x0;

      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = 0x20000000 + psync;
      rol->dabufp += bosMclose_(jw,ind2,1,1);
    }
    else
    {
      event_count++;
      run_event_count++;
      psync=0x10000000;
    }
#endif

#ifdef SCALER3
    if(syncFlag==1)
    {
      phys_sync_count++;
      run_phys_sync_count++;
      psync=0x0;

      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = 0x20000000 + psync;
      rol->dabufp += bosMclose_(jw,ind2,1,1);
    }
    else
    {
      event_count++;
      run_event_count++;
      psync=0x10000000;
    }
#endif

#ifdef SCALER4
    if(syncFlag==1)
    {
      phys_sync_count++;
      run_phys_sync_count++;
      psync=0x0;

      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = 0x20000000 + psync;
      rol->dabufp += bosMclose_(jw,ind2,1,1);
    }
    else
    {
      event_count++;
      run_event_count++;
      psync=0x10000000;
    }
#endif



    /**********************************************/
	/* polar readout was moved to helicity events */
    /*********************************************/








    /**********************************************/
    /**********************************************/
    /**********************************************/




    if(syncFlag==1)
    {
      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = 0x20000000;
      rol->dabufp += bosMclose_(jw,ind2,1,1);
    }
    else
    {
      ;
    }
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

static void
__end()
{
#ifdef POLAR1
  scaler7201control(scaler0, DISABLE_EXT_NEXT);
  scaler7201control(scaler1, DISABLE_EXT_NEXT);

#ifdef USE_INTERRUPTS
  scaler7201intdisable(scaler0);
#endif
  scaler7201reset(scaler0);
  scaler7201clear(scaler0);

  scaler7201reset(scaler1);
  scaler7201clear(scaler1);
#endif

  CDODISABLE(VME,1,0);

  end_count++;

  logMsg("INFO: User End Executed\n",1,2,3,4,5,6);

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
