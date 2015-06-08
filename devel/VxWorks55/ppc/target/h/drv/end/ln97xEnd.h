/* ln97xEnd.h - END style AMD Am79C97X PCnet-PCI Ethernet driver header */

/* Copyright 1998-2002 Wind River Systems, Inc. */
/* Copyright 1998 CETIA Inc. */

/*
modification history
--------------------
01f,30may02,sru  Added REG_SWAP and LS_MODE_NOSWAP_MASTER definitions.
01e,12nov01,pai  Added definitions for CSR5 and CSR8-CSR11 fields.  Setup
                 PCI_SWAP definition in one place.  Doubled the number of
                 receive and transmit descriptors the device will use.
                 Removed obsolete <errorStat> member from DRV_CTRL.
                 Removed superfluous "arch-specific" (CPU_FAMILY==I80X86)
                 macros.
01d,27mar01,tlc  Add definitions for interrupt level and PCI bus endianess.
                 Revised how PCI_SWAP is defined. It is now conditionally
                 defined based on the ln97xEndianess parameter set in
                 ln97xParamInit().
01c,11jun00,ham  removed reference to etherLib.
01b,19may00,pai  Implemented accumulated driver bug fixes (SPR #31349).
01a,07dec98,snk	added support 7997x[012] across architectures.
		written from 01b of lnPciEnd.h
*/

#ifndef __INCln97xEndh
#define __INCln97xEndh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#include "end.h"
#include "cacheLib.h"
#include "netinet/if_ether.h"


/* define the various levels of debugging if the DRV_DEBUG is defined */    

#ifdef DRV_DEBUG
#    include "logLib.h"
#    define DRV_DEBUG_OFF           0x0000
#    define DRV_DEBUG_RX            0x0001
#    define DRV_DEBUG_TX            0x0002
#    define DRV_DEBUG_INT           0x0004
#    define DRV_DEBUG_POLL          (DRV_DEBUG_POLL_RX | DRV_DEBUG_POLL_TX)
#    define DRV_DEBUG_POLL_RX       0x0008
#    define DRV_DEBUG_POLL_TX       0x0010
#    define DRV_DEBUG_LOAD          0x0020
#    define DRV_DEBUG_LOAD2         0x0040
#    define DRV_DEBUG_IOCTL         0x0080
#    define DRV_DEBUG_RESET         0x0100
#    define DRV_DEBUG_MCAST         0x0200
#    define DRV_DEBUG_CSR           0x0400
#    define DRV_DEBUG_RX_PKT        0x0800
#    define DRV_DEBUG_POLL_REDIR    0x10000
#    define DRV_DEBUG_LOG_NVRAM     0x20000
#    define DRV_DEBUG_ALL           0xfffff
#endif /* DRV_DEBUG */

/* board level/bus specific and architecture specific macros */

#if _BYTE_ORDER==_BIG_ENDIAN
#   define PCI_SWAP(x)  ((ln97xBusEndianess) ? (int)(x) : LONGSWAP((int)(x))) 
#else
#   define PCI_SWAP(x)  (x)
#endif

#if _BYTE_ORDER==_BIG_ENDIAN
#    define REG_SWAP(d,x)  (((d)->flags & LS_MODE_NOSWAP_MASTER) ? \
                                (int)(x) : LONGSWAP((int)(x)))
#else
#    define REG_SWAP(d,x)  (x)
#endif

/* device IO or memory mapped access and data registers */

#define LN_97X_RDP      ((UINT32 *)((UINT32)(pDrvCtrl->devAdrs) + 0x10))
#define LN_97X_RAP      ((UINT32 *)((UINT32)(pDrvCtrl->devAdrs) + 0x14))
#define LN_97X_RST      ((UINT32 *)((UINT32)(pDrvCtrl->devAdrs) + 0x18))
#define LN_97X_BDP      ((UINT32 *)((UINT32)(pDrvCtrl->devAdrs) + 0x1C))

/* csr and bcr addresses */

#define CSR(x)          (x)
#define BCR(x)          (x)

/* Definitions for fields and bits in the device */

