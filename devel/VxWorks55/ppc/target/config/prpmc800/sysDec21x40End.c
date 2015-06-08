/* sysDec21x40End.c - system configuration module for Dec21x40 END */
 
/* Copyright 1984 - 1999 Wind River Systems, Inc. */
/* Copyright 2001 Motorola, Inc. All Rights Reserved */
 
/*
modification history
--------------------
01h,07mar02,kab  SPR 70817: *EndLoad returns NULL on failure
01g,26nov01,dtr  Added low memory buffer and prevent attempted use of 
                 dec on slave.
01f,10oct01,scb  Support for shared memory.
01e,28sep01,srr  Added DEC_USR_FLAGS_140.
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
 

/* includes */

#include "vxWorks.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "end.h"
#include "config.h"
#include "drv/end/dec21x40End.h"	

#if (defined(INCLUDE_NETWORK) && defined (INCLUDE_END) && defined (INCLUDE_DEC_END))

/* defines */

/* DEC driver user flags */

#define DEC_USR_FLAGS_143 (DEC_USR_21143 | DEC_USR_MII)
#define DEC_USR_FLAGS_140 (DEC_USR_BAR_RX | DEC_USR_RML | DEC_USR_CAL_08 | \
			   DEC_USR_PBL_04 | DEC_USR_21140 | 0x00800000)
 
#define DEC_USR_MII_FLAGS (DEC_USR_MII_10MB | DEC_USR_MII_HD |             \
                           DEC_USR_MII_100MB | DEC_USR_MII_BUS_MON)
 
#define DEC_MII_PHY_ADDR 8      /* PHY's address on the MII bus */

 
 

/* Adapter-specific definitions */

#define NET_END_USER_FLAGS DEC_USR_FLAGS_143

/* forward declarations */

STATUS sysDec21143MediaSelect (DEC21X40_DRV_CTRL *, UINT *);
STATUS sysDec21x40DynEnetFind ( int, UINT, UINT, UINT *, UINT *);
STATUS sysDec21x40PciInit (void);    


char lowMemBufDec[0x4A000 + PPC_PAGE_SIZE];

/* locals */

LOCAL BOOL    sysDecPciInitdone=FALSE; 
LOCAL UINT32	membaseCsr;
LOCAL UINT32	iobaseCsr;
LOCAL UINT32	irqnum;
LOCAL UINT32	irqvec;

END_OBJ * sysDec21x40EndLoad
    (
    char * pParamStr,   /* ptr to initialization parameter string */
    void * unused       /* unused optional argument */
    );

/* imports */

