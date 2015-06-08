
#ifdef VXWORKS

#ifdef VXWORKSPPC


/******************************************************************************
*
*  pTickTimer.c  -  Driver library for a Tick timer found on the Motorola
*                   MVME2300,MVME2600,MVME3600 Single Board computers. The Library
*                   uses timer 1 on the Raven (PCI Bridge/MPIC) chip.
*       (Note that Tick timer 0 is used by vxWorks for the Aux clock on the MVME2300
*        series and should not be used)
*
*  Author: David Abbott 
*          Jefferson Lab Data Acquisition Group
*          January 1999
*
*  Instructions:   ld < pTickTimer.o               -  Load the driver
*                  pTickConnect(user_routine, arg) -  Connect a User specific
*                                                     routine to the timer
*                                                     interrupt.
*                  pTickPeriod(pval)               -  Set a timer interrupt
*                                                     period. (pval is in ticks
*                                                     where pTickFreq() returns
*                                                     the ticks/sec)
*                  pTickEnable()                   -  Enable the tick timer
*                  pTickDisable()                  -  Disable the tick timer
*/

#include "vxWorks.h"
#include "stdio.h"
#include "intLib.h"
#include "iv.h"
#include "logLib.h"
#include "taskLib.h"
#include "vxLib.h"


/* Common I/O synchronizing instructions */
#ifndef EIEIO_SYNC
# define EIEIO_SYNC  __asm__ volatile ("eieio;sync")
#endif  /* EIEIO_SYNC */
#ifndef EIEIO
# define EIEIO    __asm__ volatile ("eieio")
#endif  /* EIEIO */

/* Include Raven Base address and Register Definitions */
#define MPIC_BASE_ADRS  0xfc000000
#include "ravenMpic.h"


IMPORT  int     sysGetBusSpd(void);

IMPORT void   sysPciWrite32 (UINT32, UINT32);
IMPORT void   sysPciRead32 (UINT32, UINT32 *);

IMPORT	STATUS	pTickConnect (FUNCPTR routine, int arg);
IMPORT	STATUS	pTickEnable (void);
IMPORT	STATUS	pTickDisable (void);
IMPORT	UINT32	pTickFreq (void);
IMPORT	UINT32	pTickPeriod (int pval);
IMPORT	UINT32	pTick (void);

/*LOCAL*/ BOOL    pTickRunning  = FALSE;	/* running flag */
LOCAL FUNCPTR pTickRoutine  = NULL;	/* user rollover routine */
LOCAL int     pTickArg	   = NULL;	/* arg to user routine */

#define PTICK_INT_LEVEL 33               /* Define Interrupt vector/level */
#define PTICK_DEFAULT_PERIOD   0x7f2815  /* default 1 Sec (66MHz/8) */


/*******************************************************************************
*
* pTickInt - timer interrupt handler
*
* This rountine handles the timer interrupt.  A user routine is
* called, if one was connected by pTickConnect().
*
* RETURNS: N/A
*
* SEE ALSO: pTickConnect()
*/

LOCAL void pTickInt (void)
    {
      
      if (pTickRoutine != NULL)	/* call user routine */
        (*pTickRoutine) (pTickArg);

    }

/*******************************************************************************
*
* pTickConnect - connect a user routine to the timer interrupt
*
* This routine specifies the user interrupt routine to be called at each
* timer interrupt.  It does not enable the timestamp timer itself.
*
* RETURNS: OK, or ERROR if pTickInt() interrupt handler is not used.
*/

STATUS pTickConnect 
    (
    FUNCPTR routine,	/* routine called at each timer interrupt */
    int arg		/* argument with which to call routine */
    )
    {
      pTickRoutine = routine;
      pTickArg = arg;
      
      return (OK);
    }

/*******************************************************************************
*
* pTickEnable - initialize and enable the timer
*
* This routine connects the timer interrupt and initializes the
* counter registers.  If the timer is already running, this routine
* merely resets the timer counter.
*
* The frequency of the timer should be set explicitly within the BSP,
* in the sysHwInit() routine.  This routine does not intialize the timer
* rate.
*
* RETURNS: OK, or ERROR if the timer cannot be enabled.
*/

