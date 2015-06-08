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

#include "vmeDSClib.h"

/* Internal (static) Register Read/Write prototypes */
static unsigned int dscRead(volatile unsigned int *addr);
static void dscWrite(volatile unsigned int *addr, unsigned int val);

/* Global variables */
int Ndsc = 0;                                     /* Number of DSCs in Crate */
volatile struct dsc_struct *dscp[DSC_MAX_BOARDS]; /* pointers to DSC memory map */
int dscID[DSC_MAX_BOARDS];                        /* array of slot numbers for DSCs */
unsigned int dscMemOffset = 0;       /* Offset between VME and Local address space */

/* Macro to check for module initialization */
#define CHECKID(id) {							\
    if(id==0) id=dscID[0];						\
    if((id<0) || (id>21) || (dscp[id] == NULL))				\
      {									\
	logMsg("%s: ERROR: DSC in slot %d is not initialized \n",	\
	       __FUNCTION__,id,3,4,5,6);			\
	return ERROR;							\
      }									\
  }

/* Mutex to guard flexio read/writes */
pthread_mutex_t   dscMutex = PTHREAD_MUTEX_INITIALIZER;
#define DSCLOCK     pthread_mutex_lock(&dscMutex);
#define DSCUNLOCK   pthread_mutex_unlock(&dscMutex);


int
dscInit(unsigned int addr, unsigned int addr_incr, int ndsc)
{
  unsigned int laddr, rval, errFlag, fwrev;
  int amcode, stat, res, rdata, ii, boardID=0;

  /* Check for valid address */
  if(addr==0)
    {
      printf("%s: ERROR: Must specify a Bus (VME-based A24/A32) address for DSC\n",
	     __FUNCTION__);
      return ERROR;
    }
  else if (addr < 0x00ffffff) /* A24 Addressing */
    amcode=0x39;
  else /* A32 Addressing */
    amcode=0x09;

  if((addr_incr==0) || (ndsc==0))
    ndsc = 1; /* assume only one DSC to initialize */

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
  dscMemOffset = laddr - addr;

  Ndsc = 0;
  for (ii=0;ii<ndsc;ii++) 
    {
      dscp[ii] = (struct dsc_struct *)(laddr + ii*addr_incr);
      
      /* Check if Board exists at that address */
#ifdef VXWORKS
      res = vxMemProbe((char *) &(dscp[ii]->firmwareRev),0,4,(char *)&rdata);
#else
      res = vmeMemProbe((char *) &(dscp[ii]->firmwareRev),4,(char **)&rdata);
#endif
      if(res < 0) 
	{
	  printf("%s: ERROR: No addressable board at addr=0x%x\n",
		 __FUNCTION__,(UINT32) dscp[ii]);
	  dscp[ii] = NULL;
	  errFlag = 1;
	  break;
	} 
      else 
	{
	  /* Check if this is a DSC */
	  boardID = dscRead(&dscp[ii]->boardID); 
	  if(boardID != DSC_BOARD_ID) 
	    {
	      printf("%s: ERROR: Board ID does not match: %d \n",
		     __FUNCTION__,boardID);
	      return(ERROR);
	    }
	}
      Ndsc++;

      fwrev = dscRead(&dscp[ii]->firmwareRev);
      printf("Initialized DSC ID %d at address 0x%08x (VME 0x%08x). Firmware 0x%x.\n",
	     ii,(UINT32) dscp[ii],(UINT32) dscp[ii] - dscMemOffset,
	     fwrev&DSC_FIRMWAREREV_MASK);
    }

  return OK;

}

