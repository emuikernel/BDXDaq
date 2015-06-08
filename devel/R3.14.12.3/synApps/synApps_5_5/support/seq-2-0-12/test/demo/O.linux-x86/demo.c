/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: demo.i */

/* Event flags */
#define loFlag	1
#define hiFlag	2

/* Program "demo" */
#include "seqCom.h"

#define NUM_SS 3
#define NUM_CHANNELS 6
#define NUM_EVENTS 2
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT FALSE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT TRUE

extern struct seqProgram demo;

/* Variable declarations */
struct UserVar {
	long	light;
	double	lightOn;
	double	lightOff;
	double	voltage;
	double	loLimit;
	double	hiLimit;
};

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "START" in state set "light" */

/* Delay function for state "START" in state set "light" */
static void D_light_START(SS_ID ssId, struct UserVar *pVar)
{
# line 101 "../demo.st"
}

/* Event function for state "START" in state set "light" */
static long E_light_START(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 101 "../demo.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "START" in state set "light" */
static void A_light_START(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 99 "../demo.st"
			(pVar->light) = 0;
			seq_pvPut(ssId, 0 /* light */, 0);
		}
		return;
	}
}
/* Code for state "LIGHT_OFF" in state set "light" */

/* Delay function for state "LIGHT_OFF" in state set "light" */
static void D_light_LIGHT_OFF(SS_ID ssId, struct UserVar *pVar)
{
# line 109 "../demo.st"
}

/* Event function for state "LIGHT_OFF" in state set "light" */
static long E_light_LIGHT_OFF(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 109 "../demo.st"
	if ((pVar->voltage) > (pVar->lightOn))
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "LIGHT_OFF" in state set "light" */
static void A_light_LIGHT_OFF(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 106 "../demo.st"
			(pVar->light) = 1;
			seq_pvPut(ssId, 0 /* light */, 0);
			seq_seqLog(ssId, "light_off -> light_on\n");
		}
		return;
	}
}
/* Code for state "LIGHT_ON" in state set "light" */

/* Delay function for state "LIGHT_ON" in state set "light" */
static void D_light_LIGHT_ON(SS_ID ssId, struct UserVar *pVar)
{
# line 117 "../demo.st"
}

/* Event function for state "LIGHT_ON" in state set "light" */
static long E_light_LIGHT_ON(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 117 "../demo.st"
	if ((pVar->voltage) < (pVar->lightOff))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "LIGHT_ON" in state set "light" */
static void A_light_LIGHT_ON(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 114 "../demo.st"
			(pVar->light) = 0;
			seq_pvPut(ssId, 0 /* light */, 0);
			seq_seqLog(ssId, "light_on -> light_off\n");
		}
		return;
	}
}
/* Code for state "START" in state set "ramp" */

/* Delay function for state "START" in state set "ramp" */
static void D_ramp_START(SS_ID ssId, struct UserVar *pVar)
{
# line 128 "../demo.st"
}

/* Event function for state "START" in state set "ramp" */
static long E_ramp_START(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 128 "../demo.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "START" in state set "ramp" */
static void A_ramp_START(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 125 "../demo.st"
			(pVar->voltage) = (pVar->loLimit);
			seq_pvPut(ssId, 3 /* voltage */, 0);
			seq_seqLog(ssId, "start -> ramp_up\n");
		}
		return;
	}
}
/* Code for state "RAMP_UP" in state set "ramp" */

/* Delay function for state "RAMP_UP" in state set "ramp" */
static void D_ramp_RAMP_UP(SS_ID ssId, struct UserVar *pVar)
{
# line 134 "../demo.st"
# line 141 "../demo.st"
	seq_delayInit(ssId, 0, (0.1));
}

