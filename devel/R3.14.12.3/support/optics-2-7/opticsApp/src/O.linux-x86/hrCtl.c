/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: hrCtl.i */

/* Event flags */
#define opMode_mon	1
#define Geom_mon	2
#define hr_Moving_mon	3
#define H1_mon	4
#define K1_mon	5
#define L1_mon	6
#define A1_mon	7
#define H2_mon	8
#define K2_mon	9
#define L2_mon	10
#define A2_mon	11
#define E_mon	12
#define lambda_mon	13
#define theta1_mon	14
#define phi1_mon	15
#define phi1Off_mon	16
#define theta2_mon	17
#define phi2_mon	18
#define phi2Off_mon	19
#define phi1Dmov_mon	20
#define phi2Dmov_mon	21
#define phi1MotHiLim_mon	22
#define phi1MotLoLim_mon	23
#define phi2MotHiLim_mon	24
#define phi2MotLoLim_mon	25
#define phi1MotCmdMon_mon	26
#define phi2MotCmdMon_mon	27
#define phi1MotRdbk_mon	28
#define phi1MotRdbkPseudo_mon	29
#define phi2MotRdbk_mon	30
#define phi2MotRdbkPseudo_mon	31
#define worldOff_mon	32
#define useSetMode_mon	33

/* Program "hrCtl" */
#include "seqCom.h"

#define NUM_SS 2
#define NUM_CHANNELS 91
#define NUM_EVENTS 33
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT FALSE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT TRUE

extern struct seqProgram hrCtl;

/* Variable declarations */
struct UserVar {
	short	hrCtlDebug;
	char	seqMsg1[MAX_STRING_SIZE];
	char	seqMsg2[MAX_STRING_SIZE];
	short	opAlert;
	short	opAck;
	short	putVals;
	short	autoMode;
	short	opMode;
	short	Geom;
	short	hr_Moving;
	short	hr_Done;
	double	H1;
	double	K1;
	double	L1;
	double	A1;
	double	D1;
	double	H2;
	double	K2;
	double	L2;
	double	A2;
	double	D2;
	double	E;
	double	EHi;
	double	ELo;
	double	ERdbk;
	double	lambda;
	double	lambdaNom;
	double	lambdaHi;
	double	lambdaLo;
	double	lambdaRdbk;
	double	theta1;
	double	theta1Hi;
	double	theta1Lo;
	double	theta1Rdbk;
	double	phi1;
	double	phi1Off;
	double	phi1Hi;
	double	phi1Lo;
	double	phi1Rdbk;
	double	theta2;
	double	theta2Nom;
	double	theta2Hi;
	double	theta2Lo;
	double	theta2Rdbk;
	double	phi2;
	double	phi2Off;
	double	phi2Hi;
	double	phi2Lo;
	double	phi2Rdbk;
	char	phi1MotName[MAX_STRING_SIZE];
	char	phi2MotName[MAX_STRING_SIZE];
	double	phi1MotCmdEcho;
	double	phi2MotCmdEcho;
	double	phi1MotRdbkEcho;
	double	phi2MotRdbkEcho;
	short	phi1DmovEcho;
	short	phi2DmovEcho;
	short	phi1MotStop;
	short	phi2MotStop;
	short	phi1Dmov;
	short	phi2Dmov;
	short	phi1Hls;
	short	phi1Lls;
	short	phi2Hls;
	short	phi2Lls;
	double	phi1MotDesired;
	double	phi2MotDesired;
	double	phi1MotHiLim;
	double	phi1MotLoLim;
	double	phi2MotHiLim;
	double	phi2MotLoLim;
	double	phi1MotCmd;
	double	phi2MotCmd;
	double	phi1MotCmdMon;
	double	phi2MotCmdMon;
	double	phi1MotRdbk;
	double	phi2MotRdbk;
	double	worldOff;
	double	worldTweakVal;
	short	worldTweakInc;
	short	worldTweakDec;
	short	useSetMode;
	double	ETweakVal;
	short	ETweakInc;
	short	ETweakDec;
	double	LTweakVal;
	short	LTweakInc;
	short	LTweakDec;
	double	th1TweakVal;
	short	th1TweakInc;
	short	th1TweakDec;
	double	th2TweakVal;
	short	th2TweakInc;
	short	th2TweakDec;
	short	toggle;
	short	causedMove;
	short	initSeq;
	short	initSeqDone;
	double	hc;
	double	PI;
	double	R2D;
	double	uR2D;
	double	D2R;
	double	D2uR;
	double	temp;
};

/* C code definitions */
# line 314 "../hrCtl.st"
 short hrCtl_LS_TEST = 0;
# line 474 "../hrCtl.st"
#include <string.h>
# line 475 "../hrCtl.st"
#include <math.h>
# line 477 "../hrCtl.st"
double hrCtl_calc2dSpacing(struct UserVar *pVar, int xtal, double A, double H, double K, double L);
# line 478 "../hrCtl.st"
void hrCtl_calcReadback(struct UserVar *pVar);
# line 479 "../hrCtl.st"
char *hrCtl_pmacP;
# line 480 "../hrCtl.st"
char *hrCtl_pmacM;

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "init" in state set "hr_Ctl" */

/* Delay function for state "init" in state set "hr_Ctl" */
static void D_hr_Ctl_init(SS_ID ssId, struct UserVar *pVar)
{
# line 499 "../hrCtl.st"
}

/* Event function for state "init" in state set "hr_Ctl" */
static long E_hr_Ctl_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 499 "../hrCtl.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "hr_Ctl" */
static void A_hr_Ctl_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 498 "../hrCtl.st"
			(pVar->initSeq) = 0;
		}
		return;
	}
}
/* Code for state "initSequence" in state set "hr_Ctl" */

/* Delay function for state "initSequence" in state set "hr_Ctl" */
static void D_hr_Ctl_initSequence(SS_ID ssId, struct UserVar *pVar)
{
# line 539 "../hrCtl.st"
# line 543 "../hrCtl.st"
# line 547 "../hrCtl.st"
# line 574 "../hrCtl.st"
}

/* Event function for state "initSequence" in state set "hr_Ctl" */
static long E_hr_Ctl_initSequence(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 539 "../hrCtl.st"
	if ((pVar->initSeq) == 0)
	{
		*pNextState = 7;
		*pTransNum = 0;
		return TRUE;
	}
# line 543 "../hrCtl.st"
	if ((pVar->initSeq) == 1)
	{
		*pNextState = 16;
		*pTransNum = 1;
		return TRUE;
	}
# line 547 "../hrCtl.st"
	if ((pVar->initSeq) == 2)
	{
		*pNextState = 14;
		*pTransNum = 2;
		return TRUE;
	}
# line 574 "../hrCtl.st"
	if ((pVar->initSeq) == 3)
	{
		*pNextState = 11;
		*pTransNum = 3;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "initSequence" in state set "hr_Ctl" */
static void A_hr_Ctl_initSequence(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 505 "../hrCtl.st"
			(pVar->hc) = 12.3984244;
			(pVar->PI) = 4 * atan(1.0);
			(pVar->R2D) = 180 / (pVar->PI);
			(pVar->uR2D) = (pVar->R2D) / 1000000;
			(pVar->D2R) = (pVar->PI) / 180;
			(pVar->D2uR) = 1000000 * (pVar->PI) / 180;
			(pVar->phi1MotStop) = 1;
			(pVar->phi2MotStop) = 1;
			(pVar->putVals) = 0;
			seq_pvPut(ssId, 5 /* putVals */, 0);
			(pVar->autoMode) = 0;
			seq_pvPut(ssId, 6 /* autoMode */, 0);
			(pVar->opAck) = 0;
			seq_pvPut(ssId, 4 /* opAck */, 0);
# line 520 "../hrCtl.st"

    hrCtl_pmacP = seq_macValueGet(ssId,"P");
    hrCtl_pmacM = seq_macValueGet(ssId,"M_PHI1");
    if (hrCtl_pmacP == 0 || hrCtl_pmacM == 0) {
     sprintf(pVar->phi1MotName,"??????");
    } else {
     strcpy(pVar->phi1MotName, hrCtl_pmacP);
     strcat(pVar->phi1MotName, hrCtl_pmacM);
    }
    hrCtl_pmacM = seq_macValueGet(ssId,"M_PHI2");
    if (hrCtl_pmacP == 0 || hrCtl_pmacM == 0) {
     sprintf(pVar->phi2MotName,"??????");
    } else {
     strcpy(pVar->phi2MotName, hrCtl_pmacP);
     strcat(pVar->phi2MotName, hrCtl_pmacM);
    }
# line 535 "../hrCtl.st"
   
			seq_pvPut(ssId, 46 /* phi1MotName */, 0);
			seq_pvPut(ssId, 47 /* phi2MotName */, 0);
			(pVar->initSeq) = 1;
		}
		return;
	case 1:
		{
# line 542 "../hrCtl.st"
			(pVar->initSeq) = 2;
		}
		return;
	case 2:
		{
# line 546 "../hrCtl.st"
			(pVar->initSeq) = 3;
		}
		return;
	case 3:
		{
# line 550 "../hrCtl.st"
			sprintf((pVar->seqMsg1), "HR Control Ready");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
# line 553 "../hrCtl.st"
if (hrCtl_LS_TEST) {
# line 553 "../hrCtl.st"
			sprintf((pVar->seqMsg2), "LIMIT SWITCH TEST VERSION !!!!");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			(pVar->opAlert) = 1;
			seq_pvPut(ssId, 3 /* opAlert */, 0);
# line 558 "../hrCtl.st"
} else {
# line 558 "../hrCtl.st"
			sprintf((pVar->seqMsg2), " ");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
# line 561 "../hrCtl.st"
}
# line 561 "../hrCtl.st"
			(pVar->initSeqDone) = 1;
			(pVar->phi1) = (pVar->phi1MotRdbk) + ((pVar->phi1Off) + (pVar->worldOff) * (pVar->uR2D));
			seq_pvPut(ssId, 32 /* phi1 */, 0);
			(pVar->theta1) = (pVar->phi1);
			seq_pvPut(ssId, 28 /* theta1 */, 0);
# line 568 "../hrCtl.st"
			if ((pVar->Geom) == 0)
			{
				(pVar->phi2) = (pVar->phi2MotRdbk) * (pVar->uR2D) + (pVar->phi2Off) + (pVar->worldOff) * (pVar->uR2D);
			}
# line 570 "../hrCtl.st"
			else
			{
				(pVar->phi2) = (pVar->phi2MotRdbk) * (pVar->uR2D) + (pVar->phi2Off) - (pVar->worldOff) * (pVar->uR2D);
			}
# line 571 "../hrCtl.st"
			seq_pvPut(ssId, 32 /* phi1 */, 0);
			(pVar->theta2) = (pVar->phi2) - (pVar->phi1) - (pVar->theta1);
			seq_pvPut(ssId, 37 /* theta2 */, 0);
		}
		return;
	}
}
/* Code for state "waitForCmndEnter" in state set "hr_Ctl" */

/* Delay function for state "waitForCmndEnter" in state set "hr_Ctl" */
static void D_hr_Ctl_waitForCmndEnter(SS_ID ssId, struct UserVar *pVar)
{
# line 662 "../hrCtl.st"
	seq_delayInit(ssId, 0, (.1));
# line 665 "../hrCtl.st"
}

