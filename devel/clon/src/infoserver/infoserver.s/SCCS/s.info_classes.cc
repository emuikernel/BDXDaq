h18208
s 00137/00029/01386
d D 1.16 02/09/17 20:10:19 jhardie 20 19
c Bug fix.  After OS and Compiler upgrades, info_server was hanging in
c input.  Problem was blank lines in configuration file which apparently 
c caused an error condition in the input stream when the program
c attempted to read them.  Cause unknown, but perhaps a change in the
c behavior of the istream class in the new compiler?  Fix: switch to C++ 
c string class input rather than C-style string input.  This affects the 
c get_next_line function only.  
c 
e
s 00001/00003/01414
d D 1.15 01/01/19 15:47:42 jhardie 19 18
c fixed version problems and const B.S.
e
s 00110/00012/01307
d D 1.14 01/01/19 15:05:32 jhardie 18 16
i 17
c Merged changes between child workspace "/home/jhardie/scalers/info_server" and
c  parent workspace "/usr/local/clas/devel/source".
c 
e
s 00085/00052/01330
d D 1.12.1.3 01/01/19 14:18:58 jhardie 17 15
c current version (but not 5.6 ready)
e
s 00005/00004/01254
d D 1.13 01/01/09 16:17:40 jhardie 16 13
c Fixed const problems for ANSI compilance
c 
e
s 00016/00016/01366
d D 1.12.1.2 99/06/15 13:33:55 jhardie 15 14
c Changed output stream from "cout" to "out" in dumpthelist() function.
c 
e
s 00136/00012/01246
d D 1.12.1.1 99/06/15 13:30:40 jhardie 14 13
c Added the dumpthelist() function for printing an ascii representation
c of the input list to the specified file.
c 
e
s 00002/00001/01256
d D 1.12 99/01/06 19:54:42 jhardie 13 12
c fixed the bug introduced with the last fix.
c 
e
s 00001/00001/01256
d D 1.11 99/01/06 19:47:39 jhardie 12 11
c fixed missing / in possible names.
c 
e
s 00064/00012/01193
d D 1.10 99/01/06 19:21:26 jhardie 11 10
c Modified for verions of infoserver which
c mangle names, new ss groups, and timestamping.
e
s 00013/00011/01192
d D 1.9 98/02/05 12:11:29 jhardie 10 9
c Added ability to have spaces in field names.
c 
e
s 00072/00003/01131
d D 1.8 98/01/27 12:47:15 jhardie 9 8
c Added code to check for either all tagged or all non-tagged
c fields in an input message.  Mixed mode messages cannot be
c handled. (yet)
c 
e
s 00041/00002/01093
d D 1.7 98/01/27 11:15:27 jhardie 8 7
c Added required code for parsing tagged arrays
c 
e
s 00002/00002/01093
d D 1.6 98/01/22 17:41:00 jhardie 7 6
c Bug fixes and changes to timeout routines.  Needed a way of checking
c the value of the timeout.
c 
e
s 00017/00004/01078
d D 1.5 98/01/22 13:38:49 jhardie 6 5
c Made changes to handle timeout function.
c 
e
s 00000/00000/01082
d D 1.4 97/06/05 11:58:10 jhardie 5 4
c fixed some small problems with smartsockets arrays
e
s 00000/00000/01082
d D 1.3 97/06/05 10:40:34 jhardie 4 3
c Version 1.0 of info_server and support programs
e
s 01075/00010/00007
d D 1.2 97/06/02 18:37:12 jhardie 3 1
c Rewrite parser for full C++.  Info messages input ok,
c output still broken
e
s 00000/00000/00000
d R 1.2 97/03/25 09:44:59 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 3 2 info_server/server/info_classes.cc
c Name history : 2 1 server/info_classes.cc
c Name history : 1 0 s/server/info_classes.cc
e
s 00017/00000/00000
d D 1.1 97/03/25 09:44:58 jhardie 1 0
c New C++ version of info-server program.  Translation
c from C because of complexity of internal structures.
c These are better handled with classes.
e
u
U
f e 0
t
T
I 3
//*****************************************************************************
// scaparse.cpp
// Parser routines for the info_server program.  Reads an input command
// file and splits it into a parse tree.
//
I 20
// NOTE: Changes in get_new_line marked with JGH902 were made late at night
// to fix a hang in configuration file read.  This hang was triggered by the
// presence of blank lines in the input file.  For some unknown reason, attempting
// to read a blank line would produce a non-advancing input stream.  I suspect that
// there was an error condition being set in the input stream in this case
// but I'm not sure what error condition it was or why it was happening.
// 
// In any case, I worked around it by switching this file to use the new
// standard headers and to do the actual I/O with C++ string objects, which
// were then copied into the normal C-string style buffer for further parsing.
//
// This code BADLY needs to be updated to the new language standard.
//
// - jgh, 9/17/02
//
E 20
// Programmed by: jgh
// Date: May 1997
//*****************************************************************************

I 20
//--------
// JGH902 - Switch to new C++ header files.  Force inclusion of standard
// namespace so that the rest of the code continues to work.
//
E 20
// The standard include files
D 20
#include <fstream.h>
#include <iostream.h>
#include <iomanip.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
E 20
I 20
// #include <fstream.h>
// #include <iostream.h>
// #include <iomanip.h>
// #include <string.h>
// #include <ctype.h>
// #include <stdlib.h>
// #include <time.h>
//-------
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;
E 20

// The parser definitions.  Contains definitions for:
// 1) Token types
//		- input, output, integer, floating, array, block, alias
//		- end, string, literal, datagroup, name, quote, lparen, rparen
// 2) Token structure (token)
// 3) Parse table classes:
I 16
D 19
#include "info_includes.hh"
E 19
E 16
#include "info_classes.hh"
I 16
D 19
#include "info_proto.hh"
E 19
E 16

// Function prototypes
void printlist(llist &l);
void process_alias(alias *al, istream &infile);
void process_block(block *bl, istream &infile);
D 11
void input_element(llist &l, tokentype t, istream &i);
E 11
I 11
void input_element(input *in, tokentype t, istream &i);
E 11
int process_input(input *in, istream &infile);
int process_output(output *out, istream &infile);
int parse_config(istream &in);
I 19
int read_config(char *f);
E 19
D 16
int read_config(char *f);
E 16
int get_next_line(istream &in);
inline const long numtokens();
int countquotes(char *p);
tokentype get_token(istream &in);
int valid_line(istream &in);
char *stripwhitespace(char *s);
inline void tok_putback();
D 9

E 9
I 9
int check_tagged_syntax(input *in);
E 9

I 17
ostream &operator<<(ostream &out, lnode *x)
{
  return x->Print(out);
}

ostream &operator<<(ostream &out, lnode &x)
{
  return x.Print(out);
}



E 17
E 3
I 1
//**********************************************************************
D 3
// info_classes.cc
E 3
I 3
// Lexical Analysis
// The next few routines read the input file a line at a time and
// split it up into tokens.  First, we define a few useful global
// variables, then we get to the functions that use them.
//**********************************************************************

mytoken cur_tok;		// the most recently seen token
const int maxLineLen=4095;	// maximum length of a single input line
char line_buf[maxLineLen+1];	// +1 for the null.
char *line_ptr,*last_ptr;	// placekeepers when scanning the line
char *line_last;

