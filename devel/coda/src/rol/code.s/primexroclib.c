
/* primexlib.c - PRIMEX experiment library of routines for VxWorks */

#ifdef VXWORKS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sigLib.h>
#include <vme.h>




/* #include <VME_source.h> unresolved symbol 'theIntHandler' */
volatile struct vme_ts {
    volatile unsigned long csr;       
    volatile unsigned long csr2;       
    volatile unsigned long trig;
    volatile unsigned long roc;
    volatile unsigned long sync;
    volatile unsigned long trigCount;
    volatile unsigned long trigData;
    volatile unsigned long lrocData;
    volatile unsigned long prescale[8];
    volatile unsigned long timer[5];
    volatile unsigned long intVec;
    volatile unsigned long rocBufStatus;
    volatile unsigned long lrocBufStatus;
    volatile unsigned long rocAckStatus;
    volatile unsigned long userData1;
    volatile unsigned long userData2;
    volatile unsigned long state;
    volatile unsigned long test;
    volatile unsigned long blank1;
    volatile unsigned long scalAssign;
    volatile unsigned long scalControl;
    volatile unsigned long scaler[18];
    volatile unsigned long scalEvent;
    volatile unsigned long scalLive1;
    volatile unsigned long scalLive2;
    volatile unsigned long id;
  } VME_TS;
volatile struct vme_ts  *ts;
volatile unsigned long *tsmem;
unsigned long ts_memory[4096];






#define TS_ADDR		0xed0000
#define MEM_ADDR	(TS_ADDR+0x4000)

#define isthis(A) !strncmp(name,A,strlen(A))

void SetTrigEnable(char *name, int val);
void SetTrigPrescale(char *name, int val);
void SetSyncEnable(char *name, int val);
void SetSyncPeriod(char *name, int val);
void SetScaler(char *name, int val);
void SetBusyTimer(char *name, int val);
void ReadMLUData(FILE *f);
void PrimexPedestalEvent_InterruptHandler(int);

/* The TS memory data is read in from a file on the clon cluster. Once put in */
/* the TS, it is then read back out and written to a different file. The */
/* files can be compared on the clon side at any time. It is done this way since */
/* PrimEx uses a few different programs for different running conditions*/

int
LoadTS_MLU(void)
{
  FILE *f;
  char *fname="/home/primex/DAQ/trigger/TS_program.dat";
  unsigned long laddr;
  char name[64];
  int err;

  /* Check that pointers have been correctly set up */
  if((unsigned long)ts == 0x00000000)
  {
    printf("ts memory location not configured. ts=0x%08x  should be 0x%08x\n",
       (unsigned long)ts,TS_ADDR);
    err=sysBusToLocalAdrs(VME_AM_STD_SUP_DATA,(long *)TS_ADDR,(long **)&ts);
    printf("ts memory now set to 0x%08x\n",(unsigned long)ts);
  }

  if((unsigned long)tsmem != MEM_ADDR)
  {
    printf("tsmem memory location not configured. tsmem=0x%08x  should be 0x%08x\n"
       ,(unsigned long)tsmem,MEM_ADDR);
    err=sysBusToLocalAdrs(VME_AM_STD_SUP_DATA,(long *)MEM_ADDR,(long **)&tsmem);
    printf("tsmem memory now set to 0x%08x\n",(unsigned long)tsmem);
  }

  /* Open file to read from */
  f = fopen(fname,"r");
  if(!f)
  {
    printf("%s:%d Error opening file: %s \n",__FILE__,__LINE__,fname);
    return(0x03);
  }

  /* Print status message */
  printf("Reading TS configuration from %s\n",fname);

  /* Read in strings until we get one that we know */
  while(1 == fscanf(f,"%s",name))
  {
    char *valptr;
    int val = -1;
    valptr = strstr(name,"=");
    if(valptr)val = atoi(++valptr);
    if(isthis("enable_trig"))        SetTrigEnable(name, val);
    else if(isthis("prescale_trig")) SetTrigPrescale(name, val);
    else if(isthis("sync_enable"))   SetSyncEnable(name, val);
    else if(isthis("sync_period"))   SetSyncPeriod(name, val);
    else if(isthis("scaler"))        SetScaler(name, val);
    else if(isthis("busytimer"))     SetBusyTimer(name, val);
    else if(isthis("MLU-data"))      ReadMLUData(f);
    else printf("Skipping string \"%s\"\n",name);
  }

  /* Close file */
  fclose(f);

  /* Print status message */
  printf("TS configuration complete.\n");

  return(0x0);
}

