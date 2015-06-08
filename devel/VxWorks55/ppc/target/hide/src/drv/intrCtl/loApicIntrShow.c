/* loApicIntrShow.c - Intel Local APIC/xAPIC driver show routines */

/* Copyright 1984-2002 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01b,08mar02,hdn  added the description
01a,25jun01,hdn  extracted from 01c version of loApicIntr.c
*/

/*
DESCRIPTION
This module is a show routine for the Local APIC/xAPIC (Advanced 
Programmable Interrupt Controller) for P6 (PentiumPro, II, III) 
family processors and P7 (Pentium4) family processors.  
loApicShow() shows content of the Local APIC registers.
loApicMpShow() shows the MP configuration table.

INCLUDE FILES: loApic.h
*/

/* includes */

#include "drv/intrCtl/loApic.h"


/* externs */


/* globals */


/* locals */


/* forward declarations */


/*******************************************************************************
*
* loApicStatusShow - show Local APIC TMR, IRR, ISR registers
*
* This routine shows Local APIC TMR, IRR, ISR registers
*
* RETURNS: N/A
*/

LOCAL void loApicStatusShow (void)
    {
    int ix;

    for (ix = 0; ix < 8; ix++)
	{
        printf ("LOAPIC_TMR%02d = 0x%08x ", ix,
	        *(UINT32 *)(loApicBase + LOAPIC_TMR + (ix * 0x10)));
        printf ("LOAPIC_IRR%02d = 0x%08x ", ix,
	        *(UINT32 *)(loApicBase + LOAPIC_IRR + (ix * 0x10)));
        printf ("LOAPIC_ISR%02d = 0x%08x\n", ix,
	        *(UINT32 *)(loApicBase + LOAPIC_ISR + (ix * 0x10)));
	}
    }

/*******************************************************************************
*
* loApicShow - show Local APIC registers
*
* This routine shows Local APIC registers
*
* RETURNS: N/A
*/

void loApicShow (void)
    {
    UINT32 localApicReg;

    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_ID);
    printf ("local Apic ID              = 0x%08x\n", localApicReg);
    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_VER);
    printf ("local Apic Version         = 0x%08x\n", localApicReg);
    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_TPR);
    printf ("Task Priority              = 0x%08x\n", localApicReg);
    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_APR);
    printf ("Arbitration Priority       = 0x%08x\n", localApicReg);
    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_PPR);
    printf ("Processor Priority         = 0x%08x\n", localApicReg);
    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_LDR);
    printf ("Logical Destination        = 0x%08x\n", localApicReg);
    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_DFR);
    printf ("Destination Format         = 0x%08x\n", localApicReg);
    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_SVR);
    printf ("Spurious Interrupt Vector  = 0x%08x\n", localApicReg);
    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_ESR);
    printf ("Error Status               = 0x%08x\n", localApicReg);
    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_ICRLO);
    printf ("Interrupt Command 00-31    = 0x%08x\n", localApicReg);
    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_ICRHI);
    printf ("Interrupt Command 32-63    = 0x%08x\n", localApicReg);

    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_TIMER);
    printf ("Local Vector Table (Timer) = 0x%08x\n", localApicReg);
    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_LINT0);
    printf ("Local Vector Table (LINT0) = 0x%08x\n", localApicReg);
    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_LINT1);
    printf ("Local Vector Table (LINT1) = 0x%08x\n", localApicReg);
    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_ERROR);
    printf ("Local Vector Table (Error) = 0x%08x\n", localApicReg);

    if (loApicMaxLvt >= LOAPIC_LVT_P6)
	{
        localApicReg = *(UINT32 *)(loApicBase + LOAPIC_PMC);
        printf ("Local Vector Table (PMC)   = 0x%08x\n", localApicReg);
	}
    if (loApicMaxLvt >= LOAPIC_LVT_PENTIUM4)
	{
        localApicReg = *(UINT32 *)(loApicBase + LOAPIC_THERMAL);
        printf ("Local Vector Table (Therm) = 0x%08x\n", localApicReg);
	}

    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_TIMER_ICR);
    printf ("Timer Initial Count        = 0x%08x\n", localApicReg);
    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_TIMER_CCR);
    printf ("Timer Current Count        = 0x%08x\n", localApicReg);
    localApicReg = *(UINT32 *)(loApicBase + LOAPIC_TIMER_CONFIG);
    printf ("Timer Divide Configuration = 0x%08x\n", localApicReg);

    loApicStatusShow ();
    }

/*******************************************************************************
*
* loApicMpShow - show MP configuration table
*
* This routine shows MP configuration table.
*
* RETURNS: N/A
*/

