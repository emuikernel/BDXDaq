/* sysMv64360DevBus.c - Initialize the Mv64360 device bus support registers */

/* Copyright 2003 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01a,02apr03,simon  Ported. from ver 01d, mcpm905/sysMv64360DevBus.c.
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

	/* Sergey: change 2 following lines: f1000028 is our flash bank window
       and we do not want it do disappeare !!!
	  YES_INIT_MV64360_REG (CPUIF_DEVCS0_BASE_ADDR , 0)
	  YES_INIT_MV64360_REG (CPUIF_DEVCS0_SIZE      , 0)*/
    NOT_INIT_MV64360_REG (CPUIF_DEVCS0_BASE_ADDR , 0)
    NOT_INIT_MV64360_REG (CPUIF_DEVCS0_SIZE      , 0)

    YES_INIT_MV64360_REG (CPUIF_DEVCS1_BASE_ADDR ,	  0)
    YES_INIT_MV64360_REG (CPUIF_DEVCS1_SIZE      , 0)

    YES_INIT_MV64360_REG (CPUIF_DEVCS2_BASE_ADDR , (MV64360_DEVCS2_BASE_ADDR >> 16))   
    YES_INIT_MV64360_REG (CPUIF_DEVCS2_SIZE      , MV64360_SIZE_FIELD (MV64360_DEVCS2_SIZE))  

    YES_INIT_MV64360_REG (CPUIF_DEVCS3_BASE_ADDR , 0)   /* Unused */
    YES_INIT_MV64360_REG (CPUIF_DEVCS3_SIZE      , 0)   /* Unused */

    NOT_INIT_MV64360_REG (CPUIF_BOOTCS_BASE_ADDR , dynamic)

    NOT_INIT_MV64360_REG (CPUIF_BOOTCS_SIZE      , dynamic)

    YES_INIT_MV64360_REG (DVCTL_DEVICE_BANK0_PARAMS, DVCTL_DEVICE_BANK0_PARAMS_VAL)

    YES_INIT_MV64360_REG (DVCTL_DEVICE_BANK1_PARAMS, \
			  DVCTL_DEVICE_BANK1_PARAMS_VAL)

    YES_INIT_MV64360_REG (DVCTL_DEVICE_BANK2_PARAMS, \
			  DVCTL_DEVICE_BANK2_PARAMS_VAL)

    NOT_INIT_MV64360_REG (DVCTL_DEVICE_BANK3_PARAMS,  dynamic)

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
    UINT32   baseEnable;

    /* Initialize static device control parameters */

/*gorit*/

    sysMv64360RegInit (mv64360DvctlTbl);

/*ne gorit*/

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
     * the bits for DevCs0, DevCs1, DevCs2 and BootCs - which will enable
     * these windows.
     */

    baseEnable = MV64360_READ32 (MV64360_REG_BASE, CPUIF_BASE_ADDR_ENABLE);

    baseEnable |= ( (1 << CPUIF_BASE_ADDR_ENABLE_DEVCS0_BIT) |
		    (1 << CPUIF_BASE_ADDR_ENABLE_DEVCS1_BIT) |
		    (1 << CPUIF_BASE_ADDR_ENABLE_DEVCS2_BIT) |
		    (1 << CPUIF_BASE_ADDR_ENABLE_DEVCS3_BIT) |
		    (1 << CPUIF_BASE_ADDR_ENABLE_BOOTCS_BIT) );

    baseEnable &= ~( (1 << CPUIF_BASE_ADDR_ENABLE_DEVCS2_BIT) |
		     (1 << CPUIF_BASE_ADDR_ENABLE_BOOTCS_BIT) );

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, CPUIF_BASE_ADDR_ENABLE,
			  baseEnable);
    }
