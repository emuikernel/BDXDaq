/* harrierPhb.c - Harrier PCI-Host Bridge (PHB) chip initialization */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-2001 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01n,16nov01,scb  Map outbound 2 to local PCI bus address zero.
01m,10oct01,scb  Fix PCI memory mapping in preparation for shared memory.
01l,11jul01,scb  Slave inbound 0 maps only low-order DRAM (not all DRAM).
01k,11aug01,scb  PPC Abiter register and misc. ctl & stat reg mods.
01j,12jun01,srr  Don't enable memory access if running as a slave.
01i,08jun01,srr  Removed unneeded parameters.
01h,07dec00,krp  Added support for Watchdog Timer
01g,17nov00,dmw  Added slave Ethernet support.
01f,14nov00,dmw  Added inbound/outbound PCI windows for Monarch/Slave.
01e,27oct00,dmw  Added Xport windows and attributes.
01d,16oct00,dmw  Fixed bridge initialization.
01c,08oct00,dmw  Stubbed sysHarrierInitPhbExt, handled by romInit.
01b,12sep00,dmw  Removed byte swapping for 60x bus.
01a,31aug00,dmw  Written (from version 01b of mpcn765/hawkPhb.c).
*/

/*
DESCRIPTION
The following contains the initialization routines
for the Harrier PHB, a Host PCI Bridge/Memory Controller used in
Motorola's PowerPC based boards.
*/

/* includes */

#include "harrier.h"


/* defines */

#define MHZ	1000000
#define SHORTSWAP(x) (((x & 0xFF00) >> 8) | ((x & 0x00FF) << 8))

/* typedefs */

/* globals */

/* forward declarations */

/* externals */

IMPORT	UINT32  sysGetBusSpd(void);
IMPORT	BOOL	sysSysconAsserted (void);


/*******************************************************************************
*
* sysHarrierPhbInit - initialize the Harrier PHB registers
*
* This function performs the first phase of the Harrier PPC-bus registers and
* sets up the CPU->PCI windows.
*
* RETURNS: N/A
*/