/*******************************************************************
 *   Function : dscStatus
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
dscStatus(UINT32 id, int pflag)
{
  UINT32 pulsewidth, chEnable, orMask, delay, testCtrl;
  UINT32 firmwareRev, boardID;
  CHECKID(id);

  DSCLOCK;
  pulsewidth  = dscRead(&dscp[id]->pulsewidth);
  chEnable    = dscRead(&dscp[id]->chEnable);
  orMask      = dscRead(&dscp[id]->orMask);
  delay       = dscRead(&dscp[id]->delay);
  testCtrl    = dscRead(&dscp[id]->testCtrl);
  firmwareRev = dscRead(&dscp[id]->firmwareRev);
  boardID     = dscRead(&dscp[id]->boardID);
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
 *   Function : dscSetThreshold
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
dscSetThreshold(UINT32 id, UINT16 chan, UINT16 val, UINT16 type)
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
      oldval = dscRead(&dscp[id]->threshold[chan])&DSC_THRESHOLD_TRG_MASK;
      dscWrite(&dscp[id]->threshold[chan],
	       ((val)&DSC_THRESHOLD_TDC_MASK) | oldval);
      break;
    case 2:  /* TRG */
      /* Save the TDC value so it's not lost */
      oldval = dscRead(&dscp[id]->threshold[chan])&DSC_THRESHOLD_TDC_MASK;
      dscWrite(&dscp[id]->threshold[chan],
	       ((val<<16)&DSC_THRESHOLD_TRG_MASK) | oldval);
      break;
    case 0:
    default: /* Both */
      dscWrite(&dscp[id]->threshold[chan],
	       ((val)&DSC_THRESHOLD_TDC_MASK) | ((val<<16)&DSC_THRESHOLD_TRG_MASK));
    }
  DSCUNLOCK;
  
  return OK;
}

/*******************************************************************
 *   Function : dscGetThreshold
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
dscGetThreshold(UINT32 id, UINT16 chan, UINT16 type)
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
  rval = dscRead(&dscp[id]->threshold[chan]);
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
 *   Function : dscSetPulseWidth
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
dscSetPulseWidth(UINT32 id, UINT16 val, UINT16 type)
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
      oldval = dscRead(&dscp[id]->pulsewidth);
      oldval = (oldval&DSC_PULSEWIDTH_TRG_MASK) | (oldval&DSC_PULSEWIDTH_TRG_OUTPUT_MASK);
      dscWrite(&dscp[id]->pulsewidth,
	       (val) | oldval);
      break;
    case 2:  /* TRG */
    default:
      /* Save the TDC Pulse Width and TRG Output Pulse Width value so it's not lost */
      oldval = dscRead(&dscp[id]->pulsewidth);
      oldval = (oldval&DSC_PULSEWIDTH_TDC_MASK) | (oldval&DSC_PULSEWIDTH_TRG_OUTPUT_MASK);
      dscWrite(&dscp[id]->pulsewidth,
	       (val<<16) | oldval);
      break;
    case 3:  /* TRGOUTPUT */
      /* Save the TDC Pulse Width and TRG Pulse Width value so it's not lost */
      oldval = dscRead(&dscp[id]->pulsewidth);
      oldval = (oldval&DSC_PULSEWIDTH_TDC_MASK) | (oldval&DSC_PULSEWIDTH_TRG_MASK);
      dscWrite(&dscp[id]->pulsewidth,
	       (val<<28) | oldval);
    }
  DSCUNLOCK;

  return OK;
}

/*******************************************************************
 *   Function : dscGetPulseWidth
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
dscGetPulseWidth(UINT32 id, UINT16 type)
{
  UINT32 rval;
  CHECKID(id);

  if(type==0 || type>3)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }


  DSCLOCK;
  rval = dscRead(&dscp[id]->pulsewidth);
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

/*******************************************************************
 *   Function : dscEnableChannel
 *              dscDisableChannel
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
dscEnableChannel(UINT32 id, UINT16 chan, UINT16 type)
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
      dscWrite(&dscp[id]->chEnable,
	       dscRead(&dscp[id]->chEnable) | (wval));
      break;
    case 2: /* TRG */
      dscWrite(&dscp[id]->chEnable,
	       dscRead(&dscp[id]->chEnable) | (wval<<16));
      break;
    case 0: /* TDC and TRG */
    default:
      dscWrite(&dscp[id]->chEnable,
	       dscRead(&dscp[id]->chEnable) | ((wval) | (wval<<16)));
    }
  DSCUNLOCK;

  return OK;
}

