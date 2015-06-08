
#ifndef INCdigitelTYPEH
#define INCdigitelTYPEH
typedef enum {
	digitelTYPE_Digitel_MPC,
	digitelTYPE_Digitel_500,
	digitelTYPE_Digitel_1500
}digitelTYPE;
#endif /*INCdigitelTYPEH*/

#ifndef INCdigitelSET1H
#define INCdigitelSET1H
typedef enum {
	digitelSET1_Off,
	digitelSET1_On
}digitelSET1;
#endif /*INCdigitelSET1H*/

#ifndef INCdigitelS3BSH
#define INCdigitelS3BSH
typedef enum {
	digitelS3BS_Real_Time,
	digitelS3BS_Heat_On_Time
}digitelS3BS;
#endif /*INCdigitelS3BSH*/

#ifndef INCdigitelS1VSH
#define INCdigitelS1VSH
typedef enum {
	digitelS1VS_Off,
	digitelS1VS_On
}digitelS1VS;
#endif /*INCdigitelS1VSH*/

#ifndef INCdigitelS1MSH
#define INCdigitelS1MSH
typedef enum {
	digitelS1MS_Pressure,
	digitelS1MS_Current
}digitelS1MS;
#endif /*INCdigitelS1MSH*/

#ifndef INCdigitelPTYPH
#define INCdigitelPTYPH
typedef enum {
	digitelPTYP_30__Liter_sec,
	digitelPTYP_60__Liter_sec,
	digitelPTYP_120_Liter_sec,
	digitelPTYP_220_Liter_sec,
	digitelPTYP_400_Liter_sec,
	digitelPTYP_700_Liter_sec,
	digitelPTYP_1200_Liter_sec
}digitelPTYP;
#endif /*INCdigitelPTYPH*/

#ifndef INCdigitelMODSH
#define INCdigitelMODSH
typedef enum {
	digitelMODS_STBY,
	digitelMODS_OPER
}digitelMODS;
#endif /*INCdigitelMODSH*/

#ifndef INCdigitelMODRH
#define INCdigitelMODRH
typedef enum {
	digitelMODR_STBY,
	digitelMODR_OPER,
	digitelMODR_CONN,
	digitelMODR_COOL,
	digitelMODR_PERR,
	digitelMODR_LOCK
}digitelMODR;
#endif /*INCdigitelMODRH*/

#ifndef INCdigitelKLCKH
#define INCdigitelKLCKH
typedef enum {
	digitelKLCK_Unlocked,
	digitelKLCK_Locked
}digitelKLCK;
#endif /*INCdigitelKLCKH*/

#ifndef INCdigitelDSPLH
#define INCdigitelDSPLH
typedef enum {
	digitelDSPL_VOLTS,
	digitelDSPL_CURR,
	digitelDSPL_PRES
}digitelDSPL;
#endif /*INCdigitelDSPLH*/

#ifndef INCdigitelCMORH
#define INCdigitelCMORH
typedef enum {
	digitelCMOR_Off,
	digitelCMOR_On
}digitelCMOR;
#endif /*INCdigitelCMORH*/

#ifndef INCdigitelBKINH
#define INCdigitelBKINH
typedef enum {
	digitelBKIN_Absent,
	digitelBKIN_Installed
}digitelBKIN;
#endif /*INCdigitelBKINH*/

