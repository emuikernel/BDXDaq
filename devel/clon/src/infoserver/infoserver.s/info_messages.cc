//**********************************************************************
// info_messages
// Routines which handle incoming and outgoing SmartSockets messages
//
// Version: @(#) info_messages.cc 1.19@(#) 1.19
//
// Programmer: J.G. Hardie
//**********************************************************************
#include <fstream.h>

#include "info_includes.hh"
#include "info_classes.hh"
#include "info_globals.hh"
#include "info_proto.hh"

void internal_process_info_serv(TipcMsg &msg);
void check_lifetimes();

T_BOOL FieldType(TipcMsg &m, T_IPC_FT *t);

void downcase(char *s)
{
  while (*s)
  {
    if (isalpha(*s))
      *s = tolower(*s);
    s++;
  }
}


//<<<<<<< info_messages.cc.C.0bqZX8
// Function to return a string representation of the smartsockets
// field type.
//DEBUG(
  // This function is only included if we are debugging
  const char *PrintFieldType(T_IPC_FT t);
  
//<<<<<<< info_messages.cc.C.0bqZX8
  const char *PrintFieldType(T_IPC_FT t)
{
  switch (t)
  {
  case T_IPC_FT_INVALID:
    return "Invalid";
  case T_IPC_FT_CHAR:
    return "Single Character";
  case T_IPC_FT_BINARY:
    return "Binary Data";
  case T_IPC_FT_STR:
    return "String";
  case T_IPC_FT_STR_ARRAY:
    return "String Array";
  case T_IPC_FT_INT2:
    return "Integer*2";
  case T_IPC_FT_INT2_ARRAY:
    return "Integer*2 Array";
  case T_IPC_FT_INT4:
    return "Integer*4";
  case T_IPC_FT_INT4_ARRAY:
    return "Integer*4 Array";
  case T_IPC_FT_INT8:
    return "Integer*8";
  case T_IPC_FT_INT8_ARRAY:
    return "Integer*8 Array";
  case T_IPC_FT_REAL4:
    return "Real*4";
  case T_IPC_FT_REAL4_ARRAY:
    return "Real*4 Array";
  case T_IPC_FT_REAL8:
    return "Real*8";
  case T_IPC_FT_REAL8_ARRAY:
    return "Real*8 Array";
  case T_IPC_FT_REAL16:
    return "Real*16";
  case T_IPC_FT_REAL16_ARRAY:
    return "Real*16 Array";
  case T_IPC_FT_MSG:
    return "Smartsockets Message";
  case T_IPC_FT_MSG_ARRAY:
    return "Smartsockets Message Array";
  default:
    return "Smartsockets Internal Type";
  }
}



//----------------------------------------------------------------------
// setup_ss_callbacks
// creates the callback functions for any special incoming data groups.
// or message types
//----------------------------------------------------------------------
void setup_ss_callbacks()
{
    return;
}


//**********************************************************************
// ipc_open_input
// Scans through the input list and makes sure that we are recieving
// any needed datagroups.  Note that, for the input list, by default
// we assume that the input datagroup is "info_server", but if a
// a datagroup is specifically named, we will open a line to that
// particular datagroup
//**********************************************************************
void ipc_open_input(llist &l)
{
  input *n;
  char *dg;
  T_BOOL rv;

  DEBUG(cout << "IpcOpenInput entered" << endl;);
  
  l.top();
  while (n = (input *)l.current())
  {
    l.next();

    dg = n->dgroup();
    DEBUG(cout << "IpcOpenInput: Attempting to get dg " << dg << endl;);
    
    if (!ss_serv.DgRecv(dg))
    {
      if (!ss_serv.DgRecv(dg,TRUE))
      {
	DEBUG(cout << "IpcOpenInput: Cannot connect to dg" << endl;);
	
	cerr << "IpcOpenInput - cannot connect to datagroup "
	     << dg << endl;
	exit(1);
      }
      DEBUG(cout << "IpcOpenInput: Connected to datagroup!" << endl;);
    }
    else
    {
      DEBUG(cout << "IpcOpenInput: Datagroup already subscribed" << endl;);
      cout << "IpcOpenInput: Multiple subscriptions " << dg << endl;
    }
  }
  DEBUG(cout << "IpcOpenInput - exiting" << endl;);
  
}


