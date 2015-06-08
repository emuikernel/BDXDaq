/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: kohzuCtl.i */

/* Event flags */
#define H_mon	1
#define K_mon	2
#define L_mon	3
#define A_mon	4
#define E_mon	5
#define lambda_mon	6
#define theta_mon	7
#define thetaDmov_mon	8
#define yDmov_mon	9
#define zDmov_mon	10
#define thetaMotHiLim_mon	11
#define thetaMotLoLim_mon	12
#define thetaMotCmdMon_mon	13
#define yMotCmdMon_mon	14
#define zMotCmdMon_mon	15
#define thetaMotVel_mon	16
#define yMotVel_mon	17
#define zMotVel_mon	18
#define thetaMotRdbk_mon	19
#define thetaMotRdbkPseudo_mon	20
#define yMotRdbk_mon	21
#define zMotRdbk_mon	22
#define useSetMode_mon	23
#define thetaMotSet_mon	24
#define yMotSet_mon	25
#define zMotSet_mon	26
#define yOffset_mon	27

/* Program "kohzuCtl" */
#include "seqCom.h"

#define NUM_SS 3
#define NUM_CHANNELS 92
#define NUM_EVENTS 27
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT FALSE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT FALSE

extern struct seqProgram kohzuCtl;

/* Variable declarations */
static short	kohzuCtlDebug;
static char	seqMsg1[MAX_STRING_SIZE];
static char	seqMsg2[MAX_STRING_SIZE];
static short	opAlert;
static short	opAck;
static short	putVals;
static short	autoMode;
static short	ccMode;
static short	kohzuMoving;
static short	kohzuDone;
static double	H;
static double	K;
static double	L;
static double	A;
static double	D;
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
static char	yMotName[MAX_STRING_SIZE];
static char	zMotName[MAX_STRING_SIZE];
static double	thetaMotCmdEcho;
static double	yMotCmdEcho;
static double	zMotCmdEcho;
static double	thetaMotRdbkEcho;
static double	yMotRdbkEcho;
static double	zMotRdbkEcho;
static double	thetaMotVelEcho;
static double	yMotVelEcho;
static double	zMotVelEcho;
static short	thetaDmovEcho;
static short	yDmovEcho;
static short	zDmovEcho;
static short	thetaMotStop;
static short	yStop;
static short	zStop;
static short	thetaDmov;
static short	yDmov;
static short	zDmov;
static short	thetaHls;
static short	thetaLls;
static short	yHls;
static short	yLls;
static short	zHls;
static short	zLls;
static double	thetaMotDesired;
static double	yMotDesired;
static double	yMotDesiredHi;
static double	yMotDesiredLo;
static double	zMotDesired;
static double	thetaMotHiLim;
static double	thetaMotLoLim;
static double	yMotHiLim;
static double	yMotLoLim;
static double	zMotHiLim;
static double	zMotLoLim;
static double	thetaMotCmd;
static double	yMotCmd;
static double	zMotCmd;
static double	thetaMotCmdMon;
static double	yMotCmdMon;
static double	zMotCmdMon;
static double	thTime;
static double	oldThSpeed;
static double	newThSpeed;
static double	thSpeed;
static double	yTime;
static double	oldYSpeed;
static double	newYSpeed;
static double	ySpeed;
static double	zTime;
static double	oldZSpeed;
static double	newZSpeed;
static double	zSpeed;
static double	thetaMotRdbk;
static double	yMotRdbk;
static double	zMotRdbk;
static short	useSetMode;
static short	thetaMotSet;
static short	yMotSet;
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
static float	avg;
static int	nint;
static double	hc;
static double	radConv;
static double	temp;

/* Not used (avoids compilation warnings) */
struct UserVar {
	int	dummy;
};

/* C code definitions */
# line 257 "../kohzuCtl.st"
 short kohzuCtl_LS_TEST = 0;
# line 476 "../kohzuCtl.st"
#include <string.h>
# line 477 "../kohzuCtl.st"
#include <math.h>
# line 479 "../kohzuCtl.st"
double kohzuCtl_calc2dSpacing();
# line 480 "../kohzuCtl.st"
char *kohzuCtl_pmacP;
# line 481 "../kohzuCtl.st"
char *kohzuCtl_pmacM;
# line 482 "../kohzuCtl.st"
char *kohzuCtl_pmacGEOM;

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "init" in state set "kohzuCtl" */

/* Delay function for state "init" in state set "kohzuCtl" */
static void D_kohzuCtl_init(SS_ID ssId, struct UserVar *pVar)
{
# line 497 "../kohzuCtl.st"
}

/* Event function for state "init" in state set "kohzuCtl" */
static long E_kohzuCtl_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 497 "../kohzuCtl.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "kohzuCtl" */
static void A_kohzuCtl_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 496 "../kohzuCtl.st"
			initSeq = 0;
		}
		return;
	}
}
/* Code for state "initSequence" in state set "kohzuCtl" */

/* Delay function for state "initSequence" in state set "kohzuCtl" */
static void D_kohzuCtl_initSequence(SS_ID ssId, struct UserVar *pVar)
{
# line 562 "../kohzuCtl.st"
# line 567 "../kohzuCtl.st"
# line 572 "../kohzuCtl.st"
# line 583 "../kohzuCtl.st"
	seq_delayInit(ssId, 0, (1.));
}

/* Event function for state "initSequence" in state set "kohzuCtl" */
static long E_kohzuCtl_initSequence(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 562 "../kohzuCtl.st"
	if (initSeq == 0)
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
# line 567 "../kohzuCtl.st"
	if (initSeq == 1)
	{
		*pNextState = 15;
		*pTransNum = 1;
		return TRUE;
	}
# line 572 "../kohzuCtl.st"
	if (initSeq == 2)
	{
		*pNextState = 13;
		*pTransNum = 2;
		return TRUE;
	}
# line 583 "../kohzuCtl.st"
	if ((initSeq == 3) && seq_delay(ssId, 0))
	{
		*pNextState = 9;
		*pTransNum = 3;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "initSequence" in state set "kohzuCtl" */
static void A_kohzuCtl_initSequence(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 503 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:initSequence:initSeq=0\n");
# line 503 "../kohzuCtl.st"
			hc = 12.3984244;
			radConv = 57.2958;
			thetaMotStop = 1;
			yStop = 1;
			zStop = 1;
			putVals = 0;
			seq_pvPut(ssId, 5 /* putVals */, 0);
			autoMode = 0;
			seq_pvPut(ssId, 6 /* autoMode */, 0);
			opAck = 0;
			seq_pvPut(ssId, 4 /* opAck */, 0);
# line 515 "../kohzuCtl.st"

  kohzuCtl_pmacP = seq_macValueGet(ssId,"P");
  kohzuCtl_pmacM = seq_macValueGet(ssId,"M_THETA");
  if (kohzuCtl_pmacP == 0 || kohzuCtl_pmacM == 0) {
   sprintf(thetaMotName,"??????");
  } else {
   strcpy(thetaMotName, kohzuCtl_pmacP);
   strcat(thetaMotName, kohzuCtl_pmacM);
  }
  kohzuCtl_pmacM = seq_macValueGet(ssId,"M_Y");
  if (kohzuCtl_pmacP == 0 || kohzuCtl_pmacM == 0) {
   sprintf(yMotName,"??????");
  } else {
   strcpy(yMotName, kohzuCtl_pmacP);
   strcat(yMotName, kohzuCtl_pmacM);
  }
  kohzuCtl_pmacM = seq_macValueGet(ssId,"M_Z");
  if (kohzuCtl_pmacP == 0 || kohzuCtl_pmacM == 0) {
   sprintf(zMotName,"??????");
  } else {
   strcpy(zMotName, kohzuCtl_pmacP);
   strcat(zMotName, kohzuCtl_pmacM);
  }
  kohzuCtl_pmacGEOM = seq_macValueGet(ssId,"GEOM");
  geom = 1;
  if (*kohzuCtl_pmacGEOM == '2') geom = 2;
# line 540 "../kohzuCtl.st"

# line 552 "../kohzuCtl.st"
			if (geom == 1)
			{
				yOffsetHi = 17.5 + .000001;
				seq_pvPut(ssId, 90 /* yOffsetHi */, 0);
				yOffsetLo = 17.5 - .000001;
				seq_pvPut(ssId, 91 /* yOffsetLo */, 0);
				yOffset = 17.5;
				seq_pvPut(ssId, 89 /* yOffset */, 0);
				yMotDesiredHi = 0.;
				seq_pvPut(ssId, 55 /* yMotDesiredHi */, 0);
				yMotDesiredLo = -35;
				seq_pvPut(ssId, 56 /* yMotDesiredLo */, 0);
			}
# line 557 "../kohzuCtl.st"
			else
			{
				yMotDesiredHi = 60.;
				seq_pvPut(ssId, 55 /* yMotDesiredHi */, 0);
				yMotDesiredLo = 0.;
				seq_pvPut(ssId, 56 /* yMotDesiredLo */, 0);
			}
# line 558 "../kohzuCtl.st"
			seq_pvPut(ssId, 26 /* thetaMotName */, 0);
			seq_pvPut(ssId, 27 /* yMotName */, 0);
			seq_pvPut(ssId, 28 /* zMotName */, 0);
			initSeq = 1;
		}
		return;
	case 1:
		{
# line 566 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:initSequence:initSeq=1\n");
# line 566 "../kohzuCtl.st"
			initSeq = 2;
		}
		return;
	case 2:
		{
# line 571 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:initSequence:initSeq=2\n");
# line 571 "../kohzuCtl.st"
			initSeq = 3;
		}
		return;
	case 3:
		{
# line 576 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:initSequence:initSeq=3\n");
# line 576 "../kohzuCtl.st"
			sprintf(seqMsg1, "Kohzu Control Ready");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			sprintf(seqMsg2, " ");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			initSeqDone = 1;
			theta = thetaMotRdbk;
			seq_pvPut(ssId, 22 /* theta */, 0);
		}
		return;
	}
}
/* Code for state "waitForCmndEnter" in state set "kohzuCtl" */

