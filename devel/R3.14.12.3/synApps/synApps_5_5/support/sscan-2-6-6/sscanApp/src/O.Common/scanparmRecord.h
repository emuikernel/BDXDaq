
#ifndef INCsscanPAUSH
#define INCsscanPAUSH
typedef enum {
	sscanPAUS_Go,
	sscanPAUS_Pause
}sscanPAUS;
#endif /*INCsscanPAUSH*/

#ifndef INCsscanPASMH
#define INCsscanPASMH
typedef enum {
	sscanPASM_Stay,
	sscanPASM_Start_Pos,
	sscanPASM_Prior_Pos,
	sscanPASM_Peak_Pos,
	sscanPASM_Valley_Pos,
	sscanPASM_RisingEdge_Pos,
	sscanPASM_FallingEdge_Pos,
	sscanPASM_COM
}sscanPASM;
#endif /*INCsscanPASMH*/

#ifndef INCsscanP1SMH
#define INCsscanP1SMH
typedef enum {
	sscanP1SM_Linear,
	sscanP1SM_Table,
	sscanP1SM_On_The_Fly
}sscanP1SM;
#endif /*INCsscanP1SMH*/

#ifndef INCsscanP1NVH
#define INCsscanP1NVH
typedef enum {
	sscanP1NV_PV_OK,
	sscanP1NV_No_PV,
	sscanP1NV_PV_NoRead,
	sscanP1NV_PV_xxx,
	sscanP1NV_PV_NoWrite,
	sscanP1NV_PV_yyy,
	sscanP1NV_PV_NC
}sscanP1NV;
#endif /*INCsscanP1NVH*/

#ifndef INCsscanP1ARH
#define INCsscanP1ARH
typedef enum {
	sscanP1AR_Absolute,
	sscanP1AR_Relative
}sscanP1AR;
#endif /*INCsscanP1ARH*/

#ifndef INCsscanNOYESH
#define INCsscanNOYESH
typedef enum {
	sscanNOYES_NO,
	sscanNOYES_YES
}sscanNOYES;
#endif /*INCsscanNOYESH*/

#ifndef INCsscanLINKWAITH
#define INCsscanLINKWAITH
typedef enum {
	sscanLINKWAIT_YES,
	sscanLINKWAIT_NO
}sscanLINKWAIT;
#endif /*INCsscanLINKWAITH*/

#ifndef INCsscanFPTSH
#define INCsscanFPTSH
typedef enum {
	sscanFPTS_No,
	sscanFPTS_Freeze
}sscanFPTS;
#endif /*INCsscanFPTSH*/

#ifndef INCsscanFFOH
#define INCsscanFFOH
typedef enum {
	sscanFFO_Use_F_Flags,
	sscanFFO_Override
}sscanFFO;
#endif /*INCsscanFFOH*/

#ifndef INCsscanFAZEH
#define INCsscanFAZEH
typedef enum {
	sscanFAZE_IDLE,
	sscanFAZE_INIT_SCAN,
	sscanFAZE_BEFORE_SCAN,
	sscanFAZE_BEFORE_SCAN_WAIT,
	sscanFAZE_MOVE_MOTORS,
	sscanFAZE_CHECK_MOTORS,
	sscanFAZE_TRIG_DETCTRS,
	sscanFAZE_READ_DETCTRS,
	sscanFAZE_START_FLY,
	sscanFAZE_RETRACE_MOVE,
	sscanFAZE_RETRACE_WAIT,
	sscanFAZE_AFTER_SCAN_DO,
	sscanFAZE_AFTER_SCAN_WAIT,
	sscanFAZE_SCAN_DONE,
	sscanFAZE_SAVE_DATA_WAIT,
	sscanFAZE_SCAN_PENDING,
	sscanFAZE_TRIG_ARRAY_READ,
	sscanFAZE_ARRAY_READ_WAIT,
	sscanFAZE_PREVIEW,
	sscanFAZE_RECORD_SCALAR_DATA
}sscanFAZE;
#endif /*INCsscanFAZEH*/

#ifndef INCsscanDSTATEH
#define INCsscanDSTATEH
typedef enum {
	sscanDSTATE_UNPACKED,
	sscanDSTATE_TRIG_ARRAY_READ,
	sscanDSTATE_ARRAY_READ_WAIT,
	sscanDSTATE_ARRAY_GET_CALLBACK_WAIT,
	sscanDSTATE_RECORD_ARRAY_DATA,
	sscanDSTATE_SAVE_DATA_WAIT,
	sscanDSTATE_PACKED,
	sscanDSTATE_POSTED
}sscanDSTATE;
#endif /*INCsscanDSTATEH*/

