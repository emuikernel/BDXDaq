h40415
s 00443/00052/01004
d D 1.11 01/01/19 15:47:41 jhardie 13 12
c fixed version problems and const B.S.
e
s 00000/00000/01056
d D 1.10 01/01/19 15:05:43 jhardie 12 11
i 10
c Merged changes between child workspace "/home/jhardie/scalers/info_server" and
c  parent workspace "/usr/local/clas/devel/source".
c 
e
s 00014/00014/01046
d D 1.9 01/01/09 16:20:14 jhardie 11 9
c Fixed const problems for ANSI compliance
c 
e
s 00000/00004/01056
d D 1.8.1.1 99/06/15 13:24:38 jhardie 10 9
c Removed an incorrect comment.
c 
e
s 00034/00009/01026
d D 1.8 99/01/06 19:21:26 jhardie 9 8
c Modified for verions of infoserver which
c mangle names, new ss groups, and timestamping.
e
s 00038/00008/00997
d D 1.7 98/01/27 11:15:52 jhardie 8 7
c Added code and class defiinitions for handling tagged input arrays
c 
e
s 00011/00001/00994
d D 1.6 98/01/22 14:43:16 jhardie 7 6
c added functions to check the timeout, to reset the timeout and to
c decrement the time count
c 
e
s 00024/00004/00971
d D 1.5 98/01/22 13:47:33 jhardie 6 5
c Added definitions and code for handling timeouts in events.
c 
e
s 00000/00000/00975
d D 1.4 97/06/05 11:58:10 jhardie 5 4
c fixed some small problems with smartsockets arrays
e
s 00012/00009/00963
d D 1.3 97/06/05 10:40:35 jhardie 4 3
c Version 1.0 of info_server and support programs
e
s 00963/00009/00009
d D 1.2 97/06/02 18:37:13 jhardie 3 1
c Rewrite parser for full C++.  Info messages input ok,
c output still broken
e
s 00000/00000/00000
d R 1.2 97/03/25 09:44:59 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 3 2 info_server/server/info_classes.hh
c Name history : 2 1 server/info_classes.hh
c Name history : 1 0 s/server/info_classes.hh
e
s 00018/00000/00000
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
I 1
D 3
#ifndef INFO_CLASSES_H_J
#define INFO_CLASSES_H_J
E 3
I 3
#ifndef INFO_CLASSES_HJ
#define INFO_CLASSES_HJ
//***************************************************************************
// scaparse.h
// Standard header file for the info_server configuration file parse
// functions.
//
//
// **WARNING** 
// All data pointers passed *INTO* any of these classes will be deep copied
// and the object will own the copy.  The caller is free to destroy the
// original.
//
// BUT, all object links (i.e. lnodes) passed into a list are assumed
// to be owned by the list.  The caller must not destroy the object
// before explicitly removing it from the list.
//
// ALSO, all data pointers *RETURNED* by one of these classes are assumed
// to point to memory owned by the object.  The caller may not delete
// this memory at all.  The object destructor will clean up correctly
// when the object is explicitly destroyed or goes out of scope.
//
// Programmer: jgh
// Date: May 1997
//***************************************************************************

I 13
// Be sure that we have the smartsockets definitions.  Used to save the 
// internal state of the system.
#include "rtworks/cxxipc.hxx"


E 13
// A define for debugging
#ifndef DEBUG
#ifdef DEBUG_CODE
#define DEBUG(x) x
#else
#define DEBUG(x)
#endif
#endif

I 13
//typedef int bool;
//const int true=1;
//const int false=0;

E 13
const char * const printtype(int t);


// Define the types of input tokens:  Also used for data types
enum tokentype { 
D 8
  t_invalid, t_input, t_output, t_integer, t_floating, t_array, t_block,
D 6
  t_alias, t_end, t_datagroup, t_name, t_literal, t_ignore, t_string,
  t_reference, t_lparen, t_rparen, t_quote, t_number, t_charname
E 6
I 6
  t_alias, t_end, t_datagroup, t_name, t_literal, t_ignore, t_timeout, 
  t_string, t_reference, t_lparen, t_rparen, t_quote, t_number, t_charname
E 8
I 8
  t_invalid, t_input, t_output, t_integer, t_floating, t_array, t_tagged, 
  t_block, t_alias, t_end, t_datagroup, t_name, t_literal, t_ignore, 
D 9
  t_timeout, t_string, t_reference, t_lparen, t_rparen, t_quote, t_number, 
E 9
I 9
D 13
  t_timeout, t_string, t_reference, t_timestamp, t_lparen, t_rparen,
  t_quote, t_number, 
E 9
  t_charname
E 13
I 13
  t_timeout, t_string, t_reference, t_timestamp, t_lifetime, 
  // All literal tokens must be defined above this line.  The tokens
  // below are internal to the parser.
  t_lparen, t_rparen, t_quote, t_number, t_charname
E 13
E 8
E 6
};

