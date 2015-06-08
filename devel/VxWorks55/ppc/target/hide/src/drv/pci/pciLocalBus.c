/* pciLocalBus.c - PCI device configuration library */

/* Copyright 1984-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01i,27oct01,dat  Adding warnings about obsolete drivers
01h,29oct96,wlf  doc: cleanup.
01g,20sep96,mas  fixed for loop bug (SPR 7213)
01f,14aug96,dat  changed PCI_READ macro to include address
01e,02aug96,dat  rewritten for improved PCI spec compliance (SPR 6594, 6490)
01d,24jul96,dds  SPR 6819: The pci configuration registers are swapped
                 in the sysPciDevShow routine.
01c,23feb96,tpr  added #include "stdio.h" and removed ix in sysPciDevConfig().
01b,02fev95,vin  cleaned up.
01a,10nov94,vin  created.
*/

/*
DESCRIPTION
This library provides routines which configure a device which is located 
on the PCI bus. This library has designed with respect to the PCI LOCAL BUS
SPECIFICATION REVISION 2.0. It should be noted that the PCI BUS is a little
endian bus and any registers located on a PCI bus are little endian. So 
Big endian software should take care of this fact when writing and reading
registers located on a PCI bus.

This driver is customizable through the macros PCI_ADDR(base,index),
PCI_READ(base,index,addr), and PCI_WRITE(base,index,data).  These 3 macros
can be redefined prior to #including this file into sysLib.c.  Their default
definitions assume simple memory mapped device registers. See pciLocalBus.h
for default definitions.

PCI_ADDR(base,index) returns a local pointer to PCI_CFG space, given a
PCI base address and a longword index.

PCI_READ(base,index,addr) performs a 32 bit longword read from PCI
configuration space, storing the result at <addr>.

PCI_WRITE(base,index,data) performs a 32 bit longword write to PCI
configuration space.

The macro PSWAP(x) is always defined to perform the necessary byte
re-ordering based on whether or not the cpu is little-endian.  If not
then PSWAP is defined to be LONGSWAP.  If the cpu is little-endian, then
PSWAP is defined as a no-op.  The macros PCI_READ() and PCI_WRITE()
must include PSWAP as needed.  PSWAP is only valid for 32 bit
data elements.  See pciLocalBus.h for macros that will isolate fields
of less than 32 bits from the appropriate 32 bit value.

INCLUDE FILES: pciLocalBus.h
*/

/* include files */

#include "vxWorks.h"
#include "stdio.h"
#include "drv/pci/pciLocalBus.h"


/* pciLocalBus is being obsoleted. Please use pciConfigLib instead. */

#warning "src/drv/pci/pciLocalBus.c is outdated. Please use pciConfigLib"

/*******************************************************************************
*
* sysPciDevConfig - configure a device on a PCI bus
*
* This routine configures a device that is on a Peripheral Component
* Interconnect (PCI) bus. It specifies the base address(es) in I/O space and
* memory space.
*
* The `command' argument controls how the device is configured.  If 
* the CSR_IO_EN bit is set in `command', then the device is enabled in the 
* I/O address space at the address `devIoBaseAdrs'.  If the CSR_MEM_EN bit 
* is set, the device is enabled in the memory address space at the address
* `devMemBaseAdrs'.  In all cases, all command bits are eventually written
* into the device command register.
*
* After configuration, this routine calls the optional device-specific routine
* <devPciRoutine> with its two optional arguments.
*
* This routine does not try interpret the configuration information in any
* way; therefore it is important to configure the device with valid
* information.
*
* RETURNS: OK, or ERROR if <devPciRoutine> returns ERROR.
*/

