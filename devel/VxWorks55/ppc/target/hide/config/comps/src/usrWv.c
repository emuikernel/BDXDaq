/* usrWv.c - windview configuration library */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,03jun98,ms   written
*/

/*
DESCRIPTION
Windview initialization code.
*/

/* globals */

char *          bufferAddress;          /* windview: address of event buffer */

/******************************************************************************
*
* usrWvInit - initialize windview
*/

void usrWvInit (void)
    {
    /* initialize WindView host target connection */

    connRtnSet (evtSockInit, evtSockClose, evtSockError, evtSockDataTransfer);

    /* set the characteristics of the event task - tEvtTask */

    wvEvtTaskInit (WV_EVT_STACK, WV_EVT_PRIORITY);

    /* initialize windview instrumentation buffers and tasks */

    bufferAddress = wvInstInit (EVTBUFFER_ADDRESS, EVTBUFFER_SIZE, WV_MODE);
    }

