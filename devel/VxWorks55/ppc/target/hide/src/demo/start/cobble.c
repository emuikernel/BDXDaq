/* cobble.c - sample program for tutorial in Tornado Getting Started Guide */

/* This code is intentionally left buggy! */

/*
modification history
--------------------
01b,01apr01,ems  better badness & cripser cleanups (with gracious help of wsl)
01a,01apr99,ems  written (with gracious help of dlk)
*/

/*
DESCRIPTION:
This module is used as a simple example multitask application
in the Tornado Getting Started Guide. It is intended to help
the new Tornado user quickly become familiar with various
Tornado tools that are available for debugging such
applications.

The program simulates a data collection system in which
data is retrieved from an external source (e.g. a device
which generates interrupts as data come in). A task simulating
the ISR gives a semaphore each time a new datum arrives. (This
task could easily be replaced by a VxWorks watchdog timer.)

The data are processed in two stages. First, a sample consisting
of NUM_SAMPLE data is collected. Second, various arcane
arithmetical operations are performed on the sample to obtain
a result value. Each stage is represented by a task; the
first task gives a binary semaphore when the second task should
begin the second stage work.

The value of the result of the processing is monitored by yet
another task, which prints a warning message whenever the result
is out of the safety range.

INCLUDE FILES: None.
*/

/* includes */

#include "vxWorks.h"
#include "stdio.h"
#include "stdlib.h"
#include "semLib.h"
#include "taskLib.h"


/* defines */

#define NUM_SAMPLE 	10
#define LUCKY 		7
#define HOT 		20
#define DELAY_TICKS     4

#define STACK_SIZE	20000


/* typedefs */

typedef struct byLightning 
    {
    int 		  data;
    int 		  nodeNum;
    struct byLightning * pPrevNode;
    } LIST_NODE;


/* globals */

int tidCosmos;			/* Task IDs */
int tidSchlep;
int tidCrunch;
int tidMonitor;

int cosmicData = 0;		/* Holds a datum available to read */
int result = 0;                 /* Holds result of calculation */

	
LIST_NODE * pCurrNode = NULL;	/* head of list of data */

SEM_ID dataSemId;		/* Given when a datum is available          */
SEM_ID syncSemId;		/* Given when a data sample can be crunched */
SEM_ID nodeListGuardSemId;      /* Given when pointer to current node can 
				   be accessed                              */


/* forward declarations */

void cosmos (void);
void nodeAdd (int data, int nodeNum);
void schlep (void);
void nodeScrap (void);
void crunch (void);
void monitor (void);
void progStop (void);

/*************************************************************************
*
* progStart - start the sample program.
*
* Create various semaphores and spawn various tasks, while doing
* incredibly little error checking.
*
* RETURNS: OK
*/

STATUS progStart (void)
    {
    syncSemId = semBCreate (SEM_Q_FIFO, SEM_EMPTY);
    dataSemId = semBCreate (SEM_Q_FIFO, SEM_EMPTY);

    nodeListGuardSemId = semMCreate (  SEM_Q_PRIORITY
				| SEM_INVERSION_SAFE
				| SEM_DELETE_SAFE);
    
    pCurrNode = NULL;	/* just in case */

    /* get started */

    tidCosmos = taskSpawn ("tCosmos", 200, 0, STACK_SIZE,
        (FUNCPTR) cosmos,0,0,0,0,0,0,0,0,0,0);

    tidSchlep = taskSpawn ("tSchlep", 220, 0, STACK_SIZE,
        (FUNCPTR) schlep,0,0,0,0,0,0,0,0,0,0);

    /*
     * priority mis-assignment provides desired educational
     * malfunction of not allowing tCrunch to run because 
     * tMonitor is higher priority
     */

    tidCrunch = taskSpawn ("tCrunch", 240, 0, STACK_SIZE,
        (FUNCPTR) crunch,0,0,0,0,0,0,0,0,0,0);

    tidMonitor = taskSpawn ("tMonitor", 230, 0, STACK_SIZE,
        (FUNCPTR) monitor,0,0,0,0,0,0,0,0,0,0);

    return (OK);
    }

/*************************************************************************
*
* cosmos - simulate data arrival interrupt
*
* This routine is executed by a task which serves as a replacement for
* an ISR which signals the availability of data from some device.
* It periodically gives the semaphore dataSemId to indicate that a
* datum is available and may be read from the cosmicData variable.
*/