/* Event function for state "RAMP_UP" in state set "ramp" */
static long E_ramp_RAMP_UP(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 134 "../demo.st"
	if ((pVar->voltage) > (pVar->hiLimit))
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 141 "../demo.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "RAMP_UP" in state set "ramp" */
static void A_ramp_RAMP_UP(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 133 "../demo.st"
			seq_seqLog(ssId, "ramp_up -> ramp_down\n");
		}
		return;
	case 1:
		{
# line 137 "../demo.st"
			(pVar->voltage) += 0.1;
# line 139 "../demo.st"
			if (seq_pvPut(ssId, 3 /* voltage */, 0) < 0)
				seq_seqLog(ssId, "pvPut( voltage ) failed\n");
		}
		return;
	}
}
/* Code for state "RAMP_DOWN" in state set "ramp" */

/* Delay function for state "RAMP_DOWN" in state set "ramp" */
static void D_ramp_RAMP_DOWN(SS_ID ssId, struct UserVar *pVar)
{
# line 147 "../demo.st"
# line 154 "../demo.st"
	seq_delayInit(ssId, 0, (0.1));
}

/* Event function for state "RAMP_DOWN" in state set "ramp" */
static long E_ramp_RAMP_DOWN(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 147 "../demo.st"
	if ((pVar->voltage) < (pVar->loLimit))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 154 "../demo.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 2;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "RAMP_DOWN" in state set "ramp" */
static void A_ramp_RAMP_DOWN(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 146 "../demo.st"
			seq_seqLog(ssId, "ramp_down -> ramp_up\n");
		}
		return;
	case 1:
		{
# line 150 "../demo.st"
			(pVar->voltage) -= 0.1;
# line 152 "../demo.st"
			if (seq_pvPut(ssId, 3 /* voltage */, 0) < 0)
				seq_seqLog(ssId, "pvPut( voltage ) failed\n");
		}
		return;
	}
}
/* Code for state "START" in state set "limit" */

/* Delay function for state "START" in state set "limit" */
static void D_limit_START(SS_ID ssId, struct UserVar *pVar)
{
# line 164 "../demo.st"
# line 169 "../demo.st"
}

