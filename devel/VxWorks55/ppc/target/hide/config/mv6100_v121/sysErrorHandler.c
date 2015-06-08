/* sysErrorHandler.c - Generic Error Handling */

/* Copyright 2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01b,10aug04,scb Bug fixes, code cleanup.
01a,03aug04,scb Initial writing.
*/

/*
DESCRIPTION

INTRODUCTION:

This module is included via #define of INCLUDE_ERROR_HANDLING.

This module implements generic error handling support.  Software which
detects an error, either through interrupts or via soft scan of
hardware can make use of this module to "dispose" of the error.
Disposition can consist of displaying an immediate indication of the
error and/or saving the information about the error in a log which can
be queried later.  An optional user-written function can also be
called upon occurrence of a particular error.  This module does not
itself detect errors but instead offers supporting functions which
allow for handling of errors, which are detected elsewhere in the
system, in a uniform way.

CONNECTING TO THE ERROR HANDLING MODULE:

Before making use of the error handling services software must
"connect" with the error handling module.  This is a two-step process
consisting first of (1) Domain Initialization and secondly of (2)
Error Registrations.  After these two steps are completed actual
logging of specific errors can be performed.

Before proceeding a distinction must be made between error
"registration" and error "logging".

Error "registration" is done once for each possible error.  It is
through "registration" that the error handling module is presented
with a unique error ID which will be monitored by application software
as the system runs.  Note that "registering" and error does not mean
that the error has occurred, it just means that this particular error
is being monitored and could potentially be logged.

Error "logging" is what is done when application software detects an
error and informs the error handling module that the error has
actually occurred.  The error handling module then disposes of the
error according to rules which were established when the error was
registered.  Thus it can be seen that to successfully log an error,
that error must first have been registered.

1) DOMAIN INITIALIZATION:

The error handling module considers a "domain" to be a software
subsystem which deals with a major component of the system.  Examples
of domains would be "CPU", "Host Bridge", "PCI", "VME Bridge"
etc.  The user has some flexibility in defining these domains.  A
domain need not necessarily be confined to actual hardware on the board,
it could be a user application such as "Widget".   It is anticipated
that there will be at most a small handful of domains associated with
a specific system.  

A "domain" is identified by two things:

a)  An error domain ID.  Such an ID consists of a 32-bit identifier,
    of which only the top byte actually identifies the domain.
    For example the host bridge might have a domain ID of 0x02000000.
    (Here the high order byte "0x02" identifies this as the host
    bridge domain,  the CPU domain might have an ID of 0x01000000).
    The domain ID is important because every error which is eventually
    logged and has an error ID of the form 0x02xxxxxx will be assumed 
    to be associated with the host bridge domain because of "0x02" as 
    the high order byte.

       and

b)  An string e.g. "Disco 2" (which will be used in display of
    subsequently logged errors).  This string can be a maximum
    of 8 characters in length.

Each domain can contain three functions important to the error handling
module: 

1) An error scanning function - 'errorScan (errorId)':
     This function, when called with a specific error ID, will "scan"
     for the assertion of the error associated with this ID.  This
     is useful for errors which are not associated with interrupts and
     must be "polled" via soft read of a hardware register.

2) An error clearing function - 'errorClear (errorId)':
     This function, when called with a specific error ID, will "clear"
     an asserted error.  Again this could be used in association with
     non-interrupt generating errors to clear status bits which
     were set when the error was asserted.

3) An error formatting function - 'errorFormat (errorId)'
     This function is called by the error handling module to format
     an error message associated with a particular error.  This
     function would be called by the error handling module in
     preparation to display the message on the console.

A domain can have all three of these functions but it is not required 
to have any of them.

Each separate domain (e.g. "CPU", "PCI", "Widget") must separately
establish initialization with the error handling module.  

A single function call from application code into the error handling
module will initialize a specific domain.  An example of how to do
this for a VME bridge domain called "Tsi148" is presented below in
the following code fragment:

.CS
    /@ Forward references to this domain's helper functions @/

    char * tsi148ErrorFormat (ERROR_ID id);
    BOOL   tsi148ErrorScan   (ERROR_ID id);
    BOOL   tsi148ErrorClear  (ERROR_ID id);

    /@ Error module tells me about his functions in here @/

    ERROR_UTILS * myErrorUtils;

    /@
     @ I, "Tsi148" domain, tell error module about 
     @ my helper functions in here.
     @/

    ERROR_USR_FUNCS tsi148ErrorFuncs;

    ...

    /@ Initialize pointers to my format, scan, and clear functions @/

    tsi148ErrorFuncs.myErrorFormat = tsi148ErrorFormat;
    tsi148ErrorFuncs.myErrorScan = tsi148ErrorScan;
    tsi148ErrorFuncs.myErrorClear = tsi148ErrorClear;

    if (_func_errorHandlerInit == NULL)
	{
	logMsg ("Error Handling Suite not linked\n",0,0,0,0,0,0);
	return(FALSE);
	}

    /@ Here's the call which initializes the domain !!! @/

    myErrorUtils = (* _func_errorHandlerInit) (0x03000000,"Tsi148", 
					       &tsi148ErrorFuncs);

    /@ Now test to see if domain initialization actually worked @/

    if (myErrorUtils.errorLog == NULL)
	{

	/@
	 @ myErrorUtils.errorLog == NULL is the error handling module's
	 @ way of telling me the the domain initialization failed.
         @/

	printf ("Tsi148 error domain initialization FAILED\n")
	}
    else
	{

	/@
	 @ Domain initialization succeeded:
	 @ Now myErrorUtils.errorRegister points to the error handling
	 @ module's error registration function and
	 @ myErrorUtils.errorLog points to the error handling module's
	 @ error logging function
         @/

	printf ("Tsi148 error domain initialization SUCCEEDED\n");

	}

	...

   /@ Here are my actual helper functions - more on this later @/

   char * tsi148ErrorFormat (ERROR_ID id)
       {
       ... /@ More said about this later @/
       }

   BOOL tsi148ErrorScan   (ERROR_ID id)
       {
       ...  /@ More said about this later @/
       }

   BOOL widgetErrorClear  (ERROR_ID id)
       {
       ... /@ More said about this later @/
       }


.CE

Notice the information that is exchanged during this domain
initialization.  The application (domain) code informs the error
handling module of its domain ID, domain ASCII string, error
formatting function, error scanning function and error clearing
function.  The error handling module in turn informs the application
(domain) code of its error registration function and error logging
function.

2) ERROR REGISTRATIONS:

After a domain has initialized as described above, it must "register"
each error that it is going to monitor and could potentially "log".
This is done by calling the "error registration" function, whose
pointer was given to the domain initialization code during in the
previous step.

The following code fragment (a continuation of the fragment
illustrating domain initialization) accomplishes this:

.CS
        /@
	 @ Forward reference to error handler function 
         @ and declaration of error handler function parameter
         @/

	void tsi148ErrorHandler (int param);
	int errorParam = 0x1234;

        /@
         @ This tells how we wish to "dispose" of the error whenever
         @ it is logged.
         @/

        ERROR_DISPOSITION disposition;

        ...

        disposition = ( ERROR_DISP_SAVE |
                        ERROR_DISP_IMMEDIATE_DISPLAY |
		        ERROR_DISP_FUNCTION_CALL ); 

	/@ Here is the call that actually registers an error @/

	if (* myErrorUtils.errorRegister)(0x03010201,
                                          disposition,
				          tsi148ErrorHandler,
	                                  errorParam);

        ...


   /@
    @ This function will be executed when the 0x03010201 error
    @ is actually "logged" because 'disposition' included
    @ ERROR_DISP_FUNCTION_CALL and we did provide a nonNull
    @ function pointer: tsi148ErrorHandler
    @/

   void tsi148ErrorHandler     (int param)
       {
       logMsg("tsi148 error handler called with argument %d\n",param);
       }

.CE

Note several things about the above example:

The function call which actually performs the registration is done
through a function pointer which was given to the domain code during
domain initialization (myErrorUtils.errorRegister).  

There are four parameters used in this function call: An error ID
(0x03010201), a "disposition", a pointer to an error handling
function, and an error handling function parameter.

The error code in this example is 0x03010201.  Notice that the high
order byte 0x03 matches the high order byte used to initialize the
"Tsi148" domain.  This is important because the error handling module
will refer to that domain's error formatting, error scanning and/or
error clearing function when it is asked to deal with this error.

The disposition in this example enables all of the active
possibilities: "save", "immediate display" and "function call".
"Save" (ERROR_DISP_SAVE) means that any future calls into the error
module to "log" an occurrence of this error will cause the information
about the error to be saved in an internal error log which can later
be displayed.  "Immediate display" (ERROR_DISP_IMMEDIATE_DISPLAY)
means that when an error is logged, an immediate display (to the
console) of this error occurrence will be performed by the error
module.  "Function call" (ERROR_DISP_FUNCTION_CALL) means that
when this particular error is "logged", the last two parameters in the 
registration call will be used to construct a function call with a 
specified parameter.  That function can be anything that the user 
desires.  

The third parameter (function pointer) can be NULL if no function
is to be called when the error is logged.  In that case, even if
ERROR_DISP_FUNCTION_CALL is part of the "disposition" no function
will be called.

There is one other possible "disposition".  If the registration
"disposition" is ERROR_DISP_IGNORE, then no action is taken by the
error handling module when the error is logged.

An error can be registered more than one time.  Each error is uniquely
identified by an error ID so if a call is made to register an error ID
which has been previously registered, the information saved from the
previous registration is overwritten.  This can be useful if
application code wants to change the disposition of an error at
run time.

An error can be registered any time after the system is up, even
after error logging has begun.

ERROR LOGGING:

Once the domain has been initialized, and a particular error has
been registered it is possible that the error will assert itself.
If the software is written so that it can detect the assertion of
the error, it can call the error handling module to "log" the
error.  The following code fragment continues the example:

   
.CS

   ERROR_MSG myErrorMsg;
   
   ...

   /@ Tsi148 code detects a "bus" error and wishes to log it @/
      
   myErrorMsg.id = 0x03010201	/@ ID of bus error, previously registered @/

   myErrorMsg.params[0] = <status reg value, read from HW = 0x00002000>
   myErrorMsg.params[1] = <access address, read from HW = 0xc000ffff>

   (* myErrorUtils.errorLog) (&myErrorMsg);

.CE

That's it.  Logging the error simply involves loading an ERROR_MSG
structure with the ID of the error to log (that ID must have
previously been registered).  Also up to six parameters are available
to provide additional information about the error.  In this example
two of those parameters are loaded with HW register values.  The
function called to actually log the error is pointed at by a function
pointer which we learned about during domain initialization.

WHAT HAPPENS WHEN AN ERROR IS LOGGED?:

The error logging is initiated when the application code makes a call
to the error handling module's error logging function.  The application
code knows which function to call because the logging function pointer 
was passed to it during domain initialization.

The error handling module first attaches a "time tag", to the error
message.  This tag indicates the date and time that the error log
request was received.

The error handling module then checks to see if the error ID has been
previously registered.  If it has not then the logging request is
rejected and ERROR is returned to the logging requester.

Now that the error handling module knows that the error is registered,
it checks the disposition of the error which was set during the last
registration call for that particular error ID.  If the error
disposition is ERROR_DISP_IGNORE there is no more work to do and the
error handling module returns OK.

The error disposition is not "ignore" so the next disposition
possibility checked is ERROR_DISP_FUNCTION_CALL.  If this disposition
is set, and the error function pointer which was presented at
registration time is not NULL, the user function is called with the
parameter that was registered.  Note that execution is still in the
same thread as the error logging call so if the function which is
doing the error logging is an interrupt service routine it should be
careful to obey ISR rules (don't do any blocking calls).

Once the disposition of "function call" has been handled, the error
logging request is posted to a message queue which is being read by a
separate execution thread called the "error task".  The error logging
execution thread then returns OK to the caller.

The error task will read the message queue to obtain the error logging
request.  It will check the registration disposition to see if it
includes ERROR_DISP_IMMEDIATE_DISPLAY.  If it does, then an indication
that the error occurred is displayed at this time (more about how this
error display is formatted is presented later).

Once the "display immediately" disposition has been handled by the
error task, it checks to see if the disposition also includes
ERROR_DISP_SAVE.  If it does, then the information included in the
original error message (along with the time tag which was added) is
saved in a circular error logging table.  The size of this table is
governed by ERROR_LOG_TABLE_SIZE.  The entry is placed at the end of
the table.  If the table is full and there is no room for this entry
to be saved, the oldest saved entry is replaced by this one.  Thus the
table will contain only the latest errors logged.

The handling of the logging request is now complete.

FORMATTING ERROR MESSAGES FOR DISPLAY:

During the domain initialization, the application code presents to
the error handling module three function pointers.  One of these
is the error message formatting pointer.  Continuing our example,
here is how the formatting function might look for the "Tsi148"
domain:

.CS
    char * tsi148ErrorFormat (ERROR_ID id);
        {
	switch (id)
	    {
	    case 0x03010201:
		return ("Bus error: status = 0x%08x, addr = 0x%08x");
            case 0x03010202:
                ....
	    default: 
		return (NULL);
	    }
        }
.CE

Notice that the formatting string does NOT include actual parameters
for the formatting specifiers.  This is intended.  Also it does not
include newline characters, these are not needed either.

Here we see the Tsi148 domain's error message formatting function.
Since a pointer to this very function was passed to the error handling
module during domain initialization for "Tsi148" and the error ID
of the error being logged begins with "0x03", the error handling
module knows to call this function to assist it in formatting a
displayable message.  In the error logging example above, we logged
a Tsi148 bus error (error ID = 0x03010201).  Now our registration
disposition for this particular error included 
ERROR_DISP_IMMEDIATE_DISPLAY.  Here is how the error handling module
might display this error.

.CS
	ErrLog:  JUL 21 15:36:38 "Tsi148  " (03010201):
                Bus error: status = 0x00002000, addr = 0xc000ffff
.CE

Notice the components of the error message:

ErrLog: Preface is always part of the message to identify this as
an "error log" message.

"JUL 21 15:36:38" - is obtained from the "time tag" which the
error handling module attached to the error message when it was 
logged.

"Tsi148  " - Is the ASCII domain string which was saved by the
error handling module during domain initialization of error type
0x03000000.

"(03010201)" - Is the error ID which was passed to the error handling
module by the application when the error was logged.

"Bus error: status = 0x00002000, addr = 0xc000fffff" is constructed
from two components:  When the error was logged, two parameters
(out of a possible maximum of six) were presented to the error handling
module in the logging message parameter elements, errorMsg.params[0], 
and errorMsg.params[1].  The errorTask checks the domain initialization
for this error and sees that an error formatting function had
been specified.  It calls this function with the error ID to obtain
the formatting string which it uses, along with the parameters passed
in errorMsg.params[0-1] to construct the string to display.

Notice that if a formatting function had not been specified, then
the error task would have only displayed the first line:

.CS
	ErrLog:  JUL 21 15:36:38 "Tsi148  " (03010201):
.CE

If certain errors in the domain do not require a formatted message
component to be displayed, the formatting function should simply
return NULL for those particular error IDs.

SHOW ROUTINES

When INCLUDE_SHOW_ROUTINES is #define'd the following functions
are available:

.CS
sysErrorDomainShow (void)
sysErrorRegisterShow (mask, compare)
sysErrorLogShow (mask, compare)
.CE

Examples of use are:

.CS

-> sysErrorDomainShow ()

Error Domain Initializations: sysErrorDomainShow ()

No.     Domain        Error ID      FmtFcn     ScanFcn    ClearFcn
------------------------------------------------------------------
 1      Disco II      02xxxxxx           0           0           0
 2      CPU           01xxxxxx           0           0           0
 3      Tsi148        03xxxxxx      114c40      114c90      114cbc
 4      PCI           04xxxxxx           0           0           0
 5      Widget        0fxxxxxx      114a2c      114a4c      114a8c

.CE

The above example shows five different domains have initialized.
Two of these domains ("Tsi148" and "Widget") have registered formatting
scanning and clearing functions.  The other domains ("Disco II",
"CPU", and "PCI") have initialized but they have specified NULL
pointers for their formatting, scanning and clearing functions.

.CS

-> sysErrorRegisterShow (0,0)

Error Registrations: sysErrorRegisterShow (0x00000000, 0x00000000)

No.  Error ID              Disposition   Fcn       Param
--------------------------------------------------------
  1  03010201("Tsi148")    dsp.sav.      0         0
       Msg: "Bus error: status = 0x%08x, addr = 0x%08x"
  2  0f010101("Widget")    dsp.sav.      0         0
  3  0f010201("Widget")    fcn.dsp.sav.  114acc    16
       Msg: "Widget count error, received = %d, actual = %d"
  4  01010001("CPU")       sav.          0         0
  5  04010101("PCI")       ignore        0         0

.CE

This shows that five errors have been registered.  The error IDs
associated with each error are displyed along with the domain name
string.  Also a disposition field is presented indicating combinations
"user function call" (fcn), "immediate display" (dsp), and/or "save in
log file" (sav) or simply "ignore".  In the above table, we see that
error code 0x03010201 and 0x0f010201 are both associated with a
formatting string (a format helper function was provided at domain
initialization and it indeed returns a non-NULL formatting string).
Also error code 0x0f010201 is associated with a logging function at
0x114acc which will be called with parameter 16 when this particular
error is logged.

sysErrorRegisterShow() takes two arguments: 'mask' and 'compare'.
Several of the functions in the error handling suite take these two
arguments.  They are used to filter error IDs which appear in a table.
Only error IDs which pass the filter are acted on by the function.

The filtering operation is:

.CS
  if ((tableID & mask) == compare)
      {

      /@ tableID passes through filter @/

      }
  else
      {

      /@ tableID is blocked by filter @/

      }
.CE

So in the above example if we had used 'mask' of 0xff000000 and
'compare' of 0x03000000 by entering:

.CS
-> sysErrorRegisterShow (0xff000000, 0x03000000)
.CE

Only the "Tsi148" error IDs would would have been displayed since
0x03xxxxxx is the only type of ID which will pass this filter.
Using (0,0) as the mask and compare allows causes everything to
match.  Since (0,0) is the default VxWorks argument passage when
no arguments are entered, simply entering:

.CS

-> sysErrorRegisterShow

.CE

Would cause the entire table to be displayed.

.CS

-> sysErrorLogRawShow (0, 0)

Errors currently logged: sysErrorLogRawDisplay (0x0, 0x0)

No.  time     ID       p0       p1       p2       p3       p4       p5
----------------------------------------------------------------------------
0001 4100e5d3 01010001 00000000 11111111 22222222 33333333 44444444 55555555
0002 4100e5d5 0f010201 00000020 00000022 22222222 33333333 44444444 55555555
0003 4100e5d7 03100015 3dd58898 3dd587e8 00000001 00000000 3dd588a0 3dd588a0
0004 4100e5d8 03100016 3dd58898 3dd587e8 00000001 00000000 3dd588a0 3dd588a0
0005 4100e5d9 03100017 3dd58898 3dd587e8 00000001 00000000 3dd588a0 3dd588a0
0006 4100e5d9 03100018 3dd58898 3dd587e8 00000001 00000000 3dd588a0 3dd588a0
0007 4100e5da 03100019 3dd58898 3dd587e8 00000001 00000000 3dd588a0 3dd588a0
.CE

This command displays in "raw" format the errors which have been saved
in the log file.  Note that this command also uses 'mask' and 'compare'
filtering.  The display show the time tag, error ID, and the six parameters
which were part of the ERROR_MSG when the error was logged.  

This same display can also be presented in a more readable format via:

.CS

-> sysErrorLogShow (0, 0)

Errors currently logged: sysErrorLogDisplay (0x0, 0x0)

0001:  JUL 23 10:17:55 "CPU     " (01010001):
0002:  JUL 23 10:17:57 "Widget  " (0f010201):
       Widget count error, received = 32, actual = 34
0003:  JUL 23 10:17:59 "Tsi148  " (03100015):
0004:  JUL 23 10:18:00 "Tsi148  " (03100016):
0005:  JUL 23 10:18:01 "Tsi148  " (03100017):
0006:  JUL 23 10:18:01 "Tsi148  " (03100018):
0007:  JUL 23 10:18:02 "Tsi148  " (03100019):

.CE

Notice that sysErrorLogRawShow and sysErrorLogShow are aliases
to the functions sysErrorLogRawDisplay and sysErrorLogDisplay.
The 'Display' functions which display the actual error log are 
considered essential any time INCLUDE_ERROR_HANDLING is #define'd.
Thus they are included even if INCLUDE_SHOW_ROUTINES is not #define'd.
Also notice in the formatted display above that error code 0x0f010201
has an associated formatted string which is displayed as port of the
table.  

.CS
-> sysErrorHandlerShow
.CE

performs the following:

.CS
-> sysErrorDomainShow()
-> sysErrorRegisterShow (0, 0)
-> sysErrorLogRawShow (0, 0)
-> sysErrorLogShow (0,0)
.CE

ERROR SCANNING and CLEARING FUNCTION

The error scanning function which is optionally presented to the
error handling module at domain initialization time should scan for
assertion of an error associated with the parameter error ID.  It's
up to the user as to what to do if the error is asserted.  Since 
this is a boolean function, it could return TRUE or FALSE depending
on whether or not the error is asserted.  Another likely thing that
it might do is to actually make an error logging call if it finds
that the error is asserted.

The error clearing function is also a boolean function.  It is 
up to the user what this function does but returning TRUE if
the error was actually cleared might be useful.

LOG TABLE MANAGEMENT ROUTINES

There are several error log table management routines which
are presented below:

sysErrorScan (mask, compare) will go through the error registration
table and for each registered ID which passes the 'mask'/'compare'
filter it will call the associated scanning routine if such a routine
exists.

sysErrorClear (mask, compare) is similar to sysErrorScan() except
it will call the error clearing routine if it exists.

sysErrorDispositionIgnore (mask, compare) will change the disposition
to 'ignore' for any registered error IDs which pass the filter.

sysErrorDispositionSave (mask, compare) will add 'save' to the
disposition of any error IDs which pass the filter.

sysErrorDispositionDisplay (mask, compare) will add the 'immediate
display' disposition to any error IDs which pass the filter.

sysErrorDispositionFunction (mask, compare) will add the 'function
call' disposition to any error IDs which pass the filter.  Note that
the already registered error should have a function registered for
calling prior to use of this command.  If the function pointer is
NULL this command has no real functional effect.

sysErrorLogRemove (mask, compare) will remove from the log file,
any logged errors whose ID passes the filter.  This will free up
some room in the table for new errors.

*/

