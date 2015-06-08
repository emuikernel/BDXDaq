/* sysEnd.c - System enhanced network interface support library */

/* Copyright 1997-2002 Wind River Systems, Inc. */
/* Copyright 1999 Motorola, Inc. All Rights Reserved */

#include "copyright_wrs.h"

/*
modification history
--------------------
01d,30apr02,sbs  fixing compiler warnings
01c,26mar02,dtr  Removing compiler warnings.
01b,28mar01,pch  Remove references to DEC_USR_PHY_CHK, which is no longer
                 defined.
01a,19aug99,dmw  derived from sysEnd.c, 01g,17may99, MTXPlus.
*/

/*
.SH DESCRIPTION

This file contains the board-specific routines for the Motorola PowerPlus 
family with on-board Digital Semiconductor 21040, 21140 or 21143 Ethernet 
adapters. This file is required to update the END load string that the END 
driver utilizes for initial configuration. 
.LP

SEE ALSO: endLib
*/

#include "vxWorks.h"
#include "config.h"
#include "vmLib.h"
#include "stdio.h"
#include "drv/end/dec21x40End.h"

/* defines */

#define DEC_USR_21140   0x80000000      /* DEC 21140 part */
#define DEC_USR_21143   0x40000000      /* DEC 21143 part */

#define DEC_USR_FORCE_MODE 0x01000000	/* Force Phy via MII */
#define DEC_USR_100MB 0x02000000	/* Force 100 MB */

/* Special dec21143 configuration device driver area register */

#define PCI_CFG_21143_DA        0x40

#define END_LD_STR_SIZE 120
#define DRV_CTRL        DEC21X40_DRV_CTRL

#define MAX_DEV_ON_BUS	30	/* Could be up to 256 but that's unrealistic */

#ifndef NUM_ENET_UNITS
#   ifdef INCLUDE_SECONDARY_ENET
#       define NUM_ENET_UNITS  2
#   else
#       define NUM_ENET_UNITS  1
#   endif /* INCLUDE_SECONDARY_ENET */
#endif /* !NUM_ENET_UNITS */

/* DEC driver user flags */

#define DEC_USR_FLAGS_143 (DEC_USR_21143)
#define DEC_USR_FLAGS_140 (DEC_USR_BAR_RX | DEC_USR_RML | DEC_USR_CAL_08 | \
			   DEC_USR_PBL_04 | DEC_USR_21140 | DEC_USR_MII)
#define DEC_USR_FLAGS_040 (DEC_USR_BAR_RX | DEC_USR_RML | DEC_USR_CAL_08 | \
			   DEC_USR_PBL_04)


/* define for Auto-negoitiating link */

#define DEC_USR_MII_FLAGS (DEC_USR_MII_10MB | DEC_USR_MII_HD |             \
                           DEC_USR_MII_100MB | DEC_USR_MII_BUS_MON)


/* define for fixed 100Bit Full Duplex link */
/*
#define DEC_USR_MII_FLAGS (DEC_USR_MII_FD | DEC_USR_MII_100MB | \
                           DEC_USR_MII_NO_AN | DEC_USR_MII_BUS_MON )
*/

#define DEC_MII_PHY_ADDR 8      /* PHY's address on the MII bus */
 

/* forward declarations */

/* typedefs */
 
/* locals */

/*
 * this table may be customized by the user to force a
 * particular order how different technology abilities may be
 * negotiated by the PHY. Entries in this table may be freely combined
 * and even OR'd together.
 */
 
LOCAL INT16 motFccAnOrderTbl [] = {
                                MII_TECH_100BASE_TX,    /* 100Base-T */
                                MII_TECH_100BASE_T4,    /* 10Base-T */
                                MII_TECH_10BASE_T,      /* 100Base-T4 */
                                MII_TECH_100BASE_TX_FD, /* 100Base-T FD */
                                MII_TECH_10BASE_FD,     /* 10Base-T FD*/
                                -1                      /* end of table */
                               };



/* END load strings */
LOCAL char	endLoadStr[NUM_ENET_UNITS][END_LD_STR_SIZE];

/* imports */


IMPORT  END_TBL_ENTRY endDevTbl[];

/*******************************************************************************
*
* sysDec21x40UpdateLoadStr - Update the END load string after PCI Autoconfig
*
* This routine is to be called from sysHwInit() before the END initialization
* takes place. This routine will scan for the Ethernet devices.  If an Ethernet
* device is found, the device's configuration space is read, and the END load 
* string is composed from this information. The endDevTbl entry is then updated
* with a pointer to the updated load string, which resides in statically 
* allocated data local to this module.
*
* Note that this routine assumes that the entry for the END device is the
* first entry in the in the END device table (endDevTbl[]).
*
* RETURNS: N/A
*/
 
