/* s68154.h - Signetics 68154 Interrupt Generator header file */

/* Copyright 1984-1993 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,02mar93,eve    upgraded for 5.1
01a,19mar90,trl    written
*/

#ifndef __INCs68154h
#define __INCs68154h


#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

/* registers */

#define IGR_VEC_REG(base)      ((char *) (base) + 0)  /* vector r/w  */
#define IGR_REQ_REG(base)      ((char *) (base) + 2)  /* request r/w */


/* interrupt vector register */

#define IGR_ENABLE_INT         0x02
#define IGR_CLEAR_INT          0x04
#define IGR_INT_MASK           0xf8  /* bits 3-7 of interrupt vector */


/* interrupt request register */

#define IGR_IRQ1               0x02
#define IGR_IRQ2               0x04
#define IGR_IRQ3               0x08
#define IGR_IRQ4               0x10
#define IGR_IRQ5               0x20
#define IGR_IRQ6               0x40
#define IGR_IRQ7               0x80

#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCs68154h */
