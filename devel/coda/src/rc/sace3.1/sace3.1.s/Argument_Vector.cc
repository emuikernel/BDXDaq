/* Transforms a string BUF into an ARGV-style vector of strings. */

#include "Argument_Vector.h"

Argument_Vector::Argument_Vector (char buf[]): argv_ (0), argc_ (0)
{
  char *cp;

  if (buf == 0)
    return;

  /* First pass: count arguments */

  /* '#' is the start-comment token. */  
  for (cp = buf; *cp != '\0' && *cp != '#'; )
    {
      /* Skip whitespace. */
      while (isspace (*cp))
        cp++;

      /* Increment count and move to next whitespace. */
      if (*cp != '\0')
        this->argc_++;

      /* Grok quotes... */
      if (*cp == '\'' || *cp == '"')
        {
          char quote = *cp;

          /* Scan past the string. */
          for (cp++; *cp != '\0' && *cp != quote; cp++)
            continue;

          /* '\0' implies unmatched quote. */
          if (*cp == '\0')
            {
              Log_Msg::log (LOG_ERROR, (char *)"unmatched %c detected\n", quote);
              this->argc_--;
              break;
            } 
          else 
            cp++;
        }
      else /* Skip over non-whitespace... */
        while (*cp != '\0' && !isspace (*cp))
          cp++;
    }

  /* Second pass: copy arguments. */
  const int BUF_SIZE = 256;
  char  arg[BUF_SIZE];

  this->argv_ = new char *[this->argc_ + 1];

  for (int i = 0; i < this->argc_; i++)
    {
      /* Skip whitespace. */
      while (isspace (*buf))
        buf++;
    
      /* Copy next argument and move to next whitespace. */
      if (*buf == '\'' || *buf == '"')
        {
          char quote = *buf++;

          for (cp = arg; *buf != '\0' && *buf != quote; buf++, cp++)
            if (cp - arg < BUF_SIZE)
              *cp = *buf;
	  
          *cp = '\0';
          if (*buf == quote)
            buf++;
        }
      else
        {
          for (cp = arg; *buf && !isspace (*buf); buf++, cp++)
            if (cp - arg < BUF_SIZE)
              *cp = *buf;
          *cp = '\0';
        }
      this->argv_[i] = new char[::strlen (arg) + 1];
      ::strcpy(this->argv_[i], arg);
    }

  this->argv_[this->argc_] = 0;
}

/* Free up the space allocated by the constructor. */

Argument_Vector::~Argument_Vector (void)
{
  if (this->argv_ == 0)
    return;

  for (int i = 0; this->argv_[i] != 0; i++)
    delete [] this->argv_[i];

  delete [] this->argv_;
}

