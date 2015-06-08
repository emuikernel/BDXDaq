/*
 * a timing utilities library
 *
 * Requires 64bit integers to work.
 *
 * @(#)lib_timing.c 1.34 lm@lm.bitmover.com
 *
 * Copyright (c) 1994-1998 Larry McVoy.
 */
#define	 _LIB			/* bench.h needs this */
#include "bench.h"

#define	nz(x)	((x) == 0 ? 1 : (x))

/*
 * I know you think these should be 2^10 and 2^20, but people are quoting
 * disk sizes in powers of 10, and bandwidths are all power of ten.
 * Deal with it.
 */
#define	MB	(1000*1000.0)
#define	KB	(1000.0)

static struct timeval start_tv, stop_tv;
FILE *ftiming;
uint64 use_result_dummy;	/* !static for optimizers. */
static uint64 iterations;
static void init_timing (void);

#if defined(hpux) || defined(__hpux)
#include <sys/mman.h>
#endif

void (*sysTimestampResetRtn) () = NULL;

#ifdef	RUSAGE
#include <sys/resource.h>
#define	SECS(tv)	(tv.tv_sec + tv.tv_usec / 1000000.0)
#define	mine(f)		(int)(ru_stop.f - ru_start.f)


static struct rusage ru_start, ru_stop;

void
rusage (void)
{
  double sys, user, idle;
  double per;

  sys = SECS (ru_stop.ru_stime) - SECS (ru_start.ru_stime);
  user = SECS (ru_stop.ru_utime) - SECS (ru_start.ru_utime);
  idle = timespent () - (sys + user);
  per = idle / timespent () * 100;
  if (!ftiming)
    ftiming = stderr;
  printf ("real=%.2f sys=%.2f user=%.2f idle=%.2f stall=%.0f%% ",
	  timespent (), sys, user, idle, per);
  printf ("rd=%d wr=%d min=%d maj=%d ctx=%d\n",
	  mine (ru_inblock), mine (ru_oublock),
	  mine (ru_minflt), mine (ru_majflt),
	  mine (ru_nvcsw) + mine (ru_nivcsw));
}

#endif /* RUSAGE */
/*
 * Redirect output someplace else.
 */
void
timing (FILE * out)
{
  ftiming = out;
}

/*
 * Start timing now.
 */
void
start (struct timeval *tv)
{
  if (tv == NULL)
    {
      tv = &start_tv;
    }
#ifdef	RUSAGE
  getrusage (RUSAGE_SELF, &ru_start);
#endif
  if (sysTimestampResetRtn)
    sysTimestampResetRtn ();
  /*(void) */ gettimeofday_lmbench (tv, (struct timezone *) 0);
}

/*
 * Stop timing and return real time in microseconds.
 */
uint64 stop (struct timeval *begin, struct timeval *end)
{
  if (end == NULL)
    {
      end = &stop_tv;
    }
  /*(void) */ gettimeofday_lmbench (end, (struct timezone *) 0);
#ifdef	RUSAGE
  getrusage (RUSAGE_SELF, &ru_stop);
#endif

  if (begin == NULL)
    {
      begin = &start_tv;
    }
  return tvdelta (begin, end);
}

uint64 now (void)
{
  struct timeval t;
  uint64 m;

  /*(void) */ gettimeofday_lmbench (&t, (struct timezone *) 0);
  m = t.tv_sec;
  m *= 1000000;
  m += t.tv_usec;
  return (m);
}

double
Now (void)
{
  struct timeval t;

  /*(void) */ gettimeofday_lmbench (&t, (struct timezone *) 0);
  return (t.tv_sec * 1000000.0 + t.tv_usec);
}

uint64 delta (void)
{
  static struct timeval last;
  struct timeval t;
  struct timeval diff;
  uint64 m;

  /*(void) */ gettimeofday_lmbench (&t, (struct timezone *) 0);
  if (last.tv_usec)
    {
      tvsub (&diff, &t, &last);
      last = t;
      m = diff.tv_sec;
      m *= 1000000;
      m += diff.tv_usec;
      return (m);
    }
  else
    {
      last = t;
      return (0);
    }
}

double
Delta (void)
{
  struct timeval t;
  struct timeval diff;

  /*(void) */ gettimeofday_lmbench (&t, (struct timezone *) 0);
  tvsub (&diff, &t, &start_tv);
  return (diff.tv_sec + diff.tv_usec / 1000000.0);
}

void
save_n (uint64 n)
{
  iterations = n;
}

uint64 get_n (void)
{
  return (iterations);
}

