/*************************************************************************
 *
 *   sfiUserLib.c  - Library of routines for the user to write for 
 *                   Aysyncronous readout and buffering of events
 *                   from FASTBUS. Use RAM LIST PROGRAMMING
 *
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "sfiLib.h"
#include "dmaPList.h"

#include "uthbook.h"
#include "coda.h"
#include "fbutil.h"

#include "circbuf.h" /* just to get the number of bufs and buffer sizes */


#define FIX_7EVENT_BUG


/* maximum 131072,100 */
#define BUFSIZE_IN_BYTES1 MAX_EVENT_LENGTH/*65536*/
#define NUM_BUFFERS1 MAX_EVENT_POOL/*200*/

/* 0: External Mode   1: Trigger Supervisor Mode */
#define TS_MODE  1

/* */
#define SFI_VME_ADDR 0xe00000
#define IRQ_SOURCE  0x10

/* from Bank...h */
#define BT_UI4_ty  0x01


/**********/
/* locals */

static int syncFlag, lateFail, type;
static int *jw, *dabufp1, *dabufp2, ind;

static unsigned long MisgSlotMask = 0;
static int local_desync = 0;

static int rocid = -1;
static char *rcname = "RC00";
static int maxbytes = 0;
unsigned long spds_mask1;
signed int high64slot, low64slot, n64slots;
/* time profiling: 0-OFF, 1-ON */
static int timeprofile = 1;
/* histogram area */
static UThisti histi[NHIST];

static int nevents;

/*************/
/* externals */

extern unsigned int sfiCpuMemOffset;

extern int rocId();

extern unsigned int sfiSyncFlag;

extern struct brset adc81brset;
extern struct brset tdc77brset;
extern struct brset tdc77Sbrset;
extern struct brset tdc72brset;
extern struct brset tdc75brset;
extern struct hardmap map;
extern struct csr str;

/* Input and Output Partions for SFI Readout */
ROL_MEM_ID sfiIN, sfiOUT;

void 
SFIRamLoad() 
{
  int first_br, jram, cc;

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


#ifndef FIX_7EVENT_BUG
  /*...... load next event for 1881/1877/1877S */
  if((adc81brset.readflag != 0) ||
     (tdc77brset.readflag != 0) ||
     (tdc77Sbrset.readflag != 0))
  {
    FBWCM(BROADCAST_1881_CODE,0,0x400);
  }
#endif


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


#ifndef FIX_7EVENT_BUG
  /* broadcast to load next event for 1881/1877/1877S (increment readpointer)*/
  FBWCM(BROADCAST_1881_CODE,0,0x400);
#endif


  /* set starting address to the first(highest) slot */
  FBAD(high64slot,0);
  /* block read: use FBBR() for 32-bit or FBBRF() for 64-bit transfer */
  FBBRF(Tdc77Length*n64slots); /* use the biggest length can be */
  FBREL;

  FBSWC;                  /* Store final word count in FIFO */
  FBEND;                  /* End of List */
  SFI_WAIT;               /* Wait a little */
  SFI_RAM_LOAD_DISABLE;   /* Re-enable Sequencer */

}


/* initialize Memory partitions for managing events */
void
sfiUserInit()
{
  unsigned int res, laddr;

  /* Setup SFI pointers to Enable triggers, and establish
     DMA address offsets */
  res = (unsigned long) sysBusToLocalAdrs(0x39,
                                          (char *)SFI_VME_ADDR,(char **)&laddr);
  if(res != 0)
  {
    printf("sfiUserInit: Error Initializing SFI res=%d \n",res);
  }
  else
  {
    printf("sfiUserInit: Calling InitSFI() routine with laddr=0x%x.\n",laddr);
    InitSFI(laddr);
  }

  /* Setup Buffer memory to store events */
  dmaPFreeAll();
  sfiIN  = dmaPCreate("sfiIN",BUFSIZE_IN_BYTES1, NUM_BUFFERS1, 0);
  sfiOUT = dmaPCreate("sfiOUT",0,0,0);

  dmaPStatsAll();
}


