/* harrierMpic.h - harrier specific MPIC register definitions */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996,1998-2001 Motorola, Inc. */

/*
modification history
--------------------
01c,10oct01,scb  Add EIEIO_SYNCs to quash spurious interrupts
01b,07sep00,krp  Added the Harrier MPIC support.
*/

/*
Description:

Contains structure typedefs and device-specific defines for the Motorola MPIC..
*/


#ifndef INCharrierMpich
#define INCharrierMpich

#ifdef __cplusplus
    extern "C" {
#endif

/* macros */

#define MPIC_ADDR(reg)	  (MPIC_BASE_ADRS + reg)
#define MPIC_WRITE(X,Y)   { (*(UINT32 *) (MPIC_ADDR(X)) = Y); EIEIO_SYNC; }
#define MPIC_READ(X,Y)	  { (Y = *(UINT32 *) (MPIC_ADDR(X))); EIEIO_SYNC; }

#ifdef __cplusplus
    }
#endif

#endif	/* INCharrierMpich */

