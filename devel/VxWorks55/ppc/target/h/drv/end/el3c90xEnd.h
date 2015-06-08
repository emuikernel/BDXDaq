/* el3c90xEnd.h - 3Com EtherLink PCI XL END network interface header*/ 

/* Copyright 1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,11oct01,bur  Added more 3Com chip device ids.
01a,11jan99,mtl	 written by teamF1 Inc.
*/

#ifndef __INCel3c90xEndh
#define __INCel3c90xEndh

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef _ASMLANGUAGE

#if FALSE
#define DRV_DEBUG	/* temporary should be taken out */
#endif    
    
#ifdef	DRV_DEBUG
#include "logLib.h"
#define DRV_DEBUG_OFF		0x0000
#define DRV_DEBUG_RX		0x0001
#define	DRV_DEBUG_TX		0x0002
#define DRV_DEBUG_INT		0x0004
#define	DRV_DEBUG_POLL		(DRV_DEBUG_POLL_RX | DRV_DEBUG_POLL_TX)
#define	DRV_DEBUG_POLL_RX	0x0008
#define	DRV_DEBUG_POLL_TX	0x0010
#define	DRV_DEBUG_LOAD		0x0020
#define	DRV_DEBUG_LOAD2		0x0040
#define	DRV_DEBUG_IOCTL		0x0080
#define	DRV_DEBUG_RESET		0x0100
#define	DRV_DEBUG_MCAST		0x0200
#define	DRV_DEBUG_CSR		0x0400
#define DRV_DEBUG_RX_PKT        0x0800
#define DRV_DEBUG_POLL_REDIR	0x10000
#define	DRV_DEBUG_LOG_NVRAM	0x20000
#define DRV_DEBUG_ALL           0xfffff
#endif /* DRV_DEBUG */
    

/* configuration items */

#define EL3C90X_DEV_NAME 	"elPci"	/* device name */
#define EL3C90X_DEV_NAME_LEN 	6	
#define EA_SIZE         	6       /* one Ethernet address */
#define EH_SIZE         	14      /* ethernet header size */
#define EL3C90X_BUFSIZ		1536  	/* aligned packet size */
#define EL_MIN_FBUF    		4     	/* Minsize of first buffer in chain */
#define EL_UPD_CNT		16	/* number of upload descriptors */
#define EL_DND_CNT		16	/* number of download descriptors */

/* Definitions for the drvCtrl specific flags field */

#define EL_MEM_ALLOC_FLAG       0x01	/* allocating memory flag */
#define EL_POLLING              0x02	/* polling flag */
#define EL_MODE_MEM_IO_MAP	0x04	/* dev registers mem mapped */
#define EL_PROMISCUOUS_FLAG     0x08	/* set the promiscuous mode */


/* eeprom defines */

#define EL_EE_WAIT		162	/* 162 usec */
#define EL_EE_RETRY_CNT		100	/* retry count */    
#define EL_EE_READ		0x0080	/* read, 5 bit address */
#define EL_EE_WRITE		0x0040	/* write, 5 bit address */
#define EL_EE_ERASE		0x00c0	/* erase, 5 bit address */
#define EL_EE_EWEN		0x0030	/* erase, no data needed */
#define EL_EE_BUSY		0x8000

/* eeprom contents */
    
#define EL_EE_EADDR0		0x00	/* station address, first word */
#define EL_EE_EADDR1		0x01	/* station address, next word, */
#define EL_EE_EADDR2		0x02	/* station address, last word */
#define EL_EE_PRODID		0x03	/* product ID code */
#define EL_EE_MDATA_DATE	0x04	/* manufacturing data, date */
#define EL_EE_MDATA_DIV		0x05	/* manufacturing data, division */
#define EL_EE_MDATA_PCODE	0x06	/* manufacturing data, product code */
#define EL_EE_MFG_ID		0x07
#define EL_EE_PCI_PARM		0x08
#define EL_EE_ROM_ONFO		0x09
#define EL_EE_OEM_ADR0		0x0A
#define	EL_EE_OEM_ADR1		0x0B
#define EL_EE_OEM_ADR2		0x0C
#define EL_EE_SOFTINFO1		0x0D
#define EL_EE_COMPAT		0x0E
#define EL_EE_SOFTINFO2		0x0F
#define EL_EE_CAPS		0x10	/* capabilities word */
#define EL_EE_RSVD0		0x11
#define EL_EE_ICFG_0		0x12
#define EL_EE_ICFG_1		0x13
#define EL_EE_RSVD1		0x14
#define EL_EE_SOFTINFO3		0x15
#define EL_EE_RSVD_2		0x16
    

/* Bits in the capabilities word */

#define EL_CAPS_PNP		0x0001
#define EL_CAPS_FULL_DUPLEX	0x0002
#define EL_CAPS_LARGE_PKTS	0x0004
#define EL_CAPS_SLAVE_DMA	0x0008
#define EL_CAPS_SECOND_DMA	0x0010
#define EL_CAPS_FULL_BM		0x0020
#define EL_CAPS_FRAG_BM		0x0040
#define EL_CAPS_CRC_PASSTHRU	0x0080
#define EL_CAPS_TXDONE		0x0100
#define EL_CAPS_NO_TXLENGTH	0x0200
#define EL_CAPS_RX_REPEAT	0x0400
#define EL_CAPS_SNOOPING	0x0800
#define EL_CAPS_100MBPS		0x1000
#define EL_CAPS_PWRMGMT		0x2000

	
/* Register layouts. */

#define EL_COMMAND		0x0E
#define EL_STATUS		0x0E

#define EL_TX_STATUS		0x1B
#define EL_TX_FREE		0x1C
#define EL_DMACTL		0x20
#define EL_DOWNLIST_PTR		0x24
#define EL_TX_FREETHRESH	0x2F
#define EL_UPLIST_PTR		0x38
#define EL_UPLIST_STATUS	0x30