// structure for passing back the next token.  Note that the maximum
// token size is specified to be 2048 characters.
const int maxTokenLength=2048;

struct mytoken {
  tokentype type;
  int putback;
  char data[maxTokenLength+1];
};


// These are all defined above so we don't need to repeat ourselves
// types of nodes that can appear in an input or output list
// enum datatype {
//  none, input, output, integer, floating, array, block, alias, string,
//  literal
// };

E 3
//**********************************************************************
D 3
// Defines the class interfaces for the Info_Server program.
E 3
I 3
// structure types for handling the token lists
// First, a basic link node structure
//**********************************************************************
class lnode {
private:
  lnode *lnodeNext;
  tokentype lnodeType;
  char *lnodeName;
  
public:
  // For getting and setting the type
  tokentype type(tokentype t) { return lnodeType = t; };
  tokentype type() { return lnodeType; };
  
  // Constructor
  lnode() 
  {
    DEBUG(cout << "LNODE constructor entered" << endl;);
    
    lnodeNext = 0;
    lnodeName = 0;
    lnodeType = t_invalid;
  }
  
  lnode(tokentype t, char *s)
  {
    DEBUG(cout << "LNODE Constructor/Args.  Token type " << t 
	  << " Name " << s << endl;
      );

    lnodeNext = 0;
    lnodeType = t;
    if (s)
    {
      int len = strlen(s) + 1;
      lnodeName = new char[len];
      strcpy(lnodeName,s);
    }
    else
      lnodeName = 0;
  };
  
  
  // virtual destructor
  virtual ~lnode() 
  { 
    DEBUG(cout << "LNODE destructor called" << endl
	  << "Name = " << name() << " Type = " << type() << endl;
      );
    
    if (lnodeName) delete [] lnodeName; 
  };
  
I 13
  virtual void MsgWrite(TipcMsg &m)
  {}
  
  virtual void MsgRead(TipcMsg &m, T_IPC_FT ft = T_IPC_FT_INVALID)
  {}
  

  virtual ostream &Print(ostream &out)
  {
    return out;
  }
E 13
  
  // Set the next pointer
  lnode *next(lnode *n) { return lnodeNext = n; };
  
  // get the next pointer
  lnode *next() { return lnodeNext; };
  
  // Get/Set the name
  char *name(char *n)
  {
    if (n)
    {
      int len = strlen(n) + 1;
      if (lnodeName) delete [] lnodeName;
      lnodeName = new char[len];
      strcpy(lnodeName,n);
    }
    else
      lnodeName = 0;
I 9
    DEBUG(cout << "lnode name set to ->" << lnodeName << "<-" << endl;);
E 9
    return lnodeName;
  };
  
  char *name() { return lnodeName; }; 
};

I 13
ostream &operator<<(ostream &out, lnode *x);
ostream &operator<<(ostream &out, lnode &x);

  
E 13

//**********************************************************************
// This is a simple linked list class for manipulating the above
// nodes.  
//**********************************************************************
class llist 
{
private:
  lnode *ltop;
  lnode *lbottom;
  lnode *lcurrent;
  
public:
  // Constructor
  llist() { ltop = lbottom = lcurrent = 0; };
  
  // Destructor.  *WARNING*, this destroys all the nodes
  // currently in the list
D 8
  ~llist() 
E 8
I 8
  virtual ~llist() 
E 8
  {
    lnode *t;
    
    top();
    
    while (t = remove())
    {
      delete t;
    }
  };
  
  

  // get the current node and set to the beginning of the list
  lnode *current() { return lcurrent; };
  void top() { lcurrent = ltop; };

  // Walk the list
  lnode *next() 
  {
    if (lcurrent)
    {
      return lcurrent = lcurrent->next();
    }
    return 0;
  };

  // Insert a node into the list at the end.
  void insert(lnode *n) 
  {
    if (ltop)			// check for empty list
    {
      lbottom->next(n);
      lcurrent = lbottom = n;
      n->next(0);
    }
    else
    {
      lcurrent = ltop = lbottom = n;
      n->next(0);
    }
  };
  
  // Remove the first node in the list and pop the stack
  lnode *remove()
  {
    lnode *n;
    
    n = ltop;
    if (n)
    {
      lcurrent = ltop = n->next();
      n->next(0);
    }
    
    return n;
  };
  
  // Search for a named node in the list
  lnode *find(char *n)
  {
    if (n)
    {
      lnode *t;
      int len = strlen(n);
      
      top();
      while (t = current())
      {
	next();
	if (!strncmp(t->name(),n,len))
	  return(t);
      }
    }
    return 0;
  }

  void clear()
  {
    lnode *n;
    
    top();
    while (n = remove())
      delete n;
  }

};


//**********************************************************************
// Next, we create specific subclasses to represent the different
// kinds of data
//**********************************************************************


//**********************************************************************
// an output component.  This is basically a name and a pointer
// to data in an input list.
//**********************************************************************
class out_comp : public lnode
{
public:
  lnode *odata;
  int ignoretime;

