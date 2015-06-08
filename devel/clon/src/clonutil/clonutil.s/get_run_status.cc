//  get_run_status
//
//  gets run status (integer) from run control server
//
//  debug....
//     rcserver dies when queried...temporarily implemented file
//       containing run status, written by rc_transition in run_xxx.csh scripts
//   ejw, 13-jun-97
//
//
//
// ejw, 8-apr-97



// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system 

using namespace std;
#include <strstream>

#include <fstream.h>    // debug...
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// for cdev
//#include <cdev.h>
//#include <cdevData.h>
//#include <cdevRequestObject.h>
//#include <cdevSystem.h>


// everything must be callable by c
extern "C" {


// for initialization
static int once_only=0;

// ref to cdev system object
//static cdevSystem &cdevsys = cdevSystem::defaultSystem ();

// pointer to status request object
//static cdevRequestObject *statusReq = NULL;


//--------------------------------------------------------------------------


int get_run_status(char *session){


  // debug...
  char line[200];
  char state[30];

  sprintf(line,"%s/run_log/rcstate_%s.txt",getenv("CLON_PARMS"),session);
  ifstream file(line);
  if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad()) return 0;
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



 // perform once-only initialization of cdev directory service
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




}

//--------------------------------------------------------------------------


// end of extern "C"
}
