/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: ml_monoCtl.i */

/* Event flags */
#define Order_mon	1
#define D_mon	2
#define E_mon	3
#define lambda_mon	4
#define theta_mon	5
#define thetaDmov_mon	6
#define theta2Dmov_mon	7
#define zDmov_mon	8
#define thetaMotHiLim_mon	9
#define thetaMotLoLim_mon	10
#define theta2MotHiLim_mon	11
#define theta2MotLoLim_mon	12
#define thetaMotCmdMon_mon	13
#define theta2MotCmdMon_mon	14
#define zMotCmdMon_mon	15
#define thetaMotVel_mon	16
#define theta2MotVel_mon	17
#define zMotVel_mon	18
#define thetaMotRdbk_mon	19
#define thetaMotRdbkPseudo_mon	20
#define theta2MotRdbk_mon	21
#define theta2MotRdbkPseudo_mon	22
#define zMotRdbk_mon	23
#define yMotRdbk_mon	24
#define useSetMode_mon	25
#define thetaMotSet_mon	26
#define theta2MotSet_mon	27
#define zMotSet_mon	28
#define yOffset_mon	29

/* Program "ml_monoCtl" */
#include "seqCom.h"

#define NUM_SS 3
#define NUM_CHANNELS 89
#define NUM_EVENTS 29
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT FALSE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT FALSE

extern struct seqProgram ml_monoCtl;

/* Variable declarations */
static short	ml_monoCtlDebug;
static char	seqMsg1[MAX_STRING_SIZE];
static char	seqMsg2[MAX_STRING_SIZE];
static short	opAlert;
static short	opAck;
static short	putVals;
static short	autoMode;
static short	ccMode;
static short	ml_monoMoving;
static short	ml_monoDone;
static double	Order;
static double	D;
static double	TwoD;
static double	E;
static double	EHi;
static double	ELo;
static double	ERdbk;
static double	lambda;
static double	lambdaHi;
static double	lambdaLo;
static double	lambdaRdbk;
static double	theta;
static double	thetaHi;
static double	thetaLo;
static double	thetaRdbk;
static char	thetaMotName[MAX_STRING_SIZE];
static char	theta2MotName[MAX_STRING_SIZE];
static char	zMotName[MAX_STRING_SIZE];
static char	yMotName[MAX_STRING_SIZE];
static double	thetaMotCmdEcho;
static double	theta2MotCmdEcho;
static double	zMotCmdEcho;
static double	thetaMotRdbkEcho;
static double	theta2MotRdbkEcho;
static double	zMotRdbkEcho;
static double	thetaMotVelEcho;
static double	theta2MotVelEcho;
static double	zMotVelEcho;
static short	thetaDmovEcho;
static short	theta2DmovEcho;
static short	zDmovEcho;
static short	thetaMotStop;
static short	theta2MotStop;
static short	zStop;
static short	thetaDmov;
static short	theta2Dmov;
static short	zDmov;
static short	thetaHls;
static short	thetaLls;
static short	theta2Hls;
static short	theta2Lls;
static short	zHls;
static short	zLls;
static double	thetaMotDesired;
static double	theta2MotDesired;
static double	zMotDesired;
static double	thetaMotHiLim;
static double	thetaMotLoLim;
static double	theta2MotHiLim;
static double	theta2MotLoLim;
static double	zMotHiLim;
static double	zMotLoLim;
static double	thetaMotCmd;
static double	theta2MotCmd;
static double	zMotCmd;
static double	thetaMotCmdMon;
static double	theta2MotCmdMon;
static double	zMotCmdMon;
static double	thTime;
static double	oldThSpeed;
static double	newThSpeed;
static double	thSpeed;
static double	oldTh2Speed;
static double	newTh2Speed;
static double	th2Speed;
static double	zTime;
static double	oldZSpeed;
static double	newZSpeed;
static double	zSpeed;
static double	thetaMotRdbk;
static double	theta2MotRdbk;
static double	zMotRdbk;
static double	yMotRdbk;
static short	useSetMode;
static short	thetaMotSet;
static short	theta2MotSet;
static short	zMotSet;
static double	ETweakVal;
static short	ETweakInc;
static short	ETweakDec;
static double	LTweakVal;
static short	LTweakInc;
static short	LTweakDec;
static double	thTweakVal;
static short	thTweakInc;
static short	thTweakDec;
static double	yOffset;
static double	yOffsetHi;
static double	yOffsetLo;
static short	geom;
static short	causedMove;
static short	initSeq;
static short	initSeqDone;
static double	hc;
static double	radConv;
static double	temp;

/* Not used (avoids compilation warnings) */
struct UserVar {
	int	dummy;
};

/* C code definitions */
# line 446 "../ml_monoCtl.st"
#include <string.h>
# line 447 "../ml_monoCtl.st"
#include <math.h>
# line 449 "../ml_monoCtl.st"
double ml_monoCtl_calc2dSpacing();
# line 450 "../ml_monoCtl.st"
char *ml_monoCtl_pmacP;
# line 451 "../ml_monoCtl.st"
char *ml_monoCtl_pmacM;
# line 452 "../ml_monoCtl.st"
char *ml_monoCtl_pmacGEOM;
# line 453 "../ml_monoCtl.st"
char *ml_monoCtl_pmacY_OFF;

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "init" in state set "ml_monoCtl" */

/* Delay function for state "init" in state set "ml_monoCtl" */
static void D_ml_monoCtl_init(SS_ID ssId, struct UserVar *pVar)
{
# line 470 "../ml_monoCtl.st"
}

/* Event function for state "init" in state set "ml_monoCtl" */
static long E_ml_monoCtl_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 470 "../ml_monoCtl.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "ml_monoCtl" */
static void A_ml_monoCtl_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 466 "../ml_monoCtl.st"
			initSeq = 0;
			hc = 12.3984244;
			radConv = 57.2958;
		}
		return;
	}
}
/* Code for state "initSequence" in state set "ml_monoCtl" */

/* Delay function for state "initSequence" in state set "ml_monoCtl" */
static void D_ml_monoCtl_initSequence(SS_ID ssId, struct UserVar *pVar)
{
# line 539 "../ml_monoCtl.st"
# line 544 "../ml_monoCtl.st"
# line 549 "../ml_monoCtl.st"
# line 560 "../ml_monoCtl.st"
	seq_delayInit(ssId, 0, (1.));
}

/* Event function for state "initSequence" in state set "ml_monoCtl" */
static long E_ml_monoCtl_initSequence(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 539 "../ml_monoCtl.st"
	if (initSeq == 0)
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
# line 544 "../ml_monoCtl.st"
	if (initSeq == 1)
	{
		*pNextState = 15;
		*pTransNum = 1;
		return TRUE;
	}
# line 549 "../ml_monoCtl.st"
	if (initSeq == 2)
	{
		*pNextState = 13;
		*pTransNum = 2;
		return TRUE;
	}
# line 560 "../ml_monoCtl.st"
	if ((initSeq == 3) && seq_delay(ssId, 0))
	{
		*pNextState = 9;
		*pTransNum = 3;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "initSequence" in state set "ml_monoCtl" */
static void A_ml_monoCtl_initSequence(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 476 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:initSequence:initSeq=0\n");
# line 476 "../ml_monoCtl.st"
			thetaMotStop = 1;
			theta2MotStop = 1;
			zStop = 1;
			putVals = 0;
			seq_pvPut(ssId, 5 /* putVals */, 0);
			autoMode = 0;
			seq_pvPut(ssId, 6 /* autoMode */, 0);
			opAck = 0;
			seq_pvPut(ssId, 4 /* opAck */, 0);
# line 487 "../ml_monoCtl.st"

  ml_monoCtl_pmacP = seq_macValueGet(ssId,"P");
  ml_monoCtl_pmacM = seq_macValueGet(ssId,"M_THETA");
  if (ml_monoCtl_pmacP == 0 || ml_monoCtl_pmacM == 0) {
   sprintf(thetaMotName,"None");
  } else {
   strcpy(thetaMotName, ml_monoCtl_pmacP);
   strcat(thetaMotName, ml_monoCtl_pmacM);
  }

  ml_monoCtl_pmacM = seq_macValueGet(ssId,"M_THETA2");
  if (ml_monoCtl_pmacP == 0 || ml_monoCtl_pmacM == 0) {
   sprintf(theta2MotName,"None");
  } else {
   strcpy(theta2MotName, ml_monoCtl_pmacP);
   strcat(theta2MotName, ml_monoCtl_pmacM);
  }

  ml_monoCtl_pmacM = seq_macValueGet(ssId,"M_Z");
  if (ml_monoCtl_pmacP == 0 || ml_monoCtl_pmacM == 0) {
   sprintf(zMotName,"None");
  } else {
   strcpy(zMotName, ml_monoCtl_pmacP);
   strcat(zMotName, ml_monoCtl_pmacM);
  }

  ml_monoCtl_pmacM = seq_macValueGet(ssId,"M_Y");
  if (ml_monoCtl_pmacP == 0 || ml_monoCtl_pmacM == 0) {
   sprintf(yMotName,"None");
  } else {
   strcpy(yMotName, ml_monoCtl_pmacP);
   strcat(yMotName, ml_monoCtl_pmacM);
  }


  ml_monoCtl_pmacY_OFF = seq_macValueGet(ssId,"Y_OFF");
  yOffset = atof(ml_monoCtl_pmacY_OFF);
  if ((yOffset < 1) || (yOffset > 60.))
   yOffset = 35.;

  ml_monoCtl_pmacGEOM = seq_macValueGet(ssId,"GEOM");
  geom = 1;

# line 529 "../ml_monoCtl.st"

# line 531 "../ml_monoCtl.st"
			seq_pvPut(ssId, 86 /* yOffset */, 0);
# line 533 "../ml_monoCtl.st"
			if (geom == 1)
			{
			}
# line 534 "../ml_monoCtl.st"
			seq_pvPut(ssId, 23 /* thetaMotName */, 0);
			seq_pvPut(ssId, 24 /* theta2MotName */, 0);
			seq_pvPut(ssId, 25 /* zMotName */, 0);
			seq_pvPut(ssId, 26 /* yMotName */, 0);
			initSeq = 1;
		}
		return;
	case 1:
		{
# line 543 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:initSequence:initSeq=1\n");
# line 543 "../ml_monoCtl.st"
			initSeq = 2;
		}
		return;
	case 2:
		{
# line 548 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:initSequence:initSeq=2\n");
# line 548 "../ml_monoCtl.st"
			initSeq = 3;
		}
		return;
	case 3:
		{
# line 553 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:initSequence:initSeq=3\n");
# line 553 "../ml_monoCtl.st"
			sprintf(seqMsg1, "ml_mono Control Ready");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			sprintf(seqMsg2, " ");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			initSeqDone = 1;
			theta = thetaMotRdbk;
			seq_pvPut(ssId, 19 /* theta */, 0);
		}
		return;
	}
}
/* Code for state "waitForCmndEnter" in state set "ml_monoCtl" */

