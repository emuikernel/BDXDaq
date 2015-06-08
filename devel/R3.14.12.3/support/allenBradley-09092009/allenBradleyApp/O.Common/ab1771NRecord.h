
#ifndef INCab1771N_UnitsH
#define INCab1771N_UnitsH
typedef enum {
	ab1771N_UnitsF,
	ab1771N_UnitsC,
	ab1771N_UnitsOhms
}ab1771N_Units;
#endif /*INCab1771N_UnitsH*/

#ifndef INCab1771N_TYPEH
#define INCab1771N_TYPEH
typedef enum {
	ab1771N_TYPE_VA,
	ab1771N_TYPE_tcB,
	ab1771N_TYPE_tcE,
	ab1771N_TYPE_tcJ,
	ab1771N_TYPE_tcK,
	ab1771N_TYPE_tcR,
	ab1771N_TYPE_tcS,
	ab1771N_TYPE_tcT,
	ab1771N_TYPE_tcC,
	ab1771N_TYPE_tcN,
	ab1771N_TYPE_RTDPtEurope,
	ab1771N_TYPE_RTDPtUSA,
	ab1771N_TYPE_RTDCopper,
	ab1771N_TYPE_RTDNickel
}ab1771N_TYPE;
#endif /*INCab1771N_TYPEH*/

#ifndef INCab1771N_MTYPH
#define INCab1771N_MTYPH
typedef enum {
	ab1771N_NIS,
	ab1771N_NIV,
	ab1771N_NIV1,
	ab1771N_NIVR,
	ab1771N_NIVT,
	ab1771N_NR,
	ab1771N_NT1,
	ab1771N_NT2,
	ab1771N_NOC,
	ab1771N_NOV,
	ab1771N_NB4T,
	ab1771N_NB4S,
	ab1771N_NBSC,
	ab1771N_NBRC,
	ab1771N_NBTC,
	ab1771N_NBV1,
	ab1771N_NBVC,
	ab1771N_NX1,
	ab1771N_NX2,
	ab1771N_NX3,
	ab1771N_NX4
}ab1771N_MTYP;
#endif /*INCab1771N_MTYPH*/

#ifndef INCab1771N_CMDH
#define INCab1771N_CMDH
typedef enum {
	ab1771N_CMD_None,
	ab1771N_Init
}ab1771N_CMD;
#endif /*INCab1771N_CMDH*/

