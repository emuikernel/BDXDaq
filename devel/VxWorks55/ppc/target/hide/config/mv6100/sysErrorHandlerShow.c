/* sysErrorHandlerShow.c - Generic Error Handling "Show" routines */

/* Copyright 2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01b,10aug04,scb Add sysErrorScan and sysErrorClear to "help" message.
01a,03aug04,scb Initial writing.
*/

/*
DESCRIPTION
This file contains the show routines for the error handling module.  To
include these routines, define INCLUDE_SHOW_ROUTINES in config.h.

*/

/* includes */

#include "vxWorks.h"
#include "logLib.h"
#include "stdio.h"
#include "errorHandler.h"

/* defines */

/* typedefs */

/* globals */

/* locals */

/* forward declarations */

void sysErrorDomainShow (void);

void sysErrorRegisterShow (ERROR_ID mask, ERROR_ID compare);

void sysErrorLogRawShow (ERROR_ID mask, ERROR_ID compare);
void sysErrorLogShow (ERROR_ID mask, ERROR_ID compare);
void sysErrorHandlerShow (void);
void sysErrorHandlerHelp (void);

LOCAL void errorInitFormat (FORMAT_CALLBACK callBackFcn);

LOCAL void errorRegisterFormat (ERROR_ID mask, ERROR_ID compare, 
			        FORMAT_CALLBACK callBackFcn);

LOCAL void sysErrorHandlerHelpDisplay (FORMAT_CALLBACK callBack);

/******************************************************************************
*
* sysErrorDomainShow - Show which domains are initialized with error handler
*
* This function displays to the console all of the error domains which
* are currently initialized.
*
* RETURNS: NA
*
* SEE ALSO: errorInitFormat
*/

void sysErrorDomainShow (void)
    {
    errorInitFormat (consoleDisplay);
    }

/******************************************************************************
*
* sysErrorRegisterShow - Show which errors have been "registered".
*
* This function displays to the console all of the currently "registered"
* errors.  Errors are selected for display if their ID matches the 'mask'
* and 'compare' parameters.  If ((mask & tableEntryID) == compare) then it
* is considered a match.  Using a 'mask' parameter of zero and a 'compare'
* parameter of zero will cause every entry in the error initialization
* table match and thus the entire table will be displayed.  
*
* RETURNS: NA
*
* SEE ALSO: errorRegisterFormat
*/

void sysErrorRegisterShow 
    (
    ERROR_ID	       mask,	/* AND mask to apply to table IDs */
    ERROR_ID           compare	/* COMPARE value for masked match */
    )
    {
    errorRegisterFormat (mask, compare, consoleDisplay);
    }

/******************************************************************************
*
* sysErrorLogRawShow - Show the error log (raw)
*
* This function serves as an alias for sysErrorLogRawDisplay.
*
* RETURNS: NA
*/

void sysErrorLogRawShow 
    (
    ERROR_ID	       mask,	/* AND mask to apply to table IDs */
    ERROR_ID           compare	/* COMPARE value for masked match */
    )
    {
    sysErrorLogRawDisplay (mask, compare);
    }

/******************************************************************************
*
* sysErrorLogShow - Show the error log (formatted)
*
* This function serves as an alias for sysErrorLogDisplay.
*
* RETURNS: NA
*/

void sysErrorLogShow 
    (
    ERROR_ID	       mask,	/* AND mask to apply to table IDs */
    ERROR_ID           compare	/* COMPARE value for masked match */
    )
    {
    sysErrorLogDisplay (mask, compare);
    }

/******************************************************************************
*
* sysErrorHandlerShow - Show all error handling information
*
* This routine invokes each of the error handling "show" functions:
* sysErrorDomainShow, sysErrorRegisterShow, sysErrorLogRawShow and
* sysErrorLogShow.
*
* RETURNS: NA
*
*/

void sysErrorHandlerShow (void)
    {
    sysErrorDomainShow ();
    sysErrorRegisterShow (0, 0);
    sysErrorLogRawShow (0, 0);
    sysErrorLogShow (0, 0);
    }

/******************************************************************************
*
* sysErrorHandlerHelp - Show console invocation functions for error handling.
*
* This routine shows functions which can be called via console command to
* show and manage the error handling tables.
*
* RETURNS: NA
*
*/

void sysErrorHandlerHelp (void)
    {
    sysErrorHandlerHelpDisplay (consoleDisplay);
    }