#define EL_PKTSTAT_UP_STALLED	0x00002000
#define EL_PKTSTAT_UP_ERROR	0x00004000
#define EL_PKTSTAT_UP_CMPLT	0x00008000

#define EL_DMACTL_DN_CMPLT_REQ	0x00000002
#define EL_DMACTL_DOWN_STALLED	0x00000004
#define EL_DMACTL_UP_CMPLT	0x00000008
#define EL_DMACTL_DOWN_CMPLT	0x00000010
#define EL_DMACTL_UP_RX_EARLY	0x00000020
#define EL_DMACTL_ARM_COUNTDOWN	0x00000040
#define EL_DMACTL_DOWN_INPROG	0x00000080
#define EL_DMACTL_COUNTER_SPEED	0x00000100
#define EL_DMACTL_DOWNDOWN_MODE	0x00000200
#define EL_DMACTL_TARGET_ABORT	0x40000000
#define EL_DMACTL_MASTER_ABORT	0x80000000

/*
 * Command codes. Some command codes require that we wait for
 * the CMD_BUSY flag to clear. Those codes are marked as 'mustwait.'
 */

#define EL_CMD_RESET		0x0000	/* mustwait */
#define EL_CMD_WINSEL		0x0800
#define EL_CMD_COAX_START	0x1000
#define EL_CMD_RX_DISABLE	0x1800
#define EL_CMD_RX_ENABLE	0x2000
#define EL_CMD_RX_RESET		0x2800	/* mustwait */
#define EL_CMD_UP_STALL		0x3000	/* mustwait */
#define EL_CMD_UP_UNSTALL	0x3001
#define EL_CMD_DOWN_STALL	0x3002	/* mustwait */
#define EL_CMD_DOWN_UNSTALL	0x3003
#define EL_CMD_RX_DISCARD	0x4000
#define EL_CMD_TX_ENABLE	0x4800
#define EL_CMD_TX_DISABLE	0x5000
#define EL_CMD_TX_RESET		0x5800	/* mustwait */
#define EL_CMD_INTR_FAKE	0x6000
#define EL_CMD_INTR_ACK		0x6800
#define EL_CMD_INTR_ENB		0x7000
#define EL_CMD_STAT_ENB		0x7800
#define EL_CMD_RX_SET_FILT	0x8000
#define EL_CMD_RX_SET_THRESH	0x8800
#define EL_CMD_TX_SET_THRESH	0x9000
#define EL_CMD_TX_SET_START	0x9800
#define EL_CMD_DMA_UP		0xA000
#define EL_CMD_DMA_STOP		0xA001
#define EL_CMD_STATS_ENABLE	0xA800
#define EL_CMD_STATS_DISABLE	0xB000
#define EL_CMD_COAX_STOP	0xB800

#define EL_CMD_SET_TX_RECLAIM	0xC000 /* 3c905B only */
#define EL_CMD_RX_SET_HASH	0xC800 /* 3c905B only */

#define EL_HASH_SET		0x0400
#define EL_HASHFILT_SIZE	256

/*
 * status codes
 * Note that bits 15 to 13 indicate the currently visible register window
 * which may be anything from 0 to 7.
 */

#define EL_STAT_INTLATCH	0x0001	/* 0 */
#define EL_STAT_ADFAIL		0x0002	/* 1 */
#define EL_STAT_TX_COMPLETE	0x0004	/* 2 */
#define EL_STAT_TX_AVAIL	0x0008	/* 3 first generation */
#define EL_STAT_RX_COMPLETE	0x0010  /* 4 */
#define EL_STAT_RX_EARLY	0x0020	/* 5 */
#define EL_STAT_INTREQ		0x0040  /* 6 */
#define EL_STAT_STATSOFLOW	0x0080  /* 7 */
#define EL_STAT_DMADONE		0x0100	/* 8 first generation */
#define EL_STAT_LINKSTAT	0x0100	/* 8 3c509B */
#define EL_STAT_DOWN_COMPLETE	0x0200	/* 9 */
#define EL_STAT_UP_COMPLETE	0x0400	/* 10 */
#define EL_STAT_DMABUSY		0x0800	/* 11 first generation */
#define EL_STAT_CMDBUSY		0x1000  /* 12 */

/* interrupts we normally want enabled. */

#define EL_INTRS							\
	(EL_STAT_UP_COMPLETE|EL_STAT_STATSOFLOW|EL_STAT_ADFAIL|		\
	 EL_STAT_DOWN_COMPLETE|EL_STAT_TX_COMPLETE|EL_STAT_INTLATCH)

#define EL_WIN_0    		0
#define EL_WIN_1    		1
#define EL_WIN_2    		2
#define EL_WIN_3    		3
#define EL_WIN_4    		4
#define EL_WIN_5    		5
#define EL_WIN_6    		6
#define EL_WIN_7    		7
    
/* window 0 registers */

#define EL_W0_EE_DATA		0x0C
#define EL_W0_EE_CMD		0x0A
#define EL_W0_RSRC_CFG		0x08
#define EL_W0_ADDR_CFG		0x06
#define EL_W0_CFG_CTRL		0x04
#define EL_W0_PROD_ID		0x02
#define EL_W0_MFG_ID		0x00

/* window 1 */

#define EL_W1_TX_FIFO		0x10
#define EL_W1_FREE_TX		0x0C
#define EL_W1_TX_STATUS		0x0B
#define EL_W1_TX_TIMER		0x0A
#define EL_W1_RX_STATUS		0x08
#define EL_W1_RX_FIFO		0x00

/* RX status codes */

