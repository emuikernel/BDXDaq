/* mb87030.h - Fujitsu MB87030 SPC (SCSI Protocol Controller) header */

/* Copyright 1984-2002 Wind River Systems, Inc. */
/*
modification history
--------------------
01b,26apr02,dat  Adding cplusplus protection, SPR 75017
01a,07nov94,jds  written for backward compatability.
*/

#ifndef __INCmb87030h
#define __INCmb87030h

#ifndef INCLUDE_SCSI2

#include "mb87030_1.h"

#else

#error "mb87030 driver does not support SCSI2"

#endif /* INCLUDE_SCSI2 */

#endif /* __INCmb87030h */
