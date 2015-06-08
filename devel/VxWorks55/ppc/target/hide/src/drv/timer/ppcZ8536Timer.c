/* ppcZ8536Timer.c - Zilog Z8536 timer library */
 
/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"
 
/*
modification history
--------------------
01m,26mar02,dtr  Removing compiler warnings.
01l,19jan99,sut  changed ZCIO_DD_OUT to ZCIO_DD_PORT_x (SPR 23111)
01k,21may98,mas  cond. compile of variables ctcs and cioCtl in sysClkErrInt().
01j,18may98,dat  fix to SPR 21232 added CACHE_PIPE_FLUSH as appr.
01i,29oct96,wlf  doc: cleanup.
01h,09sep96,tam  fixed sys) for the mv160x boards (spr #7090). 
01g,31jul96,dat  fixed warnings, docs, coding stds
01f,22jul96,tam  merged cetCvme604 and generic version (spr 6424 & 6283)
01e,04mar96,tpr  added #include "drv/timer/ppcZ8536Timer.h".
01d,11oct95,kdl	 mangen cleanup.
01c,09sep95,kvk	 added Timestamp routines, for WindView support.
01b,24apr95,kvk	 modified to work for PPC MV1600 boards.
01a,24apr95,kvk	 created from z8536Timer.c
*/
 
/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
Zilog Z8536 (ZCIO) counter/timer.  This library is capable of handling
both the system clock and auxiliary clock functions, as well as the
timestamp function.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.

The macros ZCIO_CNTRL_ADRS and ZCIO_HZ must also be defined to indicate the
control register address and clock frequency of the Z8536, respectively.
To initialize this driver, execute the routine sysZ8536Init().  This
is usually done as part of sysHwInit().

The BSP must also connect the interrupt service routine sysClkIntCIO()
to the proper interrupt vector. (No arguments to sysClkIntCIO()).  This
will normally be done as part of sysHwInit2().

The system clock is the ZCIO counter/timer 3.
The auxiliary clock is the ZCIO counter/timer 2.
The timestamp clock is the ZCIO counter/timer 1.

If the macro INCLUDE_Z8536_CLK is defined, then this source code will
generate the routines sysClkConnect(), sysClkEnable(), sysClkRateSet(),
and sysClkRateGet().

If the macro INCLUDE_Z8536_AUX_CLK is defined, then this source code will
generate the routines sysAuxClkConnect(), sysAuxClkEnable(), sysAuxClkRateSet(),
and sysAuxClkRateGet().

If the macro INCLUDE_Z8536_TIMESTAMP is defined, then the following
routines will also be created sysTimestampConnect(), sysTimestampEnable(),
sysTimestampDisable(), sysTimestampFreq(), sysTimestampPeriod(),
sysTimestamp(), and sysTimestampLock().

SEE ALSO
.I "Zilog ZCIO Counter/Timer and Parallel I/O Unit Technical Manual"
*/

/* include */

#include "vxWorks.h"
#include "config.h"
#include "drv/timer/ppcZ8536Timer.h"

/* locals */

LOCAL void sysClkErrInt ();

#ifdef INCLUDE_Z8536_CLK
LOCAL void sysClkInt ();

LOCAL FUNCPTR sysClkRoutine        = NULL;
LOCAL int     sysClkArg            = 0;
LOCAL int     sysClkTicksPerSecond = 60;
LOCAL BOOL    sysClkConnected      = FALSE;
LOCAL int     sysClkRunning        = FALSE;
#endif /* INCLUDE_Z8536_CLK */

#ifdef INCLUDE_Z8536_AUXCLK
LOCAL void sysAuxClkInt ();
int sysAuxClkRateGet ();

LOCAL FUNCPTR sysAuxClkRoutine        = NULL;
LOCAL int     sysAuxClkArg            = 0;
LOCAL int     sysAuxClkTicksPerSecond = 60;
LOCAL BOOL    sysAuxClkConnected      = FALSE;
LOCAL int     sysAuxClkRunning        = FALSE;
#endif /* INCLUDE_Z8536_AUXCLK */

#ifdef INCLUDE_Z8536_TIMESTAMP

#include "drv/timer/timestampDev.h"

LOCAL int     sysTimestampTicksPerSecond = 60; 
LOCAL BOOL    sysTimestampConnected      = FALSE; 
LOCAL BOOL    sysTimestampRunning        = FALSE; 
LOCAL FUNCPTR sysTimestampRoutine        = NULL; 
LOCAL int     sysTimestampArg            = 0;
LOCAL void sysTimestampInt ();

#endif /* INCLUDE_Z8536_TIMESTAMP */

/*
 * Workaround for PPC arch problem, SPR 21165
 * Remove when SPR is closed.
 */

