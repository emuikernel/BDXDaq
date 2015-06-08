/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: orient_st.i */

/* Event flags */
#define Mode_mon	1
#define motPut_mon	2
#define motGet_mon	3
#define motTTH_mon	4
#define motTH_mon	5
#define motCHI_mon	6
#define motPHI_mon	7
#define motRBV_mon	8
#define H_mon	9
#define K_mon	10
#define L_mon	11
#define TTH_mon	12
#define TH_mon	13
#define CHI_mon	14
#define PHI_mon	15
#define xtalParm_mon	16
#define lambda_mon	17
#define energy_mon	18
#define A0_mon	19
#define OMTX_mon	20

/* Program "orient" */
#include "seqCom.h"

#define NUM_SS 4
#define NUM_CHANNELS 128
#define NUM_EVENTS 20
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT FALSE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT TRUE

extern struct seqProgram orient;

/* Variable declarations */
struct UserVar {
	char	tmpstr[100];
	char	*cp;
	short	orientDebug;
	char	Msg[MAX_STRING_SIZE];
	short	Alert;
	short	opAck;
	short	Mode;
	short	motPut;
	short	motPut_Auto;
	short	load_motPut;
	short	motGet;
	short	motGet_Auto;
	char	motTTH_name[MAX_STRING_SIZE];
	char	motTH_name[MAX_STRING_SIZE];
	char	motCHI_name[MAX_STRING_SIZE];
	char	motPHI_name[MAX_STRING_SIZE];
	double	motTTH;
	double	motTH;
	double	motCHI;
	double	motPHI;
	double	motTTH_RBV;
	double	motTH_RBV;
	double	motCHI_RBV;
	double	motPHI_RBV;
	double	rbv_angles[4];
	int	motTTH_stop;
	int	motTH_stop;
	int	motCHI_stop;
	int	motPHI_stop;
	int	stopMotors;
	int	motTTH_Connected;
	int	motTH_Connected;
	int	motCHI_Connected;
	int	motPHI_Connected;
	int	motorsConnected;
	int	motorStopsConnected;
	short	OMTX_Calc;
	short	Busy;
	double	H;
	double	H_event;
	short	H_busy;
	double	loadH;
	double	H_RBV;
	double	K;
	double	K_event;
	short	K_busy;
	double	loadK;
	double	K_RBV;
	double	L;
	double	L_event;
	short	L_busy;
	double	loadL;
	double	L_RBV;
	double	hkl[3];
	double	rbv_hkl[3];
	double	TTH;
	double	TTH_event;
	short	TTH_busy;
	double	loadTTH;
	double	TH;
	double	TH_event;
	short	TH_busy;
	double	loadTH;
	double	CHI;
	double	CHI_event;
	short	CHI_busy;
	double	loadCHI;
	double	PHI;
	double	PHI_event;
	short	PHI_busy;
	double	loadPHI;
	double	angles[4];
	double	H1;
	double	K1;
	double	L1;
	double	TTH1;
	double	TH1;
	double	CHI1;
	double	PHI1;
	short	refGet1;
	short	refPut1;
	double	H2;
	double	K2;
	double	L2;
	double	TTH2;
	double	TH2;
	double	CHI2;
	double	PHI2;
	short	refGet2;
	short	refPut2;
	double	a;
	double	b;
	double	c;
	double	alpha;
	double	beta;
	double	Gamma;
	double	lambda;
	double	lambda_event;
	double	load_lambda;
	double	energy;
	double	energy_event;
	double	load_energy;
	double	hc;
	double	A0[3][3];
	double	A0_i[3][3];
	double	A0_11;
	double	A0_12;
	double	A0_13;
	double	A0_21;
	double	A0_22;
	double	A0_23;
	double	A0_31;
	double	A0_32;
	double	A0_33;
	double	OMTX[3][3];
	double	OMTX_i[3][3];
	double	OMTX_11;
	double	OMTX_12;
	double	OMTX_13;
	double	OMTX_21;
	double	OMTX_22;
	double	OMTX_23;
	double	OMTX_31;
	double	OMTX_32;
	double	OMTX_33;
	double	errAngle;
	double	errAngleThresh;
	double	v1_hkl[3];
	double	v1_angles[4];
	double	v2_hkl[3];
	double	v2_angles[4];
	int	i;
	short	newHKL;
	short	newAngles;
	short	newMotors;
	short	waiting4Mot;
	short	waiting4motTTH;
	short	waiting4motTH;
	short	waiting4motCHI;
	short	waiting4motPHI;
	int	old_connect_state;
	long	A0_state;
	long	OMTX_state;
	int	weWrote_motTTH;
	int	weWrote_motTH;
	int	weWrote_motCHI;
	int	weWrote_motPHI;
	int	startup;
	char	*orient_pmac_PM;
	char	*orient_pmac_mTTH;
	char	*orient_pmac_mTH;
	char	*orient_pmac_mCHI;
	char	*orient_pmac_mPHI;
};

/* C code definitions */
# line 297 "../orient_st.st"
#include <string.h>
# line 298 "../orient_st.st"
#include <math.h>
# line 299 "../orient_st.st"
#include "matrix3.h"
# line 300 "../orient_st.st"
#include "orient.h"

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "init_wait" in state set "orient" */

/* Delay function for state "init_wait" in state set "orient" */
static void D_orient_init_wait(SS_ID ssId, struct UserVar *pVar)
{
# line 362 "../orient_st.st"
	seq_delayInit(ssId, 0, (1));
}

/* Event function for state "init_wait" in state set "orient" */
static long E_orient_init_wait(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 362 "../orient_st.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init_wait" in state set "orient" */
static void A_orient_init_wait(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 360 "../orient_st.st"
			if ((pVar->A0_state) == 2)
			{
				((pVar->A0_state)) = (0);
				seq_pvPut(ssId, 122 /* A0_state */, 0);
			}
# line 360 "../orient_st.st"
			;
			if ((pVar->OMTX_state) == 2)
			{
				((pVar->OMTX_state)) = (0);
				seq_pvPut(ssId, 123 /* OMTX_state */, 0);
			}
# line 361 "../orient_st.st"
			;
		}
		return;
	}
}
/* Code for state "init" in state set "orient" */

/* Delay function for state "init" in state set "orient" */
static void D_orient_init(SS_ID ssId, struct UserVar *pVar)
{
# line 452 "../orient_st.st"
# line 467 "../orient_st.st"
# line 502 "../orient_st.st"
# line 506 "../orient_st.st"
# line 556 "../orient_st.st"
# line 560 "../orient_st.st"
# line 564 "../orient_st.st"
}

/* Event function for state "init" in state set "orient" */
static long E_orient_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 452 "../orient_st.st"
	if ((pVar->startup) != 12345)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 467 "../orient_st.st"
	if (seq_efTest(ssId, lambda_mon) || seq_efTest(ssId, energy_mon))
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
# line 502 "../orient_st.st"
	if ((pVar->A0_state) == 0)
	{
		*pNextState = 1;
		*pTransNum = 2;
		return TRUE;
	}
# line 506 "../orient_st.st"
	if ((pVar->A0_state) == 2)
	{
		*pNextState = 0;
		*pTransNum = 3;
		return TRUE;
	}
# line 556 "../orient_st.st"
	if (((pVar->A0_state) == 1) && ((pVar->OMTX_state) == 0))
	{
		*pNextState = 1;
		*pTransNum = 4;
		return TRUE;
	}
# line 560 "../orient_st.st"
	if ((pVar->OMTX_state) == 2)
	{
		*pNextState = 0;
		*pTransNum = 5;
		return TRUE;
	}