#ifndef INCab1771N_CJSH
#define INCab1771N_CJSH
typedef enum {
	ab1771N_CJS_Ok,
	ab1771N_CJS_UnderRange,
	ab1771N_CJS_OverRange,
	ab1771N_CJS_Bad
}ab1771N_CJS;
#endif /*INCab1771N_CJSH*/
#ifndef INCab1771NH
#define INCab1771NH
#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"
typedef struct ab1771NRecord {
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
	void	*iao1;	/* Interface for ao */
	epicsEnum16	mtyp;	/* Module Type */
	epicsEnum16	scal;	/* Temperature Scale */
	epicsEnum16	cjae;	/* Cold Junction alarm enable */
	epicsEnum16	cjs;	/* Cold Junction status */
	epicsFloat32	cjt;	/* Cold Junction Temperature  */
	epicsEnum16	typ1;	/* Chan 1 type */
	epicsFloat32	fil1;	/* Chan 1 Filter Time Constant (.1 - 10.0)secs */
	epicsFloat32	ral1;	/* Chan 1 Rate Alarm  */
	epicsFloat32	ohm1;	/* Chan 1 10 ohm offset (RTD Copper only) */
	epicsInt16	sta1;	/* Chan 1 status */
	epicsInt16	raw1;	/* Chan 1 raw value */
	epicsEnum16	typ2;	/* Chan 2 type */
	epicsFloat32	fil2;	/* Chan 2 Filter Time Constant (.1 - 10.0)secs */
	epicsFloat32	ral2;	/* Chan 2 Rate Alarm  */
	epicsFloat32	ohm2;	/* Chan 2 10 ohm offset (RTD Copper only) */
	epicsInt16	sta2;	/* Chan 2 status */
	epicsInt16	raw2;	/* Chan 2 raw value */
	epicsEnum16	typ3;	/* Chan 3 type */
	epicsFloat32	fil3;	/* Chan 3 Filter Time Constant (.1 - 10.0)secs */
	epicsFloat32	ral3;	/* Chan 3 Rate Alarm  */
	epicsFloat32	ohm3;	/* Chan 3 10 ohm offset (RTD Copper only) */
	epicsInt16	sta3;	/* Chan 3 status */
	epicsInt16	raw3;	/* Chan 3 raw value */
	epicsEnum16	typ4;	/* Chan 4 type */
	epicsFloat32	fil4;	/* Chan 4 Filter Time Constant (.1 - 10.0)secs */
	epicsFloat32	ral4;	/* Chan 4 Rate Alarm  */
	epicsFloat32	ohm4;	/* Chan 4 10 ohm offset (RTD Copper only) */
	epicsInt16	sta4;	/* Chan 4 status */
	epicsInt16	raw4;	/* Chan 4 raw value */
	epicsEnum16	typ5;	/* Chan 5 type */
	epicsFloat32	fil5;	/* Chan 5 Filter Time Constant (.1 - 10.0)secs */
	epicsFloat32	ral5;	/* Chan 5 Rate Alarm  */
	epicsFloat32	ohm5;	/* Chan 5 10 ohm offset (RTD Copper only) */
	epicsInt16	sta5;	/* Chan 5 status */
	epicsInt16	raw5;	/* Chan 5 raw value */
	epicsEnum16	typ6;	/* Chan 6 type */
	epicsFloat32	fil6;	/* Chan 6 Filter Time Constant (.1 - 10.0)secs */
	epicsFloat32	ral6;	/* Chan 6 Rate Alarm  */
	epicsFloat32	ohm6;	/* Chan 6 10 ohm offset (RTD Copper only) */
	epicsInt16	sta6;	/* Chan 6 status */
	epicsInt16	raw6;	/* Chan 6 raw value */
	epicsEnum16	typ7;	/* Chan 7 type */
	epicsFloat32	fil7;	/* Chan 7 Filter Time Constant (.1 - 10.0)secs */
	epicsFloat32	ral7;	/* Chan 7 Rate Alarm  */
	epicsFloat32	ohm7;	/* Chan 7 10 ohm offset (RTD Copper only) */
	epicsInt16	sta7;	/* Chan 7 status */
	epicsInt16	raw7;	/* Chan 7 raw value */
	epicsEnum16	typ8;	/* Chan 8 type */
	epicsFloat32	fil8;	/* Chan 8 Filter Time Constant (.1 - 10.0)secs */
	epicsFloat32	ral8;	/* Chan 8 Rate Alarm  */
	epicsFloat32	ohm8;	/* Chan 8 10 ohm offset (RTD Copper only) */
	epicsInt16	sta8;	/* Chan 8 status */
	epicsInt16	raw8;	/* Chan 8 raw value */
} ab1771NRecord;
#define ab1771NRecordNAME	0
#define ab1771NRecordDESC	1
#define ab1771NRecordASG	2
#define ab1771NRecordSCAN	3
#define ab1771NRecordPINI	4
#define ab1771NRecordPHAS	5
#define ab1771NRecordEVNT	6
#define ab1771NRecordTSE	7
#define ab1771NRecordTSEL	8
#define ab1771NRecordDTYP	9
#define ab1771NRecordDISV	10
#define ab1771NRecordDISA	11
#define ab1771NRecordSDIS	12
#define ab1771NRecordMLOK	13
#define ab1771NRecordMLIS	14
#define ab1771NRecordDISP	15
#define ab1771NRecordPROC	16
#define ab1771NRecordSTAT	17
#define ab1771NRecordSEVR	18
#define ab1771NRecordNSTA	19
#define ab1771NRecordNSEV	20
#define ab1771NRecordACKS	21
#define ab1771NRecordACKT	22
#define ab1771NRecordDISS	23
#define ab1771NRecordLCNT	24
#define ab1771NRecordPACT	25
#define ab1771NRecordPUTF	26
#define ab1771NRecordRPRO	27
#define ab1771NRecordASP	28
#define ab1771NRecordPPN	29
#define ab1771NRecordPPNR	30
#define ab1771NRecordSPVT	31
#define ab1771NRecordRSET	32
#define ab1771NRecordDSET	33
#define ab1771NRecordDPVT	34
#define ab1771NRecordRDES	35
#define ab1771NRecordLSET	36
#define ab1771NRecordPRIO	37
#define ab1771NRecordTPRO	38
#define ab1771NRecordBKPT	39
#define ab1771NRecordUDF	40
#define ab1771NRecordTIME	41
#define ab1771NRecordFLNK	42
#define ab1771NRecordVAL	43
#define ab1771NRecordCMD	44
#define ab1771NRecordLOCA	45
#define ab1771NRecordLINK	46
#define ab1771NRecordRACK	47
#define ab1771NRecordSLOT	48
#define ab1771NRecordOUTM	49
#define ab1771NRecordINPM	50
#define ab1771NRecordIAI1	51
#define ab1771NRecordIAO1	52
#define ab1771NRecordMTYP	53
#define ab1771NRecordSCAL	54
#define ab1771NRecordCJAE	55
#define ab1771NRecordCJS	56
#define ab1771NRecordCJT	57
#define ab1771NRecordTYP1	58
#define ab1771NRecordFIL1	59
#define ab1771NRecordRAL1	60
#define ab1771NRecordOHM1	61
#define ab1771NRecordSTA1	62
#define ab1771NRecordRAW1	63
#define ab1771NRecordTYP2	64
#define ab1771NRecordFIL2	65
#define ab1771NRecordRAL2	66
#define ab1771NRecordOHM2	67
#define ab1771NRecordSTA2	68
#define ab1771NRecordRAW2	69
#define ab1771NRecordTYP3	70
#define ab1771NRecordFIL3	71
#define ab1771NRecordRAL3	72
#define ab1771NRecordOHM3	73
#define ab1771NRecordSTA3	74
#define ab1771NRecordRAW3	75
#define ab1771NRecordTYP4	76
#define ab1771NRecordFIL4	77
#define ab1771NRecordRAL4	78
#define ab1771NRecordOHM4	79
#define ab1771NRecordSTA4	80
#define ab1771NRecordRAW4	81
#define ab1771NRecordTYP5	82
#define ab1771NRecordFIL5	83
#define ab1771NRecordRAL5	84
#define ab1771NRecordOHM5	85
#define ab1771NRecordSTA5	86
#define ab1771NRecordRAW5	87
#define ab1771NRecordTYP6	88
#define ab1771NRecordFIL6	89
#define ab1771NRecordRAL6	90
#define ab1771NRecordOHM6	91
#define ab1771NRecordSTA6	92
#define ab1771NRecordRAW6	93
#define ab1771NRecordTYP7	94
#define ab1771NRecordFIL7	95
#define ab1771NRecordRAL7	96
#define ab1771NRecordOHM7	97
#define ab1771NRecordSTA7	98
#define ab1771NRecordRAW7	99
#define ab1771NRecordTYP8	100
#define ab1771NRecordFIL8	101
#define ab1771NRecordRAL8	102
#define ab1771NRecordOHM8	103
#define ab1771NRecordSTA8	104
#define ab1771NRecordRAW8	105
#endif /*INCab1771NH*/
#ifdef GEN_SIZE_OFFSET
#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int ab1771NRecordSizeOffset(dbRecordType *pdbRecordType)
{
    ab1771NRecord *prec = 0;
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
  pdbRecordType->papFldDes[52]->size=sizeof(prec->iao1);
  pdbRecordType->papFldDes[52]->offset=(short)((char *)&prec->iao1 - (char *)prec);
  pdbRecordType->papFldDes[53]->size=sizeof(prec->mtyp);
  pdbRecordType->papFldDes[53]->offset=(short)((char *)&prec->mtyp - (char *)prec);
  pdbRecordType->papFldDes[54]->size=sizeof(prec->scal);
  pdbRecordType->papFldDes[54]->offset=(short)((char *)&prec->scal - (char *)prec);
  pdbRecordType->papFldDes[55]->size=sizeof(prec->cjae);
  pdbRecordType->papFldDes[55]->offset=(short)((char *)&prec->cjae - (char *)prec);
  pdbRecordType->papFldDes[56]->size=sizeof(prec->cjs);
  pdbRecordType->papFldDes[56]->offset=(short)((char *)&prec->cjs - (char *)prec);
  pdbRecordType->papFldDes[57]->size=sizeof(prec->cjt);
  pdbRecordType->papFldDes[57]->offset=(short)((char *)&prec->cjt - (char *)prec);
  pdbRecordType->papFldDes[58]->size=sizeof(prec->typ1);
  pdbRecordType->papFldDes[58]->offset=(short)((char *)&prec->typ1 - (char *)prec);
  pdbRecordType->papFldDes[59]->size=sizeof(prec->fil1);
  pdbRecordType->papFldDes[59]->offset=(short)((char *)&prec->fil1 - (char *)prec);
  pdbRecordType->papFldDes[60]->size=sizeof(prec->ral1);
  pdbRecordType->papFldDes[60]->offset=(short)((char *)&prec->ral1 - (char *)prec);
  pdbRecordType->papFldDes[61]->size=sizeof(prec->ohm1);
  pdbRecordType->papFldDes[61]->offset=(short)((char *)&prec->ohm1 - (char *)prec);
  pdbRecordType->papFldDes[62]->size=sizeof(prec->sta1);
  pdbRecordType->papFldDes[62]->offset=(short)((char *)&prec->sta1 - (char *)prec);
  pdbRecordType->papFldDes[63]->size=sizeof(prec->raw1);
  pdbRecordType->papFldDes[63]->offset=(short)((char *)&prec->raw1 - (char *)prec);
  pdbRecordType->papFldDes[64]->size=sizeof(prec->typ2);
  pdbRecordType->papFldDes[64]->offset=(short)((char *)&prec->typ2 - (char *)prec);
  pdbRecordType->papFldDes[65]->size=sizeof(prec->fil2);
  pdbRecordType->papFldDes[65]->offset=(short)((char *)&prec->fil2 - (char *)prec);
  pdbRecordType->papFldDes[66]->size=sizeof(prec->ral2);
  pdbRecordType->papFldDes[66]->offset=(short)((char *)&prec->ral2 - (char *)prec);
  pdbRecordType->papFldDes[67]->size=sizeof(prec->ohm2);
  pdbRecordType->papFldDes[67]->offset=(short)((char *)&prec->ohm2 - (char *)prec);
  pdbRecordType->papFldDes[68]->size=sizeof(prec->sta2);
  pdbRecordType->papFldDes[68]->offset=(short)((char *)&prec->sta2 - (char *)prec);
  pdbRecordType->papFldDes[69]->size=sizeof(prec->raw2);
  pdbRecordType->papFldDes[69]->offset=(short)((char *)&prec->raw2 - (char *)prec);
  pdbRecordType->papFldDes[70]->size=sizeof(prec->typ3);
  pdbRecordType->papFldDes[70]->offset=(short)((char *)&prec->typ3 - (char *)prec);
  pdbRecordType->papFldDes[71]->size=sizeof(prec->fil3);
  pdbRecordType->papFldDes[71]->offset=(short)((char *)&prec->fil3 - (char *)prec);
  pdbRecordType->papFldDes[72]->size=sizeof(prec->ral3);
  pdbRecordType->papFldDes[72]->offset=(short)((char *)&prec->ral3 - (char *)prec);
  pdbRecordType->papFldDes[73]->size=sizeof(prec->ohm3);
  pdbRecordType->papFldDes[73]->offset=(short)((char *)&prec->ohm3 - (char *)prec);
  pdbRecordType->papFldDes[74]->size=sizeof(prec->sta3);
  pdbRecordType->papFldDes[74]->offset=(short)((char *)&prec->sta3 - (char *)prec);
  pdbRecordType->papFldDes[75]->size=sizeof(prec->raw3);
  pdbRecordType->papFldDes[75]->offset=(short)((char *)&prec->raw3 - (char *)prec);
  pdbRecordType->papFldDes[76]->size=sizeof(prec->typ4);
  pdbRecordType->papFldDes[76]->offset=(short)((char *)&prec->typ4 - (char *)prec);
  pdbRecordType->papFldDes[77]->size=sizeof(prec->fil4);
  pdbRecordType->papFldDes[77]->offset=(short)((char *)&prec->fil4 - (char *)prec);
  pdbRecordType->papFldDes[78]->size=sizeof(prec->ral4);
  pdbRecordType->papFldDes[78]->offset=(short)((char *)&prec->ral4 - (char *)prec);
  pdbRecordType->papFldDes[79]->size=sizeof(prec->ohm4);
  pdbRecordType->papFldDes[79]->offset=(short)((char *)&prec->ohm4 - (char *)prec);
  pdbRecordType->papFldDes[80]->size=sizeof(prec->sta4);
  pdbRecordType->papFldDes[80]->offset=(short)((char *)&prec->sta4 - (char *)prec);
  pdbRecordType->papFldDes[81]->size=sizeof(prec->raw4);
  pdbRecordType->papFldDes[81]->offset=(short)((char *)&prec->raw4 - (char *)prec);
  pdbRecordType->papFldDes[82]->size=sizeof(prec->typ5);
  pdbRecordType->papFldDes[82]->offset=(short)((char *)&prec->typ5 - (char *)prec);
  pdbRecordType->papFldDes[83]->size=sizeof(prec->fil5);
  pdbRecordType->papFldDes[83]->offset=(short)((char *)&prec->fil5 - (char *)prec);
  pdbRecordType->papFldDes[84]->size=sizeof(prec->ral5);
  pdbRecordType->papFldDes[84]->offset=(short)((char *)&prec->ral5 - (char *)prec);
  pdbRecordType->papFldDes[85]->size=sizeof(prec->ohm5);
  pdbRecordType->papFldDes[85]->offset=(short)((char *)&prec->ohm5 - (char *)prec);
  pdbRecordType->papFldDes[86]->size=sizeof(prec->sta5);
  pdbRecordType->papFldDes[86]->offset=(short)((char *)&prec->sta5 - (char *)prec);
  pdbRecordType->papFldDes[87]->size=sizeof(prec->raw5);
  pdbRecordType->papFldDes[87]->offset=(short)((char *)&prec->raw5 - (char *)prec);
  pdbRecordType->papFldDes[88]->size=sizeof(prec->typ6);
  pdbRecordType->papFldDes[88]->offset=(short)((char *)&prec->typ6 - (char *)prec);
  pdbRecordType->papFldDes[89]->size=sizeof(prec->fil6);
  pdbRecordType->papFldDes[89]->offset=(short)((char *)&prec->fil6 - (char *)prec);
  pdbRecordType->papFldDes[90]->size=sizeof(prec->ral6);
  pdbRecordType->papFldDes[90]->offset=(short)((char *)&prec->ral6 - (char *)prec);
  pdbRecordType->papFldDes[91]->size=sizeof(prec->ohm6);
  pdbRecordType->papFldDes[91]->offset=(short)((char *)&prec->ohm6 - (char *)prec);
  pdbRecordType->papFldDes[92]->size=sizeof(prec->sta6);
  pdbRecordType->papFldDes[92]->offset=(short)((char *)&prec->sta6 - (char *)prec);
  pdbRecordType->papFldDes[93]->size=sizeof(prec->raw6);
  pdbRecordType->papFldDes[93]->offset=(short)((char *)&prec->raw6 - (char *)prec);
  pdbRecordType->papFldDes[94]->size=sizeof(prec->typ7);
  pdbRecordType->papFldDes[94]->offset=(short)((char *)&prec->typ7 - (char *)prec);
  pdbRecordType->papFldDes[95]->size=sizeof(prec->fil7);
  pdbRecordType->papFldDes[95]->offset=(short)((char *)&prec->fil7 - (char *)prec);
  pdbRecordType->papFldDes[96]->size=sizeof(prec->ral7);
  pdbRecordType->papFldDes[96]->offset=(short)((char *)&prec->ral7 - (char *)prec);
  pdbRecordType->papFldDes[97]->size=sizeof(prec->ohm7);
  pdbRecordType->papFldDes[97]->offset=(short)((char *)&prec->ohm7 - (char *)prec);
  pdbRecordType->papFldDes[98]->size=sizeof(prec->sta7);
  pdbRecordType->papFldDes[98]->offset=(short)((char *)&prec->sta7 - (char *)prec);
  pdbRecordType->papFldDes[99]->size=sizeof(prec->raw7);
  pdbRecordType->papFldDes[99]->offset=(short)((char *)&prec->raw7 - (char *)prec);
  pdbRecordType->papFldDes[100]->size=sizeof(prec->typ8);
  pdbRecordType->papFldDes[100]->offset=(short)((char *)&prec->typ8 - (char *)prec);
  pdbRecordType->papFldDes[101]->size=sizeof(prec->fil8);
  pdbRecordType->papFldDes[101]->offset=(short)((char *)&prec->fil8 - (char *)prec);
  pdbRecordType->papFldDes[102]->size=sizeof(prec->ral8);
  pdbRecordType->papFldDes[102]->offset=(short)((char *)&prec->ral8 - (char *)prec);
  pdbRecordType->papFldDes[103]->size=sizeof(prec->ohm8);
  pdbRecordType->papFldDes[103]->offset=(short)((char *)&prec->ohm8 - (char *)prec);
  pdbRecordType->papFldDes[104]->size=sizeof(prec->sta8);
  pdbRecordType->papFldDes[104]->offset=(short)((char *)&prec->sta8 - (char *)prec);
  pdbRecordType->papFldDes[105]->size=sizeof(prec->raw8);
  pdbRecordType->papFldDes[105]->offset=(short)((char *)&prec->raw8 - (char *)prec);
    pdbRecordType->rec_size = sizeof(*prec);
    return(0);
}
epicsExportRegistrar(ab1771NRecordSizeOffset);
#ifdef __cplusplus
}
#endif
#endif /*GEN_SIZE_OFFSET*/