/* includes */

#include "vxWorks.h"
#include "logLib.h"
#include "msgQLib.h"
#include "stdio.h"
#include "intLib.h"
#include "time.h"
#include "errorHandler.h"

/* defines */

/* Index value for fetching from error tables */

#define ERROR_FIRST	(-1)
#define ERROR_DONE	(-2)

#define ERROR_TABLE_LOCK      semTake (errorMsgSemId,WAIT_FOREVER)
#define ERROR_TABLE_UNLOCK    semGive (errorMsgSemId)

#define ERROR_LOG_LIST_END(p) ((p == NULL) || (p == errorLogTail))

/* typedefs */

/* Define the error initialization table entry */

typedef struct 
    {
    ERROR_ID  id;
    char      asciiDomain [ERROR_ASCII_TYPE+1];
    ERROR_FMT_FUNCPTR     errorFormatFcn;
    ERROR_SCAN_FUNCPTR    errorScanFcn;
    ERROR_CLEAR_FUNCPTR   errorClearFcn;
    } ERROR_INIT_ENTRY;

/* Define the error registration table entry */

typedef struct
    {
    ERROR_ID		id;		/* Error ID to be registered */
    ERROR_DISPOSITION   disposition;	/* Disposition code */
    ERROR_EXC_FUNC	errorFunc;	/* Function to call (or NULL) */
    ERROR_PARAM		errorParam;	/* Parameter to use in function call */
    } ERROR_REGISTER_ENTRY;

