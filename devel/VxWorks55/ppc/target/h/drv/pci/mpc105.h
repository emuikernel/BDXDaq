/* mpc105.h - Motorola MPC105 PCI Bridge/Memory Controller */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,10oct96,tam  added missing macros.
01a,03apr96,tpr  written.
*/

/*
This file contains the related constants for the MPC105.
*/

#ifndef __INCmpc105h
#define __INCmpc105h

#ifdef __cplusplus
extern "C" {
#endif

#define MPC105_VEND_ID		0x00	/* Vendor ID Reg */
#define MPC105_DEV_ID		0x02	/* Device ID Reg */
#define MPC105_CMD		0x04	/* Pci Command Reg */
#define MPC105_STAT		0x06	/* Pci Status Reg */
#define MPC105_PMCR		0x70	/* Power Management Configuration Reg */
#define MPC105_MSAR1		0x80	/* Memory Starting Address Register 1 */
#define MPC105_MSAR2		0x84	/* Memory Starting Address Register 2 */
#define MPC105_EMSAR1		0x88	/* Extended Memory Start Addr Reg 1 */
#define MPC105_EMSAR2		0x8c	/* Extended Memory Start Addr Reg 2 */
#define MPC105_MEAR1		0x90	/* Memory Ending Address Register 1 */
#define MPC105_MEAR2		0x94	/* Memory Ending Address Register 2 */
#define MPC105_EMEAR1		0x98	/* Extended Memory Ending Addr Reg 1 */
#define MPC105_EMEAR2		0x9c	/* Extended Memory Ending Addr Reg 2 */
#define MPC105_MBER		0xa0	/* Memory Bank Enable Register */
#define MPC105_PICR1		0xa8	/* Processor Interface Config. Reg 1 */
#define MPC105_PICR2		0xac	/* Processor Interface Config. Reg 2 */
#define MPC105_OS_VPR1		0xba	/* OS Visible Parameters Register 1 */	
#define MPC105_OS_VPR2		0xbb	/* OS Visible Parameters Register 2 */	
#define MPC105_ERR_EN_R1	0xc0	/* Error Enable Register 1 */
#define MPC105_ERR_DR1		0xc1	/* Error Detection Register 1 */
#define MPC105_60X_BESR		0xc3	/* 60x Bus Error Status Register */
#define MPC105_ERR_EN_R2	0xc4	/* Error Enable Register 2 */
#define MPC105_ERR_DR2		0xc5	/* Error Detection Register 2 */
#define MPC105_PCI_BESR		0xc7	/* PCI Bus Error Status Register */
#define MPC105_60XPCI_EAR	0xc8	/* 60x/PCI Error Address Register */
#define MPC105_MCCR1		0xf0	/* Memory Control Configuration Reg 1 */
#define MPC105_MCCR2		0xf4	/* Memory Control Configuration Reg 2 */
#define MPC105_MCCR3		0xf8	/* Memory Control Configuration Reg 3 */
#define MPC105_MCCR4		0xfc	/* Memory Control Configuration Reg 4 */

/* External Configuration Registers - Address Map A */

#define MPC105_ECR1		0x80000092 /* External Configuration Reg 1 */
#define MPC105_ECR2		0x8000081c /* External Configuration Reg 2 */
#define MPC105_ECR3		0x80000850 /* External Configuration Reg 3 */

/* Other registers are accessed indirectly via the registers below */

#define	MPC105_REG_SELECT	(int *)0x80000cf8 /* register select */
#define	MPC105_REG_DATA		(int *)0x80000cfc /* register data */
#define	MPC105_REG(nn)		((nn << 24) | 0x80)

/* Pci Command Reg (CMD) - 0x04 */

#define CMD_SERR		0x0100	/* enable SERR signal */
#define CMD_PAR_ERR		0x0040	/* respond to parity errors */
#define CMD_PCI_MASTER		0x0004	/* generate pci mem. accesses */
#define CMD_PCI_SLAVE		0x0002	/* respond to pci mem. accesses */

/* Pci Status Reg (STAT) - 0x6 */

#define STAT_PAR_ERR		0x8000	/* parity error detected */
#define STAT_SERR		0x4000	/* SERR is asserted */
#define STAT_MASTER_MST_ABORT	0x2000	/* transaction using master-abort */
#define STAT_MASTER_TGT_ABORT	0x1000	/* transaction using master-abort */
#define STAT_TARGET_TGT_ABORT	0x0800	/* transaction using master-abort */
#define STAT_DATA_PAR_ERR	0x0100	/* data parity error detected */
#define STAT_NO_RSV_BITS	0xf900	/* non reserved or non hardwired bits */


/* Power Management Configuration Register (PMCR) - 0x70 */

#define PMCR_NO_NAP_MSG		0x8000	/* HALT command broadcast */
#define PMCR_NO_SLEEP_MSG	0x4000	/* Sleep message broadcast */
#define PMCR_SLEEP_MSG_TYPE	0x2000	/* Sleep message type */
#define PMCR_LP_REF_EN		0x1000	/* Low-power refresh enable */
#define PMCR_NO_604_RUN		0x0800	/* Do not assert QACK (PPC604 only) */
#define PMCR_601_NEED_QREQ	0x0400	/* QREQ requested (PPC601 only) */
#define PMCR_SUSP_QACK		0x0200	/* Suspend mode will assert QACK */
#define PMCR_PM			0x0080	/* Power Management enable */
#define PMCR_DOZE		0x0020	/* Doze mode enable */
#define PMCR_NAP		0x0010	/* Nap mode enable */
#define PMCR_SLEEP		0x0008	/* Sleep mode enable */
#define PMCR_CKO_EN		0x0004	/* test clock output driver enable */
#define PMCR_CKO_SEL		0x0002	/* Selects the clock source */
#define PMCR_BR1_WAKE		0x0001	/* Enable awareness of a second CPU */

/* Memory Starting Address Register 1 - 0x80 */

#define MSAR1_BANK3_MSK		0xff000000	/* Starting addr for bank 3 */
#define MSAR1_BANK2_MSK		0x00ff0000	/* Starting addr for bank 2 */
#define MSAR1_BANK1_MSK		0x0000ff00	/* Starting addr for bank 1 */
#define MSAR1_BANK0_MSK		0x000000ff	/* Starting addr for bank 0 */

/* Memory Starting Address Register 2 - 0x84 */

#define MSAR2_BANK7_MSK		0xff000000	/* Starting addr for bank 7 */
#define MSAR2_BANK6_MSK		0x00ff0000	/* Starting addr for bank 6 */
#define MSAR2_BANK5_MSK		0x0000ff00	/* Starting addr for bank 5 */
#define MSAR2_BANK4_MSK		0x000000ff	/* Starting addr for bank 4 */

/* Extended Memory Start Address Register 1 - 0x88 */

#define EMSAR1_BANK3_MSK	0x03000000	/* Extended starting addr 3 */ 
#define EMSAR1_BANK2_MSK	0x00030000	/* Extended starting addr 2 */ 
#define EMSAR1_BANK1_MSK	0x00000300	/* Extended starting addr 1 */ 
#define EMSAR1_BANK0_MSK	0x00000003	/* Extended starting addr 0 */ 

/* Extended Memory Start Address Register 2 - 0x8c */

#define EMSAR2_BANK7_MSK	0x03000000	/* Extended starting addr 7 */ 
#define EMSAR2_BANK6_MSK	0x00030000	/* Extended starting addr 6 */ 
#define EMSAR2_BANK5_MSK	0x00000300	/* Extended starting addr 5 */ 
#define EMSAR2_BANK4_MSK	0x00000003	/* Extended starting addr 4 */ 

/* Memory Ending Address Register 1 - 0x90 */

#define MEAR1_BANK3_MSK         0xff000000      /* Ending addr for bank 3 */
#define MEAR1_BANK2_MSK         0x00ff0000      /* Ending addr for bank 2 */
#define MEAR1_BANK1_MSK         0x0000ff00      /* Ending addr for bank 1 */
#define MEAR1_BANK0_MSK         0x000000ff      /* Ending addr for bank 0 */

/* Memory Ending Address Register 2 - 0x94 */

#define MEAR2_BANK7_MSK         0xff000000      /* Ending addr for bank 7 */
#define MEAR2_BANK6_MSK         0x00ff0000      /* Ending addr for bank 6 */
#define MEAR2_BANK5_MSK         0x0000ff00      /* Ending addr for bank 5 */
#define MEAR2_BANK4_MSK         0x000000ff      /* Ending addr for bank 4 */

/* Extended Memory Ending Address Register 1 - 0x98 */

#define EMEAR1_BANK3_MSK	0x03000000	/* Extended ending addr 3 */ 
#define EMEAR1_BANK2_MSK	0x00030000	/* Extended ending addr 2 */ 
#define EMEAR1_BANK1_MSK	0x00000300	/* Extended ending addr 1 */ 
#define EMEAR1_BANK0_MSK	0x00000003	/* Extended ending addr 0 */ 

/* Extended Memory Ending Address Register 2 - 0x9c */

#define EMEAR2_BANK7_MSK	0x03000000	/* Extended ending addr 7 */ 
#define EMEAR2_BANK6_MSK	0x00030000	/* Extended ending addr 6 */ 
#define EMEAR2_BANK5_MSK	0x00000300	/* Extended ending addr 5 */ 
#define EMEAR2_BANK4_MSK	0x00000003	/* Extended ending addr 4 */ 

/* Memory Bank Enable Register - 0xa0 */

#define MBER_BANK7		0x80		/* Enable/disable bank 7 */
#define MBER_BANK6		0x40		/* Enable/disable bank 6 */
#define MBER_BANK5		0x20		/* Enable/disable bank 5 */
#define MBER_BANK4		0x10		/* Enable/disable bank 4 */
#define MBER_BANK3		0x08		/* Enable/disable bank 3 */
#define MBER_BANK2		0x04		/* Enable/disable bank 2 */
#define MBER_BANK1		0x02		/* Enable/disable bank 1 */
#define MBER_BANK0		0x01		/* Enable/disable bank 0 */

/* Processor Interface Configuration Register 1 (PICR1) - 0xa8 */

#define PICR1_CF_CBA_MASK	0xff000000	/* L2 copy-back address mask */
#define PICR1_CF_CBA(n)		((n)<<24)
#define PICR1_CF_BREAD_WS(n)	((n)<<22)	/* Burst read wait states */
#define PICR1_CF_CACHE_1G	0x00200000	/* L2 cache 0-1Gbyte only */
#define PICR1_RCS0 		0x00100000	/* ROM location: PCI or 60x */
#define PICR1_XIO_MODE 		0x00080000	/* Discontigous IO mode map A */
#define PICR1_PROC_TYPE_MASK 	0x00060000	/* Processor type */
#define PICR1_PROC_TYPE_601 	0x00000000	/* Processor type */
#define PICR1_PROC_TYPE_603 	0x00040000	/* Processor type */
#define PICR1_PROC_TYPE_604 	0x00060000	/* Processor type */
#define PICR1_XATS		0x00010000	/* Address map: A or B */
#define PICR1_CF_MP_ID		0x00008000	/* Multiprocessor id */
#define PICR1_CF_LBA_EN		0x00002000	/* Local bus slave access en */
#define PICR1_FLASH_WR_EN	0x00001000	/* Flash writes enable */
#define PICR1_MCP_EN		0x00000800	/* Machine check enable */
#define PICR1_TEA_EN		0x00000400	/* Transfer error enable */ 
#define PICR1_CF_DPARK		0x00000200	/* 60x parked on data bus */
#define PICR1_NO_PORT_REGS	0x00000080	/* No ext configuration reg */ 
#define PICR1_ST_GATH_EN	0x00000040	/* Store gathering of writes */
#define PICR1_LE_MODE		0x00000020	/* Little-Endian mode */
#define PICR1_CF_LOOP_SNOOP	0x00000010	/* Snoop looping enable */
#define PICR1_CF_APARK		0x00000008	/* 60x parked on address bus */
#define PICR1_SPEC_PCI_READ	0x00000004	/* Speculative PCI reads en */
#define PICR1_CF_L2_CACHE_MASK	0x00000003	/* L2/multiprocessor config. */
#define	PICR1_CF_L2_NOCACHE	0x00000000
#define	PICR1_CF_L2_WRITE_THROUGH 0x00000001
#define	PICR1_CF_L2_COPY_BACK	0x00000002
#define	PICR1_CF_L2_MP		0x00000003


/* Processor Interface Configuration Register 2 (PICR2) - 0xac */

#define PICR2_L2_UPDATE_EN	    0x80000000	/* Update cache misses */
#define PICR2_L2_EN		    0x40000000	/* L2 cache enable */
#define PICR2_CF_FLUSH_L2	    0x10000000	/* Initiate L2 cache flush */
#define PICR2_CF_BYTE_DECODE	    0x02000000	/* on-chip byte-write decode */
#define PICR2_CF_FAST_L2_MODE	    0x01000000	/* Fast L2 mode timing */
#define PICR2_CF_DATA_RAM_TYPE	    0x00c00000	/* L2 data RAM type */
#define	PICR2_CF_DATA_RAM_SYNC	    0x00000000
#define	PICR2_CF_DATA_RAM_ASYNC	    0x00800000
#define PICR2_CF_WMODE		    0x00300000	/* SRAM write timing */
#define	PICR2_CF_WMODE_NORMAL	    0x00100000
#define	PICR2_CF_WMODE_DELAY	    0x00200000
#define	PICR2_CF_WMODE_EARLY	    0x00300000
#define PICR2_CF_SNOOP_WS(n)	    ((n)<<18)	/* Snoop wait states */
#define PICR2_CF_MOD_HIGH	    0x00020000	/* Cache mod. signal polarity */
#define PICR2_CF_HIT_HIGH	    0x00010000	/* Cache HIT signal polarity */
#define PICR2_CF_ADDR_ONLY_DISABLE  0x00004000	/* L2 normal operation */
#define PICR2_CF_HOLD		    0x00002000	/* L2 tag address hold */
#define PICR2_CF_INV_MODE	    0x00001000	/* L2 invalidate mode enable */
#define PICR2_CF_L2_HIT_DELAY(n)    ((n)<<9)	/* L2 cache hit delay */
#define PICR2_CF_BURST_RATE	    0x00000100	/* L2 cache burst rate */
#define PICR2_CF_FAST_CASTOUT	    0x00000080	/* Fast L2 castout timing */
#define PICR2_TOE_WIDTH		    0x00000040	/* TOE active pulse width */
#define PICR2_L2_SIZE		    0x00000030	/* L2 cache size mask */
#define	PICR2_L2_SIZE_256K	    0x00000000
#define	PICR2_L2_SIZE_512K	    0x00000010
#define	PICR2_L2_SIZE_1MEG	    0x00000020
#define PICR2_CF_APHASE_WS(n)	    ((n)<<2)	/* Address phase wait states */
#define PICR2_CF_DOE		    0x00000002	/* 1st data read access tim. */
#define PICR2_CF_WDATA		    0x00000001	/* 1st data write setup time */


/* OS Visible Parameters Register 1 - 0xba */

#define OS_VPR1_RX_SERR_EN	0X20	/* Assertion of SERR from other PCI */ 
#define OS_VPR1_XIO_MODE	0X04	/* Contiguous IO mode address map A */
#define OS_VPR1_TEA_EN		0X02	/* Transfer error enable */ 
#define OS_VPR1_MCP_EN		0X01	/* Machine check enable */ 

/* OS Visible Parameters Register 2 - 0xbb */

#define OS_VPR2_FLASH_WR_EN	0x01	/* Flash write enable */

/* Error Enable Register 1 (ERR_EN_R1) - 0xc0 */

#define ERR_EN_R1_TGT_ABRT      0x80	/* PCI target-abort error */
#define ERR_EN_R1_TGT_PERR      0x40	/* PCI target PERR */
#define ERR_EN_R1_MEM_SEL_ER    0x20 	/* Memory select error */
#define ERR_EN_R1_MASTER_PERR   0x10	/* PCI master PERR */
#define ERR_EN_R1_MEM_RD_PARITY 0x04 	/* Memory read parity error */
#define ERR_EN_R1_MASTER_ABRT   0x02 	/* PCI master-abort error */
#define ERR_EN_R1_60X_BUS_ER    0x01 	/* 60x bus error */

/* Error Detection Register 1 (ERR_DR1_PCI) - 0xc1 */

#define ERR_DR1_PCI_SERR        0x80	/* PCI SERR signaled */
#define ERR_DR1_TGT_PERR        0x40	/* PCI target PERR */
#define ERR_DR1_MEM_SEL_ER      0x20	/* Memory select error */
#define ERR_DR1_60XPCI_CYCLE    0x80	/* 60x/PCI cycle */
#define ERR_DR1_MEM_RD_PARITY     0x40	/* Memory read parity error */
#define ERR_DR1_NO_60XBUS_CYCLE 0x03	/* Unsupported 60x bus cycle */

/* 60x Bus Error Status Register - 0xc3 */

#define BESR_TT0_TT4		0xf8	/* TT0-TT4 signals copy */
#define BESR_TSIZ0_TSIZ2	0x07	/* TSIZ0-TSIZ2 signals copy */

/* Error Enable Register 2 (ERR_EN_R2) - 0xc4 */

#define ERR_EN_R2_L2_PARITY     0x10 	/* L2 Parity error */
#define ERR_EN_R2_FLASHROM_WRT  0x01 	/* FLASH ROM write error */

/* Error Detection Register 2 (ERR_DR2) - 0xc5 */

#define ERR_DR2_INVLD_ER_ADR    0x80	/* Invalid error address */
#define ERR_DR2_L2_PARITY       0x10	/* L2 parity error */
#define ERR_DR2_FLASHROM_WRT    0x01	/* FLASH ROM write error */

/* PCI Bus Error Status Register - 0xc7 */

#define PCI_BESR_MSTR_TGT	0x10	/* Master/target status */
#define PCI_BESR_CBE3_CBE0	0x0f	/* C/BE3-C/BE0 copy */

/* 60x/PCI Error Address Register - 0xc8 */

#define PCI_EAR_A24_A31_MSK	0xff000000	/* 60x: A24-A31 PCI: AD7-AD0 */
#define PCI_EAR_A16_A23_MSK	0x00ff0000	/* 60x: A16-A23 PCI: AD15-AD8 */
#define PCI_EAR_A8_A15_MSK	0x0000ff00	/* 60x: A8-A15 PCI: AD23-AD16 */
#define PCI_EAR_A0_A7_MSK	0x000000ff	/* 60x: A0-A7 PCI: AD31-AD24 */

/* Memory Control Configuration Register 1 (MCCR1) - 0xf0 */

#define MCCR1_ROMNAL_MASK	0xf0000000
#define MCCR1_ROMNAL(n)		((n)<<28)
#define MCCR1_ROMFAL_MASK	0x0f800000
#define MCCR1_ROMFAL(n)		((n)<<23)
#define MCCR1_FNR		0x00400000	/* Flash ROM configuration */
#define MCCR1_32N64		0x00200000	/* 64/32 Bit data bus wide */
#define MCCR1_BURST		0x00100000	/* Burst mode Rom access */
#define MCCR1_MEMGO		0x00080000	/* MPC105 RAM interface enable*/
#define MCCR1_SREN		0x00040000	/* DRam Self Refresh enable */
#define MCCR1_RAMTYP		0x00020000	/* DRAM type (synch/standard) */
#define MCCR1_PCKEN		0x00010000	/* Parity Check/gen. enable */
#define MCCR1_BK7_9BITS		0x00000000	/* Bank 7 -  9 row bits */
#define MCCR1_BK7_10BITS	0x00004000	/* Bank 7 - 10 row bits */
#define MCCR1_BK7_11BITS	0x00008000	/* Bank 7 - 11 row bits */
#define MCCR1_BK7_12BITS	0x0000c000	/* Bank 7 - 12 row bits */
#define MCCR1_BK6_9BITS		0x00000000	/* Bank 6 -  9 row bits */
#define MCCR1_BK6_10BITS	0x00001000	/* Bank 6 - 10 row bits */
#define MCCR1_BK6_11BITS	0x00002000	/* Bank 6 - 11 row bits */
#define MCCR1_BK6_12BITS	0x00003000	/* Bank 6 - 12 row bits */
#define MCCR1_BK5_9BITS		0x00000000	/* Bank 5 -  9 row bits */
#define MCCR1_BK5_10BITS	0x00000400	/* Bank 5 - 10 row bits */
#define MCCR1_BK5_11BITS	0x00000800	/* Bank 5 - 11 row bits */
#define MCCR1_BK5_12BITS	0x00000c00	/* Bank 5 - 12 row bits */
#define MCCR1_BK4_9BITS		0x00000000	/* Bank 4 -  9 row bits */
#define MCCR1_BK4_10BITS	0x00000100	/* Bank 4 - 10 row bits */
#define MCCR1_BK4_11BITS	0x00000200	/* Bank 4 - 11 row bits */
#define MCCR1_BK4_12BITS	0x00000300	/* Bank 4 - 12 row bits */
#define MCCR1_BK3_9BITS		0x00000000	/* Bank 3 -  9 row bits */
#define MCCR1_BK3_10BITS	0x00000040	/* Bank 3 - 10 row bits */
#define MCCR1_BK3_11BITS	0x00000080	/* Bank 3 - 11 row bits */
#define MCCR1_BK3_12BITS	0x000000c0	/* Bank 3 - 12 row bits */
#define MCCR1_BK2_9BITS		0x00000000	/* Bank 2 -  9 row bits */
#define MCCR1_BK2_10BITS	0x00000010	/* Bank 2 - 10 row bits */
#define MCCR1_BK2_11BITS	0x00000020	/* Bank 2 - 11 row bits */
#define MCCR1_BK2_12BITS	0x00000030	/* Bank 2 - 12 row bits */
#define MCCR1_BK1_9BITS		0x00000000	/* Bank 1 -  9 row bits */
#define MCCR1_BK1_10BITS	0x00000004	/* Bank 1 - 10 row bits */
#define MCCR1_BK1_11BITS	0x00000008	/* Bank 1 - 11 row bits */
#define MCCR1_BK1_12BITS	0x0000000c	/* Bank 1 - 12 row bits */
#define MCCR1_BK0_9BITS		0x00000000	/* Bank 0 -  9 row bits */
#define MCCR1_BK0_10BITS	0x00000001	/* Bank 0 - 10 row bits */
#define MCCR1_BK0_11BITS	0x00000002	/* Bank 0 - 11 row bits */
#define MCCR1_BK0_12BITS	0x00000003	/* Bank 0 - 12 row bits */

/* Memory Control Configuration Register 2 (MCCR2) - 0xf4 */

#define MCCR2_SRF		0xfffc0000	/* Self-refesh entry delay */
#define MCCR2_REFINT		0x0000fffc	/* Refresh interval */
#define MCCR2_BUF		0x00000002	/* Buffer mode */
#define MCCR2_WMODE		0x00000001	/* 8 beats per burst */

/* Memory Control Configuration Register 3 (MCCR3) - 0xf8 */

#define MCCR3_RDTOACT_MSK	0xf0000000	/* Read to active interval */
#define MCCR3_REFREC_MSK	0x0f000000	/* Refresh to active interval */
#define MCCR3_RDLAT_MSK		0x00f00000	/* Data latency from read cmd */
#define MCCR3_RAS6P_MSK		0x00078000	/* RAS assertion interval mask*/
#define MCCR3_RAS6P(n)		((n)<<15)	/* RAS assertion interval */
#define MCCR3_CAS5_MSK		0x00007000	/* CAS assertion interval mask*/
#define MCCR3_CAS5(n)		((n)<<12)	/* CAS assertion interval */
#define MCCR3_CP4_MSK		0x00000e00	/* CAS precharge interval mask*/
#define MCCR3_CP4(n)		((n)<<9)	/* CAS precharge interval */
#define MCCR3_CAS3_MSK		0x000001c0	/* CAS assertion 1st access */
#define MCCR3_CAS3(n)		((n)<<6)	/* CAS assertion 1st access */
#define MCCR3_RCD2_MSK		0x00000038	/* RAS to CAS delay interval */
#define MCCR3_RCD2(n)		((n)<<3)	/* RAS to CAS delay interval */
#define MCCR3_RP1_MSK		0x00000007	/* RAS precharge interval */
#define MCCR3_RP1(n)		(n)/* RAS precharge interval */

/* Memory Control Configuration Register 4 (MCCR4) - 0xfc */

#define MCCR4_PRETOACT		0xf0000000	/* Precharge to act. intvl */
#define MCCR4_ACTOPRE		0x0f000000	/* Activate to prech. intvl */
#define MCCR4_WCBUF		0x00200000	/* Memory write buffer type */
#define MCCR4_RCBUF		0x00100000	/* Memory read buffer type */
#define MCCR4_SDMODE		0x000fff00	/* SDRAM mode register data */
#define MCCR4_ACTORW		0x000000f0	/* Act. to read/write intvl */
#define MCCR4_WRTOACT		0x0000000f	/* Write to activate intvl */

/* External Configuration Register 1 - 0x80000092 */

#define ECR1_LE_MODE		0x02	/* Little-endian mode */ 

/* External Configuration Register 2 - 0x8000081c */

#define ECR2_L2_UPDATE_EN	0x80	/* Update cache misses */
#define ECR2_L2_EN		0x40	/* L2 cache enable */
#define ECR2_TEA_EN		0x20	/* Transfer error enable */
#define ECR2_CF_FLUSH_L2	0x10	/* L2 cache flush */

/* External Configuration Register 3 - 0x80000850 */

#define ECR3_XIO_MODE		0x01	/* Contiguous IO mode address map A*/

/* first parameter to mpc105Ioctl */

#define MPC105_IOCTL_WRITE      0
#define MPC105_IOCTL_READ       1
#define MPC105_IOCTL_OR         2
#define MPC105_IOCTL_AND        3
#define MPC105_IOCTL_AND_OR     4
#define MPC105_IOCTL_AND_NOT    5

#define SWAP32(x)	((((x) & 0x000000ff) << 24) |	\
			(((x) & 0x0000ff00) <<  8)  |	\
			(((x) & 0x00ff0000) >>  8)  |	\
			(((unsigned int)(x)) >> 24))
#define B2L_ENDIAN(x)   SWAP32(x)
#define L2B_ENDIAN(x)   SWAP32(x)


#ifndef _ASMLANGUAGE

#if     defined(__STDC__) || defined(__cplusplus)

extern void     mpc105Init      (void * devAddr);
extern void     mpc105Dram      (void * devAddr, int boardType, int speedMhz);
extern void     mpc105Ioctl     (int op, int regSize, int regNum,
                                 unsigned int wdata1, unsigned int wdata2);

#else   /* !__STDC__ */

extern void     mpc105Init      ();
extern void     mpc105Dram      ();
extern void     mpc105Ioctl     ();

#endif	/* (__STDC__) || (__cplusplus) */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCmpc105h */
