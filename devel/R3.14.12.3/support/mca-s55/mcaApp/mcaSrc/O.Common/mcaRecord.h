
#ifndef INCmcaSTRTH
#define INCmcaSTRTH
typedef enum {
	mcaSTRT_Done,
	mcaSTRT_Acquire
}mcaSTRT;
#endif /*INCmcaSTRTH*/

#ifndef INCmcaREADH
#define INCmcaREADH
typedef enum {
	mcaREAD_Done,
	mcaREAD_Read
}mcaREAD;
#endif /*INCmcaREADH*/

#ifndef INCmcaR0IPH
#define INCmcaR0IPH
typedef enum {
	mcaR0IP_N,
	mcaR0IP_Y
}mcaR0IP;
#endif /*INCmcaR0IPH*/

#ifndef INCmcaMODEH
#define INCmcaMODEH
typedef enum {
	mcaMODE_PHA,
	mcaMODE_MCS,
	mcaMODE_List
}mcaMODE;
#endif /*INCmcaMODEH*/

#ifndef INCmcaERASH
#define INCmcaERASH
typedef enum {
	mcaERAS_Done,
	mcaERAS_Erase
}mcaERAS;
#endif /*INCmcaERASH*/

#ifndef INCmcaCHASH
#define INCmcaCHASH
typedef enum {
	mcaCHAS_Internal,
	mcaCHAS_External
}mcaCHAS;
#endif /*INCmcaCHASH*/
#ifndef INCmcaH
#define INCmcaH
#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"
typedef struct mcaRecord {
	char		name[61];	/* Record Name */
	char		desc[41];	/* Descriptor */
	char		asg[29];	/* Access Security Group */
	epicsEnum16	scan;	/* Scan Mechanism */
	epicsEnum16	pini;	/* Process at iocInit */
	epicsInt16	phas;	/* Scan Phase */
	epicsInt16	evnt;	/* Event Number */
	epicsInt16	tse;	/* Time Stamp Event */
	DBLINK		tsel;	/* Time Stamp Link */
	epicsEnum16	dtyp;	/* Device Type */
	epicsInt16	disv;	/* Disable Value */
	epicsInt16	disa;	/* Disable */
	DBLINK		sdis;	/* Scanning Disable */
	epicsMutexId	mlok;	/* Monitor lock */
	ELLLIST		mlis;	/* Monitor List */
	epicsUInt8	disp;	/* Disable putField */
	epicsUInt8	proc;	/* Force Processing */
	epicsEnum16	stat;	/* Alarm Status */
	epicsEnum16	sevr;	/* Alarm Severity */
	epicsEnum16	nsta;	/* New Alarm Status */
	epicsEnum16	nsev;	/* New Alarm Severity */
	epicsEnum16	acks;	/* Alarm Ack Severity */
	epicsEnum16	ackt;	/* Alarm Ack Transient */
	epicsEnum16	diss;	/* Disable Alarm Sevrty */
	epicsUInt8	lcnt;	/* Lock Count */
	epicsUInt8	pact;	/* Record active */
	epicsUInt8	putf;	/* dbPutField process */
	epicsUInt8	rpro;	/* Reprocess  */
	struct asgMember *asp;	/* Access Security Pvt */
	struct putNotify *ppn;	/* addr of PUTNOTIFY */
	struct putNotifyRecord *ppnr;	/* pputNotifyRecord */
	struct scan_element *spvt;	/* Scan Private */
	struct rset	*rset;	/* Address of RSET */
	struct dset	*dset;	/* DSET address */
	void		*dpvt;	/* Device Private */
	struct dbRecordType *rdes;	/* Address of dbRecordType */
	struct lockRecord *lset;	/* Lock Set */
	epicsEnum16	prio;	/* Scheduling Priority */
	epicsUInt8	tpro;	/* Trace Processing */
	char bkpt;	/* Break Point */
	epicsUInt8	udf;	/* Undefined */
	epicsTimeStamp	time;	/* Time */
	DBLINK		flnk;	/* Forward Process Link */
	epicsFloat64	vers;	/* Code Version */
	void *val;	/* Value */
	void *bptr;	/* Buffer Pointer */
	void *bg;	/* Background */
	void *pbg;	/* Background buffer */
	void *pstatus;	/* Status buffer */
	epicsFloat64	hopr;	/* High Operating Range */
	epicsFloat64	lopr;	/* Low Operating Range */
	epicsInt32	nmax;	/* Max number of channels */
	epicsInt32	nord;	/* Number of channels read */
	epicsInt16	prec;	/* Display Precision */
	epicsEnum16	ftvl;	/* Field Type of Value */
	epicsEnum16	strt;	/* Start Acquire */
	epicsEnum16	erst;	/* Erase & Start Acquire */
	epicsEnum16	stop;	/* Stop Acquire */
	epicsEnum16	acqg;	/* Acquiring */
	epicsEnum16	read;	/* Read array */
	epicsEnum16	rdng;	/* Reading array */
	epicsEnum16	rdns;	/* Reading status */
	epicsEnum16	eras;	/* Erase array */
	epicsEnum16	chas;	/* Channel-advance source */
	epicsInt32	nuse;	/*  # of channels to use */
	epicsInt32	seq;	/* Time sequence # */
	epicsFloat64	dwel;	/* Dwell time per channel */
	epicsInt32	pscl;	/* Channel advance prescale */
	epicsFloat64	prtm;	/* Preset real time */
	epicsFloat64	pltm;	/* Preset live time */
	epicsInt32	pct;	/* Preset counts */
	epicsInt32	pctl;	/* Preset count low channe */
	epicsInt32	pcth;	/* Preset count high chann */
	epicsInt32	pswp;	/* Preset number of sweeps */
	epicsEnum16	mode;	/* Mode PHA/MCS/LIST */
	epicsFloat64	calo;	/* Calibration offset */
	epicsFloat64	cals;	/* Calibration slope */
	epicsFloat64	calq;	/* Calibration quadratic */
	char		egu[16];	/* Calibration Units Name */
	epicsFloat64	tth;	/* Two-theta */
	epicsFloat64	ertm;	/* Elapsed real time */
	epicsFloat64	eltm;	/* Elapsed live time */
	epicsFloat64	dtim;	/* Average % dead time */
	epicsFloat64	idtim;	/* Instantaneous % dead time */
	char		stim[40];	/* Acquisition start time */
	epicsFloat64	rtim;	/* Last read time */
	epicsInt32	act;	/* Actual counts in pregio */
	epicsInt16	nack;	/* Message not acknowledge */
	DBLINK		inp;	/* Input Specification */
	epicsFloat64	hihi;	/* Hihi Deviation Limit */
	epicsFloat64	lolo;	/* Lolo Deviation Limit */
	epicsFloat64	high;	/* High Deviation Limit */
	epicsFloat64	low;	/* Low Deviation Limit */
	epicsEnum16	hhsv;	/* Hihi Severity */
	epicsEnum16	llsv;	/* Lolo Severity */
	epicsEnum16	hsv;	/* High Severity */
	epicsEnum16	lsv;	/* Low Severity */
	epicsFloat64	hyst;	/* Alarm Deadband */
	epicsFloat64	lalm;	/* Last Value Alarmed */
	DBLINK		siol;	/* Sim Input Specifctn */
	DBLINK		siml;	/* Sim Mode Location */
	epicsEnum16	simm;	/* Simulation Mode */
	epicsEnum16	sims;	/* Sim mode Alarm Svrty */
	epicsUInt32	mmap;	/* Monitor map */
	epicsUInt32	rmap;	/* ROI monitor map */
	epicsUInt32	newv;	/* New Value map */
	epicsUInt32	newr;	/* New ROI Value map */
	epicsInt32	r0lo;	/* Region 0 low channel */
	epicsInt32	r0hi;	/* Region 0 high channel */
	epicsInt16	r0bg;	/* Region 0 bkgrnd chans */
	epicsEnum16	r0ip;	/* Region 0 is preset */
	epicsInt32	r1lo;	/* Region 1 low channel */
	epicsInt32	r1hi;	/* Region 1 high channel */
	epicsInt16	r1bg;	/* Region 1 bkgrnd chans */
	epicsEnum16	r1ip;	/* Region 1 is preset */
	epicsInt32	r2lo;	/* Region 2 low channel */
	epicsInt32	r2hi;	/* Region 2 high channel */
	epicsInt16	r2bg;	/* Region 2 bkgrnd chans */
	epicsEnum16	r2ip;	/* Region 2 is preset */
	epicsInt32	r3lo;	/* Region 3 low channel */
	epicsInt32	r3hi;	/* Region 3 high channel */
	epicsInt16	r3bg;	/* Region 3 bkgrnd chans */
	epicsEnum16	r3ip;	/* Region 3 is preset */
	epicsInt32	r4lo;	/* Region 4 low channel */
	epicsInt32	r4hi;	/* Region 4 high channel */
	epicsInt16	r4bg;	/* Region 4 bkgrnd chans */
	epicsEnum16	r4ip;	/* Region 4 is preset */
	epicsInt32	r5lo;	/* Region 5 low channel */
	epicsInt32	r5hi;	/* Region 5 high channel */
	epicsInt16	r5bg;	/* Region 5 bkgrnd chans */
	epicsEnum16	r5ip;	/* Region 5 is preset */
	epicsInt32	r6lo;	/* Region 6 low channel */
	epicsInt32	r6hi;	/* Region 6 high channel */
	epicsInt16	r6bg;	/* Region 6 bkgrnd chans */
	epicsEnum16	r6ip;	/* Region 6 is preset */
	epicsInt32	r7lo;	/* Region 7 low channel */
	epicsInt32	r7hi;	/* Region 7 high channel */
	epicsInt16	r7bg;	/* Region 7 bkgrnd chans */
	epicsEnum16	r7ip;	/* Region 7 is preset */
	epicsInt32	r8lo;	/* Region 8 low channel */
	epicsInt32	r8hi;	/* Region 8 high channel */
	epicsInt16	r8bg;	/* Region 8 bkgrnd chans */
	epicsEnum16	r8ip;	/* Region 8 is preset */
	epicsInt32	r9lo;	/* Region 9 low channel */
	epicsInt32	r9hi;	/* Region 9 high channel */
	epicsInt16	r9bg;	/* Region 9 bkgrnd chans */
	epicsEnum16	r9ip;	/* Region 9 is preset */
	epicsInt32	r10lo;	/* Region 10 low channel */
	epicsInt32	r10hi;	/* Region 10 high channel */
	epicsInt16	r10bg;	/* Region 10 bkgrnd chans */
	epicsEnum16	r10ip;	/* Region 10 is preset */
	epicsInt32	r11lo;	/* Region 11 low channel */
	epicsInt32	r11hi;	/* Region 11 high channel */
	epicsInt16	r11bg;	/* Region 11 bkgrnd chans */
	epicsEnum16	r11ip;	/* Region 11 is preset */
	epicsInt32	r12lo;	/* Region 12 low channel */
	epicsInt32	r12hi;	/* Region 12 high channel */
	epicsInt16	r12bg;	/* Region 12 bkgrnd chans */
	epicsEnum16	r12ip;	/* Region 12 is preset */
	epicsInt32	r13lo;	/* Region 13 low channel */
	epicsInt32	r13hi;	/* Region 13 high channel */
	epicsInt16	r13bg;	/* Region 13 bkgrnd chans */
	epicsEnum16	r13ip;	/* Region 13 is preset */
	epicsInt32	r14lo;	/* Region 14 low channel */
	epicsInt32	r14hi;	/* Region 14 high channel */
	epicsInt16	r14bg;	/* Region 14 bkgrnd chans */
	epicsEnum16	r14ip;	/* Region 14 is preset */
	epicsInt32	r15lo;	/* Region 15 low channel */
	epicsInt32	r15hi;	/* Region 15 high channel */
	epicsInt16	r15bg;	/* Region 15 bkgrnd chans */
	epicsEnum16	r15ip;	/* Region 15 is preset */
	epicsInt32	r16lo;	/* Region 16 low channel */
	epicsInt32	r16hi;	/* Region 16 high channel */
	epicsInt16	r16bg;	/* Region 16 bkgrnd chans */
	epicsEnum16	r16ip;	/* Region 16 is preset */
	epicsInt32	r17lo;	/* Region 17 low channel */
	epicsInt32	r17hi;	/* Region 17 high channel */
	epicsInt16	r17bg;	/* Region 17 bkgrnd chans */
	epicsEnum16	r17ip;	/* Region 17 is preset */
	epicsInt32	r18lo;	/* Region 18 low channel */
	epicsInt32	r18hi;	/* Region 18 high channel */
	epicsInt16	r18bg;	/* Region 18 bkgrnd chans */
	epicsEnum16	r18ip;	/* Region 18 is preset */
	epicsInt32	r19lo;	/* Region 19 low channel */
	epicsInt32	r19hi;	/* Region 19 high channel */
	epicsInt16	r19bg;	/* Region 19 bkgrnd chans */
	epicsEnum16	r19ip;	/* Region 19 is preset */
	epicsInt32	r20lo;	/* Region 20 low channel */
	epicsInt32	r20hi;	/* Region 20 high channel */
	epicsInt16	r20bg;	/* Region 20 bkgrnd chans */
	epicsEnum16	r20ip;	/* Region 20 is preset */
	epicsInt32	r21lo;	/* Region 21 low channel */
	epicsInt32	r21hi;	/* Region 21 high channel */
	epicsInt16	r21bg;	/* Region 21 bkgrnd chans */
	epicsEnum16	r21ip;	/* Region 21 is preset */
	epicsInt32	r22lo;	/* Region 22 low channel */
	epicsInt32	r22hi;	/* Region 22 high channel */
	epicsInt16	r22bg;	/* Region 22 bkgrnd chans */
	epicsEnum16	r22ip;	/* Region 22 is preset */
	epicsInt32	r23lo;	/* Region 23 low channel */
	epicsInt32	r23hi;	/* Region 23 high channel */
	epicsInt16	r23bg;	/* Region 23 bkgrnd chans */
	epicsEnum16	r23ip;	/* Region 23 is preset */
	epicsInt32	r24lo;	/* Region 24 low channel */
	epicsInt32	r24hi;	/* Region 24 high channel */
	epicsInt16	r24bg;	/* Region 24 bkgrnd chans */
	epicsEnum16	r24ip;	/* Region 24 is preset */
	epicsInt32	r25lo;	/* Region 25 low channel */
	epicsInt32	r25hi;	/* Region 25 high channel */
	epicsInt16	r25bg;	/* Region 25 bkgrnd chans */
	epicsEnum16	r25ip;	/* Region 25 is preset */
	epicsInt32	r26lo;	/* Region 26 low channel */
	epicsInt32	r26hi;	/* Region 26 high channel */
	epicsInt16	r26bg;	/* Region 26 bkgrnd chans */
	epicsEnum16	r26ip;	/* Region 26 is preset */
	epicsInt32	r27lo;	/* Region 27 low channel */
	epicsInt32	r27hi;	/* Region 27 high channel */
	epicsInt16	r27bg;	/* Region 27 bkgrnd chans */
	epicsEnum16	r27ip;	/* Region 27 is preset */
	epicsInt32	r28lo;	/* Region 28 low channel */
	epicsInt32	r28hi;	/* Region 28 high channel */
	epicsInt16	r28bg;	/* Region 28 bkgrnd chans */
	epicsEnum16	r28ip;	/* Region 28 is preset */
	epicsInt32	r29lo;	/* Region 29 low channel */
	epicsInt32	r29hi;	/* Region 29 high channel */
	epicsInt16	r29bg;	/* Region 29 bkgrnd chans */
	epicsEnum16	r29ip;	/* Region 29 is preset */
	epicsInt32	r30lo;	/* Region 30 low channel */
	epicsInt32	r30hi;	/* Region 30 high channel */
	epicsInt16	r30bg;	/* Region 30 bkgrnd chans */
	epicsEnum16	r30ip;	/* Region 30 is preset */
	epicsInt32	r31lo;	/* Region 31 low channel */
	epicsInt32	r31hi;	/* Region 31 high channel */
	epicsInt16	r31bg;	/* Region 31 bkgrnd chans */
	epicsEnum16	r31ip;	/* Region 31 is preset */
	epicsFloat64	r0;	/* Region 0 counts */
	epicsFloat64	r0n;	/* Region 0 net counts */
	epicsFloat64	r0p;	/* Region 0 preset count */
	epicsFloat64	r1;	/* Region 1 counts */
	epicsFloat64	r1n;	/* Region 1 net counts */
	epicsFloat64	r1p;	/* Region 1 preset count */
	epicsFloat64	r2;	/* Region 2 counts */
	epicsFloat64	r2n;	/* Region 2 net counts */
	epicsFloat64	r2p;	/* Region 2 preset count */
	epicsFloat64	r3;	/* Region 3 counts */
	epicsFloat64	r3n;	/* Region 3 net counts */
	epicsFloat64	r3p;	/* Region 3 preset count */
	epicsFloat64	r4;	/* Region 4 counts */
	epicsFloat64	r4n;	/* Region 4 net counts */
	epicsFloat64	r4p;	/* Region 4 preset count */
	epicsFloat64	r5;	/* Region 5 counts */
	epicsFloat64	r5n;	/* Region 5 net counts */
	epicsFloat64	r5p;	/* Region 5 preset count */
	epicsFloat64	r6;	/* Region 6 counts */
	epicsFloat64	r6n;	/* Region 6 net counts */
	epicsFloat64	r6p;	/* Region 6 preset */
	epicsFloat64	r7;	/* Region 7 counts */
	epicsFloat64	r7n;	/* Region 7 net counts */
	epicsFloat64	r7p;	/* Region 7 preset */
	epicsFloat64	r8;	/* Region 8 counts */
	epicsFloat64	r8n;	/* Region 8 net counts */
	epicsFloat64	r8p;	/* Region 8 preset */
	epicsFloat64	r9;	/* Region 9 counts */
	epicsFloat64	r9n;	/* Region 9 net counts */
	epicsFloat64	r9p;	/* Region 9 preset */
	epicsFloat64	r10;	/* Region 10 counts */
	epicsFloat64	r10n;	/* Region 10 net counts */
	epicsFloat64	r10p;	/* Region 10 preset */
	epicsFloat64	r11;	/* Region 11 counts */
	epicsFloat64	r11n;	/* Region 11 net counts */
	epicsFloat64	r11p;	/* Region 11 preset */
	epicsFloat64	r12;	/* Region 12 counts */
	epicsFloat64	r12n;	/* Region 12 net counts */
	epicsFloat64	r12p;	/* Region 12 preset */
	epicsFloat64	r13;	/* Region 13 counts */
	epicsFloat64	r13n;	/* Region 13 net counts */
	epicsFloat64	r13p;	/* Region 13 preset */
	epicsFloat64	r14;	/* Region 14 counts */
	epicsFloat64	r14n;	/* Region 14 net counts */
	epicsFloat64	r14p;	/* Region 14 preset */
	epicsFloat64	r15;	/* Region 15 counts */
	epicsFloat64	r15n;	/* Region 15 net counts */
	epicsFloat64	r15p;	/* Region 15 preset */
	epicsFloat64	r16;	/* Region 16 counts */
	epicsFloat64	r16n;	/* Region 16 net counts */
	epicsFloat64	r16p;	/* Region 16 preset */
	epicsFloat64	r17;	/* Region 17 counts */
	epicsFloat64	r17n;	/* Region 17 net counts */
	epicsFloat64	r17p;	/* Region 17 preset */
	epicsFloat64	r18;	/* Region 18 counts */
	epicsFloat64	r18n;	/* Region 18 net counts */
	epicsFloat64	r18p;	/* Region 18 preset */
	epicsFloat64	r19;	/* Region 19 counts */
	epicsFloat64	r19n;	/* Region 19 net counts */
	epicsFloat64	r19p;	/* Region 19 preset */
	epicsFloat64	r20;	/* Region 20 counts */
	epicsFloat64	r20n;	/* Region 20 net counts */
	epicsFloat64	r20p;	/* Region 20 preset */
	epicsFloat64	r21;	/* Region 21 counts */
	epicsFloat64	r21n;	/* Region 21 net counts */
	epicsFloat64	r21p;	/* Region 21 preset */
	epicsFloat64	r22;	/* Region 22 counts */
	epicsFloat64	r22n;	/* Region 22 net counts */
	epicsFloat64	r22p;	/* Region 22 preset */
	epicsFloat64	r23;	/* Region 23 counts */
	epicsFloat64	r23n;	/* Region 23 net counts */
	epicsFloat64	r23p;	/* Region 23 preset */
	epicsFloat64	r24;	/* Region 24 counts */
	epicsFloat64	r24n;	/* Region 24 net counts */
	epicsFloat64	r24p;	/* Region 24 preset */
	epicsFloat64	r25;	/* Region 25 counts */
	epicsFloat64	r25n;	/* Region 25 net counts */
	epicsFloat64	r25p;	/* Region 25 preset */
	epicsFloat64	r26;	/* Region 26 counts */
	epicsFloat64	r26n;	/* Region 26 net counts */
	epicsFloat64	r26p;	/* Region 26 preset */
	epicsFloat64	r27;	/* Region 27 counts */
	epicsFloat64	r27n;	/* Region 27 net counts */
	epicsFloat64	r27p;	/* Region 27 preset */
	epicsFloat64	r28;	/* Region 28 counts */
	epicsFloat64	r28n;	/* Region 28 net counts */
	epicsFloat64	r28p;	/* Region 28 preset */
	epicsFloat64	r29;	/* Region 29 counts */
	epicsFloat64	r29n;	/* Region 29 net counts */
	epicsFloat64	r29p;	/* Region 29 preset */
	epicsFloat64	r30;	/* Region 30 counts */
	epicsFloat64	r30n;	/* Region 30 net counts */
	epicsFloat64	r30p;	/* Region 30 preset */
	epicsFloat64	r31;	/* Region 31 counts */
	epicsFloat64	r31n;	/* Region 31 net counts */
	epicsFloat64	r31p;	/* Region 31 preset */
	char		r0nm[16];	/* Region 0 name */
	char		r1nm[16];	/* Region 1 name */
	char		r2nm[16];	/* Region 2 name */
	char		r3nm[16];	/* Region 3 name */
	char		r4nm[16];	/* Region 4 name */
	char		r5nm[16];	/* Region 5 name */
	char		r6nm[16];	/* Region 6 name */
	char		r7nm[16];	/* Region 7 name */
	char		r8nm[16];	/* Region 8 name */
	char		r9nm[16];	/* Region 9 name */
	char		r10nm[16];	/* Region 10 name */
	char		r11nm[16];	/* Region 11 name */
	char		r12nm[16];	/* Region 12 name */
	char		r13nm[16];	/* Region 13 name */
	char		r14nm[16];	/* Region 14 name */
	char		r15nm[16];	/* Region 15 name */
	char		r16nm[16];	/* Region 16 name */
	char		r17nm[16];	/* Region 17 name */
	char		r18nm[16];	/* Region 18 name */
	char		r19nm[16];	/* Region 19 name */
	char		r20nm[16];	/* Region 20 name */
	char		r21nm[16];	/* Region 21 name */
	char		r22nm[16];	/* Region 22 name */
	char		r23nm[16];	/* Region 23 name */
	char		r24nm[16];	/* Region 24 name */
	char		r25nm[16];	/* Region 25 name */
	char		r26nm[16];	/* Region 26 name */
	char		r27nm[16];	/* Region 27 name */
	char		r28nm[16];	/* Region 28 name */
	char		r29nm[16];	/* Region 29 name */
	char		r30nm[16];	/* Region 30 name */
	char		r31nm[16];	/* Region 31 name */
} mcaRecord;
#define mcaRecordNAME	0
#define mcaRecordDESC	1
#define mcaRecordASG	2
#define mcaRecordSCAN	3
#define mcaRecordPINI	4
#define mcaRecordPHAS	5
#define mcaRecordEVNT	6
#define mcaRecordTSE	7
#define mcaRecordTSEL	8
#define mcaRecordDTYP	9
#define mcaRecordDISV	10
#define mcaRecordDISA	11
#define mcaRecordSDIS	12
#define mcaRecordMLOK	13
#define mcaRecordMLIS	14
#define mcaRecordDISP	15
#define mcaRecordPROC	16
#define mcaRecordSTAT	17
#define mcaRecordSEVR	18
#define mcaRecordNSTA	19
#define mcaRecordNSEV	20
#define mcaRecordACKS	21
#define mcaRecordACKT	22
#define mcaRecordDISS	23
#define mcaRecordLCNT	24
#define mcaRecordPACT	25
#define mcaRecordPUTF	26
#define mcaRecordRPRO	27
#define mcaRecordASP	28
#define mcaRecordPPN	29
#define mcaRecordPPNR	30
#define mcaRecordSPVT	31
#define mcaRecordRSET	32
#define mcaRecordDSET	33
#define mcaRecordDPVT	34
#define mcaRecordRDES	35
#define mcaRecordLSET	36
#define mcaRecordPRIO	37
#define mcaRecordTPRO	38
#define mcaRecordBKPT	39
#define mcaRecordUDF	40
#define mcaRecordTIME	41
#define mcaRecordFLNK	42
#define mcaRecordVERS	43
#define mcaRecordVAL	44
#define mcaRecordBPTR	45
#define mcaRecordBG	46
#define mcaRecordPBG	47
#define mcaRecordPSTATUS	48
#define mcaRecordHOPR	49
#define mcaRecordLOPR	50
#define mcaRecordNMAX	51
#define mcaRecordNORD	52
#define mcaRecordPREC	53
#define mcaRecordFTVL	54
#define mcaRecordSTRT	55
#define mcaRecordERST	56
#define mcaRecordSTOP	57
#define mcaRecordACQG	58
#define mcaRecordREAD	59
#define mcaRecordRDNG	60
#define mcaRecordRDNS	61
#define mcaRecordERAS	62
#define mcaRecordCHAS	63
#define mcaRecordNUSE	64
#define mcaRecordSEQ	65
#define mcaRecordDWEL	66
#define mcaRecordPSCL	67
#define mcaRecordPRTM	68
#define mcaRecordPLTM	69
#define mcaRecordPCT	70
#define mcaRecordPCTL	71
#define mcaRecordPCTH	72
#define mcaRecordPSWP	73
#define mcaRecordMODE	74
#define mcaRecordCALO	75
#define mcaRecordCALS	76
#define mcaRecordCALQ	77
#define mcaRecordEGU	78
#define mcaRecordTTH	79
#define mcaRecordERTM	80
#define mcaRecordELTM	81
#define mcaRecordDTIM	82
#define mcaRecordIDTIM	83
#define mcaRecordSTIM	84
#define mcaRecordRTIM	85
#define mcaRecordACT	86
#define mcaRecordNACK	87
#define mcaRecordINP	88
#define mcaRecordHIHI	89
#define mcaRecordLOLO	90
#define mcaRecordHIGH	91
#define mcaRecordLOW	92
#define mcaRecordHHSV	93
#define mcaRecordLLSV	94
#define mcaRecordHSV	95
#define mcaRecordLSV	96
#define mcaRecordHYST	97
#define mcaRecordLALM	98
#define mcaRecordSIOL	99
#define mcaRecordSIML	100
#define mcaRecordSIMM	101
#define mcaRecordSIMS	102
#define mcaRecordMMAP	103
#define mcaRecordRMAP	104
#define mcaRecordNEWV	105
#define mcaRecordNEWR	106
#define mcaRecordR0LO	107
#define mcaRecordR0HI	108
#define mcaRecordR0BG	109
#define mcaRecordR0IP	110
#define mcaRecordR1LO	111
#define mcaRecordR1HI	112
#define mcaRecordR1BG	113
#define mcaRecordR1IP	114
#define mcaRecordR2LO	115
#define mcaRecordR2HI	116
#define mcaRecordR2BG	117
#define mcaRecordR2IP	118
#define mcaRecordR3LO	119
#define mcaRecordR3HI	120
#define mcaRecordR3BG	121
#define mcaRecordR3IP	122
#define mcaRecordR4LO	123
#define mcaRecordR4HI	124
#define mcaRecordR4BG	125
#define mcaRecordR4IP	126
#define mcaRecordR5LO	127
#define mcaRecordR5HI	128
#define mcaRecordR5BG	129
#define mcaRecordR5IP	130
#define mcaRecordR6LO	131
#define mcaRecordR6HI	132
#define mcaRecordR6BG	133
#define mcaRecordR6IP	134
#define mcaRecordR7LO	135
#define mcaRecordR7HI	136
#define mcaRecordR7BG	137
#define mcaRecordR7IP	138
#define mcaRecordR8LO	139
#define mcaRecordR8HI	140
#define mcaRecordR8BG	141
#define mcaRecordR8IP	142
#define mcaRecordR9LO	143
#define mcaRecordR9HI	144
#define mcaRecordR9BG	145
#define mcaRecordR9IP	146
#define mcaRecordR10LO	147
#define mcaRecordR10HI	148
#define mcaRecordR10BG	149
#define mcaRecordR10IP	150
#define mcaRecordR11LO	151
#define mcaRecordR11HI	152
#define mcaRecordR11BG	153
#define mcaRecordR11IP	154
#define mcaRecordR12LO	155
#define mcaRecordR12HI	156
#define mcaRecordR12BG	157
#define mcaRecordR12IP	158
#define mcaRecordR13LO	159
#define mcaRecordR13HI	160
#define mcaRecordR13BG	161
#define mcaRecordR13IP	162
#define mcaRecordR14LO	163
#define mcaRecordR14HI	164
#define mcaRecordR14BG	165
#define mcaRecordR14IP	166
#define mcaRecordR15LO	167
#define mcaRecordR15HI	168
#define mcaRecordR15BG	169
#define mcaRecordR15IP	170
#define mcaRecordR16LO	171
#define mcaRecordR16HI	172
#define mcaRecordR16BG	173
#define mcaRecordR16IP	174
#define mcaRecordR17LO	175
#define mcaRecordR17HI	176
#define mcaRecordR17BG	177
#define mcaRecordR17IP	178
#define mcaRecordR18LO	179
#define mcaRecordR18HI	180
#define mcaRecordR18BG	181
#define mcaRecordR18IP	182
#define mcaRecordR19LO	183
#define mcaRecordR19HI	184
#define mcaRecordR19BG	185
#define mcaRecordR19IP	186
#define mcaRecordR20LO	187
#define mcaRecordR20HI	188
#define mcaRecordR20BG	189
#define mcaRecordR20IP	190
#define mcaRecordR21LO	191
#define mcaRecordR21HI	192
#define mcaRecordR21BG	193
#define mcaRecordR21IP	194
#define mcaRecordR22LO	195
#define mcaRecordR22HI	196
#define mcaRecordR22BG	197
#define mcaRecordR22IP	198
#define mcaRecordR23LO	199
#define mcaRecordR23HI	200
#define mcaRecordR23BG	201
#define mcaRecordR23IP	202
#define mcaRecordR24LO	203
#define mcaRecordR24HI	204
#define mcaRecordR24BG	205
#define mcaRecordR24IP	206
#define mcaRecordR25LO	207
#define mcaRecordR25HI	208
#define mcaRecordR25BG	209
#define mcaRecordR25IP	210
#define mcaRecordR26LO	211
#define mcaRecordR26HI	212
#define mcaRecordR26BG	213
#define mcaRecordR26IP	214
#define mcaRecordR27LO	215
#define mcaRecordR27HI	216
#define mcaRecordR27BG	217
#define mcaRecordR27IP	218
#define mcaRecordR28LO	219
#define mcaRecordR28HI	220
#define mcaRecordR28BG	221
#define mcaRecordR28IP	222
#define mcaRecordR29LO	223
#define mcaRecordR29HI	224
#define mcaRecordR29BG	225
#define mcaRecordR29IP	226
#define mcaRecordR30LO	227
#define mcaRecordR30HI	228
#define mcaRecordR30BG	229
#define mcaRecordR30IP	230
#define mcaRecordR31LO	231
#define mcaRecordR31HI	232
#define mcaRecordR31BG	233
#define mcaRecordR31IP	234
#define mcaRecordR0	235
#define mcaRecordR0N	236
#define mcaRecordR0P	237
#define mcaRecordR1	238
#define mcaRecordR1N	239
#define mcaRecordR1P	240
#define mcaRecordR2	241
#define mcaRecordR2N	242
#define mcaRecordR2P	243
#define mcaRecordR3	244
#define mcaRecordR3N	245
#define mcaRecordR3P	246
#define mcaRecordR4	247
#define mcaRecordR4N	248
#define mcaRecordR4P	249
#define mcaRecordR5	250
#define mcaRecordR5N	251
#define mcaRecordR5P	252
#define mcaRecordR6	253
#define mcaRecordR6N	254
#define mcaRecordR6P	255
#define mcaRecordR7	256
#define mcaRecordR7N	257
#define mcaRecordR7P	258
#define mcaRecordR8	259
#define mcaRecordR8N	260
#define mcaRecordR8P	261
#define mcaRecordR9	262
#define mcaRecordR9N	263
#define mcaRecordR9P	264
#define mcaRecordR10	265
#define mcaRecordR10N	266
#define mcaRecordR10P	267
#define mcaRecordR11	268
#define mcaRecordR11N	269
#define mcaRecordR11P	270
#define mcaRecordR12	271
#define mcaRecordR12N	272
#define mcaRecordR12P	273
#define mcaRecordR13	274
#define mcaRecordR13N	275
#define mcaRecordR13P	276
#define mcaRecordR14	277
#define mcaRecordR14N	278
#define mcaRecordR14P	279
#define mcaRecordR15	280
#define mcaRecordR15N	281
#define mcaRecordR15P	282
#define mcaRecordR16	283
#define mcaRecordR16N	284
#define mcaRecordR16P	285
#define mcaRecordR17	286
#define mcaRecordR17N	287
#define mcaRecordR17P	288
#define mcaRecordR18	289
#define mcaRecordR18N	290
#define mcaRecordR18P	291
#define mcaRecordR19	292
#define mcaRecordR19N	293
#define mcaRecordR19P	294
#define mcaRecordR20	295
#define mcaRecordR20N	296
#define mcaRecordR20P	297
#define mcaRecordR21	298
#define mcaRecordR21N	299
#define mcaRecordR21P	300
#define mcaRecordR22	301
#define mcaRecordR22N	302
#define mcaRecordR22P	303
#define mcaRecordR23	304
#define mcaRecordR23N	305
#define mcaRecordR23P	306
#define mcaRecordR24	307
#define mcaRecordR24N	308
#define mcaRecordR24P	309
#define mcaRecordR25	310
#define mcaRecordR25N	311
#define mcaRecordR25P	312
#define mcaRecordR26	313
#define mcaRecordR26N	314
#define mcaRecordR26P	315
#define mcaRecordR27	316
#define mcaRecordR27N	317
#define mcaRecordR27P	318
#define mcaRecordR28	319
#define mcaRecordR28N	320
#define mcaRecordR28P	321
#define mcaRecordR29	322
#define mcaRecordR29N	323
#define mcaRecordR29P	324
#define mcaRecordR30	325
#define mcaRecordR30N	326
#define mcaRecordR30P	327
#define mcaRecordR31	328
#define mcaRecordR31N	329
#define mcaRecordR31P	330
#define mcaRecordR0NM	331
#define mcaRecordR1NM	332
#define mcaRecordR2NM	333
#define mcaRecordR3NM	334
#define mcaRecordR4NM	335
#define mcaRecordR5NM	336
#define mcaRecordR6NM	337
#define mcaRecordR7NM	338
#define mcaRecordR8NM	339
#define mcaRecordR9NM	340
#define mcaRecordR10NM	341
#define mcaRecordR11NM	342
#define mcaRecordR12NM	343
#define mcaRecordR13NM	344
#define mcaRecordR14NM	345
#define mcaRecordR15NM	346
#define mcaRecordR16NM	347
#define mcaRecordR17NM	348
#define mcaRecordR18NM	349
#define mcaRecordR19NM	350
#define mcaRecordR20NM	351
#define mcaRecordR21NM	352
#define mcaRecordR22NM	353
#define mcaRecordR23NM	354
#define mcaRecordR24NM	355
#define mcaRecordR25NM	356
#define mcaRecordR26NM	357
#define mcaRecordR27NM	358
#define mcaRecordR28NM	359
#define mcaRecordR29NM	360
#define mcaRecordR30NM	361
#define mcaRecordR31NM	362
#endif /*INCmcaH*/
#ifdef GEN_SIZE_OFFSET
#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int mcaRecordSizeOffset(dbRecordType *pdbRecordType)
{
    mcaRecord *prec = 0;
  pdbRecordType->papFldDes[0]->size=sizeof(prec->name);
  pdbRecordType->papFldDes[0]->offset=(short)((char *)&prec->name - (char *)prec);
  pdbRecordType->papFldDes[1]->size=sizeof(prec->desc);
  pdbRecordType->papFldDes[1]->offset=(short)((char *)&prec->desc - (char *)prec);
  pdbRecordType->papFldDes[2]->size=sizeof(prec->asg);
  pdbRecordType->papFldDes[2]->offset=(short)((char *)&prec->asg - (char *)prec);
  pdbRecordType->papFldDes[3]->size=sizeof(prec->scan);
  pdbRecordType->papFldDes[3]->offset=(short)((char *)&prec->scan - (char *)prec);
  pdbRecordType->papFldDes[4]->size=sizeof(prec->pini);
  pdbRecordType->papFldDes[4]->offset=(short)((char *)&prec->pini - (char *)prec);
  pdbRecordType->papFldDes[5]->size=sizeof(prec->phas);
  pdbRecordType->papFldDes[5]->offset=(short)((char *)&prec->phas - (char *)prec);
  pdbRecordType->papFldDes[6]->size=sizeof(prec->evnt);
  pdbRecordType->papFldDes[6]->offset=(short)((char *)&prec->evnt - (char *)prec);
  pdbRecordType->papFldDes[7]->size=sizeof(prec->tse);
  pdbRecordType->papFldDes[7]->offset=(short)((char *)&prec->tse - (char *)prec);
  pdbRecordType->papFldDes[8]->size=sizeof(prec->tsel);
  pdbRecordType->papFldDes[8]->offset=(short)((char *)&prec->tsel - (char *)prec);
  pdbRecordType->papFldDes[9]->size=sizeof(prec->dtyp);
  pdbRecordType->papFldDes[9]->offset=(short)((char *)&prec->dtyp - (char *)prec);
  pdbRecordType->papFldDes[10]->size=sizeof(prec->disv);
  pdbRecordType->papFldDes[10]->offset=(short)((char *)&prec->disv - (char *)prec);
  pdbRecordType->papFldDes[11]->size=sizeof(prec->disa);
  pdbRecordType->papFldDes[11]->offset=(short)((char *)&prec->disa - (char *)prec);
  pdbRecordType->papFldDes[12]->size=sizeof(prec->sdis);
  pdbRecordType->papFldDes[12]->offset=(short)((char *)&prec->sdis - (char *)prec);
  pdbRecordType->papFldDes[13]->size=sizeof(prec->mlok);
  pdbRecordType->papFldDes[13]->offset=(short)((char *)&prec->mlok - (char *)prec);
  pdbRecordType->papFldDes[14]->size=sizeof(prec->mlis);
  pdbRecordType->papFldDes[14]->offset=(short)((char *)&prec->mlis - (char *)prec);
  pdbRecordType->papFldDes[15]->size=sizeof(prec->disp);
  pdbRecordType->papFldDes[15]->offset=(short)((char *)&prec->disp - (char *)prec);
  pdbRecordType->papFldDes[16]->size=sizeof(prec->proc);
  pdbRecordType->papFldDes[16]->offset=(short)((char *)&prec->proc - (char *)prec);
  pdbRecordType->papFldDes[17]->size=sizeof(prec->stat);
  pdbRecordType->papFldDes[17]->offset=(short)((char *)&prec->stat - (char *)prec);
  pdbRecordType->papFldDes[18]->size=sizeof(prec->sevr);
  pdbRecordType->papFldDes[18]->offset=(short)((char *)&prec->sevr - (char *)prec);
  pdbRecordType->papFldDes[19]->size=sizeof(prec->nsta);
  pdbRecordType->papFldDes[19]->offset=(short)((char *)&prec->nsta - (char *)prec);
  pdbRecordType->papFldDes[20]->size=sizeof(prec->nsev);
  pdbRecordType->papFldDes[20]->offset=(short)((char *)&prec->nsev - (char *)prec);
  pdbRecordType->papFldDes[21]->size=sizeof(prec->acks);
  pdbRecordType->papFldDes[21]->offset=(short)((char *)&prec->acks - (char *)prec);
  pdbRecordType->papFldDes[22]->size=sizeof(prec->ackt);
  pdbRecordType->papFldDes[22]->offset=(short)((char *)&prec->ackt - (char *)prec);
  pdbRecordType->papFldDes[23]->size=sizeof(prec->diss);
  pdbRecordType->papFldDes[23]->offset=(short)((char *)&prec->diss - (char *)prec);
  pdbRecordType->papFldDes[24]->size=sizeof(prec->lcnt);
  pdbRecordType->papFldDes[24]->offset=(short)((char *)&prec->lcnt - (char *)prec);
  pdbRecordType->papFldDes[25]->size=sizeof(prec->pact);
  pdbRecordType->papFldDes[25]->offset=(short)((char *)&prec->pact - (char *)prec);
  pdbRecordType->papFldDes[26]->size=sizeof(prec->putf);
  pdbRecordType->papFldDes[26]->offset=(short)((char *)&prec->putf - (char *)prec);
  pdbRecordType->papFldDes[27]->size=sizeof(prec->rpro);
  pdbRecordType->papFldDes[27]->offset=(short)((char *)&prec->rpro - (char *)prec);
  pdbRecordType->papFldDes[28]->size=sizeof(prec->asp);
  pdbRecordType->papFldDes[28]->offset=(short)((char *)&prec->asp - (char *)prec);
  pdbRecordType->papFldDes[29]->size=sizeof(prec->ppn);
  pdbRecordType->papFldDes[29]->offset=(short)((char *)&prec->ppn - (char *)prec);
  pdbRecordType->papFldDes[30]->size=sizeof(prec->ppnr);
  pdbRecordType->papFldDes[30]->offset=(short)((char *)&prec->ppnr - (char *)prec);
  pdbRecordType->papFldDes[31]->size=sizeof(prec->spvt);
  pdbRecordType->papFldDes[31]->offset=(short)((char *)&prec->spvt - (char *)prec);
  pdbRecordType->papFldDes[32]->size=sizeof(prec->rset);
  pdbRecordType->papFldDes[32]->offset=(short)((char *)&prec->rset - (char *)prec);
  pdbRecordType->papFldDes[33]->size=sizeof(prec->dset);
  pdbRecordType->papFldDes[33]->offset=(short)((char *)&prec->dset - (char *)prec);
  pdbRecordType->papFldDes[34]->size=sizeof(prec->dpvt);
  pdbRecordType->papFldDes[34]->offset=(short)((char *)&prec->dpvt - (char *)prec);
  pdbRecordType->papFldDes[35]->size=sizeof(prec->rdes);
  pdbRecordType->papFldDes[35]->offset=(short)((char *)&prec->rdes - (char *)prec);
  pdbRecordType->papFldDes[36]->size=sizeof(prec->lset);
  pdbRecordType->papFldDes[36]->offset=(short)((char *)&prec->lset - (char *)prec);
  pdbRecordType->papFldDes[37]->size=sizeof(prec->prio);
  pdbRecordType->papFldDes[37]->offset=(short)((char *)&prec->prio - (char *)prec);
  pdbRecordType->papFldDes[38]->size=sizeof(prec->tpro);
  pdbRecordType->papFldDes[38]->offset=(short)((char *)&prec->tpro - (char *)prec);
  pdbRecordType->papFldDes[39]->size=sizeof(prec->bkpt);
  pdbRecordType->papFldDes[39]->offset=(short)((char *)&prec->bkpt - (char *)prec);
  pdbRecordType->papFldDes[40]->size=sizeof(prec->udf);
  pdbRecordType->papFldDes[40]->offset=(short)((char *)&prec->udf - (char *)prec);
  pdbRecordType->papFldDes[41]->size=sizeof(prec->time);
  pdbRecordType->papFldDes[41]->offset=(short)((char *)&prec->time - (char *)prec);
  pdbRecordType->papFldDes[42]->size=sizeof(prec->flnk);
  pdbRecordType->papFldDes[42]->offset=(short)((char *)&prec->flnk - (char *)prec);
  pdbRecordType->papFldDes[43]->size=sizeof(prec->vers);
  pdbRecordType->papFldDes[43]->offset=(short)((char *)&prec->vers - (char *)prec);
  pdbRecordType->papFldDes[44]->size=sizeof(prec->val);
  pdbRecordType->papFldDes[44]->offset=(short)((char *)&prec->val - (char *)prec);
  pdbRecordType->papFldDes[45]->size=sizeof(prec->bptr);
  pdbRecordType->papFldDes[45]->offset=(short)((char *)&prec->bptr - (char *)prec);
  pdbRecordType->papFldDes[46]->size=sizeof(prec->bg);
  pdbRecordType->papFldDes[46]->offset=(short)((char *)&prec->bg - (char *)prec);
  pdbRecordType->papFldDes[47]->size=sizeof(prec->pbg);
  pdbRecordType->papFldDes[47]->offset=(short)((char *)&prec->pbg - (char *)prec);
  pdbRecordType->papFldDes[48]->size=sizeof(prec->pstatus);
  pdbRecordType->papFldDes[48]->offset=(short)((char *)&prec->pstatus - (char *)prec);
  pdbRecordType->papFldDes[49]->size=sizeof(prec->hopr);
  pdbRecordType->papFldDes[49]->offset=(short)((char *)&prec->hopr - (char *)prec);
  pdbRecordType->papFldDes[50]->size=sizeof(prec->lopr);
  pdbRecordType->papFldDes[50]->offset=(short)((char *)&prec->lopr - (char *)prec);
  pdbRecordType->papFldDes[51]->size=sizeof(prec->nmax);
  pdbRecordType->papFldDes[51]->offset=(short)((char *)&prec->nmax - (char *)prec);
  pdbRecordType->papFldDes[52]->size=sizeof(prec->nord);
  pdbRecordType->papFldDes[52]->offset=(short)((char *)&prec->nord - (char *)prec);
  pdbRecordType->papFldDes[53]->size=sizeof(prec->prec);
  pdbRecordType->papFldDes[53]->offset=(short)((char *)&prec->prec - (char *)prec);
  pdbRecordType->papFldDes[54]->size=sizeof(prec->ftvl);
  pdbRecordType->papFldDes[54]->offset=(short)((char *)&prec->ftvl - (char *)prec);
  pdbRecordType->papFldDes[55]->size=sizeof(prec->strt);
  pdbRecordType->papFldDes[55]->offset=(short)((char *)&prec->strt - (char *)prec);
  pdbRecordType->papFldDes[56]->size=sizeof(prec->erst);
  pdbRecordType->papFldDes[56]->offset=(short)((char *)&prec->erst - (char *)prec);
  pdbRecordType->papFldDes[57]->size=sizeof(prec->stop);
  pdbRecordType->papFldDes[57]->offset=(short)((char *)&prec->stop - (char *)prec);
  pdbRecordType->papFldDes[58]->size=sizeof(prec->acqg);
  pdbRecordType->papFldDes[58]->offset=(short)((char *)&prec->acqg - (char *)prec);
  pdbRecordType->papFldDes[59]->size=sizeof(prec->read);
  pdbRecordType->papFldDes[59]->offset=(short)((char *)&prec->read - (char *)prec);
  pdbRecordType->papFldDes[60]->size=sizeof(prec->rdng);
  pdbRecordType->papFldDes[60]->offset=(short)((char *)&prec->rdng - (char *)prec);
  pdbRecordType->papFldDes[61]->size=sizeof(prec->rdns);
  pdbRecordType->papFldDes[61]->offset=(short)((char *)&prec->rdns - (char *)prec);
  pdbRecordType->papFldDes[62]->size=sizeof(prec->eras);
  pdbRecordType->papFldDes[62]->offset=(short)((char *)&prec->eras - (char *)prec);
  pdbRecordType->papFldDes[63]->size=sizeof(prec->chas);
  pdbRecordType->papFldDes[63]->offset=(short)((char *)&prec->chas - (char *)prec);
  pdbRecordType->papFldDes[64]->size=sizeof(prec->nuse);
  pdbRecordType->papFldDes[64]->offset=(short)((char *)&prec->nuse - (char *)prec);
  pdbRecordType->papFldDes[65]->size=sizeof(prec->seq);
  pdbRecordType->papFldDes[65]->offset=(short)((char *)&prec->seq - (char *)prec);
  pdbRecordType->papFldDes[66]->size=sizeof(prec->dwel);
  pdbRecordType->papFldDes[66]->offset=(short)((char *)&prec->dwel - (char *)prec);
  pdbRecordType->papFldDes[67]->size=sizeof(prec->pscl);
  pdbRecordType->papFldDes[67]->offset=(short)((char *)&prec->pscl - (char *)prec);
  pdbRecordType->papFldDes[68]->size=sizeof(prec->prtm);
  pdbRecordType->papFldDes[68]->offset=(short)((char *)&prec->prtm - (char *)prec);
  pdbRecordType->papFldDes[69]->size=sizeof(prec->pltm);
  pdbRecordType->papFldDes[69]->offset=(short)((char *)&prec->pltm - (char *)prec);
  pdbRecordType->papFldDes[70]->size=sizeof(prec->pct);
  pdbRecordType->papFldDes[70]->offset=(short)((char *)&prec->pct - (char *)prec);
  pdbRecordType->papFldDes[71]->size=sizeof(prec->pctl);
  pdbRecordType->papFldDes[71]->offset=(short)((char *)&prec->pctl - (char *)prec);
  pdbRecordType->papFldDes[72]->size=sizeof(prec->pcth);
  pdbRecordType->papFldDes[72]->offset=(short)((char *)&prec->pcth - (char *)prec);
  pdbRecordType->papFldDes[73]->size=sizeof(prec->pswp);
  pdbRecordType->papFldDes[73]->offset=(short)((char *)&prec->pswp - (char *)prec);
  pdbRecordType->papFldDes[74]->size=sizeof(prec->mode);
  pdbRecordType->papFldDes[74]->offset=(short)((char *)&prec->mode - (char *)prec);
  pdbRecordType->papFldDes[75]->size=sizeof(prec->calo);
  pdbRecordType->papFldDes[75]->offset=(short)((char *)&prec->calo - (char *)prec);
  pdbRecordType->papFldDes[76]->size=sizeof(prec->cals);
  pdbRecordType->papFldDes[76]->offset=(short)((char *)&prec->cals - (char *)prec);
  pdbRecordType->papFldDes[77]->size=sizeof(prec->calq);
  pdbRecordType->papFldDes[77]->offset=(short)((char *)&prec->calq - (char *)prec);
  pdbRecordType->papFldDes[78]->size=sizeof(prec->egu);
  pdbRecordType->papFldDes[78]->offset=(short)((char *)&prec->egu - (char *)prec);
  pdbRecordType->papFldDes[79]->size=sizeof(prec->tth);
  pdbRecordType->papFldDes[79]->offset=(short)((char *)&prec->tth - (char *)prec);
  pdbRecordType->papFldDes[80]->size=sizeof(prec->ertm);
  pdbRecordType->papFldDes[80]->offset=(short)((char *)&prec->ertm - (char *)prec);
  pdbRecordType->papFldDes[81]->size=sizeof(prec->eltm);
  pdbRecordType->papFldDes[81]->offset=(short)((char *)&prec->eltm - (char *)prec);
  pdbRecordType->papFldDes[82]->size=sizeof(prec->dtim);
  pdbRecordType->papFldDes[82]->offset=(short)((char *)&prec->dtim - (char *)prec);
  pdbRecordType->papFldDes[83]->size=sizeof(prec->idtim);
  pdbRecordType->papFldDes[83]->offset=(short)((char *)&prec->idtim - (char *)prec);
  pdbRecordType->papFldDes[84]->size=sizeof(prec->stim);
  pdbRecordType->papFldDes[84]->offset=(short)((char *)&prec->stim - (char *)prec);
  pdbRecordType->papFldDes[85]->size=sizeof(prec->rtim);
  pdbRecordType->papFldDes[85]->offset=(short)((char *)&prec->rtim - (char *)prec);
  pdbRecordType->papFldDes[86]->size=sizeof(prec->act);
  pdbRecordType->papFldDes[86]->offset=(short)((char *)&prec->act - (char *)prec);
  pdbRecordType->papFldDes[87]->size=sizeof(prec->nack);
  pdbRecordType->papFldDes[87]->offset=(short)((char *)&prec->nack - (char *)prec);
  pdbRecordType->papFldDes[88]->size=sizeof(prec->inp);
  pdbRecordType->papFldDes[88]->offset=(short)((char *)&prec->inp - (char *)prec);
  pdbRecordType->papFldDes[89]->size=sizeof(prec->hihi);
  pdbRecordType->papFldDes[89]->offset=(short)((char *)&prec->hihi - (char *)prec);
  pdbRecordType->papFldDes[90]->size=sizeof(prec->lolo);
  pdbRecordType->papFldDes[90]->offset=(short)((char *)&prec->lolo - (char *)prec);
  pdbRecordType->papFldDes[91]->size=sizeof(prec->high);
  pdbRecordType->papFldDes[91]->offset=(short)((char *)&prec->high - (char *)prec);
  pdbRecordType->papFldDes[92]->size=sizeof(prec->low);
  pdbRecordType->papFldDes[92]->offset=(short)((char *)&prec->low - (char *)prec);
  pdbRecordType->papFldDes[93]->size=sizeof(prec->hhsv);
  pdbRecordType->papFldDes[93]->offset=(short)((char *)&prec->hhsv - (char *)prec);
  pdbRecordType->papFldDes[94]->size=sizeof(prec->llsv);
  pdbRecordType->papFldDes[94]->offset=(short)((char *)&prec->llsv - (char *)prec);
  pdbRecordType->papFldDes[95]->size=sizeof(prec->hsv);
  pdbRecordType->papFldDes[95]->offset=(short)((char *)&prec->hsv - (char *)prec);
  pdbRecordType->papFldDes[96]->size=sizeof(prec->lsv);
  pdbRecordType->papFldDes[96]->offset=(short)((char *)&prec->lsv - (char *)prec);
  pdbRecordType->papFldDes[97]->size=sizeof(prec->hyst);
  pdbRecordType->papFldDes[97]->offset=(short)((char *)&prec->hyst - (char *)prec);
  pdbRecordType->papFldDes[98]->size=sizeof(prec->lalm);
  pdbRecordType->papFldDes[98]->offset=(short)((char *)&prec->lalm - (char *)prec);
  pdbRecordType->papFldDes[99]->size=sizeof(prec->siol);
  pdbRecordType->papFldDes[99]->offset=(short)((char *)&prec->siol - (char *)prec);
  pdbRecordType->papFldDes[100]->size=sizeof(prec->siml);
  pdbRecordType->papFldDes[100]->offset=(short)((char *)&prec->siml - (char *)prec);
  pdbRecordType->papFldDes[101]->size=sizeof(prec->simm);
  pdbRecordType->papFldDes[101]->offset=(short)((char *)&prec->simm - (char *)prec);
  pdbRecordType->papFldDes[102]->size=sizeof(prec->sims);
  pdbRecordType->papFldDes[102]->offset=(short)((char *)&prec->sims - (char *)prec);
  pdbRecordType->papFldDes[103]->size=sizeof(prec->mmap);
  pdbRecordType->papFldDes[103]->offset=(short)((char *)&prec->mmap - (char *)prec);
  pdbRecordType->papFldDes[104]->size=sizeof(prec->rmap);
  pdbRecordType->papFldDes[104]->offset=(short)((char *)&prec->rmap - (char *)prec);
  pdbRecordType->papFldDes[105]->size=sizeof(prec->newv);
  pdbRecordType->papFldDes[105]->offset=(short)((char *)&prec->newv - (char *)prec);
  pdbRecordType->papFldDes[106]->size=sizeof(prec->newr);
  pdbRecordType->papFldDes[106]->offset=(short)((char *)&prec->newr - (char *)prec);
  pdbRecordType->papFldDes[107]->size=sizeof(prec->r0lo);
  pdbRecordType->papFldDes[107]->offset=(short)((char *)&prec->r0lo - (char *)prec);
  pdbRecordType->papFldDes[108]->size=sizeof(prec->r0hi);
  pdbRecordType->papFldDes[108]->offset=(short)((char *)&prec->r0hi - (char *)prec);
  pdbRecordType->papFldDes[109]->size=sizeof(prec->r0bg);
  pdbRecordType->papFldDes[109]->offset=(short)((char *)&prec->r0bg - (char *)prec);
  pdbRecordType->papFldDes[110]->size=sizeof(prec->r0ip);
  pdbRecordType->papFldDes[110]->offset=(short)((char *)&prec->r0ip - (char *)prec);
  pdbRecordType->papFldDes[111]->size=sizeof(prec->r1lo);
  pdbRecordType->papFldDes[111]->offset=(short)((char *)&prec->r1lo - (char *)prec);
  pdbRecordType->papFldDes[112]->size=sizeof(prec->r1hi);
  pdbRecordType->papFldDes[112]->offset=(short)((char *)&prec->r1hi - (char *)prec);
  pdbRecordType->papFldDes[113]->size=sizeof(prec->r1bg);
  pdbRecordType->papFldDes[113]->offset=(short)((char *)&prec->r1bg - (char *)prec);
  pdbRecordType->papFldDes[114]->size=sizeof(prec->r1ip);
  pdbRecordType->papFldDes[114]->offset=(short)((char *)&prec->r1ip - (char *)prec);
  pdbRecordType->papFldDes[115]->size=sizeof(prec->r2lo);
  pdbRecordType->papFldDes[115]->offset=(short)((char *)&prec->r2lo - (char *)prec);
  pdbRecordType->papFldDes[116]->size=sizeof(prec->r2hi);
  pdbRecordType->papFldDes[116]->offset=(short)((char *)&prec->r2hi - (char *)prec);
  pdbRecordType->papFldDes[117]->size=sizeof(prec->r2bg);
  pdbRecordType->papFldDes[117]->offset=(short)((char *)&prec->r2bg - (char *)prec);
  pdbRecordType->papFldDes[118]->size=sizeof(prec->r2ip);
  pdbRecordType->papFldDes[118]->offset=(short)((char *)&prec->r2ip - (char *)prec);
  pdbRecordType->papFldDes[119]->size=sizeof(prec->r3lo);
  pdbRecordType->papFldDes[119]->offset=(short)((char *)&prec->r3lo - (char *)prec);
  pdbRecordType->papFldDes[120]->size=sizeof(prec->r3hi);
  pdbRecordType->papFldDes[120]->offset=(short)((char *)&prec->r3hi - (char *)prec);
  pdbRecordType->papFldDes[121]->size=sizeof(prec->r3bg);
  pdbRecordType->papFldDes[121]->offset=(short)((char *)&prec->r3bg - (char *)prec);
  pdbRecordType->papFldDes[122]->size=sizeof(prec->r3ip);
  pdbRecordType->papFldDes[122]->offset=(short)((char *)&prec->r3ip - (char *)prec);
  pdbRecordType->papFldDes[123]->size=sizeof(prec->r4lo);
  pdbRecordType->papFldDes[123]->offset=(short)((char *)&prec->r4lo - (char *)prec);
  pdbRecordType->papFldDes[124]->size=sizeof(prec->r4hi);
  pdbRecordType->papFldDes[124]->offset=(short)((char *)&prec->r4hi - (char *)prec);
  pdbRecordType->papFldDes[125]->size=sizeof(prec->r4bg);
  pdbRecordType->papFldDes[125]->offset=(short)((char *)&prec->r4bg - (char *)prec);
  pdbRecordType->papFldDes[126]->size=sizeof(prec->r4ip);
  pdbRecordType->papFldDes[126]->offset=(short)((char *)&prec->r4ip - (char *)prec);
  pdbRecordType->papFldDes[127]->size=sizeof(prec->r5lo);
  pdbRecordType->papFldDes[127]->offset=(short)((char *)&prec->r5lo - (char *)prec);
  pdbRecordType->papFldDes[128]->size=sizeof(prec->r5hi);
  pdbRecordType->papFldDes[128]->offset=(short)((char *)&prec->r5hi - (char *)prec);
  pdbRecordType->papFldDes[129]->size=sizeof(prec->r5bg);
  pdbRecordType->papFldDes[129]->offset=(short)((char *)&prec->r5bg - (char *)prec);
  pdbRecordType->papFldDes[130]->size=sizeof(prec->r5ip);
  pdbRecordType->papFldDes[130]->offset=(short)((char *)&prec->r5ip - (char *)prec);
  pdbRecordType->papFldDes[131]->size=sizeof(prec->r6lo);
  pdbRecordType->papFldDes[131]->offset=(short)((char *)&prec->r6lo - (char *)prec);
  pdbRecordType->papFldDes[132]->size=sizeof(prec->r6hi);
  pdbRecordType->papFldDes[132]->offset=(short)((char *)&prec->r6hi - (char *)prec);
  pdbRecordType->papFldDes[133]->size=sizeof(prec->r6bg);
  pdbRecordType->papFldDes[133]->offset=(short)((char *)&prec->r6bg - (char *)prec);
  pdbRecordType->papFldDes[134]->size=sizeof(prec->r6ip);
  pdbRecordType->papFldDes[134]->offset=(short)((char *)&prec->r6ip - (char *)prec);
  pdbRecordType->papFldDes[135]->size=sizeof(prec->r7lo);
  pdbRecordType->papFldDes[135]->offset=(short)((char *)&prec->r7lo - (char *)prec);
  pdbRecordType->papFldDes[136]->size=sizeof(prec->r7hi);
  pdbRecordType->papFldDes[136]->offset=(short)((char *)&prec->r7hi - (char *)prec);
  pdbRecordType->papFldDes[137]->size=sizeof(prec->r7bg);
  pdbRecordType->papFldDes[137]->offset=(short)((char *)&prec->r7bg - (char *)prec);
  pdbRecordType->papFldDes[138]->size=sizeof(prec->r7ip);
  pdbRecordType->papFldDes[138]->offset=(short)((char *)&prec->r7ip - (char *)prec);
  pdbRecordType->papFldDes[139]->size=sizeof(prec->r8lo);
  pdbRecordType->papFldDes[139]->offset=(short)((char *)&prec->r8lo - (char *)prec);
  pdbRecordType->papFldDes[140]->size=sizeof(prec->r8hi);
  pdbRecordType->papFldDes[140]->offset=(short)((char *)&prec->r8hi - (char *)prec);
  pdbRecordType->papFldDes[141]->size=sizeof(prec->r8bg);
  pdbRecordType->papFldDes[141]->offset=(short)((char *)&prec->r8bg - (char *)prec);
  pdbRecordType->papFldDes[142]->size=sizeof(prec->r8ip);
  pdbRecordType->papFldDes[142]->offset=(short)((char *)&prec->r8ip - (char *)prec);
  pdbRecordType->papFldDes[143]->size=sizeof(prec->r9lo);
  pdbRecordType->papFldDes[143]->offset=(short)((char *)&prec->r9lo - (char *)prec);
  pdbRecordType->papFldDes[144]->size=sizeof(prec->r9hi);
  pdbRecordType->papFldDes[144]->offset=(short)((char *)&prec->r9hi - (char *)prec);
  pdbRecordType->papFldDes[145]->size=sizeof(prec->r9bg);
  pdbRecordType->papFldDes[145]->offset=(short)((char *)&prec->r9bg - (char *)prec);
  pdbRecordType->papFldDes[146]->size=sizeof(prec->r9ip);
  pdbRecordType->papFldDes[146]->offset=(short)((char *)&prec->r9ip - (char *)prec);
  pdbRecordType->papFldDes[147]->size=sizeof(prec->r10lo);
  pdbRecordType->papFldDes[147]->offset=(short)((char *)&prec->r10lo - (char *)prec);
  pdbRecordType->papFldDes[148]->size=sizeof(prec->r10hi);
  pdbRecordType->papFldDes[148]->offset=(short)((char *)&prec->r10hi - (char *)prec);
  pdbRecordType->papFldDes[149]->size=sizeof(prec->r10bg);
  pdbRecordType->papFldDes[149]->offset=(short)((char *)&prec->r10bg - (char *)prec);
  pdbRecordType->papFldDes[150]->size=sizeof(prec->r10ip);
  pdbRecordType->papFldDes[150]->offset=(short)((char *)&prec->r10ip - (char *)prec);
  pdbRecordType->papFldDes[151]->size=sizeof(prec->r11lo);
  pdbRecordType->papFldDes[151]->offset=(short)((char *)&prec->r11lo - (char *)prec);
  pdbRecordType->papFldDes[152]->size=sizeof(prec->r11hi);
  pdbRecordType->papFldDes[152]->offset=(short)((char *)&prec->r11hi - (char *)prec);
  pdbRecordType->papFldDes[153]->size=sizeof(prec->r11bg);
  pdbRecordType->papFldDes[153]->offset=(short)((char *)&prec->r11bg - (char *)prec);
  pdbRecordType->papFldDes[154]->size=sizeof(prec->r11ip);
  pdbRecordType->papFldDes[154]->offset=(short)((char *)&prec->r11ip - (char *)prec);
  pdbRecordType->papFldDes[155]->size=sizeof(prec->r12lo);
  pdbRecordType->papFldDes[155]->offset=(short)((char *)&prec->r12lo - (char *)prec);
  pdbRecordType->papFldDes[156]->size=sizeof(prec->r12hi);
  pdbRecordType->papFldDes[156]->offset=(short)((char *)&prec->r12hi - (char *)prec);
  pdbRecordType->papFldDes[157]->size=sizeof(prec->r12bg);
  pdbRecordType->papFldDes[157]->offset=(short)((char *)&prec->r12bg - (char *)prec);
  pdbRecordType->papFldDes[158]->size=sizeof(prec->r12ip);
  pdbRecordType->papFldDes[158]->offset=(short)((char *)&prec->r12ip - (char *)prec);
  pdbRecordType->papFldDes[159]->size=sizeof(prec->r13lo);
  pdbRecordType->papFldDes[159]->offset=(short)((char *)&prec->r13lo - (char *)prec);
  pdbRecordType->papFldDes[160]->size=sizeof(prec->r13hi);
  pdbRecordType->papFldDes[160]->offset=(short)((char *)&prec->r13hi - (char *)prec);
  pdbRecordType->papFldDes[161]->size=sizeof(prec->r13bg);
  pdbRecordType->papFldDes[161]->offset=(short)((char *)&prec->r13bg - (char *)prec);
  pdbRecordType->papFldDes[162]->size=sizeof(prec->r13ip);
  pdbRecordType->papFldDes[162]->offset=(short)((char *)&prec->r13ip - (char *)prec);
  pdbRecordType->papFldDes[163]->size=sizeof(prec->r14lo);
  pdbRecordType->papFldDes[163]->offset=(short)((char *)&prec->r14lo - (char *)prec);
  pdbRecordType->papFldDes[164]->size=sizeof(prec->r14hi);
  pdbRecordType->papFldDes[164]->offset=(short)((char *)&prec->r14hi - (char *)prec);
  pdbRecordType->papFldDes[165]->size=sizeof(prec->r14bg);
  pdbRecordType->papFldDes[165]->offset=(short)((char *)&prec->r14bg - (char *)prec);
  pdbRecordType->papFldDes[166]->size=sizeof(prec->r14ip);
  pdbRecordType->papFldDes[166]->offset=(short)((char *)&prec->r14ip - (char *)prec);
  pdbRecordType->papFldDes[167]->size=sizeof(prec->r15lo);
  pdbRecordType->papFldDes[167]->offset=(short)((char *)&prec->r15lo - (char *)prec);
  pdbRecordType->papFldDes[168]->size=sizeof(prec->r15hi);
  pdbRecordType->papFldDes[168]->offset=(short)((char *)&prec->r15hi - (char *)prec);
  pdbRecordType->papFldDes[169]->size=sizeof(prec->r15bg);
  pdbRecordType->papFldDes[169]->offset=(short)((char *)&prec->r15bg - (char *)prec);
  pdbRecordType->papFldDes[170]->size=sizeof(prec->r15ip);
  pdbRecordType->papFldDes[170]->offset=(short)((char *)&prec->r15ip - (char *)prec);
  pdbRecordType->papFldDes[171]->size=sizeof(prec->r16lo);
  pdbRecordType->papFldDes[171]->offset=(short)((char *)&prec->r16lo - (char *)prec);
  pdbRecordType->papFldDes[172]->size=sizeof(prec->r16hi);
  pdbRecordType->papFldDes[172]->offset=(short)((char *)&prec->r16hi - (char *)prec);
  pdbRecordType->papFldDes[173]->size=sizeof(prec->r16bg);
  pdbRecordType->papFldDes[173]->offset=(short)((char *)&prec->r16bg - (char *)prec);
  pdbRecordType->papFldDes[174]->size=sizeof(prec->r16ip);
  pdbRecordType->papFldDes[174]->offset=(short)((char *)&prec->r16ip - (char *)prec);
  pdbRecordType->papFldDes[175]->size=sizeof(prec->r17lo);
  pdbRecordType->papFldDes[175]->offset=(short)((char *)&prec->r17lo - (char *)prec);
  pdbRecordType->papFldDes[176]->size=sizeof(prec->r17hi);
  pdbRecordType->papFldDes[176]->offset=(short)((char *)&prec->r17hi - (char *)prec);
  pdbRecordType->papFldDes[177]->size=sizeof(prec->r17bg);
  pdbRecordType->papFldDes[177]->offset=(short)((char *)&prec->r17bg - (char *)prec);
  pdbRecordType->papFldDes[178]->size=sizeof(prec->r17ip);
  pdbRecordType->papFldDes[178]->offset=(short)((char *)&prec->r17ip - (char *)prec);
  pdbRecordType->papFldDes[179]->size=sizeof(prec->r18lo);
  pdbRecordType->papFldDes[179]->offset=(short)((char *)&prec->r18lo - (char *)prec);
  pdbRecordType->papFldDes[180]->size=sizeof(prec->r18hi);
  pdbRecordType->papFldDes[180]->offset=(short)((char *)&prec->r18hi - (char *)prec);
  pdbRecordType->papFldDes[181]->size=sizeof(prec->r18bg);
  pdbRecordType->papFldDes[181]->offset=(short)((char *)&prec->r18bg - (char *)prec);
  pdbRecordType->papFldDes[182]->size=sizeof(prec->r18ip);
  pdbRecordType->papFldDes[182]->offset=(short)((char *)&prec->r18ip - (char *)prec);
  pdbRecordType->papFldDes[183]->size=sizeof(prec->r19lo);
  pdbRecordType->papFldDes[183]->offset=(short)((char *)&prec->r19lo - (char *)prec);
  pdbRecordType->papFldDes[184]->size=sizeof(prec->r19hi);
  pdbRecordType->papFldDes[184]->offset=(short)((char *)&prec->r19hi - (char *)prec);
  pdbRecordType->papFldDes[185]->size=sizeof(prec->r19bg);
  pdbRecordType->papFldDes[185]->offset=(short)((char *)&prec->r19bg - (char *)prec);
  pdbRecordType->papFldDes[186]->size=sizeof(prec->r19ip);
  pdbRecordType->papFldDes[186]->offset=(short)((char *)&prec->r19ip - (char *)prec);
  pdbRecordType->papFldDes[187]->size=sizeof(prec->r20lo);
  pdbRecordType->papFldDes[187]->offset=(short)((char *)&prec->r20lo - (char *)prec);
  pdbRecordType->papFldDes[188]->size=sizeof(prec->r20hi);
  pdbRecordType->papFldDes[188]->offset=(short)((char *)&prec->r20hi - (char *)prec);
  pdbRecordType->papFldDes[189]->size=sizeof(prec->r20bg);
  pdbRecordType->papFldDes[189]->offset=(short)((char *)&prec->r20bg - (char *)prec);
  pdbRecordType->papFldDes[190]->size=sizeof(prec->r20ip);
  pdbRecordType->papFldDes[190]->offset=(short)((char *)&prec->r20ip - (char *)prec);
  pdbRecordType->papFldDes[191]->size=sizeof(prec->r21lo);
  pdbRecordType->papFldDes[191]->offset=(short)((char *)&prec->r21lo - (char *)prec);
  pdbRecordType->papFldDes[192]->size=sizeof(prec->r21hi);
  pdbRecordType->papFldDes[192]->offset=(short)((char *)&prec->r21hi - (char *)prec);
  pdbRecordType->papFldDes[193]->size=sizeof(prec->r21bg);
  pdbRecordType->papFldDes[193]->offset=(short)((char *)&prec->r21bg - (char *)prec);
  pdbRecordType->papFldDes[194]->size=sizeof(prec->r21ip);
  pdbRecordType->papFldDes[194]->offset=(short)((char *)&prec->r21ip - (char *)prec);
  pdbRecordType->papFldDes[195]->size=sizeof(prec->r22lo);
  pdbRecordType->papFldDes[195]->offset=(short)((char *)&prec->r22lo - (char *)prec);
  pdbRecordType->papFldDes[196]->size=sizeof(prec->r22hi);
  pdbRecordType->papFldDes[196]->offset=(short)((char *)&prec->r22hi - (char *)prec);
  pdbRecordType->papFldDes[197]->size=sizeof(prec->r22bg);
  pdbRecordType->papFldDes[197]->offset=(short)((char *)&prec->r22bg - (char *)prec);
  pdbRecordType->papFldDes[198]->size=sizeof(prec->r22ip);
  pdbRecordType->papFldDes[198]->offset=(short)((char *)&prec->r22ip - (char *)prec);
  pdbRecordType->papFldDes[199]->size=sizeof(prec->r23lo);
  pdbRecordType->papFldDes[199]->offset=(short)((char *)&prec->r23lo - (char *)prec);
  pdbRecordType->papFldDes[200]->size=sizeof(prec->r23hi);
  pdbRecordType->papFldDes[200]->offset=(short)((char *)&prec->r23hi - (char *)prec);
  pdbRecordType->papFldDes[201]->size=sizeof(prec->r23bg);
  pdbRecordType->papFldDes[201]->offset=(short)((char *)&prec->r23bg - (char *)prec);
  pdbRecordType->papFldDes[202]->size=sizeof(prec->r23ip);
  pdbRecordType->papFldDes[202]->offset=(short)((char *)&prec->r23ip - (char *)prec);
  pdbRecordType->papFldDes[203]->size=sizeof(prec->r24lo);
  pdbRecordType->papFldDes[203]->offset=(short)((char *)&prec->r24lo - (char *)prec);
  pdbRecordType->papFldDes[204]->size=sizeof(prec->r24hi);
  pdbRecordType->papFldDes[204]->offset=(short)((char *)&prec->r24hi - (char *)prec);
  pdbRecordType->papFldDes[205]->size=sizeof(prec->r24bg);
  pdbRecordType->papFldDes[205]->offset=(short)((char *)&prec->r24bg - (char *)prec);
  pdbRecordType->papFldDes[206]->size=sizeof(prec->r24ip);
  pdbRecordType->papFldDes[206]->offset=(short)((char *)&prec->r24ip - (char *)prec);
  pdbRecordType->papFldDes[207]->size=sizeof(prec->r25lo);
  pdbRecordType->papFldDes[207]->offset=(short)((char *)&prec->r25lo - (char *)prec);
  pdbRecordType->papFldDes[208]->size=sizeof(prec->r25hi);
  pdbRecordType->papFldDes[208]->offset=(short)((char *)&prec->r25hi - (char *)prec);
  pdbRecordType->papFldDes[209]->size=sizeof(prec->r25bg);
  pdbRecordType->papFldDes[209]->offset=(short)((char *)&prec->r25bg - (char *)prec);
  pdbRecordType->papFldDes[210]->size=sizeof(prec->r25ip);
  pdbRecordType->papFldDes[210]->offset=(short)((char *)&prec->r25ip - (char *)prec);
  pdbRecordType->papFldDes[211]->size=sizeof(prec->r26lo);
  pdbRecordType->papFldDes[211]->offset=(short)((char *)&prec->r26lo - (char *)prec);
  pdbRecordType->papFldDes[212]->size=sizeof(prec->r26hi);
  pdbRecordType->papFldDes[212]->offset=(short)((char *)&prec->r26hi - (char *)prec);
  pdbRecordType->papFldDes[213]->size=sizeof(prec->r26bg);
  pdbRecordType->papFldDes[213]->offset=(short)((char *)&prec->r26bg - (char *)prec);
  pdbRecordType->papFldDes[214]->size=sizeof(prec->r26ip);
  pdbRecordType->papFldDes[214]->offset=(short)((char *)&prec->r26ip - (char *)prec);
  pdbRecordType->papFldDes[215]->size=sizeof(prec->r27lo);
  pdbRecordType->papFldDes[215]->offset=(short)((char *)&prec->r27lo - (char *)prec);
  pdbRecordType->papFldDes[216]->size=sizeof(prec->r27hi);
  pdbRecordType->papFldDes[216]->offset=(short)((char *)&prec->r27hi - (char *)prec);
  pdbRecordType->papFldDes[217]->size=sizeof(prec->r27bg);
  pdbRecordType->papFldDes[217]->offset=(short)((char *)&prec->r27bg - (char *)prec);
  pdbRecordType->papFldDes[218]->size=sizeof(prec->r27ip);
  pdbRecordType->papFldDes[218]->offset=(short)((char *)&prec->r27ip - (char *)prec);
  pdbRecordType->papFldDes[219]->size=sizeof(prec->r28lo);
  pdbRecordType->papFldDes[219]->offset=(short)((char *)&prec->r28lo - (char *)prec);
  pdbRecordType->papFldDes[220]->size=sizeof(prec->r28hi);
  pdbRecordType->papFldDes[220]->offset=(short)((char *)&prec->r28hi - (char *)prec);
  pdbRecordType->papFldDes[221]->size=sizeof(prec->r28bg);
  pdbRecordType->papFldDes[221]->offset=(short)((char *)&prec->r28bg - (char *)prec);
  pdbRecordType->papFldDes[222]->size=sizeof(prec->r28ip);
  pdbRecordType->papFldDes[222]->offset=(short)((char *)&prec->r28ip - (char *)prec);
  pdbRecordType->papFldDes[223]->size=sizeof(prec->r29lo);
  pdbRecordType->papFldDes[223]->offset=(short)((char *)&prec->r29lo - (char *)prec);
  pdbRecordType->papFldDes[224]->size=sizeof(prec->r29hi);
  pdbRecordType->papFldDes[224]->offset=(short)((char *)&prec->r29hi - (char *)prec);
  pdbRecordType->papFldDes[225]->size=sizeof(prec->r29bg);
  pdbRecordType->papFldDes[225]->offset=(short)((char *)&prec->r29bg - (char *)prec);
  pdbRecordType->papFldDes[226]->size=sizeof(prec->r29ip);
  pdbRecordType->papFldDes[226]->offset=(short)((char *)&prec->r29ip - (char *)prec);
  pdbRecordType->papFldDes[227]->size=sizeof(prec->r30lo);
  pdbRecordType->papFldDes[227]->offset=(short)((char *)&prec->r30lo - (char *)prec);
  pdbRecordType->papFldDes[228]->size=sizeof(prec->r30hi);
  pdbRecordType->papFldDes[228]->offset=(short)((char *)&prec->r30hi - (char *)prec);
  pdbRecordType->papFldDes[229]->size=sizeof(prec->r30bg);
  pdbRecordType->papFldDes[229]->offset=(short)((char *)&prec->r30bg - (char *)prec);
  pdbRecordType->papFldDes[230]->size=sizeof(prec->r30ip);
  pdbRecordType->papFldDes[230]->offset=(short)((char *)&prec->r30ip - (char *)prec);
  pdbRecordType->papFldDes[231]->size=sizeof(prec->r31lo);
  pdbRecordType->papFldDes[231]->offset=(short)((char *)&prec->r31lo - (char *)prec);
  pdbRecordType->papFldDes[232]->size=sizeof(prec->r31hi);
  pdbRecordType->papFldDes[232]->offset=(short)((char *)&prec->r31hi - (char *)prec);
  pdbRecordType->papFldDes[233]->size=sizeof(prec->r31bg);
  pdbRecordType->papFldDes[233]->offset=(short)((char *)&prec->r31bg - (char *)prec);
  pdbRecordType->papFldDes[234]->size=sizeof(prec->r31ip);
  pdbRecordType->papFldDes[234]->offset=(short)((char *)&prec->r31ip - (char *)prec);
  pdbRecordType->papFldDes[235]->size=sizeof(prec->r0);
  pdbRecordType->papFldDes[235]->offset=(short)((char *)&prec->r0 - (char *)prec);
  pdbRecordType->papFldDes[236]->size=sizeof(prec->r0n);
  pdbRecordType->papFldDes[236]->offset=(short)((char *)&prec->r0n - (char *)prec);
  pdbRecordType->papFldDes[237]->size=sizeof(prec->r0p);
  pdbRecordType->papFldDes[237]->offset=(short)((char *)&prec->r0p - (char *)prec);
  pdbRecordType->papFldDes[238]->size=sizeof(prec->r1);
  pdbRecordType->papFldDes[238]->offset=(short)((char *)&prec->r1 - (char *)prec);
  pdbRecordType->papFldDes[239]->size=sizeof(prec->r1n);
  pdbRecordType->papFldDes[239]->offset=(short)((char *)&prec->r1n - (char *)prec);
  pdbRecordType->papFldDes[240]->size=sizeof(prec->r1p);
  pdbRecordType->papFldDes[240]->offset=(short)((char *)&prec->r1p - (char *)prec);
  pdbRecordType->papFldDes[241]->size=sizeof(prec->r2);
  pdbRecordType->papFldDes[241]->offset=(short)((char *)&prec->r2 - (char *)prec);
  pdbRecordType->papFldDes[242]->size=sizeof(prec->r2n);
  pdbRecordType->papFldDes[242]->offset=(short)((char *)&prec->r2n - (char *)prec);
  pdbRecordType->papFldDes[243]->size=sizeof(prec->r2p);
  pdbRecordType->papFldDes[243]->offset=(short)((char *)&prec->r2p - (char *)prec);
  pdbRecordType->papFldDes[244]->size=sizeof(prec->r3);
  pdbRecordType->papFldDes[244]->offset=(short)((char *)&prec->r3 - (char *)prec);
  pdbRecordType->papFldDes[245]->size=sizeof(prec->r3n);
  pdbRecordType->papFldDes[245]->offset=(short)((char *)&prec->r3n - (char *)prec);
  pdbRecordType->papFldDes[246]->size=sizeof(prec->r3p);
  pdbRecordType->papFldDes[246]->offset=(short)((char *)&prec->r3p - (char *)prec);
  pdbRecordType->papFldDes[247]->size=sizeof(prec->r4);
  pdbRecordType->papFldDes[247]->offset=(short)((char *)&prec->r4 - (char *)prec);
  pdbRecordType->papFldDes[248]->size=sizeof(prec->r4n);
  pdbRecordType->papFldDes[248]->offset=(short)((char *)&prec->r4n - (char *)prec);
  pdbRecordType->papFldDes[249]->size=sizeof(prec->r4p);
  pdbRecordType->papFldDes[249]->offset=(short)((char *)&prec->r4p - (char *)prec);
  pdbRecordType->papFldDes[250]->size=sizeof(prec->r5);
  pdbRecordType->papFldDes[250]->offset=(short)((char *)&prec->r5 - (char *)prec);
  pdbRecordType->papFldDes[251]->size=sizeof(prec->r5n);
  pdbRecordType->papFldDes[251]->offset=(short)((char *)&prec->r5n - (char *)prec);
  pdbRecordType->papFldDes[252]->size=sizeof(prec->r5p);
  pdbRecordType->papFldDes[252]->offset=(short)((char *)&prec->r5p - (char *)prec);
  pdbRecordType->papFldDes[253]->size=sizeof(prec->r6);
  pdbRecordType->papFldDes[253]->offset=(short)((char *)&prec->r6 - (char *)prec);
  pdbRecordType->papFldDes[254]->size=sizeof(prec->r6n);
  pdbRecordType->papFldDes[254]->offset=(short)((char *)&prec->r6n - (char *)prec);
  pdbRecordType->papFldDes[255]->size=sizeof(prec->r6p);
  pdbRecordType->papFldDes[255]->offset=(short)((char *)&prec->r6p - (char *)prec);
  pdbRecordType->papFldDes[256]->size=sizeof(prec->r7);
  pdbRecordType->papFldDes[256]->offset=(short)((char *)&prec->r7 - (char *)prec);
  pdbRecordType->papFldDes[257]->size=sizeof(prec->r7n);
  pdbRecordType->papFldDes[257]->offset=(short)((char *)&prec->r7n - (char *)prec);
  pdbRecordType->papFldDes[258]->size=sizeof(prec->r7p);
  pdbRecordType->papFldDes[258]->offset=(short)((char *)&prec->r7p - (char *)prec);
  pdbRecordType->papFldDes[259]->size=sizeof(prec->r8);
  pdbRecordType->papFldDes[259]->offset=(short)((char *)&prec->r8 - (char *)prec);
  pdbRecordType->papFldDes[260]->size=sizeof(prec->r8n);
  pdbRecordType->papFldDes[260]->offset=(short)((char *)&prec->r8n - (char *)prec);
  pdbRecordType->papFldDes[261]->size=sizeof(prec->r8p);
  pdbRecordType->papFldDes[261]->offset=(short)((char *)&prec->r8p - (char *)prec);
  pdbRecordType->papFldDes[262]->size=sizeof(prec->r9);
  pdbRecordType->papFldDes[262]->offset=(short)((char *)&prec->r9 - (char *)prec);
  pdbRecordType->papFldDes[263]->size=sizeof(prec->r9n);
  pdbRecordType->papFldDes[263]->offset=(short)((char *)&prec->r9n - (char *)prec);
  pdbRecordType->papFldDes[264]->size=sizeof(prec->r9p);
  pdbRecordType->papFldDes[264]->offset=(short)((char *)&prec->r9p - (char *)prec);
  pdbRecordType->papFldDes[265]->size=sizeof(prec->r10);
  pdbRecordType->papFldDes[265]->offset=(short)((char *)&prec->r10 - (char *)prec);
  pdbRecordType->papFldDes[266]->size=sizeof(prec->r10n);
  pdbRecordType->papFldDes[266]->offset=(short)((char *)&prec->r10n - (char *)prec);
  pdbRecordType->papFldDes[267]->size=sizeof(prec->r10p);
  pdbRecordType->papFldDes[267]->offset=(short)((char *)&prec->r10p - (char *)prec);
  pdbRecordType->papFldDes[268]->size=sizeof(prec->r11);
  pdbRecordType->papFldDes[268]->offset=(short)((char *)&prec->r11 - (char *)prec);
  pdbRecordType->papFldDes[269]->size=sizeof(prec->r11n);
  pdbRecordType->papFldDes[269]->offset=(short)((char *)&prec->r11n - (char *)prec);
  pdbRecordType->papFldDes[270]->size=sizeof(prec->r11p);
  pdbRecordType->papFldDes[270]->offset=(short)((char *)&prec->r11p - (char *)prec);
  pdbRecordType->papFldDes[271]->size=sizeof(prec->r12);
  pdbRecordType->papFldDes[271]->offset=(short)((char *)&prec->r12 - (char *)prec);
  pdbRecordType->papFldDes[272]->size=sizeof(prec->r12n);
  pdbRecordType->papFldDes[272]->offset=(short)((char *)&prec->r12n - (char *)prec);
  pdbRecordType->papFldDes[273]->size=sizeof(prec->r12p);
  pdbRecordType->papFldDes[273]->offset=(short)((char *)&prec->r12p - (char *)prec);
  pdbRecordType->papFldDes[274]->size=sizeof(prec->r13);
  pdbRecordType->papFldDes[274]->offset=(short)((char *)&prec->r13 - (char *)prec);
  pdbRecordType->papFldDes[275]->size=sizeof(prec->r13n);
  pdbRecordType->papFldDes[275]->offset=(short)((char *)&prec->r13n - (char *)prec);
  pdbRecordType->papFldDes[276]->size=sizeof(prec->r13p);
  pdbRecordType->papFldDes[276]->offset=(short)((char *)&prec->r13p - (char *)prec);
  pdbRecordType->papFldDes[277]->size=sizeof(prec->r14);
  pdbRecordType->papFldDes[277]->offset=(short)((char *)&prec->r14 - (char *)prec);
  pdbRecordType->papFldDes[278]->size=sizeof(prec->r14n);
  pdbRecordType->papFldDes[278]->offset=(short)((char *)&prec->r14n - (char *)prec);
  pdbRecordType->papFldDes[279]->size=sizeof(prec->r14p);
  pdbRecordType->papFldDes[279]->offset=(short)((char *)&prec->r14p - (char *)prec);
  pdbRecordType->papFldDes[280]->size=sizeof(prec->r15);
  pdbRecordType->papFldDes[280]->offset=(short)((char *)&prec->r15 - (char *)prec);
  pdbRecordType->papFldDes[281]->size=sizeof(prec->r15n);
  pdbRecordType->papFldDes[281]->offset=(short)((char *)&prec->r15n - (char *)prec);
  pdbRecordType->papFldDes[282]->size=sizeof(prec->r15p);
  pdbRecordType->papFldDes[282]->offset=(short)((char *)&prec->r15p - (char *)prec);
  pdbRecordType->papFldDes[283]->size=sizeof(prec->r16);
  pdbRecordType->papFldDes[283]->offset=(short)((char *)&prec->r16 - (char *)prec);
  pdbRecordType->papFldDes[284]->size=sizeof(prec->r16n);
  pdbRecordType->papFldDes[284]->offset=(short)((char *)&prec->r16n - (char *)prec);
  pdbRecordType->papFldDes[285]->size=sizeof(prec->r16p);
  pdbRecordType->papFldDes[285]->offset=(short)((char *)&prec->r16p - (char *)prec);
  pdbRecordType->papFldDes[286]->size=sizeof(prec->r17);
  pdbRecordType->papFldDes[286]->offset=(short)((char *)&prec->r17 - (char *)prec);
  pdbRecordType->papFldDes[287]->size=sizeof(prec->r17n);
  pdbRecordType->papFldDes[287]->offset=(short)((char *)&prec->r17n - (char *)prec);
  pdbRecordType->papFldDes[288]->size=sizeof(prec->r17p);
  pdbRecordType->papFldDes[288]->offset=(short)((char *)&prec->r17p - (char *)prec);
  pdbRecordType->papFldDes[289]->size=sizeof(prec->r18);
  pdbRecordType->papFldDes[289]->offset=(short)((char *)&prec->r18 - (char *)prec);
  pdbRecordType->papFldDes[290]->size=sizeof(prec->r18n);
  pdbRecordType->papFldDes[290]->offset=(short)((char *)&prec->r18n - (char *)prec);
  pdbRecordType->papFldDes[291]->size=sizeof(prec->r18p);
  pdbRecordType->papFldDes[291]->offset=(short)((char *)&prec->r18p - (char *)prec);
  pdbRecordType->papFldDes[292]->size=sizeof(prec->r19);
  pdbRecordType->papFldDes[292]->offset=(short)((char *)&prec->r19 - (char *)prec);
  pdbRecordType->papFldDes[293]->size=sizeof(prec->r19n);
  pdbRecordType->papFldDes[293]->offset=(short)((char *)&prec->r19n - (char *)prec);
  pdbRecordType->papFldDes[294]->size=sizeof(prec->r19p);
  pdbRecordType->papFldDes[294]->offset=(short)((char *)&prec->r19p - (char *)prec);
  pdbRecordType->papFldDes[295]->size=sizeof(prec->r20);
  pdbRecordType->papFldDes[295]->offset=(short)((char *)&prec->r20 - (char *)prec);
  pdbRecordType->papFldDes[296]->size=sizeof(prec->r20n);
  pdbRecordType->papFldDes[296]->offset=(short)((char *)&prec->r20n - (char *)prec);
  pdbRecordType->papFldDes[297]->size=sizeof(prec->r20p);
  pdbRecordType->papFldDes[297]->offset=(short)((char *)&prec->r20p - (char *)prec);
  pdbRecordType->papFldDes[298]->size=sizeof(prec->r21);
  pdbRecordType->papFldDes[298]->offset=(short)((char *)&prec->r21 - (char *)prec);
  pdbRecordType->papFldDes[299]->size=sizeof(prec->r21n);
  pdbRecordType->papFldDes[299]->offset=(short)((char *)&prec->r21n - (char *)prec);
  pdbRecordType->papFldDes[300]->size=sizeof(prec->r21p);
  pdbRecordType->papFldDes[300]->offset=(short)((char *)&prec->r21p - (char *)prec);
  pdbRecordType->papFldDes[301]->size=sizeof(prec->r22);
  pdbRecordType->papFldDes[301]->offset=(short)((char *)&prec->r22 - (char *)prec);
  pdbRecordType->papFldDes[302]->size=sizeof(prec->r22n);
  pdbRecordType->papFldDes[302]->offset=(short)((char *)&prec->r22n - (char *)prec);
  pdbRecordType->papFldDes[303]->size=sizeof(prec->r22p);
  pdbRecordType->papFldDes[303]->offset=(short)((char *)&prec->r22p - (char *)prec);
  pdbRecordType->papFldDes[304]->size=sizeof(prec->r23);
  pdbRecordType->papFldDes[304]->offset=(short)((char *)&prec->r23 - (char *)prec);
  pdbRecordType->papFldDes[305]->size=sizeof(prec->r23n);
  pdbRecordType->papFldDes[305]->offset=(short)((char *)&prec->r23n - (char *)prec);
  pdbRecordType->papFldDes[306]->size=sizeof(prec->r23p);
  pdbRecordType->papFldDes[306]->offset=(short)((char *)&prec->r23p - (char *)prec);
  pdbRecordType->papFldDes[307]->size=sizeof(prec->r24);
  pdbRecordType->papFldDes[307]->offset=(short)((char *)&prec->r24 - (char *)prec);
  pdbRecordType->papFldDes[308]->size=sizeof(prec->r24n);
  pdbRecordType->papFldDes[308]->offset=(short)((char *)&prec->r24n - (char *)prec);
  pdbRecordType->papFldDes[309]->size=sizeof(prec->r24p);
  pdbRecordType->papFldDes[309]->offset=(short)((char *)&prec->r24p - (char *)prec);
  pdbRecordType->papFldDes[310]->size=sizeof(prec->r25);
  pdbRecordType->papFldDes[310]->offset=(short)((char *)&prec->r25 - (char *)prec);
  pdbRecordType->papFldDes[311]->size=sizeof(prec->r25n);
  pdbRecordType->papFldDes[311]->offset=(short)((char *)&prec->r25n - (char *)prec);
  pdbRecordType->papFldDes[312]->size=sizeof(prec->r25p);
  pdbRecordType->papFldDes[312]->offset=(short)((char *)&prec->r25p - (char *)prec);
  pdbRecordType->papFldDes[313]->size=sizeof(prec->r26);
  pdbRecordType->papFldDes[313]->offset=(short)((char *)&prec->r26 - (char *)prec);
  pdbRecordType->papFldDes[314]->size=sizeof(prec->r26n);
  pdbRecordType->papFldDes[314]->offset=(short)((char *)&prec->r26n - (char *)prec);
  pdbRecordType->papFldDes[315]->size=sizeof(prec->r26p);
  pdbRecordType->papFldDes[315]->offset=(short)((char *)&prec->r26p - (char *)prec);
  pdbRecordType->papFldDes[316]->size=sizeof(prec->r27);
  pdbRecordType->papFldDes[316]->offset=(short)((char *)&prec->r27 - (char *)prec);
  pdbRecordType->papFldDes[317]->size=sizeof(prec->r27n);
  pdbRecordType->papFldDes[317]->offset=(short)((char *)&prec->r27n - (char *)prec);
  pdbRecordType->papFldDes[318]->size=sizeof(prec->r27p);
  pdbRecordType->papFldDes[318]->offset=(short)((char *)&prec->r27p - (char *)prec);
  pdbRecordType->papFldDes[319]->size=sizeof(prec->r28);
  pdbRecordType->papFldDes[319]->offset=(short)((char *)&prec->r28 - (char *)prec);
  pdbRecordType->papFldDes[320]->size=sizeof(prec->r28n);
  pdbRecordType->papFldDes[320]->offset=(short)((char *)&prec->r28n - (char *)prec);
  pdbRecordType->papFldDes[321]->size=sizeof(prec->r28p);
  pdbRecordType->papFldDes[321]->offset=(short)((char *)&prec->r28p - (char *)prec);
  pdbRecordType->papFldDes[322]->size=sizeof(prec->r29);
  pdbRecordType->papFldDes[322]->offset=(short)((char *)&prec->r29 - (char *)prec);
  pdbRecordType->papFldDes[323]->size=sizeof(prec->r29n);
  pdbRecordType->papFldDes[323]->offset=(short)((char *)&prec->r29n - (char *)prec);
  pdbRecordType->papFldDes[324]->size=sizeof(prec->r29p);
  pdbRecordType->papFldDes[324]->offset=(short)((char *)&prec->r29p - (char *)prec);
  pdbRecordType->papFldDes[325]->size=sizeof(prec->r30);
  pdbRecordType->papFldDes[325]->offset=(short)((char *)&prec->r30 - (char *)prec);
  pdbRecordType->papFldDes[326]->size=sizeof(prec->r30n);
  pdbRecordType->papFldDes[326]->offset=(short)((char *)&prec->r30n - (char *)prec);
  pdbRecordType->papFldDes[327]->size=sizeof(prec->r30p);
  pdbRecordType->papFldDes[327]->offset=(short)((char *)&prec->r30p - (char *)prec);
  pdbRecordType->papFldDes[328]->size=sizeof(prec->r31);
  pdbRecordType->papFldDes[328]->offset=(short)((char *)&prec->r31 - (char *)prec);
  pdbRecordType->papFldDes[329]->size=sizeof(prec->r31n);
  pdbRecordType->papFldDes[329]->offset=(short)((char *)&prec->r31n - (char *)prec);
  pdbRecordType->papFldDes[330]->size=sizeof(prec->r31p);
  pdbRecordType->papFldDes[330]->offset=(short)((char *)&prec->r31p - (char *)prec);
  pdbRecordType->papFldDes[331]->size=sizeof(prec->r0nm);
  pdbRecordType->papFldDes[331]->offset=(short)((char *)&prec->r0nm - (char *)prec);
  pdbRecordType->papFldDes[332]->size=sizeof(prec->r1nm);
  pdbRecordType->papFldDes[332]->offset=(short)((char *)&prec->r1nm - (char *)prec);
  pdbRecordType->papFldDes[333]->size=sizeof(prec->r2nm);
  pdbRecordType->papFldDes[333]->offset=(short)((char *)&prec->r2nm - (char *)prec);
  pdbRecordType->papFldDes[334]->size=sizeof(prec->r3nm);
  pdbRecordType->papFldDes[334]->offset=(short)((char *)&prec->r3nm - (char *)prec);
  pdbRecordType->papFldDes[335]->size=sizeof(prec->r4nm);
  pdbRecordType->papFldDes[335]->offset=(short)((char *)&prec->r4nm - (char *)prec);
  pdbRecordType->papFldDes[336]->size=sizeof(prec->r5nm);
  pdbRecordType->papFldDes[336]->offset=(short)((char *)&prec->r5nm - (char *)prec);
  pdbRecordType->papFldDes[337]->size=sizeof(prec->r6nm);
  pdbRecordType->papFldDes[337]->offset=(short)((char *)&prec->r6nm - (char *)prec);
  pdbRecordType->papFldDes[338]->size=sizeof(prec->r7nm);
  pdbRecordType->papFldDes[338]->offset=(short)((char *)&prec->r7nm - (char *)prec);
  pdbRecordType->papFldDes[339]->size=sizeof(prec->r8nm);
  pdbRecordType->papFldDes[339]->offset=(short)((char *)&prec->r8nm - (char *)prec);
  pdbRecordType->papFldDes[340]->size=sizeof(prec->r9nm);
  pdbRecordType->papFldDes[340]->offset=(short)((char *)&prec->r9nm - (char *)prec);
  pdbRecordType->papFldDes[341]->size=sizeof(prec->r10nm);
  pdbRecordType->papFldDes[341]->offset=(short)((char *)&prec->r10nm - (char *)prec);
  pdbRecordType->papFldDes[342]->size=sizeof(prec->r11nm);
  pdbRecordType->papFldDes[342]->offset=(short)((char *)&prec->r11nm - (char *)prec);
  pdbRecordType->papFldDes[343]->size=sizeof(prec->r12nm);
  pdbRecordType->papFldDes[343]->offset=(short)((char *)&prec->r12nm - (char *)prec);
  pdbRecordType->papFldDes[344]->size=sizeof(prec->r13nm);
  pdbRecordType->papFldDes[344]->offset=(short)((char *)&prec->r13nm - (char *)prec);
  pdbRecordType->papFldDes[345]->size=sizeof(prec->r14nm);
  pdbRecordType->papFldDes[345]->offset=(short)((char *)&prec->r14nm - (char *)prec);
  pdbRecordType->papFldDes[346]->size=sizeof(prec->r15nm);
  pdbRecordType->papFldDes[346]->offset=(short)((char *)&prec->r15nm - (char *)prec);
  pdbRecordType->papFldDes[347]->size=sizeof(prec->r16nm);
  pdbRecordType->papFldDes[347]->offset=(short)((char *)&prec->r16nm - (char *)prec);
  pdbRecordType->papFldDes[348]->size=sizeof(prec->r17nm);
  pdbRecordType->papFldDes[348]->offset=(short)((char *)&prec->r17nm - (char *)prec);
  pdbRecordType->papFldDes[349]->size=sizeof(prec->r18nm);
  pdbRecordType->papFldDes[349]->offset=(short)((char *)&prec->r18nm - (char *)prec);
  pdbRecordType->papFldDes[350]->size=sizeof(prec->r19nm);
  pdbRecordType->papFldDes[350]->offset=(short)((char *)&prec->r19nm - (char *)prec);
  pdbRecordType->papFldDes[351]->size=sizeof(prec->r20nm);
  pdbRecordType->papFldDes[351]->offset=(short)((char *)&prec->r20nm - (char *)prec);
  pdbRecordType->papFldDes[352]->size=sizeof(prec->r21nm);
  pdbRecordType->papFldDes[352]->offset=(short)((char *)&prec->r21nm - (char *)prec);
  pdbRecordType->papFldDes[353]->size=sizeof(prec->r22nm);
  pdbRecordType->papFldDes[353]->offset=(short)((char *)&prec->r22nm - (char *)prec);
  pdbRecordType->papFldDes[354]->size=sizeof(prec->r23nm);
  pdbRecordType->papFldDes[354]->offset=(short)((char *)&prec->r23nm - (char *)prec);
  pdbRecordType->papFldDes[355]->size=sizeof(prec->r24nm);
  pdbRecordType->papFldDes[355]->offset=(short)((char *)&prec->r24nm - (char *)prec);
  pdbRecordType->papFldDes[356]->size=sizeof(prec->r25nm);
  pdbRecordType->papFldDes[356]->offset=(short)((char *)&prec->r25nm - (char *)prec);
  pdbRecordType->papFldDes[357]->size=sizeof(prec->r26nm);
  pdbRecordType->papFldDes[357]->offset=(short)((char *)&prec->r26nm - (char *)prec);
  pdbRecordType->papFldDes[358]->size=sizeof(prec->r27nm);
  pdbRecordType->papFldDes[358]->offset=(short)((char *)&prec->r27nm - (char *)prec);
  pdbRecordType->papFldDes[359]->size=sizeof(prec->r28nm);
  pdbRecordType->papFldDes[359]->offset=(short)((char *)&prec->r28nm - (char *)prec);
  pdbRecordType->papFldDes[360]->size=sizeof(prec->r29nm);
  pdbRecordType->papFldDes[360]->offset=(short)((char *)&prec->r29nm - (char *)prec);
  pdbRecordType->papFldDes[361]->size=sizeof(prec->r30nm);
  pdbRecordType->papFldDes[361]->offset=(short)((char *)&prec->r30nm - (char *)prec);
  pdbRecordType->papFldDes[362]->size=sizeof(prec->r31nm);
  pdbRecordType->papFldDes[362]->offset=(short)((char *)&prec->r31nm - (char *)prec);
    pdbRecordType->rec_size = sizeof(*prec);
    return(0);
}
epicsExportRegistrar(mcaRecordSizeOffset);
#ifdef __cplusplus
}
#endif
#endif /*GEN_SIZE_OFFSET*/