/*
 * "Internal" error logging message format: adds "utility" and "time" fields 
 * to the application error logging message format represented by
 * ERROR_MSG.
 */

typedef struct
    {
    UINT32       utility;       /* Filled in by error handling system */
    time_t       time;          /* Filled in by error handling system */
    ERROR_ID     id;            /* Copied from application task version */
    ERROR_PARAMS params;        /* Copied from application task version */
    } INTERNAL_ERROR_MSG;

/*
 * Define the error logging table entry - 
 *
 * The error logging list contains entries (errors) that have been
 * logged by application tasks.  It is organized as a singly linked
 * list with a head pointer pointing to the oldest entry in the list
 * and a tail pointer pointing to the end of the list where new entries
 * are to be linked as error logging requests are made.  For
 * simplicity of management there will always be at least one entry on
 * the list which is never removed, a dummy entry.  It will always be
 * pointed to by the tail pointer.  Newly logged entries will be placed
 * onto the list just ahead of this last dummy entry.  This
 * organization makes it easier to dynamically add and remove entries
 * from the list, preventing us from ever having to traverse the
 * entire list to find a link to the "previous" entry which we wish to
 * remove.
 */

typedef struct _ERROR_LOG_ENTRY
    {
    struct _ERROR_LOG_ENTRY *   next;	/* Next pointer in linked list */
    INTERNAL_ERROR_MSG	        msg;	/* Error Message body */
    } ERROR_LOG_ENTRY;

/* globals */

/* locals */

LOCAL MSG_Q_ID errorMsgQId     = NULL;		 
LOCAL SEM_ID   errorMsgSemId   = NULL;
LOCAL int errorTaskId	       = 0;
LOCAL int errorTaskPriority    = 0;
LOCAL int errorTaskOptions     = VX_UNBREAKABLE;
LOCAL int errorTaskStackSize   = 5000;
LOCAL ERROR_UTILS errorUtils;

/*
 * Error initialization table saves error IDs along with ASCII string 
 * identifying the domain.  This information in this table is part of 
 * the information used to construct an error message.
 */

LOCAL ERROR_INIT_ENTRY errorInitTable [ERROR_INIT_TABLE_SIZE];

/*
 * errorRegistrations contains an entry for every "registered" error,
 * include error ID and disposition, function pointer and function
 * parameter.
 */

LOCAL ERROR_REGISTER_ENTRY errorRegistrations [ERROR_REGISTER_SIZE];

/* 
 * errorReservedPool is a list of "empty" error logging entries.  When
 * an error is to be logged, an entry from this list is removed,
 * filled with the error logging information and placed at the tail of
 * the error logging list.  Likewise, when an error is removed from
 * the error logging list, it will be returned to the top of this list
 * to possibly be used later in a new logging request.  The
 * errorReservedPool list is organized just like the error log list -
 * that is it is a singly linked list pointed to by a head pointer
 * (errorReservedHead).  The last entry on the list is a dummy
 * (recognized as such because it points to NULL) and is never removed
 * from the list.  Unlike the error log list, which must allow for
 * removal of entries from the middle of the list, the
 * errorReservedPool list will only allow entries to be removed from
 * the top - in effect it acts like a stack where entries are removed
 * from the top and returned to the top.  Because this use of this
 * list is simpler than the error log list it does not require a tail
 * pointer.
 */

LOCAL ERROR_LOG_ENTRY errorReservedPool [ERROR_LOG_TABLE_SIZE + 1];

/* Here is the dummy "last" entry on the logging list */

LOCAL ERROR_LOG_ENTRY errorLogDummy;

/* Count if entries currently in "reserved" list */

LOCAL int errorReservedCount;

/* Count of entries currently logged errors (entries in error log list) */

LOCAL int errorLogCount;

/* Pointer to the head of the "reserved" list */

LOCAL struct _ERROR_LOG_ENTRY * errorReservedHead;

/* Head and tail pointers to the error log list */

LOCAL struct _ERROR_LOG_ENTRY * errorLogHead;
LOCAL struct _ERROR_LOG_ENTRY * errorLogTail;

/* forward declarations */

/* Error initialization related: */

ERROR_UTILS sysErrorHandlerInit (ERROR_ID, char * asciiDomain, 
				 ERROR_USR_FUNCS * errorFuncs);
STATUS sysErrorInitEntryFind (ERROR_ID mask, ERROR_ID compare, 
			      ERROR_INIT_ENTRY * entry);
ERROR_INIT_ENTRY sysErrorInitEntryNext (int * index);

/* Error registration related: */

STATUS sysErrorRegister (ERROR_ID id, ERROR_DISPOSITION disposition,
			 ERROR_EXC_FUNC errorFunc, ERROR_PARAM errorParam);
ERROR_REGISTER_ENTRY sysErrorRegisterEntryGet (int index);
ERROR_REGISTER_ENTRY sysErrorRegisterEntryNext (int * index);
STATUS sysErrorRegisterEntryFind (int * index, ERROR_ID mask, ERROR_ID compare, 
				  ERROR_REGISTER_ENTRY * entry );
/* Error logging related */

STATUS sysErrorLog (ERROR_MSG *msg);

/* Support for application scanning and error clearing function */

BOOL sysErrorScan (ERROR_ID mask, ERROR_ID compare);
BOOL sysErrorClear (ERROR_ID mask, ERROR_ID compare);

/* Support for changing the disposition of errors */

STATUS sysErrorDispositionIgnore (ERROR_ID mask, ERROR_ID compare);
STATUS sysErrorDispositionSave (ERROR_ID mask, ERROR_ID compare);
STATUS sysErrorDispositionDisplay (ERROR_ID mask, ERROR_ID compare);
STATUS sysErrorDispositionFunction (ERROR_ID mask, ERROR_ID compare);

