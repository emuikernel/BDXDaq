h08121
s 00015/00014/00347
d D 1.9 01/01/09 16:19:36 jhardie 10 9
c Fixed some const problems with SS
c 
e
s 00002/00002/00359
d D 1.8 99/01/06 19:21:27 jhardie 9 8
c Modified for verions of infoserver which
c mangle names, new ss groups, and timestamping.
e
s 00001/00001/00360
d D 1.7 98/02/05 16:23:42 jhardie 8 7
c Fixed tag names for tagged fields
c 
e
s 00016/00000/00345
d D 1.6 98/01/27 13:31:31 jhardie 7 6
c Added code for tagged arrays
e
s 00078/00014/00267
d D 1.5 98/01/27 13:05:03 jhardie 6 5
c Marking the working version before tagged input test
c 
e
s 00002/00002/00279
d D 1.4 97/06/06 11:33:05 jhardie 5 4
c made the defaults more reasonable
e
s 00006/00001/00275
d D 1.3 97/06/05 11:58:21 jhardie 4 3
c fixed some small problems with smartsockets arrays
e
s 00062/00019/00214
d D 1.2 97/06/05 10:40:45 jhardie 3 1
c Version 1.0 of info_server and support programs
e
s 00000/00000/00000
d R 1.2 97/06/02 18:36:17 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 3 2 info_server/server/info_in_test.cc
c Name history : 2 1 server/info_in_test.cc
c Name history : 1 0 s/server/info_in_test.cc
e
s 00233/00000/00000
d D 1.1 97/06/02 18:36:16 jhardie 1 0
c Updated files for new info_server
e
u
U
f e 0
t
T
I 1
//**********************************************************************
// info_in_test
// Program to generate input messages according to a configuration file
// and send them off to the info server.
//
// Programmed by: jgh
//**********************************************************************
I 10

E 10
#include "info_includes.hh"

#define RESERVE_SPACE
#include "info_classes.hh"
#include "info_globals.hh"
#undef RESERVE_SPACE


// Configuration related prototypes
D 10
int read_config(char *f);
E 10
I 10
int read_config(const char *f);
E 10
char *data_stream_status();

D 6
void build_input(TipcSrv &s, input *i);
E 6
I 6
void build_input(TipcSrv &s, input *i, int query);
E 6
int get_next_cmd();


#define INFO_SERV 13

I 3
// Command line arguments
extern "C" {
#include "procargs.h"
}

// Command line variables
D 5
char *app_name = "testing";
E 3
char *config_file = "testconfig.cfg";
E 5
I 5
D 10
char *app_name = "clasprod";
D 9
char *config_file = "/usr/local/clas/parms/info_server.cfg";
E 9
I 9
char *config_file = "/usr/local/clas/parms/info_server/info_server.cfg";
E 9
E 5
D 3
char *appname = "infotest";
E 3
I 3
char *unique_name = "info_server";
char *msg_file = 0;
E 10
I 10
const char *app_name = "clasprod";
const char *config_file = "/usr/local/clas/parms/info_server/info_server.cfg";
const char *unique_name = "info_server";
const char *msg_file = 0;
E 10
E 3

I 3
ARGTABLE
ARGENTRY("app",STRING,&app_name,"is the application name"),
ARGENTRY("msg",STRING,&msg_file,"is the SmartSockets message file"),
ARGENTRY("config",STRING,&config_file,"is the configuration file"),
ARGENTRY("uniq",STRING,&unique_name,"is the programs unique name"),
ARGENTRY("help",USAGE,NULL,NULL),
ENDTABLE

TipcMsgFile *mfp = 0;

E 3
int main(int argc, char **argv)
{
    int status;

I 3
    GETARGS(argc,argv);
    
E 3
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
D 10
    option = TutOptionLookup("application");
E 10
I 10
    option = TutOptionLookup(CAST "application");
E 10
    if (!option)
    {
      cout << "Error - could not get application name" << endl;
      return T_EXIT_FAILURE;
    }

D 3
    if (!TutOptionSetEnum(option, appname))
E 3
I 3
D 10
    if (!TutOptionSetEnum(option, app_name))
E 10
I 10
    if (!TutOptionSetEnum(option, CAST app_name))
E 10
E 3
    {
      cout << "Error - could not set application name" << endl;
      return T_EXIT_FAILURE;
    }
    
    // Create the info_server message type
D 10
    TipcMt mt("info_server", INFO_SERV, "verbose");
E 10
I 10
    TipcMt mt(CAST "info_server", INFO_SERV, CAST "verbose");
E 10
    if (!mt)
    {
      cout << "Error - could not create message type" << endl;
      return T_EXIT_FAILURE;
    }
    
D 3
    
    // Set the Smartsockets server connection.
    TipcSrv &ss_serv = TipcSrv::InstanceCreate(T_IPC_SRV_CONN_FULL);
    if (!ss_serv)
E 3
I 3

    if (!msg_file)
E 3
    {
D 3
      cout << "Error - could not connect to server" << endl;
      exit(T_EXIT_FAILURE);
    }
E 3
I 3
      
      // Set the Smartsockets server connection.
      TipcSrv &ss_serv = TipcSrv::InstanceCreate(T_IPC_SRV_CONN_FULL);
      if (!ss_serv)
      {
	cout << "Error - could not connect to server" << endl;
	exit(T_EXIT_FAILURE);
      }
E 3

D 3
    ss_serv.StdDgSetRecv(TRUE,FALSE);
E 3
I 3
      ss_serv.StdDgSetRecv(TRUE,FALSE);
I 4
      ss_serv.Flush();
      
E 4
    }
    else
    {
D 10
      mfp = new TipcMsgFile(msg_file,T_IPC_MSG_FILE_CREATE_WRITE);
E 10
I 10
      mfp = new TipcMsgFile(CAST msg_file,T_IPC_MSG_FILE_CREATE_WRITE);
E 10
    }
