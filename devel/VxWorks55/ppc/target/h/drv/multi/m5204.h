/* m5204.h - Motorola MCF5204 CPU control registers */

/* Copyright 1994-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,15apr98,mem  minor cleanups.
01a,09dec96,mem  written.
*/

/*
This file contains I/O addresses and related constants for the MCF5204
*/

#ifndef __INCm5204h
#define __INCm5204h

#ifdef __cplusplus
extern "C" {
#endif

/*
Need to use a distinct cast macro in order to not conflict with other
include files.
*/

#ifdef	_ASMLANGUAGE
#define	CAST_M5204(x)		
#else	/* _ASMLANGUAGE */
#define	CAST_M5204(x)		(x)
#endif	/* _ASMLANGUAGE */

/* Size of internal SRAM */

#define M5204_SRAM_SIZE		512		/* size of internal SRAM */

/* System Integration Module register addresses */

#define M5204_SIM_SIMR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x003))
#define M5204_SIM_ICR_E0(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x014))
#define M5204_SIM_ICR_E1(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x015))
#define M5204_SIM_ICR_E2(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x016))
#define M5204_SIM_ICR_E3(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x017))
#define M5204_SIM_ICR_SW(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x01b))
#define M5204_SIM_ICR_T1(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x01c))
#define M5204_SIM_ICR_T2(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x01d))
#define M5204_SIM_ICR_U1(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x01f))
#define M5204_SIM_IMR(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x034))
#define M5204_SIM_IPR(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x038))
#define M5204_SIM_RSR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x040))
#define M5204_SIM_SYPCR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x041))
#define M5204_SIM_SWIVR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x042))
#define M5204_SIM_SWSR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x043))

/* chip select registers */
#define M5204_SIM_CSAR0(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x064))
#define M5204_SIM_CSMR0(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x068))
#define M5204_SIM_CSCR0(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x06c))
#define M5204_SIM_CSAR1(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x070))
#define M5204_SIM_CSMR1(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x074))
#define M5204_SIM_CSCR1(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x078))
#define M5204_SIM_CSAR2(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x07c))
#define M5204_SIM_CSMR2(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x080))
#define M5204_SIM_CSCR2(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x084))
#define M5204_SIM_CSAR3(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x088))
#define M5204_SIM_CSMR3(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x08c))
#define M5204_SIM_CSCR3(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x090))
#define M5204_SIM_CSAR4(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x094))
#define M5204_SIM_CSMR4(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x098))
#define M5204_SIM_CSCR4(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x09c))
#define M5204_SIM_CSAR5(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x0a0))
#define M5204_SIM_CSMR5(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x0a4))
#define M5204_SIM_CSCR5(base)	(CAST_M5204(volatile UINT32 *)((base) + 0x0a8))

/* parallel port registers */
#define M5204_SIM_PAR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x0cb))
#define M5204_SIM_PADDR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x1c5))
#define M5204_SIM_PADAT(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x1c9))

/* Timers 1 and 2. */

#define M5204_TIMER_REG(base, offset, timer)	\
			((base) + (((timer) - 1) * 0x20) + (offset))

#define M5204_TIMER_TMR(base, n)	\
		(CAST_M5204(volatile UINT16 *) M5204_TIMER_REG(base, 0x100, n))
#define M5204_TIMER_TRR(base, n)	\
		(CAST_M5204(volatile UINT16 *) M5204_TIMER_REG(base, 0x104, n))
#define M5204_TIMER_TCR(base, n)	\
		(CAST_M5204(volatile UINT16 *) M5204_TIMER_REG(base, 0x108, n))
#define M5204_TIMER_TCN(base, n)	\
		(CAST_M5204(volatile UINT16 *) M5204_TIMER_REG(base, 0x10c, n))
#define M5204_TIMER_TER(base, n)	\
		(CAST_M5204(volatile UINT8  *) M5204_TIMER_REG(base, 0x111, n))

/* UART */
#define M5204_UART_MR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x140))
#define M5204_UART_SR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x144))
#define M5204_UART_CSR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x144))
#define M5204_UART_CR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x148))
#define M5204_UART_RB(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x14c))
#define M5204_UART_TB(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x14c))
#define M5204_UART_IPCR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x150))
#define M5204_UART_ACR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x150))
#define M5204_UART_ISR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x154))
#define M5204_UART_IMR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x154))
#define M5204_UART_BG1(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x158))
#define M5204_UART_BG2(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x15c))
#define M5204_UART_IVR(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x170))
#define M5204_UART_IP(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x174))
#define M5204_UART_OP1(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x178))
#define M5204_UART_OP2(base)	(CAST_M5204(volatile UINT8  *)((base) + 0x17c))

/* SIM Module Configuration Register definitions */

/* SIM Configuration register (SIMR) */
#define M5204_SIMR_FRZ0		(1 << 6)	/* bus monitor runs */
#define M5204_SIMR_FRZ1		(1 << 7)	/* watchdog timer runs */

/* ICR - encode bits for ICR_xx registers */
#define M5204_SIM_ICR_BITS(auto, level, priority)	\
			  ((((auto) & 0x01) << 7)	\
			   | (((level) & 0x07) << 2)	\
			   | ((priority) & 0x03))

/* IMR - Interrupt Mask Register bit definitions */
#define M5204_IMR_IRQ0		(1 << 1)
#define M5204_IMR_IRQ1		(1 << 2)
#define M5204_IMR_IRQ2		(1 << 3)
#define M5204_IMR_IRQ3		(1 << 4)
#define M5204_IMR_SWT		(1 << 8)
#define M5204_IMR_TIMER1	(1 << 9)
#define M5204_IMR_TIMER2	(1 << 10)
#define M5204_IMR_UART		(1 << 12)

/* chip select mask registers */
#define M5204_CS_CSMR_MASK_512b	0x00000000
#define M5204_CS_CSMR_MASK_1k	0x00000200
#define M5204_CS_CSMR_MASK_2k	0x00000600
#define M5204_CS_CSMR_MASK_4k	0x00000e00
#define M5204_CS_CSMR_MASK_8k	0x00001e00
#define M5204_CS_CSMR_MASK_16k	0x00003e00
#define M5204_CS_CSMR_MASK_32k	0x00007e00
#define M5204_CS_CSMR_MASK_64k	0x0000fe00
#define M5204_CS_CSMR_MASK_128k	0x0001fe00
#define M5204_CS_CSMR_MASK_256k	0x0003fe00
#define M5204_CS_CSMR_MASK_512k	0x0007fe00
#define M5204_CS_CSMR_MASK_1M	0x000ffe00
#define M5204_CS_CSMR_MASK_2M	0x001ffe00
#define M5204_CS_CSMR_MASK_4M	0x003ffe00
#define M5204_CS_CSMR_MASK_8M	0x007ffe00
#define M5204_CS_CSMR_MASK_16M	0x00fffe00
#define M5204_CS_CSMR_MASK_32M	0x01fffe00
#define M5204_CS_CSMR_MASK_64M	0x03fffe00
#define M5204_CS_CSMR_MASK_128M	0x07fffe00
#define M5204_CS_CSMR_MASK_256M	0x0ffffe00
#define M5204_CS_CSMR_MASK_512M	0x1ffffe00
#define M5204_CS_CSMR_MASK_1G	0x3ffffe00
#define M5204_CS_CSMR_MASK_2G	0x7ffffe00
#define M5204_CS_CSMR_MASK_4G	0xfffffe00
#define M5204_CS_CSMR_WP	(1 << 8)
#define M5204_CS_CSMR_CI	(1 << 5)
#define M5204_CS_CSMR_SC	(1 << 4)
#define M5204_CS_CSMR_SD	(1 << 3)
#define M5204_CS_CSMR_UC	(1 << 2)
#define M5204_CS_CSMR_UD	(1 << 1)

/* chip select control registers */
#define M5204_CS_CSCR_WS(n)	((n) << 10)
#define M5204_CS_CSCR_WS_7	M5204_CS_CSCR_WS(7)
#define M5204_CS_CSCR_WS_6	M5204_CS_CSCR_WS(6)
#define M5204_CS_CSCR_WS_5	M5204_CS_CSCR_WS(5)
#define M5204_CS_CSCR_WS_4	M5204_CS_CSCR_WS(4)
#define M5204_CS_CSCR_WS_3	M5204_CS_CSCR_WS(3)
#define M5204_CS_CSCR_WS_2	M5204_CS_CSCR_WS(2)
#define M5204_CS_CSCR_WS_1	M5204_CS_CSCR_WS(1)
#define M5204_CS_CSCR_WS_0	M5204_CS_CSCR_WS(0)
#define M5204_CS_CSCR_BRST	(1 << 9)
#define M5204_CS_CSCR_AA	(1 << 8)
#define M5204_CS_CSCR_PS_16	(0 << 6)
#define M5204_CS_CSCR_PS_8	(1 << 6)
#define M5204_CS_CSCR_BEM	(1 << 5)
#define M5204_CS_CSCR_V		(1 << 0)

#ifdef __cplusplus
}
#endif

#endif /* __INCm5204h */
