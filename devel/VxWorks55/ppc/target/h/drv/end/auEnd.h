/* auEnd.h - END based MAC Ethernet header */

/* Copyright 1998-2001 Wind River Systems, Inc. */

/*
 * This file has been developed or significantly modified by the
 * MIPS Center of Excellence Dedicated Engineering Staff.
 * This notice is as per the MIPS Center of Excellence Master Partner
 * Agreement, do not remove this notice without checking first with
 * WR/Platforms MIPS Center of Excellence engineering management.
 */

/*
modification history
--------------------
01h,03jun02,zmm  Fix AU_DEV_NAME_LEN, SPR 78215.
01g,14may02,zmm  Global au1000 name changes. SPR 77333.
01f,06may02,zmm  Fix SPR 76824.
01e,15nov01,zmm  Supported auto-negotiation and cache coherent mode.
                 Add base addresses of the Ethernet registers. SPR 71884. 
01d,11oct01,tlc  Change register names according to new revision of Au1000
                 Databook.  General cleanup.
01c,21sep01,agf  add ring size warning; 
                 change MAC_ENABLE to use enableAdrs field
01b,19jun01,zmm  Put volatile in the MAC macros.
01a,17may01,mem  written.
*/

#ifndef __INCauEndh
#define __INCauEndh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#include "end.h"
#include "cacheLib.h"
#include "netinet/if_ether.h"
#include "miiLib.h"

/* board level/bus specific and architecture specific macros */

#if _BYTE_ORDER==_BIG_ENDIAN
#   define PCI_SWAP(x)	LONGSWAP((int)(x))
#else
#   define PCI_SWAP(x)	(x)
#endif

/* 
 * WARNING -- 
 *   logic in the source code depends on the following being a power of 2 
 */
#define AU_N_TX_BUF			4	/* # of transmit buffers */
#define AU_N_RX_BUF			4	/* # of receive buffers */

/* Device IO or memory mapped access and data registers */

#define AU_MAC_REG(reg)	\
	(*(volatile UINT32 *)((UINT32)(pDrvCtrl->devAdrs)+(reg)))

#define AU_DMA_REG(reg)	\
	(*(volatile UINT32 *)((UINT32)(pDrvCtrl->dmaAdrs)+(reg)))

#define AU_MAC_ENABLE	\
	(*(volatile UINT32 *)((UINT32)(pDrvCtrl->enableAdrs)))

#define AU_MAC_CONTROL			AU_MAC_REG(0x00)
#define AU_MAC_ADDRESS_HIGH			AU_MAC_REG(0x04)
#define AU_MAC_ADDRESS_LOW			AU_MAC_REG(0x08)
#define AU_MULTICAST_HASH_ADDRESS_HIGH	AU_MAC_REG(0x0c)
#define AU_MULTICAST_HASH_ADDRESS_LOW	AU_MAC_REG(0x10)
#define AU_MII_CONTROL			AU_MAC_REG(0x14)
#define AU_MII_DATA				AU_MAC_REG(0x18)
#define AU_FLOW_CONTROL			AU_MAC_REG(0x1c)
#define AU_VLAN1_TAG			AU_MAC_REG(0x20)
#define AU_VLAN2_TAG			AU_MAC_REG(0x24)

/* AU Dedicated MAC DMA registers */

#define AU_TX_STATUS(idx)	AU_DMA_REG(0x00 + 0x10*(idx))
#define AU_TX_ADDRESS(idx)	AU_DMA_REG(0x04 + 0x10*(idx))
#define AU_TX_LENGTH(idx)	AU_DMA_REG(0x08 + 0x10*(idx))

#define AU_RX_STATUS(idx)	AU_DMA_REG(0x0100 + 0x10*(idx))
#define AU_RX_ADDRESS(idx)	AU_DMA_REG(0x0104 + 0x10*(idx))

/* MAC Control bits */

