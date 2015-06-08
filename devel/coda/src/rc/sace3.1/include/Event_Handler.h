/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Event_Handler.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_EVENT_HANDLER_H)
#define ACE_EVENT_HANDLER_H

#include "Time_Value.h"

typedef unsigned long Reactor_Mask;

class Event_Handler 
  // = TITLE
  //     Derived classes read input on a file number, write output on a file
  // number, handle an exception raised on a file number, or handle a
  // timer's expiration. 
  //
  // = DESCRIPTION
  //
{
public:
  enum 
  {
    NULL_MASK   = 0,
#if defined (ACE_USE_POLL_IMPLEMENTATION)
    READ_MASK   = POLLIN,
    WRITE_MASK  = POLLOUT,
    EXCEPT_MASK = POLLPRI,
    RWE_MASK    = READ_MASK | WRITE_MASK | EXCEPT_MASK,
    DONT_CALL	= 0x100 // Make sure this number is unique wrt RWE_MASK!
#else
    READ_MASK   = 0x1,
    WRITE_MASK  = 0x4,
    EXCEPT_MASK = 0x2,
#if defined (__GNUG__)
    RWE_MASK    = 0x1 | 0x2 | 0x4,
#else
    RWE_MASK    = READ_MASK | WRITE_MASK | EXCEPT_MASK,
#endif				/* __GNUG__ */
    DONT_CALL   = 0x100
#endif				/* ACE_USE_POLL_IMPLEMENTATION */
  };

  virtual ~Event_Handler (void);
  
  virtual HANDLE  get_handle (void) const;
  virtual void    set_handle (HANDLE);
  virtual int	  handle_input (HANDLE fd = -1);
  virtual int	  handle_output (HANDLE fd = -1);
  virtual int	  handle_exception (HANDLE fd = -1);
  virtual int	  handle_timeout (const Time_Value &tv, const void *arg = 0);
  virtual int	  handle_close (HANDLE fd, Reactor_Mask close_mask);
  virtual int	  handle_signal (HANDLE signum
#if defined (ACE_HAS_SIGINFO_T)
, siginfo_t * = 0, ucontext_t * = 0
#endif /* ACE_HAS_SIGINFO_T */
);

protected:
  Event_Handler (void);
  // Force Event_Handler to be an abstract base class 
};
#endif /* ACE_EVENT_HANDLER_H */
