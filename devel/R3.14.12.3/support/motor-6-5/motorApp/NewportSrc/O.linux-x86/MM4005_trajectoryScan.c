/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: MM4005_trajectoryScan.i */

/* Event flags */
#define buildMon	1
#define executeMon	2
#define execStateMon	3
#define abortMon	4
#define readbackMon	5
#define nelementsMon	6
#define motorMDVSMon	7

/* Program "MM4005_trajectoryScan" */
#include "seqCom.h"

#define NUM_SS 2
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

extern struct seqProgram MM4005_trajectoryScan;

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
	char	stringOut[256];
	char	stringIn[256];
	char	*asynPort;
	char	*pasynUser;
	int	status;
	int	i;
	int	j;
	int	k;
	double	delay;
	int	anyMoving;
	int	ncomplete;
	int	nextra;
	int	npoints;
	int	dir;
	double	dtime;
	double	dpos;
	double	posActual;
	double	posTheory;
	double	expectedTime;
	double	initialPos[8];
	char	macroBuf[100];
	char	motorName[100];
	char	*p;
	char	*tok_save;
	int	motorCurrentIndex[8];
	int	epicsMotorDoneIndex[8];
	unsigned long	startTime;
};

/* C code definitions */
# line 13 "../MM4005_trajectoryScan.st"
 #include <string.h>
# line 14 "../MM4005_trajectoryScan.st"
 #include <stdio.h>
# line 15 "../MM4005_trajectoryScan.st"
 #include <epicsString.h>
# line 16 "../MM4005_trajectoryScan.st"
 #include <asynOctetSyncIO.h>
# line 95 "../MM4005_trajectoryScan.st"
 static int writeOnly(SS_ID ssId, struct UserVar *pVar, char *command);
# line 96 "../MM4005_trajectoryScan.st"
 static int writeRead(SS_ID ssId, struct UserVar *pVar, char *command);
# line 97 "../MM4005_trajectoryScan.st"
 static int getMotorPositions(SS_ID ssId, struct UserVar *pVar, double *pos);
# line 98 "../MM4005_trajectoryScan.st"
 static int getMotorMoving(SS_ID ssId, struct UserVar *pVar);
# line 99 "../MM4005_trajectoryScan.st"
 static int getEpicsMotorMoving(SS_ID ssId, struct UserVar *pVar);
# line 100 "../MM4005_trajectoryScan.st"
 static int waitEpicsMotors(SS_ID ssId, struct UserVar *pVar);

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "init" in state set "trajectoryScan" */

/* Delay function for state "init" in state set "trajectoryScan" */
static void D_trajectoryScan_init(SS_ID ssId, struct UserVar *pVar)
{
# line 148 "../MM4005_trajectoryScan.st"
}

/* Event function for state "init" in state set "trajectoryScan" */
static long E_trajectoryScan_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 148 "../MM4005_trajectoryScan.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "trajectoryScan" */
static void A_trajectoryScan_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 108 "../MM4005_trajectoryScan.st"
			if ((pVar->numAxes) > 8)
				(pVar->numAxes) = 8;
# line 119 "../MM4005_trajectoryScan.st"
			for ((pVar->i) = 0; (pVar->i) < (pVar->numAxes); (pVar->i)++)
			{
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
# line 121 "../MM4005_trajectoryScan.st"
			(pVar->asynPort) = seq_macValueGet(ssId, "PORT");
# line 123 "../MM4005_trajectoryScan.st"
pVar->status = pasynOctetSyncIO->connect(pVar->asynPort, 0,
 (asynUser **)&pVar->pasynUser,
 NULL);
# line 128 "../MM4005_trajectoryScan.st"
			if ((pVar->status) != 0)
			{
				printf("trajectoryScan error in pasynOctetSyncIO->connect\n");
				printf("   status=%d, port=%s\n", (pVar->status), (pVar->asynPort));
			}
# line 132 "../MM4005_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
			{
				(pVar->motorCurrentIndex)[(pVar->j)] = seq_pvIndex(ssId, 60 /* motorCurrent */ + ((pVar->j)));
				(pVar->epicsMotorDoneIndex)[(pVar->j)] = seq_pvIndex(ssId, 148 /* epicsMotorDone */ + ((pVar->j)));
			}
# line 140 "../MM4005_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
			{
				sprintf((pVar->stringOut), "%dGC?", (pVar->j) + 1);
writeRead(ssId, pVar, pVar->stringOut);
				(pVar->motorMDVS)[(pVar->j)] = atof((pVar->stringIn) + 3);
				seq_pvPut(ssId, 68 /* motorMDVS */ + ((pVar->j)), 0);
			}
# line 142 "../MM4005_trajectoryScan.st"
			seq_efClear(ssId, buildMon);
			seq_efClear(ssId, executeMon);
			seq_efClear(ssId, abortMon);
			seq_efClear(ssId, readbackMon);
			seq_efClear(ssId, nelementsMon);
			seq_efClear(ssId, motorMDVSMon);
		}
		return;
	}
}
/* Code for state "monitor_inputs" in state set "trajectoryScan" */

