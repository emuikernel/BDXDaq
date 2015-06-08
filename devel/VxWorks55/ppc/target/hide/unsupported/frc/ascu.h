/* ascu.h - Force SYS68K/ASCU-2 board header */

/*
modification history
--------------------
01a,16jun87,tja  written
*/

/*
DESCRIPTION
This file contains IO address and related constants for the Force ASCU board,
a high performance system controller which contains powerful i/o devices
such as a serial interface, a parallel interface, a real time clock with
battery backup, a 4 level bus arbiter, an IEEE 488 GPIB interface (for all
you HP fans), and interrupt capabilities.

NOTE:
The define's used here are made not to conflict with the
define's used in the frc21.h header file. The ASCU board uses
the same set of chips as the CPU-21/9 so care has been taken
to eliminate multiple define's or redefining previous define's.
*/

#ifndef	INCascuh
#define	INCascuh

#ifndef	ASMLANGUAGE

struct rtc	/* real time clock structure */
    {
    int month;		/* month 1-12 */
    int day;		/* day of week 1-7 */
    int date;		/* day of month 1-31 */
    int hour;		/* 24 hour 0-23 */
    int mins;		/* minutes 0-59 */
    int sec;		/* seconds 0-59 */
    int s100;		/* hundreds and tenths of seconds */
    int s10000;		/* ten thousands of seconds */
    };

#endif	ASMLANGUAGE


#define ASCU_CLK_FREQ	8064000	/* frequency of PI/T clock input */
#define N_PROG_CHANNELS	8	/* no. of programmable IRQ channels */
#define N_USR_INTS	5	/* no. of user interrupts */

/* The base address for all on-board I/O devices is changeable.
 * Here, the as-shipped address is used.
 */

#define ASCU_BASE ((char *) 0xfcb02000)

/* port addresses for MPCC */

#define AMPCC_RSR        (ASCU_BASE + 0x001)   /* rcvr status reg */
#define AMPCC_RCR        (ASCU_BASE + 0x021)   /* rcvr control reg */
#define AMPCC_RDR        (ASCU_BASE + 0x003)   /* rcvr data reg */
#define AMPCC_RIVNR      (ASCU_BASE + 0x005)   /* rcvr interrupt vec num reg */
#define AMPCC_RIER       (ASCU_BASE + 0x025)   /* rcvr interrupt enable reg */

#define AMPCC_TSR        (ASCU_BASE + 0x009)   /* xmitter status reg */
#define AMPCC_TCR        (ASCU_BASE + 0x029)   /* xmitter control reg */
#define AMPCC_TDR        (ASCU_BASE + 0x00b)   /* xmitter data reg */
#define AMPCC_TIVNR      (ASCU_BASE + 0x00d)   /* xmitter int vec num reg */
#define AMPCC_TIER       (ASCU_BASE + 0x02d)   /* xmitter int enable reg */

#define AMPCC_SISR       (ASCU_BASE + 0x011)   /* serial intfc status reg */
#define AMPCC_SICR       (ASCU_BASE + 0x031)   /* serial intfc control reg */
#define AMPCC_SIVNR      (ASCU_BASE + 0x015)   /* serial intfc int vec num */
#define AMPCC_SIER       (ASCU_BASE + 0x035)   /* serial intfc int enable */

#define AMPCC_PSR1       (ASCU_BASE + 0x019)   /* protocol select reg 1 */
#define AMPCC_PSR2       (ASCU_BASE + 0x039)   /* protocol select reg 2 */
#define AMPCC_AR1        (ASCU_BASE + 0x01b)   /* address reg 1 */
#define AMPCC_AR2        (ASCU_BASE + 0x03b)   /* address reg 2 */
#define AMPCC_BRDR1      (ASCU_BASE + 0x01d)   /* baud rate divider reg 1 */
#define AMPCC_BRDR2      (ASCU_BASE + 0x03d)   /* baud rate divider reg 2 */
#define AMPCC_CCR        (ASCU_BASE + 0x01f)   /* clock control reg */
#define AMPCC_ECR        (ASCU_BASE + 0x03f)   /* error control reg */


