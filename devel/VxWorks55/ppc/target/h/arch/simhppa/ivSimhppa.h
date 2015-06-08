/* ivSimhppa.h - simhppa interrupt vectors */

/* Copyright 1993 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,01jan96,kab  added _ASMLANGUAGE
01a,11aug93,gae  from rrr.
*/

#ifndef __INCivSimhppa
#define __INCivSimhppa

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* macros to convert interrupt vectors <-> interrupt numbers */

#define IVEC_TO_INUM(intVec)    ((int) (intVec))
#define INUM_TO_IVEC(intNum)    ((VOIDFUNCPTR *) (intNum))

#define IVEC_TO_FD(intVec)      ((intVec) - 32)
#define FD_TO_IVEC(fd)          ((fd) + 32)

/* interrupt vector definitions */

#define IV_HUP          INUM_TO_IVEC (1)
#define IV_INT          INUM_TO_IVEC (2)
#define IV_ILL          INUM_TO_IVEC (4)
#define IV_TRAP         INUM_TO_IVEC (5)
#define IV_FPE          INUM_TO_IVEC (8)
#define IV_BUS          INUM_TO_IVEC (10)
#define IV_SEGV         INUM_TO_IVEC (11)
#define IV_CLOCK1       INUM_TO_IVEC (14)
#define IV_USR1         INUM_TO_IVEC (16)
#define IV_USR2         INUM_TO_IVEC (17)
#define IV_CLOCK2       INUM_TO_IVEC (20)
#define IV_CLOCK3       INUM_TO_IVEC (21)
#define IV_IO           INUM_TO_IVEC (22)

#ifdef __cplusplus
}
#endif

#endif /* _ASMLANGUAGE */

#endif /* __INCivSimhppa */
