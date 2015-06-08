/* ppc860Siu.h - Motorola System Interface Unit header file */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1997,1998 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01h,18sep01,pmr  fixed typo in definition of SIUMCR_MLRC_SPKROUT (SPR 64301).
01g,01dec98,cn   fixed SPR# 8614.
01f,23jun98,cn   added MPC860_REGB_OFFSET and MPC860_DPR_SCC1 macros.
01e,26mar98,map  added more LCD definitions.
01d,09dec96,tpr  added LCD register definition, corrected typo.
01c,11nov96,tam  added extra macros for power management (PLPRCR register).
01b,06nov96,tpr  added new macros.
01a,11apr96,tpr	 created.
*/

/*
This file contains constants of the System Interface Unit (SIU) for the
Motorola MPC860 and MPC821 PowerPC microcontroller
*/

#ifndef __INCppc860Siuh
#define __INCppc860Siuh

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
 * MPC860/MPC821 internal register/memory map (section 17 of prelim. spec)
 * note that these are offsets from the value stored in the IMMR 
 * register, which is in the PowerPC special register address space
 * at register number 638
 */

/* General SIU registers */

#define SIUMCR(base)	(CAST(VUINT32 *)((base) + 0x0000)) /* SIU Module Conf*/
#define	SYPCR(base)	(CAST(VUINT32 *)((base) + 0x0004)) /* Protection Ctrl */
#define	SWT(base)	(CAST(VUINT32 *)((base) + 0x0008)) /* SW watch dog */
#define	SWSR(base)	(CAST(VUINT16 *)((base) + 0x000E)) /* SW Service Reg */
#define	SIPEND(base)	(CAST(VUINT32 *)((base) + 0x0010)) /* Intr Pending reg*/
#define	SIMASK(base)	(CAST(VUINT32 *)((base) + 0x0014)) /* Intr Mask reg */
#define	SIEL(base)	(CAST(VUINT32 *)((base) + 0x0018)) /* Intr Edge Lvl */
#define	SIVEC(base)	(CAST(VUINT32 *)((base) + 0x001C)) /* Intr Vector reg */
#define	TESR(base)	(CAST(VUINT32 *)((base) + 0x0020)) /* Tx Error Status */
#define	SDCR(base)	(CAST(VUINT32 *)((base) + 0x0030)) /* SDMA Config Reg */

/* PCMCIA registers */

#define PBR0(base)	(CAST(VUINT32 *)((base) + 0x0080)) /* PCMCIA base 0 */
#define POR0(base)	(CAST(VUINT32 *)((base) + 0x0084)) /* PCMCIA option 0 */
#define PBR1(base)	(CAST(VUINT32 *)((base) + 0x0088)) /* PCMCIA base 1 */
#define POR1(base)	(CAST(VUINT32 *)((base) + 0x008C)) /* PCMCIA option 1 */
#define PBR2(base)	(CAST(VUINT32 *)((base) + 0x0090)) /* PCMCIA base 2 */
#define POR2(base)	(CAST(VUINT32 *)((base) + 0x0094)) /* PCMCIA option 2 */
#define PBR3(base)	(CAST(VUINT32 *)((base) + 0x0098)) /* PCMCIA base 3 */
#define POR3(base)	(CAST(VUINT32 *)((base) + 0x009C)) /* PCMCIA option 3 */
#define PBR4(base)	(CAST(VUINT32 *)((base) + 0x00A0)) /* PCMCIA base 4 */
#define POR4(base)	(CAST(VUINT32 *)((base) + 0x00A4)) /* PCMCIA option 4 */
#define PBR5(base)	(CAST(VUINT32 *)((base) + 0x00A8)) /* PCMCIA base 5 */
#define POR5(base)	(CAST(VUINT32 *)((base) + 0x00AC)) /* PCMCIA option 5 */
#define PBR6(base)	(CAST(VUINT32 *)((base) + 0x00B0)) /* PCMCIA base 6 */
#define POR6(base)	(CAST(VUINT32 *)((base) + 0x00B4)) /* PCMCIA option 6 */
#define PBR7(base)	(CAST(VUINT32 *)((base) + 0x00B8)) /* PCMCIA base 7 */
#define POR7(base)	(CAST(VUINT32 *)((base) + 0x00BC)) /* PCMCIA option 7 */

#define PGCRA(base)	(CAST(VUINT32 *)((base) + 0x00E0)) /* slot A ctrl */
#define PGCRB(base)	(CAST(VUINT32 *)((base) + 0x00E4)) /* slot B ctrl */
#define PSCR(base)	(CAST(VUINT32 *)((base) + 0x00E8)) /* PCMCIA Status */
#define PIPR(base)	(CAST(VUINT32 *)((base) + 0x00F0)) /* pins value */
#define PER(base)	(CAST(VUINT32 *)((base) + 0x00f8)) /* PCMCIA Enable */

/* MEMC registers */

#define BR0(base)	(CAST(VUINT32 *)((base) + 0x0100)) /* Base Reg bank 0 */
#define OR0(base)	(CAST(VUINT32 *)((base) + 0x0104)) /* Opt Reg bank 0*/
#define BR1(base)	(CAST(VUINT32 *)((base) + 0x0108)) /* Base Reg bank 1 */
#define OR1(base)	(CAST(VUINT32 *)((base) + 0x010C)) /* Opt Reg bank 1*/
#define BR2(base)	(CAST(VUINT32 *)((base) + 0x0110)) /* Base Reg bank 2 */
#define OR2(base)	(CAST(VUINT32 *)((base) + 0x0114)) /* Opt Reg bank 2*/
#define BR3(base)	(CAST(VUINT32 *)((base) + 0x0118)) /* Base Reg bank 3 */
#define OR3(base)	(CAST(VUINT32 *)((base) + 0x011C)) /* Opt Reg bank 3*/
#define BR4(base)	(CAST(VUINT32 *)((base) + 0x0120)) /* Base Reg bank 4 */
#define OR4(base)	(CAST(VUINT32 *)((base) + 0x0124)) /* Opt Reg bank 4*/
#define BR5(base)	(CAST(VUINT32 *)((base) + 0x0128)) /* Base Reg bank 5 */
#define OR5(base)	(CAST(VUINT32 *)((base) + 0x012C)) /* Opt Reg bank 5*/
#define BR6(base)	(CAST(VUINT32 *)((base) + 0x0130)) /* Base Reg bank 6 */
#define OR6(base)	(CAST(VUINT32 *)((base) + 0x0134)) /* Opt Reg bank 6*/
#define BR7(base)	(CAST(VUINT32 *)((base) + 0x0138)) /* Base Reg bank 7 */
#define OR7(base)	(CAST(VUINT32 *)((base) + 0x013C)) /* Opt Reg bank 7*/

