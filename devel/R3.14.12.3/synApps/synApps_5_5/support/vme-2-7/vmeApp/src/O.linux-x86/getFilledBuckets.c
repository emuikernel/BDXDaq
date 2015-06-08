/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: getFilledBuckets.i */

/* Event flags */

/* Program "getFillPat" */
#include "seqCom.h"

#define NUM_SS 2
#define NUM_CHANNELS 14
#define NUM_EVENTS 0
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT FALSE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT TRUE

extern struct seqProgram getFillPat;

/* Variable declarations */
struct UserVar {
	char	message[MAX_STRING_SIZE];
	short	clearMessage;
	short	loadPat;
	short	FilledBuckets[1296];
	short	BunchPattern[1296];
	short	LocalCopy[1296];
	short	BCGPattern;
	short	BCGBucketList;
	short	NumBuckets;
	short	setBucket;
	short	clearAll;
	short	autoLoad;
	short	srBeamStatus;
	short	srInjectStatus;
	int	i;
	int	running;
};

/* C code definitions */
# line 68 "../getFilledBuckets.st"
#include <string.h>
# line 71 "../getFilledBuckets.st"
 int getFilledBucketsDebug = 0;
# line 72 "../getFilledBuckets.st"
 int getFilledBucketsLDebug = 0;

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "init" in state set "getFillPattern" */

/* Delay function for state "init" in state set "getFillPattern" */
static void D_getFillPattern_init(SS_ID ssId, struct UserVar *pVar)
{
# line 83 "../getFilledBuckets.st"
# line 89 "../getFilledBuckets.st"
	seq_delayInit(ssId, 0, (2.0));
}

/* Event function for state "init" in state set "getFillPattern" */
static long E_getFillPattern_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 83 "../getFilledBuckets.st"
	if (seq_pvConnectCount(ssId) == seq_pvChannelCount(ssId))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
# line 89 "../getFilledBuckets.st"
	if (seq_delay(ssId, 0))
	{
		*pNextState = 0;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "getFillPattern" */
static void A_getFillPattern_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 80 "../getFilledBuckets.st"
			strcpy((pVar->message), "Waiting");
			seq_pvPut(ssId, 0 /* message */, 0);
			(pVar->running) = 1;
		}
		return;
	case 1:
		{
# line 86 "../getFilledBuckets.st"
			(pVar->running) = 0;
			strcpy((pVar->message), "CA Error");
			seq_pvPut(ssId, 0 /* message */, 0);
		}
		return;
	}
}
/* Code for state "waitCommand" in state set "getFillPattern" */

/* Delay function for state "waitCommand" in state set "getFillPattern" */
static void D_getFillPattern_waitCommand(SS_ID ssId, struct UserVar *pVar)
{
# line 95 "../getFilledBuckets.st"
# line 101 "../getFilledBuckets.st"
# line 129 "../getFilledBuckets.st"
}