#if  (CPU_FAMILY==PPC)
    IMPORT void vxEieio ();
#   undef CACHE_PIPE_FLUSH
#   define CACHE_PIPE_FLUSH() vxEieio()
#endif

#ifndef ZERO
#   define ZERO 0
#endif

/*****************************************************************************
*
* sysZ8536Init - initialize the Zilog Z8536 counter/timer
*
* This routine initializes the Zilog Z8536 ZCIO counter/timer for any and
* all of the system clock, auxiliary clock, and timestamp clock functions.
* It serves all counter/timer channels.
*
* This routine should be called before any other routine in this
* module.
*
* RETURNS: N/A.
*/
 
UINT sysZ8536Init (void)
    {
    volatile FAST UINT8 *cioCtl;
    int temp;
    unsigned int tc;			/* time constant */
     
     
    CACHE_PIPE_FLUSH();

    /* Program the z8536 Timer */

    /* CIO initialize: insure register sync */
     
    tc = 60;				/* default time constant */
    cioCtl = ZCIO_CNTRL_ADRS;
    temp = *cioCtl;			/* see CIO manual sec 2.3 */
    *cioCtl =  ZERO;
    CACHE_PIPE_FLUSH();
    temp = *cioCtl;
    
    /* reset CIO chip */
    
    *cioCtl =  ZCIO_MIC;		/* Master Interrupt Control */
    *cioCtl =  ZCIO_MIC_RESET;		/* device RESET */
    temp = CIO_RESET_DELAY;		/* Reset delay */
    do
	;				 /* wait for reset */
    while (temp --);

    *cioCtl =  ZERO;
     
    /* disable c/ts and ports before reset */
    
    *cioCtl =  ZCIO_MCC;		/* Master Configuration Cntrl */
    *cioCtl =  ZERO;
    
    *cioCtl = ZCIO_CT1CS;		/* C/T 1 Command and Status */
    *cioCtl = ZCIO_CS_CLIPIUS;		/* Clear IP and IUS */
     
    *cioCtl = ZCIO_CT2CS;		/* C/T 2 Command and Status */
    *cioCtl = ZCIO_CS_CLIPIUS;		/* Clear IP and IUS */
    
    *cioCtl = ZCIO_CT3CS;		/* C/T 3 Command and Status */
    *cioCtl = ZCIO_CS_CLIPIUS;		/* Clear IP and IUS */
     
    *cioCtl = ZCIO_CTIV;		/* C/T Interrupt Vector */
    *cioCtl = CIO_INT_VEC;		/* C/T base vector */
     
     
    /* Port A initialization */
     
    *cioCtl = ZCIO_PAMS;		/* Port A Mode Specification */
    *cioCtl = ZCIO_PMS_PTS_BIT;		/* BIT port */
    *cioCtl = ZCIO_PADPP;		/* Port A Data Path Polarity */
    *cioCtl = ZCIO_DPP_NONINV;		/* NON-INVerting */
    *cioCtl = ZCIO_PADD;		/* Port A Data Direction */
    *cioCtl = ZCIO_DD_PORT_A;		/* all output bits */
    *cioCtl = ZCIO_PASIOC;		/* Port A Special I/O Control */
    *cioCtl = ZCIO_SIO_NORMAL;		/* NORMAL input or output */
    *cioCtl = ZCIO_PACS;		/* Port A Command and Status */
    *cioCtl = ZCIO_CS_CLIPIUS;		/* Clear IP and IUS */
     
    /* Port B initialization */
     
    *cioCtl = ZCIO_PBMS;		/* Port B Mode Specification */
    *cioCtl = ZCIO_PMS_PTS_BIT;		/* BIT port */
    *cioCtl = ZCIO_PBDPP;		/* Port B Data Path Polarity */
    *cioCtl = ZCIO_DPP_NONINV;		/* NON-INVerting */
    *cioCtl = ZCIO_PBDD;		/* Port B Data Direction */
    *cioCtl = ZCIO_DD_PORT_B;		/* OUTput bits */
    *cioCtl = ZCIO_PBSIOC;		/* Port B Special I/O Control */
    *cioCtl = ZCIO_SIO_NORMAL;		/* all NORMAL input or output */
    *cioCtl = ZCIO_PBCS;		/* Port B Command and Status */
    *cioCtl = ZCIO_CS_CLIPIUS;		/* Clear IP and IUS */
     
    /* Port C initialization */
     
    *cioCtl = ZCIO_PCDPP;		/* Port C Data Path Polarity */
    *cioCtl = (ZCIO_DPP_NONINV & 0x0f);	/* all NONINVerting */
    *cioCtl = ZCIO_PCDD;		/* Port C Data Direction */
    *cioCtl = (ZCIO_DD_PORT_C & 0x0f); 	/* all OUTput bits */
    *cioCtl = ZCIO_PCSIOC;		/* Port C Special I/O Control */
    *cioCtl = (ZCIO_SIO_NORMAL & 0x0f);	/* all NORMAL input or output */

#ifdef INCLUDE_Z8536_CLK          
    
    /* counter/timer 3 initialize */
    tc      = ZCIO_HZ / sysClkTicksPerSecond;/* calculate time const */
    *cioCtl = ZCIO_CT3MS;		/* C/T 3 Mode Specification */
    *cioCtl = ZCIO_CTMS_CSC;
    
    *cioCtl = ZCIO_CT3TCMSB;		/* C/T 3 Time Constant (MSB) */
    *cioCtl = MSB(tc);
    *cioCtl = ZCIO_CT3TCLSB;		/* C/T 3 Time Constant (LSB) */
    *cioCtl = LSB(tc);
#endif
    

#ifdef INCLUDE_Z8536_AUXCLK     

    /* timer 2 initialize */
    tc      = ZCIO_HZ / sysAuxClkTicksPerSecond;/* calculate time const */
    *cioCtl =  ZCIO_CT2MS;	 	/* C/T 2 Mode Specification */
    *cioCtl = ZCIO_CTMS_CSC;
    
    *cioCtl = ZCIO_CT2TCMSB;		/* C/T 2 Time Constant (MSB) */
    *cioCtl = MSB(tc);
    *cioCtl = ZCIO_CT2TCLSB;		/* C/T 2 Time Constant (LSB) */
    *cioCtl = LSB(tc);
#endif

#ifdef INCLUDE_Z8536_TIMESTAMP               

    /* timer 1 stop */
    tc      = ZCIO_HZ / sysTimestampTicksPerSecond; /* calculate time const */
    *cioCtl = ZCIO_CT1MS;		/* C/T 1 Command and Status */
    *cioCtl = ZCIO_CTMS_CSC;

    *cioCtl = ZCIO_CT1TCMSB;		/* C/T 1 Time Constant (MSB) */
    *cioCtl = MSB(tc);
    *cioCtl = ZCIO_CT1TCLSB;		/* C/T 1 Time Constant (LSB) */
    *cioCtl = LSB(tc);
#endif
    
    /* CIO general initialization */
     
    *cioCtl = ZCIO_MCC;			/* Master Configuration Cntrl */
    CACHE_PIPE_FLUSH();
    temp = *cioCtl;            		/* resync's the z8536 pointer */
    
    *cioCtl = ZCIO_MCC;			/* Master Configuration Cntrl */
    *cioCtl = (ZCIO_MCC_PBE		/* Port B Enable */
#ifdef INCLUDE_Z8536_TIMESTAMP
	       | ZCIO_MCC_CT1E		/* Counter/Timer 1 Enable */
#endif
#ifdef INCLUDE_Z8536_AUXCLK          
	       | ZCIO_MCC_CT2E		/* Counter/Timer 2 Enable */
#endif
#ifdef INCLUDE_Z8536_CLK          
	       | ZCIO_MCC_CT3PCE	/* Counter/Timer 3 Enable */
#endif
	       | ZCIO_MCC_PAE);		/* Port A Enable */
 
    *cioCtl = ZCIO_MCC;			/* Master Configuration Cntrl */
    CACHE_PIPE_FLUSH();
    temp = *cioCtl;	
     
    *cioCtl =  ZCIO_MIC;		/* Master Interrupt Control */
    *cioCtl =  (ZCIO_MIC_MIE		/* Master Interrupt Enable */
            |  ZCIO_MIC_CTVIS);		/* C/T Vector Includes Status */ 
    
    *cioCtl =  ZCIO_MIC;		/* Master Interrupt Control */
    CACHE_PIPE_FLUSH();
    temp = *cioCtl;
    
#ifdef INCLUDE_Z8536_CLK
    sysClkRunning = FALSE;
#endif

#ifdef INCLUDE_Z8536_AUXCLK
    sysAuxClkRunning = FALSE;
#endif
     return 0;
    }

