/* sh7615End.h - Ethernet driver header */
 
/* Copyright 1984-2000 Wind River Systems, Inc. */
 
/*
modification history
--------------------
01j,28mar02,h_k  added lastError in SH7615END_DRV_CTRL (SPR #74021).
01i,02nov00,frf  changed macros definitions for PHY registers.
01h,26sep00,rsh  remove some macros related to allowing bsp to provide shared
                 memory
01g,25sep00,rsh  add macro defs for reading and writing registers a'la
                 motFecEnd
01f,25sep00,rsh  add some additional macros that make code more readable
01e,22sep00,rsh  continue development
01d,14sep00,rsh  continue development
01c,18aug00,rsh  continue developement
01b,16aug00,rsh  continue development
01a,14aug00,rsh   written 
*/
 
#ifndef __INCsh7615Endh
#define __INCsh7615Endh
 
#ifdef __cplusplus
extern "C" {
#endif
 
/* driver flags */


/* Ethernet Controller register offsets */

#define ETHERC_ECMR		0xFFFFFD60	/* R/W: EtherC mode register */
#define ETHERC_ECSR		0xFFFFFD64	/* R/W: EtherC status register */
#define ETHERC_ECSIPR		0xFFFFFD68	/* R/W: EtherC status int permission */
#define ETHERC_PIR		0xFFFFFD6C	/* R/W: PHY Interface register */
#define ETHERC_MAHR		0xFFFFFD70	/* R/W: MAC Address High register */
#define ETHERC_MALR		0xFFFFFD74	/* R/W: MAC Address Low register */
#define ETHERC_RFLR		0xFFFFFD78	/* R/W: Receive frame length register */
#define ETHERC_PSR		0xFFFFFD7C	/* R:   PHY status register */
#define ETHERC_TROCR		0xFFFFFD80	/* R/W: Tx retry over counter register */
#define ETHERC_CDCR		0xFFFFFD84	/* R/W: Collision detect counter register */
#define ETHERC_LCCR		0xFFFFFD88	/* R/W: Lost Carrier counter register */
#define ETHERC_CNDCR		0xFFFFFD8C	/* R/W: Carrier not detect counter */
#define ETHERC_IFLCR		0xFFFFFD90	/* R/W: Illegal frame length counter */
#define ETHERC_CEFCR		0xFFFFFD94	/* R/W: CRC error frame recv counter */
#define ETHERC_FRECR		0xFFFFFD98	/* R/W: Frame receive error counter */
#define ETHERC_TSFRCR		0xFFFFFD9C	/* R/W: Too-short Frame recv counter */
#define ETHERC_TLFRCR		0xFFFFFDA0	/* R/W: Too-long Frame recv counter */
#define ETHERC_RFCR		0xFFFFFDA4	/* R/W: Residual bit frame counter */
#define ETHERC_MAFCR		0xFFFFFDA8	/* R/W: Multicast addr frame counter */

/* E-DMAC registers */

#define E_DMAC_EDMR		0xFFFFFD00	/* R/W: E-DMAC mode register */
#define E_DMAC_EDTRR		0xFFFFFD04	/* R/W: E-DMAC tran request register */
#define E_DMAC_EDRRR		0xFFFFFD08	/* R/W: E-DMAC recv request register */
#define E_DMAC_TDLAR		0xFFFFFD0C	/* R/W: Tx desc list addr register */
#define E_DMAC_RDLAR		0xFFFFFD10	/* R/W: Rx desc list addr register */
#define E_DMAC_EESR		0xFFFFFD14	/* R/W: EtherC/E-DMAC status register */
#define E_DMAC_EESIPR		0xFFFFFD18	/* R/W: EtherC/E-DMAC status interrupt
                                                                   permission register */
#define E_DMAC_TRSCER		0xFFFFFD1C	/* R/W: Tx/Rx status copy enable */  
#define E_DMAC_RMFCR		0xFFFFFD20	/* R/W: Recevied missed-frame counter */  
#define E_DMAC_TFTR		0xFFFFFD24	/* R/W: Tx FIFO threshold register */  
#define E_DMAC_FDR		0xFFFFFD28	/* R/W: FIFO depth register */  
#define E_DMAC_RCR		0xFFFFFD2C	/* R/W: Receiver control register */  
#define E_DMAC_EDOCR		0xFFFFFD30	/* R/W: E-DMAC operation control */  

/* Ethernet Controller register bit definitions */

/*
 * EtherC mode Register bits
 */

#define ECMR_PRCEF	0x00001000 	/* Permit Receive CRC Error Frame */
#define ECMR_MPDE	0x00000200	/* Magic Packet Detection enable */
#define ECMR_RE		0x00000040	/* Receiver Enable */
#define ECMR_TE		0x00000020	/* Transmitter Enable */
#define ECMR_ILB	0x00000008	/* Internal loop back mode */
#define ECMR_ELB	0x00000004	/* External loop back mode */
#define ECMR_DM		0x00000002	/* Duplex Mode */
#define ECMR_PRM	0x00000001	/* Promiscuous Mode */

/*
 * EtherC Status Register
 */

#define ECSR_LCHNG	0x00000004	/* LINK signal changed */
#define ECSR_MPR	0x00000002	/* Magic Packet received */
#define ECSR_ICD	0x00000001	/* Illegal Carrier detection */
#define ECSR_CLEAR	0x00000007      /* clear all sources */

/* 
 * EtherC Status Interrupt Permission Register
 */

#define ECSIPR_LCHNGIP	0x00000004	/* LINK signal change int enable */
#define ECSIPR_MPRIP	0x00000002	/* Magic Packet received int enable */
#define ECSIPR_ICDIP	0x00000001	/* Illegal Carrier detection int enalbe */
#define ECSIPR_CLEAR    0x00000000	/* Disable all sources */

/* 
 * PHY Interface Register
 */

#define PIR_MDI		0x00000008	/* MII Management Data-In */
#define PIR_MDO		0x00000004	/* MII Management Data-Out */
#define PIR_MMD		0x00000002	/* MII Management Data Mode */
#define PIR_MDC		0x00000001	/* MII Management Data Clock */

/*
 * Receive Frame Length Register
 */

#define RFLR_1518	0x000005ee	/* 1518 bytes */
#define RFLR_1519	0x000005ef	/* 1519 bytes */
#define RFLR_1520	0x000005f0	/* 1520 bytes */
#define RFLR_2047	0x000007ff	/* 2047 bytes */
#define RFLR_2048	0x00000800	/* 2048 bytes */

/*
 * PHY Interface Status Register
 */

#define PSR_LMON	0x00000001	/* Link Monitor */

/*
 * counter register clear macros
 */

#define TXROC_CLEAR	0x0000FFFF	/* clear counter */
#define CDCR_CLEAR	0x0000FFFF	/* clear counter */
#define LCCR_CLEAR	0x0000FFFF	/* clear counter */
#define CNDCR_CLEAR	0x0000FFFF	/* clear counter */
#define IFLCR_CLEAR	0x0000FFFF	/* clear counter */
#define CEFCR_CLEAR	0x0000FFFF	/* clear counter */
#define FRECR_CLEAR	0x0000FFFF	/* clear counter */
#define TSFRCR_CLEAR	0x0000FFFF	/* clear counter */
#define TLFRCR_CLEAR	0x0000FFFF	/* clear counter */
#define RFCR_CLEAR	0x0000FFFF	/* clear counter */
#define MAFCR_CLEAR	0x0000FFFF	/* clear counter */

/* E-DMAC register bit definitions */

/*
 * E-DMAC Mode Register 
 */

#define EDMR_DL1	0x00000020	/* Descriptor length */
#define EDMR_DL0	0x00000010	/* Descriptor length */
#define EDMR_SWR	0x00000001	/* Software reset */

/* 
 * E-DMAC Transmit request register 
 */

#define EDTRR_TR	0x00000001	/* Transmit request */

/* 
 * E-DMAC Receive request register 
 */

#define EDRRR_RR	0x00000001	/* Receive request */

/* 
 * EtherC/E-DMAC status register
 */

#define EESR_RFCOF	0x01000000	/* Receive frame counter overflow */ 
#define EESR_ECI	0x00400000	/* EtherC status regsiter interrupt */ 
#define EESR_TC		0x00200000	/* Tx Complete */ 
#define EESR_TDE	0x00100000 	/* Tx Descriptor Exhausted */
#define EESR_TFUF	0x00080000	/* Tx FIFO Underflow */ 
#define EESR_FR		0x00040000	/* Frame Received */ 
#define EESR_RDE	0x00020000	/* Rx Descriptor Exhausted */ 
#define EESR_RFOF	0x00010000	/* Rx FIFO Overflow */ 
#define EESR_ITF	0x00001000	/* Illegal Tx Frame */ 
#define EESR_CND	0x00000800	/* Carrier not detected */ 
#define EESR_DLC	0x00000400	/* Detect loss of carrier */ 
#define EESR_CD		0x00000200	/* Collision Detect */ 
#define EESR_TRO	0x00000100	/* Tx Retry Over */ 
#define EESR_RMAF	0x00000080	/* Receive Multicast Address frame */ 
#define EESR_RRF	0x00000010	/* Receive residual-bit frame */ 
#define EESR_RTLF	0x00000008	/* Receive Too-long frame */ 
#define EESR_RTSF	0x00000004	/* Receive Too-short frame */ 
#define EESR_PRE	0x00000002	/* PHY-LSI Receive Error */ 
#define EESR_CERF	0x00000001	/* CRC Error on Received frame */ 

#define EESR_VALID_INT_MSK 0x017f1f9f   /* all possible interrupt sources */
#define EESR_CLEAR         0x017f1f9f   /* all possible interrupt sources */

/* 
 * EtherC/E-DMAC status Interrupt Permission register
 */

#define EESIPR_RFCOFIP	0x01000000	/* Receive frame counter overflow */ 
#define EESIPR_ECIIP	0x00400000	/* EtherC status regsiter interrupt */ 
#define EESIPR_TCIP	0x00200000	/* Tx Complete */ 
#define EESIPR_TDEIP	0x00100000 	/* Tx Descriptor Exhausted */
#define EESIPR_TFUFIP	0x00080000	/* Tx FIFO Underflow */ 
#define EESIPR_FRIP	0x00040000	/* Frame Received */ 
#define EESIPR_RDEIP	0x00020000	/* Rx Descriptor Exhausted */ 
#define EESIPR_RFOFIP	0x00010000	/* Rx FIFO Overflow */ 
#define EESIPR_ITFIP	0x00001000	/* Illegal Tx Frame */ 
#define EESIPR_CNDIP	0x00000800	/* Carrier not detected */ 
#define EESIPR_DLCIP	0x00000400	/* Detect loss of carrier */ 
#define EESIPR_CDIP	0x00000200	/* Collision Detect */ 
#define EESIPR_TROIP	0x00000100	/* Tx Retry Over */ 
#define EESIPR_RMAFIP	0x00000080	/* Receive Multicast Address frame */ 
#define EESIPR_RRFIP	0x00000010	/* Receive residual-bit frame */ 
#define EESIPR_RTLFIP	0x00000008	/* Receive Too-long frame */ 
#define EESIPR_RTSFIP	0x00000004	/* Receive Too-short frame */ 
#define EESIPR_PREIP	0x00000002	/* PHY-LSI Receive Error */ 
#define EESIPR_CERFIP	0x00000001	/* CRC Error on Received frame */ 

/*
 * Tx/Rx status copy enable register 
 */

#define TRSCER_ITFCE	0x00001000	/* Illegal Tx Frame */
#define TRSCER_CNDCE	0x00000800	/* Carrier not detected */
#define TRSCER_DLCCE	0x00000400	/* Detect loss of carrier */
#define TRSCER_CDCE	0x00000200	/* Collision Detect */
#define TRSCER_TROCE	0x00000100	/* Tx Retry Over */
#define TRSCER_RMAFCE	0x00000080	/* Receive Multicast Address frame */
#define TRSCER_RRFCE	0x00000010	/* Receive residual-bit frame */
#define TRSCER_RTLFCE	0x00000008	/* Receive Too-long frame */
#define TRSCER_RTSFCE	0x00000004	/* Receive Too-short frame */
#define TRSCER_PRECE	0x00000002	/* PHY-LSI Receive Error */
#define TRSCER_CERFCE	0x00000001	/* CRC Error on Received frame */

/*
 * FIFO Depth Register
 */

/*  */

/*
 * Receiver Control Register
 */

#define RCR_RNC		0x00000001	/* Receive Enable Control */

/*
 * E-DMAC Operation Control Register
 */

#define EDOCR_FEC	0x00000004	/* FIFO Error control */
#define EDOCR_AEC	0x00000002	/* Address Error Control*/
#define EDOCR_EDH	0x00000001	/* E-DMAC Halted */

/* Transmit descriptor bit definitions */

#define TD0_TACT	0x80000000	/* Tx Descriptor Active */
#define TD0_OWN		TD0_TACT        /* the chip owns the descriptor */
#define TD0_TDL		0x40000000	/* Tx Descriptor Last */
#define TD0_TFP1	0x20000000	/* Tx Frame Position */
#define TD0_TFP0	0x10000000	/* Tx FRame Position */
#define TD0_TFP   	0x30000000      /* buffer contains entire frame */
#define TD0_TFE		0x08000000	/* Tx Frame Error */
#define TD0_TFS4	0x00000010	/* Tx Illegal Frame */
#define TD0_IF		TD0_TFS4
#define TD0_TFS3	0x00000008	/* Tx Carrier Not Detect */
#define TD0_CND		TD0_TFS3
#define TD0_TFS2	0x00000004	/* Tx Detect Loss Carrier */
#define TD0_DLC		TD0_TFS2
#define TD0_TFS1	0x00000002	/* Tx Collision Detect */
#define TD0_CD		TD0_TFS1
#define TD0_TFS0	0x00000001	/* Tx Retry Over */
#define TD0_TRO		TD0_TFS0
#define TD0_CLEAR_ERRORS_N_STATS 0xf0000000

#define SH7615END_TBS_PUT(x)      ((x) << 16) 

/* Receive descriptor bit definitions */

#define RD0_RACT	0x80000000	/* Rx Descriptor Active */
#define RD0_OWN		RD0_RACT        /* the chip owns the descriptor */
#define RD0_RDL		0x40000000	/* Rx Descriptor Last */
#define RD0_RFP1	0x20000000	/* Rx Frame Position */
#define RD0_RFP0	0x10000000	/* Rx FRame Position */
#define RD0_RFE		0x08000000	/* Rx Frame Error */
#define RD0_RFS9	0x00000200	/* Rx FIFO Overflow */
#define RD0_RFS7	0x00000080	/* Rx Multicast Addr Frame */
#define RD0_RFS4	0x00000010	/* Rx Residual-Bit FRame */
#define RD0_RFS3	0x00000008	/* Rx Too-Long Frame */
#define RD0_RFS2	0x00000004	/* Rx Too-Short Frame */
#define RD0_RFS1	0x00000002	/* Rx PHY-LSI Recv Error */
#define RD0_RFS0	0x00000001	/* Rx CRC Error on Received */

#define SH7615END_FRAME_LEN_MSK       0x0000FFFF      /* Frame length mask */
#define SH7615END_FRAME_LEN_GET(x)    ((x) & SH7615END_FRAME_LEN_MSK)

/* DP83843BVJE PHY Chip */
#define PHY_ADDR		0x01

/* DP83843BVJE Phyter MII offset registers */

#define BMCR            0x00	/* Basic Mode Control Register */
#define BMSR		0x01	/* Basic Mode status Register */
#define PHYIDR1		0x02	/* PHY Identifier register #1 */
#define PHYIDR2		0x03	/* PHY Identifier register #2 */
#define ANAR            0x04	/* Auto-Negotiation Advertisement
				   Register */
#define ANLPAR          0x05	/* Auto-Negotiation Link Partner
				   Ability Register */
#define ANER            0x06	/* Auto-Negotiation Expansion
				   Register */
#define ANNPTR          0x07	/* Auto-Negotiation Next Page TX */
/*                      0x08-0x0f  Reserved */
#define PHYSTS		0x10	/* PHY Status Register */
#define MIPSCR          0x11	/* MII Interrupt PHY Specific Control
				   Register */
#define MIPGSR		0x12	/* MII Interrupt PHY Generic Status
				   Register */
#define DCR             0x13	/* Disconnect Count Register */
#define FCSCR           0x14 	/* False Carrier Sense Counter
				   Register */
#define RECR            0x15	/* Receive Error Counter Register */
#define PCSR            0x16	/* PCS Sub-Layer Configuration and
				   Status Register */
#define LBR             0x17 	/* Loopback and Bypass Register */
#define BTSCR           0x18	/* 10Base-T Status & Control Register */
#define PHYCTRL         0x19	/* PHY Control Register */
/*			0x1A-0x1F  Reserved */

/* DP83843BVJE Phyter MII register data */

#define	D_RESET		0x8000
#define D_BMCR          0x0000  
#define D_ANAR          0x01e1  
#define D_ANLPAR        0x0021  
#define D_ANER          0x0004 
#define D_ANNPTR        0x2001
#define D_MIPSCR        0x0000
#define D_DCR           0x0000
#define D_FCSCR         0x0000
#define D_RECR          0x0000
#define D_PCSR          0x0000
#define D_LBR           0x0000
#define D_10BTSCR       0x0000
				
#define D_PHYCTRL       0x0801

/*
 * Receive Message Descriptor Entry.
 * Four words per entry.  Number of entries must be a power of two.
 */
typedef struct rDesc
    {
    UINT32       rDesc0;         /* status */
    UINT32       rDesc1;         /* buffer & data count */
    UINT32       rDesc2;         /* buffer address 1 */
    UINT32       rDesc3;         /* Pad */
    } SH7615_RD;
 
/*
 * Transmit Message Descriptor Entry.
 * Four words per entry.  Number of entries must be a power of two.
 */
typedef struct tDesc
    {
    UINT32       tDesc0;         /* status */
    UINT32       tDesc1;         /* data count */
    UINT32       tDesc2;         /* buffer address 1 */
    UINT32       tDesc3;         /* Pad */
    } SH7615_TD;

typedef struct free_buf
    {
    void *      pClBuf;                  /* pointer cluster buffer */
    } FREE_BUF;
 
/* typedefs */
 
typedef struct sh7615EndDrvCtrl
    {
    END_OBJ     end;                    /* The class we inherit from. */
    END_ERR	lastError;		/* Last error passed to muxError */
    long        flags;                  /* Our local flags. */
    ULONG       userFlags;              /* some user flags */
    int         unit;                   /* unit number */
 
    int         numRds;                 /* RD ring size */
    int         rxIndex;               /* current RMD index */
    SH7615_RD*  rxRing;                 /* RMD ring start */
 
    int         numTds;                 /* TD ring size */
    int         txIndex;               /* current TMD index */
    int         txDiIndex;              /* current TMD index */
    SH7615_TD*  txRing;                 /* TMD ring start */
 
    int         ivec;                   /* interrupt vector */
    int         ilevel;                 /* interrupt level */
    UINT32      intrMask;               /* interrupt mask */
    char*       pShMem;                 /* real ptr to shared memory */
    char*       memBase;                /* LANCE memory pool base */
    int         memSize;                /* LANCE memory pool size */
 
    BOOL        loaded;                 /* interface has been loaded */
    char *      pClBlkArea;             /* cluster block pointer */
    UINT32      clBlkSize;              /* clusters block memory size */
    char *      pMBlkArea;              /* mBlock area pointer */
    UINT32      mBlkSize;               /* mBlocks area memory size */
    UCHAR       enetAddr[6];            /* ethernet address */
    CL_POOL_ID  pClPoolId;              /* cluster pool */
    BOOL        rxHandling;             /* rcv task is scheduled */
    BOOL        txCleaning;
    BOOL        txBlocked;              /* variable for blocking */
/* REVISIT - shouldn't this be the same array size as number of possible buffers? */
    FREE_BUF    freeBuf[128];
/*    PHY_INFO    *phyInfo;  */             /* info on a MII-compliant PHY */
    } SH7615END_DRV_CTRL;
 
/* rx/tx buffer descriptors definitions */

#define SH7615END_RBD_SZ          16      /* RBD size in byte */
#define SH7615END_TBD_SZ          16      /* TBD size in byte */
#define SH7615END_TBD_MIN         6       /* min number of TBDs */
#define SH7615END_RBD_MIN         4       /* min number of RBDs */
#define SH7615END_TBD_POLL_NUM    1       /* one TBD for poll operation */
#define CL_OVERHEAD               4       /* prepended cluster overhead */
#define CL_ALIGNMENT              4       /* cluster required alignment */
#define MBLK_ALIGNMENT            4       /* mBlks required alignment */
#define SH7615END_BD_ALIGN        0x10    /* required alignment for RBDs */
#define SH7615END_MAX_PCK_SZ      (ETHERMTU + SIZEOF_ETHERHEADER          \
                                 + ETHER_CRC_LEN)

#define SH7615END_RBD_DEF_NUM  48      /* default number of Recv descriptors */
#define SH7615END_TBD_DEF_NUM  64      /* default number of Xmit descriptors */
#define NUM_LOAN        16      /* number of loaner buffers */

#ifdef __cplusplus
}
#endif
 
#endif /* __INCsh7615Endh */