#define AU_MAC_CONTROL_RA		(1 << 31)	/* receive all */
#define AU_MAC_CONTROL_EM_BIG	(1 << 30)	/* big endian */
#define AU_MAC_CONTROL_DO	        (1 << 23)	/* disable receive own */
#define AU_MAC_CONTROL_F	        (1 << 20)	/* full duplex */
#define AU_MAC_CONTROL_PM		(1 << 19)	/* pass multicast */
#define AU_MAC_CONTROL_PR		(1 << 18)	/* promiscuous mode */
#define AU_MAC_CONTROL_IF		(1 << 17)	/* inverse filtering */
#define AU_MAC_CONTROL_PB		(1 << 16)	/* pass bad frames */
#define AU_MAC_CONTROL_HO		(1 << 15)	/* hash-only filter */
#define AU_MAC_CONTROL_HP		(1 << 13)	/* hash/perfect */
#define AU_MAC_CONTROL_LC		(1 << 12)	/* late collision */
#define AU_MAC_CONTROL_DB		(1 << 11)	/* disable broadcast */
#define AU_MAC_CONTROL_DR		(1 << 10)	/* disable retry */
#define AU_MAC_CONTROL_AP		(1 << 8)	/* auto pad strip */
#define AU_MAC_CONTROL_BL(n)	(((n)&3) << 6)	/* backoff limit */
#define AU_MAC_CONTROL_DC		(1 << 5)	/* deferral clock */
#define AU_MAC_CONTROL_TE		(1 << 3)	/* Tx enable */
#define AU_MAC_CONTROL_RE		(1 << 2)	/* Rx enable */
#define AU_MAC_CONTROL_LB_NORMAL	(0)		/* normal mode */

/* MAC Enable bits */

#define AU_MAC_ENABLE_JP		(1 << 6)	/* Jumbo Packet Enable */
#define AU_MAC_ENABLE_E2		(1 << 5)	/* Enable 2 */
#define AU_MAC_ENABLE_E1		(1 << 4)	/* Enable 1 */
#define AU_MAC_ENABLE_C		(1 << 3)	/* Non-Cacheable */
#define AU_MAC_ENABLE_TS		(1 << 2)	/* Toss */
#define AU_MAC_ENABLE_E0		(1 << 1)	/* Enable 0 */
#define AU_MAC_ENABLE_CE		(1 << 0)	/* Clocks Enable */

#define AU_MAC_ENABLE_RESET_ALL	0x00		/* Reset all */
#define AU_MAC_ENABLE_CLOCKS_ONLY	0x41		/* only clocks */
#define AU_MAC_ENABLE_NORMAL	0x37		/* Normal operation */


/* CRC for logical address filter */

#define AU_CRC_POLYNOMIAL	0xedb88320	/* CRC polynomial */
#define AU_CRC_TO_LAF_IX(crc)	((crc) >> 26)	/* get 6 MSBits */
#define AU_LAF_LEN		8    		/* logical addr filter legth */
#define AU_LA_LEN		6		/* logical address length */
#define MAC_ADDRS_SIZE 		6

/* MII Defines */

#define AU_MAX_PHY		1

/* MII Control bits */

#define AU_MII_CONTROL_READ		0		/* Read */
#define AU_MII_CONTROL_WRITE	(1 << 1)	/* Write */
#define AU_MII_CONTROL_BUSY		(1 << 0)	/* Busy */

#define	MII_MGMT_WR_OFF		17
#ifndef MII_WRITE
#   define MII_WRITE               ((ULONG) 0x00002000)
#endif
#ifndef MII_READ
#   define MII_READ                ((ULONG) 0x00044000)
#endif

#define	MII_MGMT_WR		((ULONG) 0x00020000)
#define	MII_MGMT_CLOCK		((ULONG) 0x00010000)
#define	MII_READ_FRM		((ULONG) 0x60000000)
#define MII_WRITE_FRM		((ULONG) 0x50020000)
#define	MII_PHY_CTRL_RES	((USHORT) 0x007F)
#define	MII_PHY_STAT_RES	((USHORT) 0x07C0)
#define	MII_PHY_NWAY_RES	((USHORT) 0x1C00)
#define	MII_PHY_NWAY_EXP_RES	((USHORT) 0xFFE0)
#define	MII_MGMT_DATA_IN	((ULONG) 0x00080000)
#define	MII_READ_DATA_MSK	MII_MGMT_DATA_IN

