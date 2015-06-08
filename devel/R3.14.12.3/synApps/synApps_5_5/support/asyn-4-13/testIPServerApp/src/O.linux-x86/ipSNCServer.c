/* SNC Version 2.0.12: Wed Oct 22 21:38:08 2014: ipSNCServer.i */

/* Event flags */

/* Program "ipSNCServer" */
#include "seqCom.h"

#define NUM_SS 1
#define NUM_CHANNELS 3
#define NUM_EVENTS 0
#define NUM_QUEUES 0

#define MAX_STRING_SIZE 40

#define ASYNC_OPT FALSE
#define CONN_OPT TRUE
#define DEBUG_OPT FALSE
#define MAIN_OPT FALSE
#define NEWEF_OPT TRUE
#define REENT_OPT TRUE

extern struct seqProgram ipSNCServer;

/* Variable declarations */
struct UserVar {
	char	input[MAX_STRING_SIZE];
	char	output[MAX_STRING_SIZE];
	int	connected;
	char	*listenerPortName;
	char	*IOPortName;
	int	readStatus;
	int	writeStatus;
	char	*pasynUser;
	char	*registrarPvt;
	char	*eventId;
};

/* C code definitions */
# line 6 "../ipSNCServer.st"
#include <string.h>
# line 7 "../ipSNCServer.st"
#include <epicsString.h>
# line 8 "../ipSNCServer.st"
#include <epicsEvent.h>
# line 9 "../ipSNCServer.st"
#include <asynDriver.h>
# line 10 "../ipSNCServer.st"
#include <asynOctet.h>
# line 11 "../ipSNCServer.st"
#include <asynOctetSyncIO.h>
# line 28 "../ipSNCServer.st"
 static void initialize(SS_ID ssId, struct UserVar *pVar);
# line 29 "../ipSNCServer.st"
 static int readSocket(SS_ID ssId, struct UserVar *pVar);
# line 30 "../ipSNCServer.st"
 static int writeSocket(SS_ID ssId, struct UserVar *pVar);

/* Entry handler */
static void entry_handler(SS_ID ssId, struct UserVar *pVar)
{
}

/* Code for state "init" in state set "ipSNCServer" */

/* Delay function for state "init" in state set "ipSNCServer" */
static void D_ipSNCServer_init(SS_ID ssId, struct UserVar *pVar)
{
# line 38 "../ipSNCServer.st"
}

/* Event function for state "init" in state set "ipSNCServer" */
static long E_ipSNCServer_init(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 38 "../ipSNCServer.st"
	if (TRUE)
	{
		*pNextState = 1;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "init" in state set "ipSNCServer" */
static void A_ipSNCServer_init(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 35 "../ipSNCServer.st"
			(pVar->listenerPortName) = seq_macValueGet(ssId, "PORT");
# line 37 "../ipSNCServer.st"
initialize(ssId, pVar);
# line 37 "../ipSNCServer.st"
			(pVar->connected) = 0;
		}
		return;
	}
}
/* Code for state "waitConnect" in state set "ipSNCServer" */

/* Delay function for state "waitConnect" in state set "ipSNCServer" */
static void D_ipSNCServer_waitConnect(SS_ID ssId, struct UserVar *pVar)
{
# line 46 "../ipSNCServer.st"
}

/* Event function for state "waitConnect" in state set "ipSNCServer" */
static long E_ipSNCServer_waitConnect(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 46 "../ipSNCServer.st"
	if (TRUE)
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "waitConnect" in state set "ipSNCServer" */
static void A_ipSNCServer_waitConnect(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 43 "../ipSNCServer.st"
			seq_pvPut(ssId, 2 /* connected */, 0);
# line 45 "../ipSNCServer.st"
epicsEventWait( (epicsEventId)pVar->eventId);
# line 45 "../ipSNCServer.st"
			seq_pvPut(ssId, 2 /* connected */, 0);
		}
		return;
	}
}
/* Code for state "processCommands" in state set "ipSNCServer" */

/* Delay function for state "processCommands" in state set "ipSNCServer" */
static void D_ipSNCServer_processCommands(SS_ID ssId, struct UserVar *pVar)
{
# line 60 "../ipSNCServer.st"
# line 63 "../ipSNCServer.st"
}

