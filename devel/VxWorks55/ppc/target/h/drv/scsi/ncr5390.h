/* ncr5390.h - NCR 53C90 Advanced SCSI Controller header file */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,26apr02,dat  Adding cplusplus protection, SPR 75017
01a,07nov94,jds  written for SCSI1 compatabilty
*/

#ifndef __INCncr5390h
#define __INCncr5390h

#ifndef INCLUDE_SCSI2

#include "ncr5390_1.h"

#else

#include "ncr5390_2.h"

#endif /* INCLUDE_SCSI2 */

#endif /* __INCncr5390h */
