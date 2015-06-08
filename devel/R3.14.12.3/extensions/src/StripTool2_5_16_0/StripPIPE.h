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

/*
 * StripPIPE
 *
 *      This module reads specially formatted data from a pipe (currently,
 *      standard input), and stores the most recently encountered tagged
 *      values for each named input set.  When request_connect() is called,
 *      the list of encountered names is scanned for the name of the
 *      supplied StripCurve object.  If found, the StripCurve is initialized
 *      with the associated data, and then sent to the Strip object.  If
 *      the name has not been encountered, it is flagged as pending, and
 *      its StripCurve will be initialized and sent to the Strip object
 *      once it is found on the input.
 *
 *      The format for data read on the pipe is:
 *
 *      <INPUT>         ==>     <SIG_DATA_LIST>
 *      <SIG_DATA_LIST> ==>     <SIG_DATA> | <SIG_DATA_LIST> <SIG_DATA>
 *      <SIG_DATA>      ==>     "NAME:" \" <STRING> \" <INFO>
 *      <STRING>        ==>     string of at most STRIP_MAX_NAME_CHAR bytes
 *      <INFO>          ==>     <INFO_LIST>
 *      <INFO_LIST>     ==>     <TAG> <VALUE> | <INFO_LIST> <TAG> <VALUE>
 *      <TAG>           ==>     "VAL:" | "EGU:" | "PREC:" | "MIN:" | "MAX:"
 *      <VALUE>         ==>     string of at most SP_MAX_VALUE_BYTES bytes
 */


#ifndef _StripPIPE
#define _StripPIPE

#include "Strip.h"
#include "StripCurve.h"
#include "StripDefines.h"
#include "StripMisc.h"

#define SP_MAX_VALUE_BYTES

typedef void *  StripPIPE;

/*
 * StripPIPE_initialize
 *
 *      Initializes pipe reader to read from standard input and
 *      allocates internal resources.
 */
StripPIPE               StripPIPE_initialize    (Strip);


/*
 * StripPIPE_terminate
 *
 *      Frees allocated resources.
 */
void            StripPIPE_terminate     (StripPIPE);


/*
 * StripPIPE_request_connect
 *
 *      Requests that the signal specified in the StripCurve be read
 *      from the standard input.
 */
int             StripPIPE_request_connect       (StripCurve, void *);


/*
 * StripPIPE_request_disconnect
 *
 *      Requests that the signal specified in the StripCurve be ignored
 *      if it is found on the standard input.
 */
int             StripPIPE_request_disconnect    (StripCurve, void *);

#endif
