/********************************************************************************
 vme_intLib.c --    Primitive trigger control for the VME 340 using available
                     interrupt sources including CEBAF designed AUX
                     Trigger supervisor interface card.




 Routines:


*/

#define USE_IN_HALLB

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <intLib.h>
#include <iv.h>
#include <logLib.h>
#include <taskLib.h>
#include <vxLib.h>

/* Local Headers */
#include "vmeLib.h"


IMPORT  STATUS sysBusToLocalAdrs(int, char *, char **);
IMPORT  STATUS sysIntEnable(int);

#ifdef VMIC
IMPORT  STATUS vmivme7050IntConnect();
#else
IMPORT  STATUS intDisconnect();
#endif

LOCAL BOOL        vmeIntRunning  = FALSE;               /* running flag */
LOCAL VOIDFUNCPTR vmeIntRoutine  = NULL;                /* user interrupt service routine */
LOCAL int         vmeIntArg      = 0;                   /* arg to user routine */
LOCAL UINT32      vmeIntLevel    = VME_INT_LEVEL;       /* default VME interrupt level */
LOCAL UINT32      vmeIntVec      = VME_INT_VEC;         /* default interrupt Vector */
LOCAL UINT32      vmeIntMode     = VME_INTERRUPT;       /* !=0  interrupt mode  */

/* Global variables */
unsigned int vmeIntCount;
unsigned int vmeTrigSource=0;
unsigned int vmeCpuMemOffset=0x08000000;
unsigned int vmeLateFail, vmeSyncFlag;
unsigned int vmeNeedAck=0;
volatile struct vme_ts  *tsP;
volatile struct vme_tir *tir[2];

/* prototypes */
void vmeIntAck(UINT32 intMask, int arg);

/* Define global variable for TIR board revision */
unsigned int tirVersion = 0; /* Default is an invalid Version ID */

#define EIEIO    __asm__ volatile ("eieio")
#define SYNC     __asm__ volatile ("sync")

#ifdef VXWORKSPPC
#define INUM_TO_IVEC(intNum)    ((VOIDFUNCPTR *) (intNum))
#else
#define INUM_TO_IVEC(intNum)    ((VOIDFUNCPTR *) (intNum<<2))
#endif


int
vmeTirInit(unsigned int tAddr)
{
  int stat = 0;
  unsigned long laddr;
  unsigned short rval;

  if (tAddr == 0) {
    tAddr = TIR_DEFAULT_ADDR;
  }

  stat = sysBusToLocalAdrs(0x29,(char *)tAddr,(char **)&laddr);
  if (stat != 0) {
     printf("tirInit: ERROR: Error in sysBusToLocalAdrs res=%d \n",stat);
  } else {
     printf("TIR address = 0x%x\n",laddr);
     tir[1] = (struct vme_tir *)laddr;
  }

  /* Check if TIR board is readable */
  stat = vxMemProbe((char *)laddr,0,2,(char *)&rval);
  if (stat != 0) {
    printf("tirInit: ERROR: TIR card not addressable\n");
    return(-1);
  } else {
    tir[1]->tir_csr = 0x80; /* Reset the board */
    tirVersion = (unsigned int)rval;
    printf("tirInit: tirVersion = 0x%x\n",tirVersion);
  }

  return(0);
  
}



/*******************************************************************************
*
* vmeInt - default interrupt handler
*
* This rountine handles the vme interrupt.  A user routine is
* called, if one was connected by vmeIntConnect().
*
* RETURNS: N/A
*
* SEE ALSO: vmeIntConnect()
*/

LOCAL void 
vmeInt (void)
{

  vmeIntCount++;
      
  if (vmeIntRoutine != NULL)       /* call user routine */
    (*vmeIntRoutine) (vmeIntArg);
  
  
}

/*******************************************************************************
*
* vmeIntUser - default user interrupt routine
*
*  This routine can be modified to add the code the User wishes to execute
*  upon recieving a scaler interrupt
*
*  RETURNS: N/A
*
*/
void
vmeIntUser (int arg)
{

  logMsg("vmeIntUser: GOT Interrupt\n",0,0,0,0,0,0);

  /* acknowledge Interrupt - reenable */
  vmeIntAck(arg,0);
}


/*******************************************************************************
*
* vmeIntConnect - connect a user routine to the vme interrupt
*
* This routine specifies the user interrupt routine to be called at each
* interrupt
*
* RETURNS: OK, or ERROR if vmeInt() interrupt handler is not used.
*/

