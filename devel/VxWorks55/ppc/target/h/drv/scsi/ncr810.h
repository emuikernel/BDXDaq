/* ncr810.h - NCR 810 Script SCSI Controller header file */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,03dec98,ihw  Added identMsg{Buf,Length} in NCR810_SCSI_CTRL, to
                 support concatenated IDENTIFY/{SYNC,WIDE} messages. (SRP 24089)
01f,25jun98,ldt  Added definitions for Quadrupler, STEST4, scratch 
                 registers c-j, 895 device id, 50,70,160 MHz speeds
            	 added siopRamBaseAdrs, updated ncr810CtrlCreate (SPR 24089)
01e,23apr98,tm,  Added SSTAT2, CTEST5_DFS definitions for SPR 8384 fix
01d,08may97,dat  temp fix for SPR 8540
01c,10mar97,dds  SPR 7766: added changes for ncr875 SCSI adapter.
01b,21nov96,myz  modified PCI_TO_MEM_OFFSET for i960jx 
01a,26may95,jds  Created ; adapted from ncr710.h
*/

#ifndef __INCncr810h
#define __INCncr810h

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#include "semLib.h"
#include "scsiLib.h"

/* PRIVATE */

typedef struct ncr810Thread 	    	/* NCR810_THREAD */
    {
    SCSI_THREAD           scsiThread;	/* generic SCSI thread structure */
    struct ncr810Shared * pShMem;	/* ptr to SIOP shared data area */
    
    /*
     *	The following is a partial context of the SIOP registers which is
     *	maintained independently for each thread.  It is saved and
     *	restored by the host when scripts are started and completed.
     *
     *	See "ncr810Script.h" for aliases for the scratch registers ...
     */

    UINT8 scratcha0;		/* image of scratch 0 register */
    UINT8 scratcha1;		/* image of scratch 1 register */
    UINT8 scratcha2;		/* image of scratch 2 register */
    UINT8 scratcha3;		/* image of scratch 3 register */
    UINT8 sxfer;		/* image of sxfer register */
    UINT8 scntl3;		/* image of scntl3 for sync xfers */
    } NCR810_THREAD;


typedef struct ncr810Event  	    	/* NCR810_EVENT */
    {
    SCSI_EVENT scsiEvent;   	 	/* generic SCSI event */
    UINT       remCount;	 	/* remaining byte count (mismatch) */
    } NCR810_EVENT;

/* Script entry point identifiers (see "ncr810StartScript()" if changed) */

typedef enum ncr810ScriptEntry
    {
    NCR810_SCRIPT_WAIT           = 0,	/* wait for re-select or host cmd */
    NCR810_SCRIPT_INIT_START     = 1,	/* start an initiator thread      */
    NCR810_SCRIPT_INIT_CONTINUE  = 2,	/* continue an initiator thread   */
    NCR810_SCRIPT_TGT_DISCONNECT = 3	/* disconnect a target thread     */
    } NCR810_SCRIPT_ENTRY;


/* SIOP state enumeration */

typedef enum ncr810State
    {
    NCR810_STATE_IDLE = 0,		/* not running any script            */
    NCR810_STATE_PASSIVE,		/* waiting for reselect or host cmd  */
    NCR810_STATE_ACTIVE			/* running a client script           */
    } NCR810_STATE;


/*
 * Structure used in the ncr810SetHwRegister() and ncr810GetHwRegister().
 * This is used to try to handle the possible different hardware
 * implementations of the chip.  This structure must contain the logical
 * value one wishes e.g 0 sets register bit to zero  and 1 sets to one.
 */

typedef struct
    {
    int stest1Bit7;		/* Disable external SCSI clock 	*/
    int stest2Bit7;		/* SCSI control enable		*/
    int stest2Bit5;		/* Enable differential SCSI bus	*/
    int stest2Bit2;		/* Always WIDE SCSI		*/
    int stest2Bit1;		/* Extend SREQ/SACK filtering 	*/
    int stest3Bit7;		/* TolerANT enable 		*/
    int dmodeBit7;              /* Burst Length transfer bit 1 	*/
    int dmodeBit6;              /* Burst Length transfer bit 0 	*/
    int dmodeBit5;		/* Source I/O memory enable 	*/
    int dmodeBit4;		/* Destination I/O memory enable*/
    int scntl1Bit7;		/* Slow cable mode              */
    } NCR810_HW_REGS;


/*
 * SCSI controller structure
 *
 * NOTE: the entire structure must be located in cache-coherent memory 
 * safe for DMA transfers. This is because some fields are shared with the 
 * SCSI controller e.g. Msg In/Out buffers, Data In/Out buffers and the Cmd 
 * buffer.
 *
 * NOTE: some fields in this structure control hardware features which are
 * not supported by the current software.
 */

