/* Copyright 2002 Wind River Systems, Inc. */

/* shellEcho.c - An example of a custom shell to interface with the telnet library */
/*
modification history
--------------------
00a,30apr02,elr  Written for Tornado 2.2 
*/

/*

HOW TO MAKE A SHELL:
--------------------

1) Decide how many shells you want to run simultaneously.
 
2) Decide if you want all of the resources (tasks and memory)
       associated with each shell created staticly during telnetd initialization
       for speed,  or created as each connection is established for memory efficiency.

3) A shell consists of at least two functions:
    a - The control function (parser control)
    b - the shell

4) Your shell control function is installed by defining the macro TELNETD_PARSER_CONTROL in config.h
   This control function is responsible for initializing, starting and stopping the shell(s).

5) Example installation:
   Add your shell into your BSP 
    a) defining MACH_EXTRA = echoShell.o
    b) defining the following lines BEFORE the line: include "configAll.h"
    c) The exact definitions will need to be tailored to your specific application.

#ifndef	_ASMLANGUAGE
extern STATUS shellParserControlDemo();
#endif
#define TELNETD_PARSER_CONTROL shellParserControlDemo
#define TELNETD_MAX_CLIENTS 4
#define TELNETD_TASKFLAG 1 /@ telnetd will create all tasks during init  @/
#include "configAll.h"


6) A shell session has three states that will be passed to your control function shellParserControlDemo() :
       REMOTE_INIT (Can be used for initialization (or running)
       REMOTE_START (Used to notify that a connection has been made)
            The return value during REMOTE_START is either:
                 OK - The shell is started / (and possibly authentication worked)
              ERROR - Authentication failed / not enough resources / some other failure
                      - The (possible) connection will be shut down.
     
       REMOTE_STOP  (The remote side has terminated the connection)

7) The telnetdExit() must be called from the shell when the shell is terminating a remote session. This 
     is done in order to clean up the communications tasks and open sockets/file descriptor.  
     You should consider deleteHook with a call to telenetdExit(),
     however a slight performance degradation of all task deletions will occur.

   NOTE: Never delete a shell task that is in the REMOTE_START state without first calling telnetdExit()
   because the resources (memory, file descriptors and tasks) must first be removed.

   NOTE: calling telnetdExit() will not cause shellParserControl( ..., REMOTE_STOP,...) to occur.

Authentication
--------------
You have the option of initiating an authentication (login) routine when your userParserControlFunction()
         receives REMOTE_START.   The return value of userParserControlFunction() indicates whether
         the authentication was successfull (OK or ERROR).  Please note that the callers task context of 
         userParserControlFunction() is that of telnetd and is therefore single-threaded.  This may be
         usefull to prevent denial-of-service attacks from overloading the kernel or password cracking.   
         Prudent use of delays after an authentication check are also recommended for this reason.


If the designer of a shell wants simultaneous authentications to be performed, then have your 
        userParserControlFunction() start the authentication as part of the shells context before 
        it returns OK to telnetd.



ABOUT THIS DEMONSTRATION CODE:
------------------------------
The following is an example of a simple telnet application and is provided in the unsupported directory.
This code is unsupported.  Use it at your own risk!

This shell can be run in either the dynamic or the static modes.   You can run as many sessions as memory
will allow.   The resident shell may be present.  The resident shell will not be the shell that telnetd connects.

Incidently, FYI: the VxWorks shell can * NOT * be run in the static modes when connected to telnetd!

This shell waits for input with a timeout.   If the the timer expires, the session is terminated.
If the user enters some character input followed by a carriage return,  the input is reprinted and 
  the session is terminated.

This code is meant a template for your own aplications.  For code simplicity,  you may remove either the 
dynamic or the static state.

*/


#include "copyright_wrs.h"


#include "vxWorks.h"
#include "stdlib.h"
#include "semLib.h"
#include "string.h"
#include "lstLib.h"
#include "loginLib.h"
#include "ioLib.h"
#include "symLib.h"
#include "symbol.h"
#include "remLib.h"
#include "errnoLib.h"
#include "stdio.h"
#include "unistd.h"
#include "sysLib.h"
#include "tickLib.h"
#include "taskLib.h"
#include "telnetLib.h"
#include "logLib.h"