/* Event function for state "processCommands" in state set "ipSNCServer" */
static long E_ipSNCServer_processCommands(SS_ID ssId, struct UserVar *pVar, short *pTransNum, short *pNextState)
{
# line 60 "../ipSNCServer.st"
	if ((pVar->connected))
	{
		*pNextState = 2;
		*pTransNum = 0;
		return TRUE;
	}
# line 63 "../ipSNCServer.st"
	if (!(pVar->connected))
	{
		*pNextState = 1;
		*pTransNum = 1;
		return TRUE;
	}
	return FALSE;
}

/* Action function for state "processCommands" in state set "ipSNCServer" */
static void A_ipSNCServer_processCommands(SS_ID ssId, struct UserVar *pVar, short transNum)
{
	switch(transNum)
	{
	case 0:
		{
# line 52 "../ipSNCServer.st"
pVar->readStatus = readSocket(ssId, pVar);
# line 59 "../ipSNCServer.st"
			if ((pVar->readStatus) == 0)
			{
				seq_pvPut(ssId, 0 /* input */, 0);
				strcpy((pVar->output), "OK");
pVar->writeStatus = writeSocket(ssId, pVar);
				if ((pVar->writeStatus) == 0)
				{
					seq_pvPut(ssId, 1 /* output */, 0);
				}
			}
		}
		return;
	case 1:
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
  {"{P}stringInput", (void *)OFFSET(struct UserVar, input[0]), "input", 
    "string", 1, 1, 0, 0, 0, 0, 0},

  {"{P}stringOutput", (void *)OFFSET(struct UserVar, output[0]), "output", 
    "string", 1, 2, 0, 0, 0, 0, 0},

  {"{P}connected", (void *)OFFSET(struct UserVar, connected), "connected", 
    "int", 1, 3, 0, 0, 0, 0, 0},

};

/* Event masks for state set ipSNCServer */
	/* Event mask for state init: */
static bitMask	EM_ipSNCServer_init[] = {
	0x00000000,
};
	/* Event mask for state waitConnect: */
static bitMask	EM_ipSNCServer_waitConnect[] = {
	0x00000000,
};
	/* Event mask for state processCommands: */
static bitMask	EM_ipSNCServer_processCommands[] = {
	0x00000008,
};

/* State Blocks */

static struct seqState state_ipSNCServer[] = {
	/* State "init" */ {
	/* state name */       "init",
	/* action function */ (ACTION_FUNC) A_ipSNCServer_init,
	/* event function */  (EVENT_FUNC) E_ipSNCServer_init,
	/* delay function */   (DELAY_FUNC) D_ipSNCServer_init,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ipSNCServer_init,
	/* state options */   (0)},

	/* State "waitConnect" */ {
	/* state name */       "waitConnect",
	/* action function */ (ACTION_FUNC) A_ipSNCServer_waitConnect,
	/* event function */  (EVENT_FUNC) E_ipSNCServer_waitConnect,
	/* delay function */   (DELAY_FUNC) D_ipSNCServer_waitConnect,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ipSNCServer_waitConnect,
	/* state options */   (0)},

	/* State "processCommands" */ {
	/* state name */       "processCommands",
	/* action function */ (ACTION_FUNC) A_ipSNCServer_processCommands,
	/* event function */  (EVENT_FUNC) E_ipSNCServer_processCommands,
	/* delay function */   (DELAY_FUNC) D_ipSNCServer_processCommands,
	/* entry function */   (ENTRY_FUNC) 0,
	/* exit function */   (EXIT_FUNC) 0,
	/* event mask array */ EM_ipSNCServer_processCommands,
	/* state options */   (0)},


};

/* State Set Blocks */
static struct seqSS seqSS[NUM_SS] = {
	/* State set "ipSNCServer" */ {
	/* ss name */            "ipSNCServer",
	/* ptr to state block */ state_ipSNCServer,
	/* number of states */   3,
	/* error state */        -1},
};

/* Program parameter list */
static char prog_param[] = "P=testIPServer:=, PORT=P5000";

/* State Program table (global) */
struct seqProgram ipSNCServer = {
	/* magic number */       20000315,
	/* *name */              "ipSNCServer",
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
# line 69 "../ipSNCServer.st"

static void connectionCallback(void *drvPvt, asynUser *pasynUserIn, char *portName, size_t len, int eomReason)
{
    struct UserVar *pVar = (struct UserVar *)drvPvt;
    asynUser *pasynUser = (asynUser *)pVar->pasynUser;
    asynStatus status;

    pVar->IOPortName = epicsStrDup(portName);
    asynPrint(pasynUser, ASYN_TRACE_FLOW,
              "ipSNCServer: connectionCallback, portName=%s\n", portName);
    status = pasynOctetSyncIO->connect(portName, 0, (asynUser **)&pVar->pasynUser, NULL);
    pasynUser = (asynUser *)pVar->pasynUser;
    if (status) {
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
                  "ipSNCServer:connectionCallback: unable to connect to port %s\n",
                  portName);
        return;
    }
    status = pasynOctetSyncIO->setInputEos(pasynUser, "\r\n", 2);
    if (status) {
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
                  "ipSNCServer:connectionCallback: unable to set input EOS on %s: %s\n",
                  portName, pasynUser->errorMessage);
        return;
    }
    status = pasynOctetSyncIO->setOutputEos(pasynUser, "\r\n", 2);
    if (status) {
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
                  "ipSNCServer:connectionCallback: unable to set output EOS on %s: %s\n",
                  portName, pasynUser->errorMessage);
        return;
    }

    pVar->connected = 1;
    epicsEventSignal( (epicsEventId)pVar->eventId);
}