# line 564 "../orient_st.st"
	if ((pVar->A0_state) == 1 && ((pVar->OMTX_state) == 1))
	{
		*pNextState = 3;
		*pTransNum = 6;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "orient" */
static void A_orient_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 373 "../orient_st.st"
			(pVar->hc) = 12.3984244;
			(pVar->motorsConnected) = 0;
			(pVar->motorStopsConnected) = 0;
# line 378 "../orient_st.st"
pVar->orient_pmac_PM = seq_macValueGet(ssId, "PM");
pVar->orient_pmac_mTTH = seq_macValueGet(ssId, "mTTH");
pVar->orient_pmac_mTH = seq_macValueGet(ssId, "mTH");
pVar->orient_pmac_mCHI = seq_macValueGet(ssId, "mCHI");
pVar->orient_pmac_mPHI = seq_macValueGet(ssId, "mPHI");
# line 384 "../orient_st.st"
			if ((pVar->orient_pmac_PM) == 0)
			{
				{
					strcpy((pVar->Msg), ("motor prefix (PM) not specified"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					{
						((pVar->Alert)) = (1);
						seq_pvPut(ssId, 2 /* Alert */, 0);
					}
					;
					printf("orient: %s\n", (pVar->Msg));
				}
				;
			}
# line 450 "../orient_st.st"
			else
			{
strcpy(pVar->motTTH_name, pVar->orient_pmac_PM);
strcpy(pVar->motTH_name, pVar->orient_pmac_PM);
strcpy(pVar->motCHI_name, pVar->orient_pmac_PM);
strcpy(pVar->motPHI_name, pVar->orient_pmac_PM);
				if ((pVar->orient_pmac_mTTH) == 0)
				{
					{
						strcpy((pVar->Msg), ("orient: TTH motor name (mTTH) not specified"));
						seq_pvPut(ssId, 1 /* Msg */, 0);
						{
							((pVar->Alert)) = (1);
							seq_pvPut(ssId, 2 /* Alert */, 0);
						}
						;
						printf("orient: %s\n", (pVar->Msg));
					}
					;
				}
				else
				{
strcat(pVar->motTTH_name, pVar->orient_pmac_mTTH);
					seq_pvPut(ssId, 10 /* motTTH_name */, 0);
strcpy(pVar->tmpstr, pVar->motTTH_name);
if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
strcat(pVar->tmpstr, ".STOP");
					seq_pvAssign(ssId, 22 /* motTTH_stop */, (pVar->tmpstr));
strcpy(pVar->tmpstr, pVar->motTTH_name);
if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
strcat(pVar->tmpstr, ".RBV");
					seq_pvAssign(ssId, 18 /* motTTH_RBV */, (pVar->tmpstr));
				}
				if ((pVar->orient_pmac_mTH) == 0)
				{
					{
						strcpy((pVar->Msg), ("orient: TH motor name (mTH) not specified"));
						seq_pvPut(ssId, 1 /* Msg */, 0);
						{
							((pVar->Alert)) = (1);
							seq_pvPut(ssId, 2 /* Alert */, 0);
						}
						;
						printf("orient: %s\n", (pVar->Msg));
					}
					;
				}
				else
				{
strcat(pVar->motTH_name, pVar->orient_pmac_mTH);
					seq_pvPut(ssId, 11 /* motTH_name */, 0);
strcpy(pVar->tmpstr, pVar->motTH_name);
if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
strcat(pVar->tmpstr, ".STOP");
					seq_pvAssign(ssId, 23 /* motTH_stop */, (pVar->tmpstr));
strcpy(pVar->tmpstr, pVar->motTH_name);
if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
strcat(pVar->tmpstr, ".RBV");
					seq_pvAssign(ssId, 19 /* motTH_RBV */, (pVar->tmpstr));
				}
				if ((pVar->orient_pmac_mCHI) == 0)
				{
					{
						strcpy((pVar->Msg), ("orient: CHI motor name (mCHI) not specified"));
						seq_pvPut(ssId, 1 /* Msg */, 0);
						{
							((pVar->Alert)) = (1);
							seq_pvPut(ssId, 2 /* Alert */, 0);
						}
						;
						printf("orient: %s\n", (pVar->Msg));
					}
					;
				}
				else
				{
strcat(pVar->motCHI_name, pVar->orient_pmac_mCHI);
					seq_pvPut(ssId, 12 /* motCHI_name */, 0);
strcpy(pVar->tmpstr, pVar->motCHI_name);
if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
strcat(pVar->tmpstr, ".STOP");
					seq_pvAssign(ssId, 24 /* motCHI_stop */, (pVar->tmpstr));
strcpy(pVar->tmpstr, pVar->motCHI_name);
if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
strcat(pVar->tmpstr, ".RBV");
					seq_pvAssign(ssId, 20 /* motCHI_RBV */, (pVar->tmpstr));
				}
				if ((pVar->orient_pmac_mPHI) == 0)
				{
					{
						strcpy((pVar->Msg), ("orient: PHI motor name (mPHI) not specified"));
						seq_pvPut(ssId, 1 /* Msg */, 0);
						{
							((pVar->Alert)) = (1);
							seq_pvPut(ssId, 2 /* Alert */, 0);
						}
						;
						printf("orient: %s\n", (pVar->Msg));
					}
					;
				}
				else
				{
strcat(pVar->motPHI_name, pVar->orient_pmac_mPHI);
					seq_pvPut(ssId, 13 /* motPHI_name */, 0);
strcpy(pVar->tmpstr, pVar->motPHI_name);
if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
strcat(pVar->tmpstr, ".STOP");
					seq_pvAssign(ssId, 25 /* motPHI_stop */, (pVar->tmpstr));
strcpy(pVar->tmpstr, pVar->motPHI_name);
if ((pVar->cp = strrchr(pVar->tmpstr, (int)'.'))) pVar->cp[0] = '\0';
strcat(pVar->tmpstr, ".RBV");
					seq_pvAssign(ssId, 21 /* motPHI_RBV */, (pVar->tmpstr));
				}
			}
# line 451 "../orient_st.st"
			(pVar->startup) = 12345;
		}
		return;
	case 1:
		{
# line 460 "../orient_st.st"
			if (seq_efTest(ssId, lambda_mon))
			{
				(pVar->energy) = (pVar->hc) / (pVar->lambda);
				{
					((pVar->load_energy)) = ((pVar->energy));
					seq_pvPut(ssId, 93 /* load_energy */, 0);
				}
				;
			}
# line 464 "../orient_st.st"
			else
			if (seq_efTest(ssId, energy_mon))
			{
				(pVar->lambda) = (pVar->hc) / (pVar->energy);
				{
					((pVar->load_lambda)) = ((pVar->lambda));
					seq_pvPut(ssId, 90 /* load_lambda */, 0);
				}
				;
			}
# line 465 "../orient_st.st"
			seq_efClear(ssId, lambda_mon);
# line 465 "../orient_st.st"
			seq_efClear(ssId, energy_mon);
			{
				((pVar->A0_state)) = (0);
				seq_pvPut(ssId, 122 /* A0_state */, 0);
			}
# line 466 "../orient_st.st"
			;
		}
		return;
	case 2:
		{
# line 480 "../orient_st.st"
			if ((pVar->lambda) > 1.0e-9)
			{
				(pVar->energy) = (pVar->hc) / (pVar->lambda);
				{
					((pVar->load_energy)) = ((pVar->energy));
					seq_pvPut(ssId, 93 /* load_energy */, 0);
				}
				;
				if ((pVar->orientDebug))
				{
					strcpy((pVar->Msg), ("calculated energy"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					if ((pVar->orientDebug))
						printf("orient: %s\n", (pVar->Msg));
				}
				;
			}
# line 484 "../orient_st.st"
			else
			if ((pVar->energy) > 1.0e-9)
			{
				(pVar->lambda) = (pVar->hc) / (pVar->energy);
				{
					((pVar->load_lambda)) = ((pVar->lambda));
					seq_pvPut(ssId, 90 /* load_lambda */, 0);
				}
				;
			}
# line 499 "../orient_st.st"
			if (((pVar->a) != 0) && ((pVar->b) != 0) && ((pVar->c) != 0) && ((pVar->alpha) != 0) && ((pVar->beta) != 0) && ((pVar->Gamma) != 0) && ((pVar->lambda) != 0))
			{
				seq_efClear(ssId, xtalParm_mon);
				seq_efClear(ssId, lambda_mon);
				seq_efClear(ssId, energy_mon);
				if ((pVar->orientDebug))
				{
					strcpy((pVar->Msg), ("calling calc_A0"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					if ((pVar->orientDebug))
						printf("orient: %s\n", (pVar->Msg));
				}
				;
pVar->i = calc_A0(pVar->a, pVar->b, pVar->c, pVar->alpha, pVar->beta, pVar->Gamma,
 pVar->lambda, pVar->A0, pVar->A0_i);
pVar->A0_11 = pVar->A0[0][0]; pVar->A0_12 = pVar->A0[0][1]; pVar->A0_13 = pVar->A0[0][2];
pVar->A0_21 = pVar->A0[1][0]; pVar->A0_22 = pVar->A0[1][1]; pVar->A0_23 = pVar->A0[1][2];
pVar->A0_31 = pVar->A0[2][0]; pVar->A0_32 = pVar->A0[2][1]; pVar->A0_33 = pVar->A0[2][2];
				seq_pvPut(ssId, 94 /* A0_11 */, 0);
				seq_pvPut(ssId, 95 /* A0_12 */, 0);
				seq_pvPut(ssId, 96 /* A0_13 */, 0);
				seq_pvPut(ssId, 97 /* A0_21 */, 0);
				seq_pvPut(ssId, 98 /* A0_22 */, 0);
				seq_pvPut(ssId, 99 /* A0_23 */, 0);
				seq_pvPut(ssId, 100 /* A0_31 */, 0);
				seq_pvPut(ssId, 101 /* A0_32 */, 0);
				seq_pvPut(ssId, 102 /* A0_33 */, 0);
				{
					((pVar->A0_state)) = (((pVar->i) == 0 ? 1 : 2));
					seq_pvPut(ssId, 122 /* A0_state */, 0);
				}
				;
				if ((pVar->A0_state) == 1)
				{
					strcpy((pVar->Msg), ("Successful A0 calc"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					if ((pVar->orientDebug))
						printf("orient: %s\n", (pVar->Msg));
				}
				;
			}
# line 501 "../orient_st.st"
			else
			{
				{
					((pVar->A0_state)) = (2);
					seq_pvPut(ssId, 122 /* A0_state */, 0);
				}
				;
			}
		}
		return;
	case 3:
		{
		}
		return;
	case 4:
		{
# line 536 "../orient_st.st"
			if (((pVar->H1) != 0 || (pVar->K1) != 0 || (pVar->L1) != 0) && ((pVar->H2) != 0 || (pVar->K2) != 0 || (pVar->L2) != 0))
			{
				if ((pVar->orientDebug))
				{
					strcpy((pVar->Msg), ("calling calc_OMTX"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					if ((pVar->orientDebug))
						printf("orient: %s\n", (pVar->Msg));
				}
				;
pVar->v1_hkl[H_INDEX] = pVar->H1; pVar->v1_hkl[K_INDEX] = pVar->K1; pVar->v1_hkl[L_INDEX] = pVar->L1;
pVar->v1_angles[TTH_INDEX] = pVar->TTH1; pVar->v1_angles[TH_INDEX] = pVar->TH1;
pVar->v1_angles[CHI_INDEX] = pVar->CHI1; pVar->v1_angles[PHI_INDEX] = pVar->PHI1;
pVar->v2_hkl[H_INDEX] = pVar->H2; pVar->v2_hkl[K_INDEX] = pVar->K2; pVar->v2_hkl[L_INDEX] = pVar->L2;
pVar->v2_angles[TTH_INDEX] = pVar->TTH2; pVar->v2_angles[TH_INDEX] = pVar->TH2;
pVar->v2_angles[CHI_INDEX] = pVar->CHI2; pVar->v2_angles[PHI_INDEX] = pVar->PHI2;
pVar->i = calc_OMTX(pVar->v1_hkl, pVar->v1_angles, pVar->v2_hkl, pVar->v2_angles,
 pVar->A0, pVar->A0_i, pVar->OMTX, pVar->OMTX_i);
pVar->OMTX_11 = pVar->OMTX[0][0]; pVar->OMTX_12 = pVar->OMTX[0][1]; pVar->OMTX_13 = pVar->OMTX[0][2];
pVar->OMTX_21 = pVar->OMTX[1][0]; pVar->OMTX_22 = pVar->OMTX[1][1]; pVar->OMTX_23 = pVar->OMTX[1][2];
pVar->OMTX_31 = pVar->OMTX[2][0]; pVar->OMTX_32 = pVar->OMTX[2][1]; pVar->OMTX_33 = pVar->OMTX[2][2];
				seq_pvPut(ssId, 103 /* OMTX_11 */, 0);
				seq_pvPut(ssId, 104 /* OMTX_12 */, 0);
				seq_pvPut(ssId, 105 /* OMTX_13 */, 0);
				seq_pvPut(ssId, 106 /* OMTX_21 */, 0);
				seq_pvPut(ssId, 107 /* OMTX_22 */, 0);
				seq_pvPut(ssId, 108 /* OMTX_23 */, 0);
				seq_pvPut(ssId, 109 /* OMTX_31 */, 0);
				seq_pvPut(ssId, 110 /* OMTX_32 */, 0);
				seq_pvPut(ssId, 111 /* OMTX_33 */, 0);
				if ((pVar->OMTX_Calc))
				{
					((pVar->OMTX_Calc)) = (0);
					seq_pvPut(ssId, 31 /* OMTX_Calc */, 0);
				}
				;
				{
					((pVar->errAngle)) = (checkOMTX((pVar->v2_hkl), (pVar->v2_angles), (pVar->A0), (pVar->A0_i), (pVar->OMTX_i)));
					seq_pvPut(ssId, 112 /* errAngle */, 0);
				}
				;
			}
# line 538 "../orient_st.st"
			else
			{
				(pVar->i) = -1;
			}
# line 549 "../orient_st.st"
			if (((pVar->i) == 0) && (fabs((pVar->errAngle)) < (pVar->errAngleThresh)))
			{
				{
					((pVar->OMTX_state)) = (1);
					seq_pvPut(ssId, 123 /* OMTX_state */, 0);
				}
				;
				{
					strcpy((pVar->Msg), ("Successful OMTX calc"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					if ((pVar->orientDebug))
						printf("orient: %s\n", (pVar->Msg));
				}
				;
			}
# line 555 "../orient_st.st"
			else
			{
				{
					((pVar->OMTX_state)) = (2);
					seq_pvPut(ssId, 123 /* OMTX_state */, 0);
				}
				;
				if ((pVar->motPut_Auto) != 0)
				{
					{
						((pVar->motPut_Auto)) = (0);
						seq_pvPut(ssId, 6 /* motPut_Auto */, 0);
					}
					;
					{
						strcpy((pVar->Msg), ("Bad OMTX calc; motPut_Auto set to Manual"));
						seq_pvPut(ssId, 1 /* Msg */, 0);
						{
							((pVar->Alert)) = (1);
							seq_pvPut(ssId, 2 /* Alert */, 0);
						}
						;
						printf("orient: %s\n", (pVar->Msg));
					}
					;
				}
			}
		}
		return;
	case 5:
		{
		}
		return;
	case 6:
		{
		}
		return;
	}
}
/* Code for state "ref_show" in state set "orient" */

/* Delay function for state "ref_show" in state set "orient" */
static void D_orient_ref_show(SS_ID ssId, struct UserVar *pVar)
{
# line 584 "../orient_st.st"
	seq_delayInit(ssId, 0, (.5));
}

/* Event function for state "ref_show" in state set "orient" */
static long E_orient_ref_show(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 584 "../orient_st.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "ref_show" in state set "orient" */
static void A_orient_ref_show(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 582 "../orient_st.st"
			{
				((pVar->refGet1)) = (0);
				seq_pvPut(ssId, 71 /* refGet1 */, 0);
			}
# line 582 "../orient_st.st"
			;
# line 582 "../orient_st.st"
			{
				((pVar->refGet2)) = (0);
				seq_pvPut(ssId, 80 /* refGet2 */, 0);
			}
# line 582 "../orient_st.st"
			;
			{
				((pVar->refPut1)) = (0);
				seq_pvPut(ssId, 72 /* refPut1 */, 0);
			}
# line 583 "../orient_st.st"
			;
# line 583 "../orient_st.st"
			{
				((pVar->refPut2)) = (0);
				seq_pvPut(ssId, 81 /* refPut2 */, 0);
			}
# line 583 "../orient_st.st"
			;
		}
		return;
	}
}
/* Code for state "enter_monitor_all" in state set "orient" */

/* Delay function for state "enter_monitor_all" in state set "orient" */
static void D_orient_enter_monitor_all(SS_ID ssId, struct UserVar *pVar)
{
# line 595 "../orient_st.st"
	seq_delayInit(ssId, 0, (10));
}

/* Event function for state "enter_monitor_all" in state set "orient" */
static long E_orient_enter_monitor_all(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 595 "../orient_st.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "enter_monitor_all" in state set "orient" */
static void A_orient_enter_monitor_all(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 590 "../orient_st.st"
			seq_efClear(ssId, A0_mon);
# line 590 "../orient_st.st"
			seq_efClear(ssId, OMTX_mon);
			seq_efClear(ssId, H_mon);
# line 591 "../orient_st.st"
			seq_efClear(ssId, K_mon);
# line 591 "../orient_st.st"
			seq_efClear(ssId, L_mon);
			seq_efClear(ssId, TTH_mon);
# line 592 "../orient_st.st"
			seq_efClear(ssId, TH_mon);
# line 592 "../orient_st.st"
			seq_efClear(ssId, CHI_mon);
# line 592 "../orient_st.st"
			seq_efClear(ssId, PHI_mon);
			seq_efClear(ssId, Mode_mon);
			{
				strcpy((pVar->Msg), ("Initialization complete; going live"));
				seq_pvPut(ssId, 1 /* Msg */, 0);
				if ((pVar->orientDebug))
					printf("orient: %s\n", (pVar->Msg));
			}
# line 594 "../orient_st.st"
			;
		}
		return;
	}
}
/* Code for state "monitor_all_delay" in state set "orient" */

/* Delay function for state "monitor_all_delay" in state set "orient" */
static void D_orient_monitor_all_delay(SS_ID ssId, struct UserVar *pVar)
{
# line 601 "../orient_st.st"
	seq_delayInit(ssId, 0, (1));
}

/* Event function for state "monitor_all_delay" in state set "orient" */
static long E_orient_monitor_all_delay(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 601 "../orient_st.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "monitor_all_delay" in state set "orient" */
static void A_orient_monitor_all_delay(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
		}
		return;
	}
}
/* Code for state "monitor_all" in state set "orient" */

/* Delay function for state "monitor_all" in state set "orient" */
static void D_orient_monitor_all(SS_ID ssId, struct UserVar *pVar)
{
# line 613 "../orient_st.st"
# line 617 "../orient_st.st"
# line 648 "../orient_st.st"
# line 677 "../orient_st.st"
# line 692 "../orient_st.st"
# line 721 "../orient_st.st"
# line 728 "../orient_st.st"
# line 734 "../orient_st.st"
# line 740 "../orient_st.st"
# line 748 "../orient_st.st"
# line 754 "../orient_st.st"
# line 760 "../orient_st.st"
# line 766 "../orient_st.st"
# line 805 "../orient_st.st"
# line 809 "../orient_st.st"
# line 813 "../orient_st.st"
# line 819 "../orient_st.st"
# line 823 "../orient_st.st"
# line 827 "../orient_st.st"
# line 833 "../orient_st.st"
# line 837 "../orient_st.st"
# line 841 "../orient_st.st"
# line 847 "../orient_st.st"
# line 851 "../orient_st.st"
# line 855 "../orient_st.st"
# line 870 "../orient_st.st"
# line 879 "../orient_st.st"
# line 891 "../orient_st.st"
# line 898 "../orient_st.st"
# line 904 "../orient_st.st"
# line 909 "../orient_st.st"
# line 915 "../orient_st.st"
# line 920 "../orient_st.st"
# line 927 "../orient_st.st"
}

/* Event function for state "monitor_all" in state set "orient" */
static long E_orient_monitor_all(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 613 "../orient_st.st"
	if ((pVar->motorsConnected) == 0)
	{
		*pNextState = 4;
		*pTransNum = 0;
		return TRUE;
	}
# line 617 "../orient_st.st"
	if ((pVar->motorStopsConnected) == 0)
	{
		*pNextState = 4;
		*pTransNum = 1;
		return TRUE;
	}
# line 648 "../orient_st.st"
	if (((pVar->A0_state) == 1) && (pVar->OMTX_Calc))
	{
		*pNextState = 7;
		*pTransNum = 2;
		return TRUE;
	}
# line 677 "../orient_st.st"
	if (seq_efTest(ssId, xtalParm_mon) || seq_efTest(ssId, lambda_mon) || seq_efTest(ssId, energy_mon))
	{
		*pNextState = 8;
		*pTransNum = 3;
		return TRUE;
	}
# line 692 "../orient_st.st"
	if (seq_efTest(ssId, A0_mon))
	{
		*pNextState = 6;
		*pTransNum = 4;
		return TRUE;
	}
# line 721 "../orient_st.st"
	if (seq_efTest(ssId, OMTX_mon))
	{
		*pNextState = 7;
		*pTransNum = 5;
		return TRUE;
	}
# line 728 "../orient_st.st"
	if (seq_efTest(ssId, H_mon))
	{
		*pNextState = 8;
		*pTransNum = 6;
		return TRUE;
	}
# line 734 "../orient_st.st"
	if (seq_efTest(ssId, K_mon))
	{
		*pNextState = 8;
		*pTransNum = 7;
		return TRUE;
	}
# line 740 "../orient_st.st"
	if (seq_efTest(ssId, L_mon))
	{
		*pNextState = 8;
		*pTransNum = 8;
		return TRUE;
	}
# line 748 "../orient_st.st"
	if (seq_efTest(ssId, TTH_mon))
	{
		*pNextState = 9;
		*pTransNum = 9;
		return TRUE;
	}
# line 754 "../orient_st.st"
	if (seq_efTest(ssId, TH_mon))
	{
		*pNextState = 9;
		*pTransNum = 10;
		return TRUE;
	}
# line 760 "../orient_st.st"
	if (seq_efTest(ssId, CHI_mon))
	{
		*pNextState = 9;
		*pTransNum = 11;
		return TRUE;
	}
# line 766 "../orient_st.st"
	if (seq_efTest(ssId, PHI_mon))
	{
		*pNextState = 9;
		*pTransNum = 12;
		return TRUE;
	}
# line 805 "../orient_st.st"
	if (seq_efTest(ssId, motTTH_mon) && (pVar->waiting4motTTH) && seq_pvPutComplete(ssId, 14 /* motTTH */, 1, 0, 0))
	{
		*pNextState = 10;
		*pTransNum = 13;
		return TRUE;
	}
# line 809 "../orient_st.st"
	if (seq_efTest(ssId, motTTH_mon) && (pVar->weWrote_motTTH))
	{
		*pNextState = 5;
		*pTransNum = 14;
		return TRUE;
	}
# line 813 "../orient_st.st"
	if (seq_efTest(ssId, motTTH_mon) && !(pVar->weWrote_motTTH))
	{
		*pNextState = 11;
		*pTransNum = 15;
		return TRUE;
	}
# line 819 "../orient_st.st"
	if (seq_efTest(ssId, motTH_mon) && (pVar->waiting4motTH) && seq_pvPutComplete(ssId, 15 /* motTH */, 1, 0, 0))
	{
		*pNextState = 10;
		*pTransNum = 16;
		return TRUE;
	}
# line 823 "../orient_st.st"
	if (seq_efTest(ssId, motTH_mon) && (pVar->weWrote_motTH))
	{
		*pNextState = 5;
		*pTransNum = 17;
		return TRUE;
	}
# line 827 "../orient_st.st"
	if (seq_efTest(ssId, motTH_mon) && !(pVar->weWrote_motTH))
	{
		*pNextState = 11;
		*pTransNum = 18;
		return TRUE;
	}
# line 833 "../orient_st.st"
	if (seq_efTest(ssId, motCHI_mon) && (pVar->waiting4motCHI) && seq_pvPutComplete(ssId, 16 /* motCHI */, 1, 0, 0))
	{
		*pNextState = 10;
		*pTransNum = 19;
		return TRUE;
	}
# line 837 "../orient_st.st"
	if (seq_efTest(ssId, motCHI_mon) && (pVar->weWrote_motCHI))
	{
		*pNextState = 5;
		*pTransNum = 20;
		return TRUE;
	}
# line 841 "../orient_st.st"
	if (seq_efTest(ssId, motCHI_mon) && !(pVar->weWrote_motCHI))
	{
		*pNextState = 11;
		*pTransNum = 21;
		return TRUE;
	}
# line 847 "../orient_st.st"
	if (seq_efTest(ssId, motPHI_mon) && (pVar->waiting4motPHI) && seq_pvPutComplete(ssId, 17 /* motPHI */, 1, 0, 0))
	{
		*pNextState = 10;
		*pTransNum = 22;
		return TRUE;
	}
# line 851 "../orient_st.st"
	if (seq_efTest(ssId, motPHI_mon) && (pVar->weWrote_motPHI))
	{
		*pNextState = 5;
		*pTransNum = 23;
		return TRUE;
	}
# line 855 "../orient_st.st"
	if (seq_efTest(ssId, motPHI_mon) && !(pVar->weWrote_motPHI))
	{
		*pNextState = 11;
		*pTransNum = 24;
		return TRUE;
	}
# line 870 "../orient_st.st"
	if ((pVar->Busy) && (pVar->waiting4Mot) && ((pVar->waiting4motTTH) == 0) && ((pVar->waiting4motTH) == 0) && ((pVar->waiting4motCHI) == 0) && ((pVar->waiting4motPHI) == 0))
	{
		*pNextState = 5;
		*pTransNum = 25;
		return TRUE;
	}
# line 879 "../orient_st.st"
	if (seq_efTest(ssId, Mode_mon))
	{
		*pNextState = 8;
		*pTransNum = 26;
		return TRUE;
	}
# line 891 "../orient_st.st"
	if ((pVar->motPut))
	{
		*pNextState = 5;
		*pTransNum = 27;
		return TRUE;
	}
# line 898 "../orient_st.st"
	if ((pVar->motGet))
	{
		*pNextState = 11;
		*pTransNum = 28;
		return TRUE;
	}
# line 904 "../orient_st.st"
	if ((pVar->refGet1))
	{
		*pNextState = 2;
		*pTransNum = 29;
		return TRUE;
	}
# line 909 "../orient_st.st"
	if ((pVar->refGet2))
	{
		*pNextState = 2;
		*pTransNum = 30;
		return TRUE;
	}
# line 915 "../orient_st.st"
	if ((pVar->refPut1))
	{
		*pNextState = 2;
		*pTransNum = 31;
		return TRUE;
	}
# line 920 "../orient_st.st"
	if ((pVar->refPut2))
	{
		*pNextState = 2;
		*pTransNum = 32;
		return TRUE;
	}
# line 927 "../orient_st.st"
	if ((pVar->opAck))
	{
		*pNextState = 5;
		*pTransNum = 33;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "monitor_all" in state set "orient" */
static void A_orient_monitor_all(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 612 "../orient_st.st"
			{
				strcpy((pVar->Msg), ("Waiting for motor connection(s)"));
				seq_pvPut(ssId, 1 /* Msg */, 0);
				if ((pVar->orientDebug))
					printf("orient: %s\n", (pVar->Msg));
			}
# line 612 "../orient_st.st"
			;
		}
		return;
	case 1:
		{
# line 616 "../orient_st.st"
			{
				strcpy((pVar->Msg), ("Waiting for motor-stop connection(s)"));
				seq_pvPut(ssId, 1 /* Msg */, 0);
				if ((pVar->orientDebug))
					printf("orient: %s\n", (pVar->Msg));
			}
# line 616 "../orient_st.st"
			;
		}
		return;
	case 2:
		{
# line 622 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				strcpy((pVar->Msg), ("Recalculating OMTX"));
				seq_pvPut(ssId, 1 /* Msg */, 0);
				if ((pVar->orientDebug))
					printf("orient: %s\n", (pVar->Msg));
			}
# line 622 "../orient_st.st"
			;
# line 624 "../orient_st.st"
pVar->v1_hkl[H_INDEX] = pVar->H1; pVar->v1_hkl[K_INDEX] = pVar->K1; pVar->v1_hkl[L_INDEX] = pVar->L1;
pVar->v1_angles[TTH_INDEX] = pVar->TTH1; pVar->v1_angles[TH_INDEX] = pVar->TH1;
pVar->v1_angles[CHI_INDEX] = pVar->CHI1; pVar->v1_angles[PHI_INDEX] = pVar->PHI1;
pVar->v2_hkl[H_INDEX] = pVar->H2; pVar->v2_hkl[K_INDEX] = pVar->K2; pVar->v2_hkl[L_INDEX] = pVar->L2;
pVar->v2_angles[TTH_INDEX] = pVar->TTH2; pVar->v2_angles[TH_INDEX] = pVar->TH2;
pVar->v2_angles[CHI_INDEX] = pVar->CHI2; pVar->v2_angles[PHI_INDEX] = pVar->PHI2;
pVar->i = calc_OMTX(pVar->v1_hkl, pVar->v1_angles, pVar->v2_hkl, pVar->v2_angles,
 pVar->A0, pVar->A0_i, pVar->OMTX, pVar->OMTX_i);
pVar->OMTX_11 = pVar->OMTX[0][0]; pVar->OMTX_12 = pVar->OMTX[0][1]; pVar->OMTX_13 = pVar->OMTX[0][2];
pVar->OMTX_21 = pVar->OMTX[1][0]; pVar->OMTX_22 = pVar->OMTX[1][1]; pVar->OMTX_23 = pVar->OMTX[1][2];
pVar->OMTX_31 = pVar->OMTX[2][0]; pVar->OMTX_32 = pVar->OMTX[2][1]; pVar->OMTX_33 = pVar->OMTX[2][2];
# line 634 "../orient_st.st"
			seq_pvPut(ssId, 103 /* OMTX_11 */, 0);
# line 634 "../orient_st.st"
			seq_pvPut(ssId, 104 /* OMTX_12 */, 0);
# line 634 "../orient_st.st"
			seq_pvPut(ssId, 105 /* OMTX_13 */, 0);
			seq_pvPut(ssId, 106 /* OMTX_21 */, 0);
# line 635 "../orient_st.st"
			seq_pvPut(ssId, 107 /* OMTX_22 */, 0);
# line 635 "../orient_st.st"
			seq_pvPut(ssId, 108 /* OMTX_23 */, 0);
			seq_pvPut(ssId, 109 /* OMTX_31 */, 0);
# line 636 "../orient_st.st"
			seq_pvPut(ssId, 110 /* OMTX_32 */, 0);
# line 636 "../orient_st.st"
			seq_pvPut(ssId, 111 /* OMTX_33 */, 0);
			{
				((pVar->OMTX_Calc)) = (0);
				seq_pvPut(ssId, 31 /* OMTX_Calc */, 0);
			}
# line 637 "../orient_st.st"
			;
			(pVar->errAngle) = checkOMTX((pVar->v2_hkl), (pVar->v2_angles), (pVar->A0), (pVar->A0_i), (pVar->OMTX_i));
			seq_pvPut(ssId, 112 /* errAngle */, 0);
# line 643 "../orient_st.st"
			if (((pVar->i) == 0) && (fabs((pVar->errAngle)) < (pVar->errAngleThresh)))
			{
				{
					((pVar->OMTX_state)) = (1);
					seq_pvPut(ssId, 123 /* OMTX_state */, 0);
				}
				;
			}
# line 647 "../orient_st.st"
			else
			{
				{
					((pVar->OMTX_state)) = (2);
					seq_pvPut(ssId, 123 /* OMTX_state */, 0);
				}
				;
				{
					((pVar->motPut_Auto)) = (0);
					seq_pvPut(ssId, 6 /* motPut_Auto */, 0);
				}
				;
				{
					strcpy((pVar->Msg), ("Bad OMTX calc; motPut_Auto set to Manual"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					{
						((pVar->Alert)) = (1);
						seq_pvPut(ssId, 2 /* Alert */, 0);
					}
					;
					printf("orient: %s\n", (pVar->Msg));
				}
				;
			}
		}
		return;
	case 3:
		{
# line 656 "../orient_st.st"
			if (seq_efTest(ssId, lambda_mon))
			{
				(pVar->energy) = (pVar->hc) / (pVar->lambda);
				{
					((pVar->load_energy)) = ((pVar->energy));
					seq_pvPut(ssId, 93 /* load_energy */, 0);
				}
				;
			}
# line 660 "../orient_st.st"
			else
			if (seq_efTest(ssId, energy_mon))
			{
				(pVar->lambda) = (pVar->hc) / (pVar->energy);
				{
					((pVar->load_lambda)) = ((pVar->lambda));
					seq_pvPut(ssId, 90 /* load_lambda */, 0);
				}
				;
			}
# line 661 "../orient_st.st"
			seq_efClear(ssId, xtalParm_mon);
# line 661 "../orient_st.st"
			seq_efClear(ssId, lambda_mon);
# line 661 "../orient_st.st"
			seq_efClear(ssId, energy_mon);
			if ((pVar->orientDebug))
			{
				strcpy((pVar->Msg), ("new XTAL parameters"));
				seq_pvPut(ssId, 1 /* Msg */, 0);
				if ((pVar->orientDebug))
					printf("orient: %s\n", (pVar->Msg));
			}
# line 662 "../orient_st.st"
			;
# line 664 "../orient_st.st"
pVar->i = calc_A0(pVar->a, pVar->b, pVar->c, pVar->alpha, pVar->beta,
 pVar->Gamma, pVar->lambda, pVar->A0, pVar->A0_i);
pVar->A0_11 = pVar->A0[0][0]; pVar->A0_12 = pVar->A0[0][1]; pVar->A0_13 = pVar->A0[0][2];
pVar->A0_21 = pVar->A0[1][0]; pVar->A0_22 = pVar->A0[1][1]; pVar->A0_23 = pVar->A0[1][2];
pVar->A0_31 = pVar->A0[2][0]; pVar->A0_32 = pVar->A0[2][1]; pVar->A0_33 = pVar->A0[2][2];
# line 668 "../orient_st.st"
			seq_pvPut(ssId, 94 /* A0_11 */, 0);
# line 668 "../orient_st.st"
			seq_pvPut(ssId, 95 /* A0_12 */, 0);
# line 668 "../orient_st.st"
			seq_pvPut(ssId, 96 /* A0_13 */, 0);
			seq_pvPut(ssId, 97 /* A0_21 */, 0);
# line 669 "../orient_st.st"
			seq_pvPut(ssId, 98 /* A0_22 */, 0);
# line 669 "../orient_st.st"
			seq_pvPut(ssId, 99 /* A0_23 */, 0);
			seq_pvPut(ssId, 100 /* A0_31 */, 0);
# line 670 "../orient_st.st"
			seq_pvPut(ssId, 101 /* A0_32 */, 0);
# line 670 "../orient_st.st"
			seq_pvPut(ssId, 102 /* A0_33 */, 0);
			{
				((pVar->A0_state)) = (((pVar->i) == 0 ? 1 : 2));
				seq_pvPut(ssId, 122 /* A0_state */, 0);
			}
# line 671 "../orient_st.st"
			;
# line 674 "../orient_st.st"
			if ((pVar->A0_state) == 1)
			{
				{
					strcpy((pVar->Msg), ("Successful A0 calc"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					if ((pVar->orientDebug))
						printf("orient: %s\n", (pVar->Msg));
				}
				;
			}
# line 676 "../orient_st.st"
			else
			{
				{
					strcpy((pVar->Msg), ("A0 calc failed"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					{
						((pVar->Alert)) = (1);
						seq_pvPut(ssId, 2 /* Alert */, 0);
					}
					;
					printf("orient: %s\n", (pVar->Msg));
				}
				;
			}
		}
		return;
	case 4:
		{
# line 681 "../orient_st.st"
			seq_efClear(ssId, A0_mon);
			if ((pVar->orientDebug))
			{
				strcpy((pVar->Msg), ("new A0 precursor(s)"));
				seq_pvPut(ssId, 1 /* Msg */, 0);
				if ((pVar->orientDebug))
					printf("orient: %s\n", (pVar->Msg));
			}
# line 682 "../orient_st.st"
			;
# line 685 "../orient_st.st"
pVar->A0[0][0] = pVar->A0_11; pVar->A0[0][1] = pVar->A0_12; pVar->A0[0][2] = pVar->A0_13;
pVar->A0[1][0] = pVar->A0_21; pVar->A0[1][1] = pVar->A0_22; pVar->A0[1][2] = pVar->A0_23;
pVar->A0[2][0] = pVar->A0_31; pVar->A0[2][1] = pVar->A0_32; pVar->A0[2][2] = pVar->A0_33;
# line 687 "../orient_st.st"
			(pVar->i) = invertArray((pVar->A0), (pVar->A0_i));
# line 691 "../orient_st.st"
			if ((pVar->i) == -1)
			{
				{
					strcpy((pVar->Msg), ("Could not invert A0 array"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					if ((pVar->orientDebug))
						printf("orient: %s\n", (pVar->Msg));
				}
				;
				{
					((pVar->A0_state)) = (2);
					seq_pvPut(ssId, 122 /* A0_state */, 0);
				}
				;
			}
		}
		return;
	case 5:
		{
# line 696 "../orient_st.st"
			seq_efClear(ssId, OMTX_mon);
			if ((pVar->orientDebug))
			{
				strcpy((pVar->Msg), ("new OMTX element(s)"));
				seq_pvPut(ssId, 1 /* Msg */, 0);
				if ((pVar->orientDebug))
					printf("orient: %s\n", (pVar->Msg));
			}
# line 697 "../orient_st.st"
			;
# line 700 "../orient_st.st"
pVar->OMTX[0][0] = pVar->OMTX_11; pVar->OMTX[0][1] = pVar->OMTX_12; pVar->OMTX[0][2] = pVar->OMTX_13;
pVar->OMTX[1][0] = pVar->OMTX_21; pVar->OMTX[1][1] = pVar->OMTX_22; pVar->OMTX[1][2] = pVar->OMTX_23;
pVar->OMTX[2][0] = pVar->OMTX_31; pVar->OMTX[2][1] = pVar->OMTX_32; pVar->OMTX[2][2] = pVar->OMTX_33;
# line 702 "../orient_st.st"
			(pVar->i) = invertArray((pVar->OMTX), (pVar->OMTX_i));
# line 706 "../orient_st.st"
			if ((pVar->i) == -1)
			{
				{
					strcpy((pVar->Msg), ("Could not invert OMTX array"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					if ((pVar->orientDebug))
						printf("orient: %s\n", (pVar->Msg));
				}
				;
				{
					((pVar->OMTX_state)) = (2);
					seq_pvPut(ssId, 123 /* OMTX_state */, 0);
				}
				;
			}
# line 709 "../orient_st.st"
			else
			if ((pVar->A0_state) != 1)
			{
				{
					strcpy((pVar->Msg), ("Can't check or use OMTX array (No A0)"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					if ((pVar->orientDebug))
						printf("orient: %s\n", (pVar->Msg));
				}
				;
				{
					((pVar->OMTX_state)) = (2);
					seq_pvPut(ssId, 123 /* OMTX_state */, 0);
				}
				;
			}
# line 720 "../orient_st.st"
			else
			{
				(pVar->errAngle) = checkOMTX((pVar->v2_hkl), (pVar->v2_angles), (pVar->A0), (pVar->A0_i), (pVar->OMTX_i));
				seq_pvPut(ssId, 112 /* errAngle */, 0);
				if (fabs((pVar->errAngle)) < (pVar->errAngleThresh))
				{
					{
						((pVar->OMTX_state)) = (1);
						seq_pvPut(ssId, 123 /* OMTX_state */, 0);
					}
					;
					if ((pVar->orientDebug))
					{
						strcpy((pVar->Msg), ("User's OMTX passes test"));
						seq_pvPut(ssId, 1 /* Msg */, 0);
						if ((pVar->orientDebug))
							printf("orient: %s\n", (pVar->Msg));
					}
					;
				}
				else
				{
					{
						((pVar->OMTX_state)) = (2);
						seq_pvPut(ssId, 123 /* OMTX_state */, 0);
					}
					;
					if ((pVar->orientDebug))
					{
						strcpy((pVar->Msg), ("User's OMTX fails test"));
						seq_pvPut(ssId, 1 /* Msg */, 0);
						if ((pVar->orientDebug))
							printf("orient: %s\n", (pVar->Msg));
					}
					;
				}
			}
		}
		return;
	case 6:
		{
# line 725 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("\norient: client wrote H\n");
			}
# line 726 "../orient_st.st"
			if (!(pVar->Busy))
			{
				((pVar->Busy)) = (1);
				seq_pvPut(ssId, 32 /* Busy */, 0);
			}
# line 726 "../orient_st.st"
			;
			seq_efClear(ssId, H_mon);
		}
		return;
	case 7:
		{
# line 731 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("\norient: client wrote K\n");
			}
# line 732 "../orient_st.st"
			if (!(pVar->Busy))
			{
				((pVar->Busy)) = (1);
				seq_pvPut(ssId, 32 /* Busy */, 0);
			}
# line 732 "../orient_st.st"
			;
			seq_efClear(ssId, K_mon);
		}
		return;
	case 8:
		{
# line 737 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("\norient: client wrote L\n");
			}
# line 738 "../orient_st.st"
			if (!(pVar->Busy))
			{
				((pVar->Busy)) = (1);
				seq_pvPut(ssId, 32 /* Busy */, 0);
			}
# line 738 "../orient_st.st"
			;
			seq_efClear(ssId, L_mon);
		}
		return;
	case 9:
		{
# line 745 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("\norient: client wrote TTH\n");
			}
# line 746 "../orient_st.st"
			if (!(pVar->Busy))
			{
				((pVar->Busy)) = (1);
				seq_pvPut(ssId, 32 /* Busy */, 0);
			}
# line 746 "../orient_st.st"
			;
			seq_efClear(ssId, TTH_mon);
		}
		return;
	case 10:
		{
# line 751 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("\norient: client wrote TH\n");
			}
# line 752 "../orient_st.st"
			if (!(pVar->Busy))
			{
				((pVar->Busy)) = (1);
				seq_pvPut(ssId, 32 /* Busy */, 0);
			}
# line 752 "../orient_st.st"
			;
			seq_efClear(ssId, TH_mon);
		}
		return;
	case 11:
		{
# line 757 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("\norient: client wrote CHI\n");
			}
# line 758 "../orient_st.st"
			if (!(pVar->Busy))
			{
				((pVar->Busy)) = (1);
				seq_pvPut(ssId, 32 /* Busy */, 0);
			}
# line 758 "../orient_st.st"
			;
			seq_efClear(ssId, CHI_mon);
		}
		return;
	case 12:
		{
# line 763 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("\norient: client wrote PHI\n");
			}
# line 764 "../orient_st.st"
			if (!(pVar->Busy))
			{
				((pVar->Busy)) = (1);
				seq_pvPut(ssId, 32 /* Busy */, 0);
			}
# line 764 "../orient_st.st"
			;
			seq_efClear(ssId, PHI_mon);
		}
		return;
	case 13:
		{
# line 796 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("orient: motTTH done moving\n");
			}
# line 797 "../orient_st.st"
			seq_efClear(ssId, motTTH_mon);
# line 797 "../orient_st.st"
			{
				((pVar->waiting4motTTH)) = (0);
				seq_pvPut(ssId, 118 /* waiting4motTTH */, 0);
			}
# line 797 "../orient_st.st"
			;
# line 804 "../orient_st.st"
			if ((pVar->weWrote_motTTH))
			{
				((pVar->weWrote_motTTH)) = (0);
				seq_pvPut(ssId, 124 /* weWrote_motTTH */, 0);
			}
# line 804 "../orient_st.st"
			;
		}
		return;
	case 14:
		{
# line 807 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("orient: We wrote motTTH\n");
			}
# line 808 "../orient_st.st"
			seq_efClear(ssId, motTTH_mon);
# line 808 "../orient_st.st"
			{
				((pVar->weWrote_motTTH)) = (0);
				seq_pvPut(ssId, 124 /* weWrote_motTTH */, 0);
			}
# line 808 "../orient_st.st"
			;
		}
		return;
	case 15:
		{
# line 811 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("orient: client wrote motTTH\n");
			}
# line 812 "../orient_st.st"
			seq_efClear(ssId, motTTH_mon);
		}
		return;
	case 16:
		{
# line 816 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("orient: motTH done moving\n");
			}
# line 817 "../orient_st.st"
			seq_efClear(ssId, motTH_mon);
# line 817 "../orient_st.st"
			{
				((pVar->waiting4motTH)) = (0);
				seq_pvPut(ssId, 119 /* waiting4motTH */, 0);
			}
# line 817 "../orient_st.st"
			;
			if ((pVar->weWrote_motTH))
			{
				((pVar->weWrote_motTH)) = (0);
				seq_pvPut(ssId, 125 /* weWrote_motTH */, 0);
			}
# line 818 "../orient_st.st"
			;
		}
		return;
	case 17:
		{
# line 821 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("orient: We wrote motTH\n");
			}
# line 822 "../orient_st.st"
			seq_efClear(ssId, motTH_mon);
# line 822 "../orient_st.st"
			{
				((pVar->weWrote_motTH)) = (0);
				seq_pvPut(ssId, 125 /* weWrote_motTH */, 0);
			}
# line 822 "../orient_st.st"
			;
		}
		return;
	case 18:
		{
# line 825 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("orient: client wrote motTH\n");
			}
# line 826 "../orient_st.st"
			seq_efClear(ssId, motTH_mon);
		}
		return;
	case 19:
		{
# line 830 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("orient: motCHI done moving\n");
			}
# line 831 "../orient_st.st"
			seq_efClear(ssId, motCHI_mon);
# line 831 "../orient_st.st"
			{
				((pVar->waiting4motCHI)) = (0);
				seq_pvPut(ssId, 120 /* waiting4motCHI */, 0);
			}
# line 831 "../orient_st.st"
			;
			if ((pVar->weWrote_motCHI))
			{
				((pVar->weWrote_motCHI)) = (0);
				seq_pvPut(ssId, 126 /* weWrote_motCHI */, 0);
			}
# line 832 "../orient_st.st"
			;
		}
		return;
	case 20:
		{
# line 835 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("orient: We wrote motCHI\n");
			}
# line 836 "../orient_st.st"
			seq_efClear(ssId, motCHI_mon);
# line 836 "../orient_st.st"
			{
				((pVar->weWrote_motCHI)) = (0);
				seq_pvPut(ssId, 126 /* weWrote_motCHI */, 0);
			}
# line 836 "../orient_st.st"
			;
		}
		return;
	case 21:
		{
# line 839 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("orient: client wrote motCHI\n");
			}
# line 840 "../orient_st.st"
			seq_efClear(ssId, motCHI_mon);
		}
		return;
	case 22:
		{
# line 844 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("orient: motPHI done moving\n");
			}
# line 845 "../orient_st.st"
			seq_efClear(ssId, motPHI_mon);
# line 845 "../orient_st.st"
			{
				((pVar->waiting4motPHI)) = (0);
				seq_pvPut(ssId, 121 /* waiting4motPHI */, 0);
			}
# line 845 "../orient_st.st"
			;
			if ((pVar->weWrote_motPHI))
			{
				((pVar->weWrote_motPHI)) = (0);
				seq_pvPut(ssId, 127 /* weWrote_motPHI */, 0);
			}
# line 846 "../orient_st.st"
			;
		}
		return;
	case 23:
		{
# line 849 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("orient: We wrote motPHI\n");
			}
# line 850 "../orient_st.st"
			seq_efClear(ssId, motPHI_mon);
# line 850 "../orient_st.st"
			{
				((pVar->weWrote_motPHI)) = (0);
				seq_pvPut(ssId, 127 /* weWrote_motPHI */, 0);
			}
# line 850 "../orient_st.st"
			;
		}
		return;
	case 24:
		{
# line 853 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("orient: client wrote motPHI\n");
			}
# line 854 "../orient_st.st"
			seq_efClear(ssId, motPHI_mon);
		}
		return;
	case 25:
		{
# line 860 "../orient_st.st"
			{
				((pVar->waiting4Mot)) = (0);
				seq_pvPut(ssId, 117 /* waiting4Mot */, 0);
			}
# line 860 "../orient_st.st"
			;
			{
				((pVar->Busy)) = (0);
				seq_pvPut(ssId, 32 /* Busy */, 0);
			}
# line 861 "../orient_st.st"
			;
			if ((pVar->H_busy))
			{
				((pVar->H_busy)) = (0);
				seq_pvPut(ssId, 35 /* H_busy */, 0);
			}
# line 862 "../orient_st.st"
			;
			if ((pVar->K_busy))
			{
				((pVar->K_busy)) = (0);
				seq_pvPut(ssId, 40 /* K_busy */, 0);
			}
# line 863 "../orient_st.st"
			;
			if ((pVar->L_busy))
			{
				((pVar->L_busy)) = (0);
				seq_pvPut(ssId, 45 /* L_busy */, 0);
			}
# line 864 "../orient_st.st"
			;
			if ((pVar->TTH_busy))
			{
				((pVar->TTH_busy)) = (0);
				seq_pvPut(ssId, 50 /* TTH_busy */, 0);
			}
# line 865 "../orient_st.st"
			;
			if ((pVar->TH_busy))
			{
				((pVar->TH_busy)) = (0);
				seq_pvPut(ssId, 54 /* TH_busy */, 0);
			}
# line 866 "../orient_st.st"
			;
			if ((pVar->CHI_busy))
			{
				((pVar->CHI_busy)) = (0);
				seq_pvPut(ssId, 58 /* CHI_busy */, 0);
			}
# line 867 "../orient_st.st"
			;
			if ((pVar->PHI_busy))
			{
				((pVar->PHI_busy)) = (0);
				seq_pvPut(ssId, 62 /* PHI_busy */, 0);
			}
# line 868 "../orient_st.st"
			;
			if ((pVar->orientDebug))
			{
				printf("motors done\n");
			}
		}
		return;
	case 26:
		{
# line 874 "../orient_st.st"
			seq_efClear(ssId, Mode_mon);
# line 878 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				{
					strcpy((pVar->Msg), ("new HKL-to-Angles Mode"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					if ((pVar->orientDebug))
						printf("orient: %s\n", (pVar->Msg));
				}
				;
				printf("...Mode=%d\n", (pVar->Mode));
			}
		}
		return;
	case 27:
		{
# line 883 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("orient: motPut==%d\n", (pVar->motPut));
			}
# line 884 "../orient_st.st"
			{
				((pVar->weWrote_motTTH)) = (1);
				seq_pvPut(ssId, 124 /* weWrote_motTTH */, 0);
			}
# line 884 "../orient_st.st"
			;
# line 884 "../orient_st.st"
			(pVar->motTTH) = (pVar->TTH);
# line 884 "../orient_st.st"
			{
				((pVar->waiting4motTTH)) = (1);
				seq_pvPut(ssId, 118 /* waiting4motTTH */, 0);
			}
# line 884 "../orient_st.st"
			;
# line 884 "../orient_st.st"
			seq_pvPut(ssId, 14 /* motTTH */, 1);
			{
				((pVar->weWrote_motTH)) = (1);
				seq_pvPut(ssId, 125 /* weWrote_motTH */, 0);
			}
# line 885 "../orient_st.st"
			;
# line 885 "../orient_st.st"
			(pVar->motTH) = (pVar->TH);
# line 885 "../orient_st.st"
			{
				((pVar->waiting4motTH)) = (1);
				seq_pvPut(ssId, 119 /* waiting4motTH */, 0);
			}
# line 885 "../orient_st.st"
			;
# line 885 "../orient_st.st"
			seq_pvPut(ssId, 15 /* motTH */, 1);
			{
				((pVar->weWrote_motCHI)) = (1);
				seq_pvPut(ssId, 126 /* weWrote_motCHI */, 0);
			}
# line 886 "../orient_st.st"
			;
# line 886 "../orient_st.st"
			(pVar->motCHI) = (pVar->CHI);
# line 886 "../orient_st.st"
			{
				((pVar->waiting4motCHI)) = (1);
				seq_pvPut(ssId, 120 /* waiting4motCHI */, 0);
			}
# line 886 "../orient_st.st"
			;
# line 886 "../orient_st.st"
			seq_pvPut(ssId, 16 /* motCHI */, 1);
			{
				((pVar->weWrote_motPHI)) = (1);
				seq_pvPut(ssId, 127 /* weWrote_motPHI */, 0);
			}
# line 887 "../orient_st.st"
			;
# line 887 "../orient_st.st"
			(pVar->motPHI) = (pVar->PHI);
# line 887 "../orient_st.st"
			{
				((pVar->waiting4motPHI)) = (1);
				seq_pvPut(ssId, 121 /* waiting4motPHI */, 0);
			}
# line 887 "../orient_st.st"
			;
# line 887 "../orient_st.st"
			seq_pvPut(ssId, 17 /* motPHI */, 1);
# line 889 "../orient_st.st"
			(pVar->motPut) = 0;
# line 889 "../orient_st.st"
			{
				((pVar->load_motPut)) = ((pVar->motPut));
				seq_pvPut(ssId, 7 /* load_motPut */, 0);
			}
# line 889 "../orient_st.st"
			;
			{
				((pVar->waiting4Mot)) = (1);
				seq_pvPut(ssId, 117 /* waiting4Mot */, 0);
			}
# line 890 "../orient_st.st"
			;
		}
		return;
	case 28:
		{
		}
		return;
	case 29:
		{
# line 902 "../orient_st.st"
			{
				((pVar->H1)) = ((pVar->H));
				seq_pvPut(ssId, 64 /* H1 */, 0);
			}
# line 902 "../orient_st.st"
			;
# line 902 "../orient_st.st"
			{
				((pVar->K1)) = ((pVar->K));
				seq_pvPut(ssId, 65 /* K1 */, 0);
			}
# line 902 "../orient_st.st"
			;
# line 902 "../orient_st.st"
			{
				((pVar->L1)) = ((pVar->L));
				seq_pvPut(ssId, 66 /* L1 */, 0);
			}
# line 902 "../orient_st.st"
			;
			{
				((pVar->TTH1)) = ((pVar->TTH));
				seq_pvPut(ssId, 67 /* TTH1 */, 0);
			}
# line 903 "../orient_st.st"
			;
# line 903 "../orient_st.st"
			{
				((pVar->TH1)) = ((pVar->TH));
				seq_pvPut(ssId, 68 /* TH1 */, 0);
			}
# line 903 "../orient_st.st"
			;
# line 903 "../orient_st.st"
			{
				((pVar->CHI1)) = ((pVar->CHI));
				seq_pvPut(ssId, 69 /* CHI1 */, 0);
			}
# line 903 "../orient_st.st"
			;
# line 903 "../orient_st.st"
			{
				((pVar->PHI1)) = ((pVar->PHI));
				seq_pvPut(ssId, 70 /* PHI1 */, 0);
			}
# line 903 "../orient_st.st"
			;
		}
		return;
	case 30:
		{
# line 907 "../orient_st.st"
			{
				((pVar->H2)) = ((pVar->H));
				seq_pvPut(ssId, 73 /* H2 */, 0);
			}
# line 907 "../orient_st.st"
			;
# line 907 "../orient_st.st"
			{
				((pVar->K2)) = ((pVar->K));
				seq_pvPut(ssId, 74 /* K2 */, 0);
			}
# line 907 "../orient_st.st"
			;
# line 907 "../orient_st.st"
			{
				((pVar->L2)) = ((pVar->L));
				seq_pvPut(ssId, 75 /* L2 */, 0);
			}
# line 907 "../orient_st.st"
			;
			{
				((pVar->TTH2)) = ((pVar->TTH));
				seq_pvPut(ssId, 76 /* TTH2 */, 0);
			}
# line 908 "../orient_st.st"
			;
# line 908 "../orient_st.st"
			{
				((pVar->TH2)) = ((pVar->TH));
				seq_pvPut(ssId, 77 /* TH2 */, 0);
			}
# line 908 "../orient_st.st"
			;
# line 908 "../orient_st.st"
			{
				((pVar->CHI2)) = ((pVar->CHI));
				seq_pvPut(ssId, 78 /* CHI2 */, 0);
			}
# line 908 "../orient_st.st"
			;
# line 908 "../orient_st.st"
			{
				((pVar->PHI2)) = ((pVar->PHI));
				seq_pvPut(ssId, 79 /* PHI2 */, 0);
			}
# line 908 "../orient_st.st"
			;
		}
		return;
	case 31:
		{
# line 913 "../orient_st.st"
			{
				((pVar->H)) = ((pVar->H1));
				seq_pvPut(ssId, 33 /* H */, 0);
			}
# line 913 "../orient_st.st"
			;
# line 913 "../orient_st.st"
			{
				((pVar->K)) = ((pVar->K1));
				seq_pvPut(ssId, 38 /* K */, 0);
			}
# line 913 "../orient_st.st"
			;
# line 913 "../orient_st.st"
			{
				((pVar->L)) = ((pVar->L1));
				seq_pvPut(ssId, 43 /* L */, 0);
			}
# line 913 "../orient_st.st"
			;
			{
				((pVar->TTH)) = ((pVar->TTH1));
				seq_pvPut(ssId, 48 /* TTH */, 0);
			}
# line 914 "../orient_st.st"
			;
# line 914 "../orient_st.st"
			{
				((pVar->TH)) = ((pVar->TH1));
				seq_pvPut(ssId, 52 /* TH */, 0);
			}
# line 914 "../orient_st.st"
			;
# line 914 "../orient_st.st"
			{
				((pVar->CHI)) = ((pVar->CHI1));
				seq_pvPut(ssId, 56 /* CHI */, 0);
			}
# line 914 "../orient_st.st"
			;
# line 914 "../orient_st.st"
			{
				((pVar->PHI)) = ((pVar->PHI1));
				seq_pvPut(ssId, 60 /* PHI */, 0);
			}
# line 914 "../orient_st.st"
			;
		}
		return;
	case 32:
		{
# line 918 "../orient_st.st"
			{
				((pVar->H)) = ((pVar->H2));
				seq_pvPut(ssId, 33 /* H */, 0);
			}
# line 918 "../orient_st.st"
			;
# line 918 "../orient_st.st"
			{
				((pVar->K)) = ((pVar->K2));
				seq_pvPut(ssId, 38 /* K */, 0);
			}
# line 918 "../orient_st.st"
			;
# line 918 "../orient_st.st"
			{
				((pVar->L)) = ((pVar->L2));
				seq_pvPut(ssId, 43 /* L */, 0);
			}
# line 918 "../orient_st.st"
			;
			{
				((pVar->TTH)) = ((pVar->TTH2));
				seq_pvPut(ssId, 48 /* TTH */, 0);
			}
# line 919 "../orient_st.st"
			;
# line 919 "../orient_st.st"
			{
				((pVar->TH)) = ((pVar->TH2));
				seq_pvPut(ssId, 52 /* TH */, 0);
			}
# line 919 "../orient_st.st"
			;
# line 919 "../orient_st.st"
			{
				((pVar->CHI)) = ((pVar->CHI2));
				seq_pvPut(ssId, 56 /* CHI */, 0);
			}
# line 919 "../orient_st.st"
			;
# line 919 "../orient_st.st"
			{
				((pVar->PHI)) = ((pVar->PHI2));
				seq_pvPut(ssId, 60 /* PHI */, 0);
			}
# line 919 "../orient_st.st"
			;
		}
		return;
	case 33:
		{
# line 924 "../orient_st.st"
			{
				strcpy((pVar->Msg), (""));
				seq_pvPut(ssId, 1 /* Msg */, 0);
				if ((pVar->orientDebug))
					printf("orient: %s\n", (pVar->Msg));
			}
# line 924 "../orient_st.st"
			;
			{
				((pVar->opAck)) = (0);
				seq_pvPut(ssId, 3 /* opAck */, 0);
			}
# line 925 "../orient_st.st"
			;
			{
				((pVar->Alert)) = (0);
				seq_pvPut(ssId, 2 /* Alert */, 0);
			}
# line 926 "../orient_st.st"
			;
		}
		return;
	}
}
/* Code for state "newA0" in state set "orient" */

