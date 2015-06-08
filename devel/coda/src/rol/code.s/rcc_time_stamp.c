#ifdef Linux

/************************************************************************/
/*                                                                      */
/*   file: tstamp.c                                                     */
/*   author: Markus Joos, CERN-EP/ESS                                   */
/*                                                                      */
/*   This is the timestamping library.                                  */
/*                                                                      */
/*   History:                                                           */
/*   13.Aug.98  MAJO  created                                           */
/*   22.Sep.98  MAJO  Separate header file created                      */
/*   26.Feb.99  MAJO  additional run control functions added            */
/*    5.Mar.99  MAJO  more functions added                              */
/*   26.Jul.99  JOP   change algorithm in ts_duration                   */
/*   27.Jul.99  JOP   NGU  PC - i386 functions added                    */
/*    2.Aug.99  MAJO  Improved algorithm in ts_duration        		*/
/*   16.Aug.99  NGU   cpu frequency reading added for Linux             */
/*   13.Aug.01  MAJO  cpu frequency reading added for CES RIO3          */
/*                    IOM error system introduced                       */
/*   16.Jul.02  MAJO  Ported to RCC environment				*/
/*   23.Sep.02  MAJO  ts_wait added					*/
/*   19.Oct.04  MAJO  ts_wait_until, ts_offset and ts_compare added	*/
/*                                                                      */
/*******Copyright Ecosoft 2004 - Made from 80% recycled bytes************/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/file.h>
#include <sys/types.h>

/*sergey*/
#define TSTAMP

#include "rcc_error.h"
#include "rcc_time_stamp.h"

#ifdef TS_DEBUG 
  #define debug(x) printf x
#else
  #define debug(x)
#endif



/* Globals */
static int isopen = 0, got_freq = 0, tst0 = 0, tsmode[MAX_OPENS], tsnum[MAX_OPENS], tsmax[MAX_OPENS], tsactive[MAX_OPENS];
static double frequency_high, frequency_low, frequency_high_us, frequency_low_us;
static tstamp *ts[MAX_OPENS], *tsp, tzero;
static FILE *dfile;


/******************************/
TS_ErrorCode_t ts_get_freq(void)
/******************************/
{
  FILE *fd;
  int i1;
  float Frequency_low_us;
  char dumc[64];

  /* Determine the frequency of the clock */
  debug(("ts_get_freq: setting frequencies for Intel\n"));
  if ((fd = fopen("/proc/cpuinfo", "r") ) == 0)
  { 
    debug(("ts_get_freq: cannot open the gd cpuinfo\n")); 
    return(RCC_ERROR_RETURN(0, TSE_FILE)); 
  }
  for(i1 = 1; i1 < 30; i1++) 
  {
    fscanf (fd, "%s ", dumc);
    if (strcmp(dumc, "MHz") == 0) 
      break;
  }
  fscanf (fd, "%s ", dumc); 
  fscanf (fd, "%f", &Frequency_low_us);
  frequency_low_us = (double)Frequency_low_us;
  frequency_high_us = (frequency_low_us) / 4294967296.0;
  fclose(fd);

  debug(("ts_get_freq: hi_us:%f low_us:%f \n", frequency_high_us, frequency_low_us));
  frequency_high = frequency_high_us * 1000000.0;
  frequency_low = frequency_low_us * 1000000.0;  

  debug(("ts_get_freq: frequency_low    =%e\n", frequency_low));
  debug(("ts_get_freq: frequency_low_us =%e\n", frequency_low_us));
  debug(("ts_get_freq: frequency_high   =%e\n", frequency_high));
  debug(("ts_get_freq: frequency_high_us=%e\n", frequency_high_us));

  got_freq = 1;
  debug(("ts_get_freq: setting frequencies done.\n"));   
  return(RCC_ERROR_RETURN(0, TSE_OK));
}


