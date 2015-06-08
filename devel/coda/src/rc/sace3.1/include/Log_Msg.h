/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Log_Msg.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_LOG_MSG_H)
#define ACE_LOG_MSG_H

#include "FIFO_Send_Msg.h"
#include "Log_Priority.h"

#if defined (ACE_MT_SAFE)
#include "Synch.h"
#endif /* ACE_MT_SAFE */

#if defined (ACE_NLOGGING)
#define LM_ERROR_RETURN(X,Y) return (Y)
#define LM_ERROR(X) 
#define LM_DEBUG(X) 
#else
#define LM_ERROR_RETURN(X,Y) do { Log_Msg::log X; return (Y); } while (0)
#define LM_ERROR(X) Log_Msg::log X
#define LM_DEBUG(X) Log_Msg::log X
#endif /* ACE_NLOGGING */

class Log_Msg 
  // = TITLE
  //     Provides a useful variable-length argument message logging abstraction.
  //
  // = DESCRIPTION
  //
{
public:
  enum
  {
    STDERR       = 01,		// Write logging messages to stderr. 
    LOGGER       = 02,		// Write the logging messages to the local log deamon. 
    VERBOSE	 = 04		// Display logging records in a verbose manner 
  };

public:
  static int open (const char *prog_name, int flags = Log_Msg::STDERR, const char *logger_key = 0);
  static ssize_t log (Log_Priority, char *format, ...);
  static void sync (const char *program_name);

private:

  static FIFO_Send_Msg message_queue_;
  // IPC conduit between sender and client daemon. 

  static const char *program_name_; 
  // Records the program name. 

  static pid_t pid_;
  // Process id of the current process. 

  static unsigned long flags_;
  // Options flags 

#if defined (ACE_MT_SAFE)
  // Synchronization variable for the MT_SAFE logging 
  static Mutex lock_; 
#endif /* ACE_MT_SAFE */       
};

#include "Log_Msg.i"
#endif /* ACE_LOG_MSG_H */
