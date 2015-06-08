/* Reactor provides an interface to the select() or poll() system call. */

#include "Reactor.h"

Reactor::Null_Callback::Null_Callback (void)
{
}

Reactor::Null_Callback::~Null_Callback (void)
{
}

/* Special trick to unblock select() or poll() during updates... 
   Thanks to Paul Stephenson at Ericsson for suggesting this approach. */

int
Reactor::Null_Callback::handle_input (HANDLE handle)
{
  char c;
  int  n;

  while ((n = ::read (handle, &c, sizeof c)) > 0)
    continue;

  if (n == -1 && errno == EWOULDBLOCK)
    return 0;
  else
    return -1;
}

/* Initialize the Reactor */

int
Reactor::open (int size, int rs)
{
  // Allow the Reactor to be reopened if you want to give it a
  // different size once the constructor has run.

  this->restart_ = rs;

  if (this->event_handlers_ == 0)
    {
      this->timer_queue_    = new Timer_Queue;
      this->max_handlep1_   = 0;
      this->max_size_       = size;
      this->event_handlers_ = new Event_Handler *[size];
#if defined (ACE_USE_POLL_IMPLEMENTATION)
      this->poll_handles_   = new pollfd[size];
#endif /* ACE_USE_POLL_IMPLEMENTATION */
    }
  else if (size > this->max_size_) // Only resize if we are making the
				   // Reactor *larger.*
    {
      delete [] this->event_handlers_;
      this->event_handlers_ = new Event_Handler *[size];

#if defined (ACE_USE_POLL_IMPLEMENTATION)
      delete [] this->poll_handles_;
      this->poll_handles_ = new pollfd[size];
#endif /* ACE_USE_POLL_IMPLEMENTATION */
      this->max_size_ = size;

      // Make sure we don't point off the end of the buffer.
      if (this->max_handlep1_ > this->max_size_)
	this->max_handlep1_ = this->max_size_;
    }

  for (int i = 0; i < size; i++)
    {
      this->event_handlers_[i] = 0;
#if defined (ACE_USE_POLL_IMPLEMENTATION)
      this->poll_handles_[i].fd    = -1;
#endif /* ACE_USE_POLL_IMPLEMENTATION */
    }

  this->is_blocked_ = 0;
  if (::pipe (this->pipe_handles_) == -1)
    return -1;
  else
    {
      if (ace_set_fl (this->pipe_handles_[1], O_NONBLOCK) == -1
	  || ace_set_fl (this->pipe_handles_[0], O_NONBLOCK) == -1
	  || this->register_handler (this->pipe_handles_[0], 
				     &this->null_callback_,
				     Event_Handler::READ_MASK) == -1)
	return -1;
      return 0;
    }
}

INLINE
Reactor::Reactor (void): event_handlers_ (0) // Use this as a flag to detect second time in...
{
  if (this->open ())
    LM_ERROR ((LOG_ERROR, "%p\n", "open failed"));
}

int
Reactor::unblock (void)
{
  if (this->is_blocked_)
    return ::write (this->pipe_handles_[1], "a", 1);
  else
    return 0;
}

/* Initialize the new Reactor */

INLINE
Reactor::Reactor (int size, int rs): event_handlers_ (0)
{
  if (this->open (size, rs) == -1)
    LM_ERROR ((LOG_ERROR, "%p\n", "open failed"));
}

/* Close down the Reactor instance, detaching any remaining Event_Handers */

void
Reactor::close (void)
{ 
  MT (Guard <Recursive_Lock <Mutex> > m (this->lock_));

  if (this->timer_queue_ != 0)
    {
      for (int i = 0; i < this->max_handlep1_; i++)
	this->detach (i, Event_Handler::RWE_MASK);

      delete [] this->event_handlers_;
#if defined (ACE_USE_POLL_IMPLEMENTATION)
      delete [] this->poll_handles_;
#endif /* ACE_USE_POLL_IMPLEMENTATION */
      delete this->timer_queue_;
    }
}
  
INLINE
Reactor::~Reactor (void)
{
  this->close ();
}

int
Reactor::register_handler (const Sig_Set &sigset, Event_Handler *new_sh)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));
  int result = 0;

  for (int i = 1; i < NSIG; i++)
    if (sigset.ismember (i) && this->register_handler (i, new_sh) == -1)
      result = -1;

  return result;
}