/* Delay function for state "waitForCmndEnter" in state set "ml_monoCtl" */
static void D_ml_monoCtl_waitForCmndEnter(SS_ID ssId, struct UserVar *pVar)
{
# line 617 "../ml_monoCtl.st"
	seq_delayInit(ssId, 0, (.1));
# line 620 "../ml_monoCtl.st"
}

/* Event function for state "waitForCmndEnter" in state set "ml_monoCtl" */
static long E_ml_monoCtl_waitForCmndEnter(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 617 "../ml_monoCtl.st"
	if (initSeqDone && seq_delay(ssId, 0))
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
# line 620 "../ml_monoCtl.st"
	if (!initSeqDone)
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "waitForCmndEnter" in state set "ml_monoCtl" */
static void A_ml_monoCtl_waitForCmndEnter(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 566 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:waitForCmndEnter:initSeqDone\n");
# line 566 "../ml_monoCtl.st"
			seq_efClear(ssId, Order_mon);
			seq_efClear(ssId, D_mon);
			seq_efClear(ssId, E_mon);
			seq_efClear(ssId, lambda_mon);
			seq_efClear(ssId, theta_mon);
# line 572 "../ml_monoCtl.st"
			thetaMotRdbkEcho = thetaMotRdbk;
			seq_pvPut(ssId, 30 /* thetaMotRdbkEcho */, 0);
			theta2MotRdbkEcho = theta2MotRdbk;
			seq_pvPut(ssId, 31 /* theta2MotRdbkEcho */, 0);
			zMotRdbkEcho = zMotRdbk;
			seq_pvPut(ssId, 32 /* zMotRdbkEcho */, 0);
			yOffset = yMotRdbk;
			seq_pvPut(ssId, 86 /* yOffset */, 0);
			thetaMotVelEcho = thSpeed;
			seq_pvPut(ssId, 33 /* thetaMotVelEcho */, 0);
			theta2MotVelEcho = th2Speed;
			seq_pvPut(ssId, 34 /* theta2MotVelEcho */, 0);
			zMotVelEcho = zSpeed;
			seq_pvPut(ssId, 35 /* zMotVelEcho */, 0);
			thetaMotCmdEcho = thetaMotCmdMon;
			seq_pvPut(ssId, 27 /* thetaMotCmdEcho */, 0);
			theta2MotCmdEcho = theta2MotCmdMon;
			seq_pvPut(ssId, 28 /* theta2MotCmdEcho */, 0);
			zMotCmdEcho = zMotCmdMon;
			seq_pvPut(ssId, 29 /* zMotCmdEcho */, 0);
			thetaDmovEcho = thetaDmov;
			seq_pvPut(ssId, 36 /* thetaDmovEcho */, 0);
			theta2DmovEcho = theta2Dmov;
			seq_pvPut(ssId, 37 /* theta2DmovEcho */, 0);
			zDmovEcho = zDmov;
			seq_pvPut(ssId, 38 /* zDmovEcho */, 0);
# line 605 "../ml_monoCtl.st"
			if (autoMode || useSetMode)
			{
				thetaRdbk = thetaMotRdbk;
				seq_pvPut(ssId, 22 /* thetaRdbk */, 0);
				lambdaRdbk = TwoD * sin(thetaRdbk / radConv);
				seq_pvPut(ssId, 18 /* lambdaRdbk */, 0);
				ERdbk = hc / lambdaRdbk;
				seq_pvPut(ssId, 14 /* ERdbk */, 0);
			}
# line 606 "../ml_monoCtl.st"
			opAck = 0;
			seq_pvPut(ssId, 4 /* opAck */, 0);
# line 611 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
			{
printf("ml_monoCtl:waitForCmndEnter: ml_monoMoving=%d; ml_monoDone=%d\n",
 ml_monoMoving, ml_monoDone);
			}
			if (ml_monoMoving && ml_monoDone)
			{
				if (ml_monoCtlDebug)
printf("ml_monoCtl:waitForCmndEnter: asserting 'done'.\n");
				ml_monoMoving = 0;
				ml_monoDone = 0;
				seq_pvPut(ssId, 8 /* ml_monoMoving */, 0);
			}
		}
		return;
	case 1:
		{
		}
		return;
	}
}
/* Code for state "waitForCmnd" in state set "ml_monoCtl" */

/* Delay function for state "waitForCmnd" in state set "ml_monoCtl" */
static void D_ml_monoCtl_waitForCmnd(SS_ID ssId, struct UserVar *pVar)
{
# line 632 "../ml_monoCtl.st"
# line 642 "../ml_monoCtl.st"
# line 652 "../ml_monoCtl.st"
# line 659 "../ml_monoCtl.st"
# line 666 "../ml_monoCtl.st"
# line 670 "../ml_monoCtl.st"
# line 674 "../ml_monoCtl.st"
# line 678 "../ml_monoCtl.st"
# line 682 "../ml_monoCtl.st"
	seq_delayInit(ssId, 0, (.1));
# line 686 "../ml_monoCtl.st"
# line 696 "../ml_monoCtl.st"
# line 706 "../ml_monoCtl.st"
}

/* Event function for state "waitForCmnd" in state set "ml_monoCtl" */
static long E_ml_monoCtl_waitForCmnd(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 632 "../ml_monoCtl.st"
	if (seq_efTest(ssId, Order_mon) || seq_efTest(ssId, D_mon))
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
# line 642 "../ml_monoCtl.st"
	if (seq_efTest(ssId, thetaMotHiLim_mon) || seq_efTest(ssId, thetaMotLoLim_mon))
	{
		*pNextState = 6;
		*pTransNum = 1;
		return TRUE;
	}
# line 652 "../ml_monoCtl.st"
	if (seq_efTest(ssId, theta2MotHiLim_mon) || seq_efTest(ssId, theta2MotLoLim_mon))
	{
		*pNextState = 6;
		*pTransNum = 2;
		return TRUE;
	}
# line 659 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, thetaMotRdbk_mon))
	{
		*pNextState = 13;
		*pTransNum = 3;
		return TRUE;
	}
# line 666 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, theta2MotRdbk_mon))
	{
		*pNextState = 13;
		*pTransNum = 4;
		return TRUE;
	}
# line 670 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, E_mon))
	{
		*pNextState = 7;
		*pTransNum = 5;
		return TRUE;
	}
# line 674 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, lambda_mon))
	{
		*pNextState = 8;
		*pTransNum = 6;
		return TRUE;
	}
# line 678 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, theta_mon))
	{
		*pNextState = 9;
		*pTransNum = 7;
		return TRUE;
	}
# line 682 "../ml_monoCtl.st"
	if (putVals && seq_delay(ssId, 0))
	{
		*pNextState = 11;
		*pTransNum = 8;
		return TRUE;
	}
# line 686 "../ml_monoCtl.st"
	if (ETweakInc || ETweakDec || LTweakInc || LTweakDec || thTweakInc || thTweakDec)
	{
		*pNextState = 4;
		*pTransNum = 9;
		return TRUE;
	}
# line 696 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, yOffset_mon) && initSeqDone)
	{
		*pNextState = 10;
		*pTransNum = 10;
		return TRUE;
	}
# line 706 "../ml_monoCtl.st"
	if (opAck)
	{
		*pNextState = 2;
		*pTransNum = 11;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "waitForCmnd" in state set "ml_monoCtl" */
static void A_ml_monoCtl_waitForCmnd(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 626 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:waitForCmnd:Order/D_mon\n");
# line 626 "../ml_monoCtl.st"
			sprintf(seqMsg1, "Layer Spacing Changed");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			sprintf(seqMsg2, " ");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			seq_efClear(ssId, Order_mon);
			seq_efClear(ssId, D_mon);
		}
		return;
	case 1:
		{
# line 636 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:waitForCmnd:thetaMotXxLim_mon\n");
# line 636 "../ml_monoCtl.st"
			sprintf(seqMsg1, "Theta Motor Soft Limits Changed");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			sprintf(seqMsg2, " ");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			seq_efClear(ssId, thetaMotHiLim_mon);
			seq_efClear(ssId, thetaMotLoLim_mon);
		}
		return;
	case 2:
		{
# line 646 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:waitForCmnd:theta2MotXxLim_mon\n");
# line 646 "../ml_monoCtl.st"
			sprintf(seqMsg1, "Theta2 Motor Soft Limits Changed");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			sprintf(seqMsg2, " ");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			seq_efClear(ssId, theta2MotHiLim_mon);
			seq_efClear(ssId, theta2MotLoLim_mon);
		}
		return;
	case 3:
		{
# line 656 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:waitForCmnd:thetaMotRdbk_mon\n");
# line 656 "../ml_monoCtl.st"
			thetaMotRdbkEcho = thetaMotRdbk;
			seq_pvPut(ssId, 30 /* thetaMotRdbkEcho */, 0);
		}
		return;
	case 4:
		{
# line 663 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:waitForCmnd:theta2MotRdbk_mon\n");
# line 663 "../ml_monoCtl.st"
			theta2MotRdbkEcho = theta2MotRdbk;
			seq_pvPut(ssId, 31 /* theta2MotRdbkEcho */, 0);
		}
		return;
	case 5:
		{
# line 670 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:waitForCmnd:E_mon\n");
		}
		return;
	case 6:
		{
# line 674 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:waitForCmnd:lambda_mon\n");
		}
		return;
	case 7:
		{
# line 678 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:waitForCmnd:theta_mon\n");
		}
		return;
	case 8:
		{
# line 682 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:waitForCmnd:putVals\n");
		}
		return;
	case 9:
		{
# line 686 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:waitForCmnd:Tweak\n");
		}
		return;
	case 10:
		{
# line 690 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:waitForCmnd:yOffset_mon\n");
# line 690 "../ml_monoCtl.st"
			autoMode = 0;
			seq_pvPut(ssId, 6 /* autoMode */, 0);
			sprintf(seqMsg1, "y offset changed to %f", yOffset);
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			sprintf(seqMsg2, "Set to Manual Mode");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
		}
		return;
	case 11:
		{
# line 700 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:waitForCmnd:opAck\n");
# line 700 "../ml_monoCtl.st"
			opAlert = 0;
			seq_pvPut(ssId, 3 /* opAlert */, 0);
			sprintf(seqMsg1, " ");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			sprintf(seqMsg2, " ");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
		}
		return;
	}
}
/* Code for state "tweak" in state set "ml_monoCtl" */