// Utility function for dealing with type fields.  Does not seem
// to be present in the C++ API
T_BOOL FieldType(TipcMsg &m, T_IPC_FT *t)
{
  T_IPC_MSG mm = m;
  
  return TipcMsgNextType(mm,t);
}


//**********************************************************************
// process_info_server
// this is the main callback for handling incoming info_server messages.
// All message processing and dispatch is done from here.  An info_server
// message is assumed to have a character string as its first element.
// The routine checks, and if this is not true, it prints an error and
// ignores the message.
//**********************************************************************
void process_info_serv(T_IPC_CONN conn, T_IPC_CONN_DEFAULT_CB_DATA data,
		       T_CB_ARG arg)
{
  TipcMsg msg(data->msg);
  internal_process_info_serv(msg);
}

void internal_process_info_serv(TipcMsg &msg)
{
  T_IPC_FT field_t;
  T_STR in_name, temp;
  input *in;
  
  DEBUG(cout << "ProcessInfoServ - entered" << endl;);
  
  // Reset the message so that we can look at the first field.
  msg.Current(0);
  if (!msg.Status())
  {
    cerr << "InfoServer: Cannot reset info_server message - ignored" << endl;
    return;
  }
  
  if (!FieldType(msg,&field_t))
  {
    cerr << "InfoServer: Could not get first field type" << endl;
    return;
  }

  DEBUG(cout << "ProcessInfoServ - got data type " 
	<< PrintFieldType(field_t) << endl;);
  
  if (field_t != T_IPC_FT_STR)
  {
    cerr << "InfoServer: info_server must start with T_IPC_FT_STR" << endl;
    return;
  }
  
  // Here we have a "valid" info_server message so we have to get the
  // first field and find the corresponding input structure.  (hope that
  // find() function works!
  msg >> in_name >> Check;
  if (!msg)
  {
    cout << "InfoServer: Could not find info_server msg name" << endl;
    return;
  }
  
  DEBUG(cout << "ProcessInfoServ - Got message type " << in_name << endl;);
  

  // downcase the string!
  temp = in_name;
  while (*temp)
  {
    if (isalpha(*temp))
      *temp = tolower(*temp);
    temp++;
  }

  if (log_any)
    cout << "InfoServer : found input message " << in_name << endl;
  
  // try to find the matching input node.
  if (!(in = (input *)inputlist.find(in_name)))
  {
    if (log_unknown)
      cout << "InfoServer: Input message without matching definition: " 
	   << in_name << endl;

    return;
  }

  DEBUG(cout << "ProcessInfoServ - Found the input - parsing..." << endl;);
  
  // So, looks like a valid message.  So we need to store the message 
  // structure itself in the input source structure.
  in->setmessage(msg);
  
  // Got it!  Process the data.
  if (process_input_msg(msg,in))
  {
    in->timestamp(last_input_time = timestamp());
    DEBUG(
      llist &thedat = in->input_data();
      printlist(thedat);
      );
    
    // Reset the lifetime for this input object.
    in->resetlife();

    // Now, the message is fully processed, we have to reconcile the
    // outputs with the new input and see if any messages have to be
    // built.
    DEBUG(cout << "ProcessInfoServ - Parse OK, check for outputs" << endl;);
    if (not_loading_state)
      reconcile_output();
  }
  else
  {
    if (log_bad)
      cout << "InfoServer: Message " << in_name
	   << " did not match definition." << endl;
    
    in->data_clear();
  }

  // All done - go back to sleep.
  DEBUG(cout << "ProcessInfoServ - exiting" << endl;);
  return;
}