/* Delay function for state "monitor_inputs" in state set "trajectoryScan" */
static void D_trajectoryScan_monitor_inputs(SS_ID ssId, struct UserVar *pVar)
{
# line 155 "../MM4005_trajectoryScan.st"
# line 158 "../MM4005_trajectoryScan.st"
# line 161 "../MM4005_trajectoryScan.st"
# line 169 "../MM4005_trajectoryScan.st"
# line 179 "../MM4005_trajectoryScan.st"
}

/* Event function for state "monitor_inputs" in state set "trajectoryScan" */
static long E_trajectoryScan_monitor_inputs(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 155 "../MM4005_trajectoryScan.st"
	if (seq_efTestAndClear(ssId, buildMon) && ((pVar->build) == 1))
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 158 "../MM4005_trajectoryScan.st"
	if (seq_efTestAndClear(ssId, executeMon) && ((pVar->execute) == 1))
	{
		*pNextState = 3;
		*pTransNum = 1;
		return TRUE;
	}
# line 161 "../MM4005_trajectoryScan.st"
	if (seq_efTestAndClear(ssId, readbackMon) && ((pVar->readback) == 1))
	{
		*pNextState = 5;
		*pTransNum = 2;
		return TRUE;
	}
# line 169 "../MM4005_trajectoryScan.st"
	if (seq_efTestAndClear(ssId, nelementsMon) && ((pVar->nelements) >= 1))
	{
		*pNextState = 1;
		*pTransNum = 3;
		return TRUE;
	}
# line 179 "../MM4005_trajectoryScan.st"
	if (seq_efTestAndClear(ssId, motorMDVSMon))
	{
		*pNextState = 1;
		*pTransNum = 4;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "monitor_inputs" in state set "trajectoryScan" */
static void A_trajectoryScan_monitor_inputs(SS_ID ssId, struct UserVar *pVar, short transNum)
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
# line 167 "../MM4005_trajectoryScan.st"
			(pVar->endPulses) = (pVar->nelements);
			seq_pvPut(ssId, 5 /* endPulses */, 0);
		}
		return;
	case 4:
		{
# line 178 "../MM4005_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
			{
				sprintf((pVar->stringOut), "%dGC%f", (pVar->j) + 1, (pVar->motorMDVS)[(pVar->j)]);
writeOnly(ssId, pVar, pVar->stringOut);
			}
		}
		return;
	}
}
/* Code for state "build" in state set "trajectoryScan" */

/* Delay function for state "build" in state set "trajectoryScan" */
static void D_trajectoryScan_build(SS_ID ssId, struct UserVar *pVar)
{
# line 354 "../MM4005_trajectoryScan.st"
}

