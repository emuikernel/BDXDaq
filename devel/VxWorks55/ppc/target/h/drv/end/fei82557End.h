/* fei82557End.h - Intel 82557 network interface header */

/* Copyright 1990-2001 Wind River Systems, Inc. */
/*

modification history
--------------------
01k,20sep01,dat  Removing ANSI errors for diab compiler
01j,28sep00,jgn  remove physical to virtual translations (SPR 34891)
01i,13sep00,jkf  T3 SPR#33822 (aka T2 SPR's 33792, 33795, and 33796)
01i,11jun00,ham  removed reference to etherLib.
01h,06nov99,dat  Merge from tor3_x.bsp_drv_facility branch
01g,12nov99,mas  added typecasting to non-PPC FEI_XXXX_RD macros (SPR 24474)
01f,21mar99,jkf  Added PPC access routines needed for Yellowknife.
01e,21jul98,cn   Moved here some redefineable macros
01d,01apr98,cn   added vendor and device id definitions
01c,11mar98,cn   checked-in
01b,05mar98,cn   code clean-up after code review
01a,07nov97,cn	 created from ../netif/if_fei.h, version 01c
*/

#ifndef __INCfei82557Endh
#define __INCfei82557Endh

/* includes */

#ifdef __cplusplus
extern "C" {
#endif

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/* defines */

/*
 * redefine the macro below in the bsp if you need to access the device
 * registers/descriptors in a more suitable way.
 */


#if (CPU_FAMILY == PPC)

#ifndef FEI_LONG_WR
#define FEI_LONG_WR(addr, value) (sysOutLong ((ULONG) addr, (ULONG) value))
#endif
 
#ifndef FEI_WORD_WR
#define FEI_WORD_WR(addr, value) (sysOutWord ((ULONG) addr, (USHORT) value))
#endif
 
#ifndef FEI_BYTE_WR
#define FEI_BYTE_WR(addr, value) (sysOutByte ((ULONG) addr, (UCHAR) value))
#endif
 
#ifndef FEI_LONG_RD
#define FEI_LONG_RD(addr, value) (value = sysInLong ((ULONG) addr))
#endif
 
#ifndef FEI_WORD_RD
#define FEI_WORD_RD(addr, value) (value = sysInWord ((ULONG) addr))
#endif
 
#ifndef FEI_BYTE_RD
#define FEI_BYTE_RD(addr, value) (value = sysInByte ((ULONG) addr))
#endif
 
#endif /* CPU_FAMILY == PPC */

#ifndef FEI_LONG_WR
#define FEI_LONG_WR(addr, value)                                        \
    (* (UINT32 *) (addr) = ((UINT32) (FEI_SWAP_LONG (value) & 0xffffffff)))
#endif /* FEI_LONG_WR */
 
#ifndef FEI_WORD_WR
#define FEI_WORD_WR(addr, value)                                        \
    (* (UINT16 *) (addr) = ((UINT16) (FEI_SWAP_WORD (value) & 0x0000ffff)))
#endif /* FEI_WORD_WR */
 
#ifndef FEI_BYTE_WR
#define FEI_BYTE_WR(addr, value)                                        \
    (* ((UINT8 *) (addr)) = ((UINT8) ((value) & 0x000000ff)))
#endif /* FEI_BYTE_WR */
 
#ifndef FEI_LONG_RD
#define FEI_LONG_RD(addr, value)                                        \
    (((value)) = (UINT32) FEI_SWAP_LONG (((*((UINT32 *)(addr))) & \
     0xffffffff)))
#endif /* FEI_LONG_RD */
 
#ifndef FEI_WORD_RD
#define FEI_WORD_RD(addr, value)                                        \
    (((value)) = (UINT16) FEI_SWAP_WORD (((*((UINT16 *)(addr))) & \
     0x0000ffff)))
#endif /* FEI_WORD_RD */
 
#ifndef FEI_BYTE_RD
#define FEI_BYTE_RD(addr, value)                                        \
    (((value)) = (UINT8) (((*((UINT8 *)(addr))) & 0x000000ff)))
#endif /* FEI_BYTE_RD */
 
/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */
 
#ifndef SYS_INT_CONNECT
#define SYS_INT_CONNECT(pDrvCtrl, pFunc, arg, pRet)                         \
    {                                                                       \
    /* SPR 33796/33822 - init SYS_INT_CONNECT's return value" to ERROR */   \
    *pRet = ERROR;                                                          \
                                                                            \
    /* SPR 33792 - remove FEI_VECTOR checking from SYS_INT_CONNECT */       \
    /* SPR 33795 - add feiEndIntConnect NULL checking in SYS_INT_CONNECT */ \
    if (feiEndIntConnect != NULL)                                           \
        {                                                                   \
        *pRet = (feiEndIntConnect) ((VOIDFUNCPTR*)                          \
                             INUM_TO_IVEC (FEI_VECTOR (pDrvCtrl)),          \
                             (pFunc), (int) (arg));                         \
        }                                                                   \
    }
#endif /* SYS_INT_CONNECT */
 
#ifndef SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDrvCtrl, pFunc, arg, pRet)                      \
    {                                                                       \
    /* SPR 33796 - initialize SYS_INT_DISCONNECT's return value to ERROR */ \
    *pRet = ERROR;                                                          \
                                                                            \
    /* SPR 33792 - remove FEI_VECTOR checking from SYS_INT_DISCONNECT */    \
    if (feiEndIntDisconnect != NULL)                                        \
        {                                                                   \
        *pRet = feiEndIntDisconnect ((VOIDFUNCPTR*)                         \
                              INUM_TO_IVEC (FEI_VECTOR (pDrvCtrl)),         \
                              (pFunc));                                     \
        }                                                                   \
    }
#endif /* SYS_INT_DISCONNECT */
 
#ifndef SYS_INT_ENABLE
#define SYS_INT_ENABLE(pDrvCtrl)                                            \
if (FEI_INT_ENABLE (pDrvCtrl))                                              \
    ((*(FUNCPTR) (FEI_INT_ENABLE (pDrvCtrl))) (pDrvCtrl->unit))
#endif /*SYS_INT_ENABLE*/
 
#ifndef SYS_INT_DISABLE
#define SYS_INT_DISABLE(pDrvCtrl)                                           \
if (FEI_INT_DISABLE (pDrvCtrl))                                             \
    ((*(FUNCPTR) (FEI_INT_DISABLE (pDrvCtrl))) (pDrvCtrl->unit))
#endif /*SYS_INT_DISABLE*/
 
#ifndef SYS_INT_ACK
#define SYS_INT_ACK(pDrvCtrl)                                               \
if (FEI_INT_ACK (pDrvCtrl))                                                 \
    ((*(FUNCPTR) (FEI_INT_ACK (pDrvCtrl))) (pDrvCtrl->unit))
#endif /*SYS_INT_ACK*/
 
#ifndef LOCAL_TO_SYS_ADDR
#define LOCAL_TO_SYS_ADDR(unit,localAddr)                                   \
    ((int) pDrvCtrl->board.sysLocalToBus ?                                  \
    (*pDrvCtrl->board.sysLocalToBus) (unit, localAddr) : localAddr)
#endif /* LOCAL_TO_SYS_ADDR */
 
#ifndef SYS_TO_LOCAL_ADDR
#define SYS_TO_LOCAL_ADDR(unit,sysAddr)                                     \
    ((int) pDrvCtrl->board.sysBusToLocal ?                                  \
    (*pDrvCtrl->board.sysBusToLocal)(unit, sysAddr) : sysAddr)
#endif /* SYS_TO_LOCAL_ADDR */
 
/* Intel PRO-100B PCI specific definitions */
 
#define PRO100B_PCI_VENDOR_ID   0x8086  /* PCI vendor ID */
#define PRO100B_PCI_DEVICE_ID   0x1229  /* PCI device ID */

#define FEI82557_DEVICE_ID      0x1229  /* 82557 device ID */ 
 
#define MC_FEI MC_EI  /* temp */

#define DEV_NAME        "fei"
#define DEV_NAME_LEN    4
#define DEF_NUM_CFDS    32                      /* default number of CFDs */
#define DEF_NUM_RFDS    32                      /* default number of RFDs */
#define FEI_100MBS      100000000               /* bits per sec */
#define FEI_10MBS       10000000                /* bits per sec */
#define EADDR_LEN       6                       /* ethernet address length */
#define FEI_ADDR_LEN    EADDR_LEN               /* ethernet address length */
 
#define FEI_ACTION_TMO          0x01    /* action command timeout in seconds */
#define FEI_SCB_TMO             0x01    /* SCB command timeout in seconds */
#define FEI_INIT_TMO            0x01    /* 557 init timeout in seconds */
#define FEI_TX_RESTART_TMO      0x01    /* tx restart watchdog timeout */
#define FEI_TCB_TX_THRESH       0x10    /* Tx threshold value */
 
/* RFD and CFD size */
 
#define RFD_SIZE_HDR            16
#define RFD_SIZE_PKT            (EH_SIZE + ETHERMTU + 2)
#define RFD_SIZE_TRAIL          RFD_SIZE_PREV + RFD_SIZE_SW_NEXT
#define RFD_SIZE_PREV           4
#define RFD_SIZE_SW_NEXT	4
 
#define RFD_SIZE                ROUND_UP ((RFD_SIZE_HDR +               \
                                           RFD_SIZE_PKT +               \
                                           RFD_SIZE_TRAIL), 4)
 
 
