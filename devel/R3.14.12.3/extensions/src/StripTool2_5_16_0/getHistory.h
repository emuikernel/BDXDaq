/*************************************************************************\
* Copyright (c) 1994-2004 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 1997-2003 Southeastern Universities Research Association,
* as Operator of Thomas Jefferson National Accelerator Facility.
* Copyright (c) 1997-2002 Deutches Elektronen-Synchrotron in der Helmholtz-
* Gemelnschaft (DESY).
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

#ifndef _getHistory_h
#define _getHistory_h

/* StripHistoryInfo
 *
 *      Contains instance data for the archive service.
 */
typedef struct _StripHistoryInfo
{
  Strip         strip;
  char          *archiverInfo;
}
StripHistoryInfo;


unsigned long getHistory(StripHistory     the_shi,
                  char                   *name,
		  struct timeval         *begin,  
		  struct timeval         *end,
		  struct timeval        **times,
		  short                 **status,
		  double                **data,
		  unsigned long          *count);
#endif  /* _getHistory_h */
