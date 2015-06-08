/* usrWdbAltivec.c - optional Altivec support for the WDB agent */

/* Copyright 1997-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,10apr01,kab  created.
*/

/*
DESCRIPTION

This library configures the WDB agent's Altivec support.
*/

extern void wdbTgtHasAltivecSet(void);

/******************************************************************************
*
* wdbAltivecInit - initialize Altivec support
*/

void wdbAltivecInit (void)
    {
    /* no altivec - return */
    if (altivecProbe() == ERROR)
        return;

    /* mark altivec unit as available */
    wdbTgtHasAltivecSet();
    }




