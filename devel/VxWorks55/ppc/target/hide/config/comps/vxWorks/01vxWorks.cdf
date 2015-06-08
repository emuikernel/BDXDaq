/*
Copyright 1984 - 2002 Wind River Systems, Inc.

modification history
--------------------
01t,11jul02,j_s  add WDB_MTU to INCLUDE_WDB_COMM_END as a configuration
		 parameter (SPR 74291)
01s,26jun00,niq  written

DESCRIPTION

This file contains updated descriptions for some vxWorks components
defined in 00vxWorks.cdf. It updates those definitons with the new ones
needed for the Raptor release
*/

Component INCLUDE_WDB_COMM_END {
	NAME		WDB END driver connection
	SYNOPSIS	supports both task and system mode debugging
	CONFIGLETTES	wdbEnd.c
	CFG_PARAMS      WDB_MTU \
			MUX_MAX_BINDS 
	MODULES		wdbEndPktDrv.o
	MACRO_NEST	INCLUDE_WDB
	HDR_FILES	wdb/wdbUdpLib.h wdb/wdbLib.h \
			wdb/wdbLibP.h drv/wdb/wdbEndPktDrv.h \
			private/funcBindP.h end.h muxLib.h \
			muxTkLib.h
	REQUIRES	INCLUDE_END
	HELP		tgtsvr WDB
}

Parameter MUX_MAX_BINDS
        {
        NAME            MUX max bind value
        SYNOPSIS        Default number of bindings that MUX allows
        TYPE            uint
        DEFAULT         16
        }
