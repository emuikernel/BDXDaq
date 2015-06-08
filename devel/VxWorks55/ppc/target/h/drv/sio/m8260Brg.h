/* m8260Brg.h - Motorola MPC8260 Baud Rate Generators header file */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,12sep99,ms_  created from m8260Cpm.h, 01d.
*/

/*
 * This file contains constants for the Baud Rate Generators (BRGs)
 * in the Motorola MPC8260 PowerQUICC II integrated Communications
 * Processor
 */

#ifndef __INCm8260Brgh
#define __INCm8260Brgh

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

#ifndef M8260_32_WR
#define M8260_32_WR(addr, value) (* ((UINT32 *)(addr)) = ((UINT32) (value)))
#endif

#ifndef M8260_16_WR
#define M8260_16_WR(addr, value) (* ((UINT16 *)(addr)) = ((UINT16) (value)))
#endif

#ifndef M8260_8_WR
#define M8260_8_WR(addr, value) (* ((UINT8 *)(addr)) = ((UINT8) (value)))
#endif

#ifndef M8260_32_RD
#define M8260_32_RD(addr, value) ((value) = (* (UINT32 *) ((UINT32 *)(addr))))
#endif

#ifndef M8260_16_RD
#define M8260_16_RD(addr, value) ((value) = (* (UINT16 *) ((UINT16 *)(addr))))
#endif

#ifndef M8260_8_RD
#define M8260_8_RD(addr, value) ((value) = (* (UINT8 *) ((UINT8 *)(addr))))
#endif

#ifndef M8260_NTH_REG_WR_32
#define M8260_NTH_REG_WR_32(regVal, regBase, offsetNext, n)	\
    {								\
    VINT32 *pReg = (VINT32 *) 					\
	    (immrVal + 						\
	    regBase +        				        \
	    ((n-1) * offsetNext));		                \
    M8260_32_WR(pReg, regVal);				        \
    }
#endif

#define M8260_BRGC_WR(value, brgc)                              \
   M8260_NTH_REG_WR_32(value,                                   \
		       M8260_BRGC_BASE,                         \
		       M8260_BRGC_OFFSET_NEXT_BRGC,             \
		       brgc)

/*
 * MPC8260 internal register/memory map (section 17 of prelim. spec)
 * note that these are offsets from the value stored in the IMMR
 * register. Also note that in the MPC8260, the IMMR is not a special
 * purpose register, but it is memory mapped.
 */
 
/* Baud Rate Generators */

#define M8260_BRGC_BASE  		0x000119F0
#define M8260_BRGC_OFFSET_NEXT_BRGC 	0x00000004

#define M8260_BRGC_RD(regVal, brg)                         	\
    {                                                           \
    VINT32 *pReg = (VINT32 *)                                   \
            (immrVal +                                          \
            M8260_BRGC_BASE +                                   \
            ((brg - 1) * M8260_BRGC_OFFSET_NEXT_BRGC));         \
    M8260_32_RD(pReg, regVal);                                  \
    }

#define M8260_BRGC_SET_BITS(bitmap)				\
    {								\
    UINT32 regVal;						\
    VINT32 *pReg = (VINT32 *) 					\
	    (immrVal + 						\
	    M8260_BRGC_BASE +        				\
	    (scc * M8260_BRGC_OFFSET_NEXT_BRGC));		\
    M8260_32_RD(pReg, regVal);				        \
    M8260_32_WR(pReg, (regVal | bitmap));			\
    }

/* macros used in the configuration registers */

#define M8260_BRGC_CD_MASK      0x00001FFE /* clock divider mask */
#define M8260_BRGC_CD_SHIFT     0x1        /* to shift CD into position */
#define M8260_BRGC_RST          0x00020000 /* Reset BRG */
#define M8260_BRGC_EN           0x00010000 /* Enable BRG count */
#define M8260_BRGC_EXTC_BRGCLK  0x0        /* clock comes from BRGCKL */
#define M8260_BRGC_EXTC_CLK3_9  0x1        /* clock comes from pin 3 or 9 */
#define M8260_BRGC_EXTC_CLK5_15 0x2        /* clock comes from pin 5 or 15 */
#define M8260_BRGC_EXTC_MASK    0x0000C000 /* External Clock Source Mask */
#define M8260_BRGC_EXTC_SHIFT   0xE        /* shift EXTC 14 bits to position */
#define M8260_BRGC_ATB          0x00002000 /* 1 = Autobaud on Rx */
					   /* 0 = normal operation */
					   #define M8260_BRGC_DIV16        0x00000001 /* BRG Clock divide by 16 */

#ifdef __cplusplus
}
#endif

#endif /* __INCm8260Brgh */
