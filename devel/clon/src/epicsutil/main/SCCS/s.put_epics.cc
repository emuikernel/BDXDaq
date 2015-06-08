h54980
s 00003/00003/00209
d D 1.7 01/04/02 15:03:51 wolin 8 7
c Bombproofed .eof(), new compiler
e
s 00001/00001/00211
d D 1.6 01/01/03 10:45:40 wolin 7 6
c is_open()
e
s 00016/00007/00196
d D 1.5 99/11/10 13:03:07 wolin 6 5
c Now links...not sure what I was working on way back then
e
s 00004/00001/00199
d D 1.4 98/06/03 11:37:26 wolin 5 4
c Bug in cmd-line mode
c 
e
s 00004/00004/00196
d D 1.3 98/06/03 11:22:44 wolin 4 3
c val now float
c 
e
s 00037/00019/00163
d D 1.2 98/04/30 11:56:18 wolin 3 1
c Added caput mode
c 
e
s 00000/00000/00000
d R 1.2 97/05/09 16:38:31 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 epicsutil/s/put_epics.cc
c Name history : 1 0 s/put_epics.cc
e
s 00182/00000/00000
d D 1.1 97/05/09 16:38:30 wolin 1 0
c Get/put epics data using channel list in file
e
u
U
f e 0
t
T
I 1
//
//  put_epics
//
//  Reads file of epics channel names and values, then puts data
I 3
//  If file doesn't exist uses file as channel name
E 3
//
I 3
//
E 3
//  File format
//  -----------
//    # first non-blank char is a comment line
//    B_xxx.DVI      value
//    MTIRBCK        value
//
//
//  usage:
//       put_epics [-c cdev_pend_time] filename
//
//
//  ejw, 9-may-97


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system stuff
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>


// for cdev
#include <cdev.h>
#include <cdevData.h>
#include <cdevDevice.h>
#include <cdevRequestObject.h>
#include <cdevSystem.h>


// max entries in file
#define MAX_ENTRY 5000


// misc
D 3
static char *hv_filename;
E 3
I 3
static char *epics_filename;
I 5
static float set_val;
E 5
E 3
static int cdev_pend_time      = 60;
static int ncallback           = 0;
static char channel[MAX_ENTRY][60];
static char channel_put[MAX_ENTRY][10];
D 4
static int channel_val[MAX_ENTRY];
E 4
I 4
static float channel_val[MAX_ENTRY];
E 4
I 3
static int caput_mode          = 0;
I 6
static int debug               = 0;
E 6
E 3


// prototypes
void decode_command_line(int argc, char **argv);
void put_epics_data(void);
void epics_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
			    cdevData& result);


// ref to cdev system object
cdevSystem &cdevsys = cdevSystem::defaultSystem ();


//--------------------------------------------------------------------------


main(int argc,char **argv){


  // decode command line
  decode_command_line(argc,argv);

  // only print cdev error messages
  cdevsys.setThreshold(CDEV_SEVERITY_ERROR);

D 6
  // put HV data
E 6
I 6
  // put data
E 6
  put_epics_data();

  // done
  exit(EXIT_SUCCESS);

}
       

//----------------------------------------------------------------


