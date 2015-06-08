
#ifndef INCmotorUEIPH
#define INCmotorUEIPH
typedef enum {
	motorUEIP_No,
	motorUEIP_Yes
}motorUEIP;
#endif /*INCmotorUEIPH*/

#ifndef INCmotorTORQH
#define INCmotorTORQH
typedef enum {
	motorTORQ_Disable,
	motorTORQ_Enable
}motorTORQ;
#endif /*INCmotorTORQH*/

#ifndef INCmotorSTUPH
#define INCmotorSTUPH
typedef enum {
	motorSTUP_OFF,
	motorSTUP_ON,
	motorSTUP_BUSY
}motorSTUP;
#endif /*INCmotorSTUPH*/

#ifndef INCmotorSPMGH
#define INCmotorSPMGH
typedef enum {
	motorSPMG_Stop,
	motorSPMG_Pause,
	motorSPMG_Move,
	motorSPMG_Go
}motorSPMG;
#endif /*INCmotorSPMGH*/

#ifndef INCmotorSETH
#define INCmotorSETH
typedef enum {
	motorSET_Use,
	motorSET_Set
}motorSET;
#endif /*INCmotorSETH*/

#ifndef INCmotorRMODH
#define INCmotorRMODH
typedef enum {
	motorRMOD_D,
	motorRMOD_A,
	motorRMOD_G
}motorRMOD;
#endif /*INCmotorRMODH*/

#ifndef INCmotorMODEH
#define INCmotorMODEH
typedef enum {
	motorMODE_Position,
	motorMODE_Velocity
}motorMODE;
#endif /*INCmotorMODEH*/

#ifndef INCmotorFOFFH
#define INCmotorFOFFH
typedef enum {
	motorFOFF_Variable,
	motorFOFF_Frozen
}motorFOFF;
#endif /*INCmotorFOFFH*/

#ifndef INCmotorDIRH
#define INCmotorDIRH
typedef enum {
	motorDIR_Pos,
	motorDIR_Neg
}motorDIR;
#endif /*INCmotorDIRH*/

#ifndef INCmenuYesNoH
#define INCmenuYesNoH
typedef enum {
	menuYesNoNO,
	menuYesNoYES
}menuYesNo;
#endif /*INCmenuYesNoH*/

