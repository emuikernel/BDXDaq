/* usrWdbFpp.c - optional floating point support for the WDB agent */

/* Copyright 1997-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,22may98,dbt	 moved fpp task mode support in usrWdbTaskFpp.c and 
		 system mode support in usrWdbSysFpp.c.
01a,21jul97,ms	 taken from 01y of usrWdb.c
*/

/*
DESCRIPTION

This library configures the WDB agent's hardware floating point
support.
*/

extern void wdbTgtHasFppSet(void);

/******************************************************************************
*
* wdbFppInit - initialize hardware floating point support
*/

void wdbFppInit (void)
    {
    /* no fpp unit - just return */

    if (fppProbe() == ERROR)
	return;

    /* mark fpp unit as available */

    wdbTgtHasFppSet();
    }


