//**********************************************************************
// info_in_test
// Program to generate input messages according to a configuration file
// and send them off to the info server.
//
// Programmed by: jgh
//**********************************************************************

#include "info_includes.hh"

#define RESERVE_SPACE
#include "info_classes.hh"
#include "info_globals.hh"
#undef RESERVE_SPACE


// Configuration related prototypes
int read_config(const char *f);
char *data_stream_status();

void build_input(TipcSrv &s, input *i, int query);
int get_next_cmd();


#define INFO_SERV 13

// Command line arguments
extern "C" {
#include "procargs.h"
}

// Command line variables
const char *app_name = "clasprod";
const char *config_file = "/usr/local/clas/parms/info_server/info_server.cfg";
const char *unique_name = "info_server";
const char *msg_file = 0;

ARGTABLE
ARGENTRY("app",STRING,&app_name,"is the application name"),
ARGENTRY("msg",STRING,&msg_file,"is the SmartSockets message file"),
ARGENTRY("config",STRING,&config_file,"is the configuration file"),
ARGENTRY("uniq",STRING,&unique_name,"is the programs unique name"),
ARGENTRY("help",USAGE,NULL,NULL),
ENDTABLE

TipcMsgFile *mfp = 0;

int main(int argc, char **argv)
{
    int status;

    GETARGS(argc,argv);
    
    // read the configuration file and build the event streams
    cout << "Reading the configuration file" << endl;
    if (!read_config(config_file))
    {
      cerr << "InfoServer:  Error in configuration file."
	   << "Info server cannot start.  Goodbye." << endl;
      exit(0);
    }

    // Read the license file
    if (!TutCommandParseStdLicense())
    {
      cout << "Error - could not load standard license file" << endl;
      return T_EXIT_FAILURE;
    }

    // Set the application name
    T_OPTION option;
    option = TutOptionLookup(CAST "application");
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
    
    // Create the info_server message type
    TipcMt mt(CAST "info_server", INFO_SERV, CAST "verbose");
    if (!mt)
    {
      cout << "Error - could not create message type" << endl;
      return T_EXIT_FAILURE;
    }
    

    if (!msg_file)
    {
      
      // Set the Smartsockets server connection.
      TipcSrv &ss_serv = TipcSrv::InstanceCreate(T_IPC_SRV_CONN_FULL);
      if (!ss_serv)
      {
	cout << "Error - could not connect to server" << endl;
	exit(T_EXIT_FAILURE);
      }

      ss_serv.StdDgSetRecv(TRUE,FALSE);
      ss_serv.Flush();
      
    }
    else
    {
      mfp = new TipcMsgFile(CAST msg_file,T_IPC_MSG_FILE_CREATE_WRITE);
    }
    
    
    // Main loop.  We want to print a message which asks which type
    // of input to construct and send out.
    int cmd;
    int query;
    while(cmd = get_next_cmd())
    {
      query = 0;
      input *i;
      if (!msg_file)
	ss_serv.MainLoop(0.0);
      if (cmd < 0)
      {
	cmd = -cmd;
	query = 1;
      }
      
      inputlist.top();
      for (int j=0; j < cmd-1; j++)
	inputlist.next();
      i = (input *)inputlist.current();
      build_input(ss_serv,i,query);
      if (!msg_file)
	ss_serv.MainLoop(0.0);
    }
    
    if (!ss_serv.Destroy(T_IPC_SRV_CONN_NONE))
    {
      cout << "Error - could not fully destroy server" << endl;
    }
    if (mfp)
      delete mfp;
    
    return T_EXIT_SUCCESS;
}

    
int get_next_cmd()
{
  input *i;
  int cmd,cvalid,maxin;

  inputlist.top();
  cout << "Enter next command and hit return: " << endl;
  cout << "CMD        Input Event" << endl;
  cmd = 1;
  cvalid = 0;
  maxin = 0;
  while (i = (input *)inputlist.current())
  {
    inputlist.next();
    maxin++;
    cout << cmd << "        " << i->name() << endl;
    cmd++;
  }
  cout << 0 << "        Quit" << endl << endl ;
  while (!cvalid)
  {
    cout << "Cmd> ";
    cin >> cmd;
    cin.ignore(10000,'\n');
    if ((0 <= cmd) && (cmd <= maxin))
      cvalid++;
    else if (cmd < 0)
      cvalid++;
    else
      cout << "Invalid command: " << cmd << endl;
  }
  return cmd;
}

