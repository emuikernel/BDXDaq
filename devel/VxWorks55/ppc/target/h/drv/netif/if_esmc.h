/* if_esmc.h - SMC 91c9x network interface header */ 

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,09may96,hdn  written. 
*/


#ifndef	INCif_esmch
#define	INCif_esmch

#ifdef __cplusplus
extern "C" {
#endif


#define	ESMC_BUFSIZE		1536	/* RX/TX buffer size in task level */
#define	ESMC_BUFSIZE_INT 	(1536*6) /* RX buffer size in int level */
#define ESMC_PACKETS		18	/* max number of packets */
#define ESMC_INDEXES		(18*6)	/* max received packets in int level */
#define ESMC_9190		3	/* SMC 91c90 */
#define ESMC_9194		4	/* SMC 91c94 */
#define ESMC_9195		5	/* SMC 91c95 */
#define ESMC_91100		7	/* SMC 91c100 */
#define ESMC_INTR_TIMEOUT	2	/* interrupt timeout */
#define ESMC_ALLOC_TIMEOUT	8	/* mem allocation timeout */
#define	ESMC_BANK_SELECT	0x0E	/* see chip mauual for details */
#define	ESMC_RXING		0x01	/* receving */
#define	ESMC_TXING		0x02	/* transmitting */

typedef struct 
    {
    struct arpcom es_ac;		/* ethernet common part */
#define es_if     es_ac.ac_if
#define es_enaddr es_ac.ac_enaddr
    int	   ioAddr;			/* device I/O address */
    int	   intVec;			/* interrupt vector */
    int	   intLevel;			/* interrupt level */
    int	   config;			/* 10BaseT or AUI */
    int	   mode;			/* 10BaseT or AUI */
    SEMAPHORE  esmcSyncSem;		/* syncronization semaphore */
    char * pBuft;			/* pointer to rx/tx buffer */
    char   buft[ESMC_BUFSIZE + 8];	/* rx/tx buffer + alignment pad */
    char * pBufi;			/* pointer to rx buffer */
    char * bufi;			/* rx buffer + alignment pad */
    char * pStart;			/* start pointer for rx buf */
    char * pIn;				/* input pointer for rx buf */
    char * pOut;			/* output pointer for rx buf */
    char * pEnd;			/* end pointer for rx buf */
    char * packetAddr[ESMC_INDEXES];	/* packet address */
    int	   packetLen[ESMC_INDEXES];	/* packet length */
    int	   indexIn;			/* index for interrupt */
    int	   indexOut;			/* index for output */
    int	   nIndex;			/* number of indexes */
    u_int  interrupt;			/* counter for interrupt */
    u_int  intRcv;			/* counter for interrupt */
    u_int  intTx;			/* counter for interrupt */
    u_int  intTxempty;			/* counter for interrupt */
    u_int  intAlloc;			/* counter for interrupt */
    u_int  intOverrun;			/* counter for interrupt */
    u_int  intEph;			/* counter for interrupt */
    u_int  intErcv;			/* counter for interrupt */
    u_int  lostCarr;			/* lost carr */
    u_int  allocTimeout;		/* allocation timeout */
    u_int  allocFailed;			/* allocation failed */
    char   flags;			/* misc control flags */
    } ESMC_SOFTC;


/* BANK 0 I/O Space mapping */

#define	ESMC_TCR 		0x0	/* transmit control register */
#define	ESMC_EPH 		0x2	/* EPH status register */
#define	ESMC_RCR		0x4	/* receive control register */
#define	ESMC_COUNTER    	0x6	/* counter register */
#define	ESMC_MIR        	0x8	/* memory information register */
#define	ESMC_MCR		0xA	/* memory configuration register */

/* Transmit Control Register */

#define ESMC_TCR_EPH_LOOP	0x2000	/* internal loopback at EPH block */
#define ESMC_TCR_STP_SQET	0x1000	/* stop trans on signal quality error */
#define ESMC_TCR_FDUPLX		0x0800	/* enable full duplex */
#define	ESMC_TCR_MON_CNS	0x0400	/* monitors the carrier while trans */
#define ESMC_TCR_NOCRC		0x0100	/* NOT append CRC to frame if set */
#define ESMC_TCR_PAD		0x0080	/* pad short frames to 64 bytes */
#define ESMC_TCR_FORCOL		0x0040	/* force collision, then auto resets */
#define ESMC_TCR_LOOP		0x0020	/* internal loopback */
#define ESMC_TCR_TXEN		0x0001	/* transmit enabled when set */ 
#define	ESMC_TCR_RESET		0x0000	/* Disable the transmitter */

/* EPH Status Register - stores status of last transmitted frame */

#define ESMC_TS_TXUNRN		0x8000	/* tramsmit under run */
#define ESMC_TS_LINK_OK		0x4000	/* twisted pair link cond */  
#define ESMC_TS_RX_OVRN		0x2000	/* on FIFO overrun... */
#define ESMC_TS_CTR_ROL		0x1000	/* counter rollover */
#define ESMC_TS_EXC_DEF		0x0800	/* excessive deferal */
#define ESMC_TS_LOST_CARR	0x0400	/* ## lost carrier sense */
#define ESMC_TS_LATCOL		0x0200	/* ## late collision */
#define ESMC_TS_TX_DEFR		0x0080	/* transmit deferred - auot cleared */
#define ESMC_TS_LTX_BRD		0x0040	/* last frame was broascast packet */
#define ESMC_TS_SQET		0x0020	/* ## signal quality error test */
#define ESMC_TS_16COL		0x0010	/* ## 16 collisions reached */
#define ESMC_TS_LXT_MULT	0x0008	/* last frame was multicast */
#define ESMC_TS_MULCOL		0x0004	/* multiple collision detected */
#define ESMC_TS_SNGLCOL		0x0002	/* single collision detected */   
#define ESMC_TS_TX_SUC		0x0001	/* last Transmit was successful */
                                 	/* ## indicates fatal error */
/* Recieve control register */

#define ESMC_RCR_EPH_RST	0x8000	/* software activated reset */
#define ESMC_RCR_FILT_CAR	0x4000	/* filter carrier sense 12 bits */           	
#define	ESMC_RCR_STRIP_CRC	0x0200	/* when set, strips CRC */
#define ESMC_RCR_RXEN		0x0100	/* ENABLE reciever */
#define ESMC_RCR_ALMUL		0x0004	/* receive all multicast packets */
#define	ESMC_RCR_PRMS		0x0002	/* enable promiscuous mode */
#define ESMC_RCR_RX_ABORT	0x0001	/* set if reciever overrun */          
#define ESMC_RCR_RESET		0x0	/* set it to a base state */

/* Memory Information Register */

#define ESMC_MIR_FREE		MIR	/* read at any time for free mem */
#define ESMC_MIR_SIZE		MIR+1	/* determine amount of onchip mem */


/* BANK 1 I/O Space mapping */

#define ESMC_CONFIG		0x0	/* configuration register */
#define	ESMC_BASE		0x2	/* base address register */
#define	ESMC_ADDR_0		0x4	/* individual address register 0 */
#define	ESMC_ADDR_1		0x6	/* individual address register 1 */
#define	ESMC_ADDR_2		0x8	/* individual address register 2 */
#define	ESMC_GENERAL		0xA	/* general purpose register */
#define	ESMC_CONTROL		0xC	/* control register */

/* Configuration register */

#define ESMC_CFG_NO_WAIT_ST	0x1000	/* when set */
#define ESMC_CFG_FULL_STEP	0x0400	/* AUI, use full step signaling */
#define ESMC_CFG_SET_SQLCH	0x0200	/* when set squelch level is 240mV */
#define ESMC_CFG_AUI_SELECT	0x0100	/* when set use AUI */
#define ESMC_CFG_16BIT		0x0080	/* usually auto set   16/8 selection */ 
#define ESMC_CFG_DIS_LINK	0x0040	/* disables link test, TP only */
#define ESMC_INT_INTR_ZERO	0xf9	/* mask to clear interrrupt setting */
#define ESMC_INT_SEL_INTR0	0x0
#define ESMC_INT_SEL_INTR1	0x2
#define ESMC_INT_SEL_INTR2	0x4
#define ESMC_INT_SEL_INTR3	0x6	

/* Control Register */

#define ESMC_CTL_RCV_BAD	0x4000	/* when set bad CRC packets received */
#define	ESMC_CTL_PWEDN		0x2000	/* high puts chip in powerdown mode */
#define ESMC_CTL_AUTO_RELEASE	0x0800	/* transmit memory auto released */
#define	ESMC_CTL_LE_ENABLE	0x0080	/* Link error enable */
#define	ESMC_CTL_CR_ENABLE	0x0040	/* counter rollover enable */
#define	ESMC_CTL_TE_ENABLE	0x0020	/* transmit error enable */
#define	ESMC_CTL_EPROM_SELECT	0x0004	/* EEprom access bit */


/* BANK 2 I/O Space mapping */

#define ESMC_MMU		0x0	/* MMU command register */
#define	ESMC_PNR		0x2	/* packet number register */	
#define ESMC_ARR		0x3	/* allocation result register */
#define ESMC_FIFO		0x4	/* FIFO ports register */
#define	ESMC_PTR		0x6	/* pointer register */
#define	ESMC_DATA_1		0x8	/* data register 1 */
#define	ESMC_DATA_2		0xA	/* data register 2 */
#define	ESMC_INT_STAT		0xC	/* int status & acknowledge register */
#define	ESMC_INT_ACK		0xC	/* int status & acknowledge register */
#define ESMC_INT_MASK		0xD	/* int mask register */

/* MMU Command Register */

#define ESMC_MMU_BUSY		0x0001	/* set if MMU busy */
#define	ESMC_MMU_ALLOC		0x0020	/* get memory from chip memory/256 */
#define ESMC_MMU_NOP		0x0000
#define	ESMC_MMU_RESET		0x0040	
#define	ESMC_MMU_RX_REMOVE	0x0060	/* remove current RX frame from FIFO */
#define ESMC_MMU_RX_RELEASE	0x0080	/* also release memory associated */
#define ESMC_MMU_TX_RELEASE	0x00A0	/* free packet memory in PNR register */
#define ESMC_MMU_ENQUEUE	0x00C0	/* Enqueue the packet for transmit */
#define ESMC_MMU_TX_RESET	0x00E0	/* reset TX FIFO's */

/* Allocation Result Register */

#define ESMC_ARR_FAILED		0x80

/* FIFO Ports Register */

#define ESMC_FP_RXEMPTY		0x8000	/* no rx packets queued in RX FIFO */
#define ESMC_FP_TXEMPTY		0x0080	/* no tx packets queued in TX FIFO  */

/* Pointer Register - address to be accessed in chip memory*/

#define	ESMC_PTR_RCV		0x8000	/* when set, refers to receive area */
#define ESMC_PTR_READ		0x2000	/* type of access to follow */
#define	ESMC_PTR_AUTOINC	0x4000	/* auto incs. PTR correct amount */

/* Interrupt Detail */

#define	ESMC_INT_EPH		0x20	/* Set when EPH handler fires */
#define	ESMC_INT_RX_OVRN	0x10	/* set when receiver overruns */
#define	ESMC_INT_ALLOC		0x08	/* set when MMU allocation is done */
#define	ESMC_INT_TX_EMPTY	0x04	/* set id TX_FIFO empty */
#define	ESMC_INT_TX		0x02	/* set when at least one packet sent */
#define ESMC_INT_RCV		0x01
#define ESMC_INT_ERCV		0x40	/* for other chip sets */

/* Interrupts for chip to generate */

#define ESMC_INTERRUPT_MASK   (ESMC_INT_EPH | ESMC_INT_RX_OVRN | ESMC_INT_RCV) 


/* BANK 3 I/O Space mapping */

#define	ESMC_MULTICAST0		0x0	/* multicast table - WORD 0 */
#define	ESMC_MULTICAST2		0x2	/* multicast table - WORD 1 */
#define	ESMC_MULTICAST4		0x4	/* multicast table - WORD 2 */
#define	ESMC_MULTICAST6		0x6	/* multicast table - WORD 3 */
#define	ESMC_MGMT		0x8
#define	ESMC_REVISION		0xA	/* chip set and revision encoded here */
#define ESMC_ERCV		0xC	/* early recieve register */

/* Recieve frame status word - located at beginning of each received frame*/

#define ESMC_RS_ALGNERR		0x8000	/* frame had alignment error */
#define ESMC_RS_BRODCAST	0x4000	/* receive frame was broadcast */
#define ESMC_RS_BADCRC		0x2000	/* CRC error */
#define ESMC_RS_ODDFRM		0x1000	/* receive frame had odd byte */
#define ESMC_RS_TOOLONG		0x0800	/* longer then 1518 bytes */
#define ESMC_RS_TOOSHORT	0x0400	/* shorter then 64 bytes */
#define ESMC_RS_MULTICAST	0x0001	/* receive frame was multicast */
#define ESMC_RS_ERROR_MASK	(ESMC_RS_ALGNERR | ESMC_RS_BADCRC | ESMC_RS_TOOLONG | ESMC_RS_TOOSHORT) 


/* Switch bank - done often! */

#define ESMC_SWITCH_BANK(x)	{ sysOutWord(ioaddr + ESMC_BANK_SELECT, x ); } 


#ifdef __cplusplus
}
#endif

#endif	/* INCif_smc9.h */
