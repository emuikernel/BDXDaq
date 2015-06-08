/* sim.cdf - simulator windows specific components */

/* Copyright 1998-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,03may02,jmp  removed dummy INCLUDE_HW_FP that was used to overwrite real
		 INCLUDE_HW_FP component (SPR #76719).
01d,15apr02,hbh  Enabled INCLUDE_WDB_SYS selection.
01c,13nov01,jmp  fixed INCLUDE_WDB_COMM_END to exclude INCLUDE_WDB_SYS.
01b,23aug01,hbh  Renamed usrConfigIf.c to usrNetConfigIf.c (SPR 67728)
		 Cleaned up : created a sim.cdf file for simsolaris only.
		 Added INIT_RTN & CONFIGLETTES to NTPASSFS component(SPR 30368)
01a,09mar99,pr   moved ULIP components from 00vxWorks.cdf (SPR #25140)
		 added mod history
*/

/*
DESCRIPTION

This file contains components that are specific to the simulators only or that
need to be overwritten for the simulators.

XXX pr the INCLUDE_NT_ULIP component actually is not a real component, and is 
just part of the END driver component.
In order to create a separate entity we need to change the usrNetEndBoot.c
file. The only reason to do this would be to keep a sort of simmetry with the
other simulators (they have the INCLUDE_ULIP) but in reality it will be a 
bogus component. Therefore, for the moment is left commented out.
*/


Component INCLUDE_NTPASSFS {
        NAME            pass-through filesystem
        SYNOPSIS        direct access to host filesystem
        MODULES         ntPassFsLib.o
        INIT_RTN        usrNtPassFsInit ();
	CONFIGLETTES	usrNtPassFs.c
	HDR_FILES	ioLib.h
        _CHILDREN       FOLDER_IO_SYSTEM
        _INIT_ORDER     usrIosExtraInit
}

Component INCLUDE_DOS_DISK {
        NAME            dosDisk
        SYNOPSIS        vxSim DOS pass-through filesystem
        MODULES         unixDrv.o
        INIT_RTN        usrDosDisk ();
        CONFIGLETTES    usrDosDisk.c
        HDR_FILES       sysLib.h stdio.h string.h
	_CHILDREN	FOLDER_IO_SYSTEM
	_INIT_ORDER	usrIosExtraInit
}

Parameter CLEAR_BSS {
	DEFAULT		FALSE
}

Selection SELECT_WDB_COMM_TYPE {
	NAME		select WDB connection
	COUNT		1-1
	CHILDREN	INCLUDE_WDB_COMM_NETWORK	\
			INCLUDE_WDB_COMM_PIPE		\
			INCLUDE_WDB_COMM_END		\
			INCLUDE_WDB_COMM_CUSTOM
	DEFAULTS	INCLUDE_WDB_COMM_PIPE
	HELP		tgtsvr WDB
}

#if FALSE /* This should be reworked together with usrNetEndBoot.c */
Component INCLUDE_NT_ULIP {
        NAME            NT ULIP 
        SYNOPSIS        Support for ULIP devices used by NT simulator
        REQUIRES  	INCLUDE_END_BOOT
        _CHILDREN       FOLDER_NET_DEV
        _INIT_ORDER     usrNetworkDevStart
}
#endif

Component INCLUDE_WDB_COMM_END {
	EXCLUDES	INCLUDE_WDB_SYS
}
