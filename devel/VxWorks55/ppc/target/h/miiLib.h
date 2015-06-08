/* miiLib.h - Media Independent Interface library header */

/* Copyright 1990-2002 Wind River Systems, Inc. */
/*
modification history
--------------------
01i,13may02,rcs added pPhyOptRegsRtn field to PHY_INFO. SPR# 76711
01h,14jun01,rcs moved new registers used by GMII to new struc MII_G_REGS 
                and placed it at end of PHY_INFO. (SPR# 68502) 
01g,23feb01,jln add basic GMII support for 1000-T auto-negotiation (SPR# 68502)
01f,23nov99,cn  added miiLibInit () to make miiLib a component (SPR #29542).
01e,08nov99,cn  added fields phyLinkDownRtn, pMiiPhyNode, miiPhyPresent
		to PHY_INFO structure. New flags MII_ALL_BUS_SCAN,
		MII_PHY_MONITOR, and macro MII_MONITOR_DELAY. Also 
		added miiPhyUnInit (), miiLibUnInit ().
01d,27sep99,cn  added definition of MII_CR_DEF_0_MASK.
01c,13sep99,cn  added definition of MII_MAX_REG_NUM, moved global APIs
		from miiLib.c (SPR# 28305).
01b,15jun99,cn	changes after the code review.
01a,16mar99,cn	written from motFccEnd.h, 01b.
*/

/*
DESCRIPTION
 
This module implements a Media Independent Interface (MII) library.

INCLUDE FILES:
*/

#ifndef __INCmiiLibh
#define __INCmiiLibh

/* includes */

#ifdef __cplusplus
extern "C" {
#endif

#define S_miiLib_PHY_LINK_DOWN			(M_miiLib | 1)
#define S_miiLib_PHY_NULL		        (M_miiLib | 2)
#define S_miiLib_PHY_NO_ABLE             	(M_miiLib | 3)
#define S_miiLib_PHY_AN_FAIL             	(M_miiLib | 4)

/* defines */

#define MII_SYS_DELAY(delay)						\
if (pPhyInfo->phyDelayRtn != NULL)					\
    ((* (pPhyInfo->phyDelayRtn)) (delay))

#define MII_READ(addr, reg, val1, val2)					\
    val2 = ((* (pPhyInfo->phyReadRtn)) ((pPhyInfo->pDrvCtrl), 		\
					(addr), (reg), (val1)));

#define MII_WRITE(addr, reg, val1, val2)				\
    val2 = ((* (pPhyInfo->phyWriteRtn)) ((pPhyInfo->pDrvCtrl), 		\
					(addr), (reg), (val1)));

/* library flags */
 
#define MII_PHY_FLAGS_SET(setBits)                                      \
    (pPhyInfo->phyFlags |= (setBits))
 
#define MII_PHY_FLAGS_ARE_SET(setBits)                                  \
    (pPhyInfo->phyFlags & (setBits))
 
#define MII_PHY_FLAGS_GET(setBits)                                      \
    (pPhyInfo->phyFlags)
 
#define MII_PHY_FLAGS_CLEAR(clearBits)                                  \
    (pPhyInfo->phyFlags &= ~(clearBits))
 
/* MII definitions */

#define MII_CRC_LEN		0x4	/* CRC length in bytes */
#define MII_ETH_MAX_PCK_SZ      (ETHERMTU + SIZEOF_ETHERHEADER          \
				 + MII_CRC_LEN)
#define MII_MAX_PHY_NUM		0x20	/* max number of attached PHYs */
#define MII_MAX_REG_NUM         0x20    /* max number of registers */

#define MII_CTRL_REG		0x0	/* Control Register */
#define MII_STAT_REG		0x1	/* Status Register */
#define MII_PHY_ID1_REG		0x2	/* PHY identifier 1 Register */
#define MII_PHY_ID2_REG		0x3	/* PHY identifier 2 Register */
#define MII_AN_ADS_REG		0x4	/* Auto-Negotiation 	  */
					/* Advertisement Register */
