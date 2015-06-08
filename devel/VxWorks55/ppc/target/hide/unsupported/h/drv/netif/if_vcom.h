/* if_vcom.h - VLAN ethernet board transparent mode library */

/* Copyright 1990-1994, PEP Modular Computers */
/* Copyright 1984-1994, Wind River Systems, Inc. */

/*
modification history
--------------------
01f,07jan94,haw  removed include of copyright, fixed spacing, indentation
01e,07jun93,haw  renamed to if_vcom.h (used to be vcomLib.h),
                 moved it to this directory, copyied definition of
                 VCOM structure to this file, moved all includes to the driver,
                 now uses of volatile variables where necessary, added
                 definitions for some registers
01d,07jan93,haw  made variables LOCAL
01c,09oct92,haw  added definitions for EEPROM
01b,12jul92,haw  added some casts to satisfy LINT
01a,11jun92,haw  written
*/
 
#ifndef __INCif_vcomh
#define __INCif_vcomh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define EADDRS          6
 
#define GOTTRAILERTYPE  0
#define GOTTYPE         1
#define GOTNOTYPE       2
 
#define ETHER_MIN   (ETHERMIN + sizeof(struct ether_header))
#define ETHER_MAX   (ETHERMTU + sizeof(struct ether_header))

/* Intel 82596 specific definitions  */
 
/*
 * RFD - receive frame descriptor: linear mode, simplified model
 */
 
typedef struct rfd
    {
    volatile UINT16      rfdStat1;   /* first status word */
    volatile UINT16      rfdStat2;   /* second status word */
    struct rfd *pRfdLeNext;          /* ptr to next rfd (little-end) */
    void       *pRfdRbd;             /* ptr to receive buffer descriptor */
    volatile UINT16      rfdAcount;  /* number of actual bytes in buffer */
    UINT16      rfdSize;             /* size of buffer */
    volatile UINT8       rfdDa [6];  /* destination address */
    volatile UINT8       rfdSa [6];  /* source address */
    UINT16      rfdLength;           /* length/type from 802.3 frame */
    UINT8       rfdData [ETHER_MAX]; /* ethernet received data */
    struct rfd *pRfdBeNext;          /* ptr to next rfd (big-end for driver) */
    struct rfd *pRfdBePrev;          /* ptr to prev rfd (big-end for driver) */
    } RFD;

/*
 * TBD - Transmit buffer descriptor
 */
 
typedef struct tbd
    {
    UINT16      tbdCnt;              /* number of bytes to xmit from this TBD */
    UINT16      zeroes;
    struct tbd *pTbdLeNext;          /* pointer to next tbd in xmit chain */
    void       *pTbdData;            /* pointer to data to xmit */
    UINT32      tbdPadd;             /* pad to 16 bytes */
    UINT8       xmitData [1536];     /* xmit data buffer (lld + alignment) */
    } TBD;

typedef struct tcb
    {
    TBD        *pTbd;                /* ptr to transmit buffer descriptor */
    UINT16      tcbCnt;              /* number of bytes to xmit from TCB */
    UINT16      zeroes;              /* must be zeroes */
    UINT8       da [6];              /* destination ethernet address */
    UINT16      tcbLen;              /* length/type for 802.3 frame */
    } TCB;
 
/*
 * CB - Command block template
 */
typedef struct cb
    {
    volatile UINT16      cbStatus;   /* status word */
    volatile UINT16      cbCommand;  /* command word */
    struct cb *pCbLeNext;            /* ptr to nxt command block (little end) */
    union
        {
        UINT8   ia [6];              /* ethernet address (for IA command) */
        TCB     tcb;                 /* transmit command block */
        } cmd;
    struct cb  *pCbBeNext;           /* next cmd block (bigend for driver) */
    struct cb  *pCbBePrev;           /* prev cmd block (bigend for driver) */
    } CB;

/*
 * SCB - system control block template
 */
 
