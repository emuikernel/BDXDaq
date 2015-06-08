/* sim.cdf - simulator solaris specific components */

/* Copyright 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,22apr02,jmp  fixed INCLUDE_DOS_DISK init routine.
01a,11sep01,hbh  splitted sim.cdf file in two files for simsolaris and simnt.
*/

/*
DESCRIPTION

This file contains components that are specific to the simulators only or that
need to be overwritten for the simulators.
*/

Component INCLUDE_PASSFS {
        NAME            pass-through filesystem
        SYNOPSIS        direct access to host filesystem
        MODULES         passFsLib.o
        INIT_RTN        usrPassFsInit ();
        CONFIGLETTES    usrPassFs.c
	HDR_FILES	stdio.h ioLib.h
	_CHILDREN	FOLDER_IO_SYSTEM
	_INIT_ORDER	usrIosExtraInit
}

Component INCLUDE_DOS_DISK {
        NAME            dosDisk
        SYNOPSIS        vxSim DOS pass-through filesystem
        MODULES         unixDrv.o
        INIT_RTN        usrDosDiskInit ();
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
			INCLUDE_WDB_COMM_CUSTOM
	DEFAULTS	INCLUDE_WDB_COMM_PIPE
	HELP		tgtsvr WDB
}

Selection SELECT_WDB_MODE {
	NAME		select WDB mode
	COUNT		1-2
	CHILDREN	INCLUDE_WDB_TASK	\
			INCLUDE_WDB_SYS
	DEFAULTS	INCLUDE_WDB_TASK	\
			INCLUDE_WDB_SYS
	HELP		tgtsvr WDB
}