/* Event function for state "build" in state set "trajectoryScan" */
static long E_trajectoryScan_build(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 354 "../MM4005_trajectoryScan.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "build" in state set "trajectoryScan" */
static void A_trajectoryScan_build(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 187 "../MM4005_trajectoryScan.st"
			(pVar->buildState) = 1;
			seq_pvPut(ssId, 13 /* buildState */, 0);
			(pVar->buildStatus) = 0;
			seq_pvPut(ssId, 14 /* buildStatus */, 0);
# line 192 "../MM4005_trajectoryScan.st"
			strcpy((pVar->stringOut), "NC");
# line 194 "../MM4005_trajectoryScan.st"
writeOnly(ssId, pVar, pVar->stringOut);
# line 198 "../MM4005_trajectoryScan.st"
			for ((pVar->i) = 0; (pVar->i) < (pVar->numAxes); (pVar->i)++)
			{
				sprintf((pVar->stringOut), "%dDC%d", (pVar->i) + 1, (pVar->moveAxis)[(pVar->i)]);
writeOnly(ssId, pVar, pVar->stringOut);
			}
# line 200 "../MM4005_trajectoryScan.st"
			sprintf((pVar->stringOut), "UC%f", (pVar->accel));
# line 202 "../MM4005_trajectoryScan.st"
writeOnly(ssId, pVar, pVar->stringOut);
# line 208 "../MM4005_trajectoryScan.st"
			if ((pVar->timeMode) == 0)
			{
				(pVar->dtime) = (pVar->time) / (pVar->nelements);
				for ((pVar->i) = 0; (pVar->i) < (pVar->nelements); (pVar->i)++)
					(pVar->timeTrajectory)[(pVar->i)] = (pVar->dtime);
				seq_pvPut(ssId, 26 /* timeTrajectory */, 0);
			}
# line 216 "../MM4005_trajectoryScan.st"
			if ((pVar->moveMode) == 0)
			{
				(pVar->npoints) = (pVar->nelements);
			}
# line 218 "../MM4005_trajectoryScan.st"
			else
			{
				(pVar->npoints) = (pVar->nelements) - 1;
			}
# line 219 "../MM4005_trajectoryScan.st"
			(pVar->nextra) = ((pVar->npoints) % 4);
# line 250 "../MM4005_trajectoryScan.st"
			if ((pVar->nextra) != 0)
			{
				(pVar->nextra) = 4 - (pVar->nextra);
				for ((pVar->i) = 0; (pVar->i) < (pVar->nextra); (pVar->i)++)
				{
					(pVar->timeTrajectory)[(pVar->npoints) + (pVar->i)] = 0.1;
					for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
					{
						if (!(pVar->moveAxis)[(pVar->j)])
							continue;
						if ((pVar->moveMode) == 0)
						{
							(pVar->motorTrajectory)[(pVar->j)][(pVar->nelements) + (pVar->i)] = (pVar->motorTrajectory)[(pVar->j)][(pVar->nelements) - 1] * 0.1 / (pVar->timeTrajectory)[(pVar->nelements) - 1];
						}
						else
						{
							(pVar->dpos) = ((pVar->motorTrajectory)[(pVar->j)][(pVar->nelements) - 1] - (pVar->motorTrajectory)[(pVar->j)][(pVar->nelements) - 2]) * 0.1 / (pVar->timeTrajectory)[(pVar->nelements) - 2];
							(pVar->motorTrajectory)[(pVar->j)][(pVar->nelements) + (pVar->i)] = (pVar->motorTrajectory)[(pVar->j)][(pVar->nelements) - 1] + (pVar->dpos) * ((pVar->i) + 1);
						}
					}
				}
				(pVar->nelements) += (pVar->nextra);
				(pVar->npoints) += (pVar->nextra);
				seq_pvPut(ssId, 2 /* nelements */, 0);
				seq_pvPut(ssId, 26 /* timeTrajectory */, 0);
				for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
				{
					seq_pvPut(ssId, 36 /* motorTrajectory */ + ((pVar->j)), 0);
				}
			}
# line 252 "../MM4005_trajectoryScan.st"
			(pVar->expectedTime) = 0;
# line 254 "../MM4005_trajectoryScan.st"
			for ((pVar->i) = 0; (pVar->i) < (pVar->npoints); (pVar->i)++)
				(pVar->expectedTime) += (pVar->timeTrajectory)[(pVar->i)];
# line 278 "../MM4005_trajectoryScan.st"
			for ((pVar->i) = 0; (pVar->i) < (pVar->npoints); (pVar->i)++)
			{
				sprintf((pVar->buildMessage), "Building element %d/%d", (pVar->i) + 1, (pVar->nelements));
				seq_pvPut(ssId, 15 /* buildMessage */, 0);
				sprintf((pVar->stringOut), "%dDT%f", (pVar->i) + 1, (pVar->timeTrajectory)[(pVar->i)]);
writeOnly(ssId, pVar, pVar->stringOut);
				for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
				{
					if (!(pVar->moveAxis)[(pVar->j)])
						continue;
					if ((pVar->moveMode) == 0)
					{
						(pVar->dpos) = (pVar->motorTrajectory)[(pVar->j)][(pVar->i)];
					}
					else
					{
						(pVar->dpos) = (pVar->motorTrajectory)[(pVar->j)][(pVar->i) + 1] - (pVar->motorTrajectory)[(pVar->j)][(pVar->i)];
					}
					if ((pVar->epicsMotorDir)[(pVar->j)] == 0)
						(pVar->dir) = 1;
					else
						(pVar->dir) = -1;
					(pVar->dpos) = (pVar->dpos) * (pVar->dir);
					sprintf((pVar->stringOut), "%dDX%f", (pVar->j) + 1, (pVar->dpos));
writeOnly(ssId, pVar, pVar->stringOut);
				}
				if ((((pVar->i) + 1) % 20) == 0)
writeRead(ssId, pVar, "TB");
			}
# line 293 "../MM4005_trajectoryScan.st"
			if ((pVar->npulses) > 0)
			{
				if ((pVar->startPulses) < 1)
					(pVar->startPulses) = 1;
				if ((pVar->startPulses) > (pVar->npoints))
					(pVar->startPulses) = (pVar->npoints);
				seq_pvPut(ssId, 4 /* startPulses */, 0);
				if ((pVar->endPulses) < (pVar->startPulses))
					(pVar->endPulses) = (pVar->startPulses);
				if ((pVar->endPulses) > (pVar->npoints))
					(pVar->endPulses) = (pVar->npoints);
				seq_pvPut(ssId, 5 /* endPulses */, 0);
				sprintf((pVar->stringOut), "MB%d,ME%d,MN%d", (pVar->startPulses), (pVar->endPulses), (pVar->npulses) + 1);
writeOnly(ssId, pVar, pVar->stringOut);
			}
# line 295 "../MM4005_trajectoryScan.st"
			strcpy((pVar->buildMessage), "Verifying trajectory");
			seq_pvPut(ssId, 15 /* buildMessage */, 0);
			strcpy((pVar->stringOut), "VC");
# line 299 "../MM4005_trajectoryScan.st"
writeOnly(ssId, pVar, pVar->stringOut);
# line 301 "../MM4005_trajectoryScan.st"
writeRead(ssId, pVar, "TB");
# line 305 "../MM4005_trajectoryScan.st"
			if ((pVar->stringIn)[2] == '@')
			{
				(pVar->buildStatus) = 1;
				strcpy((pVar->buildMessage), " ");
			}
# line 308 "../MM4005_trajectoryScan.st"
			else
			{
				(pVar->buildStatus) = 2;
				strncpy((pVar->buildMessage), (pVar->stringIn), 40 - 1);
			}
# line 344 "../MM4005_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
			{
				(pVar->p) = (pVar->stringIn);
				if (!(pVar->moveAxis)[(pVar->j)])
					continue;
				sprintf((pVar->stringOut), "%dRC1", (pVar->j) + 1);
writeRead(ssId, pVar, pVar->stringOut);
				(pVar->motorMDVE)[(pVar->j)] = atoi((pVar->p) + 3);
				seq_pvPut(ssId, 84 /* motorMDVE */ + ((pVar->j)), 0);
				sprintf((pVar->stringOut), "%dRC2", (pVar->j) + 1);
writeRead(ssId, pVar, pVar->stringOut);
				(pVar->motorMDVA)[(pVar->j)] = atof((pVar->p) + 3);
				seq_pvPut(ssId, 76 /* motorMDVA */ + ((pVar->j)), 0);
				sprintf((pVar->stringOut), "%dRC3", (pVar->j) + 1);
writeRead(ssId, pVar, pVar->stringOut);
				(pVar->motorMVE)[(pVar->j)] = atoi((pVar->p) + 3);
				seq_pvPut(ssId, 100 /* motorMVE */ + ((pVar->j)), 0);
				sprintf((pVar->stringOut), "%dRC4", (pVar->j) + 1);
writeRead(ssId, pVar, pVar->stringOut);
				(pVar->motorMVA)[(pVar->j)] = atof((pVar->p) + 3);
				seq_pvPut(ssId, 92 /* motorMVA */ + ((pVar->j)), 0);
				sprintf((pVar->stringOut), "%dRC5", (pVar->j) + 1);
writeRead(ssId, pVar, pVar->stringOut);
				(pVar->motorMAE)[(pVar->j)] = atoi((pVar->p) + 3);
				seq_pvPut(ssId, 116 /* motorMAE */ + ((pVar->j)), 0);
				sprintf((pVar->stringOut), "%dRC6", (pVar->j) + 1);
writeRead(ssId, pVar, pVar->stringOut);
				(pVar->motorMAA)[(pVar->j)] = atof((pVar->p) + 3);
				seq_pvPut(ssId, 108 /* motorMAA */ + ((pVar->j)), 0);
			}
# line 346 "../MM4005_trajectoryScan.st"
			(pVar->buildState) = 0;
			seq_pvPut(ssId, 13 /* buildState */, 0);
			seq_pvPut(ssId, 14 /* buildStatus */, 0);
			seq_pvPut(ssId, 15 /* buildMessage */, 0);
# line 352 "../MM4005_trajectoryScan.st"
			(pVar->build) = 0;
			seq_pvPut(ssId, 12 /* build */, 0);
		}
		return;
	}
}
/* Code for state "execute" in state set "trajectoryScan" */