void
SetTrigEnable(char *name, int val)
{
  int i;

  i=atoi(&name[strlen("enable_trig")]);

  if(val<0 || val>1 || i<1 || i>12)
  {
    printf("%s:%d Bad value in \"%s\"\n",__FILE__,__LINE__,name);
    return;
  }

  if(val)
    ts->trig |= 0x1<<i;
  else
    ts->trig &= ~(0x1<<i);
}

void
SetTrigPrescale(char *name, int val)
{
  int i, bad_value=0;

  i=atoi(&name[strlen("prescale_trig")]);
  bad_value  = i<1 || i>8;
  bad_value |=  val<0 || val>=(1<<(i<=4 ? 24:16));
  if(bad_value)
  {
    printf("%s:%d Bad value in \"%s\"\n",__FILE__,__LINE__,name);
    return;
  }
  ts->prescale[i-1] = val;
}

void
SetSyncEnable(char *name, int val)
{
  if(val<0 || val>1)
  {
    printf("%s:%d Bad value in \"%s\"\n",__FILE__,__LINE__,name);
    return;
  }
  ts->csr2 |= 1;
}	

void
SetSyncPeriod(char *name, int val)
{
  if(val<0 || val>=(1<<16))
  {
    printf("%s:%d Bad value in \"%s\"\n",__FILE__,__LINE__,name);
    return;
  }
  ts->sync = val;
}

void
SetScaler(char *name, int val)
{
  int i, bad_value=0;

  i=atoi(&name[strlen("scaler")]);
  bad_value  = i<13 || i>18; /* only scalers 13-18 are programmable */
  bad_value |=  val<0 || val>0x0F;
  if(bad_value)
  {
    printf("%s:%d Bad value in \"%s\"\n",__FILE__,__LINE__,name);
    return;
  }
  ts->scalAssign |= (val&0x0f)<<(i-13); 
}

void
SetBusyTimer(char *name, int val)
{
  int i, bad_value=0;

  bad_value =  val<0 || val>0x00FFFF;
  if(bad_value)
  {
    printf("%s:%d Bad value in \"%s\"\n",__FILE__,__LINE__,name);
    return;
  }
  ts->timer[3] = val;
  ts->csr2 |= 1<<2;
}

void
ReadMLUData(FILE *f)
{
  int i;
  int n_conv;
  unsigned long laddr;

  /* Read MLU program into temporary memory */
  printf("Reading MLU program");

  for(i=0; i<4096; i++)
  {
    n_conv = fscanf(f,"%x",&ts_memory[i]);
    if(i%256 == 1)
    {
      printf(".");
      fflush(stdout);
    }
    if(n_conv !=1)
    {
      printf("%s:%d MLU section short of values !!\n",__FILE__,__LINE__);
      break;
    }
  }
  printf("Done\n");

  /* Copy MLU program into TS */
  for(laddr=0; laddr<=4095; laddr++) tsmem[laddr] = ts_memory[laddr];

  /* Set test MLU program
  printf("Setting test program in MLU. Configuration file info ignored\n");
  for(laddr=0; laddr<=4095; laddr++) tsmem[laddr] = 0x03ffff03;
  */
}

int
TSVersion(void)
{
  char b[4];

  *(unsigned long*)b = ts->id;
  printf("TS version : %c.%c.%c.%c  %d.%d.%d.%d  0x%08x\n",
         b[0],b[1],b[2],b[3],(int)b[0],(int)b[1],(int)b[2],(int)b[3],
         *(unsigned long*)b);

  return(0);
}


/*---------------------------------------------------------------*/

void
SetProgram1Event(int eventType, int syncFlag)
{
  sysBusToLocalAdrs(VME_AM_STD_SUP_DATA, (long *)TS_ADDR, (long **)&ts);
  ts->userData1 = (eventType&0xF) + ((syncFlag&0x1)<<7); /* 4-bit type ! */
}

