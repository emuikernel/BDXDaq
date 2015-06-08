inline void 
High_Res_Timer::start (void)
{
  this->start_ = ::gethrtime ();
}

inline void 
High_Res_Timer::stop (void)
{
  this->end_ = ::gethrtime ();
}

inline void 
High_Res_Timer::start_incr (void)
{
  this->temp_ = ::gethrtime ();
}

inline void 
High_Res_Timer::stop_incr (void)
{
#if defined (ACE_HAS_LONGLONG_T)
  this->total_ += (::gethrtime () - this->temp_);
#endif /* ACE_HAS_LONGLONG_T */
}

