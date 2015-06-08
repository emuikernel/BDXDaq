/****************************************************************
*
* header file for use of Struck sfi with CODA crl (version 2.0)
*
*                             DJA   March 1996
*
*****************************************************************/
#ifndef __SFI__
#define __SFI__

/* Global variable to hold the source mask and an array to hold 
   valid trigger sources */
unsigned long sfiTrigSource;
unsigned long sfiAuxPort;

/* Global variable to hold the SFI Base address and OFFSET of 
   the local CPU memory as seen by the SFI VME Master. This 
   offset is gotten by a call to sysBusToLocalAdrs() */
unsigned long sfi_vme_base_address;
unsigned long sfi_cpu_mem_offset;


#include "sfi.h"
#include "sfi_fb_macros.h"





/******************************************************************/
/* trigger support routines (former sfi_triglib.h) */

/* sfi_trigLib.c -- Primitive trigger control for the SFI 340 using available
                     interrupt sources including CEBAF designed AUX
                     Trigger supervisor interface card.
                            intmask -- refers to the bit pattern one wishes
                                       to use to enable certain trigger sources
                                       (see the VME IRQ Source and Mask Register)
                                            NIM1,NIM2,NIM3
                                            ECL1
                                            AUX_B42
                                            etc...

 File : sfi_trigLib.h

 Supported Interrupt/Trigger sources:

       code       Hardware type
        1     SFI Trigger Supervisor AUX Interface

 Routines:
           void sfiAuxWrite();       write 32bit word to AUX port
           void sfiAuxRead();        read 32bit word from AUX port
	   void sfitriglink();       link in trigger isr
	   void sfitenable();        enable trigger
	   void sfitdisable();       disable trigger
           void sfitack();           Acknowledge/Reset trigger
	   char sfittype();          read trigger type
	   int  sfittest();          test for trigger (POLL)

------------------------------------------------------------------------------*/

/* Define Hardware sources */
#define AUX_SOURCE 1

/* Define Interrupt Vector and VME level */
#define SFI_VEC   0xec
#define SFI_LEVEL 5

#ifdef VXWORKSPPC
#define INUM_TO_IVEC(intNum)    ((VOIDFUNCPTR *) (intNum))
#else
#define INUM_TO_IVEC(intNum)    ((VOIDFUNCPTR *) (intNum<<2))
#endif

/* Define global variable for AUX board revision */
unsigned int sfiAuxVersion = 0; /* Default is an invalid Version ID */

#define SFI_AUX_VERSION_MASK  0x00006000 
#define SFI_AUX_VERSION_1     0x00006000
#define SFI_AUX_VERSION_2     0x00004000


void
sfiAuxWrite(val32)
     unsigned long val32;
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
  *sfi.sequencerDisable = 0;
  *sfi.writeVmeOutSignalReg = 0x2000;
  while ( (xx<10) && (val32 == 0xffffffff) )
  {
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
    printf("sfiAuxInit: sfiAuxVersion = 0x%x\n",sfiAuxVersion);
  }

  return(0);
  
}

void 
sfitriglink(int code, VOIDFUNCPTR isr)
{

/* Enable  SFI to Inturrupt VME bus */  
    *sfi.VmeIrqLevelAndVectorReg = (0x800 | (SFI_LEVEL<<8) | SFI_VEC);

#ifdef VXWORKSPPC
  if((intDisconnect(INUM_TO_IVEC(SFI_VEC)) !=0))
     printf("Error disconnecting Interrupt\n");
#endif
  intConnect(INUM_TO_IVEC(SFI_VEC),isr,0);

}

void 
sfitenable(int code, unsigned int intMask)
{
 int lock_key;

#ifndef POLLING_MODE
  lock_key = intLock();

  *sfi.VmeIrqMaskReg  = (intMask<<8);       /* Reset any Pending Trigger */ 
  *sfi.VmeIrqMaskReg  = intMask;            /* Enable source */
  sysIntEnable(SFI_LEVEL);                  /* Enable VME Level 5 interrupts */
#ifdef VXWORKSPPC
  intEnable(11);       /* Enable VME Interrupts on IBC chip */
#endif

#ifdef TRIG_SUPV
/*Set AUX Port for Trigger Supervisor triggers*/
  sfiAuxWrite(0);
#else
/*Set AUX Port for External triggers*/
  if(intMask == 0x10)
  {
    if(sfiAuxVersion == SFI_AUX_VERSION_2)
      sfiAuxWrite(0x300);
    else
      sfiAuxWrite(0x180);
  }
#endif
  intUnlock(lock_key);

#else

  *sfi.VmeIrqLevelAndVectorReg = ( (SFI_LEVEL<<8) | SFI_VEC); /* Enable Internal Interrupts */
  *sfi.VmeIrqMaskReg  = (intMask<<8);                         /* Reset any Pending Trigger */ 
  *sfi.VmeIrqMaskReg  = intMask;                              /* Enable source*/

#ifdef TRIG_SUPV
/*Set AUX Port for Trigger Supervisor triggers*/
  sfiAuxWrite(0);
#else
/*Set AUX Port for External triggers*/
  if(intMask == 0x10)
  {
    if(sfiAuxVersion == SFI_AUX_VERSION_2)
      sfiAuxWrite(0x300);
    else
      sfiAuxWrite(0x180);
  }
#endif

#endif
}

