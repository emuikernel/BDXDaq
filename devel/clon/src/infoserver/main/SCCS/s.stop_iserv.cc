h19572
s 00009/00009/00090
d D 1.6 01/01/09 16:18:53 jhardie 7 6
c Fixed some const problems with SS
c 
e
s 00001/00001/00098
d D 1.5 97/06/06 11:33:04 jhardie 6 5
c made the defaults more reasonable
e
s 00000/00000/00099
d D 1.4 97/06/05 11:58:25 jhardie 5 4
c fixed some small problems with smartsockets arrays
e
s 00003/00000/00096
d D 1.3 97/06/05 10:46:25 jhardie 4 3
c Added versioning variables for SCCS and added install: target for makefile
e
s 00000/00000/00096
d D 1.2 97/06/05 10:40:49 jhardie 3 1
c Version 1.0 of info_server and support programs
e
s 00000/00000/00000
d R 1.2 97/06/02 18:36:20 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 3 2 info_server/server/stop_iserv.cc
c Name history : 2 1 server/stop_iserv.cc
c Name history : 1 0 s/server/stop_iserv.cc
e
s 00096/00000/00000
d D 1.1 97/06/02 18:36:19 jhardie 1 0
c Updated files for new info_server
e
u
U
f e 0
t
T
I 1
//**********************************************************************
// stop_iserv
// Program to send a quit message to the info server unique datagroup.
//
I 4
// Version: %A% %I% on %D% at %T%
E 4
// Programmed by: jgh
//**********************************************************************
#include "info_includes.hh"

// Command line arguments
extern "C" {
#include "procargs.h"
}

// Command line variables
D 6
char *app_name = "testing";
E 6
I 6
D 7
char *app_name = "no_where";
E 6
char *config_file = NULL;
char *tcl_config_file = NULL;
E 7
I 7
const char *app_name = "no_where";
const char *config_file = NULL;
const char *tcl_config_file = NULL;
E 7
int debug_flag = 0;
int no_tcl = 0;
int monitor_evts = 0;
D 7
char *unique_name = "info_server";
E 7
I 7
const char *unique_name = "info_server";
E 7

ARGTABLE
ARGENTRY("app",STRING,&app_name,"is the application name"),
ARGENTRY("uniq",STRING,&unique_name,"is the programs unique name"),
ARGENTRY("help",USAGE,NULL,NULL),
ENDTABLE

int main(int argc, char **argv)
{
    int status;

I 4
    cout << "%A% %I% Starting" << endl;

E 4
    GETARGS(argc,argv);

    // Read the license file
    if (!TutCommandParseStdLicense())
    {
      cout << "Error - could not load standard license file" << endl;
      return T_EXIT_FAILURE;
    }

    // Set the application name
    T_OPTION option;
D 7
    option = TutOptionLookup("application");
E 7
I 7
    option = TutOptionLookup( CAST "application");
E 7
    if (!option)
    {
      cout << "Error - could not get application name" << endl;
      return T_EXIT_FAILURE;
    }

D 7
    if (!TutOptionSetEnum(option, app_name))
E 7
I 7
    if (!TutOptionSetEnum(option, CAST app_name))
E 7
    {
      cout << "Error - could not set application name" << endl;
      return T_EXIT_FAILURE;
    }
    
    // Set the Smartsockets server connection.
    TipcSrv &ss_serv = TipcSrv::InstanceCreate(T_IPC_SRV_CONN_FULL);
    if (!ss_serv)
    {
      cout << "Error - could not connect to server" << endl;
      exit(T_EXIT_FAILURE);
    }

    ss_serv.StdDgSetRecv(TRUE,FALSE);

    TipcMsg msg(T_MT_CONTROL);
    if (!msg)
    {
      cout << "Could not create the control message" << endl;
      return T_EXIT_FAILURE;
    }
    
D 7
    msg.Dest(unique_name);
    msg.Sender("GrimReaper");
E 7
I 7
    msg.Dest(CAST unique_name);
    msg.Sender( CAST "GrimReaper");
E 7
    
D 7
    msg << "quit force" << Check;
E 7
I 7
    msg <<  CAST "quit force" << Check;
E 7
    if (!msg)
    {
      cout << "Error - could not set up kill command" << endl;
    }
    
    ss_serv.Send(msg,TRUE);
    ss_serv.Flush();

    cout << "*\n*\n*\t Program " << unique_name 
	 << " has been killed\n*\n*\n" << endl;
    
    
    if (!ss_serv.Destroy(T_IPC_SRV_CONN_NONE))
    {
      cout << "Error - could not fully destroy server" << endl;
    }
    return T_EXIT_SUCCESS;
}

E 1