/* */
void
sfiUserDownload()
{
  unsigned long offset, res, laddr;
  int ii, len;
  char *expid, fpedname[256], dname1[256], dname2[256], fname1[256];
  char *clonparms = "/usr/local/clas/parms";
  char *pedroc = "roc00.ped";
  char tname[100];

  /* Reinitialize the Buffer memory */
  dmaPReInitAll();

  /* Initialize AUX Card */
  sfiAuxInit();

  /* Initialize SFI Interrupt interface - use defaults */
  sfiIntInit(0,0,0,0);

  /* Connect User Trigger Routine */
  sfiIntConnect(sfiUserTrigger,IRQ_SOURCE);

  printf("sfiUserDownload: User Download Executed\n");






  /* from parcer: SFI specific */
  /* Get offset for local DRAM as seen from VME bus */
  if(sysLocalToBusAdrs(0x09,0,&sfiCpuMemOffset))
  {
    printf("**ERROR** in sysLocalToBusAdrs() call \n"); 
    printf("sfiCpuMemOffset=0 FB Block Reads may fail \n"); 
  }
  else
  {
    printf("sfiCpuMemOffset = 0x%08x\n",sfiCpuMemOffset); 
  }
  







  /**************/
  /* CLAS stuff */

  utBootGetTargetName(tname,99);
  printf("target name >%s<\n",tname);

  rocid = rocId();
  printf("rocid = %d (%d)\n",rocid,rocId());

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");



  /* get global offset for A24 */
  sysBusToLocalAdrs(0x39,(char *)0,(char **)&offset);
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

      map = what_hardware(rocid,dname2); /* call 'fb_init_1' inside */

      /* sparsification if ADC's */
      len = strlen(tname);
      strncpy(fpedname,tname,len);
      strcpy((char *)&fpedname[len],".ped",4);
      fpedname[len+4] = '\0';
      printf("reading sparsification table ..\n");
      adc1881spar_file2mem(fpedname);
      printf("setting sparsification into adc's ..\n");
      adc1881spar_mem2crate(); /* call 'fb_init_1' inside */
      printf(".. sparsification done.\n");

      /* boards setting */
      sprintf(dname1,"%s/%s/%s",clonparms,"feconf/csrconf",expid);
      printf("geting board registers from %s\n",dname1);

      str = what_settings(rocid,dname1);
    }
  }

  /* calculate what the MAXIMUM number of words we can get from boards;
     if buffer size is smaller, send message */
  if((maxbytes = GetMaxEventLength(map,str)) > BUFSIZE_IN_BYTES1)
  {
    printf("\n============================================================\n");
    printf("ERROR: possible MAX event size %d bytes > allocated %d bytes !\n",
      maxbytes,BUFSIZE_IN_BYTES1);
    printf("\n============================================================\n");
  }
  else
  {
    printf("maxbytes=%d\n",maxbytes);
  }

  /* Setup SFI pointers to Enable triggers,
  and establish DMA address offsets */

  /*
  res = (unsigned long) sysBusToLocalAdrs(0x39,
                                          (char *)SFI_VME_ADDR,
                                          (char **)&laddr);
  if(res != 0)
  {
    printf("sfiUserInit: Error Initializing SFI res=%d \n",res);
  }
  else
  {
    printf("sfiUserInit: Calling InitSFI() routine with laddr=0x%x.\n",laddr);
    InitSFI(laddr);
  }
  */

}