#define EL_RXSTATUS_OVERRUN	0x01
#define EL_RXSTATUS_RUNT	0x02
#define EL_RXSTATUS_ALIGN	0x04
#define EL_RXSTATUS_CRC		0x08
#define EL_RXSTATUS_OVERSIZE	0x10
#define EL_RXSTATUS_DRIBBLE	0x20

/* TX status codes */

#define EL_TXSTATUS_RECLAIM	0x02 	/* 3c905B only */
#define EL_TXSTATUS_OVERFLOW	0x04
#define EL_TXSTATUS_MAXCOLS	0x08
#define EL_TXSTATUS_UNDERRUN	0x10
#define EL_TXSTATUS_JABBER	0x20
#define EL_TXSTATUS_INTREQ	0x40
#define EL_TXSTATUS_COMPLETE	0x80

/* window 2 */

#define EL_W2_RESET_OPTIONS	0x0C	/* 3c905B only */
#define EL_W2_STATION_MASK_HI	0x0A
#define EL_W2_STATION_MASK_MID	0x08
#define EL_W2_STATION_MASK_LO	0x06
#define EL_W2_STATION_ADDR_HI	0x04
#define EL_W2_STATION_ADDR_MID	0x02
#define EL_W2_STATION_ADDR_LO	0x00

#define EL_RESETOPT_FEATUREMASK	(0x0001 | 0x0002 | 0x004)
#define EL_RESETOPT_D3RESETDIS	0x0008
#define EL_RESETOPT_DISADVFD	0x0010
#define EL_RESETOPT_DISADV100	0x0020
#define EL_RESETOPT_DISAUTONEG	0x0040
#define EL_RESETOPT_DEBUGMODE	0x0080
#define EL_RESETOPT_FASTAUTO	0x0100
#define EL_RESETOPT_FASTEE	0x0200
#define EL_RESETOPT_FORCEDCONF	0x0400
#define EL_RESETOPT_TESTPDTPDR	0x0800
#define EL_RESETOPT_TEST100TX	0x1000
#define EL_RESETOPT_TEST100RX	0x2000

/* window 3 (fifo management) */

#define EL_W3_INTERNAL_CFG	0x00
#define EL_W3_RESET_OPT		0x08
#define EL_W3_FREE_TX		0x0C
#define EL_W3_FREE_RX		0x0A
#define EL_W3_MAC_CTRL		0x06

#define EL_ICFG_CONNECTOR_MASK	0x00F00000
#define EL_ICFG_CONNECTOR_BITS	20

#define EL_ICFG_RAMSIZE_MASK	0x00000007
#define EL_ICFG_RAMWIDTH	0x00000008
#define EL_ICFG_ROMSIZE_MASK	(0x00000040 | 0x00000080)
#define EL_ICFG_DISABLE_BASSD	0x00000100
#define EL_ICFG_RAMLOC		0x00000200
#define EL_ICFG_RAMPART		(0x00010000 | 0x00020000)
#define EL_ICFG_XCVRSEL		(0x00100000 | 0x00200000 | 0x00400000)
#define EL_ICFG_AUTOSEL		0x01000000

#define EL_XCVR_10BT		0x00
#define EL_XCVR_AUI		0x01
#define EL_XCVR_RSVD_0		0x02
#define EL_XCVR_COAX		0x03
#define EL_XCVR_100BTX		0x04
#define EL_XCVR_100BFX		0x05
#define EL_XCVR_MII		0x06
#define EL_XCVR_RSVD_1		0x07
#define EL_XCVR_AUTO		0x08	/* 3c905B only */

#define EL_MACCTRL_DEFER_EXT_END	0x0001
#define EL_MACCTRL_DEFER_0		0x0002
#define EL_MACCTRL_DEFER_1		0x0004
#define EL_MACCTRL_DEFER_2		0x0008
#define EL_MACCTRL_DEFER_3		0x0010
#define EL_MACCTRL_DUPLEX		0x0020
#define EL_MACCTRL_ALLOW_LARGE_PACK	0x0040
#define EL_MACCTRL_EXTEND_AFTER_COL	0x0080 (3c905B only)
#define EL_MACCTRL_FLOW_CONTROL_ENB	0x0100 (3c905B only)
#define EL_MACCTRL_VLT_END		0x0200 (3c905B only)

/*
 * The 'reset options' register contains power-on reset values
 * loaded from the EEPROM. This includes the supported media
 * types on the card. It is also known as the media options register.
 */
#define EL_W3_MEDIA_OPT		0x08

#define EL_MEDIAOPT_BT4		0x0001	/* MII */
#define EL_MEDIAOPT_BTX		0x0002	/* on-chip */
#define EL_MEDIAOPT_BFX		0x0004	/* on-chip */
#define EL_MEDIAOPT_BT		0x0008	/* on-chip */
#define EL_MEDIAOPT_BNC		0x0010	/* on-chip */
#define EL_MEDIAOPT_AUI		0x0020	/* on-chip */
#define EL_MEDIAOPT_MII		0x0040	/* MII */
#define EL_MEDIAOPT_VCO		0x0100	/* 1st gen chip only */

#define EL_MEDIAOPT_10FL	0x0100	/* 3x905B only, on-chip */
#define EL_MEDIAOPT_MASK	0x01FF

/* window 4 (diagnostics) */

#define EL_W4_UPPERBYTESOK	0x0D
#define EL_W4_BADSSD		0x0C
#define EL_W4_MEDIA_STATUS	0x0A
#define EL_W4_PHY_MGMT		0x08
#define EL_W4_NET_DIAG		0x06
#define EL_W4_FIFO_DIAG		0x04
#define EL_W4_VCO_DIAG		0x02

#define EL_W4_CTRLR_STAT	0x08
#define EL_W4_TX_DIAG		0x00

#define EL_MII_CLK		0x01
#define EL_MII_DATA		0x02
#define EL_MII_DIR		0x04

