/* iOlicomEnd.h - END-style Intel Olicom PCMCIA Ethernet interface header */

/* Copyright 1997-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,20sep01,dat  Removing ANSI errors for diab compiler
01f,03aug00,jpd  corrected length of drive name string (SPR #32276).
01e,11jun00,ham  removed reference to etherLib.
01d,02sep98,jpd  renamed struct type name to prevent conflict. Improved
		 data copying macros efficiency. Add big-endian support.
01c,02apr98,jpd  made conditionally-compilable version for Brutus and PID.
01b,16feb98.jpd  modified to work on Brutus, but not yet on both Brutus and PID.
01a,16dec97,rlp  copied from if_oli.h
*/

#ifndef __INCiOlicomEndh
#define __INCiOlicomEndh

#ifdef __cplusplus
extern "C" {
#endif

#include "end.h"
#include "netBufLib.h"
#include "netinet/if_ether.h"

/* Define Constants */

#define PC_SOCKET_A             0x00
#define PC_SOCKET_B             0x40

#define ATTRIBUTE_MEMORY_SLOT   0x01000


#ifndef TARGET_HAS_NO_VADEM
#define PC_INDEX        ((volatile char *)(ctrlBase + 0x00))
#define PC_DATA_READ    ((volatile int *) (ctrlBase + 0x01))
#define PC_DATA_WRITE   ((volatile char *)(ctrlBase + 0x01))

#define MAPPED_MEMORY_BASE      0x10000         /* 64K */

#define ATTRIB_MEMORY_A         MAPPED_MEMORY_BASE
#define ATTRIB_MEMORY_B         ATTRIB_MEMORY_A + ATTRIBUTE_MEMORY_SLOT
#endif /* TARGET_HAS_NO_VADEM */

/*
 * Define to have complete control over the PCMCIA system.
 * These definitions are Access macro, I/O Registers, Control Registers,
 * PCMCIA Structures and Constants.
 */

/* Macro definition for PCMCIA Access */

#ifndef I82595_REGS_CONTIGUOUS
#define PCMCIA_CALC_ADDR(x)     (((((UINT)(x)) & ~0x01) << 1) | \
				  (((UINT)(x)) & 0x01))

#define PCMCIA_READ_RAW(x, y)   (*(volatile UINT *) (PCMCIA_CALC_ADDR(x) | (y)))

#define PCMCIA_WRITE_RAW(x, y, z)       (*(volatile UINT8 *)    \
                                        (PCMCIA_CALC_ADDR(x) | (y)) = (z))

#define PCMCIA_IO_WRITE(x, y)   ((*(volatile UINT8 *)(x)) = (y))

#define PCMCIA_IO_READ(x)       (((((UINT)(x)) & 0x01) == 0) ?             \
                                (*(volatile UINT *)(x)) & OCT_MSK: \
                                (*(volatile UINT *)(x)) >> 24)

#define PCMCIA_ATTR_READ(x)	(((((UINT)(x)) & ODD_MSK) == 0) ?  \
                PCMCIA_READ_RAW(x, (int)pDrvCtrl->pcmcia.memBaseA) & OCT_MSK :\
                PCMCIA_READ_RAW(x, (int)pDrvCtrl->pcmcia.memBaseA) >> 24)

#define PCMCIA_ATTR_WRITE(x, y)  PCMCIA_WRITE_RAW((x), (int)       \
                                        pDrvCtrl->pcmcia.memBaseA, (y))
/* On PID, two apparently consecutive registers cannot be read at once */

#define READ_R14_R15(val) \
    { \
    val =  PCMCIA_IO_READ (pDrvCtrl->pcmcia.oliAddr + I595_R14); \
    val |= (PCMCIA_IO_READ (pDrvCtrl->pcmcia.oliAddr + I595_R15) << 8); \
    }
#if (_BYTE_ORDER == _BIG_ENDIAN)
#define READ_PTR_R14_R15(ptr, pR14) \
    { \
    *(ptr)++ = (0xFF & *((volatile UINT32 *)(pR14))); \
    *(ptr)++ = (*((UINT32 *)((pR14) + 1)) >> 24); \
    }
