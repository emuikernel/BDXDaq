/* -*- C++ -*- */
/* A high resolution timer class */

#include "High_Res_Timer.h"
#if defined (ACE_HAS_HI_RES_TIMER)

void 
High_Res_Timer::reset (void)
{
  (void) ::memset (&this->start_, 0, sizeof this->start_);
  (void) ::memset (&this->end_, 0, sizeof this->end_);
  (void) ::memset (&this->total_, 0, sizeof this->total_);
  (void) ::memset (&this->temp_, 0, sizeof this->temp_);
}

High_Res_Timer::High_Res_Timer (void)
{
  this->reset ();
}

void 
High_Res_Timer::print_ave (char *str, int count, int fd)
{
#if defined (ACE_HAS_LONGLONG_T)
  hrtime_t total       = this->end_ - this->start_;
  hrtime_t avg_nsecs   = total / count;
  hrtime_t total_secs  = total / (1000 * 1000 * 1000);
  hrtime_t extra_nsecs = total - (total_secs * (1000 * 1000 * 1000));
  char	   buf[100];

  ::sprintf (buf, "%s count = %d, total (secs %lld, usecs %lld), avg usecs = %lld\n",
             str, count, total_secs, extra_nsecs / 1000, avg_nsecs / 1000);
  ::write (fd, buf, strlen (buf));
#endif /* ACE_HAS_LONGLONG_T */
}

void 
High_Res_Timer::print_total (char *str, int count, int fd)
{
#if defined (ACE_HAS_LONGLONG_T)
  hrtime_t avg_nsecs   = this->total_ / count;
  hrtime_t total_secs  = this->total_ / (1000 * 1000 * 1000);
  hrtime_t extra_nsecs = this->total_ - (total_secs * (1000 * 1000 * 1000));
  char buf[100];

  ::sprintf (buf, "%s count = %d, total (secs %lld, usecs %lld), avg usecs = %lld\n",
             str, count, total_secs, extra_nsecs / 1000, avg_nsecs / 1000);
  ::write (fd, buf, strlen (buf));
#endif /* ACE_HAS_LONGLONG_T */
}
#endif /* ACE_HAS_HI_RES_TIMER */
