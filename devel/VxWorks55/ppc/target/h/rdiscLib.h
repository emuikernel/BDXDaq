/* rdiscLib.h - support for router discovery RFC 1256 */

/* Copyright 2000 - 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,29mar01,spm  file creation: copied from version 01b of tor2_0.open_stack
                 branch (wpwr VOB) for unified code base
*/

#ifndef __INCrdiscLibh
#define __INCrdiscLibh

#ifdef __cplusplus
extern "C" {
#endif

/* cmd-types for the rdCtl-function */

#define SET_ADVERT_LIFETIME	0
#define SET_MIN_ADVERT_INT	1
#define SET_MAX_ADVERT_INT	2
#define SET_MODE	        3
#define SET_FLAG		4
#define SET_ADVERT_PREF		5
#define SET_ADVERT_ADDRESS	6

#define GET_FLAG		7
#define GET_ADVERT_LIFETIME	8
#define GET_ADVERT_PREF		9
#define GET_ADVERT_ADDRESS	10
#define GET_MIN_ADVERT_INT	11
#define GET_MAX_ADVERT_INT	12

/* Modes for the rdCtl routine, valid if cmd=SET_MODE. */
#define MODE_DEBUG_OFF          0    /* Turn debugging off. */
#define MODE_DEBUG_ON           1    /* Turn debugging on. */
#define MODE_STOP               2    /* Stop rdisc */

extern void rdiscLibInit (int priority, int options, int stackSize);
extern void rdisc ();
extern STATUS rdiscIfReset ();
/* value may be an int (set-cmds) or an int* (get-cmds) */
extern STATUS rdCtl (char *ifName, int cmd, void* value);

#ifdef __cplusplus
}
#endif

#endif /* __INCrdiscLibh */