typedef struct                  /* NCR_810_SCSI_CTRL - NCR810 */
                                /* SCSI controller info       */
    {
    SCSI_CTRL scsiCtrl;         /* generic SCSI controller info */
    SEM_ID    singleStepSem;	/* use to debug script in single step mode */
    
				/* Hardware implementation dependencies */
    BOOL  slowCableMode;	/* TRUE to select slow cable mode */
    int   chipType;		/* Device type NCR7X0_TYPE */
				/* Only 810 Supported today */
    int   devType;		/* type of device (see define's below) */
    BOOL  resetReportDsbl;	/* TRUE to disable SCSI bus reset reporting */
    BOOL  parityTestMode;	/* TRUE enable parity test mode */
    BOOL  parityCheckEnbl;	/* TRUE to enable parity checking */
    UINT  clkPeriod;	    	/* period of controller clock (nsec x 100) */
    UINT8 clkDiv;		/* async and sync clock divider in  scntl3 */
    NCR810_HW_REGS hwRegs;	/* values used for hardware dependent regs */

    NCR810_STATE  state;	/* current state of controller */
    	    	    	    	/* shared data areas for */
    	    	    	    	/* identification & client threads */
    struct ncr810Shared *pIdentShMem;
    struct ncr810Shared *pClientShMem;

    NCR810_THREAD *pHwThread;   /* thread corresp. to current script run */
    NCR810_THREAD *pNewThread;  /* thread to be activated by ISR (if any) */
    BOOL          cmdPending;   /* TRUE => task wants to start new command */
    BOOL          singleStep;   /* TRUE => SIOP is in single-step mode */

    UINT8         identMsg[SCSI_MAX_IDENT_MSG_LENGTH + 5]; /* 5 for wide/sync */
    UINT          identMsgLength;

    volatile UINT8    *pScntl0;      /* SCTNL0 SCSI control register 0 */
    volatile UINT8    *pScntl1;      /* SCTNL1 SCSI control register 1 */
    volatile UINT8    *pScntl2;      /* SCTNL2 SCSI control register 2 */
    volatile UINT8    *pScntl3;      /* SCTNL3 SCSI control register 3 */
    volatile UINT8    *pScid;        /* SCID SCSI chip ID register */
    volatile UINT8    *pSxfer;       /* SODL SCSI transfer register */
    volatile UINT8    *pSdid;        /* SDID SCSI destination ID register */
    volatile UINT8    *pGpreg;       /* GPREG General Purpose register */
    volatile UINT8    *pSfbr;        /* SFBR SCSI first byte received reg */
    volatile UINT8    *pSocl;        /* SOCL SCSI output control latch reg */
    volatile UINT8    *pSsid;        /* SSID SCSI selector ID */
    volatile UINT8    *pSbcl;        /* SBCL SCSI bus control lines reg */
    volatile UINT8    *pDstat;       /* DSTAT DMA status register */
    volatile UINT8    *pSstat0;      /* SSTAT0 SCSI status register 0 */
    volatile UINT8    *pSstat1;      /* SSTAT1 SCSI status register 1 */
    volatile UINT8    *pSstat2;      /* SSTAT2 SCSI status register 2 */
    volatile UINT     *pDsa;         /* DSA data structure address */
    volatile UINT8    *pIstat;       /* ISTAT interrupt status register */
    volatile UINT8    *pCtest0;      /* CTEST0 chip test register 0 */
    volatile UINT8    *pCtest1;      /* CTEST1 chip test register 1 */
    volatile UINT8    *pCtest2;      /* CTEST2 chip test register 2 */
    volatile UINT8    *pCtest3;      /* CTEST3 chip test register 3 */
    volatile UINT     *pTemp;        /* TEMP temporary holding register */
    volatile UINT8    *pDfifo;       /* DFIFO DMA FIFO control register */
    volatile UINT8    *pCtest4;      /* CTEST4 chip test register 4 */
    volatile UINT8    *pCtest5;      /* CTEST5 chip test register 5 */
    volatile UINT8    *pCtest6;      /* CTEST6 chip test register 6 */
    volatile UINT     *pDbc;	     /* DCMD SIOP command reg (24Bits Reg) */
    volatile UINT8    *pDcmd;	     /* DBC SIOP command register 8bits  */
    volatile UINT     *pDnad;        /* DNAD DMA buffer ptr (next address) */
    volatile UINT     *pDsp;         /* DSP SIOP scripts pointer register */
    volatile UINT     *pDsps;        /* DSPS SIOP scripts ptr save reg */
    volatile UINT8    *pScratcha0;   /* SCRATCHA0 general purpose scratch reg */
    volatile UINT8    *pScratcha1;   /* SCRATCHA1 general purpose scratch reg */
    volatile UINT8    *pScratcha2;   /* SCRATCHA2 general purpose scratch reg */
    volatile UINT8    *pScratcha3;   /* SCRATCHA3 general purpose scratch reg */
    volatile UINT8    *pDmode;       /* DMODE DMA operation mode register */
    volatile UINT8    *pDien;        /* DIEN DMA interrupt enable */
    volatile UINT8    *pDwt;         /* DWT DMA watchdog timer register */
    volatile UINT8    *pDcntl;       /* DCTNL DMA control register */
    volatile UINT     *pAdder;	     /* ADDER Adder output Register */
    volatile UINT8    *pSien0;       /* SIEN0 SCSI interrupt enable Zero reg */
    volatile UINT8    *pSien1;       /* SIEN1 SCSI interrupt enable One reg */
    volatile UINT8    *pSist0;       /* SIST0 SCSI interrupt status Zero reg */
    volatile UINT8    *pSist1;       /* SIST1 SCSI interrupt status One reg */
    volatile UINT8    *pSlpar;       /* SLPAR SCSI longitudinal Parity reg */
    volatile UINT8    *pSwide;       /* NCR825 specific: SCSI Wide Residue */
    volatile UINT8    *pMacntl;      /* MACNTL Memory Access Control reg */
    volatile UINT8    *pGpcntl;      /* GPCNTL General Purpose Pin Control */
    volatile UINT8    *pStime0;      /* STIME0 SCSI Timer Zero reg */
    volatile UINT8    *pStime1;      /* STIME1 SCSI Timer One reg */
    volatile UINT8    *pRespid;      /* RESPID Response ID reg */
    volatile UINT8    *pStest0;      /* STEST0 SCSI Test Zero reg */
    volatile UINT8    *pStest1;      /* STEST1 SCSI Test One reg */
    volatile UINT8    *pStest2;      /* STEST2 SCSI Test Two reg */
    volatile UINT8    *pStest3;      /* STEST3 SCSI Test Three reg */

    volatile UINT8    *pStest4;      /* STEST4 SCSI Test Four reg */

    volatile UINT16   *pSidl;        /* SIDL SCSI input data latch reg */
    volatile UINT16   *pSodl;        /* SCSI output data latch reg */
    volatile UINT16   *pSbdl;        /* SBDL SCSI bus data lines register */
    volatile UINT     *pScratchb;    /* SCRATCHB Scratch Register B */

    volatile UINT8    *siopRamBaseAdrs; /* ram base address of SIOP */

    } NCR_810_SCSI_CTRL;