/* Delay function for state "waitForCmndEnter" in state set "kohzuCtl" */
static void D_kohzuCtl_waitForCmndEnter(SS_ID ssId, struct UserVar *pVar)
{
# line 637 "../kohzuCtl.st"
	seq_delayInit(ssId, 0, (.1));
# line 640 "../kohzuCtl.st"
}

/* Event function for state "waitForCmndEnter" in state set "kohzuCtl" */
static long E_kohzuCtl_waitForCmndEnter(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 637 "../kohzuCtl.st"
	if (initSeqDone && seq_delay(ssId, 0))
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
# line 640 "../kohzuCtl.st"
	if (!initSeqDone)
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "waitForCmndEnter" in state set "kohzuCtl" */
static void A_kohzuCtl_waitForCmndEnter(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 589 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:waitForCmndEnter:initSeqDone\n");
# line 589 "../kohzuCtl.st"
			seq_efClear(ssId, H_mon);
			seq_efClear(ssId, K_mon);
			seq_efClear(ssId, L_mon);
			seq_efClear(ssId, A_mon);
			seq_efClear(ssId, E_mon);
			seq_efClear(ssId, lambda_mon);
			seq_efClear(ssId, theta_mon);
# line 597 "../kohzuCtl.st"
			thetaMotRdbkEcho = thetaMotRdbk;
			seq_pvPut(ssId, 32 /* thetaMotRdbkEcho */, 0);
			yMotRdbkEcho = yMotRdbk;
			seq_pvPut(ssId, 33 /* yMotRdbkEcho */, 0);
			zMotRdbkEcho = zMotRdbk;
			seq_pvPut(ssId, 34 /* zMotRdbkEcho */, 0);
			thetaMotVelEcho = thSpeed;
			seq_pvPut(ssId, 35 /* thetaMotVelEcho */, 0);
			yMotVelEcho = ySpeed;
			seq_pvPut(ssId, 36 /* yMotVelEcho */, 0);
			zMotVelEcho = zSpeed;
			seq_pvPut(ssId, 37 /* zMotVelEcho */, 0);
			thetaMotCmdEcho = thetaMotCmdMon;
			seq_pvPut(ssId, 29 /* thetaMotCmdEcho */, 0);
			yMotCmdEcho = yMotCmdMon;
			seq_pvPut(ssId, 30 /* yMotCmdEcho */, 0);
			zMotCmdEcho = zMotCmdMon;
			seq_pvPut(ssId, 31 /* zMotCmdEcho */, 0);
			thetaDmovEcho = thetaDmov;
			seq_pvPut(ssId, 38 /* thetaDmovEcho */, 0);
			yDmovEcho = yDmov;
			seq_pvPut(ssId, 39 /* yDmovEcho */, 0);
			zDmovEcho = zDmov;
			seq_pvPut(ssId, 40 /* zDmovEcho */, 0);
# line 628 "../kohzuCtl.st"
			if (autoMode || useSetMode)
			{
				thetaRdbk = thetaMotRdbk;
				seq_pvPut(ssId, 25 /* thetaRdbk */, 0);
				lambdaRdbk = D * sin(thetaRdbk / radConv);
				seq_pvPut(ssId, 21 /* lambdaRdbk */, 0);
				ERdbk = hc / lambdaRdbk;
				seq_pvPut(ssId, 17 /* ERdbk */, 0);
			}
# line 629 "../kohzuCtl.st"
			opAck = 0;
			seq_pvPut(ssId, 4 /* opAck */, 0);
# line 632 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:waitForCmndEnter: kohzuMoving=%d; kohzuDone=%d\n", kohzuMoving, kohzuDone);
# line 636 "../kohzuCtl.st"
			if (kohzuMoving && kohzuDone)
			{
				if (kohzuCtlDebug)
printf("kohzuCtl:waitForCmndEnter: asserting 'done'.\n");
				kohzuMoving = 0;
				kohzuDone = 0;
				seq_pvPut(ssId, 8 /* kohzuMoving */, 0);
			}
		}
		return;
	case 1:
		{
		}
		return;
	}
}
/* Code for state "waitForCmnd" in state set "kohzuCtl" */

/* Delay function for state "waitForCmnd" in state set "kohzuCtl" */
static void D_kohzuCtl_waitForCmnd(SS_ID ssId, struct UserVar *pVar)
{
# line 654 "../kohzuCtl.st"
# line 664 "../kohzuCtl.st"
# line 669 "../kohzuCtl.st"
# line 673 "../kohzuCtl.st"
# line 677 "../kohzuCtl.st"
# line 681 "../kohzuCtl.st"
# line 685 "../kohzuCtl.st"
	seq_delayInit(ssId, 0, (.1));
# line 689 "../kohzuCtl.st"
# line 699 "../kohzuCtl.st"
# line 709 "../kohzuCtl.st"
}

