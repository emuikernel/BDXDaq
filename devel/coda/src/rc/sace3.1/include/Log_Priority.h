/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Log_Priority.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_LOG_PRIORITY_H)
#define ACE_LOG_PRIORITY_H

/* The following data type indicates the relative priorities
   of the logging messages, from lowest to highest priority... */

enum Log_Priority
{
  // = Note, this first argument *must* start at 1! 

  LOG_SHUTDOWN = 1, 
  // Shutdown the logger 

  LOG_TRACE = 2,    
  // Messages indicating function-calling sequence 

  LOG_DEBUG = 3,    
  // Messages that contain information normally of use only when debugging a program 

  LOG_INFO = 4,	    
  // Informational messages 

  LOG_NOTICE = 5,   
  // Conditions that are not error conditions, but that may require special handling 

  LOG_WARNING = 6,  
  // Warning messages 

  LOG_STARTUP = 7,  
  // Initialize the logger 

  LOG_ERROR = 8,    
  // Errors 

  LOG_CRITICAL = 9,	    
  // Critical conditions, such as hard device errors 

  LOG_ALERT = 10,    
  // A condition that should be corrected immediately, such as a corrupted system database 

  LOG_EMERGENCY = 11,   
  // A panic condition  This is normally broadcast to all users 

  LOG_MAX = 12	    
  // Maximum logging priority + 1 
};

#endif /* _LOG_PRIORITY_H */