/*
 * Make the time spend be usecs.
 */
void
lmbench_settime (uint64 usecs)
{
  bzero ((void *) &start_tv, sizeof (start_tv));
  stop_tv.tv_sec = usecs / 1000000;
  stop_tv.tv_usec = usecs % 1000000;
}

void
  bandwidth_val
  (uint64 bytes,
   uint64 times,
   struct timeval *tStart, struct timeval *tEnd, double *mb, double *bw)
{
  struct timeval tdiff;
  double secs;

  tvsub (&tdiff, tEnd, tStart);
  secs = tdiff.tv_sec;
  secs *= 1000000;
  secs += tdiff.tv_usec;
  secs /= 1000000;
  secs /= times;

  *mb = bytes / MB;
  *bw = *mb / secs;
}

void
bandwidth (uint64 bytes, uint64 times, int verbose)
{
  struct timeval tdiff;
  double mb, secs;

  tvsub (&tdiff, &stop_tv, &start_tv);
  secs = tdiff.tv_sec;
  secs *= 1000000;
  secs += tdiff.tv_usec;
  secs /= 1000000;
  secs /= times;
  mb = bytes / MB;
  if (!ftiming)
    ftiming = stderr;
  if (verbose)
    {
      /*(void) */ printf ("%.4f MB in %.4f secs, %.4f MB/sec\n",
			  mb, secs, mb / secs);
    }
  else
    {
      if (mb < 1)
	{
	  (void) printf ("%.6f ", mb);
	}
      else
	{
	  (void) printf ("%.2f ", mb);
	}
      if (mb / secs < 1)
	{
	  (void) printf ("%.6f\n", mb / secs);
	}
      else
	{
	  (void) printf ("%.2f\n", mb / secs);
	}
    }
}

void
bandwidth_n (uint64 count, uint64 times, int verbose)
{
  struct timeval tdiff;
  double mb = 0, secs = 0;

  tvsub (&tdiff, &stop_tv, &start_tv);
  secs = tdiff.tv_sec;
  secs *= 1000000;
  secs += tdiff.tv_usec;
  secs /= 1000000;
  secs /= (double) times;

  if (verbose)
    {
      (void) printf ("%ld in %.4f secs, %.4f /sec\n",
		     count, secs, ((double) count) / secs);
    }
  else
    {
      if (mb < 1)
	{
	  (void) printf ("%.6f ", mb);
	}
      else
	{
	  (void) printf ("%.2f ", mb);
	}
      if (mb / secs < 1)
	{
	  (void) printf ("%.6f\n", mb / secs);
	}
      else
	{
	  (void) printf ("%.2f\n", mb / secs);
	}
    }
}

void
kb (uint64 bytes)
{
  struct timeval td;
  double s, bs;

  tvsub (&td, &stop_tv, &start_tv);
  s = td.tv_sec + td.tv_usec / 1000000.0;
  bs = bytes / nz (s);
  if (!ftiming)
    ftiming = stderr;
  (void) printf ("%.0f KB/sec\n", bs / KB);
}

void
mb (uint64 bytes)
{
  struct timeval td;
  double s, bs;

  tvsub (&td, &stop_tv, &start_tv);
  s = td.tv_sec + td.tv_usec / 1000000.0;
  bs = bytes / nz (s);
  if (!ftiming)
    ftiming = stderr;
  (void) printf ("%.2f MB/sec\n", bs / MB);
}

void
latency (uint64 xfers, uint64 size)
{
  struct timeval td;
  double s;

  if (!ftiming)
    ftiming = stderr;
  tvsub (&td, &stop_tv, &start_tv);
  s = td.tv_sec + td.tv_usec / 1000000.0;
  if (xfers > 1)
    {
      printf ("%d %dKB xfers in %.2f secs, ",
	      (int) xfers, (int) (size / KB), s);
    }
  else
    {
      printf ("%.1fKB in ", size / KB);
    }
  if ((s * 1000 / xfers) > 100)
    {
      printf ("%.0f millisec%s, ",
	      s * 1000 / xfers, xfers > 1 ? "/xfer" : "s");
    }
  else
    {
      printf ("%.4f millisec%s, ",
	      s * 1000 / xfers, xfers > 1 ? "/xfer" : "s");
    }
  if (((xfers * size) / (MB * s)) > 1)
    {
      printf ("%.2f MB/sec\n", (xfers * size) / (MB * s));
    }
  else
    {
      printf ("%.2f KB/sec\n", (xfers * size) / (KB * s));
    }
}