/* Event function for state "waitForCmnd" in state set "kohzuCtl" */
static long E_kohzuCtl_waitForCmnd(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 654 "../kohzuCtl.st"
	if (seq_efTest(ssId, H_mon) || seq_efTest(ssId, K_mon) || seq_efTest(ssId, L_mon) || seq_efTest(ssId, A_mon))
	{
		*pNextState = 5;
		*pTransNum = 0;
		return TRUE;
	}
# line 664 "../kohzuCtl.st"
	if (seq_efTest(ssId, thetaMotHiLim_mon) || seq_efTest(ssId, thetaMotLoLim_mon))
	{
		*pNextState = 6;
		*pTransNum = 1;
		return TRUE;
	}
# line 669 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, thetaMotRdbk_mon))
	{
		*pNextState = 13;
		*pTransNum = 2;
		return TRUE;
	}
# line 673 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, E_mon))
	{
		*pNextState = 7;
		*pTransNum = 3;
		return TRUE;
	}
# line 677 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, lambda_mon))
	{
		*pNextState = 8;
		*pTransNum = 4;
		return TRUE;
	}
# line 681 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, theta_mon))
	{
		*pNextState = 9;
		*pTransNum = 5;
		return TRUE;
	}
# line 685 "../kohzuCtl.st"
	if (putVals && seq_delay(ssId, 0))
	{
		*pNextState = 11;
		*pTransNum = 6;
		return TRUE;
	}
# line 689 "../kohzuCtl.st"
	if (ETweakInc || ETweakDec || LTweakInc || LTweakDec || thTweakInc || thTweakDec)
	{
		*pNextState = 4;
		*pTransNum = 7;
		return TRUE;
	}
# line 699 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, yOffset_mon))
	{
		*pNextState = 10;
		*pTransNum = 8;
		return TRUE;
	}
# line 709 "../kohzuCtl.st"
	if (opAck)
	{
		*pNextState = 2;
		*pTransNum = 9;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "waitForCmnd" in state set "kohzuCtl" */
static void A_kohzuCtl_waitForCmnd(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 646 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:waitForCmnd:HKLA_mon\n");
# line 646 "../kohzuCtl.st"
			sprintf(seqMsg1, "Lattice Spacing Changed");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			sprintf(seqMsg2, " ");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			seq_efClear(ssId, H_mon);
			seq_efClear(ssId, K_mon);
			seq_efClear(ssId, L_mon);
			seq_efClear(ssId, A_mon);
		}
		return;
	case 1:
		{
# line 658 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:waitForCmnd:thetaMotXxLim_mon\n");
# line 658 "../kohzuCtl.st"
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
# line 668 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:waitForCmnd:thetaMotRdbk_mon\n");
# line 668 "../kohzuCtl.st"
			seq_efSet(ssId, thetaMotRdbkPseudo_mon);
		}
		return;
	case 3:
		{
# line 673 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:waitForCmnd:E_mon\n");
		}
		return;
	case 4:
		{
# line 677 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:waitForCmnd:lambda_mon\n");
		}
		return;
	case 5:
		{
# line 681 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:waitForCmnd:theta_mon\n");
		}
		return;
	case 6:
		{
# line 685 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:waitForCmnd:putVals\n");
		}
		return;
	case 7:
		{
# line 689 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:waitForCmnd:Tweak\n");
		}
		return;
	case 8:
		{
# line 693 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:waitForCmnd:yOffset_mon\n");
# line 693 "../kohzuCtl.st"
			autoMode = 0;
			seq_pvPut(ssId, 6 /* autoMode */, 0);
			sprintf(seqMsg1, "y offset changed to %f", yOffset);
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			sprintf(seqMsg2, "Set to Manual Mode");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
		}
		return;
	case 9:
		{
# line 703 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:waitForCmnd:opAck\n");
# line 703 "../kohzuCtl.st"
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
/* Code for state "tweak" in state set "kohzuCtl" */

/* Delay function for state "tweak" in state set "kohzuCtl" */
static void D_kohzuCtl_tweak(SS_ID ssId, struct UserVar *pVar)
{
# line 732 "../kohzuCtl.st"
# line 753 "../kohzuCtl.st"
# line 774 "../kohzuCtl.st"
}

/* Event function for state "tweak" in state set "kohzuCtl" */
static long E_kohzuCtl_tweak(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 732 "../kohzuCtl.st"
	if (ETweakInc || ETweakDec)
	{
		*pNextState = 7;
		*pTransNum = 0;
		return TRUE;
	}
# line 753 "../kohzuCtl.st"
	if (LTweakInc || LTweakDec)
	{
		*pNextState = 8;
		*pTransNum = 1;
		return TRUE;
	}
# line 774 "../kohzuCtl.st"
	if (thTweakInc || thTweakDec)
	{
		*pNextState = 9;
		*pTransNum = 2;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "tweak" in state set "kohzuCtl" */
static void A_kohzuCtl_tweak(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 715 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:tweak:E+\n");
# line 715 "../kohzuCtl.st"
			temp = E + ETweakVal * (ETweakInc ? 1 : -1);
# line 719 "../kohzuCtl.st"
			if ((temp >= ELo) && (temp <= EHi))
			{
				E = temp;
				seq_pvPut(ssId, 14 /* E */, 0);
			}
			else
			{
				sprintf(seqMsg1, "E tweak exceeds limit");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				opAlert = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
			}
# line 728 "../kohzuCtl.st"
			if (ETweakInc)
			{
				ETweakInc = 0;
				seq_pvPut(ssId, 81 /* ETweakInc */, 0);
			}
# line 731 "../kohzuCtl.st"
			else
			{
				ETweakDec = 0;
				seq_pvPut(ssId, 82 /* ETweakDec */, 0);
			}
		}
		return;
	case 1:
		{
			if (kohzuCtlDebug)
printf("kohzuCtl:tweak:L\n");
# line 736 "../kohzuCtl.st"
			temp = lambda + LTweakVal * (LTweakInc ? 1 : -1);
# line 740 "../kohzuCtl.st"
			if ((temp >= lambdaLo) && (temp <= lambdaHi))
			{
				lambda = temp;
				seq_pvPut(ssId, 18 /* lambda */, 0);
			}
			else
			{
				sprintf(seqMsg1, "Lambda tweak exceeds limit");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				opAlert = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
			}
# line 749 "../kohzuCtl.st"
			if (LTweakInc)
			{
				LTweakInc = 0;
				seq_pvPut(ssId, 84 /* LTweakInc */, 0);
			}
# line 752 "../kohzuCtl.st"
			else
			{
				LTweakDec = 0;
				seq_pvPut(ssId, 85 /* LTweakDec */, 0);
			}
		}
		return;
	case 2:
		{
			if (kohzuCtlDebug)
printf("kohzuCtl:tweak:th\n");
# line 757 "../kohzuCtl.st"
			temp = theta + thTweakVal * (thTweakInc ? 1 : -1);
# line 761 "../kohzuCtl.st"
			if ((temp >= thetaLo) && (temp <= thetaHi))
			{
				theta = temp;
				seq_pvPut(ssId, 22 /* theta */, 0);
			}
			else
			{
				sprintf(seqMsg1, "Theta tweak exceeds limit");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				opAlert = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
			}
# line 770 "../kohzuCtl.st"
			if (thTweakInc)
			{
				thTweakInc = 0;
				seq_pvPut(ssId, 87 /* thTweakInc */, 0);
			}
# line 773 "../kohzuCtl.st"
			else
			{
				thTweakDec = 0;
				seq_pvPut(ssId, 88 /* thTweakDec */, 0);
			}
		}
		return;
	}
}
/* Code for state "dInputChanged" in state set "kohzuCtl" */

/* Delay function for state "dInputChanged" in state set "kohzuCtl" */
static void D_kohzuCtl_dInputChanged(SS_ID ssId, struct UserVar *pVar)
{
# line 796 "../kohzuCtl.st"
}

/* Event function for state "dInputChanged" in state set "kohzuCtl" */
static long E_kohzuCtl_dInputChanged(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 796 "../kohzuCtl.st"
	if (TRUE)
	{
		*pNextState = 9;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "dInputChanged" in state set "kohzuCtl" */
static void A_kohzuCtl_dInputChanged(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 780 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:dInputChanged\n");
# line 780 "../kohzuCtl.st"
			autoMode = 0;
			seq_pvPut(ssId, 6 /* autoMode */, 0);
			sprintf(seqMsg2, "Set to Manual Mode");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			D = kohzuCtl_calc2dSpacing();
			seq_pvPut(ssId, 13 /* D */, 0);
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			lambdaHi = D * sin(thetaHi / radConv);
			seq_pvPut(ssId, 19 /* lambdaHi */, 0);
			lambdaLo = D * sin(thetaLo / radConv);
			seq_pvPut(ssId, 20 /* lambdaLo */, 0);
			EHi = hc / lambdaLo;
			seq_pvPut(ssId, 15 /* EHi */, 0);
			ELo = hc / lambdaHi;
			seq_pvPut(ssId, 16 /* ELo */, 0);
			seq_pvPut(ssId, 3 /* opAlert */, 0);
		}
		return;
	}
}
/* Code for state "thetaLimits" in state set "kohzuCtl" */

/* Delay function for state "thetaLimits" in state set "kohzuCtl" */
static void D_kohzuCtl_thetaLimits(SS_ID ssId, struct UserVar *pVar)
{
# line 824 "../kohzuCtl.st"
}

/* Event function for state "thetaLimits" in state set "kohzuCtl" */
static long E_kohzuCtl_thetaLimits(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 824 "../kohzuCtl.st"
	if (TRUE)
	{
		*pNextState = 12;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "thetaLimits" in state set "kohzuCtl" */
static void A_kohzuCtl_thetaLimits(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 802 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:thetaLimits\n");
# line 802 "../kohzuCtl.st"
			thetaHi = thetaMotHiLim;
			thetaLo = thetaMotLoLim;
# line 808 "../kohzuCtl.st"
			if (thetaHi > 89.0)
			{
				thetaHi = 89.0;
				sprintf(seqMsg2, "ThetaHiLim > 89 deg. (Using 89 deg.)");
				seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			}
# line 813 "../kohzuCtl.st"
			if (thetaLo < 1.0)
			{
				thetaLo = 1.0;
				sprintf(seqMsg2, "ThetaLoLim < 1 deg. (Using 1 deg.)");
				seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			}
# line 814 "../kohzuCtl.st"
			seq_pvPut(ssId, 23 /* thetaHi */, 0);
			seq_pvPut(ssId, 24 /* thetaLo */, 0);
			lambdaHi = D * sin(thetaHi / radConv);
			seq_pvPut(ssId, 19 /* lambdaHi */, 0);
			lambdaLo = D * sin(thetaLo / radConv);
			seq_pvPut(ssId, 20 /* lambdaLo */, 0);
			EHi = hc / lambdaLo;
			seq_pvPut(ssId, 15 /* EHi */, 0);
			ELo = hc / lambdaHi;
			seq_pvPut(ssId, 16 /* ELo */, 0);
		}
		return;
	}
}
/* Code for state "eChanged" in state set "kohzuCtl" */

/* Delay function for state "eChanged" in state set "kohzuCtl" */
static void D_kohzuCtl_eChanged(SS_ID ssId, struct UserVar *pVar)
{
# line 832 "../kohzuCtl.st"
}

/* Event function for state "eChanged" in state set "kohzuCtl" */
static long E_kohzuCtl_eChanged(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 832 "../kohzuCtl.st"
	if (TRUE)
	{
		*pNextState = 8;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "eChanged" in state set "kohzuCtl" */
static void A_kohzuCtl_eChanged(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 830 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:eChanged\n");
# line 830 "../kohzuCtl.st"
			lambda = hc / E;
			seq_pvPut(ssId, 18 /* lambda */, 0);
		}
		return;
	}
}
/* Code for state "lChanged" in state set "kohzuCtl" */

/* Delay function for state "lChanged" in state set "kohzuCtl" */
static void D_kohzuCtl_lChanged(SS_ID ssId, struct UserVar *pVar)
{
# line 843 "../kohzuCtl.st"
# line 849 "../kohzuCtl.st"
}

/* Event function for state "lChanged" in state set "kohzuCtl" */
static long E_kohzuCtl_lChanged(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 843 "../kohzuCtl.st"
	if (lambda > D)
	{
		*pNextState = 9;
		*pTransNum = 0;
		return TRUE;
	}
# line 849 "../kohzuCtl.st"
	if (TRUE)
	{
		*pNextState = 9;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "lChanged" in state set "kohzuCtl" */
static void A_kohzuCtl_lChanged(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 838 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:lChanged: lambda>D\n");
# line 839 "../kohzuCtl.st"
			sprintf(seqMsg1, "Wavelength > 2d spacing.");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			opAlert = 1;
			seq_pvPut(ssId, 3 /* opAlert */, 0);
		}
		return;
	case 1:
		{
# line 847 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:lChanged\n");
# line 847 "../kohzuCtl.st"
			theta = radConv * asin(lambda / D);
			seq_pvPut(ssId, 22 /* theta */, 0);
		}
		return;
	}
}
/* Code for state "thChanged" in state set "kohzuCtl" */

/* Delay function for state "thChanged" in state set "kohzuCtl" */
static void D_kohzuCtl_thChanged(SS_ID ssId, struct UserVar *pVar)
{
# line 877 "../kohzuCtl.st"
}

/* Event function for state "thChanged" in state set "kohzuCtl" */
static long E_kohzuCtl_thChanged(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 877 "../kohzuCtl.st"
	if (TRUE)
	{
		*pNextState = 10;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "thChanged" in state set "kohzuCtl" */
static void A_kohzuCtl_thChanged(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 855 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:thChanged\n");
# line 864 "../kohzuCtl.st"
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
# line 865 "../kohzuCtl.st"
			lambda = D * sin(theta / radConv);
			seq_pvPut(ssId, 18 /* lambda */, 0);
			E = hc / lambda;
			seq_pvPut(ssId, 14 /* E */, 0);
# line 871 "../kohzuCtl.st"
			thetaRdbk = thetaMotRdbk;
			lambdaRdbk = D * sin(thetaRdbk / radConv);
			ERdbk = hc / lambdaRdbk;
			seq_pvPut(ssId, 25 /* thetaRdbk */, 0);
			seq_pvPut(ssId, 21 /* lambdaRdbk */, 0);
			seq_pvPut(ssId, 17 /* ERdbk */, 0);
		}
		return;
	}
}
/* Code for state "calcMovements" in state set "kohzuCtl" */

/* Delay function for state "calcMovements" in state set "kohzuCtl" */
static void D_kohzuCtl_calcMovements(SS_ID ssId, struct UserVar *pVar)
{
# line 953 "../kohzuCtl.st"
}

/* Event function for state "calcMovements" in state set "kohzuCtl" */
static long E_kohzuCtl_calcMovements(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 953 "../kohzuCtl.st"
	if (TRUE)
	{
		*pNextState = 11;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "calcMovements" in state set "kohzuCtl" */
static void A_kohzuCtl_calcMovements(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 883 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:calcMovements\n");
			thetaMotDesired = theta;
# line 889 "../kohzuCtl.st"
			if (geom == 1)
			{
				yMotDesired = -yOffset / cos(theta / radConv);
				zMotDesired = yOffset / sin(theta / radConv);
			}
# line 892 "../kohzuCtl.st"
			else
			{
				yMotDesired = yOffset / (2 * cos(theta / radConv));
				zMotDesired = yOffset / (2 * sin(theta / radConv));
			}
# line 903 "../kohzuCtl.st"
			if (ccMode != 1 && ccMode != 3 && ((yMotDesired < yMotLoLim) || (yMotDesired > yMotHiLim)))
			{
				sprintf(seqMsg1, "Y will exceed soft limits");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				sprintf(seqMsg2, "Setting to Manual Mode");
				seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
				opAlert = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
				autoMode = 0;
				seq_pvPut(ssId, 6 /* autoMode */, 0);
			}
			if (ccMode != 1 && ccMode != 2 && ((zMotDesired < zMotLoLim) || (zMotDesired > zMotHiLim)))
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
# line 915 "../kohzuCtl.st"
			seq_pvPut(ssId, 53 /* thetaMotDesired */, 0);
			seq_pvPut(ssId, 54 /* yMotDesired */, 0);
			seq_pvPut(ssId, 57 /* zMotDesired */, 0);
# line 920 "../kohzuCtl.st"
			oldThSpeed = thSpeed;
			oldYSpeed = ySpeed;
			oldZSpeed = zSpeed;
			thTime = fabs(theta - thetaMotRdbk) / thSpeed;
			yTime = fabs(yMotDesired - yMotRdbk) / ySpeed;
			zTime = fabs(zMotDesired - zMotRdbk) / zSpeed;
# line 931 "../kohzuCtl.st"
			if (ccMode == 1)
			{
				yTime = 0.0;
				zTime = 0.0;
			}
# line 934 "../kohzuCtl.st"
			else
			if (ccMode == 2)
			{
				zTime = 0.0;
			}
# line 937 "../kohzuCtl.st"
			else
			if (ccMode == 3)
			{
				yTime = 0.0;
			}
# line 943 "../kohzuCtl.st"
			if ((thTime >= yTime) && (thTime >= zTime))
			{
				newThSpeed = thSpeed;
				newYSpeed = fabs(yMotDesired - yMotRdbk) / thTime;
				newZSpeed = fabs(zMotDesired - zMotRdbk) / thTime;
			}
# line 947 "../kohzuCtl.st"
			else
			if ((yTime >= thTime) && (yTime >= zTime))
			{
				newThSpeed = fabs(theta - thetaMotRdbk) / yTime;
				newYSpeed = ySpeed;
				newZSpeed = fabs(zMotDesired - zMotRdbk) / yTime;
			}
# line 951 "../kohzuCtl.st"
			else
			{
				newThSpeed = fabs(theta - thetaMotRdbk) / zTime;
				newYSpeed = fabs(yMotDesired - yMotRdbk) / zTime;
				newZSpeed = zSpeed;
			}
		}
		return;
	}
}
/* Code for state "moveKohzu" in state set "kohzuCtl" */

/* Delay function for state "moveKohzu" in state set "kohzuCtl" */
static void D_kohzuCtl_moveKohzu(SS_ID ssId, struct UserVar *pVar)
{
# line 986 "../kohzuCtl.st"
# line 990 "../kohzuCtl.st"
}

/* Event function for state "moveKohzu" in state set "kohzuCtl" */
static long E_kohzuCtl_moveKohzu(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 986 "../kohzuCtl.st"
	if (autoMode || putVals || useSetMode)
	{
		*pNextState = 13;
		*pTransNum = 0;
		return TRUE;
	}
# line 990 "../kohzuCtl.st"
	if (1)
	{
		*pNextState = 2;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "moveKohzu" in state set "kohzuCtl" */
static void A_kohzuCtl_moveKohzu(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 959 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:moveKohzu:auto|put\n");
# line 959 "../kohzuCtl.st"
			thSpeed = newThSpeed;
			ySpeed = newYSpeed;
			zSpeed = newZSpeed;
			seq_pvPut(ssId, 70 /* thSpeed */, 0);
# line 965 "../kohzuCtl.st"
			if (ccMode == 0 || ccMode == 2)
			{
				seq_pvPut(ssId, 71 /* ySpeed */, 0);
			}
# line 968 "../kohzuCtl.st"
			if (ccMode == 0 || ccMode == 3)
			{
				seq_pvPut(ssId, 72 /* zSpeed */, 0);
			}
# line 969 "../kohzuCtl.st"
			thetaMotCmd = thetaMotDesired;
			yMotCmd = yMotDesired;
			zMotCmd = zMotDesired;
			seq_pvPut(ssId, 64 /* thetaMotCmd */, 0);
# line 976 "../kohzuCtl.st"
			if (ccMode == 0)
			{
				seq_pvPut(ssId, 65 /* yMotCmd */, 0);
				seq_pvPut(ssId, 66 /* zMotCmd */, 0);
			}
# line 978 "../kohzuCtl.st"
			else
			if (ccMode == 1)
			{
				;
			}
# line 980 "../kohzuCtl.st"
			else
			if (ccMode == 2)
			{
				seq_pvPut(ssId, 65 /* yMotCmd */, 0);
			}
# line 982 "../kohzuCtl.st"
			else
			if (ccMode == 3)
			{
				seq_pvPut(ssId, 66 /* zMotCmd */, 0);
			}
# line 983 "../kohzuCtl.st"
			putVals = 0;
			seq_pvPut(ssId, 5 /* putVals */, 0);
			causedMove = 1;
		}
		return;
	case 1:
		{
# line 990 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:moveKohzu\n");
		}
		return;
	}
}
/* Code for state "checkDone" in state set "kohzuCtl" */

/* Delay function for state "checkDone" in state set "kohzuCtl" */
static void D_kohzuCtl_checkDone(SS_ID ssId, struct UserVar *pVar)
{
# line 997 "../kohzuCtl.st"
# line 1001 "../kohzuCtl.st"
}

/* Event function for state "checkDone" in state set "kohzuCtl" */
static long E_kohzuCtl_checkDone(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 997 "../kohzuCtl.st"
	if (thetaDmov && yDmov && zDmov)
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 1001 "../kohzuCtl.st"
	if (!thetaDmov || !yDmov || !zDmov)
	{
		*pNextState = 13;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "checkDone" in state set "kohzuCtl" */
static void A_kohzuCtl_checkDone(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 996 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:checkDone: done\n");
# line 996 "../kohzuCtl.st"
			kohzuDone = 1;
		}
		return;
	case 1:
		{
# line 1001 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:checkDone: not done\n");
		}
		return;
	}
}
/* Code for state "updateRdbk" in state set "kohzuCtl" */

/* Delay function for state "updateRdbk" in state set "kohzuCtl" */
static void D_kohzuCtl_updateRdbk(SS_ID ssId, struct UserVar *pVar)
{
# line 1014 "../kohzuCtl.st"
# line 1028 "../kohzuCtl.st"
# line 1041 "../kohzuCtl.st"
# line 1054 "../kohzuCtl.st"
# line 1067 "../kohzuCtl.st"
# line 1077 "../kohzuCtl.st"
	seq_delayInit(ssId, 0, (.1));
}

/* Event function for state "updateRdbk" in state set "kohzuCtl" */
static long E_kohzuCtl_updateRdbk(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1014 "../kohzuCtl.st"
	if (thetaMotRdbk == 0)
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 1028 "../kohzuCtl.st"
	if (thetaDmov && yDmov && zDmov)
	{
		*pNextState = 14;
		*pTransNum = 1;
		return TRUE;
	}
# line 1041 "../kohzuCtl.st"
	if (thetaHls || thetaLls)
	{
		*pNextState = 17;
		*pTransNum = 2;
		return TRUE;
	}
# line 1054 "../kohzuCtl.st"
	if ((ccMode != 1) && (ccMode != 3) && (yHls || yLls))
	{
		*pNextState = 17;
		*pTransNum = 3;
		return TRUE;
	}
# line 1067 "../kohzuCtl.st"
	if ((ccMode != 1) && (ccMode != 2) && (zHls || zLls))
	{
		*pNextState = 17;
		*pTransNum = 4;
		return TRUE;
	}
# line 1077 "../kohzuCtl.st"
	if ((!thetaDmov || !yDmov || !zDmov) && seq_delay(ssId, 0))
	{
		*pNextState = 13;
		*pTransNum = 5;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "updateRdbk" in state set "kohzuCtl" */
static void A_kohzuCtl_updateRdbk(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1008 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:updateRdbk:thetaMotRdbk==0\n");
# line 1008 "../kohzuCtl.st"
			sprintf(seqMsg1, "Theta Motor Readback is 0 !");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
			sprintf(seqMsg2, "Monochromator stopped");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
			opAlert = 1;
			seq_pvPut(ssId, 3 /* opAlert */, 0);
		}
		return;
	case 1:
		{
# line 1018 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:updateRdbk:*Dmov (done=%d,moving=%d)\n", kohzuDone, kohzuMoving);
# line 1018 "../kohzuCtl.st"
			thetaRdbk = thetaMotRdbk;
			lambdaRdbk = D * sin(thetaRdbk / radConv);
			ERdbk = hc / lambdaRdbk;
			seq_pvPut(ssId, 25 /* thetaRdbk */, 0);
			seq_pvPut(ssId, 21 /* lambdaRdbk */, 0);
			seq_pvPut(ssId, 17 /* ERdbk */, 0);
			kohzuDone = 1;
			seq_efSet(ssId, thetaMotRdbkPseudo_mon);
			seq_efClear(ssId, thetaMotRdbk_mon);
# line 1028 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:updateRdbk: done\n");
		}
		return;
	case 2:
		{
# line 1033 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:updateRdbk: Th limit switch\n");
# line 1033 "../kohzuCtl.st"
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
# line 1046 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:updateRdbk: Y limit switch\n");
# line 1046 "../kohzuCtl.st"
			sprintf(seqMsg1, "Y Motor hit a limit switch!");
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
# line 1059 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:updateRdbk: Z limit switch\n");
# line 1059 "../kohzuCtl.st"
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
	case 5:
		{
# line 1071 "../kohzuCtl.st"
			if (kohzuCtlDebug > 1)
printf("kohzuCtl:updateRdbk:moving\n");
# line 1071 "../kohzuCtl.st"
			thetaRdbk = thetaMotRdbk;
			lambdaRdbk = D * sin(thetaRdbk / radConv);
			ERdbk = hc / lambdaRdbk;
			seq_pvPut(ssId, 25 /* thetaRdbk */, 0);
			seq_pvPut(ssId, 21 /* lambdaRdbk */, 0);
			seq_pvPut(ssId, 17 /* ERdbk */, 0);
		}
		return;
	}
}
/* Code for state "thetaMotStopped" in state set "kohzuCtl" */

/* Delay function for state "thetaMotStopped" in state set "kohzuCtl" */
static void D_kohzuCtl_thetaMotStopped(SS_ID ssId, struct UserVar *pVar)
{
# line 1091 "../kohzuCtl.st"
# line 1112 "../kohzuCtl.st"
}

/* Event function for state "thetaMotStopped" in state set "kohzuCtl" */
static long E_kohzuCtl_thetaMotStopped(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1091 "../kohzuCtl.st"
	if (causedMove)
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 1112 "../kohzuCtl.st"
	if (!causedMove)
	{
		*pNextState = 9;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "thetaMotStopped" in state set "kohzuCtl" */
static void A_kohzuCtl_thetaMotStopped(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1083 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:thetaMotStopped:causedMove (done=%d,moving=%d)\n", kohzuDone, kohzuMoving);
# line 1083 "../kohzuCtl.st"
			thSpeed = oldThSpeed;
			ySpeed = oldYSpeed;
			zSpeed = oldZSpeed;
			seq_pvPut(ssId, 70 /* thSpeed */, 0);
			seq_pvPut(ssId, 71 /* ySpeed */, 0);
# line 1089 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:thetaMotStopped: ySpeed = %f\n", ySpeed);
# line 1089 "../kohzuCtl.st"
			seq_pvPut(ssId, 72 /* zSpeed */, 0);
			causedMove = 0;
		}
		return;
	case 1:
		{
# line 1096 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:thetaMotStopped:!causedMove (done=%d,moving=%d)\n", kohzuDone, kohzuMoving);
# line 1105 "../kohzuCtl.st"
			if (initSeqDone)
			{
				opAlert = 1;
				seq_pvPut(ssId, 3 /* opAlert */, 0);
				sprintf(seqMsg1, "Only Theta Motor Moved");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				sprintf(seqMsg2, "Hit MOVE to Adjust Y & Z");
				seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
				autoMode = 0;
				seq_pvPut(ssId, 6 /* autoMode */, 0);
			}
# line 1106 "../kohzuCtl.st"
			theta = thetaMotRdbk;
			seq_pvPut(ssId, 22 /* theta */, 0);
			lambda = D * sin(theta / radConv);
			seq_pvPut(ssId, 18 /* lambda */, 0);
			E = hc / lambda;
			seq_pvPut(ssId, 14 /* E */, 0);
		}
		return;
	}
}
/* Code for state "chkMotorLimits" in state set "kohzuCtl" */

/* Delay function for state "chkMotorLimits" in state set "kohzuCtl" */
static void D_kohzuCtl_chkMotorLimits(SS_ID ssId, struct UserVar *pVar)
{
# line 1167 "../kohzuCtl.st"
}

/* Event function for state "chkMotorLimits" in state set "kohzuCtl" */
static long E_kohzuCtl_chkMotorLimits(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1167 "../kohzuCtl.st"
	if (TRUE)
	{
		*pNextState = 16;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "chkMotorLimits" in state set "kohzuCtl" */
static void A_kohzuCtl_chkMotorLimits(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1118 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:chkMotorLimits\n");
# line 1118 "../kohzuCtl.st"
			opAlert = 0;
			thetaRdbk = thetaMotRdbk;
			lambdaRdbk = D * sin(thetaRdbk / radConv);
			ERdbk = hc / lambdaRdbk;
			seq_pvPut(ssId, 25 /* thetaRdbk */, 0);
			seq_pvPut(ssId, 21 /* lambdaRdbk */, 0);
			seq_pvPut(ssId, 17 /* ERdbk */, 0);
# line 1137 "../kohzuCtl.st"
			if (ccMode != 1 && ccMode != 3)
			{
				if (yMotRdbk == 0)
				{
					sprintf(seqMsg1, "Y Readback of 0 not reasonable");
					seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
					opAlert = 1;
				}
				if ((yMotRdbk < yMotLoLim) || (yMotRdbk > yMotHiLim))
				{
					sprintf(seqMsg1, "Y Readback outside Motor Limits");
					seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
					opAlert = 1;
				}
			}
# line 1149 "../kohzuCtl.st"
			if (ccMode != 1 && ccMode != 2)
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
# line 1154 "../kohzuCtl.st"
			if (thetaMotRdbk == 0)
			{
				sprintf(seqMsg1, "Theta Readback of 0 not reasonable");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				opAlert = 1;
			}
# line 1159 "../kohzuCtl.st"
			if ((thetaMotRdbk < thetaMotLoLim) || (thetaMotRdbk > thetaMotHiLim))
			{
				sprintf(seqMsg1, "Theta Readback outside Motor Limits");
				seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
				opAlert = 1;
			}
# line 1166 "../kohzuCtl.st"
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
/* Code for state "chkMotorLimitsExit" in state set "kohzuCtl" */

/* Delay function for state "chkMotorLimitsExit" in state set "kohzuCtl" */
static void D_kohzuCtl_chkMotorLimitsExit(SS_ID ssId, struct UserVar *pVar)
{
# line 1176 "../kohzuCtl.st"
# line 1183 "../kohzuCtl.st"
	seq_delayInit(ssId, 0, (2.));
}

/* Event function for state "chkMotorLimitsExit" in state set "kohzuCtl" */
static long E_kohzuCtl_chkMotorLimitsExit(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1176 "../kohzuCtl.st"
	if (!opAlert)
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 1183 "../kohzuCtl.st"
	if (opAlert && seq_delay(ssId, 0))
	{
		*pNextState = 15;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "chkMotorLimitsExit" in state set "kohzuCtl" */
static void A_kohzuCtl_chkMotorLimitsExit(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1173 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:chkMotorLimitsExit: OK\n");
# line 1173 "../kohzuCtl.st"
			seq_pvPut(ssId, 3 /* opAlert */, 0);
			sprintf(seqMsg1, "All motors within limits");
			seq_pvPut(ssId, 1 /* seqMsg1 */, 0);
		}
		return;
	case 1:
		{
# line 1180 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:chkMotorLimitsExit: opAlert\n");
# line 1180 "../kohzuCtl.st"
			seq_pvPut(ssId, 3 /* opAlert */, 0);
			sprintf(seqMsg2, "Modify Motor Positions/Limits");
			seq_pvPut(ssId, 2 /* seqMsg2 */, 0);
		}
		return;
	}
}
/* Code for state "stopKohzu" in state set "kohzuCtl" */

/* Delay function for state "stopKohzu" in state set "kohzuCtl" */
static void D_kohzuCtl_stopKohzu(SS_ID ssId, struct UserVar *pVar)
{
# line 1192 "../kohzuCtl.st"
}

/* Event function for state "stopKohzu" in state set "kohzuCtl" */
static long E_kohzuCtl_stopKohzu(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1192 "../kohzuCtl.st"
	if (TRUE)
	{
		*pNextState = 18;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "stopKohzu" in state set "kohzuCtl" */
static void A_kohzuCtl_stopKohzu(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1189 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:stopKohzu\n");
# line 1189 "../kohzuCtl.st"
			seq_pvPut(ssId, 41 /* thetaMotStop */, 0);
			seq_pvPut(ssId, 42 /* yStop */, 0);
			seq_pvPut(ssId, 43 /* zStop */, 0);
		}
		return;
	}
}
/* Code for state "stoppedKohzuWait" in state set "kohzuCtl" */

/* Delay function for state "stoppedKohzuWait" in state set "kohzuCtl" */
static void D_kohzuCtl_stoppedKohzuWait(SS_ID ssId, struct UserVar *pVar)
{
# line 1198 "../kohzuCtl.st"
	seq_delayInit(ssId, 0, (1.));
}

/* Event function for state "stoppedKohzuWait" in state set "kohzuCtl" */
static long E_kohzuCtl_stoppedKohzuWait(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1198 "../kohzuCtl.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 13;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "stoppedKohzuWait" in state set "kohzuCtl" */
static void A_kohzuCtl_stoppedKohzuWait(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 1198 "../kohzuCtl.st"
			if (kohzuCtlDebug)
printf("kohzuCtl:stoppedKohzuWait\n");
		}
		return;
	}
}
/* Code for state "init" in state set "updatePsuedo" */

/* Delay function for state "init" in state set "updatePsuedo" */
static void D_updatePsuedo_init(SS_ID ssId, struct UserVar *pVar)
{
# line 1212 "../kohzuCtl.st"
}

/* Event function for state "init" in state set "updatePsuedo" */
static long E_updatePsuedo_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1212 "../kohzuCtl.st"
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
# line 1219 "../kohzuCtl.st"
# line 1224 "../kohzuCtl.st"
# line 1229 "../kohzuCtl.st"
# line 1234 "../kohzuCtl.st"
# line 1239 "../kohzuCtl.st"
# line 1244 "../kohzuCtl.st"
# line 1249 "../kohzuCtl.st"
# line 1254 "../kohzuCtl.st"
# line 1259 "../kohzuCtl.st"
# line 1264 "../kohzuCtl.st"
# line 1269 "../kohzuCtl.st"
# line 1274 "../kohzuCtl.st"
}

/* Event function for state "update" in state set "updatePsuedo" */
static long E_updatePsuedo_update(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1219 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, thetaMotRdbkPseudo_mon))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 1224 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, yMotRdbk_mon))
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
# line 1229 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, zMotRdbk_mon))
	{
		*pNextState = 1;
		*pTransNum = 2;
		return TRUE;
	}
# line 1234 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, thetaMotVel_mon))
	{
		*pNextState = 1;
		*pTransNum = 3;
		return TRUE;
	}
# line 1239 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, yMotVel_mon))
	{
		*pNextState = 1;
		*pTransNum = 4;
		return TRUE;
	}
# line 1244 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, zMotVel_mon))
	{
		*pNextState = 1;
		*pTransNum = 5;
		return TRUE;
	}
# line 1249 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, thetaMotCmdMon_mon))
	{
		*pNextState = 1;
		*pTransNum = 6;
		return TRUE;
	}
# line 1254 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, yMotCmdMon_mon))
	{
		*pNextState = 1;
		*pTransNum = 7;
		return TRUE;
	}
# line 1259 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, zMotCmdMon_mon))
	{
		*pNextState = 1;
		*pTransNum = 8;
		return TRUE;
	}
# line 1264 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, thetaDmov_mon))
	{
		*pNextState = 1;
		*pTransNum = 9;
		return TRUE;
	}
# line 1269 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, yDmov_mon))
	{
		*pNextState = 1;
		*pTransNum = 10;
		return TRUE;
	}
# line 1274 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, zDmov_mon))
	{
		*pNextState = 1;
		*pTransNum = 11;
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
# line 1217 "../kohzuCtl.st"
			thetaMotRdbkEcho = thetaMotRdbk;
			seq_pvPut(ssId, 32 /* thetaMotRdbkEcho */, 0);
		}
		return;
	case 1:
		{
# line 1222 "../kohzuCtl.st"
			yMotRdbkEcho = yMotRdbk;
			seq_pvPut(ssId, 33 /* yMotRdbkEcho */, 0);
		}
		return;
	case 2:
		{
# line 1227 "../kohzuCtl.st"
			zMotRdbkEcho = zMotRdbk;
			seq_pvPut(ssId, 34 /* zMotRdbkEcho */, 0);
		}
		return;
	case 3:
		{
# line 1232 "../kohzuCtl.st"
			thetaMotVelEcho = thSpeed;
			seq_pvPut(ssId, 35 /* thetaMotVelEcho */, 0);
		}
		return;
	case 4:
		{
# line 1237 "../kohzuCtl.st"
			yMotVelEcho = ySpeed;
			seq_pvPut(ssId, 36 /* yMotVelEcho */, 0);
		}
		return;
	case 5:
		{
# line 1242 "../kohzuCtl.st"
			zMotVelEcho = zSpeed;
			seq_pvPut(ssId, 37 /* zMotVelEcho */, 0);
		}
		return;
	case 6:
		{
# line 1247 "../kohzuCtl.st"
			thetaMotCmdEcho = thetaMotCmdMon;
			seq_pvPut(ssId, 29 /* thetaMotCmdEcho */, 0);
		}
		return;
	case 7:
		{
# line 1252 "../kohzuCtl.st"
			yMotCmdEcho = yMotCmdMon;
			seq_pvPut(ssId, 30 /* yMotCmdEcho */, 0);
		}
		return;
	case 8:
		{
# line 1257 "../kohzuCtl.st"
			zMotCmdEcho = zMotCmdMon;
			seq_pvPut(ssId, 31 /* zMotCmdEcho */, 0);
		}
		return;
	case 9:
		{
# line 1262 "../kohzuCtl.st"
			thetaDmovEcho = thetaDmov;
			seq_pvPut(ssId, 38 /* thetaDmovEcho */, 0);
		}
		return;
	case 10:
		{
# line 1267 "../kohzuCtl.st"
			yDmovEcho = yDmov;
			seq_pvPut(ssId, 39 /* yDmovEcho */, 0);
		}
		return;
	case 11:
		{
# line 1272 "../kohzuCtl.st"
			zDmovEcho = zDmov;
			seq_pvPut(ssId, 40 /* zDmovEcho */, 0);
		}
		return;
	}
}
/* Code for state "init" in state set "updateSet" */