int
Reactor::remove_handler (const Sig_Set &sigset)
{
  MT (Guard <Recursive_Lock <Mutex> > m (this->lock_));
  int result = 0;

  for (int i = 1; i < NSIG; i++)
    if (sigset.ismember (i) && this->remove_handler (i) == -1)
      result = -1;

  return result;    
}

/* Note the queue handles its own locking. */

int
Reactor::schedule_timer (Event_Handler *handler, 
			 const void *arg,
			 const Time_Value &delta_time, 
			 const Time_Value &interval)
{
  int result = this->timer_queue_->schedule (handler, arg,
					    Timer_Queue::current_time () + delta_time, 
					    interval);
  this->unblock ();
  return result;
}

int 
Reactor::handle_events (Time_Value &how_long)
{
  Time_Value prev_time;
  Time_Value elapsed_time;
  int	     success;

  prev_time    = Timer_Queue::current_time ();
  success      = this->handle_events (&how_long);
  elapsed_time = Timer_Queue::current_time () - prev_time;

  if (how_long > elapsed_time)
    how_long = how_long - elapsed_time;
  else
    {
      how_long = Time_Value::zero; /* Used all of timeout */
      errno    = ETIME;
    }
  return success;
}

/* Must be called with locks held */

Time_Value *
Reactor::calculate_timeout (Time_Value *how_long) const
{
  static Time_Value timeout;

  if (!this->timer_queue_->is_empty ())
    {
      Time_Value cur_time = Timer_Queue::current_time ();

      if (this->timer_queue_->earliest_time () > cur_time)
	{
	  timeout = this->timer_queue_->earliest_time () - cur_time;
	  if (how_long == 0 || *how_long > timeout)
	    how_long = &timeout;
	}
      else
	{
	  timeout  = Time_Value::zero;
	  how_long = &timeout;
	}
    }

  return how_long;
}

int
Reactor::handle_error (void)
{
  if (errno == EINTR)
    return this->restart_;
  else if (errno == EBADF)
    return this->check_connections ();
  else
    return -1;
}

void
Reactor::notify_handle (HANDLE handle, 
			Reactor_Mask mask, 
			Handle_Set &bitmask,
			Event_Handler *iop, 
			ACE_EH_PTMF ptmf)
{
  if (iop == 0)
    return;

  int status = (iop->*ptmf) (handle);

  if  (status < 0)
    this->detach (handle, mask);
  else if (status > 0)
    bitmask.set_bit (handle);
}

/* Perform GET, CLR, SET, and ADD operations on the Handle_Sets.

   GET = 1, // Retrieve current value
   SET = 2, // Set value of bits to new mask (changes the entire mask)
   ADD = 3, // Bitwise "or" the value into the mask (only changes enabled bits)
   CLR = 4  // Bitwise "and" the negation of the value out of the mask (only changes enabled bits)

   Returns the original mask. 

   Must be called with locks held. */

int
Reactor::bit_ops (HANDLE handle,
		  Reactor_Mask mask,
		  Handle_Set &rd,
		  Handle_Set &wr, 
		  Handle_Set &ex, 
		  int ops)
{
  if (this->invalid_handle (handle))
    return -1;

  Signal_Guard sb; // Block out all signals until method returns.

  ACE_FDS_PTMF ptmf  = &Handle_Set::set_bit;
  u_long       omask = Event_Handler::NULL_MASK;

  switch (ops)
    {
    case Reactor::GET:
      if (rd.is_set (handle))
	::SET_BITS (omask, Event_Handler::READ_MASK);
      if (wr.is_set (handle))
	::SET_BITS (omask, Event_Handler::WRITE_MASK);
      if (ex.is_set (handle))
	::SET_BITS (omask, Event_Handler::EXCEPT_MASK);
      break;

    case Reactor::CLR:
      ptmf = &Handle_Set::clr_bit;
      /* FALLTHRU */
    case Reactor::SET:
      /* FALLTHRU */
    case Reactor::ADD:

      /* The following code is rather subtle...  Note that if we are
         doing a Reactor::SET then if the bit is not enabled in the mask 
	 we need to clear the bit from the Handle_Set.  On the other hand,
	 if we are doing a Reactor::CLR or a Reactor::ADD we just carry
	 out the operations specified by the mask. */

      if (::BIT_ENABLED (mask, Event_Handler::READ_MASK))
	{
	  (rd.*ptmf) (handle);
	  ::SET_BITS (omask, Event_Handler::READ_MASK);
	}
      else if (Reactor::SET)
	rd.clr_bit (handle);

      if (::BIT_ENABLED (mask, Event_Handler::WRITE_MASK))
	{
	  (wr.*ptmf) (handle);
	  ::SET_BITS (omask, Event_Handler::WRITE_MASK);
	}
      else if (Reactor::SET)
	wr.clr_bit (handle);

      if (::BIT_ENABLED (mask, Event_Handler::EXCEPT_MASK))
	{
	  (ex.*ptmf) (handle);
	  ::SET_BITS (omask, Event_Handler::EXCEPT_MASK);
	}
      else if (Reactor::SET)
	ex.clr_bit (handle);
      break;
    default: 
      return -1;
    }
  return omask;
}

