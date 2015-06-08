// insert_msg
//
//  inserts message into ipc alarm system
//  assumes ipc set up properly already
//
//    returns 0 if ok, non-zero otherwise
//
//  note: 
//     domain holds msg source to avoid recursion in ipc2cmlog and cmlog2ipc
//     
//  ejw, 18-may-99


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system stuff
#include <time.h>
#include <stdlib.h>


// for smartsockets
#include <rtworks/cxxipc.hxx>



//----------------------------------------------------------------

extern "C" {

int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	       int severity, const char *msgstatus, int code, const char *text) {


  T_STR domain   = (char*) "insert_msg";
  T_STR user     = getenv("USER");
  T_INT4 msgtime = time(NULL);
  char host[50];
  

  // get host name
  host[0]=NULL;
  gethostname(host,50);


  // just in case
  if(strlen(host)==0)   strcpy(host,"unknown");
  if(name==NULL)     	name=(char*)"unknown";
  if(facility==NULL) 	facility=(char*)"unknown";
  if(process==NULL)  	process=(char*)"unknown";
  if(msgclass==NULL) 	msgclass=(char*)"unknown";
  if(msgstatus==NULL)	msgstatus=(char*)"unknown";
  if(text==NULL)     	text=(char*)"unknown";


  // get ref to server
  TipcSrv &server=TipcSrv::Instance();


  // form and send message
  TipcMsg msg((char*)"cmlog");
  msg.Dest((char*)"cmlog");
  msg.Sender((char*)name); 
  msg << (T_STR)domain << (T_STR)host << (T_STR)user << msgtime 
      << (T_STR)name << (T_STR)facility << (T_STR)process << (T_STR)msgclass 
      << (T_INT4) severity << (T_STR)msgstatus << (T_INT4)code << (T_STR)text;
  server.Send(msg);
  server.Flush();


  return(0);
}

}

//----------------------------------------------------------------------


