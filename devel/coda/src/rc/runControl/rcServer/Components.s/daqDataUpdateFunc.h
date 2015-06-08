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
//      DAQ Data Updater Functions
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqDataUpdateFunc.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:19  chen
//   run control source
//
//
#ifndef _CODA_DAQDATA_UPDATEFUNC_H
#define _CODA_DAQDATA_UPDATEFUNC_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

extern void serverTime (char* compname,
			char* attrname, 
			void* buffer,
			int   count);

extern void compAttr   (char* compname,
			char* attrname,
			void* buffer,
			int   count);

extern void serverTimeBin (char* compname,
			   char* attrname,
			   void* buffer,
			   int   count);
#endif