/* Delay function for state "execute" in state set "trajectoryScan" */
static void D_trajectoryScan_execute(SS_ID ssId, struct UserVar *pVar)
{
# line 387 "../MM4005_trajectoryScan.st"
}

/* Event function for state "execute" in state set "trajectoryScan" */
static long E_trajectoryScan_execute(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 387 "../MM4005_trajectoryScan.st"
	if (TRUE)
	{
		*pNextState = 4;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "execute" in state set "trajectoryScan" */
static void A_trajectoryScan_execute(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 361 "../MM4005_trajectoryScan.st"
			(pVar->execState) = 1;
			seq_pvPut(ssId, 18 /* execState */, 0);
# line 364 "../MM4005_trajectoryScan.st"
			(pVar->execStatus) = 0;
			seq_pvPut(ssId, 19 /* execStatus */, 0);
# line 367 "../MM4005_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
				(pVar->initialPos)[(pVar->j)] = (pVar->epicsMotorPos)[(pVar->j)];
# line 376 "../MM4005_trajectoryScan.st"
			if ((pVar->moveMode) == 1)
			{
				for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
				{
					if (!(pVar->moveAxis)[(pVar->j)])
						continue;
					(pVar->epicsMotorPos)[(pVar->j)] = (pVar->motorTrajectory)[(pVar->j)][0];
					seq_pvPut(ssId, 124 /* epicsMotorPos */ + ((pVar->j)), 0);
				}
waitEpicsMotors(ssId, pVar);
			}
# line 379 "../MM4005_trajectoryScan.st"
			sprintf((pVar->stringOut), "LS,%dEC%f", (pVar->simMode), (pVar->timeScale));
# line 381 "../MM4005_trajectoryScan.st"
writeOnly(ssId, pVar, pVar->stringOut);
			(pVar->startTime) = time(0);
			(pVar->execState) = 2;
			seq_pvPut(ssId, 18 /* execState */, 0);
# line 386 "../MM4005_trajectoryScan.st"
			epicsThreadSleep(0.1);
		}
		return;
	}
}
/* Code for state "wait_execute" in state set "trajectoryScan" */

