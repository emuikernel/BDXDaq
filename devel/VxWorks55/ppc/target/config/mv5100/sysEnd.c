/* sysEnd.c - system configuration module for END devices */
 
/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1999-2001 Motorola, Inc., All Rights Reserved */
 
/*
modification history
--------------------
01g,07mar02,kab  SPR 70817: *EndLoad returns NULL on failure
01f,25apr01,srr  Changed to global pciAutoConfigLib and added miiLib support.
01f,29mar01,pch  Remove references to DEC_USR_PHY_CHK, which is no longer
                 defined; make references to i82559 driver conditional
01e,08mar01,rhk  Additional code standard change.
01e,03nov00,rcs  fix path to pciAutoConfigLib.h
01d,16aug00,dmw  Wind River coding standards review.
01c,12apr00,dmw  Fixed the ability to mix End devices. 
01b,03apr00,dmw  Included "latest" ./pci/pciAutoConfigLib.h.
01a,13mar00,dmw  Written (from version 01b of mcpn765/sysDec21x40End.c).
*/


/*
DESCRIPTION
This is the WRS-supplied configuration module for the VxWorks 
END drivers.  It performs the dynamic parameterization 
specific to the End drivers.  This technique of 'just-in-time' 
parameterization allows driver parameter values to be declared as 
any other defined constants rather than as static strings. 
*/
 
#if (defined(INCLUDE_NETWORK) && defined (INCLUDE_END))

/* includes */

#include "vxWorks.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "end.h"
#include "config.h"

#ifdef INCLUDE_DEC_END
#   include "drv/end/dec21x40End.h"	
#endif /* INCLUDE_DEC_END */

#ifdef INCLUDE_FEI_END
#   include "drv/end/fei82557End.h"	
#endif /* INCLUDE_FEI_END */

#include "drv/pci/pciAutoConfigLib.h"
#include "drv/pci/pciConfigLib.h"

/* defines */

#ifdef INCLUDE_DEC_END
/* DEC driver user flags */

#   define DEC_USR_FLAGS_143 (DEC_USR_21143 | DEC_USR_MII)
#   define DEC_USR_FLAGS_140 (DEC_USR_BAR_RX | DEC_USR_RML | DEC_USR_CAL_08 | \
			   DEC_USR_PBL_04 | DEC_USR_21140)

/* Adapter-specific definitions */

#   define NET_END_USER_FLAGS DEC_USR_FLAGS_143
#endif /* INCLUDE_DEC_END */

#ifdef INCLUDE_SECONDARY_ENET
#   define NUM_END_DEVICES		2 
#else
#   define NUM_END_DEVICES		1
#endif /* INCLUDE_SECONDARY_ENET */ 

/* forward declarations */

STATUS sysPciInit (void);


/* locals */

typedef struct endInfo 
    {
    PCI_ID  pciId;
    UINT32	bar0Csr;
    UINT32	bar1Csr;
    UINT8	irqnum;
    UINT8	irqvec;
    } endInfoType;
 
LOCAL endInfoType endDevices[NUM_END_DEVICES]; 
LOCAL char dbgMsg[100];
LOCAL initUnit = 0;	/* initialization unit - not sent by end interface */

/* imports */

#ifdef INCLUDE_DEC_END
IMPORT END_OBJ* dec21x40EndLoad (char *);
#endif /* INCLUDE_DEC_END */

#ifdef INCLUDE_MCG_END
IMPORT END_OBJ* i82559DrvEndLoad (char *);
#endif /* INCLUDE_MCG_END */

#ifdef INCLUDE_FEI_END
IMPORT END_OBJ* fei82557EndLoad (char *);
#endif /* INCLUDE_FEI_END */

IMPORT void sysDebugMsg (char * str, UINT32  recovery);

/******************************************************************************
*
* sysEndLoad - create load string and load the END devices.
*
* This routine loads the END devices with initial parameters.
*
* RETURNS: pointer to END object or NULL.
*
* SEE ALSO: driver xxxEndLoad() functions.
*/
 
