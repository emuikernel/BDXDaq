/* vxColor.c -  Graph coloring program to demo some vxWorks capabilities */

/* Copyright 1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,14jun95,rhp fixed some typos in names and comments
01c,xxapr95,jco took care of colin's notes:

		1- use of a binary semaphore to suspend processNodes instead of
		   using taskDelay.
		2- use of a mutex semaphore to suspend the activities while 
		   adding new connection(s) or when asking for the net state.
		3- rewrite according to WRS Coding Conventions
		4- mods of the nodeJob interface to hide the database 
		   implementation (as an array) behind a pointer.

01b,xxapr95,jco ported to vxWorks. 
01a,xxapr95,jco written.
*/

/*
DESCRIPTION
Perform the following steps to run the graph-coloring demo:

(1) Call graphInit() to start up the package.

(2) Create and stabilize the graphs for a preconfigured scenario, as
in one of the following examples:

For a simple unconnected collection of map nodes:
.CS
colTest(1);
colTest(2);
.CE   

For a collection of nodes forming a wheel:
.CS
colTest(3);
colTest(4);
.CE

For a collection of nodes with the connectivity of a map of France:
.CS
colTest(5);
colTest(6);
.CE

(3) To display the current map (the argument 2 means to produce a full
display):
.CS
graphDisp(2);
.CE

(4) To change the color of any node (where the <mode> argument
specifies whether to use the <color> argument or generate a random
color):
.CS
colorChange(<nodeId>, <mode>, <color>);
.CE

If <mode> is 1, the new color is random; if <mode> is 2, the color is changed
to the value specified with the <color> argument.

(5) To extend the map, you can both create new nodes with
nodeCreate(), and make new connections between existing nodes with
connectionCreate():

.CS
nodeCreate(<nodeId>);
connectionCreate(<nodeId1>, <nodeId2>);
.CE

(6) To kill everything and end the demo:
.CS
graphStop();
.CE

INTERNAL
The following is the thread of execution of this test program.

The ":" trace stands for a given net (a set of node creation and connection 
creation). The "|" trace stands for modification of that net. New nodes
can be appended to the net, but only new connections or color changes
trigger the task resumes. The "#" tag stands for end of task.

       Stability   Node       Connection   Graph     Color    Graph       Graph
Node   Detection   Creation   Creation    Display   Change   Init        Stop
Task   Task	   Task	      Task	  Task	    Task     Task        Task
====   =========   ========   =========   =======   ======   =====       =====
                      :          :   |                |         |          |
                      :          :   |                |         |          |
                      :          :   |                |   spawn stability  |
                      :          :   |                |       /            |
                      :          :   |                |      /             |
           <------------------------------------------------/              |
           |          :          :   |                |         |          |
           |      spawn node     :   |                |         #          |
           |          /          :   |                |                    |
           |         /           :   |                |                    |
  <--------|--------/            :   |                |                    |
           |         #           :   |                |                    |
           |                     #   |                |                    |
      [stability]                    |                |                    |
           \                         |                |                    |
            \                        |                |                    |
             \ ----------------------------->         |                    |
   taskSuspend Node Tasks            |      |         |                    |
          /                          |      #         |                    |
         /                           |                |                    |
  <-----/                            |                |                    |
    taskSuspend itself               |                |                    |
                                     |                |                    |
                                     |                |                    |
                            taskResume Node Tasks     |                    |
                                     /                |                    |
                                    /                 |                    |
  <--------------------------------/          taskResume Node Tasks        |
  |                         taskResume Stability      #                    |
  |                                  /                                     |
  |                                 /                                      |
  |        <-----------------------/                                       |
  |        |                        |                                      |
  |        |                        #                                      |
  |        |                                                               |
  |        |                                       taskdelete Node + stability
  |        |                                                               /
  |        |                                                              /
  <--------<-------------------------------------------------------------/
  #        #                                                               |
                                                                           #


INCLUDE FILES:
*/

/* includes */

#include "vxWorks.h"
#include "taskLib.h"
#include "msgQLib.h"
#include "stdarg.h"
#include "stdio.h"
#include "tickLib.h"
#include "kernelLib.h"

/* defines */

