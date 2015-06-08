/* Provides a useful variable-length argument error handling abstraction. */

/*sergey: to provide prototype for 'thr_self'*/
#ifdef SunOS
#include <thread.h>
#endif

#include "Log_Msg.h"
#include "Log_Record.h"

/* Records the program name. */
const char *Log_Msg::program_name_ = "<unknown>";            

/* Default is to use stderr */
unsigned long Log_Msg::flags_ = Log_Msg::STDERR;

#if defined (ACE_MT_SAFE)
/* Synchronization variable */
Mutex Log_Msg::lock_;
#endif /* ACE_MT_SAFE */

/* Process id of the current process. */
pid_t Log_Msg::pid_ = 0;

/* Message queue. */
FIFO_Send_Msg Log_Msg::message_queue_;

/* Open the sender-side of the Message Queue. */

int
Log_Msg::open (const char *prog_name, int flgs, const char *logger_key)
{
  /* Note, if we put a trace call here, we will lose! */
  Log_Msg::program_name_ = prog_name;
  Log_Msg::pid_		 = ::getpid ();
  int status		= 0;

  /* Note that if we fail to open the message queue the default action is to 
     use stderr (set via static initialization in the Log_Msg.C file). */
  
  if (::BIT_ENABLED (flgs, Log_Msg::LOGGER))
    {
      status = Log_Msg::message_queue_.open (logger_key);
	
      if (status == -1)
	::SET_BITS (Log_Msg::flags_, Log_Msg::STDERR);
      else
	::SET_BITS (Log_Msg::flags_, Log_Msg::LOGGER);
    }

  /* Remember, this bit is on by default... */
  if (status != -1 && ::BIT_ENABLED (flgs, Log_Msg::STDERR) == 0)
    ::CLR_BITS (Log_Msg::flags_, Log_Msg::STDERR);
  if (flgs & Log_Msg::VERBOSE)
    ::SET_BITS (Log_Msg::flags_, Log_Msg::VERBOSE);
  return status;
}

/* Valid Options (prefixed by '%', as in printf format strings) include:
   'a': exit the program at this point (var-argument is the exit status!)
   'c': print a character
   'i', 'd': print a decimal number
   'e', 'E', 'f', 'F', 'g', 'G': print a double
   'n': print the name of the program (or "<unknown>" if not set)
   'o': print as an octal number
   'P': print out the current process id
   'p': print out the appropriate errno value from sys_errlist
   'r': call the function pointed to by the corresponding argument
   'S': print out the appropriate _sys_siglist entry corresponding to var-argument.
   's': print out a character string
   't': print thread id (1 if single-threaded)
   'u': print as unsigned int
   'X', 'x': print as a hex number
   '%': print out a single percent sign, '%' */

