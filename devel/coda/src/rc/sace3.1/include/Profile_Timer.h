/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Profile_Timer.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_PROFILE_TIMER_H)
#define ACE_PROFILE_TIMER_H

#include "sysincludes.h"

#if defined (ACE_HAS_PRUSAGE_T)

class Profile_Timer
  // = TITLE
  //     An interval timer class 
  //
  // = DESCRIPTION
  //
{
public:
  struct Elapsed_Time
    {
      double real_time;
      double user_time;
      double system_time;
    };

  Profile_Timer (void);
  ~Profile_Timer (void);
  int  start (void);
  int  stop (void);
  int  elapsed_time (Elapsed_Time &et);
  void elapsed_rusage (prusage_t &rusage);
  void get_rusage (prusage_t &rusage);

private:
  void subtract (timestruc_t &tdiff, timestruc_t &t0, timestruc_t &t1);
  void compute_times (Elapsed_Time &et, prusage_t &, prusage_t &);

  prusage_t begin_usage_;
  prusage_t end_usage_;
  prusage_t last_usage_;
  int	    proc_handle_;
};

#include "Profile_Timer.i"
#endif /* ACE_HAS_PRUSAGE_T */

#endif /* ACE_PROFILE_TIMER_H */

