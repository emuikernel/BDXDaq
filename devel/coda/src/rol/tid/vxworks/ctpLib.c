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
 *     Status and Control library for the JLAB Crate Trigger Processor
 *     (CTP) module using an i2c interface from the JLAB Trigger
 *     Interface/Distribution (TID) module.
 *
 *   This file is "included" in the tidLib.c
 *
 * SVN: $Rev: 406 $
 *
 *----------------------------------------------------------------------------*/

#define DEBUG

/* This is the CTP base relative to the TID base VME address */
#define CTPBASE 0x30000 

/* Static variable to verify CTP was initialized */
static int ctpInitialized=0;

/* FPGA Channel number to Payload Port Map */
#define NUM_CTP_FPGA 3
#define NUM_FADC_CHANNELS 6 /* 5 for VLX50, 6 for VLX110 */
unsigned int ctpPayloadPort[NUM_CTP_FPGA][NUM_FADC_CHANNELS] =
  {
    /* VLX50  at i2c Board Addres 0 */
    {  7,  9, 11, 13, 15,  0},  
    /* VLX50  at i2c Board Addres 1 */
    {  8, 10, 12, 14, 16,  0},
    /* VLX110 at i2c Board Addres 2 */
    {  1,  2,  3,  4,  5,  6}
  };


/*
  ctpInit
  - Initialize the Crate Trigger Processor

*/
int
ctpInit()
{
  unsigned int tidBase=0, ctpBase=0;
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }

  /* Do something here to verify that we've got good i2c to the CTP */
  /* Verify that the ctp registers are in the correct space for the TID I2C */
  tidBase = (unsigned int)TIDp;
  ctpBase = (unsigned int)&(TIDp->ctp);
  
  if(ctpBase-tidBase != CTPBASE)
    {
      printf("%s: ERROR: CTP memory structure not in correct VME Space!\n",
	     __FUNCTION__);
      printf("   current base = 0x%08x   expected base = 0x%08x\n",
	     ctpBase-tidBase, CTPBASE);
      ctpInitialized=0;
      return ERROR;
    }
  
  printf("%s: CTP initialized at Local Base address 0x%08x\n",
	 __FUNCTION__,ctpBase);

  ctpInitialized=1;
  
  return OK;

}