/******************************************************************************
*
* sysCioReset - reset a specified Zilog Z8536 counter/timer 
*
* This routine resets the specified Zilog Z8536 ZCIO counter/timer.
*
* RETURNS: N/A.
*/
 
void sysCioReset
     (
     volatile UINT8 *pCio		/* CIO to reset */
     )
     {
     volatile UINT8 temp;
     int delay;
 
     CACHE_PIPE_FLUSH();
     temp  = *pCio;			/* see CIO manual sec 2.3 */
     *pCio = ZERO;
     CACHE_PIPE_FLUSH();
     temp  = *pCio;
     *pCio = ZCIO_MCC;			/* master config control */
     *pCio = ZERO;
     *pCio = ZCIO_MIC;			/* master interrupt control */
     *pCio = ZCIO_MIC_RESET;		/* device RESET */
 
     for (delay = 0; delay < 1000; delay++)
 	;				/* allow reset to complete */
 
     *pCio = ZCIO_MIC;
     *pCio = ZERO;
     }

/******************************************************************************
*
* sysClkIntCIO - Zilog Z8536 interrupt handler
*
* This is the clock interrupt handler for the Zilog Z8536 ZCIO counter/timer
* driver.  It supports the system clock, auxiliary clock, and the timestamp
* clock.
*
* The BSP should connect this routine to the appropriate interrupt vector
* as a normal part of sysHwInit2().
*
* RETURNS: N/A
*
* SEE ALSO: sysHwInit2()
*/
 
