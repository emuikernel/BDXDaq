/* usrWdbTaskDsp.c - optional DSP support for the extern WDB agent */

/* Copyright 1998-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,16nov00,zl	 written based on usrWdbSysFpp.c
*/

/*
DESCRIPTION

This library configures the external WDB agent's DSP support.
*/

/* externals */

extern BOOL	wdbIsInitialized;

/******************************************************************************
*
* wdbSysDspInit - initialize DSP support
*
* This routine initializes DSP support for the external WDB agent.
*
* NOMANUAL
*/

void wdbSysDspInit (void)
    {
    WDB_REG_SET_OBJ * pDspRegs;

    /* install system mode agent hook for the DSP unit */

    if (wdbTgtHasDsp() && wdbIsInitialized)
	{
	pDspRegs = wdbDspLibInit();
	wdbExternRegSetObjAdd (pDspRegs);
	}
    }
