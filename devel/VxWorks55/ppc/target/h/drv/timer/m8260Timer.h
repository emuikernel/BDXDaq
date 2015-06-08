/* m8260Timer.h */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,15jul99,ms_  make compliant with our coding standards
01b,21may99,ms_  local
01a,17apr99,ms_  adapted from ppc860Timer.h ref 01c
*/

/*
DESCRIPTION
This file contains header information for PowerPC timer routines.
*/

#ifndef __INCm8260Timerh
#define __INCm8260Timerh

#ifdef __cplusplus
extern "C" {
#endif

/* CPM Timer Register sets */

/* Timer 1 and Timer 2 Global Configuration Register */

#define M8260_TGCR1(base)   (CAST(VUINT8 *)((base) + 0x10D80))

/* Timer 3 and Timer 4 Global Configuration Register */

#define M8260_TGCR2(base)   (CAST(VUINT8 *)((base) + 0x10D84))

/* CPM Timer Mode Registers */

#define M8260_TMR1(base)   (CAST(VUINT16 *)((base) + 0x10D90)) /* Timer 1 */
#define M8260_TMR2(base)   (CAST(VUINT16 *)((base) + 0x10D92)) /* Timer 2 */
#define M8260_TMR3(base)   (CAST(VUINT16 *)((base) + 0x10DA0)) /* Timer 3 */
#define M8260_TMR4(base)   (CAST(VUINT16 *)((base) + 0x10DA2)) /* Timer 4 */

/* CPM Timer Reference Registers */

#define M8260_TRR1(base)   (CAST(VUINT16 *)((base) + 0x10D94)) /* Timer 1 */
#define M8260_TRR2(base)   (CAST(VUINT16 *)((base) + 0x10D96)) /* Timer 2 */
#define M8260_TRR3(base)   (CAST(VUINT16 *)((base) + 0x10DA4)) /* Timer 3 */
#define M8260_TRR4(base)   (CAST(VUINT16 *)((base) + 0x10DA6)) /* Timer 4 */

/* CPM Timer Capture Registers */

#define M8260_TCR1(base)   (CAST(VUINT16 *)((base) + 0x10D98)) /* Timer 1 */
#define M8260_TCR2(base)   (CAST(VUINT16 *)((base) + 0x10D9A)) /* Timer 2 */
#define M8260_TCR3(base)   (CAST(VUINT16 *)((base) + 0x10DA8)) /* Timer 3 */
#define M8260_TCR4(base)   (CAST(VUINT16 *)((base) + 0x10DAA)) /* Timer 4 */

/* CPM Timer Counters */

#define M8260_TCN1(base)   (CAST(VUINT16 *)((base) + 0x10D9C)) /* Timer 1 */
#define M8260_TCN2(base)   (CAST(VUINT16 *)((base) + 0x10D9E)) /* Timer 2 */
#define M8260_TCN3(base)   (CAST(VUINT16 *)((base) + 0x10DAC)) /* Timer 3 */
#define M8260_TCN4(base)   (CAST(VUINT16 *)((base) + 0x10DAE)) /* Timer 4 */

/* CPM Timer Event Registers */

#define M8260_TER1(base)   (CAST(VUINT16 *)((base) + 0x10DB0)) /* Timer 1 */
#define M8260_TER2(base)   (CAST(VUINT16 *)((base) + 0x10DB2)) /* Timer 2 */
#define M8260_TER3(base)   (CAST(VUINT16 *)((base) + 0x10DB4)) /* Timer 3 */
#define M8260_TER4(base)   (CAST(VUINT16 *)((base) + 0x10DB6)) /* Timer 4 */

/* CPM Timer Bit Definitions */

/* Timer Global Configuration Register Bit Definitions */

#define M8260_TGCR_CAS2		0x80	/* Cascade Timers 1 and 2*/
#define M8260_TGCR_STP2		0x20	/* Stop timer 2 */
#define M8260_TGCR_RST2		0x10	/* Reset timer 2 */
#define M8260_TGCR_GM1		0x08	/* Gate Mode for TGATE1 */
#define M8260_TGCR_STP1		0x02	/* Stop timer 1 */
#define M8260_TGCR_RST1		0x01	/* Reset timer 1 */

#define M8260_TGCR_CAS4		0x80	/* Cascade Timers 3 and 4*/
#define M8260_TGCR_STP4		0x20	/* Stop timer 4 */
#define M8260_TGCR_RST4		0x10	/* Reset timer 4 */
#define M8260_TGCR_GM2		0x08	/* Gate Mode for TGATE2 */
#define M8260_TGCR_STP3		0x02	/* Stop timer 3 */
#define M8260_TGCR_RST3		0x01	/* Reset timer 3 */


/* Timer Mode Register Bit Definitions */

#define M8260_TMR_PS_MSK	0xff00	/* Mask for Prescaler Value */
#define M8260_TMR_CE_MSK	0x00c0	/* Capture Edge and Enable Interrupt */
#define M8260_TMR_CE_INTR_DIS	0x0000	/* Disable Interrupt on capture event */
#define M8260_TMR_CE_RISING	0x0040	/* Capture on Rising TINx edge only */
#define M8260_TMR_CE_FALLING	0x0080	/* Capture on Falling TINx edge only */
#define M8260_TMR_CE_ANY	0x00c0	/* Capture on any TINx edge */
#define M8260_TMR_OM		0x0020	/* Output Mode */
#define M8260_TMR_ORI		0x0010	/* Output Reference Interrupt Enable */
#define M8260_TMR_FRR		0x0008	/* Free Run/Restart */
#define M8260_TMR_ICLK_MSK	0x0006	/* Timer Input Clock Source mask */
#define M8260_TMR_ICLK_IN_CAS	0x0000	/* Internally cascaded input */
#define M8260_TMR_ICLK_IN_GEN	0x0002	/* Internal General system clock */
/* Internal General system clk div 16 */
#define M8260_TMR_ICLK_IN_GEN_DIV16	0x0004	
#define M8260_TMR_ICLK_TIN_PIN	0x0006	/* TINx pin */
#define M8260_TMR_GE		0x0001	/* Gate Enable */

/* Timer Event Register Bit Definitions */

#define M8260_TER_REF		0x0002	/* Output Reference Event */
#define M8260_TER_CAP		0x0001	/* Capture Event */

/* Other Timer Defines */

#define M8260_TRR_SIZE		16	/* number bits in Reference Register */
#define M8260_TMR_PS_MIN	0x00	/* minimum prescale value */
#define M8260_TMR_PS_MAX	0xff00	/* maximum prescale value */

#ifdef __cplusplus
}
#endif

#endif /* __INCm8260Timerh */