#define MII_AN_PRTN_REG		0x5	/* Auto-Negotiation 	    */
					/* partner ability Register */
#define MII_AN_EXP_REG		0x6	/* Auto-Negotiation   */
					/* Expansion Register */
#define MII_AN_NEXT_REG		0x7	/* Auto-Negotiation 	       */
					/* next-page transmit Register */

#define MII_AN_PRTN_NEXT_REG	0x8  /* Link partner received next page */ 
#define MII_MASSLA_CTRL_REG	0x9  /* MATER-SLAVE control register */
#define MII_MASSLA_STAT_REG	0xa  /* MATER-SLAVE status register */
#define MII_EXT_STAT_REG	0xf  /* Extented status register */

/* MII control register bit  */

#define MII_CR_COLL_TEST	0x0080		/* collision test */
#define MII_CR_FDX		0x0100		/* FDX =1, half duplex =0 */
#define MII_CR_RESTART		0x0200		/* restart auto negotiation */
#define MII_CR_ISOLATE		0x0400		/* isolate PHY from MII */
#define MII_CR_POWER_DOWN	0x0800		/* power down */
#define MII_CR_AUTO_EN		0x1000		/* auto-negotiation enable */
#define MII_CR_100		0x2000		/* 0 = 10mb, 1 = 100mb */
#define MII_CR_LOOPBACK		0x4000		/* 0 = normal, 1 = loopback */
#define MII_CR_RESET		0x8000		/* 0 = normal, 1 = PHY reset */
#define MII_CR_NORM_EN		0x0000		/* just enable the PHY */
#define MII_CR_DEF_0_MASK       0xca7f          /* they must return zero */
#define MII_CR_RES_MASK       	0x003f          /* reserved bits,return zero */

/* MII Status register bit definitions */

#define MII_SR_LINK_STATUS	0x0004       	/* link Status -- 1 = link */
#define MII_SR_AUTO_SEL		0x0008       	/* auto speed select capable */
#define MII_SR_REMOTE_FAULT     0x0010      	/* Remote fault detect */
#define MII_SR_AUTO_NEG         0x0020      	/* auto negotiation complete */
#define MII_SR_10T_HALF_DPX     0x0800     	/* 10BaseT HD capable */
#define MII_SR_10T_FULL_DPX     0x1000    	/* 10BaseT FD capable */
#define MII_SR_TX_HALF_DPX      0x2000    	/* TX HD capable */
#define MII_SR_TX_FULL_DPX      0x4000     	/* TX FD capable */
#define MII_SR_T4               0x8000    	/* T4 capable */
#define MII_SR_ABIL_MASK        0xff80    	/* abilities mask */
#define MII_SR_EXT_CAP          0x0001    	/* extended capabilities */

/*  MII ID2 register bit mask */

#define MII_ID2_REVISON_MASK    0x000f
#define MII_ID2_MODE_MASK       0x03f0

/* MII AN advertisement Register bit definition */

#define MII_ANAR_10TX_HD        0x0020
#define MII_ANAR_10TX_FD        0x0040
#define MII_ANAR_100TX_HD       0x0080
#define MII_ANAR_100TX_FD       0x0100
#define MII_ANAR_100T_4         0x0200
#define MII_ANAR_PAUSE          0x0400
#define MII_ANAR_ASM_PAUSE      0x0800
#define MII_ANAR_REMORT_FAULT   0x2000
#define MII_ANAR_NEXT_PAGE      0x8000
#define MII_ANAR_PAUSE_MASK     0x0c00

/* MII Link Code word  bit definitions */

#define MII_BP_FAULT	0x2000       	/* remote fault */
#define MII_BP_ACK	0x4000       	/* acknowledge */
#define MII_BP_NP	0x8000       	/* nexp page is supported */

/* MII Next Page bit definitions */

#define MII_NP_TOGGLE	0x0800       	/* toggle bit */
#define MII_NP_ACK2	0x1000       	/* acknowledge two */
#define MII_NP_MSG	0x2000       	/* message page */
#define MII_NP_ACK1	0x4000       	/* acknowledge one */
#define MII_NP_NP	0x8000       	/* nexp page will follow */