//**********************************************************************
// get_next_line()
// Scans the input stream until it completes a command, or reads an
// entire input line.
// Generally, there is expected to be one command per input line, but
// if a "Literal" command is given, the arbitrary string can span several
// input lines and will be read up to the matching quote.
// This function converts all ascii letters to lower case.
//**********************************************************************
int get_next_line(istream &in)
{
I 20
  //-------
  // JGH902 - we need some C++ string objects for the new read
  // methods
  //-------
  string stemp,stemp2;
E 20
  int i;
  int linelength;
D 20
  char c;
E 20

I 20
  //-------
  // JGH902 - Force this variable to \n
  // Ugly kludge.  We're no longer reading this and it would only contain
  // newlines anyway, but we go ahead and force it just so that the
  // rest of the parsing code doesn't need to be modified.  This code
  // REALLY should be updated to use the new C++ standard facilities.
  //
  // Ugh!
  //-------
  char c = '\n';

E 20
  DEBUG(cout << "Get_Next_Line entered" << endl;);
  
  
  line_ptr = line_buf;		// initialize the pointer

  // EOF here just means that we reached the end of the
  // input file.  Note: blank lines will contain at least a newline
  // so they will not be empty.
  if (in.eof())
  {
    DEBUG(cout << "Get_Next_Line at EOF before first read" << endl;);
    
    line_buf[0] = 0;		// NULL out the string.
    return -1;			// can never have -1 chars.
  }

D 20
  in.get(line_ptr,maxLineLen,'\n');	// read to end of line
E 20
I 20
  //-------
  // JGH902 - Switch to C++ string I/O
  //
  //in.get(line_ptr,maxLineLen,'\n');	// read to end of line
  //-------
  getline(in,stemp);
E 20
  
D 20
  if (in.get(c) && c != '\n')	// Line too long - flag an error
  {
    cerr << "GetNextLine: Error in input file : Line too long" 
	 << endl << "Line was: " << line_buf << endl;
    throw ParseExcept("get_next_line buffer overrun",
		      "Input data line is too long");
  }
  linelength = strlen(line_ptr); // Doesn't count the null!
E 20
I 20

  //-------
  // JGH902 - The C-style buffer is not full at this point so print
  // the C++ string for debugging.
  //
  //DEBUG(cout << "\tLine Read = " << line_ptr << endl;);
  //-------
  DEBUG(cout << "\tLine Read = " << stemp << endl;);
  DEBUG(if (in.peek() == '\n') cout << "\tNewline coming" << endl;
	else cout << "\tNo Newline found" << endl;);

  //-------
  // JGH902 - Change handling of newline on input.
  //
  // The following condition cannot happen when using the C++ standard
  // string class and the getline(istream &, string &) function which
  // discards the newline anyway.  We can't test for a missing newline
  // since it's supposed to be gone, but the rest of the code depends on
  // the newline being detected, so we force the variable 'c' to contain
  // a newline.
  //
//   if (in.get(c) && c != '\n')	// Line too long - flag an error
//   {
//     cerr << "GetNextLine: Error in input file : Line too long" 
// 	 << endl << "Line was: " << line_buf << endl;
//     throw ParseExcept("get_next_line buffer overrun",
// 		      "Input data line is too long");
//   }
  //-------

  //-------
  // JGH902 - New way of computing string length.
  //
  //linelength = strlen(line_ptr); // Doesn't count the null!
  //-------
  linelength = stemp.length();

  DEBUG(cout << "\tLine Length Ok = " << linelength << endl;);
E 20
  
  // Now check to see if we got an even number of quotation marks.
  // an odd number of quotation marks indicates that we need to keep
  // filling the line buffer.  At this point, we make the assumption
  // that the end of a quote will be on its own line and that the
  // next command will *NOT* start on the same line.  If this is not
  // the case, an error will probably be generated in the parse somewhere.
  // You might get lucky, you might not.
I 20
  line_buf[0] = 0;

  //-------
  // JGH902 - Put chars in C-style buffer for countquotes function.
  //-------
  strcpy(line_buf,stemp.c_str());
E 20
  while (countquotes(line_buf) % 2)
  {
    int remaining;

    DEBUG(cout << "Get_Next_Line reading a quoted string" << endl;);
    
    remaining =  maxLineLen - linelength;
    line_last = line_ptr;
    line_ptr = line_ptr + linelength; // points at the null
    *line_ptr++ = c;		// be sure to keep the newline.

    // An EOF here is bad because it means that the input file
    // is incomplete.
    if (in.eof())
    {
      DEBUG(cout << "Get_Next_Line: Premature EOF reading quoted string" 
	    << endl;
	);
      
      throw ParseExcept("get_next_line premature eof",
			"End of file found before quote ended");
      // End of file found - BAD
    }
D 20
      
    in.get(line_ptr,remaining,'\n');
        
    if (in.get(c) && (c != '\n'))	// Line too long - flag an error)
    {
      cerr << "GetNextLine: Error in input file : Line too long" 
	   << endl << "Line was: " << line_buf << endl;
      throw ParseExcept("get_next_line buffer overrun",
			"Input data line is too long");
    }
    linelength = strlen(line_buf); // Doesn't count the null!
E 20
I 20
    
    //-------
    // JGH902 - New C++ I/O, and append to string.
    //
    //in.get(line_ptr,remaining,'\n');
    //-------
    getline(in,stemp2);
    stemp += stemp2;

    //-------
    // JGH902 - Fix newline handling as above.
    //
    // This can't occur.  See similar comment above.
//     if (in.get(c) && (c != '\n'))	// Line too long - flag an error)
//     {
//       cerr << "GetNextLine: Error in input file : Line too long" 
// 	   << endl << "Line was: " << line_buf << endl;
//       throw ParseExcept("get_next_line buffer overrun",
// 			"Input data line is too long");
//     }
    //-------

    //-------
    // JGH902 - New line length computation
    //
    //linelength = strlen(line_buf); // Doesn't count the null!
    //-------
    linelength = stemp.length();
E 20
  }
I 20

  //-------
  // JGH902 - Put string chars in C-style buffer for rest of code.
  //-------
  strcpy(line_buf,stemp.c_str());
E 20
  line_ptr = line_buf;
  line_last = line_ptr + linelength - 1;
  while (*line_ptr)
  {
    if (isalpha(*line_ptr))
      *line_ptr = tolower(*line_ptr);
    line_ptr++;
  }
  line_ptr = line_buf;
D 20
  
E 20
I 20
  DEBUG(cout << "Downcased.  Line = " << line_ptr << endl;);

E 20
  return(linelength);
}

    
//**********************************************************************
// countquotes()
// Counts the number of quotation marks in a character string
//**********************************************************************
int countquotes(char *p)
{
  int n = 0;
  while (*p)
  {
    if (*p++ == '"')
      n++;
  }
  return n;
}

//**********************************************************************
// get_token
// Scans the currently read line for the next valid token.  The routine
// will generate a get_next_line if it runs out of input and will
// throw away blank lines and comments.  Comments are flagged by
// a leading pound sign (#) and are assumed to continue to the
// end of the current line.
E 3
//
D 3
// Methods and implementation for the input/output data stream objects
// for the Info_Server
E 3
I 3
// NOTE: get_token assumes that get_next_line has been done before it
// is called.
//**********************************************************************

