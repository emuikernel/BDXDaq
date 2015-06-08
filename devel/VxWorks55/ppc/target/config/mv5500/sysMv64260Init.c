/* sysMv64260Init.c - Statically initialize the Mv64260 registers */

/* Copyright 2002 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01a,01oct02,scb Copied from hxeb100 base (ver 01d).
*/

/*
DESCRIPTION
This module statically initializes the Mv64260 registers.
*/

/* includes */

#include "mv64260.h"

/* defines */

/* For internal register initialization from table */

#define INTERNAL_REG_OFFSET(x)	*(x+1)
#define INTERNAL_REG_VALUE(x)	*(x+2)

/* globals */

/* locals */

/*
 * This table contains many of the the static initialization values for the 
 * MV64260 system controller chip.  The registers initialized here 
 * do not directly belong to a piece of functionality that has 
 * a supporting ".c" file - e.g. interrupt controller initialization
 * has another supporting ".c" file so the related register initialization
 * does NOT appear in this table.
 * 
 */

LOCAL UINT32 mv64260InitTbl [] =

    {

    /*
     * Timer/counter 0 is used as the MV64260 AuxClock timer.
     */

    YES_INIT_MV64260_REG (TMR_CNTR0, 0)

    YES_INIT_MV64260_REG (TMR_CNTR1, 0)

    YES_INIT_MV64260_REG (TMR_CNTR2, 0)

    YES_INIT_MV64260_REG (TMR_CNTR3, 0)

    /*
     * Timer/counter 0-3 Control Register
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

    YES_INIT_MV64260_REG (TMR_CNTR_CTRL_0_3, 0)

    /*
     * Timer/counter 0-3 Interrupt Cause Register
     * Bit 00 = 0  Timer/counter 0 terminal count cleared.
     *     01 = 0  Timer/counter 1 terminal count cleared.
     *     02 = 0  Timer/counter 2 terminal count cleared.
     *     03 = 0  Timer/counter 3 terminal count cleared.
     *  30:04 = 0  Reserved and set to 0.
     *     31 = 0  Summary of all cause bits, read only.
     */

    YES_INIT_MV64260_REG (TMR_CNTR_INT_CAUSE_0_3, 0)

    /*
     * Timer/counter 0-3 Interrupt Mask Register
     * Bit 00 = 0  Timer/counter 0 interrupt disabled.
     *     01 = 0  Timer/counter 1 interrupt disabled.
     *     02 = 0  Timer/counter 2 interrupt disabled.
     *     03 = 0  Timer/counter 3 interrupt disabled.
     *  31:04 = 0  Read only.
     */

    YES_INIT_MV64260_REG (TMR_CNTR_INT_MASK_0_3, 0)

    END_INIT_MV64260_REG
    };

/* forward declarations */

void sysMv64260RegInit (UINT32 *);

LOCAL void  sysMv64260Init (void);

/******************************************************************************
*
* sysMv64260RegInit - Initialize MV64260 registers
*
* This function statically initializes specific MV64260 registers specified
* by a table pointed at by the input parameter.
*
* RETURNS: NA
*/

void sysMv64260RegInit 
    (
    UINT32 *p		/* Pointer to initialization table */
    )
    {
    while (*p != END_INIT_MV64260_REG)
	{
	switch (*p)
	    {
	    case MV64260_INTRNL:
		MV64260_WRITE32_PUSH (MV64260_REG_BASE, INTERNAL_REG_OFFSET(p), 
				      INTERNAL_REG_VALUE(p));
		p += MV64260_INTRNL_INIT_SZ;
		break;

	    case MV64260_READ_INIT:
		(void)MV64260_READ32(MV64260_REG_BASE,INTERNAL_REG_OFFSET(p));
		p += MV64260_READ_INIT_SZ;
		break;

	    case MV64260_PCICFG:
		sysPciConfigWrite (CFG_BUS(p), CFG_DEV(p), CFG_FCN(p), 
				   CFG_OFFSET(p), CFG_SZ(p), CFG_VAL(p));
		p += MV64260_PCICFG_INIT_SZ;
		break;

	    case MV64260_PCICFG_ALT:
		sysPciConfigWriteAlt (CFG_BUS(p), CFG_DEV(p), CFG_FCN(p), 
				      CFG_OFFSET(p), CFG_SZ(p), CFG_VAL(p));
		p += MV64260_PCICFG_ALT_INIT_SZ;
		break;
            
	    default:
		break;	/* Should never get here, if so we're pooched!  */
	    }	
	}
    }

/******************************************************************************
*
* sysMv64260RegInit - Initialize MV64260 registers
*
* This function initializes the MV64260 registers which can be statically
* initialized.
*
* RETURNS: NA
*/

LOCAL void sysMv64260Init (void)
    {
    UINT32 	*p = mv64260InitTbl;
    sysMv64260RegInit (p);
    }
