/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: sncExitOptx.i */

/* Event flags */

/* Program "sncexitoptx" */
#include "seqCom.h"

#define NUM_SS 1
#define NUM_CHANNELS 0
#define NUM_EVENTS 0
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT FALSE
#define MAIN_OPT TRUE
#define NEWEF_OPT TRUE
#define REENT_OPT FALSE

extern struct seqProgram sncexitoptx;

/* Main program */
#include <string.h>
#include "epicsThread.h"
#include "iocsh.h"

int main(int argc,char *argv[]) {
    char * macro_def;
    epicsThreadId threadId;
    int callIocsh = 0;
    if(argc>1 && strcmp(argv[1],"-s")==0) {
        callIocsh=1;
        --argc; ++argv;
    }
    macro_def = (argc>1)?argv[1]:NULL;
    threadId = seq((void *)&sncexitoptx, macro_def, 0);
    if(callIocsh) {
        seqRegisterSequencerCommands();
        iocsh(0);
    } else {
        epicsThreadExitMain();
    }
    return(0);
}

/* Variable declarations */
static float	v;

/* Not used (avoids compilation warnings) */
struct UserVar {
	int	dummy;
};

/* C code definitions */
# line 7 "../sncExitOptx.st"
#include "epicsThread.h"

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "low" in state set "ss1" */

/* Exit function for state "low" in state set "ss1" */
static void O_ss1_low(SS_ID ssId, struct UserVar *pVar)
{
/* Exit 1: */
# line 27 "../sncExitOptx.st"
	printf("Pause on each exit of low, including 'iterations'\n");
# line 28 "../sncExitOptx.st"
	epicsThreadSleep(1.0);
}

/* Delay function for state "low" in state set "ss1" */
static void D_ss1_low(SS_ID ssId, struct UserVar *pVar)
{
# line 18 "../sncExitOptx.st"
# line 24 "../sncExitOptx.st"
	seq_delayInit(ssId, 0, (2.0));
}

/* Event function for state "low" in state set "ss1" */
static long E_ss1_low(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 18 "../sncExitOptx.st"
	if (v > 5.0)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 24 "../sncExitOptx.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 0;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "low" in state set "ss1" */
static void A_ss1_low(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 17 "../sncExitOptx.st"
			printf("low, v = %g, now changing to high\n", v);
		}
		return;
	case 1:
		{
# line 21 "../sncExitOptx.st"
			printf("low, delay timeout, incr v and now reenter low\n");
			v = v + 1.0;
			printf("v = %g\n", v);
		}
		return;
	}
}
/* Code for state "high" in state set "ss1" */

/* Entry function for state "high" in state set "ss1" */
static void I_ss1_high(SS_ID ssId, struct UserVar *pVar)
{
/* Entry 1: */
# line 35 "../sncExitOptx.st"
	printf("\nEntered high\n");
}

/* Exit function for state "high" in state set "ss1" */
static void O_ss1_high(SS_ID ssId, struct UserVar *pVar)
{
/* Exit 1: */
# line 47 "../sncExitOptx.st"
	printf("Exit high only on last iteration\n");
}

/* Delay function for state "high" in state set "ss1" */
static void D_ss1_high(SS_ID ssId, struct UserVar *pVar)
{
# line 39 "../sncExitOptx.st"
# line 44 "../sncExitOptx.st"
	seq_delayInit(ssId, 0, (3.0));
}

/* Event function for state "high" in state set "ss1" */
static long E_ss1_high(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 39 "../sncExitOptx.st"
	if (v == 0.0)
	{
		*pNextState = 0;
		*pTransNum = 0;
		return TRUE;
	}
# line 44 "../sncExitOptx.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "high" in state set "ss1" */
static void A_ss1_high(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 38 "../sncExitOptx.st"
			printf("changing to low\n");
		}
		return;
	case 1:
		{
# line 42 "../sncExitOptx.st"
			printf("high, delay 3.0 timeout, resetting v\n");
			v = 0;
		}
		return;
	}
}

/* Exit handler */
static void exit_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/************************ Tables ***********************/

/* No Database Blocks, create 1 for ptr init. */
static struct seqChan seqChan[1];

/* Event masks for state set ss1 */
	/* Event mask for state low: */
static bitMask	EM_ss1_low[] = {
	0x00000000,
};
	/* Event mask for state high: */
static bitMask	EM_ss1_high[] = {
	0x00000000,
};

/* State Blocks */

static struct seqState state_ss1[] = {
	/* State "low" */ {
	/* state name */       "low",
	/* action function */ (ACTION_FUNC) A_ss1_low,
	/* event function */  (EVENT_FUNC) E_ss1_low,
	/* delay function */   (DELAY_FUNC) D_ss1_low,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) O_ss1_low,
	/* event mask array */ EM_ss1_low,
	/* state options */   (0 | OPT_DOEXITTOSELF)},

	/* State "high" */ {
	/* state name */       "high",
	/* action function */ (ACTION_FUNC) A_ss1_high,
	/* event function */  (EVENT_FUNC) E_ss1_high,
	/* delay function */   (DELAY_FUNC) D_ss1_high,
	/* entry function */   (ENTRY_FUNC) I_ss1_high,
	/* exit function */   (EXIT_FUNC) O_ss1_high,
	/* event mask array */ EM_ss1_high,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "ss1" */ {
	/* ss name */            "ss1",
	/* ptr to state block */ state_ss1,
	/* number of states */   2,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "";

/* State Program table (global) */
struct seqProgram sncexitoptx = {
	/* magic number */       20000315,
	/* *name */              "sncexitoptx",
	/* *pChannels */         seqChan,
	/* numChans */           NUM_CHANNELS,
	/* *pSS */               seqSS,
	/* numSS */              NUM_SS,
	/* user variable size */ 0,
	/* *pParams */           prog_param,
	/* numEvents */          NUM_EVENTS,
	/* encoded options */    (0 | OPT_CONN | OPT_NEWEF | OPT_MAIN),
	/* entry handler */      (ENTRY_FUNC) entry_handler,
	/* exit handler */       (EXIT_FUNC) exit_handler,
	/* numQueues */          NUM_QUEUES,
};
#include "epicsExport.h"


/* Register sequencer commands and program */

void sncexitoptxRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&sncexitoptx);
}
epicsExportRegistrar(sncexitoptxRegistrar);