void
sysHarrierPhbInit (void)
{
#ifdef PCI_AUTO_DEBUG
  char txt[100];
#endif

  /* 
   * If the PCI configuration cycles hold off bit is clear, we've already
   * configured the bridge.  Just return. 
   */

  if ((*(UINT32 *)HARRIER_BRIDGE_PCI_CONTROLSTATUS_REG & 
      HARRIER_BPCS_XCSR_CSH) == 0)
  {
#ifdef PCI_AUTO_DEBUG
    sprintf(txt,"sysHarrierPhbInit: bridge configured: reg 0x%08x=0x%08x\r\n",
      (UINT32 *)HARRIER_BRIDGE_PCI_CONTROLSTATUS_REG,
      *(UINT32 *)HARRIER_BRIDGE_PCI_CONTROLSTATUS_REG);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
    return;
  }
  else
  {
    ;
#ifdef PCI_AUTO_DEBUG
    sprintf(txt,"sysHarrierPhbInit: configuring: reg 0x%08x=0x%08x\r\n",
      (UINT32 *)HARRIER_BRIDGE_PCI_CONTROLSTATUS_REG,
      *(UINT32 *)HARRIER_BRIDGE_PCI_CONTROLSTATUS_REG);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif    
  }

  /* Disable the PCI Bridge */

  *(UINT16 *)HARRIER_PHB_COMMAND_REG &= ~(HARRIER_CMMD_MTSR |
                                          HARRIER_CMMD_MEMSP |
                                          HARRIER_CMMD_IOSP);
  EIEIO_SYNC;

  *(UINT32 *)HARRIER_BRIDGE_PCI_CONTROLSTATUS_REG = HARRIER_BPCS_XCSR_CSH;

  EIEIO_SYNC;

  /* Set Read-Ahead Sync Flush, Store-Gather Sync Flush, Copyback Snarfing */

  *(UINT16 *)HARRIER_BRIDGE_PPC_CONTROLSTATUS_REG |= HARRIER_BXCS_RSF |
                                                     HARRIER_BXCS_SSF |
                                                     HARRIER_BXCS_CSE;
  EIEIO_SYNC;

    /*
     * set the PCI Arbiter.  the default will be:
     *      Priority="Round Robin"
     *      Parking Scheme="Park on last master"
     *      Enable="Enabled"
     */

  *(UINT16 *)HARRIER_PCI_ARBITER_REG = HARRIER_PARB_PRI_ROUNDROBIN |
                                         HARRIER_PARB_ENABLE;

  EIEIO_SYNC;

    /*
     * set the PPC Arbiter.  the default will be:
     *      Parking Scheme="Park on last master"
     *      Enable="Enabled"
     */

  *(UINT16 *)HARRIER_PPC_ARBITER_REG = HARRIER_XARB_PRK_LASTCPU |
                                         HARRIER_XARB_ENABLE;

  EIEIO_SYNC;

  /* Setup Vendor ID and subsystem ID */

  *(UINT16 *)HARRIER_SUBSYSTEM_VENDORID_REG = 
	       SHORTSWAP(HARRIER_SUB_VNDR_ID_VAL);
  *(UINT16 *)HARRIER_SUBSYSTEM_ID_REG = 
	       SHORTSWAP(HARRIER_SUB_SYS_ID_VAL);

  /* clear the Outbound Translation Address Registers 0, 1, and 2 */

  *(UINT32 *)(HARRIER_OUTBOUND_TRANSLATION_ADDR_0_REG) = 0; /* 0x220 */
  EIEIO_SYNC;

  *(UINT32 *)(HARRIER_OUTBOUND_TRANSLATION_ADDR_1_REG) = 0; /* 0x228 */
  EIEIO_SYNC;

  *(UINT32 *)(HARRIER_OUTBOUND_TRANSLATION_ADDR_2_REG) = 0; /* 0x230 */
  EIEIO_SYNC;

  /* clear the Outbound Translation Attribute Registers 0, 1, and 2 */

  *(UINT32 *)(HARRIER_OUTBOUND_TRANSLATION_ADDR_0_REG + 
              HARRIER_OUTBOUND_TRANSLATION_OFFSETINFO_OFFSET) = 0; /* 0x224 */

  EIEIO_SYNC;

  *(UINT32 *)(HARRIER_OUTBOUND_TRANSLATION_ADDR_1_REG + 
              HARRIER_OUTBOUND_TRANSLATION_OFFSETINFO_OFFSET) = 0; /* 0x22c */

  EIEIO_SYNC;

  *(UINT32 *)(HARRIER_OUTBOUND_TRANSLATION_ADDR_2_REG + 
              HARRIER_OUTBOUND_TRANSLATION_OFFSETINFO_OFFSET) = 0; /* 0x234 */

  EIEIO_SYNC;

    /*
     * set the Outbound Translation Address Register 3.
     * This register maps the PCI CONFIG_ADDR and CONFIG_DATA
     * registers to PCI I/O space.  The default is CHRP(Map B)
     * addressing, which places the registers at:
     *      CONFIG_ADDR = ISA_MSTR_IO_LOCAL + 0xCF8
     *      CONFIG_DATA = ISA_MSTR_IO_LOCAL + 0xCFC
     */

  *(UINT32 *)HARRIER_OUTBOUND_TRANSLATION_ADDR_3_REG = ISA_MSTR_IO_LOCAL |
      ((ISA_MSTR_IO_LOCAL >> 16) | 0x0F);

  EIEIO_SYNC;

  /* enable ENA=1,WPE=0,IOM=0 and set offet */

  *(UINT32 *)(HARRIER_OUTBOUND_TRANSLATION_ADDR_3_REG + 
              HARRIER_OUTBOUND_TRANSLATION_OFFSETINFO_OFFSET) = 
              HARRIER_PCFS_ITAT_ENA | 
	      ((ISA_MSTR_IO_BUS - (ISA_MSTR_IO_LOCAL >> 16)) << 16);

  EIEIO_SYNC;

  /* disable the Passive Slave registers */

  *(UINT32 *)HARRIER_PASSIVE_SLAVEADDRESS_REG = 0;

  EIEIO_SYNC;

  *(UINT32 *)HARRIER_PASSIVE_SLAVEOFFSET_REG = 0;

  EIEIO_SYNC;


  /**********************************************************/
  /* Set outbound translation register 0 to the top of DRAM */

  *(UINT32 *)(HARRIER_OUTBOUND_TRANSLATION_ADDR_0_REG) = 
	((CPU2PCI_ADDR0_START<<16) | CPU2PCI_ADDR0_END);                /* 0x220 */

  EIEIO_SYNC;

  /* Set offset and attributes */

  *(UINT32 *)(HARRIER_OUTBOUND_TRANSLATION_ADDR_0_REG +
                HARRIER_OUTBOUND_TRANSLATION_OFFSETINFO_OFFSET) =
	((CPU2PCI_OFFSET0 << 16) | CPU2PCI_MSATT0);                     /* 0x224 */

  EIEIO_SYNC;


  /*******************************************/
  /* Set CPU->PCI window 1 for PCI I/O space */

  *(UINT32 *)(HARRIER_OUTBOUND_TRANSLATION_ADDR_1_REG) = 
               ((CPU2PCI_ADDR1_START << 16) | (CPU2PCI_ADDR1_END)); /* 0x228 */

  EIEIO_SYNC;

  *(UINT32 *)(HARRIER_OUTBOUND_TRANSLATION_ADDR_1_REG +
                HARRIER_OUTBOUND_TRANSLATION_OFFSETINFO_OFFSET) =
                ((CPU2PCI_OFFSET1 << 16) | CPU2PCI_MSATT1);         /* 0x22c */

  EIEIO_SYNC;


  /****************************************************/
  /* Set CPU->PCI window 2 (maps local PCI address 0) */

  /**(volatile UINT32 *)0xfeff0230 = 0xd000efff;*/
  *(UINT32 *)(HARRIER_OUTBOUND_TRANSLATION_ADDR_2_REG) = 
              ((CPU2PCI_ADDR2_START<<16) | CPU2PCI_ADDR2_END);      /* 0x230 */

  EIEIO_SYNC;

  /**(volatile UINT32 *)0xfeff0234 = 0x30000782;*/
  *(UINT32 *)(HARRIER_OUTBOUND_TRANSLATION_ADDR_2_REG +
                HARRIER_OUTBOUND_TRANSLATION_OFFSETINFO_OFFSET) =
                ((CPU2PCI_OFFSET2 << 16) | CPU2PCI_MSATT2);         /* 0x234 */

  EIEIO_SYNC;


  /********************************************************************
   * Enable visibility of message passing register group.  Setting the
   * bit below will ensure that the MPBAR (Harrier BAR at PCI config
   * offset 0x10 will be visible during PCI autoconfiguration and
   * will request 0x1000 (4K) bytes of memory
   */

  *(UINT32 *)(HARRIER_MSG_PASSINGATTR_REG) = HARRIER_MPAT_ENA;

  EIEIO_SYNC;

  /* Set up the PCFS Register. Turn everything off and clear error status */

  *(UINT32 *)HARRIER_PHB_COMMAND_REG = HARRIER_STAT_RCVPE |
                                       HARRIER_STAT_SIGSE |
                                       HARRIER_STAT_RCVMA |
                                       HARRIER_STAT_RCVTA |
                                       HARRIER_STAT_SIGTA |
                                       HARRIER_STAT_DPAR;
  EIEIO_SYNC;

  *(UINT32 *)HARRIER_CACHE_LINESIZE_REG = HARRIER_CACHE_LINESIZE |
                                          HARRIER_READLATENCY;

  EIEIO_SYNC;

  /* Setup interrupt line register INTL=00,INT=1(INT_A),MNGN=00,MXLA=00 */

  *(UINT32 *)HARRIER_INTERRUPT_LINE_REG = HARRIER_INTP_1;

  EIEIO_SYNC;

    /*
     * Disable all inbound translation registers (ITBAR) - 
     * we'll enable some later.
     */

  *(UINT32 *)HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_0 = 0;
  *(UINT32 *)HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_0    = 0; 
  *(UINT8 *)HARRIER_INBOUND_TRANSLATION_SIZE_0_REG      = inboundSizeCode(0);
  *(UINT16 *)HARRIER_INBOUND_TRANSLATION_OFFSET_0_REG   = SHORTSWAP(0);

  EIEIO_SYNC;

  *(UINT32 *)HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_1 = 0;
  *(UINT32 *)HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_1    = 0; 
  *(UINT8 *)HARRIER_INBOUND_TRANSLATION_SIZE_1_REG      = inboundSizeCode(0);
  *(UINT16 *)HARRIER_INBOUND_TRANSLATION_OFFSET_1_REG   = SHORTSWAP(0);

  EIEIO_SYNC;

  *(UINT32 *)HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_2 = 0;
  *(UINT32 *)HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_2    = 0; 
  *(UINT8 *)HARRIER_INBOUND_TRANSLATION_SIZE_2_REG      = inboundSizeCode(0);
  *(UINT16 *)HARRIER_INBOUND_TRANSLATION_OFFSET_2_REG   = SHORTSWAP(0);

  EIEIO_SYNC;

  *(UINT32 *)HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_3 = 0;
  *(UINT32 *)HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_3    = 0; 
  *(UINT8 *)HARRIER_INBOUND_TRANSLATION_SIZE_3_REG      = inboundSizeCode(0);
  *(UINT16 *)HARRIER_INBOUND_TRANSLATION_OFFSET_3_REG   = SHORTSWAP(0);

  EIEIO_SYNC;

  *(UINT32 *)HARRIER_XPORT0_ADDR_RANGE_REG = 0xF000F1FF;

  EIEIO_SYNC;

  *(UINT32 *)HARRIER_XPORT0_ATTR_REG |= 0xE1F00000;

  EIEIO_SYNC;

  *(UINT32 *)HARRIER_XPORT1_ADDR_RANGE_REG = 0xFF80FFEF;

  EIEIO_SYNC;

  *(UINT32 *)HARRIER_XPORT1_ATTR_REG |= 0xEBF00000;

  EIEIO_SYNC;

#ifdef INCLUDE_PRPMC800XT
  *(UINT32 *)HARRIER_XPORT2_ADDR_RANGE_REG = 0xFF10FF1F;

  EIEIO_SYNC;

  *(UINT32 *)HARRIER_XPORT2_ATTR_REG |= 0xE0F00000;

  EIEIO_SYNC;

  *(UINT32 *)HARRIER_XPORT3_ADDR_RANGE_REG = 0xFF20FF2F;
 
  EIEIO_SYNC;
 
  *(UINT32 *)HARRIER_XPORT3_ATTR_REG |= 0xE0F00000;
 
  EIEIO_SYNC;
#endif /* INCLUDE_PRPMC800XT */

  /* Clear error exceptions */

  *(UINT32 *)HARRIER_ERROR_EXCEPTION_CLEAR_REG = 0;

  EIEIO_SYNC;

  /* Enable the PCI Bridge */

  *(UINT16 *)HARRIER_PHB_COMMAND_REG |= HARRIER_CMMD_MTSR | 
					  HARRIER_CMMD_MEMSP;

#ifdef PCI_AUTO_DEBUG
    sprintf(txt,"sysHarrierPhbInit: completed.\r\n");
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif    

  return;
}


