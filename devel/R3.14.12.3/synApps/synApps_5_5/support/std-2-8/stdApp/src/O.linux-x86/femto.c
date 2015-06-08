/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: femto.i */

/* Event flags */
#define t0_mon	1
#define t1_mon	2
#define t2_mon	3
#define tx_mon	4
#define gainidx_mon	5
#define debug_flag_mon	6

/* Program "femto" */
#include "seqCom.h"

#define NUM_SS 1
#define NUM_CHANNELS 7
#define NUM_EVENTS 6
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT FALSE
#define DEBUG_OPT TRUE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT TRUE

extern struct seqProgram femto;

/* Variable declarations */
struct UserVar {
	int	t0;
	int	t1;
	int	t2;
	int	tx;
	double	gain;
	int	gainidx;
	int	debug_flag;
	int	curgain;
	char	msg[256];
	char	*SNLtaskName;
};

/* C code definitions */
# line 23 "../femto.st"
#include <stdio.h>
# line 24 "../femto.st"
#include <math.h>
# line 43 "../femto.st"
static int powers[] = {5,6,7,8,9,10,11,0,3,4,5,6,7,8,9,0};

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "init" in state set "femto" */

/* Delay function for state "init" in state set "femto" */
static void D_femto_init(SS_ID ssId, struct UserVar *pVar)
{
# line 90 "../femto.st"
}

/* Event function for state "init" in state set "femto" */
static long E_femto_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 90 "../femto.st"
	if (TRUE)
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "femto" */
static void A_femto_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 51 "../femto.st"
			(pVar->SNLtaskName) = seq_macValueGet(ssId, "name");
# line 53 "../femto.st"
			seq_pvGet(ssId, 0 /* t0 */, 2);
			seq_pvGet(ssId, 1 /* t1 */, 2);
			seq_pvGet(ssId, 2 /* t2 */, 2);
			seq_pvGet(ssId, 3 /* tx */, 2);
# line 62 "../femto.st"
			if (((pVar->tx) + (pVar->t2) + (pVar->t1) + (pVar->t0)) == 0)
			{
				(pVar->gainidx) = 8;
				if ((pVar->debug_flag) >= 2)
				{
					printf("<%s,%d,%s,%d> ", "../femto.st", 61, (pVar->SNLtaskName), 2);
					;
					printf("%s\n", "gainidx = default (1e3 since all bits were OFF)");
					epicsThreadSleep(0.01);
				}
				;
			}
# line 68 "../femto.st"
			else
			{
				(pVar->gainidx) = ((pVar->tx) << 3) | ((pVar->t2) << 2) | ((pVar->t1) << 1) | (pVar->t0);
				sprintf((pVar->msg), "gainidx = %d", (pVar->gainidx));
				if ((pVar->debug_flag) >= 2)
				{
					printf("<%s,%d,%s,%d> ", "../femto.st", 67, (pVar->SNLtaskName), 2);
					;
					printf("%s\n", (pVar->msg));
					epicsThreadSleep(0.01);
				}
				;
			}
# line 74 "../femto.st"
			if ((pVar->gainidx) >= 15 || (pVar->gainidx) == 7)
			{
				(pVar->gainidx) = 6;
				if ((pVar->debug_flag) >= 4)
				{
					printf("<%s,%d,%s,%d> ", "../femto.st", 73, (pVar->SNLtaskName), 4);
					;
					printf("%s\n", "resetting gain to 1e11");
					epicsThreadSleep(0.01);
				}
				;
			}
# line 76 "../femto.st"
			(pVar->curgain) = -1;
			{
				(pVar->gainidx) = ((pVar->gainidx));
				seq_pvPut(ssId, 5 /* gainidx */, 2);
			}
# line 77 "../femto.st"
			;
			{
				(pVar->gain) = (pow(10, powers[(pVar->gainidx)]));
				seq_pvPut(ssId, 4 /* gain */, 2);
			}
# line 78 "../femto.st"
			;
			sprintf((pVar->msg), "gainidx = %d", (pVar->gainidx));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../femto.st", 80, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 80 "../femto.st"
			;
