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
//      CODA RunControl Simple Audio Output
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcAudioOutput.h,v $
//   Revision 1.3  1997/07/11 14:07:52  heyes
//   almost working
//
//   Revision 1.2  1996/12/04 18:32:25  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#ifndef _RC_AUDIO_OUTPUT_H
#define _RC_AUDIO_OUTPUT_H

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <rcComdOption.h>

#ifdef _CODA_USE_THREADS
#include <pthread.h>

inline void* sound (void *arg)
{
  char* script = (char *)arg;
  system (script);
  delete []script;
  return 0;
}

inline void rcAudio (char* s)
{
  rcComdOption* option = rcComdOption::option ();

  if (option->audio ()) {
    char *temp = new char[128];
    sprintf (temp, "say -g 0.8 %s", s);
    pthread_t thr;
    
    pthread_attr_t attr;
    //::pthread_attr_init (&attr);
    //::pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
    //::pthread_create (&thr, &attr, sound, (void *)temp);
  }
}
#else
inline void rcAudio (char* s)
{
  rcComdOption* option = rcComdOption::option ();

  if (option->audio ()) {
    char temp[128];
    sprintf (temp, "say -g 0.8 %s", s);
    system (temp);
  }
}
#endif

#else 
inline void* sound (void *arg)
{
  // empty
  return 0;
}

inline void rcAudio (char* s)
{
  // empty
}
#endif  /* coda_2_0 */

#endif
  
