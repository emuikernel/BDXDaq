/* motFecEnd.h - Motorola FEC Ethernet network interface header */

/* Copyright 1990-2002 Wind River Systems, Inc. */
/* Copyright 1999-2000 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01g,01apr02,pmr  SPR 72070: passing unit number to sysFecEnetAddrGet.
01f,07dec01,rcs  added  define REV_D_3 SPR# 71420  
01e,26oct01,crg  added support for proper setting of MII management interface
                 speed (SPR 33812)
01d,21nov00,rhk  added define for revision D4 processor and for pin mux bit.
01c,11jun00,ham  removed reference to etherLib.
01b,09feb99,cn	 changes required by performance improvement (SPR# 24883).
01a,09nov98,cn	 written.
*/

#ifndef __INCmotFecEndh
#define __INCmotFecEndh

/* includes */

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

/* revision D.3 and greater processors require special FEC initialization */

#define REV_D_4 0x0502
#define REV_D_3 0x0501

/*
 * redefine the macro below in the bsp if you need to access the device
 * registers/descriptors in a more suitable way.
 */

#ifndef MOT_FEC_LONG_WR
#define MOT_FEC_LONG_WR(addr, value)                                        \
    (* (addr) = ((UINT32) (value)))
#endif /* MOT_FEC_LONG_WR */

#ifndef MOT_FEC_WORD_WR
#define MOT_FEC_WORD_WR(addr, value)                                        \
    (* (addr) = ((UINT16) (value)))
#endif /* MOT_FEC_WORD_WR */

#ifndef MOT_FEC_BYTE_WR
#define MOT_FEC_BYTE_WR(addr, value)                                        \
    (* (addr) = ((UINT8) (value)))
#endif /* MOT_FEC_BYTE_WR */

#ifndef MOT_FEC_LONG_RD
#define MOT_FEC_LONG_RD(addr, value)                                        \
    ((value) = (* (UINT32 *) (addr)))
#endif /* MOT_FEC_LONG_RD */

#ifndef MOT_FEC_WORD_RD
#define MOT_FEC_WORD_RD(addr, value)                                        \
    ((value) = (* (UINT16 *) (addr)))
#endif /* MOT_FEC_WORD_RD */

#ifndef MOT_FEC_BYTE_RD
#define MOT_FEC_BYTE_RD(addr, value)                                        \
    ((value) = (* (UINT8 *) (addr)))
#endif /* MOT_FEC_BYTE_RD */

/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */

#ifndef SYS_FEC_INT_CONNECT
#define SYS_FEC_INT_CONNECT(pDrvCtrl, pFunc, arg, ret)                      \
{                                                                           \
IMPORT STATUS intConnect (VOIDFUNCPTR *, VOIDFUNCPTR, int);		    \
ret = OK;                                                                   \
                                                                            \
if (MOT_FEC_VECTOR (pDrvCtrl) && (!(pDrvCtrl->intrConnect)))  		    \
    {                                                                       \
    pDrvCtrl->intrConnect = TRUE;                                   	    \
    ret = (intConnect) ((VOIDFUNCPTR*)                              	    \
                                INUM_TO_IVEC (MOT_FEC_VECTOR (pDrvCtrl)),   \
                                (pFunc), (int) (arg));                      \
    }                                                                       \
}
#endif /* SYS_FEC_INT_CONNECT */

#ifndef SYS_FEC_INT_DISCONNECT
#define SYS_FEC_INT_DISCONNECT(pDrvCtrl, pFunc, arg, ret)                   \
{                                                                           \
ret = OK;                                                                   \
                                                                            \
if (MOT_FEC_VECTOR (pDrvCtrl) && (motFecIntDisc != NULL))             	    \
    {                                                                       \
    pDrvCtrl->intrConnect = FALSE;                                          \
    ret = (*motFecIntDisc) ((VOIDFUNCPTR*)                          	    \
                              INUM_TO_IVEC (MOT_FEC_VECTOR (pDrvCtrl)),     \
                              (pFunc));                                     \
    }                                                                       \
}
#endif /* SYS_FEC_INT_DISCONNECT */