D 16
char *lit_token[] = {
E 16
I 16
const char *lit_token[] = {
E 16
D 8
  "jgh", "input", "output", "integer", "floating", "array", "block",
E 8
I 8
  "jgh", "input", "output", "integer", "floating", "array", "tagged", "block",
E 8
D 6
  "alias", "end", "datagroup", "name", "literal", "ignore", "string", 
  "reference"
E 6
I 6
  "alias", "end", "datagroup", "name", "literal", "ignore", "timeout", 
D 11
  "string", "reference"
E 11
I 11
D 17
  "string", "reference", "timestamp"
E 17
I 17
    "string", "reference", "timestamp", "lifetime"
E 17
E 11
E 6
};
int num_literals = sizeof(lit_token)/sizeof(char *);

tokentype get_token(istream &in)
{
  int i, leng, is_number=1;
  int got_a_token = 0;
  
  DEBUG(cout << "Get_Token entered.  Assuming a number" << endl;);
  
  is_number=1;

  // Check for a recycled token.
  if (cur_tok.putback)
  {
    cur_tok.putback = 0;
    return cur_tok.type;
  }
  
  while (!got_a_token)
  {
    // Try to guarantee that we have a valid line to work on.  If
    // we dont get one, flag an invalid token and go away.  A valid
    // line is defined to have at least one non-blank character on 
    // it.
    if (!valid_line(in))
    {
      DEBUG(cout << "Get_Token: EOF or input line error" << endl;);
      
      return t_invalid;
    }
    
    
    // did we throw away the entire line?
    if (! *line_ptr)
    {
      DEBUG(cout << "Get_Token: Empty line, get another" << endl;);
      
      continue;
    }
    
    
    // is the line a comment?
    if (*line_ptr == '#')
    {
      DEBUG(cout << "Get_Token: Comment - skipping to next line" << endl;);
      
      *line_ptr = 0;
      continue;
    }
    
    
    // Ok, we got an actual character to check.  First, we'll
    // try to determine what kind of a token it is, then we will
    // parse it in more detail.
D 13
    if (isalnum(*line_ptr) || (*line_ptr == '_') || (*line_ptr == '-'))
E 13
I 13
    if (isalnum(*line_ptr) || (*line_ptr == '_') || (*line_ptr == '-')
      || (*line_ptr == '.') || (*line_ptr == ':') || (*line_ptr == '/'))
E 13
    {
      DEBUG(cout << "Get_Token: Alphanumeric token detected" << endl;);
      
      // First thing we do is check for one of the known commands
      for (i=1; i < num_literals; i++)
      {
	if (!strncmp(line_ptr, lit_token[i], strlen(lit_token[i])))
	{
	  // Ok, we have a match.  Now we need to check to see if
	  // it is a unique match - we don't want to flag something
	  // like "Endlist" and return an "End" token.
	  line_last = line_ptr + strlen(lit_token[i]);
	  if (!(*line_last) || isspace(*line_last))
	  {
	    // Found it.  Set things up.
	    DEBUG(cout << "Get_Token: Found standard token "  
		  << lit_token[i] << endl;
	      );
	    
	    cur_tok.type = (tokentype)(i);
	    strcpy(cur_tok.data,lit_token[i]);
	    line_ptr += strlen(lit_token[i]);
	    got_a_token++;
	    break;		// breaks the *FOR* loop
	  }
	}
      }
      if (got_a_token)
	break;			// breaks the *WHILE* loop

      // It wasn't a standard command.  This means that it was
      // either a character string or something like that.  In this
      // case, we copy it up to the next whitespace, or to the end
      // of the line.  We start by assuming that the token is
      // a number, and we keep going with this assumption until we
      // find otherwise.
      i = 0;
      is_number = 1;
      int found_neg=0, found_dec=0;
      DEBUG(cout << "Get_Token: Not a standard token...checking" << endl;);
      
      while ((*line_ptr) && 
	     ((isalnum(*line_ptr)) || (*line_ptr == '_') || 
D 11
	      (*line_ptr == '-') || (*line_ptr == '.')))
E 11
I 11
	      (*line_ptr == '-') || (*line_ptr == '.') ||
D 12
	       (*line_ptr == ':')))
E 12
I 12
	       (*line_ptr == ':') || (*line_ptr == '/')))
E 12
E 11
      {
	
	if ((is_number) && 
	    (((!isdigit(*line_ptr)) && (*line_ptr != '.')) 
	     || (found_neg > 1) || (found_dec > 1)))
	{
	  DEBUG(cout << "Get_Token: Token is *NOT* a number" << endl;);
	  
	  is_number = 0;
	}
	  
	
	if (*line_ptr == '.')
	  found_dec++;
	else if (*line_ptr == '-')
	  found_neg++;

I 17
// The following code was commented out because EPICS really wants to
// put decimal points in the field names.  Change made on 3/17/00 by
// jgh at the request of Elliott Wolin.
#ifdef ORIGINALCODE
E 17
	// If the token is not numeric, it cannot have decimal points.
	// So, if we find a decimal point and we know that we are not
	// a number then we'd better exit.  
	// Since a simple exit will not work, we will throw an exception
	// here and flag an illegal identifier.
	if ((!is_number) && found_dec)
	{
	  DEBUG(cout << "Got_Token: Bad identifier, throwing exception"
		<< endl << "     found a decimal point in alphanumeric"
		<< endl;
	    );
	  
	  throw ParseExcept("Illegal Identifier",
			    "Decimal points not allowed in identifiers");
	}
D 17
	
E 17
I 17
#endif	
E 17
	
	cur_tok.data[i++] = *line_ptr++;
	cur_tok.data[i] = 0;	// be sure we always have a null.
      }
      DEBUG(cout << "Get_Token: Got a(n) ";);
      
      if (is_number)
      {
	DEBUG(cout << "NUMERIC ";);
	
	cur_tok.type = t_number;
      }
      else
      {
	DEBUG(cout << "ALPHANUMERIC ";);
	
	cur_tok.type = t_charname;
      }
      
      got_a_token++;
      cur_tok.data[i] = 0;
      DEBUG(cout << "token = " << cur_tok.data << endl;);
      
    }
    else
    {
      DEBUG(cout << "Get_Token: Got a non-alpha token" << endl;);
      
      switch (*line_ptr)
      {
      case '"':			// This will be a general string
	DEBUG(cout << "Get_Token: Found a literal quote." << endl;);
	
	cur_tok.type = t_quote;
	line_ptr++;
	i = 0;
	while (*line_ptr != '"')
	{
	  if (*line_ptr)
	  {
	    if (i < maxTokenLength)
	    {
	      cur_tok.data[i++] = *line_ptr++;
	    }
	    else
	    {
	      DEBUG(cout << "Get_Token: Input line is too long" << endl;);
	      return t_invalid;	// Token too long!
	    }
	  }
	  else
	  {			// oops - no closing quote.
	    DEBUG(cout << "Get_Token: No closing quote found." << endl;);
	    
	    return t_invalid;
	  }
	}
	// If we exit the loop, then *line_ptr = '"' so step
	// once more.
	cur_tok.data[i] = 0;
	line_ptr++;
	got_a_token++;		// Got one!
	DEBUG(cout << "Get_Token: Literal quote found and complete" << endl;);
I 10
	DEBUG(cout << "           Value = " << cur_tok.data << endl;);
E 10
	break;
	
      case '(':			// beginning of an array index
	DEBUG(cout << "Get_Token: Left Paren found" << endl;);
	
	cur_tok.type = t_lparen;
	cur_tok.data[0] = '(';
	cur_tok.data[1] = 0;
	line_ptr++;
	got_a_token++;
	break;
	
      case ')':			// end of an array index
	DEBUG(cout << "Get_Token: Right Paren found" << endl;);
	
	cur_tok.type = t_rparen;
	cur_tok.data[0] = ')';
	cur_tok.data[1] = 0;
	line_ptr++;
	got_a_token++;
	break;
	
      default:			// Oops! we got garbage
	DEBUG(cout << "Get_Token: Bad character found ->" << *line_ptr
	      << "<- " << endl;);
	
	line_ptr++;
	return t_invalid;
      }
    }
  }
  
  // if we get here, then we found a valid token.
  DEBUG(cout << "Get_Token: Valid token found - returning" << endl;);
  
  return cur_tok.type;
	
}