#define MAR(base)	(CAST(VUINT32 *)((base) + 0x0164)) /* Memory Address */
#define MCR(base)	(CAST(VUINT32 *)((base) + 0x0168)) /* Memory Command */
#define MAMR(base)	(CAST(VUINT32 *)((base) + 0x0170)) /* Machine A Mode */
#define MBMR(base)	(CAST(VUINT32 *)((base) + 0x0174)) /* Machine B Mode */
#define MSTAT(base)	(CAST(VUINT16 *)((base) + 0x0178)) /* Memory Status */
#define MPTPR(base)	(CAST(VUINT16 *)((base) + 0x017A)) /* Mem Timer Presc*/
#define MDR(base)	(CAST(VUINT32 *)((base) + 0x017C)) /* Memory Data */

/* System Integration Timers */

#define TBSCR(base)	(CAST(VUINT16 *)((base) + 0x0200)) /* T.B. Status Ctrl*/
#define TBREFF0(base)	(CAST(VUINT32 *)((base) + 0x0204)) /* Time Base Ref 0 */
#define TBREFF1(base)	(CAST(VUINT32 *)((base) + 0x0208)) /* Time Base Ref 1 */

#define RTCSC(base)	(CAST(VUINT16 *)((base) + 0x0220)) /* Clock Stat Ctrl*/
#define RTC(base)	(CAST(VUINT32 *)((base) + 0x0224)) /* RT Clock */
#define RTSEC(base)	(CAST(VUINT32 *)((base) + 0x0228)) /* RT Alarm Seconds*/
#define RTCAL(base)	(CAST(VUINT32 *)((base) + 0x022C)) /* Real Time Alarm */

#define PISCR(base)	(CAST(VUINT16 *)((base) + 0x0240)) /* PIT Status Ctrl */
#define PITC(base)	(CAST(VUINT32 *)((base) + 0x0244)) /* PIT Count */
#define PITR(base)	(CAST(VUINT32 *)((base) + 0x0248)) /* PIT */

/* Clock and Reset */

#define SCCR(base)	(CAST(VUINT32 *)((base) + 0x0280)) /* Sys Clock Ctrl*/
#define PLPRCR(base)	(CAST(VUINT32 *)((base) + 0x0284)) /* PLL LPower Reset*/
#define RSR(base)	(CAST(VUINT32 *)((base) + 0x0288)) /* Reset Status Reg*/

/* System Integration Timers Keys */

#define TBSCRK(base)	(CAST(VUINT32 *)((base) + 0x0300)) /* TB Stat Ctrl key*/
#define TBREFF0K(base)	(CAST(VUINT32 *)((base) + 0x0304)) /* TB Ref 0 Key */
#define TBREFF1K(base)	(CAST(VUINT32 *)((base) + 0x0308)) /* TB Ref 1 Key */
#define TBK(base)	(CAST(VUINT32 *)((base) + 0x030C)) /* TB & Dec Key */

#define RTCSCK(base)	(CAST(VUINT32 *)((base) + 0x0320)) /* RT Stat Ctrl Key*/
#define RTCK(base)	(CAST(VUINT32 *)((base) + 0x0324)) /* RT Clock Key */
#define RTSECK(base)	(CAST(VUINT32 *)((base) + 0x0328)) /* RT Alarm Second */
#define RTCALK(base)	(CAST(VUINT32 *)((base) + 0x032C)) /* R T Alarm Key */

#define PISCRK(base)	(CAST(VUINT32 *)((base) + 0x0340)) /* PIT Stat Ctrl Key*/
#define PITCK(base)	(CAST(VUINT32 *)((base) + 0x0344)) /* PIT Count Key */

/* Clock and Reset Keys */

#define SCCRK(base)	(CAST(VUINT32 *)((base) + 0x0380)) /* System Clk Ctrl */
#define PLPRCRK(base)	(CAST(VUINT32 *)((base) + 0x0384)) /* Pll, LP&R Ctrl */
#define RSRK(base)	(CAST(VUINT32 *)((base) + 0x0388)) /* Reset Status Key*/

/* LCD */

#define LCOLR(base)	(CAST(VUINT16 *)((base) + 0x0E00)) /* Color Ram */
#define LCCR(base)	(CAST(VUINT32 *)((base) + 0x0840)) /* Config reg */
#define LCHCR(base)	(CAST(VUINT32 *)((base) + 0x0844)) /* Horizontal */
#define LCVCR(base)	(CAST(VUINT32 *)((base) + 0x0848)) /* Vertical */
#define LCFAA(base)	(CAST(VUINT32 *)((base) + 0x0850)) /* Frame A buf */
#define LCFBA(base)	(CAST(VUINT32 *)((base) + 0x0854)) /* Frame B buf */
#define LCSR(base)	(CAST(VUINT8 *) ((base) + 0x0858)) /* Status reg */

/* I2C */

#define I2MOD(base)	(CAST(VUINT8 *)((base) + 0x0860)) /* I2C Mode register*/
#define I2ADD(base)	(CAST(VUINT8 *)((base) + 0x0864)) /* I2C Address reg */
#define I2BRG(base)	(CAST(VUINT8 *)((base) + 0x0868)) /* I2C BRG register */
#define I2COM(base)	(CAST(VUINT8 *)((base) + 0x086C)) /* I2C Command reg */
#define I2CER(base)	(CAST(VUINT8 *)((base) + 0x0870)) /* I2C Event Reg */
#define I2CMR(base)	(CAST(VUINT8 *)((base) + 0x0874)) /* I2C Mask Reg */

/* DMA */

#define SDAR(base)	(CAST(VUINT32 *)((base) + 0x0904)) /* SDMA Address Reg*/
#define SDSR(base)	(CAST(VUINT8 *)((base) + 0x0908)) /* SDMA Status Reg */
#define SDMR(base)	(CAST(VUINT8 *)((base) + 0x090C)) /* SDMA Mask Reg */
#define IDSR1(base)	(CAST(VUINT8 *)((base) + 0x0910)) /* IDMA 1 Status Reg*/
#define IDMR1(base)	(CAST(VUINT8 *)((base) + 0x0914)) /* IDMA 1 Mask Reg */
#define IDSR2(base)	(CAST(VUINT8 *)((base) + 0x0918)) /* IDMA 2 Status Reg*/
#define IDMR2(base)	(CAST(VUINT8 *)((base) + 0x091C)) /* IDMA 2 Mask Reg */

/* CPM Interrupt Control registers */

#define	CIVR(base)	(CAST(VUINT16 *)((base) + 0x0930)) /* CP Int vector */
#define	CICR(base)	(CAST(VUINT32 *)((base) + 0x0940)) /* CP Int config */
#define	CIPR(base)	(CAST(VUINT32 *)((base) + 0x0944)) /* CP Int pending */
#define	CIMR(base)	(CAST(VUINT32 *)((base) + 0x0948)) /* CP Int mask reg */
#define	CISR(base)	(CAST(VUINT32 *)((base) + 0x094C)) /* CP Int in-serv*/

/* Input/Output Port register */

