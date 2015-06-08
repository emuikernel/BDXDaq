#include "Time_Value.h"

#if !defined (__INLINE__)
#include "Time_Value.i"
#endif /* __INLINE__ */

/* Static constant representing `zero-time'. */

const Time_Value Time_Value::zero;
const Time_Value *Time_Value::zerop = &Time_Value::zero;

#if defined (DEBUG)
#include <assert.h>

int
main (int argc, char *argv[])
{
  Time_Value tv1;
  Time_Value tv2 (2);
  Time_Value tv3 (100);  
  Time_Value tv4 (1, 1000000);
  Time_Value tv5 (2);
  Time_Value tv6 (1, -1000000);

  assert (tv1 == Time_Value (0));
  assert (tv2 < tv3);
  assert (tv2 <= tv2);
  assert (tv2 >= tv4);
  assert (tv5 >= tv6);
  assert (tv2 == Time_Value (1, 1000000));
  assert (tv5 == tv4);
  assert (tv2 == tv4);
  assert (tv1 != tv2);
  assert (tv6 == tv1);
}
#endif /* DEBUG */
