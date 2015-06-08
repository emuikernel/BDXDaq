/* if_dcFast.h - DEC Ethernet LAN Controller 21040 interface header */

/* Copyright 1984-1995 Wind River Systems, Inc. */
/*
modification history
--------------------
rcp - Robert C. Persons of Motorola Computer Group
01d,05mar96,rcp  added support for DEC21140
01c,05sep95,vin  added setup/filter frame defines.
01b,04apr95,caf  changed name from "dcPci" to "dc".
01a,02mar95,vin  written.
*/

#ifndef __INCif_dcFasth
#define __INCif_dcFasth

#ifdef __cplusplus
extern "C" {
#endif

#define DECPCI_REG_OFFSET		0x08	/* quad word aligned */

/* Definitions for the flags field */

#define DC_PROMISCUOUS_FLAG     	0x01	/* set promiscuous mode */
#define DC_MULTICAST_FLAG		0x02	/* pass all multicast pkts */

/* RCP - Added modes for 100-Mb/s Ethernet configuration */
#define DC_100_MB_FLAG			0x04	/* 100-Mb/s mode */	
#define DC_21140_FLAG			0x08	/* Lance chip is 21140 */
#define DC_FULLDUPLEX_FLAG		0x10	/* Full Duplex Mode */
#define DC_SCRAMBLER_FLAG		0x20	/* MII/SYM in scrambler mode */
#define DC_PCS_FLAG			0x40	/* MII/SYM in symbol mode */
#define DC_PS_FLAG			0x80	/* Serial Port selected  */
#define DC_ILOOPB_FLAG			0x100	/* Internal Loop Back Mode */
#define DC_ELOOPB_FLAG			0x200	/* External Loop Back Mode */
#define DC_HBE_FLAG			0x400	/* Heart Beat Enable Flag */

/* RCP - Added this general mode */

#if (_BYTE_ORDER == _BIG_ENDIAN)

#define PCISWAP(x)	LONGSWAP(x)		/* processor big endian */

#else

#define PCISWAP(x)	(x)			/* processor little endian */

#endif /* _BYTE_ORDER == _BIG_ENDIAN */

/* The Ethernet header */

typedef struct enet_hdr
    {
    char 	dst [6];	/* destination ethernet address */
    char 	src [6];	/* source ethernet address */
    USHORT	type;		/* type of ethernet packet */
    } ENET_HDR;

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
    } DC_RDE;

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
    } DC_TDE;

#define MIN_RDS		5	/* 5 buffers reasonable minimum */	
#define MIN_TDS		5	/* 5 buffers reasonable minimum */
#define NUM_RDS		32	/* default number of Recv descriptors */
#define NUM_TDS		32	/* default number of Xmit descriptors */

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

/* RCP - Configuration ID Register added for DEC21140 */

#define SROM_SIZE	128
#define CFID_DEVID_MASK	0xFFFF0000
#define CFID_VENID_MASK	0x0000FFFF

#define DEC21040_ID	0x00020000
#define DEC21140_ID	0x00090000
#define PMC_ETHERNET	0x0000F801 /* Ethernet bytes assigned to DEC PMC*/
/*  This value determines the position in the serial rom for the final
    ethernet value on DEC 520-AA PMC modules */
#define DEC_PMC_POS	10

/* Definitions for fields and bits in the DC_DEVICE */

/* CSR0 Bus Mode Register */

#define CSR0_TAP_NO	0x00000000	/* no xmit auto polling */
#define CSR0_TAP_200	0x00020000	/* xmit poll every 200 usecs */
#define CSR0_TAP_800	0x00040000	/* xmit poll every 800 usecs */
#define CSR0_TAP_1600	0x00060000	/* xmit poll every 1.6 millsecs */
#define CSR0_CAL_NO	0x00000000	/* cache address alignment not used */
#define CSR0_CAL_08	0x00004000	/* 08 longword boundary aligned */
#define CSR0_CAL_16	0x00008000	/* 16 longword boundary aligned */
#define CSR0_CAL_32	0x0000c000	/* 32 longword boundary aligned */
#define CSR0_DAS	0x00010000	/* Diagnostic Address Space */
#define CSR0_BLE	0x00000080	/* Big/little endian */
#define CSR0_BAR	0x00000002	/* Bus arbitration */
#define CSR0_SWR	0x00000001	/* software reset */