//**********************************************************************
// process_input_msg
// Once we know that we have a valid info_server message, which matched
// the name of one of the defined input data streams, we have to split
// out the data in the message and store it in the input stream 
// object.  This is done by looping through the input.components list
// and making sure that 1) the data type in the message agrees with the
// data type in the object and 2) that there are the same number of
// components in the message as expected in the input stream.
// If this is true, we build the entire message and save the data.  If
// not, we simply zap the partially built data list and return.
// The followup routine "reconcile_output" will not build output in that
// case.  See the comment about handshaking in main()
//**********************************************************************
int process_input_msg(TipcMsg &msg, input *in)
{

  // This list is used for parsing
  llist &cl = in->iolist();
  lnode *n;

  // Data components used in the parsing.
  T_STR string_dat;
  T_INT4 int_dat;
  T_REAL8 float_dat;
  T_INT4 arr_size;
  T_INT4 *int_array;
  T_REAL8 *flt_array;
  
  // Field type.
  T_IPC_FT field_t;
  T_BOOL rval;

  DEBUG(cout << "ProcessInputMsg - entered ok" << endl;);
  
  // Used to check for parse errors.
  int good_input = 1;
  in->data_clear();
  
  // Ok, here we go.
  cl.top();
  in->data_top();
  
  while (n = cl.current())
  {
    cl.next();
  
    rval = FieldType(msg,&field_t);

    DEBUG(cout << "ProcessInputMsg - Looking at field type " 
	  << PrintFieldType(field_t) << endl << "    Field should be : "
	  << printtype(n->type()) << endl;);
    
    switch (n->type())
    {
    case t_integer:
    {  
      if ((field_t == T_IPC_FT_INT2) ||
          (field_t == T_IPC_FT_INT4) ||
          (field_t == T_IPC_FT_INT8))
      {
	integer *i = new integer;
	i->name(n->name());
	i->type(n->type());
//	msg >> int_dat >> Check;
        i->MsgRead(msg,field_t);
	if (!msg)
	{
	  if (log_bad && good_input)
	    cout << "ProcessInputMsg - problem with Integer data" << endl;
	  good_input = 0;
	  delete i;
	}
	else
	{
//	  i->value = int_dat;
	  in->data_insert(i);
	}
      }
      else
      {
	if (log_bad && good_input)
	  cout << "ProcessInputMsg - expecting an integer but didnt get it ("
	       << n->name() << ")"
	       << endl;
	good_input = 0;
      }
      break;
    }
    case t_floating:
    {
      if ((field_t == T_IPC_FT_REAL4) ||
          (field_t == T_IPC_FT_REAL8) ||
          (field_t == T_IPC_FT_REAL16))
      {
	floating *i = new floating;
	i->name(n->name());
	i->type(n->type());
//	msg >> float_dat >> Check;
        i->MsgRead(msg,field_t);

	if (!msg)
	{
	  if (log_bad && good_input)
	    cout << "ProcessInputMsg - problem with Floating data" << endl;
	  good_input = 0;
	  delete i;
	}
	else
	{
//	  i->value = float_dat;
	  in->data_insert(i);
	}
      }
      else
      {
	if (log_bad && good_input)
	  cout << "ProcessInputMsg - expecting a float but didnt get it ("
	       << n->name() << ")"
	       << endl;
	good_input = 0;
      }
      break;
    }
    case t_string:
    {
      if (field_t == T_IPC_FT_STR)
      {
	cstring *i = new cstring;
	i->name(n->name());
	i->type(n->type());
	msg >> string_dat >> Check;
	if (!msg)
	{
	  if (log_bad && good_input)
	    cout << "ProcessInputMsg - problem with String data" << endl;
	  good_input = 0;
	  delete i;
	}
	else
	{
	  i->str(string_dat);
	  in->data_insert(i);
	}
      }
      else
      {
	if (log_bad && good_input)
	  cout << "ProcessInputMsg - expecting an string but didnt get it ("
	       << n->name() << ")"
	       << endl;
	good_input = 0;
      }
      break;
    }
    case t_array:
    {
      if ((field_t == T_IPC_FT_INT2_ARRAY) || 
          (field_t == T_IPC_FT_INT4_ARRAY) || 
          (field_t == T_IPC_FT_INT8_ARRAY))
      {
	array *i = new array;
	i->name(n->name());
	i->type(n->type());
	i->arr_type(arr_int);
//	msg >> int_array >> GetSize(&arr_size) >> Check;
// 	DEBUG(cout << "ProcessInputMsg: got integer array.  Size "
// 	      << arr_size << endl;);
// 	DEBUG(if (arr_size > 0)
// 	      for (int aiz=0; aiz < arr_size; aiz++)
// 	      cout << "Array[" << aiz << "] = " << int_array[aiz] << endl;);

	i->MsgRead(msg,field_t);
	if (!msg)
	{
	  if (log_bad && good_input)
	    cout << "ProcessInputMsg - problem with IntArray data" << endl;
	  good_input = 0;
	  delete i;
	}
	else
	{
//	  i->data(int_array,arr_size);
	  in->data_insert(i);
	}
      }
      else if ((field_t == T_IPC_FT_REAL4_ARRAY) ||
               (field_t == T_IPC_FT_REAL8_ARRAY) ||
               (field_t == T_IPC_FT_REAL16_ARRAY))
      {
	array *i = new array;
	i->name(n->name());
	i->type(n->type());
	i->arr_type(arr_float);
        i->MsgRead(msg,field_t);
// 	msg >> flt_array >> GetSize(&arr_size) >> Check;
// 	DEBUG(cout << "ProcessInputMsg: got real array.  Size "
// 	      << arr_size << endl;);
// 	DEBUG(if (arr_size > 0)
// 	      for (int aiz=0; aiz < arr_size; aiz++)
// 	      cout << "Array[" << aiz << "] = " << flt_array[aiz] << endl;);
	if (!msg)
	{
	  if (log_bad && good_input)
	    cout << "ProcessInputMsg - problem with FltArray data" << endl;
	  good_input = 0;
	  delete i;
	}
	else
	{
//	  i->data(flt_array,arr_size);
	  in->data_insert(i);
	}
      }
      else
      {
	if (log_bad && good_input)
	  cout << "ProcessInputMsg - expecting an array but didnt get it ("
	       << n->name() << ")"
	       << endl;
	good_input = 0;
      }
      break;
    }
    case t_tagged:
    {
      // To handle the tagged input type, we remember the current
      // position in the message and scan forward looking for a
      // string field which matches the specified tag.  If we
      // don't find it we just return with good_input set to true.
      // If we do find it, we store the assoiciated array into the
      // input data list and return.
      // 
      // In both cases, we reset the message pointer to the original
      // position.
      // Note: Tagged fields are not very efficient.  This method of
      // extraction requires M*N searches where M is the number of
      // tagged fields and N is the number of actual fields in the
      // incoming message.  Oh well.
      T_INT4 Counter;
      T_INT4 CurrentPos = msg.Current();
      T_INT4 NumFields = msg.NumFields();
      T_IPC_FT ftype;
      T_STR tagname;

      DEBUG(cout << "Entered processing for TAGGED field" << endl;)
      for (Counter = CurrentPos; Counter < NumFields; Counter++)
      {
	msg.Current(Counter);
	FieldType(msg,&ftype);
	if (ftype == T_IPC_FT_STR)
	{
	  msg >> tagname >> Check;
	  downcase(tagname);
	  DEBUG(cout << "Got an ascii string - the message has : "
		<< tagname << endl << "Looking for : "
		<< ((tagged *)n)->tagname() << endl;);
	  if (!strncmp(tagname,((tagged *)n)->tagname(),
		       strlen(((tagged *)n)->tagname())))
	  {
	    DEBUG(cout << "Got it!" << endl;);
	    // match.  Save the sucker
	    Counter = NumFields;       //force exit
	    FieldType(msg,&ftype);
	    if ((ftype == T_IPC_FT_INT2_ARRAY) ||
                (ftype == T_IPC_FT_INT4_ARRAY) ||
                (ftype == T_IPC_FT_INT8_ARRAY))
	    {
	      array *i = new array;
	      i->name(n->name());
	      i->type(n->type());
	      i->arr_type(arr_int);
              i->MsgRead(msg,ftype);
// 	      msg >> int_array >> GetSize(&arr_size) >> Check;
// 	      DEBUG(cout << "ProcessInputMsg: got integer array.  Size "
// 		    << arr_size << endl;);
// 	      DEBUG(if (arr_size > 0)
// 		    for (int aiz=0; aiz < arr_size; aiz++)
// 		    cout << "Array[" << aiz << "] = " << int_array[aiz] 
// 		    << endl;);
	      
	      if (!msg)
	      {
		if (log_bad && good_input)
		  cout << "ProcessInputMsg - problem with IntArray data" 
		       << endl;
		good_input = 0;
		delete i;
	      }
	      else
	      {
//		i->data(int_array,arr_size);
		in->data_insert(i);
	      }
	    }
	    else if ((ftype == T_IPC_FT_REAL4_ARRAY) ||
                     (ftype == T_IPC_FT_REAL8_ARRAY) ||
                     (ftype == T_IPC_FT_REAL16_ARRAY))
	    {
	      array *i = new array;
	      i->name(n->name());
	      i->type(n->type());
	      i->arr_type(arr_float);
              i->MsgRead(msg,ftype);

// 	      msg >> flt_array >> GetSize(&arr_size) >> Check;
// 	      DEBUG(cout << "ProcessInputMsg: got real array.  Size "
// 		    << arr_size << endl;);
// 	      DEBUG(if (arr_size > 0)
// 		    for (int aiz=0; aiz < arr_size; aiz++)
// 		    cout << "Array[" << aiz << "] = " << flt_array[aiz] 
// 		    << endl;);
	      if (!msg)
	      {
		if (log_bad && good_input)
		  cout << "ProcessInputMsg - problem with FltArray data" 
		       << endl;
		good_input = 0;
		delete i;
	      }
	      else
	      {
//		i->data(flt_array,arr_size);
		in->data_insert(i);
	      }
	    }
	    else
	    {
	      if (log_bad && good_input)
		cout << "ProcessInputMsg - expecting an array but didnt get it ("
		     << n->name() << ")"
		     << endl;
	      good_input = 0;
	    }
	  }
	}
      }
      msg.Current(CurrentPos);
      break;
    }
    case t_block:
    {
      if (log_bad && good_input)
      {
	cout << "Got a block, but ignoring it." << endl;
	cout << "BLOCK COMMAND IS NOT YET IMPLEMENTED!!!" << endl;
      }
      good_input = 0;
      break;
    }
    }
  }
  return good_input;
}

