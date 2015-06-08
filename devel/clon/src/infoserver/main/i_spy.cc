//**********************************************************************
// i_spy
// Program to watch all the messages being sent by the info_server
// to all output channels.  It reads an info_server configuration file
// and opens all the specified smartsockets datagroups.  It then sets
// up an input loop waiting on callbacks of type info_server.  When
// it gets an info_server message, it prints all the fields to stdout
// and goes back to sleep.
//
// Version: @(#) i_spy.cc 1.9@(#) 1.9 on 05/02/25 at 16:25:44
//
//**********************************************************************

#include "info_includes.hh"
#include "info_classes.hh"
#include "info_proto.hh"

extern llist inputlist;
extern llist outputlist;
extern llist aliaslist;


// Command line arguments
extern "C" {
#include "procargs.h"
}

// Command line variables
const char *app_name = "testing";
const char *config_file = "/usr/local/clas/parms/info_server/info_server.cfg";
const char *unique_name = "i_spy";
const char *datagroup = NULL;
const char *msg_file = 0;


ARGTABLE
ARGENTRY("app",STRING,&app_name,"is the application name"),
ARGENTRY("config",FILENAME,&config_file,"is the configuration file name"),
ARGENTRY("msg",FILENAME,&msg_file,"is the smartsocket message file name"),
ARGENTRY("uniq",STRING,&unique_name,"is the programs unique name"),
ARGENTRY("dg",STRING,&datagroup,"is the datagroup to monitor"),
ARGENTRY("help",USAGE,NULL,NULL),
ENDTABLE

#define INFO_SERV 13

TipcSrv &ss_serv = TipcSrv::Instance();

TipcMsg globmsg;

// Some prototypes

T_BOOL FieldType(TipcMsg &m, T_IPC_FT *t);
const char *PrintFieldType(T_IPC_FT t);
void process_info_serv(T_IPC_CONN conn, T_IPC_CONN_DEFAULT_CB_DATA data,
		       T_CB_ARG arg);
  
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
}


