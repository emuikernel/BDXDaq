
/* "fbrol1.c" */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

/* readout list SFI */
#define ROL_NAME__ "SFI"

/* ts control */
#ifndef STANDALONE
#define TRIG_SUPV
#endif

/* polling mode if needed */
#define POLLING_MODE


/* name used by loader */
#ifdef STANDALONE
#define INIT_NAME fbrol1_standalone__init
#endif
#ifdef TDC_CALIB
#define INIT_NAME fbrol1_calib__init
#endif
#ifdef TOF_LASER
#define INIT_NAME fbrol1_laser__init
#endif
#ifdef SINGLE_READ
#define INIT_NAME fbrol1_single__init
#endif
#ifdef BLOCK_READ
#define INIT_NAME fbrol1_block__init
#endif
#ifdef PROFILE
#define INIT_NAME fbrol1_profile__init
#endif
#ifdef FBROL1
#define INIT_NAME fbrol1__init
#endif

#include "rol.h"

/* sfi readout */
#include "SFI_source.h"

/*****************************/
/*****************************/




#include "uthbook.h"
#include "coda.h"
#include "fbutil.h"
#include "tdc890.h"


/* first board has address TDCADR, second TDCADR+TDCSTEP etc */
#define TDCADR  0x210000
#define TDCSTEP  0x10000




#undef DEBUG

/* v1190/v1290 stuff */
#define NBOARDS 2
#define MY_MAX_EVENT_LENGTH 200
#define TDC_OFFSET 0
#define CH_OFFSET  0
/* v1190/v1290 stuff ended */

/* global addresses for boards */
static unsigned long tdcadr[21];
static unsigned long tdcbaseadr;

unsigned long spds_mask1;

static char *rcname = "RC00";
unsigned long MisgSlotMask = 0x0;
int local_desync = 0;

signed int high64slot, low64slot, n64slots;

extern struct brset adc81brset;
extern struct brset tdc77brset;
extern struct brset tdc77Sbrset;
extern struct brset tdc72brset;
extern struct brset tdc75brset;
extern struct hardmap map;
extern struct csr str;
extern int totalPTim1;

/* time profiling: 0-OFF, 1-ON */
static int timeprofile = 0;

/* histogram area */
static UThisti histi[NHIST];

#define NDMASTAT 3
static int maxbytes = 0;

#define NTICKS 1000 /* the number of ticks per second */



void
tsleep(int n)
{
  taskDelay ((sysClkRateGet() / NTICKS) * n);
}



/* */

