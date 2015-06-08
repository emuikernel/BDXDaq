/* frcEagle02.c - Force Eagle-02 module library */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,24jun96,wlf  doc: cleanup.
01b,20oct92,caf  changed sysEagle01Init() to sysEagleInit(), made it NOMANUAL.
		 initialized *FGA_ICRLOCAL6.
01a,16jul92,caf  created.
*/

/*
DESCRIPTION
This library contains routines to manipulate the Force EAGLE-02
module.  This library addresses initialization plus the LANCE Ethernet
function of the EAGLE-02.
*/

/* includes */

#include "drv/multi/fc68165.h"

/* globals */

FC68165 *stIOC= (FC68165 *) FRC40_E002_IOC_BASE_ADRS;

/*******************************************************************************
*
* sysEagleInit - initialize the Force EAGLE-02 module hardware
*
* This routine initializes the Force EAGLE-02 module hardware.
*
* RETURNS: N/A
*/

void sysEagleInit (void)
    {
    /* Do FC68165 init - IOC and FLASH set by FGA002 Boot Software */

    stIOC->cs_range [FRC40_E002_IOCCS_VSB1] =	FRC40_E002_VSB1_BASE_ADRS   |
						FRC40_E002_VSB1_SIZE	    |
						IOC_CSRNG_FLXI		    |
						IOC_CSRNG_SELCODE_SUPRONLY  |
						IOC_CSRNG_CACHEINHIB;

    stIOC->cs_range [FRC40_E002_IOCCS_CSR] =	FRC40_E002_CSR_BASE_ADRS    |
						FRC40_E002_CSR_SIZE	    |
						IOC_CSRNG_FLXI		    |
						IOC_CSRNG_SELCODE_SUPRONLY  |
						IOC_CSRNG_CACHEINHIB;

    stIOC->cs_range [FRC40_E002_IOCCS_VSB2] =	FRC40_E002_VSB2_BASE_ADRS   |
						FRC40_E002_VSB2_SIZE	    |
						IOC_CSRNG_FLXI		    |
						IOC_CSRNG_SELCODE_SUPRONLY  |
						IOC_CSRNG_CACHEINHIB;

    stIOC->cs_range [FRC40_E002_IOCCS_SRAM] =	FRC40_E002_SRAM_BASE_ADRS   |
						FRC40_E002_SRAM_SIZE	    |
						IOC_CSRNG_FLXI		    |
						IOC_CSRNG_SELCODE_SUPRONLY  |
						IOC_CSRNG_CACHEINHIB;

    stIOC->cs_range [FRC40_E002_IOCCS_LANCE] =	FRC40_E002_LANCE_BASE_ADRS  |
						FRC40_E002_LANCE_SIZE	    |
						IOC_CSRNG_FLXI		    |
						IOC_CSRNG_SELCODE_SUPRONLY  |
						IOC_CSRNG_CACHEINHIB;

    stIOC->cs_config [FRC40_E002_IOCCS_VSB1] =	IOC_CSCFG_FLXI_SCS	    |
						IOC_CSCFG_FLXI_SBS	    |
						IOC_CSCFG_FLXI_BRDA	    |
						IOC_CSCFG_FLXI_BRDE	    |
						IOC_CSCFG_FLXI_WAITS_15	    |
						IOC_CSCFG_FLXI_STROBE_WORD  |
						IOC_CSCFG_FLXI_DSACK_NONE;

    stIOC->cs_config [FRC40_E002_IOCCS_CSR] =	IOC_CSCFG_FLXI_SCS	    |
						IOC_CSCFG_FLXI_SBS	    |
						IOC_CSCFG_FLXI_BRDA	    |
						IOC_CSCFG_FLXI_BRDE	    |
						IOC_CSCFG_FLXI_WAITS_15	    |
						IOC_CSCFG_FLXI_STROBE_WORD  |
						IOC_CSCFG_FLXI_DSACK_NONE;

    stIOC->cs_config [FRC40_E002_IOCCS_VSB2] =	IOC_CSCFG_FLXI_SCS	    |
						IOC_CSCFG_FLXI_SBS	    |
						IOC_CSCFG_FLXI_BRDA	    |
						IOC_CSCFG_FLXI_BRDE	    |
						IOC_CSCFG_FLXI_WAITS_15	    |
						IOC_CSCFG_FLXI_STROBE_WORD  |
						IOC_CSCFG_FLXI_DSACK_NONE;

    stIOC->cs_config [FRC40_E002_IOCCS_SRAM] =	IOC_CSCFG_FLXI_SCS	    |
						IOC_CSCFG_FLXI_SBS	    |
						IOC_CSCFG_FLXI_BRDA	    |
						IOC_CSCFG_FLXI_BRDE	    |
						IOC_CSCFG_FLXI_WAITS_15	    |
						IOC_CSCFG_FLXI_STROBE_WORD  |
						IOC_CSCFG_FLXI_DSACK_NONE;

    stIOC->cs_config [FRC40_E002_IOCCS_LANCE] =	IOC_CSCFG_FLXI_SCS	    |
						IOC_CSCFG_FLXI_SBS	    |
						IOC_CSCFG_FLXI_BRDA	    |
						IOC_CSCFG_FLXI_BRDE	    |
						IOC_CSCFG_FLXI_WAITS_15	    |
						IOC_CSCFG_FLXI_STROBE_WORD  |
						IOC_CSCFG_FLXI_DSACK_NONE;

    /* extended interrupt control registers */

    *FGA_ICRLOCAL1 = INT_LVL_VSB | FGA_ICR_ACTIVITY;
    *FGA_ICRLOCAL6 = NULL;
    *FGA_ICRLOCAL7 = INT_LVL_LANCE;
    }

/*******************************************************************************
*
* sysLanIntEnable - enable a LAN interrupt level
*
* This routine enables interrupts for the on-board LAN chip at a specified
* level.  LAN interrupts are controlled by the FGA-002 chip.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysLanIntDisable()
*/

STATUS sysLanIntEnable
    (
    int intLevel        /* interrupt level to enable */
    )
    {
    /* unmask local LANCE interrupt */

    *FGA_ICRLOCAL7 |= FGA_ICR_ENABLE;

    return (OK);
    }

/*******************************************************************************
*
* sysLanIntDisable - disable a LAN interrupt level
*
* This routine disables a specified interrupt level for the on-board LAN chip.
* LAN interrupts are controlled by the FGA-002 chip.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysLanIntEnable()
*/

STATUS sysLanIntDisable (void)

    {
    /* mask LANCE interrupt */

    *FGA_ICRLOCAL7 &= ~FGA_ICR_ENABLE;

    return (OK);
    }
