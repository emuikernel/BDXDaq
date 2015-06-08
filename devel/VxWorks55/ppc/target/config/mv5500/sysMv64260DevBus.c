/* sysMv64260DevBus.c - Initialize the GT-64260 device bus support registers */

/* Copyright 2002 Motorola, Inc., All Rights Reserved */

#include "copyright_wrs.h"

/*
modification history
--------------------
01b,08nov02,yyz Fix ctrl param when booting from socketed.
01a,01oct02,scb Copied from hxeb100 base (ver 01a).
*/

/*
DESCRIPTION

This module initializes Mv64260 device bus support registers.
*/

/* includes */

#include "mv64260.h"
#include "sysMotVpd.h"

/* defines */

/* typedefs */

/* globals */

/* locals */

/*
 * This table contains static initialization values for the MV64260
 * device bus support registers.  The registers listed below which
 * are associated with the NOT_INIT_MV64260_REG macro are dynamically
 * initialized inside of sysMv64260DevBusInit() based upon data
 * obtained from VPD Flash Memory Controller packets.
 */

LOCAL UINT32 mv64260DevBusTbl [] =
    {

    /* 
     * The following registers permit access to the nonboot flash bank 
     * via low and high address decode registers and a bank parameter 
     * register.  They are are not statically initialized from this table 
     * but instead are dynamically initialized inside of 
     * sysMv64260DevBusInit().
     */

    NOT_INIT_MV64260_REG (CPUIF_CS0_LO_DECODE_ADDR, dynamic)
    NOT_INIT_MV64260_REG (CPUIF_CS0_HI_DECODE_ADDR, dynamic)
    NOT_INIT_MV64260_REG (DEVCTRL_DEV_BANK0_PARAMS, dynamic)

    /* 
     * The following registers permit access to the boot flash bank 
     * via low and high address decode registers and a bank parameter 
     * register.  They are are not statically initialized from this table 
     * but instead are dynamically initialized inside of 
     * sysMv64260DevBusInit().
     */

    NOT_INIT_MV64260_REG (CPUIF_BOOTCS_LO_DECODE_ADDR, dynamic)
    NOT_INIT_MV64260_REG (CPUIF_BOOTCS_HI_DECODE_ADDR, dynamic)
    NOT_INIT_MV64260_REG (DEVCTRL_DEV_BOOT_BANK_PARAMS, dynamic)

    /* 
     * The following registers permit access to the device controller
     * via low and high address decode registers and a bank parameter 
     * register.  They are are statically programmed via these table
     * entries.
     */

    YES_INIT_MV64260_REG (CPUIF_CS1_LO_DECODE_ADDR,
			  MV64260_LO_DECODE_ADDR(MV64260_DEVCS1_BASE_ADDR))

    YES_INIT_MV64260_REG (CPUIF_CS1_HI_DECODE_ADDR,
			  MV64260_HI_DECODE_ADDR(MV64260_DEVCS1_BASE_ADDR,
						 MV64260_DEVCS1_SIZE))

    YES_INIT_MV64260_REG (DEVCTRL_DEV_BANK1_PARAMS, 
                          DEVCTRL_DEV_BANK1_PARAMS_VAL)


    /* CS2 and CS3 related registers are not used, so we will disable them */

    YES_INIT_MV64260_REG (CPUIF_CS2_LO_DECODE_ADDR, 0x00000fff)
    YES_INIT_MV64260_REG (CPUIF_CS2_HI_DECODE_ADDR, 0x00000000)
    YES_INIT_MV64260_REG (DEVCTRL_DEV_BANK2_PARAMS, 
                          DEVCTRL_DEV_BANK2_PARAMS_VAL)


    YES_INIT_MV64260_REG (CPUIF_CS3_LO_DECODE_ADDR, 0x00000fff)
    YES_INIT_MV64260_REG (CPUIF_CS3_HI_DECODE_ADDR, 0x00000000)
    YES_INIT_MV64260_REG (DEVCTRL_DEV_BANK3_PARAMS, 
                          DEVCTRL_DEV_BANK3_PARAMS_VAL)
						 
    /*
     * Device interface control register defines things like device
     * timeout values and read trigger.
     */

    YES_INIT_MV64260_REG (DEVCTRL_DEV_IF_CTRL, DVCTRL_DEV_IF_CTRL_VAL)

    /* The two crossbar registers define slices of the "pizza" arbiter */

    YES_INIT_MV64260_REG (DEVCTRL_DEV_IF_XBAR_CTRL_LO,
                          DEVCTRL_DEV_IF_XBAR_CTRL_LO_VAL)

    YES_INIT_MV64260_REG (DEVCTRL_DEV_IF_XBAR_CTRL_HI,
                          DEVCTRL_DEV_IF_XBAR_CTRL_HI_VAL)


    /* Program the Device Interface Crossbar Timeout register */

    YES_INIT_MV64260_REG (DEVCTRL_DEV_IF_XBAR_TMOUT,
                          DEVCTRL_DEV_IF_XBAR_TMOUT_VAL)


    /*
     * The device interrupt cause and device interrupt mask registers
     * will be statically programmed to their default value of 0x00000000.
     */

    YES_INIT_MV64260_REG (DEVCTRL_DEV_INT_CAUSE, 0x00000000)
    YES_INIT_MV64260_REG (DEVCTRL_DEV_INT_MASK, 0x00000000)

    /*
     * The device error address will be dynamically read in the code
     * that follows to unlatch any possibly latched value.
     */

    NOT_INIT_MV64260_REG (DEVCTRL_DEV_ERR_ADDR, readToUlatch)

    /* Finally, we're at the end of the table. */

    END_INIT_MV64260_REG

    };

