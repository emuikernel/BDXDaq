/* sysMv64360Init.c - Statically initialize the Mv64360 registers */

/* Copyright 2002 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01d,25sep02,cak  Added MV64360_PCICNFG_ALT case to sysMv64360RegInit routine. 
01c,18jul02,yyz  Modify to allow Timer/counter registers initialization.
01b,10jun02,scb  Modify to allow PCI configuration register initialization.
01a,10apr02,scb  Original writing.
*/

/*
DESCRIPTION
This module statically initializes the Mv64360 registers.
*/

/* includes */

#include "mv64360.h"

/* defines */

/* For internal register initialization from table */

#define INTERNAL_REG_OFFSET(x)	*(x+1)
#define INTERNAL_REG_VALUE(x)	*(x+2)

/* For PCI config register updating from table */

#define CFG_BUS(x)	   0		/* Always access from bus 0 */
#define CFG_DEV(x)	   0		/* Device number is always 0 */
#define CFG_FCN(x)    *(x+1)		/* Function number */
#define CFG_OFFSET(x) *(x+2)		/* Device number */
#define CFG_SIZE(x)        4		/* Size in bytes is always 4 */
#define CFG_VALUE(x)  *(x+3)		/* Value to program */

/* globals */

/* locals */

/*
 * This table contains many of the the static initialization values for the 
 * MV64360 system controller chip.  The registers initialized here 
 * do not directly belong to a piece of functionality that has 
 * a supporting ".c" file - e.g. interrupt controller initialization
 * has another supporting ".c" file so the related register initialization
 * does NOT appear in this table.
 * 
 */

LOCAL UINT32 mv64360InitTbl [] =

    {

    /*
     * Timer/counter 0 is used as the MV64360 AuxClock timer.
     */

    YES_INIT_MV64360_REG (TMR_CNTR0, 0)

    YES_INIT_MV64360_REG (TMR_CNTR1, 0)

    YES_INIT_MV64360_REG (TMR_CNTR2, 0)

    YES_INIT_MV64360_REG (TMR_CNTR3, 0)

    /*
     * Timer/counter Control Register
     * Bit 00 = 0  Disable timer/counter 0.
     *     01 = 0  Counter mode for timer/counter 0.
     *     02 = 0  No external trigger for timer/counter 0.
     *     03 = 0  TCTcnt asserted for one Tclk cycle for timer/counter 0.
     *  07:04 = 0  Reserved and set to 0.
     *     08 = 0  Disable timer/counter 1.
     *     09 = 0  Counter mode for timer/counter 1.
     *     10 = 0  No external trigger for timer/counter 1.
     *     11 = 0  TCTcnt asserted for one Tclk cycle for timer/counter 1.
     *  15:12 = 0  Reserved and set to 0.
     *     16 = 0  Disable timer/counter 2.
     *     17 = 0  Counter mode for timer/counter 2.
     *     18 = 0  No external trigger for timer/counter 2.
     *     19 = 0  TCTcnt asserted for one Tclk cycle for timer/counter 2.
     *  23:20 = 0  Reserved and set to 0.
     *     24 = 0  Disable timer/counter 3.
     *     25 = 0  Counter mode for timer/counter 3.
     *     26 = 0  No external trigger for timer/counter 3.
     *     27 = 0  TCTcnt asserted for one Tclk cycle for timer/counter 3.
     *  31:28 = 0  Reserved and set to 0.
     */

    YES_INIT_MV64360_REG (TMR_CNTR_CTRL, 0)

    /*
     * Timer/counter Interrupt Cause Register
     * Bit 00 = 0  Timer/counter 0 terminal count cleared.
     *     01 = 0  Timer/counter 1 terminal count cleared.
     *     02 = 0  Timer/counter 2 terminal count cleared.
     *     03 = 0  Timer/counter 3 terminal count cleared.
     *  30:04 = 0  Reserved and set to 0.
     *     31 = 0  Summary of all cause bits, read only.
     */

    YES_INIT_MV64360_REG (TMR_CNTR_INT_CAUSE, 0)

    /*
     * Timer/counter Interrupt Mask Register
     * Bit 00 = 0  Timer/counter 0 interrupt disabled.
     *     01 = 0  Timer/counter 1 interrupt disabled.
     *     02 = 0  Timer/counter 2 interrupt disabled.
     *     03 = 0  Timer/counter 3 interrupt disabled.
     *  31:04 = 0  Read only.
     */

    YES_INIT_MV64360_REG (TMR_CNTR_INT_MASK, 0)

    END_INIT_MV64360_REG
    };

/* forward declarations */

void sysMv64360RegInit (UINT32 *);

LOCAL void  sysMv64360Init (void);

/******************************************************************************
*
* sysMv64360RegInit - Initialize MV64360 registers
*
* This function statically initializes specific MV64360 registers specified
* by a table pointed at by the input parameter.
*
* RETURNS: NA
*/

void sysMv64360RegInit 
    (
    UINT32 *p		/* Pointer to initialization table */
    )
    {
    while (*p != END_INIT_MV64360_REG)
	{
	switch (*p)
	    {
	    case MV64360_INTERNAL:
		MV64360_WRITE32_PUSH (MV64360_REG_BASE, INTERNAL_REG_OFFSET(p), 
				      INTERNAL_REG_VALUE(p));
		p += MV64360_INTERNAL_INIT_SIZE;
		break;

	    case MV64360_PCICNFG:
		sysPciConfigWrite (CFG_BUS(p), CFG_DEV(p), CFG_FCN(p), 
				   CFG_OFFSET(p), CFG_SIZE(p), CFG_VALUE(p));

		p += MV64360_PCICNFG_INIT_SIZE;
		break;

	    case MV64360_PCICNFG_ALT:
		sysPciConfigWriteAlt (CFG_BUS(p), CFG_DEV(p), CFG_FCN(p), 
				      CFG_OFFSET(p), CFG_SIZE(p), CFG_VALUE(p));

		p += MV64360_PCICNFG_INIT_SIZE;
		break;

            default:
		break;	/* Should never get here, if so we're pooched!  */
	    }	
	}
    }

/******************************************************************************
*
* sysMv64360RegInit - Initialize MV64360 registers
*
* This function initializes the MV64360 registers which can be statically
* initialized.
*
* RETURNS: NA
*/

LOCAL void sysMv64360Init (void)
    {
    UINT32 	*p = mv64360InitTbl;
    sysMv64360RegInit (p);
    }
