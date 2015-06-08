h35939
s 00042/00020/00214
d D 1.2 03/04/17 16:55:40 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 19:12:18 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/util.s/unixtime.c
e
s 00234/00000/00000
d D 1.1 01/11/19 19:12:17 boiarino 1 0
c date and time created 01/11/19 19:12:17 by boiarino
e
u
U
f e 0
t
T
I 1
D 3
#include <time.h>
E 3
I 3

/* unixtime.c */

E 3
#include <stdio.h>
#include <string.h>
I 3
#include <time.h>
E 3

I 3
#include "uttime.h"

E 3
/**********************************************************
  Provides a FORTRAN wrapper for a call to get the ascii
  time string via the ctime functiom.


  to use:

  1)  compile this file:    $cc -c unixtime.c
  2)  link unixtime.o into your application
  3)  get the 32 bit "unix time"

        CALL GetUnixTime(timetp)

  where timetp is an INTEGER*32.

  4)      CALL GetASCIITime(timetp, timestr)

      where time is the 32 bit result of step 3, and
      timesstr is a CHARACTER*26 timestr

(of course, if timetp is from the Header bank, you can skip
step 3)

*********************************************************/

/****** getunixtime_ (FORTRAN WRAPPER) ********/

D 3
void getunixtime_(time_tp)

time_t    *time_tp;

E 3
I 3
void
getunixtime_(time_t *time_tp)
E 3
{
  time(time_tp);
}

/*-------- getasciitime -----------*/

D 3
void getasciitime(time_tp, timestr)
E 3
I 3
/*
time_tp   - 32 bit integer unix time
timestr   - should be at least 27 characters
*/
E 3

D 3
/* this will be called by the FORTRAN wrapper */

time_t *time_tp;      /* 32 bit integer unix time */
char   *timestr;      /* should be at least 27 characters */

E 3
I 3
void
getasciitime(time_t *time_tp, char *timestr)
E 3
{
D 3
  char  *datestr;
E 3
I 3
  char *datestr = NULL;
  /*printf("22\n");fflush(stdout);*/
E 3
  datestr = ctime(time_tp);
D 3
  strcpy(timestr, datestr);
E 3
I 3
  /*printf("33: 0x%08x\n",datestr);fflush(stdout);*/
  if(datestr != NULL)
  {
	/*
printf(">%24.24s<\n",datestr);fflush(stdout);
printf("lenn=%d\n",strlen(datestr));fflush(stdout);
printf("44\n");fflush(stdout);
	*/
    strcpy(timestr, datestr);
  }
  else
  {
    strcpy(timestr,"da ne znau ya vremya !!!");
  }

  return;
E 3
}

/****** getasciitime_ (FORTRAN WRAPPER) ********/
D 3

void getasciitime_(time_tp, fstr)

int    *time_tp;      /* 32 bit integer unix time */
char   *fstr;      /* should be exactly 26 characters */

E 3
I 3
/*
time_tp - 32 bit integer unix time
fstr    - should be exactly 26 characters
*/
void
getasciitime_(time_t *time_tp, char *fstr, size_t len)
E 3
{
D 3
  char  tstr[27];
E 3
I 3
  char tstr[27];
  /*printf("11: >%s< %d\n",fstr,len);fflush(stdout);*/
E 3
  getasciitime(time_tp, tstr);
  strncpy(fstr, tstr, 24);
I 3

  return;
E 3
}

/* 10 msec timer

  struct tms
  {
    clock_t tms_utime;   user time
    clock_t tms_stime;   system time
    clock_t tms_cutime;  user time of children
    clock_t tms_cstime;  system time of children
  };

*/

#if defined(SunOS) || defined(Linux)

#include <sys/times.h>
#include <limits.h>

static nev = 0;
static struct tms t1,t2;
static struct timespec to;
static double d1,d2,ddd;
static double rtimy,utimy,stimy;


void start_timer();
void stop_timer(int);

void
start_timer_()
{
  start_timer();
}

void
start_timer()
{
  ddd=times(&t1)*1./CLK_TCK;
  nev++;
  return;
}

void
stop_timer_(int *cycle)
{
  stop_timer(*cycle);
}

void
stop_timer(int cycle)
{
  ddd = times(&t2)*1./CLK_TCK - ddd;
  rtimy += ddd;
  utimy += (t2.tms_utime - t1.tms_utime)*1./CLK_TCK;
  stimy += (t2.tms_stime - t1.tms_stime)*1./CLK_TCK;
  if (nev%cycle == 0)
   printf("Utime = %7.0f micros  Stime = %7.0f micros  Rtime = %7.0f micros\n",
       utimy/nev*1000000.,stimy/nev*1000000.,rtimy/nev*1000000.);
  return;
}

#else

void
start_timer_()
{
  return;
}
void
start_timer()
{
  return;
}

void
stop_timer_(int *cycle)
{
  return;
}
void
stop_timer(int cycle)
{
  return;
}


#endif





/* 1 usec timer */

#if defined(Linux)

#include <sys/time.h>
#include <unistd.h>

static int nevent = 0;
static struct timeval tv1, tv2;
static unsigned int time1, time2;


void
start_microtimer_()
{
  nevent++;
  gettimeofday(&tv1,NULL);
  time1 = tv1.tv_sec*1000000 + tv1.tv_usec;
  return;
}

void
stop_microtimer_(int *cycle)
{
  gettimeofday(&tv2,NULL);
  time2 += (tv2.tv_sec*1000000 + tv2.tv_usec) - time1;
  if (nevent%(*cycle) == 0)
  {
    printf(">time = %u microseconds\n",time2/nevent);
    nevent = 0;
    time2 = 0;
  }

  return;
}



#else


void
start_microtimer_()
{
  return;
}

void
stop_microtimer_(int *cycle)
{
  return;
}

#endif





void
memset_(int *a, int *c, int *n)
{
  memset((char *)a,(*c),(*n)*sizeof(int));
  return;
}











E 1
