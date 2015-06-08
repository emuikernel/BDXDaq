/* -*- C++ -*- */
/* A simple trace facility for C++. */

#include "Log_Msg.h"

/* Enable the tracing facility */

INLINE void 
Trace::start_tracing (void) 
{ 
  Trace::enable_tracing_ = 1;
}

/* Disable the tracing facility. */

INLINE void 
Trace::stop_tracing (void)  
{ 
  Trace::enable_tracing_ = 0; 
}

/* Change the nesting indentation level. */

INLINE void 
Trace::set_nesting_indent (int indent) 
{ 
  nesting_indent_ = indent; 
}

/* Perform the first part of the trace, which prints out 
   the string N, the LINE, and the FILE as the function 
   is entered. */

INLINE
Trace::Trace (char *n, int line, char *file)
{
  if (Trace::enable_tracing_)
    Log_Msg::log (LOG_INFO, "%*s(%t) calling %s in file `%s' on line %d\n",
		  nesting_indent_ * nesting_depth_++, "", this->name_ = n, file, line);
}

/* Perform the second part of the trace, which prints out 
   the NAME as the function is exited. */

INLINE
Trace::~Trace (void) 
{
  if (Trace::enable_tracing_)
    Log_Msg::log (LOG_INFO, "%*s(%t) leaving %s\n", 
		  nesting_indent_ * --nesting_depth_, "", this->name_);
}


/* Sergey: Trace.i has problems if it defined; do we need it !!! */
#undef _REENTRANT

INLINE
Trace::Trace (void)
{
#if defined (_REENTRANT)
  if (Trace::once_ == 0)
    {
      this->name_ = "static dummy";
      Trace::once_ = 1;
      ::thr_keycreate (&Trace::depth_key_, Trace::cleanup);
      ::thr_keycreate (&Trace::indent_key_, Trace::cleanup);
    }
#endif /* _REENTRANT */
}

#if defined (_REENTRANT)
void
Trace::cleanup (void *ptr)
{
  Trace::stop_tracing ();
  delete ptr;
}

int *
Trace::___nesting_depth (void)
{
  int *ip = 0;

  ::thr_getspecific (Trace::depth_key_, (void **) &ip);
  if (ip == 0)
    {
      ip = new int (Trace::DEFAULT_DEPTH);
      ::thr_setspecific (Trace::depth_key_, (void *) ip);
    }
  return ip;
}

int *
Trace::___nesting_indent (void)
{
  int *ip = 0;

  ::thr_getspecific (Trace::indent_key_, (void **) &ip);
  if (ip == 0) // First time in
    {
      ip = new int (Trace::DEFAULT_INDENT); 
      ::thr_setspecific (Trace::indent_key_, (void *) ip);
    }
  return ip;
}

#endif /* _REENTRANT */


