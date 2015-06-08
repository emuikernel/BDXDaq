/* rsMain.c - UNIX main for "RAMA" server */

/*
modification history
--------------------
01c,31oct91,rrr  passed through the ansification filter
		  -changed types.h to sys/types.h
		  -changed VOID to void
		  -changed copyright notice
01b,09aug91,gae  added printErr; cleanup.
01a,29jun90,rbr  extracted from ramaServer.c.
*/

/*
DESCRIPTION
Provides VxWorks compatible routines and is main entry point
for UNIX "rama" server.
*/

#include "vxWorks.h"
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>

/********************************************************************************
* main - UNIX "rama" server main routine
*/

void main (argc, argv)
    int argc;
    char *argv [];

    {
    char hostname [100];

    if (argc == 1)
	{
	if (gethostname (hostname, sizeof (hostname)) != 0)
	    {
	    fprintf (stderr, "gethostname errro (%d)\n", errno);
	    exit (-1);
	    }
	}
    else if (argc == 2)
	strcpy (hostname, argv [1]);
    else
	{
	fprintf (stderr, "%s: invalid parameter <%s>\n",
		    argv[0], argv [argc-1]);
	exit (-1);
	}

    ramaServer (hostname);
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
/*******************************************************************************
*
* printErr - VxWorks compatible function
*
* NOTE
* Takes a fixed number of arguments.
*/

void printErr (fmt, arg1, arg2, arg3, arg4, arg5, arg6)
    char *fmt;
    int arg1;
    int arg2;
    int arg3;
    int arg4;
    int arg5;
    int arg6;

    {
    fprintf (stderr, arg1, arg2, arg3, arg4, arg5, arg6);
    }
/********************************************************************************
* rpcTaskInit - stub to satisfy reference in VxWorks' ramaServer
*/

void rpcTaskInit ()
    {
    }
