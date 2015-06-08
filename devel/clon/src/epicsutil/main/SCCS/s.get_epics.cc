h37563
s 00008/00001/00236
d D 1.14 07/10/29 00:40:08 boiarino 15 14
c *** empty log message ***
e
s 00002/00321/00235
d D 1.13 07/10/29 00:26:28 boiarino 14 13
c getepics moved to the library's getepics.c
c 
e
s 00058/00052/00498
d D 1.12 07/10/29 00:08:03 boiarino 13 12
c add getepics() function
c 
e
s 00397/00097/00153
d D 1.11 07/10/28 23:13:29 boiarino 12 11
c new version: cdev replaced by CA
c 
e
s 00004/00001/00246
d D 1.10 07/10/12 09:59:18 boiarino 11 10
c *** empty log message ***
e
s 00002/00002/00245
d D 1.9 01/04/02 15:03:54 wolin 10 9
c Bombproofed .eof(), new compiler
e
s 00001/00001/00246
d D 1.8 01/01/03 10:45:25 wolin 9 8
c is_open()
e
s 00002/00002/00245
d D 1.7 00/08/30 14:02:07 wolin 8 7
c New CC
c 
e
s 00013/00001/00234
d D 1.6 99/11/10 13:01:56 wolin 7 6
c Now doesn't die when you get array...just prints first value
e
s 00004/00004/00231
d D 1.5 98/06/03 11:22:51 wolin 6 5
c val now float
c 
e
s 00001/00000/00234
d D 1.4 98/04/30 11:50:05 wolin 5 4
c Pend 1 sec in caget mode
c 
e
s 00050/00027/00184
d D 1.3 98/04/30 11:48:07 wolin 4 3
c Added caget_mode
c 
e
s 00027/00010/00184
d D 1.2 97/05/14 09:11:49 wolin 3 1
c Added outfile to command line args
e
s 00000/00000/00000
d R 1.2 97/05/09 16:38:30 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 epicsutil/s/get_epics.cc
c Name history : 1 0 s/get_epics.cc
e
s 00194/00000/00000
d D 1.1 97/05/09 16:38:29 wolin 1 0
c Get/put epics data using channel list in file
e
u
U
f e 0
t
T
I 12
/* TODO: implement ca_pend_time ?? 
 */

E 12
I 1
//
//  get_epics
//
//  Reads file of epics channel names, gets data, then writes data to stdout
I 4
//  If file doesn't exist uses file as channel name
E 4
//
I 4
//
E 4
//  File format
//  -----------
//    # first non-blank char is a comment line
//    B_xxx.DVI                   // reads channel B_xxx   via "get DVI"
//    MTIRBCK                     // reads channel MTIRBCK via "get"
//
//
//  usage:
D 3
//       get_epics [-c cdev_pend_time] filename
E 3
I 3
D 12
//       get_epics [-c cdev_pend_time] infile [outfile]
E 12
I 12
//       get_epics [-c ca_pend_time] infile [outfile]
E 12
E 3
//
//
//  ejw, 9-may-97
I 12
// sergey: implement CA-based version, eliminate cdev
E 12

D 12

E 12
// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system stuff
I 11

using namespace std;
#include <strstream>

E 11
#include <iostream.h>
#include <fstream.h>
D 11
#include <strstream.h>
E 11
#include <iomanip.h>

I 12
D 14
// EPICS CA includes
#include <stdio.h>
#include <epicsStdlib.h>
#include <string.h>
E 14
I 14
#include "epicsutil.h"
E 14
E 12

D 12
// for cdev
#include <cdev.h>
#include <cdevData.h>
#include <cdevDevice.h>
#include <cdevRequestObject.h>
#include <cdevSystem.h>
E 12
I 12
D 14
#include <alarm.h>
#include <cadef.h>
#include <epicsGetopt.h>
E 12

I 12
#include "tool_lib_1.h"
E 12

I 12
#define PEND_EVENT_SLICES 5     /* No. of pend_event slices for callback requests */

/* Different output formats */
typedef enum { plain, terse, all, specifiedDbr } OutputT;

/* Different request types */
typedef enum { get, callback } RequestT;

