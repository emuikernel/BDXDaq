
#ifndef INCab1771IX_UnitsH
#define INCab1771IX_UnitsH
typedef enum {
	ab1771IX_UnitsC,
	ab1771IX_UnitsF
}ab1771IX_Units;
#endif /*INCab1771IX_UnitsH*/

#ifndef INCab1771IX_TYPEH
#define INCab1771IX_TYPEH
typedef enum {
	ab1771IX_TYPE_MV,
	ab1771IX_TYPE_tcB,
	ab1771IX_TYPE_tcE,
	ab1771IX_TYPE_tcJ,
	ab1771IX_TYPE_tcK,
	ab1771IX_TYPE_tcR,
	ab1771IX_TYPE_tcS,
	ab1771IX_TYPE_tcT
}ab1771IX_TYPE;
#endif /*INCab1771IX_TYPEH*/

#ifndef INCab1771IX_STAH
#define INCab1771IX_STAH
typedef enum {
	ab1771IX_STA_Ok,
	ab1771IX_STA_Under,
	ab1771IX_STA_Over
}ab1771IX_STA;
#endif /*INCab1771IX_STAH*/

#ifndef INCab1771IX_MTYPH
#define INCab1771IX_MTYPH
typedef enum {
	ab1771IX_HR,
	ab1771IX_E
}ab1771IX_MTYP;
#endif /*INCab1771IX_MTYPH*/