typedef struct scb
    {
    volatile UINT16      scbStatus;  /* status word */
    volatile UINT16      scbCmd;     /* command word */
    CB         *pCbl;                /* command block list ptr */
    RFD        *pRfa;                /* receive frame area ptr */
    UINT32      crcErrs;             /* CRC errors */
    UINT32      alignErrs;           /* alignment errors */
    UINT32      resErrs;             /* resource errors */
    UINT32      overrunErrs;         /* overrun errors */
    UINT32      rcvcdtErrs;          /* receive collisions errors */
    UINT32      sfErrs;              /* short frame errors */
    UINT16      toffTimer;           /* throttle t-off timer */
    UINT16      tonTimer;            /* throttle t-on timer */
    UINT8       cbPadd [8];          /* padd to 16-byte bound */
    } SCB;
 
/*
 * ISCP - Intermediate System configuration pointer
 */
 
typedef struct iscp
    {
    UINT8       iscpUnused1;
    UINT8       iscpBusy;            /* busy indicator */
    UINT16      iscpUnused2;
    SCB        *pIscpLeSCB;          /* (little end) scb pointer */
    SCB        *pIscpBeSCB;          /* (big end) scb pointer */
    UINT8       iscpPadd [4];        /* padd to 16-byte bound */
    } ISCP;

/*
 * SCP - System configuration pointer
 */
 
typedef struct scp
    {
    UINT32      scpSysbus;           /* configuration goodies */
    UINT32      scpUnused2;
    ISCP       *pScpLeISCP;          /* (little end) iscp pointer */
    ISCP       *pScpBeISCP;          /* (big end) iscp pointer */
    } SCP;
 
/*
 * template for selftest result
 */

typedef struct
    {
    volatile UINT32      strRomsig;  /* ROM signature */
    volatile UINT32      strResult;  /* selftest result */
    } STR;
 
/* defines for the TBD */
 
#define TBD_EOF             0x8000   /* last TBD in xmit list */
#define TBD_CNTMASK         0x3fff   /* 14-bit count field */
 
#define TBD_NOT_USED    (void *)-1
 
/* defines for the TCB */

#define TCB_EOF             0x8000   /* entire frame is in TCB */
#define TCB_CNTMASK         0x3fff   /* 14-bit count field */
 
/* defines for the RFD */
 
#define RFD_COMPLETE        0x8000   /* reception complete */
#define RFD_BUSY            0x4000   /* reception in progress */
#define RFD_OK              0x2000   /* received frame has no errors */
 
#define RFD_LAST            0x8000   /* last rfd in rdl */
#define RFD_SUSPEND         0x4000   /* suspend after receive */
#define RFD_FLEXIBLE        0x0008   /* flexible/simple memory model */
 
#define RFD_RBD_NOTUSED (void *)-1

#define RFD_EOF             0x8000   /* last buffer in frame */
#define RFD_USED            0x4000   /* buffer has been used (and abused) */
 
#define RFD_SIZEMASK        0x3fff   /* size is 14-bits */
 
/* defines for the CB */
 
#define CB_COMPLETE         0x8000   /* 1=command complete */
#define CB_BUSY             0x4000   /* 1=command in progress */
#define CB_OK               0x2000   /* 1=completed without error */
#define CB_ABORT            0x1000   /* 1=command aborted */
#define XMIT_LCOL           0x0800   /* 1=late collision on xmit */
#define XMIT_LCAR           0x0400   /* 1=no carrier sensed */
#define XMIT_LCTS           0x0200   /* 1=lost clear to send */
#define XMIT_UDMA           0x0100   /* 1=dma underrun */
#define XMIT_DEFR           0x0080   /* 1=transmit deferred */
#define XMIT_BEAT           0x0040   /* 1=CDT (heartbeat) active */
#define XMIT_MTRY           0x0020   /* 1=exceeded maximum retries for xmit */
#define XMIT_COLLMASK       0x000f   /* collision count field */
#define CB_LAST             0x8000   /* last command in list */
#define CB_SUSPEND          0x4000   /* suspend after this command */
#define CB_IRQ              0x2000   /* generate IRQ after command */
#define XMIT_NC             0x0010   /* 1=no CRC insertion enable */
#define XMIT_SF             0x0008   /* 1=flexible, 0=simplified (sic) */
#define CB_CMDMASK          0x0007   /* mask for command code */
#define CB_CMD_NOP               0   /* NOP command */
#define CB_CMD_IASETUP           1   /* Intermediate address setup command */
#define CB_CMD_CONFIG            2   /* CONFIG command */
#define CB_CMD_MCSETUP           3   /* Multicast setup command */
#define CB_CMD_XMIT              4   /* Transmit command */
#define CB_CMD_TDR               5   /* TDR command */
#define CB_CMD_DUMP              6   /* Dump internals command */
#define CB_CMD_DIAG              7   /* Diagnose command */
 
