/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: Federal.i */

/* Event flags */

/* Program "Federal" */
#include "seqCom.h"

#define NUM_SS 1
#define NUM_CHANNELS 17
#define NUM_EVENTS 0
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT FALSE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT FALSE

extern struct seqProgram Federal;

/* Variable declarations */
static int	i;
static int	FederalDebug;
static int	run;
static int	num_probes;
static int	probe_num;
static float	position;
static int	proc = 1;
static char	readback[MAX_STRING_SIZE];
static double	probe_[10];

/* Not used (avoids compilation warnings) */
struct UserVar {
	int	dummy;
};

/* C code definitions */
# line 15 "../Federal.st"
#include <stdlib.h>
# line 16 "../Federal.st"
#include <stdio.h>
# line 17 "../Federal.st"
#include <string.h>
# line 18 "../Federal.st"
#include <math.h>
# line 19 "../Federal.st"
#include <epicsThread.h>

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "get_readings" in state set "Federal_scan" */

/* Delay function for state "get_readings" in state set "Federal_scan" */
static void D_Federal_scan_get_readings(SS_ID ssId, struct UserVar *pVar)
{
# line 80 "../Federal.st"
}

/* Event function for state "get_readings" in state set "Federal_scan" */
static long E_Federal_scan_get_readings(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 80 "../Federal.st"
	if (run)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "get_readings" in state set "Federal_scan" */
static void A_Federal_scan_get_readings(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 79 "../Federal.st"
			for (i = 0; i < num_probes; i++)
			{
				if (!run)
					break;
				seq_pvPut(ssId, 5 /* proc */, 0);
				epicsThreadSleep(.33);
				seq_pvGet(ssId, 6 /* readback */, 0);
				seq_pvGet(ssId, 4 /* position */, 0);
				seq_pvGet(ssId, 3 /* probe_num */, 0);
				if (FederalDebug)
				{
					printf("\nFederal.st:  ascii input: %s\n", readback);
					printf("Federal.st:  probe number: %d\n", probe_num);
					printf("Federal.st:  position: %3.4f\n", position);
				}
				if (probe_num > num_probes)
					break;
				probe_num--;
				probe_[probe_num] = position;
				seq_pvPut(ssId, 7 /* probe_ */ + (probe_num), 0);
			}
		}
		return;
	}
}
/* Code for state "wait" in state set "Federal_scan" */

/* Delay function for state "wait" in state set "Federal_scan" */
static void D_Federal_scan_wait(SS_ID ssId, struct UserVar *pVar)
{
# line 84 "../Federal.st"
	seq_delayInit(ssId, 0, (0.5));
}

/* Event function for state "wait" in state set "Federal_scan" */
static long E_Federal_scan_wait(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 84 "../Federal.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 0;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "wait" in state set "Federal_scan" */
static void A_Federal_scan_wait(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
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
  {"{P}Fed:{S}:debug", (void *)&FederalDebug, "FederalDebug", 
    "int", 1, 1, 0, 1, 0, 0, 0},

  {"{P}Fed:{S}:run", (void *)&run, "run", 
    "int", 1, 2, 0, 1, 0, 0, 0},

  {"{P}Fed:{S}:num_probes", (void *)&num_probes, "num_probes", 
    "int", 1, 3, 0, 1, 0, 0, 0},

  {"{P}Fed:{S}:probe", (void *)&probe_num, "probe_num", 
    "int", 1, 4, 0, 0, 0, 0, 0},

  {"{P}Fed:{S}:position", (void *)&position, "position", 
    "float", 1, 5, 0, 0, 0, 0, 0},

  {"{P}Fed:{S}:serial.PROC", (void *)&proc, "proc", 
    "int", 1, 6, 0, 0, 0, 0, 0},

  {"{P}Fed:{S}:serial.AINP", (void *)&readback[0], "readback", 
    "string", 1, 7, 0, 0, 0, 0, 0},

  {"{P}Fed:{S}:probe_1", (void *)&probe_[0], "probe_[0]", 
    "double", 1, 8, 0, 0, 0, 0, 0},

  {"{P}Fed:{S}:probe_2", (void *)&probe_[1], "probe_[1]", 
    "double", 1, 9, 0, 0, 0, 0, 0},

  {"{P}Fed:{S}:probe_3", (void *)&probe_[2], "probe_[2]", 
    "double", 1, 10, 0, 0, 0, 0, 0},

  {"{P}Fed:{S}:probe_4", (void *)&probe_[3], "probe_[3]", 
    "double", 1, 11, 0, 0, 0, 0, 0},

  {"{P}Fed:{S}:probe_5", (void *)&probe_[4], "probe_[4]", 
    "double", 1, 12, 0, 0, 0, 0, 0},

  {"{P}Fed:{S}:probe_6", (void *)&probe_[5], "probe_[5]", 
    "double", 1, 13, 0, 0, 0, 0, 0},

  {"{P}Fed:{S}:probe_7", (void *)&probe_[6], "probe_[6]", 
    "double", 1, 14, 0, 0, 0, 0, 0},

  {"{P}Fed:{S}:probe_8", (void *)&probe_[7], "probe_[7]", 
    "double", 1, 15, 0, 0, 0, 0, 0},

  {"{P}Fed:{S}:probe_9", (void *)&probe_[8], "probe_[8]", 
    "double", 1, 16, 0, 0, 0, 0, 0},

  {"{P}Fed:{S}:probe_10", (void *)&probe_[9], "probe_[9]", 
    "double", 1, 17, 0, 0, 0, 0, 0},

};

/* Event masks for state set Federal_scan */
	/* Event mask for state get_readings: */
static bitMask	EM_Federal_scan_get_readings[] = {
	0x00000004,
};
	/* Event mask for state wait: */
static bitMask	EM_Federal_scan_wait[] = {
	0x00000000,
};

/* State Blocks */

static struct seqState state_Federal_scan[] = {
	/* State "get_readings" */ {
	/* state name */       "get_readings",
	/* action function */ (ACTION_FUNC) A_Federal_scan_get_readings,
	/* event function */  (EVENT_FUNC) E_Federal_scan_get_readings,
	/* delay function */   (DELAY_FUNC) D_Federal_scan_get_readings,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_Federal_scan_get_readings,
	/* state options */   (0)},

	/* State "wait" */ {
	/* state name */       "wait",
	/* action function */ (ACTION_FUNC) A_Federal_scan_wait,
	/* event function */  (EVENT_FUNC) E_Federal_scan_wait,
	/* delay function */   (DELAY_FUNC) D_Federal_scan_wait,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_Federal_scan_wait,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "Federal_scan" */ {
	/* ss name */            "Federal_scan",
	/* ptr to state block */ state_Federal_scan,
	/* number of states */   2,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "";

/* State Program table (global) */
struct seqProgram Federal = {
	/* magic number */       20000315,
	/* *name */              "Federal",
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
#include "epicsExport.h"


/* Register sequencer commands and program */

void FederalRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&Federal);
}
epicsExportRegistrar(FederalRegistrar);

