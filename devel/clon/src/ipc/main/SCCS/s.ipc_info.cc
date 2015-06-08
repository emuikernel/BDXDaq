h02042
s 00014/00001/00591
d D 1.16 07/10/11 21:46:18 boiarino 18 17
c *** empty log message ***
e
s 00004/00004/00588
d D 1.15 06/11/27 01:16:09 boiarino 17 16
c cosmetic
c 
e
s 00017/00000/00575
d D 1.14 02/08/16 10:15:59 wolin 16 15
c license.cm
e
s 00000/00014/00575
d D 1.13 02/08/13 13:27:46 wolin 15 14
c Minor
e
s 00004/00004/00585
d D 1.12 02/08/13 13:24:47 wolin 14 13
c Removed call to TutCommandParseStdLicense()
e
s 00008/00004/00581
d D 1.11 00/10/05 15:13:33 wolin 13 12
c New compiler
e
s 00002/00004/00583
d D 1.10 00/08/30 14:33:02 wolin 12 11
c New CC
e
s 00005/00000/00582
d D 1.9 98/05/11 15:20:36 wolin 11 10
c Added sync with stdio
c 
e
s 00001/00001/00581
d D 1.8 98/03/04 15:25:44 wolin 10 9
c Minor mods
c 
e
s 00001/00001/00581
d D 1.7 98/02/27 13:09:52 wolin 9 8
c More room for process name
c 
e
s 00023/00012/00559
d D 1.6 97/03/18 12:25:08 wolin 8 7
c Minor bombproofing of cmd line args
e
s 00001/00001/00570
d D 1.5 96/08/28 10:33:50 muguira 7 6
c JAM - showing off
e
s 00023/00016/00548
d D 1.4 96/08/09 16:47:53 aswin 6 5
c Fixed command line argument function against core dump.
e
s 00002/00001/00562
d D 1.3 96/08/01 11:08:19 muguira 5 3
i 4
c Merged changes between child workspace "/usr/local/clas/work_src/ipc_info" and
c  parent workspace "/usr/local/clas/src/ipc_info".
c 
e
s 00001/00001/00561
d D 1.1.1.1 96/08/01 11:03:44 muguira 4 1
c JAM - fixes to build in sun code mgmt system. This is Aswin's program.
e
s 00002/00000/00562
d D 1.2 96/07/26 12:50:22 wolin 3 1
c Added __EXTENSIONS__
e
s 00000/00000/00000
d R 1.2 96/07/24 16:42:41 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 ipcutil/s/ipc_info.cc
c Name history : 1 0 s/ipc_info.cc
e
s 00562/00000/00000
d D 1.1 96/07/24 16:42:40 muguira 1 0
c 
e
u
U
f e 0
t
T
I 1
/*  ipc_info
D 7

E 7
I 7
JAM
E 7
    Gets information on server, applications, clients, and/or datagroups.

Ussage:
(1) To look up server information:

    ipc_info -s   :  Displays all available servers
    ipc_info -i   :  Polls general info on server    


(2) To look up application information:
    ipc_info -a $   
      :Displays all available applications

    ipc_info [-a application]  
      :Displays available clients on application

    ipc_info [-a application] [-c client]   
      :Displays datagroups received by client

    ipc_info [-a application] [-d datagroup]
      :Displays clients receiving datagroup

    ipc_info [-a application] [-c client] [-d datagroup]
      :Displays client datagroup information

    ipc_info [-a application] -c ALL
      :Displays all available clients
    
    ipc_info [-a application] -d ALL
      :Displays all available datagroups

Defaults:
    application =  "clastest"
    client      =  "$"
    datagroup   =  "$"
    Poll for available servers and general info on server options
      are turned off.

Note:
    $ stands for wildcard    



aj, 6-18-96

*/



D 4
#define _POSIX_SOURCE 1
I 3
#define __EXTENSIONS__
E 3
E 4
I 4
D 5
// define _POSIX_SOURCE 1
E 5
E 4