#ifndef INCdigitelBAKSH
#define INCdigitelBAKSH
typedef enum {
	digitelBAKS_Disabled,
	digitelBAKS_Enabled
}digitelBAKS;
#endif /*INCdigitelBAKSH*/
#ifndef INCdigitelH
#define INCdigitelH
#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"
typedef struct digitelRecord {
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
	epicsFloat64	val;	/* Pressure */
	epicsFloat64	lval;	/* Pressure (log10 form) */
	epicsFloat64	hihi;	/* Pressure Hihi Alarm */
	epicsFloat64	lolo;	/* Pressure Lolo Alarm */
	epicsFloat64	high;	/* Pressure High Alarm */
	epicsFloat64	low;	/* Pressure Low Alarm */
	epicsEnum16	hhsv;	/* Pressure Hihi Severity */
	epicsEnum16	llsv;	/* Pressure Lolo Severity */
	epicsEnum16	hsv;	/* Pressure High Severity */
	epicsEnum16	lsv;	/* Pressure Low Severity */
	epicsFloat64	hyst;	/* Alarm Deadband */
	epicsFloat64	lalm;	/* Last Value Alarmed */
	epicsEnum16	dspl;	/* Display Mode */
	epicsEnum16	klck;	/* Keyboard Lock */
	epicsEnum16	mods;	/* Mode */
	epicsEnum16	modr;	/* Mode Readback */
	epicsEnum16	baks;	/* Bake */
	epicsEnum16	bakr;	/* Bake Readback */
	epicsFloat64	cool;	/* Cooldown Time */
	epicsEnum16	cmor;	/* Cooldown Mode */
	epicsEnum16	set1;	/* Setpoint 1 */
	epicsEnum16	set2;	/* Setpoint 2 */
	epicsEnum16	set3;	/* Setpoint 3 */
	epicsFloat64	accw;	/* Acc Power */
	epicsFloat64	acci;	/* Acc Current */
	epicsEnum16	ptyp;	/* Pump Type */
	epicsEnum16	type;	/* Controller Type */
	epicsFloat64	sp1s;	/* SP1 Setpoint */
	epicsFloat64	sp1r;	/* SP1 SP Readback */
	epicsFloat64	s1hs;	/* SP1 Hysteresis */
	epicsFloat64	s1hr;	/* SP1 HY Readback */
	epicsEnum16	s1ms;	/* SP1 Mode */
	epicsEnum16	s1mr;	/* SP1 Mode Readback */
	epicsEnum16	s1vs;	/* SP1 HV Interlock */
	epicsEnum16	s1vr;	/* SP1 HVI Readback */
	epicsFloat64	sp2s;	/* SP2 Setpoint */
	epicsFloat64	sp2r;	/* SP2 SP Readback */
	epicsFloat64	s2hs;	/* SP2 Hysteresis */
	epicsFloat64	s2hr;	/* SP2 HY Readback */
	epicsEnum16	s2ms;	/* SP2 Mode */
	epicsEnum16	s2mr;	/* SP2 Mode Readback */
	epicsEnum16	s2vs;	/* SP2 HV Interlock */
	epicsEnum16	s2vr;	/* SP2 HVI Readback */
	epicsFloat64	sp3s;	/* SP3 Setpoint */
	epicsFloat64	sp3r;	/* SP3 SP Readback */
	epicsFloat64	s3hs;	/* SP3 Hysteresis */
	epicsFloat64	s3hr;	/* SP3 HY Readback */
	epicsEnum16	s3ms;	/* SP3 Mode */
	epicsEnum16	s3mr;	/* SP3 Mode Readback */
	epicsEnum16	s3vs;	/* SP3 HV Interlock */
	epicsEnum16	s3vr;	/* SP3 HVI Readback */
	epicsEnum16	s3bs;	/* Bake Time Mode Set */
	epicsEnum16	s3br;	/* Bake Time Mode Read */
	epicsFloat64	s3ts;	/* Bake Time Set */
	epicsFloat64	s3tr;	/* Bake Time Read */
	epicsFloat32	hopr;	/* Pressure Display Hi */
	epicsFloat32	lopr;	/* Pressure Display Lo */
	epicsFloat32	hctr;	/* Current Display Hi */
	epicsFloat32	lctr;	/* Current Display Lo */
	epicsFloat32	hvtr;	/* Voltage Display Hi */
	epicsFloat32	lvtr;	/* Voltage Display Lo */
	epicsFloat32	hlpr;	/* Log Pres Display Hi */
	epicsFloat32	llpr;	/* Log Pres Display Lo */
	DBLINK		siml;	/* Sim Mode Location */
	epicsEnum16	simm;	/* Sim Mode Value */
	DBLINK		slmo;	/* Sim Location Mode */
	epicsEnum16	svmo;	/* Sim Value Mode */
	DBLINK		sls1;	/* Sim Location SP1 */
	epicsEnum16	svs1;	/* Sim Value SP1 */
	DBLINK		sls2;	/* Sim Location SP2 */
	epicsEnum16	svs2;	/* Sim Value SP2 */
	DBLINK		slcr;	/* Sim Location Current */
	epicsFloat64	svcr;	/* Sim Value Current */
	epicsUInt32	tonl;	/* Time Online */
	epicsFloat64	crnt;	/* Current */
	epicsFloat64	volt;	/* Voltage */
	epicsUInt32	flgs;	/* Mod Flags */
	epicsUInt32	spfg;	/* Setpoint Flags */
	epicsEnum16	bkin;	/* Bake Installed */
	epicsFloat64	ival;	/* init pressure */
	epicsFloat64	ilva;	/* init pressure (log10) */
	epicsEnum16	imod;	/* init mode */
	epicsEnum16	ibak;	/* init bake */
	epicsFloat64	icol;	/* init cooldown time */
	epicsEnum16	isp1;	/* init set1 */
	epicsEnum16	isp2;	/* init set2 */
	epicsEnum16	isp3;	/* init set3 */
	epicsFloat64	iacw;	/* init Acc power */
	epicsFloat64	iaci;	/* init Acc current */
	epicsEnum16	ipty;	/* init pump type */
	epicsEnum16	ibkn;	/* init Bake Installed */
	epicsFloat64	is1;	/* init sp1 */
	epicsFloat64	ih1;	/* init sp1 HY */
	epicsEnum16	im1;	/* init sp1 mode */
	epicsEnum16	ii1;	/* init sp1 HVI */
	epicsFloat64	is2;	/* init sp2 */
	epicsFloat64	ih2;	/* init sp2 HY */
	epicsEnum16	im2;	/* init sp2 mode */
	epicsEnum16	ii2;	/* init sp2 HVI */
	epicsFloat64	is3;	/* init sp3 */
	epicsFloat64	ih3;	/* init sp3 HY */
	epicsEnum16	im3;	/* init sp3 mode */
	epicsEnum16	ii3;	/* init sp3 HVI */
	epicsEnum16	ib3;	/* init sp3 bake time md */
	epicsFloat64	it3;	/* init sp3 bake time */
	epicsUInt32	iton;	/* init tonl */
	epicsFloat64	icrn;	/* init current */
	epicsFloat64	ivol;	/* init voltage */
	epicsInt32	cycl;	/* Cycle count */
	epicsInt16	err;	/* Error Count */
	epicsInt16	ierr;	/* init Error Count */
} digitelRecord;
#define digitelRecordNAME	0
#define digitelRecordDESC	1
#define digitelRecordASG	2
#define digitelRecordSCAN	3
#define digitelRecordPINI	4
#define digitelRecordPHAS	5
#define digitelRecordEVNT	6
#define digitelRecordTSE	7
#define digitelRecordTSEL	8
#define digitelRecordDTYP	9
#define digitelRecordDISV	10
#define digitelRecordDISA	11
#define digitelRecordSDIS	12
#define digitelRecordMLOK	13
#define digitelRecordMLIS	14
#define digitelRecordDISP	15
#define digitelRecordPROC	16
#define digitelRecordSTAT	17
#define digitelRecordSEVR	18
#define digitelRecordNSTA	19
#define digitelRecordNSEV	20
#define digitelRecordACKS	21
#define digitelRecordACKT	22
#define digitelRecordDISS	23
#define digitelRecordLCNT	24
#define digitelRecordPACT	25
#define digitelRecordPUTF	26
#define digitelRecordRPRO	27
#define digitelRecordASP	28
#define digitelRecordPPN	29
#define digitelRecordPPNR	30
#define digitelRecordSPVT	31
#define digitelRecordRSET	32
#define digitelRecordDSET	33
#define digitelRecordDPVT	34
#define digitelRecordRDES	35
#define digitelRecordLSET	36
#define digitelRecordPRIO	37
#define digitelRecordTPRO	38
#define digitelRecordBKPT	39
#define digitelRecordUDF	40
#define digitelRecordTIME	41
#define digitelRecordFLNK	42
#define digitelRecordINP	43
#define digitelRecordVAL	44
#define digitelRecordLVAL	45
#define digitelRecordHIHI	46
#define digitelRecordLOLO	47
#define digitelRecordHIGH	48
#define digitelRecordLOW	49
#define digitelRecordHHSV	50
#define digitelRecordLLSV	51
#define digitelRecordHSV	52
#define digitelRecordLSV	53
#define digitelRecordHYST	54
#define digitelRecordLALM	55
#define digitelRecordDSPL	56
#define digitelRecordKLCK	57
#define digitelRecordMODS	58
#define digitelRecordMODR	59
#define digitelRecordBAKS	60
#define digitelRecordBAKR	61
#define digitelRecordCOOL	62
#define digitelRecordCMOR	63
#define digitelRecordSET1	64
#define digitelRecordSET2	65
#define digitelRecordSET3	66
#define digitelRecordACCW	67
#define digitelRecordACCI	68
#define digitelRecordPTYP	69
#define digitelRecordTYPE	70
#define digitelRecordSP1S	71
#define digitelRecordSP1R	72
#define digitelRecordS1HS	73
#define digitelRecordS1HR	74
#define digitelRecordS1MS	75
#define digitelRecordS1MR	76
#define digitelRecordS1VS	77
#define digitelRecordS1VR	78
#define digitelRecordSP2S	79
#define digitelRecordSP2R	80
#define digitelRecordS2HS	81
#define digitelRecordS2HR	82
#define digitelRecordS2MS	83
#define digitelRecordS2MR	84
#define digitelRecordS2VS	85
#define digitelRecordS2VR	86
#define digitelRecordSP3S	87
#define digitelRecordSP3R	88
#define digitelRecordS3HS	89
#define digitelRecordS3HR	90
#define digitelRecordS3MS	91
#define digitelRecordS3MR	92
#define digitelRecordS3VS	93
#define digitelRecordS3VR	94
#define digitelRecordS3BS	95
#define digitelRecordS3BR	96
#define digitelRecordS3TS	97
#define digitelRecordS3TR	98
#define digitelRecordHOPR	99
#define digitelRecordLOPR	100
#define digitelRecordHCTR	101
#define digitelRecordLCTR	102
#define digitelRecordHVTR	103
#define digitelRecordLVTR	104
#define digitelRecordHLPR	105
#define digitelRecordLLPR	106
#define digitelRecordSIML	107
#define digitelRecordSIMM	108
#define digitelRecordSLMO	109
#define digitelRecordSVMO	110
#define digitelRecordSLS1	111
#define digitelRecordSVS1	112
#define digitelRecordSLS2	113
#define digitelRecordSVS2	114
#define digitelRecordSLCR	115
#define digitelRecordSVCR	116
#define digitelRecordTONL	117
#define digitelRecordCRNT	118
#define digitelRecordVOLT	119
#define digitelRecordFLGS	120
#define digitelRecordSPFG	121
#define digitelRecordBKIN	122
#define digitelRecordIVAL	123
#define digitelRecordILVA	124
#define digitelRecordIMOD	125
#define digitelRecordIBAK	126
#define digitelRecordICOL	127
#define digitelRecordISP1	128
#define digitelRecordISP2	129
#define digitelRecordISP3	130
#define digitelRecordIACW	131
#define digitelRecordIACI	132
#define digitelRecordIPTY	133
#define digitelRecordIBKN	134
#define digitelRecordIS1	135
#define digitelRecordIH1	136
#define digitelRecordIM1	137
#define digitelRecordII1	138
#define digitelRecordIS2	139
#define digitelRecordIH2	140
#define digitelRecordIM2	141
#define digitelRecordII2	142
#define digitelRecordIS3	143
#define digitelRecordIH3	144
#define digitelRecordIM3	145
#define digitelRecordII3	146
#define digitelRecordIB3	147
#define digitelRecordIT3	148
#define digitelRecordITON	149
#define digitelRecordICRN	150
#define digitelRecordIVOL	151
#define digitelRecordCYCL	152
#define digitelRecordERR	153
#define digitelRecordIERR	154
#endif /*INCdigitelH*/
#ifdef GEN_SIZE_OFFSET
#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int digitelRecordSizeOffset(dbRecordType *pdbRecordType)
{
    digitelRecord *prec = 0;
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
  pdbRecordType->papFldDes[44]->size=sizeof(prec->val);
  pdbRecordType->papFldDes[44]->offset=(short)((char *)&prec->val - (char *)prec);
  pdbRecordType->papFldDes[45]->size=sizeof(prec->lval);
  pdbRecordType->papFldDes[45]->offset=(short)((char *)&prec->lval - (char *)prec);
  pdbRecordType->papFldDes[46]->size=sizeof(prec->hihi);
  pdbRecordType->papFldDes[46]->offset=(short)((char *)&prec->hihi - (char *)prec);
  pdbRecordType->papFldDes[47]->size=sizeof(prec->lolo);
  pdbRecordType->papFldDes[47]->offset=(short)((char *)&prec->lolo - (char *)prec);
  pdbRecordType->papFldDes[48]->size=sizeof(prec->high);
  pdbRecordType->papFldDes[48]->offset=(short)((char *)&prec->high - (char *)prec);
  pdbRecordType->papFldDes[49]->size=sizeof(prec->low);
  pdbRecordType->papFldDes[49]->offset=(short)((char *)&prec->low - (char *)prec);
  pdbRecordType->papFldDes[50]->size=sizeof(prec->hhsv);
  pdbRecordType->papFldDes[50]->offset=(short)((char *)&prec->hhsv - (char *)prec);
  pdbRecordType->papFldDes[51]->size=sizeof(prec->llsv);
  pdbRecordType->papFldDes[51]->offset=(short)((char *)&prec->llsv - (char *)prec);
  pdbRecordType->papFldDes[52]->size=sizeof(prec->hsv);
  pdbRecordType->papFldDes[52]->offset=(short)((char *)&prec->hsv - (char *)prec);
  pdbRecordType->papFldDes[53]->size=sizeof(prec->lsv);
  pdbRecordType->papFldDes[53]->offset=(short)((char *)&prec->lsv - (char *)prec);
  pdbRecordType->papFldDes[54]->size=sizeof(prec->hyst);
  pdbRecordType->papFldDes[54]->offset=(short)((char *)&prec->hyst - (char *)prec);
  pdbRecordType->papFldDes[55]->size=sizeof(prec->lalm);
  pdbRecordType->papFldDes[55]->offset=(short)((char *)&prec->lalm - (char *)prec);
  pdbRecordType->papFldDes[56]->size=sizeof(prec->dspl);
  pdbRecordType->papFldDes[56]->offset=(short)((char *)&prec->dspl - (char *)prec);
  pdbRecordType->papFldDes[57]->size=sizeof(prec->klck);
  pdbRecordType->papFldDes[57]->offset=(short)((char *)&prec->klck - (char *)prec);
  pdbRecordType->papFldDes[58]->size=sizeof(prec->mods);
  pdbRecordType->papFldDes[58]->offset=(short)((char *)&prec->mods - (char *)prec);
  pdbRecordType->papFldDes[59]->size=sizeof(prec->modr);
  pdbRecordType->papFldDes[59]->offset=(short)((char *)&prec->modr - (char *)prec);
  pdbRecordType->papFldDes[60]->size=sizeof(prec->baks);
  pdbRecordType->papFldDes[60]->offset=(short)((char *)&prec->baks - (char *)prec);
  pdbRecordType->papFldDes[61]->size=sizeof(prec->bakr);
  pdbRecordType->papFldDes[61]->offset=(short)((char *)&prec->bakr - (char *)prec);
  pdbRecordType->papFldDes[62]->size=sizeof(prec->cool);
  pdbRecordType->papFldDes[62]->offset=(short)((char *)&prec->cool - (char *)prec);
  pdbRecordType->papFldDes[63]->size=sizeof(prec->cmor);
  pdbRecordType->papFldDes[63]->offset=(short)((char *)&prec->cmor - (char *)prec);
  pdbRecordType->papFldDes[64]->size=sizeof(prec->set1);
  pdbRecordType->papFldDes[64]->offset=(short)((char *)&prec->set1 - (char *)prec);
  pdbRecordType->papFldDes[65]->size=sizeof(prec->set2);
  pdbRecordType->papFldDes[65]->offset=(short)((char *)&prec->set2 - (char *)prec);
  pdbRecordType->papFldDes[66]->size=sizeof(prec->set3);
  pdbRecordType->papFldDes[66]->offset=(short)((char *)&prec->set3 - (char *)prec);
  pdbRecordType->papFldDes[67]->size=sizeof(prec->accw);
  pdbRecordType->papFldDes[67]->offset=(short)((char *)&prec->accw - (char *)prec);
  pdbRecordType->papFldDes[68]->size=sizeof(prec->acci);
  pdbRecordType->papFldDes[68]->offset=(short)((char *)&prec->acci - (char *)prec);
  pdbRecordType->papFldDes[69]->size=sizeof(prec->ptyp);
  pdbRecordType->papFldDes[69]->offset=(short)((char *)&prec->ptyp - (char *)prec);
  pdbRecordType->papFldDes[70]->size=sizeof(prec->type);
  pdbRecordType->papFldDes[70]->offset=(short)((char *)&prec->type - (char *)prec);
  pdbRecordType->papFldDes[71]->size=sizeof(prec->sp1s);
  pdbRecordType->papFldDes[71]->offset=(short)((char *)&prec->sp1s - (char *)prec);
  pdbRecordType->papFldDes[72]->size=sizeof(prec->sp1r);
  pdbRecordType->papFldDes[72]->offset=(short)((char *)&prec->sp1r - (char *)prec);
  pdbRecordType->papFldDes[73]->size=sizeof(prec->s1hs);
  pdbRecordType->papFldDes[73]->offset=(short)((char *)&prec->s1hs - (char *)prec);
  pdbRecordType->papFldDes[74]->size=sizeof(prec->s1hr);
  pdbRecordType->papFldDes[74]->offset=(short)((char *)&prec->s1hr - (char *)prec);
  pdbRecordType->papFldDes[75]->size=sizeof(prec->s1ms);
  pdbRecordType->papFldDes[75]->offset=(short)((char *)&prec->s1ms - (char *)prec);
  pdbRecordType->papFldDes[76]->size=sizeof(prec->s1mr);
  pdbRecordType->papFldDes[76]->offset=(short)((char *)&prec->s1mr - (char *)prec);
  pdbRecordType->papFldDes[77]->size=sizeof(prec->s1vs);
  pdbRecordType->papFldDes[77]->offset=(short)((char *)&prec->s1vs - (char *)prec);
  pdbRecordType->papFldDes[78]->size=sizeof(prec->s1vr);
  pdbRecordType->papFldDes[78]->offset=(short)((char *)&prec->s1vr - (char *)prec);
  pdbRecordType->papFldDes[79]->size=sizeof(prec->sp2s);
  pdbRecordType->papFldDes[79]->offset=(short)((char *)&prec->sp2s - (char *)prec);
  pdbRecordType->papFldDes[80]->size=sizeof(prec->sp2r);
  pdbRecordType->papFldDes[80]->offset=(short)((char *)&prec->sp2r - (char *)prec);
  pdbRecordType->papFldDes[81]->size=sizeof(prec->s2hs);
  pdbRecordType->papFldDes[81]->offset=(short)((char *)&prec->s2hs - (char *)prec);
  pdbRecordType->papFldDes[82]->size=sizeof(prec->s2hr);
  pdbRecordType->papFldDes[82]->offset=(short)((char *)&prec->s2hr - (char *)prec);
  pdbRecordType->papFldDes[83]->size=sizeof(prec->s2ms);
  pdbRecordType->papFldDes[83]->offset=(short)((char *)&prec->s2ms - (char *)prec);
  pdbRecordType->papFldDes[84]->size=sizeof(prec->s2mr);
  pdbRecordType->papFldDes[84]->offset=(short)((char *)&prec->s2mr - (char *)prec);
  pdbRecordType->papFldDes[85]->size=sizeof(prec->s2vs);
  pdbRecordType->papFldDes[85]->offset=(short)((char *)&prec->s2vs - (char *)prec);
  pdbRecordType->papFldDes[86]->size=sizeof(prec->s2vr);
  pdbRecordType->papFldDes[86]->offset=(short)((char *)&prec->s2vr - (char *)prec);
  pdbRecordType->papFldDes[87]->size=sizeof(prec->sp3s);
  pdbRecordType->papFldDes[87]->offset=(short)((char *)&prec->sp3s - (char *)prec);
  pdbRecordType->papFldDes[88]->size=sizeof(prec->sp3r);
  pdbRecordType->papFldDes[88]->offset=(short)((char *)&prec->sp3r - (char *)prec);
  pdbRecordType->papFldDes[89]->size=sizeof(prec->s3hs);
  pdbRecordType->papFldDes[89]->offset=(short)((char *)&prec->s3hs - (char *)prec);
  pdbRecordType->papFldDes[90]->size=sizeof(prec->s3hr);
  pdbRecordType->papFldDes[90]->offset=(short)((char *)&prec->s3hr - (char *)prec);
  pdbRecordType->papFldDes[91]->size=sizeof(prec->s3ms);
  pdbRecordType->papFldDes[91]->offset=(short)((char *)&prec->s3ms - (char *)prec);
  pdbRecordType->papFldDes[92]->size=sizeof(prec->s3mr);
  pdbRecordType->papFldDes[92]->offset=(short)((char *)&prec->s3mr - (char *)prec);
  pdbRecordType->papFldDes[93]->size=sizeof(prec->s3vs);
  pdbRecordType->papFldDes[93]->offset=(short)((char *)&prec->s3vs - (char *)prec);
  pdbRecordType->papFldDes[94]->size=sizeof(prec->s3vr);
  pdbRecordType->papFldDes[94]->offset=(short)((char *)&prec->s3vr - (char *)prec);
  pdbRecordType->papFldDes[95]->size=sizeof(prec->s3bs);
  pdbRecordType->papFldDes[95]->offset=(short)((char *)&prec->s3bs - (char *)prec);
  pdbRecordType->papFldDes[96]->size=sizeof(prec->s3br);
  pdbRecordType->papFldDes[96]->offset=(short)((char *)&prec->s3br - (char *)prec);
  pdbRecordType->papFldDes[97]->size=sizeof(prec->s3ts);
  pdbRecordType->papFldDes[97]->offset=(short)((char *)&prec->s3ts - (char *)prec);
  pdbRecordType->papFldDes[98]->size=sizeof(prec->s3tr);
  pdbRecordType->papFldDes[98]->offset=(short)((char *)&prec->s3tr - (char *)prec);
  pdbRecordType->papFldDes[99]->size=sizeof(prec->hopr);
  pdbRecordType->papFldDes[99]->offset=(short)((char *)&prec->hopr - (char *)prec);
  pdbRecordType->papFldDes[100]->size=sizeof(prec->lopr);
  pdbRecordType->papFldDes[100]->offset=(short)((char *)&prec->lopr - (char *)prec);
  pdbRecordType->papFldDes[101]->size=sizeof(prec->hctr);
  pdbRecordType->papFldDes[101]->offset=(short)((char *)&prec->hctr - (char *)prec);
  pdbRecordType->papFldDes[102]->size=sizeof(prec->lctr);
  pdbRecordType->papFldDes[102]->offset=(short)((char *)&prec->lctr - (char *)prec);
  pdbRecordType->papFldDes[103]->size=sizeof(prec->hvtr);
  pdbRecordType->papFldDes[103]->offset=(short)((char *)&prec->hvtr - (char *)prec);
  pdbRecordType->papFldDes[104]->size=sizeof(prec->lvtr);
  pdbRecordType->papFldDes[104]->offset=(short)((char *)&prec->lvtr - (char *)prec);
  pdbRecordType->papFldDes[105]->size=sizeof(prec->hlpr);
  pdbRecordType->papFldDes[105]->offset=(short)((char *)&prec->hlpr - (char *)prec);
  pdbRecordType->papFldDes[106]->size=sizeof(prec->llpr);
  pdbRecordType->papFldDes[106]->offset=(short)((char *)&prec->llpr - (char *)prec);
  pdbRecordType->papFldDes[107]->size=sizeof(prec->siml);
  pdbRecordType->papFldDes[107]->offset=(short)((char *)&prec->siml - (char *)prec);
  pdbRecordType->papFldDes[108]->size=sizeof(prec->simm);
  pdbRecordType->papFldDes[108]->offset=(short)((char *)&prec->simm - (char *)prec);
  pdbRecordType->papFldDes[109]->size=sizeof(prec->slmo);
  pdbRecordType->papFldDes[109]->offset=(short)((char *)&prec->slmo - (char *)prec);
  pdbRecordType->papFldDes[110]->size=sizeof(prec->svmo);
  pdbRecordType->papFldDes[110]->offset=(short)((char *)&prec->svmo - (char *)prec);
  pdbRecordType->papFldDes[111]->size=sizeof(prec->sls1);
  pdbRecordType->papFldDes[111]->offset=(short)((char *)&prec->sls1 - (char *)prec);
  pdbRecordType->papFldDes[112]->size=sizeof(prec->svs1);
  pdbRecordType->papFldDes[112]->offset=(short)((char *)&prec->svs1 - (char *)prec);
  pdbRecordType->papFldDes[113]->size=sizeof(prec->sls2);
  pdbRecordType->papFldDes[113]->offset=(short)((char *)&prec->sls2 - (char *)prec);
  pdbRecordType->papFldDes[114]->size=sizeof(prec->svs2);
  pdbRecordType->papFldDes[114]->offset=(short)((char *)&prec->svs2 - (char *)prec);
  pdbRecordType->papFldDes[115]->size=sizeof(prec->slcr);
  pdbRecordType->papFldDes[115]->offset=(short)((char *)&prec->slcr - (char *)prec);
  pdbRecordType->papFldDes[116]->size=sizeof(prec->svcr);
  pdbRecordType->papFldDes[116]->offset=(short)((char *)&prec->svcr - (char *)prec);
  pdbRecordType->papFldDes[117]->size=sizeof(prec->tonl);
  pdbRecordType->papFldDes[117]->offset=(short)((char *)&prec->tonl - (char *)prec);
  pdbRecordType->papFldDes[118]->size=sizeof(prec->crnt);
  pdbRecordType->papFldDes[118]->offset=(short)((char *)&prec->crnt - (char *)prec);
  pdbRecordType->papFldDes[119]->size=sizeof(prec->volt);
  pdbRecordType->papFldDes[119]->offset=(short)((char *)&prec->volt - (char *)prec);
  pdbRecordType->papFldDes[120]->size=sizeof(prec->flgs);
  pdbRecordType->papFldDes[120]->offset=(short)((char *)&prec->flgs - (char *)prec);
  pdbRecordType->papFldDes[121]->size=sizeof(prec->spfg);
  pdbRecordType->papFldDes[121]->offset=(short)((char *)&prec->spfg - (char *)prec);
  pdbRecordType->papFldDes[122]->size=sizeof(prec->bkin);
  pdbRecordType->papFldDes[122]->offset=(short)((char *)&prec->bkin - (char *)prec);
  pdbRecordType->papFldDes[123]->size=sizeof(prec->ival);
  pdbRecordType->papFldDes[123]->offset=(short)((char *)&prec->ival - (char *)prec);
  pdbRecordType->papFldDes[124]->size=sizeof(prec->ilva);
  pdbRecordType->papFldDes[124]->offset=(short)((char *)&prec->ilva - (char *)prec);
  pdbRecordType->papFldDes[125]->size=sizeof(prec->imod);
  pdbRecordType->papFldDes[125]->offset=(short)((char *)&prec->imod - (char *)prec);
  pdbRecordType->papFldDes[126]->size=sizeof(prec->ibak);
  pdbRecordType->papFldDes[126]->offset=(short)((char *)&prec->ibak - (char *)prec);
  pdbRecordType->papFldDes[127]->size=sizeof(prec->icol);
  pdbRecordType->papFldDes[127]->offset=(short)((char *)&prec->icol - (char *)prec);
  pdbRecordType->papFldDes[128]->size=sizeof(prec->isp1);
  pdbRecordType->papFldDes[128]->offset=(short)((char *)&prec->isp1 - (char *)prec);
  pdbRecordType->papFldDes[129]->size=sizeof(prec->isp2);
  pdbRecordType->papFldDes[129]->offset=(short)((char *)&prec->isp2 - (char *)prec);
  pdbRecordType->papFldDes[130]->size=sizeof(prec->isp3);
  pdbRecordType->papFldDes[130]->offset=(short)((char *)&prec->isp3 - (char *)prec);
  pdbRecordType->papFldDes[131]->size=sizeof(prec->iacw);
  pdbRecordType->papFldDes[131]->offset=(short)((char *)&prec->iacw - (char *)prec);
  pdbRecordType->papFldDes[132]->size=sizeof(prec->iaci);
  pdbRecordType->papFldDes[132]->offset=(short)((char *)&prec->iaci - (char *)prec);
  pdbRecordType->papFldDes[133]->size=sizeof(prec->ipty);
  pdbRecordType->papFldDes[133]->offset=(short)((char *)&prec->ipty - (char *)prec);
  pdbRecordType->papFldDes[134]->size=sizeof(prec->ibkn);
  pdbRecordType->papFldDes[134]->offset=(short)((char *)&prec->ibkn - (char *)prec);
  pdbRecordType->papFldDes[135]->size=sizeof(prec->is1);
  pdbRecordType->papFldDes[135]->offset=(short)((char *)&prec->is1 - (char *)prec);
  pdbRecordType->papFldDes[136]->size=sizeof(prec->ih1);
  pdbRecordType->papFldDes[136]->offset=(short)((char *)&prec->ih1 - (char *)prec);
  pdbRecordType->papFldDes[137]->size=sizeof(prec->im1);
  pdbRecordType->papFldDes[137]->offset=(short)((char *)&prec->im1 - (char *)prec);
  pdbRecordType->papFldDes[138]->size=sizeof(prec->ii1);
  pdbRecordType->papFldDes[138]->offset=(short)((char *)&prec->ii1 - (char *)prec);
  pdbRecordType->papFldDes[139]->size=sizeof(prec->is2);
  pdbRecordType->papFldDes[139]->offset=(short)((char *)&prec->is2 - (char *)prec);
  pdbRecordType->papFldDes[140]->size=sizeof(prec->ih2);
  pdbRecordType->papFldDes[140]->offset=(short)((char *)&prec->ih2 - (char *)prec);
  pdbRecordType->papFldDes[141]->size=sizeof(prec->im2);
  pdbRecordType->papFldDes[141]->offset=(short)((char *)&prec->im2 - (char *)prec);
  pdbRecordType->papFldDes[142]->size=sizeof(prec->ii2);
  pdbRecordType->papFldDes[142]->offset=(short)((char *)&prec->ii2 - (char *)prec);
  pdbRecordType->papFldDes[143]->size=sizeof(prec->is3);
  pdbRecordType->papFldDes[143]->offset=(short)((char *)&prec->is3 - (char *)prec);
  pdbRecordType->papFldDes[144]->size=sizeof(prec->ih3);
  pdbRecordType->papFldDes[144]->offset=(short)((char *)&prec->ih3 - (char *)prec);
  pdbRecordType->papFldDes[145]->size=sizeof(prec->im3);
  pdbRecordType->papFldDes[145]->offset=(short)((char *)&prec->im3 - (char *)prec);
  pdbRecordType->papFldDes[146]->size=sizeof(prec->ii3);
  pdbRecordType->papFldDes[146]->offset=(short)((char *)&prec->ii3 - (char *)prec);
  pdbRecordType->papFldDes[147]->size=sizeof(prec->ib3);
  pdbRecordType->papFldDes[147]->offset=(short)((char *)&prec->ib3 - (char *)prec);
  pdbRecordType->papFldDes[148]->size=sizeof(prec->it3);
  pdbRecordType->papFldDes[148]->offset=(short)((char *)&prec->it3 - (char *)prec);
  pdbRecordType->papFldDes[149]->size=sizeof(prec->iton);
  pdbRecordType->papFldDes[149]->offset=(short)((char *)&prec->iton - (char *)prec);
  pdbRecordType->papFldDes[150]->size=sizeof(prec->icrn);
  pdbRecordType->papFldDes[150]->offset=(short)((char *)&prec->icrn - (char *)prec);
  pdbRecordType->papFldDes[151]->size=sizeof(prec->ivol);
  pdbRecordType->papFldDes[151]->offset=(short)((char *)&prec->ivol - (char *)prec);
  pdbRecordType->papFldDes[152]->size=sizeof(prec->cycl);
  pdbRecordType->papFldDes[152]->offset=(short)((char *)&prec->cycl - (char *)prec);
  pdbRecordType->papFldDes[153]->size=sizeof(prec->err);
  pdbRecordType->papFldDes[153]->offset=(short)((char *)&prec->err - (char *)prec);
  pdbRecordType->papFldDes[154]->size=sizeof(prec->ierr);
  pdbRecordType->papFldDes[154]->offset=(short)((char *)&prec->ierr - (char *)prec);
    pdbRecordType->rec_size = sizeof(*prec);
    return(0);
}
epicsExportRegistrar(digitelRecordSizeOffset);
#ifdef __cplusplus
}
#endif
#endif /*GEN_SIZE_OFFSET*/
