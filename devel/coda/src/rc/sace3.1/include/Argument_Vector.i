/* Transforms a string BUF into an ARGV-style vector of strings. */

#include "Log_Msg.h"

inline size_t
Argument_Vector::count (void) const
{
  return this->argc_;
}

inline char *
Argument_Vector::operator[] (int i) const
{
  return this->argv_[i];
}

inline char **
Argument_Vector::operator& (void) const
{
  return this->argv_;
}