STATUS sysPciDevConfig
    (
    ULONG 	devPciCnfgAdrs,		/* device PCI config space addrs */
    ULONG	devIoBaseAdrs,		/* device IO base address */
    ULONG 	devMemBaseAdrs,		/* device memory base address */
    ULONG	command,		/* command to load in command reg */
    FUNCPTR	devPciRoutine,		/* device specific function */
    ULONG	devPciArg1,		/* device specific function argument */
    ULONG	devPciArg2		/* device specific function argument */
    )
    {
    int i;
    ULONG temp;

    /* enable i/o mapping ? */

    if ((command & CSR_IO_EN) == CSR_IO_EN)
	{
	/* find i/o base addr reg */

	for (i = 0; i < PCI_NUM_BAR ; i++)
	    {
	    PCI_READ (devPciCnfgAdrs, PCI_IDX_BAR_0 + i, &temp);
	    if (temp & 0x01)
		{
		PCI_WRITE (devPciCnfgAdrs, PCI_IDX_BAR_0 + i,
			    devIoBaseAdrs | BASE_ADDR_REG_IO_SPACE);
		break;
		}
	    }

	/* if can't find one, then use bar 0 (for pre 2.1 boards) */

	if (i == PCI_NUM_BAR)
	    PCI_WRITE (devPciCnfgAdrs, PCI_IDX_BAR_0,
			devIoBaseAdrs | BASE_ADDR_REG_IO_SPACE);
	}
    
    /* enable memory mapping ? */

    if ((command & CSR_MEM_EN) == CSR_MEM_EN)
	{
	/* find memory base addr reg */

	for (i = 0; i < PCI_NUM_BAR ; i++)
	    {
	    PCI_READ (devPciCnfgAdrs, PCI_IDX_BAR_0 + i, &temp);
	    if ((temp & 0x01) == 0)
		{
		PCI_WRITE (devPciCnfgAdrs, PCI_IDX_BAR_0 + i,
			    devMemBaseAdrs | BASE_ADDR_REG_MEM_4GIG);
		break;
		}
	    }

	/* if can't find one, then use bar 1 (for pre 2.1 boards) */

	if (i == PCI_NUM_BAR)
	    PCI_WRITE (devPciCnfgAdrs, PCI_IDX_BAR_1,
			devMemBaseAdrs | BASE_ADDR_REG_MEM_4GIG);
	}

    /* give device all the command bits */

    PCI_WRITE (devPciCnfgAdrs, PCI_IDX_ST_CMD, command);

    /* call the device specific routine if routine is provided */

    if (devPciRoutine != NULL)
	if ((*devPciRoutine) (devPciArg1, devPciArg2) != OK)
	    return (ERROR);

    return (OK);
    }

#ifdef INCLUDE_SHOW_ROUTINES

/*******************************************************************************
*
* sysPciDevShow - show a PCI device configuration header
*
* This routine displays the configuration header for the PCI device. 
*
* RETURNS: OK, always.
*
* SEE ALSO: 
* .I "PCI Local Bus Specification 2.1"
*/

STATUS sysPciDevShow
    (
    ULONG *	pPciCnfgHdr	/* pointer to PCI config hdr */
    )
    {
    ULONG temp;
    int ix = 0;

    printf ("Pci Configuration Header Information:\n");

    PCI_READ (pPciCnfgHdr, PCI_IDX_DEV_VNDR, &temp);
    printf ("PCI device Id       :\t0x%04lx\n", PCI_DEVICE_ID (temp));
    printf ("PCI vendor Id       :\t0x%04lx\n", PCI_VENDOR_ID (temp));

    PCI_READ (pPciCnfgHdr, PCI_IDX_CLS_REV, &temp);
    printf ("PCI classCode       :\t0x%06lx\n", PCI_CLASS (temp));
    printf ("PCI revisionId      :\t0x%02lx\n", PCI_REVISION (temp));

    PCI_READ (pPciCnfgHdr, PCI_IDX_ST_CMD, &temp);
    printf ("PCI status & cmnd   :\t0x%08lx\n", temp);

    PCI_READ (pPciCnfgHdr, PCI_IDX_B_H_L_C, &temp);
    printf ("PCI BIST            :\t0x%02lx\n", PCI_BIST (temp));
    printf ("PCI header type     :\t0x%02lx\n", PCI_HDR (temp));
    printf ("PCI latency timer   :\t0x%02lx\n", PCI_LAT_TMR (temp));
    printf ("PCI cache line Size :\t0x%02lx\n", PCI_CACHE_LN_SZ (temp));

    for (ix = 0; ix < PCI_NUM_BAR; ix ++)
	{
	PCI_READ (pPciCnfgHdr, PCI_IDX_BAR_0 + ix, &temp);
	printf ("PCI Base addr Reg %2d:\t0x%08lx\n", ix, temp);
	}

    PCI_READ (pPciCnfgHdr, PCI_IDX_CIS_PTR, &temp);
    printf ("PCI cardbus CIS Ptr :\t0x%08lx\n", temp);
    
    PCI_READ (pPciCnfgHdr, PCI_IDX_SUB_SVID, &temp);
    printf ("PCI Subsystem Id    :\t0x%04lx\n", PCI_SUB_ID (temp));
    printf ("PCI Vendor Sub Id   :\t0x%04lx\n", PCI_SUB_VDR_ID (temp));

    PCI_READ (pPciCnfgHdr, PCI_IDX_EXP_ROM, &temp);
    printf ("PCI exp rom addr    :\t0x%08lx\n", temp);

    PCI_READ (pPciCnfgHdr, PCI_IDX_RES_1, &temp);
    printf ("PCI reserved 1      :\t0x%08lx\n", temp);

    PCI_READ (pPciCnfgHdr, PCI_IDX_RES_2, &temp);
    printf ("PCI reserved 2      :\t0x%08lx\n", temp);

    PCI_READ (pPciCnfgHdr, PCI_IDX_L_G_I_I, &temp);
    printf ("PCI maximum latency :\t0x%02lx\n", PCI_MAX_LAT (temp));
    printf ("PCI minimum Grant   :\t0x%02lx\n", PCI_MIN_GNT (temp));
    printf ("PCI interrupt pin   :\t0x%02lx\n", PCI_INT_PIN (temp));
    printf ("PCI interrupt line  :\t0x%02lx\n", PCI_INT_LINE (temp));

    return (OK);
    }