#define		GRAPHMAXNODE	100	/* graph max node number */
#define		NODEMAXCONNEX	10	/* node max connection number */
#define		MAX_MSG		50	/* max messg number in queue */
#define		MSG_SIZE	sizeof (NODE_ATTRIB) /* size of message */
#define		COLORMAX	6	/* maximum number of color */
#define		OUTDEGREEMAX	5	/* node max out degree */
#define		OUTDEGREEINIT	(OUTDEGREEMAX + 1)
#define		STEP_TICKS	6	/* inter node processing  ticks count */

/* typedefs */

typedef struct /* IDS */
    {
    int		tid;			/* task id */
    char	nid;			/* node id */
    } IDS;				/* task/node id	*/

typedef struct /* NODE_ATTRIB */
    {
    char	color;			/* node color */
    int		Xvalue;			/* node Xvalue */
    } NODE_ATTRIB;			/* node attributes */

typedef struct /* CONNECT_INFO */
    {
    char	dir;	/* direction: +=to connected -=from connected */
    IDS		tnid;			/* connected node tid & nid */
    NODE_ATTRIB	att;			/* connected node attributes */
    MSG_Q_ID	wMQId;		/* write (toNeighbor) mesg queue id */
    MSG_Q_ID	rMQId;		/* read  (toNeighbor) mesg queue id */
    } CONNECT_INFO;			/* connected node info */

typedef struct /* GNODE */
    {
    char	stable;			/* color stability flag	*/
    char	rev;			/* DAG reversing flag */
    IDS		tnid;			/* node tid & nid */
    NODE_ATTRIB	att;			/* node col & Xval */
    int		pc;			/* node Prog Counter */
    int		oD;			/* node outDegree */
    int		cNum;			/* node connections # */
    CONNECT_INFO cArray[NODEMAXCONNEX];	/* node connections array */
    } GNODE;				/* global node info */

typedef struct /* DATA_BASE */
    {
    char	nNum;			/* graph node #	*/
    GNODE	nArray[GRAPHMAXNODE];	/* graph node array */
    } DATA_BASE;			/* global graph info */	

/* globals */

DATA_BASE	dB;			/* global data base of nodes */
int		graphControlId;		/* graphControl task Id */
SEM_ID		nodeSyncSem;		/* node sync semaphore */
SEM_ID		graphSem;		/* graph mutex semaphore */

/* locals */


/* forward declarations */

int 		nodeChecks (int nodeId);
int 		nodeCreate (int nodeId);
void 		nodeInit (int nodeId, GNODE * pNode);
LOCAL void	nodeJob(int nodeId, GNODE * pNode);
int 		graphColoring (GNODE * pNode);
void 		graphControl (void);
void 		graphDisp (int mode);
void		graphInit (void);
void 		graphStop (void);
int 		colorChange (int nodeId, int mode, int newColor);
void 		colTest	(int opCode);
int 		connectionCreate (int nid1, int nid2);
BOOL 		consistencyTest	(void);

/*******************************************************************************
*
* neighborTalk - talking with the pc ranked neihgbor
*
* Note about the inter nodes communication phase:
* Trial: read a neighbor at a time for the color only
* actually, all processNodes write their color to all their neighbors
* and each process node read all its links to empty them
* (avoiding pipes to get full), but only one value is used.
*
*/

void neighborTalk 
    (
    GNODE * pNode	/* current node */
    )
    {
    NODE_ATTRIB	att;		/* attributes */
    int		ix;		/* connected node index */
    int		retVal;		/* msgQ operation result */

    att = pNode->att;

    /* writing connected nodes */

    for (ix=0; ix< pNode->cNum; ix++)
	retVal = msgQSend (pNode->cArray[ix].wMQId, (char *) &att, MSG_SIZE,
			   WAIT_FOREVER, MSG_PRI_NORMAL);

    /* reading connected nodes, especially rank=pc one */

    for (ix=0; ix< pNode->cNum; ix++)
	{
	retVal = msgQReceive (pNode->cArray[ix].rMQId, (char *) &att,
			      MSG_SIZE, NO_WAIT);
	if (ix == pNode->pc)
	    pNode->cArray[ix].att =  att;
	}
    }

