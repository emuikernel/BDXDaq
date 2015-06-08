/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Argument_Vector.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_ARGUMENT_VECTOR_H)
#define ACE_ARGUMENT_VECTOR_H

#include "sysincludes.h"

class Argument_Vector
  // = TITLE
  //     Transforms a string BUF into an ARGV-style vector of strings. 
  //
  // = DESCRIPTION
  //
{
public:
  Argument_Vector (char buf[]);
  // Converts BUF into an ARGV-style vector of strings. 

  ~Argument_Vector (void);

  char *operator[] (int index) const;
  // Returns the INDEXth string in the ARGV array. 

  char **operator& (void) const;
  // Returns ARGV. 

  size_t count (void) const;
  // Returns ARGC. 

private:
  size_t argc_;
  // Number of arguments in the ARGV array. 

  char **argv_;
  // The array of string arguments. 
};

#include "Argument_Vector.i"

#endif /* ACE_ARGUMENT_VECTOR_H */
