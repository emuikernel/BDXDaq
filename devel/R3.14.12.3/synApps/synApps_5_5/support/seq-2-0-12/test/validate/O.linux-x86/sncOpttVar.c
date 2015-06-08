/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: sncOpttVar.i */

/* Event flags */

/* Program "sncopttvar" */
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
#define REENT_OPT TRUE

extern struct seqProgram sncopttvar;

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
    threadId = seq((void *)&sncopttvar, macro_def, 0);
    if(callIocsh) {
        seqRegisterSequencerCommands();
        iocsh(0);
    } else {
        epicsThreadExitMain();
    }
    return(0);
}

/* Variable declarations */
struct UserVar {
	float	d;
	float	v;
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
# line 24 "../sncOpttVar.st"
	(pVar->d) = 4.0;
# line 25 "../sncOpttVar.st"
	printf("Entered low, initialize delay to %g sec.\n", (pVar->d));
}

/* Exit function for state "low" in state set "ss1" */
static void O_ss1_low(SS_ID ssId, struct UserVar *pVar)
{
/* Exit 1: */
# line 39 "../sncOpttVar.st"
	printf("low, print this on exit of low\n");
}

/* Delay function for state "low" in state set "ss1" */
static void D_ss1_low(SS_ID ssId, struct UserVar *pVar)
{
# line 30 "../sncOpttVar.st"
# line 36 "../sncOpttVar.st"
	seq_delayInit(ssId, 0, ((pVar->d)));
}

/* Event function for state "low" in state set "ss1" */
static long E_ss1_low(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 30 "../sncOpttVar.st"
	if ((pVar->d) > 10.0)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 36 "../sncOpttVar.st"
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
# line 29 "../sncOpttVar.st"
			printf("low, delay = %g, now changing to high\n", (pVar->d));
		}
		return;
	case 1:
		{
# line 33 "../sncOpttVar.st"
			printf("low, delay timeout, reenter low\n");
			(pVar->d) += 2.0;
			printf("low, waiting %g secs to iterate\n", (pVar->d));
		}
		return;
	}
}
/* Code for state "high" in state set "ss1" */

/* Entry function for state "high" in state set "ss1" */
static void I_ss1_high(SS_ID ssId, struct UserVar *pVar)
{
/* Entry 1: */
# line 47 "../sncOpttVar.st"
	printf("\nEntered high\n");
# line 48 "../sncOpttVar.st"
	(pVar->v) = 3.0;
}

/* Delay function for state "high" in state set "ss1" */
static void D_ss1_high(SS_ID ssId, struct UserVar *pVar)
{
# line 53 "../sncOpttVar.st"
# line 59 "../sncOpttVar.st"
	seq_delayInit(ssId, 0, (3.0));
}

/* Event function for state "high" in state set "ss1" */
static long E_ss1_high(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 53 "../sncOpttVar.st"
	if ((pVar->v) == 0)
	{
		*pNextState = 0;
		*pTransNum = 0;
		return TRUE;
	}
# line 59 "../sncOpttVar.st"
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
# line 52 "../sncOpttVar.st"
			printf("changing to low\n");
		}
		return;
	case 1:
		{
# line 56 "../sncOpttVar.st"
			printf("high, delay 3.0 timeout, decr v and re-enter high\n");
			(pVar->v) = (pVar->v) - 1.0;
			printf("v = %g\n", (pVar->v));
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
	/* entry function */   (ENTRY_FUNC) I_ss1_low,
	/* exit function */   (EXIT_FUNC) O_ss1_low,
	/* event mask array */ EM_ss1_low,
	/* state options */   (0 | OPT_NORESETTIMERS)},

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
struct seqProgram sncopttvar = {
	/* magic number */       20000315,
	/* *name */              "sncopttvar",
	/* *pChannels */         seqChan,
	/* numChans */           NUM_CHANNELS,
	/* *pSS */               seqSS,
	/* numSS */              NUM_SS,
	/* user variable size */ sizeof(struct UserVar),
	/* *pParams */           prog_param,
	/* numEvents */          NUM_EVENTS,
	/* encoded options */    (0 | OPT_CONN | OPT_NEWEF | OPT_REENT | OPT_MAIN),
	/* entry handler */      (ENTRY_FUNC) entry_handler,
	/* exit handler */       (EXIT_FUNC) exit_handler,
	/* numQueues */          NUM_QUEUES,
};
#include "epicsExport.h"


/* Register sequencer commands and program */

void sncopttvarRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&sncopttvar);
}
epicsExportRegistrar(sncopttvarRegistrar);