/* Delay function for state "newA0" in state set "orient" */
static void D_orient_newA0(SS_ID ssId, struct UserVar *pVar)
{
# line 939 "../orient_st.st"
# line 941 "../orient_st.st"
}

/* Event function for state "newA0" in state set "orient" */
static long E_orient_newA0(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 939 "../orient_st.st"
	if ((pVar->A0_state) != 1)
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
# line 941 "../orient_st.st"
	if ((pVar->A0_state) == 1)
	{
		*pNextState = 8;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "newA0" in state set "orient" */
static void A_orient_newA0(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
		}
		return;
	case 1:
		{
		}
		return;
	}
}
/* Code for state "newOMTX" in state set "orient" */

/* Delay function for state "newOMTX" in state set "orient" */
static void D_orient_newOMTX(SS_ID ssId, struct UserVar *pVar)
{
# line 950 "../orient_st.st"
# line 954 "../orient_st.st"
}

/* Event function for state "newOMTX" in state set "orient" */
static long E_orient_newOMTX(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 950 "../orient_st.st"
	if ((pVar->OMTX_state) == 1)
	{
		*pNextState = 8;
		*pTransNum = 0;
		return TRUE;
	}
# line 954 "../orient_st.st"
	if ((pVar->OMTX_state) != 1)
	{
		*pNextState = 5;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "newOMTX" in state set "orient" */
static void A_orient_newOMTX(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
		}
		return;
	case 1:
		{
# line 952 "../orient_st.st"
			{
				((pVar->motPut_Auto)) = (0);
				seq_pvPut(ssId, 6 /* motPut_Auto */, 0);
			}
# line 952 "../orient_st.st"
			;
			{
				strcpy((pVar->Msg), ("Bad OMTX calc; motPut_Auto set to Manual"));
				seq_pvPut(ssId, 1 /* Msg */, 0);
				{
					((pVar->Alert)) = (1);
					seq_pvPut(ssId, 2 /* Alert */, 0);
				}
				;
				printf("orient: %s\n", (pVar->Msg));
			}
# line 953 "../orient_st.st"
			;
		}
		return;
	}
}
/* Code for state "newHKL" in state set "orient" */