void
SFIRamLoad() 
{
  unsigned long first_br, jram, cc;

  first_br = 0;

  printf("DEBUG in SFIRamLoad => %d %d %d %d %d\n",
            adc81brset.readflag, tdc77brset.readflag,
            tdc72brset.readflag, tdc75brset.readflag,
            tdc77Sbrset.readflag);
  printf("slot#   1872  1875  1881(2)  1877(2)  1877S(2)\n");
  for(cc=0; cc<=25; cc++)
  {
    printf("[%2d]      %1d     %1d      %1d %1d      %1d %1d       %1d %1d\n",
            cc,
            map.map_tdc72[cc],map.map_tdc75[cc],
            map.map_adc[cc], adc81brset.lastsing[cc],
            map.map_tdc77[cc], tdc77brset.lastsing[cc],
            map.map_tdc77S[cc], tdc77Sbrset.lastsing[cc]);
  }


  /* .......Setup Sequencer for RAM List Loading at RAM_ADDR */
  SFI_RAM_LOAD_ENABLE(RAM_ADDR); 

  /*...... load next event for 1881/1877/1877S */
  if((adc81brset.readflag != 0) ||
     (tdc77brset.readflag != 0) ||
     (tdc77Sbrset.readflag != 0))
  {
    FBWCM(BROADCAST_1881_CODE,0,0x400);
  }

  for(cc=0; cc<=25; cc++)
  {
    jram=25-cc;

    /*...............1875 block read ..............*/
    if(map.map_tdc75[jram] && (first_br == 0))
    {
      FBAD(jram,0);
      FBBR(Tdc72Length);
      FBREL;
      first_br = 1;
      fpwc(jram,0,0x00800000);
    }
    else if(map.map_tdc75[jram] && (first_br == 1))
    {
      FBAD(jram,0);
      FBBRNC(Tdc72Length);
      FBREL;
      fpwc(jram,0,0x00800000);
    }

    /*...............1872 block read ..............*/
    else if(map.map_tdc72[jram] && (first_br == 0))
    {
      FBAD(jram,0);
      FBBR(Tdc72Length);
      FBREL;
      first_br = 1;
      fpwc(jram,0,0x00800000);
    }
    else if(map.map_tdc72[jram] && (first_br == 1))
    {
      FBAD(jram,0);
      FBBRNC(Tdc72Length);
      FBREL;
      fpwc(jram,0,0x00800000);
    }

    /*...............1877 block read ..............*/
    else if(map.map_tdc77[jram] && (first_br == 0))
    {
      if(tdc77brset.lastsing[jram] == SINGLE)
      {
        FBAD(jram,0);
        FBBR(Tdc77Length);
        FBREL;
        first_br=1;
      }
      else if(tdc77brset.lastsing[jram] == LAST)
      {
        FBAD(jram,0);
        FBBR(Tdc77Length*tdc77brset.brlength[jram]);
        FBREL;
        first_br=1;
      }
    }
    else if(map.map_tdc77[jram] && (first_br == 1))
    {
      if(tdc77brset.lastsing[jram] == SINGLE)
      {
        FBAD(jram,0);
        FBBRNC(Tdc77Length);
        FBREL;
      }
      else if(tdc77brset.lastsing[jram] == LAST)
      {
        FBAD(jram,0);
        FBBRNC(Tdc77Length*tdc77brset.brlength[jram]);
        FBREL;
      }
    }

    /*...............1877S block read ..............*/
    else if(map.map_tdc77S[jram] && (first_br == 0))
    {
      if(tdc77Sbrset.lastsing[jram] == SINGLE)
      {
        FBAD(jram,0);
        FBBR(Tdc77Length);
        FBREL;
        first_br=1;
      }
      else if(tdc77Sbrset.lastsing[jram] == LAST)
      {
        FBAD(jram,0);
        FBBR(Tdc77Length*tdc77Sbrset.brlength[jram]);
        FBREL;
        first_br=1;
      }
    }

    else if(map.map_tdc77S[jram] && (first_br == 1))
    {
      if(tdc77Sbrset.lastsing[jram] == SINGLE)
      {
        FBAD(jram,0);
        FBBRNC(Tdc77Length);
        FBREL;
      }
      else if(tdc77Sbrset.lastsing[jram] == LAST)
      {
        FBAD(jram,0);
        FBBRNC(Tdc77Length*tdc77Sbrset.brlength[jram]);
        FBREL;
      }
    }

    /*...............1881 block read ..............*/
    else if(map.map_adc[jram] && (first_br == 0))
    {
      if(adc81brset.lastsing[jram] == SINGLE)
      {
        FBAD(jram,0);
        FBBR(Adc81Length);
        FBREL;
        first_br=1;
      }
      else if(adc81brset.lastsing[jram] == LAST)
      {
        FBAD(jram,0);
        FBBR(Adc81Length*adc81brset.brlength[jram]);
        FBREL;
        first_br=1;
      }
    }

    else if(map.map_adc[jram] && (first_br == 1))
    {
      if(adc81brset.lastsing[jram] == SINGLE)
      {
        FBAD(jram,0);
        FBBRNC(Adc81Length);
        FBREL;
      }
      else if(adc81brset.lastsing[jram] == LAST)
      {
        FBAD(jram,0);
        FBBRNC(Adc81Length*adc81brset.brlength[jram]);
        FBREL;
      }
    }
  }

  /*........... Store final word count in FIFO*/
  FBSWC;
  FBEND;                         /*..... End of List */
  SFI_WAIT;                      /*..... Wait a little */
  SFI_RAM_LOAD_DISABLE;          /*..... Re-enable Sequencer */

}


