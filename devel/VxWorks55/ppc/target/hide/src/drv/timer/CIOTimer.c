/* CIOTimer.c - Cyclone CVME964 Z8536 CIO timer routines */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,03jan97,wlf  doc: more cleanup.
01d,14jun96,wlf  doc: cleanup.
01c,17apr96,myz  lock and unlock ints at the chip level 
01b,10jan96,myz  Modified sysCIOInt, sysClkRateSet and sysAuxClkRateSet 
01a,17dec93,snc  Created as a result of partitioning/ANSIfying per vxWorks
                 porting guide
*/

/*
DESCRIPTION
This library contains routines to manipulate and use the Zilog Z85C36 CIO
which serves as the System Clock and Auxiliary Clock source on the Cyclone
CVME964.

SEE ALSO:
.iB "Cyclone CVME964 User's Manual"
.iB "Intel 80960CA User's Manual"
*/

#include "vxWorks.h"
#include "vxLib.h"
#include "sysLib.h"
#include "intLib.h"
#include "config.h"
#include "drv/timer/z8536.h"


/* forward declarations */

void	sysCioInt ();
void 	sysClkInt ();
void    sysAuxClkInt ();

/* globals */

int     sysClkTicksPerSecond    = 60;
int     sysClkRunning           = FALSE;
FUNCPTR	sysClkRoutine           = NULL;
int     sysClkArg               = NULL;
BOOL	sysClkConnected		= FALSE;
int     sysAuxClkTicksPerSecond = 60;
int     sysAuxClkRunning        = FALSE;
FUNCPTR	sysAuxClkRoutine        = NULL;
int     sysAuxClkArg            = NULL;

/*******************************************************************************
*
* sysCioInt - handle a system CIO interrupt
*
* This routine acknowledges a system CIO interrupt
* and determines which CIO subsection caused it.
*
* EXAMPLE: An interrupt results from the expiration of a timer:
*
*     CIO Timer 1 - system tick clock
*     CIO Timer 2 - system auxiliary clock
*     CIO Timer 3 - serial EEPROM accesses
*
* If the interrupt resulted from the expiration of the system clock timer
* (Timer 1), the system clock service routine is called.  If the interrupt
* is the result of an expired auxiliary clock timer (Timer 2), the system
* auxiliary clock service routine is called.
*
* NOTE: During CIO accesses that are required by various code segments,
* CIO interrupts are masked to ensure that their servicing 
* by this routine does not affect the CIO register access state machine.
*
* NOMANUAL
*/

void sysCioInt (void)
    {
    volatile UINT8 stat;
    int intHandling;

    /*
     * This do loop is needed to prevent edge triggered interrupt from lockup
     */
    do
        {
        intHandling = FALSE;

        CIO->ctrl = ZCIO_CT1CS;
        stat = CIO->ctrl;           /* get Counter/Timer1 status */

        if (stat & ZCIO_CS_IP)              /* System Clock Tick */
	    {
	    intHandling = TRUE;
            sysClkInt ();
            }

        CIO->ctrl = ZCIO_CT2CS;
        stat = CIO->ctrl;           /* get Counter/Timer2 status */

        if (stat & ZCIO_CS_IP)
	    {
	    intHandling = TRUE;
            sysAuxClkInt ();
            }

        /* check  counter/timer 3  */

        CIO->ctrl = ZCIO_CT3CS;
        stat    = CIO->ctrl;

        if (stat & ZCIO_CS_IP )
            {
            intHandling = TRUE;
            
            /* should not get a interrupt from timer 3 */
            /*  simply acknowledge the clock interrupt */

            CIO->ctrl = ZCIO_CT3CS;             /* C/T 3 Command and Status */
            CIO->ctrl = ZCIO_CS_CLIPIUS         /* Clear IP and IUS */
                        | ZCIO_CS_GCB;            /* Gate Command Bit */

            }
            
        } while(intHandling == TRUE);

    }