void 
sfitdisable(int code, unsigned int intMask)
{

  *sfi.VmeIrqMaskReg  = (intMask<<8);      /* Clear Source  */
#ifndef POLLING
  sysIntDisable(SFI_LEVEL);        /* Disable VME Level 5 interrupts */
#endif

/*Reset AUX Port */
  if(intMask == 0x10)
  {
    sfiAuxWrite(0x8000);
  }
}

void 
sfitack(int code, unsigned int intMask)
{
#ifdef POLLING_MODE
  *sfi.VmeIrqMaskReg  = (intMask<<8);      /* Clear Source  */
#endif

  if(intMask == 0x10)
  {
    *sfi.writeVmeOutSignalReg = 0x1000;       /* Set A10 Ack       */
    *sfi.VmeIrqMaskReg  = intMask;            /* Enable Source  */
    *sfi.writeVmeOutSignalReg = 0x10000000;   /* Clear A10 Ack     */
  }
  else
  {
    *sfi.VmeIrqMaskReg  = intMask;            /* Enable Source  */
  }
}

unsigned long 
sfittype(int code)
{
  unsigned long tt;

#ifdef TRIG_SUPV
/* Read Trigger type from TS AUX port */
  sfiAuxPort = sfiAuxRead();
  if(sfiAuxPort == 0xffffffff)
    logMsg("sfittype: ERROR: Error reading Aux port\n",0,0,0,0,0,0);

  if(sfiAuxVersion == SFI_AUX_VERSION_2)
    tt       = ((sfiAuxPort&0xfc)>>2);
  else
    tt       = ((sfiAuxPort&0x3c)>>2);

  lateFail = ((sfiAuxPort&0x02)>>1);
  syncFlag = ((sfiAuxPort&0x01));

#else
/* Set default trigger type to 1 */
  tt=1;
#endif
  return(tt);
}

int 
sfittest(int code)
{
  int ii=0;
  /* NOTE: See that internal VME IRQ is set on the SFI*/
  ii = (((*sfi.VmeIrqLevelAndVectorReg) & 0x4000) != 0);
  return(ii);
}

/******************************************************************/
/******************************************************************/




/* define CODA readout list specific routines/definitions */
static int padr, sadr;
static int SFI_handlers,SFIflag;
static unsigned long SFI_isAsync;

#define SFI_REPORT_ERROR  sfi_error_decode(0)

static void 
fpbr(int pa, long len) 
{
  unsigned long bufp, rb, lenb;
  int res;

  /* Make maximum len(lw) equal to 1/4 minus header of Max event length */
  if (len <= 0) len = (MAX_EVENT_LENGTH>>4) - 2;
  bufp = (unsigned long) (rol->dabufp) + sfi_cpu_mem_offset;
  lenb = (len<<2);

  /*
bufp = 0x08001000; - RABOTET !!!
  */



  if(pa >= 0)
  {
    res = fb_frdb_1(pa,0,bufp,lenb,&rb,1,0,1,0,0x0a,0,0,1);
  }
  else
  {
    res = fb_frdb_1(0,0,bufp,lenb,&rb,1,1,1,0,0x0a,1,1,1);
  }

  /*
logMsg("fpbr: pa=%d res=0x%x maxBytes=%d retBytes=%d fifo=0x%x\n",
      pa,res,lenb,rb,sfiSeq2VmeFifoVal,0);
logMsg("fpbr: len=%d bufp=0x%08x (0x%08x 0x%08x)\n",
      len,bufp,rol->dabufp,sfi_cpu_mem_offset,5,6);
  */
  if((rb > (lenb+4))||(res != 0))
  {
    *rol->dabufp++ = 0xfb000bad;
    logMsg("fpbr error pa=%d res=0x%x maxBytes=%d retBytes=%d fifo=0x%x\n",
      pa,res,lenb,rb,sfiSeq2VmeFifoVal,0);
    logMsg("fpbr error len=%d bufp=0x%08x (0x%08x 0x%08x)\n",
      len,bufp,rol->dabufp,sfi_cpu_mem_offset,5,6);
    sfi_error_decode(0x3);
  }
  else
  {
    rol->dabufp += (rb>>2);
  }

  return;   

fooy:
  SFI_REPORT_ERROR;
  return;
}