void
SFIRamLoad64()
{

  /* .......Setup Sequencer for RAM List Loading at RAM_ADDR */
  SFI_RAM_LOAD_ENABLE(RAM_ADDR); 

  /* broadcast to load next event for 1881/1877/1877S (increment readpointer)*/
  FBWCM(BROADCAST_1881_CODE,0,0x400);

  /* set starting address to the first(highest) slot */
  FBAD(high64slot,0);
  /* block read: use FBBR() for 32-bit or FBBRF() for 64-bit transfer */
  FBBR(Tdc77Length*n64slots); /* use the biggest length can be */
  FBREL;

  /*........... Store final word count in FIFO*/
  FBSWC;
  FBEND;                         /*..... End of List */
  SFI_WAIT;                      /*..... Wait a little */
  SFI_RAM_LOAD_DISABLE;          /*..... Re-enable Sequencer */

}




static void
__download()
{
  unsigned long res, laddr;
  unsigned long offset;
  int ii, len;
  char *expid, fpedname[256], dname1[256], dname2[256], fname1[256];
  char *clonparms = "/usr/local/clas/parms";
  char *pedroc = "roc00.ped";
  char tname[100];

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  /* from parcer: SFI specific */
  /* Get offset for local DRAM as seen from VME bus */
  if(sysLocalToBusAdrs(0x09,0,&sfi_cpu_mem_offset))
  {
    printf("**ERROR** in sysLocalToBusAdrs() call \n"); 
    printf("sfi_cpu_mem_offset=0 FB Block Reads may fail \n"); 
  }
  else
  { 
    printf("sfi_cpu_mem_offset = 0x%08x\n",sfi_cpu_mem_offset); 
  } 

  utBootGetTargetName(tname,99);
  printf("target name >%s<\n",tname);

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");

  /* get global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&offset);
  printf("fbrol1: offset = 0x%08x\n",offset);

  /*if((clonparms = (char *)getenv("CLON_PARMS")) == NULL)
  {
    printf("ERROR: cannot get CLON_PARMS environment variable\n");
  }
  else*/
  {
    /* use EXPID env. var. to get directory name using envGet() */
    if((expid = (char *)getenv("EXPID")) == NULL)
    {
      printf("ERROR: cannot get EXPID environment variable\n");
    }
    else
    {
      /* ROC's map */
      sprintf(dname2,"%s/%s/%s",clonparms,"feconf/rocmap",expid);
      printf("puting ROC maps to %s\n",dname2);
      map = what_hardware(rol->pid,dname2); /* call 'fb_init_1' inside */

      /* sparsification if ADC's */
      len = strlen(tname);
      strncpy(fpedname,tname,len);
      strncpy((char *)&fpedname[len],".ped",4);
      fpedname[len+4] = '\0';
      printf("reading sparsification table ..\n");
      adc1881spar_file2mem(fpedname);
      printf("setting sparsification into adc's ..\n");
      adc1881spar_mem2crate(); /* call 'fb_init_1' inside */
      printf(".. sparsification done.\n");

      /* boards setting */
      sprintf(dname1,"%s/%s/%s",clonparms,"feconf/csrconf",expid);
      printf("geting board registers from %s\n",dname1);
      str = what_settings(rol->pid,dname1);
    }
  }

  /* calculate what the MAXIMUM number of words we can get from boards;
     if buffer size is smaller, send message */
  if((maxbytes = GetMaxEventLength(map,str)) > MAX_EVENT_LENGTH)
  {
    printf("\n============================================================\n");
    printf("ERROR: possible MAX event size %d bytes > allocated %d bytes !\n",
      maxbytes,MAX_EVENT_LENGTH);
    printf("\n============================================================\n");
  }
  else
  {
    printf("maxbytes=%d\n",maxbytes);
  }


  /* Setup SFI pointers to Enable triggers, and establish
     DMA address offsets */
  res = (unsigned long) sysBusToLocalAdrs(0x39,SFI_VME_ADDR,&laddr);
  if(res != 0)
  {
    printf("Error Initializing SFI res=%d \n",res);
  }
  else
  {
    printf("Calling InitSFI() routine with laddr=0x%x.\n",laddr);
    InitSFI(laddr);
  }

  /* set NIM output 03, can be used as veto, will be reset in 'go' */
  *sfi.writeVmeOutSignalReg = 0x400;

  logMsg("INFO: User Download Executed\n",1,2,3,4,5,6);

  return;
}