void sysClkIntCIO (void)
    {
    volatile FAST UINT8 *cioCtl;
    volatile UINT8 mask;
 
    cioCtl = ZCIO_CNTRL_ADRS;

#ifdef	ZCIO_IACK_ADRS			/* special case for MVME 1600 boards */

    CACHE_PIPE_FLUSH();
    mask = (*(char *)ZCIO_IACK_ADRS);	/* read MVME-1600 IACK register */

#else	/* ZCIO_IACK_ADRS */

    *cioCtl = ZCIO_CTIV;		/* select ctr/timer intr. vector reg. */
    CACHE_PIPE_FLUSH();
    mask = *cioCtl;			/* read register */

#endif	/* ZCIO_IACK_ADRS */

    switch (mask & 0x06)		/* check which clock */
 	{
 	case ZCIO_IV_CT3:		/* system clock */
#ifdef INCLUDE_Z8536_CLK
	    *cioCtl = ZCIO_CT3CS;	/* C/T 3 Command and Status */
	    *cioCtl = ZCIO_CS_CLIPIUS 	/* Clear IP and IUS */
		    | ZCIO_CS_GCB;	/* Gate Command Bit */
	    sysClkInt ();		/* increment clock tick variable */
#endif
	    break;
 
 	case ZCIO_IV_CT2:		/* auxiliary clock */
#ifdef INCLUDE_Z8536_AUXCLK
	    *cioCtl = ZCIO_CT2CS;	/* C/T 2 Command and Status */
	    *cioCtl = ZCIO_CS_CLIPIUS	/* Clear IP and IUS */
 			 | ZCIO_CS_GCB; /* Gate Command Bit */
	    sysAuxClkInt ();		/* clock tick variable */
#endif
	    break;
 
 	case ZCIO_IV_CT1:	/* not used */
#ifdef INCLUDE_Z8536_TIMESTAMP
	    *cioCtl = ZCIO_CT1CS;	/* C/T 1 Command and Status */
	    *cioCtl = ZCIO_CS_CLIPIUS	/* Clear IP and IUS */
		    | ZCIO_CS_GCB;	/* Gate Command Bit */
	    sysTimestampInt ();		/* increment clock tick variable */
#endif
	    break;
 
 	case ZCIO_IV_CTE:		/* Interrupt Error */
	    sysClkErrInt ();		
	    break;
 	}
    }

/******************************************************************************
*
* sysClkErrInt - clock error interrupt handler
*
* An error interrupt occurs because a timer interrupt
* was not serviced before the next interrupt from the same timer
* occurred.  One or more clock ticks have been lost.
* An error interrupt is treated as a regular timer interrupt.
*
* NOTE: The system clock is the ZCIO counter/timer 3.
* The auxiliary clock is the ZCIO counter/timer 2.
* ZCIO counter/timer 1 is the timestamp counter.
*/
 
LOCAL void sysClkErrInt (void)
    {
#if (defined(INCLUDE_Z8536_CLK) || defined(INCLUDE_Z8536_AUXCLK) || \
     defined(INCLUDE_Z8536_TIMESTAMP))
    UINT8  ctcs; 			/* C/T Command and Status */
    volatile UINT8 *cioCtl = ZCIO_CNTRL_ADRS;
#endif /* (defined(INCLUDE_Z8536_CLK) || defined(INCLUDE_Z8536_AUXCLK) || \
           defined(INCLUDE_Z8536_TIMESTAMP)) */