/*******************************************************************************
*
* graphColoring - DAG & Coloring processing
*
*
*/

int graphColoring 
    (
    GNODE * pNode	/* current node */
    )
    {
    int		ix;		/* index */
    int		outDegree;	/* outdegree of this node */
    int		colConf;	/* if color conflict */
    int		col[COLORMAX];	/* color check array */
    long	XvalueMax;	/* max Xvalue of node */
#if 0
    /* not used */
    NODE_ATTRIB	att;		/* attributes */
#endif

    if (pNode->pc < pNode->cNum) /* connected neighbors scaning uncompleted */
        {

	/* talking with pc ranked neighbor */

	neighborTalk (pNode);


	/* program counter incrementation */

	pNode->pc++;
        }
    else /* connected neighbors scaning completed */
        {

	/* DAG conversion: computes links directions and outDegree */

	XvalueMax = 0;
        outDegree = 0;
    	for (ix=0; ix < pNode->cNum; ix++)
	    {
	    if ( (pNode->att.Xvalue < pNode->cArray[ix].att.Xvalue ) || 
		 ( (pNode->att.Xvalue == pNode->cArray[ix].att.Xvalue) &&
		   (pNode->tnid.nid < pNode->cArray[ix].tnid.nid) ) )
		{

		pNode->cArray[ix].dir = '+';
		outDegree++;
		if (pNode->cArray[ix].att.Xvalue > XvalueMax)
		    XvalueMax = pNode->cArray[ix].att.Xvalue;
	        }
	    else
		{
		pNode->cArray[ix].dir = '-';
	        }
	    }
	pNode->oD = outDegree;

        if (outDegree > OUTDEGREEMAX)
	    {

	    /* DAG conversion: reversing current node */

	    printf("reversing %d: Xvalue from %d to Xvalue = %ld.\n",
		   pNode->tnid.nid, pNode->att.Xvalue, XvalueMax + 1);
            pNode->att.Xvalue = XvalueMax + 1;
	    pNode->rev++;
            }
	else /* outDegree <= OUTDEGREEMAX : this enables coloring algo */
	    {	

	    /* COL : detecting color conflict and 'n' tagging used color */

            colConf = 0;
    	    for (ix=0; ix<6; ix++)
		col[ix] = 'y';

    	    for (ix=0; ix < pNode->cNum; ix++)
		{
	        if (pNode->cArray[ix].dir == '+')
		    {
	            if (pNode->att.color == pNode->cArray[ix].att.color)
			colConf = 1;
	            col[(unsigned int)(pNode->cArray[ix].att.color)] = 'n';
	            }
	        }

	    /* COL: update color in case of coloring conflict */

	    if ( colConf == 1 )
		{
		ix=0; /* looks for the first free color	*/

		while( ix<6 && col[ix]=='n')
		    ix++;
		if (ix==6)
		    {
		    printf("End of node %d: col error.\n", pNode->tnid.nid);
		    exit(2);
		    }
		else
		    {
		    printf("Node %d: color update %d --> %d\n",
			pNode->tnid.nid, pNode->att.color, ix);
		    pNode->att.color = ix;
		    }
		pNode->stable = 0;
	        }
	    else
		{
		pNode->stable = 1;
	        }
	    }

   	/* reset program counter		*/

	pNode->pc 	= 0;
        }
    return 0;
    }

/*******************************************************************************
*
* consistencyTest - tests whether arcs are well oriented throughout the graph
*
* RETURNS: TRUE if graph is consistent, FALSE otherwise
*/

