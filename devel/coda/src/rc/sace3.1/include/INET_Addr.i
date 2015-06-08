/* -*- C++ -*- */
/* Defines the Internet domain address family address format. */

#include "Log_Msg.h"

INLINE
INET_Addr::INET_Addr (void): Addr (AF_INET, sizeof this->inet_addr_)
{
  (void) ::memset ((void *) &this->inet_addr_, 
		   0, sizeof this->inet_addr_);
}

/* Copy constructor. */

INLINE
INET_Addr::INET_Addr (const INET_Addr &sa)
  : Addr (AF_INET, sizeof this->inet_addr_)
{
  (void) ::memcpy ((void *) &this->inet_addr_, 
		   (void *) &sa.inet_addr_, 
		   sizeof this->inet_addr_);
}

/* Initializes a INET_Addr from a PORT_NUMBER and an Internet address. */

INLINE int
INET_Addr::set (unsigned short port_number, long inet_address, int encode)
{
  this->Addr::base_set (AF_INET, sizeof this->inet_addr_);
  (void) ::memset ((void *) &this->inet_addr_, 0, sizeof this->inet_addr_);
  this->inet_addr_.sin_family = AF_INET;
  this->inet_addr_.sin_port   = encode ? htons (port_number) : port_number;

  /* We need to special case the INADDR_ANY value! */
  if (inet_address == INADDR_ANY)
    this->inet_addr_.sin_addr.s_addr = INADDR_ANY;
  else
    (void) ::memcpy ((void *) &this->inet_addr_.sin_addr, 
		     (void *) &inet_address,
		     sizeof this->inet_addr_.sin_addr);
  return 0;
}

/* Initializes a INET_Addr from a PORT_NUMBER and the remote HOST_NAME. */

INLINE int
INET_Addr::set (unsigned short port_number, const char host_name[], int encode)
{
  hostent *server_info;  
  long	  addr;
  
  this->Addr::base_set (AF_INET, sizeof this->inet_addr_);
  (void) ::memset ((void *) &this->inet_addr_, 0, sizeof this->inet_addr_);

  // Yow, someone gave us a NULL host_name!
  if (host_name == 0)
    {
      errno = EINVAL;
      return -1;
    }
  else if ((addr = ::inet_addr (host_name)) != -1 
	   || ::strcmp (host_name, "255.255.255.255") == 0) // Broadcast addresses are weird...
    return this->set (port_number, addr, encode);
  else if ((server_info = ::gethostbyname (host_name)) != 0)
    {
      (void) ::memcpy ((void *) &addr, server_info->h_addr, server_info->h_length);
      return this->set (port_number, addr, encode);
    }
}

/* Initializes a INET_Addr from a PORT_NAME and the remote HOST_NAME. */

INLINE int
INET_Addr::set (const char port_name[], const char host_name[])
{
  servent *sp;
  
  if ((sp = ::getservbyname ((char*) port_name, "tcp")) == 0)
    return -1;
  else
    return this->set (sp->s_port, host_name, 0);
}

/* Initializes a INET_Addr from a PORT_NAME and an Internet address. */

INLINE int
INET_Addr::set (const char port_name[], long inet_address)
{
  servent *sp;

  if ((sp = ::getservbyname((char*) port_name, "tcp")) == 0) 
    return -1;
  else
    return this->set (sp->s_port, inet_address, 0);
}

/* Creates a INET_Addr from a PORT_NUMBER and the remote HOST_NAME. */

INLINE
INET_Addr::INET_Addr (unsigned short port_number, const char host_name[])
{
  if (this->set (port_number, host_name) == -1)
    LM_ERROR ((LOG_ERROR, "INET_Addr::INET_Addr"));
}

/* Creates a INET_Addr from a sockaddr_in structure. */

INLINE int
INET_Addr::set (const sockaddr_in *addr, int len)
{
  this->Addr::base_set (AF_INET, len);
  ::memcpy ((void *) &this->inet_addr_, (void *) addr, len);
  return 0;
}

/* Creates a INET_Addr from a sockaddr_in structure. */

INLINE
INET_Addr::INET_Addr (const sockaddr_in *addr, int len)
{
  this->set (addr, len);
}

/* Creates a INET_Addr from a PORT_NUMBER and an Internet address. */

INLINE
INET_Addr::INET_Addr (unsigned short port_number, long inet_address)
{
  if (this->set (port_number, inet_address) == -1)
    LM_ERROR ((LOG_ERROR, "INET_Addr::INET_Addr"));
}

/* Creates a INET_Addr from a PORT_NAME and the remote HOST_NAME. */

INLINE
INET_Addr::INET_Addr (const char port_name[], const char host_name[])
{
  if (this->set (port_name, host_name) == -1)
    LM_ERROR ((LOG_ERROR, "INET_Addr::INET_Addr"));
}

/* Creates a INET_Addr from a PORT_NAME and an Internet address. */

INLINE
INET_Addr::INET_Addr (const char* port_name, long inet_address)
{
  if (this->set (port_name, inet_address) == -1)
    LM_ERROR ((LOG_ERROR, "INET_Addr::INET_Addr"));
}

/* Return the address. */

INLINE void *
INET_Addr::get_addr (void) const
{
  return (void *) &this->inet_addr_;
}

/* Transform the current address into string format. */

INLINE int
INET_Addr::addr_to_string (char s[], size_t) const
{
  // This should check to make sure len is long enough...
  sprintf (s, "%s:%d", this->get_host_addr (), this->get_port_number ());
  return 0;
}

/* Compare two addresses for equality. */

INLINE int
INET_Addr::operator == (const Addr &sap) const
{
  return ::memcmp ((void *) &this->inet_addr_.sin_addr, 
		   (void *) &((INET_Addr &) sap).inet_addr_.sin_addr,
		   sizeof (this->inet_addr_.sin_addr)) == 0;
}

/* Compare two addresses for inequality. */

INLINE int
INET_Addr::operator != (const Addr &sap) const
{
  return !((*this) == sap);
}

/* Return the character representation of the hostname. */

INLINE const char *
INET_Addr::get_host_name (void) const
{
  hostent *hp;
  int	  a_len = sizeof this->inet_addr_.sin_addr.s_addr;

  if ((hp = ::gethostbyaddr ((char *) &this->inet_addr_.sin_addr,
			     a_len, this->addr_type_)) == 0)
    return 0;
  else
    return hp->h_name;
}

/* Return the dotted Internet address. */

INLINE const char *
INET_Addr::get_host_addr (void) const
{
  return ::inet_ntoa (this->inet_addr_.sin_addr);  
}

/* Return the 4-byte IP address. */

INLINE unsigned long
INET_Addr::get_ip_address (void) const
{
  return (unsigned long) this->inet_addr_.sin_addr.s_addr;
}

/* Return the port number. */

INLINE unsigned short
INET_Addr::get_port_number (void) const
{
  return this->inet_addr_.sin_port;
}
