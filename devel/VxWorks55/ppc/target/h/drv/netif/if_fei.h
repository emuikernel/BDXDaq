/* if_fei.h - Intel 82557 network interface header */

/* Copyright 1990-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,09nov01,dat  Adding obsolescence warnings to outdated drivers
01e,09mar99,cn   added typedef CSR_ID (SPR# 25242).
01d,17mar98,sbs  added PCI specific board definitions.
01c.13nov96,hdn  added two members to BOARD_INFO. removed two xxxAddrGet().
01b.06oct96,myz  added board specific data structure
01a,02sep96,dzb  written, based on v02h of h/drv/netif/if_ei.h.
*/

#ifndef __INCif_feih
#define __INCif_feih

#include "etherLib.h"

#warning "if_fei is obsolete, please use fei82557End driver"

#ifdef __cplusplus
extern "C" {
#endif

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

#define MC_FEI MC_EI  /* temp */

/* board specific infomation */
typedef struct
    {
    UINT32 vector;               /* interrupt vector number */
    UINT32 baseAddr;		 /* memory base address for CSR */
    UCHAR  enetAddr[6];		 /* ethernet address */
    int (*intEnable)(int unit);  /* board specific interrupt enable routine */
    int (*intDisable)(int unit); /* board specific interrupt disable routine */
    int (*intAck) (int unit);    /* interrupt ack */
    UINT32 (*sysLocalToBus)(int unit,UINT32 localAdr);
    UINT32 (*sysBusToLocal)(int unit,UINT32 sysAdr);

    /* configuration parameters, user can override in the BSP */
    UINT8 phyAddr;  /* PHY device address, valid addresses: 0-32 */
    UINT8 phySpeed;  
    UINT8 phyDpx;
    UINT8 fill0;

    UINT16 tcbTxThresh;

    UINT16 others; /* allow you write all bits to Mode Control Register in the
	   PHY device, don't touch it unless you know what you're doing. */

    UINT32 spare1;
    UINT32 spare2;
    UINT32 spare3;
    UINT32 spare4;
    } BOARD_INFO;

/* Intel PRO-100B PCI specific definitions */

#define PRO100B_PCI_VENDOR_ID   0x8086  /* PCI vendor ID */
#define PRO100B_PCI_DEVICE_ID   0x1229  /* PCI device ID */

/* constants needed within this file */

#define FEI_MAX_UNITS	4		/* max units supported */
#define FEI_CFD_DEF	32		/* default number of CFDs */
#define FEI_RFD_DEF	32		/* default number of RFDs */
#define FEI_RFD_LOAN	8		/* number RFDs that can be loaned */
#define EH_SIZE		14  		/* avoid structure padding issues */
#define N_MCAST		12

/* Intel 82557 endian safe link macros and structure definitions */

#define LINK_WR(pLink,value)						\
    (((pLink)->lsw = (UINT16)((UINT32)(value) & 0x0000ffff)),		\
    ((pLink)->msw  = (UINT16)(((UINT32)(value) >> 16) & 0x0000ffff)))

#define LINK_RD(pLink) ((pLink)->lsw | ((pLink)->msw << 16))

#define STAT_RD		LINK_RD		/* statistic read is a link read */
#define STAT_WR		LINK_WR		/* statistic write is a link write */
#define PORT_WR		LINK_WR		/* port write is a link write */

typedef struct fei_link 		/* FEI_LINK - endian resolvable link */
    {
    UINT16 		lsw;		/* least significant word */
    UINT16 		msw;		/* most significant word */
    } FEI_LINK;

/* Intel 82557 structure and offset */

typedef struct csr 		/* CSR - Control Status Registers */
    {
    volatile UINT16     scbStatus;
    volatile UINT16     scbCommand;

    FEI_LINK 		scbGP;		/* 0x04: SCB General Pointer */
    FEI_LINK 		port;		/* 0x08: PORT register */
    UINT16		flashCR;	/* 0x0c: FLASH Control Register */
    UINT16		eepromCR;	/* 0x0e: EEPROM Control Register */
    FEI_LINK		mdiCR;		/* 0x10: MDI Control Register */
    FEI_LINK 		rxbc;		/* 0x14: early RCV Byte Count */
    } CSR;

typedef CSR *	CSR_ID;

#define SCB_STATUS	0x00		/* SCB status byte */
#define SCB_CMD		0x02		/* SCB command byte */
#define SCB_POINTER	0x04		/* SCB pointer */
#define SCB_PORT	0x08		/* SCB port register */
#define SCB_FLASH	0x0c		/* SCB flash control register */
#define SCB_EEPROM	0x0e		/* SCB eeprom control register */
#define SCB_MDI		0x10		/* SCB mdi control register */
#define SCB_EARLYRX	0x14		/* SCB early receive byte count */

/* SCB Status Word byte mask definitions */

#define SCB_S_RUMASK	0x3c		/* RU status mask */
#define SCB_S_RUIDLE	0x00		/* RU is idle */
#define SCB_S_RUSUSP	0x04		/* RU is suspended */
#define SCB_S_RUNORSR	0x08		/* RU no more resources */
#define SCB_S_RURDY	0x10		/* RU is ready */
#define SCB_S_RUSUSPNR	0x24		/* RU is suspended/no more resources */
#define SCB_S_RUNORSRBD	0x28		/* RU no more resources/no more RBD's */
#define SCB_S_RURDYBD	0x30		/* RU is ready/no more RBD's */
#define SCB_S_CUMASK	0xc0		/* CU status mask */
#define SCB_S_CUIDLE	0x00		/* CU is idle */
#define SCB_S_CUSUSP	0x40		/* CU is suspended */
#define SCB_S_CUACTIVE	0x80		/* CU is active */
#define SCB_S_MASK      0xff

/* SCB STAT/ACK byte bit mask definitions */

#define SCB_S_STATMASK	0xfc00		/* stat mask */
#define SCB_S_SWI	0x0400		/* software generated interrupt */
#define SCB_S_MDI	0x0800		/* An MDI read or write cycle is done */
#define SCB_S_RNR	0x1000		/* RU left the ready state */
#define SCB_S_CNA	0x2000		/* CU left the active state */
#define SCB_S_CI	0x2000		/* CU entered the idle state */
#define SCB_S_FR	0x4000		/* RU finished receiveing a frame */
#define SCB_S_CX	0x8000		/* CU finished a cmd with I bit set */
#define SCB_S_TNO	0x8000		/* Tx command ended with not OK */

/* SCB Command byte bit mask definitions */

#define SCB_C_RUMASK	0x07		/* RU Command mask */
#define SCB_C_RUNOP	0x00		/* NOP */
#define SCB_C_RUSTART	0x01		/* RU start reception of frames */
#define SCB_C_RURESUME	0x02		/* RU resume reception of frames */
#define SCB_C_RUABORT	0x04		/* RU abort receiver immediately */
#define SCB_C_RULDHDS	0x05		/* RU load header data size */
#define SCB_C_RULDBASE	0x06		/* load RU base address */
#define SCB_C_RURBDRSM	0x07		/* RBD resume */
#define SCB_C_CUMASK	0x70		/* CU Command mask */
#define SCB_C_CUNOP	0x00		/* NOP */
#define SCB_C_CUSTART	0x10		/* CU start execution */
#define SCB_C_CURESUME	0x20		/* CU resume execution */
#define SCB_C_CULDDUMP	0x40		/* load dump counters address */
#define SCB_C_CUDUMP	0x50		/* dump stat counters */
#define SCB_C_CULDBASE	0x60		/* load CU base address */
#define SCB_C_CUDUMPRST	0x70		/* dump and reset stat counters */
#define SCB_CR_MASK     0xff

/* SCB Interrupt Control byte bit mask definitions */

#define SCB_C_M		0x0100		/* interrupt mask */
#define SCB_C_SI	0x0200		/* software generated interrupt */

typedef struct fei_count	/* FEI_COUNT - 82557 statistical counters */
    {
    FEI_LINK		txGood;		/* tx good frames */
    FEI_LINK		txMaxCol;	/* tx max collisions errors */
    FEI_LINK		txLateCol;	/* tx late collisions errors */
    FEI_LINK		txUnder;	/* tx underrun errors */
    FEI_LINK		txCRS;		/* tx carrier sense loss */
    FEI_LINK		txDefer;	/* tx deferred */
    FEI_LINK		txSglCol;	/* tx single collisions */
    FEI_LINK		txMultCol;	/* tx multiple collisions */
    FEI_LINK		txTotCol;	/* tx total collisions */
    FEI_LINK		rxGood;		/* rx good frames */
    FEI_LINK		rxCRC;		/* rx CRC errors */
    FEI_LINK		rxAllgn;	/* rx allignment errors */
    FEI_LINK		rxRsr;		/* rx resource errors */
    FEI_LINK		rxOver;		/* rx overrun errors */
    FEI_LINK		rxCDT;		/* rx collision detect errors */
    FEI_LINK		rxShort;	/* rx short frame errors */
    } FEI_COUNT;

/* PORT commands */

#define FEI_PORT_RESET		0x0	/* PORT reset command */
#define FEI_PORT_SELFTEST	0x1	/* PORT selftest command  */
#define FEI_PORT_SELRESET	0x2	/* PORT selective reset command */
#define FEI_PORT_DUMP		0x3	/* PORT dump command */

/* Action Command Descriptions */

typedef struct ac_iasetup 	/* AC_IASETUP - Individual Address Setup */
    {
    UINT8		ciAddress[6];	/* local ethernet address */
    UINT16		ciFill;
    } AC_IASETUP;

typedef struct ac_config 	/* AC_CONFIG - i82557 Configure */
    {
    UINT8		ccByte0;	/* byte count */
    UINT8		ccByte1;	/* FIFO limits */
    UINT8		ccByte2;	/* adaptive IFS */
    UINT8		ccByte3;	/* reserved - set to 0 */
    UINT8		ccByte4;	/* rx max byte count */
    UINT8		ccByte5;	/* tx max byte count, byte cnt enable */
    UINT8		ccByte6;	/* save bf, CI/TNO int, late SCB */
    UINT8		ccByte7;	/* underrun retry, short discard */
    UINT8		ccByte8;	/* 503/MII */
    UINT8		ccByte9;	/* reserved - set to 0 */
    UINT8		ccByte10;	/* loopback, reamble, NSAI */
    UINT8		ccByte11;	/* linear priority */
    UINT8		ccByte12;	/* interframe spacing, L. pri mode */
    UINT8		ccByte13;	/* reserved - set to 0 */
    UINT8		ccByte14;	/* reserved - set to 0xf2 */
    UINT8		ccByte15;	/* CRS+CDT, Broadcast, promisc  */
    UINT8		ccByte16;	/* reserved - set to 0 */
    UINT8		ccByte17;	/* reserved - set to 0x40 */
    UINT8		ccByte18;	/* CRC, padding, stripping */
    UINT8		ccByte19;	/* full duplex, force full duplex */
    UINT8		ccByte20;	/* multi IA */
    UINT8		ccByte21;	/* multicast */
    } AC_CONFIG;

typedef struct ac_mcast 	/* AC_MCAST - Mulitcast Setup */
    {
    UINT16		cmMcCount;	/* the number of bytes in MC list */
    UINT8		cmAddrList[6 * N_MCAST];/* mulitcast address list */
    } AC_MCAST;

typedef struct ac_tcb 		/* AC_TCB - Transmit Control Blocks */
    {
    FEI_LINK		pTBD;		/* link to tx buf desc array */
    UINT16		count;		/* length of data */
    UINT8		txThresh;	/* tx threshold */
    UINT8		tbdNum;		/* tx buf desc number */
    UINT8		enetHdr [EH_SIZE];/* the ethernet header */
    char		enetData [ETHERMTU];/* transmit data */
    UINT16		ccFill;		/* padding */
    } AC_TCB;

/* special TCB-specific command block bit mask definitions */

#define CFD_S_TX_DMA_U	0x1000		/* DMA underrun */
#define TCB_CNT_EOF	0x8000		/* all data kept in TFD */
#define CFD_C_TX_FLEX	0x0008		/* use flexible mode */

typedef struct ac_dump 		/* AC_DUMP - Dump */
    {
    FEI_LINK		bufAddr; 	/* address of dump buffer */
    } AC_DUMP;

/* special Diag-specific command block bit mask definitions */

#define CFD_S_DIAG_F	0x0800		/* diag failed */

/* Command Frame Description and defines */

typedef struct cfd 		/* CFD - Command Frame Descriptor */
    {
    volatile UINT16	cfdStatus;	/* command status field */
    UINT16		cfdCommand;	/* command field */
    FEI_LINK		link;		/* address of next CB */
    union 				/* command dependent section */
	{
	struct ac_iasetup	cfd_iasetup;	/* IA setup */
	struct ac_config	cfd_config;	/* config */
	struct ac_mcast		cfd_mcast;	/* multicast setup */
	struct ac_tcb		cfd_tcb;	/* transmit */
	struct ac_dump		cfd_dump;	/* dump */
	} cfd_cmd;
    struct cfd *	pPrev;		/* previos CFD */
    } CFD;

#define cfdIASetup	cfd_cmd.cfd_iasetup
#define cfdConfig	cfd_cmd.cfd_config
#define cfdMcast 	cfd_cmd.cfd_mcast
#define cfdTcb	 	cfd_cmd.cfd_tcb
#define cfdDump 	cfd_cmd.cfd_dump

/* Action Status Word bit mask definitions */

#define CFD_S_OK	0x2000		/* Command completed successfully */
#define CFD_S_B         0x4000          /* CU starts access this CFD */
#define CFD_S_COMPLETE	0x8000		/* Command complete */

/* Action Command Word bit mask definitions */

#define CFD_C_NOP	0x0000		/* No Operation */
#define CFD_C_IASETUP	0x0001		/* Individual Address Setup */
#define CFD_C_CONFIG	0x0002		/* Configure Chip */
#define CFD_C_MASETUP	0x0003		/* Multicast Setup */
#define CFD_C_XMIT	0x0004		/* Transmit (see below too ...) */
#define CFD_C_DUMP	0x0006		/* Dump Registers */
#define CFD_C_DIAG	0x0007		/* Diagnose */
#define CFD_C_INT	0x2000		/* 557 interrupts CPU after execution */
#define CFD_C_SUSP	0x4000		/* CU should suspend after execution */
#define CFD_C_EL	0x8000		/* End of command list */

/* 82557 Receive Frame Descriptors */

typedef struct rfd 		/* RFD - Receive Frame Descriptor */
    {
    volatile UINT16	rfdStatus;	/* status field */
    UINT16		rfdCommand;	/* command field */
    FEI_LINK		link;		/* link to next rfd */
    FEI_LINK		pRBD;		/* link to rx buf desc array */
    volatile UINT16	actualCnt;	/* actual byte count */
    UINT16		bufSize;	/* buffer size */
    UINT8		enetHdr [EH_SIZE];/* the ethernet header */
    char		enetData [ETHERMTU];/* received data */
    UINT8		refCnt;		/* reference count */
    UINT8		ccFill;		/* padding */
    struct rfd *	pPrev;		/* previos RFD */
    } RFD;

/* RFD bit mask definitions */

#define RFD_S_COLL	0x0001		/* collision during reception */
#define RFD_S_IA	0x0002		/* individual address match */
#define RFD_S_RXER	0x0010		/* receive error */
#define RFD_S_LEN	0x0020		/* type/len field designator */
#define RFD_S_SHORT	0x0080		/* frame too short */
#define RFD_S_DMA	0x0100		/* DMA Overrun failure to get bus */
#define RFD_S_RSRC	0x0200		/* received, but ran out of buffers */
#define RFD_S_ALGN	0x0400		/* received misaligned with CRC error */
#define RFD_S_CRC	0x0800		/* received with CRC error */
#define RFD_S_OK	0x2000		/* frame received successfully */
#define RFD_S_B         0x4000          /* RU begins accessing this RFD */
#define RFD_S_COMPLETE	0x8000		/* frame reception complete */

#define RFD_C_FLEX	0x0008		/* flexible mode */
#define RFD_C_HEADER	0x0010		/* frame is a header */
#define RFD_C_SUSP	0x4000		/* suspend RU after receiving frame */
#define RFD_C_EL	0x8000		/* end of RFD list */

#define RFD_CNT_F	0x4000		/* actual count updated */
#define RFD_CNT_EOF	0x8000		/* end of frame */

/* EEPROM bit definitions */

#define FEI_EESK	0x01		/* EEPROM shift clock */
#define FEI_EECS	0x02		/* EEPROM chip select */
#define FEI_EEDI	0x04		/* EEPROM data in */
#define FEI_EEDO	0x08		/* EEPROM data out */


/* Following defines should be in another header file */

/* MDI definitions */

#define MDI_READ   0x2
#define MDI_WRITE  0x1
#define MDI_CTRL_REG   0x0
#define MDI_STATUS_REG 0x1

/* MDI control register bit  */
#define MDI_CR_COLL_TEST            0x80
#define MDI_CR_FDX                  0x100       /* FDX =1, half duplex =0 */
#define MDI_CR_RESTART              0x200       /* restart auto negotiation */
#define MDI_CR_ISOLATE              0x400       /* isolate PHY from MII */
#define MDI_CR_POWER_DOWN           0x800       /* power down */
#define MDI_CR_SELECT               0x1000      /* auto speed select  */
#define MDI_CR_100                  0x2000      /* 0 = 10mb, 1 = 100mb */
#define MDI_CR_LOOPBACK             0x4000      /* 0 = normal, 1 = loopback */
#define MDI_CR_RESET                0x8000      /* 0 = normal, 1 = PHY reset */

/* MDI Status register bit definitions */
#define MDI_SR_LINK_STATUS          0x4       /* link Status -- 1 = link */
#define MDI_SR_AUTO_SELECT          0x8       /* auto speed select capable */
#define MDI_SR_REMOTE_FAULT         0x10      /* Remote fault detect */
#define MDI_SR_AUTO_NEG             0x20      /* auto negotiation complete */
#define MDI_SR_10T_HALF_DPX         0x800     /* 10BaseT half duplex capable */
#define MDI_SR_10T_FULL_DPX         0x1000    /* 10BaseT full duplex capable */
#define MDI_SR_TX_HALF_DPX          0x2000    /* TX half duplex capable */
#define MDI_SR_TX_FULL_DPX          0x4000     /* TX full duplex capable */
#define MDI_SR_T4                   0x8000    /* T4 capable */

#define PHY_100MBS      1
#define PHY_10MBS       0
#define PHY_AUTO_SPEED  2

#define PHY_FULL_DPX    1
#define PHY_HALF_DPX    0
#define PHY_AUTO_DPX    2

#define PHY_LINK_ERROR  0x10
#define PHY_AUTO_FAIL   0x20

/* PRO/100B definitions */

#define EP100B_PCI_VENDOR_ID    0x8086          /* Intel vendor ID */
#define EP100B_PCI_DEVICE_ID    0x1229          /* PRO/100B device ID */

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif	/* CPU_FAMILY==I960 */

#ifdef __cplusplus
}
#endif

#endif /* __INCif_feih */