/* Delay function for state "wait_execute" in state set "trajectoryScan" */
static void D_trajectoryScan_wait_execute(SS_ID ssId, struct UserVar *pVar)
{
# line 403 "../MM4005_trajectoryScan.st"
# line 437 "../MM4005_trajectoryScan.st"
# line 458 "../MM4005_trajectoryScan.st"
}

/* Event function for state "wait_execute" in state set "trajectoryScan" */
static long E_trajectoryScan_wait_execute(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 403 "../MM4005_trajectoryScan.st"
	if ((pVar->execStatus) == 3)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 437 "../MM4005_trajectoryScan.st"
	if ((pVar->execState) == 2)
	{
		*pNextState = 4;
		*pTransNum = 1;
		return TRUE;
	}
# line 458 "../MM4005_trajectoryScan.st"
	if ((pVar->execState) == 3)
	{
		*pNextState = 1;
		*pTransNum = 2;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "wait_execute" in state set "trajectoryScan" */
static void A_trajectoryScan_wait_execute(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 397 "../MM4005_trajectoryScan.st"
			(pVar->execState) = 0;
			seq_pvPut(ssId, 18 /* execState */, 0);
# line 401 "../MM4005_trajectoryScan.st"
			(pVar->execute) = 0;
			seq_pvPut(ssId, 17 /* execute */, 0);
		}
		return;
	case 1:
		{
# line 408 "../MM4005_trajectoryScan.st"
getMotorPositions(ssId, pVar, pVar->motorCurrent);
# line 408 "../MM4005_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
				seq_pvPut(ssId, 60 /* motorCurrent */ + ((pVar->j)), 0);
# line 411 "../MM4005_trajectoryScan.st"
writeRead(ssId, pVar, "XC1");
			(pVar->ncomplete) = atoi( & (pVar->stringIn)[2]);
# line 414 "../MM4005_trajectoryScan.st"
			sprintf((pVar->execMessage), "Executing element %d/%d", (pVar->ncomplete), (pVar->nelements));
			seq_pvPut(ssId, 20 /* execMessage */, 0);
# line 417 "../MM4005_trajectoryScan.st"
pVar->anyMoving = getMotorMoving(ssId, pVar);
# line 421 "../MM4005_trajectoryScan.st"
			if (!(pVar->anyMoving))
			{
				(pVar->execState) = 3;
				(pVar->execStatus) = 1;
				strcpy((pVar->execMessage), " ");
			}
			if (difftime(time(0), (pVar->startTime)) > (pVar->expectedTime) * (pVar->timeScale) * 2.)
			{
				(pVar->execState) = 3;
				(pVar->execStatus) = 4;
				strcpy((pVar->execMessage), "Timeout");
			}
# line 430 "../MM4005_trajectoryScan.st"
writeRead(ssId, pVar, "TB");
# line 436 "../MM4005_trajectoryScan.st"
			if ((pVar->stringIn)[2] != '@')
			{
				(pVar->execState) = 3;
				(pVar->execStatus) = 2;
				strncpy((pVar->execMessage), (pVar->stringIn), 40 - 1);
			}
		}
		return;
	case 2:
		{
# line 440 "../MM4005_trajectoryScan.st"
			seq_pvPut(ssId, 18 /* execState */, 0);
			seq_pvPut(ssId, 19 /* execStatus */, 0);
			seq_pvPut(ssId, 20 /* execMessage */, 0);
# line 445 "../MM4005_trajectoryScan.st"
getMotorPositions(ssId, pVar, pVar->motorCurrent);
# line 445 "../MM4005_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
				seq_pvPut(ssId, 60 /* motorCurrent */ + ((pVar->j)), 0);
# line 450 "../MM4005_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
			{
				if (!(pVar->moveAxis)[(pVar->j)])
					continue;
				(pVar->epicsMotorPos)[(pVar->j)] = (pVar->motorCurrent)[(pVar->j)];
				seq_pvPut(ssId, 124 /* epicsMotorPos */ + ((pVar->j)), 0);
			}
# line 452 "../MM4005_trajectoryScan.st"
waitEpicsMotors(ssId, pVar);
# line 452 "../MM4005_trajectoryScan.st"
			(pVar->execState) = 0;
			seq_pvPut(ssId, 18 /* execState */, 0);
# line 456 "../MM4005_trajectoryScan.st"
			(pVar->execute) = 0;
			seq_pvPut(ssId, 17 /* execute */, 0);
		}
		return;
	}
}
/* Code for state "readback" in state set "trajectoryScan" */