/******************************************/
TS_ErrorCode_t ts_open(int size, int handle)
/******************************************/
{
  err_type ret;

  debug(("ts_open called\n"));

  if(!isopen)
  {  
    ret = iom_error_init(P_ID_TS, packTS_err_get) ;
    if(ret)
      return(RCC_ERROR_RETURN(0, TSE_ERROR_FAIL));
  }

  if (!got_freq)
  {
    ret = ts_get_freq();
    if (ret)
    {
      debug(("ts_open: error in ts_get_freq()\n"));
      return(RCC_ERROR_RETURN(ret, TSE_NO_FREQ));
    }
  }
  debug(("ts_open: frequency acquired\n"));

  if (handle != 0)
  {  
    if (handle > MAX_OPENS || handle < 1)
      return(RCC_ERROR_RETURN(0, TSE_ILL_HANDLE));

    /* Allocate an aray for the time stamps */
    tsmax[handle] = size;
    if (size)
      ts[handle] = (tstamp *)malloc(size * sizeof(tstamp));
    else
    {
      debug(("ts_open: error with parameter <size>\n"));
      return(RCC_ERROR_RETURN(0, TSE_ILL_SIZE));
    }

    tsactive[handle] = TS_STOP_TS;
    tsnum[handle] = 0;
    tsmode[handle] = TS_MODE_NORING;
  }

  isopen++;
  return(RCC_ERROR_RETURN(0, TSE_OK));
}


/***************************************************/
TS_ErrorCode_t ts_mode(int handle, unsigned int mode)
/***************************************************/
{
  int loop;
  
  if (!isopen)
  {
    debug(("ts_mode: Error=library is closed\n"));
    return(RCC_ERROR_RETURN(0, TSE_IS_CLOSED));
  }   
  
  if (handle > MAX_OPENS || handle < 1)
  {
    debug(("ts_mode: Error=TSE_ILL_HANDLE\n"));
    return(RCC_ERROR_RETURN(0, TSE_ILL_HANDLE));
  }
  
  if (mode != TS_MODE_RING && mode != TS_MODE_NORING)
  {
    debug(("ts_mode: Error=TSE_ILL_MODE\n"));
    return(RCC_ERROR_RETURN(0, TSE_ILL_MODE));
  }
    
  tsmode[handle] = mode;

  if (mode == TS_MODE_RING)
  {
    for(loop = 0; loop < tsmax[handle]; loop++)
    {
      tsp = (tstamp *)&ts[handle][loop];
      tsp->low = 0;
      tsp->high = 0;
      tsp->data = 0;
    }
  }
  
  debug(("ts_mode: mode set to %d\n", mode));
  return(RCC_ERROR_RETURN(0, TSE_OK));
}


/********************************************/
TS_ErrorCode_t ts_save(int handle, char *name)
/********************************************/
{
  /* Write the time stamps from the array to an ASCII file and reset the array */
  int res, loop, num;

  if (!isopen)
  {
    debug(("ts_save: Error=library is closed\n"));
    return(RCC_ERROR_RETURN(0, TSE_IS_CLOSED));
  }
  
  if (handle > MAX_OPENS || handle < 1)
    return(RCC_ERROR_RETURN(0, TSE_ILL_HANDLE));
    
  if(tsmode[handle] == TS_MODE_RING || tsmax[handle])
  { 
    dfile = fopen(name,"w+");
    if (dfile == 0)
    {
      debug(("ts_save: Cannot open parameter file\n"));
      return(RCC_ERROR_RETURN(0, TSE_PFILE));
    }

    /* Save the frequencies */
    fprintf(dfile, "%e\n", frequency_high);
    fprintf(dfile, "%e\n", frequency_low);
    debug(("ts_save: frequencies saved\n"));

    if (tsmode[handle] == TS_MODE_RING)
      num = tsmax[handle];
    else
      num = tsnum[handle];
      
    debug(("ts_save: saving %d data points\n", num)); 
    for(loop = 0; loop < num; loop++)
      fprintf(dfile, "%u %u %u\n", ts[handle][loop].high, ts[handle][loop].low, ts[handle][loop].data);

    res = fclose(dfile);
    if (res)
    {
      debug(("ts_save: Data file closed with error %d\n",res));
      return(RCC_ERROR_RETURN(0, TSE_PFILE));
    }
    tsnum[handle] = 0;
  } 
  return(RCC_ERROR_RETURN(0, TSE_OK));
}


/*********************************/
TS_ErrorCode_t ts_close(int handle)
/*********************************/
{
  
  if (handle > MAX_OPENS || handle < 0)
    return(RCC_ERROR_RETURN(0, TSE_ILL_HANDLE));
    
  if (isopen > 0)
  {
    isopen--;
    if (handle)
      free((void *)ts[handle]);
  }
  else
  {
    debug(("ts_close: Error=library is closed\n"));
    return(RCC_ERROR_RETURN(0, TSE_IS_CLOSED));
  }
  return(RCC_ERROR_RETURN(0, TSE_OK));
}