#define CSR0_RESRVD         0xffff0000	/* resvd: written 0, read undefined */
#define CSR0_ERR            0x8000	/* (RO) err flg (BABL|CERR|MISS|MERR) */
#define CSR0_BABL           0x4000	/* (RC) babble transmitter timeout */
#define CSR0_CERR           0x2000	/* (RC) collision error */
#define CSR0_MISS           0x1000	/* (RC) missed packet */
#define CSR0_MERR           0x0800	/* (RC) memory error */
#define CSR0_RINT           0x0400	/* (RC) receiver interrupt */
#define CSR0_TINT           0x0200	/* (RC) transmitter interrupt */
#define CSR0_IDON           0x0100	/* (RC) initialization done */
#define CSR0_INTR           0x0080	/* (RO) interrupt flag */
#define CSR0_INEA           0x0040	/* (RW) interrupt enable */
#define CSR0_RXON           0x0020	/* (RO) receiver on */
#define CSR0_TXON           0x0010	/* (RO) transmitter on */
#define CSR0_TDMD           0x0008	/* (WOO)transmit demand */
#define CSR0_STOP           0x0004	/* (WOO)stop (& reset) chip */
#define CSR0_STRT           0x0002	/* (RW) start chip */
#define CSR0_INIT           0x0001	/* (RW) initialize (acces init block) */

#define CSR0_INTMASK            (CSR0_BABL | CSR0_CERR | CSR0_MISS | \
                                 CSR0_MERR | CSR0_RINT | CSR0_TINT | \
                                 CSR0_IDON | CSR0_INEA)

#define CSR3_DXSUFLO		0x0040	/* disable transmit stop on UFLO */
#define	CSR3_BSWP		0x0004	/* Byte Swap */

/* CSR 4 test and features control */

#define CSR4_EN124		0x8000	/* enable CSR124 access */
#define CSR4_DMAPLUS		0x4000	/* disable DMA burst xfer counter */
#define CSR4_TIMER		0x2000	/* enable bus activity timer */
#define CSR4_DPOLL		0x1000	/* disable transmit polling */
#define CSR4_APAD_XMIT		0x0800	/* enable auto pad transmit */
#define CSR4_ASTRP_RCV		0x0400	/* enable auto pad stripping */
#define CSR4_MFCO		0x0200	/* missed frame counter overflow */
#define CSR4_MFCOM		0x0100	/* missed frame counter mask */
#define CSR4_UINTCMD		0x0080	/* user interrupt command */
#define CSR4_UINT		0x0040	/* user interrupt issued */
#define CSR4_RCVCC0		0x0020	/* receive collision cntr overflow */
#define CSR4_RCVCCOM		0x0010	/* receive collision cntr mask */
#define CSR4_TXSTRT		0x0008	/* transmit start status */
#define CSR4_TXSTRTM		0x0004	/* transmit start mask */
#define CSR4_JAB		0x0002	/* jabber error indicated */
#define CSR4_JABM		0x0001	/* jabber error mask */

#define CSR4_TFC_MASK		(CSR4_MFCOM | CSR4_RCVCCOM | \
                                 CSR4_TXSTRTM | CSR4_JABM)

/* CSR 5 extended control and interrupt */

#define CSR5_RESRVD_B       0xffff0000	/* resvd: written 0, read undefined */
#define CSR5_TOKINTD        0x00008000	/* transmit OK interrupt disable */
#define CSR5_LTINTEN        0x00004000	/* last transmit interrupt enable */
#define CSR5_RESRVD_A       0x00003000	/* resvd: written 0, read undefined */
#define CSR5_SINT           0x00000800	/* system interrupt asserted */
#define CSR5_SINTE          0x00000400	/* system interrupt enable */
#define CSR5_SLPINT         0x00000200	/* sleep interrupt asserted */
#define CSR5_SLPINTE        0x00000100	/* sleep interrupt enable */
#define CSR5_EXDINT         0x00000080	/* excessive deferral asserted */
#define CSR5_EXDINTE        0x00000040	/* excessive deferral int. enable */
#define CSR5_MPPLBA         0x00000020	/* magic packet broadcast accept */
#define CSR5_MPINT          0x00000010	/* magic packet int. asserted */
#define CSR5_MPINTE         0x00000008	/* magic packet int. enable */
#define CSR5_MPEN           0x00000004	/* magic packet mode enable */
#define CSR5_MPMODE         0x00000002	/* set magic packet mode */
#define CSR5_SPND           0x00000001	/* set suspend mode */

#define CSR5_RESRVD_MASK    (~(CSR5_RESRVD_B | CSR5_RESRVD_A))

/* CSR  8 logical address filter 0 */

#define CSR8_RESRVD         0xffff0000	/* reserved locations, write as 0 */
#define CSR8_LADRF0         0x0000ffff	/* logical address filter [15:0] */

/* CSR  9 logical address filter 1 */

#define CSR9_RESRVD         0xffff0000	/* reserved locations, write as 0 */
#define CSR9_LADRF1         0x0000ffff	/* logical address filter [31:16] */

/* CSR 10 logical address filter 2 */

#define CSR10_RESRVD        0xffff0000	/* reserved locations, write as 0 */
#define CSR10_LADRF2        0x0000ffff	/* logical address filter [47:32] */

