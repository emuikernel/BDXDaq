/* ioApicIntrShow.c - Intel IO APIC/xAPIC driver show routines */

/* Copyright 1984-2002 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01b,08mar02,hdn  added the description
01a,25jun01,hdn  extracted from 01d version of ioApicIntr.c
*/

/*
DESCRIPTION
This module is a show routine for the IO APIC/xAPIC (Advanced 
Programmable Interrupt Controller) for P6 (PentiumPro, II, III) 
family processors and P7 (Pentium4) family processors.  
ioApicShow() shows content of the IO APIC registers.

INCLUDE FILES: loApic.h, ioApic.h

SEE ALSO: ioApicIntr.c
*/

/* includes */

#include "drv/intrCtl/loApic.h"
#include "drv/intrCtl/ioApic.h"


/* externs */


/* globals */


/* locals */


/* forward declarations */


/*******************************************************************************
*
* ioApicShow - show IO APIC registers
*
* This routine shows IO APIC registers
*
* RETURNS: N/A
*/

void ioApicShow (void)
    {
    int ix;
    int version;
    UINT32 ioApicData = ioApicBase + IOAPIC_DATA;

    printf ("IOAPIC_ID    = 0x%08x\n", 
	    ioApicGet (ioApicBase, ioApicData, IOAPIC_ID));

    version = ioApicGet (ioApicBase, ioApicData, IOAPIC_VERS);
    printf ("IOAPIC_VER   = 0x%08x\n", version);

    printf ("IOAPIC_ARB   = 0x%08x\n", 
	    ioApicGet (ioApicBase, ioApicData, IOAPIC_ARB));

    if (ioApicVersion & IOAPIC_PRQ)			/* use PRQ bit XXX */
        printf ("IOAPIC_BOOT  = 0x%08x\n", 
                ioApicGet (ioApicBase, ioApicData, IOAPIC_BOOT));

    for (ix = 0; ix <= ((version & IOAPIC_MRE_MASK) >> 16); ix++)
	{
        printf ("IOAPIC_TBL%02d = 0x%08x ", ix, 
	        ioApicGet (ioApicBase, ioApicData, IOAPIC_REDTBL + ix * 2 + 1));
        printf ("%08x\n", 
	        ioApicGet (ioApicBase, ioApicData, IOAPIC_REDTBL + ix * 2));
	}
    }