#define EL_MEDIA_SQE		0x0008
#define EL_MEDIA_10TP		0x00C0
#define EL_MEDIA_LNK		0x0080
#define EL_MEDIA_LNKBEAT	0x0800

#define EL_MEDIASTAT_CRCSTRIP	0x0004
#define EL_MEDIASTAT_SQEENB	0x0008
#define EL_MEDIASTAT_COLDET	0x0010
#define EL_MEDIASTAT_CARRIER	0x0020
#define EL_MEDIASTAT_JABGUARD	0x0040
#define EL_MEDIASTAT_LINKBEAT	0x0080
#define EL_MEDIASTAT_JABDETECT	0x0200
#define EL_MEDIASTAT_POLREVERS	0x0400
#define EL_MEDIASTAT_LINKDETECT	0x0800
#define EL_MEDIASTAT_TXINPROG	0x1000
#define EL_MEDIASTAT_DCENB	0x4000
#define EL_MEDIASTAT_AUIDIS	0x8000

#define EL_NETDIAG_TEST_LOWVOLT		0x0001
#define EL_NETDIAG_ASIC_REVMASK		(0x0002|0x0004|0x0008|0x0010|0x0020)
#define EL_NETDIAG_UPPER_BYTES_ENABLE	0x0040
#define EL_NETDIAG_STATS_ENABLED	0x0080
#define EL_NETDIAG_TX_FATALERR		0x0100
#define EL_NETDIAG_TRANSMITTING		0x0200
#define EL_NETDIAG_RX_ENABLED		0x0400
#define EL_NETDIAG_TX_ENABLED		0x0800
#define EL_NETDIAG_FIFO_LOOPBACK	0x1000
#define EL_NETDIAG_MAC_LOOPBACK		0x2000
#define EL_NETDIAG_ENDEC_LOOPBACK	0x4000
#define EL_NETDIAG_EXTERNAL_LOOP	0x8000

/* window 5 */

#define EL_W5_STAT_ENB		0x0C
#define EL_W5_INTR_ENB		0x0A
#define EL_W5_RECLAIM_THRESH	0x09	/* 3c905B only */
#define EL_W5_RX_FILTER		0x08
#define EL_W5_RX_EARLYTHRESH	0x06
#define EL_W5_TX_AVAILTHRESH	0x02
#define EL_W5_TX_STARTTHRESH	0x00

/* RX filter bits */
#define EL_RXFILTER_INDIVIDUAL	0x01
#define EL_RXFILTER_ALLMULTI	0x02
#define EL_RXFILTER_BROADCAST	0x04
#define EL_RXFILTER_ALLFRAMES	0x08
#define EL_RXFILTER_MULTIHASH	0x10 /* 3c905B only */

/* window 6 (stats) */
#define EL_W6_TX_BYTES_OK	0x0C
#define EL_W6_RX_BYTES_OK	0x0A
#define EL_W6_UPPER_FRAMES_OK	0x09
#define EL_W6_DEFERRED		0x08
#define EL_W6_RX_OK		0x07
#define EL_W6_TX_OK		0x06
#define EL_W6_RX_OVERRUN	0x05
#define EL_W6_COL_LATE		0x04
#define EL_W6_COL_SINGLE	0x03
#define EL_W6_COL_MULTIPLE	0x02
#define EL_W6_SQE_ERRORS	0x01
#define EL_W6_CARRIER_LOST	0x00

/* window 7 (bus master control) */
#define EL_W7_BM_ADDR		0x00
#define EL_W7_BM_LEN		0x06
#define EL_W7_BM_STATUS		0x0B
#define EL_W7_BM_TIMEr		0x0A

/* bus master control registers */

#define EL_BM_PKTSTAT		0x20
#define EL_BM_DOWNLISTPTR	0x24
#define EL_BM_FRAGADDR		0x28
#define EL_BM_FRAGLEN		0x2C
#define EL_BM_TXFREETHRESH	0x2F
#define EL_BM_UPPKTSTAT		0x30
#define EL_BM_UPLISTPTR		0x38

#define EL_LAST_FRAG		0x80000000

/* descriptor registers and defines */    

#define EL_MAXFRAGS		63
#define EL_MIN_FRAMELEN		60

#define EL_RXSTAT_LENMASK	0x00001FFF	/* bits 0 to 12 length mask */
#define EL_RXSTAT_UP_ERROR	0x00004000
#define EL_RXSTAT_UP_CMPLT	0x00008000
#define EL_RXSTAT_UP_OVERRUN	0x00010000
#define EL_RXSTAT_RUNT		0x00020000
#define EL_RXSTAT_ALIGN		0x00040000
#define EL_RXSTAT_CRC		0x00080000
#define EL_RXSTAT_OVERSIZE	0x00100000
#define EL_RXSTAT_DRIBBLE	0x00800000
#define EL_RXSTAT_UP_OFLOW	0x01000000
#define EL_RXSTAT_IPCKERR	0x02000000	/* 3c905B only */
#define EL_RXSTAT_TCPCKERR	0x04000000	/* 3c905B only */
#define EL_RXSTAT_UDPCKERR	0x08000000	/* 3c905B only */
#define EL_RXSTAT_BUFEN		0x10000000	/* 3c905B only */
#define EL_RXSTAT_IPCKOK	0x20000000	/* 3c905B only */
#define EL_RXSTAT_TCPCOK	0x40000000	/* 3c905B only */
#define EL_RXSTAT_UDPCKOK	0x80000000	/* 3c905B only */

