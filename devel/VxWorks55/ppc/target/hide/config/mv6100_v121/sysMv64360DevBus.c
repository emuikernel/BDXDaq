/* sysMv64360DevBus.c - Initialize the Mv64360 device bus support registers */

/* Copyright 2004 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01l,16feb04,cak  Modified to support two soldered banks of 64MB each.
01k,17dec03,cak  BSP update.
01j,20sep02,cak  Modifications during debug. 
01i,03sep02,scb  Fixes found when testing on hxeb100.
01h,11jul02,scb  Adjustment to memory map for flash banks.
01g,26jun02,scb  Register mnemonic name changes.
01f,25jun02,cak  Replaced sysVpdPktGet with sysMotVpdPktGet.
01e,14jun02,scb  Minor changes to register mnemonics.
01d,20may02,scb  Shift DevCsx base address value before stuffing register.
01c,20may02,scb  Make BootCs bank size a constant value of 8MB.
01b,16may02,scb  Program address base enable register.
01a,25apr02,scb  Original writing.
*/

/*
DESCRIPTION
This module initializes Mv64360 device bus support registers.
*/

/* includes */

#include "mv64360.h"
#include "sysMotVpd.h"

/* defines */

/* typedefs */

/* globals */

/* locals */

/*
 * This table contains static initialization values for the MV64360
 * device bus support registers.  The registers listed below which
 * are associated with the NOT_INIT_MV64360_REG macro are dynamically
 * initialized inside of sysMv64360DevBusInit() based upon data
 * obtained from VPD Flash Memory Controller packets.
 */

LOCAL UINT32 mv64360DvctlTbl [] =
    {

    NOT_INIT_MV64360_REG (CPUIF_DEVCS0_BASE_ADDR , dymamic)

    NOT_INIT_MV64360_REG (CPUIF_DEVCS0_SIZE      , dynamic)

    YES_INIT_MV64360_REG (CPUIF_DEVCS1_BASE_ADDR ,
			  (MV64360_DEVCS1_BASE_ADDR >> 16))

    YES_INIT_MV64360_REG (CPUIF_DEVCS1_SIZE      , \
			  MV64360_SIZE_FIELD (MV64360_DEVCS1_SIZE))

    YES_INIT_MV64360_REG (CPUIF_DEVCS2_BASE_ADDR , 0)   /* Unused */
    YES_INIT_MV64360_REG (CPUIF_DEVCS2_SIZE      , 0)   /* Unused */

    YES_INIT_MV64360_REG (CPUIF_DEVCS3_BASE_ADDR , 0)   /* Unused */
    YES_INIT_MV64360_REG (CPUIF_DEVCS3_SIZE      , 0)   /* Unused */

    NOT_INIT_MV64360_REG (CPUIF_BOOTCS_BASE_ADDR , dynamic)

    NOT_INIT_MV64360_REG (CPUIF_BOOTCS_SIZE      , dynamic)

    NOT_INIT_MV64360_REG (DVCTL_DEVICE_BANK0_PARAMS, dynamic)

    YES_INIT_MV64360_REG (DVCTL_DEVICE_BANK1_PARAMS, \
			  DVCTL_DEVICE_BANK1_PARAMS_VAL)

    YES_INIT_MV64360_REG (DVCTL_DEVICE_BANK2_PARAMS, \
			  DVCTL_DEVICE_BANK2_PARAMS_VAL)

    YES_INIT_MV64360_REG (DVCTL_DEVICE_BANK3_PARAMS, \
			  DVCTL_DEVICE_BANK3_PARAMS_VAL)

    NOT_INIT_MV64360_REG (DVCTL_DEVICE_BOOT_BANK_PARAMS, dynamic)

    YES_INIT_MV64360_REG (DVCTL_DEVICE_INTERFACE_CTRL, \
			  DVCTL_DEVICE_INTERFACE_CTRL_VAL)

    YES_INIT_MV64360_REG (DVCTL_DEVICE_INTERFACE_CROSSBAR_CTRL_LOW, \
			  DVCTL_DEVICE_INTERFACE_CROSSBAR_CTRL_LOW_VAL)

    YES_INIT_MV64360_REG (DVCTL_DEVICE_INTERFACE_CROSSBAR_CTRL_HI, \
			  DVCTL_DEVICE_INTERFACE_CROSSBAR_CTRL_HI_VAL)


    YES_INIT_MV64360_REG (DVCTL_DEVICE_INTERFACE_CROSSBAR_TIMEOUT, \
			  DVCTL_DEVICE_INTERFACE_CROSSBAR_TIMEOUT_VAL)

    YES_INIT_MV64360_REG (DVCTL_DEVICE_INTERRUPT_CAUSE , 0)
    YES_INIT_MV64360_REG (DVCTL_DEVICE_INTERRUPT_MASK  , 0)

    /*
     * The device error address, data, and parity registers latch data
     * until the device error address register is read.  The error
     * address register will be dynamically read, the other registers
     * need no explicit initialization.
     */

    NOT_INIT_MV64360_REG (DVCTL_DEVICE_ERR_ADDR        , read)
    NOT_INIT_MV64360_REG (DVCTL_DEVICE_ERR_DATA        , latched)
    NOT_INIT_MV64360_REG (DVCTL_DEVICE_ERR_PARITY      , latched)

    END_INIT_MV64360_REG
    };