BOOL consistencyTest (void)
    {
    int		i;	/* node index */
    int		j;	/* connected node index */
    int		k;	/* node index */
    int		l;	/* connected node index */
    char	dir1;	/* link direction */
    char	dir2;	/* link direction */

    for(i=0; i<dB.nNum; i++)
	{
	for(j=0; j<dB.nArray[i].cNum; j++)
	    {
	    if (dB.nArray[i].att.color == dB.nArray[i].cArray[j].att.color)
		{
		printf("(i,j)=(%d,%d) direct color conflict.\n", i, j);
		return FALSE;
    	        }
    	    if ( (k = nodeChecks (dB.nArray[i].cArray[j].tnid.nid)) == -1 )
		{
		printf("Bad link definition.\n");
		return FALSE;
		}
	    /* is that node at rank k has correct connected info */
    	    l = 0;
    	    while ( l<dB.nArray[k].cNum && 
		    dB.nArray[k].cArray[l].tnid.nid != dB.nArray[i].tnid.nid )
		l++;
    	    if (l == dB.nNum)
		{
		printf("Symetric link violation.\n");
		return FALSE;
    	        }
	    /* is that node at rank k has correct color */
	    if ( dB.nArray[i].cArray[j].att.color != dB.nArray[k].att.color )
		{
		printf("(i,j,k)=(%d,%d,%d) connection color inconsistency.\n", i, j, k);
		return FALSE;
    	        }

	    dir1 = dB.nArray[i].cArray[j].dir;
	    dir2 = dB.nArray[k].cArray[l].dir;

	    if ( (dir1 != '+' && dir1 != '-') || (dir2 != '+' && dir2 != '-')
		 || (dir1==dir2) )
		{
		printf("Asymetric direction inconsistency.\n");
		return FALSE;
	        }
	    }
        }
    return TRUE;
    }

/*******************************************************************************
*
* colorChange - randomly change a node color
*
* note: database is a critical resource that needs a mutex semaphore
*/

int colorChange
    (
    int nodeId,		/* node id */
    int mode,		/* colorChange mode: 1 = random, 2 = specified color */
    int newColor	/* specified color of the colorChange mode # 2 */
    )
    {
    int		rank;	/* node rank in the database */
    char	color;	/* temporary color */

    /* checks if that node id is already in use in the graph */

    if ( (rank = nodeChecks (nodeId)) == -1 )
	{
	printf("No such node id %d in current graph.\n", nodeId);
	return -1;
	}

    switch (mode)
	{
	case 1: /* randomly picks out a color among 6 values */
    	    srand (tickGet ());
    	    color = dB.nArray[rank].att.color;
    	    while ( color == dB.nArray[rank].att.color)
	        color = rand()%6;
	    break;

	case 2:
	    color = newColor;
	    break;

	default: 
	    printf("Unknown colorChange mode.\n");
	    exit(0);
	}

    printf("Node %d: color change %d --> %d.\n", nodeId,
	   dB.nArray[rank].att.color, color);

    /* access the dataBase critical resource */

    semTake (graphSem, -1);

    dB.nArray[rank].att.color = color;
    dB.nArray[rank].stable	   = 0;

    /* give back the dataBase critical resource */

    semGive (graphSem);

    /* resume the graphControl task */

    taskResume (graphControlId);
    return 0;
}

/*******************************************************************************
*
*  graphDisp - display current graph state
*
* note: database access should be frozzen during the display
*
*/

void graphDisp
    (
    int mode	/* display mode 2 = full !2 = regular */
    )
    {

    int	i, j;

    printf("******\n* dataBase graph info:\n*\n");
    printf("nid color Xvalue sb oD Rev cNum connected nodes\n");
    for(i=0; i<dB.nNum; i++)
	{
	printf("%3d %5d %6d %2d %2d %3d %4d", 
	    dB.nArray[i].tnid.nid,
	    dB.nArray[i].att.color,
	    dB.nArray[i].att.Xvalue,
	    dB.nArray[i].stable,
	    dB.nArray[i].oD,
	    dB.nArray[i].rev,
	    dB.nArray[i].cNum);
	if (mode==2) /* full display */
	    {
	    for(j=0; j<dB.nArray[i].cNum; j++)
		printf(" * %2d%c %6d %d", dB.nArray[i].cArray[j].tnid.nid,
  		  dB.nArray[i].cArray[j].dir, dB.nArray[i].cArray[j].att.Xvalue,
		  dB.nArray[i].cArray[j].att.color);
	    }
	printf("\n");
        }
    }


/*******************************************************************************
*
* graphInit - coloring graph initialization
*
*/

