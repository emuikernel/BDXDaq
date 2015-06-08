/* Interface to timers. */

#include "Timer_Queue.h"

#if !defined (__INLINE__)
#include "Timer_Queue.i"
#endif /* __INLINE__ */

#if defined (DEBUG)
#include <assert.h>

class Example_Handler : public IO_Handler
{
public:
  virtual void timer_expired (Time_Value tv)
  {
    static int i = 0;

    printf ("yow, the time has come and gone %d times, Horatio!\n", i++);
  }
};

int
main (int argc, char *argv[])
{
  Timer_Queue tq;
  Example_Handler eh;

  assert (tq.is_empty ());
  assert (Timer_Queue::zero_time () == Time_Value (0));
  tq.insert (Timer_Queue::current_time (), &eh);
  tq.insert (Timer_Queue::current_time (), &eh);
  tq.insert (Timer_Queue::current_time (), &eh);
  assert (!tq.is_empty ());
  tq.expire (Timer_Queue::current_time ());
}
#endif /* DEBUG */