  out_comp() 
  {
    DEBUG(cout << "OUTCOMP constructor called" << endl;);
    ignoretime = 0;
    odata = 0;
  }
  
};


//**********************************************************************
// An input source or an output sink, base class.
//**********************************************************************
class ionode : public lnode {
private:
  char *datagroup;
  llist components;
I 4
  time_t i_timestamp;
E 4
  
public:
  // constructor
  ionode() 
  {
    DEBUG(cout << "IONODE Constructor called" << endl;);
    
    this->type(t_invalid);
    datagroup = 0;
I 4
    i_timestamp = 0;		// never been processed
E 4
  };
  ionode(char *n, tokentype t=t_invalid, char *d=0)
  {
    DEBUG(cout << "IONODE Constructor/Args called "
	  << "Name = " << n 
	  << "Type = " << t << " Datagroup = " << d << endl;
      );
    
    this->name(n);
    this->type(t);
I 4
    i_timestamp = 0;		// never been processed
E 4
    if (d)
D 9
      this->dgroup(d);
E 9
I 9
D 11
      this->dgroup("info_server/in/",d);
E 11
I 11
D 13
      this->dgroup((char *)"info_server/in/",d);
E 13
I 13
      this->dgroup("info_server/in/",d);
E 13
E 11
E 9
    else
D 9
      this->dgroup(n);
E 9
I 9
D 11
      this->dgroup("info_server/in/",n);
E 11
I 11
D 13
      this->dgroup((char *)"info_server/in/",n);
E 13
I 13
      this->dgroup("info_server/in/",n);
E 13
E 11
E 9
  }

  // destructor  (destruction of sublist happens automatically)
D 8
  ~ionode() 
E 8
I 8
  virtual ~ionode() 
E 8
  {
    DEBUG(cout << "IONODE Destructor called" << endl
	  << "Name = " << name() << " Type = " << type() << endl;
      );
    
    if (datagroup)
      delete [] datagroup;
  }
  
  // Return a pointer to the internal list
  llist &iolist()
  {
    return components;
  }

  // get/set the data group name
  char *dgroup() 
  {
    return datagroup;
  }
D 9
  char *dgroup(char *d)
E 9
I 9
D 11
  char *dgroup(char *d1,char *d2)
E 11
I 11
  char *dgroup(const char *d1,const char *d2)
E 11
E 9
  {
    DEBUG(cout << "IONODE Setting datagroup name for node " << name()
D 9
	  << endl << "Datagroup = " << d << endl;
E 9
I 9
	  << endl << "Datagroup = " << d1 << d2 << endl;
E 9
      );
    
    if (datagroup)
    {
      delete [] datagroup;
    }
D 9
    if (d)
E 9
I 9
    if (d2)
E 9
    {
D 9
      int len = strlen(d)+1;
E 9
I 9
      int len = strlen(d2)+strlen(d1) + 1;
E 9
      datagroup = new char[len];
D 9
      strcpy(datagroup,d);
E 9
I 9
      strcpy(datagroup,d1);
      strcat(datagroup,d2);
E 9
    }
    else
      datagroup=0;
    return datagroup;
  }
  
  // Insert a data component
  void insert(lnode *c)
  {
    DEBUG(cout << "IONODE Inserting node name " << c->name() << endl;);
    
    components.insert(c);
  }

  // wrappers for some list routines
  void top() { components.top(); }
  lnode *current() 
  {
    lnode *n = components.current();
    components.next();
    return n;
  }

  // Resets the internal list to a pristine condition.
  void clean()
  {
    lnode *n;
    
    DEBUG(cout << "IONODE - Cleaning up internal list for node"
	  << "name " << name() << endl;
      );
    
    components.top();
    while (n = components.remove())
      delete n;
  }
  
  // Search for a specified name
  lnode *find(char *s)
  {
    return components.find(s);
  }
  
I 4
  // functions for timestamping this object.
  time_t timestamp()
  {
    return i_timestamp;
  }
  time_t timestamp(time_t t)
  {
    return i_timestamp = t;
  }
E 4
};

//**********************************************************************
// Input data source  (currently just inherits)
//**********************************************************************
class input : public ionode
{
  llist parsedata;
D 4
  time_t i_timestamp;
E 4

I 13
  TipcMsg LastData;
  bool MessageSet;
  int lifetime;
  int lifemax;
  
E 13
public:
  
  input() 
  {
    DEBUG(cout << "INPUT Constructor called" << endl;);
D 13
    
E 13
I 13
    MessageSet = false;
E 13
    type(t_input);
I 13
    lifemax =  lifetime = 0;
E 13
  }

I 13
  bool messageset()
  {
    return MessageSet;
  }

  void resetlife()
  {
    lifetime = lifemax;
  }
  
  void setlifetime(int life)
  {
    lifemax = lifetime = life;
  }
  