/* END PRIVATE */

/* Data stream swapping macros THESE ARE NOW OBSOLETE */

#if (_BYTE_ORDER == _BIG_ENDIAN)
#   define BYTE_SWAP_32_BIT(x)     LONGSWAP(x)
#   define BYTE_SWAP_16_BIT(x)     ((LSB(x) << 8) | MSB(x))
#else
#   define BYTE_SWAP_32_BIT(x)    (x)
#   define BYTE_SWAP_16_BIT(x)    (x)
#endif 

/* Bit Registers definitions for ncr810 */

/* SCNTL0 */

#define    SCNTL0_ARB1    0x80	/*     Arbitration bit1 and bit0, where  */
#define    SCNTL0_ARB0    0x40	/*     00 - simple arb; 11 - full arb.   */
#define    SCNTL0_START   0x20	/* Start sequence */
#define    SCNTL0_WATN    0x10	/* Select w/wo atn */
#define    SCNTL0_EPC     0x08       /* Parity checking */
#define    SCNTL0_AAP     0x02	/* Assert ATN on parity error */
#define    SCNTL0_TRG     0x01	/* Target/initiator mode */

/* SCNTL1 */

#define    SCNTL1_EXC     0x80	/* Extra Data Set up */
#define    SCNTL1_ADB     0x40	/* Assert Data (SODL) onto scsi */
#define    SCNTL1_DHP     0x20	/* Disable Halt on Parity Error or ATN (targ) */
#define    SCNTL1_CON     0x10	/* connected bit status */
#define    SCNTL1_RST     0x08	/* Assert Rst on scsi */
#define    SCNTL1_AESP    0x04	/* Assert even parity -force error */
#define    SCNTL1_IARB    0x02	/* Immediate arbitration */
#define    SCNTL1_SST     0x01	/* Start SCSI Transfer */

/* SCNTL2 */

#define    SCNTL2_SDU     0x80  /* SCSI Disconnect Unexpected */
#define    SCNTL2_WSR     0x01  /* Wide SCSI Receive */
 
/* SCNTL3 */

#define    SCNTL3_ULTRA   0x80  /* Ultra Enable */
#define    SCNTL3_SCF2    0x40  /* Syncronous Clock Conversion Factor bits */
#define    SCNTL3_SCF1    0x20
#define    SCNTL3_SCF0    0x10
#define    SCNTL3_EWS  	  0x08	/* Enable WIDE SCSI */
#define    SCNTL3_CCF2    0x04	/* Clock Conversion Factor bits */
#define    SCNTL3_CCF1    0x02	
#define    SCNTL3_CCF0    0x01	
#define    SCNTL3_SCF_MASK 0x70 /* Sync Clock Conv Factor bit mask */
#define    SCNTL3_CCF_MASK 0x07 /* Clock Conv Factor bit mask */

/* SCID */

#define    SCID_RRE     0x40	/* Enable Response to Reselection */
#define    SCID_SRE     0x20       /* Enable Response to Selection   */
#define    SCID_ENC2    0x04       /* Encoded SCSI ID bit 2 */
#define    SCID_ENC1    0x02       /* Encoded SCSI ID bit 1 */
#define    SCID_ENC0    0x01       /* Encoded SCSI ID bit 0 */
#define    SCID_ENC_MASK 0x07	   /* The Encoded SCSI ID mask */

/* SXFER */

#define    SXFER_TP2     0x80	/* Synchronous transfer period 2 */
#define    SXFER_TP1     0x40	/* Synchronous transfer period 1 */
#define    SXFER_TP0     0x20	/* Synchronous transfer period 0 */
#define    SXFER_MO3     0x08	/* Maximun scsi Synchronous transfer offset */
#define    SXFER_MO2     0x04	/* Maximun scsi Synchronous transfer offset */
#define    SXFER_MO1     0x02	/* Maximun scsi Synchronous transfer offset */
#define    SXFER_MO0     0x01	/* Maximun scsi Synchronous transfer offset */
#define    SXFER_OFFSET  0x0f

/* SDID */

#define    SDID_ENC2     0x04   /* Destination SCSI ID bit 2 */
#define    SDID_ENC1     0x02   /* Destination SCSI ID bit 1 */
#define    SDID_ENC0     0x01   /* Destination SCSI ID bit 0 */
#define    SDID_ENC_MASK 0x07	   /* The Encoded SCSI ID mask */

/* GPREG */

/* ??? */

/* SFBR */

/* No need to specify bits */

/*  SOCL (RW) and SBCL (RO not latched) */

#define    B_REQ     0x80	/* Assert scsi req */
#define    B_ACK     0x40	/* Assert scsi ack */
#define    B_BSY     0x20	/* Assert scsi busy */

/*#define    B_SEL     0x10*/	/* Assert scsi sel */

#define    B_ATN     0x08	/* Assert scsi atn */
#define    B_MSG     0x04	/* Assert scsi msg */
#define    B_CD      0x02	/* Assert scsi c/d */
#define    B_IO      0x01	/* Assert scsi i/o */

/* SSID */

#define    SSID_VAL  0x80	/* SCSI Valid Bit. ENID is valid only if this
				   bit is set. */

#define    SSID_ENC_MASK_7 0x07  /* Binary encoded SCSI ID mask 0-7 */
#define    SSID_ENC_MASK_F 0x0F  /* Binary encoded SCSI ID mask 0-15 */

#if (SCSI_MAX_BUS_ID == SCSI_MAX_BUS_ID_7)
   #define    SSID_ENC_MASK   SSID_ENC_MASK_7 /* SCSI ID mask 0-7 */
#else
   #define    SSID_ENC_MASK   SSID_ENC_MASK_F /* SCSI ID mask 0-15 */
