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

// Be sure that we have the smartsockets definitions.  Used to save the 
// internal state of the system.
#include "rtworks/cxxipc.hxx"


// A define for debugging
#ifndef DEBUG
#ifdef DEBUG_CODE
#define DEBUG(x) x
#else
#define DEBUG(x)
#endif
#endif

//typedef int bool;
//const int true=1;
//const int false=0;

const char * const printtype(int t);


// Define the types of input tokens:  Also used for data types
enum tokentype { 
  t_invalid, t_input, t_output, t_integer, t_floating, t_array, t_tagged, 
  t_block, t_alias, t_end, t_datagroup, t_name, t_literal, t_ignore, 
  t_timeout, t_string, t_reference, t_timestamp, t_lifetime, 
  // All literal tokens must be defined above this line.  The tokens
  // below are internal to the parser.
  t_lparen, t_rparen, t_quote, t_number, t_charname
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

//**********************************************************************
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
  
  virtual void MsgWrite(TipcMsg &m)
  {}
  
  virtual void MsgRead(TipcMsg &m, T_IPC_FT ft = T_IPC_FT_INVALID)
  {}
  

  virtual ostream &Print(ostream &out)
  {
    return out;
  }
  
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
    DEBUG(cout << "lnode name set to ->" << lnodeName << "<-" << endl;);
    return lnodeName;
  };
  
  char *name() { return lnodeName; }; 
};

ostream &operator<<(ostream &out, lnode *x);
ostream &operator<<(ostream &out, lnode &x);

  

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
  virtual ~llist() 
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
  time_t i_timestamp;
  