/* Event function for state "waitCommand" in state set "getFillPattern" */
static long E_getFillPattern_waitCommand(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 95 "../getFilledBuckets.st"
	if (seq_pvConnectCount(ssId) != seq_pvChannelCount(ssId))
	{
		*pNextState = 0;
		*pTransNum = 0;
		return TRUE;
	}
# line 101 "../getFilledBuckets.st"
	if ((pVar->clearMessage) == 1)
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
# line 129 "../getFilledBuckets.st"
	if ((pVar->loadPat) == 1)
	{
		*pNextState = 1;
		*pTransNum = 2;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "waitCommand" in state set "getFillPattern" */
static void A_getFillPattern_waitCommand(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 94 "../getFilledBuckets.st"
			(pVar->running) = 0;
		}
		return;
	case 1:
		{
# line 97 "../getFilledBuckets.st"
			(pVar->clearMessage) = 0;
			seq_pvPut(ssId, 1 /* clearMessage */, 0);
			strcpy((pVar->message), "Message Cleared");
			seq_pvPut(ssId, 0 /* message */, 0);
		}
		return;
	case 2:
		{
# line 103 "../getFilledBuckets.st"
			(pVar->loadPat) = 0;
			seq_pvPut(ssId, 2 /* loadPat */, 0);
			strcpy((pVar->message), "Loading Pattern");
			seq_pvPut(ssId, 0 /* message */, 0);
			(pVar->clearAll) = 1;
			seq_pvPut(ssId, 10 /* clearAll */, 0);
			seq_pvGet(ssId, 8 /* NumBuckets */, 0);
# line 111 "../getFilledBuckets.st"
 if(getFilledBucketsDebug) {
# line 111 "../getFilledBuckets.st"
			printf("NumBuckets = %d\n", (pVar->NumBuckets));
# line 113 "../getFilledBuckets.st"
 }
			seq_pvGet(ssId, 5 /* LocalCopy */, 0);
# line 121 "../getFilledBuckets.st"
			for ((pVar->i) = 0; (pVar->i) < (pVar->NumBuckets); (pVar->i)++)
			{
				(pVar->setBucket) = (pVar->LocalCopy)[(pVar->i)];
				seq_pvPut(ssId, 9 /* setBucket */, 0);
 if(getFilledBucketsDebug) {
				printf("%d\t%d\n", (pVar->i), (pVar->LocalCopy)[(pVar->i)]);
 }
			}
# line 123 "../getFilledBuckets.st"
			(pVar->BCGPattern) = 1;
			seq_pvPut(ssId, 6 /* BCGPattern */, 0);
			(pVar->BCGBucketList) = 1;
			seq_pvPut(ssId, 7 /* BCGBucketList */, 0);
			strcpy((pVar->message), "Pattern Loaded");
			seq_pvPut(ssId, 0 /* message */, 0);
		}
		return;
	}
}
/* Code for state "idle" in state set "autoLoad" */

/* Delay function for state "idle" in state set "autoLoad" */
static void D_autoLoad_idle(SS_ID ssId, struct UserVar *pVar)
{
# line 141 "../getFilledBuckets.st"
}

/* Event function for state "idle" in state set "autoLoad" */
static long E_autoLoad_idle(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 141 "../getFilledBuckets.st"
	if (((pVar->running) == 1) && ((pVar->autoLoad) == 1))
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "idle" in state set "autoLoad" */
static void A_autoLoad_idle(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 139 "../getFilledBuckets.st"
 if(getFilledBucketsLDebug) {
 printf("autoLoad: State NoBeam\n");
 }
		}
		return;
	}
}
/* Code for state "NoBeam" in state set "autoLoad" */

/* Delay function for state "NoBeam" in state set "autoLoad" */
static void D_autoLoad_NoBeam(SS_ID ssId, struct UserVar *pVar)
{
# line 148 "../getFilledBuckets.st"
# line 157 "../getFilledBuckets.st"
	seq_delayInit(ssId, 0, (1.0));
}

/* Event function for state "NoBeam" in state set "autoLoad" */
static long E_autoLoad_NoBeam(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 148 "../getFilledBuckets.st"
	if (((pVar->running) != 1) || ((pVar->autoLoad) != 1))
	{
		*pNextState = 0;
		*pTransNum = 0;
		return TRUE;
	}
# line 157 "../getFilledBuckets.st"
	if (((pVar->srInjectStatus) == 0) && ((pVar->srBeamStatus)) == 1)
	{
		*pNextState = 2;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "NoBeam" in state set "autoLoad" */
static void A_autoLoad_NoBeam(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 146 "../getFilledBuckets.st"
 if(getFilledBucketsLDebug) {
 printf("autoLoad: State idle\n");
 }
		}
		return;
	case 1:
		{
# line 151 "../getFilledBuckets.st"
			seq_delay(ssId, 0);
			(pVar->loadPat) = 1;
			seq_pvPut(ssId, 2 /* loadPat */, 0);
# line 155 "../getFilledBuckets.st"
 if(getFilledBucketsLDebug) {
 printf("autoLoad: State Pattern Loaded\n");
 }
		}
		return;
	}
}
/* Code for state "PatternLoaded" in state set "autoLoad" */