/* Delay function for state "tweak" in state set "ml_monoCtl" */
static void D_ml_monoCtl_tweak(SS_ID ssId, struct UserVar *pVar)
{
# line 729 "../ml_monoCtl.st"
# line 750 "../ml_monoCtl.st"
# line 771 "../ml_monoCtl.st"
}

/* Event function for state "tweak" in state set "ml_monoCtl" */
static long E_ml_monoCtl_tweak(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 729 "../ml_monoCtl.st"
	if (ETweakInc || ETweakDec)
	{
		*pNextState = 7;
		*pTransNum = 0;
		return TRUE;
	}
# line 750 "../ml_monoCtl.st"
	if (LTweakInc || LTweakDec)
	{
		*pNextState = 8;
		*pTransNum = 1;
		return TRUE;
	}
# line 771 "../ml_monoCtl.st"
	if (thTweakInc || thTweakDec)
	{
		*pNextState = 9;
		*pTransNum = 2;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "tweak" in state set "ml_monoCtl" */
static void A_ml_monoCtl_tweak(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 712 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:tweak:E+\n");
# line 712 "../ml_monoCtl.st"
			temp = E + ETweakVal * (ETweakInc ? 1 : -1);
# line 716 "../ml_monoCtl.st"
			if ((temp >= ELo) && (temp <= EHi))
			{
				E = temp;
				seq_pvPut(ssId, 11 /* E */, 0);
			}
			else
			{
				sprintf(seqMsg1, "E tweak exceeds limit");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				opAlert = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
			}
# line 725 "../ml_monoCtl.st"
			if (ETweakInc)
			{
				ETweakInc = 0;
				seq_pvPut(ssId, 78 /* ETweakInc */, 0);
			}
# line 728 "../ml_monoCtl.st"
			else
			{
				ETweakDec = 0;
				seq_pvPut(ssId, 79 /* ETweakDec */, 0);
			}
		}
		return;
	case 1:
		{
			if (ml_monoCtlDebug)
printf("ml_monoCtl:tweak:L\n");
# line 733 "../ml_monoCtl.st"
			temp = lambda + LTweakVal * (LTweakInc ? 1 : -1);
# line 737 "../ml_monoCtl.st"
			if ((temp >= lambdaLo) && (temp <= lambdaHi))
			{
				lambda = temp;
				seq_pvPut(ssId, 15 /* lambda */, 0);
			}
			else
			{
				sprintf(seqMsg1, "Lambda tweak exceeds limit");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				opAlert = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
			}
# line 746 "../ml_monoCtl.st"
			if (LTweakInc)
			{
				LTweakInc = 0;
				seq_pvPut(ssId, 81 /* LTweakInc */, 0);
			}
# line 749 "../ml_monoCtl.st"
			else
			{
				LTweakDec = 0;
				seq_pvPut(ssId, 82 /* LTweakDec */, 0);
			}
		}
		return;
	case 2:
		{
			if (ml_monoCtlDebug)
printf("ml_monoCtl:tweak:th\n");
# line 754 "../ml_monoCtl.st"
			temp = theta + thTweakVal * (thTweakInc ? 1 : -1);
# line 758 "../ml_monoCtl.st"
			if ((temp >= thetaLo) && (temp <= thetaHi))
			{
				theta = temp;
				seq_pvPut(ssId, 19 /* theta */, 0);
			}
			else
			{
				sprintf(seqMsg1, "Theta tweak exceeds limit");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				opAlert = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
			}
# line 767 "../ml_monoCtl.st"
			if (thTweakInc)
			{
				thTweakInc = 0;
				seq_pvPut(ssId, 84 /* thTweakInc */, 0);
			}
# line 770 "../ml_monoCtl.st"
			else
			{
				thTweakDec = 0;
				seq_pvPut(ssId, 85 /* thTweakDec */, 0);
			}
		}
		return;
	}
}
/* Code for state "dInputChanged" in state set "ml_monoCtl" */

/* Delay function for state "dInputChanged" in state set "ml_monoCtl" */
static void D_ml_monoCtl_dInputChanged(SS_ID ssId, struct UserVar *pVar)
{
# line 792 "../ml_monoCtl.st"
}

/* Event function for state "dInputChanged" in state set "ml_monoCtl" */
static long E_ml_monoCtl_dInputChanged(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 792 "../ml_monoCtl.st"
	if (TRUE)
	{
		*pNextState = 9;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "dInputChanged" in state set "ml_monoCtl" */
static void A_ml_monoCtl_dInputChanged(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 777 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:dInputChanged\n");
# line 777 "../ml_monoCtl.st"
			autoMode = 0;
			seq_pvPut(ssId, 6 /* autoMode */, 0);
			sprintf(seqMsg2, "Set to Manual Mode");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			TwoD = ml_monoCtl_calc2dSpacing();
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			lambdaHi = TwoD * sin(thetaHi / radConv);
			seq_pvPut(ssId, 16 /* lambdaHi */, 0);
			lambdaLo = TwoD * sin(thetaLo / radConv);
			seq_pvPut(ssId, 17 /* lambdaLo */, 0);
			EHi = hc / lambdaLo;
			seq_pvPut(ssId, 12 /* EHi */, 0);
			ELo = hc / lambdaHi;
			seq_pvPut(ssId, 13 /* ELo */, 0);
			seq_pvPut(ssId, 3 /* opAlert */, 0);
		}
		return;
	}
}
/* Code for state "thetaLimits" in state set "ml_monoCtl" */

/* Delay function for state "thetaLimits" in state set "ml_monoCtl" */
static void D_ml_monoCtl_thetaLimits(SS_ID ssId, struct UserVar *pVar)
{
# line 820 "../ml_monoCtl.st"
}

/* Event function for state "thetaLimits" in state set "ml_monoCtl" */
static long E_ml_monoCtl_thetaLimits(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 820 "../ml_monoCtl.st"
	if (TRUE)
	{
		*pNextState = 12;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "thetaLimits" in state set "ml_monoCtl" */
static void A_ml_monoCtl_thetaLimits(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 798 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:thetaLimits\n");
# line 798 "../ml_monoCtl.st"
			thetaHi = thetaMotHiLim;
			thetaLo = thetaMotLoLim;
# line 804 "../ml_monoCtl.st"
			if (thetaHi > 89.0)
			{
				thetaHi = 89.0;
				sprintf(seqMsg2, "ThetaHiLim > 89 deg. (Using 89 deg.)");
				seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			}
# line 809 "../ml_monoCtl.st"
			if (thetaLo < 0.1)
			{
				thetaLo = 0.1;
				sprintf(seqMsg2, "ThetaLoLim < 0.1 deg. (Using 0.1 deg.)");
				seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			}
# line 810 "../ml_monoCtl.st"
			seq_pvPut(ssId, 20 /* thetaHi */, 0);
			seq_pvPut(ssId, 21 /* thetaLo */, 0);
			lambdaHi = TwoD * sin(thetaHi / radConv);
			seq_pvPut(ssId, 16 /* lambdaHi */, 0);
			lambdaLo = TwoD * sin(thetaLo / radConv);
			seq_pvPut(ssId, 17 /* lambdaLo */, 0);
			EHi = hc / lambdaLo;
			seq_pvPut(ssId, 12 /* EHi */, 0);
			ELo = hc / lambdaHi;
			seq_pvPut(ssId, 13 /* ELo */, 0);
		}
		return;
	}
}
/* Code for state "eChanged" in state set "ml_monoCtl" */

/* Delay function for state "eChanged" in state set "ml_monoCtl" */
static void D_ml_monoCtl_eChanged(SS_ID ssId, struct UserVar *pVar)
{
# line 828 "../ml_monoCtl.st"
}

/* Event function for state "eChanged" in state set "ml_monoCtl" */
static long E_ml_monoCtl_eChanged(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 828 "../ml_monoCtl.st"
	if (TRUE)
	{
		*pNextState = 8;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "eChanged" in state set "ml_monoCtl" */
static void A_ml_monoCtl_eChanged(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 826 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:eChanged\n");
# line 826 "../ml_monoCtl.st"
			lambda = hc / E;
			seq_pvPut(ssId, 15 /* lambda */, 0);
		}
		return;
	}
}
/* Code for state "lChanged" in state set "ml_monoCtl" */

/* Delay function for state "lChanged" in state set "ml_monoCtl" */
static void D_ml_monoCtl_lChanged(SS_ID ssId, struct UserVar *pVar)
{
# line 839 "../ml_monoCtl.st"
# line 845 "../ml_monoCtl.st"
}

/* Event function for state "lChanged" in state set "ml_monoCtl" */
static long E_ml_monoCtl_lChanged(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 839 "../ml_monoCtl.st"
	if (lambda > TwoD)
	{
		*pNextState = 9;
		*pTransNum = 0;
		return TRUE;
	}
# line 845 "../ml_monoCtl.st"
	if (TRUE)
	{
		*pNextState = 9;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "lChanged" in state set "ml_monoCtl" */
static void A_ml_monoCtl_lChanged(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 834 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:lChanged: lambda>2*D\n");
# line 835 "../ml_monoCtl.st"
			sprintf(seqMsg1, "Wavelength > 2d spacing.");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			opAlert = 1;
			seq_pvPut(ssId, 3 /* opAlert */, 0);
		}
		return;
	case 1:
		{
# line 843 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:lChanged\n");
# line 843 "../ml_monoCtl.st"
			theta = radConv * asin(lambda / TwoD);
			seq_pvPut(ssId, 19 /* theta */, 0);
		}
		return;
	}
}
/* Code for state "thChanged" in state set "ml_monoCtl" */

/* Delay function for state "thChanged" in state set "ml_monoCtl" */
static void D_ml_monoCtl_thChanged(SS_ID ssId, struct UserVar *pVar)
{
# line 873 "../ml_monoCtl.st"
}