/*******************************************************************************
*
* sysClkInt - handle a system clock interrupt
*
* This routine handles a system clock interrupt.
*
* The system clock is driven by CIO Counter/Timer 1.
* It calls the routine installed by sysClkConnect().
*
* RETURNS: N/A
*
* SEE ALSO: sysClkConnect()
*
* NOMANUAL
*/

void sysClkInt (void)

    {
    /* reset IUS and IP, don't gate */

    CIO->ctrl = ZCIO_CT1CS;
    CIO->ctrl = ZCIO_CS_CLIPIUS | ZCIO_CS_GCB;

    if (sysClkRoutine != NULL)
	(*sysClkRoutine) (sysClkArg);
    }

/*******************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  It does not enable system clock interrupts.  Normally,
* it is called from usrRoot() in usrConfig.c to connect usrClock() to the
* system clock interrupt.  It also specifies the interrupt service routines
* for the NMI and parity interrupts and enables the parity interrupt.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/

STATUS sysClkConnect 
    (
    FUNCPTR routine,	/* routine called at each system clock interrupt */
    int arg 		/* argument with which to call routine           */
    )

    {
    /* if the system clock interrupt is being connected for the first
     * time, connected the CIO handler.
     */

    intConnect ((void *)VECTOR_CIO, sysCioInt, 0);

    sysClkRoutine   = routine;
    sysClkArg       = arg;

    sysClkConnected = TRUE;

    sysHwInit2();

    return (OK);
    }
/*******************************************************************************
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
    volatile unsigned char junk;

    if (!sysClkRunning)
	return;

    /* mask CIO interrupts to ensure that the CIO access state machine
       is not corrupted */
    
    CIO->ctrl =  ZCIO_MIC;               /* Master Interrupt Control     */
    junk      =  CIO->ctrl;
    CIO->ctrl =  ZCIO_MIC;               /* Master Interrupt Control     */
    CIO->ctrl =  (~ZCIO_MIC_MIE) & junk;   /* Master Interrupt Disable      */


    /* Disable Timer 1 interrupts from the CIO */
    junk = CIO->ctrl;		/* reset the CIO state machine */
    CIO->ctrl = ZCIO_CT1CS;
    CIO->ctrl = ZCIO_CS_CLIE;	/* clear Interrupt Enable
					   Gate Counter */

    sysClkRunning = FALSE;

    /* don't unmask the CIO interrupt if no system clocks are running */

    if (sysClkRunning || sysAuxClkRunning)
         {
         CIO->ctrl =  ZCIO_MIC;            /* Master Interrupt Control     */
         junk      =  CIO->ctrl;
         CIO->ctrl =  ZCIO_MIC;               /* Master Interrupt Control   */
         CIO->ctrl =  ZCIO_MIC_MIE | junk;   /* Master Interrupt Enable     */
         }
    }

/*******************************************************************************
*
* sysClkEnable - turn on system clock interrupts
*
* This routine enables system clock interrupts.
*
* NOTE: To calculate the time constant of the CIO Timer 1, use the following
* formula:
* .CS
* "CIO_CLK" PCLK ticks   1 timer tick     1 sec
* -------------------- x ------------ x --------------------
*        1 sec           2 PCLK ticks   sysClkTicksPerSecond
* .CE
* The result is the number of timer ticks it takes to achieve the desired
* number of ticks/sec.  This number is calculated as a long, however, the
* CIO uses a 16-bit time constant.  If the result is greater than 65535, the
* time constant is set to 65535.  A time constant of 65535 is almost 32
* milliseconds (31.47 ticks/sec.).
*
* CIO_CLK is defined in ep960jx.h.
*
* NOTE: Timers are clocked at a rate of 1/2 of PCLK.  
* 
* RETURNS: N/A
*
* SEE ALSO: sysClkConnect(), sysClkDisable(), sysClkRateSet(),
* .I "Zilog Z8536 CIO Counter/Timer Technical Manual"
*/