/******************************************************************************
*
* sysErrorHandlerHelpDisplay - Format a "help" error handler message.
*
* This function formats a display of the error handler help information.
* The display consists of console callable functions.
*
* RETURNS: NA
*/

LOCAL void sysErrorHandlerHelpDisplay 
    (
    FORMAT_CALLBACK callBack	/* Callback function to process each line */
    )
    {
    (* callBack)("\n\rsysErrorHandlerHelp:\n\r");
    (* callBack)("\n\r");
    (* callBack)("  sysErrorHandlerShow():\n\r");
    (* callBack)("    Displays all error handling tables\n\r");
    (* callBack)("\n\r");
    (* callBack)("  sysErrorDomainShow()\n\r");
    (* callBack)("    Shows currently initialized error domains\n\r");
    (* callBack)("\n\r");
    (* callBack)("  sysErrorRegisterShow\n\r");
    (* callBack)("    Shows error IDs of currently \"registered\" errors\n\r");
    (* callBack)("\n\r");
    (* callBack)("  sysErrorLogRawShow (mask, compare)\n\r");
    (* callBack)("    Shows currently logged errors in \"raw\" format\n\r");
    (* callBack)("\n\r");
    (* callBack)("  sysErrorLogShow (mask, compare)\n\r");
    (* callBack)("    Shows currently logged errors in full format\n\r");
    (* callBack)("\n\r");
    (* callBack)("  sysErrorLogDisplay (mask, compare)\n\r");
    (* callBack)("    Same as sysErrorLogShow (mask, compare)\n\r");
    (* callBack)("\n\r");
    (* callBack)("  sysErrorLogRemove (mask, compare)\n\r");
    (* callBack)("    Removes selected errors from logging table\n\r");
    (* callBack)("\n\r");
    (* callBack)("  sysErrorDispositionIgnore (mask, compare)\n\r");
    (* callBack)("    Sets disposition of selected errors to \"ignore\"\n\r");
    (* callBack)("\n\r");
    (* callBack)("  sysErrorDispositionSave (mask, compare)\n\r");
    (* callBack)("    Adds disposition of \"save\" to selected errors\n\r");
    (* callBack)("\n\r");
    (* callBack)("  sysErrorDispositionDisplay (mask, compare)\n\r");
    (* callBack)("    Adds disposition of \"display\" to selected errors\n\r");
    (* callBack)("\n\r");
    (* callBack)("  sysErrorDispositionFunction (mask, compare)\n\r");
    (* callBack)
        ("    Adds disposition of \"function call\" to selected errors\n\r");
    (* callBack)("\n\r");
    (* callBack)("  sysErrorScan (mask, compare)\n\r");
    (* callBack)("    Scans for assertion of selected errors\n\r");
    (* callBack)("\n\r");
    (* callBack)("  sysErrorClear (mask, compare)\n\r");
    (* callBack)("    Clears assertion of selected errors\n\r");
    (* callBack)("\n\r");
    (* callBack)(NULL);
    }

/******************************************************************************
*
* errorInitFormat - Format a display of error initialization table
*
* This function formats a display of the error initialization table.
* e.g. "CPU   01xxxxxx".  The formatted display shows the ASCII domain 
* identifier ("CPU", "PCI", etc.) along with the associated error ID type 
* (01xxxxxx, 05xxxxxx etc.).  The entries were placed into this table 
* via call to _func_errorHandlerInit().  Each formatted line is presented
* to a callback function which is an input argument to this function.
*
* RETURNS: NA
*/

LOCAL void errorInitFormat 
    (
    FORMAT_CALLBACK callBack	/* Callback function to process each line */
    )
    {
    ERROR_INIT_ENTRY entry;
    int index;
    int i;
    char scr[100];

    i = 0;
    index = ERROR_FIRST;
    entry = sysErrorInitEntryNext (&index);

    while (index != ERROR_DONE)
	{
	if (i == 0)
            {
            (* callBack)("\n\rError Domain Initializations: "
			 "sysErrorDomainShow ()\n\r\n\r");
            (* callBack)("No.     Domain        Error ID      FmtFcn     "
                         "ScanFcn    ClearFcn\n\r");
            (* callBack)("-----------------------------------------------"
                         "-------------------\n\r");
            }
        sprintf(scr,"%2d      %-8s      %02xxxxxxx    %8x    %8x    %8x\n\r",
               i+1, entry.asciiDomain,
               (entry.id >> 24),
               (UINT32)entry.errorFormatFcn,
               (UINT32)entry.errorScanFcn,
               (UINT32)entry.errorClearFcn);
	(* callBack)(scr);
	entry = sysErrorInitEntryNext (&index);
	i++;
	}

    if (i == 0)
        (* callBack)("\n\rNo error initialization has been done\n\r");
    (* callBack)("\n\r\n\r");
    (* callBack)(NULL);
    }

