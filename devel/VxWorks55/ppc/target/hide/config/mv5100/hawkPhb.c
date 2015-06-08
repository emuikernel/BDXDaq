/* hawkPhb.c - Hawk PCI-Host Bridge (PHB) chip initialization */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-2001 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01c,06nov01,kab  SPR #71519: Decrease Hawk bus timeout to prevent
                 hang on interrupted 64bit RMW cycle
01b,07mar01,rhk  WRS coding standards changes.
01a,13mar00,dmw  Created (from version 01a of mcpn765/hawkPhb.c).
*/

/*
DESCRIPTION
The following contains the initialization routines
for the Hawk PHB, a Host PCI Bridge/Memory Controller used in
Motorola's PowerPC based boards.
*/

/* includes */

/* defines */

#define HAWK_ADDR( reg )        ( HAWK_PHB_BASE_ADRS + reg )

#define HAWK_WRITE8( addr, data )  ( *((UINT8 *)(addr)) = data )

#define HAWK_WRITE16( addr, data )  ( *((UINT16 *)(addr)) = data )

#define HAWK_WRITE32( addr, data )  ( *((UINT32 *)(addr)) = data )

#define HAWK_READ8( addr, data )  ( data = *((UINT8 *)(addr)) )

#define HAWK_READ16( addr, data )  ( data = *((UINT16 *)(addr)) )

#define HAWK_READ32( addr, data )  ( data = *((UINT32 *)(addr)) )

#define MHZ	1000000

/* typedefs */

/* globals */

/* forward declarations */

/* externals */

IMPORT	UINT32  sysGetBusSpd(void);
IMPORT	BOOL	sysSysconAsserted (void);


/*******************************************************************************
*
* sysHawkPhbInit - initialize the Hawk PHB registers
*
* This function performs the first phase of the Hawk PPC-bus registers and
* sets up the CPU->PCI windows.
*
* RETURNS: N/A
*/

void sysHawkPhbInit (void)
    {

#ifdef DISABLE_SPRQ

    /* disable speculative PCI requests (interferes with 21554 arbiter) */

    UINT16 temp;

    HAWK_READ16(HAWK_ADDR(HAWK_MPC_HCSR), temp);
    temp &= ~(1<<(15-HAWK_MPC_HCSR_SPRQ_BIT));
    HAWK_WRITE16(HAWK_ADDR(HAWK_MPC_HCSR), temp);
    EIEIO_SYNC;

#endif /* DISABLE_SPRQ */

    /* 
     * Enable the MPIC and set bus timout to 64 uSecs. The 64 uSec
     * bus timeout value is necessary to prevent problems caused
     * by Universe II's premature removal of PCI #LOCK during
     * pending RMW operations while the Hawk is previously
     * awaiting bus timout. 64 uSecs ensures the Hawk will timeout
     * before the Universe can remove PCI #LOCK so the pending
     * locked transaction can be completed.
     */

    HAWK_WRITE16( HAWK_ADDR(HAWK_MPC_GCSR),
		  (HAWK_MPC_GCSR_OPIC_ENA | HAWK_MPC_GCSR_XBT_64) );
    EIEIO_SYNC;

    /*
     * Enable round-robin PCI bus arbitration, with PCI bus parked on Hawk,
     * PPC bus parked on CPU0 (only one on this board) and park the arbiter
     * at any PCI bus master which has successfully locked the PCI bus.
     */

    HAWK_WRITE32( HAWK_ADDR(HAWK_MPC_MARB), 0x00206041 );
    EIEIO_SYNC;

    HAWK_WRITE16( HAWK_ADDR(HAWK_MPC_MEREN), 0 );
    EIEIO_SYNC;

    HAWK_WRITE8( HAWK_ADDR(HAWK_MPC_MERST), 0xFF );
    EIEIO_SYNC;

    /* initially set the CPU->PCI attribute registers to disabled */

    HAWK_WRITE8( HAWK_ADDR(HAWK_MPC_MSATT0), 0 );
    EIEIO_SYNC;

    HAWK_WRITE8( HAWK_ADDR(HAWK_MPC_MSATT1), 0 );
    EIEIO_SYNC;

    HAWK_WRITE8( HAWK_ADDR(HAWK_MPC_MSATT2), 0 );
    EIEIO_SYNC;

    HAWK_WRITE8( HAWK_ADDR(HAWK_MPC_MSATT3), 0 );
    EIEIO_SYNC;

    /* Do all of the CPU to PCI window registers */

    /* MSADD0, MSOFF0, MSATT0 registers */

    HAWK_WRITE32( HAWK_ADDR(HAWK_MPC_MSADD0), 
		   ((CPU2PCI_ADDR0_START<<16) | CPU2PCI_ADDR0_END) );
    EIEIO_SYNC;

    HAWK_WRITE16( HAWK_ADDR(HAWK_MPC_MSOFF0), CPU2PCI_OFFSET0 );
    EIEIO_SYNC;

    HAWK_WRITE8( HAWK_ADDR(HAWK_MPC_MSATT0), CPU2PCI_MSATT0 );
    EIEIO_SYNC;

    /* MSADD1, MSOFF1, MSATT1 registers */

    HAWK_WRITE32( HAWK_ADDR(HAWK_MPC_MSADD1),
		   ((CPU2PCI_ADDR1_START<<16) | CPU2PCI_ADDR1_END) );
    EIEIO_SYNC;

    HAWK_WRITE16( HAWK_ADDR(HAWK_MPC_MSOFF1), CPU2PCI_OFFSET1 );
    EIEIO_SYNC;

    HAWK_WRITE8( HAWK_ADDR(HAWK_MPC_MSATT1), CPU2PCI_MSATT1 );
    EIEIO_SYNC;


    /* MSADD2, MSOFF2, MSATT2 registers */
    
    HAWK_WRITE32( HAWK_ADDR(HAWK_MPC_MSADD2),
                   ((CPU2PCI_ADDR2_START<<16) | CPU2PCI_ADDR2_END) );
    EIEIO_SYNC;

    HAWK_WRITE16( HAWK_ADDR(HAWK_MPC_MSOFF2), CPU2PCI_OFFSET2 );
    EIEIO_SYNC;

    HAWK_WRITE8( HAWK_ADDR(HAWK_MPC_MSATT2), CPU2PCI_MSATT2 );
    EIEIO_SYNC;

    /*
     * PCI address space 3 registers supports config. space access and
     * special cycle generation.  It should be configured for I/O space.
     */

    HAWK_WRITE32( HAWK_ADDR(HAWK_MPC_MSADD3),
                   ((CPU2PCI_ADDR3_START<<16) | CPU2PCI_ADDR3_END) );
    EIEIO_SYNC;

    HAWK_WRITE16( HAWK_ADDR(HAWK_MPC_MSOFF3), CPU2PCI_OFFSET3 );
    EIEIO_SYNC;

    HAWK_WRITE8( HAWK_ADDR(HAWK_MPC_MSATT3), CPU2PCI_MSATT3 );
    EIEIO_SYNC;

    }


