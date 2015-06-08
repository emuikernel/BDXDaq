/* ncr5390_2.h - NCR 53C90 Advanced SCSI Controller header file */

/*
modification history
--------------------
01a,07may96,dds  written (from jaideep)
*/

#ifndef __INCncr5390_2h
#define __INCncr5390_2h

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#include "scsiLib.h"
#include "semLib.h"


/* ASC state enumeration */

typedef enum ascState
    {
    ASC_STATE_IDLE = 0,
    ASC_STATE_SELECT_PENDING,
    ASC_STATE_CONNECTED
    } ASC_STATE;

/* SCSI controller structure */

typedef struct			/* NCR_5390_SCSI_CTRL - NCR 5390 SCSI controller info */
    {
    SCSI_CTRL  scsiCtrl;        /* generic SCSI controller info */
    SEM_ID     xferDoneSem;     /* semaphore for waiting on transfer */
    UINT       clkPeriod;       /* period of the controller clock (nsec) */
    UINT       maxBytesPerXfer;	/* limit of ASC's DMA transfer counter */
    UINT       defSelTimeOut;   /* default dev. select time-out (usec) */
    FUNCPTR    sysScsiDmaStart; /* function for SCSI DMA I/O */
    FUNCPTR    sysScsiDmaAbort; /* function for SCSI DMA aborts */
    int        sysScsiDmaArg;   /* call-back argument for SCSI DMA funcs. */
    int        devType;		/* type of device (see define's below) */
    TBOOL      slowCableMode;	/* TRUE to select slow cable mode */
    TBOOL      resetReportDsbl;	/* TRUE to disable SCSI bus reset reporting */
    TBOOL      parityTestMode;	/* TRUE to enable par test mode (DO NOT USE!) */
    TBOOL      parityCheckEnbl;	/* TRUE to enable parity checking */
    UINT8      clkCvtFactor;   	/* value of the clock conversion factor */
    int        chipType;    	/* chip type (see definitions below) */
    ASC_STATE  state;           /* current state of controller */
    int        selBusId;        /* SCSI id of target being selected */
    int        reqPhase;        /* current info xfer phase of SCSI */
    BOOL       xferPending;     /* is a transfer pending on the controller ? */
    int        xferPhase;   	/* SCSI phase currently being xferred */
    UINT       xferNBytes;  	/* Number of bytes currently being xfered */
    UINT8    * xferAddr;        /* Address of buffer being xfered */
    int        xferDirection;   /* Direction of current transfer */
    BOOL       xferUsingDma;	/* Current transfer is using DMA ? */
    BOOL       syncDataXfer;	/* Synchronous data transfer enabled ? */

    volatile UINT8 *pTclReg;	/* ptr xfer count LSB reg */
    volatile UINT8 *pTchReg;	/* ptr xfer count MSB reg */
    volatile UINT8 *pFifoReg;	/* ptr FIFO reg */
    volatile UINT8 *pCmdReg;	/* ptr command reg */
    volatile UINT8 *pStatReg;	/* ptr status reg */
    volatile UINT8 *pIntrReg;	/* ptr interrupt reg */
    volatile UINT8 *pStepReg;	/* ptr sequence step reg */
    volatile UINT8 *pFlgsReg;	/* ptr FIFO flags reg */
    volatile UINT8 *pCfg1Reg;	/* ptr configuration 1 reg */
    volatile UINT8 *pClkReg;	/* ptr clock conversion factor reg */
    volatile UINT8 *pTestReg;	/* ptr test mode reg */
    volatile UINT8 *pCfg2Reg;	/* ptr configuration 2 reg (if present) */
    volatile UINT8 *pCfg3Reg;	/* ptr configuration 3 reg (if present) */
    volatile UINT8 *pTcxReg;	/* ptr to xfer count ext'n reg (if present) */
    } NCR_5390_SCSI_CTRL;