/* CSR 11 logical address filter 3 */

#define CSR11_RESRVD        0xffff0000	/* reserved locations, write as 0 */
#define CSR11_LADRF3        0x0000ffff	/* logical address filter [63:48] */

/* CSR 15 mode register - loaded from initialization block */

#define CSR15_PROM          0x00008000	/* set promiscuous mode */
#define CSR15_DTX           0x00000002	/* disable transmitter section */
#define CSR15_DRX           0x00000001	/* disable receiver section */

/* CSR 88 and CSR 89 chip ID registers */

#define CSR88_LN970_PARTID  0x00001003	/* Am79c970 part ID lower value */
#define CSR88_LN971_PARTID  0x00003003	/* Am79c971 part ID lower value */
#define CSR88_LN972_PARTID  0x00004003	/* Am79c972 part ID lower value */
#define CSR88_LN973_PARTID  0x00005003	/* Am79c973 part ID lower value */
#define CSR88_LN975_PARTID  0x00007003	/* Am79c975 part ID lower value */

#define CSR89_LN97X_PARTID  0x00000262	/* Am79c97x part ID upper value */

/* chip ID values are formed as follows:
 *
 *     chipId = (CSR(88) | (CSR(89) << 16))
 *     chipId = (chipId >> 12) & 0xffff
 */

#define CHIP_ID_AM79C970A   (0x2621)    /* PCnet-PCI II   Am79C970A */
#define CHIP_ID_AM79C971    (0x2623)    /* PCnet-FAST     Am79C971 */
#define CHIP_ID_AM79C972    (0x2624)    /* PCnet-FAST +   Am79C972 */
#define CHIP_ID_AM79C973    (0x2625)    /* PCnet-FAST III Am79C973 */
#define CHIP_ID_AM79C975    (0x2627)    /* PCnet Fast III Am79C975 */

/* BCR 2 */

#define BCR2_LEDPE              0x1000  /* LED program enable */
#define BCR2_AUTO_SELECT        0x0002  /* auto select port type 10BT/AUI */
#define BCR2_ACTIVE_HIGH        0x80    /* select active high interrupt pin */
#define BCR2_ACTIVE_LOW         0x00    /* select active low interrupt pin  */

/* BCR 18 burst size and bus control register */

#define BCR18_ROMTMG		0x9000	/* expansion ROM timing (default) */
#define BCR18_MEMCMD		0x0200	/* enable PCI memory read multiple */
#define BCR18_BREADE		0x0040	/* burst read enable (SWSTYLE=3) */
#define BCR18_BWRITE		0x0020	/* burst write enable (SWSTYLE=3) */
#define BCR18_RESRVD		0x0001	/* reserved location (default) */

#define BCR18_BSBC_MASK		(BCR18_ROMTMG | BCR18_RESRVD | \
                                 BCR18_BREADE | BCR18_BWRITE)

/* BCR 20 software style register */
    
#define BCR20_SSIZE32		0x0100
#define BCR20_SWSTYLE_LANCE	0x0000
#define BCR20_SWSTYLE_ILACC	0x0001
#define BCR20_SWSTYLE_PCNET	0x0002
#define BCR20_SWSTYLE_PCNET_II	0x0003
    
/* Control block definitions for AMD PCnet (Ethernet) chip. */

typedef struct
    {
    ULONG       rdp;            /* register data Port */
    ULONG       rap;            /* Register Address Port */
    ULONG       rst;            /* Reset Register */
    ULONG       bdp;            /* Bus Configuration Register Data Port */
    } LN_DEVICE;

/* Initialization Block */

typedef struct lnIB
    {
    ULONG       lnIBMode;       /* mode register */
    UCHAR       lnIBPadr [8];   /* PADR: bswapd ethnt phys address */
    UCHAR       lnIBLadrf [8];  /* logical address filter */
    ULONG       lnIBRdra;       /* RDRA: read ring address, long word */
    ULONG       lnIBTdra;       /* TDRA: transmit ring address long word */
    } LN_IB;

/* Receive Message Descriptor Entry.*/

typedef struct lnRMD
    {
    /* element order changed to accomodate burst mode operation */
    ULONG       lnRMD2;         /* message byte count */
    ULONG       lnRMD1;         /* status & buffer byte count (negative) */
    ULONG       lnRMD0;         /* bits 31:00 of receive buffer address */
    ULONG       lnRMD3;         /* reserved */
    } LN_RMD;

/* Transmit Message Descriptor Entry. */