/* defines for the SCB */
 
#define SCB_STATUS_CX       0x8000   /* command complete interrupt */
#define SCB_STATUS_FR       0x4000   /* finished receiving frame interrupt */
#define SCB_STATUS_CNA      0x2000   /* CU left in active state */
#define SCB_STATUS_RNR      0x1000   /* RU left in ready state */
#define SCB_CU_ACTIVE       0x0200   /* CU in active state */
#define SCB_CU_SUSPEND      0x0100   /* CU is suspended */
#define SCB_CU_IDLE         0x0000   /* CU is idle */
#define SCB_RU_NO_RBD       0x0080   /* RU is in no RBD state */
#define SCB_RU_READY        0x0040   /* RU is in ready state */
#define SCB_RU_NO_RES       0x0020   /* RU is in no resources state */
#define SCB_RU_SUSPEND      0x0010   /* RU is suspended */
#define SCB_RU_IDLE         0x0000   /* RU is idle */
#define SCB_THROTTLE        0x0008   /* throttle timers loaded */
#define SCB_CX_ACK          0x8000   /* complete complete int ack */
#define SCB_FR_ACK          0x4000   /* frame reception int ack */
#define SCB_CNA_ACK         0x2000   /* CU not ready int ack */
#define SCB_RNR_ACK         0x1000   /* RU not ready int ack */
#define SCB_CU_BUSTHRI      0x0600   /* load bus throttle timers */
#define SCB_CU_BUSTHR       0x0500   /* load bus throttle timers */
#define SCB_CU_ABORT_CMD    0x0400   /* abort CU cmd */
#define SCB_CU_SUSPEND_CMD  0x0300   /* suspend CU cmd */
#define SCB_CU_RESUME_CMD   0x0200   /* resume CU cmd */
#define SCB_CU_START_CMD    0x0100   /* start CU cmd */
#define SCB_CU_NOP_CMD      0x0000   /* CU nop cmd */
#define SCB_CHIP_RESET_CMD  0x0080   /* chip reset */
#define SCB_RU_ABORT_CMD    0x0040   /* RU abort cmd */
#define SCB_RU_SUSPEND_CMD  0x0030   /* RU suspend cmd */
#define SCB_RU_RESUME_CMD   0x0020   /* RU resume cmd */
#define SCB_RU_START_CMD    0x0010   /* RU start cmd */
#define SCB_RU_NOP_CMD      0x0000   /* RU nop cmd */
 
/* defines for the ISCP */
 
#define ISCP_NOT_BUSY            1
 
/* defines for the SCP */
 
#define SYSBUS_CSW      0x00000040   /* reserved */
#define SYSBUS_INTLO    0x00000020   /* interrupt polarity */
#define SYSBUS_INTHI    0x00000020   /* interrupt polarity */
#define SYSBUS_LOCK     0x00000010   /* lock function */
#define SYSBUS_TRG      0x00000008   /* bus throttle triggers */
#define SYSBUS_MODEMASK 0x00000006   /* operation mode */
#define SYSBUS_LINEAR   0x00000004   /* operation mode (linear) */