#define DEMO_SHELL_TASK_PRIORITY 100
#define DEMO_SHELL_TASK_OPTIONS  VX_SUPERVISOR_MODE
#define DEMO_SHELL_TASK_SIZE     10000

#define MAX_NUM_SHELLS 3
#define MAX_LEN 100

/* global variables */


/* local variables */

/* forward declarations */

void echoShell (TELNETD_SESSION_DATA  *session);
  
/*
 * The following taskLib functions are undocumented, and unsupported!
 * These task functions should not be used for Tornado AE.
 * They were used for efficiency, but you can use taskSpawn/taskDelete
 *  and a binary semaphore just as easily.
 *
 */ 
extern int taskCreat(); 
extern int taskInit(); 
extern STATUS taskTerminate();


/* 
 * This function is called by the telnetd daemon.  We must always return quickly 
 * from this function as the telnetd is waiting for us to finish. 
 *
 * RETURNS:
 * The return result is critical for REMOTE_INIT and REMOTE_START
 * Returning FAIL during REMOTE_INIT indicates problems with initialization 
 *   and telnetd will not connect.
 * Returning FAIL during REMOTE_START indicates that the connection should be
 *   dropped.  This is typical if the remote user failed to login properly.
 * In this case, the loging-in stage is done from the context of telnetd.
 * This is acceptable since a denial-of-service attack or password cracker
 *   might attempt multiple logins.  Here,  we only allow a single login at a time
 */
STATUS shellParserControlDemo
    (
    UINT32 remoteEvent,	                /* Starting or stopping a connection? */
    TELNETD_SESSION_DATA  *session, 	/* identifier for each session */
    UINT32 slaveFd                      /* Preferred file descriptor for character i/o.
                                         * An alternative is: session->slaveFd
                                         */
    )
    {
    char sessionTaskName[128];

    switch (remoteEvent)
        {
        case REMOTE_INIT:
            {
            /* 
             * Create a task during telnet library initialization,  
             * but don't start it yet until we have a client socket connection
             * 
             * NOTE: We are seperating the creation of the task so it
             *       will activate quickly when the connection arrives
             *       and also in case there is a time-dependant login routine 
             *       in the shell
             *      
             *       An alternative would be to just do a taskSpawn here
             *       or in the REMOTE_START case.  The task might want to 
             *       immediately pend on a binary semaphore if it has a time-dependant
             *       login routine.   The semaphore would then be given on REMOTE_START.
             *
             */
            if ( telnetdStaticTaskInitializationGet() == TRUE )
                {
                (SEM_ID *)(session->userData) = semBCreate (SEM_Q_FIFO, SEM_EMPTY);
                if ((int)(session->userData) == (int)NULL)
                    return (ERROR);                   
                
                sprintf (sessionTaskName, "echoShell_%08x", (unsigned int)session);
                session->shellTaskId = 
                taskSpawn (sessionTaskName, 
                           DEMO_SHELL_TASK_PRIORITY,
                           DEMO_SHELL_TASK_OPTIONS, 
                           DEMO_SHELL_TASK_SIZE,
                           (FUNCPTR)echoShell, 
                           (int)session,
                           0, 0, 0, 0, 0, 0, 0, 0, 0);
                }
            return (OK);
            break;
            }

        case REMOTE_START:
            {
            if ( telnetdStaticTaskInitializationGet() != TRUE )
                {
                
                sprintf (sessionTaskName, "echoShell_%08x", (unsigned int)session);
                session->shellTaskId = 
                taskSpawn (sessionTaskName, 
                           DEMO_SHELL_TASK_PRIORITY,
                           DEMO_SHELL_TASK_OPTIONS, 
                           DEMO_SHELL_TASK_SIZE,
                           (FUNCPTR)echoShell, 
                           (int)session,
                           0, 0, 0, 0, 0, 0, 0, 0, 0);
                }
            else
                {
                /* will taskActivate will start the task */
                semGive ((SEM_ID)session->userData);
                }
            return (OK);
            break;
            }

       
        /* 
         *  This case is due to one of the following:
         *         Remote session forced a disconnect via telnet protocal
         *         Remote session dropped socket connection 
         */
        case REMOTE_STOP:
            {

            /* Check if we need to re-use this task */
            if ( telnetdStaticTaskInitializationGet() == TRUE )
                {
                /* 
                 * STATIC INITIALIZATION OF SHELL TASKS
                 *
                 * The restarted task will probably pend on a 
                 * binary semaphore representing 'REMOTE_START'
                 */

                taskRestart (session->shellTaskId);
                }
            else
                {
                /* 
                 * DYNAMIC INITIALIZATION OF SHELL TASKS
                 *
                 * It is our responsibility to delete the shell we 
                 * created.  The telnetd libary will cleanup the *session 
                 * object after we return from this function.
                 */              
                taskDelete (session->shellTaskId);
                }
            return (OK);
            }
            break;
        default:
            return (ERROR); /* We should never get here */
        }
     }

