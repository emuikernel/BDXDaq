h13696
s 00004/00004/00076
d D 1.17 00/10/26 12:55:23 wolin 18 17
c const
e
s 00002/00002/00078
d D 1.16 00/10/26 12:51:01 wolin 17 16
c const
e
s 00000/00000/00080
d D 1.15 00/06/22 17:00:33 wolin 16 15
c char * problem
e
s 00009/00009/00071
d D 1.14 00/06/22 13:28:58 wolin 15 14
c char * problem
e
s 00007/00002/00073
d D 1.13 99/06/03 13:17:25 wolin 14 13
c Addional bombproofing
c 
e
s 00008/00002/00067
d D 1.12 99/06/03 12:47:59 wolin 13 12
c Using gethostbyname, other bombproofing
c 
e
s 00004/00000/00065
d D 1.11 99/06/03 12:43:28 wolin 12 11
c HOST env var not set in cron jobs
c 
e
s 00003/00003/00062
d D 1.10 99/06/03 12:28:35 wolin 11 10
c Using char* didn't help
c 
e
s 00033/00057/00032
d D 1.9 99/06/01 15:42:18 wolin 10 9
c Completely rewritten to use smartsockets instead of cmlog
c 
e
s 00003/00000/00086
d D 1.8 98/02/12 14:43:54 wolin 9 8
c Undid last change
c 
e
s 00000/00003/00086
d D 1.7 98/02/12 12:05:24 wolin 8 7
c Removed tag table call following Jie's instructions
c 
e
s 00007/00000/00082
d D 1.6 98/02/10 16:24:42 wolin 7 6
c Added session name in domain
c 
e
s 00003/00003/00079
d D 1.5 97/05/23 11:29:15 wolin 6 5
c Typo
e
s 00000/00000/00082
d D 1.4 97/05/23 11:27:45 wolin 5 4
c Now using text tag instead of message tag
e
s 00005/00004/00077
d D 1.3 97/04/24 15:21:18 wolin 4 3
c Added new status field, modified severity
e
s 00003/00006/00078
d D 1.2 97/03/27 16:16:16 wolin 3 1
c Added name arg
e
s 00000/00000/00000
d R 1.2 97/03/25 09:54:42 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libutil/s/insert_msg.cc
c Name history : 1 0 s/insert_msg.cc
e
s 00084/00000/00000
d D 1.1 97/03/25 09:54:41 wolin 1 0
c Inserts message into coda message logging system
e
u
U
f e 0
t
T
I 1
D 10
//
E 10
// insert_msg
//
D 6
//  inserts message into CODA message logging system
E 6
I 6
D 10
//  inserts message into CODA CMLOG system
E 6
//
// usage:
//
D 3
//    int insert_msg(char *facility, char *process, char *msgclass, char *severity, 
E 3
I 3
D 4
//    int insert_msg(char *name, char *facility, char *process, char *msgclass, char *severity, 
E 3
//                      int code, char *text);
E 4
I 4
//    int insert_msg(char *name, char *facility, char *process, char *msgclass, 
//                      int severity, char *status, int code, char *text);
E 10
I 10
//  inserts message into ipc alarm system
//  assumes ipc set up properly already
E 10
E 4
//
//    returns 0 if ok, non-zero otherwise
//
D 10
//
//  note:  function declared extern "C" to allow access from c and fortran
//
//
//  ejw, 14-mar-97
E 10
I 10
//  note: 
//     domain holds msg source to avoid recursion in ipc2cmlog and cmlog2ipc
//     
//  ejw, 18-may-99
E 10


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system stuff
D 10
#include <unistd.h>
E 10
I 10
#include <time.h>
#include <stdlib.h>
E 10


D 10
// for cmlog system
#include <cdevData.h>
#include <cmlogClient.h>
#include <cmlogUtil.h>
E 10
I 10
// for smartsockets
#include <rtworks/cxxipc.hxx>

E 10


//----------------------------------------------------------------

