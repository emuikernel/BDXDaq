/* -*- C++ -*- */
// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Addr.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_ADDR_H)
#define ACE_ADDR_H

#include "sysincludes.h"

class Addr 
  //  = TITLE
  //     Defines the base class for the "address family independent" address
  //     format.
  // 
  // = DESCRIPTION
  // 
{
public:
  Addr (void);
  Addr (int type, int size);

  // = Get/set the size of the address. 
  int get_size (void) const;
  void set_size (int size);

  // = Get/set the type of the address. 
  int get_type (void) const;
  void set_type (int type);

  virtual void *get_addr (void) const;
  // Return the address of the address.

  virtual int addr_to_string (char addr[], size_t) const;
  // Transform the current address into string format. 

  // = Equality/inequality tests
  virtual int operator == (const Addr &sap) const;
  virtual int operator != (const Addr &sap) const;

  void base_set (int type, int size);
  // Initializes local variables. 

protected:
  int addr_type_; // e.g. AF_UNIX or AF_INET. 
  int addr_size_; // Number of bytes in the address.
};

extern const Addr sap_any;

#if defined (__INLINE__)
#define INLINE inline
#include "Addr.i"
#else
#define INLINE 
#endif /* __INLINE__ */
#endif /* _ADDR_H */