/* Support for removing select error log entries */

void sysErrorLogRemove (ERROR_ID mask, ERROR_ID compare);

/*
 * Error log display routines:  Note that "displaying" the error log
 * is considered an essential part of having error handling included
 * in the build.  Thus these routines are "display" routines and not
 * subject to the conditional INCLUDE_SHOW_ROUTINES compile switch.
 */

void sysErrorLogRawDisplay (ERROR_ID mask, ERROR_ID compare);
void sysErrorLogDisplay (ERROR_ID mask, ERROR_ID compare);

/* Used by "show" routine to get ASCII error domain from error ID */

char * sysAsciiErrorDomainGet (ERROR_ID id);

/* Support for error registration */

LOCAL STATUS errorRegisterAdd (ERROR_REGISTER_ENTRY entry);

/* Support for  sysErrorLog() */

LOCAL void errorMsgLog (INTERNAL_ERROR_MSG * errorMsg);

/* Support for display of error log */

LOCAL void errorLogRawFormat (ERROR_ID  mask, ERROR_ID  compare, 
			      FORMAT_CALLBACK callBack);
LOCAL void errorLogFormat (ERROR_ID  mask, ERROR_ID  compare, 
		           FORMAT_CALLBACK callBack);

LOCAL void errorEntryDisplay (char * prepend, ERROR_INIT_ENTRY *initEntry, 
			      ERROR_REGISTER_ENTRY *regEntry, 
			      INTERNAL_ERROR_MSG *errMsg, 
			      FORMAT_CALLBACK callBack);

LOCAL void consoleDisplay (char * dispString);

/* Support for sysErrorLogRemove() */

LOCAL void _sysErrorLogRemove (ERROR_ID mask, ERROR_ID compare);

/*
 * Routines supporting management of the error log e.g. "getting" 
 * and "putting" to reserve pool, "removing" and "putting" to error log
 * list etc. 
 */

LOCAL void errorPoolEntryPut (struct _ERROR_LOG_ENTRY *p);
LOCAL struct _ERROR_LOG_ENTRY * errorPoolEntryGet (void);
LOCAL void errorLogEntryPut (struct _ERROR_LOG_ENTRY *p);
LOCAL struct _ERROR_LOG_ENTRY * errorLogEntryRemove 
					     (struct _ERROR_LOG_ENTRY *p);
LOCAL struct _ERROR_LOG_ENTRY * errorLogNext ( struct _ERROR_LOG_ENTRY *p );

/* Here is the top level function of the error task itself */

LOCAL void errorTask (void);

#ifdef INCLUDE_RTC

    /* Converts a time to a displayable string */

    LOCAL void timeString (time_t timeInSecs, char * retStr);
#endif /* INCLUDE_RTC */

/* externals */

#ifdef INCLUDE_RTC
    IMPORT void setAnsiSysTime (void);
#endif /* INCLUDE_RTC */

#ifdef INCLUDE_SHOW_ROUTINES
#    include "sysErrorHandlerShow.c"
#endif /* INCLUDE_SHOW_ROUTINES */

/******************************************************************************
*
* sysErrorHandlerInit - Initialize error handling suite
*
* This function will initialize the error handling environment for a
* particular error logging domain.  Each separate domain (e.g.  CPU,
* PCI, etc.) should call this function with an ID which is unique to
* that domain in the top byte.  For example, the "CPU" domain calls
* with an ID of 0x01000000 and all subsequent error ID's which apply
* to the "CPU" domain will be of the form 0x01xxxxxx.  This
* function is called via a function hook (_func_errorHandlerInit)
* which is initialized in sysHwInit2.  In addition to telling the
* error handling module its ID type and CII identifier (e.g.  "PCI",
* "CPU"), it also informs the error handling module where its helper
* functions (error scanning, error clearing, and error message
* formatting) are.  In turn, the error handling systems returns
* information to the application as to where the error registration
* functions and error logging functions are.
* 
* RETURNS: ERROR_UTILS containing valid function pointers
* if initialization is successful.  If initialization is not 
* successful then function pointers inside of ERROR_UTILS return are NULL.
*/

ERROR_UTILS sysErrorHandlerInit 
    (
    ERROR_ID           id,	      /* generic code (i.e. 0x02000000) */
    char *             asciiDomain,   /* ASCII domain ID (i.e "Disco II") */
    ERROR_USR_FUNCS  * errorFuncs     /* Domain helper functions */
    )
    {
    ERROR_ID myId;
    ERROR_UTILS dummyErrorUtils = { NULL, NULL };
    int i;
    int j;

    /* Retain only top byte of error ID to save */

    myId = id & ERROR_DOMAIN_MASK;

    /* If this error type is "utility" we will not register it */

    if ((id & ERROR_DOMAIN_MASK) == ERROR_UTIL_ID_MASK)
	return (dummyErrorUtils);

    /* If this is the first time we've been called ... */

    if (errorMsgQId == NULL)
	{

#ifdef INCLUDE_RTC

	/* Set the time for time tags which may follow */

	setAnsiSysTime ();

#endif /* INCLUDE_RTC */

	/* Make a blank error initialization table */

	for (i = 0; i < ERROR_INIT_TABLE_SIZE; i++)
	    {
	    errorInitTable[i].id = 0;
	    for (j = 0; j < ERROR_ASCII_TYPE; j++)
                errorInitTable[i].asciiDomain[j] = ' ';
	    errorInitTable[i].errorFormatFcn = NULL;
	    errorInitTable[i].errorScanFcn = NULL;
	    errorInitTable[i].errorClearFcn = NULL;
            }

	/* Make a blank errorRegistrations table */

	for (i = 0; i < ERROR_REGISTER_SIZE; i++)
	    {
            errorRegistrations[i].id = 0;
            errorRegistrations[i].disposition = ERROR_DISP_DEFAULT;
            errorRegistrations[i].errorFunc = NULL;
            errorRegistrations[i].errorParam = 0;
            }

	/* 
	 * Initialize the logging tables (reserved and logged).  All
	 * of the initial entries will be placed into a linked list
	 * pointed to by errorReservedHead.  When we need a new entry
	 * we will grab it from this list append it to the list
	 * pointed to by errorLogHead. 
	 */

	errorLogHead = NULL;
	errorLogTail = NULL;
	errorReservedHead = &errorReservedPool[0];
	for (i = 0; i < ERROR_LOG_TABLE_SIZE; i++)
	    {
	    errorReservedPool[i].msg.time = 0;
	    errorReservedPool[i].msg.id = 0;
	    errorReservedPool[i].msg.utility = 0;
	    errorReservedPool[i].msg.params[0] = 0;
	    errorReservedPool[i].msg.params[1] = 0;
	    errorReservedPool[i].msg.params[2] = 0;
	    errorReservedPool[i].msg.params[3] = 0;
	    errorReservedPool[i].msg.params[4] = 0;
	    errorReservedPool[i].msg.params[5] = 0;
	    errorReservedPool[i].next =  &errorReservedPool[i+1];
	    }

	/* Last entry is never used, contains pointer to NULL */

        errorReservedPool[ERROR_LOG_TABLE_SIZE].next = NULL;
	errorReservedCount = ERROR_LOG_TABLE_SIZE;
	errorLogCount = 0;

	/* Put the dummy error entry in its place */

        errorLogHead = &errorLogDummy;
        errorLogTail = &errorLogDummy;
	errorLogDummy.next = NULL;	/* This marks the dummy entry */

	/*
	 * Create a queue to decouple the error logging task from the
	 * logging thread.  The error logging task is the only thread
	 * which will change the error log list and thus we need not
	 * worry about protection from simultaneous updates.  Also, it
	 * is possible (and even likely) that the logging agent will
	 * be an interrupt service routine and we would like to dump
	 * the work of actually logging the error onto a separate thread
	 * that operates in the background from the interrupt service
	 * routine.
	 */

        errorMsgQId = msgQCreate (MAX_ERROR_Q_MSGS, sizeof (INTERNAL_ERROR_MSG),
	    		          MSG_Q_FIFO);

        if ( errorMsgQId == NULL)
	    {
	    logMsg("sysErrorHandlerInit: Cannot create error queue\n",
		   0,0,0,0,0,0);
	    return (dummyErrorUtils);
	    }

	/*
	 * Create a semaphore to protect the error initialization and
	 * error registration tables.  Note that error initialization
	 * and registration are not done by the error handling task which
	 * has been decoupled through a message queue.  Thus we will
	 * use a semaphore  to provide protection against simultaneous 
	 * updates to the initialization table or registration table.  
	 * This approach dictates that we should never have an interrupt 
	 * service routine doing error initialization calls or error 
	 * registration calls - but as previously mentioned error logging 
	 * calls are OK for ISRs to do.
	 */

	errorMsgSemId = semBCreate (SEM_Q_PRIORITY, SEM_FULL);

        if ( errorMsgSemId == NULL)
	    {
	    logMsg("sysErrorHandlerInit: Cannot create error handling "
		   "semaphore\n", 0,0,0,0,0,0);
	    (void) msgQDelete (errorMsgQId);
	    return (dummyErrorUtils);
	    }

	/* OK, let's spawn the error handling task */

	errorTaskId = taskSpawn ("errorTask", errorTaskPriority,
				 errorTaskOptions, errorTaskStackSize,
				 (FUNCPTR)errorTask, 0,0,0,0,0,0,0,0,0,0);
	if (errorTaskId == ERROR)
	    {
	    logMsg("sysErrorHandlerInit: Cannot spawn error task\n",
		   0,0,0,0,0,0);
	    (void) msgQDelete (errorMsgQId);
	    (void) semDelete (errorMsgSemId);
	    return (dummyErrorUtils);
	    }
        }

    /* 
     * Save away the generic error ID and domain string into 
     * the next open slot in the errorInitTable.  Make sure that 
     * the ascii string representing the domain is no longer than 
     * ERROR_ASCII_TYPE.  If there is no room in the table,
     * call logMsg with an error and return error indication to
     * the caller.  Also save away the associated function pointers
     * for formatting, scanning and clearing errors.
     */

    i = 0;

    ERROR_TABLE_LOCK;
    while (i < ERROR_INIT_TABLE_SIZE)
	{
	if (myId == 0)
	    break;
	if (errorInitTable[i].id == myId)
	    {
	    ERROR_TABLE_UNLOCK;
	    logMsg("sysErrorHandlerInit: duplicate error init 0x%08x\n",
		   id,0,0,0,0,0);
	    return (dummyErrorUtils);
	    }

	if (errorInitTable[i].id == 0)
	    {
	    errorInitTable[i].id = myId;

	    for (j = 0 ; j < ERROR_ASCII_TYPE; j++)
		{
		if (asciiDomain [j] == '\0')
		    break;
		errorInitTable[i].asciiDomain[j] = asciiDomain [j];
		}
	    errorInitTable[i].asciiDomain[j] = '\0';
	    errorInitTable[i].errorFormatFcn = errorFuncs->myErrorFormat;
	    errorInitTable[i].errorScanFcn = errorFuncs->myErrorScan;
	    errorInitTable[i].errorClearFcn = errorFuncs->myErrorClear;
	    break;
	    }
        i++;
	if (i >= ERROR_INIT_TABLE_SIZE)
	    {
	    ERROR_TABLE_UNLOCK;
	    logMsg("sysErrorHandlerInit: Error initialization table overflow\n",
		    0,0,0,0,0,0);
	    return (dummyErrorUtils);
	    }
	}

    /* Return the pointers to the registration and logging functions */

    ERROR_TABLE_UNLOCK;
    errorUtils.errorRegister = sysErrorRegister;
    errorUtils.errorLog = sysErrorLog;

    return (errorUtils);
    }