typedef struct
    {
    UINT32      ramAddr;             /* base address for i82596 shared ram */
    UINT32      ramOffset;           /* offset in window for i82596 */
    UINT32      ramAdjust;           /* ramaddr-ramadjust = i82596 ram addr */
    UINT32      ramSize;             /* size of ram for i82596 */
    UINT32      idPromAddr;          /* address of ID prom */
    UINT16      maxRfd;              /* max. receive frame descriptors */
    UINT16      maxCbl;              /* max. xmit commands to queue */
    UINT16      portOff;             /* offset (from dev addr) to port */
    UINT16      caOff;               /* offset (from dev addr) to chan attn */
    UINT16      irqOff;              /* offset (from dev addr) to clear IRQ */
    UINT16      tOn;                 /* i82596 t-on  bus throttle value */
    UINT16      tOff;                /* i82596 t-off bus throttle value */
    UINT8       ramWidth;            /* 2=D16 (word) ram, 4=D32 (long) ram */
    UINT8       portWidth;           /* i82596 port access width */
    UINT8       caWidth;             /* i82596 channel access width */
    UINT8       irqWidth;            /* i82596 clear IRQ width */
    UINT8       sysbus;              /* i82596 SCP sysbus value */
    } VCOM_DESC;

/* chip access macros */
 
#define I82596_RESET(xx)\
    doportCommand((int) xx, (UINT8 *) 0, (UINT8) 0)
#define I82596_SELFTEST(xx, addr)\
    doportCommand((int) xx, (UINT8 *) addr, (UINT8) 1)
#define I82596_ASCP(xx, addr)\
    doportCommand((int) xx, (UINT8 *) addr, (UINT8) 2)
#define I82596_DUMP(xx, addr)\
    doportCommand((int) xx, (UINT8 *) addr, (UINT8) 3)
 
#define I82596_CHANATTN(xx)\
    *((UINT8 *)xx->pCaAddr) = 0;

/* board address offsets */
 
#define VCOM_RESET(port)      ((UINT8 *) port + 0x93)
#define VCOM_SNOOP_IRQ(port)  ((UINT8 *) port + 0x17)
#define VCOM_IRQ_VEC(port)    ((UINT8 *) port + 0x13)
#define VCOM_VME_ACC(port)    ((UINT8 *) port + 0x0f)
#define VCOM_LED_IRQ(port)    ((UINT8 *) port + 0x0b)

/* defines for VCOM_SNOOP_IRQ */

#define IROUT_VME   0x01
#define IRQEN       0x02
#define IRQL_5      0x04
#define SC0         0x40
#define SC1         0x80

/* defines for VCOM_LED_IRQ */

#define ENVRQ       0x01
#define LED_ETHER   0x02
#define LED_SCSI    0x04
 
/* EEPROM definitions */
 
#define DATA_ADDRESS       (UINT8 *) ((UINT8 *) port + 0x03)
#define CS_ADDRESS         (UINT8 *) ((UINT8 *) port + 0x04)
#define CS_ACTIVE          *(CS_ADDRESS) = 1
#define CS_PASSIVE         dummyCS = *(CS_ADDRESS)
#define SET_INPUT          *(UINT8 *) ((UINT8 *) port + 0x07) = 1

#define TOGGLE\
    {\
    *(UINT8 *) ((UINT8 *) port + 0x06) = 1;\
    lnop ();\
    *(UINT8 *) ((UINT8 *) port + 0x06) = 1;\
    }
 
/* EEPROM command definition */
 
/*                    +------------------ start bit                 */
/*                    |++---------------- opcode                    */
/*                    |||++-------------- additional opcode         */
/*                    |||||++++---------- address                   */
/*                    |||||||||                                     */
/*                    vvvvvvvvv                                     */
 
#define EREAD    /* 0%110000000 */ 0x188 /* read word from address  */

#define WORD_ACC(a,b) a = b;
 
#define CONV(a) ((UINT32) vs->pSharedRam > 0x87000000 && \
                 (UINT32) vs->pSharedRam < 0x88000000) ? \
                 (((UINT32) (a) & 0x000fffff) | 0xD0000000) : \
                 (UINT32) (a)

/* some address definitions */
 
#define VCOM_LXB_RAM    0xD0000000
#define VCOM_LXB_IO     0xC8000000
 
LOCAL UINT8 *vcomVmeRam [] =
    {
    (UINT8 *) 0x87400000,
    (UINT8 *) 0x87600000,
    (UINT8 *) 0x87800000,
    (UINT8 *) 0x87A00000
    };