/* Delay function for state "PatternLoaded" in state set "autoLoad" */
static void D_autoLoad_PatternLoaded(SS_ID ssId, struct UserVar *pVar)
{
# line 164 "../getFilledBuckets.st"
# line 169 "../getFilledBuckets.st"
# line 174 "../getFilledBuckets.st"
}

/* Event function for state "PatternLoaded" in state set "autoLoad" */
static long E_autoLoad_PatternLoaded(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 164 "../getFilledBuckets.st"
	if (((pVar->running) != 1) || ((pVar->autoLoad) != 1))
	{
		*pNextState = 0;
		*pTransNum = 0;
		return TRUE;
	}
# line 169 "../getFilledBuckets.st"
	if ((pVar->srInjectStatus) == 1)
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
# line 174 "../getFilledBuckets.st"
	if ((pVar->srBeamStatus) != 1)
	{
		*pNextState = 1;
		*pTransNum = 2;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "PatternLoaded" in state set "autoLoad" */
static void A_autoLoad_PatternLoaded(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 162 "../getFilledBuckets.st"
 if(getFilledBucketsLDebug) {
 printf("autoLoad: State idle\n");
 }
		}
		return;
	case 1:
		{
# line 167 "../getFilledBuckets.st"
 if(getFilledBucketsLDebug) {
 printf("autoLoad: State NoBeam\n");
 }
		}
		return;
	case 2:
		{
# line 172 "../getFilledBuckets.st"
 if(getFilledBucketsLDebug) {
 printf("autoLoad: State NoBeam\n");
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
  {"{unit}BnchClkGenMessageSI", (void *)OFFSET(struct UserVar, message[0]), "message", 
    "string", 1, 1, 0, 0, 0, 0, 0},

  {"{unit}BnchClkGenClearMessBO", (void *)OFFSET(struct UserVar, clearMessage), "clearMessage", 
    "short", 1, 2, 0, 1, 0, 0, 0},

  {"{unit}BnchClkGenLoadPatBO", (void *)OFFSET(struct UserVar, loadPat), "loadPat", 
    "short", 1, 3, 0, 1, 0, 0, 0},

  {"Mt:S:FilledBucketsWF.VAL", (void *)OFFSET(struct UserVar, FilledBuckets[0]), "FilledBuckets[0]", 
    "short", 1296, 4, 0, 0, 0, 0, 0},

  {"Mt:S:FillPatternWF.VAL", (void *)OFFSET(struct UserVar, BunchPattern[0]), "BunchPattern[0]", 
    "short", 1296, 5, 0, 0, 0, 0, 0},

  {"{unit}BnchClkGenListWF", (void *)OFFSET(struct UserVar, LocalCopy[0]), "LocalCopy[0]", 
    "short", 1296, 6, 0, 0, 0, 0, 0},

  {"{unit}BnchClkGenPatternWF.PROC", (void *)OFFSET(struct UserVar, BCGPattern), "BCGPattern", 
    "short", 1, 7, 0, 0, 0, 0, 0},

  {"{unit}BnchClkGenBucketsWF.PROC", (void *)OFFSET(struct UserVar, BCGBucketList), "BCGBucketList", 
    "short", 1, 8, 0, 0, 0, 0, 0},

  {"Mt:S:NumBucketsFilledAI", (void *)OFFSET(struct UserVar, NumBuckets), "NumBuckets", 
    "short", 1, 9, 0, 0, 0, 0, 0},

  {"{unit}BnchClkGenSetBucketAO.VAL", (void *)OFFSET(struct UserVar, setBucket), "setBucket", 
    "short", 1, 10, 0, 0, 0, 0, 0},

  {"{unit}BnchClkGenClearPatBO.VAL", (void *)OFFSET(struct UserVar, clearAll), "clearAll", 
    "short", 1, 11, 0, 0, 0, 0, 0},

  {"{unit}BnchClkGenAutoLoadBO", (void *)OFFSET(struct UserVar, autoLoad), "autoLoad", 
    "short", 1, 12, 0, 1, 0, 0, 0},

  {"S:BeamStatusMBBI.VAL", (void *)OFFSET(struct UserVar, srBeamStatus), "srBeamStatus", 
    "short", 1, 13, 0, 1, 0, 0, 0},

  {"S:InjectingStatusMBBI", (void *)OFFSET(struct UserVar, srInjectStatus), "srInjectStatus", 
    "short", 1, 14, 0, 1, 0, 0, 0},

};

/* Event masks for state set getFillPattern */
	/* Event mask for state init: */
static bitMask	EM_getFillPattern_init[] = {
	0x00000000,
};
	/* Event mask for state waitCommand: */
static bitMask	EM_getFillPattern_waitCommand[] = {
	0x0000000c,
};

/* State Blocks */

static struct seqState state_getFillPattern[] = {
	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_getFillPattern_init,
	/* event function */  (EVENT_FUNC) E_getFillPattern_init,
	/* delay function */   (DELAY_FUNC) D_getFillPattern_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_getFillPattern_init,
	/* state options */   (0)},

	/* State "waitCommand" */ {
	/* state name */       "waitCommand",
	/* action function */ (ACTION_FUNC) A_getFillPattern_waitCommand,
	/* event function */  (EVENT_FUNC) E_getFillPattern_waitCommand,
	/* delay function */   (DELAY_FUNC) D_getFillPattern_waitCommand,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_getFillPattern_waitCommand,
	/* state options */   (0)},


};

/* Event masks for state set autoLoad */
	/* Event mask for state idle: */
static bitMask	EM_autoLoad_idle[] = {
	0x00001000,
};
	/* Event mask for state NoBeam: */
static bitMask	EM_autoLoad_NoBeam[] = {
	0x00007000,
};
	/* Event mask for state PatternLoaded: */
static bitMask	EM_autoLoad_PatternLoaded[] = {
	0x00007000,
};

/* State Blocks */

static struct seqState state_autoLoad[] = {
	/* State "idle" */ {
	/* state name */       "idle",
	/* action function */ (ACTION_FUNC) A_autoLoad_idle,
	/* event function */  (EVENT_FUNC) E_autoLoad_idle,
	/* delay function */   (DELAY_FUNC) D_autoLoad_idle,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_autoLoad_idle,
	/* state options */   (0)},

	/* State "NoBeam" */ {
	/* state name */       "NoBeam",
	/* action function */ (ACTION_FUNC) A_autoLoad_NoBeam,
	/* event function */  (EVENT_FUNC) E_autoLoad_NoBeam,
	/* delay function */   (DELAY_FUNC) D_autoLoad_NoBeam,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_autoLoad_NoBeam,
	/* state options */   (0)},

	/* State "PatternLoaded" */ {
	/* state name */       "PatternLoaded",
	/* action function */ (ACTION_FUNC) A_autoLoad_PatternLoaded,
	/* event function */  (EVENT_FUNC) E_autoLoad_PatternLoaded,
	/* delay function */   (DELAY_FUNC) D_autoLoad_PatternLoaded,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_autoLoad_PatternLoaded,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "getFillPattern" */ {
	/* ss name */            "getFillPattern",
	/* ptr to state block */ state_getFillPattern,
	/* number of states */   2,
	/* error state */        -1},


	/* State set "autoLoad" */ {
	/* ss name */            "autoLoad",
	/* ptr to state block */ state_autoLoad,
	/* number of states */   3,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "unit=dummy";

/* State Program table (global) */
struct seqProgram getFillPat = {
	/* magic number */       20000315,
	/* *name */              "getFillPat",
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
# line 182 "../getFilledBuckets.st"



#include "epicsExport.h"


/* Register sequencer commands and program */

void getFillPatRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&getFillPat);
}
epicsExportRegistrar(getFillPatRegistrar);