#define CFD_SIZE_HDR            8
#define CFD_SIZE_DUMP           4
#define CFD_SIZE_MULTIC         (2 + (6 * N_MCAST))
#define CFD_SIZE_IA             8
#define CFD_SIZE_NOP            0
#define CFD_SIZE_PKT            (EH_SIZE + ETHERMTU + 2)
#define CFD_SIZE_TCB            (8 + CFD_SIZE_PKT)
#define CFD_SIZE_PREV           4
#define CFD_SIZE_SW_NEXT	4
#define CFD_SIZE_ACTION         1
#define CFD_SIZE_TRAIL          (CFD_SIZE_PREV +			\
				 CFD_SIZE_SW_NEXT +			\
				 CFD_SIZE_ACTION)
#define CFD_SIZE_CORE           CFD_SIZE_TCB
 
#define CFD_SIZE                ROUND_UP ((CFD_SIZE_HDR +               \
                                           CFD_SIZE_CORE +              \
                                           CFD_SIZE_TRAIL), 4)
 
/* frame descriptors definitions */
 
#define CFD_STAT_OFFSET         0x00                    /* status word */
#define CFD_COMM_OFFSET         0x02                    /* command word */
#define CFD_NEXT_OFFSET         0x04                    /* next frame */
#define CFD_PREV_OFFSET         (CFD_SIZE - 4)          /* previous frame */
#define CFD_TBD_OFFSET          0x08                    /* buffer descriptor*/
#define CFD_COUNT_OFFSET        0x0c                    /* count field */
#define CFD_NUM_OFFSET          0x0f                    /* buffer num  field */
#define CFD_THRESH_OFFSET       0x0e                    /* threshold field */
#define CFD_ACTION_OFFSET       (CFD_SIZE - 9)          /* action/transmit */
#define CFD_SW_NEXT_OFFSET	(CFD_SIZE - 8)		/* s/w next link */
#define CFD_PKT_OFFSET          (CFD_SIZE_HDR + 8)      /* packet data */
#define CFD_IA_OFFSET           CFD_SIZE_HDR            /* individual address */
#define CFD_MCOUNT_OFFSET       CFD_SIZE_HDR            /* mcount field */
#define CFD_MC_OFFSET           (CFD_SIZE_HDR + 2)      /* mcast list */
 
