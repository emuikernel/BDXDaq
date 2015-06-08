/* usrAltivec.c - dsp support library */

/* Copyright 1984-1999 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,10may01,dtr Created
*/

/* DESCRIPTION :
 * Initialises shell task options and general spawn task options to 
 * include VX_ALTIVEC_TASK 
 */

#include "altivecLib.h"
 
void usrAltivecInit (void)
    {
    altivecInit();
#ifdef  INCLUDE_SHELL
    {
    extern int shellTaskOptions;
    extern int spTaskOptions;
    shellTaskOptions |= VX_ALTIVEC_TASK;
    spTaskOptions |= VX_ALTIVEC_TASK;
    }
#endif  /* INCLUDE_SHELL */

#ifdef  INCLUDE_SHOW_ROUTINES
    altivecShowInit ();                     /* install dsp show routine */
#endif  /* INCLUDE_SHOW_ROUTINES */
    }