void sysDec21x40UpdateLoadStr (void)
    {
    int     pciBus    = 0;
    int     pciDevice = 0;
    int     pciFunc   = 0;
    int     ivec; 
    int     ilevel;
    UINT32  iobaseCsr;
    UINT8   intLine;
    UINT32  cpuPciAddr;
    UINT32  flags = 0;
    int	    unit = 0;
    UINT    devVend;

    /* Find the ethernet devices. Stop when END_TBL_END is incurred. */

    while (endDevTbl[unit].endLoadString != END_TBL_END)
        {

        pciConfigInLong (pciBus, pciDevice, pciFunc, PCI_CFG_VENDOR_ID, 
                         &devVend);

        if ((devVend == PCI_ID_LN_DEC21040) ||
            (devVend == PCI_ID_LN_DEC21140) ||
            (devVend == PCI_ID_LN_DEC21143))
            {

            /*
             * We must statically program the DEC device's PCI configuration
             * header at PRIMARY_ENET_ID in this non-pciAutoConfig BSP.
             */

            if (unit == PRIMARY_ENET_ID)
                {
                (void)pciDevConfig(pciBus, pciDevice, pciFunc,
                                   PCI_IO_LN_ADRS, 0,
                                   (PCI_CMD_MASTER_ENABLE | PCI_CMD_IO_ENABLE));

                intLine = INT_LVL_DC;

                pciConfigOutByte (pciBus, pciDevice, pciFunc,
                                  PCI_CFG_DEV_INT_LINE, intLine);
                }

#ifdef INCLUDE_SECONDARY_ENET
            /*
             *  Hard-code the Standard PCI Header of the Secondary Ethernet
             *  if present.
             */

            if (unit == SECONDARY_ENET_ID)
                {
                (void)pciDevConfig(pciBus, pciDevice, pciFunc,
                                   PCI_IO_LN2_ADRS, 0,
                                   (PCI_CMD_MASTER_ENABLE | PCI_CMD_IO_ENABLE));

                intLine = INT_LVL_DC1;

                pciConfigOutByte (pciBus, pciDevice, pciFunc,
                                  PCI_CFG_DEV_INT_LINE, intLine);
                }
#endif  /* INCLUDE_SECONDARY_ENET */
 
            /*
             * Get the PCI I/O base address - for the Digital 21x4x 
             * the first BAR is PCI I/O and the second is PCI Memory.
             */

            pciConfigInLong (pciBus, pciDevice, pciFunc,
                             PCI_CFG_BASE_ADDRESS_0, &iobaseCsr);

            cpuPciAddr = ((iobaseCsr & 0xfffffffe) | CPU_PCI_ISA_IO_ADRS);

            pciConfigInByte (pciBus, pciDevice, pciFunc,
                             PCI_CFG_DEV_INT_LINE, &intLine);

            ivec = (int)(intLine);
            ilevel = (int)(intLine);

            switch (devVend)
                {
                case PCI_ID_LN_DEC21040:
                    flags = DEC_USR_FLAGS_040;
                    break;
                case PCI_ID_LN_DEC21140:
                    flags = DEC_USR_FLAGS_140;
                    break;
                case PCI_ID_LN_DEC21143:
                    flags = DEC_USR_FLAGS_143;

                    /* If this is a dec21143, disable sleep mode */

                    pciConfigOutLong (pciBus, pciDevice, pciFunc,
                                      PCI_CFG_21143_DA, 0 );
                    break;
                default:
                    printf("Unsupported Ethernet device found\r\n");
                    break;
                }

            /*
             * create the config string. The format is:
             * 
             * <devAdrs>:<pciAdrs>:<ivec>:<inum>:<numrds>:<numtds>:
             * <membase>:<memsize>:<userflags>
             */

	    /*
            sprintf (endLoadStr[unit],"0x%x:0x%x:0x%x:0x%x:-1:-1:-1:0:0x%x",
                     cpuPciAddr, PCI2DRAM_BASE_ADRS, ivec, ilevel, flags);
	    */

            /* DJA 7/2003 Use expanded load string to control PHY */
            sprintf (endLoadStr[unit],"0x%x:0x%x:0x%x:0x%x:-1:-1:-1:0:0x%x:0x%x:0x%x:0x%x",
                     cpuPciAddr, PCI2DRAM_BASE_ADRS, ivec, ilevel, flags,
		     DEC_MII_PHY_ADDR,
		     motFccAnOrderTbl,
		     DEC_USR_MII_FLAGS);

            /* Update string and increment unit index */

            endDevTbl[unit].endLoadString = endLoadStr[unit++];

            }

        pciDevice++;

        if (pciDevice > MAX_DEV_ON_BUS)
            {
            pciDevice = 0;
            pciBus++;
            }
        }

    return;

    }