/* Delay function for state "newHKL" in state set "orient" */
static void D_orient_newHKL(SS_ID ssId, struct UserVar *pVar)
{
# line 971 "../orient_st.st"
# line 991 "../orient_st.st"
	seq_delayInit(ssId, 0, (0.02));
}

/* Event function for state "newHKL" in state set "orient" */
static long E_orient_newHKL(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 971 "../orient_st.st"
	if (((pVar->A0_state) != 1) || ((pVar->OMTX_state) != 1))
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
# line 991 "../orient_st.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 12;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "newHKL" in state set "orient" */
static void A_orient_newHKL(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 968 "../orient_st.st"
			if ((pVar->A0_state) != 1)
			{
				{
					strcpy((pVar->Msg), ("We don't have a good A0 matrix"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					{
						((pVar->Alert)) = (1);
						seq_pvPut(ssId, 2 /* Alert */, 0);
					}
					;
					printf("orient: %s\n", (pVar->Msg));
				}
				;
			}
# line 970 "../orient_st.st"
			else
			{
				{
					strcpy((pVar->Msg), ("We don't have a good OMTX matrix"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					{
						((pVar->Alert)) = (1);
						seq_pvPut(ssId, 2 /* Alert */, 0);
					}
					;
					printf("orient: %s\n", (pVar->Msg));
				}
				;
			}
		}
		return;
	case 1:
		{
# line 973 "../orient_st.st"
			if (seq_efTest(ssId, H_mon))
			{
				seq_efClear(ssId, H_mon);
				if ((pVar->orientDebug))
					printf("orient:newHKL: client also wrote H\n");
			}
# line 974 "../orient_st.st"
			if (seq_efTest(ssId, K_mon))
			{
				seq_efClear(ssId, K_mon);
				if ((pVar->orientDebug))
					printf("orient:newHKL: client also wrote K\n");
			}
# line 975 "../orient_st.st"
			if (seq_efTest(ssId, L_mon))
			{
				seq_efClear(ssId, L_mon);
				if ((pVar->orientDebug))
					printf("orient:newHKL: client also wrote L\n");
			}
# line 978 "../orient_st.st"
pVar->hkl[H_INDEX] = pVar->H; pVar->hkl[K_INDEX] = pVar->K; pVar->hkl[L_INDEX] = pVar->L;
pVar->angles[TTH_INDEX] = pVar->TTH; pVar->angles[TH_INDEX] = pVar->TH;
pVar->angles[CHI_INDEX] = pVar->CHI; pVar->angles[PHI_INDEX] = pVar->PHI;
pVar->i = HKL_to_angles(pVar->hkl, pVar->A0, pVar->OMTX, pVar->angles, (int)pVar->Mode);
# line 988 "../orient_st.st"
			if ((pVar->i) == 0)
			{
pVar->TTH = pVar->angles[TTH_INDEX];
pVar->TH = pVar->angles[TH_INDEX];
pVar->CHI = pVar->angles[CHI_INDEX];
pVar->PHI = pVar->angles[PHI_INDEX];
				{
					((pVar->newAngles)) = (1);
					seq_pvPut(ssId, 115 /* newAngles */, 0);
				}
				;
				if ((pVar->motPut_Auto))
				{
					((pVar->newMotors)) = (1);
					seq_pvPut(ssId, 116 /* newMotors */, 0);
				}
				;
			}
# line 990 "../orient_st.st"
			else
			if ((pVar->i))
			{
				{
					strcpy((pVar->Msg), ("Bad HKL2angles calc."));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					{
						((pVar->Alert)) = (1);
						seq_pvPut(ssId, 2 /* Alert */, 0);
					}
					;
					printf("orient: %s\n", (pVar->Msg));
				}
				;
			}
		}
		return;
	}
}
/* Code for state "newAngles" in state set "orient" */

/* Delay function for state "newAngles" in state set "orient" */
static void D_orient_newAngles(SS_ID ssId, struct UserVar *pVar)
{
# line 1006 "../orient_st.st"
# line 1021 "../orient_st.st"
	seq_delayInit(ssId, 0, (0.02));
}

/* Event function for state "newAngles" in state set "orient" */
static long E_orient_newAngles(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1006 "../orient_st.st"
	if (((pVar->A0_state) != 1) || ((pVar->OMTX_state) != 1))
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
# line 1021 "../orient_st.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 12;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "newAngles" in state set "orient" */
static void A_orient_newAngles(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1003 "../orient_st.st"
			if ((pVar->A0_state) != 1)
			{
				{
					strcpy((pVar->Msg), ("We don't have a good A0 matrix"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					{
						((pVar->Alert)) = (1);
						seq_pvPut(ssId, 2 /* Alert */, 0);
					}
					;
					printf("orient: %s\n", (pVar->Msg));
				}
				;
			}
# line 1005 "../orient_st.st"
			else
			{
				{
					strcpy((pVar->Msg), ("We don't have a good OMTX matrix"));
					seq_pvPut(ssId, 1 /* Msg */, 0);
					{
						((pVar->Alert)) = (1);
						seq_pvPut(ssId, 2 /* Alert */, 0);
					}
					;
					printf("orient: %s\n", (pVar->Msg));
				}
				;
			}
		}
		return;
	case 1:
		{
# line 1008 "../orient_st.st"
			if (seq_efTest(ssId, TTH_mon))
			{
				seq_efClear(ssId, TTH_mon);
				if ((pVar->orientDebug))
					printf("orient:newAngles: client also wrote TTH\n");
			}
# line 1009 "../orient_st.st"
			if (seq_efTest(ssId, TH_mon))
			{
				seq_efClear(ssId, TH_mon);
				if ((pVar->orientDebug))
					printf("orient:newAngles: client also wrote TH\n");
			}
# line 1010 "../orient_st.st"
			if (seq_efTest(ssId, CHI_mon))
			{
				seq_efClear(ssId, CHI_mon);
				if ((pVar->orientDebug))
					printf("orient:newAngles: client also wrote CHI\n");
			}
# line 1011 "../orient_st.st"
			if (seq_efTest(ssId, PHI_mon))
			{
				seq_efClear(ssId, PHI_mon);
				if ((pVar->orientDebug))
					printf("orient:newAngles: client also wrote PHI\n");
			}
# line 1014 "../orient_st.st"
pVar->angles[TTH_INDEX] = pVar->TTH; pVar->angles[TH_INDEX] = pVar->TH;
pVar->angles[CHI_INDEX] = pVar->CHI; pVar->angles[PHI_INDEX] = pVar->PHI;
angles_to_HKL(pVar->angles, pVar->OMTX_i, pVar->A0_i, pVar->hkl);
pVar->H = pVar->hkl[H_INDEX]; pVar->K = pVar->hkl[K_INDEX]; pVar->L = pVar->hkl[L_INDEX];
# line 1017 "../orient_st.st"
			{
				((pVar->newHKL)) = (1);
				seq_pvPut(ssId, 114 /* newHKL */, 0);
			}
# line 1017 "../orient_st.st"
			;
# line 1020 "../orient_st.st"
			if ((pVar->motPut_Auto))
			{
				{
					((pVar->newMotors)) = (1);
					seq_pvPut(ssId, 116 /* newMotors */, 0);
				}
				;
			}
		}
		return;
	}
}
/* Code for state "checkDone" in state set "orient" */

/* Delay function for state "checkDone" in state set "orient" */
static void D_orient_checkDone(SS_ID ssId, struct UserVar *pVar)
{
# line 1038 "../orient_st.st"
# line 1040 "../orient_st.st"
}

/* Event function for state "checkDone" in state set "orient" */
static long E_orient_checkDone(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1038 "../orient_st.st"
	if ((pVar->Busy) && (pVar->waiting4Mot) && ((pVar->waiting4motTTH) == 0) && ((pVar->waiting4motTH) == 0) && ((pVar->waiting4motCHI) == 0) && ((pVar->waiting4motPHI) == 0))
	{
		*pNextState = 11;
		*pTransNum = 0;
		return TRUE;
	}
# line 1040 "../orient_st.st"
	if (TRUE)
	{
		*pNextState = 5;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "checkDone" in state set "orient" */
static void A_orient_checkDone(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1028 "../orient_st.st"
			{
				((pVar->waiting4Mot)) = (0);
				seq_pvPut(ssId, 117 /* waiting4Mot */, 0);
			}
# line 1028 "../orient_st.st"
			;
			{
				((pVar->Busy)) = (0);
				seq_pvPut(ssId, 32 /* Busy */, 0);
			}
# line 1029 "../orient_st.st"
			;
			if ((pVar->H_busy))
			{
				((pVar->H_busy)) = (0);
				seq_pvPut(ssId, 35 /* H_busy */, 0);
			}
# line 1030 "../orient_st.st"
			;
			if ((pVar->K_busy))
			{
				((pVar->K_busy)) = (0);
				seq_pvPut(ssId, 40 /* K_busy */, 0);
			}
# line 1031 "../orient_st.st"
			;
			if ((pVar->L_busy))
			{
				((pVar->L_busy)) = (0);
				seq_pvPut(ssId, 45 /* L_busy */, 0);
			}
# line 1032 "../orient_st.st"
			;
			if ((pVar->TTH_busy))
			{
				((pVar->TTH_busy)) = (0);
				seq_pvPut(ssId, 50 /* TTH_busy */, 0);
			}
# line 1033 "../orient_st.st"
			;
			if ((pVar->TH_busy))
			{
				((pVar->TH_busy)) = (0);
				seq_pvPut(ssId, 54 /* TH_busy */, 0);
			}
# line 1034 "../orient_st.st"
			;
			if ((pVar->CHI_busy))
			{
				((pVar->CHI_busy)) = (0);
				seq_pvPut(ssId, 58 /* CHI_busy */, 0);
			}
# line 1035 "../orient_st.st"
			;
			if ((pVar->PHI_busy))
			{
				((pVar->PHI_busy)) = (0);
				seq_pvPut(ssId, 62 /* PHI_busy */, 0);
			}
# line 1036 "../orient_st.st"
			;
			if ((pVar->orientDebug))
			{
				printf("motors done\n");
			}
		}
		return;
	case 1:
		{
		}
		return;
	}
}
/* Code for state "newMotors" in state set "orient" */

/* Delay function for state "newMotors" in state set "orient" */
static void D_orient_newMotors(SS_ID ssId, struct UserVar *pVar)
{
# line 1060 "../orient_st.st"
}

/* Event function for state "newMotors" in state set "orient" */
static long E_orient_newMotors(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1060 "../orient_st.st"
	if (TRUE)
	{
		*pNextState = 12;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "newMotors" in state set "orient" */
static void A_orient_newMotors(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1046 "../orient_st.st"
			if ((pVar->orientDebug))
			{
				printf("orient:newMotors: motGet=%d\n", (pVar->motGet));
			}
# line 1059 "../orient_st.st"
			if ((pVar->motGet) || ((pVar->motGet_Auto) && !(pVar->waiting4Mot)))
			{
				if ((pVar->orientDebug))
				{
					printf("orient:newMotors: getting values\n");
				}
				(pVar->TTH) = (pVar->motTTH);
				(pVar->TH) = (pVar->motTH);
				(pVar->CHI) = (pVar->motCHI);
				(pVar->PHI) = (pVar->motPHI);
				{
					((pVar->newAngles)) = (1);
					seq_pvPut(ssId, 115 /* newAngles */, 0);
				}
				;
pVar->angles[TTH_INDEX] = pVar->TTH; pVar->angles[TH_INDEX] = pVar->TH;
pVar->angles[CHI_INDEX] = pVar->CHI; pVar->angles[PHI_INDEX] = pVar->PHI;
angles_to_HKL(pVar->angles, pVar->OMTX_i, pVar->A0_i, pVar->hkl);
pVar->H = pVar->hkl[H_INDEX]; pVar->K = pVar->hkl[K_INDEX]; pVar->L = pVar->hkl[L_INDEX];
				{
					((pVar->newHKL)) = (1);
					seq_pvPut(ssId, 114 /* newHKL */, 0);
				}
				;
				if ((pVar->motGet))
				{
					((pVar->motGet)) = (0);
					seq_pvPut(ssId, 8 /* motGet */, 0);
				}
				;
			}
		}
		return;
	}
}
/* Code for state "putAll" in state set "orient" */

/* Delay function for state "putAll" in state set "orient" */
static void D_orient_putAll(SS_ID ssId, struct UserVar *pVar)
{
# line 1137 "../orient_st.st"
}

/* Event function for state "putAll" in state set "orient" */
static long E_orient_putAll(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1137 "../orient_st.st"
	if (TRUE)
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "putAll" in state set "orient" */
static void A_orient_putAll(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1081 "../orient_st.st"
			if ((pVar->newHKL))
			{
				if ((pVar->orientDebug))
				{
					printf("orient:putAll: writing HKL\n");
				}
				{
					((pVar->loadH)) = ((pVar->H));
					seq_pvPut(ssId, 36 /* loadH */, 0);
				}
				;
				{
					((pVar->loadK)) = ((pVar->K));
					seq_pvPut(ssId, 41 /* loadK */, 0);
				}
				;
				{
					((pVar->loadL)) = ((pVar->L));
					seq_pvPut(ssId, 46 /* loadL */, 0);
				}
				;
				{
					((pVar->newHKL)) = (0);
					seq_pvPut(ssId, 114 /* newHKL */, 0);
				}
				;
			}
# line 1090 "../orient_st.st"
			if ((pVar->newAngles))
			{
				if ((pVar->orientDebug))
				{
					printf("orient:putAll: writing Angles\n");
				}
				{
					((pVar->loadTTH)) = ((pVar->TTH));
					seq_pvPut(ssId, 51 /* loadTTH */, 0);
				}
				;
				{
					((pVar->loadTH)) = ((pVar->TH));
					seq_pvPut(ssId, 55 /* loadTH */, 0);
				}
				;
				{
					((pVar->loadCHI)) = ((pVar->CHI));
					seq_pvPut(ssId, 59 /* loadCHI */, 0);
				}
				;
				{
					((pVar->loadPHI)) = ((pVar->PHI));
					seq_pvPut(ssId, 63 /* loadPHI */, 0);
				}
				;
				{
					((pVar->newAngles)) = (0);
					seq_pvPut(ssId, 115 /* newAngles */, 0);
				}
				;
			}
# line 1124 "../orient_st.st"
			if ((pVar->newMotors))
			{
				if ((pVar->orientDebug))
				{
					printf("orient:putAll: comparing new/old Motor values\n");
				}
				if ((pVar->motTTH_Connected) && (pVar->motTH_Connected) && (pVar->motCHI_Connected) && (pVar->motPHI_Connected))
				{
					if ((pVar->orientDebug))
						printf("putAll:newMotors: H=%8.4f, K=%8.4f, L=%8.4f\n", (pVar->H), (pVar->K), (pVar->L));
					if ((fabs(((pVar->motTTH)) - ((pVar->TTH))) > 1.0e-9))
					{
						if ((pVar->orientDebug))
							printf("putAll:newMotors: TTH=%8.4f, motTTH=%8.4f\n", (pVar->TTH), (pVar->motTTH));
						{
							((pVar->weWrote_motTTH)) = (1);
							seq_pvPut(ssId, 124 /* weWrote_motTTH */, 0);
						}
						;
						{
							((pVar->waiting4motTTH)) = (1);
							seq_pvPut(ssId, 118 /* waiting4motTTH */, 0);
						}
						;
						(pVar->motTTH) = (pVar->TTH);
						seq_pvPut(ssId, 14 /* motTTH */, 1);
					}
					if ((fabs(((pVar->motTH)) - ((pVar->TH))) > 1.0e-9))
					{
						if ((pVar->orientDebug))
							printf("putAll:newMotors:  TH=%8.4f,  motTH=%8.4f\n", (pVar->TH), (pVar->motTH));
						{
							((pVar->weWrote_motTH)) = (1);
							seq_pvPut(ssId, 125 /* weWrote_motTH */, 0);
						}
						;
						{
							((pVar->waiting4motTH)) = (1);
							seq_pvPut(ssId, 119 /* waiting4motTH */, 0);
						}
						;
						(pVar->motTH) = (pVar->TH);
						seq_pvPut(ssId, 15 /* motTH */, 1);
					}
					if ((fabs(((pVar->motCHI)) - ((pVar->CHI))) > 1.0e-9))
					{
						if ((pVar->orientDebug))
							printf("putAll:newMotors: CHI=%8.4f, motCHI=%8.4f\n", (pVar->CHI), (pVar->motCHI));
						{
							((pVar->weWrote_motCHI)) = (1);
							seq_pvPut(ssId, 126 /* weWrote_motCHI */, 0);
						}
						;
						{
							((pVar->waiting4motCHI)) = (1);
							seq_pvPut(ssId, 120 /* waiting4motCHI */, 0);
						}
						;
						(pVar->motCHI) = (pVar->CHI);
						seq_pvPut(ssId, 16 /* motCHI */, 1);
					}
					if ((fabs(((pVar->motPHI)) - ((pVar->PHI))) > 1.0e-9))
					{
						if ((pVar->orientDebug))
							printf("putAll:newMotors: PHI=%8.4f, motPHI=%8.4f\n", (pVar->PHI), (pVar->motPHI));
						{
							((pVar->weWrote_motPHI)) = (1);
							seq_pvPut(ssId, 127 /* weWrote_motPHI */, 0);
						}
						;
						{
							((pVar->waiting4motPHI)) = (1);
							seq_pvPut(ssId, 121 /* waiting4motPHI */, 0);
						}
						;
						(pVar->motPHI) = (pVar->PHI);
						seq_pvPut(ssId, 17 /* motPHI */, 1);
					}
					if ((pVar->waiting4motTTH) || (pVar->waiting4motTH) || (pVar->waiting4motCHI) || (pVar->waiting4motPHI))
					{
						{
							((pVar->waiting4Mot)) = (1);
							seq_pvPut(ssId, 117 /* waiting4Mot */, 0);
						}
						;
						if (!(pVar->Busy))
						{
							((pVar->Busy)) = (1);
							seq_pvPut(ssId, 32 /* Busy */, 0);
						}
						;
					}
					{
						((pVar->newMotors)) = (0);
						seq_pvPut(ssId, 116 /* newMotors */, 0);
					}
					;
				}
				else
				{
					{
						strcpy((pVar->Msg), ("Motors not connected"));
						seq_pvPut(ssId, 1 /* Msg */, 0);
						{
							((pVar->Alert)) = (1);
							seq_pvPut(ssId, 2 /* Alert */, 0);
						}
						;
						printf("orient: %s\n", (pVar->Msg));
					}
					;
					{
						((pVar->newMotors)) = (0);
						seq_pvPut(ssId, 116 /* newMotors */, 0);
					}
					;
				}
			}
# line 1136 "../orient_st.st"
			if (!(pVar->waiting4Mot))
			{
				if ((pVar->Busy))
				{
					((pVar->Busy)) = (0);
					seq_pvPut(ssId, 32 /* Busy */, 0);
				}
				;
				if ((pVar->H_busy))
				{
					((pVar->H_busy)) = (0);
					seq_pvPut(ssId, 35 /* H_busy */, 0);
				}
				;
				if ((pVar->K_busy))
				{
					((pVar->K_busy)) = (0);
					seq_pvPut(ssId, 40 /* K_busy */, 0);
				}
				;
				if ((pVar->L_busy))
				{
					((pVar->L_busy)) = (0);
					seq_pvPut(ssId, 45 /* L_busy */, 0);
				}
				;
				if ((pVar->TTH_busy))
				{
					((pVar->TTH_busy)) = (0);
					seq_pvPut(ssId, 50 /* TTH_busy */, 0);
				}
				;
				if ((pVar->TH_busy))
				{
					((pVar->TH_busy)) = (0);
					seq_pvPut(ssId, 54 /* TH_busy */, 0);
				}
				;
				if ((pVar->CHI_busy))
				{
					((pVar->CHI_busy)) = (0);
					seq_pvPut(ssId, 58 /* CHI_busy */, 0);
				}
				;
				if ((pVar->PHI_busy))
				{
					((pVar->PHI_busy)) = (0);
					seq_pvPut(ssId, 62 /* PHI_busy */, 0);
				}
				;
				if ((pVar->orientDebug))
				{
					printf("orient:putAll: all complete (no motors)\n");
				}
			}
		}
		return;
	}
}
/* Code for state "check" in state set "connections" */

/* Delay function for state "check" in state set "connections" */
static void D_connections_check(SS_ID ssId, struct UserVar *pVar)
{
# line 1170 "../orient_st.st"
	seq_delayInit(ssId, 0, (1));
}

/* Event function for state "check" in state set "connections" */
static long E_connections_check(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1170 "../orient_st.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 0;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "check" in state set "connections" */
static void A_connections_check(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1147 "../orient_st.st"
			(pVar->old_connect_state) = (pVar->motTTH_Connected);
			(pVar->motTTH_Connected) = seq_pvAssigned(ssId, 14 /* motTTH */) ? seq_pvConnected(ssId, 14 /* motTTH */) : 0;
			if ((pVar->old_connect_state) != (pVar->motTTH_Connected))
				seq_pvPut(ssId, 27 /* motTTH_Connected */, 0);
			(pVar->old_connect_state) = (pVar->motTH_Connected);
			(pVar->motTH_Connected) = seq_pvAssigned(ssId, 15 /* motTH */) ? seq_pvConnected(ssId, 15 /* motTH */) : 0;
			if ((pVar->old_connect_state) != (pVar->motTH_Connected))
				seq_pvPut(ssId, 28 /* motTH_Connected */, 0);
			(pVar->old_connect_state) = (pVar->motCHI_Connected);
			(pVar->motCHI_Connected) = seq_pvAssigned(ssId, 16 /* motCHI */) ? seq_pvConnected(ssId, 16 /* motCHI */) : 0;
			if ((pVar->old_connect_state) != (pVar->motCHI_Connected))
				seq_pvPut(ssId, 29 /* motCHI_Connected */, 0);
			(pVar->old_connect_state) = (pVar->motPHI_Connected);
			(pVar->motPHI_Connected) = seq_pvAssigned(ssId, 17 /* motPHI */) ? seq_pvConnected(ssId, 17 /* motPHI */) : 0;
			if ((pVar->old_connect_state) != (pVar->motPHI_Connected))
				seq_pvPut(ssId, 30 /* motPHI_Connected */, 0);
# line 1164 "../orient_st.st"
			(pVar->motorsConnected) = (pVar->motTTH_Connected) && (pVar->motTH_Connected) && (pVar->motCHI_Connected) && (pVar->motPHI_Connected);
# line 1169 "../orient_st.st"
			(pVar->motorStopsConnected) = seq_pvAssigned(ssId, 22 /* motTTH_stop */) && seq_pvConnected(ssId, 22 /* motTTH_stop */) && seq_pvAssigned(ssId, 23 /* motTH_stop */) && seq_pvConnected(ssId, 23 /* motTH_stop */) && seq_pvAssigned(ssId, 24 /* motCHI_stop */) && seq_pvConnected(ssId, 24 /* motCHI_stop */) && seq_pvAssigned(ssId, 25 /* motPHI_stop */) && seq_pvConnected(ssId, 25 /* motPHI_stop */);
		}
		return;
	}
}
/* Code for state "rbv_monitor_throttle" in state set "readback" */

/* Delay function for state "rbv_monitor_throttle" in state set "readback" */
static void D_readback_rbv_monitor_throttle(SS_ID ssId, struct UserVar *pVar)
{
# line 1178 "../orient_st.st"
	seq_delayInit(ssId, 0, (.1));
}

/* Event function for state "rbv_monitor_throttle" in state set "readback" */
static long E_readback_rbv_monitor_throttle(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1178 "../orient_st.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "rbv_monitor_throttle" in state set "readback" */
static void A_readback_rbv_monitor_throttle(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
		}
		return;
	}
}
/* Code for state "rbv_monitor_wait" in state set "readback" */

/* Delay function for state "rbv_monitor_wait" in state set "readback" */
static void D_readback_rbv_monitor_wait(SS_ID ssId, struct UserVar *pVar)
{
# line 1183 "../orient_st.st"
# line 1186 "../orient_st.st"
	seq_delayInit(ssId, 0, (5));
}

/* Event function for state "rbv_monitor_wait" in state set "readback" */
static long E_readback_rbv_monitor_wait(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1183 "../orient_st.st"
	if (((pVar->A0_state) == 1) && ((pVar->OMTX_state) == 1))
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 1186 "../orient_st.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 2;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "rbv_monitor_wait" in state set "readback" */
static void A_readback_rbv_monitor_wait(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
		}
		return;
	case 1:
		{
		}
		return;
	}
}
/* Code for state "rbv_monitor" in state set "readback" */