/* Event function for state "thChanged" in state set "ml_monoCtl" */
static long E_ml_monoCtl_thChanged(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 873 "../ml_monoCtl.st"
	if (TRUE)
	{
		*pNextState = 10;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "thChanged" in state set "ml_monoCtl" */
static void A_ml_monoCtl_thChanged(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 851 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:thChanged\n");
# line 860 "../ml_monoCtl.st"
			if ((theta <= thetaLo) || (theta >= thetaHi))
			{
				sprintf(seqMsg1, "Theta constrained to LIMIT");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				opAlert = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
				autoMode = 0;
				seq_pvPut(ssId, 6 /* autoMode */, 0);
				sprintf(seqMsg2, "Set to Manual Mode");
				seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			}
# line 861 "../ml_monoCtl.st"
			lambda = TwoD * sin(theta / radConv);
			seq_pvPut(ssId, 15 /* lambda */, 0);
			E = hc / lambda;
			seq_pvPut(ssId, 11 /* E */, 0);
# line 867 "../ml_monoCtl.st"
			thetaRdbk = thetaMotRdbk;
			lambdaRdbk = TwoD * sin(thetaRdbk / radConv);
			ERdbk = hc / lambdaRdbk;
			seq_pvPut(ssId, 22 /* thetaRdbk */, 0);
			seq_pvPut(ssId, 18 /* lambdaRdbk */, 0);
			seq_pvPut(ssId, 14 /* ERdbk */, 0);
		}
		return;
	}
}
/* Code for state "calcMovements" in state set "ml_monoCtl" */

/* Delay function for state "calcMovements" in state set "ml_monoCtl" */
static void D_ml_monoCtl_calcMovements(SS_ID ssId, struct UserVar *pVar)
{
# line 925 "../ml_monoCtl.st"
}

/* Event function for state "calcMovements" in state set "ml_monoCtl" */
static long E_ml_monoCtl_calcMovements(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 925 "../ml_monoCtl.st"
	if (TRUE)
	{
		*pNextState = 11;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "calcMovements" in state set "ml_monoCtl" */
static void A_ml_monoCtl_calcMovements(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 879 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:calcMovements\n");
			thetaMotDesired = theta;
			theta2MotDesired = theta;
# line 885 "../ml_monoCtl.st"
			if (geom == 1)
			{
				zMotDesired = yOffset / tan(2 * theta / radConv);
			}
# line 887 "../ml_monoCtl.st"
			else
			{
				printf("ml_mono:calcMovements: unimplemented geom=%d\n", geom);
			}
# line 898 "../ml_monoCtl.st"
			if (ccMode != 1 && ((zMotDesired < zMotLoLim) || (zMotDesired > zMotHiLim)))
			{
				sprintf(seqMsg1, "Z will exceed soft limits");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				sprintf(seqMsg2, "Setting to Manual Mode");
				seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
				opAlert = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
				autoMode = 0;
				seq_pvPut(ssId, 6 /* autoMode */, 0);
			}
# line 899 "../ml_monoCtl.st"
			seq_pvPut(ssId, 51 /* thetaMotDesired */, 0);
			seq_pvPut(ssId, 52 /* theta2MotDesired */, 0);
			seq_pvPut(ssId, 53 /* zMotDesired */, 0);
# line 904 "../ml_monoCtl.st"
			oldThSpeed = thSpeed;
			oldTh2Speed = th2Speed;
			oldZSpeed = zSpeed;
			zTime = fabs(zMotDesired - zMotRdbk) / zSpeed;
			thTime = fabs(theta - thetaMotRdbk) / thSpeed;
# line 913 "../ml_monoCtl.st"
			if (ccMode == 1)
			{
				zTime = 0.0;
			}
# line 919 "../ml_monoCtl.st"
			if (thTime >= zTime)
			{
				newThSpeed = thSpeed;
				newTh2Speed = th2Speed;
				newZSpeed = fabs(zMotDesired - zMotRdbk) / thTime;
			}
# line 923 "../ml_monoCtl.st"
			else
			{
				newThSpeed = fabs(theta - thetaMotRdbk) / zTime;
				newTh2Speed = newThSpeed;
				newZSpeed = zSpeed;
			}
		}
		return;
	}
}
/* Code for state "move_ml_mono" in state set "ml_monoCtl" */

/* Delay function for state "move_ml_mono" in state set "ml_monoCtl" */
static void D_ml_monoCtl_move_ml_mono(SS_ID ssId, struct UserVar *pVar)
{
# line 950 "../ml_monoCtl.st"
# line 954 "../ml_monoCtl.st"
}

/* Event function for state "move_ml_mono" in state set "ml_monoCtl" */
static long E_ml_monoCtl_move_ml_mono(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 950 "../ml_monoCtl.st"
	if (autoMode || putVals || useSetMode)
	{
		*pNextState = 13;
		*pTransNum = 0;
		return TRUE;
	}
# line 954 "../ml_monoCtl.st"
	if (1)
	{
		*pNextState = 2;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "move_ml_mono" in state set "ml_monoCtl" */
static void A_ml_monoCtl_move_ml_mono(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 931 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:move_ml_mono:auto|put\n");
# line 931 "../ml_monoCtl.st"
			thSpeed = newThSpeed;
			th2Speed = newThSpeed;
			zSpeed = newZSpeed;
			seq_pvPut(ssId, 66 /* thSpeed */, 0);
			seq_pvPut(ssId, 67 /* th2Speed */, 0);
# line 938 "../ml_monoCtl.st"
			if (ccMode == 0)
			{
				seq_pvPut(ssId, 68 /* zSpeed */, 0);
			}
# line 939 "../ml_monoCtl.st"
			thetaMotCmd = thetaMotDesired;
			theta2MotCmd = theta2MotDesired;
			zMotCmd = zMotDesired;
			seq_pvPut(ssId, 60 /* thetaMotCmd */, 0);
			seq_pvPut(ssId, 61 /* theta2MotCmd */, 0);
# line 946 "../ml_monoCtl.st"
			if (ccMode == 0)
			{
				seq_pvPut(ssId, 62 /* zMotCmd */, 0);
			}
# line 947 "../ml_monoCtl.st"
			putVals = 0;
			seq_pvPut(ssId, 5 /* putVals */, 0);
			causedMove = 1;
		}
		return;
	case 1:
		{
# line 954 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:move_ml_mono\n");
		}
		return;
	}
}
/* Code for state "checkDone" in state set "ml_monoCtl" */

/* Delay function for state "checkDone" in state set "ml_monoCtl" */
static void D_ml_monoCtl_checkDone(SS_ID ssId, struct UserVar *pVar)
{
# line 961 "../ml_monoCtl.st"
# line 965 "../ml_monoCtl.st"
}

/* Event function for state "checkDone" in state set "ml_monoCtl" */
static long E_ml_monoCtl_checkDone(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 961 "../ml_monoCtl.st"
	if (thetaDmov && theta2Dmov && zDmov)
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 965 "../ml_monoCtl.st"
	if (!thetaDmov || !theta2Dmov || !zDmov)
	{
		*pNextState = 13;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "checkDone" in state set "ml_monoCtl" */
static void A_ml_monoCtl_checkDone(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 960 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:checkDone: done\n");
# line 960 "../ml_monoCtl.st"
			ml_monoDone = 1;
		}
		return;
	case 1:
		{
# line 965 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:checkDone: not done\n");
		}
		return;
	}
}
/* Code for state "updateRdbk" in state set "ml_monoCtl" */

/* Delay function for state "updateRdbk" in state set "ml_monoCtl" */
static void D_ml_monoCtl_updateRdbk(SS_ID ssId, struct UserVar *pVar)
{
# line 978 "../ml_monoCtl.st"
# line 997 "../ml_monoCtl.st"
# line 1010 "../ml_monoCtl.st"
# line 1023 "../ml_monoCtl.st"
# line 1039 "../ml_monoCtl.st"
	seq_delayInit(ssId, 0, (.1));
}

/* Event function for state "updateRdbk" in state set "ml_monoCtl" */
static long E_ml_monoCtl_updateRdbk(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 978 "../ml_monoCtl.st"
	if (thetaMotRdbk == 0 || theta2MotRdbk == 0)
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 997 "../ml_monoCtl.st"
	if (thetaDmov && theta2Dmov && zDmov)
	{
		*pNextState = 14;
		*pTransNum = 1;
		return TRUE;
	}
# line 1010 "../ml_monoCtl.st"
	if (thetaHls || thetaLls || theta2Hls || theta2Lls)
	{
		*pNextState = 17;
		*pTransNum = 2;
		return TRUE;
	}
# line 1023 "../ml_monoCtl.st"
	if ((ccMode != 1) && (zHls || zLls))
	{
		*pNextState = 17;
		*pTransNum = 3;
		return TRUE;
	}
# line 1039 "../ml_monoCtl.st"
	if ((!thetaDmov || !theta2Dmov || !zDmov) && seq_delay(ssId, 0))
	{
		*pNextState = 13;
		*pTransNum = 4;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "updateRdbk" in state set "ml_monoCtl" */
static void A_ml_monoCtl_updateRdbk(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 972 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:updateRdbk:thetaMotRdbk==0\n");
# line 972 "../ml_monoCtl.st"
			sprintf(seqMsg1, "Theta Motor Readback is 0 !");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			sprintf(seqMsg2, " ");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			opAlert = 1;
			seq_pvPut(ssId, 3 /* opAlert */, 0);
		}
		return;
	case 1:
		{
# line 984 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
			{
printf("ml_monoCtl:updateRdbk:*Dmov (done=%d,moving=%d)\n",
 ml_monoDone, ml_monoMoving);
			}
# line 985 "../ml_monoCtl.st"
			thetaRdbk = thetaMotRdbk;
			lambdaRdbk = TwoD * sin(thetaRdbk / radConv);
			ERdbk = hc / lambdaRdbk;
			seq_pvPut(ssId, 22 /* thetaRdbk */, 0);
			seq_pvPut(ssId, 18 /* lambdaRdbk */, 0);
			seq_pvPut(ssId, 14 /* ERdbk */, 0);
			ml_monoDone = 1;
			seq_efSet(ssId, thetaMotRdbkPseudo_mon);
			seq_efClear(ssId, thetaMotRdbk_mon);
			seq_efSet(ssId, theta2MotRdbkPseudo_mon);
			seq_efClear(ssId, theta2MotRdbk_mon);
# line 997 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:updateRdbk: done\n");
		}
		return;
	case 2:
		{
# line 1002 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:updateRdbk: Th limit switch\n");
# line 1002 "../ml_monoCtl.st"
			sprintf(seqMsg1, "Theta Motor hit a limit switch!");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			opAlert = 1;
			seq_pvPut(ssId, 3 /* opAlert */, 0);
			autoMode = 0;
			seq_pvPut(ssId, 6 /* autoMode */, 0);
			sprintf(seqMsg2, "Setting to Manual Mode");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
		}
		return;
	case 3:
		{
# line 1015 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:updateRdbk: Z limit switch\n");
# line 1015 "../ml_monoCtl.st"
			sprintf(seqMsg1, "Z Motor hit a limit switch!");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			opAlert = 1;
			seq_pvPut(ssId, 3 /* opAlert */, 0);
			autoMode = 0;
			seq_pvPut(ssId, 6 /* autoMode */, 0);
			sprintf(seqMsg2, "Setting to Manual Mode");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
		}
		return;
	case 4:
		{
# line 1027 "../ml_monoCtl.st"
			if (ml_monoCtlDebug > 1)
printf("ml_monoCtl:updateRdbk:moving\n");
# line 1027 "../ml_monoCtl.st"
			thetaRdbk = thetaMotRdbk;
			lambdaRdbk = TwoD * sin(thetaRdbk / radConv);
			ERdbk = hc / lambdaRdbk;
			seq_pvPut(ssId, 22 /* thetaRdbk */, 0);
			seq_pvPut(ssId, 18 /* lambdaRdbk */, 0);
			seq_pvPut(ssId, 14 /* ERdbk */, 0);
# line 1034 "../ml_monoCtl.st"
			thetaMotRdbkEcho = thetaMotRdbk;
			seq_pvPut(ssId, 30 /* thetaMotRdbkEcho */, 0);
			theta2MotRdbkEcho = theta2MotRdbk;
			seq_pvPut(ssId, 31 /* theta2MotRdbkEcho */, 0);
		}
		return;
	}
}
/* Code for state "thetaMotStopped" in state set "ml_monoCtl" */

