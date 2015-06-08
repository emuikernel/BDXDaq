/* -*- C++ -*- */
/* Defines the base class for the ``address family independent'' address format. */

/* Initializes local variables. */

INLINE void
Addr::base_set (int type, int size)
{
  this->addr_type_ = type;
  this->addr_size_ = size;
}

INLINE
Addr::Addr (void)
{
  this->base_set (0, 0); /* Note that 0 is an unspecified protocol family type... */
}

/* Initializes local variables. */

INLINE
Addr::Addr (int type, int size)
{
  this->base_set (type, size);
}

/* Return the address of the address.  Note that
   the only reason this is here is to enable
   us to define the sap_any object...  */

INLINE void *
Addr::get_addr (void) const
{
  return 0;
}

/* Transform the current address into string format. */

INLINE int
Addr::addr_to_string (char [], size_t) const
{
  return -1;
}

INLINE int 
Addr::operator == (const Addr &sap) const
{
  return sap.addr_type_ == 0;
}

INLINE int
Addr::operator != (const Addr &sap) const
{
  return sap.addr_type_ != 0;
}

/* Return the size of the address. */

INLINE int
Addr::get_size (void) const
{
  return this->addr_size_;
}

/* Sets the size of the address. */

INLINE void
Addr::set_size (int size)
{
  this->addr_size_ = size;
}

/* Return the type of the address. */

INLINE int
Addr::get_type (void) const
{
  return this->addr_type_;
}

/* Return the type of the address. */

INLINE void
Addr::set_type (int type)
{
  this->addr_type_ = type;
}