static void
__prestart()
{
  unsigned long datascan;
  char *env;
  char *histname = "ROL1 RC00";
  signed int jj, is64possible;
  unsigned short value, value0, value1, array0[32], array1[32], data16;
  int i, ii, ifull;

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  logMsg("INFO: Entering User Prestart\n",1,2,3,4,5,6);

  /* init trig source SFI */
  SFI_INIT;

  /* Register an async trigger source, (up to 32 sources) */
  /* CTRIGRSA(source,code,handler,done)*/
  /* link async trig source SFI 1 to usrtrig and usrtrig_done */
#ifdef POLLING_MODE
  CTRIGRSS(SFI, 1, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */
#else
  CTRIGRSA(SFI, 1, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */
#endif

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  sfiAuxWrite(0x8000); /* Reset AUX Port */
  sfiTrigSource = IRQ_SOURCE_MASK;
  fb_init_1(0); /* reset crate 0 */
  sfi_error_decode();


  spds_mask1 = create_spds_mask(&map);
  printf ("spds_mask1: 0x%x\n",spds_mask1);  

#ifndef BLOCK_READ
#ifndef SINGLE_READ

  /* check if we can use 64-bit transfer, namely must be */
  /* no empty slots between boards and no 1872/1875s */
  high64slot = -1;
  low64slot = 100;
  n64slots = 0;
  is64possible = 0;
  for(jj=0; jj<=25; jj++)
  {
    if(map.map_tdc72[jj] || map.map_tdc75[jj])
    {
      is64possible = -1;
      break;
    }
    if(map.map_adc[jj] || map.map_tdc77[jj] || map.map_tdc77S[jj])
    {
      n64slots ++;
      if(jj < low64slot) low64slot = jj;
      if(jj > high64slot) high64slot = jj;
    }
  }

  if((is64possible == 0) && (high64slot-low64slot+1)==n64slots)
  {
    logMsg("Use 64-bit Ram List: %d slots from %d to %d\n",
      n64slots,high64slot,low64slot,0,0,0);
    program_hardware_64(&map,&str,low64slot,high64slot);
    SFIRamLoad64();
  }
  else
  {
    logMsg("Use 32-bit Ram List\n",0,0,0,0,0,0);
    program_hardware(&map,&str,2);
    SFIRamLoad();
  }

#endif
#endif

#ifdef BLOCK_READ
  program_hardware(&map,&str,1);
#endif

#ifdef SINGLE_READ
  program_hardware(&map,&str,0);
#endif

  printf("DEBUG %d %d %d %d \n",adc81brset.readflag, tdc77brset.readflag,
                                tdc72brset.readflag, tdc75brset.readflag);

  /*
#ifdef STANDALONE
  CDODISABLE(SFI,1,IRQ_SOURCE_MASK);
#endif
  */

  /* read from broadcast control 9 into datascan */
  padr = 9;
  fb_frcm_1(padr,0,&datascan,1,0,1,0,0,0);


  printf("INFO: datascan = 0x%08x\n",datascan);
  printf("INFO: datascan&spds_mask1 = 0x%08x\n",datascan&spds_mask1);

/* following code will produce endless scan if run in standalone mode and if
ADC gate taken not from AUX L1 but directly from trigger logic; in that case
when 'busy' for that logic is off then gates keep coming into ADC and cleanup
never ends; to avoid that NIM output 3 must be used to enable/disable incoming
triggers */
/*
#ifndef STANDALONE
*/
  while( ( (datascan&spds_mask1) != 0) )
  { 
    logMsg("datascan = 0x%08x\n",datascan);
    logMsg("=1=> cleanup buffers:\n",0,0,0,0,0,0);
    for(jj=0; jj<=25; jj++)
    {
      if(datascan & (0x00000001 <<jj))
      {
        logMsg("  slot %d\n",jj,0,0,0,0,0);
        if(map.map_adc[jj])    fpwc(jj,0,0x400);
        if(map.map_tdc72[jj])  fpwc(jj,0,0x00800000);
        if(map.map_tdc75[jj])  fpwc(jj,0,0x00800000);
        if(map.map_tdc77[jj])  fpwc(jj,0,0x400);
        if(map.map_tdc77S[jj]) fpwc(jj,0,0x400);
      }
    }
    fb_frcm_1(9,0,&datascan,1,0,1,0,0,0);
  }
/*
#endif
*/

  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);
  if( (env=getenv("PROFILE")) != NULL  )
  {
    if(*env == 'T')
    {
      timeprofile = 1;
      logMsg("rol1: time profiling is ON\n");
    }
    else
    {
      timeprofile = 0;
      logMsg("rol1: time profiling is OFF\n");
    }
  }
  if(timeprofile)
  {
    sprintf((char *)&histname[7],"%02d",rol->pid);
    uthbook1(histi, 1000+rol->pid, histname, 200, 0, 200);
  }

  logMsg("INFO: User Prestart Executed\n",1,2,3,4,5,6);

  /* from parser */
  *(rol->nevents) = 0;
  rol->recNb = 0;

  return;
}


static void
__end()
{
  CDODISABLE(SFI,1,IRQ_SOURCE_MASK);
 
  /* set NIM output 03, can be used as veto, will be reset in 'go' */
  *sfi.writeVmeOutSignalReg = 0x400;

#ifdef TDC_CALIB
  /* strobe NIM output 02, disable laser (just to be safe) */
  *sfi.writeVmeOutSignalReg = 0x2000000;
#endif

#ifdef TOF_LASER
  /* strobe NIM output 01, disable laser (just to be safe) */
  *sfi.writeVmeOutSignalReg = 0x1000000;
#endif

  /* enable sequencer in case if we want to execute some FASTBUS
     commands here; it is disabled in previous command */
  *sfi.sequencerEnable = 1;
 
  logMsg("INFO: User End Executed\n",1,2,3,4,5,6);   

  return;
}


static void
__pause()
{
  CDODISABLE(SFI,1,IRQ_SOURCE_MASK);

  logMsg("INFO: User Pause Executed\n",1,2,3,4,5,6);

  return;
}


static void
__go()
{
  unsigned long ii;

  logMsg("INFO: Entering User Go\n",1,2,3,4,5,6); 
#ifdef TDC_CALIB
  /* strobe NIM output 02, for vetoing RF */
  *sfi.writeVmeOutSignalReg = 0x200;
#endif

#ifdef TOF_LASER
  /* strobe NIM output 01, laser on */
  *sfi.writeVmeOutSignalReg = 0x100;
#endif

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  /*logMsg("++9++ 0x%08x 0x%08x\n",rol->pool,rol->input,3,4,5,6);*/

  CDOENABLE(SFI,1,IRQ_SOURCE_MASK);

  /* for standalone mode: sometimes it hungs on first event; we suspect
  that system is not quite ready yet, so we wait before enabling; need to
  investigate ! */
  taskDelay(sysClkRateGet());

  /* reset NIM output 03 */
  *sfi.writeVmeOutSignalReg = 0x4000000;

  return;
}


void
usrtrig(unsigned long EVTYPE, unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
  int len;
  unsigned long ii, jj, datascan, cc;
  unsigned long fbres, word_count, dmastat[NDMASTAT];
  TIMER_VAR;
  long *jw, *dabufp1, *dabufp2;
  long blen, ind;
  PROFILE_VAR;

  int nev, rlen, rlenbuf[NBOARDS];
  unsigned long res;
  unsigned long tdcslot, tdcchan, tdcval, tdc14, tdcedge;
  unsigned long tdceventid, tdcbunchid, tdcwordcount, tdcerrorflags;
  unsigned int *tdc, itdcbuf, tdcbuf[NBOARDS*MY_MAX_EVENT_LENGTH];
  unsigned int ind890;
  unsigned int sl890  [NBOARDS*MY_MAX_EVENT_LENGTH];
  unsigned int ch890  [NBOARDS*MY_MAX_EVENT_LENGTH];
  unsigned int edge890[NBOARDS*MY_MAX_EVENT_LENGTH];
  unsigned int tdc890 [NBOARDS*MY_MAX_EVENT_LENGTH];
  unsigned int tdcoffset, tdcvalue;

#ifdef PROFILE
  PROFILE_START;
#endif

  if(timeprofile)
  {
    TIMER_NORMALIZE;
    TIMER_START;
  }

  EIEIO;
  SynC;

  *sfi.sequencerEnable = 1; /* enable SFI Sequencer */
  rol->dabufp = (long *) 0;

  EIEIO;
  SynC;

  /* open event type EVTYPE of BT_UI4 */
  CEOPEN(EVTYPE, BT_UI4);

  /* get the pointer after the CODA header */
  jw = rol->dabufp;
  dabufp1 = rol->dabufp;

  /* put 1 in the first word of the CODA header */
  jw[-2 ] = 1;

  /* */
  if((syncFlag<0)||(syncFlag>1))         /* illegal */
  {
    logMsg("ERROR: syncFlag<0 || syncFlag>1\n",0,0,0,0,0,0);
  }
  else if((syncFlag==0)&&(EVTYPE==0))    /* illegal */
  {
    logMsg("ERROR: syncFlag==0 && EVTYPE==0\n",0,0,0,0,0,0);
  }
  else if((syncFlag==1)&&(EVTYPE==0))    /* force_sync (scaler) events */
  {
    logMsg("fbrol1: scaler event 0x%08x 0x%08x\n",jw[-2],jw[-1],3,4,5,6);
  }
  else if((syncFlag==0)&&(EVTYPE==15)) /* helicity strob events */
  {
    ;
  }
  else/*if(type > 0)*/
  {
    ii = 0;
    datascan = 0;

    while( (ii < 30) && (spds_mask1 != (datascan&spds_mask1)) )
    { 
      EIEIO;
      SynC;
      padr = 9;
      /* read from broadcast control 9 into datascan */
      fb_frcm_1(padr,0,&datascan,1,0,1,0,0,0);
      ii++ ;
    }

    /* Ok */
    if(ii<30)
    {
      EIEIO;
      SynC;
      if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
      {
        logMsg("bosMopen_ Error: %d\n",ind,0,0,0,0,0);
      }
      rol->dabufp += NHEAD;

#if defined(BLOCK_READ) || defined(SINGLE_READ)
      /* load next event for 1881s or 1877s */
      if((adc81brset.readflag != 0) ||
         (tdc77brset.readflag != 0) ||
         (tdc77Sbrset.readflag != 0))
      {
        /*logMsg("broadcast for 1881/1877/1877S\n",0,0,0,0,0,0);*/
        fpwcm(BROADCAST_1881_CODE,0,0x400);
      }

      for(cc=0; cc<=25; cc++)
      {
        jj = 25 - cc;

        if(map.map_tdc75[jj])
        {
          fpbr(jj,Tdc72Length);
          fpwc(jj,0,0x00800000); /* increment read pointer */
        }
        else if(map.map_tdc72[jj])
        {
          fpbr(jj,Tdc72Length);
          fpwc(jj,0,0x00800000); /* increment read pointer */
        }
        else if(map.map_tdc77[jj])
        {
          if(tdc77brset.lastsing[jj] == SINGLE)
          {
            fpbr(jj,Tdc77Length);
          }
          else if(tdc77brset.lastsing[jj] == LAST)
          {
            fpbr(jj,Tdc77Length*tdc77brset.brlength[jj]);
          }
        }
        else if(map.map_tdc77S[jj])
        {
          if(tdc77Sbrset.lastsing[jj] == SINGLE)
          {
            fpbr(jj,Tdc77Length);
          }
          else if(tdc77Sbrset.lastsing[jj] == LAST)
          {
            fpbr(jj,Tdc77Length*tdc77Sbrset.brlength[jj]);
          }
        }
        else if(map.map_adc[jj])
        {
          if(adc81brset.lastsing[jj] == SINGLE)
          {
            fpbr(jj,Adc81Length);
          } 
          else if (adc81brset.lastsing[jj] == LAST)
          {
            fpbr(jj,Adc81Length*adc81brset.brlength[jj]);
          } 
        }
      }
#else

      SFI_LOAD_ADDR(0); /* load 'rol->dabufp' as DMA destination */
      fpramStart(RAM_ADDR);
      fbres = fpramDone(NDMASTAT,dmastat);	  
      if(fbres)
      {
        EIEIO;
        SynC;
        logMsg("fbres=0x%08x, dmastat=0x%08x,0x%08x,0x%08x\n",
          fbres,dmastat[0],dmastat[1],dmastat[2],0,0);
        rol->dabufp -= NHEAD;
        sfi_error_decode();
        *sfi.sequencerReset = 1;
      }
      else
      {
        EIEIO;
        SynC;
        word_count = dmastat[0] & 0x00ffffff;
		/*
logMsg("word_count=%d\n",word_count,2,3,4,5,6);
		*/
        if(word_count > maxbytes/4)
        {
          rol->dabufp -= NHEAD;
          logMsg("word_count=0x%08x(%d), dmastat=0x%08x,0x%08x,0x%08x\n",
            word_count,word_count,dmastat[0],dmastat[1],dmastat[2],0);
        }
        else
        {
          rol->dabufp += word_count;
        }
      }
#endif
      dabufp2 = rol->dabufp;
      blen = rol->dabufp - (int *)&jw[ind+1];

      if(blen >= (MAX_EVENT_LENGTH/4))
      {
        /*logMsg("1ERROR: event too long, blen=%d, ind=%d\n",blen,ind,0,0,0,0);*/
        logMsg(": %d %d 0x%x 0x%x\n",blen,ind,dabufp1,dabufp2,0,0);
        tsleep(1); /* 1 = 0.01 sec */
      }

      EIEIO;
      SynC;
      if(bosMclose_(jw,ind,1,blen) == 0)
      {
        logMsg("2ERROR in bosMclose_ - space is not enough !!!\n");
      }

      datascan = 0;
    }
    else /* missing gate */
    {
      local_desync = 1;
      logMsg("MISG: spds_mask1 = 0x%08x, datascan = 0x%08x, ii=%d\n",
        spds_mask1,datascan,ii,0,0,0);

      logMsg("=2=> cleanup buffers:\n",0,0,0,0,0,0);
      for(jj=0; jj<=25; jj++)
      {
        EIEIO;
        SynC;
        if(datascan & (MEK<<jj))
        {
          EIEIO;
          SynC;
          logMsg("  slot %d\n",jj,0,0,0,0,0);
          if(map.map_adc[jj])    fpwc(jj,0,0x400);
          if(map.map_tdc72[jj])  fpwc(jj,0,0x00800000);
          if(map.map_tdc75[jj])  fpwc(jj,0,0x00800000);
          if(map.map_tdc77[jj])  fpwc(jj,0,0x400);
          if(map.map_tdc77S[jj]) fpwc(jj,0,0x400);
        }
      }

      /*...................> open, fill and close the bank for missing gate */
      /*...................> sync error */
      EIEIO;
      SynC;
      ind = bosMopen_(jw,"PSYN",rol->pid,1,0);
      rol->dabufp += NHEAD;
      jw[ind+1] = PHYS + (datascan&spds_mask1);
      bosMclose_(jw,ind,1,1);
      rol->dabufp++;

      MisgSlotMask = (~datascan&spds_mask1);

      /*...............................>End valid datascan routine */
    }

    /*...............................> Check if we have a sync flag */
    if(syncFlag == 1)
    {
      EIEIO;
      SynC;
      /*logMsg("PTRN !!!!!!!!!\n",0,0,0,0,0,0);*/
      datascan = 0;
      fb_frcm_1(9,0,&datascan,1,0,1,0,0,0);
      /* open, fill the bank for synch */
      ind = bosMopen_(jw,"PTRN",rol->pid,1,0);
      rol->dabufp += NHEAD;
      jw[ind+1] = SYNC + (datascan&spds_mask1);

      EIEIO;
      SynC;
      if((datascan&spds_mask1) != 0 )
      {
        logMsg("SYNC:datascan= 0x%x\n",datascan,0,0,0,0,0);
      }
      else if(local_desync == 1)
      {
        EIEIO;
        SynC;
        if(!MisgSlotMask) MisgSlotMask = 0xfffffff;
	    jw[ind+1] = SYNC + MisgSlotMask; 
        local_desync = 0;
        logMsg("SYNC:MisgMask = 0x%08x\n",MisgSlotMask,0,0,0,0,0);
      }
      else
      {
        local_desync = 0;
      }
      /*.....................> close the bank for synch  */

      bosMclose_(jw,ind,1,1);
      rol->dabufp++;

      /* ...................................>clear buffers  */
      while ((datascan&spds_mask1) != 0 )
      {
        logMsg("=3=> cleanup buffers:\n",0,0,0,0,0,0);
        for(jj=0; jj<=25; jj++)
        {
          EIEIO;
          SynC;
          if(datascan & (MEK<<jj))
          {
            EIEIO;
            SynC;
            logMsg("  slot %d\n",jj,0,0,0,0,0);
            if(map.map_adc[jj])    fpwc(jj,0,0x400);
            if(map.map_tdc72[jj])  fpwc(jj,0,0x00800000);
            if(map.map_tdc75[jj])  fpwc(jj,0,0x00800000);
            if(map.map_tdc77[jj])  fpwc(jj,0,0x400);
            if(map.map_tdc77S[jj]) fpwc(jj,0,0x400);
          }
        }
        EIEIO;
        SynC;
        fb_frcm_1(9,0,&datascan,1,0,1,0,0,0);
      }


    }


  } /* if(type > 0) */


  if(timeprofile) TIMER_STOP(100000,1000+rol->pid);

  /* force_sync (scaler) event */
  if((syncFlag==1)&&(EVTYPE==0))
  {
    if(timeprofile)
    {
      /*logMsg("report histogram ...\n");*/
      rol->dabufp += uth2bos(histi, 1000+rol->pid, jw);
    }
  }

#ifdef PROFILE
  PROFILE_STOP(10000);
#endif

  EIEIO;
  SynC;

  /* close event */
  CECLOSE;

  if(jw[-2] > 1000) logMsg("fbrol1: ERRORRRRRRRRR: 0x%08x 0x%08x\n",jw[-2],jw[-1],3,4,5,6);

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

  /* acknowledge Interrupt */
  CDOACK(SFI,1,IRQ_SOURCE_MASK);

  return;
}  

static void
__status()
{
  return;
}  