#ifndef SYS_FEC_INT_ENABLE
#define SYS_FEC_INT_ENABLE(pDrvCtrl, ret)				    \
{                                                                           \
IMPORT int intEnable (int);						    \
ret = OK;                                                                   \
                                                                            \
if (MOT_FEC_VECTOR (pDrvCtrl)) 			    			    \
    ret = intEnable ((int) (MOT_FEC_VECTOR (pDrvCtrl)));		    \
}
#endif /* SYS_FEC_INT_ENABLE */

#ifndef SYS_FEC_INT_DISABLE
#define SYS_FEC_INT_DISABLE(pDrvCtrl, ret)				    \
{                                                                           \
IMPORT int intDisable (int);						    \
ret = OK;                                                                   \
                                                                            \
if (MOT_FEC_VECTOR (pDrvCtrl)) 			    			    \
    ret = intDisable ((int) (MOT_FEC_VECTOR (pDrvCtrl)));		    \
}
#endif /* SYS_FEC_INT_DISABLE */

#define SYS_FEC_ENET_ADDR_GET(address)                                  \
if (sysFecEnetAddrGet != NULL)                                          \
    if (sysFecEnetAddrGet (pDrvCtrl->unit, (address)) == ERROR)         \
        {                                                               \
        errnoSet (S_iosLib_INVALID_ETHERNET_ADDRESS);                   \
        return (NULL);                                                  \
        }

#define SYS_FEC_ENET_ENABLE                                             \
if (sysFecEnetEnable != NULL)                                           \
    if (sysFecEnetEnable (pDrvCtrl->motCpmAddr) == ERROR)               \
        return (ERROR);

#define SYS_FEC_ENET_DISABLE                                            \
if (sysFecEnetDisable != NULL)                                          \
    if (sysFecEnetDisable (pDrvCtrl->motCpmAddr) == ERROR)              \
        return (ERROR);

#define MOT_FEC_DEV_NAME       	"motfec"
#define MOT_FEC_DEV_NAME_LEN   	7
#define MOT_FEC_TBD_DEF_NUM    	64		/* default number of TBDs */
#define MOT_FEC_RBD_DEF_NUM    	48		/* default number of RBDs */
#define MOT_FEC_TX_CL_NUM    	6		/* number of tx clusters */
#define MOT_FEC_BD_LOAN_NUM    	32		/* loaned BDs */
#define MOT_FEC_TBD_MAX         128     	/* max number of TBDs */
#define MOT_FEC_RBD_MAX         128     	/* max number of TBDs */
#define MOT_FEC_WAIT_MAX	0xf0000000	/* max delay after sending */

#define MOT_FEC_ADDR_LEN        6               /* ethernet address length */

/* Control/Status Registers (CSR) definitions */

#define MOT_FEC_CSR_OFF         0x0e00  /* CSRs offset in the 860T RAM */
#define MOT_FEC_ADDR_L_OFF      0x0e00  /* lower 32-bits of MAC address */
#define MOT_FEC_ADDR_H_OFF      0x0e04  /* upper 16-bits of MAC address */
#define MOT_FEC_HASH_H_OFF      0x0e08  /* upper 32-bits of hash table */
#define MOT_FEC_HASH_L_OFF      0x0e0c  /* lower 32-bits of hash table */
#define MOT_FEC_RX_START_OFF    0x0e10  /* rx ring start address */
#define MOT_FEC_TX_START_OFF    0x0e14  /* tx ring start address */
#define MOT_FEC_RX_BUF_OFF      0x0e18  /* max rx buf length */
#define MOT_FEC_CTRL_OFF        0x0e40  /* FEC control register */
#define MOT_FEC_EVENT_OFF       0x0e44  /* interrupt event register */
#define MOT_FEC_MASK_OFF        0x0e48  /* interrupt mask register */
#define MOT_FEC_VEC_OFF         0x0e4c  /* interrupt level/vector register */
#define MOT_FEC_RX_ACT_OFF      0x0e50  /* rx ring has been updated */
#define MOT_FEC_TX_ACT_OFF      0x0e54  /* tx ring has been updated */
#define MOT_FEC_MII_DATA_OFF    0x0e80  /* MII data register */
#define MOT_FEC_MII_SPEED_OFF   0x0e84  /* MII speed register */
#define MOT_FEC_RX_BOUND_OFF    0x0ecc  /* rx fifo limit in the 860T ram */
#define MOT_FEC_RX_FIFO_OFF     0x0ed0  /* rx fifo base in the 860T ram */
#define MOT_FEC_TX_FIFO_OFF     0x0eec  /* tx fifo base in the 860T ram */
#define MOT_FEC_SDMA_OFF        0x0f34  /* function code to SDMA */
#define MOT_FEC_RX_CTRL_OFF     0x0f44  /* rx control register */
#define MOT_FEC_RX_FR_OFF       0x0f48  /* max rx frame length */
#define MOT_FEC_TX_CTRL_OFF     0x0f84  /* tx control register */