/*********************************************/
TS_ErrorCode_t ts_record(int handle, int udata)
/*********************************************/
{
  /* Record a time stamp together with an user defined identifier */
  /* if there is space left in the array */

  if (!isopen)
  {
    debug(("ts_record: Error=library is closed\n"));
    return(RCC_ERROR_RETURN(0, TSE_IS_CLOSED));                                
  }

  if (tsactive[handle] == 0)
  {
    tsp = (tstamp *)&ts[handle][tsnum[handle]];
    __asm__("rdtsc":"=a" (tsp->low), "=d" (tsp->high));
    tsp->data = udata;
    if (++tsnum[handle] >= tsmax[handle])
    {
      if (tsmode[handle] == TS_MODE_NORING)
      {
        /*debug(("ts_record: TS_MODE_NORING\n")); */
        tsactive[handle] |= TS_FULL;
      }
      else
      {
        /*debug(("ts_record: TS_MODE_RING\n")); */
        tsnum[handle] = 0;
      }
    }
  }
  return(RCC_ERROR_RETURN(0, TSE_OK));
}


/***************************/
TS_ErrorCode_t ts_sett0(void)
/***************************/
{
  if (!isopen)
  {
    debug(("ts_sett0: Error=library is closed\n"));
    return(RCC_ERROR_RETURN(0, TSE_IS_CLOSED));                                
  }

  if (!tst0)
  {
    ts_clock(&tzero);
    tst0 = 1;
  }

  return(RCC_ERROR_RETURN(0, TSE_OK));
}
 

/*********************************/
TS_ErrorCode_t ts_start(int handle)
/*********************************/
{
  if (!isopen)
  {
    debug(("ts_start: Error=library is closed\n"));
    return(RCC_ERROR_RETURN(0, TSE_IS_CLOSED));                                
  }
  
  if (handle > MAX_OPENS || handle < 1)
    return(RCC_ERROR_RETURN(0, TSE_ILL_HANDLE));
    
  tsactive[handle] &= TS_START_TS;
  debug(("ts_start: time stamping enabled for handle %d\n", handle));
  return(RCC_ERROR_RETURN(0, TSE_OK));
}


/*********************************/
TS_ErrorCode_t ts_pause(int handle)
/*********************************/
{
  if (!isopen)
  {
    debug(("ts_pause: Error=library is closed\n"));
    return(RCC_ERROR_RETURN(0, TSE_IS_CLOSED));                                
  }
  
  if (handle > MAX_OPENS || handle < 1)
    return(RCC_ERROR_RETURN(0, TSE_ILL_HANDLE));
    
  tsactive[handle] |= TS_STOP_TS;
  return(RCC_ERROR_RETURN(0, TSE_OK));
}


/**********************************/
TS_ErrorCode_t ts_resume(int handle)
/**********************************/
{
  if (!isopen)
  {
    debug(("ts_resume: Error=library is closed\n"));
    return(RCC_ERROR_RETURN(0, TSE_IS_CLOSED));                                
  }
  
  if (handle > MAX_OPENS || handle < 1)
    return(RCC_ERROR_RETURN(0, TSE_ILL_HANDLE));
    
  tsactive[handle] &= TS_START_TS;
  return(RCC_ERROR_RETURN(0, TSE_OK));
}


/************************************************/
TS_ErrorCode_t ts_elapsed (tstamp t1, float *time)
/************************************************/
{
  /* Calculate the time elapsed between a time stamp and tzero in us */
  float fdiff, low, hi;
  int diff;

  if (!isopen)
  {
    debug(("ts_elapsed: Error=library is closed\n"));
    return(RCC_ERROR_RETURN(0, TSE_IS_CLOSED));                                
  }

  if (!tst0)  /* tzero has not yet been recorded */
    return(RCC_ERROR_RETURN(0, TSE_NO_REF));

  diff = t1.low - tzero.low;
  fdiff = (float)diff;
  low = fdiff / frequency_low_us;

  diff = t1.high - tzero.high;
  fdiff = (float)diff;
  hi = fdiff / frequency_high_us;

  *time = hi + low;
  return(RCC_ERROR_RETURN(0, TSE_OK));
}


