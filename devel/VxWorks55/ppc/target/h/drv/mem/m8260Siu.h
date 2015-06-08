/* m8260Siu.h - Motorola MPC8260 System Integration Unit header file */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,07jun02,dtr  Changing size of PPC_ACR register.
01a,12sep99,ms_  created from m8260Cpm.h, 01d.
*/

/*
 * This file contains constants for System Integration Unit (SIU)
 * in the Motorola MPC8260 PowerQUICC II integrated Communications
 * Processor
 */

#ifndef __INCm8260Siuh
#define __INCm8260Siuh

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
 
/* General SIU registers */
 
/* SIU Module Configuration Register */

#define M8260_SIUMCR(base)      (CAST(VUINT32 *)((base) + 0x10000))
 
/* Protection Ctrl */

#define M8260_SYPCR(base)       (CAST(VUINT32 *)((base) + 0x10004))
 
/* SW Service Reg */

#define M8260_SWSR(base)        (CAST(VUINT16 *)((base) + 0x1000E))
 
/* Bus Configuration Register */

#define M8260_BCR(base)         (CAST(VUINT32 *)((base) + 0x10024))
 
/* 60x Bus Arbiter Configuration Register */

#define M8260_PPC_ACR(base)     (CAST(VUINT8 *)((base) + 0x10028))
 
/* 60x Bus Arbitration Level Register High */

#define M8260_PPC_ALRH(base)    (CAST(VUINT32 *)((base) + 0x1002c))
 
/* 60x Bus Transfer Error Control Status */

#define M8260_TESCR1(base)      (CAST(VUINT32 *)((base) + 0x10040))
 
/* Local Bus Transfer Error Control Status */

#define M8260_LTESCR1(base)     (CAST(VUINT32 *)((base) + 0x10048))

/* System Protection Control register bit definition (SYPCR - 0x04) */

#define M8260_SYPCR_SWTC        0xffff0000      /* Software 'dog Timer Count */
#define M8260_SYPCR_BMT         0x0000ff00      /* Bus Monitor Timing */
#define M8260_SYPCR_PBME        0x00000080      /* 60x Bus Monitor Enable */
#define M8260_SYPCR_LBME        0x00000040      /* local Bus Monitor Enable */
#define M8260_SYPCR_SWE         0x00000004      /* Software Watchdog Enable */
#define M8260_SYPCR_SWRI        0x00000002      /* Software Watchdog Reset/Int*/
#define M8260_SYPCR_SWP         0x00000001      /* Software Watchdog Prescale */

#ifdef __cplusplus
}
#endif

#endif /* __INCm8260Siuh */
