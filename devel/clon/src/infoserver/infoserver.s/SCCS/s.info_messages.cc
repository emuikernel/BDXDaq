h19735
s 00005/00004/01200
d D 1.19 01/01/19 15:47:42 jhardie 21 20
c fixed version problems and const B.S.
e
s 00002/00001/01202
d D 1.18 01/01/19 15:30:35 jhardie 20 19
c fix const problem.
c 
e
s 00001/00001/01202
d D 1.17 01/01/19 15:27:48 jhardie 19 18
c fixed const problem with save_state_now()
c 
e
s 00001/00001/01202
d D 1.16 01/01/19 15:16:30 jhardie 18 17
c fixed missing paren on DEBUG macro (from merge)
c 
e
s 00006/00002/01197
d D 1.15 01/01/19 15:07:40 jhardie 17 15
i 16
c Merged changes between child workspace "/home/jhardie/scalers/info_server" and
c  parent workspace "/usr/local/clas/devel/source".
c 
e
s 00350/00149/00849
d D 1.12.1.1 01/01/19 14:18:59 jhardie 16 13
c current version (but not 5.6 ready)
e
s 00002/00002/00996
d D 1.14 01/01/09 17:09:56 jhardie 15 14
c Fix ANOTHER const problem
c 
e
s 00003/00003/00995
d D 1.13 01/01/09 16:20:40 jhardie 14 13
c Fixed const problems
c 
e
s 00037/00008/00961
d D 1.12 99/01/06 19:21:27 jhardie 13 12
c Modified for verions of infoserver which
c mangle names, new ss groups, and timestamping.
e
s 00013/00000/00956
d D 1.11 98/06/03 14:29:41 jhardie 12 11
c ???
c 
e
s 00003/00001/00953
d D 1.10 98/02/14 16:41:05 jhardie 11 10
c added name to missed message type.
c 
e
s 00001/00000/00953
d D 1.9 98/02/05 16:46:05 jhardie 10 9
c Fixed timestamp bug in output construction routines
c 
e
s 00110/00001/00843
d D 1.8 98/01/27 13:31:32 jhardie 9 8
c Added code for tagged arrays
e
s 00001/00001/00843
d D 1.7 98/01/22 17:41:26 jhardie 8 7
c Fixed bugs in timeout function.
c 
e
s 00004/00002/00840
d D 1.6 98/01/22 15:10:05 jhardie 7 6
c fixed reference problem with overload ambiguity for SS message output.
c Wants an actual integer variable (or reference thereto) rather than a
c literal constant.
c 
e
s 00098/00009/00744
d D 1.5 98/01/22 14:47:39 jhardie 6 5
c Added code to handle timeouts.  This includes the timeout loop, the
c extra code in output to deal with missing inputs (they will be sent as
c zeros) and the code to send a message even if all fields are supposed
c to be ignored.
c 
e
s 00051/00003/00702
d D 1.4 97/06/05 11:58:14 jhardie 5 4
c fixed some small problems with smartsockets arrays
e
s 00287/00015/00418
d D 1.3 97/06/05 10:40:39 jhardie 4 3
c Version 1.0 of info_server and support programs
e
s 00410/00001/00023
d D 1.2 97/06/02 18:37:16 jhardie 3 1
c Rewrite parser for full C++.  Info messages input ok,
c output still broken
e
s 00000/00000/00000
d R 1.2 97/03/25 09:45:01 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 3 2 info_server/server/info_messages.cc
c Name history : 2 1 server/info_messages.cc
c Name history : 1 0 s/server/info_messages.cc
e
s 00024/00000/00000
d D 1.1 97/03/25 09:45:00 jhardie 1 0
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
//**********************************************************************
// info_messages
// Routines which handle incoming and outgoing SmartSockets messages
//
// Version: %A% %I%
//
// Programmer: J.G. Hardie
//**********************************************************************
I 16
#include <fstream.h>
E 16

#include "info_includes.hh"
D 3
#include "info_proto.hh"
E 3
#include "info_classes.hh"
#include "info_globals.hh"
I 3
#include "info_proto.hh"
E 3

I 16
void internal_process_info_serv(TipcMsg &msg);
void check_lifetimes();
E 16

I 3
T_BOOL FieldType(TipcMsg &m, T_IPC_FT *t);