/*************************************/
float ts_duration(tstamp t1, tstamp t2)
/*************************************/
{
  /* Calculate the time elapsed between two time stamps (for on-line processing) */
  double time1, time2;
  int ret;

  if (!isopen)
  {
    debug(("ts_duration: Error=library is closed\n"));
    return(-1);                                
  }

  if (!got_freq)
  {
    ret = ts_get_freq();
    if(ret)
      return(-1);
  }

  debug(("ts_duration: frequency_high=%f\n", frequency_high));
  debug(("ts_duration: frequency_low=%f\n", frequency_low));
  debug(("ts_duration: t1.high      =%u\n", t1.high));
  debug(("ts_duration: t1.low       =%u\n", t1.low));
  debug(("ts_duration: t2.high      =%u\n", t2.high));
  debug(("ts_duration: t2.low       =%u\n", t2.low));
  time1 = (double)t1.high / frequency_high + (double)t1.low / frequency_low;
  time2 = (double)t2.high / frequency_high + (double)t2.low / frequency_low;
  debug(("ts_duration: time1        =%e\n", time1));
  debug(("ts_duration: time2        =%e\n", time2));
  debug(("ts_duration: duration     =%e\n", time2 - time1));

  return (time2 - time1);
}


/**********************************/
int ts_compare(tstamp t1, tstamp t2) 
/**********************************/
{
  /* Compare two timestamps */
  /* Returns: */
  /*  1 if t1>t2 */
  /* -1 if t1<t2  */
  /*  0 if t1=t2 */

  if (!isopen)
  {
    debug(("ts_compare: Error=library is closed\n"));
    return(RCC_ERROR_RETURN(0, TSE_IS_CLOSED));                                
  }
    
  if (t1.high > t2.high) return 1;
  if (t1.high < t2.high) return -1;
  if (t1.low > t2.low) return 1;
  if (t1.low < t2.low) return -1;
  return 0;  
}


/******************************************************/
TS_ErrorCode_t ts_offset(tstamp *ts, unsigned int usecs)
/******************************************************/
{
  /* Offset a given timestamp ts by a given time in usec   */

  long long int th64, ticks, ts64;
  
  if (!isopen)
  {
    debug(("ts_offset: Error=library is closed\n"));
    return(RCC_ERROR_RETURN(0, TSE_IS_CLOSED));                                
  }

  ticks = (unsigned long long int)((double)usecs * frequency_low_us);
  debug(("ts_offset: %d us = 0x%llx ticks\n", usecs, ticks));
  
  th64 = ts->high;
  ts64 = (th64 << 32) + ts->low;
  debug(("ts_offset: ts64 = 0x%llx\n", ts64));
  ts64 += ticks;
  debug(("ts_offset: ts64 + ticks = 0x%llx\n", ts64));

  ts->low = ts64 & 0xffffffff;
  ts->high = ts64 >> 32;
  
  debug(("ts_offset: New  low 32 bit of time stamp = 0x%08x\n", ts->low));
  debug(("ts_offset: New high 32 bit of time stamp = 0x%08x\n", ts->high));
  
  return(RCC_ERROR_RETURN(0, TSE_OK));
}


/***************************************************************/
TS_ErrorCode_t ts_wait_until(tstamp target, unsigned int *nyield)
/***************************************************************/
{
  /* Yield the current thread until a target time */
  int cnt;
  tstamp now;

  if (!isopen)
  {
    debug(("ts_wait_until: Error=library is closed\n"));
    return(RCC_ERROR_RETURN(0, TSE_IS_CLOSED));                           
  }
  
  cnt = 0;
  ts_clock(&now);
  while(ts_compare(target, now) > 0)
  {
    sched_yield();  /*Let's give other processes a chance*/
    cnt++;
    ts_clock(&now);
  }
  *nyield = cnt;

  return(RCC_ERROR_RETURN(0, TSE_OK));  
}


/***********************************/
TS_ErrorCode_t ts_clock(tstamp *time)
/***********************************/
{
  /* Record a time stamp for on-line processing */
  unsigned int a, b;

  if (!isopen)
   {
    debug(("ts_clock: Error=library is closed\n"));
    return(RCC_ERROR_RETURN(0, TSE_IS_CLOSED));                                  
   }

  __asm__("rdtsc" :"=a" (b), "=d" (a));
  time->high = a;
  time->low = b;
  return(RCC_ERROR_RETURN(0, TSE_OK));
}