#endif

/* DSTAT (RO) and DIEN (RW enable intr) */

#define    B_DFE     0x80	/* Dma fifo empty */
#define    B_MDPE    0x40       /* Master Data Parity Error */
#define    B_BF      0x20	/* Bus Fault */
#define    B_ABT     0x10	/* Abort condition */
#define    B_SSI     0x08	/* Scsi Step Interrupt */
#define    B_SIR     0x04	/* Script Interrupt received */
#define    B_IID     0x01	/* Illegal Instruction Detected */

/* SSTAT0 */

#define    SSTAT0_ILF 0x80      /* SIDL Input Latch Full */
#define    SSTAT0_ORF 0x40      /* SODR Output Register Full */
#define    SSTAT0_OLF 0x20      /* SODL Output Latch Full */
#define    SSTAT0_AIP 0x10      /* Arbitration In Progress */ 
#define    SSTAT0_LOA 0x01      /* LOst Arbitration */
#define    SSTAT0_WOA 0x02	/* WOn Arbitration */
#define    SSTAT0_RST 0x04    	/* SCSI ReSeT signal in the ISTAT register */
#define    SSTAT0_SDP 0x08 	/* SCSI SDP/ parity signal */

/* SSTAT1 RO register */

#define    SSTAT1_FF3     0x80	/* Fifo flag 3 :number of bytes in fifo */
#define    SSTAT1_FF2     0x40	/* Fifo flag 2 :number of bytes in fifo */
#define    SSTAT1_FF1     0x20	/* Fifo flag 1 :number of bytes in fifo */
#define    SSTAT1_FF0     0x10	/* Fifo flag 0 :number of bytes in fifo */
				/* 0000=0 ..... 1000=8 */
#define    SSTAT1_SPDL    0x08	/* Latched parity line */
#define    SSTAT1_MSGL    0x04	/* Latched state of MSG line */
#define    SSTAT1_CDL     0x02	/* Latched state of C/D line */
#define    SSTAT1_IOL     0x01	/* Latched sate of IO line */
#define    FIFO_MASK      0xf0

/* SSTAT2 */

#define    SSTAT2_ILF  0x80     /* SIDL MSB full */
#define    SSTAT2_ORF  0x40     /* SODR MSB full */
#define    SSTAT2_OLF  0x20     /* SODL MSB full */
#define    SSTAT2_FF4  0x10     /* FIFO flags bit 4 */
#define    SSTAT2_SPL  0x08     /* Latched SCSI Parity */
#define    SSTAT2_DSP  0x04     /* differential sense pin */
#define    SSTAT2_LDSC 0x02 	/* Last Disconnect; used in conjunction with
				   CON bit in SCNTL1 */
#define    SSTAT2_SDP  0x01 	/* SCSI SDP/ parity signal */


/* ISTAT RW register */

#define    ISTAT_ABRT    0x80	/* Abort operation */
#define    ISTAT_SOFTRST 0x40	/* Soft chip reset */
#define    ISTAT_SIGP    0x20	/* signal process */
#define    ISTAT_SEM     0x10  	/* Semaphore */
#define    ISTAT_CON     0x08	/* stat connected (reset doesn't disconnect) */
#define    ISTAT_INTF    0x04  	/* set by INTFLY script instruction. Signals 
				   ISRs while scripts are still running     */
#define    ISTAT_SIP     0x02	/* SCSI Interrupt Pending */
#define    ISTAT_DIP     0x01	/* Dma Interrupt Pending */

/* CTEST1 */

/* 
 * FMT? bits define the bottom bytes in the FIFO that are empty. ? specifies
 * the byte lane number. If byte lane3 is empty then FMT3 bit is set. All
 * 1's indicate an empty FIFO.
 */

#define    CTEST1_FMT3   0x80	/* Byte lane 3 empty? */
#define    CTEST1_FMT2   0x40	/* Byte lane 2 empty? */
#define    CTEST1_FMT1   0x20	/* Byte lane 1 empty? */
#define    CTEST1_FMT0   0x10	/* Byte lane 0 empty? */

/*
 * FFL? bits define the top bytes in the FIFO that are full. ? specifies the
 * byte lane number. If byte lane3 is full then FFL3 is set. All 1's indicate
 * a full FIFO.
 */

#define    CTEST1_FFL3   0x08	/* Byte lane 3 full? */
#define    CTEST1_FFL2   0x04	/* Byte lane 2 full? */
#define    CTEST1_FFL1   0x02	/* Byte lane 1 full? */
#define    CTEST1_FFL0   0x01	/* Byte lane 0 full? */

/* CTEST2 */

#define    CTEST2_DDIR   0x80 	/* Data Direction. 1 => From SCSI to board */
#define    CTEST2_SIGP   0x40	/* Signal Process. Same as in ISTAT. When this
				   register is read SIGP is cleared in ISTAT */
#define    CTEST2_CIO	 0x20   /* Configured as I/O XXX ??? */
#define    CTEST2_CM     0x10	/* Configured as Memory XXX ??? */
#define    CTEST2_TEOP   0x04   /* SCSI true end of process */
#define    CTEST2_DREQ   0x02 	/* 1 => DREQ is active */
#define    CTEST_DACK    0x01 	/* 1 => DACK is inactive */

/* CTEST3 */

#define    CTEST3_V3      0x80	/* Chip Revision Bit 3 */
#define    CTEST3_V2      0x40	/* Chip Revision Bit 2 */
#define    CTEST3_V1      0x20	/* Chip Revision Bit 1 */
#define    CTEST3_V0      0x10	/* Chip Revision Bit 0 */
#define    CTEST3_FLF     0x08	/* Flush DMA FIFO */
#define    CTEST3_CLF     0x04	/* Clear DMA FIFO */
#define    CTEST3_FM      0x02	/* Fetch Pin mode */
#define    CTEST3_WRIE    0x01 	/* Write and Invalidate Enable */