typedef struct ncr5390functbl
    {
    FUNCPTR ascCommand;
    FUNCPTR ascIntr;
    FUNCPTR ascRegRead;
    FUNCPTR ascRegWrite;
    FUNCPTR ascXferCountGet;
    FUNCPTR ascXferCountSet;
    FUNCPTR ncr5390CtrlInit;
    FUNCPTR ncr5390Show;
    } SCSIDRV_FUNC_TBL;

/* defines for the overlapping registers */

#define pBidReg           pStatReg		/* select/reselect bus id */
#define pTmoReg           pIntrReg		/* select/reselect bus id */

/* ASC device types */

#define ASC_NCR5390		0
#define ASC_NCR5390A		1
#define ASC_NCR5390B		2
#define ASC_NCR5394		3
#define ASC_NCR5395		4
#define ASC_NCR5396		5
#define	ASC_NCR53C9X	        6   	    /* as in NCR 89C100 (for frc5ce) */

/* FIFO register */

#define NCR5390_FIFO_DEPTH       16

/* command register */

#define NCR5390_NOP             0x00
#define NCR5390_FIFO_FLUSH      0x01
#define NCR5390_CHIP_RESET      0x02
#define NCR5390_BUS_RESET       0x03

#define NCR5390_INFO_TRANSFER   0x10
#define NCR5390_I_CMD_COMPLETE  0x11
#define NCR5390_MSG_ACCEPTED    0x12
#define NCR5390_SET_ATTENTION   0x1a
#define NCR5390_UNSET_ATTENTION 0x1b

#define NCR5390_SEND_MESSAGE    0x20
#define NCR5390_SEND_STATUS     0x21
#define NCR5390_SEND_DATA       0x22
#define NCR5390_DISCONNECT_SEQ  0x23
#define NCR5390_TERMINATE_SEQ   0x24
#define NCR5390_T_CMD_COMPLETE  0x25
#define NCR5390_DISCONNECT      0x27
#define NCR5390_RCV_MESSAGE     0x28
#define NCR5390_RCV_COMMAND     0x29
#define NCR5390_RCV_DATA        0x2a
#define NCR5390_RCV_CMD_SEQ     0x2b

#define NCR5390_RESELECT        0x40
#define NCR5390_SELECT          0x41
#define NCR5390_SELECT_ATN      0x42
#define NCR5390_SEL_ATN_STOP    0x43
#define NCR5390_SELECTION_ENBL  0x44
#define NCR5390_SELECTION_DSBL  0x45
#define NCR5390_SELECT_ATN3     0x46

#define NCR5390_DMA_OP          0x80

/* status register */

#define NCR5390_DOUT_PHASE      0x00
#define NCR5390_DIN_PHASE       0x01
#define NCR5390_CMND_PHASE      0x02
#define NCR5390_STAT_PHASE      0x03
#define NCR5390_MSGOUT_PHASE    0x06
#define NCR5390_MSGIN_PHASE     0x07
#define NCR5390_PHASE_MASK      0x07
#define NCR5390_VAL_GROUP       0x08
#define NCR5390_TERMINAL_CNT    0x10
#define NCR5390_PARITY_ERR      0x20
#define NCR5390_GROSS_ERR       0x40
#define NCR5390_INTERRUPT       0x80

/* interrupt status register */

#define NCR5390_SELECTED        0x01
#define NCR5390_ATN_SELECTED    0x02
#define NCR5390_RESELECTED      0x04
#define NCR5390_FUNC_COMPLETE   0x08
#define NCR5390_BUS_SERVICE     0x10
#define NCR5390_DISCONNECTED    0x20
#define NCR5390_ILLEGAL_CMD     0x40
#define NCR5390_SCSI_RESET      0x80

/* fifo flags register */

#define NCR5390_MORE_DATA       0x1f

/* configuration register 1 */

