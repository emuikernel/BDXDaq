/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    High_Res_Timer.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_HIGH_RES_TIMER_H)
#define ACE_HIGH_RES_TIMER_H

#include "sysincludes.h"

#if defined (ACE_HAS_HI_RES_TIMER)

class High_Res_Timer
  // = TITLE
  //     A high resolution timer class 
  //
  // = DESCRIPTION
  //
{
public:
  High_Res_Timer (void);
  void reset (void);

  void start (void);
  void stop (void);

  void start_incr (void);
  void stop_incr (void);
  void print_total (char *, int, int); 
  void print_ave (char *, int, int); 

private:
  hrtime_t start_;
  hrtime_t end_;
  hrtime_t total_;
  hrtime_t temp_;
};

#include "High_Res_Timer.i"
#endif /* ACE_HAS_HI_RES_TIMER */
#endif /* _HIGH_RES_TIMER_H */