#define	PADIR(base)	(CAST(VUINT16 *)((base) + 0x0950)) /* PA data dir*/
#define	PAPAR(base)	(CAST(VUINT16 *)((base) + 0x0952)) /* PA pin assign*/
#define	PAODR(base)	(CAST(VUINT16 *)((base) + 0x0954)) /* PA open drain*/
#define	PADAT(base)	(CAST(VUINT16 *)((base) + 0x0956)) /* PA data reg */
#define	PCDIR(base)	(CAST(VUINT16 *)((base) + 0x0960)) /* PC data dir */
#define	PCPAR(base)	(CAST(VUINT16 *)((base) + 0x0962)) /* PC pin assign*/
#define	PCSO(base)	(CAST(VUINT16 *)((base) + 0x0964)) /* PC options */
#define	PCDAT(base)	(CAST(VUINT16 *)((base) + 0x0966)) /* PC data reg */
#define	PCINT(base)	(CAST(VUINT16 *)((base) + 0x0968)) /* PC intr ctrl */
#define	PDDIR(base)	(CAST(VUINT16 *)((base) + 0x0970)) /* PD data dir */
#define	PDPAR(base)	(CAST(VUINT16 *)((base) + 0x0972)) /* PD pin assign*/
#define	PDDAT(base)	(CAST(VUINT16 *)((base) + 0x0976)) /* PD data reg */

/* CPM timer register set */

#define TGCR(base)	(CAST(VUINT16 *)((base) + 0x0980)) /* Tmr Global Cfg */
#define TMR1(base)	(CAST(VUINT16 *)((base) + 0x0990)) /* Tmr 1 Mode Reg */
#define TMR2(base)	(CAST(VUINT16 *)((base) + 0x0992)) /* Tmr 2 Mode Reg */
#define TRR1(base)	(CAST(VUINT16 *)((base) + 0x0994)) /* Tmr 1 Ref Reg */
#define TRR2(base)	(CAST(VUINT16 *)((base) + 0x0996)) /* Tmr 2 Ref Reg */
#define TCR1(base)	(CAST(VUINT16 *)((base) + 0x0998)) /* Tmr 1 Capture */
#define TCR2(base)	(CAST(VUINT16 *)((base) + 0x099A)) /* Tmr 2 Capture */
#define TCN1(base)	(CAST(VUINT16 *)((base) + 0x099C)) /* Tmr 1 Counter */
#define TCN2(base)	(CAST(VUINT16 *)((base) + 0x099E)) /* Tmr 2 Counter */

#define TMR3(base)	(CAST(VUINT16 *)((base) + 0x09A0)) /* Tmr 3 Mode Reg */
#define TMR4(base)	(CAST(VUINT16 *)((base) + 0x09A2)) /* Tmr 4 Mode Reg */
#define TRR3(base)	(CAST(VUINT16 *)((base) + 0x09A4)) /* Tmr 3 Reference*/
#define TRR4(base)	(CAST(VUINT16 *)((base) + 0x09A6)) /* Tmr 4 Reference*/
#define TCR3(base)	(CAST(VUINT16 *)((base) + 0x09A8)) /* Tmr 3 Capture */
#define TCR4(base)	(CAST(VUINT16 *)((base) + 0x09AA)) /* Tmr 4 Capture */
#define TCN3(base)	(CAST(VUINT16 *)((base) + 0x09AC)) /* Tmr 3 Counter */
#define TCN4(base)	(CAST(VUINT16 *)((base) + 0x09AE)) /* Tmr 4 Counter */

#define TER1(base)	(CAST(VUINT16 *)((base) + 0x09B0)) /* Tmr 1 Event */
#define TER2(base)	(CAST(VUINT16 *)((base) + 0x09B2)) /* Tmr 2 Event */
#define TER3(base)	(CAST(VUINT16 *)((base) + 0x09B4)) /* Tmr 3 Event */
#define TER4(base)	(CAST(VUINT16 *)((base) + 0x09B6)) /* Tmr 4 Event */

/* Communication Processor register set */

#define	CPCR(base)	(CAST(VUINT16 *)((base) + 0x09C0)) /* Com Proc Cmd*/
#define	RCCR(base)	(CAST(VUINT16 *)((base) + 0x09C4)) /* RISC Config Reg */
#define	RMDS(base)	(CAST(VUINT8 *) ((base) + 0x09C7)) /* RISC Dev Sup stat*/
#define	RMDR(base)	(CAST(VUINT32 *)((base) + 0x09C8)) /* Mcode Dev ctrl */
#define	RCTR1(base)	(CAST(VUINT16 *)((base) + 0x09CC)) /* RISC Ctrl Trap 1*/
#define	RCTR2(base)	(CAST(VUINT16 *)((base) + 0x09CE)) /* RISC Ctrl Trap 2*/
#define	RCTR3(base)	(CAST(VUINT16 *)((base) + 0x09D0)) /* RISC Ctrl Trap 3*/
#define	RCTR4(base)	(CAST(VUINT16 *)((base) + 0x09D2)) /* RISC Ctrl Trap 4*/
#define	RTER(base)	(CAST(VUINT16 *)((base) + 0x09D6)) /* RISC Tmr Event*/
#define	RTMR(base)	(CAST(VUINT16 *)((base) + 0x09DA)) /* RISC Tmr Mask */

/* Baud Rate Generation Register set */

#define	BRGC1(base)	(CAST(VUINT32 *)((base) + 0x09F0)) /* BRG1 Config Reg */
#define	BRGC2(base)	(CAST(VUINT32 *)((base) + 0x09F4)) /* BRG2 Config Reg */
#define	BRGC3(base)	(CAST(VUINT32 *)((base) + 0x09F8)) /* BRG3 Config Reg */
#define	BRGC4(base)	(CAST(VUINT32 *)((base) + 0x09FC)) /* BRG4 Config Reg */

/* SCC 1 register set */

#define GSMR_L1(base)	(CAST(VUINT32 *)((base) + 0x0A00)) /* SCC1 Gen Mode*/
#define GSMR_H1(base)	(CAST(VUINT32 *)((base) + 0x0A04)) /* SCC1 Gen Mode*/
#define PSMR1(base)	(CAST(VUINT16 *)((base) + 0x0A08)) /* SCC1 Proto Spec */
#define TODR1(base)	(CAST(VUINT16 *)((base) + 0x0A0C)) /* SCC1 Tx Demand*/
#define DSR1(base)	(CAST(VUINT16 *)((base) + 0x0A0E)) /* SCC1 Data Sync */
#define SCCE1(base)	(CAST(VUINT16 *)((base) + 0x0A10)) /* SCC1 Event Reg */
#define SCCM1(base)	(CAST(VUINT16 *)((base) + 0x0A14)) /* SCC1 Mask Reg */
#define SCCS1(base)	(CAST(VUINT8 *) ((base) + 0x0A17)) /* SCC1 Status Reg */

/* SCC 2 register set */