int
dscDisableChannel(UINT32 id, UINT16 chan, UINT16 type)
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
      dscWrite(&dscp[id]->chEnable,
	       dscRead(&dscp[id]->chEnable) & ~(wval));
      break;
    case 2: /* TRG */
      dscWrite(&dscp[id]->chEnable,
	       dscRead(&dscp[id]->chEnable) & ~(wval<<16));
      break;
    case 0: /* TDC and TRG */
    default:
      dscWrite(&dscp[id]->chEnable,
	       dscRead(&dscp[id]->chEnable) & ~((wval) | (wval<<16)));
    }
  DSCUNLOCK;

  return OK;
}

/*******************************************************************
 *   Function : dscSetChannelMask
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
dscSetChannelMask(UINT32 id, UINT16 chMask, UINT16 type)
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
      dscWrite(&dscp[id]->chEnable,
	       dscRead(&dscp[id]->chEnable)&DSC_CHENABLE_TRG_MASK | (chMask));
      break;
    case 2: /* TRG */
      dscWrite(&dscp[id]->chEnable,
	       dscRead(&dscp[id]->chEnable)&DSC_CHENABLE_TDC_MASK | (chMask<<16));
      break;
    case 0: /* TDC and TRG */
    default:
      dscWrite(&dscp[id]->chEnable,
	       (chMask) | (chMask<<16));
    }
  DSCUNLOCK;

  return OK;
}

/*******************************************************************
 *   Function : dscGetChannelMask
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
dscGetChannelMask(UINT32 id, UINT16 type)
{
  int rval;
  CHECKID(id);

  if(type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }

  DSCLOCK;
  rval = (int)dscRead(&dscp[id]->chEnable);
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
 *   Function : dscEnableChannelOR
 *              dscDisableChannelOR
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
dscEnableChannelOR(UINT32 id, UINT16 chan, UINT16 type)
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
      dscWrite(&dscp[id]->orMask,
	       dscRead(&dscp[id]->orMask) | (wval));
      break;
    case 2: /* TRG */
      dscWrite(&dscp[id]->orMask,
	       dscRead(&dscp[id]->orMask) | (wval<<16));
      break;
    case 0: /* TDC and TRG */
    default:
      dscWrite(&dscp[id]->orMask,
	       dscRead(&dscp[id]->orMask) | ((wval) | (wval<<16)));
    }
  DSCUNLOCK;

  return OK;
}

int
dscDisableChannelOR(UINT32 id, UINT16 chan, UINT16 type)
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
      dscWrite(&dscp[id]->orMask,
	       dscRead(&dscp[id]->orMask) & ~(wval));
      break;
    case 2: /* TRG */
      dscWrite(&dscp[id]->orMask,
	       dscRead(&dscp[id]->orMask) & ~(wval<<16));
      break;
    case 0: /* TDC and TRG */
    default:
      dscWrite(&dscp[id]->orMask,
	       dscRead(&dscp[id]->orMask) & ~((wval) | (wval<<16)));
    }
  DSCUNLOCK;

  return OK;
}


/*******************************************************************
 *   Function : dscSetChannelORMask
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
dscSetChannelORMask(UINT32 id, UINT16 chMask, UINT16 type)
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
      dscWrite(&dscp[id]->orMask,
	       dscRead(&dscp[id]->orMask)&DSC_ORMASK_TRG_MASK | (chMask));
      break;
    case 2: /* TRG */
      dscWrite(&dscp[id]->orMask,
	       dscRead(&dscp[id]->orMask)&DSC_ORMASK_TDC_MASK | (chMask<<16));
      break;
    case 0: /* TDC and TRG */
    default:
      dscWrite(&dscp[id]->orMask,
	       ((chMask) | (chMask<<16)));
    }
  DSCUNLOCK;

  return OK;
}