# line 82 "../femto.st"
			seq_efClear(ssId, t0_mon);
			seq_efClear(ssId, t1_mon);
			seq_efClear(ssId, t2_mon);
			seq_efClear(ssId, tx_mon);
			seq_efClear(ssId, gainidx_mon);
			seq_efClear(ssId, debug_flag_mon);
# line 89 "../femto.st"
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../femto.st", 89, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "init complete");
				epicsThreadSleep(0.01);
			}
# line 89 "../femto.st"
			;
		}
		return;
	}
}
/* Code for state "updateGain" in state set "femto" */

/* Delay function for state "updateGain" in state set "femto" */
static void D_femto_updateGain(SS_ID ssId, struct UserVar *pVar)
{
# line 114 "../femto.st"
}

/* Event function for state "updateGain" in state set "femto" */
static long E_femto_updateGain(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 114 "../femto.st"
	if (TRUE)
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "updateGain" in state set "femto" */
static void A_femto_updateGain(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 98 "../femto.st"
			(pVar->gainidx) = ((pVar->tx) << 3) | ((pVar->t2) << 2) | ((pVar->t1) << 1) | (pVar->t0);
			sprintf((pVar->msg), "gainidx = %d", (pVar->gainidx));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../femto.st", 100, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 100 "../femto.st"
			;
# line 106 "../femto.st"
			if ((pVar->gainidx) >= 15 || (pVar->gainidx) == 7)
			{
				(pVar->gainidx) = 6;
				if ((pVar->debug_flag) >= 4)
				{
					printf("<%s,%d,%s,%d> ", "../femto.st", 105, (pVar->SNLtaskName), 4);
					;
					printf("%s\n", "resetting gain to 1e11");
					epicsThreadSleep(0.01);
				}
				;
			}
# line 108 "../femto.st"
			(pVar->curgain) = (pVar->gainidx);
			{
				(pVar->gainidx) = ((pVar->gainidx));
				seq_pvPut(ssId, 5 /* gainidx */, 2);
			}
# line 109 "../femto.st"
			;
			{
				(pVar->gain) = (pow(10, powers[(pVar->gainidx)]));
				seq_pvPut(ssId, 4 /* gain */, 2);
			}
# line 110 "../femto.st"
			;
			sprintf((pVar->msg), "gainidx = %d", (pVar->gainidx));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../femto.st", 112, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 112 "../femto.st"
			;
		}
		return;
	}
}
/* Code for state "changeGain" in state set "femto" */

/* Delay function for state "changeGain" in state set "femto" */
static void D_femto_changeGain(SS_ID ssId, struct UserVar *pVar)
{
# line 123 "../femto.st"
# line 131 "../femto.st"
# line 139 "../femto.st"
# line 147 "../femto.st"
# line 289 "../femto.st"
}

