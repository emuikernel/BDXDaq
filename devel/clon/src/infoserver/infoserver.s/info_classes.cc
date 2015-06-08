//*****************************************************************************
// scaparse.cpp
// Parser routines for the info_server program.  Reads an input command
// file and splits it into a parse tree.
//
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
// Programmed by: jgh
// Date: May 1997
//*****************************************************************************

//--------
// JGH902 - Switch to new C++ header files.  Force inclusion of standard
// namespace so that the rest of the code continues to work.
//
// The standard include files
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

// The parser definitions.  Contains definitions for:
// 1) Token types
//		- input, output, integer, floating, array, block, alias
//		- end, string, literal, datagroup, name, quote, lparen, rparen
// 2) Token structure (token)
// 3) Parse table classes:
#include "info_classes.hh"

// Function prototypes
void printlist(llist &l);
void process_alias(alias *al, istream &infile);
void process_block(block *bl, istream &infile);
void input_element(input *in, tokentype t, istream &i);
int process_input(input *in, istream &infile);
int process_output(output *out, istream &infile);
int parse_config(istream &in);
int read_config(char *f);
int get_next_line(istream &in);
inline const long numtokens();
int countquotes(char *p);
tokentype get_token(istream &in);
int valid_line(istream &in);
char *stripwhitespace(char *s);
inline void tok_putback();
int check_tagged_syntax(input *in);

ostream &operator<<(ostream &out, lnode *x)
{
  return x->Print(out);
}

ostream &operator<<(ostream &out, lnode &x)
{
  return x.Print(out);
}



//**********************************************************************
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
  //-------
  // JGH902 - we need some C++ string objects for the new read
  // methods
  //-------
  string stemp,stemp2;
  int i;
  int linelength;

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

  //-------
  // JGH902 - Switch to C++ string I/O
  //
  //in.get(line_ptr,maxLineLen,'\n');	// read to end of line
  //-------
  getline(in,stemp);
  

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
  
  // Now check to see if we got an even number of quotation marks.
  // an odd number of quotation marks indicates that we need to keep
  // filling the line buffer.  At this point, we make the assumption
  // that the end of a quote will be on its own line and that the
  // next command will *NOT* start on the same line.  If this is not
  // the case, an error will probably be generated in the parse somewhere.
  // You might get lucky, you might not.
  line_buf[0] = 0;

  //-------
  // JGH902 - Put chars in C-style buffer for countquotes function.
  //-------
  strcpy(line_buf,stemp.c_str());
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
  }

  //-------
  // JGH902 - Put string chars in C-style buffer for rest of code.
  //-------
  strcpy(line_buf,stemp.c_str());
  line_ptr = line_buf;
  line_last = line_ptr + linelength - 1;
  while (*line_ptr)
  {
    if (isalpha(*line_ptr))
      *line_ptr = tolower(*line_ptr);
    line_ptr++;
  }
  line_ptr = line_buf;
  DEBUG(cout << "Downcased.  Line = " << line_ptr << endl;);

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
//
// NOTE: get_token assumes that get_next_line has been done before it
// is called.
//**********************************************************************

const char *lit_token[] = {
  "jgh", "input", "output", "integer", "floating", "array", "tagged", "block",
  "alias", "end", "datagroup", "name", "literal", "ignore", "timeout", 
    "string", "reference", "timestamp", "lifetime"
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
    if (isalnum(*line_ptr) || (*line_ptr == '_') || (*line_ptr == '-')
      || (*line_ptr == '.') || (*line_ptr == ':') || (*line_ptr == '/'))
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
	      (*line_ptr == '-') || (*line_ptr == '.') ||
	       (*line_ptr == ':') || (*line_ptr == '/')))
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