extern "C" {

D 3
int insert_msg(char *facility, char *process, char *msgclass, char *severity, 
E 3
I 3
D 4
int insert_msg(char *name, char *facility, char *process, char *msgclass, char *severity, 
E 3
		   int code, char *message){
E 4
I 4
D 10
int insert_msg(char *name, char *facility, char *process, char *msgclass, 
D 6
	       int severity, char *status, int code, char *message){
E 6
I 6
	       int severity, char *status, int code, char *text){
E 10
I 10
D 11
int insert_msg(T_STR name, T_STR facility, T_STR process, T_STR msgclass, 
	       T_INT4 severity, T_STR msgstatus, T_INT4 code, T_STR text) {
E 11
I 11
D 17
int insert_msg(char *name, char *facility, char *process, char *msgclass, 
	       int severity, char *msgstatus, int code, char *text) {
E 17
I 17
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	       int severity, const char *msgstatus, int code, const char *text) {
E 17
E 11


D 15
  T_STR domain   ="insert_msg";
E 15
I 15
  T_STR domain   = (char*) "insert_msg";
E 15
D 13
  T_STR host     = getenv("HOST");
E 13
  T_STR user     = getenv("USER");
  T_INT4 msgtime = time(NULL);
I 13
  char host[50];
  
E 13

I 13
  // get host name
  host[0]=NULL;
  gethostname(host,50);
E 13

I 12
D 13
  if(host==NULL)host="unknown";
E 13
I 13

  // just in case
D 14
  if(strlen(host)==0)strcpy(host,"unknown");
E 13
  if(user==NULL)user="unknown";
E 14
I 14
  if(strlen(host)==0)   strcpy(host,"unknown");
D 15
  if(name==NULL)     	name="unknown";
  if(facility==NULL) 	facility="unknown";
  if(process==NULL)  	process="unknown";
  if(msgclass==NULL) 	msgclass="unknown";
  if(msgstatus==NULL)	msgstatus="unknown";
  if(text==NULL)     	text="unknown";
E 15
I 15
  if(name==NULL)     	name=(char*)"unknown";
  if(facility==NULL) 	facility=(char*)"unknown";
  if(process==NULL)  	process=(char*)"unknown";
  if(msgclass==NULL) 	msgclass=(char*)"unknown";
  if(msgstatus==NULL)	msgstatus=(char*)"unknown";
  if(text==NULL)     	text=(char*)"unknown";
E 15
E 14


E 12
  // get ref to server
  TipcSrv &server=TipcSrv::Instance();
E 10
E 6
E 4

I 7
D 10
  char *domain;
E 10
E 7

I 10
  // form and send message
D 15
  TipcMsg msg("cmlog");
  msg.Dest("cmlog");
E 15
I 15
  TipcMsg msg((char*)"cmlog");
  msg.Dest((char*)"cmlog");
E 15
D 18
  msg.Sender(name); 
  msg << domain << host << user << msgtime 
      << name << facility << process << msgclass 
D 11
      << severity << msgstatus << code << text;
E 11
I 11
      << (T_INT4) severity << msgstatus << (T_INT4)code << text;
E 18
I 18
  msg.Sender((char*)name); 
  msg << (T_STR)domain << (T_STR)host << (T_STR)user << msgtime 
      << (T_STR)name << (T_STR)facility << (T_STR)process << (T_STR)msgclass 
      << (T_INT4) severity << (T_STR)msgstatus << (T_INT4)code << (T_STR)text;
E 18
E 11
  server.Send(msg);
  server.Flush();
E 10
I 7

D 10
  // session name stored in domain tag
  domain=getenv("DD_NAME");
  if(domain==NULL)domain="clasprod";

I 9
  // define standard tags
  cmlogUtil::setTags();
  
E 9
E 7
  // create data object
  cdevData data;

D 8
  // define standard tags
  cmlogUtil::setTags();

E 8
  // fill data object
I 7
  data.insert("domain",          domain);
E 7
  data.insert("facility",      facility);
D 3
  data.insert("host",    getenv("HOST"));
E 3
  data.insert("process",	process);
D 3
  data.insert("pid",           getpid());
  data.insert("user",    getenv("USER"));
E 3
  data.insert("class",         msgclass);
  data.insert("severity",      severity);
I 4
  data.insert("status",          status);
E 4
  data.insert("code",   	   code);
D 6
  data.insert("message",	message);
E 6
I 6
  data.insert("text",              text);
E 6

  // create client object and connect to client daemon
D 3
  cmlogClient client("insert_msg");
E 3
I 3
  cmlogClient client(name);
E 3
  if(client.connect()!=CMLOG_SUCCESS){
    return(1);
  }

  // post message
  if(client.postData(data)!=CMLOG_SUCCESS){
    client.disconnect();
    return(2);
  }

  // disconnect
  if(client.disconnect()!=CMLOG_SUCCESS){
    return(3);
  }
E 10

D 10
   return(0);
E 10
I 10
  return(0);
E 10
}

}

//----------------------------------------------------------------------


E 1
