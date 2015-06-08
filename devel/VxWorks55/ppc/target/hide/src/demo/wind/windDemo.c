/* windDemo - repeatedly test various kernel function */

/*
modification history
--------------------
02d,28apr99,jrp  upped stack size to make it run on sparc ss5 etc.
02c,23aug93,jcf  fixed synchronization.
02b,01aug93,dvs  fixed loop count printing.
02a,18mar93,dvs  took out timer/benchmark information.
                 ansified code.
		 general cleanup of code to use as MicroWorks demo.
01a,12nov90,shl  written.
*/

/*
DESCRIPTION
This program repeatedly exercises different kernel facilities of 
the Wind kernel.

The functions involved include the use of semaphores as sychronization
and mutual exclusion primitives, the use of taskSuspend()/taskResume() 
for task control, the use of message queues for communication and the 
use of watchdogs for task timeouts.

To exercise these kernel facilities two tasks are used, a high priority 
task and a low priority task. The high priority task executes functions
with which the resources are not available. As the high priority
task blocks, the low priority task takes over and makes available
the resources that the high priority task is waiting for. This may
sound simple at first but the underlying execution of this test program
involves context switching, rescheduling of tasks, and shuffling of
the ready queue, pend queue, and the timer queue.

These functions are chosen because they are the most commonly used
functions in sychronization, mutual exclusion, task control, inter-task
communication and timer facilities. These are the basic building blocks of
the operating system itself and also used in applications.  Repeatedly
execution of this "death loop" is a good indication of how the system will
perform in real-life as these functions are utiltized heavily in every
application.

The following is the thread of execution of this test program.


	     Higher Priority	   	Lower Priority
		  Task1			    Task2
	     ===============            ==============
		  |
		  V
	/----->	semGive()
	|	semTake()
	|	  |
	|	  V
	|	semTake()
	|	  \
	|	   \	
	|	    \------------->	   semGive()
	|				       /
	|				      /
	|	taskSuspend()  <-------------/
	|	  \
	|	   \
	|	    \------------->	   taskResume()
	|				       /
	|				      /
	|	msgQSend()     <-------------/
	|	msgQReceive()
	|	  |
	|	  V
	|	msgQReceive()
	|	  \
	|	   \	
	|	    \------------->	   msgQSend()
	|				       /
	|				      /
    	|	wdStart()      <-------------/
     	|	wdCancel()
        \---------|
		  V
		exit           
		  \
		   \	
		    \------------->	   exit
*/

#include "vxWorks.h"
#include "semLib.h"
#include "taskLib.h"
#include "msgQLib.h"
#include "wdLib.h"
#include "logLib.h"
#include "tickLib.h"
#include "sysLib.h"
#include "stdio.h"


/* defines */

#if FALSE
#define STATUS_INFO			/* define to allow printf() calls */
#endif

#define MAX_MSG		1		/* max number of messages in queue */
#define MSG_SIZE	sizeof (MY_MSG)	/* size of message */
#define DELAY		100		/* 100 ticks */
#define	HIGH_PRI	150		/* priority of high priority task */
#define LOW_PRI		200		/* priority of low priority task */

#define TASK_HIGHPRI_TEXT	"Hello from the 'high priority' task"
#define TASK_LOWPRI_TEXT	"Hello from the 'low priority' task"


/* typedefs */

typedef struct my_msg
	{
	int    childLoopCount;		/* loop count in task sending msg */
	char * buffer;			/* message text */
	} MY_MSG;


/* globals */

SEM_ID		semId;			/* semaphore ID */
MSG_Q_ID	msgQId;			/* message queue ID */
WDOG_ID		wdId;			/* watchdog ID */
int		highPriId;		/* task ID of high priority task */
int		lowPriId;		/* task ID of low priority task */
int		windDemoId;		/* task ID of windDemo task */


/* forward declarations */

LOCAL void taskHighPri (int iteration);
LOCAL void taskLowPri (int iteration);



/*******************************************************************************
*
* windDemo - parent task to spawn children 
*
* This task calls taskHighPri() and taskLowPri() to do the
* actual operations of the test and suspends itself.
* Task is resumed by the low priority task.
*
*/

