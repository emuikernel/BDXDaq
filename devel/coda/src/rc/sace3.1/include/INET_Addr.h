/* -*- C++ -*- */
// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    INET_Addr.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_INET_ADDR_H)
#define ACE_INET_ADDR_H
#include "Addr.h"

class INET_Addr : public Addr
  // = TITLE
  //    Defines the Internet domain address family address format. 
  //
  //  = DESCRIPTION
  //

{
public:
  INET_Addr (void);
  INET_Addr (const INET_Addr &);
  INET_Addr (const sockaddr_in *, int len);
  INET_Addr (unsigned short port_number, const char host_name[]);
  INET_Addr (unsigned short port_number, long ip_addr = INADDR_ANY);
  INET_Addr (const char port_name[], const char host_name[]);
  INET_Addr (const char port_name[], long ip_addr = INADDR_ANY);

  int set (unsigned short port_number, const char host_name[], int encode = 1);
  int set (unsigned short port_number, long ip_addr = INADDR_ANY, int encode = 1);
  int set (const char port_name[], const char host_name[]);
  int set (const char port_name[], long ip_addr = INADDR_ANY);
  int set (const sockaddr_in *, int len);

  virtual void   *get_addr (void) const;
  virtual int	 addr_to_string (char addr[], size_t) const;
  virtual int    operator == (const Addr &SAP) const;
  virtual int    operator != (const Addr &SAP) const;

  const char     *get_host_name (void) const;
  const	char     *get_host_addr (void) const;
  unsigned long  get_ip_address (void) const;
  unsigned short get_port_number (void) const;

private:
  sockaddr_in inet_addr_;
};

#if defined (__INLINE__)
#define INLINE inline
#include "INET_Addr.i"
#else
#define INLINE 
#endif /* __INLINE__ */

#endif /* _INET_ADDR_H */