/* Control/Status Registers (CSR) bit definitions */

#define MOT_FEC_RX_START_MSK    0xfffffffc      /* quad-word alignment */
                                                /* required for rx BDs */

#define MOT_FEC_TX_START_MSK    0xfffffffc      /* quad-word alignment */
                                                /* required for tx BDs */
/* Ethernet CSR bit definitions */

#define MOT_FEC_ETH_PINMUX	0x00000004	/* select FEC for port D pins */
#define MOT_FEC_ETH_EN          0x00000002      /* enable Ethernet operation */
#define MOT_FEC_ETH_DIS         0x00000000      /* disable Ethernet operation */
#define MOT_FEC_ETH_RES         0x00000001      /* reset the FEC */
#define MOT_FEC_CTRL_MASK       0x00000007      /* FEC control register mask */

/*
 * interrupt bits definitions: these are common to both the
 * mask and the event register.
 */

#define MOT_FEC_EVENT_HB        0x80000000      /* heartbeat error */
#define MOT_FEC_EVENT_BABR      0x40000000      /* babbling rx error */
#define MOT_FEC_EVENT_BABT      0x20000000      /* babbling tx error */
#define MOT_FEC_EVENT_GRA       0x10000000      /* graceful stop complete */
#define MOT_FEC_EVENT_TXF       0x08000000      /* tx frame */
#define MOT_FEC_EVENT_TXB       0x04000000      /* tx buffer */
#define MOT_FEC_EVENT_RXF       0x02000000      /* rx frame */
#define MOT_FEC_EVENT_RXB       0x01000000      /* rx buffer */
#define MOT_FEC_EVENT_MII       0x00800000      /* MII transfer */
#define MOT_FEC_EVENT_BERR      0x00400000      /* U-bus access error */
#define MOT_FEC_EVENT_MSK       0xffc00000      /* clear all interrupts */
#define MOT_FEC_MASK_ALL        MOT_FEC_EVENT_MSK    /* mask all interrupts */

/* bit masks for the interrupt level/vector CSR */

#define MOT_FEC_LVL_MSK         0xe0000000      /* intr level */
#define MOT_FEC_TYPE_MSK        0x0000000c      /* highest pending intr */
#define MOT_FEC_VEC_MSK         0xe000000c      /* this register mask */
#define MOT_FEC_RES_MSK         0x1ffffff3      /* reserved bits */
#define MOT_FEC_LVL_SHIFT       0x1d            /* intr level bits location */

/* transmit and receive active registers definitions */

#define MOT_FEC_TX_ACT          0x01000000      /* tx active bit */
#define MOT_FEC_RX_ACT          0x01000000      /* rx active bit */

/* MII management frame CSRs */

#define MOT_FEC_MII_ST          0x40000000      /* start of frame delimiter */
#define MOT_FEC_MII_OP_RD       0x20000000      /* perform a read operation */
#define MOT_FEC_MII_OP_WR       0x10000000      /* perform a write operation */
#define MOT_FEC_MII_ADDR_MSK    0x0f800000      /* PHY address field mask */
#define MOT_FEC_MII_REG_MSK     0x007c0000      /* PHY register field mask */
#define MOT_FEC_MII_TA          0x00020000      /* turnaround */
#define MOT_FEC_MII_DATA_MSK    0x0000ffff      /* PHY data field */
#define MOT_FEC_MII_RA_SHIFT    0x12            /* mii reg address bits */
#define MOT_FEC_MII_PA_SHIFT    0x17            /* mii PHY address bits */

