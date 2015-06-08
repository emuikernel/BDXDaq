/* -*- C++ -*- */

/* The following 9 methods provide a C++ wrapper for the C sigset_t API. */

INLINE
Sig_Set::Sig_Set (sigset_t *ss): sigset_ (*ss) // Structure assignment
{
}

INLINE
Sig_Set::Sig_Set (int fill)
{
#ifdef Darwin
  if (fill)
    sigfillset (&this->sigset_);
  else
    sigemptyset (&this->sigset_);
#else
  if (fill)
    ::sigfillset (&this->sigset_);
  else
    ::sigemptyset (&this->sigset_);
#endif
}

INLINE
Sig_Set::~Sig_Set (void)
{
#ifdef Darwin
  sigemptyset (&this->sigset_);
#else
  ::sigemptyset (&this->sigset_);
#endif
}

INLINE int
Sig_Set::emptyset (void)
{
#ifdef Darwin
  return sigemptyset (&this->sigset_);  
#else
  return ::sigemptyset (&this->sigset_);  
#endif
}

INLINE int
Sig_Set::fillset (void)
{
#ifdef Darwin
  return sigfillset (&this->sigset_);
#else
  return ::sigfillset (&this->sigset_);
#endif
}

INLINE int
Sig_Set::addset (int signo)
{
#ifdef Darwin
  return sigaddset (&this->sigset_, signo);
#else
  return ::sigaddset (&this->sigset_, signo);
#endif
}

INLINE int
Sig_Set::delset (int signo)
{
#ifdef Darwin
  return sigdelset (&this->sigset_, signo);
#else
  return ::sigdelset (&this->sigset_, signo);
#endif
}

INLINE int
Sig_Set::ismember (int signo) const
{
#ifdef Darwin
  return sigismember ((sigset_t *) &this->sigset_, signo);
#else
  return ::sigismember ((sigset_t *) &this->sigset_, signo);
#endif
}

INLINE
Sig_Set::operator sigset_t *(void)
{
  return &this->sigset_;
}

INLINE int
Sig_Action::flags (void)
{
  return this->sa_.sa_flags;
}

INLINE void
Sig_Action::flags (int flags)
{
  this->sa_.sa_flags = flags;
}

INLINE sigset_t *
Sig_Action::mask (void)
{
  return &this->sa_.sa_mask;
}

INLINE void
Sig_Action::mask (sigset_t *ss)
{
  this->sa_.sa_mask = *ss; // Structure assignment
}

INLINE SignalHandler
Sig_Action::handler (void)
{
  return SignalHandler (this->sa_.sa_handler);
}

INLINE void
Sig_Action::handler (SignalHandler handler)
{
  this->sa_.sa_handler = SignalHandlerV (handler);
}

INLINE struct sigaction *
Sig_Action::get (void)
{
  return &this->sa_;
}

INLINE void
Sig_Action::set (struct sigaction *sa)
{
  this->sa_ = *sa; // Structure assignment.
}

INLINE int
Sig_Action::register_action (int signum, Sig_Action *oaction)
{
  struct sigaction *sa = oaction == 0 ? 0 : oaction->get ();
  
  return ::sigaction (signum, &this->sa_, sa);
}

INLINE int
Sig_Action::restore_action (int signum, Sig_Action *oaction)
{
  if (oaction != 0)
    {
      this->sa_ = *oaction->get (); // Structure assignment
      return ::sigaction (signum, &this->sa_, 0);
    }
  return 0;
}

/* Block out the signal MASK until the destructor is called. */

INLINE 
Signal_Guard::Signal_Guard (Sig_Set *mask)
{
  // If MASK is 0 then block all signals! 
  if (mask == 0)
    {
      Sig_Set smask (1);

      ::sigprocmask (SIG_BLOCK, (sigset_t *) smask, (sigset_t *) this->omask_);
    }
  else
    ::sigprocmask (SIG_BLOCK, (sigset_t *) *mask, (sigset_t *) this->omask_);
}

/* Restore the signal mask. */

INLINE 
Signal_Guard::~Signal_Guard (void)
{
  ::sigprocmask (SIG_SETMASK, (sigset_t *) this->omask_, 0);
}

INLINE sig_atomic_t
Signal_Handler::sig_pending (void)
{
  return Signal_Handler::sig_pending_;
}

INLINE void 
Signal_Handler::clear_sig_pending (void)
{
  Signal_Handler::sig_pending_ = 0;
}

INLINE int
Signal_Handler::in_range (int signum)
{
  return signum > 0 && signum < NSIG;
}

INLINE Event_Handler *
Signal_Handler::handler (int signum)
{
  if (Signal_Handler::in_range (signum))
    return Signal_Handler::signal_handlers_[signum];
  else
    return 0;
}

INLINE Event_Handler *
Signal_Handler::handler (int signum, Event_Handler *new_sh)
{
  if (Signal_Handler::in_range (signum))
    {
      Event_Handler *sh = Signal_Handler::signal_handlers_[signum];

      Signal_Handler::signal_handlers_[signum] = new_sh;    
      return sh;
    }
  else
    return 0;
}
