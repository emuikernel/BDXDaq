/* m8260Fcc.h - Motorola MPC8260 Fast Communications Controller header file */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,12sep99,ms_  created from m8260Cpm.h, 01d.
*/

/*
 * This file contains constants for the Fast Communications Controllers
 * (fCCs) in the Motorola MPC8260 PowerQUICC II integrated Communications
 * Processor
 */

#ifndef __INCm8260Fcch
#define __INCm8260Fcch

#ifdef __cplusplus
extern "C" {
#endif

#ifndef M8260ABBREVIATIONS
#define M8260ABBREVIATIONS

#ifdef  _ASMLANGUAGE
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
#endif  /* _ASMLANGUAGE */

#endif /* M8260ABBREVIATIONS */

/*
 * MPC8260 internal register/memory map (section 17 of prelim. spec)
 * note that these are offsets from the value stored in the IMMR
 * register. Also note that in the MPC8260, the IMMR is not a special
 * purpose register, but it is memory mapped.
 */
 
/* CPM mux FCC clock route register */

#define M8260_CMXFCR(base)   (CAST(VUINT32 *)((base) + 0x11B04)) 

/* FCC 1 register set */
 
#define M8260_FGMR1(base)   (CAST(VUINT32 *)((base) + 0x11300)) /* Gen Mode */
#define M8260_FPSMR1(base)  (CAST(VUINT32 *)((base) + 0x11304)) /* Prot Spec */
#define M8260_FTODR1(base)  (CAST(VUINT32 *)((base) + 0x11308)) /* transmit */
								/* on demand */
#define M8260_FDSR1(base)   (CAST(VUINT32 *)((base) + 0x1130C)) /* data sync */
#define M8260_FCCER1(base)  (CAST(VUINT32 *)((base) + 0x11310)) /* event */
#define M8260_FCCMR1(base)  (CAST(VUINT32 *)((base) + 0x11304)) /* mask */
#define M8260_FCCSR1(base)  (CAST(VUINT32 *)((base) + 0x11308)) /* status */

/* FCC1 transmit internal rate registers for PHY 0-3 */

#define M8260_FTIRR1_PHY0(base)	(CAST(VUINT32 *)((base) + 0x1131C)) 
#define M8260_FTIRR1_PHY1(base)	(CAST(VUINT32 *)((base) + 0x1131D)) 
#define M8260_FTIRR1_PHY2(base)	(CAST(VUINT32 *)((base) + 0x1131E)) 
#define M8260_FTIRR1_PHY3(base)	(CAST(VUINT32 *)((base) + 0x1131F)) 

/* offsets in internal RAM of FCC registers */

#define M8260_FCC_IRAM_GAP	0x20		/* gap between FCCs */
						/* in Internal RAM */
#define M8260_FCC_GFMR_OFF	0x0		/* GFMR offset */
#define M8260_FCC_FPSMR_OFF	0x4		/* FPSMR offset */
#define M8260_FCC_FTODR_OFF	0x8		/* FTODR offset */
#define M8260_FCC_FDSR_OFF	0xC		/* FDSR offset */
#define M8260_FCC_FCCER_OFF	0x10		/* FCCER offset */
#define M8260_FCC_FCCMR_OFF	0x14		/* FCCMR offset */
#define M8260_FCC_FCCSR_OFF	0x18		/* FCCSR offset */

/* FCC Dual-Ported RAM definitions */
 
#define M8260_FCC1_BASE(base)	(CAST(VUINT32 *)((base) + 0x8400)) /* FCC1 */
#define M8260_FCC2_BASE(base)	(CAST(VUINT32 *)((base) + 0x8500)) /* FCC2 */
#define M8260_FCC3_BASE(base)	(CAST(VUINT32 *)((base) + 0x8600)) /* FCC3 */
#define M8260_FCC_DPRAM_GAP	0x100		/* gap between FCCs */
						/* parameter RAM in DPRAM */
#define M8260_FCC_RIPTR_OFF	0x0		/* rx FIFO pointer offset */
#define M8260_FCC_TIPTR_OFF	0x2		/* tx FIFO pointer offset */
#define M8260_FCC_RES1_OFF	0x4		/* reserved */
#define M8260_FCC_MRBLR_OFF	0x6		/* max rx buffer length */
#define M8260_FCC_RSTATE_OFF	0x8		/* rx internal state */
#define M8260_FCC_RBASE_OFF	0xC		/* RBD base address */
#define M8260_FCC_RBDSTAT_OFF	0x10		/* RBD status/control */
#define M8260_FCC_RBDLEN_OFF	0x12		/* RBD data length */
#define M8260_FCC_RDPTR_OFF	0x14		/* RBD data pointer */
#define M8260_FCC_TSTATE_OFF	0x18		/* tx internal state */
#define M8260_FCC_TBASE_OFF	0x1C		/* TBD base address */
#define M8260_FCC_TBDSTAT_OFF	0x20		/* TBD status/control */
#define M8260_FCC_TBDLEN_OFF	0x22		/* TBD data length */
#define M8260_FCC_TDPTR_OFF	0x24		/* TBD data pointer */
#define M8260_FCC_RBPTR_OFF	0x28		/* RBD pointer */
#define M8260_FCC_TBPTR_OFF	0x2C		/* TBD pointer */
#define M8260_FCC_RCRC_OFF	0x30		/* temp rx CRC */
#define M8260_FCC_TCRC_OFF	0x34		/* temp tx CRC */

#define M8260_FCC_STATBUF_OFF	0x3C		/* internal buffer */
#define M8260_FCC_CAM_PTR_OFF	0x40		/* CAM address */
#define M8260_FCC_C_MASK_OFF	0x44		/* MASK for CRC */
#define M8260_FCC_C_PRES_OFF	0x48		/* preset CRC */
#define M8260_FCC_CRCEC_OFF	0x4C		/* CRC error counter */
#define M8260_FCC_ALEC_OFF	0x50		/* alignment error counter */
#define M8260_FCC_DISFC_OFF	0x54		/* discard frame counter */
#define M8260_FCC_RET_LIM_OFF	0x58		/* retry limit */
#define M8260_FCC_RET_CNT_OFF	0x5A		/* retry limit counter */
#define M8260_FCC_P_PER_OFF	0x5C		/* persistence */
#define M8260_FCC_BOFF_CNT_OFF	0x5E		/* backoff counter */
#define M8260_FCC_GADDR_H_OFF	0x60		/* group address filter high */
#define M8260_FCC_GADDR_L_OFF	0x64		/* group address filter low */
#define M8260_FCC_TFCSTAT_OFF	0x68		/* out-of-sequence TBD stat */
#define M8260_FCC_TFCLEN_OFF	0x6A		/* out-of-sequence TBD length */
#define M8260_FCC_TFCPTR_OFF	0x6C		/* out-of-sequence TBD pointer*/
#define M8260_FCC_MFLR_OFF	0x70		/* max receive frame length */
#define M8260_FCC_PADDR_H_OFF	0x72		/* individual address high */
#define M8260_FCC_PADDR_M_OFF	0x74		/* individual address medium */
#define M8260_FCC_PADDR_L_OFF	0x76		/* individual address low */
#define M8260_FCC_IBD_CNT_OFF	0x78		/* internal BD counter */
#define M8260_FCC_IBD_START_OFF	0x7A		/* internal BD start pointer */
#define M8260_FCC_IBD_END_OFF	0x7C		/* internal BD end pointer */
#define M8260_FCC_TX_LEN_OFF	0x7E		/* tx frame length counter */
#define M8260_FCC_IBD_BASE_OFF	0x80		/* internal BD base */
#define M8260_FCC_IADDR_H_OFF	0xA0		/* individual addr filter high*/
#define M8260_FCC_IADDR_L_OFF	0xA4		/* individual addr filter low */
#define M8260_FCC_MINFLR_OFF	0xA8		/* min frame lenght */
#define M8260_FCC_TADDR_H_OFF	0xAA		/* set hash table addr high */
#define M8260_FCC_TADDR_M_OFF	0xAC		/* set hash table addr medium */
#define M8260_FCC_TADDR_L_OFF	0xAE		/* set hash table addr low */
#define M8260_FCC_PAD_PTR_OFF	0xB0		/* internal PAD pointer */
#define M8260_FCC_RES2_OFF	0xB2		/* reserved */
#define M8260_FCC_CF_RANGE_OFF	0xB4		/* control frame range */
#define M8260_FCC_MAX_B_OFF	0xB6		/* max BD byte counter */
#define M8260_FCC_MAXD1_OFF	0xB8		/* max DMA1 lenght */
#define M8260_FCC_MAXD2_OFF	0xBA		/* max DMA2 lenght */
#define M8260_FCC_MAXD_OFF	0xBC		/* rx max DMA lenght */
#define M8260_FCC_DMA_CNT_OFF	0xBE		/* rx DMA counter */
#define M8260_FCC_OCTC_OFF	0xC0		/* data octets number */
#define M8260_FCC_COLC_OFF	0xC4		/* collision estimate */
#define M8260_FCC_BROC_OFF	0xC8		/* received broadast packets */
#define M8260_FCC_MULC_OFF	0xCC		/* received multicast packets */
#define M8260_FCC_USPC_OFF	0xD0		/* good packets shorter than */
						/* 64 bytes */
#define M8260_FCC_FRGC_OFF	0xD4		/* bad packets shorter than */
						/* 64 bytes */
#define M8260_FCC_OSPC_OFF	0xD8		/* good packets longer than */
						/* 1518 bytes */
#define M8260_FCC_JBRC_OFF	0xDC		/* bad packets longer than */
						/* 1518 bytes */
#define M8260_FCC_P64C_OFF	0xE0		/* packets 64-byte long */
#define M8260_FCC_P65C_OFF	0xE4		/* packets < 128 bytes and */
						/* > 64 bytes */
#define M8260_FCC_P128C_OFF	0xE8		/* packets < 256 bytes and */ 
						/* > 127 bytes */
#define M8260_FCC_P256C_OFF	0xEC		/* packets < 512 bytes and */ 
						/* > 255 bytes */
#define M8260_FCC_P512C_OFF	0xF0		/* packets < 1024 bytes and */ 
						/* > 511 bytes */
#define M8260_FCC_P1024C_OFF	0xF4		/* packets < 1519 bytes and */ 
						/* > 1023 bytes */
#define M8260_FCC_CAM_BUF_OFF	0xF8		/* internal buffer */
#define M8260_FCC_RES3_OFF	0xFC		/* reserved */

/* General FCC Mode Register definitions */
 
#define M8260_GFMR_HDLC		0x00000000      /* HDLC mode */
#define M8260_GFMR_RES1		0x00000001      /* reserved mode */
#define M8260_GFMR_RES2		0x00000002      /* reserved mode */
#define M8260_GFMR_RES3		0x00000003      /* reserved mode */
#define M8260_GFMR_RES4		0x00000004      /* reserved mode */
#define M8260_GFMR_RES5		0x00000005      /* reserved mode */
#define M8260_GFMR_RES6		0x00000006      /* reserved mode */
#define M8260_GFMR_RES7		0x00000007      /* reserved mode */
#define M8260_GFMR_RES8		0x00000008      /* reserved mode */
#define M8260_GFMR_RES9		0x00000009      /* reserved mode */
#define M8260_GFMR_ATM		0x0000000a      /* ATM mode */
#define M8260_GFMR_RES10	0x0000000b      /* reserved mode */
#define M8260_GFMR_ETHERNET	0x0000000c      /* ethernet mode */
#define M8260_GFMR_RES11	0x0000000d      /* reserved mode */
#define M8260_GFMR_RES12	0x0000000e      /* reserved mode */
#define M8260_GFMR_RES13	0x0000000f      /* reserved mode */
#define M8260_GFMR_NORM		0x00000000      /* normal mode */
#define M8260_GFMR_LOOP		0x40000000      /* local loopback */
#define M8260_GFMR_ECHO		0x80000000      /* automatic echo */
#define M8260_GFMR_LOOP_ECHO	0xc0000000      /* loop & echo */
#define M8260_GFMR_TCI       	0x20000000      /* tx clock invert */
#define M8260_GFMR_TRX        	0x10000000      /* transparent receiver */
#define M8260_GFMR_TTX       	0x08000000      /* transparent transmitter */
#define M8260_GFMR_CDP        	0x04000000      /* CD* pulse */
#define M8260_GFMR_CTSP       	0x02000000      /* CTS* pulse */
#define M8260_GFMR_CDS        	0x01000000      /* CD* sampling */
#define M8260_GFMR_CTSS       	0x00800000      /* CTS* sampling */
#define M8260_GFMR_SYN_EXT    	0x00000000      /* external sync */
#define M8260_GFMR_SYN_AUTO   	0x00004000      /* automatic sync */
#define M8260_GFMR_SYN_8	0x00008000      /* 8-bit sync pattern */
#define M8260_GFMR_SYN_16	0x0000c000      /* 16-bit sync pattern */
#define M8260_GFMR_RTSM		0x00002000      /* RTS* mode */
#define M8260_GFMR_RENC_RES   	0x00001000      /* receiver encoding reserved */
#define M8260_GFMR_RENC_NRZI  	0x00000800      /* receiver encoding NRZI */
#define M8260_GFMR_RENC_NRZ   	0x00000000      /* receiver encoding NRZ */
#define M8260_GFMR_REVD       	0x00000400      /* reverse data */
#define M8260_GFMR_TENC_RES   	0x00000200      /* transmitter encoding res */
#define M8260_GFMR_TENC_NRZI  	0x00000100      /* transmitter encoding NRZI */
#define M8260_GFMR_TENC_NRZ   	0x00000000      /* transmitter encoding NRZ */
#define M8260_GFMR_TCRC_RES   	0x00000040      /* transparent CRC reserved */
#define M8260_GFMR_TCRC_16    	0x00000000      /* 16-bit transparent CRC */
#define M8260_GFMR_TCRC_32    	0x00000080      /* 32-bit transparent CRC */
#define M8260_GFMR_ENT		0x00000010      /* enable transmitter */
#define M8260_GFMR_ENR		0x00000020      /* enable receiver */

/* FCC Data Synchronization Register definitions */
 
#define M8260_FDSR_SYN1_MASK	0x00ff      	/* sync pattern mask 1 */
#define M8260_FDSR_SYN2_MASK	0xff00      	/* sync pattern mask 2 */
#define M8260_FDSR_ETH_SYN1	0x55      	/* Ethernet sync pattern 1 */
#define M8260_FDSR_ETH_SYN2	0xd5      	/* Ethernet sync pattern 2 */

/* FCC Transmit on Demand Register definitions */
 
#define M8260_FTODR_TOD		0x8000      	/* transmit on demand */

/* FCC Function Code Register definitions */
 
#define M8260_FCR_GBL		0x20      	/* global mem operation */
						/* enable snooping */
#define M8260_FCR_BO_BE		0x10      	/* big-endian ordering */
#define M8260_FCR_BO_LE		0x08      	/* little-endian ordering */
#define M8260_FCR_TC2		0x04      	/* transfer code for TC[2] */
#define M8260_FCR_DTB		0x02      	/* data is in the local bus */
#define M8260_FCR_BDB		0x01      	/* BDs are in the local bus */
#define M8260_FCR_SHIFT		24      	/* get to fcr bits in xstate */

/* FCC Ethernet Protocol Specific Mode Register definitions */
 
#define M8260_FPSMR_ETH_HBC	0x80000000          /* heartbeat checking */
#define M8260_FPSMR_ETH_FC	0x40000000          /* force collision */
#define M8260_FPSMR_ETH_SBT	0x20000000          /* stop backoff timer */
#define M8260_FPSMR_ETH_LPB	0x10000000          /* loopback operation */
#define M8260_FPSMR_ETH_LCW	0x08000000          /* late collision window */
#define M8260_FPSMR_ETH_FDE	0x04000000          /* full duplex enable */
#define M8260_FPSMR_ETH_MON	0x02000000          /* enable RMON mode */
#define M8260_FPSMR_ETH_PRO	0x00400000          /* enable promiscous mode */
#define M8260_FPSMR_ETH_FCE	0x00200000          /* flow control enable */
#define M8260_FPSMR_ETH_RSH	0x00100000          /* receive short frame */
#define M8260_FPSMR_ETH_CAM	0x00000400          /* CAM address matching */
#define M8260_FPSMR_ETH_BRO	0x00000200          /* broadcast enable */
#define M8260_FPSMR_ETH_CRC_32	0x00000080          /* use 32-bit CCITT CRC */
#define M8260_FPSMR_ETH_CRC_MASK	0x000000c0          /* CRC mask field */

/* FCC Ethernet Event and Mask Register definitions */
 
#define M8260_FEM_ETH_RES      0xff00          /* reserved mask */
#define M8260_FEM_ETH_EVENT    0x00ff          /* event mask */
#define M8260_FEM_ETH_GRA      0x0080          /* graceful stop event */
#define M8260_FEM_ETH_RXC      0x0040          /* rx control frame event */
#define M8260_FEM_ETH_TXC      0x0020          /* tx control frame event */
#define M8260_FEM_ETH_TXE      0x0010          /* transmission error event */
#define M8260_FEM_ETH_RXF      0x0008          /* frame received event */
#define M8260_FEM_ETH_BSY      0x0004          /* busy condition */
#define M8260_FEM_ETH_TXB      0x0002          /* buffer transmitted event */
#define M8260_FEM_ETH_RXB      0x0001          /* buffer received event */
 
/* FCC Ethernet Receive Buffer Descriptor definitions */
 
#define M8260_FETH_RBD_E       0x8000          /* buffer is empty */
#define M8260_FETH_RBD_W       0x2000          /* last BD in ring */
#define M8260_FETH_RBD_I       0x1000          /* interrupt on receive */
#define M8260_FETH_RBD_L       0x0800          /* buffer is last in frame */
#define M8260_FETH_RBD_F       0x0400          /* buffer is first in frame */
#define M8260_FETH_RBD_M       0x0100          /* miss bit for prom mode */
#define M8260_FETH_RBD_BC      0x0080          /* broadcast address frame */
#define M8260_FETH_RBD_MC      0x0040          /* multicast address frame */
#define M8260_FETH_RBD_LG      0x0020          /* frame length violation */
#define M8260_FETH_RBD_NO      0x0010          /* nonoctet aligned frame */
#define M8260_FETH_RBD_SH      0x0008          /* short frame received */
#define M8260_FETH_RBD_CR      0x0004          /* Rx CRC error */
#define M8260_FETH_RBD_OV      0x0002          /* overrun condition */
#define M8260_FETH_RBD_CL      0x0001          /* collision condition */
 
/* FCC Ethernet Transmit Buffer Descriptor definitions */
 
#define M8260_FETH_TBD_R       0x8000          /* buffer is ready */
#define M8260_FETH_TBD_PAD     0x4000          /* auto pad short frames */
#define M8260_FETH_TBD_W       0x2000          /* last BD in ring */
#define M8260_FETH_TBD_I       0x1000          /* interrupt on transmit */
#define M8260_FETH_TBD_L       0x0800          /* buffer is last in frame */
#define M8260_FETH_TBD_TC      0x0400          /* auto transmit CRC */
#define M8260_FETH_TBD_DEF     0x0200          /* defer indication */
#define M8260_FETH_TBD_HB      0x0100          /* heartbeat */
#define M8260_FETH_TBD_LC      0x0080          /* late collision */
#define M8260_FETH_TBD_RL      0x0040          /* retransmission limit */
#define M8260_FETH_TBD_RC      0x003c          /* retry count */
#define M8260_FETH_TBD_UN      0x0002          /* underrun */
#define M8260_FETH_TBD_CSL     0x0001          /* carrier sense lost */
 
#ifdef __cplusplus
}
#endif

#endif /* __INCm8260Fcch */
