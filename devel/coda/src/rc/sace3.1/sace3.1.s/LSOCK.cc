/* Defines the member functions for the Local SOCK abstraction. 
   These functions provide the ability to pass open file descriptors 
   between processes. */

#include "LSOCK.h"

/* This routine sends an open file descriptor to THIS->SOK_FD. */

int
LSOCK::send_handle (const HANDLE fd) const
{
  unsigned char a[2];
  iovec	        iov;
  msghdr	send_msg;

  a[0]                      = 0xab;
  a[1]                      = 0xcd; 
  iov.iov_base	            = (char *) a;
  iov.iov_len		    = sizeof a;
  send_msg.msg_iov	    = &iov;
  send_msg.msg_iovlen	    = 1;
  send_msg.msg_name	    = (char *) 0;
  send_msg.msg_namelen	    = 0;

  return ::sendmsg (this->get_handle (), &send_msg, 0);
}

/* This file receives an open file descriptor from THIS->SOK_FD.
   Note, this routine returns -1 if problems occur, 0 if we recv
   a message that does not have file descriptor in it, and 1
   otherwise. */

int
LSOCK::recv_handle (HANDLE &fd, char *pbuf, int *len) const
{
  unsigned char a[2];
  iovec	   iov;
  msghdr   recv_msg;
  ssize_t   nbytes;
   
  if (pbuf != 0 && len != 0)
    {
      iov.iov_base = pbuf;
      iov.iov_len  = *len;
    }
  else
    {
      iov.iov_base = (char *) a;
      iov.iov_len  = sizeof a;
    }
   
  recv_msg.msg_iov          = &iov;
  recv_msg.msg_iovlen	    = 1;
  recv_msg.msg_name	    = (char *) 0;
  recv_msg.msg_namelen	    = 0;
   
#if defined (ACE_HAS_STREAMS)
  if ((nbytes = ::recvmsg (this->get_handle (), &recv_msg, 0)) != IPC_SAP::INVALID_HANDLE)
    {
      if (len != 0)
	*len = nbytes;

      if (nbytes == sizeof a && ((unsigned char) iov.iov_base[0]) == 0xab
	  && ((unsigned char) iov.iov_base[1]) == 0xcd)
	return 1;
      else
	return 0;
    }
#else
  if ((nbytes = ::recvmsg (this->get_handle (), &recv_msg, MSG_PEEK)) != IPC_SAP::INVALID_HANDLE)
    {
    }
#endif /* ACE_HAS_STREAMS */
  else
    return IPC_SAP::INVALID_HANDLE;
}