#if defined (ACE_USE_POLL_IMPLEMENTATION)
/* Perform GET, CLR, SET, and ADD operations on the poll() events mask. 

   GET = 1, // Retrieve current value
   SET = 2, // Set value of bits to new mask (changes the entire mask)
   ADD = 3, // Bitwise "or" the value into the mask (only changes enabled bits)
   CLR = 4  // Bitwise "and" the negation of the value out of the mask (only changes enabled bits)

   Returns the original mask. */

int
Reactor::mask_ops (HANDLE handle, Reactor_Mask mask, int ops)
{
  MT (Guard <Recursive_Lock <Mutex> > m (this->lock_));
  
  // Tim sez that documentation would help... ;-)
  if (this->invalid_handle (handle) || this->poll_handles_[handle].fd == -1)
    return -1;

  u_long omask = this->poll_handles_[handle].events;

  switch (ops)
    {
    case Reactor::GET:
      return omask;
    case Reactor::CLR:
      ::CLR_BITS (this->poll_handles_[handle].events, mask);
      break;
    case Reactor::SET:
      this->poll_handles_[handle].events = ops;
      break;
    case Reactor::ADD:
      ::SET_BITS (this->poll_handles_[handle].events, mask);
      break;
    default:
      return -1;
    }

  return omask;
}
/* Must be called with locks held */

int
Reactor::get (HANDLE handle, 
	      Reactor_Mask mask, 
	      Event_Handler **handler)
{
  if (this->invalid_handle (handle)
      || this->poll_handles_[handle].fd == -1
      || (BIT_ENABLED (this->poll_handles_[handle].events, mask)) == 0)
    return -1;
  if (handler != 0)
    *handler = this->event_handlers_[handle];
  return 0;
}

/* Must be called with locks held */

int
Reactor::resume (HANDLE handle)
{
  if (this->invalid_handle (handle) || this->event_handlers_[handle] == 0)
    return -1;
  
  this->poll_handles_[handle].fd = handle;
  return 0;
}

/* Must be called with locks held */

int
Reactor::suspend (HANDLE handle)
{
  if (this->invalid_handle (handle) || this->event_handlers_[handle] == 0)
    return -1;

  this->poll_handles_[handle].fd = -1;  
  return 0;
}

/* Must be called with locks held */

int
Reactor::attach (HANDLE handle, 
		 Event_Handler *handler, 
		 Reactor_Mask mask)
{
  if (this->invalid_handle (handle))
    return -1;

  this->event_handlers_[handle]      = handler;
  this->poll_handles_[handle].fd     = handle;
  this->poll_handles_[handle].events = mask;

  if (this->max_handlep1_ < handle + 1)
    this->max_handlep1_ = handle + 1;

  return this->unblock ();
}

/* This function *must* be called with the lock held... */

int
Reactor::detach (HANDLE handle, Reactor_Mask mask)
{
  if (this->invalid_handle (handle) || this->event_handlers_[handle] == 0)
    return -1;

  Event_Handler *eh = this->event_handlers_[handle];
  
  CLR_BITS (this->poll_handles_[handle].events, mask); 

  if (::BIT_ENABLED (mask, Event_Handler::DONT_CALL) == 0)
    eh->handle_close (handle, mask);

  /* If all the events were cleared then totally shut down Event Handler */

  if (this->poll_handles_[handle].events == 0)
    {
      this->event_handlers_[handle] = 0;
      this->poll_handles_[handle].fd = -1;

      if (this->max_handlep1_ == handle + 1)
	{
	  while (--handle >= 0 && this->event_handlers_[handle] == 0)
	    continue;

	  this->max_handlep1_ = handle + 1;
	}
    }
  return this->unblock ();
}