#define GSMR_L2(base)	(CAST(VUINT32 *)((base) + 0x0A20)) /* SCC2 Gen Mode*/
#define GSMR_H2(base)	(CAST(VUINT32 *)((base) + 0x0A24)) /* SCC2 Gen Mode*/
#define PSMR2(base)	(CAST(VUINT16 *)((base) + 0x0A28)) /* SCC2 Proto Spec */
#define TODR2(base)	(CAST(VUINT16 *)((base) + 0x0A2C)) /* SCC2 Tx Demand*/
#define DSR2(base)	(CAST(VUINT16 *)((base) + 0x0A2E)) /* SCC2 Data Sync */
#define SCCE2(base)	(CAST(VUINT16 *)((base) + 0x0A30)) /* SCC2 Event Reg */
#define SCCM2(base)	(CAST(VUINT16 *)((base) + 0x0A34)) /* SCC2 Mask Reg */
#define SCCS2(base)	(CAST(VUINT8 *)	((base) + 0x0A37)) /* SCC2 Status Reg */

/* SCC 3 register set */

#define GSMR_L3(base)	(CAST(VUINT32 *)((base) + 0x0A40)) /* SCC3 Gen Mode*/
#define GSMR_H3(base)	(CAST(VUINT32 *)((base) + 0x0A44)) /* SCC3 Gen Mode*/
#define PSMR3(base)	(CAST(VUINT16 *)((base) + 0x0A48)) /* SCC3 Proto Spec */
#define TODR3(base)	(CAST(VUINT16 *)((base) + 0x0A4C)) /* SCC3 Tx Demand*/
#define DSR3(base)	(CAST(VUINT16 *)((base) + 0x0A4E)) /* SCC3 Data Sync */
#define SCCE3(base)	(CAST(VUINT16 *)((base) + 0x0A50)) /* SCC3 Event Reg */
#define SCCM3(base)	(CAST(VUINT16 *)((base) + 0x0A54)) /* SCC3 Mask Reg */
#define SCCS3(base)	(CAST(VUINT8 *) ((base) + 0x0A57)) /* SCC3 Status Reg */

/* SCC 4 register set */

#define GSMR_L4(base)	(CAST(VUINT32 *)((base) + 0x0A60)) /* SCC4 Gen Mode*/
#define GSMR_H4(base)	(CAST(VUINT32 *)((base) + 0x0A64)) /* SCC4 Gen Mode*/
#define PSMR4(base)	(CAST(VUINT16 *)((base) + 0x0A68)) /* SCC4 Proto Spec */
#define TODR4(base)	(CAST(VUINT16 *)((base) + 0x0A6C)) /* SCC4 Tx Demand*/
#define DSR4(base)	(CAST(VUINT16 *)((base) + 0x0A6E)) /* SCC4 Data Sync */
#define SCCE4(base)	(CAST(VUINT16 *)((base) + 0x0A70)) /* SCC4 Event Reg */
#define SCCM4(base)	(CAST(VUINT16 *)((base) + 0x0A74)) /* SCC2 Mask Reg */
#define SCCS4(base)	(CAST(VUINT8 *) ((base) + 0x0A77)) /* SCC4 Status Reg */

/* SMC 1 register set */

#define	SMCMR1(base)	(CAST(VUINT16 *)((base) + 0x0A82)) /* SMC1 Mode Reg */
#define	SMCE1(base)	(CAST(VUINT8 *) ((base) + 0x0A86)) /* SMC1 Event Reg */
#define	SMCM1(base)	(CAST(VUINT8 *) ((base) + 0x0A8A)) /* SMC1 Mask Reg */

/* SMC 2 register set */

#define	SMCMR2(base)	(CAST(VUINT16 *)((base) + 0x0A92)) /* SMC2 Mode Reg */
#define	SMCE2(base)	(CAST(VUINT8 *) ((base) + 0x0A96)) /* SMC2/PIP Event*/
#define	SMCM2(base)	(CAST(VUINT8 *) ((base) + 0x0A9A)) /* SMC2 Mask Reg */

/* SPI regiter set */

#define	SPMODE(base)	(CAST(VUINT16 *)((base) + 0x0AA0)) /* SPI Mode Reg */
#define	SPIE(base)	(CAST(VUINT8 *) ((base) + 0x0AA6)) /* SPI Event Reg */
#define	SPIM(base)	(CAST(VUINT8 *) ((base) + 0x0AAA)) /* SPI Mask Reg */
#define	SPCOM(base)	(CAST(VUINT8 *) ((base) + 0x0AAD)) /* SPI Command Reg */

/* PIP register set */

#define	PIPC(base)	(CAST(VUINT16 *)((base) + 0x0AB2)) /* PIP Config Reg */
#define	PTPR(base)	(CAST(VUINT16 *)((base) + 0x0AB6)) /* PIP Timing parms*/
#define	PBDIR(base)	(CAST(VUINT32 *)((base) + 0x0AB8)) /* PB data dir */
#define	PBPAR(base)	(CAST(VUINT32 *)((base) + 0x0ABC)) /* PB pin ass*/
#define	PBODR(base)	(CAST(VUINT16 *)((base) + 0x0AC2)) /* PB open drain*/
#define	PBDAT(base)	(CAST(VUINT32 *)((base) + 0x0AC4)) /* PB Data Reg*/

/* SI register set */

#define	SIMODE(base)	(CAST(VUINT32 *)((base) + 0x0AE0)) /* SI Mode Reg */
#define	SIGMR(base)	(CAST(VUINT8 *) ((base) + 0x0AE4)) /* SI Global Mode */
#define	SISTR(base)	(CAST(VUINT8 *) ((base) + 0x0AE6)) /* SI Status Reg */
#define	SICMR(base)	(CAST(VUINT8 *) ((base) + 0x0AE7)) /* SI Command Reg */
#define	SICR(base)	(CAST(VUINT32 *)((base) + 0x0AEC)) /* SI Clock route */
#define	SIRP(base)	(CAST(VUINT32 *) ((base) + 0x0AF0)) /* SI RAM pointers*/
#define	SIRAM(base)	(CAST(VUINT32 *)((base) + 0x0C00)) /* SI Routing RAM */

#define MPC860_REGB_OFFSET      0x2000  /* offset to internal registers */
 
/* Data Param RAM */

#define       DPRAM(base)     (CAST(VUINT32 *) ((base) + MPC860_REGB_OFFSET))

/* MPC860 Dual Ported Ram addresses */
 
#define MPC860_DPR_SCC1(base)  (CAST(VUINT32 *) ((base) + MPC860_REGB_OFFSET \
                                                 + 0x1c00))

/* SIU Module Configuration register bit definition (SIUMCR - 0x00) */

