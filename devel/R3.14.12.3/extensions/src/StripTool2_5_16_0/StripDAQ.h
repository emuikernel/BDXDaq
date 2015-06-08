/*************************************************************************\
* Copyright (c) 1994-2004 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 1997-2003 Southeastern Universities Research Association,
* as Operator of Thomas Jefferson National Accelerator Facility.
* Copyright (c) 1997-2002 Deutches Elektronen-Synchrotron in der Helmholtz-
* Gemelnschaft (DESY).
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

#ifndef _StripDAQ
#define _StripDAQ

#include "Strip.h"
#include "StripCurve.h"
#include "StripDefines.h"
#include "StripMisc.h"

typedef void *  StripDAQ;

/*
 * StripDAQ_initialize
 *
 *      Initializes DAQ subsystem and allocates internal resources.
 */
StripDAQ StripDAQ_initialize (Strip);


/*
 * StripDAQ_terminate
 *
 *      Exits Channel Access, freeing allocated resources.
 */
void StripDAQ_terminate (StripDAQ);


/*
 * StripDAQ_request_connect
 *
 *      Requests that the signal specified in the StripCurve be connected
 *      to the DAQ subsystem.
 */
int StripDAQ_request_connect (StripCurve, void *);


/*
 * StripDAQ_request_disconnect
 *
 *      Requests that the signal specified in the StripCurve be disconnected
 *      from the DAQ subsystem.
 */
int StripDAQ_request_disconnect (StripCurve, void *);

/*
 * StripDAQ_retry_connections
 *
 *      Tries to reconnect to currently unconnected PVs.
 */
int StripDAQ_retry_connections (StripDAQ the_sca, Display *display);

#endif
