/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Reactor.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_Reactor_H)
#define ACE_Reactor_H

#include "Handle_Set.h"
#include "Timer_Queue.h"
#include "Signal.h"

typedef void (Handle_Set::*ACE_FDS_PTMF) (HANDLE);
typedef int (Event_Handler::*ACE_EH_PTMF) (HANDLE);

class Reactor
  // = TITLE
  //     Wait on multiple file descriptors until a condition occurs. 
  //
  // = DESCRIPTION
  //
{
public:
  enum {DEFAULT_SIZE = 256};
  
  // = Initialization and termination. 

  Reactor (void);
  Reactor (int size, int restart = 0);
  virtual ~Reactor (void);
  
  virtual int  open (int size = DEFAULT_SIZE, int restart = 0);
  virtual void close (void);

  // = Timer management. 
  virtual int  schedule_timer (Event_Handler *, const void *arg,
			       const Time_Value &delta,
			       const Time_Value &interval = Time_Value::zero);
  virtual int  cancel_timer (Event_Handler *);
  
  // = Event loop drivers. 
  virtual int  handle_events (Time_Value * = 0);
  virtual int  handle_events (Time_Value &tv);

  // = Register and remove Handlers. 
  virtual int  register_handler (Event_Handler *, Reactor_Mask);
  virtual int  register_handler (HANDLE handle, Event_Handler *, Reactor_Mask);      
  virtual int  register_handler (int signum, 
				 Event_Handler *new_sh,
				 Sig_Action *new_disp = 0,
				 Event_Handler **old_sh = 0,
				 Sig_Action *old_disp = 0);
  virtual int  register_handler (const Sig_Set &sigset, Event_Handler *new_sh);

  virtual int  remove_handler (Event_Handler *, Reactor_Mask);
  virtual int  remove_handler (HANDLE handle, Reactor_Mask);
  virtual int  remove_handler (int signum, Sig_Action *new_disp, Sig_Action *old_disp);
  virtual int  remove_handler (const Sig_Set &sigset);

  // = Suspend and resume Handlers. 
  virtual int  resume_handler (Event_Handler *);
  virtual int  resume_handler (HANDLE handle);

  virtual int  suspend_handler (Event_Handler *);
  virtual int  suspend_handler (HANDLE handle);

  // = Misc. Handler operations. 
  virtual int  handler (HANDLE handle, Reactor_Mask, Event_Handler ** = 0);
  virtual int  handler (int signum, Event_Handler ** = 0);

// = Operations on the "ready" bits and the "dispatch" bits. 
  enum 
  {
    GET = 1, // Retrieve current value
    SET = 2, // Set value of bits to new mask (changes the entire mask)
    ADD = 3, // Bitwise "or" the value into the mask (only changes enabled bits)
    CLR = 4  // Bitwise "and" the negation of the value out of the mask 
             // (only changes enabled bits)
  };

  // = Get/set/add/clr the ready bit. 
  virtual int  ready_ops (Event_Handler *, Reactor_Mask, int ops);
  virtual int  ready_ops (HANDLE handle, Reactor_Mask, int ops);

  // = Get/set/add/clr the dispatch mask. 
  virtual int  mask_ops (Event_Handler *, Reactor_Mask, int ops);
  virtual int  mask_ops (HANDLE handle, Reactor_Mask, int ops);  

  virtual int  unblock (void);
  // Wakeup Reactor if currently block in select()/poll(). 
  
protected:
  virtual int  attach (HANDLE handle, Event_Handler *, Reactor_Mask);
  virtual int  detach (HANDLE handle, Reactor_Mask);
  virtual int  get (HANDLE handle, Reactor_Mask, Event_Handler ** = 0);
  virtual int  get (int signum, Event_Handler ** = 0);
  virtual int  any_ready (void) const;
  virtual void notify_handle (HANDLE, Reactor_Mask, Handle_Set &, 
			      Event_Handler *, ACE_EH_PTMF);
  
  virtual int bit_ops (HANDLE handle, Reactor_Mask mask, 
		       Handle_Set &rd, Handle_Set &wr, Handle_Set &ex, int ops);
  virtual int  suspend (HANDLE handle);
  virtual int  resume (HANDLE handle);
  virtual Time_Value *calculate_timeout (Time_Value *) const;
  virtual int  handle_error (void);
  virtual int  check_connections (void);
  virtual int  invalid_handle (HANDLE handle);
  
#if defined (ACE_USE_POLL_IMPLEMENTATION)
  virtual int  fill_in_ready (void);
  virtual int  wait_for (Time_Value *);
  virtual void dispatch (int);
#else
  virtual int  fill_in_ready (Handle_Set &, Handle_Set &, Handle_Set &);
  virtual int  wait_for (Handle_Set &, Handle_Set &, Handle_Set &, Time_Value *);
  virtual void dispatch (int, Handle_Set &, Handle_Set &, Handle_Set &);
#endif /* ACE_USE_POLL_IMPLEMENTATION */

protected:
  class Null_Callback : public Event_Handler
  {
  public:
    Null_Callback (void);
    ~Null_Callback (void);
    virtual int handle_input (HANDLE handle);
  } null_callback_;
    
  HANDLE pipe_handles_[2]; 
  // Used to unblock reactor when changes are made dynamically 
  int is_blocked_;
  int restart_; 
  // Restart automatically when interrupted 
  Timer_Queue *timer_queue_; 
  // Defined as a pointer to allow overriding by derived classes... 
  int max_size_;
  int max_handlep1_;
  Handle_Set rd_handle_mask_ready_;
  Handle_Set wr_handle_mask_ready_;
  Handle_Set ex_handle_mask_ready_;
  Event_Handler **event_handlers_;
  
#if defined (ACE_USE_POLL_IMPLEMENTATION)
  pollfd *poll_handles_;
#else
  Handle_Set rd_handle_mask_;
  Handle_Set wr_handle_mask_;
  Handle_Set ex_handle_mask_;
#endif /* ACE_USE_POLL_IMPLEMENTATION */

  Signal_Handler signal_handler;
  // Handle signals in an elegant manner. 

private:
  int max (HANDLE i, HANDLE j, HANDLE k);
  
  // = Deny access since member-wise won't work 
  Reactor (const Reactor &);
  Reactor &operator = (const Reactor &);
  
#if defined (ACE_MT_SAFE)
  Recursive_Lock <Mutex> lock_; 
  // Synchronization variable for the MT_SAFE Reactor 
#endif /* ACE_MT_SAFE */
};

#include "Reactor.i"
#endif /* _Reactor_H */