#define EL_TXSTAT_LENMASK	0x00001FFF	/* bits 0 to 12 length mask */
#define EL_TXSTAT_CRCDIS	0x00002000
#define EL_TXSTAT_TX_INTR	0x00008000
#define EL_TXSTAT_DL_COMPLETE	0x00010000
#define EL_TXSTAT_IPCKSUM	0x02000000	/* 3c905B only */
#define EL_TXSTAT_TCPCKSUM	0x04000000	/* 3c905B only */
#define EL_TXSTAT_UDPCKSUM	0x08000000	/* 3c905B only */
#define EL_TXSTAT_DL_INTR	0x80000000

#define EL_CAPABILITY_BM	0x20

/* MII constants */
    
#define EL_MII_STARTDELIM	0x01
#define EL_MII_READOP		0x02
#define EL_MII_WRITEOP		0x01
#define EL_MII_TURNAROUND	0x02

/*
 * The 3C905B adapters implement a few features that we want to
 * take advantage of, namely the multicast hash filter. With older
 * chips, you only have the option of turning on reception of all
 * multicast frames.
 */
    
#define EL_TYPE_905B	1
#define EL_TYPE_90X	2
#define EL_TIMEOUT		1000

/* General constants . 3Com PCI vendor ID */

#define	TC_VENDORID		0x10B7

/* 3Com chip device IDs.*/
            
#define	TC_DEVICEID_BOOMERANG_10BT		0x9000
#define TC_DEVICEID_BOOMERANG_10BT_COMBO	0x9001
#define TC_DEVICEID_BOOMERANG_10_100BT		0x9050
#define TC_DEVICEID_BOOMERANG_100BT4		0x9051
#define TC_DEVICEID_CYCLONE_10BT		0x9004
#define TC_DEVICEID_CYCLONE_10BT_COMBO		0x9005
#define TC_DEVICEID_CYCLONE_10_100BT		0x9055
#define TC_DEVICEID_CYCLONE_10_100BT4		0x9056
#define TC_DEVICEID_CYCLONE_10_100FX		0x905A
#define TC_DEVICEID_CYCLONE_10_100BT_SERV	0x9800
#define TC_DEVICEID_CYCLONE_10FL		0x900A
#define TC_DEVICEID_CYCLONE_10_100_COMBO	0x9058
#define TC_DEVICEID_KRAKATOA_10BT_TPC		0x9006
#define TC_DEVICEID_TORNADO_10_100BT		0x9200
#define TC_DEVICEID_TORNADO_10_100BT_SERV	0x9805
#define TC_DEVICEID_TORNADO_HOMECONNECT		0x4500
#define TC_DEVICEID_HURRICANE_SOHO100TX		0x7646

/*
 * Texas Instruments PHY identifiers
 *
 * The ThunderLAN manual has a curious and confusing error in it.
 * In chapter 7, which describes PHYs, it says that TI PHYs have
 * the following ID codes, where xx denotes a revision:
 *
 * 0x4000501xx			internal 10baseT PHY
 * 0x4000502xx			TNETE211 100VG-AnyLan PMI
 *
 * The problem here is that these are not valid 32-bit hex numbers:
 * there's one digit too many. My guess is that they mean the internal
 * 10baseT PHY is 0x4000501x and the TNETE211 is 0x4000502x since these
 * are the only numbers that make sense.
 */
#define TI_PHY_VENDORID		0x4000
#define TI_PHY_10BT		0x501F
#define TI_PHY_100VGPMI		0x502F

/*
 * These ID values are for the NS DP83840A 10/100 PHY
 */
#define NS_PHY_VENDORID		0x2000
#define NS_PHY_83840A		0x5C0F

/*
 * Level 1 10/100 PHY
 */
#define LEVEL1_PHY_VENDORID	0x7810
#define LEVEL1_PHY_LXT970	0x000F

/*
 * Intel 82555 10/100 PHY
 */
#define INTEL_PHY_VENDORID	0x0A28
#define INTEL_PHY_82555		0x015F

/*
 * SEEQ 80220 10/100 PHY
 */
#define SEEQ_PHY_VENDORID	0x0016
#define SEEQ_PHY_80220		0xF83F


/*
 * PCI low memory base and low I/O base register, and
 * other PCI registers. Note: some are only available on
 * the 3c905B, in particular those that related to power management.
 */

#define EL_PCI_VENDOR_ID	0x00
#define EL_PCI_DEVICE_ID	0x02
#define EL_PCI_COMMAND		0x04
#define EL_PCI_STATUS		0x06
#define EL_PCI_CLASSCODE	0x09
#define EL_PCI_LATENCY_TIMER	0x0D
#define EL_PCI_HEADER_TYPE	0x0E
#define EL_PCI_LOIO		0x10
#define EL_PCI_LOMEM		0x14
#define EL_PCI_BIOSROM		0x30
#define EL_PCI_INTLINE		0x3C
#define EL_PCI_INTPIN		0x3D
#define EL_PCI_MINGNT		0x3E
#define EL_PCI_MINLAT		0x0F
#define EL_PCI_RESETOPT		0x48
#define EL_PCI_EEPROM_DATA	0x4C

/* 3c905B-only registers */
#define EL_PCI_CAPID		0xDC /* 8 bits */
#define EL_PCI_NEXTPTR		0xDD /* 8 bits */
#define EL_PCI_PWRMGMTCAP	0xDE /* 16 bits */
#define EL_PCI_PWRMGMTCTRL	0xE0 /* 16 bits */

#define EL_PSTATE_MASK		0x0003
#define EL_PSTATE_D0		0x0000
#define EL_PSTATE_D1		0x0002
#define EL_PSTATE_D2		0x0002
#define EL_PSTATE_D3		0x0003
#define EL_PME_EN		0x0010
#define EL_PME_STATUS		0x8000

#define PHY_UNKNOWN		6

#define EL_PHYADDR_MIN		0x00
#define EL_PHYADDR_MAX		0x1F