/* DFIFO */

#define    DFIFO_BO_MASK  0x7f  /* 7 bits of SCSI Byte Offset counter between 
				   SCSI core and DMA core */

/* CTEST4 */

#define    CTEST4_BDIS    0x80	/* Burst transfer DISable */
#define    CTEST4_ZMOD    0x40	/* Host bus high impedance mode  */
#define    CTEST4_SZD     0x20	/* Scsi data high impedance mode */
#define    CTEST4_SRTM    0x10	/* Shadow Register Test Mode */
#define    CTEST4_MPEE    0x08	/* Master Parity Error Enable */
#define    CTEST4_FBL2    0x04	/* Enable bytes lane */
#define    CTEST4_FLB1    0x02	/* Bit one mux byte lane */
#define    CTEST4_FLB0    0x01	/* Bit zero mux byte lane */

/* CTEST5 */

#define    CTEST5_ADCK    0x80	/* Increment DNAD register */
#define    CTEST5_BBCK    0x40	/* Decrement DBC register */
#define    CTEST5_DFS     0x20  /* DMA FIFO Size: 0=88, 1=536 bytes */
#define    CTEST5_MASR    0x10	/* set/reset values for bit 3 */
#define    CTEST5_DDIR    0x08	/* control internel DMAWR (scsi to host) */
#define    CTEST5_EOP     0x04	/* control internal EOP (DMA/SCSI) */
#define    CTEST5_DREQ    0x02	/* control internal DREQ */
#define    CTEST5_DACK    0x01	/* control internal DACK (DMA/SCSI) */

/* CTEST6 */

#define    CTEST6_DF_MASK 0xff 	/* Dma Fifo bits - mask */

/* DMODE RW register */

#define    DMODE_BL1     0x80	/* Burst Length transfer bits; determine the */
#define    DMODE_BL0     0x40	/* number of transfers per bus ownership */
				/* 00=2,01=4,10=8,11=16 */
#define    DMODE_SIOM    0x20	/* Source I/O Memory Enable; 1 => I/O space */
#define    DMODE_DIOM    0x10 	/* Destination I/O Memory Enable; "  "  "   */
#define    DMODE_ERL     0x08	/* Enable Read Line; */
#define    DMODE_ERMP    0x04	/* Enable Read Multiple */
#define    DMODE_BOF     0x02	/* Burst Opcode Fetch enable; instrs fetched 
				   in burst mode */
#define    DMODE_MAN     0x01	/* Manual start mode ,when set disable */
				/* autostart script when writting in DSP */

/* DCNTL - DMA Control bits */

#define    DCNTL_CLSE    0x80	/* Cache Line Size Enable */
#define    DCNTL_PFF     0x40   /* Pre-Fetch Flush */
#define    DCNTL_PFEN    0x20   /* Pre-Fetch ENable */ 
#define    DCNTL_SSM     0x10  	/* Single-Step Mode */
#define    DCNTL_IRQM    0x08	/* IRQ Mode */
#define    DCNTL_STD     0x04  	/* STart Dma operation; instr. is in DSP */
#define    DCNTL_IRQD    0x02	/* IRQ Disable; 1 => IRQ/ wont be asserted */
#define    DCNTL_COM     0x01 	/* Compatability XXX ??? 1 => ncr700 mode ?? */

/* SIEN0 (enable intr)and SIST0 (RO Status) */

/* Enable interrupt */

#define    B_MA      0x80	/* Phase Mismatch; initiator interrupt occurs */
#define    B_CMP     0x40	/* Funtion CoMPlete; interrupt when full 
				   arbitration and selection is complete */
#define    B_SEL     0x20	/* Selected; intr when selected as target */
#define    B_RSL     0x10	/* Reselected; intr when reselected */
#define    B_SGE     0x08	/* SCSI Gross Error; intr enable */
#define    B_UDC     0x04	/* Enable Unexpected Disconnect intr */
#define    B_RST     0x02	/* Enable Reset intr */
#define    B_PAR     0x01	/* Enable parity intr */

/* SIEN1 (enable intr) and SIST1 (RO status) */

#define    B_STO     0x04	/* Selection/Reselection Timeout; intr enable */
#define    B_GEN     0x02	/* GENeral purpose timer expired; intr enable */
#define    B_HTH     0x01	/* Handshake to Handshake timer expired; intr */

/* MACNTL - Memory Access Control (RW) */

#define    MACNTL_TYP_MASK 0xf0	/* Identify the chip TYPe */
#define    MACNTL_DWR      0x08	/* Data WRite is local memeory access */
#define    MACNTL_DRD      0x04	/* Data ReaD is local memory access */
#define    MACNTL_PSCPT    0x02	/* Pointer to SCRIPTs is local mem access */
#define    MACNTL_SCPTS    0x01	/* SCRIPTS fetch is local mem access */

/* GPCNTL - General Purpose Pin Control (RW) */

/* This reg is used to determine if GPREG pins are inputs or outputs */

#define    GPCNTL_ME    0x80	/* Master Enable; internal bus master is GPIO1*/
#define    GPCNTL_FE    0x40	/* Fetch Enable; internal op-code fetch GPIO0 */
#define    GPCNTL_GPIO1 0x02 	/* GPIO1: set - input and reset - output */
#define    GPCNTL_GPIO0 0x01 	/* GPIO0: set - input and reset - output */

/* STIME0  SCSI Timer Zero (RW) */

/* These bits select various timeout values.e.g 0000 => disable; 1111 => 1.6s */

#define    STIME0_HTH_MASK 0xf0	/* Handshake-To-Handshake timer period mask */
#define    STIME0_SEL_MASK 0x0f	/* SELection timeout mask */

/* STIME1 SCSI Timer One (RW) */

#define    STIME1_GEN_MASK 0x0f	/* GENeral timer timeout mask */

/* STEST0 SCSI Test Zero (RO) */