#define SIUMCR_EARB		0x80000000	/* External Abritation */
#define SIUMCR_EARP		0x70000000	/* Extern Abri. Request prior.*/
#define SIUMCR_DSHW		0x00800000	/* Data Showcycles */
#define SIUMCR_DBGC		0x00600000	/* Debug pins configuration */
#define SIUMCR_DBPC		0x00180000	/* Debug Port pins Config. */
#define SIUMCR_FRC		0x00020000	/* FRZ pin Configuration */
#define SIUMCR_DLK		0x00010000	/* Debug Register Lock */
#define SIUMCR_PNCS		0x00008000	/* Parity Non-mem Crtl reg */
#define SIUMCR_OPAR		0x00004000	/* Odd Parity */
#define SIUMCR_DPC		0x00002000	/* Data Parity pins Config. */
#define SIUMCR_MPRE		0x00001000	/* Multi CPU Reserva. Enable */
#define SIUMCR_MLRC_IRQ4	0x00000000	/* Multi Level Reserva. Ctrl */
#define SIUMCR_MLRC_3STATES	0x00000400	/* Multi Level Reserva. Ctrl */
#define SIUMCR_MLRC_KR		0x00000800	/* Multi Level Reserva. Ctrl */
#define SIUMCR_MLRC_SPKROUT	0x00000c00	/* Multi Level Reserva. Ctrl */
#define SIUMCR_AEME		0x00000200	/* Asynchro External Master */
#define SIUMCR_SEME		0x00000100	/* Synchro External Master */
#define SIUMCR_BSC		0x00000080	/* Byte Select Configuration */
#define SIUMCR_GB5E		0x00000040	/* GPL_B(5) Enable */
#define SIUMCR_B2DD		0x00000020	/* Bank 2 Double Drive */
#define SIUMCR_B3DD		0x00000010	/* Bank 3 Double Drive */

/* System Portection Control register bit definition (SYPCR - 0x04) */

#define SYPCR_SWTC	0xffff0000	/* Software Watchdog Timer Count */
#define SYPCR_BMT	0x0000ff00	/* Bus Monitor Timing */
#define SYPCR_BME	0x00000080	/* Bus Monitor Enable */
#define SYPCR_SWF	0x00000008	/* Software Watchdog Freeze */
#define SYPCR_SWE	0x00000004	/* Software Watchdog Enable */
#define SYPCR_SWRI	0x00000002	/* Software Watchdog Reset/Int Select */
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

/* System Interrupt MASK register bit definition (SIMARK - 0x14) */

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
#define TESR_IPB	0x00000f00	/* Instr. Parity Error on Byte */
#define TESR_DEXT	0x00000020	/* Data External Transfer Error Ack */
#define TESR_DBM	0x00000010	/* Data transfer Monitor Time-Out */
#define TESR_DPB	0x0000000f	/* Data Parity Error on Byte */

/* Memory STATus register bit definition (MSTAT - 0x178) */

#define MSTAT_PER0	0x8000		/* Parity Error bank 0 */
#define MSTAT_PER1	0x4000		/* Parity Error bank 1 */
#define MSTAT_PER2	0x2000		/* Parity Error bank 2 */
#define MSTAT_PER3	0x1000		/* Parity Error bank 3 */
#define MSTAT_PER4	0x0800		/* Parity Error bank 4 */
#define MSTAT_PER5	0x0400		/* Parity Error bank 5 */
#define MSTAT_PER6	0x0200		/* Parity Error bank 6 */
#define MSTAT_PER7	0x0100		/* Parity Error bank 7 */
#define MSTAT_WPER	0x0080		/* Write Protection Error */

/* Memory Periodic Timer Prescaler Register bit definition (MPTPR - 0x17A) */

#define MPTPR_PTP_MSK	0xff00		/* Periodic Timers Prescaler Mask */
#define MPTPR_PTP_DIV2	0x2000		/* BRGCLK divided by 2 */
#define MPTPR_PTP_DIV4	0x1000		/* BRGCLK divided by 4 */
#define MPTPR_PTP_DIV8	0x0800		/* BRGCLK divided by 8 */
#define MPTPR_PTP_DIV16	0x0400		/* BRGCLK divided by 16 */
#define MPTPR_PTP_DIV32	0x0200		/* BRGCLK divided by 32 */
#define MPTPR_PTP_DIV64	0x0100		/* BRGCLK divided by 64 */

/* Base Register bit definition (BRx - 0x100) */

#define BR_BA_MSK	0xffff8000	/* Base Address Mask */
#define BR_AT_MSK	0x00007000	/* Address Type Mask */
#define BR_PS_MSK	0x00000c00	/* Port Size Mask */
#define BR_PARE		0x00000200	/* Parity Enable */
#define BR_WP		0x00000100	/* Write Protect */
#define BR_MS_MSK	0x000000c0	/* Machine Select Mask */
#define BR_MS_GPCM	0x00000000	/* G.P.C.M. Machine Select */
#define BR_MS_UPMA	0x00000080	/* U.P.M.A Machine Select */
#define BR_MS_UPMB	0x000000c0	/* U.P.M.B Machine Select */
#define BR_V		0x00000001	/* Bank Valid */
#define BR_PS_8		0x00000400	/* 8 bit port size */
#define BR_PS_16	0x00000800	/* 16 bit port size */
#define BR_PS_32	0x00000000	/* 32 bit port size */

/* Option Register bit definition (ORx - 0x104) */

#define OR_AM_MSK	0xffff8000	/* Address Mask Mask */
#define OR_ATM_MSK	0x00007000	/* Address Type Mask Mask */
#define OR_CSNT_SAM	0x00000800	/* Chip Select Negation Time/ Start */
					/* Address Multiplex */
#define OR_ACS_MSK	0x00000600	/* Address to Chip Select Setup mask */
#define OR_ACS_DIV1	0x00000000	/* CS is output at the same time */
#define OR_ACS_DIV4	0x00000400	/* CS is output 1/4 a clock later */
#define OR_ACS_DIV2	0x00000600	/* CS is output 1/2 a clock later */
#define OR_BI		0x00000100	/* Burst inhibit */
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
#define OR_SETA		0x00000008	/* External Transfer Acknowledge */
#define OR_TRLX		0x00000004	/* Timing Relaxed */

/* Memory Command Register bit definition (MCR - 0x168) */

#define MCR_OP_WRITE	0x00000000	/* Command Opcode: Write */
#define MCR_OP_READ	0x40000000	/* Command Opcode: Read */
#define MCR_OP_RUN	0x80000000	/* Command Opcode: Run */
#define MCR_UM_MSK	0x00800000	/* User Machine mask */
#define MCR_UM_UPMA	0x00000000	/* User Machine = UPMA */
#define MCR_UM_UPMB	0x00800000	/* User Machine = UPMB */
#define MCR_MB_MSK	0x0000e000	/* Memory Bank mask */
#define MCR_MB_CS0	0x00000000	/* CS0 enabled */
#define MCR_MB_CS1	0x00002000	/* CS1 enabled */
#define MCR_MB_CS2	0x00004000	/* CS3 enabled */
#define MCR_MB_CS3	0x00006000	/* CS3 enabled */
#define MCR_MB_CS4	0x00008000	/* CS4 enabled */
#define MCR_MB_CS5	0x0000a000	/* CS5 enabled */
#define MCR_MB_CS6	0x0000c000	/* CS6 enabled */
#define MCR_MB_CS7	0x0000e000	/* CS7 enabled */
#define MCR_MCLF_MSK	0x00000f00	/* Memory Command Loop Field mask */
#define MCR_MCLF_1X	0x00000100	/* The Cmd Loop is executed 1 time */
#define MCR_MCLF_2X	0x00000200	/* The Cmd Loop is executed 2 times */
#define MCR_MCLF_3X	0x00000300	/* The Cmd Loop is executed 3 times */
#define MCR_MCLF_4X	0x00000400	/* The Cmd Loop is executed 4 times */
#define MCR_MCLF_5X	0x00000500	/* The Cmd Loop is executed 5 times */
#define MCR_MCLF_6X	0x00000600	/* The Cmd Loop is executed 6 times */
#define MCR_MCLF_7X	0x00000700	/* The Cmd Loop is executed 7 times */
#define MCR_MCLF_8X	0x00000800	/* The Cmd Loop is executed 8 times */
#define MCR_MCLF_9X	0x00000900	/* The Cmd Loop is executed 9 times */
#define MCR_MCLF_10X	0x00000a00	/* The Cmd Loop is executed 10 times */
#define MCR_MCLF_11X	0x00000b00	/* The Cmd Loop is executed 11 times */
#define MCR_MCLF_12X	0x00000c00	/* The Cmd Loop is executed 12 times */
#define MCR_MCLF_13X	0x00000d00	/* The Cmd Loop is executed 13 times */
#define MCR_MCLF_14X	0x00000e00	/* The Cmd Loop is executed 14 times */
#define MCR_MCLF_15X	0x00000f00	/* The Cmd Loop is executed 15 times */
#define MCR_MCLF_16X	0x00000000	/* The Cmd Loop is executed 16 times */
#define MCR_MAD_MSK	0x0000003f	/* Machine Address mask */