I 3

I 5
#define __EXTENSIONS__

E 5
E 3
// SmartSockets include file
#include <rtworks/cxxipc.hxx>

// C++ include files
I 16
D 18
#include <strstream.h>
E 18
I 18

using namespace std;
#include <strstream>

E 18
E 16
#include <iostream.h>
#include <iomanip.h>

D 12
//Boolean definition
typedef int bool;   const bool true = 1;    const bool false = 0;   
E 12

//Enumeration of type of poll request to server
enum Calls{AppN, CliN, DtgN, CliDtg, DtgCli, DtgI};
Calls MsgCall;

char *app = "clastest";        // default application
char *client = "$";            // default client setting
char *dg = "$";	               // default datagroup setting
D 13
bool SerAll = false;           // default server settings
bool SerInfo = false; 
E 13
I 13

#define false 0
#define true  1
int SerAll = false;           // default server settings
int SerInfo = false; 
E 13

//Help line
D 12
char *help = "\nusage:\n\n ipc_info [-a application] [-c client] [-d datagroup]\n ipc_info [-s]   Servers Available\n ipc_info [-i]   Server Information\n";
E 12
I 12
const char *help = "\nusage:\n\n ipc_info [-a application] [-c client] [-d datagroup]\n ipc_info [-s]   Servers Available\n ipc_info [-i]   Server Information\n";
E 12


// Function Prototypes
void ComLine (int argc, char **argv);
I 16
void GetLicense();
E 16
D 15
void GetLicense();
E 15
void SetAppl();
void ProcessCom(); 
void ProcessMsg();
void PrintMsg(T_STR *data1, T_STR *data2);
void ProcessSerMsg ();
void ProcessSerCom ();
void ProcessDtgMsg(); 
void ProcessCliMsg();

//Globals
TipcMt mt();
TipcMsg msg;
TipcMsg msg2;
TipcMon mon;
TipcMonServer server_mon (T_IPC_MON_ALL);
TipcMonClient mon_client (client), mon_client2 (client);




D 8



E 8
// main:  Takes up to six arguments
//********************************************************************
int main (int argc, char **argv)  {

I 11

  // synch with c i/o
  ios::sync_with_stdio();


E 11
  ComLine(argc, argv);

I 16
  GetLicense();

E 16
D 15
  GetLicense();

E 15
  if (strcmp (app,"$") != 0)
    SetAppl();
D 8
  TutCommandParseStr("setopt unique_datagroup IPC_Info");
  mon.IdentStr("IPC_Info Monitor");
E 8
I 8
  mon.IdentStr("ipc_info monitor");
E 8

// Connect to server
  TipcSrv& server = TipcSrv::InstanceCreate (T_IPC_SRV_CONN_FULL);

  if ((SerAll == true)||(SerInfo == true))
    ProcessSerCom();     // Processes server option if invoked
  else 
    ProcessCom();        // Process application options

// Receive poll msg from server
  server.Flush();
  msg = server.Next(10.0);
  if (!msg)  {
    cout << "Message not received.  "<< endl;
    exit(EXIT_FAILURE);
  }

  if (MsgCall == DtgCli) {
    mon_client2 = client;
    mon_client2.MsgTrafficPoll();
    if (!mon_client)  {
      cout << "Poll on datagroups received by client failed.  " << endl;
      exit (EXIT_FAILURE);
    }
    msg2 = server.Next(10.0);
    if (!msg2)  {
      cout << "Message not received.  " << endl;
      exit (EXIT_FAILURE);
   }  
  }

  cout << endl << endl << "=====================================================================" << endl;

//Process msg received from server
  if ((SerAll == true) || (SerInfo == true))
    ProcessSerMsg ();
  else
    ProcessMsg ();

  cout << endl << "=====================================================================" << endl;

 
// Destroy and clean up
  msg.Destroy();
  msg2.Destroy();
  server.Destroy(T_IPC_SRV_CONN_NONE);

}  // main
//********************************************************************


