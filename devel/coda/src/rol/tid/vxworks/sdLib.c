/*----------------------------------------------------------------------------*
 *  Copyright (c) 2010        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Authors: Bryan Moffit                                                   *
 *             moffit@jlab.org                   Jefferson Lab, MS-12B3       *
 *             Phone: (757) 269-5660             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *
 * Description:
 *     Status and Control library for the JLAB Signal Distribution
 *     (SD) module using an i2c interface from the JLAB Trigger
 *     Interface/Distribution (TID) module.
 *
 *   This file is "included" in the tidLib.c
 *
 * SVN: $Rev: 407 $
 *
 *----------------------------------------------------------------------------*/

#define DEVEL

/* This is the SD base relative to the TID base VME address */
#define SDBASE 0x40000 

/* Static variable to verify SD was initialized */
static int sdInitialized=0;

/*
  sdInit
  - Initialize the Signal Distribution Module
    Nothing here, at the moment.
*/
int
sdInit()
{
  unsigned int tidBase=0, sdBase=0;
  unsigned int version;

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  /* Do something here to verify that we've got good i2c to the SD..
     - maybe just check the status of the clk A and B */
  /* Verify that the ctp registers are in the correct space for the TID I2C */
  tidBase = (unsigned int)TIDp;
  sdBase = (unsigned int)&(TIDp->sd);
  
  if(sdBase-tidBase != SDBASE)
    {
      printf("%s: ERROR: SD memory structure not in correct VME Space!\n",
	     __FUNCTION__);
      printf("   current base = 0x%08x   expected base = 0x%08x\n",
	     sdBase-tidBase, SDBASE);
      sdInitialized=0;
      return ERROR;
    }
 
  TIDLOCK;
  version = tidRead(&TIDp->sd.version)&0xffff;
  TIDUNLOCK;

  printf("%s: SD (version 0x%x) initialized at Local Base address 0x%08x\n",
	 __FUNCTION__,version,sdBase);

  sdInitialized=1;

  return OK;
}