/* MPCC rcvr status reg bits */

#ifndef MPCC_RSR_RDA
#define MPCC_RSR_RDA    0x80            /* rcvr data available */
#define MPCC_RSR_EOF    0x40            /* end of frame */
#define MPCC_RSR_CPERR  0x10            /* crc/parity error */
#define MPCC_RSR_FRERR  0x08            /* framing error */
#define MPCC_RSR_ROVRN  0x04            /* rcvr overrun */
#define MPCC_RSR_RAB    0x02            /* rcvr abort/break */
#define MPCC_RSR_RIDLE  0x01            /* rcvr idle */

/* MPCC rcvr control reg bits */

#define MPCC_RCR_RDSREN 0x40            /* rcvr data service req enable */
#define MPCC_RCR_DONEEN 0x20            /* DONE output enable */
#define MPCC_RCR_RSYNEN 0x10            /* RSYNEN output enable */
#define MPCC_RCR_STRSYN 0x08            /* strip SYN char (COP only) */
#define MPCC_RCR_2ADCMP 0x04            /* 1/2 address compare (BOP only) */
#define MPCC_RCR_RABTEN 0x02            /* rcvr abort enable (BOP only) */
#define MPCC_RCR_RRES   0x01            /* rcvr reset command */

/* MPCC rcvr interrupt enable reg bits */

#define MPCC_RIER_RDA   0x80            /* rcvr data available int enable */
#define MPCC_RIER_EOF   0x40            /* EOF interrupt enable */
#define MPCC_RIER_CPERR 0x20            /* crc/parity error int enable */
#define MPCC_RIER_FRERR 0x08            /* frame error int enable */
#define MPCC_RIER_ROVRN 0x04            /* rcvr overrun int enable */
#define MPCC_RIER_RAB   0x02            /* rcvr abort/break int enable */

/* MPCC xmitter status reg bits */

#define MPCC_TSR_TDRA   0x80            /* xmit data reg avail */
#define MPCC_TSR_TFC    0x40            /* xmitted frame complete */
#define MPCC_TSR_TUNRN  0x04            /* xmitter underrun */
#define MPCC_TSR_TFERR  0x02            /* xmitter frame error */

/* MPCC xmitter control reg bits */

#define MPCC_TCR_TEN    0x80            /* xmitter enable */
#define MPCC_TCR_TDSREN 0x40            /* xmitter data service req enable */
#define MPCC_TCR_TICS   0x20            /* xmitter idle char select */
#define MPCC_TCR_THW    0x10            /* xmit half word (word mode only) */
#define MPCC_TCR_TLAST  0x08            /* xmit last char */
#define MPCC_TCR_TSYN   0x04            /* xmit SYN */
#define MPCC_TCR_TABT   0x02            /* xmit ABORT */
#define MPCC_TCR_TRES   0x01            /* xmitter reset command */

/* MPCC xmitter interrupt enable reg bits */

#define MPCC_TIER_TDRA  0x80            /* xmitter data reg avail int enable */
#define MPCC_TIER_TFC   0x40            /* xmit frame complete int enable */
#define MPCC_TIER_TUNRN 0x04            /* xmitter underrun int enable */
#define MPCC_TIER_TFERR 0x02            /* xmitter frame error int enable */

/* MPCC Serial Interface status reg bits - not currently used */
/* MPCC Serial Interface control reg bits */

#define MPCC_SICR_RTSLVL 0x80           /* Request To Send level */
#define MPCC_SICR_DTRLVL 0x40           /* Data Terminal Ready level */
#define MPCC_SICR_ECHO   0x04           /* echo mode enable */
#define MPCC_SICR_TEST   0x02           /* self-test enable */
#define MPCC_SICR_NRZI   0x01           /* NRZI data format select */

/* MPCC Serial Interface interrupt enable reg bits - not currently used */
/* MPCC protocol select reg 1 bits - not currently used */
/* MPCC protocol select reg 2 bits */

#define MPCC_PSR2_WDBYTE        0x80    /* data bus word/byte */