// The following code was commented out because EPICS really wants to
// put decimal points in the field names.  Change made on 3/17/00 by
// jgh at the request of Elliott Wolin.
#ifdef ORIGINALCODE
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
#endif	
	
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
	DEBUG(cout << "           Value = " << cur_tok.data << endl;);
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
// read_config
// This function starts the parse of the configuration file.  It is
// passed the name of the configuration file, opens the file and
// sets up the various pointers.  It then calls the first parse routine
// to construct the lists.  This routine is also the one which catches
// any exceptions thrown by the parse routines.
//**********************************************************************
int read_config(const char *filename)
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
      else
	delete in;
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
//
// Any invalid command will cause an exception to be thrown.
// The program assumes that the input token has already been read and
// that the name should be the next token in the file.
//**********************************************************************
int process_input(input *in, istream &infile)
{
  tokentype retval;
  
  DEBUG(cout << "\nProcessInput: entered - checking for input name" << endl;);
  
  retval = get_token(infile);
  if (retval != t_charname)
    throw ParseExcept("ProcessInput - no name specified for input stream",
		      "INPUT specifier was found with no corresponding name");
  
  in->name(cur_tok.data);	// set the input stream name
  in->dgroup("/info_server/in/",cur_tok.data);
                     // default datagroup is "info_server/in/name"

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
      if ((retval != t_charname) && (retval != t_string))
	throw ParseExcept("ProcessInput - no name for datagroup",
			  "DATAGROUP found with no corresponding name");
      in->dgroup("",cur_tok.data);
      break;
    default:
      input_element(in,retval,infile);
    }
  }
  DEBUG(cout << "ProcessInput: Complete INPUT block found" << endl;);
  
  return 1;
}

