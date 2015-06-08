/***************************************************************************
                          main.c  -  Scaler Readout
                             -------------------
    begin                : Thu Sep 14 15:52:00 UTC 2002
    copyright            : (C) 2002 by Tanest Chinwanawich
    email                : tanest@jlab.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef VXWORKS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vxWorks.h>
#include <taskLib.h>
#include <intLib.h>
#include <logLib.h>
#include <vxLib.h>

#include "sis3801.h"


IMPORT STATUS sysBusToLocalAdrs(int, char *, char **);
IMPORT STATUS intDisconnect(int);
IMPORT STATUS sysIntEnable(int);
int    sysClkRateGet();
#ifdef VXWORKSPPC
IMPORT int sysBusIntAck(int);
#endif


STATUS intconnect(VOIDFUNCPTR routine, int arg);
STATUS scalIntInit (UINT32, UINT32, UINT32);
STATUS scalIntEnable (UINT32);
STATUS scalIntDisable (void);

LOCAL BOOL            scalIntRunning = FALSE;
LOCAL VOIDFUNCPTR     scalIntRoutine = NULL;
LOCAL int             scalIntArg     = (int)NULL;
LOCAL UINT32          scalIntLevel   = SCAL_VME_INT_LEVEL;
LOCAL UINT32          scalIntVec     = SCAL_INT_VEC;

volatile struct fifo_scaler *pf;
volatile UINT32 scalData[32];

unsigned int *ptr;

/* Set of helper functions */

int sis3801readfifo(int addr) 
{
 ptr=(unsigned int *)(addr + 0x100);
 return (*ptr);
}

void sis3801writefifo(int addr, int value)
{
 ptr = (unsigned int *) (addr + 0x10);
 *ptr = value;
} 

int sis3801status(int addr)
{
 ptr = (unsigned int *) addr;
 return(*ptr);
}

void sis3801control(int addr, int value)
{
 ptr=(unsigned int*) addr;
 *ptr = value;
}

void sis3801mask(int addr, int value)
{
 ptr = (unsigned int *) (addr + 0xc);
 *ptr = value;
}

void sis3801clear(int addr)
{
 ptr = (unsigned int *) (addr + 0x20);
 *ptr = 0x1;
}

void sis3801nextclock(int addr)
{
 ptr = (unsigned int *) (addr + 0x24);
 *ptr = 0x1;
}

void sis3801enablenextlogic(int addr)
{
 ptr = (unsigned int *) (addr + 0x28);
 *ptr = 0x1;
}

void sis3801disablenextlogic(int addr)
{
 ptr = (unsigned int *) (addr + 0x2c);
 *ptr = 0x1;
}

void sis3801reset(int addr)
{
 ptr = (unsigned int *) (addr + 0x60);
 *ptr = 0x1;
}

void sis3801testclock(int addr)
{
 ptr = (unsigned int *) (addr + 0x68);
 *ptr = 0x1;
}

void sis3801Uledon(int addr)
{
 ptr = (unsigned int *) addr;
 *ptr = 0x00000001;
}

void sis3801Uledoff(int addr)
{
 ptr = (unsigned int *) addr;
 *ptr = 0x00000100;
}

int sis3801almostread(int addr, int *value)
{
 int *outbuf;
 outbuf = value;
 while ( !(sis3801status(addr) & FIFO_ALMOST_EMPTY))
  *outbuf++= sis3801readfifo(addr);
 return ((int)(outbuf - value));
}

int sis3801read(int addr, int *value)
{
 int *outbuf, len;
 outbuf = value;
 len = *value;
 while ( !(sis3801status(addr) & FIFO_EMPTY))
 {
  *outbuf++ = sis3801readfifo(addr);
  len--;
  if(len == 0)
   break;
 }
 return ((int)(outbuf - value)); {
   taskDelay(1);
 } 
}