void loApicMpShow (void)
    {
    MP_FPS *	pFps = NULL;
    MP_HEADER *	pTbl;
    MP_CPU *	pCpu;
    MP_IOAPIC *	pIoApic;
    char * p;
    int ix;

    /* MP Floating Point Structure */

    pFps = (MP_FPS *)loApicMpScan ((char *)EBDA_START, (char *)EBDA_END);
    if (pFps == NULL)
        pFps = (MP_FPS *)loApicMpScan ((char *)BIOS_ROM_START, 
				       (char *)BIOS_ROM_END);

    if (pFps == NULL)
	{
	printf ("Not MP Compliant\n");
	return;
	}
    
    printf ("MP Floating Point Structure\n");
    printf ("  Address Pointer       = 0x%08x\n", pFps->configTableAddr);
    printf ("  Spec Version          = 0x%02x\n", pFps->specRev);
    printf ("  Feature byte-1        = 0x%02x\n", pFps->featureByte[0]);
    printf ("  Feature byte-2        = 0x%02x\n", pFps->featureByte[1]);
    printf ("  Feature byte-3        = 0x%02x\n", pFps->featureByte[2]);
    printf ("  Feature byte-4        = 0x%02x\n", pFps->featureByte[3]);
    printf ("  Feature byte-5        = 0x%02x\n", pFps->featureByte[4]);

    if ((pFps->featureByte[0] != 0) || (pFps->configTableAddr == 0))
	{
	printf ("MP Configuration Table does not exist");
	return;
	}

    /* show MP Configuration Table Header */

    pTbl = (MP_HEADER *)pFps->configTableAddr;

    printf ("MP Configuration Table\n");
    printf ("  Base Table Length     = 0x%04x\n", pTbl->tableLength);
    printf ("  OEM ID String         = ");
    for (ix = 0; ix < sizeof (pTbl->oemId); ix++)
	printf ("%c", pTbl->oemId[ix]);
    printf ("\n");
    printf ("  Product ID String     = ");
    for (ix = 0; ix < sizeof (pTbl->prodId); ix++)
	printf ("%c", pTbl->prodId[ix]);
    printf ("\n");
    printf ("  OEM Table Pointer     = 0x%08x\n", pTbl->oemTablePtr);
    printf ("  OEM Table Size        = 0x%04x\n", pTbl->oemTableSize);
    printf ("  Entry Count           = 0x%04x\n", pTbl->entryCount);
    printf ("  Address of local APIC = 0x%08x\n", pTbl->localApicBaseAddr);
    printf ("  Extended Table Length = 0x%04x\n", pTbl->extendedTableLength);

    /* show MP Configuration Table Entry */

    p = (char *)pTbl + sizeof(MP_HEADER);
    for (ix = 0; ix < pTbl->entryCount; ix++)
	switch (*p)
	    {
	    case MP_ENTRY_CPU:			/* Processor Entry */
		pCpu = (MP_CPU *)p;
		printf ("Processor Entry\n");
		printf ("  Local Apic ID         = 0x%02x\n", 
			pCpu->localApicId);
		printf ("  Local Apic Ver        = 0x%02x\n", 
			pCpu->localApicVersion);
		printf ("  CPU Flags             = 0x%02x\n", pCpu->cpuFlags);
		printf ("  CPU Signature         = 0x%08x\n", pCpu->cpuSig);
		printf ("  Feature Flags         = 0x%08x\n", 
			pCpu->featureFlags);

		p += sizeof (MP_CPU);
		break;

	    case MP_ENTRY_BUS:			/* Bus Entry */
		printf ("Bus Entry\n");
		printf ("  Bus ID                = 0x%02x\n", *(p + 1));
		printf ("  Bus Type String       = ");
		for (ix = 0; ix < 6; ix++)
		    printf ("%c", *(p + 2 + ix));
		printf ("\n");

		p += 8;
		break;

	    case MP_ENTRY_IOAPIC:		/* IO APIC Entry */
		pIoApic = (MP_IOAPIC *)p;
		printf ("IO Apic Entry\n");
		printf ("  IO Apic ID            = 0x%02x\n", 
			pIoApic->ioApicId);
		printf ("  IO Apic Ver           = 0x%02x\n", 
			pIoApic->ioApicVersion);
		printf ("  IO Apic Flags         = 0x%02x\n", 
			pIoApic->ioApicFlags);
		printf ("  IO Apic Address       = 0x%08x\n", 
			pIoApic->ioApicBaseAddress);

		p += sizeof (MP_IOAPIC);
		break;

	    case MP_ENTRY_IOINTERRUPT:		/* IO Interrupt Entry */
		printf ("IO Interrupt Entry\n");
		printf ("  Interrupt Type        = 0x%02x\n", *(p + 1));
		printf ("  IO Interrupt Flag     = 0x%04x\n", 
			*(UINT16 *)(p + 2));
		printf ("  Source Bus ID         = 0x%02x\n", *(p + 4));
		printf ("  Source Bus IRQ        = 0x%02x\n", *(p + 5));
		printf ("  Dest IO Apic ID       = 0x%02x\n", *(p + 6));
		printf ("  Dest IO Apic INTIN    = 0x%02x\n", *(p + 7));

		p += 8;
		break;

	    case MP_ENTRY_LOINTERRUPT:		/* Local Interrupt Entry */
		printf ("Local Interrupt Entry\n");
		printf ("  Interrupt Type        = 0x%02x\n", *(p + 1));
		printf ("  IO Interrupt Flag     = 0x%04x\n", 
			*(UINT16 *)(p + 2));
		printf ("  Source Bus ID         = 0x%02x\n", *(p + 4));
		printf ("  Source Bus IRQ        = 0x%02x\n", *(p + 5));
		printf ("  Dest Local Apic ID    = 0x%02x\n", *(p + 6));
		printf ("  Dest Local Apic INTIN = 0x%02x\n", *(p + 7));

		p += 8;
		break;

	    default:		/* Unknown Entry */
		p += 8;		/* wild guess */
	    }
    }