#define MOT_FEC_MII_PRE_DIS     0x00000080      /* desable preamble */
#define MOT_FEC_MII_SPEED_25    0x00000005      /* recommended for 25Mhz CPU */
#define MOT_FEC_MII_SPEED_33    0x00000007      /* recommended for 33Mhz CPU */
#define MOT_FEC_MII_SPEED_40    0x00000008      /* recommended for 40Mhz CPU */
#define MOT_FEC_MII_SPEED_50    0x0000000a      /* recommended for 50Mhz CPU */
#define MOT_FEC_MII_SPEED_SHIFT	1		/* MII_SPEED bits location */
#define MOT_FEC_MII_CLOCK_MAX	2500000		/* max freq of MII clock (Hz) */
#define MOT_FEC_MII_MAN_DIS     0x00000000      /* disable the MII management */
                                                /* interface */
#define MOT_FEC_MII_SPEED_MSK   0xffffff81      /* speed field mask */

/* FIFO transmit and receive CSRs definitions */

#define MOT_FEC_FIFO_MSK        0x000003ff      /* FIFO rx/tx/bound mask */

/* SDMA function code CSR */

#define MOT_FEC_SDMA_DATA_BE    0x60000000      /* big-endian byte-ordering */
                                                /* for SDMA data transfer */

#define MOT_FEC_SDMA_DATA_PPC   0x20000000      /* PPC byte-ordering */
                                                /* for SDMA data transfer */

#define MOT_FEC_SDMA_DATA_RES   0x00000000      /* reserved value */

#define MOT_FEC_SDMA_BD_BE      0x18000000      /* big-endian byte-ordering */
                                                /* for SDMA BDs transfer */

#define MOT_FEC_SDMA_BD_PPC     0x08000000      /* PPC byte-ordering */
                                                /* for SDMA BDs transfer */


#define MOT_FEC_SDMA_BD_RES     0x00000000      /* reserved value */
#define MOT_FEC_SDMA_FUNC_0     0x00000000      /* no function code */

/* receive control/hash registers bit definitions */

#define MOT_FEC_RX_CTRL_PROM    0x00000008      /* promiscous mode */
#define MOT_FEC_RX_CTRL_MII     0x00000004      /* select MII interface */
#define MOT_FEC_RX_CTRL_DRT     0x00000002      /* disable rx on transmit */
#define MOT_FEC_RX_CTRL_LOOP    0x00000001      /* loopback mode */
#define MOT_FEC_RX_FR_MSK       0x000007ff      /* rx frame length mask */


/* transmit control register bit definitions */

#define MOT_FEC_TX_CTRL_FD      0x00000004      /* enable full duplex mode */
#define MOT_FEC_TX_CTRL_HBC     0x00000002      /* HB check is performed */
#define MOT_FEC_TX_CTRL_GRA     0x00000001      /* issue a graceful tx stop */

/* rx/tx buffer descriptors definitions */

#define MOT_FEC_RBD_SZ          8       /* RBD size in byte */
#define MOT_FEC_TBD_SZ          8       /* TBD size in byte */
#define MOT_FEC_TBD_MIN         6       /* min number of TBDs */
#define MOT_FEC_RBD_MIN         4       /* min number of RBDs */
#define MOT_FEC_TBD_POLL_NUM    1       /* one TBD for poll operation */
#define CL_OVERHEAD             4       /* prepended cluster overhead */
#define CL_ALIGNMENT            4       /* cluster required alignment */
#define MBLK_ALIGNMENT          4       /* mBlks required alignment */
#define MOT_FEC_BD_ALIGN        0x10    /* required alignment for RBDs */
#define MOT_FEC_MAX_PCK_SZ      (ETHERMTU + SIZEOF_ETHERHEADER          \
                                 + ETHER_CRC_LEN)

#define MOT_FEC_BD_STAT_OFF     0       /* offset of the status word */
#define MOT_FEC_BD_LEN_OFF      2       /* offset of the data length word */
#define MOT_FEC_BD_ADDR_OFF     4       /* offset of the data pointer word */

/* TBD bits definitions */

