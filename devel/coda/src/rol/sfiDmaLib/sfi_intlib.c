/********************************************************************************
 sfi_intLib.c --    Primitive trigger control for the SFI 340 using available
                     interrupt sources including CEBAF designed AUX
                     Trigger supervisor interface card.




 Routines:


*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <intLib.h>
#include <iv.h>
#include <logLib.h>
#include <taskLib.h>
#include <vxLib.h>

/* Local Headers */
#include "sfi.h"
#include "sfi_fb_macros.h"

/* Define Interrupt Vector and VME level */
#define SFI_INT_VEC   0xec
#define SFI_VME_INT_LEVEL 5
#define MAX_EVENT_LENGTH  60000
#define SFI_POLLING   0
#define SFI_INTERRUPT 1

IMPORT struct sfiStruct sfi;

IMPORT  STATUS sysBusToLocalAdrs(int, char *, char **);
IMPORT  STATUS sysIntEnable(int);

#ifdef VMIC
IMPORT  STATUS vmivme7050IntConnect();
#else
IMPORT  STATUS intDisconnect();
#endif

LOCAL BOOL        sfiIntRunning  = FALSE;               /* running flag */
LOCAL VOIDFUNCPTR sfiIntRoutine  = NULL;                /* user interrupt service routine */
LOCAL int         sfiIntArg      = 0;                   /* arg to user routine */
LOCAL UINT32      sfiIntLevel    = SFI_VME_INT_LEVEL;   /* default VME interrupt level */
LOCAL UINT32      sfiIntVec      = SFI_INT_VEC;         /* default interrupt Vector */
LOCAL UINT32      sfiIntMode     = SFI_INTERRUPT;       /* !=0  interrupt mode  */

/* Global variables */
unsigned int sfiIntCount;
unsigned int sfiTrigSource;
unsigned int sfiAuxPort;
unsigned int sfiCpuMemOffset=0x08000000;
unsigned int sfiLateFail, sfiSyncFlag;
unsigned int sfiNeedAck=0;

/* prototypes */
void sfiIntAck(UINT32 intMask, int arg);

/* Define Hardware Interrupt sources */
#define NIM1_SOURCE 0x01
#define NIM2_SOURCE 0x02
#define NIM3_SOURCE 0x04
#define ECL1_SOURCE 0x08
#define AUX_SOURCE  0x10

/* Define global variable for AUX board revision */
unsigned int sfiAuxVersion = 0; /* Default is an invalid Version ID */

#define SFI_AUX_VERSION_MASK  0x00006000 
#define SFI_AUX_VERSION_1     0x00006000
#define SFI_AUX_VERSION_2     0x00004000

#define EIEIO    __asm__ volatile ("eieio")
#define SYNC     __asm__ volatile ("sync")


void
sfiAuxWrite(unsigned int val32)
{
  *sfi.sequencerDisable = 0;
  *sfi.writeAuxReg = val32;
  *sfi.writeVmeOutSignalReg = 0x4000;
  *sfi.generateAuxB40Pulse = 0;
  *sfi.writeVmeOutSignalReg = 0x40000000;
}

unsigned long
sfiAuxRead()
{
  int xx=0;
  unsigned long val32 = 0xffffffff;
  
  *sfi.sequencerDisable = 1;
  *sfi.writeVmeOutSignalReg = 0x2000;
  while ( (xx<10) && (val32 == 0xffffffff) ) {
    val32 = *sfi.readLocalFbAdBus;
    xx++;
  }
  *sfi.writeVmeOutSignalReg = 0x20000000;

  return (val32);
}

int
sfiAuxInit()
{
  unsigned long rval = 0;

  /* Check if SFI AUX board is readable */
  rval = sfiAuxRead();
  if (rval == 0xffffffff) {
    printf("sfiAuxInit: ERROR: AUX card not addressable\n");
    return(-1);
  } else {
    sfiAuxWrite(0x8000); /* Reset the board */
    sfiAuxVersion = (SFI_AUX_VERSION_MASK & rval);
    if(sfiAuxVersion == SFI_AUX_VERSION_1) {
      printf("sfiAuxInit: sfiAux Card Version = 1\n");
    } else if(sfiAuxVersion == SFI_AUX_VERSION_2) {
      printf("sfiAuxInit: sfiAux Card Version = 2\n");
    }

  }

  /* Set AUX port as the interrupt source */
  sfiTrigSource=AUX_SOURCE;

  return(0);
  
}