/* MII Expansion Register bit definitions */

#define MII_EXP_FAULT	0x0010       	/* parallel detection fault */
#define MII_EXP_PRTN_NP	0x0008       	/* link partner next-page able */
#define MII_EXP_LOC_NP	0x0004       	/* local PHY next-page able */
#define MII_EXP_PR	0x0002       	/* full page received */
#define MII_EXP_PRT_AN	0x0001       	/* link partner auto negotiation able */

/* MII Master-Slave Control register bit definition */

#define MII_MASSLA_CTRL_1000T_HD    0x100
#define MII_MASSLA_CTRL_1000T_FD    0x200
#define MII_MASSLA_CTRL_PORT_TYPE   0x400
#define MII_MASSLA_CTRL_CONFIG_VAL  0x800
#define MII_MASSLA_CTRL_CONFIG_EN   0x1000

/* MII Master-Slave Status register bit definition */

#define MII_MASSLA_STAT_LP1000T_HD  0x400
#define MII_MASSLA_STAT_LP1000T_FD  0x800
#define MII_MASSLA_STAT_REMOTE_RCV  0x1000
#define MII_MASSLA_STAT_LOCAL_RCV   0x2000
#define MII_MASSLA_STAT_CONF_RES    0x4000
#define MII_MASSLA_STAT_CONF_FAULT  0x8000

/* MII Extented Status register bit definition */

#define MII_EXT_STAT_1000T_HD       0x1000
#define MII_EXT_STAT_1000T_FD       0x2000
#define MII_EXT_STAT_1000X_HD       0x4000
#define MII_EXT_STAT_1000X_FD       0x8000

/* technology ability field bit definitions */

#define MII_TECH_10BASE_T	0x0020	/* 10Base-T */
#define MII_TECH_10BASE_FD	0x0040	/* 10Base-T Full Duplex */
#define MII_TECH_100BASE_TX	0x0080	/* 100Base-TX */
#define MII_TECH_100BASE_TX_FD	0x0100	/* 100Base-TX Full Duplex */
#define MII_TECH_100BASE_T4	0x0200	/* 100Base-T4 */

#define MII_TECH_PAUSE		0x0400  /* PAUSE */
#define MII_TECH_ASM_PAUSE	0x0800  /* Asym pause */
#define MII_TECH_PAUSE_MASK	0x0c00 

#define MII_ADS_TECH_MASK	0x1fe0	/* technology abilities mask */
#define MII_TECH_MASK		MII_ADS_TECH_MASK
#define MII_ADS_SEL_MASK	0x001f	/* selector field mask */

#define MII_AN_FAIL             0x10    /* auto-negotiation fail */
#define MII_STAT_FAIL           0x20    /* errors in the status register */
#define MII_PHY_NO_ABLE     	0x40    /* the PHY lacks some abilities */

/* MII management frame structure */

#define MII_MF_PREAMBLE		0xffffffff	/* preamble pattern */
#define MII_MF_ST		0x1		/* start of frame pattern */
#define MII_MF_OP_RD		0x2		/* read operation pattern */
#define MII_MF_OP_WR		0x1		/* write operation pattern */

#define MII_MF_PREAMBLE_LEN	0x20		/* preamble lenght in bit */
#define MII_MF_ST_LEN		0x2		/* start frame lenght in bit */
#define MII_MF_OP_LEN		0x2		/* op code lenght in bit */
#define MII_MF_ADDR_LEN		0x5		/* PHY addr lenght in bit */
#define MII_MF_REG_LEN		0x5		/* PHY reg lenght in bit */
#define MII_MF_TA_LEN		0x2		/* turnaround lenght in bit */
#define MII_MF_DATA_LEN		0x10		/* data lenght in bit */

/* defines related to the PHY device */
 
