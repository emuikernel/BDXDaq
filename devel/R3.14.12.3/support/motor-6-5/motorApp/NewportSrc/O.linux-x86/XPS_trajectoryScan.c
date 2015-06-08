/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: XPS_trajectoryScan.i */

/* Event flags */
#define buildMon	1
#define executeMon	2
#define execStateMon	3
#define abortMon	4
#define readbackMon	5
#define nelementsMon	6
#define motorMDVSMon	7

/* Program "XPS_trajectoryScan" */
#include "seqCom.h"

#define NUM_SS 4
#define NUM_CHANNELS 156
#define NUM_EVENTS 7
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT FALSE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT TRUE

extern struct seqProgram XPS_trajectoryScan;

/* Variable declarations */
struct UserVar {
	int	debugLevel;
	int	numAxes;
	int	nelements;
	int	npulses;
	int	startPulses;
	int	endPulses;
	int	nactual;
	int	moveMode;
	double	time;
	double	timeScale;
	int	timeMode;
	double	accel;
	int	build;
	int	buildState;
	int	buildStatus;
	char	buildMessage[MAX_STRING_SIZE];
	int	simMode;
	int	execute;
	int	execState;
	int	execStatus;
	char	execMessage[MAX_STRING_SIZE];
	int	abort;
	int	readback;
	int	readState;
	int	readStatus;
	char	readMessage[MAX_STRING_SIZE];
	double	timeTrajectory[2000];
	char	trajectoryFile[MAX_STRING_SIZE];
	int	moveAxis[8];
	double	motorTrajectory[8][2000];
	double	motorReadbacks[8][2000];
	double	motorError[8][2000];
	double	motorCurrent[8];
	double	motorMDVS[8];
	double	motorMDVA[8];
	int	motorMDVE[8];
	double	motorMVA[8];
	int	motorMVE[8];
	double	motorMAA[8];
	int	motorMAE[8];
	double	epicsMotorPos[8];
	double	epicsMotorDir[8];
	double	epicsMotorOff[8];
	double	epicsMotorDone[8];
	int	status;
	int	i;
	int	j;
	int	k;
	int	anyMoving;
	int	ncomplete;
	int	nextra;
	int	npoints;
	int	dir;
	int	pollSocket;
	int	driveSocket;
	int	abortSocket;
	int	positionSocket;
	int	xpsStatus;
	int	count;
	double	dtime;
	double	posActual;
	double	posTheory;
	double	expectedTime;
	double	initialPos[8];
	double	trajVel;
	double	preDistance[8];
	double	postDistance[8];
	double	motorLowLimit[8];
	double	motorHighLimit[8];
	double	motorMinPos[8];
	double	motorMaxPos[8];
	double	pulseTime;
	double	pulsePeriod;
	char	groupName[100];
	char	xpsAddress[100];
	char	*axisName[8];
	char	macroBuf[100];
	char	motorName[100];
	char	userName[100];
	char	password[100];
	int	xpsPort;
	int	motorCurrentIndex[8];
	int	epicsMotorDoneIndex[8];
	unsigned long	startTime;
};

/* C code definitions */
# line 20 "../XPS_trajectoryScan.st"
 #include <string.h>
# line 21 "../XPS_trajectoryScan.st"
 #include <stdio.h>
# line 22 "../XPS_trajectoryScan.st"
 #include <math.h>
# line 23 "../XPS_trajectoryScan.st"
 #include <stdarg.h>
# line 24 "../XPS_trajectoryScan.st"
 #include "XPS_C8_drivers.h"
# line 25 "../XPS_trajectoryScan.st"
 #include "Socket.h"
# line 26 "../XPS_trajectoryScan.st"
 #include "xps_ftp.h"
# line 126 "../XPS_trajectoryScan.st"
 static int getMotorPositions(SS_ID ssId, struct UserVar *pVar, double *pos);
# line 127 "../XPS_trajectoryScan.st"
 static int getMotorMoving(SS_ID ssId, struct UserVar *pVar);
# line 128 "../XPS_trajectoryScan.st"
 static int waitMotors(SS_ID ssId, struct UserVar *pVar);
# line 129 "../XPS_trajectoryScan.st"
 static int getEpicsMotorMoving(SS_ID ssId, struct UserVar *pVar);
# line 130 "../XPS_trajectoryScan.st"
 static int waitEpicsMotors(SS_ID ssId, struct UserVar *pVar);
# line 132 "../XPS_trajectoryScan.st"
 static int getSocket(SS_ID ssId, struct UserVar *pVar, double timeout);
# line 133 "../XPS_trajectoryScan.st"
 static void trajectoryExecute(SS_ID ssId, struct UserVar *pVar);
# line 134 "../XPS_trajectoryScan.st"
 static void buildAndVerify(SS_ID ssId, struct UserVar *pVar);
# line 135 "../XPS_trajectoryScan.st"
 static int currentElement(SS_ID ssId, struct UserVar *pVar);
# line 136 "../XPS_trajectoryScan.st"
 static int getGroupStatus(SS_ID ssId, struct UserVar *pVar);
# line 137 "../XPS_trajectoryScan.st"
 static void readGathering(SS_ID ssId, struct UserVar *pVar);
# line 138 "../XPS_trajectoryScan.st"
 static int trajectoryAbort(SS_ID ssId, struct UserVar *pVar);
# line 139 "../XPS_trajectoryScan.st"
 static void printMessage(const char *pformat, ...);

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "init" in state set "xpsTrajectoryScan" */

/* Delay function for state "init" in state set "xpsTrajectoryScan" */
static void D_xpsTrajectoryScan_init(SS_ID ssId, struct UserVar *pVar)
{
# line 199 "../XPS_trajectoryScan.st"
}

/* Event function for state "init" in state set "xpsTrajectoryScan" */
static long E_xpsTrajectoryScan_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 199 "../XPS_trajectoryScan.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "xpsTrajectoryScan" */
static void A_xpsTrajectoryScan_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 147 "../XPS_trajectoryScan.st"
			strcpy((pVar->groupName), seq_macValueGet(ssId, "GROUP"));
			strcpy((pVar->xpsAddress), seq_macValueGet(ssId, "IPADDR"));
			strcpy((pVar->userName), seq_macValueGet(ssId, "USERNAME"));
			strcpy((pVar->password), seq_macValueGet(ssId, "PASSWORD"));
			(pVar->xpsPort) = atoi(seq_macValueGet(ssId, "PORT"));
# line 155 "../XPS_trajectoryScan.st"
			if ((pVar->debugLevel) > 0)
			{
				printf("XPS_trajectoryScan: ss xpsTrajectoryScan: state init: xpsAddress=%s groupName=%s\n", (pVar->xpsAddress), (pVar->groupName));
			}
# line 169 "../XPS_trajectoryScan.st"
			for ((pVar->i) = 0; (pVar->i) < (pVar->numAxes); (pVar->i)++)
			{
				(pVar->axisName)[(pVar->i)] = malloc(100);
				sprintf((pVar->macroBuf), "P%d", (pVar->i) + 1);
				sprintf((pVar->axisName)[(pVar->i)], "%s.%s", (pVar->groupName), seq_macValueGet(ssId, (pVar->macroBuf)));
				sprintf((pVar->macroBuf), "M%d", (pVar->i) + 1);
				sprintf((pVar->motorName), "%s%s.VAL", seq_macValueGet(ssId, "P"), seq_macValueGet(ssId, (pVar->macroBuf)));
				seq_pvAssign(ssId, 124 /* epicsMotorPos */ + ((pVar->i)), (pVar->motorName));
				sprintf((pVar->motorName), "%s%s.DIR", seq_macValueGet(ssId, "P"), seq_macValueGet(ssId, (pVar->macroBuf)));
				seq_pvAssign(ssId, 132 /* epicsMotorDir */ + ((pVar->i)), (pVar->motorName));
				sprintf((pVar->motorName), "%s%s.OFF", seq_macValueGet(ssId, "P"), seq_macValueGet(ssId, (pVar->macroBuf)));
				seq_pvAssign(ssId, 140 /* epicsMotorOff */ + ((pVar->i)), (pVar->motorName));
				sprintf((pVar->motorName), "%s%s.DMOV", seq_macValueGet(ssId, "P"), seq_macValueGet(ssId, (pVar->macroBuf)));
				seq_pvAssign(ssId, 148 /* epicsMotorDone */ + ((pVar->i)), (pVar->motorName));
			}
# line 170 "../XPS_trajectoryScan.st"
			(pVar->xpsStatus) = 0;
			(pVar->ncomplete) = 1;
# line 178 "../XPS_trajectoryScan.st"
			for ((pVar->i) = 0; (pVar->i) < 2000; (pVar->i)++)
			{
				for ((pVar->j) = 0; (pVar->j) < 8; (pVar->j)++)
				{
					(pVar->motorTrajectory)[(pVar->j)][(pVar->i)] = 0.0;
				}
			}
# line 181 "../XPS_trajectoryScan.st"
			if ((pVar->numAxes) > 8)
				(pVar->numAxes) = 8;
# line 185 "../XPS_trajectoryScan.st"
pVar->pollSocket = getSocket(ssId, pVar, 1.0);
pVar->driveSocket = getSocket(ssId, pVar, 100000.);
pVar->abortSocket = getSocket(ssId, pVar, 10.);
# line 191 "../XPS_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
			{
				(pVar->motorCurrentIndex)[(pVar->j)] = seq_pvIndex(ssId, 60 /* motorCurrent */ + ((pVar->j)));
				(pVar->epicsMotorDoneIndex)[(pVar->j)] = seq_pvIndex(ssId, 148 /* epicsMotorDone */ + ((pVar->j)));
			}
# line 194 "../XPS_trajectoryScan.st"
			seq_efClear(ssId, buildMon);
			seq_efClear(ssId, executeMon);
			seq_efClear(ssId, abortMon);
			seq_efClear(ssId, readbackMon);
			seq_efClear(ssId, nelementsMon);
		}
		return;
	}
}
/* Code for state "monitor_inputs" in state set "xpsTrajectoryScan" */

/* Entry function for state "monitor_inputs" in state set "xpsTrajectoryScan" */
static void I_xpsTrajectoryScan_monitor_inputs(SS_ID ssId, struct UserVar *pVar)
{
/* Entry 1: */
# line 208 "../XPS_trajectoryScan.st"
	if ((pVar->debugLevel) > 0)
	{
		printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state monitor_inputs: entry\n");
	}
}

/* Delay function for state "monitor_inputs" in state set "xpsTrajectoryScan" */
static void D_xpsTrajectoryScan_monitor_inputs(SS_ID ssId, struct UserVar *pVar)
{
# line 211 "../XPS_trajectoryScan.st"
# line 215 "../XPS_trajectoryScan.st"
# line 219 "../XPS_trajectoryScan.st"
# line 230 "../XPS_trajectoryScan.st"
}

