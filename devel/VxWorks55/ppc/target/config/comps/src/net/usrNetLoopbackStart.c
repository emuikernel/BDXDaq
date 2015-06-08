/* usrNetLoopbackStart.c - Start the loopback interface */

/* Copyright 1992 - 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,19aug98,ann  removed call to usrNetIfAttach and made an
                 explicit call to loattach (i.e removed BSD dependancy)
01a,05dec97,spm  added DHCP code review modifications
*/


/*******************************************************************************
*
* usrNetLoopbackStart - start the loopback device
*
* This routine attaches the loopback driver with IP address 127.0.0.1 and
* host name "localhost". It is the initialization routine for the
* INCLUDE_LOOPBACK_START component which is included automatically by
* the configuration tool when INCLUDE_LOOPBACK is defined.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetLoopbackStart (void)
    {
    printf ("Attaching interface lo0...");

    if (loattach () != OK)
	return;

    printf ("done\n");

    if (usrNetIfConfig ("lo", 0, "127.0.0.1", "localhost", 0) != OK)
	return;

    return; 
    }