static void initialize(SS_ID ssId, struct UserVar *pVar)
{
    int addr=0;
    asynInterface *pasynInterface;
    asynUser *pasynUser;
    asynOctet *pasynOctet;
    int status;

    pVar->eventId = (char *)epicsEventCreate(epicsEventEmpty);
    pasynUser = pasynManager->createAsynUser(0,0);
    pVar->pasynUser = (char *)pasynUser;
    pasynUser->userPvt = pVar;
    status = pasynManager->connectDevice(pasynUser, pVar->listenerPortName, addr);
    if(status!=asynSuccess) {
        printf("can't connect to port %s: %s\n", pVar->listenerPortName, pasynUser->errorMessage);
        return;
    }
    pasynInterface = pasynManager->findInterface(pasynUser,asynOctetType,1);
    if(!pasynInterface) {
        printf("%s driver not supported\n",asynOctetType);
        return;
    }
    pasynOctet = (asynOctet *)pasynInterface->pinterface;
    status = pasynOctet->registerInterruptUser(
                 pasynInterface->drvPvt, pasynUser,
                 connectionCallback,pVar,(void **)&pVar->registrarPvt);
    if(status!=asynSuccess) {
        printf("ipSNCServer devAsynOctet registerInterruptUser %s\n",
               pasynUser->errorMessage);
    }
}

static int readSocket(SS_ID ssId, struct UserVar *pVar)
{
    char buffer[80];
    size_t nread;
    int eomReason;
    asynUser *pasynUser = (asynUser *)pVar->pasynUser;
    asynStatus status;

    status = pasynOctetSyncIO->read(pasynUser, buffer, 80,
                                    -1.0, &nread, &eomReason);
    if (status) {
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
                  "ipSNCServer:readSocket: read error on: %s: %s\n",
                  pVar->IOPortName, pasynUser->errorMessage);
        pVar->connected = 0;
        strcpy(pVar->input, "");
    }
    else {
        asynPrint(pasynUser, ASYN_TRACEIO_DEVICE,
                  "ipSNCServer:readSocket: %s read %s: %s: %s\n",
                   pVar->IOPortName, buffer);
        strcpy(pVar->input, buffer);
    }
    return(status);
}

static int writeSocket(SS_ID ssId, struct UserVar *pVar)
{
    size_t nwrite;
    asynUser *pasynUser = (asynUser *)pVar->pasynUser;
    asynStatus status;

    status = pasynOctetSyncIO->write(pasynUser, pVar->output, strlen(pVar->output),
                                     0.0, &nwrite);
    if (status) {
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
                  "ipSNCServer:writeSocket: write error on: %s: %s\n",
                  pVar->IOPortName, pasynUser->errorMessage);
        pVar->connected = 0;
    }
    else {
        asynPrint(pasynUser, ASYN_TRACEIO_DEVICE,
                   "ipSNCServer:writeSocket: %s write %s\n",
                   pVar->IOPortName, pVar->output);
    }
    return(status);
}


#include "epicsExport.h"


/* Register sequencer commands and program */

void ipSNCServerRegistrar (void) {
    seqRegisterSequencerCommands();
    seqRegisterSequencerProgram (&ipSNCServer);
}
epicsExportRegistrar(ipSNCServerRegistrar);