STATUS 
vmeIntConnect (VOIDFUNCPTR routine, int arg)
{
  vmeIntRoutine = routine;
  vmeIntArg = arg;
  
  return (OK);
}


/*******************************************************************************
*
* vmeIntInit - initialize vmeer for interrupts
*
*
* RETURNS: OK, or ERROR if the source address is invalid.
*/

STATUS 
vmeIntInit (UINT32 source, UINT32 level, UINT32 vector, int mode)
{

  int res;
  unsigned int laddr=0;


  /* Set defaults */
  if((level==0)||(level>7))       level = VME_INT_LEVEL;
  if((vector<=32)||(vector>255))  vector = VME_INT_VEC;

  if(source > 0xffff) 
    vmeTrigSource = TS_SOURCE;
  else
    vmeTrigSource = TIR_SOURCE;

  if(mode != 0) 
    vmeIntMode=VME_POLLING;
  else
    vmeIntMode=VME_INTERRUPT;

  /* Set Globals */
  vmeIntLevel = level;
  vmeIntVec   = vector;

  vmeIntCount = 0;
  vmeIntRunning = 0;
  vmeNeedAck = 0;

  if(vmeTrigSource == TIR_SOURCE) {
    res = vmeTirInit(source);
    if(res) {
      printf("vmeIntInit: ERROR: Unable to initialize trigger source\n");
      return(ERROR);
    }
    tir[1]->tir_vec = vmeIntVec;
  }
  if(vmeTrigSource == TS_SOURCE) {
    if(tsP) {
      tsP->intVec = VME_INT_VEC;
    }else{
      printf("vmeIntInit: ERROR: Unable to initialize trigger source\n");
      return(ERROR);
    }
  }


  /* Find Local memory offset as seen by the VME for purposes of
     DMA transfer of data into the local memory */
  res = sysLocalToBusAdrs(0x09,0,(char **)&laddr);
  if (res != 0) {
    printf("vmeIntInit: ERROR in sysLocalToBusAdrs(0x09,0,&laddr) \n");
    return(ERROR);
  } else {
    vmeCpuMemOffset = laddr;
  }


  if(vmeIntMode==VME_INTERRUPT) {

#ifdef VMIC
    vmivme7050IntConnect(INUM_TO_IVEC(vector),vmeInt,0);
    vmeCpuMemOffset = 0;
#else

#ifdef VXWORKSPPC
  if((intDisconnect(INUM_TO_IVEC(vector)) !=0))
    printf("Error disconnecting Interrupt\n");
#endif
  intConnect(INUM_TO_IVEC(vector),vmeInt,0);

#endif

  }

  return(OK);
}

/*******************************************************************************
*
* vmeIntEnable - enable vmeer interrupts
*
* This routine takes an interrupt source  and enables interrupts
* for the initialized vme.
*      if iflag > 0 then the source should be enables for TS mode
*
* RETURNS: OK, or ERROR if not intialized.
*/

STATUS 
vmeIntEnable (UINT32 intMask, int iflag)
{
  int lock_key;

  if(intMask==0) intMask=vmeTrigSource;

  if(vmeIntMode==VME_INTERRUPT) {
    lock_key = intLock();

    sysIntEnable(vmeIntLevel);  /* Enable VME interrupts */

    if(intMask == TIR_SOURCE)
      tir[1]->tir_vec = vmeIntVec;
    if(intMask == TS_SOURCE)
      tsP->intVec = vmeIntVec;
    

    if(iflag) {
      /*Set Source Port for Trigger Supervisor triggers*/
      if(intMask == TIR_SOURCE)
	tir[1]->tir_csr = 0x6;
      
      if(intMask == TS_SOURCE)
	tsP->csr2 |= 0x1800;

    }else{
      /*Set Source Port for External triggers*/
      if(intMask == TIR_SOURCE)
	tir[1]->tir_csr = 0x7;
      
      if(intMask == TS_SOURCE)
	tsP->csr2 |= 0x0000;

    }
    vmeIntRunning = 1;
    intUnlock(lock_key);

  }else{


    if(iflag) {
      /*Set AUX Port for Trigger Supervisor triggers*/
      if(intMask == TIR_SOURCE)
	tir[1]->tir_csr = 0x2;
      
      if(intMask == TS_SOURCE)
	tsP->csr2 |= 0x1000;

    }else{
      /*Set AUX Port for External triggers*/
      if(intMask == TIR_SOURCE)
	tir[1]->tir_csr = 0x3;
      
      if(intMask == TS_SOURCE)
	tsP->csr2 |= 0x0000;

    }
  }

  return(OK);
}


