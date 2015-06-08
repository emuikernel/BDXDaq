
#ifndef INCtableSETH
#define INCtableSETH
typedef enum {
	tableSET_Use,
	tableSET_Set
}tableSET;
#endif /*INCtableSETH*/

#ifndef INCtableGEOMH
#define INCtableGEOMH
typedef enum {
	tableGEOM_SRI,
	tableGEOM_GEOCARS,
	tableGEOM_NEWPORT,
	tableGEOM_PNC
}tableGEOM;
#endif /*INCtableGEOMH*/
#ifndef INCtableH
#define INCtableH
#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"
typedef struct tableRecord {
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
	epicsFloat64	lx;	/* wheelbase x */
	epicsFloat64	lz;	/* wheelbase z */
	epicsFloat64	sx;	/* x of fixed point */
	epicsFloat64	sy;	/* y of fixed point */
	epicsFloat64	sz;	/* z of fixed point */
	epicsFloat64	rx;	/* x of ref point */
	epicsFloat64	ry;	/* y of ref point */
	epicsFloat64	rz;	/* z of ref point */
	epicsFloat64	yang;	/* Orientation angle */
	epicsFloat64	ax;	/* x angle */
	epicsFloat64	ay;	/* y angle */
	epicsFloat64	az;	/* z angle */
	epicsFloat64	x;	/* x translation */
	epicsFloat64	y;	/* y translation */
	epicsFloat64	z;	/* z translation */
	epicsFloat64	ax0;	/* x-angle offset */
	epicsFloat64	ay0;	/* y-angle offset */
	epicsFloat64	az0;	/* z-angle offset */
	epicsFloat64	x0;	/* x offset */
	epicsFloat64	y0;	/* y offset */
	epicsFloat64	z0;	/* z offset */
	epicsFloat64	axl;	/* x angle true value */
	epicsFloat64	ayl;	/* y angle true value */
	epicsFloat64	azl;	/* z angle true value */
	epicsFloat64	xl;	/* x true value */
	epicsFloat64	yl;	/* y true value */
	epicsFloat64	zl;	/* z true value */
	epicsFloat64	axrb;	/* x angle readback */
	epicsFloat64	ayrb;	/* y angle readback */
	epicsFloat64	azrb;	/* z angle readback */
	epicsFloat64	xrb;	/* x readback value */
	epicsFloat64	yrb;	/* y readback value */
	epicsFloat64	zrb;	/* z readback value */
	epicsFloat64	eax;	/* encoder x angle */
	epicsFloat64	eay;	/* encoder y angle */
	epicsFloat64	eaz;	/* encoder z angle */
	epicsFloat64	ex;	/* encoder x */
	epicsFloat64	ey;	/* encoder y */
	epicsFloat64	ez;	/* encoder z */
	epicsFloat64	hlax;	/* x angle hi limit */
	epicsFloat64	hlay;	/* y angle hi limit */
	epicsFloat64	hlaz;	/* z angle hi limit */
	epicsFloat64	hlx;	/* x hi limit */
	epicsFloat64	hly;	/* y hi limit */
	epicsFloat64	hlz;	/* z hi limit */
	epicsFloat64	llax;	/* x angle lo limit */
	epicsFloat64	llay;	/* y angle lo limit */
	epicsFloat64	llaz;	/* z angle lo limit */
	epicsFloat64	llx;	/* x lo limit */
	epicsFloat64	lly;	/* y lo limit */
	epicsFloat64	llz;	/* z lo limit */
	epicsFloat64	uhax;	/* x angle user hi limit */
	epicsFloat64	uhay;	/* y angle user hi limit */
	epicsFloat64	uhaz;	/* z angle user hi limit */
	epicsFloat64	uhx;	/* x user hi limit */
	epicsFloat64	uhy;	/* y user hi limit */
	epicsFloat64	uhz;	/* z user hi limit */
	epicsFloat64	ulax;	/* x angle user lo limit */
	epicsFloat64	ulay;	/* y angle user lo limit */
	epicsFloat64	ulaz;	/* z angle user lo limit */
	epicsFloat64	ulx;	/* x user lo limit */
	epicsFloat64	uly;	/* y user lo limit */
	epicsFloat64	ulz;	/* z user lo limit */
	DBLINK		m0xl;	/* motor 0X outlink */
	DBLINK		m0yl;	/* motor 0Y outlink */
	DBLINK		m1yl;	/* motor 1Y outlink */
	DBLINK		m2xl;	/* motor 2X outlink */
	DBLINK		m2yl;	/* motor 2Y outlink */
	DBLINK		m2zl;	/* motor 2Z outlink */
	epicsFloat64	m0x;	/* motor 0X val */
	epicsFloat64	m0y;	/* motor 0Y val */
	epicsFloat64	m1y;	/* motor 1Y val */
	epicsFloat64	m2x;	/* motor 2X val */
	epicsFloat64	m2y;	/* motor 2Y val */
	epicsFloat64	m2z;	/* motor 2Z val */
	DBLINK		r0xi;	/* motor 0X RBV link */
	DBLINK		r0yi;	/* motor 0Y RBV link */
	DBLINK		r1yi;	/* motor 1Y RBV link */
	DBLINK		r2xi;	/* motor 2X RBV link */
	DBLINK		r2yi;	/* motor 2Y RBV link */
	DBLINK		r2zi;	/* motor 2Z RBV link */
	epicsFloat64	r0x;	/* motor 0X readback */
	epicsFloat64	r0y;	/* motor 0Y readback */
	epicsFloat64	r1y;	/* motor 1Y readback */
	epicsFloat64	r2x;	/* motor 2X readback */
	epicsFloat64	r2y;	/* motor 2Y readback */
	epicsFloat64	r2z;	/* motor 2Z readback */
	DBLINK		e0xi;	/* encoder 0X inlink */
	DBLINK		e0yi;	/* encoder 0Y inlink */
	DBLINK		e1yi;	/* encoder 1Y inlink */
	DBLINK		e2xi;	/* encoder 2X inlink */
	DBLINK		e2yi;	/* encoder 2Y inlink */
	DBLINK		e2zi;	/* encoder 2Z inlink */
	epicsFloat64	e0x;	/* encoder 0X val */
	epicsFloat64	e0y;	/* encoder 0Y val */
	epicsFloat64	e1y;	/* encoder 1Y val */
	epicsFloat64	e2x;	/* encoder 2X val */
	epicsFloat64	e2y;	/* encoder 2Y val */
	epicsFloat64	e2z;	/* encoder 2Z val */
	DBLINK		v0xl;	/* speed 0X outlink */
	DBLINK		v0yl;	/* speed 0Y outlink */
	DBLINK		v1yl;	/* speed 1Y outlink */
	DBLINK		v2xl;	/* speed 2X outlink */
	DBLINK		v2yl;	/* speed 2Y outlink */
	DBLINK		v2zl;	/* speed 2Z outlink */
	epicsFloat64	v0x;	/* speed 0X val */
	epicsFloat64	v0y;	/* speed 0Y val */
	epicsFloat64	v1y;	/* speed 1Y val */
	epicsFloat64	v2x;	/* speed 2X val */
	epicsFloat64	v2y;	/* speed 2Y val */
	epicsFloat64	v2z;	/* speed 2Z val */
	DBLINK		v0xi;	/* speed 0X inlink */
	DBLINK		v0yi;	/* speed 0Y inlink */
	DBLINK		v1yi;	/* speed 1Y inlink */
	DBLINK		v2xi;	/* speed 2X inlink */
	DBLINK		v2yi;	/* speed 2Y inlink */
	DBLINK		v2zi;	/* speed 2Z inlink */
	DBLINK		h0xl;	/* motor 0X HLM link */
	DBLINK		h0yl;	/* motor 0Y HLM link */
	DBLINK		h1yl;	/* motor 1Y HLM link */
	DBLINK		h2xl;	/* motor 2X HLM link */
	DBLINK		h2yl;	/* motor 2Y HLM link */
	DBLINK		h2zl;	/* motor 2Z HLM link */
	epicsFloat64	h0x;	/* motor 0X hi limit */
	epicsFloat64	h0y;	/* motor 0Y hi limit */
	epicsFloat64	h1y;	/* motor 1Y hi limit */
	epicsFloat64	h2x;	/* motor 2X hi limit */
	epicsFloat64	h2y;	/* motor 2Y hi limit */
	epicsFloat64	h2z;	/* motor 2Z hi limit */
	DBLINK		l0xl;	/* motor 0X LLM link */
	DBLINK		l0yl;	/* motor 0Y LLM link */
	DBLINK		l1yl;	/* motor 1Y LLM link */
	DBLINK		l2xl;	/* motor 2X LLM link */
	DBLINK		l2yl;	/* motor 2Y LLM link */
	DBLINK		l2zl;	/* motor 2Z LLM link */
	epicsFloat64	l0x;	/* motor 0X lo limit */
	epicsFloat64	l0y;	/* motor 0Y lo limit */
	epicsFloat64	l1y;	/* motor 1Y lo limit */
	epicsFloat64	l2x;	/* motor 2X lo limit */
	epicsFloat64	l2y;	/* motor 2Y lo limit */
	epicsFloat64	l2z;	/* motor 2Z lo limit */
	epicsInt16	init;	/* init table */
	epicsInt16	zero;	/* zero table */
	epicsInt16	sync;	/* sync with motors */
	epicsInt16	read;	/* readback motors */
	epicsEnum16	set;	/* set table */
	epicsInt16	sset;	/* Set SET Mode */
	epicsInt16	suse;	/* Set USE Mode */
	epicsInt16	lvio;	/* limit violation */
	double *	pp0;	/* pivot pt 0 */
	double *	pp1;	/* pivot pt 1 */
	double *	pp2;	/* pivot pt 2 */
	double *	ppo0;	/* pivot pt 0 orig */
	double *	ppo1;	/* pivot pt 1 orig */
	double *	ppo2;	/* pivot pt 2 orig */
	double **	a;	/* x to m matrix */
	double **	b;	/* m to x matrix */
	char		legu[16];	/* Linear Units Name */
	char		aegu[16];	/* Angular Units Name */
	epicsInt16	prec;	/* Display Precision */
	epicsUInt32	mmap;	/* Monitor Mask */
	epicsEnum16	geom;	/* Geometry */
} tableRecord;
#define tableRecordNAME	0
#define tableRecordDESC	1
#define tableRecordASG	2
#define tableRecordSCAN	3
#define tableRecordPINI	4
#define tableRecordPHAS	5
#define tableRecordEVNT	6
#define tableRecordTSE	7
#define tableRecordTSEL	8
#define tableRecordDTYP	9
#define tableRecordDISV	10
#define tableRecordDISA	11
#define tableRecordSDIS	12
#define tableRecordMLOK	13
#define tableRecordMLIS	14
#define tableRecordDISP	15
#define tableRecordPROC	16
#define tableRecordSTAT	17
#define tableRecordSEVR	18
#define tableRecordNSTA	19
#define tableRecordNSEV	20
#define tableRecordACKS	21
#define tableRecordACKT	22
#define tableRecordDISS	23
#define tableRecordLCNT	24
#define tableRecordPACT	25
#define tableRecordPUTF	26
#define tableRecordRPRO	27
#define tableRecordASP	28
#define tableRecordPPN	29
#define tableRecordPPNR	30
#define tableRecordSPVT	31
#define tableRecordRSET	32
#define tableRecordDSET	33
#define tableRecordDPVT	34
#define tableRecordRDES	35
#define tableRecordLSET	36
#define tableRecordPRIO	37
#define tableRecordTPRO	38
#define tableRecordBKPT	39
#define tableRecordUDF	40
#define tableRecordTIME	41
#define tableRecordFLNK	42
#define tableRecordVERS	43
#define tableRecordVAL	44
#define tableRecordLX	45
#define tableRecordLZ	46
#define tableRecordSX	47
#define tableRecordSY	48
#define tableRecordSZ	49
#define tableRecordRX	50
#define tableRecordRY	51
#define tableRecordRZ	52
#define tableRecordYANG	53
#define tableRecordAX	54
#define tableRecordAY	55
#define tableRecordAZ	56
#define tableRecordX	57
#define tableRecordY	58
#define tableRecordZ	59
#define tableRecordAX0	60
#define tableRecordAY0	61
#define tableRecordAZ0	62
#define tableRecordX0	63
#define tableRecordY0	64
#define tableRecordZ0	65
#define tableRecordAXL	66
#define tableRecordAYL	67
#define tableRecordAZL	68
#define tableRecordXL	69
#define tableRecordYL	70
#define tableRecordZL	71
#define tableRecordAXRB	72
#define tableRecordAYRB	73
#define tableRecordAZRB	74
#define tableRecordXRB	75
#define tableRecordYRB	76
#define tableRecordZRB	77
#define tableRecordEAX	78
#define tableRecordEAY	79
#define tableRecordEAZ	80
#define tableRecordEX	81
#define tableRecordEY	82
#define tableRecordEZ	83
#define tableRecordHLAX	84
#define tableRecordHLAY	85
#define tableRecordHLAZ	86
#define tableRecordHLX	87
#define tableRecordHLY	88
#define tableRecordHLZ	89
#define tableRecordLLAX	90
#define tableRecordLLAY	91
#define tableRecordLLAZ	92
#define tableRecordLLX	93
#define tableRecordLLY	94
#define tableRecordLLZ	95
#define tableRecordUHAX	96
#define tableRecordUHAY	97
#define tableRecordUHAZ	98
#define tableRecordUHX	99
#define tableRecordUHY	100
#define tableRecordUHZ	101
#define tableRecordULAX	102
#define tableRecordULAY	103
#define tableRecordULAZ	104
#define tableRecordULX	105
#define tableRecordULY	106
#define tableRecordULZ	107
#define tableRecordM0XL	108
#define tableRecordM0YL	109
#define tableRecordM1YL	110
#define tableRecordM2XL	111
#define tableRecordM2YL	112
#define tableRecordM2ZL	113
#define tableRecordM0X	114
#define tableRecordM0Y	115
#define tableRecordM1Y	116
#define tableRecordM2X	117
#define tableRecordM2Y	118
#define tableRecordM2Z	119
#define tableRecordR0XI	120
#define tableRecordR0YI	121
#define tableRecordR1YI	122
#define tableRecordR2XI	123
#define tableRecordR2YI	124
#define tableRecordR2ZI	125
#define tableRecordR0X	126
#define tableRecordR0Y	127
#define tableRecordR1Y	128
#define tableRecordR2X	129
#define tableRecordR2Y	130
#define tableRecordR2Z	131
#define tableRecordE0XI	132
#define tableRecordE0YI	133
#define tableRecordE1YI	134
#define tableRecordE2XI	135
#define tableRecordE2YI	136
#define tableRecordE2ZI	137
#define tableRecordE0X	138
#define tableRecordE0Y	139
#define tableRecordE1Y	140
#define tableRecordE2X	141
#define tableRecordE2Y	142
#define tableRecordE2Z	143
#define tableRecordV0XL	144
#define tableRecordV0YL	145
#define tableRecordV1YL	146
#define tableRecordV2XL	147
#define tableRecordV2YL	148
#define tableRecordV2ZL	149
#define tableRecordV0X	150
#define tableRecordV0Y	151
#define tableRecordV1Y	152
#define tableRecordV2X	153
#define tableRecordV2Y	154
#define tableRecordV2Z	155
#define tableRecordV0XI	156
#define tableRecordV0YI	157
#define tableRecordV1YI	158
#define tableRecordV2XI	159
#define tableRecordV2YI	160
#define tableRecordV2ZI	161
#define tableRecordH0XL	162
#define tableRecordH0YL	163
#define tableRecordH1YL	164
#define tableRecordH2XL	165
#define tableRecordH2YL	166
#define tableRecordH2ZL	167
#define tableRecordH0X	168
#define tableRecordH0Y	169
#define tableRecordH1Y	170
#define tableRecordH2X	171
#define tableRecordH2Y	172
#define tableRecordH2Z	173
#define tableRecordL0XL	174
#define tableRecordL0YL	175
#define tableRecordL1YL	176
#define tableRecordL2XL	177
#define tableRecordL2YL	178
#define tableRecordL2ZL	179
#define tableRecordL0X	180
#define tableRecordL0Y	181
#define tableRecordL1Y	182
#define tableRecordL2X	183
#define tableRecordL2Y	184
#define tableRecordL2Z	185
#define tableRecordINIT	186
#define tableRecordZERO	187
#define tableRecordSYNC	188
#define tableRecordREAD	189
#define tableRecordSET	190
#define tableRecordSSET	191
#define tableRecordSUSE	192
#define tableRecordLVIO	193
#define tableRecordPP0	194
#define tableRecordPP1	195
#define tableRecordPP2	196
#define tableRecordPPO0	197
#define tableRecordPPO1	198
#define tableRecordPPO2	199
#define tableRecordA	200
#define tableRecordB	201
#define tableRecordLEGU	202
#define tableRecordAEGU	203
#define tableRecordPREC	204
#define tableRecordMMAP	205
#define tableRecordGEOM	206
#endif /*INCtableH*/
#ifdef GEN_SIZE_OFFSET
#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int tableRecordSizeOffset(dbRecordType *pdbRecordType)
{
    tableRecord *prec = 0;
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
  pdbRecordType->papFldDes[45]->size=sizeof(prec->lx);
  pdbRecordType->papFldDes[45]->offset=(short)((char *)&prec->lx - (char *)prec);
  pdbRecordType->papFldDes[46]->size=sizeof(prec->lz);
  pdbRecordType->papFldDes[46]->offset=(short)((char *)&prec->lz - (char *)prec);
  pdbRecordType->papFldDes[47]->size=sizeof(prec->sx);
  pdbRecordType->papFldDes[47]->offset=(short)((char *)&prec->sx - (char *)prec);
  pdbRecordType->papFldDes[48]->size=sizeof(prec->sy);
  pdbRecordType->papFldDes[48]->offset=(short)((char *)&prec->sy - (char *)prec);
  pdbRecordType->papFldDes[49]->size=sizeof(prec->sz);
  pdbRecordType->papFldDes[49]->offset=(short)((char *)&prec->sz - (char *)prec);
  pdbRecordType->papFldDes[50]->size=sizeof(prec->rx);
  pdbRecordType->papFldDes[50]->offset=(short)((char *)&prec->rx - (char *)prec);
  pdbRecordType->papFldDes[51]->size=sizeof(prec->ry);
  pdbRecordType->papFldDes[51]->offset=(short)((char *)&prec->ry - (char *)prec);
  pdbRecordType->papFldDes[52]->size=sizeof(prec->rz);
  pdbRecordType->papFldDes[52]->offset=(short)((char *)&prec->rz - (char *)prec);
  pdbRecordType->papFldDes[53]->size=sizeof(prec->yang);
  pdbRecordType->papFldDes[53]->offset=(short)((char *)&prec->yang - (char *)prec);
  pdbRecordType->papFldDes[54]->size=sizeof(prec->ax);
  pdbRecordType->papFldDes[54]->offset=(short)((char *)&prec->ax - (char *)prec);
  pdbRecordType->papFldDes[55]->size=sizeof(prec->ay);
  pdbRecordType->papFldDes[55]->offset=(short)((char *)&prec->ay - (char *)prec);
  pdbRecordType->papFldDes[56]->size=sizeof(prec->az);
  pdbRecordType->papFldDes[56]->offset=(short)((char *)&prec->az - (char *)prec);
  pdbRecordType->papFldDes[57]->size=sizeof(prec->x);
  pdbRecordType->papFldDes[57]->offset=(short)((char *)&prec->x - (char *)prec);
  pdbRecordType->papFldDes[58]->size=sizeof(prec->y);
  pdbRecordType->papFldDes[58]->offset=(short)((char *)&prec->y - (char *)prec);
  pdbRecordType->papFldDes[59]->size=sizeof(prec->z);
  pdbRecordType->papFldDes[59]->offset=(short)((char *)&prec->z - (char *)prec);
  pdbRecordType->papFldDes[60]->size=sizeof(prec->ax0);
  pdbRecordType->papFldDes[60]->offset=(short)((char *)&prec->ax0 - (char *)prec);
  pdbRecordType->papFldDes[61]->size=sizeof(prec->ay0);
  pdbRecordType->papFldDes[61]->offset=(short)((char *)&prec->ay0 - (char *)prec);
  pdbRecordType->papFldDes[62]->size=sizeof(prec->az0);
  pdbRecordType->papFldDes[62]->offset=(short)((char *)&prec->az0 - (char *)prec);
  pdbRecordType->papFldDes[63]->size=sizeof(prec->x0);
  pdbRecordType->papFldDes[63]->offset=(short)((char *)&prec->x0 - (char *)prec);
  pdbRecordType->papFldDes[64]->size=sizeof(prec->y0);
  pdbRecordType->papFldDes[64]->offset=(short)((char *)&prec->y0 - (char *)prec);
  pdbRecordType->papFldDes[65]->size=sizeof(prec->z0);
  pdbRecordType->papFldDes[65]->offset=(short)((char *)&prec->z0 - (char *)prec);
  pdbRecordType->papFldDes[66]->size=sizeof(prec->axl);
  pdbRecordType->papFldDes[66]->offset=(short)((char *)&prec->axl - (char *)prec);
  pdbRecordType->papFldDes[67]->size=sizeof(prec->ayl);
  pdbRecordType->papFldDes[67]->offset=(short)((char *)&prec->ayl - (char *)prec);
  pdbRecordType->papFldDes[68]->size=sizeof(prec->azl);
  pdbRecordType->papFldDes[68]->offset=(short)((char *)&prec->azl - (char *)prec);
  pdbRecordType->papFldDes[69]->size=sizeof(prec->xl);
  pdbRecordType->papFldDes[69]->offset=(short)((char *)&prec->xl - (char *)prec);
  pdbRecordType->papFldDes[70]->size=sizeof(prec->yl);
  pdbRecordType->papFldDes[70]->offset=(short)((char *)&prec->yl - (char *)prec);
  pdbRecordType->papFldDes[71]->size=sizeof(prec->zl);
  pdbRecordType->papFldDes[71]->offset=(short)((char *)&prec->zl - (char *)prec);
  pdbRecordType->papFldDes[72]->size=sizeof(prec->axrb);
  pdbRecordType->papFldDes[72]->offset=(short)((char *)&prec->axrb - (char *)prec);
  pdbRecordType->papFldDes[73]->size=sizeof(prec->ayrb);
  pdbRecordType->papFldDes[73]->offset=(short)((char *)&prec->ayrb - (char *)prec);
  pdbRecordType->papFldDes[74]->size=sizeof(prec->azrb);
  pdbRecordType->papFldDes[74]->offset=(short)((char *)&prec->azrb - (char *)prec);
  pdbRecordType->papFldDes[75]->size=sizeof(prec->xrb);
  pdbRecordType->papFldDes[75]->offset=(short)((char *)&prec->xrb - (char *)prec);
  pdbRecordType->papFldDes[76]->size=sizeof(prec->yrb);
  pdbRecordType->papFldDes[76]->offset=(short)((char *)&prec->yrb - (char *)prec);
  pdbRecordType->papFldDes[77]->size=sizeof(prec->zrb);
  pdbRecordType->papFldDes[77]->offset=(short)((char *)&prec->zrb - (char *)prec);
  pdbRecordType->papFldDes[78]->size=sizeof(prec->eax);
  pdbRecordType->papFldDes[78]->offset=(short)((char *)&prec->eax - (char *)prec);
  pdbRecordType->papFldDes[79]->size=sizeof(prec->eay);
  pdbRecordType->papFldDes[79]->offset=(short)((char *)&prec->eay - (char *)prec);
  pdbRecordType->papFldDes[80]->size=sizeof(prec->eaz);
  pdbRecordType->papFldDes[80]->offset=(short)((char *)&prec->eaz - (char *)prec);
  pdbRecordType->papFldDes[81]->size=sizeof(prec->ex);
  pdbRecordType->papFldDes[81]->offset=(short)((char *)&prec->ex - (char *)prec);
  pdbRecordType->papFldDes[82]->size=sizeof(prec->ey);
  pdbRecordType->papFldDes[82]->offset=(short)((char *)&prec->ey - (char *)prec);
  pdbRecordType->papFldDes[83]->size=sizeof(prec->ez);
  pdbRecordType->papFldDes[83]->offset=(short)((char *)&prec->ez - (char *)prec);
  pdbRecordType->papFldDes[84]->size=sizeof(prec->hlax);
  pdbRecordType->papFldDes[84]->offset=(short)((char *)&prec->hlax - (char *)prec);
  pdbRecordType->papFldDes[85]->size=sizeof(prec->hlay);
  pdbRecordType->papFldDes[85]->offset=(short)((char *)&prec->hlay - (char *)prec);
  pdbRecordType->papFldDes[86]->size=sizeof(prec->hlaz);
  pdbRecordType->papFldDes[86]->offset=(short)((char *)&prec->hlaz - (char *)prec);
  pdbRecordType->papFldDes[87]->size=sizeof(prec->hlx);
  pdbRecordType->papFldDes[87]->offset=(short)((char *)&prec->hlx - (char *)prec);
  pdbRecordType->papFldDes[88]->size=sizeof(prec->hly);
  pdbRecordType->papFldDes[88]->offset=(short)((char *)&prec->hly - (char *)prec);
  pdbRecordType->papFldDes[89]->size=sizeof(prec->hlz);
  pdbRecordType->papFldDes[89]->offset=(short)((char *)&prec->hlz - (char *)prec);
  pdbRecordType->papFldDes[90]->size=sizeof(prec->llax);
  pdbRecordType->papFldDes[90]->offset=(short)((char *)&prec->llax - (char *)prec);
  pdbRecordType->papFldDes[91]->size=sizeof(prec->llay);
  pdbRecordType->papFldDes[91]->offset=(short)((char *)&prec->llay - (char *)prec);
  pdbRecordType->papFldDes[92]->size=sizeof(prec->llaz);
  pdbRecordType->papFldDes[92]->offset=(short)((char *)&prec->llaz - (char *)prec);
  pdbRecordType->papFldDes[93]->size=sizeof(prec->llx);
  pdbRecordType->papFldDes[93]->offset=(short)((char *)&prec->llx - (char *)prec);
  pdbRecordType->papFldDes[94]->size=sizeof(prec->lly);
  pdbRecordType->papFldDes[94]->offset=(short)((char *)&prec->lly - (char *)prec);
  pdbRecordType->papFldDes[95]->size=sizeof(prec->llz);
  pdbRecordType->papFldDes[95]->offset=(short)((char *)&prec->llz - (char *)prec);
  pdbRecordType->papFldDes[96]->size=sizeof(prec->uhax);
  pdbRecordType->papFldDes[96]->offset=(short)((char *)&prec->uhax - (char *)prec);
  pdbRecordType->papFldDes[97]->size=sizeof(prec->uhay);
  pdbRecordType->papFldDes[97]->offset=(short)((char *)&prec->uhay - (char *)prec);
  pdbRecordType->papFldDes[98]->size=sizeof(prec->uhaz);
  pdbRecordType->papFldDes[98]->offset=(short)((char *)&prec->uhaz - (char *)prec);
  pdbRecordType->papFldDes[99]->size=sizeof(prec->uhx);
  pdbRecordType->papFldDes[99]->offset=(short)((char *)&prec->uhx - (char *)prec);
  pdbRecordType->papFldDes[100]->size=sizeof(prec->uhy);
  pdbRecordType->papFldDes[100]->offset=(short)((char *)&prec->uhy - (char *)prec);
  pdbRecordType->papFldDes[101]->size=sizeof(prec->uhz);
  pdbRecordType->papFldDes[101]->offset=(short)((char *)&prec->uhz - (char *)prec);
  pdbRecordType->papFldDes[102]->size=sizeof(prec->ulax);
  pdbRecordType->papFldDes[102]->offset=(short)((char *)&prec->ulax - (char *)prec);
  pdbRecordType->papFldDes[103]->size=sizeof(prec->ulay);
  pdbRecordType->papFldDes[103]->offset=(short)((char *)&prec->ulay - (char *)prec);
  pdbRecordType->papFldDes[104]->size=sizeof(prec->ulaz);
  pdbRecordType->papFldDes[104]->offset=(short)((char *)&prec->ulaz - (char *)prec);
  pdbRecordType->papFldDes[105]->size=sizeof(prec->ulx);
  pdbRecordType->papFldDes[105]->offset=(short)((char *)&prec->ulx - (char *)prec);
  pdbRecordType->papFldDes[106]->size=sizeof(prec->uly);
  pdbRecordType->papFldDes[106]->offset=(short)((char *)&prec->uly - (char *)prec);
  pdbRecordType->papFldDes[107]->size=sizeof(prec->ulz);
  pdbRecordType->papFldDes[107]->offset=(short)((char *)&prec->ulz - (char *)prec);
  pdbRecordType->papFldDes[108]->size=sizeof(prec->m0xl);
  pdbRecordType->papFldDes[108]->offset=(short)((char *)&prec->m0xl - (char *)prec);
  pdbRecordType->papFldDes[109]->size=sizeof(prec->m0yl);
  pdbRecordType->papFldDes[109]->offset=(short)((char *)&prec->m0yl - (char *)prec);
  pdbRecordType->papFldDes[110]->size=sizeof(prec->m1yl);
  pdbRecordType->papFldDes[110]->offset=(short)((char *)&prec->m1yl - (char *)prec);
  pdbRecordType->papFldDes[111]->size=sizeof(prec->m2xl);
  pdbRecordType->papFldDes[111]->offset=(short)((char *)&prec->m2xl - (char *)prec);
  pdbRecordType->papFldDes[112]->size=sizeof(prec->m2yl);
  pdbRecordType->papFldDes[112]->offset=(short)((char *)&prec->m2yl - (char *)prec);
  pdbRecordType->papFldDes[113]->size=sizeof(prec->m2zl);
  pdbRecordType->papFldDes[113]->offset=(short)((char *)&prec->m2zl - (char *)prec);
  pdbRecordType->papFldDes[114]->size=sizeof(prec->m0x);
  pdbRecordType->papFldDes[114]->offset=(short)((char *)&prec->m0x - (char *)prec);
  pdbRecordType->papFldDes[115]->size=sizeof(prec->m0y);
  pdbRecordType->papFldDes[115]->offset=(short)((char *)&prec->m0y - (char *)prec);
  pdbRecordType->papFldDes[116]->size=sizeof(prec->m1y);
  pdbRecordType->papFldDes[116]->offset=(short)((char *)&prec->m1y - (char *)prec);
  pdbRecordType->papFldDes[117]->size=sizeof(prec->m2x);
  pdbRecordType->papFldDes[117]->offset=(short)((char *)&prec->m2x - (char *)prec);
  pdbRecordType->papFldDes[118]->size=sizeof(prec->m2y);
  pdbRecordType->papFldDes[118]->offset=(short)((char *)&prec->m2y - (char *)prec);
  pdbRecordType->papFldDes[119]->size=sizeof(prec->m2z);
  pdbRecordType->papFldDes[119]->offset=(short)((char *)&prec->m2z - (char *)prec);
  pdbRecordType->papFldDes[120]->size=sizeof(prec->r0xi);
  pdbRecordType->papFldDes[120]->offset=(short)((char *)&prec->r0xi - (char *)prec);
  pdbRecordType->papFldDes[121]->size=sizeof(prec->r0yi);
  pdbRecordType->papFldDes[121]->offset=(short)((char *)&prec->r0yi - (char *)prec);
  pdbRecordType->papFldDes[122]->size=sizeof(prec->r1yi);
  pdbRecordType->papFldDes[122]->offset=(short)((char *)&prec->r1yi - (char *)prec);
  pdbRecordType->papFldDes[123]->size=sizeof(prec->r2xi);
  pdbRecordType->papFldDes[123]->offset=(short)((char *)&prec->r2xi - (char *)prec);
  pdbRecordType->papFldDes[124]->size=sizeof(prec->r2yi);
  pdbRecordType->papFldDes[124]->offset=(short)((char *)&prec->r2yi - (char *)prec);
  pdbRecordType->papFldDes[125]->size=sizeof(prec->r2zi);
  pdbRecordType->papFldDes[125]->offset=(short)((char *)&prec->r2zi - (char *)prec);
  pdbRecordType->papFldDes[126]->size=sizeof(prec->r0x);
  pdbRecordType->papFldDes[126]->offset=(short)((char *)&prec->r0x - (char *)prec);
  pdbRecordType->papFldDes[127]->size=sizeof(prec->r0y);
  pdbRecordType->papFldDes[127]->offset=(short)((char *)&prec->r0y - (char *)prec);
  pdbRecordType->papFldDes[128]->size=sizeof(prec->r1y);
  pdbRecordType->papFldDes[128]->offset=(short)((char *)&prec->r1y - (char *)prec);
  pdbRecordType->papFldDes[129]->size=sizeof(prec->r2x);
  pdbRecordType->papFldDes[129]->offset=(short)((char *)&prec->r2x - (char *)prec);
  pdbRecordType->papFldDes[130]->size=sizeof(prec->r2y);
  pdbRecordType->papFldDes[130]->offset=(short)((char *)&prec->r2y - (char *)prec);
  pdbRecordType->papFldDes[131]->size=sizeof(prec->r2z);
  pdbRecordType->papFldDes[131]->offset=(short)((char *)&prec->r2z - (char *)prec);
  pdbRecordType->papFldDes[132]->size=sizeof(prec->e0xi);
  pdbRecordType->papFldDes[132]->offset=(short)((char *)&prec->e0xi - (char *)prec);
  pdbRecordType->papFldDes[133]->size=sizeof(prec->e0yi);
  pdbRecordType->papFldDes[133]->offset=(short)((char *)&prec->e0yi - (char *)prec);
  pdbRecordType->papFldDes[134]->size=sizeof(prec->e1yi);
  pdbRecordType->papFldDes[134]->offset=(short)((char *)&prec->e1yi - (char *)prec);
  pdbRecordType->papFldDes[135]->size=sizeof(prec->e2xi);
  pdbRecordType->papFldDes[135]->offset=(short)((char *)&prec->e2xi - (char *)prec);
  pdbRecordType->papFldDes[136]->size=sizeof(prec->e2yi);
  pdbRecordType->papFldDes[136]->offset=(short)((char *)&prec->e2yi - (char *)prec);
  pdbRecordType->papFldDes[137]->size=sizeof(prec->e2zi);
  pdbRecordType->papFldDes[137]->offset=(short)((char *)&prec->e2zi - (char *)prec);
  pdbRecordType->papFldDes[138]->size=sizeof(prec->e0x);
  pdbRecordType->papFldDes[138]->offset=(short)((char *)&prec->e0x - (char *)prec);
  pdbRecordType->papFldDes[139]->size=sizeof(prec->e0y);
  pdbRecordType->papFldDes[139]->offset=(short)((char *)&prec->e0y - (char *)prec);
  pdbRecordType->papFldDes[140]->size=sizeof(prec->e1y);
  pdbRecordType->papFldDes[140]->offset=(short)((char *)&prec->e1y - (char *)prec);
  pdbRecordType->papFldDes[141]->size=sizeof(prec->e2x);
  pdbRecordType->papFldDes[141]->offset=(short)((char *)&prec->e2x - (char *)prec);
  pdbRecordType->papFldDes[142]->size=sizeof(prec->e2y);
  pdbRecordType->papFldDes[142]->offset=(short)((char *)&prec->e2y - (char *)prec);
  pdbRecordType->papFldDes[143]->size=sizeof(prec->e2z);
  pdbRecordType->papFldDes[143]->offset=(short)((char *)&prec->e2z - (char *)prec);
  pdbRecordType->papFldDes[144]->size=sizeof(prec->v0xl);
  pdbRecordType->papFldDes[144]->offset=(short)((char *)&prec->v0xl - (char *)prec);
  pdbRecordType->papFldDes[145]->size=sizeof(prec->v0yl);
  pdbRecordType->papFldDes[145]->offset=(short)((char *)&prec->v0yl - (char *)prec);
  pdbRecordType->papFldDes[146]->size=sizeof(prec->v1yl);
  pdbRecordType->papFldDes[146]->offset=(short)((char *)&prec->v1yl - (char *)prec);
  pdbRecordType->papFldDes[147]->size=sizeof(prec->v2xl);
  pdbRecordType->papFldDes[147]->offset=(short)((char *)&prec->v2xl - (char *)prec);
  pdbRecordType->papFldDes[148]->size=sizeof(prec->v2yl);
  pdbRecordType->papFldDes[148]->offset=(short)((char *)&prec->v2yl - (char *)prec);
  pdbRecordType->papFldDes[149]->size=sizeof(prec->v2zl);
  pdbRecordType->papFldDes[149]->offset=(short)((char *)&prec->v2zl - (char *)prec);
  pdbRecordType->papFldDes[150]->size=sizeof(prec->v0x);
  pdbRecordType->papFldDes[150]->offset=(short)((char *)&prec->v0x - (char *)prec);
  pdbRecordType->papFldDes[151]->size=sizeof(prec->v0y);
  pdbRecordType->papFldDes[151]->offset=(short)((char *)&prec->v0y - (char *)prec);
  pdbRecordType->papFldDes[152]->size=sizeof(prec->v1y);
  pdbRecordType->papFldDes[152]->offset=(short)((char *)&prec->v1y - (char *)prec);
  pdbRecordType->papFldDes[153]->size=sizeof(prec->v2x);
  pdbRecordType->papFldDes[153]->offset=(short)((char *)&prec->v2x - (char *)prec);
  pdbRecordType->papFldDes[154]->size=sizeof(prec->v2y);
  pdbRecordType->papFldDes[154]->offset=(short)((char *)&prec->v2y - (char *)prec);
  pdbRecordType->papFldDes[155]->size=sizeof(prec->v2z);
  pdbRecordType->papFldDes[155]->offset=(short)((char *)&prec->v2z - (char *)prec);
  pdbRecordType->papFldDes[156]->size=sizeof(prec->v0xi);
  pdbRecordType->papFldDes[156]->offset=(short)((char *)&prec->v0xi - (char *)prec);
  pdbRecordType->papFldDes[157]->size=sizeof(prec->v0yi);
  pdbRecordType->papFldDes[157]->offset=(short)((char *)&prec->v0yi - (char *)prec);
  pdbRecordType->papFldDes[158]->size=sizeof(prec->v1yi);
  pdbRecordType->papFldDes[158]->offset=(short)((char *)&prec->v1yi - (char *)prec);
  pdbRecordType->papFldDes[159]->size=sizeof(prec->v2xi);
  pdbRecordType->papFldDes[159]->offset=(short)((char *)&prec->v2xi - (char *)prec);
  pdbRecordType->papFldDes[160]->size=sizeof(prec->v2yi);
  pdbRecordType->papFldDes[160]->offset=(short)((char *)&prec->v2yi - (char *)prec);
  pdbRecordType->papFldDes[161]->size=sizeof(prec->v2zi);
  pdbRecordType->papFldDes[161]->offset=(short)((char *)&prec->v2zi - (char *)prec);
  pdbRecordType->papFldDes[162]->size=sizeof(prec->h0xl);
  pdbRecordType->papFldDes[162]->offset=(short)((char *)&prec->h0xl - (char *)prec);
  pdbRecordType->papFldDes[163]->size=sizeof(prec->h0yl);
  pdbRecordType->papFldDes[163]->offset=(short)((char *)&prec->h0yl - (char *)prec);
  pdbRecordType->papFldDes[164]->size=sizeof(prec->h1yl);
  pdbRecordType->papFldDes[164]->offset=(short)((char *)&prec->h1yl - (char *)prec);
  pdbRecordType->papFldDes[165]->size=sizeof(prec->h2xl);
  pdbRecordType->papFldDes[165]->offset=(short)((char *)&prec->h2xl - (char *)prec);
  pdbRecordType->papFldDes[166]->size=sizeof(prec->h2yl);
  pdbRecordType->papFldDes[166]->offset=(short)((char *)&prec->h2yl - (char *)prec);
  pdbRecordType->papFldDes[167]->size=sizeof(prec->h2zl);
  pdbRecordType->papFldDes[167]->offset=(short)((char *)&prec->h2zl - (char *)prec);
  pdbRecordType->papFldDes[168]->size=sizeof(prec->h0x);
  pdbRecordType->papFldDes[168]->offset=(short)((char *)&prec->h0x - (char *)prec);
  pdbRecordType->papFldDes[169]->size=sizeof(prec->h0y);
  pdbRecordType->papFldDes[169]->offset=(short)((char *)&prec->h0y - (char *)prec);
  pdbRecordType->papFldDes[170]->size=sizeof(prec->h1y);
  pdbRecordType->papFldDes[170]->offset=(short)((char *)&prec->h1y - (char *)prec);
  pdbRecordType->papFldDes[171]->size=sizeof(prec->h2x);
  pdbRecordType->papFldDes[171]->offset=(short)((char *)&prec->h2x - (char *)prec);
  pdbRecordType->papFldDes[172]->size=sizeof(prec->h2y);
  pdbRecordType->papFldDes[172]->offset=(short)((char *)&prec->h2y - (char *)prec);
  pdbRecordType->papFldDes[173]->size=sizeof(prec->h2z);
  pdbRecordType->papFldDes[173]->offset=(short)((char *)&prec->h2z - (char *)prec);
  pdbRecordType->papFldDes[174]->size=sizeof(prec->l0xl);
  pdbRecordType->papFldDes[174]->offset=(short)((char *)&prec->l0xl - (char *)prec);
  pdbRecordType->papFldDes[175]->size=sizeof(prec->l0yl);
  pdbRecordType->papFldDes[175]->offset=(short)((char *)&prec->l0yl - (char *)prec);
  pdbRecordType->papFldDes[176]->size=sizeof(prec->l1yl);
  pdbRecordType->papFldDes[176]->offset=(short)((char *)&prec->l1yl - (char *)prec);
  pdbRecordType->papFldDes[177]->size=sizeof(prec->l2xl);
  pdbRecordType->papFldDes[177]->offset=(short)((char *)&prec->l2xl - (char *)prec);
  pdbRecordType->papFldDes[178]->size=sizeof(prec->l2yl);
  pdbRecordType->papFldDes[178]->offset=(short)((char *)&prec->l2yl - (char *)prec);
  pdbRecordType->papFldDes[179]->size=sizeof(prec->l2zl);
  pdbRecordType->papFldDes[179]->offset=(short)((char *)&prec->l2zl - (char *)prec);
  pdbRecordType->papFldDes[180]->size=sizeof(prec->l0x);
  pdbRecordType->papFldDes[180]->offset=(short)((char *)&prec->l0x - (char *)prec);
  pdbRecordType->papFldDes[181]->size=sizeof(prec->l0y);
  pdbRecordType->papFldDes[181]->offset=(short)((char *)&prec->l0y - (char *)prec);
  pdbRecordType->papFldDes[182]->size=sizeof(prec->l1y);
  pdbRecordType->papFldDes[182]->offset=(short)((char *)&prec->l1y - (char *)prec);
  pdbRecordType->papFldDes[183]->size=sizeof(prec->l2x);
  pdbRecordType->papFldDes[183]->offset=(short)((char *)&prec->l2x - (char *)prec);
  pdbRecordType->papFldDes[184]->size=sizeof(prec->l2y);
  pdbRecordType->papFldDes[184]->offset=(short)((char *)&prec->l2y - (char *)prec);
  pdbRecordType->papFldDes[185]->size=sizeof(prec->l2z);
  pdbRecordType->papFldDes[185]->offset=(short)((char *)&prec->l2z - (char *)prec);
  pdbRecordType->papFldDes[186]->size=sizeof(prec->init);
  pdbRecordType->papFldDes[186]->offset=(short)((char *)&prec->init - (char *)prec);
  pdbRecordType->papFldDes[187]->size=sizeof(prec->zero);
  pdbRecordType->papFldDes[187]->offset=(short)((char *)&prec->zero - (char *)prec);
  pdbRecordType->papFldDes[188]->size=sizeof(prec->sync);
  pdbRecordType->papFldDes[188]->offset=(short)((char *)&prec->sync - (char *)prec);
  pdbRecordType->papFldDes[189]->size=sizeof(prec->read);
  pdbRecordType->papFldDes[189]->offset=(short)((char *)&prec->read - (char *)prec);
  pdbRecordType->papFldDes[190]->size=sizeof(prec->set);
  pdbRecordType->papFldDes[190]->offset=(short)((char *)&prec->set - (char *)prec);
  pdbRecordType->papFldDes[191]->size=sizeof(prec->sset);
  pdbRecordType->papFldDes[191]->offset=(short)((char *)&prec->sset - (char *)prec);
  pdbRecordType->papFldDes[192]->size=sizeof(prec->suse);
  pdbRecordType->papFldDes[192]->offset=(short)((char *)&prec->suse - (char *)prec);
  pdbRecordType->papFldDes[193]->size=sizeof(prec->lvio);
  pdbRecordType->papFldDes[193]->offset=(short)((char *)&prec->lvio - (char *)prec);
  pdbRecordType->papFldDes[194]->size=sizeof(prec->pp0);
  pdbRecordType->papFldDes[194]->offset=(short)((char *)&prec->pp0 - (char *)prec);
  pdbRecordType->papFldDes[195]->size=sizeof(prec->pp1);
  pdbRecordType->papFldDes[195]->offset=(short)((char *)&prec->pp1 - (char *)prec);
  pdbRecordType->papFldDes[196]->size=sizeof(prec->pp2);
  pdbRecordType->papFldDes[196]->offset=(short)((char *)&prec->pp2 - (char *)prec);
  pdbRecordType->papFldDes[197]->size=sizeof(prec->ppo0);
  pdbRecordType->papFldDes[197]->offset=(short)((char *)&prec->ppo0 - (char *)prec);
  pdbRecordType->papFldDes[198]->size=sizeof(prec->ppo1);
  pdbRecordType->papFldDes[198]->offset=(short)((char *)&prec->ppo1 - (char *)prec);
  pdbRecordType->papFldDes[199]->size=sizeof(prec->ppo2);
  pdbRecordType->papFldDes[199]->offset=(short)((char *)&prec->ppo2 - (char *)prec);
  pdbRecordType->papFldDes[200]->size=sizeof(prec->a);
  pdbRecordType->papFldDes[200]->offset=(short)((char *)&prec->a - (char *)prec);
  pdbRecordType->papFldDes[201]->size=sizeof(prec->b);
  pdbRecordType->papFldDes[201]->offset=(short)((char *)&prec->b - (char *)prec);
  pdbRecordType->papFldDes[202]->size=sizeof(prec->legu);
  pdbRecordType->papFldDes[202]->offset=(short)((char *)&prec->legu - (char *)prec);
  pdbRecordType->papFldDes[203]->size=sizeof(prec->aegu);
  pdbRecordType->papFldDes[203]->offset=(short)((char *)&prec->aegu - (char *)prec);
  pdbRecordType->papFldDes[204]->size=sizeof(prec->prec);
  pdbRecordType->papFldDes[204]->offset=(short)((char *)&prec->prec - (char *)prec);
  pdbRecordType->papFldDes[205]->size=sizeof(prec->mmap);
  pdbRecordType->papFldDes[205]->offset=(short)((char *)&prec->mmap - (char *)prec);
  pdbRecordType->papFldDes[206]->size=sizeof(prec->geom);
  pdbRecordType->papFldDes[206]->offset=(short)((char *)&prec->geom - (char *)prec);
    pdbRecordType->rec_size = sizeof(*prec);
    return(0);
}
epicsExportRegistrar(tableRecordSizeOffset);
#ifdef __cplusplus
}
#endif
#endif /*GEN_SIZE_OFFSET*/