/* Delay function for state "thetaMotStopped" in state set "ml_monoCtl" */
static void D_ml_monoCtl_thetaMotStopped(SS_ID ssId, struct UserVar *pVar)
{
# line 1055 "../ml_monoCtl.st"
# line 1079 "../ml_monoCtl.st"
}

/* Event function for state "thetaMotStopped" in state set "ml_monoCtl" */
static long E_ml_monoCtl_thetaMotStopped(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1055 "../ml_monoCtl.st"
	if (causedMove)
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 1079 "../ml_monoCtl.st"
	if (!causedMove)
	{
		*pNextState = 9;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "thetaMotStopped" in state set "ml_monoCtl" */
static void A_ml_monoCtl_thetaMotStopped(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1047 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
			{
printf("ml_monoCtl:thetaMotStopped:causedMove (done=%d,moving=%d)\n",
 ml_monoDone, ml_monoMoving);
			}
# line 1048 "../ml_monoCtl.st"
			thSpeed = oldThSpeed;
			th2Speed = oldTh2Speed;
			zSpeed = oldZSpeed;
			seq_pvPut(ssId, 66 /* thSpeed */, 0);
			seq_pvPut(ssId, 67 /* th2Speed */, 0);
			seq_pvPut(ssId, 68 /* zSpeed */, 0);
			causedMove = 0;
		}
		return;
	case 1:
		{
# line 1062 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
			{
printf("ml_monoCtl:thetaMotStopped:!causedMove (done=%d,moving=%d)\n",
 ml_monoDone, ml_monoMoving);
			}
# line 1072 "../ml_monoCtl.st"
			if (initSeqDone)
			{
				opAlert = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
				sprintf(seqMsg1, "Only Theta Motor Moved");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				sprintf(seqMsg2, "Hit MOVE to Adjust Z");
				seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
				autoMode = 0;
				seq_pvPut(ssId, 6 /* autoMode */, 0);
			}
# line 1073 "../ml_monoCtl.st"
			theta = thetaMotRdbk;
			seq_pvPut(ssId, 19 /* theta */, 0);
			lambda = TwoD * sin(theta / radConv);
			seq_pvPut(ssId, 15 /* lambda */, 0);
			E = hc / lambda;
			seq_pvPut(ssId, 11 /* E */, 0);
		}
		return;
	}
}
/* Code for state "chkMotorLimits" in state set "ml_monoCtl" */

/* Delay function for state "chkMotorLimits" in state set "ml_monoCtl" */
static void D_ml_monoCtl_chkMotorLimits(SS_ID ssId, struct UserVar *pVar)
{
# line 1139 "../ml_monoCtl.st"
}

/* Event function for state "chkMotorLimits" in state set "ml_monoCtl" */
static long E_ml_monoCtl_chkMotorLimits(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1139 "../ml_monoCtl.st"
	if (TRUE)
	{
		*pNextState = 16;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "chkMotorLimits" in state set "ml_monoCtl" */
static void A_ml_monoCtl_chkMotorLimits(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1085 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:chkMotorLimits\n");
# line 1085 "../ml_monoCtl.st"
			opAlert = 0;
			thetaRdbk = thetaMotRdbk;
			lambdaRdbk = TwoD * sin(thetaRdbk / radConv);
			ERdbk = hc / lambdaRdbk;
			seq_pvPut(ssId, 22 /* thetaRdbk */, 0);
			seq_pvPut(ssId, 18 /* lambdaRdbk */, 0);
			seq_pvPut(ssId, 14 /* ERdbk */, 0);
# line 1104 "../ml_monoCtl.st"
			if (ccMode != 1)
			{
				if (zMotRdbk == 0)
				{
					sprintf(seqMsg1, "Z Readback of 0 not reasonable");
					seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
					opAlert = 1;
				}
				if ((zMotRdbk < zMotLoLim) || (zMotRdbk > zMotHiLim))
				{
					sprintf(seqMsg1, "Z Readback outside Motor Limits");
					seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
					opAlert = 1;
				}
			}
# line 1109 "../ml_monoCtl.st"
			if (yMotRdbk == 0)
			{
				sprintf(seqMsg1, "Y Readback of 0 not reasonable");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				opAlert = 1;
			}
# line 1114 "../ml_monoCtl.st"
			if (thetaMotRdbk == 0)
			{
				sprintf(seqMsg1, "Theta Readback of 0 not reasonable");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				opAlert = 1;
			}
# line 1119 "../ml_monoCtl.st"
			if ((thetaMotRdbk < thetaMotLoLim) || (thetaMotRdbk > thetaMotHiLim))
			{
				sprintf(seqMsg1, "Theta Readback outside Motor Limits");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				opAlert = 1;
			}
			if (theta2MotRdbk == 0)
			{
				sprintf(seqMsg1, "Theta2 Readback of 0 not reasonable");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				opAlert = 1;
			}
# line 1130 "../ml_monoCtl.st"
			if ((theta2MotRdbk < theta2MotLoLim) || (theta2MotRdbk > theta2MotHiLim))
			{
				sprintf(seqMsg1, "Theta2 Readback outside Motor Limits");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				opAlert = 1;
			}
# line 1138 "../ml_monoCtl.st"
			if (opAlert)
			{
				seq_pvPut(ssId, 3 /* opAlert */, 0);
				autoMode = 0;
				seq_pvPut(ssId, 6 /* autoMode */, 0);
				sprintf(seqMsg2, "Set to Manual Mode");
				seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			}
		}
		return;
	}
}
/* Code for state "chkMotorLimitsExit" in state set "ml_monoCtl" */

/* Delay function for state "chkMotorLimitsExit" in state set "ml_monoCtl" */
static void D_ml_monoCtl_chkMotorLimitsExit(SS_ID ssId, struct UserVar *pVar)
{
# line 1148 "../ml_monoCtl.st"
# line 1155 "../ml_monoCtl.st"
	seq_delayInit(ssId, 0, (2.));
}

/* Event function for state "chkMotorLimitsExit" in state set "ml_monoCtl" */
static long E_ml_monoCtl_chkMotorLimitsExit(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1148 "../ml_monoCtl.st"
	if (!opAlert)
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 1155 "../ml_monoCtl.st"
	if (opAlert && seq_delay(ssId, 0))
	{
		*pNextState = 15;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "chkMotorLimitsExit" in state set "ml_monoCtl" */
static void A_ml_monoCtl_chkMotorLimitsExit(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1145 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:chkMotorLimitsExit: OK\n");
# line 1145 "../ml_monoCtl.st"
			seq_pvPut(ssId, 3 /* opAlert */, 0);
			sprintf(seqMsg1, "All motors within limits");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
		}
		return;
	case 1:
		{
# line 1152 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:chkMotorLimitsExit: opAlert\n");
# line 1152 "../ml_monoCtl.st"
			seq_pvPut(ssId, 3 /* opAlert */, 0);
			sprintf(seqMsg2, "Modify Motor Positions/Limits");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
		}
		return;
	}
}
/* Code for state "stop_ml_mono" in state set "ml_monoCtl" */

/* Delay function for state "stop_ml_mono" in state set "ml_monoCtl" */
static void D_ml_monoCtl_stop_ml_mono(SS_ID ssId, struct UserVar *pVar)
{
# line 1164 "../ml_monoCtl.st"
}

/* Event function for state "stop_ml_mono" in state set "ml_monoCtl" */
static long E_ml_monoCtl_stop_ml_mono(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1164 "../ml_monoCtl.st"
	if (TRUE)
	{
		*pNextState = 18;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "stop_ml_mono" in state set "ml_monoCtl" */
static void A_ml_monoCtl_stop_ml_mono(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1161 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:stop_ml_mono\n");
# line 1161 "../ml_monoCtl.st"
			seq_pvPut(ssId, 39 /* thetaMotStop */, 0);
			seq_pvPut(ssId, 40 /* theta2MotStop */, 0);
			seq_pvPut(ssId, 41 /* zStop */, 0);
		}
		return;
	}
}
/* Code for state "stopped_ml_monoWait" in state set "ml_monoCtl" */

/* Delay function for state "stopped_ml_monoWait" in state set "ml_monoCtl" */
static void D_ml_monoCtl_stopped_ml_monoWait(SS_ID ssId, struct UserVar *pVar)
{
# line 1170 "../ml_monoCtl.st"
	seq_delayInit(ssId, 0, (1.));
}

/* Event function for state "stopped_ml_monoWait" in state set "ml_monoCtl" */
static long E_ml_monoCtl_stopped_ml_monoWait(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1170 "../ml_monoCtl.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 13;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "stopped_ml_monoWait" in state set "ml_monoCtl" */
static void A_ml_monoCtl_stopped_ml_monoWait(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1170 "../ml_monoCtl.st"
			if (ml_monoCtlDebug)
printf("ml_monoCtl:stopped_ml_monoWait\n");
		}
		return;
	}
}
/* Code for state "init" in state set "updatePsuedo" */

/* Delay function for state "init" in state set "updatePsuedo" */
static void D_updatePsuedo_init(SS_ID ssId, struct UserVar *pVar)
{
# line 1184 "../ml_monoCtl.st"
}