//**********************************************************************
// valid_line()
// Checks to be sure that we have a valid input line for the tokenizer.
// A valid line is one which there is at least one non-whitespace
// character.
//**********************************************************************
int valid_line(istream &in)
{
  int valid = 0;
  
  while (!valid)
  {
    line_ptr = stripwhitespace(line_ptr);
    if (*line_ptr)
      valid++;
    else
    {
      int len;
      len = get_next_line(in);
      if (len < 0)
	break;
    }  
  }
  return valid;
}

//**********************************************************************
// stripwhitespace()
// Scans along a character string for a non-whitespace character.
// Returns a pointer to the first non-white character in the string.
//**********************************************************************
char *stripwhitespace(char *s)
{
  while ((*s) && isspace(*s))
    s++;
  return s;
}


//**********************************************************************
// tok_putback()
// Pushes the current token back onto the token stack so it can be
// reused later.  This avoids having to backtrack when we read a token
// that we cannot use in the current context.
//**********************************************************************
inline void tok_putback()
{
  DEBUG(cout << "Put back token: " << cur_tok.data 
	<< " type " << printtype(cur_tok.type) << endl;);
  
  cur_tok.putback = 1;
}



//**********************************************************************
// Parser
// The following routines implement a recursive descent parser for the
// configuration file language.  The parser builds three lists, the
// input list, the output list and the alias list.  After parsing is
// complete, the input list will contain correct input structures, the
// output list will contain correct output structures and the alias
// list will contain fully linked aliases.  The parser can throw any
// number of exceptions so there is one giant collection of catch
// blocks to be sure that we grab them.
//**********************************************************************

llist inputlist;
llist outputlist;
llist aliaslist;

//**********************************************************************
D 6
// parse_config
E 6
I 6
// read_config
E 6
// This function starts the parse of the configuration file.  It is
// passed the name of the configuration file, opens the file and
// sets up the various pointers.  It then calls the first parse routine
// to construct the lists.  This routine is also the one which catches
// any exceptions thrown by the parse routines.
//**********************************************************************
D 16
int read_config(char *filename)
E 16
I 16
int read_config(const char *filename)
E 16
{
  ifstream in(filename);
  int returnval = 0;
  
  DEBUG(cout << "ReadConfig: beginning to process config file" 
	<< filename << endl;);
  
  if (!in)
  {
    cout << "Error in READ_CONFIG: cannot open file " 
	 << filename << endl;
    return 0;
  }

  line_buf[0] = 0;
  line_ptr = line_last = last_ptr = line_buf;
  
  try
  {
    returnval = parse_config(in);
  }
  catch (ParseExcept &pe)
  {
    cout << "Read_Config caught a parser execption while processing"
	 << endl << "the configuration file " << filename << endl;
    cout << "Type : " << pe.type() << endl;
    cout << "Mesg : " << pe.msg() << endl;
    cout << "Current Line: " << line_buf << endl;
    cout << "Current Token: " << cur_tok.data << endl;
    cout << "Current Token Type: " << printtype(cur_tok.type) << endl;
    inputlist.clear();
    outputlist.clear();
    aliaslist.clear();
    return 0;
  }

  if (!returnval)		// some kind of bad parse
  {
    cerr << "Read_Config: uncaught syntax error while processing " << endl
	 <<"file " << filename << endl << "Parse Abandoned" << endl;
    inputlist.clear();
    outputlist.clear();
    aliaslist.clear();
  }
  DEBUG(cout << "Returning from READ_CONFIG" << endl;);
  
  return returnval;
}


//**********************************************************************
// parse_config
// The first routine in the parser proper.  This routine looks for the
// top level input file structure - a sequence of complete input and
// output sources.  It checks for illegal statements at the top level
// and throws an exception if it finds one.
//**********************************************************************
int parse_config(istream &infile)
{
  tokentype retval;
  
  DEBUG(cout << "\n\nParseConfig entered" << endl;);
  
  while ((retval = get_token(infile)) != t_invalid)
  {
    DEBUG(cout << "ParseConfig: Got a valid token: "
	  << cur_tok.data << " with type " << cur_tok.type << endl;);
    
    switch (retval)
    {
    case t_input:		// start an input source
    {
      input *in = new input;
      if (process_input(in,infile))
D 9
	inputlist.insert(in);
E 9
I 9
      {
	if (check_tagged_syntax(in))
	  inputlist.insert(in);
	else
	{
	  cout << "ParseConfig: ERROR - Input element "
	       << in->name() << " Contains both tagged and" << endl;
	  cout << "untagged data elements.  Not Allowed." << endl;
	  delete in;
	}
      }
E 9
      else
	delete in;
D 9
      
E 9
      break;
    }
    case t_output:		// start an output source
    {
      output *out = new output;
      if (process_output(out,infile))
	outputlist.insert(out);
      else
	delete out;
      
      break;
    }
    case t_invalid:		// end of file
      break;
    default:			// oops!  error in file
      DEBUG(cout << "ParseConfig: Token was not INPUT or OUTPUT"
	    << endl << "    Throwing an exception!" << endl;);
      throw ParseExcept("ParseConfig - invalid token type",
			"Token was not an input or output stream");
    }
  }
  return 1;
}