/******************************************************************************
*
* sysErrorInitEntryFind - Find a particular error initialization table entry
*
* This function will return the first error initialization table entry
* whose masked error ID matches the requested masked ID.  That is if 
* ((mask & tableEntryID) == compare), it is considered a match.
*
* RETURNS: OK if match was found, ERROR otherwise
*/

STATUS sysErrorInitEntryFind
    (
    ERROR_ID	       mask,	/* AND mask to apply to table IDs */
    ERROR_ID           compare,	/* COMPARE value for masked match */
    ERROR_INIT_ENTRY * entry	/* Init table entry returned here */
    )
    {
    int index;
    LOCAL ERROR_INIT_ENTRY retEntry;

    index = ERROR_FIRST;

    for (;;)
	{
        retEntry = sysErrorInitEntryNext (&index);

	if (index == ERROR_DONE)
	    {

	    /* Did not find it */

	    return (ERROR);
	    }

	if ((retEntry.id & mask) ==  compare)
	    {

	    /* We found it */

	    *entry = retEntry;
	    return (OK);

	    }
	}
    }

/******************************************************************************
* 
* sysErrorInitEntryNext - Return next entry from error initialization table
*
* This function will return the next entry from the error initialization
* table.  First call to this function should pass in an argument index
* of ERROR_FIRST, this function will indicate that there are no more entries
* to return by setting the index parameter to ERROR_DONE.
*
* RETURNS: ERROR_INIT_ENTRY item
*/

ERROR_INIT_ENTRY sysErrorInitEntryNext
    (
    int * index		/* Index of entry returned */
    )
    {
    int myIndex = *index;
    LOCAL ERROR_INIT_ENTRY retEntry;

    if (myIndex == ERROR_FIRST)
	myIndex = 0;
    else
        myIndex++;

    if ((myIndex < 0) || (myIndex >= ERROR_INIT_TABLE_SIZE) ||
        (errorInitTable[myIndex].id == 0)) 
        {
        myIndex = ERROR_DONE;
	retEntry = errorInitTable[0];
        }
    else
	retEntry = errorInitTable[myIndex];

    *index = myIndex;
    return (retEntry);
    }

/******************************************************************************
*
* sysErrorRegister - Register an error for later logging
*
* This function will accept an error code for registration.  After
* checking the code for legitimacy (by determining if the associated
* error domain has initialized), it will enter the error registration
* information into the error registration table.  Note that it is
* necessary to register an error ID successfully before an error
* logging request for that ID can be honored.  sysErrorRegister is
* called by individual domains (host bridge, cpu etc.) through the
* function pointer supplied at initialization via call to
* _func_errorHandlerInit
*
* RETURNS: OK if error successfully registered, ERROR otherwise.
*/

STATUS sysErrorRegister 
    (
    ERROR_ID          id,	       /* Error identification code */
    ERROR_DISPOSITION disposition,     /* Error disposition */
    ERROR_EXC_FUNC    errorFunc,       /* Error function to call (or NULL) */
    ERROR_PARAM	      errorParam       /* Error parameter to function call */
    )
    {
    int index;    
    ERROR_INIT_ENTRY     initEntry;
    ERROR_REGISTER_ENTRY registerEntry;

    /*
     * First check to make sure that the error being registered, has
     * had its domain registered via prior call to sysErrorHandlerInit
     */

    ERROR_TABLE_LOCK;
    index = ERROR_FIRST;

    if (sysErrorInitEntryFind ((id & ERROR_DOMAIN_MASK), 
			       (id & ERROR_DOMAIN_MASK),
			        &initEntry) == ERROR)
	{
        ERROR_TABLE_UNLOCK;

	/*
	 * The error that we are trying to register has not had its 
         * domain previously initialized via call to sysErrorHandlerInit
	 * so we must reject it - we'll squeal vi logMsg.
	 */

	logMsg("sysErrorRegister: invalid error registration, 0x%08x\n",
	       id,0,0,0,0,0);
	return (ERROR);
	}

    /*
     * OK, we know that the error type we are about to register has
     * previously had its domain initialized.  We can now register this 
     * error ID.  Note that if the ID is already in the registration 
     * table, this is not necessarily an error.  We will just overwrite
     * the previous registration with this one.
     */
	
    index = ERROR_FIRST;

    if (sysErrorRegisterEntryFind (&index, 0xffffffff, id, &registerEntry)
				   == OK)
	{

	/* This is an overwrite of an already registered error */

        registerEntry.id = id;
        registerEntry.disposition = disposition;
        registerEntry.errorFunc = errorFunc;
        registerEntry.errorParam = errorParam;
        errorRegistrations [index] = registerEntry;
	}
    else
	{

	/* This is a new registration */

        registerEntry.id = id;
        registerEntry.disposition = disposition;
        registerEntry.errorFunc = errorFunc;
        registerEntry.errorParam = errorParam;

        if (errorRegisterAdd (registerEntry) == ERROR)
	    {
            ERROR_TABLE_UNLOCK;

	    logMsg(
	    "sysErrorRegister: Error registration table overflow, 0x%08x\n",
	    id,0,0,0,0,0);

	    return (ERROR);
            }
	}

    ERROR_TABLE_UNLOCK;
    return (OK);
    }

/******************************************************************************
*
* sysErrorRegisterEntryGet - Get a particular error registration table entry
*
* This function will return the error registration table entry associated
* with a particular index.
*
* RETURNS: ERROR_REGISTER_ENTRY item
*/

ERROR_REGISTER_ENTRY sysErrorRegisterEntryGet 
    (
    int index		/* Index of registration table entry to get */
    )
    {
    return (errorRegistrations [index]);
    }

/******************************************************************************
* 
* sysErrorRegisterEntryNext - Return next entry from error registration table
*
* This function will return the next entry from the error registration
* table.  First call to this function should pass in an argument index
* of ERROR_FIRST, this function will indicate that there are no more entries
* to return by setting the index parameter to ERROR_DONE.
*
* RETURNS: ERROR_REGISTER_ENTRY item
*/

ERROR_REGISTER_ENTRY sysErrorRegisterEntryNext
    (
    int * index		/* Index of entry returned */
    )
    {
    int myIndex = *index;
    LOCAL ERROR_REGISTER_ENTRY retEntry;

    if (myIndex == ERROR_FIRST)
	myIndex = 0;
    else
	myIndex++;

    if ((myIndex < 0) || (myIndex >= ERROR_REGISTER_SIZE) ||
        (errorRegistrations[myIndex].id == 0)) 
        {
        myIndex = ERROR_DONE;
	retEntry = errorRegistrations[0];
        }
    else
	retEntry = errorRegistrations[myIndex];

    *index = myIndex;
    return (retEntry);
    }

/******************************************************************************
*
* sysErrorRegisterEntryFind - Find a particular error registration table entry
*
* This function will return the first error registration table entry
* whose masked error ID matches the requested masked ID.  That is if
* ((mask & tableEntryID) == compare), it is considered a match.
*
* RETURNS: OK if match was found, ERROR otherwise
*/

STATUS sysErrorRegisterEntryFind
    (
    int			   *index,	/* index for search */
    ERROR_ID	           mask,	/* AND mask to apply to table IDs */
    ERROR_ID               compare,	/* COMPARE value for masked match */
    ERROR_REGISTER_ENTRY   *entry	/* Init table entry returned here */
    )
    {
    int myIndex;
    LOCAL ERROR_REGISTER_ENTRY  retEntry;

    myIndex = *index;
    for (;;)
	{
        retEntry = sysErrorRegisterEntryNext (&myIndex);

	if (myIndex == ERROR_DONE)
	    {

	    /* Did not find it */

	    return (ERROR);
	    }

	if ((retEntry.id & mask) ==  compare)
	    {

	    /* We found it */

	    *entry = retEntry;
	    *index = myIndex;
	    return (OK);

	    }
	}
    }

