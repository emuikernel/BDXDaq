
#ifndef INCvsTYPEH
#define INCvsTYPEH
typedef enum {
	vsTYPE_GP307,
	vsTYPE_GP350,
	vsTYPE_MM200
}vsTYPE;
#endif /*INCvsTYPEH*/

#ifndef INCvsOFFONH
#define INCvsOFFONH
typedef enum {
	vsOFFON_Off,
	vsOFFON_On
}vsOFFON;
#endif /*INCvsOFFONH*/
#ifndef INCvsH
#define INCvsH
#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"
typedef struct vsRecord {
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
	DBLINK		inp;	/* Device Specification */
	epicsEnum16	type;	/* Controller Type */
	epicsInt16	err;	/* Controller Err Cnt */
	epicsEnum16	ig1s;	/* Ion Gauge 1 Set */
	epicsEnum16	ig2s;	/* Ion Gauge 2 Set */
	epicsEnum16	dgss;	/* Degas Set */
	epicsEnum16	ig1r;	/* Ion Gauge 1 Read */
	epicsEnum16	ig2r;	/* Ion Gauge 2 Read */
	epicsEnum16	dgsr;	/* Degas Read */
	epicsEnum16	fltr;	/* Fault Read */
	epicsEnum16	sp1;	/* Set Point 1 */
	epicsEnum16	sp2;	/* Set Point 2 */
	epicsEnum16	sp3;	/* Set Point 3 */
	epicsEnum16	sp4;	/* Set Point 4 */
	epicsEnum16	sp5;	/* Set Point 5 */
	epicsEnum16	sp6;	/* Set Point 6 */
	epicsFloat64	sp1s;	/* SP 1 Setpoint Set */
	epicsFloat64	sp1r;	/* SP 1 Readback */
	epicsFloat64	sp2s;	/* SP 2 Setpoint Set */
	epicsFloat64	sp2r;	/* SP 2 Readback */
	epicsFloat64	sp3s;	/* SP 3 Setpoint Set */
	epicsFloat64	sp3r;	/* SP 3 Readback */
	epicsFloat64	sp4s;	/* SP 4 Setpoint Set */
	epicsFloat64	sp4r;	/* SP 4 Readback */
	epicsFloat64	val;	/* Gauge Pressure */
	epicsFloat64	pres;	/* Gauge Pressure */
	epicsFloat64	cgap;	/* Convectron-A Pressure */
	epicsFloat64	cgbp;	/* Convectron-B Pressure */
	epicsFloat64	lprs;	/* IG Log10 Pressure */
	epicsFloat64	lcap;	/* Conv-A Log10 Pressure */
	epicsFloat64	lcbp;	/* Conv-B Log10 Pressure */
	epicsUInt16	chgc;	/* Changed Control */
	epicsFloat32	hopr;	/* IG Pres High Display */
	epicsFloat32	lopr;	/* IG Pres Low Display */
	epicsFloat32	hlpr;	/* IG Log10 High Display */
	epicsFloat32	llpr;	/* IG Log10 Low Display */
	epicsFloat32	hapr;	/* CGA Pres High Display */
	epicsFloat32	lapr;	/* CGA Pres Low Display */
	epicsFloat32	halr;	/* CGA Log10 High Display */
	epicsFloat32	lalr;	/* CGA Log10 Low Display */
	epicsFloat32	hbpr;	/* CGB Pres High Display */
	epicsFloat32	lbpr;	/* CGB Pres Low Display */
	epicsFloat32	hblr;	/* CGB Log10 High Display */
	epicsFloat32	lblr;	/* CGB Log10 Low Display */
	epicsEnum16	pi1s;	/* prev Ion Gauge 1 */
	epicsEnum16	pi2s;	/* prev Ion Gauge 2 */
	epicsEnum16	pdss;	/* prev Degas */
	epicsEnum16	pig1;	/* prev Ion Gauge 1 */
	epicsEnum16	pig2;	/* prev Ion Gauge 2 */
	epicsEnum16	pdgs;	/* prev Degas */
	epicsEnum16	pflt;	/* prev Fault */
	epicsEnum16	psp1;	/* prev Set Point 1 */
	epicsEnum16	psp2;	/* prev Set Point 2 */
	epicsEnum16	psp3;	/* prev Set Point 3 */
	epicsEnum16	psp4;	/* prev Set Point 4 */
	epicsEnum16	psp5;	/* prev Set Point 5 */
	epicsEnum16	psp6;	/* prev Set Point 6 */
	epicsFloat64	ps1s;	/* prev SP1 Set */
	epicsFloat64	ps2s;	/* prev SP2 Set */
	epicsFloat64	ps3s;	/* prev SP3 Set */
	epicsFloat64	ps4s;	/* prev SP4 Set */
	epicsFloat64	ps1r;	/* prev SP1 Readback */
	epicsFloat64	ps2r;	/* prev SP2 Readback */
	epicsFloat64	ps3r;	/* prev SP3 Readback */
	epicsFloat64	ps4r;	/* prev SP4 Readback */
	epicsFloat64	pval;	/* prev Gauge Pres */
	epicsFloat64	ppre;	/* prev Gauge Pres */
	epicsFloat64	pcga;	/* prev Conv-A Pres */
	epicsFloat64	pcgb;	/* prev Conv-B Pres */
	epicsFloat64	plpe;	/* prev IG Log10 Pres */
	epicsFloat64	plca;	/* prev Conv-A Log10 Pres */
	epicsFloat64	plcb;	/* prev Conv-B Log10 Pres */
	epicsFloat32	hihi;	/* IG Hihi Alarm */
	epicsFloat32	lolo;	/* IG Lolo Alarm */
	epicsFloat32	high;	/* IG High Alarm */
	epicsFloat32	low;	/* IG Low Alarm */
	epicsEnum16	hhsv;	/* IG Hihi Severity */
	epicsEnum16	llsv;	/* IG Lolo Severity */
	epicsEnum16	hsv;	/* IG High Severity */
	epicsEnum16	lsv;	/* IG Low Severity */
	epicsFloat64	hyst;	/* IG Alarm Deadband */
	epicsFloat64	lalm;	/* IG Last Value Alarmed */
} vsRecord;
#define vsRecordNAME	0
#define vsRecordDESC	1
#define vsRecordASG	2
#define vsRecordSCAN	3
#define vsRecordPINI	4
#define vsRecordPHAS	5
#define vsRecordEVNT	6
#define vsRecordTSE	7
#define vsRecordTSEL	8
#define vsRecordDTYP	9
#define vsRecordDISV	10
#define vsRecordDISA	11
#define vsRecordSDIS	12
#define vsRecordMLOK	13
#define vsRecordMLIS	14
#define vsRecordDISP	15
#define vsRecordPROC	16
#define vsRecordSTAT	17
#define vsRecordSEVR	18
#define vsRecordNSTA	19
#define vsRecordNSEV	20
#define vsRecordACKS	21
#define vsRecordACKT	22
#define vsRecordDISS	23
#define vsRecordLCNT	24
#define vsRecordPACT	25
#define vsRecordPUTF	26
#define vsRecordRPRO	27
#define vsRecordASP	28
#define vsRecordPPN	29
#define vsRecordPPNR	30
#define vsRecordSPVT	31
#define vsRecordRSET	32
#define vsRecordDSET	33
#define vsRecordDPVT	34
#define vsRecordRDES	35
#define vsRecordLSET	36
#define vsRecordPRIO	37
#define vsRecordTPRO	38
#define vsRecordBKPT	39
#define vsRecordUDF	40
#define vsRecordTIME	41
#define vsRecordFLNK	42
#define vsRecordINP	43
#define vsRecordTYPE	44
#define vsRecordERR	45
#define vsRecordIG1S	46
#define vsRecordIG2S	47
#define vsRecordDGSS	48
#define vsRecordIG1R	49
#define vsRecordIG2R	50
#define vsRecordDGSR	51
#define vsRecordFLTR	52
#define vsRecordSP1	53
#define vsRecordSP2	54
#define vsRecordSP3	55
#define vsRecordSP4	56
#define vsRecordSP5	57
#define vsRecordSP6	58
#define vsRecordSP1S	59
#define vsRecordSP1R	60
#define vsRecordSP2S	61
#define vsRecordSP2R	62
#define vsRecordSP3S	63
#define vsRecordSP3R	64
#define vsRecordSP4S	65
#define vsRecordSP4R	66
#define vsRecordVAL	67
#define vsRecordPRES	68
#define vsRecordCGAP	69
#define vsRecordCGBP	70
#define vsRecordLPRS	71
#define vsRecordLCAP	72
#define vsRecordLCBP	73
#define vsRecordCHGC	74
#define vsRecordHOPR	75
#define vsRecordLOPR	76
#define vsRecordHLPR	77
#define vsRecordLLPR	78
#define vsRecordHAPR	79
#define vsRecordLAPR	80
#define vsRecordHALR	81
#define vsRecordLALR	82
#define vsRecordHBPR	83
#define vsRecordLBPR	84
#define vsRecordHBLR	85
#define vsRecordLBLR	86
#define vsRecordPI1S	87
#define vsRecordPI2S	88
#define vsRecordPDSS	89
#define vsRecordPIG1	90
#define vsRecordPIG2	91
#define vsRecordPDGS	92
#define vsRecordPFLT	93
#define vsRecordPSP1	94
#define vsRecordPSP2	95
#define vsRecordPSP3	96
#define vsRecordPSP4	97
#define vsRecordPSP5	98
#define vsRecordPSP6	99
#define vsRecordPS1S	100
#define vsRecordPS2S	101
#define vsRecordPS3S	102
#define vsRecordPS4S	103
#define vsRecordPS1R	104
#define vsRecordPS2R	105
#define vsRecordPS3R	106
#define vsRecordPS4R	107
#define vsRecordPVAL	108
#define vsRecordPPRE	109
#define vsRecordPCGA	110
#define vsRecordPCGB	111
#define vsRecordPLPE	112
#define vsRecordPLCA	113
#define vsRecordPLCB	114
#define vsRecordHIHI	115
#define vsRecordLOLO	116
#define vsRecordHIGH	117
#define vsRecordLOW	118
#define vsRecordHHSV	119
#define vsRecordLLSV	120
#define vsRecordHSV	121
#define vsRecordLSV	122
#define vsRecordHYST	123
#define vsRecordLALM	124
#endif /*INCvsH*/
#ifdef GEN_SIZE_OFFSET
#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int vsRecordSizeOffset(dbRecordType *pdbRecordType)
{
    vsRecord *prec = 0;
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
  pdbRecordType->papFldDes[43]->size=sizeof(prec->inp);
  pdbRecordType->papFldDes[43]->offset=(short)((char *)&prec->inp - (char *)prec);
  pdbRecordType->papFldDes[44]->size=sizeof(prec->type);
  pdbRecordType->papFldDes[44]->offset=(short)((char *)&prec->type - (char *)prec);
  pdbRecordType->papFldDes[45]->size=sizeof(prec->err);
  pdbRecordType->papFldDes[45]->offset=(short)((char *)&prec->err - (char *)prec);
  pdbRecordType->papFldDes[46]->size=sizeof(prec->ig1s);
  pdbRecordType->papFldDes[46]->offset=(short)((char *)&prec->ig1s - (char *)prec);
  pdbRecordType->papFldDes[47]->size=sizeof(prec->ig2s);
  pdbRecordType->papFldDes[47]->offset=(short)((char *)&prec->ig2s - (char *)prec);
  pdbRecordType->papFldDes[48]->size=sizeof(prec->dgss);
  pdbRecordType->papFldDes[48]->offset=(short)((char *)&prec->dgss - (char *)prec);
  pdbRecordType->papFldDes[49]->size=sizeof(prec->ig1r);
  pdbRecordType->papFldDes[49]->offset=(short)((char *)&prec->ig1r - (char *)prec);
  pdbRecordType->papFldDes[50]->size=sizeof(prec->ig2r);
  pdbRecordType->papFldDes[50]->offset=(short)((char *)&prec->ig2r - (char *)prec);
  pdbRecordType->papFldDes[51]->size=sizeof(prec->dgsr);
  pdbRecordType->papFldDes[51]->offset=(short)((char *)&prec->dgsr - (char *)prec);
  pdbRecordType->papFldDes[52]->size=sizeof(prec->fltr);
  pdbRecordType->papFldDes[52]->offset=(short)((char *)&prec->fltr - (char *)prec);
  pdbRecordType->papFldDes[53]->size=sizeof(prec->sp1);
  pdbRecordType->papFldDes[53]->offset=(short)((char *)&prec->sp1 - (char *)prec);
  pdbRecordType->papFldDes[54]->size=sizeof(prec->sp2);
  pdbRecordType->papFldDes[54]->offset=(short)((char *)&prec->sp2 - (char *)prec);
  pdbRecordType->papFldDes[55]->size=sizeof(prec->sp3);
  pdbRecordType->papFldDes[55]->offset=(short)((char *)&prec->sp3 - (char *)prec);
  pdbRecordType->papFldDes[56]->size=sizeof(prec->sp4);
  pdbRecordType->papFldDes[56]->offset=(short)((char *)&prec->sp4 - (char *)prec);
  pdbRecordType->papFldDes[57]->size=sizeof(prec->sp5);
  pdbRecordType->papFldDes[57]->offset=(short)((char *)&prec->sp5 - (char *)prec);
  pdbRecordType->papFldDes[58]->size=sizeof(prec->sp6);
  pdbRecordType->papFldDes[58]->offset=(short)((char *)&prec->sp6 - (char *)prec);
  pdbRecordType->papFldDes[59]->size=sizeof(prec->sp1s);
  pdbRecordType->papFldDes[59]->offset=(short)((char *)&prec->sp1s - (char *)prec);
  pdbRecordType->papFldDes[60]->size=sizeof(prec->sp1r);
  pdbRecordType->papFldDes[60]->offset=(short)((char *)&prec->sp1r - (char *)prec);
  pdbRecordType->papFldDes[61]->size=sizeof(prec->sp2s);
  pdbRecordType->papFldDes[61]->offset=(short)((char *)&prec->sp2s - (char *)prec);
  pdbRecordType->papFldDes[62]->size=sizeof(prec->sp2r);
  pdbRecordType->papFldDes[62]->offset=(short)((char *)&prec->sp2r - (char *)prec);
  pdbRecordType->papFldDes[63]->size=sizeof(prec->sp3s);
  pdbRecordType->papFldDes[63]->offset=(short)((char *)&prec->sp3s - (char *)prec);
  pdbRecordType->papFldDes[64]->size=sizeof(prec->sp3r);
  pdbRecordType->papFldDes[64]->offset=(short)((char *)&prec->sp3r - (char *)prec);
  pdbRecordType->papFldDes[65]->size=sizeof(prec->sp4s);
  pdbRecordType->papFldDes[65]->offset=(short)((char *)&prec->sp4s - (char *)prec);
  pdbRecordType->papFldDes[66]->size=sizeof(prec->sp4r);
  pdbRecordType->papFldDes[66]->offset=(short)((char *)&prec->sp4r - (char *)prec);
  pdbRecordType->papFldDes[67]->size=sizeof(prec->val);
  pdbRecordType->papFldDes[67]->offset=(short)((char *)&prec->val - (char *)prec);
  pdbRecordType->papFldDes[68]->size=sizeof(prec->pres);
  pdbRecordType->papFldDes[68]->offset=(short)((char *)&prec->pres - (char *)prec);
  pdbRecordType->papFldDes[69]->size=sizeof(prec->cgap);
  pdbRecordType->papFldDes[69]->offset=(short)((char *)&prec->cgap - (char *)prec);
  pdbRecordType->papFldDes[70]->size=sizeof(prec->cgbp);
  pdbRecordType->papFldDes[70]->offset=(short)((char *)&prec->cgbp - (char *)prec);
  pdbRecordType->papFldDes[71]->size=sizeof(prec->lprs);
  pdbRecordType->papFldDes[71]->offset=(short)((char *)&prec->lprs - (char *)prec);
  pdbRecordType->papFldDes[72]->size=sizeof(prec->lcap);
  pdbRecordType->papFldDes[72]->offset=(short)((char *)&prec->lcap - (char *)prec);
  pdbRecordType->papFldDes[73]->size=sizeof(prec->lcbp);
  pdbRecordType->papFldDes[73]->offset=(short)((char *)&prec->lcbp - (char *)prec);
  pdbRecordType->papFldDes[74]->size=sizeof(prec->chgc);
  pdbRecordType->papFldDes[74]->offset=(short)((char *)&prec->chgc - (char *)prec);
  pdbRecordType->papFldDes[75]->size=sizeof(prec->hopr);
  pdbRecordType->papFldDes[75]->offset=(short)((char *)&prec->hopr - (char *)prec);
  pdbRecordType->papFldDes[76]->size=sizeof(prec->lopr);
  pdbRecordType->papFldDes[76]->offset=(short)((char *)&prec->lopr - (char *)prec);
  pdbRecordType->papFldDes[77]->size=sizeof(prec->hlpr);
  pdbRecordType->papFldDes[77]->offset=(short)((char *)&prec->hlpr - (char *)prec);
  pdbRecordType->papFldDes[78]->size=sizeof(prec->llpr);
  pdbRecordType->papFldDes[78]->offset=(short)((char *)&prec->llpr - (char *)prec);
  pdbRecordType->papFldDes[79]->size=sizeof(prec->hapr);
  pdbRecordType->papFldDes[79]->offset=(short)((char *)&prec->hapr - (char *)prec);
  pdbRecordType->papFldDes[80]->size=sizeof(prec->lapr);
  pdbRecordType->papFldDes[80]->offset=(short)((char *)&prec->lapr - (char *)prec);
  pdbRecordType->papFldDes[81]->size=sizeof(prec->halr);
  pdbRecordType->papFldDes[81]->offset=(short)((char *)&prec->halr - (char *)prec);
  pdbRecordType->papFldDes[82]->size=sizeof(prec->lalr);
  pdbRecordType->papFldDes[82]->offset=(short)((char *)&prec->lalr - (char *)prec);
  pdbRecordType->papFldDes[83]->size=sizeof(prec->hbpr);
  pdbRecordType->papFldDes[83]->offset=(short)((char *)&prec->hbpr - (char *)prec);
  pdbRecordType->papFldDes[84]->size=sizeof(prec->lbpr);
  pdbRecordType->papFldDes[84]->offset=(short)((char *)&prec->lbpr - (char *)prec);
  pdbRecordType->papFldDes[85]->size=sizeof(prec->hblr);
  pdbRecordType->papFldDes[85]->offset=(short)((char *)&prec->hblr - (char *)prec);
  pdbRecordType->papFldDes[86]->size=sizeof(prec->lblr);
  pdbRecordType->papFldDes[86]->offset=(short)((char *)&prec->lblr - (char *)prec);
  pdbRecordType->papFldDes[87]->size=sizeof(prec->pi1s);
  pdbRecordType->papFldDes[87]->offset=(short)((char *)&prec->pi1s - (char *)prec);
  pdbRecordType->papFldDes[88]->size=sizeof(prec->pi2s);
  pdbRecordType->papFldDes[88]->offset=(short)((char *)&prec->pi2s - (char *)prec);
  pdbRecordType->papFldDes[89]->size=sizeof(prec->pdss);
  pdbRecordType->papFldDes[89]->offset=(short)((char *)&prec->pdss - (char *)prec);
  pdbRecordType->papFldDes[90]->size=sizeof(prec->pig1);
  pdbRecordType->papFldDes[90]->offset=(short)((char *)&prec->pig1 - (char *)prec);
  pdbRecordType->papFldDes[91]->size=sizeof(prec->pig2);
  pdbRecordType->papFldDes[91]->offset=(short)((char *)&prec->pig2 - (char *)prec);
  pdbRecordType->papFldDes[92]->size=sizeof(prec->pdgs);
  pdbRecordType->papFldDes[92]->offset=(short)((char *)&prec->pdgs - (char *)prec);
  pdbRecordType->papFldDes[93]->size=sizeof(prec->pflt);
  pdbRecordType->papFldDes[93]->offset=(short)((char *)&prec->pflt - (char *)prec);
  pdbRecordType->papFldDes[94]->size=sizeof(prec->psp1);
  pdbRecordType->papFldDes[94]->offset=(short)((char *)&prec->psp1 - (char *)prec);
  pdbRecordType->papFldDes[95]->size=sizeof(prec->psp2);
  pdbRecordType->papFldDes[95]->offset=(short)((char *)&prec->psp2 - (char *)prec);
  pdbRecordType->papFldDes[96]->size=sizeof(prec->psp3);
  pdbRecordType->papFldDes[96]->offset=(short)((char *)&prec->psp3 - (char *)prec);
  pdbRecordType->papFldDes[97]->size=sizeof(prec->psp4);
  pdbRecordType->papFldDes[97]->offset=(short)((char *)&prec->psp4 - (char *)prec);
  pdbRecordType->papFldDes[98]->size=sizeof(prec->psp5);
  pdbRecordType->papFldDes[98]->offset=(short)((char *)&prec->psp5 - (char *)prec);
  pdbRecordType->papFldDes[99]->size=sizeof(prec->psp6);
  pdbRecordType->papFldDes[99]->offset=(short)((char *)&prec->psp6 - (char *)prec);
  pdbRecordType->papFldDes[100]->size=sizeof(prec->ps1s);
  pdbRecordType->papFldDes[100]->offset=(short)((char *)&prec->ps1s - (char *)prec);
  pdbRecordType->papFldDes[101]->size=sizeof(prec->ps2s);
  pdbRecordType->papFldDes[101]->offset=(short)((char *)&prec->ps2s - (char *)prec);
  pdbRecordType->papFldDes[102]->size=sizeof(prec->ps3s);
  pdbRecordType->papFldDes[102]->offset=(short)((char *)&prec->ps3s - (char *)prec);
  pdbRecordType->papFldDes[103]->size=sizeof(prec->ps4s);
  pdbRecordType->papFldDes[103]->offset=(short)((char *)&prec->ps4s - (char *)prec);
  pdbRecordType->papFldDes[104]->size=sizeof(prec->ps1r);
  pdbRecordType->papFldDes[104]->offset=(short)((char *)&prec->ps1r - (char *)prec);
  pdbRecordType->papFldDes[105]->size=sizeof(prec->ps2r);
  pdbRecordType->papFldDes[105]->offset=(short)((char *)&prec->ps2r - (char *)prec);
  pdbRecordType->papFldDes[106]->size=sizeof(prec->ps3r);
  pdbRecordType->papFldDes[106]->offset=(short)((char *)&prec->ps3r - (char *)prec);
  pdbRecordType->papFldDes[107]->size=sizeof(prec->ps4r);
  pdbRecordType->papFldDes[107]->offset=(short)((char *)&prec->ps4r - (char *)prec);
  pdbRecordType->papFldDes[108]->size=sizeof(prec->pval);
  pdbRecordType->papFldDes[108]->offset=(short)((char *)&prec->pval - (char *)prec);
  pdbRecordType->papFldDes[109]->size=sizeof(prec->ppre);
  pdbRecordType->papFldDes[109]->offset=(short)((char *)&prec->ppre - (char *)prec);
  pdbRecordType->papFldDes[110]->size=sizeof(prec->pcga);
  pdbRecordType->papFldDes[110]->offset=(short)((char *)&prec->pcga - (char *)prec);
  pdbRecordType->papFldDes[111]->size=sizeof(prec->pcgb);
  pdbRecordType->papFldDes[111]->offset=(short)((char *)&prec->pcgb - (char *)prec);
  pdbRecordType->papFldDes[112]->size=sizeof(prec->plpe);
  pdbRecordType->papFldDes[112]->offset=(short)((char *)&prec->plpe - (char *)prec);
  pdbRecordType->papFldDes[113]->size=sizeof(prec->plca);
  pdbRecordType->papFldDes[113]->offset=(short)((char *)&prec->plca - (char *)prec);
  pdbRecordType->papFldDes[114]->size=sizeof(prec->plcb);
  pdbRecordType->papFldDes[114]->offset=(short)((char *)&prec->plcb - (char *)prec);
  pdbRecordType->papFldDes[115]->size=sizeof(prec->hihi);
  pdbRecordType->papFldDes[115]->offset=(short)((char *)&prec->hihi - (char *)prec);
  pdbRecordType->papFldDes[116]->size=sizeof(prec->lolo);
  pdbRecordType->papFldDes[116]->offset=(short)((char *)&prec->lolo - (char *)prec);
  pdbRecordType->papFldDes[117]->size=sizeof(prec->high);
  pdbRecordType->papFldDes[117]->offset=(short)((char *)&prec->high - (char *)prec);
  pdbRecordType->papFldDes[118]->size=sizeof(prec->low);
  pdbRecordType->papFldDes[118]->offset=(short)((char *)&prec->low - (char *)prec);
  pdbRecordType->papFldDes[119]->size=sizeof(prec->hhsv);
  pdbRecordType->papFldDes[119]->offset=(short)((char *)&prec->hhsv - (char *)prec);
  pdbRecordType->papFldDes[120]->size=sizeof(prec->llsv);
  pdbRecordType->papFldDes[120]->offset=(short)((char *)&prec->llsv - (char *)prec);
  pdbRecordType->papFldDes[121]->size=sizeof(prec->hsv);
  pdbRecordType->papFldDes[121]->offset=(short)((char *)&prec->hsv - (char *)prec);
  pdbRecordType->papFldDes[122]->size=sizeof(prec->lsv);
  pdbRecordType->papFldDes[122]->offset=(short)((char *)&prec->lsv - (char *)prec);
  pdbRecordType->papFldDes[123]->size=sizeof(prec->hyst);
  pdbRecordType->papFldDes[123]->offset=(short)((char *)&prec->hyst - (char *)prec);
  pdbRecordType->papFldDes[124]->size=sizeof(prec->lalm);
  pdbRecordType->papFldDes[124]->offset=(short)((char *)&prec->lalm - (char *)prec);
    pdbRecordType->rec_size = sizeof(*prec);
    return(0);
}
epicsExportRegistrar(vsRecordSizeOffset);
#ifdef __cplusplus
}
#endif
#endif /*GEN_SIZE_OFFSET*/