/* Event function for state "monitor_inputs" in state set "xpsTrajectoryScan" */
static long E_xpsTrajectoryScan_monitor_inputs(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 211 "../XPS_trajectoryScan.st"
	if (seq_efTestAndClear(ssId, buildMon) && ((pVar->build) == 1))
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 215 "../XPS_trajectoryScan.st"
	if (seq_efTestAndClear(ssId, executeMon) && ((pVar->execute) == 1) && ((pVar->buildStatus) == 1))
	{
		*pNextState = 3;
		*pTransNum = 1;
		return TRUE;
	}
# line 219 "../XPS_trajectoryScan.st"
	if (seq_efTestAndClear(ssId, readbackMon) && ((pVar->readback) == 1) && ((pVar->execStatus) == 1))
	{
		*pNextState = 5;
		*pTransNum = 2;
		return TRUE;
	}
# line 230 "../XPS_trajectoryScan.st"
	if (seq_efTestAndClear(ssId, nelementsMon) && ((pVar->nelements) >= 1))
	{
		*pNextState = 1;
		*pTransNum = 3;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "monitor_inputs" in state set "xpsTrajectoryScan" */
static void A_xpsTrajectoryScan_monitor_inputs(SS_ID ssId, struct UserVar *pVar, short transNum)
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
	case 2:
		{
		}
		return;
	case 3:
		{
# line 226 "../XPS_trajectoryScan.st"
			if ((pVar->moveMode) == 0)
				(pVar->endPulses) = (pVar->nelements);
			else
				(pVar->endPulses) = (pVar->nelements) - 1;
# line 229 "../XPS_trajectoryScan.st"
			seq_pvPut(ssId, 5 /* endPulses */, 0);
		}
		return;
	}
}
/* Code for state "build" in state set "xpsTrajectoryScan" */

/* Entry function for state "build" in state set "xpsTrajectoryScan" */
static void I_xpsTrajectoryScan_build(SS_ID ssId, struct UserVar *pVar)
{
/* Entry 1: */
# line 240 "../XPS_trajectoryScan.st"
	if ((pVar->debugLevel) > 0)
	{
		printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state build: entry\n");
	}
}

/* Delay function for state "build" in state set "xpsTrajectoryScan" */
static void D_xpsTrajectoryScan_build(SS_ID ssId, struct UserVar *pVar)
{
# line 288 "../XPS_trajectoryScan.st"
}

/* Event function for state "build" in state set "xpsTrajectoryScan" */
static long E_xpsTrajectoryScan_build(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 288 "../XPS_trajectoryScan.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "build" in state set "xpsTrajectoryScan" */
static void A_xpsTrajectoryScan_build(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 244 "../XPS_trajectoryScan.st"
			(pVar->buildState) = 1;
			seq_pvPut(ssId, 13 /* buildState */, 0);
			(pVar->buildStatus) = 0;
			seq_pvPut(ssId, 14 /* buildStatus */, 0);
# line 250 "../XPS_trajectoryScan.st"
			if ((pVar->moveMode) == 0)
				(pVar->npoints) = (pVar->nelements);
			else
				(pVar->npoints) = (pVar->nelements) - 1;
# line 259 "../XPS_trajectoryScan.st"
			if ((pVar->timeMode) == 0)
			{
				(pVar->dtime) = (pVar->time) / (pVar->npoints);
				for ((pVar->i) = 0; (pVar->i) < (pVar->nelements); (pVar->i)++)
					(pVar->timeTrajectory)[(pVar->i)] = (pVar->dtime);
				seq_pvPut(ssId, 26 /* timeTrajectory */, 0);
			}
# line 262 "../XPS_trajectoryScan.st"
			(pVar->expectedTime) = 0;
# line 265 "../XPS_trajectoryScan.st"
			for ((pVar->i) = 0; (pVar->i) < (pVar->npoints); (pVar->i)++)
			{
				(pVar->expectedTime) += (pVar->timeTrajectory)[(pVar->i)];
			}
# line 268 "../XPS_trajectoryScan.st"
buildAndVerify(ssId, pVar);
# line 273 "../XPS_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
			{
				seq_pvPut(ssId, 92 /* motorMVA */ + ((pVar->j)), 0);
				seq_pvPut(ssId, 108 /* motorMAA */ + ((pVar->j)), 0);
			}
# line 275 "../XPS_trajectoryScan.st"
			(pVar->buildState) = 0;
			seq_pvPut(ssId, 13 /* buildState */, 0);
			seq_pvPut(ssId, 14 /* buildStatus */, 0);
			seq_pvPut(ssId, 15 /* buildMessage */, 0);
# line 281 "../XPS_trajectoryScan.st"
			(pVar->build) = 0;
			seq_pvPut(ssId, 12 /* build */, 0);
# line 284 "../XPS_trajectoryScan.st"
			(pVar->execStatus) = 0;
			seq_pvPut(ssId, 19 /* execStatus */, 0);
			strcpy((pVar->execMessage), " ");
		}
		return;
	}
}
/* Code for state "execute" in state set "xpsTrajectoryScan" */

/* Entry function for state "execute" in state set "xpsTrajectoryScan" */
static void I_xpsTrajectoryScan_execute(SS_ID ssId, struct UserVar *pVar)
{
/* Entry 1: */
# line 296 "../XPS_trajectoryScan.st"
	if ((pVar->debugLevel) > 0)
	{
		printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state execute: entry\n");
	}
# line 298 "../XPS_trajectoryScan.st"
waitMotors(ssId, pVar);
# line 300 "../XPS_trajectoryScan.st"
	for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
		(pVar->initialPos)[(pVar->j)] = (pVar->epicsMotorPos)[(pVar->j)];
# line 310 "../XPS_trajectoryScan.st"
	if ((pVar->moveMode) == 1)
	{
		for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
		{
			if ((pVar->moveAxis)[(pVar->j)])
			{
				(pVar->epicsMotorPos)[(pVar->j)] = (pVar->motorTrajectory)[(pVar->j)][0] - (pVar->preDistance)[(pVar->j)];
				seq_pvPut(ssId, 124 /* epicsMotorPos */ + ((pVar->j)), 0);
			}
		}
	}
# line 318 "../XPS_trajectoryScan.st"
	else
	{
		for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
		{
			if ((pVar->moveAxis)[(pVar->j)])
			{
				(pVar->epicsMotorPos)[(pVar->j)] = (pVar->epicsMotorPos)[(pVar->j)] - (pVar->preDistance)[(pVar->j)];
				seq_pvPut(ssId, 124 /* epicsMotorPos */ + ((pVar->j)), 0);
			}
		}
	}
# line 320 "../XPS_trajectoryScan.st"
waitEpicsMotors(ssId, pVar);
# line 342 "../XPS_trajectoryScan.st"
	if ((pVar->npulses) > 0)
	{
		if ((pVar->startPulses) < 1)
			(pVar->startPulses) = 1;
		if ((pVar->startPulses) > (pVar->nelements))
			(pVar->startPulses) = (pVar->nelements);
		seq_pvPut(ssId, 4 /* startPulses */, 0);
		if ((pVar->endPulses) < (pVar->startPulses))
			(pVar->endPulses) = (pVar->startPulses);
		if ((pVar->endPulses) > (pVar->nelements))
			(pVar->endPulses) = (pVar->nelements);
		seq_pvPut(ssId, 5 /* endPulses */, 0);
		(pVar->pulseTime) = 0;
		for ((pVar->i) = (pVar->startPulses); (pVar->i) <= (pVar->endPulses); (pVar->i)++)
		{
			(pVar->pulseTime) += (pVar->timeTrajectory)[(pVar->i) - 1];
		}
		(pVar->pulsePeriod) = (pVar->pulseTime) / (pVar->npulses);
	}
# line 344 "../XPS_trajectoryScan.st"
	else
	{
		(pVar->pulsePeriod) = 0.;
	}
# line 366 "../XPS_trajectoryScan.st"
	for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
	{
		if ((pVar->moveAxis)[(pVar->j)])
		{
			(pVar->status) = PositionerUserTravelLimitsGet((pVar->pollSocket), (pVar->axisName)[(pVar->j)],  & (pVar->motorLowLimit)[(pVar->j)],  & (pVar->motorHighLimit)[(pVar->j)]);
			if (((pVar->epicsMotorPos)[(pVar->j)] + (pVar->motorMinPos)[(pVar->j)]) < (pVar->motorLowLimit)[(pVar->j)])
			{
				(pVar->execStatus) = 2;
				seq_pvPut(ssId, 19 /* execStatus */, 0);
				sprintf((pVar->execMessage), "Low soft limit violation on motor %d", (pVar->j));
				seq_pvPut(ssId, 20 /* execMessage */, 0);
			}
			if (((pVar->epicsMotorPos)[(pVar->j)] + (pVar->motorMaxPos)[(pVar->j)]) > (pVar->motorHighLimit)[(pVar->j)])
			{
				(pVar->execStatus) = 2;
				seq_pvPut(ssId, 19 /* execStatus */, 0);
				sprintf((pVar->execMessage), "High soft limit violation on motor %d", (pVar->j));
				seq_pvPut(ssId, 20 /* execMessage */, 0);
			}
		}
	}
}

/* Delay function for state "execute" in state set "xpsTrajectoryScan" */
static void D_xpsTrajectoryScan_execute(SS_ID ssId, struct UserVar *pVar)
{
# line 378 "../XPS_trajectoryScan.st"
# line 418 "../XPS_trajectoryScan.st"
}

/* Event function for state "execute" in state set "xpsTrajectoryScan" */
static long E_xpsTrajectoryScan_execute(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 378 "../XPS_trajectoryScan.st"
	if ((pVar->execStatus) == 2)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 418 "../XPS_trajectoryScan.st"
	if ((pVar->execStatus) != 2)
	{
		*pNextState = 4;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "execute" in state set "xpsTrajectoryScan" */
static void A_xpsTrajectoryScan_execute(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 375 "../XPS_trajectoryScan.st"
			if ((pVar->debugLevel) > 0)
			{
				printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state execute: execStatus = STATUS_FAILURE\n");
			}
# line 376 "../XPS_trajectoryScan.st"
			(pVar->execute) = 0;
			seq_pvPut(ssId, 17 /* execute */, 0);
		}
		return;
	case 1:
		{
# line 383 "../XPS_trajectoryScan.st"
pVar->xpsStatus = getGroupStatus(ssId, pVar);
# line 386 "../XPS_trajectoryScan.st"
			if ((pVar->debugLevel) > 0)
			{
				printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state execute: setting execState = EXECUTE_STATE_MOVE_START\n");
			}
# line 387 "../XPS_trajectoryScan.st"
			(pVar->execState) = 1;
			seq_pvPut(ssId, 18 /* execState */, 0);
			(pVar->count) = 0;
# line 394 "../XPS_trajectoryScan.st"
			while ((pVar->execState) != 2 && (pVar->count) < 100)
			{
				epicsThreadSleep(0.1);
				(pVar->count)++;
			}
# line 400 "../XPS_trajectoryScan.st"
			if ((pVar->count) == 100)
			{
				strcpy((pVar->execMessage), "Exec Timeout!");
				seq_pvPut(ssId, 20 /* execMessage */, 0);
				(pVar->execStatus) = 3;
				seq_pvPut(ssId, 19 /* execStatus */, 0);
			}
# line 401 "../XPS_trajectoryScan.st"
			(pVar->count) = 0;
# line 409 "../XPS_trajectoryScan.st"
			while ((pVar->xpsStatus) != 45 && (pVar->count) < 100 && (pVar->execStatus) != 2)
			{
				epicsThreadSleep(0.1);
				(pVar->count)++;
pVar->xpsStatus = getGroupStatus(ssId, pVar);
			}
			if ((pVar->count) == 100)
			{
				strcpy((pVar->execMessage), "Exec Timeout!");
				seq_pvPut(ssId, 20 /* execMessage */, 0);
				(pVar->execStatus) = 3;
				seq_pvPut(ssId, 19 /* execStatus */, 0);
			}
# line 416 "../XPS_trajectoryScan.st"
			(pVar->readStatus) = 0;
			seq_pvPut(ssId, 24 /* readStatus */, 0);
		}
		return;
	}
}
/* Code for state "wait_execute" in state set "xpsTrajectoryScan" */