#ifndef INCmenuOmslH
#define INCmenuOmslH
typedef enum {
	menuOmslsupervisory,
	menuOmslclosed_loop
}menuOmsl;
#endif /*INCmenuOmslH*/
#ifndef INCmotorH
#define INCmotorH
#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"
typedef struct motorRecord {
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
	epicsFloat64	off;	/* User Offset (EGU) */
	epicsEnum16	foff;	/* Offset-Freeze Switch */
	epicsInt16	fof;	/* Freeze Offset */
	epicsInt16	vof;	/* Variable Offset */
	epicsEnum16	dir;	/* User Direction */
	epicsEnum16	set;	/* Set/Use Switch */
	epicsInt16	sset;	/* Set SET Mode */
	epicsInt16	suse;	/* Set USE Mode */
	epicsFloat64	velo;	/* Velocity (EGU/s) */
	epicsFloat64	vbas;	/* Base Velocity (EGU/s) */
	epicsFloat64	vmax;	/* Max. Velocity (EGU/s) */
	epicsFloat64	s;	/* Speed (revolutions/sec) */
	epicsFloat64	sbas;	/* Base Speed (RPS) */
	epicsFloat64	smax;	/* Max. Speed (RPS) */
	epicsFloat64	accl;	/* Seconds to Velocity */
	epicsFloat64	bdst;	/* BL Distance (EGU) */
	epicsFloat64	bvel;	/* BL Velocity (EGU/s) */
	epicsFloat64	sbak;	/* BL Speed (RPS) */
	epicsFloat64	bacc;	/* BL Seconds to Velocity */
	epicsFloat32	frac;	/* Move Fraction */
	DBLINK		out;	/* Output Specification */
	epicsInt16	card;	/* Card Number */
	DBLINK		rdbl;	/* Readback Location */
	DBLINK		dol;	/* Desired Output Loc */
	epicsEnum16	omsl;	/* Output Mode Select */
	DBLINK		rlnk;	/* Readback OutLink */
	epicsInt32	srev;	/* Steps per Revolution */
	epicsFloat64	urev;	/* EGU's per Revolution */
	epicsFloat64	mres;	/* Motor Step Size (EGU) */
	epicsFloat64	eres;	/* Encoder Step Size (EGU) */
	epicsFloat64	rres;	/* Readback Step Size (EGU */
	epicsEnum16	ueip;	/* Use Encoder If Present */
	epicsEnum16	urip;	/* Use RDBL Link If Presen */
	epicsInt16	prec;	/* Display Precision */
	char		egu[16];	/* Engineering Units */
	epicsFloat64	hlm;	/* User High Limit */
	epicsFloat64	llm;	/* User Low Limit */
	epicsFloat64	dhlm;	/* Dial High Limit */
	epicsFloat64	dllm;	/* Dial Low Limit */
	epicsFloat64	hopr;	/* High Operating Range */
	epicsFloat64	lopr;	/* Low Operating Range */
	epicsInt16	hls;	/* User High Limit Switch */
	epicsInt16	lls;	/* User Low Limit Switch */
	epicsInt16	rhls;	/* Raw High Limit Switch */
	epicsInt16	rlls;	/* Raw Low Limit Switch */
	epicsFloat64	hihi;	/* Hihi Alarm Limit (EGU) */
	epicsFloat64	lolo;	/* Lolo Alarm Limit (EGU) */
	epicsFloat64	high;	/* High Alarm Limit (EGU) */
	epicsFloat64	low;	/* Low Alarm Limit (EGU) */
	epicsEnum16	hhsv;	/* Hihi Severity */
	epicsEnum16	llsv;	/* Lolo Severity */
	epicsEnum16	hsv;	/* High Severity */
	epicsEnum16	lsv;	/* Low Severity */
	epicsEnum16	hlsv;	/* HW Limit Violation Svr */
	epicsFloat64	rdbd;	/* Retry Deadband (EGU) */
	epicsInt16	rcnt;	/* Retry count */
	epicsInt16	rtry;	/* Max retry count */
	epicsInt16	miss;	/* Ran out of retries */
	epicsEnum16	spmg;	/* Stop/Pause/Move/Go */
	epicsEnum16	lspg;	/* Last SPMG */
	epicsInt16	stop;	/* Stop */
	epicsInt16	homf;	/* Home Forward */
	epicsInt16	homr;	/* Home Reverse */
	epicsInt16	jogf;	/* Jog motor Forward */
	epicsInt16	jogr;	/* Jog motor Reverse */
	epicsInt16	twf;	/* Tweak motor Forward */
	epicsInt16	twr;	/* Tweak motor Reverse */
	epicsFloat64	twv;	/* Tweak Step Size (EGU) */
	epicsFloat64	val;	/* User Desired Value (EGU */
	epicsFloat64	lval;	/* Last User Des Val (EGU) */
	epicsFloat64	dval;	/* Dial Desired Value (EGU */
	epicsFloat64	ldvl;	/* Last Dial Des Val (EGU) */
	epicsInt32	rval;	/* Raw Desired Value (step */
	epicsInt32	lrvl;	/* Last Raw Des Val (steps */
	epicsFloat64	rlv;	/* Relative Value (EGU) */
	epicsFloat64	lrlv;	/* Last Rel Value (EGU) */
	epicsFloat64	rbv;	/* User Readback Value */
	epicsFloat64	drbv;	/* Dial Readback Value */
	epicsFloat64	diff;	/* Difference dval-drbv */
	epicsInt32	rdif;	/* Difference rval-rrbv */
	epicsInt16	cdir;	/* Raw cmnd direction */
	epicsInt32	rrbv;	/* Raw Readback Value */
	epicsInt32	rmp;	/* Raw Motor Position */
	epicsInt32	rep;	/* Raw Encoder Position */
	epicsInt32	rvel;	/* Raw Velocity */
	epicsInt16	dmov;	/* Done moving to value */
	epicsInt16	movn;	/* Motor is moving */
	epicsUInt32	msta;	/* Motor Status */
	epicsInt16	lvio;	/* Limit violation */
	epicsInt16	tdir;	/* Direction of Travel */
	epicsInt16	athm;	/* At HOME */
	epicsInt16	pp;	/* Post process command */
	epicsUInt16	mip;	/* Motion In Progress */
	epicsUInt32	mmap;	/* Monitor Mask */
	epicsUInt32	nmap;	/* Monitor Mask (more) */
	epicsFloat64	dly;	/* Readback settle time (s) */
	void             *cbak;	/* Callback structure */
	epicsFloat64	pcof;	/* Proportional Gain */
	epicsFloat64	icof;	/* Integral Gain */
	epicsFloat64	dcof;	/* Derivative Gain */
	epicsEnum16	cnen;	/* Enable control */
	char		init[40];	/* Startup commands */
	char		prem[40];	/* Pre-move commands */
	char		post[40];	/* Post-move commands */
	DBLINK		stoo;	/* STOP OutLink */
	DBLINK		dinp;	/* DMOV Input Link */
	DBLINK		rinp;	/* RMP Input Link */
	epicsFloat64	jvel;	/* Jog Velocity (EGU/s) */
	epicsFloat64	jar;	/* Jog Accel. (EGU/s^2) */
	epicsEnum16	lock;	/* Soft Channel Position Lock */
	epicsEnum16	ntm;	/* New Target Monitor */
	epicsUInt16	ntmf;	/* NTM Deadband Factor */
	epicsFloat64	hvel;	/* Home Velocity (EGU/s) */
	epicsEnum16	stup;	/* Status Update */
	epicsEnum16	rmod;	/* Retry Mode */
	epicsFloat64	adel;	/* Archive Deadband */
	epicsFloat64	mdel;	/* Monitor Deadband */
	epicsFloat64	alst;	/* Last Value Archived */
	epicsFloat64	mlst;	/* Last Val Monitored */
	epicsInt16	sync;	/* Sync position */
} motorRecord;
#define motorRecordNAME	0
#define motorRecordDESC	1
#define motorRecordASG	2
#define motorRecordSCAN	3
#define motorRecordPINI	4
#define motorRecordPHAS	5
#define motorRecordEVNT	6
#define motorRecordTSE	7
#define motorRecordTSEL	8
#define motorRecordDTYP	9
#define motorRecordDISV	10
#define motorRecordDISA	11
#define motorRecordSDIS	12
#define motorRecordMLOK	13
#define motorRecordMLIS	14
#define motorRecordDISP	15
#define motorRecordPROC	16
#define motorRecordSTAT	17
#define motorRecordSEVR	18
#define motorRecordNSTA	19
#define motorRecordNSEV	20
#define motorRecordACKS	21
#define motorRecordACKT	22
#define motorRecordDISS	23
#define motorRecordLCNT	24
#define motorRecordPACT	25
#define motorRecordPUTF	26
#define motorRecordRPRO	27
#define motorRecordASP	28
#define motorRecordPPN	29
#define motorRecordPPNR	30
#define motorRecordSPVT	31
#define motorRecordRSET	32
#define motorRecordDSET	33
#define motorRecordDPVT	34
#define motorRecordRDES	35
#define motorRecordLSET	36
#define motorRecordPRIO	37
#define motorRecordTPRO	38
#define motorRecordBKPT	39
#define motorRecordUDF	40
#define motorRecordTIME	41
#define motorRecordFLNK	42
#define motorRecordVERS	43
#define motorRecordOFF	44
#define motorRecordFOFF	45
#define motorRecordFOF	46
#define motorRecordVOF	47
#define motorRecordDIR	48
#define motorRecordSET	49
#define motorRecordSSET	50
#define motorRecordSUSE	51
#define motorRecordVELO	52
#define motorRecordVBAS	53
#define motorRecordVMAX	54
#define motorRecordS	55
#define motorRecordSBAS	56
#define motorRecordSMAX	57
#define motorRecordACCL	58
#define motorRecordBDST	59
#define motorRecordBVEL	60
#define motorRecordSBAK	61
#define motorRecordBACC	62
#define motorRecordFRAC	63
#define motorRecordOUT	64
#define motorRecordCARD	65
#define motorRecordRDBL	66
#define motorRecordDOL	67
#define motorRecordOMSL	68
#define motorRecordRLNK	69
#define motorRecordSREV	70
#define motorRecordUREV	71
#define motorRecordMRES	72
#define motorRecordERES	73
#define motorRecordRRES	74
#define motorRecordUEIP	75
#define motorRecordURIP	76
#define motorRecordPREC	77
#define motorRecordEGU	78
#define motorRecordHLM	79
#define motorRecordLLM	80
#define motorRecordDHLM	81
#define motorRecordDLLM	82
#define motorRecordHOPR	83
#define motorRecordLOPR	84
#define motorRecordHLS	85
#define motorRecordLLS	86
#define motorRecordRHLS	87
#define motorRecordRLLS	88
#define motorRecordHIHI	89
#define motorRecordLOLO	90
#define motorRecordHIGH	91
#define motorRecordLOW	92
#define motorRecordHHSV	93
#define motorRecordLLSV	94
#define motorRecordHSV	95
#define motorRecordLSV	96
#define motorRecordHLSV	97
#define motorRecordRDBD	98
#define motorRecordRCNT	99
#define motorRecordRTRY	100
#define motorRecordMISS	101
#define motorRecordSPMG	102
#define motorRecordLSPG	103
#define motorRecordSTOP	104
#define motorRecordHOMF	105
#define motorRecordHOMR	106
#define motorRecordJOGF	107
#define motorRecordJOGR	108
#define motorRecordTWF	109
#define motorRecordTWR	110
#define motorRecordTWV	111
#define motorRecordVAL	112
#define motorRecordLVAL	113
#define motorRecordDVAL	114
#define motorRecordLDVL	115
#define motorRecordRVAL	116
#define motorRecordLRVL	117
#define motorRecordRLV	118
#define motorRecordLRLV	119
#define motorRecordRBV	120
#define motorRecordDRBV	121
#define motorRecordDIFF	122
#define motorRecordRDIF	123
#define motorRecordCDIR	124
#define motorRecordRRBV	125
#define motorRecordRMP	126
#define motorRecordREP	127
#define motorRecordRVEL	128
#define motorRecordDMOV	129
#define motorRecordMOVN	130
#define motorRecordMSTA	131
#define motorRecordLVIO	132
#define motorRecordTDIR	133
#define motorRecordATHM	134
#define motorRecordPP	135
#define motorRecordMIP	136
#define motorRecordMMAP	137
#define motorRecordNMAP	138
#define motorRecordDLY	139
#define motorRecordCBAK	140
#define motorRecordPCOF	141
#define motorRecordICOF	142
#define motorRecordDCOF	143
#define motorRecordCNEN	144
#define motorRecordINIT	145
#define motorRecordPREM	146
#define motorRecordPOST	147
#define motorRecordSTOO	148
#define motorRecordDINP	149
#define motorRecordRINP	150
#define motorRecordJVEL	151
#define motorRecordJAR	152
#define motorRecordLOCK	153
#define motorRecordNTM	154
#define motorRecordNTMF	155
#define motorRecordHVEL	156
#define motorRecordSTUP	157
#define motorRecordRMOD	158
#define motorRecordADEL	159
#define motorRecordMDEL	160
#define motorRecordALST	161
#define motorRecordMLST	162
#define motorRecordSYNC	163
#endif /*INCmotorH*/
#ifdef GEN_SIZE_OFFSET
#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int motorRecordSizeOffset(dbRecordType *pdbRecordType)
{
    motorRecord *prec = 0;
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
  pdbRecordType->papFldDes[44]->size=sizeof(prec->off);
  pdbRecordType->papFldDes[44]->offset=(short)((char *)&prec->off - (char *)prec);
  pdbRecordType->papFldDes[45]->size=sizeof(prec->foff);
  pdbRecordType->papFldDes[45]->offset=(short)((char *)&prec->foff - (char *)prec);
  pdbRecordType->papFldDes[46]->size=sizeof(prec->fof);
  pdbRecordType->papFldDes[46]->offset=(short)((char *)&prec->fof - (char *)prec);
  pdbRecordType->papFldDes[47]->size=sizeof(prec->vof);
  pdbRecordType->papFldDes[47]->offset=(short)((char *)&prec->vof - (char *)prec);
  pdbRecordType->papFldDes[48]->size=sizeof(prec->dir);
  pdbRecordType->papFldDes[48]->offset=(short)((char *)&prec->dir - (char *)prec);
  pdbRecordType->papFldDes[49]->size=sizeof(prec->set);
  pdbRecordType->papFldDes[49]->offset=(short)((char *)&prec->set - (char *)prec);
  pdbRecordType->papFldDes[50]->size=sizeof(prec->sset);
  pdbRecordType->papFldDes[50]->offset=(short)((char *)&prec->sset - (char *)prec);
  pdbRecordType->papFldDes[51]->size=sizeof(prec->suse);
  pdbRecordType->papFldDes[51]->offset=(short)((char *)&prec->suse - (char *)prec);
  pdbRecordType->papFldDes[52]->size=sizeof(prec->velo);
  pdbRecordType->papFldDes[52]->offset=(short)((char *)&prec->velo - (char *)prec);
  pdbRecordType->papFldDes[53]->size=sizeof(prec->vbas);
  pdbRecordType->papFldDes[53]->offset=(short)((char *)&prec->vbas - (char *)prec);
  pdbRecordType->papFldDes[54]->size=sizeof(prec->vmax);
  pdbRecordType->papFldDes[54]->offset=(short)((char *)&prec->vmax - (char *)prec);
  pdbRecordType->papFldDes[55]->size=sizeof(prec->s);
  pdbRecordType->papFldDes[55]->offset=(short)((char *)&prec->s - (char *)prec);
  pdbRecordType->papFldDes[56]->size=sizeof(prec->sbas);
  pdbRecordType->papFldDes[56]->offset=(short)((char *)&prec->sbas - (char *)prec);
  pdbRecordType->papFldDes[57]->size=sizeof(prec->smax);
  pdbRecordType->papFldDes[57]->offset=(short)((char *)&prec->smax - (char *)prec);
  pdbRecordType->papFldDes[58]->size=sizeof(prec->accl);
  pdbRecordType->papFldDes[58]->offset=(short)((char *)&prec->accl - (char *)prec);
  pdbRecordType->papFldDes[59]->size=sizeof(prec->bdst);
  pdbRecordType->papFldDes[59]->offset=(short)((char *)&prec->bdst - (char *)prec);
  pdbRecordType->papFldDes[60]->size=sizeof(prec->bvel);
  pdbRecordType->papFldDes[60]->offset=(short)((char *)&prec->bvel - (char *)prec);
  pdbRecordType->papFldDes[61]->size=sizeof(prec->sbak);
  pdbRecordType->papFldDes[61]->offset=(short)((char *)&prec->sbak - (char *)prec);
  pdbRecordType->papFldDes[62]->size=sizeof(prec->bacc);
  pdbRecordType->papFldDes[62]->offset=(short)((char *)&prec->bacc - (char *)prec);
  pdbRecordType->papFldDes[63]->size=sizeof(prec->frac);
  pdbRecordType->papFldDes[63]->offset=(short)((char *)&prec->frac - (char *)prec);
  pdbRecordType->papFldDes[64]->size=sizeof(prec->out);
  pdbRecordType->papFldDes[64]->offset=(short)((char *)&prec->out - (char *)prec);
  pdbRecordType->papFldDes[65]->size=sizeof(prec->card);
  pdbRecordType->papFldDes[65]->offset=(short)((char *)&prec->card - (char *)prec);
  pdbRecordType->papFldDes[66]->size=sizeof(prec->rdbl);
  pdbRecordType->papFldDes[66]->offset=(short)((char *)&prec->rdbl - (char *)prec);
  pdbRecordType->papFldDes[67]->size=sizeof(prec->dol);
  pdbRecordType->papFldDes[67]->offset=(short)((char *)&prec->dol - (char *)prec);
  pdbRecordType->papFldDes[68]->size=sizeof(prec->omsl);
  pdbRecordType->papFldDes[68]->offset=(short)((char *)&prec->omsl - (char *)prec);
  pdbRecordType->papFldDes[69]->size=sizeof(prec->rlnk);
  pdbRecordType->papFldDes[69]->offset=(short)((char *)&prec->rlnk - (char *)prec);
  pdbRecordType->papFldDes[70]->size=sizeof(prec->srev);
  pdbRecordType->papFldDes[70]->offset=(short)((char *)&prec->srev - (char *)prec);
  pdbRecordType->papFldDes[71]->size=sizeof(prec->urev);
  pdbRecordType->papFldDes[71]->offset=(short)((char *)&prec->urev - (char *)prec);
  pdbRecordType->papFldDes[72]->size=sizeof(prec->mres);
  pdbRecordType->papFldDes[72]->offset=(short)((char *)&prec->mres - (char *)prec);
  pdbRecordType->papFldDes[73]->size=sizeof(prec->eres);
  pdbRecordType->papFldDes[73]->offset=(short)((char *)&prec->eres - (char *)prec);
  pdbRecordType->papFldDes[74]->size=sizeof(prec->rres);
  pdbRecordType->papFldDes[74]->offset=(short)((char *)&prec->rres - (char *)prec);
  pdbRecordType->papFldDes[75]->size=sizeof(prec->ueip);
  pdbRecordType->papFldDes[75]->offset=(short)((char *)&prec->ueip - (char *)prec);
  pdbRecordType->papFldDes[76]->size=sizeof(prec->urip);
  pdbRecordType->papFldDes[76]->offset=(short)((char *)&prec->urip - (char *)prec);
  pdbRecordType->papFldDes[77]->size=sizeof(prec->prec);
  pdbRecordType->papFldDes[77]->offset=(short)((char *)&prec->prec - (char *)prec);
  pdbRecordType->papFldDes[78]->size=sizeof(prec->egu);
  pdbRecordType->papFldDes[78]->offset=(short)((char *)&prec->egu - (char *)prec);
  pdbRecordType->papFldDes[79]->size=sizeof(prec->hlm);
  pdbRecordType->papFldDes[79]->offset=(short)((char *)&prec->hlm - (char *)prec);
  pdbRecordType->papFldDes[80]->size=sizeof(prec->llm);
  pdbRecordType->papFldDes[80]->offset=(short)((char *)&prec->llm - (char *)prec);
  pdbRecordType->papFldDes[81]->size=sizeof(prec->dhlm);
  pdbRecordType->papFldDes[81]->offset=(short)((char *)&prec->dhlm - (char *)prec);
  pdbRecordType->papFldDes[82]->size=sizeof(prec->dllm);
  pdbRecordType->papFldDes[82]->offset=(short)((char *)&prec->dllm - (char *)prec);
  pdbRecordType->papFldDes[83]->size=sizeof(prec->hopr);
  pdbRecordType->papFldDes[83]->offset=(short)((char *)&prec->hopr - (char *)prec);
  pdbRecordType->papFldDes[84]->size=sizeof(prec->lopr);
  pdbRecordType->papFldDes[84]->offset=(short)((char *)&prec->lopr - (char *)prec);
  pdbRecordType->papFldDes[85]->size=sizeof(prec->hls);
  pdbRecordType->papFldDes[85]->offset=(short)((char *)&prec->hls - (char *)prec);
  pdbRecordType->papFldDes[86]->size=sizeof(prec->lls);
  pdbRecordType->papFldDes[86]->offset=(short)((char *)&prec->lls - (char *)prec);
  pdbRecordType->papFldDes[87]->size=sizeof(prec->rhls);
  pdbRecordType->papFldDes[87]->offset=(short)((char *)&prec->rhls - (char *)prec);
  pdbRecordType->papFldDes[88]->size=sizeof(prec->rlls);
  pdbRecordType->papFldDes[88]->offset=(short)((char *)&prec->rlls - (char *)prec);
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
  pdbRecordType->papFldDes[97]->size=sizeof(prec->hlsv);
  pdbRecordType->papFldDes[97]->offset=(short)((char *)&prec->hlsv - (char *)prec);
  pdbRecordType->papFldDes[98]->size=sizeof(prec->rdbd);
  pdbRecordType->papFldDes[98]->offset=(short)((char *)&prec->rdbd - (char *)prec);
  pdbRecordType->papFldDes[99]->size=sizeof(prec->rcnt);
  pdbRecordType->papFldDes[99]->offset=(short)((char *)&prec->rcnt - (char *)prec);
  pdbRecordType->papFldDes[100]->size=sizeof(prec->rtry);
  pdbRecordType->papFldDes[100]->offset=(short)((char *)&prec->rtry - (char *)prec);
  pdbRecordType->papFldDes[101]->size=sizeof(prec->miss);
  pdbRecordType->papFldDes[101]->offset=(short)((char *)&prec->miss - (char *)prec);
  pdbRecordType->papFldDes[102]->size=sizeof(prec->spmg);
  pdbRecordType->papFldDes[102]->offset=(short)((char *)&prec->spmg - (char *)prec);
  pdbRecordType->papFldDes[103]->size=sizeof(prec->lspg);
  pdbRecordType->papFldDes[103]->offset=(short)((char *)&prec->lspg - (char *)prec);
  pdbRecordType->papFldDes[104]->size=sizeof(prec->stop);
  pdbRecordType->papFldDes[104]->offset=(short)((char *)&prec->stop - (char *)prec);
  pdbRecordType->papFldDes[105]->size=sizeof(prec->homf);
  pdbRecordType->papFldDes[105]->offset=(short)((char *)&prec->homf - (char *)prec);
  pdbRecordType->papFldDes[106]->size=sizeof(prec->homr);
  pdbRecordType->papFldDes[106]->offset=(short)((char *)&prec->homr - (char *)prec);
  pdbRecordType->papFldDes[107]->size=sizeof(prec->jogf);
  pdbRecordType->papFldDes[107]->offset=(short)((char *)&prec->jogf - (char *)prec);
  pdbRecordType->papFldDes[108]->size=sizeof(prec->jogr);
  pdbRecordType->papFldDes[108]->offset=(short)((char *)&prec->jogr - (char *)prec);
  pdbRecordType->papFldDes[109]->size=sizeof(prec->twf);
  pdbRecordType->papFldDes[109]->offset=(short)((char *)&prec->twf - (char *)prec);
  pdbRecordType->papFldDes[110]->size=sizeof(prec->twr);
  pdbRecordType->papFldDes[110]->offset=(short)((char *)&prec->twr - (char *)prec);
  pdbRecordType->papFldDes[111]->size=sizeof(prec->twv);
  pdbRecordType->papFldDes[111]->offset=(short)((char *)&prec->twv - (char *)prec);
  pdbRecordType->papFldDes[112]->size=sizeof(prec->val);
  pdbRecordType->papFldDes[112]->offset=(short)((char *)&prec->val - (char *)prec);
  pdbRecordType->papFldDes[113]->size=sizeof(prec->lval);
  pdbRecordType->papFldDes[113]->offset=(short)((char *)&prec->lval - (char *)prec);
  pdbRecordType->papFldDes[114]->size=sizeof(prec->dval);
  pdbRecordType->papFldDes[114]->offset=(short)((char *)&prec->dval - (char *)prec);
  pdbRecordType->papFldDes[115]->size=sizeof(prec->ldvl);
  pdbRecordType->papFldDes[115]->offset=(short)((char *)&prec->ldvl - (char *)prec);
  pdbRecordType->papFldDes[116]->size=sizeof(prec->rval);
  pdbRecordType->papFldDes[116]->offset=(short)((char *)&prec->rval - (char *)prec);
  pdbRecordType->papFldDes[117]->size=sizeof(prec->lrvl);
  pdbRecordType->papFldDes[117]->offset=(short)((char *)&prec->lrvl - (char *)prec);
  pdbRecordType->papFldDes[118]->size=sizeof(prec->rlv);
  pdbRecordType->papFldDes[118]->offset=(short)((char *)&prec->rlv - (char *)prec);
  pdbRecordType->papFldDes[119]->size=sizeof(prec->lrlv);
  pdbRecordType->papFldDes[119]->offset=(short)((char *)&prec->lrlv - (char *)prec);
  pdbRecordType->papFldDes[120]->size=sizeof(prec->rbv);
  pdbRecordType->papFldDes[120]->offset=(short)((char *)&prec->rbv - (char *)prec);
  pdbRecordType->papFldDes[121]->size=sizeof(prec->drbv);
  pdbRecordType->papFldDes[121]->offset=(short)((char *)&prec->drbv - (char *)prec);
  pdbRecordType->papFldDes[122]->size=sizeof(prec->diff);
  pdbRecordType->papFldDes[122]->offset=(short)((char *)&prec->diff - (char *)prec);
  pdbRecordType->papFldDes[123]->size=sizeof(prec->rdif);
  pdbRecordType->papFldDes[123]->offset=(short)((char *)&prec->rdif - (char *)prec);
  pdbRecordType->papFldDes[124]->size=sizeof(prec->cdir);
  pdbRecordType->papFldDes[124]->offset=(short)((char *)&prec->cdir - (char *)prec);
  pdbRecordType->papFldDes[125]->size=sizeof(prec->rrbv);
  pdbRecordType->papFldDes[125]->offset=(short)((char *)&prec->rrbv - (char *)prec);
  pdbRecordType->papFldDes[126]->size=sizeof(prec->rmp);
  pdbRecordType->papFldDes[126]->offset=(short)((char *)&prec->rmp - (char *)prec);
  pdbRecordType->papFldDes[127]->size=sizeof(prec->rep);
  pdbRecordType->papFldDes[127]->offset=(short)((char *)&prec->rep - (char *)prec);
  pdbRecordType->papFldDes[128]->size=sizeof(prec->rvel);
  pdbRecordType->papFldDes[128]->offset=(short)((char *)&prec->rvel - (char *)prec);
  pdbRecordType->papFldDes[129]->size=sizeof(prec->dmov);
  pdbRecordType->papFldDes[129]->offset=(short)((char *)&prec->dmov - (char *)prec);
  pdbRecordType->papFldDes[130]->size=sizeof(prec->movn);
  pdbRecordType->papFldDes[130]->offset=(short)((char *)&prec->movn - (char *)prec);
  pdbRecordType->papFldDes[131]->size=sizeof(prec->msta);
  pdbRecordType->papFldDes[131]->offset=(short)((char *)&prec->msta - (char *)prec);
  pdbRecordType->papFldDes[132]->size=sizeof(prec->lvio);
  pdbRecordType->papFldDes[132]->offset=(short)((char *)&prec->lvio - (char *)prec);
  pdbRecordType->papFldDes[133]->size=sizeof(prec->tdir);
  pdbRecordType->papFldDes[133]->offset=(short)((char *)&prec->tdir - (char *)prec);
  pdbRecordType->papFldDes[134]->size=sizeof(prec->athm);
  pdbRecordType->papFldDes[134]->offset=(short)((char *)&prec->athm - (char *)prec);
  pdbRecordType->papFldDes[135]->size=sizeof(prec->pp);
  pdbRecordType->papFldDes[135]->offset=(short)((char *)&prec->pp - (char *)prec);
  pdbRecordType->papFldDes[136]->size=sizeof(prec->mip);
  pdbRecordType->papFldDes[136]->offset=(short)((char *)&prec->mip - (char *)prec);
  pdbRecordType->papFldDes[137]->size=sizeof(prec->mmap);
  pdbRecordType->papFldDes[137]->offset=(short)((char *)&prec->mmap - (char *)prec);
  pdbRecordType->papFldDes[138]->size=sizeof(prec->nmap);
  pdbRecordType->papFldDes[138]->offset=(short)((char *)&prec->nmap - (char *)prec);
  pdbRecordType->papFldDes[139]->size=sizeof(prec->dly);
  pdbRecordType->papFldDes[139]->offset=(short)((char *)&prec->dly - (char *)prec);
  pdbRecordType->papFldDes[140]->size=sizeof(prec->cbak);
  pdbRecordType->papFldDes[140]->offset=(short)((char *)&prec->cbak - (char *)prec);
  pdbRecordType->papFldDes[141]->size=sizeof(prec->pcof);
  pdbRecordType->papFldDes[141]->offset=(short)((char *)&prec->pcof - (char *)prec);
  pdbRecordType->papFldDes[142]->size=sizeof(prec->icof);
  pdbRecordType->papFldDes[142]->offset=(short)((char *)&prec->icof - (char *)prec);
  pdbRecordType->papFldDes[143]->size=sizeof(prec->dcof);
  pdbRecordType->papFldDes[143]->offset=(short)((char *)&prec->dcof - (char *)prec);
  pdbRecordType->papFldDes[144]->size=sizeof(prec->cnen);
  pdbRecordType->papFldDes[144]->offset=(short)((char *)&prec->cnen - (char *)prec);
  pdbRecordType->papFldDes[145]->size=sizeof(prec->init);
  pdbRecordType->papFldDes[145]->offset=(short)((char *)&prec->init - (char *)prec);
  pdbRecordType->papFldDes[146]->size=sizeof(prec->prem);
  pdbRecordType->papFldDes[146]->offset=(short)((char *)&prec->prem - (char *)prec);
  pdbRecordType->papFldDes[147]->size=sizeof(prec->post);
  pdbRecordType->papFldDes[147]->offset=(short)((char *)&prec->post - (char *)prec);
  pdbRecordType->papFldDes[148]->size=sizeof(prec->stoo);
  pdbRecordType->papFldDes[148]->offset=(short)((char *)&prec->stoo - (char *)prec);
  pdbRecordType->papFldDes[149]->size=sizeof(prec->dinp);
  pdbRecordType->papFldDes[149]->offset=(short)((char *)&prec->dinp - (char *)prec);
  pdbRecordType->papFldDes[150]->size=sizeof(prec->rinp);
  pdbRecordType->papFldDes[150]->offset=(short)((char *)&prec->rinp - (char *)prec);
  pdbRecordType->papFldDes[151]->size=sizeof(prec->jvel);
  pdbRecordType->papFldDes[151]->offset=(short)((char *)&prec->jvel - (char *)prec);
  pdbRecordType->papFldDes[152]->size=sizeof(prec->jar);
  pdbRecordType->papFldDes[152]->offset=(short)((char *)&prec->jar - (char *)prec);
  pdbRecordType->papFldDes[153]->size=sizeof(prec->lock);
  pdbRecordType->papFldDes[153]->offset=(short)((char *)&prec->lock - (char *)prec);
  pdbRecordType->papFldDes[154]->size=sizeof(prec->ntm);
  pdbRecordType->papFldDes[154]->offset=(short)((char *)&prec->ntm - (char *)prec);
  pdbRecordType->papFldDes[155]->size=sizeof(prec->ntmf);
  pdbRecordType->papFldDes[155]->offset=(short)((char *)&prec->ntmf - (char *)prec);
  pdbRecordType->papFldDes[156]->size=sizeof(prec->hvel);
  pdbRecordType->papFldDes[156]->offset=(short)((char *)&prec->hvel - (char *)prec);
  pdbRecordType->papFldDes[157]->size=sizeof(prec->stup);
  pdbRecordType->papFldDes[157]->offset=(short)((char *)&prec->stup - (char *)prec);
  pdbRecordType->papFldDes[158]->size=sizeof(prec->rmod);
  pdbRecordType->papFldDes[158]->offset=(short)((char *)&prec->rmod - (char *)prec);
  pdbRecordType->papFldDes[159]->size=sizeof(prec->adel);
  pdbRecordType->papFldDes[159]->offset=(short)((char *)&prec->adel - (char *)prec);
  pdbRecordType->papFldDes[160]->size=sizeof(prec->mdel);
  pdbRecordType->papFldDes[160]->offset=(short)((char *)&prec->mdel - (char *)prec);
  pdbRecordType->papFldDes[161]->size=sizeof(prec->alst);
  pdbRecordType->papFldDes[161]->offset=(short)((char *)&prec->alst - (char *)prec);
  pdbRecordType->papFldDes[162]->size=sizeof(prec->mlst);
  pdbRecordType->papFldDes[162]->offset=(short)((char *)&prec->mlst - (char *)prec);
  pdbRecordType->papFldDes[163]->size=sizeof(prec->sync);
  pdbRecordType->papFldDes[163]->offset=(short)((char *)&prec->sync - (char *)prec);
    pdbRecordType->rec_size = sizeof(*prec);
    return(0);
}
epicsExportRegistrar(motorRecordSizeOffset);
#ifdef __cplusplus
}
#endif
#endif /*GEN_SIZE_OFFSET*/