/**************************************************************************/
/*
LOCAL void scalInt(void)
{
  int mask;
  mask = (pf->csr)&ENABLE_IRQ;
  pf->csr  = DISABLE_IRQ;
  
  if  (scalIntRoutine != NULL)
    ( *scalIntRoutine) (scalIntArg);
  pf -> csr = mask;
}     
*/
/*************************************************************************/

STATUS scalIntInit (UINT32 addr, UINT32 level, UINT32 vector)
{  
   if(scalIntRunning)
   {
     printf("scalIntInit:ERROR call scalIntDiable() first\n");
     return (ERROR);
   }
 
#ifdef  VXWORKSPPC
   intEnable(scalIntVec);
#endif
   sysIntEnable(scalIntLevel);
   bzero((char *)scalData,sizeof(scalData));
   
   return(OK);
 }
/*************************************************************************/

STATUS scalIntConnect
 (
  VOIDFUNCPTR routine,
  int arg
 )
 {  
   scalIntRoutine = routine;
   scalIntArg = arg;
   return (OK);
 }

/*************************************************************************/

STATUS scalIntEnable (UINT32 mask)

 {
   if((mask==0) || (mask > 0xf)) {
     printf("VME Interrupt mask=0x%x is out of range\n",mask);
     return(ERROR);
   }else{
     if (pf) {
       pf->csr = DISABLE_IRQ;
       pf->irq = (0x800) | (scalIntLevel<<8) | scalIntVec;
       pf->csr = (mask<<20);
     }else{
       printf("scalInt not Initialized. call scalIntInit();\n");
 
       return(ERROR);
     }
   }
  return(OK);
  
}
   
/*************************************************************************/

STATUS scalIntDisable(void)
{   
  if(pf)
  {
    pf->csr = DISABLE_IRQ;
    scalIntRunning = FALSE;
  }
  else
  {
    printf ("scalInt not Initialized. Call scalIntInit()!\n");
    return(ERROR);
  }

  return(OK);
}

/************************************************************************/

int
store(int addr, int mask)
{ 
  logMsg("store data\n",1,2,3,4,5,6);
  sis3801reset(addr);
  sis3801clear(addr);
  sis3801enablenextlogic(addr);
  sis3801nextclock(addr);
  sis3801control(addr, ENABLE_TEST); 
  sis3801nextclock(addr);
  sis3801testclock(addr);
  sis3801control(addr, DISABLE_TEST);
  sis3801Uledon(addr);
  
  return(0);
}


/****************************/
/* level 2 scaler functions */
/****************************/

#define NL2CH 30
static char lookup[30][10] = {
  "[S1R1 AX]","[S1R2 AX]","[S1R3 AX]","[S2R1 AX]","[S2R2 AX]","[S2R3 AX]",
  "[S3R1 AX]","[S3R2 AX]","[S3R3 AX]","[S4R1 AX]","[S4R2 AX]","[S4R3 AX]",
  "[S5R1 AX]","[S5R2 AX]","[S5R3 AX]","[S6R1 AX]","[S6R2 AX]","[S6R3 AX]",
  "[S1R2 ST]","[S1R3 ST]","[S2R2 ST]","[S2R3 ST]","[S3R2 ST]","[S3R3 ST]",
  "[S4R2 ST]","[S4R3 ST]","[S5R2 ST]","[S5R3 ST]","[S6R2 ST]","[S6R3 ST]"
                             };

#define L2SCALER 0x350000

int sysBusToLocalAdrs();
static unsigned long ppc_offset;
/* BAD: MUST USE sysBusToLocalAdrs() function !!!!! */
static unsigned long addr;


int
l2_init()
{
  sysBusToLocalAdrs(0x39,0,(char **)&ppc_offset);
  addr = ppc_offset + L2SCALER;
  printf("l2_init: scaler address = 0x%08x\n",addr);
}


