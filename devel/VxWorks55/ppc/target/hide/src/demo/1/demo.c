/* demo.c - a program to demonstrate multitasking under vxWorks */

/* Copyright 1984-1993 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01j,13feb93,jcf  cleaned.
01i,31oct91,rrr  passed through the ansification filter
		  -changed VOID to void
		  -changed copyright notice
01h,26aug88,gae  made it print the task id and name.
01g,22jun88,dnw  name tweaks.
01f,30may88,dnw  changed to v4 names.
01e,15mar88,jcf  changed vxMyTaskId to taskId.
                 removed include of vxLib.h
01d,03mar87,dnw  changed myTaskId to vxMyTaskId.
01c,08jan87,jlf  minor cleanup.
		 changed <> to " in includes.
01b,12nov86,jlf  use myTaskId instead of vxTinquiry for clarity.
01a,14aug84,jlf  written
*/

/*
DESCRIPTION
This routine prints its task id and name and its startup parameter.
Is is for demonstration purposes.  It may be spawned, run
periodically, or called directly from the shell.

EXAMPLE
The following is an example from the VxWorks shell:

    -> ld 1 <demo.o
    value = 0 = 0x0
    -> demo
    Hello from task 0x3d6cac (shell). Startup parameter was 0.
    value = 2 = 0x2
    -> sp demo, $10
    task spawned: id = 0x3c96d8, name = 1
    value = 3970776 = 0x3c96d8
    -> Hello from task 0x3c96d8 (1). Startup parameter was 16.

    -> repeat 4, demo, $a
    task spawned: id = 0x3c96d8, name = 2
    value = 0 = 0x0
    -> Hello from task 0x3c96d8 (2). Startup parameter was 10.
    Hello from task 0x3c96d8 (2). Startup parameter was 10.
    Hello from task 0x3c96d8 (2). Startup parameter was 10.
    Hello from task 0x3c96d8 (2). Startup parameter was 10.
    ->

SEE ALSO: "Getting Started"
*/

#include "vxWorks.h"
#include "taskLib.h"
#include "stdio.h"


/*******************************************************************************
*
* demo - a simple demo routine
*
* This routine prints its task id and name and startup parameter.
*/

void demo (param)
    int param;

    {
    /* find out what task we are, and print message */

    printf ("Hello from task %#x (%s). Startup parameter was %d.\n",
	    taskIdSelf (), taskName (taskIdSelf()), param);
    }