#define MPCC_PSR2_1STOP         0x00    /* 1 stop bit */
#define MPCC_PSR2_12STOP        0x20    /* 1 1/2 stop bits */
#define MPCC_PSR2_2STOP         0x40    /* 2 stop bits */

#define MPCC_PSR2_5BITS         0x00    /* 5-bit chars */
#define MPCC_PSR2_6BITS         0x08    /* 6-bit chars */
#define MPCC_PSR2_7BITS         0x10    /* 7-bit chars */
#define MPCC_PSR2_8BITS         0x18    /* 8-bit chars */

#define MPCC_PSR2_ASYNC         0x06    /* async protocol */

/* MPCC clock control reg bits */

#define MPCC_CCR_PSCDIV         0x10    /* prescaler divider */
#define MPCC_CCR_TCLKO          0x08    /* xmitter clock output sel */
#define MPCC_CCR_RCLKIN         0x04    /* select external/internal rx clock */
#define MPCC_CCR_DIV1           0x00    /* external rx clock divide by 1 */
#define MPCC_CCR_DIV16          0x01    /* external rx clock divide by 16 */
#define MPCC_CCR_DIV32          0x02    /* external rx clock divide by 32 */
#define MPCC_CCR_DIV64          0x03    /* external rx clock divide by 64 */

/* MPCC error control reg bits */

#define MPCC_ECR_PAREN          0x80    /* parity enable */
#define MPCC_ECR_ODDPAR         0x40    /* odd/even parity select */
#define MPCC_ECR_CRCCTL         0x08    /* control field CRC enable */
#define MPCC_ECR_CRCPRE         0x04    /* crc generator preset select */
#define MPCC_ECR_CRCSEL1        0x00    /* crc polynomial 1 */
#define MPCC_ECR_CRCSEL2        0x01    /* crc polynomial 2 */
#define MPCC_ECR_CRCSEL3        0x02    /* crc polynomial 3 */
#define MPCC_ECR_CRCSEL4        0x03    /* crc polynomial 4 */
#endif

/* port addresses for PI/T-1 */

#define PIT1_PGCR        (ASCU_BASE + 0x041)    /* port general control reg */
#define PIT1_PSRR        (ASCU_BASE + 0x043)    /* port service request reg */
#define PIT1_PADDR       (ASCU_BASE + 0x045)    /* port A data direction reg */
#define PIT1_PBDDR       (ASCU_BASE + 0x047)    /* port B data direction reg */
#define PIT1_PCDDR       (ASCU_BASE + 0x049)    /* port C data direction reg */
#define PIT1_PIVR        (ASCU_BASE + 0x04b)    /* port interrupt vector reg */
#define PIT1_PACR        (ASCU_BASE + 0x04d)    /* port A control reg */
#define PIT1_PBCR        (ASCU_BASE + 0x04f)    /* port B control reg */
#define PIT1_PADR        (ASCU_BASE + 0x051)    /* port A data reg */
#define PIT1_PBDR        (ASCU_BASE + 0x053)    /* port B data reg */
#define PIT1_PAAR        (ASCU_BASE + 0x055)    /* port A alternate reg */
#define PIT1_PBAR        (ASCU_BASE + 0x057)    /* port B alternate reg */
#define PIT1_PCDR        (ASCU_BASE + 0x059)    /* port C data reg */
#define PIT1_PSR         (ASCU_BASE + 0x05b)    /* port status reg */

#define PIT1_TCR         (ASCU_BASE + 0x061)   /* timer control reg */
#define PIT1_TVIR        (ASCU_BASE + 0x063)   /* timer interrupt vector reg */
#define PIT1_CPRH        (ASCU_BASE + 0x067)   /* counter preload reg high */
#define PIT1_CPRM        (ASCU_BASE + 0x069)   /* counter preload reg medium */
#define PIT1_CPRL        (ASCU_BASE + 0x06b)   /* counter preload reg low */
#define PIT1_CNTRH       (ASCU_BASE + 0x06f)   /* count reg high */
#define PIT1_CNTRM       (ASCU_BASE + 0x071)   /* count reg medium */
#define PIT1_CNTRL       (ASCU_BASE + 0x073)   /* count reg low */
#define PIT1_TSR         (ASCU_BASE + 0x075)   /* timer status reg */