void sysClkEnable (void)

    {
    long num_ticks = 0;			/* number of ticks needed */
    volatile unsigned char junk;

    /* don't do it if already enabled */
    if (sysClkRunning)
	return;

    /* mask CIO interrupts to ensure that the CIO access state machine
       is not corrupted */

    CIO->ctrl =  ZCIO_MIC;               /* Master Interrupt Control     */
    junk      =  CIO->ctrl;
    CIO->ctrl =  ZCIO_MIC;               /* Master Interrupt Control     */
    CIO->ctrl =  (~ZCIO_MIC_MIE) & junk;   /* Master Interrupt Disable   */

    /* Disable Timer 1 interrupts from the CIO */
    junk = CIO->ctrl;		/* reset the CIO state machine */
    CIO->ctrl = ZCIO_CT1CS;
    CIO->ctrl = ZCIO_CS_CLIE;	/* clear Interrupt Enable
					   Gate Counter */

    /* set up the CIO Time Constant registers based on the value
       of sysClkTicksPerSecond */

    if (sysClkTicksPerSecond < SYS_CLK_RATE_MIN)
	num_ticks = 0x0ffff;

    else if (sysClkTicksPerSecond > SYS_CLK_RATE_MAX)
	num_ticks = 0x01;

    else
   	num_ticks = (long) (CIO_CLK / (2 * sysClkTicksPerSecond));

    /* Set up Counter/Timer 1 Mode */
    CIO->ctrl = ZCIO_CT1MS;
    CIO->ctrl = ZCIO_CTMS_CSC;	/* continuous cycle
				   no external signals used
				   pulse output */

    /* Load time constant into Counter/timer 1's TC registers */
    CIO->ctrl = ZCIO_CT1TCMSB;	/* Timer 1 time constant MSB */
    CIO->ctrl = (num_ticks >> 8) & 0x0ff;
    CIO->ctrl = ZCIO_CT1TCLSB;	/* Timer 1 time constant LSB */
    CIO->ctrl = num_ticks & 0x0ff;

    CIO->ctrl = ZCIO_CT1CS;
    CIO->ctrl = ZCIO_CS_CLIPIUS;	/* clear IP and IUS, gate */

    CIO->ctrl = ZCIO_MCC;
    junk = CIO->ctrl;
    CIO->ctrl = ZCIO_MCC;
    CIO->ctrl = junk | 0x40;	/* enable Timer 1 */

    CIO->ctrl = ZCIO_CT1CS;
    CIO->ctrl = ZCIO_CS_SIE |
                ZCIO_CS_GCB |
		ZCIO_CS_TCB;	/* enable int.
				   no gate
				   trigger (load) */

    sysClkRunning = TRUE;

    /* don't unmask the CIO interrupt if no system clocks are running */

    if (sysClkRunning || sysAuxClkRunning)
        {
        CIO->ctrl =  ZCIO_MIC;               /* Master Interrupt Control  */ 
        junk      =  CIO->ctrl;
        CIO->ctrl =  ZCIO_MIC;               /* Master Interrupt Control  */
        CIO->ctrl =  ZCIO_MIC_MIE | junk;   /* Master Interrupt Enable    */
        }

    }

/*******************************************************************************
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

/*******************************************************************************
*
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.
* It is called by usrRoot() in usrConfig.c.
*
* NOTE:	Since the CIO contains only a 16-bit counter, if the time constant 
* exceeds 65535, the counter uses 32 ticks/sec.  A value less than 32 ticks/sec.
* does not increase the tick period beyond 32ms.
*
* RETURNS: OK, or ERROR if the tick rate is invalid.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/

STATUS sysClkRateSet 
    (
    int ticksPerSecond 	/* number of clock interrupts per second */
    )

    {
    if (ticksPerSecond < SYS_CLK_RATE_MIN || ticksPerSecond > SYS_CLK_RATE_MAX)
        return (ERROR);

    sysClkTicksPerSecond = ticksPerSecond;
    if (sysClkRunning)
        {
	sysClkDisable ();
	sysClkEnable ();
	}
   
    return (OK);
    }

/*******************************************************************************
*
* sysAuxClkInt - handle an auxiliary clock interrupt
*
* This routine handles auxiliary clock interrupts by calling
* the routine installed by sysClkConnect().
*
* The auxiliary clock is driven by Counter/Timer 2 of the CIO.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkConnect()
*
* NOMANUAL
*/

