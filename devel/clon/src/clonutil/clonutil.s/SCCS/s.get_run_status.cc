h26580
s 00006/00006/00122
d D 1.13 07/11/23 19:10:50 boiarino 14 13
c remove cdev remanents
c 
e
s 00001/00004/00127
d D 1.12 07/10/12 09:50:11 boiarino 13 12
c *** empty log message ***
e
s 00006/00000/00125
d D 1.11 06/07/11 21:56:18 boiarino 12 11
c Linux port
e
s 00001/00002/00124
d D 1.10 01/04/02 14:58:29 wolin 11 10
c Bombproofed .eof()
e
s 00052/00052/00074
d D 1.9 01/02/09 16:43:38 wolin 10 9
c Switched back to rcstate file
e
s 00001/00001/00125
d D 1.8 01/01/03 10:50:07 wolin 9 8
c is_open()
e
s 00062/00062/00064
d D 1.7 00/06/22 13:31:06 wolin 8 7
c char * problem
e
s 00012/00009/00114
d D 1.6 98/05/21 10:21:02 wolin 7 6
c No cdev until rcserver stuff works
c 
e
s 00011/00012/00112
d D 1.5 98/05/21 10:15:20 wolin 6 5
c Still no rcserver
c 
e
s 00001/00001/00123
d D 1.4 98/01/22 11:59:19 wolin 5 4
c Env vars
c 
e
s 00067/00026/00057
d D 1.3 97/06/13 14:22:28 wolin 4 3
c Debug...just reads rcstate file until rcserver fixed
e
s 00006/00004/00077
d D 1.2 97/04/09 09:20:32 wolin 3 1
c Minor bugs
e
s 00000/00000/00000
d R 1.2 97/04/08 12:21:34 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libutil/s/get_run_status.cc
c Name history : 1 0 s/get_run_status.cc
e
s 00081/00000/00000
d D 1.1 97/04/08 12:21:33 wolin 1 0
c Returns run status (integer) from run control server
e
u
U
f e 0
t
T
I 1
//  get_run_status
//
//  gets run status (integer) from run control server
D 6
//  returns negative if can't talk to RCS
// 
E 6
I 4
//
//  debug....
//     rcserver dies when queried...temporarily implemented file
//       containing run status, written by rc_transition in run_xxx.csh scripts
//   ejw, 13-jun-97
//
//
//
E 4
// ejw, 8-apr-97



// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system 
I 12
D 13
#ifdef Linux
E 13
I 13

E 13
using namespace std;
#include <strstream>
D 13
#else
E 12
#include <strstream.h>
I 12
#endif
E 13

E 12
I 4
#include <fstream.h>    // debug...
I 7
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
E 7
E 4


D 7
// for cdev
#include <cdev.h>
#include <cdevData.h>
#include <cdevRequestObject.h>
#include <cdevSystem.h>
E 7
I 7
D 8
// // for cdev
// #include <cdev.h>
// #include <cdevData.h>
// #include <cdevRequestObject.h>
// #include <cdevSystem.h>
E 8
I 8
// for cdev
D 14
#include <cdev.h>
#include <cdevData.h>
#include <cdevRequestObject.h>
#include <cdevSystem.h>
E 14
I 14
//#include <cdev.h>
//#include <cdevData.h>
//#include <cdevRequestObject.h>
//#include <cdevSystem.h>
E 14
E 8
E 7