#else /* (_BYTE_ORDER == _BIG_ENDIAN) */
#define READ_PTR_R14_R15(ptr, pR14) \
    { \
    *(ptr)++ = *((volatile UINT8 *)(pR14)); \
    *(ptr)++ = (*((UINT32 *)((pR14) + 1)) >> 24); \
    }
#endif	/* _BYTE_ORDER == _BIG_ENDIAN */
#define WRITE_R14_R15(val) \
    { \
    PCMCIA_IO_WRITE (pDrvCtrl->pcmcia.oliAddr + I595_R14, (val)); \
    PCMCIA_IO_WRITE (pDrvCtrl->pcmcia.oliAddr + I595_R15, ((val) >> 8)); \
    }
#define WRITE_PTR_R14_R15(ptr, pR14) \
    { \
    *(pR14)	    =  *(ptr)++; \
    *((pR14) + 1) =  *(ptr)++; \
    }
#else /* I82595_REGS_CONTIGUOUS */
#define PCMCIA_CALC_ADDR(x)	(x)
#define PCMCIA_IO_WRITE(x, y)	((*(volatile UINT8 *) (x)) = (y))
#define PCMCIA_IO_WRITE16(x, y)	((*(volatile UINT16 *)(x)) = (y))
#define PCMCIA_IO_READ(x)	(* (volatile UINT8 *) (x))
#define PCMCIA_IO_READ16(x)	(* (volatile UINT16 *)(x))
#define PCMCIA_ATTR_READ(x)	(* (volatile UINT8 *) (x))
#define PCMCIA_ATTR_WRITE(x, y)	((*(volatile UINT8 *) (x)) = (y))

/* On Brutus, R14 and R15 can be read with one 16 bit operation */

#define READ_R14_R15(val) \
    { \
    val = (PCMCIA_IO_READ16 (pDrvCtrl->pcmcia.oliAddr + I595_R14)); \
    }
#define READ_PTR_R14_R15(ptr, pR14) \
    { \
    *((UINT16 *)(ptr))++ = *((volatile UINT16 *)(pR14));\
    }
#define WRITE_R14_R15(val) \
    PCMCIA_IO_WRITE16 (pDrvCtrl->pcmcia.oliAddr + I595_R14, (val));

#define WRITE_PTR_R14_R15(ptr, pr14) \
	*(volatile UINT16 *)(pR14) = *((UINT16 *)(ptr))++;
#endif /* !I82595_REGS_CONTIGUOUS */

/* General Setup Registers */

#define PCIDREV		0x00	/* Identification & Revision */
#define PCIFSTATUS	0x01	/* Interface Status */
#define PCPWRRSTCTL	0x02	/* Power and RESETDRV Control */
#define PCCSTATCHNG	0x04	/* Card Status Change */
#define PCADDWINEN	0x06	/* Address Window Enable */
#define PCCDGCR		0x16	/* Card Detect and General Control Register */
#define PCGLBCTL	0x1E	/* Global Control Register */

#define PCINTGCTL	0x03	/* Interrupt & General Control */
#define PCSTATCHNGINT	0x05	/* Card Status Change Interrupt Configuration */

/* Definitions for fields in general setup registers */

/* Interface Status Register */

#define GSR_CD		0x0C	/* Card detect - Complement of CD[2:1] pins */
#define GSR_RDY		0x20	/* Card is ready */

/* Power and RESETDRV control register */

#define GSR_CPE		0x10	/* PC card power enable */
#define	GSR_PSE		0x20	/* Auto power switch enable */
#define	GSR_OE		0x80	/* Output enable */

/* Interrupt & general control register */

#define GSR_DRT		0x40	/* Deactivates reset signal to the PC card */

/* Card Status Change Register */

#define GSR_CDC		0x08	/* Card detect change */

/* Card status change interrupt configuration register */

#define GSR_IRQ4	0x40	/* IRQ4 enable */
#define GSR_IRQ3	0x30	/* IRQ3 enable */
#define GSR_CDE		0x08	/* Card detect enable */

/* Address window enable register */

#define GSR_WD0		0x01	/* Enable window 0 */