/* Delay function for state "init" in state set "updateSet" */
static void D_updateSet_init(SS_ID ssId, struct UserVar *pVar)
{
# line 1287 "../kohzuCtl.st"
}

/* Event function for state "init" in state set "updateSet" */
static long E_updateSet_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1287 "../kohzuCtl.st"
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
# line 1304 "../kohzuCtl.st"
# line 1311 "../kohzuCtl.st"
# line 1318 "../kohzuCtl.st"
# line 1325 "../kohzuCtl.st"
}

/* Event function for state "update" in state set "updateSet" */
static long E_updateSet_update(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 1304 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, useSetMode_mon))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 1311 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, thetaMotSet_mon))
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
# line 1318 "../kohzuCtl.st"
	if (seq_efTestAndClear(ssId, yMotSet_mon))
	{
		*pNextState = 1;
		*pTransNum = 2;
		return TRUE;
	}
# line 1325 "../kohzuCtl.st"
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
# line 1295 "../kohzuCtl.st"
			if (thetaMotSet != useSetMode)
			{
				thetaMotSet = useSetMode;
				seq_pvPut(ssId, 77 /* thetaMotSet */, 0);
			}
# line 1299 "../kohzuCtl.st"
			if (yMotSet != useSetMode)
			{
				yMotSet = useSetMode;
				seq_pvPut(ssId, 78 /* yMotSet */, 0);
			}