//**********************************************************************
// reconcile_output
// Function gets triggered every time an input message is received.  It
// scans through the output message definitions looking for one which
// needs to be updated.  If it finds a message which must be updated,
// it will build that message and send it off to the appropriate 
// datagroup.
//**********************************************************************
void reconcile_output()
{
  DEBUG(cout << "ReconcileOutput - entered" << endl;);

  // Check to see if anything has to be done.
  if (last_input_time < last_output_time)
  {
    DEBUG(cout << "ReconcileOutput - nothing needs to be done" << endl;);
    return;
  }
  

  output *o;			// pointer to individual output channel
  input *i;			// pointer to individual input channel
  out_comp *oc;
  lnode *onode,*inode;
  tokentype ttype;
  time_t o_time,i_time;		// timestamps for in and out nodes.
  time_t newest;		// newest input dependency so far.
  time_t only_ignore;
  

  // Algorithm...
  // For each output channel, save the timestamp.  Then for each
  // component in that output channel, search the input list for it and
  // record the last time that element was updated.
  // Save the last updated time for all the output components.  If this
  // time is later than the last time that the output component was
  // built, generate an output message.

  // One of the lists o->components or i->components will need to
  // be scanned multiple times.  This is unavoidable, but it is a toss up
  // as to which one *should* be looped over in the innermost loop.  I
  // choose to loop over the input list in the inner level.
  // Note: the number of loops here gets big in a hurry.  Suppose that
  // we have 4 output nodes, each of which contains 5 non-atomic (i.e.
  // not a 'name' or 'literal') components.  Further suppose that we have
  // 5 input sources, with three components each.  Then, this output search
  // will require 4*5*5*3 = 300 iterations.  Each iteration involves
  // a string comparison (at minimum), so this can take a while if the number
  // of sources or the size of the messages gets even modestly large.
  // Time estimate:  Assume that a character comparison takes roughly 0.1
  // microsecond on a 150 MHz Sparc.  Also assume that the character names
  // are 8 characters on average.  Then the worst case execution time
  // of this routine would be:
  // 300 x 8 x 0.1 us = 240 us
  // BUT, if we had 20 inputs/outputs with 25 components each, the routine
  // would take:
  // 20x20x25x25x8x0.1 = 200,000 us = 0.2 seconds *PER INPUT EVENT*!!!!
  //
  // Watch out for this - it is clear that the info_server is not a time
  // critical monitor!
  outputlist.top();
  DEBUG(cout << "ReconcileOutput - checking for construction" << endl;);
  
  while (o = (output *)outputlist.current())
  {
    int safe_to_build,hits;
    
    // start the list.
    outputlist.next();
    o_time = o->timestamp();
    newest = 0;
    o->top();

    DEBUG(cout << "ReconcileOutput - checking onode " << o->name()
	  << endl;);
    
    safe_to_build = 1;
    only_ignore = 1;
    
    // Check each component in the output stream.
    while (onode = o->current())
    {
      hits = 0;

      // Note: we ignore the timestamp for this search...
      ttype = onode->type();
      if (ttype != t_reference)	// doesn't refer to inputs
	continue;
      
      oc = (out_comp *)onode;

      // <sigh> we have to loop through the input here.
      inputlist.top();
      while (i = (input *)inputlist.current())
      {
	inputlist.next();

	// we check the *DATA* list not the parse list.  We don't care
	// if the input list could contain the output component, we want
	// to know if the output component has been *seen* recently.
	i->data_top();
	if (i->data_find(oc->name()))
	{
	  hits = 1;
	  if (!oc->ignoretime)	// if we can ignore time, don't check.
	  {
	    DEBUG(cout << "Time Counts" << endl;);
	    only_ignore = 0;
	    i_time = i->timestamp();
	    if (!newest)
	      newest = i_time;
	    if (i_time < newest)
	      newest = i_time;	// keep the earliest time around.
	  }
	  else
	  {
	    DEBUG(cout << "Time ignored" << endl;);
	  }
	  
	}
      }
      DEBUG(if (!hits) 
	    {
	      cout << "ReconcileOutput - comp " << onode->name()
		   << " was not found" << endl;
	    }
	    else
	    {
	      cout << "ReconcileOutput - comp " << onode->name()
		   << " was found" << endl;
	    });
      
      safe_to_build = safe_to_build * hits;
    }
    
    // Well, we finished searching for all the relevant components
    // of the output stream.  Now check to see if the stream needs
    // to be rebuilt.  If so, generate the output message.
    if ((safe_to_build) && ((newest > o_time) || only_ignore))
    {
      DEBUG(cout << "ReconcileOutput - node needs to be built" << endl;);
      construct_output(o);
      o->timestamp(last_output_time = timestamp());
    }
    else
    {
      DEBUG(cout << "ReconcileOutput - node does not need building" << endl;);
    }
    
  }

  // my job here is finished.
  DEBUG(cout << "ReconcileOutput - exiting" << endl;);
    
}

