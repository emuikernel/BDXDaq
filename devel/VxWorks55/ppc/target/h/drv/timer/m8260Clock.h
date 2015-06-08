/* m8260Clock - Motorola MPC8260 Clock header file */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,12sep99,ms_  created from m8260Cpm.h, 01d.
*/

/*
 * This file contains constants for the clock block in the Motorola 
 * MPC8260 PowerQUICC II integrated Communications Processor
 */

#ifndef __INCm8260Clockh
#define __INCm8260Clockh

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef M8260ABBREVIATIONS
#define M8260ABBREVIATIONS

#ifdef  _ASMLANGUAGE
#define CAST(x)
#else /* _ASMLANGUAGE */
typedef volatile UCHAR VCHAR;   /* shorthand for volatile UCHAR */
typedef volatile INT32 VINT32; /* volatile unsigned word */
typedef volatile INT16 VINT16; /* volatile unsigned halfword */
typedef volatile INT8 VINT8;   /* volatile unsigned byte */
typedef volatile UINT32 VUINT32; /* volatile unsigned word */
typedef volatile UINT16 VUINT16; /* volatile unsigned halfword */
typedef volatile UINT8 VUINT8;   /* volatile unsigned byte */
#define CAST(x) (x)
#endif  /* _ASMLANGUAGE */

#endif /* M8260ABBREVIATIONS */

/*
 * MPC8260 internal register/memory map (section 17 of prelim. spec)
 * note that these are offsets from the value stored in the IMMR
 * register. Also note that in the MPC8260, the IMMR is not a special
 * purpose register, but it is memory mapped.
 */
 
/* System Clock Control Register */

#define M8260_SCCR(base)        (CAST(VUINT32 *)((base) + 0x10c80))

/* System Clock Control Register bit definition (SCCR - 0x10c80) */

#define M8260_SCCR_RES_MSK      0xfffffff8      /* reserved field Mask */
#define M8260_SCCR_CPM_LOW      0x00000004      /* CPM and SIU low power */
#define M8260_SCCR_DFBRG_4      0x00000000      /* divide by 4 */
#define M8260_SCCR_DFBRG_16     0x00000001      /* divide by 16 */
#define M8260_SCCR_DFBRG_64     0x00000002      /* divide by 64 */
#define M8260_SCCR_DFBRG_128    0x00000003      /* divide by 128 */
#define M8260_SCCR_TBS          0x02000000      /* Time Base Source */

#ifdef __cplusplus
}
#endif

#endif /* __INCm8260Clockh */
