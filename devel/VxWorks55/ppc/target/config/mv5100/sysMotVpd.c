/* sysMotVpd.c - Board-Specified Vital Product Data Routines. */

/* Copyright 1999-2001 Wind River Systems, Inc. */
/* Copyright 1999-2001 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01d,08mar01,rhk  Cleaned up comments in DESCRIPTION section.
01c,16aug00,dmw  Wind River coding standards review.
01b,07jun00,dmw  Update to PrPMC name convention.
01a,13mar00,dmw  Written (from verison 01b of mcpn765/sysVpd.c).
*/

/*
DESCRIPTION
This file contains the Board-Specific Vital Product Data utility routines.

These routines extract or provide access to various parameters stored in the
board's Vital Product Data (VPD). In general, a board's VPD is stored in a
serial EEPROM device wired to the I2C bus and replaces the functions formerly
provided by system registers. A lower-level access library provides the basic
VPD read, validate and parse operations while the routines in this file extract
and save commonly used VPD parameters for faster access.

The PCI bus frequency and processor core frequency are derived by routines
provided in this file.  The processor bus speed is calculated by the routine
'sysCalcBusSpd' in the file hawkSmc.c.

CAVEATS
These routines are needed before the kernel is un-compressed. For proper
operation, this file must be added to the BOOT_EXTRA list in the Makefile to
prevent it from being compressed during kernel generation.
*/

#include "sysMotVpd.h"

/* defines */

/* locals */

static UINT tableArthur[] =     /* PLL multiplier times two */
    {
    0,  15,  14,   2,   4,  13,   5,   9,
    6,  11,   8,  10,   3,  12,   7,   0
    };

static UINT tableHawk[] =       /* PCI clock ratios times two */
    {
    0,   2,   4,   6,   3,   0,   5,  0
    };

/* globals */

VPD sysVpd; /* board vital product data */

VPD_PACKET * sysVpdPkts[VPD_PKT_LIMIT] = { 0 }; /* board vpd packets */

UINT32 sysBusSpeed = DEFAULT_BUS_CLOCK;      /* bus speed in Hz. */
UINT32 sysCpuSpeed = DEFAULT_INTERNAL_CLOCK; /* internal cpu speed in Hz. */
UINT32 sysPciSpeed = DEFAULT_PCI_CLOCK;      /* PCI bus speed in Hz. */

UCHAR        sysProductStr[25] = DEFAULT_PRODUCT_ID; /* product identifier */
VPD_PACKET * pSysBrdOpts       = NULL;               /* board options packet */

/* externals */

IMPORT STATUS sysVpdPktInit (UCHAR devAdrs, UCHAR devOffset, 
                             VPD   *pVpd, VPD_PACKET **pVpdPtr,
                             UINT32 PktLimit);
IMPORT STATUS sysVpdPktGet  (UCHAR  vpdType, UINT32 vpdInstance, 
                             VPD_PACKET **pVpdPtr, VPD_PACKET ** pVpdPacket);
IMPORT void   sysDebugMsg   (char * str, UINT32  recovery);
IMPORT UINT32 sysCalcBusSpd (void);


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
    char * str		/* message string to print */
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
* RETURNS: The bus speed (in Hz).
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
* RETURNS: The bus speed (in Hz).
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
* RETURNS: The bus speed (in Hz).
*/

UINT sysGetMpuSpd (void)
    {
    return (sysCpuSpeed);
    }


/******************************************************************************
*
* sysCalcPciSpd - get the speed of the PCI bus.
*
* This routine returns the speed (in Hz) of the PCI bus
*
* RETURNS: The approximate PCI speed (in Hz).
*
*/

LOCAL UINT sysCalcPciSpd (void)
    {
    UINT speed;
    UINT busDivisor;
    UCHAR index;

    index = *((UCHAR *)HAWK_PHB_BASE_ADRS + HAWK_MPC_HCSR);
    busDivisor = tableHawk[index & HAWK_MPC_HCSR_XPR_MASK];
    speed = (sysGetBusSpd() * 2) / busDivisor;
    return (speed);

    }


/******************************************************************************
*
* sysCalcMpuSpd - get the speed of the MPC750 processor.
*
* This routine returns the speed (in Hz) of the 60x processor
*
* Note:  This speed returned is an approximation based on the
* accuracy of the value returned by sysGetBusSpd().
* For a 267MHz Arthur system, running with a bus clock
* of 67 MHz, the actual speed returned is 268MHz.
*
* This function is dependent upon proper bus speed being
* returned via call to sysGetBusSpd().  In addition to this
* dependency, the function will only work for CPUs whose
* speed is dependent upon the bus speed and the value in
* the PLL bits of the HID1 register.  CPUs which HAVE this
* property are 602, Arthur, 604ev and 604r.  CPUs which
* do NOT HAVE this property (and for which this function
* will NOT work) are: 601, 603, 603e, 603p, and 604.
*
* RETURNS: The approximate CPU speed (in Hz).
*/

LOCAL UINT sysCalcMpuSpd (void)
    {
    UINT speed;
    UINT busMultiplier;

    busMultiplier = tableArthur[sysHid1Get() >> 28];
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
*/

STATUS sysVpdInit (void)
    {
    VPD_PACKET * pVpdPkt;
    UINT32	 idx;

    /* get the bus speed */

    sysBusSpeed = sysCalcBusSpd ();

    /* get the PCI bus speed */

    sysPciSpeed = sysCalcPciSpd ();

    /* get the CPU internal clock speed */

    sysCpuSpeed = sysCalcMpuSpd();

    /* read the vpd and parse data from the serial eeprom. */

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
*/

BOOL sysProdConfigGet
    (
    UINT32 optionId	/* option to test */
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


#ifdef  INCLUDE_DEC_END
/******************************************************************************
*
* sysEnetAddrGet - retrieve ethernet address.
*
* This routine returns a six-byte ethernet address for a given ethernet unit.
* The dec21x40End driver uses this routine to obtain the ethernet address if
* indicated by a user-flag in DEC_LOAD_STRING in configNet.h; or if the
* reading the ethernet address ROM is unsuccessful.
*
* RETURNS: OK if a valid address was found, otherwise it returns ERROR.
*/

STATUS sysEnetAddrGet
    (
    int         unit,		/* unit number */
    char *      enetAdrs	/* return variable */
    )
    {
    VPD_PACKET * pPkt;
    UINT32	 idx;

    /*
     * see if there is a valid ethernet address packet for the specified
     * unit in vpd.
     */

    if ( (sysVpdPktGet (VPD_PID_EA, unit, &sysVpdPkts[0], &pPkt) == OK) &&
         (pPkt->size >= 6) )
        {

        /* copy the address to the caller's data area. */

        for (idx = 0; idx < 6; idx++)
            enetAdrs[idx] = pPkt->data[idx];
        return (OK);
        }
    else

        /* no valid ethernet address packet found, return error indication. */

        return (ERROR);
    }
#endif  /* INCLUDE_DEC_END */
