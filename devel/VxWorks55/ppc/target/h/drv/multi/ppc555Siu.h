/* ppc555Siu.h - PowerPC 555 Unified System Interface Unit header file */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,26aug99,cmc  Added new definitions
01b,15apr99.cmc	 Fixed PISCR_PIRQ values
01a,17mar99,zl 	 created.
*/

/*
This file contains constants of the System Interface Unit (SIU) for the
Motorola MPC555 PowerPC microcontroller
*/

#ifndef __INCppc555Siuh
#define __INCppc555Siuh

#ifdef __cplusplus
extern "C" {
#endif

#ifdef	_ASMLANGUAGE
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
#endif	/* _ASMLANGUAGE */


/* 
 * MPC555 SIU internal register/memory map 
 */

/* General SIU registers */

#define SIUMCR(base)	(CAST(VUINT32 *) (base + 0x2FC000)) /* SIU Module Config*/
#define	SYPCR(base)	(CAST(VUINT32 *) (base + 0x2FC004)) /* Protection Ctrl */
#define	SWSR(base)	(CAST(VUINT16 *) (base + 0x2FC00E)) /* SW Service Reg */
#define	SIPEND(base)	(CAST(VUINT32 *) (base + 0x2FC010)) /* Intr Pending reg */
#define	SIMASK(base)	(CAST(VUINT32 *) (base + 0x2FC014)) /* Intr Mask reg */
#define	SIEL(base)	(CAST(VUINT32 *) (base + 0x2FC018)) /* Intr Edge Lvl */
#define	SIVEC(base)	(CAST(VUINT32 *) (base + 0x2FC01C)) /* Intr Vector reg */
#define	TESR(base)	(CAST(VUINT32 *) (base + 0x2FC020)) /* Tx Error Status */
#define	SGPIODT1(base)	(CAST(VUINT32 *) (base + 0x2FC024)) /* GP I/O Data reg */
#define	SGPIODT2(base)	(CAST(VUINT32 *) (base + 0x2FC028)) /* GP I/O Data reg 2 */
#define	SGPIOCR(base)	(CAST(VUINT32 *) (base + 0x2FC02C)) /* GP I/O Control reg */
#define	EMCR(base)	(CAST(VUINT32 *) (base + 0x2FC030)) /* Ext Mstr Mode Ctrl */
#define	PDMCR(base)	(CAST(VUINT16 *) (base + 0x2FC03C)) /* Pads Module Ctrl */

/* MEMC registers */

#define BR0(base)	(CAST(VUINT32 *) (base + 0x2FC100)) /* Base Reg bank 0 */
#define OR0(base)	(CAST(VUINT32 *) (base + 0x2FC104)) /* Option Reg bank 0*/
#define BR1(base)	(CAST(VUINT32 *) (base + 0x2FC108)) /* Base Reg bank 1 */
#define OR1(base)	(CAST(VUINT32 *) (base + 0x2FC10C)) /* Option Reg bank 1*/
#define BR2(base)	(CAST(VUINT32 *) (base + 0x2FC110)) /* Base Reg bank 2 */
#define OR2(base)	(CAST(VUINT32 *) (base + 0x2FC114)) /* Option Reg bank 2*/
#define BR3(base)	(CAST(VUINT32 *) (base + 0x2FC118)) /* Base Reg bank 3 */
#define OR3(base)	(CAST(VUINT32 *) (base + 0x2FC11C)) /* Option Reg bank 3*/

#define DMBR(base)	(CAST(VUINT32 *) (base + 0x2FC140)) /* Dual-Mapping Base reg*/
#define DMOR(base)	(CAST(VUINT32 *) (base + 0x2FC144)) /* Dual-Mapping Opt reg*/
#define MSTAT(base)	(CAST(VUINT16 *) (base + 0x2FC178)) /* Memory Status */

/* System Integration Timers */

#define TBSCR(base)	(CAST(VUINT16 *) (base + 0x2FC200)) /* T.B. Status Ctrl */
#define TBREFF0(base)	(CAST(VUINT32 *) (base + 0x2FC204)) /* Time Base Ref 0 */
#define TBREFF1(base)	(CAST(VUINT32 *) (base + 0x2FC208)) /* Time Base Ref 1 */

#define RTCSC(base)	(CAST(VUINT16 *) (base + 0x2FC220)) /* Clock Status Ctrl*/
#define RTC(base)	(CAST(VUINT32 *) (base + 0x2FC224)) /* RT Clock */
#define RTSEC(base)	(CAST(VUINT32 *) (base + 0x2FC228)) /* RT Alarm Seconds */
#define RTCAL(base)	(CAST(VUINT32 *) (base + 0x2FC22C)) /* Real Time Alarm */

#define PISCR(base)	(CAST(VUINT16 *) (base + 0x2FC240)) /* PIT Status Ctrl */
#define PITC(base)	(CAST(VUINT32 *) (base + 0x2FC244)) /* PIT Count */
#define PITR(base)	(CAST(VUINT32 *) (base + 0x2FC248)) /* PIT */

/* Clock and Reset */

#define SCCR(base)	(CAST(VUINT32 *) (base + 0x2FC280)) /* System Clock Ctrl*/
#define PLPRCR(base)	(CAST(VUINT32 *) (base + 0x2FC284)) /* PLL, LPower Reset*/
#define RSR(base)	(CAST(VUINT16 *) (base + 0x2FC288)) /* Reset Status Reg */
#define COLIR(base)	(CAST(VUINT16 *) (base + 0x2FC28C)) /* Chg of Lock Int reg*/
#define VSRMCR(base)	(CAST(VUINT16 *) (base + 0x2FC290)) /* VDDSRM Ctrl reg*/

/* System Integration Timers Keys */

#define TBSCRK(base)	(CAST(VUINT32 *) (base + 0x2FC300)) /* TB Stat Ctrl key */
#define TBREFF0K(base)	(CAST(VUINT32 *) (base + 0x2FC304)) /* TB Ref 0 Key */
#define TBREFF1K(base)	(CAST(VUINT32 *) (base + 0x2FC308)) /* TB Ref 1 Key */
#define TBK(base)	(CAST(VUINT32 *) (base + 0x2FC30C)) /* TB & Dec Key */

#define RTCSCK(base)	(CAST(VUINT32 *) (base + 0x2FC320)) /* RT Stat Ctrl Key */
#define RTCK(base)	(CAST(VUINT32 *) (base + 0x2FC324)) /* RT Clock Key */
#define RTSECK(base)	(CAST(VUINT32 *) (base + 0x2FC328)) /* RT Alarm Second */
#define RTCALK(base)	(CAST(VUINT32 *) (base + 0x2FC32C)) /* R T Alarm Key */

#define PISCRK(base)	(CAST(VUINT32 *) (base + 0x2FC340)) /* PIT Stat Ctrl Key*/
#define PITCK(base)	(CAST(VUINT32 *) (base + 0x2FC344)) /* PIT Count Key */

/* Clock and Reset Keys */

#define SCCRK(base)	(CAST(VUINT32 *) (base + 0x2FC380)) /* System Clk Ctrl */
#define PLPRCRK(base)	(CAST(VUINT32 *) (base + 0x2FC384)) /* Pll, LP&R Ctrl */
#define RSRK(base)	(CAST(VUINT32 *) (base + 0x2FC388)) /* Reset Status Key */
#define  UMCR(base)     (CAST(VUINT32 *) (base + 0x307F80)) /* IMB Module Config */

/*
 * SIU register bit definitions 
 */

/* SIU Module Configuration register bit definition (SIUMCR - 0x00) */

#define SIUMCR_EARB	0x80000000	/* External Abritation */
#define SIUMCR_EARP	0x70000000	/* Extern Abri. Req. prior.*/
#define SIUMCR_DSHW	0x00800000	/* Data Showcycles */
#define SIUMCR_DBGC	0x00600000	/* Debug pins conf */
#define SIUMCR_DBPC	0x00100000	/* Debug Port pins conf */
#define SIUMCR_ATWC	0x00080000	/* Addr wrt type enable conf */
#define SIUMCR_GPC	0x00060000	/* General pins conf */
#define SIUMCR_DLK	0x00010000	/* Debug Register Lock */
#define SIUMCR_SC	0x00006000	/* Single-chip select*/
#define SIUMCR_RCTX	0x00001000	/* Reset conf/timer exp.*/
#define SIUMCR_MLRC	0x00000C00	/* Multi-level reserv. ctrl*/
#define SIUMCR_MTSC	0x00000010	/* Memory transfer start ctrl*/

/* System Portection Control register bit definition (SYPCR - 0x04) */

#define SYPCR_SWTC	0xffff0000	/* Software Watchdog Timer Count */
#define SYPCR_BMT	0x0000ff00	/* Bus Monitor Timing */
#define SYPCR_BME	0x00000080	/* Bus Monitor Enable */
#define SYPCR_SWF	0x00000008	/* Software Watchdog Freeze */
#define SYPCR_SWE	0x00000004	/* Software Watchdog Enable */
#define SYPCR_SWRI	0x00000002	/* Software Watchdog Reset/Int Sel */
#define SYPCR_SWP	0x00000001	/* Software Watchdog Prescale */

/* System Interrupt PENDing register bit definition (SIPEND - 0x10) */

#define SIPEND_IRQ0	0x80000000	/* Interrupt IRQ0 pending */
#define SIPEND_LVL0	0x40000000	/* Interrupt LEVEL 0 pending */
#define SIPEND_IRQ1	0x20000000	/* Interrupt IRQ1 pending */
#define SIPEND_LVL1	0x10000000	/* Interrupt LEVEL 1 pending */
#define SIPEND_IRQ2	0x08000000	/* Interrupt IRQ2 pending */
#define SIPEND_LVL2	0x04000000	/* Interrupt LEVEL 2 pending */
#define SIPEND_IRQ3	0x02000000	/* Interrupt IRQ3 pending */
#define SIPEND_LVL3	0x01000000	/* Interrupt LEVEL 3 pending */
#define SIPEND_IRQ4	0x00800000	/* Interrupt IRQ4 pending */
#define SIPEND_LVL4	0x00400000	/* Interrupt LEVEL 4 pending */
#define SIPEND_IRQ5	0x00200000	/* Interrupt IRQ5 pending */
#define SIPEND_LVL5	0x00100000	/* Interrupt LEVEL 5 pending */
#define SIPEND_IRQ6	0x00080000	/* Interrupt IRQ6 pending */
#define SIPEND_LVL6	0x00040000	/* Interrupt LEVEL 6 pending */
#define SIPEND_IRQ7	0x00020000	/* Interrupt IRQ7 pending */
#define SIPEND_LVL7	0x00010000	/* Interrupt LEVEL 7 pending */

/* System Interrupt MASK register bit definition (SIMASK - 0x14) */

#define SIMASK_IRM0	0x80000000	/* Interrupt IRQ0 mask */
#define SIMASK_LVM0	0x40000000	/* Interrupt LEVEL 0 mask */
#define SIMASK_IRM1	0x20000000	/* Interrupt IRQ1 mask */
#define SIMASK_LVM1	0x10000000	/* Interrupt LEVEL 1 mask */
#define SIMASK_IRM2	0x08000000	/* Interrupt IRQ2 mask */
#define SIMASK_LVM2	0x04000000	/* Interrupt LEVEL 2 mask */
#define SIMASK_IRM3	0x02000000	/* Interrupt IRQ3 mask */
#define SIMASK_LVM3	0x01000000	/* Interrupt LEVEL 3 mask */
#define SIMASK_IRM4	0x00800000	/* Interrupt IRQ4 mask */
#define SIMASK_LVM4	0x00400000	/* Interrupt LEVEL 4 mask */
#define SIMASK_IRM5	0x00200000	/* Interrupt IRQ5 mask */
#define SIMASK_LVM5	0x00100000	/* Interrupt LEVEL 5 mask */
#define SIMASK_IRM6	0x00080000	/* Interrupt IRQ6 mask */
#define SIMASK_LVM6	0x00040000	/* Interrupt LEVEL 6 mask */
#define SIMASK_IRM7	0x00020000	/* Interrupt IRQ7 mask */
#define SIMASK_LVM7	0x00010000	/* Interrupt LEVEL 7 mask */
#define SIMASK_ALL	0xffff0000	/* All interrupt mask */

/* System Interrupt Edge Level mask register bit definition (SIEL - 0x1C) */

#define SIEL_ED0	0x80000000	/* Interrupt IRQ0 on falling Edge */
#define SIEL_WM0	0x40000000	/* Interrupt IRQ0 Wake up Mask */
#define SIEL_ED1	0x20000000	/* Interrupt IRQ1 on falling Edge */
#define SIEL_WM1	0x10000000	/* Interrupt IRQ1 Wake up Mask */
#define SIEL_ED2	0x08000000	/* Interrupt IRQ2 on falling Edge */
#define SIEL_WM2	0x04000000	/* Interrupt IRQ2 Wake up Mask */
#define SIEL_ED3	0x02000000	/* Interrupt IRQ3 on falling Edge */
#define SIEL_WM3	0x01000000	/* Interrupt IRQ3 Wake up Mask */
#define SIEL_ED4	0x00800000	/* Interrupt IRQ4 on falling Edge */
#define SIEL_WM4	0x00400000	/* Interrupt IRQ4 Wake up Mask */
#define SIEL_ED5	0x00200000	/* Interrupt IRQ5 on falling Edge */
#define SIEL_WM5	0x00100000	/* Interrupt IRQ5 Wake up Mask */
#define SIEL_ED6	0x00080000	/* Interrupt IRQ6 on falling Edge */
#define SIEL_WM6	0x00040000	/* Interrupt IRQ6 Wake up Mask */
#define SIEL_ED7	0x00020000	/* Interrupt IRQ7 on falling Edge */
#define SIEL_WM7	0x00010000	/* Interrupt IRQ7 Wake up Mask */

/* Transfert Error Status register bit definition (TESR - 0x20) */

#define TESR_IEXT	0x00002000	/* Instr. External Transfer Error Ack */
#define TESR_IBM	0x00001000	/* Instr. transfer Monitor Time-Out */
#define TESR_DEXT	0x00000020	/* Data External Transfer Error Ack */
#define TESR_DBM	0x00000010	/* Data transfer Monitor Time-Out */

/* Base Register bit definition (BRx - 0x100) */

#define BR_BA_MSK	0xffff8000	/* Base Address Mask */
#define BR_AT_MSK	0x00007000	/* Address Type Mask */
#define BR_PS_MSK	0x00000c00	/* Port Size Mask */
#define BR_PS_8		0x00000400	/* 8 bit port size */
#define BR_PS_16	0x00000800	/* 16 bit port size */
#define BR_PS_32	0x00000000	/* 32 bit port size */
#define BR_WP		0x00000100	/* Write Protect */
#define BR_WEBS		0x00000020	/* Write-enable/byte-select. */
#define BR_TBDIP	0x00000010	/* Toggle-burst data in progress. */
#define BR_LBDIP	0x00000008	/* Late-burst-data-in-progress */
#define BR_SETA		0x00000004	/* External transfer acknowledge */
#define BR_BI		0x00000002	/* Burst inhibit */
#define BR_V		0x00000001	/* Bank valid */

/* Option Register bit definition (ORx - 0x104) */

#define OR_AM_MSK	0xffff8000	/* Address Mask Mask */
#define OR_ATM_MSK	0x00007000	/* Address Type Mask Mask */
#define OR_CSNT_SAM	0x00000800	/* Chip Select Negation Time/ Start */
					/* Address Multiplex */
#define OR_ACS_MSK	0x00000600	/* Address to Chip Select Setup mask */
#define OR_ACS_DIV1	0x00000000	/* CS is output at the same time */
#define OR_ACS_DIV4	0x00000400	/* CS is output 1/4 a clock later */
#define OR_ACS_DIV2	0x00000600	/* CS is output 1/2 a clock later */
#define OR_EHTR		0x00000100	/* Extended hold time on read */
#define OR_SCY_MSK	0x000000f0	/* Cycle Lenght in Clocks */
#define OR_SCY_0_CLK	0x00000000	/* 0 clock cycles wait states */
#define OR_SCY_1_CLK	0x00000010	/* 1 clock cycles wait states */
#define OR_SCY_2_CLK	0x00000020	/* 2 clock cycles wait states */
#define OR_SCY_3_CLK	0x00000030	/* 3 clock cycles wait states */
#define OR_SCY_4_CLK	0x00000040	/* 4 clock cycles wait states */
#define OR_SCY_5_CLK	0x00000050	/* 5 clock cycles wait states */
#define OR_SCY_6_CLK	0x00000060	/* 6 clock cycles wait states */
#define OR_SCY_7_CLK	0x00000070	/* 7 clock cycles wait states */
#define OR_SCY_8_CLK	0x00000080	/* 8 clock cycles wait states */
#define OR_SCY_9_CLK	0x00000090	/* 9 clock cycles wait states */
#define OR_SCY_10_CLK	0x000000a0	/* 10 clock cycles wait states */
#define OR_SCY_11_CLK	0x000000b0	/* 11 clock cycles wait states */
#define OR_SCY_12_CLK	0x000000c0	/* 12 clock cycles wait states */
#define OR_SCY_13_CLK	0x000000d0	/* 13 clock cycles wait states */
#define OR_SCY_14_CLK	0x000000e0	/* 14 clock cycles wait states */
#define OR_SCY_15_CLK	0x000000f0	/* 15 clock cycles wait states */
#define OR_BSCY_MSK	0x0000000e	/* Burst beats length in clocks. */
#define OR_BSCY_0_CLK	0x00000000	/* 0 clock cycle wait states */
#define OR_BSCY_1_CLK	0x00000002	/* 1 clock cycle wait states */
#define OR_BSCY_2_CLK	0x00000004	/* 2 clock cycle wait states */
#define OR_BSCY_3_CLK	0x00000006	/* 3 clock cycle wait states */
#define OR_TRLX		0x00000001	/* Timing Relaxed */

/* Memory STATus register bit definition (MSTAT - 0x178) */

#define MSTAT_WPER0	0x0080		/* Write Protection Error bank 0 */
#define MSTAT_WPER1	0x0040		/* Write Protection Error bank 1 */
#define MSTAT_WPER2	0x0020		/* Write Protection Error bank 2 */
#define MSTAT_WPER3	0x0010		/* Write Protection Error bank 3 */

/* Time Base Status and Contol register bit definition (TBSCR - 0x200) */

#define TBSCR_TBIRQ_MSK	0xFF00		/* Time Base IRQ Mask */
/* The following values were not verified */
#define TBSCR_TBIRQ_0	0x0100		/* Time Base Interrupt Request LVL 0 */
#define TBSCR_TBIRQ_1	0x0200		/* Time Base Interrupt Request LVL 1 */
#define TBSCR_TBIRQ_2	0x0400		/* Time Base Interrupt Request LVL 2 */
#define TBSCR_TBIRQ_3	0x0800		/* Time Base Interrupt Request LVL 3 */
#define TBSCR_TBIRQ_4	0x1000		/* Time Base Interrupt Request LVL 4 */
#define TBSCR_TBIRQ_5	0x2000		/* Time Base Interrupt Request LVL 5 */
#define TBSCR_TBIRQ_6	0x4000		/* Time Base Interrupt Request LVL 6 */
#define TBSCR_TBIRQ_7	0x8000		/* Time Base Interrupt Request LVL 7 */

#define TBSCR_REFA	0x0080		/* Reference Interrupt Status A */
#define TBSCR_REFB	0x0040		/* Reference Interrupt Status B */
#define TBSCR_REFAE	0x0008		/* Second Interrupt Enable A */
#define TBSCR_REFBE	0x0004		/* Second Interrupt Enable B */
#define TBSCR_TBF	0x0002		/* Time Base Freeze */
#define TBSCR_TBE	0x0001		/* Time Base Enable */

/* Real Time Clock Status and Control register bit definition (RTCSC - 0x220) */

#define RTCSC_RTCIRQ_MSK 0xFF00		/* Real Time Clock IRQ Mask */
#define RTCSC_RTCIRQ_0	0x0100		/* Real Time Clock IRQ LVL 0 */
#define RTCSC_RTCIRQ_1	0x0200		/* Real Time Clock IRQ LVL 1 */
#define RTCSC_RTCIRQ_2	0x0400		/* Real Time Clock IRQ LVL 2 */
#define RTCSC_RTCIRQ_3	0x0800		/* Real Time Clock IRQ LVL 3 */
#define RTCSC_RTCIRQ_4	0x1000		/* Real Time Clock IRQ LVL 4 */
#define RTCSC_RTCIRQ_5	0x2000		/* Real Time Clock IRQ LVL 5 */
#define RTCSC_RTCIRQ_6	0x4000		/* Real Time Clock IRQ LVL 6 */
#define RTCSC_RTCIRQ_7	0x8000		/* Real Time Clock IRQ LVL 7 */
#define RTCSC_SEC	0x0080		/* Once per Second interrupt */
#define RTCSC_ALR	0x0040		/* Alarm interrupt */
#define RTCSC_4M	0x0010		/* Real Time Clock at 4 MHz */
#define RTCSC_SIE	0x0008		/* Second Interrupt Enable */
#define RTCSC_ALE	0x0004		/* Alarm Interrupt Enable */
#define RTCSC_RTF	0x0002		/* Real Time Clock Freeze */
#define RTCSC_RTE	0x0001		/* Real Time Clock Enable */

/* PIT Status and Control Register bit definition (PISCR - 0x0240) */

#define PISCR_PIRQ_MSK	0xFF00		/* PIT IRQ Mask */
#define PISCR_PIRQ_0	0x8000		/* PIT IRQ LVL 0 */
#define PISCR_PIRQ_1	0x4000		/* PIT IRQ LVL 1 */
#define PISCR_PIRQ_2	0x2000		/* PIT IRQ LVL 2 */
#define PISCR_PIRQ_3	0x1000		/* PIT IRQ LVL 3 */
#define PISCR_PIRQ_4	0x0800		/* PIT IRQ LVL 4 */
#define PISCR_PIRQ_5	0x0400		/* PIT IRQ LVL 5 */
#define PISCR_PIRQ_6	0x0200		/* PIT IRQ LVL 6 */
#define PISCR_PIRQ_7	0x0100		/* PIT IRQ LVL 7 */
#define PISCR_PS	0x0080		/* Periodic interrupt Status */
#define PISCR_PIE	0x0004		/* Periodic Interrupt Enable */
#define PISCR_PITF	0x0002		/* Periodic Interrupt Timer Freeze */
#define PISCR_PTE	0x0001		/* Periodic Timer Enable */

/* System Clock Control Register bit definition (SCCR - 0x280) */

#define SCCR_DBCT	0x80000000	/* Disable backup clock for timers */
#define SCCR_COM_MSK	0x60000000	/* Clock output mode */
#define SCCR_DCSLR	0x10000000	/* Disable clock switching */
#define SCCR_MFPDL	0x08000000	/* MF and pre-divider lock */
#define SCCR_LPML	0x04000000	/* LPM lock */
#define SCCR_TBS	0x02000000	/* Time base source */
#define SCCR_RTDIV_MSK	0x01000000	/* RTC (and PIT) clock divider */
#define SCCR_STBUC	0x00800000	/* Switch to backup clock control */
#define SCCR_PRQEN	0x00200000	/* Power management request enable */
#define SCCR_RTSEL	0x00100000	/* RTC circuit input source select */
#define SCCR_BUCS	0x00080000	/* Backup clock status */
#define SCCR_EBDF_MSK	0x00060000	/* External bus division factor */
#define SCCR_LME	0x00010000	/* Limp mode enable */
#define SCCR_EECLK_MSK	0x0000c000	/* Enable engineering clock */
#define SCCR_ENGDIV_MSK	0x00003f00	/* Engineering clock division factor */
#define SCCR_DFNL_MSK	0x00000070	/* Division factor low frequency */
#define SCCR_DFNH_MSK	0x00000007	/* Division factor high frequency */

/* Pll, Low Power and Reset Control Register bit definition (PLPRCR - 284) */

#define PLPRCR_MF_MSK	0xfff00000	/* Multiplication factor bits */
#define PLPRCR_MF_SHIFT 0x00000014      /* Multiplication factor shift value */
#define PLPRCR_LOCS	0x00040000	/* Loss of clock status */
#define PLPRCR_LOCSS	0x00020000	/* Loss of clock sticky */
#define PLPRCR_SPLS	0x00010000	/* System PLL lock status */

#define PLPRCR_SPLSS	0x00008000	/* SPLL Lock Status Sticky bit */
#define PLPRCR_TEXPS	0x00004000	/* TEXP Status */
#define PLPRCR_TMIST	0x00001000	/* Timers Interrupt Status */
#define PLPRCR_CSRC	0x00000400	/* Clock Source */
#define PLPRCR_LPM_MSK	0x00000300	/* Low Power Mode mask */
#define	PLPRCR_LPM_NORMAL 0x00000000	/* normal power management mode */
#define	PLPRCR_LPM_DOZE	  0x00000100	/* doze power management mode */
#define	PLPRCR_LPM_SLEEP  0x00000200	/* sleep power management mode */
#define	PLPRCR_LPM_DEEP_SLEEP 0x00000300 /* deep sleep power mgt mode */
#define	PLPRCR_LPM_DOWN   0x00000300	/* down power management mode */
#define PLPRCR_CSR	0x00000080	/* CheskStop Reset value */
#define PLPRCR_LOLRE	0x00000040	/* Loss Of Lock Reset Enable */
#define PLPRCR_DIVF_MSK	0x0000001F	/* pre-divider in the SPLL circuit */	

/* BBC module config register */
#define BBCMCR_BE	0x00002000	/* burst enable */
#define BBCMCR_ETRE	0x00001000	/* exception relocation enable */
#define BBCMCR_OERC	0x00000800	/* other exception relocation control */

#ifdef  __cplusplus
}
#endif

#endif /* __INCppc555Siuh */
