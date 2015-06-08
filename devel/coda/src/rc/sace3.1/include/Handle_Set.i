/* -*- C++ -*- */
/* Wrapper for the Handle_Set abstraction. */

/* Initialize the bitmask to all 0s and reset the associated fields. */

inline void 
Handle_Set::reset (void)
{
  this->max_handle_ = -1;
  this->size_   = 0;
  FD_ZERO (&this->mask_);
}

/* Constructor, initializes the bitmask to all 0s. */

inline
Handle_Set::Handle_Set (void)
{
  this->reset ();
}

inline
Handle_Set::Handle_Set (const fd_set &fd_mask): size_ (0)
{
  ::memcpy ((void *) &this->mask_, (void *) &fd_mask, sizeof this->mask_);
  this->sync (FD_SETSIZE);
}

/* Returns the number of the large bit. */

inline int  
Handle_Set::max_set (void) const
{
  return this->max_handle_;
}

/* Checks whether FD is enabled. */

inline int
Handle_Set::is_set (int fd) const
{
  return FD_ISSET (fd, &this->mask_);
}

/* Enables the FD. */

inline void 
Handle_Set::set_bit (int fd)
{
  if (!this->is_set (fd))
    {
      FD_SET (fd, &this->mask_);
      this->size_++;
      if (fd > this->max_handle_)
	this->max_handle_ = fd;
    }
}

/* Disables the FD. */

inline void 
Handle_Set::clr_bit (int fd)
{
  if (this->is_set (fd))
    {
      FD_CLR (fd, &this->mask_);
      this->size_--;

      if (fd == this->max_handle_)
	this->set_max (this->max_handle_);
    }
}

/* Returns a count of the number of enabled bits. */

inline int 
Handle_Set::num_set (void) const
{
  return this->size_;
}

/* Returns a pointer to the underlying fd_set. */

inline
Handle_Set::operator fd_set *()
{
  return &this->mask_;
}

inline int
Handle_Set_Iterator::operator () (void)
{
  return this->num_ <= this->fds_.max_handle_ ? this->num_ : -1;
}