I 12
void downcase(char *s)
{
  while (*s)
  {
    if (isalpha(*s))
      *s = tolower(*s);
    s++;
  }
}

E 12
D 4
extern TipcSrv &ss_serv;
E 4

I 17
//<<<<<<< info_messages.cc.C.0bqZX8
E 17
D 16
DEBUG(
  // This function is only included if we are debugging
D 15
  char *PrintFieldType(T_IPC_FT t);
E 15
I 15
  const char *PrintFieldType(T_IPC_FT t);
E 15
  
D 15
  char *PrintFieldType(T_IPC_FT t)
E 15
I 15
  const char *PrintFieldType(T_IPC_FT t)
E 15
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
    case T_IPC_FT_REAL4:
      return "Real*4";
    case T_IPC_FT_REAL4_ARRAY:
      return "Real*4 Array";
    case T_IPC_FT_REAL8:
      return "Real*8";
    case T_IPC_FT_REAL8_ARRAY:
      return "Real*8 Array";
    case T_IPC_FT_MSG:
      return "Smartsockets Message";
    case T_IPC_FT_MSG_ARRAY:
      return "Smartsockets Message Array";
    default:
      return "Smartsockets Internal Type";
    }
E 16
I 16
// Function to return a string representation of the smartsockets
// field type.
D 17
char *PrintFieldType(T_IPC_FT t);
E 17
I 17
D 18
DEBUG(
E 18
I 18
//DEBUG(
E 18
  // This function is only included if we are debugging
  const char *PrintFieldType(T_IPC_FT t);
E 17
  
D 17
char *PrintFieldType(T_IPC_FT t)
E 17
I 17
//<<<<<<< info_messages.cc.C.0bqZX8
  const char *PrintFieldType(T_IPC_FT t)
E 17
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
E 16
  }
D 16
  ); 
E 16
I 16
}

E 16


E 3
//----------------------------------------------------------------------
// setup_ss_callbacks
// creates the callback functions for any special incoming data groups.
// or message types
//----------------------------------------------------------------------
void setup_ss_callbacks()
{
    return;
I 3
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
I 13
      DEBUG(cout << "IpcOpenInput: Connected to datagroup!" << endl;);
E 13
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
I 16
  internal_process_info_serv(msg);
}

