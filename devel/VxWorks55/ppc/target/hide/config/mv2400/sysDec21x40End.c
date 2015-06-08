/* sysDec21x40End.c - system configuration module for Dec21x40 END */
 
/* Copyright 1984-2002 Wind River Systems, Inc. */
 
/*
modification history
--------------------
01f,16apr02,dat  Removing warnings for T2.2 release
01e,07mar02,kab  SPR 70817: *EndLoad returns NULL on failure
01d,09nov99,cn   updated to use parameters for MII-comliant PHYs.
01c,01jul99,tm   adapted (simplified) for planar DEC 21143-based mv2400
01b,29apr99,jkf  merged with T2.
01a,01apr99,jkf  written 
*/


/*
DESCRIPTION
This is the WRS-supplied configuration module for the VxWorks 
dec21x40End (dc) END driver.  It performs the dynamic parameterization 
of the dec21x40End driver.  This technique of 'just-in-time' 
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
#include "drv/end/dec21x40End.h"	

/* defines */

/* DEC driver user flags */

#define DEC_USR_FLAGS_143 (DEC_USR_21143 | DEC_USR_MII)

/* define for Auto-negoitiating link */

#define DEC_USR_MII_FLAGS (DEC_USR_MII_10MB | DEC_USR_MII_HD |             \
                           DEC_USR_MII_100MB | DEC_USR_MII_BUS_MON)


/* define for fixed 100Bit Full Duplex link */
/*
#define DEC_USR_MII_FLAGS (DEC_USR_MII_FD | DEC_USR_MII_100MB | \
                           DEC_USR_MII_NO_AN | DEC_USR_MII_BUS_MON )
*/


 
#define DEC_MII_PHY_ADDR 8      /* PHY's address on the MII bus */
 
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
 

/* Adapter-specific definitions */

#define NET_END_USER_FLAGS DEC_USR_FLAGS_143

/* forward declarations */

STATUS sysDec21143MediaSelect (DEC21X40_DRV_CTRL *, UINT *);
STATUS sysDec21x40DynEnetFind ( int, INT32, INT32, INT32 *, INT32 *);
 
/* locals */
 
LOCAL UINT32	membaseCsr;
LOCAL UINT32	iobaseCsr;
LOCAL UINT32	irqnum;
LOCAL UINT32	irqvec;

/* imports */

IMPORT END_OBJ* dec21x40EndLoad (char *);

/******************************************************************************
*
* sysDec21x40EndLoad - create load string and load a dec21x40 (dc) device.
*
* This routine loads the dc device with initial parameters probed
* during dec21x40PciInit().
*
* RETURNS: pointer to END object or NULL.
*
* SEE ALSO: dec21x40EndLoad()
*/
 
END_OBJ * sysDec21x40EndLoad
    (
    char * pParamStr,   /* ptr to initialization parameter string */
    void * unused       /* unused optional argument */
    )
    {
    /*
     * The dec21x40End driver END_LOAD_STRING should be:
     * The format of the parameter string is:
     *
     * "<device_addr>:<PCI_addr>:<ivec>:<ilevel>:<num_rds>:<num_tds>:
     * <mem_base>:<mem_size>:<user_flags>:<phyAddr>:<pMiiPhyTbl>:<miiPhyFlags>"
     *
     * Note that dec21x40 unit number is prepended in muxDevLoad, so we 
     * don't put it here!
     */

    char * cp; 			
    char paramStr [END_INIT_STR_MAX];   	/* end.h */
    END_OBJ * pEnd;
    static char decParamTemplate[] = 
			"0x%x:0x%x:0x%x:0x%x:-1:-1:-1:0:0x%x:0x%x:0x%x:0x%x";

    if (strlen (pParamStr) == 0)
        {
        /* 
         * muxDevLoad() calls us twice.  If the string is
         * zero length, then this is the first time through
         * this routine, so we just return.
         */

        pEnd = dec21x40EndLoad (pParamStr);
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

        /* Now, we advance cp, by finding the end the string */

        cp += strlen (paramStr);
        
        /* finish off the initialization parameter string */

	sprintf (cp, decParamTemplate, 
                 (UINT) (iobaseCsr),     /* device io base */
                 (UINT) PCI_SLV_MEM_BUS, /* pciMemBase */
                  irqvec,                /* interrupt irq vector */
                  irqnum,                /* interrupt irq number */
		  NET_END_USER_FLAGS,
		  DEC_MII_PHY_ADDR,
		  motFccAnOrderTbl,
		  DEC_USR_MII_FLAGS);

        if ((pEnd = dec21x40EndLoad (paramStr)) == (END_OBJ *)NULL)
	    {
            printf ("Error: Dec21x4x device failed dec21x40EndLoad routine.\n");
	    }
	}

    return (pEnd);
    }