//**********************************************************************
// input_element
// processes individual commands that are valid in an input source
// specification.
//**********************************************************************
void input_element(input *in, tokentype retval, istream &infile)
{
  DEBUG(cout << "\nInputElement: entered, looking for operation" << endl;);
  char namebuf[10000];
  
  llist &l = in->iolist();
  
  switch (retval)
  {
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
  case t_integer:
  {
    retval = get_token(infile);
    if ((retval != t_charname) && (retval != t_quote))
      throw ParseExcept("InputElement - no name for integer",
			"INTEGER found with no corresponding name");
    integer *i = new integer;
    strcpy(namebuf,in->name());
    strcat(namebuf,"::");
    strcat(namebuf,cur_tok.data);
    i->name(namebuf);
    l.insert(i);
    break;
  }
  case t_floating:
  {
    retval = get_token(infile);
    if ((retval != t_charname) && (retval != t_quote))
      throw ParseExcept("InputElement - no name for floating",
			"FLOATING found with no corresponding name");
    floating *i = new floating;
    i->name(cur_tok.data);
    strcpy(namebuf,in->name());
    strcat(namebuf,"::");
    strcat(namebuf,cur_tok.data);
    i->name(namebuf);
    l.insert(i);
    break;
  }
  case t_alias:
  {
    retval = get_token(infile);
    if ((retval != t_charname) && (retval != t_quote))
      throw ParseExcept("InputElement - no name for alias",
			"ALIAS found with no corresponding name");
    alias *i = new alias;
    i->name(cur_tok.data);
    strcpy(namebuf,in->name());
    strcat(namebuf,"::");
    strcat(namebuf,cur_tok.data);
    i->name(namebuf);
    process_alias(i,infile);
    // aliaslist.insert(i);          // Not handled yet.
    break;
  }
  case t_block:
  {
    retval = get_token(infile);
    if ((retval != t_charname) && (retval != t_quote))
      throw ParseExcept("InputElement - no name for block",
			"BLOCK found with no corresponding name");
    block *i = new block;
    i->name(cur_tok.data);
    strcpy(namebuf,in->name());
    strcat(namebuf,"::");
    strcat(namebuf,cur_tok.data);
    i->name(namebuf);
    process_block(i,infile);
    // l.insert(i);                 // Not handled yet.
    break;
  }
  case t_array:
  {
    retval = get_token(infile);
    if ((retval != t_charname) && (retval != t_quote))
      throw ParseExcept("InputElement - no name for array",
			"ARRAY found with no corresponding name");
    array *i = new array;
    i->name(cur_tok.data);
    strcpy(namebuf,in->name());
    strcat(namebuf,"::");
    strcat(namebuf,cur_tok.data);
    i->name(namebuf);
    retval = get_token(infile);
    if ((retval != t_integer) && (retval != t_floating))
      throw ParseExcept("InputElement - invalid array type found",
			"Arrays can only be type FLOATING or INTEGER");
    i->arr_type( (retval==t_integer) ? arr_int : arr_float);
    l.insert(i);
    break;
  }
  case t_tagged:
  {
    retval = get_token(infile);
    if ((retval != t_charname) && (retval != t_quote))
      throw ParseExcept("InputElement - no name for tagged array",
			"TAGGED found with no corresponding name");
    tagged *i = new tagged;
    i->tagname(cur_tok.data);
    strcpy(namebuf,in->name());
    strcat(namebuf,"::");
    strcat(namebuf,cur_tok.data);
    i->name(namebuf);
    retval = get_token(infile);
    if ((retval != t_integer) && (retval != t_floating))
      throw ParseExcept("InputElement - invalid array type found",
			"Arrays can only be type FLOATING or INTEGER");
    i->arr_type( (retval==t_integer) ? arr_int : arr_float);
    l.insert(i);
    break;
  }
  case t_string:
  {
    retval = get_token(infile);
    if ((retval != t_charname) && (retval != t_quote))
      throw ParseExcept("InputElement - no name for string",
			"STRING found with no corresponding name");
    cstring *i = new cstring;
    i->name(cur_tok.data);
    strcpy(namebuf,in->name());
    strcat(namebuf,"::");
    strcat(namebuf,cur_tok.data);
    i->name(namebuf);
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
  out->dgroup("/info_server/out/",cur_tok.data);	// default datagroup is same as name

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
      out->dgroup("",cur_tok.data);
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
    case t_reference:
    {
      retval = get_token(infile);
      if ((retval != t_charname) && (retval != t_quote))
	throw ParseExcept("ProcessOutput - no name in reference",
			  "REFERENCE with no referrent found. Need name.");
    }
    case t_quote:
    case t_charname:
    {
      out_comp *l = new out_comp;
      l->name(cur_tok.data);
      l->type(t_reference);
      out->insert(l);
      break;
    }
    case t_timeout:
    {
      retval = get_token(infile);
      if (retval != t_number)
      {
	throw ParseExcept("ProcessOutput - no timeout specified",
			  "TIMEOUT must have time specified in seconds");
      }
      int timeout = atoi(cur_tok.data);
      out->timeout(timeout);
      break;
    }
    case t_ignore:
    {
      lnode *l = new lnode;
      retval = get_token(infile);
      if ((retval != t_charname) && (retval != t_quote))
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
  if ((retval != t_charname) && (retval != t_quote))	// UhOh!  command is hosed.
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
      cout << "Value = " << n << endl;
      break;
    case t_floating:
      cout << "Value = " << n << endl;
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
      break;
      
    case t_tagged:
      cout << "Tagged array" << endl;
      
    case t_array:
    {
      array *a = (array *)n;
      cout << "    Array Type : " << 
	((a->arr_type() == arr_int)?"integer":"floating") << endl;
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

    case t_timestamp:
    {
      out_comp *oc = (out_comp *)n;
      cout << "    Component: " << oc->name() << " Ignore: " 
	   << oc->ignoretime << endl;
      break;
    }
    
    default:
      cout << "    ****OOPS**** left one out" << endl;
    }
  }
}

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
      out << "Value = " << n << endl;
      break;
    case t_floating:
      out << "Value = " << n << endl;
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
      out << "Array Type : " << 
	((a->arr_type() == arr_int)?"integer":"floating") << endl;
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
      llist &il = i->input_data();
      out << "Input Element Components:" << endl;
      dumpthelist(il,out);
      break;
    }

    case t_output:
    {
      output *o = (output *)n;
      llist &ol = o->iolist();
      out << "Output Element Components:" << endl;
      dumpthelist(ol,out);
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



const char *t_tokennames[] = {
  "Invalid", "Input", "Output", "Integer", "Floating", "Array", "Tagged",
  "Block", "Alias", "End", "Datagroup", "Name", "Literal", "Ignore", 
  "Timeout", "String","Reference", "TimeStamp", "Lparen", "Rparen", "Quote", 
  "Number", "CharName" 
};

long t_numtokens = sizeof(t_tokennames)/sizeof(char *);

const char * const printtype(int t)
{
  
  if ((t >= 0) && (t < t_numtokens))
    return t_tokennames[t];
  else
    return "Index out of range";
}

inline const long numtokens()
{
  return(t_numtokens);
}


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

  
  