void
context (uint64 xfers)
{
  struct timeval td;
  double s;

  tvsub (&td, &stop_tv, &start_tv);
  s = td.tv_sec + td.tv_usec / 1000000.0;
  if (!ftiming)
    ftiming = stderr;
  fprintf (ftiming,
	   "%d context switches in %.2f secs, %.0f microsec/switch\n",
	   (int) xfers, s, s * 1000000 / xfers);
}

void
nano (char *s, uint64 n)
{
  struct timeval td;
  double micro;

  tvsub (&td, &stop_tv, &start_tv);
  micro = td.tv_sec * 1000000 + td.tv_usec;
  micro *= 1000;
  if (!ftiming)
    ftiming = stderr;
  printf ("%s: %.0f nanoseconds\n", s, micro / n);
}

void
micro (char *s, uint64 n)
{
  struct timeval td;
  double micro;

  tvsub (&td, &stop_tv, &start_tv);
  micro = td.tv_sec * 1000000 + td.tv_usec;
  micro /= n;
  if (!ftiming)
    ftiming = stderr;
  printf ("%s: %.4f microseconds\n", s, micro);
#if 0
  if (micro >= 100)
    {
      printf ("%s: %.1f microseconds\n", s, micro);
    }
  else if (micro >= 10)
    {
      printf ("%s: %.3f microseconds\n", s, micro);
    }
  else
    {
      printf ("%s: %.4f microseconds\n", s, micro);
    }
#endif
}

void
micromb (uint64 sz, uint64 n)
{
  struct timeval td;
  double mb, micro;

  tvsub (&td, &stop_tv, &start_tv);
  micro = td.tv_sec * 1000000 + td.tv_usec;
  micro /= n;
  mb = sz;
  mb /= MB;
  if (!ftiming)
    ftiming = stderr;
  if (micro >= 10)
    {
      printf ("%.6f %.0f\n", mb, micro);
    }
  else
    {
      printf ("%.6f %.3f\n", mb, micro);
    }
}

void
milli (char *s, uint64 n)
{
  struct timeval td;
  uint64 milli;

  tvsub (&td, &stop_tv, &start_tv);
  milli = td.tv_sec * 1000 + td.tv_usec / 1000;
  milli /= n;
  if (!ftiming)
    ftiming = stderr;
  printf ("%s: %d milliseconds\n", s, (int) milli);
}

void
ptime (uint64 n)
{
  struct timeval td;
  double s;

  tvsub (&td, &stop_tv, &start_tv);
  s = td.tv_sec + td.tv_usec / 1000000.0;
  if (!ftiming)
    ftiming = stderr;
  fprintf (ftiming,
	   "%d in %.2f secs, %.0f microseconds each\n",
	   (int) n, s, s * 1000000 / n);
}

uint64 tvdelta (struct timeval *start, struct timeval *stop)
{
  struct timeval td;
  uint64 usecs;

  tvsub (&td, stop, start);
  usecs = td.tv_sec;
  usecs *= 1000000;
  usecs += td.tv_usec;
  return (usecs);
}

void
tvsub (struct timeval *tdiff, struct timeval *t1, struct timeval *t0)
{
  tdiff->tv_sec = t1->tv_sec - t0->tv_sec;
  tdiff->tv_usec = t1->tv_usec - t0->tv_usec;
  if (tdiff->tv_usec < 0)
    tdiff->tv_sec--, tdiff->tv_usec += 1000000;
}

uint64 gettime (void)
{
  return (tvdelta (&start_tv, &stop_tv));
}

double
timespent (void)
{
  struct timeval td;

  tvsub (&td, &stop_tv, &start_tv);
  return (td.tv_sec + td.tv_usec / 1000000.0);
}

static char p64buf[10][20];
static int n;

char *
p64 (uint64 big)
{
  char *s = p64buf[n++];

  if (n == 10)
    n = 0;
#ifdef  linux
  {
    int *a = (int *) &big;

    if (a[1])
      {
	sprintf (s, "0x%x%08x", a[1], a[0]);
      }
    else
      {
	sprintf (s, "0x%x", a[0]);
      }
  }
#endif
#ifdef	__sgi
  sprintf (s, "0x%llx", big);
#endif
  return (s);
}

char *
p64sz (uint64 big)
{
  double d = big;
  char *tags = " KMGTPE";
  int t = 0;
  char *s = p64buf[n++];

  if (n == 10)
    n = 0;
  while (d > 512)
    t++, d /= 1024;
  if (d == 0)
    {
      return ("0");
    }
  if (d < 100)
    {
      sprintf (s, "%.4f%c", d, tags[t]);
    }
  else
    {
      sprintf (s, "%.2f%c", d, tags[t]);
    }
  return (s);
}

