#ifndef _RORC_DATE_H_
#define _RORC_DATE_H_

/*
 ***************************************************
 * rorc_date.h
 *
 * Header file for ALICE RORC programs used in DATE
 *
 * last updated: 25/07/2007
 *
 * 15/08/2007: change sprintf and strcat to snprintf and strncat
 * 25/07/2007: simplification of log call by calling infoLog_f
 * 15/03/2007: change LOG_TO call to LOG
 * 24/10/2006: add quiet option to date_log_message definition
 * 04/09/2006: change LOG_NORMAL_TH to LOG_DETAILED_TH
 * 09/05/2003: original version
 *
 * written by: Peter Csato and Ervin Denes
 ***************************************************
 */

/* info logger */

#ifdef DATE_LOG   //DATE_LOG
  #define LOG_FACILITY "DDL/RORC"
  #include "infoLogger.h"
  #define date_log(message...) infoLog_f(LOG_FACILITY,LOG_INFO,message)
#else
  #ifdef QUIET
    #define date_log(message...)
  #else
    #define date_log(message...) ( printf(message), printf("\n") )
  #endif
#endif

#define MAX_STAT 4096
#define stat_log(message...) (snprintf (statLine, 128, message), strncat(statInfo, statLine, MAX_STAT-1 - strlen(statInfo)))
char statLine[128], statInfo[MAX_STAT];

#endif /* _RORC_DATE_H_ */

