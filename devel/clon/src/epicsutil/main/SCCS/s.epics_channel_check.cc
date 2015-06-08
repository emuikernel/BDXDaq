h64712
s 00003/00001/00145
d D 1.3 07/10/12 09:57:13 boiarino 4 3
c *** empty log message ***
e
s 00001/00001/00145
d D 1.2 99/05/05 13:08:11 wolin 3 1
c Increased timeout to 5 sec
c 
e
s 00000/00000/00000
d R 1.2 97/12/23 11:36:31 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 epicsutil/s/epics_channel_check.cc
e
s 00146/00000/00000
d D 1.1 97/12/23 11:36:30 wolin 1 0
c date and time created 97/12/23 11:36:30 by wolin
e
u
U
f e 0
t
T
I 1
//  epics_check

//  checks if epics channel is alive

//  ejw, 23-dec-97



// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system stuff
#include <iostream.h>
#include <fstream.h>
D 4
#include <strstream.h>
E 4

I 4
using namespace std;
#include <strstream>
E 4

I 4

E 4
// for cdev
#include <cdev.h>
#include <cdevData.h>
#include <cdevDevice.h>
#include <cdevRequestObject.h>
#include <cdevSystem.h>


// misc variables
D 3
int cdev_pend_time  = 2;
E 3
I 3
int cdev_pend_time  = 5;
E 3
int debug           = 0;
int ncallback       = 0;
float epics_val;


// prototypes
void decode_command_line(int argc, char **argv);
void epics_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
			    cdevData& result);


// ref to cdev system object
cdevSystem &cdevsys = cdevSystem::defaultSystem ();



//--------------------------------------------------------------------------


main(int argc,char **argv){

  int status;
  char *channel=argv[argc-1];
  cdevGroup group;


  // decode command line...flags may be overridden in Tcl startup script
  decode_command_line(argc,argv);


  // only print cdev error messages
  cdevsys.setThreshold(CDEV_SEVERITY_ERROR);
  

  // create request object
  cdevRequestObject *obj = cdevRequestObject::attachPtr(channel,"get");


  // check channel if object exists
  if(obj!=NULL) {

    // create callback
    cdevCallback cb(epics_callback_func,(void*)0);
    
    
    // process group
    group.start();
    obj->sendCallback(NULL,cb);
    group.pend((double)cdev_pend_time);
    
  
    // check if callback received
    if(ncallback>0) {
      if(debug==1) { cout << "\nChannel " << channel << " value is " << epics_val << endl << endl;}
      exit(EXIT_SUCCESS);
    } else {
      if(debug==1) { cout << "\n?no response from " << channel << " in "
			  << cdev_pend_time << " seconds" << endl << endl;}
      exit(EXIT_FAILURE);
    }

  } else {
    if(debug==1) { cout << "\n?unable to create request object" << endl << endl;}
    exit(EXIT_FAILURE);
  }
  
}


//---------------------------------------------------------------------------


void epics_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
			 cdevData& result){
  
  ncallback++;
  epics_val = (float) result;
  
  return;
}


//---------------------------------------------------------------------------


void decode_command_line(int argc, char**argv) {

  char *help = "\nusage:\n\n  epics_check [-debug] [-p cdev_pend_time] channel_name\n";


  // loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0){
      cout << help << endl;
      exit(EXIT_SUCCESS);

    } else if (strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
      i=i+1;

    } else if (strncasecmp(argv[i],"-p",2)==0) {
      cdev_pend_time=atoi(argv[i+1]);
      i=i+2;

    } else {
      i++;
    }
  }

  return;
}

  
//----------------------------------------------------------------


E 1
