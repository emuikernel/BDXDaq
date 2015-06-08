//**********************************************************************
// stop_iserv
// Program to send a quit message to the info server unique datagroup.
//
// Version: @(#) stop_iserv.cc 1.6@(#) 1.6 on 05/02/25 at 16:25:44
// Programmed by: jgh
//**********************************************************************
#include "info_includes.hh"

// Command line arguments
extern "C" {
#include "procargs.h"
}

// Command line variables
const char *app_name = "no_where";
const char *config_file = NULL;
const char *tcl_config_file = NULL;
int debug_flag = 0;
int no_tcl = 0;
int monitor_evts = 0;
const char *unique_name = "info_server";

ARGTABLE
ARGENTRY("app",STRING,&app_name,"is the application name"),
ARGENTRY("uniq",STRING,&unique_name,"is the programs unique name"),
ARGENTRY("help",USAGE,NULL,NULL),
ENDTABLE

int main(int argc, char **argv)
{
    int status;

    cout << "@(#) stop_iserv.cc 1.6@(#) 1.6 Starting" << endl;

    GETARGS(argc,argv);

    // Read the license file
    if (!TutCommandParseStdLicense())
    {
      cout << "Error - could not load standard license file" << endl;
      return T_EXIT_FAILURE;
    }

    // Set the application name
    T_OPTION option;
    option = TutOptionLookup( CAST "application");
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
    
    msg.Dest(CAST unique_name);
    msg.Sender( CAST "GrimReaper");
    
    msg <<  CAST "quit force" << Check;
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