/**************************************************************/
TS_ErrorCode_t ts_wait(unsigned int usecs, unsigned int *nyield)
/**************************************************************/
{
  volatile unsigned int a, b, y, z;
  int cnt;
  double delay, waited, time1, time2;

  if (!isopen)
  {
    debug(("ts_wait: Error=library is closed\n"));
    return(RCC_ERROR_RETURN(0, TSE_IS_CLOSED));                           
  }
  
  delay = (float)usecs;
  debug(("ts_wait: delay=%f\n", delay));

  cnt = 0;
  __asm__("rdtsc" :"=a" (b), "=d" (a));
  do
  {
    sched_yield();  /*Let's give other processes a chance */
    cnt++;
    __asm__("rdtsc" :"=a" (z), "=d" (y));
    time1 = (double)a / frequency_high_us + (double)b / frequency_low_us;
    time2 = (double)y / frequency_high_us + (double)z / frequency_low_us;
    waited = time2 - time1; 
    debug(("ts_wait: a=%d     b=%d\n", a, b));
    debug(("ts_wait: y=%d     z=%d\n", y, z));
    debug(("ts_wait: waited=%f\n", waited));
  }
  while(waited < delay);
  
  *nyield = cnt;

  return(RCC_ERROR_RETURN(0, TSE_OK));  
}


/*****************************************/
TS_ErrorCode_t ts_delay(unsigned int usecs)
/*****************************************/
{
  volatile unsigned int a, b, y, z;
  double delay, waited, time1, time2;

  delay = (float)usecs;

  if (!isopen)
  {
    debug(("ts_delay: Error=library is closed\n"));
    return(RCC_ERROR_RETURN(0, TSE_IS_CLOSED));                           
  }

  __asm__("rdtsc" :"=a" (b), "=d" (a));

  debug(("ts_delay: delay=%f\n", delay));

  do
  {
    __asm__("rdtsc" :"=a" (z), "=d" (y));

    time1 = (double)a / frequency_high_us + (double)b / frequency_low_us;
    time2 = (double)y / frequency_high_us + (double)z / frequency_low_us;
    waited = time2 - time1; 
    debug(("ts_delay: a=%d     b=%d\n", a, b));
    debug(("ts_delay: y=%d     z=%d\n", y, z));
    debug(("ts_delay: waited=%f\n", waited));
  }
  while(waited < delay);

  debug(("ts_delay: a=%d     b=%d\n", a, b));
  debug(("ts_delay: y=%d     z=%d\n", y, z));
  debug(("ts_delay: frequency_high_us: %e\n", frequency_high_us));
  debug(("ts_delay: frequency_low_us:  %e\n", frequency_low_us));
  debug(("ts_delay: frequency_high:    %e\n", frequency_high));
  debug(("ts_delay: frequency_low:     %e\n", frequency_low));

  return(RCC_ERROR_RETURN(0, TSE_OK));  
}


/***************************************************************************/
TS_ErrorCode_t packTS_err_get (err_pack err, err_str pid_str, err_str en_str)
/***************************************************************************/
{
  err_type result;

  strcpy(pid_str, P_ID_TS_STR);

  result = TSE_NOCODE;
  switch(err)
  {
    case TSE_OK:         strcpy(en_str, TSE_OK_STR);         break;
    case TSE_NO_FREQ:    strcpy(en_str, TSE_NO_FREQ_STR);    break;
    case TSE_FILE:       strcpy(en_str, TSE_FILE_STR);       break;
    case TSE_ILL_HANDLE: strcpy(en_str, TSE_ILL_HANDLE_STR); break;
    case TSE_ILL_SIZE:   strcpy(en_str, TSE_ILL_SIZE_STR);   break;
    case TSE_PFILE:      strcpy(en_str, TSE_PFILE_STR);      break;
    case TSE_NO_REF:     strcpy(en_str, TSE_NO_REF_STR);     break;
    case TSE_ERROR_FAIL: strcpy(en_str, TSE_ERROR_FAIL_STR); break;
    case TSE_IS_CLOSED:  strcpy(en_str, TSE_IS_CLOSED_STR);  break;
    case TSE_ILL_MODE:   strcpy(en_str, TSE_ILL_MODE_STR);   break;
    case TSE_NOCODE:     strcpy(en_str, TSE_NOCODE_STR);     break;
  } 

  if (result == TSE_NOCODE)
    strcpy(en_str, TSE_NOCODE_STR);

  return(RCC_ERROR_RETURN(0, result));
}


#endif