typedef struct lnTMD
    {
    /* element order changed to accomodate burst mode operation */
    ULONG       lnTMD2;         /* errors */
    ULONG       lnTMD1;         /* message byte count */
    ULONG       lnTMD0;         /* bits 31:00 of transmit buffer address */
    ULONG       lnTMD3;         /* reserved */
    } LN_TMD;

typedef union addrFilter        /* logical address filter layout */
    {
    UINT8  lafBytes  [8];       /* (8) bytes */
    UINT16 lafWords  [4];       /* (4) 16-bit words */
    UINT32 lafDwords [2];       /* (2) 32-bit dwords */
    } ADDR_FILTER;

typedef enum filterModCmd       /* addrFilterMod() commands */
    {
    LN_LADRF_ADD,               /* add a logical address */
    LN_LADRF_DEL,               /* delete a logical address */
    LN_LADRF_ADD_ALL,           /* add all logical addresses */
    LN_LADRF_DEL_ALL            /* delete all logical addresses */
    } FILTER_MOD_CMD;


/* initialization block */

#define IB_MODE_TLEN_MSK	0xf0000000
#define IB_MODE_RLEN_MSK	0x00f00000

/* receive descriptor */

#define RMD1_OWN		0x80000000	/* Own */
#define RMD1_ERR		0x40000000	/* Error */
#define RMD1_FRAM		0x20000000	/* Framming Error */
#define RMD1_OFLO		0x10000000	/* Overflow */
#define RMD1_CRC		0x08000000	/* CRC */
#define RMD1_BUFF		0x04000000	/* Buffer Error */
#define RMD1_STP		0x02000000	/* Start of Packet */
#define RMD1_ENP		0x01000000	/* End of Packet */
#define RMD1_RES		0x00ff0000	/* reserved */
#define RMD1_CNST		0x0000f000	/* rmd1 constant value */

#define RMD1_BCNT_MSK		0x00000fff	/* buffer cnt mask */
#define RMD2_MCNT_MSK		0x00000fff	/* message buffer cnt mask */

/* transmit descriptor */

#define TMD1_OWN		0x80000000	/* Own */
#define TMD1_ERR		0x40000000	/* Error */
#define TMD1_MORE		0x10000000	/* More than One Retry */
#define TMD1_ONE		0x08000000	/* One Retry */
#define TMD1_DEF		0x04000000	/* Deferred */
#define TMD1_STP		0x02000000	/* Start of Packet */
#define TMD1_ENP		0x01000000	/* End of Packet */
#define TMD1_BPE                0x00800000      /* Bus Parity Error */
#define TMD1_RES		0x007F0000	/* Reserved locations */
#define TMD1_CNST		0x0000f000	/* tmd1 constant value */

#define TMD2_BUFF		0x80000000	/* Buffer Error */
#define TMD2_UFLO		0x40000000	/* Underflow Error */
#define TMD2_LCOL		0x10000000	/* Late Collision */
#define TMD2_LCAR		0x08000000	/* Lost Carrier */
#define TMD2_RTRY		0x04000000	/* Retry Error */
#define TMD2_TDR		0x03FF0000	/* Time Domain Reflectometry */

#define TMD1_BCNT_MSK		0x00000fff	/* buffer cnt mask */

/* CRC for logical address filter */

#define LN_CRC_POLYNOMIAL       (0xedb88320)    /* Ethernet CRC polynomial */
#define LN_CRC_TO_LAF_IX(crc)   ((crc) >> 26)   /* get 6 MSBits */
#define LN_LAF_LEN              (8)             /* logical addr filter length */
#define LN_LA_LEN               (6)             /* logical address length */
#define BITS_PER_LA             (8 * LN_LA_LEN) /* logical addr length bits */

/* Definitions for the drvCtrl specific flags field */

#define LS_PROMISCUOUS_FLAG     0x1     /* set the promiscuous mode */
#define LS_MEM_ALLOC_FLAG       0x2     /* allocating memory flag */
#define LS_PAD_USED_FLAG        0x4     /* padding used flag */
#define LS_RCV_HANDLING_FLAG    0x8     /* handling recv packet */
#define LS_START_OUTPUT_FLAG    0x10    /* trigger output flag */
#define LS_POLLING              0x20    /* polling flag */
#define LS_MODE_MEM_IO_MAP      0x100   /* device registers memory mapped */
#define LS_MODE_DWIO            0x200   /* device in 32 bit mode */
#define LS_MODE_NOSWAP_MASTER	0x400   /* don't swap PCI master I/O ops */

/* descriptor size */

#define RMD_SIZ                 sizeof(LN_RMD)
#define TMD_SIZ                 sizeof(LN_TMD)
#define IB_SIZ                  sizeof(LN_IB)