/******************************************************************************
*
* sysErrorLog - Log an error
*
* This function accepts an ERROR_MSG item for logging.  It checks the
* legitimacy of the error code (code must have been previously
* registered via call sysErrorRegister) and calls an error routine if
* one has been connected.  It attaches a time tag, representing the
* current time, to the error message and then posts the logging request
* to the error queue for processing by the errorTask.  This function
* is called by code in an error domain (host bridge, cpu etc.) through
* the function pointer supplied at initialization via call to
* _func_errorHandlerInit
*
* RETURNS: OK if logging successfully initiated, ERROR otherwise
*/

STATUS sysErrorLog
    (
    ERROR_MSG *errorMsg		/* Error message to log */
    )
    {
    int index;
    ERROR_REGISTER_ENTRY errRegEntry;
    INTERNAL_ERROR_MSG myErrorMsg;

    /*
     * Copy the error log message to our "internal" one so we can augment
     * it with a time tag.
     */

    myErrorMsg.id = errorMsg->id;
    myErrorMsg.params[0] = errorMsg->params[0];
    myErrorMsg.params[1] = errorMsg->params[1];
    myErrorMsg.params[2] = errorMsg->params[2];
    myErrorMsg.params[3] = errorMsg->params[3];
    myErrorMsg.params[4] = errorMsg->params[4];
    myErrorMsg.params[5] = errorMsg->params[5];

#ifdef INCLUDE_RTC
    /* Inject time into the message */

    (void) time (&myErrorMsg.time);
#endif /* INCLUDE_RTC */

    /* Call error task for utility function if necessary */

    if (((errorMsg->id) & (ERROR_UTIL_ID_MASK)) != 0)
	{
        if (msgQSend (errorMsgQId, (char *)&myErrorMsg, 
                      sizeof (INTERNAL_ERROR_MSG),
		      INT_CONTEXT() ? NO_WAIT : WAIT_FOREVER, 
		      MSG_PRI_NORMAL) != OK)

            return (ERROR);
	}

    /* Not a utility call, if ID not registered return ERROR) */

    index = ERROR_FIRST;
    if (sysErrorRegisterEntryFind (&index, errorMsg->id, errorMsg->id, 
		                   &errRegEntry) == ERROR)
	{
        return (ERROR);
	}
	
    /* It's registered, if disposition is "ignore" ignore it */

    if (errRegEntry.disposition == ERROR_DISP_IGNORE)
	{
        return (OK);    
	}

    /* If function is to be called, do it here */

    if (errRegEntry.disposition & ERROR_DISP_FUNCTION_CALL)
        {
        if (errRegEntry.errorFunc != NULL)
            (* errRegEntry.errorFunc)(errRegEntry.errorParam);
        }

    /*
     * All the rest of the dispositions, (immediate display, save
     * on queue) can be done by the error task so we post
     * the message to the error task here.  To save the error
     * task some time, let's pass it the registration table index
     * via utility field in the error message.
     */

    myErrorMsg.utility = index;

    if (msgQSend (errorMsgQId, (char *)&myErrorMsg, sizeof (INTERNAL_ERROR_MSG),
	          INT_CONTEXT() ? NO_WAIT : WAIT_FOREVER, 
		  MSG_PRI_NORMAL) != OK)

	{
        return (ERROR);
	}

    return (OK);
    }

/*****************************************************************************
*
* sysErrorScan - Scan for asserted errors
*
* This function will call the "scanning" function (if it exists) for 
* errors which have been registered and also match the mask and compare 
* parameters.  It is considered a match if 
* ((mask & registrationTableEntryID) == compare).  
* Note that the entire registration table is scanned and it is possible 
* for multiple matches to occur.  If this is the case, multiple scanning 
* functions will be called.
*
* RETURNS: Value (TRUE or FALSE) of last scanning function called.  FALSE
* is returned if no scanning function is called.
*/

BOOL sysErrorScan
    (
    ERROR_ID  mask,		/* AND mask to apply to table IDs */
    ERROR_ID  compare		/* COMPARE value for masked match */
    )
    {
    int index1;
    int index2;
    ERROR_INIT_ENTRY initEntry;
    ERROR_REGISTER_ENTRY regEntry;
    BOOL ret = FALSE;

    index1 = ERROR_FIRST;
    while (sysErrorRegisterEntryFind (&index1, mask, compare, 
				      &regEntry) == OK)
        {
        index2 = ERROR_FIRST;
        for (;;)
  	    {
	    initEntry = sysErrorInitEntryNext (&index2);

	    if (index2 == ERROR_DONE)
	        break;

            if ((regEntry.id & ERROR_DOMAIN_MASK) == 
	        (initEntry.id & ERROR_DOMAIN_MASK))
		    {
		    if (initEntry.errorScanFcn != NULL)
	                 ret = (initEntry.errorScanFcn) (regEntry.id);
		    }
	    }
        }
   
    return (ret);
    }

/*****************************************************************************
*
* sysErrorClear - Clear asserted errors
*
* This function will call the "error clearing" function (if it exists) for 
* errors which have been registered and also match the mask and compare 
* parameters.  It is considered a match if 
* ((mask & registrationTableEntryID) == compare).  
* Note that the entire registration table is scanned and it is possible 
* for multiple matches to occur.  If this is the case, multiple clearing 
* functions will be called.
*
* RETURNS: Value (TRUE or FALSE) of last clearing function called.  FALSE
* is returned if no clearing function is called.
*/

BOOL sysErrorClear
    (
    ERROR_ID  mask,		/* AND mask to apply to table IDs */
    ERROR_ID  compare		/* COMPARE value for masked match */
    )
    {
    int index1;
    int index2;
    ERROR_INIT_ENTRY initEntry;
    ERROR_REGISTER_ENTRY regEntry;
    BOOL ret = FALSE;

    index1 = ERROR_FIRST;
    while (sysErrorRegisterEntryFind (&index1, mask, compare, 
				      &regEntry) == OK)
        {
        index2 = ERROR_FIRST;
        for (;;)
  	    {
	    initEntry = sysErrorInitEntryNext (&index2);

	    if (index2 == ERROR_DONE)
	        break;

            if ((regEntry.id & ERROR_DOMAIN_MASK) == 
	        (initEntry.id & ERROR_DOMAIN_MASK))
		    {
		    if (initEntry.errorClearFcn != NULL)
	                 ret = (initEntry.errorClearFcn) (regEntry.id);
		    }
	    }
        }
   
    return (ret);
    }

/*****************************************************************************
*
* sysErrorDispositionIgnore - Change error registration disposition to "ignore"
*
* This function will change the error disposition to "ignore" for any
* errors in the registration table whose IDs match the requested masked
* ID.  That is if ((mask & tableEntryID) == compare), it is considered a 
* match.
*
* RETURNS: OK if registered error found (and disposition changed), 
* ERROR otherwise.
*
*/

STATUS sysErrorDispositionIgnore
    (
    ERROR_ID  mask,	/* AND mask to apply to registration table IDs */
    ERROR_ID  compare	/* COMPARE value for masked match */
    )
    {
    int index;
    STATUS stat = ERROR;
    ERROR_REGISTER_ENTRY registerEntry;

    index = ERROR_FIRST;
    while (sysErrorRegisterEntryFind (&index, mask, compare, 
	                              &registerEntry) == OK)
        {
	stat = OK;
	registerEntry.disposition = ERROR_DISP_IGNORE;
        errorRegistrations [index] = registerEntry;
	}
    return (stat);
    }

/*****************************************************************************
*
* sysErrorDispositionSave - Add "save" error registration disposition.
*
* This function add the error disposition to "save" to any errors in the 
* registration table whose IDs match the requested masked ID.  That is if 
* ((mask & tableEntryID) == compare), it is considered a  match.
*
* RETURNS: OK if registered error found (and disposition changed), 
* ERROR otherwise.
*
*/

STATUS sysErrorDispositionSave
    (
    ERROR_ID  mask,	/* AND mask to apply to registration table IDs */
    ERROR_ID  compare	/* COMPARE value for masked match */
    )
    {
    int index;
    STATUS stat = ERROR;
    ERROR_REGISTER_ENTRY registerEntry;

    index = ERROR_FIRST;
    while (sysErrorRegisterEntryFind (&index, mask, compare, 
				      &registerEntry) == OK)
        {
	stat = OK;
	registerEntry.disposition |= ERROR_DISP_SAVE;
        errorRegistrations [index] = registerEntry;
	}
    return (stat);
    }

/*****************************************************************************
*
* sysErrorDispositionDisplay - Add "display" error registration disposition.
*
* This function add the error disposition to "display" to any errors in the 
* registration table whose IDs match the requested masked ID.  That is if 
* ((mask & tableEntryID) == compare), it is considered a  match.
*
* RETURNS: OK if registered error found (and disposition changed), 
* ERROR otherwise.
*
*/

STATUS sysErrorDispositionDisplay
    (
    ERROR_ID  mask,	/* AND mask to apply to registration table IDs */
    ERROR_ID  compare	/* COMPARE value for masked match */
    )
    {
    int index;
    STATUS stat = ERROR;
    ERROR_REGISTER_ENTRY registerEntry;

    index = ERROR_FIRST;
    while (sysErrorRegisterEntryFind (&index, mask, compare, 
				      &registerEntry) == OK)
        {
	stat = OK;
	registerEntry.disposition |= ERROR_DISP_IMMEDIATE_DISPLAY;
        errorRegistrations [index] = registerEntry;
	}
    return (stat);
    }

/*****************************************************************************
*
* sysErrorDispositionFunction - Add "function" to error registration disposition
*
* This function add the error disposition to "function cal " to any errors 
* in the  registration table whose IDs match the requested masked ID.  That 
* is if ((mask & tableEntryID) == compare), it is considered a  match.
*
* RETURNS: OK if registered error found (and disposition changed), 
* ERROR otherwise.
*
*/

STATUS sysErrorDispositionFunction
    (
    ERROR_ID  mask,	/* AND mask to apply to registration table IDs */
    ERROR_ID  compare	/* COMPARE value for masked match */
    )
    {
    int index;
    STATUS stat = ERROR;
    ERROR_REGISTER_ENTRY registerEntry;

    index = ERROR_FIRST;
    while (sysErrorRegisterEntryFind (&index, mask, compare, 
				      &registerEntry) == OK)
        {
	stat = OK;
	registerEntry.disposition |= ERROR_DISP_FUNCTION_CALL;
        errorRegistrations [index] = registerEntry;
	}
    return (stat);
    }