//**********************************************************************
// construct_output
// This function builds the requisite output message and sends it
// to the appropriate datagroup.
// Algorithm:
//  1) reset the top of the components list.
//  2) Allocate a TipcMsg object and initialize it.
//  3) Set the msg.Dest to the output object datagroup
//  4) Sender is the infoserver
//  5) For each component in the output object, add the correct data
//     to the message.  If the component is atomic, just insert the
//     data directly into the message, otherwise search for it in
//     the input list.
//  6) When we're done, send the stupid message already.
//
// *WARNING* this routine is not safe unless the input list is available
// for a scan.  If the input list is being traversed by some other
// routine, this routine will cause the list traversal to fail in
// strange and mysterious ways.  Caveat Emptor.
//**********************************************************************
void construct_output(output *out)
{
  lnode *onode,*inode;
  TipcMsg msg(CAST "info_server");
  long i,j,datalength;
  long *idata;
  double *fdata;

  DEBUG(cout << "ConstructOutput - entered" << endl;);
  
  out->top();

  msg.Dest(out->dgroup());
  msg.Sender(CAST "info_server");
  msg.Current(0);
  
  // Ok, loop through the components and build the message.  
  while (onode = out->current())
  {
    switch (onode->type())
    {
    case t_name:		// append output stream name
      DEBUG(cout << "ConstructOutput - appending output name "
	    << out->name() << endl;);
      
      msg << out->name() << Check;
      break;
    case t_literal:		// write a fixed string.
    {
      literal *l = (literal *)onode;
      DEBUG(cout << "ConstructOutput - appending a literal "
	    << l->str() << endl;);
      
      msg << l->str() << Check;
      break;
    }
    case t_timestamp:		// put the time stamp in the output
    {
      out_comp *oc = (out_comp *)onode;
      input *icomp;

      DEBUG(cout << "ConstructOutput - appending a timestamp for "
	    << oc->name() << endl;);

      inputlist.top();
      while (icomp = (input *)inputlist.current())
      {
	if (!strcmp(oc->name(),icomp->name()))
	{
	  msg << icomp->timestamp() << Check;
	  break;
	}
	inputlist.next();
      }
      break;
    }
    case t_reference:		// refers to the input somewhere.
    {
      out_comp *oc = (out_comp *)onode;
      input *icomp;

      DEBUG(cout << "ConstructOutput - appending a reference to " 
	    << oc->name() << endl;);
      
      inputlist.top();
      while (icomp = (input *)inputlist.current())
      {
	inputlist.next();
	icomp->data_top();
	DEBUG(cout << "ConstructOutput - checking input"
	      << icomp->name() << endl;);
	
	if (inode = icomp->data_find(oc->name()))
	{
	  DEBUG(cout << "ConstructOutput - found the object.  It is"
		<< " type ";);
	  
	  switch (inode->type())
	  {
	  case t_integer:	// output a single integer
	  {
	    integer *n = (integer *)inode;
//	    DEBUG(cout << "integer, value = " << n->value << endl;);
//	    msg << n->value << Check;
            n->MsgWrite(msg);
	    break;
	  }
	  case t_floating:	// a single floating point number.
	  {
	    floating *n = (floating *)inode;
//	    DEBUG(cout << "floating, value = " << n->value << endl;);
//	    msg << n->value << Check;
            n->MsgWrite(msg);
	    break;
	  }
	  case t_string:	// a character string
	  {
	    cstring *n = (cstring *)inode;
	    DEBUG(cout << "string, value = " << n->str() << endl;);
	    msg << n->str() << Check;
	    break;
	  }
	  case t_tagged:
	    msg << inode->name() << Check;
	    
	  case t_array:		// an array of numbers.
	  {
	    array *n = (array *)inode;

	    datalength = n->length();
	    DEBUG(cout << "array, length = " << datalength 
		  << " elem_type ";);

	    n->MsgWrite(msg);

// 	    if (n->arr_type() == arr_int)
// 	    {
// 	      idata = n->idata();
// 	      DEBUG(cout << "integer" << endl
// 		    << "Contents:" << endl;
// 		    for (int dbind=0; dbind < datalength; dbind++)
// 		    {
// 		      cout << setw(10) << idata[dbind];
// 		      if (!((dbind+1)%8))
// 			cout << endl;
// 		    }
// 		    if ((datalength%8))
// 		    cout << endl;);
// 	      msg << SetSize(datalength) << idata << Check;
// 	    }
// 	    else if (n->arr_type() == arr_float)
// 	    {
// 	      fdata = n->ddata();
// 	      DEBUG(cout << "float" << endl
// 		    << "Contents:" << endl;
// 		    for (int dbind=0; dbind < datalength; dbind++)
// 		    {
// 		      cout << setw(10) << fdata[dbind];
// 		      if (!((dbind+1)%8))
// 			cout << endl;
// 		    }
// 		    if ((datalength%8))
// 		    cout << endl;
// 		);
// 	      msg << SetSize(datalength) << fdata << Check;
// 	    }
	    break;		// breaks the inner switch.
	  }
	  }
	}
	else if (inode = icomp->find(oc->name()))
	{
	  DEBUG(cout << "ConstructOutput - found the parse object.  It is"
		<< " type ";);
	  
	  switch (inode->type())
	  {
	  case t_integer:	// output a single integer
	  {
	    DEBUG(cout << "integer, value = " << 0 << endl;);
	    T_INT4 val = 0;
	    msg << val << Check;
	    break;
	  }
	  case t_floating:	// a single floating point number.
	  {
	    T_REAL8 val = 0.0;
	    DEBUG(cout << "floating, value = " << 0.0 << endl;);
	    msg << val << Check;
	    break;
	  }
	  case t_string:	// a character string
	  {
	    DEBUG(cout << "string, value = " << " " << endl;);
	    msg << CAST " " << Check;
	    break;
	  }
	  case t_tagged:
	    msg << inode->name() << Check;
	    
	  case t_array:		// an array of numbers.
	  {
	    array *n = (array *)inode;

	    datalength = n->length();
	    DEBUG(cout << "array, length = " << datalength 
		  << " elem_type ";);
	    
	    if (n->arr_type() == arr_int)
	    {
	      T_INT4 arr[1];
	      arr[0] = 0;
	      DEBUG(cout << "integer" << endl);
	      msg << SetSize(1) << (T_INT4 *)arr << Check;
	    }
	    else if (n->arr_type() == arr_float)
	    {
	      DEBUG(cout << "float" << endl);
	      T_REAL8 arr[1];
	      arr[0] = 0.0;

	      msg << SetSize(1) << (T_REAL8 *)arr << Check;
	    }
	    break;		// breaks the inner switch.
	  }
	  }
	}
//	break;		// breaks the while input
      }
      
      break;			// breaks the outer switch.
    }
    if (!msg)
    {
      cerr << "ConstructOutput - problem adding to message" << endl;
      DEBUG(cout << "ConstructObject - message build aborted" << endl;);
      return;
    }
    }
  }

  // Ok, we have a constructed message to send.
  if (!ss_serv.Send(msg,TRUE))
  {
    cerr << "ConstructOutput - cannot send message" << endl;
  }
  
  if (!ss_serv.Flush())
  {
    cerr << "ConstructOutput - cannot flush message queue" << endl;
  }
  
}

    
    
