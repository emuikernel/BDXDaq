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
extern "C" {
#include "StripHistory.h"
#include "StripDataSource.h"
}

#include <iostream.h>
#include "osiTime.h"
#include "osiTimeHelper.h"
#include <BinArchive.h>
#include <BinValueIterator.h>
#include <signal.h>
#include <math.h>

#define ARCHIVE_NAME_ENVIRONMENT_VAR "STRIP_ARCHIVE"
#define ARCHIVE_REQUEST_ENVIRONMENT_VAR "STRIP_VERBOSE"
/*USING_NAMESPACE_CHANARCH
#ifdef USE_NAMESPACE_STD
  using namespace std;
#endif
*/

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

/* StripHistoryInfo
 *
 *      Contains instance data for the archive service.
 */
typedef struct _StripHistoryInfo
{
  Strip         strip;
  ArchiveI      *archiveI;
}
StripHistoryInfo;


/* StripHistory_init
 */
extern "C" StripHistory    StripHistory_init       (Strip strip)
{
  StripHistoryInfo      *shi = 0;
  stdString ARCHIVE_NAME, VERBOSE_SWITCH;
  
  ARCHIVE_NAME = getenv(ARCHIVE_NAME_ENVIRONMENT_VAR);
  if(ARCHIVE_NAME.empty())
    {
      cout << "You must declare environment variable STRIP_ARCHIVE" << endl;
      exit(1);
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
  
  
  if (shi = (StripHistoryInfo *)malloc (sizeof(StripHistoryInfo)))
    {
      shi->strip = strip;
      shi->archiveI = new BinArchive (ARCHIVE_NAME);
    }
  
  return (StripHistory)shi;
}


/* StripHistory_delete
 */
extern "C" void    StripHistory_delete     (StripHistory the_shi)
{
  StripHistoryInfo      *shi = (StripHistoryInfo *)the_shi;

  delete shi->archiveI;
  free (shi);
}


/* StripHistory_fetch
 */
extern "C" FetchStatus     StripHistory_fetch      (StripHistory           the_shi,
						    char                   *name,
						    struct timeval         *begin,
						    struct timeval         *end,
						    StripHistoryResult     *result,
						    StripHistoryCallback   BOGUS(callback),
						    void                   *BOGUS(call_data))
{
  StripHistoryInfo *shi = (StripHistoryInfo *)the_shi;
  size_t samples;
  int no_of_points;

  osiTime t0 = osiTime(begin->tv_sec, (begin->tv_usec * 1000));
  osiTime t1 = osiTime(end->tv_sec, (end->tv_usec * 1000));

  samples = CountSamples(shi->archiveI, name, t0, t1);

  if(VERBOSE == true)
    {  
      cout << "Requested Channel: " << name << " " << t0 << " - " << t1
	   << " (" << samples << ")\n";
    }
  
  
  struct timeval *times = new struct timeval[samples];
  double *data = new double[samples];
  short *status = new short[samples];

  no_of_points = AccessChanArch(shi->archiveI, name, t0, t1, data, status, times, samples);

  if(VERBOSE == true)
    {  
      cout << "Requested Channel: " << name << " " << t0 << " - " << t1
	   << "NoP= (" << no_of_points << ")\n";
    }
  
  
  /* remember the request range */
  result->t0 = *begin;
  result->t1 = *end;

  if ((compare_times (begin, &times[no_of_points-1]) <= 0) && 
      (compare_times (end, &times[0]) >= 0) &&
      (compare_times (begin, end) <= 0)) 
    {
      result->times = times; 
      result->data = data;
      result->status = status;
      result->n_points = no_of_points; 
      result->fetch_stat = FETCH_DONE;
    }
  else
    { 
      result->times = 0;
      result->data = 0;
      result->status = 0;
      delete[] times;
      delete[] data;
      delete[] status;
      result->fetch_stat = FETCH_NODATA;
    }

  return result->fetch_stat;
}


/* StripHistory_cancel
 */
extern "C" void    StripHistory_cancel     (StripHistory           BOGUS(the_shi),
					    StripHistoryResult     *BOGUS(result))
{
}


/* StripHistoryResult_release
 */
void  StripHistoryResult_release    (StripHistory           BOGUS(the_shi),
                                     StripHistoryResult     *result)
{
  delete[] result->data;
  delete[] result->times;
  delete[] result->status;
}

size_t CountSamples(ArchiveI *archiveI, const stdString &channel_name, const osiTime &start, const osiTime &end)
{
  Archive archive (archiveI);
  ChannelIterator channel(archive);
  ValueIterator	value(archive);
  size_t chunk_size=0, chunk_cumulative=0;

  archive.findChannelByName (channel_name, channel);

  channel->getValueAfterTime (start, value);

  while (value && (end == nullTime  ||  value->getTime() < end))     
    {
      chunk_size = value.determineChunk(end);
      for(int i = 0; i < chunk_size; i++) ++value;
      chunk_cumulative += chunk_size;
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
  int i = 0;
  
  archive.findChannelByName (channel_name, channel);

  channel->getValueAfterTime (start, value);

  if(!value)
    {
      for(i = 0; i < 2; i++) status[i] &= ~DATASTAT_PLOTABLE;
      times[0].tv_sec = start.getSec();
      times[0].tv_usec = start.getUSec();
      times[1].tv_sec = end.getSec();
      times[1].tv_usec = start.getUSec();
      archive.detach();
      return 2;
    }
	  
  while (value && (end == nullTime  ||  value->getTime() < end) && i < (samples - 1))
    {
      if(value->isInfo())
	status[i] &= ~DATASTAT_PLOTABLE;
      else {
	data[i] = value->getDouble();	  
	status[i] |= DATASTAT_PLOTABLE;
      }
      
      times[i].tv_sec = value->getTime().getSec();
      times[i].tv_usec = value->getTime().getUSec();      
      
      ++value;
      ++i;
    }

  //pad end with a ~DATASTAT_PLOTABLE to avoid interpolation
  status[i] &= ~DATASTAT_PLOTABLE;
  
  times[i].tv_sec = end.getSec();
  times[i].tv_usec = end.getUSec();
  
  //########################  
  
  archive.detach();
  
  return ++i;
}
#undef NO_X11_HERE /* Albert */










