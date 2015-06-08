/* usrWdbTaskFpp.c - optional floating point support for the extern WDB agent */

/* Copyright 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,22may98,dbt	 taken from 01a of usrWdbFpp.c
*/

/*
DESCRIPTION

This library configures the external WDB agent's hardware floating point
support.
*/

/* externals */

extern BOOL	wdbIsInitialized;

/******************************************************************************
*
* wdbSysFppInit - initialize hardware floating point support
*
* This routine initializes hardware floating point support for the external
* WDB agent.
*
* NOMANUAL
*/

void wdbSysFppInit (void)
    {
    WDB_REG_SET_OBJ * pFpRegs;

    /* install system mode agent hook for the fpp unit */

    if (wdbTgtHasFpp() && wdbIsInitialized)
	{
	pFpRegs = wdbFpLibInit();
	wdbExternRegSetObjAdd (pFpRegs);
	}
    }
