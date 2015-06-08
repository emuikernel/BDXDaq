/* dec21x40End.h - DEC Ethernet LAN Controller 21x40 interface header */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
03h,30may02,sru  Added REGSWAP and DEC_DRV_NOSWAP_MASTER definitions
03g,13may02,rcs  added loanBufs field to DEC21X40_DRV_CTRL SPR# 75455 
03f,22apr02,rcs  added txPollSendTd and txPollSendBuf fields to 
                 DEC21X40_DRV_CTRL SPR# 76102
03e,20jul01,rcs  added LE_BYTE_SWAP to byte swap only little endian (SPR# 69093)
03c,17jul00,ar   standardised
03c,17apr00,an   Added DEC_USR_HPNA_FORCE_FAST and SLOW flags. Add 
                 DEC_USR_HPNA_FORCE_LOW_PWR and HI_PWR flags. Removed
                 unused DEC_USR_FORCE, DEC_USR_100MB, DEC_USR_FD, and
		 DEC_USR_PHY_CHK flags.
03b,13apr00,an   Added DEC_USR_HPNA_PREFER_10BT flag to support special 
                 dec21145 media change technique.
03a,18feb00,an   Added support for 21145. Added missing 21040,21140 and 
                 21143 defines.
01w,11jun00,ham  removed reference to etherLib.
01v,08may00,jgn  SPR 31198, Added more SROM macros
01u,23mar00,dmw  Added Serial ROM EXT2 Info Block, valid media types
01t,24feb00,stv  made a selective merge to add alignment fix.
01s,06dic99,cn   re-introduced definitions of MII_READ and MII_WRITE for
		 backward compatibility (SPR #29542).
01r,08nov99,cn   Added MII-related definitions.
01q,19aug99,dmw  Added mask to IBLK_EXT_SIZE to look at only the size (0x7F).
01p,17aug99,dmw  Added defines for ANA and ANLPA registers.
01p,07apr99,dra  Added offset field for alignment fix.
01o,30mar99,dat  Fixed SROM_VERSION, SPR 26201
01n,11mar99,tm   Added CSR9/MII/21143 defines, DEC_USR_PHY_CHK flag (SPR 22196)
01m,02mar99,tm   Added intr mask to drvCtrl struct for multipass ISR (SPR 23950)
01l,18jan99,scb  Added support for END driver on dec21143.
01k,31jul98,cn   Added macro DEC_USR_FD [SPR# 21683] and field intrConnect
		 in pDrvCtrl [SPR#21557].
01j,20apr98,map  fixed endian-safe SROM_SHORT macro [SPR# 21013]
01i,24sep97,vin  changed MBLK_CONFIG to M_CL_CONFIG
01h,31aug97,vin  added a txBlocked variable to the drvCtrl structure.
01g,22aug97,gnn  changes due to new buffering scheme.
01f,21aug97,vin  modified free_buf type def added map's fixes
01e,10jun97,map  renamed funcDec21x40MediaSelect to _func_dec21x40MediaSelect.
01d,02jun97,map  added DEC Serial ROM support, and driver control definitions.
01c,24apr97,map  added PCISWAP_SHORT
01b,18apr97,map  added support for dec 21140
01a,10jan97,map  written from ../netif/if_dc.h
*/

#ifndef __INCdec21x40Endh
#define __INCdec21x40Endh

#ifdef __cplusplus
extern "C" {
#endif

#include "end.h"
#include "netBufLib.h"
#include "miiLib.h"
#include "netinet/if_ether.h"

#define DECPCI_REG_OFFSET		0x08	/* quad word aligned */

/* Endian safe macros - PCISWAP, PCISWAP_SHORT, SROM_SHORT */

#if (_BYTE_ORDER == _BIG_ENDIAN)
#   define PCISWAP(x)		LONGSWAP(x)
#   define PCISWAP_SHORT(x)	(MSB(x) | LSB(x) << 8)
#   define LE_BYTE_SWAP(x)     (x)
#else
#   define PCISWAP(x)		(x)
#   define PCISWAP_SHORT(x)	(x)
#   define LE_BYTE_SWAP(x)  (MSB(x) | LSB(x) << 8) /* little endian byte SWAP */
#endif /* _BYTE_ORDER == _BIG_ENDIAN */

/* REGSWAP - Run-time controllable PCISWAP, for use with chip registers */

#if (_BYTE_ORDER == _BIG_ENDIAN)
#    define REGSWAP(x)  (DRV_FLAGS_ISSET(DEC_NOSWAP_MASTER) ? \
                                (int)(x) : LONGSWAP((int)(x)))
#else
#    define REGSWAP(x)  (x)
#endif



#define SROM_SHORT(pX)		(*(UINT8*)(pX) | *((UINT8*)(pX)+1) << 8)

/*
 * Receive Message Descriptor Entry.
 * Four words per entry.  Number of entries must be a power of two.
 */

typedef struct rDesc
    {
    ULONG	rDesc0;		/* status and ownership */
    ULONG	rDesc1;		/* control & buffer count */
    ULONG	rDesc2;		/* buffer address 1 */
    ULONG	rDesc3;		/* buffer address 2 */
    } DEC_RD;

/*
 * Transmit Message Descriptor Entry.
 * Four words per entry.  Number of entries must be a power of two.
 */

typedef struct tDesc
    {
    ULONG	tDesc0;		/* status and ownership */
    ULONG	tDesc1;		/* control & buffer count */
    ULONG	tDesc2;		/* buffer address 1 */
    ULONG	tDesc3;		/* buffer address 2 */
    } DEC_TD;

#define MIN_RDS		5	/* 5 buffers reasonable minimum */
#define MIN_TDS		5	/* 5 buffers reasonable minimum */
#define NUM_RDS_DEF	32	/* default number of Recv descriptors */
#define NUM_TDS_DEF	64	/* default number of Xmit descriptors */
#define NUM_LOAN	16      /* number of loaner buffers */

/* HomePNA PHY reg */
/* maximum number of PHA PHY registers */

#define MAX_PNA_PHY_REGISTERS   0x20

/* define names to registers */

#define PNA_PHY_CTRL_LOW        0x0
#define PNA_PHY_CTRL_HI         0x1
#define PNA_PHY_STAT_LOW        0x2
#define PNA_PHY_STAT_HI         0x3
#define PNA_PHY_IMASK_LOW       0x4
#define PNA_PHY_IMASK_HI        0x5
#define PNA_PHY_ISTAT_LOW       0x6
#define PNA_PHY_ISTAT_HI        0x7
#define PNA_PHY_TX_PCOM_1       0x8
#define PNA_PHY_TX_PCOM_2       0x9
#define PNA_PHY_TX_PCOM_3       0xa
#define PNA_PHY_TX_PCOM_4       0xb
#define PNA_PHY_RX_PCOM_1       0xc
#define PNA_PHY_RX_PCOM_2       0xd
#define PNA_PHY_RX_PCOM_3       0xe
#define PNA_PHY_RX_PCOM_4       0xf
#define PNA_PHY_NOISE           0x10
#define PNA_PHY_PEAK            0x11
#define PNA_PHY_NSE_FLOOR       0x12
#define PNA_PHY_NSE_CEILING     0x13
#define PNA_PHY_NSE_ATTACK      0x14
#define PNA_PHY_NSE_EVENTS      0x15
#define PNA_PHY_AID_ADDR        0x19
#define PNA_PHY_AID_INTERVAL    0x1a
#define PNA_PHY_AID_ISBI        0x1b
#define PNA_PHY_ISBI_SLOW       0x1c
#define PNA_PHY_ISBI_FAST       0x1d
#define PNA_PHY_TX_PULSE_WIDTH  0x1e
#define PNA_PHY_TX_PULSE_CYCLES 0x1f

/* PNA PHY register bit values */

#define PHA_PHY_CTRL_LOW_PDN    0x08  /* PHY power down mode */  
#define PHA_PHY_CTRL_LOW_HS     0x04  /* PHY high speed mode */  
#define PHA_PHY_CTRL_LOW_HP     0x02  /* PHY high power mode */  

/* HomePNA SPI opcodes */

#define PNA_SPI_OPCODE_SET_WE   0x06
#define PNA_SPI_OPCODE_CLEAR_WE 0x04
#define PNA_SPI_OPCODE_READ     0x03
#define PNA_SPI_OPCODE_WRITE    0x02

/* HOMEPNA_PHY_REGS defines */

#define PNA_PHY_REG_VALUE      0x0
#define PNA_PHY_REG_UPDATED    0x1
typedef struct homePNAPhyInfo
    {
    USHORT   sp_csr13;  /* SP field for CSR13 */ 

    /* PHA phy register values */

    UCHAR    pna_phy_regs[MAX_PNA_PHY_REGISTERS][2];   
    } HOMEPNA_PHY_INFO;

/* define CSRs and descriptors */

#define CSR0	0		/* csr 0 */
#define CSR1	1		/* csr 1 */
#define CSR2	2		/* csr 2 */
#define CSR3	3		/* csr 3 */
#define CSR4	4		/* csr 4 */
#define CSR5	5		/* csr 5 */
#define CSR6	6		/* csr 6 */
#define CSR7	7		/* csr 7 */
#define CSR8	8		/* csr 8 */
#define CSR9	9		/* csr 9 */
#define CSR10	10		/* csr 10 */
#define CSR11	11		/* csr 11 */
#define CSR12	12		/* csr 12 */
#define CSR13	13		/* csr 13 */
#define CSR14	14		/* csr 14 */
#define CSR15	15		/* csr 15 */

#define RDESC0	0		/* recv desc 0 */
#define RDESC1	1		/* recv desc 1 */
#define RDESC2	2		/* recv desc 2 */
#define RDESC3	3		/* recv desc 3 */

#define TDESC0	0		/* xmit desc 0 */
#define TDESC1	1		/* xmit desc 1 */
#define TDESC2	2		/* xmit desc 2 */
#define TDESC3	3		/* xmit desc 3 */

/* command status register read write */

#define CSR(base,x)		((ULONG)(base) + ((DECPCI_REG_OFFSET) * (x)))

#define READ_CSR(base,x)	(PCISWAP(*((ULONG *)CSR((base),(x)))))

#define WRITE_CSR(base,x,val)	(*((ULONG *)CSR((base),(x))) = PCISWAP((val)))


/* recv xmit descriptor read write */

#define DESC(base,x)		((ULONG)(base) + (4 * (x)))

#define READ_DESC(base,x)	(PCISWAP(*((ULONG *)(DESC((base),(x))))))

#define WRITE_DESC(base,x,val)	(*((ULONG *)(DESC((base),(x)))) = PCISWAP((val)))


/* Definitions for fields and bits in the DEC_DEVICE */

/* CSR0 Bus Mode Register */

#define CSR0_21143_EONR	0x04000000	/* enable OnNow registers */
#define CSR0_21140_WIE	0x01000000	/* pci write and invalidate */
#define CSR0_21140_RLE	0x00800000	/* pci read line enable */
#define	CSR0_21140_RML	0x00200000	/* pci read multiple - 21140 */
#define CSR0_21140_DBO	0x00100000      /* descriptor byte ordering - 21140 */
#define CSR0_TAP_NO	0x00000000	/* no xmit auto polling */
#define CSR0_TAP_200	0x00020000	/* xmit poll every 200 usecs */
#define CSR0_TAP_800	0x00040000	/* xmit poll every 800 usecs */
#define CSR0_TAP_1600	0x00060000	/* xmit poll every 1.6 millsecs */
#define	CSR0_TAP_12	0x00080000	/* xmit poll every 12.8 usecs - 21140 */
#define	CSR0_TAP_25	0x000A0000	/* xmit poll every 25.6 usecs - 21140 */
#define	CSR0_TAP_51	0x000C0000	/* xmit poll every 51.2 usecs - 21140 */
#define	CSR0_TAP_102	0x000E0000	/* xmit poll every 102.4 usecs- 21140 */
#define CSR0_DAS	0x00010000	/* Diagnostic Address Space */
#define CSR0_CAL_NO	0x00000000	/* cache address alignment not used */
#define CSR0_CAL_08	0x00004000	/* 08 longword boundary aligned */
#define CSR0_CAL_16	0x00008000	/* 16 longword boundary aligned */
#define CSR0_CAL_32	0x0000c000	/* 32 longword boundary aligned */
#define	CSR0_PBL_UL	0x00000000	/* dma burst len - unlimited */
#define	CSR0_PBL_01	0x00000100	/* dma burst len -  1 lword */
#define	CSR0_PBL_02	0x00000200	/* dma burst len -  2 lwords */
#define	CSR0_PBL_04	0x00000400	/* dma burst len -  4 lwords */
#define	CSR0_PBL_08	0x00000800	/* dma burst len -  8 lwords */
#define	CSR0_PBL_16	0x00001000	/* dma burst len - 16 lwords */
#define	CSR0_PBL_32	0x00002000	/* dma burst len - 32 lwords */
#define CSR0_BLE	0x00000080	/* Big/little endian */
#define CSR0_BAR	0x00000002	/* Bus arbitration */
#define CSR0_SWR	0x00000001	/* software reset */

#define CSR0_PBL_MSK	0x00003F00	/* Dma burst length mask */
#define CSR0_PBL_VAL(x)	(((x) << 8) & CSR0_PBL_MSK)

#define CSR0_DSL_MSK	0x0000007C	/* Descriptor skip length */
#define CSR0_DSL_VAL(x) (((x) << 2) & CSR0_DSL_MSK)

#define CSR0_TAP_MSK	0x00060000
#define CSR0_BLE_MSK	0x00000080
#define	CSR0_CAL_MSK	0x0000c000


/* CSR1 Transmit Poll Demand Register */

#define CSR1_TPD	0x00000001	/* Transmit poll demand */

/* CSR2 Recieve Poll Demand Register */

#define CSR2_RPD	0x00000001	/* Transmit poll demand */

/* CSR3 Receive List Base address Register */

#define CSR3_RDBA_MSK	0xFFFFFFFC	/* long word aligned */
#define CSR3_RDBA_VAL(x) ((x) & CSR3_RDBA_MSK)

/* CSR4 Transmit List Base address Register */

#define CSR4_TDBA_MSK	0xFFFFFFFC	/* long word aligned */
#define CSR4_TDBA_VAL(x) ((x) & CSR4_TDBA_MSK)

/* CSR5 Status register */

#define CSR5_21145_HRI  0x10000000      /* HomePNA PHY interrupt */
#define CSR5_21143_LC   0x08000000      /* link changed */
#define CSR5_21143_GPI  0x04000000      /* general purpose port intterupt */

#define CSR5_ERR_PE	0x00000000	/* parity error */
#define CSR5_ERR_MA	0x00800000	/* Master abort */
#define CSR5_ERR_TA	0x01000000	/* target abort */

#define CSR5_TPS_ST	0x00000000	/* Stopped */
#define CSR5_TPS_RFTD	0x00100000	/* Running Fetch xmit descriptor */
#define CSR5_TPS_RWET	0x00200000	/* Running Wait for end of Xmission */
#define CSR5_TPS_RRBM	0x00300000	/* Running Read buff from memory */
#define CSR5_TPS_RSP	0x00500000	/* Running Set up packet */
#define CSR5_TPS_STFU	0x00600000	/* Suspended xmit FIFO underflow */
#define CSR5_TPS_RCTD	0x00700000	/* Running Close xmit descriptor */

#define CSR5_RPS_ST	0x00000000	/* stopped reset or stop rcv command */
#define CSR5_RPS_RFRD	0x00020000	/* Running Fetch rcv descriptor */
#define CSR5_RPS_RCEP	0x00040000	/* Running Check end of rcv packet */
#define CSR5_RPS_RWRP	0x00060000	/* Running Wait for rcv packet */
#define CSR5_RPS_SURB	0x00080000	/* Suspended - unavailable rcv buff */
#define CSR5_RPS_RCRD	0x000A0000	/* Running close rcv descriptor */
#define CSR5_RPS_RFFF	0x000C0000	/* flush frame from rcv FIFO */
#define CSR5_RPS_RQRF	0x000E0000	/* queue the rcv frame into rcv buff */

#define CSR5_NIS	0x00010000	/* normal interrupt summary */
#define CSR5_AIS	0x00008000	/* abnormal interrupt summary */
#define CSR5_SE		0x00002000	/* system error */
#define CSR5_21040_LNF	0x00001000	/* link fail - 21040 */
#define CSR5_21040_FD	0x00000800	/* Full duplex short frm rxd - 21040 */
#define	CSR5_21140_TMR	0x00000800	/* GP timer expired - 21140 */
#define CSR5_21040_AT	0x00000400	/* AUI / 10BaseT Pin - 21040 */
#define	CSR5_21140_ETI	0x00000400	/* Early Tx intrrupt - 21140 */
#define CSR5_RWT	0x00000200	/* rcv watchdog time-out */
#define CSR5_RPS	0x00000100	/* rcv process stopped */
#define CSR5_RU		0x00000080	/* rcv buffer unavailable */
#define CSR5_RI		0x00000040	/* rcv interrupt */
#define CSR5_UNF	0x00000020	/* xmit underflow */
#define CSR5_ANC	0x00000010	/* auto-negotiate complete */
#define CSR5_TJT	0x00000008	/* xmit jabber time-out */
#define CSR5_TU		0x00000004	/* xmit buffer unavailable */
#define CSR5_TPS	0x00000002	/* Xmit Process stopped */
#define CSR5_TI		0x00000001	/* xmit interrupt */

#define CSR5_RPS_MSK	0x000E0000	/* Rcv process state mask */
#define CSR5_TPS_MSK	0x00700000	/* Xmit process state mask */
#define CSR5_ERR_MSK	0x03800000	/* error mask */

/* CSR6 Operation Mode Register */

#define	CSR6_21140_SC	0x80000000	/* special capture effect enable */
#define	CSR6_21140_RA	0x40000000	/* receive all */
#define	CSR6_21145_IGDL	0x04000000	/* ignore dest addr LSB */
#define	CSR6_21143_IGDM	0x04000000	/* ignore dest addr MSB */
#define	CSR6_21140_MB1	0x02000000	/* must be 1 */
#define	CSR6_21140_SCR	0x01000000	/* scrambler mode */
#define	CSR6_21140_PCS	0x00800000	/* PCS function */
#define	CSR6_21140_TTM	0x00400000	/* tx threshold mode */
#define	CSR6_21140_SF	0x00200000	/* store and forward */
#define	CSR6_21140_HBD	0x00080000	/* heartbeat disable */
#define	CSR6_21140_PS	0x00040000	/* port select */
#define	CSR6_CAE	0x00020000	/* capture effect enable */
#define	CSR6_21040_BP	0x00010000	/* back pressure */
#define	CSR6_THR_072	0x00000000	/* threshold bytes 72 */
#define	CSR6_THR_096	0x00004000	/* threshold bytes 96 */
#define	CSR6_THR_128	0x00008000	/* threshold bytes 128 */
#define	CSR6_THR_160	0x0000C000	/* threshold bytes 160 */
#define	CSR6_ST		0x00002000	/* start/stop Xmit command */
#define	CSR6_FC		0x00001000	/* Force collision mode */
#define	CSR6_OM_EL	0x00000800	/* External loopback mode */
#define	CSR6_OM_IL	0x00000400	/* Internal loopback mode */
#define	CSR6_OM_NOR	0x00000000	/* normal mode */
#define	CSR6_FD		0x00000200	/* Full Duplex mode */
#define	CSR6_21040_FKD	0x00000100	/* Flaky oscillator disable */
#define	CSR6_PM		0x00000080	/* Pass all multicast */
#define	CSR6_PR		0x00000040	/* promiscuous mode */
#define	CSR6_SB		0x00000020	/* Start/Stop Back off counter */
#define	CSR6_IF		0x00000010	/* inverse filtering [RO] */
#define	CSR6_PB		0x00000008	/* pass bad frames */
#define	CSR6_HO		0x00000004	/* hash only filter mode [RO] */
#define	CSR6_SR		0x00000002	/* start/stop receive command */
#define	CSR6_HP		0x00000001	/* hash/perfect filter mode [RO] */

/* CSR7 Interrupt Mask register */

#define	CSR7_21145_HRIM	0x10000000	/* HomePNA PHY interrupt mask */ 
#define	CSR7_21143_LCM	0x08000000	/* link changed mask */ 
#define	CSR7_21143_GPM	0x04000000	/* general purpose port mask */ 
#define	CSR7_NIM	0x00010000	/* normal interrupt mask */
#define	CSR7_AIM	0x00008000	/* abnormal interrupt mask */
#define	CSR7_21040_ERM	0x00004000	/* early receive interrupt mask */ 
#define	CSR7_SEM	0x00002000	/* system error mask */
#define	CSR7_21040_LFM	0x00001000	/* link fail mask */
#define	CSR7_21040_FDM	0x00000800	/* full duplex mask */
#define	CSR7_21140_TMR	0x00000800	/* gp timer mask */
#define	CSR7_21040_ATM	0x00000400	/* aui/tp switch mask */
#define	CSR7_21140_ETM	0x00000400	/* early trasmit mask */
#define	CSR7_RWM	0x00000200	/* rcv watchdog time-out mask */
#define	CSR7_RSM	0x00000100	/* rcv stopped mask */
#define	CSR7_RUM	0x00000080	/* rcv buff unavailable mask */
#define	CSR7_RIM	0x00000040	/* rcv  interrupt mask */
#define	CSR7_UNM	0x00000020	/* underflow interrupt mask */
#define CSR7_ANC	0x00000010	/* auto-negotiation complete */
#define	CSR7_TJM	0x00000008	/* xmit jabber timer out mask */
#define	CSR7_TUM	0x00000004	/* xmit buff unavailable mask */
#define	CSR7_TSM	0x00000002	/* xmission stopped mask */
#define	CSR7_TIM	0x00000001	/* xmit interrupt mask */


/* CSR8 Missing Frame Counter */

#define	CSR8_21140_OFO		0x10000000   /* overflow counter overflow */
#define	CSR8_21140_OFC_MSK	0x0FFE0000   /* overflow counter */
#define	CSR8_MFO		0x00010000   /* missed frame overflow */
#define CSR8_MFC_MSK		0x0000FFFF   /* Missed frame counter mask */

/* CSR9 Ethernet Address ROM Register */

#define	CSR9_21145_SPI_DO	0x00800000	/* SPI serial data out */
#define	CSR9_21145_SPI_DI	0x00400000	/* SPI serial data in */
#define	CSR9_21145_SPI_CLK	0x00200000	/* SPI serial clock */
#define	CSR9_21145_SPI_CS	0x00100000	/* SPI chip select */
#define	CSR9_21145_SPI_MSK	0x00f00000	/* SPI bit mask */
#define	CSR9_21140_MDI	0x00080000	/* MII mgmt data in */
#define	CSR9_21140_MII	0x00040000	/* MII mgmt op mode */
#define	CSR9_21140_MDO	0x00020000	/* MII mgmt write data */
#define	CSR9_21140_MDC	0x00010000	/* MII mgmt clock */
#define	CSR9_21140_RD	0x00004000	/* Serial ROM Read */
#define	CSR9_21140_WR	0x00002000	/* Serial ROM Write */
#define	CSR9_21140_BR	0x00001000	/* boot rom select */
#define	CSR9_21140_SR	0x00000800	/* serial rom select */
#define	CSR9_21140_REG	0x00000400	/* external register select */

#define	CSR9_21040_DNV	0x80000000	/* Data not valid */
#define CSR9_DAT_MSK	0x000000FF	/* data mask */

#define ENET_ROM_SIZE	8		/* ethernet rom register size */

#define CSR9_MDO_SHF	17
#define CSR9_MDI_SHF	19
#define CSR9_MII_DBIT_RD(X)     (((X) & CSR9_21140_MDI) >> CSR9_MDI_SHF)
#define CSR9_MII_DBIT_WR(X)     (((X) & 0x1) << CSR9_MDO_SHF)

/* CSR10 Reserved */


/* CSR11 Full Duplex Register */

#define	CSR11_FDACV_MSK	0x0000FFFF	/* full duplex auto config mask */


/* CSR12 SIA status Register - 21040 */

#define	CSR12_21040_DA0	0x00000080	/* Diagnostic bit all One */
#define	CSR12_21040_DAZ	0x00000040	/* Diagnostic bit all zero */
#define	CSR12_21040_DSP	0x00000020	/* Diagnostic BIST status indicator */
#define	CSR12_21040_DSD	0x00000010	/* Diagnostic Self test done */
#define	CSR12_21040_APS	0x00000008	/* Auto polarity state */
#define	CSR12_21040_LKF	0x00000004	/* link fail status */
#define	CSR12_21040_NCR	0x00000002	/* network connection error */
#define	CSR12_21040_PAUI 0x00000001	/* pin AUI_TP indication */

/* CSR12 General Purpose Register - 21140 */

#define CSR12_21140_GPC	0x00000100      /* General purpose control */
#define CSR12_21140_MD	0x000000ff	/* General purpose mode/data */


/* CSR12 Auto-negotiation state values - 21143 */

#define	CSR12_21143_LPC_MSK	0xffff0000	/* link partner code bitmask */
#define	CSR12_21143_LPN		0x00008000	/* line partner negtiable */

#define CSR12_AN_MASK		0x00007000
#define CSR12_AN_DISABLED	0x00000000
#define CSR12_AN_TX_DISABLED	0x00001000
#define CSR12_AN_START		0x00001000
#define CSR12_AN_ABILITY_DETECT	0x00002000
#define CSR12_AN_ACK_DETECT	0x00003000
#define CSR12_AN_COMPLETE_ACK	0x00004000
#define CSR12_AN_COMPLETE	0x00005000
#define CSR12_AN_LINK_CHECK	0x00006000

/* CSR12 values for 21143 */

#define	CSR12_21143_TRF		0x00000800	/* transmit remote fault */
#define	CSR12_21143_NSN		0x00000400	/* non-stable NLPs detected */
#define	CSR12_21143_TRA		0x00000200	/* 10BASE-T receive port activity */
#define	CSR12_21145_HRA		0x00000100	/* HomePNA receive port activity */
#define	CSR12_21143_ARA		0x00000100	/* AUI receive port activity */
#define	CSR12_21143_APS		0x00000008	/* Auto polarity state */
#define CSR12_21143_LS10	0x00000004
#define CSR12_21143_LS100	0x00000002
#define CSR12_21143_MII		0x00000001

/* CSR13 SIA connectivity Register */

#define CSR13_21145_SP_MSK 0xffff0000	/* special field for HomePNA use */
#define CSR13_OE57	0x00008000	/* Output enable 5 6 7 */
#define CSR13_OE24	0x00004000	/* output enable 2 4 */
#define CSR13_OE13	0x00002000	/* output enable 1 3 */
#define CSR13_IE	0x00001000	/* input enable */
#define CSR13_SEL_LED	0x00000f00	/* select LED and external driver */
#define CSR13_ASE_APLL	0x00000080	/* ase apll start enable */
#define CSR13_SIM	0x00000040	/* serial iface input multiplexer */
#define CSR13_ENI	0x00000020	/* encoder Input multiplexer */
#define CSR13_EDP_SIA	0x00000010	/* pll external input enable */
#define CSR13_AUI_TP	0x00000008	/* AUI or 10BASE-T or AUI */
#define CSR13_CAC_CSR	0x00000004	/* auto config register */
#define	CSR13_PS	0x00000002	/* pin AUI_TP select */
#define CSR13_SRL_SIA	0x00000001	/* srl sia Reset */

/* CSR14 SIA xmit rcv Register */

#define CSR14_21143_T4	0x00040000      /* 1000Base-T4 -21143 */
#define CSR14_21143_TXF	0x00020000      /* 100Base-TX full duplex -21143 */
#define CSR14_21143_TXH	0x00010000      /* 100Base-TX half duplex -21143 */
#define CSR14_21143_TAS	0x00008000      /* 10Base-T/AUI autosensing -21143  */
#define CSR14_SPP	0x00004000	/* set polarity plus */
#define CSR14_APE	0x00002000	/* auto polarity enable */
#define CSR14_LTE	0x00001000	/* link test enable */
#define CSR14_SQE	0x00000800	/* signal quality generate enable */
#define CSR14_CLD	0x00000400	/* collision detect enable */
#define CSR14_CSQ	0x00000200	/* collision squelch enable */
#define CSR14_RSQ	0x00000100	/* receive squelch enable */
#define CSR14_21143_ANE 0x00000080      /* autonegotiation enable */
#define CSR14_21143_TH  0x00000040      /* 10Base-T half duplex enable */
#define CSR14_CPEN_NC	0x00000030	/* no compensation */
#define CSR14_CPEN_HP	0x00000020	/* high power mode */
#define CSR14_CPEN_DM	0x00000010	/* disable mode */
#define CSR14_LSE	0x00000008	/* link pulse send enable */
#define CSR14_DREN	0x00000004	/* driver enable */
#define CSR14_LBK	0x00000002	/* loopback enable */
#define CSR14_ECEN	0x00000001	/* encoder enable */

/* CSR15 SIA general register */

#define CSR15_21143_RMI	0x40000000      /* receive match interrupt */
#define CSR15_21143_GI1	0x20000000      /* general port interrupt 1 */
#define CSR15_21143_GI0	0x10000000      /* general port interrupt 0 */
#define CSR15_21143_CWE	0x08000000      /* control write enable */
#define CSR15_21143_RME	0x04000000      /* receive match enable */
#define CSR15_21143_GE1	0x02000000      /* GEP interrupt enable on  port 1 */
#define CSR15_21143_GE0	0x01000000      /* GEP interrupt enable on  port 0 */
#define CSR15_21143_LG3	0x00800000      /* LED/GEP3 select */
#define CSR15_21143_LG2	0x00400000      /* LED/GEP2 select */
#define CSR15_21143_LG1	0x00200000      /* LED/GEP1 select */
#define CSR15_21143_LG0	0x00100000      /* LED/GEP0 select */
#define CSR15_21143_LEE	0x00000800      /* link extend enable */
#define CSR15_21143_RWR	0x00000020      /* receive watchdog release */
#define CSR15_21143_RWD	0x00000010      /* receive watchdog disable */
#define CSR15_21143_ABM	0x00000008      /* AUI/BNC mode */
#define CSR15_JCK	0x00000004	/* jabber clock */
#define CSR15_HUJ	0x00000002	/* host unjab */
#define CSR15_JBD	0x00000001	/* jabber disable */

#define CSR15_MD_MSK    0x000F0000      /* general purpose mode mask */
#define CSR15_MODE_10	0x00050000

#define CSR15_MD_VAL(x) (((x) << 16) & CSR15_MD_MSK)

/* receive descriptor */

/* receive descriptor 0 */

#define RDESC0_OWN		0x80000000	/* Own */
#define RDESC0_ES		0x00008000	/* Error summary */
#define RDESC0_LE		0x00004000
#define RDESC0_DT_SRF		0x00000000	/* serial rcvd frame */
#define RDESC0_DT_ILF		0x00001000	/* internal loop back frame */
#define RDESC0_DT_ELF		0x00002000	/* external loop back frame */
#define RDESC0_RF		0x00000800	/* runt frame */
#define RDESC0_MF		0x00000400	/* multicast frame */
#define RDESC0_FD		0x00000200	/* first descriptor */
#define RDESC0_LS		0x00000100	/* last descriptor */
#define RDESC0_TL		0x00000080	/* frame too long */
#define RDESC0_CS		0x00000040	/* collision seen */
#define RDESC0_FT		0x00000020	/* frame type */
#define RDESC0_RJ		0x00000010	/* receive watch dog */
#define RDESC0_DB		0x00000004	/* dribbling bit */
#define RDESC0_CE		0x00000002	/* crc error */
#define RDESC0_OF		0x00000001	/* Over flow */

#define DEC_FRAME_LEN_MSK	0x3FFF0000	/* Frame length mask */

#define DEC_FRAME_LEN_GET(x)	(((x) & DEC_FRAME_LEN_MSK) >> 16)
#define DEC_FRAME_LEN_SET(x)	(((x) << 16) & DEC_FRAME_LEN_MSK)

/* receive descriptor 1 */

#define RDESC1_RER		0x02000000	/* recv end of ring */
#define RDESC1_RCH		0x01000000	/* second address chained */

#define RDESC1_RBS2_MSK		0x003FF800	/* RBS2 buffer 2 size */
#define RDESC1_RBS1_MSK		0x000007FF	/* RBS1 buffer 1 size */

#define RDESC1_RBS1_VAL(x)	((x) & RDESC1_RBS1_MSK)	/* multiple of 4 */
#define RDESC1_RBS2_VAL(x)	(((x) << 11) & RDESC1_RBS2_MSK)

/* transmit descriptor */

/* xmit descriptor 0 */

#define TDESC0_OWN		0x80000000	/* own */
#define TDESC0_ES		0x00008000	/* error summary */
#define TDESC0_TO		0x00004000	/* xmit jabber time out */
#define TDESC0_LO		0x00000800	/* loss of carrier */
#define TDESC0_NC		0x00000400	/* NC No carrier */
#define TDESC0_LC		0x00000200	/* late collision */
#define TDESC0_EC		0x00000100	/* excessive collision */
#define TDESC0_HF		0x00000080	/* heart beat fail */
#define TDESC0_LF		0x00000004	/* link fail */
#define TDESC0_UF		0x00000002	/* underflow error */
#define TDESC0_DE	        0x00000001	/* deffered */

#define TDESC0_CC_MSK		0x00000078
#define	TDESC0_CC_VAL(X)	(((X) & TDESC0_CC_MSK) >> 3)

/* xmit descriptor 1 */

#define TDESC1_IC		0x80000000	/* interrupt on completion */
#define TDESC1_LS		0x40000000	/* last segment */
#define TDESC1_FS		0x20000000	/* first segment */
#define TDESC1_FT1		0x10000000	/* filtering type */
#define TDESC1_SET		0x08000000	/* setup packet */
#define TDESC1_AC		0x04000000	/* add crc disable */
#define TDESC1_TER		0x02000000	/* xmit end of ring */
#define TDESC1_TCH		0x01000000	/* second address chained */
#define TDESC1_DPD		0x00800000	/* disabled padding */
#define TDESC1_FT0		0x00400000	/* filtering type */

#define TDESC1_TBS2_MSK		0x003FF800	/* TBS2 buffer 2 size */
#define TDESC1_TBS1_MSK		0x000007FF	/* TBS2 buffer 1 size */

#define TDESC1_TBS1_PUT(x)	((x) & TDESC1_TBS1_MSK)	/* multiple of 4 */
#define TDESC1_TBS2_PUT(x)	(((x) << 11) & TDESC1_TBS2_MSK)

#define FLTR_FRM_SIZE		0xC0		/* filter frm size 192 bytes */
#define FLTR_FRM_SIZE_ULONGS	(FLTR_FRM_SIZE / sizeof (ULONG))
#define FLTR_FRM_ADRS_NUM	0x10		/* filter frm holds 16 addrs */
#define FLTR_FRM_ADRS_SIZE	0x06		/* size of each phys addrs */
#define FLTR_FRM_DEF_ADRS	0xFFFFFFFF	/* enet broad cast address */
#define FLTR_FRM_PHY_ADRS_OFF	156             /* word - 39 */

#define DEC_CRC_POLY		0x04c11db6   /* for CRC computation */
#define DEC_FLT_INDEX(I)	((((I) & ~0x1) * 2) + ((I) & 0x1))

/* MII Defines */

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

#define MII_PHY_SR_

/* DEC Serial ROM */

#define DEC21140_SROM_SIZE	128
#define DEC21140_SROM_WORDS	64
#define	DEC21140_SROM_VERSION_3	0x3
#define	DEC21140_SROM_VERSION_4	0x4

/* Serial ROM access macros */

#define SROM_VERSION(pSrom)		(UCHAR) *(pSrom + 0x12)
#define SROM_ILEAF0_OFFSET(pSrom)      	SROM_SHORT (pSrom + 0x1b)

/* Serial ROM Info Leaf */

#define	ILEAF_GPR_MODE(pILeaf)		(UCHAR) *(pILeaf + 2)
#define ILEAF_MEDIA_COUNT(pILeaf)	(UCHAR) *(pILeaf + 3)
#define	ILEAF_INFO_BLK0(pILeaf)		(UCHAR *) (pILeaf + 4)

/* Serial ROM Info Leaf  - 21143 */

#define ILEAF_21143_MEDIA_COUNT(pILeaf) (UCHAR) *(pILeaf + 2)
#define ILEAF_21143_INFO_BLK0(pILeaf)   (UCHAR *) (pILeaf + 3)

/* Serial ROM Info Leaf  - 21145 */

#define ILEAF_21145_MEDIA_COUNT(pILeaf) (UCHAR) *(pILeaf + 2)
#define ILEAF_21145_INFO_BLK0(pILeaf)   (UCHAR *) (pILeaf + 3)

/* Serial ROM Info Block */

#define	IBLK_COMPACT_SIZE		4
#define IBLK_IS_COMPACT(pIBlk)		((*(pIBlk) & 0x80) == 0x00)
#define	IBLK_IS_EXT(pIBlk)		((*(pIBlk) & 0x80) == 0x80)
#define IBLK_IS_EXT0(pIBlk)                                             \
	    (IBLK_IS_EXT(pIBlk) && (IBLK_EXT_TYPE(pIBlk) == 0x00))

#define IBLK_IS_EXT1(pIBlk)		                                \
	    (IBLK_IS_EXT(pIBlk) && (IBLK_EXT_TYPE(pIBlk) == 0x01))


#define IBLK_COMPACT_MCODE(pIBlk)	(UCHAR) (*pIBlk & 0x7F)
#define	IBLK_COMPACT_GPDATA(pIBlk)	(UCHAR) *(pIBlk+1)
#define IBLK_COMPACT_CMD(pIBlk)		SROM_SHORT(pIBlk+2)

#define IBLK_EXT_SIZE(pIBlk)		(UCHAR) (*(pIBlk+0) & 0x7F)
#define IBLK_EXT_TYPE(pIBlk)		(UCHAR) *(pIBlk+1)

#define IBLK_IS_EXT2    0x02    /* Block type 2 */
#define IBLK_IS_EXT3    0x03    /* Block type 3 */
#define IBLK_IS_EXT4    0x04    /* Block type 4 */
#define IBLK_IS_EXT5    0x05    /* Block type 5 */
#define IBLK_IS_EXT7    0x07    /* Block type 7 */


#define IBLK_EXT0_TO_COMPACT(pIBlk)	(UCHAR *)(pIBlk+2)
#define IBLK_EXT1_PHY(pIBlk)		(UCHAR) *(pIBlk+2)
#define	IBLK_EXT1_INIT_LEN(pIBlk)	(UCHAR) *(pIBlk+3)
#define	IBLK_EXT1_INIT_STR(pIBlk)	(UCHAR *)(pIBlk+4)
#define	IBLK_EXT1_RESET_LEN(pIBlk)                                           \
    (UCHAR) *(IBLK_EXT1_INIT_STR(pIBlk) + IBLK_EXT1_INIT_LEN(pIBlk))
#define IBLK_EXT1_RESET_STR(pIBlk)	                                     \
    (UCHAR *) (IBLK_EXT1_INIT_STR(pIBlk) + IBLK_EXT1_INIT_LEN(pIBlk) + 1)
#define IBLK_EXT1_MEDIA_CAP(pIBlk)                                           \
    SROM_SHORT( IBLK_EXT1_RESET_STR(pIBlk) + IBLK_EXT1_RESET_LEN(pIBlk))
#define IBLK_EXT1_AUTO_AD(pIBlk)                                             \
    SROM_SHORT( IBLK_EXT1_RESET_STR(pIBlk) + IBLK_EXT1_RESET_LEN(pIBlk) + 2)
#define	IBLK_EXT1_FD_MAP(pIBlk)                                              \
    SROM_SHORT( IBLK_EXT1_RESET_STR(pIBlk) + IBLK_EXT1_RESET_LEN(pIBlk) + 4)
#define IBLK_EXT1_TTM_MAP(pIBlk)                                             \
    SROM_SHORT( IBLK_EXT1_RESET_STR(pIBlk) + IBLK_EXT1_RESET_LEN(pIBlk) + 6)

/* Extended format - Block type 2, 3, 4 & 5 - 21143 */

#define IBLK_EXT2_MCODE(pIBlk)		(UCHAR) (*(pIBlk + 2) & 0x3F)
#define IBLK_EXT2_EXT(pIBlk)		(UCHAR) ((*(pIBlk + 2) & 0x40) >> 6)
#define IBLK_EXT2_MSD_CSR13(pIBlk)      SROM_SHORT(pIBlk + 3)
#define IBLK_EXT2_MSD_CSR14(pIBlk)      SROM_SHORT(pIBlk + 5)
#define IBLK_EXT2_MSD_CSR15(pIBlk)      SROM_SHORT(pIBlk + 7)
#define	IBLK_EXT2_GPC(pIBlk)						    \
    SROM_SHORT( pIBlk + 3 + ((((UCHAR) *pIBlk) & 0x7F) - 0x6))
#define IBLK_EXT2_GPD(pIBlk)						    \
    SROM_SHORT( pIBlk + 5 + ((((UCHAR) *pIBlk) & 0x7F) - 0x6))

#define IBLK_EXT3_PHY(pIBlk)            (UCHAR) *(pIBlk+2)
#define IBLK_EXT3_INIT_LEN(pIBlk)       (UCHAR) *(pIBlk+3)
#define IBLK_EXT3_INIT_STR(pIBlk)       (UCHAR *)(pIBlk+4)
#define IBLK_EXT3_RESET_LEN(pIBlk)                                           \
    (UCHAR) *(IBLK_EXT3_INIT_STR(pIBlk) + IBLK_EXT3_INIT_LEN(pIBlk))
#define IBLK_EXT3_RESET_STR(pIBlk)                                           \
    (UCHAR *) (IBLK_EXT3_INIT_STR(pIBlk) + IBLK_EXT3_INIT_LEN(pIBlk) + 1)
#define IBLK_EXT3_MEDIA_CAP(pIBlk)                                           \
    SROM_SHORT( IBLK_EXT3_RESET_STR(pIBlk) + IBLK_EXT3_RESET_LEN(pIBlk))
#define IBLK_EXT3_AUTO_AD(pIBlk)                                             \
    SROM_SHORT( IBLK_EXT3_RESET_STR(pIBlk) + IBLK_EXT3_RESET_LEN(pIBlk) + 2)
#define IBLK_EXT3_FD_MAP(pIBlk)                                              \
    SROM_SHORT( IBLK_EXT3_RESET_STR(pIBlk) + IBLK_EXT3_RESET_LEN(pIBlk) + 4)
#define IBLK_EXT3_TTM_MAP(pIBlk)                                             \
    SROM_SHORT( IBLK_EXT3_RESET_STR(pIBlk) + IBLK_EXT3_RESET_LEN(pIBlk) + 6)
#define IBLK_EXT3_MII_CI(pIBlk)                                             \
    SROM_SHORT( IBLK_EXT3_RESET_STR(pIBlk) + IBLK_EXT3_RESET_LEN(pIBlk) + 8)

#define IBLK_EXT4_MCODE(pIBlk)		(UCHAR) (*(pIBlk + 2) & 0x3F)
#define	IBLK_EXT4_GPC(pIBlk)		SROM_SHORT(pIBlk + 3 )
#define	IBLK_EXT4_GPD(pIBlk)		SROM_SHORT(pIBlk + 5 )
#define	IBLK_EXT4_CMD(pIBlk)		SROM_SHORT(pIBlk + 7 )
#define	IBLK_EXT4_CMD_PS		0x0001
#define	IBLK_EXT4_CMD_TTM		0x0010
#define	IBLK_EXT4_CMD_PCS		0x0020
#define	IBLK_EXT4_CMD_SCR		0x0040

#define IBLK_EXT5_RESET_LEN(pIBlk)	(UCHAR) *(pIBlk+2)

#define	IBLK_EXT7_ANALOG_CTRL(pIBlk)	SROM_SHORT(pIBlk + 2 ) 
#define	IBLK_EXT7_CTRL_LOW(pIBlk)	(UCHAR) *(pIBlk + 4 ) 
#define	IBLK_EXT7_CTRL_HI(pIBlk)	(UCHAR) *(pIBlk + 5 ) 
#define	IBLK_EXT7_NOISE(pIBlk)		(UCHAR) *(pIBlk + 6 ) 
#define	IBLK_EXT7_NSE_FLOOR(pIBlk)	(UCHAR) *(pIBlk + 7 ) 
#define	IBLK_EXT7_NSE_CEILING(pIBlk)	(UCHAR) *(pIBlk + 8 ) 
#define	IBLK_EXT7_NSE_ATTACK(pIBlk)	(UCHAR) *(pIBlk + 9 ) 
#define	IBLK_EXT7_EXTRA_INDEX(pIBlk)    (UCHAR) (0x1f & *(pIBlk))  
#define	IBLK_EXT7_EXTRA_DATA(pIBlk)     (UCHAR) *(pIBlk +1 )  

/* Serial ROM Compact Info Block, command field */

#define COMPACT_CMD_ACT_INV	0x8000       /* Active Invalid */
#define	COMPACT_CMD_MED_DEF	0x4000       /* Default Media */
#define	COMPACT_CMD_POLARITY	0x0080       /* Media bit polarity */
#define COMPACT_CMD_SCR		0x0040       /* Scrambler mode */
#define	COMPACT_CMD_PCS		0x0030       /* PCS Function */
#define	COMPACT_CMD_MED_SENSE	0x000E       /* Media Sense */
#define	COMPACT_CMD_PS		0x0001       /* Port Select */

/* Serial ROM EXT1 Info Block, valid media types */

#define EXT1_MEDIA_100BT4	0x0200
#define EXT1_MEDIA_100BTX_FD	0x0100
#define EXT1_MEDIA_100BTX	0x0080
#define EXT1_MEDIA_10BT_FD	0x0040
#define EXT1_MEDIA_10BT		0x0020
#define	EXT1_MEDIA_CAP_MSK	0x03E0

/* Serial ROM EXT2 Info Block, valid media types */

#define EXT2_MEDIA_10TP		0x0000  /* 10baseT (Twisted Pair) */
#define EXT2_MEDIA_BNC		0x0001  /* 10base2 (BNC) */
#define EXT2_MEDIA_AUI		0x0002  /* 10base5 (AUI) */
#define EXT2_MEDIA_10FD		0x0004  /* 10baseT Full Duplex */
#define EXT2_MEDIA_HOMEPNA      0x0012  /* Home PNA */

typedef struct free_buf
    {
    void *	pClBuf;			     /* pointer cluster buffer */
    } FREE_BUF;

/* The dec21x40End driver control structure */

typedef struct dec21x40DrvCtrl
    {
    END_OBJ		endObj;              /* base class   */
    int			flags;               /* driver flags */

    int			unit;                /* unit number */
    ULONG		devAdrs;             /* IO base address */
    int			ivec;                /* interrupt vector */
    int			ilevel;              /* interrupt level */
    UINT32		intrMask;            /* interrupt mask */
    char *		memBase;             /* descriptor mempool base addr */
    ULONG		memSize;             /* descriptor mempool size */
    ULONG		pciMemBase;          /* memory base on PCI adr space */
    ULONG		usrFlags;
    ULONG		drvFlags;
    int			offset;		     /* offset to fix alignment */

    int			numRds;              /* RD ring size */
    int			rxIndex;             /* index into RD ring */
    DEC_RD *		rxRing;              /* RD ring */

    int			numTds;              /* TD ring size */
    int			txIndex;             /* index into TD ring */
    int			txDiIndex;           /* disposal index into TD ring */
    DEC_TD *		txRing;              /* TD ring */

    UINT8		mediaCount;          /* Number of PHY devices [RW] */
    UINT8		mediaDefault;        /* Default PHY device [RW] */
    UINT8		mediaCurrent;        /* Current PHY device [RW] */
    UINT8		gprModeVal;          /* Mode bits for GP register [RW]*/

    BOOL		rxHandling;
    BOOL		txCleaning;
    BOOL		intrConnect;	     /* intr routine connected */

    CACHE_FUNCS 	cacheFuncs;          /* cache function pointers */
    BOOL		txBlocked;	     /* variable for blocking */
    FREE_BUF		freeBuf[128];
    CL_POOL_ID		clPoolId;

    int			rxLen;               /* current receive bytes */
    int			rxMaxLen;            /* receive managment limit */

    PHY_INFO            *pPhyInfo;	     /* info on a MII-compliant PHY */
    UINT32		miiPhyFlags;	     /* MII-compliant PHY flags */
    UINT8		phyAddr;	     /* MII-compliant PHY address */
    MII_AN_ORDER_TBL *	pMiiPhyTbl;	     /* MII-compliant PHY's table */
    HOMEPNA_PHY_INFO    *pHomePNAPhyInfo;   /* info on a HomePNA PHY */
    BOOL                homePNAPhyValuesFound; /* existance of defaults */
    char                *txPollSendBuf;	     /* buffer for poll mode send */
    int                 loanBufs;            /* number of loaner clusters */
    } DEC21X40_DRV_CTRL;

IMPORT FUNCPTR	_func_dec21x40MediaSelect;

#define DRV_NAME		"dc"
#define DRV_NAME_LEN            3
#define EADDR_LEN		6
#define ETH_CRC_LEN		4
#define DEC_BUFSIZ		(ETHERMTU + SIZEOF_ETHERHEADER + EADDR_LEN)

/* DRV_CTRL flags */
#define DEC_MEMOWN		0x0001	/* TDs and RDs allocated by driver */
#define DEC_TX_KICKSTART	0x0002	/* No transmit poll */
#define DEC_POLLING		0x0004	/* Poll mode, io mode */
#define DEC_PROMISC		0x0008	/* Promiscuous, rx mode */
#define DEC_MCAST		0x0010	/* Multicast, rx mode */
#define DEC_BSP_EADRS		0x0020  /* Use BSP routine to get ether addr */
#define DEC_NOSWAP_MASTER	0x0040  /* Don't swap PCI bus master I/O */
#define DEC_21140		0x4000	/* DEC21140 device type */
#define DEC_21040		0x8000	/* DEC21040 device type */
#define DEC_21143		0x2000	/* DEC21143 device type */
#define DEC_21145		0x1000	/* DEC21145 device type */

/*
 * User options
 *
 * These options may be set in the userFlags field of the dec21x40End driver
 * load string. The driver, at initialization, will set up apt registers to
 * reflect the selected options.
 */

#define DEC_USR_BE	0x00000001	/* CSR0: Big Endian */

#define	DEC_USR_TAP_02	0x00000002	/* CSR0: Tx poll every 200 usec */
#define	DEC_USR_TAP_08	0x00000004	/* CSR0: Tx poll every 800 usec */
#define	DEC_USR_TAP_16	0x00000006	/* CSR0: Tx poll every 1.6 msec */
#define DEC_USR_TAP_012	0x00000008      /* CSR0: Tx poll every 12.8 usec */
#define DEC_USR_TAP_025	0x0000000A      /* CSR0: Tx poll every 25.6 usec */
#define DEC_USR_TAP_051	0x0000000C      /* CSR0: Tx poll every 51.2 usec */
#define DEC_USR_TAP_102	0x0000000E      /* CSR0: Tx poll every 102.4 usec */
#define DEC_USR_TAP_MSK 0x0000000E
#define DEC_USR_TAP_SHF	16

#define	DEC_USR_CAL_08	0x00000010	/* CSR0: Cache adrs aligned  8 lwords */
#define	DEC_USR_CAL_16	0x00000020	/* CSR0: Cache adrs aligned 16 lwords */
#define	DEC_USR_CAL_32	0x00000030	/* CSR0: Cache adrs aligned 32 lwords */
#define	DEC_USR_CAL_MSK	0x00000030
#define DEC_USR_CAL_SHF	10

#define	DEC_USR_PBL_01	0x00000040	/* CSR0: DMA burst len  1 lword */
#define	DEC_USR_PBL_02	0x00000080	/* CSR0: DMA burst len  2 lwords */
#define	DEC_USR_PBL_04	0x00000100	/* CSR0: DMA burst len  4 lwords */
#define	DEC_USR_PBL_08	0x00000200	/* CSR0: DMA burst len  8 lwords */
#define	DEC_USR_PBL_16	0x00000400	/* CSR0: DMA burst len 16 lwords */
#define	DEC_USR_PBL_32	0x00000800	/* CSR0: DMA burst len 32 lwords */
#define	DEC_USR_PBL_MSK	0x00000FC0
#define DEC_USR_PBL_SHF	2

#define	DEC_USR_BAR_RX	0x00001000	/* CSR0: Rx has priority over Tx */
#define	DEC_USR_RML	0x00002000	/* CSR0: PCI memory-read-multiple */
#define	DEC_USR_XEA	0x00004000      /* Use sysDec21x40EnetAddrGet() */
#define	DEC_USR_SF	0x00008000	/* Enable store&forward CSR6:21 */

#define DEC_USR_THR_072	0x00000000      /* Use TxTheshold 72/128 bytes */
#define	DEC_USR_THR_096 0x00010000      /* Use TxTheshold 96/256 bytes */
#define	DEC_USR_THR_128 0x00020000      /* Use TxTheshold 128/512 bytes */
#define	DEC_USR_THR_160 0x00030000      /* Use TxTheshold 160/1024 bytes */
#define DEC_USR_THR_MSK 0x00030000
#define DEC_USR_THR_SHF	2

#define DEC_USR_SB	0x00040000      /* Enable backoff counter CSR6:5 */
#define DEC_USR_PB	0x00080000	/* Enable pass bad frame  CSR6:3 */
#define DEC_USR_SC	0x00100000	/* Enable spl capture effect CSR6:31 */
#define DEC_USR_CA	0x00200000	/* Enable capture effect CSR6:17 */

/*
 * since USR_FORCE_MODE, USR_100MB, USR_FD, USR_PHY_CHK dont seem to be 
 * used anymore by the current drirver, we take over their bits for HomePNA
 */

/* FORCE HomePNA to desired modes */

#define DEC_USR_HPNA_FORCE_LOW_PWR 0x00400000 /* low power HomePNA */
#define DEC_USR_HPNA_FORCE_HI_PWR  0x00800000 /* high power HomePNA */
#define DEC_USR_HPNA_FORCE_SLOW    0x01000000 /* low speed HomePNA 0.7 MB/s */
#define DEC_USR_HPNA_FORCE_FAST    0x02000000 /* hi speed HomePNA 1 MB/s */

#define DEC_USR_MII    	   0x04000000	/* use additional pars for MII PHY */

#define DEC_USR_HPNA_PREFER_10BT   0x08000000	/* continually autonegotiate */
						/* 10BT link and select if avaliable */
#define	DEC_USR_21145	0x20000000	/* Intel/DEC 21145 part */
#define	DEC_USR_21143	0x40000000	/* DEC 21143 part */
#define	DEC_USR_21140	0x80000000	/* DEC 21140 part */
#define	DEC_USR_VER_MSK	0xE0000000	/* DEC version mask */



#define	DEC_USR_CSR6_MSK (CSR6_21140_PS | CSR6_21140_PCS | CSR6_21140_SCR | \
			  CSR6_21140_TTM | CSR6_FD | CSR6_21140_HBD)

/* Special MII flag bit definitions, not the user flag bits */

#define DEC_USR_MII_BUS_MON	0x01000000	/* monitor the MII bus */
#define DEC_USR_MII_AN_TBL	0x04000000	/* use auto-negotiation table */
#define DEC_USR_MII_NO_AN	0x08000000	/* do not auto-negotiate */
#define DEC_USR_MII_FD		0x10000000	/* allow full duplex */
#define DEC_USR_MII_100MB	0x20000000	/* allow 100 MB */
#define DEC_USR_MII_HD		0x40000000	/* allow half duplex */
#define DEC_USR_MII_10MB	0x80000000	/* allow 10 MB */


/*
 * Driver options
 *
 * These options may be set in the drvFlags field of the dec21x40End driver
 * load string.  The driver's run time behavior can be controlled by these
 * flags.
 */

#define DEC_DRV_NOSWAP_MASTER	0x00000001	/* don't swap PCI master I/O */


/*
 * Device specific network configuration defined in configNet.h
 */

IMPORT M_CL_CONFIG	dec21x40MBlkConfig;  /* mblk config description */
IMPORT CL_DESC		dec21x40ClDescTbl[]; /* cluster pool config table */
IMPORT int		dec21x40ClDescTblEnt;

#ifdef __cplusplus
}
#endif

#endif /* __INCdec21x40Endh */