/* Event function for state "waitForCmndEnter" in state set "hr_Ctl" */
static long E_hr_Ctl_waitForCmndEnter(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 662 "../hrCtl.st"
	if ((pVar->initSeqDone) && seq_delay(ssId, 0))
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
# line 665 "../hrCtl.st"
	if (!(pVar->initSeqDone))
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "waitForCmndEnter" in state set "hr_Ctl" */
static void A_hr_Ctl_waitForCmndEnter(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 581 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:waitForCmdEnter\n");
# line 581 "../hrCtl.st"
			seq_efClear(ssId, H1_mon);
			seq_efClear(ssId, K1_mon);
			seq_efClear(ssId, L1_mon);
			seq_efClear(ssId, A1_mon);
			seq_efClear(ssId, H2_mon);
			seq_efClear(ssId, K2_mon);
			seq_efClear(ssId, L2_mon);
			seq_efClear(ssId, A2_mon);
			seq_efClear(ssId, E_mon);
			seq_efClear(ssId, lambda_mon);
			seq_efClear(ssId, theta1_mon);
			seq_efClear(ssId, theta2_mon);
			seq_efClear(ssId, phi1Off_mon);
			seq_efClear(ssId, phi2Off_mon);
			seq_efClear(ssId, worldOff_mon);
			seq_efClear(ssId, useSetMode_mon);
			seq_efClear(ssId, opMode_mon);
			seq_efClear(ssId, Geom_mon);
# line 600 "../hrCtl.st"
			(pVar->phi1MotRdbkEcho) = (pVar->phi1MotRdbk);
			seq_pvPut(ssId, 50 /* phi1MotRdbkEcho */, 0);
			(pVar->phi2MotRdbkEcho) = (pVar->phi2MotRdbk);
			seq_pvPut(ssId, 51 /* phi2MotRdbkEcho */, 0);
# line 605 "../hrCtl.st"
			(pVar->phi1MotCmdEcho) = (pVar->phi1MotCmdMon);
			seq_pvPut(ssId, 48 /* phi1MotCmdEcho */, 0);
			(pVar->phi1DmovEcho) = (pVar->phi1Dmov);
			seq_pvPut(ssId, 52 /* phi1DmovEcho */, 0);
# line 610 "../hrCtl.st"
			(pVar->phi2MotCmdEcho) = (pVar->phi2MotCmdMon);
			seq_pvPut(ssId, 49 /* phi2MotCmdEcho */, 0);
			(pVar->phi2DmovEcho) = (pVar->phi2Dmov);
			seq_pvPut(ssId, 53 /* phi2DmovEcho */, 0);
# line 653 "../hrCtl.st"
			if ((pVar->autoMode) && 0)
			{
				(pVar->phi1) = (pVar->phi1MotCmdMon) * (pVar->uR2D) + ((pVar->phi1Off) + (pVar->worldOff) * (pVar->uR2D));
				seq_pvPut(ssId, 32 /* phi1 */, 0);
				(pVar->theta1) = (pVar->phi1);
				if ((pVar->Geom) == 0)
				{
					(pVar->phi2) = (pVar->phi2MotCmdMon) * (pVar->uR2D) + (pVar->phi2Off) + (pVar->worldOff) * (pVar->uR2D);
				}
				else
				{
					(pVar->phi2) = (pVar->phi2MotCmdMon) * (pVar->uR2D) + (pVar->phi2Off) - (pVar->worldOff) * (pVar->uR2D);
				}
				seq_pvPut(ssId, 41 /* phi2 */, 0);
				if (((pVar->opMode) == 0) || ((pVar->opMode) == 1))
				{
					(pVar->lambda) = (pVar->D1) * sin((pVar->theta1) * (pVar->D2R));
				}
				else
				{
					(pVar->theta2Nom) = asin((pVar->D1) * sin((pVar->theta1) * (pVar->D2R)) / (pVar->D2)) * (pVar->R2D);
					(pVar->lambda) = (pVar->D1) * sin((pVar->theta1) * (pVar->D2R)) + (pVar->D2R) * ((pVar->phi2) - (pVar->phi1) - (pVar->theta1) - (pVar->theta2Nom)) / (1 / ((pVar->D1) * cos((pVar->theta1) * (pVar->D2R))) + 1 / ((pVar->D2) * cos((pVar->theta2Nom) * (pVar->D2R))));
				}
				seq_pvPut(ssId, 24 /* lambda */, 0);
				(pVar->theta1) = asin((pVar->lambda) / (pVar->D1)) * (pVar->R2D);
				seq_pvPut(ssId, 28 /* theta1 */, 0);
				(pVar->theta2) = asin((pVar->lambda) / (pVar->D2)) * (pVar->R2D);
				seq_pvPut(ssId, 37 /* theta2 */, 0);
				(pVar->E) = (pVar->hc) / (pVar->lambda);
				seq_pvPut(ssId, 20 /* E */, 0);
hrCtl_calcReadback(pVar);
				seq_pvPut(ssId, 36 /* phi1Rdbk */, 0);
				seq_pvPut(ssId, 31 /* theta1Rdbk */, 0);
				seq_pvPut(ssId, 45 /* phi2Rdbk */, 0);
				seq_pvPut(ssId, 40 /* theta2Rdbk */, 0);
				seq_pvPut(ssId, 27 /* lambdaRdbk */, 0);
				seq_pvPut(ssId, 23 /* ERdbk */, 0);
			}
# line 654 "../hrCtl.st"
			(pVar->opAck) = 0;
			seq_pvPut(ssId, 4 /* opAck */, 0);
# line 657 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:waitForCmndEnter: Moving=%d; Done=%d\n", pVar->hr_Moving, pVar->hr_Done);
# line 661 "../hrCtl.st"
			if ((pVar->hr_Moving) && (pVar->hr_Done))
			{
				if ((pVar->hrCtlDebug))
printf("hrCtl:waitForCmndEnter: asserting 'done'.\n");
				(pVar->hr_Moving) = 0;
				(pVar->hr_Done) = 0;
				seq_pvPut(ssId, 9 /* hr_Moving */, 0);
			}
		}
		return;
	case 1:
		{
		}
		return;
	}
}
/* Code for state "waitForCmnd" in state set "hr_Ctl" */

/* Delay function for state "waitForCmnd" in state set "hr_Ctl" */
static void D_hr_Ctl_waitForCmnd(SS_ID ssId, struct UserVar *pVar)
{
# line 676 "../hrCtl.st"
# line 685 "../hrCtl.st"
# line 690 "../hrCtl.st"
# line 695 "../hrCtl.st"
# line 700 "../hrCtl.st"
# line 705 "../hrCtl.st"
# line 711 "../hrCtl.st"
# line 716 "../hrCtl.st"
# line 732 "../hrCtl.st"
# line 736 "../hrCtl.st"
	seq_delayInit(ssId, 0, (.1));
# line 752 "../hrCtl.st"
# line 756 "../hrCtl.st"
# line 764 "../hrCtl.st"
	seq_delayInit(ssId, 1, (.1));
# line 776 "../hrCtl.st"
	seq_delayInit(ssId, 2, (.1));
# line 781 "../hrCtl.st"
# line 790 "../hrCtl.st"
# line 794 "../hrCtl.st"
}

/* Event function for state "waitForCmnd" in state set "hr_Ctl" */
static long E_hr_Ctl_waitForCmnd(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 676 "../hrCtl.st"
	if (seq_efTestAndClear(ssId, phi1MotHiLim_mon) || seq_efTestAndClear(ssId, phi1MotLoLim_mon) || seq_efTestAndClear(ssId, phi2MotHiLim_mon) || seq_efTestAndClear(ssId, phi2MotLoLim_mon))
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
# line 685 "../hrCtl.st"
	if (seq_efTestAndClear(ssId, H1_mon) || seq_efTestAndClear(ssId, K1_mon) || seq_efTestAndClear(ssId, L1_mon) || seq_efTestAndClear(ssId, A1_mon) || seq_efTestAndClear(ssId, H2_mon) || seq_efTestAndClear(ssId, K2_mon) || seq_efTestAndClear(ssId, L2_mon) || seq_efTestAndClear(ssId, A2_mon))
	{
		*pNextState = 7;
		*pTransNum = 1;
		return TRUE;
	}
# line 690 "../hrCtl.st"
	if (seq_efTestAndClear(ssId, phi1MotRdbkPseudo_mon))
	{
		*pNextState = 14;
		*pTransNum = 2;
		return TRUE;
	}
# line 695 "../hrCtl.st"
	if (seq_efTestAndClear(ssId, phi2MotRdbkPseudo_mon))
	{
		*pNextState = 14;
		*pTransNum = 3;
		return TRUE;
	}
# line 700 "../hrCtl.st"
	if (seq_efTest(ssId, E_mon))
	{
		*pNextState = 9;
		*pTransNum = 4;
		return TRUE;
	}
# line 705 "../hrCtl.st"
	if (seq_efTest(ssId, lambda_mon))
	{
		*pNextState = 10;
		*pTransNum = 5;
		return TRUE;
	}
# line 711 "../hrCtl.st"
	if (seq_efTest(ssId, theta1_mon) || seq_efTest(ssId, theta2_mon))
	{
		*pNextState = 11;
		*pTransNum = 6;
		return TRUE;
	}
# line 716 "../hrCtl.st"
	if (seq_efTest(ssId, worldOff_mon))
	{
		*pNextState = 12;
		*pTransNum = 7;
		return TRUE;
	}
# line 732 "../hrCtl.st"
	if (seq_efTest(ssId, phi1Off_mon) || seq_efTest(ssId, phi2Off_mon))
	{
		*pNextState = 5;
		*pTransNum = 8;
		return TRUE;
	}
# line 736 "../hrCtl.st"
	if ((pVar->putVals) && seq_delay(ssId, 0))
	{
		*pNextState = 12;
		*pTransNum = 9;
		return TRUE;
	}
# line 752 "../hrCtl.st"
	if (seq_efTest(ssId, useSetMode_mon) && (pVar->useSetMode))
	{
		*pNextState = 9;
		*pTransNum = 10;
		return TRUE;
	}
# line 756 "../hrCtl.st"
	if (seq_efTest(ssId, useSetMode_mon) && !(pVar->useSetMode))
	{
		*pNextState = 5;
		*pTransNum = 11;
		return TRUE;
	}
# line 764 "../hrCtl.st"
	if (seq_efTestAndClear(ssId, opMode_mon) && seq_delay(ssId, 1))
	{
		*pNextState = 12;
		*pTransNum = 12;
		return TRUE;
	}
# line 776 "../hrCtl.st"
	if (seq_efTestAndClear(ssId, Geom_mon) && seq_delay(ssId, 2))
	{
		*pNextState = 12;
		*pTransNum = 13;
		return TRUE;
	}
# line 781 "../hrCtl.st"
	if ((pVar->ETweakInc) || (pVar->ETweakDec) || (pVar->LTweakInc) || (pVar->LTweakDec) || (pVar->th1TweakInc) || (pVar->th1TweakDec) || (pVar->th2TweakInc) || (pVar->th2TweakDec) || (pVar->worldTweakInc) || (pVar->worldTweakDec))
	{
		*pNextState = 8;
		*pTransNum = 14;
		return TRUE;
	}
# line 790 "../hrCtl.st"
	if ((pVar->opAck))
	{
		*pNextState = 2;
		*pTransNum = 15;
		return TRUE;
	}
# line 794 "../hrCtl.st"
	if (seq_efTest(ssId, hr_Moving_mon))
	{
		*pNextState = 4;
		*pTransNum = 16;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "waitForCmnd" in state set "hr_Ctl" */
static void A_hr_Ctl_waitForCmnd(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 672 "../hrCtl.st"
			sprintf((pVar->seqMsg1), "Motor Soft Limit Changed");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			sprintf((pVar->seqMsg2), " ");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
		}
		return;
	case 1:
		{
# line 681 "../hrCtl.st"
			sprintf((pVar->seqMsg1), "Lattice Spacing Changed");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			sprintf((pVar->seqMsg2), " ");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
		}
		return;
	case 2:
		{
# line 689 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:waitForCmd: phi1MotRdbkPseudo_mon\n");
# line 689 "../hrCtl.st"
			seq_efSet(ssId, phi1MotRdbkPseudo_mon);
		}
		return;
	case 3:
		{
# line 694 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:waitForCmd: phi2MotRdbkPseudo_mon\n");
# line 694 "../hrCtl.st"
			seq_efSet(ssId, phi2MotRdbkPseudo_mon);
		}
		return;
	case 4:
		{
# line 699 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:waitForCmd: E_mon\n");
# line 699 "../hrCtl.st"
			seq_efClear(ssId, E_mon);
		}
		return;
	case 5:
		{
# line 704 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:waitForCmd: lambda_mon\n");
# line 704 "../hrCtl.st"
			seq_efClear(ssId, lambda_mon);
		}
		return;
	case 6:
		{
# line 709 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:waitForCmd: theta*_mon\n");
# line 709 "../hrCtl.st"
			seq_efClear(ssId, theta1_mon);
			seq_efClear(ssId, theta2_mon);
		}
		return;
	case 7:
		{
# line 715 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:waitForCmd: worldOff_mon\n");
# line 715 "../hrCtl.st"
			seq_efClear(ssId, worldOff_mon);
		}
		return;
	case 8:
		{
# line 720 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:waitForCmd: phi*Off_mon\n");
# line 720 "../hrCtl.st"
			seq_efClear(ssId, phi1Off_mon);
			seq_efClear(ssId, phi2Off_mon);
# line 724 "../hrCtl.st"
hrCtl_calcReadback(pVar);
# line 724 "../hrCtl.st"
			seq_pvPut(ssId, 36 /* phi1Rdbk */, 0);
			seq_pvPut(ssId, 31 /* theta1Rdbk */, 0);
# line 729 "../hrCtl.st"
			if ((pVar->opMode) != 0)
			{
				seq_pvPut(ssId, 45 /* phi2Rdbk */, 0);
				seq_pvPut(ssId, 40 /* theta2Rdbk */, 0);
			}
# line 730 "../hrCtl.st"
			seq_pvPut(ssId, 27 /* lambdaRdbk */, 0);
			seq_pvPut(ssId, 23 /* ERdbk */, 0);
		}
		return;
	case 9:
		{
# line 736 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:waitForCmnd:putVals\n");
		}
		return;
	case 10:
		{
# line 739 "../hrCtl.st"
			seq_efClear(ssId, useSetMode_mon);
			(pVar->EHi) = 0;
# line 740 "../hrCtl.st"
			seq_pvPut(ssId, 21 /* EHi */, 0);
			(pVar->ELo) = 0;
# line 741 "../hrCtl.st"
			seq_pvPut(ssId, 22 /* ELo */, 0);
			(pVar->lambdaHi) = 0;
# line 742 "../hrCtl.st"
			seq_pvPut(ssId, 25 /* lambdaHi */, 0);
			(pVar->lambdaLo) = 0;
# line 743 "../hrCtl.st"
			seq_pvPut(ssId, 26 /* lambdaLo */, 0);
			(pVar->theta1Hi) = 0;
# line 744 "../hrCtl.st"
			seq_pvPut(ssId, 29 /* theta1Hi */, 0);
			(pVar->theta1Lo) = 0;
# line 745 "../hrCtl.st"
			seq_pvPut(ssId, 30 /* theta1Lo */, 0);
			(pVar->phi1Hi) = 0;
# line 746 "../hrCtl.st"
			seq_pvPut(ssId, 34 /* phi1Hi */, 0);
			(pVar->phi1Lo) = 0;
# line 747 "../hrCtl.st"
			seq_pvPut(ssId, 35 /* phi1Lo */, 0);
			(pVar->theta2Hi) = 0;
# line 748 "../hrCtl.st"
			seq_pvPut(ssId, 38 /* theta2Hi */, 0);
			(pVar->theta2Lo) = 0;
# line 749 "../hrCtl.st"
			seq_pvPut(ssId, 39 /* theta2Lo */, 0);
			(pVar->phi2Hi) = 0;
# line 750 "../hrCtl.st"
			seq_pvPut(ssId, 43 /* phi2Hi */, 0);
			(pVar->phi2Lo) = 0;
# line 751 "../hrCtl.st"
			seq_pvPut(ssId, 44 /* phi2Lo */, 0);
		}
		return;
	case 11:
		{
# line 755 "../hrCtl.st"
			seq_efClear(ssId, useSetMode_mon);
		}
		return;
	case 12:
		{
# line 760 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:waitForCmnd: opMode event.\n");
# line 760 "../hrCtl.st"
			(pVar->autoMode) = 0;
			seq_pvPut(ssId, 6 /* autoMode */, 0);
			sprintf((pVar->seqMsg2), "Set to Manual Mode");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
		}
		return;
	case 13:
		{
# line 768 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:waitForCmnd: Geom event.\n");
# line 768 "../hrCtl.st"
			(pVar->autoMode) = 0;
			seq_pvPut(ssId, 6 /* autoMode */, 0);
			sprintf((pVar->seqMsg1), "New geometry.  Switch Phi 2 motor dir.");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			sprintf((pVar->seqMsg2), "and recalibrate monochromator.");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			(pVar->opAlert) = 1;
			seq_pvPut(ssId, 3 /* opAlert */, 0);
		}
		return;
	case 14:
		{
		}
		return;
	case 15:
		{
# line 784 "../hrCtl.st"
			(pVar->opAlert) = 0;
			seq_pvPut(ssId, 3 /* opAlert */, 0);
			sprintf((pVar->seqMsg1), " ");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			sprintf((pVar->seqMsg2), " ");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
		}
		return;
	case 16:
		{
# line 793 "../hrCtl.st"
			seq_efClear(ssId, hr_Moving_mon);
		}
		return;
	}
}
/* Code for state "checkAutoMode" in state set "hr_Ctl" */

