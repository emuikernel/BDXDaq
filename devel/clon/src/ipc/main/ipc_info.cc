/*  ipc_info
JAM
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





#define __EXTENSIONS__

// SmartSockets include file
#include <rtworks/cxxipc.hxx>

// C++ include files

using namespace std;
#include <strstream>

#include <iostream.h>
#include <iomanip.h>


//Enumeration of type of poll request to server
enum Calls{AppN, CliN, DtgN, CliDtg, DtgCli, DtgI};
Calls MsgCall;

char *app = "clastest";        // default application
char *client = "$";            // default client setting
char *dg = "$";	               // default datagroup setting

#define false 0
#define true  1
int SerAll = false;           // default server settings
int SerInfo = false; 

//Help line
const char *help = "\nusage:\n\n ipc_info [-a application] [-c client] [-d datagroup]\n ipc_info [-s]   Servers Available\n ipc_info [-i]   Server Information\n";


// Function Prototypes
void ComLine (int argc, char **argv);
void GetLicense();
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




// main:  Takes up to six arguments
//********************************************************************
int main (int argc, char **argv)  {


  // synch with c i/o
  ios::sync_with_stdio();


  ComLine(argc, argv);

  GetLicense();

  if (strcmp (app,"$") != 0)
    SetAppl();
  mon.IdentStr("ipc_info monitor");

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
      else if (strncasecmp (argv[i], "-s", 2) == 0)  {
        SerAll = true;
        i ++;
      }
      else if (strncasecmp (argv[i], "-i", 2) == 0) {
        SerInfo = true;
        i ++;
      }
      else if (argc>=(i+1)) {
	if (strncasecmp (argv[i], "-a", 2) == 0)  {
	  if((i+2)<=argc){
	    app = strdup (argv[i+1]);
	    i = i + 2;
	  } else {
	    app="$";
	    i = i + 1;
	  }
	}
	else if (strncasecmp (argv[i], "-c", 2) == 0)  {
	  if((i+2)<=argc) {
	    client = strdup (argv[i+1]);
	    i = i + 2;
	  } else {
	    client="$";
	    i = i + 1;
	  }
	}
	else if (strncasecmp (argv[i], "-d", 2) == 0)  {
	  if((i+2)<=argc){
	    dg = strdup (argv[i+1]);
	    i = i + 2;
	  } else {
	    dg="$";
	    i = i + 1;
	  }
	}
	else if (i == 1)  {
	  cerr << "Unknown command line arg: " << argv[i] << endl;
	  cerr << help << endl;
	  exit(1);
	}
      }
      else {
	cerr << "Unknown command line arg: " << argv[i] << endl;
	cerr << help << endl;
	exit(1);
      }
    }
  }
}
//********************************************************************



//********************************************************************
void GetLicense()  {
#ifdef SunOS
  strstream s;
  s << getenv("RTHOME") << "/standard/license.cm" << ends;
  if(!TutCommandParseFile(s.str())){
    cerr << "Unable to read in license.cm " << endl; 
    exit(1);
  }
#else
  strstream s;
  s << getenv("RTHOME") << "/standard/license.cm" << ends;
  if(!TutCommandParseFile(s.str())){
    cerr << "Unable to read in license.cm " << endl; 
    exit(1);
  }
#endif

}
//********************************************************************



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
  int i;

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
  for (i = 0; i < data_size; i++)   
    cout << "	(" << i+1 << ")  " <<  ConLic[i] << " "; 

//  _getsize(msg,&data_size);
  cout << endl << "  Extra Licenses checked out:  "<< endl;
//  for (i = 0; i < 0; i++)
//    cout << "   (" << i+1 << ")  " << ExtraLic[i] << " ";   

  _getsize(msg,&data_size);
  cout << endl << "  DataGroups Received:  " << endl;
  for (i = 0; i < data_size; i++)
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
    cout << "All existing" << endl;
    msg >> data1;
  }
  else 
    cout << app << endl;
  
  switch (MsgCall)  {

  case CliN:
    cout << "CLIENT:  All existing  (with id and user@node)" << endl;
    msg >> data1 >> data2;
    break;
  case DtgN:  
    cout << "DATAGROUP:  All existing"  << endl;
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
    cout << "	(" << setfill(' ') << setw(2) << i+1 << ")  " <<  data1 [i] << " "; 
    if ((MsgCall == CliN) || (SerAll == true))
      cout << setfill('-') << setw(40-strlen(data1[i])) << "> " << data2[i];
    cout << endl;
  }      
}
//********************************************************************














































