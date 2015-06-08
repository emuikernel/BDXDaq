/*************************************************************************\
* Copyright (c) 1994-2004 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 1997-2003 Southeastern Universities Research Association,
* as Operator of Thomas Jefferson National Accelerator Facility.
* Copyright (c) 1997-2002 Deutches Elektronen-Synchrotron in der Helmholtz-
* Gemelnschaft (DESY).
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

#define NO_X11_HERE /* Albert */
#define MAX_CAR_HIST_DEFAULT 1024*100 /* 100kB */
#define DEBUG 0
extern "C" {
#include "StripHistory.h"
#include "StripDataSource.h"
#include "getHistory.h"
#include "StripMisc.h"
}
#include <iostream.h>
#include "osiTime.h"
#include "osiTimeHelper.h"
/*
#include <BinArchive.h>
#include <BinValueIterator.h>
*/
#include <MultiArchive.h>
#include <MultiValueIterator.h>
#include "ArchiveException.h"

#include <signal.h>
#include <math.h>

#define ARCHIVE_NAME_ENVIRONMENT_VAR "STRIP_ARCHIVE"
#define ARCHIVE_REQUEST_ENVIRONMENT_VAR "STRIP_VERBOSE"
#define CAR_MAX_BUF_ENVIRONMENT_VAR "CAR_MAX_BUF"
/*USING_NAMESPACE_CHANARCH  new version of CAR doesn't use namespace */
#ifdef USE_NAMESPACE_STD
  using namespace std;
#endif
extern unsigned int historySize;
// AccessChanArch does the actual data acquisition from the archiver
int AccessChanArch(ArchiveI *, 
		   const stdString &, 
		   const osiTime &, 
		   const osiTime &, 
		   double *, 
		   short *, 
		   struct timeval *, 
		   size_t);

// CountSamples finds out how many distinct data points we have in the requested range 
// in order to allocate memory properly
size_t CountSamples(ArchiveI *, 
		    const stdString &, 
		    const osiTime &, 
		    const osiTime &);

static bool VERBOSE = false;

/* StripHistory_init
 */
extern "C" int CAR_init       (char ** archiverInfoP)
{
  //  StripHistoryInfo      *shi = 0;
  stdString ARCHIVE_NAME, VERBOSE_SWITCH;
  char *envString;
  ARCHIVE_NAME = getenv(ARCHIVE_NAME_ENVIRONMENT_VAR);
  if(ARCHIVE_NAME.empty())
    {
      cout << "You must declare environment variable STRIP_ARCHIVE" << endl;
      return -1;
    }
  
  VERBOSE_SWITCH = getenv(ARCHIVE_REQUEST_ENVIRONMENT_VAR);
  if(VERBOSE_SWITCH.empty())
    {
      cout << "You may want to declare environment variable STRIP_VERBOSE (=on or =off)" << endl;
    }
  else if(VERBOSE_SWITCH == "on")
    {
      cout << "Strip_History Verbose is turned on" << endl;
      VERBOSE = true;
    }

  //  historySize=MAX_CAR_HIST_DEFAULT;

  if(envString=getenv(CAR_MAX_BUF_ENVIRONMENT_VAR))
    if(historySize=atoi(envString)) {
      if(historySize>MAX_CAR_HIST_DEFAULT)
	historySize=MAX_CAR_HIST_DEFAULT;
    }

  cout <<"historySize="<<historySize <<endl;

  /*  *archiverInfoP =(char *) new BinArchive (ARCHIVE_NAME);*/
  *archiverInfoP =(char *) new MultiArchive (ARCHIVE_NAME);
  if(!*archiverInfoP ) return 1;
  return 0;
}


/* CAR_delete
 */
extern "C" void    CAR_delete     (StripHistoryInfo *shi)
{
  delete shi->archiverInfo;
  free (shi);
}



/* StripHistoryResult_release
 */
extern "C" void  CAR_Result_release (StripHistoryResult     *result)
{
 if(result->data)    delete[] result->data;
 if(result->times)   delete[] result->times;
 if(result->status)  delete[] result->status;
}