#define EL_PHY_GENCTL		0x00
#define EL_PHY_GENSTS		0x01
#define EL_PHY_VENID		0x02
#define EL_PHY_DEVID		0x03
#define EL_PHY_ANAR		0x04
#define EL_PHY_LPAR		0x05
#define EL_PHY_ANEXP		0x06

#define PHY_ANAR_NEXTPAGE	0x8000
#define PHY_ANAR_RSVD0		0x4000
#define PHY_ANAR_TLRFLT		0x2000
#define PHY_ANAR_RSVD1		0x1000
#define PHY_ANAR_RSVD2		0x0800
#define PHY_ANAR_RSVD3		0x0400
#define PHY_ANAR_100BT4		0x0200
#define PHY_ANAR_100BTXFULL	0x0100
#define PHY_ANAR_100BTXHALF	0x0080
#define PHY_ANAR_10BTFULL	0x0040
#define PHY_ANAR_10BTHALF	0x0020
#define PHY_ANAR_PROTO4		0x0010
#define PHY_ANAR_PROTO3		0x0008
#define PHY_ANAR_PROTO2		0x0004
#define PHY_ANAR_PROTO1		0x0002
#define PHY_ANAR_PROTO0		0x0001

/*
 * These are the register definitions for the PHY (physical layer
 * interface chip).
 */

/*
 * PHY BMCR Basic Mode Control Register
 */

#define PHY_BMCR		0x00
#define PHY_BMCR_RESET		0x8000
#define PHY_BMCR_LOOPBK		0x4000
#define PHY_BMCR_SPEEDSEL	0x2000
#define PHY_BMCR_AUTONEGENBL	0x1000
#define PHY_BMCR_RSVD0		0x0800	/* write as zero */
#define PHY_BMCR_ISOLATE	0x0400
#define PHY_BMCR_AUTONEGRSTR	0x0200
#define PHY_BMCR_DUPLEX		0x0100
#define PHY_BMCR_COLLTEST	0x0080
#define PHY_BMCR_RSVD1		0x0040	/* write as zero, don't care */
#define PHY_BMCR_RSVD2		0x0020	/* write as zero, don't care */
#define PHY_BMCR_RSVD3		0x0010	/* write as zero, don't care */
#define PHY_BMCR_RSVD4		0x0008	/* write as zero, don't care */
#define PHY_BMCR_RSVD5		0x0004	/* write as zero, don't care */
#define PHY_BMCR_RSVD6		0x0002	/* write as zero, don't care */
#define PHY_BMCR_RSVD7		0x0001	/* write as zero, don't care */

/*
 * RESET: 1 == software reset, 0 == normal operation
 * Resets status and control registers to default values.
 * Relatches all hardware config values.
 *
 * LOOPBK: 1 == loopback operation enabled, 0 == normal operation
 *
 * SPEEDSEL: 1 == 100Mb/s, 0 == 10Mb/s
 * Link speed is selected byt his bit or if auto-negotiation if bit
 * 12 (AUTONEGENBL) is set (in which case the value of this register
 * is ignored).
 *
 * AUTONEGENBL: 1 == Autonegotiation enabled, 0 == Autonegotiation disabled
 * Bits 8 and 13 are ignored when autoneg is set, otherwise bits 8 and 13
 * determine speed and mode. Should be cleared and then set if PHY configured
 * for no autoneg on startup.
 *
 * ISOLATE: 1 == isolate PHY from MII, 0 == normal operation
 *
 * AUTONEGRSTR: 1 == restart autonegotiation, 0 = normal operation
 *
 * DUPLEX: 1 == full duplex mode, 0 == half duplex mode
 *
 * COLLTEST: 1 == collision test enabled, 0 == normal operation
 */

/* 
 * PHY, BMSR Basic Mode Status Register 
 */   
#define PHY_BMSR		0x01
#define PHY_BMSR_100BT4		0x8000
#define PHY_BMSR_100BTXFULL	0x4000
#define PHY_BMSR_100BTXHALF	0x2000
#define PHY_BMSR_10BTFULL	0x1000
#define PHY_BMSR_10BTHALF	0x0800
#define PHY_BMSR_RSVD1		0x0400	/* write as zero, don't care */
#define PHY_BMSR_RSVD2		0x0200	/* write as zero, don't care */
#define PHY_BMSR_RSVD3		0x0100	/* write as zero, don't care */
#define PHY_BMSR_RSVD4		0x0080	/* write as zero, don't care */
#define PHY_BMSR_MFPRESUP	0x0040
#define PHY_BMSR_AUTONEGCOMP	0x0020
#define PHY_BMSR_REMFAULT	0x0010
#define PHY_BMSR_CANAUTONEG	0x0008
#define PHY_BMSR_LINKSTAT	0x0004
#define PHY_BMSR_JABBER		0x0002
#define PHY_BMSR_EXTENDED	0x0001

/* ethernet media */

#define IFM_ETHER		0x00000020
#define	IFM_10_T		3		/* 10BaseT - RJ45 */
#define	IFM_10_2		4		/* 10Base2 - Thinnet */
#define	IFM_10_5		5		/* 10Base5 - AUI */
#define	IFM_100_TX		6		/* 100BaseTX - RJ45 */
#define	IFM_100_FX		7		/* 100BaseFX - Fiber */
#define	IFM_100_T4		8		/* 100BaseT4 - 4 pair cat 3 */
#define	IFM_100_VG		9		/* 100VG-AnyLAN */
#define	IFM_100_T2		10		/* 100BaseT2 */

/* Shared media sub-types */

#define	IFM_AUTO		0	/* Autoselect best media */
#define	IFM_MANUAL		1	/* Jumper/dipswitch selects media */
#define	IFM_NONE		2	/* Deselect all media */

/* Shared options */

