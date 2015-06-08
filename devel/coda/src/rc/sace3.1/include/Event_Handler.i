#include "Event_Handler.h"

/* Implement conceptually abstract virtual functions in the base class
   so derived classes don't have to implement unused ones. */

INLINE
Event_Handler::Event_Handler (void) 
{
}

INLINE
Event_Handler::~Event_Handler (void) 
{
}

/* Gets the file descriptor associated with this I/O device. */

INLINE HANDLE
Event_Handler::get_handle (void) const
{
  return -1;
}

/* Sets the file descriptor associated with this I/O device. */

INLINE void
Event_Handler::set_handle (HANDLE)
{
}

/* Called when the object is about to be removed from the
   Dispatcher tables. */

INLINE int
Event_Handler::handle_close (HANDLE, Reactor_Mask)
{
  return -1;
}

/* Called when input becomes available on fd. */

INLINE int 
Event_Handler::handle_input (HANDLE)
{
  return -1;
}

/* Called when output is possible on fd. */

INLINE int 
Event_Handler::handle_output (HANDLE)
{
  return -1;
}

/* Called when urgent data is available on fd. */

INLINE int 
Event_Handler::handle_exception (HANDLE)
{
  return -1;
}

/* Called when timer expires, TV stores the current time. */

INLINE int
Event_Handler::handle_timeout (const Time_Value &, const void *)
{
  return -1;
}

/* Called when a registered signal occurs */

INLINE int 
Event_Handler::handle_signal (HANDLE
#if defined (ACE_HAS_SIGINFO_T)
, siginfo_t *, ucontext_t *
#endif /* ACE_HAS_SIGINFO_T */
)
{
  return -1;
}