#define MII_PHY_PRE_INIT    	0x0001          /* PHY info pre-initialized */
#define MII_PHY_AUTO        	0x0010          /* auto-negotiation allowed */
#define MII_PHY_TBL         	0x0020          /* use negotiation table */
#define MII_PHY_100         	0x0040          /* PHY may use 100Mbit speed */
#define MII_PHY_10          	0x0080          /* PHY may use 10Mbit speed */
#define MII_PHY_FD          	0x0100          /* PHY may use full duplex */
#define MII_PHY_HD          	0x0200          /* PHY may use half duplex */
#define MII_PHY_ISO		0x0400          /* isolate all PHYs */
#define MII_PHY_PWR_DOWN    	0x0800          /* power down mode */
#define MII_PHY_DEF_SET		0x1000		/* set a default mode */
#define MII_ALL_BUS_SCAN	0x2000		/* scan the all bus */
#define MII_PHY_MONITOR		0x4000		/* monitor the PHY's status */
#define MII_PHY_INIT		0x8000		/* PHY info initialized */
#define MII_PHY_1000T_FD	0x10000		/* PHY may use 1000-T full duplex */
#define MII_PHY_1000T_HD	0x20000		/* PHY mau use 1000-T half duplex */
#define MII_PHY_TX_FLOW_CTRL	0x40000		/* Transmit flow control */
#define MII_PHY_RX_FLOW_CTRL	0x80000		/* Receive flow control */
#define MII_PHY_GMII_TYPE	0x100000    /* GMII = 1, MII = 0 */

/* miscellaneous defines */

#define MII_PHY_DEF_DELAY   300             /* max delay before link up, etc. */
#define MII_PHY_NO_DELAY    0x0		    /* do not delay */
#define MII_PHY_NULL        0xff            /* PHY is not present */
#define MII_PHY_DEF_ADDR    0x0             /* default PHY's logical address */

#ifndef MII_MONITOR_DELAY
#   define MII_MONITOR_DELAY   0x5		/* in seconds */
#endif

#define MII_PHY_LINK_UNKNOWN	0x0       /* link method - Unknown */
#define MII_PHY_LINK_AUTO	0x1       /* link method - Auto-Negotiation */
#define MII_PHY_LINK_FORCE	0x2       /* link method - Force link */

/*
 * these values may be used in the default phy mode field of the load
 * string, since that is used to force the operating mode of the PHY
 * in case any attempt to establish the link failed.
 */
 
#define PHY_10BASE_T            0x00     /* 10 Base-T */
#define PHY_10BASE_T_FDX        0x01     /* 10 Base Tx, full duplex */
#define PHY_100BASE_TX          0x02     /* 100 Base Tx */
#define PHY_100BASE_TX_FDX      0x03     /* 100 Base TX, full duplex */
#define PHY_100BASE_T4          0x04     /* 100 Base T4 */
#define PHY_AN_ENABLE          	0x05     /* re-enable auto-negotiation */
 
#define MII_FDX_STR         "full duplex"   /* full duplex mode */
#define MII_FDX_LEN         sizeof (MII_FDX_STR)   /* full duplex mode */
#define MII_HDX_STR         "half duplex"   /* half duplex mode */
#define MII_HDX_LEN         sizeof (MII_HDX_STR)   /* full duplex mode */

#define MII_AN_TBL_MAX		20	/* max number of entries in the table */

/* allowed PHY's speeds */
#define MII_1000MBS         1000000000      /* bits per sec */ 
#define MII_100MBS          100000000       /* bits per sec */
#define MII_10MBS           10000000        /* bits per sec */

/* typedefs */

typedef INT16 MII_AN_ORDER_TBL [MII_AN_TBL_MAX];

typedef BOOL MII_PHY_BUS [MII_MAX_PHY_NUM];

typedef struct mii_phy_node
    {
    NODE              	node;
    struct phy_info *	pPhyInfo;
    } MII_PHY_NODE;

/* 
 * the structure below only represents the status of the PHY's registers
 * when they were last read through the MII interface
 */

