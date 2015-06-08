h07301
s 00099/00000/00000
d D 1.1 07/10/29 00:27:42 boiarino 1 0
c date and time created 07/10/29 00:27:42 by boiarino
e
u
U
f e 0
t
T
I 1
/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* Copyright (c) 2002 Berliner Elektronenspeicherringgesellschaft fuer
*     Synchrotronstrahlung.
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution. 
\*************************************************************************/
/* 
 *  tool_lib.h,v 1.1.2.6 2004/10/19 15:17:03 lange Exp
 *
 *  Author: Ralph Lange (BESSY)
 *
 */

#ifndef INCLtool_libh
#define INCLtool_libh

#include <epicsTime.h>

/* Convert status and severity to strings */
#define stat_to_str(stat)                                       \
        ((stat) >= 0 && (stat) <= (signed)lastEpicsAlarmCond) ? \
        epicsAlarmConditionStrings[stat] : "??"

#define sevr_to_str(stat)                                       \
        ((stat) >= 0 && (stat) <= (signed)lastEpicsAlarmSev) ?  \
        epicsAlarmSeverityStrings[stat] : "??"

#define stat_to_str_unsigned(stat)              \
        ((stat) <= lastEpicsAlarmCond) ?        \
        epicsAlarmConditionStrings[stat] : "??"

#define sevr_to_str_unsigned(stat)              \
        ((stat) <= lastEpicsAlarmSev) ?         \
        epicsAlarmSeverityStrings[stat] : "??"

/* The different versions are necessary because stat and sevr are
 * defined unsigned in CA's DBR_STSACK structure and signed in all the
 * others. Some compilers generate warnings if you check an unsigned
 * being >=0 */


#define CA_PRIORITY 50          /* CA priority */
#define DEFAULT_TIMEOUT 1.0     /* Default CA timeout */


/* Type of timestamp */
typedef enum { absolute, relative, incremental, incrementalByChan } TimeT;

/* Output formats for integer data types */
typedef enum { dec_, bin_, oct_, hex_ } IntFormatT;

/* Structure representing one PV (= channel) */
typedef struct 
{
    char *name;
    chid  ch_id;
    long  dbfType;
    long  dbrType;
    unsigned long nElems;
    unsigned long reqElems;
    int status;
    void *value;
    epicsTimeStamp tsPrevious;
    char firstStampPrinted;
    char onceConnected;

/*sergey: if we have an array of channels, number them starting from 0*/
    int index;

} pv;


extern TimeT tsType;        /* Timestamp type flag (-r -i -I options) */
extern IntFormatT outType;  /* Flag used for -0.. output format option */
extern int enumAsNr;        /* Used for -n option (get DBF_ENUM as number) */
extern double caTimeout;    /* Wait time default (see -w option) */
extern char dblFormatStr[]; /* Format string to print doubles (see -e -f option) */

#ifdef  __cplusplus
extern "C" {
#endif
  /*extern*/ char *val2str(const void *v, unsigned type, int index, float *valfloat);
  /*extern*/ char *dbr2str(const void *value, unsigned type);
  /*extern*/ char *print_time_val_sts(pv *pv, unsigned long nElems, float *valfloat);
  /*extern*/ int  create_pvs(pv *pvs, int nPvs, caCh *pCB );
  /*extern*/ int  connect_pvs(pv *pvs, int nPvs );
#ifdef  __cplusplus
}
#endif

/*
 * no additions below this endif
 */
#endif /* ifndef INCLtool_libh */
E 1