  bool checklifetime()
  {
    bool retval = false;
    if (lifemax != 0)
    {
      --lifetime;
      retval = (lifetime == 0);
    }
    return retval;
  }
  
  void setmessage(TipcMsg &m)
  {
    MessageSet = true;
    LastData = m;
  }

  TipcMsg &getmessage()
  {
    return LastData;
  }
  
E 13
  llist &input_data()
  {
    return parsedata;
  }

  void data_top()
  {
    parsedata.top();
  }
  
  lnode *data_current()
  {
    return parsedata.current();
  }

  void data_clear()
  {
    parsedata.clear();
  }
  
  void data_insert(lnode *l)
  {
    parsedata.insert(l);
  }

  lnode *data_find(char *c)
  {
    return parsedata.find(c);
  }
  
D 4
  time_t timestamp()
  {
    return i_timestamp;
  }
  time_t timestamp(time_t t)
  {
    return i_timestamp = t;
  }
E 4
  
};

//**********************************************************************
// Output data source  (currently just inherits)
//**********************************************************************
class output : public ionode
{
D 6
  llist o_ignore;		// we don't care about time for these
E 6
I 6
  llist o_ignore;
  int o_timeout;
  int o_countdown;
E 6
  
public:
  output()
  {
    DEBUG(cout << "OUTPUT Constructor called" << endl;);
D 6
    
E 6
I 6

    o_timeout = o_countdown = 0;
E 6
    type(t_output);
  }

  void ignore_top()
  {
    o_ignore.top();
  }
  
  lnode *ignore()
  {
    lnode *n;
    n = o_ignore.current();
    o_ignore.next();
    return n;
  }
  
  lnode *ignore(lnode *n)
  {
    o_ignore.insert(n);
    return n;
  }
I 6

D 7
  void reset_timeout()
E 7
I 7
  void reset()
E 7
  {
    if (o_timeout)
      o_countdown = o_timeout;
  }
  
  void timeout(int thetime)
  {
    o_timeout = o_countdown = thetime;
  }
  
  int check_timeout()
  {
    return o_timeout && (!o_countdown);
  }
I 7

  int has_timeout()
  {
    return o_timeout;
  }
E 7
  
I 7
  void countdown()
  {
    if (o_countdown)
      o_countdown--;
  }
E 7
E 6
    
};

//**********************************************************************
// A single integer
//**********************************************************************
I 13
enum intsize {int2, int4, int8};

E 13
class integer : public lnode 
{
I 13
  intsize mysize;
  
E 13
public:
D 13
  long value;
E 13
I 13
  T_INT2 value2;
  T_INT4 value4;
  T_INT8 value8;
E 13
  integer() 
  { 
    DEBUG(cout << "INTEGER Constructor called" << endl;);
D 13
    
E 13
I 13
    mysize = int4;
E 13
    type(t_integer); 
  }
I 13

  void MsgWrite(TipcMsg &m)
  {
    switch (mysize)
    {
    case int2:
      m << value2 << Check;
      break;
    case int4:
      m << value4 << Check;
      break;
    case int8:
      m << value8 << Check;
    }
  }
E 13
  
I 13
  void MsgRead(TipcMsg &m, T_IPC_FT ft = T_IPC_FT_INVALID)
  {
    switch (ft)
    {
    case T_IPC_FT_INT2:
      mysize = int2;
      m >> value2 >> Check;
      break;
    case T_IPC_FT_INT4:
      mysize = int4;
      m >> value4 >> Check;
      break;
    case T_IPC_FT_INT8:
      mysize = int8;
      m >> value8 >> Check;
      break;
    }
  }

  ostream &Print(ostream &m)
  {
    switch (mysize)
    {
    case int2:
      m << value2;
      break;
    case int4:
      m << value4;
      break;
    case int8:
      m << value8;
    }
    return m;
  }
E 13
};

//**********************************************************************
// A single floating point number
//**********************************************************************
I 13
enum floatsize { flt4, flt8, flt16 };

E 13
class floating : public lnode
{
I 13
  floatsize mysize;

E 13
public:
D 13
  double value;
E 13
I 13
  T_REAL4 value4;
  T_REAL8 value8;
  T_REAL16 value16;

E 13
  floating() 
D 13
  { 
E 13
I 13
  {   
E 13
    DEBUG(cout << "FLOATING Constructor called" << endl;);
D 13
    
E 13
I 13
    mysize = flt8;
E 13
    type(t_floating); 
  }
I 13

  void MsgWrite(TipcMsg &m)
  {
    switch (mysize)
    {
    case flt4:
      m << value4 << Check;
      break;
    case flt8:
      m << value8 << Check;
      break;
    case flt16:
      m << value16 << Check;
    }
  }
  
  void MsgRead(TipcMsg &m, T_IPC_FT ft = T_IPC_FT_INVALID)
  {
    switch (ft)
    {
    case T_IPC_FT_REAL4:
      mysize = flt4;
      m >> value4 >> Check;
      break;
    case T_IPC_FT_REAL8:
      mysize = flt8;
      m >> value8 >> Check;
      break;
    case T_IPC_FT_REAL16:
      mysize = flt16;
      m >> value16 >> Check;
      break;
    }
  }

  ostream &Print(ostream &m)
  {
    switch (mysize)
    {
    case flt4:
      m << value4;
      break;
    case flt8:
      m << value8; 
      break;
    case flt16:
      m << "REAL16 Values cannot be printed"; // value16;
    }
    return m;
  }
  
E 13
};

//**********************************************************************
// A single character string
//**********************************************************************
class cstring : public lnode
{
private:
  char *the_str;
public:

  // Two constructors - one with a string argument.
  cstring()
  {
    DEBUG(cout << "CSTRING Constructor called" << endl;);
    
    type(t_string);
    the_str=0;
  }
  
  cstring(char *s)
  {
    DEBUG(cout << "CSTRING Constructor/Args called with" << endl
	  << "String = " << s << endl;
      );
    
    type(t_string);
    if (s)
    {
      int len = strlen(s)+1;
      the_str = new char[len];
      strcpy(the_str,s);
    }
    else
      the_str = 0;
  }

  // Destructor to make sure that we clean up the memory correctly.
D 8
  ~cstring()
E 8
I 8
  virtual ~cstring()
E 8
  {
    DEBUG(cout << "CSTRING Destructor called for object named" << endl
	  << "Name = " << name() << " Type = " << type() << "with" << endl
	  << "Value = " << the_str << endl;
      );
    
    if (the_str)
      delete [] the_str;
  }
  
  // Get/Set the character string
  char *str()
  {
    return the_str;
  }
  
  char *str(char *s)
  {
    if (s)
    {
      int len = strlen(s)+1;
      the_str = new char[len];
      strcpy(the_str,s);
    }
    else
      the_str = 0;
    return the_str;
  }
  
};


//**********************************************************************
// A literal string.  These can only appear in the output streams.
// They will be ignored in the input specifications.
//**********************************************************************
class literal : public cstring
{
public:
  // Two simple constructors
  literal(char *s)
  {
    DEBUG(cout << "LITERAL Constructor/Args called" << endl
	  << "String = " << s << endl;
      );
    
    type(t_literal);
    str(s);
  }
  literal()
  {
    DEBUG(cout << "LITERAL Constructor called" << endl;);
    
    type(t_literal);
  }

  // This class doesn't need a destructor, and it inherits everything
  // from cstring anyway.
};

//**********************************************************************
// An alias.  This is a strange class - it will go on the alias
// list only, not in the i/o spec lists.  When an output message
// is being built, the alias list will be checked first and the
// pointer to the correct element will be returned, along with the
// index.
//**********************************************************************
class alias : public lnode
{
private:
  lnode *a_obj;
  long a_low,a_high;		// section of an array
  char *a_objname;
public:
  
  // Constructor for this type never takes arguments
  alias() 
  {
    DEBUG(cout << "ALIAS Constructor called" << endl;);
    
    type(t_alias);
    
    a_obj=0;
    a_low=a_high=0;
    a_objname=0;
  }
  
  // Destructor to clean up the extra allocation
D 8
  ~alias()
E 8
I 8
  virtual ~alias()
E 8
  {
    DEBUG(cout << "Alias Destructor called for object" << endl
	  << "Name = " << name() << " Object = " << a_objname << endl;
      );
    
    if (a_objname)
      delete [] a_objname;
  }
  

  // Get/Set the object name
  char *objname()
  {
    return a_objname;
  }
  
  char *objname(char *n)
  {
    if (a_objname)
      delete [] a_objname;
    if (n) 
    {
      int len = strlen(n)+1;
      a_objname = new char[len];
      strcpy(a_objname,n);
    }
    else
      a_objname=0;
    return a_objname;
  }
  
  // Get/Set the object itself
  lnode *obj()
  {
    return a_obj;
  }
  lnode *obj(lnode *o)
  {
    return a_obj=o;
  }
  
  // Get/Set the lower limit
  long low()
  {
    return a_low;
  }
  long low(long l)
  {
    return a_low=l;
  }
  
  // Get/Set the upper limit
  long high()
  {
    return a_high;
  }
  long high(long h)
  {
    return a_high=h;
  }   
};


//**********************************************************************
// class representing an array.  This can contain several different
// types of array elements - integers, floating point or character
// strings.  We need extra members to determine which...
E 3
//
D 3
// Version: %A% %I% on %D% at %T%
E 3
I 3
// But first, we define two support types to make management a little
// easier
E 3
//
D 3
// Programmer: J.G. Hardie
E 3
I 3
// BEWARE::: First version supports only integer and floating point
// arrays to avoid the messy deep-copy problem
E 3
//**********************************************************************
I 3
enum arraytype { arr_invalid, arr_int, arr_float };
E 3

D 3
// Preliminary definitions.  Needed to support the main definitions.
E 3
I 3
union array_data 
{
  // char **strings;     // to be added later
D 13
  double *floats;
  long *ints;
E 13
I 13
  T_REAL4 *floats4;
  T_REAL8 *floats8;
  T_REAL16 *floats16;
  T_INT2 *ints2;
  T_INT4 *ints4;
  T_INT8 *ints8;
E 13
};
E 3

D 3
typedef enum { false, true} bool;         // Boolean data type
E 3
I 3
class array : public lnode
{
private:
  arraytype a_type;
  long a_length;
  array_data a_data;
I 13
  intsize myisize;
  floatsize myfsize;