#define  STEST0_SSAID_MASK 0x70	/* SCSI Selected As ID */
#define  STEST0_SLT        0x08	/* Selection response logic test */
#define  STEST0_ART	   0x04	/* Arbitation Priority encoder Test */
#define  STEST0_SOZ	   0x02	/* SCSI Synchronous Offset Zero */
#define  STEST0_SOM	   0x01	/* SCSI Synchronous Offset Maximum */

/* STEST1 SCSI Test One (RW) */

#define     STEST1_SCLK  0x80	/* Disables external SCLK and uses PCI internal
				   SCSI clock */
#define     STEST1_SISO	 0x40	/* SCSI Isolation Mode; inputs isolated from
	    			   the SCSI bus */
#define     STEST1_QEN   0x08   /* Quadrupler Enable: Power on clock Quadrupler */
#define     STEST1_QSEL  0x04   /* Quadrupler Select: Increase clock to 160 MHz */

/* STEST2 SCSI Test Two (RW) */

#define     STEST2_SCE   0x80	/* SCSI Control Enable */
#define     STEST2_ROF   0x40  	/* Reset SCSI OFfset */

#define     STEST2_DIF   0x20   /* NCR825 specific: SCSI Differential Mode */
#define     STEST2_AWS   0x04   /* NCR825 specific: Always Wide SCSI */

#define     STEST2_SLB   0x10 	/* SCSI Loopback Mode */
#define     STEST2_SZM 	 0x08	/* SCSI High Impedence Mode */
#define     STEST2_EXT	 0x02	/* Extend SREQ/SACK filtering - Should not be
				   set during FAST SCSI > 5MB/s */
#define     STEST2_LOW	 0x01	/* SCSI LOW level mode; no DMA operation occur
				   and no SCRIPTS execute */

/* STEST3 SCSI Test Three (RW) */

#define     STEST3_TE    0x80	/* Tolerant Enable */ 
#define     STEST3_STR   0x40	/* SCSI FIFO Test Read */
#define     STEST3_HSC	 0x20	/* Halt Scsi Clock */
#define     STEST3_DSI	 0x10	/* Disable Single Initiator response (SCSI-1) */
#define     STEST3_TTM	 0x04	/* Timer Test Mode */
#define     STEST3_CSF   0x02	/* Clear Scsi Fifo */
#define     STEST3_STW   0x01	/* SCSI Fifo Test Write */

/* STEST4 SCSI Test Four (RW) */

#define     STEST4_SMODE1    0x80	/* SCSI Mode Detect (upper bit) */
#define     STEST4_SMODE0    0x40	/* SCSI Mode Detect (lower bit) */
#define     STEST4_LOCK      0x20	/* Detect Clock Quadrupler Lockup */


/* Two types of register base address. Memory I/O and Cofig base addresses */

#define MEMIO_REG_BASE  (0x00)
#define CONFIG_REG_BASE (0x80)
#define BASE MEMIO_REG_BASE

/* _BYTE_ORDER is LITTLE_ENDIAN for PCI devices */

