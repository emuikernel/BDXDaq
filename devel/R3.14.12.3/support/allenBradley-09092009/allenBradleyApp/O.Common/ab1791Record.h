
#ifndef INCab1791TYPEH
#define INCab1791TYPEH
typedef enum {
	ab1791TYPE_Voltage,
	ab1791TYPE_Current
}ab1791TYPE;
#endif /*INCab1791TYPEH*/

#ifndef INCab1791RANGH
#define INCab1791RANGH
typedef enum {
	ab1791RANG__10_to_10,
	ab1791RANG__5_to_5,
	ab1791RANG_0_to_10,
	ab1791RANG_0_to_5
}ab1791RANG;
#endif /*INCab1791RANGH*/

#ifndef INCab1791FILTH
#define INCab1791FILTH
typedef enum {
	ab1791FILT_None,
	ab1791FILT_200ms,
	ab1791FILT_300ms,
	ab1791FILT_400ms,
	ab1791FILT_500ms,
	ab1791FILT_600ms,
	ab1791FILT_700ms,
	ab1791FILT_800ms,
	ab1791FILT_900ms,
	ab1791FILT_1000ms,
	ab1791FILT_1100ms,
	ab1791FILT_1200ms,
	ab1791FILT_1300ms,
	ab1791FILT_1400ms,
	ab1791FILT_1500ms
}ab1791FILT;
#endif /*INCab1791FILTH*/
#ifndef INCab1791H
#define INCab1791H
#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"
typedef struct ab1791Record {
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
	epicsInt16	val;	/* Value - Not Used */
	epicsEnum16	type;	/* Type */
	epicsEnum16	rang;	/* Range */
	epicsEnum16	filt;	/* Filter */
	char		lkst[40];	/* Link Status */
	epicsEnum16	loca;	/* scanIoRequest comm err */
	epicsUInt16	link;	/* Scanner Number */
	epicsUInt16	rack;	/* Rack */
	epicsUInt16	slot;	/* Slot */
	epicsInt16	nfai;	/* Number Failures */
	epicsInt16	inp0;	/* Input Channel 0 */
	epicsInt16	inp1;	/* Input Channel 1 */
	epicsInt16	inp2;	/* Input Channel 2 */
	epicsInt16	inp3;	/* Input Channel 3 */
	epicsInt16	out0;	/* Output Channel 0 */
	epicsInt16	out1;	/* Output Channel 1 */
} ab1791Record;
#define ab1791RecordNAME	0
#define ab1791RecordDESC	1
#define ab1791RecordASG	2
#define ab1791RecordSCAN	3
#define ab1791RecordPINI	4
#define ab1791RecordPHAS	5
#define ab1791RecordEVNT	6
#define ab1791RecordTSE	7
#define ab1791RecordTSEL	8
#define ab1791RecordDTYP	9
#define ab1791RecordDISV	10
#define ab1791RecordDISA	11
#define ab1791RecordSDIS	12
#define ab1791RecordMLOK	13
#define ab1791RecordMLIS	14
#define ab1791RecordDISP	15
#define ab1791RecordPROC	16
#define ab1791RecordSTAT	17
#define ab1791RecordSEVR	18
#define ab1791RecordNSTA	19
#define ab1791RecordNSEV	20
#define ab1791RecordACKS	21
#define ab1791RecordACKT	22
#define ab1791RecordDISS	23
#define ab1791RecordLCNT	24
#define ab1791RecordPACT	25
#define ab1791RecordPUTF	26
#define ab1791RecordRPRO	27
#define ab1791RecordASP	28
#define ab1791RecordPPN	29
#define ab1791RecordPPNR	30
#define ab1791RecordSPVT	31
#define ab1791RecordRSET	32
#define ab1791RecordDSET	33
#define ab1791RecordDPVT	34
#define ab1791RecordRDES	35
#define ab1791RecordLSET	36
#define ab1791RecordPRIO	37
#define ab1791RecordTPRO	38
#define ab1791RecordBKPT	39
#define ab1791RecordUDF	40
#define ab1791RecordTIME	41
#define ab1791RecordFLNK	42
#define ab1791RecordVAL	43
#define ab1791RecordTYPE	44
#define ab1791RecordRANG	45
#define ab1791RecordFILT	46
#define ab1791RecordLKST	47
#define ab1791RecordLOCA	48
#define ab1791RecordLINK	49
#define ab1791RecordRACK	50
#define ab1791RecordSLOT	51
#define ab1791RecordNFAI	52
#define ab1791RecordINP0	53
#define ab1791RecordINP1	54
#define ab1791RecordINP2	55
#define ab1791RecordINP3	56
#define ab1791RecordOUT0	57
#define ab1791RecordOUT1	58
#endif /*INCab1791H*/
#ifdef GEN_SIZE_OFFSET
#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int ab1791RecordSizeOffset(dbRecordType *pdbRecordType)
{
    ab1791Record *prec = 0;
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
  pdbRecordType->papFldDes[44]->size=sizeof(prec->type);
  pdbRecordType->papFldDes[44]->offset=(short)((char *)&prec->type - (char *)prec);
  pdbRecordType->papFldDes[45]->size=sizeof(prec->rang);
  pdbRecordType->papFldDes[45]->offset=(short)((char *)&prec->rang - (char *)prec);
  pdbRecordType->papFldDes[46]->size=sizeof(prec->filt);
  pdbRecordType->papFldDes[46]->offset=(short)((char *)&prec->filt - (char *)prec);
  pdbRecordType->papFldDes[47]->size=sizeof(prec->lkst);
  pdbRecordType->papFldDes[47]->offset=(short)((char *)&prec->lkst - (char *)prec);
  pdbRecordType->papFldDes[48]->size=sizeof(prec->loca);
  pdbRecordType->papFldDes[48]->offset=(short)((char *)&prec->loca - (char *)prec);
  pdbRecordType->papFldDes[49]->size=sizeof(prec->link);
  pdbRecordType->papFldDes[49]->offset=(short)((char *)&prec->link - (char *)prec);
  pdbRecordType->papFldDes[50]->size=sizeof(prec->rack);
  pdbRecordType->papFldDes[50]->offset=(short)((char *)&prec->rack - (char *)prec);
  pdbRecordType->papFldDes[51]->size=sizeof(prec->slot);
  pdbRecordType->papFldDes[51]->offset=(short)((char *)&prec->slot - (char *)prec);
  pdbRecordType->papFldDes[52]->size=sizeof(prec->nfai);
  pdbRecordType->papFldDes[52]->offset=(short)((char *)&prec->nfai - (char *)prec);
  pdbRecordType->papFldDes[53]->size=sizeof(prec->inp0);
  pdbRecordType->papFldDes[53]->offset=(short)((char *)&prec->inp0 - (char *)prec);
  pdbRecordType->papFldDes[54]->size=sizeof(prec->inp1);
  pdbRecordType->papFldDes[54]->offset=(short)((char *)&prec->inp1 - (char *)prec);
  pdbRecordType->papFldDes[55]->size=sizeof(prec->inp2);
  pdbRecordType->papFldDes[55]->offset=(short)((char *)&prec->inp2 - (char *)prec);
  pdbRecordType->papFldDes[56]->size=sizeof(prec->inp3);
  pdbRecordType->papFldDes[56]->offset=(short)((char *)&prec->inp3 - (char *)prec);
  pdbRecordType->papFldDes[57]->size=sizeof(prec->out0);
  pdbRecordType->papFldDes[57]->offset=(short)((char *)&prec->out0 - (char *)prec);
  pdbRecordType->papFldDes[58]->size=sizeof(prec->out1);
  pdbRecordType->papFldDes[58]->offset=(short)((char *)&prec->out1 - (char *)prec);
    pdbRecordType->rec_size = sizeof(*prec);
    return(0);
}
epicsExportRegistrar(ab1791RecordSizeOffset);
#ifdef __cplusplus
}
#endif
#endif /*GEN_SIZE_OFFSET*/