static void 
fpbr2(int pa, int sa, long len) 
{
  unsigned long bufp, rb, lenb;
  int res;

/* Make maximum len(lw) equal to 1/4 minus header of Max event length */
 if (len <= 0) len = (MAX_EVENT_LENGTH>>4) - 2;
 bufp = (unsigned long) (rol->dabufp) + sfi_cpu_mem_offset;
 lenb = (len<<2);

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
   *rol->dabufp++ = 0xfb000bad;
   logMsg("fpbr2 error pa=%d res=0x%x maxBytes=%d retBytes=%d \n",pa,res,lenb,rb,0,0);
   sfi_error_decode(0x3);
 }else{
   rol->dabufp += (rb>>2);
 }

 return;   
 fooy:
  SFI_REPORT_ERROR;
  return;
}

static void 
fpbrf(int pa, long len) 
{
  unsigned long bufp, rb, lenb;
  int res;

/* Make maximum len(lw) equal to 1/4 minus header of Max event length */
  if (len <= 0) len = (MAX_EVENT_LENGTH>>4) - 2;
  lenb = (len<<2);
/* Check for 8 byte boundary for address */
  if( ( (unsigned long) (rol->dabufp)&0x7) ) {
    *rol->dabufp++ = 0xdaffffff;
    bufp = (unsigned long) (rol->dabufp) + sfi_cpu_mem_offset;
  } else {
    bufp = (unsigned long) (rol->dabufp) + sfi_cpu_mem_offset;
  }

  if (pa >= 0) {
    res = fb_frdb_1(pa,0,bufp,lenb,&rb,1,0,1,0,0x08,0,0,1);
  } else {
    res = fb_frdb_1(0,0,bufp,lenb,&rb,1,1,1,0,0x08,1,1,1);
  }

  if ((rb > (lenb+4))||(res != 0)) {
    *rol->dabufp++ = 0xfb000bad;
    logMsg("fpbrf error pa = %d res = 0x%x maxbytes = %d returnBytes = %d \n",pa,res,lenb,rb);
    sfi_error_decode(0x3);
  }else{
    rol->dabufp += (rb>>2);
  }

  return;   
 fooy:
  SFI_REPORT_ERROR;
  return;
}

static void 
fpbrf2(int pa, int sa, long len) 
{
  unsigned long bufp, rb, lenb;
  int res;

/* Make maximum len(lw) equal to 1/4 minus header of Max event length */
  if (len <= 0) len = (MAX_EVENT_LENGTH>>4) - 2;
  lenb = (len<<2);
/* Check for 8 byte boundary for address */
  if( ( (unsigned long) (rol->dabufp)&0x7) ) {
    *rol->dabufp++ = 0xdaffffff;
    bufp = (unsigned long) (rol->dabufp) + sfi_cpu_mem_offset;
  } else {
    bufp = (unsigned long) (rol->dabufp) + sfi_cpu_mem_offset;
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
    *rol->dabufp++ = 0xfb000bad;
    logMsg("fpbrf error pa = %d res = 0x%x maxbytes = %d returnBytes = %d \n",pa,res,lenb,rb);
    sfi_error_decode(0x3);
  }else{
    rol->dabufp += (rb>>2);
  }

  return;   
 fooy:
  SFI_REPORT_ERROR;
  return;
}


void 
SFI_int_handler()
{
/* Clear inturrupt source */
  *sfi.VmeIrqMaskReg  = (sfiTrigSource<<8);      /* Clear VME Source */

#ifdef VXWORKSPPC
/*  sysBusIntAck(5); */
#endif

  theIntHandler(SFI_handlers);                   /* Call our handler */
}


#define SFI_TEST  sfittest

#define SFI_INIT {SFI_handlers=0; SFI_isAsync=0; SFIflag=0;}

#define SFI_ASYNC(code) {SFI_handlers=1; SFI_isAsync=1; sfitriglink(code,SFI_int_handler);}

#define SFI_SYNC(code)  {SFI_handlers=1; SFI_isAsync=0;}

#define SFI_SETA(code) {theIntHandler(SFI_handlers);} /* = code; ??? */

#define SFI_SETS(code) SFIflag = code;

#define SFI_ENA(code,val) sfitenable(code,val);

#define SFI_DIS(code,val) sfitdisable(code,val);

#define SFI_ACK(code,val) sfitack(code,val);

#define SFI_CLRS(code) SFIflag = 0;

#define SFI_TTYPE sfittype

#define SFI_START(val)	 {;}

#define SFI_STOP(val)	 {;}

#define SFI_ENCODE(code) code


#endif

