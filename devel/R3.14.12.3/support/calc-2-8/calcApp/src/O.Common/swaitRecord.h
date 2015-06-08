
#ifndef INCswaitOOPTH
#define INCswaitOOPTH
typedef enum {
	swaitOOPT_Every_Time,
	swaitOOPT_On_Change,
	swaitOOPT_When_Zero,
	swaitOOPT_When_Non_zero,
	swaitOOPT_Transition_To_Zero,
	swaitOOPT_Transition_To_Non_zero,
	swaitOOPT_Never
}swaitOOPT;
#endif /*INCswaitOOPTH*/

#ifndef INCswaitINAVH
#define INCswaitINAVH
typedef enum {
	swaitINAV_PV_OK,
	swaitINAV_PV_BAD,
	swaitINAV_No_PV
}swaitINAV;
#endif /*INCswaitINAVH*/

#ifndef INCswaitINAPH
#define INCswaitINAPH
typedef enum {
	swaitINAP_No,
	swaitINAP_Yes
}swaitINAP;
#endif /*INCswaitINAPH*/

#ifndef INCswaitDOPTH
#define INCswaitDOPTH
typedef enum {
	swaitDOPT_Use_VAL,
	swaitDOPT_Use_DOL
}swaitDOPT;
#endif /*INCswaitDOPTH*/
#ifndef INCswaitH
#define INCswaitH
#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"
typedef struct swaitRecord {
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
	epicsFloat32	hopr;	/* High Operating Range */
	epicsFloat32	lopr;	/* Low Operating Range */
	epicsInt16	init;	/* Initialized? */
	void *         cbst;	/* Pointer to cbStruct */
	char		inan[40];	/* INPA PV Name */
	char		inbn[40];	/* INPB PV Name */
	char		incn[40];	/* INPC PV Name */
	char		indn[40];	/* INPD PV Name */
	char		inen[40];	/* INPE PV Name */
	char		infn[40];	/* INPF PV Name */
	char		ingn[40];	/* INPG PV Name */
	char		inhn[40];	/* INPH PV Name */
	char		inin[40];	/* INPI PV Name */
	char		injn[40];	/* INPJ PV Name */
	char		inkn[40];	/* INPK PV Name */
	char		inln[40];	/* INPL PV Name */
	char		doln[40];	/* DOL  PV Name */
	char		outn[40];	/* OUT  PV Name */
	epicsEnum16	inav;	/* INPA PV Status */
	epicsEnum16	inbv;	/* INPB PV Status */
	epicsEnum16	incv;	/* INPC PV Status */
	epicsEnum16	indv;	/* INPD PV Status */
	epicsEnum16	inev;	/* INPE PV Status */
	epicsEnum16	infv;	/* INPF PV Status */
	epicsEnum16	ingv;	/* INPG PV Status */
	epicsEnum16	inhv;	/* INPH PV Status */
	epicsEnum16	iniv;	/* INPI PV Status */
	epicsEnum16	injv;	/* INPJ PV Status */
	epicsEnum16	inkv;	/* INPK PV Status */
	epicsEnum16	inlv;	/* INPL PV Status */
	epicsEnum16	dolv;	/* DOL  PV Status */
	epicsEnum16	outv;	/* OUT  PV Status */
	epicsFloat64	a;	/* Value of Input A */
	epicsFloat64	b;	/* Value of Input B */
	epicsFloat64	c;	/* Value of Input C */
	epicsFloat64	d;	/* Value of Input D */
	epicsFloat64	e;	/* Value of Input E */
	epicsFloat64	f;	/* Value of Input F */
	epicsFloat64	g;	/* Value of Input G */
	epicsFloat64	h;	/* Value of Input H */
	epicsFloat64	i;	/* Value of Input I */
	epicsFloat64	j;	/* Value of Input J */
	epicsFloat64	k;	/* Value of Input K */
	epicsFloat64	l;	/* Value of Input L */
	epicsFloat64	la;	/* Last Val of Input A */
	epicsFloat64	lb;	/* Last Val of Input B */
	epicsFloat64	lc;	/* Last Val of Input C */
	epicsFloat64	ld;	/* Last Val of Input D */
	epicsFloat64	le;	/* Last Val of Input E */
	epicsFloat64	lf;	/* Last Val of Input F */
	epicsFloat64	lg;	/* Last Val of Input G */
	epicsFloat64	lh;	/* Last Val of Input H */
	epicsFloat64	li;	/* Last Val of Input I */
	epicsFloat64	lj;	/* Last Val of Input J */
	epicsFloat64	lk;	/* Last Val of Input K */
	epicsFloat64	ll;	/* Last Val of Input L */
	epicsEnum16	inap;	/* INPA causes I/O INTR */
	epicsEnum16	inbp;	/* INPB causes I/O INTR */
	epicsEnum16	incp;	/* INPC causes I/O INTR */
	epicsEnum16	indp;	/* INPD causes I/O INTR */
	epicsEnum16	inep;	/* INPE causes I/O INTR */
	epicsEnum16	infp;	/* INPF causes I/O INTR */
	epicsEnum16	ingp;	/* INPG causes I/O INTR */
	epicsEnum16	inhp;	/* INPH causes I/O INTR */
	epicsEnum16	inip;	/* INPI causes I/O INTR */
	epicsEnum16	injp;	/* INPJ causes I/O INTR */
	epicsEnum16	inkp;	/* INPK causes I/O INTR */
	epicsEnum16	inlp;	/* INPL causes I/O INTR */
	char		calc[36];	/* Calculation */
	char    rpcl[184];	/* Reverse Polish Calc */
	epicsInt32	clcv;	/* CALC Valid */
	epicsFloat64	val;	/* Value Field */
	epicsFloat64	oval;	/* Old Value */
	epicsInt16	prec;	/* Display Precision */
	epicsEnum16	oopt;	/* Output Execute Opt */
	epicsFloat32	odly;	/* Output Execute Delay */
	epicsEnum16	dopt;	/* Output Data Option */
	epicsFloat64	dold;	/* Desired Output Data */
	epicsUInt16	oevt;	/* Event To Issue */
	epicsFloat64	adel;	/* Archive Deadband */
	epicsFloat64	mdel;	/* Monitor Deadband */
	epicsFloat64	alst;	/* Last Value Archived */
	epicsFloat64	mlst;	/* Last Val Monitored */
	DBLINK		siol;	/* Sim Input Specifctn */
	epicsFloat64	sval;	/* Simulation Value */
	DBLINK		siml;	/* Sim Mode Location */
	epicsEnum16	simm;	/* Simulation Mode */
	epicsEnum16	sims;	/* Sim mode Alarm Svrty */
} swaitRecord;
#define swaitRecordNAME	0
#define swaitRecordDESC	1
#define swaitRecordASG	2
#define swaitRecordSCAN	3
#define swaitRecordPINI	4
#define swaitRecordPHAS	5
#define swaitRecordEVNT	6
#define swaitRecordTSE	7
#define swaitRecordTSEL	8
#define swaitRecordDTYP	9
#define swaitRecordDISV	10
#define swaitRecordDISA	11
#define swaitRecordSDIS	12
#define swaitRecordMLOK	13
#define swaitRecordMLIS	14
#define swaitRecordDISP	15
#define swaitRecordPROC	16
#define swaitRecordSTAT	17
#define swaitRecordSEVR	18
#define swaitRecordNSTA	19
#define swaitRecordNSEV	20
#define swaitRecordACKS	21
#define swaitRecordACKT	22
#define swaitRecordDISS	23
#define swaitRecordLCNT	24
#define swaitRecordPACT	25
#define swaitRecordPUTF	26
#define swaitRecordRPRO	27
#define swaitRecordASP	28
#define swaitRecordPPN	29
#define swaitRecordPPNR	30
#define swaitRecordSPVT	31
#define swaitRecordRSET	32
#define swaitRecordDSET	33
#define swaitRecordDPVT	34
#define swaitRecordRDES	35
#define swaitRecordLSET	36
#define swaitRecordPRIO	37
#define swaitRecordTPRO	38
#define swaitRecordBKPT	39
#define swaitRecordUDF	40
#define swaitRecordTIME	41
#define swaitRecordFLNK	42
#define swaitRecordVERS	43
#define swaitRecordHOPR	44
#define swaitRecordLOPR	45
#define swaitRecordINIT	46
#define swaitRecordCBST	47
#define swaitRecordINAN	48
#define swaitRecordINBN	49
#define swaitRecordINCN	50
#define swaitRecordINDN	51
#define swaitRecordINEN	52
#define swaitRecordINFN	53
#define swaitRecordINGN	54
#define swaitRecordINHN	55
#define swaitRecordININ	56
#define swaitRecordINJN	57
#define swaitRecordINKN	58
#define swaitRecordINLN	59
#define swaitRecordDOLN	60
#define swaitRecordOUTN	61
#define swaitRecordINAV	62
#define swaitRecordINBV	63
#define swaitRecordINCV	64
#define swaitRecordINDV	65
#define swaitRecordINEV	66
#define swaitRecordINFV	67
#define swaitRecordINGV	68
#define swaitRecordINHV	69
#define swaitRecordINIV	70
#define swaitRecordINJV	71
#define swaitRecordINKV	72
#define swaitRecordINLV	73
#define swaitRecordDOLV	74
#define swaitRecordOUTV	75
#define swaitRecordA	76
#define swaitRecordB	77
#define swaitRecordC	78
#define swaitRecordD	79
#define swaitRecordE	80
#define swaitRecordF	81
#define swaitRecordG	82
#define swaitRecordH	83
#define swaitRecordI	84
#define swaitRecordJ	85
#define swaitRecordK	86
#define swaitRecordL	87
#define swaitRecordLA	88
#define swaitRecordLB	89
#define swaitRecordLC	90
#define swaitRecordLD	91
#define swaitRecordLE	92
#define swaitRecordLF	93
#define swaitRecordLG	94
#define swaitRecordLH	95
#define swaitRecordLI	96
#define swaitRecordLJ	97
#define swaitRecordLK	98
#define swaitRecordLL	99
#define swaitRecordINAP	100
#define swaitRecordINBP	101
#define swaitRecordINCP	102
#define swaitRecordINDP	103
#define swaitRecordINEP	104
#define swaitRecordINFP	105
#define swaitRecordINGP	106
#define swaitRecordINHP	107
#define swaitRecordINIP	108
#define swaitRecordINJP	109
#define swaitRecordINKP	110
#define swaitRecordINLP	111
#define swaitRecordCALC	112
#define swaitRecordRPCL	113
#define swaitRecordCLCV	114
#define swaitRecordVAL	115
#define swaitRecordOVAL	116
#define swaitRecordPREC	117
#define swaitRecordOOPT	118
#define swaitRecordODLY	119
#define swaitRecordDOPT	120
#define swaitRecordDOLD	121
#define swaitRecordOEVT	122
#define swaitRecordADEL	123
#define swaitRecordMDEL	124
#define swaitRecordALST	125
#define swaitRecordMLST	126
#define swaitRecordSIOL	127
#define swaitRecordSVAL	128
#define swaitRecordSIML	129
#define swaitRecordSIMM	130
#define swaitRecordSIMS	131
#endif /*INCswaitH*/
#ifdef GEN_SIZE_OFFSET
#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int swaitRecordSizeOffset(dbRecordType *pdbRecordType)
{
    swaitRecord *prec = 0;
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
  pdbRecordType->papFldDes[44]->size=sizeof(prec->hopr);
  pdbRecordType->papFldDes[44]->offset=(short)((char *)&prec->hopr - (char *)prec);
  pdbRecordType->papFldDes[45]->size=sizeof(prec->lopr);
  pdbRecordType->papFldDes[45]->offset=(short)((char *)&prec->lopr - (char *)prec);
  pdbRecordType->papFldDes[46]->size=sizeof(prec->init);
  pdbRecordType->papFldDes[46]->offset=(short)((char *)&prec->init - (char *)prec);
  pdbRecordType->papFldDes[47]->size=sizeof(prec->cbst);
  pdbRecordType->papFldDes[47]->offset=(short)((char *)&prec->cbst - (char *)prec);
  pdbRecordType->papFldDes[48]->size=sizeof(prec->inan);
  pdbRecordType->papFldDes[48]->offset=(short)((char *)&prec->inan - (char *)prec);
  pdbRecordType->papFldDes[49]->size=sizeof(prec->inbn);
  pdbRecordType->papFldDes[49]->offset=(short)((char *)&prec->inbn - (char *)prec);
  pdbRecordType->papFldDes[50]->size=sizeof(prec->incn);
  pdbRecordType->papFldDes[50]->offset=(short)((char *)&prec->incn - (char *)prec);
  pdbRecordType->papFldDes[51]->size=sizeof(prec->indn);
  pdbRecordType->papFldDes[51]->offset=(short)((char *)&prec->indn - (char *)prec);
  pdbRecordType->papFldDes[52]->size=sizeof(prec->inen);
  pdbRecordType->papFldDes[52]->offset=(short)((char *)&prec->inen - (char *)prec);
  pdbRecordType->papFldDes[53]->size=sizeof(prec->infn);
  pdbRecordType->papFldDes[53]->offset=(short)((char *)&prec->infn - (char *)prec);
  pdbRecordType->papFldDes[54]->size=sizeof(prec->ingn);
  pdbRecordType->papFldDes[54]->offset=(short)((char *)&prec->ingn - (char *)prec);
  pdbRecordType->papFldDes[55]->size=sizeof(prec->inhn);
  pdbRecordType->papFldDes[55]->offset=(short)((char *)&prec->inhn - (char *)prec);
  pdbRecordType->papFldDes[56]->size=sizeof(prec->inin);
  pdbRecordType->papFldDes[56]->offset=(short)((char *)&prec->inin - (char *)prec);
  pdbRecordType->papFldDes[57]->size=sizeof(prec->injn);
  pdbRecordType->papFldDes[57]->offset=(short)((char *)&prec->injn - (char *)prec);
  pdbRecordType->papFldDes[58]->size=sizeof(prec->inkn);
  pdbRecordType->papFldDes[58]->offset=(short)((char *)&prec->inkn - (char *)prec);
  pdbRecordType->papFldDes[59]->size=sizeof(prec->inln);
  pdbRecordType->papFldDes[59]->offset=(short)((char *)&prec->inln - (char *)prec);
  pdbRecordType->papFldDes[60]->size=sizeof(prec->doln);
  pdbRecordType->papFldDes[60]->offset=(short)((char *)&prec->doln - (char *)prec);
  pdbRecordType->papFldDes[61]->size=sizeof(prec->outn);
  pdbRecordType->papFldDes[61]->offset=(short)((char *)&prec->outn - (char *)prec);
  pdbRecordType->papFldDes[62]->size=sizeof(prec->inav);
  pdbRecordType->papFldDes[62]->offset=(short)((char *)&prec->inav - (char *)prec);
  pdbRecordType->papFldDes[63]->size=sizeof(prec->inbv);
  pdbRecordType->papFldDes[63]->offset=(short)((char *)&prec->inbv - (char *)prec);
  pdbRecordType->papFldDes[64]->size=sizeof(prec->incv);
  pdbRecordType->papFldDes[64]->offset=(short)((char *)&prec->incv - (char *)prec);
  pdbRecordType->papFldDes[65]->size=sizeof(prec->indv);
  pdbRecordType->papFldDes[65]->offset=(short)((char *)&prec->indv - (char *)prec);
  pdbRecordType->papFldDes[66]->size=sizeof(prec->inev);
  pdbRecordType->papFldDes[66]->offset=(short)((char *)&prec->inev - (char *)prec);
  pdbRecordType->papFldDes[67]->size=sizeof(prec->infv);
  pdbRecordType->papFldDes[67]->offset=(short)((char *)&prec->infv - (char *)prec);
  pdbRecordType->papFldDes[68]->size=sizeof(prec->ingv);
  pdbRecordType->papFldDes[68]->offset=(short)((char *)&prec->ingv - (char *)prec);
  pdbRecordType->papFldDes[69]->size=sizeof(prec->inhv);
  pdbRecordType->papFldDes[69]->offset=(short)((char *)&prec->inhv - (char *)prec);
  pdbRecordType->papFldDes[70]->size=sizeof(prec->iniv);
  pdbRecordType->papFldDes[70]->offset=(short)((char *)&prec->iniv - (char *)prec);
  pdbRecordType->papFldDes[71]->size=sizeof(prec->injv);
  pdbRecordType->papFldDes[71]->offset=(short)((char *)&prec->injv - (char *)prec);
  pdbRecordType->papFldDes[72]->size=sizeof(prec->inkv);
  pdbRecordType->papFldDes[72]->offset=(short)((char *)&prec->inkv - (char *)prec);
  pdbRecordType->papFldDes[73]->size=sizeof(prec->inlv);
  pdbRecordType->papFldDes[73]->offset=(short)((char *)&prec->inlv - (char *)prec);
  pdbRecordType->papFldDes[74]->size=sizeof(prec->dolv);
  pdbRecordType->papFldDes[74]->offset=(short)((char *)&prec->dolv - (char *)prec);
  pdbRecordType->papFldDes[75]->size=sizeof(prec->outv);
  pdbRecordType->papFldDes[75]->offset=(short)((char *)&prec->outv - (char *)prec);
  pdbRecordType->papFldDes[76]->size=sizeof(prec->a);
  pdbRecordType->papFldDes[76]->offset=(short)((char *)&prec->a - (char *)prec);
  pdbRecordType->papFldDes[77]->size=sizeof(prec->b);
  pdbRecordType->papFldDes[77]->offset=(short)((char *)&prec->b - (char *)prec);
  pdbRecordType->papFldDes[78]->size=sizeof(prec->c);
  pdbRecordType->papFldDes[78]->offset=(short)((char *)&prec->c - (char *)prec);
  pdbRecordType->papFldDes[79]->size=sizeof(prec->d);
  pdbRecordType->papFldDes[79]->offset=(short)((char *)&prec->d - (char *)prec);
  pdbRecordType->papFldDes[80]->size=sizeof(prec->e);
  pdbRecordType->papFldDes[80]->offset=(short)((char *)&prec->e - (char *)prec);
  pdbRecordType->papFldDes[81]->size=sizeof(prec->f);
  pdbRecordType->papFldDes[81]->offset=(short)((char *)&prec->f - (char *)prec);
  pdbRecordType->papFldDes[82]->size=sizeof(prec->g);
  pdbRecordType->papFldDes[82]->offset=(short)((char *)&prec->g - (char *)prec);
  pdbRecordType->papFldDes[83]->size=sizeof(prec->h);
  pdbRecordType->papFldDes[83]->offset=(short)((char *)&prec->h - (char *)prec);
  pdbRecordType->papFldDes[84]->size=sizeof(prec->i);
  pdbRecordType->papFldDes[84]->offset=(short)((char *)&prec->i - (char *)prec);
  pdbRecordType->papFldDes[85]->size=sizeof(prec->j);
  pdbRecordType->papFldDes[85]->offset=(short)((char *)&prec->j - (char *)prec);
  pdbRecordType->papFldDes[86]->size=sizeof(prec->k);
  pdbRecordType->papFldDes[86]->offset=(short)((char *)&prec->k - (char *)prec);
  pdbRecordType->papFldDes[87]->size=sizeof(prec->l);
  pdbRecordType->papFldDes[87]->offset=(short)((char *)&prec->l - (char *)prec);
  pdbRecordType->papFldDes[88]->size=sizeof(prec->la);
  pdbRecordType->papFldDes[88]->offset=(short)((char *)&prec->la - (char *)prec);
  pdbRecordType->papFldDes[89]->size=sizeof(prec->lb);
  pdbRecordType->papFldDes[89]->offset=(short)((char *)&prec->lb - (char *)prec);
  pdbRecordType->papFldDes[90]->size=sizeof(prec->lc);
  pdbRecordType->papFldDes[90]->offset=(short)((char *)&prec->lc - (char *)prec);
  pdbRecordType->papFldDes[91]->size=sizeof(prec->ld);
  pdbRecordType->papFldDes[91]->offset=(short)((char *)&prec->ld - (char *)prec);
  pdbRecordType->papFldDes[92]->size=sizeof(prec->le);
  pdbRecordType->papFldDes[92]->offset=(short)((char *)&prec->le - (char *)prec);
  pdbRecordType->papFldDes[93]->size=sizeof(prec->lf);
  pdbRecordType->papFldDes[93]->offset=(short)((char *)&prec->lf - (char *)prec);
  pdbRecordType->papFldDes[94]->size=sizeof(prec->lg);
  pdbRecordType->papFldDes[94]->offset=(short)((char *)&prec->lg - (char *)prec);
  pdbRecordType->papFldDes[95]->size=sizeof(prec->lh);
  pdbRecordType->papFldDes[95]->offset=(short)((char *)&prec->lh - (char *)prec);
  pdbRecordType->papFldDes[96]->size=sizeof(prec->li);
  pdbRecordType->papFldDes[96]->offset=(short)((char *)&prec->li - (char *)prec);
  pdbRecordType->papFldDes[97]->size=sizeof(prec->lj);
  pdbRecordType->papFldDes[97]->offset=(short)((char *)&prec->lj - (char *)prec);
  pdbRecordType->papFldDes[98]->size=sizeof(prec->lk);
  pdbRecordType->papFldDes[98]->offset=(short)((char *)&prec->lk - (char *)prec);
  pdbRecordType->papFldDes[99]->size=sizeof(prec->ll);
  pdbRecordType->papFldDes[99]->offset=(short)((char *)&prec->ll - (char *)prec);
  pdbRecordType->papFldDes[100]->size=sizeof(prec->inap);
  pdbRecordType->papFldDes[100]->offset=(short)((char *)&prec->inap - (char *)prec);
  pdbRecordType->papFldDes[101]->size=sizeof(prec->inbp);
  pdbRecordType->papFldDes[101]->offset=(short)((char *)&prec->inbp - (char *)prec);
  pdbRecordType->papFldDes[102]->size=sizeof(prec->incp);
  pdbRecordType->papFldDes[102]->offset=(short)((char *)&prec->incp - (char *)prec);
  pdbRecordType->papFldDes[103]->size=sizeof(prec->indp);
  pdbRecordType->papFldDes[103]->offset=(short)((char *)&prec->indp - (char *)prec);
  pdbRecordType->papFldDes[104]->size=sizeof(prec->inep);
  pdbRecordType->papFldDes[104]->offset=(short)((char *)&prec->inep - (char *)prec);
  pdbRecordType->papFldDes[105]->size=sizeof(prec->infp);
  pdbRecordType->papFldDes[105]->offset=(short)((char *)&prec->infp - (char *)prec);
  pdbRecordType->papFldDes[106]->size=sizeof(prec->ingp);
  pdbRecordType->papFldDes[106]->offset=(short)((char *)&prec->ingp - (char *)prec);
  pdbRecordType->papFldDes[107]->size=sizeof(prec->inhp);
  pdbRecordType->papFldDes[107]->offset=(short)((char *)&prec->inhp - (char *)prec);
  pdbRecordType->papFldDes[108]->size=sizeof(prec->inip);
  pdbRecordType->papFldDes[108]->offset=(short)((char *)&prec->inip - (char *)prec);
  pdbRecordType->papFldDes[109]->size=sizeof(prec->injp);
  pdbRecordType->papFldDes[109]->offset=(short)((char *)&prec->injp - (char *)prec);
  pdbRecordType->papFldDes[110]->size=sizeof(prec->inkp);
  pdbRecordType->papFldDes[110]->offset=(short)((char *)&prec->inkp - (char *)prec);
  pdbRecordType->papFldDes[111]->size=sizeof(prec->inlp);
  pdbRecordType->papFldDes[111]->offset=(short)((char *)&prec->inlp - (char *)prec);
  pdbRecordType->papFldDes[112]->size=sizeof(prec->calc);
  pdbRecordType->papFldDes[112]->offset=(short)((char *)&prec->calc - (char *)prec);
  pdbRecordType->papFldDes[113]->size=sizeof(prec->rpcl);
  pdbRecordType->papFldDes[113]->offset=(short)((char *)&prec->rpcl - (char *)prec);
  pdbRecordType->papFldDes[114]->size=sizeof(prec->clcv);
  pdbRecordType->papFldDes[114]->offset=(short)((char *)&prec->clcv - (char *)prec);
  pdbRecordType->papFldDes[115]->size=sizeof(prec->val);
  pdbRecordType->papFldDes[115]->offset=(short)((char *)&prec->val - (char *)prec);
  pdbRecordType->papFldDes[116]->size=sizeof(prec->oval);
  pdbRecordType->papFldDes[116]->offset=(short)((char *)&prec->oval - (char *)prec);
  pdbRecordType->papFldDes[117]->size=sizeof(prec->prec);
  pdbRecordType->papFldDes[117]->offset=(short)((char *)&prec->prec - (char *)prec);
  pdbRecordType->papFldDes[118]->size=sizeof(prec->oopt);
  pdbRecordType->papFldDes[118]->offset=(short)((char *)&prec->oopt - (char *)prec);
  pdbRecordType->papFldDes[119]->size=sizeof(prec->odly);
  pdbRecordType->papFldDes[119]->offset=(short)((char *)&prec->odly - (char *)prec);
  pdbRecordType->papFldDes[120]->size=sizeof(prec->dopt);
  pdbRecordType->papFldDes[120]->offset=(short)((char *)&prec->dopt - (char *)prec);
  pdbRecordType->papFldDes[121]->size=sizeof(prec->dold);
  pdbRecordType->papFldDes[121]->offset=(short)((char *)&prec->dold - (char *)prec);
  pdbRecordType->papFldDes[122]->size=sizeof(prec->oevt);
  pdbRecordType->papFldDes[122]->offset=(short)((char *)&prec->oevt - (char *)prec);
  pdbRecordType->papFldDes[123]->size=sizeof(prec->adel);
  pdbRecordType->papFldDes[123]->offset=(short)((char *)&prec->adel - (char *)prec);
  pdbRecordType->papFldDes[124]->size=sizeof(prec->mdel);
  pdbRecordType->papFldDes[124]->offset=(short)((char *)&prec->mdel - (char *)prec);
  pdbRecordType->papFldDes[125]->size=sizeof(prec->alst);
  pdbRecordType->papFldDes[125]->offset=(short)((char *)&prec->alst - (char *)prec);
  pdbRecordType->papFldDes[126]->size=sizeof(prec->mlst);
  pdbRecordType->papFldDes[126]->offset=(short)((char *)&prec->mlst - (char *)prec);
  pdbRecordType->papFldDes[127]->size=sizeof(prec->siol);
  pdbRecordType->papFldDes[127]->offset=(short)((char *)&prec->siol - (char *)prec);
  pdbRecordType->papFldDes[128]->size=sizeof(prec->sval);
  pdbRecordType->papFldDes[128]->offset=(short)((char *)&prec->sval - (char *)prec);
  pdbRecordType->papFldDes[129]->size=sizeof(prec->siml);
  pdbRecordType->papFldDes[129]->offset=(short)((char *)&prec->siml - (char *)prec);
  pdbRecordType->papFldDes[130]->size=sizeof(prec->simm);
  pdbRecordType->papFldDes[130]->offset=(short)((char *)&prec->simm - (char *)prec);
  pdbRecordType->papFldDes[131]->size=sizeof(prec->sims);
  pdbRecordType->papFldDes[131]->offset=(short)((char *)&prec->sims - (char *)prec);
    pdbRecordType->rec_size = sizeof(*prec);
    return(0);
}
epicsExportRegistrar(swaitRecordSizeOffset);
#ifdef __cplusplus
}
#endif
#endif /*GEN_SIZE_OFFSET*/
