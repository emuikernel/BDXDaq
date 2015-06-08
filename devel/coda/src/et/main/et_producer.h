/*----------------------------------------------------------------------------*
 *  Copyright (c) 1998        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Author:  Randy MacLeod
 *    Maintainer: Carl Timmer                                                    *
 *             timmer@jlab.org                   Jefferson Lab, MS-12H        *
 *             Phone: (757) 269-5130             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *
 * Description:
 *      ET system, sample event producer header file.
 *
 *----------------------------------------------------------------------------*/

#ifndef ET_PRODUCER_H
#define ET_PRODUCER_H

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE !FALSE
#endif
#if (TRUE == FALSE)
TRUE == FALSE, This is bad.
#endif

/* #define EV_IN_CHUNK   100 */
#define EV_IN_CHUNK   10
#define EV_SIZE        1000
/* default delay results in 10 events per second. */
#define EV_UDELAY      100000

#define ET_PROD_NAP_SEC        0
#define ET_PROD_NAP_NSEC 1000000

typedef struct opt_struct {
  int               verbose;
  int               ev_size;
  int               ev_in_chunk;
  int               incr_word;
  char             *et_filename;
  int               udelay;
} opt_t;

typedef struct prod_struct {
  int               numloops;
  int               res_nsec;
  et_sys_id         id;
  et_att_id	    att_id;
} prod_conf_t;

typedef struct stats_struct {
  pthread_mutex_t   mutex;
  int               report_period;
  int               ev_in_chunk;
  int               nChunks;
  int               evCount;
#if defined linux || defined __APPLE__
  struct timeval    start;
  struct timeval    end;
#else
  struct timespec   start;
  struct timespec   end;
#endif
} etp_stats_t;

/* prototype for threads. */
static void  *signal_thread(void *arg);
static void  *statistician(void *arg);

prod_conf_t   conf_et_producer(opt_t *opts);
opt_t         parse_command_line_et_pro(int argc, char* argv[]);
int           et_prod_run_et(opt_t options, prod_conf_t conf);

#endif /* ET_PRODUCER_H */