/* Machine A Mode Register bit definition (MAMR - 0x170) */

#define MAMR_PTA_MSK	0xff000000	/* Periodic Timer A period mask */
#define MAMR_PTA_SHIFT	0x00000018	/* Periodic Timer A period shift */
#define MAMR_PTAE	0x00800000	/* Periodic Timer A Enable */
#define MAMR_AMA_MSK	0x00700000	/* Addess Multiplexing size A */
#define MAMR_AMA_TYPE_0	0x00000000	/* Addess Multiplexing Type 0 */
#define MAMR_AMA_TYPE_1	0x00100000	/* Addess Multiplexing Type 1 */
#define MAMR_AMA_TYPE_2	0x00200000	/* Addess Multiplexing Type 2 */
#define MAMR_AMA_TYPE_3	0x00300000	/* Addess Multiplexing Type 3 */
#define MAMR_AMA_TYPE_4	0x00400000	/* Addess Multiplexing Type 4 */
#define MAMR_AMA_TYPE_5	0x00500000	/* Addess Multiplexing Type 5 */
#define MAMR_DSA_MSK	0x00060000	/* Disable Timer period mask */
#define MAMR_DSA_1_CYCL	0x00000000	/* 1 cycle Disable Period */
#define MAMR_DSA_2_CYCL	0x00020000	/* 2 cycle Disable Period */
#define MAMR_DSA_3_CYCL	0x00040000	/* 3 cycle Disable Period */
#define MAMR_DSA_4_CYCL	0x00060000	/* 4 cycle Disable Period */
#define MAMR_G0CLA_MSK	0x0000e000	/* General Line 0 Control A */
#define MAMR_G0CLA_A12	0x00000000	/* General Line 0 : A12 */
#define MAMR_G0CLA_A11	0x00002000	/* General Line 0 : A11 */
#define MAMR_G0CLA_A10	0x00004000	/* General Line 0 : A10 */
#define MAMR_G0CLA_A9	0x00006000	/* General Line 0 : A9 */
#define MAMR_G0CLA_A8	0x00008000	/* General Line 0 : A8 */
#define MAMR_G0CLA_A7	0x0000a000	/* General Line 0 : A7 */
#define MAMR_G0CLA_A6	0x0000b000	/* General Line 0 : A6 */
#define MAMR_G0CLA_A5	0x0000e000	/* General Line 0 : A5 */
#define MAMR_GPL_A4DIS	0x00001000	/* GPL_A4 ouput line Disable */
#define MAMR_RLFA_MSK	0x00000f00	/* Read Loop Field A mask */
#define MAMR_RLFA_1X	0x00000100	/* The Read Loop is executed 1 time */
#define MAMR_RLFA_2X	0x00000200	/* The Read Loop is executed 2 times */
#define MAMR_RLFA_3X	0x00000300	/* The Read Loop is executed 3 times */
#define MAMR_RLFA_4X	0x00000400	/* The Read Loop is executed 4 times */
#define MAMR_RLFA_5X	0x00000500	/* The Read Loop is executed 5 times */
#define MAMR_RLFA_6X	0x00000600	/* The Read Loop is executed 6 times */
#define MAMR_RLFA_7X	0x00000700	/* The Read Loop is executed 7 times */
#define MAMR_RLFA_8X	0x00000800	/* The Read Loop is executed 8 times */
#define MAMR_RLFA_9X	0x00000900	/* The Read Loop is executed 9 times */
#define MAMR_RLFA_10X	0x00000a00	/* The Read Loop is executed 10 times */
#define MAMR_RLFA_11X	0x00000b00	/* The Read Loop is executed 11 times */
#define MAMR_RLFA_12X	0x00000c00	/* The Read Loop is executed 12 times */
#define MAMR_RLFA_13X	0x00000d00	/* The Read Loop is executed 13 times */
#define MAMR_RLFA_14X	0x00000e00	/* The Read Loop is executed 14 times */
#define MAMR_RLFA_15X	0x00000f00	/* The Read Loop is executed 15 times */
#define MAMR_RLFA_16X	0x00000000	/* The Read Loop is executed 16 times */
#define MAMR_WLFA_MSK	0x000000f0	/* Write Loop Field A mask */
#define MAMR_WLFA_1X	0x00000010	/* The Write Loop is executed 1 time */
#define MAMR_WLFA_2X	0x00000020	/* The Write Loop is executed 2 times */
#define MAMR_WLFA_3X	0x00000030	/* The Write Loop is executed 3 times */
#define MAMR_WLFA_4X	0x00000040	/* The Write Loop is executed 4 times */
#define MAMR_WLFA_5X	0x00000050	/* The Write Loop is executed 5 times */
#define MAMR_WLFA_6X	0x00000060	/* The Write Loop is executed 6 times */
#define MAMR_WLFA_7X	0x00000070	/* The Write Loop is executed 7 times */
#define MAMR_WLFA_8X	0x00000080	/* The Write Loop is executed 8 times */
#define MAMR_WLFA_9X	0x00000090	/* The Write Loop is executed 9 times */
#define MAMR_WLFA_10X	0x000000a0	/* The Write Loop is executed 10 times*/
#define MAMR_WLFA_11X	0x000000b0	/* The Write Loop is executed 11 times*/
#define MAMR_WLFA_12X	0x000000c0	/* The Write Loop is executed 12 times*/
#define MAMR_WLFA_13X	0x000000d0	/* The Write Loop is executed 13 times*/
#define MAMR_WLFA_14X	0x000000e0	/* The Write Loop is executed 14 times*/
#define MAMR_WLFA_15X	0x000000f0	/* The Write Loop is executed 15 times*/
#define MAMR_WLFA_16X	0x00000000	/* The Write Loop is executed 16 times*/
#define MAMR_TLFA_MSK	0x0000000f	/* Timer Loop Field A mask */
#define MAMR_TLFA_1X	0x00000001	/* The Timer Loop is executed 1 time */
#define MAMR_TLFA_2X	0x00000002	/* The Timer Loop is executed 2 times */
#define MAMR_TLFA_3X	0x00000003	/* The Timer Loop is executed 3 times */
#define MAMR_TLFA_4X	0x00000004	/* The Timer Loop is executed 4 times */
#define MAMR_TLFA_5X	0x00000005	/* The Timer Loop is executed 5 times */
#define MAMR_TLFA_6X	0x00000006	/* The Timer Loop is executed 6 times */
#define MAMR_TLFA_7X	0x00000007	/* The Timer Loop is executed 7 times */
#define MAMR_TLFA_8X	0x00000008	/* The Timer Loop is executed 8 times */
#define MAMR_TLFA_9X	0x00000009	/* The Timer Loop is executed 9 times */
#define MAMR_TLFA_10X	0x0000000a	/* The Timer Loop is executed 10 times*/
#define MAMR_TLFA_11X	0x0000000b	/* The Timer Loop is executed 11 times*/
#define MAMR_TLFA_12X	0x0000000c	/* The Timer Loop is executed 12 times*/
#define MAMR_TLFA_13X	0x0000000d	/* The Timer Loop is executed 13 times*/
#define MAMR_TLFA_14X	0x0000000e	/* The Timer Loop is executed 14 times*/
#define MAMR_TLFA_15X	0x0000000f	/* The Timer Loop is executed 15 times*/
#define MAMR_TLFA_16X	0x00000000	/* The Timer Loop is executed 16 times*/