/*******************************************************************************
*
* sfiInt - default interrupt handler
*
* This rountine handles the sfi interrupt.  A user routine is
* called, if one was connected by sfiIntConnect().
*
* RETURNS: N/A
*
* SEE ALSO: sfiIntConnect()
*/

LOCAL void 
sfiInt (void)
{

  /* Clear inturrupt source */
  *sfi.VmeIrqMaskReg  = (sfiTrigSource<<8);      /* Clear VME Source */

  sfiIntCount++;
      
  if (sfiIntRoutine != NULL)       /* call user routine */
    (*sfiIntRoutine) (sfiIntArg);
  
  
}

/*******************************************************************************
*
* sfiIntUser - default user interrupt routine
*
*  This routine can be modified to add the code the User wishes to execute
*  upon recieving a scaler interrupt
*
*  RETURNS: N/A
*
*/
void
sfiIntUser (int arg)
{

  logMsg("sfiIntUser: GOT Interrupt\n",0,0,0,0,0,0);

  /* acknowledge Interrupt - reenable */
  sfiIntAck(arg,0);
}


/*******************************************************************************
*
* sfiIntConnect - connect a user routine to the sfi interrupt
*
* This routine specifies the user interrupt routine to be called at each
* interrupt
*
* RETURNS: OK, or ERROR if sfiInt() interrupt handler is not used.
*/

STATUS 
sfiIntConnect (VOIDFUNCPTR routine, int arg)
{
  sfiIntRoutine = routine;
  sfiIntArg = arg;
  
  return (OK);
}


/*******************************************************************************
*
* sfiIntInit - initialize sfier for interrupts
*
*
* RETURNS: OK, or ERROR if the sfier address is invalid.
*/

STATUS 
sfiIntInit (UINT32 source, UINT32 level, UINT32 vector, int mode)
{

  int res;
  unsigned int laddr=0;

  /* Reset All Interrupts */
  *sfi.VmeIrqMaskReg  = 0xff00;

  /* Set defaults */
  if((level==0)||(level>7))      level=SFI_VME_INT_LEVEL;
  if((vector<=32)||(vector>255)) vector = SFI_INT_VEC;
  if((source==0)||(source>0x1f)) source=AUX_SOURCE;

  if(mode != 0) 
    sfiIntMode=SFI_POLLING;
  else
    sfiIntMode=SFI_INTERRUPT;

  sfiIntLevel = level;
  sfiIntVec   = vector;
  sfiTrigSource = source;

  sfiIntCount = 0;
  sfiIntRunning = 0;
  sfiNeedAck = 0;

  if(sfiIntMode==SFI_INTERRUPT) {
    /* Enable  SFI to Interrupt VME bus */  
    *sfi.VmeIrqLevelAndVectorReg = (0x800 | (level<<8) | vector);


    /* Find Local memory offset as seen by the SFI for purposes of
       DMA transfer of data into the local memory */
    res = sysLocalToBusAdrs(0x09,0,(char **)&laddr);
    if (res != 0) {
      printf("sfiIntInit: ERROR in sysLocalToBusAdrs(0x09,0,&laddr) \n");
      return(ERROR);
    } else {
      sfiCpuMemOffset = laddr;
    }




#ifdef VMIC
    vmivme7050IntConnect(INUM_TO_IVEC(vector),sfiInt,0);
    sfiCpuMemOffset = 0;
#else

#ifdef VXWORKSPPC
  if((intDisconnect(INUM_TO_IVEC(vector)) !=0))
    printf("Error disconnecting Interrupt\n");
#endif
  intConnect(INUM_TO_IVEC(vector),sfiInt,0);

#endif

  }

  return(OK);
}

/*******************************************************************************
*
* sfiIntEnable - enable sfier interrupts
*
* This routine takes an interrupt source mask and enables interrupts
* for the initialized sfi.
*
* RETURNS: OK, or ERROR if not intialized.
*/