#ifdef INCLUDE_Z8536_CLK
    /* check if counter/timer 3 error set */

    *cioCtl = ZCIO_CT3CS;
    CACHE_PIPE_FLUSH();
    ctcs    = *cioCtl;
 
    if (ctcs & (ZCIO_CS_IP | ZCIO_CS_ERR))
	{
	*cioCtl = ZCIO_CT3CS;		/* C/T 3 Command and Status */
	*cioCtl = ZCIO_CS_CLIPIUS 	/* Clear IP and IUS */
		| ZCIO_CS_GCB;		/* Gate Command Bit */
 	sysClkInt ();
	}
#endif

#ifdef INCLUDE_Z8536_AUXCLK
    /* check if counter/timer 2 error set */
 
    *cioCtl = ZCIO_CT2CS;
    CACHE_PIPE_FLUSH();
    ctcs    = *cioCtl;
 
    if (ctcs & (ZCIO_CS_IP | ZCIO_CS_ERR))
	{
	*cioCtl = ZCIO_CT2CS;		/* C/T 1 Command and Status */
	*cioCtl = ZCIO_CS_CLIPIUS 	/* Clear IP and IUS */
		| ZCIO_CS_GCB;		/* Gate Command Bit */
 	sysAuxClkInt ();
	}
#endif

#ifdef INCLUDE_Z8536_TIMESTAMP
    /* check if counter/timer 1 error set */
 
    *cioCtl = ZCIO_CT1CS;
    CACHE_PIPE_FLUSH();
    ctcs    = *cioCtl;
 
    if (ctcs & (ZCIO_CS_IP | ZCIO_CS_ERR))
	{
	*cioCtl = ZCIO_CT1CS;		/* C/T 1 Command and Status */
	*cioCtl = ZCIO_CS_CLIPIUS 	/* Clear IP and IUS */
		| ZCIO_CS_GCB;		/* Gate Command Bit */
	sysTimestampInt ();
	}
#endif
    }


#ifdef INCLUDE_Z8536_CLK
 
/******************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  Normally, it is called from usrRoot() in usrConfig.c to
* connect usrClock() to the system clock interrupt.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/
 
STATUS sysClkConnect
     (
     FUNCPTR routine,    /* routine called at each system clock interrupt */
     int arg             /* argument with which to call routine           */
     )
     {
     sysHwInit2 ();      /* XXX for now -- needs to be in usrConfig.c */
 
     sysClkRoutine	= routine;
     sysClkArg		= arg;
     sysClkConnected	= TRUE;
 
     return (OK);
     }
 
   
/******************************************************************************
*
* sysClkDisable - turn off system clock interrupts
*
* This routine disables system clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysClkEnable()
*/
 
void sysClkDisable (void)
     {
     volatile UINT8 *cioCtl;
     int lock;

     if (sysClkRunning)
         {
	 cioCtl = ZCIO_CNTRL_ADRS;
	 /* disable interrupts */
 
	 lock = intLock ();
	 *cioCtl = ZCIO_CT3CS;			/* C/T 3 Command and Status */
	 *cioCtl = ZCIO_CS_CLIE;		/* CLear Interrupt Enable */
	 intUnlock (lock);
	 
	 sysClkRunning = FALSE;
         }
     }

/*****************************************************************************
*
* sysClkEnable - turn on system clock interrupts
*
* This routine enables system clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysClkConnect(), sysClkDisable(), sysClkRateSet()
*/
 
void sysClkEnable (void)
     {
     volatile UINT8 *cioCtl;
     ULONG  tc;    /* time constant */
     int lock;
     
     if (!sysClkRunning)
	 {
 	cioCtl = ZCIO_CNTRL_ADRS;
	
 	/* set time constant */
	
 	tc = ZCIO_HZ / sysClkTicksPerSecond;

	lock = intLock ();
 	*cioCtl = ZCIO_CT3TCMSB;             /* C/T 3 Time Const (MS Byte) */
 	*cioCtl = (UINT8) MSB(tc);
 	*cioCtl = ZCIO_CT3TCLSB;             /* C/T 3 Time Const (LS Byte) */
 	*cioCtl = (UINT8) LSB(tc);
	
 	/* clear and start timer */
	
 	*cioCtl = ZCIO_CT3CS;                /* C/T 3 Command and Status */
 	*cioCtl = ZCIO_CS_CLIPIUS;           /* Clear IP and IUS */
 	*cioCtl = ZCIO_CT3CS;                /* C/T 3 Command and Status */
 	*cioCtl = ZCIO_CS_SIE                /* Set Interrupt Enable */
	    	| ZCIO_CS_GCB                /* Gate Command Bit */
 		| ZCIO_CS_TCB;               /* Trigger Command Bit */
	intUnlock (lock);
	
 	sysClkRunning = TRUE;
 	}
     }
 