void internal_process_info_serv(TipcMsg &msg)
{
E 16
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
I 16

  if (log_any)
    cout << "InfoServer : found input message " << in_name << endl;
E 16
  
  // try to find the matching input node.
  if (!(in = (input *)inputlist.find(in_name)))
  {
D 11
    cout << "InfoServer: Input message without matching definition" << endl;
E 11
I 11
D 16
    cout << "InfoServer: Input message without matching definition: " 
	 << in_name << endl;
E 16
I 16
    if (log_unknown)
      cout << "InfoServer: Input message without matching definition: " 
	   << in_name << endl;
E 16

E 11
    return;
  }

  DEBUG(cout << "ProcessInfoServ - Found the input - parsing..." << endl;);
D 16

I 12
  cout << "InfoServer: Found message " << in_name << endl;
E 16
I 16
  
  // So, looks like a valid message.  So we need to store the message 
  // structure itself in the input source structure.
  in->setmessage(msg);
E 16
  
E 12
  // Got it!  Process the data.
D 4
  process_input_msg(msg,in);
  
  DEBUG(
    llist &thedat = in->input_data();
    printlist(thedat);
    );
  
  // Now, the message is fully processed, we have to reconcile the
  // outputs with the new input and see if any messages have to be
  // built.
  DEBUG(cout << "ProcessInfoServ - Parse OK, check for outputs" << endl;);
  reconcile_output();
  
E 4
I 4
  if (process_input_msg(msg,in))
  {
    in->timestamp(last_input_time = timestamp());
    DEBUG(
      llist &thedat = in->input_data();
      printlist(thedat);
      );
    
I 16
    // Reset the lifetime for this input object.
    in->resetlife();

E 16
    // Now, the message is fully processed, we have to reconcile the
    // outputs with the new input and see if any messages have to be
    // built.
    DEBUG(cout << "ProcessInfoServ - Parse OK, check for outputs" << endl;);
D 16
    reconcile_output();
E 16
I 16
    if (not_loading_state)
      reconcile_output();
E 16
  }
  else
  {
I 16
    if (log_bad)
      cout << "InfoServer: Message " << in_name
	   << " did not match definition." << endl;
    
E 16
    in->data_clear();
  }

E 4
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
D 4
void process_input_msg(TipcMsg &msg, input *in)
E 4
I 4
int process_input_msg(TipcMsg &msg, input *in)
E 4
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
D 16
      if (field_t == T_IPC_FT_INT4)
E 16
I 16
      if ((field_t == T_IPC_FT_INT2) ||
          (field_t == T_IPC_FT_INT4) ||
          (field_t == T_IPC_FT_INT8))
E 16
      {
	integer *i = new integer;
	i->name(n->name());
	i->type(n->type());
D 16
	msg >> int_dat >> Check;
E 16
I 16
//	msg >> int_dat >> Check;
        i->MsgRead(msg,field_t);
E 16
	if (!msg)
	{
I 16
	  if (log_bad && good_input)
	    cout << "ProcessInputMsg - problem with Integer data" << endl;
E 16
I 4
	  good_input = 0;
E 4
D 16
	  cout << "ProcessInputMsg - problem with Integer data" << endl;
E 16
	  delete i;
	}
	else
	{
D 16
	  i->value = int_dat;
E 16
I 16
//	  i->value = int_dat;
E 16
	  in->data_insert(i);
	}
      }
      else
      {
I 16
	if (log_bad && good_input)
	  cout << "ProcessInputMsg - expecting an integer but didnt get it ("
	       << n->name() << ")"
	       << endl;
E 16
I 4
	good_input = 0;
E 4
D 16
	cout << "ProcessInputMsg - expecting an integer but didnt get it"
	     << endl;
E 16
      }
      break;
    }
    case t_floating:
    {
D 16
      if (field_t == T_IPC_FT_REAL8)
E 16
I 16
      if ((field_t == T_IPC_FT_REAL4) ||
          (field_t == T_IPC_FT_REAL8) ||
          (field_t == T_IPC_FT_REAL16))
E 16
      {
	floating *i = new floating;
	i->name(n->name());
	i->type(n->type());
D 16
	msg >> float_dat >> Check;
E 16
I 16
//	msg >> float_dat >> Check;
        i->MsgRead(msg,field_t);

E 16
	if (!msg)
	{
I 16
	  if (log_bad && good_input)
	    cout << "ProcessInputMsg - problem with Floating data" << endl;
E 16
I 4
	  good_input = 0;
E 4
D 16
	  cout << "ProcessInputMsg - problem with Floating data" << endl;
E 16
	  delete i;
	}
	else
	{
D 16
	  i->value = float_dat;
E 16
I 16
//	  i->value = float_dat;
E 16
	  in->data_insert(i);
	}
      }
      else
      {
I 16
	if (log_bad && good_input)
	  cout << "ProcessInputMsg - expecting a float but didnt get it ("
	       << n->name() << ")"
	       << endl;
E 16
I 4
	good_input = 0;
E 4
D 16
	cout << "ProcessInputMsg - expecting a float but didnt get it"
	     << endl;
E 16
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
I 16
	  if (log_bad && good_input)
	    cout << "ProcessInputMsg - problem with String data" << endl;
E 16
I 4
	  good_input = 0;
E 4
D 16
	  cout << "ProcessInputMsg - problem with String data" << endl;
E 16
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
I 16
	if (log_bad && good_input)
	  cout << "ProcessInputMsg - expecting an string but didnt get it ("
	       << n->name() << ")"
	       << endl;
E 16
I 4
	good_input = 0;
E 4
D 16
	cout << "ProcessInputMsg - expecting an string but didnt get it"
	     << endl;
E 16
      }
      break;
    }
    case t_array:
    {
D 16
      if (field_t == T_IPC_FT_INT4_ARRAY)
E 16
I 16
      if ((field_t == T_IPC_FT_INT2_ARRAY) || 
          (field_t == T_IPC_FT_INT4_ARRAY) || 
          (field_t == T_IPC_FT_INT8_ARRAY))
E 16
      {
	array *i = new array;
	i->name(n->name());
	i->type(n->type());
	i->arr_type(arr_int);
D 16
	msg >> int_array >> GetSize(&arr_size) >> Check;
	DEBUG(cout << "ProcessInputMsg: got integer array.  Size "
	      << arr_size << endl;);
	DEBUG(if (arr_size > 0)
	      for (int aiz=0; aiz < 10; aiz++)
	      cout << "Array[" << aiz << "] = " << int_array[aiz] << endl;);
	
E 16
I 16
//	msg >> int_array >> GetSize(&arr_size) >> Check;
// 	DEBUG(cout << "ProcessInputMsg: got integer array.  Size "
// 	      << arr_size << endl;);
// 	DEBUG(if (arr_size > 0)
// 	      for (int aiz=0; aiz < arr_size; aiz++)
// 	      cout << "Array[" << aiz << "] = " << int_array[aiz] << endl;);

	i->MsgRead(msg,field_t);
E 16
	if (!msg)
	{
I 16
	  if (log_bad && good_input)
	    cout << "ProcessInputMsg - problem with IntArray data" << endl;
E 16
I 4
	  good_input = 0;
E 4
D 16
	  cout << "ProcessInputMsg - problem with IntArray data" << endl;
E 16
	  delete i;
	}
	else
	{
D 16
	  i->data(int_array,arr_size);
E 16
I 16
//	  i->data(int_array,arr_size);
E 16
	  in->data_insert(i);
	}
      }
D 16
      else if (field_t == T_IPC_FT_REAL8_ARRAY)
E 16
I 16
      else if ((field_t == T_IPC_FT_REAL4_ARRAY) ||
               (field_t == T_IPC_FT_REAL8_ARRAY) ||
               (field_t == T_IPC_FT_REAL16_ARRAY))
E 16
      {
	array *i = new array;
	i->name(n->name());
	i->type(n->type());
	i->arr_type(arr_float);
D 16
	msg >> flt_array >> GetSize(&arr_size) >> Check;
	DEBUG(cout << "ProcessInputMsg: got real array.  Size "
	      << arr_size << endl;);
	DEBUG(if (arr_size > 0)
	      for (int aiz=0; aiz < 10; aiz++)
	      cout << "Array[" << aiz << "] = " << flt_array[aiz] << endl;);
E 16
I 16
        i->MsgRead(msg,field_t);
// 	msg >> flt_array >> GetSize(&arr_size) >> Check;
// 	DEBUG(cout << "ProcessInputMsg: got real array.  Size "
// 	      << arr_size << endl;);
// 	DEBUG(if (arr_size > 0)
// 	      for (int aiz=0; aiz < arr_size; aiz++)
// 	      cout << "Array[" << aiz << "] = " << flt_array[aiz] << endl;);
E 16
	if (!msg)
	{
I 16
	  if (log_bad && good_input)
	    cout << "ProcessInputMsg - problem with FltArray data" << endl;
E 16
I 4
	  good_input = 0;
E 4
D 16
	  cout << "ProcessInputMsg - problem with FltArray data" << endl;
E 16
	  delete i;
	}
	else
	{
D 16
	  i->data(flt_array,arr_size);
E 16
I 16
//	  i->data(flt_array,arr_size);
E 16
	  in->data_insert(i);
	}
      }
      else
      {
I 16
	if (log_bad && good_input)
	  cout << "ProcessInputMsg - expecting an array but didnt get it ("
	       << n->name() << ")"
	       << endl;
E 16
I 4
	good_input = 0;
E 4
D 16
	cout << "ProcessInputMsg - expecting an array but didnt get it"
	     << endl;
E 16
      }
      break;
    }
