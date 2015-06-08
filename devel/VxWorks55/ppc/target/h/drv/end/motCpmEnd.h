/* motCpmEnd.h - Motorola CPM core Ethernet interface header */

/* Copyright 1996 - 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,24jan02,rcs  removed #include "drv/netif/if_cpm.h"
01f,24jan02,rcs  moved General SCC Mode Register definitions,
                 SCC Ethernet Protocol Specific Mode Register definitions,
                 SCC Ethernet Event and Mask Register definitions,
                 SCC Ethernet Receive Buffer Descriptor definitions,
                 SCC Ethernet Transmit Buffer Descriptor definitions,
                 SCC Serial Comunications Controller,
                 SCC device descriptor, and 
                 #include "drv/multi/ppc860Cpm.h" from h/drv/netif/if_cpm.h
01e,08jan02,mil  Backed out last update "Remove compilation warning".
01d,22dec01,g_h  Remove compilation warning
01c,28aug98,dat  changed all motCmp names to motCpm.
01b,29jun98,cn   fixed compiler errors for CPUs other than CPU32 or PPC860.
01a,22jun98,cn   created. 
*/

/*
This file contains definitions to support the end-style Network Driver 
for the Motorola CPM core Ethernet controller used in the M68EN360 and 
PPC800-series communications controllers. These definitions are compiler
dependant, meaning that their values are based on the particular cpu
being used.
*/

#ifndef	__INCmotCpmEndh
#define	__INCmotCpmEndh

