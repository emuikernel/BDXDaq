/* -*- C++ -*- */
/* Provides a useful variable-length argument message logging abstraction. */

/* Call after a fork to resynchronize the PID and PROGRAM_NAME variables. */

inline void
Log_Msg::sync (const char *prog_name)
{
  Log_Msg::program_name_ = prog_name;
  Log_Msg::pid_		 = ::getpid ();
}

