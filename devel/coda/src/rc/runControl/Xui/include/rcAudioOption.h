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
//      RunControl Preference Audio Output Option
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcAudioOption.h,v $
//   Revision 1.2  1996/12/04 18:32:25  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#ifndef _RC_AUDIO_OPTION_H
#define _RC_AUDIO_OPTION_H

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)

#include <rcMenuTogComd.h>

class rcAudioOption: public rcMenuTogComd
{
public:
  // constructor and destructor
  rcAudioOption (char* name, int active,
		 char* acc, char* acc_text,
		 int state,
		 rcClientHandler& handler);
  ~rcAudioOption (void);

  // class name
  virtual const char* className (void) const {return "rcAudioOption";}

protected:
  // inherited operations
  virtual void doit    (void);
  virtual void undoit  (void);
};


#endif  /* coda_2_0 */

#endif




  