E 3
    
I 3
    
E 3
    // Main loop.  We want to print a message which asks which type
    // of input to construct and send out.
    int cmd;
I 6
    int query;
E 6
    while(cmd = get_next_cmd())
    {
I 6
      query = 0;
E 6
      input *i;
D 3
      ss_serv.MainLoop(0.0);
E 3
I 3
      if (!msg_file)
	ss_serv.MainLoop(0.0);
E 3
      if (cmd < 0)
D 6
	continue;
E 6
I 6
      {
	cmd = -cmd;
	query = 1;
      }
E 6
      
      inputlist.top();
      for (int j=0; j < cmd-1; j++)
	inputlist.next();
      i = (input *)inputlist.current();
D 6
      build_input(ss_serv,i);
E 6
I 6
      build_input(ss_serv,i,query);
E 6
D 3
      ss_serv.MainLoop(0.0);
E 3
I 3
      if (!msg_file)
	ss_serv.MainLoop(0.0);
E 3
    }
    
    if (!ss_serv.Destroy(T_IPC_SRV_CONN_NONE))
    {
      cout << "Error - could not fully destroy server" << endl;
    }
I 3
    if (mfp)
      delete mfp;
    
E 3
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
I 6
    cin.ignore(10000,'\n');
E 6
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
D 10
char *str = "ThisString";
E 10
I 10
const char *str = "ThisString";
E 10

D 6
void build_input(TipcSrv &t, input *i)
E 6
I 6
void build_input(TipcSrv &t, input *i, int query)
E 6
{
  lnode *elem;
  
  long int_val = 0;
  double flt_val = 0.0;
D 6
  
E 6
I 6
  long q_int;
  double q_flt;
  long q_i_ary[5];
  double q_f_ary[5];
  char buf[128];

E 6
  cout << "BuildInput entered" << endl;
D 3
  
E 3
I 3

E 3
D 10
  TipcMsg msg("info_server");
E 10
I 10
  TipcMsg msg(CAST "info_server");
E 10

D 3
  msg.Dest("info_server");
E 3
I 3
D 10
  msg.Dest(unique_name);
E 3
  msg.Sender("info_test");
E 10
I 10
  msg.Dest(CAST unique_name);
  msg.Sender(CAST "info_test");
E 10
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
D 6
      
      msg << int_val << Check;
      int_val++;
E 6
I 6
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
E 6
      break;
    case t_floating:
      cout << "Floating" << endl;
D 6
      
      msg << flt_val << Check;
      flt_val += 1.0;
E 6
I 6
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
E 6
      break;
I 7
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
D 8
	msg << str << Check;
E 8
I 8
D 9
	msg << elem->name() << Check;
E 9
I 9
	msg << ((tagged *)elem)->tagname() << Check;
E 9
E 8
      }
    }
E 7
    case t_array:
    {
      cout << "Array" << endl;
      
      array *ar = (array *)elem;
      switch (ar->arr_type())
      {
      case arr_int:
	cout << " Ints" << endl;
D 6
	msg << SetSize(10) << inta << Check;
E 6
I 6
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
E 6
	break;
      case arr_float:
	cout << " Floats" << endl;
D 6
	msg << SetSize(10) << floata << Check;
E 6
I 6
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
E 6
	break;
      default:
	break;
      }
      break;
    }
    case t_string:
      cout << "String" << endl;
D 6
      msg << str << Check;
E 6
I 6
      if (query)
      {
	cin >> buf;
	cin.ignore(10000,'\n');
	msg << buf << Check;
      }
      else
      {
D 10
	msg << str << Check;
E 10
I 10
	msg << CAST str << Check;
E 10
      }
E 6
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
  
D 4
  cout << "Sending!" << endl;
E 4
I 4
  cout << "Sending ";
E 4
D 3
  
  if (!t.Send(msg,TRUE))
    cout << "Error - could not send the message" << endl;
E 3

D 3
  if (!t.Flush())
    cout << "Error - could not flush the messages" << endl;
E 3
I 3
  if (!msg_file)
  {
    
I 4
    cout << "to datagroup " << msg.Dest();
    
E 4
    if (!t.Send(msg,TRUE))
      cout << "Error - could not send the message" << endl;
    
    if (!t.Flush())
      cout << "Error - could not flush the messages" << endl;
  }
  else
  {
    (*mfp) << msg;
  }
I 4
  cout << endl;
E 4
  
E 3
}

E 1