/*******************************************************************************
*
* sysDec21x40PciInit - prepare LAN adapter for DEC21X4X initialization
*
* This routine finds the PCI device, and maps its memory and IO address.
* It must be done prior to initializing the DEC21X4X, sysDec21x40Init().  
*
* RETURNS: N/A
*/

STATUS sysDec21x40PciInit (void)
    {
    BOOL                found = FALSE;
    INT32 pciBus;
    INT32 pciDevice;
    INT32 pciFunc;
    UCHAR intLine;

    pciBus = 0;

    /* EnetFind args: unit, instance, bus, pDev, pFunc */

    if (sysDec21x40DynEnetFind(0,0,pciBus,&pciDevice,&pciFunc) == OK)
	{
	/* board detected */

	found = TRUE;
	}

    if (!found || (pciDevice > PCI_MAX_DEV))
        return (ERROR);


    /* get memory base address and IO base address */

    pciConfigInLong (pciBus, pciDevice, pciFunc,
                            PCI_CFG_BASE_ADDRESS_0, &iobaseCsr);
    pciConfigInLong (pciBus, pciDevice, pciFunc,
                            PCI_CFG_BASE_ADDRESS_1, &membaseCsr);
    pciConfigInByte (pciBus, pciDevice, pciFunc,
                            PCI_CFG_DEV_INT_LINE, &intLine);

    iobaseCsr = (iobaseCsr & PCI_IOBASE_MASK) - PCI_MSTR_IO_BUS
                      + PCI_MSTR_IO_LOCAL;
	
    membaseCsr   &= PCI_MEMBASE_MASK;

    irqvec = (UINT32)intLine;
    irqnum = (UINT32)intLine;

    /* disable sleep mode */

    pciConfigOutByte (pciBus, pciDevice, pciFunc, PCI_CFG_MODE, 
		      SLEEP_MODE_DIS);

    pciConfigOutLong ( pciBus, pciDevice, pciFunc,
		       PCI_CFG_21143_DA, 0 );
    return (OK);
    }

/*******************************************************************************
*
* sysDec21x40Init - prepare LAN adapter for dec21X4X initialization
*
* This routine is expected to perform any adapter-specific or target-specific
* initialization that must be done prior to initializing the dec21X4X.
*
* The dec21X4X driver calls this routine from the driver endLoad routine before
* any other routines in this library.
*
* This routine determines the base address, the main memory address over 
* the PCI bus, the interrupt level and the interrupt vector parameter.
*
* RETURNS: OK or ERROR if the adapter could not be prepared for initialization.
*/
 
STATUS sysDec21x40Init
    (
    DEC21X40_DRV_CTRL * pDrvCtrl
    )
    {
    return (OK);
    }
 

/*******************************************************************************
*
* sysDec21x40EnetAddrGet - get Ethernet address
*
* This routine provides a target-specific interface for accessing a
* device Ethernet address.
*
* RETURNS: OK or ERROR if could not be obtained.
*/

STATUS  sysDec21x40EnetAddrGet
    (
    int         unit,
    char *      enetAdrs
    )
    {
    /*
     * There isn't a target-specific interface for accessing a
     * device Ethernet address.
     */

    return (ERROR);
    }