/*******************************************************************************
*
* vmeIntDisable - disable the vme interrupts
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS 
vmeIntDisable (UINT32 intMask, int iflag)
{

  if(intMask==0) intMask=vmeTrigSource;

  if(intMask == TIR_SOURCE)
    tir[1]->tir_csr = 0x80;
  
  if(intMask == TS_SOURCE)
    tsP->csr2 &= ~(0x1800);


  if(vmeIntMode==VME_INTERRUPT) {
    sysIntDisable(VME_INT_LEVEL);        /* Disable VME Level 5 interrupts */
    vmeIntRunning = 0;
  }

  return(OK);
}


/*******************************************************************************
*
* vmeIntAck - Acknowledge/renable the vme interrupts
*
* RETURNS: OK, or ERROR if not initialized
*/

void 
vmeIntAck(UINT32 intMask, int iflag)
{

  if(intMask==0) intMask=vmeTrigSource;

  vmeNeedAck = 0;

  if(intMask == TIR_SOURCE)
    tir[1]->tir_dat = 0x8000;
 
  if(intMask == TS_SOURCE)
    tsP->lrocBufStatus = 0x100;

}

int
vmeIntStatus()
{
  if(vmeIntRunning)
    return(1);
  else
    return(0);

}


int 
vmeTrigTest()
{
  int ii=0;

  /* NOTE: See that internal VME IRQ is set on the VME*/
  if( vmeTrigSource == TIR_SOURCE) {
    ii = (tir[1]->tir_csr)&0x8000;
  }

  return(ii);
}


STATUS
vmeIntClearCount()
{
  if(vmeIntRunning)
    return(ERROR);

  vmeIntCount = 0;
  return(OK);
}


unsigned int
vmeTrigType(UINT32 intMask, int iflag)
{
  unsigned int rval, tt;

  if(intMask == 0) intMask = vmeTrigSource;

  if(iflag) {
    /* Read Trigger type */
    if(intMask == TIR_SOURCE) {
      rval = tir[1]->tir_dat;
      tt = (((rval)&0x3c)>>2);
      vmeSyncFlag = (rval)&0x01;
      vmeLateFail = ((rval)&0x02)>>1;
    }

    if(intMask == TS_SOURCE) {
      rval = tsP->lrocData;
      tt = (((rval)&0xfc)>>2);
      vmeSyncFlag = (rval)&0x01;
      vmeLateFail = ((rval)&0x02)>>1;
    }

  } else {
    /* Just return trigger data register*/
    if(intMask == TIR_SOURCE) {
      tt = (tir[1]->tir_dat)&0x3f;
    }

    if(intMask == TS_SOURCE) {
      tt = (tsP->lrocData)&0xff;
    }
    
  }

  return(tt);
}


/* Functions for Asyncronous Operations */
int
vmeDmaStatus()
{
  int res;

#ifdef USE_IN_HALLB
  res = usrVme2MemDmaDone();
#else
  res = sysVmeDmaDone(10000,1);  /* return final byte count */
#endif

  return(res);
}

/* Start the DMA - from the VME Bus to Local Memory  */
void
vmeDmaStart(UINT32 data, UINT32 vmeAddr, int maxWords)
{
  int res=0;

#ifdef USE_IN_HALLB
  res = usrVme2MemDmaStart(vmeAddr, data, (maxWords<<2));
#else
  res = sysVmeDmaSend(data, vmeAddr, (maxWords<<2),0);
#endif
  if(res<0) {
    logMsg("vmeDmaStart: Error in Starting DMA\n",0,0,0,0,0,0);
  }

}



void
vmeTirPulse(int mode, unsigned short mask)
{

  if(mask == 0) {  /* Clear all bits */
    tir[1]->tir_oport = 0;
    return;
  }

  if(mode >= 2) {                  /* Pulse On and off */
    tir[1]->tir_oport |= mask;
    tir[1]->tir_oport &= ~(mask);
  } else if(mode == 1) {           /* Set */
    tir[1]->tir_oport |= mask;
  } else {                         /* Clear */
    tir[1]->tir_oport &= ~(mask);
  }

}