//**********************************************************************
// process_input()
// Checks the configuration file for a valid set of commands to
// specify the source and structure of an input stream.  This routine
// gets called when an "input" token is seen in the file.  The current
// routine looks for any number of acceptable commands, followed by
// an "end" token.
E 3
//
D 3
// Version: %A% %I% 
//
// Programmer: J.G. Hardie
E 3
I 3
// Any invalid command will cause an exception to be thrown.
// The program assumes that the input token has already been read and
// that the name should be the next token in the file.
E 3
//**********************************************************************
I 3
int process_input(input *in, istream &infile)
{
  tokentype retval;
  
  DEBUG(cout << "\nProcessInput: entered - checking for input name" << endl;);
  
  retval = get_token(infile);
  if (retval != t_charname)
    throw ParseExcept("ProcessInput - no name specified for input stream",
		      "INPUT specifier was found with no corresponding name");
  
  in->name(cur_tok.data);	// set the input stream name
D 11
  in->dgroup("info_server");	// default datagroup is "info_server"
E 11
I 11
  in->dgroup("/info_server/in/",cur_tok.data);
                     // default datagroup is "info_server/in/name"
E 11
E 3

D 3
#include "info_includes.hh"
#include "info_proto.hh"
#include "info_globals.hh"
#include "info_classes.hh"
E 3
I 3
  DEBUG(cout << "ProcessInput: Name OK, reading the block contents"
	<< endl; );
  
  while ((retval = get_token(infile)) != t_end)
  {
    if (retval == t_invalid)
    {
      throw ParseExcept("ProcessInput: invalid token found",
			"Unexpected t_invalid when looking for elements");
    }
    
    DEBUG(cout << "ProcessInput: Got a token -> " << cur_tok.data
	  << " type -> " << cur_tok.type << endl;);
    
    switch (retval)
    {
    case t_datagroup:
      retval = get_token(infile);
D 10
      if (retval != t_charname)
E 10
I 10
      if ((retval != t_charname) && (retval != t_string))
E 10
	throw ParseExcept("ProcessInput - no name for datagroup",
			  "DATAGROUP found with no corresponding name");
D 11
      in->dgroup(cur_tok.data);
E 11
I 11
      in->dgroup("",cur_tok.data);
E 11
      break;
    default:
D 11
      input_element(in->iolist(),retval,infile);
E 11
I 11
      input_element(in,retval,infile);
E 11
    }
  }
  DEBUG(cout << "ProcessInput: Complete INPUT block found" << endl;);
  
  return 1;
}
E 3

I 3
//**********************************************************************
// input_element
// processes individual commands that are valid in an input source
// specification.
//**********************************************************************
D 11
void input_element(llist &l, tokentype retval, istream &infile)
E 11
I 11
void input_element(input *in, tokentype retval, istream &infile)
E 11
{
  DEBUG(cout << "\nInputElement: entered, looking for operation" << endl;);
I 11
  char namebuf[10000];
  
  llist &l = in->iolist();
E 11
  
  switch (retval)
  {
I 17
  case t_lifetime:
  {
    // This is a line of the form "lifetime <nnn>" where <nnn> is
    // the number of seconds that this input field should remain
    // valid.
    retval = get_token(infile);
    if (retval != t_number)
    {
      throw ParseExcept("InputElement - no number for lifetime",
			"LIFETIME found without a time interval");
    }
    int timeval = atoi(cur_tok.data);
    in->setlifetime(timeval);
    break;
  }
E 17
  case t_integer:
  {
    retval = get_token(infile);
D 10
    if (retval != t_charname)
E 10
I 10
    if ((retval != t_charname) && (retval != t_quote))
E 10
      throw ParseExcept("InputElement - no name for integer",
			"INTEGER found with no corresponding name");
    integer *i = new integer;
D 11
    i->name(cur_tok.data);
E 11
I 11
    strcpy(namebuf,in->name());
    strcat(namebuf,"::");
    strcat(namebuf,cur_tok.data);
    i->name(namebuf);
E 11
    l.insert(i);
    break;
  }
  case t_floating:
  {
    retval = get_token(infile);
D 10
    if (retval != t_charname)
E 10
I 10
    if ((retval != t_charname) && (retval != t_quote))
E 10
      throw ParseExcept("InputElement - no name for floating",
			"FLOATING found with no corresponding name");
    floating *i = new floating;
    i->name(cur_tok.data);
I 11
    strcpy(namebuf,in->name());
    strcat(namebuf,"::");
    strcat(namebuf,cur_tok.data);
    i->name(namebuf);
E 11
    l.insert(i);
    break;
  }
  case t_alias:
  {
    retval = get_token(infile);
D 10
    if (retval != t_charname)
E 10
I 10
    if ((retval != t_charname) && (retval != t_quote))
E 10
      throw ParseExcept("InputElement - no name for alias",
			"ALIAS found with no corresponding name");
    alias *i = new alias;
    i->name(cur_tok.data);
I 11
    strcpy(namebuf,in->name());
    strcat(namebuf,"::");
    strcat(namebuf,cur_tok.data);
    i->name(namebuf);
E 11
    process_alias(i,infile);
    // aliaslist.insert(i);          // Not handled yet.
    break;
  }
  case t_block:
  {
    retval = get_token(infile);
D 10
    if (retval != t_charname)
E 10
I 10
    if ((retval != t_charname) && (retval != t_quote))
E 10
      throw ParseExcept("InputElement - no name for block",
			"BLOCK found with no corresponding name");
    block *i = new block;
    i->name(cur_tok.data);
I 11
    strcpy(namebuf,in->name());
    strcat(namebuf,"::");
    strcat(namebuf,cur_tok.data);
    i->name(namebuf);
E 11
    process_block(i,infile);
    // l.insert(i);                 // Not handled yet.
    break;
  }
  case t_array:
  {
    retval = get_token(infile);
D 10
    if (retval != t_charname)
E 10
I 10
    if ((retval != t_charname) && (retval != t_quote))
E 10
      throw ParseExcept("InputElement - no name for array",
			"ARRAY found with no corresponding name");
    array *i = new array;
    i->name(cur_tok.data);
I 11
    strcpy(namebuf,in->name());
    strcat(namebuf,"::");
    strcat(namebuf,cur_tok.data);
    i->name(namebuf);
E 11
    retval = get_token(infile);
    if ((retval != t_integer) && (retval != t_floating))
      throw ParseExcept("InputElement - invalid array type found",
			"Arrays can only be type FLOATING or INTEGER");
    i->arr_type( (retval==t_integer) ? arr_int : arr_float);
    l.insert(i);
    break;
  }
I 8
  case t_tagged:
  {
    retval = get_token(infile);
D 10
    if (retval != t_charname)
E 10
I 10
    if ((retval != t_charname) && (retval != t_quote))
E 10
      throw ParseExcept("InputElement - no name for tagged array",
			"TAGGED found with no corresponding name");
    tagged *i = new tagged;
D 11
    i->name(cur_tok.data);
E 11
I 11
    i->tagname(cur_tok.data);
    strcpy(namebuf,in->name());
    strcat(namebuf,"::");
    strcat(namebuf,cur_tok.data);
    i->name(namebuf);
E 11
    retval = get_token(infile);
    if ((retval != t_integer) && (retval != t_floating))
      throw ParseExcept("InputElement - invalid array type found",
			"Arrays can only be type FLOATING or INTEGER");
    i->arr_type( (retval==t_integer) ? arr_int : arr_float);
    l.insert(i);
    break;
  }
E 8
  case t_string:
  {
    retval = get_token(infile);
D 10
    if (retval != t_charname)
E 10
I 10
    if ((retval != t_charname) && (retval != t_quote))
E 10
      throw ParseExcept("InputElement - no name for string",
			"STRING found with no corresponding name");
    cstring *i = new cstring;
    i->name(cur_tok.data);
I 11
    strcpy(namebuf,in->name());
    strcat(namebuf,"::");
    strcat(namebuf,cur_tok.data);
    i->name(namebuf);
E 11
    l.insert(i);
    break;
  }
  default:
    DEBUG(cout << "InputElement: Token type not acceptable in an "
	  << endl << "    input block.  Throwing exception for token type "
	  << printtype(retval) << endl;);
    
    throw ParseExcept("InputElement - invalid command in input stream",
		      "Illegal definition in an input/block specifier");
  }

  DEBUG(cout << "InputElement: Valid input element found" << endl;);
  
}