END_OBJ * sysEndLoad
    (
    char * pParamStr,   /* ptr to initialization parameter string */
    void * unused       /* unused optional argument */
    )
    {

    /*
     * The End driver END_LOAD_STRING should be:
     * The format of the parameter string is:
     *
     * "<device_addr>:<PCI_addr>:<ivec>:<ilevel>:<num_rds>:<num_tds>:
     * <mem_base>:<mem_size>:<user_flags>"
     *
     * Note that unit number is prepended in muxDevLoad, so we 
     * don't put it here!
     */

    char * cp; 			
    char paramStr [END_INIT_STR_MAX];   	/* end.h */
    END_OBJ * pEnd = (END_OBJ *)NULL;
    UINT32  unit = 0;
    char unitString[3];

    /* read PCI configuration and initialize endDevices[] */

    sysPciInit ();

    if (strlen (pParamStr) == 0)
        {

        /* 
         * muxDevLoad() calls us twice.  If the string is
         * zero length, then this is the first time through
         * this routine, so we just return.
         */

        switch (endDevices[initUnit].pciId.devVend)
            {
            case PCI_ID_I82559:
            case PCI_ID_I82559ER:
#ifdef INCLUDE_FEI_END
                pEnd = fei82557EndLoad (pParamStr);
                sysDebugMsg("Selected fei end driver\n", 0);
                break;
#endif /* INCLUDE_FEI_END */

#ifdef INCLUDE_MCG_END
                pEnd = i82559DrvEndLoad (pParamStr);
                sysDebugMsg("Selected mcg end driver\n", 0);
                break;
#endif /* INCLUDE_MCG_END */

#ifdef INCLUDE_DEC_END
            case PCI_ID_LN_DEC21040:
            case PCI_ID_LN_DEC21140:
            case PCI_ID_LN_DEC21143:
                pEnd = dec21x40EndLoad (pParamStr);
                break;
#endif /* INCLUDE_DEC_END */
            default:
                sprintf(dbgMsg, "Unknown End device %x unit %d\r\n",
                       endDevices[initUnit].pciId.devVend, initUnit);
                sysDebugMsg(dbgMsg, 0);
                return ((END_OBJ *)NULL);
            }
            initUnit++;

            /* never go past the end of the list */

            if (initUnit == NUM_END_DEVICES)
                initUnit = 0;
        }
    else
        {

        /*
         * On the second pass though here, we actually create 
         * the initialization parameter string on the fly.   
         * Note that we will be handed our unit number on the 
         * second pass through and we need to preserve that information.
         * So we use the unit number handed from the input string.
         */

        cp = strcpy (paramStr, pParamStr); /* cp points to paramStr */

        unitString[0] = paramStr[0];
        unitString[1] = '\0';

        unit = atoi(unitString);

        /* Now, we advance cp, by finding the end the string */

        cp += strlen (paramStr);
        
        /* finish off the initialization parameter string */

        switch (endDevices[unit].pciId.devVend)
            {
            case PCI_ID_I82559:
            case PCI_ID_I82559ER:
#ifdef INCLUDE_FEI_END
                sprintf (cp, "%d:%d:0x%x:0x%x:0x%x:0x%x",
		         -1,
		         -1,
		         0x4,
			 0x4,
		         0x0,
		         0x0);

                if ((pEnd = fei82557EndLoad (paramStr)) == (END_OBJ *)NULL)
                    {
                    sprintf(dbgMsg, "Failed fei82557EndLoad.\r\n");
                    sysDebugMsg(dbgMsg, 0);
                    return ((END_OBJ *)NULL);
                    }
                break;
#endif /* INCLUDE_FEI_END */

#ifdef INCLUDE_MCG_END
                sprintf (cp, "%d:%d:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x", 
                         unit,
                         0,
                         endDevices[unit].pciId.loc.bus,
                         endDevices[unit].pciId.loc.device,
                         endDevices[unit].pciId.loc.function,
                         endDevices[unit].irqvec,
                         endDevices[unit].irqnum,
                         endDevices[unit].bar1Csr, /* device io base */
                         (UINT) PCI_SLV_MEM_BUS,   /* pciMemBase */
                         0); /* user flags */

                if ((pEnd = i82559DrvEndLoad (paramStr)) == (END_OBJ *)NULL)
                    {
                    sprintf(dbgMsg, "Failed i82559DrvEndLoad.\r\n");
                    sysDebugMsg(dbgMsg, 0);
                    return ((END_OBJ *)NULL);
                    }
                break;
#endif /* INCLUDE_MCG_END */

#ifdef INCLUDE_DEC_END
            case PCI_ID_LN_DEC21040:
            case PCI_ID_LN_DEC21140:
	            sprintf (cp, 
                         "0x%x:0x%x:0x%x:0x%x:-1:-1:-1:0:0x%x:0x%x:0x%x:0x%x",
		                 endDevices[unit].bar0Csr, PCI_SLV_MEM_BUS, 
                         endDevices[unit].irqvec, endDevices[unit].irqnum, 
		                 DEC_USR_FLAGS_140,
                         8, /* PHY address - keeps driver from hunting */
                         0, /* phy negotiation table */
                         
                         /* default PHY flags */

                         MII_PHY_AUTO | DEC_USR_MII_10MB | DEC_USR_MII_HD |
                         DEC_USR_MII_100MB | DEC_USR_MII_FD |
                         DEC_USR_MII_BUS_MON);

                if ((pEnd = dec21x40EndLoad (paramStr)) == (END_OBJ *)NULL)
                    {
                    sprintf(dbgMsg, "Failed dec21x40EndLoad.\r\n");
                    sysDebugMsg(dbgMsg, 0);
                    return ((END_OBJ *)NULL);
                    }

                break;

            case PCI_ID_LN_DEC21143:
	            sprintf (cp, 
                         "0x%x:0x%x:0x%x:0x%x:-1:-1:-1:0:0x%x:0x%x:0x%x:0x%x",
		                 endDevices[unit].bar0Csr, PCI_SLV_MEM_BUS, 
                         endDevices[unit].irqvec, endDevices[unit].irqnum, 
		                 DEC_USR_FLAGS_143,
                         8, /* PHY address - keeps driver from hunting */
                         0, /* phy negotiation table */

                         /* default PHY flags */

                         MII_PHY_AUTO | DEC_USR_MII_10MB | DEC_USR_MII_HD |
                         DEC_USR_MII_100MB | DEC_USR_MII_FD |
                         DEC_USR_MII_BUS_MON);

                if ((pEnd = dec21x40EndLoad (paramStr)) == (END_OBJ *)NULL)
                    {
                    sprintf(dbgMsg, "Failed dec21x40EndLoad.\r\n");
                    sysDebugMsg(dbgMsg, 0);
                    return ((END_OBJ *)NULL);
                    }

                break;
#endif /* INCLUDE_DEC_END */

            default:
                sprintf(dbgMsg, "Unknown End device %x unit %d\r\n",
                       endDevices[unit].pciId.devVend, unit);
                sysDebugMsg(dbgMsg, 0);
                return ((END_OBJ *)NULL);
            }
        }

    return (pEnd);
    }


