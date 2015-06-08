/* FASTBUS Library routines for the STRUCK SFI 340. These are non-standard
   routines adapted from Struck distributed example code. The FASTBUS standard
   Level 1 routines reside in fb_sfi_1.h .

   Adapter: David Abbott  March 1996
            CEBAF Data Acquisition Group
******************************************************************************/

#ifdef VXWORKS

#include <stdio.h>
#include <stdlib.h>

#include <vxWorks.h>
#include <cacheLib.h>
#include <logLib.h>

/* Some local defines for debugging */
#define SFI_READ_ADDR_OFFSET    0x100
#define EIEIO    __asm__ volatile ("eieio")
#define SYNC     __asm__ volatile ("sync")

/*
#define EIEIO    
#define SYNC     
*/

/* Define external Functions */
IMPORT  STATUS sysBusToLocalAdrs(int, char *, char **);

/* Include some useful utility routines and SFI structure definition*/
#include "sfi.h"
#include "coda_sfi.h"


/* This Module holds:  */
/* 
unsigned long fpwc(PAddr, SAddr,WData)
unsigned long fprc(PAddr, SAddr,rData)
unsigned long fpwd(PAddr, SAddr,WData)
unsigned long fprd(PAddr, SAddr,rData)
unsigned long fpwcm(PAddr, SAddr,WData)
unsigned long fprcm(PAddr, SAddr,rData)
unsigned long fpwdm(PAddr, SAddr,WData)
unsigned long fprdm(PAddr, SAddr,rData)
unsigned long fpac(PAddr, SAddr)
unsigned long fpad(PAddr, SAddr)
unsigned long fpsaw(SAddr)
unsigned long fpr()
unsigned long fpw(WData)
unsigned long fprdb(PAddr,SAddr,Buffer,next_buffer,Max_ExpLWord,cnt_RecLWord,Mode,Wait)
unsigned long fprel()
unsigned long fpfifo(fflag)
*/

/* Include the FASTBUS standard Level 1 routines */
#include "fb_sfi_1.h"

/*======================================================================*/
/*
   Funktion     : FWC

   In           : pAddr		primary address
                  sAddr     secondary address
                  lc        limit count

   Out          : returns 0, if no error, else number of errors

   Bemerkungen  : 

*/
/*======================================================================*/

