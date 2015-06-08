/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    UNIX_Addr.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_UNIX_ADDR_H)
#define ACE_UNIX_ADDR_H
#include "Addr.h"

class UNIX_Addr : public Addr
  // = TITLE
  //    Defines the ``UNIX domain address family'' address format. 
  //
  // = DESCRIPTION
  // 
{
public:
  UNIX_Addr (void);
  UNIX_Addr (const UNIX_Addr &sa);
  UNIX_Addr (const char rendezvous_point[]);
  UNIX_Addr (const sockaddr_un *, int len);

  void set (const char rendezvous_point[]);
  void set (const sockaddr_un *, int len);

  virtual void *get_addr (void) const;
  virtual int addr_to_string (char addr[], size_t) const;
  virtual int  operator == (const Addr &SAP) const;
  virtual int  operator != (const Addr &SAP) const;

  const char *get_path_name (void) const; 

private:
  sockaddr_un unix_addr_;
};

#if defined (__INLINE__)
#define INLINE inline
#include "UNIX_Addr.i"
#else
#define INLINE 
#endif /* __INLINE__ */

#endif /* _UNIX_ADDR_H */
