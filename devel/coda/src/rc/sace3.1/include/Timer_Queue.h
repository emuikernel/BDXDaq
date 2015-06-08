/* -*- C++ -*- */
/* Interface to timers. */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Timer_Queue.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_TIMER_QUEUE_H)
#define ACE_TIMER_QUEUE_H

#include "Event_Handler.h"
#include "Time_Value.h"

#if defined (ACE_MT_SAFE)
#include "Synch.h"
#endif /* ACE_MT_SAFE */

/* Note, this should be a nested class, but some compilers don't like these yet... */

struct Timer_Node
  // = TITLE
  //
  //
  // = DESCRIPTION
  //
{
friend class Timer_Queue;
private:
  Timer_Node (Event_Handler *h, const void *a, const Time_Value &t, const Time_Value &i, Timer_Node *n);

  Event_Handler *handler_;
  const void          *arg_;
  Time_Value	      timer_value_;
  Time_Value	      interval_;
  Timer_Node	      *next_;
};

class Timer_Queue
  // = TITLE
  //
  //
  // = DESCRIPTION
  // 
{
public: 
  Timer_Queue (void);
  virtual ~Timer_Queue (void);

  int 			  is_empty (void) const;
  const Time_Value        &earliest_time (void) const;
  static const Time_Value &current_time (void);

  int schedule (Event_Handler *, const void *a, const Time_Value &, const Time_Value & = Time_Value::zero);
  int cancel (Event_Handler *);
  int expire (const Time_Value &);

private:
  void reschedule (Timer_Node *);

  Timer_Node *head_; // Pointer to linked list of Timer_Nodes. 


#if defined (ACE_MT_SAFE)
  Recursive_Lock<Mutex> lock_; // Synchronization variable for the MT_SAFE Reactor 
#endif /* ACE_MT_SAFE */
};

#if defined (__INLINE__)
#define INLINE inline
#include "Timer_Queue.i"
#else
#define INLINE
#endif /* __INLINE__ */

#endif /* ACE_TIMER_QUEUE_H */
