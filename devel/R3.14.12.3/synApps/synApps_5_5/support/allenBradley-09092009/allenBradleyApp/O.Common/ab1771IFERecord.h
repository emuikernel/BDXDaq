
#ifndef INCab1771IFE_SEDIH
#define INCab1771IFE_SEDIH
typedef enum {
	ab1771IFE_SE,
	ab1771IFE_DI
}ab1771IFE_SEDI;
#endif /*INCab1771IFE_SEDIH*/

#ifndef INCab1771IFE_RANGEH
#define INCab1771IFE_RANGEH
typedef enum {
	ab1771IFE_RANGE_0,
	ab1771IFE_RANGE_1,
	ab1771IFE_RANGE_2,
	ab1771IFE_RANGE_3
}ab1771IFE_RANGE;
#endif /*INCab1771IFE_RANGEH*/

#ifndef INCab1771IFE_CMDH
#define INCab1771IFE_CMDH
typedef enum {
	ab1771IFE_CMD_None,
	ab1771IFE_Init
}ab1771IFE_CMD;
#endif /*INCab1771IFE_CMDH*/
#ifndef INCab1771IFEH
#define INCab1771IFEH
#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"
typedef struct ab1771IFERecord {
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
	epicsEnum16	sedi;	/* Single/Double Ended */
	epicsInt16	filt;	/* Filter 00to99 */
	epicsInt16	stim;	/* Sample Time Period */
	epicsEnum16	r0;	/* Range signal 0 */
	epicsEnum16	r1;	/* Range signal 1 */
	epicsEnum16	r2;	/* Range signal 2 */
	epicsEnum16	r3;	/* Range signal 3 */
	epicsEnum16	r4;	/* Range signal 4 */
	epicsEnum16	r5;	/* Range signal 5 */
	epicsEnum16	r6;	/* Range signal 6 */
	epicsEnum16	r7;	/* Range signal 7 */
	epicsEnum16	r8;	/* Range signal 8 */
	epicsEnum16	r9;	/* Range signal 9 */
	epicsEnum16	r10;	/* Range signal 10 */
	epicsEnum16	r11;	/* Range signal 11 */
	epicsEnum16	r12;	/* Range signal 12 */
	epicsEnum16	r13;	/* Range signal 13 */
	epicsEnum16	r14;	/* Range signal 14 */
	epicsEnum16	r15;	/* Range signal 15 */
} ab1771IFERecord;
#define ab1771IFERecordNAME	0
#define ab1771IFERecordDESC	1
#define ab1771IFERecordASG	2
#define ab1771IFERecordSCAN	3
#define ab1771IFERecordPINI	4
#define ab1771IFERecordPHAS	5
#define ab1771IFERecordEVNT	6
#define ab1771IFERecordTSE	7
#define ab1771IFERecordTSEL	8
#define ab1771IFERecordDTYP	9
#define ab1771IFERecordDISV	10
#define ab1771IFERecordDISA	11
#define ab1771IFERecordSDIS	12
#define ab1771IFERecordMLOK	13
#define ab1771IFERecordMLIS	14
#define ab1771IFERecordDISP	15
#define ab1771IFERecordPROC	16
#define ab1771IFERecordSTAT	17
#define ab1771IFERecordSEVR	18
#define ab1771IFERecordNSTA	19
#define ab1771IFERecordNSEV	20
#define ab1771IFERecordACKS	21
#define ab1771IFERecordACKT	22
#define ab1771IFERecordDISS	23
#define ab1771IFERecordLCNT	24
#define ab1771IFERecordPACT	25
#define ab1771IFERecordPUTF	26
#define ab1771IFERecordRPRO	27
#define ab1771IFERecordASP	28
#define ab1771IFERecordPPN	29
#define ab1771IFERecordPPNR	30
#define ab1771IFERecordSPVT	31
#define ab1771IFERecordRSET	32
#define ab1771IFERecordDSET	33
#define ab1771IFERecordDPVT	34
#define ab1771IFERecordRDES	35
#define ab1771IFERecordLSET	36
#define ab1771IFERecordPRIO	37
#define ab1771IFERecordTPRO	38
#define ab1771IFERecordBKPT	39
#define ab1771IFERecordUDF	40
#define ab1771IFERecordTIME	41
#define ab1771IFERecordFLNK	42
#define ab1771IFERecordVAL	43
#define ab1771IFERecordCMD	44
#define ab1771IFERecordLOCA	45
#define ab1771IFERecordLINK	46
#define ab1771IFERecordRACK	47
#define ab1771IFERecordSLOT	48
#define ab1771IFERecordOUTM	49
#define ab1771IFERecordINPM	50
#define ab1771IFERecordIAI1	51
#define ab1771IFERecordSEDI	52
#define ab1771IFERecordFILT	53
#define ab1771IFERecordSTIM	54
#define ab1771IFERecordR0	55
#define ab1771IFERecordR1	56
#define ab1771IFERecordR2	57
#define ab1771IFERecordR3	58
#define ab1771IFERecordR4	59
#define ab1771IFERecordR5	60
#define ab1771IFERecordR6	61
#define ab1771IFERecordR7	62
#define ab1771IFERecordR8	63
#define ab1771IFERecordR9	64
#define ab1771IFERecordR10	65
#define ab1771IFERecordR11	66
#define ab1771IFERecordR12	67
#define ab1771IFERecordR13	68
#define ab1771IFERecordR14	69
#define ab1771IFERecordR15	70
#endif /*INCab1771IFEH*/
#ifdef GEN_SIZE_OFFSET
#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int ab1771IFERecordSizeOffset(dbRecordType *pdbRecordType)
{
    ab1771IFERecord *prec = 0;
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
  pdbRecordType->papFldDes[52]->size=sizeof(prec->sedi);
  pdbRecordType->papFldDes[52]->offset=(short)((char *)&prec->sedi - (char *)prec);
  pdbRecordType->papFldDes[53]->size=sizeof(prec->filt);
  pdbRecordType->papFldDes[53]->offset=(short)((char *)&prec->filt - (char *)prec);
  pdbRecordType->papFldDes[54]->size=sizeof(prec->stim);
  pdbRecordType->papFldDes[54]->offset=(short)((char *)&prec->stim - (char *)prec);
  pdbRecordType->papFldDes[55]->size=sizeof(prec->r0);
  pdbRecordType->papFldDes[55]->offset=(short)((char *)&prec->r0 - (char *)prec);
  pdbRecordType->papFldDes[56]->size=sizeof(prec->r1);
  pdbRecordType->papFldDes[56]->offset=(short)((char *)&prec->r1 - (char *)prec);
  pdbRecordType->papFldDes[57]->size=sizeof(prec->r2);
  pdbRecordType->papFldDes[57]->offset=(short)((char *)&prec->r2 - (char *)prec);
  pdbRecordType->papFldDes[58]->size=sizeof(prec->r3);
  pdbRecordType->papFldDes[58]->offset=(short)((char *)&prec->r3 - (char *)prec);
  pdbRecordType->papFldDes[59]->size=sizeof(prec->r4);
  pdbRecordType->papFldDes[59]->offset=(short)((char *)&prec->r4 - (char *)prec);
  pdbRecordType->papFldDes[60]->size=sizeof(prec->r5);
  pdbRecordType->papFldDes[60]->offset=(short)((char *)&prec->r5 - (char *)prec);
  pdbRecordType->papFldDes[61]->size=sizeof(prec->r6);
  pdbRecordType->papFldDes[61]->offset=(short)((char *)&prec->r6 - (char *)prec);
  pdbRecordType->papFldDes[62]->size=sizeof(prec->r7);
  pdbRecordType->papFldDes[62]->offset=(short)((char *)&prec->r7 - (char *)prec);
  pdbRecordType->papFldDes[63]->size=sizeof(prec->r8);
  pdbRecordType->papFldDes[63]->offset=(short)((char *)&prec->r8 - (char *)prec);
  pdbRecordType->papFldDes[64]->size=sizeof(prec->r9);
  pdbRecordType->papFldDes[64]->offset=(short)((char *)&prec->r9 - (char *)prec);
  pdbRecordType->papFldDes[65]->size=sizeof(prec->r10);
  pdbRecordType->papFldDes[65]->offset=(short)((char *)&prec->r10 - (char *)prec);
  pdbRecordType->papFldDes[66]->size=sizeof(prec->r11);
  pdbRecordType->papFldDes[66]->offset=(short)((char *)&prec->r11 - (char *)prec);
  pdbRecordType->papFldDes[67]->size=sizeof(prec->r12);
  pdbRecordType->papFldDes[67]->offset=(short)((char *)&prec->r12 - (char *)prec);
  pdbRecordType->papFldDes[68]->size=sizeof(prec->r13);
  pdbRecordType->papFldDes[68]->offset=(short)((char *)&prec->r13 - (char *)prec);
  pdbRecordType->papFldDes[69]->size=sizeof(prec->r14);
  pdbRecordType->papFldDes[69]->offset=(short)((char *)&prec->r14 - (char *)prec);
  pdbRecordType->papFldDes[70]->size=sizeof(prec->r15);
  pdbRecordType->papFldDes[70]->offset=(short)((char *)&prec->r15 - (char *)prec);
    pdbRecordType->rec_size = sizeof(*prec);
    return(0);
}
epicsExportRegistrar(ab1771IFERecordSizeOffset);
#ifdef __cplusplus
}
#endif
#endif /*GEN_SIZE_OFFSET*/