/*****************************************************************************
*
* sysClkRateGet - get the system clock rate
*
* This routine returns the system clock rate.
*
* RETURNS: The number of ticks per second of the system clock.
*
* SEE ALSO: sysClkEnable(), sysClkRateSet()
*/

int sysClkRateGet (void)
     {
     return (sysClkTicksPerSecond);
     }
 
/******************************************************************************
*
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.  It is called by
* usrRoot() in usrConfig.c.
*
* NOTE: The valid range for the system clock is 38 to 10000
* ticks per second.
*
* RETURNS: OK, or ERROR if the tick rate is invalid.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/
 
STATUS sysClkRateSet
     (
     int ticksPerSecond  /* number of clock interrupts per second */
     )
     {
     if (ticksPerSecond < SYS_CLK_RATE_MIN ||
	 ticksPerSecond > SYS_CLK_RATE_MAX)
         return (ERROR);
 
     sysClkTicksPerSecond = ticksPerSecond;
 
     if (sysClkRunning)
 	{
 	sysClkDisable ();
 	sysClkEnable ();
 	}
 
     return (OK);
     }

/******************************************************************************
*
* sysClkInt - handle a system clock interrupt
*
* This routine handles a system clock interrupt.  It acknowledges the
* interrupt and calls the routine installed by sysClkConnect().
*/
 
LOCAL void sysClkInt (void)
     {
     /* interrupt acknowledged - just call other attached routine */
 
     if (sysClkRoutine != NULL)
 	(*sysClkRoutine) (sysClkArg);	/* call system clock service routine */
     }
 
#endif /* INCLUDE_Z8536_CLK */


#ifdef INCLUDE_Z8536_AUXCLK 

/******************************************************************************
*
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.  It does not enable auxiliary clock interrupts.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysAuxClkEnable()
*/
 
STATUS sysAuxClkConnect
     (
     FUNCPTR routine,    /* routine called at each aux clock interrupt */
     int arg             /* argument with which to call routine        */
     )
     {
     sysAuxClkRoutine	= routine;
     sysAuxClkArg	= arg;
     sysAuxClkConnected	= TRUE;
 
     return (OK);
     }
 
/******************************************************************************
*
* sysAuxClkInt - handle an auxiliary clock interrupt
*
* This routine handles an auxiliary clock interrupt.  It acknowledges the
* interrupt and calls the routine installed by sysAuxClkConnect().
*
* RETURNS: N/A
*/
 
LOCAL void sysAuxClkInt (void)
     {
     /* acknowledge the interrupt */
 
     /* call auxiliary clock service routine */
 
     if (sysAuxClkRoutine != NULL)
 	(*sysAuxClkRoutine) (sysAuxClkArg);
     }
 
/******************************************************************************
*
* sysAuxClkDisable - turn off auxiliary clock interrupts
*
* This routine disables auxiliary clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkEnable()
*/
 
void sysAuxClkDisable (void)
     {
     volatile UINT8 *cioCtl;
     int lock;

     if (sysAuxClkRunning)
         {
 	cioCtl = ZCIO_CNTRL_ADRS;
 	/* disable interrupts */
 
	lock = intLock ();
 	*cioCtl = ZCIO_CT2CS;			/* C/T 2 Command and Status */
 	*cioCtl = ZCIO_CS_CLIE; 		/* Clear Interrupt Enable */
	intUnlock (lock);
 
 	sysAuxClkRunning = FALSE;
         }
     }
 
/******************************************************************************
*
* sysAuxClkEnable - turn on auxiliary clock interrupts
*
* This routine enables auxiliary clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkConnect(), sysAuxClkDisable(), sysAuxClkRateSet()
*/
 
void sysAuxClkEnable (void)
     {
     volatile UINT8 *cioCtl;
     ULONG  tc;             /* time constant */
     int lock;
 
     if (!sysAuxClkRunning)
         {
 	cioCtl = ZCIO_CNTRL_ADRS;
 
 	/* set time constant */
 
 	tc = ZCIO_HZ / sysAuxClkTicksPerSecond;

	lock = intLock ();
 	*cioCtl = ZCIO_CT2TCMSB;             /* C/T 2 Time Const (MS Byte) */
	*cioCtl = (UINT8) MSB(tc);
 	*cioCtl = ZCIO_CT2TCLSB;             /* C/T 2 Time Const (LS Byte) */
	*cioCtl = (UINT8) LSB(tc);
 
 	/* clear and start timer */
 
 	*cioCtl = ZCIO_CT2CS;                /* C/T 2 Command and Status */
 	*cioCtl = ZCIO_CS_CLIPIUS;           /* Clear IP and IUS */
 	*cioCtl = ZCIO_CT2CS;                /* C/T 2 Command and Status */
 	*cioCtl = ZCIO_CS_SIE                /* Set Interrupt Enable */
 		| ZCIO_CS_GCB                /* Gate Command Bit */
 		| ZCIO_CS_TCB;               /* Trigger Command Bit */

	intUnlock (lock);
 
 	sysAuxClkRunning = TRUE;
 	}
     }
 