void windDemo 
    (
    int iteration			/* number of iterations of child code */
    )
    {
    int loopCount = 0; 			/* number of times through windDemo */

#ifdef	STATUS_INFO
    printf ("Entering windDemo\n");
#endif /* STATUS_INFO */

    if (iteration == 0)			/* set default to 10,000 */
	iteration = 10000;

    /* create objects used by the child tasks */

    msgQId	= msgQCreate (MAX_MSG, MSG_SIZE, MSG_Q_FIFO);
    semId	= semBCreate (SEM_Q_PRIORITY, SEM_FULL);
    wdId	= wdCreate ();

    windDemoId = taskIdSelf ();

    FOREVER
	{

	/* spawn child tasks to exercise kernel routines */

    	highPriId = taskSpawn ("tHighPri", HIGH_PRI, VX_SUPERVISOR_MODE, 4000, 
			   (FUNCPTR) taskHighPri, iteration,0,0,0,0,0,0,0,0,0);

    	lowPriId = taskSpawn ("tLowPri", LOW_PRI, VX_SUPERVISOR_MODE, 4000, 
			   (FUNCPTR) taskLowPri, iteration,0,0,0,0,0,0,0,0,0);


    	taskSuspend (0);		/* to be waken up by taskLowPri */ 

#ifdef	STATUS_INFO
    	printf ("\nParent windDemo has just completed loop number %d\n",
		loopCount);
#endif /* STATUS_INFO */

    	loopCount++;
	}
    }

/*******************************************************************************
*
* taskHighPri - high priority task
*
* This tasks exercises various kernel functions. It will block if the
* resource is not available and relingish the CPU to the next ready task.
*
*/

LOCAL void taskHighPri 
    (
    int iteration			/* number of iterations through loop */
    )
    {
    int    ix;				/* loop counter */
    MY_MSG msg;				/* message to send */
    MY_MSG newMsg;			/* message to receive */

    for (ix = 0; ix < iteration; ix++)
	{

	/* take and give a semaphore - no context switch involved */

	semGive (semId);		
	semTake (semId, 100);		/* semTake with timeout */


	/* 
	 * take semaphore - context switch will occur since semaphore 
	 * is unavailable 
	 */

	semTake (semId, WAIT_FOREVER);	/* semaphore not available */

	taskSuspend (0);		/* suspend itself */


	/* build message and send it */

	msg.childLoopCount = ix;
	msg.buffer = TASK_HIGHPRI_TEXT;

	msgQSend (msgQId, (char *) &msg, MSG_SIZE, 0, MSG_PRI_NORMAL);

	/* 
	 * read message that this task just sent and print it - no context 
	 * switch will occur since there is a message already in the queue 
	 */

	msgQReceive (msgQId, (char *) &newMsg, MSG_SIZE, NO_WAIT);

#ifdef	STATUS_INFO
	printf ("%s\n Number of iterations is %d\n", 
		newMsg.buffer, newMsg.childLoopCount);
#endif /* STATUS_INFO */

	/* 
	 * block on message queue waiting for message from low priority task 
	 * context switch will occur since there is no message in the queue
	 * when message is received, print it 
	 */

	msgQReceive (msgQId, (char *) &newMsg, MSG_SIZE, WAIT_FOREVER);

#ifdef	STATUS_INFO
	printf ("%s\n Number of iterations by this task is: %d\n", 
		newMsg.buffer, newMsg.childLoopCount);
#endif /* STATUS_INFO */

	/* test watchdog timer */

	wdStart (wdId, DELAY, (FUNCPTR) tickGet, 1); 

	wdCancel (wdId);
	}
    }

/*******************************************************************************
*
* taskLowPri - low priority task
*
* This task runs at a lower priority and is designed to make available
* the resouces that the high priority task is waiting for and subsequently
* unblock the high priority task.
*
*/

LOCAL void taskLowPri 
    (
    int iteration			/* number of times through loop */
    )
    {
    int    ix;				/* loop counter */
    MY_MSG msg;				/* message to send */

    for (ix = 0; ix < iteration; ix++)
	{
	semGive (semId);		/* unblock tHighPri */

	taskResume (highPriId);		/* unblock tHighPri */

	/* build message and send it */

	msg.childLoopCount = ix;
	msg.buffer = TASK_LOWPRI_TEXT;
	msgQSend (msgQId, (char *) &msg, MSG_SIZE, 0, MSG_PRI_NORMAL);
	taskDelay (60);
	}

    taskResume (windDemoId);		/* wake up the windDemo task */
    }