//**********************************************************************
// build_input
// In this builder, all arrays are 25 elements long and contain the
// numbers from 1 to 25.
// All integers/floats are numbered sequentially and all strings are
// "ThisString"
//**********************************************************************
T_INT4 inta[] = {1,2,3,4,5,6,7,8,9,10};
T_REAL8 floata[] = {1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0};
const char *str = "ThisString";

void build_input(TipcSrv &t, input *i, int query)
{
  lnode *elem;
  
  long int_val = 0;
  double flt_val = 0.0;
  long q_int;
  double q_flt;
  long q_i_ary[5];
  double q_f_ary[5];
  char buf[128];

  cout << "BuildInput entered" << endl;

  TipcMsg msg(CAST "info_server");

  msg.Dest(CAST unique_name);
  msg.Sender(CAST "info_test");
  msg.Current(0);
 
  cout << "Resetting the input object" << endl;
  msg << i->name() << Check;
  
  i->top();
  while(elem = i->current())
  {
    cout << "    Adding a field " << endl;
    
    switch (elem->type())
    {
    case t_integer:
      cout << "Integer" << endl;
      if (query)
      {
	cin >> q_int;
	cin.ignore(10000,'\n');
	msg << q_int << Check;
      }
      else
      {
	msg << int_val << Check;
	int_val++;
      }
      break;
    case t_floating:
      cout << "Floating" << endl;
      if (query)
      {
	cin >> q_flt;
	cin.ignore(10000,'\n');
	msg << q_flt << Check;
      }
      else
      {
	msg << flt_val << Check;
	flt_val += 1.0;
      }
      break;
    case t_tagged:
    {
      cout << "Array Tag (Skip to omit)" << endl;
      if (query)
      {
	cin >> buf;
	cin.ignore(10000,'\n');
	msg << buf << Check;
	if (!strcmp("Skip",buf))
	  break;
      }
      else
      {
	msg << ((tagged *)elem)->tagname() << Check;
      }
    }
    case t_array:
    {
      cout << "Array" << endl;
      
      array *ar = (array *)elem;
      switch (ar->arr_type())
      {
      case arr_int:
	cout << " Ints" << endl;
	if (query)
	{
	  int c;
	  for (c=0; c<5; c++)
	  {
	    cout << "arr[" << c << "] = ";
	    cin >> q_i_ary[c];
	    cin.ignore(10000,'\n');
	  }
	  msg << SetSize(5) << q_i_ary << Check;
	}
	else
	{
	  msg << SetSize(10) << inta << Check;
	}
	break;
      case arr_float:
	cout << " Floats" << endl;
	if (query)
	{
	  int c;
	  for (c=0; c<5; c++)
	  {
	    cout << "arr[" << c << "] = ";
	    cin >> q_f_ary[c];
	    cin.ignore(10000,'\n');
	  }
	  msg << SetSize(5) << q_f_ary << Check;
	}
	else
	{
	  msg << SetSize(10) << floata << Check;
	}
	break;
      default:
	break;
      }
      break;
    }
    case t_string:
      cout << "String" << endl;
      if (query)
      {
	cin >> buf;
	cin.ignore(10000,'\n');
	msg << buf << Check;
      }
      else
      {
	msg << CAST str << Check;
      }
      break;
    default:
      break;
    }
    
    if (!msg)
    {
      cout << "Error appending message" << endl;
      return;
    }
  }
  
  cout << "Sending ";

  if (!msg_file)
  {
    
    cout << "to datagroup " << msg.Dest();
    
    if (!t.Send(msg,TRUE))
      cout << "Error - could not send the message" << endl;
    
    if (!t.Flush())
      cout << "Error - could not flush the messages" << endl;
  }
  else
  {
    (*mfp) << msg;
  }
  cout << endl;
  
}