/******************************************************************************
*
* sysAuxClkRateGet - get the auxiliary clock rate
*
* This routine returns the interrupt rate of the auxiliary clock.
*
* RETURNS: The number of ticks per second of the auxiliary clock.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateSet()
*/
 
int sysAuxClkRateGet (void)
     {
     return (sysAuxClkTicksPerSecond);
     }
 
/******************************************************************************
*
* sysAuxClkRateSet - set the auxiliary clock rate
*
* This routine sets the interrupt rate of the auxiliary clock.  It does not
* enable auxiliary clock interrupts.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateGet()
*/
 
STATUS sysAuxClkRateSet
     (
     int ticksPerSecond  /* number of clock interrupts per second */
     )
     {
     if (ticksPerSecond < AUX_CLK_RATE_MIN ||
	 ticksPerSecond > AUX_CLK_RATE_MAX)
         return (ERROR);
 
     sysAuxClkTicksPerSecond = ticksPerSecond;
 
     if (sysAuxClkRunning)
 	{
 	sysAuxClkDisable ();
 	sysAuxClkEnable ();
 	}
 
     return (OK);
     }

#endif /* INCLUDE_Z8536_AUXCLK */
 
#ifdef INCLUDE_Z8536_TIMESTAMP


/**************************************************************************
*
* sysTimestampInt - timestamp timer interrupt handler
*
* This routine handles the timestamp timer interrupt.   A user routine is
* called, if one was connected by sysTimestampConnect().
*
* RETURNS: N/A
*
* SEE ALSO: sysTimestampConnect()
*/

LOCAL void sysTimestampInt (void)
    {
    if (sysTimestampRoutine != NULL)     /* call user-connected routine */
        (*sysTimestampRoutine) (sysTimestampArg);
    }

/**************************************************************************
*
* sysTimestampConnect - connect a user routine to a timestamp timer interrupt
*
* This routine specifies the user interrupt routine to be called at each
* timestamp timer interrupt.  
*
* RETURNS: OK, always.
*/

STATUS sysTimestampConnect
    (
    FUNCPTR routine, /* routine called at each timestamp timer interrupt */
    int arg          /* argument with which to call routine */
    )
    {
    sysTimestampConnected = TRUE; 
    sysTimestampRoutine = routine;
    sysTimestampArg = arg;
    return (OK);
    }

/**************************************************************************
*
* sysTimestampEnable - enable a timestamp timer interrupt
*
* This routine enables timestamp timer interrupts and resets the counter.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysTimestampDisable()
*/

STATUS sysTimestampEnable(void)
    {

    volatile UINT8 *cioCtl;
    ULONG  tc;             /* time constant */

    if (!sysTimestampRunning)
        {
	int oldLevel;

	cioCtl = ZCIO_CNTRL_ADRS;

	/* set time constant */

	tc = ZCIO_HZ / sysTimestampTicksPerSecond;

	oldLevel = intLock ();

	*cioCtl = ZCIO_CT1TCMSB;             /* C/T 1 Time Const (MS Byte) */
        *cioCtl = (UINT8) MSB(tc);
	*cioCtl = ZCIO_CT1TCLSB;             /* C/T 1 Time Const (LS Byte) */
        *cioCtl = (UINT8) LSB(tc);

	/* clear and start timer */
	
	*cioCtl = ZCIO_CT1CS;                /* C/T 1 Command and Status */
	*cioCtl = ZCIO_CS_CLIPIUS;           /* Clear IP and IUS */
	*cioCtl = ZCIO_CT1CS;                /* C/T 1 Command and Status */
	*cioCtl = ZCIO_CS_SIE                /* Set Interrupt Enable */
		| ZCIO_CS_GCB                /* Gate Command Bit */
		| ZCIO_CS_TCB;               /* Trigger Command Bit */
	
	intUnlock (oldLevel);

	sysTimestampRunning = TRUE;
	}

    return (OK);
    }

/**************************************************************************
*
* sysTimestampDisable - disable a timestamp timer interrupt
*
* This routine disables timestamp timer interrupts.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysTimestampEnable()
*/

STATUS sysTimestampDisable (void)
    {
    volatile UINT8 *cioCtl;

    if (sysTimestampRunning)
        {
	int oldLevel;

 	cioCtl = ZCIO_CNTRL_ADRS;

	oldLevel = intLock ();

	*cioCtl = ZCIO_CT1CS;			/* C/T 1 Command and Status */
	*cioCtl = ZCIO_CS_CLIE;

	intUnlock (oldLevel);

	sysTimestampRunning = FALSE;
        }

    return (OK);
    }