/*******************************************************************************
*
* sysHarrierPhbInit2 - initialize the Harrier PHB registers which require VPD
*
* This function performs the second phase of the Harrier PPC-bus registers.
* These registers require information contained in the VPD.
*
* RETURNS: N/A
*/

void
sysHarrierPhbInit2 (void)
{
#ifdef PCI_AUTO_DEBUG
  char txt[100];
  sprintf(txt,"sysHarrierPhbInit2 reached.\r\n");
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif    
  *(UINT32 *)HARRIER_PPC_CLOCK_FREQUENCY_REG = 
        256 - ((sysGetBusSpd () + (MHZ/2))/MHZ);
  EIEIO_SYNC;

  return;
}


/******************************************************************************
*
* sysHarrierInitPhbExt - initialize the extended portion of the Harrier PHB 
* PCI header.
*
* This routine initializes the extended portion of the PCI header for the
* Motorola Harrier PCI-Host Bridge Controller (PHB).
*
* RETURNS: OK
*/

STATUS
sysHarrierInitPhbExt (void)
{
/*#ifdef PCI_AUTO_DEBUG*/
  char txt[100];
/*#endif*/

  /*
   * Partially initialize the Harrier's Slave decoders (attribute and
   * size).  pciAutoConfig(), which has not yet run, will be setting
   * the BAR base addresses after which we will set the translations
   * in sysHarrierInitInpOffset().
   *
   * These decoders map addresses on the PCI bus to the CPU for
   * access to local DRAM.
   *
   * Because hardware can read past real memory, it is necessary to disable
   * Read Ahead for the last 64k of physical memory (DRAM).
   */

  /* Setup Inbound Translation Window some low DRAM onto PCI bus. */

#ifdef PCI_AUTO_DEBUG
  sprintf(txt,"sysHarrierInitPhbExt: attr,size,offset: 0x%08x 0x%08x 0x%08x\r\n",
    PCI2CPU_0_ATTR,inboundSizeCode(PCI2CPU_0_SIZE),SHORTSWAP(PCI2CPU_0_TARG));
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif


  /* if following commented, it works ! */

  *(UINT32 *)HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_0 = PCI2CPU_0_ATTR;/* 34C */


  /* if following commented out, it works but ethernet does not ! */
  /*
  [0x10] ===> ret=0, bars=0x83000000 0x00000008 0x00000000 0x00000100
  [0x11] ===> ret=0, bars=0x83002000 0x00000041 0x83020000 0x3808011a

  (must be

  [0x10] ===> ret=0, bars=0x83000000 0x00000008 0x00000000 0x00000100
  [0x11] ===> ret=0, bars=0x90000000 0x00000041 0x90020000 0x3808011a
  */

  /*
   PCI2CPU_0_SIZE=PCI2DRAM_MAP_SIZE=0x08000000 -> 0x0F(currently)
     00-4k,01-8k,02-16k,03-32k,04-64k,05-128k,06-256k,07-512k,08-1M,
     09-2M,0A-4M,0B-8M,0C-16M,0D-32M,0E-64M,0F-128M,10-256M,11-512M
   *(UINT8 *)HARRIER_INBOUND_TRANSLATION_SIZE_0_REG     = 0x0F;
  */
  *(UINT8 *)HARRIER_INBOUND_TRANSLATION_SIZE_0_REG     = 
   inboundSizeCode(PCI2CPU_0_SIZE);  /* 0x348 */



  /* does nothing: PCI2CPU_0_TARG=0x0 */

  *(UINT16 *)HARRIER_INBOUND_TRANSLATION_OFFSET_0_REG  = 
	SHORTSWAP(PCI2CPU_0_TARG); /* 0x34A */

  EIEIO_SYNC;



  /* following is not needed for mv5500, but does not harm .. */

  *(UINT32 *)HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_1   = PCI2CPU_1_ATTR;
  *(UINT8 *)HARRIER_INBOUND_TRANSLATION_SIZE_1_REG     = 
              inboundSizeCode(PCI2CPU_1_SIZE);
  *(UINT16 *)HARRIER_INBOUND_TRANSLATION_OFFSET_1_REG  = 
               SHORTSWAP(PCI2CPU_1_TARG);

  EIEIO_SYNC;

  *(UINT32 *)HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_2   = PCI2CPU_2_ATTR;
  *(UINT8 *)HARRIER_INBOUND_TRANSLATION_SIZE_2_REG     = 
              inboundSizeCode(PCI2CPU_2_SIZE);
  *(UINT16 *)HARRIER_INBOUND_TRANSLATION_OFFSET_2_REG  = 
               SHORTSWAP(PCI2CPU_2_TARG);

  EIEIO_SYNC;

  *(UINT32 *)HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_3   = PCI2CPU_3_ATTR;
  *(UINT8 *)HARRIER_INBOUND_TRANSLATION_SIZE_3_REG     = 
	      inboundSizeCode(PCI2CPU_3_SIZE);
  *(UINT16 *)HARRIER_INBOUND_TRANSLATION_OFFSET_3_REG  = 
               SHORTSWAP(PCI2CPU_3_TARG);

  EIEIO_SYNC;

  /*#endif*/




sprintf(txt,"sysHarrierInitPhbExt: huge delay 1 ..\r\n ");
sysDebugMsg(txt,CONTINUE_EXECUTION);
    sysUsDelay(20000000);
sprintf(txt,"sysHarrierInitPhbExt: huge delay finished, 0x%08x\r\n ",
*(volatile UINT32 *)(HARRIER_MISC_CONTROL_STATUS_REG));
sysDebugMsg(txt,CONTINUE_EXECUTION);



  /* Sergey: EREADY stuff ?! */

  /* Clear PCI configuration cycles hold off bit (0xfeff0200 bit 20) */
  /* Sergey: as result brigde will be visible from PCI */

  *(UINT32 *)HARRIER_BRIDGE_PCI_CONTROLSTATUS_REG &= 
               ~HARRIER_BPCS_XCSR_CSH;

  /*
   * Set the PCI Bus Enumeration Ready bit and wait for the PCI Bus
   * Enumeration status bit to flip before proceeding (0xfeff001c)
   */

  *(UINT32 *)HARRIER_MISC_CONTROL_STATUS_REG |= HARRIER_MCSR_EREADY;

  EIEIO_SYNC;

/*#ifdef PCI_AUTO_DEBUG*/
sprintf(txt,
"sysHarrierInitPhbExt: wait for Enumeration Ready bit flip 0x%08x=0x%08x\r\n",
(volatile UINT32 *)(HARRIER_MISC_CONTROL_STATUS_REG),
*(volatile UINT32 *)(HARRIER_MISC_CONTROL_STATUS_REG));
sysDebugMsg(txt,CONTINUE_EXECUTION);
/*#endif*/

  while((*(volatile UINT32 *)(HARRIER_MISC_CONTROL_STATUS_REG)) & 
			        (HARRIER_MCSR_EREADYS == 0))
  {
	;
  }

/*#ifdef PCI_AUTO_DEBUG*/
sprintf(txt,"sysHarrierInitPhbExt: it fliped ! 0x%08x=0x%08x\r\n",
(volatile UINT32 *)(HARRIER_MISC_CONTROL_STATUS_REG),
*(volatile UINT32 *)(HARRIER_MISC_CONTROL_STATUS_REG));
sysDebugMsg(txt,CONTINUE_EXECUTION);
/*#endif*/


  return(OK);
}