#ifndef INCsscanACQTH
#define INCsscanACQTH
typedef enum {
	sscanACQT_SCALAR,
	sscanACQT_1D_ARRAY
}sscanACQT;
#endif /*INCsscanACQTH*/

#ifndef INCsscanACQMH
#define INCsscanACQMH
typedef enum {
	sscanACQM_NORMAL,
	sscanACQM_ACC,
	sscanACQM_ADD
}sscanACQM;
#endif /*INCsscanACQMH*/
#ifndef INCscanparmH
#define INCscanparmH
#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"
typedef struct scanparmRecord {
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
	epicsFloat64	val;	/* Result */
	epicsInt16	prec;	/* Display Precision */
	epicsInt16	pre;	/* PRE-write command */
	DBLINK		opre;	/* PRE-write OutLink */
	epicsEnum16	sm;	/* StepMode */
	DBLINK		osm;	/* SM OutLink */
	epicsEnum16	ar;	/* absRel */
	DBLINK		oar;	/* AR OutLink */
	epicsEnum16	aft;	/* After */
	DBLINK		oaft;	/* AFT OutLink */
	char		ppv[40];	/* PositionerPVName */
	char		rpv[40];	/* ReadbackPVName */
	char		dpv[40];	/* DetPVName */
	char		tpv[40];	/* TrigPVName */
	DBLINK		oppv;	/* P1PV OutLink */
	DBLINK		orpv;	/* R1PV OutLink */
	DBLINK		odpv;	/* D1PV OutLink */
	DBLINK		otpv;	/* T1PV OutLink */
	epicsFloat64	sp;	/* StartPos */
	DBLINK		osp;	/* SP OutLink */
	epicsFloat64	ep;	/* EndPos */
	DBLINK		oep;	/* EP OutLink */
	epicsInt32	np;	/* nPts */
	DBLINK		onp;	/* NP OutLink */
	epicsInt16	sc;	/* StartCmd */
	DBLINK		osc;	/* SC OutLink */
	epicsFloat64	aqt;	/* Acquire time */
	DBLINK		oaqt;	/* AQT OutLink */
	epicsInt32	mp;	/* MaxPts */
	DBLINK		imp;	/* MP InLink */
	epicsInt16	act;	/* ScanActive */
	DBLINK		iact;	/* InLink */
	epicsInt16	load;	/* Load */
	DBLINK		oload;	/* LOAD OutLink */
	epicsInt16	go;	/* Go */
	DBLINK		ogo;	/* GO OutLink */
	epicsFloat64	step;	/* StepSize */
	epicsFloat64	lstp;	/* Last stepSize */
} scanparmRecord;
#define scanparmRecordNAME	0
#define scanparmRecordDESC	1
#define scanparmRecordASG	2
#define scanparmRecordSCAN	3
#define scanparmRecordPINI	4
#define scanparmRecordPHAS	5
#define scanparmRecordEVNT	6
#define scanparmRecordTSE	7
#define scanparmRecordTSEL	8
#define scanparmRecordDTYP	9
#define scanparmRecordDISV	10
#define scanparmRecordDISA	11
#define scanparmRecordSDIS	12
#define scanparmRecordMLOK	13
#define scanparmRecordMLIS	14
#define scanparmRecordDISP	15
#define scanparmRecordPROC	16
#define scanparmRecordSTAT	17
#define scanparmRecordSEVR	18
#define scanparmRecordNSTA	19
#define scanparmRecordNSEV	20
#define scanparmRecordACKS	21
#define scanparmRecordACKT	22
#define scanparmRecordDISS	23
#define scanparmRecordLCNT	24
#define scanparmRecordPACT	25
#define scanparmRecordPUTF	26
#define scanparmRecordRPRO	27
#define scanparmRecordASP	28
#define scanparmRecordPPN	29
#define scanparmRecordPPNR	30
#define scanparmRecordSPVT	31
#define scanparmRecordRSET	32
#define scanparmRecordDSET	33
#define scanparmRecordDPVT	34
#define scanparmRecordRDES	35
#define scanparmRecordLSET	36
#define scanparmRecordPRIO	37
#define scanparmRecordTPRO	38
#define scanparmRecordBKPT	39
#define scanparmRecordUDF	40
#define scanparmRecordTIME	41
#define scanparmRecordFLNK	42
#define scanparmRecordVERS	43
#define scanparmRecordVAL	44
#define scanparmRecordPREC	45
#define scanparmRecordPRE	46
#define scanparmRecordOPRE	47
#define scanparmRecordSM	48
#define scanparmRecordOSM	49
#define scanparmRecordAR	50
#define scanparmRecordOAR	51
#define scanparmRecordAFT	52
#define scanparmRecordOAFT	53
#define scanparmRecordPPV	54
#define scanparmRecordRPV	55
#define scanparmRecordDPV	56
#define scanparmRecordTPV	57
#define scanparmRecordOPPV	58
#define scanparmRecordORPV	59
#define scanparmRecordODPV	60
#define scanparmRecordOTPV	61
#define scanparmRecordSP	62
#define scanparmRecordOSP	63
#define scanparmRecordEP	64
#define scanparmRecordOEP	65
#define scanparmRecordNP	66
#define scanparmRecordONP	67
#define scanparmRecordSC	68
#define scanparmRecordOSC	69
#define scanparmRecordAQT	70
#define scanparmRecordOAQT	71
#define scanparmRecordMP	72
#define scanparmRecordIMP	73
#define scanparmRecordACT	74
#define scanparmRecordIACT	75
#define scanparmRecordLOAD	76
#define scanparmRecordOLOAD	77
#define scanparmRecordGO	78
#define scanparmRecordOGO	79
#define scanparmRecordSTEP	80
#define scanparmRecordLSTP	81
#endif /*INCscanparmH*/
#ifdef GEN_SIZE_OFFSET
#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int scanparmRecordSizeOffset(dbRecordType *pdbRecordType)
{
    scanparmRecord *prec = 0;
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
  pdbRecordType->papFldDes[45]->size=sizeof(prec->prec);
  pdbRecordType->papFldDes[45]->offset=(short)((char *)&prec->prec - (char *)prec);
  pdbRecordType->papFldDes[46]->size=sizeof(prec->pre);
  pdbRecordType->papFldDes[46]->offset=(short)((char *)&prec->pre - (char *)prec);
  pdbRecordType->papFldDes[47]->size=sizeof(prec->opre);
  pdbRecordType->papFldDes[47]->offset=(short)((char *)&prec->opre - (char *)prec);
  pdbRecordType->papFldDes[48]->size=sizeof(prec->sm);
  pdbRecordType->papFldDes[48]->offset=(short)((char *)&prec->sm - (char *)prec);
  pdbRecordType->papFldDes[49]->size=sizeof(prec->osm);
  pdbRecordType->papFldDes[49]->offset=(short)((char *)&prec->osm - (char *)prec);
  pdbRecordType->papFldDes[50]->size=sizeof(prec->ar);
  pdbRecordType->papFldDes[50]->offset=(short)((char *)&prec->ar - (char *)prec);
  pdbRecordType->papFldDes[51]->size=sizeof(prec->oar);
  pdbRecordType->papFldDes[51]->offset=(short)((char *)&prec->oar - (char *)prec);
  pdbRecordType->papFldDes[52]->size=sizeof(prec->aft);
  pdbRecordType->papFldDes[52]->offset=(short)((char *)&prec->aft - (char *)prec);
  pdbRecordType->papFldDes[53]->size=sizeof(prec->oaft);
  pdbRecordType->papFldDes[53]->offset=(short)((char *)&prec->oaft - (char *)prec);
  pdbRecordType->papFldDes[54]->size=sizeof(prec->ppv);
  pdbRecordType->papFldDes[54]->offset=(short)((char *)&prec->ppv - (char *)prec);
  pdbRecordType->papFldDes[55]->size=sizeof(prec->rpv);
  pdbRecordType->papFldDes[55]->offset=(short)((char *)&prec->rpv - (char *)prec);
  pdbRecordType->papFldDes[56]->size=sizeof(prec->dpv);
  pdbRecordType->papFldDes[56]->offset=(short)((char *)&prec->dpv - (char *)prec);
  pdbRecordType->papFldDes[57]->size=sizeof(prec->tpv);
  pdbRecordType->papFldDes[57]->offset=(short)((char *)&prec->tpv - (char *)prec);
  pdbRecordType->papFldDes[58]->size=sizeof(prec->oppv);
  pdbRecordType->papFldDes[58]->offset=(short)((char *)&prec->oppv - (char *)prec);
  pdbRecordType->papFldDes[59]->size=sizeof(prec->orpv);
  pdbRecordType->papFldDes[59]->offset=(short)((char *)&prec->orpv - (char *)prec);
  pdbRecordType->papFldDes[60]->size=sizeof(prec->odpv);
  pdbRecordType->papFldDes[60]->offset=(short)((char *)&prec->odpv - (char *)prec);
  pdbRecordType->papFldDes[61]->size=sizeof(prec->otpv);
  pdbRecordType->papFldDes[61]->offset=(short)((char *)&prec->otpv - (char *)prec);
  pdbRecordType->papFldDes[62]->size=sizeof(prec->sp);
  pdbRecordType->papFldDes[62]->offset=(short)((char *)&prec->sp - (char *)prec);
  pdbRecordType->papFldDes[63]->size=sizeof(prec->osp);
  pdbRecordType->papFldDes[63]->offset=(short)((char *)&prec->osp - (char *)prec);
  pdbRecordType->papFldDes[64]->size=sizeof(prec->ep);
  pdbRecordType->papFldDes[64]->offset=(short)((char *)&prec->ep - (char *)prec);
  pdbRecordType->papFldDes[65]->size=sizeof(prec->oep);
  pdbRecordType->papFldDes[65]->offset=(short)((char *)&prec->oep - (char *)prec);
  pdbRecordType->papFldDes[66]->size=sizeof(prec->np);
  pdbRecordType->papFldDes[66]->offset=(short)((char *)&prec->np - (char *)prec);
  pdbRecordType->papFldDes[67]->size=sizeof(prec->onp);
  pdbRecordType->papFldDes[67]->offset=(short)((char *)&prec->onp - (char *)prec);
  pdbRecordType->papFldDes[68]->size=sizeof(prec->sc);
  pdbRecordType->papFldDes[68]->offset=(short)((char *)&prec->sc - (char *)prec);
  pdbRecordType->papFldDes[69]->size=sizeof(prec->osc);
  pdbRecordType->papFldDes[69]->offset=(short)((char *)&prec->osc - (char *)prec);
  pdbRecordType->papFldDes[70]->size=sizeof(prec->aqt);
  pdbRecordType->papFldDes[70]->offset=(short)((char *)&prec->aqt - (char *)prec);
  pdbRecordType->papFldDes[71]->size=sizeof(prec->oaqt);
  pdbRecordType->papFldDes[71]->offset=(short)((char *)&prec->oaqt - (char *)prec);
  pdbRecordType->papFldDes[72]->size=sizeof(prec->mp);
  pdbRecordType->papFldDes[72]->offset=(short)((char *)&prec->mp - (char *)prec);
  pdbRecordType->papFldDes[73]->size=sizeof(prec->imp);
  pdbRecordType->papFldDes[73]->offset=(short)((char *)&prec->imp - (char *)prec);
  pdbRecordType->papFldDes[74]->size=sizeof(prec->act);
  pdbRecordType->papFldDes[74]->offset=(short)((char *)&prec->act - (char *)prec);
  pdbRecordType->papFldDes[75]->size=sizeof(prec->iact);
  pdbRecordType->papFldDes[75]->offset=(short)((char *)&prec->iact - (char *)prec);
  pdbRecordType->papFldDes[76]->size=sizeof(prec->load);
  pdbRecordType->papFldDes[76]->offset=(short)((char *)&prec->load - (char *)prec);
  pdbRecordType->papFldDes[77]->size=sizeof(prec->oload);
  pdbRecordType->papFldDes[77]->offset=(short)((char *)&prec->oload - (char *)prec);
  pdbRecordType->papFldDes[78]->size=sizeof(prec->go);
  pdbRecordType->papFldDes[78]->offset=(short)((char *)&prec->go - (char *)prec);
  pdbRecordType->papFldDes[79]->size=sizeof(prec->ogo);
  pdbRecordType->papFldDes[79]->offset=(short)((char *)&prec->ogo - (char *)prec);
  pdbRecordType->papFldDes[80]->size=sizeof(prec->step);
  pdbRecordType->papFldDes[80]->offset=(short)((char *)&prec->step - (char *)prec);
  pdbRecordType->papFldDes[81]->size=sizeof(prec->lstp);
  pdbRecordType->papFldDes[81]->offset=(short)((char *)&prec->lstp - (char *)prec);
    pdbRecordType->rec_size = sizeof(*prec);
    return(0);
}
epicsExportRegistrar(scanparmRecordSizeOffset);
#ifdef __cplusplus
}
#endif
#endif /*GEN_SIZE_OFFSET*/
