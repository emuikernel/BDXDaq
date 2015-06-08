/* 00visionTMDriver.cdf - Wind River visionTMD component description file */

/* Copyright 1988-2002 Wind River Systems Inc. */

/*
modification history
--------------------
01b,30nov01,g_h  Cleaning for T2.2
01a,30may01,g_h  written

*/

/*
DESCRIPTION
This file contains descriptions for the TM Driver.
The configuration tool searches for .cdf files in four
places; here, arch/<arch>, config/<bsp>, and the project directory.
*/

Component  INCLUDE_WDB_COMM_VTMD {

    NAME         WDB visionTMD connection
    SYNOPSIS     Transparent Mode Driver connection using visionPROBE II or visionICE II
    CONFIGLETTES wdbVisionDriver.c
    CFG_PARAMS   TMD_DEFAULT_POLL_DELAY \
                 WDB_COMM_VTMD
    MACRO_NEST	 INCLUDE_WDB
    HDR_FILES    wdb/wdbUdpLib.h wdb/wdbLib.h wdb/wdbLibP.h drv/wdb/vision/wdbVisionPktDrv.h
    _CHILDREN    SELECT_WDB_COMM_TYPE
    REQUIRES     INCLUDE_WDB
    HELP         tgtsvr wdbVisionTMDPktDrv WDB
}

Parameter TMD_DEFAULT_POLL_DELAY {

    SYNOPSIS     clock tick interval to poll for data
    DEFAULT      2
    TYPE         uint
}

Parameter WDB_COMM_VTMD {

    SYNOPSIS     WDB Comm Device Number (used for switch/case)
    DEFAULT      8
    TYPE         uint
}