#define IFM_FDX			0x00100000	/* Force full duplex */
#define	IFM_HDX			0x00200000	/* Force half duplex */
#define IFM_FLAG0		0x01000000	/* Driver defined flag */
#define IFM_FLAG1		0x02000000	/* Driver defined flag */
#define IFM_FLAG2		0x04000000	/* Driver defined flag */
#define	IFM_LOOP		0x08000000	/* Put hardware in loopback */

/* Masks */

#define	IFM_NMASK		0x000000e0	/* Network type */
#define	IFM_TMASK		0x0000000f	/* Media sub-type */
#define	IFM_IMASK		0xf0000000	/* Instance */
#define	IFM_ISHIFT		28		/* Instance shift */
#define	IFM_OMASK		0x0000ff00	/* Type specific options */
#define	IFM_GMASK		0x0ff00000	/* Global options */

/* Status bits */

#define	IFM_AVALID		0x00000001	/* Active bit valid */
#define	IFM_ACTIVE		0x00000002	/* Iface bound to working net */

    
/*
 * fragment structure in a descriptor each descriptor can have upto
 * 63 fragments
 */
    
typedef struct elFrag
    {
    UINT32	fragAddr;		/* fragment address */
    UINT32	fragLen;		/* fragment length */
    } EL_FRAG;

/* structure of a descriptor with multiple fragments */
    
typedef struct elDesc
    {
    UINT32	nextDesc;		/* final entry has 0 nextptr */
    UINT32	status;			/* status of the descriptor */
    EL_FRAG	descFrag [EL_MAXFRAGS]; /* fragments */
    } EL_DESC;

/* structure of a descriptor with one fragment */

typedef struct elDescOnefrag
    {
    UINT32	nextDesc;		/* final entry has 0 nextptr */
    UINT32	status;			/* status of the descriptor */
    EL_FRAG	descFrag;		/* one fragment only */
    } EL_SIMPLE_DESC;

/* structure of the recv and transmit descriptors laid out in memory */
    
typedef struct elDescBlk
    {
    EL_SIMPLE_DESC	rxDescs [EL_UPD_CNT];
    EL_DESC		txDescs [EL_DND_CNT];
    unsigned char	descPad [EL_MIN_FRAMELEN];
    } EL_DESC_BLK;

/* structure of a linked list holding ptrs to descriptors */
    
typedef struct elChain
    {
    EL_DESC *		pDesc;
    M_BLK *		pMblk;
    struct elChain *	pNextChain;
    } EL_DESC_CHAIN;

/* structure of a linked list holding ptrs to simple descriptors */    

typedef struct elChainOnefrag
    {
    EL_SIMPLE_DESC *		pDesc;
    M_BLK *			pMblk;
    struct elChainOnefrag *	pNextChain;
    } EL_SIMPLE_DESC_CHAIN;

/* structure of recv and transmit chains */
    
typedef struct elRxTxChain
    {
    EL_SIMPLE_DESC_CHAIN	rxChain [EL_UPD_CNT];
    EL_DESC_CHAIN		txChain [EL_DND_CNT];
    EL_SIMPLE_DESC_CHAIN *	pRxHead;
    EL_DESC_CHAIN *		pTxHead;
    EL_DESC_CHAIN *		pTxTail;
    EL_DESC_CHAIN *		pTxFree;
    } EL_RX_TX_CHAIN;
    
/* structure of the device type */
    
typedef struct elType
    {
    UINT16		vendorId;
    UINT16		deviceId;
    char *		devName;
    } EL_DEV_TYPE;

/* structure of the media independant interface frame */
    
typedef struct elMiiFrame
    {
    UINT8		stDelim;
    UINT8		opCode;
    UINT8		phyAddr;
    UINT8		regAddr;
    UINT8		turnAround;
    UINT16		data;
    } EL_MII_FRAME;

/* statistics structure */
    
typedef struct elStats
    {
    UINT8		carrierLost;
    UINT8		sqeErrs;
    UINT8		txMultiCollision;
    UINT8		txSingleCollision;
    UINT8		txLateCollision;
    UINT8		rxOverrun;
    UINT8		txFramesOk;
    UINT8		rxFramesOk;
    UINT8		txDeferred;
    UINT8		upperFramesOk;
    UINT16		rxBytesOk;
    UINT16		txBytesOk;
    UINT16		status;
    } EL_STATS;
    
/* 3com 3c90x device control structure */
    
typedef struct el3c90xDevice
    {
    END_OBJ			endObj;      /* The class we inherit from */
    int 			unit;	     /* unit number of the device */
    UINT32      		flags;       /* Our local flags */
    EL_DEV_TYPE *		pPhyDevType; /* phy info */
    UINT8			devType;     /* 3c90x or 3c905B */
    UINT8			devPhyAddr;  /* PHY address */
    UINT32			xCvr;	     /* transciever */
    UINT16			devMedia;    /* media */
    UINT16			devCaps;     /* capabilities */
    char *			pDescMem;    /* descriptor memory pointer */
    EL_DESC_BLK *	 	pDescBlk;    /* pointer to descriptor blk */
    EL_RX_TX_CHAIN		rxTxChain;   /* recv Transmit chains */
    UINT32      		devAdrs;     /* device structure address */
    int                 	ivec;        /* interrupt vector */
    int                 	intLevel;    /* interrupt level */
    char *			memAdrs;     /* 3COM memory pool base */
    int         		memSize;     /* 3COM memory pool size */
    int         		memWidth;    /* width of data port */
    int         		offset;	     /* offset of data in the buffer */
    int				bufMtplr;   /* buffer multiplier */
    UINT32			pciMemBase;  /* memory base as seen from PCI*/
    UINT8			enetAddr[6]; /* ethernet address */
    CACHE_FUNCS 		cacheFuncs;  /* cache function pointers */
    BOOL        		txBlocked;   /* transmit flow control */
    CL_POOL_ID  		pClPoolId;   /* cluster pool Id */
    M_CL_CONFIG			mClCfg;      /* mBlk & cluster cfg structure */
    CL_DESC			clDesc;	     /* cluster descriptor table */
    } EL3C90X_DEVICE;