size_t CountSamples(ArchiveI *archiveI, const stdString &channel_name, const osiTime &start, const osiTime &end)
{
  size_t chunk_size=0, chunk_cumulative=0;

  Archive archive (archiveI);
  ChannelIterator channel(archive);
  ValueIterator	value(archive);
  if(!archive.findChannelByName (channel_name, channel))
    {
      cout <<channel_name<<" is not in Archive"<<endl;
      archive.detach();
      return 0;
    }

  channel->getValueAfterTime (start, value);

  while (value && (end == nullTime  ||  value->getTime() < end))     
    {
      chunk_size = value.determineChunk(end);
      for(size_t i = 0; i < chunk_size; i++) ++value;
      chunk_cumulative += chunk_size;

      if(chunk_cumulative > historySize) {
	History_MessageBox_popup
	  ("BIG REQUEST", 
	   "Ok",
	   "So many Data from Archiver\nPlease, decrease interval or increase # historyPoints.\n");	
	archive.detach();
	return 2;
      }
    }

  archive.detach();
  chunk_cumulative += 2;
  return chunk_cumulative;
}

int AccessChanArch(ArchiveI *archiveI, const stdString &channel_name, const osiTime &start, const osiTime &end, double *data, short *status, struct timeval *times, size_t samples)
{  
  Archive archive (archiveI);
  ChannelIterator channel(archive);
  ValueIterator	value(archive);
  size_t i = 0;
  size_t real_count=0;
  
  archive.findChannelByName (channel_name, channel);
  channel->getValueAfterTime (start, value);


  if(!value) {
    channel->getValueBeforeTime (start, value);
    data[real_count] = value->getDouble();
    times[real_count].tv_sec =  start.getSec() + 1;
    times[real_count].tv_usec = 0;    
    archive.detach();
    return 1;
  }
 
  --value;
  if(value) { /* left fitting */
    if(DEBUG) cerr<< "left fitting OK" <<endl;
    data[real_count] = value->getDouble();
    if(DEBUG) cerr<< "right fitting val="<< data[real_count] <<endl;
    times[real_count].tv_sec =  start.getSec()+1;
    times[real_count].tv_usec = 0;            
    ++real_count;
  } else  if(DEBUG) cerr<< "left fitting NO" <<endl;
  ++value;

  while (value && (end==nullTime || value->getTime() <end)&& i< (samples - 1))
    {
      i++;
      data[real_count] = value->getDouble();
      times[real_count].tv_sec =  value->getTime().getSec();
      times[real_count].tv_usec = value->getTime().getUSec();            
      ++value;
      ++real_count;
    }

  if(value) {
    if(DEBUG) cerr<< "right fitting OK" <<endl;
    data[real_count] = value->getDouble();
    if(DEBUG)  cerr<< "right fitting val="<< data[real_count] <<endl;
    times[real_count].tv_sec =  end.getSec() ;
    times[real_count].tv_usec = 0;            
    ++real_count;
  } else if(DEBUG) cerr<< "right fitting NO" <<endl;


  archive.detach();
  if(DEBUG) cerr<< "real_count=" <<real_count<< endl;
  return real_count;
}
extern "C" u_long get_CAR_data(        StripHistory   the_shi,
		     char           *nameP,
		     struct timeval *begin,
		     struct timeval *end,
		     struct timeval **timesP,
		     short          **statusP,
		     double         **dataP,
		     u_long *count)
{
  StripHistoryInfo *shi = (StripHistoryInfo *)the_shi;
  size_t samples;
  int no_of_p;
  stdString name=nameP;
  osiTime t0 = osiTime(begin->tv_sec, (begin->tv_usec * 1000));
  osiTime t1 = osiTime(end->tv_sec, (end->tv_usec * 1000));
  try
    {
      ArchiveI* arI=(ArchiveI*) shi->archiverInfo; 
      samples = CountSamples((ArchiveI*) shi->archiverInfo, name, t0, t1);
      if(VERBOSE == true)
	{  
	  cout << "Requested Channel: " << name << " " << t0 << " - " << t1
	       << " (" << samples << ")\n";
	}
    }
  catch (ArchiveException &e)
    {
      cerr<<"get_CAR_data:CountSamples problem " << e.what() <<endl;
      return (1);
    }
#if 1
  if(samples<=2) {
    if(VERBOSE == true) cout <<name<<" -no data in ChannelArchiver"<<endl;
    return (1);
  }
#endif
  struct timeval *times = new struct timeval[samples];
  double *data   = new double[samples];
  short  *status = new  short[samples];
  try
    {     
      no_of_p=AccessChanArch
	((ArchiveI*) shi->archiverInfo,name,t0,t1,data,status,times,samples);
      *timesP=times;
      *statusP=status;
      *dataP=data;
      *count=no_of_p;
    }
  catch (ArchiveException &e)
    {
      LOG_MSG ("BinChannel::getValueAfterTime caught: " << e.what());
      if(times)  free(times) ;
      if(data)   free(data)  ;
      if(status) free(status);
      return (1);
    }
  return(0);
}

#undef NO_X11_HERE /* Albert */










