/* ivSimnt.h - simnt interrupt vectors */

/* Copyright 1993-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,06oct97,cym  written
*/

#ifndef __INCivSimnth
#define __INCivSimnth

#ifdef __cplusplus
extern "C" {
#endif

/* macros to convert interrupt vectors <-> interrupt numbers */

#define IVEC_TO_INUM(intVec)    	(intVec)
#define INUM_TO_IVEC(intNum)    	(intNum)

#define IVEC_TO_MESSAGE(intVec) 	(intVec)
#define MESSAGE_TO_IVEC(message)        (message)

/* interrupt vector definitions */

#ifdef __cplusplus
}
#endif

#endif /* __INCivSimnth */