/*
  sdStatus
  - Display status of SD registers

*/
int
sdStatus()
{
  unsigned int system, status, payloadSlots, tokenSlots, 
    busyoutSlots, trigoutSlots,
    busyoutStatus, trigoutStatus;

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(sdInitialized!=1)
    {
      printf("%s: ERROR: SD not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  system        = tidRead(&TIDp->sd.system);
  status       = tidRead(&TIDp->sd.status);
  payloadSlots  = tidRead(&TIDp->sd.payloadSlots);
  tokenSlots    = tidRead(&TIDp->sd.tokenSlots);
  busyoutSlots  = tidRead(&TIDp->sd.busyoutSlots);
  trigoutSlots  = tidRead(&TIDp->sd.trigoutSlots);
#ifdef OLDMAP
  status2       = tidRead(&TIDp->sd.status2);
#endif
  busyoutStatus = tidRead(&TIDp->sd.busyoutStatus);
  trigoutStatus = tidRead(&TIDp->sd.trigoutStatus);
  TIDUNLOCK;
  
  printf("*** Signal Distribution Module Status ***\n");
  printf("  Status Register = 0x%04x\n",status);
  printf("  System Register = 0x%04x\n",system);
#ifdef OLDMAP
  printf("  Clock A:  ");
  switch( (status&SD_STATUS_CLKA_FREQUENCY_MASK)>>2 )
    {
    case 0:
      printf("  31.25 MHz  ");
      break;
    case 1:
      printf("  62.50 MHz  ");
      break;
    case 2:
      printf(" 125.00 MHz  ");
      break;
    case 3:
      printf(" 250.00 MHz  ");
      break;
    }
  switch( (status&SD_STATUS_CLKA_BYPASS_MODE) )
    {
    case 0:
      printf("  Attenuated Mode  ");
      break;
    case 1:
      printf("  Bypass Mode  ");
      break;
    }
  printf("\n");

  printf("  Clock B:  ");
  switch( (status&SD_STATUS_CLKB_FREQUENCY_MASK)>>6 )
    {
    case 0:
      printf("  31.25 MHz  ");
      break;
    case 1:
      printf("  62.50 MHz  ");
      break;
    case 2:
      printf(" 125.00 MHz  ");
      break;
    case 3:
      printf(" 250.00 MHz  ");
      break;
    }
  switch( (status&SD_STATUS_CLKB_BYPASS_MODE) )
    {
    case 0:
      printf("  Attenuated Mode  ");
      break;
    case 1:
      printf("  Bypass Mode  ");
      break;
    }
  printf("\n");
#endif

  printf("  Payload Boards Mask = 0x%08x   Token Passing Boards Mask = 0x%08x\n",
	 payloadSlots,tokenSlots);
  printf("  BusyOut Boards Mask = 0x%08x   TrigOut Boards Mask       = 0x%08x\n",
	 busyoutSlots,trigoutSlots);

#ifdef OLDMAP
  if( status2 & SD_STATUS2_POWER_FAULT ) 
    {
      printf("  *** Power Fault Detected ***\n");
    }
  if( status2 & SD_STATUS2_TRIGOUT ) 
    {
      printf("  *** At least one board has asserted TrigOut since last read ***\n");
    }
  if( status2 & SD_STATUS2_BUSYOUT ) 
    {
      printf("  *** At least one board has asserted Busy since last read ***\n");
    }

  if ( status2 & SD_STATUS2_CLKA_LOSS_OF_LOCK )
    {
      printf("  *** Loss of Lock detected for Clock A PLL *** \n");
    }
  if ( status2 & SD_STATUS2_CLKA_LOSS_OF_SIGNAL )
    {
      printf("  *** Loss of Signal detected for Clock A PLL *** \n");
    }
  if ( status2 & SD_STATUS2_CLKB_LOSS_OF_LOCK )
    {
      printf("  *** Loss of Lock detected for Clock B PLL *** \n");
    }
  if ( status2 & SD_STATUS2_CLKB_LOSS_OF_SIGNAL )
    {
      printf("  *** Loss of Signal detected for Clock B PLL *** \n");
    }

  if(status2 & SD_STATUS2_LAST_TOKEN_ADDR_MASK) 
    {
      printf("  Token last received at payload slot %d\n", 
	     (status2 & SD_STATUS2_LAST_TOKEN_ADDR_MASK)>>8 );
    }
#endif

  printf("  Busyout State Mask  = 0x%08x   TrigOut State Mask        = 0x%08x\n",
	 busyoutStatus,trigoutStatus);

  return OK;
}

/*
  sdSetClockFrequency
  - Set the Clock Frequency of A/B/Both
    iclk  : 0 for A
            1 for B
	    2 for Both
    ifreq : 0 for 31.25 MHz
            1 for 62.50 MHz
	    2 for 125.00 MHz
	    3 for 250.00 MHz

*/

int
sdSetClockFrequency(int iclk, int ifreq)
{
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(sdInitialized!=1)
    {
      printf("%s: ERROR: SD not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(iclk<0 || iclk>2)
    {
      printf("%s: ERROR: Invalid value of iclk (%d).  Must be 0, 1, or 2.\n",
	     __FUNCTION__,iclk);
      return ERROR;
    }
  if(ifreq<0 || ifreq>3)
    {
      printf("%s: ERROR: Invalid value of ifreq (%d).  Must be 0, 1, 2, or 3.\n",
	     __FUNCTION__,ifreq);
      return ERROR;
    }

  TIDLOCK;
  if(iclk==0 || iclk==2)
    tidWrite(&TIDp->sd.status,
	     (tidRead(&TIDp->sd.status) & ~(SD_STATUS_CLKA_FREQUENCY_MASK)) |
	     (ifreq<<2) );

  if(iclk==1 || iclk==2)
    tidWrite(&TIDp->sd.status,
	     (tidRead(&TIDp->sd.status) & ~(SD_STATUS_CLKB_FREQUENCY_MASK)) |
	     (ifreq<<6) );
  TIDUNLOCK;

  return OK;
}

/*
  sdGetClockFrequency
  - Return the clock frequency for the selected iclk
    iclk  : 0 for A
            1 for B

    returns : 0 for 31.25 MHz
              1 for 62.50 MHz
	      2 for 125.00 MHz
	      3 for 250.00 MHz

*/
int
sdGetClockFrequency(int iclk)
{
  int rval;
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(sdInitialized!=1)
    {
      printf("%s: ERROR: SD not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(iclk<0 || iclk>1)
    {
      printf("%s: ERROR: Invalid value of iclk (%d).  Must be 0 or 1.\n",
	     __FUNCTION__,iclk);
      return ERROR;
    }

  TIDLOCK;
  if(iclk==0)
    rval = (tidRead(&TIDp->sd.status) & (SD_STATUS_CLKA_FREQUENCY_MASK))>>2;
  else
    rval = (tidRead(&TIDp->sd.status) & (SD_STATUS_CLKB_FREQUENCY_MASK))>>6;
  TIDUNLOCK;

  return rval;
}

/*
  sdSetClockMode
  - Select whether the Clock fanned out will be jitter attenuated or
    as received from the TI(D)
    iclk  : 0 for A
            1 for B

    imode : 0 for Attentuated mode
            1 for Bypass mode    
*/
int
sdSetClockMode(int iclk, int imode)
{
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(sdInitialized!=1)
    {
      printf("%s: ERROR: SD not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(iclk<0 || iclk>1)
    {
      printf("%s: ERROR: Invalid value of iclk (%d).  Must be 0 or 1.\n",
	     __FUNCTION__,iclk);
      return ERROR;
    }
  if(imode<0 || imode>1)
    {
      printf("%s: ERROR: Invalid value of imode (%d).  Must be 0 or 1.\n",
	     __FUNCTION__,imode);
      return ERROR;
    }

  TIDLOCK;
  if(iclk==0)
    tidWrite(&TIDp->sd.status,
	     (tidRead(&TIDp->sd.status) & ~(SD_STATUS_CLKA_BYPASS_MODE)) |
	     (imode<<0) );
  else
    tidWrite(&TIDp->sd.status,
	     (tidRead(&TIDp->sd.status) & ~(SD_STATUS_CLKB_BYPASS_MODE)) |
	     (imode<<4) );
  TIDUNLOCK;

  return OK;
}

/*
  sdGetClockMode
  - Return whether the Clock fanned out will be jitter attenuated or
    as received from the TI(D)
    iclk  : 0 for A
            1 for B

    return : 0 for Attentuated mode
             1 for Bypass mode    
*/
int
sdGetClockMode(int iclk)
{
  int rval;
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(sdInitialized!=1)
    {
      printf("%s: ERROR: SD not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(iclk<0 || iclk>1)
    {
      printf("%s: ERROR: Invalid value of iclk (%d).  Must be 0 or 1.\n",
	     __FUNCTION__,iclk);
      return ERROR;
    }

  TIDLOCK;
  if(iclk==0)
    rval = (tidRead(&TIDp->sd.status) & (SD_STATUS_CLKA_BYPASS_MODE));
  else
    rval = (tidRead(&TIDp->sd.status) & (SD_STATUS_CLKB_BYPASS_MODE))>>4;
  TIDUNLOCK;

  return rval;
}

/*
  sdResetPLL
  - Reset the PLL for a selected clock
*/
int
sdResetPLL(int iclk)
{
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(sdInitialized!=1)
    {
      printf("%s: ERROR: SD not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(iclk<0 || iclk>1)
    {
      printf("%s: ERROR: Invalid value of iclk (%d).  Must be 0 or 1.\n",
	     __FUNCTION__,iclk);
      return ERROR;
    }

  TIDLOCK;
  if(iclk==0)
    tidWrite(&TIDp->sd.status,
	     (tidRead(&TIDp->sd.status) & ~(SD_STATUS_CLKA_RESET)) |
	     (SD_STATUS_CLKA_RESET) );
  else
    tidWrite(&TIDp->sd.status,
	     (tidRead(&TIDp->sd.status) & ~(SD_STATUS_CLKB_RESET)) |
	     (SD_STATUS_CLKB_RESET) );
  TIDUNLOCK;

  return OK;
}

/*
  sdReset
  - Reset the SD (System Reset)
*/
int
sdReset()
{
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(sdInitialized!=1)
    {
      printf("%s: ERROR: SD not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  tidWrite(&TIDp->sd.status,SD_STATUS_RESET);
  TIDUNLOCK;

  return OK;
}

/*
  sdSetActivePayloadSlots
  - Routine for user to define the Payload Slots that participate in 
  Trigger Out, Busy Out, Token, and Status communication.

*/
int
sdSetActivePayloadSlots(unsigned int imask)
{
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(sdInitialized!=1)
    {
      printf("%s: ERROR: SD not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if(imask<0 || imask>0xffff)
    {
      printf("%s: ERROR: Invalid imask 0x%x\n",
	     __FUNCTION__,imask);
      return ERROR;
    }

  printf("imask = 0x%08x\n",imask);

  printf("payloadSlots addr = 0x%08x\n",
	 (unsigned int)&TIDp->sd.payloadSlots-(unsigned int)TIDp);

  TIDLOCK;
  tidWrite(&TIDp->sd.payloadSlots, imask);
  tidWrite(&TIDp->sd.tokenSlots, imask);
  tidWrite(&TIDp->sd.busyoutSlots, imask);
  tidWrite(&TIDp->sd.trigoutSlots, imask);
  TIDUNLOCK;

  return OK;
}

/*
  sdGetActivePayloadSlots
  - Routine to return the currently defined Payload Slots that participate in 
    Trigger Out, Busy Out, Token, and Status communication.

*/
int
sdGetActivePayloadSlots()
{
  int rval;
  unsigned int payloadSlots, tokenSlots, busyoutSlots, trigoutSlots;
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(sdInitialized!=1)
    {
      printf("%s: ERROR: SD not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  payloadSlots = tidRead(&TIDp->sd.payloadSlots);
  tokenSlots = tidRead(&TIDp->sd.tokenSlots);
  busyoutSlots = tidRead(&TIDp->sd.busyoutSlots);
  trigoutSlots = tidRead(&TIDp->sd.trigoutSlots);
  TIDUNLOCK;

  rval = payloadSlots;

  /* Simple check for consistency, warn if there's not */
  if((payloadSlots != tokenSlots) ||
     (payloadSlots != busyoutSlots) ||
     (payloadSlots != trigoutSlots) )
    {
      printf("%s: WARNING: Inconsistent payload slot masks..",__FUNCTION__);
      printf("    payloadSlots = 0x%08x\n",payloadSlots);
      printf("    tokenSlots   = 0x%08x\n",tokenSlots);
      printf("    busyoutSlots = 0x%08x\n",busyoutSlots);
      printf("    trigoutSlots = 0x%08x\n",trigoutSlots);
    }
  
  return rval;
}

/*
  sdGetBusyoutCounter
  - Return the value of the Busyout Counter for a specified payload board
    Value of the counter is reset after read

*/

int
sdGetBusyoutCounter(int ipayload)
{
  int rval;
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(sdInitialized!=1)
    {
      printf("%s: ERROR: SD not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if(ipayload<1 || ipayload>16)
    {
      printf("%s: ERROR: Invalid ipayload = %d.  Must be 1-16\n",
	     __FUNCTION__,ipayload);
      return ERROR;
    }

  TIDLOCK;
  rval = tidRead(&TIDp->sd.busyoutCounter[ipayload-1]);
  TIDUNLOCK;

  return rval;

}