void
SetProgram2Event(int eventType, int syncFlag)
{
  sysBusToLocalAdrs(VME_AM_STD_SUP_DATA, (long *)TS_ADDR, (long **)&ts);
  ts->userData2 = (eventType&0xF) + ((syncFlag&0x1)<<7); /* 4-bit type ! */
}

void
SendProgram1Event(void)
{
  sysBusToLocalAdrs(VME_AM_STD_SUP_DATA, (long *)TS_ADDR, (long **)&ts);
  ts->csr = 0x10;
}

void
SendProgram2Event(void)
{
  sysBusToLocalAdrs(VME_AM_STD_SUP_DATA, (long *)TS_ADDR, (long **)&ts);
  ts->csr = 0x20;
}

/* function for forcing synch. event every 10 seconds; use command
      taskSpawn "FORCE_SYNC",119,0,6000,force_synch
   in the boot script */

void
force_synch()
{
  int eventType, syncFlag, word;
  extern unsigned long sysClkRateGet();

  sysBusToLocalAdrs(VME_AM_STD_SUP_DATA, (long *)TS_ADDR, (long **)&ts);

  eventType = 0;
  syncFlag = 1;

  ts->csr = 0x8000;  /* initialize TS board */
  SetProgram1Event(eventType, syncFlag);

  while(1)
  {
    taskDelay(sysClkRateGet()*10); 
    word = ts->csr;
    if(word & 0x1) /* check if 'Go' bit is set */
    {
      /*logMsg("force_synch: 'Go' bit is set - SendProgram1Event\n",0,0,0,0,0,0);*/
      SendProgram1Event();
    }
    else
    {
      /*logMsg("force_synch: 'Go' bit is not set - do nothing\n",0,0,0,0,0,0);*/
    }
  }
}


/*---------------------------------------------------------------*/

void
pause_run_(void)
{
  sysBusToLocalAdrs(VME_AM_STD_SUP_DATA, (long *)TS_ADDR, (long **)&ts);
  ts->csr = 0x10000; /* reset 'Go' bit */
}

void
resume_run_(void)
{
  sysBusToLocalAdrs(VME_AM_STD_SUP_DATA, (long *)TS_ADDR, (long **)&ts);
  ts->csr = 0x1; /* set 'Go'bit */
}

/*---------------------------------------------------------------*/

int
CheckTSPointer(void)
{
  int err;

  /* Check that pointers have been correctly set up */
  if((unsigned long)ts == 0x00000000){
    printf("ts memory location not configured. ts=0x%08x  should be 0x%08x\n",
           (unsigned long)ts,TS_ADDR);
    err = sysBusToLocalAdrs(VME_AM_STD_SUP_DATA, (long *)TS_ADDR, (long **)&ts);
    printf("ts memory now set to 0x%08x\n",(unsigned long)ts);
  }

  return(0);
}