#define CSR0_PBL_MSK	0x00003F00	/* Dma burst length mask */
#define CSR0_PBL_VAL(x)	(((x) << 8) & CSR0_PBL_MSK)

#define CSR0_DSL_MSK	0x0000007C	/* Descriptor skip length */
#define CSR0_DSL_VAL(x) (((x) << 2) & CSR0_DSL_MSK)

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
#define CSR5_LNF	0x00001000	/* link fail */

/*RCP - Added bit description for the DEC21140*/
#define CSR5_GPTE	0x00000800	/* General Purpose Timer Expire */

#define CSR5_FD		0x00000800	/* Full-duplex short frame rcvd */
#define CSR5_AT		0x00000400	/* AUI / Ten base T Pin */
#define CSR5_RWT	0x00000200	/* rcv watchdog time-out */
#define CSR5_RPS	0x00000100	/* rcv process stopped */
#define CSR5_RU		0x00000080	/* rcv buffer unavailable */
#define CSR5_RI		0x00000040	/* rcv interrupt */
#define CSR5_UNF	0x00000020	/* xmit underflow */
#define CSR5_TJT	0x00000008	/* xmit jabber time-out */
#define CSR5_TU		0x00000004	/* xmit buffer unavailable */
#define CSR5_TPS	0x00000002	/* Xmit Process stopped */
#define CSR5_TI		0x00000001	/* xmit interrupt */

#define CSR5_RPS_MSK	0x000E0000	/* Rcv process state mask */
#define CSR5_TPS_MSK	0x00700000	/* Xmit process state mask */
#define CSR5_ERR_MSK	0x03800000	/* error mask */

/* CSR6 Operation Mode Register */

/*RCP - Added bit description for CSR6 for the DEC21140*/
#define	CSR6_BIT25	0x02000000	/* BIT MUST ALWAYS BE 1 */
#define	CSR6_SCR	0x01000000	/* scrambler mode enable */
#define	CSR6_PCS	0x00800000	/* PCS mode enable */
#define	CSR6_TTM	0x00400000	/* transmit thresold mode */
#define	CSR6_SF		0x00200000	/* store forward mode */
#define	CSR6_HBD	0x00080000	/* heartbeat disabled */
#define	CSR6_PS		0x00040000	/* MII/SYM port selected */

/* Common Bit States for CSR6 */
#define	CSR6_CAE	0x00020000	/* capture effect enable */
#define	CSR6_BP		0x00010000	/* back pressure */
#define	CSR6_THR_072	0x00000000	/* threshold bytes 72 */
#define	CSR6_THR_096	0x00004000	/* threshold bytes 96 */
#define	CSR6_THR_128	0x00008000	/* threshold bytes 128 */
#define	CSR6_THR_160	0x0000C000	/* threshold bytes 160 */
#define	CSR6_ST		0x00002000	/* start/stop Xmit command */
#define	CSR6_FC		0x00001000	/* Force collision mode */
#define	CSR6_OM_NOR	0x00000000	/* normal mode */
#define	CSR6_OM_ILB	0x00000400	/* internal loopback mode */
#define	CSR6_OM_ELB	0x00000800	/* external loopback mode */
#define	CSR6_FD		0x00000200	/* Full Duplex mode */
#define	CSR6_FKD	0x00000100	/* Flaky oscillator disable */
#define	CSR6_PM		0x00000080	/* Pass all multicast */
#define	CSR6_PR		0x00000040	/* promiscuous mode */
#define	CSR6_SB		0x00000020	/* Start/Stop Back off counter */
#define	CSR6_IF		0x00000010	/* inverse filtering */
#define	CSR6_PB		0x00000008	/* pass bad frames */
#define	CSR6_HO		0x00000004	/* hash only filtering mode */
#define	CSR6_SR		0x00000002	/* start/stop receive command */
#define	CSR6_HP		0x00000001	/* hash/perfect recv filtering mode */

/* CSR7 Interrupt Mask register */

