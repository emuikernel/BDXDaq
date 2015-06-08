/* -*- C++ -*- */
/* An interval timer class */

inline int 
Profile_Timer::start (void) 
{
  return ::ioctl (this->proc_handle_, PIOCUSAGE, &this->begin_usage_);
}

inline int 
Profile_Timer::stop (void) 
{
  this->last_usage_ = this->end_usage_;
  return ::ioctl (this->proc_handle_, PIOCUSAGE, &this->end_usage_);
}

