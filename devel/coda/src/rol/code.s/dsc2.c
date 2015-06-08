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
 *     Driver library for TJNAF VME 16 Channel Discriminator/Scaler using
 *      a vxWorks 5.4 or later or Linux 2.6.18 or later based Single Board
 *      Computer.
 *
 *----------------------------------------------------------------------------*/

#if defined(VXWORKS) || defined(Linux_vme)

#ifdef VXWORKS
#include <vxWorks.h>
#include <vxLib.h>
#include <logLib.h>
#else
#include "jvme.h"
#endif

#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "dsc2.h"

#ifdef VXWORKS
#define EIEIO    __asm__ volatile ("eieio")
#define SYNC     __asm__ volatile ("sync")
#else
#define EIEIO    
#define SYNC     
#endif

/* Global variables */
int Ndsc = 0;                         /* Number of DSCs in Crate */
volatile DSC2 *dscp[DSC_MAX_BOARDS];  /* pointers to DSC memory map */
unsigned int dscMemOffset = 0;        /* Offset between VME and Local address space */

/* Macro to check for module initialization */
#define CHECKID(id) {							\
    if((id<0) || (id>DSC_MAX_BOARDS) || (dscp[id] == NULL))		\
      {									\
	logMsg("%s: ERROR: DSC in slot %d is not initialized \n",	\
	       (int)__FUNCTION__,id,3,4,5,6);				\
	return ERROR;							\
      }									\
  }

#ifdef VXWORKS
#define DSCLOCK
#define DSCUNLOCK
#else
/* Mutex to guard flexio read/writes */
pthread_mutex_t   dscMutex = PTHREAD_MUTEX_INITIALIZER;
#define DSCLOCK     pthread_mutex_lock(&dscMutex);
#define DSCUNLOCK   pthread_mutex_unlock(&dscMutex);
#endif


/*******************************************************************
 *   Function : dsc2Status
 *                                                    
 *   Function : Print DSC module status to standard output
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT16 pflag - unused
 *                                                    
 *   Returns -1 if Error, 0 if OK.                    
 *                                                    
 *******************************************************************/

int
dsc2Status(UINT32 id, int pflag)
{
  UINT32 pulsewidth, chEnable, orMask, delay, testCtrl;
  UINT32 firmwareRev, boardID;
  CHECKID(id);

  DSCLOCK;
  pulsewidth  = vmeRead32(&dscp[id]->width);
  chEnable    = vmeRead32(&dscp[id]->chMask);
  orMask      = vmeRead32(&dscp[id]->orMask);
  delay       = vmeRead32(&dscp[id]->delay);
  testCtrl    = vmeRead32(&dscp[id]->testCtrl);
  firmwareRev = vmeRead32(&dscp[id]->firmware);
  boardID     = vmeRead32(&dscp[id]->boardID);
  DSCUNLOCK;

  printf("\nSTATUS for DSC in slot %d at base address 0x%08x (VME = 0x%08x)\n",
	 id, (UINT32) dscp[id], (UINT32) dscp[id]-dscMemOffset);
  printf("-----------------------------------------------------------------------\n");
  printf(" Board Firmware = 0x%04x  Board ID = 0x%08x\n",
	 firmwareRev&DSC_FIRMWAREREV_MASK, boardID);
  printf(" Pulse widths (0x%08x):\n",
	 pulsewidth);
  printf("   TDC: 0x%02x (%d ns)   TRG: 0x%02x (%d ns)   TRG OUTPUT: 0x%x (%d ns)\n",
	 pulsewidth&DSC_PULSEWIDTH_TDC_MASK,
	 pulsewidth&DSC_PULSEWIDTH_TDC_MASK,
	 (pulsewidth&DSC_PULSEWIDTH_TRG_MASK)>>16,
	 (pulsewidth&DSC_PULSEWIDTH_TRG_MASK)>>16,
	 (pulsewidth&DSC_PULSEWIDTH_TRG_OUTPUT_MASK)>>28,
	 4*((pulsewidth&DSC_PULSEWIDTH_TRG_OUTPUT_MASK)>>28));
  printf(" Channel Enabled Mask           = 0x%08x\n",chEnable);
  printf(" Channel Enabled OR Output Mask = 0x%08x\n",orMask);
  printf(" Delays (ns): (0x%08x)\n",delay);
  printf("   Scaler Input Delay: 0x%02x (%d ns)   TRG Output Delay: 0x%02x (%d ns)\n",
	 (delay&DSC_DELAY_SCALER_MASK), 
	 8*(delay&DSC_DELAY_SCALER_MASK), 
	 ((delay&DSC_DELAY_TRG_MASK)>>16),
	 4*((delay&DSC_DELAY_TRG_MASK)>>16));
  if(testCtrl)
    {
      printf(" Test Input Mode ENABLED\n");
    }
  printf("-----------------------------------------------------------------------\n");
  printf("\n\n");

  return OK;
}

/*******************************************************************
 *   Function : dsc2SetThreshold
 *                                                    
 *   Function : Set threshold for individual channels
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT16 chan  - Channel Number
 *                 UINT16 val   - Threshold value (-1 mV units)
 *                 UINT16 type  - 1 to set TDC threshold
 *                                2 to set TRG threshold
 *                                0 to set Both TDC & TRG
 *                                                    
 *   Returns -1 if Error, 0 if OK.                    
 *                                                    
 *******************************************************************/

int
dsc2SetThreshold(UINT32 id, UINT16 chan, UINT16 val, UINT16 type)
{
  UINT32 oldval=0;
  CHECKID(id);

  if(chan>16)
    {
      printf("%s: ERROR: invalid channel (&d)\n",__FUNCTION__,chan);
      return ERROR;
    }

  if(val>0x3ff)
    {
      printf("%s: ERROR: invalid value=%d (0x%x)\n",__FUNCTION__,val);
      return ERROR;
    }

  if(type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }


  DSCLOCK;
  switch(type)
    {
    case 1:  /* TDC */
      /* Save the TRG value so it's not lost */
      oldval = vmeRead32(&dscp[id]->threshold[chan])&DSC_THRESHOLD_TRG_MASK;
      vmeWrite32(&dscp[id]->threshold[chan],
	       ((val)&DSC_THRESHOLD_TDC_MASK) | oldval);
      break;
    case 2:  /* TRG */
      /* Save the TDC value so it's not lost */
      oldval = vmeRead32(&dscp[id]->threshold[chan])&DSC_THRESHOLD_TDC_MASK;
      vmeWrite32(&dscp[id]->threshold[chan],
	       ((val<<16)&DSC_THRESHOLD_TRG_MASK) | oldval);
      break;
    case 0:
    default: /* Both */
      vmeWrite32(&dscp[id]->threshold[chan],
	       ((val)&DSC_THRESHOLD_TDC_MASK) | ((val<<16)&DSC_THRESHOLD_TRG_MASK));
    }
  DSCUNLOCK;
  
  return OK;
}

/*******************************************************************
 *   Function : dsc2GetThreshold
 *                                                    
 *   Function : Get threshold for individual channels
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT16 chan  - Channel Number
 *                 UINT16 type  - 1 to set TDC threshold
 *                                2 to set TRG threshold
 *                                                    
 *   Returns value of threshold (-1 mV units) or Error.
 *                                                    
 *******************************************************************/

int
dsc2GetThreshold(UINT32 id, UINT16 chan, UINT16 type)
{
  int rval;
  CHECKID(id);

  if(chan>16)
    {
      printf("%s: ERROR: invalid channel (&d)\n",__FUNCTION__,chan);
      return ERROR;
    }

  if(type==0 || type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }

  DSCLOCK;
  rval = vmeRead32(&dscp[id]->threshold[chan]);
  DSCUNLOCK;
  switch(type)
    {
    case 1:  /* TDC */
      rval = rval&DSC_THRESHOLD_TDC_MASK;
      break;
    case 2:  /* TRG */
    default:
      rval = (rval&DSC_THRESHOLD_TRG_MASK)>>16;
    }
  
  return rval;
}

/*******************************************************************
 *   Function : dsc2SetPulseWidth
 *                                                    
 *   Function : Set the pulse width
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT16 val   - Pulse width 
 *                                units = 1ns for TDC and TRG
 *                                      = 4ns for TRG Output
 *                 UINT16 type  - 1 to set TDC pulse width
 *                                2 to set TRG pulse width
 *                                3 to set TRG Output pulse width
 *                                                    
 *   Returns -1 if Error, 0 if OK.                    
 *                                                    
 *******************************************************************/

int
dsc2SetPulseWidth(UINT32 id, UINT16 val, UINT16 type)
{
  UINT32 oldval=0;
  CHECKID(id);
  
  if(type==0 || type>3)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }

  if(type==1 || type==2)
    {
      if(val > 0x3f)
	{
	  printf("%s: ERROR: val=%d greater than maximum allowed (%d).\n",__FUNCTION__,val,0x3f);
	  return ERROR;
	}
    }
  else if(type==3)
    {
      if(val > 0xf)
	{
	  printf("%s: ERROR: val=%d greater than maximum allowed (%d).\n",__FUNCTION__,val,0xf);
	  return ERROR;
	}
    }

  DSCLOCK;
  switch(type)
    {
    case 1:  /* TDC */
      /* Save the TRG Pulse Width and TRG Output Pulse Width value so it's not lost */
      oldval = vmeRead32(&dscp[id]->width);
      oldval = (oldval&DSC_PULSEWIDTH_TRG_MASK) | (oldval&DSC_PULSEWIDTH_TRG_OUTPUT_MASK);
      vmeWrite32(&dscp[id]->width,
	       (val) | oldval);
      break;
    case 2:  /* TRG */
    default:
      /* Save the TDC Pulse Width and TRG Output Pulse Width value so it's not lost */
      oldval = vmeRead32(&dscp[id]->width);
      oldval = (oldval&DSC_PULSEWIDTH_TDC_MASK) | (oldval&DSC_PULSEWIDTH_TRG_OUTPUT_MASK);
      vmeWrite32(&dscp[id]->width,
	       (val<<16) | oldval);
      break;
    case 3:  /* TRGOUTPUT */
      /* Save the TDC Pulse Width and TRG Pulse Width value so it's not lost */
      oldval = vmeRead32(&dscp[id]->width);
      oldval = (oldval&DSC_PULSEWIDTH_TDC_MASK) | (oldval&DSC_PULSEWIDTH_TRG_MASK);
      vmeWrite32(&dscp[id]->width,
	       (val<<28) | oldval);
    }
  DSCUNLOCK;

  return OK;
}

/*******************************************************************
 *   Function : dsc2GetPulseWidth
 *                                                    
 *   Function : Set the pulse width
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT16 type  - 1 to set TDC pulse width
 *                                2 to set TRG pulse width
 *                                3 to set TRG Output pulse width
 *                                                    
 *   Returns -1 if Error, Value of width if OK.
 *                                                    
 *******************************************************************/

int
dsc2GetPulseWidth(UINT32 id, UINT16 type)
{
  UINT32 rval;
  CHECKID(id);

  if(type==0 || type>3)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }


  DSCLOCK;
  rval = vmeRead32(&dscp[id]->width);
  DSCUNLOCK;
  switch(type)
    {
    case 1:  /* TDC */
      rval = (rval&DSC_PULSEWIDTH_TDC_MASK);
      break;
    case 2:  /* TRG */
    default:
      rval = (rval&DSC_PULSEWIDTH_TRG_MASK)>>16;
      break;
    case 3:  /* TRG */
      rval = (rval&DSC_PULSEWIDTH_TRG_OUTPUT_MASK)>>28;
    }
  
  return rval;
}

int
dsc2SetPulseWidthAll(UINT16 tdcVal, UINT16 trgVal, UINT16 trgoutVal)
{
	int i, j;
	for(i = 0; i < Ndsc; i++)
	{
		dsc2SetPulseWidth(i, tdcVal, 1);
		dsc2SetPulseWidth(i, trgVal, 2);
		dsc2SetPulseWidth(i, trgoutVal, 3);
	}
	return OK;
}

int
dsc2SetThresholdAll(UINT16 tdcVal, UINT16 trgVal)
{
	int i, j;
	for(i = 0; i < Ndsc; i++)
	{
		for(j = 0; j < 16; j++)
		{
			dsc2SetThreshold(i, j, tdcVal, 1);
			dsc2SetThreshold(i, j, trgVal, 2);
		}
	}
	return OK;
}

/*******************************************************************
 *   Function : dsc2EnableChannel
 *              dsc2DisableChannel
 *                      
 *   Function : Enable/Disable a channel
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT16 chan  - Channel to enable/disable
 *                 UINT16 type  - 1 to set TDC enable
 *                                2 to set TRG enable
 *                                0 to set Both TDC & TRG enable
 *                                                    
 *   Returns -1 if Error, 0 if OK.
 *                                                    
 *******************************************************************/