/*
  ctpStatus
  - Display the status of the CTP registers 
  FIXME: Check see this works.
*/
int
ctpStatus()
{
  struct CTP_FPGAStruct fpga[3];
  int ifpga, ichan, ipport;
  int lane_up[16+1];    /* Stored payload port that has it's "lane up" */
  int channel_up[16+1]; /* Stored payload port that has it's "channel up" */
  int firmware_version[3];
  unsigned int threshold_lsb, threshold_msb;

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(ctpInitialized!=1)
    {
      printf("%s: ERROR: CTP not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  fpga[0].status0 = tidRead(&TIDp->ctp.fpga1.status0);
  fpga[0].status1 = tidRead(&TIDp->ctp.fpga1.status1);
  fpga[1].status0 = tidRead(&TIDp->ctp.fpga2.status0);
  fpga[1].status1 = tidRead(&TIDp->ctp.fpga2.status1);
  fpga[2].status0 = tidRead(&TIDp->ctp.fpga3.status0);
  fpga[2].status1 = tidRead(&TIDp->ctp.fpga3.status1);

  fpga[0].temp    = tidRead(&TIDp->ctp.fpga1.temp);
  fpga[1].temp    = tidRead(&TIDp->ctp.fpga2.temp);
  fpga[2].temp    = tidRead(&TIDp->ctp.fpga3.temp);

  fpga[0].vint    = tidRead(&TIDp->ctp.fpga1.vint);
  fpga[1].vint    = tidRead(&TIDp->ctp.fpga2.vint);
  fpga[2].vint    = tidRead(&TIDp->ctp.fpga3.vint);

  fpga[0].config0 = tidRead(&TIDp->ctp.fpga1.config0);
  fpga[1].config0 = tidRead(&TIDp->ctp.fpga2.config0);
  fpga[2].config0 = tidRead(&TIDp->ctp.fpga3.config0);

  fpga[0].enable[0] = tidRead(&TIDp->ctp.fpga1.enable[0]);
  fpga[1].enable[0] = tidRead(&TIDp->ctp.fpga2.enable[0]);
  fpga[2].enable[0] = tidRead(&TIDp->ctp.fpga3.enable[0]);

  fpga[0].enable[1] = tidRead(&TIDp->ctp.fpga1.enable[1]);
  fpga[1].enable[1] = tidRead(&TIDp->ctp.fpga2.enable[1]);
  fpga[2].enable[1] = tidRead(&TIDp->ctp.fpga3.enable[1]);

  threshold_lsb = tidRead(&TIDp->ctp.sum_threshold_lsb);
  threshold_msb = tidRead(&TIDp->ctp.sum_threshold_msb);
  TIDUNLOCK;

  /* Loop over FPGAs and Channels to get the detailed status info.
     This is quite tedious... but so is the map of registers to
     channels to payload slots.
   */
  for(ifpga=0;ifpga<NUM_CTP_FPGA; ifpga++)
    {
      for(ichan=0;ichan<NUM_FADC_CHANNELS;ichan++)
	{
	  if(ichan==5 && ifpga<2) 
	    continue; /* Skip channel 5 for the VLX50s */

	  /* Determine if Lane's are up for each Payload slot */
	  if(fpga[ifpga].status0 & (CTP_FPGA_STATUS0_LANE_UP_MASK<<(ichan*2)) )
	    lane_up[ctpPayloadPort[ifpga][ichan]]=1;
	  else
	    lane_up[ctpPayloadPort[ifpga][ichan]]=0;

	  /* Determine of Channel is up for each payload slot */
	  if(ichan<4)
	    {
	      if(fpga[ifpga].status0 & (1<<(12+ichan)) )
		channel_up[ctpPayloadPort[ifpga][ichan]]=1;
	      else
		channel_up[ctpPayloadPort[ifpga][ichan]]=0;
	    }
	  else /* ichan>4 */
	    {
	      if(fpga[ifpga].status1 & (1<<(ichan)) )
		channel_up[ctpPayloadPort[ifpga][ichan]]=1;
	      else
		channel_up[ctpPayloadPort[ifpga][ichan]]=0;
	    }
	}

      /* Get the firmware version */
      firmware_version[ifpga] = 
	(fpga[ifpga].status1 & CTP_FPGA_STATUS1_FIRMWARE_VERSION_MASK) >> 8;
    }
  
  /* Now printout what we've got */
  printf("*** Crate Trigger Processor Module Status ***\n");

  printf("  FPGA firmware versions:\n");
  for(ifpga=0;ifpga<NUM_CTP_FPGA;ifpga++)
    {
      printf("  %d: 0x%x\n",ifpga+1,firmware_version[ifpga]);
    }

  printf("   Raw Regs:\n");
  for(ifpga=0;ifpga<NUM_CTP_FPGA;ifpga++)
    {
      printf("  %d: status0 0x%04x    status1 0x%04x\n",
	     ifpga+1,fpga[ifpga].status0,fpga[ifpga].status1);
      printf("  %d: temp    0x%04x    vint    0x%04x\n",
	     ifpga+1,fpga[ifpga].temp,fpga[ifpga].vint);
      printf("  %d: enable0 0x%04x    enable1 0x%04x\n",
	     ifpga+1,fpga[ifpga].enable[0],fpga[ifpga].enable[1]);
      printf("  %d: config0 0x%04x\n",
	     ifpga+1,fpga[ifpga].config0);
      if(ifpga+1==NUM_CTP_FPGA)
	{
	  printf("  %d: thr_lsb 0x%04x    thr_msb 0x%04x\n",
		 ifpga+1,threshold_lsb,threshold_msb);
	}
      printf("\n");
    }

  printf("  Payload port lanes up: ");
  for(ipport=1; ipport<17; ipport++)
    {
      if(lane_up[ipport])
	printf("%2d ",ipport);
    }
  printf("\n");
  printf("  Payload port lanes down: ");
  for(ipport=1; ipport<17; ipport++)
    {
      if(!lane_up[ipport])
	printf("%2d ",ipport);
    }

  printf("\n");
  printf("  Payload port channels up: ");
  for(ipport=1; ipport<17; ipport++)
    {
      if(channel_up[ipport])
	printf("%2d ",ipport);
    }
  printf("\n");
  printf("  Payload port channels down: ");
  for(ipport=1; ipport<17; ipport++)
    {
      if(!channel_up[ipport])
	printf("%2d ",ipport);
    }
  printf("\n");

  /* new enable regs */
  printf("  Enable regs:\n");
  for(ifpga=0;ifpga<NUM_CTP_FPGA;ifpga++)
    {
      printf("     %d: 0x%04x    0x%04x\n",
	     ifpga+1,fpga[ifpga].enable[0],fpga[ifpga].enable[1]);
    }

  printf("\n\n");

  printf("  Threshold lsb = %d (0x%04x)\n",threshold_lsb,threshold_lsb);
  printf("  Threshold msb = %d (0x%04x)\n",threshold_msb,threshold_msb);

  return OK;
}

/*
  ctpSetFinalSumThreshold
  - Set the threshold for the Final Sum
  - Arm the trigger, if specified - FIXME: Not sure if this is needed

*/
int
ctpSetFinalSumThreshold(unsigned int threshold, int arm)
{
  unsigned int threshold_lsb, threshold_msb;
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(ctpInitialized!=1)
    {
      printf("%s: ERROR: CTP not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(arm < 0 || arm > 1)
    {
      printf("%s: Invalid value for arm (%d).  Must be 0 or 1.\n",
	     __FUNCTION__,arm);
      return ERROR;
    }


  threshold_lsb = threshold&0xffff;
  threshold_msb = threshold>>16;

#ifdef CTP_DEBUG
  printf("%s: To write...\n",__FUNCTION__);
  printf("%s:   Threshold lsb = %d (0x%04x)\n",__FUNCTION__,
	 threshold_lsb,threshold_lsb);
  printf("%s:   Threshold msb = %d (0x%04x)\n",__FUNCTION__,
	 threshold_msb,threshold_msb);
  printf("%s:   Threshold     = %d (0x%04x)\n",__FUNCTION__,
	 (threshold_msb<<16)|(threshold_lsb),
	 (threshold_msb<<16)|(threshold_lsb));
#endif

  TIDLOCK;
  tidWrite(&TIDp->ctp.sum_threshold_lsb, threshold_lsb);
  tidWrite(&TIDp->ctp.sum_threshold_msb, threshold_msb);

  /* Ben did this in his code... to Arm the trigger?  Maybe this isn't needed */
/*   if(arm==1) */
/*     { */
/*       tidWrite(&TIDp->ctp.fpga3.config0,1); */
/*       tidWrite(&TIDp->ctp.fpga3.config0,0); */
/*     } */

  threshold_lsb = tidRead(&TIDp->ctp.sum_threshold_lsb);
  threshold_msb = tidRead(&TIDp->ctp.sum_threshold_msb);

#ifdef CTP_DEBUG
  printf("%s: Readback, after write...\n",__FUNCTION__);
  printf("%s:   Threshold lsb = %d (0x%04x)\n",__FUNCTION__,
	 threshold_lsb,threshold_lsb);
  printf("%s:   Threshold msb = %d (0x%04x)\n",__FUNCTION__,
	 threshold_msb,threshold_msb);
  printf("%s:   Threshold     = %d (0x%04x)\n",__FUNCTION__,
	 (threshold_msb<<16)|(threshold_lsb),
	 (threshold_msb<<16)|(threshold_lsb));
#endif
  TIDUNLOCK;

  return OK;
}

/*
  ctpGetFinalSumThreshold
  - Return the value set for the Final Sum threshold

*/
int
ctpGetFinalSumThreshold()
{
  unsigned int rval;
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(ctpInitialized!=1)
    {
      printf("%s: ERROR: CTP not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  rval = tidRead(&TIDp->ctp.sum_threshold_lsb);
  rval |= (tidRead(&TIDp->ctp.sum_threshold_msb)<<16);
  TIDUNLOCK;

  return rval;
}

/*
  ctpSetPayloadEnableMask
  - Set the payload ports from the input MASK to be enabled.
  RETURNS: OK if successful, otherwise ERROR.
  - Mask Convention: 
    bit 0: Port 1
    bit 1: Port 2
    ...
    bit 5: Port 6
    .etc.

    FIXME: This may not work anymore - 19May2011

*/
int
ctpSetPayloadEnableMask(int enableMask)
{
  int ifpga, ichan;
  unsigned int chipMask=0;
  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(ctpInitialized!=1)
    {
      printf("%s: ERROR: CTP not initialized\n",__FUNCTION__);
      return ERROR;
    }

  if( enableMask >= (1<<(17-1)) ) /* 16 = Maximum Payload port number */
    {
      printf("%s: ERROR: Invalid enableMask (0x%x).  Includes payload port > 16.\n",
	     __FUNCTION__,enableMask);
      return ERROR;
    }

#ifdef OLDWAY  
  /* Loop over the map for each fpga (and channel) to set the bit mask
     to send to the register */
  for(ifpga=0; ifpga<NUM_CTP_FPGA; ifpga++)
    {
      chipMask=0;
      for(ichan=0; ichan<NUM_FADC_CHANNELS; ichan++)
	{
	  if( ctpPayloadPort[ifpga][ichan] != 0)
	    {
	      /* Translate the payload port number to a bit and check
		 it against the mask. */
	      if( (1<<(ctpPayloadPort[ifpga][ichan] -1 )) & enableMask)
		{
#ifdef DEBUG
		  printf("  Found... cPP[%d][%d] = %2d  logic = 0x%.4x \n",ifpga,ichan,
			 ctpPayloadPort[ifpga][ichan], (1<<(ctpPayloadPort[ifpga][ichan] -1)));
#endif /* DEBUG */
		  chipMask |= 1<<ichan;
		}
	    }
	}
      /* Configuration 0 register, according to the chipMask */
#ifdef DEBUG
      printf("%s: Chipmask for FPGA %d = 0x%2x\n",
	     __FUNCTION__, ifpga, chipMask);
#endif /* DEBUG */      
      if( (ifpga<2 && (chipMask > 0x1F)) || (ifpga==2 && (chipMask > 0x3F)) )
	{
	  printf("%s: Library ERROR: Invalid Chipmask (0x%x) for FPGA%d.  Check Payload Port map!\n",
		 __FUNCTION__,chipMask,ifpga+1);
	  return ERROR;
	}
#ifndef DEBUG
      TIDLOCK;
      switch (ifpga)
	{
	  case 0:
	    tidWrite(&TIDp->ctp.fpga1.config0,chipMask);
	    break;
	  case 1: 
	    tidWrite(&TIDp->ctp.fpga2.config0,chipMask);
	    break;
	  case 2: 
	    tidWrite(&TIDp->ctp.fpga3.config0,chipMask);
	    break;
	}
      TIDUNLOCK;
#endif /* DEBUG */      
    }
#else /* OLDWAY */
  TIDLOCK;
  chipMask = enableMask;
  printf("%s: Writing 0x%08x to all chips \n",__FUNCTION__,chipMask);
  tidWrite(&TIDp->ctp.fpga1.config0,chipMask);
  tidWrite(&TIDp->ctp.fpga2.config0,chipMask);
  tidWrite(&TIDp->ctp.fpga3.config0,chipMask);
  TIDUNLOCK;
#endif /* OLDWAY */



  return OK;
}

/*
  ctpEnableSlotMask
  - Enable these slots in the mask (defined by VME slots = bit+1)
    in the sum for the trigger

*/

int
ctpEnableSlotMask(unsigned int inMask)
{
  unsigned int maxMask = ((1<<21)-1);
  unsigned int readBack[3][2];
  unsigned int ifpga;

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(ctpInitialized!=1)
    {
      printf("%s: ERROR: CTP not initialized\n",__FUNCTION__);
      return ERROR;
    }

  /* Make sure the mask doesn't include slots > 21 */
  if( inMask > maxMask )
    {
      printf("%s: ERROR: Invalid Mask (0x%08x).  Must be less than 0x%08x\n",
	     __FUNCTION__,
	     inMask,
	     maxMask);
      return ERROR;
    }

  printf("%s: LSB Mask = 0x%04x     MSB mask = 0x%04x\n",
	 __FUNCTION__,
	 inMask&0xffff,
	 inMask>>16);

  TIDLOCK;
  tidWrite(&TIDp->ctp.fpga3.enable[0], inMask&0xffff);
  tidWrite(&TIDp->ctp.fpga1.enable[0], inMask&0xffff);
  tidWrite(&TIDp->ctp.fpga2.enable[0], inMask&0xffff);

  tidWrite(&TIDp->ctp.fpga3.enable[1], inMask>>16);
  tidWrite(&TIDp->ctp.fpga1.enable[1], inMask>>16);
  tidWrite(&TIDp->ctp.fpga2.enable[1], inMask>>16);

  readBack[0][0] = tidRead(&TIDp->ctp.fpga1.enable[0]);
  readBack[1][0] = tidRead(&TIDp->ctp.fpga2.enable[0]);
  readBack[2][0] = tidRead(&TIDp->ctp.fpga3.enable[0]);

  readBack[0][1] = tidRead(&TIDp->ctp.fpga1.enable[1]);
  readBack[1][1] = tidRead(&TIDp->ctp.fpga2.enable[1]);
  readBack[2][1] = tidRead(&TIDp->ctp.fpga3.enable[1]);

  TIDUNLOCK;

  printf("%s: Readback... \n",__FUNCTION__);
  for(ifpga=0; ifpga<3; ifpga++)
    {
      printf("   %d:  0x%04x   0x%04x\n",
	     ifpga+1, readBack[ifpga][0], readBack[ifpga][1]);
    }
  

  return OK;
}

/*
  ctpResetGTP
  - Reset All GTP between FADCs and FPGAs.
  - FIXME: This doesn't do what it used to... kill it.
*/
int
ctpResetGTP()
{
  return ERROR; // FIXME: DON'T USE THIS ROUTINE

  if(TIDp==NULL)
    {
      printf("%s: ERROR: TID not initialized\n",__FUNCTION__);
      return ERROR;
    }
  if(ctpInitialized!=1)
    {
      printf("%s: ERROR: CTP not initialized\n",__FUNCTION__);
      return ERROR;
    }

  TIDLOCK;
  tidWrite(&TIDp->ctp.fpga3.config0, 0x0FC0);
  tidWrite(&TIDp->ctp.fpga1.config0, 0x0FC0);
  tidWrite(&TIDp->ctp.fpga2.config0, 0x0FC0);

  tidWrite(&TIDp->ctp.fpga3.config0, 0x0);
  tidWrite(&TIDp->ctp.fpga1.config0, 0x0);
  tidWrite(&TIDp->ctp.fpga2.config0, 0x0);
  TIDUNLOCK;

  return OK;
}