/* Delay function for state "rbv_monitor" in state set "readback" */
static void D_readback_rbv_monitor(SS_ID ssId, struct UserVar *pVar)
{
# line 1194 "../orient_st.st"
# line 1204 "../orient_st.st"
}

/* Event function for state "rbv_monitor" in state set "readback" */
static long E_readback_rbv_monitor(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1194 "../orient_st.st"
	if (((pVar->A0_state) != 1) || ((pVar->OMTX_state) != 1))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 1204 "../orient_st.st"
	if (seq_efTest(ssId, motRBV_mon))
	{
		*pNextState = 0;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "rbv_monitor" in state set "readback" */
static void A_readback_rbv_monitor(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1191 "../orient_st.st"
			if ((pVar->H_RBV) != 0)
			{
				((pVar->H_RBV)) = (0);
				seq_pvPut(ssId, 37 /* H_RBV */, 0);
			}
# line 1191 "../orient_st.st"
			;
			if ((pVar->K_RBV) != 0)
			{
				((pVar->K_RBV)) = (0);
				seq_pvPut(ssId, 42 /* K_RBV */, 0);
			}
# line 1192 "../orient_st.st"
			;
			if ((pVar->L_RBV) != 0)
			{
				((pVar->L_RBV)) = (0);
				seq_pvPut(ssId, 47 /* L_RBV */, 0);
			}
# line 1193 "../orient_st.st"
			;
		}
		return;
	case 1:
		{
# line 1197 "../orient_st.st"
			seq_efClear(ssId, motRBV_mon);
# line 1200 "../orient_st.st"
pVar->rbv_angles[TTH_INDEX] = pVar->motTTH_RBV; pVar->rbv_angles[TH_INDEX] = pVar->motTH_RBV;
pVar->rbv_angles[CHI_INDEX] = pVar->motCHI_RBV; pVar->rbv_angles[PHI_INDEX] = pVar->motPHI_RBV;
angles_to_HKL(pVar->rbv_angles, pVar->OMTX_i, pVar->A0_i, pVar->rbv_hkl);
pVar->H_RBV = pVar->rbv_hkl[H_INDEX]; pVar->K_RBV = pVar->rbv_hkl[K_INDEX]; pVar->L_RBV = pVar->rbv_hkl[L_INDEX];
# line 1203 "../orient_st.st"
			seq_pvPut(ssId, 37 /* H_RBV */, 0);
# line 1203 "../orient_st.st"
			seq_pvPut(ssId, 42 /* K_RBV */, 0);
# line 1203 "../orient_st.st"
			seq_pvPut(ssId, 47 /* L_RBV */, 0);
		}
		return;
	}
}
/* Code for state "stop_all" in state set "stop" */