void sysAuxClkInt (void)

    {
    /* reset IUS and IP, don't gate */

    CIO->ctrl = ZCIO_CT2CS;
    CIO->ctrl = ZCIO_CS_CLIPIUS | ZCIO_CS_GCB;
 
    /* call auxiliary clock service routine */

    if (sysAuxClkRoutine != NULL)
    	(*sysAuxClkRoutine) (sysAuxClkArg);
    }

/*******************************************************************************
*
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.  It does not enable auxiliary clock
* interrupts.
*
* RETURNS: OK, always. 
*
* SEE ALSO: intConnect(), sysAuxClkEnable(), sysAuxClkDisconnect(),
* 	    sysAuxClkInt()
*/

STATUS sysAuxClkConnect 
    (
    FUNCPTR routine,    /* routine called at each aux. clock interrupt */
    int     arg         /* argument with which to call routine         */
    )

    {
    /*
     * Assumes that the interrupt from the CIO has already been
     * connected.
     */

    sysAuxClkRoutine = routine;
    sysAuxClkArg = arg;
    return (OK);
    }

/*******************************************************************************
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
    volatile unsigned char junk;

    if (!sysAuxClkRunning)
	return;

    /* mask CIO interrupts to ensure that the CIO access state machine
	    is not corrupted */

    CIO->ctrl =  ZCIO_MIC;               /* Master Interrupt Control     */
    junk      =  CIO->ctrl;
    CIO->ctrl =  ZCIO_MIC;               /* Master Interrupt Control     */
    CIO->ctrl =  (~ZCIO_MIC_MIE) & junk;   /* Master Interrupt Disable   */

    /* Disable Timer 2 interrupts from the CIO */
    junk = CIO->ctrl;		/* reset the CIO state machine */
    CIO->ctrl = ZCIO_CT2CS;
    CIO->ctrl = ZCIO_CS_CLIE;  	/* clear Interrupt Enable
					   Gate Counter */

    sysAuxClkRunning = FALSE;

    /* don't unmask the CIO interrupt if no system clocks are running */

    if (sysClkRunning || sysAuxClkRunning)
        {
        CIO->ctrl =  ZCIO_MIC;               /* Master Interrupt Control  */
        junk      =  CIO->ctrl;
        CIO->ctrl =  ZCIO_MIC;               /* Master Interrupt Control */
        CIO->ctrl =  ZCIO_MIC_MIE | junk;   /* Master Interrupt Enable   */
        }

    }

/*******************************************************************************
*
* sysAuxClkEnable - turn on auxiliary clock interrupts
*
* This routine enables auxiliary clock interrupts.
*
* NOTE: To calculate the time constant of the CIO Timer 1, use the following
* formula:
* .CS
* "CIO_CLK" PCLK ticks   1 timer tick     1 sec
* -------------------- x ------------ x --------------------
*        1 sec           2 PCLK ticks   sysClkTicksPerSecond
* .CE
* The result is the number of timer ticks it takes to achieve the desired
* number of ticks/sec.  This number is calculated as a long integer,
* however, the CIO uses a 16-bit time constant.  If the result is greater
* than 65535, the time constant is set to 65535.  A time constant of 65535
* is almost 32 milliseconds (31.47 ticks/sec.).
*
* CIO_CLK is defined in ep960jx.h.
*
* NOTE: Timers are clocked at a rate of 1/2 of PCLK.  
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkConnect(), sysAuxClkDisable(), sysAuxClkRateSet(),
* .I "Zilog Z8536 CIO Counter/Timer Technical Manual"
*/