int 
Reactor::fill_in_ready (void)
{
  Signal_Guard sb;
  int	       nfound = 0;
  int	       i;

  for (Handle_Set_Iterator fdir (this->rd_handle_mask_ready_); 
       (i = fdir ()) != -1; 
       ++fdir, nfound++)
    ::SET_BITS (this->poll_handles_[i].revents, POLLIN);

  for (Handle_Set_Iterator fdiw (this->wr_handle_mask_ready_); 
       (i = fdiw ()) != -1; 
       ++fdiw, nfound++)
    ::SET_BITS (this->poll_handles_[i].revents, POLLOUT);

  for (Handle_Set_Iterator fdie (this->ex_handle_mask_ready_); 
       (i = fdie ()) != -1; 
       ++fdie, nfound++)
    ::SET_BITS (this->poll_handles_[i].revents, POLLPRI);

  this->rd_handle_mask_ready_.reset ();
  this->wr_handle_mask_ready_.reset ();
  this->ex_handle_mask_ready_.reset ();
  return nfound;
}

/* Must be called with lock held */

int 
Reactor::wait_for (Time_Value *how_long)
{
  int nfound;

  this->is_blocked_ = 1;

  do
    {
      how_long = this->calculate_timeout (how_long);
      long   msecs = how_long == 0
	? -1 : (how_long->sec () * 1000) + (how_long->usec () / 1000);
      int    nhandles  = this->max_handlep1_;
      pollfd *phandles = this->poll_handles_;

#if defined (ACE_HAS_THREADS)
      MT (this->lock_.release ());
#endif /* ACE_HAS_THREADS */
      nfound = ::poll (phandles, nhandles, int (msecs));
#if defined (ACE_HAS_THREADS)
      MT (this->lock_.acquire ());
#endif /* ACE_HAS_THREADS */
    }
  while (nfound == -1 && this->handle_error () > 0);

  this->is_blocked_ = 0;
  return nfound;
}

void 
Reactor::dispatch (int nfound)
{
  for (int i = 0; nfound > 0 && i < this->max_handlep1_; i++)
    {
      int found = 0;

      // Handle output conditions (this code needs to come
      // first to handle the obscure case of piggy-backed data
      // coming along with the final handshake message of a
      // nonblocking connection.

      if (this->poll_handles_[i].revents & POLLOUT)
	{
	  this->notify_handle (i, Event_Handler::WRITE_MASK, this->wr_handle_mask_ready_, 
			       this->event_handlers_[i], &Event_Handler::handle_output);
	  found = 1;
	}

      // Handle input and shutdown conditions.
      if ((this->poll_handles_[i].revents & POLLIN) 
	  || (this->poll_handles_[i].revents & POLLHUP)
	  || this->poll_handles_[i].revents & POLLERR)
	{
	  this->notify_handle (i, Event_Handler::READ_MASK, this->rd_handle_mask_ready_, 
			       this->event_handlers_[i], &Event_Handler::handle_input);
	  found = 1;
	}

      // Handle "exceptional" conditions.
      if (this->poll_handles_[i].revents & POLLPRI)
	{
	  this->notify_handle (i, Event_Handler::EXCEPT_MASK, this->ex_handle_mask_ready_, 
			       this->event_handlers_[i], &Event_Handler::handle_exception);
	  found = 1;
	}

      this->poll_handles_[i].revents = 0;

      if (found)
	nfound--;
    }

  if (!this->timer_queue_->is_empty ())
    this->timer_queue_->expire (Timer_Queue::current_time ());
}

int 
Reactor::handle_events (Time_Value *how_long)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));
  int nfound;

  if (this->any_ready ())
    nfound = this->fill_in_ready ();
  else
    nfound = this->wait_for (how_long);

  for (;;)
    {
      this->dispatch (nfound);

      if (Signal_Handler::sig_pending () != 0)
	{
	  Signal_Handler::clear_sig_pending ();

	  if (this->any_ready ())
	    {
	      nfound = this->fill_in_ready ();
	      continue;
	    }
	}
      break;
    }

  return nfound;
}

int
Reactor::check_connections (void)
{
  pollfd p_handle;

  p_handle.events = POLLIN;

  for (int handle = 0; handle < this->max_handlep1_; handle++)
    {
      if (this->event_handlers_[handle] != 0)
	{
	  p_handle.fd = handle;
	  if (::poll (&p_handle, 1, 0) == -1)
	    this->detach (handle, Event_Handler::RWE_MASK);
	}
    }

  return 0;
}

#else /* Use the select() implementation */