I 9
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
D 13
      
E 13
I 13

      DEBUG(cout << "Entered processing for TAGGED field" << endl;)
E 13
      for (Counter = CurrentPos; Counter < NumFields; Counter++)
      {
	msg.Current(Counter);
	FieldType(msg,&ftype);
	if (ftype == T_IPC_FT_STR)
	{
	  msg >> tagname >> Check;
I 12
	  downcase(tagname);
E 12
D 13
	  if (!strncmp(tagname,n->name(),strlen(n->name())))
E 13
I 13
	  DEBUG(cout << "Got an ascii string - the message has : "
		<< tagname << endl << "Looking for : "
		<< ((tagged *)n)->tagname() << endl;);
	  if (!strncmp(tagname,((tagged *)n)->tagname(),
		       strlen(((tagged *)n)->tagname())))
E 13
	  {
I 13
	    DEBUG(cout << "Got it!" << endl;);
E 13
	    // match.  Save the sucker
	    Counter = NumFields;       //force exit
	    FieldType(msg,&ftype);
D 16
	    if (ftype == T_IPC_FT_INT4_ARRAY)
E 16
I 16
	    if ((ftype == T_IPC_FT_INT2_ARRAY) ||
                (ftype == T_IPC_FT_INT4_ARRAY) ||
                (ftype == T_IPC_FT_INT8_ARRAY))
E 16
	    {
	      array *i = new array;
	      i->name(n->name());
	      i->type(n->type());
	      i->arr_type(arr_int);
D 16
	      msg >> int_array >> GetSize(&arr_size) >> Check;
	      DEBUG(cout << "ProcessInputMsg: got integer array.  Size "
		    << arr_size << endl;);
	      DEBUG(if (arr_size > 0)
		    for (int aiz=0; aiz < 10; aiz++)
		    cout << "Array[" << aiz << "] = " << int_array[aiz] 
		    << endl;);
E 16
I 16
              i->MsgRead(msg,ftype);
// 	      msg >> int_array >> GetSize(&arr_size) >> Check;
// 	      DEBUG(cout << "ProcessInputMsg: got integer array.  Size "
// 		    << arr_size << endl;);
// 	      DEBUG(if (arr_size > 0)
// 		    for (int aiz=0; aiz < arr_size; aiz++)
// 		    cout << "Array[" << aiz << "] = " << int_array[aiz] 
// 		    << endl;);
E 16
	      
	      if (!msg)
	      {
I 16
		if (log_bad && good_input)
		  cout << "ProcessInputMsg - problem with IntArray data" 
		       << endl;
E 16
		good_input = 0;
D 16
		cout << "ProcessInputMsg - problem with IntArray data" 
		     << endl;
E 16
		delete i;
	      }
	      else
	      {
D 16
		i->data(int_array,arr_size);
E 16
I 16
//		i->data(int_array,arr_size);
E 16
		in->data_insert(i);
	      }
	    }
D 16
	    else if (ftype == T_IPC_FT_REAL8_ARRAY)
E 16
I 16
	    else if ((ftype == T_IPC_FT_REAL4_ARRAY) ||
                     (ftype == T_IPC_FT_REAL8_ARRAY) ||
                     (ftype == T_IPC_FT_REAL16_ARRAY))
E 16
	    {
	      array *i = new array;
	      i->name(n->name());
	      i->type(n->type());
	      i->arr_type(arr_float);
D 16
	      msg >> flt_array >> GetSize(&arr_size) >> Check;
	      DEBUG(cout << "ProcessInputMsg: got real array.  Size "
		    << arr_size << endl;);
	      DEBUG(if (arr_size > 0)
		    for (int aiz=0; aiz < 10; aiz++)
		    cout << "Array[" << aiz << "] = " << flt_array[aiz] 
		    << endl;);
E 16
I 16
              i->MsgRead(msg,ftype);

// 	      msg >> flt_array >> GetSize(&arr_size) >> Check;
// 	      DEBUG(cout << "ProcessInputMsg: got real array.  Size "
// 		    << arr_size << endl;);
// 	      DEBUG(if (arr_size > 0)
// 		    for (int aiz=0; aiz < arr_size; aiz++)
// 		    cout << "Array[" << aiz << "] = " << flt_array[aiz] 
// 		    << endl;);
E 16
	      if (!msg)
	      {
I 16
		if (log_bad && good_input)
		  cout << "ProcessInputMsg - problem with FltArray data" 
		       << endl;
E 16
		good_input = 0;
D 16
		cout << "ProcessInputMsg - problem with FltArray data" 
		     << endl;
E 16
		delete i;
	      }
	      else
	      {
D 16
		i->data(flt_array,arr_size);
E 16
I 16
//		i->data(flt_array,arr_size);
E 16
		in->data_insert(i);
	      }
	    }
	    else
	    {
I 16
	      if (log_bad && good_input)
		cout << "ProcessInputMsg - expecting an array but didnt get it ("
		     << n->name() << ")"
		     << endl;
E 16
	      good_input = 0;
D 16
	      cout << "ProcessInputMsg - expecting an array but didnt get it"
		   << endl;
E 16
	    }
	  }
	}
      }
      msg.Current(CurrentPos);
      break;
    }