void sysAuxClkEnable (void)

    {
    long num_ticks = 0;			/* number of ticks needed */
    volatile unsigned char junk;

    if (sysAuxClkRunning)
	return;

    /* mask CIO interrupts to ensure that the CIO access state machine
	    is not corrupted */

    CIO->ctrl =  ZCIO_MIC;               /* Master Interrupt Control     */
    junk      =  CIO->ctrl;
    CIO->ctrl =  ZCIO_MIC;               /* Master Interrupt Control     */
    CIO->ctrl =  (~ZCIO_MIC_MIE) & junk;   /* Master Interrupt Disable   */

    /* Disable Timer 2 interrupts from the CIO */
    junk = CIO->ctrl;		/* reset the CIO state machine */
    CIO->ctrl = ZCIO_CT2CS;
    CIO->ctrl = ZCIO_CS_CLIE;	/* clear Interrupt Enable
					   Gate Counter */

    /* set up the CIO Time Constant registers based on the value
       of sysAuxClkTicksPerSecond */

    if (sysAuxClkTicksPerSecond < AUX_CLK_RATE_MIN) 
        num_ticks = 0x0ffff; 
 
    else if (sysAuxClkTicksPerSecond > AUX_CLK_RATE_MAX) 
        num_ticks = 0x01; 
 
    else 
        num_ticks = (long) (CIO_CLK / (2 * sysAuxClkTicksPerSecond));

    /* Set up Counter/Timer 2 Mode */
    CIO->ctrl = ZCIO_CT2MS;
    CIO->ctrl = ZCIO_CTMS_CSC;	/* continuous cycle
				   no external signals used
				   pulse output */

    /* Load time constant into Counter/timer 2's TC registers */
    CIO->ctrl = ZCIO_CT2TCMSB;	/* Timer 2 time constant MSB */
    CIO->ctrl = (num_ticks >> 8) & 0x0ff;
    CIO->ctrl = ZCIO_CT2TCLSB;	/* Timer 2 time constant LSB */
    CIO->ctrl = num_ticks & 0x0ff;

    CIO->ctrl = ZCIO_CT2CS;
    CIO->ctrl = ZCIO_CS_CLIPIUS;	/* clear IP and IUS, gate */

    CIO->ctrl = ZCIO_MCC;
    junk = CIO->ctrl;
    CIO->ctrl = ZCIO_MCC;
    CIO->ctrl = junk | 0x20;        /* enable Timer 2 */
 
    CIO->ctrl = ZCIO_CT2CS;
    CIO->ctrl = ZCIO_CS_SIE |
                ZCIO_CS_GCB |
	        ZCIO_CS_TCB;	/* enable int.
				   no gate
				   trigger (load) */

    sysAuxClkRunning = TRUE;

    /* don't unmask the CIO interrupt if no system clocks are running */

    if (sysClkRunning || sysAuxClkRunning)
        {
        CIO->ctrl =  ZCIO_MIC;               /* Master Interrupt Control */
        junk      =  CIO->ctrl;
        CIO->ctrl =  ZCIO_MIC;               /* Master Interrupt Control */
        CIO->ctrl =  ZCIO_MIC_MIE | junk;    /* Master Interrupt Disable */
        }

    }

/*******************************************************************************
*
* sysAuxClkRateGet - get the auxiliary clock rate
*
* This routine returns the interrupt rate of the auxiliary clock.
*
* RETURNS: Number of ticks per second of the auxiliary clock.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateSet()
*/

int sysAuxClkRateGet (void)
    {
    return (sysAuxClkTicksPerSecond);
    }

/*******************************************************************************
*
* sysAuxClkRateSet - set the auxiliary clock rate
*
* This routine sets the interrupt rate of the auxiliary clock.  It does not
* enable auxiliary clock interrupts.  
*
* RETURNS: OK, or ERROR if the tick rate is invalid.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateGet()
*/

STATUS sysAuxClkRateSet 
    (
    int ticksPerSecond  /* number of clock interrupts per second */
    )

    {
    if (ticksPerSecond < AUX_CLK_RATE_MIN || ticksPerSecond > AUX_CLK_RATE_MAX)
        return (ERROR);

    sysAuxClkTicksPerSecond = ticksPerSecond;

    if (sysAuxClkRunning)
	{
	sysAuxClkDisable ();
	sysAuxClkEnable ();
	}

    return (OK);
    }


