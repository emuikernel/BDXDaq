/* m8260IOPort.h - Motorola MPC8260 I/O Port header file */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,12sep99,ms_  created from m8260Sio.h, 01b
*/

/*
 * This file contains constants for the I/O Ports in the Motorola 
 * MPC8260 PowerQUICC II integrated Communications Processor
 */

#ifndef __INCm8260IOPorth
#define __INCm8260IOPorth
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE


/* I/O Port Configuration Registers */

/*** Port A data direction reg ***/

#define	M8260_IOP_PADIR(base)	((VINT32 *) ((base) + 0x010D00))

/*** Port A pin assign reg ***/

#define	M8260_IOP_PAPAR(base)	((VINT32 *) ((base) + 0x010D04))

/*** Port A special options reg ***/

#define	M8260_IOP_PASO(base)	((VINT32 *) ((base) + 0x010D08))

/*** Port A open drain reg ***/

#define	M8260_IOP_PAODR(base)	((VINT32 *) ((base) + 0x010D0C))

/*** Port A data reg ***/

#define	M8260_IOP_PADAT(base)	((VINT32 *) ((base) + 0x010D10))

/*** Port B data direction reg ***/

#define	M8260_IOP_PBDIR(base)	((VINT32 *) ((base) + 0x010D20))

/*** Port B pin assign reg ***/

#define	M8260_IOP_PBPAR(base)	((VINT32 *) ((base) + 0x010D24))

/*** Port B special options reg ***/

#define	M8260_IOP_PBSO(base)	((VINT32 *) ((base) + 0x010D28))

/*** Port B open drain reg ***/

#define	M8260_IOP_PBODR(base)	((VINT32 *) ((base) + 0x010D2C))

/*** Port B Data register ***/

#define	M8260_IOP_PBDAT(base)	((VINT32 *) ((base) + 0x010D30))

/*** Port C data direction reg ***/

#define	M8260_IOP_PCDIR(base)	((VINT32 *) ((base) + 0x010D40))

/*** Port C pin assign reg ***/

#define	M8260_IOP_PCPAR(base)	((VINT32 *) ((base) + 0x010D44))

/*** Port C special options reg ***/

#define	M8260_IOP_PCSO(base)	((VINT32 *) ((base) + 0x010D48))

/*** Port C open drain reg ***/

#define	M8260_IOP_PCODR(base)	((VINT32 *) ((base) + 0x010D4C))

/*** Port C data reg ***/

#define	M8260_IOP_PCDAT(base)	((VINT32 *) ((base) + 0x010D50))

/*** Port D data direction reg ***/

#define	M8260_IOP_PDDIR(base)	((VINT32 *) ((base) + 0x010D60))

/*** Port D pin assign reg ***/

#define	M8260_IOP_PDPAR(base)	((VINT32 *) ((base) + 0x010D64))

/*** Port D special options reg ***/

#define	M8260_IOP_PDSO(base)	((VINT32 *) ((base) + 0x010D68))

/*** Port D open drain reg ***/

#define	M8260_IOP_PDODR(base)	((VINT32 *) ((base) + 0x010D6C))

/*** Port D data reg ***/

#define	M8260_IOP_PDDAT(base)	((VINT32 *) ((base) + 0x010D70))

#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCm8260IOPorth */