static int nConn = 0;           /* Number of connected PVs */
static int nRead = 0;           /* Number of channels that were read */
static int floatAsString = 0;   /* Flag: fetch floats as string */




E 12
// max entries in file
#define MAX_ENTRY 5000

E 14
I 12
// max number of elements in CA array
#define MAX_CHANNELS_LENGTH 1000
E 12

I 15
// max length of epics ca name
#define MAXCANAMELEN 60

E 15
I 13
D 14
// max length of epics ca name
#define MAXCANAMELEN 60

E 14
E 13
// misc
D 3
static char *hv_filename;
E 3
I 3
D 4
static char *hv_filename       = NULL;
E 4
I 4
static char *epics_filename    = NULL;
E 4
static char *outfile           = NULL;
E 3
D 12
static int cdev_pend_time      = 60;
E 12
I 12
static int ca_pend_time      = 60;
E 12
static int ncallback           = 0;
D 13
static char channel[MAX_ENTRY][60];
E 13
I 13
D 15
static char channel[MAX_ENTRY][MAXCANAMELEN];
E 15
I 15
static char *channel[MAX_ENTRY];
E 15
E 13
static char channel_get[MAX_ENTRY][10];
D 6
static int channel_val[MAX_ENTRY];
E 6
I 6
static float channel_val[MAX_ENTRY];
E 6
I 4
static int caget_mode          = 0;
E 4


// prototypes
void decode_command_line(int argc, char **argv);
D 12
void get_epics_data(void);
void epics_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
			    cdevData& result);
E 12
I 12
int get_epics_data(void);
E 12


D 12
// ref to cdev system object
cdevSystem &cdevsys = cdevSystem::defaultSystem ();
E 12

I 12
D 14
/*********************************************************************************/
/*********************************************************************************/
/****************** from $EPICS_BASE/src/catools/caget.c *************************/
E 12

D 12
//--------------------------------------------------------------------------
E 12
I 12
/*+**************************************************************************
 *
 * Function:	event_handler
 *
 * Description:	CA event_handler for request type callback
 * 		Allocates the dbr structure and copies the data
 *
 * Arg(s) In:	args  -  event handler args (see CA manual)
 *
 **************************************************************************-*/
E 12

I 12
void
event_handler(evargs args)
{
  pv *ppv;
E 12

D 12
main(int argc,char **argv){
E 12
I 12
  /* args.usr contains pointer to the 'ppv' from ca_create_subscription() call
  (see connaction_handler) */
  ppv = (pv *)args.usr;
E 12

I 12
  ppv->status = args.status;
  if (args.status == ECA_NORMAL)
  {
    ppv->dbrType = args.type;
    ppv->value   = calloc(1, dbr_size_n(args.type, args.count));
    memcpy(ppv->value, args.dbr, dbr_size_n(args.type, args.count));
    ppv->onceConnected = 1;
    nRead++;
  }
}
E 12

I 12
/*+**************************************************************************
 *
 * Function:	caget
 *
 * Description:	Issue read requests, wait for incoming data
 * 		and print the data according to the selected format
 *
 * Arg(s) In:	pvs       -  Pointer to an array of pv structures
 *              nPvs      -  Number of elements in the pvs array
 *              request   -  Request type
 *              format    -  Output format
 *              dbrType   -  Requested dbr type
 *              reqElems  -  Requested number of (array) elements
 *
 * Return(s):	Error code: 0 = OK, 1 = Error
 *
 **************************************************************************-*/

/* sergey: instead of printing values we'll fill channe1_val[] array; all error prints
   are commented out as well: we'll return '-9999' in case of any problem; only
   format='plain' is used here */
int
caget(pv *pvs, int nPvs, RequestT request, OutputT format,
D 13
      chtype dbrType, unsigned long reqElems)
E 13
I 13
      chtype dbrType, unsigned long reqElems, float valfloatarray[MAX_CHANNELS_LENGTH])
