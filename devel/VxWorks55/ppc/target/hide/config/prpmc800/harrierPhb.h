/* @(#)harrierPhb.h	5.1 08/14/00 /wrk/VxWorks/ppmc/src/./target/config/ppmc750/s.harrierPhb.h */
/* harrierPhb.h - Motorola Harrier PCI-Host Bridge header */

/* Copyright 1999,2001 Wind River Systems, Inc. */
/* Copyright 1999,2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,09nov99,rhv  Written (using symbols from version 01b of ppmc750/ppmc750.h)

*/

/*
This file contains I/O addresses and related constants for the
Motorola PCI-Host bridge portion of the Harrier ASIC.
*/

#ifndef	INCharrierPhbh
#define	INCharrierPhbh

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Base address of harrier PHB registers as seen from CPU.
 *
 * Note: The HARRIER ASIC is a combined FALCON/RAVEN. So the
 * HARRIER PHB (PCI Host Bridge) is equivalent to the RAVEN.
 */

#define HARRIER_PHB_BASE_ADRS	0xfeff0000
#define	HARRIER_PHB_REG_SIZE	0x00010000

/* this is the smallest PCU->PCI and PCI->CPU window supported by the Harrier */

#define HARRIER_PHB_MIN_WINDOW_SIZE   0x10000	/* 64KB */

/* PCI Space Definitions  -- For configuring the Harrier */

#define CPU2PCI_ATTR_REN     (1<<7)  /* Read enable */
#define CPU2PCI_ATTR_WEN     (1<<6)  /* Write enable */
#define CPU2PCI_ATTR_WPEN    (1<<4)  /* Write post enable */
#define CPU2PCI_ATTR_MEM     (1<<1)  /* PCI memory cycle */
#define CPU2PCI_ATTR_IOM     (1<<0)  /* PCI I/O mode */

/* Harrier Slave Window Attributes */

#define HARRIER_PCI_SLV_ATTR_REN_BIT   7	/* allow reads from PCI bus */
#define HARRIER_PCI_SLV_ATTR_WEN_BIT   6	/* allow writes from PCI bus */
#define HARRIER_PCI_SLV_ATTR_WPEN_BIT  5	/* enable write posting */
#define HARRIER_PCI_SLV_ATTR_RAEN_BIT  4	/* enable read-ahead */
#define HARRIER_PCI_SLV_ATTR_GBL_BIT   1	/* mark read and write cycles global */
#define HARRIER_PCI_SLV_ATTR_INV_BIT   0	/* enable invalidate transactions */

#define HARRIER_PCI_SLV_ATTR_REN_MASK	(1<<HARRIER_PCI_SLV_ATTR_REN_BIT)
#define HARRIER_PCI_SLV_ATTR_WEN_MASK	(1<<HARRIER_PCI_SLV_ATTR_WEN_BIT)
#define HARRIER_PCI_SLV_ATTR_WPEN_MASK	(1<<HARRIER_PCI_SLV_ATTR_WPEN_BIT)
#define HARRIER_PCI_SLV_ATTR_RAEN_MASK	(1<<HARRIER_PCI_SLV_ATTR_RAEN_BIT)
#define HARRIER_PCI_SLV_ATTR_GBL_MASK	(1<<HARRIER_PCI_SLV_ATTR_GBL_BIT)
#define HARRIER_PCI_SLV_ATTR_INV_MASK	(1<<HARRIER_PCI_SLV_ATTR_INV_BIT)

/*
 * Harrier Extensions to Standard PCI Header
 *
 * Type declarations for the PCI Header and the macros in regards to the
 * PCI BUS.  These definitions have been made with respect to PCI LOCAL
 * BUS SPECIFICATION REVISION 2.1.  Every device on the PCI BUS has to
 * support 256 bytes of configuration space of which the first 64 bytes
 * are a predefined header portion defined by the PCI commitee.  Bytes
 * 64 to 255 are dedicated to the device specific registers.
 *
 * Note: the PCI bus is inherently little endian.
 */

