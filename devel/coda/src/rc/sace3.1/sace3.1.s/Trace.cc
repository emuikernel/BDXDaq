#include "Trace.h"

#if defined (_REENTRANT)
thread_key_t Trace::depth_key_ = 0;
thread_key_t Trace::indent_key_ = 0;
int	     Trace::once_ = 0;
Trace	     Trace::t_;
#else
int Trace::nesting_depth_  = Trace::DEFAULT_DEPTH;
int Trace::nesting_indent_ = Trace::DEFAULT_INDENT;
#endif /* _REENTRANT */
int Trace::enable_tracing_ = Trace::DEFAULT_TRACING;

#if !defined (__INLINE__)
#include "Trace.i"
#endif /* __INLINE__ */

#ifdef DEBUG
#include <libc.h>

void
foo (int max_depth)
{
  T ("void foo (void)");
  static int i = 0;

  if (i++ < max_depth)
    foo (max_depth);
  else
    i = 0;
  /* Destructor automatically called */
}

int 
main (int argc, char *argv[])
{
  const int MAX_DEPTH = argc == 1 ? 10 : atoi (argv[1]);

  if (argc > 2)
    Trace::set_nesting_indent (atoi (argv[2]));
  if (argc > 3)
    Trace::stop_tracing ();

  T ("int main (int argc, char *argv[])");

  signal (SIGUSR1, SignalHandler (Trace::start_tracing));
  signal (SIGUSR2, SignalHandler (Trace::stop_tracing));

  for (;;)
    {
      foo (MAX_DEPTH);
      sigpause (0);
    }

  /* Destructor automatically called */
}
#endif /* DEBUG */
