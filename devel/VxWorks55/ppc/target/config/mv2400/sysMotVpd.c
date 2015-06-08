/* sysMotVpd.c - Board-Specified Vital Product Data Routines. */

/* Copyright 1998-2002 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01l,16apr02,dat  Removing warnings for T2.2 release
01k,19sep01,dat  Removing sysHid1Get, using vxHid1Get()
01j,22jun01,pch  Fix bootrom build
01i,19nov99,srr  Adjusted multipliers to be ratios of 2 to avoid overflow.
01h,01jul99,rhv  Incorporating WRS code review changes and file name change
                 from sysVpd.c to sysMotVpd.c.
01g,29jan99,rhv  Removed unused definition of EXIT_TO_MONITOR and fixed a
                 bug in product ID string processing.
01f,28jan99,rhv  Changed sysMsgSpit to sysDebugMsg.
01e,19jan99,rhv  Removing unused variables.
01d,14jan99,mdp	 Fix for Hawk Aux Clock.
01c,13jan99,rhv  Re-ordering file history.
01b,13jan99,rhv  Moved sysModel back to sysLib.c and updated copyright.
01a,15dec98,rhv  Created by Motorola.
*/

/*
DESCRIPTION
This file contains the Board-Specific Vital Product Data utility routines.

CAVEATS
These routines are needed before the kernel is un-compressed. For proper
operation, this file must be added to the BOOT_EXTRA list in the Makefile to
prevent it from being compressed during kernel generation.
*/

#include "vxWorks.h"
#include "mv2400.h"
#include "sysMotVpd.h"
#include "arch/ppc/vxPpcLib.h"

/* defines */

/* locals */

static UINT tableArthur[] =     /* PLL multiplier times two */
    {
     0,  15,  14,   2,   4,  13,   5,   9,
     6,  11,   8,  10,   3,  12,   7,   0
    };

/* globals */

VPD sysVpd; /* board vital product data */

VPD_PACKET * sysVpdPkts[VPD_PKT_LIMIT] = { 0 }; /* board vpd packets */

UINT32 sysBusSpeed = DEFAULT_BUS_CLOCK; /* bus speed in Hz. */
UINT32 sysCpuSpeed = DEFAULT_INTERNAL_CLOCK; /* internal cpu speed in Hz. */
UINT32 sysPciSpeed = DEFAULT_PCI_CLOCK; /* PCI bus speed in Hz. */

UCHAR   sysProductStr[25] = DEFAULT_PRODUCT_ID; /* product identifier */
VPD_PACKET * pSysBrdOpts = NULL; /* board options packet */

/* forwards */

STATUS sysVpdPktInit ( UCHAR, UCHAR, VPD *, VPD_PACKET **, UINT32);
STATUS sysVpdPktGet ( UCHAR, UINT32, VPD_PACKET **, VPD_PACKET **);
void   sysDebugMsg (char *, UINT32);

/******************************************************************************
*
* sysReportVpdError - routine to report errors in vpd data.
*
* This routine prints an error message at the system console and optionally
* returns to the boot rom.
*
* RETURNS: N/A
*/
void sysReportVpdError
    (
    char * str
    )
    {
    sysDebugMsg (str, DEFAULT_BSP_ERROR_BEHAVIOR);
    }

/******************************************************************************
*
* sysGetBusSpd - routine to get the speed of the 60x processor bus
*
* This routine returns the speed (in Hz) of the 60x system bus.
*
* RETURNS: The bus speed (inHz).
*/

UINT sysGetBusSpd (void)
    {
    return (sysBusSpeed);
    }

/******************************************************************************
*
* sysGetPciSpd - routine to get the speed of the PCI bus
*
* This routine returns the speed (in Hz) of the PCI bus.
*
* RETURNS: The bus speed (inHz).
*/

UINT sysGetPciSpd (void)
    {
    return (sysPciSpeed);
    }

/******************************************************************************
*
* sysGetMpuSpd - routine to get the speed of the 60x processor bus
*
* This routine returns the speed (in Hz) of the 60x system bus.
*
* RETURNS: The bus speed (inHz).
*/

UINT sysGetMpuSpd (void)
    {
    return (sysCpuSpeed);
    }