int
l2_readscaler(unsigned int counts[NL2CH])
{
  int j;

  sis3801reset(addr);
  sis3801clear(addr);
  sis3801enablenextlogic(addr);
  sis3801nextclock(addr);
 
  /*sis3801control(addr, ENABLE_TEST); */
  /*sis3801control(addr, DISABLE_TEST); */
  sis3801Uledon(addr);

  while(sis3801status(addr) & FIFO_EMPTY)
  {
    taskDelay(sysClkRateGet()*1); /* 1 sec delay */
    sis3801nextclock(addr);
  }

  for(j=0; j<NL2CH; j++) counts[j] = sis3801readfifo(addr);  
  sis3801clear(addr);
   
  return(0);
} 
/*************************************************************************/

int
l2_status()
{
  int counts[NL2CH], j;

  l2_readscaler(counts);

  for(j=0; j<NL2CH; j++)
  {
    printf("[%2d] %9.9s %8d Hz\n",j+1,lookup[j],counts[j]);  
  }

  return(0);
} 

int
l2_status_dead()
{
  int counts[NL2CH], j, mean;




/* DISABLE THIS FUNCTION AS SOON AS NOBODY WANT TO FIX HARDWARE PROBLEM */
/*return(0);*/





  l2_readscaler(counts);

  mean = 0;
  for(j=0; j<NL2CH; j++)
  {
    mean += counts[j];
  }
  mean = mean / NL2CH;
  /*printf("mean=%d\n",mean);*/

  /* report dead channels only if count high enough */
  if(mean > 10000)
  {
    for(j=0; j<NL2CH; j++)
    {
      if(counts[j]==0) printf("%9.9s\n",lookup[j]);
    }
  }

  return(0);
}

int
l2_status_hot()
{
  int counts[NL2CH], j, mean, nalive;

  l2_readscaler(counts);

  nalive = mean = 0;
  for(j=0; j<NL2CH; j++)
  {
    if(counts[j] != 0)
    {
      nalive ++;
      mean += counts[j];
    }
  }

  if(nalive > 0)
  {
    mean = mean / nalive;
    /*printf("nalive=%d mean=%d\n",nalive,mean);*/
    for(j=0; j<NL2CH; j++)
    {
      if(counts[j] != 0)
      {
        if(counts[j] > mean*2) /* how much higher then 'mean' value channel */
        {                      /* must count to be considered as 'hot'      */
          printf("%9.9s\n",lookup[j]);
        }
      }
    }
  }

  return(0);
}


/**************************************************************************/
/***  SP: additional functions to make remote reset for ADB crates  *******/
/**************************************************************************/

#define FLEX_ADDR      0x0EE0

struct flex_struct {
  volatile unsigned short csr1;        /* 0x00 */
  volatile unsigned short port1_data;  /* 0x02 */
  volatile unsigned short csr2;        /* 0x04 */
  volatile unsigned short port2_data;  /* 0x06 */
  volatile unsigned short vector;      /* 0x08 */
};

struct flex_struct *flex_io;

int
l2_flex_init()
{
  unsigned long laddr;
  sysBusToLocalAdrs(0x29,(char *)FLEX_ADDR,(char **)&laddr);
  flex_io = (struct flex_struct *)laddr;
  flex_io->csr1 = 0x8000;  /* reset FLEX_IO */
  printf("l2_flex_init:  FLEX_IO address = 0x%08x\n",laddr);
}

int
l2_ADB_reset(char *ADB_name)
{
  int  j;

  if(strcmp(ADB_name,NULL) == 0) {
    /* reserved to reset all bad ADB crates in ons shot */
    printf("l2_ADB_reset:  ADB_name = %9.9s \n",ADB_name);
    return(200);
  }

  j = l2_ADB_name2id(ADB_name,0);       /* get correspondence ADB<->id */
  if      (j == -1) {
    return(-1);
  }
  else if (j < 16) {
    flex_io->port1_data = (1<<j);         /* enable  Direct VME output */
    taskDelay((sysClkRateGet()/1000)*15); /* delay = 1msec * 15        */
    flex_io->port1_data = 0;              /* disable Direct VME output */
  }
  else {
    flex_io->port2_data = (1<<(j-16));    /* enable  Direct VME output */
    taskDelay((sysClkRateGet()/1000)*15); /* delay = 1msec * 15        */
    flex_io->port2_data = 0;              /* disable Direct VME output */

  }

  printf("l2_ADB_reset:  ADB crate \"%s\" was reseted \n",ADB_name);
  return(j);
}

