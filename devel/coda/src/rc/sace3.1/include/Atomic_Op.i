#include "Synch.h"

template <class LOCK, class TYPE> inline 
Atomic_Op<LOCK, TYPE>::Atomic_Op (void) 
{ 
  this->count_ = 0; 
}

template <class LOCK, class TYPE> inline 
Atomic_Op<LOCK, TYPE>::Atomic_Op (TYPE c) 
{ 
  this->count_ = c; 
}

template <class LOCK, class TYPE> inline TYPE 
Atomic_Op<LOCK, TYPE>::operator++ (void)
{
  Guard<LOCK> m (this->lock_);
  return ++this->count_;
}

template <class LOCK, class TYPE> inline TYPE
Atomic_Op<LOCK, TYPE>::operator+= (const TYPE i)
{
  Guard<LOCK> m (this->lock_);
  return this->count_ += i;
}

template <class LOCK, class TYPE> inline TYPE
Atomic_Op<LOCK, TYPE>::operator-- (void)
{
  Guard<LOCK> m (this->lock_);
  return --this->count_;
}

template <class LOCK, class TYPE> inline TYPE
Atomic_Op<LOCK, TYPE>::operator-= (const TYPE i)
{
  Guard<LOCK> m (this->lock_);
  return this->count_ -= i;
}

template <class LOCK, class TYPE> inline TYPE
Atomic_Op<LOCK, TYPE>::operator== (const TYPE i)
{
  Guard<LOCK> m (this->lock_);
  return this->count_ == i;
}

template <class LOCK, class TYPE> inline TYPE
Atomic_Op<LOCK, TYPE>::operator>= (const TYPE i)
{
  Guard<LOCK> m (this->lock_);
  return this->count_ >= i;
}

template <class LOCK, class TYPE> inline TYPE 
Atomic_Op<LOCK, TYPE>::operator> (const TYPE rhs)
{
  Guard<LOCK> m (this->lock_);
  return this->count_ > rhs;
}

template <class LOCK, class TYPE> inline TYPE 
Atomic_Op<LOCK, TYPE>::operator<= (const TYPE rhs)
{
  Guard<LOCK> m (this->lock_);
  return this->count_ <= rhs;
}

template <class LOCK, class TYPE> inline TYPE 
Atomic_Op<LOCK, TYPE>::operator< (const TYPE rhs)
{
  Guard<LOCK> m (this->lock_);
  return this->count_ < rhs;
}

template <class LOCK, class TYPE> inline void
Atomic_Op<LOCK, TYPE>::operator= (const TYPE i)
{
  Guard<LOCK> m (this->lock_);
  this->count_ = i;
}

template <class LOCK, class TYPE> inline
Atomic_Op<LOCK, TYPE>::operator TYPE ()
{
  Guard<LOCK> m (this->lock_);
  return this->count_;    
}