/* I/O Registers */

#define PCIOCTL	0x7	/* I/O Control */
#define PCIOA0STARTL	0x08	/* I/O Addr 0 Start Low */
#define PCIOA0STARTH	0x09	/* I/O Addr 0 Start High */
#define PCIOA0STOPL	0x0A	/* I/O Addr 0 Stop Low */
#define PCIOA0STOPH	0x0B	/* I/O Addr 0 Stop High */
#define PCIOA1STARTL	0x0C	/* I/O Addr 1 Start Low */
#define PCIOA1STARTH	0x0D	/* I/O Addr 1 Start High */
#define PCIOA1STOPL	0x0E	/* I/O Addr 1 Stop Low */
#define PCIOA1STOPH	0x0F	/* I/O Addr 1 Stop High */

#define PCSYSA0MSTARTL	0x10	/* System Memory Addr 0 Mapping Start Low */
#define PCSYSA0MSTARTH	0x11	/* System Memory Addr 0 Mapping Start High */
#define PCSYSA0MSTOPL	0x12	/* System Memory Addr 0 Mapping Stop Low */
#define PCSYSA0MSTOPH	0x13	/* System Memory Addr 0 Mapping Stop High */
#define PCCMEMA0OFFL	0x14	/* Card Memory Offset Addr 0 Low */
#define PCCMEMA0OFFH	0x15	/* Card Memory Offset Addr 0 High */

#define PCSYSA1MSTARTL	0x18	/* System Memory Addr 1 Mapping Start Low */
#define PCSYSA1MSTARTH	0x19	/* System Memory Addr 1 Mapping Start High */
#define PCSYSA1MSTOPL	0x1A	/* System Memory Addr 1 Mapping Stop Low */
#define PCSYSA1MSTOPH	0x1B	/* System Memory Addr 1 Mapping Stop High */
#define PCCMEMA1OFFL	0x1C	/* Card Memory Offset Addr 1 Low */
#define PCCMEMA1OFFH	0x1D	/* Card Memory Offset Addr 1 High */

#define PCSYSA2MSTARTL	0x20	/* System Memory Addr 2 Mapping Start Low */
#define PCSYSA2MSTARTH	0x21	/* System Memory Addr 2 Mapping Start High */
#define PCSYSA2MSTOPL	0x22	/* System Memory Addr 2 Mapping Stop Low */
#define PCSYSA2MSTOPH	0x23	/* System Memory Addr 2 Mapping Stop High */
#define PCCMEMA2OFFL	0x24	/* Card Memory Offset Addr 2 Low */
#define PCCMEMA2OFFH	0x25	/* Card Memory Offset Addr 2 High */

#define PCSYSA3MSTARTL	0x28	/* System Memory Addr 3 Mapping Start Low */
#define PCSYSA3MSTARTH	0x29	/* System Memory Addr 3 Mapping Start High */
#define PCSYSA3MSTOPL	0x2A	/* System Memory Addr 3 Mapping Stop Low */
#define PCSYSA3MSTOPH	0x2B	/* System Memory Addr 3 Mapping Stop High */
#define PCCMEMA3OFFL	0x2C	/* Card Memory Offset Addr 3 Low */
#define PCCMEMA3OFFH	0x2D	/* Card Memory Offset Addr 3 High */

#define PCSYSA4MSTARTL	0x30	/* System Memory Addr 4 Mapping Start Low */
#define PCSYSA4MSTARTH	0x31	/* System Memory Addr 4 Mapping Start High */
#define PCSYSA4MSTOPL	0x32	/* System Memory Addr 4 Mapping Stop Low */
#define PCSYSA4MSTOPH	0x33	/* System Memory Addr 4 Mapping Stop High */
#define PCCMEMA4OFFL	0x34	/* Card Memory Offset Addr 4 Low */
#define PCCMEMA4OFFH	0x35	/* Card Memory Offset Addr 4 High */

/* Unique Registers */

