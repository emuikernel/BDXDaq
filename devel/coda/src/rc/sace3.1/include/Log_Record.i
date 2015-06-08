/* -*- C++ -*- */

inline void
Log_Record::encode (void)
{
  this->type_       = htonl (this->type_);
  this->length_     = htonl (this->length_);
  this->time_stamp_ = htonl (this->time_stamp_);
  this->pid_        = htonl (this->pid_);
}

inline void
Log_Record::decode (void)
{
  this->type_       = ntohl (this->type_);
  this->time_stamp_ = ntohl (this->time_stamp_);
  this->pid_        = ntohl (this->pid_);
  this->length_     = htonl (this->length_);
}

inline long 
Log_Record::type (void) const
{
  return this->type_;
}

inline void 
Log_Record::type (long t)
{
  this->type_ = t;
}

inline long 
Log_Record::length (void) const
{
  return this->length_;
}

inline void 
Log_Record::length (long l)
{
  this->length_ = l;
}

inline long 
Log_Record::time_stamp (void) const
{
  return this->time_stamp_;
}

inline void 
Log_Record::time_stamp (long ts)
{
  this->time_stamp_ = ts;
}

inline long 
Log_Record::pid (void) const
{
  return this->pid_;
}

inline void 
Log_Record::pid (long p)
{
  this->pid_ = p;
}

inline char *
Log_Record::msg_data (void)
{
  return this->msg_data_;
}

inline void 
Log_Record::msg_data (char *data)
{
  ::strncpy (this->msg_data_, data, Log_Record::MAXLOGMSGLEN);
}

