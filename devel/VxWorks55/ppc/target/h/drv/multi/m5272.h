/* m5272.h - Motorola MCF5272 CPU control registers */

/* Copyright 1994-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,20mar01,hjg  Created from m5206e.h
*/

/*
This file contains I/O addresses and related constants for the MCF5272
*/

#ifndef __INCm5272h
#define __INCm5272h

#ifdef __cplusplus
extern "C" {
#endif

/*
Need to use a distinct cast macro in order to not conflict with other
include files.
*/

#ifdef	_ASMLANGUAGE
#define	CAST_M5272(x)		
#else	/* _ASMLANGUAGE */
#define	CAST_M5272(x)		(x)
#endif	/* _ASMLANGUAGE */

/* Size of internal SRAM */

#define M5272_SRAM_SIZE		4096		/* size of internal SRAM */

/* System Integration Module register addresses */
#define M5272_SIM_MBAR(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x000))
#define M5272_SIM_SCR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x004))
#define M5272_SIM_SPR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x006))
#define M5272_SIM_PMR(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x008))
#define M5272_SIM_ALPR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x00e))
#define M5272_SIM_DIR(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x010))

#define M5272_SIM_MBAR_BA	(0xffff << 16)	/* base address */
#define M5272_SIM_MBAR_SC	(1 << 4)	/* set masks supervisor code */
#define M5272_SIM_MBAR_SD	(1 << 3)	/* set masks supervisor data */
#define M5272_SIM_MBAR_UC	(1 << 2)	/* set masks user code */
#define M5272_SIM_MBAR_UD	(1 << 1)	/* set masks user data */
#define M5272_SIM_MBAR_V	(1 << 0)	/* settings are valid */

#define M5272_SIM_SCR_RSTSRC	(3 << 12)	/* reset source */
#define M5272_SIM_SCR_Priority	(1 << 8)	/* select bus priority scheme */
#define M5272_SIM_SCR_AR	(1 << 7)	/* assume request */
#define M5272_SIM_SCR_SoftRST	(1 << 6)	/* reset on-chip peripherals */
#define M5272_SIM_SCR_BusLock	(1 << 3)	/* lock ownership of the bus */
#define M5272_SIM_SCR_HWR_128	(0x0)		/* hw watchdog   128 clocks */
#define M5272_SIM_SCR_HWR_256	(0x1)		/* hw watchdog   256 clocks */
#define M5272_SIM_SCR_HWR_512	(0x2)		/* hw watchdog   512 clocks */
#define M5272_SIM_SCR_HWR_1024	(0x3)		/* hw watchdog  1024 clocks */
#define M5272_SIM_SCR_HWR_2048	(0x4)		/* hw watchdog  2048 clocks */
#define M5272_SIM_SCR_HWR_4096	(0x5)		/* hw watchdog  4096 clocks */
#define M5272_SIM_SCR_HWR_8192	(0x6)		/* hw watchdog  8192 clocks */
#define M5272_SIM_SCR_HWR_16384	(0x7)		/* hw watchdog 16384 clocks */

#define M5272_SIM_SPR_ADC	(1 << 15)	/* addr decode conflict */
#define M5272_SIM_SPR_WPV	(1 << 14)	/* wr protect violation */
#define M5272_SIM_SPR_SMV	(1 << 13)	/* stopped module viol */
#define M5272_SIM_SPR_PE	(1 << 12)	/* peripheral error */
#define M5272_SIM_SPR_HWT	(1 << 11)	/* hw watchdog timer */
#define M5272_SIM_SPR_RPV	(1 << 10)	/* read protect viol */
#define M5272_SIM_SPR_EXT	(1 << 9)	/* extern. transf. err. */
#define M5272_SIM_SPR_SUV	(1 << 8)	/* superv./user viol */
#define M5272_SIM_SPR_ADCEN	(1 << 7)	/* addr decode conflict, excp */
#define M5272_SIM_SPR_WPVEN	(1 << 6)	/* wr protect violation, excp */
#define M5272_SIM_SPR_SMVEN	(1 << 5)	/* stopped module viol, excp */
#define M5272_SIM_SPR_PEVEN	(1 << 4)	/* peripheral error, excp */
#define M5272_SIM_SPR_HWTEN	(1 << 3)	/* hw watchdog timer, excp */
#define M5272_SIM_SPR_RPVEN	(1 << 2)	/* read protect viol, excp */
#define M5272_SIM_SPR_EXTEN	(1 << 1)	/* extern. transf. err., excp */
#define M5272_SIM_SPR_SUVEN	(1 << 0)	/* superv./user viol, excp */

#define M5272_SIM_PMR_BDMPDN	(1 << 31)	/* debug power-down enable */
#define M5272_SIM_PMR_ENETPDN	(1 << 26)	/* ethernet power-down enable */
#define M5272_SIM_PMR_PLIPDN	(1 << 25)	/* PLIC power-down enable */
#define M5272_SIM_PMR_DRAMPDN	(1 << 24)	/* DRAM contr. pwrdwn enable */
#define M5272_SIM_PMR_DMAPDN	(1 << 23)	/* DMA contr. pwrdwn enable */
#define M5272_SIM_PMR_PWMPDN	(1 << 22)	/* PWM power-down enable */
#define M5272_SIM_PMR_QSPIPDN	(1 << 21)	/* QSPI power-down enable */
#define M5272_SIM_PMR_TIMERPDN	(1 << 20)	/* timer power-down enable */
#define M5272_SIM_PMR_GPIOPDN	(1 << 19)	/* Parallel port pwrdwn enable*/
#define M5272_SIM_PMR_USBPDN	(1 << 18)	/* USB power-down enable */
#define M5272_SIM_PMR_UART1PDN	(1 << 17)	/* UART1 power-down enable */
#define M5272_SIM_PMR_UART0PDN	(1 << 16)	/* UART0 power-down enable */ 
#define M5272_SIM_PMR_USBWK	(1 << 10)	/* USB wakeup enable */ 
#define M5272_SIM_PMR_UART1WK	(1 << 9)	/* UART1 wakeup enable */ 
#define M5272_SIM_PMR_UART0WK	(1 << 8)	/* UART0 wakeup enable */ 
#define M5272_SIM_PMR_MOS	(1 << 5)	/* main oscillator stop */ 
#define M5272_SIM_PMR_SLPEN	(1 << 4)	/* sleep enable */ 

#define M5272_SIM_DIR_VERSION	(0xf << 28)	/* version number */ 
#define M5272_SIM_DIR_DSGNCNTR	(0x3f << 22)	/* design center */ 
#define M5272_SIM_DIR_DEVNR	(0x3ff << 12)	/* device number */ 
#define M5272_SIM_DIR_JEDEC	(0xfff << 1)	/* reduced Motorola-JEDEC-ID */ 

/* Interrupt Controller Registers */
#define M5272_SIM_ICR1(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x020))
#define M5272_SIM_ICR2(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x024))
#define M5272_SIM_ICR3(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x028))
#define M5272_SIM_ICR4(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x02c))
#define M5272_SIM_ISR(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x030))
#define M5272_SIM_PITR(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x034))
#define M5272_SIM_PIWR(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x038))
#define M5272_SIM_PIVR(base)	(CAST_M5272(volatile UINT8  *)((base) + 0x03f))

#define M5272_SIM_PITR_POS_EDGE	(0x00000000)	/* positive edge triggered */
#define M5272_SIM_PITR_NEG_EDGE	(0xf0000060)	/* negative edge triggered */

/* Chip Select Registers */
#define M5272_CS_CSBR0(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x040))
#define M5272_CS_CSOR0(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x044))
#define M5272_CS_CSBR1(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x048))
#define M5272_CS_CSOR1(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x04c))
#define M5272_CS_CSBR2(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x050))
#define M5272_CS_CSOR2(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x054))
#define M5272_CS_CSBR3(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x058))
#define M5272_CS_CSOR3(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x05c))
#define M5272_CS_CSBR4(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x060))
#define M5272_CS_CSOR4(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x064))
#define M5272_CS_CSBR5(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x068))
#define M5272_CS_CSOR5(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x06c))
#define M5272_CS_CSBR6(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x070))
#define M5272_CS_CSOR6(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x074))
#define M5272_CS_CSBR7(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x078))
#define M5272_CS_CSOR7(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x07c))

#define M5272_CS_CSBR_BA	(0xfffff << 12)	/* base address */
#define M5272_CS_CSBR_EBI	(3 << 10)	/* ext. bus interface modes */
#define M5272_CS_CSBR_BW	(3 << 8)	/* bus width */
#define M5272_CS_CSBR_SUPER	(1 << 7)	/* supervisor mode */
#define M5272_CS_CSBR_TT	(3 << 5)	/* transfer type */
#define M5272_CS_CSBR_TM	(7 << 2)	/* transfer modifier */
#define M5272_CS_CSBR_CTM	(1 << 1)	/* compare TM */
#define M5272_CS_CSBR_ENABLE	(1 << 0)	/* enable/disable chip select */

#define M5272_CS_CSBR_BW_LONGW	(0x0 << 8)	/* bus width = 32 bit */
#define M5272_CS_CSBR_BW_BYTE	(0x1 << 8)	/* bus width = 8 bit */
#define M5272_CS_CSBR_BW_WORD	(0x2 << 8)	/* bus width = 16 bit */
#define M5272_CS_CSBR_BW_LINE	(0x3 << 8)	/* bus width = cache line (32)*/
#define M5272_CS_CSBR_EBI_1632	(0x0 << 10)		/* 16/32 bit SRAM/ROM */
#define M5272_CS_CSBR_EBI_SDRAM	(0x1 << 10)		/* SDRAM (CS7 only) */
#define M5272_CS_CSBR_EBI_8	(0x3 << 10)		/* 8 bit SRAM/ROM */

#define M5272_CS_CSOR_BAM	(0xfffff << 12)	/* address mask */
#define M5272_CS_CSOR_ASET	(1 << 11)	/* address setup enable */
#define M5272_CS_CSOR_WRAH	(1 << 10)	/* write address hold enable */
#define M5272_CS_CSOR_RDAH	(1 << 9)	/* read address hold enable */
#define M5272_CS_CSOR_EXTBURST	(1 << 8)	/* enable extended burst */
#define M5272_CS_CSOR_WS(a)	(((a)&0x1f)<<2)	/* wait state generator */
#define M5272_CS_CSOR_RW	(1 << 1)	/* RW and MRW determine if */
#define M5272_CS_CSOR_MRW	(1 << 0)	/* read only or write only  */

#define M5272_CS_CSOR_BAM_32M	(0xfe000000)
#define M5272_CS_CSOR_BAM_16M	(0xff000000)
#define M5272_CS_CSOR_BAM_8M	(0xff800000)
#define M5272_CS_CSOR_BAM_4M	(0xffc00000)
#define M5272_CS_CSOR_BAM_2M	(0xffe00000)
#define M5272_CS_CSOR_BAM_1M	(0xfff00000)
#define M5272_CS_CSOR_BAM_512K	(0xfff80000)
#define M5272_CS_CSOR_BAM_256K	(0xfffc0000)
#define M5272_CS_CSOR_BAM_128K	(0xfffe0000)
#define M5272_CS_CSOR_BAM_64K	(0xffff0000)
#define M5272_CS_CSOR_BAM_32K	(0xffff8000)
#define M5272_CS_CSOR_BAM_16K	(0xffffc000)
#define M5272_CS_CSOR_BAM_8K	(0xffffe000)
#define M5272_CS_CSOR_BAM_4K	(0xfffff000)

/* Software Watchdog Registers */
#define M5272_SIM_WRRR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x282))
#define M5272_SIM_WIRR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x286))
#define M5272_SIM_WCR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x28a))
#define M5272_SIM_WER(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x28e))

#define M5272_SIM_WRRR_REF	(0x7fff << 1)	/* reference value */ 
#define M5272_SIM_WRRR_EN	(1 << 0)	/* enable watchdog */ 

#define M5272_SIM_WIRR_REF	(0x7fff << 1)	/* reference value */ 
#define M5272_SIM_WIRR_IEN	(1 << 0)	/* enable interrupt */ 

#define M5272_SIM_WCR_COUNT	(0xffff << 0)	/* counter value */ 

#define M5272_SIM_WER_WIE	(1 << 0)	/* watchdog interrupt event */ 

/* GPIO Port Register Memory Map */
#define M5272_GPIO_PACNT(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x080))
#define M5272_GPIO_PADDR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x084))
#define M5272_GPIO_PADAT(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x086))
#define M5272_GPIO_PBCNT(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x088))
#define M5272_GPIO_PBDDR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x08c))
#define M5272_GPIO_PBDAT(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x08e))
#define M5272_GPIO_PCDDR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x094))
#define M5272_GPIO_PCDAT(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x096))
#define M5272_GPIO_PDCNT(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x098))

#define M5272_GPIO_PACNT_PACNT15	(3 << 30)	/* configure pin M3 */ 
#define M5272_GPIO_PACNT_PACNT14	(3 << 28)	/* configure pin M2 */ 
#define M5272_GPIO_PACNT_PACNT13	(3 << 26)	/* configure pin L3 */ 
#define M5272_GPIO_PACNT_PACNT12	(3 << 24)	/* configure pin L2 */ 
#define M5272_GPIO_PACNT_PACNT11	(3 << 22)	/* configure pin L1 */ 
#define M5272_GPIO_PACNT_PACNT10	(3 << 20)	/* configure pin K5 */ 
#define M5272_GPIO_PACNT_PACNT9		(3 << 18)	/* configure pin J3 */ 
#define M5272_GPIO_PACNT_PACNT8		(3 << 16)	/* configure pin J2 */ 
#define M5272_GPIO_PACNT_PACNT7		(3 << 14)	/* configure pin P1 */ 
#define M5272_GPIO_PACNT_PACNT6		(3 << 12)	/* configure pin E1 */ 
#define M5272_GPIO_PACNT_PACNT5		(3 << 10)	/* configure pin E2 */ 
#define M5272_GPIO_PACNT_PACNT4		(3 << 8)	/* configure pin E3 */ 
#define M5272_GPIO_PACNT_PACNT3		(3 << 6)	/* configure pin E4 */ 
#define M5272_GPIO_PACNT_PACNT2		(3 << 4)	/* configure pin E5 */ 
#define M5272_GPIO_PACNT_PACNT1		(3 << 2)	/* configure pin D1 */ 
#define M5272_GPIO_PACNT_PACNT0		(3 << 0)	/* configure pin D2 */ 

#define M5272_GPIO_PBCNT_PBCNT15	(3 << 30)	/* configure pin P10 */ 
#define M5272_GPIO_PBCNT_PBCNT14	(3 << 28)	/* configure pin L9 */ 
#define M5272_GPIO_PBCNT_PBCNT13	(3 << 26)	/* configure pin M9 */ 
#define M5272_GPIO_PBCNT_PBCNT12	(3 << 24)	/* configure pin N9 */ 
#define M5272_GPIO_PBCNT_PBCNT11	(3 << 22)	/* configure pin P9 */ 
#define M5272_GPIO_PBCNT_PBCNT10	(3 << 20)	/* configure pin L8 */ 
#define M5272_GPIO_PBCNT_PBCNT9		(3 << 18)	/* configure pin M8 */ 
#define M5272_GPIO_PBCNT_PBCNT8		(3 << 16)	/* configure pin N8 */ 
#define M5272_GPIO_PBCNT_PBCNT7		(3 << 14)	/* configure pin M6 */ 
#define M5272_GPIO_PBCNT_PBCNT6		(3 << 12)	/* configure pin G4 */ 
#define M5272_GPIO_PBCNT_PBCNT5		(3 << 10)	/* configure pin F3 */ 
#define M5272_GPIO_PBCNT_PBCNT4		(3 << 8)	/* configure pin G3 */ 
#define M5272_GPIO_PBCNT_PBCNT3		(3 << 6)	/* configure pin H3 */ 
#define M5272_GPIO_PBCNT_PBCNT2		(3 << 4)	/* configure pin H2 */ 
#define M5272_GPIO_PBCNT_PBCNT1		(3 << 2)	/* configure pin H1 */ 
#define M5272_GPIO_PBCNT_PBCNT0		(3 << 0)	/* configure pin H4 */ 

#define M5272_GPIO_PDCNT_PDCNT7	(3 << 14)	/* configure pin K6 */ 
#define M5272_GPIO_PDCNT_PDCNT6	(3 << 12)	/* configure pin P5 */ 
#define M5272_GPIO_PDCNT_PDCNT5	(3 << 10)	/* configure pin P2 */ 
#define M5272_GPIO_PDCNT_PDCNT4	(3 << 8)	/* configure pin K1 */ 
#define M5272_GPIO_PDCNT_PDCNT3	(3 << 6)	/* configure pin K3 */ 
#define M5272_GPIO_PDCNT_PDCNT2	(3 << 4)	/* configure pin K2 */ 
#define M5272_GPIO_PDCNT_PDCNT1	(3 << 2)	/* configure pin K1 */ 
#define M5272_GPIO_PDCNT_PDCNT0	(3 << 0)	/* configure pin J4 */ 

/* QSPI Module Memory Map */
#define M5272_QSPI_QMR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x0a0))
#define M5272_QSPI_QDLYR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x0a4))
#define M5272_QSPI_QWR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x0a8))
#define M5272_QSPI_QIR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x0ac))
#define M5272_QSPI_QAR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x0b0))
#define M5272_QSPI_QDR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x0b4))

/* PWM Module Memory Map */
#define M5272_PWM_PWCR1(base)	(CAST_M5272(volatile UINT8  *)((base) + 0x0c0))
#define M5272_PWM_PWCR2(base)	(CAST_M5272(volatile UINT8  *)((base) + 0x0c4))
#define M5272_PWM_PWCR3(base)	(CAST_M5272(volatile UINT8  *)((base) + 0x0c8))
#define M5272_PWM_PWWD1(base)	(CAST_M5272(volatile UINT8  *)((base) + 0x0d0))
#define M5272_PWM_PWWD2(base)	(CAST_M5272(volatile UINT8  *)((base) + 0x0d4))
#define M5272_PWM_PWWD3(base)	(CAST_M5272(volatile UINT8  *)((base) + 0x0d8))

/* DMA Module Memory Map */
#define M5272_DMA_DMR(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x0e0))
#define M5272_DMA_DIR(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x0e6))
#define M5272_DMA_DBCR(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x0e8))
#define M5272_DMA_DSAR(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x0ec))
#define M5272_DMA_DDAR(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x0f0))

#define M5272_DMA_DMR_RESET	(1 << 31)	/* reset */ 
#define M5272_DMA_DMR_EN	(1 << 30)	/* enable */ 
#define M5272_DMA_DMR_RQM	(3 << 18)	/* request mode */ 
#define M5272_DMA_DMR_DSTM	(3 << 13)	/* dest. addressing mode */ 
#define M5272_DMA_DMR_DSTT	(7 << 10)	/* dest. addressing type */ 
#define M5272_DMA_DMR_DSTS	(3 << 8)	/* dest. data transfer type */ 
#define M5272_DMA_DMR_SRCM	(1 << 5)	/* source addressing mode */ 
#define M5272_DMA_DMR_SRCT	(7 << 2)	/* source addressing type */ 
#define M5272_DMA_DMR_SRCS	(3 << 0)	/* source data transfer type */ 

#define M5272_DMA_DIR_INVEN	(1 << 12)	/* inval. combin. interrupt */ 
#define M5272_DMA_DIR_ASCEN	(1 << 11)	/* addr. seq. compl. interr. */ 
#define M5272_DMA_DIR_TEEN	(1 << 9)	/* Transfer error interrupt */ 
#define M5272_DMA_DIR_TCEN	(1 << 8)	/* Transfer compl. interrupt */ 
#define M5272_DMA_DIR_INV	(1 << 4)	/* invalid combination */ 
#define M5272_DMA_DIR_ASC	(1 << 3)	/* addr. sequence complete */ 
#define M5272_DMA_DIR_TE	(1 << 1)	/* transfer error */ 
#define M5272_DMA_DIR_TC	(1 << 0)	/* transfer complete */ 

/* UART 0 & 1 */
#define M5272_UART_REG(base, offset, chan)	\
		((base) + (offset) + ((chan) * 0x40))
#define M5272_UART_UMR1(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x100, n))
#define M5272_UART_UMR2(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x100, n))
#define M5272_UART_USR(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x104, n))
#define M5272_UART_UCSR(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x104, n))
#define M5272_UART_UCR(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x108, n))
#define M5272_UART_URB(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x10c, n))
#define M5272_UART_UTB(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x10c, n))
#define M5272_UART_UIPCR(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x110, n))
#define M5272_UART_UACR(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x110, n))
#define M5272_UART_UISR(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x114, n))
#define M5272_UART_UIMR(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x114, n))
#define M5272_UART_UDU(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x118, n))
#define M5272_UART_UDL(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x11c, n))
#define M5272_UART_UABU(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x120, n))
#define M5272_UART_UABL(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x124, n))
#define M5272_UART_UTF(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x128, n))
#define M5272_UART_URF(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x12c, n))
#define M5272_UART_UFPD(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x130, n))
#define M5272_UART_UIP(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x134, n))
#define M5272_UART_UOP1(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x138, n))
#define M5272_UART_UOP0(base, n)	\
		(CAST_M5272(volatile UINT8 *) M5272_UART_REG((base), 0x13c, n))

#define M5272_UART_UMR1_RxRTS	(1 << 7)	/* receiver request-to-send */ 
#define M5272_UART_UMR1_RxIRQ_FFULL	(1 << 6)	/* rec. interr. sel. */ 
#define M5272_UART_UMR1_ERR	(1 << 5)	/* error mode-to-send */ 
#define M5272_UART_UMR1_PM	(3 << 3)	/* parity mode */ 
#define M5272_UART_UMR1_PT	(1 << 2)	/* parity type */ 
#define M5272_UART_UMR1_B_C	(3 << 0)	/* bits per character */ 

#define M5272_UART_UMR2_CM	(3 << 6)	/* channel mode */ 
#define M5272_UART_UMR2_TxRTS	(1 << 5)	/* transmitter ready-to-send */ 
#define M5272_UART_UMR2_TxCTS	(1 << 4)	/* transmitter clear-to-send */ 
#define M5272_UART_UMR2_SB	(0xf << 0)	/* stop-bit length control */ 

#define M5272_UART_USR_RB	(1 << 7)	/* received break */ 
#define M5272_UART_USR_FE	(1 << 6)	/* framing error */ 
#define M5272_UART_USR_PE	(1 << 5)	/* parity error */ 
#define M5272_UART_USR_OE	(1 << 4)	/* overrun error */ 
#define M5272_UART_USR_TxEMP	(1 << 3)	/* transmitter empty */ 
#define M5272_UART_USR_TxRDY	(1 << 2)	/* transmitter ready */ 
#define M5272_UART_USR_FFULL	(1 << 1)	/* FIFO full */ 
#define M5272_UART_USR_RxRDY	(1 << 0)	/* receiver ready */ 

#define M5272_UART_UCSR_RCS	(0xf << 4)	/* receiver clock select */ 
#define M5272_UART_UCSR_TCS	(0xf << 0)	/* transmitter clock select */ 

#define M5272_UART_UCR_ENAB	(1 << 7)	/* enable autobaud */ 
#define M5272_UART_UCR_MISC	(7 << 4)	/* misc field */ 
#define M5272_UART_UCR_TC	(3 << 2)	/* TC field */ 
#define M5272_UART_UCR_RC	(3 << 0)	/* RC field */ 

#define M5272_UART_UIPCR_COS	(1 << 4)	/* change of state */ 
#define M5272_UART_UIPCR_CTS	(1 << 0)	/* current state */ 

#define M5272_UART_UACR_RTSL	(3 << 1)	/* /RTS level */ 
#define M5272_UART_UACR_IEC	(1 << 0)	/* input enable control */ 

#define M5272_UART_UISR_UIMR_COS	(1 << 7)	/* change of state */ 
#define M5272_UART_UISR_UIMR_ABC	(1 << 6)	/* autobaud calc. */ 
#define M5272_UART_UISR_UIMR_RxFIFO	(1 << 5)	/* rec. FIFO status */ 
#define M5272_UART_UISR_UIMR_TxFIFO	(1 << 4)	/* trans. FIFO status */ 
#define M5272_UART_UISR_UIMR_RxFTO	(1 << 3)	/* rec. FIFO timeout */ 
#define M5272_UART_UISR_UIMR_DB		(1 << 2)	/* delta break */ 
#define M5272_UART_UISR_UIMR_FFULL_RxRDY	(1 << 1)	/* FFULL/RxRDY*/ 
#define M5272_UART_UISR_UIMR_TxRDY	(1 << 0)	/* transmitter ready */ 

#define M5272_UART_UTF_TXS	(3 << 6)	/* transmitter status */ 
#define M5272_UART_UTF_FULL	(1 << 5)	/* transmitter FIFO full */ 
#define M5272_UART_UTF_TXB	(0x1f << 0)	/* transmitter buf data level */ 

#define M5272_UART_URF_RXS	(3 << 6)	/* receiver status */ 
#define M5272_UART_URF_FULL	(1 << 5)	/* receiver FIFO full */ 
#define M5272_UART_URF_RXB	(0x1f << 0)	/* receiver buf data level */ 

/* SDRAM Controller Memory Map */
#define M5272_SDRAM_SDCR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x180))
#define M5272_SDRAM_SDTR(base)	(CAST_M5272(volatile UINT16 *)((base) + 0x184))

/* SDCR - SDRAM Configuration register */
#define M5272_SDRAM_SDCR_MCAS	(3 << 13)	/* Maximum CAS address */
#define M5272_SDRAM_SDCR_MCAS_A7	(0 << 13)
#define M5272_SDRAM_SDCR_MCAS_A8	(1 << 13)
#define M5272_SDRAM_SDCR_MCAS_A9	(2 << 13)
#define M5272_SDRAM_SDCR_MCAS_A10	(3 << 13)
#define M5272_SDRAM_SDCR_BALOC	(7 << 8)	/* Bank address location */
#define M5272_SDRAM_SDCR_BALOC_A20	(1 << 8) /* SDBA0 = A20, SDBA1 = A21 */
#define M5272_SDRAM_SDCR_BALOC_A21	(2 << 8) /* SDBA0 = A21, SDBA1 = A22 */
#define M5272_SDRAM_SDCR_BALOC_A22	(3 << 8) /* SDBA0 = A22, SDBA1 = A23 */
#define M5272_SDRAM_SDCR_BALOC_A23	(4 << 8) /* SDBA0 = A23, SDBA1 = A24 */
#define M5272_SDRAM_SDCR_BALOC_A24	(5 << 8) /* SDBA0 = A24, SDBA1 = A25 */
#define M5272_SDRAM_SDCR_GSL	(1 << 7)	/* Go to sleep */
#define M5272_SDRAM_SDCR_REG	(1 << 4)	/* Register read data */
#define M5272_SDRAM_SDCR_INV	(1 << 3)	/* Invert clock */
#define M5272_SDRAM_SDCR_SLEEP	(1 << 2)	/* Sleep mode */
#define M5272_SDRAM_SDCR_ACT	(1 << 1)	/* Active */
#define M5272_SDRAM_SDCR_INIT	(1 << 0)	/* Initialisation enable */

/* SDTR - SDRAM Timing Register */
#define M5272_SDRAM_SDTR_RTP	0xfc00		/* Refresh timing prescalar */
#define M5272_SDRAM_SDTR_RTP_66	(61 << 10)	/* 66MHz */
#define M5272_SDRAM_SDTR_RTP_48	(43 << 10)	/* 48MHz */
#define M5272_SDRAM_SDTR_RTP_33	(29 << 10)	/* 33MHz */
#define M5272_SDRAM_SDTR_RTP_25	(22 << 10)	/* 25MHz */
#define M5272_SDRAM_SDTR_RTP_5	(4 << 10)	/* 5MHz (emulator) */
#define M5272_SDRAM_SDTR_RC	(3 << 8)	/* Refresh count */
#define M5272_SDRAM_SDTR_RC_5	(0 << 8)	/* 5 cycles */
#define M5272_SDRAM_SDTR_RC_6	(1 << 8)	/* 6 cycles */
#define M5272_SDRAM_SDTR_RC_7	(2 << 8)	/* 7 cycles */
#define M5272_SDRAM_SDTR_RC_8	(3 << 8)	/* 8 cycles */
#define M5272_SDRAM_SDTR_RP	(3 << 4)	/* Precharge time */
#define M5272_SDRAM_SDTR_RP_1	(0 << 4)	/* 1 cycles */
#define M5272_SDRAM_SDTR_RP_2	(1 << 4)	/* 2 cycles */
#define M5272_SDRAM_SDTR_RP_3	(2 << 4)	/* 3 cycles */
#define M5272_SDRAM_SDTR_RP_4	(3 << 4)	/* 4 cycles */
#define M5272_SDRAM_SDTR_RCD	(3 << 2)	/* RAS-CAS delay */
#define M5272_SDRAM_SDTR_RCD_1	(0 << 2)	/* 1 cycles */
#define M5272_SDRAM_SDTR_RCD_2	(1 << 2)	/* 2 cycles */
#define M5272_SDRAM_SDTR_RCD_3	(2 << 2)	/* 3 cycles */
#define M5272_SDRAM_SDTR_RCD_4	(3 << 2)	/* 4 cycles */
#define M5272_SDRAM_SDTR_CLT	(3 << 0)	/* CAS latency */
#define M5272_SDRAM_SDTR_CLT_2	(1 << 0)	/* 2 cycles */

/* Timers 0 and 1 */
#define M5272_TIMER_REG(base, offset, timer)	\
			((base) + ((timer) * 0x20) + (offset))
#define M5272_TIMER_TMR(base, n)	\
		(CAST_M5272(volatile UINT16 *) M5272_TIMER_REG(base, 0x200, n))
#define M5272_TIMER_TRR(base, n)	\
		(CAST_M5272(volatile UINT16 *) M5272_TIMER_REG(base, 0x204, n))
#define M5272_TIMER_TCR(base, n)	\
		(CAST_M5272(volatile UINT16 *) M5272_TIMER_REG(base, 0x208, n))
#define M5272_TIMER_TCN(base, n)	\
		(CAST_M5272(volatile UINT16 *) M5272_TIMER_REG(base, 0x20c, n))
#define M5272_TIMER_TER(base, n)	\
		(CAST_M5272(volatile UINT16 *) M5272_TIMER_REG(base, 0x210, n))

#define M5272_TIMER_TMR_PRESCALER	(0xffff << 8)	/* prescaler */ 
#define M5272_TIMER_TMR_CE	(3 << 6)	/* capture edge */ 
#define M5272_TIMER_TMR_OM	(1 << 5)	/* output mode */ 
#define M5272_TIMER_TMR_ORI	(1 << 4)	/* output reference */ 
#define M5272_TIMER_TMR_FRR	(1 << 3)	/* free run/restart */ 
#define M5272_TIMER_TMR_CLK	(3 << 1)	/* input clock source */ 
#define M5272_TIMER_TMR_RST	(1 << 0)	/* reset timer */ 

#define M5272_TIMER_TER_REF	(1 << 1)	/* output reference event */ 
#define M5272_TIMER_TER_CAP	(1 << 0)	/* capture event */ 

/* PLIC Module Memory Map */
#define M5272_PLIC_BASE(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x300))

/* Ethernet Module Memory Map */
#define M5272_FEC_BASE(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x840))

/* USB Module Memory Map */
#define M5272_USB_BASE(base)	(CAST_M5272(volatile UINT32 *)((base) + 0x1000))

/* ISR - Interrupt Source Register bit definitions */
#define M5272_ISR_INT1		(1 << 31)
#define M5272_ISR_INT2		(1 << 30)
#define M5272_ISR_INT3		(1 << 29)
#define M5272_ISR_INT4		(1 << 28)
#define M5272_ISR_TMR1		(1 << 27)
#define M5272_ISR_TMR2		(1 << 26)
#define M5272_ISR_TMR3		(1 << 25)
#define M5272_ISR_TMR4		(1 << 24)
#define M5272_ISR_UART1		(1 << 23)
#define M5272_ISR_UART2		(1 << 22)
#define M5272_ISR_PLI_P		(1 << 21)
#define M5272_ISR_PLI_A		(1 << 20)
#define M5272_ISR_USB0		(1 << 19)
#define M5272_ISR_USB1		(1 << 18)
#define M5272_ISR_USB2		(1 << 17)
#define M5272_ISR_USB3		(1 << 16)
#define M5272_ISR_USB4		(1 << 15)
#define M5272_ISR_USB5		(1 << 14)
#define M5272_ISR_USB6		(1 << 13)
#define M5272_ISR_USB7		(1 << 12)
#define M5272_ISR_DMA		(1 << 11)
#define M5272_ISR_ERx		(1 << 10)
#define M5272_ISR_ETx		(1 <<  9)
#define M5272_ISR_ENTC		(1 <<  8)
#define M5272_ISR_QSPI		(1 <<  7)
#define M5272_ISR_INT5		(1 <<  6)
#define M5272_ISR_INT6		(1 <<  5)
#define M5272_ISR_SWTO		(1 <<  4)

/* Interrupt numbers on the 5272 are at fixed positions above
   the base value (INT_NUM_BASE) defined in the BSP (config.h.)
   INT_NUM_BASE must be a multiple of 32, and not less than 64.
*/
#define INT_NUM_SPURIOUS    (INT_NUM_BASE+0x00)     /* reserved */
#define INT_NUM_INT1        (INT_NUM_BASE+0x01)     /* external int 1 */
#define INT_NUM_INT2        (INT_NUM_BASE+0x02)     /* external int 2 */
#define INT_NUM_INT3        (INT_NUM_BASE+0x03)     /* external int 3 */
#define INT_NUM_INT4        (INT_NUM_BASE+0x04)     /* external int 4 */
#define INT_NUM_TMR1        (INT_NUM_BASE+0x05)     /* timer 1 */
#define INT_NUM_TMR2        (INT_NUM_BASE+0x06)     /* timer 2 */
#define INT_NUM_TMR3        (INT_NUM_BASE+0x07)     /* timer 3 */
#define INT_NUM_TMR4        (INT_NUM_BASE+0x08)     /* timer 4 */
#define INT_NUM_UART1       (INT_NUM_BASE+0x09)     /* uart 1 */
#define INT_NUM_UART2       (INT_NUM_BASE+0x0a)     /* uart 2 */
#define INT_NUM_PLIP        (INT_NUM_BASE+0x0b)     /* plic 2kHz periodic */
#define INT_NUM_PLIA        (INT_NUM_BASE+0x0c)     /* plic async */
#define INT_NUM_USB0        (INT_NUM_BASE+0x0d)     /* usb endpoint 0 */
#define INT_NUM_USB1        (INT_NUM_BASE+0x0e)     /* usb endpoint 1 */
#define INT_NUM_USB2        (INT_NUM_BASE+0x0f)     /* usb endpoint 2 */
#define INT_NUM_USB3        (INT_NUM_BASE+0x10)     /* usb endpoint 3 */
#define INT_NUM_USB4        (INT_NUM_BASE+0x11)     /* usb endpoint 4 */
#define INT_NUM_USB5        (INT_NUM_BASE+0x12)     /* usb endpoint 5 */
#define INT_NUM_USB6        (INT_NUM_BASE+0x13)     /* usb endpoint 6 */
#define INT_NUM_USB7        (INT_NUM_BASE+0x14)     /* usb endpoint 7 */
#define INT_NUM_DMA         (INT_NUM_BASE+0x15)     /* dma controller */
#define INT_NUM_ERX         (INT_NUM_BASE+0x16)     /* ethernet receiver */
#define INT_NUM_ETX         (INT_NUM_BASE+0x17)     /* ethernet transmitter */
#define INT_NUM_ENTC        (INT_NUM_BASE+0x18)     /* ethernet module */
#define INT_NUM_QSPI        (INT_NUM_BASE+0x19)     /* queued serial i'f */
#define INT_NUM_INT5        (INT_NUM_BASE+0x1a)     /* external int 5 */
#define INT_NUM_INT6        (INT_NUM_BASE+0x1b)     /* external int 6 */
#define INT_NUM_SWTO        (INT_NUM_BASE+0x1c)     /* s/w watchdog */
#define INT_NUM_RES1        (INT_NUM_BASE+0x1d)     /* reserved */
#define INT_NUM_RES2        (INT_NUM_BASE+0x1e)     /* reserved */
#define INT_NUM_RES3        (INT_NUM_BASE+0x1f)     /* reserved */

#ifdef __cplusplus
}
#endif

#endif /* __INCm5272h */