/*******************************************************************
 *   Function : dscGetChannelORMask
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
dscGetChannelORMask(UINT32 id, UINT16 type)
{
  int rval;
  CHECKID(id);

  if(type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }

  DSCLOCK;
  rval = (int)dscRead(&dscp[id]->orMask);
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
 *   Function : dscSetDelay
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
dscSetDelay(UINT32 id, UINT16 val, UINT16 type)
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
      oldval = dscRead(&dscp[id]->delay)&DSC_DELAY_TRG_MASK;
      dscWrite(&dscp[id]->delay,
	       (val) | oldval);
      break;
    case 2:  /* TRG Output */
    default:
      /* Save the Scaler delay value so it's not lost */
      oldval = dscRead(&dscp[id]->delay)&DSC_DELAY_SCALER_MASK;
      dscWrite(&dscp[id]->delay,
	       ((val)<<16) | oldval);
      break;
    }
  DSCUNLOCK;

  return OK;
}

/*******************************************************************
 *   Function : dscGetDelay
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
dscGetDelay(UINT32 id, UINT16 type)
{
  UINT32 rval;
  CHECKID(id);

  if(type==0 || type>2)
    {
      printf("%s: ERROR: invalid type=%d\n",__FUNCTION__,type);
      return ERROR;
    }


  DSCLOCK;
  rval = dscRead(&dscp[id]->delay);
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
 *   Function : dscTest
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
dscTest(UINT32 id, UINT32 flag)
{
  CHECKID(id);  

  DSCLOCK;
  if(flag==0)
    {
      dscWrite(&dscp[id]->testCtrl,0);
    }
  else
    {
      dscWrite(&dscp[id]->testCtrl,1);
    }
  DSCUNLOCK;

  return OK;
}

/*******************************************************************
 *   Function : dscLatchScalers
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
dscLatchScalers(UINT32 id, UINT16 type)
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
      dscWrite(&dscp[id]->vmeScalerLatch,1);
//      printf("ungated latch\n");
      break;
    case 1: /* Gated scalers */
      dscWrite(&dscp[id]->gatedScalerLatch,1);
//      printf("gated latch\n");
      break;
    case 2: /* Both */
      dscWrite(&dscp[id]->vmeScalerLatch,1);
      dscWrite(&dscp[id]->gatedScalerLatch,1);
//      printf("both latch\n");
    }
  DSCUNLOCK;

  return OK;
}

/*******************************************************************
 *   Function : dscReadScalers
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
dscReadScalers(UINT32 id, volatile UINT32 *data, int nwrds, int rflag)
{
  int dCnt=0, ichan;
  CHECKID(id);

  if(data==NULL) 
    {
      logMsg("%s: ERROR: Invalid Destination address\n",__FUNCTION__,0,0,0,0,0);
      return(ERROR);
    }
  
  /* Latch, if requested */
  if(rflag & DSC_LATCH_GATED_BEFORE_READOUT)
    {
      dscLatchScalers(id,1);
    }
  if(rflag & DSC_LATCH_VME_BEFORE_READOUT)
    {
      dscLatchScalers(id,0);
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
		     __FUNCTION__,nwrds,3,4,5,6);
	      DSCUNLOCK;
	      return dCnt;
	    }
	  data[dCnt] = dscRead(&dscp[id]->triggerScaler[ichan]);
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
		     __FUNCTION__,nwrds,3,4,5,6);
	      DSCUNLOCK;
	      return dCnt;
	    }
	  data[dCnt] = dscRead(&dscp[id]->tdcScaler[ichan]);
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
		     __FUNCTION__,nwrds,3,4,5,6);
	      DSCUNLOCK;
	      return dCnt;
	    }
	  data[dCnt] = dscRead(&dscp[id]->triggerVmeScaler[ichan]);
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
		     __FUNCTION__,nwrds,3,4,5,6);
	      DSCUNLOCK;
	      return dCnt;
	    }
	  data[dCnt] = dscRead(&dscp[id]->tdcVmeScaler[ichan]);
	  dCnt++;
	}
    }
  if(rflag & DSC_READOUT_REF)
    {
      if(dCnt==nwrds)
	{
	  logMsg("%s: ERROR: More data than what was requested (nwrds = %d)",
		 __FUNCTION__,nwrds,3,4,5,6);
	      DSCUNLOCK;
	      return dCnt;
	}
      data[dCnt] = dscRead(&dscp[id]->refGatedScaler);
      dCnt++;
    }
  if(rflag & DSC_READOUT_REFVME)
    {
      if(dCnt==nwrds)
	{
	  logMsg("%s: ERROR: More data than what was requested (nwrds = %d)",
		 __FUNCTION__,nwrds,3,4,5,6);
	      DSCUNLOCK;
	      return dCnt;
	}
      data[dCnt] = dscRead(&dscp[id]->refVmeScaler);
      dCnt++;
    }

  DSCUNLOCK;
  return dCnt;

}