/******************************************************************************
*
* sysHarrierInitPhbExt2 - Fix address translation offsets.
*
* This routine calculates the address translation offsets following PCI
* Auto Configuration.
*
* Sergey: called from sysHarrierFixInp() only for revision 1 (not for us !?)
*
* RETURNS: OK
*/

STATUS
sysHarrierInitPhbExt2(void)
{
  UINT32 barVal;
  UINT32 whichBar = HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_0;
  UINT32 whichOffset = HARRIER_INBOUND_TRANSLATION_OFFSET_0_REG;
  UINT32 barOffset;
  int    barCount = 0;
#ifdef PCI_AUTO_DEBUG
  char txt[100];
  sprintf(txt,"sysHarrierInitPhbExt2: bar,offset,mask: 0x%08x 0x%08x 0x%08x\r\n",
    whichBar,whichOffset,HARRIER_PCFS_ITBAR_BASE_MASK);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  for( ; barCount < 4; barCount++)
  {
    barVal = 
            LONGSWAP(*(UINT32 *)(whichBar + (barCount * 4))) & 
		     HARRIER_PCFS_ITBAR_BASE_MASK;
    barOffset = (0x0 - barVal) >> 16;
    *(UINT16 *)(whichOffset + (barCount * 8))  = SHORTSWAP(barOffset);

#ifdef PCI_AUTO_DEBUG
    sprintf(txt,"sysHarrierInitPhbExt2: [%1d] inadr=0x%08x barVal=0x%08x\r\n",
      barCount,((UINT32 *)(whichBar + (barCount * 4))),barVal);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    sprintf(txt,"sysHarrierInitPhbExt2: [%1d] barOffset=0x%08x write into 0x%08x\r\n",
      barCount,barOffset,((UINT16 *)(whichOffset + (barCount * 8))));
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  }

  return(OK);
}

