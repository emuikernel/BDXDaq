/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Atomic_Op.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_ATOMIC_OP)
#define ACE_ATOMIC_OP

template <class LOCK, class TYPE>
class Atomic_Op
  // = TITLE
  // Transparently parameterizes synchronization into basic arithmetic
  // operations.
  //
  // = DESCRIPTION
  //  This class is described in an article in the July/August 1994
  //  issue of the C++ Report magazine. 
  //
{
public:
  inline Atomic_Op (void);
  // Initialize <count_> to 0.

  inline Atomic_Op (TYPE c);
  // Initialize <count_> to c.

  inline TYPE operator++ (void);
  // Atomically increment <count_>.

  inline TYPE operator+= (const TYPE i);
  // Atomically increment <count_> by inc.

  inline TYPE operator-- (void);
  // Atomically decrement <count_>.

  inline TYPE operator-= (const TYPE i);
  // Atomically decrement <count_> by dec.

  inline TYPE operator== (const TYPE i);
  // Atomically compare <count_> with rhs.

  inline TYPE operator>= (const TYPE i);
  // Atomically check if <count_> greater than or equal to rhs.

  TYPE operator> (const TYPE rhs);
  // Atomically check if <count_> greater than rhs.

  TYPE operator<= (const TYPE rhs);
  // Atomically check if <count_> less than or equal to rhs.

  TYPE operator< (const TYPE rhs);
  // Atomically check if <count_> less than rhs.

  inline void operator= (const TYPE i);
  // Atomically assign rhs to <count_>.

  inline operator TYPE ();
  // Atomically return <count_>.

private:
  LOCK lock_;
  TYPE  count_;
};

#include "Atomic_Op.i"
#endif /* ACE_ATOMIC_OP */