public:
  // constructor
  ionode() 
  {
    DEBUG(cout << "IONODE Constructor called" << endl;);
    
    this->type(t_invalid);
    datagroup = 0;
    i_timestamp = 0;		// never been processed
  };
  ionode(char *n, tokentype t=t_invalid, char *d=0)
  {
    DEBUG(cout << "IONODE Constructor/Args called "
	  << "Name = " << n 
	  << "Type = " << t << " Datagroup = " << d << endl;
      );
    
    this->name(n);
    this->type(t);
    i_timestamp = 0;		// never been processed
    if (d)
      this->dgroup("info_server/in/",d);
    else
      this->dgroup("info_server/in/",n);
  }

  // destructor  (destruction of sublist happens automatically)
  virtual ~ionode() 
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
  char *dgroup(const char *d1,const char *d2)
  {
    DEBUG(cout << "IONODE Setting datagroup name for node " << name()
	  << endl << "Datagroup = " << d1 << d2 << endl;
      );
    
    if (datagroup)
    {
      delete [] datagroup;
    }
    if (d2)
    {
      int len = strlen(d2)+strlen(d1) + 1;
      datagroup = new char[len];
      strcpy(datagroup,d1);
      strcat(datagroup,d2);
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
  
  // functions for timestamping this object.
  time_t timestamp()
  {
    return i_timestamp;
  }
  time_t timestamp(time_t t)
  {
    return i_timestamp = t;
  }
};

//**********************************************************************
// Input data source  (currently just inherits)
//**********************************************************************
class input : public ionode
{
  llist parsedata;

  TipcMsg LastData;
  bool MessageSet;
  int lifetime;
  int lifemax;
  
public:
  
  input() 
  {
    DEBUG(cout << "INPUT Constructor called" << endl;);
    MessageSet = false;
    type(t_input);
    lifemax =  lifetime = 0;
  }

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
  
  
};

//**********************************************************************
// Output data source  (currently just inherits)
//**********************************************************************
class output : public ionode
{
  llist o_ignore;
  int o_timeout;
  int o_countdown;
  
public:
  output()
  {
    DEBUG(cout << "OUTPUT Constructor called" << endl;);

    o_timeout = o_countdown = 0;
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

  void reset()
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

  int has_timeout()
  {
    return o_timeout;
  }
  
  void countdown()
  {
    if (o_countdown)
      o_countdown--;
  }
    
};

//**********************************************************************
// A single integer
//**********************************************************************
enum intsize {int2, int4, int8};

class integer : public lnode 
{
  intsize mysize;
  
public:
  T_INT2 value2;
  T_INT4 value4;
  T_INT8 value8;
  integer() 
  { 
    DEBUG(cout << "INTEGER Constructor called" << endl;);
    mysize = int4;
    type(t_integer); 
  }

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
};

//**********************************************************************
// A single floating point number
//**********************************************************************
enum floatsize { flt4, flt8, flt16 };

class floating : public lnode
{
  floatsize mysize;

public:
  T_REAL4 value4;
  T_REAL8 value8;
  T_REAL16 value16;

  floating() 
  {   
    DEBUG(cout << "FLOATING Constructor called" << endl;);
    mysize = flt8;
    type(t_floating); 
  }

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
  virtual ~cstring()
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
  virtual ~alias()
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
//
// But first, we define two support types to make management a little
// easier
//
// BEWARE::: First version supports only integer and floating point
// arrays to avoid the messy deep-copy problem
//**********************************************************************
enum arraytype { arr_invalid, arr_int, arr_float };

union array_data 
{
  // char **strings;     // to be added later
  T_REAL4 *floats4;
  T_REAL8 *floats8;
  T_REAL16 *floats16;
  T_INT2 *ints2;
  T_INT4 *ints4;
  T_INT8 *ints8;
};

class array : public lnode
{
private:
  arraytype a_type;
  long a_length;
  array_data a_data;
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
  
public:
  array()
  {
    DEBUG(cout << "ARRAY Constructor called" << endl;);
    
    type(t_array);
    a_type = arr_invalid;
    a_length = 0;
    a_data.floats4 = 0;
    myisize = int4;
    myfsize = flt8;
  }


  // destructor is needed because we have to clean up any allocated
  // memory.  care must be taken to get the type right!
  virtual ~array()
  {
    DEBUG(cout << "ARRAY Destructor called for object" << endl
	  << "Name = " << name() << "of type = " << 
	  aptypestr(a_type) << "(" << a_type << ")" << endl;
      );
  
    cleanup();
  }
  
  const char *aptypestr(arraytype a)
  {
    switch (a)
    {
    case arr_invalid:
      return "Invalid";
      break;
    case arr_int:
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
    case arr_float:
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
    default:
      return "Unknown";
    }
  }
  
  // A set of routines for manipulating the data.
  // WARNING!  THESE ARE LEFT FOR COMPATIBILITY WITH EXISTING
  // CODE!  THEY WILL NOT WORK IF THE ARRAY SHOULD ACTUALLY
  // CONTAIN DATA OF DIFFERENT SIZES!
  T_REAL8 *ddata()
  {
    return a_data.floats8;
  };

  T_INT4 *idata()
  {
    return a_data.ints4;
  };

  void data(T_REAL8 *d, long l)
  {
    cleanup();

    myfsize = flt8;

    a_length = l;
    a_data.floats8 = new T_REAL8[l];
    for (int i=0; i<l; i++)
      (a_data.floats8)[i] = d[i];
    
  };

  
  void data(T_INT4 *d, long l)
  {
    cleanup();

    myisize = int4;

    a_length = l;
    a_data.ints4 = new T_INT4[l];
    for (int i=0; i<l; i++)
      (a_data.ints4)[i] = d[i];
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
  
};


//----------------------------------------------------------------------
// tagged array type
// This is identical to the standard array type except for the type
// field.  We simply inherit everything from the array.
//----------------------------------------------------------------------
class tagged : public array
{
  char *t_tagname;
  
public:
  tagged()
  {
    DEBUG(cout << "TAGGED Constructor called" << endl;);
    t_tagname = 0;
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
  const char *p_type; 
  const char *p_msg;
  
public:
  ParseExcept()
  {
    DEBUG(cout << "PARSEEXCEPT Constructor entered - general fault" << endl;);
    
    p_type = "General Parse Exception";
    p_msg = "Parse failed";    
  };
  
  ParseExcept(const char *t, const char *m)
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
  
  const char *msg()
  {
    return p_msg;
  }
  const char *type()
  {
    return p_type;
  }
};



#endif INFO_CLASSES_HJ