/* Delay function for state "checkAutoMode" in state set "hr_Ctl" */
static void D_hr_Ctl_checkAutoMode(SS_ID ssId, struct UserVar *pVar)
{
# line 800 "../hrCtl.st"
# line 802 "../hrCtl.st"
}

/* Event function for state "checkAutoMode" in state set "hr_Ctl" */
static long E_hr_Ctl_checkAutoMode(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 800 "../hrCtl.st"
	if ((pVar->autoMode))
	{
		*pNextState = 6;
		*pTransNum = 0;
		return TRUE;
	}
# line 802 "../hrCtl.st"
	if (1)
	{
		*pNextState = 3;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "checkAutoMode" in state set "hr_Ctl" */
static void A_hr_Ctl_checkAutoMode(SS_ID ssId, struct UserVar *pVar, short transNum)
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
/* Code for state "phiLimits" in state set "hr_Ctl" */

/* Delay function for state "phiLimits" in state set "hr_Ctl" */
static void D_hr_Ctl_phiLimits(SS_ID ssId, struct UserVar *pVar)
{
# line 877 "../hrCtl.st"
}

/* Event function for state "phiLimits" in state set "hr_Ctl" */
static long E_hr_Ctl_phiLimits(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 877 "../hrCtl.st"
	if (TRUE)
	{
		*pNextState = 6;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "phiLimits" in state set "hr_Ctl" */
static void A_hr_Ctl_phiLimits(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 809 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:phiLimits\n");
# line 862 "../hrCtl.st"
			if (!(pVar->useSetMode))
			{
				(pVar->phi1Hi) = (pVar->phi1MotHiLim) * (pVar->uR2D) + ((pVar->phi1Off) + (pVar->worldOff) * (pVar->uR2D));
				(pVar->theta1Hi) = (pVar->phi1Hi);
				(pVar->phi1Lo) = (pVar->phi1MotLoLim) * (pVar->uR2D) + ((pVar->phi1Off) + (pVar->worldOff) * (pVar->uR2D));
				(pVar->theta1Lo) = (pVar->phi1Lo);
				if ((pVar->theta1Hi) > 89.0)
				{
					(pVar->theta1Hi) = 89.0;
					sprintf((pVar->seqMsg2), "Theta1HiLim > 89 deg. (Using 89 deg.)");
					seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
				}
				if ((pVar->theta1Lo) < 1.0)
				{
					(pVar->theta1Lo) = 1.0;
					sprintf((pVar->seqMsg2), "Theta1LoLim < 1 deg. (Using 1 deg.)");
					seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
				}
				seq_pvPut(ssId, 29 /* theta1Hi */, 0);
				seq_pvPut(ssId, 30 /* theta1Lo */, 0);
				if ((pVar->Geom) == 0)
				{
					(pVar->phi2Hi) = (pVar->phi2MotHiLim) * (pVar->uR2D) + (pVar->phi2Off) + (pVar->worldOff) * (pVar->uR2D);
					(pVar->phi2Lo) = (pVar->phi2MotLoLim) * (pVar->uR2D) + (pVar->phi2Off) + (pVar->worldOff) * (pVar->uR2D);
					(pVar->theta2Hi) = (pVar->phi2Hi) - (pVar->phi1Lo) - (pVar->theta1Lo);
					(pVar->theta2Lo) = (pVar->phi2Lo) - (pVar->phi1Hi) - (pVar->theta1Hi);
				}
				else
				{
					(pVar->phi2Hi) = (pVar->phi2MotHiLim) * (pVar->uR2D) + (pVar->phi2Off) - (pVar->worldOff) * (pVar->uR2D);
					(pVar->phi2Lo) = (pVar->phi2MotLoLim) * (pVar->uR2D) + (pVar->phi2Off) - (pVar->worldOff) * (pVar->uR2D);
					(pVar->theta2Hi) = (pVar->phi2Hi);
					(pVar->theta2Lo) = (pVar->phi2Lo);
				}
				if ((pVar->theta2Hi) > 89.0)
				{
					(pVar->theta2Hi) = 89.0;
					sprintf((pVar->seqMsg2), "Theta2HiLim > 89 deg. (Using 89 deg.)");
					seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
				}
				if ((pVar->theta2Lo) < 1.0)
				{
					(pVar->theta2Lo) = 1.0;
					sprintf((pVar->seqMsg2), "Theta2LoLim < 1 deg. (Using 1 deg.)");
					seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
				}
				seq_pvPut(ssId, 38 /* theta2Hi */, 0);
				seq_pvPut(ssId, 39 /* theta2Lo */, 0);
				(pVar->lambdaHi) = (pVar->D1) * sin((pVar->theta1Hi) * (pVar->D2R));
				seq_pvPut(ssId, 25 /* lambdaHi */, 0);
				(pVar->lambdaLo) = (pVar->D1) * sin((pVar->theta1Lo) * (pVar->D2R));
				seq_pvPut(ssId, 26 /* lambdaLo */, 0);
				(pVar->EHi) = (pVar->hc) / (pVar->lambdaLo);
				seq_pvPut(ssId, 21 /* EHi */, 0);
				(pVar->ELo) = (pVar->hc) / (pVar->lambdaHi);
				seq_pvPut(ssId, 22 /* ELo */, 0);
			}
# line 876 "../hrCtl.st"
			else
			{
				(pVar->EHi) = 0;
				seq_pvPut(ssId, 21 /* EHi */, 0);
				(pVar->ELo) = 0;
				seq_pvPut(ssId, 22 /* ELo */, 0);
				(pVar->lambdaHi) = 0;
				seq_pvPut(ssId, 25 /* lambdaHi */, 0);
				(pVar->lambdaLo) = 0;
				seq_pvPut(ssId, 26 /* lambdaLo */, 0);
				(pVar->theta1Hi) = 0;
				seq_pvPut(ssId, 29 /* theta1Hi */, 0);
				(pVar->theta1Lo) = 0;
				seq_pvPut(ssId, 30 /* theta1Lo */, 0);
				(pVar->phi1Hi) = 0;
				seq_pvPut(ssId, 34 /* phi1Hi */, 0);
				(pVar->phi1Lo) = 0;
				seq_pvPut(ssId, 35 /* phi1Lo */, 0);
				(pVar->theta2Hi) = 0;
				seq_pvPut(ssId, 38 /* theta2Hi */, 0);
				(pVar->theta2Lo) = 0;
				seq_pvPut(ssId, 39 /* theta2Lo */, 0);
				(pVar->phi2Hi) = 0;
				seq_pvPut(ssId, 43 /* phi2Hi */, 0);
				(pVar->phi2Lo) = 0;
				seq_pvPut(ssId, 44 /* phi2Lo */, 0);
			}
		}
		return;
	}
}
/* Code for state "checkDone" in state set "hr_Ctl" */

/* Delay function for state "checkDone" in state set "hr_Ctl" */
static void D_hr_Ctl_checkDone(SS_ID ssId, struct UserVar *pVar)
{
# line 884 "../hrCtl.st"
# line 887 "../hrCtl.st"
}

/* Event function for state "checkDone" in state set "hr_Ctl" */
static long E_hr_Ctl_checkDone(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 884 "../hrCtl.st"
	if ((pVar->phi1Dmov) && (pVar->phi2Dmov))
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 887 "../hrCtl.st"
	if (1)
	{
		*pNextState = 14;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "checkDone" in state set "hr_Ctl" */
static void A_hr_Ctl_checkDone(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 883 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:checkDone: done\n");
# line 883 "../hrCtl.st"
			(pVar->hr_Done) = 1;
		}
		return;
	case 1:
		{
		}
		return;
	}
}
/* Code for state "dInputChanged" in state set "hr_Ctl" */

/* Delay function for state "dInputChanged" in state set "hr_Ctl" */
static void D_hr_Ctl_dInputChanged(SS_ID ssId, struct UserVar *pVar)
{
# line 921 "../hrCtl.st"
}

/* Event function for state "dInputChanged" in state set "hr_Ctl" */
static long E_hr_Ctl_dInputChanged(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 921 "../hrCtl.st"
	if (TRUE)
	{
		*pNextState = 11;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "dInputChanged" in state set "hr_Ctl" */
static void A_hr_Ctl_dInputChanged(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 894 "../hrCtl.st"
			(pVar->autoMode) = 0;
			seq_pvPut(ssId, 6 /* autoMode */, 0);
			sprintf((pVar->seqMsg2), "Set to Manual Mode");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
# line 899 "../hrCtl.st"
pVar->D1 = hrCtl_calc2dSpacing(pVar, 1, pVar->A1, pVar->H1, pVar->K1, pVar->L1);
# line 899 "../hrCtl.st"
			seq_pvPut(ssId, 14 /* D1 */, 0);
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
# line 902 "../hrCtl.st"
pVar->D2 = hrCtl_calc2dSpacing(pVar, 2, pVar->A2, pVar->H2, pVar->K2, pVar->L2);
# line 902 "../hrCtl.st"
			seq_pvPut(ssId, 19 /* D2 */, 0);
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
# line 910 "../hrCtl.st"
			if ((pVar->useSetMode))
			{
				(pVar->lambdaHi) = (pVar->D1) * sin((pVar->theta1Hi) * (pVar->D2R));
				(pVar->lambdaLo) = (pVar->D1) * sin((pVar->theta1Lo) * (pVar->D2R));
				(pVar->EHi) = (pVar->hc) / (pVar->lambdaLo);
				(pVar->ELo) = (pVar->hc) / (pVar->lambdaHi);
			}
# line 915 "../hrCtl.st"
			else
			{
				(pVar->lambdaHi) = 0;
				(pVar->lambdaLo) = 0;
				(pVar->EHi) = 0;
				(pVar->ELo) = 0;
			}
# line 916 "../hrCtl.st"
			seq_pvPut(ssId, 26 /* lambdaLo */, 0);
			seq_pvPut(ssId, 25 /* lambdaHi */, 0);
			seq_pvPut(ssId, 21 /* EHi */, 0);
			seq_pvPut(ssId, 22 /* ELo */, 0);
			seq_pvPut(ssId, 3 /* opAlert */, 0);
		}
		return;
	}
}
/* Code for state "tweak" in state set "hr_Ctl" */

/* Delay function for state "tweak" in state set "hr_Ctl" */
static void D_hr_Ctl_tweak(SS_ID ssId, struct UserVar *pVar)
{
# line 949 "../hrCtl.st"
# line 974 "../hrCtl.st"
# line 998 "../hrCtl.st"
# line 1022 "../hrCtl.st"
# line 1038 "../hrCtl.st"
}

/* Event function for state "tweak" in state set "hr_Ctl" */
static long E_hr_Ctl_tweak(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 949 "../hrCtl.st"
	if ((pVar->ETweakInc) || (pVar->ETweakDec))
	{
		*pNextState = 9;
		*pTransNum = 0;
		return TRUE;
	}
# line 974 "../hrCtl.st"
	if ((pVar->LTweakInc) || (pVar->LTweakDec))
	{
		*pNextState = 10;
		*pTransNum = 1;
		return TRUE;
	}
# line 998 "../hrCtl.st"
	if ((pVar->th1TweakInc) || (pVar->th1TweakDec))
	{
		*pNextState = 11;
		*pTransNum = 2;
		return TRUE;
	}
# line 1022 "../hrCtl.st"
	if ((pVar->th2TweakInc) || (pVar->th2TweakDec))
	{
		*pNextState = 11;
		*pTransNum = 3;
		return TRUE;
	}
# line 1038 "../hrCtl.st"
	if ((pVar->worldTweakInc) || (pVar->worldTweakDec))
	{
		*pNextState = 12;
		*pTransNum = 4;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "tweak" in state set "hr_Ctl" */
static void A_hr_Ctl_tweak(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 930 "../hrCtl.st"
			if ((pVar->ETweakInc))
			{
				(pVar->temp) = (pVar->E) + (pVar->ETweakVal) / 1000000;
			}
# line 932 "../hrCtl.st"
			else
			{
				(pVar->temp) = (pVar->E) - (pVar->ETweakVal) / 1000000;
			}
# line 938 "../hrCtl.st"
			if ((pVar->useSetMode) || (((pVar->temp) >= (pVar->ELo)) && ((pVar->temp) <= (pVar->EHi))))
			{
				(pVar->E) = (pVar->temp);
				seq_pvPut(ssId, 20 /* E */, 0);
				sprintf((pVar->seqMsg1), "Tweaked E");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			}
# line 943 "../hrCtl.st"
			else
			{
				sprintf((pVar->seqMsg1), "Exceeds Limit");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				(pVar->opAlert) = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
			}
# line 946 "../hrCtl.st"
			if ((pVar->ETweakInc))
			{
				(pVar->ETweakInc) = 0;
				seq_pvPut(ssId, 80 /* ETweakInc */, 0);
			}
# line 948 "../hrCtl.st"
			else
			{
				(pVar->ETweakDec) = 0;
				seq_pvPut(ssId, 81 /* ETweakDec */, 0);
			}
		}
		return;
	case 1:
		{
# line 955 "../hrCtl.st"
			if ((pVar->LTweakInc))
			{
				(pVar->temp) = (pVar->lambda) + (pVar->LTweakVal) / 100000;
			}
# line 957 "../hrCtl.st"
			else
			{
				(pVar->temp) = (pVar->lambda) - (pVar->LTweakVal) / 100000;
			}
# line 963 "../hrCtl.st"
			if ((pVar->useSetMode) || (((pVar->temp) >= (pVar->lambdaLo)) && ((pVar->temp) <= (pVar->lambdaHi))))
			{
				(pVar->lambda) = (pVar->temp);
				seq_pvPut(ssId, 24 /* lambda */, 0);
				sprintf((pVar->seqMsg1), "Tweaked Lambda");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			}
# line 968 "../hrCtl.st"
			else
			{
				sprintf((pVar->seqMsg1), "Exceeds Limit");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				(pVar->opAlert) = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
			}
# line 971 "../hrCtl.st"
			if ((pVar->LTweakInc))
			{
				(pVar->LTweakInc) = 0;
				seq_pvPut(ssId, 83 /* LTweakInc */, 0);
			}
# line 973 "../hrCtl.st"
			else
			{
				(pVar->LTweakDec) = 0;
				seq_pvPut(ssId, 84 /* LTweakDec */, 0);
			}
		}
		return;
	case 2:
		{
# line 979 "../hrCtl.st"
			if ((pVar->th1TweakInc))
			{
				(pVar->temp) = (pVar->theta1) + (pVar->th1TweakVal);
			}
# line 981 "../hrCtl.st"
			else
			{
				(pVar->temp) = (pVar->theta1) - (pVar->th1TweakVal);
			}
# line 987 "../hrCtl.st"
			if ((pVar->useSetMode) || (((pVar->temp) >= (pVar->theta1Lo)) && ((pVar->temp) <= (pVar->theta1Hi))))
			{
				(pVar->theta1) = (pVar->temp);
				seq_pvPut(ssId, 28 /* theta1 */, 0);
				sprintf((pVar->seqMsg1), "Tweaked Theta 1");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			}
# line 992 "../hrCtl.st"
			else
			{
				sprintf((pVar->seqMsg1), "Exceeds Limit");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				(pVar->opAlert) = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
			}
# line 995 "../hrCtl.st"
			if ((pVar->th1TweakInc))
			{
				(pVar->th1TweakInc) = 0;
				seq_pvPut(ssId, 86 /* th1TweakInc */, 0);
			}
# line 997 "../hrCtl.st"
			else
			{
				(pVar->th1TweakDec) = 0;
				seq_pvPut(ssId, 87 /* th1TweakDec */, 0);
			}
		}
		return;
	case 3:
		{
# line 1003 "../hrCtl.st"
			if ((pVar->th2TweakInc))
			{
				(pVar->temp) = (pVar->theta2) + (pVar->th2TweakVal);
			}
# line 1005 "../hrCtl.st"
			else
			{
				(pVar->temp) = (pVar->theta2) - (pVar->th2TweakVal);
			}
# line 1011 "../hrCtl.st"
			if ((pVar->useSetMode) || (((pVar->temp) >= (pVar->theta2Lo)) && ((pVar->temp) <= (pVar->theta2Hi))))
			{
				(pVar->theta2) = (pVar->temp);
				seq_pvPut(ssId, 37 /* theta2 */, 0);
				sprintf((pVar->seqMsg1), "Tweaked Theta 2");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			}
# line 1016 "../hrCtl.st"
			else
			{
				sprintf((pVar->seqMsg1), "Exceeds Limit");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				(pVar->opAlert) = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
			}
# line 1019 "../hrCtl.st"
			if ((pVar->th2TweakInc))
			{
				(pVar->th2TweakInc) = 0;
				seq_pvPut(ssId, 89 /* th2TweakInc */, 0);
			}
# line 1021 "../hrCtl.st"
			else
			{
				(pVar->th2TweakDec) = 0;
				seq_pvPut(ssId, 90 /* th2TweakDec */, 0);
			}
		}
		return;
	case 4:
		{
# line 1027 "../hrCtl.st"
			if ((pVar->worldTweakInc))
			{
				(pVar->worldOff) = (pVar->worldOff) + (pVar->worldTweakVal);
			}
# line 1029 "../hrCtl.st"
			else
			{
				(pVar->worldOff) = (pVar->worldOff) - (pVar->worldTweakVal);
			}
# line 1030 "../hrCtl.st"
			seq_pvPut(ssId, 74 /* worldOff */, 0);
			sprintf((pVar->seqMsg1), "Tweaked world");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
# line 1035 "../hrCtl.st"
			if ((pVar->worldTweakInc))
			{
				(pVar->worldTweakInc) = 0;
				seq_pvPut(ssId, 76 /* worldTweakInc */, 0);
			}
# line 1037 "../hrCtl.st"
			else
			{
				(pVar->worldTweakDec) = 0;
				seq_pvPut(ssId, 77 /* worldTweakDec */, 0);
			}
		}
		return;
	}
}
/* Code for state "eChanged" in state set "hr_Ctl" */

/* Delay function for state "eChanged" in state set "hr_Ctl" */
static void D_hr_Ctl_eChanged(SS_ID ssId, struct UserVar *pVar)
{
# line 1048 "../hrCtl.st"
}

/* Event function for state "eChanged" in state set "hr_Ctl" */
static long E_hr_Ctl_eChanged(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1048 "../hrCtl.st"
	if (TRUE)
	{
		*pNextState = 10;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "eChanged" in state set "hr_Ctl" */
static void A_hr_Ctl_eChanged(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1046 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:eChanged\n");
# line 1046 "../hrCtl.st"
			(pVar->lambda) = (pVar->hc) / (pVar->E);
			seq_pvPut(ssId, 24 /* lambda */, 0);
		}
		return;
	}
}
/* Code for state "lChanged" in state set "hr_Ctl" */

/* Delay function for state "lChanged" in state set "hr_Ctl" */
static void D_hr_Ctl_lChanged(SS_ID ssId, struct UserVar *pVar)
{
# line 1059 "../hrCtl.st"
# line 1067 "../hrCtl.st"
# line 1074 "../hrCtl.st"
# line 1083 "../hrCtl.st"
# line 1094 "../hrCtl.st"
}

/* Event function for state "lChanged" in state set "hr_Ctl" */
static long E_hr_Ctl_lChanged(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1059 "../hrCtl.st"
	if ((pVar->lambda) > (pVar->D1))
	{
		*pNextState = 11;
		*pTransNum = 0;
		return TRUE;
	}
# line 1067 "../hrCtl.st"
	if (((pVar->lambda) > (pVar->D2)) && ((pVar->opMode) != 0))
	{
		*pNextState = 11;
		*pTransNum = 1;
		return TRUE;
	}
# line 1074 "../hrCtl.st"
	if ((pVar->opMode) == 0)
	{
		*pNextState = 11;
		*pTransNum = 2;
		return TRUE;
	}
# line 1083 "../hrCtl.st"
	if ((pVar->opMode) == 1)
	{
		*pNextState = 11;
		*pTransNum = 3;
		return TRUE;
	}
# line 1094 "../hrCtl.st"
	if ((pVar->opMode) == 2)
	{
		*pNextState = 11;
		*pTransNum = 4;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "lChanged" in state set "hr_Ctl" */
static void A_hr_Ctl_lChanged(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1055 "../hrCtl.st"
			sprintf((pVar->seqMsg1), "Wavelength > 2d spacing of crystal 1.");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			(pVar->opAlert) = 1;
			seq_pvPut(ssId, 3 /* opAlert */, 0);
		}
		return;
	case 1:
		{
# line 1063 "../hrCtl.st"
			sprintf((pVar->seqMsg1), "Wavelength > 2d spacing of crystal 2.");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			(pVar->opAlert) = 1;
			seq_pvPut(ssId, 3 /* opAlert */, 0);
		}
		return;
	case 2:
		{
# line 1071 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:lChanged, opMode 0\n");
# line 1072 "../hrCtl.st"
			(pVar->theta1) = asin((pVar->lambda) / (pVar->D1)) * (pVar->R2D);
			seq_pvPut(ssId, 28 /* theta1 */, 0);
		}
		return;
	case 3:
		{
# line 1078 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:lChanged, opMode 1\n");
# line 1079 "../hrCtl.st"
			(pVar->theta1) = asin((pVar->lambda) / (pVar->D1)) * (pVar->R2D);
			seq_pvPut(ssId, 28 /* theta1 */, 0);
			(pVar->theta2) = asin((pVar->lambda) / (pVar->D2)) * (pVar->R2D);
			seq_pvPut(ssId, 37 /* theta2 */, 0);
		}
		return;
	case 4:
		{
# line 1087 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:lChanged, opMode 1\n");
# line 1092 "../hrCtl.st"
			(pVar->theta2) = asin((pVar->lambda) / (pVar->D2)) * (pVar->R2D);
			seq_pvPut(ssId, 37 /* theta2 */, 0);
		}
		return;
	}
}
/* Code for state "thChanged" in state set "hr_Ctl" */

/* Delay function for state "thChanged" in state set "hr_Ctl" */
static void D_hr_Ctl_thChanged(SS_ID ssId, struct UserVar *pVar)
{
# line 1166 "../hrCtl.st"
}

/* Event function for state "thChanged" in state set "hr_Ctl" */
static long E_hr_Ctl_thChanged(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1166 "../hrCtl.st"
	if (TRUE)
	{
		*pNextState = 12;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "thChanged" in state set "hr_Ctl" */
static void A_hr_Ctl_thChanged(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1101 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:thChanged\n");
# line 1110 "../hrCtl.st"
			if (!(pVar->useSetMode) && (((pVar->theta1) <= (pVar->theta1Lo)) || ((pVar->theta1) >= (pVar->theta1Hi))))
			{
				sprintf((pVar->seqMsg1), "Theta 1 constrained to LIMIT");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				(pVar->opAlert) = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
				(pVar->autoMode) = 0;
				seq_pvPut(ssId, 6 /* autoMode */, 0);
				sprintf((pVar->seqMsg2), "Set to Manual Mode");
				seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			}
			if (!(pVar->useSetMode) && ((pVar->opMode) != 0) && (((pVar->theta2) <= (pVar->theta2Lo)) || ((pVar->theta2) >= (pVar->theta2Hi))))
			{
				sprintf((pVar->seqMsg1), "Theta 2 constrained to LIMIT");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				(pVar->opAlert) = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
				(pVar->autoMode) = 0;
				seq_pvPut(ssId, 6 /* autoMode */, 0);
				sprintf((pVar->seqMsg2), "Set to Manual Mode");
				seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			}
# line 1133 "../hrCtl.st"
			if (((pVar->opMode) == 0) || ((pVar->opMode) == 1))
			{
				(pVar->lambda) = (pVar->D1) * sin((pVar->theta1) * (pVar->D2R));
				(pVar->phi1) = (pVar->theta1);
				if ((pVar->opMode) == 1)
				{
					if ((pVar->Geom) == 0)
					{
						(pVar->phi2) = (pVar->phi1) + (pVar->theta1) + (pVar->theta2);
					}
					else
					{
						(pVar->phi2) = (pVar->theta2);
					}
				}
			}
# line 1145 "../hrCtl.st"
			else
			{
				(pVar->lambdaNom) = (pVar->D1) * sin((pVar->theta1) * (pVar->D2R));
				(pVar->theta2Nom) = asin((pVar->lambdaNom) / (pVar->D2)) * (pVar->R2D);
				(pVar->lambda) = (pVar->D2) * sin((pVar->theta2) * (pVar->D2R));
				(pVar->phi1) = (pVar->theta1);
				if ((pVar->Geom) == 0)
				{
					(pVar->phi2) = ((pVar->phi1) + (pVar->theta1) + (pVar->theta2Nom)) + (pVar->R2D) * ((pVar->lambda) - (pVar->lambdaNom)) * (1 / ((pVar->D1) * cos((pVar->theta1) * (pVar->D2R))) + 1 / ((pVar->D2) * cos((pVar->theta2Nom) * (pVar->D2R))));
				}
				else
				{
					(pVar->phi2) = (pVar->theta2Nom) + (pVar->R2D) * ((pVar->lambda) - (pVar->lambdaNom)) * (1 / ((pVar->D1) * cos((pVar->theta1) * (pVar->D2R))) + 1 / ((pVar->D2) * cos((pVar->theta2Nom) * (pVar->D2R))));
				}
			}
# line 1146 "../hrCtl.st"
			seq_pvPut(ssId, 32 /* phi1 */, 0);
			if ((pVar->opMode) != 0)
				seq_pvPut(ssId, 41 /* phi2 */, 0);
# line 1148 "../hrCtl.st"
			seq_pvPut(ssId, 24 /* lambda */, 0);
# line 1150 "../hrCtl.st"
			(pVar->E) = (pVar->hc) / (pVar->lambda);
			seq_pvPut(ssId, 20 /* E */, 0);
# line 1158 "../hrCtl.st"
hrCtl_calcReadback(pVar);
# line 1158 "../hrCtl.st"
			seq_pvPut(ssId, 36 /* phi1Rdbk */, 0);
			seq_pvPut(ssId, 31 /* theta1Rdbk */, 0);
# line 1163 "../hrCtl.st"
			if ((pVar->opMode) != 0)
			{
				seq_pvPut(ssId, 45 /* phi2Rdbk */, 0);
				seq_pvPut(ssId, 40 /* theta2Rdbk */, 0);
			}
# line 1164 "../hrCtl.st"
			seq_pvPut(ssId, 27 /* lambdaRdbk */, 0);
			seq_pvPut(ssId, 23 /* ERdbk */, 0);
		}
		return;
	}
}
/* Code for state "calcMovements" in state set "hr_Ctl" */

/* Delay function for state "calcMovements" in state set "hr_Ctl" */
static void D_hr_Ctl_calcMovements(SS_ID ssId, struct UserVar *pVar)
{
# line 1184 "../hrCtl.st"
# line 1227 "../hrCtl.st"
}

/* Event function for state "calcMovements" in state set "hr_Ctl" */
static long E_hr_Ctl_calcMovements(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1184 "../hrCtl.st"
	if ((pVar->useSetMode) == 0)
	{
		*pNextState = 13;
		*pTransNum = 0;
		return TRUE;
	}
# line 1227 "../hrCtl.st"
	if ((pVar->useSetMode) == 1)
	{
		*pNextState = 2;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "calcMovements" in state set "hr_Ctl" */
static void A_hr_Ctl_calcMovements(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1173 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:calcMovements, USE\n");
# line 1174 "../hrCtl.st"
			(pVar->phi1MotDesired) = ((pVar->phi1) - (pVar->phi1Off) - (pVar->worldOff) * (pVar->uR2D)) * (pVar->D2uR);
			seq_pvPut(ssId, 62 /* phi1MotDesired */, 0);
# line 1183 "../hrCtl.st"
			if ((pVar->opMode) != 0)
			{
				if ((pVar->Geom) == 0)
				{
					(pVar->phi2MotDesired) = ((pVar->phi2) - (pVar->phi2Off) - (pVar->worldOff) * (pVar->uR2D)) * (pVar->D2uR);
				}
				else
				{
					(pVar->phi2MotDesired) = ((pVar->phi2) - (pVar->phi2Off) + (pVar->worldOff) * (pVar->uR2D)) * (pVar->D2uR);
				}
				seq_pvPut(ssId, 63 /* phi2MotDesired */, 0);
			}
		}
		return;
	case 1:
		{
# line 1188 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:calcMovements, SET\n");
			(pVar->phi1Off) = (pVar->phi1) - (pVar->phi1MotRdbk) / (pVar->D2uR);
			seq_pvPut(ssId, 33 /* phi1Off */, 0);
# line 1202 "../hrCtl.st"
			if ((pVar->opMode) != 0)
			{
				(pVar->phi2Off) = (pVar->phi2) - (pVar->phi2MotRdbk) / (pVar->D2uR);
				seq_pvPut(ssId, 42 /* phi2Off */, 0);
			}
# line 1204 "../hrCtl.st"
			(pVar->phi1MotDesired) = ((pVar->phi1) - (pVar->phi1Off) - (pVar->worldOff) * (pVar->uR2D)) * (pVar->D2uR);
			seq_pvPut(ssId, 62 /* phi1MotDesired */, 0);
# line 1213 "../hrCtl.st"
			if ((pVar->opMode) != 0)
			{
				if ((pVar->Geom) == 0)
				{
					(pVar->phi2MotDesired) = ((pVar->phi2) - (pVar->phi2Off) - (pVar->worldOff) * (pVar->uR2D)) * (pVar->D2uR);
				}
				else
				{
					(pVar->phi2MotDesired) = ((pVar->phi2) - (pVar->phi2Off) + (pVar->worldOff) * (pVar->uR2D)) * (pVar->D2uR);
				}
				seq_pvPut(ssId, 63 /* phi2MotDesired */, 0);
			}
# line 1214 "../hrCtl.st"
			(pVar->putVals) = 0;
			seq_pvPut(ssId, 5 /* putVals */, 0);
# line 1219 "../hrCtl.st"
hrCtl_calcReadback(pVar);
# line 1219 "../hrCtl.st"
			seq_pvPut(ssId, 36 /* phi1Rdbk */, 0);
			seq_pvPut(ssId, 31 /* theta1Rdbk */, 0);
# line 1224 "../hrCtl.st"
			if ((pVar->opMode) != 0)
			{
				seq_pvPut(ssId, 45 /* phi2Rdbk */, 0);
				seq_pvPut(ssId, 40 /* theta2Rdbk */, 0);
			}
# line 1225 "../hrCtl.st"
			seq_pvPut(ssId, 27 /* lambdaRdbk */, 0);
			seq_pvPut(ssId, 23 /* ERdbk */, 0);
		}
		return;
	}
}
/* Code for state "moveHR" in state set "hr_Ctl" */

/* Delay function for state "moveHR" in state set "hr_Ctl" */
static void D_hr_Ctl_moveHR(SS_ID ssId, struct UserVar *pVar)
{
# line 1251 "../hrCtl.st"
# line 1255 "../hrCtl.st"
}

/* Event function for state "moveHR" in state set "hr_Ctl" */
static long E_hr_Ctl_moveHR(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1251 "../hrCtl.st"
	if ((pVar->autoMode) || (pVar->putVals))
	{
		*pNextState = 14;
		*pTransNum = 0;
		return TRUE;
	}
# line 1255 "../hrCtl.st"
	if (1)
	{
		*pNextState = 2;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "moveHR" in state set "hr_Ctl" */
static void A_hr_Ctl_moveHR(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1234 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:moveHR: putting to motors\n");
# line 1234 "../hrCtl.st"
			(pVar->phi1MotCmd) = (pVar->phi1MotDesired);
# line 1237 "../hrCtl.st"
			if ((pVar->opMode) == 0)
			{
				seq_pvPut(ssId, 68 /* phi1MotCmd */, 0);
			}
# line 1247 "../hrCtl.st"
			else
			if (((pVar->opMode) == 1) || ((pVar->opMode) == 2))
			{
				(pVar->phi2MotCmd) = (pVar->phi2MotDesired);
				seq_pvPut(ssId, 69 /* phi2MotCmd */, 0);
				seq_pvPut(ssId, 68 /* phi1MotCmd */, 0);
			}
# line 1248 "../hrCtl.st"
			(pVar->putVals) = 0;
			seq_pvPut(ssId, 5 /* putVals */, 0);
			(pVar->causedMove) = 1;
		}
		return;
	case 1:
		{
# line 1255 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:moveHR: NOT putting to motors\n");
		}
		return;
	}
}
/* Code for state "updateRdbk" in state set "hr_Ctl" */

/* Delay function for state "updateRdbk" in state set "hr_Ctl" */
static void D_hr_Ctl_updateRdbk(SS_ID ssId, struct UserVar *pVar)
{
# line 1273 "../hrCtl.st"
# line 1291 "../hrCtl.st"
# line 1307 "../hrCtl.st"
	seq_delayInit(ssId, 0, (.1));
}

/* Event function for state "updateRdbk" in state set "hr_Ctl" */
static long E_hr_Ctl_updateRdbk(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1273 "../hrCtl.st"
	if ((pVar->phi1Dmov) && (pVar->phi2Dmov))
	{
		*pNextState = 15;
		*pTransNum = 0;
		return TRUE;
	}
# line 1291 "../hrCtl.st"
	if ((pVar->phi1Hls) || (pVar->phi1Lls) || (pVar->phi2Hls) || (pVar->phi2Lls))
	{
		*pNextState = 18;
		*pTransNum = 1;
		return TRUE;
	}
# line 1307 "../hrCtl.st"
	if ((!(pVar->phi1Dmov) || !(pVar->phi2Dmov)) && seq_delay(ssId, 0))
	{
		*pNextState = 14;
		*pTransNum = 2;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "updateRdbk" in state set "hr_Ctl" */
static void A_hr_Ctl_updateRdbk(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1262 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:updateRdbk: motors stopped\n");
hrCtl_calcReadback(pVar);
# line 1264 "../hrCtl.st"
			seq_pvPut(ssId, 36 /* phi1Rdbk */, 0);
			seq_pvPut(ssId, 31 /* theta1Rdbk */, 0);
# line 1269 "../hrCtl.st"
			if ((pVar->opMode) != 0)
			{
				seq_pvPut(ssId, 45 /* phi2Rdbk */, 0);
				seq_pvPut(ssId, 40 /* theta2Rdbk */, 0);
			}
# line 1270 "../hrCtl.st"
			seq_pvPut(ssId, 27 /* lambdaRdbk */, 0);
			seq_pvPut(ssId, 23 /* ERdbk */, 0);
			(pVar->hr_Done) = 1;
		}
		return;
	case 1:
		{
# line 1280 "../hrCtl.st"
			if ((pVar->phi1Hls) || (pVar->phi1Lls))
			{
				sprintf((pVar->seqMsg1), "Theta 1 motor hit a limit switch !");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			}
# line 1284 "../hrCtl.st"
			if (((pVar->opMode) != 0) && ((pVar->phi2Hls) || (pVar->phi2Lls)))
			{
				sprintf((pVar->seqMsg1), "Theta 2 motor hit a limit switch !");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			}
# line 1285 "../hrCtl.st"
			(pVar->opAlert) = 1;
			seq_pvPut(ssId, 3 /* opAlert */, 0);
			(pVar->autoMode) = 0;
			seq_pvPut(ssId, 6 /* autoMode */, 0);
			sprintf((pVar->seqMsg2), "Setting to Manual Mode");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
		}
		return;
	case 2:
		{
# line 1294 "../hrCtl.st"
			(pVar->hr_Moving) = 1;
			seq_pvPut(ssId, 9 /* hr_Moving */, 0);
# line 1299 "../hrCtl.st"
hrCtl_calcReadback(pVar);
# line 1299 "../hrCtl.st"
			seq_pvPut(ssId, 36 /* phi1Rdbk */, 0);
			seq_pvPut(ssId, 31 /* theta1Rdbk */, 0);
# line 1304 "../hrCtl.st"
			if ((pVar->opMode) != 0)
			{
				seq_pvPut(ssId, 45 /* phi2Rdbk */, 0);
				seq_pvPut(ssId, 40 /* theta2Rdbk */, 0);
			}
# line 1305 "../hrCtl.st"
			seq_pvPut(ssId, 27 /* lambdaRdbk */, 0);
			seq_pvPut(ssId, 23 /* ERdbk */, 0);
		}
		return;
	}
}
/* Code for state "motorsStopped" in state set "hr_Ctl" */

/* Delay function for state "motorsStopped" in state set "hr_Ctl" */
static void D_hr_Ctl_motorsStopped(SS_ID ssId, struct UserVar *pVar)
{
# line 1315 "../hrCtl.st"
# line 1338 "../hrCtl.st"
}

/* Event function for state "motorsStopped" in state set "hr_Ctl" */
static long E_hr_Ctl_motorsStopped(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1315 "../hrCtl.st"
	if ((pVar->causedMove))
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 1338 "../hrCtl.st"
	if (!(pVar->causedMove))
	{
		*pNextState = 2;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "motorsStopped" in state set "hr_Ctl" */
static void A_hr_Ctl_motorsStopped(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1314 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:motorsStopped (we started them)\n");
# line 1314 "../hrCtl.st"
			(pVar->causedMove) = 0;
		}
		return;
	case 1:
		{
# line 1319 "../hrCtl.st"
			if ((pVar->hrCtlDebug))
printf("hrCtl:motorsStopped (we didn't start them)\n");
# line 1327 "../hrCtl.st"
			if ((pVar->initSeqDone))
			{
				(pVar->opAlert) = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
				sprintf((pVar->seqMsg1), "Somebody moved one of my motors.");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				(pVar->autoMode) = 0;
				seq_pvPut(ssId, 6 /* autoMode */, 0);
			}
# line 1330 "../hrCtl.st"
hrCtl_calcReadback(pVar);
# line 1330 "../hrCtl.st"
			seq_pvPut(ssId, 36 /* phi1Rdbk */, 0);
			seq_pvPut(ssId, 31 /* theta1Rdbk */, 0);
# line 1335 "../hrCtl.st"
			if ((pVar->opMode) != 0)
			{
				seq_pvPut(ssId, 45 /* phi2Rdbk */, 0);
				seq_pvPut(ssId, 40 /* theta2Rdbk */, 0);
			}
# line 1336 "../hrCtl.st"
			seq_pvPut(ssId, 27 /* lambdaRdbk */, 0);
			seq_pvPut(ssId, 23 /* ERdbk */, 0);
		}
		return;
	}
}
/* Code for state "chkMotorLimits" in state set "hr_Ctl" */

/* Delay function for state "chkMotorLimits" in state set "hr_Ctl" */
static void D_hr_Ctl_chkMotorLimits(SS_ID ssId, struct UserVar *pVar)
{
# line 1372 "../hrCtl.st"
}

/* Event function for state "chkMotorLimits" in state set "hr_Ctl" */
static long E_hr_Ctl_chkMotorLimits(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1372 "../hrCtl.st"
	if (TRUE)
	{
		*pNextState = 17;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "chkMotorLimits" in state set "hr_Ctl" */
static void A_hr_Ctl_chkMotorLimits(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1344 "../hrCtl.st"
			(pVar->opAlert) = 0;
# line 1347 "../hrCtl.st"
hrCtl_calcReadback(pVar);
# line 1347 "../hrCtl.st"
			seq_pvPut(ssId, 36 /* phi1Rdbk */, 0);
			seq_pvPut(ssId, 31 /* theta1Rdbk */, 0);
			seq_pvPut(ssId, 45 /* phi2Rdbk */, 0);
			seq_pvPut(ssId, 40 /* theta2Rdbk */, 0);
			seq_pvPut(ssId, 27 /* lambdaRdbk */, 0);
			seq_pvPut(ssId, 23 /* ERdbk */, 0);
# line 1358 "../hrCtl.st"
			if (((pVar->phi1MotRdbk) < (pVar->phi1MotLoLim)) || ((pVar->phi1MotRdbk) > (pVar->phi1MotHiLim)))
			{
				sprintf((pVar->seqMsg1), "Theta 1 motor is outside its limits");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				(pVar->opAlert) = 1;
			}
			if (((pVar->opMode) != 0) && (((pVar->phi2MotRdbk) < (pVar->phi2MotLoLim)) || ((pVar->phi2MotRdbk) > (pVar->phi2MotHiLim))))
			{
				sprintf((pVar->seqMsg1), "Theta 2 motor is outside its limits");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				(pVar->opAlert) = 1;
			}
# line 1371 "../hrCtl.st"
			if ((pVar->opAlert))
			{
				seq_pvPut(ssId, 3 /* opAlert */, 0);
				(pVar->autoMode) = 0;
				seq_pvPut(ssId, 6 /* autoMode */, 0);
				sprintf((pVar->seqMsg2), "Set to manual mode");
				seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			}
		}
		return;
	}
}
/* Code for state "chkMotorLimitsExit" in state set "hr_Ctl" */

/* Delay function for state "chkMotorLimitsExit" in state set "hr_Ctl" */
static void D_hr_Ctl_chkMotorLimitsExit(SS_ID ssId, struct UserVar *pVar)
{
# line 1381 "../hrCtl.st"
# line 1387 "../hrCtl.st"
	seq_delayInit(ssId, 0, (2.0));
}

/* Event function for state "chkMotorLimitsExit" in state set "hr_Ctl" */
static long E_hr_Ctl_chkMotorLimitsExit(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1381 "../hrCtl.st"
	if (!(pVar->opAlert))
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 1387 "../hrCtl.st"
	if ((pVar->opAlert) && seq_delay(ssId, 0))
	{
		*pNextState = 16;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "chkMotorLimitsExit" in state set "hr_Ctl" */
static void A_hr_Ctl_chkMotorLimitsExit(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1378 "../hrCtl.st"
			seq_pvPut(ssId, 3 /* opAlert */, 0);
			sprintf((pVar->seqMsg1), "All motors within limits");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
		}
		return;
	case 1:
		{
# line 1384 "../hrCtl.st"
			seq_pvPut(ssId, 3 /* opAlert */, 0);
			sprintf((pVar->seqMsg2), "Modify motor positions and/or limits");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
		}
		return;
	}
}
/* Code for state "stopHR" in state set "hr_Ctl" */

/* Delay function for state "stopHR" in state set "hr_Ctl" */
static void D_hr_Ctl_stopHR(SS_ID ssId, struct UserVar *pVar)
{
# line 1395 "../hrCtl.st"
}

/* Event function for state "stopHR" in state set "hr_Ctl" */
static long E_hr_Ctl_stopHR(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1395 "../hrCtl.st"
	if (TRUE)
	{
		*pNextState = 19;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "stopHR" in state set "hr_Ctl" */
static void A_hr_Ctl_stopHR(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1393 "../hrCtl.st"
			seq_pvPut(ssId, 54 /* phi1MotStop */, 0);
			if ((pVar->opMode) != 0)
				seq_pvPut(ssId, 55 /* phi2MotStop */, 0);
		}
		return;
	}
}
/* Code for state "stoppedHRWait" in state set "hr_Ctl" */

/* Delay function for state "stoppedHRWait" in state set "hr_Ctl" */
static void D_hr_Ctl_stoppedHRWait(SS_ID ssId, struct UserVar *pVar)
{
# line 1401 "../hrCtl.st"
	seq_delayInit(ssId, 0, (1.0));
}

/* Event function for state "stoppedHRWait" in state set "hr_Ctl" */
static long E_hr_Ctl_stoppedHRWait(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1401 "../hrCtl.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 14;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "stoppedHRWait" in state set "hr_Ctl" */
static void A_hr_Ctl_stoppedHRWait(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
		}
		return;
	}
}
/* Code for state "init" in state set "updatePsuedo" */

/* Delay function for state "init" in state set "updatePsuedo" */
static void D_updatePsuedo_init(SS_ID ssId, struct UserVar *pVar)
{
# line 1416 "../hrCtl.st"
}

/* Event function for state "init" in state set "updatePsuedo" */
static long E_updatePsuedo_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1416 "../hrCtl.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "updatePsuedo" */
static void A_updatePsuedo_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
		}
		return;
	}
}
/* Code for state "update" in state set "updatePsuedo" */