unsigned long 
fpwc(unsigned long PAddr, unsigned long SAddr, unsigned long WData)
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;

  *fastPrimCsr  = PAddr; /* primary address cycle   */
  *fastSecadW   = SAddr; /* secondary address cycle */ 
  *fastRndmWDis = WData;

  /* wait for sequencer done */
  do 
  {
    EIEIO;
    SYNC;

    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
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

/****************/

unsigned long 
fprc(unsigned long PAddr, unsigned long SAddr, volatile unsigned long *rData)
{
  volatile unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;
  volatile unsigned long *vaddr;
  unsigned long    kk;

  *fastPrimCsr  = PAddr; /* primary address cycle   */
  *fastSecadW   = SAddr; /* secondary address cycle */ 
  *fastRndmRDis = reg32;

  sfiSeq2VmeFifoVal=0;

  /* wait for sequencer done */
  do 
  {
    EIEIO;
    SYNC;

    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
      case 0x00008001:
        /* OK */
        Return = 0;
        Exit = 1;
        /* read seq fifo flags */
        /* check if EMPTY ; if EMPTY then Dummy read and Loop until not Empty */
        /*                   else if not empty then read fifo */

        reg32 = *sfi.readSeqFifoFlags;
	    if((reg32 & 0x00000010) == 0x00000010)  /* Empty */
	    {
	      kk=0;
	      vaddr = sfi.readSeqFifoFlags;
	      vaddr += (SFI_READ_ADDR_OFFSET>>2);
	      reg32 = *vaddr;
	      while ( (kk<10) && ((reg32&0x00000010) == 0x00000010) )
          {
	        sfiSeq2VmeFifoVal = *sfi.readSeq2VmeFifoBase;  /* Dummy read */
	        vaddr += (SFI_READ_ADDR_OFFSET>>2);
	        reg32 =  *vaddr;
	        kk++;
	      }
	      if(kk > 1)
          {
	        /* logMsg("fprc: kk = %d\n",kk,0,0,0,0,0); */
	        if(kk >= 10) /* Still Empty */
		    {
		      /* Error !!! */
		      sfiSeq2VmeFifoVal = *sfi.readSeq2VmeFifoBase;
		      logMsg("fprc: Empty Fifo Status=0x%x Val=0x%x\n",reg32,sfiSeq2VmeFifoVal,0,0,0,0);
		      Return = *sfi.sequencerStatusReg & 0x0000ffff;
		      Return |= 0x00100000;
		   
		      break;
		    }
	      }
        }
        
        /* read fifo */

        *rData = *sfi.readSeq2VmeFifoBase;
         
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


/*======================================================================*/
/*
   Funktion     : FWD

   In           : pAddr		primary address
                  sAddr     secondary address
                  lc        limit count
                  &wc       address where word count is stored
                  &ap       address where address pointer is stored
		  
   Out          : returns 0, if no error, else number of errors
                  *wc       word count
                  *ap       address pointer

   Bemerkungen  : 

*/
/*======================================================================*/

unsigned long 
fpwd(unsigned long PAddr, unsigned long SAddr, unsigned long WData)
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;

  *fastPrimDsr  = PAddr; /* primary address cycle   */
  *fastSecadW   = SAddr; /* secondary address cycle */ 
  *fastRndmWDis = WData;

  /* wait for sequencer done */
  do 
  {
    EIEIO;
    SYNC;

    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
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



unsigned long 
fprd(unsigned long PAddr, unsigned long SAddr, unsigned long *rData)
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;

  unsigned long kk;

  *fastPrimDsr  = PAddr; /* primary address cycle   */
  *fastSecadW   = SAddr; /* secondary address cycle */ 
  *fastRndmRDis = reg32;


  /* wait for sequencer done */
  do 
  {
    EIEIO;
    SYNC;

    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
      case 0x00008001:
         /* OK */
         Return = 0;
         Exit = 1;
        /* read seq fifo flags */
        /* check if EMPTY ; if EMPTY then Dummy read and Loop until not Empty */
        /*                   else if not empty then read fifo */


	 reg32 = *sfi.readSeqFifoFlags;
	 if((reg32 & 0x00000010) == 0x00000010)  /* Empty */
	   {
	     kk=0;
	     reg32 = *sfi.readSeqFifoFlags;
	     while ( (kk<20) && ((reg32&0x00000010) == 0x00000010) ) {
	       sfiSeq2VmeFifoVal = *sfi.readSeq2VmeFifoBase; /* Dummy read */
	       reg32 = *sfi.readSeqFifoFlags;
	       kk++;
	     }
	     if(kk >= 20) /* Still Empty */
	       {
		 /* Error !!! */
		 logMsg("fprd: Empty Fifo Status=0x%x Val=0x%x\n",reg32,sfiSeq2VmeFifoVal,0,0,0,0);
		 Return = *sfi.sequencerStatusReg & 0x0000ffff;
		 Return |= 0x00100000;
		 break;
	       }
           }
        
         /* read fifo */

         *rData = *sfi.readSeq2VmeFifoBase;
         
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

/*======================================================================*/
/*
   Funktion     : FWC

   In           : pAddr		primary address
                  sAddr     secondary address
                  lc        limit count
                  &wc       address where word count is stored
                  &ap       address where address pointer is stored
		  
   Out          : returns 0, if no error, else number of errors
                  *wc       word count
                  *ap       address pointer

   Bemerkungen  : 

*/
/*======================================================================*/

unsigned long 
fpwcm(unsigned long PAddr, unsigned long SAddr, unsigned long WData)
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;

  *fastPrimCsrM = PAddr; /* primary address cycle   */
  *fastSecadW   = SAddr; /* secondary address cycle */ 
  *fastRndmWDis = WData;

  /* wait for sequencer done */
  do 
  {
    EIEIO;
    SYNC;

    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
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



unsigned long 
fprcm(unsigned long PAddr, unsigned long SAddr, volatile unsigned long *rData)
{
  volatile unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;

  volatile unsigned long *vaddr;
  volatile unsigned long *vfaddr;
  unsigned long kk;


  *fastPrimCsrM = PAddr; /* primary address cycle   */
  *fastSecadW   = SAddr; /* secondary address cycle */ 
  *fastRndmRDis = reg32;

  /* wait for sequencer done */
  do 
  {
    EIEIO;
    SYNC;

    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
      case 0x00008001:
         /* OK */
         Return = 0;
         Exit = 1;
        /* read seq fifo flags */
        /* check if EMPTY ; if EMPTY then Dummy read and Loop until not Empty */
        /*                   else if not empty then read fifo */


 	 reg32 = *sfi.readSeqFifoFlags;
	 if((reg32 & 0x00000010) == 0x00000010)  /* Empty */
	   {
	     kk=0;
	     vaddr = sfi.readSeqFifoFlags;
	     vfaddr = sfi.readSeq2VmeFifoBase;
	     vaddr += (SFI_READ_ADDR_OFFSET>>2);
	     reg32 = *vaddr;
	     while ( (kk<10) && ((reg32&0x00000010) == 0x00000010) ) {
	       sfiSeq2VmeFifoVal = *vfaddr++;    /* Dummy read */
	       vaddr += (SFI_READ_ADDR_OFFSET>>2);
	       reg32 =  *vaddr;
	       kk++;
	     }
	     if(kk > 1) {
	       /*logMsg("fprcm: kk = %d\n",kk,0,0,0,0,0); */
	       if(kk >= 10) /* Still Empty */
		 {
		   /* Error !!! */
		   sfiSeq2VmeFifoVal = *sfi.readSeq2VmeFifoBase;
		   logMsg("fprcm: Empty Fifo Status=0x%x Val=0x%x\n",reg32,sfiSeq2VmeFifoVal,0,0,0,0);
		   Return = *sfi.sequencerStatusReg & 0x0000ffff;
		   Return |= 0x00100000;
		   
		   break;
		 }
	     }
           }
        
         /* read fifo */

         *rData = *sfi.readSeq2VmeFifoBase;
         
         break;
      case 0x00000001:
         /* Not Finished */
         continue;
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

/*======================================================================*/
/*
   Funktion     : FWDM

   In           : pAddr		primary address
                  sAddr     secondary address
                  lc        limit count
                  &wc       address where word count is stored
                  &ap       address where address pointer is stored
		  
   Out          : returns 0, if no error, else number of errors
                  *wc       word count
                  *ap       address pointer

   Bemerkungen  : 

*/
/*======================================================================*/

unsigned long 
fpwdm(unsigned long PAddr, unsigned long SAddr, unsigned long WData)
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;

  *fastPrimDsrM = PAddr; /* primary address cycle   */
  *fastSecadW   = SAddr; /* secondary address cycle */ 
  *fastRndmWDis = WData;

  /* wait for sequencer done */
  do 
  {
    EIEIO;
    SYNC;

    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
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



unsigned long 
fprdm(unsigned long PAddr, unsigned long SAddr, unsigned long *rData)
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;

  unsigned long kk;

  *fastPrimDsrM = PAddr; /* primary address cycle   */
  *fastSecadW   = SAddr; /* secondary address cycle */ 
  *fastRndmRDis = reg32;

  /* wait for sequencer done */
  do 
  {
    EIEIO;
    SYNC;

    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
      case 0x00008001:
         /* OK */
         Return = 0;
         Exit = 1;
        /* read seq fifo flags */
        /* check if EMPTY ; if EMPTY then Dummy read and Loop until not Empty */
        /*                   else if not empty then read fifo */


	 reg32 = *sfi.readSeqFifoFlags;
	 if((reg32 & 0x00000010) == 0x00000010)  /* Empty */
	   {
	     kk=0;
	     reg32 = *sfi.readSeqFifoFlags;
	     while ( (kk<20) && ((reg32&0x00000010) == 0x00000010) ) {
	       sfiSeq2VmeFifoVal = *sfi.readSeq2VmeFifoBase; /* Dummy read */
	       reg32 = *sfi.readSeqFifoFlags;
	       kk++;
	     }
	     if(kk >= 20) /* Still Empty */
	       {
		 /* Error !!! */
		 logMsg("fb_rdb_1: Empty Fifo Status=0x%x Val=0x%x\n",reg32,sfiSeq2VmeFifoVal,0,0,0,0);
		 Return = *sfi.sequencerStatusReg & 0x0000ffff;
		 Return |= 0x00100000;
		 break;
	       }
           }
        
         /* read fifo */

         *rData = *sfi.readSeq2VmeFifoBase;
         
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

/*======================================================================*/
/*
   Funktion     : FAC

   In           : pAddr	    primary address
                  sAddr     secondary address

		  
   Out          : returns 0, if no error, else number of errors


   Bemerkungen  : 

*/
/*======================================================================*/

unsigned long 
fpac(unsigned long PAddr, unsigned long SAddr)
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;

  *fastPrimCsr   = PAddr; /* primary address cycle   */
  *fastSecadW    = SAddr; /* secondary address cycle */ 

  /* wait for sequencer done */
  do 
  {
    EIEIO;
    SYNC;

    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
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



/*======================================================================*/
/*
   Funktion     : FAD

   In           : pAddr		primary address
                  sAddr     secondary address
		  
   Out          : returns 0, if no error, else number of errors

   Bemerkungen  : 

*/
/*======================================================================*/

unsigned long 
fpad(unsigned long PAddr, unsigned long SAddr)
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;

  *fastPrimDsr   = PAddr; /* primary address cycle   */
  *fastSecadW    = SAddr; /* secondary address cycle */ 

  /* wait for sequencer done */
  do 
  {
    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
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


/*======================================================================*/
/*
   Funktion     : FSAR, FSAW   secondary address read/write

   In           : SAddr		secondary address (for write)
		  
   Out          : returns 0, if no error, else seq status register

   Bemerkungen  : 

*/
/*======================================================================*/

unsigned long 
fpsar()
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;

  *fastSecadR = reg32;

  /* wait for sequencer done */
  do 
  {
    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
      case 0x00008001:
         /* OK */
         Return = 0;
         Exit = 1;
        /* read seq fifo flags */
        /* check if EMPTY ; if EMPTY then Dummy read and Loop until not Empty */
        /*                   else if not empty then read fifo */


	reg32 = *sfi.readSeqFifoFlags;

	
        if((reg32 & 0x00000010) == 0x00000010)
	   {
	   reg32 = *sfi.readSeq2VmeFifoBase; /* dummy read */
	   reg32 = *sfi.readSeqFifoFlags;
           if((reg32 & 0x00000010) == 0x00000010) 
              {
	      /* Fehler !!! */
              Return = *sfi.sequencerStatusReg & 0x0000ffff;
              Return |= 0x00100000;
              }
           }
        
         /* read fifo */

         Return = *sfi.readSeq2VmeFifoBase;
         
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


/*************/

unsigned long 
fpsaw(unsigned long SAddr)
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;

  *fastSecadW = SAddr; /* secondary address cycle */ 

  /* wait for sequencer done */
  do 
  {
    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
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


/*======================================================================*/
/*
   Funktion     : FPR FPW     (Fastbus single read/write from attached slave)

   In           : NONE
		  
   Out          : returns result of read else the error

   Bemerkungen  : 

*/
/*======================================================================*/

unsigned long 
fpr()
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;

  *fastRndmR = reg32;

/* wait for sequencer done */
  do 
  {
    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
      case 0x00008001:
         /* OK */
         Return = 0;
         Exit = 1;
        /* read seq fifo flags */
        /* check if EMPTY ; if EMPTY then Dummy read and Loop until not Empty */
        /*                   else if not empty then read fifo */


	reg32 = *sfi.readSeqFifoFlags;

	
        if((reg32 & 0x00000010) == 0x00000010)
	   {
	   reg32 = *sfi.readSeq2VmeFifoBase; /* dummy read */
	   reg32 = *sfi.readSeqFifoFlags;
           if((reg32 & 0x00000010) == 0x00000010) 
              {
	      /* Fehler !!! */
              Return = *sfi.sequencerStatusReg & 0x0000ffff;
              Return |= 0x00100000;
              }
           }
        
         /* read fifo */

         Return = *sfi.readSeq2VmeFifoBase;
         
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

unsigned long 
fpw(unsigned long WData)
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;

  *fastRndmW = WData;

  /* wait for sequencer done */
  do 
  {
    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
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



/*======================================================================*/
/*
   Funktion     : frdb      (Fastbus block read routine)

   In           : See discription below
		  
   Out          : returns 0, if no error, else number of errors

   Bemerkungen  : 

*/
/*======================================================================*/

unsigned long 
fprdb(PAddr,SAddr,Buffer,next_buffer,Max_ExpLWord,cnt_RecLWord,Mode,Wait)

unsigned long PAddr;           /* Primary Address */
unsigned long SAddr;	       /* Secondary Address */
unsigned long Buffer;          /* Address of LWord-Buffer (VME-Slave Address!!! )*/
unsigned long *next_buffer;    /* Address of LWord-Buffer after READ */
unsigned long Max_ExpLWord;    /* Max. Count of 32Bit Datawords  */
unsigned long *cnt_RecLWord;   /* Count of Received 32Bit Datawords  */ 
unsigned long Mode;            /* Readout-Modus (Direct and/or VME) */
			       /* 0x10 only Direct            */
			       /* 0x09 VMED32 Data cycle      */
			       /* 0x0A VMED32 Blocktransfer   */
			       /* 0x19 Direct and VME32Datacycl. */
			       /* 0x1A Direct and VME32Blocktr.  */
unsigned long Wait;            /* Count for CPU idle loop before
                                  polling the Sequencer status */
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;
  unsigned long ii,kk;

  Max_ExpLWord &= 0x00ffffff;
  Max_ExpLWord |= (Mode << 24);

  *fastLoadDmaAddressPointer = Buffer;
  *fastPrimDsr               = PAddr; /* primary address cycle   */
  *fastSecadW                = SAddr; /* secondary address cycle */ 
  *fastStartFrdbWithClearWc  = Max_ExpLWord; /* start dma    */
  *fastDiscon                = reg32;    /*             disconnect   */
  *fastStoreFrdbWc           = reg32;           /* get wordcount*/
  *fastStoreFrdbAp           = reg32;           /* get adr.ptr. */

  /* wait for sequencer done */
  for (ii=0;ii<Wait;ii++) kk = ii*ii;

  do 
  {
    EIEIO;
    SYNC;

    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
      case 0x00008001:   /* OK */
         Return = 0;
         Exit = 1;
        /* read seq fifo flags */
        /* check if EMPTY ; if EMPTY then Dummy read and Loop until not Empty */
        /*                   else if not empty then read fifo */

	 reg32 = *sfi.readSeqFifoFlags;
	 if((reg32 & 0x00000010) == 0x00000010)
	   {
	     reg32 = *sfi.readSeq2VmeFifoBase; /* dummy read */
	     reg32 = *sfi.readSeqFifoFlags;
	     if((reg32 & 0x00000010) == 0x00000010) 
	       {
		 /* Error !!! */
		 Return = *sfi.sequencerStatusReg & 0x0000ffff;
		 Return |= 0x00100000;
		 break;
	       }
           }
        
         /* read fifo */

         *cnt_RecLWord = *sfi.readSeq2VmeFifoBase;
         /* Check for FIFO Empty: */
	 reg32 = *sfi.readSeqFifoFlags;
         if((reg32 & 0x00000010) == 0x00000010) 
	   {
	     /* Error !!! */
	     Return = *sfi.sequencerStatusReg & 0x0000ffff;
	     Return |= 0x00040000;
	     break;
	   }
	 
         /* read fifo */

         *next_buffer  = *sfi.readSeq2VmeFifoBase;
         /* Check for FIFO Empty: */
	 reg32 = *sfi.readSeqFifoFlags;
         if((reg32 & 0x00000010) != 0x00000010) 
	   {
	     /* Error !!! */
	     Return = *sfi.sequencerStatusReg & 0x0000ffff;
	     Return |= 0x00040000;
	     break;
	   }

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



/*======================================================================*/
/*
   Funktion     : FREL

   In           : 
                 

		  
   Out          : returns 0, if no error, else number of errors


   Bemerkungen  : 

*/
/*======================================================================*/

unsigned long
fprel()
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;

  *fastDiscon   = reg32; /* Release fastbus */

  /* wait for sequencer done */
  do 
  {
    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
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


/*======================================================================*/
/*
   Function     : FPRAM

   In           : ramAddr - Local RAM address from where to start execution
                            (must be on 256 byte boundary i.e. 0x100,0x200 etc...)
		  nRet    - Number of Return data words to be read from Sequencer
                            FIFO.
                  rData   - Pointer to Array for return values to be placed.
		  
   Out          : returns 0, if no error, else error result with Sequencer Status


   Description  : Execute SFI Sequencer RAM list 

*/
/*======================================================================*/

unsigned long 
fpram(unsigned long ramAddr, int nRet, unsigned long *rData)
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;
  volatile unsigned long *vaddr;
  int kk;

  /* start Sequencer at RAM Address */
  *(fastEnableRamSequencer + (ramAddr>>2)) = 1;

  /* wait for sequencer done */
  do 
  {
    EIEIO;
    SYNC;

    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
      case 0x00008001:
         /* OK */
         Return = 0;
         Exit = 1;

	 if(nRet) {
	   /* check if EMPTY ; if EMPTY then Dummy read and Loop until not Empty */
	   /*                   else if not empty then read fifo */
	   reg32 = *sfi.readSeqFifoFlags;
	   if((reg32 & 0x00000010) == 0x00000010)  /* Empty */
	     {
	       kk=0;
	       vaddr = sfi.readSeqFifoFlags;
	       vaddr += (SFI_READ_ADDR_OFFSET>>2);
	       reg32 = *vaddr;
	       while ( (kk<50) && ((reg32&0x00000010) == 0x00000010) ) {
		 sfiSeq2VmeFifoVal = *sfi.readSeq2VmeFifoBase; /* Dummy read */
		 if(kk<10) vaddr += (SFI_READ_ADDR_OFFSET>>2);
		 reg32 =  *vaddr;
		 kk++;
	       }
	       if(kk > 1) {
		 /* logMsg("fpram: kk = %d\n",kk,0,0,0,0,0); */
		 if(kk >= 50) /* Still Empty */
		   {
		     /* Error !!! */
		     logMsg("fpram: Empty Fifo Status=0x%x Val=0x%x\n",reg32,sfiSeq2VmeFifoVal,0,0,0,0);
		     Return = *sfi.sequencerStatusReg & 0x0000ffff;
		     Return |= 0x00100000;
		     break;
		   }
	       }
	     }
	   
	   /* read fifo */
	   if(rData) {
	     for (kk=0; kk<nRet; kk++)
	       *rData++ = *sfi.readSeq2VmeFifoBase;
	   } else {
	     logMsg("fpram: Error - Null Pointer for Return Data\n",0,0,0,0,0,0);
	   }
	 }
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



unsigned long 
fpramStart(unsigned long ramAddr)
{
  /* start Sequencer at RAM Address */
  *(fastEnableRamSequencer + (ramAddr>>2)) = 1;

  return(0);
}




unsigned long 
fpramDone(int nRet, unsigned long *rData)
{
  register unsigned long reg32 = 0;
  register int Return = 0;
  register int Exit   = 0;
  volatile unsigned long *vaddr;
  int kk;

  /* wait for sequencer done */
  do 
  {
    EIEIO;
    SYNC;

    reg32 = *sfi.sequencerStatusReg;

    switch(reg32 & 0x00008001)
    {
      case 0x00008001:
         /* OK */
         Return = 0;
         Exit = 1;

	 if(nRet) {
	   /* check if EMPTY ; if EMPTY then Dummy read and Loop until not Empty */
	   /*                   else if not empty then read fifo */
	   reg32 = *sfi.readSeqFifoFlags;
	   if((reg32 & 0x00000010) == 0x00000010)  /* Empty */
	     {
	       kk=0;
	       vaddr = sfi.readSeqFifoFlags;
	       vaddr += (SFI_READ_ADDR_OFFSET>>2);
	       reg32 = *vaddr;
	       while ( (kk<50) && ((reg32&0x00000010) == 0x00000010) ) {
		 sfiSeq2VmeFifoVal = *sfi.readSeq2VmeFifoBase; /* Dummy read */
		 if(kk<10) vaddr += (SFI_READ_ADDR_OFFSET>>2);
		 reg32 =  *vaddr;
		 kk++;
	       }
	       if(kk > 1) {
		 /* logMsg("fpram: kk = %d\n",kk,0,0,0,0,0); */
		 if(kk >= 50) /* Still Empty */
		   {
		     /* Error !!! */
		     logMsg("fpram: Empty Fifo Status=0x%x Val=0x%x\n",reg32,sfiSeq2VmeFifoVal,0,0,0,0);
		     Return = *sfi.sequencerStatusReg & 0x0000ffff;
		     Return |= 0x00100000;
		     break;
		   }
	       }
	     }
	   
	   /* read fifo */
	   if(rData) {
	     for (kk=0; kk<nRet; kk++)
	       *rData++ = *sfi.readSeq2VmeFifoBase;
	   } else {
	     logMsg("fpram: Error - Null Pointer for Return Data\n",0,0,0,0,0,0);
	   }
	 }
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

#else /* no UNIX version */

void
libsfifb_dummy()
{
  return;
}

#endif