/* Machine B Mode Register bit definition (MBMR - 0x174) */

#define MAMR_PTB_MSK	0xff000000	/* Periodic Timer B period mask */
#define MAMR_PTBE	0x00800000	/* Periodic Timer B Enable */
#define MAMR_AMB_MSK	0x00700000	/* Addess Multiplex size B */
#define MAMR_DSB_MSK	0x00060000	/* Disable Timer period mask */
#define MAMR_DSB_1_CYCL	0x00000000	/* 1 cycle Disable Period */
#define MAMR_DSB_2_CYCL	0x00020000	/* 2 cycle Disable Period */
#define MAMR_DSB_3_CYCL	0x00040000	/* 3 cycle Disable Period */
#define MAMR_DSB_4_CYCL	0x00060000	/* 4 cycle Disable Period */
#define MAMR_G0CLB_MSK	0x0000e000	/* General Line 0 Control B */
#define MAMR_G0CLB_A12	0x00000000	/* General Line 0 : A12 */
#define MAMR_G0CLB_A11	0x00002000	/* General Line 0 : A11 */
#define MAMR_G0CLB_A10	0x00004000	/* General Line 0 : A10 */
#define MAMR_G0CLB_A9	0x00006000	/* General Line 0 : A9 */
#define MAMR_G0CLB_A8	0x00008000	/* General Line 0 : A8 */
#define MAMR_G0CLB_A7	0x0000a000	/* General Line 0 : A7 */
#define MAMR_G0CLB_A6	0x0000b000	/* General Line 0 : A6 */
#define MAMR_G0CLB_A5	0x0000e000	/* General Line 0 : A5 */
#define MAMR_GPL_B4DIS	0x00001000	/* GPL_B4 ouput line Disable */
#define MAMR_RLFB_MSK	0x00000f00	/* Read Loop Field B mask */
#define MAMR_RLFB_1X	0x00000100	/* The Read Loop is executed 1 time */
#define MAMR_RLFB_2X	0x00000200	/* The Read Loop is executed 2 times */
#define MAMR_RLFB_3X	0x00000300	/* The Read Loop is executed 3 times */
#define MAMR_RLFB_4X	0x00000400	/* The Read Loop is executed 4 times */
#define MAMR_RLFB_5X	0x00000500	/* The Read Loop is executed 5 times */
#define MAMR_RLFB_6X	0x00000600	/* The Read Loop is executed 6 times */
#define MAMR_RLFB_7X	0x00000700	/* The Read Loop is executed 7 times */
#define MAMR_RLFB_8X	0x00000800	/* The Read Loop is executed 8 times */
#define MAMR_RLFB_9X	0x00000900	/* The Read Loop is executed 9 times */
#define MAMR_RLFB_10X	0x00000a00	/* The Read Loop is executed 10 times */
#define MAMR_RLFB_11X	0x00000b00	/* The Read Loop is executed 11 times */
#define MAMR_RLFB_12X	0x00000c00	/* The Read Loop is executed 12 times */
#define MAMR_RLFB_13X	0x00000d00	/* The Read Loop is executed 13 times */
#define MAMR_RLFB_14X	0x00000e00	/* The Read Loop is executed 14 times */
#define MAMR_RLFB_15X	0x00000f00	/* The Read Loop is executed 15 times */
#define MAMR_RLFB_16X	0x00000000	/* The Read Loop is executed 16 times */
#define MAMR_WLFB_MSK	0x000000f0	/* Write Loop Field B mask */
#define MAMR_WLFB_1X	0x00000010	/* The Write Loop is executed 1 time */
#define MAMR_WLFB_2X	0x00000020	/* The Write Loop is executed 2 times */
#define MAMR_WLFB_3X	0x00000030	/* The Write Loop is executed 3 times */
#define MAMR_WLFB_4X	0x00000040	/* The Write Loop is executed 4 times */
#define MAMR_WLFB_5X	0x00000050	/* The Write Loop is executed 5 times */
#define MAMR_WLFB_6X	0x00000060	/* The Write Loop is executed 6 times */
#define MAMR_WLFB_7X	0x00000070	/* The Write Loop is executed 7 times */
#define MAMR_WLFB_8X	0x00000080	/* The Write Loop is executed 8 times */
#define MAMR_WLFB_9X	0x00000090	/* The Write Loop is executed 9 times */
#define MAMR_WLFB_10X	0x000000a0	/* The Write Loop is executed 10 times*/
#define MAMR_WLFB_11X	0x000000b0	/* The Write Loop is executed 11 times*/
#define MAMR_WLFB_12X	0x000000c0	/* The Write Loop is executed 12 times*/
#define MAMR_WLFB_13X	0x000000d0	/* The Write Loop is executed 13 times*/
#define MAMR_WLFB_14X	0x000000e0	/* The Write Loop is executed 14 times*/
#define MAMR_WLFB_15X	0x000000f0	/* The Write Loop is executed 15 times*/
#define MAMR_WLFB_16X	0x00000000	/* The Write Loop is executed 16 times*/
#define MAMR_TLFB_MSK	0x0000000f	/* Timer Loop Field B mask */
#define MAMR_TLFB_1X	0x00000001	/* The Timer Loop is executed 1 time */
#define MAMR_TLFB_2X	0x00000002	/* The Timer Loop is executed 2 times */
#define MAMR_TLFB_3X	0x00000003	/* The Timer Loop is executed 3 times */
#define MAMR_TLFB_4X	0x00000004	/* The Timer Loop is executed 4 times */
#define MAMR_TLFB_5X	0x00000005	/* The Timer Loop is executed 5 times */
#define MAMR_TLFB_6X	0x00000006	/* The Timer Loop is executed 6 times */
#define MAMR_TLFB_7X	0x00000007	/* The Timer Loop is executed 7 times */
#define MAMR_TLFB_8X	0x00000008	/* The Timer Loop is executed 8 times */
#define MAMR_TLFB_9X	0x00000009	/* The Timer Loop is executed 9 times */
#define MAMR_TLFB_10X	0x0000000a	/* The Timer Loop is executed 10 times*/
#define MAMR_TLFB_11X	0x0000000b	/* The Timer Loop is executed 11 times*/
#define MAMR_TLFB_12X	0x0000000c	/* The Timer Loop is executed 12 times*/
#define MAMR_TLFB_13X	0x0000000d	/* The Timer Loop is executed 13 times*/
#define MAMR_TLFB_14X	0x0000000e	/* The Timer Loop is executed 14 times*/
#define MAMR_TLFB_15X	0x0000000f	/* The Timer Loop is executed 15 times*/
#define MAMR_TLFB_16X	0x00000000	/* The Timer Loop is executed 16 times*/