void cosmos (void)
    {
    int nadaNichtsIdx = 0;

    FOREVER
	{
        if (nadaNichtsIdx != LUCKY)
            cosmicData = rand ();
        else
	    {
            cosmicData = 0; /* because you can't wait forever for nothing */
            nadaNichtsIdx = 0; 
	    }

        ++nadaNichtsIdx;
       
        /* semaphore and delay ensure only new data will be read */
        semGive (dataSemId);
        taskDelay (DELAY_TICKS);
	}
    }

/*************************************************************************
*
* nodeAdd - link a new node to front of data chain.
*
* Allocates and initializes a node, puts it onto the data chain. 
* 
* RETURNS: N/A
*/

void nodeAdd
    (
    int data,
    int nodeNum
    )
    {
    LIST_NODE * node;

    if ( (node = (LIST_NODE *) malloc (sizeof (LIST_NODE))) != NULL)
	{
	node->data = data;
	node->nodeNum = nodeNum;

	semTake (nodeListGuardSemId, WAIT_FOREVER); 
	node->pPrevNode = pCurrNode;
	pCurrNode = node;
	semGive (nodeListGuardSemId);
	}
    else
	{
	printf ("cobble: Out of Memory.\n");
	taskSuspend (0);
	}
    return;
    }

/*************************************************************************
*
* schlep - collect data into a sample to be processed
*
* Repeatedly, wait for and place NUM_SAMPLE data onto the list,
* then awaken the cruncher.
*/

void schlep (void)
    {
    int nodeIdx;
  
    FOREVER
	{
	for (nodeIdx = 0; nodeIdx < NUM_SAMPLE; nodeIdx++)
            {
	    semTake (dataSemId, WAIT_FOREVER);	/* Wait for datum */
	    nodeAdd (cosmicData, nodeIdx);
            }

	semGive (syncSemId); /* Give entire sample to cruncher! */
	}     
    }

/*************************************************************************
*
* nodeScrap - relegate a node to the dust bin of history
*
*/

void nodeScrap (void)
    {
    LIST_NODE * pTmpNode;

    pTmpNode = pCurrNode;
    pCurrNode = pCurrNode->pPrevNode; 
    free (pTmpNode);         
    return;
    }

/*************************************************************************
*
* crunch - process data samples
*
* Applies dubious transformations to data.
* But first it waits for the schlepper to send it a sample.
*/

void crunch (void)
    {
    int	sampleSum = 0;
    int div;
    int * pDiv = &div; 

    FOREVER
	{
	semTake (syncSemId, WAIT_FOREVER);	/* Wait for dinner */
	semTake (nodeListGuardSemId, WAIT_FOREVER); /* reserve access to 
	                                          pCurrNode */
	while (pCurrNode != NULL)
	    {
	    sampleSum += pCurrNode->data;
	    div = pCurrNode->data;
	    nodeScrap ();
	    }
	
	semGive (nodeListGuardSemId);   /* release access to pCurrNode */


        /* Oh! Woe! Errant exclamation point!  */

        if (!(pDiv = NULL) && (*pDiv != 0))  
/*      if ((pDiv != NULL) && (*pDiv != 0))  */ 
            result = sampleSum / (*pDiv);

        sampleSum = 0;		   /* Clean up for the next round. */
	}
    }


/*************************************************************************
*
* monitor - monitors results of calculation
*
* Checks results of calculation and prints warning if too hot.
*/

void monitor (void)
    {
    int isHot = FALSE;

    FOREVER
	{
        if (!isHot && result >= HOT)
            {
            isHot = TRUE;
  	    printf ("WARNING: HOT!\n");
	    }
	else if (isHot && result < HOT)
	    {                 
	    isHot = FALSE;
	    printf ("cool\n");
	    }
	}
    }

/*************************************************************************
*
* progStop - stops the program 
*
* Call this routine to end it all. 
*/

void progStop (void)
    {
    taskDelete (tidCosmos);
    taskDelete (tidSchlep);
    taskDelete (tidCrunch);
    taskDelete (tidMonitor);

    semDelete (dataSemId);
    semDelete (syncSemId);
    semDelete (nodeListGuardSemId);
    
    while (pCurrNode != NULL)
	nodeScrap();

    /* My, aren't we tidy! */

    printf ("BYE!TSCHUESS!ADIEU!\n");
    return;
    }