#define	CSR7_NIM	0x00010000	/* normal interrupt mask */
#define	CSR7_AIM	0x00008000	/* abnormal interrupt mask */
#define	CSR7_SEM	0x00002000	/* system error mask */
#define	CSR7_LFM	0x00001000	/* link fail mask */
#define	CSR7_FDM	0x00000800	/* full duplex mask */
#define	CSR7_ATM	0x00000400	/* aui/tp switch mask */
#define	CSR7_RWM	0x00000200	/* rcv watchdog time-out mask */
#define	CSR7_RSM	0x00000100	/* rcv stopped mask */
#define	CSR7_RUM	0x00000080	/* rcv buff unavailable mask */
#define	CSR7_RIM	0x00000040	/* rcv  interrupt mask */
#define	CSR7_UNM	0x00000020	/* underflow interrupt mask */ 
#define	CSR7_TJM	0x00000008	/* xmit jabber timer out mask */ 
#define	CSR7_TUM	0x00000004	/* xmit buff unavailable mask */
#define	CSR7_TSM	0x00000002	/* xmission stopped mask */
#define	CSR7_TIM	0x00000001	/* xmit interrupt mask */


/* CSR8 Missing Frame Counter */

#define	CSR8_MFO	0x00010000	/* missed frame overflow */
#define CSR8_MFC_MSK	0x0000FFFF	/* Missed frame counter mask */

/* CSR9 Ethernet Address ROM Register */

#define	CSR9_DNV	0x80000000	/* Data not valid */
#define CSR9_DAT_MSK	0x000000FF	/* data mask */
#define ENET_ROM_SIZE	8		/* ethernet rom register size */


/* RCP - CSR9 Serial Address ROM and MII Management Register for the DEC21140 */

#define	CSR9_MDI_FLAG		0x00080000	/* MII Management Data_In */
#define	CSR9_MMIMOD_WRITE0	0x00000000	/* MII Management Write 0 */
#define	CSR9_MMIMOD_WRITE1	0x00020000	/* MII Management Write 1 */
#define	CSR9_MMIMOD_READ	0x00040000	/* MII Management Read */
#define	CSR9_CLOCK_FLAG		0x00010000	/* MII Management Clock */
#define	CSR9_SRO_FLAG		0x00004000	/* Serial ROM Read Operation */
#define	CSR9_SWO_FLAG		0x00002000	/* Serial ROM Write Operation */
#define	CSR9_SSR_FLAG		0x00000800	/* Select Serial ROM */
#define	CSR9_SDO_FLAG		0x00000008	/* Serial ROM Data_Out */
#define	CSR9_SDI_FLAG		0x00000004	/* Serial ROM Data_In */
#define	CSR9_SDLK_FLAG		0x00000002	/* Serial ROM Serial Clock */
#define	CSR9_SDS_FLAG		0x00000001	/* Serial ROM Chip Select */

/* CSR10 Reserved */


/* CSR11 Full Duplex Register */

#define	CSR11_FDACV_MSK	0x0000FFFF	/* full duplex auto config mask */

/*RCP - CSR11 General-Purpose Timer Register for the DEC21140 */

#define	CSR11_CON_MODE		0x00010000	/* GPT Continuous Mode */
#define	CSR11_VALUE_MASK	0x0000FFFF	/* GPT Timer Value Mask */

/* CSR12 SIA status Register */

#define	CSR12_DA0	0x00000080	/* Diagnostic bit all One */
#define	CSR12_DAZ	0x00000040	/* Diagnostic bit all zero */
#define	CSR12_DSP      	0x00000020	/* Diagnostic BIST status indicator */
#define	CSR12_DSD	0x00000010	/* Diagnostic Self test done */
#define	CSR12_APS	0x00000008	/* Auto polarity state */
#define	CSR12_LKF	0x00000004	/* link fail status */
#define	CSR12_NCR	0x00000002	/* network connection error */
#define	CSR12_PAUI	0x00000001	/* pin AUI_TP indication */

/*RCP - CSR12 General-Purpose Registerr for the DEC21140 */

#define	CSR12_CNTRL_FLAG	0x00000100   /* Control Flag */
#define	CSR12_DATA_MASK		0x000000FF   /* Mode and Data Mask */

#define INIT_CSR12	CSR12_CNTRL_FLAG | 0x0f	/* Base value for CSR12 */
#define SYM_MODE	0x00000009		/* SYM value for CSR12 */

