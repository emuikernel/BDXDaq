#include "Signal.h"

#if !defined (__INLINE__)
#include "Signal.i"
#endif /* __INLINE__ */

/* Array of Event_Handlers that will handle the signals. */
Event_Handler *Signal_Handler::signal_handlers_[NSIG];

/* Remembers if a signal has occurred. */
sig_atomic_t Signal_Handler::sig_pending_ = 0;

Sig_Action::Sig_Action (int sig_flags, sigset_t *sig_mask, SignalHandler sig_handler)
{
  this->sa_.sa_flags = sig_flags;
  if (sig_mask == 0)
#ifdef Darwin
    sigemptyset (&this->sa_.sa_mask);
#else
    ::sigemptyset (&this->sa_.sa_mask);
#endif
  else
    this->sa_.sa_mask = *sig_mask; // Structure assignment...
  this->sa_.sa_handler = SignalHandlerV (sig_handler);
}
/* Master dispatcher function that gets called by a signal handler. */

void
Signal_Handler::dispatch (int signum
#if defined (ACE_HAS_SIGINFO_T)
, siginfo_t *siginfo, ucontext_t *ucontext
#endif /* ACE_HAS_SIGINFO_T */
)
{
  Signal_Handler::sig_pending_ = 1;

  Event_Handler *eh = (Event_Handler *) Signal_Handler::handler (signum);

  if (eh != 0 && 
#if defined (ACE_HAS_SIGINFO_T)
      eh->handle_signal (signum, siginfo, ucontext) < 0)
#else
      eh->handle_signal (signum) < 0)
#endif /* ACE_HAS_SIGINFO_T */
    {
      Sig_Action sa;

      Signal_Handler::handler (signum, 0);
      sa.register_action (signum);
    }
}

/* Register an Event_Handler along with the corresponding SIGNUM. */

int
Signal_Handler::register_handler (int signum, 
				  Event_Handler *new_sh, 
				  Sig_Action *new_disp, 
				  Event_Handler **old_sh,
				  Sig_Action *old_disp)
{
  if (Signal_Handler::in_range (signum))
    {
      Sig_Action sa;
      Event_Handler *sh = Signal_Handler::handler (signum, new_sh);

      // Stack the old Signal_Handler if the user gives us a pointer to a object.
      if (old_sh != 0)
	*old_sh = sh;

      // Make sure that new_disp points to a valid location if the user doesn't care...
      if (new_disp == 0)
        new_disp = &sa;
  
      new_disp->handler (SignalHandler (Signal_Handler::dispatch));
#if defined (ACE_HAS_SIGINFO_T)
      new_disp->flags (new_disp->flags () | SA_SIGINFO);
#endif /* ACE_HAS_SIGINFO_T */
      return new_disp->register_action (signum, old_disp);
    }
  else
    return -1;
}

/* Remove an Event_Handler along. */

int
Signal_Handler::remove_handler (int signum, 
			        Sig_Action *new_disp,
				Sig_Action *old_disp)
{
  if (Signal_Handler::in_range (signum))
    {
      if (new_disp == 0)
	{
	  Sig_Action sa;
	  new_disp = &sa;
	}

      Signal_Handler::handler (signum, 0);
      return new_disp->register_action (signum, old_disp);
    }
  else
    return -1;
}

#if defined (DEBUGGING)
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

class My_Handler : public Event_Handler
{
public:
  My_Handler (void);
  virtual int handle_signal (int signum, siginfo_t *siginfo, ucontext_t *ucontext);  

  /* Addr that we are going to manipulate... */
  char *addr;
};

/* Constructor for My_Handler, which registers this
   object with the Signal_Handler. */

My_Handler::My_Handler (void): addr (0)
{
  if (Signal_Handler::register_handler (SIGSEGV, this) == -1)
    perror ("Signal_Handler::register_handler"), exit (1);
}

/* Called by the Signal_Handler::dispatch() function when a 
   signal of interest occurs. */

int 
My_Handler::handle_signal (int signum, siginfo_t *siginfo, ucontext_t *ucontext)
{
  fprintf (stderr, "signal %d occurred\n", signum);

  if (siginfo != 0
      && siginfo->si_signo == SIGSEGV
      && siginfo->si_code != SI_NOINFO
      && siginfo->si_code == SEGV_MAPERR)
    {
      fprintf (stderr, "this->addr = %u, faulting address = %u!\n", 
	       this->addr, siginfo->si_addr);
      
      if (siginfo->si_addr == this->addr)
	{
	  this->addr = "hello world\n";
	  return 0;
	}
      else
	return -1;
    }
}

int 
main (int argc, char *argv)
{
  My_Handler handler;
  
  /* Dereference a NULL pointer. */
  char c = *handler.addr;
  return 0;
}
#endif /* DEBUGGING */
