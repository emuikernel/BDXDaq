/* ncr710Script.h - NCR 710 Script definition header */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,26apr02,dat  Adding cplusplus protection, SPR 75017
01a,23oct94,jds	 Created for backward compatability
*/

#ifndef __INCncr710Scripth
#define __INCncr710Scripth

#ifndef INCLUDE_SCSI2

#include "ncr710Script1.h"

#else

#include "ncr710Script2.h"

#endif /* INCLUDE_SCSI2 */

#endif /* __INCncr710Scripth */
