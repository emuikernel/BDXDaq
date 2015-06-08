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

// A define for debugging
#ifdef DEBUG_CODE
#define DEBUG(x) {x}
#else
#define DEBUG(x)
#endif


// Define the types of input tokens:  Also used for data types
enum tokentype { 
  t_invalid, t_input, t_output, t_integer, t_floating, t_array, t_block,
  t_alias, t_end, t_datagroup, t_name, t_literal, t_ignore, t_string,
  t_reference, t_lparen, t_rparen, t_quote, t_number, t_charname
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
    return lnodeName;
  };
  
  char *name() { return lnodeName; }; 
};


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
  ~llist() 
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
  
public:
  // constructor
  ionode() 
  {
    DEBUG(cout << "IONODE Constructor called" << endl;);
    
    this->type(t_invalid);
    datagroup = 0;
  };
  ionode(char *n, tokentype t=t_invalid, char *d=0)
  {
    DEBUG(cout << "IONODE Constructor/Args called "
	  << "Name = " << n 
	  << "Type = " << t << " Datagroup = " << d << endl;
      );
    
    this->name(n);
    this->type(t);
    if (d)
      this->dgroup(d);
    else
      this->dgroup(n);
  }

  // destructor  (destruction of sublist happens automatically)
  ~ionode() 
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
  char *dgroup(char *d)
  {
    DEBUG(cout << "IONODE Setting datagroup name for node " << name()
	  << endl << "Datagroup = " << d << endl;
      );
    
    if (datagroup)
    {
      delete [] datagroup;
    }
    if (d)
    {
      int len = strlen(d)+1;
      datagroup = new char[len];
      strcpy(datagroup,d);
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
  
};

//**********************************************************************
// Input data source  (currently just inherits)
//**********************************************************************
class input : public ionode
{
  llist parsedata;
  int timestamp;

public:
  
  input() 
  {
    DEBUG(cout << "INPUT Constructor called" << endl;);
    
    type(t_input);
  }

  void data_top()
  {
    parsedata.top();
  }
  
  lnode *data_current()
  {
    return parsedata.current();
  };
  
  void data_clear()
  {
    parsedata.clear();
  };
  
  
  void data_insert(lnode *l)
  {
    parsedata.insert(l);
  };
  

  lnode *data_find(char *c)
  {
    return parsedata.find(c);
  };
  
  
};

//**********************************************************************
// Output data source  (currently just inherits)
//**********************************************************************
class output : public ionode
{
  llist o_ignore;		// we don't care about time for these
  
public:
  output()
  {
    DEBUG(cout << "OUTPUT Constructor called" << endl;);
    
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
    
};

//**********************************************************************
// A single integer
//**********************************************************************
class integer : public lnode 
{
public:
  long value;
  integer() 
  { 
    DEBUG(cout << "INTEGER Constructor called" << endl;);
    
    type(t_integer); 
  }
  
};

//**********************************************************************
// A single floating point number
//**********************************************************************
class floating : public lnode
{
public:
  double value;
  floating() 
  { 
    DEBUG(cout << "FLOATING Constructor called" << endl;);
    
    type(t_floating); 
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
  ~cstring()
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
  ~alias()
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
enum arraytype {
  arr_invalid, arr_int, arr_float 
};

union array_data 
{
  // char **strings;     // to be added later
  double *floats;
  int *ints;
};

class array : public lnode
{
private:
  arraytype a_type;
  long a_length;
  array_data a_data;
  
public:
  array()
  {
    DEBUG(cout << "ARRAY Constructor called" << endl;);
    
    type(t_array);
    a_type = arr_invalid;
    a_length = 0;
    a_data.floats = 0;
  }


  // destructor is needed because we have to clean up any allocated
  // memory.  care must be taken to get the type right!
  ~array()
  {
    DEBUG(cout << "ARRAY Destructor called for object" << endl
	  << "Name = " << name() << "of type = " << a_type << endl;
      );
    
    if (a_data.floats)
    {
      switch (a_type)
      {
      arr_int:		        // integer array
	delete [] a_data.ints;
	break;
      arr_float:		// floating point array
	delete [] a_data.floats;
	break;
      default:
	cout << "ERROR IN ARRAY - INVALID TYPE IN DESTRUCTOR" << endl;
      }
    }
  }
  
  // **NOTE**NOTE**NOTE**NOTE**NOTE
  // THESE ROUTINES ARE NOT YET DEFINED.
  // **NOTE**NOTE**NOTE**NOTE**NOTE

  // A set of routines for manipulating the data.
  double *ddata()
  {
    return a_data.floats;
  };
  int *idata()
  {
    return a_data.ints;
  };
  void data(double *d, long l)
  {
    if (a_data.floats)
      delete [] a_data.floats;

    a_length = l;
    a_data.floats = new double[l];
    for (int i=0; i<l; i++)
      (a_data.floats)[i] = d[i];
    
  };

  void data(int *d, long l)
  {
    if (a_data.ints)
      delete [] a_data.ints;
    a_length = l;
    a_data.ints = new int[l];
    for (int i=0; i<l; i++)
      (a_data.ints)[i] = d[i];
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
    return a_type=t;
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
  char *p_type; 
  char *p_msg;
  
public:
  ParseExcept()
  {
    DEBUG(cout << "PARSEEXCEPT Constructor entered - general fault" << endl;);
    
    p_type = "General Parse Exception";
    p_msg = "Parse failed";    
  };
  
  ParseExcept(char *t, char *m)
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
  
  char *msg()
  {
    return p_msg;
  }
  char *type()
  {
    return p_type;
  }
};

