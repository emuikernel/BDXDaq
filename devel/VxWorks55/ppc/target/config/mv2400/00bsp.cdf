/* 00bsp.cdf - BSP specific component descriptions for mv2400 BSP */

/* Copyright 1984-2002 Wind River Systems, Inc.  Alameda, CA */

/*
modification history
--------------------
01a,25apr02,dat  written (from SPR 30712)
*/


/*
 * This makes the AUX_CLK and SPY components require the 
 * HAWK_AUXCLK component.  Without this, the customer has
 * manually edit config.h in the BSP to add/subtract the
 * HAWK_AUXCLK as needed. (SPR 30712)
 */

Component INCLUDE_HAWK_AUXCLK {
	NAME            Hawk Auxiliary Clock for mv2400
	SYNOPSIS        Hawk Auxiliary Clock for mv2400
	_CHILDREN	FOLDER_BSP_CONFIG
	_REQUIRES       INCLUDE_AUX_CLK INCLUDE_SPY
}

