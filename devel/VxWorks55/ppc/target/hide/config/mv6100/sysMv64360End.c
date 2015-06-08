/* sysMv64360End.c - system configuration for the MV64360 ethernet driver */

/* Copyright 2002-2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01c,31jan03,efb  Removed dependency on gei64360End.h
01b,29jan03,efb  Fixed sources of strict-mode compiler warnings.
01a,29aug02,efb  New file.
*/


/*

DESCRIPTION

This is the system level configuration module for the VxWorks MV64360 
ethernet driver.  It performs the dynamic parameterization
of the driver with board specific values. This allows board specific
values to be declared outside of the driver elsewhere in the BSP so 
that they can be managed or modified without recompiling driver source.

*/

/* includes */

#include "vxWorks.h"
#include "end.h"
#include "string.h"
#include "config.h"
#include "sysMotVpd.h"

#include "mv64360EnetUnit.h"

/* defines */

/* typedefs */

/* forward declarations */

END_OBJ *sysMv64360EndLoad (char *, void *);
void sysMv64360EndHwInit (void);
STATUS sysMv64360EndGetMacAddr (UCHAR *, UINT32);
UINT32 sysMv64360EndGetPhyAddr (UINT32);
UINT32 sysMv64360EndGetDescBaseAddr (UINT32);

/* external references */

IMPORT VPD_PACKET *sysVpdPkts[];      /* board vpd packets */

IMPORT END_OBJ *gei64360EndLoad (char *);
IMPORT STATUS sysMotVpdPktGet (UCHAR, UINT32, VPD_PACKET **, VPD_PACKET **);

/***************************************************************************    
*
* sysMv64360EndLoad - load the gei64360 device driver
*
* This routine loads the gei mv64360 device with initial parameters.
* The format of the driver END_LOAD_STRING passed in as paramStr is:
* "<memBase>:<memSize>:<nTfds>:<nRfds>:<flags>:<offset>"
*
* RETURNS: An END object upon success
* ERROR if driver could not be loaded.
*/

END_OBJ *sysMv64360EndLoad
    (
    char *paramStr,   /* Initialization parameter string */
    void *unused      /* Unused optional legacy argument */
    )
    {
    char     initStr[END_INIT_STR_MAX]; 
    END_OBJ *endObj = NULL;

    unused = NULL;       /* Prevents "unused" warning from compiler */

    if (strlen (paramStr) == 0)
        {

        /*
         * If the paramStr is zero length, this is the first call from the
         * mux layer, so the string is passed on to driver END load function
         * as is.
         */

        endObj = gei64360EndLoad (paramStr);
        
        }
    else
        {

        /*
         * This string will not be empty on the second call from the mux 
         * layer. It will contain the unit number as the first 
         * field. The rest of the parameter string is appended immediately
         * after the unit field. The completed string is then passed
         * down to the driver END load function to complete the load 
         * operation.
         */

        strcpy (initStr, paramStr); 
        strcat (initStr, "-1:0x00:0x40:0x40:0x00");

        endObj = gei64360EndLoad (initStr);
        }

    return (endObj);
    }

/***************************************************************************    
*
* sysMv64360EndHwInit - initialize the MV64360 ethernet unit
*
* This routine initializes all of the ethernet unit registers, including all 
* port and queue registers, to reset default values. Ethernet device window
* mappings are also set up. Unit interrupts (only) are connected and all
* are enabled. The attached PHYs are reset. This routine should be executed
* during system hardware initialization one time, before loading the 
* the associated END driver.
*
* RETURNS: N/A
*/

void sysMv64360EndHwInit (void)
    {
    mv64360EnetUnitInit ();

    mv64360EnetUnitPhyInit (PHY_ADDR_ENET_PORT0);
    mv64360EnetUnitPhyInit (PHY_ADDR_ENET_PORT1);

    mv64360EnetUnitIntEnable (GE_UNIT_INT_MASK);
    }

/***************************************************************************    
*
* sysMv64360EndGetMacAddr - retrieve the ethernet MAC address.
*
* This routine retrieves the MAC address for the specified port instance 
* from the previously initialized system VPD array. The MAC address is 
* copied to the macAddrDest buffer provided.
*
* RETURNS: OK upon success.
* ERROR if the destination parameter is null.
*/

STATUS sysMv64360EndGetMacAddr 
    (
    UCHAR  *macAddrDest,  /* destination buffer */
    UINT32  instance      /* the address to get if multiple ethernet ports */
    )
    {
    STATUS      status = OK;
    VPD_PACKET *etherVpd = NULL;

    if (macAddrDest != NULL)
        {
        if (sysMotVpdPktGet (VPD_PID_EA, instance, sysVpdPkts, &etherVpd) !=
            ERROR)
            {
            bcopy ((char *)etherVpd->data, (char *)macAddrDest, 
                   (int)etherVpd->size);
            }
        else
            {
 
            /* VPD get failed */

            status = ERROR;
            }
        }
    else 
        {

        /* Invalid parameter */

        status = ERROR;
        }

    return (status);
    }

/***************************************************************************    
*
* sysMv64360EndGetPhyAddr - retrieve the port PHY address.
*
* This routine retrieves the PHY address for the specified port instance.
*
* RETURNS: The PHY address.
* ERROR if the port instance is out of range.
*/

UINT32 sysMv64360EndGetPhyAddr 
    (
    UINT32  thePort      /* which ethernet port */
    )
    {
    UINT32 phyAddr = ERROR;

    switch (thePort)
        {
        case 0 :
            phyAddr = PHY_ADDR_ENET_PORT0;
            break;

        case 1 :
            phyAddr = PHY_ADDR_ENET_PORT1;
            break;

        case 2 :
            phyAddr = PHY_ADDR_ENET_PORT2;
            break;

        default :
            phyAddr = ERROR;
            break;
        }

    return (phyAddr);
    }

/***************************************************************************    
*
* sysMv64360EndGetDescBaseAddr - retrieve the descriptor base address.
*
* This routine retrieves the port's descriptor base address where its 
* transmit and receive descriptor lists will be placed.
*
* Currently a fixed memory space from a non-cacheable partition is 
* returned due to continuing coherency problems with the MV64260/MV64360
* ethernet DMA. If dynamic allocation becomes feasible in the future,
* this routine can be changed to support it without impact to the
* driver itself.
*
* RETURNS: The base address.
* ERROR if the port instance is out of range.
*/

UINT32 sysMv64360EndGetDescBaseAddr 
    (
    UINT32  thePort      /* which ethernet port */
    )
    {
    UINT32 baseAddr = ERROR;

    if (thePort < MAX_ENET_PORTS)
        {
        baseAddr = ETHER_MEM_START_ADDR + (ETHER_PORT_MEM_SIZE * thePort);
        }
    
    return (baseAddr);
    }
