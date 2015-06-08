/* evtLogLibP.h - event log header */

/* Copyright 1994-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01k,09may02,pcm  added preceding underscore to ASMLANGUAGE
01j,18oct01,tcr  update for VxWorks 5.5
01i,17feb98,pr   changed prototype for evtLogO and evtLogOInt.
01h,02feb98,nps  Allow inclusion in (68k) assembler source.
01g,27jan98 cth  added BEGIN, CONFIG, BUFFER, END sizes, added function 
		 declarations, updated copyright
01f,31jul97 nps  WindView 2.0 -
                 EVTLOG_STR_SIZE was incorrectly defined as 18, should be 22.
01e,14dec94 rdc  mods for am29k.
01d,28nov94 rdc  mods for x86.
01c,05may94,smb  corrected macro name EVTLOG_T1_SIZE
01b,12jan94,c_s  made EVTLOG_*_SIZE constants architecture-specific; commented
           +maf    them, and added definition of MAX_KERNEL_EVENT_SIZE.
	           SPR #2840.
01a,10dec93,smb  written.
*/

#ifndef __INCevtloglibph
#define __INCevtloglibph

#ifndef _ASMLANGUAGE
#ifdef __cplusplus
extern "C" {
#endif

#include "private/wvBufferP.h"

#endif	/* !_ASMLANGUAGE */

/* defines */

#define MAX_WV_TASKS 500        /* should be the same MAX_DSP_TASKS */

/* sizes of some common events. */

#define EVENT_BEGIN_SIZE   (sizeof(short) + (5 * sizeof(int)))
#define EVENT_END_SIZE     (sizeof (short))
#define EVENT_CONFIG_SIZE  (sizeof (short) + (7 * sizeof(int)))
#define EVENT_BUFFER_SIZE  (sizeof(short) + sizeof(int))
#define EVENT_COMMENT_SIZE (sizeof(short) + sizeof(int))



#if	(CPU_FAMILY==MC680X0 || CPU_FAMILY==I80X86 || CPU_FAMILY==AM29XXX)

/*
 * These constants represent the sizes of various categories of events
 * in the 68k family instrumented kernel, for use by assembly routines.
 */

#define EVTLOG_T0_SIZE		 6	/* timestamped; no parameters	*/
#define EVTLOG_T1_SIZE		10	/* timestamped; one parameter	*/

#define EVTLOG_M0_SIZE		 2	/* not timestamped; no params	*/
#define EVTLOG_M1_SIZE		 6	/* not timestamped; one param   */
#define EVTLOG_M2_SIZE		10	/* not timestamped; two params  */
#define EVTLOG_M3_SIZE		14	/* not timestamped; three params*/

#define EVTLOG_O_SIZE		26	/* timestamped; up to six params*/

#define EVTLOG_STR_SIZE		22	/* base size of initial task evt*/
#define EVTLOG_PT_SIZE		14	/* eventpoint	 		*/
#define EVTDISPATCH_SIZE	14	/* (max) size of "dispatch"	*/
#define EVTIDLE_SIZE		 6	/* idle				*/

/* 
 * The MAX_KERNEL_EVENT_SIZE is the size of the largest event that will 
 * be placed in the buffer after a call to scrPadToBuffer.
 */

#define MAX_KERNEL_EVENT_SIZE	EVTDISPATCH_SIZE

#else	/* CPU_FAMILY==MC680X0 || CPU_FAMILY==I80X86 || CPU_FAMILY==AM29XXX */

/* XXX
#error You must recompute the event size macros for a new architecture.
 for the moment 
 */

/*
 * These constants represent the sizes of various categories of events
 * in the 68k family instrumented kernel, for use by assembly routines.
 */

#define EVTLOG_T0_SIZE           6      /* timestamped; no parameters   */
#define EVTLOG_T1_SIZE          10      /* timestamped; one parameter   */

#define EVTLOG_M0_SIZE           2      /* not timestamped; no params   */
#define EVTLOG_M1_SIZE           6      /* not timestamped; one param   */
#define EVTLOG_M2_SIZE          10      /* not timestamped; two params  */
#define EVTLOG_M3_SIZE          14      /* not timestamped; three params*/

#define EVTLOG_O_SIZE           26      /* timestamped; up to six params*/

#define EVTLOG_STR_SIZE         22      /* base size of initial task evt*/
#define EVTLOG_PT_SIZE          14      /* eventpoint                   */
#define EVTDISPATCH_SIZE        14      /* (max) size of "dispatch"     */
#define EVTIDLE_SIZE             6      /* idle                         */

#define MAX_KERNEL_EVENT_SIZE	EVTDISPATCH_SIZE

#endif	/* CPU_FAMILY==MC680X0 || CPU_FAMILY==I80X86 || CPU_FAMILY==AM29XXX */


#ifndef	_ASMLANGUAGE

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void evtObjLogFuncBind (void);
extern void evtLogFuncBind (void);
extern void evtBufferBind (BUFFER_ID bufId);
extern void evtHeaderLog (void);
extern void evtLogTasks (void);
extern void evtLogOInt (event_t action, int nParam, int arg1, int arg2, 
			int arg3, int arg4, int arg5);
extern void evtLogO    (event_t action, int nParam, int arg1, int arg2, 
			int arg3, int arg4, int arg5);
extern int evtLogPoint (event_t action, void *addr, size_t nbytes, char *buf);

#else   /* __STDC__ */

extern void evtObjLogFuncBind ();
extern void evtLogFuncBind ();
extern void evtBufferBind ();
extern void evtHeaderLog ();
extern void evtLogTasks ();
extern void evtLogOInt ();
extern void evtLogO ();
extern int evtLogPoint ();

#endif  /* __STDC__ */


#ifdef __cplusplus
}
#endif
#endif	/* !_ASMLANGUAGE */

#endif /* __INCevtloglibph*/

