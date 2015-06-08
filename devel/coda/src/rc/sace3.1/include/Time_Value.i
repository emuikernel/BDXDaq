/* -*- C++ -*- */

const long ONE_SECOND = 1000000L;

void
Time_Value::normalize (void)
{
  while ((this->tv_usec_ >= ONE_SECOND) 
	 || (this->tv_sec_ < 0 && this->tv_usec_ > 0 ))
    {
      this->tv_usec_ -= ONE_SECOND;
      this->tv_sec_++;
    }

  while ((this->tv_usec_ <= -ONE_SECOND) 
	 || (this->tv_sec_ > 0 && this->tv_usec_ < 0))
    {
      this->tv_usec_ += ONE_SECOND;
      this->tv_sec_--;
    }
}

/* Initializes the Time_Value object. */

INLINE
Time_Value::Time_Value (long sec, long usec)
		       : tv_sec_ (sec), tv_usec_ (usec)
{
  this->normalize ();
}

#if defined (ACE_HAS_POSIX_TIMERS)
/* Initializes a timestruc_t.  Note that this
   approach loses precision since it converts the
   nano-seconds into micro-seconds.  But then again, 
   how many systems have nano-second timer 
   precision anyway?! */

INLINE void
Time_Value::set (const timestruc_t &tv)
{
  this->tv_sec_ = tv.tv_sec;
  this->tv_usec_ = tv.tv_nsec / 1000;

  this->normalize ();
}

/* Initializes the Time_Value object from a timestruc_t. */

INLINE
Time_Value::Time_Value (const timestruc_t &tv)
{
  this->set (tv);
}

/* Returns the value of the object as a timestruc_t. */

INLINE 
Time_Value::operator timestruc_t () const
{
  timestruc_t tv;
  tv.tv_sec = this->tv_sec_;
  tv.tv_nsec = this->tv_usec_ * 1000;
  return tv;
}

#endif /* ACE_HAS_POSIX_TIMERS */

/* Initializes the Time_Value object from a timeval. */

INLINE
Time_Value::Time_Value (const timeval &tv)
{
  this->set (tv);
}

INLINE void
Time_Value::set (const timeval &tv)
{
  this->tv_sec_ = tv.tv_sec;
  this->tv_usec_ = tv.tv_usec;

  this->normalize ();
}

/* Initializes the Time_Value object from another Time_Value */

INLINE
Time_Value::Time_Value (const Time_Value &tv)
		       : tv_sec_ (tv.tv_sec_), tv_usec_ (tv.tv_usec_)
{
}

/* Returns number of seconds. */

INLINE long 
Time_Value::sec (void) const
{
  return this->tv_sec_;
}

/* Sets the number of seconds. */

INLINE void 
Time_Value::sec (long sec) 
{
  this->tv_sec_ = sec;
}

/* Returns number of micro-seconds. */

INLINE long 
Time_Value::usec (void) const
{
  return this->tv_usec_;
}

/* Sets the number of micro-seconds. */

INLINE void 
Time_Value::usec (long usec) 
{
  this->tv_usec_ = usec;
}

/* Returns the value of the object as a timeval. */

INLINE 
Time_Value::operator timeval () const
{
  timeval tv;
  tv.tv_sec = this->tv_sec_;
  tv.tv_usec = this->tv_usec_;
  return tv;
}

/* Adds two Time_Value objects together, returns the sum. */

INLINE Time_Value 
operator + (Time_Value src1, Time_Value src2)
{
  Time_Value sum (src1.tv_sec_ + src2.tv_sec_, src1.tv_usec_ + src2.tv_usec_);

  sum.normalize ();
  return sum;
}

/* Subtracts two Time_Value objects, returns the difference. */

INLINE Time_Value 
operator - (Time_Value src1, Time_Value src2)
{
  Time_Value delta (src1.tv_sec_ - src2.tv_sec_, src1.tv_usec_ - src2.tv_usec_);

  delta.normalize ();
  return delta;
}

/* True if tv1 > tv2. */

INLINE int
operator > (Time_Value src1, Time_Value src2)
{
  if (src1.tv_sec_ > src2.tv_sec_)
    return 1;
  else if (src1.tv_sec_ == src2.tv_sec_ && src1.tv_usec_ > src2.tv_usec_)
    return 1;
  else
    return 0;
}

/* True if tv1 >= tv2. */

INLINE int
operator >= (Time_Value src1, Time_Value src2)
{
  if (src1.tv_sec_ > src2.tv_sec_)
    return 1;
  else if (src1.tv_sec_ == src2.tv_sec_ && src1.tv_usec_ >= src2.tv_usec_)
    return 1;
  else
    return 0;
}

/* True if tv1 < tv2. */

INLINE int
operator < (Time_Value src1, Time_Value src2)
{
  return src2 > src1;
}

/* True if tv1 >= tv2. */

INLINE int
operator <= (Time_Value src1, Time_Value src2)
{
  return src2 >= src1;
}

/* True if tv1 == tv2. */

INLINE int
operator == (Time_Value src1, Time_Value src2)
{
  return src1.tv_sec_ == src2.tv_sec_ && src1.tv_usec_ == src2.tv_usec_;
}

/* True if tv1 != tv2. */

INLINE int
operator != (Time_Value src1, Time_Value src2)
{
  return !(src1 == src2);
}