//**********************************************************************
// process_output
// Main driver routine for parsing an output data stream specification.
//**********************************************************************
int process_output(output *out, istream &infile)
{
  tokentype retval;
  llist ignoreme;
  lnode *ig,*loc;
  
  
  DEBUG(cout << "ProcessOutput: entered, looking for output block" << endl;);
  retval = get_token(infile);
  if (retval != t_charname)
    throw ParseExcept("ProcessOutput - no name specified for output stream",
		      "OUTPUT specifier was found with no corresponding name");
  
  out->name(cur_tok.data);	// set the input stream name
D 11
  out->dgroup(cur_tok.data);	// default datagroup is same as name
E 11
I 11
  out->dgroup("/info_server/out/",cur_tok.data);	// default datagroup is same as name
E 11

  DEBUG(cout << "ProcessOutput: Name OK, reading the block contents"
	<< endl; );
  
  while ((retval = get_token(infile)) != t_end)
  {
    if (retval == t_invalid)
    {
      throw ParseExcept("ProcessOutput: invalid token found",
			"Unexpected t_invalid when looking for elements");
    }
    
    DEBUG(cout << "ProcessOutput: Got a token -> " << cur_tok.data
	  << " type -> " << cur_tok.type << endl;);
    
    switch (retval)
    {
    case t_datagroup:
      retval = get_token(infile);
      if (retval != t_charname)
	throw ParseExcept("ProcessOutput - no name for datagroup",
			  "DATAGROUP found with no corresponding name");
D 11
      out->dgroup(cur_tok.data);
E 11
I 11
      out->dgroup("",cur_tok.data);
E 11
      break;
    case t_name:
    {
      lnode *n = new lnode;
      n->name(out->name());
      n->type(t_name);
      out->insert(n);
      break;
    }
    case t_literal:
    {
      literal *l = new literal;
      retval = get_token(infile);
      if (retval != t_quote)
	throw ParseExcept("ProcessOutput - no string in literal",
			  "LITERAL command had no literal string");
      l->name(out->name());
      l->str(cur_tok.data);
      out->insert(l);
      break;
    }
I 11
    case t_timestamp:
    {
      retval = get_token(infile);
      if (retval != t_charname)
	throw ParseExcept("ProcessOutput - no name in timestamp",
			  "timestamp with no input message name given");
      out_comp *l = new out_comp;
      l->name(cur_tok.data);
      l->type(t_timestamp);
      out->insert(l);
      break;
    }
E 11
    case t_reference:
    {
      retval = get_token(infile);
D 10
      if (retval != t_charname)
E 10
I 10
      if ((retval != t_charname) && (retval != t_quote))
E 10
	throw ParseExcept("ProcessOutput - no name in reference",
			  "REFERENCE with no referrent found. Need name.");
    }
I 10
    case t_quote:
E 10
    case t_charname:
    {
      out_comp *l = new out_comp;
      l->name(cur_tok.data);
      l->type(t_reference);
      out->insert(l);
      break;
    }
I 6
    case t_timeout:
    {
      retval = get_token(infile);
      if (retval != t_number)
      {
	throw ParseExcept("ProcessOutput - no timeout specified",
			  "TIMEOUT must have time specified in seconds");
      }
D 7
      int timeout = atoi(curtok.data);
      out.timeout(timeout);
E 7
I 7
      int timeout = atoi(cur_tok.data);
      out->timeout(timeout);
E 7
      break;
    }
E 6
    case t_ignore:
    {
      lnode *l = new lnode;
      retval = get_token(infile);
D 10
      if (retval != t_charname)
E 10
I 10
      if ((retval != t_charname) && (retval != t_quote))
E 10
	throw ParseExcept("ProcessOutput: Ignore without referent",
			  "IGNORE command must specify name to ignore");
      l->name(cur_tok.data);
      l->type(t_ignore);
      ignoreme.insert(l);
      break;
    }
    default:
      DEBUG(cout << "ProcessOutput: Illegal command in output" << endl;);
      throw ParseExcept("ProcessOutput - illegal output command",
			"Unexpected command found in output block");
      
    }
  }
  DEBUG(cout << "ProcessOutput: Complete OUTPUT block found" << endl;);
  
  // At this point, we have a complete output specification.  We have
  // to process the ignore list and flag the items to be ignored.  The
  // find routines in the linked list class had better work...
  ignoreme.top();
  while ((ig = ignoreme.current()))
  {
    ignoreme.next();
    
    DEBUG(cout << "ProcessOutput: Linking up the ignore clauses" << endl;);
    
    if (!(loc = out->find(ig->name())))
      throw ParseExcept("ProcessOutput: WARNING - ignore not found",
			"IGNORE refers to a name but it is not present");

    if (loc->type() != t_reference)
      throw ParseExcept("ProcessOutput: invalid ignore reference",
			"IGNORE refers to non-data item");
    out_comp *oc = (out_comp *)loc;
    oc->ignoretime = 1;
  }
  
  // we finally have a valid output object.
  return 1;
}



//***********************************************************************
// process_block
// This function handles the parsing and storage of the BLOCK command.
// Essentially, the block command is a recursive call to the input
// command handler, except that the block ignores the effect of several
// commands that are allowed in the input structure.  To allow for
// easier future modification, we split the block handling out into a
// separate function.
//**********************************************************************
void process_block(block *bl, istream &infile)
{
  tokentype retval;
  
  DEBUG(cout << "ProcessBlock: entered, looking for block" << endl;);
  
  // We don't impliment the block yet.  To avoid trouble we have to
  // scan past the "END" command so that the input tokens don't get
  // out of synch.  
  // WARNING - if you put block commands in the configuration file,
  // they will be parsed, but they will not be used in splitting up
  // the incoming data messages.
  while ((retval = get_token(infile)) != t_end)
  {
    switch (retval)
    {
    case t_invalid:
    {
      throw ParseExcept("ProcessBlock: invalid token found",
			"BLOCK command encountered incomplete data");
    }
    default:
      DEBUG(cout << "    Token found of type: " << printtype(retval)
	    << endl << "    With name: " << cur_tok.data << endl;);
    }
  }
  
  cout << "BLOCK command not yet implimented.  Will be ignored." << endl;
}

//**********************************************************************
// process_alias
// function scans the input stream for the correct number (either 2 or
// 5) tokens following the detection of an "alias" command.  The
// command must be followed by at least two names, and possibly by
// an array reference.
//**********************************************************************
void process_alias(alias *al, istream &infile)
{
  tokentype retval;
  
  DEBUG(cout << "ProcessAlias: entered, looking for alias" << endl;);
  
  // The alias MUST be followed by two names.  If we get this far, then
  // the first required name has already been found and it remains 
  // to determine if the second name is a simple name or an array
  // reference.

  retval = get_token(infile);
D 10
  if (retval != t_charname)	// UhOh!  command is hosed.
E 10
I 10
  if ((retval != t_charname) && (retval != t_quote))	// UhOh!  command is hosed.
E 10
    throw ParseExcept("ProcessAlias: no data reference",
		      "ALIAS must refer to a defined data fragment");
  
  // So, it is a name.  Now check to see if we got an array reference
  // also.
  retval = get_token(infile);
  if (retval != t_lparen)
  {
    tok_putback();
  }
  else
  {
    retval = get_token(infile);	// the number
    if (retval != t_number)
      throw ParseExcept("ProcessAlias: bad reference",
			"Array reference must be a single number");
    retval = get_token(infile);	// the right paren.
    if (retval != t_rparen)
      throw ParseExcept("ProcessAlias: missing right parenthesis",
			"Right parenthesis must immediately follow reference");
  }  
  cout << "ALIAS command not yet implimented.  Will be ingnored" << endl;
  
}