E 9
    case t_block:
    {
I 16
      if (log_bad && good_input)
      {
	cout << "Got a block, but ignoring it." << endl;
	cout << "BLOCK COMMAND IS NOT YET IMPLEMENTED!!!" << endl;
      }
E 16
I 4
      good_input = 0;
E 4
D 16
      cout << "Got a block, but ignoring it." << endl;
      cout << "BLOCK COMMAND IS NOT YET IMPLEMENTED!!!" << endl;
E 16
      break;
    }
    }
  }
I 4
  return good_input;
E 4
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
I 4
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
I 6
  time_t only_ignore;
  
E 6

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
I 6
D 13
    only_ignore = 0;
E 13
I 13
    only_ignore = 1;
E 13
E 6
    
    // Check each component in the output stream.
    while (onode = o->current())
    {
      hits = 0;
D 13
      
E 13
I 13

      // Note: we ignore the timestamp for this search...
E 13
      ttype = onode->type();
D 13
      if (ttype != t_reference)	// does not refer to the inputs.
E 13
I 13
      if (ttype != t_reference)	// doesn't refer to inputs
E 13
	continue;
      
      oc = (out_comp *)onode;
D 6
      if (oc->ignoretime)	// if we can ignore time, don't check.
	continue;
E 6

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
D 6
	  i_time = i->timestamp();
	  if (i_time > newest)
	    newest = i_time;	// keep the latest time around for comparison.
E 6
I 6
	  if (!oc->ignoretime)	// if we can ignore time, don't check.
	  {
I 9
	    DEBUG(cout << "Time Counts" << endl;);
E 9
	    only_ignore = 0;
	    i_time = i->timestamp();
D 13
	    if (i_time > newest)
	      newest = i_time;	// keep the latest time around for comparison.
E 13
I 13
	    if (!newest)
	      newest = i_time;
	    if (i_time < newest)
	      newest = i_time;	// keep the earliest time around.
E 13
	  }
	  else
I 9
	  {
	    DEBUG(cout << "Time ignored" << endl;);
E 9
D 13
	    only_ignore = 1;
E 13
I 9
	  }
	  
E 9
E 6
	}
      }
      DEBUG(if (!hits) 
	    {
	      cout << "ReconcileOutput - comp " << onode->name()
		   << " was not found" << endl;
I 5
	    }
	    else
	    {
	      cout << "ReconcileOutput - comp " << onode->name()
		   << " was found" << endl;
E 5
	    });
      
      safe_to_build = safe_to_build * hits;
    }
    
    // Well, we finished searching for all the relevant components
    // of the output stream.  Now check to see if the stream needs
    // to be rebuilt.  If so, generate the output message.
