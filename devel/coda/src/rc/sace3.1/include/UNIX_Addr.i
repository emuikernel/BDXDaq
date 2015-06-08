/* -*- C++ -*- */
/* Defines the ``UNIX domain address family'' address format. */

/* Do nothing constructor. */

INLINE
UNIX_Addr::UNIX_Addr (void): Addr (AF_UNIX, sizeof this->unix_addr_)
{
  (void) ::memset ((void *) &this->unix_addr_, 0, sizeof this->unix_addr_);
}

/* Copy constructor. */

INLINE
UNIX_Addr::UNIX_Addr (const UNIX_Addr &sa)
  : Addr (AF_UNIX, sa.get_size ())
{
  size_t size = sa.get_size ();

  // Add one extra byte to account for the NUL at the end of the pathname.
  if (size < sizeof this->unix_addr_)
    size = sa.get_size () + 1;

  this->unix_addr_.sun_family = AF_UNIX;
  strcpy (this->unix_addr_.sun_path, sa.unix_addr_.sun_path);
}

INLINE void
UNIX_Addr::set (const sockaddr_un *un, int len)
{
  (void) ::memset ((void *) &this->unix_addr_, 0, sizeof this->unix_addr_);
  this->unix_addr_.sun_family = AF_UNIX;
  strcpy (this->unix_addr_.sun_path, un->sun_path);
  this->base_set (AF_UNIX, len);
}

INLINE
UNIX_Addr::UNIX_Addr (const sockaddr_un *un, int len)
{
  this->set (un, len);
}

INLINE void 
UNIX_Addr::set (const char rendezvous_point[])
{
  (void) ::memset ((void *) &this->unix_addr_, 0, sizeof this->unix_addr_);
  int len = ::strlen (rendezvous_point);

  this->unix_addr_.sun_family = AF_UNIX;

  if (len >= sizeof this->unix_addr_.sun_path)
    {
      /* At this point, things are screwed up, so we might as well make sure we 
         don't crash. */
      (void) ::strncpy (this->unix_addr_.sun_path, rendezvous_point, 
			sizeof this->unix_addr_.sun_path);
      len = sizeof this->unix_addr_.sun_path;
      this->unix_addr_.sun_path[len - 1] = '\0';
      len -= 2; /* Don't count the NUL byte at the end of the string. */
    }
  else
    (void) ::strcpy (this->unix_addr_.sun_path, rendezvous_point);
  this->Addr::base_set (AF_UNIX, len + sizeof this->unix_addr_.sun_family);
}

/* Create a Addr from a UNIX pathname. */

INLINE
UNIX_Addr::UNIX_Addr (const char rendezvous_point[])
{
  this->set (rendezvous_point);
}

/* Return the address. */

INLINE void *
UNIX_Addr::get_addr (void) const
{
  return (void *) &this->unix_addr_;
}

/* Transform the current address into string format. */

INLINE int
UNIX_Addr::addr_to_string (char s[], size_t len) const
{
  ::strncpy (s, this->unix_addr_.sun_path, len);
  return 0;
}

/* Compare two addresses for equality. */

INLINE int
UNIX_Addr::operator == (const Addr &sap) const
{
  return ::strcmp (this->unix_addr_.sun_path, 
		   ((const UNIX_Addr &) sap).unix_addr_.sun_path) == 0;
}

/* Compare two addresses for inequality. */

INLINE int
UNIX_Addr::operator != (const Addr &sap) const
{
  return !((*this) == sap);	/* This is lazy, of course... ;-) */
}

/* Return the path name used for the rendezvous point. */

INLINE const char *
UNIX_Addr::get_path_name (void) const
{
  return this->unix_addr_.sun_path;
}
