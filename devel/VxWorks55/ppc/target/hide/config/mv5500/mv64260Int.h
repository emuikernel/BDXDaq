/* mv64260Int.h - Mv64260 interrupt support */

/* Copyright 2002 Motorola, Inc. */

/*
modification history
--------------------
01b,06nov02,scb Moved INT_UTIL_EXTRA to <board.h> file.
01a,01oct02,scb Copied from hxeb100 base (ver 01a).
*/

#ifndef INCmv64260Inth
#define INCmv64260Inth

#ifdef __cplusplus
    extern "C" {
#endif

/*
 * Template for the 256 entry interrupt handler table.  An entry is
 * filled in for each intConnect().  Index into table is vector
 * number which is numerically equivalent to interrupt number.
 */

typedef struct intHandlerDesc           /* interrupt handler description */
    {
    VOIDFUNCPTR                 vec;    /* interrupt vector */
    int                         arg;    /* interrupt handler argument */
    struct intHandlerDesc *     next;   /* next interrupt handler & argument */
    } INT_HANDLER_DESC;

/*
 * Entry which defines interrupt number range and associated intEnable
 * and intDisable functions which cover the interrupt numbers within
 * the specified range.
 */

typedef struct intUtilDesc
    {
    UINT32		loIntNum;	        /* lowest interrupt number */
    UINT32		hiIntNum;	        /* highest interrupt number */
    int			(*intEnableFunc)(int);  /* assoc. intEnable fcn. */
    int			(*intDisableFunc)(int); /* assoc. intDisable fnc. */
    } INT_UTIL_DESC;

/* Utility function #defines for intUtil [] */

#define INT_UTIL_ICI ICI_MICL_INT_NUM_MIN,ICI_MICH_INT_NUM_MAX, \
			iciIntEnable,iciIntDisable

#define INT_UTIL_GPP GPP_IC_INT_NUM_MIN,GPP_IC_INT_NUM_MAX, \
			gppIntEnable,gppIntDisable

#ifdef __cplusplus
    }
#endif

#endif /* INCmv64260Inth */