STATUS pTickEnable (void)
    {
    static BOOL beenHere = FALSE;

    if (pTickRunning)
	{
	  sysPciWrite32(MPIC_ADDR(MPIC_TIMER1_BASE_CT_REG), pTickPeriod(0));
	  EIEIO;					/* synchronize */
	  return (OK);
	}

    if (!beenHere)
	{
	intConnect (INUM_TO_IVEC (PTICK_INT_LEVEL),pTickInt, NULL);
	intEnable(PTICK_INT_LEVEL);
	beenHere = TRUE;
	}

    pTickRunning = TRUE;

    /* reset & enable the timer interrupt */

    /* disable counter */
    sysPciWrite32(MPIC_ADDR(MPIC_TIMER1_BASE_CT_REG), 0xffffffff);
    EIEIO;					/* synchronize */
   
    /* setup timer frequency register as defined by the BSP*/
    sysPciWrite32(MPIC_ADDR(MPIC_TIMER_FREQ_REG), PTICK_DEFAULT_PERIOD);
   
    /* interrupt unmasked, priority level 15, vector TIMER0_INT_VEC. */
    sysPciWrite32(MPIC_ADDR(MPIC_TIMER1_VEC_PRI_REG), 
                  (( PRIORITY_LVL7 | (PTICK_INT_LEVEL) ) & ~TIMER_INHIBIT));
   
    /* interrupt directed at processor 0 */
    sysPciWrite32(MPIC_ADDR(MPIC_TIMER1_DEST_REG), DESTINATION_CPU0);
    EIEIO;					/* synchronize */

    /* Enable Timer with currently defined Period */
    sysPciWrite32(MPIC_ADDR(MPIC_TIMER1_BASE_CT_REG), pTickPeriod(0));
    EIEIO;					/* synchronize */

    return (OK);
    }

/*******************************************************************************
*
* pTickDisable - disable the timer
*
* This routine disables the timer.  Interrupts are not disabled,
* although the tick counter will not increment after the timer
* is disabled, thus interrupts will no longer be generated.
*
* RETURNS: OK, or ERROR if the timer cannot be disabled.
*/

STATUS pTickDisable (void)
    {
      if (pTickRunning) {

	sysPciWrite32(MPIC_ADDR(MPIC_TIMER1_BASE_CT_REG), 0xffffffff);
	EIEIO;					/* synchronize */
	
	pTickRunning = FALSE;
      }
      
      return (OK);
    }

/*******************************************************************************
*
* pTickPeriod - get/set the timer period
*
* This routine returns the period of the timer in ticks.
* The period, or terminal count, is the number of ticks to which the timestamp
* timer will count before rolling over and restarting the counting process.
*
* ARGUMENTS: pval - value in ticks to set Timer period to. If pval=0 then
*                   the currently set value is returned
*
* RETURNS: The period of the timestamp timer in counter ticks.
*/

UINT32 pTickPeriod (int pval)
    {
      static UINT32 currentPeriod;
      
      if(pval) {
	if(pTickRunning) {
	  printf("pTick is Enabled !! Call pTickDisable before changing the Period.\n");
        } else { 
	  if((pval < 0) || (pval >= TIMER_INHIBIT)) {
	    printf("pTick Period 0x%x is out of range. It has been reset to 1 sec\n",pval);
	    pval = PTICK_DEFAULT_PERIOD;
	  }
	  sysPciWrite32(MPIC_ADDR(MPIC_TIMER1_BASE_CT_REG), (TIMER_INHIBIT | pval));
	  EIEIO;					     /* synchronize */
	  /* read back the register (Mask the Inhibit bit */
	  sysPciRead32(MPIC_ADDR(MPIC_TIMER1_BASE_CT_REG), &currentPeriod);
	  currentPeriod = ~TIMER_INHIBIT & currentPeriod;
	}
      }else{
	if(currentPeriod == 0) currentPeriod = PTICK_DEFAULT_PERIOD;
      }
      return (currentPeriod);
    }

/*******************************************************************************
*
* pTickFreq - get the timer clock frequency
*
* This routine returns the frequency of the timer clock, in ticks per second.
* The rate of the timer should be set explicitly within the BSP,
* in the sysHwInit() routine.
*
* RETURNS: The timer clock frequency, in ticks per second.
*/

UINT32 pTickFreq (void)
    {
      UINT32 timerFreq;
      UINT32 Bclk;
      
      if (sysGetBusSpd()==67)	/* check clock speed */
        Bclk = 66666666;
      else
	Bclk = 33333333;
      
      timerFreq = Bclk/8;
      
      return (timerFreq);
    }