/* Event function for state "init" in state set "updatePsuedo" */
static long E_updatePsuedo_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1184 "../ml_monoCtl.st"
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
# line 1191 "../ml_monoCtl.st"
# line 1196 "../ml_monoCtl.st"
# line 1201 "../ml_monoCtl.st"
# line 1206 "../ml_monoCtl.st"
# line 1211 "../ml_monoCtl.st"
# line 1216 "../ml_monoCtl.st"
# line 1221 "../ml_monoCtl.st"
# line 1226 "../ml_monoCtl.st"
# line 1231 "../ml_monoCtl.st"
# line 1236 "../ml_monoCtl.st"
# line 1241 "../ml_monoCtl.st"
# line 1246 "../ml_monoCtl.st"
# line 1251 "../ml_monoCtl.st"
}

/* Event function for state "update" in state set "updatePsuedo" */
static long E_updatePsuedo_update(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1191 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, thetaMotRdbkPseudo_mon))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 1196 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, theta2MotRdbkPseudo_mon))
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
# line 1201 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, zMotRdbk_mon))
	{
		*pNextState = 1;
		*pTransNum = 2;
		return TRUE;
	}
# line 1206 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, yMotRdbk_mon))
	{
		*pNextState = 1;
		*pTransNum = 3;
		return TRUE;
	}
# line 1211 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, thetaMotVel_mon))
	{
		*pNextState = 1;
		*pTransNum = 4;
		return TRUE;
	}
# line 1216 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, theta2MotVel_mon))
	{
		*pNextState = 1;
		*pTransNum = 5;
		return TRUE;
	}
# line 1221 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, zMotVel_mon))
	{
		*pNextState = 1;
		*pTransNum = 6;
		return TRUE;
	}
# line 1226 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, thetaMotCmdMon_mon))
	{
		*pNextState = 1;
		*pTransNum = 7;
		return TRUE;
	}
# line 1231 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, theta2MotCmdMon_mon))
	{
		*pNextState = 1;
		*pTransNum = 8;
		return TRUE;
	}
# line 1236 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, zMotCmdMon_mon))
	{
		*pNextState = 1;
		*pTransNum = 9;
		return TRUE;
	}
# line 1241 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, thetaDmov_mon))
	{
		*pNextState = 1;
		*pTransNum = 10;
		return TRUE;
	}
# line 1246 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, theta2Dmov_mon))
	{
		*pNextState = 1;
		*pTransNum = 11;
		return TRUE;
	}
# line 1251 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, zDmov_mon))
	{
		*pNextState = 1;
		*pTransNum = 12;
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
# line 1189 "../ml_monoCtl.st"
			thetaMotRdbkEcho = thetaMotRdbk;
			seq_pvPut(ssId, 30 /* thetaMotRdbkEcho */, 0);
		}
		return;
	case 1:
		{
# line 1194 "../ml_monoCtl.st"
			theta2MotRdbkEcho = theta2MotRdbk;
			seq_pvPut(ssId, 31 /* theta2MotRdbkEcho */, 0);
		}
		return;
	case 2:
		{
# line 1199 "../ml_monoCtl.st"
			zMotRdbkEcho = zMotRdbk;
			seq_pvPut(ssId, 32 /* zMotRdbkEcho */, 0);
		}
		return;
	case 3:
		{
# line 1204 "../ml_monoCtl.st"
			yOffset = yMotRdbk;
			seq_pvPut(ssId, 86 /* yOffset */, 0);
		}
		return;
	case 4:
		{
# line 1209 "../ml_monoCtl.st"
			thetaMotVelEcho = thSpeed;
			seq_pvPut(ssId, 33 /* thetaMotVelEcho */, 0);
		}
		return;
	case 5:
		{
# line 1214 "../ml_monoCtl.st"
			theta2MotVelEcho = th2Speed;
			seq_pvPut(ssId, 34 /* theta2MotVelEcho */, 0);
		}
		return;
	case 6:
		{
# line 1219 "../ml_monoCtl.st"
			zMotVelEcho = zSpeed;
			seq_pvPut(ssId, 35 /* zMotVelEcho */, 0);
		}
		return;
	case 7:
		{
# line 1224 "../ml_monoCtl.st"
			thetaMotCmdEcho = thetaMotCmdMon;
			seq_pvPut(ssId, 27 /* thetaMotCmdEcho */, 0);
		}
		return;
	case 8:
		{
# line 1229 "../ml_monoCtl.st"
			theta2MotCmdEcho = theta2MotCmdMon;
			seq_pvPut(ssId, 28 /* theta2MotCmdEcho */, 0);
		}
		return;
	case 9:
		{
# line 1234 "../ml_monoCtl.st"
			zMotCmdEcho = zMotCmdMon;
			seq_pvPut(ssId, 29 /* zMotCmdEcho */, 0);
		}
		return;
	case 10:
		{
# line 1239 "../ml_monoCtl.st"
			thetaDmovEcho = thetaDmov;
			seq_pvPut(ssId, 36 /* thetaDmovEcho */, 0);
		}
		return;
	case 11:
		{
# line 1244 "../ml_monoCtl.st"
			theta2DmovEcho = theta2Dmov;
			seq_pvPut(ssId, 37 /* theta2DmovEcho */, 0);
		}
		return;
	case 12:
		{
# line 1249 "../ml_monoCtl.st"
			zDmovEcho = zDmov;
			seq_pvPut(ssId, 38 /* zDmovEcho */, 0);
		}
		return;
	}
}
/* Code for state "init" in state set "updateSet" */

/* Delay function for state "init" in state set "updateSet" */
static void D_updateSet_init(SS_ID ssId, struct UserVar *pVar)
{
# line 1264 "../ml_monoCtl.st"
}

/* Event function for state "init" in state set "updateSet" */
static long E_updateSet_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1264 "../ml_monoCtl.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "updateSet" */
static void A_updateSet_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
		}
		return;
	}
}
/* Code for state "update" in state set "updateSet" */

/* Delay function for state "update" in state set "updateSet" */
static void D_updateSet_update(SS_ID ssId, struct UserVar *pVar)
{
# line 1281 "../ml_monoCtl.st"
# line 1288 "../ml_monoCtl.st"
# line 1295 "../ml_monoCtl.st"
# line 1302 "../ml_monoCtl.st"
}

/* Event function for state "update" in state set "updateSet" */
static long E_updateSet_update(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1281 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, useSetMode_mon))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 1288 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, thetaMotSet_mon))
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
# line 1295 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, theta2MotSet_mon))
	{
		*pNextState = 1;
		*pTransNum = 2;
		return TRUE;
	}