int
dsc2EnableChannel(UINT32 id, UINT16 chan, UINT16 type)
{
  int wval=0;
  CHECKID(id);

  if(chan>32)
    {
      printf("%s: ERROR: invalid chan=%d.\n",__FUNCTION__,chan);
      return ERROR;
    }

  if(type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }

  wval = (1<<chan);
  DSCLOCK;
  switch(type)
    {
    case 1: /* TDC */
      vmeWrite32(&dscp[id]->chMask,
	       vmeRead32(&dscp[id]->chMask) | (wval));
      break;
    case 2: /* TRG */
      vmeWrite32(&dscp[id]->chMask,
	       vmeRead32(&dscp[id]->chMask) | (wval<<16));
      break;
    case 0: /* TDC and TRG */
    default:
      vmeWrite32(&dscp[id]->chMask,
	       vmeRead32(&dscp[id]->chMask) | ((wval) | (wval<<16)));
    }
  DSCUNLOCK;

  return OK;
}

int
dsc2DisableChannel(UINT32 id, UINT16 chan, UINT16 type)
{
  int wval;
  CHECKID(id);

  if(chan>32)
    {
      printf("%s: ERROR: invalid chan=%d.\n",__FUNCTION__,chan);
      return ERROR;
    }

  if(type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }

  wval = (1<<chan);
  DSCLOCK;
  switch(type)
    {
    case 1: /* TDC */
      vmeWrite32(&dscp[id]->chMask,
	       vmeRead32(&dscp[id]->chMask) & ~(wval));
      break;
    case 2: /* TRG */
      vmeWrite32(&dscp[id]->chMask,
	       vmeRead32(&dscp[id]->chMask) & ~(wval<<16));
      break;
    case 0: /* TDC and TRG */
    default:
      vmeWrite32(&dscp[id]->chMask,
	       vmeRead32(&dscp[id]->chMask) & ~((wval) | (wval<<16)));
    }
  DSCUNLOCK;

  return OK;
}

/*******************************************************************
 *   Function : dsc2SetChannelMask
 *                      
 *   Function : Set the mask of enabled channels
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT16 chMask- Mask of channels to enable
 *                                e.g.
 *                                 enable ch0 and 13:
 *                                  mask = 0x2001
 *                 UINT16 type  - 1 to set TDC mask
 *                                2 to set TRG mask
 *                                0 to set Both TDC & TRG mask
 *                                                    
 *   Returns -1 if Error, 0 if OK.
 *                                                    
 *******************************************************************/

int
dsc2SetChannelMask(UINT32 id, UINT16 chMask, UINT16 type)
{
  CHECKID(id);

  if(type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }

  DSCLOCK;
  switch(type)
    {
    case 1: /* TDC */
      vmeWrite32(&dscp[id]->chMask,
	       vmeRead32(&dscp[id]->chMask)&DSC_CHENABLE_TRG_MASK | (chMask));
      break;
    case 2: /* TRG */
      vmeWrite32(&dscp[id]->chMask,
	       vmeRead32(&dscp[id]->chMask)&DSC_CHENABLE_TDC_MASK | (chMask<<16));
      break;
    case 0: /* TDC and TRG */
    default:
      vmeWrite32(&dscp[id]->chMask,
	       (chMask) | (chMask<<16));
    }
  DSCUNLOCK;

  return OK;
}

/*******************************************************************
 *   Function : dsc2GetChannelMask
 *                      
 *   Function : Get the mask of enabled channels
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT16 type  - 1 to get TDC mask
 *                                2 to get TRG mask
 *                                0 to get Both TDC & TRG mask
 *                                                    
 *   Returns -1 if Error, Enabled channel mask if OK.
 *                                                    
 *******************************************************************/
int
dsc2GetChannelMask(UINT32 id, UINT16 type)
{
  int rval;
  CHECKID(id);

  if(type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }

  DSCLOCK;
  rval = (int)vmeRead32(&dscp[id]->chMask);
  DSCUNLOCK;
  switch(type)
    {
    case 1: /* TDC */
      rval &= DSC_CHENABLE_TDC_MASK;
      break;
    case 2: /* TRG */
      rval = (rval&DSC_CHENABLE_TRG_MASK)>>16;
      break;
    case 0: /* TDC and TRG */
      break;
    }

  return rval;
}

/*******************************************************************
 *   Function : dsc2EnableChannelOR
 *              dsc2DisableChannelOR
 *                      
 *   Function : Enable/Disable a channel contributing to 
 *              the OR Output
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT16 chan  - Channel to enable/disable
 *                 UINT16 type  - 1 to set TDC enable
 *                                2 to set TRG enable
 *                                0 to set Both TDC & TRG
 *                                                    
 *   Returns -1 if Error, 0 if OK.
 *                                                    
 *******************************************************************/
int
dsc2EnableChannelOR(UINT32 id, UINT16 chan, UINT16 type)
{
  int wval=0;
  CHECKID(id);

  if(chan>32)
    {
      printf("%s: ERROR: invalid chan=%d.\n",__FUNCTION__,chan);
      return ERROR;
    }

  if(type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }

  wval = (1<<chan);
  DSCLOCK;
  switch(type)
    {
    case 1: /* TDC */
      vmeWrite32(&dscp[id]->orMask,
	       vmeRead32(&dscp[id]->orMask) | (wval));
      break;
    case 2: /* TRG */
      vmeWrite32(&dscp[id]->orMask,
	       vmeRead32(&dscp[id]->orMask) | (wval<<16));
      break;
    case 0: /* TDC and TRG */
    default:
      vmeWrite32(&dscp[id]->orMask,
	       vmeRead32(&dscp[id]->orMask) | ((wval) | (wval<<16)));
    }
  DSCUNLOCK;

  return OK;
}

int
dsc2DisableChannelOR(UINT32 id, UINT16 chan, UINT16 type)
{
  int wval;
  CHECKID(id);

  if(chan>32)
    {
      printf("%s: ERROR: invalid chan=%d.\n",__FUNCTION__,chan);
      return ERROR;
    }

  if(type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }

  wval = (1<<chan);
  DSCLOCK;
  switch(type)
    {
    case 1: /* TDC */
      vmeWrite32(&dscp[id]->orMask,
	       vmeRead32(&dscp[id]->orMask) & ~(wval));
      break;
    case 2: /* TRG */
      vmeWrite32(&dscp[id]->orMask,
	       vmeRead32(&dscp[id]->orMask) & ~(wval<<16));
      break;
    case 0: /* TDC and TRG */
    default:
      vmeWrite32(&dscp[id]->orMask,
	       vmeRead32(&dscp[id]->orMask) & ~((wval) | (wval<<16)));
    }
  DSCUNLOCK;

  return OK;
}


/*******************************************************************
 *   Function : dsc2SetChannelORMask
 *                      
 *   Function : Set the mask of enabled channels contributing
 *              to the OR Ouput
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT16 chMask- Mask of channels to enable
 *                                e.g.
 *                                 enable ch0 and 13:
 *                                  mask = 0x2001
 *                 UINT16 type  - 1 to set TDC OR mask
 *                                2 to set TRG OR mask
 *                                0 to set Both TDC & TRG OR mask
 *                                                    
 *   Returns -1 if Error, 0 if OK.
 *                                                    
 *******************************************************************/

int
dsc2SetChannelORMask(UINT32 id, UINT16 chMask, UINT16 type)
{
  CHECKID(id);

  if(type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }

  DSCLOCK;
  switch(type)
    {
    case 1: /* TDC */
      vmeWrite32(&dscp[id]->orMask,
	       vmeRead32(&dscp[id]->orMask)&DSC_ORMASK_TRG_MASK | (chMask));
      break;
    case 2: /* TRG */
      vmeWrite32(&dscp[id]->orMask,
	       vmeRead32(&dscp[id]->orMask)&DSC_ORMASK_TDC_MASK | (chMask<<16));
      break;
    case 0: /* TDC and TRG */
    default:
      vmeWrite32(&dscp[id]->orMask,
	       ((chMask) | (chMask<<16)));
    }
  DSCUNLOCK;

  return OK;
}

/*******************************************************************
 *   Function : dsc2GetChannelORMask
 *                      
 *   Function : Get the mask of enabled channels contributing to
 *              the OR Output
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT16 type  - 1 to get TDC OR mask
 *                                2 to get TRG OR mask
 *                                0 to get Both TDC & TRG OR mask
 *                                                    
 *   Returns -1 if Error, Enabled channel mask if OK.
 *                                                    
 *******************************************************************/
int
dsc2GetChannelORMask(UINT32 id, UINT16 type)
{
  int rval;
  CHECKID(id);

  if(type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }

  DSCLOCK;
  rval = (int)vmeRead32(&dscp[id]->orMask);
  DSCUNLOCK;
  switch(type)
    {
    case 1: /* TDC */
      rval &= DSC_ORMASK_TDC_MASK;
      break;
    case 2: /* TRG */
      rval = (rval&DSC_ORMASK_TRG_MASK)>>16;
      break;
    case 0: /* TDC and TRG */
      break;
    }

  return rval;
}

/*******************************************************************
 *   Function : dsc2SetDelay
 *                                                    
 *   Function : Set the delay to the scaler and/or trigger output
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT16 val   - Delay
 *                                units = 8ns for Scaler
 *                                      = 4ns for TRG Output
 *                 UINT16 type  - 1 to set Scaler delay
 *                                2 to set TRG Output delay
 *                                                    
 *   Returns -1 if Error, 0 if OK.                    
 *                                                    
 *******************************************************************/

int
dsc2SetDelay(UINT32 id, UINT16 val, UINT16 type)
{
  UINT32 oldval=0;
  CHECKID(id);
  
  if(type==0 || type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }

  if(val > 0x7f)
    {
      printf("%s: ERROR: val=%d greater than maximum allowed (%d).\n",__FUNCTION__,val,0x3f);
      return ERROR;
    }

  DSCLOCK;
  switch(type)
    {
    case 1:  /* Scaler */
      /* Save the TRG Output delay value so it's not lost */
      oldval = vmeRead32(&dscp[id]->delay)&DSC_DELAY_TRG_MASK;
      vmeWrite32(&dscp[id]->delay,
	       (val) | oldval);
      break;
    case 2:  /* TRG Output */
    default:
      /* Save the Scaler delay value so it's not lost */
      oldval = vmeRead32(&dscp[id]->delay)&DSC_DELAY_SCALER_MASK;
      vmeWrite32(&dscp[id]->delay,
	       ((val)<<16) | oldval);
      break;
    }
  DSCUNLOCK;

  return OK;
}

/*******************************************************************
 *   Function : dsc2GetDelay
 *                                                    
 *   Function : Get the delay to the scaler and/or trigger output
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT16 type  - 1 to set Scaler delay
 *                                2 to set TRG Output delay
 *                                                    
 *   Returns -1 if Error, Value of width if OK.
 *                                                    
 *******************************************************************/

int
dsc2GetDelay(UINT32 id, UINT16 type)
{
  UINT32 rval;
  CHECKID(id);

  if(type==0 || type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }


  DSCLOCK;
  rval = vmeRead32(&dscp[id]->delay);
  DSCUNLOCK;
  switch(type)
    {
    case 1:  /* Scaler */
      rval = rval&DSC_DELAY_SCALER_MASK;
      break;
    case 2:  /* TRG Output */
    default:
      rval = (rval&DSC_DELAY_TRG_MASK)>>16;
      break;
    }
  
  return rval;
}

/*******************************************************************
 *   Function : dsc2Test
 *                      
 *   Function : Enable/Disable front panel test input
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT32 flag  - 0 to disable test input
 *                              >=1 to enable test input
 *                                                    
 *   Returns -1 if Error, 0 if OK.
 *                                                    
 *******************************************************************/

int
dsc2Test(UINT32 id, UINT32 flag)
{
  CHECKID(id);  

  DSCLOCK;
  if(flag==0)
    {
      vmeWrite32(&dscp[id]->testCtrl,0);
    }
  else
    {
      vmeWrite32(&dscp[id]->testCtrl,1);
    }
  DSCUNLOCK;

  return OK;
}

/*******************************************************************
 *   Function : dsc2LatchScalers
 *                      
 *   Function : Latch the scalers for readout.
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT32 flag  - 0 to latch the VME scalers
 *                                1 to latch the Gated scalers
 *                                2 to latch VME and Gated scalers
 *                                                    
 *   Returns -1 if Error, 0 if OK.
 *                                                    
 *******************************************************************/

int
dsc2LatchScalers(UINT32 id, UINT16 type)
{
  CHECKID(id);

  if(type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }

  DSCLOCK;
  switch(type)
    {
    case 0: /* VME scalers */
      vmeWrite32(&dscp[id]->vmeScalerLatch,1);
	  /*      printf("ungated latch\n");*/
      break;
    case 1: /* Gated scalers */
      vmeWrite32(&dscp[id]->gatedScalerLatch,1);
	  /*      printf("gated latch\n");*/
      break;
    case 2: /* Both */
      vmeWrite32(&dscp[id]->vmeScalerLatch,1);
      vmeWrite32(&dscp[id]->gatedScalerLatch,1);
	  /*      printf("both latch\n");*/
    }
  DSCUNLOCK;

  return OK;
}