// everything must be callable by c
extern "C" {


I 3
// for initialization
static int once_only=0;

E 3
D 7
// ref to cdev system object
D 4
cdevSystem &cdevsys = cdevSystem::defaultSystem ();
E 4
I 4
static cdevSystem &cdevsys = cdevSystem::defaultSystem ();
E 7
I 7
D 8
// // ref to cdev system object
// static cdevSystem &cdevsys = cdevSystem::defaultSystem ();
E 8
I 8
// ref to cdev system object
D 14
static cdevSystem &cdevsys = cdevSystem::defaultSystem ();
E 14
I 14
//static cdevSystem &cdevsys = cdevSystem::defaultSystem ();
E 14
E 8
E 7
E 4

D 7
// pointer to status request object
static cdevRequestObject *statusReq = NULL;
E 7
I 7
D 8
// // pointer to status request object
// static cdevRequestObject *statusReq = NULL;
E 8
I 8
// pointer to status request object
D 14
static cdevRequestObject *statusReq = NULL;
E 14
I 14
//static cdevRequestObject *statusReq = NULL;
E 14
E 8
E 7


//--------------------------------------------------------------------------


int get_run_status(char *session){

D 3
  cdevData result,rcs,nsresult;
  strstream temp;
  int once_only=0;
E 3

D 3

E 3
D 4
  // perform once-only initialization of cdev directory service
  if(once_only==0){
    once_only=1;
I 3
    cdevData rcs,nsresult;
E 3
    cdevDevice& ns = cdevDevice::attachRef("cdevDirectory");
I 3
    strstream temp;
E 3
    temp << "RCS : " << session << ";" << ends;
    rcs.insert ("value",temp.str());
    if(ns.send("update", rcs, nsresult)!=CDEV_SUCCESS){ return(-1); }
E 4
I 4
  // debug...
  char line[200];
  char state[30];

D 5
  sprintf(line,"/usr/local/clas/parms/run_log/rcstate_%s.txt",session);
E 5
I 5
D 8
  sprintf(line,"%s/run_log/rcstate_%s.txt",getenv("CLON_PARMS"),session);
E 5
  ifstream file(line);
  if(file.bad())return 0;
  if(file.eof())return 0;
  file.getline(line,sizeof(line));
  sscanf(line,"%s",state);
D 6
  if(strcmp(state,"boot")==0){
E 6
I 6
  if(strcasecmp(state,"boot")==0){
E 6
    return 2;
D 6
  } else if(strcmp(state,"config")==0){
E 6
I 6
  } else if(strcasecmp(state,"config")==0){
E 6
    return 4;
D 6
  } else if(strcmp(state,"download")==0){
E 6
I 6
  } else if(strcasecmp(state,"download")==0){
E 6
    return 6;
D 6
  } else if(strcmp(state,"prestart")==0){
E 6
I 6
  } else if(strcasecmp(state,"prestart")==0){
E 6
    return 8;
D 6
  } else if(strcmp(state,"pause")==0){
E 6
I 6
  } else if(strcasecmp(state,"pause")==0){
E 6
    return 8;
D 6
  } else if(strcmp(state,"go")==0){
E 6
I 6
  } else if(strcasecmp(state,"go")==0){
E 6
    return 11;
D 6
  } else if(strcmp(state,"end")==0){
E 6
I 6
  } else if(strcasecmp(state,"end")==0){
E 6
    return 12;
  } else {
    return 0;
E 4
  }
E 8
I 8
D 10
//    sprintf(line,"%s/run_log/rcstate_%s.txt",getenv("CLON_PARMS"),session);
//    ifstream file(line);
D 9
//    if(file.bad())return 0;
E 9
I 9
//    if(!file.is_open())return 0;
E 9
//    if(file.eof())return 0;
//    file.getline(line,sizeof(line));
//    sscanf(line,"%s",state);
//    if(strcasecmp(state,"boot")==0){
//      return 2;
//    } else if(strcasecmp(state,"config")==0){
//      return 4;
//    } else if(strcasecmp(state,"download")==0){
//      return 6;
//    } else if(strcasecmp(state,"prestart")==0){
//      return 8;
//    } else if(strcasecmp(state,"pause")==0){
//      return 8;
//    } else if(strcasecmp(state,"go")==0){
//      return 11;
//    } else if(strcasecmp(state,"end")==0){
//      return 12;
//    } else {
//      return 0;
//    }
E 10
I 10
  sprintf(line,"%s/run_log/rcstate_%s.txt",getenv("CLON_PARMS"),session);
  ifstream file(line);
D 11
  if(!file.is_open())return 0;
  if(file.eof())return 0;
E 11
I 11
  if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad()) return 0;
E 11
  file.getline(line,sizeof(line));
  sscanf(line,"%s",state);
  if(strcasecmp(state,"boot")==0){
    return 2;
  } else if(strcasecmp(state,"config")==0){
    return 4;
  } else if(strcasecmp(state,"download")==0){
    return 6;
  } else if(strcasecmp(state,"prestart")==0){
    return 8;
  } else if(strcasecmp(state,"pause")==0){
    return 8;
  } else if(strcasecmp(state,"go")==0){
    return 11;
  } else if(strcasecmp(state,"end")==0){
    return 12;
  } else {
    return 0;
  }
E 10
E 8


D 4
  // try to create status request object if it doesn't exist
  if(statusReq==NULL){
    statusReq = cdevRequestObject::attachPtr(session,"get status");
  }
E 4

D 6

D 4
  // try to get run status if status request object exists
  if(statusReq==NULL) {
    return(-1);
  } else {
I 3
    cdevData result;
E 3
    statusReq->send(0,result);
    if(!statusReq) {
      return(-1);
    } else {
      return((int)result);
    }
  }
E 4

E 6
I 4
D 8
//   // perform once-only initialization of cdev directory service
//   if(once_only==0){
//     once_only=1;
//     cdevData rcs,nsresult;
//     cdevDevice& ns = cdevDevice::attachRef("cdevDirectory");
//     strstream temp;
//     temp << "RCS : " << session << ";" << ends;
D 6
//     rcs.insert ("value",temp.str());
E 6
I 6
//     rcs.insert ("file",temp.str());
E 6
//     if(ns.send("update", rcs, nsresult)!=CDEV_SUCCESS){ return(-1); }
//   }


//   // try to create status request object if it doesn't exist
//   if(statusReq==NULL){
//     statusReq = cdevRequestObject::attachPtr(session,"get status");
//   }


//   // try to get run status if status request object exists
//   if(statusReq==NULL) {
//     return(-1);
//   } else {
//     cdevData result;
//     statusReq->send(0,result);
//     if(!statusReq) {
//       return(-1);
//     } else {
//       return((int)result);
//     }
//  }
E 8
I 8
 // perform once-only initialization of cdev directory service
D 10
 if(once_only==0){
   once_only=1;
   cdevData rcs,nsresult;
   cdevDevice& ns = cdevDevice::attachRef((char*)"cdevDirectory");
   strstream temp;
   temp << "RCS : " << session << ";" << ends;
   rcs.insert ((char*)"file",temp.str());
   if(ns.send((char*)"update", rcs, nsresult)!=CDEV_SUCCESS){ return(-1); }
 }


 // try to create status request object if it doesn't exist
 if(statusReq==NULL){
   statusReq = cdevRequestObject::attachPtr(session,(char*)"get status");
 }


 // try to get run status if status request object exists
 if(statusReq==NULL) {
   return(-1);
 } else {
   cdevData result;
   statusReq->send(0,result);
   if(!statusReq) {
     return(-1);
   } else {
     return((int)result);
   }
}
E 10
I 10
//   if(once_only==0){
//     once_only=1;
//     cdevData rcs,nsresult;
//     cdevDevice& ns = cdevDevice::attachRef((char*)"cdevDirectory");
//     strstream temp;
//     temp << "RCS : " << session << ";" << ends;
//     rcs.insert ((char*)"file",temp.str());
//     if(ns.send((char*)"update", rcs, nsresult)!=CDEV_SUCCESS){ return(-1); }
//   }


//   // try to create status request object if it doesn't exist
//   if(statusReq==NULL){
//     statusReq = cdevRequestObject::attachPtr(session,(char*)"get status");
//   }


//   // try to get run status if status request object exists
//   if(statusReq==NULL) {
//     return(-1);
//   } else {
//     cdevData result;
//     statusReq->send(0,result);
//     if(!statusReq) {
//       return(-1);
//     } else {
//       return((int)result);
//     }
//  }
E 10
E 8

I 6



E 6
E 4
}

//--------------------------------------------------------------------------


// end of extern "C"
}
E 1