/* Entry function for state "wait_execute" in state set "xpsTrajectoryScan" */
static void I_xpsTrajectoryScan_wait_execute(SS_ID ssId, struct UserVar *pVar)
{
/* Entry 1: */
# line 427 "../XPS_trajectoryScan.st"
	if ((pVar->debugLevel) > 0)
	{
		printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state wait_execute: entry\n");
	}
}

/* Delay function for state "wait_execute" in state set "xpsTrajectoryScan" */
static void D_xpsTrajectoryScan_wait_execute(SS_ID ssId, struct UserVar *pVar)
{
# line 442 "../XPS_trajectoryScan.st"
# line 474 "../XPS_trajectoryScan.st"
	seq_delayInit(ssId, 0, (0.1));
# line 506 "../XPS_trajectoryScan.st"
}

/* Event function for state "wait_execute" in state set "xpsTrajectoryScan" */
static long E_xpsTrajectoryScan_wait_execute(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 442 "../XPS_trajectoryScan.st"
	if ((pVar->execStatus) == 3)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 474 "../XPS_trajectoryScan.st"
	if (seq_delay(ssId, 0) && (pVar->execState) == 2)
	{
		*pNextState = 4;
		*pTransNum = 1;
		return TRUE;
	}
# line 506 "../XPS_trajectoryScan.st"
	if ((pVar->execState) == 3)
	{
		*pNextState = 1;
		*pTransNum = 2;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "wait_execute" in state set "xpsTrajectoryScan" */
static void A_xpsTrajectoryScan_wait_execute(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 432 "../XPS_trajectoryScan.st"
			if ((pVar->debugLevel) > 0)
			{
				printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state wait_execute: execStatus = STATUS_ABORT\n");
			}
			(pVar->execState) = 0;
			seq_pvPut(ssId, 18 /* execState */, 0);
# line 440 "../XPS_trajectoryScan.st"
			(pVar->execute) = 0;
			seq_pvPut(ssId, 17 /* execute */, 0);
		}
		return;
	case 1:
		{
# line 447 "../XPS_trajectoryScan.st"
getMotorPositions(ssId, pVar, pVar->motorCurrent);
# line 447 "../XPS_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
				seq_pvPut(ssId, 60 /* motorCurrent */ + ((pVar->j)), 0);
# line 450 "../XPS_trajectoryScan.st"
pVar->xpsStatus = getGroupStatus(ssId, pVar);
# line 455 "../XPS_trajectoryScan.st"
			if ((pVar->xpsStatus) == 45)
			{
pVar->ncomplete = currentElement(ssId, pVar);
				sprintf((pVar->execMessage), "Executing element %d/%d", (pVar->ncomplete), (pVar->nelements));
				seq_pvPut(ssId, 20 /* execMessage */, 0);
			}
			else
			if ((pVar->xpsStatus) == 12)
			{
				(pVar->execState) = 3;
				(pVar->execStatus) = 1;
				strcpy((pVar->execMessage), " ");
			}
# line 467 "../XPS_trajectoryScan.st"
			else
			if ((pVar->xpsStatus) < 10)
			{
				(pVar->execState) = 3;
				(pVar->execStatus) = 2;
				sprintf((pVar->execMessage), "XPS Status Error=%d", (pVar->xpsStatus));
			}
# line 473 "../XPS_trajectoryScan.st"
			if (difftime(time(0), (pVar->startTime)) > ((pVar->expectedTime) + 10))
			{
				(pVar->execState) = 3;
				(pVar->execStatus) = 4;
				strcpy((pVar->execMessage), "Timeout");
			}
		}
		return;
	case 2:
		{
			if ((pVar->debugLevel) > 0)
			{
				printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state wait_execute: execStatus = EXECUTE_STATE_FLYBACK\n");
			}
# line 480 "../XPS_trajectoryScan.st"
			seq_pvPut(ssId, 18 /* execState */, 0);
			seq_pvPut(ssId, 19 /* execStatus */, 0);
			seq_pvPut(ssId, 20 /* execMessage */, 0);
# line 498 "../XPS_trajectoryScan.st"
			if ((pVar->execStatus) == 1)
			{
waitMotors(ssId, pVar);
waitEpicsMotors(ssId, pVar);
getMotorPositions(ssId, pVar, pVar->motorCurrent);
				for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
					seq_pvPut(ssId, 60 /* motorCurrent */ + ((pVar->j)), 0);
				for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
				{
					(pVar->epicsMotorPos)[(pVar->j)] = (pVar->motorCurrent)[(pVar->j)];
					seq_pvPut(ssId, 124 /* epicsMotorPos */ + ((pVar->j)), 0);
				}
waitEpicsMotors(ssId, pVar);
			}
# line 500 "../XPS_trajectoryScan.st"
			(pVar->execState) = 0;
			seq_pvPut(ssId, 18 /* execState */, 0);
# line 504 "../XPS_trajectoryScan.st"
			(pVar->execute) = 0;
			seq_pvPut(ssId, 17 /* execute */, 0);
		}
		return;
	}
}
/* Code for state "readback" in state set "xpsTrajectoryScan" */

/* Entry function for state "readback" in state set "xpsTrajectoryScan" */
static void I_xpsTrajectoryScan_readback(SS_ID ssId, struct UserVar *pVar)
{
/* Entry 1: */
# line 515 "../XPS_trajectoryScan.st"
	if ((pVar->debugLevel) > 0)
	{
		printMessage("XPS_trajectoryScan: ss xpsTrajectoryScan: state readback: entry\n");
	}
}

/* Delay function for state "readback" in state set "xpsTrajectoryScan" */
static void D_xpsTrajectoryScan_readback(SS_ID ssId, struct UserVar *pVar)
{
# line 553 "../XPS_trajectoryScan.st"
}

/* Event function for state "readback" in state set "xpsTrajectoryScan" */
static long E_xpsTrajectoryScan_readback(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 553 "../XPS_trajectoryScan.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "readback" in state set "xpsTrajectoryScan" */
static void A_xpsTrajectoryScan_readback(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 519 "../XPS_trajectoryScan.st"
			(pVar->readState) = 1;
			seq_pvPut(ssId, 23 /* readState */, 0);
			(pVar->readStatus) = 0;
			seq_pvPut(ssId, 24 /* readStatus */, 0);
# line 529 "../XPS_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
			{
				for ((pVar->i) = 0; (pVar->i) < 2000; (pVar->i)++)
				{
					(pVar->motorReadbacks)[(pVar->j)][(pVar->i)] = 0.;
					(pVar->motorError)[(pVar->j)][(pVar->i)] = 0.;
				}
			}
# line 532 "../XPS_trajectoryScan.st"
readGathering(ssId, pVar);
# line 534 "../XPS_trajectoryScan.st"
			seq_pvPut(ssId, 6 /* nactual */, 0);
# line 540 "../XPS_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
			{
				seq_pvPut(ssId, 44 /* motorReadbacks */ + ((pVar->j)), 0);
				seq_pvPut(ssId, 52 /* motorError */ + ((pVar->j)), 0);
			}
# line 542 "../XPS_trajectoryScan.st"
			(pVar->readState) = 0;
			seq_pvPut(ssId, 23 /* readState */, 0);
# line 545 "../XPS_trajectoryScan.st"
			(pVar->readStatus) = 1;
			seq_pvPut(ssId, 24 /* readStatus */, 0);
			strcpy((pVar->readMessage), " ");
			seq_pvPut(ssId, 25 /* readMessage */, 0);
# line 551 "../XPS_trajectoryScan.st"
			(pVar->readback) = 0;
			seq_pvPut(ssId, 22 /* readback */, 0);
		}
		return;
	}
}
/* Code for state "monitorAbort" in state set "xpsTrajectoryAbort" */

/* Delay function for state "monitorAbort" in state set "xpsTrajectoryAbort" */
static void D_xpsTrajectoryAbort_monitorAbort(SS_ID ssId, struct UserVar *pVar)
{
# line 581 "../XPS_trajectoryScan.st"
}

/* Event function for state "monitorAbort" in state set "xpsTrajectoryAbort" */
static long E_xpsTrajectoryAbort_monitorAbort(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 581 "../XPS_trajectoryScan.st"
	if ((seq_efTestAndClear(ssId, abortMon)) && ((pVar->abort) == 1) && ((pVar->execState) == 2))
	{
		*pNextState = 0;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "monitorAbort" in state set "xpsTrajectoryAbort" */
static void A_xpsTrajectoryAbort_monitorAbort(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 567 "../XPS_trajectoryScan.st"
			(pVar->execStatus) = 3;
# line 570 "../XPS_trajectoryScan.st"
			if ((pVar->debugLevel) > 0)
			{
				printMessage("XPS_trajectoryScan: ss xpsTrajectoryAbort: state monitorAbort: setting execStatus = STATUS_ABORT\n");
			}
# line 571 "../XPS_trajectoryScan.st"
			seq_pvPut(ssId, 19 /* execStatus */, 0);
			strcpy((pVar->execMessage), "Motion aborted");
			seq_pvPut(ssId, 20 /* execMessage */, 0);
# line 576 "../XPS_trajectoryScan.st"
trajectoryAbort(ssId, pVar);
# line 579 "../XPS_trajectoryScan.st"
			(pVar->abort) = 0;
			seq_pvPut(ssId, 21 /* abort */, 0);
		}
		return;
	}
}
/* Code for state "asyncExecute" in state set "xpsTrajectoryRun" */

/* Delay function for state "asyncExecute" in state set "xpsTrajectoryRun" */
static void D_xpsTrajectoryRun_asyncExecute(SS_ID ssId, struct UserVar *pVar)
{
# line 608 "../XPS_trajectoryScan.st"
}