#define PCCONTROL	0x38	/* Control */
#define PCACTTIM	0x39	/* Activity Timer */
#define PCMISC		0x3A	/* Miscellaneous */
#define PCGPIOCON	0x3B	/* GPIO Configuration */
#define PCPROGCS	0x3D	/* Programmable Chip Select */
#define PCPROGCSCON	0x3E	/* Programmable Chip Select
				 * Configuration Register */
#define PCATA		0x3F	/* ATA */

/* Define PCMCIA structures */

typedef enum
    {
    COMMON,	/* Common memory */
    ATTRIBUTE	/* Attribute memory */
    }
    MEMTYPE;

typedef struct PCMCIASocket
{
    UINT	pcs_attrbase;
    UINT	pcs_attrrange;
    UINT	pcs_iobase;
    UINT	pcs_iorange;
} PCMCIASOCKET;

/* PCMCIA Control Structure */

typedef struct pcmcia_ctrl
    {
    char *		ioBaseA;	/* I/O space base address for Card A */
    char *		attrBaseA;	/* Attribute space base address Card A*/
    char *		memBaseA;	/* Memory space base address Card A */
    char *		ioBaseB;	/* I/O space base address for Card B */
    char *		attrBaseB;	/* Attribute space base address Card B*/
    char *		memBaseB;	/* Memory space base address Card B */
    char *		ctrlBase;	/* Base of Vadem PCMCIA controller */
    char *		oliAddr;	/* address of board */
    char *		oliAttribMem;	/* Address of attribute memory */
    int                 intLevelA;      /* Interrupt level for Card A */
    int                 intVectA;       /* Interrupt vector for Card A */
    int                 intLevelB;      /* Interrupt level for Card B */
    int                 intVectB;       /* Interrupt vector for Card B */
    BOOL                cardPresent;    /* TRUE if a card is in the slot */
    UINT		socket;         /* Socket holding card, A or B */
    PCMCIASOCKET        theSocket;      /* PCMCIA Socket descriptor */
    } OLI_PCMCIA_CTRL;

/* Trasmit Buffer Descriptor */

typedef struct tx_bd
    {
    UINT16              statusMode;     /* status and control */
    UINT16              dataLength;     /* length of data buffer in bytes */
    char *		dataPointer;    /* points to memory data buffer */
    } TX_BD;

/* Receive Buffer Descriptor */

typedef struct rx_bd
    {
    UINT16              statusMode;     /* status and control */
    UINT16              dataLength;     /* length of data buffer in bytes */
    char *		dataPointer;    /* points to memory data buffer */
    } RX_BD;

/* Olicom Card Configuration registers */

#define CARDCONFREG0	0x3F8
#define CARDCONFREG1	0x3FA

/* Definitions for fields in Olicom card configuration registers */

/* Register 1 */

#define CREG1_INT_IO	0x02	/* I/O Interrupt */

/* There are four tuples that we need to read from the card */

#define CARD_FUNCID	0x21	/* Should identify the card as
				   an Network Adaptor */
#define CARD_FUNCE	0x22	/* Function extension tuple detailing
				   the Ethernet hardware address */
#define	CARD_CONFIG	0x1A	/* Describes card configuration registers */
#define CARD_CFT_ENTRY	0x1B	/* There are several different copies of
				   this one tuple, which describe the
				   different I/O addresses accepted by the
				   Intel 82595 Controller */

/*
 * Register layout of the Intel 82595TX chip
 *
 * Those ever imaginative people at Intel have no other
 * names for the registers on their chip than r1, r2
 * etc..
 *
 */

#ifndef I82595_REGS_CONTIGUOUS
/*
 * To try and optimise I/O to the chip, this structure represents
 * the *real* appearance of the chip in the PID7T NISA I/O space;
 * the weird wiring arrangement described in pid.h means that,
 * although the chip has a sequential collection of registers, they
 * appear to be grouped as two-register pairs, aligned on a word
 * boundary
 */
