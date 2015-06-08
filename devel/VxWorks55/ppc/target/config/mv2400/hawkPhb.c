/* hawkPhb.c - Hawk PCI-Host Bridge (PHB) chip initialization */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/* Copyright 1996-1999 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01e,09jul99,rhv  Created #defines for Hawk configuration constants in mv2400.h.
01d,30jun99,rhv  Incorporating WRS code review changes.
01c,04feb99,rhv  Changing PCI arbiter configuration.
01b,29jan99,rhv  Splitting PHB init into 2 phases.
01a,20jan99,rhv  Created (derived from Mesquite/Sitka ravenI04febnit.c 01a)
*/

/*
DESCRIPTION
The following contains the initialization routines
for the Hawk PHB, a Host PCI Bridge/Memory Controller used in
Motorola's PowerPC based boards.

.CS
   Initialize the HAWK PHB registers.
   notes:

   1. For the standard vxWorks configuration the CPU to
      PCI mapping registers are initialized to the PReP
      model with some additions:

   CPU Address Range   PCI Address Range      Definition
   -----------------   -----------------   -----------------
   80000000 BF7FFFFF   00000000 3F7FFFFF   ISA/PCI I/O space
   C0000000 FCFFFFFF   00000000 3CFFFFFF   ISA/PCI Mem space w/MPIC
   FD000000 FDFFFFFF   00000000 00FFFFFF   ISA/PCI Mem space
   FE000000 FE7FFFFF   00000000 007FFFFF   ISA/PCI I/O space

   2. These assignments do not include the entire PReP PCI
      address space, this is due to the conflicting local
      resources of the H/W.

   3. When EXTENDED_VME is defined the mapping is as follows:

   CPU Address Range   PCI Address Range      Definition
   -----------------   ------------------  -----------------
   10000000 FBFFFFFF   10000000 FBFFFFFF   VME address space (1)
   FC000000 FCFFFFFF   FC000000 FCFFFFFF   MPIC/Reg space
   FD000000 FDFFFFFF   FD000000 FDFFFFFF   ISA/PCI Memory space
   FE000000 FE7FFFFF   00000000 007FFFFF   ISA/PCI I/O space

   Note 1, the starting address is controlled by the macro
   VME_A32_MSTR_LOCAL
.CE

*/

/* includes */

/* defines */

#define HAWK_ADDR( reg )        ( HAWK_PHB_BASE_ADRS + reg )

#define HAWK_WRITE8( writeAddr, data )  ( *(UINT8 *)(writeAddr) = data )

#define HAWK_WRITE16( writeAddr, data )  ( *(UINT16 *)(writeAddr) = data )

#define HAWK_WRITE32( writeAddr, data )  ( *(UINT32 *)(writeAddr) = data )

/* typedefs */

/* globals */

/* forward declarations */

/* externals */

UINT32 sysGetBusSpd(void);

/*******************************************************************************
*
* hawkPhbInit - initialize the Hawk PHB registers
*
* This function performs the first phase of the Hawk PPC-bus registers and
* sets up the CPU->PCI windows.
*
* RETURNS: N/A
*/

void hawkPhbInit (void)
    {

    /* make sure MPIC is enabled */

    HAWK_WRITE16( HAWK_ADDR(HAWK_MPC_GCSR), HAWK_MPC_GCSR_OPIC_ENA);
    EIEIO_SYNC;

    /* Configure the hawk 60x and PCI bus arbiters. */

    HAWK_WRITE32( HAWK_ADDR(HAWK_MPC_MARB), HAWK_MPC_MARB_VAL);
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
* hawkPhbInit2 - initialize the Hawk PHB registers which require VPD
*
* This function performs the second phase of the Hawk PPC-bus registers.
* These registers require information contained in the VPD.
*
* RETURNS: N/A
*/

void hawkPhbInit2 (void)
    {
    HAWK_WRITE8( HAWK_ADDR(HAWK_MPC_PADJ), 
                ( 256 - (sysGetBusSpd () + 500000)/1000000) );
    EIEIO_SYNC;
    }

/******************************************************************************
*
* hawkInitPhbExt - initialize the extended portion of the Hawk PHB PCI header
*
* This routine initializes the extended portion of the PCI header for the
* Motorola Hawk PCI-Host Bridge Controller (PHB).
*
* RETURNS: OK, always
*
*/

STATUS hawkInitPhbExt
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
     * IOSPACE  - 0x00000000  [no access]
     * MEMSPACE - 0x3C000000  [MPIC_PCI_BASE_ADRS]
     */

    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_0,
                      0x00000000);
    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_1,
                      MPIC_PCI_BASE_ADRS);

    /*
     *  Init Hawk's Slave decoders (range/offset/attributes)
     *
     *  These decoders map addresses on the PCI bus to the CPU for
     *  access to local DRAM.
     *
     *  Because hardware can read past real memory, it is necessary to disable
     *  Read Ahead for the last 64k of physical memory (DRAM).
     */

    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSADD0,
                      PCI2CPU_ADDR0_RANGE);
    pciConfigOutWord (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSOFF0,
                      PCI2CPU_OFFSET0);
    pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSATT0,
                      PCI2CPU_ATT0);

    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSADD1,
                      PCI2CPU_ADDR1_RANGE);
    pciConfigOutWord (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSOFF1,
                      PCI2CPU_OFFSET1);
    pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSATT1,
                      PCI2CPU_ATT1);

    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSADD2,
                      PCI2CPU_ADDR2_RANGE);
    pciConfigOutWord (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSOFF2,
                      PCI2CPU_OFFSET2);
    pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSATT2,
                      PCI2CPU_ATT2);

    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSADD3,
                      PCI2CPU_ADDR3_RANGE);
    pciConfigOutWord (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSOFF3,
                      PCI2CPU_OFFSET3);
    pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_HAWK_PSATT3,
                      PCI2CPU_ATT3);

    /*
     *  Enable Hawk's PCI master capability and MEM space
     *  (i.e., enable PCI space decoders).
     */
    pciConfigOutWord (busNo, deviceNo, funcNo, PCI_CFG_COMMAND,
                      PCI_CMD_MEM_ENABLE | PCI_CMD_MASTER_ENABLE);

    return(OK);
    }

/******************************************************************************
*
* hawkErrClr - Clear error conditions in Hawk
*
* This routine clears any existing errors in the Motorola Hawk PCI Host Bridge
* Controller.
*
* RETURNS: N/A
*/

void hawkErrClr (void)
    {
    /* Clear MPC Error Status register */

    sysOutByte ((HAWK_PHB_BASE_ADRS + HAWK_MPC_MERST), HAWK_MPC_MERST_CLR);

    /* get and clear Hawk PCI status reg */

    pciConfigOutWord (hawkPciBusNo, hawkPciDevNo, hawkPciFuncNo, PCI_CFG_STATUS,
                      HAWK_PCI_CFG_STATUS_DPAR | HAWK_PCI_CFG_STATUS_SIGTA |
                      HAWK_PCI_CFG_STATUS_RCVTA | HAWK_PCI_CFG_STATUS_RCVMA |
                      HAWK_PCI_CFG_STATUS_SIGSE | HAWK_PCI_CFG_STATUS_RCVPE);
    }