LOCAL UINT8 *vcomVmePort [] =
    {
    (UINT8 *) 0x87FE0000,
    (UINT8 *) 0x87FE0100,
    (UINT8 *) 0x87FF0000,
    (UINT8 *) 0x87FF0100
    };

#define MHz

LOCAL VCOM_DESC vcomDesc =
    {
    0,          /* ramAddr:    passed by call                   */
    0,          /* ramOffset:  always 0                         */
    0,          /* ramAdjust:  always 0                         */
    256*1024,   /* ramSize:    size of ram for i82596           */
    0,          /* idPromAddr: address of ID prom               */
    32,         /* maxRfd:     max. receive frame descriptors   */
    32,         /* maxCbl:     max. xmit commands to queue      */
    0x60,       /* portOff:    offset to port access            */
    0x61,       /* caOff:      offset to channel attention      */
    0x00,       /* irqOff:     obsolete                         */
    7*25 MHz,   /* tOn:        i82596 t-on  bus throttle value  */
    1*25 MHz,   /* tOff:       i82596 t-off bus throttle value  */
    4,          /* ramWidth:   longword                         */
    2,          /* portWidth:  i82596 port access width         */
    1,          /* caWidth:    i82596 channel access width      */
    0,          /* irqWidth:   obsolete                         */
    0x54        /* sysbus:     i82596 SCP sysbus value          */
                /* 0%01010100                                   */
                /*    ^^^^^^                                    */
                /*    ||||||                                    */
                /*    ||||++----- linear mode (never change)    */
                /*    |||+------- external/internal bus trigger */
                /*    ||+-------- lock disable/enable           */
                /*    |+--------- IRQ pin active low/high       */
                /*    +---------- CSW must be 1 (Intel Errata)  */
    };

typedef struct
    {
    struct  arpcom vs_ac;        /* common ethernet structures */
    int vcomIntLevel;            /* vme interrupt level */
    int vcomIntVec;              /* vme interrupt vector */
    BOOL taskLevelActive;        /* netTask is currently processing packets */
    UINT8 *pAddr;                /* base address for device */
    UINT8 *pSharedRam;           /* base of shared CPU/VCOM RAM */
    UINT8 *pZeroPad;             /* zero padding for short packets */
    void *pPortAddr;             /* port access address */
    void *pCaAddr;               /* channel access address */
    SCP *pScp;                   /* ptr to chip SCP structure */
    ISCP *pIscp;                 /* ptr to chip ISCP structure */
    SCB *pScb;                   /* ptr to chip SCB structure */
    CB *pCb;                     /* ptr to chip CB structure (list) */
    CB *pCbTail;                 /* ptr to last CB in chip list to process */
    CB *pCbHead;                 /* ptr to first CB in chip list to process */
    CB *pCbNext;                 /* ptr to next CB entry to use */
    RFD *pRfd;                   /* ptr to chip first RFD structure */
    RFD *pRfdHead;               /* ptr to first RFD to receive from */
    TBD *pXdHead;                /* ptr to xmit buffer pool */
    TBD *pXdLast;                /* ptr to last xmit buffer */
    TBD *pXdNext;                /* ptr to next xmit buffer to use */
    BOOL running;                /* driver initialized and running */
    UINT16 nRfd;                 /* number of RFDs in use */
    UINT16 nCbl;                 /* number of CB entries in use */
    VCOM_DESC desc;              /* copy of device descriptor goodies */
    UINT8 ownEther [EADDRS];     /* own ethernet address */
    SEM_ID semId;                /* semaphore for initialization */
    } VCOM_SOFTC;
 
#define vs_if           vs_ac.ac_if     /* network-visible interface */
#define vs_enaddr       vs_ac.ac_enaddr /* hardware ethernet address */

/*
 * the structure arpcom looks in detail this way:
 *  struct ifnet ac_if;         struct ifnet
 *  UINT8 ac_enaddr [6];        ethernet hardware address
 *  UINT32 ac_ipaddr.s_addr;    copy of ip address
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCif_vcomh */
