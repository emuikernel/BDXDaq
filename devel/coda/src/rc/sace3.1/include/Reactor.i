/* -*- C++ -*- */
/* Implementation of the Reactor event demultiplexor and event handler dispatcher. */

#include "Log_Msg.h"

/* Performs sanity checking on the HANDLE. */

inline int
Reactor::invalid_handle (HANDLE handle)
{
  return handle < 0 || handle >= this->max_size_;
}

inline int
Reactor::resume_handler (Event_Handler *h)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));
  return this->resume (h->get_handle ());
}

inline int
Reactor::resume_handler (HANDLE handle)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));
  return this->resume (handle);
}

inline int
Reactor::suspend_handler (Event_Handler *h)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));
  return this->suspend (h->get_handle ());
}

inline int
Reactor::suspend_handler (HANDLE handle)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));
  return this->suspend (handle);
}

inline int
Reactor::register_handler (Event_Handler *handler, Reactor_Mask mask)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));
  return this->attach (handler->get_handle (), handler, mask);
}

inline int
Reactor::register_handler (HANDLE handle, 
			   Event_Handler *handler, 
			   Reactor_Mask mask)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));
  return this->attach (handle, handler, mask);
}

inline int
Reactor::register_handler (int signum, 
			   Event_Handler *new_sh, 
			   Sig_Action *new_disp, 
			   Event_Handler **old_sh,
			   Sig_Action *old_disp)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));
  return Signal_Handler::register_handler (signum, new_sh, new_disp, old_sh, old_disp);
}

/* Must be called with locks held */

inline int 
Reactor::get (int signum, Event_Handler **eh)
{
  Event_Handler *handler = Signal_Handler::handler (signum);
  if (eh != 0)
    *eh = handler;
  return 0;
}

inline int
Reactor::handler (HANDLE handle, 
		      Reactor_Mask mask, 
		      Event_Handler **handler)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));
  return this->get (handle, mask, handler);
}

inline int
Reactor::handler (int signum, Event_Handler **handler)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));
  return this->get (signum, handler);
}

inline int
Reactor::remove_handler (Event_Handler *handler, Reactor_Mask mask)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));
  return this->detach (handler->get_handle (), mask);
}

inline int
Reactor::remove_handler (HANDLE handle, Reactor_Mask mask)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));
  return this->detach (handle, mask);
}

inline int
Reactor::remove_handler (int signum, Sig_Action *new_disp, Sig_Action *old_disp)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));
  return Signal_Handler::remove_handler (signum, new_disp, old_disp);
}

inline int
Reactor::max (HANDLE i, HANDLE j, HANDLE k)
{
  int t = i < j ? j : i;

  return k < t ? t : k;
}

/* Note the queue handles its own locking. */

inline int
Reactor::cancel_timer (Event_Handler *handler)
{
  return this->timer_queue_->cancel (handler);
}

/* Performs operations on the "ready" bits. */

inline int
Reactor::ready_ops (HANDLE handle, Reactor_Mask mask, int ops)
{
  MT (Guard <Recursive_Lock <Mutex> > m (this->lock_));
  return this->bit_ops (handle, mask, 
			this->rd_handle_mask_ready_, 
			this->wr_handle_mask_ready_, 
			this->ex_handle_mask_ready_, 
			ops);
}

/* Performs operations on the "ready" bits. */
inline int
Reactor::ready_ops (Event_Handler *handler, Reactor_Mask mask, int ops)
{
  return this->ready_ops (handler->get_handle (), mask, ops);
}


#if !defined (ACE_USE_POLL_IMPLEMENTATION)
/* Perform GET, CLR, SET, and ADD operations on the select() Handle_Sets. 

   GET = 1, // Retrieve current value
   SET = 2, // Set value of bits to new mask (changes the entire mask)
   ADD = 3, // Bitwise "or" the value into the mask (only changes enabled bits)
   CLR = 4  // Bitwise "and" the negation of the value out of the mask (only changes enabled bits)

   Returns the original mask. */

inline int
Reactor::mask_ops (HANDLE handle, Reactor_Mask mask, int ops)
{
  MT (Guard< Recursive_Lock <Mutex> > m (this->lock_));
  return this->bit_ops (handle, mask, 
			this->rd_handle_mask_,
			this->wr_handle_mask_, 
			this->ex_handle_mask_, 
			ops);
}
#endif /* !ACE_USE_POLL_IMPLEMENTATION */

/* Performs operations on the "dispatch" masks. */

inline int
Reactor::mask_ops (Event_Handler *handler, Reactor_Mask mask, int ops)
{
  return this->mask_ops (handler->get_handle (), mask, ops);
}

inline int 
Reactor::any_ready (void) const
{
  Signal_Guard sb;
  
  int result = this->rd_handle_mask_ready_.num_set () > 0
    || this->wr_handle_mask_ready_.num_set () > 0
    || this->ex_handle_mask_ready_.num_set () > 0;

  return result;
}