/* Delay function for state "update" in state set "updatePsuedo" */
static void D_updatePsuedo_update(SS_ID ssId, struct UserVar *pVar)
{
# line 1424 "../hrCtl.st"
# line 1429 "../hrCtl.st"
# line 1435 "../hrCtl.st"
# line 1441 "../hrCtl.st"
# line 1447 "../hrCtl.st"
# line 1453 "../hrCtl.st"
}

/* Event function for state "update" in state set "updatePsuedo" */
static long E_updatePsuedo_update(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1424 "../hrCtl.st"
	if (seq_efTestAndClear(ssId, phi1MotRdbkPseudo_mon))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 1429 "../hrCtl.st"
	if (seq_efTestAndClear(ssId, phi2MotRdbkPseudo_mon))
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
# line 1435 "../hrCtl.st"
	if (seq_efTest(ssId, phi1MotCmdMon_mon))
	{
		*pNextState = 1;
		*pTransNum = 2;
		return TRUE;
	}
# line 1441 "../hrCtl.st"
	if (seq_efTest(ssId, phi2MotCmdMon_mon))
	{
		*pNextState = 1;
		*pTransNum = 3;
		return TRUE;
	}
# line 1447 "../hrCtl.st"
	if (seq_efTest(ssId, phi1Dmov_mon))
	{
		*pNextState = 1;
		*pTransNum = 4;
		return TRUE;
	}
# line 1453 "../hrCtl.st"
	if (seq_efTest(ssId, phi2Dmov_mon))
	{
		*pNextState = 1;
		*pTransNum = 5;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "update" in state set "updatePsuedo" */
static void A_updatePsuedo_update(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1422 "../hrCtl.st"
			(pVar->phi1MotRdbkEcho) = (pVar->phi1MotRdbk);
			seq_pvPut(ssId, 50 /* phi1MotRdbkEcho */, 0);
		}
		return;
	case 1:
		{
# line 1427 "../hrCtl.st"
			(pVar->phi2MotRdbkEcho) = (pVar->phi2MotRdbk);
			seq_pvPut(ssId, 51 /* phi2MotRdbkEcho */, 0);
		}
		return;
	case 2:
		{
# line 1432 "../hrCtl.st"
			(pVar->phi1MotCmdEcho) = (pVar->phi1MotCmdMon);
			seq_pvPut(ssId, 48 /* phi1MotCmdEcho */, 0);
			seq_efClear(ssId, phi1MotCmdMon_mon);
		}
		return;
	case 3:
		{
# line 1438 "../hrCtl.st"
			(pVar->phi2MotCmdEcho) = (pVar->phi2MotCmdMon);
			seq_pvPut(ssId, 49 /* phi2MotCmdEcho */, 0);
			seq_efClear(ssId, phi2MotCmdMon_mon);
		}
		return;
	case 4:
		{
# line 1444 "../hrCtl.st"
			(pVar->phi1DmovEcho) = (pVar->phi1Dmov);
			seq_pvPut(ssId, 52 /* phi1DmovEcho */, 0);
			seq_efClear(ssId, phi1Dmov_mon);
		}
		return;
	case 5:
		{
# line 1450 "../hrCtl.st"
			(pVar->phi2DmovEcho) = (pVar->phi2Dmov);
			seq_pvPut(ssId, 53 /* phi2DmovEcho */, 0);
			seq_efClear(ssId, phi2Dmov_mon);
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
  {"{P}HR{N}CtlDebug", (void *)OFFSET(struct UserVar, hrCtlDebug), "hrCtlDebug", 
    "short", 1, 34, 0, 1, 0, 0, 0},

  {"{P}HR{N}_SeqMsg1SI", (void *)OFFSET(struct UserVar, seqMsg1[0]), "seqMsg1", 
    "string", 1, 35, 0, 0, 0, 0, 0},

  {"{P}HR{N}_SeqMsg2SI", (void *)OFFSET(struct UserVar, seqMsg2[0]), "seqMsg2", 
    "string", 1, 36, 0, 0, 0, 0, 0},

  {"{P}HR{N}_AlertBO", (void *)OFFSET(struct UserVar, opAlert), "opAlert", 
    "short", 1, 37, 0, 0, 0, 0, 0},

  {"{P}HR{N}_OperAckBO", (void *)OFFSET(struct UserVar, opAck), "opAck", 
    "short", 1, 38, 0, 1, 0, 0, 0},

  {"{P}HR{N}_PutBO", (void *)OFFSET(struct UserVar, putVals), "putVals", 
    "short", 1, 39, 0, 1, 0, 0, 0},

  {"{P}HR{N}_ModeBO", (void *)OFFSET(struct UserVar, autoMode), "autoMode", 
    "short", 1, 40, 0, 1, 0, 0, 0},

  {"{P}HR{N}_Mode2MO", (void *)OFFSET(struct UserVar, opMode), "opMode", 
    "short", 1, 41, 1, 1, 0, 0, 0},

  {"{P}HR{N}_GeomMO", (void *)OFFSET(struct UserVar, Geom), "Geom", 
    "short", 1, 42, 2, 1, 0, 0, 0},

  {"{P}HR{N}_Moving", (void *)OFFSET(struct UserVar, hr_Moving), "hr_Moving", 
    "short", 1, 43, 3, 1, 0, 0, 0},

  {"{P}HR{N}_H1AO", (void *)OFFSET(struct UserVar, H1), "H1", 
    "double", 1, 44, 4, 1, 0, 0, 0},

  {"{P}HR{N}_K1AO", (void *)OFFSET(struct UserVar, K1), "K1", 
    "double", 1, 45, 5, 1, 0, 0, 0},

  {"{P}HR{N}_L1AO", (void *)OFFSET(struct UserVar, L1), "L1", 
    "double", 1, 46, 6, 1, 0, 0, 0},

  {"{P}HR{N}_A1AO", (void *)OFFSET(struct UserVar, A1), "A1", 
    "double", 1, 47, 7, 1, 0, 0, 0},

  {"{P}HR{N}_2d1AO", (void *)OFFSET(struct UserVar, D1), "D1", 
    "double", 1, 48, 0, 0, 0, 0, 0},

  {"{P}HR{N}_H2AO", (void *)OFFSET(struct UserVar, H2), "H2", 
    "double", 1, 49, 8, 1, 0, 0, 0},

  {"{P}HR{N}_K2AO", (void *)OFFSET(struct UserVar, K2), "K2", 
    "double", 1, 50, 9, 1, 0, 0, 0},

  {"{P}HR{N}_L2AO", (void *)OFFSET(struct UserVar, L2), "L2", 
    "double", 1, 51, 10, 1, 0, 0, 0},

  {"{P}HR{N}_A2AO", (void *)OFFSET(struct UserVar, A2), "A2", 
    "double", 1, 52, 11, 1, 0, 0, 0},

  {"{P}HR{N}_2d2AO", (void *)OFFSET(struct UserVar, D2), "D2", 
    "double", 1, 53, 0, 0, 0, 0, 0},

  {"{P}HR{N}_EAO", (void *)OFFSET(struct UserVar, E), "E", 
    "double", 1, 54, 12, 1, 0, 0, 0},

  {"{P}HR{N}_EAO.DRVH", (void *)OFFSET(struct UserVar, EHi), "EHi", 
    "double", 1, 55, 0, 0, 0, 0, 0},

  {"{P}HR{N}_EAO.DRVL", (void *)OFFSET(struct UserVar, ELo), "ELo", 
    "double", 1, 56, 0, 0, 0, 0, 0},

  {"{P}HR{N}_ERdbkAO", (void *)OFFSET(struct UserVar, ERdbk), "ERdbk", 
    "double", 1, 57, 0, 0, 0, 0, 0},

  {"{P}HR{N}_LambdaAO", (void *)OFFSET(struct UserVar, lambda), "lambda", 
    "double", 1, 58, 13, 1, 0, 0, 0},

  {"{P}HR{N}_LambdaAO.DRVH", (void *)OFFSET(struct UserVar, lambdaHi), "lambdaHi", 
    "double", 1, 59, 0, 0, 0, 0, 0},

  {"{P}HR{N}_LambdaAO.DRVL", (void *)OFFSET(struct UserVar, lambdaLo), "lambdaLo", 
    "double", 1, 60, 0, 0, 0, 0, 0},

  {"{P}HR{N}_LambdaRdbkAO", (void *)OFFSET(struct UserVar, lambdaRdbk), "lambdaRdbk", 
    "double", 1, 61, 0, 0, 0, 0, 0},

  {"{P}HR{N}_Theta1AO", (void *)OFFSET(struct UserVar, theta1), "theta1", 
    "double", 1, 62, 14, 1, 0, 0, 0},

  {"{P}HR{N}_Theta1AO.DRVH", (void *)OFFSET(struct UserVar, theta1Hi), "theta1Hi", 
    "double", 1, 63, 0, 0, 0, 0, 0},

  {"{P}HR{N}_Theta1AO.DRVL", (void *)OFFSET(struct UserVar, theta1Lo), "theta1Lo", 
    "double", 1, 64, 0, 0, 0, 0, 0},

  {"{P}HR{N}_Theta1RdbkAO", (void *)OFFSET(struct UserVar, theta1Rdbk), "theta1Rdbk", 
    "double", 1, 65, 0, 0, 0, 0, 0},

  {"{P}HR{N}_phi1AO", (void *)OFFSET(struct UserVar, phi1), "phi1", 
    "double", 1, 66, 15, 1, 0, 0, 0},

  {"{P}HR{N}_phi1OffAO", (void *)OFFSET(struct UserVar, phi1Off), "phi1Off", 
    "double", 1, 67, 16, 1, 0, 0, 0},

  {"{P}HR{N}_phi1AO.DRVH", (void *)OFFSET(struct UserVar, phi1Hi), "phi1Hi", 
    "double", 1, 68, 0, 0, 0, 0, 0},

  {"{P}HR{N}_phi1AO.DRVL", (void *)OFFSET(struct UserVar, phi1Lo), "phi1Lo", 
    "double", 1, 69, 0, 0, 0, 0, 0},

  {"{P}HR{N}_phi1RdbkAO", (void *)OFFSET(struct UserVar, phi1Rdbk), "phi1Rdbk", 
    "double", 1, 70, 0, 0, 0, 0, 0},

  {"{P}HR{N}_Theta2AO", (void *)OFFSET(struct UserVar, theta2), "theta2", 
    "double", 1, 71, 17, 1, 0, 0, 0},

  {"{P}HR{N}_Theta2AO.DRVH", (void *)OFFSET(struct UserVar, theta2Hi), "theta2Hi", 
    "double", 1, 72, 0, 0, 0, 0, 0},

  {"{P}HR{N}_Theta2AO.DRVL", (void *)OFFSET(struct UserVar, theta2Lo), "theta2Lo", 
    "double", 1, 73, 0, 0, 0, 0, 0},

  {"{P}HR{N}_Theta2RdbkAO", (void *)OFFSET(struct UserVar, theta2Rdbk), "theta2Rdbk", 
    "double", 1, 74, 0, 0, 0, 0, 0},

  {"{P}HR{N}_phi2AO", (void *)OFFSET(struct UserVar, phi2), "phi2", 
    "double", 1, 75, 18, 1, 0, 0, 0},

  {"{P}HR{N}_phi2OffAO", (void *)OFFSET(struct UserVar, phi2Off), "phi2Off", 
    "double", 1, 76, 19, 1, 0, 0, 0},

  {"{P}HR{N}_phi2AO.DRVH", (void *)OFFSET(struct UserVar, phi2Hi), "phi2Hi", 
    "double", 1, 77, 0, 0, 0, 0, 0},

  {"{P}HR{N}_phi2AO.DRVL", (void *)OFFSET(struct UserVar, phi2Lo), "phi2Lo", 
    "double", 1, 78, 0, 0, 0, 0, 0},

  {"{P}HR{N}_phi2RdbkAO", (void *)OFFSET(struct UserVar, phi2Rdbk), "phi2Rdbk", 
    "double", 1, 79, 0, 0, 0, 0, 0},

  {"{P}HR{N}_phi1PvSI", (void *)OFFSET(struct UserVar, phi1MotName[0]), "phi1MotName", 
    "string", 1, 80, 0, 0, 0, 0, 0},

  {"{P}HR{N}_phi2PvSI", (void *)OFFSET(struct UserVar, phi2MotName[0]), "phi2MotName", 
    "string", 1, 81, 0, 0, 0, 0, 0},

  {"{P}HR{N}_phi1CmdAO", (void *)OFFSET(struct UserVar, phi1MotCmdEcho), "phi1MotCmdEcho", 
    "double", 1, 82, 0, 0, 0, 0, 0},

  {"{P}HR{N}_phi2CmdAO", (void *)OFFSET(struct UserVar, phi2MotCmdEcho), "phi2MotCmdEcho", 
    "double", 1, 83, 0, 0, 0, 0, 0},

  {"{P}HR{N}_phi1RdbkAI", (void *)OFFSET(struct UserVar, phi1MotRdbkEcho), "phi1MotRdbkEcho", 
    "double", 1, 84, 0, 0, 0, 0, 0},

  {"{P}HR{N}_phi2RdbkAI", (void *)OFFSET(struct UserVar, phi2MotRdbkEcho), "phi2MotRdbkEcho", 
    "double", 1, 85, 0, 0, 0, 0, 0},

  {"{P}HR{N}_phi1DmovBI", (void *)OFFSET(struct UserVar, phi1DmovEcho), "phi1DmovEcho", 
    "short", 1, 86, 0, 0, 0, 0, 0},

  {"{P}HR{N}_phi2DmovBI", (void *)OFFSET(struct UserVar, phi2DmovEcho), "phi2DmovEcho", 
    "short", 1, 87, 0, 0, 0, 0, 0},

  {"{P}{M_PHI1}.STOP", (void *)OFFSET(struct UserVar, phi1MotStop), "phi1MotStop", 
    "short", 1, 88, 0, 0, 0, 0, 0},

  {"{P}{M_PHI2}.STOP", (void *)OFFSET(struct UserVar, phi2MotStop), "phi2MotStop", 
    "short", 1, 89, 0, 0, 0, 0, 0},

  {"{P}{M_PHI1}.DMOV", (void *)OFFSET(struct UserVar, phi1Dmov), "phi1Dmov", 
    "short", 1, 90, 20, 1, 0, 0, 0},

  {"{P}{M_PHI2}.DMOV", (void *)OFFSET(struct UserVar, phi2Dmov), "phi2Dmov", 
    "short", 1, 91, 21, 1, 0, 0, 0},

  {"{P}{M_PHI1}.HLS", (void *)OFFSET(struct UserVar, phi1Hls), "phi1Hls", 
    "short", 1, 92, 0, 1, 0, 0, 0},

  {"{P}{M_PHI1}.LLS", (void *)OFFSET(struct UserVar, phi1Lls), "phi1Lls", 
    "short", 1, 93, 0, 1, 0, 0, 0},

  {"{P}{M_PHI2}.HLS", (void *)OFFSET(struct UserVar, phi2Hls), "phi2Hls", 
    "short", 1, 94, 0, 1, 0, 0, 0},

  {"{P}{M_PHI2}.LLS", (void *)OFFSET(struct UserVar, phi2Lls), "phi2Lls", 
    "short", 1, 95, 0, 1, 0, 0, 0},

  {"{P}HR{N}_phi1SetAO", (void *)OFFSET(struct UserVar, phi1MotDesired), "phi1MotDesired", 
    "double", 1, 96, 0, 1, 0, 0, 0},

  {"{P}HR{N}_phi2SetAO", (void *)OFFSET(struct UserVar, phi2MotDesired), "phi2MotDesired", 
    "double", 1, 97, 0, 1, 0, 0, 0},

  {"{P}{M_PHI1}.HLM", (void *)OFFSET(struct UserVar, phi1MotHiLim), "phi1MotHiLim", 
    "double", 1, 98, 22, 1, 0, 0, 0},

  {"{P}{M_PHI1}.LLM", (void *)OFFSET(struct UserVar, phi1MotLoLim), "phi1MotLoLim", 
    "double", 1, 99, 23, 1, 0, 0, 0},

  {"{P}{M_PHI2}.HLM", (void *)OFFSET(struct UserVar, phi2MotHiLim), "phi2MotHiLim", 
    "double", 1, 100, 24, 1, 0, 0, 0},

  {"{P}{M_PHI2}.LLM", (void *)OFFSET(struct UserVar, phi2MotLoLim), "phi2MotLoLim", 
    "double", 1, 101, 25, 1, 0, 0, 0},

  {"{P}{M_PHI1}", (void *)OFFSET(struct UserVar, phi1MotCmd), "phi1MotCmd", 
    "double", 1, 102, 0, 0, 0, 0, 0},

  {"{P}{M_PHI2}", (void *)OFFSET(struct UserVar, phi2MotCmd), "phi2MotCmd", 
    "double", 1, 103, 0, 0, 0, 0, 0},

  {"{P}{M_PHI1}", (void *)OFFSET(struct UserVar, phi1MotCmdMon), "phi1MotCmdMon", 
    "double", 1, 104, 26, 1, 0, 0, 0},

  {"{P}{M_PHI2}", (void *)OFFSET(struct UserVar, phi2MotCmdMon), "phi2MotCmdMon", 
    "double", 1, 105, 27, 1, 0, 0, 0},

  {"{P}{M_PHI1}.RBV", (void *)OFFSET(struct UserVar, phi1MotRdbk), "phi1MotRdbk", 
    "double", 1, 106, 28, 1, 0, 0, 0},

  {"{P}{M_PHI2}.RBV", (void *)OFFSET(struct UserVar, phi2MotRdbk), "phi2MotRdbk", 
    "double", 1, 107, 30, 1, 0, 0, 0},

  {"{P}HR{N}_worldOffAO", (void *)OFFSET(struct UserVar, worldOff), "worldOff", 
    "double", 1, 108, 32, 1, 0, 0, 0},

  {"{P}HR{N}_worldTweakAI", (void *)OFFSET(struct UserVar, worldTweakVal), "worldTweakVal", 
    "double", 1, 109, 0, 1, 0, 0, 0},

  {"{P}HR{N}_worldIncBO", (void *)OFFSET(struct UserVar, worldTweakInc), "worldTweakInc", 
    "short", 1, 110, 0, 1, 0, 0, 0},

  {"{P}HR{N}_worldDecBO", (void *)OFFSET(struct UserVar, worldTweakDec), "worldTweakDec", 
    "short", 1, 111, 0, 1, 0, 0, 0},

  {"{P}HR{N}_UseSetBO", (void *)OFFSET(struct UserVar, useSetMode), "useSetMode", 
    "short", 1, 112, 33, 1, 0, 0, 0},

  {"{P}HR{N}_ETweakAI", (void *)OFFSET(struct UserVar, ETweakVal), "ETweakVal", 
    "double", 1, 113, 0, 1, 0, 0, 0},

  {"{P}HR{N}_EIncBO", (void *)OFFSET(struct UserVar, ETweakInc), "ETweakInc", 
    "short", 1, 114, 0, 1, 0, 0, 0},

  {"{P}HR{N}_EDecBO", (void *)OFFSET(struct UserVar, ETweakDec), "ETweakDec", 
    "short", 1, 115, 0, 1, 0, 0, 0},

  {"{P}HR{N}_LambdaTweakAI", (void *)OFFSET(struct UserVar, LTweakVal), "LTweakVal", 
    "double", 1, 116, 0, 1, 0, 0, 0},

  {"{P}HR{N}_LambdaIncBO", (void *)OFFSET(struct UserVar, LTweakInc), "LTweakInc", 
    "short", 1, 117, 0, 1, 0, 0, 0},

  {"{P}HR{N}_LambdaDecBO", (void *)OFFSET(struct UserVar, LTweakDec), "LTweakDec", 
    "short", 1, 118, 0, 1, 0, 0, 0},

  {"{P}HR{N}_Theta1TweakAI", (void *)OFFSET(struct UserVar, th1TweakVal), "th1TweakVal", 
    "double", 1, 119, 0, 1, 0, 0, 0},

  {"{P}HR{N}_Theta1IncBO", (void *)OFFSET(struct UserVar, th1TweakInc), "th1TweakInc", 
    "short", 1, 120, 0, 1, 0, 0, 0},

  {"{P}HR{N}_Theta1DecBO", (void *)OFFSET(struct UserVar, th1TweakDec), "th1TweakDec", 
    "short", 1, 121, 0, 1, 0, 0, 0},

  {"{P}HR{N}_Theta2TweakAI", (void *)OFFSET(struct UserVar, th2TweakVal), "th2TweakVal", 
    "double", 1, 122, 0, 1, 0, 0, 0},

  {"{P}HR{N}_Theta2IncBO", (void *)OFFSET(struct UserVar, th2TweakInc), "th2TweakInc", 
    "short", 1, 123, 0, 1, 0, 0, 0},

  {"{P}HR{N}_Theta2DecBO", (void *)OFFSET(struct UserVar, th2TweakDec), "th2TweakDec", 
    "short", 1, 124, 0, 1, 0, 0, 0},

};

/* Event masks for state set hr_Ctl */
	/* Event mask for state init: */
static bitMask	EM_hr_Ctl_init[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state initSequence: */
static bitMask	EM_hr_Ctl_initSequence[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state waitForCmndEnter: */
static bitMask	EM_hr_Ctl_waitForCmndEnter[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state waitForCmnd: */
static bitMask	EM_hr_Ctl_waitForCmnd[] = {
	0xa3cb7ffe,
	0x000000c3,
	0x00000000,
	0x1b6dc000,
};
	/* Event mask for state checkAutoMode: */
static bitMask	EM_hr_Ctl_checkAutoMode[] = {
	0x00000000,
	0x00000100,
	0x00000000,
	0x00000000,
};
	/* Event mask for state phiLimits: */
static bitMask	EM_hr_Ctl_phiLimits[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state checkDone: */
static bitMask	EM_hr_Ctl_checkDone[] = {
	0x00000000,
	0x00000000,
	0x0c000000,
	0x00000000,
};
	/* Event mask for state dInputChanged: */
static bitMask	EM_hr_Ctl_dInputChanged[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state tweak: */
static bitMask	EM_hr_Ctl_tweak[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x1b6cc000,
};
	/* Event mask for state eChanged: */
static bitMask	EM_hr_Ctl_eChanged[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state lChanged: */
static bitMask	EM_hr_Ctl_lChanged[] = {
	0x00000000,
	0x04210200,
	0x00000000,
	0x00000000,
};
	/* Event mask for state thChanged: */
static bitMask	EM_hr_Ctl_thChanged[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state calcMovements: */
static bitMask	EM_hr_Ctl_calcMovements[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00010000,
};
	/* Event mask for state moveHR: */
static bitMask	EM_hr_Ctl_moveHR[] = {
	0x00000000,
	0x00000180,
	0x00000000,
	0x00000000,
};
	/* Event mask for state updateRdbk: */
static bitMask	EM_hr_Ctl_updateRdbk[] = {
	0x00000000,
	0x00000000,
	0xfc000000,
	0x00000000,
};
	/* Event mask for state motorsStopped: */
static bitMask	EM_hr_Ctl_motorsStopped[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state chkMotorLimits: */
static bitMask	EM_hr_Ctl_chkMotorLimits[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state chkMotorLimitsExit: */
static bitMask	EM_hr_Ctl_chkMotorLimitsExit[] = {
	0x00000000,
	0x00000020,
	0x00000000,
	0x00000000,
};
	/* Event mask for state stopHR: */
static bitMask	EM_hr_Ctl_stopHR[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state stoppedHRWait: */
static bitMask	EM_hr_Ctl_stoppedHRWait[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_hr_Ctl[] = {
	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_init,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_init,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_init,
	/* state options */   (0)},

	/* State "initSequence" */ {
	/* state name */       "initSequence",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_initSequence,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_initSequence,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_initSequence,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_initSequence,
	/* state options */   (0)},

	/* State "waitForCmndEnter" */ {
	/* state name */       "waitForCmndEnter",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_waitForCmndEnter,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_waitForCmndEnter,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_waitForCmndEnter,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_waitForCmndEnter,
	/* state options */   (0)},

	/* State "waitForCmnd" */ {
	/* state name */       "waitForCmnd",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_waitForCmnd,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_waitForCmnd,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_waitForCmnd,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_waitForCmnd,
	/* state options */   (0)},

	/* State "checkAutoMode" */ {
	/* state name */       "checkAutoMode",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_checkAutoMode,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_checkAutoMode,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_checkAutoMode,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_checkAutoMode,
	/* state options */   (0)},

	/* State "phiLimits" */ {
	/* state name */       "phiLimits",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_phiLimits,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_phiLimits,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_phiLimits,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_phiLimits,
	/* state options */   (0)},

	/* State "checkDone" */ {
	/* state name */       "checkDone",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_checkDone,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_checkDone,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_checkDone,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_checkDone,
	/* state options */   (0)},

	/* State "dInputChanged" */ {
	/* state name */       "dInputChanged",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_dInputChanged,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_dInputChanged,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_dInputChanged,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_dInputChanged,
	/* state options */   (0)},

	/* State "tweak" */ {
	/* state name */       "tweak",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_tweak,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_tweak,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_tweak,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_tweak,
	/* state options */   (0)},

	/* State "eChanged" */ {
	/* state name */       "eChanged",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_eChanged,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_eChanged,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_eChanged,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_eChanged,
	/* state options */   (0)},

	/* State "lChanged" */ {
	/* state name */       "lChanged",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_lChanged,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_lChanged,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_lChanged,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_lChanged,
	/* state options */   (0)},

	/* State "thChanged" */ {
	/* state name */       "thChanged",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_thChanged,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_thChanged,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_thChanged,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_thChanged,
	/* state options */   (0)},

	/* State "calcMovements" */ {
	/* state name */       "calcMovements",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_calcMovements,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_calcMovements,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_calcMovements,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_calcMovements,
	/* state options */   (0)},

	/* State "moveHR" */ {
	/* state name */       "moveHR",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_moveHR,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_moveHR,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_moveHR,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_moveHR,
	/* state options */   (0)},

	/* State "updateRdbk" */ {
	/* state name */       "updateRdbk",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_updateRdbk,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_updateRdbk,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_updateRdbk,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_updateRdbk,
	/* state options */   (0)},

	/* State "motorsStopped" */ {
	/* state name */       "motorsStopped",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_motorsStopped,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_motorsStopped,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_motorsStopped,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_motorsStopped,
	/* state options */   (0)},

	/* State "chkMotorLimits" */ {
	/* state name */       "chkMotorLimits",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_chkMotorLimits,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_chkMotorLimits,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_chkMotorLimits,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_chkMotorLimits,
	/* state options */   (0)},

	/* State "chkMotorLimitsExit" */ {
	/* state name */       "chkMotorLimitsExit",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_chkMotorLimitsExit,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_chkMotorLimitsExit,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_chkMotorLimitsExit,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_chkMotorLimitsExit,
	/* state options */   (0)},

	/* State "stopHR" */ {
	/* state name */       "stopHR",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_stopHR,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_stopHR,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_stopHR,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_stopHR,
	/* state options */   (0)},

	/* State "stoppedHRWait" */ {
	/* state name */       "stoppedHRWait",
	/* action function */ (ACTION_FUNC) A_hr_Ctl_stoppedHRWait,
	/* event function */  (EVENT_FUNC) E_hr_Ctl_stoppedHRWait,
	/* delay function */   (DELAY_FUNC) D_hr_Ctl_stoppedHRWait,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_hr_Ctl_stoppedHRWait,
	/* state options */   (0)},


};

/* Event masks for state set updatePsuedo */
	/* Event mask for state init: */
static bitMask	EM_updatePsuedo_init[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state update: */
static bitMask	EM_updatePsuedo_update[] = {
	0xac300000,
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_updatePsuedo[] = {
	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_updatePsuedo_init,
	/* event function */  (EVENT_FUNC) E_updatePsuedo_init,
	/* delay function */   (DELAY_FUNC) D_updatePsuedo_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_updatePsuedo_init,
	/* state options */   (0)},

	/* State "update" */ {
	/* state name */       "update",
	/* action function */ (ACTION_FUNC) A_updatePsuedo_update,
	/* event function */  (EVENT_FUNC) E_updatePsuedo_update,
	/* delay function */   (DELAY_FUNC) D_updatePsuedo_update,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_updatePsuedo_update,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "hr_Ctl" */ {
	/* ss name */            "hr_Ctl",
	/* ptr to state block */ state_hr_Ctl,
	/* number of states */   20,
	/* error state */        -1},


	/* State set "updatePsuedo" */ {
	/* ss name */            "updatePsuedo",
	/* ptr to state block */ state_updatePsuedo,
	/* number of states */   2,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "P=tmm, N=1, M_PHI1=m9, M_PHI2=m10";

/* State Program table (global) */
struct seqProgram hrCtl = {
	/* magic number */       20000315,
	/* *name */              "hrCtl",
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
/* Global C code */
# line 1463 "../hrCtl.st"




double hrCtl_calc2dSpacing(struct UserVar *pVar, int xtal, double A, double H, double K, double L)
{
 double result;
 float avg;
 int nint;

 result = (2*A)/sqrt(H*H + K*K + L*L);


 pVar->opAlert = 0;
 if (((fmod(H,2)) != (fmod(K,2))) || ((fmod(H,2)) != (fmod(L,2))))
  pVar->opAlert = 1;
 avg = (H+K+L)/2.0;
 nint = (int)((avg)>0 ? (avg)+0.5 : (avg)-0.5);
 if ((fabs(avg-nint) <= .25) && (fmod(nint,2)))
  pVar->opAlert = 1;
 if (pVar->opAlert) {
  sprintf(pVar->seqMsg1, "Invalid HKL combination for crystal %1d", xtal);
 }
    return(result);
}

void hrCtl_calcReadback(struct UserVar *pVar)
{

 pVar->phi1Rdbk = pVar->phi1MotRdbk * pVar->uR2D +
  (pVar->phi1Off + pVar->worldOff*pVar->uR2D);
 pVar->theta1Rdbk = pVar->phi1Rdbk;

 switch (pVar->opMode) {
 case 0:
  pVar->lambdaRdbk = pVar->D1*sin(pVar->theta1Rdbk*pVar->D2R);
  break;

 case 1:
  if (pVar->Geom == 0) {
   pVar->phi2Rdbk = pVar->phi2MotRdbk * pVar->uR2D +
    (pVar->phi2Off + pVar->worldOff*pVar->uR2D);
  } else {
   pVar->phi2Rdbk = pVar->phi2MotRdbk * pVar->uR2D +
    (pVar->phi2Off - pVar->worldOff*pVar->uR2D);
  }
  pVar->lambdaRdbk = pVar->D1*sin(pVar->theta1Rdbk*pVar->D2R);
  break;

 case 2:
  if (pVar->Geom == 0) {
   pVar->phi2Rdbk = pVar->phi2MotRdbk * pVar->uR2D +
    (pVar->phi2Off + pVar->worldOff*pVar->uR2D);
  } else {
   pVar->phi2Rdbk = pVar->phi2MotRdbk * pVar->uR2D +
    (pVar->phi2Off - pVar->worldOff*pVar->uR2D);
  }
  pVar->theta2Nom = asin(pVar->D1*sin(pVar->theta1Rdbk*pVar->D2R)/pVar->D2) * pVar->R2D;
  if (pVar->Geom == 0) {
   pVar->lambdaRdbk = pVar->D1*sin(pVar->theta1Rdbk*pVar->D2R) +
    pVar->D2R * (pVar->phi2Rdbk - pVar->phi1Rdbk - pVar->theta1Rdbk - pVar->theta2Nom) /
    ( 1/(pVar->D1*cos(pVar->theta1Rdbk*pVar->D2R)) + 1/(pVar->D2*cos(pVar->theta2Nom*pVar->D2R)) );
  } else {
   pVar->lambdaRdbk = pVar->D1*sin(pVar->theta1Rdbk*pVar->D2R) +
    pVar->D2R * (pVar->phi2Rdbk - pVar->theta2Nom) /
    ( 1/(pVar->D1*cos(pVar->theta1Rdbk*pVar->D2R)) + 1/(pVar->D2*cos(pVar->theta2Nom*pVar->D2R)) );
  }
  break;
 }
 pVar->theta1Rdbk = asin(pVar->lambdaRdbk/pVar->D1) * pVar->R2D;
 if (pVar->opMode != 0) pVar->theta2Rdbk = asin(pVar->lambdaRdbk/pVar->D2) * pVar->R2D;
 pVar->ERdbk = pVar->hc/pVar->lambdaRdbk;

 return;
}


#include "epicsExport.h"


/* Register sequencer commands and program */

void hrCtlRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&hrCtl);
}
epicsExportRegistrar(hrCtlRegistrar);