/*******************************************************************************
*
* sysPciInit - prepare LAN adapters for initialization
*
* This routine finds the PCI device, and maps its memory and IO address.
*
* RETURNS: N/A
*/

STATUS sysPciInit (void)
    {
    UINT32 pciBus;
    UINT32 unit = 0;

    endDevices[0].pciId.loc.device = PCI_IDSEL_PRI_LAN;

    /* limit search to bus 0 */

    pciBus = 0;

#ifdef INCLUDE_SECONDARY_ENET
    for (;unit < NUM_END_DEVICES; unit++)
        {
        if (unit)
            endDevices[unit].pciId.loc.device = PCI_IDSEL_SEC_LAN;

#endif /* INCLUDE_SECONDARY_ENET */

        endDevices[unit].pciId.loc.bus = pciBus;

        pciConfigInLong (pciBus, endDevices[unit].pciId.loc.device, 
                         0, PCI_CFG_VENDOR_ID, 
                         &(endDevices[unit].pciId.devVend));

        /* get memory base address, IO base address, and interrupt line */

        pciConfigInLong (endDevices[unit].pciId.loc.bus, 
                         endDevices[unit].pciId.loc.device, 
                         endDevices[unit].pciId.loc.function,
                         PCI_CFG_BASE_ADDRESS_0, &(endDevices[unit].bar0Csr));
        pciConfigInLong (endDevices[unit].pciId.loc.bus, 
                         endDevices[unit].pciId.loc.device, 
                         endDevices[unit].pciId.loc.function,
                         PCI_CFG_BASE_ADDRESS_1, &(endDevices[unit].bar1Csr));
        pciConfigInByte (endDevices[unit].pciId.loc.bus, 
                         endDevices[unit].pciId.loc.device, 
                         endDevices[unit].pciId.loc.function,
                         PCI_CFG_DEV_INT_LINE, &(endDevices[unit].irqvec));

        endDevices[unit].irqnum = endDevices[unit].irqvec;

        switch (endDevices[unit].pciId.devVend)
            {
            case PCI_ID_I82559:
            case PCI_ID_I82559ER:
                /* convert Memory base addresses from PCI space to CPU space */

                endDevices[unit].bar0Csr = TRANSLATE(
                                            (endDevices[unit].bar0Csr & 
                                             PCI_MEMBASE_MASK),
                                             PCI_MSTR_MEMIO_BUS, 
                                             PCI_MSTR_MEMIO_LOCAL);

                /* convert I/O base addresses from PCI space to CPU space */

                endDevices[unit].bar1Csr = TRANSLATE(
                                             (endDevices[unit].bar1Csr & 
                                             PCI_IOBASE_MASK),
                                             PCI_MSTR_IO_BUS,PCI_MSTR_IO_LOCAL);
                break;

#ifdef INCLUDE_DEC_END
            case PCI_ID_LN_DEC21040:
            case PCI_ID_LN_DEC21140:
            case PCI_ID_LN_DEC21143: 

                /* convert I/O and Memory base from PCI space to CPU space */

                endDevices[unit].bar0Csr = TRANSLATE(
                                           (endDevices[unit].bar0Csr & 
                                           PCI_IOBASE_MASK),
                                           PCI_MSTR_IO_BUS,PCI_MSTR_IO_LOCAL);

                endDevices[unit].bar1Csr = TRANSLATE(
                                           (endDevices[unit].bar1Csr & 
                                           PCI_MEMBASE_MASK),
                                           PCI_MSTR_MEMIO_BUS, 
                                           PCI_MSTR_MEMIO_LOCAL);

                if (endDevices[unit].pciId.devVend == PCI_ID_LN_DEC21143)
                    { 
                    /* disable sleep mode */

                    pciConfigOutByte (endDevices[unit].pciId.loc.bus, 
                                      endDevices[unit].pciId.loc.device, 
                                      endDevices[unit].pciId.loc.function, 
                                      PCI_CFG_MODE, 
                                      SLEEP_MODE_DIS);

                    pciConfigOutLong (endDevices[unit].pciId.loc.bus,
                                      endDevices[unit].pciId.loc.device,
                                      endDevices[unit].pciId.loc.function, 
                                      PCI_CFG_21143_DA, 0 );
                    }
                break;
#endif /* INCLUDE_DEC_END */

            default:
                sprintf(dbgMsg, "Unknown End device %x unit %d\r\n",
                       endDevices[unit].pciId.devVend, unit);
                sysDebugMsg(dbgMsg, 0);
                break;
            }

#ifdef INCLUDE_SECONDARY_ENET
        }
#endif /* INCLUDE_SECONDARY_ENET */

    return (OK);
    }


#ifdef INCLUDE_DEC_END
/*******************************************************************************
*
* sysDec21x40EnetAddrGet - gets the ethernet address from the ROM register
*
* This routine returns ERROR.  It is legacy and should never be called.
*
* RETURNS: ERROR
*/

STATUS sysDec21x40EnetAddrGet (void)
    {
    return (ERROR);
    }
#endif /* INCLUDE_DEC_END */

#endif /* defined(INCLUDE_NETWORK) && defined (INCLUDE_END) */