/*****************************************************************************
*
* echoShell - Just prompt and echo what is typed...
*
*/

void echoShell
    (
    TELNETD_SESSION_DATA  *session 	/* session identifier */
    )
    {
    int   nbytes 	  = 0;		/* bytes read */
    int   timeOutInSecond = 15;
    long  bytesToRead;
    ULONG secondPassed;
    ULONG startTick;
    char  name[MAX_LEN];

    /* Set our i/o so this task uses the telnet connection */

    ioTaskStdSet (0, STD_IN, session->slaveFd);
    ioTaskStdSet (0, STD_OUT, session->slaveFd);
    ioTaskStdSet (0, STD_ERR, session->slaveFd);

    name[0] = EOS;

    if ( telnetdStaticTaskInitializationGet() == TRUE )
        semTake ((SEM_ID)(session->userData), WAIT_FOREVER);

    printf ( "\n\nWelcome to the demonstration shell. running as taskId:%08x\n", taskIdSelf());
    printf ("One line of your input will be echo'ed back and then the session ends.\n");
    while (name == NULL || name[0] == EOS)
	{
	printf ("\nOk,  here is a prompt (enter to complete the input) >");

	secondPassed = 0;
	bytesToRead  = 0;
	startTick    = tickGet ();

	while (secondPassed < timeOutInSecond && bytesToRead == 0)
	    {
	    if (ioctl (STD_IN, FIONREAD, (int)&bytesToRead) == ERROR)
                {
                if (telnetdStaticTaskInitializationGet() == TRUE)
                    taskRestart(0);             
                else
                    {
                    /* The shell ** MUST ** call telnetdExit()  before exiting */
	            telnetdExit ((int) session); 
                    return; /* Our exit */
                    }
                }

	    taskDelay (sysClkRateGet () /4);
	    secondPassed = (tickGet () - startTick) / sysClkRateGet ();
	    }

	if (secondPassed >= timeOutInSecond)
	    {
	    printf ("\nTIMED OUT - good bye\n");
	    name[0] = EOS;
            
            if (telnetdStaticTaskInitializationGet() == TRUE)
                {
	        telnetdExit ((UINT32) session); /* The shell ** MUST ** call telnetdExit() before exiting */
                taskRestart(0);             
                }
            else
                {
	        telnetdExit ((UINT32) session); /* The shell ** MUST ** call telnetdExit() before exiting */
                return; /* Our exit */
                }
	    }

	if (bytesToRead > 0)
	   nbytes = read (STD_IN, name, MAX_LEN);

	if (nbytes > 0)
	    {
	    name [nbytes - 1] = EOS;

	    if (nbytes > 1)
	        {
		printf ("\nWow!, Nice going.  I received the string:\"%s\"\nBYE BYE!\n", name);
            
                if (telnetdStaticTaskInitializationGet() == TRUE)
                    {
                    telnetdExit ((UINT32) session); /* The shell ** MUST ** call telnetdExit() before exiting */
                    taskRestart(0);             
                    }
                else
                    {
                    telnetdExit ((UINT32) session); /* The shell ** MUST ** call telnetdExit() before exiting */
                    return; /* Our exit */
                    }
		}
	    }
        printf ("I didn't get any input!  Please try again\n");
        }

            
    /* just in case. However, we should never reach this point */
    if (telnetdStaticTaskInitializationGet() == TRUE)
        {
        telnetdExit ((UINT32) session); /* The shell ** MUST ** call telnetdExit() before exiting */
        taskRestart(0);             
        }
    else
        telnetdExit ((UINT32) session); /* The shell ** MUST ** call telnetdExit() before exiting */
    }