E 13
{
  unsigned int i;
  int n, result;
  float valfloat;
D 13
  float valfloatarray[MAX_CHANNELS_LENGTH];
E 13

  for(n=0; n<nPvs; n++)
  {
    /* Set up pvs structure */
    /* -------------------- */

    /* Get natural type and array count */
    pvs[n].nElems  = ca_element_count(pvs[n].ch_id);
    pvs[n].dbfType = ca_field_type(pvs[n].ch_id);

    /* Set up value structures */
    if (format != specifiedDbr)
    {
      dbrType = dbf_type_to_DBR_TIME(pvs[n].dbfType); /* Use native type */
      if (dbr_type_is_ENUM(dbrType))                  /* Enums honour -n option */
      {
        if (enumAsNr) dbrType = DBR_TIME_INT;
        else          dbrType = DBR_TIME_STRING;
      }
      else if (floatAsString &&
                 (dbr_type_is_FLOAT(dbrType) || dbr_type_is_DOUBLE(dbrType)))
      {
        dbrType = DBR_TIME_STRING;
      }
    }

    /* Adjust array count */
    if (reqElems == 0 || pvs[n].nElems < reqElems) reqElems = pvs[n].nElems;

    /* Remember dbrType and reqElems */
    pvs[n].dbrType  = dbrType;
    pvs[n].reqElems = reqElems;

    /* Issue CA request */
    /* ---------------- */

    if (ca_state(pvs[n].ch_id) == cs_conn)
    {
      nConn++;
      pvs[n].onceConnected = 1;
      if (request == callback) /* Event handler will allocate value */
      {
        result = ca_array_get_callback(dbrType,
                                       reqElems,
                                       pvs[n].ch_id,
                                       event_handler,
                                       (void*)&pvs[n]);
      }
      else /* Allocate value structure */
      {
        pvs[n].value = calloc(1, dbr_size_n(dbrType, reqElems));
        result = ca_array_get(dbrType,
                              reqElems,
                              pvs[n].ch_id,
                              pvs[n].value);
      }
      pvs[n].status = result;
    }
    else
    {
      pvs[n].status = ECA_DISCONN;
    }
  }
  if (!nConn) return(1);              /* No connection? We're done. */

  /* Wait for completion */
  /* ------------------- */

  result = ca_pend_io(caTimeout);
  if (result == ECA_TIMEOUT)
	fprintf(stderr, "Read operation timed out: some PV data was not read.\n");

  if (request == callback)    /* Also wait for callbacks */
  {
    double slice = caTimeout / PEND_EVENT_SLICES;
    for (n = 0; n < PEND_EVENT_SLICES; n++)
    {
      ca_pend_event(slice);
      if (nRead >= nConn) break;
    }
    if (nRead < nConn)
            fprintf(stderr, "Read operation timed out: some PV data was not read.\n");
  }

  /* Print the data */
  /* -------------- */

  for(n=0; n<nPvs; n++)
  {
    reqElems = pvs[n].reqElems;

    switch (format)
    {
        case plain:             /* Emulate old caget behaviour */
            if (reqElems <= 1) /*printf("%-30s ", pvs[n].name)*/;
            else               /*printf("%s", pvs[n].name)*/;
        case terse:
            if (pvs[n].status == ECA_DISCONN)
			  /*printf("*** not connected\n")*/;
            else if (pvs[n].status == ECA_NORDACCESS)
			  /*printf("*** no read access\n")*/;
            else if (pvs[n].status != ECA_NORMAL)
			  /*printf("*** CA error %s\n", ca_message(pvs[n].status))*/;
            else if (pvs[n].value == 0)
			  /*printf("*** no data available (timeout)\n")*/;
            else
            {
              char *ss;
              if (reqElems > 1) /*printf(" %lu ", reqElems)*/;
              for (i=0; i<reqElems; ++i)
			  {
                ss = val2str(pvs[n].value, pvs[n].dbrType, i, &valfloat);
                /*printf("%s ", ss)*/;
			  }
              /*printf("\n")*/;
D 13
              if(reqElems == 1) channel_val[n] = valfloat; /* no CA arrays here */
E 13
I 13
              if(reqElems == 1) valfloatarray[n] = valfloat; /* no CA arrays here */
E 13
            }
            break;
        case all:
            print_time_val_sts(&pvs[n], reqElems, valfloatarray);
            break;
        case specifiedDbr:
            printf("%s\n", pvs[n].name);
            if (pvs[n].status == ECA_DISCONN)
                printf("    *** not connected\n");
            else if (pvs[n].status == ECA_NORDACCESS)
                printf("    *** no read access\n");
            else if (pvs[n].status != ECA_NORMAL)
                printf("    *** CA error %s\n", ca_message(pvs[n].status));
            else
            {
              printf("    Data type:      %s (native: %s)\n",
                       dbr_type_to_text(pvs[n].dbrType),
                       dbf_type_to_text(pvs[n].dbfType));
              if (pvs[n].dbrType == DBR_CLASS_NAME)
			  {
                printf("    Class Name:     %s\n",
                           *((dbr_string_t*)dbr_value_ptr(pvs[n].value,
                                                          pvs[n].dbrType)));
			  }
              else
              {
                printf("    Element count:  %lu\n"
                           "    Value:          ",
                           reqElems);
                for (i=0; i<reqElems; ++i)       /* Print value(s) */
                        printf("%s ", val2str(pvs[n].value, pvs[n].dbrType, i, &valfloat));
                printf("\n");

                /* Extended type extra info */
                if (pvs[n].dbrType > DBR_DOUBLE)
                        printf("%s\n", dbr2str(pvs[n].value, pvs[n].dbrType));
                   
              }
            }
            break;
        default :
            break;
    }
  }
  return(0);
}