/******************************************************************************
*
* sysErrorLogRemove - Remove logged entries from the table
*
* This function will initiate removal of select error logging table
* entries.  "Removed" entries are placed back onto the reserve pool.
* The "mask" and "compare" parameters are used against the logging
* table ID to determining a match.  If ((mask & tableEntryID) ==
* compare), it is considered a match.  This function actually queues
* the request through the error handling task message queue so it is
* the errorTask which actually performs the removal.
*
* RETURNS: NA
*/

void sysErrorLogRemove
    (
    ERROR_ID  mask,		/* AND mask to apply to table IDs */
    ERROR_ID  compare		/* COMPARE value for masked match */
    )
    {
    INTERNAL_ERROR_MSG errorMsg;
    errorMsg.id = ERROR_UTIL_FCN_CALL_ID;
    errorMsg.params[0] = (UINT32) mask;
    errorMsg.params[1] = (UINT32) compare;
    errorMsg.utility = (UINT32) &_sysErrorLogRemove;

    (void) msgQSend (errorMsgQId, (char *)&errorMsg, 
		     sizeof (INTERNAL_ERROR_MSG), 
		     INT_CONTEXT() ? NO_WAIT : WAIT_FOREVER, 
		     MSG_PRI_NORMAL);
    }

/******************************************************************************
*
* sysErrorLogRawDisplay - Display the error log in "raw" format.
*
* This function displays to the console all of the selected errors which
* are currently logged.  Errors are selected for display if they match the
* 'mask' and 'compare' parameters.  If ((mask & tableEntryID) == compare)
* then it is considered a match.  Using a 'mask' parameter of zero and a
* 'compare' parameter of zero will cause every entry in the error logging
* table match and thus the entire table will be displayed.  The display is
* raw (not formatted).
*
* RETURNS: NA
*/

void sysErrorLogRawDisplay 
    (
    ERROR_ID  mask,		/* AND mask to apply to error log table IDs */
    ERROR_ID  compare		/* COMPARE value for masked match */
    )
    {
    errorLogRawFormat (mask, compare, consoleDisplay);
    }

/******************************************************************************
*
* sysErrorLogDisplay - Display the error log (formatted).
*
* This function displays to the console all of the selected errors which
* are currently logged.  Errors are selected for display if they match the
* 'mask' and 'compare' parameters.  If ((mask & tableEntryID) == compare)
* then it is considered a match.  Using a 'mask' parameter of zero and a
* 'compare' parameter of zero will cause every entry in the error logging
* table to match and thus the entire table will be displayed.
*
* RETURNS: NA
*/

void sysErrorLogDisplay 
    (
    ERROR_ID  mask,		/* AND mask to apply to error log table IDs */
    ERROR_ID  compare		/* COMPARE value for masked match */
    )
    {
    errorLogFormat (mask, compare, consoleDisplay);
    }

/******************************************************************************
*
* sysAsciiErrorDomainGet - Find ASCII domain string
*
* This function takes an error ID and returns the ASCII domain 
* description string which is associated with this ID in the 
* error domain table.  
*
* RETURNS: pointer to ASCII domain description if one exists, else NULL.
*
*/

char * sysAsciiErrorDomainGet
    (
    ERROR_ID id		/* Error ID in registration table */
    )
    {
    LOCAL char *retStr;
    LOCAL ERROR_INIT_ENTRY entry;

    retStr = "";
    if (sysErrorInitEntryFind (ERROR_DOMAIN_MASK,
			       (ERROR_DOMAIN_MASK & id), &entry) == OK)
	{
	retStr = (char *)&entry.asciiDomain;
	}

    return (retStr);
    }

/******************************************************************************
*
* errorRegisterAdd - Add an entry to the error registration table.
*
* This function will save an error registration entry in the error
* registration table.  It is a supporting function to sysErrorRegister.
*
* RETURNS: OK if room in table and entry was added, ERROR otherwise
*/

LOCAL STATUS errorRegisterAdd
    (
    ERROR_REGISTER_ENTRY entry		/* Registration entry to add */
    )
    {
    LOCAL int nextIndex = 0;

    if (nextIndex >= ERROR_REGISTER_SIZE)
	return (ERROR);
    else
	{
	errorRegistrations[nextIndex] = entry;
	nextIndex++;
	}
    return (OK);
    }

/******************************************************************************
*
* errorMsgLog - Place error message into the log.
*
* This routines places the requested error message into the log.
* It is a supporting function to sysErrorLog.
*
* RETURNS: NA
*/

LOCAL void errorMsgLog
    (
    INTERNAL_ERROR_MSG * errorMsg	/* Pointer to error message to log */
    )
    {
    struct _ERROR_LOG_ENTRY * p;
    struct _ERROR_LOG_ENTRY * r;

    if ((p = errorPoolEntryGet()) == NULL)
	{

	/* 
	 * No more empty queue entries left, remove the oldest logged
	 * entry and use it.
	 */

	logMsg ("errorMsgLog: Error log overflow\n\r",0,0,0,0,0,0);

	r = NULL;
	p = errorLogNext (r);
	r = p;
	p = errorLogEntryRemove (r);

	}

    /* We have an 'empty' logging cell, fill it and push it onto log queue */

    p->msg = *errorMsg;
    errorLogEntryPut (p);

    }

/******************************************************************************
*
* errorLogRawFormat - Format the error log for a "raw" display.
*
* This function constructs a raw, ASCII filtered display of the error
* log.  The filtering is controlled by mask and compare parameters
* such that if ((mask & tableEntryID) == compare), it is considered a
* match.  Only error table entries which are "matches" are included in
* the raw output of this function.  The ASCII strings are passed back
* to a call back routine, one line at a time.  After the complete
* display is passed to the call back function, one last invocation of
* the call back function is made with a NULL character pointer to mark
* the end of the ASCII data.
*
* RETURNS: NA
*/

LOCAL void errorLogRawFormat
    (
    ERROR_ID  mask,		/* AND mask to apply to table IDs */
    ERROR_ID  compare,		/* COMPARE value for masked match */
    FORMAT_CALLBACK callBack	/* Callback function to process each line */
    )
    {
    int i = 1;
    struct _ERROR_LOG_ENTRY *p;
    char stage[100];

    sprintf(stage,"\n\rErrors currently logged: sysErrorLogRawDisplay "
	          "(0x%x, 0x%x)\n\r\n\r", mask, compare);
    (* callBack) (stage);

    p = NULL;
    p = errorLogNext (p);

    if (ERROR_LOG_LIST_END(p))
	{
        (* callBack) ("\n\rNo errors currently logged\n\r");
	return;
	}
    
    (* callBack) ("No.  time     ID       p0       p1       p2       p3       "
	          "p4       p5\n\r");
    (* callBack) ("----------------------------------------------------------"
	          "------------------\n\r");

    while (p != NULL)
	{
	if ((p->msg.id & mask) == compare)
	    {
	    sprintf(stage,"%04d %08x %08x %08x %08x %08x %08x %08x %08x\n\r",
		          i, (UINT32)p->msg.time, p->msg.id, p->msg.params[0], 
			  p->msg.params[1], p->msg.params[2], p->msg.params[3],
			  p->msg.params[4], p->msg.params[5]);
	    (* callBack) (stage);
	    }
	i++;
	p = errorLogNext (p);
	}
    (* callBack)(NULL);
    }

/******************************************************************************
*
* errorLogFormat -  Format the error log for display.
*
* This function constructs a formatted, ASCII filtered display of the
* error log.  The filtering is controlled by mask and compare
* parameters such that if ((mask & tableEntryID) == compare), it is
* considered a match.  Only error table entries which are "matches"
* are included in the formatted output of this function.  The ASCII
* strings are passed back to a call back routine, one line at a time.
* After the complete display is passed to the call back function, one
* last invocation of the call back function is made with a NULL
* character pointer to mark the end of the ASCII data.
*
* RETURNS: NA
*/

LOCAL void errorLogFormat
    (
    ERROR_ID  mask,		/* AND mask to apply to table IDs */
    ERROR_ID  compare,		/* COMPARE value for masked match */
    FORMAT_CALLBACK callBack	/* Callback function to process each line */
    )
    {
    int i = 1;
    int index;
    struct _ERROR_LOG_ENTRY *p;
    INTERNAL_ERROR_MSG errorMsg;
    ERROR_INIT_ENTRY errorInitEntry;
    ERROR_REGISTER_ENTRY errRegEntry;
    char stage[100];

    sprintf(stage,"\n\rErrors currently logged: sysErrorLogDisplay "
	          "(0x%x, 0x%x)\n\r\n\r", mask, compare);
    (* callBack) (stage);

    p = NULL;
    p = errorLogNext (p);

    if (ERROR_LOG_LIST_END(p))
	{
        (* callBack) ("\n\rNo errors currently logged\n\r");
	return;
	}

    i = 1;

    while (!ERROR_LOG_LIST_END(p))
	{
	if ((p->msg.id & mask) == compare)
            {
	    if (sysErrorInitEntryFind (ERROR_DOMAIN_MASK, 
				       (p->msg.id & ERROR_DOMAIN_MASK), 
				       &errorInitEntry) == OK)
		{
	        index = ERROR_FIRST;
	        if (sysErrorRegisterEntryFind (&index, p->msg.id, p->msg.id, 
				               &errRegEntry) == OK)

		    {

        	    /* Call the display routine */

		    errorMsg = p->msg;
		    sprintf(stage, "%04d: ",i);
        	    errorEntryDisplay (stage, &errorInitEntry, &errRegEntry,
    			   	       &errorMsg, callBack);
		    }
	        }
	    }

	i++;
	p = errorLogNext (p);

	(* callBack) (NULL);
	}
    }

