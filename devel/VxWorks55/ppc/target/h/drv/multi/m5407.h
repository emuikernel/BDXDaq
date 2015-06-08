/* m5407.h - Motorola MCF5407 CPU control registers */

/* Copyright 1994-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,27sep00,dh   written (based on m5307.h)
*/

/*
This file contains I/O addresses and related constants for the MCF5407
*/

#ifndef __INCm5407h
#define __INCm5407h

#ifdef __cplusplus
extern "C" {
#endif

/*
Need to use a distinct cast macro in order to not conflict with other
include files.
*/

#ifdef	_ASMLANGUAGE
#define	CAST_M5407(x)		
#else	/* _ASMLANGUAGE */
#define	CAST_M5407(x)		(x)
#endif	/* _ASMLANGUAGE */

/* Size of internal SRAM */

#define M5407_SRAM_SIZE		4096		/* size of SRAM */
#define M5407_SRAM_BANK_SIZE	2048		/* size of SRAM bank */
#define M5407_DCACHE_SIZE	8192		/* size of data cache */
#define M5407_ICACHE_SIZE	16384		/* size of instruction cache */

/* ACRs (access control registers) */

#define M5407_ACR_ADDR_BASE(n)	((n) & 0xff000000)	/* region base */
#define M5407_ACR_ADDR_MASK(n)	(((n)>>8) & 0x00ff0000)	/* region mask */
#define M5407_ACR_ENABLE	(1 << 15)		/* enable ACR */
#define M5407_ACR_MODE_USER	(0 << 13)	/* match if user */
#define M5407_ACR_MODE_SUPER	(1 << 13)	/* match if supervisor */
#define M5407_ACR_MODE_ANY	(2 << 13)	/* match any */
#define M5407_ACR_WRITETHROUGH	(0 << 5)	/* cacheable, writethrough */
#define M5407_ACR_COPYBACK	(1 << 5)	/* cacheable, copyback */
#define M5407_ACR_INH_PRECISE	(2 << 5)	/* cache-inhibited, precise */
#define M5407_ACR_INH_IMPRECISE	(3 << 5)	/* cache-inhibited, imprecise*/
#define M5407_ACR_WP		(1 << 2)	/* write-protect */

/* System Integration Module register addresses */

#define M5407_SIM_RSR(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x000))
#define M5407_SIM_SYPCR(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x001))
#define M5407_SIM_SWIVR(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x002))
#define M5407_SIM_SWSR(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x003))

#define M5407_SIM_PAR(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x004))
#define M5407_SIM_IRQPAR(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x006))
#define M5407_SIM_PLLCR(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x008))
#define M5407_SIM_MPARK(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x00c))

#define M5407_SIM_IPR(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x040))
#define M5407_SIM_IMR(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x044))

/* Autovector control */
#define MCF5407_SIM_AVCR_BLK	(1 << 0)
#define MCF5407_SIM_AVCR_AVEC1	(1 << 1)
#define MCF5407_SIM_AVCR_AVEC2	(1 << 2)
#define MCF5407_SIM_AVCR_AVEC3	(1 << 3)
#define MCF5407_SIM_AVCR_AVEC4	(1 << 4)
#define MCF5407_SIM_AVCR_AVEC5	(1 << 5)
#define MCF5407_SIM_AVCR_AVEC6	(1 << 6)
#define MCF5407_SIM_AVCR_AVEC7	(1 << 7)

#define M5407_SIM_AVCR(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x04b))

#define M5407_SIM_ICR_SWT(base)		\
	(CAST_M5407(volatile UINT8  *)((base) + 0x04c))
#define M5407_SIM_ICR_TIMER1(base)	\
	(CAST_M5407(volatile UINT8  *)((base) + 0x04d))
#define M5407_SIM_ICR_TIMER2(base)	\
	(CAST_M5407(volatile UINT8  *)((base) + 0x04e))
#define M5407_SIM_ICR_MBUS(base)	\
	(CAST_M5407(volatile UINT8  *)((base) + 0x04f))
#define M5407_SIM_ICR_UART1(base)	\
	(CAST_M5407(volatile UINT8  *)((base) + 0x050))
#define M5407_SIM_ICR_UART2(base)	\
	(CAST_M5407(volatile UINT8  *)((base) + 0x051))
#define M5407_SIM_ICR_DMA0(base)	\
	(CAST_M5407(volatile UINT8  *)((base) + 0x052))
#define M5407_SIM_ICR_DMA1(base)	\
	(CAST_M5407(volatile UINT8  *)((base) + 0x053))
#define M5407_SIM_ICR_DMA2(base)	\
	(CAST_M5407(volatile UINT8  *)((base) + 0x054))
#define M5407_SIM_ICR_DMA3(base)	\
	(CAST_M5407(volatile UINT8  *)((base) + 0x055))
#define M5407_SIM_ICR_ICR10(base)		\
	(CAST_M5407(volatile UINT8  *)((base) + 0x056))
#define M5407_SIM_ICR_ICR11(base)		\
	(CAST_M5407(volatile UINT8  *)((base) + 0x057))

/* Chip select registers */

#define M5407_SIM_CSAR0(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x080))
#define M5407_SIM_CSMR0(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x084))
#define M5407_SIM_CSCR0(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x08a))

#define M5407_SIM_CSAR1(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x08c))
#define M5407_SIM_CSMR1(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x090))
#define M5407_SIM_CSCR1(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x096))

#define M5407_SIM_CSAR2(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x098))
#define M5407_SIM_CSMR2(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x09c))
#define M5407_SIM_CSCR2(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x0a2))

#define M5407_SIM_CSAR3(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x0a4))
#define M5407_SIM_CSMR3(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x0a8))
#define M5407_SIM_CSCR3(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x0ae))

#define M5407_SIM_CSAR4(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x0b0))
#define M5407_SIM_CSMR4(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x0b4))
#define M5407_SIM_CSCR4(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x0ba))

#define M5407_SIM_CSAR5(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x0bc))
#define M5407_SIM_CSMR5(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x0c0))
#define M5407_SIM_CSCR5(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x0c6))

#define M5407_SIM_CSAR6(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x0c8))
#define M5407_SIM_CSMR6(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x0cc))
#define M5407_SIM_CSCR6(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x0d2))

#define M5407_SIM_CSAR7(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x0d4))
#define M5407_SIM_CSMR7(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x0d8))
#define M5407_SIM_CSCR7(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x0de))


#define M5407_SIM_DCR(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x100))
#define M5407_SIM_DACR0(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x108))
#define M5407_SIM_DMR0(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x10c))
#define M5407_SIM_DACR1(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x110))
#define M5407_SIM_DMR1(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x114))
#define M5407_SIM_TMR0(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x140))
#define M5407_SIM_TRR0(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x144))
#define M5407_SIM_TCR0(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x148))
#define M5407_SIM_TCN0(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x14c))
#define M5407_SIM_TER0(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x151))
#define M5407_SIM_TMR1(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x180))
#define M5407_SIM_TRR1(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x184))
#define M5407_SIM_TCR1(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x188))
#define M5407_SIM_TCN1(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x18c))
#define M5407_SIM_TER1(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x191))

#define M5407_UART_MR(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1c0 + ((uart) ? 0x40 : 0x00)))
#define M5407_UART_SR(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1c4 + ((uart) ? 0x40 : 0x00)))
#define M5407_UART_CSR(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1c4 + ((uart) ? 0x40 : 0x00)))
#define M5407_UART_CR(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1c8 + ((uart) ? 0x40 : 0x00)))
#define M5407_UART_RB(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1cc + ((uart) ? 0x40 : 0x00)))
#define M5407_UART_TB(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1cc + ((uart) ? 0x40 : 0x00)))
#define M5407_UART_IPCR(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1d0 + ((uart) ? 0x40 : 0x00)))
#define M5407_UART_ACR(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1d0 + ((uart) ? 0x40 : 0x00)))
#define M5407_UART_ISR(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1d4 + ((uart) ? 0x40 : 0x00)))
#define M5407_UART_IMR(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1d4 + ((uart) ? 0x40 : 0x00)))
#define M5407_UART_BG1(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1d8 + ((uart) ? 0x40 : 0x00)))
#define M5407_UART_BG2(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1dc + ((uart) ? 0x40 : 0x00)))
#define M5407_UART_IVR(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1f0 + ((uart) ? 0x40 : 0x00)))
#define M5407_UART_IP(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1f4 + ((uart) ? 0x40 : 0x00)))
#define M5407_UART_OP2(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1f8 + ((uart) ? 0x40 : 0x00)))
#define M5407_UART_OP1(base, uart)	\
	(CAST_M5407(volatile UINT8 *)((base) + 0x1fc + ((uart) ? 0x40 : 0x00)))

#define M5407_SIM_PADDR(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x244))
#define M5407_SIM_PADAT(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x248))
#define M5407_SIM_MADR(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x280))
#define M5407_SIM_MFDR(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x284))
#define M5407_SIM_MBCR(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x288))
#define M5407_SIM_MBSR(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x28c))
#define M5407_SIM_MBDR(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x290))
#define M5407_SIM_SAR0(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x300))
#define M5407_SIM_DAR0(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x304))
#define M5407_SIM_DCR0(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x308))
#define M5407_SIM_BCR0(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x30c))
#define M5407_SIM_DSR0(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x310))
#define M5407_SIM_DIVR0(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x314))
#define M5407_SIM_SAR1(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x340))
#define M5407_SIM_DAR1(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x344))
#define M5407_SIM_DCR1(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x348))
#define M5407_SIM_BCR1(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x34c))
#define M5407_SIM_DSR1(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x350))
#define M5407_SIM_DIVR1(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x354))
#define M5407_SIM_SAR2(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x380))
#define M5407_SIM_DAR2(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x384))
#define M5407_SIM_DCR2(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x388))
#define M5407_SIM_BCR2(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x38c))
#define M5407_SIM_DSR2(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x390))
#define M5407_SIM_DIVR2(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x394))
#define M5407_SIM_SAR3(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x3c0))
#define M5407_SIM_DAR3(base)	(CAST_M5407(volatile UINT32 *)((base) + 0x3c4))
#define M5407_SIM_DCR3(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x3c8))
#define M5407_SIM_BCR3(base)	(CAST_M5407(volatile UINT16 *)((base) + 0x3cc))
#define M5407_SIM_DSR3(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x3d0))
#define M5407_SIM_DIVR3(base)	(CAST_M5407(volatile UINT8  *)((base) + 0x3d4))

/* SIM Module Configuration Register definitions */

/* ICR - encode bits for ICR_xx registers */
#define M5407_SIM_ICR_BITS(auto, level, priority)	\
			  ((((auto) & 0x01) << 7)	\
			   | (((level) & 0x07) << 2)	\
			   | ((priority) & 0x03))

/* IMR - Interrupt Mask Register bit definitions */
#define M5407_IMR_EINT1		(1 << 1)
#define M5407_IMR_EINT2		(1 << 2)
#define M5407_IMR_EINT3		(1 << 3)
#define M5407_IMR_EINT4		(1 << 4)
#define M5407_IMR_EINT5		(1 << 5)
#define M5407_IMR_EINT6		(1 << 6)
#define M5407_IMR_EINT7		(1 << 7)
#define M5407_IMR_SWT		(1 << 8)
#define M5407_IMR_TIMER1	(1 << 9)
#define M5407_IMR_TIMER2	(1 << 10)
#define M5407_IMR_MBUS		(1 << 11)
#define M5407_IMR_UART1		(1 << 12)
#define M5407_IMR_UART2		(1 << 13)
#define M5407_IMR_DMA0		(1 << 14)
#define M5407_IMR_DMA1		(1 << 15)
#define M5407_IMR_DMA2		(1 << 16)
#define M5407_IMR_DMA3		(1 << 17)

/* chip select mask registers */
#define M5407_CS_CSMR_MASK_64k	0x00000000
#define M5407_CS_CSMR_MASK_128k	0x00010000
#define M5407_CS_CSMR_MASK_256k	0x00030000
#define M5407_CS_CSMR_MASK_512k	0x00070000
#define M5407_CS_CSMR_MASK_1M	0x000f0000
#define M5407_CS_CSMR_MASK_2M	0x001f0000
#define M5407_CS_CSMR_MASK_4M	0x003f0000
#define M5407_CS_CSMR_MASK_8M	0x007f0000
#define M5407_CS_CSMR_MASK_16M	0x00ff0000
#define M5407_CS_CSMR_MASK_32M	0x01ff0000
#define M5407_CS_CSMR_MASK_64M	0x03ff0000
#define M5407_CS_CSMR_MASK_128M	0x07ff0000
#define M5407_CS_CSMR_MASK_256M	0x0fff0000
#define M5407_CS_CSMR_MASK_512M	0x1fff0000
#define M5407_CS_CSMR_MASK_1G	0x3fff0000
#define M5407_CS_CSMR_MASK_2G	0x7fff0000
#define M5407_CS_CSMR_MASK_4G	0xffff0000
#define M5407_CS_CSMR_WP	(1 << 8)	/* write protect */
#define M5407_CS_CSMR_AM	(1 << 6)	/* alternate master */
#define M5407_CS_CSMR_CI	(1 << 5)	/* mask CPU/IACK */
#define M5407_CS_CSMR_SC	(1 << 4)	/* mask supervisor code */
#define M5407_CS_CSMR_SD	(1 << 3)	/* mask supervisor data */
#define M5407_CS_CSMR_UC	(1 << 2)	/* mask user code */
#define M5407_CS_CSMR_UD	(1 << 1)	/* mask user data */
#define M5407_CS_CSMR_V		(1 << 0)	/* valid */

/* chip select control regs (CSCR) & default mem control reg (DMCR) */
#define M5407_CS_CSCR_WS(n)	((n) << 10)		/* wait-states */
#define M5407_CS_CSCR_WS_15	M5407_CS_CSCR_WS(15)
#define M5407_CS_CSCR_WS_14	M5407_CS_CSCR_WS(14)
#define M5407_CS_CSCR_WS_13	M5407_CS_CSCR_WS(13)
#define M5407_CS_CSCR_WS_12	M5407_CS_CSCR_WS(12)
#define M5407_CS_CSCR_WS_11	M5407_CS_CSCR_WS(11)
#define M5407_CS_CSCR_WS_10	M5407_CS_CSCR_WS(10)
#define M5407_CS_CSCR_WS_9	M5407_CS_CSCR_WS(9)
#define M5407_CS_CSCR_WS_8	M5407_CS_CSCR_WS(8)
#define M5407_CS_CSCR_WS_7	M5407_CS_CSCR_WS(7)
#define M5407_CS_CSCR_WS_6	M5407_CS_CSCR_WS(6)
#define M5407_CS_CSCR_WS_5	M5407_CS_CSCR_WS(5)
#define M5407_CS_CSCR_WS_4	M5407_CS_CSCR_WS(4)
#define M5407_CS_CSCR_WS_3	M5407_CS_CSCR_WS(3)
#define M5407_CS_CSCR_WS_2	M5407_CS_CSCR_WS(2)
#define M5407_CS_CSCR_WS_1	M5407_CS_CSCR_WS(1)
#define M5407_CS_CSCR_WS_0	M5407_CS_CSCR_WS(0)
#define M5407_CS_CSCR_AA	(1 << 8)	/* auto-acknowledge */
#define M5407_CS_CSCR_PS_32	(0 << 6)	/* 32-bit port size */
#define M5407_CS_CSCR_PS_8	(1 << 6)	/* 8-bit port size */
#define M5407_CS_CSCR_PS_16	(2 << 6)	/* 16-bit port size */
#define M5407_CS_CSCR_BEM	(1 << 5)	/* byte enable module */
#define M5407_CS_CSCR_BSTR	(1 << 4)	/* burst read enable */
#define M5407_CS_CSCR_BSTW	(1 << 3)	/* burst write enable */

/* DRAM controller */

/* DRAM Control Register (DCR) */
#define M5407_DCR_SO		(1 << 15)	/* synchronous operation */
#define M5407_DCR_NAM		(1 << 13)	/* no address multiplexing */
/* sync only bits */
#define M5407_DCR_COC		(1 << 12)	/* command on clock enable */
#define M5407_DCR_IS		(1 << 11)	/* initiate self-refresh */
#define M5407_DCR_RTIM_3CLKS	(0 << 9)	/* 3 clks REFRESH to ACTV */
#define M5407_DCR_RTIM_6CLKS	(1 << 9)	/* 6 clks REFRESH to ACTV */
#define M5407_DCR_RTIM_9CLKS	(2 << 9)	/* 9 clks REFRESH to ACTV */
/* async only bits */
#define M5407_DCR_RRA_2CLKS	(0 << 11)	/* RAS asserted 2 clocks */
#define M5407_DCR_RRA_3CLKS	(1 << 11)	/* RAS asserted 3 clocks */
#define M5407_DCR_RRA_4CLKS	(2 << 11)	/* RAS asserted 4 clocks */
#define M5407_DCR_RRA_5CLKS	(3 << 11)	/* RAS asserted 5 clocks */
#define M5407_DCR_RRP_1CLKS	(0 << 9)	/* RAS precharged 1 clocks */
#define M5407_DCR_RRP_2CLKS	(1 << 9)	/* RAS precharged 2 clocks */
#define M5407_DCR_RRP_3CLKS	(2 << 9)	/* RAS precharged 3 clocks */
#define M5407_DCR_RRP_4CLKS	(3 << 9)	/* RAS precharged 4 clocks */
#define M5407_DCR_RC(n)		((n) & 0x01ff)	/* refresh count */

/* DRAM controller address and control registers */
#define M5407_DACR_RE		(1 << 15)	/* refresh enable */
#define M5407_DACR_PS_32	(0 << 4)	/* 32 bit port */
#define M5407_DACR_PS_8		(1 << 4)	/* 8 bit port */
#define M5407_DACR_PS_16	(2 << 4)	/* 16 bit port */
/* sync */
#define M5407_DACR_CASL_1	(0 << 12)	/* CASL 1 (see manual) */ 
#define M5407_DACR_CASL_2	(1 << 12)	/* CASL 2 (see manual) */ 
#define M5407_DACR_CASL_3	(2 << 12)	/* CASL 3 (see manual) */ 
#define M5407_DACR_CBM(n)	((n) << 8)	/* command and bank mux */
#define M5407_DACR_IMRS		(1 << 6)	/* init mode reg set cmd */
#define M5407_DACR_IP		(1 << 3)	/* init precharge all cmd */
#define M5407_DACR_PM		(1 << 2)	/* page mode */
/* async */
#define M5407_DACR_CAS_1CLKS	(0 << 12)	/* CAS asserted 1 clocks */
#define M5407_DACR_CAS_2CLKS	(1 << 12)	/* CAS asserted 2 clocks */
#define M5407_DACR_CAS_3CLKS	(2 << 12)	/* CAS asserted 3 clocks */
#define M5407_DACR_CAS_4CLKS	(3 << 12)	/* CAS asserted 4 clocks */
#define M5407_DACR_RAS_1CLKS	(0 << 10)	/* RAS asserted 1 clocks */
#define M5407_DACR_RAS_2CLKS	(1 << 10)	/* RAS asserted 2 clocks */
#define M5407_DACR_RAS_3CLKS	(2 << 10)	/* RAS asserted 3 clocks */
#define M5407_DACR_RAS_4CLKS	(3 << 10)	/* RAS asserted 4 clocks */
#define M5407_DACR_RNCN		(1 << 9)	/* RAS to CAS negate */
#define M5407_DACR_RCD		(1 << 8)	/* RAS to CAS delay */
#define M5407_DACR_EDO		(1 << 6)	/* extended data out */
#define M5407_DACR_PM_NONE	(0 << 2)	/* no page mode */
#define M5407_DACR_PM_BRST	(1 << 2)	/* page mode on burst only */
#define M5407_DACR_PM_CONT	(3 << 2)	/* continuous page mode */

/* DRAM controller mask registers */
#define M5407_DMR_MASK_256k	0x00000000
#define M5407_DMR_MASK_512k	0x00040000
#define M5407_DMR_MASK_1M	0x000c0000
#define M5407_DMR_MASK_2M	0x001c0000
#define M5407_DMR_MASK_4M	0x003c0000
#define M5407_DMR_MASK_8M	0x007c0000
#define M5407_DMR_MASK_16M	0x00fc0000
#define M5407_DMR_MASK_32M	0x01fc0000
#define M5407_DMR_MASK_64M	0x03fc0000
#define M5407_DMR_MASK_128M	0x07fc0000
#define M5407_DMR_MASK_256M	0x0ffc0000
#define M5407_DMR_MASK_512M	0x1ffc0000
#define M5407_DMR_MASK_1G	0x3ffc0000
#define M5407_DMR_MASK_2G	0x7ffc0000
#define M5407_DMR_MASK_4G	0xfffc0000
#define M5407_DMR_WP		(1 << 8)	/* write protect */
#define M5407_DMR_AM		(1 << 6)	/* alternate master */
#define M5407_DMR_CI		(1 << 5)	/* mask CPU/IACK */
#define M5407_DMR_SC		(1 << 4)	/* mask supervisor code */
#define M5407_DMR_SD		(1 << 3)	/* mask supervisor data */
#define M5407_DMR_UC		(1 << 2)	/* mask user code */
#define M5407_DMR_UD		(1 << 1)	/* mask user data */
#define M5407_DMR_V		(1 << 0)	/* valid */

/* DRAM controller (synchronous operation) */

#ifdef __cplusplus
}
#endif

#endif /* __INCm5407h */