/*********************************************************/
/*********************************************************/
/*********************************************************/



E 14
//--------------------------------------------------------------------------

int
main(int argc,char **argv)
{
E 12
  // decode command line
  decode_command_line(argc,argv);

D 12
  // only print cdev error messages
  cdevsys.setThreshold(CDEV_SEVERITY_ERROR);

E 12
  // get HV data
  get_epics_data();

  // done
  exit(EXIT_SUCCESS);

}
       

//----------------------------------------------------------------

I 13
D 14
/*
input:
 nPvs - the number of PVs
 */
E 13

E 14
D 12
void get_epics_data(){
E 12
I 12
int
D 13
getepics()
E 13
I 13
D 14
getepics(int nPvs, char canames[MAX_ENTRY][MAXCANAMELEN], float valfloats[MAX_ENTRY])
E 13
{
I 13
  int n = 0;
  int result;
  OutputT format = plain;     /* User specified format */
  RequestT request = get;     /* User specified request type */
  int count = 0;              /* 0 = not specified by -# option */
  int type = -1;              /* getopt() data type argument */
  pv *pvs = 0;                /* Array of PV structures */

  if(nPvs < 1)
  {
    fprintf(stderr, "No pv name specified. ('get_epics -h' for help.)\n");
    return(1);
  }

  /* Start up Channel Access */

  result = ca_context_create(ca_disable_preemptive_callback);
  if(result != ECA_NORMAL)
  {
    fprintf(stderr, "CA error %s occurred while trying "
            "to start channel access '%s'.\n", ca_message(result), pvs[n].name);
    return(1);
  }

  /* Allocate PV structure array */
  pvs = (pv *) calloc(nPvs, sizeof(pv));
  if (!pvs)
  {
    fprintf(stderr, "Memory allocation for channel structures failed.\n");
    return(1);
  }

  /* Connect channels */
  for(n=0; n<nPvs; n++)
  {
    pvs[n].name = (char *)&canames[n][0];       /* Copy PV names */
  }
  connect_pvs(pvs, nPvs);

  /* Read and print data */
  result = caget(pvs, nPvs, request, format, type, count, valfloats);

  /* Shut down Channel Access */
  ca_context_destroy();

E 13
  return(0);
}
E 12

D 12
  cdevGroup group;
E 12
I 12
int
E 14
get_epics_data()
{
E 12
  char buffer[120];
  int nentry=0;
D 12
  cdevRequestObject *obj[MAX_ENTRY];
E 12
  char chget[20];
I 12
D 13
  /* CA stuff */
  int n = 0;
E 13
  int result;
D 13
  OutputT format = plain;     /* User specified format */
  RequestT request = get;     /* User specified request type */
  int count = 0;              /* 0 = not specified by -# option */
  int type = -1;              /* getopt() data type argument */
  int nPvs;                   /* Number of PVs */
  pv *pvs = 0;                /* Array of PV structures */
  /************/
E 13
E 12

D 12

E 12
  // read channel names from file and create request objects...ignore comments and blank lines
D 4
  ifstream file(hv_filename);
I 3
  if(file.bad()){ cerr << "cannot open " << hv_filename << endl << endl; exit(EXIT_FAILURE);}
E 3
  while (file.eof()==0) {
    file.getline(buffer,sizeof(buffer));
    if(strlen(buffer)==0)continue;
    if(strlen(buffer)==strspn(buffer," \t\n\r\b"))continue;
    if(strncasecmp(buffer+strspn(buffer," \t\n\r\b"),"#",1)==0)continue;
    if(nentry>=MAX_ENTRY){
      cout << "?too many entries..." << MAX_ENTRY << " max...the rest are being ignored!" << endl;
      break;
E 4
I 4
  ifstream file(epics_filename);
D 9
  if(!file.bad()) {
E 9
I 9
D 12
  if(file.is_open()) {
E 9
D 10
    while (file.eof()==0) {
E 10
I 10
    while(file.is_open()&&file.good()&&!file.eof()&&!file.fail()&&!file.bad()) {
E 12
I 12
  if(file.is_open())
  {
    while(file.is_open()&&file.good()&&!file.eof()&&!file.fail()&&!file.bad())
    {
E 12
E 10
      file.getline(buffer,sizeof(buffer));
      if(strlen(buffer)==0)continue;
      if(strlen(buffer)==strspn(buffer," \t\n\r\b"))continue;
      if(strncasecmp(buffer+strspn(buffer," \t\n\r\b"),"#",1)==0)continue;
D 12
      if(nentry>=MAX_ENTRY){
	cout << "?too many entries..." << MAX_ENTRY << " max...the rest are being ignored!" << endl;
	break;
E 12
I 12
      if(nentry>=MAX_ENTRY)
      {
	    cout << "?too many entries..." << MAX_ENTRY << " max...the rest are being ignored!" << endl;
	    break;
E 12
      }
      nentry++;
I 15
      channel[nentry-1] = (char *) malloc(MAXCANAMELEN+1);
E 15
      sscanf(buffer+strspn(buffer," \t\n\r\b"),"%[^ .].%[^. ]",
	     channel[nentry-1],channel_get[nentry-1]);
D 6
      channel_val[nentry-1]=-9999;
E 6
I 6
      channel_val[nentry-1]=-9999.;
E 6
D 12
      if(strlen(channel_get[nentry-1])==0)strcpy(channel_get[nentry-1],"VAL");
      sprintf(chget,"get %s",channel_get[nentry-1]);
      obj[nentry-1] = cdevRequestObject::attachPtr(channel[nentry-1],chget);
E 12
I 12
      if(strlen(channel_get[nentry-1])==0) strcpy(channel_get[nentry-1],"VAL");
E 12
E 4
    }
D 4
    nentry++;
    sscanf(buffer+strspn(buffer," \t\n\r\b"),"%[^ .].%[^. ]",
	   channel[nentry-1],channel_get[nentry-1]);
    channel_val[nentry-1]=-1;
    if(strlen(channel_get[nentry-1])==0)strcpy(channel_get[nentry-1],"VAL");
    sprintf(chget,"get %s",channel_get[nentry-1]);
    obj[nentry-1] = cdevRequestObject::attachPtr(channel[nentry-1],chget);
E 4
I 4
    file.close();
D 12

  } else {
E 12
I 12
  }
  else
  {
E 12
    caget_mode=1;
I 5
D 12
    cdev_pend_time=1;
E 12
I 12
    ca_pend_time=1;
E 12
E 5
    nentry=1;
I 15
    channel[0] = (char *) malloc(MAXCANAMELEN+1);
E 15
    sscanf(epics_filename+strspn(epics_filename," \t\n\r\b"),"%[^ .].%[^. ]",
	   channel[0],channel_get[0]);
D 6
    channel_val[0]=-9999;
E 6
I 6
    channel_val[0]=-9999.;
E 6
D 12
    if(strlen(channel_get[0])==0)strcpy(channel_get[0],"VAL");
    sprintf(chget,"get %s",channel_get[0]);
    obj[0] = cdevRequestObject::attachPtr(channel[0],chget);
E 12
I 12
    if(strlen(channel_get[0])==0) strcpy(channel_get[0],"VAL");
E 12
E 4
  }
D 4
  file.close();
E 4


D 12
  // start group for all callbacks
  group.start();
E 12


D 12
  // create callbacks
  for(int i=0; i<nentry; i++){
    cdevCallback cb(epics_callback_func,(void*)(i));
    obj[i]->sendCallback(NULL,cb);
E 12
I 12



  /*** sergey: new CA stuff ***/

D 13
  nPvs = nentry;       /* the number of PV names */
  if(nPvs < 1)
  {
    fprintf(stderr, "No pv name specified. ('get_epics -h' for help.)\n");
    return(1);
E 12
  }
E 13
I 13
  result = getepics(nentry, channel, channel_val);
D 14
  printf("%d %d >%s< -> %f\n",result,nentry,channel[0],channel_val[0]);
E 14
I 14
  /*printf("%d %d >%s< -> %f\n",result,nentry,channel[0],channel_val[0]);*/
E 14
E 13
D 12
  
  
  // process the group
  group.pend((double)cdev_pend_time);
  
  
E 12
I 12

D 13
  /* Start up Channel Access */

  result = ca_context_create(ca_disable_preemptive_callback);
  if(result != ECA_NORMAL)
  {
    fprintf(stderr, "CA error %s occurred while trying "
            "to start channel access '%s'.\n", ca_message(result), pvs[n].name);
    return(1);
  }

  /* Allocate PV structure array */
  pvs = (pv *) calloc(nPvs, sizeof(pv));
  if (!pvs)
  {
    fprintf(stderr, "Memory allocation for channel structures failed.\n");
    return(1);
  }

  /* Connect channels */
  for(n=0; n<nPvs; n++)
  {
    pvs[n].name = channel[n];       /* Copy PV names */
  }
  connect_pvs(pvs, nPvs);

  /* Read and print data */
  result = caget(pvs, nPvs, request, format, type, count);

  /* Shut down Channel Access */
  ca_context_destroy();


E 13
  /****************************/







  /*
E 12
  // check if all callbacks received
D 4
  if(ncallback<nentry){
    cerr << "?only received " << ncallback << " callbacks" << " out of " 
E 4
I 4
  if(ncallback<nentry) {
    if(caget_mode==0) {
      cerr << "?only received " << ncallback << " callbacks" << " out of " 
E 4
	 << nentry << " expected" << endl;
I 4
    } else {
      cout << channel_val[0] << endl;
      return;
    }
E 4
  }    
D 12
  
E 12
I 12
  */
E 12

I 12


E 12
D 3
  // write results to stdout
  cout.setf(ios::left);
  for( i=0; i<nentry; i++){
    strstream name;
    name << channel[i] << "." << channel_get[i] << ends;
    cout << setw(30) << name.str() << channel_val[i] << endl;
E 3
I 3
  // write results
D 12
  if(outfile==NULL){
E 12
I 12
  if(outfile==NULL)
  {
E 12
    cout.setf(ios::left);
D 4
    for( i=0; i<nentry; i++){
      strstream name;
      name << channel[i] << "." << channel_get[i] << ends;
      cout << setw(30) << name.str() << channel_val[i] << endl;
E 4
I 4
D 8
    for( i=0; i<nentry; i++) {
E 8
I 8
D 12
    for(int i=0; i<nentry; i++) {
E 8
      if(caget_mode==0) {
	strstream name;
	name << channel[i] << "." << channel_get[i] << ends;
	cout << setw(30) << name.str() << channel_val[i] << endl;
      } else {
	cout << channel_val[0] << endl;
E 12
I 12
    for(int i=0; i<nentry; i++)
    {
      if(caget_mode==0)
      {
	    strstream name;
	    name << channel[i] << "." << channel_get[i] << ends;
	    cout << setw(30) << name.str() << channel_val[i] << endl;
E 12
      }
I 12
      else
      {
	    cout << channel_val[0] << endl;
      }
E 12
E 4
    }
D 12
  } else {
E 12
I 12
  }
  else
  {
E 12
    ofstream ofile(outfile);
    ofile.setf(ios::left);
D 8
    for( i=0; i<nentry; i++){
E 8
I 8
D 12
    for(int i=0; i<nentry; i++){
E 12
I 12
    for(int i=0; i<nentry; i++)
    {
E 12
E 8
      strstream name;
      name << channel[i] << "." << channel_get[i] << ends;
      ofile << setw(30) << name.str() << channel_val[i] << endl;
    }
E 3
  }

I 15
  for(int i=0; i<nentry; i++) free(channel[i]);

E 15
D 3

E 3
D 12
  return;
E 12
I 12
  return(result);
E 12
}


D 12
//---------------------------------------------------------------------------
E 12

D 12

void epics_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
			    cdevData& result){

I 7
  size_t count;


E 7
  ncallback++;
D 6
  channel_val[(int)userarg] = (int) result;
E 6
I 6
D 7
  channel_val[(int)userarg] = (float) result;
E 7
I 7
  result.getElems((char*)"value",&count);

  if(count==1) {
    channel_val[(int)userarg] = (float) result;
  } else {
    float *f = new float[count];
    channel_val[(int)userarg] = f[0];
    delete[] f;
  }

E 7
E 6

  return;
}


E 12
//---------------------------------------------------------------------------


D 12
void decode_command_line(int argc, char **argv){

E 12
I 12
void
decode_command_line(int argc, char **argv)
{
E 12
  int i=1;
D 3
  char *help = "\n  get_epics [-c cdev_pend_time] filename\n\n";
E 3
I 3
D 10
  char *help = "\n  get_epics [-c cdev_pend_time] infile [outfile]\n\n";
E 10
I 10
D 12
  const char *help = "\n  get_epics [-c cdev_pend_time] infile [outfile]\n\n";
E 12
I 12
  const char *help = "\n  get_epics [-c ca_pend_time] infile [outfile]\n\n";
E 12
E 10
E 3

I 3
D 12
  if(argc<2){ cout << help; exit(EXIT_SUCCESS);}
E 12
I 12
  if(argc<2)
  {
    cout << help;
    exit(EXIT_SUCCESS);
  }
E 12

E 3
D 12
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0){
E 12
I 12
  while(i<argc)
  {
    if(strncasecmp(argv[i],"-h",2)==0)
    {
E 12
      printf(help);
      exit(EXIT_SUCCESS);
    }
D 12
    else if (strncasecmp(argv[i],"-c",2)==0){
      cdev_pend_time=atoi(argv[i+1]);
      i=i+2;
E 12
I 12
    else if (strncasecmp(argv[i],"-c",2)==0)
    {
      ca_pend_time=atoi(argv[i+1]);
      i += 2;
E 12
    }
D 12
    else if (strncasecmp(argv[i],"-",1)!=0){
E 12
I 12
    else if (strncasecmp(argv[i],"-",1)!=0)
    {
E 12
D 4
      hv_filename=strdup(argv[i]);
E 4
I 4
      epics_filename=strdup(argv[i]);
E 4
I 3
D 12
      if(argc>i+1)outfile=strdup(argv[i+1]);
E 12
I 12
      if(argc>i+1) outfile=strdup(argv[i+1]);
E 12
E 3
      break;
    }
  }

I 3
D 4
  if(hv_filename==NULL){ cout << help; exit(EXIT_SUCCESS);}
E 4
I 4
D 12
  if(epics_filename==NULL){ cout << help; exit(EXIT_SUCCESS);}
E 12
I 12
  if(epics_filename==NULL)
  {
    cout << help;
    exit(EXIT_SUCCESS);
  }
E 12
E 4

  return;
E 3
}


/*---------------------------------------------------------------------*/

E 1
