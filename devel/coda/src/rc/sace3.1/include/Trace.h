/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Trace.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_TRACE_H)
#define ACE_TRACE_H

#include "sysincludes.h"

/*sergey: only pthread_t defined here ???*/
#ifdef Darwin
typedef pthread_key_t thread_key_t;
#endif

#if !defined ACE_NTRACE
#define T(X) Trace ____ (X, __LINE__, __FILE__)
#else
#define T(X)
#endif /* ACE_NTRACE */

class Trace
  // = TITLE
  //     A simple trace facility for C++. 
  //
  // = DESCRIPTION
  //
{
public:
  Trace (char *n, int line = 0, char *file = "");
  ~Trace (void);

  static void start_tracing (void);
  static void stop_tracing (void);
  static void set_nesting_indent (int indent);

  Trace (void);
private:
#if defined (_REENTRANT)
  static thread_key_t depth_key_;
  static thread_key_t indent_key_;
  static int	      once_;
  static Trace	      t_; // Dummy used to initialize the keys 

  static void	      cleanup (void *);
  static int	      *___nesting_indent();
  static int	      *___nesting_depth();
#define	nesting_indent_ (*(___nesting_indent()))
#define	nesting_depth_ (*(___nesting_depth()))
#else
  static int nesting_depth_;
  static int nesting_indent_;
#endif /* _REENTRANT */
  static int enable_tracing_;
  
  char *name_;
  enum 
  {
    DEFAULT_DEPTH   = 0,
    DEFAULT_INDENT  = 3,
    DEFAULT_TRACING = 0
  };
};

#if defined (__INLINE__)
#define INLINE inline
#include "Trace.i"
#else
#define INLINE
#endif /* __INLINE__ */
#endif /* ACE_TRACE_H */