/*******************************************************************
 *   Function : dsc2ReadScalers
 *                      
 *   Function : Read values from scalers.  Perform latch (before read)
 *              if requested.
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT32 *data - local memory address to place data
 *                 UINT32 nwrds - Max number of words to transfer
 *                 UINT32 rflag - Readout flag
 *                                 bit 0 - TRG scalers
 *                                 bit 1 - TDC scalers
 *                                 bit 2 - TRG VME scalers
 *                                 bit 3 - TDC VME scalers
 *                                 bit 4 - Reference Scaler
 *                                 bit 5 - Reference VME scaler
 *                                 bit 6 - Latch VME before read
 *                                 bit 7 - Latch GATED before read
 *                                                    
 *   Returns -1 if Error, Number of words transferred if OK.
 *                                                    
 *******************************************************************/

int
dsc2ReadScalers(UINT32 id, volatile UINT32 *data, int nwrds, int rflag)
{
  int dCnt=0, ichan;
  CHECKID(id);

  if(data==NULL) 
    {
      logMsg("%s: ERROR: Invalid Destination address\n",(int)__FUNCTION__,0,0,0,0,0);
      return(ERROR);
    }
  
  /* Latch, if requested */
  if(rflag & DSC_LATCH_GATED_BEFORE_READOUT)
    {
      dsc2LatchScalers(id,1);
    }
  if(rflag & DSC_LATCH_VME_BEFORE_READOUT)
    {
      dsc2LatchScalers(id,0);
    }

  /* Insert DSC header word */
  data[dCnt] = DSC_SCT_HEADER | (id<<8) | rflag;
  dCnt++;

  DSCLOCK;
  /* Readout each requested scaler channels */
  if(rflag & DSC_READOUT_TRG)
    {
      for(ichan=0; ichan<16; ichan++)
	{
	  if(dCnt==nwrds)
	    {
	      logMsg("%s: ERROR: More data than what was requested (nwrds = %d)",
		     (int)__FUNCTION__,nwrds,3,4,5,6);
	      DSCUNLOCK;
	      return dCnt;
	    }
	  data[dCnt] = vmeRead32(&dscp[id]->trgScaler[ichan]);
	  dCnt++;
	}
    }
  if(rflag & DSC_READOUT_TDC)
    {
      for(ichan=0; ichan<16; ichan++)
	{
	  if(dCnt==nwrds)
	    {
	      logMsg("%s: ERROR: More data than what was requested (nwrds = %d)",
		     (int)__FUNCTION__,nwrds,3,4,5,6);
	      DSCUNLOCK;
	      return dCnt;
	    }
	  data[dCnt] = vmeRead32(&dscp[id]->tdcScaler[ichan]);
	  dCnt++;
	}
    }
  if(rflag & DSC_READOUT_TRGVME)
    {
      for(ichan=0; ichan<16; ichan++)
	{
	  if(dCnt==nwrds)
	    {
	      logMsg("%s: ERROR: More data than what was requested (nwrds = %d)",
		     (int)__FUNCTION__,nwrds,3,4,5,6);
	      DSCUNLOCK;
	      return dCnt;
	    }
	  data[dCnt] = vmeRead32(&dscp[id]->trgVmeScaler[ichan]);
	  dCnt++;
	}
    }
  if(rflag & DSC_READOUT_TDCVME)
    {
      for(ichan=0; ichan<16; ichan++)
	{
	  if(dCnt==nwrds)
	    {
	      logMsg("%s: ERROR: More data than what was requested (nwrds = %d)",
		     (int)__FUNCTION__,nwrds,3,4,5,6);
	      DSCUNLOCK;
	      return dCnt;
	    }
	  data[dCnt] = vmeRead32(&dscp[id]->tdcVmeScaler[ichan]);
	  dCnt++;
	}
    }
  if(rflag & DSC_READOUT_REF)
    {
      if(dCnt==nwrds)
	{
	  logMsg("%s: ERROR: More data than what was requested (nwrds = %d)",
		 (int)__FUNCTION__,nwrds,3,4,5,6);
	      DSCUNLOCK;
	      return dCnt;
	}
      data[dCnt] = vmeRead32(&dscp[id]->refGatedScaler);
      dCnt++;
    }
  if(rflag & DSC_READOUT_REFVME)
    {
      if(dCnt==nwrds)
	{
	  logMsg("%s: ERROR: More data than what was requested (nwrds = %d)",
		 (int)__FUNCTION__,nwrds,3,4,5,6);
	      DSCUNLOCK;
	      return dCnt;
	}
      data[dCnt] = vmeRead32(&dscp[id]->refVmeScaler);
      dCnt++;
    }

  DSCUNLOCK;
  return dCnt;

}

/*******************************************************************
 *   Function : dsc2PrintScalers
 *                      
 *   Function : Print values from scalers to standard out.  Perform 
 *              latch (before read) if requested.
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT32 rflag - Readout flag
 *                                 bit 0 - TRG scalers
 *                                 bit 1 - TDC scalers
 *                                 bit 2 - TRG VME scalers
 *                                 bit 3 - TDC VME scalers
 *                                 bit 4 - Reference Scaler
 *                                 bit 5 - Reference VME scaler
 *                                 bit 6 - Latch VME before read
 *                                 bit 7 - Latch GATED before read
 *                                                    
 *   Returns -1 if Error, Number of words transferred if OK.
 *                                                    
 *******************************************************************/

int
dsc2PrintScalers(UINT32 id, int rflag)
{
  volatile UINT32 data[4*16+3];
  int nwrds=4*16+3;
  int dCnt, iword=0, ii;
  UINT32 header=0, header_rflag=0;
  CHECKID(id);

  printf("%s: Scalers read for DSC in slot %d:\n\n",(int)__FUNCTION__,id);
  dCnt = dsc2ReadScalers(id,(volatile UINT32 *)&data,nwrds,rflag);
  if(dCnt!=ERROR)
    {
      /* First word should be the header */
      header = data[0];
      if((header & 0xffff0000) == DSC_SCT_HEADER)
	{
	  printf("  HEADER WORD = 0x%08x\n",header);
	}
      else
	{
	  printf("  INVALID HEADER WORD (0x%08x)\n",header);
	  return ERROR;
	}
      header_rflag = header & 0xffff;
      iword++;
      if(header_rflag & DSC_READOUT_TRG)
	{
	  printf("  Data from TRG scalers:\n");
	  for(ii=0; ii<16; ii++)
	    {
	      printf("     0x%08x",data[iword]);
	      if((ii-3)%4==0) printf("\n");
	      iword++;
	    }
	}
      if(header_rflag & DSC_READOUT_TDC)
	{
	  printf("  Data from TDC scalers:\n");
	  for(ii=0; ii<16; ii++)
	    {
	      printf("     0x%08x",data[iword]);
	      if((ii-3)%4==0) printf("\n");
	      iword++;
	    }
	}
      if(header_rflag & DSC_READOUT_TRGVME)
	{
	  printf("  Data from TRG VME scalers:\n");
	  for(ii=0; ii<16; ii++)
	    {
	      printf("     0x%08x",data[iword]);
	      if((ii-3)%4==0) printf("\n");
	      iword++;
	    }
	}
      if(header_rflag & DSC_READOUT_TDCVME)
	{
	  printf("  Data from TDC VME scalers:\n");
	  for(ii=0; ii<16; ii++)
	    {
	      printf("     0x%08x",data[iword]);
	      if((ii-3)%4==0) printf("\n");
	      iword++;
	    }
	}
      if(header_rflag & DSC_READOUT_REF)
	{
	  printf("  Data from Reference scaler:\n");
	  printf("     0x%08x\n",data[iword]);
	  iword++;
	}
      if(header_rflag & DSC_READOUT_REFVME)
	{
	  printf("  Data from Reference VME scaler:\n");
	  printf("     0x%08x\n",data[iword]);
	  iword++;
	}
    }
  else
    {
      printf("%s: ERROR: dsc2ReadScalers returned ERROR\n");
      return ERROR;
    }

  return dCnt;
}

/*******************************************************************
 *   Function : dsc2PrintScalerRates
 *                      
 *   Function : Print rates from scalers to standard out.  Latch
 *              is performed based on chosen scalers (gated or 
 *              non-gated) to print in rflag.
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT32 rflag - Readout flag
 *                                 bit 0 - TRG scalers
 *                                 bit 1 - TDC scalers
 *                                 bit 2 - TRG VME scalers
 *                                 bit 3 - TDC VME scalers
 *                                                    
 *   Returns -1 if Error, 0 if OK.
 *                                                    
 *******************************************************************/

int
dsc2PrintScalerRates(UINT32 id, int rflag)
{
  UINT32 latch_flag=0;
  volatile UINT32 data[4*16+3];
  int nwrds=4*16+3;
  int dCnt, iword=0, ii;
  unsigned int bitflag,bitcount;
  UINT32 header=0, header_rflag=0;
  UINT32 refScaler=0, refGatedScaler=0;
  double refTime=0, refGatedTime=0;

  CHECKID(id);

  if(rflag>0xf)
    {
      printf("%s: ERROR: Invalid rflag (0x%x)\n",
	     (int)__FUNCTION__,rflag);
      return ERROR;
    }
  if(rflag==0)
    {
      printf("%s: WARN: Nothing to do with rflag=0x%x\n",
	     (int)__FUNCTION__,rflag);
      return OK;
    }

  if((rflag&DSC_READOUT_TRG) || (rflag&DSC_READOUT_TDC))
    {
      latch_flag |= (DSC_LATCH_GATED_BEFORE_READOUT | DSC_READOUT_REF);
    }
  if((rflag&DSC_READOUT_TRGVME) || (rflag&DSC_READOUT_TDCVME))
    {
      latch_flag |= (DSC_LATCH_VME_BEFORE_READOUT | DSC_READOUT_REFVME);
    }

  /* Count the bits in rflag */
  bitflag = rflag;
  for (bitcount = 0; bitflag; bitcount++)
      bitflag &= bitflag - 1; 

  dCnt = dsc2ReadScalers(id,(volatile UINT32 *)&data,nwrds,rflag|latch_flag);

  if(dCnt!=ERROR)
    {
      printf("%s: Scaler rates read for DSC in slot %d:\n\n",(int)__FUNCTION__,id);

      /* First word should be the header */
      header = data[0];
      if((header & 0xffff0000) == DSC_SCT_HEADER)
	{
	  printf("  HEADER WORD = 0x%08x\n",header);
	}
      else
	{
	  printf("  INVALID HEADER WORD (0x%08x)\n",header);
	  return ERROR;
	}
      header_rflag = header & 0xffff;

      /* Jump ahead in the data and get the reference counts */
      if(latch_flag & DSC_LATCH_GATED_BEFORE_READOUT)
	{
	  refScaler = data[16*bitcount+1];
	  if(refScaler==0)
	    {
	      printf("%s: ERROR: Reference scaler returned 0 counts\n",
		     (int)__FUNCTION__);
	      return ERROR;
	    }
	  else if(refScaler==-1)
	    {
	      printf("%s: ERROR: Reference scaler count saturated\n",
		     (int)__FUNCTION__);
	      return ERROR;
	    }
	  else
	    {
	      refTime = (double)refScaler/DSC_REFERENCE_RATE;
	      printf("  Time since last latch = %lfs\n",refTime);
	    }
	  
	  if(latch_flag & DSC_LATCH_VME_BEFORE_READOUT)
	    {
	      refGatedScaler = data[16*bitcount+2];
	      if(refGatedScaler==0)
		{
		  printf("%s: ERROR: Reference Gated scaler returned 0 counts\n",
			 (int)__FUNCTION__);
		  return ERROR;
		}
	      else if(refGatedScaler==-1)
		{
		  printf("%s: ERROR: Reference Gated scaler count saturated\n",
			 (int)__FUNCTION__);
		  return ERROR;
		}
	      else
		{
		  refGatedTime = (double)refGatedScaler/DSC_REFERENCE_RATE;
		  printf("  Time since last gated latch = %lfs\n",refGatedTime);
		}
	    }
	}
      else
	{
	  refGatedScaler = data[16*bitcount+1];
	  if(refGatedScaler==0)
	    {
	      printf("%s: ERROR: Reference Gated scaler returned 0 counts\n",
		     (int)__FUNCTION__);
	      return ERROR;
	    }
	  else if(refGatedScaler==-1)
	    {
	      printf("%s: ERROR: Reference Gated scaler count saturated\n",
		     (int)__FUNCTION__);
	      return ERROR;
	    }
	  else
	    {
	      refGatedTime = (double)refGatedScaler/DSC_REFERENCE_RATE;
	      printf("  Time since last gated latch = %lfs\n",refGatedTime);
	    }
	}

      iword++;
      if(header_rflag & DSC_READOUT_TRG)
	{
	  printf("  Rates from TRG scalers (Hz):\n");
	  for(ii=0; ii<16; ii++)
	    {
	      printf("  %12.3f",(double)data[iword]/refGatedTime);
	      if((ii-3)%4==0) printf("\n");
	      iword++;
	    }
	}
      if(header_rflag & DSC_READOUT_TDC)
	{
	  printf("  Rates from TDC scalers (Hz):\n");
	  for(ii=0; ii<16; ii++)
	    {
	      printf("  %12.3f",(double)data[iword]/refGatedTime);
	      if((ii-3)%4==0) printf("\n");
	      iword++;
	    }
	}
      if(header_rflag & DSC_READOUT_TRGVME)
	{
	  printf("  Rates from TRG VME scalers (Hz):\n");
	  for(ii=0; ii<16; ii++)
	    {
	      printf("  %12.3f",(double)data[iword]/refTime);
	      if((ii-3)%4==0) printf("\n");
	      iword++;
	    }
	}
      if(header_rflag & DSC_READOUT_TDCVME)
	{
	  printf("  Rates from TDC VME scalers (Hz):\n");
	  for(ii=0; ii<16; ii++)
	    {
	      printf("  %12.3f",(double)data[iword]/refTime);
	      if((ii-3)%4==0) printf("\n");
	      iword++;
	    }
	}
    }
  else
    {
      printf("%s: ERROR: dsc2ReadScalers returned ERROR\n");
      return ERROR;
    }

  return dCnt;
}