/******************************************************************************
*
* sysCalcMpuSpd - get the speed of the MPC750 processor.
*
* This routine returns the speed (in MHz) of the 60x processor
*
* RETURNS: The approximate CPU speed (inMHz).
*
* Note:  This speed returned is an approximation based on the
*        accuracy of the value returned by sysGetBusSpd().
*        For a 267MHz Arthur system, running with a bus clock
*        of 67 MHz, the actual speed returned is 268MHz.
*
*        This function is dependent upon proper bus speed being
*        returned via call to sysGetBusSpd().  In addition to this
*        dependency, the function will only work for CPUs whose
*        speed is dependent upon the bus speed and the value in
*        the PLL bits of the HID1 register.  CPUs which HAVE this
*        property are 602, Arthur, 604ev and 604r.  CPUs which
*        do NOT HAVE this property (and for which this function
*        will NOT work) are: 601, 603, 603e, 603p, and 604.
*/

LOCAL UINT sysCalcMpuSpd(void)
    {
    UINT speed;
    UINT busMultiplier;

    busMultiplier = tableArthur[vxHid1Get() >> 28];
    speed = (sysGetBusSpd() * busMultiplier)/2;
    return(speed);
    }

/*******************************************************************************
*
* sysVpdInit - initialize the board vital product data structures.
*
* This routine reads the VPD and extracts the commonly used data.
*
* RETURNS: OK, if successful or ERROR if unsuccessful.
*
* SEE ALSO: N/A()
*/

STATUS sysVpdInit(void)
    {
    VPD_PACKET * pVpdPkt;
    UINT32	 idx;

    /* read the vpd from the serial eeprom. */

    if ( sysVpdPktInit (VPD_BRD_EEPROM_ADRS, VPD_BRD_OFFSET, &sysVpd,
                            &sysVpdPkts[0], VPD_PKT_LIMIT) != OK)
        {
        sysReportVpdError (
                    "sysVpdInit: Unable to read Vital Product Data (VPD).\n\r");
        return (ERROR);
        }

    /* get the board type */

    if ( (sysVpdPktGet (VPD_PID_PID, 0, &sysVpdPkts[0], &pVpdPkt) == OK) &&
         ((pVpdPkt->size + 1) < sizeof (sysProductStr)) )
        {

        for (idx = 0; idx < pVpdPkt->size; idx++)
            sysProductStr[idx] = pVpdPkt->data[idx];
        sysProductStr[idx] = '\0';

        }
    else
        sysReportVpdError ("sysVpdInit: Unable to read board type.\n\r");
       
    /* get the PCI bus speed */

    if ( (sysVpdPktGet (VPD_PID_PCS, 0, &sysVpdPkts[0], &pVpdPkt) == OK) &&
	 (pVpdPkt->size == sizeof (UINT32)) )

	sysPciSpeed = *(UINT32 *)&pVpdPkt->data[0];

    else
	sysReportVpdError ("sysVpdInit: Unable to read PCI bus speed.\n\r");

    /* get the bus speed */

    if ( (sysVpdPktGet (VPD_PID_ECS, 0, &sysVpdPkts[0], &pVpdPkt) == OK) &&
	 (pVpdPkt->size == sizeof (UINT32)) )

        sysBusSpeed = *(UINT32 *)&pVpdPkt->data[0];

    else
        sysReportVpdError ("sysVpdInit: Unable to read bus speed.\n\r");

    /* calculate the internal cpu speed */

    sysCpuSpeed = sysCalcMpuSpd();

    /* get the product configuration options */

    if (sysVpdPktGet (VPD_PID_PCO, 0, &sysVpdPkts[0], &pSysBrdOpts) != OK)
        sysReportVpdError (
                       "sysVpdInit: Unable to read configuration options.\n\r");

    return (OK);

    }

/*******************************************************************************
*
* sysProdConfigGet - initialize the board vital product data structures.
*
* This routine tests the presence of a caller specified product configuration
* option using the contents of the product configuration option vpd packet.
*
* RETURNS: TRUE if the option is present or FALSE if the product configuration
* option is not present or if the product configuration option packet is
* missing.
*
* SEE ALSO: N/A
*/
BOOL sysProdConfigGet
    (
    UINT32 optionId
    )
    {
    UCHAR  mask;
    UINT32 byteIdx;

#ifdef BYPASS_VPD_PCO

    /* console and first ethernet are alway present */

    if ( (optionId == PCO_SERIAL1_CONN) ||
         (optionId == PCO_ENET1_CONN) )
	return (TRUE);

#endif /* BYPASS_VPD_PCO */

    /* see if we have a valid board option vpd packet and option id. */

    if (pSysBrdOpts == NULL)
	return (FALSE);

    /* calculate the bit mask and byte index */

    mask = 0x80 >> (optionId % 8);
    byteIdx = optionId / 8;

    /* if byte index is off the end of the data array, return false. */

    if (byteIdx >= pSysBrdOpts->size)
	return (FALSE);

    return ( (pSysBrdOpts->data[byteIdx] & mask) != 0 );
    }