/* port addresses for PI/T-2 */

#define PIT2_PGCR	(ASCU_BASE + 0x081)	/* port general control reg */
#define PIT2_PSRR	(ASCU_BASE + 0x083)	/* port service request reg */
#define PIT2_PADDR	(ASCU_BASE + 0x085)	/* port A data direction reg */
#define PIT2_PBDDR	(ASCU_BASE + 0x087)	/* port B data direction reg */
#define PIT2_PCDDR	(ASCU_BASE + 0x089)	/* port C data direction reg */
#define PIT2_PIVR	(ASCU_BASE + 0x08b)	/* port interrupt vector reg */
#define PIT2_PACR	(ASCU_BASE + 0x08d)	/* port A control reg */
#define PIT2_PBCR	(ASCU_BASE + 0x08f)	/* port B control reg */
#define PIT2_PADR	(ASCU_BASE + 0x091)	/* port A data reg */
#define PIT2_PBDR	(ASCU_BASE + 0x093)	/* port B data reg */
#define PIT2_PAAR	(ASCU_BASE + 0x095)	/* port A alternate reg */
#define PIT2_PBAR	(ASCU_BASE + 0x097)	/* port B alternate reg */
#define PIT2_PCDR	(ASCU_BASE + 0x099)	/* port C data reg */
#define PIT2_PSR	(ASCU_BASE + 0x09b)	/* port status reg */

#define PIT2_TCR         (ASCU_BASE + 0x0a1)	/* timer control reg */
#define PIT2_TVIR        (ASCU_BASE + 0x0a3)	/* timer interrupt vector reg */
#define PIT2_CPRH        (ASCU_BASE + 0x0a7)	/* counter preload reg high */
#define PIT2_CPRM        (ASCU_BASE + 0x0a9)	/* counter preload reg medium */
#define PIT2_CPRL        (ASCU_BASE + 0x0ab)	/* counter preload reg low */
#define PIT2_CNTRH       (ASCU_BASE + 0x0af)	/* count reg high */
#define PIT2_CNTRM       (ASCU_BASE + 0x0b1)	/* count reg medium */
#define PIT2_CNTRL       (ASCU_BASE + 0x0b3)	/* count reg low */
#define PIT2_TSR         (ASCU_BASE + 0x0b5)	/* timer status reg */


/* PI/T timer status register bits */

#ifndef	PIT_TSR_ZDS
#define	PIT_TSR_ZDS             0x01            /* zero detect status */

/* PI/T timer control reg bits */

#define PIT_TCR_TOUT0	0x00	/* TOUT/TIACK ctrl 0 */
#define PIT_TCR_TOUT2	0x40	/* TOUT/TIACK ctrl 2 */
#define PIT_TCR_TOUT4	0x80	/* TOUT/TIACK ctrl 4 */
#define PIT_TCR_TOUT5	0xa0	/* TOUT/TIACK ctrl 5 */
#define PIT_TCR_TOUT6	0xc0	/* TOUT/TIACK ctrl 6 */
#define PIT_TCR_TOUT7	0xd0	/* TOUT/TIACK ctrl 7 */
#define PIT_TCR_ZDC	0x10	/* zero detect control */
#define PIT_TCR_CC0	0x00	/* clock control 0 */
#define PIT_TCR_CC1	0x02	/* clock control 0 */
#define PIT_TCR_CC2	0x04	/* clock control 0 */
#define PIT_TCR_CC3	0x06	/* clock control 0 */
#define PIT_TCR_TEN	0x01	/* timer enable */

#endif	PIT_TSR_ZDS

/* port addresses for RTC */