void
dsc2FlashChipSelect(UINT32 id, int select)
{
	if(select)
		vmeWrite32(&dscp[id]->calCmd,1);
	else
		vmeWrite32(&dscp[id]->calCmd,2);
	vmeWrite32(&dscp[id]->calExe,1);
	while(vmeRead32(&dscp[id]->calCmd) != 0xFFFFFFFF);
}

int
dsc2FlashGetId(UINT32 id)
{
	return 0x20;
	vmeWrite32(&dscp[id]->calBuf[1],0x9F);
	vmeWrite32(&dscp[id]->calBuf[0],2);
	vmeWrite32(&dscp[id]->calCmd,3 | 0x30000);	/* read flash id*/
	vmeWrite32(&dscp[id]->calExe,1);
	while(vmeRead32(&dscp[id]->calCmd) != 0xFFFFFFFF);

	return vmeRead32(&dscp[id]->calBuf[2]) & 0xFF;
}

int
dsc2FlashPollStatus(UINT32 id)
{
	vmeWrite32(&dscp[id]->calBuf[1],0x05);
	vmeWrite32(&dscp[id]->calBuf[0],2);
	vmeWrite32(&dscp[id]->calCmd,3 | 0x30000);	/* read flash status*/
	vmeWrite32(&dscp[id]->calExe,1);
	while(vmeRead32(&dscp[id]->calCmd) != 0xFFFFFFFF);

	return vmeRead32(&dscp[id]->calBuf[2]) & 0xFF;
}

int
dsc2Calibrate(UINT32 id)
{
	vmeWrite32(&dscp[id]->calCmd,0xFF);	/* run calibration*/
	vmeWrite32(&dscp[id]->calExe,1);
	while(vmeRead32(&dscp[id]->calCmd) != 0xFFFFFFFF);

	return OK;
}

int
dsc2CalibrateAll(void)
{
	int i;
	
	for(i = 0; i < Ndsc; i++)
	{
		printf("Calibrating unit %d...", i);
		dsc2Calibrate(i);
		printf("complete.\n");
	}
	return OK;
}

int
dsc2ReloadFPGA(UINT32 id)
{
	vmeWrite32(&dscp[id]->calCmd,6);	/* reload fpga*/
	vmeWrite32(&dscp[id]->calExe,1);
}

int
dsc2UpdateFirmwareAll(const char *filename)
{
	int i, result;
	
	for(i = 0; i < Ndsc; i++)
	{
		printf("Updating firmware on unit %d...", i);
		result = dsc2UpdateFirmware(i, filename);
		if(result != OK)
		{
			printf("failed.\n");
			return result;
		}
		else
			printf("succeeded.");
		
		printf(" Verifying.");
		result = dsc2VerifyFirmware(i, filename);
		if(result != OK)
		{
			printf("failed.\n");
			return result;
		}
		else
			printf("ok.\n");

		dsc2ReloadFPGA(i);
	}
	return OK;
}

int
dsc2UpdateFirmware(UINT32 id, const char *filename)
{
	FILE *f;
	int i, flashId = 0;
	unsigned int addr = 0;
	unsigned char buf[528];

	CHECKID(id);

	f = fopen(filename, "rb");
	if(!f)
	{
		printf("%s: ERROR: dsc2UpdateFirmware invalid file %s\n", (int)__FUNCTION__, filename);
		return ERROR;
	}

	DSCLOCK;
	
	dsc2FlashChipSelect(id, 0);
	flashId = dsc2FlashGetId(id);

	if(flashId == 0x20)			/* Numonyx flash*/
	{
		memset(buf, 0xff, 256);
		while(fread(buf, 1, 256, f) > 0)
		{
			if(!(addr % 65536))		/* sector erase*/
			{
				vmeWrite32(&dscp[id]->calBuf[1],0x06);	/* write enable*/
				vmeWrite32(&dscp[id]->calBuf[0],1);
				vmeWrite32(&dscp[id]->calCmd,3 | 0x30000);
				vmeWrite32(&dscp[id]->calExe,1);
				while(vmeRead32(&dscp[id]->calCmd) != 0xFFFFFFFF);

				vmeWrite32(&dscp[id]->calCmd,3 | 0x30000);
				vmeWrite32(&dscp[id]->calBuf[4],(addr)     & 0xFF);
				vmeWrite32(&dscp[id]->calBuf[3],(addr>>8)  & 0xFF);
				vmeWrite32(&dscp[id]->calBuf[2],(addr>>16) & 0xFF);
				vmeWrite32(&dscp[id]->calBuf[1],0xD8);	/* erase sector*/
				vmeWrite32(&dscp[id]->calBuf[0],4);
				vmeWrite32(&dscp[id]->calExe,1);
				while(vmeRead32(&dscp[id]->calCmd) != 0xFFFFFFFF);

				i = 0;
				while(1)
				{
					if(!(dsc2FlashPollStatus(id) & 0x1))	/* ~10us per poll*/
						break;
					if(!(i % 30000))
						printf(".");
					if(i == 300000)	/* max 3s sector erase time*/
					{
						DSCUNLOCK;
						fclose(f);
					}
					i++;
				}
			}

			vmeWrite32(&dscp[id]->calBuf[1],0x06);	/* write enable*/
			vmeWrite32(&dscp[id]->calBuf[0],1);
			vmeWrite32(&dscp[id]->calCmd,3 | 0x30000);
			vmeWrite32(&dscp[id]->calExe,1);
			while(vmeRead32(&dscp[id]->calCmd) != 0xFFFFFFFF);

			vmeWrite32(&dscp[id]->calBuf[4],(addr)     & 0xFF);
			vmeWrite32(&dscp[id]->calBuf[3],(addr>>8)  & 0xFF);
			vmeWrite32(&dscp[id]->calBuf[2],(addr>>16) & 0xFF);
			vmeWrite32(&dscp[id]->calBuf[1],0x02);	/* page write*/
			vmeWrite32(&dscp[id]->calBuf[0],256+4);
			vmeWrite32(&dscp[id]->calCmd,3 | 0x30000);
			for(i = 255; i >= 0; i--)
				vmeWrite32(&dscp[id]->calBuf[5+i], buf[i]);
			vmeWrite32(&dscp[id]->calExe,1);
			while(vmeRead32(&dscp[id]->calCmd) != 0xFFFFFFFF);

			i = 0;
			while(1)
			{
				if(!(dsc2FlashPollStatus(id) & 0x1))	/* ~10us per poll*/
					break;
				if(i == 500)	/* max 5ms page program time*/
				{
					DSCUNLOCK;
					fclose(f);
				}
				i++;
			}

			addr+= 256;
			memset(buf, 0xff, 256);
		}
	}
	else if(flashId == 0x1F)	/* Atmel flash*/
	{
	}
	else
	{
		DSCUNLOCK;
		fclose(f);
		printf("%s: ERROR: dsc2UpdateFirmware failed to identify flash id 0x%02X\n", (int)__FUNCTION__, flashId);
		return ERROR;
	}
	DSCUNLOCK;
	fclose(f);
	return OK;
}

int
dsc2VerifyFirmware(UINT32 id, const char *filename)
{
	FILE *f;
	int i,len, flashId = 0;
	unsigned int addr = 0;
	unsigned char buf[528];

	CHECKID(id);

	f = fopen(filename, "rb");
	if(!f)
	{
		printf("%s: ERROR: dsc2VerifyFirmware invalid file %s\n", (int)__FUNCTION__, filename);
		return ERROR;
	}

	DSCLOCK;
	
	dsc2FlashChipSelect(id, 0);
	flashId = dsc2FlashGetId(id);

	if(flashId == 0x20)			/* Numonyx flash*/
	{
		vmeWrite32(&dscp[id]->calBuf[4],0x00);
		vmeWrite32(&dscp[id]->calBuf[3],0x00);
		vmeWrite32(&dscp[id]->calBuf[2],0x00);
		vmeWrite32(&dscp[id]->calBuf[1],0x03);	/* read array*/
		vmeWrite32(&dscp[id]->calBuf[0],4);
		vmeWrite32(&dscp[id]->calCmd,3 | 0x10000);
		vmeWrite32(&dscp[id]->calExe,1);
		while(vmeRead32(&dscp[id]->calCmd) != 0xFFFFFFFF);

		while((len = fread(buf, 1, 256, f)) > 0)
		{
			vmeWrite32(&dscp[id]->calBuf[0],256);
			vmeWrite32(&dscp[id]->calCmd,3);
			vmeWrite32(&dscp[id]->calExe,1);
			while(vmeRead32(&dscp[id]->calCmd) != 0xFFFFFFFF);

			for(i = len-1; i >= 0; i--)
			{
				if(buf[i] != vmeRead32(&dscp[id]->calBuf[1+i]))
				{
					dsc2FlashChipSelect(id, 0);
					DSCUNLOCK;
					fclose(f);					
					printf("%s: ERROR: dsc2VerifyFirmware failed verify at addess 0x%08X[%02X,%02X]\n", (int)__FUNCTION__, addr+i, buf[i], (int)vmeRead32(&dscp[id]->calBuf[1+i]));
					return ERROR;
				}
			}
			addr+=256;
		}
		dsc2FlashChipSelect(id, 0);
	}
	else if(flashId == 0x1F)	/* Atmel flash*/
	{
	}
	else
	{
		DSCUNLOCK;
		fclose(f);
		printf("%s: ERROR: dsc2VerifyFirmware failed to identify flash id 0x%02X\n", (int)__FUNCTION__, flashId);
		return ERROR;
	}
	DSCUNLOCK;
	fclose(f);
	return OK;
}

int
dsc2ReadFirmware(UINT32 id, const char *filename)
{
	FILE *f;
	int i, flashId = 0;
	unsigned int addr = 0;
	unsigned char buf[528];

	CHECKID(id);

	f = fopen(filename, "wb");
	if(!f)
	{
		printf("%s: ERROR: dsc2ReadFirmware invalid file %s\n", (int)__FUNCTION__, filename);
		return ERROR;
	}

	DSCLOCK;
	
	dsc2FlashChipSelect(id, 0);
	flashId = dsc2FlashGetId(id);

	if(flashId == 0x20)			/* Numonyx flash*/
	{
		vmeWrite32(&dscp[id]->calBuf[4],0x00);
		vmeWrite32(&dscp[id]->calBuf[3],0x00);
		vmeWrite32(&dscp[id]->calBuf[2],0x00);
		vmeWrite32(&dscp[id]->calBuf[1],0x03);	/* read array*/
		vmeWrite32(&dscp[id]->calBuf[0],4);
		vmeWrite32(&dscp[id]->calCmd,3 | 0x10000);
		vmeWrite32(&dscp[id]->calExe,1);
		while(vmeRead32(&dscp[id]->calCmd) != 0xFFFFFFFF);

		while(addr < 2*1024*1024)
		{
			printf("addr 0x%08X\n", addr);

			vmeWrite32(&dscp[id]->calBuf[0],256);
			vmeWrite32(&dscp[id]->calCmd,3);
			vmeWrite32(&dscp[id]->calExe,1);
			while(vmeRead32(&dscp[id]->calCmd) != 0xFFFFFFFF);

			for(i = 255; i >= 0; i--)
				buf[i] = vmeRead32(&dscp[id]->calBuf[1+i]);

			addr+= 256;
			fwrite(buf, 1, 256, f);
		}
		dsc2FlashChipSelect(id, 0);
	}
	else if(flashId == 0x1F)	/* Atmel flash*/
	{
	}
	else
	{
		DSCUNLOCK;
		fclose(f);
		printf("%s: ERROR: dsc2UpdateFirmware failed to identify flash id 0x%02X\n", (int)__FUNCTION__, flashId);
		return ERROR;
	}
	DSCUNLOCK;
	fclose(f);
	return OK;
}



