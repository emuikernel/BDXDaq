/* -*- C++ -*- */
/* Get_Opt for GNU.
   Copyright (C) 1987, 1989 Free Software Foundation, Inc.
   (Modified by Douglas C. Schmidt for use with C++.)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    Get_Opt.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_GET_OPT_H)
#define ACE_GET_OPT_H

/* This version of `get_opt' appears to the caller like standard Unix `get_opt'
   but it behaves differently for the user, since it allows the user
   to intersperse the options with the other arguments.

   As `get_opt' works, it permutes the elements of `argv' so that,
   when it is done, all the options precede everything else.  Thus
   all application programs are extended to handle flexible argument order.

   Setting the environment variable _POSIX_OPTION_ORDER disables permutation.
   Then the behavior is completely standard.

   GNU application programs can use a third alternative mode in which
   they can distinguish the relative order of options and other arguments.  */

class Get_Opt
  // = TITLE
  //
  //
  // = DESCRIPTION
  //
{
private:
  
  char *nextchar;
  // The next char to be scanned in the option-element
  // in which the last option character we returned was found.
  // This allows us to pick up the scan where we left off.
  //    
  // If this is zero, or a null string, it means resume the scan
  // by advancing to the next ARGV-element.  
  
 
  int first_nonopt;
  int last_nonopt;
  // =  Describe the part of ARGV that contains non-options that have
  // been skipped.  `first_nonopt' is the index in ARGV of the first of them;
  // `last_nonopt' is the index after the last of them.  
  
public:
  
  char *optarg;
  // For communication from `get_opt' to the caller.
  // When `get_opt' finds an option that takes an argument,
  // the argument value is returned here.
  // Also, when `ordering' is RETURN_IN_ORDER,
  // each non-option ARGV-element is returned here.  
  
  
  int optind;
  // Index in ARGV of the next element to be scanned.
  // This is used for communication to and from the caller
  // and for communication between successive calls to `get_opt'.
  // On entry to `get_opt', zero means this is the first call; initialize.
  //      
  // When `get_opt' returns EOF, this is the index of the first of the
  // non-option elements that the caller should itself scan.
  //          
  // 
  // Otherwise, `optind' communicates from one call to the next
  // how much of ARGV has been scanned so far.  

  
  int opterr;
  // Callers store zero here to inhibit the error message
  // for unrecognized options.  
  
  
  int    nargc;
  char **nargv;
  char  *noptstring;
  
  Get_Opt (int argc, char **argv, char *optstring, int skip = 1, int report_errors = 0);
  int operator () (void);
};

#if defined (__INLINE__)
#define INLINE inline
#include "Get_Opt.i"
#else
#define INLINE
#endif /* __INLINE__ */

#endif /* _GET_OPT_H */

