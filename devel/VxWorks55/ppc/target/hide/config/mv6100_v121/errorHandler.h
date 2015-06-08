/* errorHandler.h - Error handling suite */

/* Copyright 2004, Motorola Inc. All Rights Reserved */

/*
modification history
--------------------
01a,03aug04,scb Initial writing.
*/

/*
This file contains the definitions which provide the glue between
modules which detect errors, register and log errors and the generic 
handling module.
*/

#ifndef __INCerrorHandlerh
#define __INCerrorHandlerh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* Sizes of various items */

#define MAX_ERROR_Q_MSGS      (10)  /* Max number of messages in error queue */
#define ERROR_ASCII_TYPE      (8)   /* Size of ASCII domain display string */
#define ERROR_INIT_TABLE_SIZE (20)  /* Number of domains that can initialize */
#define ERROR_REGISTER_SIZE   (100) /* Max # of distinct error codes */
#define ERROR_LOG_TABLE_SIZE  (200) /* Max # of errors that can be logged */

/* Error logging disposition codes (for error registering) */

#define ERROR_DISP_IGNORE 		(0)
#define ERROR_DISP_FUNCTION_CALL	(1<<0)
#define ERROR_DISP_IMMEDIATE_DISPLAY	(1<<1)
#define ERROR_DISP_SAVE			(1<<2)

#define ERROR_DISP_DEFAULT (ERROR_DISP_IMMEDIATE_DISPLAY | ERROR_DISP_SAVE)

/*
 * Define "generic" error ID types for the error logging domains to use
 * for error initialization.
 */

#define ERROR_CPU_TYPE		(0x01000000)
#define ERROR_HOST_BRIDGE_TYPE	(0x02000000)
#define ERROR_PCI_T0_VME_TYPE	(0x03000000)
#define ERROR_GENERIC_PCI_TYPE  (0x04000000)
#define ERROR_USER_TYPE		(0x0f000000)

#define ERROR_DOMAIN_MASK	(0xff000000)

/*
 * Define "utility" error ID's  - these are IDs which don't represent real
 * errors but convey information to cause the error Task to perform a
 * special utility function.  "Utility" error ID's are distinguished from
 * real error logging ID's by the high order (top bit) being set.
 */

#define ERROR_UTIL_FCN_CALL_ID	 (0x80000001)	/* void function call */
#define ERROR_UTIL_ID_MASK	 (0x80000000)


#ifndef _ASMLANGUAGE

/* Error related data types */

typedef UINT32  ERROR_ID;
typedef UINT32  ERROR_PARAM;
typedef UINT32  ERROR_PARAMS [6];
typedef UINT32  ERROR_DISPOSITION;

/*
 * Application specific error handling helper function pointers.  The
 * functions represented by these pointers reside in the applicaton 
 * code and the associated functions are called by the error handing
 * utility code.
 */

typedef char * (* ERROR_FMT_FUNCPTR)   ( ERROR_ID id );
typedef BOOL   (* ERROR_SCAN_FUNCPTR)  ( ERROR_ID id );
typedef BOOL   (* ERROR_CLEAR_FUNCPTR) ( ERROR_ID id );
typedef void   (* ERROR_EXC_FUNC) (int param);  /* Function call disposition */

/*
 * Passed by application code calling error handler to the error handling
 * utility module during initialization.  It advertises the applications's
 * "helper" functions used for error string formatting, error scanning 
 * and error clearing.
 */

typedef struct
    {
    ERROR_FMT_FUNCPTR     myErrorFormat;
    ERROR_SCAN_FUNCPTR    myErrorScan;
    ERROR_CLEAR_FUNCPTR   myErrorClear;
    UINT32		  reserved;
    } ERROR_USR_FUNCS;

/*
 * Data format for logging an error.  This is the parameter that the
 * application passes to the error logging function.
 */

typedef struct
    {
    ERROR_ID	 id;		/* Filled in by application task */
    ERROR_PARAMS params;	/* Filled in by application task */
    } ERROR_MSG;

/*
 * Error utility specific error handling function pointers.  The functions
 * represented by these pointers are called by application code to
 * "register" an error and for logging errors.
 */ 

typedef STATUS (* ERROR_REGISTER_FUNCPTR)    ( ERROR_ID id,
					       ERROR_DISPOSITION disposition,
					       ERROR_EXC_FUNC errorFunc,
					       ERROR_PARAM errorParam );

typedef STATUS (* ERROR_LOG_FUNCPTR) ( ERROR_MSG *msg );

/*
 * Passed error handler initilization back to the application code during
 * error handling initialization.  It advertises the error handling
 * modules, error registration and error logging functions.
 */

typedef struct 
    {
    ERROR_REGISTER_FUNCPTR errorRegister;
    ERROR_LOG_FUNCPTR	   errorLog;
    } ERROR_UTILS;

/*
 * Error handler utility function pointer, called by application to
 * initialize error handling.
 */

typedef ERROR_UTILS (* ERROR_HANDLER_INIT) ( ERROR_ID id, 
				             char * asciiDomain,
                                             ERROR_USR_FUNCS * errorFuncs );

/*
 * Pointer to function which returns formatted string - used by show
 * routines.
 */

typedef void    (* FORMAT_CALLBACK) (char * formattedStr);

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* __INCerrorhandlerh */
