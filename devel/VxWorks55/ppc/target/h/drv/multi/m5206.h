/* m5206.h - Motorola MCF5206 CPU control registers */

/* Copyright 1994-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,21jan97,kab  created from m5204.h
*/

/*
This file contains I/O addresses and related constants for the MCF5206
*/

#ifndef __INCm5206h
#define __INCm5206h

#ifdef __cplusplus
extern "C" {
#endif

/*
Need to use a distinct cast macro in order to not conflict with other
include files.
*/

#ifdef	_ASMLANGUAGE
#define	CAST_M5206(x)		
#else	/* _ASMLANGUAGE */
#define	CAST_M5206(x)		(x)
#endif	/* _ASMLANGUAGE */

/* Size of internal SRAM */

#define M5206_SRAM_SIZE		512		/* size of internal SRAM */

/* System Integration Module register addresses */

#define M5206_SIM_SIMR(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x003))
/* External IP1-7 */
#define M5206_SIM_ICR1(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x014))
#define M5206_SIM_ICR2(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x015))
#define M5206_SIM_ICR3(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x016))
#define M5206_SIM_ICR4(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x017))
#define M5206_SIM_ICR5(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x018))
#define M5206_SIM_ICR6(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x019))
#define M5206_SIM_ICR7(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x01a))
/* SWT */
#define M5206_SIM_ICR8(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x01b))
/* Timer 1 INT */
#define M5206_SIM_ICR9(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x01c))
/* Timer 2 INT */
#define M5206_SIM_ICR10(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x01d))
/* MBUS interrupt */
#define M5206_SIM_ICR11(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x01e))
/* UART 1 INT */
#define M5206_SIM_ICR12(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x01f))
/* UART 2 INT */
#define M5206_SIM_ICR13(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x020))
#define M5206_SIM_IMR(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x036))
#define M5206_SIM_IPR(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x03a))
#define M5206_SIM_RSR(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x040))
#define M5206_SIM_SYPCR(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x041))
#define M5206_SIM_SWIVR(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x042))
#define M5206_SIM_SWSR(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x043))

/* DRAM */
#define M5206_DCRR(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x046))
#define M5206_DCTR(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x04a))
#define M5206_DCAR0(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x04c))
#define M5206_DCMR0(base)	(CAST_M5206(volatile UINT32 *)((base) + 0x050))
#define M5206_DCCR0(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x057))
#define M5206_DCAR1(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x058))
#define M5206_DCMR1(base)	(CAST_M5206(volatile UINT32 *)((base) + 0x05c))
#define M5206_DCCR1(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x063))

#define M5206_SIM_CSAR0(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x064))
#define M5206_SIM_CSMR0(base)	(CAST_M5206(volatile UINT32 *)((base) + 0x068))
#define M5206_SIM_CSCR0(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x06e))
#define M5206_SIM_CSAR1(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x070))
#define M5206_SIM_CSMR1(base)	(CAST_M5206(volatile UINT32 *)((base) + 0x074))
#define M5206_SIM_CSCR1(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x07a))
#define M5206_SIM_CSAR2(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x07c))
#define M5206_SIM_CSMR2(base)	(CAST_M5206(volatile UINT32 *)((base) + 0x080))
#define M5206_SIM_CSCR2(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x086))
#define M5206_SIM_CSAR3(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x088))
#define M5206_SIM_CSMR3(base)	(CAST_M5206(volatile UINT32 *)((base) + 0x08c))
#define M5206_SIM_CSCR3(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x092))
#define M5206_SIM_CSAR4(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x094))
#define M5206_SIM_CSMR4(base)	(CAST_M5206(volatile UINT32 *)((base) + 0x098))
#define M5206_SIM_CSCR4(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x09e))
#define M5206_SIM_CSAR5(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x0a0))
#define M5206_SIM_CSMR5(base)	(CAST_M5206(volatile UINT32 *)((base) + 0x0a4))
#define M5206_SIM_CSCR5(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x0aa))
#define M5206_SIM_CSAR6(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x0ac))
#define M5206_SIM_CSMR6(base)	(CAST_M5206(volatile UINT32 *)((base) + 0x0b0))
#define M5206_SIM_CSCR6(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x0b6))
#define M5206_SIM_CSAR7(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x0b8))
#define M5206_SIM_CSMR7(base)	(CAST_M5206(volatile UINT32 *)((base) + 0x0bc))
#define M5206_SIM_CSCR7(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x0c2))
#define M5206_SIM_DMCR(base)	(CAST_M5206(volatile UINT16 *)((base) + 0x0c6))
#define M5206_SIM_PAR(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x0cb))
#define M5206_SIM_PADDR(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x1c5))
#define M5206_SIM_PADAT(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x1c9))

/* Timers 1 and 2. */

#define M5206_TIMER_REG(base, offset, timer)	\
			((base) + (((timer) - 1) * 0x20) + (offset))

#define M5206_TIMER_TMR(base, n)	\
		(CAST_M5206(volatile UINT16 *) M5206_TIMER_REG(base, 0x100, n))
#define M5206_TIMER_TRR(base, n)	\
		(CAST_M5206(volatile UINT16 *) M5206_TIMER_REG(base, 0x104, n))
#define M5206_TIMER_TCR(base, n)	\
		(CAST_M5206(volatile UINT16 *) M5206_TIMER_REG(base, 0x108, n))
#define M5206_TIMER_TCN(base, n)	\
		(CAST_M5206(volatile UINT16 *) M5206_TIMER_REG(base, 0x10c, n))
#define M5206_TIMER_TER(base, n)	\
		(CAST_M5206(volatile UINT8  *) M5206_TIMER_REG(base, 0x111, n))

/* UART 1 & 2 */
#define M5206_UART_REG(base, offset, chan)	\
		((base) + (offset) + (((chan) - 1) * 0x40))

#define M5206_UART_MR(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x140, n))
#define M5206_UART_SR(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x144, n))
#define M5206_UART_CSR(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x144, n))
#define M5206_UART_CR(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x148, n))
#define M5206_UART_RB(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x14c, n))
#define M5206_UART_TB(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x14c, n))
#define M5206_UART_IPCR(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x150, n))
#define M5206_UART_ACR(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x150, n))
#define M5206_UART_ISR(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x154, n))
#define M5206_UART_IMR(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x154, n))
#define M5206_UART_BG1(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x158, n))
#define M5206_UART_BG2(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x15c, n))
#define M5206_UART_IVR(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x170, n))
#define M5206_UART_IP(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x174, n))
#define M5206_UART_OP1(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x178, n))
#define M5206_UART_OP2(base, n)	\
		(CAST_M5206(volatile UINT8 *) M5206_UART_REG((base), 0x17c, n))

/* MBUS regs */
#define M5206_MADR(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x1e0))
#define M5206_MFDR(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x1e4))
#define M5206_MBCR(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x1e8))
#define M5206_MBSR(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x1ec))
#define M5206_MBDR(base)	(CAST_M5206(volatile UINT8  *)((base) + 0x1f0))


/* SIM Module Configuration Register definitions */

/* ICR - encode bits for ICR_xx registers */
#define M5206_SIM_ICR_BITS(auto, level, priority)	\
			  ((((auto) & 0x01) << 7)	\
			   | (((level) & 0x07) << 2)	\
			   | ((priority) & 0x03))

/* IMR - Interrupt Mask Register bit definitions */
#define M5206_IMR_IRQ1		(1 << 1)
#define M5206_IMR_IRQ2		(1 << 2)
#define M5206_IMR_IRQ3		(1 << 3)
#define M5206_IMR_IRQ4		(1 << 4)
#define M5206_IMR_IRQ5		(1 << 5)
#define M5206_IMR_IRQ6		(1 << 6)
#define M5206_IMR_IRQ7		(1 << 7)
#define M5206_IMR_SWT		(1 << 8)
#define M5206_IMR_TIMER1	(1 << 9)
#define M5206_IMR_TIMER2	(1 << 10)
#define M5206_IMR_MBUS		(1 << 11)
#define M5206_IMR_UART1		(1 << 12)
#define M5206_IMR_UART2		(1 << 13)

/* chip select mask registers */
#define M5206_CS_CSMR_MASK_64k	0x00000000
#define M5206_CS_CSMR_MASK_128k	0x00010000
#define M5206_CS_CSMR_MASK_256k	0x00030000
#define M5206_CS_CSMR_MASK_512k	0x00070000
#define M5206_CS_CSMR_MASK_1M	0x000f0000
#define M5206_CS_CSMR_MASK_2M	0x001f0000
#define M5206_CS_CSMR_MASK_4M	0x003f0000
#define M5206_CS_CSMR_MASK_8M	0x007f0000
#define M5206_CS_CSMR_MASK_16M	0x00ff0000
#define M5206_CS_CSMR_MASK_32M	0x01ff0000
#define M5206_CS_CSMR_MASK_64M	0x03ff0000
#define M5206_CS_CSMR_MASK_128M	0x07ff0000
#define M5206_CS_CSMR_MASK_256M	0x0fff0000
#define M5206_CS_CSMR_MASK_512M	0x1fff0000
#define M5206_CS_CSMR_MASK_1G	0x3fff0000
#define M5206_CS_CSMR_MASK_2G	0x7fff0000
#define M5206_CS_CSMR_MASK_4G	0xffff0000
#define M5206_CS_CSMR_CI	(1 << 5)		/* /CS1 only */
#define M5206_CS_CSMR_SC	(1 << 4)
#define M5206_CS_CSMR_SD	(1 << 3)
#define M5206_CS_CSMR_UC	(1 << 2)
#define M5206_CS_CSMR_UD	(1 << 1)

/* chip select control regs (CSCR) & default mem control reg (DMCR) */
#define M5206_CS_CSCR_WS(n)	((n) << 10)		/* wait-states */
#define M5206_CS_CSCR_WS_15	M5206_CS_CSCR_WS(15)
#define M5206_CS_CSCR_WS_14	M5206_CS_CSCR_WS(14)
#define M5206_CS_CSCR_WS_13	M5206_CS_CSCR_WS(13)
#define M5206_CS_CSCR_WS_12	M5206_CS_CSCR_WS(12)
#define M5206_CS_CSCR_WS_11	M5206_CS_CSCR_WS(11)
#define M5206_CS_CSCR_WS_10	M5206_CS_CSCR_WS(10)
#define M5206_CS_CSCR_WS_9	M5206_CS_CSCR_WS(9)
#define M5206_CS_CSCR_WS_8	M5206_CS_CSCR_WS(8)
#define M5206_CS_CSCR_WS_7	M5206_CS_CSCR_WS(7)
#define M5206_CS_CSCR_WS_6	M5206_CS_CSCR_WS(6)
#define M5206_CS_CSCR_WS_5	M5206_CS_CSCR_WS(5)
#define M5206_CS_CSCR_WS_4	M5206_CS_CSCR_WS(4)
#define M5206_CS_CSCR_WS_3	M5206_CS_CSCR_WS(3)
#define M5206_CS_CSCR_WS_2	M5206_CS_CSCR_WS(2)
#define M5206_CS_CSCR_WS_1	M5206_CS_CSCR_WS(1)
#define M5206_CS_CSCR_WS_0	M5206_CS_CSCR_WS(0)
#define M5206_CS_CSCR_BRST	(1 << 9)	/* allow burst access */
#define M5206_CS_CSCR_AA	(1 << 8)	/* auto-acknowledge */
#define M5206_CS_CSCR_PS_32	(0 << 6)	/* 32-bit port size */
#define M5206_CS_CSCR_PS_8	(1 << 6)	/* 8-bit port size */
#define M5206_CS_CSCR_PS_16	(2 << 6)	/* 16-bit port size */
#define M5206_CS_CSCR_EMAA	(1 << 5)	/* alternate master auto-ack */
#define M5206_CS_CSCR_ASET	(1 << 4)	/* address setup enable */
#define M5206_CS_CSCR_WRAH	(1 << 3)	/* write address hold enable */
#define M5206_CS_CSCR_RDAH	(1 << 2)	/* read address hold enable */
#define M5206_CS_CSCR_WR	(1 << 1)	/* write enable */
#define M5206_CS_CSCR_RD	(1 << 0)	/* read enable */

/* DRAM Controller Timing Register (DCTR) */
#define M5206_DCTR_DAEM		(1 << 15)	/* drive multiplexed addr */
#define M5206_DCTR_EDO		(1 << 14)	/* EDO enable */
#define M5206_DCTR_RCD		(1 << 12)	/* RAS-to-CAS delay */
#define M5206_DCTR_RSH1		(1 << 10)	/* RAS hold time */
#define M5206_DCTR_RSH0		(1 << 9)	/* RAS hold time */
#define M5206_DCTR_RP1		(1 << 6)	/* RAS precharge time */
#define M5206_DCTR_RP0		(1 << 5)	/* RAS prechange time */
#define M5206_DCTR_CAS		(1 << 3)	/* CAS time */
#define M5206_DCTR_CP		(1 << 1)	/* CAS precharge time */
#define M5206_DCTR_CSR		(1 << 0)	/* CAS setup time */

/* dram controller mask registers */
#define M5206_DCMR_MASK_128k	0x00000000
#define M5206_DCMR_MASK_256k	0x00020000
#define M5206_DCMR_MASK_512k	0x00060000
#define M5206_DCMR_MASK_1M	0x000e0000
#define M5206_DCMR_MASK_2M	0x001e0000
#define M5206_DCMR_MASK_4M	0x003e0000
#define M5206_DCMR_MASK_8M	0x007e0000
#define M5206_DCMR_MASK_16M	0x00fe0000
#define M5206_DCMR_MASK_32M	0x01fe0000
#define M5206_DCMR_MASK_64M	0x03fe0000
#define M5206_DCMR_MASK_128M	0x07fe0000
#define M5206_DCMR_MASK_256M	0x0ffe0000
#define M5206_DCMR_MASK_512M	0x1ffe0000
#define M5206_DCMR_MASK_1G	0x3ffe0000
#define M5206_DCMR_MASK_2G	0x7ffe0000
#define M5206_DCMR_MASK_4G	0xfffe0000
#define M5206_DCMR_SC		(1 << 4)
#define M5206_DCMR_SD		(1 << 3)
#define M5206_DCMR_UC		(1 << 2)
#define M5206_DCMR_UD		(1 << 1)

/* DRAM Controller Control Register */
#define M5206_DCCR_PS_32	(0 << 6)	/* 32-bit port size */
#define M5206_DCCR_PS_8		(1 << 6)	/* 8-bit port size */
#define M5206_DCCR_PS_16	(3 << 6)	/* 16-bit port size */
#define M5206_DCCR_BPS_512B	(0 << 4)	/* 512byte bank page size */
#define M5206_DCCR_BPS_1K	(1 << 4)	/* 1Kb bank page size */
#define M5206_DCCR_BPS_2K	(2 << 4)	/* 2Kb bank page size */
#define M5206_DCCR_MODE_NORMAL	(0 << 2)	/* normal page mode */
#define M5206_DCCR_MODE_BURST	(1 << 2)	/* burst page mode */
#define M5206_DCCR_MODE_FAST	(3 << 2)	/* fast page mode */
#define M5206_DCCR_WR		(1 << 1)	/* Signal on write xfer */
#define M5206_DCCR_RD		(1 << 0)	/* Signal on read xfer */

#ifdef __cplusplus
}
#endif

#endif /* __INCm5206h */