#define I595_R0		0x00
#define I595_R1		0x01
#define I595_R2		0x04
#define I595_R3		0x05
#define I595_R4		0x08
#define I595_R5		0x09
#define I595_R6		0x0C
#define I595_R7		0x0D
#define I595_R8		0x10
#define I595_R9		0x11
#define I595_R10	0x14
#define I595_R11	0x15
#define I595_R12	0x18
#define I595_R13	0x19
#define I595_R14	0x1C
#define I595_R15	0x1D
#else
/* Normal layout */
#define I595_R0		0x00
#define I595_R1		0x01
#define I595_R2		0x02
#define I595_R3		0x03
#define I595_R4		0x04
#define I595_R5		0x05
#define I595_R6		0x06
#define I595_R7		0x07
#define I595_R8		0x08
#define I595_R9		0x09
#define I595_R10	0x0A
#define I595_R11	0x0B
#define I595_R12	0x0C
#define I595_R13	0x0D
#define I595_R14	0x0E
#define I595_R15	0x0F
#endif /* !I82595_REGS_CONTIGUOUS */

/*
 * Register 0 is special, as it's the same in all banks
 * Access Macro to Register 0.
 */

#define SELECTBANK(c, b)(PCMCIA_IO_WRITE((c) + I595_R0, (b) << 6))
#define RUNCMD(c, o)	(PCMCIA_IO_WRITE((c) + I595_R0, \
                                                 ((o) & OPCODE_MASK)))
#define ABORTCMD(c)	(PCMCIA_IO_WRITE((c) + I595_R0, (1 << 5)))

/* 82595TX Command Set for register 0 */

#define CMD_MC_SETUP		0x03
#define CMD_TRANSMIT		0x04
#define CMD_TDR			0x05
#define CMD_DUMP		0x06
#define CMD_DIAGNOSE		0x07
#define CMD_RCV_ENABLE		0x08
#define CMD_RCV_DISABLE		0x0A
#define CMD_RCV_STOP		0x0B
#define CMD_ABORT		0x0D
#define CMD_RESET		0x0E
#define CMD_XMIT_RAW		0x14
#define CMD_CONT_XMIT_LIST	0x15
#define CMD_SET_TRISTATE	0x16
#define CMD_RESET_TRISTATE	0x17
#define CMD_POWER_DOWN		0x18
#define CMD_RESUME_XMIT_LIST	0x1C
#define CMD_SEL_RESET		0x1E

#define OPCODE_MASK		0x1F

/* 82595TX result codes for register 0 */

#define RESULT_INIT_DONE	0x0E

/* Definitions for fields and bits in the OLI_DEVICE */

/* I/O Bank 0 */

/* Reg 1 Interrupt/Status Register */

#define	BNK0_RX_SIT	0x01	/* RX stop interrupt */
#define	BNK0_RX_IT	0x02	/* RX interrupt */
#define	BNK0_TX_IT	0x04	/* TX interrupt */
#define	BNK0_EXE_IT	0x08	/* Execution interrupt */
#define	BNK0_EXE_IDL	0x00	/* Execution state idle */
#define	BNK0_EXE_BSY	0x20	/* Execution state busy */
#define	BNK0_EXE_ART	0x30	/* Execution state aborting */
#define	BNK0_RCV_DIS	0x00	/* Receive state disable */
#define	BNK0_RCV_RDY	0x40	/* Receive state ready */
#define	BNK0_RCV_ACT	0x80	/* Receive state active */
#define	BNK0_RCV_STP	0xC0	/* Receive state stopping */

#define BNK0_ITS_MSK	0x0F	/* All interrupts mask */
#define BNK0_EXE_MSK	0x30	/* Execution state mask */
#define BNK0_RCV_MSK	0xC0	/* Receive state mask */

#define BNK0_EXE_STE(r)	((r) & BNK0_EXE_MSK)
#define BNK0_RCV_STE(r)	((r) & BNK0_RCV_MSK)

/* Reg 3 Interrupt Mask Register */

#define BNK0_RX_SMK	0x01	/* RX stop interrupt mask */
#define	BNK0_RX_MK	0x02	/* RX interrupt mask */
#define	BNK0_TX_MK	0x04	/* TX interrupt mask */
#define	BNK0_EXE_MK	0x08	/* Execution interrupt mask */
#define BNK0_HAR_SLT	0x10	/* 32Bit IO port/host address select */
#define	BNK0_CBR_SLT	0x20	/* Current/Base register select */