int
l2_ADB_reset_VMEpulse(char *ADB_name)
{
  int  j;

  if(strcmp(ADB_name,NULL) == 0) {
    /* reserved to reset all bad ADB crates in ons shot */
    printf("l2_ADB_reset:  ADB_name = %9.9s \n",ADB_name);
    return(200);
  }

  j = l2_ADB_name2id(ADB_name,0);      /* get correspondence ADB<->id */
  if      (j == -1) {
    return(-1);
  }
  else if (j < 16) {
    flex_io->port1_data = (1<<j);      /* enable one reset channel */
    flex_io->csr1       = 0x101;       /* send reset by one VME Pulse */
  }
  else {
    flex_io->port2_data = (1<<(j-16)); /* enable one reset channel */
    flex_io->csr2       = 0x101;       /* send reset by one VME Pulse */
  }

  printf("l2_ADB_reset:  ADB crate \"%s\" was reseted \n",ADB_name);
  return(j);
}

int
l2_ADB_name2id(char *ADB_name, int prn)
{
  char tmp[80];
  int  j,res;

  sprintf(tmp, "[%s]", ADB_name);

  for(j=0; j<NL2CH; j++)
  {
    if(strcmp(lookup[j],tmp) == 0)
    {
      res  = j;
      j   += NL2CH;
      if(prn == 1)
	printf("l2_ADB_name2id:  ADB_name = %9.9s -> ADB_id = %d \n",lookup[res],res);
    }
  }

  if(j == NL2CH)
  {
    printf("l2_ADB_name2id:  Wrong ADB_name \"%s\" \n",ADB_name);
    return(-1);
  }

  return(res);
}

static int dead_reset_STOP;
void
l2_status_dead_reset_STOP()
{
  dead_reset_STOP = 1;
}
int
l2_trigger_ERROR_set()
{
  char dddd[80];
  /*
  getFromHost("date",0,0,dddd);
  */
  getFromHost("error_msg l2_check clonalarm l2_check trigger 2 ERROR 1 \"Level 2 problem\"",0,0,dddd);
  printf(" dddd=>%s<\n",dddd);
}

int
l2_trigger_ERROR_clear()
{
  char dddd[80];
  getFromHost("error_msg l2_check clonalarm l2_check trigger 0 INFO 1 \"Level 2 ok\"",0,0,dddd);
  printf(" dddd=>%s<\n",dddd);
}


#define ACTUAL_RESET \
	if (j < 16) { \
      flex_io->port1_data = (1<<j);         /* enable  Direct VME output */ \
	  taskDelay((sysClkRateGet()/1000)*15); /* delay = 1msec * 15        */ \
	  flex_io->port1_data = 0;              /* disable Direct VME output */ \
	} \
    else { \
	  flex_io->port2_data = (1<<(j-16));    /* enable  Direct VME output */ \
	  taskDelay((sysClkRateGet()/1000)*15); /* delay = 1msec * 15        */ \
      flex_io->port2_data = 0;              /* disable Direct VME output */ \
	}


int
l2_reset()
{
  int j;

  for(j=0; j<NL2CH; j++)
  {
    ACTUAL_RESET;
  }
  return(0);
}





