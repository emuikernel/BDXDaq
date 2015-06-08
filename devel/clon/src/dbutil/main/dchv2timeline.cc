// dchv_timeline

// reads dchv output file and enters dchv info into timeline

// ejw, 26-apr-02



#define _POSIX_SOURCE 1
#define __EXTENSIONS__


//  for smartsockets
#include <rtworks/cxxipc.hxx>


// CLAS ipc
#include <clas_ipc_prototypes.h>


//  system
#include <fstream.h>
#include <iomanip.h>


// misc
static char *application   = (char*)"clastest";
static char *msql_database = (char*)"clasrun";
static char *session       = (char*)"clastest";
static char temp[1024];


// prototypes
void read_and_send_msg(char *filename);
extern "C"{
void decode_command_line(int argc, char **argv);
int get_run_number(char *msql_database, char *session);
}


// ref to server (connection created later)
TipcSrv &server=TipcSrv::Instance();


//---------------------------------------------------------------


int main(int argc, char **argv) {

  int run;
  int status;


  // synch with stdio
  ios::sync_with_stdio();


  // decode command line
  decode_command_line(argc,argv);


  // connect to ipc
  ipc_set_application(application);
  status=ipc_init("","dchv2timeline");
  if(status<0) {
    cerr << "\n?Unable to connect to server..."
	 << "probably duplicate unique id\n"
	 << "   ...check for another dchv2timeline connected to "
	 << application << " using ipc_info\n"
	 << "   ...only one such process allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }


  // get file name, read and send message
  run=get_run_number(msql_database,session);
  sprintf(temp,"%s/dchv/archive/dchv_%06d.txt",getenv("CLON_PARMS"),run);
  read_and_send_msg(temp);


  // done
  exit(EXIT_SUCCESS);
}


//------------------------------------------------------------------


void read_and_send_msg(char *filename) {

  int id[1], current[1], voltage[1];


  // open file and read data


  // create message
  TipcMsg msg((T_STR)"info_server");
  msg.Dest((T_STR)"info_server/in/dchv2timeline");


  // fill msg
  msg << (T_STR)"dchv2timeline" << (T_INT4*)id;


  // send and flush
  server.Send(msg,TRUE);
  server.Flush();


  return;
}


//-----------------------------------------------------------------------


void decode_command_line(int argc, char **argv) {

  const char *help ="\nusage:\n\n   dchv2timeline [-a application] [-m msql_database] [-s session]\n\n";
  int i=1;

  while(i<argc) {
    
    if(strncasecmp(argv[i],"-h",2)==0) {
      printf("%s", help);
      exit(EXIT_SUCCESS);
    } 
    else if (strncasecmp(argv[i],"-a",2)==0) {
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-m",2)==0) {
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0) {
      session=strdup(argv[i+1]);
      i=i+2;
    }
    else {
      printf("Unknown command line arg: %s\n",argv[i]);
      i=i+1;
    }
  }
}


//-------------------------------------------------------------------
