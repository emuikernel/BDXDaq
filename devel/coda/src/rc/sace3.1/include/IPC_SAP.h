/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    IPC_SAP.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_IPC_SAP_H)
#define ACE_IPC_SAP_H

#include "sysincludes.h"

class IPC_SAP
  // = TITLE
  //     Defines the member functions for the base class of the IPC_SAP abstraction.
  // 
  // = DESCRIPTION
  //
{
public:
  enum
  {
    INVALID_HANDLE = -1 // Be consistent with Winsock 
  };

  int control (int cmd, void *) const;
  // Interface for ioctl. 

  // = Methods for manipulating common I/O descriptor options related to
  //   sockets. 
  int enable (int signum) const;
  int disable (int signum) const;

  // = Set/get the underlying descriptor. 
  HANDLE get_handle (void) const;
  void	 set_handle (HANDLE handle);

protected:
  IPC_SAP (void);
  // Ensure that IPC_SAP is an abstract base class. 

private:
  HANDLE handle_;
  // Underlying I/O descriptor. 

  static pid_t pid_;
  // Cache the process ID. 
};

#include "IPC_SAP.i"

#endif /* _IPC_SAP_H */