#define PCI_CFG_HARRIER_PSADD0	0x80
#define PCI_CFG_HARRIER_PSATT0	0x84
#define PCI_CFG_HARRIER_PSOFF0	0x86
#define PCI_CFG_HARRIER_PSADD1	0x88
#define PCI_CFG_HARRIER_PSATT1	0x8c
#define PCI_CFG_HARRIER_PSOFF1	0x8e
#define PCI_CFG_HARRIER_PSADD2	0x90
#define PCI_CFG_HARRIER_PSATT2	0x94
#define PCI_CFG_HARRIER_PSOFF2	0x96
#define PCI_CFG_HARRIER_PSADD3	0x98
#define PCI_CFG_HARRIER_PSATT3	0x9c
#define PCI_CFG_HARRIER_PSOFF3	0x9e

/* Harrier MPC register offsets */

#define HARRIER_MPC_VENID		0x00
#define HARRIER_MPC_DEVID		0x02
#define HARRIER_MPC_REVID		0x05
#define HARRIER_MPC_GCSR		0x08
#define HARRIER_MPC_FEAT		0x0a
#define HARRIER_MPC_MARB		0x0c
#define HARRIER_MPC_HCSR		0x10
#define HARRIER_MPC_PADJ		0x13
#define HARRIER_MPC_MEREN		0x22
#define HARRIER_MPC_MERST		0x27
#define HARRIER_MPC_MERAD		0x28
#define HARRIER_MPC_MERAT		0x2e
#define HARRIER_MPC_PIACK		0x30
#define HARRIER_MPC_MSADD0		0x40
#define HARRIER_MPC_MSOFF0		0x44
#define HARRIER_MPC_MSATT0		0x47
#define HARRIER_MPC_MSADD1		0x48
#define HARRIER_MPC_MSOFF1		0x4c
#define HARRIER_MPC_MSATT1		0x4f
#define HARRIER_MPC_MSADD2		0x50
#define HARRIER_MPC_MSOFF2		0x54
#define HARRIER_MPC_MSATT2		0x57
#define HARRIER_MPC_MSADD3		0x58
#define HARRIER_MPC_MSOFF3		0x5c
#define HARRIER_MPC_MSATT3		0x5f
#define HARRIER_MPC_WDT1CNTL	0x60
#define HARRIER_MPC_WDT2CNTL	0x68
#define HARRIER_MPC_GPREG0_U	0x70
#define HARRIER_MPC_GPREG0_L	0x74
#define HARRIER_MPC_GPREG1_U	0x78
#define HARRIER_MPC_GPREG1_L	0x7c

/*
 * Harrier register bit masks
 *
 * Bits marked with 'C' indicate conditions which can be cleared by
 * writing a 1 to the bits.
 */

/* Harrier MPC Hardware Control/Status register bit masks */

#define HARRIER_MPC_HCSR_XPR_MASK	0x07	/* isolates the 60x/pci freq ratio */

#define HARRIER_MPC_HCSR_SPRQ_BIT	11	/* PCI speculative request control */

/* Harrier MPC Error Enable (MEREN) register bit masks */

#define HARRIER_MPC_MEREN_RTAI	0x0001	/* PCI mstr Recvd Target Abort Int */
#define HARRIER_MPC_MEREN_SMAI	0x0002	/* PCI mstr Signld Target Abort Int */
#define HARRIER_MPC_MEREN_SERRI	0x0004	/* PCI System Error Int */
#define HARRIER_MPC_MEREN_PERRI	0x0008	/* PCI Parity Error Int */
#define HARRIER_MPC_MEREN_XDPEI    0x0010  /* MPC Data Parity Error Int */
#define HARRIER_MPC_MEREN_MATOI	0x0020	/* MPC Address Bus Time-out Int */
#define HARRIER_MPC_MEREN_RTAM	0x0100	/* RTAI machine check enable */
#define HARRIER_MPC_MEREN_SMAM	0x0200	/* SMAI machine check enable */
#define HARRIER_MPC_MEREN_SERRM	0x0400	/* SERRI machine check enable */
#define HARRIER_MPC_MEREN_PERRM	0x0800	/* PERRI machine check enable */
#define HARRIER_MPC_MEREN_XDPEM	0x1000	/* XDPEI machine check enable */
#define HARRIER_MPC_MEREN_MATOM	0x2000	/* MATOI machine check enable */
#define HARRIER_MPC_MEREN_DFLT	0x4000	/* Default MPC Master ID select */
#define HARRIER_MPC_MEREN_VALID	0x7F3F	/* Mask for valid MEREN bits */