/**************************************************************************
*
* sysTimestampPeriod - get the period of a timestamp timer 
*
* This routine gets the period of the timestamp timer, in ticks.  The
* period, or terminal count, is the number of ticks to which the timestamp
* timer counts before rolling over and restarting the counting process.
*
* RETURNS: The period of the timestamp timer in counter ticks.
*/

UINT32 sysTimestampPeriod (void)
    {
    /* 
     * Return the timestamp timer period here.
     * The highest period (maximum terminal count) should be used so
     * that rollover interrupts are kept to a minimum.
     */

    return (ZCIO_HZ / sysTimestampTicksPerSecond);
    }

/**************************************************************************
*
* sysTimestampFreq - get a timestamp timer clock frequency
*
* This routine gets the frequency of the timer clock, in ticks per 
* second.  The rate of the timestamp timer is set explicitly by the 
* hardware and typically cannot be altered.
*
* RETURNS: The timestamp timer clock frequency, in ticks per second.
*/

UINT32 sysTimestampFreq (void)
    {
    UINT32 timerFreq;

    /*
     * Return the timestamp tick output frequency here.
     * This value can be determined from the following equation:
     *     timerFreq = clock input frequency / prescaler
     *
     * When possible, read the clock input frequency and prescaler values
     * directly from chip registers.
     */

    timerFreq = (ZCIO_HZ);

    return (timerFreq);
    }

/**************************************************************************
*
* sysTimestamp - get a timestamp timer tick count
*
* This routine returns the current value of the timestamp timer tick counter.
* The tick count can be converted to seconds by dividing it by the return of
* sysTimestampFreq().
*
* This routine should be called with interrupts locked.  If interrupts are
* not locked, sysTimestampLock() should be used instead.
*
* RETURNS: The current timestamp timer tick count.
*
* SEE ALSO: sysTimestampFreq(), sysTimestampLock()
*/

UINT32 sysTimestamp (void)
    {
    volatile UINT8 *cioCtl;
    volatile UINT32 countValue;
    volatile UINT8 countValueMSB;
    volatile UINT8 countValueLSB;

    /* Read the timer counter register */
    cioCtl = ZCIO_CNTRL_ADRS;

    *cioCtl	     = ZCIO_CT1CCMSB;  		/* C/T 1 Current Count MSB */
    CACHE_PIPE_FLUSH();
    countValueMSB    = *cioCtl;			/* Current Count MSB */
    
    *cioCtl	     = ZCIO_CT1CCLSB;  		/* C/T 1 Current Count LSB */
    CACHE_PIPE_FLUSH();
    countValueLSB    = *cioCtl;			/* Current Count MSB */

    countValue = (countValueMSB << 8);
    
    countValue |= (countValueLSB);   

    /* return the timestamp timer tick count here */

    return (countValue);
    }

/**************************************************************************
*
* sysTimestampLock - lock interrupts and get the timestamp timer tick count
*
* This routine locks interrupts when the tick counter must be stopped 
* in order to read it or when two independent counters must be read.  
* It then returns the current value of the timestamp timer tick
* counter.
* 
* The tick count can be converted to seconds by dividing it by the return of
* sysTimestampFreq().
*
* If interrupts are already locked, sysTimestamp() should be
* used instead.
*
* RETURNS: The current timestamp timer tick count.
*
* SEE ALSO: sysTimestampFreq(), sysTimestamp()
*/

UINT32 sysTimestampLock (void)
    {
    int lockKey;
    volatile UINT8 *cioCtl;

    volatile UINT32 countValue;
    volatile UINT8 countValueMSB;
    volatile UINT8 countValueLSB;

    lockKey = intLock ();
    
    /* Read the timer counter register */
    cioCtl = ZCIO_CNTRL_ADRS;

    *cioCtl	     = ZCIO_CT1CCMSB;  		/* C/T 1 Current Count MSB */
    CACHE_PIPE_FLUSH();
    countValueMSB    = *cioCtl;			/* Current Count MSB */
    
    *cioCtl	     = ZCIO_CT1CCLSB;  		/* C/T 1 Current Count LSB */
    CACHE_PIPE_FLUSH();
    countValueLSB    = *cioCtl;			/* Current Count MSB */

    countValue = (((countValueMSB) << 8) | countValueLSB);

    intUnlock (lockKey);
    
    /* return the timestamp timer tick count here */
    
    return (countValue);
    }

#endif   /* INCLUDE_Z8536_TIMESTAMP */