/* Delay function for state "readback" in state set "trajectoryScan" */
static void D_trajectoryScan_readback(SS_ID ssId, struct UserVar *pVar)
{
# line 522 "../MM4005_trajectoryScan.st"
}

/* Event function for state "readback" in state set "trajectoryScan" */
static long E_trajectoryScan_readback(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 522 "../MM4005_trajectoryScan.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "readback" in state set "trajectoryScan" */
static void A_trajectoryScan_readback(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 466 "../MM4005_trajectoryScan.st"
			(pVar->readState) = 1;
			seq_pvPut(ssId, 23 /* readState */, 0);
			(pVar->readStatus) = 0;
			seq_pvPut(ssId, 24 /* readStatus */, 0);
# line 476 "../MM4005_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
			{
				for ((pVar->i) = 0; (pVar->i) < 2000; (pVar->i)++)
				{
					(pVar->motorReadbacks)[(pVar->j)][(pVar->i)] = 0.;
					(pVar->motorError)[(pVar->j)][(pVar->i)] = 0.;
				}
			}
# line 479 "../MM4005_trajectoryScan.st"
writeRead(ssId, pVar, "NQ");
			(pVar->nactual) = atoi( & (pVar->stringIn)[2]);
			seq_pvPut(ssId, 6 /* nactual */, 0);
# line 504 "../MM4005_trajectoryScan.st"
			for ((pVar->i) = 0; (pVar->i) < (pVar->nactual); (pVar->i)++)
			{
				sprintf((pVar->readMessage), "Reading point %d/%d", (pVar->i) + 1, (pVar->nactual));
				seq_pvPut(ssId, 25 /* readMessage */, 0);
				sprintf((pVar->stringOut), "%dTQ", (pVar->i) + 1);
writeRead(ssId, pVar, pVar->stringOut);
				(pVar->tok_save) = 0;
				(pVar->p) = epicsStrtok_r((pVar->stringIn), ",",  & (pVar->tok_save));
				for ((pVar->j) = 0; ((pVar->j) < (pVar->numAxes) && (pVar->p) != 0); (pVar->j)++)
				{
					(pVar->p) = epicsStrtok_r(0, ",",  & (pVar->tok_save));
					(pVar->posTheory) = atof((pVar->p) + 3);
					(pVar->p) = epicsStrtok_r(0, ",",  & (pVar->tok_save));
					if ((pVar->epicsMotorDir)[(pVar->j)] == 0)
						(pVar->dir) = 1;
					else
						(pVar->dir) = -1;
					(pVar->posActual) = atof((pVar->p) + 3);
					(pVar->motorError)[(pVar->j)][(pVar->i)] = (pVar->posActual) - (pVar->posTheory);
					(pVar->posActual) = (pVar->posActual) * (pVar->dir) + (pVar->epicsMotorOff)[(pVar->j)];
					(pVar->motorReadbacks)[(pVar->j)][(pVar->i)] = (pVar->posActual);
				}
			}
# line 509 "../MM4005_trajectoryScan.st"
			for ((pVar->j) = 0; (pVar->j) < (pVar->numAxes); (pVar->j)++)
			{
				seq_pvPut(ssId, 44 /* motorReadbacks */ + ((pVar->j)), 0);
				seq_pvPut(ssId, 52 /* motorError */ + ((pVar->j)), 0);
			}
# line 511 "../MM4005_trajectoryScan.st"
			(pVar->readState) = 0;
			seq_pvPut(ssId, 23 /* readState */, 0);
# line 514 "../MM4005_trajectoryScan.st"
			(pVar->readStatus) = 1;
			seq_pvPut(ssId, 24 /* readStatus */, 0);
			strcpy((pVar->readMessage), " ");
			seq_pvPut(ssId, 25 /* readMessage */, 0);
# line 520 "../MM4005_trajectoryScan.st"
			(pVar->readback) = 0;
			seq_pvPut(ssId, 22 /* readback */, 0);
		}
		return;
	}
}
/* Code for state "monitorAbort" in state set "trajectoryAbort" */