D 6
    if ((safe_to_build) && (newest > o_time))
E 6
I 6
    if ((safe_to_build) && ((newest > o_time) || only_ignore))
E 6
    {
      DEBUG(cout << "ReconcileOutput - node needs to be built" << endl;);
      construct_output(o);
I 10
      o->timestamp(last_output_time = timestamp());
E 10
    }
    else
    {
      DEBUG(cout << "ReconcileOutput - node does not need building" << endl;);
    }
    
  }

  // my job here is finished.
  DEBUG(cout << "ReconcileOutput - exiting" << endl;);
    
E 4
E 3
}
I 4

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
D 14
  TipcMsg msg("info_server");
E 14
I 14
  TipcMsg msg(CAST "info_server");
E 14
  long i,j,datalength;
  long *idata;
  double *fdata;

I 5
  DEBUG(cout << "ConstructOutput - entered" << endl;);
  
E 5
  out->top();

  msg.Dest(out->dgroup());
D 14
  msg.Sender("info_server");
E 14
I 14
  msg.Sender(CAST "info_server");
E 14
  msg.Current(0);
  
  // Ok, loop through the components and build the message.  
  while (onode = out->current())
  {
    switch (onode->type())
    {
    case t_name:		// append output stream name
I 5
      DEBUG(cout << "ConstructOutput - appending output name "
	    << out->name() << endl;);
      
E 5
      msg << out->name() << Check;
      break;
    case t_literal:		// write a fixed string.
    {
      literal *l = (literal *)onode;
I 5
      DEBUG(cout << "ConstructOutput - appending a literal "
	    << l->str() << endl;);
      
E 5
      msg << l->str() << Check;
I 13
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
E 13
      break;
    }
    case t_reference:		// refers to the input somewhere.
    {
      out_comp *oc = (out_comp *)onode;
      input *icomp;

I 5
      DEBUG(cout << "ConstructOutput - appending a reference to " 
	    << oc->name() << endl;);
      
E 5
      inputlist.top();
      while (icomp = (input *)inputlist.current())
      {
	inputlist.next();
	icomp->data_top();
I 5
	DEBUG(cout << "ConstructOutput - checking input"
	      << icomp->name() << endl;);
	
E 5
	if (inode = icomp->data_find(oc->name()))
	{
I 5
	  DEBUG(cout << "ConstructOutput - found the object.  It is"
		<< " type ";);
	  
E 5
	  switch (inode->type())
	  {
	  case t_integer:	// output a single integer
	  {
	    integer *n = (integer *)inode;
I 5
D 21
	    DEBUG(cout << "integer, value = " << n->value << endl;);
E 21
I 21
//	    DEBUG(cout << "integer, value = " << n->value << endl;);
E 21
E 5
D 16
	    msg << n->value << Check;
E 16
I 16
//	    msg << n->value << Check;
            n->MsgWrite(msg);
E 16
	    break;
	  }
	  case t_floating:	// a single floating point number.
	  {
	    floating *n = (floating *)inode;
I 5
D 21
	    DEBUG(cout << "floating, value = " << n->value << endl;);
E 21
I 21
//	    DEBUG(cout << "floating, value = " << n->value << endl;);
E 21
E 5
D 16
	    msg << n->value << Check;
E 16
I 16
//	    msg << n->value << Check;
            n->MsgWrite(msg);
E 16
	    break;
	  }
	  case t_string:	// a character string
	  {
	    cstring *n = (cstring *)inode;
I 5
	    DEBUG(cout << "string, value = " << n->str() << endl;);
E 5
	    msg << n->str() << Check;
	    break;
	  }
I 9
	  case t_tagged:
	    msg << inode->name() << Check;
	    
E 9
	  case t_array:		// an array of numbers.
	  {
	    array *n = (array *)inode;

	    datalength = n->length();
D 5
	    if (n->type() == arr_int)
E 5
I 5
	    DEBUG(cout << "array, length = " << datalength 
		  << " elem_type ";);
D 16
	    
	    if (n->arr_type() == arr_int)
E 5
	    {
	      idata = n->idata();
D 5
	      
E 5
I 5
	      DEBUG(cout << "integer" << endl
		    << "Contents:" << endl;
		    for (int dbind=0; dbind < datalength; dbind++)
		    {
		      cout << setw(10) << idata[dbind];
		      if (!((dbind+1)%8))
			cout << endl;
		    }
		    if ((datalength%8))
		    cout << endl;);
E 5
	      msg << SetSize(datalength) << idata << Check;
	    }
D 5
	    else if (n->type() == arr_float)
E 5
I 5
	    else if (n->arr_type() == arr_float)
E 5
	    {
	      fdata = n->ddata();
I 5
	      DEBUG(cout << "float" << endl
		    << "Contents:" << endl;
		    for (int dbind=0; dbind < datalength; dbind++)
		    {
		      cout << setw(10) << fdata[dbind];
		      if (!((dbind+1)%8))
			cout << endl;
		    }
		    if ((datalength%8))
		    cout << endl;
		);
E 5
	      msg << SetSize(datalength) << fdata << Check;
	    }
E 16
I 16

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
E 16
	    break;		// breaks the inner switch.
	  }
	  }
D 6
	  break;		// breaks the while input
E 6
	}
I 6
	else if (inode = icomp->find(oc->name()))
	{
	  DEBUG(cout << "ConstructOutput - found the parse object.  It is"
		<< " type ";);
	  
	  switch (inode->type())
	  {
	  case t_integer:	// output a single integer
	  {
	    DEBUG(cout << "integer, value = " << 0 << endl;);
D 7
	    msg << int(0) << Check;
E 7
I 7
D 8
	    int val = 0;
E 8
I 8
D 16
	    long val = 0;
E 16
I 16
	    T_INT4 val = 0;
E 16
E 8
	    msg << val << Check;
E 7
	    break;
	  }
	  case t_floating:	// a single floating point number.
	  {
I 7
D 16
	    double val = 0.0;
E 16
I 16
	    T_REAL8 val = 0.0;
E 16
E 7
	    DEBUG(cout << "floating, value = " << 0.0 << endl;);
D 7
	    msg << double(0.0) << Check;
E 7
I 7
	    msg << val << Check;
E 7
	    break;
	  }
	  case t_string:	// a character string
	  {
	    DEBUG(cout << "string, value = " << " " << endl;);
D 14
	    msg << " " << Check;
E 14
I 14
	    msg << CAST " " << Check;
E 14
	    break;
	  }
I 9
	  case t_tagged:
	    msg << inode->name() << Check;
	    
E 9
	  case t_array:		// an array of numbers.
	  {
	    array *n = (array *)inode;

	    datalength = n->length();
	    DEBUG(cout << "array, length = " << datalength 
		  << " elem_type ";);
	    
	    if (n->arr_type() == arr_int)
	    {
D 16
	      int arr[1];
E 16
I 16
	      T_INT4 arr[1];
E 16
	      arr[0] = 0;
	      DEBUG(cout << "integer" << endl);
D 16
	      msg << SetSize(1) << arr << Check;
E 16
I 16
	      msg << SetSize(1) << (T_INT4 *)arr << Check;
E 16
	    }
	    else if (n->arr_type() == arr_float)
	    {
	      DEBUG(cout << "float" << endl);
D 16
	      double arr[1];
E 16
I 16
	      T_REAL8 arr[1];
E 16
	      arr[0] = 0.0;

D 16
	      msg << SetSize(1) << arr << Check;
E 16
I 16
	      msg << SetSize(1) << (T_REAL8 *)arr << Check;
E 16
	    }
	    break;		// breaks the inner switch.
	  }
	  }
	}
D 9
	break;		// breaks the while input
E 9
I 9
//	break;		// breaks the while input
E 9
E 6
      }
I 6
      
E 6
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

    
    
I 6
//----------------------------------------------------------------------
// handle_timers()
// This routine gets called every time there is a timeout.  It scans
// the output list looking for output events which have timers on them.
// If the output list has an enabled timer, the value of this timer is
// checked.  If the timer triggers, then the output object is built
// and the message sent.  Then the timer is set back to the original
// time.
I 16
//
// The variable dumptimer is used to keep track of the interval between
// complete data dumps.
//
// The variable statetimer is used to keep track of the interval between
// internal state dumps.  The constant stateinterval sets the interval
// to 10 minutes.
E 16
//----------------------------------------------------------------------
I 16
int dumptimer = 0;
int statetimer = 0;

void dump_inputs_now();

E 16
void handle_timers()
{
  outputlist.top();
  output *o;
I 16

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
  
E 16
  

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
I 16

  inputlist.top();
  input *in;
  while (in = (input *)inputlist.current())
  {
    if (in->checklifetime())
      in->data_clear();
    
    inputlist.next();
  }
  
E 16
}

E 6
      
I 16
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
D 19
void save_state_now(char *filename)
E 19
I 19
void save_state_now(const char *filename)
E 19
{
D 20
  TipcMsgFile msgfile(filename,T_IPC_MSG_FILE_CREATE_WRITE);
E 20
I 20
  TipcMsgFile msgfile(const_cast<char *>(filename),
		      T_IPC_MSG_FILE_CREATE_WRITE);
E 20
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
D 21
void process_info_state(char *filename)
E 21
I 21
void process_info_state(const char *filename)
E 21
{
D 21
  TipcMsgFile msgfile(filename,T_IPC_MSG_FILE_CREATE_READ);
E 21
I 21
  TipcMsgFile msgfile(const_cast<char *>(filename),
		      T_IPC_MSG_FILE_CREATE_READ);
E 21
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

E 16
D 6
	  
	    
E 6
E 4
E 1