# line 1302 "../ml_monoCtl.st"
	if (seq_efTestAndClear(ssId, zMotSet_mon))
	{
		*pNextState = 1;
		*pTransNum = 3;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "update" in state set "updateSet" */
static void A_updateSet_update(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1272 "../ml_monoCtl.st"
			if (thetaMotSet != useSetMode)
			{
				thetaMotSet = useSetMode;
				seq_pvPut(ssId, 74 /* thetaMotSet */, 0);
			}
# line 1276 "../ml_monoCtl.st"
			if (theta2MotSet != useSetMode)
			{
				theta2MotSet = useSetMode;
				seq_pvPut(ssId, 75 /* theta2MotSet */, 0);
			}
# line 1280 "../ml_monoCtl.st"
			if (zMotSet != useSetMode)
			{
				zMotSet = useSetMode;
				seq_pvPut(ssId, 76 /* zMotSet */, 0);
			}
		}
		return;
	case 1:
		{
# line 1287 "../ml_monoCtl.st"
			if (useSetMode != thetaMotSet)
			{
				useSetMode = thetaMotSet;
				seq_pvPut(ssId, 73 /* useSetMode */, 0);
			}
		}
		return;
	case 2:
		{
# line 1294 "../ml_monoCtl.st"
			if (useSetMode != theta2MotSet)
			{
				useSetMode = theta2MotSet;
				seq_pvPut(ssId, 73 /* useSetMode */, 0);
			}
		}
		return;
	case 3:
		{
# line 1301 "../ml_monoCtl.st"
			if (useSetMode != zMotSet)
			{
				useSetMode = zMotSet;
				seq_pvPut(ssId, 73 /* useSetMode */, 0);
			}
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
  {"{P}ml_monoCtlDebug", (void *)&ml_monoCtlDebug, "ml_monoCtlDebug", 
    "short", 1, 30, 0, 1, 0, 0, 0},

  {"{P}ml_monoSeqMsg1", (void *)&seqMsg1[0], "seqMsg1", 
    "string", 1, 31, 0, 0, 0, 0, 0},

  {"{P}ml_monoSeqMsg2", (void *)&seqMsg2[0], "seqMsg2", 
    "string", 1, 32, 0, 0, 0, 0, 0},

  {"{P}ml_monoAlert", (void *)&opAlert, "opAlert", 
    "short", 1, 33, 0, 0, 0, 0, 0},

  {"{P}ml_monoOperAck", (void *)&opAck, "opAck", 
    "short", 1, 34, 0, 1, 0, 0, 0},

  {"{P}ml_monoPut", (void *)&putVals, "putVals", 
    "short", 1, 35, 0, 1, 0, 0, 0},

  {"{P}ml_monoMode", (void *)&autoMode, "autoMode", 
    "short", 1, 36, 0, 1, 0, 0, 0},

  {"{P}ml_monoMode2", (void *)&ccMode, "ccMode", 
    "short", 1, 37, 0, 1, 0, 0, 0},

  {"{P}ml_monoMoving", (void *)&ml_monoMoving, "ml_monoMoving", 
    "short", 1, 38, 0, 1, 0, 0, 0},

  {"{P}ml_monoOrder", (void *)&Order, "Order", 
    "double", 1, 39, 1, 1, 0, 0, 0},

  {"{P}ml_monoD", (void *)&D, "D", 
    "double", 1, 40, 2, 1, 0, 0, 0},

  {"{P}ml_monoE", (void *)&E, "E", 
    "double", 1, 41, 3, 1, 0, 0, 0},

  {"{P}ml_monoE.DRVH", (void *)&EHi, "EHi", 
    "double", 1, 42, 0, 0, 0, 0, 0},

  {"{P}ml_monoE.DRVL", (void *)&ELo, "ELo", 
    "double", 1, 43, 0, 0, 0, 0, 0},

  {"{P}ml_monoERdbk", (void *)&ERdbk, "ERdbk", 
    "double", 1, 44, 0, 0, 0, 0, 0},

  {"{P}ml_monoLambda", (void *)&lambda, "lambda", 
    "double", 1, 45, 4, 1, 0, 0, 0},

  {"{P}ml_monoLambda.DRVH", (void *)&lambdaHi, "lambdaHi", 
    "double", 1, 46, 0, 0, 0, 0, 0},

  {"{P}ml_monoLambda.DRVL", (void *)&lambdaLo, "lambdaLo", 
    "double", 1, 47, 0, 0, 0, 0, 0},

  {"{P}ml_monoLambdaRdbk", (void *)&lambdaRdbk, "lambdaRdbk", 
    "double", 1, 48, 0, 0, 0, 0, 0},

  {"{P}ml_monoTheta", (void *)&theta, "theta", 
    "double", 1, 49, 5, 1, 0, 0, 0},

  {"{P}ml_monoTheta.DRVH", (void *)&thetaHi, "thetaHi", 
    "double", 1, 50, 0, 0, 0, 0, 0},

  {"{P}ml_monoTheta.DRVL", (void *)&thetaLo, "thetaLo", 
    "double", 1, 51, 0, 0, 0, 0, 0},

  {"{P}ml_monoThetaRdbk", (void *)&thetaRdbk, "thetaRdbk", 
    "double", 1, 52, 0, 0, 0, 0, 0},

  {"{P}ml_monoThetaPv", (void *)&thetaMotName[0], "thetaMotName", 
    "string", 1, 53, 0, 0, 0, 0, 0},

  {"{P}ml_monoTheta2Pv", (void *)&theta2MotName[0], "theta2MotName", 
    "string", 1, 54, 0, 0, 0, 0, 0},

  {"{P}ml_monoZPv", (void *)&zMotName[0], "zMotName", 
    "string", 1, 55, 0, 0, 0, 0, 0},

  {"{P}ml_monoYPv", (void *)&yMotName[0], "yMotName", 
    "string", 1, 56, 0, 0, 0, 0, 0},

  {"{P}ml_monoThetaCmd", (void *)&thetaMotCmdEcho, "thetaMotCmdEcho", 
    "double", 1, 57, 0, 0, 0, 0, 0},

  {"{P}ml_monoTheta2Cmd", (void *)&theta2MotCmdEcho, "theta2MotCmdEcho", 
    "double", 1, 58, 0, 0, 0, 0, 0},

  {"{P}ml_monoZCmd", (void *)&zMotCmdEcho, "zMotCmdEcho", 
    "double", 1, 59, 0, 0, 0, 0, 0},

  {"{P}ml_monoThetaRdbkEcho", (void *)&thetaMotRdbkEcho, "thetaMotRdbkEcho", 
    "double", 1, 60, 0, 0, 0, 0, 0},

  {"{P}ml_monoTheta2RdbkEcho", (void *)&theta2MotRdbkEcho, "theta2MotRdbkEcho", 
    "double", 1, 61, 0, 0, 0, 0, 0},

  {"{P}ml_monoZRdbk", (void *)&zMotRdbkEcho, "zMotRdbkEcho", 
    "double", 1, 62, 0, 0, 0, 0, 0},

  {"{P}ml_monoThetaVel", (void *)&thetaMotVelEcho, "thetaMotVelEcho", 
    "double", 1, 63, 0, 0, 0, 0, 0},

  {"{P}ml_monoTheta2Vel", (void *)&theta2MotVelEcho, "theta2MotVelEcho", 
    "double", 1, 64, 0, 0, 0, 0, 0},

  {"{P}ml_monoZVel", (void *)&zMotVelEcho, "zMotVelEcho", 
    "double", 1, 65, 0, 0, 0, 0, 0},

  {"{P}ml_monoThetaDmov", (void *)&thetaDmovEcho, "thetaDmovEcho", 
    "short", 1, 66, 0, 0, 0, 0, 0},

  {"{P}ml_monoTheta2Dmov", (void *)&theta2DmovEcho, "theta2DmovEcho", 
    "short", 1, 67, 0, 0, 0, 0, 0},

  {"{P}ml_monoZDmov", (void *)&zDmovEcho, "zDmovEcho", 
    "short", 1, 68, 0, 0, 0, 0, 0},

  {"{P}{M_THETA}.STOP", (void *)&thetaMotStop, "thetaMotStop", 
    "short", 1, 69, 0, 0, 0, 0, 0},

  {"{P}{M_THETA2}.STOP", (void *)&theta2MotStop, "theta2MotStop", 
    "short", 1, 70, 0, 0, 0, 0, 0},

  {"{P}{M_Z}.STOP", (void *)&zStop, "zStop", 
    "short", 1, 71, 0, 0, 0, 0, 0},

  {"{P}{M_THETA}.DMOV", (void *)&thetaDmov, "thetaDmov", 
    "short", 1, 72, 6, 1, 0, 0, 0},

  {"{P}{M_THETA2}.DMOV", (void *)&theta2Dmov, "theta2Dmov", 
    "short", 1, 73, 7, 1, 0, 0, 0},

  {"{P}{M_Z}.DMOV", (void *)&zDmov, "zDmov", 
    "short", 1, 74, 8, 1, 0, 0, 0},

  {"{P}{M_THETA}.HLS", (void *)&thetaHls, "thetaHls", 
    "short", 1, 75, 0, 1, 0, 0, 0},

  {"{P}{M_THETA}.LLS", (void *)&thetaLls, "thetaLls", 
    "short", 1, 76, 0, 1, 0, 0, 0},

  {"{P}{M_THETA2}.HLS", (void *)&theta2Hls, "theta2Hls", 
    "short", 1, 77, 0, 1, 0, 0, 0},

  {"{P}{M_THETA2}.LLS", (void *)&theta2Lls, "theta2Lls", 
    "short", 1, 78, 0, 1, 0, 0, 0},

  {"{P}{M_Z}.HLS", (void *)&zHls, "zHls", 
    "short", 1, 79, 0, 1, 0, 0, 0},

  {"{P}{M_Z}.LLS", (void *)&zLls, "zLls", 
    "short", 1, 80, 0, 1, 0, 0, 0},

  {"{P}ml_monoThetaSet", (void *)&thetaMotDesired, "thetaMotDesired", 
    "double", 1, 81, 0, 1, 0, 0, 0},

  {"{P}ml_monoTheta2Set", (void *)&theta2MotDesired, "theta2MotDesired", 
    "double", 1, 82, 0, 1, 0, 0, 0},

  {"{P}ml_monoZSet", (void *)&zMotDesired, "zMotDesired", 
    "double", 1, 83, 0, 1, 0, 0, 0},

  {"{P}{M_THETA}.HLM", (void *)&thetaMotHiLim, "thetaMotHiLim", 
    "double", 1, 84, 9, 1, 0, 0, 0},

  {"{P}{M_THETA}.LLM", (void *)&thetaMotLoLim, "thetaMotLoLim", 
    "double", 1, 85, 10, 1, 0, 0, 0},

  {"{P}{M_THETA2}.HLM", (void *)&theta2MotHiLim, "theta2MotHiLim", 
    "double", 1, 86, 11, 1, 0, 0, 0},

  {"{P}{M_THETA2}.LLM", (void *)&theta2MotLoLim, "theta2MotLoLim", 
    "double", 1, 87, 12, 1, 0, 0, 0},

  {"{P}{M_Z}.HLM", (void *)&zMotHiLim, "zMotHiLim", 
    "double", 1, 88, 0, 1, 0, 0, 0},

  {"{P}{M_Z}.LLM", (void *)&zMotLoLim, "zMotLoLim", 
    "double", 1, 89, 0, 1, 0, 0, 0},

  {"{P}{M_THETA}", (void *)&thetaMotCmd, "thetaMotCmd", 
    "double", 1, 90, 0, 0, 0, 0, 0},

  {"{P}{M_THETA2}", (void *)&theta2MotCmd, "theta2MotCmd", 
    "double", 1, 91, 0, 0, 0, 0, 0},

  {"{P}{M_Z}", (void *)&zMotCmd, "zMotCmd", 
    "double", 1, 92, 0, 0, 0, 0, 0},

  {"{P}{M_THETA}", (void *)&thetaMotCmdMon, "thetaMotCmdMon", 
    "double", 1, 93, 13, 1, 0, 0, 0},

  {"{P}{M_THETA2}", (void *)&theta2MotCmdMon, "theta2MotCmdMon", 
    "double", 1, 94, 14, 1, 0, 0, 0},

  {"{P}{M_Z}", (void *)&zMotCmdMon, "zMotCmdMon", 
    "double", 1, 95, 15, 1, 0, 0, 0},

  {"{P}{M_THETA}.VELO", (void *)&thSpeed, "thSpeed", 
    "double", 1, 96, 16, 1, 0, 0, 0},

  {"{P}{M_THETA2}.VELO", (void *)&th2Speed, "th2Speed", 
    "double", 1, 97, 17, 1, 0, 0, 0},

  {"{P}{M_Z}.VELO", (void *)&zSpeed, "zSpeed", 
    "double", 1, 98, 18, 1, 0, 0, 0},

  {"{P}{M_THETA}.RBV", (void *)&thetaMotRdbk, "thetaMotRdbk", 
    "double", 1, 99, 19, 1, 0, 0, 0},

  {"{P}{M_THETA2}.RBV", (void *)&theta2MotRdbk, "theta2MotRdbk", 
    "double", 1, 100, 21, 1, 0, 0, 0},

  {"{P}{M_Z}.RBV", (void *)&zMotRdbk, "zMotRdbk", 
    "double", 1, 101, 23, 1, 0, 0, 0},

  {"{P}{M_Y}.RBV", (void *)&yMotRdbk, "yMotRdbk", 
    "double", 1, 102, 24, 1, 0, 0, 0},

  {"{P}ml_monoUseSet", (void *)&useSetMode, "useSetMode", 
    "short", 1, 103, 25, 1, 0, 0, 0},

  {"{P}{M_THETA}.SET", (void *)&thetaMotSet, "thetaMotSet", 
    "short", 1, 104, 26, 1, 0, 0, 0},

  {"{P}{M_THETA2}.SET", (void *)&theta2MotSet, "theta2MotSet", 
    "short", 1, 105, 27, 1, 0, 0, 0},

  {"{P}{M_Z}.SET", (void *)&zMotSet, "zMotSet", 
    "short", 1, 106, 28, 1, 0, 0, 0},

  {"{P}ml_monoETweak", (void *)&ETweakVal, "ETweakVal", 
    "double", 1, 107, 0, 1, 0, 0, 0},

  {"{P}ml_monoEInc", (void *)&ETweakInc, "ETweakInc", 
    "short", 1, 108, 0, 1, 0, 0, 0},

  {"{P}ml_monoEDec", (void *)&ETweakDec, "ETweakDec", 
    "short", 1, 109, 0, 1, 0, 0, 0},

  {"{P}ml_monoLambdaTweak", (void *)&LTweakVal, "LTweakVal", 
    "double", 1, 110, 0, 1, 0, 0, 0},

  {"{P}ml_monoLambdaInc", (void *)&LTweakInc, "LTweakInc", 
    "short", 1, 111, 0, 1, 0, 0, 0},

  {"{P}ml_monoLambdaDec", (void *)&LTweakDec, "LTweakDec", 
    "short", 1, 112, 0, 1, 0, 0, 0},

  {"{P}ml_monoThetaTweak", (void *)&thTweakVal, "thTweakVal", 
    "double", 1, 113, 0, 1, 0, 0, 0},

  {"{P}ml_monoThetaInc", (void *)&thTweakInc, "thTweakInc", 
    "short", 1, 114, 0, 1, 0, 0, 0},

  {"{P}ml_monoThetaDec", (void *)&thTweakDec, "thTweakDec", 
    "short", 1, 115, 0, 1, 0, 0, 0},

  {"{P}ml_mono_yOffset", (void *)&yOffset, "yOffset", 
    "double", 1, 116, 29, 1, 0, 0, 0},

  {"{P}ml_mono_yOffset.DRVH", (void *)&yOffsetHi, "yOffsetHi", 
    "double", 1, 117, 0, 0, 0, 0, 0},

  {"{P}ml_mono_yOffset.DRVL", (void *)&yOffsetLo, "yOffsetLo", 
    "double", 1, 118, 0, 0, 0, 0, 0},

};

/* Event masks for state set ml_monoCtl */
	/* Event mask for state init: */
static bitMask	EM_ml_monoCtl_init[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state initSequence: */
static bitMask	EM_ml_monoCtl_initSequence[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state waitForCmndEnter: */
static bitMask	EM_ml_monoCtl_waitForCmndEnter[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state waitForCmnd: */
static bitMask	EM_ml_monoCtl_waitForCmnd[] = {
	0x20281e3e,
	0x0000000c,
	0x00000000,
	0x000db000,
};
	/* Event mask for state tweak: */
static bitMask	EM_ml_monoCtl_tweak[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x000db000,
};
	/* Event mask for state dInputChanged: */
static bitMask	EM_ml_monoCtl_dInputChanged[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state thetaLimits: */
static bitMask	EM_ml_monoCtl_thetaLimits[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state eChanged: */
static bitMask	EM_ml_monoCtl_eChanged[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state lChanged: */
static bitMask	EM_ml_monoCtl_lChanged[] = {
	0x00000000,
	0x00002000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state thChanged: */
static bitMask	EM_ml_monoCtl_thChanged[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state calcMovements: */
static bitMask	EM_ml_monoCtl_calcMovements[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state move_ml_mono: */
static bitMask	EM_ml_monoCtl_move_ml_mono[] = {
	0x00000000,
	0x00000018,
	0x00000000,
	0x00000080,
};
	/* Event mask for state checkDone: */
static bitMask	EM_ml_monoCtl_checkDone[] = {
	0x00000000,
	0x00000000,
	0x00000700,
	0x00000000,
};
	/* Event mask for state updateRdbk: */
static bitMask	EM_ml_monoCtl_updateRdbk[] = {
	0x00000000,
	0x00000020,
	0x0001ff00,
	0x00000018,
};
	/* Event mask for state thetaMotStopped: */
static bitMask	EM_ml_monoCtl_thetaMotStopped[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state chkMotorLimits: */
static bitMask	EM_ml_monoCtl_chkMotorLimits[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state chkMotorLimitsExit: */
static bitMask	EM_ml_monoCtl_chkMotorLimitsExit[] = {
	0x00000000,
	0x00000002,
	0x00000000,
	0x00000000,
};
	/* Event mask for state stop_ml_mono: */
static bitMask	EM_ml_monoCtl_stop_ml_mono[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state stopped_ml_monoWait: */
static bitMask	EM_ml_monoCtl_stopped_ml_monoWait[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_ml_monoCtl[] = {
	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_init,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_init,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_init,
	/* state options */   (0)},

	/* State "initSequence" */ {
	/* state name */       "initSequence",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_initSequence,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_initSequence,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_initSequence,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_initSequence,
	/* state options */   (0)},

	/* State "waitForCmndEnter" */ {
	/* state name */       "waitForCmndEnter",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_waitForCmndEnter,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_waitForCmndEnter,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_waitForCmndEnter,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_waitForCmndEnter,
	/* state options */   (0)},

	/* State "waitForCmnd" */ {
	/* state name */       "waitForCmnd",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_waitForCmnd,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_waitForCmnd,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_waitForCmnd,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_waitForCmnd,
	/* state options */   (0)},

	/* State "tweak" */ {
	/* state name */       "tweak",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_tweak,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_tweak,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_tweak,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_tweak,
	/* state options */   (0)},

	/* State "dInputChanged" */ {
	/* state name */       "dInputChanged",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_dInputChanged,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_dInputChanged,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_dInputChanged,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_dInputChanged,
	/* state options */   (0)},

	/* State "thetaLimits" */ {
	/* state name */       "thetaLimits",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_thetaLimits,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_thetaLimits,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_thetaLimits,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_thetaLimits,
	/* state options */   (0)},

	/* State "eChanged" */ {
	/* state name */       "eChanged",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_eChanged,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_eChanged,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_eChanged,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_eChanged,
	/* state options */   (0)},

	/* State "lChanged" */ {
	/* state name */       "lChanged",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_lChanged,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_lChanged,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_lChanged,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_lChanged,
	/* state options */   (0)},

	/* State "thChanged" */ {
	/* state name */       "thChanged",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_thChanged,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_thChanged,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_thChanged,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_thChanged,
	/* state options */   (0)},

	/* State "calcMovements" */ {
	/* state name */       "calcMovements",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_calcMovements,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_calcMovements,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_calcMovements,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_calcMovements,
	/* state options */   (0)},

	/* State "move_ml_mono" */ {
	/* state name */       "move_ml_mono",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_move_ml_mono,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_move_ml_mono,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_move_ml_mono,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_move_ml_mono,
	/* state options */   (0)},

	/* State "checkDone" */ {
	/* state name */       "checkDone",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_checkDone,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_checkDone,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_checkDone,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_checkDone,
	/* state options */   (0)},

	/* State "updateRdbk" */ {
	/* state name */       "updateRdbk",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_updateRdbk,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_updateRdbk,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_updateRdbk,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_updateRdbk,
	/* state options */   (0)},

	/* State "thetaMotStopped" */ {
	/* state name */       "thetaMotStopped",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_thetaMotStopped,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_thetaMotStopped,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_thetaMotStopped,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_thetaMotStopped,
	/* state options */   (0)},

	/* State "chkMotorLimits" */ {
	/* state name */       "chkMotorLimits",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_chkMotorLimits,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_chkMotorLimits,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_chkMotorLimits,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_chkMotorLimits,
	/* state options */   (0)},

	/* State "chkMotorLimitsExit" */ {
	/* state name */       "chkMotorLimitsExit",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_chkMotorLimitsExit,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_chkMotorLimitsExit,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_chkMotorLimitsExit,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_chkMotorLimitsExit,
	/* state options */   (0)},

	/* State "stop_ml_mono" */ {
	/* state name */       "stop_ml_mono",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_stop_ml_mono,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_stop_ml_mono,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_stop_ml_mono,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_stop_ml_mono,
	/* state options */   (0)},

	/* State "stopped_ml_monoWait" */ {
	/* state name */       "stopped_ml_monoWait",
	/* action function */ (ACTION_FUNC) A_ml_monoCtl_stopped_ml_monoWait,
	/* event function */  (EVENT_FUNC) E_ml_monoCtl_stopped_ml_monoWait,
	/* delay function */   (DELAY_FUNC) D_ml_monoCtl_stopped_ml_monoWait,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ml_monoCtl_stopped_ml_monoWait,
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
	0x01d7e1c0,
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

/* Event masks for state set updateSet */
	/* Event mask for state init: */
static bitMask	EM_updateSet_init[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state update: */
static bitMask	EM_updateSet_update[] = {
	0x1e000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_updateSet[] = {
	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_updateSet_init,
	/* event function */  (EVENT_FUNC) E_updateSet_init,
	/* delay function */   (DELAY_FUNC) D_updateSet_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_updateSet_init,
	/* state options */   (0)},

	/* State "update" */ {
	/* state name */       "update",
	/* action function */ (ACTION_FUNC) A_updateSet_update,
	/* event function */  (EVENT_FUNC) E_updateSet_update,
	/* delay function */   (DELAY_FUNC) D_updateSet_update,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_updateSet_update,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "ml_monoCtl" */ {
	/* ss name */            "ml_monoCtl",
	/* ptr to state block */ state_ml_monoCtl,
	/* number of states */   19,
	/* error state */        -1},


	/* State set "updatePsuedo" */ {
	/* ss name */            "updatePsuedo",
	/* ptr to state block */ state_updatePsuedo,
	/* number of states */   2,
	/* error state */        -1},


	/* State set "updateSet" */ {
	/* ss name */            "updateSet",
	/* ptr to state block */ state_updateSet,
	/* number of states */   2,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "P=xxx:, M_THETA=m9, M_THETA2=m12, M_Y=m10, M_Z=m11, Y_OFF=35, GEOM=1";

/* State Program table (global) */
struct seqProgram ml_monoCtl = {
	/* magic number */       20000315,
	/* *name */              "ml_monoCtl",
	/* *pChannels */         seqChan,
	/* numChans */           NUM_CHANNELS,
	/* *pSS */               seqSS,
	/* numSS */              NUM_SS,
	/* user variable size */ 0,
	/* *pParams */           prog_param,
	/* numEvents */          NUM_EVENTS,
	/* encoded options */    (0 | OPT_CONN | OPT_NEWEF),
	/* entry handler */      (ENTRY_FUNC) entry_handler,
	/* exit handler */       (EXIT_FUNC) exit_handler,
	/* numQueues */          NUM_QUEUES,
};
/* Global C code */
# line 1311 "../ml_monoCtl.st"



double ml_monoCtl_calc2dSpacing()
{

 double result;

 result = (2*D)/Order;

 opAlert = 0;
 if (Order < 1) opAlert = 1;
 if (opAlert) {
  sprintf(seqMsg1, "Order == %f?", Order);
 } else {
  sprintf(seqMsg1, "New effective d spacing");
 }
 return(result);
}



#include "epicsExport.h"


/* Register sequencer commands and program */

void ml_monoCtlRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&ml_monoCtl);
}
epicsExportRegistrar(ml_monoCtlRegistrar);

