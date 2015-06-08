/* -*- C++ -*- */
/* Define a simple "mark and release" memory allocation utility */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Obstack.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_OBSTACK_H)
#define ACE_OBSTACK_H

#include "sysincludes.h"

/* This should be a nested class but some compilers don't like them yet */
class Obchunk
  // = TITLE
  //
  //
  // = DESCRIPTION
  //
{
friend class Obstack;
private:
  char  *end;
  char  *cur;
  Obchunk *next;
  char  contents[4];
};

class Obstack
  // = TITLE
  //
  //
  // = DESCRIPTION
  //
{
public:
  Obstack (int size = 4080);
  ~Obstack (void);

  char *copy (const char* data, size_t len);
  void release (void);

protected:
  size_t size;
  class Obchunk  *head;
  class Obchunk  *curr;

  class Obchunk  *new_chunk (void);
};

#endif /* ACE_OBSTACK_H */
