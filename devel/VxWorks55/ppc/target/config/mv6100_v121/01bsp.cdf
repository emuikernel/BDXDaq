/* 01bsp.cdf - BSP component description file for MVME6100 BSP */

/* Copyright 2004-2005 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01b,13jan05,efb  Fixed syntax error.
01a,26oct04,cak  Initial writing.
*/

/*
DESCRIPTION
This file describes components that are specific to the MVME6100
BSP.

The configuration tool searches for .cdf files in four
places: target/config/comps/vxWorks, arch/<arch>, config/<bsp>, 
and the project directory.  These .cdf files are used to create
and configure the vxWorks "tree" structure you see in Project in
your workspace.  Once this has been created the config.h for the
BSP is parsed and the items in the "tree" are configured as per
config.h.
*/

/* 
 * MV64360 IDMA Controller Configuration 
 *
 * Allows DMA Controller support to be enabled/disabled, and allows
 * the setting of several DMA specific parameters.
 *
 * define/undef INCLUDE_MV64360_DMA
 * define/undef IDMA_SNOOP_ON
 * IDMA_DST_BURST_LIMIT = 
 *   {IDMA_CNTL_DBURST_LIMIT_32B,IDMA_CNTL_DBURST_LIMIT_128B}
 * IDMA_SRC_BURST_LIMIT = 
 *   {IDMA_CNTL_SBURST_LIMIT_32B,IDMA_CNTL_SBURST_LIMIT_128B}
 */

Component INCLUDE_MV64360_DMA {
	NAME		mv64360 idma controller support
	SYNOPSIS	MV64360 IDMA Controller Support
	_CHILDREN	FOLDER_BSP_CONFIG
	CFG_PARAMS	IDMA_SNOOP_ON		\
			IDMA_DST_BURST_LIMIT	\
			IDMA_SRC_BURST_LIMIT
}

Parameter IDMA_SNOOP_ON {
	NAME		enable dma snoop
	TYPE		exists
	DEFAULT		TRUE
}

Parameter IDMA_DST_BURST_LIMIT {
	NAME		idma destination burst limit
	SYNOPSIS	Valid values are IDMA_CNTL_DBURST_LIMIT_32B for a burst limit of 32 bytes, and IDMA_CNTL_DBURST_LIMIT_128B for a burst limit of 128 bytes.  If DMA snoop is enabled the burst limit should be set to 32 bytes to maintain cache coherency.	
	DEFAULT		IDMA_CNTL_DBURST_LIMIT_32B
}

Parameter IDMA_SRC_BURST_LIMIT {
	NAME		idma source burst limit
	SYNOPSIS	Valid values are IDMA_CNTL_SBURST_LIMIT_32B for a burst limit of 32 bytes, and IDMA_CNTL_SBURST_LIMIT_128B for a burst limit of 128 bytes.  If DMA snoop is enabled the burst limit should be set to 32 bytes to maintain cache coherency.	
	DEFAULT		IDMA_CNTL_SBURST_LIMIT_32B
}

/* 
 * Error Handling Suite 
 *
 * Allows the error handling suite support to be enabled/disabled.
 *
 * define/undef INCLUDE_ERROR_HANDLING
 */

Component INCLUDE_ERROR_HANDLING {
	NAME		error handling suite	
	SYNOPSIS	enables error handling suite	
	_CHILDREN	FOLDER_BSP_CONFIG
}

/* 
 * SGS-Thompson M48T37V TimeKeeper SRAM Real-Time Clock 
 *
 * Allows the real-time clock support to be enabled/disabled.
 *
 * define/undef INCLUDE_RTC
 */

Component INCLUDE_RTC {
	NAME		m48t37v timekeeper real-time clock
	SYNOPSIS	enables real-time clock of m48t37v timekeeper sram
	_CHILDREN	FOLDER_BSP_CONFIG
	INCLUDE_WHEN	INCLUDE_ERROR_HANDLING
}

/* 
 * SGS-Thompson M48T37V TimeKeeper SRAM Failsafe(watchdog) Timer 
 *
 * Allows the failsafe timer support to be enabled/disabled.
 *
 * define/undef INCLUDE_FAILSAFE
 */

Component INCLUDE_FAILSAFE {
	NAME		m48t37v timekeeper failsafe timer
	SYNOPSIS	enables failsafe timer on m48t37v timekeeper sram
	_CHILDREN	FOLDER_BSP_CONFIG
}

/* 
 * Processor Dynamic Power Management 
 *
 * Allows the processor dynamic power management to be enabled/disabled.
 *
 * define/undef INCLUDE_DPM
 */

Component INCLUDE_DPM {
	NAME		processor dynamic power management
	SYNOPSIS	enables processor dynamic power management
	_CHILDREN	FOLDER_BSP_CONFIG
}

/*
 * Flash Boot Image Programming
 *
 * This allows the boot image programming support to be enabled/disabled.
 *
 * define/undef INCLUDE_BOOT_IMAGE_PROGRAM
 */

Component INCLUDE_BOOT_IMAGE_PROGRAM {
	NAME		flash boot image programming support
	REQUIRES	INCLUDE_USER_APPL
	_CHILDREN	FOLDER_BSP_CONFIG
}

Parameter  USER_APPL_INIT {
	NAME		User application initialization code fragment
	_CFG_PARAMS	INCLUDE_USER_APPL
	TYPE string
	DEFAULT		"sysApplInit()"
}

/*
 * MV64360 Watchdog Timer Support
 *
 * This allows the mv64360's watchdog timer support to be enabled/disabled.
 *
 * define/undef INCLUDE_MV64360_WDOG
 */

Component INCLUDE_MV64360_WDOG {
	NAME		mv64360 watchdog timer support
	SYNOPSIS	enable support for the mv64360s watchdog timer
	_CHILDREN	FOLDER_BSP_CONFIG
}

/*
 * Maxim DS1621 Temp Sensor Support
 *
 * This allows the support for the Maxim DS1621 Temperature sensor
 * to be enabled/disabled.
 *
 * define/undef INCLUDE_DS1621
 */

Component INCLUDE_DS1621 {
	NAME		maxim ds1621 temperature sensor support
	SYNOPSIS	enable support for the maxim ds1621 temp sensor
	_CHILDREN	FOLDER_BSP_CONFIG
}

/*
 * MV64360 Timer/Counter Support 
 *
 * This allows the support for the MV64360s timer/counters 
 * to be enabled/disabled.
 *
 * define/undef INCLUDE_MV64360_TMRCNTR 
 */

Component INCLUDE_MV64360_TMRCNTR {
	NAME		mv64360 timer/counter support	
	SYNOPSIS	enable support for the mv64360 timer/counter support 
	_CHILDREN	FOLDER_BSP_CONFIG
}

/* 
 * AuxClock Timer/Counter Selection 
 *
 * This allows the user to select which of the mv64360's 
 * timer/counters to use for the vxWorks Aux Clock.
 *
 * MV64360_AUXCLK_TMR = {0, 1, 2, 3}
 */

Parameter MV64360_AUXCLK_TMR {
	NAME		mv64360 aux clock timer
	_CFG_PARAMS	INCLUDE_AUX_CLK
	TYPE		uint
	DEFAULT		0
}