/* I/O Bank 1 */

/* Reg 1 Config ISA Register/Tri-state irq lines */

#define	BNK1_ISA_HBW	0x02	/* Host bus width (ISA only) */
#define BNK1_ISA_ART	0x40	/* Alternate ready timing (ISA only) */
#define BNK1_TST_EN	0x80	/* Tri-state interrupt lines */

/* I/O Bank 2 */

/* Reg 1 Control the program features of the 82595TX */

#define	BNK2_TCP_DIS	0x01	/* Transmit concurrent processing disable */
#define	BNK2_PCM_ISA	0x10	/* PCMCIA/ISA Select */
#define	BNK2_TCH_END	0x20	/* Transmit chaining int mode */
#define	BNK2_TCH_ERR	0x40	/* Transmit chaining error stop */
#define	BNK2_BAD_DSC	0x80	/* Discard bad frames */

/* Reg 2 Control the program features of the 82595TX */

#define	BNK2_PCS_EN	0x01	/* Promiscous mode enable */
#define	BNK2_BRO_DIS	0x02	/* Disable broadcast */
#define	BNK2_RX_CRC	0x04	/* RX CRC in memory */
#define	BNK2_LEN_DIS	0x08	/* Length disable */
#define	BNK2_SAI_EN	0x10	/* enable source address insertion */
#define	BNK2_MIA_EN	0x20	/* Multi individual address enable */
#define	BNK2_LP_DIS	0x00	/* Loopback disable */
#define	BNK2_LP_INT	0x40	/* Internal loopback */
#define	BNK2_LP_EXT	0x80	/* External loopback */

/* Reg 3 Control the program features of the 82595TX */

#define BNK2_LI_DIS	0x01	/* Link integrity disable */
#define BNK2_PC_DIS	0x02	/* disable polarity correction */
#define BNK2_TPE_AUI	0x04	/* TPE/AUI */
#define BNK2_JB_DIS	0x08	/* Jabber disable */
#define BNK2_AP_EN	0x10	/* APORT */
#define BNK2_BNC_TPE	0x20	/* BNC/TPE */
#define	BNK2_TST_EN	0xC0	/* Production testing mode */


/* RCV status field*/

#define RCV_EOF		0x08	/* Reception of frame completed */
#define RCV_BOF		0x10	/* Begining of frame bit */
#define RCV_RXCPY	0x20	/* RCV copy threshold */
#define RCV_RECLM	0x40	/* RCV-DMA reclaimes location */
#define	RCV_UNDRN	0x80	/* RCV data underrun */

#define RCV_COL_DCT	0x0001	/* Collision detected */
#define RCV_IA_MCH	0x0002	/* Individual address match */
#define RCV_SRT_FRM	0x0080	/* Received frame shorter than 64Bytes */
#define RCV_OVR_RUN	0x0100	/* OVERRUN */
#define RCV_ALG_ERR	0x0400	/* Alignment error */
#define RCV_CRC_ERR	0x0800	/* CRC error */
#define RCV_LEN_ERR	0x1000	/* Length error */
#define RCV_OK		0x2000	/* Frame received OK */
#define RCV_TYP_LEN	0x8000	/* Type field */

/* XMT status field*/

#define XMT_COL_MAX	0x0020	/* Failed due to max collisions */
#define	XMT_HRT_BET	0x0040	/* Collision detect test passed after prev Tx */
#define	XMT_LN_DEF	0x0080	/* Tx deferred due to link activity */
#define	XMT_UND_RUN	0x0100	/* Underrun */
#define	XMT_LST_CRS	0x0400	/* Carrier sense lost */
#define	XMT_LT_COL	0x0800	/* Later collision */
#define	XMT_OK		0x2000	/* Transmit OK */
#define	XMT_COLL	0x8000	/* Collision on the last transmit */

#define	XMT_COL_MSK	0x000F	/* number collisions mask */
#define XMT_CHAIN	0x8000	/* Flag indicates a XMT chaining (16Bit) */
#define XMT_CHAIN_OCT	0x80	/* Flag indicates a XMT chaining (8Bit) */