int main(int argc, char **argv)
{
  cout << "@(#) i_spy.cc 1.9@(#) 1.9 Starting" << endl;

  GETARGS(argc,argv);
  
  if (!read_config(config_file))
  {
    cerr << "I_Spy: Unable to read config file " << config_file
	 << endl;
    return T_EXIT_FAILURE;
  }

  if (!TutCommandParseStdLicense())
  {
    cerr << "I_Spy: Unable to load SmartSockets license" << endl;
    return T_EXIT_FAILURE;
  }
  
  
  // Set the application name
  T_OPTION option;
  option = TutOptionLookup((char *)"application");
  if (!option)
  {
    cout << "Error - could not get application name" << endl;
    return T_EXIT_FAILURE;
  }
  
  if (!TutOptionSetEnum(option, CAST app_name))
  {
    cout << "Error - could not set application name" << endl;
    return T_EXIT_FAILURE;
  }

  // We don't really want the unique name to be set.  We *do* want
  // to receive the datagroup specified by the unique name.
#ifdef DO_UNIQUE_REALLY
  option = TutOptionLookup("unique_datagroup");
  if (!option)
  {
    cout << "Error - could not get unique datagroup name" << endl;
    return T_EXIT_FAILURE;
  }
  
  if (!TutOptionSetEnum(option, unique_name))
  {
    cout << "Error - could not set unique name" << endl;
    return T_EXIT_FAILURE;
  }
#endif DO_UNIQUE_REALLY

  // Create the info_server message type
  TipcMt mt((char *)"info_server", INFO_SERV, (char *)"verbose");
  if (!mt)
  {
    cout << "Error - could not create message type" << endl;
    return T_EXIT_FAILURE;
  }

  if (!msg_file)
  {
    if (!ss_serv.Create(T_IPC_SRV_CONN_FULL))
    {
      cerr << "I_Spy - unable to connect to SmartSockets" << endl;
      return T_EXIT_FAILURE;
    }

    ss_serv.StdDgSetRecv(TRUE,FALSE);
    if (!ss_serv.DgRecv(CAST unique_name,TRUE))
    {
      cout << "InfoServer: Unable to connect to dg " << unique_name << endl;
    }
    ss_serv.Flush();
    
  }
  
  // Next, set a callback for the incoming message type "info_server"
  mt.Lookup((char *)"info_server");
  if (!mt)
  {
    cout << "Info_Server: could not find info_server mt" << endl;
    return T_EXIT_FAILURE;
  }
  
  if (!msg_file)
  {
    if (!ss_serv.ProcessCbCreate(mt,process_info_serv,0))
    {
      cout << "Info_Server: could not set info_server cb" << endl;
      return T_EXIT_FAILURE;
    }
  }
  

  // We next have to open a data channel to all the required
  // data streams.  We do this by looping through the output list
  // and trying to attach to each specified datagroup.
  if (!msg_file)
  {
    output *o;
    outputlist.top();
    while (o = (output *)outputlist.current())
    {
      outputlist.next();
      char *dg;
      dg = o->dgroup();
      if (datagroup)
      {
	if (!strcmp(datagroup,dg))
	{
	  cout << "I-Spy: Accepting datagroup " << dg << endl;
	  if (!ss_serv.DgRecv(dg,TRUE))
	  {
	    cout << "I-Spy: unable to watch datagroup " << dg << endl;
	    return T_EXIT_FAILURE;
	  }
	}
      }
      else
      {
	cout << "I-Spy: Accepting datagroup " << dg << endl;
	if (!ss_serv.DgRecv(dg,TRUE))
	{
	  cout << "I-Spy: unable to watch datagroup " << dg << endl;
	}
      }
    }
    ss_serv.Flush();
  }
  
  
  // Well, the message callback has been set, so we go to sleep now
  // and wait for some well-intentioned soul to kill us later.
  if (!msg_file)
    ss_serv.MainLoop(T_TIMEOUT_FOREVER);
  else
  {
    TipcMsgFile mfp(CAST msg_file,T_IPC_MSG_FILE_CREATE_READ);
    int keep_going;

    keep_going = 1;
    while (keep_going)
    {
      mfp >> globmsg;
      if (!mfp)
	break;
      process_info_serv(0,0,0);
    }
  }
  
      

  cout << "I_Spy - going away now" << endl;
  return T_EXIT_SUCCESS;
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
  TipcMsg msg;
  
  T_IPC_FT field_t;
  T_STR in_name, temp;
  input *in;
  long nfields;

  // Data components used in the parsing.
  T_STR string_dat;
  T_INT4 int_dat;
  T_REAL8 float_dat;
  T_INT4 arr_size;
  T_INT4 *int_array;
  T_REAL8 *flt_array;

  if (!msg_file)
    msg = data->msg;
  else
    msg = globmsg;
  
  DEBUG(cout << "ProcessInfoServ - entered" << endl;);
  
  // Reset the message so that we can look at the first field.
  msg.Current(0);
  if (!msg.Status())
  {
    cerr << "I-Spy: Cannot reset info_server message - ignored" << endl;
    return;
  }

  // Loop through the fields of the message and print out each
  // element.  First, we check to see if the first field is a
  // character string.  If it is, we print an informative message.
  // (Note: since these are supposed to be info_server messages, the
  // first field *should* be a character string.
  cout << "Got a message on datagroup " << msg.Dest() << " from "
       << msg.Sender() << endl;

  cout << "Message Headers:" << endl;
  cout << "DataSize: " << msg.DataSize() << "   Number of Fields: "
       << msg.NumFields() << "    Sequence #: " << msg.SeqNum() << endl;
  cout << "Delivery Mode: " << msg.DeliveryMode() << "    Type: "
       << msg.Type() << endl;

  cout << "Message Contents:" << endl;
  nfields = msg.NumFields();

  for (int i = 0; i < nfields; i++)
  {
    FieldType(msg,&field_t);
    cout << PrintFieldType(field_t) << ": " ;
    switch (field_t)
    {
    case T_IPC_FT_STR:
      msg >> string_dat >> Check;
      cout << string_dat << endl;
      break;
      
    case T_IPC_FT_INT4:
      msg >> int_dat >> Check;
      cout << int_dat << endl;
      break;
      
    case T_IPC_FT_INT4_ARRAY:
    {
      msg >> int_array >> GetSize(&arr_size);
      cout << "size: " << arr_size << endl;
      for (int j = 0; j < arr_size; j++)
      {
	cout << setw(10) << int_array[j];
	if (!((j+1)%8))
	  cout << endl;
      }
      if ((arr_size%8))
	cout << endl;
      break;
    }
    
    case T_IPC_FT_REAL8:
      msg >> float_dat >> Check;
      cout << float_dat << endl;
      break;
      
    case T_IPC_FT_REAL8_ARRAY:
    {
      msg >> flt_array >> GetSize(&arr_size);
      cout << "size " << arr_size <<  endl;
      for (int j = 0; j < arr_size; j++)
      {
	cout << setw(12) << flt_array[j];
	if (!((j+1)%6))
	  cout << endl;
      }
      if ((arr_size %6))
	cout << endl;
      break;
    }
    
    default:
      cout << "Not valid for an infoserver message" << endl;
      msg.Current(i);
      break;
    }
  }
  
  
  // All done - go back to sleep.
  cout << endl << endl;
  
  DEBUG(cout << "ProcessInfoServ - exiting" << endl;);
  return;
}