/* Delay function for state "stop_all" in state set "stop" */
static void D_stop_stop_all(SS_ID ssId, struct UserVar *pVar)
{
# line 1218 "../orient_st.st"
}

/* Event function for state "stop_all" in state set "stop" */
static long E_stop_stop_all(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1218 "../orient_st.st"
	if ((pVar->stopMotors))
	{
		*pNextState = 0;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "stop_all" in state set "stop" */
static void A_stop_stop_all(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1212 "../orient_st.st"
			{
				strcpy((pVar->Msg), ("stopping motors"));
				seq_pvPut(ssId, 1 /* Msg */, 0);
				if ((pVar->orientDebug))
					printf("orient: %s\n", (pVar->Msg));
			}
# line 1212 "../orient_st.st"
			;
			if (seq_pvAssigned(ssId, 22 /* motTTH_stop */) && seq_pvConnected(ssId, 22 /* motTTH_stop */))
			{
				((pVar->motTTH_stop)) = (1);
				seq_pvPut(ssId, 22 /* motTTH_stop */, 0);
			}
# line 1213 "../orient_st.st"
			;
			if (seq_pvAssigned(ssId, 23 /* motTH_stop */) && seq_pvConnected(ssId, 23 /* motTH_stop */))
			{
				((pVar->motTH_stop)) = (1);
				seq_pvPut(ssId, 23 /* motTH_stop */, 0);
			}
# line 1214 "../orient_st.st"
			;
			if (seq_pvAssigned(ssId, 24 /* motCHI_stop */) && seq_pvConnected(ssId, 24 /* motCHI_stop */))
			{
				((pVar->motCHI_stop)) = (1);
				seq_pvPut(ssId, 24 /* motCHI_stop */, 0);
			}
# line 1215 "../orient_st.st"
			;
			if (seq_pvAssigned(ssId, 25 /* motPHI_stop */) && seq_pvConnected(ssId, 25 /* motPHI_stop */))
			{
				((pVar->motPHI_stop)) = (1);
				seq_pvPut(ssId, 25 /* motPHI_stop */, 0);
			}
# line 1216 "../orient_st.st"
			;
			{
				((pVar->stopMotors)) = (0);
				seq_pvPut(ssId, 26 /* stopMotors */, 0);
			}
# line 1217 "../orient_st.st"
			;
		}
		return;
	}
}