/* Event function for state "asyncExecute" in state set "xpsTrajectoryRun" */
static long E_xpsTrajectoryRun_asyncExecute(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 608 "../XPS_trajectoryScan.st"
	if (seq_efTestAndClear(ssId, execStateMon) && ((pVar->execState) == 1))
	{
		*pNextState = 0;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "asyncExecute" in state set "xpsTrajectoryRun" */
static void A_xpsTrajectoryRun_asyncExecute(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 594 "../XPS_trajectoryScan.st"
			if ((pVar->debugLevel) > 0)
			{
				printMessage("XPS_trajectoryScan: ss xpsTrajectoryRun: state asyncExecute: setting execState = EXECUTE_STATE_EXECUTING\n");
			}
# line 595 "../XPS_trajectoryScan.st"
			(pVar->execState) = 2;
			seq_pvPut(ssId, 18 /* execState */, 0);
# line 604 "../XPS_trajectoryScan.st"
			if ((pVar->xpsStatus) > 9 && (pVar->xpsStatus) < 20)
			{
				(pVar->startTime) = time(0);
trajectoryExecute(ssId, pVar);
			}
# line 607 "../XPS_trajectoryScan.st"
			else
			{
				(pVar->execStatus) = 2;
				seq_pvPut(ssId, 19 /* execStatus */, 0);
			}
		}
		return;
	}
}
/* Code for state "positionUpdate" in state set "xpsTrajectoryPosition" */

/* Delay function for state "positionUpdate" in state set "xpsTrajectoryPosition" */
static void D_xpsTrajectoryPosition_positionUpdate(SS_ID ssId, struct UserVar *pVar)
{
# line 619 "../XPS_trajectoryScan.st"
	seq_delayInit(ssId, 0, (1));
}