/* Delay function for state "monitorAbort" in state set "trajectoryAbort" */
static void D_trajectoryAbort_monitorAbort(SS_ID ssId, struct UserVar *pVar)
{
# line 545 "../MM4005_trajectoryScan.st"
}

/* Event function for state "monitorAbort" in state set "trajectoryAbort" */
static long E_trajectoryAbort_monitorAbort(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 545 "../MM4005_trajectoryScan.st"
	if (seq_efTestAndClear(ssId, abortMon) && ((pVar->abort) == 1))
	{
		*pNextState = 0;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "monitorAbort" in state set "trajectoryAbort" */
static void A_trajectoryAbort_monitorAbort(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 535 "../MM4005_trajectoryScan.st"
			strcpy((pVar->stringOut), "AB");
# line 537 "../MM4005_trajectoryScan.st"
writeOnly(ssId, pVar, pVar->stringOut);
# line 537 "../MM4005_trajectoryScan.st"
			(pVar->execStatus) = 3;
			seq_pvPut(ssId, 19 /* execStatus */, 0);
			strcpy((pVar->execMessage), "Motion aborted");
			seq_pvPut(ssId, 20 /* execMessage */, 0);
# line 543 "../MM4005_trajectoryScan.st"
			(pVar->abort) = 0;
			seq_pvPut(ssId, 21 /* abort */, 0);
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

/* Event masks for state set trajectoryScan */
	/* Event mask for state init: */
static bitMask	EM_trajectoryScan_init[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state monitor_inputs: */
static bitMask	EM_trajectoryScan_monitor_inputs[] = {
	0x421004e6,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state build: */
static bitMask	EM_trajectoryScan_build[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state execute: */
static bitMask	EM_trajectoryScan_execute[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state wait_execute: */
static bitMask	EM_trajectoryScan_wait_execute[] = {
	0x0c000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
	/* Event mask for state readback: */
static bitMask	EM_trajectoryScan_readback[] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_trajectoryScan[] = {
	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_trajectoryScan_init,
	/* event function */  (EVENT_FUNC) E_trajectoryScan_init,
	/* delay function */   (DELAY_FUNC) D_trajectoryScan_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_trajectoryScan_init,
	/* state options */   (0)},

	/* State "monitor_inputs" */ {
	/* state name */       "monitor_inputs",
	/* action function */ (ACTION_FUNC) A_trajectoryScan_monitor_inputs,
	/* event function */  (EVENT_FUNC) E_trajectoryScan_monitor_inputs,
	/* delay function */   (DELAY_FUNC) D_trajectoryScan_monitor_inputs,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_trajectoryScan_monitor_inputs,
	/* state options */   (0)},

	/* State "build" */ {
	/* state name */       "build",
	/* action function */ (ACTION_FUNC) A_trajectoryScan_build,
	/* event function */  (EVENT_FUNC) E_trajectoryScan_build,
	/* delay function */   (DELAY_FUNC) D_trajectoryScan_build,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_trajectoryScan_build,
	/* state options */   (0)},

	/* State "execute" */ {
	/* state name */       "execute",
	/* action function */ (ACTION_FUNC) A_trajectoryScan_execute,
	/* event function */  (EVENT_FUNC) E_trajectoryScan_execute,
	/* delay function */   (DELAY_FUNC) D_trajectoryScan_execute,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_trajectoryScan_execute,
	/* state options */   (0)},

	/* State "wait_execute" */ {
	/* state name */       "wait_execute",
	/* action function */ (ACTION_FUNC) A_trajectoryScan_wait_execute,
	/* event function */  (EVENT_FUNC) E_trajectoryScan_wait_execute,
	/* delay function */   (DELAY_FUNC) D_trajectoryScan_wait_execute,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_trajectoryScan_wait_execute,
	/* state options */   (0)},

	/* State "readback" */ {
	/* state name */       "readback",
	/* action function */ (ACTION_FUNC) A_trajectoryScan_readback,
	/* event function */  (EVENT_FUNC) E_trajectoryScan_readback,
	/* delay function */   (DELAY_FUNC) D_trajectoryScan_readback,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_trajectoryScan_readback,
	/* state options */   (0)},


};

/* Event masks for state set trajectoryAbort */
	/* Event mask for state monitorAbort: */
static bitMask	EM_trajectoryAbort_monitorAbort[] = {
	0x20000010,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

/* State Blocks */

static struct seqState state_trajectoryAbort[] = {
	/* State "monitorAbort" */ {
	/* state name */       "monitorAbort",
	/* action function */ (ACTION_FUNC) A_trajectoryAbort_monitorAbort,
	/* event function */  (EVENT_FUNC) E_trajectoryAbort_monitorAbort,
	/* delay function */   (DELAY_FUNC) D_trajectoryAbort_monitorAbort,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_trajectoryAbort_monitorAbort,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "trajectoryScan" */ {
	/* ss name */            "trajectoryScan",
	/* ptr to state block */ state_trajectoryScan,
	/* number of states */   6,
	/* error state */        -1},


	/* State set "trajectoryAbort" */ {
	/* ss name */            "trajectoryAbort",
	/* ptr to state block */ state_trajectoryAbort,
	/* number of states */   1,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "P=13IDC:,R=traj1,M1=M1,M2=M2,M3=M3,M4=M4,M5=M5,M6=M6,M7=M7,M8=M8,PORT=serial1";

/* State Program table (global) */
struct seqProgram MM4005_trajectoryScan = {
	/* magic number */       20000315,
	/* *name */              "MM4005_trajectoryScan",
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
# line 552 "../MM4005_trajectoryScan.st"



static int writeOnly(SS_ID ssId, struct UserVar *pVar, char *command)
{
    asynStatus status;
    size_t nwrite;
    char buffer[256];


    strncpy(buffer, command, 256 -3);
    strcat(buffer, "\r");
    status = pasynOctetSyncIO->write((asynUser *)pVar->pasynUser, buffer,
                               strlen(buffer), 1.0, &nwrite);
    return(status);
}



static int writeRead(SS_ID ssId, struct UserVar *pVar, char *command)
{
    asynStatus status;
    size_t nwrite, nread;
    int eomReason;
    char buffer[256];


    strncpy(buffer, command, 256 -3);
    strcat(buffer, "\r");

    status = pasynOctetSyncIO->writeRead((asynUser *)pVar->pasynUser, buffer,
                               strlen(buffer), pVar->stringIn, 256,
                               30.0, &nwrite, &nread, &eomReason);
    return(status);
}



static int getMotorPositions(SS_ID ssId, struct UserVar *pVar, double *pos)
{
    char *p, *tok_save;
    int j;
    int dir;


    writeRead(ssId, pVar, "TP");


    tok_save = 0;
    p = epicsStrtok_r(pVar->stringIn, ",", &tok_save);
    for (j=0; (j<pVar->numAxes && p!=0); j++) {
        if (pVar->epicsMotorDir[j] == 0) dir=1; else dir=-1;
        pos[j] = atof(p+3)*dir + pVar->epicsMotorOff[j];
        p = epicsStrtok_r(0, ",", &tok_save);
    }
    return(0);
}





static int getMotorMoving(SS_ID ssId, struct UserVar *pVar)
{
    char *p, *tok_save;
    int j;
    int result=0, mask=0x01;


    writeRead(ssId, pVar, "MS");


    tok_save = 0;
    p = epicsStrtok_r(pVar->stringIn, ",", &tok_save);
    for (j=0; (j<pVar->numAxes && p!=0); j++) {

        if (*(p+3) & 0x01) result |= mask;
        mask = mask << 1;
        p = epicsStrtok_r(0, ",", &tok_save);
    }
    return(result);
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


#include "epicsExport.h"


/* Register sequencer commands and program */

void MM4005_trajectoryScanRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&MM4005_trajectoryScan);
}
epicsExportRegistrar(MM4005_trajectoryScanRegistrar);