# line 1303 "../kohzuCtl.st"
			if (zMotSet != useSetMode)
			{
				zMotSet = useSetMode;
				seq_pvPut(ssId, 79 /* zMotSet */, 0);
			}
		}
		return;
	case 1:
		{
# line 1310 "../kohzuCtl.st"
			if (useSetMode != thetaMotSet)
			{
				useSetMode = thetaMotSet;
				seq_pvPut(ssId, 76 /* useSetMode */, 0);
			}
		}
		return;
	case 2:
		{
# line 1317 "../kohzuCtl.st"
			if (useSetMode != yMotSet)
			{
				useSetMode = yMotSet;
				seq_pvPut(ssId, 76 /* useSetMode */, 0);
			}
		}
		return;
	case 3:
		{
# line 1324 "../kohzuCtl.st"
			if (useSetMode != zMotSet)
			{
				useSetMode = zMotSet;
				seq_pvPut(ssId, 76 /* useSetMode */, 0);
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
  {"{P}KohzuCtlDebug", (void *)&kohzuCtlDebug, "kohzuCtlDebug", 
    "short", 1, 28, 0, 1, 0, 0, 0},

  {"{P}KohzuSeqMsg1SI", (void *)&seqMsg1[0], "seqMsg1", 
    "string", 1, 29, 0, 0, 0, 0, 0},

  {"{P}KohzuSeqMsg2SI", (void *)&seqMsg2[0], "seqMsg2", 
    "string", 1, 30, 0, 0, 0, 0, 0},

  {"{P}KohzuAlertBO", (void *)&opAlert, "opAlert", 
    "short", 1, 31, 0, 0, 0, 0, 0},

  {"{P}KohzuOperAckBO", (void *)&opAck, "opAck", 
    "short", 1, 32, 0, 1, 0, 0, 0},

  {"{P}KohzuPutBO", (void *)&putVals, "putVals", 
    "short", 1, 33, 0, 1, 0, 0, 0},

  {"{P}KohzuModeBO", (void *)&autoMode, "autoMode", 
    "short", 1, 34, 0, 1, 0, 0, 0},

  {"{P}KohzuMode2MO", (void *)&ccMode, "ccMode", 
    "short", 1, 35, 0, 1, 0, 0, 0},

  {"{P}KohzuMoving", (void *)&kohzuMoving, "kohzuMoving", 
    "short", 1, 36, 0, 1, 0, 0, 0},

  {"{P}BraggHAO", (void *)&H, "H", 
    "double", 1, 37, 1, 1, 0, 0, 0},

  {"{P}BraggKAO", (void *)&K, "K", 
    "double", 1, 38, 2, 1, 0, 0, 0},

  {"{P}BraggLAO", (void *)&L, "L", 
    "double", 1, 39, 3, 1, 0, 0, 0},

  {"{P}BraggAAO", (void *)&A, "A", 
    "double", 1, 40, 4, 1, 0, 0, 0},

  {"{P}Bragg2dSpacingAO", (void *)&D, "D", 
    "double", 1, 41, 0, 0, 0, 0, 0},

  {"{P}BraggEAO", (void *)&E, "E", 
    "double", 1, 42, 5, 1, 0, 0, 0},

  {"{P}BraggEAO.DRVH", (void *)&EHi, "EHi", 
    "double", 1, 43, 0, 0, 0, 0, 0},

  {"{P}BraggEAO.DRVL", (void *)&ELo, "ELo", 
    "double", 1, 44, 0, 0, 0, 0, 0},

  {"{P}BraggERdbkAO", (void *)&ERdbk, "ERdbk", 
    "double", 1, 45, 0, 0, 0, 0, 0},

  {"{P}BraggLambdaAO", (void *)&lambda, "lambda", 
    "double", 1, 46, 6, 1, 0, 0, 0},

  {"{P}BraggLambdaAO.DRVH", (void *)&lambdaHi, "lambdaHi", 
    "double", 1, 47, 0, 0, 0, 0, 0},

  {"{P}BraggLambdaAO.DRVL", (void *)&lambdaLo, "lambdaLo", 
    "double", 1, 48, 0, 0, 0, 0, 0},

  {"{P}BraggLambdaRdbkAO", (void *)&lambdaRdbk, "lambdaRdbk", 
    "double", 1, 49, 0, 0, 0, 0, 0},

  {"{P}BraggThetaAO", (void *)&theta, "theta", 
    "double", 1, 50, 7, 1, 0, 0, 0},

  {"{P}BraggThetaAO.DRVH", (void *)&thetaHi, "thetaHi", 
    "double", 1, 51, 0, 0, 0, 0, 0},

  {"{P}BraggThetaAO.DRVL", (void *)&thetaLo, "thetaLo", 
    "double", 1, 52, 0, 0, 0, 0, 0},

  {"{P}BraggThetaRdbkAO", (void *)&thetaRdbk, "thetaRdbk", 
    "double", 1, 53, 0, 0, 0, 0, 0},

  {"{P}KohzuThetaPvSI", (void *)&thetaMotName[0], "thetaMotName", 
    "string", 1, 54, 0, 0, 0, 0, 0},

  {"{P}KohzuYPvSI", (void *)&yMotName[0], "yMotName", 
    "string", 1, 55, 0, 0, 0, 0, 0},

  {"{P}KohzuZPvSI", (void *)&zMotName[0], "zMotName", 
    "string", 1, 56, 0, 0, 0, 0, 0},

  {"{P}KohzuThetaCmdAO", (void *)&thetaMotCmdEcho, "thetaMotCmdEcho", 
    "double", 1, 57, 0, 0, 0, 0, 0},

  {"{P}KohzuYCmdAO", (void *)&yMotCmdEcho, "yMotCmdEcho", 
    "double", 1, 58, 0, 0, 0, 0, 0},

  {"{P}KohzuZCmdAO", (void *)&zMotCmdEcho, "zMotCmdEcho", 
    "double", 1, 59, 0, 0, 0, 0, 0},

  {"{P}KohzuThetaRdbkAI", (void *)&thetaMotRdbkEcho, "thetaMotRdbkEcho", 
    "double", 1, 60, 0, 0, 0, 0, 0},

  {"{P}KohzuYRdbkAI", (void *)&yMotRdbkEcho, "yMotRdbkEcho", 
    "double", 1, 61, 0, 0, 0, 0, 0},

  {"{P}KohzuZRdbkAI", (void *)&zMotRdbkEcho, "zMotRdbkEcho", 
    "double", 1, 62, 0, 0, 0, 0, 0},

  {"{P}KohzuThetaVelAI", (void *)&thetaMotVelEcho, "thetaMotVelEcho", 
    "double", 1, 63, 0, 0, 0, 0, 0},

  {"{P}KohzuYVelAI", (void *)&yMotVelEcho, "yMotVelEcho", 
    "double", 1, 64, 0, 0, 0, 0, 0},

  {"{P}KohzuZVelAI", (void *)&zMotVelEcho, "zMotVelEcho", 
    "double", 1, 65, 0, 0, 0, 0, 0},

  {"{P}KohzuThetaDmovBI", (void *)&thetaDmovEcho, "thetaDmovEcho", 
    "short", 1, 66, 0, 0, 0, 0, 0},

  {"{P}KohzuYDmovBI", (void *)&yDmovEcho, "yDmovEcho", 
    "short", 1, 67, 0, 0, 0, 0, 0},

  {"{P}KohzuZDmovBI", (void *)&zDmovEcho, "zDmovEcho", 
    "short", 1, 68, 0, 0, 0, 0, 0},

  {"{P}{M_THETA}.STOP", (void *)&thetaMotStop, "thetaMotStop", 
    "short", 1, 69, 0, 0, 0, 0, 0},

  {"{P}{M_Y}.STOP", (void *)&yStop, "yStop", 
    "short", 1, 70, 0, 0, 0, 0, 0},

  {"{P}{M_Z}.STOP", (void *)&zStop, "zStop", 
    "short", 1, 71, 0, 0, 0, 0, 0},

  {"{P}{M_THETA}.DMOV", (void *)&thetaDmov, "thetaDmov", 
    "short", 1, 72, 8, 1, 0, 0, 0},

  {"{P}{M_Y}.DMOV", (void *)&yDmov, "yDmov", 
    "short", 1, 73, 9, 1, 0, 0, 0},

  {"{P}{M_Z}.DMOV", (void *)&zDmov, "zDmov", 
    "short", 1, 74, 10, 1, 0, 0, 0},

  {"{P}{M_THETA}.HLS", (void *)&thetaHls, "thetaHls", 
    "short", 1, 75, 0, 1, 0, 0, 0},

  {"{P}{M_THETA}.LLS", (void *)&thetaLls, "thetaLls", 
    "short", 1, 76, 0, 1, 0, 0, 0},

  {"{P}{M_Y}.HLS", (void *)&yHls, "yHls", 
    "short", 1, 77, 0, 1, 0, 0, 0},

  {"{P}{M_Y}.LLS", (void *)&yLls, "yLls", 
    "short", 1, 78, 0, 1, 0, 0, 0},

  {"{P}{M_Z}.HLS", (void *)&zHls, "zHls", 
    "short", 1, 79, 0, 1, 0, 0, 0},

  {"{P}{M_Z}.LLS", (void *)&zLls, "zLls", 
    "short", 1, 80, 0, 1, 0, 0, 0},

  {"{P}KohzuThetaSetAO", (void *)&thetaMotDesired, "thetaMotDesired", 
    "double", 1, 81, 0, 1, 0, 0, 0},

  {"{P}KohzuYSetAO", (void *)&yMotDesired, "yMotDesired", 
    "double", 1, 82, 0, 1, 0, 0, 0},

  {"{P}KohzuYSetAO.DRVH", (void *)&yMotDesiredHi, "yMotDesiredHi", 
    "double", 1, 83, 0, 0, 0, 0, 0},

  {"{P}KohzuYSetAO.DRVL", (void *)&yMotDesiredLo, "yMotDesiredLo", 
    "double", 1, 84, 0, 0, 0, 0, 0},

  {"{P}KohzuZSetAO", (void *)&zMotDesired, "zMotDesired", 
    "double", 1, 85, 0, 1, 0, 0, 0},

  {"{P}{M_THETA}.HLM", (void *)&thetaMotHiLim, "thetaMotHiLim", 
    "double", 1, 86, 11, 1, 0, 0, 0},

  {"{P}{M_THETA}.LLM", (void *)&thetaMotLoLim, "thetaMotLoLim", 
    "double", 1, 87, 12, 1, 0, 0, 0},

  {"{P}{M_Y}.HLM", (void *)&yMotHiLim, "yMotHiLim", 
    "double", 1, 88, 0, 1, 0, 0, 0},

  {"{P}{M_Y}.LLM", (void *)&yMotLoLim, "yMotLoLim", 
    "double", 1, 89, 0, 1, 0, 0, 0},

  {"{P}{M_Z}.HLM", (void *)&zMotHiLim, "zMotHiLim", 
    "double", 1, 90, 0, 1, 0, 0, 0},

  {"{P}{M_Z}.LLM", (void *)&zMotLoLim, "zMotLoLim", 
    "double", 1, 91, 0, 1, 0, 0, 0},

  {"{P}{M_THETA}", (void *)&thetaMotCmd, "thetaMotCmd", 
    "double", 1, 92, 0, 0, 0, 0, 0},

  {"{P}{M_Y}", (void *)&yMotCmd, "yMotCmd", 
    "double", 1, 93, 0, 0, 0, 0, 0},

  {"{P}{M_Z}", (void *)&zMotCmd, "zMotCmd", 
    "double", 1, 94, 0, 0, 0, 0, 0},

  {"{P}{M_THETA}", (void *)&thetaMotCmdMon, "thetaMotCmdMon", 
    "double", 1, 95, 13, 1, 0, 0, 0},

  {"{P}{M_Y}", (void *)&yMotCmdMon, "yMotCmdMon", 
    "double", 1, 96, 14, 1, 0, 0, 0},

  {"{P}{M_Z}", (void *)&zMotCmdMon, "zMotCmdMon", 
    "double", 1, 97, 15, 1, 0, 0, 0},

  {"{P}{M_THETA}.VELO", (void *)&thSpeed, "thSpeed", 
    "double", 1, 98, 16, 1, 0, 0, 0},

  {"{P}{M_Y}.VELO", (void *)&ySpeed, "ySpeed", 
    "double", 1, 99, 17, 1, 0, 0, 0},

  {"{P}{M_Z}.VELO", (void *)&zSpeed, "zSpeed", 
    "double", 1, 100, 18, 1, 0, 0, 0},

  {"{P}{M_THETA}.RBV", (void *)&thetaMotRdbk, "thetaMotRdbk", 
    "double", 1, 101, 19, 1, 0, 0, 0},

  {"{P}{M_Y}.RBV", (void *)&yMotRdbk, "yMotRdbk", 
    "double", 1, 102, 21, 1, 0, 0, 0},

  {"{P}{M_Z}.RBV", (void *)&zMotRdbk, "zMotRdbk", 
    "double", 1, 103, 22, 1, 0, 0, 0},

  {"{P}KohzuUseSetBO", (void *)&useSetMode, "useSetMode", 
    "short", 1, 104, 23, 1, 0, 0, 0},

  {"{P}{M_THETA}.SET", (void *)&thetaMotSet, "thetaMotSet", 
    "short", 1, 105, 24, 1, 0, 0, 0},

  {"{P}{M_Y}.SET", (void *)&yMotSet, "yMotSet", 
    "short", 1, 106, 25, 1, 0, 0, 0},

  {"{P}{M_Z}.SET", (void *)&zMotSet, "zMotSet", 
    "short", 1, 107, 26, 1, 0, 0, 0},

  {"{P}BraggETweakAI", (void *)&ETweakVal, "ETweakVal", 
    "double", 1, 108, 0, 1, 0, 0, 0},

  {"{P}BraggEIncBO", (void *)&ETweakInc, "ETweakInc", 
    "short", 1, 109, 0, 1, 0, 0, 0},

  {"{P}BraggEDecBO", (void *)&ETweakDec, "ETweakDec", 
    "short", 1, 110, 0, 1, 0, 0, 0},

  {"{P}BraggLambdaTweakAI", (void *)&LTweakVal, "LTweakVal", 
    "double", 1, 111, 0, 1, 0, 0, 0},

  {"{P}BraggLambdaIncBO", (void *)&LTweakInc, "LTweakInc", 
    "short", 1, 112, 0, 1, 0, 0, 0},

  {"{P}BraggLambdaDecBO", (void *)&LTweakDec, "LTweakDec", 
    "short", 1, 113, 0, 1, 0, 0, 0},

  {"{P}BraggThetaTweakAI", (void *)&thTweakVal, "thTweakVal", 
    "double", 1, 114, 0, 1, 0, 0, 0},

  {"{P}BraggThetaIncBO", (void *)&thTweakInc, "thTweakInc", 
    "short", 1, 115, 0, 1, 0, 0, 0},

  {"{P}BraggThetaDecBO", (void *)&thTweakDec, "thTweakDec", 
    "short", 1, 116, 0, 1, 0, 0, 0},

  {"{P}Kohzu_yOffsetAO", (void *)&yOffset, "yOffset", 
    "double", 1, 117, 27, 1, 0, 0, 0},

  {"{P}Kohzu_yOffsetAO.DRVH", (void *)&yOffsetHi, "yOffsetHi", 
    "double", 1, 118, 0, 0, 0, 0, 0},

  {"{P}Kohzu_yOffsetAO.DRVL", (void *)&yOffsetLo, "yOffsetLo", 
    "double", 1, 119, 0, 0, 0, 0, 0},

};

/* Event masks for state set kohzuCtl */
	/* Event mask for state init: */
static bitMask	EM_kohzuCtl_init[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state initSequence: */
static bitMask	EM_kohzuCtl_initSequence[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state waitForCmndEnter: */
static bitMask	EM_kohzuCtl_waitForCmndEnter[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state waitForCmnd: */
static bitMask	EM_kohzuCtl_waitForCmnd[] = {
	0x080818fe,
	0x00000003,
	0x00000000,
	0x001b6000,
};
	/* Event mask for state tweak: */
static bitMask	EM_kohzuCtl_tweak[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x001b6000,
};
	/* Event mask for state dInputChanged: */
static bitMask	EM_kohzuCtl_dInputChanged[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state thetaLimits: */
static bitMask	EM_kohzuCtl_thetaLimits[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state eChanged: */
static bitMask	EM_kohzuCtl_eChanged[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state lChanged: */
static bitMask	EM_kohzuCtl_lChanged[] = {
	0x00000000,
	0x00004200,
	0x00000000,
	0x00000000,
};
	/* Event mask for state thChanged: */
static bitMask	EM_kohzuCtl_thChanged[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state calcMovements: */
static bitMask	EM_kohzuCtl_calcMovements[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state moveKohzu: */
static bitMask	EM_kohzuCtl_moveKohzu[] = {
	0x00000000,
	0x00000006,
	0x00000000,
	0x00000100,
};
	/* Event mask for state checkDone: */
static bitMask	EM_kohzuCtl_checkDone[] = {
	0x00000000,
	0x00000000,
	0x00000700,
	0x00000000,
};
	/* Event mask for state updateRdbk: */
static bitMask	EM_kohzuCtl_updateRdbk[] = {
	0x00000000,
	0x00000008,
	0x0001ff00,
	0x00000020,
};
	/* Event mask for state thetaMotStopped: */
static bitMask	EM_kohzuCtl_thetaMotStopped[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state chkMotorLimits: */
static bitMask	EM_kohzuCtl_chkMotorLimits[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state chkMotorLimitsExit: */
static bitMask	EM_kohzuCtl_chkMotorLimitsExit[] = {
	0x80000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state stopKohzu: */
static bitMask	EM_kohzuCtl_stopKohzu[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state stoppedKohzuWait: */
static bitMask	EM_kohzuCtl_stoppedKohzuWait[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_kohzuCtl[] = {
	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_init,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_init,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_init,
	/* state options */   (0)},

	/* State "initSequence" */ {
	/* state name */       "initSequence",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_initSequence,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_initSequence,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_initSequence,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_initSequence,
	/* state options */   (0)},

	/* State "waitForCmndEnter" */ {
	/* state name */       "waitForCmndEnter",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_waitForCmndEnter,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_waitForCmndEnter,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_waitForCmndEnter,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_waitForCmndEnter,
	/* state options */   (0)},

	/* State "waitForCmnd" */ {
	/* state name */       "waitForCmnd",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_waitForCmnd,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_waitForCmnd,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_waitForCmnd,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_waitForCmnd,
	/* state options */   (0)},

	/* State "tweak" */ {
	/* state name */       "tweak",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_tweak,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_tweak,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_tweak,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_tweak,
	/* state options */   (0)},

	/* State "dInputChanged" */ {
	/* state name */       "dInputChanged",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_dInputChanged,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_dInputChanged,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_dInputChanged,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_dInputChanged,
	/* state options */   (0)},

	/* State "thetaLimits" */ {
	/* state name */       "thetaLimits",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_thetaLimits,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_thetaLimits,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_thetaLimits,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_thetaLimits,
	/* state options */   (0)},

	/* State "eChanged" */ {
	/* state name */       "eChanged",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_eChanged,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_eChanged,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_eChanged,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_eChanged,
	/* state options */   (0)},

	/* State "lChanged" */ {
	/* state name */       "lChanged",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_lChanged,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_lChanged,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_lChanged,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_lChanged,
	/* state options */   (0)},

	/* State "thChanged" */ {
	/* state name */       "thChanged",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_thChanged,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_thChanged,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_thChanged,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_thChanged,
	/* state options */   (0)},

	/* State "calcMovements" */ {
	/* state name */       "calcMovements",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_calcMovements,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_calcMovements,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_calcMovements,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_calcMovements,
	/* state options */   (0)},

	/* State "moveKohzu" */ {
	/* state name */       "moveKohzu",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_moveKohzu,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_moveKohzu,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_moveKohzu,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_moveKohzu,
	/* state options */   (0)},

	/* State "checkDone" */ {
	/* state name */       "checkDone",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_checkDone,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_checkDone,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_checkDone,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_checkDone,
	/* state options */   (0)},

	/* State "updateRdbk" */ {
	/* state name */       "updateRdbk",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_updateRdbk,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_updateRdbk,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_updateRdbk,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_updateRdbk,
	/* state options */   (0)},

	/* State "thetaMotStopped" */ {
	/* state name */       "thetaMotStopped",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_thetaMotStopped,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_thetaMotStopped,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_thetaMotStopped,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_thetaMotStopped,
	/* state options */   (0)},

	/* State "chkMotorLimits" */ {
	/* state name */       "chkMotorLimits",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_chkMotorLimits,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_chkMotorLimits,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_chkMotorLimits,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_chkMotorLimits,
	/* state options */   (0)},

	/* State "chkMotorLimitsExit" */ {
	/* state name */       "chkMotorLimitsExit",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_chkMotorLimitsExit,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_chkMotorLimitsExit,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_chkMotorLimitsExit,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_chkMotorLimitsExit,
	/* state options */   (0)},

	/* State "stopKohzu" */ {
	/* state name */       "stopKohzu",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_stopKohzu,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_stopKohzu,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_stopKohzu,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_stopKohzu,
	/* state options */   (0)},

	/* State "stoppedKohzuWait" */ {
	/* state name */       "stoppedKohzuWait",
	/* action function */ (ACTION_FUNC) A_kohzuCtl_stoppedKohzuWait,
	/* event function */  (EVENT_FUNC) E_kohzuCtl_stoppedKohzuWait,
	/* delay function */   (DELAY_FUNC) D_kohzuCtl_stoppedKohzuWait,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_kohzuCtl_stoppedKohzuWait,
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
	0x0077e700,
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
	0x07800000,
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
	/* State set "kohzuCtl" */ {
	/* ss name */            "kohzuCtl",
	/* ptr to state block */ state_kohzuCtl,
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
static char prog_param[] = "P=nda, M_THETA=m9, M_Y=m10, M_Z=m11, GEOM=1";

/* State Program table (global) */
struct seqProgram kohzuCtl = {
	/* magic number */       20000315,
	/* *name */              "kohzuCtl",
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
# line 1334 "../kohzuCtl.st"



double kohzuCtl_calc2dSpacing()
{

 double result;

 result = (2*A)/sqrt(H*H + K*K + L*L);


 opAlert = 0;
 if (((fmod(H,2)) != (fmod(K,2))) || ((fmod(H,2)) != (fmod(L,2))))
  opAlert = 1;
 avg = (H+K+L)/2.0;
 nint = (int)((avg)>0 ? (avg)+0.5 : (avg)-0.5);
 if ((fabs(avg-nint) <= .25) && (fmod(nint,2)))
  opAlert = 1;
 if (opAlert) {
  sprintf(seqMsg1, "(H,K,L) is 'forbidden' reflection");
 } else {
  sprintf(seqMsg1, "New d spacing");
 }
 return(result);
}



#include "epicsExport.h"


/* Register sequencer commands and program */

void kohzuCtlRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&kohzuCtl);
}
epicsExportRegistrar(kohzuCtlRegistrar);