/* Event function for state "positionUpdate" in state set "xpsTrajectoryPosition" */
static long E_xpsTrajectoryPosition_positionUpdate(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 619 "../XPS_trajectoryScan.st"
	if (seq_delay(ssId, 0) && ((pVar->execState) == 0))
	{
		*pNextState = 0;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "positionUpdate" in state set "xpsTrajectoryPosition" */
static void A_xpsTrajectoryPosition_positionUpdate(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 618 "../XPS_trajectoryScan.st"
getMotorPositions(ssId, pVar, pVar->motorCurrent);
# line 618 "../XPS_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
				seq_pvPut(ssId, 60 /* motorCurrent */ + ((pVar->j)), 0);
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
  {"{P}{R}DebugLevel.VAL", (void *)OFFSET(struct UserVar, debugLevel), "debugLevel", 
    "int", 1, 8, 0, 1, 0, 0, 0},

  {"{P}{R}NumAxes.VAL", (void *)OFFSET(struct UserVar, numAxes), "numAxes", 
    "int", 1, 9, 0, 1, 0, 0, 0},

  {"{P}{R}Nelements.VAL", (void *)OFFSET(struct UserVar, nelements), "nelements", 
    "int", 1, 10, 6, 1, 0, 0, 0},

  {"{P}{R}Npulses.VAL", (void *)OFFSET(struct UserVar, npulses), "npulses", 
    "int", 1, 11, 0, 1, 0, 0, 0},

  {"{P}{R}StartPulses.VAL", (void *)OFFSET(struct UserVar, startPulses), "startPulses", 
    "int", 1, 12, 0, 1, 0, 0, 0},

  {"{P}{R}EndPulses.VAL", (void *)OFFSET(struct UserVar, endPulses), "endPulses", 
    "int", 1, 13, 0, 1, 0, 0, 0},

  {"{P}{R}Nactual.VAL", (void *)OFFSET(struct UserVar, nactual), "nactual", 
    "int", 1, 14, 0, 0, 0, 0, 0},

  {"{P}{R}MoveMode.VAL", (void *)OFFSET(struct UserVar, moveMode), "moveMode", 
    "int", 1, 15, 0, 1, 0, 0, 0},

  {"{P}{R}Time.VAL", (void *)OFFSET(struct UserVar, time), "time", 
    "double", 1, 16, 0, 1, 0, 0, 0},

  {"{P}{R}TimeScale.VAL", (void *)OFFSET(struct UserVar, timeScale), "timeScale", 
    "double", 1, 17, 0, 1, 0, 0, 0},

  {"{P}{R}TimeMode.VAL", (void *)OFFSET(struct UserVar, timeMode), "timeMode", 
    "int", 1, 18, 0, 1, 0, 0, 0},

  {"{P}{R}Accel.VAL", (void *)OFFSET(struct UserVar, accel), "accel", 
    "double", 1, 19, 0, 1, 0, 0, 0},

  {"{P}{R}Build.VAL", (void *)OFFSET(struct UserVar, build), "build", 
    "int", 1, 20, 1, 1, 0, 0, 0},

  {"{P}{R}BuildState.VAL", (void *)OFFSET(struct UserVar, buildState), "buildState", 
    "int", 1, 21, 0, 0, 0, 0, 0},

  {"{P}{R}BuildStatus.VAL", (void *)OFFSET(struct UserVar, buildStatus), "buildStatus", 
    "int", 1, 22, 0, 0, 0, 0, 0},

  {"{P}{R}BuildMessage.VAL", (void *)OFFSET(struct UserVar, buildMessage[0]), "buildMessage", 
    "string", 1, 23, 0, 0, 0, 0, 0},

  {"{P}{R}SimMode.VAL", (void *)OFFSET(struct UserVar, simMode), "simMode", 
    "int", 1, 24, 0, 1, 0, 0, 0},

  {"{P}{R}Execute.VAL", (void *)OFFSET(struct UserVar, execute), "execute", 
    "int", 1, 25, 2, 1, 0, 0, 0},

  {"{P}{R}ExecState.VAL", (void *)OFFSET(struct UserVar, execState), "execState", 
    "int", 1, 26, 3, 1, 0, 0, 0},

  {"{P}{R}ExecStatus.VAL", (void *)OFFSET(struct UserVar, execStatus), "execStatus", 
    "int", 1, 27, 0, 0, 0, 0, 0},

  {"{P}{R}ExecMessage.VAL", (void *)OFFSET(struct UserVar, execMessage[0]), "execMessage", 
    "string", 1, 28, 0, 0, 0, 0, 0},

  {"{P}{R}Abort.VAL", (void *)OFFSET(struct UserVar, abort), "abort", 
    "int", 1, 29, 4, 1, 0, 0, 0},

  {"{P}{R}Readback.VAL", (void *)OFFSET(struct UserVar, readback), "readback", 
    "int", 1, 30, 5, 1, 0, 0, 0},

  {"{P}{R}ReadState.VAL", (void *)OFFSET(struct UserVar, readState), "readState", 
    "int", 1, 31, 0, 0, 0, 0, 0},

  {"{P}{R}ReadStatus.VAL", (void *)OFFSET(struct UserVar, readStatus), "readStatus", 
    "int", 1, 32, 0, 0, 0, 0, 0},

  {"{P}{R}ReadMessage.VAL", (void *)OFFSET(struct UserVar, readMessage[0]), "readMessage", 
    "string", 1, 33, 0, 0, 0, 0, 0},

  {"{P}{R}TimeTraj.VAL", (void *)OFFSET(struct UserVar, timeTrajectory[0]), "timeTrajectory[0]", 
    "double", 2000, 34, 0, 1, 0, 0, 0},

  {"{P}{R}TrajectoryFile.VAL", (void *)OFFSET(struct UserVar, trajectoryFile[0]), "trajectoryFile", 
    "string", 1, 35, 0, 1, 0, 0, 0},

  {"{P}{R}M1Move.VAL", (void *)OFFSET(struct UserVar, moveAxis[0]), "moveAxis[0]", 
    "int", 1, 36, 0, 1, 0, 0, 0},

  {"{P}{R}M2Move.VAL", (void *)OFFSET(struct UserVar, moveAxis[1]), "moveAxis[1]", 
    "int", 1, 37, 0, 1, 0, 0, 0},

  {"{P}{R}M3Move.VAL", (void *)OFFSET(struct UserVar, moveAxis[2]), "moveAxis[2]", 
    "int", 1, 38, 0, 1, 0, 0, 0},

  {"{P}{R}M4Move.VAL", (void *)OFFSET(struct UserVar, moveAxis[3]), "moveAxis[3]", 
    "int", 1, 39, 0, 1, 0, 0, 0},

  {"{P}{R}M5Move.VAL", (void *)OFFSET(struct UserVar, moveAxis[4]), "moveAxis[4]", 
    "int", 1, 40, 0, 1, 0, 0, 0},

  {"{P}{R}M6Move.VAL", (void *)OFFSET(struct UserVar, moveAxis[5]), "moveAxis[5]", 
    "int", 1, 41, 0, 1, 0, 0, 0},

  {"{P}{R}M7Move.VAL", (void *)OFFSET(struct UserVar, moveAxis[6]), "moveAxis[6]", 
    "int", 1, 42, 0, 1, 0, 0, 0},

  {"{P}{R}M8Move.VAL", (void *)OFFSET(struct UserVar, moveAxis[7]), "moveAxis[7]", 
    "int", 1, 43, 0, 1, 0, 0, 0},

  {"{P}{R}M1Traj.VAL", (void *)OFFSET(struct UserVar, motorTrajectory[0][0]), "motorTrajectory[0][0]", 
    "double", 2000, 44, 0, 1, 0, 0, 0},

  {"{P}{R}M2Traj.VAL", (void *)OFFSET(struct UserVar, motorTrajectory[1][0]), "motorTrajectory[1][0]", 
    "double", 2000, 45, 0, 1, 0, 0, 0},

  {"{P}{R}M3Traj.VAL", (void *)OFFSET(struct UserVar, motorTrajectory[2][0]), "motorTrajectory[2][0]", 
    "double", 2000, 46, 0, 1, 0, 0, 0},

  {"{P}{R}M4Traj.VAL", (void *)OFFSET(struct UserVar, motorTrajectory[3][0]), "motorTrajectory[3][0]", 
    "double", 2000, 47, 0, 1, 0, 0, 0},

  {"{P}{R}M5Traj.VAL", (void *)OFFSET(struct UserVar, motorTrajectory[4][0]), "motorTrajectory[4][0]", 
    "double", 2000, 48, 0, 1, 0, 0, 0},

  {"{P}{R}M6Traj.VAL", (void *)OFFSET(struct UserVar, motorTrajectory[5][0]), "motorTrajectory[5][0]", 
    "double", 2000, 49, 0, 1, 0, 0, 0},

  {"{P}{R}M7Traj.VAL", (void *)OFFSET(struct UserVar, motorTrajectory[6][0]), "motorTrajectory[6][0]", 
    "double", 2000, 50, 0, 1, 0, 0, 0},

  {"{P}{R}M8Traj.VAL", (void *)OFFSET(struct UserVar, motorTrajectory[7][0]), "motorTrajectory[7][0]", 
    "double", 2000, 51, 0, 1, 0, 0, 0},

  {"{P}{R}M1Actual.VAL", (void *)OFFSET(struct UserVar, motorReadbacks[0][0]), "motorReadbacks[0][0]", 
    "double", 2000, 52, 0, 0, 0, 0, 0},

  {"{P}{R}M2Actual.VAL", (void *)OFFSET(struct UserVar, motorReadbacks[1][0]), "motorReadbacks[1][0]", 
    "double", 2000, 53, 0, 0, 0, 0, 0},

  {"{P}{R}M3Actual.VAL", (void *)OFFSET(struct UserVar, motorReadbacks[2][0]), "motorReadbacks[2][0]", 
    "double", 2000, 54, 0, 0, 0, 0, 0},

  {"{P}{R}M4Actual.VAL", (void *)OFFSET(struct UserVar, motorReadbacks[3][0]), "motorReadbacks[3][0]", 
    "double", 2000, 55, 0, 0, 0, 0, 0},

  {"{P}{R}M5Actual.VAL", (void *)OFFSET(struct UserVar, motorReadbacks[4][0]), "motorReadbacks[4][0]", 
    "double", 2000, 56, 0, 0, 0, 0, 0},

  {"{P}{R}M6Actual.VAL", (void *)OFFSET(struct UserVar, motorReadbacks[5][0]), "motorReadbacks[5][0]", 
    "double", 2000, 57, 0, 0, 0, 0, 0},

  {"{P}{R}M7Actual.VAL", (void *)OFFSET(struct UserVar, motorReadbacks[6][0]), "motorReadbacks[6][0]", 
    "double", 2000, 58, 0, 0, 0, 0, 0},

  {"{P}{R}M8Actual.VAL", (void *)OFFSET(struct UserVar, motorReadbacks[7][0]), "motorReadbacks[7][0]", 
    "double", 2000, 59, 0, 0, 0, 0, 0},

  {"{P}{R}M1Error.VAL", (void *)OFFSET(struct UserVar, motorError[0][0]), "motorError[0][0]", 
    "double", 2000, 60, 0, 0, 0, 0, 0},

  {"{P}{R}M2Error.VAL", (void *)OFFSET(struct UserVar, motorError[1][0]), "motorError[1][0]", 
    "double", 2000, 61, 0, 0, 0, 0, 0},

  {"{P}{R}M3Error.VAL", (void *)OFFSET(struct UserVar, motorError[2][0]), "motorError[2][0]", 
    "double", 2000, 62, 0, 0, 0, 0, 0},

  {"{P}{R}M4Error.VAL", (void *)OFFSET(struct UserVar, motorError[3][0]), "motorError[3][0]", 
    "double", 2000, 63, 0, 0, 0, 0, 0},

  {"{P}{R}M5Error.VAL", (void *)OFFSET(struct UserVar, motorError[4][0]), "motorError[4][0]", 
    "double", 2000, 64, 0, 0, 0, 0, 0},

  {"{P}{R}M6Error.VAL", (void *)OFFSET(struct UserVar, motorError[5][0]), "motorError[5][0]", 
    "double", 2000, 65, 0, 0, 0, 0, 0},

  {"{P}{R}M7Error.VAL", (void *)OFFSET(struct UserVar, motorError[6][0]), "motorError[6][0]", 
    "double", 2000, 66, 0, 0, 0, 0, 0},

  {"{P}{R}M8Error.VAL", (void *)OFFSET(struct UserVar, motorError[7][0]), "motorError[7][0]", 
    "double", 2000, 67, 0, 0, 0, 0, 0},

  {"{P}{R}M1Current.VAL", (void *)OFFSET(struct UserVar, motorCurrent[0]), "motorCurrent[0]", 
    "double", 1, 68, 0, 0, 0, 0, 0},

  {"{P}{R}M2Current.VAL", (void *)OFFSET(struct UserVar, motorCurrent[1]), "motorCurrent[1]", 
    "double", 1, 69, 0, 0, 0, 0, 0},

  {"{P}{R}M3Current.VAL", (void *)OFFSET(struct UserVar, motorCurrent[2]), "motorCurrent[2]", 
    "double", 1, 70, 0, 0, 0, 0, 0},

  {"{P}{R}M4Current.VAL", (void *)OFFSET(struct UserVar, motorCurrent[3]), "motorCurrent[3]", 
    "double", 1, 71, 0, 0, 0, 0, 0},

  {"{P}{R}M5Current.VAL", (void *)OFFSET(struct UserVar, motorCurrent[4]), "motorCurrent[4]", 
    "double", 1, 72, 0, 0, 0, 0, 0},

  {"{P}{R}M6Current.VAL", (void *)OFFSET(struct UserVar, motorCurrent[5]), "motorCurrent[5]", 
    "double", 1, 73, 0, 0, 0, 0, 0},

  {"{P}{R}M7Current.VAL", (void *)OFFSET(struct UserVar, motorCurrent[6]), "motorCurrent[6]", 
    "double", 1, 74, 0, 0, 0, 0, 0},

  {"{P}{R}M8Current.VAL", (void *)OFFSET(struct UserVar, motorCurrent[7]), "motorCurrent[7]", 
    "double", 1, 75, 0, 0, 0, 0, 0},

  {"{P}{R}M1MDVS.VAL", (void *)OFFSET(struct UserVar, motorMDVS[0]), "motorMDVS[0]", 
    "double", 1, 76, 7, 1, 0, 0, 0},

  {"{P}{R}M2MDVS.VAL", (void *)OFFSET(struct UserVar, motorMDVS[1]), "motorMDVS[1]", 
    "double", 1, 77, 7, 1, 0, 0, 0},

  {"{P}{R}M3MDVS.VAL", (void *)OFFSET(struct UserVar, motorMDVS[2]), "motorMDVS[2]", 
    "double", 1, 78, 7, 1, 0, 0, 0},

  {"{P}{R}M4MDVS.VAL", (void *)OFFSET(struct UserVar, motorMDVS[3]), "motorMDVS[3]", 
    "double", 1, 79, 7, 1, 0, 0, 0},

  {"{P}{R}M5MDVS.VAL", (void *)OFFSET(struct UserVar, motorMDVS[4]), "motorMDVS[4]", 
    "double", 1, 80, 7, 1, 0, 0, 0},

  {"{P}{R}M6MDVS.VAL", (void *)OFFSET(struct UserVar, motorMDVS[5]), "motorMDVS[5]", 
    "double", 1, 81, 7, 1, 0, 0, 0},

  {"{P}{R}M7MDVS.VAL", (void *)OFFSET(struct UserVar, motorMDVS[6]), "motorMDVS[6]", 
    "double", 1, 82, 7, 1, 0, 0, 0},

  {"{P}{R}M8MDVS.VAL", (void *)OFFSET(struct UserVar, motorMDVS[7]), "motorMDVS[7]", 
    "double", 1, 83, 7, 1, 0, 0, 0},

  {"{P}{R}M1MDVA.VAL", (void *)OFFSET(struct UserVar, motorMDVA[0]), "motorMDVA[0]", 
    "double", 1, 84, 0, 0, 0, 0, 0},

  {"{P}{R}M2MDVA.VAL", (void *)OFFSET(struct UserVar, motorMDVA[1]), "motorMDVA[1]", 
    "double", 1, 85, 0, 0, 0, 0, 0},

  {"{P}{R}M3MDVA.VAL", (void *)OFFSET(struct UserVar, motorMDVA[2]), "motorMDVA[2]", 
    "double", 1, 86, 0, 0, 0, 0, 0},

  {"{P}{R}M4MDVA.VAL", (void *)OFFSET(struct UserVar, motorMDVA[3]), "motorMDVA[3]", 
    "double", 1, 87, 0, 0, 0, 0, 0},

  {"{P}{R}M5MDVA.VAL", (void *)OFFSET(struct UserVar, motorMDVA[4]), "motorMDVA[4]", 
    "double", 1, 88, 0, 0, 0, 0, 0},

  {"{P}{R}M6MDVA.VAL", (void *)OFFSET(struct UserVar, motorMDVA[5]), "motorMDVA[5]", 
    "double", 1, 89, 0, 0, 0, 0, 0},

  {"{P}{R}M7MDVA.VAL", (void *)OFFSET(struct UserVar, motorMDVA[6]), "motorMDVA[6]", 
    "double", 1, 90, 0, 0, 0, 0, 0},

  {"{P}{R}M8MDVA.VAL", (void *)OFFSET(struct UserVar, motorMDVA[7]), "motorMDVA[7]", 
    "double", 1, 91, 0, 0, 0, 0, 0},

  {"{P}{R}M1MDVE.VAL", (void *)OFFSET(struct UserVar, motorMDVE[0]), "motorMDVE[0]", 
    "int", 1, 92, 0, 0, 0, 0, 0},

  {"{P}{R}M2MDVE.VAL", (void *)OFFSET(struct UserVar, motorMDVE[1]), "motorMDVE[1]", 
    "int", 1, 93, 0, 0, 0, 0, 0},

  {"{P}{R}M3MDVE.VAL", (void *)OFFSET(struct UserVar, motorMDVE[2]), "motorMDVE[2]", 
    "int", 1, 94, 0, 0, 0, 0, 0},

  {"{P}{R}M4MDVE.VAL", (void *)OFFSET(struct UserVar, motorMDVE[3]), "motorMDVE[3]", 
    "int", 1, 95, 0, 0, 0, 0, 0},

  {"{P}{R}M5MDVE.VAL", (void *)OFFSET(struct UserVar, motorMDVE[4]), "motorMDVE[4]", 
    "int", 1, 96, 0, 0, 0, 0, 0},

  {"{P}{R}M6MDVE.VAL", (void *)OFFSET(struct UserVar, motorMDVE[5]), "motorMDVE[5]", 
    "int", 1, 97, 0, 0, 0, 0, 0},

  {"{P}{R}M7MDVE.VAL", (void *)OFFSET(struct UserVar, motorMDVE[6]), "motorMDVE[6]", 
    "int", 1, 98, 0, 0, 0, 0, 0},

  {"{P}{R}M8MDVE.VAL", (void *)OFFSET(struct UserVar, motorMDVE[7]), "motorMDVE[7]", 
    "int", 1, 99, 0, 0, 0, 0, 0},

  {"{P}{R}M1MVA.VAL", (void *)OFFSET(struct UserVar, motorMVA[0]), "motorMVA[0]", 
    "double", 1, 100, 0, 0, 0, 0, 0},

  {"{P}{R}M2MVA.VAL", (void *)OFFSET(struct UserVar, motorMVA[1]), "motorMVA[1]", 
    "double", 1, 101, 0, 0, 0, 0, 0},

  {"{P}{R}M3MVA.VAL", (void *)OFFSET(struct UserVar, motorMVA[2]), "motorMVA[2]", 
    "double", 1, 102, 0, 0, 0, 0, 0},

  {"{P}{R}M4MVA.VAL", (void *)OFFSET(struct UserVar, motorMVA[3]), "motorMVA[3]", 
    "double", 1, 103, 0, 0, 0, 0, 0},

  {"{P}{R}M5MVA.VAL", (void *)OFFSET(struct UserVar, motorMVA[4]), "motorMVA[4]", 
    "double", 1, 104, 0, 0, 0, 0, 0},

  {"{P}{R}M6MVA.VAL", (void *)OFFSET(struct UserVar, motorMVA[5]), "motorMVA[5]", 
    "double", 1, 105, 0, 0, 0, 0, 0},

  {"{P}{R}M7MVA.VAL", (void *)OFFSET(struct UserVar, motorMVA[6]), "motorMVA[6]", 
    "double", 1, 106, 0, 0, 0, 0, 0},

  {"{P}{R}M8MVA.VAL", (void *)OFFSET(struct UserVar, motorMVA[7]), "motorMVA[7]", 
    "double", 1, 107, 0, 0, 0, 0, 0},

  {"{P}{R}M1MVE.VAL", (void *)OFFSET(struct UserVar, motorMVE[0]), "motorMVE[0]", 
    "int", 1, 108, 0, 0, 0, 0, 0},

  {"{P}{R}M2MVE.VAL", (void *)OFFSET(struct UserVar, motorMVE[1]), "motorMVE[1]", 
    "int", 1, 109, 0, 0, 0, 0, 0},

  {"{P}{R}M3MVE.VAL", (void *)OFFSET(struct UserVar, motorMVE[2]), "motorMVE[2]", 
    "int", 1, 110, 0, 0, 0, 0, 0},

  {"{P}{R}M4MVE.VAL", (void *)OFFSET(struct UserVar, motorMVE[3]), "motorMVE[3]", 
    "int", 1, 111, 0, 0, 0, 0, 0},

  {"{P}{R}M5MVE.VAL", (void *)OFFSET(struct UserVar, motorMVE[4]), "motorMVE[4]", 
    "int", 1, 112, 0, 0, 0, 0, 0},

  {"{P}{R}M6MVE.VAL", (void *)OFFSET(struct UserVar, motorMVE[5]), "motorMVE[5]", 
    "int", 1, 113, 0, 0, 0, 0, 0},

  {"{P}{R}M7MVE.VAL", (void *)OFFSET(struct UserVar, motorMVE[6]), "motorMVE[6]", 
    "int", 1, 114, 0, 0, 0, 0, 0},

  {"{P}{R}M8MVE.VAL", (void *)OFFSET(struct UserVar, motorMVE[7]), "motorMVE[7]", 
    "int", 1, 115, 0, 0, 0, 0, 0},

  {"{P}{R}M1MAA.VAL", (void *)OFFSET(struct UserVar, motorMAA[0]), "motorMAA[0]", 
    "double", 1, 116, 0, 0, 0, 0, 0},

  {"{P}{R}M2MAA.VAL", (void *)OFFSET(struct UserVar, motorMAA[1]), "motorMAA[1]", 
    "double", 1, 117, 0, 0, 0, 0, 0},

  {"{P}{R}M3MAA.VAL", (void *)OFFSET(struct UserVar, motorMAA[2]), "motorMAA[2]", 
    "double", 1, 118, 0, 0, 0, 0, 0},

  {"{P}{R}M4MAA.VAL", (void *)OFFSET(struct UserVar, motorMAA[3]), "motorMAA[3]", 
    "double", 1, 119, 0, 0, 0, 0, 0},

  {"{P}{R}M5MAA.VAL", (void *)OFFSET(struct UserVar, motorMAA[4]), "motorMAA[4]", 
    "double", 1, 120, 0, 0, 0, 0, 0},

  {"{P}{R}M6MAA.VAL", (void *)OFFSET(struct UserVar, motorMAA[5]), "motorMAA[5]", 
    "double", 1, 121, 0, 0, 0, 0, 0},

  {"{P}{R}M7MAA.VAL", (void *)OFFSET(struct UserVar, motorMAA[6]), "motorMAA[6]", 
    "double", 1, 122, 0, 0, 0, 0, 0},

  {"{P}{R}M8MAA.VAL", (void *)OFFSET(struct UserVar, motorMAA[7]), "motorMAA[7]", 
    "double", 1, 123, 0, 0, 0, 0, 0},

  {"{P}{R}M1MAE.VAL", (void *)OFFSET(struct UserVar, motorMAE[0]), "motorMAE[0]", 
    "int", 1, 124, 0, 0, 0, 0, 0},

  {"{P}{R}M2MAE.VAL", (void *)OFFSET(struct UserVar, motorMAE[1]), "motorMAE[1]", 
    "int", 1, 125, 0, 0, 0, 0, 0},

  {"{P}{R}M3MAE.VAL", (void *)OFFSET(struct UserVar, motorMAE[2]), "motorMAE[2]", 
    "int", 1, 126, 0, 0, 0, 0, 0},

  {"{P}{R}M4MAE.VAL", (void *)OFFSET(struct UserVar, motorMAE[3]), "motorMAE[3]", 
    "int", 1, 127, 0, 0, 0, 0, 0},

  {"{P}{R}M5MAE.VAL", (void *)OFFSET(struct UserVar, motorMAE[4]), "motorMAE[4]", 
    "int", 1, 128, 0, 0, 0, 0, 0},

  {"{P}{R}M6MAE.VAL", (void *)OFFSET(struct UserVar, motorMAE[5]), "motorMAE[5]", 
    "int", 1, 129, 0, 0, 0, 0, 0},

  {"{P}{R}M7MAE.VAL", (void *)OFFSET(struct UserVar, motorMAE[6]), "motorMAE[6]", 
    "int", 1, 130, 0, 0, 0, 0, 0},

  {"{P}{R}M8MAE.VAL", (void *)OFFSET(struct UserVar, motorMAE[7]), "motorMAE[7]", 
    "int", 1, 131, 0, 0, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorPos[0]), "epicsMotorPos[0]", 
    "double", 1, 132, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorPos[1]), "epicsMotorPos[1]", 
    "double", 1, 133, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorPos[2]), "epicsMotorPos[2]", 
    "double", 1, 134, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorPos[3]), "epicsMotorPos[3]", 
    "double", 1, 135, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorPos[4]), "epicsMotorPos[4]", 
    "double", 1, 136, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorPos[5]), "epicsMotorPos[5]", 
    "double", 1, 137, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorPos[6]), "epicsMotorPos[6]", 
    "double", 1, 138, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorPos[7]), "epicsMotorPos[7]", 
    "double", 1, 139, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDir[0]), "epicsMotorDir[0]", 
    "double", 1, 140, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDir[1]), "epicsMotorDir[1]", 
    "double", 1, 141, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDir[2]), "epicsMotorDir[2]", 
    "double", 1, 142, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDir[3]), "epicsMotorDir[3]", 
    "double", 1, 143, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDir[4]), "epicsMotorDir[4]", 
    "double", 1, 144, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDir[5]), "epicsMotorDir[5]", 
    "double", 1, 145, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDir[6]), "epicsMotorDir[6]", 
    "double", 1, 146, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDir[7]), "epicsMotorDir[7]", 
    "double", 1, 147, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorOff[0]), "epicsMotorOff[0]", 
    "double", 1, 148, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorOff[1]), "epicsMotorOff[1]", 
    "double", 1, 149, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorOff[2]), "epicsMotorOff[2]", 
    "double", 1, 150, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorOff[3]), "epicsMotorOff[3]", 
    "double", 1, 151, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorOff[4]), "epicsMotorOff[4]", 
    "double", 1, 152, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorOff[5]), "epicsMotorOff[5]", 
    "double", 1, 153, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorOff[6]), "epicsMotorOff[6]", 
    "double", 1, 154, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorOff[7]), "epicsMotorOff[7]", 
    "double", 1, 155, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDone[0]), "epicsMotorDone[0]", 
    "double", 1, 156, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDone[1]), "epicsMotorDone[1]", 
    "double", 1, 157, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDone[2]), "epicsMotorDone[2]", 
    "double", 1, 158, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDone[3]), "epicsMotorDone[3]", 
    "double", 1, 159, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDone[4]), "epicsMotorDone[4]", 
    "double", 1, 160, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDone[5]), "epicsMotorDone[5]", 
    "double", 1, 161, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDone[6]), "epicsMotorDone[6]", 
    "double", 1, 162, 0, 1, 0, 0, 0},

  {"", (void *)OFFSET(struct UserVar, epicsMotorDone[7]), "epicsMotorDone[7]", 
    "double", 1, 163, 0, 1, 0, 0, 0},

};