void graphInit (void)
    {

    /* set initial node number to 0 */

    dB.nNum = 0;

    /*
     * enabling round robin: necessary to give each node of the
     * graph (which have all the same priority) some CPU access
     */

    kernelTimeSlice(2);

    /* spawn graphControl with low priority (199) to let tasks spawned from
     * the shell (100) able to run, but with higher priority than node's tasks
     * (200) to provides fast display 
     */

    graphControlId = taskSpawn ("tContrNet", 199, VX_SUPERVISOR_MODE,
	10000, (FUNCPTR) graphControl, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }


/*******************************************************************************
*
* graphControl - graph supervisor task
*
*/

void graphControl (void)
    {
    int		ix;	/* node index */

    /* sync binary semaphore to avoid nodes keep running */

    nodeSyncSem	= semBCreate (0, SEM_EMPTY);

    /* binary semaphore to provide graph mutex protection */

    graphSem	= semBCreate (0, SEM_FULL);

    FOREVER
	{

	/* access the dataBase critical resource */

	semTake (graphSem, -1);

	/* unblocks all the nodes in an atomic way */

	semFlush (nodeSyncSem);

	/* check stability */

	if (dB.nNum > 0)
	    {
	    ix=0;
	    while (ix<dB.nNum && dB.nArray[ix].stable == 1)
		ix++;
	    if (ix == dB.nNum) /* nodes are stable */
		{
		if (consistencyTest ()) /* dataBase is stable */
		    {
		    graphDisp (0);
		    semGive (graphSem);
    	            taskSuspend (0); /* suspend itself */
		    semTake (graphSem, -1);
		    }
		}
	    }

	/* hereunder delay should be enough for one step of coloring */

	taskDelay (STEP_TICKS);

	/* give back the dataBase access */

	semGive (graphSem);
	}
    }

/*******************************************************************************
*
* nodeCreate - node creation
*
*/

int nodeCreate
    (
    int nodeId
    )
    {

    char 	name[8];
    GNODE *	pNode;

    /* checks if that node id is already in use in the graph */

    if ( nodeChecks (nodeId) != -1 )
	{
	printf("Node id %d already used in current graph.\n", nodeId);
	return -1;
	}

    /* check if the authorized maximum number of node is reached */

    if (dB.nNum == GRAPHMAXNODE-1)
	{
	printf("GRAPHMAXNODE reached!\n");
	return -1;
	}

    sprintf(name, "tNode%d", nodeId);
    pNode = &(dB.nArray[(unsigned int)dB.nNum]);
    dB.nNum++;

    /* spawn nodeJob with lower priority (200) than the default (100) */

    dB.nArray[dB.nNum-1].tnid.tid = 
	taskSpawn (name, 200, VX_SUPERVISOR_MODE, 10000, (FUNCPTR) nodeJob ,
		   nodeId, (int) pNode, 0, 0, 0, 0, 0, 0, 0, 0);
    return 0;
}



/*******************************************************************************
*
* nodeChecks - checks about a given node
*
* tests if a given node id is declared in the current database and if the
* connected node number (cNum) is under the limit
*
* RETURNS: -1 if bad node or bad cNum, rank in the database otherwise
*
*/

int nodeChecks
    (
    int 	nodeId /* node id */
    )
    {
    int		ix;	/* index */

    /* tests if the node exists */

    ix = 0;
    while (ix<dB.nNum && dB.nArray[ix].tnid.nid != nodeId)
	ix++;
    if (ix == dB.nNum)
	return -1;

    /* tests if the connected node number is under the limit */

    if (dB.nArray[ix].cNum == NODEMAXCONNEX)
	return -1;

    /* return the rank in the database */

    return ix;
    }

/*******************************************************************************
*
* connectionCreate - establish a connection between two nodes 
*
* the database is a critical resource that needs a mutex semaphore
*
*/

int connectionCreate
    (
    int nid1, 
    int nid2
    )
    {
    int		cNum1;
    int		cNum2;
    int		rank1;
    int		rank2;
    MSG_Q_ID	mqId;

    /* dataBase access request */

    semTake (graphSem, -1);

    /* check nodes id */

    if ( (rank1 = nodeChecks (nid1)) == -1 )
	{
	printf("No such node Id or NODEMAXCONNEX reached!\n");
	exit(2);
	}
    cNum1 = dB.nArray[rank1].cNum;
    if ( (rank2 = nodeChecks (nid2)) == -1 )
	{
	printf("No such node Id or NODEMAXCONNEX reached!\n");
	exit(2);
	}
    cNum2 = dB.nArray[rank2].cNum;

    dB.nArray[rank1].cArray[cNum1].tnid = dB.nArray[rank2].tnid;
    dB.nArray[rank1].cArray[cNum1].dir	= '0'; /* init with no direction */
    dB.nArray[rank1].cNum++;

    dB.nArray[rank2].cArray[cNum2].tnid = dB.nArray[rank1].tnid;
    dB.nArray[rank2].cArray[cNum2].dir	= '0'; /* init with no direction */
    dB.nArray[rank2].cNum++;

    /* create two Message Queues for node1 <-> node2 full duplex */

    if ( (mqId = msgQCreate(MAX_MSG, MSG_SIZE, MSG_Q_PRIORITY)) == NULL )
	return (ERROR);

    dB.nArray[rank1].cArray[cNum1].wMQId = mqId;
    dB.nArray[rank2].cArray[cNum2].rMQId = mqId;


    if ( (mqId = msgQCreate(MAX_MSG, MSG_SIZE, MSG_Q_PRIORITY)) == NULL )
	return (ERROR);

    dB.nArray[rank1].cArray[cNum1].rMQId = mqId;
    dB.nArray[rank2].cArray[cNum2].wMQId = mqId;

    /*printf("Link %d<->%d established.\n", nid1, nid2);*/

    /* free dataBase access */

    semGive (graphSem);

    /* resume the graphControl task (nop if graphControl already alive) */

    taskResume (graphControlId);
    return 0;
}

/*******************************************************************************
*
* nodeJob - task performed by each nodes in the graph
*
* More detailed description
*
*/

LOCAL void nodeJob 
    (
    int nodeId,		/* id of the node		*/
    GNODE * pNode	/* node processed by this task	*/
    )
    {

    /* node initialization		*/

    nodeInit(nodeId, pNode);

    /*printf("Node %d registered with random color=%d and random Xvlue = %d\n",
	    nodeId, pNode->att.color, pNode->att.Xvalue);*/

    FOREVER
	{

	/*
	 * each node block on the same semaphore (this avoid using here a
	 * taskDelay which leads, when used with many tasks, to sync problems)
	 */

	semTake (nodeSyncSem, -1);

	if (pNode->cNum > 0)
	    graphColoring (pNode);
	}
    }
    
/*******************************************************************************
*
* nodeInit - node initialization 
*
* More detailed description
*
*/

void nodeInit
    (
    int nodeId,		/* id of the node		*/
    GNODE * pNode	/* node processed by this task	*/
    )
    {

    /* random number generator init	*/
    srand (tickGet () + taskIdSelf ());

    /* node initialization		*/

    pNode->tnid.tid	= taskIdSelf ();
    pNode->tnid.nid	= nodeId;
    pNode->cNum 	= 0;
    pNode->rev 		= 0;
    pNode->pc 		= 0;
    pNode->stable 	= -1;
    pNode->oD 		= OUTDEGREEINIT;
    pNode->att.color	= rand()%6;
    pNode->att.Xvalue	= rand();

    }

/*******************************************************************************
*
* graphStop - collective Suicide
*
* kills task node's task, control task, mesgQs & semaphores
*
*/

void graphStop (void)
    {

    int	ix;	/* node index */
    int	jx;	/* connected node index */

    semDelete(graphSem);
    semDelete(nodeSyncSem);
    taskDelete(graphControlId);
    for(ix=0; ix< dB.nNum; ix++)
	taskDelete(dB.nArray[ix].tnid.tid);
    for(ix=0; ix< dB.nNum; ix++)
        {
	for(jx=0; jx< dB.nArray[ix].cNum; jx++)
	    {
	    msgQDelete(dB.nArray[ix].cArray[jx].rMQId);
	    msgQDelete(dB.nArray[ix].cArray[jx].wMQId);
	    }
        }
    printf("Graph coloring is finished.\n");
    }

/*******************************************************************************
*
* connectFullNode - performs full connections for a given node
*
*/

void connectFullNode
    (
    int nodId,		/* current node id */
    int nb_arg,		/* number of argument = connection # */
    ...			/* ellipse */
    )
    {
    int		ix;	/* index */
    va_list	p_list;	/* ellipse list */

    va_start(p_list, nb_arg);

    for (ix=0; ix<nb_arg; ix++)
	connectionCreate(nodId, va_arg(p_list, int));
    }

/*******************************************************************************
*
* entry point for colTest 
*
*/

void colTest 
    (
    int opCode
    )
    {
    int	ix;	/* index */

    if (opCode == 1)
	for(ix=1; ix<10; ix++)
	    nodeCreate(ix);

    if (opCode == 2) {
	connectFullNode(1, 7, 2, 4, 5, 6, 7, 8, 9);
	connectionCreate(2, 6);
	connectFullNode(3, 3, 4, 5, 8);
	connectFullNode(4, 2, 5, 8);
	connectionCreate(5, 8);
	connectFullNode(6, 2, 7, 8);
	connectFullNode(7, 2, 8, 9);
	connectionCreate(8, 9);
    }

    if (opCode == 3)
	for(ix=1; ix<10; ix++)
	    nodeCreate(ix);

    if (opCode == 4) {

	connectionCreate(1, 9);
	connectionCreate(9, 2);

	for(ix=2; ix<9; ix++) {
	    connectionCreate(1, ix);
	    connectionCreate(ix, ix+1);
	}
    }

    if (opCode == 5) { /* French counties */
	for(ix=1; ix<96; ix++)
	    nodeCreate(ix);
    }

    if (opCode == 6)
	{ 
        /* connections between counties of the french territory */
    
	connectFullNode(1, 6, 38, 39, 69, 71, 73, 74); /* Ain */
	connectFullNode(2, 6, 8, 51, 59, 60, 62, 77); /* Aisne */
	connectFullNode(3, 6, 18, 23, 42, 58, 63, 71); /* Allier */
	connectFullNode(4, 6, 5, 6, 13, 26, 83, 84); /* Alpes Haute Provence */
	connectFullNode(5, 4, 26, 38, 73, 84); /* Hautes Alpes */
	connectionCreate(6, 83); /* Alpes maritimes */
	connectFullNode(7, 5, 26, 30, 42, 43, 48); /* Ardeche */
	connectFullNode(8, 2, 51, 55); /* Ardennes */
	connectFullNode(9, 3, 11, 31, 66); /* Ariege */
	connectFullNode(10, 5, 21, 51, 52, 77, 89); /* Aube */
	connectFullNode(11, 4, 34, 66, 81, 82); /* Aude */
	connectFullNode(12, 7, 15, 30, 34, 46, 48, 81, 82); /* Aveyron */
	connectFullNode(13, 4, 20, 30, 83, 84); /* Bouches du Rhone */
	connectFullNode(14, 4, 27, 50, 61, 76); /* Calvados */
	connectFullNode(15, 5, 19, 43, 46, 48, 63); /* Cantal */
	connectFullNode(16, 6, 17, 24, 33, 79, 86, 87); /* Charente */
	connectFullNode(17, 3, 33, 79, 85); /* Charente maritime */
	connectFullNode(18, 5, 23, 36, 41, 45, 58); /* Cher */
	connectFullNode(19, 6, 23, 24, 46, 63, 87); /* Correze */
        /* Corse 20: no connection  */
	connectFullNode(21, 6, 39, 52, 58, 70, 71, 89); /* Cote d'or */
	connectFullNode(22, 3, 29, 35, 56); /* Cote d'armor */
	connectFullNode(23, 3, 36, 63, 87); /* Creuse */
	connectFullNode(24, 4, 33, 46, 47, 87); /* Dordogne */
	connectFullNode(25, 3, 39, 70, 90); /* Doubs */
	connectFullNode(26, 3, 30, 38, 84); /* Drome */
	connectFullNode(27, 7, 41, 45, 60, 61, 76, 78, 95); /* Eure */
	connectFullNode(28, 5, 41, 45, 61, 78, 91); /* Eure  et loir */
	connectionCreate(29, 56); /* Finistere */
	connectFullNode(30, 3, 34, 48, 84); /* Gard */
	connectFullNode(31, 4, 32, 65, 81, 82); /* Haute Garonne */
	connectFullNode(32, 5, 40, 47, 64, 65, 82); /* Gers */
	connectFullNode(33, 2, 40, 47); /* Gironde */
	connectionCreate(34, 81); /* Herault */
	connectFullNode(35, 4, 44, 50, 53, 56); /* Ile et vilaine */
	connectFullNode(36, 4, 37, 41, 86, 87); /* Indre */
	connectFullNode(37, 4, 41, 49, 72, 86); /* Indre et Loire */
	connectFullNode(38, 2, 69, 73); /* Isere */
	connectFullNode(39, 2, 70, 71); /* Jura */
	connectFullNode(40, 2, 47, 64); /* Landes */
	connectFullNode(41, 2, 45, 72); /* Loire et Cher */
	connectFullNode(42, 4, 43, 63, 69, 71); /* Loire */
	connectFullNode(43, 2, 48, 63); /* Haute Loire */
	connectFullNode(44, 3, 49, 56, 85); /* Loire Atlantique */
	connectFullNode(45, 4, 58, 77, 89, 91); /* Loiret */
	connectFullNode(46,  2, 47, 82); /* Lot */
	connectionCreate(47, 82); /* Lot et Garonne */
        /* Lozere 48 : no connection  */
	connectFullNode(49, 5, 53, 72, 79, 85, 86); /* Maine et Loire */
	connectionCreate(50, 53); /* Manche  */
	connectFullNode(51, 3, 52, 55, 77); /* Marne */
	connectFullNode(52, 3, 55, 70, 88); /* Haute Marne */
	connectFullNode(53, 2, 61, 72); /* Mayenne */
	connectFullNode(54, 4, 55, 57, 67, 88); /* Meurthe et Moselle */
	connectFullNode(55, 2, 57, 88); /* Meuse */
        /* Morbihan   56  */
	connectionCreate(57, 67); /* Moselle */
	connectionCreate(58, 71); /* Nievre */
	connectFullNode(59, 2, 62, 80); /* Nord */
	connectFullNode(60, 4, 76, 77, 80, 95); /* Oise */
	connectionCreate(61, 72); /* Orne */
	connectionCreate(62, 80); /* Pas de calais */
        /* Puy de Dome   63   */
	connectionCreate(64, 65); /* Pyrennees Atlantiques */
        /* Hautes Pyrennees  65 */
        /* Pyrennees Orientales 66 */
	connectFullNode(67, 2, 68, 88); /* Bas Rhin */
	connectFullNode(68, 2, 88, 90); /* Haut Rhin */
	connectionCreate(69, 71); /* Rhone */
	connectFullNode(70, 2, 88, 90); /* Haute Saone */
        /* Saone et Loire 71 */
        /* Sarthe 72*/
	connectionCreate(73, 74); /* Savoie */
        /* Haute Savoie 74 */
	connectFullNode(75, 3, 92, 93, 94); /* Paris */
	connectionCreate(76, 80); /* Seine Maritime */
	connectFullNode(77, 4, 89, 91, 93, 94); /* Seine et Marne */
	connectFullNode(78, 3, 91, 92, 95); /* Yvelines */
	connectFullNode(79, 2, 85, 86); /* Deux Sevres */
        /* Somme  80 */
	connectionCreate(81, 82); /* Tarn */
        /* Tarn  et garonne 82 */
        /* Var 83 */
        /* Vaucluse 84 */
        /* Vendee  85 */
	connectionCreate(86, 87); /* Vienne */
        /* Haute Vienne 87 */
        /* Vosges  88 */
        /* Yonne  89 */
        /* Territoire de Belfort  90 */
	connectFullNode(91, 2, 92, 94); /* Essonne */
	connectFullNode(92, 3, 93, 94, 95); /* Hauts de Seine */
	connectFullNode(93,  2, 94, 95); /* Seine St Denis */
        /* Val de Seine 94 */
        /* Val d'Oise 95 */
        }
    }