// ComLine:  Determines program execution given specified arguements
//********************************************************************
void ComLine (int argc, char **argv)  {  
  if (argc>7) {
    cout << help << endl;
    exit (0);
  }
  if (argc > 1)  {
    int i = 1;
    while (i < argc)  {
      if (strncasecmp (argv[i], "-h", 2) == 0)  {
        cout << help << endl;
        exit (0);
      }
      if (strncasecmp (argv[i], "-", 1) != 0)  {
        cerr << "Unknown command line arg: " << argv[i] << endl;
        cerr << help << endl;
        exit(1);
      }
D 6
      else if (strncasecmp (argv[i], "-a", 2) == 0)  {
        app = strdup (argv[i+1]);
        i = i + 2;
      }
      else if (strncasecmp (argv[i], "-c", 2) == 0)  {
        client = strdup (argv[i+1]);
        i = i + 2;
      }
      else if (strncasecmp (argv[i], "-d", 2) == 0)  {
        dg = strdup (argv[i+1]);
  	i = i + 2;
      }
E 6
      else if (strncasecmp (argv[i], "-s", 2) == 0)  {
        SerAll = true;
        i ++;
      }
      else if (strncasecmp (argv[i], "-i", 2) == 0) {
        SerInfo = true;
        i ++;
      }
D 6
      else if (i == 1)  {
        cerr << "Unknown command line arg: " << argv[i] << endl;
        cerr << help << endl;
        exit(1);
E 6
I 6
D 8
      else if (argc!=(i+1)) {
E 8
I 8
      else if (argc>=(i+1)) {
E 8
	if (strncasecmp (argv[i], "-a", 2) == 0)  {
D 8
	  app = strdup (argv[i+1]);
	  i = i + 2;
E 8
I 8
	  if((i+2)<=argc){
	    app = strdup (argv[i+1]);
	    i = i + 2;
	  } else {
	    app="$";
	    i = i + 1;
	  }
E 8
	}
	else if (strncasecmp (argv[i], "-c", 2) == 0)  {
D 8
	  client = strdup (argv[i+1]);
	  i = i + 2;
E 8
I 8
	  if((i+2)<=argc) {
	    client = strdup (argv[i+1]);
	    i = i + 2;
	  } else {
	    client="$";
	    i = i + 1;
	  }
E 8
	}
	else if (strncasecmp (argv[i], "-d", 2) == 0)  {
D 8
	  dg = strdup (argv[i+1]);
	  i = i + 2;
E 8
I 8
	  if((i+2)<=argc){
	    dg = strdup (argv[i+1]);
	    i = i + 2;
	  } else {
	    dg="$";
	    i = i + 1;
	  }
E 8
	}
	else if (i == 1)  {
	  cerr << "Unknown command line arg: " << argv[i] << endl;
	  cerr << help << endl;
	  exit(1);
	}
E 6
      }
I 6
      else {
	cerr << "Unknown command line arg: " << argv[i] << endl;
	cerr << help << endl;
	exit(1);
      }
E 6
    }
  }
}
I 16
//********************************************************************



//********************************************************************
void GetLicense()  {
I 18
#ifdef SunOS
E 18
  strstream s;
  s << getenv("RTHOME") << "/standard/license.cm" << ends;
  if(!TutCommandParseFile(s.str())){
    cerr << "Unable to read in license.cm " << endl; 
    exit(1);
  }
I 18
#else
  strstream s;
  s << getenv("RTHOME") << "/standard/license.cm" << ends;
  if(!TutCommandParseFile(s.str())){
    cerr << "Unable to read in license.cm " << endl; 
    exit(1);
  }
#endif

E 18
}
E 16
//********************************************************************
D 15



//********************************************************************
void GetLicense()  {
D 14
  if(!TutCommandParseStdLicense()){
    cerr << "Unable to read in License. " << endl; 
    exit(1);
  }
E 14
I 14
  //  if(!TutCommandParseStdLicense()){
  //    cerr << "Unable to read in License. " << endl; 
  //    exit(1);
  //  }
E 14
}
//********************************************************************
E 15



//SetAppl:  Set current application or set to default if not specified.
//********************************************************************
void SetAppl()  {
    T_OPTION opt = TutOptionLookup ("Application");
    if (!TutOptionSetEnum (opt, app))  {
      cout << "Unable to set application \n";
      exit (1);
    }
}
//********************************************************************



//********************************************************************
void ProcessSerCom ()  {
  if (SerAll == true)
    mon.ServerNamesPoll();
  else 
    server_mon.GeneralPoll();
  return;
}
//********************************************************************



//********************************************************************
void ProcessCom()  {

  if (strcmp(app, "$") == 0)  {
    MsgCall = AppN;
    mon.AppNamesPoll();
    if (!mon)  {
      cout << "Poll on applications failed.  " << endl;
      exit (EXIT_FAILURE);
    }
    return;
  }
  else if ((strcmp(dg, "ALL") == 0) && (strcmp(client,"$") == 0))  {
    MsgCall = DtgN;
    mon.DgNamesPoll();
    if (!mon)  {
      cout << "Poll on existing datagroups failed.  "  << endl;
      exit(EXIT_FAILURE);
    }
    return;
  }
  else if ((strcmp(dg, "$") == 0) && (strcmp(client, "ALL") == 0))  {
    MsgCall = CliN; 
    mon.ClientNamesPoll ();
    if (!mon)  {
      cout << "Poll on existing clients failed.  " << endl;
      exit (EXIT_FAILURE);
    }
    return;
  }
  else if ((strcmp(dg, "$") != 0) && (strcmp(client,"$") != 0))  {
    MsgCall = DtgI;
    mon_client = client;
    mon_client.DgPoll(dg);
    if (!mon_client)  {
      cout << "Poll on RTclient datagroup infomation failed." << endl;
      exit (EXIT_FAILURE);
    }

    return;
  }
  else if ((strcmp(dg, "$") != 0) && (strcmp(client,"$") == 0))  {
    MsgCall = CliDtg;
    mon.DgRecvPoll(dg);
    if (!mon)  {
      cout << "Poll on clients receiving datagroup failed.  " << endl;
      exit(EXIT_FAILURE);
    }
    return; 
  }
  else if ((strcmp(dg, "$") == 0) && (strcmp(client, "$") != 0)) {
    MsgCall = DtgCli;  
    mon_client = client; 
    mon_client.GeneralPoll();
    if (!mon_client)  {
      cout << "Poll on datagroups received by client failed.  " << endl;
      exit (EXIT_FAILURE);
    }
    return;
  }
  else {
    MsgCall = CliN; 
    mon.ClientNamesPoll ();
    if (!mon)  {
      cout << "Poll on existing clients failed.  " << endl;
      exit (EXIT_FAILURE);
    }
    return;
  }     

}
//********************************************************************


//********************************************************************
void ProcessSerMsg ()  {
  T_STR *data1, *data2;
  if (SerAll == true)  {
    cout << "AVAILABLE SERVERS:  " << endl << endl;
    msg >> data1 >> data2;
    PrintMsg(data1, data2);
  }
  else {
    T_STR ser_name, mon_id, node_name, user_name, arch, startup_com_file;
    T_INT4 pid, sbrk, d_sbrk, no_dae, cmd_mod, size;
    T_INT2 int_num_for, real_num_for;
    T_STR *clients;
 
    msg >> ser_name >> mon_id >> node_name >> user_name;
    msg >> pid >> arch >> sbrk >> d_sbrk >> int_num_for >> real_num_for;
    msg >> startup_com_file >> no_dae >> cmd_mod >> clients;

    cout << "  SERVER INFORMATION:         " << endl << endl;
    cout << "  RTserver Process:           " << ser_name << endl;
    cout << "  Process Monitoring ID:      " << mon_id << endl;
    cout << "  Node Name Running on:       " << node_name << endl;
    cout << "  User Name Running on:       " << user_name << endl;
    cout << "  PID:                        " << pid << endl;
    cout << "  Architecture:               " << arch << endl;
    cout << "  sbrk Value:                 " << sbrk << endl;
    cout << "  Change in sbrk Value:       " << d_sbrk << endl;
    cout << "  Integer Number Format:      " << int_num_for << endl;
    cout << "  Real Number Format:         " << real_num_for << endl;
    cout << "  Startup Command File:       " << startup_com_file << endl;
    cout << "  -no_daemon status:          " << no_dae << endl;
    cout << "  -cmd_mode status:           " << cmd_mod << endl << endl;
    cout << "  Clients in the Same Appl as Polling Client: " << endl;
     
     _getsize(msg,&size);
    for (int i = 0; i < size; i++) 
      cout << "      (" << i+1 << ")  " << clients[i] << endl;  

  }
}
//********************************************************************


//ProcessCliMsg:  Prints information on client specified
//********************************************************************
void ProcessCliMsg()  {
  T_STR RtClient, MonId, NodName, UName, Appl, RtServer, Arch, LogName;
  T_INT4 Pid, sbrk, d_sbrk, data_size;
  T_INT2 int_num_for, real_num_for;
  T_INT4 *ConLic;
  T_STR *ExtraLic, *DtgNames;
I 13
  int i;
E 13

  msg >> RtClient >> MonId >> NodName >> UName >> Pid >> Appl >> RtServer; 
  msg >> Arch >> sbrk >> d_sbrk >> int_num_for;
  msg >> real_num_for >> LogName >> ConLic >> ExtraLic >> DtgNames;

  if (!msg)  {
	cout << "Message fields in incorrect format to process." << endl;
  	exit (1);	
  }

  cout << endl << endl;
  cout << "  Client:                       " << RtClient << endl;
  cout << "  Process Monitoring ID:        " << MonId << endl;
  cout << "  Node Name:                    " << NodName << endl;
  cout << "  User Name:                    " << UName << endl;
  cout << "  PID:                          " << Pid << endl;
  cout << "  Application:                  " << Appl << endl;
  cout << "  RTserver process:             " << RtServer << endl;
  cout << "  Architecture:                 " << Arch << endl;
  cout << "  sbrk Value:                   " << sbrk << endl;
  cout << "  Change in sbrk Value:         " << d_sbrk << endl;
  cout << "  Integer Number Format:        " << int_num_for << endl;
  cout << "  Real Number Format:           " << real_num_for << endl;
  cout << "  Logical Connection Name:      " << LogName << endl;
  cout << endl << "  Counted Licenses checked out: "<< endl;
  _getsize(msg,&data_size);
D 13
  for (int i = 0; i < data_size; i++)   
E 13
I 13
  for (i = 0; i < data_size; i++)   
E 13
    cout << "	(" << i+1 << ")  " <<  ConLic[i] << " "; 

//  _getsize(msg,&data_size);
  cout << endl << "  Extra Licenses checked out:  "<< endl;
//  for (i = 0; i < 0; i++)
//    cout << "   (" << i+1 << ")  " << ExtraLic[i] << " ";   

  _getsize(msg,&data_size);
  cout << endl << "  DataGroups Received:  " << endl;
D 12
  for (i = 0; i < data_size; i++)
E 12
I 12
D 13
  for (int i = 0; i < data_size; i++)
E 13
I 13
  for (i = 0; i < data_size; i++)
E 13
E 12
   cout << "    (" << i+1 << ")  " << DtgNames[i] << " "; 

  T_INT4 MsgS, MsgR, ByS, ByR;

  cout << endl << "---------------------------------------------------------------------" << endl;

  msg2 >> RtClient >> MsgS >> MsgR >> ByS >> ByR;
  cout << "  Trafic Information:  " << endl << endl;
  cout << "     Messages Sent:     " << MsgS << endl;
  cout << "     Messages Received: " << MsgR << endl;
  cout << "     Bytes Sent:        " << ByS  << endl;
  cout << "     Bytes Received:    " << ByR  << endl;
}



//ProcessDtgMsg:  Prints infomation on datagroup specified
//********************************************************************
void ProcessDtgMsg()  {
  T_STR RtClient, DtgName;
  T_INT4 NumMesS, NumMesR, NumBytS, NumBytR;

  msg >> RtClient >> DtgName >> NumMesS >> NumMesR >> NumBytS >> NumBytR;

  cout << endl;
  cout << "  RTclient Process:             " << RtClient << endl;
  cout << "  Name of Datagroup:            " << DtgName << endl;
  cout << "  Number of Messages Sent:      " << NumMesS << endl;
  cout << "  Number of Messages Received:  " << NumMesR << endl;
  cout << "  Bytes Sent:                   " << NumBytS << endl;
  cout << "  Bytes Received:               " << NumBytR << endl << endl;
}
//*******************************************************************


// ProcessMsg:  Get info on msg received and print
//********************************************************************
void ProcessMsg()  {
  T_STR *data1, *data2;

  cout << "APPLICATION:  ";

  if (MsgCall == AppN)  {
D 17
    cout << "All exisiting" << endl;
E 17
I 17
    cout << "All existing" << endl;
E 17
    msg >> data1;
  }
  else 
    cout << app << endl;
  
  switch (MsgCall)  {

  case CliN:
D 17
    cout << "CLIENT:  All exisiting  (with id and user@node)" << endl;
E 17
I 17
    cout << "CLIENT:  All existing  (with id and user@node)" << endl;
E 17
    msg >> data1 >> data2;
    break;
  case DtgN:  
D 17
    cout << "DATAGROUP:  All exisiting"  << endl;
E 17
I 17
    cout << "DATAGROUP:  All existing"  << endl;
E 17
    msg >> data1;
    break;
  case CliDtg:
    cout << "DATAGROUP:  "<< dg << endl << "Clients receiving this datagroup:  " << endl;
    msg.Current(1);
    msg >> data1;
    break;
  case DtgCli:
    ProcessCliMsg (); 
    return;
  case DtgI:
    cout << endl << "Datagroup Information:  " << endl << endl;
    ProcessDtgMsg ();
    return;
  case AppN:
    break;
  }

  if (!msg)  {
    cout << "Message read in failed.  " << endl;
    exit (EXIT_FAILURE);
  }

  PrintMsg (data1, data2);
  return;
}  
//********************************************************************


// PrintMsg:  Printing subroutine 
//********************************************************************
void PrintMsg(T_STR *data1, T_STR *data2)  {  
  T_INT4 data_size;

  _getsize(msg,&data_size);
  cout << endl;
  for (int i = 0; i < data_size; i++) {  
D 10
    cout << "	(" << i+1 << ")  " <<  data1 [i] << " "; 
E 10
I 10
    cout << "	(" << setfill(' ') << setw(2) << i+1 << ")  " <<  data1 [i] << " "; 
E 10
    if ((MsgCall == CliN) || (SerAll == true))
D 9
      cout << setfill('-') << setw(20-strlen(data1[i])) << "> " << data2[i];
E 9
I 9
D 17
      cout << setfill('-') << setw(30-strlen(data1[i])) << "> " << data2[i];
E 17
I 17
      cout << setfill('-') << setw(40-strlen(data1[i])) << "> " << data2[i];
E 17
E 9
    cout << endl;
  }      
}
//********************************************************************














































E 1