/* Exit handler */
static void exit_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/************************ Tables ***********************/

/* Database Blocks */
static struct seqChan seqChan[NUM_CHANNELS] = {
  {"{P}Debug", (void *)OFFSET(struct UserVar, orientDebug), "orientDebug", 
    "short", 1, 21, 0, 1, 0, 0, 0},

  {"{P}Msg", (void *)OFFSET(struct UserVar, Msg[0]), "Msg", 
    "string", 1, 22, 0, 0, 0, 0, 0},

  {"{P}Alert", (void *)OFFSET(struct UserVar, Alert), "Alert", 
    "short", 1, 23, 0, 0, 0, 0, 0},

  {"{P}OperAck", (void *)OFFSET(struct UserVar, opAck), "opAck", 
    "short", 1, 24, 0, 1, 0, 0, 0},

  {"{P}Mode", (void *)OFFSET(struct UserVar, Mode), "Mode", 
    "short", 1, 25, 1, 1, 0, 0, 0},

  {"{P}motPut", (void *)OFFSET(struct UserVar, motPut), "motPut", 
    "short", 1, 26, 2, 1, 0, 0, 0},

  {"{P}motPut_Auto", (void *)OFFSET(struct UserVar, motPut_Auto), "motPut_Auto", 
    "short", 1, 27, 0, 1, 0, 0, 0},

  {"{P}load_motPut", (void *)OFFSET(struct UserVar, load_motPut), "load_motPut", 
    "short", 1, 28, 0, 0, 0, 0, 0},

  {"{P}motGet", (void *)OFFSET(struct UserVar, motGet), "motGet", 
    "short", 1, 29, 3, 1, 0, 0, 0},

  {"{P}motGet_Auto", (void *)OFFSET(struct UserVar, motGet_Auto), "motGet_Auto", 
    "short", 1, 30, 0, 1, 0, 0, 0},

  {"{P}motTTH_name", (void *)OFFSET(struct UserVar, motTTH_name[0]), "motTTH_name", 
    "string", 1, 31, 0, 1, 0, 0, 0},

  {"{P}motTH_name", (void *)OFFSET(struct UserVar, motTH_name[0]), "motTH_name", 
    "string", 1, 32, 0, 1, 0, 0, 0},

  {"{P}motCHI_name", (void *)OFFSET(struct UserVar, motCHI_name[0]), "motCHI_name", 
    "string", 1, 33, 0, 1, 0, 0, 0},

  {"{P}motPHI_name", (void *)OFFSET(struct UserVar, motPHI_name[0]), "motPHI_name", 
    "string", 1, 34, 0, 1, 0, 0, 0},

  {"{PM}{mTTH}", (void *)OFFSET(struct UserVar, motTTH), "motTTH", 
    "double", 1, 35, 4, 1, 0, 0, 0},

  {"{PM}{mTH}", (void *)OFFSET(struct UserVar, motTH), "motTH", 
    "double", 1, 36, 5, 1, 0, 0, 0},

  {"{PM}{mCHI}", (void *)OFFSET(struct UserVar, motCHI), "motCHI", 
    "double", 1, 37, 6, 1, 0, 0, 0},

  {"{PM}{mPHI}", (void *)OFFSET(struct UserVar, motPHI), "motPHI", 
    "double", 1, 38, 7, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, motTTH_RBV), "motTTH_RBV", 
    "double", 1, 39, 8, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, motTH_RBV), "motTH_RBV", 
    "double", 1, 40, 8, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, motCHI_RBV), "motCHI_RBV", 
    "double", 1, 41, 8, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, motPHI_RBV), "motPHI_RBV", 
    "double", 1, 42, 8, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, motTTH_stop), "motTTH_stop", 
    "int", 1, 43, 0, 0, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, motTH_stop), "motTH_stop", 
    "int", 1, 44, 0, 0, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, motCHI_stop), "motCHI_stop", 
    "int", 1, 45, 0, 0, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, motPHI_stop), "motPHI_stop", 
    "int", 1, 46, 0, 0, 0, 0, 0},

  {"{P}stopMotors", (void *)OFFSET(struct UserVar, stopMotors), "stopMotors", 
    "int", 1, 47, 0, 1, 0, 0, 0},

  {"{P}motTTH_Connected", (void *)OFFSET(struct UserVar, motTTH_Connected), "motTTH_Connected", 
    "int", 1, 48, 0, 0, 0, 0, 0},

  {"{P}motTH_Connected", (void *)OFFSET(struct UserVar, motTH_Connected), "motTH_Connected", 
    "int", 1, 49, 0, 0, 0, 0, 0},

  {"{P}motCHI_Connected", (void *)OFFSET(struct UserVar, motCHI_Connected), "motCHI_Connected", 
    "int", 1, 50, 0, 0, 0, 0, 0},

  {"{P}motPHI_Connected", (void *)OFFSET(struct UserVar, motPHI_Connected), "motPHI_Connected", 
    "int", 1, 51, 0, 0, 0, 0, 0},

  {"{P}OMTX_Calc", (void *)OFFSET(struct UserVar, OMTX_Calc), "OMTX_Calc", 
    "short", 1, 52, 0, 1, 0, 0, 0},

  {"{P}Busy", (void *)OFFSET(struct UserVar, Busy), "Busy", 
    "short", 1, 53, 0, 1, 0, 0, 0},

  {"{P}H", (void *)OFFSET(struct UserVar, H), "H", 
    "double", 1, 54, 0, 1, 0, 0, 0},

  {"{P}H_event", (void *)OFFSET(struct UserVar, H_event), "H_event", 
    "double", 1, 55, 9, 1, 0, 0, 0},

  {"{P}H_busy", (void *)OFFSET(struct UserVar, H_busy), "H_busy", 
    "short", 1, 56, 0, 1, 0, 0, 0},

  {"{P}loadH", (void *)OFFSET(struct UserVar, loadH), "loadH", 
    "double", 1, 57, 0, 0, 0, 0, 0},

  {"{P}H_RBV", (void *)OFFSET(struct UserVar, H_RBV), "H_RBV", 
    "double", 1, 58, 0, 0, 0, 0, 0},

  {"{P}K", (void *)OFFSET(struct UserVar, K), "K", 
    "double", 1, 59, 0, 1, 0, 0, 0},

  {"{P}K_event", (void *)OFFSET(struct UserVar, K_event), "K_event", 
    "double", 1, 60, 10, 1, 0, 0, 0},

  {"{P}K_busy", (void *)OFFSET(struct UserVar, K_busy), "K_busy", 
    "short", 1, 61, 0, 1, 0, 0, 0},

  {"{P}loadK", (void *)OFFSET(struct UserVar, loadK), "loadK", 
    "double", 1, 62, 0, 0, 0, 0, 0},

  {"{P}K_RBV", (void *)OFFSET(struct UserVar, K_RBV), "K_RBV", 
    "double", 1, 63, 0, 0, 0, 0, 0},

  {"{P}L", (void *)OFFSET(struct UserVar, L), "L", 
    "double", 1, 64, 0, 1, 0, 0, 0},

  {"{P}L_event", (void *)OFFSET(struct UserVar, L_event), "L_event", 
    "double", 1, 65, 11, 1, 0, 0, 0},

  {"{P}L_busy", (void *)OFFSET(struct UserVar, L_busy), "L_busy", 
    "short", 1, 66, 0, 1, 0, 0, 0},

  {"{P}loadL", (void *)OFFSET(struct UserVar, loadL), "loadL", 
    "double", 1, 67, 0, 0, 0, 0, 0},

  {"{P}L_RBV", (void *)OFFSET(struct UserVar, L_RBV), "L_RBV", 
    "double", 1, 68, 0, 0, 0, 0, 0},

  {"{P}TTH", (void *)OFFSET(struct UserVar, TTH), "TTH", 
    "double", 1, 69, 0, 1, 0, 0, 0},

  {"{P}TTH_event", (void *)OFFSET(struct UserVar, TTH_event), "TTH_event", 
    "double", 1, 70, 12, 1, 0, 0, 0},

  {"{P}TTH_busy", (void *)OFFSET(struct UserVar, TTH_busy), "TTH_busy", 
    "short", 1, 71, 0, 1, 0, 0, 0},

  {"{P}loadTTH", (void *)OFFSET(struct UserVar, loadTTH), "loadTTH", 
    "double", 1, 72, 0, 0, 0, 0, 0},

  {"{P}TH", (void *)OFFSET(struct UserVar, TH), "TH", 
    "double", 1, 73, 0, 1, 0, 0, 0},

  {"{P}TH_event", (void *)OFFSET(struct UserVar, TH_event), "TH_event", 
    "double", 1, 74, 13, 1, 0, 0, 0},

  {"{P}TH_busy", (void *)OFFSET(struct UserVar, TH_busy), "TH_busy", 
    "short", 1, 75, 0, 1, 0, 0, 0},

  {"{P}loadTH", (void *)OFFSET(struct UserVar, loadTH), "loadTH", 
    "double", 1, 76, 0, 0, 0, 0, 0},

  {"{P}CHI", (void *)OFFSET(struct UserVar, CHI), "CHI", 
    "double", 1, 77, 0, 1, 0, 0, 0},

  {"{P}CHI_event", (void *)OFFSET(struct UserVar, CHI_event), "CHI_event", 
    "double", 1, 78, 14, 1, 0, 0, 0},

  {"{P}CHI_busy", (void *)OFFSET(struct UserVar, CHI_busy), "CHI_busy", 
    "short", 1, 79, 0, 1, 0, 0, 0},

  {"{P}loadCHI", (void *)OFFSET(struct UserVar, loadCHI), "loadCHI", 
    "double", 1, 80, 0, 0, 0, 0, 0},

  {"{P}PHI", (void *)OFFSET(struct UserVar, PHI), "PHI", 
    "double", 1, 81, 0, 1, 0, 0, 0},

  {"{P}PHI_event", (void *)OFFSET(struct UserVar, PHI_event), "PHI_event", 
    "double", 1, 82, 15, 1, 0, 0, 0},

  {"{P}PHI_busy", (void *)OFFSET(struct UserVar, PHI_busy), "PHI_busy", 
    "short", 1, 83, 0, 1, 0, 0, 0},

  {"{P}loadPHI", (void *)OFFSET(struct UserVar, loadPHI), "loadPHI", 
    "double", 1, 84, 0, 0, 0, 0, 0},

  {"{P}H1", (void *)OFFSET(struct UserVar, H1), "H1", 
    "double", 1, 85, 0, 1, 0, 0, 0},

  {"{P}K1", (void *)OFFSET(struct UserVar, K1), "K1", 
    "double", 1, 86, 0, 1, 0, 0, 0},

  {"{P}L1", (void *)OFFSET(struct UserVar, L1), "L1", 
    "double", 1, 87, 0, 1, 0, 0, 0},

  {"{P}TTH1", (void *)OFFSET(struct UserVar, TTH1), "TTH1", 
    "double", 1, 88, 0, 1, 0, 0, 0},

  {"{P}TH1", (void *)OFFSET(struct UserVar, TH1), "TH1", 
    "double", 1, 89, 0, 1, 0, 0, 0},

  {"{P}CHI1", (void *)OFFSET(struct UserVar, CHI1), "CHI1", 
    "double", 1, 90, 0, 1, 0, 0, 0},

  {"{P}PHI1", (void *)OFFSET(struct UserVar, PHI1), "PHI1", 
    "double", 1, 91, 0, 1, 0, 0, 0},

  {"{P}refGet1", (void *)OFFSET(struct UserVar, refGet1), "refGet1", 
    "short", 1, 92, 0, 1, 0, 0, 0},

  {"{P}refPut1", (void *)OFFSET(struct UserVar, refPut1), "refPut1", 
    "short", 1, 93, 0, 1, 0, 0, 0},

  {"{P}H2", (void *)OFFSET(struct UserVar, H2), "H2", 
    "double", 1, 94, 0, 1, 0, 0, 0},

  {"{P}K2", (void *)OFFSET(struct UserVar, K2), "K2", 
    "double", 1, 95, 0, 1, 0, 0, 0},

  {"{P}L2", (void *)OFFSET(struct UserVar, L2), "L2", 
    "double", 1, 96, 0, 1, 0, 0, 0},

  {"{P}TTH2", (void *)OFFSET(struct UserVar, TTH2), "TTH2", 
    "double", 1, 97, 0, 1, 0, 0, 0},

  {"{P}TH2", (void *)OFFSET(struct UserVar, TH2), "TH2", 
    "double", 1, 98, 0, 1, 0, 0, 0},

  {"{P}CHI2", (void *)OFFSET(struct UserVar, CHI2), "CHI2", 
    "double", 1, 99, 0, 1, 0, 0, 0},

  {"{P}PHI2", (void *)OFFSET(struct UserVar, PHI2), "PHI2", 
    "double", 1, 100, 0, 1, 0, 0, 0},

  {"{P}refGet2", (void *)OFFSET(struct UserVar, refGet2), "refGet2", 
    "short", 1, 101, 0, 1, 0, 0, 0},

  {"{P}refPut2", (void *)OFFSET(struct UserVar, refPut2), "refPut2", 
    "short", 1, 102, 0, 1, 0, 0, 0},

  {"{P}a", (void *)OFFSET(struct UserVar, a), "a", 
    "double", 1, 103, 16, 1, 0, 0, 0},

  {"{P}b", (void *)OFFSET(struct UserVar, b), "b", 
    "double", 1, 104, 16, 1, 0, 0, 0},

  {"{P}c", (void *)OFFSET(struct UserVar, c), "c", 
    "double", 1, 105, 16, 1, 0, 0, 0},

  {"{P}alpha", (void *)OFFSET(struct UserVar, alpha), "alpha", 
    "double", 1, 106, 16, 1, 0, 0, 0},

  {"{P}beta", (void *)OFFSET(struct UserVar, beta), "beta", 
    "double", 1, 107, 16, 1, 0, 0, 0},

  {"{P}gamma", (void *)OFFSET(struct UserVar, Gamma), "Gamma", 
    "double", 1, 108, 16, 1, 0, 0, 0},

  {"{P}lambda", (void *)OFFSET(struct UserVar, lambda), "lambda", 
    "double", 1, 109, 0, 1, 0, 0, 0},

  {"{P}lambda_event", (void *)OFFSET(struct UserVar, lambda_event), "lambda_event", 
    "double", 1, 110, 17, 1, 0, 0, 0},

  {"{P}load_lambda", (void *)OFFSET(struct UserVar, load_lambda), "load_lambda", 
    "double", 1, 111, 0, 0, 0, 0, 0},

  {"{P}energy", (void *)OFFSET(struct UserVar, energy), "energy", 
    "double", 1, 112, 0, 1, 0, 0, 0},

  {"{P}energy_event", (void *)OFFSET(struct UserVar, energy_event), "energy_event", 
    "double", 1, 113, 18, 1, 0, 0, 0},

  {"{P}load_energy", (void *)OFFSET(struct UserVar, load_energy), "load_energy", 
    "double", 1, 114, 0, 0, 0, 0, 0},

  {"{P}A0_11", (void *)OFFSET(struct UserVar, A0_11), "A0_11", 
    "double", 1, 115, 19, 1, 0, 0, 0},

  {"{P}A0_12", (void *)OFFSET(struct UserVar, A0_12), "A0_12", 
    "double", 1, 116, 19, 1, 0, 0, 0},

  {"{P}A0_13", (void *)OFFSET(struct UserVar, A0_13), "A0_13", 
    "double", 1, 117, 19, 1, 0, 0, 0},

  {"{P}A0_21", (void *)OFFSET(struct UserVar, A0_21), "A0_21", 
    "double", 1, 118, 19, 1, 0, 0, 0},

  {"{P}A0_22", (void *)OFFSET(struct UserVar, A0_22), "A0_22", 
    "double", 1, 119, 19, 1, 0, 0, 0},

  {"{P}A0_23", (void *)OFFSET(struct UserVar, A0_23), "A0_23", 
    "double", 1, 120, 19, 1, 0, 0, 0},

  {"{P}A0_31", (void *)OFFSET(struct UserVar, A0_31), "A0_31", 
    "double", 1, 121, 19, 1, 0, 0, 0},

  {"{P}A0_32", (void *)OFFSET(struct UserVar, A0_32), "A0_32", 
    "double", 1, 122, 19, 1, 0, 0, 0},

  {"{P}A0_33", (void *)OFFSET(struct UserVar, A0_33), "A0_33", 
    "double", 1, 123, 19, 1, 0, 0, 0},

  {"{P}OMTX_11", (void *)OFFSET(struct UserVar, OMTX_11), "OMTX_11", 
    "double", 1, 124, 20, 1, 0, 0, 0},

  {"{P}OMTX_12", (void *)OFFSET(struct UserVar, OMTX_12), "OMTX_12", 
    "double", 1, 125, 20, 1, 0, 0, 0},

  {"{P}OMTX_13", (void *)OFFSET(struct UserVar, OMTX_13), "OMTX_13", 
    "double", 1, 126, 20, 1, 0, 0, 0},

  {"{P}OMTX_21", (void *)OFFSET(struct UserVar, OMTX_21), "OMTX_21", 
    "double", 1, 127, 20, 1, 0, 0, 0},

  {"{P}OMTX_22", (void *)OFFSET(struct UserVar, OMTX_22), "OMTX_22", 
    "double", 1, 128, 20, 1, 0, 0, 0},

  {"{P}OMTX_23", (void *)OFFSET(struct UserVar, OMTX_23), "OMTX_23", 
    "double", 1, 129, 20, 1, 0, 0, 0},

  {"{P}OMTX_31", (void *)OFFSET(struct UserVar, OMTX_31), "OMTX_31", 
    "double", 1, 130, 20, 1, 0, 0, 0},

  {"{P}OMTX_32", (void *)OFFSET(struct UserVar, OMTX_32), "OMTX_32", 
    "double", 1, 131, 20, 1, 0, 0, 0},

  {"{P}OMTX_33", (void *)OFFSET(struct UserVar, OMTX_33), "OMTX_33", 
    "double", 1, 132, 20, 1, 0, 0, 0},

  {"{P}errAngle", (void *)OFFSET(struct UserVar, errAngle), "errAngle", 
    "double", 1, 133, 0, 0, 0, 0, 0},

  {"{P}errAngleThresh", (void *)OFFSET(struct UserVar, errAngleThresh), "errAngleThresh", 
    "double", 1, 134, 0, 1, 0, 0, 0},

  {"{P}newHKL", (void *)OFFSET(struct UserVar, newHKL), "newHKL", 
    "short", 1, 135, 0, 0, 0, 0, 0},

  {"{P}newAngles", (void *)OFFSET(struct UserVar, newAngles), "newAngles", 
    "short", 1, 136, 0, 0, 0, 0, 0},

  {"{P}newMotors", (void *)OFFSET(struct UserVar, newMotors), "newMotors", 
    "short", 1, 137, 0, 0, 0, 0, 0},

  {"{P}waiting4Mot", (void *)OFFSET(struct UserVar, waiting4Mot), "waiting4Mot", 
    "short", 1, 138, 0, 0, 0, 0, 0},

  {"{P}waiting4motTTH", (void *)OFFSET(struct UserVar, waiting4motTTH), "waiting4motTTH", 
    "short", 1, 139, 0, 1, 0, 0, 0},

  {"{P}waiting4motTH", (void *)OFFSET(struct UserVar, waiting4motTH), "waiting4motTH", 
    "short", 1, 140, 0, 1, 0, 0, 0},

  {"{P}waiting4motCHI", (void *)OFFSET(struct UserVar, waiting4motCHI), "waiting4motCHI", 
    "short", 1, 141, 0, 1, 0, 0, 0},

  {"{P}waiting4motPHI", (void *)OFFSET(struct UserVar, waiting4motPHI), "waiting4motPHI", 
    "short", 1, 142, 0, 1, 0, 0, 0},

  {"{P}A0_state", (void *)OFFSET(struct UserVar, A0_state), "A0_state", 
    "long", 1, 143, 0, 1, 0, 0, 0},

  {"{P}OMTX_state", (void *)OFFSET(struct UserVar, OMTX_state), "OMTX_state", 
    "long", 1, 144, 0, 1, 0, 0, 0},

  {"{P}weWrote_motTTH", (void *)OFFSET(struct UserVar, weWrote_motTTH), "weWrote_motTTH", 
    "int", 1, 145, 0, 1, 0, 0, 0},

  {"{P}weWrote_motTH", (void *)OFFSET(struct UserVar, weWrote_motTH), "weWrote_motTH", 
    "int", 1, 146, 0, 1, 0, 0, 0},

  {"{P}weWrote_motCHI", (void *)OFFSET(struct UserVar, weWrote_motCHI), "weWrote_motCHI", 
    "int", 1, 147, 0, 1, 0, 0, 0},

  {"{P}weWrote_motPHI", (void *)OFFSET(struct UserVar, weWrote_motPHI), "weWrote_motPHI", 
    "int", 1, 148, 0, 1, 0, 0, 0},

};