  void cleanup()
  {
    if (a_data.floats4)
    {
      switch (a_type)
      {
      case arr_int:		        // integer array
      {
        switch (myisize)
        {
        case int2:
          delete [] a_data.ints2;
          break;
        case int4:
          delete [] a_data.ints4;
          break;
        case int8:
          delete [] a_data.ints8;
          break;
        }
	break;
      }
      case arr_float:		// floating point array
      {
        switch (myfsize)
        {
        case flt4:
          delete [] a_data.floats4;
          break;
        case flt8:
          delete [] a_data.floats8;
          break;
        case flt16:
          delete [] a_data.floats16;
          break;
        }
        break;
      }
      default:
	cout << "ERROR IN ARRAY - INVALID TYPE IN DESTRUCTOR" << endl;
      }
    }
    a_data.floats4 = 0;
  }
E 13
  
public:
  array()
  {
    DEBUG(cout << "ARRAY Constructor called" << endl;);
    
    type(t_array);
    a_type = arr_invalid;
    a_length = 0;
D 13
    a_data.floats = 0;
E 13
I 13
    a_data.floats4 = 0;
    myisize = int4;
    myfsize = flt8;
E 13
  }
E 3

D 3
#include "linked_list.h"
E 3

I 3
  // destructor is needed because we have to clean up any allocated
  // memory.  care must be taken to get the type right!
D 8
  ~array()
E 8
I 8
  virtual ~array()
E 8
  {
    DEBUG(cout << "ARRAY Destructor called for object" << endl
	  << "Name = " << name() << "of type = " << 
	  aptypestr(a_type) << "(" << a_type << ")" << endl;
      );
D 13
    
    if (a_data.floats)
    {
      switch (a_type)
      {
      case arr_int:		        // integer array
	delete [] a_data.ints;
	break;
      case arr_float:		// floating point array
	delete [] a_data.floats;
	break;
      default:
	cout << "ERROR IN ARRAY - INVALID TYPE IN DESTRUCTOR" << endl;
      }
    }
E 13
I 13
  
    cleanup();
E 13
  }
  
D 13
  char *aptypestr(arraytype a)
E 13
I 13
  const char *aptypestr(arraytype a)
E 13
  {
    switch (a)
    {
    case arr_invalid:
D 11
      return "Invalid";
E 11
I 11
D 13
      return (char *)"Invalid";
E 13
I 13
      return "Invalid";
E 13
E 11
      break;
    case arr_int:
D 11
      return "Integer";
E 11
I 11
D 13
      return (char *)"Integer";
E 11
      break;
E 13
I 13
    {
      switch (myisize)
      {
      case int2:
        return "Integer*2";
      case int4:
        return "Integer*4";
      case int8:
        return "Integer*8";
      }
    }
E 13
    case arr_float:
D 11
      return "Float";
E 11
I 11
D 13
      return (char *)"Float";
E 11
      break;
E 13
I 13
    {
      switch (myfsize)
      {
      case flt4:
        return "Float*4";
      case flt8:
        return "Float*8";
      case flt16:
        return "Float*16";
      }
    }
E 13
    default:
D 11
      return "Unknown";
E 11
I 11
D 13
      return (char *)"Unknown";
E 13
I 13
      return "Unknown";
E 13
E 11
    }
  }
  
D 10
  // **NOTE**NOTE**NOTE**NOTE**NOTE
  // THESE ROUTINES ARE NOT YET DEFINED.
  // **NOTE**NOTE**NOTE**NOTE**NOTE
E 3

E 10
D 3
#endif INFO_CLASSES_H_J
E 3
I 3
  // A set of routines for manipulating the data.
D 13
  double *ddata()
E 13
I 13
  // WARNING!  THESE ARE LEFT FOR COMPATIBILITY WITH EXISTING
  // CODE!  THEY WILL NOT WORK IF THE ARRAY SHOULD ACTUALLY
  // CONTAIN DATA OF DIFFERENT SIZES!
  T_REAL8 *ddata()
E 13
  {
D 13
    return a_data.floats;
E 13
I 13
    return a_data.floats8;
E 13
  };
D 13
  long *idata()
E 13
I 13

  T_INT4 *idata()
E 13
  {
D 13
    return a_data.ints;
E 13
I 13
    return a_data.ints4;
E 13
  };
D 13
  void data(double *d, long l)
E 13
I 13