/* Must be called with locks held */

int
Reactor::get (HANDLE handle, Reactor_Mask mask, Event_Handler **handler)
{
  if (this->invalid_handle (handle) || this->event_handlers_[handle] == 0)
    return -1;
  else
    {
      if (::BIT_ENABLED (mask, Event_Handler::READ_MASK)
	  && this->rd_handle_mask_.is_set (handle) == 0)
	return -1;
      if (::BIT_ENABLED (mask, Event_Handler::WRITE_MASK)
	  && this->wr_handle_mask_.is_set (handle) == 0)
	return -1;
      if (::BIT_ENABLED (mask, Event_Handler::EXCEPT_MASK)
	  && this->ex_handle_mask_.is_set (handle) == 0)	  
	return -1;
    }

  if (handler != 0)
    *handler = this->event_handlers_[handle];
  return 0;
}

/* Must be called with locks held */

int
Reactor::resume (HANDLE handle)
{
  if (this->invalid_handle (handle) || this->event_handlers_[handle] == 0)
    return -1;
  
  this->rd_handle_mask_.set_bit (handle);
  this->wr_handle_mask_.set_bit (handle);
  this->ex_handle_mask_.set_bit (handle);
  return 0;
}

/* Must be called with locks held */

int
Reactor::suspend (HANDLE handle)
{
  if (this->invalid_handle (handle) || this->event_handlers_[handle] == 0)
    return -1;

  this->rd_handle_mask_.clr_bit (handle);
  this->wr_handle_mask_.clr_bit (handle);
  this->ex_handle_mask_.clr_bit (handle);
  return 0;
}

/* Must be called with locks held */

int
Reactor::attach (HANDLE handle, 
		 Event_Handler *handler,
		 Reactor_Mask mask)
{
  if (this->invalid_handle (handle))
    return -1;

  this->event_handlers_[handle] = handler;

  this->bit_ops (handle, mask, 
		 this->rd_handle_mask_, 
		 this->wr_handle_mask_, 
		 this->ex_handle_mask_, 
		 Reactor::ADD);

  if (this->max_handlep1_ < handle + 1)
    this->max_handlep1_ = handle + 1;

  return this->unblock ();
}

int
Reactor::detach (HANDLE handle, Reactor_Mask mask)
{
  if (this->invalid_handle (handle) || this->event_handlers_[handle] == 0)
    return -1;

  Event_Handler *eh = this->event_handlers_[handle];

  this->bit_ops (handle, mask, this->rd_handle_mask_, this->wr_handle_mask_,
		 this->ex_handle_mask_, Reactor::CLR);

  if (::BIT_ENABLED (mask, Event_Handler::DONT_CALL) == 0)
    eh->handle_close (handle, mask);

  /* If all the events were cleared then totally shut down Event Handler */

  if (this->rd_handle_mask_.is_set (handle) == 0
      && this->wr_handle_mask_.is_set (handle) == 0 
      && this->ex_handle_mask_.is_set (handle) == 0)
    {
      this->event_handlers_[handle] = 0;

      if (this->max_handlep1_ == handle + 1)
	this->max_handlep1_ = this->max (this->rd_handle_mask_.max_set (),
				    this->wr_handle_mask_.max_set (),
				    this->ex_handle_mask_.max_set ()) + 1;
    }
  return this->unblock ();
}

/* Must be called with lock held */

int 
Reactor::wait_for (Handle_Set &rmaskret, Handle_Set &wmaskret, 
		   Handle_Set &emaskret, Time_Value *how_long)
{
  int nfound;

  this->is_blocked_ = 1;

  do
    {
      int width = this->max_handlep1_;
      rmaskret  = this->rd_handle_mask_;
      wmaskret  = this->wr_handle_mask_;
      emaskret  = this->ex_handle_mask_;
      how_long  = this->calculate_timeout (how_long);
      
#if defined (ACE_HAS_THREADS)
      MT (this->lock_.release ());
#endif /* ACE_HAS_THREADS */
#if defined (ACE_SELECT_USES_INT)
      nfound = ::select (width, (int *) (fd_set *)rmaskret, (int *) (fd_set *) wmaskret, 
			 (int *) (fd_set *) emaskret, (timeval *) how_long);
#else
        nfound = ::select (width, (fd_set *) rmaskret, (fd_set *) wmaskret, 
			   (fd_set *) emaskret, (timeval *) how_long);
#endif /* ACE_SELECT_USES_INT */
#if defined (ACE_HAS_THREADS)
      MT (this->lock_.acquire ());
#endif /* ACE_HAS_THREADS */
    } 
  while (nfound == -1 && this->handle_error () > 0);
  
  this->is_blocked_ = 0;

  if (nfound > 0)
    {
      rmaskret.sync (this->max_handlep1_);
      wmaskret.sync (this->max_handlep1_);
      emaskret.sync (this->max_handlep1_);
    }
  return nfound; /* Timed out or input available */
}