/* forward declarations */

/* externals */

IMPORT VPD_PACKET * sysVpdPkts[]; /* board vpd packets */
IMPORT STATUS sysMotVpdPktGet (UCHAR  vpdType, UINT32 vpdInstance,
			       VPD_PACKET **pVpdPtr, VPD_PACKET ** pVpdPacket);
IMPORT UCHAR sysInByte (ULONG addr);

/******************************************************************************
*
* sysMv64360DevBusInit - Dynamic initialization of Mv64360 device bus interface
*
* This routine performs the dynamic initialization of the Mv64360 device bus
* interface registers.  It depends upon the I2C interface being functional.
*
* RETURNS: NA
*/

void sysMv64360DevBusInit (void)
    {

    UINT8    statReg;
    BOOL     bootedFromBankA;
    int      i;                 /* VPD packet counter */
    UCHAR  * v       = NULL;    /* Pointer to VPD packet */
    UCHAR  * devCs0  = NULL;
    UCHAR  * devBoot = NULL;
    UINT32   bankSize;
    UINT32   bankBase;
    UINT32   intfWidth;
    UINT32   bankParamVal;
    UINT32   baseEnable;

    /* Initialize static device control parameters */

    sysMv64360RegInit (mv64360DvctlTbl);

    /*
     * Find out which bank we booted from (A or B).
     * Note that devCs1 region has already been programmed by the
     * previous call to sysMv64360RegInit().  This will allow us to
     * actually access the system status register.
     */

    statReg = sysInByte(SYSTEM_STATUS_REG_1);
    statReg &= SYSTEM_STATUS_REG_1_BANK_SELECT_MASK;

    bootedFromBankA = (statReg == 0);

    /* Point devCs0 and devBoot to the correct VPD packets */

    for (i=0; i<=1; i++)
	{

	if (sysMotVpdPktGet(VPD_PID_FMC, i, &sysVpdPkts[0],
			    (VPD_PACKET **) &v) == ERROR)
	    {
	    sysDebugMsg("sysMv64360DevBusInit(): error reading VPD\n",
			CONTINUE_EXECUTION);
	    break;
	    }
	else
	    {
	    v += 2;	/* Advance to data portion of packet */
	    if ((v [VPD_FMC_BANK] == 0) || (v [VPD_FMC_BANK] == 1))
		{
		if (bootedFromBankA)
		    {

                    /*
                     * Bank selection jumper indicates a boot from bank A. 
                     */

		    if (v [VPD_FMC_BANK] == 0)  /* If flash bank A packet */
			devBoot = v;
		    if (v [VPD_FMC_BANK] == 1)  /* If flash bank B packet */
			devCs0 = v;
		    }
		else
		    {

                    /*
                     * Bank selection jumper indicates a boot from bank B.
                     */

		    if (v [VPD_FMC_BANK] == 0)  /* If flash bank A packet */
			devCs0 = v;
		    if (v [VPD_FMC_BANK] == 1)  /* If flash bank B packet */
			devBoot = v;

		    }
		}
	    else
		{
		sysDebugMsg(
		    "sysMv64360DevBusInit(): invalid Flash Config packet\n",
		    CONTINUE_EXECUTION);
		}
	    }
	}

    /*
     * Now program the Device Bus registers that require dynamic
     * calculation from VPD data - first handle the case where we
     * have booted from bank A.
     */

    if (bootedFromBankA)     /* If booting from bank A */
	{
	intfWidth = 16 * 2;     /* Default value */

	/*
	 * Handle the devCs0 related programming, if VPD is available
	 * we'll use it to determine the size in bytes (legal sizes are
	 * 8MB, 16MB, or 32MB) and width.  If VPD is not available then
	 * we will use default values.
	 */

        bankSize = SIZE_2MB;    /* Default bank B val in case VPD is botched */

	if (devCs0 != NULL)     /* If VPD data for this bank is available ... */
	    {

	    if ((devCs0 [VPD_FMC_SIZE] <= 8) && (devCs0 [VPD_FMC_SIZE] >= 5))
		bankSize = (1 << devCs0 [VPD_FMC_SIZE]) * SIZE_256KB;

	    if ((devCs0 [VPD_FMC_DDW] == 8) || (devCs0 [VPD_FMC_DDW] == 16) ||
		(devCs0 [VPD_FMC_DDW] == 32))
		{

		/*
		 * Multiply VPD "device data width" by VPD "number of devices/
		 * sockets present" to obtain the value to program into
		 * the Discovery II.
		 */

		intfWidth = (devCs0 [VPD_FMC_DDW] * devCs0 [VPD_FMC_NOD]);
		}
	    }

	/* Program the base address register */

        bankBase = MV64360_DEVCS0_BASE_ADDR;
	MV64360_WRITE32_PUSH (MV64360_REG_BASE, CPUIF_DEVCS0_BASE_ADDR,
				  MV64360_BASE_DECODE_ADDR(bankBase));

	/* Program the size address register */

	MV64360_WRITE32_PUSH (MV64360_REG_BASE, CPUIF_DEVCS0_SIZE,
			      MV64360_SIZE_FIELD (bankSize));

	/* Program the bank parameter register (dev width is variable) */

	bankParamVal = ((DVCTL_DEVICE_BANK0_PARAMS_VAL & ~(0x3 << 20)) |
			((intfWidth >> 4) << 20));

	MV64360_WRITE32_PUSH (MV64360_REG_BASE,
			      DVCTL_DEVICE_BANK0_PARAMS, bankParamVal);

	/*
	 * Handle the devBootCs related programming, if VPD is available
	 * we'll use it to determine the width.  If VPD is not available
	 * then we will use the default value.
	 */

	intfWidth = 16 * 2;      /* Default value */

	if (devBoot != NULL)    /* If VPD data for this bank is available ... */
	    {

	    if ((devBoot [VPD_FMC_DDW] == 8) || (devBoot [VPD_FMC_DDW] == 16) ||
		(devBoot [VPD_FMC_DDW] == 32))
		{

		/* Perform calculation as we did for Cs0 case */

		intfWidth = (devBoot [VPD_FMC_DDW] * devBoot [VPD_FMC_NOD]);
		}
	    }

	/* Program the base address register */

        bankBase = MV64360_BOOTCS_BASE_ADDR;
	MV64360_WRITE32_PUSH (MV64360_REG_BASE, CPUIF_BOOTCS_BASE_ADDR,
			      MV64360_BASE_DECODE_ADDR(bankBase));

	/*
	 * Program the size register:
	 * This bank must cover the entire range from the flash
	 * bank base address thru the end of 32-bit memory space
	 * (0xffffffff) in  order to make sure that  "shadowing" of
	 * bootrom covers the bootrom address range from
	 * 0xfff00000 thru 0xffffffff.
	 */

	bankSize = ( (UINT32) 0xffffffff -
		     (UINT32) MV64360_BOOTCS_BASE_ADDR ) + 1;

	MV64360_WRITE32_PUSH (MV64360_REG_BASE, CPUIF_BOOTCS_SIZE,
			      MV64360_SIZE_FIELD (bankSize));

	/* Program the bank parameter register (dev width is variable) */

	bankParamVal = ((DVCTL_DEVICE_BOOT_BANK_PARAMS_VAL & ~(0x3 << 20)) |
			((intfWidth >> 4) << 20));

	MV64360_WRITE32_PUSH (MV64360_REG_BASE,
			      DVCTL_DEVICE_BOOT_BANK_PARAMS, bankParamVal);

	}

    else        /* Boot from Bank B */

	{
	intfWidth = 16 * 2;     /* Default value */

	/*
	 * Handle the devCs0 related programming, if VPD is available
	 * we'll use it to determine the size in bytes (legal sizes are
	 * 8MB, 16MB, 32MB, or 64MB) and width.  If VPD is not available
	 * then  we will use default values.
	 */

        bankSize = SIZE_4MB;    /* Default bank A val in case VPD is botched */

	if (devCs0 != NULL)     /* If VPD data for this bank is available ... */
	    {

	    if ((devCs0 [VPD_FMC_SIZE] <= 9) && (devCs0 [VPD_FMC_SIZE] >= 5))
		bankSize = (1 << devCs0 [VPD_FMC_SIZE]) * SIZE_256KB;

	    if ((devCs0 [VPD_FMC_DDW] == 8) || (devCs0 [VPD_FMC_DDW] == 16) ||
		(devCs0 [VPD_FMC_DDW] == 32))
		{

		/*
		 * Multiply VPD "device data width" by VPD "number of devices/
		 * sockets present" to obtain the value to program into
		 * the Discovery II.
		 */

		intfWidth = (devCs0 [VPD_FMC_DDW] * devCs0 [VPD_FMC_NOD]);
		}
	    }

	/* Program the base address register */

        bankBase = MV64360_DEVCS0_BASE_ADDR;
	MV64360_WRITE32_PUSH (MV64360_REG_BASE, CPUIF_DEVCS0_BASE_ADDR,
			      MV64360_BASE_DECODE_ADDR(bankBase));

	/* Program the size register */

	MV64360_WRITE32_PUSH (MV64360_REG_BASE, CPUIF_DEVCS0_SIZE,
			      MV64360_SIZE_FIELD (bankSize));

	/* Program the bank parameter register (dev width is variable) */

	bankParamVal = ((DVCTL_DEVICE_BANK0_PARAMS_VAL & ~(0x3 << 20)) |
			((intfWidth >> 4) << 20));

	MV64360_WRITE32_PUSH (MV64360_REG_BASE,
			      DVCTL_DEVICE_BANK0_PARAMS, bankParamVal);
	/*
	 * Handle the devBootCs related programming, if VPD is available
	 * we'll use it to determine the width.  If VPD is not available
	 * then we will use the default value.
	 */

	intfWidth = 16 * 2;      /* Default value */

	if (devBoot != NULL)    /* If VPD data for this bank is available ... */
	    {

	    if ((devBoot [VPD_FMC_DDW] == 8) || (devBoot [VPD_FMC_DDW] == 16) ||
		(devBoot [VPD_FMC_DDW] == 32) )
		{

		/* Perform calculation as we did for Cs0 case */

		intfWidth = (devBoot [VPD_FMC_DDW] * devCs0 [VPD_FMC_NOD]);
		}
	    }

	/*
	 * Handle the devBoot related programming.  This bank
	 * must cover the entire range from the flash bank base
	 * address thru the end of 32-bit memory space (0xffffffff) in
	 * order to make sure that "shadowing" of bootrom covers the
	 * bootrom address range from 0xfff00000 thru 0xffffffff.
	 */

	bankSize = ( (UINT32) 0xffffffff -
		     (UINT32) MV64360_BOOTCS_BASE_ADDR ) + 1;

	/* Program the base address register */

        bankBase = MV64360_BOOTCS_BASE_ADDR;
	MV64360_WRITE32_PUSH (MV64360_REG_BASE, CPUIF_BOOTCS_BASE_ADDR,
			      MV64360_BASE_DECODE_ADDR(bankBase));

	/* Program the size register */

	MV64360_WRITE32_PUSH (MV64360_REG_BASE, CPUIF_BOOTCS_SIZE,
			      MV64360_SIZE_FIELD (bankSize));

	/* Program the bank parameter register (dev width is variable) */

	bankParamVal = ((DVCTL_DEVICE_BOOT_BANK_PARAMS_VAL & ~(0x3 << 20)) |
			((intfWidth >> 4) << 20));

	MV64360_WRITE32_PUSH (MV64360_REG_BASE,
			     DVCTL_DEVICE_BOOT_BANK_PARAMS, bankParamVal);

	}

    /*
     * If an address is latched in the following register, reading the
     * register will unlatch it.
     */

    (void)MV64360_READ32 (MV64360_REG_BASE, DVCTL_DEVICE_ERR_ADDR);

    /*
     * Now enable the appropriate windows by tweeking the "Base
     * Address Enable" register.  First read the base enable
     * register, then disable all of the DevX related windows by
     * setting the appropriate bits in the image and then clear
     * the bits for DevCs0, DevCs1 and BootCs - which will enable
     * these windows.
     */

    baseEnable = MV64360_READ32 (MV64360_REG_BASE, CPUIF_BASE_ADDR_ENABLE);

    baseEnable |= ( (1 << CPUIF_BASE_ADDR_ENABLE_DEVCS0_BIT) |
		    (1 << CPUIF_BASE_ADDR_ENABLE_DEVCS1_BIT) |
		    (1 << CPUIF_BASE_ADDR_ENABLE_DEVCS2_BIT) |
		    (1 << CPUIF_BASE_ADDR_ENABLE_DEVCS3_BIT) |
		    (1 << CPUIF_BASE_ADDR_ENABLE_BOOTCS_BIT) );

    baseEnable &= ~( (1 << CPUIF_BASE_ADDR_ENABLE_DEVCS0_BIT) |
		     (1 << CPUIF_BASE_ADDR_ENABLE_DEVCS1_BIT) |
		     (1 << CPUIF_BASE_ADDR_ENABLE_BOOTCS_BIT) );

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, CPUIF_BASE_ADDR_ENABLE,
			  baseEnable);
    }