  void data(T_REAL8 *d, long l)
E 13
  {
D 13
    if (a_data.floats)
      delete [] a_data.floats;
E 13
I 13
    cleanup();

    myfsize = flt8;
E 13

    a_length = l;
D 13
    a_data.floats = new double[l];
E 13
I 13
    a_data.floats8 = new T_REAL8[l];
E 13
    for (int i=0; i<l; i++)
D 13
      (a_data.floats)[i] = d[i];
E 13
I 13
      (a_data.floats8)[i] = d[i];
E 13
    
  };

  
D 13
  void data(long *d, long l)
E 13
I 13
  void data(T_INT4 *d, long l)
E 13
  {
D 13
    if (a_data.ints)
      delete [] a_data.ints;
E 13
I 13
    cleanup();

    myisize = int4;

E 13
    a_length = l;
D 13
    a_data.ints = new long[l];
E 13
I 13
    a_data.ints4 = new T_INT4[l];
E 13
    for (int i=0; i<l; i++)
D 13
      (a_data.ints)[i] = d[i];
E 13
I 13
      (a_data.ints4)[i] = d[i];
E 13
  };

  long length()
  {
    return a_length;
  }
  
  
  // Routines for setting/getting the array type
  arraytype arr_type()
  {
    return a_type;
  }

  arraytype arr_type(arraytype t)
  {
    DEBUG(cout << "ARRAY - setting data type to " << aptypestr(t) << endl;);
    
    return a_type=t;
  }
I 13

  void MsgWrite(TipcMsg &m)
  {
    if (a_type == arr_int)
    {
      switch (myisize)
      {
      case int2:
        m << SetSize(a_length) << a_data.ints2 << Check;
        break;
      case int4:
        m << SetSize(a_length) << a_data.ints4 << Check;
        break;
      case int8:
        m << SetSize(a_length) << a_data.ints8 << Check;
        break;
      }
    }
    else if (a_type == arr_float)
    {
      switch (myfsize)
      {
      case flt4:
        m << SetSize(a_length) <<  a_data.floats4 << Check;
        break;
      case flt8:
        m << SetSize(a_length) <<  a_data.floats8 << Check;
        break;
      case flt16:
        m << SetSize(a_length) <<  a_data.floats16 << Check;
        break;
      }
    }
  }

  void MsgRead(TipcMsg &m, T_IPC_FT ft = T_IPC_FT_INVALID)
  {
    cleanup();

    T_INT2 *i2;
    T_INT4 *i4;
    T_INT8 *i8;
    T_REAL4 *r4;
    T_REAL8 *r8;
    T_REAL16 *r16;
    int i;

    switch (ft)
    {
    case T_IPC_FT_INT2_ARRAY:
      m >> i2 >>  GetSize(&a_length) >> Check;
      myisize = int2;
      a_type = arr_int;
      a_data.ints2 = new T_INT2[a_length];
      for ( i=0; i<a_length; i++)
        a_data.ints2[i] = i2[i];
      break;
    case T_IPC_FT_INT4_ARRAY:
      m >> i4 >>  GetSize(&a_length) >> Check;
      myisize = int4;
      a_type = arr_int;
      a_data.ints4 = new T_INT4[a_length];
      for ( i=0; i<a_length; i++)
        a_data.ints4[i] = i4[i];
      break;
    case T_IPC_FT_INT8_ARRAY:
      m >> i8 >>  GetSize(&a_length) >> Check;
      myisize = int8;
      a_type = arr_int;
      a_data.ints8 = new T_INT8[a_length];
      for ( i=0; i<a_length; i++)
        a_data.ints8[i] = i8[i];
      break;
    case T_IPC_FT_REAL4_ARRAY:
      m >> r4 >>  GetSize(&a_length) >> Check;
      myfsize = flt4;
      a_type = arr_float;
      a_data.floats4 = new T_REAL4[a_length];
      for ( i=0; i<a_length; i++)
        a_data.floats4[i] = r4[i];
      break;
    case T_IPC_FT_REAL8_ARRAY:
      m >> r8 >>  GetSize(&a_length) >> Check;
      myfsize = flt8;
      a_type = arr_float;
      a_data.floats8 = new T_REAL8[a_length];
      for ( i=0; i<a_length; i++)
        a_data.floats8[i] = r8[i];
      break;
    case T_IPC_FT_REAL16_ARRAY:
      m >> r16 >>  GetSize(&a_length) >> Check;
      myfsize = flt16;
      a_type = arr_float;
      a_data.floats16 = new T_REAL16[a_length];
      for ( i=0; i<a_length; i++)
        a_data.floats16[i] = r16[i];
      break;
    }
  }