void 
Reactor::dispatch (int nfound, 
		   Handle_Set &rmaskret, 
		   Handle_Set &wmaskret, 
		   Handle_Set &emaskret)
{
  int i;

  if (nfound > 0)
    // Handle output conditions (this code needs to come
    // first to handle the obscure case of piggy-backed data
    // coming along with the final handshake message of a
    // nonblocking connection.

    for (Handle_Set_Iterator fdiw (wmaskret); 
	 (i = fdiw ()) != -1 && --nfound >= 0; 
	 ++fdiw)
      this->notify_handle (i, Event_Handler::WRITE_MASK, this->wr_handle_mask_ready_, 
		       this->event_handlers_[i], &Event_Handler::handle_output);

  if (nfound > 0)
    // Handle input and shutdown conditions.
    for (Handle_Set_Iterator fdir (rmaskret); 
	 (i = fdir ()) != -1 && --nfound >= 0; 
	 ++fdir)
      this->notify_handle (i, Event_Handler::READ_MASK, this->rd_handle_mask_ready_, 
		       this->event_handlers_[i], &Event_Handler::handle_input);

  if (nfound > 0)
      // Handle "exceptional" conditions.
      for (Handle_Set_Iterator fdie (emaskret); 
	   (i = fdie ()) != -1 && --nfound >= 0; 
	   ++fdie)
      this->notify_handle (i, Event_Handler::EXCEPT_MASK, this->ex_handle_mask_ready_, 
		       this->event_handlers_[i], &Event_Handler::handle_exception);

  if (!this->timer_queue_->is_empty ())
    this->timer_queue_->expire (Timer_Queue::current_time ());
}

int 
Reactor::fill_in_ready (Handle_Set &rmaskret, Handle_Set &wmaskret, Handle_Set &emaskret)
{
  Signal_Guard sb;
  int	       result;
  
  rmaskret = this->rd_handle_mask_ready_;
  wmaskret = this->wr_handle_mask_ready_;
  emaskret = this->ex_handle_mask_ready_;

  this->rd_handle_mask_ready_.reset ();
  this->wr_handle_mask_ready_.reset ();
  this->ex_handle_mask_ready_.reset ();

  result = rmaskret.num_set () + wmaskret.num_set () + emaskret.num_set ();
  return result;
}

int 
Reactor::handle_events (Time_Value *how_long)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));

  Handle_Set rmaskret;
  Handle_Set wmaskret;
  Handle_Set emaskret;
  int	 nfound;

  if (this->any_ready ())
    nfound = this->fill_in_ready (rmaskret, wmaskret, emaskret);
  else
    nfound = this->wait_for (rmaskret, wmaskret, emaskret, how_long);

  for (;;)
    {
      this->dispatch (nfound, rmaskret, wmaskret, emaskret);

      if (Signal_Handler::sig_pending () != 0)
	{
	  Signal_Handler::clear_sig_pending ();
	  
	  if (this->any_ready ())
	    {
	      nfound = this->fill_in_ready (rmaskret, wmaskret, emaskret);
	      continue;
	    }
	}
      break;
    }
  return nfound;
}

int
Reactor::check_connections (void)
{
  Time_Value time_poll = Time_Value::zero;
  Handle_Set     rmask;

  for (int handle = 0; handle < this->max_handlep1_; handle++)
    {
      if (this->event_handlers_[handle] != 0)
	{
	  rmask.set_bit (handle);

#if defined (ACE_SELECT_USES_INT)
	  if (::select (handle + 1, (int *) (fd_set *) rmask, 0, 0, (timeval *) &time_poll) < 0)
#else
	  if (::select (handle + 1, rmask, 0, 0, (timeval *) &time_poll) < 0)
#endif /* ACE_SELECT_USES_INT */
	    this->detach (handle, Event_Handler::RWE_MASK);
	  rmask.clr_bit (handle);
	}
    }
  return 0;
}
#endif /* ACE_USE_POLL_IMPLEMENTATION */
