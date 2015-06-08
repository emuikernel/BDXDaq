/* configNet.h - network configuration header */

/* Copyright 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,03sep01,hbh  written based on ads860.
*/
 
#ifndef INCconfigNeth
#define INCconfigNeth

/* includes */

#include "vxWorks.h"
#include "end.h"
#include "config.h"

/* defines */
 
/* max number of SENS ipAttachments we can have */
 
#ifndef IP_MAX_UNITS
#   define IP_MAX_UNITS (NELEMENTS (endDevTbl) - 1)
#endif
 
END_TBL_ENTRY endDevTbl [] =
{
    { 0, END_TBL_END, NULL, 0, NULL, FALSE},
};

#endif /* INCconfigNeth */
