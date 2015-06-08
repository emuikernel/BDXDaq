/* ppc860Cpm.h - Motorola PPC860 Communication Processor Module header file */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,11jan99,cn   Corrected/added defines for CP commands (SPR# 20337),
		 added qualifier volatile where needed (SPR# 24295).
01f,09nov98,cn   Added support for Fast Ethernet Controller.
01e,20apr98,map  fixed SIMODE definitions [SPR# 20157]
01d,13apr98,map  added more SCC definitions.
01c,25mar98,map  added SCC, PIP, I2C, CPCR, definitions.
01b,09may96,dzb  added CPM_CR_... flags for the CPM command register.
01a,24apr96,tpr	 created.
*/

/*
 * This file contains constants of the Communication Processor Unit (SIU) for
 * the Motorola MPC860 PowerPC microcontroller.
 */

#ifndef __INCppc860Cpmh
#define __INCppc860Cpmh

#ifdef __cplusplus
extern "C" {
#endif

    
/* device and channel structures */

/* 
 * The PPC860SMC_CHAN structure defines a serial I/O channel which
 * describes the (TBD) registers for a given channel. 
 * Also the various SIO driver functions ( ioctl, txStarup, callbackInstall,
 * pollInput, polloutput ) that this channel could utilize are declared 
 * here.
 */

#ifdef	_ASMLANGUAGE
#  define SMC_ADRS(reg)	(MPC860SMC1_BASE + (reg * MPC860SMC1_REG_OFFSET))
#else
#  define SMC_ADRS(reg)	((VCHAR *)MPC860SMC1_BASE+(reg*MPC860SMC1_REG_OFFSET))
#endif	/* _ASMLANGUAGE */

/* SIO -- mpc860 serial channel chip -- register definitions */

/* Buffer Descriptor Pointer Definitions */

/* Receive BD status bits 16-bit value */

#define BD_RX_EMPTY_BIT		0x8000	/* buffer is empty */
#define BD_RX_WRAP_BIT		0x2000	/* last BD in chain */
#define BD_RX_INTERRUPT_BIT	0x1000	/* set interrupt when filled */
#define BD_RX_CON_MODE_BIT	0x0200	/* Continuous Mode bit */
#define BD_RX_IDLE_CLOSE_BIT	0x0100	/* Close on IDLE recv bit */
#define BD_RX_BREAK_CLOSE_BIT	0x0020	/* Close on break recv bit */
#define BD_RX_FRAME_CLOSE_BIT	0x0010	/* Close on frame error bit */
#define BD_RX_PARITY_ERROR_BIT	0x0008	/* Parity error in last byte */
#define BD_RX_OVERRUN_ERROR_BIT	0x0002	/* Overrun occurred */

/* Transmit BD status bits 16-bit value */

#define BD_TX_READY_BIT		0x8000	/* Transmit ready/busy bit */
#define BD_TX_WRAP_BIT		0x2000	/* last BD in chain */
#define BD_TX_INTERRUPT_BIT	0x1000	/* set interrupt when emptied */
#define BD_TX_CON_MODE_BIT	0x0200	/* Continuous Mode bit */
#define BD_TX_PREABMLE_BIT	0x0100	/* send preamble sequence */

#define BD_STATUS_OFFSET	0x00	/* two bytes */
#define BD_DATA_LENGTH_OFFSET	0x02	/* two bytes */
#define	BD_BUF_POINTER_OFFSET	0x04	/* four bytes */

/* SDMA Configuraton Register bit definition (SDCR - 0x30) */

#define SDCR_FRZ_MSK	0x00006000	/* Freeze mask */
#define SDCR_RAID_MSK	0x00000003	/* Risc Controller Arbitration ID */
#define SDCR_FAID_MSK   0x0000000c      /* FEC Arbitration ID mask */
#define SDCR_FAM_EN     0x00000040      /* FEC Agressive Mode Enable */
#define SDCR_FAID_BR6   0x00000000      /* FEC Arbitration ID U-Bus priority 6*/
#define SDCR_FAID_BR5   0x00000004      /* FEC Arbitration ID U-Bus priority 5*/
#define SDCR_FAID_BR2   0x00000008      /* FEC Arbitration ID U-Bus priority 2*/
#define SDCR_FAID_BR1   0x0000000c      /* FEC Arbitration ID U-Bus priority 1*/
#define SDCR_RAID_BR6	0x00000000	/* U-BUS arbitration priority 6 (BR6) */
#define SDCR_RAID_BR5	0x00000001	/* U-BUS arbitration priority 5 (BR5) */
#define SDCR_RAID_BR2	0x00000002	/* U-BUS arbitration priority 2 (BR2) */
#define SDCR_RAID_BR1	0x00000003	/* U-BUS arbitration priority 1 (BR1) */

/* SDMA Status Register bit definition (SDSR - 0x908) */

#define SDSR_SBER	0x80		/* Channel Bus Error */
#define SDSR_RINT	0x40		/* Reserved Interrupt */
#define SDSR_DSP2	0x02		/* DSP chain interrupt 2 */
#define SDSR_DSP1	0x01		/* DSP chain interrupt 1 */

/* IDMA Status Register bit definition (IDSR - 0x910) */

#define IDSR_OB		0x04		/* Out of Buffer */
#define IDSR_DONE	0x02		/* IDMA transfert Done */
#define IDSR_AD		0x01		/* Auxiliry Done */

/* Timer Global Configuration Register bit definition (TGCR - 0x980) */

#define TGCR_CAS4		0x8000		/* Cascade Timer 3 and 4 */
#define TGCR_FRZ4		0x4000		/* Freeze timer 4 */
#define TGCR_STP4		0x2000		/* Stop timer 4 */
#define TGCR_RST4		0x1000		/* Reset timer 4 */
#define TGCR_GM2		0x0800		/* Gate Mode for Pin 2 */
#define TGCR_FRZ3		0x0400		/* Freeze timer 3 */
#define TGCR_STP3		0x0200		/* Stop timer 3 */
#define TGCR_RST3		0x0100		/* Reset timer 3 */
#define TGCR_CAS2		0x0080		/* Cascade Timer 1 and 2 */
#define TGCR_FRZ2		0x0040		/* Freeze timer 2 */
#define TGCR_STP2		0x0020		/* Stop timer 2 */
#define TGCR_RST2		0x0010		/* Reset timer 2 */
#define TGCR_GM1		0x0008		/* Gate Mode for Pin 1 */
#define TGCR_FRZ1		0x0004		/* Freeze timer 1 */
#define TGCR_STP1		0x0002		/* Stop timer 1 */
#define TGCR_RST1		0x0001		/* Reset timer 1 */

/* Timer Mode Register bit definition (TMRx - 0x990) */

#define TMR_PS_MSK		0xff00	/* Prescaler Value */
#define TMR_CE_MSK		0x00c0	/* Capture Edge and Enable Interrupt */
#define TMR_CE_INTR_DIS		0x0000	/* Disable Interrupt on capture event */
#define TMR_CE_RISING		0x0040	/* Capture on Rising TINx edge only */
#define TMR_CE_FALLING		0x0080	/* Capture on Falling TINx edge only */
#define TMR_CE_ANY		0x00c0	/* Capture on any TINx edge */
#define TMR_OM			0x0020	/* Output Mode */
#define TMR_ORI			0x0010	/* Output Reference Interrupt Enable */
#define TMR_FRR			0x0008	/* Free Run/Restart */
#define TMR_ICLK_MSK		0x0006	/* Timer Input Clock Source mask */
#define TMR_ICLK_IN_CAS		0x0000	/* Internally cascaded input */
#define TMR_ICLK_IN_GEN		0x0002	/* Internal General system clock */
#define TMR_ICLK_IN_GEN_DIV16	0x0004	/* Internal General system clk div 16 */
#define TMR_ICLK_TIN_PIN	0x0006	/* TINx pin */
#define TMR_GE			0x0001	/* Gate Enable */

/* Timer Event Register bit definition (TERx - 0x9B0) */

#define TER_REF		0x0002		/* Output Reference Event */
#define TER_CAP		0x0001		/* Capture Event */

/* BDG Configuratioon REgister bit definition (BRGC - 0x9F0) */

#define BRGC_RST		0x20000		/* Reset BRG */
#define BRGC_EN			0x10000		/* Enable BRG count */
#define BRGC_EXTC_MSK		0x08000		/* External Clock Source Mask */
#define BRGC_EXTC_BRGCLK	0x00000		/* Baud Rate Gen clock source */
#define BRGC_EXTC_CLK2 		0x04000		/* CLK2 pin = BRG source */
#define BRGC_EXTC_CLK6		0x08000		/* CLK6 pin = BRG source */
#define BRGC_ATB		0x02000		/* 1 = Autobaud on Rx */
						/* 0 = normal operation */
#define BRGC_CD_MSK		0x01FFE		/* clock divider 12 bit value */
#define BRGC_CD_SHIFT		0x1		/* shifted up one bit */
#define BRGC_DIV16		0x00001		/* BRG Clock divide by 16 */

/* UART Mode Register bit definition (PSMR - 0x0A08) */

#define PSMR_FLC		0x8000	/* Flow Control */
#define PSMR_SL			0x4000	/* Stop Length */
#define PSMR_CL_MSK		0x3000	/* Character Length Mask */
#define PSMR_CL_5_BITS		0x0000	/* Character Length: 5 bits */
#define PSMR_CL_6_BITS		0x1000	/* Character Length: 6 bits */
#define PSMR_CL_7_BITS		0x2000	/* Character Length: 7 bits */
#define PSMR_CL_8_BITS		0x3000	/* Character Length: 8 bits */
#define PSMR_UM_MSK		0x0c00	/* UART Mode Mask */
#define PSMR_UM_NORMAL		0x0000	/* UART Mode: normal */
#define PSMR_UM_NON_AUTO	0x0400	/* UART Mode: multidrop non-automatic */
#define PSMR_UM_AUTO		0x0c00	/* UART Mode: multidrop automatic */
#define PSMR_FRZ		0x0200	/* Freeze Transmission */
#define PSMR_RZS		0x0100	/* Receive Zero Stop bits */
#define PSMR_SYN		0x0080	/* Synchronous Mode */
#define PSMR_DRT		0x0040	/* Disable Receiver While Transmitting*/
#define PSMR_PEN		0x0010	/* Parity Enable */
#define PSMR_RPN_MSK		0x000c	/* Receiver Parity Mode Mask */
#define PSMR_RPN_ODD		0x0000	/* Receiver Parity Mode: Odd parity */
#define PSMR_RPN_LOW		0x0004	/* Receiver Parity Mode: Low parity */
#define PSMR_RPN_EVEN		0x0008	/* Receiver Parity Mode: Even parity */
#define PSMR_RPN_HIGH		0x000c	/* Receiver Parity Mode: High parity */
#define PSMR_TPN_MSK		0x0003	/* Transmitter Parity Mode Mask */
#define PSMR_TPN_ODD		0x0000	/* Transmit Parity Mode: Odd parity */
#define PSMR_TPN_LOW		0x0001	/* Transmit Parity Mode: Low parity */
#define PSMR_TPN_EVEN		0x0002	/* Transmit Parity Mode: Even parity */
#define PSMR_TPN_HIGH		0x0003	/* Transmit Parity Mode: High parity */

/* CPM - Communication Processor Module */

/* Command Register definitions (CPCR - 0x9C0) */    
 
#define CPM_CR_FLG              0x0001	/* flag - command executing */ 
#define	CPM_CR_RESET		0x8000	/* software reset command */
#define CPM_CR_OPCODE           0x0f00	/* command opcode */
#define	CPM_CR_CHAN(x)		(((x) & 0x000f) << 4)
#define	CPM_CR_OP(x)		(((x) & 0x000f) << 8)
    
/* CPCR - Channel Numbers */

#define CPM_CR_CHANNEL_SCC1     CPM_CR_CHAN(0x0)	/* SCC1 channel */ 
#define	CPM_CR_CHANNEL_I2C	CPM_CR_CHAN(0x1)	/* I2C channel */
#define CPM_CR_CHANNEL_SCC2     CPM_CR_CHAN(0x4)	/* SCC2 channel */
#define CPM_CR_CHANNEL_SPI      CPM_CR_CHAN(0x5)	/* SPI channel */ 
#define CPM_CR_CHANNEL_RTMR     CPM_CR_CHAN(0x5)	/* RISC timer channel */
#define CPM_CR_CHANNEL_SCC3     CPM_CR_CHAN(0x8)	/* SCC3 channel */ 
#define CPM_CR_CHANNEL_SMC1     CPM_CR_CHAN(0x9)	/* SMC1 channel */ 
#define CPM_CR_CHANNEL_IDMA1    CPM_CR_CHAN(0x1)	/* IDMA1 channel */ 
#define CPM_CR_CHANNEL_SCC4     CPM_CR_CHAN(0xc)	/* SCC4 channel */ 
#define CPM_CR_CHANNEL_SMC2     CPM_CR_CHAN(0xd)	/* SMC2 channel */ 
#define	CPM_CR_CHANNEL_PIP	CPM_CR_CHAN(0xd)	/* PIP channel */
#define CPM_CR_CHANNEL_IDMA2    CPM_CR_CHAN(0x5)	/* IDMA2 channel */ 
#define CPM_CR_CHANNEL_DSP1     CPM_CR_CHAN(0x9)	/* DSP1 channel */ 
#define CPM_CR_CHANNEL_DSP2     CPM_CR_CHAN(0xd)	/* DSP2 channel */ 
 
/* CPCR - opcodes */
 
#define CPM_CR_OPCODE_INIT_RT   CPM_CR_OP(0x0)		/* Init rx and tx */
#define CPM_CR_OPCODE_INIT_R    CPM_CR_OP(0x1)		/* init rx only */
#define CPM_CR_OPCODE_INIT_T    CPM_CR_OP(0x2)		/* init tx only */
#define CPM_CR_OPCODE_HUNT      CPM_CR_OP(0x3)		/* rx frame hunt mode */
#define CPM_CR_OPCODE_STOP      CPM_CR_OP(0x4)		/* stop tx */
#define CPM_CR_OPCODE_GRSTOP    CPM_CR_OP(0x5)		/* gracefully stop tx */
#define	CPM_CR_OPCODE_IDMA_INIT	CPM_CR_OP(0x5)		/* init idma */
#define CPM_CR_OPCODE_RESTART   CPM_CR_OP(0x6)		/* restart tx */
#define CPM_CR_OPCODE_CLOSE     CPM_CR_OP(0x7)		/* close rx buffer */
#define CPM_CR_OPCODE_SET_GROUP CPM_CR_OP(0x8)		/* set group address */
#define	CPM_CR_OPCODE_SET_ITMR	CPM_CR_OP(0x8)		/* set itimer */
#define	CPM_CR_OPCODE_GCI_TMO	CPM_CR_OP(0x9)		/* gci timeout */
#define CPM_CR_OPCODE_RESET_BCS CPM_CR_OP(0xa)		/* blk chk seq reset */
#define	CPM_CR_OPCODE_GCI_ABRT	CPM_CR_OP(0xa)		/* gci abort request */
#define	CPM_CR_OPCODE_IDMA_STOP	CPM_CR_OP(0xb)		/* stop idma */
#define	CPM_CR_OPCODE_DSP_START	CPM_CR_OP(0xc)		/* start DSP */
#define	CPM_CR_OPCODE_DSP_INIT	CPM_CR_OP(0xd)		/* init DSP */    

#define CPM_CR_SCC_INIT_RT      CPM_CR_OPCODE_INIT_RT	/* rx and tx init */
#define CPM_CR_SCC_INIT_R       CPM_CR_OPCODE_INIT_R	/* init rx only */
#define CPM_CR_SCC_INIT_T       CPM_CR_OPCODE_INIT_T	/* init tx only */
#define CPM_CR_SCC_HUNT         CPM_CR_OPCODE_HUNT	/* rx frm hunt mode */
#define CPM_CR_SCC_STOP         CPM_CR_OPCODE_STOP	/* stop tx */
#define CPM_CR_SCC_GRSTOP       CPM_CR_OPCODE_GRSTOP	/* graceful stop tx */
#define CPM_CR_SCC_RESTART      CPM_CR_OPCODE_RESTART	/* restart tx */
#define CPM_CR_SCC_CLOSE        CPM_CR_OPCODE_CLOSE	/* close rx buffer */
#define CPM_CR_SCC_SET_GROUP    CPM_CR_OPCODE_SET_GROUP	/* set group adrs */
#define CPM_CR_SCC_RESET_BCS    CPM_CR_OPCODE_RESET_BCS	/* reset BCS */
 
#define CPM_CR_SMC_INIT_RT      CPM_CR_OPCODE_INIT_RT	/* rx and tx init */
#define CPM_CR_SMC_INIT_R       CPM_CR_OPCODE_INIT_R	/* init rx only */
#define CPM_CR_SMC_INIT_T       CPM_CR_OPCODE_INIT_T	/* init tx only */
#define CPM_CR_SMC_HUNT         CPM_CR_OPCODE_HUNT	/* rx frm hunt mode */
#define CPM_CR_SMC_STOP         CPM_CR_OPCODE_STOP	/* stop tx */
#define CPM_CR_SMC_RESTART	CPM_CR_OPCODE_RESTART	/* restart tx */
#define CPM_CR_SMC_CLOSE        CPM_CR_OPCODE_CLOSE	/* close rx buffer */

#define CPM_CR_SMC_GCI_INIT_RT	CPM_CR_OPCODE_INIT_RT	/* rx and tx init */
#define CPM_CR_SMC_GCI_TMO	CPM_CR_OPCODE_GCI_TMO	/* GCI timeout */
#define CPM_CR_SMC_GCI_ABORT    CPM_CR_OPCODE_GCI_ABRT	/* GCI abort */

#define CPM_CR_SPI_INIT_RT      CPM_CR_OPCODE_INIT_RT	/* rx and tx init */
#define CPM_CR_SPI_INIT_R       CPM_CR_OPCODE_INIT_R	/* init rx only */
#define CPM_CR_SPI_INIT_T       CPM_CR_OPCODE_INIT_T	/* init tx only */
#define CPM_CR_SPI_CLOSE        CPM_CR_OPCODE_CLOSE	/* close rx buffer */

#define CPM_CR_I2C_INIT_RT      CPM_CR_OPCODE_INIT_RT	/* rx and tx init */
#define CPM_CR_I2C_INIT_R       CPM_CR_OPCODE_INIT_R	/* init rx only */
#define CPM_CR_I2C_INIT_T       CPM_CR_OPCODE_INIT_T	/* init tx only */
#define CPM_CR_I2C_CLOSE        CPM_CR_OPCODE_CLOSE	/* close rx buffer */

#define CPM_CR_IDMA_INIT	CPM_CR_OPCODE_IDMA_INIT	/* IDMA init */
#define CPM_CR_IDMA_STOP	CPM_CR_OPCODE_IDMA_STOP	/* IDMA stop */

#define CPM_CR_DSP_START	CPM_CR_OPCODE_DSP_START	/* DSP start */
#define CPM_CR_DSP_INIT		CPM_CR_OPCODE_DSP_INIT	/* DSP init */

#define CPM_CR_TIMER_SET	CPM_CR_OPCODE_SET_ITMR	/* TIMER set */

/* UART Even Register bit definition (SCCE - 0x0A10) */

#define SCCE_GLR		0x1000	/* Glitch on Receiver */
#define SCCE_GLT		0x0800	/* Glitch on Transmitter */
#define SCCE_AB			0x0200	/* Auto Baud */
#define SCCE_IDL		0x0100	/* Idle Sequence Status Changed */
#define SCCE_GRA		0x0080	/* Graceful stop complete */
#define SCCE_BRKE		0x0040	/* Break End */
#define SCCE_BRKS		0x0020	/* Break Start */
#define SCCE_CCR		0x0008	/* Control Character Received */
#define SCCE_BSY		0x0004	/* Busy Condition */
#define SCCE_TX			0x0002	/* Transmitter Buffer */
#define SCCE_RX			0x0001	/* Receiver Buffer */

/* SCC Status Register bit definition (SCCS - 0x0A17) */

#define SCCS_ID			0x01	/* Idle Status */

/* SI Mode Register bit definition (SIMODE - 0xAE0) */

#define SIMODE_SMC2_MUX		0x80000000	/* connected to mux SI */
#define SIMODE_SMC2_NMSI	0x00000000	/* NMSI mode */
#define SIMODE_SMC2CS_BRG1	0x00000000	/* BRG1 is clock source */
#define SIMODE_SMC2CS_BRG2	0x10000000	/* BRG2 is clock source */
#define SIMODE_SMC2CS_BRG3	0x20000000	/* BRG3 is clock source */
#define SIMODE_SMC2CS_BRG4	0x30000000	/* BRG4 is clock source */
#define SIMODE_SMC2CS_CLK5	0x40000000	/* CLK5 is clock source */
#define SIMODE_SMC2CS_CLK6	0x50000000	/* CLK6 is clock source */
#define SIMODE_SMC2CS_CLK7	0x60000000	/* CLK7 is clock source */
#define SIMODE_SMC2CS_CLK8	0x70000000	/* CLK8 is clock source */
#define SIMODE_SDMB_MSK		0x0c000000	/* SI Diagnostic Mode - TDM B */
#define SIMODE_SDMB_NORM	0x00000000	/* Diag Mode-normal operation */
#define SIMODE_SDMB_ECHO	0x04000000	/* Diag Mode-auto echo */
#define SIMODE_SDMB_LOOP	0x08000000	/* Diag Mode-internal loopback*/
#define SIMODE_SDMB_LCTR	0x0C000000	/* Diag Mode-loopback control */
#define SIMODE_RFSDB_MSK	0x03000000	/* Receive Frame Delay TDM B */
#define SIMODE_RFSDB_NO_DELAY	0x00000000	/* Receive Delay - No Delay */
#define SIMODE_RFSDB_1BIT	0x01000000	/* Receive Delay - 1bit delay */
#define SIMODE_RFSDB_2BIT	0x02000000	/* Receive Delay - 2bit delay */
#define SIMODE_RFSDB_3BIT	0x03000000	/* Receive Delay - 3bit delay */
#define SIMODE_DSCB		0x00800000	/* Double Speed Clocl TDM B */
#define SIMODE_CRTB		0x00400000	/* Common Rcv and Trans. pins */
#define SIMODE_STZB		0x00200000	/* Set L1TxDx to Zero TDM B */
#define SIMODE_CEB_FALLING	0x00000000	/* Clock Edge - falling */
#define SIMODE_CEB_RISING	0x00100000	/* Clock Edge - rising */
#define SIMODE_FEB_FALLING	0x00000000	/* Frame Sync Edge - falling  */
#define SIMODE_FEB_RISING	0x00080000	/* Frame Sync Edge - rising */
#define SIMODE_GMB_GCI_SCIT	0x00000000	/* Grant Mode - GCI/SCIT */
#define SIMODE_GMB_IDL		0x00040000	/* Grant Mode - idle */
#define SIMODE_TFSDB_MSK	0x00030000	/* Transmit Frame Sync Delay */
#define SIMODE_TFSDB_NO_DELAY	0x00000000	/* Transmit Delay - No Delay */
#define SIMODE_TFSDB_1BIT	0x00010000	/* Transmit Delay - 1bit */
#define SIMODE_TFSDB_2BIT	0x00020000	/* Transmit Delay - 2bit */
#define SIMODE_TFSDB_3BIT	0x00030000	/* Transmit Delay - 3bit */


#define SIMODE_SMC1_MUX		0x00008000	/* connected to mux SI */
#define SIMODE_SMC1_NMSI	0x00000000	/* NMSI mode */
#define SIMODE_SMC1CS_BRG1	0x00000000	/* BRG1 is clock source */
#define SIMODE_SMC1CS_BRG2	0x00001000	/* BRG2 is clock source */
#define SIMODE_SMC1CS_BRG3	0x00002000	/* BRG3 is clock source */
#define SIMODE_SMC1CS_BRG4	0x00003000	/* BRG4 is clock source */
#define SIMODE_SMC1CS_CLK1	0x00004000	/* CLK1 is clock source */
#define SIMODE_SMC1CS_CLK2	0x00005000	/* CLK2 is clock source */
#define SIMODE_SMC1CS_CLK3	0x00006000	/* CLK3 is clock source */
#define SIMODE_SMC1CS_CLK4	0x00007000	/* CLK4 is clock source */
#define SIMODE_SDMA_MSK		0x00000c00	/* SI Diagnostic Mode - TDM B */
#define SIMODE_SDMA_NORM	0x00000000	/* Diag Mode-normal operation */
#define SIMODE_SDMA_ECHO	0x00000400	/* Diag Mode-auto echo */
#define SIMODE_SDMA_LOOP	0x00000800	/* Diag Mode-internal loopback*/
#define SIMODE_SDMA_LCTR	0x00000C00	/* Diag Mode-loopback control */
#define SIMODE_RFSDA_MSK	0x00000300	/* Receive Frame Delay TDM A */
#define SIMODE_RFSDA_NO_DELAY	0x00000000	/* Receive Delay - No Delay */
#define SIMODE_RFSDA_1BIT_DELAY	0x00000100	/* Receive Delay - 1bit delay */
#define SIMODE_RFSDA_2BIT_DELAY	0x00000200	/* Receive Delay - 2bit delay */
#define SIMODE_RFSDA_3BIT_DELAY	0x00000300	/* Receive Delay - 3bit delay */
#define SIMODE_DSCA		0x00000080	/* Double Speed Clocl TDM A */
#define SIMODE_CRTA		0x00000040	/* Common Rcv and Trans. pins */
#define SIMODE_STZA		0x00000020	/* Set L1TxDx to Zero TDM A */
#define SIMODE_CEA_FALLING	0x00000000	/* Clock Edge - falling */
#define SIMODE_CEA_RISING	0x00000010	/* Clock Edge - rising */
#define SIMODE_FEA_FALLING	0x00000000	/* Frame Sync Edge - falling  */
#define SIMODE_FEA_RISING	0x00000008	/* Frame Sync Edge - rising */
#define SIMODE_GMA_GCI_SCIT	0x00000000	/* Grant Mode - GCI/SCIT */
#define SIMODE_GMA_IDL		0x00000004	/* Grant Mode - idle */
#define SIMODE_TFSDA_MSK	0x00000003	/* Transmit Frame Sync Delay */
#define SIMODE_TFSDA_NO_DELAY	0x00000000	/* Transmit Delay - No Delay */
#define SIMODE_TFSDA_1BIT	0x00000001	/* Transmit Delay - 1bit */
#define SIMODE_TFSDA_2BIT	0x00000002	/* Transmit Delay - 2bit */
#define SIMODE_TFSDA_3BIT	0x00000003	/* Transmit Delay - 3bit */

/* SI Global Mode Register bit definition (SIGMR - 0xAE4) */

#define SIGMR_ENB	0x08		/* Enable Channel B */
#define SIGMR_ENA	0x04		/* Enable Channel A */
#define SIGMR_RDM_MSK	0x03		/* Ram Division Mode mask */

/* SI Status Register bit definition (SISTR - 0xAE6) */

#define SISTR_CRORA		0x80	/* Current Route of TDM A Receiver */
#define SISTR_CROTA		0x40	/* Current Route of TDM A Transmitter */
#define SISTR_CRORB		0x20	/* Current Route of TDM B Receiver */
#define SISTR_CROTB		0x10	/* Current Route of TDM B Transmitter */

/* SI Command Register bit definition (SICMR - 0xAE7) */

#define SICMR_CSRRA		0x80	/* Change Shadow RAM - TDM A Receiver */
#define SICMR_CSRTA		0x40	/* Change Shadow RAM - TDM A Transmit */
#define SICMR_CSRRB		0x20	/* Change Shadow RAM - TDM B Receiver */
#define SICMR_CSRTB		0x10	/* Change Shadow RAM - TDM B Transmit */

/* SI Clock Route Register bit definition (SICR - 0xAEC) */

#define SICR_GR4		0x80000000	/* Grant Support of SCC4 */
#define SICR_SC4_MUX		0x40000000	/* SCC4 Connection - mux SI */
#define SICR_R4CS_MSK		0x38000000	/* SCC4 Receive Clock Source */
#define SICR_R4CS_BRG1		0x00000000	/* BRG1 clock source */
#define SICR_R4CS_BRG2		0x08000000	/* BRG2 clock source */
#define SICR_R4CS_BRG3		0x10000000	/* BRG3 clock source */
#define SICR_R4CS_BRG4		0x18000000	/* BRG4 clock source */
#define SICR_R4CS_CLK5		0x20000000	/* CLK5 clock source */
#define SICR_R4CS_CLK6		0x28000000	/* CLK6 clock source */
#define SICR_R4CS_CLK7		0x30000000	/* CLK7 clock source */
#define SICR_R4CS_CLK8		0x38000000	/* CLK8 clock source */
#define SICR_T4CS_MSK		0x07000000	/* SCC4 Transmit Clock Source */
#define SICR_T4CS_BRG1		0x00000000	/* BRG1 clock source */
#define SICR_T4CS_BRG2		0x01000000	/* BRG2 clock source */
#define SICR_T4CS_BRG3		0x02000000	/* BRG3 clock source */
#define SICR_T4CS_BRG4		0x03000000	/* BRG4 clock source */
#define SICR_T4CS_CLK5		0x04000000	/* CLK5 clock source */
#define SICR_T4CS_CLK6		0x05000000	/* CLK6 clock source */
#define SICR_T4CS_CLK7		0x06000000	/* CLK7 clock source */
#define SICR_T4CS_CLK8		0x07000000	/* CLK8 clock source */

#define SICR_GR3		0x00800000	/* Grant Support of SCC3 */
#define SICR_SC3_MUX		0x00400000	/* SCC3 Connection - mux SI */
#define SICR_R3CS_MSK		0x00380000	/* SCC3 Receive Clock Source */
#define SICR_R3CS_BRG1		0x00000000	/* BRG1 clock source */
#define SICR_R3CS_BRG2		0x00080000	/* BRG2 clock source */
#define SICR_R3CS_BRG3		0x00100000	/* BRG3 clock source */
#define SICR_R3CS_BRG4		0x00180000	/* BRG4 clock source */
#define SICR_R3CS_CLK5		0x00200000	/* CLK5 clock source */
#define SICR_R3CS_CLK6		0x00280000	/* CLK6 clock source */
#define SICR_R3CS_CLK7		0x00300000	/* CLK7 clock source */
#define SICR_R3CS_CLK8		0x00380000	/* CLK8 clock source */
#define SICR_T3CS_MSK		0x00070000	/* SCC3 Transmit Clock Source */
#define SICR_T3CS_BRG1		0x00000000	/* BRG1 clock source */
#define SICR_T3CS_BRG2		0x00010000	/* BRG2 clock source */
#define SICR_T3CS_BRG3		0x00020000	/* BRG3 clock source */
#define SICR_T3CS_BRG4		0x00030000	/* BRG4 clock source */
#define SICR_T3CS_CLK5		0x00040000	/* CLK5 clock source */
#define SICR_T3CS_CLK6		0x00050000	/* CLK6 clock source */
#define SICR_T3CS_CLK7		0x00060000	/* CLK7 clock source */
#define SICR_T3CS_CLK8		0x00070000	/* CLK8 clock source */

#define SICR_GR2		0x00008000	/* Grant Support of SCC2 */
#define SICR_SC2_MUX		0x00004000	/* SCC2 Connection - mux SI */
#define SICR_R2CS_MSK		0x00003800	/* SCC2 Receive Clock Source */
#define SICR_R2CS_BRG1		0x00000000	/* BRG1 clock source */
#define SICR_R2CS_BRG2		0x00000800	/* BRG2 clock source */
#define SICR_R2CS_BRG3		0x00001000	/* BRG3 clock source */
#define SICR_R2CS_BRG4		0x00001800	/* BRG4 clock source */
#define SICR_R2CS_CLK1		0x00002000	/* CLK1 clock source */
#define SICR_R2CS_CLK2		0x00002800	/* CLK2 clock source */
#define SICR_R2CS_CLK3		0x00003000	/* CLK3 clock source */
#define SICR_R2CS_CLK4		0x00003800	/* CLK4 clock source */
#define SICR_T2CS_MSK		0x00000700	/* SCC2 Transmit Clock Source */
#define SICR_T2CS_BRG1		0x00000000	/* BRG1 clock source */
#define SICR_T2CS_BRG2		0x00000100	/* BRG2 clock source */
#define SICR_T2CS_BRG3		0x00000200	/* BRG3 clock source */
#define SICR_T2CS_BRG4		0x00000300	/* BRG4 clock source */
#define SICR_T2CS_CLK1		0x00000400	/* CLK1 clock source */
#define SICR_T2CS_CLK2		0x00000500	/* CLK2 clock source */
#define SICR_T2CS_CLK3		0x00000600	/* CLK3 clock source */
#define SICR_T2CS_CLK4		0x00000700	/* CLK4 clock source */

#define SICR_GR1		0x00000080	/* Grant Support of SCC1 */
#define SICR_SC1_MUX		0x00000040	/* SCC1 Connection - mux SI */
#define SICR_R1CS_MSK		0x00000038	/* SCC1 Receive Clock Source */
#define SICR_R1CS_BRG1		0x00000000	/* BRG1 clock source */
#define SICR_R1CS_BRG2		0x00000008	/* BRG2 clock source */
#define SICR_R1CS_BRG3		0x00000010	/* BRG3 clock source */
#define SICR_R1CS_BRG4		0x00000018	/* BRG4 clock source */
#define SICR_R1CS_CLK1		0x00000020	/* CLK1 clock source */
#define SICR_R1CS_CLK2		0x00000028	/* CLK2 clock source */
#define SICR_R1CS_CLK3		0x00000030	/* CLK3 clock source */
#define SICR_R1CS_CLK4		0x00000038	/* CLK4 clock source */
#define SICR_T1CS_MSK		0x00000007	/* SCC1 Transmit Clock Source */
#define SICR_T1CS_BRG1		0x00000000	/* BRG1 clock source */
#define SICR_T1CS_BRG2		0x00000001	/* BRG2 clock source */
#define SICR_T1CS_BRG3		0x00000002	/* BRG3 clock source */
#define SICR_T1CS_BRG4		0x00000003	/* BRG4 clock source */
#define SICR_T1CS_CLK1		0x00000004	/* CLK1 clock source */
#define SICR_T1CS_CLK2		0x00000005	/* CLK2 clock source */
#define SICR_T1CS_CLK3		0x00000006	/* CLK3 clock source */
#define SICR_T1CS_CLK4		0x00000007	/* CLK4 clock source */

/* SI Ram Pointer bit definition (SIRP - 0xAF0) */

#define SIRP_VTB_MSK		0x20000000	/* Transmitter B pointer Valid*/
#define SIRP_TBPTR_MSK		0x1f000000	/* Transmitter B pointer */
#define SIRP_VTA_MSK		0x00200000	/* Transmitter A pointer Valid*/
#define SIRP_TAPTR_MSK		0x001f0000	/* Transmitter A pointer */
#define SIRP_VRB_MSK		0x00002000	/* Receiver B pointer Valid */
#define SIRP_RBPTR_MSK		0x00001f00	/* Receiver B pointer */
#define SIRP_VRA_MSK		0x00000020	/* Receiver A pointer Valid */
#define SIRP_RAPTR_MSK		0x0000001f	/* Receiver A pointer */

/* SMC UART Event Register bit definition (SMCE - 0x0A86) */ 

#define SMCE_BRK		0x10	/* Break character received */
#define SMCE_BSY		0x04	/* Busy Condition */
#define SMCE_TX			0x02	/* Transmit Buffer */
#define SMCE_RX			0x01	/* Receive Buffer */
#define SMCE_ALL_EVENTS		(SMCE_BRK | SMCE_BSY | SMCE_TX | SMCE_RX)

/* SMC UART Mask Register bit definition (SMCM - 0x0A8A) */ 

#define SMCM_BRK_MSK		0x10	/* Break character received Mask */
#define SMCM_BSY_MSK		0x04	/* Busy Condition Mask */
#define SMCM_TX_MSK		0x02	/* Transmit Buffer Mask */
#define SMCM_RX_MSK		0x01	/* Receive Buffer Mask */

/* equates for CICR register CP interrupt configuration register */
#define CICR_IRL_LEVEL_0	0x00000000	/* highest interrupt level */
#define CICR_IRL_LEVEL_1	0x00020000
#define CICR_IRL_LEVEL_2	0x00040000
#define CICR_IRL_LEVEL_3	0x00060000
#define CICR_IRL_LEVEL_4	0x00080000	/* standard value */
#define CICR_IRL_LEVEL_5	0x000A0000
#define CICR_IRL_LEVEL_6	0x000C0000
#define CICR_IRL_LEVEL_7	0x000E0000	/* lowest */
#define CICR_HP_SRC_STD		0x0001F000	/* highest priority int */
#define CICR_MASTER_IEN		0x00000080	/* master interrupt enable */

/* SPI Mode Register bit definition (SPMODE - 0x0AA0) */

#define SPMODE_LOOP		0x4000	/* Loop Mode */
#define SPMODE_CI		0x2000	/* Clock Invert */
#define SPMODE_CI_LOW		0x0000	/* Inactive state is low */
#define SPMODE_CI_HIGH		0x2000	/* Inactive state is high */
#define SPMODE_CP		0x1000	/* Clock Phase */
#define SPMODE_CP_MIDDLE	0x0000	/* Clock Phase: middle of the data */
#define SPMODE_CP_BEGIN		0x1000	/* Clock Phase: Beginning of the data */
#define SPMODE_DIV16		0x0800	/* BRGCLK Divide by 16	*/
#define SPMODE_REV		0x0400	/* Reverse Data */
#define SPMODE_MS		0x0200	/* SPI Master */
#define SPMODE_EN		0x0100	/* Enable SPI */
#define SPMODE_LEN_MSK		0x00f0	/* Character Length */
#define SPMODE_LEN_1_BIT	0x0000	/* Character Length: 1 bit */
#define SPMODE_LEN_2_BITS	0x0010	/* Character Length: 2 bits */
#define SPMODE_LEN_3_BITS	0x0020	/* Character Length: 3 bits */
#define SPMODE_LEN_4_BITS	0x0030	/* Character Length: 4 bits */
#define SPMODE_LEN_5_BITS	0x0040	/* Character Length: 5 bits */
#define SPMODE_LEN_6_BITS	0x0050	/* Character Length: 6 bits */
#define SPMODE_LEN_7_BITS	0x0060	/* Character Length: 7 bits */
#define SPMODE_LEN_8_BITS	0x0070	/* Character Length: 8 bits */
#define SPMODE_LEN_9_BITS	0x0080	/* Character Length: 9 bits */
#define SPMODE_LEN_10_BITS	0x0090	/* Character Length: 10 bits */
#define SPMODE_LEN_11_BITS	0x00a0	/* Character Length: 11 bits */
#define SPMODE_LEN_12_BITS	0x00b0	/* Character Length: 12 bits */
#define SPMODE_LEN_13_BITS	0x00c0	/* Character Length: 13 bits */
#define SPMODE_LEN_14_BITS	0x00d0	/* Character Length: 14 bits */
#define SPMODE_LEN_15_BITS	0x00e0	/* Character Length: 15 bits */
#define SPMODE_LEN_16_BITS	0x00f0	/* Character Length: 16 bits */
#define SPMODE_PM_MSK		0x000f	/* BRGCLK Prescale Modulus select mask*/
#define SPMODE_PM_DIV4		0x0000	/* BRGCLK clock divided by 4 */
#define SPMODE_PM_DIV8		0x0001	/* BRGCLK clock divided by 8 */
#define SPMODE_PM_DIV12		0x0002	/* BRGCLK clock divided by 12 */
#define SPMODE_PM_DIV16		0x0003	/* BRGCLK clock divided by 16 */
#define SPMODE_PM_DIV20		0x0004	/* BRGCLK clock divided by 20 */
#define SPMODE_PM_DIV24		0x0005	/* BRGCLK clock divided by 24 */
#define SPMODE_PM_DIV28		0x0006	/* BRGCLK clock divided by 28 */
#define SPMODE_PM_DIV32		0x0007	/* BRGCLK clock divided by 32 */
#define SPMODE_PM_DIV36		0x0008	/* BRGCLK clock divided by 36 */
#define SPMODE_PM_DIV40		0x0009	/* BRGCLK clock divided by 40 */
#define SPMODE_PM_DIV44		0x000a	/* BRGCLK clock divided by 44 */
#define SPMODE_PM_DIV48		0x000b	/* BRGCLK clock divided by 48 */
#define SPMODE_PM_DIV52		0x000c	/* BRGCLK clock divided by 52 */
#define SPMODE_PM_DIV56		0x000d	/* BRGCLK clock divided by 56 */
#define SPMODE_PM_DIV60		0x000e	/* BRGCLK clock divided by 60 */
#define SPMODE_PM_DIV64		0x000f	/* BRGCLK clock divided by 64 */

/* SPI Command Register (SPCOM - 0x0AAD) */

#define SPCOM_STR		0x80	/* Start Transmit */


/* SCC Receive Function Code Register bit definition (RFCR) */

#define RFCR_BO_MSK		0x18	/* Byte Order Mask */
#define RFCR_BO_DEC_LE		0x00	/* Byte Order: DEC little endian mode */
#define RFCR_BO_PPC_LE		0x08	/* Byte Order: PPC little endian mode */
					/* reverse transmission order of */
					/* bytes compared to DEC/Intel mode */
#define RFCR_BO_MOT_BE		0x18	/* Motorola Big endian mode */
#define RFCR_AT_MSK		0x07	/* Address Type Mask */

/* SCC Transmit Function Code Register bit definition (TFCR) */ 

#define TFCR_BO_MSK		0x18	/* Byte Order Mask */
#define TFCR_BO_DEC_LE		0x00	/* Byte Order: DEC little endian mode */
#define TFCR_BO_PPC_LE		0x08	/* Byte Order: PPC little endian mode */
					/* reverse transmission order of */
					/* bytes compared to DEC/Intel mode */
#define TFCR_BO_MOT_BE		0x18	/* Motorola Big endian mode */
#define TFCR_AT_MSK		0x07	/* Address Type Mask */

/* SMC UART Mode Register bit definition (SMCMR - 0x0A82) */

#define	SMCMR_CLEN_MSK		0x7800	/* Character Length Mask */
#define SMCMR_CLEN_0_BIT	0x0000	/* Character Length: 0 bit */
#define SMCMR_CLEN_1_BIT	0x0800	/* Character Length: 1 bit */
#define SMCMR_CLEN_2_BITS	0x1000	/* Character Length: 2 bits */
#define SMCMR_CLEN_3_BITS	0x1800	/* Character Length: 3 bits */
#define SMCMR_CLEN_4_BITS	0x2000	/* Character Length: 4 bits */
#define SMCMR_CLEN_5_BITS	0x2800	/* Character Length: 5 bits */
#define SMCMR_CLEN_6_BITS	0x3000	/* Character Length: 6 bits */
#define SMCMR_CLEN_7_BITS	0x3800	/* Character Length: 7 bits */
#define SMCMR_CLEN_8_BITS	0x4000	/* Character Length: 8 bits */
#define SMCMR_CLEN_9_BITS	0x4800	/* Character Length: 9 bits */
#define SMCMR_CLEN_10_BITS	0x5000	/* Character Length: 10 bits */
#define SMCMR_CLEN_11_BITS	0x5800	/* Character Length: 11 bits */
#define SMCMR_CLEN_12_BITS	0x6000	/* Character Length: 12 bits */
#define SMCMR_CLEN_13_BITS	0x6800	/* Character Length: 13 bits */
#define SMCMR_CLEN_14_BITS	0x7000	/* Character Length: 14 bits */
#define SMCMR_CLEN_15_BITS	0x7800	/* Character Length: 15 bits */
#define SMCMR_SL		0x0400	/* Stop Length */		
#define SMCMR_SL_1_BIT		0x0000	/* Stop Length: 1 bit */		
#define SMCMR_SL_2_BITS		0x0400	/* Stop Length: 2 bits */		
#define SMCMR_PEN		0x0200	/* Parity Enable */
#define SMCMR_PM		0x0100	/* Parity Mode */
#define SMCMR_PM_ODD		0x0000	/* Parity Mode: Odd parity */
#define SMCMR_PM_EVEN		0x0100	/* Parity Mode: Even parity */
#define SMCMR_SM_MSK		0x0030	/* SMC Mode Mask */
#define SMCMR_SM_GCI_SCIT	0x0000	/* SMC Mode: GCI or SCIT support */
#define SMCMR_SM_UART		0x0020	/* SMC Mode: GCI or SCIT support */
#define SMCMR_SM_TRANSPARENT	0x0030	/* SMC Mode: Totally transparent */
#define SMCMR_DM_MSK		0x000c	/* Diagnostic Mode Mask */
#define SMCMR_DM_NORMAL		0x0000	/* Diagnostic Mode: Normal */
#define SMCMR_DM_LCLB		0x0004	/* Diagnostic Mode: Local Loopback */
#define SMCMR_DM_ECHO		0x0008	/* Diagnostic Mode: Echo */
#define SMCMR_TEN		0x0002	/* SMC Transmit Enable */
#define SMCMR_REN		0x0001	/* SMC Receive Enable */

#define SMCMR_STD_MODE  	(SMCMR_CLEN_9_BITS | \
				 SMCMR_SL_1_BIT    | \
				 SMCMR_SM_UART	   | \
				 SMCMR_DM_NORMAL)


#define SCMCR_STD_MODE_ENABLED	(SMCMR_STD_MODE	| \
				 SMCMR_TEN	| \
				 SMCMR_REN)

/* CPM Interrupt Vector Register (CIVR - 0x930) */

#define CIVR_IVN_MSK		0xf800		/* Interrupt Vector Number */
#define CIVR_IACK		0x0001		/* Interrupt Acknowledge */

/* CPM Interrupt configuration Register (CICR - 0x940) */

#define CICR_SCCDP_MSK		0xc00000	/* SCCd Priority Order Mask */
#define CICR_SCCDP_SCC1		0x000000	/* SCC1 asserts the SCCd */ 
#define CICR_SCCDP_SCC2		0x400000	/* SCC2 asserts the SCCd */ 
#define CICR_SCCDP_SCC3		0x800000	/* SCC3 asserts the SCCd */ 
#define CICR_SCCDP_SCC4		0xc00000	/* SCC4 asserts the SCCd */ 
#define CICR_SCCCP_MSK		0x300000	/* SCCc Priority Order Mask */
#define CICR_SCCCP_SCC1		0x000000	/* SCC1 asserts the SCCc */ 
#define CICR_SCCCP_SCC2		0x100000	/* SCC2 asserts the SCCc */ 
#define CICR_SCCCP_SCC3		0x200000	/* SCC3 asserts the SCCc */ 
#define CICR_SCCCP_SCC4		0x300000	/* SCC4 asserts the SCCc */ 
#define CICR_SCCBP_MSK		0x0c0000	/* SCCb Priority Order Mask */
#define CICR_SCCBP_SCC1		0x000000	/* SCC1 asserts the SCCb */ 
#define CICR_SCCBP_SCC2		0x040000	/* SCC2 asserts the SCCb */ 
#define CICR_SCCBP_SCC3		0x080000	/* SCC3 asserts the SCCb */ 
#define CICR_SCCBP_SCC4		0x0c0000	/* SCC4 asserts the SCCb */ 
#define CICR_SCCAP_MSK		0x030000	/* SCCa Priority Order Mask */
#define CICR_SCCAP_SCC1		0x000000	/* SCC1 asserts the SCCa */ 
#define CICR_SCCAP_SCC2		0x010000	/* SCC2 asserts the SCCa */ 
#define CICR_SCCAP_SCC3		0x020000	/* SCC3 asserts the SCCa */ 
#define CICR_SCCAP_SCC4		0x030000	/* SCC4 asserts the SCCa */ 
#define CICR_IRL_MSK		0x00e000	/* Interrupt Request Level */
#define CICR_IRL_LVL0		0x000000	/* Interrupt Request Level 0 */
#define CICR_IRL_LVL1		0x002000	/* Interrupt Request Level 1 */
#define CICR_IRL_LVL2		0x004000	/* Interrupt Request Level 2 */
#define CICR_IRL_LVL3		0x006000	/* Interrupt Request Level 3 */
#define CICR_IRL_LVL4		0x008000	/* Interrupt Request Level 4 */
#define CICR_IRL_LVL5		0x00a000	/* Interrupt Request Level 5 */
#define CICR_IRL_LVL6		0x00c000	/* Interrupt Request Level 6 */
#define CICR_IRL_LVL7		0x00e000	/* Interrupt Request Level 7 */
#define CICR_HP_MSK		0x001f00	/* Highest Priority */
#define CICR_IEN		0x000080	/* Interrupt Enable */
#define CICR_SPS		0x000001	/* Spread Priority Scheme */

/* CPM Interrupt Pending Register (CIPR - 0x944) */

#define CIPR_PC15		0x80000000	/* Parallel I/O Port C[15] */
#define CIPR_SCC1		0x40000000	/* SCC 1 */
#define CIPR_SCC2		0x20000000	/* SCC 2 */
#define CIPR_SCC3		0x10000000	/* SCC 3 */
#define CIPR_SCC4		0x08000000	/* SCC 4 */
#define CIPR_PC14		0x04000000	/* Parallel I/O Port C[14] */
#define CIPR_TIMER1		0x02000000	/* Timer 1 */
#define CIPR_PC13		0x01000000	/* Parallel I/O Port C[13] */
#define CIPR_PC12		0x00800000	/* Parallel I/O Port C[12] */
#define CIPR_SDMA		0x00400000	/* SDMA bus error */
#define CIPR_IDMA1		0x00200000	/* IDMA 1 */	
#define CIPR_IDMA2		0x00100000	/* IDMA 2 */	
#define CIPR_TIMER2		0x00040000	/* Timer 2 */
#define CIPR_R_TT		0x00020000	/* Risc Timer Table */
#define CIPR_I2C		0x00010000	/* I2C */
#define CIPR_PC11		0x00008000	/* Parallel I/O Port C[11] */
#define CIPR_PC10		0x00004000	/* Parallel I/O Port C[10] */
#define CIPR_TIMER3		0x00001000	/* Timer 3 */
#define CIPR_PC9		0x00000800	/* Parallel I/O Port C[9] */
#define CIPR_PC8		0x00000400	/* Parallel I/O Port C[8] */
#define CIPR_PC7		0x00000200	/* Parallel I/O Port C[7] */
#define CIPR_TIMER4		0x00000080	/* Timer 4 */
#define CIPR_PC6		0x00000040	/* Parallel I/O Port C[6] */
#define CIPR_SPI		0x00000020	/* SPI */
#define CIPR_SMC1		0x00000010	/* SMC 1 */
#define CIPR_SMC2_PIP		0x00000008	/* SMC 1 */
#define CIPR_PC5		0x00000004	/* Parallel I/O Port C[5] */
#define CIPR_PC4		0x00000002	/* Parallel I/O Port C[4] */

/* CPM Interrupt Mask Register (CIMR - 0x948) */

#define CIMR_PC15		0x80000000	/* Parallel I/O Port C[15] */
#define CIMR_SCC1		0x40000000	/* SCC 1 */
#define CIMR_SCC2		0x20000000	/* SCC 2 */
#define CIMR_SCC3		0x10000000	/* SCC 3 */
#define CIMR_SCC4		0x08000000	/* SCC 4 */
#define CIMR_PC14		0x04000000	/* Parallel I/O Port C[14] */
#define CIMR_TIMER1		0x02000000	/* Timer 1 */
#define CIMR_PC13		0x01000000	/* Parallel I/O Port C[13] */
#define CIMR_PC12		0x00800000	/* Parallel I/O Port C[12] */
#define CIMR_SDMA		0x00400000	/* SDMA bus error */
#define CIMR_IDMA1		0x00200000	/* IDMA 1 */	
#define CIMR_IDMA2		0x00100000	/* IDMA 2 */	
#define CIMR_TIMER2		0x00040000	/* Timer 2 */
#define CIMR_R_TT		0x00020000	/* Risc Timer Table */
#define CIMR_I2C		0x00010000	/* I2C */
#define CIMR_PC11		0x00008000	/* Parallel I/O Port C[11] */
#define CIMR_PC10		0x00004000	/* Parallel I/O Port C[10] */
#define CIMR_TIMER3		0x00001000	/* Timer 3 */
#define CIMR_PC9		0x00000800	/* Parallel I/O Port C[9] */
#define CIMR_PC8		0x00000400	/* Parallel I/O Port C[8] */
#define CIMR_PC7		0x00000200	/* Parallel I/O Port C[7] */
#define CIMR_TIMER4		0x00000080	/* Timer 4 */
#define CIMR_PC6		0x00000040	/* Parallel I/O Port C[6] */
#define CIMR_SPI		0x00000020	/* SPI */
#define CIMR_SMC1		0x00000010	/* SMC 1 */
#define CIMR_SMC2_PIP		0x00000008	/* SMC 1 */
#define CIMR_PC5		0x00000004	/* Parallel I/O Port C[5] */
#define CIMR_PC4		0x00000002	/* Parallel I/O Port C[4] */
#define CIMR_ALL		0xfff7defe	/* all interrupt masks */

/* CPM Interrupt in Service Register (CISR - 0x94c) */

#define CISR_PC15		0x80000000	/* Parallel I/O Port C[15] */
#define CISR_SCC1		0x40000000	/* SCC 1 */
#define CISR_SCC2		0x20000000	/* SCC 2 */
#define CISR_SCC3		0x10000000	/* SCC 3 */
#define CISR_SCC4		0x08000000	/* SCC 4 */
#define CISR_PC14		0x04000000	/* Parallel I/O Port C[14] */
#define CISR_TIMER1		0x02000000	/* Timer 1 */
#define CISR_PC13		0x01000000	/* Parallel I/O Port C[13] */
#define CISR_PC12		0x00800000	/* Parallel I/O Port C[12] */
#define CISR_SDMA		0x00400000	/* SDMA bus error */
#define CISR_IDMA1		0x00200000	/* IDMA 1 */	
#define CISR_IDMA2		0x00100000	/* IDMA 2 */	
#define CISR_TIMER2		0x00040000	/* Timer 2 */
#define CISR_R_TT		0x00020000	/* Risc Timer Table */
#define CISR_I2C		0x00010000	/* I2C */
#define CISR_PC11		0x00008000	/* Parallel I/O Port C[11] */
#define CISR_PC10		0x00004000	/* Parallel I/O Port C[10] */
#define CISR_TIMER3		0x00001000	/* Timer 3 */
#define CISR_PC9		0x00000800	/* Parallel I/O Port C[9] */
#define CISR_PC8		0x00000400	/* Parallel I/O Port C[8] */
#define CISR_PC7		0x00000200	/* Parallel I/O Port C[7] */
#define CISR_TIMER4		0x00000080	/* Timer 4 */
#define CISR_PC6		0x00000040	/* Parallel I/O Port C[6] */
#define CISR_SPI		0x00000020	/* SPI */
#define CISR_SMC1		0x00000010	/* SMC 1 */
#define CISR_SMC2_PIP		0x00000008	/* SMC 1 */
#define CISR_PC5		0x00000004	/* Parallel I/O Port C[5] */
#define CISR_PC4		0x00000002	/* Parallel I/O Port C[4] */

/* SCC - Serial Communication Controller */


/* General SCC Mode Register definitions  */

#define SCC_GSMRL_HDLC		0x00000000	/* HDLC mode */
#define SCC_GSMRL_APPLETALK	0x00000002	/* AppleTalk mode (LocalTalk) */
#define SCC_GSMRL_SS7		0x00000003	/* SS7 mode (microcode) */
#define SCC_GSMRL_UART		0x00000004	/* UART mode */
#define SCC_GSMRL_PROFI_BUS	0x00000005	/* Profi-Bus mode (microcode) */
#define SCC_GSMRL_ASYNC_HDLC	0x00000006	/* async HDLC mode (microcode)*/
#define SCC_GSMRL_V14		0x00000007	/* V.14 mode */
#define SCC_GSMRL_BISYNC	0x00000008	/* BISYNC mode */
#define SCC_GSMRL_DDCMP		0x00000009	/* DDCMP mode (microcode) */
#define SCC_GSMRL_ETHERNET	0x0000000c	/* ethernet mode (SCC1 only) */
#define SCC_GSMRL_ENT		0x00000010	/* enable transmitter */
#define SCC_GSMRL_ENR		0x00000020	/* enable receiver */
#define SCC_GSMRL_LOOPBACK	0x00000040	/* local loopback mode */
#define SCC_GSMRL_ECHO		0x00000080	/* automatic echo mode */
#define SCC_GSMRL_TENC		0x00000700	/* transmitter encoding method*/
#define SCC_GSMRL_RENC		0x00003800	/* receiver encoding method */
#define SCC_GSMRL_RDCR_X8	0x00004000	/* receive DPLL clock x8 */
#define SCC_GSMRL_RDCR_X16	0x00008000	/* receive DPLL clock x16 */
#define SCC_GSMRL_RDCR_X32	0x0000c000	/* receive DPLL clock x32 */
#define SCC_GSMRL_TDCR_X8	0x00010000	/* transmit DPLL clock x8 */
#define SCC_GSMRL_TDCR_X16	0x00020000	/* transmit DPLL clock x16 */
#define SCC_GSMRL_TDCR_X32	0x00030000	/* transmit DPLL clock x32 */
#define SCC_GSMRL_TEND		0x00040000	/* transmitter frame ending */
#define SCC_GSMRL_TPP_00	0x00180000	/* Tx preamble pattern = 00 */
#define SCC_GSMRL_TPP_10	0x00080000	/* Tx preamble pattern = 10 */
#define SCC_GSMRL_TPP_01	0x00100000	/* Tx preamble pattern = 01 */
#define SCC_GSMRL_TPP_11	0x00180000	/* Tx preamble pattern = 11 */
#define SCC_GSMRL_TPL_NONE	0x00000000	/* no Tx preamble (default) */
#define SCC_GSMRL_TPL_8		0x00200000	/* Tx preamble = 1 byte */
#define SCC_GSMRL_TPL_16	0x00400000	/* Tx preamble = 2 bytes */
#define SCC_GSMRL_TPL_32	0x00600000	/* Tx preamble = 4 bytes */
#define SCC_GSMRL_TPL_48	0x00800000	/* Tx preamble = 6 bytes */
#define SCC_GSMRL_TPL_64	0x00a00000	/* Tx preamble = 8 bytes */
#define SCC_GSMRL_TPL_128	0x00c00000	/* Tx preamble = 16 bytes */
#define SCC_GSMRL_TINV		0x01000000	/* DPLL transmit input invert */
#define SCC_GSMRL_RINV		0x02000000	/* DPLL receive input invert */
#define SCC_GSMRL_TSNC		0x0c000000	/* transmit sense */
#define SCC_GSMRL_TCI		0x10000000	/* transmit clock invert */
#define SCC_GSMRL_EDGE		0x60000000	/* adjustment edge +/- */

#define SCC_GSMRH_RSYN		0x00000001	/* receive sync timing*/
#define SCC_GSMRH_RTSM		0x00000002	/* RTS* mode */
#define SCC_GSMRH_SYNL		0x0000000c	/* sync length */
#define SCC_GSMRH_TXSY		0x00000010	/* transmitter/receiver sync */
#define SCC_GSMRH_RFW		0x00000020	/* Rx FIFO width */
#define SCC_GSMRH_TFL		0x00000040	/* transmit FIFO length */
#define SCC_GSMRH_CTSS		0x00000080	/* CTS* sampling */
#define SCC_GSMRH_CDS		0x00000100	/* CD* sampling */
#define SCC_GSMRH_CTSP		0x00000200	/* CTS* pulse */
#define SCC_GSMRH_CDP		0x00000400	/* CD* pulse */
#define SCC_GSMRH_TTX		0x00000800	/* transparent transmitter */
#define SCC_GSMRH_TRX		0x00001000	/* transparent receiver */
#define SCC_GSMRH_REVD		0x00002000	/* reverse data */
#define SCC_GSMRH_TCRC		0x0000c000	/* transparent CRC */
#define SCC_GSMRH_GDE		0x00010000	/* glitch detect enable */

/* SCC UART protocol specific parameters */

typedef struct		/* SCC_UART_PROTO */
    {
    UINT32	res1;			/* reserved */
    UINT32	res2;			/* reserved */
    UINT16	maxIdl;			/* maximum idle characters */
    UINT16	idlc;			/* temporary idle counter */
    UINT16	brkcr;			/* break count register (transmit) */
    UINT16	parec;			/* receive parity error counter */
    UINT16	frmer;			/* receive framing error counter */
    UINT16	nosec;			/* receive noise counter */
    UINT16	brkec;			/* receive break condition counter */
    UINT16	brkln;			/* last received break length */
    UINT16	uaddr1;			/* uart address character 1 */
    UINT16	uaddr2;			/* uart address character 2 */
    UINT16	rtemp;			/* temp storage */
    UINT16	toseq;			/* transmit out-of-sequence character */
    UINT16	character1;		/* control character 1 */
    UINT16	character2;		/* control character 2 */
    UINT16	character3;		/* control character 3 */
    UINT16	character4;		/* control character 4 */
    UINT16	character5;		/* control character 5 */
    UINT16	character6;		/* control character 6 */
    UINT16	character7;		/* control character 7 */
    UINT16	character8;		/* control character 8 */
    UINT16	rccm;			/* receive control character mask */
    UINT16	rccr;			/* receive control character register */
    UINT16	rlbc;			/* receive last break character */
    } SCC_UART_PROTO;

/* SCC UART Protocol Specific Mode Register definitions */

#define SCC_UART_PSMR_TPM_ODD	0x0000		/* odd parity mode (Tx) */
#define SCC_UART_PSMR_TPM_LOW	0x0001		/* low parity mode (Tx) */
#define SCC_UART_PSMR_TPM_EVEN	0x0002		/* even parity mode (Tx) */
#define SCC_UART_PSMR_TPM_HIGH	0x0003		/* high parity mode (Tx) */
#define SCC_UART_PSMR_RPM_ODD	0x0000		/* odd parity mode (Rx) */
#define SCC_UART_PSMR_RPM_LOW	0x0004		/* low parity mode (Rx) */
#define SCC_UART_PSMR_RPM_EVEN	0x0008		/* even parity mode (Rx) */
#define SCC_UART_PSMR_RPM_HIGH	0x000c		/* high parity mode (Rx) */
#define SCC_UART_PSMR_PEN	0x0010		/* parity enable */
#define SCC_UART_PSMR_DRT	0x0040		/* disable Rx while Tx */
#define SCC_UART_PSMR_SYN	0x0080		/* synchronous mode */
#define SCC_UART_PSMR_RZS	0x0100		/* receive zero stop bits */
#define SCC_UART_PSMR_FRZ	0x0200		/* freeze transmission */
#define SCC_UART_PSMR_UM_NML	0x0000		/* noraml UART operation */
#define SCC_UART_PSMR_UM_MULT_M	0x0400		/* multidrop non-auto mode */
#define SCC_UART_PSMR_UM_MULT_A	0x0c00		/* multidrop automatic mode */
#define SCC_UART_PSMR_CL_5BIT	0x0000		/* 5 bit character length */
#define SCC_UART_PSMR_CL_6BIT	0x1000		/* 6 bit character length */
#define SCC_UART_PSMR_CL_7BIT	0x2000		/* 7 bit character length */
#define SCC_UART_PSMR_CL_8BIT	0x3000		/* 8 bit character length */
#define SCC_UART_PSMR_SL	0x4000		/* 1 or 2 bit stop length */
#define SCC_UART_PSMR_FLC	0x8000		/* flow control */
	
/* SCC UART Event and Mask Register definitions */

#define SCC_UART_SCCX_RX 	0x0001		/* buffer received */
#define SCC_UART_SCCX_TX 	0x0002		/* buffer transmitted */
#define SCC_UART_SCCX_BSY 	0x0004		/* busy condition */
#define SCC_UART_SCCX_CCR 	0x0008		/* control character received */
#define SCC_UART_SCCX_BRK_S 	0x0020	 	/* break start */
#define SCC_UART_SCCX_BRK_E 	0x0040		/* break end */
#define SCC_UART_SCCX_GRA 	0x0080		/* graceful stop complete */
#define SCC_UART_SCCX_IDL	0x0100		/* idle sequence stat changed */
#define SCC_UART_SCCX_AB  	0x0200		/* autobaud lock */
#define SCC_UART_SCCX_GL_T 	0x0800		/* glitch on Tx */
#define SCC_UART_SCCX_GL_R 	0x1000		/* glitch on Rx */

/* SCC UART Receive Buffer Descriptor definitions */

#define SCC_UART_RX_BD_CD	0x0001		/* carrier detect loss */
#define SCC_UART_RX_BD_OV	0x0002		/* receiver overrun */
#define SCC_UART_RX_BD_PR	0x0008		/* parity error */
#define SCC_UART_RX_BD_FR	0x0010		/* framing error */
#define SCC_UART_RX_BD_BR	0x0020		/* break received */
#define SCC_UART_RX_BD_AM	0x0080		/* address match */
#define SCC_UART_RX_BD_ID	0x0100		/* buf closed on IDLES */
#define SCC_UART_RX_BD_CM	0x0200		/* continous mode */
#define SCC_UART_RX_BD_ADDR	0x0400		/* buffer contains address */
#define SCC_UART_RX_BD_CNT	0x0800		/* control character */
#define SCC_UART_RX_BD_INT	0x1000		/* interrupt generated */
#define SCC_UART_RX_BD_WRAP	0x2000		/* wrap back to first BD */
#define SCC_UART_RX_BD_EMPTY	0x8000		/* buffer is empty */

/* SCC UART Transmit Buffer Descriptor definitions */

#define SCC_UART_TX_BD_CT	0x0001		/* cts was lost during tx */
#define SCC_UART_TX_BD_NS	0x0080		/* no stop bit transmitted */
#define SCC_UART_TX_BD_PREAMBLE	0x0100		/* enable preamble */
#define SCC_UART_TX_BD_CM	0x0200		/* continous mode */
#define SCC_UART_TX_BD_ADDR	0x0400		/* buffer contains address */
#define SCC_UART_TX_BD_CTSR	0x0800		/* normal cts error reporting */
#define SCC_UART_TX_BD_INT	0x1000		/* interrupt generated */
#define SCC_UART_TX_BD_WRAP	0x2000		/* wrap back to first BD */
#define SCC_UART_TX_BD_READY	0x8000		/* buffer is being sent */

/* SCC Buffer */

typedef struct          /* SCC_BUF */
    {
    volatile UINT16	statusMode;	/* status and control */
    UINT16		dataLength;	/* length of data buffer in bytes */
    u_char *		dataPointer;	/* points to data buffer */
    } SCC_BUF;

/* SCC Parameters */

typedef struct          /* SCC_PARAM */
    {		                /* offset description*/
    volatile INT16      rbase;	/* 00 Rx buffer descriptor base address */
    volatile INT16      tbase;	/* 02 Tx buffer descriptor base address */
    volatile INT8       rfcr;	/* 04 Rx function code */
    volatile INT8       tfcr;	/* 05 Tx function code */
    volatile INT16      mrblr;	/* 06 maximum receive buffer length */
    volatile INT32      rstate;	/* 08 Rx internal state */
    volatile INT32      res1;	/* 0C Rx internal data pointer */
    volatile INT16      rbptr;	/* 10 Rx buffer descriptor pointer */
    volatile INT16      res2;	/* 12 reserved/internal */
    volatile INT32      res3;	/* 14 reserved/internal */
    volatile INT32      tstate;	/* 18 Tx internal state */
    volatile INT32      res4;	/* 1C reserved/internal */
    volatile INT16      tbptr;	/* 20 Tx buffer descriptor pointer */
    volatile INT16      res5;	/* 22 reserved/internal */
    volatile INT32      res6;	/* 24 reserved/internal */
    volatile INT32	rcrc;	/* 28 temp receive CRC */
    volatile INT32	tcrc;	/* 2C temp transmit CRC */
    } SCC_PARAM;
   
typedef struct          /* SCC */
    {
    SCC_PARAM   param;                  /* SCC parameters */
    char        prot[64];               /* protocol specific area */
    } SCC;

typedef struct          /* SCC_REG */
    {
    UINT32      	gsmrl;		/* SCC general mode register - low */
    UINT32      	gsmrh;  	/* SCC eneral mode register - high */
    UINT16      	psmr;   	/* SCC protocol mode register */
    UINT16      	res1;   	/* reserved */
    UINT16      	todr;   	/* SCC transmit on demand */
    UINT16      	dsr;    	/* SCC data sync. register */
    volatile UINT16     scce;   	/* SCC event register */
    UINT16      	res2;   	/* reserved */
    UINT16      	sccm;   	/* SCC mask register */
    UINT8       	res3;   	/* reserved */
    volatile UINT8      sccs;   	/* SCC status register */
    } SCC_REG;

/* PIP related definitions: */

#define CPM_PIP_STB         0x00010000	/* printer data-strobe control */
#define CPM_PIP_ACK         0x00020000	/* printer acknowledge status */
#define CPM_PIP_BSY         0x00000001	/* printer busy status */
#define CPM_PIP_SEL         0x00000002	/* printer select status */
#define CPM_PIP_PE          0x00000004	/* printer paper error status */
#define CPM_PIP_F           0x00000008	/* printer error/fault status */
#define CPM_PIP_DATA        0x0000ff00	/* data mask */

/* I2C related definitions */

/* I2C receive buffer descriptor control/status */

#define CPM_I2C_R_CS_E     0x8000   /* empty */
#define CPM_I2C_R_CS_W     0x2000   /* wrap, final BD in table */
#define CPM_I2C_R_CS_I     0x1000   /* interrupt */
#define CPM_I2C_R_CS_L     0x0800   /* buffer closed on start/stop/error */
#define CPM_I2C_R_CS_OV    0x0002   /* overrun */

/* I2C mode register bit definitions */

#define CPM_I2C_MODE_REVD            0x0020  /* reverse data */
#define CPM_I2C_MODE_GCD             0x0010  /* general call disable */
#define CPM_I2C_MODE_FLT             0x0008  /* clock filter */
#define CPM_I2C_MODE_PDIV_MASK       0x0006  /* predivider mask */
#define CPM_I2C_MODE_PDIV_BRGCLK32   0x0000  /* predivider - BRGCLK/32 */
#define CPM_I2C_MODE_PDIV_BRGCLK16   0x0002  /* predivider - BRGCLK/16 */
#define CPM_I2C_MODE_PDIV_BRGCLK8    0x0004  /* predivider - BRGCLK/8 */
#define CPM_I2C_MODE_PDIV_BRGCLK4    0x0006  /* predivider - BRGCLK/4 */
#define CPM_I2C_MODE_EN              0x0001  /* enable i2c */

/* I2C event/mask register bit definitions */

#define CPM_I2C_ER_TXE        0x0010  /* transmit error */
#define CPM_I2C_ER_BSY        0x0004  /* busy condition */
#define CPM_I2C_ER_TXB        0x0002  /* transmit buffer */
#define CPM_I2C_ER_RXB        0x0001  /* receive buffer */

/* I2C command register bit definitions */

#define CPM_I2C_COMMAND_STR  0x0080  /* start transmit */
#define CPM_I2C_COMMAND_MS   0x0001  /* master/slave mode */

/* I2C transmit buffer descriptor control/status */

#define CPM_I2C_T_CS_R       0x8000  /* ready */
#define CPM_I2C_T_CS_W       0x2000  /* wrap, final BD in table */
#define CPM_I2C_T_CS_I       0x1000  /* interrupt */
#define CPM_I2C_T_CS_L       0x0800  /* last buffer of message */
#define CPM_I2C_T_CS_S       0x0400  /* transmit start condition */
#define CPM_I2C_T_CS_NAK     0x0004  /* no acknowledge */
#define CPM_I2C_T_CS_UN      0x0002  /* underrun */
#define CPM_I2C_T_CS_CL      0x0001  /* collision */

#ifdef __cplusplus
}
#endif

#endif /* __INCppc860Cpmh */