/* Event function for state "changeGain" in state set "femto" */
static long E_femto_changeGain(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 123 "../femto.st"
	if ((pVar->curgain) == (pVar->gainidx))
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
# line 131 "../femto.st"
	if ((pVar->gainidx) < 0)
	{
		*pNextState = 3;
		*pTransNum = 1;
		return TRUE;
	}
# line 139 "../femto.st"
	if ((pVar->gainidx) > 15)
	{
		*pNextState = 3;
		*pTransNum = 2;
		return TRUE;
	}
# line 147 "../femto.st"
	if ((pVar->gainidx) == 7 || (pVar->gainidx) == 15)
	{
		*pNextState = 3;
		*pTransNum = 3;
		return TRUE;
	}
# line 289 "../femto.st"
	if (TRUE)
	{
		*pNextState = 3;
		*pTransNum = 4;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "changeGain" in state set "femto" */
static void A_femto_changeGain(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 122 "../femto.st"
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../femto.st", 122, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "No gain change required");
				epicsThreadSleep(0.01);
			}
# line 122 "../femto.st"
			;
		}
		return;
	case 1:
		{
# line 127 "../femto.st"
			{
				(pVar->gainidx) = ((pVar->curgain));
				seq_pvPut(ssId, 5 /* gainidx */, 2);
			}
# line 127 "../femto.st"
			;
			{
				(pVar->gain) = (pow(10, powers[(pVar->curgain)]));
				seq_pvPut(ssId, 4 /* gain */, 2);
			}
# line 128 "../femto.st"
			;
			sprintf((pVar->msg), "Requested gain (%d) too low!", (pVar->gainidx));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../femto.st", 130, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 130 "../femto.st"
			;
		}
		return;
	case 2:
		{
# line 135 "../femto.st"
			{
				(pVar->gainidx) = ((pVar->curgain));
				seq_pvPut(ssId, 5 /* gainidx */, 2);
			}
# line 135 "../femto.st"
			;
			{
				(pVar->gain) = (pow(10, powers[(pVar->curgain)]));
				seq_pvPut(ssId, 4 /* gain */, 2);
			}
# line 136 "../femto.st"
			;
			sprintf((pVar->msg), "Requested gain (%d) too high!", (pVar->gainidx));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../femto.st", 138, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 138 "../femto.st"
			;
		}
		return;
	case 3:
		{
# line 143 "../femto.st"
			{
				(pVar->gainidx) = ((pVar->curgain));
				seq_pvPut(ssId, 5 /* gainidx */, 2);
			}
# line 143 "../femto.st"
			;
			{
				(pVar->gain) = (pow(10, powers[(pVar->curgain)]));
				seq_pvPut(ssId, 4 /* gain */, 2);
			}
# line 144 "../femto.st"
			;
			sprintf((pVar->msg), "Requested gain (%d) invalid!", (pVar->gainidx));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../femto.st", 146, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 146 "../femto.st"
			;
		}
		return;
	case 4:
		{
# line 151 "../femto.st"
			sprintf((pVar->msg), "gain %d selected", (pVar->gainidx));
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../femto.st", 152, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 152 "../femto.st"
			;
# line 161 "../femto.st"
			if ((pVar->gainidx) == 0)
			{
				{
					(pVar->t0) = (0);
					seq_pvPut(ssId, 0 /* t0 */, 2);
				}
				;
				{
					(pVar->t1) = (0);
					seq_pvPut(ssId, 1 /* t1 */, 2);
				}
				;
				{
					(pVar->t2) = (0);
					seq_pvPut(ssId, 2 /* t2 */, 2);
				}
				;
				{
					(pVar->tx) = (0);
					seq_pvPut(ssId, 3 /* tx */, 2);
				}
				;
			}
# line 170 "../femto.st"
			if ((pVar->gainidx) == 1)
			{
				{
					(pVar->t0) = (1);
					seq_pvPut(ssId, 0 /* t0 */, 2);
				}
				;
				{
					(pVar->t1) = (0);
					seq_pvPut(ssId, 1 /* t1 */, 2);
				}
				;
				{
					(pVar->t2) = (0);
					seq_pvPut(ssId, 2 /* t2 */, 2);
				}
				;
				{
					(pVar->tx) = (0);
					seq_pvPut(ssId, 3 /* tx */, 2);
				}
				;
			}
# line 179 "../femto.st"
			if ((pVar->gainidx) == 2)
			{
				{
					(pVar->t0) = (0);
					seq_pvPut(ssId, 0 /* t0 */, 2);
				}
				;
				{
					(pVar->t1) = (1);
					seq_pvPut(ssId, 1 /* t1 */, 2);
				}
				;
				{
					(pVar->t2) = (0);
					seq_pvPut(ssId, 2 /* t2 */, 2);
				}
				;
				{
					(pVar->tx) = (0);
					seq_pvPut(ssId, 3 /* tx */, 2);
				}
				;
			}
# line 188 "../femto.st"
			if ((pVar->gainidx) == 3)
			{
				{
					(pVar->t0) = (1);
					seq_pvPut(ssId, 0 /* t0 */, 2);
				}
				;
				{
					(pVar->t1) = (1);
					seq_pvPut(ssId, 1 /* t1 */, 2);
				}
				;
				{
					(pVar->t2) = (0);
					seq_pvPut(ssId, 2 /* t2 */, 2);
				}
				;
				{
					(pVar->tx) = (0);
					seq_pvPut(ssId, 3 /* tx */, 2);
				}
				;
			}
# line 197 "../femto.st"
			if ((pVar->gainidx) == 4)
			{
				{
					(pVar->t0) = (0);
					seq_pvPut(ssId, 0 /* t0 */, 2);
				}
				;
				{
					(pVar->t1) = (0);
					seq_pvPut(ssId, 1 /* t1 */, 2);
				}
				;
				{
					(pVar->t2) = (1);
					seq_pvPut(ssId, 2 /* t2 */, 2);
				}
				;
				{
					(pVar->tx) = (0);
					seq_pvPut(ssId, 3 /* tx */, 2);
				}
				;
			}
# line 206 "../femto.st"
			if ((pVar->gainidx) == 5)
			{
				{
					(pVar->t0) = (1);
					seq_pvPut(ssId, 0 /* t0 */, 2);
				}
				;
				{
					(pVar->t1) = (0);
					seq_pvPut(ssId, 1 /* t1 */, 2);
				}
				;
				{
					(pVar->t2) = (1);
					seq_pvPut(ssId, 2 /* t2 */, 2);
				}
				;
				{
					(pVar->tx) = (0);
					seq_pvPut(ssId, 3 /* tx */, 2);
				}
				;
			}
# line 215 "../femto.st"
			if ((pVar->gainidx) == 6)
			{
				{
					(pVar->t0) = (0);
					seq_pvPut(ssId, 0 /* t0 */, 2);
				}
				;
				{
					(pVar->t1) = (1);
					seq_pvPut(ssId, 1 /* t1 */, 2);
				}
				;
				{
					(pVar->t2) = (1);
					seq_pvPut(ssId, 2 /* t2 */, 2);
				}
				;
				{
					(pVar->tx) = (0);
					seq_pvPut(ssId, 3 /* tx */, 2);
				}
				;
			}
# line 224 "../femto.st"
			if ((pVar->gainidx) == 8)
			{
				{
					(pVar->t0) = (0);
					seq_pvPut(ssId, 0 /* t0 */, 2);
				}
				;
				{
					(pVar->t1) = (0);
					seq_pvPut(ssId, 1 /* t1 */, 2);
				}
				;
				{
					(pVar->t2) = (0);
					seq_pvPut(ssId, 2 /* t2 */, 2);
				}
				;
				{
					(pVar->tx) = (1);
					seq_pvPut(ssId, 3 /* tx */, 2);
				}
				;
			}
# line 233 "../femto.st"
			if ((pVar->gainidx) == 9)
			{
				{
					(pVar->t0) = (1);
					seq_pvPut(ssId, 0 /* t0 */, 2);
				}
				;
				{
					(pVar->t1) = (0);
					seq_pvPut(ssId, 1 /* t1 */, 2);
				}
				;
				{
					(pVar->t2) = (0);
					seq_pvPut(ssId, 2 /* t2 */, 2);
				}
				;
				{
					(pVar->tx) = (1);
					seq_pvPut(ssId, 3 /* tx */, 2);
				}
				;
			}
# line 242 "../femto.st"
			if ((pVar->gainidx) == 10)
			{
				{
					(pVar->t0) = (0);
					seq_pvPut(ssId, 0 /* t0 */, 2);
				}
				;
				{
					(pVar->t1) = (1);
					seq_pvPut(ssId, 1 /* t1 */, 2);
				}
				;
				{
					(pVar->t2) = (0);
					seq_pvPut(ssId, 2 /* t2 */, 2);
				}
				;
				{
					(pVar->tx) = (1);
					seq_pvPut(ssId, 3 /* tx */, 2);
				}
				;
			}
# line 251 "../femto.st"
			if ((pVar->gainidx) == 11)
			{
				{
					(pVar->t0) = (1);
					seq_pvPut(ssId, 0 /* t0 */, 2);
				}
				;
				{
					(pVar->t1) = (1);
					seq_pvPut(ssId, 1 /* t1 */, 2);
				}
				;
				{
					(pVar->t2) = (0);
					seq_pvPut(ssId, 2 /* t2 */, 2);
				}
				;
				{
					(pVar->tx) = (1);
					seq_pvPut(ssId, 3 /* tx */, 2);
				}
				;
			}
# line 260 "../femto.st"
			if ((pVar->gainidx) == 12)
			{
				{
					(pVar->t0) = (0);
					seq_pvPut(ssId, 0 /* t0 */, 2);
				}
				;
				{
					(pVar->t1) = (0);
					seq_pvPut(ssId, 1 /* t1 */, 2);
				}
				;
				{
					(pVar->t2) = (1);
					seq_pvPut(ssId, 2 /* t2 */, 2);
				}
				;
				{
					(pVar->tx) = (1);
					seq_pvPut(ssId, 3 /* tx */, 2);
				}
				;
			}
# line 269 "../femto.st"
			if ((pVar->gainidx) == 13)
			{
				{
					(pVar->t0) = (1);
					seq_pvPut(ssId, 0 /* t0 */, 2);
				}
				;
				{
					(pVar->t1) = (0);
					seq_pvPut(ssId, 1 /* t1 */, 2);
				}
				;
				{
					(pVar->t2) = (1);
					seq_pvPut(ssId, 2 /* t2 */, 2);
				}
				;
				{
					(pVar->tx) = (1);
					seq_pvPut(ssId, 3 /* tx */, 2);
				}
				;
			}
# line 278 "../femto.st"
			if ((pVar->gainidx) == 14)
			{
				{
					(pVar->t0) = (0);
					seq_pvPut(ssId, 0 /* t0 */, 2);
				}
				;
				{
					(pVar->t1) = (1);
					seq_pvPut(ssId, 1 /* t1 */, 2);
				}
				;
				{
					(pVar->t2) = (1);
					seq_pvPut(ssId, 2 /* t2 */, 2);
				}
				;
				{
					(pVar->tx) = (1);
					seq_pvPut(ssId, 3 /* tx */, 2);
				}
				;
			}
# line 280 "../femto.st"
			(pVar->curgain) = (pVar->gainidx);
			{
				(pVar->gain) = (pow(10, powers[(pVar->gainidx)]));
				seq_pvPut(ssId, 4 /* gain */, 2);
			}
# line 281 "../femto.st"
			;
# line 283 "../femto.st"
			seq_efClear(ssId, t0_mon);
			seq_efClear(ssId, t1_mon);
			seq_efClear(ssId, t2_mon);
			seq_efClear(ssId, tx_mon);
			seq_efClear(ssId, gainidx_mon);
		}
		return;
	}
}
/* Code for state "idle" in state set "femto" */