IMPORT BOOL     sysMonarchMode;  /* TRUE if Monarch */
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
                         "0x%x:0x%x:0x%x:0x%x:";
    static char decParamTemplate2[] = "-1:-1:0x%x:0x%x:0x%x:8:0:0x%x";
    UINT32 pciBus;
    UINT32 pciDevice;
    UINT32 pciFunc;
    UINT32 devVend;
    pciBus = 0;
    pciDevice = PCI_IDSEL_SEC_LAN;
    pciFunc = 0;

    /* read PCI configuration and initialize endDevices[] */
    if(!sysMonarchMode)
       return((END_OBJ *)NULL);

    if (sysDecPciInitdone == FALSE)
        {
        sysDec21x40PciInit();
        sysDecPciInitdone = TRUE;
        }
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
        pciConfigInLong (pciBus, pciDevice, pciFunc,
                     PCI_CFG_VENDOR_ID, &devVend);
        	
	switch(devVend) {
        case PCI_ID_LN_DEC21040:
        case PCI_ID_LN_DEC21140:
	sprintf (cp, decParamTemplate, 
                  iobaseCsr,     /* device io base */
                  PCI_SLV_MEM_BUS, /* pciMemBase */
                  irqvec,                /* interrupt irq vector */
                  irqnum);                /* interrupt irq number */
        cp = paramStr + strlen (paramStr);
	sprintf (cp,decParamTemplate2,
		  (UINT32*)ENET_BUF_ALIGN (lowMemBufDec),
		  (UINT32)(sizeof(lowMemBufDec) - (ENET_BUF_ALIGN (lowMemBufDec) - (UINT32)(lowMemBufDec))),
		  (UINT32)DEC_USR_FLAGS_140,
                  /* default PHY flags */
                  MII_PHY_AUTO | DEC_USR_MII_10MB | DEC_USR_MII_HD |
                  DEC_USR_MII_100MB | DEC_USR_MII_FD |
                  DEC_USR_MII_BUS_MON);
	break;	
        case PCI_ID_LN_DEC21143:
	sprintf (cp, decParamTemplate, 
                  iobaseCsr,     /* device io base */
                  PCI_SLV_MEM_BUS, /* pciMemBase */
                  irqvec,                /* interrupt irq vector */
                  irqnum);                /* interrupt irq number */
        cp = paramStr + strlen (paramStr);
	sprintf (cp,decParamTemplate2,
		  (UINT32*)ENET_BUF_ALIGN (lowMemBufDec),
		  (UINT32)(sizeof(lowMemBufDec) - (ENET_BUF_ALIGN (lowMemBufDec) - (UINT32)(lowMemBufDec))),
		  (UINT32)DEC_USR_FLAGS_143,
                  /* default PHY flags */
                  MII_PHY_AUTO | DEC_USR_MII_10MB | DEC_USR_MII_HD |
                  DEC_USR_MII_100MB | DEC_USR_MII_FD |
                  DEC_USR_MII_BUS_MON);
        break;
	default:
	break;
	}
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
    UINT32 pciBus;
    UINT32 pciDevice;
    UINT32 pciFunc;
    UCHAR intLine;
    UINT32 devVend;

    pciBus = 0;
    pciDevice = PCI_IDSEL_SEC_LAN;
    pciFunc = 0;
    /* get memory base address and IO base address */

    pciConfigInLong (pciBus, pciDevice, pciFunc,
                            PCI_CFG_BASE_ADDRESS_0, &iobaseCsr);
    pciConfigInLong (pciBus, pciDevice, pciFunc,
                            PCI_CFG_BASE_ADDRESS_1, &membaseCsr);
    pciConfigInByte (pciBus, pciDevice, pciFunc,
                            PCI_CFG_DEV_INT_LINE, &intLine);

    iobaseCsr = TRANSLATE( (iobaseCsr & PCI_IOBASE_MASK),
                           PCI_MSTR_IO_BUS,PCI_MSTR_IO_LOCAL);
    
    membaseCsr =  TRANSLATE((membaseCsr & PCI_MEMBASE_MASK),
                            PCI_MSTR_MEMIO_BUS, 
                            PCI_MSTR_MEMIO_LOCAL);
    irqvec = (UINT32)intLine;
    irqnum = (UINT32)intLine;
    pciConfigInLong (pciBus, pciDevice, pciFunc,
                     PCI_CFG_VENDOR_ID, &devVend);


    if (devVend == PCI_ID_LN_DEC21143) {
    /* disable sleep mode */


    pciConfigOutByte (pciBus, pciDevice, pciFunc, PCI_CFG_MODE, 
		      SLEEP_MODE_DIS);

    pciConfigOutLong ( pciBus, pciDevice, pciFunc,
		       PCI_CFG_21143_DA, 0 );
    }



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
    int       unit,             /* input: unit number */
    UINT      instance,         /* input: instance number */
    UINT      bus,              /* input: bus number on which to look */
    UINT32 *    pDev,		/* output: device number found */
    UINT32 *    pFunc		/* output: function number found */
    )
    {

    UINT32  findClass;
    UINT32  pciBus;
    UINT32  devVend;
    UINT32  index;
    STATUS  status = ERROR;

    index = instance;

    findClass = (UINT)((PCI_CLASS_NETWORK_CTLR << 16)  |
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

            if (unit == 1) 
		{
		switch (devVend) 
		    {
		    case PCI_ID_LN_DEC21143:
	            case PCI_ID_LN_DEC21040:
        	    case PCI_ID_LN_DEC21140:
                        status = OK;
                        break;
		    default:
			break;
		    }
                }
            }
        index++;
        }
    return (status);
    }

#endif /* (defined(INCLUDE_NETWORK) && defined (INCLUDE_END)) */
