/*
//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      CODA constants
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaConst.h,v $
//   Revision 1.2  1996/12/04 18:32:19  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
//
*/
#ifndef _CODA_CONST_H
#define _CODA_CONST_H

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
#define CODA_VERSION             "Version 2.0"
#else
#define CODA_VERSION             "Version 1.4"
#endif

#define CODA_FATAL               -2
#define CODA_ERROR               -1
#define CODA_SUCCESS              0
#define CODA_WARNING              1
#define CODA_ACCESS_VIOLATION     2
#define CODA_IGNORED              3
#define CODA_INVALID_CALL         4

#define CODA_ISMASTER             0x1000
#define CODA_NOMASTER             0x1001
#define CODA_LOCKED               0x1002

#endif