//----------------------------------------------------------------------
// handle_timers()
// This routine gets called every time there is a timeout.  It scans
// the output list looking for output events which have timers on them.
// If the output list has an enabled timer, the value of this timer is
// checked.  If the timer triggers, then the output object is built
// and the message sent.  Then the timer is set back to the original
// time.
//
// The variable dumptimer is used to keep track of the interval between
// complete data dumps.
//
// The variable statetimer is used to keep track of the interval between
// internal state dumps.  The constant stateinterval sets the interval
// to 10 minutes.
//----------------------------------------------------------------------
int dumptimer = 0;
int statetimer = 0;

void dump_inputs_now();

void handle_timers()
{
  outputlist.top();
  output *o;

  if (dump_flag)
  {
    dumptimer++;
    if (dumptimer == dump_time)
    {
      dumptimer = 0;
      dump_inputs_now();
    }
  }
  
  // If stateinterval is identically zero, then we don't bother
  // with the state saving
  if (stateinterval)
  {
    statetimer++;
    if (statetimer == stateinterval)
    {
      statetimer = 0;
      save_state_now(state_file);
    }
  }
  
  

  while (o = (output *)outputlist.current())
  {
    outputlist.next();
    
    if (o->has_timeout())
    {
      o->countdown();
      if (o->check_timeout())
      {
	construct_output(o);
	o->reset();
      }
    }
  }

  inputlist.top();
  input *in;
  while (in = (input *)inputlist.current())
  {
    if (in->checklifetime())
      in->data_clear();
    
    inputlist.next();
  }
  
}

      
//----------------------------------------------------------------------
// dump_inputs_now()
// This function writes the complete contents of the input list
// to the specified dump_file.
//----------------------------------------------------------------------
void dump_inputs_now()
{
  ofstream out(dump_file);
  if (!out)
  {
    cerr << "Error!  Unable to open dump file " << dump_file << endl;
    return;
  }

  dumpthelist(inputlist,out);
  out.close();
}