ssize_t
Log_Msg::log (Log_Priority log_priority, char *format_str, ...)
{ 
  /* Note, if we put a trace call here, we will lose! */
  MT (Guard<Mutex> m (Log_Msg::lock_));

  /* External decls. */
#ifdef Darwin
  extern const int sys_nerr;
#else
  extern int sys_nerr;
#endif

  typedef void (*PTF)(...);

  va_list    argp;		/* Start of variable args section. */
  Log_Record log_Record (log_priority, ::time ((time_t *) 0), Log_Msg::pid_);
  char	     *bp	  = log_Record.msg_data ();
  int	     abort_prog   = 0;
  int	     exit_value   = 0;
  int	     result       = 0;
  char	     *format      = ::strdup (format_str);
  char	     *save_p      = format;	// remember for ::free()

  if (format == 0)
    return -1;

  if (::BIT_ENABLED (Log_Msg::flags_, Log_Msg::VERBOSE))
    {
      /* Prepend the program name onto this message */

      for (const char *s = Log_Msg::program_name_; (*bp = *s) != '\0'; s++)
	bp++;

      *bp++ = '|';
    }

  va_start (argp, format_str);

  while (*format != '\0')
    {
      /* Copy input to output until we encounter a %, however a
      % followed by another % is not a format specification. */

      if (*format != '%')
	*bp++ = *format++;
      else if (*(format + 1) == '%')
	{
	  *bp++ = *format++;	/* First % */
	  *bp++ = *format++;	/* Second % */
	}
      else
	{
	  char c;		/* high use character	*/
	  char *fp;		/* local format pointer	*/
	  int  wpc;		/* width/precision cnt  */
	  const int CONTINUE = 0;
	  const int SKIP_SPRINTF = -1;  /* We must skip the sprintf phase */ 
	  const int SKIP_NUL_LOCATE = -2; /* Skip locating the NUL character */
	  int type = CONTINUE;	/* conversion type	*/
	  int  w[2];		/* width/precision vals */

	  /* % starts a format specification that ends with one of
	  "arnPpSsdciouxXfFeEgG".  An optional width and/or precision
	  (indicated by an "*") may be encountered prior to the
	  end of the specification, each consumes an int arg.
	  A call to sprintf() does the actual conversion. */

	  fp = format++;	/* Remember beginning of format.   */
	  wpc = 0;		/* Assume no width/precision seen. */

	  while (type == CONTINUE)
	    {
	      switch (*format++)
		{
		case 'a': /* Abort program after handling all of format string. */
		  type = SKIP_SPRINTF;
		  abort_prog = 1; 
		  exit_value = va_arg (argp, int); 
		  break;
		case 'r': /* Run (invoke) this subroutine. */
		  type = SKIP_SPRINTF;
		  (*va_arg (argp, PTF))(); 
		  break;
		case 'n': /* Print the name of the program. */
		  type = SKIP_SPRINTF;
		  ::strcpy (bp, Log_Msg::program_name_ ? Log_Msg::program_name_ : "<unknown>");
		  break;
                case 'P': /* Print out the current process id */
		  type = SKIP_SPRINTF;
		  ::sprintf (bp, "%d", Log_Msg::pid_);
		  break;
		case 'p': /* Print out the string assocated with the value of errno. */
		  {
		    type = SKIP_SPRINTF;
		    if (errno >= 0 && errno < sys_nerr) 
		      ::sprintf (bp, "%s: %s", va_arg (argp, char *), strerror (errno));
		    else
		      ::sprintf (bp, "%s: <unknown error> = %d", va_arg (argp, char *), errno);
		    break;
		  }
		case 'S': /* Print out the string associated with this signal number. */
		  {
		    int sig = va_arg (argp, int);
		    type = SKIP_SPRINTF;
#if defined (ACE_HAS_SYS_SIGLIST)
		    if (sig >= 0 && sig < NSIG)
		      ::strcpy (bp, _sys_siglist[sig]);
		    else
		      ::sprintf (bp, "<unknown signal> %d", sig);
#else
		    ::sprintf (bp, "signal %d", sig);
#endif /* ACE_HAS_SYS_SIGLIST */
		    break;
		  }
		case 't': /* Print out thread id. */
		  type = SKIP_SPRINTF;
/* Sergey */
#if !defined(Linux) && !defined(Darwin)
		  sprintf (bp, "%d", thr_self ());
#endif
		  break;
		case 's':
		  type = 1 + wpc; /* 1, 2, 3 */
		  break;
		case 'd': case 'c': case 'i': case 'o': 
		case 'u': case 'x': case 'X': 
		  type = 4 + wpc; /* 4, 5, 6 */
		  break;
		case 'F': case 'f': case 'e': case 'E':
		case 'g': case 'G':
		  type = 7 + wpc; /* 7, 8, 9 */
		  break;
		case '*':	/* consume width/precision */
		  w[wpc++] = va_arg (argp, int);
		  break;
		default:
		  /* ? */
		  break;
		}
	    }

	  if (type != SKIP_SPRINTF)
	    {
	      c = *format;	/* Remember char before we overwrite. */
	      *format = 0;	/* Overwrite, terminating format. */

	      switch (type)
		{
		case 1:
		  sprintf (bp, fp, va_arg (argp, char *));
		  break;
		case 2:
		  sprintf (bp, fp, w[0], va_arg (argp, char *));
		  bp += w[0];
		  type = SKIP_NUL_LOCATE;
		  break;
		case 3:
		  sprintf (bp, fp, w[0], w[1], va_arg (argp, char *));
		  bp += w[0];
		  type = SKIP_NUL_LOCATE;
		  break;
		case 4:
		  sprintf (bp, fp, va_arg (argp, int));
		  break;
		case 5:
		  sprintf (bp, fp, w[0], va_arg (argp, int));
		  break;
		case 6:
		  sprintf (bp, fp, w[0], w[1], va_arg (argp, int));
		  break;
		case 7:
		  sprintf (bp, fp, va_arg (argp, double));
		  break;
		case 8:
		  sprintf (bp, fp, w[0], va_arg (argp, double));
		  break;
		case 9:
		  sprintf (bp, fp, w[0], w[1], va_arg (argp, double));
		  break;
		}
	      *format = c;	/* Restore char we overwrote. */
	    }

	  if (type != SKIP_NUL_LOCATE)
	    while (*bp != '\0')	/* Locate end of bp. */
	      bp++;
	}
    }

  *bp++	= '\0';			/* Terminate bp. */

  free (save_p);
  log_Record.round_up (bp);

  /* Either format the message and print it to stderr, or
     ship it off to the log_client daemon. */

  if (::BIT_ENABLED (Log_Msg::flags_, Log_Msg::STDERR))
    log_Record.print ("<localhost>", ::BIT_ENABLED (Log_Msg::flags_, Log_Msg::VERBOSE));
  if (::BIT_ENABLED (Log_Msg::flags_, Log_Msg::LOGGER))
    {
      Str_Buf log_msg ((void *) &log_Record, int (log_Record.length ()));
#if defined (ACE_HAS_STREAM_PIPES)
      result = Log_Msg::message_queue_.send (int (log_Record.type ()), &log_msg);
#else
      result = Log_Msg::message_queue_.send (log_msg);
#endif /* ACE_HAS_STREAM_PIPES */
    }

  if (abort_prog)
    ::exit (exit_value);

  va_end (argp);
  return result;
}

#if defined (DEBUGGING)
void
cleanup (void)
{
  Log_Msg::log (Log_Msg::LOG_INFO, "later on from PID %d!\n", ::getpid ());
}

int
main (int argc, char *argv[])
{
  if (Log_Msg::open (argv[0]) == -1)
    Log_Msg::log (Log_Msg::LOG_ERR, "cannot open logger!!!\n");
  Log_Msg::log (Log_Msg::LOG_INFO, "%f, %*s, %s = %d\n", 
		3.1416, 8, "", "hello world", 10000);

  if (execl ("lkjdf", "lkjdf", (char *) 0) == -1)
    Log_Msg::log (Log_Msg::LOG_ERR, "%n: (%x), %p%r%a\n", 10000, "lkjdf", cleanup, 1);
}
#endif /* DEBUGGING */