/* The definition of the driver control structure */

typedef struct ln97xDevice
    {
    END_OBJ           endObj;            /* The class we inherit from */

    int               unit;              /* unit number of the device */
    volatile LN_IB *  ib;                /* ptr to Initialization Block */

    int               rmdIndex;          /* current RMD send index */
    int               rringSize;         /* RMD ring size */
    int               rringLen;          /* RMD ring length (bytes) */
    volatile LN_RMD * pRring;            /* RMD ring start */

    int               tmdIndex;          /* current TMD transmit index */
    int               tmdIndexC;         /* current TMD cleaning index */
    int               tringSize;         /* TMD ring size */
    int               tringLen;          /* TMD ring length (bytes) */
    volatile LN_TMD * pTring;            /* TMD ring start */

    int               inum;              /* interrupt number */
    int               ilevel;            /* interrupt level */

    volatile UINT32 * pRdp;              /* device register CSR */
    volatile UINT32 * pRap;              /* device register RAP */
    volatile UINT32 * pReset;            /* device register Reset */
    volatile UINT32 * pBdp;              /* device register BCR */

    volatile char *   pShMem;            /* real ptr to shared memory */
    volatile char *   memBase;           /* PCnet memory pool base */
    volatile char *   memAdrs;           /* PCnet memory pool base */

    UINT32            devAdrs;           /* device structure address */
    UINT16            csr3B;             /* csr3 value board specific */
    UINT32            chipId;            /* (csr88|csr89) chip ID value */
    int               memSize;           /* PCnet memory pool size */
    int               memWidth;          /* width of data port */
    int               offset;            /* offset of data in the buffer */
    volatile UINT32   flags;             /* Our local flags */
    UINT32            pciMemBase;        /* memory base as seen from PCI*/
    UINT8             enetAddr[6];       /* ethernet address */
    CACHE_FUNCS       cacheFuncs;        /* cache function pointers */

    volatile BOOL     txBlocked;         /* transmit flow control */
    volatile BOOL     txCleaning;        /* transmit descriptor cleaning */

    CL_POOL_ID        pClPoolId;         /* cluster pool Id */
    M_CL_CONFIG       mClCfg;            /* mBlk & cluster config structure */
    CL_DESC           clDesc;            /* cluster descriptor table */

    END_ERR           lastError;         /* Last error passed to muxError */
    volatile BOOL     errorHandling;     /* task level error handling */

    } LN_97X_DRV_CTRL;

/* Configuration items */

#define LN_MIN_FBUF             (100)     /* Min sizeof first buffer in chain */
#define LN_BUFSIZ               (ETHERMTU + SIZEOF_ETHERHEADER + 6)
#define LN_SPEED                (10000000)
#define LN_RMD_RLEN             (7)       /* ring size as a ^ 2 => 128 RMD's */
#define LN_TMD_TLEN             (7)       /* ring size as a ^ 2 => 128 TMD's */
#define LN_RMD_MIN              (2)       /* min descriptors 4 */
#define LN_TMD_MIN              (2)       /* min descriptors 4 */
#define LN_RMD_MAX              (9)       /* max descriptors 512 */
#define LN_TMD_MAX              (9)       /* max descriptors 512 */
#define LN_97X_DEV_NAME         "lnPci"   /* name of the device */
#define LN_97X_DEV_NAME_LEN     (6)       /* length of the name string */
#define LN_97X_APROM_SIZE       (32)      /* hardware Address PROM size bytes */
#define LN_LITTLE_ENDIAN        (0)       /* little endian PCI bus      */
#define LN_BIG_ENDIAN           (1)       /* big endian PCI bus         */
#define LN_ACTIVE_LOW           (0)       /* active-low interrupt line  */
#define LN_ACTIVE_HI            (1)       /* active-high interrupt line */

#define LN_RMD_OWNED(rmd)       (PCI_SWAP ((rmd)->lnRMD1) & RMD1_OWN)
#define LN_TMD_OWNED(tmd)       (PCI_SWAP ((tmd)->lnTMD1) & TMD1_OWN)

#define LN_PKT_LEN_GET(rmd)     (PCI_SWAP ((rmd)->lnRMD2) - 4)


#if defined(__STDC__) || defined(__cplusplus)

IMPORT STATUS    ln97xInitParse (LN_97X_DRV_CTRL * pDrvCtrl, char * initString);
IMPORT END_OBJ * ln97xEndLoad (char * initString);

#else

IMPORT STATUS    ln97xInitParse ();
IMPORT END_OBJ * ln97xEndLoad ();

#endif  /* __STDC__ */

#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCln97xEndh */

