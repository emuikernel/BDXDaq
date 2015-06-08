/* usrWdbDsp.c - optional DSP support for the WDB agent */

/* Copyright 1997-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,16nov00,zl	 written based on usrWdbFpp.c
*/

/*
DESCRIPTION

This library configures the WDB agent's DSP unit support.
*/

extern void wdbTgtHasDspSet(void);

/******************************************************************************
*
* wdbDspInit - initialize DSP support
*/

void wdbDspInit (void)
    {
    /* no DSP unit - just return */

    if (dspProbe() == ERROR)
	return;

    /* mark DSP unit as available */

    wdbTgtHasDspSet();
    }