#define RFD_STAT_OFFSET         0x00                    /* status word */
#define RFD_COMM_OFFSET         0x02                    /* command word */
#define RFD_NEXT_OFFSET         0x04                    /* next frame field */
#define RFD_PREV_OFFSET         (RFD_SIZE - 4)          /* previous frame */
#define RFD_SW_NEXT_OFFSET      (RFD_SIZE - 8)          /* s/w next frame */
#define RFD_RBD_OFFSET          0x08                    /* buffer descriptor */
#define RFD_COUNT_OFFSET        0x0c                    /* count field */
#define RFD_SIZE_OFFSET         0x0e                    /* data size */
#define RFD_PKT_OFFSET          (RFD_SIZE_HDR)          /* packet data*/
 
#define TBD_NOT_USED            0xffffffff            /* use semplified mode */
#define RBD_NOT_USED            0xffffffff            /* use semplified mode */

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
    } FEI_BOARD_INFO;

/* constants needed within this file */

#define FEI_MAX_UNITS	4		/* max units supported */
#define FEI_CFD_DEF	32		/* default number of CFDs */
#define FEI_RFD_DEF	32		/* default number of RFDs */
#define FEI_RFD_LOAN	8		/* number RFDs that can be loaned */
#define EH_SIZE		14  		/* avoid structure padding issues */
#define N_MCAST		12

/* SCB registers offset definitions @@@ */
#define SCB_STATUS      0x00            /* SCB status byte */
#define SCB_CMD         0x02            /* SCB command byte */
#define SCB_POINTER     0x04            /* SCB pointer */
#define SCB_PORT        0x08            /* SCB port register */
#define SCB_FLASH       0x0c            /* SCB flash control register */
#define SCB_EEPROM      0x0e            /* SCB eeprom control register */
#define SCB_MDI         0x10            /* SCB mdi control register */
#define SCB_EARLYRX     0x14            /* SCB early receive */

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

#define SCB_C_M		0x01		/* interrupt mask */
#define SCB_C_SI	0x02		/* software generated interrupt */
#define SCB_C_RNR	0x10		/* RU ran out of resources */
#define SCB_C_CNA	0x20		/* CU left active state (transmit) */
#define SCB_C_FR	0x40		/* frame receive */

/* PORT commands */

#define FEI_PORT_RESET		0x0	/* PORT reset command */
#define FEI_PORT_SELFTEST	0x1	/* PORT selftest command  */
#define FEI_PORT_SELRESET	0x2	/* PORT selective reset command */
#define FEI_PORT_DUMP		0x3	/* PORT dump command */

/* Action Command Descriptions */

/* special TCB-specific command block bit mask definitions */

#define CFD_S_TX_DMA_U	0x1000		/* DMA underrun */
#define TCB_CNT_EOF	0x8000		/* all data kept in TFD */
#define CFD_C_TX_FLEX	0x0008		/* use flexible mode */


/* special Diag-specific command block bit mask definitions */

#define CFD_S_DIAG_F	0x0800		/* diag failed */

/* Command Frame Description and defines */

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

typedef UINT32 *	FD_ID;
typedef UINT32 *	CSR_ID;
typedef FD_ID	  	CFD_ID;
typedef FD_ID	  	RFD_ID;

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

#endif /* __INCfei82557Endh */