/******************************************************************************
*
* sysHarrierFixInp - fix input offsets if Harrier revision 1.
* 
* This routine conditionally changes the input offset registers if we're
* working with a Harrier revision 1.
*
* RETURNS: OK
*/

STATUS
sysHarrierFixInp (void)
{
  USHORT revisionId;
  revisionId = ((*(UINT32 *)HARRIER_REGISTER_REVISION_ID) >>
                              HARRIER_REVISION_ID_SHIFT_MASK);

   /* 
    * Harrier Errata 7 - Inbound map decoders
    * The specification states that the PCI address bits which 
    * correspond with the programmable bits position within the BAR
    * are set to zero before the offset is added to the PCI address
    * to create the PowerPC bus address. The address bits are not 
    * set to zero before the offset is added. This means the offset
    * is dependent on the base address.
    * e.g. If the PCI base address is 0x08000000 and the size is 128M
    * and the offset is 0, the incoming PCI address should map to
    * PowerPC address 0.
    */

  if(revisionId == HARRIER_REVISION_1)
  {

    /*
     * Re-evaluate the Inbound Translation Offset from base addr.
     * This fix is required only for the Harrier I chipset
     */

    (void)sysHarrierInitPhbExt2 (); 
  }

  return (OK);
}

/******************************************************************************
*
* sysHarrierErrClr - Clear error conditions in Harrier
*
* This routine clears any existing errors in the Motorola Harrier PCI Host 
* Bridge Controller.
*
* RETURNS: N/A
*/

void
sysHarrierErrClr (void)
{
  *(UINT32 *)HARRIER_ERROR_EXCEPTION_CLEAR_REG = 0;
}