/* forward declarations */

void sysMv64260DevBusInit (void);

/* externals */

IMPORT VPD_PACKET * sysVpdPkts[]; /* board vpd packets */
IMPORT STATUS sysMotVpdPktGet (UCHAR  vpdType, UINT32 vpdInstance,
                               VPD_PACKET **pVpdPtr, VPD_PACKET ** pVpdPacket);
IMPORT UCHAR sysInByte (ULONG addr);

/******************************************************************************
*
* sysMv64260DevBusInit - Dynamic initialization of Mv64260 device bus interface
*
* This routine performs the dynamic initialization of the Mv64260 device bus
* interface registers.  It depends upon the I2C interface being functional.
*
* RETURNS: NA
*/

void sysMv64260DevBusInit (void)
    {

    UINT8	statReg;
    BOOL	bootedFromSoldered;
    int		i;		/* VPD packet counter */
    UCHAR  * v       = NULL;    /* Pointer to VPD packet */
    UCHAR  * devCs0  = NULL;
    UCHAR  * devBoot = NULL;
    UINT32   bankBase;
    UINT32   bankSize;
    UINT32   intfWidth;
    UINT32   bankParamVal;
		
    /* Initialize static device control parameters */

    sysMv64260RegInit (mv64260DevBusTbl);

    /*
     * Find out which bank we booted from (soldered (A) or socketed (B)).
     * Note that devCs1 region has already been programmed by the
     * previous call to sysMv64260RegInit().  This will allow us to
     * actually access the system status register.
     */

    statReg = sysInByte(SYSTEM_STATUS_REG_1);
    statReg &= SYSTEM_STATUS_REG_1_BANK_SELECT_MASK;

    bootedFromSoldered = (statReg == 0);

    /* Point devCs0 and devBoot to the correct VPD packets */

    for (i=0; i<=1; i++)
	{
	if (sysMotVpdPktGet(VPD_PID_FMC, i, &sysVpdPkts[0],
			    (VPD_PACKET **) &v) == ERROR)
	    {
	    logMsg("sysMv64260DevBusInit(): error reading VPD\n",0,0,0,0,0,0);
	    break;
	    }
	else
	    {
	    v += 2;	/* Advance to data portion of packet */
	    if ( (v [VPD_FMC_BANK] == 0) || (v [VPD_FMC_BANK] == 1))
		{
		if (bootedFromSoldered)
		    {

		    /*
		     * Bank selection jumper indicates a boot from bank A 
		     * (soldered flash).
		     */

		    if (v [VPD_FMC_BANK] == 0)  /* If flash bank A packet */
			devBoot = v;
		    if (v [VPD_FMC_BANK] == 1)  /* If flash bank B packet */
			devCs0 = v;
		    }
		else
		    {

		    /*
		     * Bank selection jumper indicates a boot from bank B
		     * (socketed flash).
		     */

		    if (v [VPD_FMC_BANK] == 0)  /* If flash bank A packet */
			devCs0 = v;
		    if (v [VPD_FMC_BANK] == 1)  /* If flash bank B packet */
			devBoot = v;
		    }
		}
	    else
		{
		logMsg("sysMv64260DevBusInit(): invalid Flash Config packet\n",
			0,0,0,0,0,0);
		}
	    }
	}

    /*
     * Now program the Device Bus registers that require dynamic
     * calculation from VPD data - first handle the case where we
     * have booted from bank A (soldered).
     */

    if (bootedFromSoldered)     /* If booting from Soldered - bank A */
	{
	intfWidth = 16 * 2;     /* Default value */

	/*
	 * Handle the devCs0 related programming, if VPD is available
	 * we'll use it to determine the size in bytes (legal sizes are
	 * 8MB, 16MB, 32MB, or 64MB) in width.  If VPD is not available then
	 * we will use default values.
	 */

	bankSize = SIZE_2MB;	/* Default bank B val in case VPD is botched */

	if (devCs0 != NULL)     /* If VPD data for this bank is available ... */
	    {
	    if ((devCs0 [VPD_FMC_SIZE] <= 8) && (devCs0 [VPD_FMC_SIZE] >= 5))
		bankSize =  (1 << devCs0 [VPD_FMC_SIZE]) * SIZE_256KB;

	    if ((devCs0 [VPD_FMC_DDW] == 8) || (devCs0 [VPD_FMC_DDW] == 16) ||
		(devCs0 [VPD_FMC_DDW] == 32))
		{

		/*
		 * Multiply VPD "device data width" by VPD "number of devices/
		 * sockets present" to obtain the value to program into
		 * the Discovery I.
		 */

		intfWidth = (devCs0 [VPD_FMC_DDW] * devCs0 [VPD_FMC_NOD]);
		}
	    }

	/* Program the base address register */

	bankBase = MV64260_FLSH_BANK_B_ADDR_BOOT_A;
	MV64260_WRITE32_PUSH (MV64260_REG_BASE, CPUIF_CS0_LO_DECODE_ADDR,
		     MV64260_LO_DECODE_ADDR(bankBase));	

	/* Program the size address register */

	MV64260_WRITE32_PUSH (MV64260_REG_BASE, CPUIF_CS0_HI_DECODE_ADDR,
			      MV64260_HI_DECODE_ADDR(bankBase, bankSize)); 

	/* Program the bank parameter register (dev width is variable) */

	bankParamVal = ((DEVCTRL_DEV_BANK0_PARAMS_VAL & ~(0x3 << 20)) |
			((intfWidth >> 4) << 20));

	MV64260_WRITE32_PUSH (MV64260_REG_BASE,
			      DEVCTRL_DEV_BANK0_PARAMS, bankParamVal);
	/*
	 * Handle the devBootCs related programming, if VPD is available
	 * we'll use it to determine the width.  If VPD is not available
	 * then we will use the default value.
	 */

	intfWidth = 16 * 2;     /* Default value */

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

	bankBase = MV64260_FLSH_BANK_A_ADDR;
	MV64260_WRITE32_PUSH (MV64260_REG_BASE, CPUIF_BOOTCS_LO_DECODE_ADDR,
		 	      MV64260_LO_DECODE_ADDR(bankBase)); 

	/*
	 * Program the size register:
	 * This bank must cover the entire range from the flash
	 * bank base address thru the end of 32-bit memory space
	 * (0xffffffff) in  order to make sure that  "shadowing" of
	 * bootrom covers the bootrom address range from
	 * 0xfff00000 thru 0xffffffff.
	 */

	bankSize = ( (UINT32) 0xffffffff -
		     (UINT32) MV64260_FLSH_BANK_A_ADDR ) + 1;

	MV64260_WRITE32_PUSH (MV64260_REG_BASE, CPUIF_BOOTCS_HI_DECODE_ADDR,
			      MV64260_HI_DECODE_ADDR (bankBase, bankSize));

	/* Program the bank parameter register (dev width is variable) */

	bankParamVal = ((DEVCTRL_DEV_BOOT_BANK_PARAMS_VAL & ~(0x3 << 20)) |
			((intfWidth >> 4) << 20));

	MV64260_WRITE32_PUSH (MV64260_REG_BASE,
			      DEVCTRL_DEV_BOOT_BANK_PARAMS, bankParamVal);
	}

    else        /* Boot from socketed - Bank B (not soldered) */

	{
	intfWidth = 16 * 2;     /* Default value */

	/*
	 * Handle the devCs0 related programming, if VPD is available
	 * we'll use it to determine the size in bytes (legal sizes are
	 * 8MB, 16MB, 32MB, 64MB, or 128MB) in width.  If VPD is not 
	 * available then  we will use default values.
	 */

	bankSize = SIZE_4MB;    /* Default bank A val in case VPD is botched */

	if (devCs0 != NULL)     /* If VPD data for this bank is available ... */
	    {
	    if ((devCs0 [VPD_FMC_SIZE] <= 9) && (devCs0 [VPD_FMC_SIZE] >= 5))
		bankSize =  (1 << devCs0 [VPD_FMC_SIZE]) * SIZE_256KB;

	    if ((devCs0 [VPD_FMC_DDW] == 8) || (devCs0 [VPD_FMC_DDW] == 16) ||
		(devCs0 [VPD_FMC_DDW] == 32))
		{

		/*
		 * Multiply VPD "device data width" by VPD "number of devices/
		 * sockets present" to obtain the value to program into
		 * the Discovery I.
		 */

		intfWidth = (devCs0 [VPD_FMC_DDW] * devCs0 [VPD_FMC_NOD]);
		}
	    }

	/* Program the base address register */

	bankBase = MV64260_FLSH_BANK_A_ADDR;
	MV64260_WRITE32_PUSH (MV64260_REG_BASE, CPUIF_CS0_LO_DECODE_ADDR,
		              MV64260_LO_DECODE_ADDR(bankBase));

	/* Program the size register */

	MV64260_WRITE32_PUSH (MV64260_REG_BASE, CPUIF_CS0_HI_DECODE_ADDR,
			      MV64260_HI_DECODE_ADDR (bankBase,bankSize));

	/* Program the bank parameter register (dev width is variable) */

	bankParamVal = ((DEVCTRL_DEV_BANK0_PARAMS_VAL & ~(0x3 << 20)) |
			((intfWidth >> 4) << 20));

	MV64260_WRITE32_PUSH (MV64260_REG_BASE,
			      DEVCTRL_DEV_BANK0_PARAMS, bankParamVal);
	/*
	 * Handle the devBootCs related programming, if VPD is available
	 * we'll use it to determine the width.  If VPD is not available
	 * then we will use the default value.
	 */

	intfWidth = 16 * 2;     /* Default value */

	if (devBoot != NULL)    /* If VPD data for this bank is available ... */
	    {
	    if ((devBoot [VPD_FMC_DDW] == 8) || (devBoot [VPD_FMC_DDW] == 16) ||
		(devBoot [VPD_FMC_DDW] == 32))
		{

		/* Perform calculation as we did for Cs0 case */

		intfWidth = (devBoot [VPD_FMC_DDW] * devBoot [VPD_FMC_NOD]);
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
		     (UINT32) MV64260_FLSH_BANK_B_ADDR_BOOT_B ) + 1;

	/* Program the base address register */

	bankBase = MV64260_FLSH_BANK_B_ADDR_BOOT_B;
	MV64260_WRITE32_PUSH (MV64260_REG_BASE, CPUIF_BOOTCS_LO_DECODE_ADDR,
		      MV64260_LO_DECODE_ADDR(bankBase));

	/* Program the size register */

	MV64260_WRITE32_PUSH (MV64260_REG_BASE, CPUIF_BOOTCS_HI_DECODE_ADDR,
			      MV64260_HI_DECODE_ADDR (bankBase, bankSize));

	/* Program the bank parameter register (dev width is variable) */

	bankParamVal = ((DEVCTRL_DEV_BOOT_BANK_PARAMS_VAL & ~(0x3 << 20)) |
			((intfWidth >> 4) << 20));

	MV64260_WRITE32_PUSH (MV64260_REG_BASE,
			     DEVCTRL_DEV_BOOT_BANK_PARAMS, bankParamVal);
	}

    /*
     * If an address is latched in the following register, reading the
     * register will unlatch it.
     */

    (void)MV64260_READ32 (MV64260_REG_BASE, DEVCTRL_DEV_ERR_ADDR);

    }