#endif /*INCLUDE_SHOW_ROUTINES*/


/*******************************************************************************
*
* sysPciDevProbe - probe a PCI bus for a specified device ID
*
* This routine scans a Peripheral Component Interconnect (PCI) bus for 
* a device with a specified ID.
*
* This routine assumes that each PCI ID select (IDSEL) line is mapped to 
* an address line, A10 through A31.  PCI_DEV_SEL is initially A10, and is
* moved left with each pass through the loop.  If the selection bit hits a
* fixed bit in the configuration address, then the entire range of valid
* addresses has been scanned.
*
* If a multifunction card is found, then this routine searches all eight
* possible functions for a match to the device ID.
*
* CAVEAT: This routine cannot cross a PCI-PCI bridge, and therefore does not
* comply with the 
* .I "PCI Local Bus Specification 2.1."
*
* RETURNS: OK, or ERROR if the device is not found.
*
* SEE ALSO:
* .I "PCI Local Bus Specification 2.1."
*/

STATUS sysPciDevProbe
    (
    ULONG	pciCnfgBaseAdrs,	/* PCI configuration base address */
    ULONG	pciVndrDevId,		/* PCI vendor device ID */
    ULONG *	pPciDevCnfgAdrs		/* PCI device config addr returned */
    )
    {
    ULONG	devIdSel;
    int		ix;
    int		iy;
    int		temp;
    ULONG * 	pConfigReg;

    devIdSel = PCI_DEV_IDSEL;
    
    /* Scan while the selection bit is still visible */

    for (ix = 0; ix < PCI_DEV_IDSEL_MAX ; ix++, devIdSel <<= 1)
	{
	/* generate base address by using address lines for select bits */

	pConfigReg = (ULONG *)(pciCnfgBaseAdrs | devIdSel);

	/* stop if selection bit no longer makes a difference */

	if ((ULONG)pConfigReg == pciCnfgBaseAdrs)
	    break;

	/* copy config address to user's variable */

	*pPciDevCnfgAdrs = (ULONG)pConfigReg;

	/* test function 0 for a match */

	PCI_READ (pConfigReg, PCI_IDX_DEV_VNDR, &temp);

	if (pciVndrDevId == temp)
	    return (OK);

	if (PCI_VENDOR_ID (temp) == INVALID_VENDOR_ID)
	    continue; /* an empty slot, try next slot */

	/* If a multi-function card, we check functions 1 thru 7 */

	PCI_READ (pConfigReg, PCI_IDX_B_H_L_C, &temp);
	if (PCI_MULTI_FUNC_HDR(temp))
	    {
	    for (iy=1; iy < 8; iy++)
		{
		pConfigReg += PCI_CNFG_HDR_LONGS; /* # of longs in hdr */

		*pPciDevCnfgAdrs = (ULONG)pConfigReg; /* copy to user's area */

		PCI_READ (pConfigReg, PCI_IDX_DEV_VNDR, &temp);
		if (pciVndrDevId == temp)
		    return (OK); /* match found in sub-function */
		}
	    }
	}

    /* device not found on the primary PCI BUS */

    return (ERROR);
    }