#define TOTAL_RAM_SIZE	(64 * 1024)
#define RAM_TX_BASE	(0)
#define RAM_TX_SIZE	(32 * 1024)
#define RAM_TX_LIMIT	(RAM_TX_BASE + (RAM_TX_SIZE - 1))

#define RAM_RX_SIZE	(TOTAL_RAM_SIZE - RAM_TX_SIZE)
#define RAM_RX_BASE	(RAM_TX_SIZE)
#define RAM_RX_LIMIT	(RAM_RX_BASE + (RAM_RX_SIZE - 1))


/*
 * Macro for converting from packet byte count to length in Tx ring
 * buffer which needs to include 8 bytes of header and half-word
 * alignment
 */

#define COUNT_TO_LEN(x)	((x) + ((((x) & 0x01) == 0) ? 8 : 9))

/* Define Ethernet Network Driver parameters */

#define MAX_UNITS       1       /* max Olicom units */
#define DRV_NAME        "oli"
#define DRV_NAME_LEN    4	/* including null terminator */
#define EADDR_LEN       6
#define OLI_BUFSIZ      (ETHERMTU + SIZEOF_ETHERHEADER + EADDR_LEN)
#define OLI_SPEED       10000000

#define TX_BD_MIN       2       /* minimum number of Tx buffer descriptors */
#define TX_BD_MAX       0x14    /* maximum number of Tx buffer descriptors */
#define RX_BD_MIN       2       /* minimum number of Rx buffer descriptors */
#define TX_BD_DEFAULT   0x10    /* default number of Tx buffer descriptors */
#define RX_BD_DEFAULT   0x10    /* default number of Rx buffer descriptors */
#define OLI_MIN_FBUF    9       /* min. size of the first buffer in a frame */
#define OLI_MAX_XMT     0x600   /* max. size of the XMT block mem. structure */
#define NUM_LOAN        0x10    /* number of Rx loaner buffers in pool */

/* Define control structures */

/* typedefs */

typedef struct free_buf
    {
    void *      pClBuf;                 /* pointer cluster buffer */
    } FREE_BUF;

/* The definition of the driver control structure */

typedef struct end_device
    {
    END_OBJ     endObj;                 /* The class we inherit from. */
    int         unit;                   /* unit number */
    long        flags;                  /* Our local flags. */

    OLI_PCMCIA_CTRL pcmcia;             /* PCMCIA control structure */
    int         ivec;                   /* interrupt vector */
    int         ilevel;                 /* interrupt level */

    int         txBdNum;                /* number of transmit BD */
    TX_BD *     txBdBase;               /* transmit BD base address */
    int         txBdNext;               /* next transmit BD to fill */
    int         txBdIndexC;             /* current transmit BD index */

    int         rxBdNum;                /* number of receive BD */
    RX_BD *     rxBdBase;               /* receive BD base address */
    int         rxBdNext;               /* next receive BD to read */

    char *      pShMem;                 /* real ptr to shared memory */
    ULONG       shMemSize;              /* shared memory size */

    UINT8       enetAddr[EADDR_LEN];    /* ethernet address */
    int         offset;

    CACHE_FUNCS cacheFuncs;             /* cache function pointers */
    FREE_BUF    freeBuf[128];           /* Array of free routines. */
    CL_POOL_ID  clPoolId;
    } END_DEVICE;

#define FLTR_FRM_SIZE	0x180	/* filter frm size 64 Multicast address */

/* Definitions for the flags field */

#define OLI_RCV_HANDLING	0x0001
#define OLI_POLLING		0x0002	/* Poll mode */
#define OLI_TX_CLEANING		0x0004
#define OLI_TX_STOP		0x0008

#define OLI_MEMOWN		0x8000	/* memory allocated by driver */

/* state of transmit BD */

#define TX_BD_READY	0x80

/* state of receive BD */

#define RX_BD_EMPTY	0x01

/* definitions of several mask */

#define	OCT_MSK		0xFF	/* 8-bit mask */
#define	ODD_MSK		0x01	/* Odd mask */


#ifdef __cplusplus
}
#endif


#endif /* __INCiOlicomEndh */