/* Delay function for state "idle" in state set "femto" */
static void D_femto_idle(SS_ID ssId, struct UserVar *pVar)
{
# line 299 "../femto.st"
# line 306 "../femto.st"
# line 311 "../femto.st"
# line 313 "../femto.st"
	seq_delayInit(ssId, 0, (.1));
}

/* Event function for state "idle" in state set "femto" */
static long E_femto_idle(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 299 "../femto.st"
	if (seq_efTestAndClear(ssId, debug_flag_mon))
	{
		*pNextState = 3;
		*pTransNum = 0;
		return TRUE;
	}
# line 306 "../femto.st"
	if (seq_efTestAndClear(ssId, tx_mon) || seq_efTestAndClear(ssId, t2_mon) || seq_efTestAndClear(ssId, t1_mon) || seq_efTestAndClear(ssId, t0_mon))
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
# line 311 "../femto.st"
	if (seq_efTestAndClear(ssId, gainidx_mon))
	{
		*pNextState = 2;
		*pTransNum = 2;
		return TRUE;
	}
# line 313 "../femto.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 3;
		*pTransNum = 3;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "idle" in state set "femto" */
static void A_femto_idle(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 297 "../femto.st"
			sprintf((pVar->msg), "Debug level changed to %d", (pVar->debug_flag));
			if ((pVar->debug_flag) >= 1)
			{
				printf("<%s,%d,%s,%d> ", "../femto.st", 298, (pVar->SNLtaskName), 1);
				;
				printf("%s\n", (pVar->msg));
				epicsThreadSleep(0.01);
			}
# line 298 "../femto.st"
			;
		}
		return;
	case 1:
		{
# line 305 "../femto.st"
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../femto.st", 305, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "bits changed, updating");
				epicsThreadSleep(0.01);
			}
# line 305 "../femto.st"
			;
		}
		return;
	case 2:
		{
# line 310 "../femto.st"
			if ((pVar->debug_flag) >= 2)
			{
				printf("<%s,%d,%s,%d> ", "../femto.st", 310, (pVar->SNLtaskName), 2);
				;
				printf("%s\n", "gain change");
				epicsThreadSleep(0.01);
			}
# line 310 "../femto.st"
			;
		}
		return;
	case 3:
		{
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
  {"{G1}", (void *)OFFSET(struct UserVar, t0), "t0", 
    "int", 1, 7, 1, 1, 0, 0, 0},

  {"{G2}", (void *)OFFSET(struct UserVar, t1), "t1", 
    "int", 1, 8, 2, 1, 0, 0, 0},

  {"{G3}", (void *)OFFSET(struct UserVar, t2), "t2", 
    "int", 1, 9, 3, 1, 0, 0, 0},

  {"{NO}", (void *)OFFSET(struct UserVar, tx), "tx", 
    "int", 1, 10, 4, 1, 0, 0, 0},

  {"{P}{H}{F}gain", (void *)OFFSET(struct UserVar, gain), "gain", 
    "double", 1, 11, 0, 0, 0, 0, 0},

  {"{P}{H}{F}gainidx", (void *)OFFSET(struct UserVar, gainidx), "gainidx", 
    "int", 1, 12, 5, 1, 0, 0, 0},

  {"{P}{H}{F}debug", (void *)OFFSET(struct UserVar, debug_flag), "debug_flag", 
    "int", 1, 13, 6, 1, 0, 0, 0},

};

