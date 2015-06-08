/* bsp.cdf - BSP-specific component descriptor file */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,08nov01,jmp  added PPP configuration parameters.
01c,29oct01,hbh  Removed INCLUDE_END component.
01b,06sep01,hbh  Added undefined parameters (SPR 67728).
		 Replaced ULIP component include when nework is included by
		 PPP component. ULIP is no more supported on Solaris 2.7.
01a,30Sep98,ms   written
*/

/*
DESCRIPTION

This file contains BSP-specific changes to the generic component
descriptor files.
*/

/* include the PPP driver whenever the network is included */

Component INCLUDE_PPP {
	INCLUDE_WHEN	INCLUDE_NET_INIT
}

Folder FOLDER_BSP_CONFIG {
	NAME		BSP configuration variants
	SYNOPSIS	solaris configuration parameters
	CHILDREN	INCLUDE_SOLARIS_NET_CONFIG
}

Component INCLUDE_SOLARIS_NET_CONFIG {
	NAME		Full solaris simulator configuration parameters
	SYNOPSIS	configuration parameters for full solaris simulator
	CFG_PARAMS	PPP_PSEUDO_TTY_PATH \
			VXSIM_IP_ADDR
	INCLUDE_WHEN	INCLUDE_PPP
}

Parameter ROM_TEXT_ADRS { 
	NAME 		ROM text address 
	SYNOPSIS 	ROM text address 
	DEFAULT		(ROM_BASE_ADRS) 
}

Parameter ROM_BASE_ADRS { 
	NAME 		ROM base address 
	SYNOPSIS 	ROM base address 
	DEFAULT		0x0
}

Parameter ROM_SIZE { 
	NAME 		ROM size 
	SYNOPSIS	ROM size 
	DEFAULT		0x0
}

Parameter PPP_PSEUDO_TTY_PATH {
	NAME		PPP pseudo terminal path
	SYNOPSIS	Default pseudo terminal path for PPP (Solaris 2.9 only)
	DEFAULT		"/dev/ptyr%x"
}

Parameter VXSIM_IP_ADDR  {
	NAME		vxsim IP address
	SYNOPSIS	IP base address for vxWorks simulator
	DEFAULT		"192.168.255.%d"
}