//**********************************************************************
// printlist
// dumps a text representation of the nodes in a list.
//**********************************************************************
void printlist(llist &l)
{
  lnode *n;
  tokentype t;
  
  l.top();
  while(n=l.current())
  {
    l.next();
    t = n->type();
    cout << "    Token Type: " << printtype(t) << " Name: " 
	 << n->name() << endl;
    switch(t)
    {
    case t_integer:
D 17
      cout << "Value = " << ((integer *)n)->value << endl;
E 17
I 17
      cout << "Value = " << n << endl;
E 17
      break;
    case t_floating:
D 17
      cout << "Value = " << ((floating *)n)->value << endl;
E 17
I 17
      cout << "Value = " << n << endl;
E 17
      break;
    case t_string:
      cout << "Value = " << ((cstring *)n)->str() << endl;
      break;
    case t_name:
      break;
      
    case t_invalid:
    case t_end:
    case t_lparen:
    case t_rparen:
    case t_ignore:
    case t_datagroup:
    case t_quote:
      cout << "    ERROR - this type should not be in a list" << endl;
I 8
      break;
      
    case t_tagged:
      cout << "Tagged array" << endl;
E 8
      
    case t_array:
    {
      array *a = (array *)n;
      cout << "    Array Type : " << 
	((a->arr_type() == arr_int)?"integer":"floating") << endl;
D 17
      switch (a->arr_type())
      {
      case arr_int:
      {
	long *data = a->idata();
	long count = a->length();
	for (int ijk=0; ijk < count; ijk++)
	  cout << "Array[" << ijk << "]= " << data[ijk] << endl;
	break;
      }
      case arr_float:
      {
	double *data = a->ddata();
	long count = a->length();
	for (int ijk=0; ijk < count; ijk++)
	  cout << "Array[" << ijk << "]= " << data[ijk] << endl;
	break;
      }
      default:
	cout << "Unrecognized array type" << endl;
      }
E 17
I 17
      cout << n << endl;
//       switch (a->arr_type())
//       {
//       case arr_int:
//       {
// 	long *data = a->idata();
// 	long count = a->length();
// 	for (int ijk=0; ijk < count; ijk++)
// 	  cout << "Array[" << ijk << "]= " << data[ijk] << endl;
// 	break;
//       }
//       case arr_float:
//       {
// 	double *data = a->ddata();
// 	long count = a->length();
// 	for (int ijk=0; ijk < count; ijk++)
// 	  cout << "Array[" << ijk << "]= " << data[ijk] << endl;
// 	break;
//       }
//      default:
//	cout << "Unrecognized array type" << endl;
//      }
E 17
      break;
    }
    case t_alias:
      break;
    case t_literal:
    {
      literal *l = (literal *)n;
      cout << "    Literal string: " << l->str() << endl;
      break;
    }
    case t_block:
      break;
      
    case t_reference:
    {
      out_comp *oc = (out_comp *)n;
      cout << "    Component: " << oc->name() << " Ignore: " 
	   << oc->ignoretime << endl;
      break;
    }
I 8

    case t_input:
    {
      input *i = (input *)n;
      llist &il = i->iolist();
      cout << "Input Element Components:" << endl;
      printlist(il);
      break;
    }

    case t_output:
    {
      output *o = (output *)n;
      llist &ol = o->iolist();
      cout << "Output Element Components:" << endl;
      printlist(ol);
      break;
    }
I 11

    case t_timestamp:
    {
      out_comp *oc = (out_comp *)n;
      cout << "    Component: " << oc->name() << " Ignore: " 
	   << oc->ignoretime << endl;
      break;
    }
E 11
    
E 8
    default:
      cout << "    ****OOPS**** left one out" << endl;
    }
  }
}