/* Time Base Status and Contol register bit definition (TBSCR - 0x200) */

#if	FALSE				/* XXX TPR TO verify */

#define TBSCR_TBIRQ0	0x0100	/* Time Base Interrupt Request 0 */
#define TBSCR_TBIRQ1	0x0200	/* Time Base Interrupt Request 1 */
#define TBSCR_TBIRQ2	0x0400	/* Time Base Interrupt Request 2 */
#define TBSCR_TBIRQ3	0x0800	/* Time Base Interrupt Request 3 */
#define TBSCR_TBIRQ4	0x1000	/* Time Base Interrupt Request 4 */
#define TBSCR_TBIRQ5	0x2000	/* Time Base Interrupt Request 5 */
#define TBSCR_TBIRQ6	0x4000	/* Time Base Interrupt Request 6 */
#define TBSCR_TBIRQ7	0x8000	/* Time Base Interrupt Request 7 */

#endif

#define TBSCR_REFA	0x0080	/* Reference Interrupt Status A */
#define TBSCR_REFB	0x0040	/* Reference Interrupt Status B */
#define TBSCR_REFAE	0x0008	/* Second Interrupt Enable A */
#define TBSCR_REFBE	0x0004	/* Second Interrupt Enable B */
#define TBSCR_TBF	0x0002	/* Time Base Freeze */
#define TBSCR_TBE	0x0001	/* Time Base Enable */

/* Real Time Clock Status and Control register bit definition (RTCSC - 0x220) */

#define RTCSC_RTCIRQ
#define RTCSC_SEC	0x0080		/* Once per Second interrupt */
#define RTCSC_ALR	0x0040		/* Alarm interrupt */
#define RTCSC_38K	0x0010		/* Real Time Clock at 38.4 KHz */
#define RTCSC_SIE	0x0008		/* Second Interrupt Enable */
#define RTCSC_ALE	0x0004		/* Alarm Interrupt Enable */
#define RTCSC_RTF	0x0002		/* Real Time Clock Freeze */
#define RTCSC_RTE	0x0001		/* Real Time Clock Enable */

/* PIT Status and Control Register bit definition (PISCR - 0x0240) */

#define PISCR_PIRQ
#define PISCR_PS	0x0080		/* Periodic interrupt Status */
#define PISCR_PIE	0x0004		/* Periodic Interrupt Enable */
#define PISCR_PITF	0x0002		/* Periodic Interrupt Timer Freeze */
#define PISCR_PTE	0x0001		/* Periodic Timer Enable */

/* System Clock Control Register bit definition (SCCR - 0x280) */

#define SCCR_COM_MSK	0x60000000	/* Clock Output Mode Mask */
#define SCCR_TBS	0x02000000	/* Time Base Source */
#define SCCR_RTDIV	0x01000000	/* RTC Clock Dive */
#define SCCR_RTSEL	0x00800000	/* RTC circuit input source select */
#define SCCR_CRQEN	0x00400000	/* CPM Request Enable */
#define SCCR_PRQEN	0x00200000	/* Power Management Request Enable */
#define SCCR_DFSYNC_MSK	0x00006000	/* Division Factor of SyncCLK Mask */
#define SCCR_DFBRG_MSK	0x00001800	/* Division Factor of BRGCLK Mask */
#define SCCR_DFBRG_SHIFT    0x000c	/* Division Factor of BRGCLK shift */
#define SCCR_DFNL_MSK	0x00000700	/* Division Factor Low Frequency Mask */
#define SCCR_DFNH_MSK	0x000000e0	/* Division Factor High Frequency Mask*/
#define SCCR_DFLCD_MSK	0x0000001c	/* Division Factor of LCD Clock Mask */
#define SCCR_DFALCD_MSK	0x00000002	/* Add. Division Factor of LCD Clock */

/* Pll, Low Power and Reset Control Register bit definition (PLPRCR - 284) */

#define PLPRCR_MF_MSK	0xfff00000	/* Multiplication factor bits */
#define PLPRCR_MF_SHIFT	0x00000014	/* Multiplication factor shift value */
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
#define PLPRCR_FIOPD	0x00000020	/* Force I/O Pull Down */

/* LCD register bit definitions */

#define LCD_ENABLE	0x0010

/* LCD Panel Configuration Register (LCCR)	0x0840 */

#define LCD_IEN		0x00008000	/* LCD interrupt enable */
#define LCD_CLKP	0x00000800	/* LCD Clock Polarity */
#define LCD_OEP		0x00000400	/* LCD Output Enable Polarity */
#define LCD_HSP		0x00000200	/* LCD horizontal sync polarity */
#define LCD_VSP		0x00000100	/* LCD vertical sync polarity */
#define LCD_DP		0x00000080	/* LCD data polarity */
#define LCD_BPIX	0x00000060	/* LCD bits per pixel mask */
#define LCD_BPIX_8	0x00000060	/* eight bits per pixel setting */
#define LCD_BPIX_4	0x00000040	/* four bits per pixel setting */
#define LCD_BPIX_2	0x00000020	/* two bits per pixel setting */
#define LCD_BPIX_1	0x00000000	/* one bit per pixel setting */
#define LCD_LBW		0x00000010	/* LCD Bus width */
#define LCD_LBW_8	0x00000010	/* LCD Bus width 8 */
#define LCD_LBW_4	0x00000000	/* LCD Bus width 4 */
#define LCD_SPLT	0x00000008	/* LCD split display mode */
#define LCD_CLOR	0x00000004	/* LCD color display */
#define LCD_TFT		0x00000002	/* LCD TFT display */
#define LCD_PON		0x00000001	/* Panel On/Off control bit */

#ifdef  __cplusplus
}
#endif

#endif /* __INCppc860Siuh */