void
sfiUserPrestart()
{
  unsigned int adc_id;
  unsigned int datascan;
  char *env;
  char *histname = "ROL1 RC00";
  signed int jj, is64possible;
  unsigned short value, value0, value1, array0[32], array1[32], data16;
  int i, ii, ifull;



  /* repeat it here until problem fixed */
  dmaPReInitAll();


  SFI_ENABLE_SEQUENCER;








  /**************/
  /* CLAS stuff */


  spds_mask1 = create_spds_mask(&map);
  printf ("spds_mask1: 0x%x\n",spds_mask1);

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

  printf("DEBUG %d %d %d %d \n",adc81brset.readflag, tdc77brset.readflag,
                                tdc72brset.readflag, tdc75brset.readflag);

  /*read from broadcast control 9 into datascan*/
  fb_frcm_1(9,0,&datascan,1,0,1,0,0,0);
  printf("INFO: datascan = 0x%x\n",datascan);
  printf("INFO: datascan&spds_mask1 = 0x%x\n",datascan&spds_mask1);
  while( ( (datascan & spds_mask1) != 0) )
  { 
    logMsg("datascan = 0x%08x",datascan);
    logMsg("=1=> cleanup buffers:\n",0,0,0,0,0,0);
    for(jj=0; jj<=25; jj++)
    {
      if(datascan & (MEK<<jj))
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

  sprintf((char *)&rcname[2],"%02d",rocid);
  printf("rcname >%4.4s<\n",rcname);


  /* time profile setup 
  if( (env=getenv("PROFILE")) != NULL )
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
  */

  if(timeprofile)
  {
    sprintf((char *)&histname[7],"%02d",rocid);
    uthbook1(histi, 1000+rocid, histname, 200, 0, 200);
  }



  /* end of CLAS stuff */
  /*********************/


  MisgSlotMask = 0;
  local_desync = 0;

  nevents = 0;
  syncFlag = 0;

  /* Initialize SFI Interrupt variables */
  sfiIntClearCount();

}

void
sfiUserGo()
{

#ifdef TDC_CALIB
  /* strobe NIM output 02, for vetoing RF */
  *sfi.writeVmeOutSignalReg = 0x200;
#endif

#ifdef TOF_LASER
  /* strobe NIM output 01, laser on */
  *sfi.writeVmeOutSignalReg = 0x100;
#endif

  logMsg("sfiIntCount=%d\n",sfiIntCount,2,3,4,5,6);

  /* Enable Interrupts */
  sfiIntEnable(IRQ_SOURCE,TS_MODE);
}

void
sfiUserEnd()
{
  int status, dma, count, blen;

  /* Disable Interrupts */
  sfiIntDisable(IRQ_SOURCE,TS_MODE);

  dmaPStatsAll();
  
  printf("ENDINGGGGGG: sfiIntCount=%d syncFlag=%d lateFail=%d type=%d\n",
    sfiIntCount,syncFlag,lateFail,type);

  /* Check on last Transfer */
  if(sfiIntCount > 0)
  {
    if((syncFlag==1)&&(type==0))  /* force_sync (scaler) events */
    {
      logMsg("ending by force_sync (scaler) event\n",1,2,3,4,5,6);
    }
    else if((syncFlag==0)&&(type==15))  /* helicity events */
    {
      logMsg("ending by helicity event\n",1,2,3,4,5,6);
    }
    else /* physics and physics (scheduled) sync events */
    {
      status = sfiSeqDone();
      if(status != 0)
      {
        logMsg("sfiUserEnd: ERROR: Last Transfer Event NUMBER %d, status = 0x%x (0x%x 0x%x 0x%x 0x%x)\n",
          sfiIntCount,status,*(dma_dabufp-4),
          *(dma_dabufp-3),*(dma_dabufp-2),*(dma_dabufp-1));
        sfi_error_decode(0);
        /* remove BOS header created at DMA start time */
        dma_dabufp -= NHEAD;
      }
      else
      {
        logMsg("sfiUserEnd: INFO: Last Event %d, status=%d (0x%08x 0x%08x)\n",
          sfiIntCount,status,dma_dabufp,jw,4,5,6);
        logMsg("data: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
          *(dma_dabufp-4),*(dma_dabufp-3),*(dma_dabufp-2),
          *(dma_dabufp-1),*(dma_dabufp),*(dma_dabufp+1));
        logMsg("jw1 : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
          *(jw-4),*(jw-3),*(jw-2),
          *(jw-1),*(jw),*(jw+1));

        dma = sfiReadSeqFifo();
        count = dma&0xffffff;
        dma_dabufp += count;

        dabufp2 = dma_dabufp;
        blen = (int *)dma_dabufp - (int *)&jw[ind+1];
        logMsg("Last DMA status = 0x%x count=%d blen=%d\n",dma,count,blen,4,5,6);
        if(blen >= (BUFSIZE_IN_BYTES1/4))
        {
          logMsg("1ERROR sfI: event too long, blen=%d, ind=%d\n",blen,ind,0,0,0,0);
          logMsg(": %d %d 0x%x 0x%x\n",blen,ind,dabufp1,dabufp2,0,0);
        }
        if(bosMclose_(jw,ind,1,blen) == 0)
        {
          logMsg("2ERROR in bosMclose_ - space is not enough !!!\n");
        }
      }
	}

    PUTEVENT(sfiOUT);
  }

  /*Reset AUX Port */
  sfiAuxWrite(0x8000);


#ifdef TDC_CALIB
  /* strobe NIM output 02, disable laser (just to be safe) */
  *sfi.writeVmeOutSignalReg = 0x2000000;
#endif

#ifdef TOF_LASER
  /* strobe NIM output 01, disable laser (just to be safe) */
  *sfi.writeVmeOutSignalReg = 0x1000000;
#endif

  dmaPStatsAll();

  printf("sfiUserEnd: Ended after %d events\n",sfiIntCount);
}


#define MYGETEVENT \
    GETEVENT(sfiIN,sfiIntCount); \
    /* move 'dma_dabufp' pointer to reserve a room for CODA header */ \
    dma_dabufp += 2; \
    /* set the 'jw' pointer and initialize BOS format */ \
    jw = dma_dabufp; \
    dabufp1 = jw; \
    jw[ILEN] = 1; \
    /* fill second CODA header word */ \
	nevents ++; \
    jw[-1] = (syncFlag<<24)|(type<<16)|(BT_UI4_ty<<8)|(0xff&nevents)

#define MYPUTEVENT \
    status = sfiSeqDone(); \
    if(status != 0) \
    { \
      logMsg("Trig: ERROR: Last Transfer Event NUMBER %d, status=0x%08x\n", \
        sfiIntCount,status,0,0,0,0); \
      sfi_error_decode(0); \
      /* remove BOS header created at DMA start time */ \
      dma_dabufp -= NHEAD; \
    } \
    else \
    { \
      dma = sfiReadSeqFifo(); \
      count = dma & 0xffffff; \
      dma_dabufp += count; \
      dabufp2 = dma_dabufp; \
      blen = (int *)dma_dabufp - (int *)&jw[ind+1]; \
      if(blen >= (BUFSIZE_IN_BYTES1/4)) \
      { \
        logMsg("ERROR: evt too long, blen=%d, ind=%d\n",blen,ind,0,0,0,0); \
        logMsg(": %d %d 0x%x 0x%x\n",blen,ind,dabufp1,dabufp2,0,0); \
      } \
      if(bosMclose_(jw,ind,1,blen) == 0) \
      { \
        logMsg("2ERROR in bosMclose_ - space is not enough !!!\n"); \
      } \
    }


void
sfiUserTrigger(int arg)
{
  int i, ii, jj, status, dma, count, blen;
  unsigned int datascan, auxval;
  TIMER_VAR;

  if(timeprofile)
  {
    TIMER_NORMALIZE;
    TIMER_START;
  }

  /***************************************************/
  /* complete previous event processing if necessary */
  /***************************************************/

  if(sfiIntCount > 1)
  {
    if(syncFlag==1)
    {
      if(type!=0) /* physics sync event */
	  {
        /*logMsg("previous event was physics sync\n",1,2,3,4,5,6)*/;
        /*MYPUTEVENT; done in previous event*/
	  }
      if(type==0) /* force_sync (scaler) events */
      {
        /*logMsg("previous event was force_sync (scaler)\n",1,2,3,4,5,6)*/;
      }
    }
    else if(type==15)
	{
      /*logMsg("previous event was helicity event\n",1,2,3,4,5,6)*/;
	}
    else /* physics non-sync event (seq was started on previous event) */
	{
      MYPUTEVENT;
	}
    PUTEVENT(sfiOUT);
  }



  /**********************************/
  /* start current event processing */
  /**********************************/

  /* gets 'type', 'lateFail' and 'syncFlag' */
  /* must be called before 'SFI_ENABLE_SEQUENCER' !!!*/
  auxval = sfiAuxRead();
  type     = ((auxval&0x3c)>>2);
  lateFail = ((auxval&0x02)>>1);
  syncFlag = ((auxval&0x01));
  /*
logMsg("EVTYPE=%d syncFlag=%d\n",type,syncFlag,3,4,5,6);
  */
  /* check event type and sync flag consistency */
  if((syncFlag<0)||(syncFlag>1))         /* illegal */
  {
    logMsg("Illegal1: syncFlag=%d EVTYPE=%d\n",syncFlag,type,3,4,5,6);
  }
  else if((syncFlag==0)&&(type==0))    /* illegal */
  {
    logMsg("Illegal2: syncFlag=%d EVTYPE=%d\n",syncFlag,type,3,4,5,6);
  }
  else if((syncFlag==1)&&(type==0))    /* force_sync (scaler) events */
  {
    MYGETEVENT;

    if(timeprofile)
    {
      /*logMsg("report timing histogram\n",1,2,3,4,5,6);*/
      dma_dabufp += uth2bos(histi, 1000+rocid, jw);
    }
  }
  else if((syncFlag==0)&&(type==15)) /* helicity strob events */
  {
    MYGETEVENT;
  }
  else /* physics and physics (scheduled) sync events */
  {
    /* perform FASTBUS datascan */
    SFI_ENABLE_SEQUENCER;
    ii=0;
    datascan = 0;
    while ( (ii < 30) && (spds_mask1 != (datascan&spds_mask1)) )
    {
      datascan = 0;
      fb_frcm_1(9,0,&datascan,1,0,1,0,0,0);
      ii++;
    }

    /* 'open' event */
    MYGETEVENT;

    /* create BOS bank header */
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
    {
      logMsg("bosMopen_ Error: %d\n",ind,0,0,0,0,0);
    }
    dma_dabufp += NHEAD;

    /* start transfer if all boards have at least one event */
    if(ii<30)
    {
#ifdef FIX_7EVENT_BUG
      fb_fwcm_1(BROADCAST_1881_CODE,0,0x400,1,0,1,0,0,0);
#endif
      sfiRamStart(RAM_ADDR,dma_dabufp);
    }
    else /* 'missing gate' situation */
    {
      logMsg("missing gate !!!\n",1,2,3,4,5,6);

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

      /* open, fill and close PSYN bank for missing gate */
      EIEIO;
      SynC;
      ind = bosMopen_(jw,"PSYN",rocid,1,0);
      dma_dabufp += NHEAD;
      jw[ind+1] = PHYS + (datascan&spds_mask1);
      bosMclose_(jw,ind,1,1);
      dma_dabufp++;

      MisgSlotMask = (~datascan&spds_mask1);

    }

    if(syncFlag==1)
    {
      MYPUTEVENT;
      /*logMsg("physics sync event\n",1,2,3,4,5,6)*/;

      /* make sure nothing left in board buffers */

      EIEIO;
      SynC;

      /* datascan */
      datascan = 0;
      fb_frcm_1(9,0,&datascan,1,0,1,0,0,0);

      /* open and fill PTRN bank */
      ind = bosMopen_(jw,"PTRN",rocid,1,0);
      dma_dabufp += NHEAD;
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
        logMsg("SYNC:MisgMask = 0x%08x\n",MisgSlotMask,0,0,0,0,0);
      }
      else
      {
        ;
      }

      /* close PTRN bank */
      bosMclose_(jw,ind,1,1);
      dma_dabufp++;

      /* clear buffers if necessary */
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

      MisgSlotMask = 0;
      local_desync = 0;
    }

  }

  if(timeprofile) TIMER_STOP(100000,1000+rocid);

  /* if there is at least one input buffer - acknowledge interrupt */
  /* otherwise set 'sfiNeedAck' flag (will be acknowledged in 'done' */
  if(dmaPEmpty(sfiIN))
  {
    sfiNeedAck = 1;
  }
  else
  {
    sfiIntAck(IRQ_SOURCE,TS_MODE);
  }

}

void
sfiUserStatus()
{
  ;
}

void
sfiUserTest()
{
  int lock_key;
  DANODE *outEvent;
  
  while(1)
  {
    if(dmaPEmpty(sfiOUT)) continue;
    
    /* get event - then free it */
    lock_key = intLock();
    outEvent = dmaPGetItem(sfiOUT);
    dmaPFreeItem(outEvent);
    intUnlock(lock_key);
  }

}