/*******************************************************************
 *  SP stuff
 *******************************************************************/

/*

examples from SP:
dsc2Init(0x10d10000,0x10000,8)
dsc2MonAll()
dsc2MonBoard(0)
dsc2SetTRGOutputWidth(0x90d10000,20)
dsc2SetTDCPulserWidth(0x90d10000,40)
dsc2SetTRGOutputDelay(0x90d10000,4)

examples from Bryan:
dsc2Status(7,0)
dsc2PrintScalers(0,0xff)

dsc2ReadBoardDmaStart(0, data, nwrds, 0xff)

*/


#if defined(VXWORKS) || defined(Linux_vme)

#define SP_DEBUGx

#define F_NAME    100       /* length of config. file name */
#define STRLEN    250       /* length of str_tmp */
#define ROCLEN    20        /* length of ROC_name */
#define NBOARDS   DSC_MAX_BOARDS


/* Define global variables */
static UINT32  dsc2_BaseAddr        [NBOARDS];
static UINT32  dsc2_vmeAddr         [NBOARDS];
static int     dsc2_TDC_Threshold   [NBOARDS][16];
static int     dsc2_TRG_Threshold   [NBOARDS][16];
static int     dsc2_TDC_PulserWidth [NBOARDS];
static int     dsc2_TRG_PulserWidth [NBOARDS];
static int     dsc2_TRG_OutputWidth [NBOARDS];
static UINT32  dsc2_Channel_Mask    [NBOARDS];
static UINT32  dsc2_OR_Mask         [NBOARDS];
static int     dsc2_Scaler_Delay    [NBOARDS];
static int     dsc2_TRG_OutputDelay [NBOARDS];
static int     dsc2_Nconf           [NBOARDS];
static int     dsc2_NNconf;
static int     dsc2_NN;



/* Function Prototypes */
int    dsc2Init           (UINT32 addr, UINT32 addr_inc, int ndsc2);
void   dsc2InitGlobals    ();
int    dsc2ReadConfigFile (char *fname);
int    dsc2DownloadAll    ();
int    dsc2DownloadConfig ();
int    dsc2SetConfig      (char *fname);
void   dsc2MonAll         ();
void   dsc2MonConfig      ();
void   dsc2MonBoard       (int board_number);
/*============================================================*/
int    dsc2GetBoardID          (UINT32 addr);
int    dsc2GetFirmwareRevMin   (UINT32 addr);
int    dsc2GetFirmwareRevMaj   (UINT32 addr);
char  *dsc2GetBoardRev         (UINT32 addr);
int    dsc2GetBoardSN          (UINT32 addr);
/*============================================================*/
void   dsc2MonAllThreshold     (UINT32 addr);
int    dsc2GetTDCThreshold     (UINT32 addr, int channel);
int    dsc2GetTRGThreshold     (UINT32 addr, int channel);
int    dsc2SetTDCThreshold     (UINT32 addr, int channel, int thresh);
int    dsc2SetTRGThreshold     (UINT32 addr, int channel, int thresh);
int    dsc2SetUniTDCThreshold  (UINT32 addr, int thresh);
int    dsc2SetUniTRGThreshold  (UINT32 addr, int thresh);
/*============================================================*/
void   dsc2MonAllWidth         (UINT32 addr);
int    dsc2GetTDCPulserWidth   (UINT32 addr);
int    dsc2GetTRGPulserWidth   (UINT32 addr);
int    dsc2GetTRGOutputWidth   (UINT32 addr);
int    dsc2SetTDCPulserWidth   (UINT32 addr, int width);
int    dsc2SetTRGPulserWidth   (UINT32 addr, int width);
int    dsc2SetTRGOutputWidth   (UINT32 addr, int width);
/*============================================================*/
int    dsc2SetTRGandTDCmask    (UINT32 addr, UINT32 mask);
int    dsc2GetTRGandTDCmask    (UINT32 addr);
int    dsc2GetTDCmask          (UINT32 addr);
int    dsc2GetTRGmask          (UINT32 addr);
int    dsc2EnableTDCch         (UINT32 addr, int channel);
int    dsc2EnableTRGch         (UINT32 addr, int channel);
int    dsc2EnableAllTDCch      (UINT32 addr);
int    dsc2EnableAllTRGch      (UINT32 addr);
int    dsc2EnableAllAllch      (UINT32 addr);
int    dsc2DisableTDCch        (UINT32 addr, int channel);
int    dsc2DisableTRGch        (UINT32 addr, int channel);
int    dsc2DisableAllTDCch     (UINT32 addr);
int    dsc2DisableAllTRGch     (UINT32 addr);
int    dsc2DisableAllAllch     (UINT32 addr);
/*============================================================*/
int    dsc2SetORmask           (UINT32 addr, UINT32 mask);
int    dsc2GetORmask           (UINT32 addr);
int    dsc2GetORmaskTDC        (UINT32 addr);
int    dsc2GetORmaskTRG        (UINT32 addr);
int    dsc2EnableORchTDC       (UINT32 addr, int channel);
int    dsc2EnableORchTRG       (UINT32 addr, int channel);
int    dsc2EnableORAllchTDC    (UINT32 addr);
int    dsc2EnableORAllchTRG    (UINT32 addr);
int    dsc2EnableORAll         (UINT32 addr);
int    dsc2DisableORchTDC      (UINT32 addr, int channel);
int    dsc2DisableORchTRG      (UINT32 addr, int channel);
int    dsc2DisableORAllchTDC   (UINT32 addr);
int    dsc2DisableORAllchTRG   (UINT32 addr);
int    dsc2DisableORAll        (UINT32 addr);
/*============================================================*/
int    dsc2GetScalerInputDelay (UINT32 addr);
int    dsc2GetTRGOutputDelay   (UINT32 addr);
int    dsc2SetScalerInputDelay (UINT32 addr, int delay);
int    dsc2SetTRGOutputDelay   (UINT32 addr, int delay);
/*============================================================*/
int    dsc2LatchVMEScalers     (UINT32 addr);
int    dsc2LatchGatedScalers   (UINT32 addr);
UINT32 dsc2GetTRGscaler        (UINT32 addr, int channel);
UINT32 dsc2GetTDCscaler        (UINT32 addr, int channel);
UINT32 dsc2GetTRGscalerVME     (UINT32 addr, int channel);
UINT32 dsc2GetTDCscalerVME     (UINT32 addr, int channel);
UINT32 dsc2GetREFscalerVME     (UINT32 addr);
UINT32 dsc2GetREFscalerGATE    (UINT32 addr);
/*============================================================*/
UINT32 dsc2GetValidAddr    (UINT32 addr);
int    dsc2GetNumberByAddr (UINT32 addr);
UINT32 dsc2GetAddrByNumber (int board_number);


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/


/*******************************************************************
 *  2011-05-03 - merged with Bryan's Init
 *******************************************************************/
int
dsc2Init(UINT32 addr, UINT32 addr_inc, int ndsc2)
{
  UINT32 laddr;
  int    amcode, res, ii, rdata;
  int    boardID = 0;
  UINT32 addrsave = addr;


  /* Initialise global variables */
  dsc2InitGlobals();


  /* Check for valid address */
  addr &= 0x00ffffff;
  if(addr==0)
  {
    printf("%s: ERROR: Must specify a Bus (VME-based A32/A24) address for board #0\n",
	   __FUNCTION__);
    return(ERROR);
  }
  else  amcode=0x39;  /* assume A24 Addressing */

#ifdef VXWORKS
  res = sysBusToLocalAdrs(amcode,(char *)addr,(char **)&laddr);
#else
  res = vmeBusToLocalAdrs(amcode,(char *)addr,(char **)&laddr);
#endif

  if (res != 0) 
  {
#ifdef VXWORKS
    printf("%s: ERROR in sysBusToLocalAdrs(0x%x,0x%x,&laddr) \n",
	   __FUNCTION__,amcode,addr);
#else
    printf("%s: ERROR in vmeBusToLocalAdrs(0x%x,0x%x,&laddr) \n",
	   __FUNCTION__,amcode,addr);
#endif
    return(ERROR);
  }


  /* Check for valid dcs2 quantity */
  if((addr_inc==0) || (ndsc2==0))
    ndsc2 = 1; /* assume only one DSC2 to initialize */


  Ndsc = 0;
  dsc2_NN = 0;
  for (ii=0; ii<ndsc2; ii++)
  {
    dscp[ii] = (DSC2 *) (laddr + ii*addr_inc);

    /* Check if Board exists at that address */
    /* vxMemProbe does not work in 2306, need 2400 and higher !!! */
#ifdef VXWORKS
    res = vxMemProbe((char *) &(dscp[ii]->boardID),0,2,(char *)&rdata);
#else
    res = vmeMemProbe((char *) &(dscp[ii]->boardID),2,(char *)&rdata);
#endif
    if(res < 0)
    {
      printf("%s: No addressable board at addr=0x%x\n",
	     __FUNCTION__,(UINT32) dscp[ii]);
      dscp[ii] = NULL;
      break;
    }
    else
    {
      /* Check if this is a Model DSC2 */
      boardID = vmeRead32(&dscp[ii]->boardID); 
      if(boardID != DSC2_BOARD_ID)
      {
	printf("%s: ERROR: Board ID does not match: 0x%08x , expected 0x%08x\n",
	       __FUNCTION__,boardID,DSC2_BOARD_ID);
	dscp[ii] = NULL;
	return(ERROR);
      }

      printf("%s: Found addressable board at addr=0x%x\n",
	     __FUNCTION__,(UINT32) dscp[ii]);
    }

    Ndsc++;
    dsc2_NN++;
    dsc2_BaseAddr[ii] = (UINT32) dscp[ii];
    dsc2_vmeAddr[ii] = addrsave + ii*addr_inc;
    dsc2_vmeAddr[ii] += ii*0x01000000; /*temporary until vme firmware fixed*/

	/* disable VME Bus Error, will read exact number of words */
    dsc2BusError(ii, 0);
  }


  /* Download initialised global variables to scanned DSC2 boards*/
  dsc2DownloadAll();

  return(dsc2_NN);
}

void
dsc2InitGlobals()
{
  int ii, jj;

  for(ii=0; ii<NBOARDS; ii++)
  {
    for(jj=0;jj<16;jj++)  dsc2_TDC_Threshold [ii][jj] = 100;
    for(jj=0;jj<16;jj++)  dsc2_TRG_Threshold [ii][jj] = 140;
    dsc2_TDC_PulserWidth [ii] = 20;
    dsc2_TRG_PulserWidth [ii] = 30;
    dsc2_TRG_OutputWidth [ii] = 12;
    dsc2_Channel_Mask    [ii] = 0xffffffff;
    dsc2_OR_Mask         [ii] = 0xffffffff;
    dsc2_Scaler_Delay    [ii] = 16;
    dsc2_TRG_OutputDelay [ii] = 8;
    dsc2_BaseAddr        [ii] = 0xffffff00;
    dsc2_Nconf           [ii] = -1;
  }
  dsc2_NNconf = 0;
  dsc2_NN     = 0;
}


#define DSC2_READ_MASKS(XXMASK,R_MSK,JJ_ADD) \
	else if( (strcmp(keyword,(XXMASK)) == 0) && (kk >= 0) ) \
	{ \
	  sscanf (str_tmp, "%*s %d %d %d %d %d %d %d %d \
                                %d %d %d %d %d %d %d %d", \
		  &msk[ 0], &msk[ 1], &msk[ 2], &msk[ 3], \
		  &msk[ 4], &msk[ 5], &msk[ 6], &msk[ 7], \
		  &msk[ 8], &msk[ 9], &msk[10], &msk[11], \
		  &msk[12], &msk[13], &msk[14], &msk[15]); \
	  for(jj=0; jj<16; jj++) \
	  { \
	    if( (msk[jj] < 0) || (msk[jj] > 1) ) \
	    { \
	      printf("\nReadConfigFile: Wrong mask bit value, %d\n\n",msk[jj]); return(-4); \
	    } \
	    (R_MSK) [r_Nconf[kk]] |= (msk[jj]<<(JJ_ADD)); \
	  } \
	}