typedef struct mii_regs
    {
    UINT16		phyStatus;	/* PHY's status register */
    UINT16		phyCtrl;	/* PHY's control register */
    UINT16		phyId1;		/* PHY's identifier field 1 */
    UINT16		phyId2;		/* PHY's identifier field 2 */
    UINT16		phyAds;		/* PHY's advertisement register */
    UINT16		phyPrtn;	/* PHY's partner register */
    UINT16		phyExp;		/* PHY's expansion register */
    UINT16		phyNext;	/* PHY's next page transmit register */
    } MII_REGS;

typedef struct mii_g_regs
    {
    UINT16              phyRcvNext;     /* PHY'S partner received next page */
    UINT16              phyMSCtrl;      /* PHY'S MASTER-SLAVE Control register */
    UINT16              phyMSStatus;    /* PHY'S MASTER-SLAVE Status register */
    UINT16              phyExtStatus;   /* PHY'S extented Status register */
    } MII_G_REGS;


/* 
 * the structure below is to be allocated and filled in by the 
 * driver before it attempts to use the miiPhyInit () routine. Some
 * of its field may be overwritten by miiPhyInit (), since their initial
 * values may not be appropriate. In addition, some field do not have
 * to be initialized by the calling task, since miiPhyInit () may use a
 * default value, whereas some other are mandatory. To help understand this,
 * the comment for each field includes flags stating whether the relevent
 * field in the PHY_INFO structure is an input (I) parameter to miiPhyInit (),
 * an output (O) parameter, or an input-output (I/O) parameter first,
 * and then whether it is optional (O) or mandatory (M). For output 
 * parameters, the second flag is a "don't care" (X).
 */

typedef struct phy_info
    {
    MII_REGS	        miiRegs;	/* PHY registers - (O|X) */
    UINT8		phyAddr;	/* address of a PHY - (I/O)|(O) */
    UINT32		phyFlags;	/* some flags - (I/O)|(O) */
    UINT32              phySpeed;   	/* 10/100 Mbit/sec  - (O|X) */
    char                phyMode[20];    /* half/full duplex mode  - (O|X) */
    UINT32              phyDefMode;     /* default operating mode  - (I|O) */
    MII_AN_ORDER_TBL *	phyAnOrderTbl;	/* auto-negotiation table - (I|O) */
    FUNCPTR		phyReadRtn;	/* phy read routine - (I|M) */
    FUNCPTR		phyWriteRtn;	/* phy write routine - (I|M) */
    FUNCPTR		phyDelayRtn;	/* phy delay routine - (I|O) */
    UINT32		phyDelayParm;	/* parameter to delay routine - (I|O) */
    UINT32		phyMaxDelay;	/* phy max delay - (I|O) */
    void * 		pDrvCtrl;	/* driver control structure - (I|M) */
    FUNCPTR		phyLinkDownRtn; /* phy status down routine - (I|O) */
    MII_PHY_NODE *	pMiiPhyNode;	/* phy node - O */
    MII_PHY_BUS *	miiPhyPresent;	/* which PHYs are there - O */
    UINT32		phyLinkMethod;	/* phy link method (Auto/Force) - (O|X) */
    UINT32		phyAutoNegotiateFlags; /* flag for auto-negotiation - (O|X) */
    MII_G_REGS		miiGRegs;       /* Giga Bit PHY registers - (O|X) */
    FUNCPTR		pPhyOptRegsRtn; /* device specific pPhyOptRegsRtn */
    } PHY_INFO;

/* globals */
 
void  		miiPhyOptFuncSet (FUNCPTR optRegsFunc);
STATUS          miiPhyInit (PHY_INFO * pPhyInfo);
STATUS          miiLibUnInit (void);
STATUS          miiLibInit (void);
STATUS          miiPhyUnInit (PHY_INFO * pPhyInfo);
void            miiShow (PHY_INFO * pPhyInfo);
STATUS          miiRegsGet (PHY_INFO * pPhyInfo, UINT regNum, UCHAR * buff);
STATUS		miiAnCheck (PHY_INFO * pPhyInfo, UINT8 phyAddr);

#ifdef __cplusplus
}
#endif

#endif /* __INCmiiLibh */