#define MOT_FEC_TBD_RDY         0x8000          /* ready for transmission */
#define MOT_FEC_TBD_TO1         0x4000          /* transmit ownership bit 1 */
#define MOT_FEC_TBD_WRAP        0x2000          /* look at CSR5 for next bd */
#define MOT_FEC_TBD_TO2         0x1000          /* transmit ownership bit 2 */
#define MOT_FEC_TBD_LAST        0x0800          /* last bd in this frame */
#define MOT_FEC_TBD_CRC         0x0400          /* transmit the CRC sequence */
#define MOT_FEC_TBD_DEF         0x0200          /* deferred transmission */
#define MOT_FEC_TBD_HB          0x0100          /* heartbeat error */
#define MOT_FEC_TBD_LC          0x0080          /* late collision */
#define MOT_FEC_TBD_RL          0x0040          /* retransmission limit */
#define MOT_FEC_TBD_UN          0x0002          /* underrun error */
#define MOT_FEC_TBD_CSL         0x0001          /* carrier sense lost */
#define MOT_FEC_TBD_RC_MASK     0x003c          /* retransmission count mask */

/* RBD bits definitions */

#define MOT_FEC_RBD_EMPTY       0x8000          /* ready for reception */
#define MOT_FEC_RBD_RO1         0x4000          /* receive ownership bit 1 */
#define MOT_FEC_RBD_WRAP        0x2000          /* look at CSR4 for next bd */
#define MOT_FEC_RBD_RO2         0x1000          /* receive ownership bit 2 */
#define MOT_FEC_RBD_LAST        0x0800          /* last bd in this frame */
#define MOT_FEC_RBD_RES1        0x0400          /* reserved bit 1 */
#define MOT_FEC_RBD_RES2        0x0200          /* reserved bit 2 */
#define MOT_FEC_RBD_MISS        0x0100          /* address recognition miss */
#define MOT_FEC_RBD_BC          0x0080          /* broadcast frame */
#define MOT_FEC_RBD_MC          0x0040          /* multicast frame */
#define MOT_FEC_RBD_LG          0x0020          /* frame length violation */
#define MOT_FEC_RBD_NO          0x0010          /* nonoctet aligned frame */
#define MOT_FEC_RBD_SH          0x0008          /* short frame error */
                                                /* not supported by the 860T */
#define MOT_FEC_RBD_CRC         0x0004          /* CRC error */
#define MOT_FEC_RBD_OV          0x0002          /* overrun error */
#define MOT_FEC_RBD_TR          0x0001          /* truncated frame (>2KB) */
#define MOT_FEC_RBD_ERR         (MOT_FEC_RBD_LG  |                      \
                                 MOT_FEC_RBD_NO  |                      \
                                 MOT_FEC_RBD_CRC |                      \
                                 MOT_FEC_RBD_OV  |                      \
                                 MOT_FEC_RBD_TR)

#define MOT_FEC_CRC_POLY	0x04c11db7	/* CRC polynomium: */
						/* x^32 + x^26 + x^23 + */
						/* x^22 + x^16 + x^12 + */
						/* x^11 + x^10 + x^8  + */
						/* x^7  + x^5  + x^4  + */
						/* x^2  + x^1  + x^0  + */

#define MOT_FEC_HASH_MASK	0x7c000000	/* bits 27-31 */
#define MOT_FEC_HASH_SHIFT	0x1a		/* to get the index */

/* defines related to the PHY device */

#define MOT_FEC_PHY_PRE_INIT	0x0001		/* PHY info initialized */
#define MOT_FEC_PHY_AUTO	0x0010		/* enable auto-negotiation */
#define MOT_FEC_PHY_TBL		0x0020		/* use negotiation table */
#define MOT_FEC_PHY_100		0x0040		/* PHY may use 100Mbit speed */
#define MOT_FEC_PHY_10		0x0080		/* PHY may use 10Mbit speed */
#define MOT_FEC_PHY_FD		0x0100		/* PHY may use full duplex */
#define MOT_FEC_PHY_HD		0x0200		/* PHY may use half duplex */
#define MOT_FEC_PHY_MAX_WAIT	0x100		/* max delay before */
#define MOT_FEC_PHY_NULL	0xff		/* PHY is not present */
#define MOT_FEC_PHY_DEF		0x0		/* PHY's logical address */

/* allowed PHY's speeds */

#define MOT_FEC_100MBS		100000000       /* bits per sec */
#define MOT_FEC_10MBS		10000000        /* bits per sec */

/*
 * user flags: full duplex mode, loopback mode, serial interface etc.
 * the user may configure some of this options according to his needs
 * by setting the related bits in the <userFlags> field of the load string.
 */