/*******************************************************************
 *   Function : dscPrintScalers
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
dscPrintScalers(UINT32 id, int rflag)
{
  volatile UINT32 data[4*16+3];
  int nwrds=4*16+3;
  int dCnt, iword=0, ii;
  UINT32 header=0, header_rflag=0;
  CHECKID(id);

  printf("%s: Scalers read for DSC in slot %d:\n\n",__FUNCTION__,id);
  dCnt = dscReadScalers(id,(volatile UINT32 *)&data,nwrds,rflag);
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
      printf("%s: ERROR: dscReadScalers returned ERROR\n");
      return ERROR;
    }

  return dCnt;
}

/*******************************************************************
 *   Function : dscPrintScalerRates
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
dscPrintScalerRates(UINT32 id, int rflag)
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
	     __FUNCTION__,rflag);
      return ERROR;
    }
  if(rflag==0)
    {
      printf("%s: WARN: Nothing to do with rflag=0x%x\n",
	     __FUNCTION__,rflag);
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

  dCnt = dscReadScalers(id,(volatile UINT32 *)&data,nwrds,rflag|latch_flag);

  if(dCnt!=ERROR)
    {
      printf("%s: Scaler rates read for DSC in slot %d:\n\n",__FUNCTION__,id);

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
		     __FUNCTION__);
	      return ERROR;
	    }
	  else if(refScaler==-1)
	    {
	      printf("%s: ERROR: Reference scaler count saturated\n",
		     __FUNCTION__);
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
			 __FUNCTION__);
		  return ERROR;
		}
	      else if(refGatedScaler==-1)
		{
		  printf("%s: ERROR: Reference Gated scaler count saturated\n",
			 __FUNCTION__);
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
		     __FUNCTION__);
	      return ERROR;
	    }
	  else if(refGatedScaler==-1)
	    {
	      printf("%s: ERROR: Reference Gated scaler count saturated\n",
		     __FUNCTION__);
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
      printf("%s: ERROR: dscReadScalers returned ERROR\n");
      return ERROR;
    }

  return dCnt;
}

void
dscFlashChipSelect(UINT32 id, int select)
{
	if(select)
		dscWrite(&dscp[id]->calCmd,1);
	else
		dscWrite(&dscp[id]->calCmd,2);
	dscWrite(&dscp[id]->calExe,1);
	while(dscRead(&dscp[id]->calCmd) != 0xFFFFFFFF);
}

int
dscFlashGetId(UINT32 id)
{
	return 0x20;
	dscWrite(&dscp[id]->calBuf[1],0x9F);
	dscWrite(&dscp[id]->calBuf[0],2);
	dscWrite(&dscp[id]->calCmd,3 | 0x30000);	// read flash id
	dscWrite(&dscp[id]->calExe,1);
	while(dscRead(&dscp[id]->calCmd) != 0xFFFFFFFF);

	return dscRead(&dscp[id]->calBuf[2]) & 0xFF;
}

int
dscFlashPollStatus(UINT32 id)
{
	dscWrite(&dscp[id]->calBuf[1],0x05);
	dscWrite(&dscp[id]->calBuf[0],2);
	dscWrite(&dscp[id]->calCmd,3 | 0x30000);	// read flash status
	dscWrite(&dscp[id]->calExe,1);
	while(dscRead(&dscp[id]->calCmd) != 0xFFFFFFFF);

	return dscRead(&dscp[id]->calBuf[2]) & 0xFF;
}

int
dscCalibrate(UINT32 id)
{
	dscWrite(&dscp[id]->calCmd,0xFF);	// run calibration
	dscWrite(&dscp[id]->calExe,1);
	while(dscRead(&dscp[id]->calCmd) != 0xFFFFFFFF);

	return OK;
}

int
dscCalibrateAll(void)
{
	int i;
	
	for(i = 0; i < Ndsc; i++)
	{
		printf("Calibrating unit %d...", i);
		dscCalibrate(i);
		printf("complete.\n");
	}
	return OK;
}

int
dscReloadFPGA(UINT32 id)
{
	dscWrite(&dscp[id]->calCmd,6);	// reload fpga
	dscWrite(&dscp[id]->calExe,1);
}

int
dscUpdateFirmwareAll(const char *filename)
{
	int i, result;
	
	for(i = 0; i < Ndsc; i++)
	{
		printf("Updating firmware on unit %d...", i);
		result = dscUpdateFirmware(i, filename);
		if(result != OK)
		{
			printf("failed.\n");
			return result;
		}
		else
			printf("succeeded.");
		
		printf(" Verifying.");
		result = dscVerifyFirmware(i, filename);
		if(result != OK)
		{
			printf("failed.\n");
			return result;
		}
		else
			printf("ok.\n");

		dscReloadFPGA(i);
	}
	return OK;
}

int
dscUpdateFirmware(UINT32 id, const char *filename)
{
	FILE *f;
	int i, flashId = 0;
	unsigned int addr = 0;
	unsigned char buf[528];

	CHECKID(id);

	f = fopen(filename, "rb");
	if(!f)
	{
		printf("%s: ERROR: dscUpdateFirmware invalid file %s\n", __FUNCTION__, filename);
		return ERROR;
	}

	DSCLOCK;
	
	dscFlashChipSelect(id, 0);
	flashId = dscFlashGetId(id);

	if(flashId == 0x20)			// Numonyx flash
	{
		memset(buf, 0xff, 256);
		while(fread(buf, 1, 256, f) > 0)
		{
			if(!(addr % 65536))		// sector erase
			{
				dscWrite(&dscp[id]->calBuf[1],0x06);	// write enable
				dscWrite(&dscp[id]->calBuf[0],1);
				dscWrite(&dscp[id]->calCmd,3 | 0x30000);
				dscWrite(&dscp[id]->calExe,1);
				while(dscRead(&dscp[id]->calCmd) != 0xFFFFFFFF);

				dscWrite(&dscp[id]->calCmd,3 | 0x30000);
				dscWrite(&dscp[id]->calBuf[4],(addr)     & 0xFF);
				dscWrite(&dscp[id]->calBuf[3],(addr>>8)  & 0xFF);
				dscWrite(&dscp[id]->calBuf[2],(addr>>16) & 0xFF);
				dscWrite(&dscp[id]->calBuf[1],0xD8);	// erase sector
				dscWrite(&dscp[id]->calBuf[0],4);
				dscWrite(&dscp[id]->calExe,1);
				while(dscRead(&dscp[id]->calCmd) != 0xFFFFFFFF);

				i = 0;
				while(1)
				{
					if(!(dscFlashPollStatus(id) & 0x1))	// ~10us per poll
						break;
					if(!(i % 30000))
						printf(".");
					if(i == 300000)	// max 3s sector erase time
					{
						DSCUNLOCK;
						fclose(f);
					}
					i++;
				}
			}

			dscWrite(&dscp[id]->calBuf[1],0x06);	// write enable
			dscWrite(&dscp[id]->calBuf[0],1);
			dscWrite(&dscp[id]->calCmd,3 | 0x30000);
			dscWrite(&dscp[id]->calExe,1);
			while(dscRead(&dscp[id]->calCmd) != 0xFFFFFFFF);

			dscWrite(&dscp[id]->calBuf[4],(addr)     & 0xFF);
			dscWrite(&dscp[id]->calBuf[3],(addr>>8)  & 0xFF);
			dscWrite(&dscp[id]->calBuf[2],(addr>>16) & 0xFF);
			dscWrite(&dscp[id]->calBuf[1],0x02);	// page write
			dscWrite(&dscp[id]->calBuf[0],256+4);
			dscWrite(&dscp[id]->calCmd,3 | 0x30000);
			for(i = 255; i >= 0; i--)
				dscWrite(&dscp[id]->calBuf[5+i], buf[i]);
			dscWrite(&dscp[id]->calExe,1);
			while(dscRead(&dscp[id]->calCmd) != 0xFFFFFFFF);

			i = 0;
			while(1)
			{
				if(!(dscFlashPollStatus(id) & 0x1))	// ~10us per poll
					break;
				if(i == 500)	// max 5ms page program time
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
	else if(flashId == 0x1F)	// Atmel flash
	{
	}
	else
	{
		DSCUNLOCK;
		fclose(f);
		printf("%s: ERROR: dscUpdateFirmware failed to identify flash id 0x%02X\n", __FUNCTION__, flashId);
		return ERROR;
	}
	DSCUNLOCK;
	fclose(f);
	return OK;
}

int
dscVerifyFirmware(UINT32 id, const char *filename)
{
	FILE *f;
	int i,len, flashId = 0;
	unsigned int addr = 0;
	unsigned char buf[528];

	CHECKID(id);

	f = fopen(filename, "rb");
	if(!f)
	{
		printf("%s: ERROR: dscVerifyFirmware invalid file %s\n", __FUNCTION__, filename);
		return ERROR;
	}

	DSCLOCK;
	
	dscFlashChipSelect(id, 0);
	flashId = dscFlashGetId(id);

	if(flashId == 0x20)			// Numonyx flash
	{
		dscWrite(&dscp[id]->calBuf[4],0x00);
		dscWrite(&dscp[id]->calBuf[3],0x00);
		dscWrite(&dscp[id]->calBuf[2],0x00);
		dscWrite(&dscp[id]->calBuf[1],0x03);	// read array
		dscWrite(&dscp[id]->calBuf[0],4);
		dscWrite(&dscp[id]->calCmd,3 | 0x10000);
		dscWrite(&dscp[id]->calExe,1);
		while(dscRead(&dscp[id]->calCmd) != 0xFFFFFFFF);

		while((len = fread(buf, 1, 256, f)) > 0)
		{
			dscWrite(&dscp[id]->calBuf[0],256);
			dscWrite(&dscp[id]->calCmd,3);
			dscWrite(&dscp[id]->calExe,1);
			while(dscRead(&dscp[id]->calCmd) != 0xFFFFFFFF);

			for(i = len-1; i >= 0; i--)
			{
				if(buf[i] != dscRead(&dscp[id]->calBuf[1+i]))
				{
					dscFlashChipSelect(id, 0);
					DSCUNLOCK;
					fclose(f);					
					printf("%s: ERROR: dscVerifyFirmware failed verify at addess 0x%08X[%02X,%02X]\n", __FUNCTION__, addr+i, buf[i], (int)dscRead(&dscp[id]->calBuf[1+i]));
					return ERROR;
				}
			}
			addr+=256;
		}
		dscFlashChipSelect(id, 0);
	}
	else if(flashId == 0x1F)	// Atmel flash
	{
	}
	else
	{
		DSCUNLOCK;
		fclose(f);
		printf("%s: ERROR: dscVerifyFirmware failed to identify flash id 0x%02X\n", __FUNCTION__, flashId);
		return ERROR;
	}
	DSCUNLOCK;
	fclose(f);
	return OK;
}

int
dscReadFirmware(UINT32 id, const char *filename)
{
	FILE *f;
	int i, flashId = 0;
	unsigned int addr = 0;
	unsigned char buf[528];

	CHECKID(id);

	f = fopen(filename, "wb");
	if(!f)
	{
		printf("%s: ERROR: dscReadFirmware invalid file %s\n", __FUNCTION__, filename);
		return ERROR;
	}

	DSCLOCK;
	
	dscFlashChipSelect(id, 0);
	flashId = dscFlashGetId(id);

	if(flashId == 0x20)			// Numonyx flash
	{
		dscWrite(&dscp[id]->calBuf[4],0x00);
		dscWrite(&dscp[id]->calBuf[3],0x00);
		dscWrite(&dscp[id]->calBuf[2],0x00);
		dscWrite(&dscp[id]->calBuf[1],0x03);	// read array
		dscWrite(&dscp[id]->calBuf[0],4);
		dscWrite(&dscp[id]->calCmd,3 | 0x10000);
		dscWrite(&dscp[id]->calExe,1);
		while(dscRead(&dscp[id]->calCmd) != 0xFFFFFFFF);

		while(addr < 2*1024*1024)
		{
			printf("addr 0x%08X\n", addr);

			dscWrite(&dscp[id]->calBuf[0],256);
			dscWrite(&dscp[id]->calCmd,3);
			dscWrite(&dscp[id]->calExe,1);
			while(dscRead(&dscp[id]->calCmd) != 0xFFFFFFFF);

			for(i = 255; i >= 0; i--)
				buf[i] = dscRead(&dscp[id]->calBuf[1+i]);

			addr+= 256;
			fwrite(buf, 1, 256, f);
		}
		dscFlashChipSelect(id, 0);
	}
	else if(flashId == 0x1F)	// Atmel flash
	{
	}
	else
	{
		DSCUNLOCK;
		fclose(f);
		printf("%s: ERROR: dscUpdateFirmware failed to identify flash id 0x%02X\n", __FUNCTION__, flashId);
		return ERROR;
	}
	DSCUNLOCK;
	fclose(f);
	return OK;
}

int
dscSetPulseWidthAll(UINT16 tdcVal, UINT16 trgVal, UINT16 trgoutVal)
{
	int i, j;
	for(i = 0; i < Ndsc; i++)
	{
		dscSetPulseWidth(i, tdcVal, 1);
		dscSetPulseWidth(i, trgVal, 2);
		dscSetPulseWidth(i, trgoutVal, 3);
	}
	return OK;
}

int
dscSetThresholdAll(UINT16 tdcVal, UINT16 trgVal)
{
	int i, j;
	for(i = 0; i < Ndsc; i++)
	{
		for(j = 0; j < 16; j++)
		{
			dscSetThreshold(i, j, tdcVal, 1);
			dscSetThreshold(i, j, trgVal, 2);
		}
	}
	return OK;
}

/*******************************************************************
 *   Function : dscRead
 *              dscWrite
 *                      
 *   Function : DSC Register read/write routines
 *              These routines are static (internal to library only)
 *                                                    
 *******************************************************************/

static unsigned int
dscRead(volatile unsigned int *addr)
{
  unsigned int rval;
  rval = *addr;
#ifndef VXWORKS
  rval = LSWAP(rval);
#endif
  return rval;
}

static void
dscWrite(volatile unsigned int *addr, unsigned int val)
{
#ifndef VXWORKS
  val = LSWAP(val);
#endif
  *addr = val;
  return;
}