  ostream &Print(ostream &m)
  {
    for (int i=0; i<a_length; i++)
    {
      if (a_type == arr_int)
      {
        switch (myisize)
        {
        case int2:
          m << a_data.ints2[i];
          break;
        case int4:
          m  << a_data.ints4[i];
          break;
        case int8:
          m << a_data.ints8[i];
          break;
        }
      }
      else if (a_type == arr_float)
      {
        switch (myfsize)
        {
        case flt4:
          m <<  a_data.floats4[i];
          break;
        case flt8:
          m << a_data.floats8[i];
          break;
        case flt16:
//          m << a_data.floats16[i];
          if (i == 0)
            m << "REAL16 values cannot be printed";
          break;
        }
      }
      if ((i+1)%8 == 0)
        m << endl;
      else
        m << " ";
    }
    return m;
  }
  
E 13
I 8
};


//----------------------------------------------------------------------
// tagged array type
// This is identical to the standard array type except for the type
// field.  We simply inherit everything from the array.
//----------------------------------------------------------------------
class tagged : public array
{
I 9
  char *t_tagname;
  
E 9
public:
  tagged()
  {
    DEBUG(cout << "TAGGED Constructor called" << endl;);
D 9
    
E 9
I 9
    t_tagname = 0;
E 9
    type(t_tagged);
  }


  // destructor is needed because we have to clean up any allocated
  // memory.  care must be taken to get the type right!
  virtual ~tagged()
  {
    DEBUG(cout << "TAGGED Destructor called for object" << endl
	  << "Name = " << name() << "of type = " << 
	  aptypestr(arr_type()) << "(" << arr_type() << ")" << endl;
      );
  }
I 9

  char *tagname()
  {
    return t_tagname;
  }
  char *tagname(char *tn)
  {
    if (tn)
    {
      if (t_tagname)
	delete [] t_tagname;
      int len = strlen(tn) + 1;
      t_tagname = new char[len];
      strcpy(t_tagname,tn);
    }
    DEBUG(cout << "TAG name set to " << t_tagname << endl;);
    return t_tagname;
  }
  
      
E 9
  
E 8
};


//**********************************************************************
// Block class.
// This class represents a block of related data fragments.  Any
// number of copies of this block can appear in the input stream.
// This means that the block must contain a linked list of its
// elements and a count of the number of repeats, in addition to the
// usual name,type and link stuff.
//**********************************************************************
class block : public lnode
{
private:
  llist elements;
  llist data;
  long rep_count;
  
public:
  block() 
  {
    DEBUG(cout << "BLOCK Constructor called" << endl;);
    
    type(t_block);
    
    rep_count = 0;
  }
  
  // No explicit destructor requried - inheritance does it.

  // Return a reference to the internal list.  Warning, this
  // breaks encapsulation, but it makes traversing the list
  // during message construction/storage much simpler.
  llist &get_elements()
  {
    return elements;
  }

  // routines for manipulating the internal list
  void clean()			// delete everything
  {
    lnode *n;
    
    DEBUG(cout << "BLOCK - cleaning internal list for " << name() << endl;);
    
    elements.top();
    while (n = elements.remove())
      delete n;
  }

  // Reset list to the beginning to begin a traverse
  void top()
  {
    elements.top();
  }

  // Insert a data component
  void insert(lnode *c)
  {
    elements.insert(c);
  }

  // Traverse the list.
  lnode *current() 
  {
    lnode *n = elements.current();
    elements.next();
    return n;
  }
};


//**********************************************************************
// Exceptions that could be thrown in parsing
//**********************************************************************
class ParseExcept 
{ 
private:
D 11
  char *p_type; 
  char *p_msg;
E 11
I 11
  const char *p_type; 
  const char *p_msg;
E 11
  
public:
  ParseExcept()
  {
    DEBUG(cout << "PARSEEXCEPT Constructor entered - general fault" << endl;);
    
D 11
    p_type = "General Parse Exception";
    p_msg = "Parse failed";    
E 11
I 11
D 13
    p_type = (char *)"General Parse Exception";
    p_msg = (char *)"Parse failed";    
E 13
I 13
    p_type = "General Parse Exception";
    p_msg = "Parse failed";    
E 13
E 11
  };
  
D 11
  ParseExcept(char *t, char *m)
E 11
I 11
  ParseExcept(const char *t, const char *m)
E 11
  {
    DEBUG(cout << "PARSEEXCEPT Constructor/Args entered" << endl
	  << "Exception type: " << t << endl
	  << "Exception msg : " << m << endl;
      );
    
    if (t)
    {
      p_type = t;
    }
    if (m)
    {
      p_msg = m;
    }
  };
  
  virtual ~ParseExcept()
  {
    DEBUG(cout << "PARSEEXCEPT Destructor called" << endl
	  << "Exception type: " << p_type << endl
	  << "Exception msg : " << p_msg << endl;
      );
  }
  
D 11
  char *msg()
E 11
I 11
  const char *msg()
E 11
  {
    return p_msg;
  }
D 11
  char *type()
E 11
I 11
  const char *type()
E 11
  {
    return p_type;
  }
};



#endif INFO_CLASSES_HJ




E 3
E 1