STATUS 
sfiIntEnable (UINT32 intMask, int iflag)
{
  int lock_key;

  if(intMask==0) intMask=sfiTrigSource;

  if(sfiIntMode==SFI_INTERRUPT) {
    lock_key = intLock();

    *sfi.VmeIrqMaskReg  = (intMask<<8);         /* Reset any Pending Trigger */ 
    *sfi.VmeIrqMaskReg  = intMask;              /* Enable source */
    sysIntEnable(SFI_VME_INT_LEVEL);                    /* Enable VME Level 5 interrupts */


    if(iflag) {
      /*Set AUX Port for Trigger Supervisor triggers*/
      sfiAuxWrite(0);
    }else{
      /*Set AUX Port for External triggers*/
      if(intMask == AUX_SOURCE) {
	if(sfiAuxVersion == SFI_AUX_VERSION_2)
	  sfiAuxWrite(0x300);
	else
	  sfiAuxWrite(0x180);
      }
    }
    sfiIntRunning = 1;
    intUnlock(lock_key);

  }else{

    *sfi.VmeIrqLevelAndVectorReg = ( (SFI_VME_INT_LEVEL<<8) | SFI_INT_VEC);     /* Enable Internal Interrupts */
    *sfi.VmeIrqMaskReg  = (intMask<<8);                                       /* Reset any Pending Trigger */ 
    *sfi.VmeIrqMaskReg  = intMask;                                          /* Enable source*/

    if(iflag) {
      /*Set AUX Port for Trigger Supervisor triggers*/
      sfiAuxWrite(0);
    }else{
      /*Set AUX Port for External triggers*/
      if(intMask == AUX_SOURCE) {
	if(sfiAuxVersion == SFI_AUX_VERSION_2)
	  sfiAuxWrite(0x300);
	else
	  sfiAuxWrite(0x180);
      }
    }

  }

  return(OK);
}


/*******************************************************************************
*
* sfiIntDisable - disable the sfi interrupts
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS 
sfiIntDisable (UINT32 intMask, int iflag)
{

  if(intMask==0) intMask=sfiTrigSource;
  *sfi.VmeIrqMaskReg  = (intMask<<8);      /* Clear Source  */

  if(sfiIntMode==SFI_INTERRUPT) {
    sysIntDisable(SFI_VME_INT_LEVEL);        /* Disable VME Level 5 interrupts */
    sfiIntRunning = 0;
  }

  return(OK);
}


/*******************************************************************************
*
* sfiIntAck - Acknowledge/renable the sfi interrupts
*
* RETURNS: OK, or ERROR if not initialized
*/

void 
sfiIntAck(UINT32 intMask, int iflag)
{

  if(intMask==0) intMask=sfiTrigSource;

  sfiNeedAck = 0;

  if(sfiIntMode==SFI_POLLING)
    *sfi.VmeIrqMaskReg  = (intMask<<8);      /* Clear Source  */

  if(intMask == AUX_SOURCE) {
    *sfi.writeVmeOutSignalReg = 0x1000;       /* Set A10 Ack       */
    *sfi.VmeIrqMaskReg  = intMask;            /* Enable Source  */
    *sfi.writeVmeOutSignalReg = 0x10000000;   /* Clear A10 Ack     */
  } else {
    *sfi.VmeIrqMaskReg  = intMask;            /* Enable Source  */
  }

}

int 
sfiTrigTest()
{
  int ii=0;
  /* NOTE: See that internal VME IRQ is set on the SFI*/
  ii = (((*sfi.VmeIrqLevelAndVectorReg) & 0x4000) != 0);
  return(ii);
}


STATUS
sfiIntClearCount()
{
  if(sfiIntRunning)
    return(ERROR);

  sfiIntCount = 0;
  return(OK);
}


unsigned int
sfiAuxTrigType(int iflag)
{
  unsigned int tt;

  if(iflag) {
    /* Read Trigger type from TS AUX port */
    sfiAuxPort = sfiAuxRead();
    if(sfiAuxPort == 0xffffffff)
      logMsg("sfittype: ERROR: Error reading Aux port\n",0,0,0,0,0,0);

    if(sfiAuxVersion == SFI_AUX_VERSION_2)
      tt       = ((sfiAuxPort&0xfc)>>2);
    else
      tt       = ((sfiAuxPort&0x3c)>>2);

    sfiLateFail = ((sfiAuxPort&0x02)>>1);
    sfiSyncFlag = ((sfiAuxPort&0x01));

  } else {
    /* Set default trigger type to 1 */
    tt=1;
  }

  return(tt);
}

/* Functions for Asyncronous Operations */
int
sfiSeqStatus()
{
  unsigned int reg32=0;
  int res;

  reg32 = *sfi.sequencerStatusReg;

  switch (reg32&0x8001) {
  case 0x00000000: /* Disabled */
    res = 0;
    break;
  case 0x00000001: /* Busy */
    res = 1;
    break;
  case 0x00008000: /* Error */
    res = -1;
    break;
  case 0x00008001: /* OK - Done */
    res = 2;
  }

  return(res);
}

