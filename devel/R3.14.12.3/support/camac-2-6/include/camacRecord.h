
#ifndef INCcamacTMODH
#define INCcamacTMODH
typedef enum {
	camacTMOD_Single,
	camacTMOD_Q_Stop,
	camacTMOD_Q_Repeat,
	camacTMOD_Q_Scan
}camacTMOD;
#endif /*INCcamacTMODH*/

#ifndef INCcamacCCMDH
#define INCcamacCCMDH
typedef enum {
	camacCCMD_None,
	camacCCMD_Clear_Inhibit,
	camacCCMD_Set_Inhibit,
	camacCCMD_Clear__C_,
	camacCCMD_Initialize__Z_
}camacCCMD;
#endif /*INCcamacCCMDH*/
#ifndef INCcamacH
#define INCcamacH
#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"
typedef struct camacRecord {
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
	epicsInt32	val;	/* Current value */
	void *bptr;	/* Buffer Pointer */
	epicsInt32	q;	/* Q  response */
	epicsInt32	x;	/* X  response */
	epicsInt32	inhb;	/* Inhibit status */
	epicsInt32	b;	/* Branch */
	epicsInt32	c;	/* Crate */
	epicsInt32	n;	/* Station */
	epicsInt32	a;	/* Subaddress */
	epicsInt32	f;	/* Function */
	epicsEnum16	tmod;	/* Transfer mode */
	epicsInt32	nmax;	/* Max. number of values */
	epicsInt32	nuse;	/* Number of values to R/W */
	epicsInt32	nact;	/* Actual number of values */
	epicsEnum16	ccmd;	/* Crate command */
} camacRecord;
#define camacRecordNAME	0
#define camacRecordDESC	1
#define camacRecordASG	2
#define camacRecordSCAN	3
#define camacRecordPINI	4
#define camacRecordPHAS	5
#define camacRecordEVNT	6
#define camacRecordTSE	7
#define camacRecordTSEL	8
#define camacRecordDTYP	9
#define camacRecordDISV	10
#define camacRecordDISA	11
#define camacRecordSDIS	12
#define camacRecordMLOK	13
#define camacRecordMLIS	14
#define camacRecordDISP	15
#define camacRecordPROC	16
#define camacRecordSTAT	17
#define camacRecordSEVR	18
#define camacRecordNSTA	19
#define camacRecordNSEV	20
#define camacRecordACKS	21
#define camacRecordACKT	22
#define camacRecordDISS	23
#define camacRecordLCNT	24
#define camacRecordPACT	25
#define camacRecordPUTF	26
#define camacRecordRPRO	27
#define camacRecordASP	28
#define camacRecordPPN	29
#define camacRecordPPNR	30
#define camacRecordSPVT	31
#define camacRecordRSET	32
#define camacRecordDSET	33
#define camacRecordDPVT	34
#define camacRecordRDES	35
#define camacRecordLSET	36
#define camacRecordPRIO	37
#define camacRecordTPRO	38
#define camacRecordBKPT	39
#define camacRecordUDF	40
#define camacRecordTIME	41
#define camacRecordFLNK	42
#define camacRecordVAL	43
#define camacRecordBPTR	44
#define camacRecordQ	45
#define camacRecordX	46
#define camacRecordINHB	47
#define camacRecordB	48
#define camacRecordC	49
#define camacRecordN	50
#define camacRecordA	51
#define camacRecordF	52
#define camacRecordTMOD	53
#define camacRecordNMAX	54
#define camacRecordNUSE	55
#define camacRecordNACT	56
#define camacRecordCCMD	57
#endif /*INCcamacH*/
#ifdef GEN_SIZE_OFFSET
#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int camacRecordSizeOffset(dbRecordType *pdbRecordType)
{
    camacRecord *prec = 0;
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
  pdbRecordType->papFldDes[44]->size=sizeof(prec->bptr);
  pdbRecordType->papFldDes[44]->offset=(short)((char *)&prec->bptr - (char *)prec);
  pdbRecordType->papFldDes[45]->size=sizeof(prec->q);
  pdbRecordType->papFldDes[45]->offset=(short)((char *)&prec->q - (char *)prec);
  pdbRecordType->papFldDes[46]->size=sizeof(prec->x);
  pdbRecordType->papFldDes[46]->offset=(short)((char *)&prec->x - (char *)prec);
  pdbRecordType->papFldDes[47]->size=sizeof(prec->inhb);
  pdbRecordType->papFldDes[47]->offset=(short)((char *)&prec->inhb - (char *)prec);
  pdbRecordType->papFldDes[48]->size=sizeof(prec->b);
  pdbRecordType->papFldDes[48]->offset=(short)((char *)&prec->b - (char *)prec);
  pdbRecordType->papFldDes[49]->size=sizeof(prec->c);
  pdbRecordType->papFldDes[49]->offset=(short)((char *)&prec->c - (char *)prec);
  pdbRecordType->papFldDes[50]->size=sizeof(prec->n);
  pdbRecordType->papFldDes[50]->offset=(short)((char *)&prec->n - (char *)prec);
  pdbRecordType->papFldDes[51]->size=sizeof(prec->a);
  pdbRecordType->papFldDes[51]->offset=(short)((char *)&prec->a - (char *)prec);
  pdbRecordType->papFldDes[52]->size=sizeof(prec->f);
  pdbRecordType->papFldDes[52]->offset=(short)((char *)&prec->f - (char *)prec);
  pdbRecordType->papFldDes[53]->size=sizeof(prec->tmod);
  pdbRecordType->papFldDes[53]->offset=(short)((char *)&prec->tmod - (char *)prec);
  pdbRecordType->papFldDes[54]->size=sizeof(prec->nmax);
  pdbRecordType->papFldDes[54]->offset=(short)((char *)&prec->nmax - (char *)prec);
  pdbRecordType->papFldDes[55]->size=sizeof(prec->nuse);
  pdbRecordType->papFldDes[55]->offset=(short)((char *)&prec->nuse - (char *)prec);
  pdbRecordType->papFldDes[56]->size=sizeof(prec->nact);
  pdbRecordType->papFldDes[56]->offset=(short)((char *)&prec->nact - (char *)prec);
  pdbRecordType->papFldDes[57]->size=sizeof(prec->ccmd);
  pdbRecordType->papFldDes[57]->offset=(short)((char *)&prec->ccmd - (char *)prec);
    pdbRecordType->rec_size = sizeof(*prec);
    return(0);
}
epicsExportRegistrar(camacRecordSizeOffset);
#ifdef __cplusplus
}
#endif
#endif /*GEN_SIZE_OFFSET*/