int
dsc2ReadConfigFile (char *fname)
{
  FILE   *ff;
  int    ii, jj, ch, kk = -1;
  char   str_tmp[STRLEN], keyword[ROCLEN];
  char   host[ROCLEN], ROC_name[ROCLEN];
  int    nameLen, start_conf = 0;
  UINT32 addr, laddr;
  int    Tw, Gw, Ow, msk[16];
  int    del_SCL, del_TRG, ch_n, TDC_thr, TRG_thr;
  int    r_TDCthrsh [NBOARDS][16];
  int    r_TRGthrsh [NBOARDS][16];
  int    r_TDC_pw   [NBOARDS];
  int    r_TRG_pw   [NBOARDS];
  int    r_TRG_Ow   [NBOARDS];
  UINT32 r_CHmsk    [NBOARDS];
  UINT32 r_ORmsk    [NBOARDS];
  int    r_Scl_del  [NBOARDS];
  int    r_TRG_del  [NBOARDS];
  UINT32 r_bAddr    [NBOARDS];
  int    r_Nconf    [NBOARDS];
  int    r_chN      [NBOARDS];
  int    r_chn      [NBOARDS][16];


  /* Init tmp variables */
  for(ii=0; ii<NBOARDS; ii++)
  {
    for(jj=0;jj<16;jj++)  r_TDCthrsh [ii][jj] = 0;
    for(jj=0;jj<16;jj++)  r_TRGthrsh [ii][jj] = 0;
    r_TDC_pw  [ii] = 0;
    r_TRG_pw  [ii] = 0;
    r_TRG_Ow  [ii] = 0;
    r_CHmsk   [ii] = 0;
    r_ORmsk   [ii] = 0;
    r_Scl_del [ii] = 0;
    r_TRG_del [ii] = 0;
    r_bAddr   [ii] = 0;
    r_Nconf   [ii] = 0;
    r_chN     [ii] = 0;
    for(jj=0;jj<16;jj++)  r_chn [ii][jj] = 0;
  }


  /* Open config file */
  if ( (ff = fopen(fname,"r")) == NULL)
  {
    printf("\nReadConfigFile: Can't open config file\n%s\n\n",fname);           return(-2);
  }

  gethostname(host, &nameLen);
  sprintf(ROC_name, "<%s>", host);
#ifdef SP_DEBUG
  printf("\n       host=%s;    nameLen=%d \n", host, nameLen);
  printf("   ROC_name=%s; \n", ROC_name);
#endif


  /* Parsing of config file */
  while ((ch = getc(ff)) != EOF)
  {
    if     ( ch == '#' || ch == ' ' || ch == '\t' )
    {
      while (getc(ff) != '\n') {}
    }
    else if( ch == '\n' ) {}
    else
    {
      ungetc(ch,ff);
      fgets(str_tmp, STRLEN, ff);
      sscanf (str_tmp, "%s", keyword);

      if(strcmp(keyword,"CRATE") == 0)
      {
	if( (strstr(str_tmp,ROC_name) != NULL) && (kk == -1) )  start_conf = 1;
	else                                                    start_conf = 0;

#ifdef SP_DEBUG
	printf("\n    str_tmp=%s;\n", str_tmp);
	printf("    keyword=%s;\n", keyword);
	printf(" start_conf=%d;\n\n", start_conf);
#endif
      }
      else if(start_conf == 1)
      {
	if(strcmp(keyword,"BOARD") == 0)
	{
	  kk++;
	  sscanf (str_tmp, "%*s%*s %x %d %d %d", &addr, &Tw, &Gw, &Ow);

	  laddr = dsc2GetValidAddr(addr);
	  if( (r_Nconf[kk] = dsc2GetNumberByAddr(laddr)) == -1 )
	  {
	    printf("\nReadConfigFile: Wrong address, 0x%08x\n\n",addr);         return(-3);
	  }
	  r_bAddr  [r_Nconf[kk]] = laddr;
	  r_TDC_pw [r_Nconf[kk]] = Tw;
	  r_TRG_pw [r_Nconf[kk]] = Gw;
	  r_TRG_Ow [r_Nconf[kk]] = Ow;
	}

        DSC2_READ_MASKS("TDCCHMASK",r_CHmsk,(jj))
        DSC2_READ_MASKS("TRGCHMASK",r_CHmsk,(jj+16))
        DSC2_READ_MASKS("TDCORMASK",r_ORmsk,(jj))
        DSC2_READ_MASKS("TRGORMASK",r_ORmsk,(jj+16))

	else if( (strcmp(keyword,"DELAY") == 0) && (kk >= 0) )
	{
	  sscanf (str_tmp, "%*s %d %d", &del_SCL, &del_TRG);

	  r_Scl_del [r_Nconf[kk]] = del_SCL;
	  r_TRG_del [r_Nconf[kk]] = del_TRG;
	}
	else if( (strcmp(keyword,"CHANNEL") == 0) && (kk >= 0) )
	{
	  sscanf (str_tmp, "%*s %d %d %d", &ch_n, &TDC_thr, &TRG_thr);

	  if( (ch_n < 0) || (ch_n > 15) )
	  {
	    printf("\nReadConfigFile: Wrong channel number, %d\n\n",ch_n);      return(-5);
	  }
	  r_TDCthrsh [r_Nconf[kk]][ch_n] = TDC_thr;
	  r_TRGthrsh [r_Nconf[kk]][ch_n] = TRG_thr;
	  r_chn [r_Nconf[kk]][r_chN[r_Nconf[kk]]] = ch_n;
	  r_chN  [r_Nconf[kk]] ++;

#ifdef SP_DEBUG
	  printf(" 0x%08x :  read_TRG_thresh[%2d]=%4d   read_TDC_thresh[%2d]=%4d \n",
		 r_bAddr[r_Nconf[kk]],
		 ch_n,r_TRGthrsh [r_Nconf[kk]][ch_n],
		 ch_n,r_TDCthrsh [r_Nconf[kk]][ch_n]);
#endif
	}
	else
	{
	  printf("ReadConfigFile: Unknown Field or Missed Field in\n");
	  printf("   %s \n", fname);
	  printf("   str_tmp=%s", str_tmp);
	  printf("   keyword=%s \n\n", keyword);                                return(-6);
	}
      }
    }
  }
  fclose(ff);

  dsc2_NNconf = kk + 1;


  /* Prepare global variables for Download */
  for(ii=0; ii<dsc2_NNconf; ii++)
  {

    for(jj=0;jj<r_chN[r_Nconf[ii]];jj++)
    {
      dsc2_TDC_Threshold [r_Nconf[ii]] [r_chn[r_Nconf[ii]][jj]] =
	r_TDCthrsh       [r_Nconf[ii]] [r_chn[r_Nconf[ii]][jj]];
      dsc2_TRG_Threshold [r_Nconf[ii]] [r_chn[r_Nconf[ii]][jj]] =
	r_TRGthrsh       [r_Nconf[ii]] [r_chn[r_Nconf[ii]][jj]];
    }
    dsc2_TDC_PulserWidth [r_Nconf[ii]] = r_TDC_pw  [r_Nconf[ii]];
    dsc2_TRG_PulserWidth [r_Nconf[ii]] = r_TRG_pw  [r_Nconf[ii]];
    dsc2_TRG_OutputWidth [r_Nconf[ii]] = r_TRG_Ow  [r_Nconf[ii]];
    dsc2_Channel_Mask    [r_Nconf[ii]] = r_CHmsk   [r_Nconf[ii]];
    dsc2_OR_Mask         [r_Nconf[ii]] = r_ORmsk   [r_Nconf[ii]];

#ifdef SP_DEBUG
   printf(" 0x%08x : ii=%d :  dsc2_Channel_Mask[%2d]=0x%08x   r_CHmsk[%2d]=0x%08x \n",
	   r_bAddr[r_Nconf[kk]], ii,
	   r_Nconf[ii], dsc2_Channel_Mask    [r_Nconf[ii]],
	   r_Nconf[ii], r_CHmsk              [r_Nconf[ii]]);
    printf(" 0x%08x : ii=%d :  dsc2_OR_Mask[%2d]=0x%08x   r_ORmsk[%2d]=0x%08x \n",
	   r_bAddr[r_Nconf[kk]], ii,
	   r_Nconf[ii], dsc2_OR_Mask    [r_Nconf[ii]],
	   r_Nconf[ii], r_ORmsk              [r_Nconf[ii]]);
#endif

    dsc2_Scaler_Delay    [r_Nconf[ii]] = r_Scl_del [r_Nconf[ii]];
    dsc2_TRG_OutputDelay [r_Nconf[ii]] = r_TRG_del [r_Nconf[ii]];
    dsc2_BaseAddr        [r_Nconf[ii]] = r_bAddr   [r_Nconf[ii]];
    dsc2_Nconf           [ii]          = r_Nconf   [ii];
  }


  return(kk);
}

#define DSC2_DOWNLOAD(DNN,Dkk) \
  int kk, jj; \
  for(kk=0; kk<(DNN); kk++) \
  { \
    dsc2SetTDCPulserWidth   (dsc2_BaseAddr[(Dkk)], dsc2_TDC_PulserWidth[(Dkk)]); \
    dsc2SetTRGPulserWidth   (dsc2_BaseAddr[(Dkk)], dsc2_TRG_PulserWidth[(Dkk)]); \
    dsc2SetTRGOutputWidth   (dsc2_BaseAddr[(Dkk)], dsc2_TRG_OutputWidth[(Dkk)]); \
    dsc2SetTRGandTDCmask    (dsc2_BaseAddr[(Dkk)], dsc2_Channel_Mask[(Dkk)]); \
    dsc2SetORmask           (dsc2_BaseAddr[(Dkk)], dsc2_OR_Mask[(Dkk)]); \
    dsc2SetScalerInputDelay (dsc2_BaseAddr[(Dkk)], dsc2_Scaler_Delay[(Dkk)]); \
    dsc2SetTRGOutputDelay   (dsc2_BaseAddr[(Dkk)], dsc2_TRG_OutputDelay[(Dkk)]); \
    for(jj=0;jj<16;jj++) { \
      dsc2SetTDCThreshold (dsc2_BaseAddr[(Dkk)],jj,dsc2_TDC_Threshold[(Dkk)][jj]); \
      dsc2SetTRGThreshold (dsc2_BaseAddr[(Dkk)],jj,dsc2_TRG_Threshold[(Dkk)][jj]); \
    } \
  } \
  return(0)

int
dsc2DownloadAll()
{
  DSC2_DOWNLOAD(dsc2_NN,kk);
}


int
dsc2DownloadConfig()
{
  DSC2_DOWNLOAD(dsc2_NNconf,dsc2_Nconf[kk]);
}

int
dsc2SetConfig(char *fname)
{
  int res;
  if( (res = dsc2ReadConfigFile(fname)) < 0 )   return(res);
  return(dsc2DownloadConfig());
}

#define DSC2_MON_PRINT(Dk0,DNN,Dkk) \
  int kk, jj; \
  for(kk=(Dk0); kk<(DNN); kk++) \
  { \
    printf("dsc2[0x%08x]:   ID=0x%08x   Rev=%s   SN=%d   Firmware=%d.%d\n", \
	   dsc2_BaseAddr[(Dkk)], \
	   dsc2GetBoardID(dsc2_BaseAddr[(Dkk)]),  \
	   dsc2GetBoardRev(dsc2_BaseAddr[(Dkk)]), \
	   dsc2GetBoardSN(dsc2_BaseAddr[(Dkk)]),  \
	   dsc2GetFirmwareRevMaj(dsc2_BaseAddr[(Dkk)]),  \
	   dsc2GetFirmwareRevMin(dsc2_BaseAddr[(Dkk)])); \
    printf("                   Scaler_delay=%d           TRG_delay=%d\n", \
	   dsc2GetScalerInputDelay(dsc2_BaseAddr[(Dkk)]), \
	   dsc2GetTRGOutputDelay(dsc2_BaseAddr[(Dkk)])); \
    printf("                   CH_mask=0x%08x       OR_mask=0x%08x \n", \
	   dsc2GetTRGandTDCmask(dsc2_BaseAddr[(Dkk)]), \
	   dsc2GetORmask(dsc2_BaseAddr[(Dkk)])); \
    printf("                   TDC_width=%d    TRG_width=%d    TRG_OutputWidth=%d \n", \
	   dsc2GetTDCPulserWidth(dsc2_BaseAddr[(Dkk)]), \
	   dsc2GetTRGPulserWidth(dsc2_BaseAddr[(Dkk)]), \
	   dsc2GetTRGOutputWidth(dsc2_BaseAddr[(Dkk)])); \
    for(jj=0;jj<16;jj++) \
    { \
      printf("                   TDC_threshold[%2d]=%4d   TRG_threshold[%2d]=%4d \n", \
	   jj, dsc2GetTDCThreshold(dsc2_BaseAddr[(Dkk)],jj), \
	   jj, dsc2GetTRGThreshold(dsc2_BaseAddr[(Dkk)],jj)); \
    } \
    printf("\n"); \
  }

void
dsc2MonAll()
{
  DSC2_MON_PRINT(0,dsc2_NN,kk);
}