/*******************************************************************************
*
* sysDec21143MediaSelect - hook routine for dec21x40Start
*
* This is hook routine for dec21x40Start(). This routine selects a media for
* dec21143 board.
*
* RETURNS: OK, always.
*/
 
STATUS sysDec21143MediaSelect
    (
    DEC21X40_DRV_CTRL *      pDrvCtrl,   /* Driver control */
    UINT *                   pCsr6Val    /* CSR6 return value */
    )
    {
    ULONG *     csrReg;
    ULONG       csrData;

    /* set CSR6 value */
 
    *pCsr6Val |= CSR6_21140_MB1;

    /* Led/Control selection */
 
    csrReg   = (ULONG *)(pDrvCtrl->devAdrs + (CSR15 * DECPCI_REG_OFFSET));
    csrData  = (CSR15_21143_CWE | CSR15_21143_LG3 | CSR15_21143_LG1
                                                         | CSR15_MD_MSK);
    *csrReg  = PCISWAP (csrData);

    csrData = (CSR15_21143_LG3 | CSR15_21143_LG1 | CSR15_MODE_10);
    *csrReg  = PCISWAP (csrData);

    /* force 10Base-T half duplex configuration */
 
    /* set SIA registers */
 
    csrReg   = (ULONG *)(pDrvCtrl->devAdrs + (CSR14 * DECPCI_REG_OFFSET));
    csrData  = PCISWAP (*csrReg);
    csrData &= ~(CSR14_21143_T4 | CSR14_21143_TXF| CSR14_21143_TXH |
                        CSR14_21143_TAS | CSR14_21143_TH | CSR14_21143_ANE);
    *csrReg = PCISWAP(csrData);
 
    csrReg   = (ULONG *)(pDrvCtrl->devAdrs + (CSR13 * DECPCI_REG_OFFSET));
    csrData  = PCISWAP (*csrReg);
    csrData |= CSR13_SRL_SIA;
    *csrReg  = PCISWAP (csrData);
 
    return (OK);
    }

/*****************************************************************************
*
* sysDec21x40DynEnetFind - Find ethernet network device dynamically
*
* This function dynamically finds the ethernet network device.  The
* encoded location is returned the 'pciLoc' output parameter if the
* device is found.  The constraints of the search require that the
* device found be on the PCI bus number which matches the 'bus' input
* parameter.  In addition, if the 'unit' input parameter is zero, the
* device/vendor ID must match the PCI_ID_PRI_LAN define, and if the
* 'unit' parameter is nonzero, the device/vendor * ID must match the
* PCI_ID_SEC_LAN define.
*
* RETURNS: OK if ethernet device is found, ERROR otherwise
*/

STATUS sysDec21x40DynEnetFind
    (
    int      unit,      /* input: unit number */
    INT32    instance,  /* input: instance number */
    INT32    bus,       /* input: bus number on which to look */
    INT32 *  pDev,	/* output: device number found */
    INT32 *  pFunc	/* output: function number found */
    )
    {

    INT32  findClass;
    INT32  pciBus;
    UINT32  devVend;
    INT32  index;
    STATUS  status = ERROR;

    index = instance;

    findClass = (INT32)((PCI_CLASS_NETWORK_CTLR << 16)  |
                      (PCI_SUBCLASS_NET_ETHERNET << 8) |
                      (0 << 0) /* Prog I/F = 0 */
                      );

    while (pciFindClass(findClass, index,
           &pciBus, pDev, pFunc) == OK)
        {
        if (pciBus == bus)
            {
            pciConfigInLong (pciBus, *pDev, *pFunc,
                             PCI_CFG_VENDOR_ID, &devVend);

            if ( ((unit == 0) && (devVend == PCI_ID_PRI_LAN)) )
                {
                status = OK;
                break;
                }
            }
        index++;
        }
    return (status);
    }

#endif /* defined(INCLUDE_NETWORK) && defined (INCLUDE_END) */