/* Harrier MPC Error Status (MERST) register bit masks */

#define HARRIER_MPC_MERST_RTA	0x01	/* C PCI mstr Recvd Target Abort */
#define HARRIER_MPC_MERST_SMA	0x02	/* C PCI mstr Signld Target Abort */
#define HARRIER_MPC_MERST_SERR	0x04	/* C PCI System Error */
#define HARRIER_MPC_MERST_PERR	0x08	/* C PCI Parity Error */
#define HARRIER_MPC_MERST_XDPE	0x10	/* C MPC Data Parity Error */
#define HARRIER_MPC_MERST_MATO	0x20	/* C MPC Address Bus Time-out */
#define HARRIER_MPC_MERST_OVF	0x80	/* C Error Status Overflow */
#define HARRIER_MPC_MERST_VALID	0xBF	/*   Mask for valid MERST bits */
#define HARRIER_MPC_MERST_CLR	0xBF	/*   Clears all errors */

/* Harrier PCI Configuration Status register bit masks */

#define HARRIER_PCI_CFG_STATUS_FAST    0x0010  /*   Fast back-to-back capable */
#define HARRIER_PCI_CFG_STATUS_DPAR    0x0100  /* C Data prity error detected */
#define HARRIER_PCI_CFG_STATUS_SELTIM0 0x0200  /*   Device select timing bit 0 */
#define HARRIER_PCI_CFG_STATUS_SELTIM1 0x0400  /*   Device select timing bit 1 */
#define HARRIER_PCI_CFG_STATUS_SIGTA   0x0800  /* C Signalled Target Abort */
#define HARRIER_PCI_CFG_STATUS_RCVTA   0x1000  /* C Received Target Abort */
#define HARRIER_PCI_CFG_STATUS_RCVMA   0x2000  /* C Received Master Abort */
#define HARRIER_PCI_CFG_STATUS_SIGSE   0x4000  /* C Signalled System Error */
#define HARRIER_PCI_CFG_STATUS_RCVPE   0x8000  /* C Detected Parity Error */
#define HARRIER_PCI_CFG_STATUS_VALID   0xFF10  /*   Valid status bits */
#define HARRIER_PCI_CFG_STATUS_CLR     (HARRIER_PCI_CFG_STATUS_DPAR  | \
                                     HARRIER_PCI_CFG_STATUS_SIGTA | \
                                     HARRIER_PCI_CFG_STATUS_RCVTA | \
                                     HARRIER_PCI_CFG_STATUS_RCVMA | \
                                     HARRIER_PCI_CFG_STATUS_SIGSE | \
                                     HARRIER_PCI_CFG_STATUS_RCVPE)

/* Harrier PCI Configuration Command register bit masks */

#define HARRIER_PCI_CFG_CMD_IOSP       0x0001  /* IO Space Enable */
#define HARRIER_PCI_CFG_CMD_MEMSP      0x0002  /* Memory Space Enable */
#define HARRIER_PCI_CFG_CMD_MSTR       0x0004  /* Bus Master Enable */
#define HARRIER_PCI_CFG_CMD_PERR       0x0040  /* Parity Err Response Enable */
#define HARRIER_PCI_CFG_CMD_SERR       0x0100  /* System Error Enable */

#ifdef __cplusplus
}
#endif

#endif /* INCharrierPhbh */