void
dsc2MonConfig()
{
  DSC2_MON_PRINT(0,dsc2_NNconf,dsc2_Nconf[kk]);
}

void
dsc2MonBoard(int board_number)
{
  DSC2_MON_PRINT(board_number,(board_number+1),kk);
}

/*============================================================*/

int
dsc2GetBoardID(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return(vmeRead32(&(dsc2->boardID)));
}

int
dsc2GetFirmwareRevMin(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return(vmeRead32(&(dsc2->firmware)) & DSC2_FIRMWARE_REV_MIN);
}

int
dsc2GetFirmwareRevMaj(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return((vmeRead32(&(dsc2->firmware)) & DSC2_FIRMWARE_REV_MAJ) >> 8);
}

/*
dsc2GetAssyInfo(0xfad10000,1)
  assumes sysClkRateGet() = 100 ticks/sec
*/
/*
char *
dsc2GetAssyInfo(UINT32 addr, int prn)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  int id, i, tmp;
  static char buf[512];

  memset(buf, 0x2A, 256);

  vmeWrite32(&dsc2->calBuf[1], 0x9F);
  vmeWrite32(&dsc2->calBuf[0], 2);
  vmeWrite32(&dsc2->calCmd,    3 | 0x30000);
  vmeWrite32(&dsc2->calExe,    0x01);
  taskDelay(sysClkRateGet());
  while(vmeRead32(&dsc2->calCmd) != 0xFFFFFFFF)
  {
    taskDelay(1);
  }

  id = vmeRead32(&dsc2->calBuf[2]) & 0xFF;
  if(prn == 1)
    printf("0x%08x : FlashId   0x%x (%d)\n",addr,id,id);


  if(id = SPIFLASH_ID_NUMONYX)
  {
	printf("SPIFLASH_ID_NUMONYX\n");
    vmeWrite32(&dsc2->calBuf[4], ((CAL_ADDR_NUMONYX+0x1F00)>>0) & 0xFF);
    vmeWrite32(&dsc2->calBuf[3], ((CAL_ADDR_NUMONYX+0x1F00)>>8) & 0xFF);
    vmeWrite32(&dsc2->calBuf[2], ((CAL_ADDR_NUMONYX+0x1F00)>>16) & 0xFF);
  }
  else if(id == SPIFLASH_ID_ATMEL)
  {
	printf("SPIFLASH_ID_ATMEL\n");
    vmeWrite32(&dsc2->calBuf[4], ((CAL_ADDR_ATMEL+0x3C00)>>0) & 0xFF);
    vmeWrite32(&dsc2->calBuf[3], ((CAL_ADDR_ATMEL+0x3C00)>>8) & 0xFF);
    vmeWrite32(&dsc2->calBuf[2], ((CAL_ADDR_ATMEL+0x3C00)>>16) & 0xFF);
  }
  vmeWrite32(&dsc2->calBuf[1], 0x03);
  vmeWrite32(&dsc2->calBuf[0], 260);
  vmeWrite32(&dsc2->calCmd,    3 | 0x30000);
  vmeWrite32(&dsc2->calExe,    0x01);
  taskDelay(sysClkRateGet());

  if(prn == 1)
    printf("000       buf : >%s< \n",buf);

  for(i=0; i<255; i++) {
    taskDelay(1);
    tmp = vmeRead32(&dsc2->calBuf[5+i]);
    printf("tmp[%d] = 0x%08x\n",i,tmp);
    buf[i] =  tmp & 0xFF;
  }

  if(prn == 1)
    printf("       buf : >%s< \n",buf);

  return(buf);
}

void
dsc2MonAllAssyInfo()
{
  int jj;
  for(jj=0; jj<dsc2_NN; jj++)
    dsc2GetAssyInfo(dsc2_BaseAddr[jj],1);
}
*/

char *
dsc2GetBoardRev(UINT32 addr)
{
  static char buf[80];

  strcpy(buf, "C");
  /*
  sscanf (dsc2GetAssyInfo(addr,0), "%*s%*s %c %*s", buf);
  */
  /*  printf(" >%s<\n",buf); */
  return(buf);
}

int
dsc2GetBoardSN(UINT32 addr)
{
  int i, SerN=0;
  UINT32 laddr[] = {0xfad10000, 0xfad20000, 0xfad30000, 0xfad40000, 
		    0xfad50000, 0xfad60000, 0xfad70000, 0xfad80000, 
		    0xfad90000, 0xfada0000, 0xfadb0000, 0xfadc0000, 
		    0xfadd0000, 0xfade0000, 0xfadf0000, 0xfae00000, 
		    0xfae10000, 0xfae20000, 0xfae30000, 0xfae40000, 
		    0xfae50000, 0xfae60000, 0xfae70000, 0xfae80000, 
		    0xfae90000, 0xfaea0000, 0xfaeb0000, 0xfaec0000 };
  int lSN[] = { 27, 19, 31, 32,  9, 22, 13, 15, 
		30, 16, 17, 42, 45, 37, 24, 34, 
		43, 47, 44, 41, 35, 36, 39, 40, 
		49, 46, 38, 48 };

  for(i=0; i<28; i++)
    if(addr == laddr[i]) {
      SerN = lSN[i];
      break;
    }

  /*
  sscanf (dsc2GetAssyInfo(addr,0), "%*s%*s%*s%*s %d %*s", &SerN);
  */
  /*  printf(" >%d<\n",SerN); */
  return(SerN);
}

/*============================================================*/

void
dsc2MonAllThreshold(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  int jj;
  for(jj=0; jj<16; jj++)
    printf("  TDC_threshold[%2d]=%4d   TRG_threshold[%2d]=%4d \n",
	   jj, dsc2GetTDCThreshold(addr, jj),
	   jj, dsc2GetTRGThreshold(addr, jj));
}

int
dsc2GetTDCThreshold(UINT32 addr, int channel)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return(vmeRead32(&(dsc2->threshold[channel])) & DSC2_TDC_THRESHOLD);
}

int
dsc2GetTRGThreshold(UINT32 addr, int channel)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return((vmeRead32(&(dsc2->threshold[channel])) & DSC2_TRG_THRESHOLD) >> 16);
}

/***************************************************************************
 * TRG and TDC thresholds can be in a range from 0 to -1023 mV
 * programmable range = from 0 to 1023 (0x3ff), step = -1 mV
 * Notes:
 *  TRG threshold should be > 25mV above TDC threshold (for same channel)
 *  to avoid introducing jitter onto timing sensitive TDC comparator
 */
int
dsc2SetTDCThreshold(UINT32 addr, int channel, int thresh) /* thresh in mV */
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  if(thresh < 1)     thresh = 1;
  if(thresh > 1023)  thresh = 1023;
  thresh |= (dsc2GetTRGThreshold(addr, channel) << 16);
  vmeWrite32(&(dsc2->threshold[channel]),thresh);
  return(thresh);
}

int
dsc2SetTRGThreshold(UINT32 addr, int channel, int thresh) /* thresh in mV */
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  if(thresh < 1)     thresh = 1;
  if(thresh > 1023)  thresh = 1023;
  thresh <<= 16;
  thresh |= dsc2GetTDCThreshold(addr, channel);
  vmeWrite32(&(dsc2->threshold[channel]),thresh);
  return(thresh);
}

int
dsc2SetUniTDCThreshold(UINT32 addr, int thresh) /* thresh in mV */
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  int jj;
  if(thresh < 1)     thresh = 1;
  if(thresh > 1023)  thresh = 1023;
  for(jj=0; jj<16; jj++)
    vmeWrite32(&(dsc2->threshold[jj]),
	       (thresh | (dsc2GetTRGThreshold(addr, jj) << 16)));
  return(0);
}

int
dsc2SetUniTRGThreshold(UINT32 addr, int thresh) /* thresh in mV */
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  int jj;
  if(thresh < 1)     thresh = 1;
  if(thresh > 1023)  thresh = 1023;
  for(jj=0; jj<16; jj++)
    vmeWrite32(&(dsc2->threshold[jj]),
	       ((thresh << 16) | dsc2GetTDCThreshold(addr, jj)));
  return(0);
}

/*============================================================*/

void
dsc2MonAllWidth(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  printf("dsc2_TDC_PulserWidth[0x%08x] = %4d dsc2_TRG_PulserWidth[0x%08x] = %4d dsc2_TRG_OutputWidth[0x%08x] = %4d \n",
	 addr, dsc2GetTDCPulserWidth(addr),
	 addr, dsc2GetTRGPulserWidth(addr),
	 addr, dsc2GetTRGOutputWidth(addr));
}

int
dsc2GetTDCPulserWidth(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return(vmeRead32(&(dsc2->width)) & DSC2_TDC_PULSER_WIDTH);
}

int
dsc2GetTRGPulserWidth(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return((vmeRead32(&(dsc2->width)) & DSC2_TRG_PULSER_WIDTH) >> 16);
}

int
dsc2GetTRGOutputWidth(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return((((vmeRead32(&(dsc2->width)) & DSC2_TRG_OUTPUT_WIDTH) >> 28)+1)*4);
}

/****************************************************************************
 * TDC Pulser Width (R/W):
 * Controls pulser width (in units ns) for all TDC channels.
 * Will be calibrated from 4ns to 40ns (1ns accuracy).
 * Values outside this range are not guaranteed to be calibrated.
 */
int
dsc2SetTDCPulserWidth  (UINT32 addr, int width)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  UINT32 w1 = (dsc2GetTRGOutputWidth(addr)/4-1) << 28;
  UINT32 w2 =  dsc2GetTRGPulserWidth(addr) << 16;
  if(width < 4)   width = 4;
  if(width > 40)  width = 40;
  width = w1 | w2 | width;
  vmeWrite32(&(dsc2->width),width);
  return(width);
}

/****************************************************************************
 * TRG Pulser Width (R/W):
 * Controls pulser width (in units ns) for all TRG channels.
 * Will be calibrated from 4ns to 40ns (1ns accuracy).
 * Values outside this range are not guaranteed to work.
 */
int
dsc2SetTRGPulserWidth  (UINT32 addr, int width)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  UINT32 w1 = (dsc2GetTRGOutputWidth(addr)/4-1) << 28;
  UINT32 w2 =  dsc2GetTDCPulserWidth(addr);
  if(width < 4)   width = 4;
  if(width > 40)  width = 40;
  width = w1 | (width << 16) | w2;
  vmeWrite32(&(dsc2->width),width);
  return(width);
}

/****************************************************************************
 * TRG Output Pulse Width (R/W):
 * Digitally delayed/pulse stretched trigger output pulse width (0-64ns):
 *    width = (TRGOutputWidth+1)*4ns
 */
int
dsc2SetTRGOutputWidth(UINT32 addr, int width)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  UINT32 w1 = dsc2GetTRGPulserWidth(addr) << 16;
  UINT32 w2 = dsc2GetTDCPulserWidth(addr);
  width = width/4 - 1;
  if(width < 0)  width = 0;
  if(width > 15) width = 15;
  width = (width << 28) | w1 | w2;
  vmeWrite32(&(dsc2->width),width);
  return(width);
}

/*============================================================*/

int
dsc2SetTRGandTDCmask(UINT32 addr, UINT32 mask)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->chMask),mask);
  return(mask);
}

int
dsc2GetTRGandTDCmask(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return(vmeRead32(&(dsc2->chMask)));
}

int
dsc2GetTDCmask(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return(vmeRead32(&(dsc2->chMask)) & DSC2_TDC_MASK);
}

int
dsc2GetTRGmask(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return((vmeRead32(&(dsc2->chMask)) & DSC2_TRG_MASK) >> 16);
}

int
dsc2EnableTDCch(UINT32 addr, int channel)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->chMask), (dsc2GetTRGandTDCmask(addr) | (1 << channel)));
  return(0);
}

int
dsc2EnableTRGch(UINT32 addr, int channel)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->chMask), (dsc2GetTRGandTDCmask(addr) | (1 << (channel+16))));
  return(0);
}

int
dsc2EnableAllTDCch(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->chMask), (dsc2GetTRGandTDCmask(addr) | DSC2_TDC_MASK));
  return(0);
}

int
dsc2EnableAllTRGch(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->chMask), (dsc2GetTRGandTDCmask(addr) | DSC2_TRG_MASK));
  return(0);
}

int
dsc2EnableAllAllch(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->chMask), (DSC2_TDC_MASK | DSC2_TRG_MASK));
  return(0);
}

int
dsc2DisableTDCch(UINT32 addr, int channel)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->chMask), (dsc2GetTRGandTDCmask(addr) & ~(1 << channel)));
  return(0);
}

int
dsc2DisableTRGch(UINT32 addr, int channel)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->chMask), (dsc2GetTRGandTDCmask(addr) & ~(1 << (channel+16))));
  return(0);
}

int
dsc2DisableAllTDCch(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->chMask), (dsc2GetTRGandTDCmask(addr) & DSC2_TRG_MASK));
  return(0);
}