//----------------------------------------------------------------------
// save_state_now()
// write the archived info_server input messages to a smartsockets
// message file so that the internal state can be restored.
//----------------------------------------------------------------------
void save_state_now(const char *filename)
{
  TipcMsgFile msgfile(const_cast<char *>(filename),
		      T_IPC_MSG_FILE_CREATE_WRITE);
  if (!msgfile)
  {
    cerr << "save_state_now: error trying to open output message"
	 << " file " << filename << endl;
    cerr << "Info_Server state not saved" << endl;
    return;
  }
  
  inputlist.top();
  input *inp;
  while (inp = (input *)inputlist.current())
  {
    inputlist.next();
    if (inp->messageset())
      msgfile << inp->getmessage();
  }
}


//----------------------------------------------------------------------
// process_info_state()
// Reads all the input messages from the specified smartsockets data
// file and processes each message in turn.
//----------------------------------------------------------------------
void process_info_state(const char *filename)
{
  TipcMsgFile msgfile(const_cast<char *>(filename),
		      T_IPC_MSG_FILE_CREATE_READ);
  if (!msgfile)
  {
    cerr << "process_info_state: error trying to open output message"
	 << " file " << filename << endl;
    cerr << "Info_Server state not loaded" << endl;
    return;
  }
  
  // turn off output processing
  not_loading_state = 0;
  
  TipcMsg msg;
  bool notdone = true;
  msgfile >> msg;
  if (!msgfile)
    notdone = false;
  while (notdone)
  {
    internal_process_info_serv(msg);
    msgfile >> msg;
    if (!msgfile)
      notdone = false;
  }
  
  not_loading_state = 1;
}