unsigned int
sfiReadSeqFifo()
{
  register unsigned int reg32;
  volatile unsigned int fval;
  int kk;

  reg32 = *sfi.readSeqFifoFlags;
  if((reg32 & 0x00000010) == 0x00000010)  /* Empty */
    {
      fval = *sfi.readSeq2VmeFifoBase; /* Dummy read */
      reg32 = *sfi.readSeqFifoFlags;
      if((reg32&0x00000010) == 0x00000010) {
	/* STILL EMPTY - Error !!! */
	return(0);
      }
    }
	   
  /* read fifo */
  fval = *sfi.readSeq2VmeFifoBase;
  
  return(fval);
}

int 
sfiSeqDone()
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;

  int kk;

  /* wait for sequencer done */
  do
  {
	/* Sergey: somebody forgot this again ! */
    EIEIO;
    SYNC;

    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001) {
    case 0x00008001:
      /* OK */
      Return = 0;
      Exit = 1;
      break;
    case 0x00000001:
      /* Not Finished */
      break;
    case 0x00000000:
      /* Not Initialized */
      Return = *sfi.sequencerStatusReg & 0x0000ffff;
      Return |= 0x00020000;
      Exit = 1;
      break;
    case 0x00008000:
      /* Bad Status is set, we will see */
      Return = *sfi.sequencerStatusReg & 0x0000ffff;
      Return |= 0x00010000;
      Exit = 1;
      break;
    }
  } while(!Exit);

  return(Return);
}

void
sfiRamStart(unsigned int ramAddr, unsigned int *data)
{

  *fastLoadDmaAddressPointer = ((unsigned int) (data) + sfiCpuMemOffset);

  *(fastEnableRamSequencer + (ramAddr>>2))  = 1;       /* start Sequencer at RAM Address */

}



int 
sfiReadBlock(int pa, int sa, unsigned int *data, int len) 
{
  unsigned int bufp, rb, lenb;
  int res;

/* Make maximum len(lw) equal to 1/4 minus header of Max event length */
 if (len <= 0) len = (MAX_EVENT_LENGTH>>4) - 2;
 lenb = (len<<2);

 bufp = (unsigned int) (data) + sfiCpuMemOffset;

 if (pa >= 0) {
   if (sa > 0) {
     res = fb_frdb_1(pa,sa,bufp,lenb,&rb,1,0,0,0,0x0a,0,0,1);
   }else{
     res = fb_frdb_1(pa,0,bufp,lenb,&rb,1,0,1,0,0x0a,0,0,1);
   }
 } else {
     res = fb_frdb_1(0,0,bufp,lenb,&rb,1,1,1,0,0x0a,1,1,1);
 }

 if ((rb > (lenb+4))||(res != 0)) {
   *data++ = 0xfb000bad;
   logMsg("sfiReadBlock Error pa=%d res=0x%x maxBytes=%d retBytes=%d \n",pa,res,lenb,rb,0,0);
   sfi_error_decode(0x3);
   return(1);
 }else{
   return((rb>>2));
 }

  return(-1);
}

int 
sfiReadBlock64(int pa, int sa, unsigned int *data, int len) 
{
  unsigned int bufp, rb, lenb;
  int res;

/* Make maximum len(lw) equal to 1/4 minus header of Max event length */
  if (len <= 0) len = (MAX_EVENT_LENGTH>>4) - 2;
  lenb = (len<<2);

/* Check for 8 byte boundary for address */
  if( ( (unsigned int) (data)&0x7) ) {
    *data++ = 0xdaffffff;
    bufp = (unsigned long) (data) + sfiCpuMemOffset;
  } else {
    bufp = (unsigned long) (data) + sfiCpuMemOffset;
  }

  if (pa >= 0) {
    if (sa > 0) {
      res = fb_frdb_1(pa,sa,bufp,lenb,&rb,1,0,0,0,0x08,0,0,1);
    }else{
      res = fb_frdb_1(pa,0,bufp,lenb,&rb,1,0,1,0,0x08,0,0,1);
    }
  } else {
    res = fb_frdb_1(0,0,bufp,lenb,&rb,1,1,1,0,0x08,1,1,1);
  }

  if ((rb > (lenb+4))||(res != 0)) {
    *data++ = 0xfb000bad;
    logMsg("sfiReadBlock64 Error pa = %d res = 0x%x maxbytes = %d returnBytes = %d \n",pa,res,lenb,rb,0,0);
    sfi_error_decode(0x3);
    return(1);
  }else{
    return((rb>>2));
  }

  return(-1);
}
