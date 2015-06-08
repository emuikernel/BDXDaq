/* usrWdbTaskAltivec.c - optional Altivec support for the extern WDB agent */

/* Copyright 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,15apr01,kab  created
*/

/*
DESCRIPTION

This library configures the external WDB agent's altivec support.
*/

/* externals */

extern BOOL	wdbIsInitialized;

/******************************************************************************
*
* wdbSysAltivecInit - initialize AltiVec support
*
* This routine initializes AltiVec support for the external
* WDB agent.
*
* NOMANUAL
*/

void wdbSysAltivecInit (void)
    {
    WDB_REG_SET_OBJ * pAvRegs;

    /* install system mode agent hook for the Altivec unit */

    if (wdbTgtHasAltivec() && wdbIsInitialized)
	{
	pAvRegs = wdbAltivecLibInit();
	wdbExternRegSetObjAdd (pAvRegs);
	}
    }