#define NCR5390_OWN_ID_MASK     0x07
#define NCR5390_CHIPTEST_ENBL   0x08
#define NCR5390_PAR_CHECK_ENBL  0x10
#define NCR5390_PAR_TEST_ENBL   0x20
#define NCR5390_RESET_REP_DSBL  0x40
#define NCR5390_SLOW_CABLE      0x80

/* test register */

#define NCR5390_TARGET	 	0x01
#define NCR5390_INITIATOR       0x02
#define NCR5390_HIGH_IMP        0x04

/* configuration register 2 */

#define NCR5390_DMA_PAR_ENBL    0x01
#define NCR5390_REG_PAR_ENBL    0x02
#define NCR5390_PARITY_ABORT    0x04
#define NCR5390_SCSI_2          0x08
#define NCR5390_DREQ_HIGH_IMP   0x10
#define	NCR5390_FEATURE_ENABLE	0x40	/* enable advanced features */

/* configuration register 3 */

#define	NCR5390_FAST_CLOCK  	0x01	/* clock freq > 25 MHz               */
#define	NCR5390_FAST_SCSI   	0x02	/* allow fast SCSI data rates        */
#define	NCR5390_CDB10	    	0x04	/* allow 10-byte group 2 CDBs        */
#define	NCR5390_Q_TAG_ENABLE	0x08	/* auto-receive queue tag messages   */
#define	NCR5390_CHECK_ID_MSG	0x10	/* check ID msg during (re)selection */

/** sequence step register **/

#define NCR5390_SEQ_ATNSTOP     0x01
#define NCR5390_SEQ_OUT         0x04
#define NCR5390_SEQ_TIME        0x07
#define NCR5390_SEQ_ERR         0x00

/** miscellaneous constants **/

#define ASC_SYNC_XFER_PARAMS_ASYNC 0x00

#define ASC_MIN_XFER_PERIOD     5
#define ASC_MAX_XFER_PERIOD     35
#define ASC_MAX_REQ_ACK_OFFSET  15

#define ASC_STAT_BITS           0x07

typedef NCR_5390_SCSI_CTRL ASC;

/* external declarations */

IMPORT SCSIDRV_FUNC_TBL *pNcr5390IfTbl;

#if defined(__STDC__) || defined(__cplusplus)

IMPORT  void ncr5390IfTblInit ();
IMPORT  void ascIntr ( ASC *);
IMPORT STATUS ncr5390CtrlInit (FAST ASC *, FAST int , FAST UINT, int);

IMPORT	NCR_5390_SCSI_CTRL * ncr5390CtrlCreateScsi2 (UINT8 *baseAdrs, 
						int regOffset,
						UINT clkPeriod,
						UINT    sysScsiDmaMaxBytes,
						FUNCPTR sysScsiDmaXfer,
						FUNCPTR sysScsiDmaAbort,
						int     sysScsiDmaArg);


#ifndef NCR5390_2_LOCAL_FUNCS
IMPORT void ascCommand (ASC *pAsc, int cmdCode);
IMPORT void ncr5390Intr (ASC *);
IMPORT void ascXferCountGet (FAST ASC *, FAST UINT *);
IMPORT STATUS ascXferCountSet (FAST ASC *, FAST UINT);
IMPORT STATUS ncr5390CtrlInitScsi2 (FAST ASC *, FAST int , FAST UINT);
IMPORT STATUS ncr5390ShowScsi2 ( FAST NCR_5390_SCSI_CTRL *);
#endif

#else	/* __STDC__ */

IMPORT  void ncr5390IfTblInit();
IMPORT	NCR_5390_SCSI_CTRL * ncr5390CtrlCreateScsi2 ();

#ifndef NCR5390_2_LOCAL_FUNCS
IMPORT void ascCommand ();
IMPORT void ncr5390Intr ();
IMPORT void ascXferCountGet ();
IMPORT STATUS ascXferCountSet ();
IMPORT STATUS ncr5390CtrlInitScsi2 ();
IMPORT STATUS ncr5390ShowScsi2 ();
#endif

#endif	/* __STDC__ */
#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCncr5390_2h */