/* Event masks for state set xpsTrajectoryScan */
	/* Event mask for state init: */
static bitMask	EM_xpsTrajectoryScan_init[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state monitor_inputs: */
static bitMask	EM_xpsTrajectoryScan_monitor_inputs[] = {
	0x4a500466,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state build: */
static bitMask	EM_xpsTrajectoryScan_build[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state execute: */
static bitMask	EM_xpsTrajectoryScan_execute[] = {
	0x08000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state wait_execute: */
static bitMask	EM_xpsTrajectoryScan_wait_execute[] = {
	0x0c000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state readback: */
static bitMask	EM_xpsTrajectoryScan_readback[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_xpsTrajectoryScan[] = {
	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_xpsTrajectoryScan_init,
	/* event function */  (EVENT_FUNC) E_xpsTrajectoryScan_init,
	/* delay function */   (DELAY_FUNC) D_xpsTrajectoryScan_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xpsTrajectoryScan_init,
	/* state options */   (0)},

	/* State "monitor_inputs" */ {
	/* state name */       "monitor_inputs",
	/* action function */ (ACTION_FUNC) A_xpsTrajectoryScan_monitor_inputs,
	/* event function */  (EVENT_FUNC) E_xpsTrajectoryScan_monitor_inputs,
	/* delay function */   (DELAY_FUNC) D_xpsTrajectoryScan_monitor_inputs,
	/* entry function */   (ENTRY_FUNC) I_xpsTrajectoryScan_monitor_inputs,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xpsTrajectoryScan_monitor_inputs,
	/* state options */   (0)},

	/* State "build" */ {
	/* state name */       "build",
	/* action function */ (ACTION_FUNC) A_xpsTrajectoryScan_build,
	/* event function */  (EVENT_FUNC) E_xpsTrajectoryScan_build,
	/* delay function */   (DELAY_FUNC) D_xpsTrajectoryScan_build,
	/* entry function */   (ENTRY_FUNC) I_xpsTrajectoryScan_build,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xpsTrajectoryScan_build,
	/* state options */   (0)},

	/* State "execute" */ {
	/* state name */       "execute",
	/* action function */ (ACTION_FUNC) A_xpsTrajectoryScan_execute,
	/* event function */  (EVENT_FUNC) E_xpsTrajectoryScan_execute,
	/* delay function */   (DELAY_FUNC) D_xpsTrajectoryScan_execute,
	/* entry function */   (ENTRY_FUNC) I_xpsTrajectoryScan_execute,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xpsTrajectoryScan_execute,
	/* state options */   (0)},

	/* State "wait_execute" */ {
	/* state name */       "wait_execute",
	/* action function */ (ACTION_FUNC) A_xpsTrajectoryScan_wait_execute,
	/* event function */  (EVENT_FUNC) E_xpsTrajectoryScan_wait_execute,
	/* delay function */   (DELAY_FUNC) D_xpsTrajectoryScan_wait_execute,
	/* entry function */   (ENTRY_FUNC) I_xpsTrajectoryScan_wait_execute,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xpsTrajectoryScan_wait_execute,
	/* state options */   (0)},

	/* State "readback" */ {
	/* state name */       "readback",
	/* action function */ (ACTION_FUNC) A_xpsTrajectoryScan_readback,
	/* event function */  (EVENT_FUNC) E_xpsTrajectoryScan_readback,
	/* delay function */   (DELAY_FUNC) D_xpsTrajectoryScan_readback,
	/* entry function */   (ENTRY_FUNC) I_xpsTrajectoryScan_readback,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xpsTrajectoryScan_readback,
	/* state options */   (0)},


};

/* Event masks for state set xpsTrajectoryAbort */
	/* Event mask for state monitorAbort: */
static bitMask	EM_xpsTrajectoryAbort_monitorAbort[] = {
	0x24000010,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_xpsTrajectoryAbort[] = {
	/* State "monitorAbort" */ {
	/* state name */       "monitorAbort",
	/* action function */ (ACTION_FUNC) A_xpsTrajectoryAbort_monitorAbort,
	/* event function */  (EVENT_FUNC) E_xpsTrajectoryAbort_monitorAbort,
	/* delay function */   (DELAY_FUNC) D_xpsTrajectoryAbort_monitorAbort,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xpsTrajectoryAbort_monitorAbort,
	/* state options */   (0)},


};

/* Event masks for state set xpsTrajectoryRun */
	/* Event mask for state asyncExecute: */
static bitMask	EM_xpsTrajectoryRun_asyncExecute[] = {
	0x04000008,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_xpsTrajectoryRun[] = {
	/* State "asyncExecute" */ {
	/* state name */       "asyncExecute",
	/* action function */ (ACTION_FUNC) A_xpsTrajectoryRun_asyncExecute,
	/* event function */  (EVENT_FUNC) E_xpsTrajectoryRun_asyncExecute,
	/* delay function */   (DELAY_FUNC) D_xpsTrajectoryRun_asyncExecute,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xpsTrajectoryRun_asyncExecute,
	/* state options */   (0)},


};

/* Event masks for state set xpsTrajectoryPosition */
	/* Event mask for state positionUpdate: */
static bitMask	EM_xpsTrajectoryPosition_positionUpdate[] = {
	0x04000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_xpsTrajectoryPosition[] = {
	/* State "positionUpdate" */ {
	/* state name */       "positionUpdate",
	/* action function */ (ACTION_FUNC) A_xpsTrajectoryPosition_positionUpdate,
	/* event function */  (EVENT_FUNC) E_xpsTrajectoryPosition_positionUpdate,
	/* delay function */   (DELAY_FUNC) D_xpsTrajectoryPosition_positionUpdate,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_xpsTrajectoryPosition_positionUpdate,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "xpsTrajectoryScan" */ {
	/* ss name */            "xpsTrajectoryScan",
	/* ptr to state block */ state_xpsTrajectoryScan,
	/* number of states */   6,
	/* error state */        -1},


	/* State set "xpsTrajectoryAbort" */ {
	/* ss name */            "xpsTrajectoryAbort",
	/* ptr to state block */ state_xpsTrajectoryAbort,
	/* number of states */   1,
	/* error state */        -1},


	/* State set "xpsTrajectoryRun" */ {
	/* ss name */            "xpsTrajectoryRun",
	/* ptr to state block */ state_xpsTrajectoryRun,
	/* number of states */   1,
	/* error state */        -1},


	/* State set "xpsTrajectoryPosition" */ {
	/* ss name */            "xpsTrajectoryPosition",
	/* ptr to state block */ state_xpsTrajectoryPosition,
	/* number of states */   1,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "P=13BMC:,R=traj1,IPADDR=164.54.160.34,PORT=5001,USERNAME=Administrator,PASSWORD=Administrator,M1=m1,M2=m2,M3=m3,M4=m4,M5=m5,M6=m6,M7=m7,M8=m8,GROUP=g1,P1=p1,P2=p2,P3=p3,P4=p4,P5=p5,P6=p6,P7=p7,P8=p8";

/* State Program table (global) */
struct seqProgram XPS_trajectoryScan = {
	/* magic number */       20000315,
	/* *name */              "XPS_trajectoryScan",
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
# line 626 "../XPS_trajectoryScan.st"





static int getMotorPositions(SS_ID ssId, struct UserVar *pVar, double *pos)
{
    int status;
    int j;
    int dir;



    status = GroupPositionCurrentGet(pVar->positionSocket,
                                     pVar->groupName,pVar->numAxes,pos);
    if (status != 0)
        printMessage("Error performing GroupPositionCurrentGet%i\n", status);

    for (j=0; j<pVar->numAxes; j++) {
        if (pVar->epicsMotorDir[j] == 0) dir=1; else dir=-1;
        pos[j] = pos[j]*dir + pVar->epicsMotorOff[j];
    }

    return(status);
}



static int getMotorMoving(SS_ID ssId, struct UserVar *pVar)
{
    int status;
    int moving=0;
    int groupStatus;



    status = GroupStatusGet(pVar->pollSocket,pVar->groupName,&groupStatus);
    if (status != 0)
        printMessage("Error performing GroupStatusGet %i\n",status);

    if (groupStatus > 42)
        moving = 1;

    return(moving);
}




static int getEpicsMotorMoving(SS_ID ssId, struct UserVar *pVar)
{
    int j;
    int result=0, mask=0x01;

    for (j=0; j<pVar->numAxes; j++) {
        seq_pvGet(ssId, pVar->epicsMotorDoneIndex[j], 0);
        if (pVar->epicsMotorDone[j] == 0) result |= mask;
        mask = mask << 1;
    }
    return(result);
}



static int waitMotors(SS_ID ssId, struct UserVar *pVar)
{
    int j;



    while(getMotorMoving(ssId, pVar)) {

        getMotorPositions(ssId, pVar, pVar->motorCurrent);
        for (j=0; j<pVar->numAxes; j++) seq_pvPut(ssId, pVar->motorCurrentIndex[j], 0);
        epicsThreadSleep(0.1);
    }
    getMotorPositions(ssId, pVar, pVar->motorCurrent);
    for (j=0; j<pVar->numAxes; j++) seq_pvPut(ssId, pVar->motorCurrentIndex[j], 0);
    return(0);
}



static int waitEpicsMotors(SS_ID ssId, struct UserVar *pVar)
{
    int j;



    while(getEpicsMotorMoving(ssId, pVar)) {

        for (j=0; j<pVar->numAxes; j++) {
            pVar->motorCurrent[j] = pVar->epicsMotorPos[j];
            seq_pvPut(ssId, pVar->motorCurrentIndex[j], 0);
        }
        epicsThreadSleep(0.1);
    }
    for (j=0; j<pVar->numAxes; j++) {
        pVar->motorCurrent[j] = pVar->epicsMotorPos[j];
        seq_pvPut(ssId, pVar->motorCurrentIndex[j], 0);
    }
    return(0);
}


static int getSocket(SS_ID ssId, struct UserVar *pVar, double timeout)
{
    int sock = 0;

    sock = TCP_ConnectToServer(pVar->xpsAddress, pVar->xpsPort, timeout);
    if (sock < 0)
            printMessage("Error TCP_ConnectToServer, status=%d\n",sock);
    return (sock);
}



static void trajectoryExecute(SS_ID ssId, struct UserVar *pVar)
{
    int status;
    int j;
    char buffer[60 * 2 * 8];
    int eventId;

    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " entry\n");
    }



    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling GatheringReset(%d)\n", pVar->pollSocket);
    }
    status = GatheringReset(pVar->pollSocket);
    if (status != 0) {
        printMessage("Error performing GatheringReset, status=%d\n",status);
        return;
    }



    strcpy(buffer, "");
    for (j=0; j<pVar->numAxes; j++) {
        strcat (buffer, pVar->axisName[j]);
        strcat (buffer, ".SetpointPosition;");
        strcat (buffer, pVar->axisName[j]);
        strcat (buffer, ".CurrentPosition;");
    }



    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling GatheringConfigurationSet(%d, %d, %s)\n",
               pVar->pollSocket, pVar->numAxes*2, buffer);
    }
    status = GatheringConfigurationSet(pVar->pollSocket,
                                       pVar->numAxes*2, buffer);
    if (status != 0)
            printMessage("Error performing GatheringConfigurationSet, status=%d, buffer=%p\n",
                   status, buffer);
# 808 "../XPS_trajectoryScan.st"
    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling MultipleAxesPVTPulseOutputSet(%d, %s, %d, %d, %f)\n",
                                           pVar->pollSocket, pVar->groupName,
                                           pVar->startPulses+1,
                                           pVar->endPulses+1,
                                           pVar->pulsePeriod);
    }
    status = MultipleAxesPVTPulseOutputSet(pVar->pollSocket, pVar->groupName,
                                           pVar->startPulses+1,
                                           pVar->endPulses+1,
                                           pVar->pulsePeriod);


    sprintf(buffer, "Always;%s.PVT.TrajectoryPulse", pVar->groupName);
    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling EventExtendedConfigurationTriggerSet(%d, %d, %s, %s, %s, %s, %s)\n",
                                                  pVar->pollSocket, 2, buffer,
                                                  "", "", "", "");
    }
    status = EventExtendedConfigurationTriggerSet(pVar->pollSocket, 2, buffer,
                                                  "", "", "", "");
    if (status != 0) {
        printMessage("Error performing EventExtendedConfigurationTriggerSet, status=%d, buffer=%s\n",
               status, buffer);
        return;
    }


    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling EventExtendedConfigurationActionSet(%d, %d, %s, %s, %s, %s, %s)\n",
                                                 pVar->pollSocket, 1,
                                                 "GatheringOneData",
                                                 "", "", "", "");
    }
    status = EventExtendedConfigurationActionSet(pVar->pollSocket, 1,
                                                 "GatheringOneData",
                                                 "", "", "", "");
    if (status != 0) {
        printMessage("Error performing EventExtendedConfigurationActionSet, status=%d\n",
               status);
        return;
    }


    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling EventExtendedStart(%d, %p)\n",
                                            pVar->pollSocket, &eventId);
    }
    status= EventExtendedStart(pVar->pollSocket, &eventId);
    if (status != 0) {
        printMessage("Error performing EventExtendedStart, status=%d\n",status);
        return;
    }

    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling MultipleAxesPVTExecution(%d, %s, %s, %d)\n",
                                      pVar->driveSocket, pVar->groupName,
                                      pVar->trajectoryFile, 1);
    }
    status = MultipleAxesPVTExecution(pVar->driveSocket, pVar->groupName,
                                      pVar->trajectoryFile, 1);

    if ((status != 0) && (status != -27))
        printMessage("Error performing MultipleAxesPVTExecution, status=%d\n", status);


    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling EventExtendedRemove(%d, %d)\n", pVar->pollSocket, eventId);
    }
    status = EventExtendedRemove(pVar->pollSocket, eventId);
    if (status != 0) {
        printMessage("Error performing ExtendedEventRemove, status=%d\n",status);
        return;
    }


    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: trajectoryExecute:"
               " calling GatheringStopAndSave(%d)\n", pVar->pollSocket);
    }
    status = GatheringStopAndSave(pVar->pollSocket);



    if ((status != 0) && (status != -30))
        printMessage("Error performing GatheringStopAndSave, status=%d\n", status);

    return;
}



