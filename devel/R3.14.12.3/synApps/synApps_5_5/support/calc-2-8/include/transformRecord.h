
#ifndef INCtransformIVLAH
#define INCtransformIVLAH
typedef enum {
	transformIVLA_IGNORE,
	transformIVLA_DO_NOTHING
}transformIVLA;
#endif /*INCtransformIVLAH*/

#ifndef INCtransformIAVH
#define INCtransformIAVH
typedef enum {
	transformIAV_EXT_NC,
	transformIAV_EXT,
	transformIAV_LOC,
	transformIAV_CON
}transformIAV;
#endif /*INCtransformIAVH*/

#ifndef INCtransformCOPTH
#define INCtransformCOPTH
typedef enum {
	transformCOPT_CONDITIONAL,
	transformCOPT_ALWAYS
}transformCOPT;
#endif /*INCtransformCOPTH*/
#ifndef INCtransformH
#define INCtransformH
#include "epicsTypes.h"
#include "link.h"
#include "epicsMutex.h"
#include "ellLib.h"
#include "epicsTime.h"
typedef struct transformRecord {
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
	void *         rpvt;	/* Record Private */
	epicsEnum16	ivla;	/* Invalid link action */
	epicsEnum16	copt;	/* Calc option */
	epicsFloat64	val;	/* Result */
	char		clca[40];	/* Calculation A */
	char		clcb[40];	/* Calculation B */
	char		clcc[40];	/* Calculation C */
	char		clcd[40];	/* Calculation D */
	char		clce[40];	/* Calculation E */
	char		clcf[40];	/* Calculation F */
	char		clcg[40];	/* Calculation G */
	char		clch[40];	/* Calculation H */
	char		clci[40];	/* Calculation I */
	char		clcj[40];	/* Calculation J */
	char		clck[40];	/* Calculation K */
	char		clcl[40];	/* Calculation L */
	char		clcm[40];	/* Calculation M */
	char		clcn[40];	/* Calculation N */
	char		clco[40];	/* Calculation O */
	char		clcp[40];	/* Calculation P */
	epicsInt32	cav;	/* CALC Invalid */
	epicsInt32	cbv;	/* CALC Invalid */
	epicsInt32	ccv;	/* CALC Invalid */
	epicsInt32	cdv;	/* CALC Invalid */
	epicsInt32	cev;	/* CALC Invalid */
	epicsInt32	cfv;	/* CALC Invalid */
	epicsInt32	cgv;	/* CALC Invalid */
	epicsInt32	chv;	/* CALC Invalid */
	epicsInt32	civ;	/* CALC Invalid */
	epicsInt32	cjv;	/* CALC Invalid */
	epicsInt32	ckv;	/* CALC Invalid */
	epicsInt32	clv;	/* CALC Invalid */
	epicsInt32	cmv;	/* CALC Invalid */
	epicsInt32	cnv;	/* CALC Invalid */
	epicsInt32	cov;	/* CALC Invalid */
	epicsInt32	cpv;	/* CALC Invalid */
	DBLINK		inpa;	/* Input A */
	DBLINK		inpb;	/* Input B */
	DBLINK		inpc;	/* Input C */
	DBLINK		inpd;	/* Input D */
	DBLINK		inpe;	/* Input E */
	DBLINK		inpf;	/* Input F */
	DBLINK		inpg;	/* Input G */
	DBLINK		inph;	/* Input H */
	DBLINK		inpi;	/* Input I */
	DBLINK		inpj;	/* Input J */
	DBLINK		inpk;	/* Input K */
	DBLINK		inpl;	/* Input L */
	DBLINK		inpm;	/* Input M */
	DBLINK		inpn;	/* Input N */
	DBLINK		inpo;	/* Input O */
	DBLINK		inpp;	/* Input P */
	DBLINK		outa;	/* Output A */
	DBLINK		outb;	/* Output B */
	DBLINK		outc;	/* Output C */
	DBLINK		outd;	/* Output D */
	DBLINK		oute;	/* Output E */
	DBLINK		outf;	/* Output F */
	DBLINK		outg;	/* Output G */
	DBLINK		outh;	/* Output H */
	DBLINK		outi;	/* Output I */
	DBLINK		outj;	/* Output J */
	DBLINK		outk;	/* Output K */
	DBLINK		outl;	/* Output L */
	DBLINK		outm;	/* Output M */
	DBLINK		outn;	/* Output N */
	DBLINK		outo;	/* Output O */
	DBLINK		outp;	/* Output P */
	char		egu[16];	/* Units Name */
	epicsInt16	prec;	/* Display Precision */
	epicsFloat64	a;	/* Value of A */
	epicsFloat64	b;	/* Value of B */
	epicsFloat64	c;	/* Value of C */
	epicsFloat64	d;	/* Value of D */
	epicsFloat64	e;	/* Value of E */
	epicsFloat64	f;	/* Value of F */
	epicsFloat64	g;	/* Value of G */
	epicsFloat64	h;	/* Value of H */
	epicsFloat64	i;	/* Value of I */
	epicsFloat64	j;	/* Value of J */
	epicsFloat64	k;	/* Value of K */
	epicsFloat64	l;	/* Value of L */
	epicsFloat64	m;	/* Value of M */
	epicsFloat64	n;	/* Value of N */
	epicsFloat64	o;	/* Value of O */
	epicsFloat64	p;	/* Value of P */
	epicsFloat64	la;	/* Prev Value of A */
	epicsFloat64	lb;	/* Prev Value of B */
	epicsFloat64	lc;	/* Prev Value of C */
	epicsFloat64	ld;	/* Prev Value of D */
	epicsFloat64	le;	/* Prev Value of E */
	epicsFloat64	lf;	/* Prev Value of F */
	epicsFloat64	lg;	/* Prev Value of G */
	epicsFloat64	lh;	/* Prev Value of H */
	epicsFloat64	li;	/* Prev Value of I */
	epicsFloat64	lj;	/* Prev Value of J */
	epicsFloat64	lk;	/* Prev Value of K */
	epicsFloat64	ll;	/* Prev Value of L */
	epicsFloat64	lm;	/* Prev Value of M */
	epicsFloat64	ln;	/* Prev Value of N */
	epicsFloat64	lo;	/* Prev Value of O */
	epicsFloat64	lp;	/* Prev Value of P */
	char	rpca[240];	/* Postfix Calc A */
	char	rpcb[240];	/* Postfix Calc B */
	char	rpcc[240];	/* Postfix Calc C */
	char	rpcd[240];	/* Postfix Calc D */
	char	rpce[240];	/* Postfix Calc E */
	char	rpcf[240];	/* Postfix Calc F */
	char	rpcg[240];	/* Postfix Calc G */
	char	rpch[240];	/* Postfix Calc H */
	char	rpci[240];	/* Postfix Calc I */
	char	rpcj[240];	/* Postfix Calc J */
	char	rpck[240];	/* Postfix Calc K */
	char	rpcl[240];	/* Postfix Calc L */
	char	rpcm[240];	/* Postfix Calc M */
	char	rpcn[240];	/* Postfix Calc N */
	char	rpco[240];	/* Postfix Calc O */
	char	rpcp[240];	/* Postfix Calc P */
	char		cmta[39];	/* Comment A */
	char		cmtb[39];	/* Comment B */
	char		cmtc[39];	/* Comment C */
	char		cmtd[39];	/* Comment D */
	char		cmte[39];	/* Comment E */
	char		cmtf[39];	/* Comment F */
	char		cmtg[39];	/* Comment G */
	char		cmth[39];	/* Comment H */
	char		cmti[39];	/* Comment I */
	char		cmtj[39];	/* Comment J */
	char		cmtk[39];	/* Comment K */
	char		cmtl[39];	/* Comment L */
	char		cmtm[39];	/* Comment M */
	char		cmtn[39];	/* Comment N */
	char		cmto[39];	/* Comment O */
	char		cmtp[39];	/* Comment P */
	epicsInt16	map;	/* Input bitmap */
	epicsEnum16	iav;	/* Input link valid */
	epicsEnum16	ibv;	/* Input link valid */
	epicsEnum16	icv;	/* Input link valid */
	epicsEnum16	idv;	/* Input link valid */
	epicsEnum16	iev;	/* Input link valid */
	epicsEnum16	ifv;	/* Input link valid */
	epicsEnum16	igv;	/* Input link valid */
	epicsEnum16	ihv;	/* Input link valid */
	epicsEnum16	iiv;	/* Input link valid */
	epicsEnum16	ijv;	/* Input link valid */
	epicsEnum16	ikv;	/* Input link valid */
	epicsEnum16	ilv;	/* Input link valid */
	epicsEnum16	imv;	/* Input link valid */
	epicsEnum16	inv;	/* Input link valid */
	epicsEnum16	iov;	/* Input link valid */
	epicsEnum16	ipv;	/* Input link valid */
	epicsEnum16	oav;	/* Output link valid */
	epicsEnum16	obv;	/* Output link valid */
	epicsEnum16	ocv;	/* Output link valid */
	epicsEnum16	odv;	/* Output link valid */
	epicsEnum16	oev;	/* Output link valid */
	epicsEnum16	ofv;	/* Output link valid */
	epicsEnum16	ogv;	/* Output link valid */
	epicsEnum16	ohv;	/* Output link valid */
	epicsEnum16	oiv;	/* Output link valid */
	epicsEnum16	ojv;	/* Output link valid */
	epicsEnum16	okv;	/* Output link valid */
	epicsEnum16	olv;	/* Output link valid */
	epicsEnum16	omv;	/* Output link valid */
	epicsEnum16	onv;	/* Output link valid */
	epicsEnum16	oov;	/* Output link valid */
	epicsEnum16	opv;	/* Output link valid */
} transformRecord;
#define transformRecordNAME	0
#define transformRecordDESC	1
#define transformRecordASG	2
#define transformRecordSCAN	3
#define transformRecordPINI	4
#define transformRecordPHAS	5
#define transformRecordEVNT	6
#define transformRecordTSE	7
#define transformRecordTSEL	8
#define transformRecordDTYP	9
#define transformRecordDISV	10
#define transformRecordDISA	11
#define transformRecordSDIS	12
#define transformRecordMLOK	13
#define transformRecordMLIS	14
#define transformRecordDISP	15
#define transformRecordPROC	16
#define transformRecordSTAT	17
#define transformRecordSEVR	18
#define transformRecordNSTA	19
#define transformRecordNSEV	20
#define transformRecordACKS	21
#define transformRecordACKT	22
#define transformRecordDISS	23
#define transformRecordLCNT	24
#define transformRecordPACT	25
#define transformRecordPUTF	26
#define transformRecordRPRO	27
#define transformRecordASP	28
#define transformRecordPPN	29
#define transformRecordPPNR	30
#define transformRecordSPVT	31
#define transformRecordRSET	32
#define transformRecordDSET	33
#define transformRecordDPVT	34
#define transformRecordRDES	35
#define transformRecordLSET	36
#define transformRecordPRIO	37
#define transformRecordTPRO	38
#define transformRecordBKPT	39
#define transformRecordUDF	40
#define transformRecordTIME	41
#define transformRecordFLNK	42
#define transformRecordVERS	43
#define transformRecordRPVT	44
#define transformRecordIVLA	45
#define transformRecordCOPT	46
#define transformRecordVAL	47
#define transformRecordCLCA	48
#define transformRecordCLCB	49
#define transformRecordCLCC	50
#define transformRecordCLCD	51
#define transformRecordCLCE	52
#define transformRecordCLCF	53
#define transformRecordCLCG	54
#define transformRecordCLCH	55
#define transformRecordCLCI	56
#define transformRecordCLCJ	57
#define transformRecordCLCK	58
#define transformRecordCLCL	59
#define transformRecordCLCM	60
#define transformRecordCLCN	61
#define transformRecordCLCO	62
#define transformRecordCLCP	63
#define transformRecordCAV	64
#define transformRecordCBV	65
#define transformRecordCCV	66
#define transformRecordCDV	67
#define transformRecordCEV	68
#define transformRecordCFV	69
#define transformRecordCGV	70
#define transformRecordCHV	71
#define transformRecordCIV	72
#define transformRecordCJV	73
#define transformRecordCKV	74
#define transformRecordCLV	75
#define transformRecordCMV	76
#define transformRecordCNV	77
#define transformRecordCOV	78
#define transformRecordCPV	79
#define transformRecordINPA	80
#define transformRecordINPB	81
#define transformRecordINPC	82
#define transformRecordINPD	83
#define transformRecordINPE	84
#define transformRecordINPF	85
#define transformRecordINPG	86
#define transformRecordINPH	87
#define transformRecordINPI	88
#define transformRecordINPJ	89
#define transformRecordINPK	90
#define transformRecordINPL	91
#define transformRecordINPM	92
#define transformRecordINPN	93
#define transformRecordINPO	94
#define transformRecordINPP	95
#define transformRecordOUTA	96
#define transformRecordOUTB	97
#define transformRecordOUTC	98
#define transformRecordOUTD	99
#define transformRecordOUTE	100
#define transformRecordOUTF	101
#define transformRecordOUTG	102
#define transformRecordOUTH	103
#define transformRecordOUTI	104
#define transformRecordOUTJ	105
#define transformRecordOUTK	106
#define transformRecordOUTL	107
#define transformRecordOUTM	108
#define transformRecordOUTN	109
#define transformRecordOUTO	110
#define transformRecordOUTP	111
#define transformRecordEGU	112
#define transformRecordPREC	113
#define transformRecordA	114
#define transformRecordB	115
#define transformRecordC	116
#define transformRecordD	117
#define transformRecordE	118
#define transformRecordF	119
#define transformRecordG	120
#define transformRecordH	121
#define transformRecordI	122
#define transformRecordJ	123
#define transformRecordK	124
#define transformRecordL	125
#define transformRecordM	126
#define transformRecordN	127
#define transformRecordO	128
#define transformRecordP	129
#define transformRecordLA	130
#define transformRecordLB	131
#define transformRecordLC	132
#define transformRecordLD	133
#define transformRecordLE	134
#define transformRecordLF	135
#define transformRecordLG	136
#define transformRecordLH	137
#define transformRecordLI	138
#define transformRecordLJ	139
#define transformRecordLK	140
#define transformRecordLL	141
#define transformRecordLM	142
#define transformRecordLN	143
#define transformRecordLO	144
#define transformRecordLP	145
#define transformRecordRPCA	146
#define transformRecordRPCB	147
#define transformRecordRPCC	148
#define transformRecordRPCD	149
#define transformRecordRPCE	150
#define transformRecordRPCF	151
#define transformRecordRPCG	152
#define transformRecordRPCH	153
#define transformRecordRPCI	154
#define transformRecordRPCJ	155
#define transformRecordRPCK	156
#define transformRecordRPCL	157
#define transformRecordRPCM	158
#define transformRecordRPCN	159
#define transformRecordRPCO	160
#define transformRecordRPCP	161
#define transformRecordCMTA	162
#define transformRecordCMTB	163
#define transformRecordCMTC	164
#define transformRecordCMTD	165
#define transformRecordCMTE	166
#define transformRecordCMTF	167
#define transformRecordCMTG	168
#define transformRecordCMTH	169
#define transformRecordCMTI	170
#define transformRecordCMTJ	171
#define transformRecordCMTK	172
#define transformRecordCMTL	173
#define transformRecordCMTM	174
#define transformRecordCMTN	175
#define transformRecordCMTO	176
#define transformRecordCMTP	177
#define transformRecordMAP	178
#define transformRecordIAV	179
#define transformRecordIBV	180
#define transformRecordICV	181
#define transformRecordIDV	182
#define transformRecordIEV	183
#define transformRecordIFV	184
#define transformRecordIGV	185
#define transformRecordIHV	186
#define transformRecordIIV	187
#define transformRecordIJV	188
#define transformRecordIKV	189
#define transformRecordILV	190
#define transformRecordIMV	191
#define transformRecordINV	192
#define transformRecordIOV	193
#define transformRecordIPV	194
#define transformRecordOAV	195
#define transformRecordOBV	196
#define transformRecordOCV	197
#define transformRecordODV	198
#define transformRecordOEV	199
#define transformRecordOFV	200
#define transformRecordOGV	201
#define transformRecordOHV	202
#define transformRecordOIV	203
#define transformRecordOJV	204
#define transformRecordOKV	205
#define transformRecordOLV	206
#define transformRecordOMV	207
#define transformRecordONV	208
#define transformRecordOOV	209
#define transformRecordOPV	210
#endif /*INCtransformH*/
#ifdef GEN_SIZE_OFFSET
#ifdef __cplusplus
extern "C" {
#endif
#include <epicsExport.h>
static int transformRecordSizeOffset(dbRecordType *pdbRecordType)
{
    transformRecord *prec = 0;
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
  pdbRecordType->papFldDes[44]->size=sizeof(prec->rpvt);
  pdbRecordType->papFldDes[44]->offset=(short)((char *)&prec->rpvt - (char *)prec);
  pdbRecordType->papFldDes[45]->size=sizeof(prec->ivla);
  pdbRecordType->papFldDes[45]->offset=(short)((char *)&prec->ivla - (char *)prec);
  pdbRecordType->papFldDes[46]->size=sizeof(prec->copt);
  pdbRecordType->papFldDes[46]->offset=(short)((char *)&prec->copt - (char *)prec);
  pdbRecordType->papFldDes[47]->size=sizeof(prec->val);
  pdbRecordType->papFldDes[47]->offset=(short)((char *)&prec->val - (char *)prec);
  pdbRecordType->papFldDes[48]->size=sizeof(prec->clca);
  pdbRecordType->papFldDes[48]->offset=(short)((char *)&prec->clca - (char *)prec);
  pdbRecordType->papFldDes[49]->size=sizeof(prec->clcb);
  pdbRecordType->papFldDes[49]->offset=(short)((char *)&prec->clcb - (char *)prec);
  pdbRecordType->papFldDes[50]->size=sizeof(prec->clcc);
  pdbRecordType->papFldDes[50]->offset=(short)((char *)&prec->clcc - (char *)prec);
  pdbRecordType->papFldDes[51]->size=sizeof(prec->clcd);
  pdbRecordType->papFldDes[51]->offset=(short)((char *)&prec->clcd - (char *)prec);
  pdbRecordType->papFldDes[52]->size=sizeof(prec->clce);
  pdbRecordType->papFldDes[52]->offset=(short)((char *)&prec->clce - (char *)prec);
  pdbRecordType->papFldDes[53]->size=sizeof(prec->clcf);
  pdbRecordType->papFldDes[53]->offset=(short)((char *)&prec->clcf - (char *)prec);
  pdbRecordType->papFldDes[54]->size=sizeof(prec->clcg);
  pdbRecordType->papFldDes[54]->offset=(short)((char *)&prec->clcg - (char *)prec);
  pdbRecordType->papFldDes[55]->size=sizeof(prec->clch);
  pdbRecordType->papFldDes[55]->offset=(short)((char *)&prec->clch - (char *)prec);
  pdbRecordType->papFldDes[56]->size=sizeof(prec->clci);
  pdbRecordType->papFldDes[56]->offset=(short)((char *)&prec->clci - (char *)prec);
  pdbRecordType->papFldDes[57]->size=sizeof(prec->clcj);
  pdbRecordType->papFldDes[57]->offset=(short)((char *)&prec->clcj - (char *)prec);
  pdbRecordType->papFldDes[58]->size=sizeof(prec->clck);
  pdbRecordType->papFldDes[58]->offset=(short)((char *)&prec->clck - (char *)prec);
  pdbRecordType->papFldDes[59]->size=sizeof(prec->clcl);
  pdbRecordType->papFldDes[59]->offset=(short)((char *)&prec->clcl - (char *)prec);
  pdbRecordType->papFldDes[60]->size=sizeof(prec->clcm);
  pdbRecordType->papFldDes[60]->offset=(short)((char *)&prec->clcm - (char *)prec);
  pdbRecordType->papFldDes[61]->size=sizeof(prec->clcn);
  pdbRecordType->papFldDes[61]->offset=(short)((char *)&prec->clcn - (char *)prec);
  pdbRecordType->papFldDes[62]->size=sizeof(prec->clco);
  pdbRecordType->papFldDes[62]->offset=(short)((char *)&prec->clco - (char *)prec);
  pdbRecordType->papFldDes[63]->size=sizeof(prec->clcp);
  pdbRecordType->papFldDes[63]->offset=(short)((char *)&prec->clcp - (char *)prec);
  pdbRecordType->papFldDes[64]->size=sizeof(prec->cav);
  pdbRecordType->papFldDes[64]->offset=(short)((char *)&prec->cav - (char *)prec);
  pdbRecordType->papFldDes[65]->size=sizeof(prec->cbv);
  pdbRecordType->papFldDes[65]->offset=(short)((char *)&prec->cbv - (char *)prec);
  pdbRecordType->papFldDes[66]->size=sizeof(prec->ccv);
  pdbRecordType->papFldDes[66]->offset=(short)((char *)&prec->ccv - (char *)prec);
  pdbRecordType->papFldDes[67]->size=sizeof(prec->cdv);
  pdbRecordType->papFldDes[67]->offset=(short)((char *)&prec->cdv - (char *)prec);
  pdbRecordType->papFldDes[68]->size=sizeof(prec->cev);
  pdbRecordType->papFldDes[68]->offset=(short)((char *)&prec->cev - (char *)prec);
  pdbRecordType->papFldDes[69]->size=sizeof(prec->cfv);
  pdbRecordType->papFldDes[69]->offset=(short)((char *)&prec->cfv - (char *)prec);
  pdbRecordType->papFldDes[70]->size=sizeof(prec->cgv);
  pdbRecordType->papFldDes[70]->offset=(short)((char *)&prec->cgv - (char *)prec);
  pdbRecordType->papFldDes[71]->size=sizeof(prec->chv);
  pdbRecordType->papFldDes[71]->offset=(short)((char *)&prec->chv - (char *)prec);
  pdbRecordType->papFldDes[72]->size=sizeof(prec->civ);
  pdbRecordType->papFldDes[72]->offset=(short)((char *)&prec->civ - (char *)prec);
  pdbRecordType->papFldDes[73]->size=sizeof(prec->cjv);
  pdbRecordType->papFldDes[73]->offset=(short)((char *)&prec->cjv - (char *)prec);
  pdbRecordType->papFldDes[74]->size=sizeof(prec->ckv);
  pdbRecordType->papFldDes[74]->offset=(short)((char *)&prec->ckv - (char *)prec);
  pdbRecordType->papFldDes[75]->size=sizeof(prec->clv);
  pdbRecordType->papFldDes[75]->offset=(short)((char *)&prec->clv - (char *)prec);
  pdbRecordType->papFldDes[76]->size=sizeof(prec->cmv);
  pdbRecordType->papFldDes[76]->offset=(short)((char *)&prec->cmv - (char *)prec);
  pdbRecordType->papFldDes[77]->size=sizeof(prec->cnv);
  pdbRecordType->papFldDes[77]->offset=(short)((char *)&prec->cnv - (char *)prec);
  pdbRecordType->papFldDes[78]->size=sizeof(prec->cov);
  pdbRecordType->papFldDes[78]->offset=(short)((char *)&prec->cov - (char *)prec);
  pdbRecordType->papFldDes[79]->size=sizeof(prec->cpv);
  pdbRecordType->papFldDes[79]->offset=(short)((char *)&prec->cpv - (char *)prec);
  pdbRecordType->papFldDes[80]->size=sizeof(prec->inpa);
  pdbRecordType->papFldDes[80]->offset=(short)((char *)&prec->inpa - (char *)prec);
  pdbRecordType->papFldDes[81]->size=sizeof(prec->inpb);
  pdbRecordType->papFldDes[81]->offset=(short)((char *)&prec->inpb - (char *)prec);
  pdbRecordType->papFldDes[82]->size=sizeof(prec->inpc);
  pdbRecordType->papFldDes[82]->offset=(short)((char *)&prec->inpc - (char *)prec);
  pdbRecordType->papFldDes[83]->size=sizeof(prec->inpd);
  pdbRecordType->papFldDes[83]->offset=(short)((char *)&prec->inpd - (char *)prec);
  pdbRecordType->papFldDes[84]->size=sizeof(prec->inpe);
  pdbRecordType->papFldDes[84]->offset=(short)((char *)&prec->inpe - (char *)prec);
  pdbRecordType->papFldDes[85]->size=sizeof(prec->inpf);
  pdbRecordType->papFldDes[85]->offset=(short)((char *)&prec->inpf - (char *)prec);
  pdbRecordType->papFldDes[86]->size=sizeof(prec->inpg);
  pdbRecordType->papFldDes[86]->offset=(short)((char *)&prec->inpg - (char *)prec);
  pdbRecordType->papFldDes[87]->size=sizeof(prec->inph);
  pdbRecordType->papFldDes[87]->offset=(short)((char *)&prec->inph - (char *)prec);
  pdbRecordType->papFldDes[88]->size=sizeof(prec->inpi);
  pdbRecordType->papFldDes[88]->offset=(short)((char *)&prec->inpi - (char *)prec);
  pdbRecordType->papFldDes[89]->size=sizeof(prec->inpj);
  pdbRecordType->papFldDes[89]->offset=(short)((char *)&prec->inpj - (char *)prec);
  pdbRecordType->papFldDes[90]->size=sizeof(prec->inpk);
  pdbRecordType->papFldDes[90]->offset=(short)((char *)&prec->inpk - (char *)prec);
  pdbRecordType->papFldDes[91]->size=sizeof(prec->inpl);
  pdbRecordType->papFldDes[91]->offset=(short)((char *)&prec->inpl - (char *)prec);
  pdbRecordType->papFldDes[92]->size=sizeof(prec->inpm);
  pdbRecordType->papFldDes[92]->offset=(short)((char *)&prec->inpm - (char *)prec);
  pdbRecordType->papFldDes[93]->size=sizeof(prec->inpn);
  pdbRecordType->papFldDes[93]->offset=(short)((char *)&prec->inpn - (char *)prec);
  pdbRecordType->papFldDes[94]->size=sizeof(prec->inpo);
  pdbRecordType->papFldDes[94]->offset=(short)((char *)&prec->inpo - (char *)prec);
  pdbRecordType->papFldDes[95]->size=sizeof(prec->inpp);
  pdbRecordType->papFldDes[95]->offset=(short)((char *)&prec->inpp - (char *)prec);
  pdbRecordType->papFldDes[96]->size=sizeof(prec->outa);
  pdbRecordType->papFldDes[96]->offset=(short)((char *)&prec->outa - (char *)prec);
  pdbRecordType->papFldDes[97]->size=sizeof(prec->outb);
  pdbRecordType->papFldDes[97]->offset=(short)((char *)&prec->outb - (char *)prec);
  pdbRecordType->papFldDes[98]->size=sizeof(prec->outc);
  pdbRecordType->papFldDes[98]->offset=(short)((char *)&prec->outc - (char *)prec);
  pdbRecordType->papFldDes[99]->size=sizeof(prec->outd);
  pdbRecordType->papFldDes[99]->offset=(short)((char *)&prec->outd - (char *)prec);
  pdbRecordType->papFldDes[100]->size=sizeof(prec->oute);
  pdbRecordType->papFldDes[100]->offset=(short)((char *)&prec->oute - (char *)prec);
  pdbRecordType->papFldDes[101]->size=sizeof(prec->outf);
  pdbRecordType->papFldDes[101]->offset=(short)((char *)&prec->outf - (char *)prec);
  pdbRecordType->papFldDes[102]->size=sizeof(prec->outg);
  pdbRecordType->papFldDes[102]->offset=(short)((char *)&prec->outg - (char *)prec);
  pdbRecordType->papFldDes[103]->size=sizeof(prec->outh);
  pdbRecordType->papFldDes[103]->offset=(short)((char *)&prec->outh - (char *)prec);
  pdbRecordType->papFldDes[104]->size=sizeof(prec->outi);
  pdbRecordType->papFldDes[104]->offset=(short)((char *)&prec->outi - (char *)prec);
  pdbRecordType->papFldDes[105]->size=sizeof(prec->outj);
  pdbRecordType->papFldDes[105]->offset=(short)((char *)&prec->outj - (char *)prec);
  pdbRecordType->papFldDes[106]->size=sizeof(prec->outk);
  pdbRecordType->papFldDes[106]->offset=(short)((char *)&prec->outk - (char *)prec);
  pdbRecordType->papFldDes[107]->size=sizeof(prec->outl);
  pdbRecordType->papFldDes[107]->offset=(short)((char *)&prec->outl - (char *)prec);
  pdbRecordType->papFldDes[108]->size=sizeof(prec->outm);
  pdbRecordType->papFldDes[108]->offset=(short)((char *)&prec->outm - (char *)prec);
  pdbRecordType->papFldDes[109]->size=sizeof(prec->outn);
  pdbRecordType->papFldDes[109]->offset=(short)((char *)&prec->outn - (char *)prec);
  pdbRecordType->papFldDes[110]->size=sizeof(prec->outo);
  pdbRecordType->papFldDes[110]->offset=(short)((char *)&prec->outo - (char *)prec);
  pdbRecordType->papFldDes[111]->size=sizeof(prec->outp);
  pdbRecordType->papFldDes[111]->offset=(short)((char *)&prec->outp - (char *)prec);
  pdbRecordType->papFldDes[112]->size=sizeof(prec->egu);
  pdbRecordType->papFldDes[112]->offset=(short)((char *)&prec->egu - (char *)prec);
  pdbRecordType->papFldDes[113]->size=sizeof(prec->prec);
  pdbRecordType->papFldDes[113]->offset=(short)((char *)&prec->prec - (char *)prec);
  pdbRecordType->papFldDes[114]->size=sizeof(prec->a);
  pdbRecordType->papFldDes[114]->offset=(short)((char *)&prec->a - (char *)prec);
  pdbRecordType->papFldDes[115]->size=sizeof(prec->b);
  pdbRecordType->papFldDes[115]->offset=(short)((char *)&prec->b - (char *)prec);
  pdbRecordType->papFldDes[116]->size=sizeof(prec->c);
  pdbRecordType->papFldDes[116]->offset=(short)((char *)&prec->c - (char *)prec);
  pdbRecordType->papFldDes[117]->size=sizeof(prec->d);
  pdbRecordType->papFldDes[117]->offset=(short)((char *)&prec->d - (char *)prec);
  pdbRecordType->papFldDes[118]->size=sizeof(prec->e);
  pdbRecordType->papFldDes[118]->offset=(short)((char *)&prec->e - (char *)prec);
  pdbRecordType->papFldDes[119]->size=sizeof(prec->f);
  pdbRecordType->papFldDes[119]->offset=(short)((char *)&prec->f - (char *)prec);
  pdbRecordType->papFldDes[120]->size=sizeof(prec->g);
  pdbRecordType->papFldDes[120]->offset=(short)((char *)&prec->g - (char *)prec);
  pdbRecordType->papFldDes[121]->size=sizeof(prec->h);
  pdbRecordType->papFldDes[121]->offset=(short)((char *)&prec->h - (char *)prec);
  pdbRecordType->papFldDes[122]->size=sizeof(prec->i);
  pdbRecordType->papFldDes[122]->offset=(short)((char *)&prec->i - (char *)prec);
  pdbRecordType->papFldDes[123]->size=sizeof(prec->j);
  pdbRecordType->papFldDes[123]->offset=(short)((char *)&prec->j - (char *)prec);
  pdbRecordType->papFldDes[124]->size=sizeof(prec->k);
  pdbRecordType->papFldDes[124]->offset=(short)((char *)&prec->k - (char *)prec);
  pdbRecordType->papFldDes[125]->size=sizeof(prec->l);
  pdbRecordType->papFldDes[125]->offset=(short)((char *)&prec->l - (char *)prec);
  pdbRecordType->papFldDes[126]->size=sizeof(prec->m);
  pdbRecordType->papFldDes[126]->offset=(short)((char *)&prec->m - (char *)prec);
  pdbRecordType->papFldDes[127]->size=sizeof(prec->n);
  pdbRecordType->papFldDes[127]->offset=(short)((char *)&prec->n - (char *)prec);
  pdbRecordType->papFldDes[128]->size=sizeof(prec->o);
  pdbRecordType->papFldDes[128]->offset=(short)((char *)&prec->o - (char *)prec);
  pdbRecordType->papFldDes[129]->size=sizeof(prec->p);
  pdbRecordType->papFldDes[129]->offset=(short)((char *)&prec->p - (char *)prec);
  pdbRecordType->papFldDes[130]->size=sizeof(prec->la);
  pdbRecordType->papFldDes[130]->offset=(short)((char *)&prec->la - (char *)prec);
  pdbRecordType->papFldDes[131]->size=sizeof(prec->lb);
  pdbRecordType->papFldDes[131]->offset=(short)((char *)&prec->lb - (char *)prec);
  pdbRecordType->papFldDes[132]->size=sizeof(prec->lc);
  pdbRecordType->papFldDes[132]->offset=(short)((char *)&prec->lc - (char *)prec);
  pdbRecordType->papFldDes[133]->size=sizeof(prec->ld);
  pdbRecordType->papFldDes[133]->offset=(short)((char *)&prec->ld - (char *)prec);
  pdbRecordType->papFldDes[134]->size=sizeof(prec->le);
  pdbRecordType->papFldDes[134]->offset=(short)((char *)&prec->le - (char *)prec);
  pdbRecordType->papFldDes[135]->size=sizeof(prec->lf);
  pdbRecordType->papFldDes[135]->offset=(short)((char *)&prec->lf - (char *)prec);
  pdbRecordType->papFldDes[136]->size=sizeof(prec->lg);
  pdbRecordType->papFldDes[136]->offset=(short)((char *)&prec->lg - (char *)prec);
  pdbRecordType->papFldDes[137]->size=sizeof(prec->lh);
  pdbRecordType->papFldDes[137]->offset=(short)((char *)&prec->lh - (char *)prec);
  pdbRecordType->papFldDes[138]->size=sizeof(prec->li);
  pdbRecordType->papFldDes[138]->offset=(short)((char *)&prec->li - (char *)prec);
  pdbRecordType->papFldDes[139]->size=sizeof(prec->lj);
  pdbRecordType->papFldDes[139]->offset=(short)((char *)&prec->lj - (char *)prec);
  pdbRecordType->papFldDes[140]->size=sizeof(prec->lk);
  pdbRecordType->papFldDes[140]->offset=(short)((char *)&prec->lk - (char *)prec);
  pdbRecordType->papFldDes[141]->size=sizeof(prec->ll);
  pdbRecordType->papFldDes[141]->offset=(short)((char *)&prec->ll - (char *)prec);
  pdbRecordType->papFldDes[142]->size=sizeof(prec->lm);
  pdbRecordType->papFldDes[142]->offset=(short)((char *)&prec->lm - (char *)prec);
  pdbRecordType->papFldDes[143]->size=sizeof(prec->ln);
  pdbRecordType->papFldDes[143]->offset=(short)((char *)&prec->ln - (char *)prec);
  pdbRecordType->papFldDes[144]->size=sizeof(prec->lo);
  pdbRecordType->papFldDes[144]->offset=(short)((char *)&prec->lo - (char *)prec);
  pdbRecordType->papFldDes[145]->size=sizeof(prec->lp);
  pdbRecordType->papFldDes[145]->offset=(short)((char *)&prec->lp - (char *)prec);
  pdbRecordType->papFldDes[146]->size=sizeof(prec->rpca);
  pdbRecordType->papFldDes[146]->offset=(short)((char *)&prec->rpca - (char *)prec);
  pdbRecordType->papFldDes[147]->size=sizeof(prec->rpcb);
  pdbRecordType->papFldDes[147]->offset=(short)((char *)&prec->rpcb - (char *)prec);
  pdbRecordType->papFldDes[148]->size=sizeof(prec->rpcc);
  pdbRecordType->papFldDes[148]->offset=(short)((char *)&prec->rpcc - (char *)prec);
  pdbRecordType->papFldDes[149]->size=sizeof(prec->rpcd);
  pdbRecordType->papFldDes[149]->offset=(short)((char *)&prec->rpcd - (char *)prec);
  pdbRecordType->papFldDes[150]->size=sizeof(prec->rpce);
  pdbRecordType->papFldDes[150]->offset=(short)((char *)&prec->rpce - (char *)prec);
  pdbRecordType->papFldDes[151]->size=sizeof(prec->rpcf);
  pdbRecordType->papFldDes[151]->offset=(short)((char *)&prec->rpcf - (char *)prec);
  pdbRecordType->papFldDes[152]->size=sizeof(prec->rpcg);
  pdbRecordType->papFldDes[152]->offset=(short)((char *)&prec->rpcg - (char *)prec);
  pdbRecordType->papFldDes[153]->size=sizeof(prec->rpch);
  pdbRecordType->papFldDes[153]->offset=(short)((char *)&prec->rpch - (char *)prec);
  pdbRecordType->papFldDes[154]->size=sizeof(prec->rpci);
  pdbRecordType->papFldDes[154]->offset=(short)((char *)&prec->rpci - (char *)prec);
  pdbRecordType->papFldDes[155]->size=sizeof(prec->rpcj);
  pdbRecordType->papFldDes[155]->offset=(short)((char *)&prec->rpcj - (char *)prec);
  pdbRecordType->papFldDes[156]->size=sizeof(prec->rpck);
  pdbRecordType->papFldDes[156]->offset=(short)((char *)&prec->rpck - (char *)prec);
  pdbRecordType->papFldDes[157]->size=sizeof(prec->rpcl);
  pdbRecordType->papFldDes[157]->offset=(short)((char *)&prec->rpcl - (char *)prec);
  pdbRecordType->papFldDes[158]->size=sizeof(prec->rpcm);
  pdbRecordType->papFldDes[158]->offset=(short)((char *)&prec->rpcm - (char *)prec);
  pdbRecordType->papFldDes[159]->size=sizeof(prec->rpcn);
  pdbRecordType->papFldDes[159]->offset=(short)((char *)&prec->rpcn - (char *)prec);
  pdbRecordType->papFldDes[160]->size=sizeof(prec->rpco);
  pdbRecordType->papFldDes[160]->offset=(short)((char *)&prec->rpco - (char *)prec);
  pdbRecordType->papFldDes[161]->size=sizeof(prec->rpcp);
  pdbRecordType->papFldDes[161]->offset=(short)((char *)&prec->rpcp - (char *)prec);
  pdbRecordType->papFldDes[162]->size=sizeof(prec->cmta);
  pdbRecordType->papFldDes[162]->offset=(short)((char *)&prec->cmta - (char *)prec);
  pdbRecordType->papFldDes[163]->size=sizeof(prec->cmtb);
  pdbRecordType->papFldDes[163]->offset=(short)((char *)&prec->cmtb - (char *)prec);
  pdbRecordType->papFldDes[164]->size=sizeof(prec->cmtc);
  pdbRecordType->papFldDes[164]->offset=(short)((char *)&prec->cmtc - (char *)prec);
  pdbRecordType->papFldDes[165]->size=sizeof(prec->cmtd);
  pdbRecordType->papFldDes[165]->offset=(short)((char *)&prec->cmtd - (char *)prec);
  pdbRecordType->papFldDes[166]->size=sizeof(prec->cmte);
  pdbRecordType->papFldDes[166]->offset=(short)((char *)&prec->cmte - (char *)prec);
  pdbRecordType->papFldDes[167]->size=sizeof(prec->cmtf);
  pdbRecordType->papFldDes[167]->offset=(short)((char *)&prec->cmtf - (char *)prec);
  pdbRecordType->papFldDes[168]->size=sizeof(prec->cmtg);
  pdbRecordType->papFldDes[168]->offset=(short)((char *)&prec->cmtg - (char *)prec);
  pdbRecordType->papFldDes[169]->size=sizeof(prec->cmth);
  pdbRecordType->papFldDes[169]->offset=(short)((char *)&prec->cmth - (char *)prec);
  pdbRecordType->papFldDes[170]->size=sizeof(prec->cmti);
  pdbRecordType->papFldDes[170]->offset=(short)((char *)&prec->cmti - (char *)prec);
  pdbRecordType->papFldDes[171]->size=sizeof(prec->cmtj);
  pdbRecordType->papFldDes[171]->offset=(short)((char *)&prec->cmtj - (char *)prec);
  pdbRecordType->papFldDes[172]->size=sizeof(prec->cmtk);
  pdbRecordType->papFldDes[172]->offset=(short)((char *)&prec->cmtk - (char *)prec);
  pdbRecordType->papFldDes[173]->size=sizeof(prec->cmtl);
  pdbRecordType->papFldDes[173]->offset=(short)((char *)&prec->cmtl - (char *)prec);
  pdbRecordType->papFldDes[174]->size=sizeof(prec->cmtm);
  pdbRecordType->papFldDes[174]->offset=(short)((char *)&prec->cmtm - (char *)prec);
  pdbRecordType->papFldDes[175]->size=sizeof(prec->cmtn);
  pdbRecordType->papFldDes[175]->offset=(short)((char *)&prec->cmtn - (char *)prec);
  pdbRecordType->papFldDes[176]->size=sizeof(prec->cmto);
  pdbRecordType->papFldDes[176]->offset=(short)((char *)&prec->cmto - (char *)prec);
  pdbRecordType->papFldDes[177]->size=sizeof(prec->cmtp);
  pdbRecordType->papFldDes[177]->offset=(short)((char *)&prec->cmtp - (char *)prec);
  pdbRecordType->papFldDes[178]->size=sizeof(prec->map);
  pdbRecordType->papFldDes[178]->offset=(short)((char *)&prec->map - (char *)prec);
  pdbRecordType->papFldDes[179]->size=sizeof(prec->iav);
  pdbRecordType->papFldDes[179]->offset=(short)((char *)&prec->iav - (char *)prec);
  pdbRecordType->papFldDes[180]->size=sizeof(prec->ibv);
  pdbRecordType->papFldDes[180]->offset=(short)((char *)&prec->ibv - (char *)prec);
  pdbRecordType->papFldDes[181]->size=sizeof(prec->icv);
  pdbRecordType->papFldDes[181]->offset=(short)((char *)&prec->icv - (char *)prec);
  pdbRecordType->papFldDes[182]->size=sizeof(prec->idv);
  pdbRecordType->papFldDes[182]->offset=(short)((char *)&prec->idv - (char *)prec);
  pdbRecordType->papFldDes[183]->size=sizeof(prec->iev);
  pdbRecordType->papFldDes[183]->offset=(short)((char *)&prec->iev - (char *)prec);
  pdbRecordType->papFldDes[184]->size=sizeof(prec->ifv);
  pdbRecordType->papFldDes[184]->offset=(short)((char *)&prec->ifv - (char *)prec);
  pdbRecordType->papFldDes[185]->size=sizeof(prec->igv);
  pdbRecordType->papFldDes[185]->offset=(short)((char *)&prec->igv - (char *)prec);
  pdbRecordType->papFldDes[186]->size=sizeof(prec->ihv);
  pdbRecordType->papFldDes[186]->offset=(short)((char *)&prec->ihv - (char *)prec);
  pdbRecordType->papFldDes[187]->size=sizeof(prec->iiv);
  pdbRecordType->papFldDes[187]->offset=(short)((char *)&prec->iiv - (char *)prec);
  pdbRecordType->papFldDes[188]->size=sizeof(prec->ijv);
  pdbRecordType->papFldDes[188]->offset=(short)((char *)&prec->ijv - (char *)prec);
  pdbRecordType->papFldDes[189]->size=sizeof(prec->ikv);
  pdbRecordType->papFldDes[189]->offset=(short)((char *)&prec->ikv - (char *)prec);
  pdbRecordType->papFldDes[190]->size=sizeof(prec->ilv);
  pdbRecordType->papFldDes[190]->offset=(short)((char *)&prec->ilv - (char *)prec);
  pdbRecordType->papFldDes[191]->size=sizeof(prec->imv);
  pdbRecordType->papFldDes[191]->offset=(short)((char *)&prec->imv - (char *)prec);
  pdbRecordType->papFldDes[192]->size=sizeof(prec->inv);
  pdbRecordType->papFldDes[192]->offset=(short)((char *)&prec->inv - (char *)prec);
  pdbRecordType->papFldDes[193]->size=sizeof(prec->iov);
  pdbRecordType->papFldDes[193]->offset=(short)((char *)&prec->iov - (char *)prec);
  pdbRecordType->papFldDes[194]->size=sizeof(prec->ipv);
  pdbRecordType->papFldDes[194]->offset=(short)((char *)&prec->ipv - (char *)prec);
  pdbRecordType->papFldDes[195]->size=sizeof(prec->oav);
  pdbRecordType->papFldDes[195]->offset=(short)((char *)&prec->oav - (char *)prec);
  pdbRecordType->papFldDes[196]->size=sizeof(prec->obv);
  pdbRecordType->papFldDes[196]->offset=(short)((char *)&prec->obv - (char *)prec);
  pdbRecordType->papFldDes[197]->size=sizeof(prec->ocv);
  pdbRecordType->papFldDes[197]->offset=(short)((char *)&prec->ocv - (char *)prec);
  pdbRecordType->papFldDes[198]->size=sizeof(prec->odv);
  pdbRecordType->papFldDes[198]->offset=(short)((char *)&prec->odv - (char *)prec);
  pdbRecordType->papFldDes[199]->size=sizeof(prec->oev);
  pdbRecordType->papFldDes[199]->offset=(short)((char *)&prec->oev - (char *)prec);
  pdbRecordType->papFldDes[200]->size=sizeof(prec->ofv);
  pdbRecordType->papFldDes[200]->offset=(short)((char *)&prec->ofv - (char *)prec);
  pdbRecordType->papFldDes[201]->size=sizeof(prec->ogv);
  pdbRecordType->papFldDes[201]->offset=(short)((char *)&prec->ogv - (char *)prec);
  pdbRecordType->papFldDes[202]->size=sizeof(prec->ohv);
  pdbRecordType->papFldDes[202]->offset=(short)((char *)&prec->ohv - (char *)prec);
  pdbRecordType->papFldDes[203]->size=sizeof(prec->oiv);
  pdbRecordType->papFldDes[203]->offset=(short)((char *)&prec->oiv - (char *)prec);
  pdbRecordType->papFldDes[204]->size=sizeof(prec->ojv);
  pdbRecordType->papFldDes[204]->offset=(short)((char *)&prec->ojv - (char *)prec);
  pdbRecordType->papFldDes[205]->size=sizeof(prec->okv);
  pdbRecordType->papFldDes[205]->offset=(short)((char *)&prec->okv - (char *)prec);
  pdbRecordType->papFldDes[206]->size=sizeof(prec->olv);
  pdbRecordType->papFldDes[206]->offset=(short)((char *)&prec->olv - (char *)prec);
  pdbRecordType->papFldDes[207]->size=sizeof(prec->omv);
  pdbRecordType->papFldDes[207]->offset=(short)((char *)&prec->omv - (char *)prec);
  pdbRecordType->papFldDes[208]->size=sizeof(prec->onv);
  pdbRecordType->papFldDes[208]->offset=(short)((char *)&prec->onv - (char *)prec);
  pdbRecordType->papFldDes[209]->size=sizeof(prec->oov);
  pdbRecordType->papFldDes[209]->offset=(short)((char *)&prec->oov - (char *)prec);
  pdbRecordType->papFldDes[210]->size=sizeof(prec->opv);
  pdbRecordType->papFldDes[210]->offset=(short)((char *)&prec->opv - (char *)prec);
    pdbRecordType->rec_size = sizeof(*prec);
    return(0);
}
epicsExportRegistrar(transformRecordSizeOffset);
#ifdef __cplusplus
}
#endif
#endif /*GEN_SIZE_OFFSET*/