/* Event masks for state set femto */
	/* Event mask for state init: */
static bitMask	EM_femto_init[] = {
	0x00000000,
};
	/* Event mask for state updateGain: */
static bitMask	EM_femto_updateGain[] = {
	0x00000000,
};
	/* Event mask for state changeGain: */
static bitMask	EM_femto_changeGain[] = {
	0x00001000,
};
	/* Event mask for state idle: */
static bitMask	EM_femto_idle[] = {
	0x0000007e,
};

/* State Blocks */

static struct seqState state_femto[] = {
	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_femto_init,
	/* event function */  (EVENT_FUNC) E_femto_init,
	/* delay function */   (DELAY_FUNC) D_femto_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_femto_init,
	/* state options */   (0)},

	/* State "updateGain" */ {
	/* state name */       "updateGain",
	/* action function */ (ACTION_FUNC) A_femto_updateGain,
	/* event function */  (EVENT_FUNC) E_femto_updateGain,
	/* delay function */   (DELAY_FUNC) D_femto_updateGain,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_femto_updateGain,
	/* state options */   (0)},

	/* State "changeGain" */ {
	/* state name */       "changeGain",
	/* action function */ (ACTION_FUNC) A_femto_changeGain,
	/* event function */  (EVENT_FUNC) E_femto_changeGain,
	/* delay function */   (DELAY_FUNC) D_femto_changeGain,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_femto_changeGain,
	/* state options */   (0)},

	/* State "idle" */ {
	/* state name */       "idle",
	/* action function */ (ACTION_FUNC) A_femto_idle,
	/* event function */  (EVENT_FUNC) E_femto_idle,
	/* delay function */   (DELAY_FUNC) D_femto_idle,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_femto_idle,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "femto" */ {
	/* ss name */            "femto",
	/* ptr to state block */ state_femto,
	/* number of states */   4,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "name=femto,P=,H=,F=,G1=,G2=,G3=,NO=";

/* State Program table (global) */
struct seqProgram femto = {
	/* magic number */       20000315,
	/* *name */              "femto",
	/* *pChannels */         seqChan,
	/* numChans */           NUM_CHANNELS,
	/* *pSS */               seqSS,
	/* numSS */              NUM_SS,
	/* user variable size */ sizeof(struct UserVar),
	/* *pParams */           prog_param,
	/* numEvents */          NUM_EVENTS,
	/* encoded options */    (0 | OPT_DEBUG | OPT_NEWEF | OPT_REENT),
	/* entry handler */      (ENTRY_FUNC) entry_handler,
	/* exit handler */       (EXIT_FUNC) exit_handler,
	/* numQueues */          NUM_QUEUES,
};
#include "epicsExport.h"


/* Register sequencer commands and program */

void femtoRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&femto);
}
epicsExportRegistrar(femtoRegistrar);