#ifndef INCab1771IX_CMDH
#define INCab1771IX_CMDH
typedef enum {
	ab1771IX_CMD_None,
	ab1771IX_Init
}ab1771IX_CMD;
#endif /*INCab1771IX_CMDH*/
#ifndef INCab1771IXH
#define INCab1771IXH
#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"
typedef struct ab1771IXRecord {
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
	char		val[40];	/* Status */
	epicsEnum16	cmd;	/* Command */
	epicsEnum16	loca;	/* scanIoRequest comm err */
	epicsUInt16	link;	/* Scanner Number */
	epicsUInt16	rack;	/* Rack */
	epicsUInt16	slot;	/* Slot */
	short	*outm;	/* Output Message */
	short	*inpm;	/* Input Message */
	void	*iai1;	/* Interface for ai */
	epicsEnum16	mtyp;	/* Module Type */
	epicsEnum16	typa;	/* Chan 1-4 type */
	epicsEnum16	typb;	/* Chan 5-8 type */
	epicsEnum16	scal;	/* Temperature Scale */
	epicsInt16	stim;	/* Sample Time Period */
	epicsEnum16	zoom;	/* Enable zoom for millivolt */
	epicsInt16	zca;	/* Zoom center channels 1-4 */
	epicsInt16	zcb;	/* Zoom center channels 5-8 */
	epicsInt16	fila;	/* Filter value channels 1-4 */
	epicsInt16	filb;	/* Filter value channels 5-8 */
	epicsFloat32	cjt;	/* Cold Junction Temperature  */
	epicsEnum16	sta1;	/* Chan 1 status */
	epicsInt16	raw1;	/* Chan 1 raw value */
	epicsEnum16	sta2;	/* Chan 2 status */
	epicsInt16	raw2;	/* Chan 2 raw value */
	epicsEnum16	sta3;	/* Chan 3 status */
	epicsInt16	raw3;	/* Chan 3 raw value */
	epicsEnum16	sta4;	/* Chan 4 status */
	epicsInt16	raw4;	/* Chan 4 raw value */
	epicsEnum16	sta5;	/* Chan 5 status */
	epicsInt16	raw5;	/* Chan 5 raw value */
	epicsEnum16	sta6;	/* Chan 6 status */
	epicsInt16	raw6;	/* Chan 6 raw value */
	epicsEnum16	sta7;	/* Chan 7 status */
	epicsInt16	raw7;	/* Chan 7 raw value */
	epicsEnum16	sta8;	/* Chan 8 status */
	epicsInt16	raw8;	/* Chan 8 raw value */
} ab1771IXRecord;
#define ab1771IXRecordNAME	0
#define ab1771IXRecordDESC	1
#define ab1771IXRecordASG	2
#define ab1771IXRecordSCAN	3
#define ab1771IXRecordPINI	4
#define ab1771IXRecordPHAS	5
#define ab1771IXRecordEVNT	6
#define ab1771IXRecordTSE	7
#define ab1771IXRecordTSEL	8
#define ab1771IXRecordDTYP	9
#define ab1771IXRecordDISV	10
#define ab1771IXRecordDISA	11
#define ab1771IXRecordSDIS	12
#define ab1771IXRecordMLOK	13
#define ab1771IXRecordMLIS	14
#define ab1771IXRecordDISP	15
#define ab1771IXRecordPROC	16
#define ab1771IXRecordSTAT	17
#define ab1771IXRecordSEVR	18
#define ab1771IXRecordNSTA	19
#define ab1771IXRecordNSEV	20
#define ab1771IXRecordACKS	21
#define ab1771IXRecordACKT	22
#define ab1771IXRecordDISS	23
#define ab1771IXRecordLCNT	24
#define ab1771IXRecordPACT	25
#define ab1771IXRecordPUTF	26
#define ab1771IXRecordRPRO	27
#define ab1771IXRecordASP	28
#define ab1771IXRecordPPN	29
#define ab1771IXRecordPPNR	30
#define ab1771IXRecordSPVT	31
#define ab1771IXRecordRSET	32
#define ab1771IXRecordDSET	33
#define ab1771IXRecordDPVT	34
#define ab1771IXRecordRDES	35
#define ab1771IXRecordLSET	36
#define ab1771IXRecordPRIO	37
#define ab1771IXRecordTPRO	38
#define ab1771IXRecordBKPT	39
#define ab1771IXRecordUDF	40
#define ab1771IXRecordTIME	41
#define ab1771IXRecordFLNK	42
#define ab1771IXRecordVAL	43
#define ab1771IXRecordCMD	44
#define ab1771IXRecordLOCA	45
#define ab1771IXRecordLINK	46
#define ab1771IXRecordRACK	47
#define ab1771IXRecordSLOT	48
#define ab1771IXRecordOUTM	49
#define ab1771IXRecordINPM	50
#define ab1771IXRecordIAI1	51
#define ab1771IXRecordMTYP	52
#define ab1771IXRecordTYPA	53
#define ab1771IXRecordTYPB	54
#define ab1771IXRecordSCAL	55
#define ab1771IXRecordSTIM	56
#define ab1771IXRecordZOOM	57
#define ab1771IXRecordZCA	58
#define ab1771IXRecordZCB	59
#define ab1771IXRecordFILA	60
#define ab1771IXRecordFILB	61
#define ab1771IXRecordCJT	62
#define ab1771IXRecordSTA1	63
#define ab1771IXRecordRAW1	64
#define ab1771IXRecordSTA2	65
#define ab1771IXRecordRAW2	66
#define ab1771IXRecordSTA3	67
#define ab1771IXRecordRAW3	68
#define ab1771IXRecordSTA4	69
#define ab1771IXRecordRAW4	70
#define ab1771IXRecordSTA5	71
#define ab1771IXRecordRAW5	72
#define ab1771IXRecordSTA6	73
#define ab1771IXRecordRAW6	74
#define ab1771IXRecordSTA7	75
#define ab1771IXRecordRAW7	76
#define ab1771IXRecordSTA8	77
#define ab1771IXRecordRAW8	78
#endif /*INCab1771IXH*/
#ifdef GEN_SIZE_OFFSET
#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int ab1771IXRecordSizeOffset(dbRecordType *pdbRecordType)
{
    ab1771IXRecord *prec = 0;
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
  pdbRecordType->papFldDes[43]->size=sizeof(prec->val);
  pdbRecordType->papFldDes[43]->offset=(short)((char *)&prec->val - (char *)prec);
  pdbRecordType->papFldDes[44]->size=sizeof(prec->cmd);
  pdbRecordType->papFldDes[44]->offset=(short)((char *)&prec->cmd - (char *)prec);
  pdbRecordType->papFldDes[45]->size=sizeof(prec->loca);
  pdbRecordType->papFldDes[45]->offset=(short)((char *)&prec->loca - (char *)prec);
  pdbRecordType->papFldDes[46]->size=sizeof(prec->link);
  pdbRecordType->papFldDes[46]->offset=(short)((char *)&prec->link - (char *)prec);
  pdbRecordType->papFldDes[47]->size=sizeof(prec->rack);
  pdbRecordType->papFldDes[47]->offset=(short)((char *)&prec->rack - (char *)prec);
  pdbRecordType->papFldDes[48]->size=sizeof(prec->slot);
  pdbRecordType->papFldDes[48]->offset=(short)((char *)&prec->slot - (char *)prec);
  pdbRecordType->papFldDes[49]->size=sizeof(prec->outm);
  pdbRecordType->papFldDes[49]->offset=(short)((char *)&prec->outm - (char *)prec);
  pdbRecordType->papFldDes[50]->size=sizeof(prec->inpm);
  pdbRecordType->papFldDes[50]->offset=(short)((char *)&prec->inpm - (char *)prec);
  pdbRecordType->papFldDes[51]->size=sizeof(prec->iai1);
  pdbRecordType->papFldDes[51]->offset=(short)((char *)&prec->iai1 - (char *)prec);
  pdbRecordType->papFldDes[52]->size=sizeof(prec->mtyp);
  pdbRecordType->papFldDes[52]->offset=(short)((char *)&prec->mtyp - (char *)prec);
  pdbRecordType->papFldDes[53]->size=sizeof(prec->typa);
  pdbRecordType->papFldDes[53]->offset=(short)((char *)&prec->typa - (char *)prec);
  pdbRecordType->papFldDes[54]->size=sizeof(prec->typb);
  pdbRecordType->papFldDes[54]->offset=(short)((char *)&prec->typb - (char *)prec);
  pdbRecordType->papFldDes[55]->size=sizeof(prec->scal);
  pdbRecordType->papFldDes[55]->offset=(short)((char *)&prec->scal - (char *)prec);
  pdbRecordType->papFldDes[56]->size=sizeof(prec->stim);
  pdbRecordType->papFldDes[56]->offset=(short)((char *)&prec->stim - (char *)prec);
  pdbRecordType->papFldDes[57]->size=sizeof(prec->zoom);
  pdbRecordType->papFldDes[57]->offset=(short)((char *)&prec->zoom - (char *)prec);
  pdbRecordType->papFldDes[58]->size=sizeof(prec->zca);
  pdbRecordType->papFldDes[58]->offset=(short)((char *)&prec->zca - (char *)prec);
  pdbRecordType->papFldDes[59]->size=sizeof(prec->zcb);
  pdbRecordType->papFldDes[59]->offset=(short)((char *)&prec->zcb - (char *)prec);
  pdbRecordType->papFldDes[60]->size=sizeof(prec->fila);
  pdbRecordType->papFldDes[60]->offset=(short)((char *)&prec->fila - (char *)prec);
  pdbRecordType->papFldDes[61]->size=sizeof(prec->filb);
  pdbRecordType->papFldDes[61]->offset=(short)((char *)&prec->filb - (char *)prec);
  pdbRecordType->papFldDes[62]->size=sizeof(prec->cjt);
  pdbRecordType->papFldDes[62]->offset=(short)((char *)&prec->cjt - (char *)prec);
  pdbRecordType->papFldDes[63]->size=sizeof(prec->sta1);
  pdbRecordType->papFldDes[63]->offset=(short)((char *)&prec->sta1 - (char *)prec);
  pdbRecordType->papFldDes[64]->size=sizeof(prec->raw1);
  pdbRecordType->papFldDes[64]->offset=(short)((char *)&prec->raw1 - (char *)prec);
  pdbRecordType->papFldDes[65]->size=sizeof(prec->sta2);
  pdbRecordType->papFldDes[65]->offset=(short)((char *)&prec->sta2 - (char *)prec);
  pdbRecordType->papFldDes[66]->size=sizeof(prec->raw2);
  pdbRecordType->papFldDes[66]->offset=(short)((char *)&prec->raw2 - (char *)prec);
  pdbRecordType->papFldDes[67]->size=sizeof(prec->sta3);
  pdbRecordType->papFldDes[67]->offset=(short)((char *)&prec->sta3 - (char *)prec);
  pdbRecordType->papFldDes[68]->size=sizeof(prec->raw3);
  pdbRecordType->papFldDes[68]->offset=(short)((char *)&prec->raw3 - (char *)prec);
  pdbRecordType->papFldDes[69]->size=sizeof(prec->sta4);
  pdbRecordType->papFldDes[69]->offset=(short)((char *)&prec->sta4 - (char *)prec);
  pdbRecordType->papFldDes[70]->size=sizeof(prec->raw4);
  pdbRecordType->papFldDes[70]->offset=(short)((char *)&prec->raw4 - (char *)prec);
  pdbRecordType->papFldDes[71]->size=sizeof(prec->sta5);
  pdbRecordType->papFldDes[71]->offset=(short)((char *)&prec->sta5 - (char *)prec);
  pdbRecordType->papFldDes[72]->size=sizeof(prec->raw5);
  pdbRecordType->papFldDes[72]->offset=(short)((char *)&prec->raw5 - (char *)prec);
  pdbRecordType->papFldDes[73]->size=sizeof(prec->sta6);
  pdbRecordType->papFldDes[73]->offset=(short)((char *)&prec->sta6 - (char *)prec);
  pdbRecordType->papFldDes[74]->size=sizeof(prec->raw6);
  pdbRecordType->papFldDes[74]->offset=(short)((char *)&prec->raw6 - (char *)prec);
  pdbRecordType->papFldDes[75]->size=sizeof(prec->sta7);
  pdbRecordType->papFldDes[75]->offset=(short)((char *)&prec->sta7 - (char *)prec);
  pdbRecordType->papFldDes[76]->size=sizeof(prec->raw7);
  pdbRecordType->papFldDes[76]->offset=(short)((char *)&prec->raw7 - (char *)prec);
  pdbRecordType->papFldDes[77]->size=sizeof(prec->sta8);
  pdbRecordType->papFldDes[77]->offset=(short)((char *)&prec->sta8 - (char *)prec);
  pdbRecordType->papFldDes[78]->size=sizeof(prec->raw8);
  pdbRecordType->papFldDes[78]->offset=(short)((char *)&prec->raw8 - (char *)prec);
    pdbRecordType->rec_size = sizeof(*prec);
    return(0);
}
epicsExportRegistrar(ab1771IXRecordSizeOffset);
#ifdef __cplusplus
}
#endif
#endif /*GEN_SIZE_OFFSET*/
