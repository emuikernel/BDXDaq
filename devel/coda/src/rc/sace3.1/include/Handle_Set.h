/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Handle_Set.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_FD_SET_H)
#define ACE_FD_SET_H

#include "sysincludes.h"

/* This wrapper design is not very portable to DEC OSF/1	*/
/* I had to redefine NFDBITS to 32. On OSF/1 NFDBITS is a 	*/
/* macro that expands to (sizeof(fd_mask)*8) which is 4096 by	*/
/* default. This was an inappropriate value for defining the 	*/
/* MSB_MASK default value. Any ideas?	The workaround is a 	*/
/* pretty severe restriction for OSF/1.		DJT		*/
/*#if defined (__osf__)						*/
/*#define NFDBITS 32						*/
/*#endif							*/

class Handle_Set 
{
  // = TITLE
  //     C++ wrapper for the BSD fd_set abstraction. 
  //
  // = DESCRIPTION
  //

  friend class Handle_Set_Iterator;
public:
  // = Initialization and termination. 

  Handle_Set (void);
  Handle_Set (const fd_set &mask);

  // = Methods for manipulating bitsets.
  void reset (void);
  int  is_set (int) const;
  void set_bit (int);
  void clr_bit (int);
  int  num_set (void) const;
  int  max_set (void) const;
  int  pr_mask (void);
  void sync (int max = FD_SETSIZE);
  operator fd_set *();

private:
  int	 size_;
  int	 max_handle_;
  fd_set mask_;

  enum 
  {
    MAX_SIZE  = NOFILE,
    WORD_SIZE = NFDBITS,
    NUM_WORDS = howmany (NOFILE, NFDBITS),
    MSB_MASK	= ~(1 << (NFDBITS - 1))
  };

  int  count_bits (unsigned long n) const;
  void set_max (int max);

  static const char nbits_[256];
};

class Handle_Set_Iterator
  // = TITLE
  //  Iterator for the Handle_Set abstraction. 
  //
  // = DESCRIPTION
  //
{
public:
  Handle_Set_Iterator (Handle_Set &);
  int operator ()(void);
  void operator++ (void);

private:
  Handle_Set  &fds_;
  int	  index_;
  int	  num_; 
  fd_mask val_;
};

#include "Handle_Set.i"
#endif /* ACE_FD_SET */