int
dsc2DisableAllTRGch(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->chMask), (dsc2GetTRGandTDCmask(addr) & DSC2_TDC_MASK));
  return(0);
}

int
dsc2DisableAllAllch(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->chMask), 0);
  return(0);
}

/*============================================================*/

int
dsc2SetORmask(UINT32 addr, UINT32 mask)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->orMask), mask);
  return(mask);
}

int
dsc2GetORmask(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return(vmeRead32(&(dsc2->orMask)));
}

int
dsc2GetORmaskTDC(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return(vmeRead32(&(dsc2->orMask)) & DSC2_TDC_MASK);
}

int
dsc2GetORmaskTRG(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return((vmeRead32(&(dsc2->orMask)) & DSC2_TRG_MASK) >> 16);
}

int
dsc2EnableORchTDC(UINT32 addr, int channel)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->orMask), (dsc2GetORmask(addr) | (1 << channel)));
  return(0);
}

int
dsc2EnableORchTRG(UINT32 addr, int channel)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->orMask), (dsc2GetORmask(addr) | (1 << (channel+16))));
  return(0);
}

int
dsc2EnableORAllchTDC(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->orMask), (dsc2GetORmask(addr) | DSC2_TDC_MASK));
  return(0);
}

int
dsc2EnableORAllchTRG(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->orMask), (dsc2GetORmask(addr) | DSC2_TRG_MASK));
  return(0);
}

int
dsc2EnableORAll(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->orMask), (DSC2_TDC_MASK | DSC2_TRG_MASK));
  return(0);
}

int
dsc2DisableORchTDC(UINT32 addr, int channel)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->orMask), (dsc2GetORmask(addr) & ~(1 << channel)));
  return(0);
}

int
dsc2DisableORchTRG(UINT32 addr, int channel)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->orMask), (dsc2GetORmask(addr) & ~(1 << (channel+16))));
  return(0);
}

int
dsc2DisableORAllchTDC(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->orMask), (dsc2GetORmask(addr) & DSC2_TRG_MASK));
  return(0);
}

int
dsc2DisableORAllchTRG(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->orMask), (dsc2GetORmask(addr) & DSC2_TDC_MASK));
  return(0);
}

int
dsc2DisableORAll(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->orMask), 0);
  return(0);
}

/*============================================================*/

int
dsc2GetScalerInputDelay(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return((vmeRead32(&(dsc2->delay)) & DSC2_SCALER_INPUT_DELAY)*8);
}

int
dsc2GetTRGOutputDelay(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return(((vmeRead32(&(dsc2->delay)) & DSC2_TRG_OUTPUT_DELAY) >> 16)*4);
}

/****************************************************************************
 * ScalerDelay (R/W):
 * Scaler Input Delay 0-127 count (in 8ns ticks), so: 0-1016 ns
 */
int
dsc2SetScalerInputDelay(UINT32 addr, int delay)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  UINT32 d1 = (dsc2GetTRGOutputDelay(addr)/4) << 16;
  delay = delay/8;
  if(delay < 0)   delay = 0;
  if(delay > 127) delay = 127;
  delay = d1 | delay;
  vmeWrite32(&(dsc2->delay), delay);
  return(delay);
}

/****************************************************************************
 * TRGOutputDelay (R/W):
 * Trigger Output Delay 0-127 count (in 4ns ticks), so: 0-508 ns
 */
int
dsc2SetTRGOutputDelay(UINT32 addr, int delay)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  UINT32 d1 = dsc2GetScalerInputDelay(addr)/8;
  delay = delay/4;
  if(delay < 0)   delay = 0;
  if(delay > 127) delay = 127;
  delay = (delay << 16) | d1;
  vmeWrite32(&(dsc2->delay), delay);
  return(delay);
}

/*============================================================*/

int
dsc2LatchVMEScalers(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->vmeScalerLatch), 1);
  return(0);
}

int
dsc2LatchGatedScalers(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  vmeWrite32(&(dsc2->gatedScalerLatch), 1);
  return(0);
}

UINT32
dsc2GetTRGscaler(UINT32 addr, int channel)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return(vmeRead32(&(dsc2->trgScaler[channel])));
}

UINT32
dsc2GetTDCscaler(UINT32 addr, int channel)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return(vmeRead32(&(dsc2->tdcScaler[channel])));
}

UINT32
dsc2GetTRGscalerVME(UINT32 addr, int channel)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return(vmeRead32(&(dsc2->trgVmeScaler[channel])));
}

UINT32
dsc2GetTDCscalerVME(UINT32 addr, int channel)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return(vmeRead32(&(dsc2->tdcVmeScaler[channel])));
}

UINT32
dsc2GetREFscalerVME(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return(vmeRead32(&(dsc2->refVmeScaler)));
}

UINT32
dsc2GetREFscalerGATE(UINT32 addr)
{
  volatile DSC2 *dsc2 = (DSC2 *) addr;
  return(vmeRead32(&(dsc2->refGatedScaler)));
}

/*============================================================*/

UINT32
dsc2GetValidAddr(UINT32 addr)
{
  UINT32 laddr;
  int    amcode;

  if(dsc2GetNumberByAddr(addr) == -1)
  {
    if (addr < 0x00ffffff) amcode=0x39;  /* A24 Addressing */
    else                   amcode=0x09;  /* A32 Addressing */

#ifdef VXWORKS
    sysBusToLocalAdrs(amcode,(char *)addr,(char **)&laddr);
#else
    vmeBusToLocalAdrs(amcode,(char *)addr,(char **)&laddr);
#endif

    if(dsc2GetNumberByAddr(laddr) == -1)  return(0xffffffff);
    else                                  return(laddr);
  }
  else
    return(addr);
}

int
dsc2GetNumberByAddr(UINT32 addr)
{
  int ii, dsc2_number = -1;
  for(ii=0; ii<dsc2_NN; ii++)
    if(dsc2_BaseAddr[ii] == addr ) return(ii);
  return(dsc2_number);
}

UINT32
dsc2GetAddrByNumber(int board_number)
{
  UINT32 addr = 0xffffffff;
  if((board_number >= 0) && (board_number < dsc2_NN))
    return(dsc2_BaseAddr[board_number]);
  return(addr);
}

/*============================================================*/
/*============================================================*/
/*============================================================*/



/*******************************************************************
 *  sergey's dma
 *******************************************************************/

/* create event by moving rflag-specified scalers to FIFO */

int
dsc2SoftTrigger(UINT32 id, int rflag)
{
  int softTrigger = rflag & 0xFF;

  CHECKID(id);

  DSCLOCK;

  /* start moving event to FIFO */
  vmeWrite32(&dscp[id]->softTrigger,softTrigger);

  DSCUNLOCK;

  return(0);
}

/* sets bus error bit (0-disable, 1-enable) */
int
dsc2BusError(UINT32 id, int berr)
{
  int readoutConfig;

  CHECKID(id);

  DSCLOCK;

  readoutConfig = vmeRead32(&dscp[id]->readoutConfig);
  readoutConfig &= 0xFFFFFFFE;
  readoutConfig |= (berr&0x1);
  vmeWrite32(&dscp[id]->readoutConfig,readoutConfig);

  DSCUNLOCK;

  return(0);
}


/* variables filled by DmaStart and used by DmaDone */
static int ndata_save[DSC_MAX_BOARDS];

/*******************************************************************
 *   Function : dsc2ReadBoardDmaStart
 *                      
 *   Function : issues software trigger and Dma data from one scaler
 *              board in according to rflag
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT32 *data - local memory address to place data
 *                 UINT32 nwrds - Max number of words to transfer
 *                 UINT32 rflag - Readout flag
 *                                 bit 0 - TRG scalers
 *                                 bit 1 - TDC scalers
 *                                 bit 2 - TRG VME scalers
 *                                 bit 3 - TDC VME scalers
 *                                 bit 4 - Reference Scaler
 *                                 bit 5 - Reference VME scaler
 *                                 bit 6 - Latch VME before read
 *                                 bit 7 - Latch GATED before read
 *                                                    
 *   Returns -1 if Error, Number of words transferred if OK.
 *                                                    
 *******************************************************************/

int
dsc2ReadBoardDmaStart(UINT32 id, volatile UINT32 *data, int nwrds, int rflag)
{
  int dCnt=0, res;
  int softTrigger = rflag & 0xFF;
  volatile UINT32 *vmeAdr;

  CHECKID(id);

  if(data==NULL) 
  {
    logMsg("%s: ERROR: Invalid Destination address\n",(int)__FUNCTION__,0,0,0,0,0);
    return(ERROR);
  }
  
  /* calculate the length of event */

  dCnt ++; /* header word always there */
  if(softTrigger && DSC_READOUT_TRG)    dCnt+=16;
  if(softTrigger && DSC_READOUT_TDC)    dCnt+=16;
  if(softTrigger && DSC_READOUT_TRGVME) dCnt+=16;
  if(softTrigger && DSC_READOUT_TDCVME) dCnt+=16;
  if(softTrigger && DSC_READOUT_REF)    dCnt++;
  if(softTrigger && DSC_READOUT_REFVME) dCnt++;
  if(dCnt%2) dCnt++; /* if not aligned to 64bit, board will send us extra filler */

  /*128-bit alignment for 2eSST: must be done in VME FPGA (ask Ed) */
  /* if 2eSST (or 2eVME ?) dCnt += ... */

  if(dCnt > nwrds)
  {
    logMsg("%s: ERROR: buffer too small (%d < %d)\n",(int)__FUNCTION__,nwrds,dCnt,0,0,0);
    return(ERROR);
  }

  ndata_save[id] = dCnt;

  DSCLOCK;

  /* fifo address: use highest byte from board switches */
  vmeAdr = (UINT32 *)dsc2_vmeAddr[id];

  /*
  dCnt+=4;
  */
  /* start Dma */
  /*
  logMsg("softTrigger=0x%08x vmeAdr=0x%08x data=0x%08x nbytes=%d\n",
    softTrigger,(UINT32 *)vmeAdr,(UINT32 *)data,(dCnt<<2),5,6);
  */
  res = usrVme2MemDmaStart( (UINT32 *)vmeAdr, (UINT32 *)data, (dCnt<<2));
  if(res < 0)
  {
    logMsg("v1720ReadEventDmaStart: ERROR: usrVme2MemDmaStart returned %d\n",
           res,0,0,0,0,0);
  }

  DSCUNLOCK;

  return(dCnt);
}

int
dsc2ReadBoardDmaDone(int id)
{
  int res, mdata;

  if((res = usrVme2MemDmaDone()) < 0)
  {
    logMsg("%s: ERROR: usrVme2MemDmaDone returned = %d\n",(int)__FUNCTION__,
             res,3,4,5,6);
    return(0);
  }
  else
  {
    mdata = ndata_save[id] - (res>>2);
	/*logMsg("%s: res=%d ndata_save=%d -> mdata=%d\n",__FUNCTION__,res,ndata_save[id],mdata,5,6);*/

    if( (res>16) || ((mdata%2)!=0) )
    {
      logMsg("%s: WRONG: res=%d (ndata_save=%d => was %d), mdata=%d\n",(int)__FUNCTION__,
          res,ndata_save[id],ndata_save[id]<<2,mdata,6);
    }
	/*logMsg("%s: INFO: usrVme2MemDmaDone returned = %d (%d)\n",(int)__FUNCTION__,res,mdata,4,5,6);*/
  }

  return(mdata);
}

/*
usrVmeDmaSetConfig(2,5,1)
usrVmeDmaSetConfig(2,3,0)
dsc2dmatest(0)
taskSpawn "DSC2",255,0,100000,dsc2test1
*/

void
dsc2dmatest(int id)
{
  int ii, nw, ret, nwrds = 100;
  int data[100];

  dsc2SoftTrigger(id, 0xff);
  /*  
for(ii=0; ii<100; ii++) data[ii] = 0;
  */
  nw = dsc2ReadBoardDmaStart(id, data, nwrds, 0xff);
  ret = dsc2ReadBoardDmaDone(id);
  /*
printf("ret=%d\n",ret);
for(ii=0; ii<nw; ii++) printf("[%2d] 0x%08x\n",ii,data[ii]);
usrTempeDmaStatus(0,1);
  */
  return;
}

void
dsc2test1(int loops)
{
  int ii;

  while(loops>0)
  {
    loops --;
    for(ii=0; ii<8; ii++) dsc2dmatest(ii);
    if(!(loops%10000)) printf("loops=%d\n",loops);
    /*taskDelay(1);*/
  }

}

#endif /*vxworks */




#else /* dummy version*/

void
dsc2_dummy()
{
  return;
}

#endif


/* TODO:

DMA without BERR for exact amount of words (always align to 128 ?)

high jumpers must be different, even if low jumpers are different - need software A32 address setting

2eSST does not work if 160MB/s specified, need any higher speed

LED's are off when reading fast

TRG Output Delay - can it be set to each channel individually ?

*/