int
TSstatus(void)
{
	int branch;
	unsigned int status;
	int i;
	int rocenabled;
	
	CheckTSPointer();

	/* CSR1 */
	status = ts->csr;
	printf("CSR 1 (0x0):\n");
	printf("\t                              Go : %d\n", (status>>0)&1);
	printf("\t    Pause on Next scheduled Sync : %d\n", (status>>1)&1);
	printf("\t                  Sync and Pause : %d\n", (status>>2)&1);
	printf("\t             Initiate Sync Event : %d\n", (status>>3)&1);
	printf("\t        Initiate Program 1 Event : %d\n", (status>>4)&1);
	printf("\t        Initiate Program 2 Event : %d\n", (status>>5)&1);
	printf("\t Enable Level 1 (drives outputs) : %d\n", (status>>6)&1);
	printf("\t                Override Inhibit : %d\n", (status>>7)&1);
	printf("\t                       Test Mode : %d\n", (status>>8)&1);
	printf("\t                        Reserved : %d\n", (status>>9)&1);
	printf("\t                           Reset : %d\n", (status>>14)&1);
	printf("\t                      Initialize : %d\n", (status>>15)&1);
	printf("\n");
	printf("\t             Sync Event occurred : %d\n", (status>>16)&1);
	printf("\t        Program 1 Event occurred : %d\n", (status>>17)&1);
	printf("\t        Program 2 Event occurred : %d\n", (status>>18)&1);
	printf("\t              Late Fail occurred : %d\n", (status>>19)&1);
	printf("\t                Inhibit occurred : %d\n", (status>>20)&1);
	printf("\t       Write FIFO error occurred : %d\n", (status>>21)&1);
	printf("\t       Read FIFO error occurreds : %d\n", (status>>22)&1);


	/* CSR2 */
	status = ts->csr2;
	printf("CSR 2 (0x4):\n");
	printf("\t              Enable Scheduled Sync : %d\n", (status>>0)&1);
	printf("\t             Use Clear Permit Timer : %d\n", (status>>1)&1);
	printf("\t               Use Front Busy Timer : %d\n", (status>>2)&1);
	printf("\t               Use Clear Hold Timer : %d\n", (status>>3)&1);
	printf("\t            Use External Front Busy : %d\n", (status>>4)&1);
	printf("\t                  Lock ROC Branch 1 : %d\n", (status>>5)&1);
	printf("\t                  Lock ROC Branch 2 : %d\n", (status>>6)&1);
	printf("\t                  Lock ROC Branch 3 : %d\n", (status>>7)&1);
	printf("\t                  Lock ROC Branch 4 : %d\n", (status>>8)&1);
	printf("\t                  Lock ROC Branch 5 : %d\n", (status>>9)&1);
	printf("\t Enable Program 1 front panel input : %d\n", (status>>10)&1);
	printf("\t                   Enable Interrupt : %d\n", (status>>11)&1);
	printf("\t        Enable local ROC (branch 5) : %d\n", (status>>12)&1);
	printf("\n");

	/* ROC Enable Register */
	rocenabled = ts->roc;
	printf("ROC Enable Register (0x60) val=0x%x:\n",rocenabled);
	for(branch=1;branch<=4;branch++){
		printf("Branch %d: 0x%2x  bits: ", branch, rocenabled&0xFF);
		for(i=0;i<=7;i++)printf("%d",(rocenabled>>(7-i))&0x01);
		printf("\n");
		rocenabled >>=8;
	}
	printf("\n");

	/* ROC Acknowledge Status Register */
	status = ts->rocAckStatus;
	printf("ROC Acknowledge Status Register (0x60): val=0x%x\n",status);
	for(branch=1;branch<=4;branch++){
		printf("Branch %d: 0x%2x  bits: ", branch, status&0xFF);
		for(i=0;i<=7;i++)printf("%d",(status>>(7-i))&0x01);
		printf("  masked: ");
		for(i=0;i<=7;i++)printf("%d",(status>>(7-i))&0x01&(rocenabled>>(7-i)));
		printf("\n");
		status >>=8;
	}
	printf("\n");

	/* Front Busy Timer Register */
	printf("Front Busy Timer Register = %d\n\n",ts->timer[3]&0xFFFF);

	/* State Register */
	status = ts->state;
	printf("State Register (0x6C):\n");
	printf("\t               Level 1 Accept     : %d\n", (status>>0)&1);
	printf("\t            Start Level 2 Trigger : %d\n", (status>>1)&1);
	printf("\t             Level 2 Pass Latched : %d\n", (status>>2)&1);
	printf("\t             Level 2 Fail Latched : %d\n", (status>>3)&1);
	printf("\t                   Level 2 Accept : %d\n", (status>>4)&1);
	printf("\t            Start Level 3 Trigger : %d\n", (status>>5)&1);
	printf("\t             Level 3 Pass Latched : %d\n", (status>>6)&1);
	printf("\t             Level 3 Fail Latched : %d\n", (status>>7)&1);
	printf("\t                   Level 3 Accept : %d\n", (status>>8)&1);
	printf("\t                            Clear : %d\n", (status>>9)&1);
	printf("\t        Front End Busy (external) : %d\n", (status>>10)&1);
	printf("\t                 External Inhibit : %d\n", (status>>11)&1);
	printf("\t                  Latched Trigger : %d\n", (status>>12)&1);
	printf("\t                          TS Busy : %d\n", (status>>13)&1);
	printf("\t                        TS Active : %d\n", (status>>14)&1);
	printf("\t                         TS Ready : %d\n", (status>>15)&1);
	printf("\t            Main Sequencer Active : %d\n", (status>>16)&1);
	printf("\t Synchronization Sequencer Active : %d\n", (status>>17)&1);
	printf("\t Program 1 Event Sequencer Active : %d\n", (status>>18)&1);
	printf("\t Program 2 Event Sequencer Active : %d\n", (status>>19)&1);
	printf("\n\n");

	/* Trigger Word Count Register */
	status = ts->trigCount;
	printf("Trigger Word Count Register (0x14): %d\n",status&0x0FFF);

	return 0;
}

