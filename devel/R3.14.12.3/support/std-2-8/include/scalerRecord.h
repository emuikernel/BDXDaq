
#ifndef INCscalerG1H
#define INCscalerG1H
typedef enum {
	scalerG1_N,
	scalerG1_Y
}scalerG1;
#endif /*INCscalerG1H*/

#ifndef INCscalerD1H
#define INCscalerD1H
typedef enum {
	scalerD1_Up,
	scalerD1_Dn
}scalerD1;
#endif /*INCscalerD1H*/

#ifndef INCscalerCONTH
#define INCscalerCONTH
typedef enum {
	scalerCONT_OneShot,
	scalerCONT_AutoCount
}scalerCONT;
#endif /*INCscalerCONTH*/

#ifndef INCscalerCNTH
#define INCscalerCNTH
typedef enum {
	scalerCNT_Done,
	scalerCNT_Count
}scalerCNT;
#endif /*INCscalerCNTH*/
#ifndef INCscalerH
#define INCscalerH
#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"
typedef struct scalerRecord {
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
	epicsFloat32	vers;	/* Code Version */
	void *rpvt;	/* Record Private */
	epicsFloat64	val;	/* Result */
	epicsFloat64	freq;	/* Time base freq */
	epicsEnum16	cnt;	/* Count */
	epicsEnum16	pcnt;	/* Prev Count */
	epicsInt16	ss;	/* Scaler state */
	epicsInt16	us;	/* User state */
	epicsEnum16	cont;	/* OneShot/AutoCount */
	DBLINK		out;	/* Output Specification */
	epicsFloat32	rate;	/* Display Rate (Hz.) */
	epicsFloat32	rat1;	/* Auto Display Rate (Hz.) */
	epicsFloat32	dly;	/* Delay */
	epicsFloat32	dly1;	/* Auto-mode Delay */
	epicsInt16	nch;	/* Number of Channels */
	epicsEnum16	d1;	/* Count Direction 1 */
	epicsEnum16	d2;	/* Count Direction 2 */
	epicsEnum16	d3;	/* Count Direction 3 */
	epicsEnum16	d4;	/* Count Direction 4 */
	epicsEnum16	d5;	/* Count Direction 5 */
	epicsEnum16	d6;	/* Count Direction 6 */
	epicsEnum16	d7;	/* Count Direction 7 */
	epicsEnum16	d8;	/* Count Direction 8 */
	epicsEnum16	d9;	/* Count Direction 9 */
	epicsEnum16	d10;	/* Count Direction 10 */
	epicsEnum16	d11;	/* Count Direction 11 */
	epicsEnum16	d12;	/* Count Direction 12 */
	epicsEnum16	d13;	/* Count Direction 13 */
	epicsEnum16	d14;	/* Count Direction 14 */
	epicsEnum16	d15;	/* Count Direction 15 */
	epicsEnum16	d16;	/* Count Direction 16 */
	epicsEnum16	d17;	/* Count Direction 17 */
	epicsEnum16	d18;	/* Count Direction 18 */
	epicsEnum16	d19;	/* Count Direction 19 */
	epicsEnum16	d20;	/* Count Direction 20 */
	epicsEnum16	d21;	/* Count Direction 21 */
	epicsEnum16	d22;	/* Count Direction 22 */
	epicsEnum16	d23;	/* Count Direction 23 */
	epicsEnum16	d24;	/* Count Direction 24 */
	epicsEnum16	d25;	/* Count Direction 25 */
	epicsEnum16	d26;	/* Count Direction 26 */
	epicsEnum16	d27;	/* Count Direction 27 */
	epicsEnum16	d28;	/* Count Direction 28 */
	epicsEnum16	d29;	/* Count Direction 29 */
	epicsEnum16	d30;	/* Count Direction 30 */
	epicsEnum16	d31;	/* Count Direction 31 */
	epicsEnum16	d32;	/* Count Direction 32 */
	epicsEnum16	d33;	/* Count Direction 33 */
	epicsEnum16	d34;	/* Count Direction 34 */
	epicsEnum16	d35;	/* Count Direction 35 */
	epicsEnum16	d36;	/* Count Direction 36 */
	epicsEnum16	d37;	/* Count Direction 37 */
	epicsEnum16	d38;	/* Count Direction 38 */
	epicsEnum16	d39;	/* Count Direction 39 */
	epicsEnum16	d40;	/* Count Direction 40 */
	epicsEnum16	d41;	/* Count Direction 41 */
	epicsEnum16	d42;	/* Count Direction 42 */
	epicsEnum16	d43;	/* Count Direction 43 */
	epicsEnum16	d44;	/* Count Direction 44 */
	epicsEnum16	d45;	/* Count Direction 45 */
	epicsEnum16	d46;	/* Count Direction 46 */
	epicsEnum16	d47;	/* Count Direction 47 */
	epicsEnum16	d48;	/* Count Direction 48 */
	epicsEnum16	d49;	/* Count Direction 49 */
	epicsEnum16	d50;	/* Count Direction 50 */
	epicsEnum16	d51;	/* Count Direction 51 */
	epicsEnum16	d52;	/* Count Direction 52 */
	epicsEnum16	d53;	/* Count Direction 53 */
	epicsEnum16	d54;	/* Count Direction 54 */
	epicsEnum16	d55;	/* Count Direction 55 */
	epicsEnum16	d56;	/* Count Direction 56 */
	epicsEnum16	d57;	/* Count Direction 57 */
	epicsEnum16	d58;	/* Count Direction 58 */
	epicsEnum16	d59;	/* Count Direction 59 */
	epicsEnum16	d60;	/* Count Direction 60 */
	epicsEnum16	d61;	/* Count Direction 61 */
	epicsEnum16	d62;	/* Count Direction 62 */
	epicsEnum16	d63;	/* Count Direction 63 */
	epicsEnum16	d64;	/* Count Direction 64 */
	epicsEnum16	g1;	/* Gate Control 1 */
	epicsEnum16	g2;	/* Gate Control 2 */
	epicsEnum16	g3;	/* Gate Control 3 */
	epicsEnum16	g4;	/* Gate Control 4 */
	epicsEnum16	g5;	/* Gate Control 5 */
	epicsEnum16	g6;	/* Gate Control 6 */
	epicsEnum16	g7;	/* Gate Control 7 */
	epicsEnum16	g8;	/* Gate Control 8 */
	epicsEnum16	g9;	/* Gate Control 9 */
	epicsEnum16	g10;	/* Gate Control 10 */
	epicsEnum16	g11;	/* Gate Control 11 */
	epicsEnum16	g12;	/* Gate Control 12 */
	epicsEnum16	g13;	/* Gate Control 13 */
	epicsEnum16	g14;	/* Gate Control 14 */
	epicsEnum16	g15;	/* Gate Control 15 */
	epicsEnum16	g16;	/* Gate Control 16 */
	epicsEnum16	g17;	/* Gate Control 17 */
	epicsEnum16	g18;	/* Gate Control 18 */
	epicsEnum16	g19;	/* Gate Control 19 */
	epicsEnum16	g20;	/* Gate Control 20 */
	epicsEnum16	g21;	/* Gate Control 21 */
	epicsEnum16	g22;	/* Gate Control 22 */
	epicsEnum16	g23;	/* Gate Control 23 */
	epicsEnum16	g24;	/* Gate Control 24 */
	epicsEnum16	g25;	/* Gate Control 25 */
	epicsEnum16	g26;	/* Gate Control 26 */
	epicsEnum16	g27;	/* Gate Control 27 */
	epicsEnum16	g28;	/* Gate Control 28 */
	epicsEnum16	g29;	/* Gate Control 29 */
	epicsEnum16	g30;	/* Gate Control 30 */
	epicsEnum16	g31;	/* Gate Control 31 */
	epicsEnum16	g32;	/* Gate Control 32 */
	epicsEnum16	g33;	/* Gate Control */
	epicsEnum16	g34;	/* Gate Control */
	epicsEnum16	g35;	/* Gate Control */
	epicsEnum16	g36;	/* Gate Control */
	epicsEnum16	g37;	/* Gate Control */
	epicsEnum16	g38;	/* Gate Control */
	epicsEnum16	g39;	/* Gate Control */
	epicsEnum16	g40;	/* Gate Control */
	epicsEnum16	g41;	/* Gate Control */
	epicsEnum16	g42;	/* Gate Control */
	epicsEnum16	g43;	/* Gate Control */
	epicsEnum16	g44;	/* Gate Control */
	epicsEnum16	g45;	/* Gate Control */
	epicsEnum16	g46;	/* Gate Control */
	epicsEnum16	g47;	/* Gate Control */
	epicsEnum16	g48;	/* Gate Control */
	epicsEnum16	g49;	/* Gate Control */
	epicsEnum16	g50;	/* Gate Control */
	epicsEnum16	g51;	/* Gate Control */
	epicsEnum16	g52;	/* Gate Control */
	epicsEnum16	g53;	/* Gate Control */
	epicsEnum16	g54;	/* Gate Control */
	epicsEnum16	g55;	/* Gate Control */
	epicsEnum16	g56;	/* Gate Control */
	epicsEnum16	g57;	/* Gate Control */
	epicsEnum16	g58;	/* Gate Control */
	epicsEnum16	g59;	/* Gate Control */
	epicsEnum16	g60;	/* Gate Control */
	epicsEnum16	g61;	/* Gate Control */
	epicsEnum16	g62;	/* Gate Control */
	epicsEnum16	g63;	/* Gate Control */
	epicsEnum16	g64;	/* Gate Control */
	epicsFloat64	tp;	/* Time Preset */
	epicsFloat64	tp1;	/* Time Preset */
	epicsUInt32	pr1;	/* Preset 1 */
	epicsUInt32	pr2;	/* Preset 2 */
	epicsUInt32	pr3;	/* Preset 3 */
	epicsUInt32	pr4;	/* Preset 4 */
	epicsUInt32	pr5;	/* Preset 5 */
	epicsUInt32	pr6;	/* Preset 6 */
	epicsUInt32	pr7;	/* Preset 7 */
	epicsUInt32	pr8;	/* Preset 8 */
	epicsUInt32	pr9;	/* Preset 9 */
	epicsUInt32	pr10;	/* Preset 10 */
	epicsUInt32	pr11;	/* Preset 11 */
	epicsUInt32	pr12;	/* Preset 12 */
	epicsUInt32	pr13;	/* Preset 13 */
	epicsUInt32	pr14;	/* Preset 14 */
	epicsUInt32	pr15;	/* Preset 15 */
	epicsUInt32	pr16;	/* Preset 16 */
	epicsUInt32	pr17;	/* Preset 17 */
	epicsUInt32	pr18;	/* Preset 18 */
	epicsUInt32	pr19;	/* Preset 19 */
	epicsUInt32	pr20;	/* Preset 20 */
	epicsUInt32	pr21;	/* Preset 21 */
	epicsUInt32	pr22;	/* Preset 22 */
	epicsUInt32	pr23;	/* Preset 23 */
	epicsUInt32	pr24;	/* Preset 24 */
	epicsUInt32	pr25;	/* Preset 25 */
	epicsUInt32	pr26;	/* Preset 26 */
	epicsUInt32	pr27;	/* Preset 27 */
	epicsUInt32	pr28;	/* Preset 28 */
	epicsUInt32	pr29;	/* Preset 29 */
	epicsUInt32	pr30;	/* Preset 30 */
	epicsUInt32	pr31;	/* Preset 31 */
	epicsUInt32	pr32;	/* Preset 32 */
	epicsUInt32	pr33;	/* Preset */
	epicsUInt32	pr34;	/* Preset */
	epicsUInt32	pr35;	/* Preset */
	epicsUInt32	pr36;	/* Preset */
	epicsUInt32	pr37;	/* Preset */
	epicsUInt32	pr38;	/* Preset */
	epicsUInt32	pr39;	/* Preset */
	epicsUInt32	pr40;	/* Preset */
	epicsUInt32	pr41;	/* Preset */
	epicsUInt32	pr42;	/* Preset */
	epicsUInt32	pr43;	/* Preset */
	epicsUInt32	pr44;	/* Preset */
	epicsUInt32	pr45;	/* Preset */
	epicsUInt32	pr46;	/* Preset */
	epicsUInt32	pr47;	/* Preset */
	epicsUInt32	pr48;	/* Preset */
	epicsUInt32	pr49;	/* Preset */
	epicsUInt32	pr50;	/* Preset */
	epicsUInt32	pr51;	/* Preset */
	epicsUInt32	pr52;	/* Preset */
	epicsUInt32	pr53;	/* Preset */
	epicsUInt32	pr54;	/* Preset */
	epicsUInt32	pr55;	/* Preset */
	epicsUInt32	pr56;	/* Preset */
	epicsUInt32	pr57;	/* Preset */
	epicsUInt32	pr58;	/* Preset */
	epicsUInt32	pr59;	/* Preset */
	epicsUInt32	pr60;	/* Preset */
	epicsUInt32	pr61;	/* Preset */
	epicsUInt32	pr62;	/* Preset */
	epicsUInt32	pr63;	/* Preset */
	epicsUInt32	pr64;	/* Preset */
	epicsFloat64	t;	/* Timer */
	epicsUInt32	s1;	/* Count */
	epicsUInt32	s2;	/* Count */
	epicsUInt32	s3;	/* Count */
	epicsUInt32	s4;	/* Count */
	epicsUInt32	s5;	/* Count */
	epicsUInt32	s6;	/* Count */
	epicsUInt32	s7;	/* Count */
	epicsUInt32	s8;	/* Count */
	epicsUInt32	s9;	/* Count */
	epicsUInt32	s10;	/* Count */
	epicsUInt32	s11;	/* Count */
	epicsUInt32	s12;	/* Count */
	epicsUInt32	s13;	/* Count */
	epicsUInt32	s14;	/* Count */
	epicsUInt32	s15;	/* Count */
	epicsUInt32	s16;	/* Count */
	epicsUInt32	s17;	/* Count */
	epicsUInt32	s18;	/* Count */
	epicsUInt32	s19;	/* Count */
	epicsUInt32	s20;	/* Count */
	epicsUInt32	s21;	/* Count */
	epicsUInt32	s22;	/* Count */
	epicsUInt32	s23;	/* Count */
	epicsUInt32	s24;	/* Count */
	epicsUInt32	s25;	/* Count */
	epicsUInt32	s26;	/* Count */
	epicsUInt32	s27;	/* Count */
	epicsUInt32	s28;	/* Count */
	epicsUInt32	s29;	/* Count */
	epicsUInt32	s30;	/* Count */
	epicsUInt32	s31;	/* Count */
	epicsUInt32	s32;	/* Count */
	epicsUInt32	s33;	/* Count */
	epicsUInt32	s34;	/* Count */
	epicsUInt32	s35;	/* Count */
	epicsUInt32	s36;	/* Count */
	epicsUInt32	s37;	/* Count */
	epicsUInt32	s38;	/* Count */
	epicsUInt32	s39;	/* Count */
	epicsUInt32	s40;	/* Count */
	epicsUInt32	s41;	/* Count */
	epicsUInt32	s42;	/* Count */
	epicsUInt32	s43;	/* Count */
	epicsUInt32	s44;	/* Count */
	epicsUInt32	s45;	/* Count */
	epicsUInt32	s46;	/* Count */
	epicsUInt32	s47;	/* Count */
	epicsUInt32	s48;	/* Count */
	epicsUInt32	s49;	/* Count */
	epicsUInt32	s50;	/* Count */
	epicsUInt32	s51;	/* Count */
	epicsUInt32	s52;	/* Count */
	epicsUInt32	s53;	/* Count */
	epicsUInt32	s54;	/* Count */
	epicsUInt32	s55;	/* Count */
	epicsUInt32	s56;	/* Count */
	epicsUInt32	s57;	/* Count */
	epicsUInt32	s58;	/* Count */
	epicsUInt32	s59;	/* Count */
	epicsUInt32	s60;	/* Count */
	epicsUInt32	s61;	/* Count */
	epicsUInt32	s62;	/* Count */
	epicsUInt32	s63;	/* Count */
	epicsUInt32	s64;	/* Count */
	char		nm1[16];	/* Scaler 1 name */
	char		nm2[16];	/* Scaler 2 name */
	char		nm3[16];	/* Scaler 3 name */
	char		nm4[16];	/* Scaler 4 name */
	char		nm5[16];	/* Scaler 5 name */
	char		nm6[16];	/* Scaler 6 name */
	char		nm7[16];	/* Scaler 7 name */
	char		nm8[16];	/* Scaler 8 name */
	char		nm9[16];	/* Scaler 9 name */
	char		nm10[16];	/* Scaler 10 name */
	char		nm11[16];	/* Scaler 11 name */
	char		nm12[16];	/* Scaler 12 name */
	char		nm13[16];	/* Scaler 13 name */
	char		nm14[16];	/* Scaler 14 name */
	char		nm15[16];	/* Scaler 15 name */
	char		nm16[16];	/* Scaler 16 name */
	char		nm17[16];	/* Scaler 17 name */
	char		nm18[16];	/* Scaler 18 name */
	char		nm19[16];	/* Scaler 19 name */
	char		nm20[16];	/* Scaler 20 name */
	char		nm21[16];	/* Scaler 21 name */
	char		nm22[16];	/* Scaler 22 name */
	char		nm23[16];	/* Scaler 23 name */
	char		nm24[16];	/* Scaler 24 name */
	char		nm25[16];	/* Scaler 25 name */
	char		nm26[16];	/* Scaler 26 name */
	char		nm27[16];	/* Scaler 27 name */
	char		nm28[16];	/* Scaler 28 name */
	char		nm29[16];	/* Scaler 29 name */
	char		nm30[16];	/* Scaler 30 name */
	char		nm31[16];	/* Scaler 31 name */
	char		nm32[16];	/* Scaler 32 name */
	char		nm33[16];	/* Scaler name */
	char		nm34[16];	/* Scaler name */
	char		nm35[16];	/* Scaler name */
	char		nm36[16];	/* Scaler name */
	char		nm37[16];	/* Scaler name */
	char		nm38[16];	/* Scaler name */
	char		nm39[16];	/* Scaler name */
	char		nm40[16];	/* Scaler name */
	char		nm41[16];	/* Scaler name */
	char		nm42[16];	/* Scaler name */
	char		nm43[16];	/* Scaler name */
	char		nm44[16];	/* Scaler name */
	char		nm45[16];	/* Scaler name */
	char		nm46[16];	/* Scaler name */
	char		nm47[16];	/* Scaler name */
	char		nm48[16];	/* Scaler name */
	char		nm49[16];	/* Scaler name */
	char		nm50[16];	/* Scaler name */
	char		nm51[16];	/* Scaler name */
	char		nm52[16];	/* Scaler name */
	char		nm53[16];	/* Scaler name */
	char		nm54[16];	/* Scaler name */
	char		nm55[16];	/* Scaler name */
	char		nm56[16];	/* Scaler name */
	char		nm57[16];	/* Scaler name */
	char		nm58[16];	/* Scaler name */
	char		nm59[16];	/* Scaler name */
	char		nm60[16];	/* Scaler name */
	char		nm61[16];	/* Scaler name */
	char		nm62[16];	/* Scaler name */
	char		nm63[16];	/* Scaler name */
	char		nm64[16];	/* Scaler name */
	char		egu[16];	/* Units Name */
	epicsInt16	prec;	/* Display Precision */
	DBLINK		cout;	/* CNT Output */
	DBLINK		coutp;	/* CNT Output Prompt */
} scalerRecord;
#define scalerRecordNAME	0
#define scalerRecordDESC	1
#define scalerRecordASG	2
#define scalerRecordSCAN	3
#define scalerRecordPINI	4
#define scalerRecordPHAS	5
#define scalerRecordEVNT	6
#define scalerRecordTSE	7
#define scalerRecordTSEL	8
#define scalerRecordDTYP	9
#define scalerRecordDISV	10
#define scalerRecordDISA	11
#define scalerRecordSDIS	12
#define scalerRecordMLOK	13
#define scalerRecordMLIS	14
#define scalerRecordDISP	15
#define scalerRecordPROC	16
#define scalerRecordSTAT	17
#define scalerRecordSEVR	18
#define scalerRecordNSTA	19
#define scalerRecordNSEV	20
#define scalerRecordACKS	21
#define scalerRecordACKT	22
#define scalerRecordDISS	23
#define scalerRecordLCNT	24
#define scalerRecordPACT	25
#define scalerRecordPUTF	26
#define scalerRecordRPRO	27
#define scalerRecordASP	28
#define scalerRecordPPN	29
#define scalerRecordPPNR	30
#define scalerRecordSPVT	31
#define scalerRecordRSET	32
#define scalerRecordDSET	33
#define scalerRecordDPVT	34
#define scalerRecordRDES	35
#define scalerRecordLSET	36
#define scalerRecordPRIO	37
#define scalerRecordTPRO	38
#define scalerRecordBKPT	39
#define scalerRecordUDF	40
#define scalerRecordTIME	41
#define scalerRecordFLNK	42
#define scalerRecordVERS	43
#define scalerRecordRPVT	44
#define scalerRecordVAL	45
#define scalerRecordFREQ	46
#define scalerRecordCNT	47
#define scalerRecordPCNT	48
#define scalerRecordSS	49
#define scalerRecordUS	50
#define scalerRecordCONT	51
#define scalerRecordOUT	52
#define scalerRecordRATE	53
#define scalerRecordRAT1	54
#define scalerRecordDLY	55
#define scalerRecordDLY1	56
#define scalerRecordNCH	57
#define scalerRecordD1	58
#define scalerRecordD2	59
#define scalerRecordD3	60
#define scalerRecordD4	61
#define scalerRecordD5	62
#define scalerRecordD6	63
#define scalerRecordD7	64
#define scalerRecordD8	65
#define scalerRecordD9	66
#define scalerRecordD10	67
#define scalerRecordD11	68
#define scalerRecordD12	69
#define scalerRecordD13	70
#define scalerRecordD14	71
#define scalerRecordD15	72
#define scalerRecordD16	73
#define scalerRecordD17	74
#define scalerRecordD18	75
#define scalerRecordD19	76
#define scalerRecordD20	77
#define scalerRecordD21	78
#define scalerRecordD22	79
#define scalerRecordD23	80
#define scalerRecordD24	81
#define scalerRecordD25	82
#define scalerRecordD26	83
#define scalerRecordD27	84
#define scalerRecordD28	85
#define scalerRecordD29	86
#define scalerRecordD30	87
#define scalerRecordD31	88
#define scalerRecordD32	89
#define scalerRecordD33	90
#define scalerRecordD34	91
#define scalerRecordD35	92
#define scalerRecordD36	93
#define scalerRecordD37	94
#define scalerRecordD38	95
#define scalerRecordD39	96
#define scalerRecordD40	97
#define scalerRecordD41	98
#define scalerRecordD42	99
#define scalerRecordD43	100
#define scalerRecordD44	101
#define scalerRecordD45	102
#define scalerRecordD46	103
#define scalerRecordD47	104
#define scalerRecordD48	105
#define scalerRecordD49	106
#define scalerRecordD50	107
#define scalerRecordD51	108
#define scalerRecordD52	109
#define scalerRecordD53	110
#define scalerRecordD54	111
#define scalerRecordD55	112
#define scalerRecordD56	113
#define scalerRecordD57	114
#define scalerRecordD58	115
#define scalerRecordD59	116
#define scalerRecordD60	117
#define scalerRecordD61	118
#define scalerRecordD62	119
#define scalerRecordD63	120
#define scalerRecordD64	121
#define scalerRecordG1	122
#define scalerRecordG2	123
#define scalerRecordG3	124
#define scalerRecordG4	125
#define scalerRecordG5	126
#define scalerRecordG6	127
#define scalerRecordG7	128
#define scalerRecordG8	129
#define scalerRecordG9	130
#define scalerRecordG10	131
#define scalerRecordG11	132
#define scalerRecordG12	133
#define scalerRecordG13	134
#define scalerRecordG14	135
#define scalerRecordG15	136
#define scalerRecordG16	137
#define scalerRecordG17	138
#define scalerRecordG18	139
#define scalerRecordG19	140
#define scalerRecordG20	141
#define scalerRecordG21	142
#define scalerRecordG22	143
#define scalerRecordG23	144
#define scalerRecordG24	145
#define scalerRecordG25	146
#define scalerRecordG26	147
#define scalerRecordG27	148
#define scalerRecordG28	149
#define scalerRecordG29	150
#define scalerRecordG30	151
#define scalerRecordG31	152
#define scalerRecordG32	153
#define scalerRecordG33	154
#define scalerRecordG34	155
#define scalerRecordG35	156
#define scalerRecordG36	157
#define scalerRecordG37	158
#define scalerRecordG38	159
#define scalerRecordG39	160
#define scalerRecordG40	161
#define scalerRecordG41	162
#define scalerRecordG42	163
#define scalerRecordG43	164
#define scalerRecordG44	165
#define scalerRecordG45	166
#define scalerRecordG46	167
#define scalerRecordG47	168
#define scalerRecordG48	169
#define scalerRecordG49	170
#define scalerRecordG50	171
#define scalerRecordG51	172
#define scalerRecordG52	173
#define scalerRecordG53	174
#define scalerRecordG54	175
#define scalerRecordG55	176
#define scalerRecordG56	177
#define scalerRecordG57	178
#define scalerRecordG58	179
#define scalerRecordG59	180
#define scalerRecordG60	181
#define scalerRecordG61	182
#define scalerRecordG62	183
#define scalerRecordG63	184
#define scalerRecordG64	185
#define scalerRecordTP	186
#define scalerRecordTP1	187
#define scalerRecordPR1	188
#define scalerRecordPR2	189
#define scalerRecordPR3	190
#define scalerRecordPR4	191
#define scalerRecordPR5	192
#define scalerRecordPR6	193
#define scalerRecordPR7	194
#define scalerRecordPR8	195
#define scalerRecordPR9	196
#define scalerRecordPR10	197
#define scalerRecordPR11	198
#define scalerRecordPR12	199
#define scalerRecordPR13	200
#define scalerRecordPR14	201
#define scalerRecordPR15	202
#define scalerRecordPR16	203
#define scalerRecordPR17	204
#define scalerRecordPR18	205
#define scalerRecordPR19	206
#define scalerRecordPR20	207
#define scalerRecordPR21	208
#define scalerRecordPR22	209
#define scalerRecordPR23	210
#define scalerRecordPR24	211
#define scalerRecordPR25	212
#define scalerRecordPR26	213
#define scalerRecordPR27	214
#define scalerRecordPR28	215
#define scalerRecordPR29	216
#define scalerRecordPR30	217
#define scalerRecordPR31	218
#define scalerRecordPR32	219
#define scalerRecordPR33	220
#define scalerRecordPR34	221
#define scalerRecordPR35	222
#define scalerRecordPR36	223
#define scalerRecordPR37	224
#define scalerRecordPR38	225
#define scalerRecordPR39	226
#define scalerRecordPR40	227
#define scalerRecordPR41	228
#define scalerRecordPR42	229
#define scalerRecordPR43	230
#define scalerRecordPR44	231
#define scalerRecordPR45	232
#define scalerRecordPR46	233
#define scalerRecordPR47	234
#define scalerRecordPR48	235
#define scalerRecordPR49	236
#define scalerRecordPR50	237
#define scalerRecordPR51	238
#define scalerRecordPR52	239
#define scalerRecordPR53	240
#define scalerRecordPR54	241
#define scalerRecordPR55	242
#define scalerRecordPR56	243
#define scalerRecordPR57	244
#define scalerRecordPR58	245
#define scalerRecordPR59	246
#define scalerRecordPR60	247
#define scalerRecordPR61	248
#define scalerRecordPR62	249
#define scalerRecordPR63	250
#define scalerRecordPR64	251
#define scalerRecordT	252
#define scalerRecordS1	253
#define scalerRecordS2	254
#define scalerRecordS3	255
#define scalerRecordS4	256
#define scalerRecordS5	257
#define scalerRecordS6	258
#define scalerRecordS7	259
#define scalerRecordS8	260
#define scalerRecordS9	261
#define scalerRecordS10	262
#define scalerRecordS11	263
#define scalerRecordS12	264
#define scalerRecordS13	265
#define scalerRecordS14	266
#define scalerRecordS15	267
#define scalerRecordS16	268
#define scalerRecordS17	269
#define scalerRecordS18	270
#define scalerRecordS19	271
#define scalerRecordS20	272
#define scalerRecordS21	273
#define scalerRecordS22	274
#define scalerRecordS23	275
#define scalerRecordS24	276
#define scalerRecordS25	277
#define scalerRecordS26	278
#define scalerRecordS27	279
#define scalerRecordS28	280
#define scalerRecordS29	281
#define scalerRecordS30	282
#define scalerRecordS31	283
#define scalerRecordS32	284
#define scalerRecordS33	285
#define scalerRecordS34	286
#define scalerRecordS35	287
#define scalerRecordS36	288
#define scalerRecordS37	289
#define scalerRecordS38	290
#define scalerRecordS39	291
#define scalerRecordS40	292
#define scalerRecordS41	293
#define scalerRecordS42	294
#define scalerRecordS43	295
#define scalerRecordS44	296
#define scalerRecordS45	297
#define scalerRecordS46	298
#define scalerRecordS47	299
#define scalerRecordS48	300
#define scalerRecordS49	301
#define scalerRecordS50	302
#define scalerRecordS51	303
#define scalerRecordS52	304
#define scalerRecordS53	305
#define scalerRecordS54	306
#define scalerRecordS55	307
#define scalerRecordS56	308
#define scalerRecordS57	309
#define scalerRecordS58	310
#define scalerRecordS59	311
#define scalerRecordS60	312
#define scalerRecordS61	313
#define scalerRecordS62	314
#define scalerRecordS63	315
#define scalerRecordS64	316
#define scalerRecordNM1	317
#define scalerRecordNM2	318
#define scalerRecordNM3	319
#define scalerRecordNM4	320
#define scalerRecordNM5	321
#define scalerRecordNM6	322
#define scalerRecordNM7	323
#define scalerRecordNM8	324
#define scalerRecordNM9	325
#define scalerRecordNM10	326
#define scalerRecordNM11	327
#define scalerRecordNM12	328
#define scalerRecordNM13	329
#define scalerRecordNM14	330
#define scalerRecordNM15	331
#define scalerRecordNM16	332
#define scalerRecordNM17	333
#define scalerRecordNM18	334
#define scalerRecordNM19	335
#define scalerRecordNM20	336
#define scalerRecordNM21	337
#define scalerRecordNM22	338
#define scalerRecordNM23	339
#define scalerRecordNM24	340
#define scalerRecordNM25	341
#define scalerRecordNM26	342
#define scalerRecordNM27	343
#define scalerRecordNM28	344
#define scalerRecordNM29	345
#define scalerRecordNM30	346
#define scalerRecordNM31	347
#define scalerRecordNM32	348
#define scalerRecordNM33	349
#define scalerRecordNM34	350
#define scalerRecordNM35	351
#define scalerRecordNM36	352
#define scalerRecordNM37	353
#define scalerRecordNM38	354
#define scalerRecordNM39	355
#define scalerRecordNM40	356
#define scalerRecordNM41	357
#define scalerRecordNM42	358
#define scalerRecordNM43	359
#define scalerRecordNM44	360
#define scalerRecordNM45	361
#define scalerRecordNM46	362
#define scalerRecordNM47	363
#define scalerRecordNM48	364
#define scalerRecordNM49	365
#define scalerRecordNM50	366
#define scalerRecordNM51	367
#define scalerRecordNM52	368
#define scalerRecordNM53	369
#define scalerRecordNM54	370
#define scalerRecordNM55	371
#define scalerRecordNM56	372
#define scalerRecordNM57	373
#define scalerRecordNM58	374
#define scalerRecordNM59	375
#define scalerRecordNM60	376
#define scalerRecordNM61	377
#define scalerRecordNM62	378
#define scalerRecordNM63	379
#define scalerRecordNM64	380
#define scalerRecordEGU	381
#define scalerRecordPREC	382
#define scalerRecordCOUT	383
#define scalerRecordCOUTP	384
#endif /*INCscalerH*/
#ifdef GEN_SIZE_OFFSET
#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int scalerRecordSizeOffset(dbRecordType *pdbRecordType)
{
    scalerRecord *prec = 0;
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
  pdbRecordType->papFldDes[44]->size=sizeof(prec->rpvt);
  pdbRecordType->papFldDes[44]->offset=(short)((char *)&prec->rpvt - (char *)prec);
  pdbRecordType->papFldDes[45]->size=sizeof(prec->val);
  pdbRecordType->papFldDes[45]->offset=(short)((char *)&prec->val - (char *)prec);
  pdbRecordType->papFldDes[46]->size=sizeof(prec->freq);
  pdbRecordType->papFldDes[46]->offset=(short)((char *)&prec->freq - (char *)prec);
  pdbRecordType->papFldDes[47]->size=sizeof(prec->cnt);
  pdbRecordType->papFldDes[47]->offset=(short)((char *)&prec->cnt - (char *)prec);
  pdbRecordType->papFldDes[48]->size=sizeof(prec->pcnt);
  pdbRecordType->papFldDes[48]->offset=(short)((char *)&prec->pcnt - (char *)prec);
  pdbRecordType->papFldDes[49]->size=sizeof(prec->ss);
  pdbRecordType->papFldDes[49]->offset=(short)((char *)&prec->ss - (char *)prec);
  pdbRecordType->papFldDes[50]->size=sizeof(prec->us);
  pdbRecordType->papFldDes[50]->offset=(short)((char *)&prec->us - (char *)prec);
  pdbRecordType->papFldDes[51]->size=sizeof(prec->cont);
  pdbRecordType->papFldDes[51]->offset=(short)((char *)&prec->cont - (char *)prec);
  pdbRecordType->papFldDes[52]->size=sizeof(prec->out);
  pdbRecordType->papFldDes[52]->offset=(short)((char *)&prec->out - (char *)prec);
  pdbRecordType->papFldDes[53]->size=sizeof(prec->rate);
  pdbRecordType->papFldDes[53]->offset=(short)((char *)&prec->rate - (char *)prec);
  pdbRecordType->papFldDes[54]->size=sizeof(prec->rat1);
  pdbRecordType->papFldDes[54]->offset=(short)((char *)&prec->rat1 - (char *)prec);
  pdbRecordType->papFldDes[55]->size=sizeof(prec->dly);
  pdbRecordType->papFldDes[55]->offset=(short)((char *)&prec->dly - (char *)prec);
  pdbRecordType->papFldDes[56]->size=sizeof(prec->dly1);
  pdbRecordType->papFldDes[56]->offset=(short)((char *)&prec->dly1 - (char *)prec);
  pdbRecordType->papFldDes[57]->size=sizeof(prec->nch);
  pdbRecordType->papFldDes[57]->offset=(short)((char *)&prec->nch - (char *)prec);
  pdbRecordType->papFldDes[58]->size=sizeof(prec->d1);
  pdbRecordType->papFldDes[58]->offset=(short)((char *)&prec->d1 - (char *)prec);
  pdbRecordType->papFldDes[59]->size=sizeof(prec->d2);
  pdbRecordType->papFldDes[59]->offset=(short)((char *)&prec->d2 - (char *)prec);
  pdbRecordType->papFldDes[60]->size=sizeof(prec->d3);
  pdbRecordType->papFldDes[60]->offset=(short)((char *)&prec->d3 - (char *)prec);
  pdbRecordType->papFldDes[61]->size=sizeof(prec->d4);
  pdbRecordType->papFldDes[61]->offset=(short)((char *)&prec->d4 - (char *)prec);
  pdbRecordType->papFldDes[62]->size=sizeof(prec->d5);
  pdbRecordType->papFldDes[62]->offset=(short)((char *)&prec->d5 - (char *)prec);
  pdbRecordType->papFldDes[63]->size=sizeof(prec->d6);
  pdbRecordType->papFldDes[63]->offset=(short)((char *)&prec->d6 - (char *)prec);
  pdbRecordType->papFldDes[64]->size=sizeof(prec->d7);
  pdbRecordType->papFldDes[64]->offset=(short)((char *)&prec->d7 - (char *)prec);
  pdbRecordType->papFldDes[65]->size=sizeof(prec->d8);
  pdbRecordType->papFldDes[65]->offset=(short)((char *)&prec->d8 - (char *)prec);
  pdbRecordType->papFldDes[66]->size=sizeof(prec->d9);
  pdbRecordType->papFldDes[66]->offset=(short)((char *)&prec->d9 - (char *)prec);
  pdbRecordType->papFldDes[67]->size=sizeof(prec->d10);
  pdbRecordType->papFldDes[67]->offset=(short)((char *)&prec->d10 - (char *)prec);
  pdbRecordType->papFldDes[68]->size=sizeof(prec->d11);
  pdbRecordType->papFldDes[68]->offset=(short)((char *)&prec->d11 - (char *)prec);
  pdbRecordType->papFldDes[69]->size=sizeof(prec->d12);
  pdbRecordType->papFldDes[69]->offset=(short)((char *)&prec->d12 - (char *)prec);
  pdbRecordType->papFldDes[70]->size=sizeof(prec->d13);
  pdbRecordType->papFldDes[70]->offset=(short)((char *)&prec->d13 - (char *)prec);
  pdbRecordType->papFldDes[71]->size=sizeof(prec->d14);
  pdbRecordType->papFldDes[71]->offset=(short)((char *)&prec->d14 - (char *)prec);
  pdbRecordType->papFldDes[72]->size=sizeof(prec->d15);
  pdbRecordType->papFldDes[72]->offset=(short)((char *)&prec->d15 - (char *)prec);
  pdbRecordType->papFldDes[73]->size=sizeof(prec->d16);
  pdbRecordType->papFldDes[73]->offset=(short)((char *)&prec->d16 - (char *)prec);
  pdbRecordType->papFldDes[74]->size=sizeof(prec->d17);
  pdbRecordType->papFldDes[74]->offset=(short)((char *)&prec->d17 - (char *)prec);
  pdbRecordType->papFldDes[75]->size=sizeof(prec->d18);
  pdbRecordType->papFldDes[75]->offset=(short)((char *)&prec->d18 - (char *)prec);
  pdbRecordType->papFldDes[76]->size=sizeof(prec->d19);
  pdbRecordType->papFldDes[76]->offset=(short)((char *)&prec->d19 - (char *)prec);
  pdbRecordType->papFldDes[77]->size=sizeof(prec->d20);
  pdbRecordType->papFldDes[77]->offset=(short)((char *)&prec->d20 - (char *)prec);
  pdbRecordType->papFldDes[78]->size=sizeof(prec->d21);
  pdbRecordType->papFldDes[78]->offset=(short)((char *)&prec->d21 - (char *)prec);
  pdbRecordType->papFldDes[79]->size=sizeof(prec->d22);
  pdbRecordType->papFldDes[79]->offset=(short)((char *)&prec->d22 - (char *)prec);
  pdbRecordType->papFldDes[80]->size=sizeof(prec->d23);
  pdbRecordType->papFldDes[80]->offset=(short)((char *)&prec->d23 - (char *)prec);
  pdbRecordType->papFldDes[81]->size=sizeof(prec->d24);
  pdbRecordType->papFldDes[81]->offset=(short)((char *)&prec->d24 - (char *)prec);
  pdbRecordType->papFldDes[82]->size=sizeof(prec->d25);
  pdbRecordType->papFldDes[82]->offset=(short)((char *)&prec->d25 - (char *)prec);
  pdbRecordType->papFldDes[83]->size=sizeof(prec->d26);
  pdbRecordType->papFldDes[83]->offset=(short)((char *)&prec->d26 - (char *)prec);
  pdbRecordType->papFldDes[84]->size=sizeof(prec->d27);
  pdbRecordType->papFldDes[84]->offset=(short)((char *)&prec->d27 - (char *)prec);
  pdbRecordType->papFldDes[85]->size=sizeof(prec->d28);
  pdbRecordType->papFldDes[85]->offset=(short)((char *)&prec->d28 - (char *)prec);
  pdbRecordType->papFldDes[86]->size=sizeof(prec->d29);
  pdbRecordType->papFldDes[86]->offset=(short)((char *)&prec->d29 - (char *)prec);
  pdbRecordType->papFldDes[87]->size=sizeof(prec->d30);
  pdbRecordType->papFldDes[87]->offset=(short)((char *)&prec->d30 - (char *)prec);
  pdbRecordType->papFldDes[88]->size=sizeof(prec->d31);
  pdbRecordType->papFldDes[88]->offset=(short)((char *)&prec->d31 - (char *)prec);
  pdbRecordType->papFldDes[89]->size=sizeof(prec->d32);
  pdbRecordType->papFldDes[89]->offset=(short)((char *)&prec->d32 - (char *)prec);
  pdbRecordType->papFldDes[90]->size=sizeof(prec->d33);
  pdbRecordType->papFldDes[90]->offset=(short)((char *)&prec->d33 - (char *)prec);
  pdbRecordType->papFldDes[91]->size=sizeof(prec->d34);
  pdbRecordType->papFldDes[91]->offset=(short)((char *)&prec->d34 - (char *)prec);
  pdbRecordType->papFldDes[92]->size=sizeof(prec->d35);
  pdbRecordType->papFldDes[92]->offset=(short)((char *)&prec->d35 - (char *)prec);
  pdbRecordType->papFldDes[93]->size=sizeof(prec->d36);
  pdbRecordType->papFldDes[93]->offset=(short)((char *)&prec->d36 - (char *)prec);
  pdbRecordType->papFldDes[94]->size=sizeof(prec->d37);
  pdbRecordType->papFldDes[94]->offset=(short)((char *)&prec->d37 - (char *)prec);
  pdbRecordType->papFldDes[95]->size=sizeof(prec->d38);
  pdbRecordType->papFldDes[95]->offset=(short)((char *)&prec->d38 - (char *)prec);
  pdbRecordType->papFldDes[96]->size=sizeof(prec->d39);
  pdbRecordType->papFldDes[96]->offset=(short)((char *)&prec->d39 - (char *)prec);
  pdbRecordType->papFldDes[97]->size=sizeof(prec->d40);
  pdbRecordType->papFldDes[97]->offset=(short)((char *)&prec->d40 - (char *)prec);
  pdbRecordType->papFldDes[98]->size=sizeof(prec->d41);
  pdbRecordType->papFldDes[98]->offset=(short)((char *)&prec->d41 - (char *)prec);
  pdbRecordType->papFldDes[99]->size=sizeof(prec->d42);
  pdbRecordType->papFldDes[99]->offset=(short)((char *)&prec->d42 - (char *)prec);
  pdbRecordType->papFldDes[100]->size=sizeof(prec->d43);
  pdbRecordType->papFldDes[100]->offset=(short)((char *)&prec->d43 - (char *)prec);
  pdbRecordType->papFldDes[101]->size=sizeof(prec->d44);
  pdbRecordType->papFldDes[101]->offset=(short)((char *)&prec->d44 - (char *)prec);
  pdbRecordType->papFldDes[102]->size=sizeof(prec->d45);
  pdbRecordType->papFldDes[102]->offset=(short)((char *)&prec->d45 - (char *)prec);
  pdbRecordType->papFldDes[103]->size=sizeof(prec->d46);
  pdbRecordType->papFldDes[103]->offset=(short)((char *)&prec->d46 - (char *)prec);
  pdbRecordType->papFldDes[104]->size=sizeof(prec->d47);
  pdbRecordType->papFldDes[104]->offset=(short)((char *)&prec->d47 - (char *)prec);
  pdbRecordType->papFldDes[105]->size=sizeof(prec->d48);
  pdbRecordType->papFldDes[105]->offset=(short)((char *)&prec->d48 - (char *)prec);
  pdbRecordType->papFldDes[106]->size=sizeof(prec->d49);
  pdbRecordType->papFldDes[106]->offset=(short)((char *)&prec->d49 - (char *)prec);
  pdbRecordType->papFldDes[107]->size=sizeof(prec->d50);
  pdbRecordType->papFldDes[107]->offset=(short)((char *)&prec->d50 - (char *)prec);
  pdbRecordType->papFldDes[108]->size=sizeof(prec->d51);
  pdbRecordType->papFldDes[108]->offset=(short)((char *)&prec->d51 - (char *)prec);
  pdbRecordType->papFldDes[109]->size=sizeof(prec->d52);
  pdbRecordType->papFldDes[109]->offset=(short)((char *)&prec->d52 - (char *)prec);
  pdbRecordType->papFldDes[110]->size=sizeof(prec->d53);
  pdbRecordType->papFldDes[110]->offset=(short)((char *)&prec->d53 - (char *)prec);
  pdbRecordType->papFldDes[111]->size=sizeof(prec->d54);
  pdbRecordType->papFldDes[111]->offset=(short)((char *)&prec->d54 - (char *)prec);
  pdbRecordType->papFldDes[112]->size=sizeof(prec->d55);
  pdbRecordType->papFldDes[112]->offset=(short)((char *)&prec->d55 - (char *)prec);
  pdbRecordType->papFldDes[113]->size=sizeof(prec->d56);
  pdbRecordType->papFldDes[113]->offset=(short)((char *)&prec->d56 - (char *)prec);
  pdbRecordType->papFldDes[114]->size=sizeof(prec->d57);
  pdbRecordType->papFldDes[114]->offset=(short)((char *)&prec->d57 - (char *)prec);
  pdbRecordType->papFldDes[115]->size=sizeof(prec->d58);
  pdbRecordType->papFldDes[115]->offset=(short)((char *)&prec->d58 - (char *)prec);
  pdbRecordType->papFldDes[116]->size=sizeof(prec->d59);
  pdbRecordType->papFldDes[116]->offset=(short)((char *)&prec->d59 - (char *)prec);
  pdbRecordType->papFldDes[117]->size=sizeof(prec->d60);
  pdbRecordType->papFldDes[117]->offset=(short)((char *)&prec->d60 - (char *)prec);
  pdbRecordType->papFldDes[118]->size=sizeof(prec->d61);
  pdbRecordType->papFldDes[118]->offset=(short)((char *)&prec->d61 - (char *)prec);
  pdbRecordType->papFldDes[119]->size=sizeof(prec->d62);
  pdbRecordType->papFldDes[119]->offset=(short)((char *)&prec->d62 - (char *)prec);
  pdbRecordType->papFldDes[120]->size=sizeof(prec->d63);
  pdbRecordType->papFldDes[120]->offset=(short)((char *)&prec->d63 - (char *)prec);
  pdbRecordType->papFldDes[121]->size=sizeof(prec->d64);
  pdbRecordType->papFldDes[121]->offset=(short)((char *)&prec->d64 - (char *)prec);
  pdbRecordType->papFldDes[122]->size=sizeof(prec->g1);
  pdbRecordType->papFldDes[122]->offset=(short)((char *)&prec->g1 - (char *)prec);
  pdbRecordType->papFldDes[123]->size=sizeof(prec->g2);
  pdbRecordType->papFldDes[123]->offset=(short)((char *)&prec->g2 - (char *)prec);
  pdbRecordType->papFldDes[124]->size=sizeof(prec->g3);
  pdbRecordType->papFldDes[124]->offset=(short)((char *)&prec->g3 - (char *)prec);
  pdbRecordType->papFldDes[125]->size=sizeof(prec->g4);
  pdbRecordType->papFldDes[125]->offset=(short)((char *)&prec->g4 - (char *)prec);
  pdbRecordType->papFldDes[126]->size=sizeof(prec->g5);
  pdbRecordType->papFldDes[126]->offset=(short)((char *)&prec->g5 - (char *)prec);
  pdbRecordType->papFldDes[127]->size=sizeof(prec->g6);
  pdbRecordType->papFldDes[127]->offset=(short)((char *)&prec->g6 - (char *)prec);
  pdbRecordType->papFldDes[128]->size=sizeof(prec->g7);
  pdbRecordType->papFldDes[128]->offset=(short)((char *)&prec->g7 - (char *)prec);
  pdbRecordType->papFldDes[129]->size=sizeof(prec->g8);
  pdbRecordType->papFldDes[129]->offset=(short)((char *)&prec->g8 - (char *)prec);
  pdbRecordType->papFldDes[130]->size=sizeof(prec->g9);
  pdbRecordType->papFldDes[130]->offset=(short)((char *)&prec->g9 - (char *)prec);
  pdbRecordType->papFldDes[131]->size=sizeof(prec->g10);
  pdbRecordType->papFldDes[131]->offset=(short)((char *)&prec->g10 - (char *)prec);
  pdbRecordType->papFldDes[132]->size=sizeof(prec->g11);
  pdbRecordType->papFldDes[132]->offset=(short)((char *)&prec->g11 - (char *)prec);
  pdbRecordType->papFldDes[133]->size=sizeof(prec->g12);
  pdbRecordType->papFldDes[133]->offset=(short)((char *)&prec->g12 - (char *)prec);
  pdbRecordType->papFldDes[134]->size=sizeof(prec->g13);
  pdbRecordType->papFldDes[134]->offset=(short)((char *)&prec->g13 - (char *)prec);
  pdbRecordType->papFldDes[135]->size=sizeof(prec->g14);
  pdbRecordType->papFldDes[135]->offset=(short)((char *)&prec->g14 - (char *)prec);
  pdbRecordType->papFldDes[136]->size=sizeof(prec->g15);
  pdbRecordType->papFldDes[136]->offset=(short)((char *)&prec->g15 - (char *)prec);
  pdbRecordType->papFldDes[137]->size=sizeof(prec->g16);
  pdbRecordType->papFldDes[137]->offset=(short)((char *)&prec->g16 - (char *)prec);
  pdbRecordType->papFldDes[138]->size=sizeof(prec->g17);
  pdbRecordType->papFldDes[138]->offset=(short)((char *)&prec->g17 - (char *)prec);
  pdbRecordType->papFldDes[139]->size=sizeof(prec->g18);
  pdbRecordType->papFldDes[139]->offset=(short)((char *)&prec->g18 - (char *)prec);
  pdbRecordType->papFldDes[140]->size=sizeof(prec->g19);
  pdbRecordType->papFldDes[140]->offset=(short)((char *)&prec->g19 - (char *)prec);
  pdbRecordType->papFldDes[141]->size=sizeof(prec->g20);
  pdbRecordType->papFldDes[141]->offset=(short)((char *)&prec->g20 - (char *)prec);
  pdbRecordType->papFldDes[142]->size=sizeof(prec->g21);
  pdbRecordType->papFldDes[142]->offset=(short)((char *)&prec->g21 - (char *)prec);
  pdbRecordType->papFldDes[143]->size=sizeof(prec->g22);
  pdbRecordType->papFldDes[143]->offset=(short)((char *)&prec->g22 - (char *)prec);
  pdbRecordType->papFldDes[144]->size=sizeof(prec->g23);
  pdbRecordType->papFldDes[144]->offset=(short)((char *)&prec->g23 - (char *)prec);
  pdbRecordType->papFldDes[145]->size=sizeof(prec->g24);
  pdbRecordType->papFldDes[145]->offset=(short)((char *)&prec->g24 - (char *)prec);
  pdbRecordType->papFldDes[146]->size=sizeof(prec->g25);
  pdbRecordType->papFldDes[146]->offset=(short)((char *)&prec->g25 - (char *)prec);
  pdbRecordType->papFldDes[147]->size=sizeof(prec->g26);
  pdbRecordType->papFldDes[147]->offset=(short)((char *)&prec->g26 - (char *)prec);
  pdbRecordType->papFldDes[148]->size=sizeof(prec->g27);
  pdbRecordType->papFldDes[148]->offset=(short)((char *)&prec->g27 - (char *)prec);
  pdbRecordType->papFldDes[149]->size=sizeof(prec->g28);
  pdbRecordType->papFldDes[149]->offset=(short)((char *)&prec->g28 - (char *)prec);
  pdbRecordType->papFldDes[150]->size=sizeof(prec->g29);
  pdbRecordType->papFldDes[150]->offset=(short)((char *)&prec->g29 - (char *)prec);
  pdbRecordType->papFldDes[151]->size=sizeof(prec->g30);
  pdbRecordType->papFldDes[151]->offset=(short)((char *)&prec->g30 - (char *)prec);
  pdbRecordType->papFldDes[152]->size=sizeof(prec->g31);
  pdbRecordType->papFldDes[152]->offset=(short)((char *)&prec->g31 - (char *)prec);
  pdbRecordType->papFldDes[153]->size=sizeof(prec->g32);
  pdbRecordType->papFldDes[153]->offset=(short)((char *)&prec->g32 - (char *)prec);
  pdbRecordType->papFldDes[154]->size=sizeof(prec->g33);
  pdbRecordType->papFldDes[154]->offset=(short)((char *)&prec->g33 - (char *)prec);
  pdbRecordType->papFldDes[155]->size=sizeof(prec->g34);
  pdbRecordType->papFldDes[155]->offset=(short)((char *)&prec->g34 - (char *)prec);
  pdbRecordType->papFldDes[156]->size=sizeof(prec->g35);
  pdbRecordType->papFldDes[156]->offset=(short)((char *)&prec->g35 - (char *)prec);
  pdbRecordType->papFldDes[157]->size=sizeof(prec->g36);
  pdbRecordType->papFldDes[157]->offset=(short)((char *)&prec->g36 - (char *)prec);
  pdbRecordType->papFldDes[158]->size=sizeof(prec->g37);
  pdbRecordType->papFldDes[158]->offset=(short)((char *)&prec->g37 - (char *)prec);
  pdbRecordType->papFldDes[159]->size=sizeof(prec->g38);
  pdbRecordType->papFldDes[159]->offset=(short)((char *)&prec->g38 - (char *)prec);
  pdbRecordType->papFldDes[160]->size=sizeof(prec->g39);
  pdbRecordType->papFldDes[160]->offset=(short)((char *)&prec->g39 - (char *)prec);
  pdbRecordType->papFldDes[161]->size=sizeof(prec->g40);
  pdbRecordType->papFldDes[161]->offset=(short)((char *)&prec->g40 - (char *)prec);
  pdbRecordType->papFldDes[162]->size=sizeof(prec->g41);
  pdbRecordType->papFldDes[162]->offset=(short)((char *)&prec->g41 - (char *)prec);
  pdbRecordType->papFldDes[163]->size=sizeof(prec->g42);
  pdbRecordType->papFldDes[163]->offset=(short)((char *)&prec->g42 - (char *)prec);
  pdbRecordType->papFldDes[164]->size=sizeof(prec->g43);
  pdbRecordType->papFldDes[164]->offset=(short)((char *)&prec->g43 - (char *)prec);
  pdbRecordType->papFldDes[165]->size=sizeof(prec->g44);
  pdbRecordType->papFldDes[165]->offset=(short)((char *)&prec->g44 - (char *)prec);
  pdbRecordType->papFldDes[166]->size=sizeof(prec->g45);
  pdbRecordType->papFldDes[166]->offset=(short)((char *)&prec->g45 - (char *)prec);
  pdbRecordType->papFldDes[167]->size=sizeof(prec->g46);
  pdbRecordType->papFldDes[167]->offset=(short)((char *)&prec->g46 - (char *)prec);
  pdbRecordType->papFldDes[168]->size=sizeof(prec->g47);
  pdbRecordType->papFldDes[168]->offset=(short)((char *)&prec->g47 - (char *)prec);
  pdbRecordType->papFldDes[169]->size=sizeof(prec->g48);
  pdbRecordType->papFldDes[169]->offset=(short)((char *)&prec->g48 - (char *)prec);
  pdbRecordType->papFldDes[170]->size=sizeof(prec->g49);
  pdbRecordType->papFldDes[170]->offset=(short)((char *)&prec->g49 - (char *)prec);
  pdbRecordType->papFldDes[171]->size=sizeof(prec->g50);
  pdbRecordType->papFldDes[171]->offset=(short)((char *)&prec->g50 - (char *)prec);
  pdbRecordType->papFldDes[172]->size=sizeof(prec->g51);
  pdbRecordType->papFldDes[172]->offset=(short)((char *)&prec->g51 - (char *)prec);
  pdbRecordType->papFldDes[173]->size=sizeof(prec->g52);
  pdbRecordType->papFldDes[173]->offset=(short)((char *)&prec->g52 - (char *)prec);
  pdbRecordType->papFldDes[174]->size=sizeof(prec->g53);
  pdbRecordType->papFldDes[174]->offset=(short)((char *)&prec->g53 - (char *)prec);
  pdbRecordType->papFldDes[175]->size=sizeof(prec->g54);
  pdbRecordType->papFldDes[175]->offset=(short)((char *)&prec->g54 - (char *)prec);
  pdbRecordType->papFldDes[176]->size=sizeof(prec->g55);
  pdbRecordType->papFldDes[176]->offset=(short)((char *)&prec->g55 - (char *)prec);
  pdbRecordType->papFldDes[177]->size=sizeof(prec->g56);
  pdbRecordType->papFldDes[177]->offset=(short)((char *)&prec->g56 - (char *)prec);
  pdbRecordType->papFldDes[178]->size=sizeof(prec->g57);
  pdbRecordType->papFldDes[178]->offset=(short)((char *)&prec->g57 - (char *)prec);
  pdbRecordType->papFldDes[179]->size=sizeof(prec->g58);
  pdbRecordType->papFldDes[179]->offset=(short)((char *)&prec->g58 - (char *)prec);
  pdbRecordType->papFldDes[180]->size=sizeof(prec->g59);
  pdbRecordType->papFldDes[180]->offset=(short)((char *)&prec->g59 - (char *)prec);
  pdbRecordType->papFldDes[181]->size=sizeof(prec->g60);
  pdbRecordType->papFldDes[181]->offset=(short)((char *)&prec->g60 - (char *)prec);
  pdbRecordType->papFldDes[182]->size=sizeof(prec->g61);
  pdbRecordType->papFldDes[182]->offset=(short)((char *)&prec->g61 - (char *)prec);
  pdbRecordType->papFldDes[183]->size=sizeof(prec->g62);
  pdbRecordType->papFldDes[183]->offset=(short)((char *)&prec->g62 - (char *)prec);
  pdbRecordType->papFldDes[184]->size=sizeof(prec->g63);
  pdbRecordType->papFldDes[184]->offset=(short)((char *)&prec->g63 - (char *)prec);
  pdbRecordType->papFldDes[185]->size=sizeof(prec->g64);
  pdbRecordType->papFldDes[185]->offset=(short)((char *)&prec->g64 - (char *)prec);
  pdbRecordType->papFldDes[186]->size=sizeof(prec->tp);
  pdbRecordType->papFldDes[186]->offset=(short)((char *)&prec->tp - (char *)prec);
  pdbRecordType->papFldDes[187]->size=sizeof(prec->tp1);
  pdbRecordType->papFldDes[187]->offset=(short)((char *)&prec->tp1 - (char *)prec);
  pdbRecordType->papFldDes[188]->size=sizeof(prec->pr1);
  pdbRecordType->papFldDes[188]->offset=(short)((char *)&prec->pr1 - (char *)prec);
  pdbRecordType->papFldDes[189]->size=sizeof(prec->pr2);
  pdbRecordType->papFldDes[189]->offset=(short)((char *)&prec->pr2 - (char *)prec);
  pdbRecordType->papFldDes[190]->size=sizeof(prec->pr3);
  pdbRecordType->papFldDes[190]->offset=(short)((char *)&prec->pr3 - (char *)prec);
  pdbRecordType->papFldDes[191]->size=sizeof(prec->pr4);
  pdbRecordType->papFldDes[191]->offset=(short)((char *)&prec->pr4 - (char *)prec);
  pdbRecordType->papFldDes[192]->size=sizeof(prec->pr5);
  pdbRecordType->papFldDes[192]->offset=(short)((char *)&prec->pr5 - (char *)prec);
  pdbRecordType->papFldDes[193]->size=sizeof(prec->pr6);
  pdbRecordType->papFldDes[193]->offset=(short)((char *)&prec->pr6 - (char *)prec);
  pdbRecordType->papFldDes[194]->size=sizeof(prec->pr7);
  pdbRecordType->papFldDes[194]->offset=(short)((char *)&prec->pr7 - (char *)prec);
  pdbRecordType->papFldDes[195]->size=sizeof(prec->pr8);
  pdbRecordType->papFldDes[195]->offset=(short)((char *)&prec->pr8 - (char *)prec);
  pdbRecordType->papFldDes[196]->size=sizeof(prec->pr9);
  pdbRecordType->papFldDes[196]->offset=(short)((char *)&prec->pr9 - (char *)prec);
  pdbRecordType->papFldDes[197]->size=sizeof(prec->pr10);
  pdbRecordType->papFldDes[197]->offset=(short)((char *)&prec->pr10 - (char *)prec);
  pdbRecordType->papFldDes[198]->size=sizeof(prec->pr11);
  pdbRecordType->papFldDes[198]->offset=(short)((char *)&prec->pr11 - (char *)prec);
  pdbRecordType->papFldDes[199]->size=sizeof(prec->pr12);
  pdbRecordType->papFldDes[199]->offset=(short)((char *)&prec->pr12 - (char *)prec);
  pdbRecordType->papFldDes[200]->size=sizeof(prec->pr13);
  pdbRecordType->papFldDes[200]->offset=(short)((char *)&prec->pr13 - (char *)prec);
  pdbRecordType->papFldDes[201]->size=sizeof(prec->pr14);
  pdbRecordType->papFldDes[201]->offset=(short)((char *)&prec->pr14 - (char *)prec);
  pdbRecordType->papFldDes[202]->size=sizeof(prec->pr15);
  pdbRecordType->papFldDes[202]->offset=(short)((char *)&prec->pr15 - (char *)prec);
  pdbRecordType->papFldDes[203]->size=sizeof(prec->pr16);
  pdbRecordType->papFldDes[203]->offset=(short)((char *)&prec->pr16 - (char *)prec);
  pdbRecordType->papFldDes[204]->size=sizeof(prec->pr17);
  pdbRecordType->papFldDes[204]->offset=(short)((char *)&prec->pr17 - (char *)prec);
  pdbRecordType->papFldDes[205]->size=sizeof(prec->pr18);
  pdbRecordType->papFldDes[205]->offset=(short)((char *)&prec->pr18 - (char *)prec);
  pdbRecordType->papFldDes[206]->size=sizeof(prec->pr19);
  pdbRecordType->papFldDes[206]->offset=(short)((char *)&prec->pr19 - (char *)prec);
  pdbRecordType->papFldDes[207]->size=sizeof(prec->pr20);
  pdbRecordType->papFldDes[207]->offset=(short)((char *)&prec->pr20 - (char *)prec);
  pdbRecordType->papFldDes[208]->size=sizeof(prec->pr21);
  pdbRecordType->papFldDes[208]->offset=(short)((char *)&prec->pr21 - (char *)prec);
  pdbRecordType->papFldDes[209]->size=sizeof(prec->pr22);
  pdbRecordType->papFldDes[209]->offset=(short)((char *)&prec->pr22 - (char *)prec);
  pdbRecordType->papFldDes[210]->size=sizeof(prec->pr23);
  pdbRecordType->papFldDes[210]->offset=(short)((char *)&prec->pr23 - (char *)prec);
  pdbRecordType->papFldDes[211]->size=sizeof(prec->pr24);
  pdbRecordType->papFldDes[211]->offset=(short)((char *)&prec->pr24 - (char *)prec);
  pdbRecordType->papFldDes[212]->size=sizeof(prec->pr25);
  pdbRecordType->papFldDes[212]->offset=(short)((char *)&prec->pr25 - (char *)prec);
  pdbRecordType->papFldDes[213]->size=sizeof(prec->pr26);
  pdbRecordType->papFldDes[213]->offset=(short)((char *)&prec->pr26 - (char *)prec);
  pdbRecordType->papFldDes[214]->size=sizeof(prec->pr27);
  pdbRecordType->papFldDes[214]->offset=(short)((char *)&prec->pr27 - (char *)prec);
  pdbRecordType->papFldDes[215]->size=sizeof(prec->pr28);
  pdbRecordType->papFldDes[215]->offset=(short)((char *)&prec->pr28 - (char *)prec);
  pdbRecordType->papFldDes[216]->size=sizeof(prec->pr29);
  pdbRecordType->papFldDes[216]->offset=(short)((char *)&prec->pr29 - (char *)prec);
  pdbRecordType->papFldDes[217]->size=sizeof(prec->pr30);
  pdbRecordType->papFldDes[217]->offset=(short)((char *)&prec->pr30 - (char *)prec);
  pdbRecordType->papFldDes[218]->size=sizeof(prec->pr31);
  pdbRecordType->papFldDes[218]->offset=(short)((char *)&prec->pr31 - (char *)prec);
  pdbRecordType->papFldDes[219]->size=sizeof(prec->pr32);
  pdbRecordType->papFldDes[219]->offset=(short)((char *)&prec->pr32 - (char *)prec);
  pdbRecordType->papFldDes[220]->size=sizeof(prec->pr33);
  pdbRecordType->papFldDes[220]->offset=(short)((char *)&prec->pr33 - (char *)prec);
  pdbRecordType->papFldDes[221]->size=sizeof(prec->pr34);
  pdbRecordType->papFldDes[221]->offset=(short)((char *)&prec->pr34 - (char *)prec);
  pdbRecordType->papFldDes[222]->size=sizeof(prec->pr35);
  pdbRecordType->papFldDes[222]->offset=(short)((char *)&prec->pr35 - (char *)prec);
  pdbRecordType->papFldDes[223]->size=sizeof(prec->pr36);
  pdbRecordType->papFldDes[223]->offset=(short)((char *)&prec->pr36 - (char *)prec);
  pdbRecordType->papFldDes[224]->size=sizeof(prec->pr37);
  pdbRecordType->papFldDes[224]->offset=(short)((char *)&prec->pr37 - (char *)prec);
  pdbRecordType->papFldDes[225]->size=sizeof(prec->pr38);
  pdbRecordType->papFldDes[225]->offset=(short)((char *)&prec->pr38 - (char *)prec);
  pdbRecordType->papFldDes[226]->size=sizeof(prec->pr39);
  pdbRecordType->papFldDes[226]->offset=(short)((char *)&prec->pr39 - (char *)prec);
  pdbRecordType->papFldDes[227]->size=sizeof(prec->pr40);
  pdbRecordType->papFldDes[227]->offset=(short)((char *)&prec->pr40 - (char *)prec);
  pdbRecordType->papFldDes[228]->size=sizeof(prec->pr41);
  pdbRecordType->papFldDes[228]->offset=(short)((char *)&prec->pr41 - (char *)prec);
  pdbRecordType->papFldDes[229]->size=sizeof(prec->pr42);
  pdbRecordType->papFldDes[229]->offset=(short)((char *)&prec->pr42 - (char *)prec);
  pdbRecordType->papFldDes[230]->size=sizeof(prec->pr43);
  pdbRecordType->papFldDes[230]->offset=(short)((char *)&prec->pr43 - (char *)prec);
  pdbRecordType->papFldDes[231]->size=sizeof(prec->pr44);
  pdbRecordType->papFldDes[231]->offset=(short)((char *)&prec->pr44 - (char *)prec);
  pdbRecordType->papFldDes[232]->size=sizeof(prec->pr45);
  pdbRecordType->papFldDes[232]->offset=(short)((char *)&prec->pr45 - (char *)prec);
  pdbRecordType->papFldDes[233]->size=sizeof(prec->pr46);
  pdbRecordType->papFldDes[233]->offset=(short)((char *)&prec->pr46 - (char *)prec);
  pdbRecordType->papFldDes[234]->size=sizeof(prec->pr47);
  pdbRecordType->papFldDes[234]->offset=(short)((char *)&prec->pr47 - (char *)prec);
  pdbRecordType->papFldDes[235]->size=sizeof(prec->pr48);
  pdbRecordType->papFldDes[235]->offset=(short)((char *)&prec->pr48 - (char *)prec);
  pdbRecordType->papFldDes[236]->size=sizeof(prec->pr49);
  pdbRecordType->papFldDes[236]->offset=(short)((char *)&prec->pr49 - (char *)prec);
  pdbRecordType->papFldDes[237]->size=sizeof(prec->pr50);
  pdbRecordType->papFldDes[237]->offset=(short)((char *)&prec->pr50 - (char *)prec);
  pdbRecordType->papFldDes[238]->size=sizeof(prec->pr51);
  pdbRecordType->papFldDes[238]->offset=(short)((char *)&prec->pr51 - (char *)prec);
  pdbRecordType->papFldDes[239]->size=sizeof(prec->pr52);
  pdbRecordType->papFldDes[239]->offset=(short)((char *)&prec->pr52 - (char *)prec);
  pdbRecordType->papFldDes[240]->size=sizeof(prec->pr53);
  pdbRecordType->papFldDes[240]->offset=(short)((char *)&prec->pr53 - (char *)prec);
  pdbRecordType->papFldDes[241]->size=sizeof(prec->pr54);
  pdbRecordType->papFldDes[241]->offset=(short)((char *)&prec->pr54 - (char *)prec);
  pdbRecordType->papFldDes[242]->size=sizeof(prec->pr55);
  pdbRecordType->papFldDes[242]->offset=(short)((char *)&prec->pr55 - (char *)prec);
  pdbRecordType->papFldDes[243]->size=sizeof(prec->pr56);
  pdbRecordType->papFldDes[243]->offset=(short)((char *)&prec->pr56 - (char *)prec);
  pdbRecordType->papFldDes[244]->size=sizeof(prec->pr57);
  pdbRecordType->papFldDes[244]->offset=(short)((char *)&prec->pr57 - (char *)prec);
  pdbRecordType->papFldDes[245]->size=sizeof(prec->pr58);
  pdbRecordType->papFldDes[245]->offset=(short)((char *)&prec->pr58 - (char *)prec);
  pdbRecordType->papFldDes[246]->size=sizeof(prec->pr59);
  pdbRecordType->papFldDes[246]->offset=(short)((char *)&prec->pr59 - (char *)prec);
  pdbRecordType->papFldDes[247]->size=sizeof(prec->pr60);
  pdbRecordType->papFldDes[247]->offset=(short)((char *)&prec->pr60 - (char *)prec);
  pdbRecordType->papFldDes[248]->size=sizeof(prec->pr61);
  pdbRecordType->papFldDes[248]->offset=(short)((char *)&prec->pr61 - (char *)prec);
  pdbRecordType->papFldDes[249]->size=sizeof(prec->pr62);
  pdbRecordType->papFldDes[249]->offset=(short)((char *)&prec->pr62 - (char *)prec);
  pdbRecordType->papFldDes[250]->size=sizeof(prec->pr63);
  pdbRecordType->papFldDes[250]->offset=(short)((char *)&prec->pr63 - (char *)prec);
  pdbRecordType->papFldDes[251]->size=sizeof(prec->pr64);
  pdbRecordType->papFldDes[251]->offset=(short)((char *)&prec->pr64 - (char *)prec);
  pdbRecordType->papFldDes[252]->size=sizeof(prec->t);
  pdbRecordType->papFldDes[252]->offset=(short)((char *)&prec->t - (char *)prec);
  pdbRecordType->papFldDes[253]->size=sizeof(prec->s1);
  pdbRecordType->papFldDes[253]->offset=(short)((char *)&prec->s1 - (char *)prec);
  pdbRecordType->papFldDes[254]->size=sizeof(prec->s2);
  pdbRecordType->papFldDes[254]->offset=(short)((char *)&prec->s2 - (char *)prec);
  pdbRecordType->papFldDes[255]->size=sizeof(prec->s3);
  pdbRecordType->papFldDes[255]->offset=(short)((char *)&prec->s3 - (char *)prec);
  pdbRecordType->papFldDes[256]->size=sizeof(prec->s4);
  pdbRecordType->papFldDes[256]->offset=(short)((char *)&prec->s4 - (char *)prec);
  pdbRecordType->papFldDes[257]->size=sizeof(prec->s5);
  pdbRecordType->papFldDes[257]->offset=(short)((char *)&prec->s5 - (char *)prec);
  pdbRecordType->papFldDes[258]->size=sizeof(prec->s6);
  pdbRecordType->papFldDes[258]->offset=(short)((char *)&prec->s6 - (char *)prec);
  pdbRecordType->papFldDes[259]->size=sizeof(prec->s7);
  pdbRecordType->papFldDes[259]->offset=(short)((char *)&prec->s7 - (char *)prec);
  pdbRecordType->papFldDes[260]->size=sizeof(prec->s8);
  pdbRecordType->papFldDes[260]->offset=(short)((char *)&prec->s8 - (char *)prec);
  pdbRecordType->papFldDes[261]->size=sizeof(prec->s9);
  pdbRecordType->papFldDes[261]->offset=(short)((char *)&prec->s9 - (char *)prec);
  pdbRecordType->papFldDes[262]->size=sizeof(prec->s10);
  pdbRecordType->papFldDes[262]->offset=(short)((char *)&prec->s10 - (char *)prec);
  pdbRecordType->papFldDes[263]->size=sizeof(prec->s11);
  pdbRecordType->papFldDes[263]->offset=(short)((char *)&prec->s11 - (char *)prec);
  pdbRecordType->papFldDes[264]->size=sizeof(prec->s12);
  pdbRecordType->papFldDes[264]->offset=(short)((char *)&prec->s12 - (char *)prec);
  pdbRecordType->papFldDes[265]->size=sizeof(prec->s13);
  pdbRecordType->papFldDes[265]->offset=(short)((char *)&prec->s13 - (char *)prec);
  pdbRecordType->papFldDes[266]->size=sizeof(prec->s14);
  pdbRecordType->papFldDes[266]->offset=(short)((char *)&prec->s14 - (char *)prec);
  pdbRecordType->papFldDes[267]->size=sizeof(prec->s15);
  pdbRecordType->papFldDes[267]->offset=(short)((char *)&prec->s15 - (char *)prec);
  pdbRecordType->papFldDes[268]->size=sizeof(prec->s16);
  pdbRecordType->papFldDes[268]->offset=(short)((char *)&prec->s16 - (char *)prec);
  pdbRecordType->papFldDes[269]->size=sizeof(prec->s17);
  pdbRecordType->papFldDes[269]->offset=(short)((char *)&prec->s17 - (char *)prec);
  pdbRecordType->papFldDes[270]->size=sizeof(prec->s18);
  pdbRecordType->papFldDes[270]->offset=(short)((char *)&prec->s18 - (char *)prec);
  pdbRecordType->papFldDes[271]->size=sizeof(prec->s19);
  pdbRecordType->papFldDes[271]->offset=(short)((char *)&prec->s19 - (char *)prec);
  pdbRecordType->papFldDes[272]->size=sizeof(prec->s20);
  pdbRecordType->papFldDes[272]->offset=(short)((char *)&prec->s20 - (char *)prec);
  pdbRecordType->papFldDes[273]->size=sizeof(prec->s21);
  pdbRecordType->papFldDes[273]->offset=(short)((char *)&prec->s21 - (char *)prec);
  pdbRecordType->papFldDes[274]->size=sizeof(prec->s22);
  pdbRecordType->papFldDes[274]->offset=(short)((char *)&prec->s22 - (char *)prec);
  pdbRecordType->papFldDes[275]->size=sizeof(prec->s23);
  pdbRecordType->papFldDes[275]->offset=(short)((char *)&prec->s23 - (char *)prec);
  pdbRecordType->papFldDes[276]->size=sizeof(prec->s24);
  pdbRecordType->papFldDes[276]->offset=(short)((char *)&prec->s24 - (char *)prec);
  pdbRecordType->papFldDes[277]->size=sizeof(prec->s25);
  pdbRecordType->papFldDes[277]->offset=(short)((char *)&prec->s25 - (char *)prec);
  pdbRecordType->papFldDes[278]->size=sizeof(prec->s26);
  pdbRecordType->papFldDes[278]->offset=(short)((char *)&prec->s26 - (char *)prec);
  pdbRecordType->papFldDes[279]->size=sizeof(prec->s27);
  pdbRecordType->papFldDes[279]->offset=(short)((char *)&prec->s27 - (char *)prec);
  pdbRecordType->papFldDes[280]->size=sizeof(prec->s28);
  pdbRecordType->papFldDes[280]->offset=(short)((char *)&prec->s28 - (char *)prec);
  pdbRecordType->papFldDes[281]->size=sizeof(prec->s29);
  pdbRecordType->papFldDes[281]->offset=(short)((char *)&prec->s29 - (char *)prec);
  pdbRecordType->papFldDes[282]->size=sizeof(prec->s30);
  pdbRecordType->papFldDes[282]->offset=(short)((char *)&prec->s30 - (char *)prec);
  pdbRecordType->papFldDes[283]->size=sizeof(prec->s31);
  pdbRecordType->papFldDes[283]->offset=(short)((char *)&prec->s31 - (char *)prec);
  pdbRecordType->papFldDes[284]->size=sizeof(prec->s32);
  pdbRecordType->papFldDes[284]->offset=(short)((char *)&prec->s32 - (char *)prec);
  pdbRecordType->papFldDes[285]->size=sizeof(prec->s33);
  pdbRecordType->papFldDes[285]->offset=(short)((char *)&prec->s33 - (char *)prec);
  pdbRecordType->papFldDes[286]->size=sizeof(prec->s34);
  pdbRecordType->papFldDes[286]->offset=(short)((char *)&prec->s34 - (char *)prec);
  pdbRecordType->papFldDes[287]->size=sizeof(prec->s35);
  pdbRecordType->papFldDes[287]->offset=(short)((char *)&prec->s35 - (char *)prec);
  pdbRecordType->papFldDes[288]->size=sizeof(prec->s36);
  pdbRecordType->papFldDes[288]->offset=(short)((char *)&prec->s36 - (char *)prec);
  pdbRecordType->papFldDes[289]->size=sizeof(prec->s37);
  pdbRecordType->papFldDes[289]->offset=(short)((char *)&prec->s37 - (char *)prec);
  pdbRecordType->papFldDes[290]->size=sizeof(prec->s38);
  pdbRecordType->papFldDes[290]->offset=(short)((char *)&prec->s38 - (char *)prec);
  pdbRecordType->papFldDes[291]->size=sizeof(prec->s39);
  pdbRecordType->papFldDes[291]->offset=(short)((char *)&prec->s39 - (char *)prec);
  pdbRecordType->papFldDes[292]->size=sizeof(prec->s40);
  pdbRecordType->papFldDes[292]->offset=(short)((char *)&prec->s40 - (char *)prec);
  pdbRecordType->papFldDes[293]->size=sizeof(prec->s41);
  pdbRecordType->papFldDes[293]->offset=(short)((char *)&prec->s41 - (char *)prec);
  pdbRecordType->papFldDes[294]->size=sizeof(prec->s42);
  pdbRecordType->papFldDes[294]->offset=(short)((char *)&prec->s42 - (char *)prec);
  pdbRecordType->papFldDes[295]->size=sizeof(prec->s43);
  pdbRecordType->papFldDes[295]->offset=(short)((char *)&prec->s43 - (char *)prec);
  pdbRecordType->papFldDes[296]->size=sizeof(prec->s44);
  pdbRecordType->papFldDes[296]->offset=(short)((char *)&prec->s44 - (char *)prec);
  pdbRecordType->papFldDes[297]->size=sizeof(prec->s45);
  pdbRecordType->papFldDes[297]->offset=(short)((char *)&prec->s45 - (char *)prec);
  pdbRecordType->papFldDes[298]->size=sizeof(prec->s46);
  pdbRecordType->papFldDes[298]->offset=(short)((char *)&prec->s46 - (char *)prec);
  pdbRecordType->papFldDes[299]->size=sizeof(prec->s47);
  pdbRecordType->papFldDes[299]->offset=(short)((char *)&prec->s47 - (char *)prec);
  pdbRecordType->papFldDes[300]->size=sizeof(prec->s48);
  pdbRecordType->papFldDes[300]->offset=(short)((char *)&prec->s48 - (char *)prec);
  pdbRecordType->papFldDes[301]->size=sizeof(prec->s49);
  pdbRecordType->papFldDes[301]->offset=(short)((char *)&prec->s49 - (char *)prec);
  pdbRecordType->papFldDes[302]->size=sizeof(prec->s50);
  pdbRecordType->papFldDes[302]->offset=(short)((char *)&prec->s50 - (char *)prec);
  pdbRecordType->papFldDes[303]->size=sizeof(prec->s51);
  pdbRecordType->papFldDes[303]->offset=(short)((char *)&prec->s51 - (char *)prec);
  pdbRecordType->papFldDes[304]->size=sizeof(prec->s52);
  pdbRecordType->papFldDes[304]->offset=(short)((char *)&prec->s52 - (char *)prec);
  pdbRecordType->papFldDes[305]->size=sizeof(prec->s53);
  pdbRecordType->papFldDes[305]->offset=(short)((char *)&prec->s53 - (char *)prec);
  pdbRecordType->papFldDes[306]->size=sizeof(prec->s54);
  pdbRecordType->papFldDes[306]->offset=(short)((char *)&prec->s54 - (char *)prec);
  pdbRecordType->papFldDes[307]->size=sizeof(prec->s55);
  pdbRecordType->papFldDes[307]->offset=(short)((char *)&prec->s55 - (char *)prec);
  pdbRecordType->papFldDes[308]->size=sizeof(prec->s56);
  pdbRecordType->papFldDes[308]->offset=(short)((char *)&prec->s56 - (char *)prec);
  pdbRecordType->papFldDes[309]->size=sizeof(prec->s57);
  pdbRecordType->papFldDes[309]->offset=(short)((char *)&prec->s57 - (char *)prec);
  pdbRecordType->papFldDes[310]->size=sizeof(prec->s58);
  pdbRecordType->papFldDes[310]->offset=(short)((char *)&prec->s58 - (char *)prec);
  pdbRecordType->papFldDes[311]->size=sizeof(prec->s59);
  pdbRecordType->papFldDes[311]->offset=(short)((char *)&prec->s59 - (char *)prec);
  pdbRecordType->papFldDes[312]->size=sizeof(prec->s60);
  pdbRecordType->papFldDes[312]->offset=(short)((char *)&prec->s60 - (char *)prec);
  pdbRecordType->papFldDes[313]->size=sizeof(prec->s61);
  pdbRecordType->papFldDes[313]->offset=(short)((char *)&prec->s61 - (char *)prec);
  pdbRecordType->papFldDes[314]->size=sizeof(prec->s62);
  pdbRecordType->papFldDes[314]->offset=(short)((char *)&prec->s62 - (char *)prec);
  pdbRecordType->papFldDes[315]->size=sizeof(prec->s63);
  pdbRecordType->papFldDes[315]->offset=(short)((char *)&prec->s63 - (char *)prec);
  pdbRecordType->papFldDes[316]->size=sizeof(prec->s64);
  pdbRecordType->papFldDes[316]->offset=(short)((char *)&prec->s64 - (char *)prec);
  pdbRecordType->papFldDes[317]->size=sizeof(prec->nm1);
  pdbRecordType->papFldDes[317]->offset=(short)((char *)&prec->nm1 - (char *)prec);
  pdbRecordType->papFldDes[318]->size=sizeof(prec->nm2);
  pdbRecordType->papFldDes[318]->offset=(short)((char *)&prec->nm2 - (char *)prec);
  pdbRecordType->papFldDes[319]->size=sizeof(prec->nm3);
  pdbRecordType->papFldDes[319]->offset=(short)((char *)&prec->nm3 - (char *)prec);
  pdbRecordType->papFldDes[320]->size=sizeof(prec->nm4);
  pdbRecordType->papFldDes[320]->offset=(short)((char *)&prec->nm4 - (char *)prec);
  pdbRecordType->papFldDes[321]->size=sizeof(prec->nm5);
  pdbRecordType->papFldDes[321]->offset=(short)((char *)&prec->nm5 - (char *)prec);
  pdbRecordType->papFldDes[322]->size=sizeof(prec->nm6);
  pdbRecordType->papFldDes[322]->offset=(short)((char *)&prec->nm6 - (char *)prec);
  pdbRecordType->papFldDes[323]->size=sizeof(prec->nm7);
  pdbRecordType->papFldDes[323]->offset=(short)((char *)&prec->nm7 - (char *)prec);
  pdbRecordType->papFldDes[324]->size=sizeof(prec->nm8);
  pdbRecordType->papFldDes[324]->offset=(short)((char *)&prec->nm8 - (char *)prec);
  pdbRecordType->papFldDes[325]->size=sizeof(prec->nm9);
  pdbRecordType->papFldDes[325]->offset=(short)((char *)&prec->nm9 - (char *)prec);
  pdbRecordType->papFldDes[326]->size=sizeof(prec->nm10);
  pdbRecordType->papFldDes[326]->offset=(short)((char *)&prec->nm10 - (char *)prec);
  pdbRecordType->papFldDes[327]->size=sizeof(prec->nm11);
  pdbRecordType->papFldDes[327]->offset=(short)((char *)&prec->nm11 - (char *)prec);
  pdbRecordType->papFldDes[328]->size=sizeof(prec->nm12);
  pdbRecordType->papFldDes[328]->offset=(short)((char *)&prec->nm12 - (char *)prec);
  pdbRecordType->papFldDes[329]->size=sizeof(prec->nm13);
  pdbRecordType->papFldDes[329]->offset=(short)((char *)&prec->nm13 - (char *)prec);
  pdbRecordType->papFldDes[330]->size=sizeof(prec->nm14);
  pdbRecordType->papFldDes[330]->offset=(short)((char *)&prec->nm14 - (char *)prec);
  pdbRecordType->papFldDes[331]->size=sizeof(prec->nm15);
  pdbRecordType->papFldDes[331]->offset=(short)((char *)&prec->nm15 - (char *)prec);
  pdbRecordType->papFldDes[332]->size=sizeof(prec->nm16);
  pdbRecordType->papFldDes[332]->offset=(short)((char *)&prec->nm16 - (char *)prec);
  pdbRecordType->papFldDes[333]->size=sizeof(prec->nm17);
  pdbRecordType->papFldDes[333]->offset=(short)((char *)&prec->nm17 - (char *)prec);
  pdbRecordType->papFldDes[334]->size=sizeof(prec->nm18);
  pdbRecordType->papFldDes[334]->offset=(short)((char *)&prec->nm18 - (char *)prec);
  pdbRecordType->papFldDes[335]->size=sizeof(prec->nm19);
  pdbRecordType->papFldDes[335]->offset=(short)((char *)&prec->nm19 - (char *)prec);
  pdbRecordType->papFldDes[336]->size=sizeof(prec->nm20);
  pdbRecordType->papFldDes[336]->offset=(short)((char *)&prec->nm20 - (char *)prec);
  pdbRecordType->papFldDes[337]->size=sizeof(prec->nm21);
  pdbRecordType->papFldDes[337]->offset=(short)((char *)&prec->nm21 - (char *)prec);
  pdbRecordType->papFldDes[338]->size=sizeof(prec->nm22);
  pdbRecordType->papFldDes[338]->offset=(short)((char *)&prec->nm22 - (char *)prec);
  pdbRecordType->papFldDes[339]->size=sizeof(prec->nm23);
  pdbRecordType->papFldDes[339]->offset=(short)((char *)&prec->nm23 - (char *)prec);
  pdbRecordType->papFldDes[340]->size=sizeof(prec->nm24);
  pdbRecordType->papFldDes[340]->offset=(short)((char *)&prec->nm24 - (char *)prec);
  pdbRecordType->papFldDes[341]->size=sizeof(prec->nm25);
  pdbRecordType->papFldDes[341]->offset=(short)((char *)&prec->nm25 - (char *)prec);
  pdbRecordType->papFldDes[342]->size=sizeof(prec->nm26);
  pdbRecordType->papFldDes[342]->offset=(short)((char *)&prec->nm26 - (char *)prec);
  pdbRecordType->papFldDes[343]->size=sizeof(prec->nm27);
  pdbRecordType->papFldDes[343]->offset=(short)((char *)&prec->nm27 - (char *)prec);
  pdbRecordType->papFldDes[344]->size=sizeof(prec->nm28);
  pdbRecordType->papFldDes[344]->offset=(short)((char *)&prec->nm28 - (char *)prec);
  pdbRecordType->papFldDes[345]->size=sizeof(prec->nm29);
  pdbRecordType->papFldDes[345]->offset=(short)((char *)&prec->nm29 - (char *)prec);
  pdbRecordType->papFldDes[346]->size=sizeof(prec->nm30);
  pdbRecordType->papFldDes[346]->offset=(short)((char *)&prec->nm30 - (char *)prec);
  pdbRecordType->papFldDes[347]->size=sizeof(prec->nm31);
  pdbRecordType->papFldDes[347]->offset=(short)((char *)&prec->nm31 - (char *)prec);
  pdbRecordType->papFldDes[348]->size=sizeof(prec->nm32);
  pdbRecordType->papFldDes[348]->offset=(short)((char *)&prec->nm32 - (char *)prec);
  pdbRecordType->papFldDes[349]->size=sizeof(prec->nm33);
  pdbRecordType->papFldDes[349]->offset=(short)((char *)&prec->nm33 - (char *)prec);
  pdbRecordType->papFldDes[350]->size=sizeof(prec->nm34);
  pdbRecordType->papFldDes[350]->offset=(short)((char *)&prec->nm34 - (char *)prec);
  pdbRecordType->papFldDes[351]->size=sizeof(prec->nm35);
  pdbRecordType->papFldDes[351]->offset=(short)((char *)&prec->nm35 - (char *)prec);
  pdbRecordType->papFldDes[352]->size=sizeof(prec->nm36);
  pdbRecordType->papFldDes[352]->offset=(short)((char *)&prec->nm36 - (char *)prec);
  pdbRecordType->papFldDes[353]->size=sizeof(prec->nm37);
  pdbRecordType->papFldDes[353]->offset=(short)((char *)&prec->nm37 - (char *)prec);
  pdbRecordType->papFldDes[354]->size=sizeof(prec->nm38);
  pdbRecordType->papFldDes[354]->offset=(short)((char *)&prec->nm38 - (char *)prec);
  pdbRecordType->papFldDes[355]->size=sizeof(prec->nm39);
  pdbRecordType->papFldDes[355]->offset=(short)((char *)&prec->nm39 - (char *)prec);
  pdbRecordType->papFldDes[356]->size=sizeof(prec->nm40);
  pdbRecordType->papFldDes[356]->offset=(short)((char *)&prec->nm40 - (char *)prec);
  pdbRecordType->papFldDes[357]->size=sizeof(prec->nm41);
  pdbRecordType->papFldDes[357]->offset=(short)((char *)&prec->nm41 - (char *)prec);
  pdbRecordType->papFldDes[358]->size=sizeof(prec->nm42);
  pdbRecordType->papFldDes[358]->offset=(short)((char *)&prec->nm42 - (char *)prec);
  pdbRecordType->papFldDes[359]->size=sizeof(prec->nm43);
  pdbRecordType->papFldDes[359]->offset=(short)((char *)&prec->nm43 - (char *)prec);
  pdbRecordType->papFldDes[360]->size=sizeof(prec->nm44);
  pdbRecordType->papFldDes[360]->offset=(short)((char *)&prec->nm44 - (char *)prec);
  pdbRecordType->papFldDes[361]->size=sizeof(prec->nm45);
  pdbRecordType->papFldDes[361]->offset=(short)((char *)&prec->nm45 - (char *)prec);
  pdbRecordType->papFldDes[362]->size=sizeof(prec->nm46);
  pdbRecordType->papFldDes[362]->offset=(short)((char *)&prec->nm46 - (char *)prec);
  pdbRecordType->papFldDes[363]->size=sizeof(prec->nm47);
  pdbRecordType->papFldDes[363]->offset=(short)((char *)&prec->nm47 - (char *)prec);
  pdbRecordType->papFldDes[364]->size=sizeof(prec->nm48);
  pdbRecordType->papFldDes[364]->offset=(short)((char *)&prec->nm48 - (char *)prec);
  pdbRecordType->papFldDes[365]->size=sizeof(prec->nm49);
  pdbRecordType->papFldDes[365]->offset=(short)((char *)&prec->nm49 - (char *)prec);
  pdbRecordType->papFldDes[366]->size=sizeof(prec->nm50);
  pdbRecordType->papFldDes[366]->offset=(short)((char *)&prec->nm50 - (char *)prec);
  pdbRecordType->papFldDes[367]->size=sizeof(prec->nm51);
  pdbRecordType->papFldDes[367]->offset=(short)((char *)&prec->nm51 - (char *)prec);
  pdbRecordType->papFldDes[368]->size=sizeof(prec->nm52);
  pdbRecordType->papFldDes[368]->offset=(short)((char *)&prec->nm52 - (char *)prec);
  pdbRecordType->papFldDes[369]->size=sizeof(prec->nm53);
  pdbRecordType->papFldDes[369]->offset=(short)((char *)&prec->nm53 - (char *)prec);
  pdbRecordType->papFldDes[370]->size=sizeof(prec->nm54);
  pdbRecordType->papFldDes[370]->offset=(short)((char *)&prec->nm54 - (char *)prec);
  pdbRecordType->papFldDes[371]->size=sizeof(prec->nm55);
  pdbRecordType->papFldDes[371]->offset=(short)((char *)&prec->nm55 - (char *)prec);
  pdbRecordType->papFldDes[372]->size=sizeof(prec->nm56);
  pdbRecordType->papFldDes[372]->offset=(short)((char *)&prec->nm56 - (char *)prec);
  pdbRecordType->papFldDes[373]->size=sizeof(prec->nm57);
  pdbRecordType->papFldDes[373]->offset=(short)((char *)&prec->nm57 - (char *)prec);
  pdbRecordType->papFldDes[374]->size=sizeof(prec->nm58);
  pdbRecordType->papFldDes[374]->offset=(short)((char *)&prec->nm58 - (char *)prec);
  pdbRecordType->papFldDes[375]->size=sizeof(prec->nm59);
  pdbRecordType->papFldDes[375]->offset=(short)((char *)&prec->nm59 - (char *)prec);
  pdbRecordType->papFldDes[376]->size=sizeof(prec->nm60);
  pdbRecordType->papFldDes[376]->offset=(short)((char *)&prec->nm60 - (char *)prec);
  pdbRecordType->papFldDes[377]->size=sizeof(prec->nm61);
  pdbRecordType->papFldDes[377]->offset=(short)((char *)&prec->nm61 - (char *)prec);
  pdbRecordType->papFldDes[378]->size=sizeof(prec->nm62);
  pdbRecordType->papFldDes[378]->offset=(short)((char *)&prec->nm62 - (char *)prec);
  pdbRecordType->papFldDes[379]->size=sizeof(prec->nm63);
  pdbRecordType->papFldDes[379]->offset=(short)((char *)&prec->nm63 - (char *)prec);
  pdbRecordType->papFldDes[380]->size=sizeof(prec->nm64);
  pdbRecordType->papFldDes[380]->offset=(short)((char *)&prec->nm64 - (char *)prec);
  pdbRecordType->papFldDes[381]->size=sizeof(prec->egu);
  pdbRecordType->papFldDes[381]->offset=(short)((char *)&prec->egu - (char *)prec);
  pdbRecordType->papFldDes[382]->size=sizeof(prec->prec);
  pdbRecordType->papFldDes[382]->offset=(short)((char *)&prec->prec - (char *)prec);
  pdbRecordType->papFldDes[383]->size=sizeof(prec->cout);
  pdbRecordType->papFldDes[383]->offset=(short)((char *)&prec->cout - (char *)prec);
  pdbRecordType->papFldDes[384]->size=sizeof(prec->coutp);
  pdbRecordType->papFldDes[384]->offset=(short)((char *)&prec->coutp - (char *)prec);
    pdbRecordType->rec_size = sizeof(*prec);
    return(0);
}
epicsExportRegistrar(scalerRecordSizeOffset);
#ifdef __cplusplus
}
#endif
#endif /*GEN_SIZE_OFFSET*/