#define OFF_SCNTL0      (BASE) 		/* SCTNL0 SCSI control register 0 */
#define OFF_SCNTL1      (BASE + 0x01)	/* SCTNL1 SCSI control register 1 */
#define OFF_SCNTL2      (BASE + 0x02)	/* SCTNL2 SCSI control register 2 */
#define OFF_SCNTL3      (BASE + 0x03)	/* SCTNL3 SCSI control register 3 */
#define OFF_SCID	(BASE + 0x04)	/* SCID SCSI chip ID register */
#define OFF_SXFER	(BASE + 0x05)	/* SXFER SCSI chip ID regiester */
#define OFF_SDID	(BASE + 0x06)	/* SDID SCSI destination ID register */
#define OFF_GPREG	(BASE + 0x07)	/* GPREG General Purpose register */
#define OFF_SFBR	(BASE + 0x08)	/* SFBR SCSI first byte received reg */
#define OFF_SOCL	(BASE + 0x09) 	/* SOCL SCSI output control latch reg */
#define OFF_SSID	(BASE + 0x0a)	/* SSID SCSI selector ID */
#define OFF_SBCL	(BASE + 0x0b)	/* SBCL SCSI bus control lines reg */
#define OFF_DSTAT 	(BASE + 0x0c)	/* DSTAT DMA status register */
#define OFF_SSTAT0	(BASE + 0x0d)	/* SSTAT0 SCSI status register 0 */
#define OFF_SSTAT1	(BASE + 0x0e)	/* SSTAT1 SCSI status register 0 */
#define OFF_SSTAT2	(BASE + 0x0f)	/* SSTAT2 SCSI status register 0 */
#define OFF_DSA 	(BASE + 0x10)	/* DSA data structure address */
#define OFF_ISTAT   	(BASE + 0x14)	/* ISTAT interrupt status register */
#define OFF_CTEST0	(BASE + 0x18)	/* CTEST0 chip test register 0 */
#define OFF_CTEST1	(BASE + 0x19)	/* CTEST1 chip test register 1 */
#define OFF_CTEST2	(BASE + 0x1a)	/* CTEST2 chip test register 2 */
#define OFF_CTEST3	(BASE + 0x1b)	/* CTEST3 chip test register 3 */
#define OFF_TEMP  	(BASE + 0x1c)	/* TEMP temporary holding register */
#define OFF_DFIFO	(BASE + 0x20)	/* DFIFO DMA FIFO control register */
#define OFF_CTEST4	(BASE + 0x21)	/* CTEST4 chip test register 4 */
#define OFF_CTEST5	(BASE + 0x22)	/* CTEST5 chip test register 5 */
#define OFF_CTEST6	(BASE + 0x23)	/* CTEST6 chip test register 6 */
#define OFF_DBC 	(BASE + 0x24) 	/* DBC SIOP command register 24bits  */
#define OFF_DCMD	(BASE + 0x27)	/* DCMD SIOP command reg (8Bits Reg) */
#define OFF_DNAD	(BASE + 0x28)	/* DNAD DMA buffer ptr (next address) */
#define OFF_DSP 	(BASE + 0x2c)	/* DSP SIOP scripts pointer register */
#define OFF_DSPS	(BASE + 0x30)	/* DSPS SIOP scripts ptr save reg */
#define OFF_SCRATCHA0	(BASE + 0x34)	/* SCRATCHA0 gen purpose scratch reg */
#define OFF_SCRATCHA1	(BASE + 0x35)	/* SCRATCHA1 gen purpose scratch reg */
#define OFF_SCRATCHA2	(BASE + 0x36)	/* SCRATCHA2 gen purpose scratch reg */
#define OFF_SCRATCHA3	(BASE + 0x37)	/* SCRATCHA3 gen purpose scratch reg */
#define OFF_DMODE	(BASE + 0x38)	/* DMODE DMA operation mode register */
#define OFF_DIEN 	(BASE + 0x39)	/* DIEN DMA interrupt enable */
#define OFF_DWT  	(BASE + 0x3a)	/* DWT DMA watchdog timer register */
#define OFF_DCNTL	(BASE + 0x3b)	/* DCTNL DMA control register */	
#define OFF_ADDER	(BASE + 0x3c) 	/* ADDER Adder output Register */	
#define OFF_SIEN0	(BASE + 0x40)	/* SIEN0 SCSI interrupt enable 0 reg */
#define OFF_SIEN1	(BASE + 0x41)	/* SIEN1 SCSI interrupt enable 1 reg */
#define OFF_SIST0	(BASE + 0x42)	/* SIST0 SCSI interrupt status 0 reg */
#define OFF_SIST1	(BASE + 0x43)	/* SIST1 SCSI interrupt status 1 reg */
#define OFF_SLPAR	(BASE + 0x44)	/* SLPAR SCSI longitudinal Parity reg */
#define OFF_SWIDE	(BASE + 0x45)	/* NCR825: SWIDE SCSI Wide Residue */
#define OFF_MACNTL	(BASE + 0x46)	/* MACNTL Memory Access Control reg */
#define OFF_GPCNTL	(BASE + 0x47)	/* GPCNTL General Purpose Pin Control */
#define OFF_STIME0	(BASE + 0x48)	/* STIME0 SCSI Timer Zero reg */
#define OFF_STIME1	(BASE + 0x49)	/* STIME1 SCSI Timer One reg */
#define OFF_RESPID	(BASE + 0x4a)	/* RESPID Response ID reg */
#define OFF_STEST0	(BASE + 0x4c)	/* STEST0 SCSI Test 0 reg */
#define OFF_STEST1	(BASE + 0x4d)	/* STEST1 SCSI Test 1 reg */
#define OFF_STEST2	(BASE + 0x4e)	/* STEST2 SCSI Test 2 reg */
#define OFF_STEST3	(BASE + 0x4f)	/* STEST3 SCSI Test 3 reg */
#define OFF_SIDL	(BASE + 0x50)	/* SIDL SCSI input data latch reg */

#define OFF_STEST4	(BASE + 0x52)	/* STEST4 SCSI Test 4 reg */

#define OFF_SODL 	(BASE + 0x54)	/* SCSI output data latch reg */
#define OFF_SBDL 	(BASE + 0x58)	/* SBDL SCSI bus data lines register */
#define OFF_SCRATCHB	(BASE + 0x5c)	/* SCRATCHB Scratch Register B */

#define OFF_SCRATCHC	(BASE + 0x60)	/* SCRATCHC Scratch Register C */
#define OFF_SCRATCHD	(BASE + 0x64)	/* SCRATCHD Scratch Register D */
#define OFF_SCRATCHE	(BASE + 0x68)	/* SCRATCHE Scratch Register E */
#define OFF_SCRATCHF	(BASE + 0x6c)	/* SCRATCHF Scratch Register F */
#define OFF_SCRATCHG	(BASE + 0x70)	/* SCRATCHG Scratch Register G */
#define OFF_SCRATCHH	(BASE + 0x74)	/* SCRATCHH Scratch Register H */
#define OFF_SCRATCHI	(BASE + 0x78)	/* SCRATCHI Scratch Register I */
#define OFF_SCRATCHJ	(BASE + 0x7C)	/* SCRATCHJ Scratch Register J */

#define NCR810_DEVICE_ID   0x0001
#define NCR825_DEVICE_ID   0x0003
#define NCR875_DEVICE_ID   0x000f
#define NCR895_DEVICE_ID   0x000c
#define SYM895_DEVICE_ID   NCR895_DEVICE_ID
#define PCI_ID_SYMBIOS 0x1000

/*
 * NOTICE: The macro PCI_TO_MEM_OFFSET is not to be used anymore.  It
 * is a BSP specific value, not an architectural value.  The BSP should
 * update the value ncr810PciMemOffset with the appropriate offset value.
 */

/*
 * When memory is accessed from the PCI bus, the addresses need to contain this
 * offset
 *
 * MAKE NO FURTHER CHANGES HERE SEE SPR 8540
 */ 

#ifndef PCI_TO_MEM_OFFSET
#   if (CPU == I960JX)
#	define PCI_TO_MEM_OFFSET  0x00000000
#   else
#	define PCI_TO_MEM_OFFSET 0x80000000
#   endif
#endif


/* Mask Values */

#define NCR810_COUNT_MASK    ((UINT)0x00ffffff) /* Mask 24 bit value in block */
                                                /* move description */
/* Shift values for "device" field in shared data structure */

#define NCR810_TARGET_BUS_ID_SHIFT  16	/* bits 23-16: specify target bus ID */
#define	NCR810_XFER_PARAMS_SHIFT     8	/* bits 15-08: copy of sxfer reg.    */
 
/* Sync offset and period (SXFER register) */

