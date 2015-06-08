/* -*- C++ -*- */

#include "sysincludes.h"

/* Initialize the internal data when the first call is made.
   Start processing options with ARGV-element 0 + SKIP; the sequence of
   previously skipped non-option ARGV-elements is empty.  */

INLINE 
Get_Opt::Get_Opt (int argc, char **argv, char *optstring, int skip, int report_errors):
		 opterr (report_errors), nextchar (0), optarg (0), 
		 first_nonopt (skip), last_nonopt (skip), optind (skip),
		 noptstring (optstring), nargc (argc), nargv (argv)
{
}

/* Scan elements of ARGV (whose length is ARGC) for option characters
   given in OPTSTRING.

   If an element of ARGV starts with '-', and is not exactly "-" or "--",
   then it is an option element.  The characters of this element
   (aside from the initial '-') are option characters.  If `get_opt'
   is called repeatedly, it returns successively each of the option characters
   from each of the option elements.

   If `get_opt' finds another option character, it returns that character,
   updating `optind' and `nextchar' so that the next call to `get_opt' can
   resume the scan with the following option character or ARGV-element.

   If there are no more option characters, `get_opt' returns `EOF'.
   Then `optind' is the index in ARGV of the first ARGV-element
   that is not an option.  (The ARGV-elements have been permuted
   so that those that are not options now come last.)

   OPTSTRING is a string containing the legitimate option characters.
   A colon in OPTSTRING means that the previous character is an option
   that wants an argument.  The argument is taken from the rest of the
   current ARGV-element, or from the following ARGV-element,
   and returned in `optarg'.

   If an option character is seen that is not listed in OPTSTRING,
   return '?' after printing an error message.  If you set `opterr' to
   zero, the error message is suppressed but we still return '?'.

   If a char in OPTSTRING is followed by a colon, that means it wants an arg,
   so the following text in the same ARGV-element, or the text of the following
   ARGV-element, is returned in `optarg.  Two colons mean an option that
   wants an optional arg; if there is text in the current ARGV-element,
   it is returned in `optarg'.

   If OPTSTRING starts with `-', it requests a different method of handling the
   non-option ARGV-elements.  See the comments about RETURN_IN_ORDER, above.  */

INLINE int
Get_Opt::operator () (void)
{
  if (this->nextchar == 0 || *this->nextchar == 0)
    {
      /* Special ARGV-element `--' means premature end of options.
         Skip it like a null option,
         then exchange with previous non-options as if it were an option,
         then skip everything else like a non-option.  */

      if (this->optind != this->nargc && !strcmp (this->nargv[this->optind], "--"))
        {
          this->optind++;

          if (this->first_nonopt == this->last_nonopt)
            this->first_nonopt = this->optind;
          this->last_nonopt = this->nargc;

          this->optind = this->nargc;
        }

      /* If we have done all the ARGV-elements, stop the scan
         and back over any non-options that we skipped and permuted.  */

      if (this->optind == this->nargc)
        {
          /* Set the next-arg-index to point at the non-options
             that we previously skipped, so the caller will digest them.  */
          if (this->first_nonopt != this->last_nonopt)
            this->optind = this->first_nonopt;
          return EOF;
        }
	 
      /* If we have come to a non-option and did not permute it,
         either stop the scan or describe it to the caller and pass it by.  */

      if (this->nargv[this->optind][0] != '-' || this->nargv[this->optind][1] == 0)
	return EOF;

      /* We have found another option-ARGV-element.
         Start decoding its characters.  */

      this->nextchar = this->nargv[this->optind] + 1;
    }

  /* Look at and handle the next option-character.  */

  {
    char c = *this->nextchar++;
    char *temp = (char *) strchr (this->noptstring, c);

    /* Increment `optind' when we start to process its last character.  */
    if (*this->nextchar == 0)
      this->optind++;

    if (temp == 0 || c == ':')
      {
        if (this->opterr != 0)
          {
            if (c < 040 || c >= 0177)
              fprintf (stderr, "%s: unrecognized option, character code 0%o\n",
                       this->nargv[0], c);
            else
              fprintf (stderr, "%s: unrecognized option `-%c'\n",
                       this->nargv[0], c);
          }
        return '?';
      }
    if (temp[1] == ':')
      {
        if (temp[2] == ':')
          {
            /* This is an option that accepts an argument optionally.  */
            if (*this->nextchar != 0)
              {
                this->optarg = this->nextchar;
                this->optind++;
              }
            else
              this->optarg = 0;
            this->nextchar = 0;
          }
        else
          {
            /* This is an option that requires an argument.  */
            if (*this->nextchar != 0)
              {
                this->optarg = this->nextchar;
                /* If we end this ARGV-element by taking the rest as an arg,
                   we must advance to the next element now.  */
                this->optind++;
              }
            else if (this->optind == this->nargc)
              {
                if (this->opterr != 0)
                  fprintf (stderr, "%s: no argument for `-%c' option\n",
                           this->nargv[0], c);
                c = '?';
              }
            else
              /* We already incremented `optind' once;
                 increment it again when taking next ARGV-elt as argument.  */
              this->optarg = this->nargv[this->optind++];
            this->nextchar = 0;
          }
      }
    return c;
  }
}