void put_epics_data(){

  cdevGroup group;
  char buffer[120];
  int nentry=0;
  cdevRequestObject *obj[MAX_ENTRY];
  char chput[20];


  // read channel names from file and create request objects...ignore comments and blank lines
D 3
  ifstream file(hv_filename);
  while (file.eof()==0) {
    file.getline(buffer,sizeof(buffer));
    if(strlen(buffer)==0)continue;
    if(strlen(buffer)==strspn(buffer," \t\n\r\b"))continue;
    if(strncasecmp(buffer+strspn(buffer," \t\n\r\b"),"#",1)==0)continue;
    if(nentry>=MAX_ENTRY){
      cout << "?too many entries..." << MAX_ENTRY << " max...the rest are being ignored!" << endl;
      break;
E 3
I 3
  ifstream file(epics_filename);
D 7
  if(!file.bad()) {
E 7
I 7
  if(file.is_open()) {
E 7
D 8
    while (file.eof()==0) {
E 8
I 8
    while(file.is_open()&&file.good()&&!file.eof()&&!file.fail()&&!file.bad()) {
E 8
      file.getline(buffer,sizeof(buffer));
      if(strlen(buffer)==0)continue;
      if(strlen(buffer)==strspn(buffer," \t\n\r\b"))continue;
      if(strncasecmp(buffer+strspn(buffer," \t\n\r\b"),"#",1)==0)continue;
      if(nentry>=MAX_ENTRY){
	cout << "?too many entries..." << MAX_ENTRY << " max...the rest are being ignored!" << endl;
	break;
      }
      nentry++;
D 4
      sscanf(buffer,"%[^ .].%[^. ]%d",
E 4
I 4
      sscanf(buffer,"%[^ .].%[^. ]%f",
E 4
	     channel[nentry-1],channel_put[nentry-1],&channel_val[nentry-1]);
      if(strlen(channel_put[nentry-1])==0)strcpy(channel_put[nentry-1],"VAL");
      sprintf(chput,"set %s",channel_put[nentry-1]);
      obj[nentry-1] = cdevRequestObject::attachPtr(channel[nentry-1],chput);
E 3
    }
D 3
    nentry++;
E 3
I 3
    file.close();

  } else {
    caput_mode=1;
    cdev_pend_time=1;
    nentry=1;
I 5
D 6
    sprintf(buffer,"%s %f",epics_filename,set_val);
E 5
E 3
D 4
    sscanf(buffer,"%[^ .].%[^. ]%d",
E 4
I 4
    sscanf(buffer,"%[^ .].%[^. ]%f",
E 4
D 3
	   channel[nentry-1],channel_put[nentry-1],&channel_val[nentry-1]);
    if(strlen(channel_put[nentry-1])==0)strcpy(channel_put[nentry-1],"VAL");
    sprintf(chput,"set %s",channel_put[nentry-1]);
    obj[nentry-1] = cdevRequestObject::attachPtr(channel[nentry-1],chput);
E 3
I 3
	   channel[0],channel_put[0],&channel_val[0]);
E 6
I 6
    channel_val[0]=set_val;
    sprintf(buffer,"%s",epics_filename);
    sscanf(buffer,"%[^ .].%[^. ]",channel[0],channel_put[0]);
E 6
    if(strlen(channel_put[0])==0)strcpy(channel_put[0],"VAL");
    sprintf(chput,"set %s",channel_put[0]);
    obj[0] = cdevRequestObject::attachPtr(channel[0],chput);
I 6
    if(debug==1) {
      cout << channel[0] << "  '" << chput << "'  " << set_val << endl;
    }
E 6
E 3
  }
D 3
  file.close();
E 3


I 3

E 3
  // start group for all callbacks
  group.start();


  // create callbacks
  for(int i=0; i<nentry; i++){
    cdevCallback cb(epics_callback_func,(void*)(i));
    cdevData data;
D 4
    data.insert("value",(int) channel_val[i]);
E 4
I 4
D 6
    data.insert("value",(float)channel_val[i]);
E 6
I 6
D 8
    data.insert("value",channel_val[i]);
E 8
I 8
    data.insert((char*)"value",channel_val[i]);
E 8
E 6
E 4
    obj[i]->sendCallback(data,cb);
I 6
    //    obj[i]->send(data,0);
E 6
  }
  
  
  // process the group
  group.pend((double)cdev_pend_time);
  
  
  // check if all callbacks received
  if(ncallback<nentry){
D 3
    cerr << "?only received " << ncallback << " callbacks" << " out of " 
	 << nentry << " expected" << endl;
E 3
I 3
    if(caput_mode==0) {
      cerr << "?only received " << ncallback << " callbacks" << " out of " 
	   << nentry << " expected" << endl;
    }
E 3
  }    
  
  return;
}


//---------------------------------------------------------------------------


void epics_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
D 6
			    cdevData& result){
E 6
I 6
			    cdevData& result) {
E 6

  ncallback++;
  return;
}


//---------------------------------------------------------------------------


void decode_command_line(int argc, char **argv){

  int i=1;
D 5
  char *help = "\n  put_epics [-c cdev_pend_time] filename\n\n";
E 5
I 5
D 6
  char *help = "\n  put_epics [-c cdev_pend_time] filename|channel_name [value]\n\n";
E 6
I 6
D 8
  char *help = "\n  put_epics [-debug] [-c cdev_pend_time] filename|channel_name [value]\n\n";
E 8
I 8
  const char *help = "\n  put_epics [-debug] [-c cdev_pend_time] filename|channel_name [value]\n\n";
E 8
E 6
E 5

  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0){
      printf(help);
      exit(EXIT_SUCCESS);
I 6
    }
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
E 6
    }
    else if (strncasecmp(argv[i],"-c",2)==0){
      cdev_pend_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-",1)!=0){
D 3
      hv_filename=strdup(argv[i]);
E 3
I 3
      epics_filename=strdup(argv[i]);
I 5
      if(argc>=i)set_val=atof(argv[i+1]);
E 5
E 3
      break;
    }
  }

}


/*---------------------------------------------------------------------*/

E 1