#ifdef	__cplusplus
extern "C" {
#endif

#if (CPU == PPC860)
#include "drv/multi/ppc860Cpm.h"
#include "drv/multi/ppc860Siu.h"
#else
#include "drv/multi/m68360.h" 	/* default: use CPU32 include file */
#endif /* CPU == PPC860 */

/* defines */

/*
 * the following may be redefined in the bsp to account for the actual
 * number of SCC devices in the system.
 */

#ifndef MAX_SCC_CHANNELS
#define MAX_SCC_CHANNELS  4       /* max SCC number for Ethernet channels */
#endif /* MAX_SCC_CHANNELS */

#if (CPU == PPC860)

#define CPM_DPR_SCC1(baseAddr)  MPC860_DPR_SCC1 (baseAddr)
#define CPM_GSMR_L1(baseAddr)   GSMR_L1 (baseAddr)
#define CPM_CIMR_SCC4           CIMR_SCC4
#define CPM_CIMR(baseAddr)      CIMR (baseAddr)
#define CPM_CISR(baseAddr)      CISR (baseAddr)
#define CPM_CPCR(baseAddr)      CPCR (baseAddr)
#define END_OBJ_STRING		"MPC860 Power-QUICC Enhanced Network Driver"
#define MOT_DEV_NAME "cpm"
#define MOT_DEV_NAME_LEN 4

/* General SCC Mode Register definitions */

#define SCC_GSMRL_HDLC          0x00000000      /* HDLC mode */
#define SCC_GSMRL_APPLETALK     0x00000002      /* AppleTalk mode (LocalTalk) */
#define SCC_GSMRL_SS7           0x00000003      /* SS7 mode (microcode) */
#define SCC_GSMRL_UART          0x00000004      /* UART mode */
#define SCC_GSMRL_PROFI_BUS     0x00000005      /* Profi-Bus mode (microcode) */
#define SCC_GSMRL_ASYNC_HDLC    0x00000006      /* async HDLC mode (microcode)*/
#define SCC_GSMRL_V14           0x00000007      /* V.14 mode */
#define SCC_GSMRL_BISYNC        0x00000008      /* BISYNC mode */
#define SCC_GSMRL_DDCMP         0x00000009      /* DDCMP mode (microcode) */
#define SCC_GSMRL_ETHERNET      0x0000000c      /* ethernet mode (SCC1 only) */
#define SCC_GSMRL_ENT           0x00000010      /* enable transmitter */
#define SCC_GSMRL_ENR           0x00000020      /* enable receiver */
#define SCC_GSMRL_LOOPBACK      0x00000040      /* local loopback mode */
#define SCC_GSMRL_ECHO          0x00000080      /* automatic echo mode */
#define SCC_GSMRL_TENC          0x00000700      /* transmitter encoding method*/
#define SCC_GSMRL_RENC          0x00003800      /* receiver encoding method */
#define SCC_GSMRL_RDCR_X8       0x00004000      /* receive DPLL clock x8 */
#define SCC_GSMRL_RDCR_X16      0x00008000      /* receive DPLL clock x16 */
#define SCC_GSMRL_RDCR_X32      0x0000c000      /* receive DPLL clock x32 */
#define SCC_GSMRL_TDCR_X8       0x00010000      /* transmit DPLL clock x8 */
#define SCC_GSMRL_TDCR_X16      0x00020000      /* transmit DPLL clock x16 */
#define SCC_GSMRL_TDCR_X32      0x00030000      /* transmit DPLL clock x32 */
#define SCC_GSMRL_TEND          0x00040000      /* transmitter frame ending */
#define SCC_GSMRL_TPP_00        0x00180000      /* Tx preamble pattern = 00 */
#define SCC_GSMRL_TPP_10        0x00080000      /* Tx preamble pattern = 10 */
#define SCC_GSMRL_TPP_01        0x00100000      /* Tx preamble pattern = 01 */
#define SCC_GSMRL_TPP_11        0x00180000      /* Tx preamble pattern = 11 */
#define SCC_GSMRL_TPL_NONE      0x00000000      /* no Tx preamble (default) */
#define SCC_GSMRL_TPL_8         0x00200000      /* Tx preamble = 1 byte */
#define SCC_GSMRL_TPL_16        0x00400000      /* Tx preamble = 2 bytes */
#define SCC_GSMRL_TPL_32        0x00600000      /* Tx preamble = 4 bytes */
#define SCC_GSMRL_TPL_48        0x00800000      /* Tx preamble = 6 bytes */
#define SCC_GSMRL_TPL_64        0x00a00000      /* Tx preamble = 8 bytes */
#define SCC_GSMRL_TPL_128       0x00c00000      /* Tx preamble = 16 bytes */
#define SCC_GSMRL_TINV          0x01000000      /* DPLL transmit input invert */
#define SCC_GSMRL_RINV          0x02000000      /* DPLL receive input invert */
#define SCC_GSMRL_TSNC          0x0c000000      /* transmit sense */
#define SCC_GSMRL_TCI           0x10000000      /* transmit clock invert */
#define SCC_GSMRL_EDGE          0x60000000      /* adjustment edge +/- */
 
#define SCC_GSMRH_RSYN          0x00000001      /* receive sync timing*/
#define SCC_GSMRH_RTSM          0x00000002      /* RTS* mode */
#define SCC_GSMRH_SYNL          0x0000000c      /* sync length */
#define SCC_GSMRH_TXSY          0x00000010      /* transmitter/receiver sync */
#define SCC_GSMRH_RFW           0x00000020      /* Rx FIFO width */
#define SCC_GSMRH_TFL           0x00000040      /* transmit FIFO length */
#define SCC_GSMRH_CTSS          0x00000080      /* CTS* sampling */
#define SCC_GSMRH_CDS           0x00000100      /* CD* sampling */
#define SCC_GSMRH_CTSP          0x00000200      /* CTS* pulse */
#define SCC_GSMRH_CDP           0x00000400      /* CD* pulse */
#define SCC_GSMRH_TTX           0x00000800      /* transparent transmitter */
#define SCC_GSMRH_TRX           0x00001000      /* transparent receiver */
#define SCC_GSMRH_REVD          0x00002000      /* reverse data */
#define SCC_GSMRH_TCRC          0x0000c000      /* transparent CRC */
#define SCC_GSMRH_GDE           0x00010000      /* glitch detect enable */

/* CPM - Communication Processor Module */

/* SCC Ethernet Protocol Specific Mode Register definitions */
 
#define SCC_ETHER_PSMR_NIB_13   0x0000          /* SFD 13 bits after TENA */
#define SCC_ETHER_PSMR_NIB_14   0x0002          /* SFD 14 bits after TENA */
#define SCC_ETHER_PSMR_NIB_15   0x0004          /* SFD 15 bits after TENA */
#define SCC_ETHER_PSMR_NIB_16   0x0006          /* SFD 16 bits after TENA */
#define SCC_ETHER_PSMR_NIB_21   0x0008          /* SFD 21 bits after TENA */
#define SCC_ETHER_PSMR_NIB_22   0x000a          /* SFD 22 bits after TENA */
#define SCC_ETHER_PSMR_NIB_23   0x000c          /* SFD 23 bits after TENA */
#define SCC_ETHER_PSMR_NIB_24   0x000e          /* SFD 24 bits after TENA */
#define SCC_ETHER_PSMR_LCW      0x0100          /* late collision window */
#define SCC_ETHER_PSMR_SIP      0x0200          /* sample input pins */
#define SCC_ETHER_PSMR_LPB      0x0400          /* loopback operation */
#define SCC_ETHER_PSMR_SBT      0x0800          /* stop backoff timer */
#define SCC_ETHER_PSMR_BRO      0x0100          /* broadcast address */
#define SCC_ETHER_PSMR_PRO      0x0200          /* promiscuous mode */
#define SCC_ETHER_PSMR_CRC      0x0800          /* CRC selection */
#define SCC_ETHER_PSMR_IAM      0x1000          /* individual address mode */
#define SCC_ETHER_PSMR_RSH      0x2000          /* receive short frame */
#define SCC_ETHER_PSMR_FC       0x4000          /* force collision */
#define SCC_ETHER_PSMR_HBC      0x8000          /* heartbeat checking*/
 
/* SCC Ethernet Event and Mask Register definitions */
 
#define SCC_ETHER_SCCX_RXB      0x0001          /* buffer received event */
#define SCC_ETHER_SCCX_TXB      0x0002          /* buffer transmitted event */
#define SCC_ETHER_SCCX_BSY      0x0004          /* busy condition */
#define SCC_ETHER_SCCX_RXF      0x0008          /* frame received event */
#define SCC_ETHER_SCCX_TXE      0x0010          /* transmission error event */
#define SCC_ETHER_SCCX_GRA      0x0080          /* graceful stop event */
 
/* SCC Ethernet Receive Buffer Descriptor definitions */
 
#define SCC_ETHER_RX_BD_CL      0x0001          /* collision condition */
#define SCC_ETHER_RX_BD_OV      0x0002          /* overrun condition */
#define SCC_ETHER_RX_BD_CR      0x0004          /* Rx CRC error */
#define SCC_ETHER_RX_BD_SH      0x0008          /* short frame received */
#define SCC_ETHER_RX_BD_NO      0x0010          /* Rx nonoctet aligned frame */
#define SCC_ETHER_RX_BD_LG      0x0020          /* Rx frame length violation */
#define SCC_ETHER_RX_BD_M       0x0100          /* miss bit for prom mode */
#define SCC_ETHER_RX_BD_F       0x0400          /* buffer is first in frame */
#define SCC_ETHER_RX_BD_L       0x0800          /* buffer is last in frame */
#define SCC_ETHER_RX_BD_I       0x1000          /* interrupt on receive */
#define SCC_ETHER_RX_BD_W       0x2000          /* last BD in ring */
#define SCC_ETHER_RX_BD_E       0x8000          /* buffer is empty */
 
/* SCC Ethernet Transmit Buffer Descriptor definitions */
 
#define SCC_ETHER_TX_BD_CSL     0x0001          /* carrier sense lost */
#define SCC_ETHER_TX_BD_UN      0x0002          /* underrun */
#define SCC_ETHER_TX_BD_RC      0x003c          /* retry count */
#define SCC_ETHER_TX_BD_RL      0x0040          /* retransmission limit */
#define SCC_ETHER_TX_BD_LC      0x0080          /* late collision */
#define SCC_ETHER_TX_BD_HB      0x0100          /* heartbeat */
#define SCC_ETHER_TX_BD_DEF     0x0200          /* defer indication */
#define SCC_ETHER_TX_BD_TC      0x0400          /* auto transmit CRC */
#define SCC_ETHER_TX_BD_L       0x0800          /* buffer is last in frame */
#define SCC_ETHER_TX_BD_I       0x1000          /* interrupt on transmit */
#define SCC_ETHER_TX_BD_W       0x2000          /* last BD in ring */
#define SCC_ETHER_TX_BD_PAD     0x4000          /* auto pad short frames */
#define SCC_ETHER_TX_BD_R       0x8000          /* buffer is ready */

/* typedefs */

/* SCC - Serial Comunications Controller */

typedef struct          /* SCC_ETHER_PROTO */
    {
    UINT32      	c_pres;             /* preset CRC */
    UINT32      	c_mask;             /* constant mask for CRC */
    volatile UINT32     crcec;              /* CRC error counter */
    volatile UINT32     alec;               /* alignment error counter */
    volatile UINT32     disfc;              /* discard frame counter */
    UINT16      	pads;               /* short frame pad value */
    UINT16      	ret_lim;            /* retry limit threshold */
    volatile UINT16     ret_cnt;            /* retry limit counter */
    UINT16      	mflr;               /* maximum frame length register */
    UINT16      	minflr;             /* minimum frame length register */
    UINT16      	maxd1;              /* max DMA1 length register */
    UINT16      	maxd2;              /* max DMA2 length register */
    volatile UINT16     maxd;               /* Rx max DMA */
    volatile UINT16     dma_cnt;            /* Rx DMA counter */
    volatile UINT16     max_b;              /* max BD byte count */
    UINT16      	gaddr1;             /* group address filter 1 */
    UINT16      	gaddr2;             /* group address filter 2 */
    UINT16      	gaddr3;             /* group address filter 3 */
    UINT16      	gaddr4;             /* group address filter 4 */
    volatile UINT32     tbuf0_data0;        /* save area 0 - current frame */
    volatile UINT32     tbuf0_data1;        /* save area 1 - current frame */
    volatile UINT32     tbuf0_rba0;         /* ? */
    volatile UINT32     tbuf0_crc;          /* ? */
    volatile UINT16     tbuf0_bcnt;         /* ? */
    UINT16      	paddr1_h;           /* physical address 1 (MSB) */
    UINT16      	paddr1_m;           /* physical address 1 */
    UINT16      	paddr1_l;           /* physical address 1 (LSB) */
    UINT16      	p_per;              /* persistence */
    volatile UINT16     rfbd_ptr;           /* Rx first BD pointer */
    volatile UINT16     tfbd_ptr;           /* Tx first BD pointer */
    volatile UINT16     tlbd_ptr;           /* Tx last BD pointer */
    volatile UINT32     tbuf1_data0;        /* save area 0 - next frame */
    volatile UINT32     tbuf1_data1;        /* ? */
    volatile UINT32     tbuf1_rba0;         /* ? */
    volatile UINT32     tbuf1_crc;          /* ? */
    volatile UINT16     tbuf1_bcnt;         /* ? */
    volatile UINT16     tx_len;             /* Tx frame length counter */
    UINT16      	iaddr1;             /* individual address filter 1 */
    UINT16      	iaddr2;             /* individual address filter 2 */
    UINT16      	iaddr3;             /* individual address filter 3 */
    UINT16      	iaddr4;             /* individual address filter 4 */
    volatile UINT16     boff_cnt;           /* backoff counter */
    UINT16      	taddr_h;            /* temp address (MSB) */
    UINT16      	taddr_m;            /* temp address */
    UINT16      	taddr_l;            /* temp address (LSB) */
    } SCC_ETHER_PROTO;
 
/* SCC device descriptor */

typedef struct          /* SCC_ETHER_DEV */
    {
    int                 sccNum;         /* number of SCC device */
    int                 txBdNum;        /* number of transmit buf descriptors */
    int                 rxBdNum;        /* number of receive buf descriptors */
    SCC_BUF *           txBdBase;       /* transmit BD base address */
    SCC_BUF *           rxBdBase;       /* receive BD base address */
    u_char *            txBufBase;      /* transmit buffer base address */
    u_char *            rxBufBase;      /* receive buffer base address */
    UINT32              txBufSize;      /* transmit buffer size */
    UINT32              rxBufSize;      /* receive buffer size */
    int                 txBdNext;       /* next transmit BD to fill */
    int                 rxBdNext;       /* next receive BD to read */
    volatile SCC *      pScc;           /* SCC parameter RAM */
    volatile SCC_REG *  pSccReg;        /* SCC registers */
    UINT32              intMask;        /* interrupt acknowledge mask */
    } SCC_ETHER_DEV;

#else				/* default: use CPU32 definitions */

#define SCC_ETHER_DEV		SCC_DEV
#define CPM_DPR_SCC1(baseAddr)  M360_DPR_SCC1 (baseAddr)
#define CPM_GSMR_L1(baseAddr)   M360_CPM_GSMR_L1 (baseAddr)
#define CPM_CIMR_SCC4           CPIC_CIXR_SCC4
#define CPM_CIMR(baseAddr)      M360_CPM_CIMR (baseAddr)
#define CPM_CISR(baseAddr)      M360_CPM_CISR (baseAddr)
#define CPM_CPCR(baseAddr)      M360_CPM_CR (baseAddr)
#define END_OBJ_STRING		"MC68EN360 QUICC Enhanced Network Driver"
#define MOT_DEV_NAME "qu"
#define MOT_DEV_NAME_LEN 3

#endif /* CPU == PPC860 */

/* bsp-specific routine to include */

#if (CPU == PPC860)

#define SYS_ENET_ADDR_GET(address)                                  	\
if (sysCpmEnetAddrGet != NULL)                                          \
    if (sysCpmEnetAddrGet (pDrvCtrl->unit, (address)) == ERROR)		\
	{								\
	errnoSet (S_iosLib_INVALID_ETHERNET_ADDRESS);			\
	return (NULL);							\
	}
 
#define SYS_ENET_ENABLE                                             	\
if (sysCpmEnetEnable != NULL)                                           \
    if (sysCpmEnetEnable (pDrvCtrl->unit) == ERROR)			\
	return (ERROR);
 
#define SYS_ENET_DISABLE                                            	\
if (sysCpmEnetDisable != NULL)                                          \
    sysCpmEnetDisable (pDrvCtrl->unit);
 
#else				/* default: use CPU32 definitions */

#define SYS_ENET_ADDR_GET(address)                                  	\
if (sys360EnetAddrGet != NULL)                                          \
    if (sys360EnetAddrGet (pDrvCtrl->unit, (address)) == ERROR)		\
	{								\
	errnoSet (S_iosLib_INVALID_ETHERNET_ADDRESS);			\
	return (NULL);							\
	}
 
#define SYS_ENET_ENABLE                                             	\
if (sys360EnetEnable != NULL)                                           \
    if (sys360EnetEnable (pDrvCtrl->unit, pDrvCtrl->regBase) == ERROR)  \
	return (ERROR);
 
#define SYS_ENET_DISABLE                                            	\
if (sys360EnetDisable != NULL)                                          \
    sys360EnetDisable (pDrvCtrl->unit, pDrvCtrl->regBase);
 
#endif /* CPU == PPC860 */

/* globals */
 
#if (CPU == PPC860)

IMPORT STATUS sysCpmEnetEnable  (int unit); /* enable ctrl */
IMPORT void   sysCpmEnetDisable (int unit); /* disable ctrl */
IMPORT STATUS sysCpmEnetAddrGet (int unit, u_char * addr);  /* get enet addr */

#else				/* default: use CPU32 prototypes */

IMPORT STATUS sys360EnetEnable  (int unit, UINT32 regBase); /* enable ctrl */
IMPORT void   sys360EnetDisable (int unit, UINT32 regBase); /* disable ctrl */
IMPORT STATUS sys360EnetAddrGet (int unit, u_char * addr);  /* get enet addr */

#endif /* CPU == PPC860 */

#ifdef	__cplusplus
}
#endif

#endif	/* __INCmotCpmEndh */