I 18
D 19
//<<<<<<< info_classes.cc.C.k0pWZ_
E 19
//----------------------------------------------------------------------
// dumpthelist(llist &l, ostream &out)
// Writes an ASCII representation of the list to the specified output
//----------------------------------------------------------------------
void dumpthelist(llist &l,ostream &out)
{
  lnode *n;
  tokentype t;
  
  l.top();
  while(n=l.current())
  {
    l.next();
    t = n->type();
    out << "    Token Type: " << printtype(t) << " Name: " 
	 << n->name() << endl;
    switch(t)
    {
    case t_integer:
E 18
I 14
//----------------------------------------------------------------------
// dumpthelist(llist &l, ostream &out)
// Writes an ASCII representation of the list to the specified output
//----------------------------------------------------------------------
void dumpthelist(llist &l,ostream &out)
{
  lnode *n;
  tokentype t;
  
  l.top();
  while(n=l.current())
  {
    l.next();
    t = n->type();
D 15
    cout << "    Token Type: " << printtype(t) << " Name: " 
E 15
I 15
    out << "    Token Type: " << printtype(t) << " Name: " 
E 15
	 << n->name() << endl;
    switch(t)
    {
    case t_integer:
D 15
      cout << "Value = " << ((integer *)n)->value << endl;
E 15
I 15
D 17
      out << "Value = " << ((integer *)n)->value << endl;
E 17
I 17
      out << "Value = " << n << endl;
I 18
      break;
    case t_floating:
E 18
E 17
E 15
      break;
    case t_floating:
D 15
      cout << "Value = " << ((floating *)n)->value << endl;
E 15
I 15
D 17
      out << "Value = " << ((floating *)n)->value << endl;
E 17
I 17
      out << "Value = " << n << endl;
I 18
      break;
    case t_string:
      out << "Value = " << ((cstring *)n)->str() << endl;
      break;
    case t_name:
      break;
      
    case t_invalid:
    case t_end:
    case t_lparen:
    case t_rparen:
    case t_ignore:
    case t_datagroup:
    case t_quote:
      out << "    ERROR - this type should not be in a list" << endl;
      break;
      
    case t_tagged:
      out << "Tagged array" << endl;
      
    case t_array:
    {
      array *a = (array *)n;
E 18
E 17
E 15
      break;
    case t_string:
D 15
      cout << "Value = " << ((cstring *)n)->str() << endl;
E 15
I 15
      out << "Value = " << ((cstring *)n)->str() << endl;
E 15
      break;
    case t_name:
      break;
      
    case t_invalid:
    case t_end:
    case t_lparen:
    case t_rparen:
    case t_ignore:
    case t_datagroup:
    case t_quote:
D 15
      cout << "    ERROR - this type should not be in a list" << endl;
E 15
I 15
      out << "    ERROR - this type should not be in a list" << endl;
E 15
      break;
      
    case t_tagged:
D 15
      cout << "Tagged array" << endl;
E 15
I 15
      out << "Tagged array" << endl;
E 15
      
    case t_array:
    {
      array *a = (array *)n;
D 15
      cout << "    Array Type : " << 
E 15
I 15
D 17
      out << "    Array Type : " << 
E 17
I 17
      out << "Array Type : " << 
I 18
	((a->arr_type() == arr_int)?"integer":"floating") << endl;
E 18
E 17
E 15
	((a->arr_type() == arr_int)?"integer":"floating") << endl;
D 17
      switch (a->arr_type())
      {
      case arr_int:
      {
	long *data = a->idata();
	long count = a->length();
	for (int ijk=0; ijk < count; ijk++)
D 15
	  cout << "Array[" << ijk << "]= " << data[ijk] << endl;
E 15
I 15
	  out << "Array[" << ijk << "]= " << data[ijk] << endl;
E 15
	break;
      }
      case arr_float:
      {
	double *data = a->ddata();
	long count = a->length();
	for (int ijk=0; ijk < count; ijk++)
D 15
	  cout << "Array[" << ijk << "]= " << data[ijk] << endl;
E 15
I 15
	  out << "Array[" << ijk << "]= " << data[ijk] << endl;
E 15
	break;
      }
      default:
D 15
	cout << "Unrecognized array type" << endl;
E 15
I 15
	out << "Unrecognized array type" << endl;
E 15
      }
E 17
I 17
      out << n << endl;
//       switch (a->arr_type())
//       {
//       case arr_int:
//       {
// 	long *data = a->idata();
// 	long count = a->length();
// 	for (int ijk=0; ijk < count; ijk++)
// 	  out << "Array[" << ijk << "]= " << data[ijk] << endl;
// 	break;
//       }
//       case arr_float:
//       {
// 	double *data = a->ddata();
// 	long count = a->length();
// 	for (int ijk=0; ijk < count; ijk++)
// 	  out << "Array[" << ijk << "]= " << data[ijk] << endl;
// 	break;
//       }
//       default:
// 	out << "Unrecognized array type" << endl;
//       }
I 18
      break;
    }
    case t_alias:
      break;
    case t_literal:
    {
      literal *l = (literal *)n;
      out << "    Literal string: " << l->str() << endl;
      break;
    }
    case t_block:
      break;
      
    case t_reference:
    {
      out_comp *oc = (out_comp *)n;
      out << "    Component: " << oc->name() << " Ignore: " 
	   << oc->ignoretime << endl;
      break;
    }

    case t_input:
    {
      input *i = (input *)n;
E 18
E 17
      break;
    }
    case t_alias:
      break;
    case t_literal:
    {
      literal *l = (literal *)n;
D 15
      cout << "    Literal string: " << l->str() << endl;
E 15
I 15
      out << "    Literal string: " << l->str() << endl;
E 15
      break;
    }
    case t_block:
      break;
      
    case t_reference:
    {
      out_comp *oc = (out_comp *)n;
D 15
      cout << "    Component: " << oc->name() << " Ignore: " 
E 15
I 15
      out << "    Component: " << oc->name() << " Ignore: " 
E 15
	   << oc->ignoretime << endl;
      break;
    }

    case t_input:
    {
      input *i = (input *)n;
D 17
      llist &il = i->iolist();
E 17
I 17
      llist &il = i->input_data();
I 18
      out << "Input Element Components:" << endl;
E 18
E 17
D 15
      cout << "Input Element Components:" << endl;
E 15
I 15
      out << "Input Element Components:" << endl;
E 15
D 17
      printlist(il);
E 17
I 17
      dumpthelist(il,out);
I 18
      break;
    }

    case t_output:
    {
      output *o = (output *)n;
      llist &ol = o->iolist();
      out << "Output Element Components:" << endl;
E 18
E 17
      break;
    }

    case t_output:
    {
      output *o = (output *)n;
      llist &ol = o->iolist();
D 15
      cout << "Output Element Components:" << endl;
E 15
I 15
      out << "Output Element Components:" << endl;
E 15
D 17
      printlist(ol);
E 17
I 17
      dumpthelist(ol,out);
I 18
      break;
    }

    case t_timestamp:
    {
      out_comp *oc = (out_comp *)n;
      out << "    Component: " << oc->name() << " Ignore: " 
	   << oc->ignoretime << endl;
      break;
    }
    
    default:
      out << "    ****OOPS**** left one out" << endl;
    }
  }
}



E 18
E 17
      break;
    }

    case t_timestamp:
    {
      out_comp *oc = (out_comp *)n;
D 15
      cout << "    Component: " << oc->name() << " Ignore: " 
E 15
I 15
      out << "    Component: " << oc->name() << " Ignore: " 
E 15
	   << oc->ignoretime << endl;
      break;
    }
    
    default:
D 15
      cout << "    ****OOPS**** left one out" << endl;
E 15
I 15
      out << "    ****OOPS**** left one out" << endl;
E 15
    }
  }
}



E 14
D 16
char *t_tokennames[] = {
E 16
I 16
const char *t_tokennames[] = {
E 16
D 8
  "Invalid", "Input", "Output", "Integer", "Floating", "Array",
E 8
I 8
  "Invalid", "Input", "Output", "Integer", "Floating", "Array", "Tagged",
E 8
  "Block", "Alias", "End", "Datagroup", "Name", "Literal", "Ignore", 
D 6
  "String","Reference", "Lparen", "Rparen", "Quote", "Number", "CharName" 
E 6
I 6
D 11
  "Timeout", "String","Reference", "Lparen", "Rparen", "Quote", 
E 11
I 11
  "Timeout", "String","Reference", "TimeStamp", "Lparen", "Rparen", "Quote", 
E 11
  "Number", "CharName" 
E 6
};

long t_numtokens = sizeof(t_tokennames)/sizeof(char *);

D 14
D 18
  const char * const printtype(int t)
  {
    
    if ((t >= 0) && (t < t_numtokens))
      return t_tokennames[t];
    else
      return "Index out of range";
  }
E 18
I 18
const char * const printtype(int t)
{
  
  if ((t >= 0) && (t < t_numtokens))
    return t_tokennames[t];
  else
    return "Index out of range";
}
E 18
E 14
I 14
const char * const printtype(int t)
{
  
  if ((t >= 0) && (t < t_numtokens))
    return t_tokennames[t];
  else
    return "Index out of range";
}
E 14

D 14
D 18
  inline const long numtokens()
  {
    return(t_numtokens);
  }
E 18
I 18
inline const long numtokens()
{
  return(t_numtokens);
}
E 18
E 14
I 14
inline const long numtokens()
{
  return(t_numtokens);
}
E 14
I 9


//----------------------------------------------------------------------
// check_tagged_syntax()
// This scans through the component list of an input element and
// determines if any of the input elements are tagged arrays.  If they
// are, it checks to see if all of the input elements are tagged
// arrays.  
// If this is the case, true is returned, otherwise false.
//----------------------------------------------------------------------
int check_tagged_syntax(input *in)
{
  int num_tagged = 0;
  int num_nontagged = 0;
  
  llist &l = in->iolist();
  lnode *n;

  DEBUG(
    cout << "DEBUG: Checking tagged syntax" << endl;);
  
   
  
  l.top();
  
  while (n = l.current())
  {
    l.next();
    
    switch(n->type())
    {
    case t_tagged:
      num_tagged++;
      break;
    default:
      num_nontagged++;
      break;
    }
  }
  
  DEBUG(
    cout << "Tagged = " << num_tagged << endl;
    cout << "NonTagged = " << num_nontagged << endl;
    if ( !(num_tagged && num_nontagged))
    {
      cout << "  -- Syntax OK " << endl;
    }
    else
    {
      cout << "  -- Syntax BAD" << endl;
    }
    );
  
	   

  return !( num_tagged && num_nontagged);
}

  
  
E 9
E 3

E 1
