h56591
s 00002/00002/00147
d D 1.2 02/06/12 13:26:57 wolin 3 1
c Still working on this, not sure it is needed
e
s 00000/00000/00000
d R 1.2 02/04/26 14:06:49 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 dbutil/s/dchv2timeline.cc
e
s 00149/00000/00000
d D 1.1 02/04/26 14:06:48 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
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
D 3
  sprintf(temp,"%s/ ??? %s.txt",getenv("CLON_PARMS"),run);
E 3
I 3
  sprintf(temp,"%s/dchv/archive/dchv_%06d.txt",getenv("CLON_PARMS"),run);
E 3
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
D 3
  msg << (T_STR)"dchv2timeline" << 10.0;
E 3
I 3
  msg << (T_STR)"dchv2timeline" << (T_INT4*)id;
E 3


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
E 1