#define NCR810_MIN_REQ_ACK_OFFSET    1	/* Minimum sync offset        */
#define NCR810_MAX_REQ_ACK_OFFSET    8	/* Maximum sync offset        */
/* #define NCR810_MAX_REQ_ACK_OFFSET   31 /@ Maximum sync offset 875/895*/
#define NCR810_MIN_XFER_PERIOD      0	/* Minimum sync period (clks) */
#define NCR810_MAX_XFER_PERIOD      7	/* Maximum sync period (clks) */
#define NCR810_MIN_XFERP            4	/* Minimum value of xferp */
#define NCR810_MAX_XFERP           11 	/* Maximum value of xferp */

#define	NCR810_SYNC_XFER_PERIOD_SHIFT 	5

#define	NCR810_SYNC_XFER_PARAMS_ASYNC	0x00

/* Synchronous transfer clock division factor (SBCL register bits 1,0 (w-o) */

#define	NCR810_MIN_CLOCK_DIV	    	0x00
#define	NCR810_MAX_CLOCK_DIV	    	0x03
#define	NCR810_CLOCK_DIVIDE_ASYNC   	0x00

/* Clock conversion factor */

/* prescale factor for asynchronous scsi core (scntl3) */

#define NCR810_16MHZ_ASYNC_DIV        0x01    /* 16.67-25.00Mhz input clock */
#define NCR810_25MHZ_ASYNC_DIV        0x02    /* 25.01-37.50Mhz input clock */
#define NCR810_50MHZ_ASYNC_DIV        0x03    /* 37.51-50.00Mhz input clock */
#define NCR810_66MHZ_ASYNC_DIV        0x04    /* 50.01-66.00Mhz input clock */

#define NCR810_75MHZ_ASYNC_DIV        0x04    /* 50.01-75.00Mhz input clock */

/*
 * Note: 80 Mhz value also has bit for clock doubler
 * set. We have to enable clock doubler
 * as we enabled the clock quadrupler. haven't included support
 * for clock doubler yet.
 *
 * Note: When Ultra enable bit is set, the Tolerant Enable bit must be set
 * also. It is STEST3:TE (bit 7).
 *
 */

#define NCR810_80MHZ_ASYNC_DIV        0x85    /* 75.01-80.00Mhz input clock */

/* note 120 MHz normally not used */

#define NCR810_160MHZ_ASYNC_DIV       0x87    /* 160.00 Mhz input clock with 
                                                clock quadrupler and 40 Mhz 
                                                input clock                 */

/* ns x 100 clock period */

#define NCR810_1667MHZ  6000    /* 16.67Mhz chip */
#define NCR810_20MHZ    5000    /* 20Mhz chip */
#define NCR810_25MHZ    4000    /* 25Mhz chip */
#define NCR810_3750MHZ  2667    /* 37.50Mhz chip */
#define NCR810_40MHZ    2500    /* 40Mhz chip */
#define NCR810_50MHZ    2000    /* 50Mhz chip */
#define NCR810_66MHZ    1515    /* 66Mhz chip */
#define NCR810_6666MHZ  1500    /* 66Mhz chip */
#define NCR810_75MHZ    1333    /* 75Mhz chip */
#define NCR810_80MHZ    1250    /* 80Mhz chip */
#define NCR810_160MHZ    625    /* 40Mhz chip with Quadrupler */


#define NCR810_MAX_SYNC_PERIOD   NCR810_50MHZ
#define NCR810_MAX_ASYNC_PERIOD  NCR810_25MHZ

/* for ncr875/895
#define NCR810_MAX_SYNC_PERIOD   NCR810_160MHZ
#define NCR810_MAX_ASYNC_PERIOD  NCR810_4MHZ
*/


#define NCR810_MAX_XFER_WIDTH 1	/* in transfer width exponent units. 16bits */

/* Chip Type */

#define NCR700_TYPE	0x700   /* not supported */
#define NCR810_TYPE	0x810   /* supported */
#define NCR720_TYPE	0x720   /* not supported */

/*
 * Default value to initialize some registers involved
 * in the hardware implementation.Those values could be
 * overwritten by a bsp call with the ncr810HwSetRegister().
 * The ncr810 Data Manual documentation will you give all
 * the light regarding values choose. See also the
 * documentation of ncr810SetHwRegister call.
 * See above the NCR810_HW_REGS for the meaning of the
 * values prefill.
 */

#define DEFAULT_810_HW_REGS {0,0,0,0,0,1,0,0,0,0,0}

/* function declarations */

IMPORT  NCR_810_SCSI_CTRL * ncr810CtrlCreate (
				UINT8 *siopBaseAdrs,
				UINT   clkPeriod, 
				UINT16 devType);

IMPORT  NCR_810_SCSI_CTRL * ncr810CtrlCreate2 (
				UINT8 *siopBaseAdrs,
				UINT   clkPeriod, 
				UINT16 devType,
				UINT8 *siopRamBaseAdrs,
				UINT8 *siopIOBaseAdrs);

IMPORT  STATUS  ncr810CtrlInit (NCR_810_SCSI_CTRL *pSiop, 
					int scsiCtrlBusId);
IMPORT	STATUS	ncr810SetHwRegister (NCR_810_SCSI_CTRL *pScsiCtrl,
				    NCR810_HW_REGS *pHwRegs);
IMPORT  void    ncr810Intr  (NCR_810_SCSI_CTRL *pSiop);
IMPORT  STATUS 	ncr810Show  (SCSI_CTRL *pScsiCtrl);
IMPORT  void    ncr810StepEnable (NCR_810_SCSI_CTRL *pSiop, BOOL enable);
IMPORT  void    ncr810SingleStep (NCR_810_SCSI_CTRL *pSiop, BOOL verbose);

#endif	/* _ASMLANGUAGE */
#ifdef __cplusplus
}
#endif

#endif /* __INCncr810h */