/*******************************************************************************
*
* pTick - get the timestamp timer tick count
*
* This routine returns the current value of the timer tick counter.
* The tick count can be converted to seconds by dividing by the return of
* pTickFreq().
*
* RETURNS: The current timestamp timer tick count.
*
*/

UINT32 pTick (void)
    {
      UINT32 cnt;

      sysPciRead32(MPIC_ADDR(MPIC_TIMER1_CUR_CNT_REG), &cnt);
      
      return (cnt);
    }

/*******************************************************************************
*
* pTickUser - dummy user interrupt routine
*
*  This routine can be modified to add the code the User wishes to execute
*  upon recieving a tickcounter interrupt.
*
*  RETURNS: N/A
*
*/
void pTickUser (int arg)
{
  logMsg("pTick %d \n",arg,0,0,0,0,0);

}


#else


/******************************************************************************
*
*  pTickTimer.c  -  Driver library for a Tick timer found on the Motorola
*                   MVME162 and MVEM167 Single Board computers. The Library
*                   uses Tick timer 3 on the MCChip (Memory controller).
*       (Note that Tick timer 1 and 2 are used by vxWorks for the System clock and 
*        Aux clock respectively and should not be used)
*
*  Author: David Abbott 
*          Jefferson Lab Data Acquisition Group
*          December 1998
*
*  Instructions:   ld < pTickTimer.o               -  Load the driver
*                  pTickConnect(user_routine, arg) -  Connect a User specific
*                                                     routine to the timer
*                                                     interrupt.
*                  pTickPeriod(pval)               -  Set a timer interrupt
*                                                     period. (pval is in ticks
*                                                     where pTickFreq() returns
*                                                     the ticks/sec)
*                  pTickEnable()                   -  Enable the tick timer
*                  pTickDisable()                  -  Disable the tick timer
*/

#include "vxWorks.h"
#include "stdio.h"
#include "intLib.h"
#include "logLib.h"
#include "taskLib.h"
#include "vxLib.h"

#include "mcchip.h"

IMPORT	STATUS	pTickConnect (FUNCPTR routine, int arg);
IMPORT	STATUS	pTickEnable (void);
IMPORT	STATUS	pTickDisable (void);
IMPORT	UINT32	pTickFreq (void);
IMPORT	UINT32	pTickPeriod (int pval);
IMPORT	UINT32	pTick (void);

BOOL  pTickRunning  = FALSE;    /* no LOCAL, for external use (SP, 11-Sep-2002) */
/*
LOCAL BOOL    pTickRunning  = FALSE; */ /* running flag */
LOCAL FUNCPTR pTickRoutine  = NULL;     /* user rollover routine */
LOCAL int     pTickArg	    = NULL;     /* arg to user routine */

#define PTICK_INT_LEVEL 1
#define PTICK_DEFAULT_PERIOD   0xf4240  /* default 1 Sec */


/*******************************************************************************
*
* pTickInt - timer interrupt handler
*
* This rountine handles the timer interrupt.  A user routine is
* called, if one was connected by pTickConnect().
*
* RETURNS: N/A
*
* SEE ALSO: pTickConnect()
*/

LOCAL void pTickInt (void)
    {
      *MCC_T3_IRQ_CR |= T3_IRQ_CR_ICLR;	/* acknowledge timer interrupt */
      
      if (pTickRoutine != NULL)	/* call user routine */
        (*pTickRoutine) (pTickArg);

    }

/*******************************************************************************
*
* pTickConnect - connect a user routine to the timer interrupt
*
* This routine specifies the user interrupt routine to be called at each
* timer interrupt.  It does not enable the timestamp timer itself.
*
* RETURNS: OK, or ERROR if pTickInt() interrupt handler is not used.
*/

STATUS pTickConnect 
    (
    FUNCPTR routine,	/* routine called at each timer interrupt */
    int arg		/* argument with which to call routine */
    )
    {
      pTickRoutine = routine;
      pTickArg = arg;
      
      return (OK);
    }