static void buildAndVerify(SS_ID ssId, struct UserVar *pVar)
{
    FILE *trajFile;
    int i, j, status;
    int npoints;
    double trajVel;
    double D0, D1, T0, T1;
    int ftpSocket;
    char fileName[100];
    double distance;
    double maxVelocity[8], maxAcceleration[8];
    double minJerkTime[8], maxJerkTime[8];
    double preTimeMax, postTimeMax;
    double preVelocity[8], postVelocity[8];
    double time;
    int dir[8];

    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: buildAndVerify:"
               " entry\n");
    }
# 937 "../XPS_trajectoryScan.st"
    preTimeMax = 0.;
    postTimeMax = 0.;

    for (j=0; j<pVar->numAxes; j++) {
        preVelocity[j] = 0.;
        postVelocity[j] = 0.;
    }
    for (j=0; j<pVar->numAxes; j++) {
        if (!pVar->moveAxis[j]) continue;
        status = PositionerSGammaParametersGet(pVar->pollSocket, pVar->axisName[j],
                                               &maxVelocity[j], &maxAcceleration[j],
                                               &minJerkTime[j], &maxJerkTime[j]);
        if (status != 0) {
            printMessage("Error calling positionerSGammaParametersSet, status=%d\n",
                   status);
        }



        maxAcceleration[j] = 0.9 * maxAcceleration[j];



        if (pVar->moveMode == 0) {
            distance = pVar->motorTrajectory[j][0];
        } else {
            distance = pVar->motorTrajectory[j][1] - pVar->motorTrajectory[j][0];
        }
        preVelocity[j] = distance/pVar->timeTrajectory[0];
        time = fabs(preVelocity[j]) / maxAcceleration[j];
        if (time > preTimeMax) preTimeMax = time;
        if (pVar->moveMode == 0) {
            distance = pVar->motorTrajectory[j][pVar->nelements-1];
        } else {
            distance = pVar->motorTrajectory[j][pVar->nelements-1] -
                       pVar->motorTrajectory[j][pVar->nelements-2];
        }
        postVelocity[j] = distance/pVar->timeTrajectory[pVar->nelements-1];
        time = fabs(postVelocity[j]) / maxAcceleration[j];
        if (time > postTimeMax) postTimeMax = time;
    }



    for (j=0; j<pVar->numAxes; j++) {
        pVar->preDistance[j] = 0.5 * preVelocity[j] * preTimeMax;
        pVar->postDistance[j] = 0.5 * postVelocity[j] * postTimeMax;
    }


    trajFile = fopen (pVar->trajectoryFile, "w");



    for (j=0; j<pVar->numAxes; j++) {
        if (pVar->epicsMotorDir[j] == 0) dir[j]=1; else dir[j]=-1;
    }

    fprintf(trajFile,"%f", preTimeMax);
    for (j=0; j<pVar->numAxes; j++)
        fprintf(trajFile,", %f, %f", pVar->preDistance[j]*dir[j], preVelocity[j]*dir[j]);
    fprintf(trajFile,"\n");



    if (pVar->moveMode == 0)
        npoints = pVar->nelements;
    else
        npoints = pVar->nelements-1;
    for (i=0; i<npoints; i++) {
        T0 = pVar->timeTrajectory[i];
        if (i < npoints-1)
            T1 = pVar->timeTrajectory[i+1];
        else
            T1 = T0;
        for (j=0; j<pVar->numAxes; j++) {
            if (pVar->moveMode == 0) {
                D0 = pVar->motorTrajectory[j][i] * dir[j];
                if (i < npoints-1)
                    D1 = pVar->motorTrajectory[j][i+1] * dir[j];
                else
                    D1 = D0;
            } else {
                D0 = pVar->motorTrajectory[j][i+1] *dir[j] -
                           pVar->motorTrajectory[j][i] * dir[j];
                if (i < npoints-1)
                    D1 = pVar->motorTrajectory[j][i+2] * dir[j] -
                           pVar->motorTrajectory[j][i+1] * dir[j];
                else
                    D1 = D0;
            }


            trajVel = ((D0 + D1) / (T0 + T1));
            if (!(pVar->moveAxis[j])) {
                D0 = 0.0;
                trajVel = 0.0;
            }

            if (j == 0) fprintf(trajFile,"%f", pVar->timeTrajectory[i]);
            fprintf(trajFile,", %f, %f",D0,trajVel);
            if (j == (pVar->numAxes-1)) fprintf(trajFile,"\n");
        }
    }


    fprintf(trajFile,"%f", postTimeMax);
    for (j=0; j<pVar->numAxes; j++)
        fprintf(trajFile,", %f, %f", pVar->postDistance[j]*dir[j], 0.);
    fprintf(trajFile,"\n");
    fclose (trajFile);


    status = ftpConnect(pVar->xpsAddress, pVar->userName, pVar->password, &ftpSocket);
    if (status != 0) {
        printMessage("Error calling ftpConnect, status=%d\n", status);
        return;
    }
    status = ftpChangeDir(ftpSocket, "/Admin/public/Trajectories");
    if (status != 0) {
        printMessage("Error calling ftpChangeDir, status=%d\n", status);
        return;
    }
    status = ftpStoreFile(ftpSocket, pVar->trajectoryFile);
    if (status != 0) {
        printMessage("Error calling ftpStoreFile, status=%d\n", status);
        return;
    }
    status = ftpDisconnect(ftpSocket);
    if (status != 0) {
        printMessage("Error calling ftpDisconnect, status=%d\n", status);
        return;
    }


    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: buildAndVerify:"
               " calling MultipleAxesPVTVerification(%d, %s, %s)\n",
               pVar->pollSocket, pVar->groupName, pVar->trajectoryFile);
    }
    status = MultipleAxesPVTVerification(pVar->pollSocket, pVar->groupName,
                                         pVar->trajectoryFile);

    pVar->buildStatus = 2;
    if (status == 0) {
        strcpy(pVar->buildMessage, " ");
        pVar->buildStatus = 1;
    }
    else if (status == -69)
        strcpy(pVar->buildMessage, "Acceleration Too High");
    else if (status == -68)
        strcpy(pVar->buildMessage, "Velocity Too High");
    else if (status == -70)
        strcpy(pVar->buildMessage, "Final Velocity Non Zero");
    else if (status == -75)
        strcpy(pVar->buildMessage, "Negative or Null Delta Time");
    else
        sprintf(pVar->buildMessage, "Unknown trajectory verify error=%d", status);



    if (status == 0){
        pVar->buildStatus = 1;
    }
    if (1) {
        for (j=0; j<pVar->numAxes; j++) {
            status = MultipleAxesPVTVerificationResultGet(pVar->pollSocket,
                         pVar->axisName[j], fileName, &pVar->motorMinPos[j], &pVar->motorMaxPos[j],
                         &pVar->motorMVA[j], &pVar->motorMAA[j]);
            if (status != 0) {
                printMessage("Error performing MultipleAxesPVTVerificationResultGet for axis %s, status=%d\n",
                       pVar->axisName[j], status);
            }
        }
    } else {
        for (j=0; j<pVar->numAxes; j++) {
            pVar->motorMVA[j] = 0;
            pVar->motorMAA[j] = 0;
        }
    }

    return;
}