int
l2_status_dead_reset()
{
  FILE *l2log;
  char date[80], tmp[80];
  int  counts[NL2CH], N_reset[NL2CH];
  int  j, mean, err_flag, D_prn=0, N_dead=0;

  dead_reset_STOP = 0;
  for(j=0; j<NL2CH; j++) N_reset[j]=0;

  while(dead_reset_STOP == 0)
  {
    l2log = fopen("/nfs/usr/clas/logs/run_log/l2_status_dead_reset.log", "a+");

    getFromHost("date",0,0,date);
    l2_readscaler(counts);

    mean = 0;
    for(j=0; j<NL2CH; j++)
    {
      mean += counts[j];
    }
    mean = mean / NL2CH;

    if(mean > 30000)  /* report dead channels only if count high enough */
    {
      for(j=0; j<NL2CH; j++)
      {
	    if((counts[j] == 0) && (N_reset[j] < 10))
	    {
	      N_reset[j]++;
	      N_dead++;

          ACTUAL_RESET;

	      if (D_prn == 0)
          {
	        fprintf(l2log,"------------------------------------------------ \n\n");
	        fprintf(l2log,"%s \n",date);
	        D_prn = 1;
	      }
	      fprintf(l2log," reset signal was sent to ADB crate %9.9s \n",lookup[j]);
	    }
	    else if((counts[j] != 0) && (N_reset[j] != 0))
	    {
	      N_reset[j] = 0;
	      if (D_prn == 0)
          {
	        fprintf(l2log,"%s \n",date);
	        D_prn = 1;
	      }
	      fprintf(l2log," l2_status is OK now for ADB crate %9.9s \n",lookup[j]);
	    }
      }

      err_flag = 0;
      for(j=0; j<NL2CH; j++)
      {
	    if(N_reset[j] >= 10)  err_flag = 2+j;
	    if(N_dead > 3)
	      fprintf(l2log,"  mean=%d [%2d] %9.9s %8d Hz\n",mean,j+1,lookup[j],counts[j]);
      }

      if (err_flag >= 2)
      {
	    getFromHost("error_msg l2_check clonalarm l2_check trigger 2 ERROR 1 \"Level 2 problem\"",0,0,tmp);
	    fprintf(l2log," alarm set UP due to N_reset=%d for %9.9s \n",
		N_reset[err_flag-2],lookup[err_flag-2]);
      }
      else
      {
	    getFromHost("error_msg l2_check clonalarm l2_check trigger 0 INFO 1 \"Level 2 OK\"",0,0,tmp);
      }
    }
    
    D_prn  = 0;
    N_dead = 0;
    fclose(l2log);
    taskDelay(sysClkRateGet()*20);  /* 20 sec delay */
  }

  return(0);
}

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/



int
readout1()
{
  unsigned int len, buffer[100];
  int i,j;
  char qui='z';
  ptr = (unsigned int *) addr;

  sis3801reset(addr);
  sis3801clear(addr);
  sis3801enablenextlogic(addr);
  sis3801nextclock(addr);
 
  while(qui!='q')
  {
    sis3801nextclock(addr);
    sis3801Uledon(addr);
    while(sis3801status(addr) & FIFO_EMPTY)
    {
      taskDelay(1);
    }
    printf("Status: %08x\n",sis3801status(addr));
    for(j=0;j<32;j++)
    printf("%d FIFO: 0x%08x\n",j,sis3801readfifo(addr));  
    len = sis3801read(addr,buffer);
    for(i=0; i<len; i++) printf("value=%08x\n",buffer[i]);
    printf("... done.\n");
    /*sis3801clear(addr);*/   
    printf("Enter 'q' to quit or anyother to continue\n");
    scanf("%s",&qui);
  }  

  return(0);
} 
  
/***********************************************************************/

void  scalIntUser(int arg)
{
  int ii;
  if ((pf->csr) & FIFO_EMPTY){
    logMsg ("scalIntUser: FIFO EMPTY\n",0,0,0,0,0,0); 
  printf("data");  
  }else {
  for(ii=0;ii<<arg;ii++){
    /*printf("data");*/ 
   scalData[ii] = scalData[ii]+pf->fifo[0];
    
  }
 }
}

