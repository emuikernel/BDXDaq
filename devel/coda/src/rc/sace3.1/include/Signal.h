/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Signal.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_SIGNAL_HANDLER_H)
#define ACE_SIGNAL_HANDLER_H

#include "Event_Handler.h"
#include "sysincludes.h"

/* Provide a C++ wrapper for the C sigset_t API. */

class Sig_Set
  // = TITLE
  //     Handle signals via a more elegant C++ interface (e.g., doesn't require
  // the use of global variables or global functions in an application). 
  //
  // = DESCRIPTION
  //
{
public:
  Sig_Set (sigset_t *);
  Sig_Set (int fill = 0);
  ~Sig_Set (void);
  int emptyset (void);
  int fillset (void);
  int addset (int signo);
  int delset (int signo);
  int ismember (int signo) const;
  operator sigset_t *();

private:
  sigset_t sigset_;
};


class Sig_Action
  // = TITLE
  //     C++ wrapper around struct sigaction 
  // 
  // = DESCRIPTION
  //
{
public:
  Sig_Action (int = 0, sigset_t * = 0, SignalHandler = 0);
  Sig_Action (struct sigaction *);

  int register_action (int signum, Sig_Action *oaction = 0);
  int restore_action (int signum, Sig_Action *oaction);

  void set (struct sigaction *);
  struct sigaction *get (void);

  int flags (void);
  void flags (int);

  sigset_t *mask (void);
  void mask (sigset_t *);

  SignalHandler handler (void);
  void handler (SignalHandler);

private:
  struct sigaction sa_;
};


class Signal_Guard
  // = TITLE
  //     Hold signals in MASK for duration of a C++ statement block.  Note that   // a "0" for mask causes all signals to be held. 
  //
  // = DESCRIPTION
  //
{
public:
  Signal_Guard (Sig_Set *mask = 0);
  ~Signal_Guard (void);

private:
  Sig_Set omask_; // Original signal mask
};

class Signal_Handler
  // = TITLE
  //
  //
  // = DESCRIPTION
  //
{
public:
  static int register_handler (int signum, 
			       Event_Handler *new_sh, 
			       Sig_Action *new_disp = 0, 
			       Event_Handler **old_sh = 0,
			       Sig_Action *old_disp = 0);
  // Register a new Event_Handler and a new sigaction associated
  // with SIGNUM.  Return the existing Event_Handler and its
  // sigaction if pointers are non-zero. 
  
  static int remove_handler (int signum, 
			     Sig_Action *new_disp = 0,
			     Sig_Action *old_disp = 0);
  // Remove the Event_Handler currently associated with SIGNUM.
  // Install the new disposition (if given) and return the 
  // previous disposition (if desired by the caller). 

  static sig_atomic_t sig_pending (void);
  // True if there is a pending signal. 

  static void clear_sig_pending (void);
  // Reset the value of SIG_PENDING_ so that no signal is pending. 

  static Event_Handler *handler (int signum);
  // Return the Event_Handler associated with SIGNUM. 
  
  static Event_Handler *handler (int signum, Event_Handler *);
  // Set a new Event_Handler that is associated with SIGNUM.
  // Return the existing handler. 

private:
  /* Callback routine registered with sigaction(2) that dispatches the 
     handle_signal() method of the appropriate pre-registered Event_Handler. */ 
#if defined (ACE_HAS_SIGINFO_T)
  static void dispatch (int, siginfo_t *, ucontext_t *);
#else
  static void dispatch (int);
#endif /* ACE_HAS_SIGINFO_T */

  static int in_range (int signum);
  // Check whether the SIGNUM is within the legal range of signals. 

  static sig_atomic_t sig_pending_;
  // Keeps track of whether a signal is pending. 
  
  static Event_Handler *signal_handlers_[NSIG];
  // Array used to store user-defined Event_Handlers. 
};

#if defined (__INLINE__)
#define INLINE inline
#include "Signal.i"
#else
#define INLINE
#endif /* __INLINE__ */

#endif /* ACE_SIGNAL_HANDLER_H */