#define MOT_FEC_USR_PHY_NO_AN	0x00000001	/* do not auto-negotiate */
#define MOT_FEC_USR_PHY_TBL	0x00000002	/* use negotiation table */
#define MOT_FEC_USR_PHY_NO_FD	0x00000004	/* do not use full duplex */
#define MOT_FEC_USR_PHY_NO_100	0x00000008	/* do not use 100Mbit speed */
#define MOT_FEC_USR_PHY_NO_HD	0x00000010	/* do not use half duplex */
#define MOT_FEC_USR_PHY_NO_10	0x00000020	/* do not use 10Mbit speed */
#define MOT_FEC_USR_PHY_ISO	0x00000100	/* isolate a PHY */
#define MOT_FEC_USR_SER		0x00000200	/* 7-wire serial interface */
#define MOT_FEC_USR_LOOP	0x00000400	/* loopback mode */
						/* only use it for testing */
#define MOT_FEC_USR_HBC		0x00000080	/* perform heartbeat control */

#define MOT_FEC_TBD_OK		0x1		/* the TBD is a good one */
#define MOT_FEC_TBD_BUSY	0x2		/* the TBD has not been used */
#define MOT_FEC_TBD_ERROR	0x4		/* the TBD is errored */

#define PKT_TYPE_MULTI		0x1	/* packet with a multicast address */
#define PKT_TYPE_UNI		0x2	/* packet with a unicast address */
#define PKT_TYPE_NONE		0x4	/* address type is not meaningful */

#define BUF_TYPE_CL		0x1	/* this's a cluster pointer */
#define BUF_TYPE_MBLK		0x2	/* this's a mblk pointer */

/* frame descriptors definitions */

typedef char *			MOT_FEC_BD_ID;
typedef MOT_FEC_BD_ID	  	MOT_FEC_TBD_ID;
typedef MOT_FEC_BD_ID	  	MOT_FEC_RBD_ID;

/* MII definitions */

#define ETHER_CRC_LEN		0x4	/* CRC length in bytes */
#define MII_MAX_PHY_NUM		0x20	/* max number of attached PHYs */

#define MII_CTRL_REG		0x0	/* Control Register */
#define MII_STAT_REG		0x1	/* Status Register */
#define MII_PHY_ID1_REG		0x2	/* PHY identifier 1 Register */
#define MII_PHY_ID2_REG		0x3	/* PHY identifier 2 Register */
#define MII_AN_ADS_REG		0x4	/* Auto-Negotiation 	  */
					/* Advertisement Register */
#define MII_AN_PRTN_REG		0x5	/* Auto-Negotiation 	    */
					/* partner ability Register */
#define MII_AN_EXP_REG		0x6	/* Auto-Negotiation   */
					/* Expansion Register */
#define MII_AN_NEXT_REG		0x7	/* Auto-Negotiation 	       */
					/* next-page transmit Register */

/* MII control register bit  */

#define MII_CR_COLL_TEST	0x0080		/* collision test */
#define MII_CR_FDX		0x0100		/* FDX =1, half duplex =0 */
#define MII_CR_RESTART		0x0200		/* restart auto negotiation */
#define MII_CR_ISOLATE		0x0400		/* isolate PHY from MII */
#define MII_CR_POWER_DOWN	0x0800		/* power down */
#define MII_CR_AUTO_EN		0x1000		/* auto-negotiation enable */
#define MII_CR_100		0x2000		/* 0 = 10mb, 1 = 100mb */
#define MII_CR_LOOPBACK		0x4000		/* 0 = normal, 1 = loopback */
#define MII_CR_RESET		0x8000		/* 0 = normal, 1 = PHY reset */
#define MII_CR_NORM_EN		0x0000		/* just enable the PHY */

/* MII Status register bit definitions */

#define MII_SR_LINK_STATUS	0x0004       	/* link Status -- 1 = link */
#define MII_SR_AUTO_SEL		0x0008       	/* auto speed select capable */
#define MII_SR_REMOTE_FAULT     0x0010      	/* Remote fault detect */
#define MII_SR_AUTO_NEG         0x0020      	/* auto negotiation complete */
#define MII_SR_10T_HALF_DPX     0x0800     	/* 10BaseT HD capable */
#define MII_SR_10T_FULL_DPX     0x1000    	/* 10BaseT FD capable */
#define MII_SR_TX_HALF_DPX      0x2000    	/* TX HD capable */
#define MII_SR_TX_FULL_DPX      0x4000     	/* TX FD capable */
#define MII_SR_T4               0x8000    	/* T4 capable */