/*******************************************************************************
*
* sysHawkPhbInit2 - initialize the Hawk PHB registers which require VPD
*
* This function performs the second phase of the Hawk PPC-bus registers.
* These registers require information contained in the VPD.
*
* RETURNS: N/A
*/

void sysHawkPhbInit2 (void)
    {
    HAWK_WRITE8( HAWK_ADDR(HAWK_MPC_PADJ), 
                256 - ((sysGetBusSpd () + (MHZ/2))/MHZ) );
    EIEIO_SYNC;
    }


/******************************************************************************
*
* sysHawkInitPhbExt - initialize the extended portion of the Hawk PHB PCI header
*
* This routine initializes the extended portion of the PCI header for the
* Motorola Hawk PCI-Host Bridge Controller (PHB).
*
* RETURNS: OK always
*/

STATUS sysHawkInitPhbExt
    (
    int busNo,          /* bus number */
    int deviceNo,       /* device number */
    int funcNo          /* function number */
    )
    {

    /*
     * Init Hawk's MPIC control register access addresses in I/O and
     * memory spaces:
     *
     * IOSPACE  = no access
     * MEMSPACE = MPIC_PCI_BASE_ADRS
     */

    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_0,
                      0x00000000);
    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_1,
                      MPIC_PCI_BASE_ADRS);

    /* initially disable all PCI slave decoders */

    pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSATT0, 0);
    pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSATT1, 0);
    pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSATT2, 0);
    pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSATT3, 0);

    /*
     *  Init Hawk's Slave decoders (range/offset/attributes)
     *
     *  These decoders map addresses on the PCI bus to the CPU for
     *  access to local DRAM.
     *
     *  Because hardware can read past real memory, it is necessary to disable
     *  Read Ahead for the last 64k of physical memory (DRAM).
     */



	/* Sergey */
    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSADD0,
                      PCI2CPU_ADDR0_RANGE);
    pciConfigOutWord (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSOFF0,
                      PCI2CPU_OFFSET0);



    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSADD1,
                      PCI2CPU_ADDR1_RANGE);
    pciConfigOutWord (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSOFF1,
                      PCI2CPU_OFFSET1);

    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSADD2,
                      PCI2CPU_ADDR2_RANGE);
    pciConfigOutWord (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSOFF2,
                      PCI2CPU_OFFSET2);

    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSADD3,
                      PCI2CPU_ADDR3_RANGE);
    pciConfigOutWord (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSOFF3,
                      PCI2CPU_OFFSET3);

    /* only enable PCI slave decoders if we are SYSCON */

    if ( sysSysconAsserted ())
        {
        pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSATT0,
                          PCI2CPU_ATT0);
        pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSATT1,
                          PCI2CPU_ATT1);
        pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSATT2,
                          PCI2CPU_ATT2);
        pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSATT3,
                          PCI2CPU_ATT3);
        }

    /*
     *  Enable Hawk's PCI master capability and MEM space
     *  (i.e., enable PCI space decoders).
     */

    pciConfigOutWord (busNo, deviceNo, funcNo, PCI_CFG_COMMAND, 0x0006);

    return(OK);
    }


/******************************************************************************
*
* sysHawkErrClr - Clear error conditions in Hawk
*
* This routine clears any existing errors in the Motorola Hawk PCI Host Bridge
* Controller.
*
* RETURNS: N/A
*/

void sysHawkErrClr (void)
    {
    /* Clear MPC Error Status register */

    sysOutByte ((HAWK_PHB_BASE_ADRS + HAWK_MPC_MERST), HAWK_MPC_MERST_CLR);

    /* get and clear Hawk PCI status reg */

    pciConfigOutWord (sysHawkPciBusNo, sysHawkPciDevNo, sysHawkPciFuncNo,
                      PCI_CFG_STATUS,
                      HAWK_PCI_CFG_STATUS_DPAR | HAWK_PCI_CFG_STATUS_SIGTA |
                      HAWK_PCI_CFG_STATUS_RCVTA | HAWK_PCI_CFG_STATUS_RCVMA |
                      HAWK_PCI_CFG_STATUS_SIGSE | HAWK_PCI_CFG_STATUS_RCVPE);
    }


