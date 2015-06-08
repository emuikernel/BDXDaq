/* mc146818.h - Motorolla ?  146818 RTC (Real Time Clock) */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,13apr01,hdn  added macros for status register A, B, C, D.
01b,09nov93,vin  moved AUX_CLK_RATE_MIN and AUX_CLK_RATE_MAX macro definitions
                 to config.h
01a,17aug93,hdn	 written.
*/

#ifndef	__INCmc146818h
#define	__INCmc146818h

#ifdef __cplusplus
extern "C" {
#endif


#ifndef	_ASMLANGUAGE

typedef struct
    {
    int rate;
    char bits;
    } CLK_RATE;

#endif	/* _ASMLANGUAGE */


/* offset of status register A, B, C, D */

#define MC146818_STATUS_A		0x0a	/* Status Register A */
#define MC146818_STATUS_B		0x0b	/* Status Register B */
#define MC146818_STATUS_C		0x0c	/* Status Register C */
#define MC146818_STATUS_D		0x0d	/* Status Register D */

/* bit values for status register A - RW except UIP */

#define MC146818_UIP			0x80	/* Update In Progress */
#define MC146818_DV_BITS		0x70	/* Divider Bits */
#define MC146818_RS_BITS		0x0f	/* Rate Selection Bits */

/* bit values for status register B - RW */

#define MC146818_SET			0x80	/* Set to 1 (abort update) */
#define MC146818_PIE			0x40	/* Periodic Int Enable */
#define MC146818_AIE			0x20	/* Alarm Int Enable */
#define MC146818_UIE			0x10	/* Update-ended Int Enable */
#define MC146818_SQWE			0x08	/* Square Wave Enable */
#define MC146818_DM			0x04	/* Date Mode */
#define MC146818_24			0x02	/* 1=24-hour 0=12-hour */
#define MC146818_DSE			0x01	/* Daylight Saving Enable */

/* bit values for status register C - R */

#define MC146818_IRQF			0x80	/* Int Request Flag */
#define MC146818_PF			0x40	/* Periodic Int Flag */
#define MC146818_AF			0x20	/* Alarm Int Flag */
#define MC146818_UF			0x10	/* Update-ended Int Flag */

/* bit values for status register D - R */

#define MC146818_VRT			0x80	/* Valid RAM and Time */


#ifdef __cplusplus
}
#endif

#endif	/* __INCmc146818h */
