/* ssMain.c - UNIX simpleSprite main */

/*
modification history
--------------------
01c,31oct91,rrr  passed through the ansification filter
		  -changed VOID to void
		  -changed copyright notice
01b,10aug91,gae  added printErr; cleanup.
01a,29jun90,rbr  extracted from simpleSprite.c.
*/

/*
DESCRIPTION
Provides VxWorks compatible routines and is main entry point
for UNIX "simple sprite" client.
*/

#include "vxWorks.h"
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>

/********************************************************************************
* main - UNIX main for "sprite" client
*/

void main (argc, argv)
    int argc;
    char *argv [];

    {
    int xvel;
    int yvel;
    char hostname [100];	/* our host name */

    if (argc == 3)
	gethostname (hostname, sizeof (hostname));
    else if (argc == 4)
	strcpy (hostname, argv [1]);
    else
        {
        fprintf (stderr, "usage: %s [host] xvel yvel\n", argv [0]);
        return;
        }

    xvel = atoi (argv [argc - 2]);
    yvel = atoi (argv [argc - 1]);

    simpleSprite (hostname, xvel, yvel);
    }
/********************************************************************************
* taskDelay -
*/

void taskDelay (ticks)
    int ticks;

    {
    int yy;
    int ix;

    for (ix = 0; ix < ticks * 10000; ix++)
        yy += ix;
    }
/********************************************************************************
* hostGetByName - VxWorks compatible function to return inet address as long
*/

long hostGetByName (hostname)
    char *hostname;

    {
    struct hostent *destHost;

    /* get the internet address for the given host */

    if ((destHost = (struct hostent *) gethostbyname (hostname)) == NULL)
        return ((u_long)ERROR);

    return (*(u_long *)destHost->h_addr);
    }
/********************************************************************************
* errnoGet - VxWorks compatible function to return errno
*/

int errnoGet ()

    {
    return (errno);
    }
/********************************************************************************
* printErr - VxWorks compatible function
*
* NOTE
* Takes fixed number of arguments.
*/

int printErr (fmt, arg1, arg2, arg3, arg4, arg5, arg6)
    char *fmt;
    int arg1;
    int arg2;
    int arg3;
    int arg4;
    int arg5;
    int arg6;

    {
    fprintf (stderr, fmt, arg1, arg2, arg3, arg4, arg5, arg6);
    }
/********************************************************************************
* printErrno - VxWorks compatible function to print errno
*/

int printErrno (no)
    int no;

    {
    fprintf (stderr, "errno = %d\n", (no == 0) ? errno : no);
    }
/********************************************************************************
* rpcTaskInit - stub to satisfy reference in simpleSprite()
*/

void rpcTaskInit ()
    {
    }