/* Event masks for state set orient */
	/* Event mask for state init_wait: */
static bitMask	EM_orient_init_wait[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state init: */
static bitMask	EM_orient_init[] = {
	0x00060000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00018000,
};
	/* Event mask for state ref_show: */
static bitMask	EM_orient_ref_show[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state enter_monitor_all: */
static bitMask	EM_orient_enter_monitor_all[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state monitor_all_delay: */
static bitMask	EM_orient_monitor_all_delay[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state monitor_all: */
static bitMask	EM_orient_monitor_all[] = {
	0x251ffef2,
	0x00300078,
	0x30000000,
	0x00000060,
	0x001efc00,
};
	/* Event mask for state newA0: */
static bitMask	EM_orient_newA0[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00008000,
};
	/* Event mask for state newOMTX: */
static bitMask	EM_orient_newOMTX[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00010000,
};
	/* Event mask for state newHKL: */
static bitMask	EM_orient_newHKL[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00018000,
};
	/* Event mask for state newAngles: */
static bitMask	EM_orient_newAngles[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00018000,
};
	/* Event mask for state checkDone: */
static bitMask	EM_orient_checkDone[] = {
	0x00000000,
	0x00200000,
	0x00000000,
	0x00000000,
	0x00007c00,
};
	/* Event mask for state newMotors: */
static bitMask	EM_orient_newMotors[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state putAll: */
static bitMask	EM_orient_putAll[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_orient[] = {
	/* State "init_wait" */ {
	/* state name */       "init_wait",
	/* action function */ (ACTION_FUNC) A_orient_init_wait,
	/* event function */  (EVENT_FUNC) E_orient_init_wait,
	/* delay function */   (DELAY_FUNC) D_orient_init_wait,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_orient_init_wait,
	/* state options */   (0)},

	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_orient_init,
	/* event function */  (EVENT_FUNC) E_orient_init,
	/* delay function */   (DELAY_FUNC) D_orient_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_orient_init,
	/* state options */   (0)},

	/* State "ref_show" */ {
	/* state name */       "ref_show",
	/* action function */ (ACTION_FUNC) A_orient_ref_show,
	/* event function */  (EVENT_FUNC) E_orient_ref_show,
	/* delay function */   (DELAY_FUNC) D_orient_ref_show,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_orient_ref_show,
	/* state options */   (0)},

	/* State "enter_monitor_all" */ {
	/* state name */       "enter_monitor_all",
	/* action function */ (ACTION_FUNC) A_orient_enter_monitor_all,
	/* event function */  (EVENT_FUNC) E_orient_enter_monitor_all,
	/* delay function */   (DELAY_FUNC) D_orient_enter_monitor_all,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_orient_enter_monitor_all,
	/* state options */   (0)},

	/* State "monitor_all_delay" */ {
	/* state name */       "monitor_all_delay",
	/* action function */ (ACTION_FUNC) A_orient_monitor_all_delay,
	/* event function */  (EVENT_FUNC) E_orient_monitor_all_delay,
	/* delay function */   (DELAY_FUNC) D_orient_monitor_all_delay,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_orient_monitor_all_delay,
	/* state options */   (0)},

	/* State "monitor_all" */ {
	/* state name */       "monitor_all",
	/* action function */ (ACTION_FUNC) A_orient_monitor_all,
	/* event function */  (EVENT_FUNC) E_orient_monitor_all,
	/* delay function */   (DELAY_FUNC) D_orient_monitor_all,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_orient_monitor_all,
	/* state options */   (0)},

	/* State "newA0" */ {
	/* state name */       "newA0",
	/* action function */ (ACTION_FUNC) A_orient_newA0,
	/* event function */  (EVENT_FUNC) E_orient_newA0,
	/* delay function */   (DELAY_FUNC) D_orient_newA0,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_orient_newA0,
	/* state options */   (0)},

	/* State "newOMTX" */ {
	/* state name */       "newOMTX",
	/* action function */ (ACTION_FUNC) A_orient_newOMTX,
	/* event function */  (EVENT_FUNC) E_orient_newOMTX,
	/* delay function */   (DELAY_FUNC) D_orient_newOMTX,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_orient_newOMTX,
	/* state options */   (0)},

	/* State "newHKL" */ {
	/* state name */       "newHKL",
	/* action function */ (ACTION_FUNC) A_orient_newHKL,
	/* event function */  (EVENT_FUNC) E_orient_newHKL,
	/* delay function */   (DELAY_FUNC) D_orient_newHKL,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_orient_newHKL,
	/* state options */   (0)},

	/* State "newAngles" */ {
	/* state name */       "newAngles",
	/* action function */ (ACTION_FUNC) A_orient_newAngles,
	/* event function */  (EVENT_FUNC) E_orient_newAngles,
	/* delay function */   (DELAY_FUNC) D_orient_newAngles,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_orient_newAngles,
	/* state options */   (0)},

	/* State "checkDone" */ {
	/* state name */       "checkDone",
	/* action function */ (ACTION_FUNC) A_orient_checkDone,
	/* event function */  (EVENT_FUNC) E_orient_checkDone,
	/* delay function */   (DELAY_FUNC) D_orient_checkDone,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_orient_checkDone,
	/* state options */   (0)},

	/* State "newMotors" */ {
	/* state name */       "newMotors",
	/* action function */ (ACTION_FUNC) A_orient_newMotors,
	/* event function */  (EVENT_FUNC) E_orient_newMotors,
	/* delay function */   (DELAY_FUNC) D_orient_newMotors,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_orient_newMotors,
	/* state options */   (0)},

	/* State "putAll" */ {
	/* state name */       "putAll",
	/* action function */ (ACTION_FUNC) A_orient_putAll,
	/* event function */  (EVENT_FUNC) E_orient_putAll,
	/* delay function */   (DELAY_FUNC) D_orient_putAll,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_orient_putAll,
	/* state options */   (0)},


};

/* Event masks for state set connections */
	/* Event mask for state check: */
static bitMask	EM_connections_check[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_connections[] = {
	/* State "check" */ {
	/* state name */       "check",
	/* action function */ (ACTION_FUNC) A_connections_check,
	/* event function */  (EVENT_FUNC) E_connections_check,
	/* delay function */   (DELAY_FUNC) D_connections_check,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_connections_check,
	/* state options */   (0)},


};

/* Event masks for state set readback */
	/* Event mask for state rbv_monitor_throttle: */
static bitMask	EM_readback_rbv_monitor_throttle[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state rbv_monitor_wait: */
static bitMask	EM_readback_rbv_monitor_wait[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00018000,
};
	/* Event mask for state rbv_monitor: */
static bitMask	EM_readback_rbv_monitor[] = {
	0x00000100,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00018000,
};

/* State Blocks */

static struct seqState state_readback[] = {
	/* State "rbv_monitor_throttle" */ {
	/* state name */       "rbv_monitor_throttle",
	/* action function */ (ACTION_FUNC) A_readback_rbv_monitor_throttle,
	/* event function */  (EVENT_FUNC) E_readback_rbv_monitor_throttle,
	/* delay function */   (DELAY_FUNC) D_readback_rbv_monitor_throttle,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_readback_rbv_monitor_throttle,
	/* state options */   (0)},

	/* State "rbv_monitor_wait" */ {
	/* state name */       "rbv_monitor_wait",
	/* action function */ (ACTION_FUNC) A_readback_rbv_monitor_wait,
	/* event function */  (EVENT_FUNC) E_readback_rbv_monitor_wait,
	/* delay function */   (DELAY_FUNC) D_readback_rbv_monitor_wait,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_readback_rbv_monitor_wait,
	/* state options */   (0)},

	/* State "rbv_monitor" */ {
	/* state name */       "rbv_monitor",
	/* action function */ (ACTION_FUNC) A_readback_rbv_monitor,
	/* event function */  (EVENT_FUNC) E_readback_rbv_monitor,
	/* delay function */   (DELAY_FUNC) D_readback_rbv_monitor,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_readback_rbv_monitor,
	/* state options */   (0)},


};

/* Event masks for state set stop */
	/* Event mask for state stop_all: */
static bitMask	EM_stop_stop_all[] = {
	0x00000000,
	0x00008000,
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_stop[] = {
	/* State "stop_all" */ {
	/* state name */       "stop_all",
	/* action function */ (ACTION_FUNC) A_stop_stop_all,
	/* event function */  (EVENT_FUNC) E_stop_stop_all,
	/* delay function */   (DELAY_FUNC) D_stop_stop_all,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_stop_stop_all,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "orient" */ {
	/* ss name */            "orient",
	/* ptr to state block */ state_orient,
	/* number of states */   13,
	/* error state */        -1},


	/* State set "connections" */ {
	/* ss name */            "connections",
	/* ptr to state block */ state_connections,
	/* number of states */   1,
	/* error state */        -1},


	/* State set "readback" */ {
	/* ss name */            "readback",
	/* ptr to state block */ state_readback,
	/* number of states */   3,
	/* error state */        -1},


	/* State set "stop" */ {
	/* ss name */            "stop",
	/* ptr to state block */ state_stop,
	/* number of states */   1,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "P=xxx:orient1:,PM=xxx:,mTTH=m9,mTH=m10,mCHI=m11,mPHI=m12";

/* State Program table (global) */
struct seqProgram orient = {
	/* magic number */       20000315,
	/* *name */              "orient",
	/* *pChannels */         seqChan,
	/* numChans */           NUM_CHANNELS,
	/* *pSS */               seqSS,
	/* numSS */              NUM_SS,
	/* user variable size */ sizeof(struct UserVar),
	/* *pParams */           prog_param,
	/* numEvents */          NUM_EVENTS,
	/* encoded options */    (0 | OPT_CONN | OPT_NEWEF | OPT_REENT),
	/* entry handler */      (ENTRY_FUNC) entry_handler,
	/* exit handler */       (EXIT_FUNC) exit_handler,
	/* numQueues */          NUM_QUEUES,
};
#include "epicsExport.h"


/* Register sequencer commands and program */

void orientRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&orient);
}
epicsExportRegistrar(orientRegistrar);