/************************************************************************/
void scalPrint()
{
  int ii;
  printf(" scaler data (Sums):\n");
  for(ii=1;ii<32;ii+=4){
    printf("SCAL%02d: %10d SCAL%02d: %10d SCAL%02d: %10d SCAL%02d: %10d\n",
	   ii,scalData[ii-1],ii+1,scalData[ii],ii+2,scalData[ii+1],ii+3,scalData[ii+2]);
  }
}
/*************************************************************************/
void scalPulse(int count)
{
  int ii;
  if(pf) {
    if(pf->csr&ENABLE_TEST){
      for(ii=0;ii<count;ii++)
	pf->test = 1;
    }else {
      printf("Mode_disable call test(mode)\n");
    }
  } else {
printf("call init\n");
 }
} 
/**************************************************************************/
  
int
l2_input(unsigned int counts[NL2CH])
{
  int j;
  int time;
  sis3801reset(addr);
  sis3801clear(addr);
  sis3801enablenextlogic(addr);
  sis3801nextclock(addr);
  sis3801Uledon(addr);
  printf("Enter time delay --> ");
  scanf("%d",&time);
  while(sis3801status(addr) & FIFO_EMPTY)
  {
    taskDelay(sysClkRateGet()*time); /* 1 sec delay */
    sis3801nextclock(addr);
  }

  for(j=0; j<NL2CH; j++) counts[j] = sis3801readfifo(addr);  
  sis3801clear(addr);
   
  return(0);
} 

int pp(){
printf("pepepe");
return(0);
}

int
l2_delay()

{
  int counts[NL2CH], j;
  printf("test123");
  l2_input(counts);
  for(j=0; j<NL2CH; j++)
  {
    printf("[%2d] %9.9s %8d Hz\n",j,lookup[j],counts[j]);  
  }

  return(0);
} 

/**************************************************************************/

int
junk(unsigned int counts[NL2CH])
{
  int j;
  int time;
  sis3801reset(addr);
  sis3801clear(addr);
  sis3801enablenextlogic(addr);
  sis3801nextclock(addr);
  sis3801Uledon(addr);
  printf("Enter time delay --> ");
  scanf("%d",&time);
  while(sis3801status(addr) & FIFO_EMPTY)
  {
	taskDelay(sysClkRateGet()*time); /* 1 sec delay */
    sis3801nextclock(addr);
  }

  for(j=0; j<NL2CH; j++) counts[j] = sis3801readfifo(addr);  
  sis3801clear(addr);
   
  return(0);
} 

int
hist()

{
  FILE *stream;
  FILE *log;
  int counts[NL2CH], j;
  junk(counts);
  stream=fopen("/usr/local/clas/devel/source/vxworks/code/temp/snapshot.txt", "w");
  log=fopen("/usr/local/clas/devel/source/vxworks/code/temp/log.txt", "a+");
  for(j=0; j<NL2CH; j++)
  {
  if(j==0){
  fclose(stream);
  stream=fopen("/usr/local/clas/devel/source/vxworks/code/temp/snapshot.txt", "a+");
  }
 
   printf("[%2d] %9.9s %8d Hz\n",j,lookup[j],counts[j]);  
    if(stream == NULL) {
    printf("Error creating file!\n");
    }else{
    fprintf(stream,"[%2d] %9.9s %8d Hz\n",j,lookup[j],counts[j]);
    }   
    /*fclose(stream);*/
    if(log == NULL) {
     printf("Error appending to hist.txt\n"); 
    }else{ 
    fprintf(log,"[%2d] %9.9s %8d Hz\n",j,lookup[j],counts[j]);
     }
 }
  fclose(stream);
  fclose(log);
  return(0);
} 
/*****************************************************************************/

#else /* no UNIX version */

void
sis3801_dummy()
{
  return;
}

#endif