#define DEC_MAX_PHY              32      /* max number of PHY devices */
#define DEC_MAX_LINK_TOUT        6       /* max link timeout (in secs) */

#define MII_PREAMBLE            ((ULONG) 0xFFFFFFFF)
#define MII_LINK_STATUS		0x4

/* MII frame header format */

#define MII_SOF                 0x4     /* start of frame */
#define MII_RD                  0x2     /* op-code: Read */
#define MII_WR                  0x1     /* op-code: Write  */

/* MII PHY registers */

#define MII_PHY_CR              0x00    /* Control Register */
#define MII_PHY_SR              0x01    /* Status Register */
#define MII_PHY_ID0             0x02    /* Identifier Register 0 */
#define MII_PHY_ID1             0x03    /* Identifier Register 1 */
#define MII_PHY_ANA             0x04    /* Auto Negot'n Advertisement */
#define MII_PHY_ANLPA           0x05    /* Auto Negot'n Link Partner Ability */
#define MII_PHY_ANE             0x06    /* Auto Negot'n Expansion */

/* MII PHY Auto Negotiation Advertisement Register */

#define MII_PHY_ANA_TAF		0x03e0  /* Technology Ability Field */
#define MII_PHY_ANA_T4AM	0x0200  /* T4 Technology Ability Mask */
#define MII_PHY_ANA_TXAM	0x0180  /* TX Technology Ability Mask */
#define MII_PHY_ANA_FDAM	0x0140  /* Full Duplex Technology Ability Mask */
#define MII_PHY_ANA_HDAM	0x02a0  /* Half Duplex Technology Ability Mask */
#define MII_PHY_ANA_100M	0x0380  /* 100Mb Technology Ability Mask */
#define MII_PHY_ANA_10M		0x0060  /* 10Mb Technology Ability Mask */
#define MII_PHY_ANA_CSMA	0x0001  /* CSMA-CD Capable */

/* MII PHY Auto Negotiation Remote End Register */

#define MII_PHY_ANLPA_NP	0x8000  /* Next Page (Enable) */
#define MII_PHY_ANLPA_ACK	0x4000  /* Remote Acknowledge */
#define MII_PHY_ANLPA_RF	0x2000  /* Remote Fault */
#define MII_PHY_ANLPA_TAF	0x03e0  /* Technology Ability Field */
#define MII_PHY_ANLPA_T4AM	0x0200  /* T4 Technology Ability Mask */
#define MII_PHY_ANLPA_TXAM	0x0180  /* TX Technology Ability Mask */
#define MII_PHY_ANLPA_FDAM	0x0140  /* Full Duplex Technology Ability Mask */
#define MII_PHY_ANLPA_HDAM	0x02a0  /* Half Duplex Technology Ability Mask */
#define MII_PHY_ANLPA_100M	0x0380  /* 100Mb Technology Ability Mask */
#define MII_PHY_ANLPA_10M	0x0060  /* 10Mb Technology Ability Mask */
#define MII_PHY_ANLPA_CSMA	0x0001  /* CSMA-CD Capable */

/* Special MII flag bit definitions, not the user flag bits */

#define AU_USR_MII_BUS_MON     0x01000000      /* monitor the MII bus */
#define AU_USR_MII_AN_TBL      0x04000000      /* use auto-negotiation table */
#define AU_USR_MII_NO_AN       0x08000000      /* do not auto-negotiate */
#define AU_USR_MII_FD          0x10000000      /* allow full duplex */
#define AU_USR_MII_100MB       0x20000000      /* allow 100 MB */
#define AU_USR_MII_HD          0x40000000      /* allow half duplex */
#define AU_USR_MII_10MB        0x80000000      /* allow 10 MB */


/* Definitions for the drvCtrl specific flags field */

#define AU_PROMISCUOUS_FLAG     0x1	    /* set the promiscuous mode */
#define AU_MEM_ALLOC_FLAG       0x2	    /* allocating memory flag */
#define AU_PAD_USED_FLAG        0x4	    /* padding used flag */
#define AU_RCV_HANDLING_FLAG    0x8	    /* handling recv packet */
#define AU_START_OUTPUT_FLAG    0x10    /* trigger output flag */
#define AU_POLLING              0x20    /* polling flag */
#define AU_MODE_MEM_IO_MAP	    0x100   /* device registers memory mapped */
#define AU_MODE_DWIO	    0x200   /* device in 32 bit mode */