/* MII Link Code word  bit definitions */

#define MII_BP_FAULT	0x2000       	/* remote fault */
#define MII_BP_ACK	0x4000       	/* acknowledge */
#define MII_BP_NP	0x8000       	/* nexp page is supported */

/* MII Next Page bit definitions */

#define MII_NP_TOGGLE	0x0800       	/* toggle bit */
#define MII_NP_ACK2	0x1000       	/* acknowledge two */
#define MII_NP_MSG	0x2000       	/* message page */
#define MII_NP_ACK1	0x4000       	/* acknowledge one */
#define MII_NP_NP	0x8000       	/* nexp page will follow */

/* MII Expansion Register bit definitions */

#define MII_EXP_FAULT	0x0010       	/* parallel detection fault */
#define MII_EXP_PRTN_NP	0x0008       	/* link partner next-page able */
#define MII_EXP_LOC_NP	0x0004       	/* local PHY next-page able */
#define MII_EXP_PR	0x0002       	/* full page received */
#define MII_EXP_PRT_AN	0x0001       	/* link partner auto negotiation able */

/* technology ability field bit definitions */

#define MII_TECH_10BASE_T	0x0020	/* 10Base-T */
#define MII_TECH_10BASE_FD	0x0040	/* 10Base-T Full Duplex */
#define MII_TECH_100BASE_TX	0x0080	/* 100Base-TX */
#define MII_TECH_100BASE_TX_FD	0x0100	/* 100Base-TX Full Duplex */
#define MII_TECH_100BASE_T4	0x0200	/* 100Base-T4 */
#define MII_TECH_MASK		0x1fe0	/* technology abilities mask */

#define MII_AN_FAIL		0x10	/* auto-negotiation fail */
#define MII_STAT_FAIL		0x20	/* errors in the status register */
#define MOT_FEC_PHY_NO_ABLE	0x40	/* the PHY lacks some abilities */

/*
 * this table may be customized by the user in configNet.h
 */

IMPORT INT16 motFecPhyAnOrderTbl [];

/*
 * the table below is used to translate user settings
 * into MII-standard values for technology abilities.
 */

LOCAL UINT16 miiAnLookupTbl [] = {
				  MII_TECH_10BASE_T,	
				  MII_TECH_10BASE_FD,
				  MII_TECH_100BASE_TX,
				  MII_TECH_100BASE_TX_FD,
				  MII_TECH_100BASE_T4	
			         };

LOCAL UINT16 miiCrLookupTbl [] = {
				  MII_CR_NORM_EN,	
				  MII_CR_FDX,
				  MII_CR_100,
				  (MII_CR_100 | MII_CR_FDX),
				  (MII_CR_100 | MII_CR_FDX)	
			         };

typedef struct mii_regs
    {
    UINT16		phyStatus;	/* PHY's status register */
    UINT16		phyCtrl;	/* PHY's control register */
    UINT16		phyId1;		/* PHY's identifier field 1 */
    UINT16		phyId2;		/* PHY's identifier field 2 */
    UINT16		phyAds;		/* PHY's advertisement register */
    UINT16		phyPrtn;	/* PHY's partner register */
    UINT16		phyExp;		/* PHY's expansion register */
    UINT16		phyNext;	/* PHY's next paget transmit register */
    } MII_REGS;

typedef struct phy_info
    {
    MII_REGS		miiRegs;	/* PHY registers */
    UINT8		phyAddr;	/* address of the PHY to be used */
    UINT8		isoPhyAddr;	/* address of a PHY to isolate */
    UINT32		phyFlags;	/* some flags */
    UINT32              phySpeed;     	/* 10/100 Mbit/sec */
    UINT32              phyMode;     	/* half/full duplex mode */
    UINT32              phyDefMode;    	/* default operating mode */
    } PHY_INFO;

