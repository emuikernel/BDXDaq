/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Time_Value.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_TIME_VALUE_H)
#define ACE_TIME_VALUE_H

#include "sysincludes.h"

class Time_Value
  // = TITLE
  //     Operations on timeval structures. 
  //
  // = DESCRIPTION
  //
{
public:
  static const Time_Value zero;
  static const Time_Value *zerop;

  Time_Value (long sec = 0, long usec = 0);
  Time_Value (const timeval &t);
  Time_Value (const Time_Value &tv);

  void set (const timeval &t);
  long sec (void) const;
  void sec (long sec);
  long usec (void) const;
  void usec (long usec);
  operator timeval () const;

#if defined (ACE_HAS_POSIX_TIMERS)
  Time_Value (const timestruc_t &t);
  void set (const timestruc_t &t);
  operator timestruc_t () const;
#endif /* ACE_HAS_POSIX_TIMERS */

  friend Time_Value operator + (Time_Value tv1, Time_Value tv2);
  friend Time_Value operator - (Time_Value tv1, Time_Value tv2);
  friend int	    operator < (Time_Value tv1, Time_Value tv2);
  friend int	    operator > (Time_Value tv1, Time_Value tv2);  
  friend int	    operator <= (Time_Value tv1, Time_Value tv2);
  friend int	    operator >= (Time_Value tv1, Time_Value tv2);  
  friend int	    operator == (Time_Value tv1, Time_Value tv2);  
  friend int	    operator != (Time_Value tv1, Time_Value tv2);  

private:
  void normalize (void);

  long tv_sec_;
  long tv_usec_;
};

#if defined (__INLINE__)
#define INLINE inline
#include "Time_Value.i"
#else
#define INLINE
#endif /* __INLINE__ */
#endif /* TIME_VALUE */
