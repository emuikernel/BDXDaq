/* ns83902End.h - NIC driver header */

/* Copyright 1984-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,11apr00,zl   added more register definitions
01b,10apr00,zl   renamed file to ns83902End.h, cleaned up
01a,20jan99,zl   written based on nicEvbEnd.h
*/

#ifndef __INCns83902Endh
#define __INCns83902Endh

#ifdef __cplusplus
extern "C" {
#endif

/* driver flags */

typedef UINT8 NS83902_DRV_FLAG;

#define NS83902_FLAG_POLL	0x01	/* for poll mode */    

#define NS83902_WIDE_MASK	0x01	/* option bit 0 (LSB) */
#define NS83902_INTERVAL_MASK	0x02	/* option bit 1 */
#define MINPKTSIZE		64	/* Minimum packet size */
#define NS83902_PAGE_SIZE	256	/* Page size in buffer memory */
#define NS83902_TX_POOL_SIZE	1514	/* TX pool size */

/* Page 0 Internal register offsets */

#define NS83902_CR	0x00		/* R/W: Command Register R/W */
#define NS83902_PSTART	0x01		/* W: Page Start Register */
#define NS83902_CLDA0	0x01		/* R: Current Local DMA Address0 */
#define NS83902_PSTOP	0x02		/* W: Page Stop Register */
#define NS83902_CLDA1	0x02		/* R: Current Local DMA Address1 */
#define NS83902_BNRY	0x03		/* R/W: Boundary Pointer Register */
#define NS83902_TPSR	0x04		/* W: Transmit Page Start Register */
#define NS83902_TSR	0x04		/* R: Transmit Status Register */
#define NS83902_TBCR0	0x05		/* W: Transmit Byte Count0 Register */
#define NS83902_NCR	0x05		/* R: Number of Collisions */
#define NS83902_TBCR1	0x06		/* W: Transmit Byte Count1 Register */
#define NS83902_FIFO	0x06		/* R: FIFO Register */
#define NS83902_ISR	0x07		/* R/W: Interrup Status Register */
#define NS83902_RSAR0	0x08		/* W: Remote Start Address0 Register */
#define NS83902_CRDA0	0x08		/* R: Current Remote DMA Address0 Reg.*/
#define NS83902_RSAR1	0x09		/* W: Remote Start Address1 Register */
#define NS83902_CRDA1	0x09		/* R: Current Remote DMA Address1 Reg.*/
#define NS83902_RBCR0	0x0A		/* W: Remote Byte Count0 Register */
#define NS83902_RBCR1	0x0B		/* W: Remote Byte Count1 Register */
#define NS83902_RCR	0x0C		/* W: Receive Configuration Register */
#define NS83902_RSR	0x0C		/* R: Receive Status Register */
#define NS83902_TCR	0x0D		/* W: Transmit Configuration Register */
#define NS83902_CNTR0	0x0D		/* R: Tally Counter 0 (Frm. Alignment.*/
#define NS83902_DCR	0x0E		/* W: Data Configuration Register */
#define NS83902_CNTR1	0x0E		/* R: Tally Counter 1 (CRC) */
#define NS83902_IMR	0x0F		/* W: Interrupt Mask Register */
#define NS83902_CNTR2	0x0F		/* R: Tally Counter 1 (Missed Pkt.)*/

/* Page 1 Internal Register Offsets */

#define NS83902_CR	0x00		/* Command register */
#define NS83902_PAR0	0x01		/* Phsical Address Register 0 */
#define NS83902_PAR1	0x02		/* Phsical Address Register 1 */
#define NS83902_PAR2	0x03		/* Phsical Address Register 2 */
#define NS83902_PAR3	0x04		/* Phsical Address Register 3 */
#define NS83902_PAR4	0x05		/* Phsical Address Register 4 */
#define NS83902_PAR5	0x06		/* Phsical Address Register 5 */
#define NS83902_CURR	0x07		/* Current Page Register */
#define NS83902_MAR0	0x08		/* Multicast Address Register 0 */
#define NS83902_MAR1	0x09		/* Multicast Address Register 1 */
#define NS83902_MAR2	0x0a		/* Multicast Address Register 2 */
#define NS83902_MAR3	0x0b		/* Multicast Address Register 3 */
#define NS83902_MAR4	0x0c		/* Multicast Address Register 4 */
#define NS83902_MAR5	0x0d		/* Multicast Address Register 5 */
#define NS83902_MAR6	0x0e		/* Multicast Address Register 6 */    
#define NS83902_MAR7	0x0f		/* Multicast Address Register 7 */


/*
 * Configuration Register bits
 */

#define CR_STP		0x1		/* Stop */
#define CR_STA		0x2		/* Start */
#define CR_TXP		0x4		/* Transmit Packet command */
#define CR_RREAD	0x8		/* Remote DMA Read command */
#define CR_RWRITE	0x10		/* Remote DMA Write command */
#define CR_SPKT		0x18		/* Send Packet command */
#define CR_ABORT	0x20		/* Abort command */
#define CR_RPAGE0	0x00		/* Register Page 0 */
#define CR_RPAGE1	0x40		/* Register Page 1 */
#define CR_RPAGE2	0x80		/* Register Page 2 */
#define CR_PAGE_MASK	0xc0		/* Register Page mask */

/*
 * Interrupt Status Register
 */

#define ISR_PRX		0x1		/* Packet Received */
#define ISR_PTX		0x2		/* Packet Transmitted */
#define ISR_RXE		0x4		/* Receive Error */
#define ISR_TXE		0x8		/* Transmit Error */
#define ISR_OVW		0x10		/* Overwrite Warning */
#define ISR_CNT		0x20		/* Counter Overflow */
#define ISR_RDC		0x40		/* Remote DMA complete */
#define ISR_RST		0x80		/* Reset Status */

/*
 * Interrupt Mode Register
 */

#define IMR_PRXE	0x1		/* Packet Received Intr. Enable */
#define IMR_PTXE	0x2		/* Packet Transmitted Intr. Enable */
#define IMR_RXEE	0x4		/* Receive Error Intr. Enable */
#define IMR_TXEE	0x8		/* Transmit Error Intr. Enable */
#define IMR_OVWE	0x10		/* Overwrite Warning Intr. Enable */
#define IMR_CNTE	0x20		/* Counter Overflow Intr. Enable */
#define IMR_RDCE	0x40		/* Remote DMA complete Intr. Enable */

/*
 * Data Configuration Register
 */

#define DCR_WTS		0x1		/* Word Transfer Select */
#define DCR_BOS		0x2		/* Byte Order Select */
#define DCR_LAS		0x4		/* Long Address Select */
#define DCR_NOTLS	0x8		/* Loopback Select */
#define DCR_ARM		0x10		/* Auto-Initialize Select */
#define DCR_FIFO2	0x00		/* Fifo Threshold 1 word  /2 bytes */
#define DCR_FIFO4	0x20		/* Fifo Threshold 2 words / 4 bytes */
#define DCR_FIFO8	0x40		/* Fifo Threshold 4 words / 8 bytes */
#define DCR_FIFO12	0x60		/* Fifo Threshold 6 words / 12 bytes */

/*
 * Transmit Configuration Register
 */

#define TCR_CRC 	0x1		/* Inhibit CRC */
#define TCR_MODE0	0x0		/* Norman operation */
#define TCR_MODE1	0x2		/* Internal NIC Module Loopback */
#define TCR_MODE2	0x4		/* Internal ENDEC Module Loopback */
#define TCR_MODE3	0x6		/* External Loopback */
#define TCR_ATD		0x8		/* Auto Transmit Disable */
#define TCR_OFST	0x10		/* Collision Offset Enable */

/*
 * Transmit Status Register
 */

#define TSR_PTX		0x1		/* Packet transmitted */
#define TSR_COL		0x4		/* Transmit Collided */
#define TSR_ABT		0x8		/* Transmit Aborted */
#define TSR_CRS		0x10		/* Carrier Sense Lost */
#define TSR_FU		0x20		/* FIFO underrun */
#define TSR_CDH		0x40		/* CD Heartbeat */
#define TSR_OWC		0x80		/* Out of Window Collision */

/*
 * Receive Configuration Register
 */

#define	RCR_SEP		0x1		/* Save Errored Packets */
#define RCR_AR		0x2		/* Accept Runt Packets */
#define RCR_AB		0x4		/* Acept Broadcast */
#define RCR_AM		0x8		/* Accept Multicast */
#define RCR_PRO		0x10		/* Promiscuous Physical */
#define RCR_MON		0x20		/* Monitor Mode */

/*
 * Receive Status Register
 */

#define RSR_PRX		0x1		/* Packet Received Intact */
#define RSR_CRC		0x2		/* CRC Error */
#define RSR_FAE		0x4		/* Frame Alignment Error */
#define RSR_FO		0x8		/* FIFO Overrun */
#define RSR_MPA		0x10		/* Missed Packet */
#define RSR_PHY		0x20		/* Physical/Multicast Address */
#define RSR_DIS		0x40		/* Receiver Disabled */
#define RSR_DFR		0x80		/* Deferring */

#ifdef __cplusplus
}
#endif

#endif /* __INCns83902Endh */