/* CSR13 SIA connectivity Register */

#define CSR13_OE57	0x00008000	/* Output enable 5 6 7 */
#define CSR13_OE24	0x00004000	/* output enable 2 4 */
#define CSR13_OE13	0x00002000	/* output enable 1 3 */
#define CSR13_IE	0x00001000	/* input enable */	
#define CSR13_SEL_LED	0x00000f00	/* select LED and external driver */
#define CSR13_ASE_APLL	0x00000080	/* ase apll start enable */
#define CSR13_SIM	0x00000040	/* serial iface input multiplexer */
#define CSR13_ENI	0x00000020	/* encoder Input multiplexer */
#define CSR13_EDP_SIA	0x00000010	/* pll external input enable */
#define CSR13_AUI_TP	0x00000008	/* AUI - 10BASE-T or AUI */
#define CSR13_CAC_CSR	0x00000004	/* auto config register */
#define	CSR13_PS	0x00000002	/* pin AUI_TP select */	
#define CSR13_SRL_SIA	0x00000001	/* srl sia Reset */

/* CSR14 SIA xmit rcv Register */

#define CSR14_SPP	0x00004000	/* set polarity plus */
#define CSR14_APE	0x00002000	/* auto polarity enable */
#define CSR14_LTE	0x00001000	/* link test enable */
#define CSR14_SQE	0x00000800	/* signal quality generate enable */
#define CSR14_CLD	0x00000400	/* collision detect enable */
#define CSR14_CSQ	0x00000200	/* collision squelch enable */
#define CSR14_RSQ	0x00000100	/* receive squelch enable */
#define CSR14_CPEN_NC	0x00000030	/* no compensation */
#define CSR14_CPEN_HP	0x00000020	/* high power mode */
#define CSR14_CPEN_DM	0x00000010	/* disable mode */
#define CSR14_LSE	0x00000008	/* link pulse send enable */
#define CSR14_DREN	0x00000004	/* driver enable */
#define CSR14_LBK	0x00000002	/* loopback enable */
#define CSR14_ECEN	0x00000001	/* encoder enable */

/* CSR15 SIA general register */

#define CSR15_JCK	0x00000004	/* jabber clock */
#define CSR15_HUJ	0x00000002	/* host unjab */
#define CSR15_JBD	0x00000001	/* jabber disable */

/*RCP - CSR15 Watchdog Timer Register for the DEC21140 */

#define	CSR15_RWR_FLAG	0x00000020   /* Receive Watchdog Release */
#define	CSR15_RWD_FLAG	0x00000010   /* Receive Watchdog Disable */

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
#define RDESC0_DB		0x00000004	/* dribbling bit 
#define RDESC0_CE		0x00000002	/* crc error */
#define RDESC0_OF		0x00000001	/* Over flow */

#define RDESC0_FL_MSK		0x7FFF0000	/* Frame length mask */
#define RDESC0_FL_GET(x)	(((x) & RDESC0_FL_MSK) >> 16)
#define RDESC0_FL_PUT(x)	(((x) << 16) & RDESC0_FL_MSK)

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
#define TDESC1_FTO		0x00400000	/* filtering type */

#define TDESC1_TBS2_MSK		0x003FF800	/* TBS2 buffer 2 size */
#define TDESC1_TBS1_MSK		0x000007FF	/* TBS2 buffer 1 size */

#define TDESC1_TBS1_PUT(x)	((x) & TDESC1_TBS1_MSK)	/* multiple of 4 */
#define TDESC1_TBS2_PUT(x)	(((x) << 11) & TDESC1_TBS2_MSK)

#define FLTR_FRM_SIZE		0xC0		/* filter frm size 192 bytes */
#define FLTR_FRM_SIZE_ULONGS	(FLTR_FRM_SIZE / sizeof (ULONG))
#define FLTR_FRM_ADRS_NUM	0x10		/* filter frm holds 16 addrs */
#define FLTR_FRM_ADRS_SIZE	0x06		/* size of each phys addrs */
#define FLTR_FRM_DEF_ADRS	0xFFFFFFFF	/* enet broad cast address */


#ifdef __cplusplus
}
#endif

#endif /* __INCif_dcFasth */