char
last (char *s)
{
  while (*s++)
    ;
  return (s[-2]);
}

int
bytes (char *s)
{
  int n = atoi (s);

  if ((last (s) == 'k') || (last (s) == 'K'))
    n *= 1024;
  if ((last (s) == 'm') || (last (s) == 'M'))
    n *= (1024 * 1024);
  return (n);
}

void
use_int (int result)
{
  use_result_dummy += result;
}

void
use_pointer (void *result)
{
  use_result_dummy += (int) result;
}

void
insertinit (result_t * r)
{
  int i;

  r->N = 0;
  for (i = 0; i < TRIES; i++)
    {
      r->u[i] = 0;
      r->n[i] = 1;
    }
}

/* biggest to smallest */
void
insertsort (uint64 u, uint64 n, result_t * r)
{
  int i, j;

  for (i = 0; i < r->N; ++i)
    {
      if (u / (double) n > r->u[i] / (double) r->n[i])
	{
	  for (j = r->N; j > i; --j)
	    {
	      r->u[j] = r->u[j - 1];
	      r->n[j] = r->n[j - 1];
	    }
	  break;
	}
    }
  r->u[i] = u;
  r->n[i] = n;
  r->N++;
}

static result_t results;

void
get_results (result_t * r)
{
  *r = results;
}

void
save_results (result_t * r)
{
  results = *r;
  save_median ();
}

void
save_minimum ()
{
  save_n (results.n[results.N - 1]);
  lmbench_settime (results.u[results.N - 1]);
}

void
save_median ()
{
  int i = results.N / 2;
  uint64 u, n;

  if (results.N % 2)
    {
      n = results.n[i];
      u = results.u[i];
    }
  else
    {
      n = (results.n[i] + results.n[i - 1]) / 2;
      u = (results.u[i] + results.u[i - 1]) / 2;
    }
  save_n (n);
  lmbench_settime (u);
}

/*
 * The inner loop tracks bench.h but uses a different results array.
 */
static long *
one_op (register long *p)
{
  BENCH_INNER (p = (long *) *p, 0);
  return (p);
}

static long *
two_op (register long *p, register long *q)
{
  BENCH_INNER (p = (long *) *q;
	       q = (long *) *p, 0);
  return (p);
}

static long *p = (long *) &p;
static long *q = (long *) &q;

double
l_overhead (void)
{
  int i;
  uint64 N_save, u_save;
  static double overhead;
  static int initialized = 0;
  result_t one, two, r_save;

  init_timing ();
  if (initialized)
    return overhead;

  initialized = 1;
  if (getenv ("LOOP_O"))
    {
      overhead = atof (getenv ("LOOP_O"));
    }
  else
    {
      get_results (&r_save);
      N_save = get_n ();
      u_save = gettime ();
      insertinit (&one);
      insertinit (&two);
      for (i = 0; i < TRIES; ++i)
	{
	  use_pointer ((void *) one_op (p));
	  insertsort (gettime () - t_overhead (), get_n (), &one);
	  use_pointer ((void *) two_op (p, q));
	  insertsort (gettime () - t_overhead (), get_n (), &two);
	}
      /*
       * u1 = (n1 * (overhead + work))
       * u2 = (n2 * (overhead + 2 * work))
       * ==> overhead = 2. * u1 / n1 - u2 / n2
       */
      save_results (&one);
      save_minimum ();
      overhead = 2. * gettime () / (double) get_n ();

      save_results (&two);
      save_minimum ();
      overhead -= gettime () / (double) get_n ();

      if (overhead < 0.)
	overhead = 0.;		/* Gag */

      save_results (&r_save);
      save_n (N_save);
      lmbench_settime (u_save);
    }
  return overhead;
}

/*
 * Figure out the timing overhead.  This has to track bench.h
 */
uint64 t_overhead (void)
{
  uint64 N_save, u_save;
  static int initialized = 0;
  static uint64 overhead = 0;
  struct timeval tv;
  result_t r_save;

  init_timing ();
  if (initialized)
    return overhead;

  initialized = 1;
  if (getenv ("TIMING_O"))
    {
      overhead = atof (getenv ("TIMING_O"));
    }
  else if (get_enough (0) <= 50000)
    {
      /* it is not in the noise, so compute it */
      int i;
      result_t r;

      get_results (&r_save);
      N_save = get_n ();
      u_save = gettime ();
      insertinit (&r);
      for (i = 0; i < TRIES; ++i)
	{
	  BENCH_INNER (gettimeofday_lmbench (&tv, 0), 0);
	  insertsort (gettime (), get_n (), &r);
	}
      save_results (&r);
      save_minimum ();
      overhead = gettime () / get_n ();

      save_results (&r_save);
      save_n (N_save);
      lmbench_settime (u_save);
    }
  return overhead;
}

