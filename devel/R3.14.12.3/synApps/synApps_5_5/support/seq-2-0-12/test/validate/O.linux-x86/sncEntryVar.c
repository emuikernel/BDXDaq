/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: sncEntryVar.i */

/* Event flags */

/* Program "sncentryvar" */
#include "seqCom.h"

#define NUM_SS 1
#define NUM_CHANNELS 1
#define NUM_EVENTS 0
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT FALSE
#define MAIN_OPT TRUE
#define NEWEF_OPT TRUE
#define REENT_OPT FALSE

extern struct seqProgram sncentryvar;

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
    threadId = seq((void *)&sncentryvar, macro_def, 0);
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

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "low" in state set "ss1" */

/* Entry function for state "low" in state set "ss1" */
static void I_ss1_low(SS_ID ssId, struct UserVar *pVar)
{
/* Entry 1: */
# line 17 "../sncEntryVar.st"
	v = 20.0;
# line 18 "../sncEntryVar.st"
	seq_pvPut(ssId, 0 /* v */, 0);
# line 19 "../sncEntryVar.st"
	printf("Init v = %g on first entry to low\n", v);
}

/* Exit function for state "low" in state set "ss1" */
static void O_ss1_low(SS_ID ssId, struct UserVar *pVar)
{
/* Exit 1: */
# line 40 "../sncEntryVar.st"
	printf("low, print this on exit of low\n");
}

/* Delay function for state "low" in state set "ss1" */
static void D_ss1_low(SS_ID ssId, struct UserVar *pVar)
{
# line 25 "../sncEntryVar.st"
# line 30 "../sncEntryVar.st"
# line 37 "../sncEntryVar.st"
	seq_delayInit(ssId, 0, (2.0));
}

/* Event function for state "low" in state set "ss1" */
static long E_ss1_low(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 25 "../sncEntryVar.st"
	if (v < 11.0)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 30 "../sncEntryVar.st"
	if (v > 80)
	{
		*pNextState = 0;
		*pTransNum = 1;
		return TRUE;
	}
# line 37 "../sncEntryVar.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 0;
		*pTransNum = 2;
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
# line 24 "../sncEntryVar.st"
			printf("low, v = %g, now changing to high\n", v);
		}
		return;
	case 1:
		{
# line 28 "../sncEntryVar.st"
			v = 20;
			seq_pvPut(ssId, 0 /* v */, 0);
		}
		return;
	case 2:
		{
# line 33 "../sncEntryVar.st"
			printf("low, delay timeout, incr v and now reenter low\n");
			v = v + 1.0;
			seq_pvPut(ssId, 0 /* v */, 0);
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
# line 47 "../sncEntryVar.st"
	printf("\nEntered high\n");
}

/* Delay function for state "high" in state set "ss1" */
static void D_ss1_high(SS_ID ssId, struct UserVar *pVar)
{
# line 51 "../sncEntryVar.st"
# line 57 "../sncEntryVar.st"
	seq_delayInit(ssId, 0, (3.0));
}

/* Event function for state "high" in state set "ss1" */
static long E_ss1_high(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 51 "../sncEntryVar.st"
	if (v >= 20.0)
	{
		*pNextState = 0;
		*pTransNum = 0;
		return TRUE;
	}
# line 57 "../sncEntryVar.st"
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
# line 50 "../sncEntryVar.st"
			printf("high, v = %g changing to low\n", v);
		}
		return;
	case 1:
		{
# line 54 "../sncEntryVar.st"
			v = 20;
			seq_pvPut(ssId, 0 /* v */, 0);
			printf("high, delay timeout, resetting v = %g\n", v);
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
  {"CD01:FD:PERIOD", (void *)&v, "v", 
    "float", 1, 1, 0, 1, 0, 0, 0},

};

/* Event masks for state set ss1 */
	/* Event mask for state low: */
static bitMask	EM_ss1_low[] = {
	0x00000002,
};
	/* Event mask for state high: */
static bitMask	EM_ss1_high[] = {
	0x00000002,
};

/* State Blocks */

static struct seqState state_ss1[] = {
	/* State "low" */ {
	/* state name */       "low",
	/* action function */ (ACTION_FUNC) A_ss1_low,
	/* event function */  (EVENT_FUNC) E_ss1_low,
	/* delay function */   (DELAY_FUNC) D_ss1_low,
	/* entry function */   (ENTRY_FUNC) I_ss1_low,
	/* exit function */   (EXIT_FUNC) O_ss1_low,
	/* event mask array */ EM_ss1_low,
	/* state options */   (0)},

	/* State "high" */ {
	/* state name */       "high",
	/* action function */ (ACTION_FUNC) A_ss1_high,
	/* event function */  (EVENT_FUNC) E_ss1_high,
	/* delay function */   (DELAY_FUNC) D_ss1_high,
	/* entry function */   (ENTRY_FUNC) I_ss1_high,
	/* exit function */   (EXIT_FUNC) 0,
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
struct seqProgram sncentryvar = {
	/* magic number */       20000315,
	/* *name */              "sncentryvar",
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

void sncentryvarRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&sncentryvar);
}
epicsExportRegistrar(sncentryvarRegistrar);