#define EL_RX_GOODFRAMES(x) \
	((x.upperFramesOk & 0x03) << 8) | x.rxFramesOk

#define EL_TX_GOODFRAMES(x) \
	((x.upperFramesOk & 0x30) << 4) | x.txFramesOk

/*
 * Macros to extract various bits of information from the media word.
 */
#define	IFM_TYPE(x)	((x) & IFM_NMASK)
#define	IFM_SUBTYPE(x)	((x) & IFM_TMASK)
#define	IFM_INST(x)	(((x) & IFM_IMASK) >> IFM_ISHIFT)

/* macro to select a window */
            
#define EL_SEL_WIN(x)	\
	SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND, \
                                  EL_CMD_WINSEL | (x)))

/*
 * MII access routines are provided for adapters with external
 * PHYs (3c905-TX, 3c905-T4, 3c905B-T4) and those with built-in
 * autoneg logic that's faked up to look like a PHY (3c905B-TX).
 * Note: if you don't perform the MDIO operations just right,
 * it's possible to end up with code that works correctly with
 * some chips/CPUs/processor speeds/bus speeds/etc but not
 * with others.
 */
#define EL_MII_SET(x)					                     \
        el3c90xCsrWriteWord (pDrvCtrl, EL_W4_PHY_MGMT,                       \
                             (el3c90xCsrReadWord(pDrvCtrl,                   \
                                                 EL_W4_PHY_MGMT,             \
                                                 EL_WIN_4) | (x)), EL_WIN_4)
    
#define EL_MII_CLR(x)					                     \
        el3c90xCsrWriteWord (pDrvCtrl, EL_W4_PHY_MGMT,                       \
                             (el3c90xCsrReadWord(pDrvCtrl,                   \
                                                 EL_W4_PHY_MGMT,             \
                                                 EL_WIN_4) & ~(x)), EL_WIN_4)
    

/* board level/bus specific and architecture specific macros */
    
#if _BYTE_ORDER==_BIG_ENDIAN
#define PCI_SWAP(x)		LONGSWAP((int)(x))
#define PCI_WORD_SWAP(x)	((LSB((USHORT)(x)) << 8) | MSB((USHORT)(x)))
#else
#define PCI_SWAP(x)		(x)
#define PCI_WORD_SWAP(x)	(x)
#endif
    
#if (CPU_FAMILY==I80X86)

#ifndef EL3C90X_CACHE_VIRT_TO_PHYS
#define EL3C90X_CACHE_VIRT_TO_PHYS(address) \
        CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, (address))
#endif /* EL3C90X_CACHE_VIRT_TO_PHYS */
    
#ifndef SYS_OUT_LONG    
#define SYS_OUT_LONG(pDrvCtrl,addr,value) \
    { \
    if (pDrvCtrl->flags & EL_MODE_MEM_IO_MAP) 	\
        *((ULONG *)(addr)) = (value); 		\
    else 					\
        sysOutLong((int)(addr), (value)); 	\
    }
#endif /* SYS_OUT_LONG */

#ifndef SYS_IN_LONG    
#define SYS_IN_LONG(pDrvCtrl, addr, data) \
    { \
    if (pDrvCtrl->flags & EL_MODE_MEM_IO_MAP) 	\
        ((data) = *((ULONG *)(addr))); 		\
    else 					\
        ((data) = sysInLong((int) (addr))); 	\
    }
#endif /* SYS_IN_LONG */

#ifndef SYS_OUT_SHORT
#define SYS_OUT_SHORT(pDrvCtrl,addr,value) \
    { \
    if (pDrvCtrl->flags & EL_MODE_MEM_IO_MAP) 	\
        *((USHORT *)(addr)) = (value); 		\
    else 					\
        sysOutWord((int)(addr), (value)); 	\
    }
#endif /* SYS_OUT_SHORT */

#ifndef SYS_IN_SHORT
#define SYS_IN_SHORT(pDrvCtrl, addr, data) \
    { \
    if (pDrvCtrl->flags & EL_MODE_MEM_IO_MAP) 	\
        ((data) = *((USHORT *)(addr)));		\
    else 					\
        ((data) = sysInWord((int) (addr))); 	\
    }
#endif /* SYS_IN_SHORT */

#ifndef SYS_OUT_BYTE
#define SYS_OUT_BYTE(pDrvCtrl,addr,value) \
    { \
    if (pDrvCtrl->flags & EL_MODE_MEM_IO_MAP) 	\
        *((UCHAR *)(addr)) = (value); 		\
    else 					\
        sysOutByte((int)(addr), (value)); 	\
    }
#endif /* SYS_OUT_BYTE */

#ifndef SYS_IN_BYTE
#define SYS_IN_BYTE(pDrvCtrl, addr, data) \
    { \
    if (pDrvCtrl->flags & EL_MODE_MEM_IO_MAP) 	\
        ((data) = *((UCHAR *)(addr))); 		\
    else 					\
        ((data) = sysInByte((int) (addr))); 	\
    }
#endif /* SYS_IN_BYTE */
#endif /* CPU_FAMILY == I80x86 */
    
#if defined(__STDC__) || defined(__cplusplus)
IMPORT END_OBJ * el3c90xEndLoad (char * initString);
IMPORT STATUS 	 el3c90xInitParse (EL3C90X_DEVICE * pDrvCtrl,
                                   char * initString);
#else
IMPORT END_OBJ * el3c90xEndLoad ();
IMPORT STATUS	 el3c90xInitParse ();
#endif  /* __STDC__ */
    
#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif  /* __INCel3c90xEndh */