static int currentElement(SS_ID ssId, struct UserVar *pVar)
{
    int status;
    int number;
    char fileName[100];

    status = MultipleAxesPVTParametersGet(pVar->pollSocket,
                 pVar->groupName, fileName, &number);
    if (status != 0)
        printMessage("Error performing MultipleAxesPVTParametersGet, status=%d\n",
               status);
    return (number);
}

static int getGroupStatus(SS_ID ssId, struct UserVar *pVar)
{
    int status;
    int groupStatus;



    status = GroupStatusGet(pVar->pollSocket,pVar->groupName,&groupStatus);
    if (status != 0)
        printMessage("Error performing GroupStatusGet, status=%d\n", status);
    return(groupStatus);
}





static void readGathering(SS_ID ssId, struct UserVar *pVar)
{
    char buffer[60 * 2 * 8];
    FILE *gatheringFile;
    int i,j;
    int nitems;
    double setpointPosition, actualPosition;
    int ftpSocket;
    int status;
    int dir;

    if (pVar->debugLevel > 0) {
        printMessage("XPS_trajectoryScan: readGathering:"
               " entry\n");
    }

    status = ftpConnect(pVar->xpsAddress, pVar->userName, pVar->password, &ftpSocket);
    if (status != 0) {
        printMessage("Error calling ftpConnect, status=%d\n", status);
        return;
    }
    status = ftpChangeDir(ftpSocket, "/Admin/public/");
    if (status != 0) {
        printMessage("Error calling ftpChangeDir, status=%d\n", status);
        return;
    }
    status = ftpRetrieveFile(ftpSocket, "Gathering.dat");
    if (status != 0) {
        printMessage("Error calling ftpRetrieveFile, status=%d\n", status);
        return;
    }
    status = ftpDisconnect(ftpSocket);
    if (status != 0) {
        printMessage("Error calling ftpDisconnect, status=%d\n", status);
        return;
    }

    gatheringFile = fopen("Gathering.dat", "r");


    for (i=0; i<2; i++) {
        fgets (buffer, 60 * 2 * 8, gatheringFile);
    }
# 1207 "../XPS_trajectoryScan.st"
    for (i=0; i<pVar->npulses; i++) {
        for (j=0; j<pVar->numAxes; j++) {


            nitems = fscanf(gatheringFile, "%lf %lf ",
                            &setpointPosition, &actualPosition);
            if (nitems != 2) {
                printMessage("Error reading Gathering.dat file, nitems=%d, should be %d\n",
                       nitems, 2);
                goto done;
            }
            pVar->motorError[j][i] = actualPosition - setpointPosition;

            if (pVar->epicsMotorDir[j] == 0) dir=1; else dir=-1;
            pVar->motorReadbacks[j][i] = actualPosition * dir + pVar->epicsMotorOff[j];
        }
    }

    done:
    fclose (gatheringFile);
    pVar->nactual = i;

    return;
}


static int trajectoryAbort(SS_ID ssId, struct UserVar *pVar)
{
    int status;

    status = GroupMoveAbort(pVar->abortSocket,pVar->groupName);
    if (status != 0)
        printMessage("Error performing GroupMoveAbort, status=%d\n", status);

    return (0);
}

static void printMessage(const char *pformat, ...)
{
    va_list pvar;
    FILE *fp = stdout;
    epicsTimeStamp now;
    char nowText[40];

    epicsTimeGetCurrent(&now);
    nowText[0] = 0;
    epicsTimeToStrftime(nowText,sizeof(nowText),
         "%Y/%m/%d %H:%M:%S.%03f",&now);
    fprintf(fp,"%s ",nowText);
    va_start(pvar, pformat);
    vfprintf(fp,pformat,pvar);
    va_end(pvar);
    fflush(fp);
}


#include "epicsExport.h"


/* Register sequencer commands and program */

void XPS_trajectoryScanRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&XPS_trajectoryScan);
}
epicsExportRegistrar(XPS_trajectoryScanRegistrar);