int
TSscalers(void)
{
	int i;

	CheckTSPointer();

	for(i=1;i<=18;i++){
		if(i<=12){
			printf("Trigger Input %d Count : %d\n",i,ts->scaler[i-1]);
		}else if(i<=18){
			printf("Scaler %d Count : %d\n",i,ts->scaler[i-1]);
		}
	}

	printf("Event Count : %d\n",ts->scalEvent);
	printf("Live 1 Count : %d\n",ts->scalLive1);
	printf("Live 2 Count : %d\n\n",ts->scalLive2);

	return 0;
}








/*******************/
/* O B S O L E T E */
/*******************/

/* This process is spawned at prestart and is killed after the initial set
   of pedestal events have been read in. */

int
PrimexPedestalEvent(void)
{
	SIGVEC sv;
	unsigned long ticks;

	printf("PrimexPedestalEvent spawned.\n");

	/* Check that pointers have been correctly set up */
	if((unsigned long)ts != TS_ADDR){
		printf("ts memory location not configured. ts=0x%08x  should be 0x%08x\n"
			,(unsigned long)ts,TS_ADDR);
		ts = (struct vme_ts *)TS_ADDR;
		printf("ts memory now set to 0x%08x\n",(unsigned long)ts);
	}
	if((unsigned long)tsmem != MEM_ADDR){
		printf("tsmem memory location not configured. tsmem=0x%08x  should be 0x%08x\n"
			,(unsigned long)tsmem,MEM_ADDR);
		tsmem = (unsigned long *)MEM_ADDR;
		printf("tsmem memory now set to 0x%08x\n",(unsigned long)tsmem);
	}

	/* Set up to recieve signals */
	sv.sv_handler	= (VOIDFUNCPTR) PrimexPedestalEvent_InterruptHandler;
	sv.sv_mask		= 0;
	sv.sv_flags		= 0;
	sigvec(SIGUSR1, &sv, NULL);

	/* Loop infinitely until task is deleted. Each loop will simply initiate
	   a Program 1 event and then sleep until signaled to do so again */
	while(1){
		printf("Writing Program 1 Event\n");
		ts->test |= 1<<0; /* Initiate Level 1 event */
		pause(); /* Sleep until we get a signal */
		ticks = tickGet();
		while(ticks==tickGet()); 	/* wait approximately 1 tick. */
									/* On average, this should be 
                                       about 1/200th seconds */
	}
}

void
PrimexPedestalEvent_InterruptHandler(int sig)
{
  printf("Signal received. Sleeping for a few ticks.\n");
  return;
}


/* Program thresholds for sparsified readout */

int
SetSparsificationThresholds(char *rocname, int slot)
{
	char fname[64];
	FILE *f;
	int i;
	unsigned int thresh;
	int N;
	
	sprintf(fname, "/home/primex/DAQ/calib_parms/adc_thresholds_%s_slot_%d.dat",rocname,slot);
	f=fopen(fname,"r");
	if(!f){
		printf("Unable to open \"%s\"!\n",fname);
		return 0x01;
	}
	printf("Reading ADC thresholds from \"%s\" ...",fname);

	/* Read in ascii data from file and program 1881M */
	for(i=0;i<64;i++){
		N = fscanf(f,"%d",&thresh);
		if( N!=1){
			printf("\nToo few values in file (only %d)!\n",i);
			return 0x02;
		}
		fpwc(slot, 0xC0000000+i, thresh);
	}
	printf("OK");

	/* Close file */
	fclose(f);

	return 0;
}

#else /* no UNIX version */

void
primexroclib_dummy()
{
  return;
}

#endif