typedef struct mot_fec_tbd_list
    {
    UINT16			fragNum;
    UINT16			pktType;
    UCHAR *			pBuf;
    UINT16			bufType;
    struct mot_fec_tbd_list *	pNext;
    MOT_FEC_TBD_ID		pTbd;
    } MOT_FEC_TBD_LIST;

typedef MOT_FEC_TBD_LIST *	MOT_FEC_TBD_LIST_ID;

/* The definition of the driver control structure */

typedef struct drv_ctrl
    {
    END_OBJ             endObj;         /* base class */
    int                 unit;           /* unit number */
    UINT32              motCpmAddr;     /* internal RAM base address */
    int                 ivec;           /* interrupt vector number */
    int                 ilevel;         /* interrupt level */
    UINT32              fifoTxBase;     /* address of Tx FIFO in internal RAM */
    UINT32              fifoRxBase;     /* address of Rx FIFO in internal RAM */
    char *              pBufBase;       /* FEC memory pool base */
    ULONG               bufSize;        /* FEC memory pool size */
    UINT16              rbdNum;         /* number of RBDs */
    MOT_FEC_RBD_ID	rbdBase;        /* RBD ring */
    UINT16              rbdIndex;       /* RBD index */
    UINT16              tbdNum;         /* number of TBDs */
    MOT_FEC_TBD_ID      tbdBase;        /* TBD ring */
    UINT16              tbdIndex;       /* TBD index */
    UINT16              usedTbdIndex;   /* used TBD index */
    UINT16              cleanTbdNum;    /* number of clean TBDs */
    BOOL                txStall;        /* tx handler stalled - no Tbd */
    MOT_FEC_TBD_LIST * 	pTbdList [MOT_FEC_TBD_MAX];
					/* list of TBDs */
    ULONG               userFlags;      /* some user flags */
    INT8                flags;          /* driver state */
    BOOL                loaded;         /* interface has been loaded */
    BOOL                intrConnect;    /* interrupt has been connected */
    UINT32		intMask;	/* interrupt mask register */
    UCHAR *		pTxPollBuf;	/* cluster pointer for poll mode */
    UCHAR *            	rxBuf[MOT_FEC_RBD_MAX];
                                        /* array of pointers to clusters */
    SEM_ID		miiSem;		/* synch semaphore for mii frames */
    SEM_ID		graSem;		/* synch semaphore for graceful */
					/* transmit command */
    char *              pClBlkArea;     /* cluster block pointer */
    UINT32              clBlkSize;     	/* clusters block memory size */
    char *              pMBlkArea;     	/* mBlock area pointer */
    UINT32              mBlkSize;     	/* mBlocks area memory size */
    CACHE_FUNCS         bdCacheFuncs;   /* cache descriptor */
    CACHE_FUNCS         bufCacheFuncs;  /* cache descriptor */
    CL_POOL_ID          pClPoolId;      /* cluster pool identifier */
    PHY_INFO		*phyInfo;	/* info on a MII-compliant PHY */
    UINT32		clockSpeed;	/* clock speed (Hz) for MII_SPEED */
    } DRV_CTRL;

/*
 * this cache functions descriptors is used to flush/invalidate
 * the FEC's data buffers. They are set to the system's cache
 * flush and invalidate routine. This will allow proper operation
 * of the driver if data cache are turned on.
 */

IMPORT STATUS   cacheArchInvalidate (CACHE_TYPE, void *, size_t);

IMPORT STATUS   cacheArchFlush (CACHE_TYPE, void *, size_t);

LOCAL   CACHE_FUNCS     motFecBufCacheFuncs;
LOCAL	FUNCPTR motFecBufInvRtn = cacheArchInvalidate;
LOCAL	FUNCPTR motFecBufFlushRtn = cacheArchFlush;

FUNCPTR motFecIntDisc = NULL;   /* assign a proper disc routine */
IMPORT STATUS	sysFecEnetAddrGet (UINT32 motCmpAddr, UCHAR * address);
IMPORT STATUS	sysFecEnetEnable (UINT32 motCmpAddr);
IMPORT STATUS	sysFecEnetDisable (UINT32 motCmpAddr);
IMPORT FUNCPTR  _func_motFecPhyInit;
IMPORT FUNCPTR  _func_motFecHbFail;

#ifdef __cplusplus
}
#endif

#endif /* __INCmotFecEndh */