/******************************************************************************
*
* errorEntryDisplay - Display an error entry.
*
* This function takes an error initialization entry, error registry entry,
* and error log entry and displays the appropriate message to the console.
*
* RETURNS: NA
*/

LOCAL void errorEntryDisplay
    (
    char *prepend,			/* "Prepend string" for display */
    ERROR_INIT_ENTRY *initEntry,	/* Associated initialization entry */
    ERROR_REGISTER_ENTRY *regEntry,	/* Associated registration entry */
    INTERNAL_ERROR_MSG *errMsg,		/* Actual error message */
    FORMAT_CALLBACK callBack		/* Function to call for each string */
    )
    {
    char stage [200];
    char format [200];
    char *f = format;
    char *p;

#ifdef INCLUDE_RTC
    timeString (errMsg->time, format);
    sprintf(stage,"%s %s \"%-8s\" (%08x):", prepend, format,
	    initEntry->asciiDomain, errMsg->id);
#else
    format [0] = '\0';
    sprintf(stage,"%s %s \"%-8s\" (%08x):", prepend, format,
	    initEntry->asciiDomain, errMsg->id);
#endif /* INCLUDE_RTC */

    format[0] = '\0';
    if (initEntry->errorFormatFcn != NULL)
        if (initEntry->errorFormatFcn(errMsg->id) != NULL)
	    {
            strcat (f, "\n\r       ");
	    strcat (f, initEntry->errorFormatFcn(errMsg->id));
	    }

    p = &stage[strlen(stage)];
    sprintf(p, f, errMsg->params[0], errMsg->params[1], errMsg->params[2],
		  errMsg->params[3], errMsg->params[4], errMsg->params[5]);
    strcat(stage,"\n\r");

    (* callBack) (stage);
    }

/*****************************************************************************
*
* consoleDisplay - Callback function to display information on the console
*
* This function will display a string on the console via printf.
*
* RETURNS: NA
*/

LOCAL void consoleDisplay
    (
    char * dispString		/* ASCII string to display on console */
    )
    {
    if (dispString != NULL)
        printf("%s",dispString);
    }

/******************************************************************************
*
* _sysErrorLogRemove - Remove logged entries from the table
*
* This function will remove selected errors which are currently saved in
* the error logging table.  "Removed" entries are placed back onto the
* reserve pool.  This is a support function to sysErrorLogRemove.
*
* RETURNS: NA
*/

LOCAL void _sysErrorLogRemove 
    (
    ERROR_ID  mask,		/* AND mask to apply to table IDs */
    ERROR_ID  compare		/* COMPARE value for masked match */
    )
    {
    struct _ERROR_LOG_ENTRY * p;
    struct _ERROR_LOG_ENTRY * r;
    p = NULL; 
    p = errorLogNext (p);
    while (!ERROR_LOG_LIST_END(p))
	{
	if ((p->msg.id & mask) == compare)
	    {
	    r = errorLogEntryRemove (p);
	    errorPoolEntryPut (r);
	    }
	else
	    {
	    p = errorLogNext (p);
	    }
	}
    }

/*****************************************************************************
*
* errorPoolEntryPut - Put a logging entry back into the error pool
*
* This functions pushes an empty error logging entry on the the reserved 
* entry pool.
*
* RETURNS: NA
*/

LOCAL void errorPoolEntryPut 
    (
    struct _ERROR_LOG_ENTRY * p		/* Pointer to 'return' entry */
    )
    {
    p->next = errorReservedHead;
    errorReservedHead = p;
    errorReservedCount++;
    }

/*****************************************************************************
*
* errorPoolEntryGet - Get a logging entry from the reserved error pool.
*
* This function fetches an entry from the error reserved pool (removing it 
* from the pool), presumably to be used in holding a logging entry.
*
* RETURNS: pointer to logging entry if one is available, NULL otherwise.
*/

LOCAL struct _ERROR_LOG_ENTRY * errorPoolEntryGet (void)
    {
    struct _ERROR_LOG_ENTRY * p;
    if (errorReservedHead->next == NULL)
	p = NULL;
    else
	{
        p = errorReservedHead;
	errorReservedHead = p->next;
	errorReservedCount--;
	}
    return (p);
    }

/*****************************************************************************
*
* errorLogEntryPut - Put a logging entry back into the log list
*
* This functions puts an logging entry at the tail end of the error logging
* list.
*
* RETURNS: NA
*/

LOCAL void errorLogEntryPut 
    (
    struct _ERROR_LOG_ENTRY * p		/* Pointer to 'return' entry */
    )
    {
    *errorLogTail = *p;	    /* Copy entry to be added to trailing dummy cell */
    errorLogTail->next = p;
    errorLogTail = p;
    errorLogTail->next = NULL;
    errorLogCount++;
    }

/*****************************************************************************
*
* errorLogEntryRemove - Remove a logging entry from the error log pool
*
* This function removes an entry from the error logging pool.  It returns
* to the caller a pointer to a freed entry.
*
* RETURNS: Pointer to removed entry (can be returned to reserve pool).
*/

LOCAL struct _ERROR_LOG_ENTRY * errorLogEntryRemove
    (
    struct _ERROR_LOG_ENTRY * p		/* Pointer to entry to remove */
    )
    {
    struct _ERROR_LOG_ENTRY * r;	/* Return pointer */ 

    r = p->next;
    if (p->next == errorLogTail)
	errorLogTail = p;
    *p = *(p->next);
    if (errorLogTail == p)
	p->next = NULL;
    errorLogCount--;

    return(r);
    }

/*****************************************************************************
*
* errorLogNext - Returns 'next' error log entry
*
* This function returns the next entry in the error log list.
*
* RETURNS: Pointer to next entry on error log list, NULL if end of list 
*/

LOCAL struct _ERROR_LOG_ENTRY * errorLogNext
    (
    struct _ERROR_LOG_ENTRY * p	 	/* Pointer to current entry */ 
    )
    {
    struct _ERROR_LOG_ENTRY * r = NULL;

    if (p == NULL)
	{
	p = (errorLogHead);
	if (errorLogHead != errorLogTail)
	    r = p;
	}
    else
	{
	if (p != errorLogTail)
	    {
	    r = p->next;
	    if (r == errorLogTail)
		r = NULL;
	    }
	}

    return (r);
    }

/******************************************************************************
*
* errorTask - Handle logging and task registration
*
* The routine runs as a task.  It is spawned by sysErrorHandlerInit.
* It waits on a message queue, processing error logging and registration 
* requests as they are deposited into the queue.
*
* RETURNS: NA
*/

LOCAL void errorTask (void)
    {
    INTERNAL_ERROR_MSG errorMsg;	
    ERROR_INIT_ENTRY errorInitEntry;
    ERROR_REGISTER_ENTRY errRegEntry;

    for (;;)
        {
        if (msgQReceive (errorMsgQId, (char *) &errorMsg, sizeof (errorMsg),
	                 WAIT_FOREVER) != sizeof (errorMsg))
	    {
            logMsg("Error reading error message queue\n",0,0,0,0,0,0);
	    continue;
	    }
    
        switch (errorMsg.id)
            {
            case ERROR_UTIL_FCN_CALL_ID:

                /*
                 * Generic request for errorTask to call a function.
                 * Function pointer is in errorMsg.utility arguments
                 * are in errorMsg.params[0-5].
                 */

                (* (VOIDFUNCPTR *)&errorMsg.utility) (errorMsg.params[0],
                                                      errorMsg.params[1],
                                                      errorMsg.params[2],
                                                      errorMsg.params[3],
                                                      errorMsg.params[4],
                                                      errorMsg.params[5]);
                break;

            default:

                /*
                 * This is a real error logging request, note the the
                 * queueing function, sysErrorLog() has already taken
		 * care of dispositions 'ignore' and 'execute function'.
		 * Start by getting hold of the error registration entry.
		 * Also sysErrorLog() has found the error registration
		 * entry and passed it to us in the "utility" field of
		 * the error message.
                 */

		errRegEntry = sysErrorRegisterEntryGet (errorMsg.utility);

		/* 
		 * Now obtain the error initialization entry so we can
		 * get our hands on the formatting function.
		 */

    		if (errRegEntry.disposition & ERROR_DISP_IMMEDIATE_DISPLAY)
		    {
		    if (sysErrorInitEntryFind 
			                 (ERROR_DOMAIN_MASK, 
			                  (ERROR_DOMAIN_MASK & errorMsg.id),
			                  &errorInitEntry) == ERROR)
		        {

			/*
			 * Since sysErrorLog() has already found a registration
			 * entry for this error ID, we should also have an
			 * error initialization entry so we should never
			 * really get to this place in the code.  If we do
			 * things are really botched !!!
			 */

			logMsg("Uninitialized error ID = 0x%x\n",errorMsg.id,
			       0,0,0,0,0);
			continue;
			}

		    /* Call the display routine */

		    errorEntryDisplay ("\n\rErrLog: ", &errorInitEntry, 
				       &errRegEntry, &errorMsg, consoleDisplay);

		    /* Always terminate display with NULL pointer */

		    consoleDisplay (NULL);
		    }

		/* If disposition includes "save" then do it !!! */

    		if (errRegEntry.disposition & ERROR_DISP_SAVE)
		    {
                    errorMsgLog (&errorMsg);
		    }
            }
        }
    }

#ifdef INCLUDE_RTC
/*****************************************************************************
*
* timeString - Format a time tag string
*
* This function will format a time tag string to use for display with a
* formatted error log message.
*
* RETURNS: NA
*/

LOCAL void timeString
    (
    time_t timeInSecs,
    char * retStr
    )
    {
    char scratch[30];
    char timeStr[20];

    scratch[0] = '\0';
    strncpy (scratch, ctime (&timeInSecs), 24);
    timeStr[0] = '\0';
    strncpy(timeStr, &scratch[4],15);
    timeStr[15] = '\0';
    strcpy (retStr, timeStr);
    }
#endif /* INCLUDE_RTC */
