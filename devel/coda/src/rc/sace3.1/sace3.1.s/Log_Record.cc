/* Provides a useful variable-length argument error handling abstraction. */

#include "Log_Record.h"

Log_Record::Log_Record (Log_Priority lp, long ts, long p)
		       : type_ (long (lp)), time_stamp_ (ts), pid_ (p) 
{
}

void
Log_Record::round_up (char *current_ptr)
{
  int len = (sizeof *this - MAXLOGMSGLEN) + (current_ptr - this->msg_data_);

  this->length_ = (len + Log_Record::ALIGN_WORDB - 1) & ~(Log_Record::ALIGN_WORDB - 1);
}

Log_Record::Log_Record (void)
{
}

/* Print out the record on the stderr stream with the appropriate format. */

int
Log_Record::print (const char host_name[], int verbose, FILE *fp)
{
  if (verbose)
    {
      time_t now  = this->time_stamp_;
      char	 *ctp = ::ctime (&now);

      /* 01234567890123456789012345 */
      /* Wed Oct 18 14:25:36 1989n0 */

      ctp[24] = '\0';

      return ::fprintf (fp, "%s@%s@%d@%d@%s", ctp + 4, host_name, this->pid_,
			this->type_, this->msg_data_);
    }
  else
    return ::fprintf (fp, "%s", this->msg_data_);
}