#define RTC_CTR_10000	(ASCU_BASE + 0x0c1)	/* counter, 1/10000 sec */
#define RTC_CTR_100	(ASCU_BASE + 0x0c3)	/* counter, 1/100 & 1/10 sec */
#define RTC_CTR_1	(ASCU_BASE + 0x0c5)	/* counter, 1 sec */
#define RTC_CTR_MIN	(ASCU_BASE + 0x0c7)	/* counter, minutes */
#define RTC_CTR_HOUR	(ASCU_BASE + 0x0c9)	/* counter, hours */
#define RTC_CTR_DAY	(ASCU_BASE + 0x0cb)	/* counter, day of week */
#define RTC_CTR_DATE	(ASCU_BASE + 0x0cd)	/* counter, day of month */
#define RTC_CTR_MONTH	(ASCU_BASE + 0x0cf)	/* counter, month */
#define RTC_RAM_10000	(ASCU_BASE + 0x0d1)	/* counter, 1/10000 sec */
#define RTC_RAM_100	(ASCU_BASE + 0x0d3)	/* counter, 1/10 & 1/10 sec */
#define RTC_RAM_1	(ASCU_BASE + 0x0d5)	/* counter, 1 sec */
#define RTC_RAM_MIN	(ASCU_BASE + 0x0d7)	/* counter, minutes */
#define RTC_RAM_HOUR	(ASCU_BASE + 0x0d9)	/* counter, hours */
#define RTC_RAM_DAY	(ASCU_BASE + 0x0db)	/* counter, day of week */
#define RTC_RAM_DATE	(ASCU_BASE + 0x0dd)	/* counter, day of month */
#define RTC_RAM_MONTH	(ASCU_BASE + 0x0df)	/* counter, month */
#define RTC_ISR		(ASCU_BASE + 0x0e1)	/* interrupt status reg */
#define RTC_ICR		(ASCU_BASE + 0x0e3)	/* interrupt control reg */
#define RTC_CTR_RESET	(ASCU_BASE + 0x0e5)	/* counter reset */
#define RTC_RAM_RESET	(ASCU_BASE + 0x0e7)	/* RAM reset */
#define RTC_STATUS	(ASCU_BASE + 0x0e9)	/* status bit */
#define RTC_GO		(ASCU_BASE + 0x0eb)	/* GO command */
#define RTC_STANDBY_INT	(ASCU_BASE + 0x0ed)	/* standby-interrupt */
#define RTC_TEST	(ASCU_BASE + 0x0ff)	/* test mode */

/* bit values for RTC interrupt control and status registers */

#define RTC_IR_COMPARE		0x01		/* RAM compare interrupt */
#define RTC_IR_10		0x02		/* 10Hz interrupt */
#define RTC_IR_1		0x04		/* 1 Hz interrupt */
#define RTC_IR_MIN		0x08		/* 1/Minute int */
#define RTC_IR_HOUR		0x10		/* 1/Hour int */
#define RTC_IR_DAY		0x20		/* 1/Day int */
#define RTC_IR_WEEK		0x40		/* 1/Week int */
#define RTC_IR_MONTH		0x80		/* 1/Month int */

/* port addresses for GPIB controller */

#define GPIB_DATAIN	(ASCU_BASE + 0x141)	/* r/o data in */
#define GPIB_INTS1	(ASCU_BASE + 0x143)	/* r/o interrupt status 1 */
#define GPIB_INTS2	(ASCU_BASE + 0x145)	/* r/o interrupt status 2 */
#define GPIB_SERPS	(ASCU_BASE + 0x147)	/* r/o serial poll status */
#define GPIB_ADDRS	(ASCU_BASE + 0x149)	/* r/o address status */
#define GPIB_COMPT	(ASCU_BASE + 0x14b)	/* r/o command pass thru */
#define GPIB_ADDR0	(ASCU_BASE + 0x14d)	/* r/o address 0 */
#define GPIB_ADDR1	(ASCU_BASE + 0x14f)	/* r/o address 1 */

