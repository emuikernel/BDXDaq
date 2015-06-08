/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Log_Record.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_LOG_RECORD_H)
#define ACE_LOG_RECORD_H

#include "sysincludes.h"
#include "Log_Priority.h"

class Log_Record
{
  // = TITLE
  //     Provides a useful variable-length argument message logging abstraction.
  //
  // = DESCRIPTION
  //
public:
  enum 
  {
    MAXLOGMSGLEN = BUFSIZ * 4,	// Maximum size of a logging message. 
    ALIGN_WORDB	 = 8	        // Most restrictive alignment. 
  };

  Log_Record (void);
  Log_Record (Log_Priority lp, long ts, long p);
  int  print (const char host_name[], int verbose = 1, FILE *fp = stderr);
  void round_up (char *current_ptr);
  void encode (void);
  void decode (void);

  long type (void) const;
  void type (long);
  long length (void) const;
  void length (long);
  long time_stamp (void) const;
  void time_stamp (long);
  long pid (void) const;
  void pid (long);
  char *msg_data (void);
  void msg_data (char *data);

private:
  long type_;
  // Type of logging record 

  long length_;      
  // length of the logging record 

  long time_stamp_;  
  // Time logging record generated 

  long pid_;         
  // Id of process that generated the record 

  char msg_data_[MAXLOGMSGLEN]; 
  // Logging record data 
};

#include "Log_Record.i"
#endif /* ACE_LOG_Record_H */