/* Event function for state "START" in state set "limit" */
static long E_limit_START(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 164 "../demo.st"
	if (seq_efTestAndClear(ssId, loFlag) && (pVar->loLimit) > (pVar->hiLimit))
	{
		*pNextState = 0;
		*pTransNum = 0;
		return TRUE;
	}
# line 169 "../demo.st"
	if (seq_efTestAndClear(ssId, hiFlag) && (pVar->hiLimit) < (pVar->loLimit))
	{
		*pNextState = 0;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "START" in state set "limit" */
static void A_limit_START(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 162 "../demo.st"
			(pVar->hiLimit) = (pVar->loLimit);
			seq_pvPut(ssId, 5 /* hiLimit */, 0);
		}
		return;
	case 1:
		{
# line 167 "../demo.st"
			(pVar->loLimit) = (pVar->hiLimit);
			seq_pvPut(ssId, 4 /* loLimit */, 0);
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
  {"demo:light", (void *)OFFSET(struct UserVar, light), "light", 
    "long", 1, 3, 0, 0, 0, 0, 0},

  {"demo:lightOn", (void *)OFFSET(struct UserVar, lightOn), "lightOn", 
    "double", 1, 4, 0, 1, 0, 0, 0},

  {"demo:lightOff", (void *)OFFSET(struct UserVar, lightOff), "lightOff", 
    "double", 1, 5, 0, 1, 0, 0, 0},

  {"demo:voltage", (void *)OFFSET(struct UserVar, voltage), "voltage", 
    "double", 1, 6, 0, 1, 0, 0, 0},

  {"demo:loLimit", (void *)OFFSET(struct UserVar, loLimit), "loLimit", 
    "double", 1, 7, 1, 1, 0, 0, 0},

  {"demo:hiLimit", (void *)OFFSET(struct UserVar, hiLimit), "hiLimit", 
    "double", 1, 8, 2, 1, 0, 0, 0},

};

/* Event masks for state set light */
	/* Event mask for state START: */
static bitMask	EM_light_START[] = {
	0x00000000,
};
	/* Event mask for state LIGHT_OFF: */
static bitMask	EM_light_LIGHT_OFF[] = {
	0x00000050,
};
	/* Event mask for state LIGHT_ON: */
static bitMask	EM_light_LIGHT_ON[] = {
	0x00000060,
};

/* State Blocks */

static struct seqState state_light[] = {
	/* State "START" */ {
	/* state name */       "START",
	/* action function */ (ACTION_FUNC) A_light_START,
	/* event function */  (EVENT_FUNC) E_light_START,
	/* delay function */   (DELAY_FUNC) D_light_START,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_light_START,
	/* state options */   (0)},

	/* State "LIGHT_OFF" */ {
	/* state name */       "LIGHT_OFF",
	/* action function */ (ACTION_FUNC) A_light_LIGHT_OFF,
	/* event function */  (EVENT_FUNC) E_light_LIGHT_OFF,
	/* delay function */   (DELAY_FUNC) D_light_LIGHT_OFF,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_light_LIGHT_OFF,
	/* state options */   (0)},

	/* State "LIGHT_ON" */ {
	/* state name */       "LIGHT_ON",
	/* action function */ (ACTION_FUNC) A_light_LIGHT_ON,
	/* event function */  (EVENT_FUNC) E_light_LIGHT_ON,
	/* delay function */   (DELAY_FUNC) D_light_LIGHT_ON,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_light_LIGHT_ON,
	/* state options */   (0)},


};

/* Event masks for state set ramp */
	/* Event mask for state START: */
static bitMask	EM_ramp_START[] = {
	0x00000000,
};
	/* Event mask for state RAMP_UP: */
static bitMask	EM_ramp_RAMP_UP[] = {
	0x00000140,
};
	/* Event mask for state RAMP_DOWN: */
static bitMask	EM_ramp_RAMP_DOWN[] = {
	0x000000c0,
};

/* State Blocks */

static struct seqState state_ramp[] = {
	/* State "START" */ {
	/* state name */       "START",
	/* action function */ (ACTION_FUNC) A_ramp_START,
	/* event function */  (EVENT_FUNC) E_ramp_START,
	/* delay function */   (DELAY_FUNC) D_ramp_START,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ramp_START,
	/* state options */   (0)},

	/* State "RAMP_UP" */ {
	/* state name */       "RAMP_UP",
	/* action function */ (ACTION_FUNC) A_ramp_RAMP_UP,
	/* event function */  (EVENT_FUNC) E_ramp_RAMP_UP,
	/* delay function */   (DELAY_FUNC) D_ramp_RAMP_UP,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ramp_RAMP_UP,
	/* state options */   (0)},

	/* State "RAMP_DOWN" */ {
	/* state name */       "RAMP_DOWN",
	/* action function */ (ACTION_FUNC) A_ramp_RAMP_DOWN,
	/* event function */  (EVENT_FUNC) E_ramp_RAMP_DOWN,
	/* delay function */   (DELAY_FUNC) D_ramp_RAMP_DOWN,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ramp_RAMP_DOWN,
	/* state options */   (0)},


};

/* Event masks for state set limit */
	/* Event mask for state START: */
static bitMask	EM_limit_START[] = {
	0x00000186,
};

/* State Blocks */

static struct seqState state_limit[] = {
	/* State "START" */ {
	/* state name */       "START",
	/* action function */ (ACTION_FUNC) A_limit_START,
	/* event function */  (EVENT_FUNC) E_limit_START,
	/* delay function */   (DELAY_FUNC) D_limit_START,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_limit_START,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "light" */ {
	/* ss name */            "light",
	/* ptr to state block */ state_light,
	/* number of states */   3,
	/* error state */        -1},


	/* State set "ramp" */ {
	/* ss name */            "ramp",
	/* ptr to state block */ state_ramp,
	/* number of states */   3,
	/* error state */        -1},


	/* State set "limit" */ {
	/* ss name */            "limit",
	/* ptr to state block */ state_limit,
	/* number of states */   1,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "pvsys=ca,debug=1";

/* State Program table (global) */
struct seqProgram demo = {
	/* magic number */       20000315,
	/* *name */              "demo",
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

void demoRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&demo);
}
epicsExportRegistrar(demoRegistrar);