#define GPIB_BYTEO	(ASCU_BASE + 0x141)	/* w/o byte out */
#define GPIB_INTM1	(ASCU_BASE + 0x143)	/* w/o interrupt mask 1 */
#define GPIB_INTM2	(ASCU_BASE + 0x145)	/* w/o interrupt mask 2 */
#define GPIB_SERPM	(ASCU_BASE + 0x147)	/* w/o serial poll mask */
#define GPIB_ADDRM	(ASCU_BASE + 0x149)	/* w/o address mode */
#define GPIB_AUXM	(ASCU_BASE + 0x14b)	/* w/o auxiliary mode */
#define GPIB_ADDR01	(ASCU_BASE + 0x14d)	/* w/o address 0/1 */
#define GPIB_EOS	(ASCU_BASE + 0x14f)	/* w/o end of string */

/* port addresses for BIM-1 */

#define BIM1_CR1	(ASCU_BASE + 0x101)	/* control reg 1 */
#define BIM1_CR2	(ASCU_BASE + 0x103)	/* control reg 2 */
#define BIM1_CR3	(ASCU_BASE + 0x105)	/* control reg 3 */
#define BIM1_CR4	(ASCU_BASE + 0x107)	/* control reg 4 */
#define BIM1_VR1	(ASCU_BASE + 0x109)	/* vector reg 1 */
#define BIM1_VR2	(ASCU_BASE + 0x10b)	/* vector reg 2 */
#define BIM1_VR3	(ASCU_BASE + 0x10d)	/* vector reg 3 */
#define BIM1_VR4	(ASCU_BASE + 0x10f)	/* vector reg 4 */

/* port addresses for BIM-2 */

#define BIM2_CR1	(ASCU_BASE + 0x111)	/* control reg 1 */
#define BIM2_CR2	(ASCU_BASE + 0x113)	/* control reg 2 */
#define BIM2_CR3	(ASCU_BASE + 0x115)	/* control reg 3 */
#define BIM2_CR4	(ASCU_BASE + 0x117)	/* control reg 4 */
#define BIM2_VR1	(ASCU_BASE + 0x119)	/* vector reg 1 */
#define BIM2_VR2	(ASCU_BASE + 0x11b)	/* vector reg 2 */
#define BIM2_VR3	(ASCU_BASE + 0x11d)	/* vector reg 3 */
#define BIM2_VR4	(ASCU_BASE + 0x11f)	/* vector reg 4 */

/* port addresses for BIM-3 */

#define BIM3_CR1	(ASCU_BASE + 0x121)	/* control reg 1 */
#define BIM3_CR2	(ASCU_BASE + 0x123)	/* control reg 2 */
#define BIM3_CR3	(ASCU_BASE + 0x125)	/* control reg 3 */
#define BIM3_CR4	(ASCU_BASE + 0x127)	/* control reg 4 */
#define BIM3_VR1	(ASCU_BASE + 0x129)	/* vector reg 1 */
#define BIM3_VR2	(ASCU_BASE + 0x12b)	/* vector reg 2 */
#define BIM3_VR3	(ASCU_BASE + 0x12d)	/* vector reg 3 */
#define BIM3_VR4	(ASCU_BASE + 0x12f)	/* vector reg 4 */

/* port addresses for BIM-4 */

#define BIM4_CR1	(ASCU_BASE + 0x131)	/* control reg 1 */
#define BIM4_CR2	(ASCU_BASE + 0x133)	/* control reg 2 */
#define BIM4_CR3	(ASCU_BASE + 0x135)	/* control reg 3 */
#define BIM4_CR4	(ASCU_BASE + 0x137)	/* control reg 4 */
#define BIM4_VR1	(ASCU_BASE + 0x139)	/* vector reg 1 */
#define BIM4_VR2	(ASCU_BASE + 0x13b)	/* vector reg 2 */
#define BIM4_VR3	(ASCU_BASE + 0x13d)	/* vector reg 3 */
#define BIM4_VR4	(ASCU_BASE + 0x13f)	/* vector reg 4 */

/* BIM control reg bits */

#ifndef	BIM_IRE

#define BIM_IRE		0x10		/* interrupt enable */
#define BIM_IRAC	0x08		/* interrupt auto-clear */
#define BIM_XIN		0x20		/* don't supply vector */
#define BIM_F		0x80		/* user-defined flag */
#define BIM_FAC		0x40		/* clear flag in interrupt */

#endif	BIM_IRE

#endif	INCascuh