/* The definition of the driver control structure */

typedef struct auDevice
    {
    END_OBJ	endObj;			/* The class we inherit from */
    int 	unit;			/* unit number of the device */
    UINT32      flags;               	/* Our local flags */
    UINT8	enetAddr[6];		/* ethernet address */

    int         ivec;                   /* interrupt vector */
    int         ilevel;                 /* interrupt level */
    UINT32      devAdrs;                /* device structure address */
    UINT32      dmaAdrs;                /* DMA device address */
    UINT32      enableAdrs;             /* MAC enable register address */

    char *	pRxMem[AU_N_RX_BUF]; /* Base of receive buffers */
    char *	pTxMem[AU_N_TX_BUF]; /* Base of transmit buffers */

    char *      pRxMemBase;             /* Base address of receive buffers */
    char *      pTxMemBase;             /* Base address of transmit buffers */

    int         offset;			/* offset of data in the buffer */

    int         rmdIndex;               /* current RMD index */
    int         rringSize;              /* RMD ring size */

    int         tmdIndex;               /* current TMD index */
    int         tmdLastIndex;           /* last TMD index */
    int         tringSize;              /* TMD ring size */

    PHY_INFO   *pPhyInfo;		/* info on a MII-compliant PHY */
    UINT32		miiPhyFlags;	     /* MII-compliant PHY flags */
    UINT8		phyAddr;	     /* MII-compliant PHY address */
    MII_AN_ORDER_TBL *	pMiiPhyTbl;	     /* MII-compliant PHY's table */

    CACHE_FUNCS cacheFuncs;             /* cache function pointers */
    BOOL        txBlocked;              /* transmit flow control */
    BOOL        txCleaning;		/* transmit descriptor cleaning */
    CL_POOL_ID  pClPoolId;		/* cluster pool Id */
    M_CL_CONFIG	mClCfg;			/* mBlk & cluster config structure */
    CL_DESC	clDesc;			/* cluster descriptor table */
    END_ERR     lastError;              /* Last error passed to muxError */
    BOOL        errorHandling;          /* task level error handling */
    u_short     errorStat;              /* error status */
    } AU_DRV_CTRL;

/* Configuration items */

#define BUS_LATENCY_COUNT	0x1000	/* Max BUS timeo len in 0.1 uSeconds */
#define AU_MIN_FBUF		100	/* Minsize of first buffer in chain */
#define AU_BUFSIZ		(ETHERMTU + SIZEOF_ETHERHEADER + 6)
#define AU_DMA_BUFSIZ	2048	/* Req'd by Au1000 DMA engine */
#define AU_SPEED		10000000
#define AU_RMD_RLEN		5	/* ring size as a ^ 2 -- 32 RMD's */
#define AU_TMD_TLEN		5	/* ring size as a ^ 2 -- 32 TMD's */
#define AU_RMD_MIN		2	/* min descriptors 4 */
#define AU_TMD_MIN		2	/* min descriptors 4 */
#define AU_RMD_MAX		9	/* max descriptors 512 */
#define AU_TMD_MAX		9	/* max descriptors 512 */
#define AU_DEV_NAME		"au"	/* name of the device */
#define AU_DEV_NAME_LEN	3	/* length of the name string */
#define AU_APROM_SIZE	32	/* hardware address prom size */

#define AU_PKT_LEN_GET(rmd)		PCI_SWAP (rmd->rBufMskCnt) - 4;

#if defined(__STDC__) || defined(__cplusplus)
IMPORT END_OBJ * auEndLoad (char * initString);
IMPORT STATUS 	 auInitParse (AU_DRV_CTRL * pDrvCtrl,
				  char * initString);
#else
IMPORT END_OBJ * auEndLoad ();
IMPORT STATUS	 auInitParse ();
#endif  /* __STDC__ */

#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCauEndh */
