/* -*- C++ -*- */
/* Interface to timers. */

INLINE
Timer_Node::Timer_Node (Event_Handler *h, const void *a, const Time_Value &t, 
			const Time_Value &i, Timer_Node *n)
		       : handler_ (h), arg_ (a), timer_value_ (t), interval_ (i), next_ (n)
{
}

/* Create an empty queue. */

INLINE
Timer_Queue::Timer_Queue (void): head_ (0)
{
}

/* Remove all remaining items in the queue. */

INLINE
Timer_Queue::~Timer_Queue (void)
{
  MT (Guard<Recursive_Lock<Mutex> > m (this->lock_));
  
  Timer_Node *temp = this->head_;
  
  while (temp != 0)
    {
      Timer_Node *next = temp->next_;
      delete temp;
      temp = next;
    }
}

/* Checks if queue is empty. */

INLINE int 
Timer_Queue::is_empty (void) const
{
  return this->head_ == 0;
}

/* Returns earliest time in a non-empty queue. */

INLINE const Time_Value &
Timer_Queue::earliest_time (void) const
{
  return this->head_->timer_value_;
}

/* Returns the current time of day. */

INLINE const Time_Value &
Timer_Queue::current_time (void)
{
  static Time_Value tv;
  timeval	  cur_time;

#if !defined (ACE_HAS_SVR4_GETTIMEOFDAY)
  ::gettimeofday (&cur_time, 0);
#else
  ::gettimeofday (&cur_time);
#endif
  tv.set (cur_time);
  return tv;
}

/* Reschedule a periodic timer.  This function must be
   called with the mutex lock held. */

INLINE void 
Timer_Queue::reschedule (Timer_Node *expired)
{
  if (this->is_empty () || expired->timer_value_ < this->earliest_time ())
    {
      expired->next_ = this->head_;
      this->head_    = expired;
    }
  else
    {
      Timer_Node *before = this->head_;
      Timer_Node *after  = this->head_->next_;

      /* Locate the proper position in the queue */

      while (after != 0 && expired->timer_value_ > after->timer_value_)
	{
	  before = after;
	  after  = after->next_;
	}

      expired->next_ = after;
      before->next_  = expired;
    }
}

/* Insert a new handler that expires at time future_time; 
   if interval is > 0, the handler will be reinvoked periodically. */

INLINE int
Timer_Queue::schedule (Event_Handler *handler, 
		       const void	   *arg,
		       const Time_Value	   &future_time, 
		       const Time_Value	   &interval)
{
  MT (Guard<Recursive_Lock<Mutex> > m (this->lock_));

  if (this->is_empty () || future_time < this->earliest_time ())
    {
      if ((this->head_ = new Timer_Node (handler, arg, future_time, interval, this->head_)) == 0)
	return -1;
    }
  else
    {
      Timer_Node *before = this->head_;
      Timer_Node *after  = this->head_->next_;

      while (after != 0 && future_time > after->timer_value_)
	{
	  before = after;
	  after = after->next_;
	}

      if ((before->next_ = new Timer_Node (handler, arg, future_time, interval, after)) == 0)
	return -1;
    }

  return 0;
}

/* Locate and remove handler from the timer queue */

INLINE int
Timer_Queue::cancel (Event_Handler *handler)
{
  MT (Guard<Recursive_Lock<Mutex> > m (this->lock_));

  Timer_Node *before = 0;
  Timer_Node *temp   = this->head_;

  while (temp != 0 && temp->handler_ != handler)
    {
      before = temp;
      temp   = temp->next_;
    }

  if (temp != 0)
    {
      if (before == 0)
	this->head_ = temp->next_;
      else
	before->next_ = temp->next_;
      delete temp;
    }
  return 0;
}

/* Expire all Timers whose values are <= cur_time. */

INLINE int
Timer_Queue::expire (const Time_Value &cur_time)
{
  MT (Guard<Recursive_Lock<Mutex> > m (this->lock_));

  for (;;)
    {
      if (this->is_empty () || this->earliest_time () > cur_time)
	break;

      Timer_Node    *expired = this->head_;
      Event_Handler *handler = (Event_Handler *) expired->handler_;
      const void    *arg     = expired->arg_;
      int	    reclaim  = 1;
      int	    res;

      this->head_ = this->head_->next_;

      if (expired->interval_ > Time_Value::zero)
	{
	  expired->timer_value_ = cur_time + expired->interval_;
	  this->reschedule (expired);
	  reclaim = 0;
	}

      res = handler->handle_timeout (cur_time, arg);

      if (res == -1)
	this->cancel (handler);

      if (reclaim)
	delete expired;
    }
  return 0;
}