/*******************************************************************************
*
* pTickEnable - initialize and enable the timer
*
* This routine connects the timer interrupt and initializes the
* counter registers.  If the timer is already running, this routine
* merely resets the timer counter.
*
* The rate of the timer should be set explicitly within the BSP,
* in the sysHwInit() routine.  This routine does not intialize the timer
* rate.
*
* RETURNS: OK, or ERROR if the timer cannot be enabled.
*/

STATUS pTickEnable (void)
    {
    static BOOL beenHere = FALSE;

    if (pTickRunning)
	{
	*MCC_TIMER3_CNT = 0;			/* clear the counter */
	return (OK);
	}

    if (!beenHere)
	{
	intConnect (INUM_TO_IVEC (MCC_INT_VEC_BASE + MCC_INT_TT3),
			   pTickInt, NULL);
	beenHere = TRUE;
	}

    pTickRunning = TRUE;

    /* reset & enable the timestamp timer interrupt */

    *MCC_T3_IRQ_CR = T3_IRQ_CR_IEN | T3_IRQ_CR_ICLR | PTICK_INT_LEVEL;

    *MCC_TIMER3_CMP = pTickPeriod (0);	 /* set the timer period */

    *MCC_TIMER3_CNT = 0;			     /* clear the counter */
    *MCC_TIMER3_CR = TIMER3_CR_COC | TIMER3_CR_CEN;   /* enable the timer */

    return (OK);
    }

/*******************************************************************************
*
* pTickDisable - disable the timer
*
* This routine disables the timer.  Interrupts are not disabled,
* although the tick counter will not increment after the timer
* is disabled, thus interrupts will no longer be generated.
*
* RETURNS: OK, or ERROR if the timer cannot be disabled.
*/

STATUS pTickDisable (void)
    {
      if (pTickRunning) {
	*MCC_TIMER3_CR = TIMER3_CR_DIS;		/* disable the timer */
	
	pTickRunning = FALSE;
      }
      
      return (OK);
    }

/*******************************************************************************
*
* pTickPeriod - get/set the timer period
*
* This routine returns the period of the timer in ticks.
* The period, or terminal count, is the number of ticks to which the timestamp
* timer will count before rolling over and restarting the counting process.
*
* ARGUMENTS: pval - value in ticks to set Timer period to. If pval=0 then
*                   the currently set value is returned
*
* RETURNS: The period of the timestamp timer in counter ticks.
*/

UINT32 pTickPeriod (int pval)
    {
      static UINT32 currentPeriod;
      
      if(pval) {
	if(pTickRunning) {
	  printf("pTick is Enabled !! Call pTickDisable before changing the Period.\n");
        } else { 
	  *MCC_TIMER3_CMP = pval;              /* set the timer period */
	  currentPeriod = *MCC_TIMER3_CMP;
	}
      }else{
	if(currentPeriod == 0) currentPeriod = PTICK_DEFAULT_PERIOD;
      }
      return (currentPeriod);
    }

/*******************************************************************************
*
* pTickFreq - get the timer clock frequency
*
* This routine returns the frequency of the timer clock, in ticks per second.
* The rate of the timestamp timer should be set explicitly within the BSP,
* in the sysHwInit() routine.
*
* RETURNS: The timer clock frequency, in ticks per second.
*/

UINT32 pTickFreq (void)
    {
      UINT32 timerFreq;
      UINT32 Bclk;
      
      if (*MCC_VERSION_REG & VERSION_REG_SPEED)	/* check clock speed */
        Bclk = 33000000;
      else
	Bclk = 25000000;
      
      timerFreq = Bclk/(256 - (*MCC_PRESCALE_CLK_ADJ & 0xff));
      
      return (timerFreq);
    }

/*******************************************************************************
*
* pTick - get the timestamp timer tick count
*
* This routine returns the current value of the timer tick counter.
* The tick count can be converted to seconds by dividing by the return of
* pTickFreq().
*
* RETURNS: The current timestamp timer tick count.
*
*/

UINT32 pTick (void)
    {
    return (*MCC_TIMER3_CNT);
    }

/*******************************************************************************
*
* pTickUser - dummy user interrupt routine
*
*  This routine can be modified to add the code the User wishes to execute
*  upon recieving a tickcounter interrupt.
*
*  RETURNS: N/A
*
*/
void pTickUser (int arg)
{

  logMsg("pTick %d\n",arg,0,0,0,0,0);

}

#endif

#else

pTickTimer_dummy()
{
  return;
}

#endif