/******************************************************************************
*
* errorRegisterFormat - Show which errors are registered
*
* This show routine formats a display all of the errors which have been 
* registered and which are unaltered when 'anded' with the input parameter
* mask.  Each line formatted corresponds to one registered error.
* An error is considered "registered" if it appears in the error registration
* as a result of a call to the function: errorUtils.errorRegister.
* The errorUtils structure is obtained via previous call to 
* _func_errorHandlerInit().  Each formatted line is presented to a callback 
* function which is also input argument to this function.
*
* RETURNS: NA
*/

LOCAL void errorRegisterFormat 
    (
    ERROR_ID mask,		/* 'ANDing' mask to filter entry */
    ERROR_ID compare,		/* Compare entry AND mask to 'compare' */
    FORMAT_CALLBACK callBack	/* Callback function to process each line */
    )
    {
    ERROR_REGISTER_ENTRY regEntry;
    ERROR_INIT_ENTRY     initEntry;
    int index;
    int i;
    char dispStr[300];
    char *p;
    char *m;
    int len;

    i = 0;
    index = ERROR_FIRST;

    while (sysErrorRegisterEntryFind (&index, mask, compare, &regEntry) == OK)
	{
        if (i == 0)
            {
            sprintf(dispStr,
		   "\n\rError Registrations: "
		   "sysErrorRegisterShow (0x%08x, 0x%08x)\n\r\n\r",mask, 
					  compare); 
            (* callBack)(dispStr);

            sprintf(dispStr,"No.  Error ID              Disposition"
                            "   Fcn       Param\n\r");
            (* callBack)(dispStr);

            (* callBack)("------------------------------------------"
                         "--------------\n\r");
            }

        sprintf(dispStr,"%3d",i+1);
        p = &dispStr[strlen(dispStr)];
        sprintf(p,"  %08x",regEntry.id);
        p = &dispStr[strlen(dispStr)];

        len = strlen(dispStr) + strlen("(qxxxxxxxxq)  ");
        sprintf(p,"(\"%s\")", sysAsciiErrorDomainGet(regEntry.id));
        while (strlen(dispStr) < len)
            strcat(dispStr," ");

        p = &dispStr[strlen(dispStr)];
        len = strlen(dispStr) + strlen("fcn.sav.dsp.");
        if (regEntry.disposition == ERROR_DISP_IGNORE)
            sprintf(p,"ignore");
        else
            {
            if (regEntry.disposition & ERROR_DISP_FUNCTION_CALL)
                sprintf(p,"fcn.");
            p = &dispStr[strlen(dispStr)];
            if (regEntry.disposition & ERROR_DISP_IMMEDIATE_DISPLAY)
                sprintf(p,"dsp.");
            p = &dispStr[strlen(dispStr)];
            if (regEntry.disposition & ERROR_DISP_SAVE)
                sprintf(p,"sav.");
            }
        while (strlen(dispStr) < len)
            strcat(dispStr," ");
        p = &dispStr[strlen(dispStr)];
        sprintf(p,"  %-8x",(UINT32)regEntry.errorFunc);
        p = &dispStr[strlen(dispStr)];
        sprintf(p,"  %-8x",regEntry.errorParam);
        strcat(dispStr,"\n\r");
        (* callBack)(dispStr);

        if (sysErrorInitEntryFind ((regEntry.id & ERROR_DOMAIN_MASK),
	      (regEntry.id & ERROR_DOMAIN_MASK), &initEntry) == OK)
	    {
	    if (initEntry.errorFormatFcn != NULL)
		{
	        if ((m = (* initEntry.errorFormatFcn)(regEntry.id)) != NULL)
		    {
	            sprintf(dispStr,"       Msg: ");
	            p = &dispStr[strlen(dispStr)];
	            if (initEntry.errorFormatFcn != NULL)
		        {
	                sprintf(p, "\"%s\"\n\r", m);
		        (* callBack)(dispStr);
		        }
		    }
		}
	    }

        i++;
        }
    if (i == 0)
        (* callBack) ("\n\rNo Errors registered\n\r");
    (* callBack)("\n\r\n\r");
    (* callBack)(NULL);
    }