/*
 * Figure out how long to run it.
 * If enough == 0, then they want us to figure it out.
 * If enough is !0 then return it unless we think it is too short.
 */
static int long_enough;
static int compute_enough ();

int
get_enough (int e)
{
  init_timing ();
  return (long_enough > e ? long_enough : e);
}


static void
init_timing (void)
{
  static int done = 0;

  if (done)
    return;
  done = 1;
  long_enough = compute_enough ();
  t_overhead ();
  l_overhead ();
}

typedef long TYPE;

static TYPE **
enough_duration (register long N, register TYPE ** p)
{
#define	ENOUGH_DURATION_TEN(one)	one one one one one one one one one one
  while (N-- > 0)
    {
      ENOUGH_DURATION_TEN (p = (TYPE **) * p;
	);
    }
  return p;
}

static int
duration (int N)
{
  int usecs;
  TYPE *x = (TYPE *) & x;
  TYPE **p = (TYPE **) & x;

  start (0);
  p = enough_duration (N, p);
  usecs = stop (0, 0);
  use_pointer ((void *) p);
  return usecs;
}

/*
 * find the minimum time that work "N" takes in "tries" tests
 */
static int
time_N (long N)
{
  int i;
  result_t r;

  insertinit (&r);
  for (i = 1; i < TRIES; ++i)
    {
      insertsort (duration (N), N, &r);
    }
  save_results (&r);
  save_minimum ();
  return gettime ();
}

/*
 * return the amount of work needed to run "enough" microseconds
 */
static int
find_N (int enough)
{
  int tries;
  static int N = 10000;
  static int usecs = 0;

  if (!usecs)
    usecs = time_N (N);

  for (tries = 0; tries < 10; ++tries)
    {
      if (0.98 * enough < usecs && usecs < 1.02 * enough)
	return N;
      if (usecs < 1000)
	N *= 10;
      else
	{
	  double n = N;

	  n /= usecs;
	  n *= enough;
	  N = n + 1;
	}
      usecs = time_N (N);
    }
  return -1;
}

/*
 * We want to verify that small modifications proportionally affect the runtime
 */
static double test_points[] = { 1.015, 1.02, 1.035 };
static int
test_time (int enough)
{
  int i, N, usecs, expected, baseline;

  if ((N = find_N (enough)) <= 0)
    return 0;

  baseline = time_N (N);

  for (i = 0; i < sizeof (test_points) / sizeof (double); ++i)
    {
      usecs = time_N ((int) ((double) N * test_points[i]));
      expected = (int) ((double) baseline * test_points[i]);
      if (ABS (expected - usecs) / (double) expected > 0.0025)
	return 0;
    }
  return 1;
}


/*
 * We want to find the smallest timing interval that has accurate timing
 */
static int possibilities[] = { 5000, 10000, 50000, 100000 };
static int
compute_enough ()
{
  int i;

  if (getenv ("ENOUGH"))
    {
      return (atoi (getenv ("ENOUGH")));
    }
  for (i = 0; i < sizeof (possibilities) / sizeof (int); ++i)
    {
      if (test_time (possibilities[i]))
	return possibilities[i];
    }

  /* 
   * if we can't find a timing interval that is sufficient, 
   * then use SHORT as a default.
   */
  return SHORT;
}

/*
 * This stuff isn't really lib_timing, but ...
 */
void
morefds (void)
{
#ifdef	RLIMIT_NOFILE
  struct rlimit r;

  getrlimit (RLIMIT_NOFILE, &r);
  r.rlim_cur = r.rlim_max;
  setrlimit (RLIMIT_NOFILE, &r);
#endif
}

void
touch (char *buf, int nbytes)
{
  static int psize;

  if (!psize)
    {
      psize = getpagesize_lmbench ();
    }
  while (nbytes > 0)
    {
      *buf = 1;
      buf += psize;
      nbytes -= psize;
    }
}

#if defined(hpux) || defined(__hpux)
int
getpagesize ()
{
  return sysconf (_SC_PAGE_SIZE);
}
#endif
