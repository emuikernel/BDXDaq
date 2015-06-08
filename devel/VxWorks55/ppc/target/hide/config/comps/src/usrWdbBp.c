/* usrWdbBp.c - configuration file for WDB agent breakpoints */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,21jul97,ms   taken from 01y of usrWdb.c
*/

/*
DESCRIPTION

This library configures and initializes the WDB agents system
breakpoint library.
*/

/* locals */

static struct brkpt wdbBreakPoints [WDB_BP_MAX];

/* externals */

extern VOIDFUNCPTR	wdbBpSysEnterHook;
extern VOIDFUNCPTR	wdbBpSysExitHook;

/******************************************************************************
*
* usrWdbBp - configure and initialize the WDB agents breakpoint library
*
*/

void usrWdbBp (void)
    {
    wdbSysBpLibInit (wdbBreakPoints, WDB_BP_MAX);
    wdbBpSysEnterHook = wdbDbgBpRemoveAll;
    wdbBpSysExitHook = wdbBpInstall;
    }

