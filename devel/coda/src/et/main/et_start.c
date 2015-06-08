/*----------------------------------------------------------------------------*
 *  Copyright (c) 1998        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Author:  Carl Timmer                                                    *
 *             timmer@jlab.org                   Jefferson Lab, MS-12H        *
 *             Phone: (757) 269-5130             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *
 * Description:
 *      An example program to show how to start up an Event Transfer system.
 *
 *----------------------------------------------------------------------------*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include "et.h"

#define CODASSIPC

#ifdef CODASSIPC
#include "./cinclude/codassipc.c"
char  project[1000];
char  session[1000];
char  uniquename[1000];


/*mutex-protected event counter*/
static int nevents;
pthread_mutex_t mutex_nevents;
#define SSTIMEINTERVAL 10.0 /* # seconds to sleep between smartsocket transactions */

/* ssipc thread */
static int force_exit;
static void *
ssipc_thread (void *arg)
{
  SSTHREAD *ssarg, ssargin;
  ssarg = (SSTHREAD *)arg;
  while(1)
  {
    pthread_mutex_lock(&mutex_nevents);
    ssarg->rate_rec = ssarg->rate_proc = ((float)nevents)/SSTIMEINTERVAL;
    nevents = 0;
    /*printf("2: rec = %f %f\n",ssarg->rate_rec,ssarg->rate_proc);*/
    /*printf("2: nev = %d %d\n\n",ssarg->nev_rec,ssarg->nev_proc);*/
    pthread_mutex_unlock(&mutex_nevents);

    ssipc_put_args(ssarg);
    ipc_check(SSTIMEINTERVAL);
    ssipc_get_args(&ssargin);
    if(force_exit)
    {
      printf("ssipc_thread: force_exit\n");
      break;
    }
  }
  force_exit = 0;
  return(0);
}

#define STRLEN 256

#endif


int
main(int argc, char **argv)
{  
#ifdef CODASSIPC
  SSTHREAD ssarg;
  pthread_t       sid;
  char ch[STRLEN], *cc;
#endif
  int           c;
  extern char  *optarg;
  extern int    optind;
  int           errflg = 0;
  int           i_tmp;
  
  int           status, sig_num;
  int           et_verbose = ET_DEBUG_NONE;
/*
  sigset_t      sigblockset, sigwaitset;
*/
  et_statconfig sconfig;
  et_sysconfig  config;
  et_stat_id    statid;
  et_sys_id     id;

  /************************************/
  /* default configuration parameters */
  /************************************/
  int   nevents = 2000;               /* total number of events */
  int   event_size = 3000;            /* size of event in bytes */
  char *et_filename = NULL, *dir = NULL;
  char  et_name[ET_FILENAME_LENGTH];
  char  et_user_library[512];
  char  et_nodename[512];

  strcpy(et_nodename,"xxx"); /* set default node name */

  while ((c = getopt(argc, argv, "vn:s:f:m:")) != EOF)
  {
    switch (c) {
    case 'n':
      i_tmp = atoi(optarg);
      if (i_tmp > 0) {
	nevents = i_tmp;
      } else {
	printf("Invalid argument to -n. Must be a positive integer.\n");
	exit(-1);
      }
      break;
      
    case 's':
      i_tmp = atoi(optarg);
      if (i_tmp > 0)
      {
	    event_size = i_tmp;
      }
      else
      {
	    printf("Invalid argument to -s. Must be a positive integer.\n");
	    exit(-1);
      }
      break;
      
    case 'f':
      if (strlen(optarg) >= ET_FILENAME_LENGTH) {
        fprintf(stderr, "ET file name is too long\n");
        exit(-1);
      }
      strcpy(et_name, optarg);
      et_filename = et_name;
      break;

    case 'm':
      if (strlen(optarg) >= 512) {
        fprintf(stderr, "node name is too long\n");
        exit(-1);
      }
      strcpy(et_nodename, optarg);
      printf("et_nodename >%s<\n",et_nodename);
      break;

    case 'v':
      et_verbose = ET_DEBUG_INFO;
      break;
      
    case '?':
      errflg++;
    }
  }

  if (errflg){
    fprintf(stderr, "usage: %s -v [-n events] [-s event_size] [-f file] [-m node]\n", argv[0]);
    exit(2);
  }

  /* Check et_filename */
  if (et_filename == NULL) {
    /* see if env variable SESSION is defined */
    if ( (et_filename = getenv("SESSION")) == NULL ) {
      fprintf(stderr, "No ET file name given and SESSION env variable not defined\n");
      exit(-1);
    }
    /* check length of name */
    if ( (strlen(et_filename) + 12) >=  ET_FILENAME_LENGTH) {
      fprintf(stderr, "ET file name is too long\n");
      exit(-1);
    }
    sprintf(et_name, "%s%s", "/tmp/et_sys_", et_filename);
  }
  
  for ( ; optind < argc; optind++) {
    printf("%s\n", argv[optind]);
  }

  if (et_verbose) {
    printf("et_start: asking for %d byte events.\n", event_size);
    printf("et_start: asking for %d events.\n", nevents);
  }

#ifdef CODASSIPC
  /* always use 'clasprod' for project (what about test setup etc ???????) */
  strcpy(project,"clasprod");

  {
    /* extract 'session' from the tail of 'et_name' */
    strcpy(ch,et_name);
    cc = strrchr(ch,'_'); /* get pointer to the location of last '_' */
    if(cc==NULL)
    {
      printf("must be at least one '_' in 'to_et' - exit\n");
      exit(0);
    }
    strcpy(session,(cc+1));
  }
#endif

  /********************************/
  /* set configuration parameters */
  /********************************/
  
  if (et_system_config_init(&config) == ET_ERROR) {
    printf("et_start: no more memory\n");
    exit(1);
  }
  /* total number of events */
  et_system_config_setevents(config, nevents);

  /* size of event in bytes */
  et_system_config_setsize(config, event_size);

  /* max number of temporary (specially allocated mem) events */
  /* This cannot exceed total # of events                     */
  et_system_config_settemps(config, nevents);

  /* limit on # of stations */
  et_system_config_setstations(config, 10);
  
  /* soft limit on # of attachments (hard limit = ET_ATTACHMENTS_MAX) */
  et_system_config_setattachments(config, 20);
  
  /* soft limit on # of processes (hard limit = ET_PROCESSES_MAX) */
  et_system_config_setprocs(config, 10);



  /* set TCP server port 
  et_system_config_setserverport(config, 11223);
*/
  
  /* add multicast address to listen to  */
  /* et_system_config_addmulticast(config, ET_MULTICAST_ADDR); */


  /************************************/
  /* Sergey: remove old file if exist */
  {
    char txt[512];
    sprintf(txt,"rm -f %s\n",et_name);
    printf("removing file >%s<\n",et_name);
	system(txt);
    sleep(3);
    printf("file >%s< removed\n",et_name);
  }


  
  /* Make sure filename is null-terminated string */
  if (et_system_config_setfile(config, et_name) == ET_ERROR) {
    printf("et_start: bad filename argument\n");
    exit(1);
  }
  
  /*************************/
  /* setup signal handling */
  /*************************/
/*
  sigfillset(&sigblockset);
  status = pthread_sigmask(SIG_BLOCK, &sigblockset, NULL);
  if (status != 0) {
    printf("et_start: pthread_sigmask failure\n");
    exit(1);
  }
  sigemptyset(&sigwaitset);
  sigaddset(&sigwaitset, SIGINT);
*/
  
  /*************************/
  /*    start ET system    */
  /*************************/
  if (et_verbose)
  {
    printf("et_start: starting ET system %s\n", et_name);
  }
  if (et_system_start(&id, config) != ET_OK) {
    printf("et_start: error in starting ET system\n");
    exit(1);
  }

  /* turn on debug messages */
  et_system_setdebug(id, et_verbose);


  /**************************************************************/
  /************** get user library ******************************/
  /**************************************************************/

  dir = getenv("CODA_LIB");
  if(dir == NULL)
  {
    printf("et_start: env var CODA_LIB is not defined - exit\n");
    exit(0); 
  }
  strcpy(et_user_library,dir);
  strcat(et_user_library,"/libet_user.so");
  printf("ET user library >%s< will be used\n",et_user_library);


  /**************************************************************/
  /************** create stations *******************************/
  /**************************************************************/

  if( /*(!strcmp(et_nodename,"daq")) ||*/ (!strcmp(et_nodename,"eb")) )
  {
    /* LEVEL3 station for Level3 Trigger */
    et_station_config_init(&sconfig);
    et_station_config_setselect(sconfig,   ET_STATION_SELECT_ALL);
    et_station_config_setblock(sconfig,    ET_STATION_BLOCKING);
    et_station_config_setuser(sconfig,     ET_STATION_USER_SINGLE);
    et_station_config_setrestore(sconfig,  ET_STATION_RESTORE_OUT);
    et_station_config_setprescale(sconfig, 1);
    et_station_config_setcue(sconfig,      100);
    if((status = et_station_create(id, &statid, "LEVEL3", sconfig)) < 0)
    {
      if(status == ET_ERROR_EXISTS)
      {
        printf("et_start: \"LEVEL3\" station exists\n");
      }
      else
      {
        printf("et_start: cannot create \"LEVEL3\" station, error=%d\n",status);
        exit(1);
      }
    }
    et_station_config_destroy(sconfig);
  }
  if( (!strcmp(et_nodename,"eb")) )
  {
    /* ET2ER station to transfer data to ER node */
    et_station_config_init(&sconfig);
    et_station_config_setselect(sconfig,   ET_STATION_SELECT_ALL);
    et_station_config_setblock(sconfig,    ET_STATION_BLOCKING);
    et_station_config_setuser(sconfig,     ET_STATION_USER_SINGLE);
    et_station_config_setrestore(sconfig,  ET_STATION_RESTORE_OUT);
    et_station_config_setprescale(sconfig, 1);
    et_station_config_setcue(sconfig,      100);
    if((status = et_station_create(id, &statid, "ET2ER", sconfig)) < 0)
    {
      if(status == ET_ERROR_EXISTS)
      {
        printf("et_start: \"ET2ER\" station exists\n");
      }
      else
      {
        printf("et_start: cannot create \"ET2ER\" station, error=%d\n",status);
        exit(1);
      }
    }
    et_station_config_destroy(sconfig);
  }

  if( /*(!strcmp(et_nodename,"daq")) ||*/ (!strcmp(et_nodename,"er")) )
  {
    /* TAPE station for Event Recorder */
    et_station_config_init(&sconfig);
    et_station_config_setselect(sconfig,   ET_STATION_SELECT_ALL);
    et_station_config_setblock(sconfig,    ET_STATION_BLOCKING);
    et_station_config_setuser(sconfig,     ET_STATION_USER_MULTI);
    et_station_config_setrestore(sconfig,  ET_STATION_RESTORE_OUT);
    et_station_config_setprescale(sconfig, 1);
    if((status = et_station_create(id, &statid, "TAPE", sconfig)) < 0)
    {
      if(status == ET_ERROR_EXISTS)
      {
        printf("et_start: \"TAPE\" station exists\n");
      }
      else
      {
        printf("et_start: cannot create \"TAPE\" station, error=%d\n",status);
        exit(1);
      }
    }
    et_station_config_destroy(sconfig);
  }

  if( (!strcmp(et_nodename,"er")) )
  {
    /* ET2ET10 station for Data Transfer */
    et_station_config_init(&sconfig);
    et_station_config_setselect(sconfig,   ET_STATION_SELECT_USER);
    et_station_config_setblock(sconfig,    ET_STATION_BLOCKING);
    et_station_config_setuser(sconfig,     ET_STATION_USER_SINGLE);
    et_station_config_setrestore(sconfig,  ET_STATION_RESTORE_OUT);
    et_station_config_setprescale(sconfig, 1);
    et_station_config_setcue(sconfig,      100);
    et_station_config_setlib(sconfig,      et_user_library);
    et_station_config_setfunction(sconfig, "et2et_function");
    if((status = et_station_create(id, &statid, "ET2ET10", sconfig)) < 0)
    {
      if(status == ET_ERROR_EXISTS)
      {
        printf("et_start: \"ET2ET10\" station exists\n");
      }
      else
      {
        printf("et_start: cannot create \"ET2ET10\" station, error=%d\n",status);
        exit(1);
      }
    }
    et_station_config_destroy(sconfig);
  }

  if( (!strcmp(et_nodename,"daq")) )
  {
    /* ET2ET00 station for Data Transfer */
    et_station_config_init(&sconfig);
    et_station_config_setselect(sconfig,   ET_STATION_SELECT_USER);
    et_station_config_setblock(sconfig,    ET_STATION_BLOCKING);
    et_station_config_setuser(sconfig,     ET_STATION_USER_SINGLE);
    et_station_config_setrestore(sconfig,  ET_STATION_RESTORE_OUT);
    et_station_config_setprescale(sconfig, 1);
    et_station_config_setcue(sconfig,      100);
    et_station_config_setlib(sconfig,      et_user_library);
    et_station_config_setfunction(sconfig, "et2et_function");
    if((status = et_station_create(id, &statid, "ET2ET00", sconfig)) < 0)
    {
      if(status == ET_ERROR_EXISTS)
      {
        printf("et_start: \"ET2ET00\" station exists\n");
      }
      else
      {
        printf("et_start: cannot create \"ET2ET00\" station, error=%d\n",status);
        exit(1);
      }
    }
    et_station_config_destroy(sconfig);
  }

  if( (!strcmp(et_nodename,"daq")) )
  {
    /* ET2ET02 station for Data Transfer */
    et_station_config_init(&sconfig);
    et_station_config_setselect(sconfig,   ET_STATION_SELECT_USER);
    et_station_config_setblock(sconfig,    ET_STATION_BLOCKING);
    et_station_config_setuser(sconfig,     ET_STATION_USER_SINGLE);
    et_station_config_setrestore(sconfig,  ET_STATION_RESTORE_OUT);
    et_station_config_setprescale(sconfig, 1);
    et_station_config_setcue(sconfig,      100);
    et_station_config_setlib(sconfig,      et_user_library);
    et_station_config_setfunction(sconfig, "et2et_function");
    if((status = et_station_create(id, &statid, "ET2ET02", sconfig)) < 0)
    {
      if(status == ET_ERROR_EXISTS)
      {
        printf("et_start: \"ET2ET02\" station exists\n");
      }
      else
      {
        printf("et_start: cannot create \"ET2ET02\" station, error=%d\n",status);
        exit(1);
      }
    }
    et_station_config_destroy(sconfig);
  }

  if( (!strcmp(et_nodename,"daq")) )
  {
    /* SCALER station for ... */
    et_station_config_init(&sconfig);
    et_station_config_setselect(sconfig,   ET_STATION_SELECT_USER);
    et_station_config_setblock(sconfig,    ET_STATION_BLOCKING);
    et_station_config_setuser(sconfig,     ET_STATION_USER_SINGLE);
    et_station_config_setrestore(sconfig,  ET_STATION_RESTORE_OUT);
    et_station_config_setprescale(sconfig, 1);
    et_station_config_setcue(sconfig,      100);
    et_station_config_setlib(sconfig,      et_user_library);
    et_station_config_setfunction(sconfig, "et_scaler_function");
    if((status = et_station_create(id, &statid, "SCALER", sconfig)) < 0)
    {
      if(status == ET_ERROR_EXISTS)
      {
        printf("et_start: \"SCALER\" station exists\n");
      }
      else
      {
        printf("et_start: cannot create \"SCALER\" station, error=%d\n",status);
        exit(1);
      }
    }
    et_station_config_destroy(sconfig);

  }


  /* clon00, to be obsolete, everything is moving to mon0 */
  if( !strcmp(et_nodename,"mon") )
  {
    /* PRLIB for Patter Recognition */
    et_station_config_init(&sconfig);
    /*??et_station_config_setselect(sconfig,   ET_STATION_SELECT_USER);*/
    et_station_config_setblock(sconfig,    ET_STATION_NONBLOCKING);
    et_station_config_setuser(sconfig,     ET_STATION_USER_MULTI);
    et_station_config_setrestore(sconfig,  ET_STATION_RESTORE_OUT);
    et_station_config_setprescale(sconfig, 1);
    et_station_config_setcue(sconfig,      100);
    if((status = et_station_create(id, &statid, "PRLIB", sconfig)) < 0)
    {
      if(status == ET_ERROR_EXISTS)
      {
        printf("et_start: \"PRLIB\" station exists\n");
      }
      else
      {
        printf("et_start: cannot create \"PRLIB\" station, error=%d\n",status);
        exit(1);
      }
    }
    et_station_config_destroy(sconfig);

    /* TRLIB for Patter Recognition */
    et_station_config_init(&sconfig);
    /*??et_station_config_setselect(sconfig,   ET_STATION_SELECT_USER);*/
    et_station_config_setblock(sconfig,    ET_STATION_NONBLOCKING);
    et_station_config_setuser(sconfig,     ET_STATION_USER_MULTI);
    et_station_config_setrestore(sconfig,  ET_STATION_RESTORE_OUT);
    et_station_config_setprescale(sconfig, 1);
    et_station_config_setcue(sconfig,      100);
    if((status = et_station_create(id, &statid, "TRLIB", sconfig)) < 0)
    {
      if(status == ET_ERROR_EXISTS)
      {
        printf("et_start: \"TRLIB\" station exists\n");
      }
      else
      {
        printf("et_start: cannot create \"TRLIB\" station, error=%d\n",status);
        exit(1);
      }
    }
    et_station_config_destroy(sconfig);
  }


  if( !strcmp(et_nodename,"mon0") )
  {
    /* PRLIB for Patter Recognition */
    et_station_config_init(&sconfig);
    /*??et_station_config_setselect(sconfig,   ET_STATION_SELECT_USER);*/
    et_station_config_setblock(sconfig,    ET_STATION_NONBLOCKING);
    et_station_config_setuser(sconfig,     ET_STATION_USER_MULTI);
    et_station_config_setrestore(sconfig,  ET_STATION_RESTORE_OUT);
    et_station_config_setprescale(sconfig, 1);
    et_station_config_setcue(sconfig,      100);
    if((status = et_station_create(id, &statid, "PRLIB", sconfig)) < 0)
    {
      if(status == ET_ERROR_EXISTS)
      {
        printf("et_start: \"PRLIB\" station exists\n");
      }
      else
      {
        printf("et_start: cannot create \"PRLIB\" station, error=%d\n",status);
        exit(1);
      }
    }
    et_station_config_destroy(sconfig);

    /* TRLIB for Patter Recognition */
    et_station_config_init(&sconfig);
    /*??et_station_config_setselect(sconfig,   ET_STATION_SELECT_USER);*/
    et_station_config_setblock(sconfig,    ET_STATION_NONBLOCKING);
    et_station_config_setuser(sconfig,     ET_STATION_USER_MULTI);
    et_station_config_setrestore(sconfig,  ET_STATION_RESTORE_OUT);
    et_station_config_setprescale(sconfig, 1);
    et_station_config_setcue(sconfig,      100);
    if((status = et_station_create(id, &statid, "TRLIB", sconfig)) < 0)
    {
      if(status == ET_ERROR_EXISTS)
      {
        printf("et_start: \"TRLIB\" station exists\n");
      }
      else
      {
        printf("et_start: cannot create \"TRLIB\" station, error=%d\n",status);
        exit(1);
      }
    }
    et_station_config_destroy(sconfig);
  }




  if( !strcmp(et_nodename,"mon1") )
  {
    /* MON station for event monitor */
    et_station_config_init(&sconfig);
    et_station_config_setselect(sconfig,   ET_STATION_SELECT_USER);
    et_station_config_setblock(sconfig,    ET_STATION_BLOCKING);
    et_station_config_setuser(sconfig,     ET_STATION_USER_SINGLE);
    et_station_config_setrestore(sconfig,  ET_STATION_RESTORE_OUT);
    et_station_config_setprescale(sconfig, 1);
    et_station_config_setcue(sconfig,      100);
    et_station_config_setlib(sconfig,      et_user_library);
    et_station_config_setfunction(sconfig, "et_mon_function");
    if((status = et_station_create(id, &statid, "MON", sconfig)) < 0)
    {
      if(status == ET_ERROR_EXISTS)
      {
        printf("et_start: \"MON\" station exists\n");
      }
      else
      {
        printf("et_start: cannot create \"MON\" station, error=%d\n",status);
        exit(1);
      }
    }
    et_station_config_destroy(sconfig);
  }



  if( !strcmp(et_nodename,"mon2") )
  {
    /* CED station for event display */
    et_station_config_init(&sconfig);
    et_station_config_setselect(sconfig,   ET_STATION_SELECT_USER);
    et_station_config_setblock(sconfig,    ET_STATION_NONBLOCKING);
    et_station_config_setuser(sconfig,     ET_STATION_USER_SINGLE);
    et_station_config_setrestore(sconfig,  ET_STATION_RESTORE_OUT);
    et_station_config_setprescale(sconfig, 1);
    et_station_config_setcue(sconfig,      100);
    et_station_config_setlib(sconfig,      et_user_library);
    et_station_config_setfunction(sconfig, "et_ced_function");
    if((status = et_station_create(id, &statid, "CED", sconfig)) < 0)
    {
      if(status == ET_ERROR_EXISTS)
      {
        printf("et_start: \"CED\" station exists\n");
      }
      else
      {
        printf("et_start: cannot create \"CED\" station, error=%d\n",status);
        exit(1);
      }
    }
    et_station_config_destroy(sconfig);
  }

  /**************************************************************/
  /**************************************************************/
#ifdef CODASSIPC

  ssarg.project    = project;
  ssarg.session    = session;
  ssarg.rate_rec   = 0.0; 
  ssarg.rate_proc  = 0.0; 
  ssarg.nev_rec    = 0;
  ssarg.nev_proc   = 0;
  ssarg.et_ok      = 1; /* 1 if connected, 0 if not; we'll always set it 1 */
  ssarg.force_exit = 0;
  ssarg.last_time  = time(NULL);

  /*  set ipc parameters, connect to server, etc. */
  ipc_set_application(ssarg.project);
  ipc_set_quit_callback(ssipc_quit_callback);
  ipc_set_user_status_poll_callback(ssipc_status_poll_callback);
  ipc_set_control_message_callback(ssipc_control_message_callback);


  /* construct unique name */
  ssarg.data_flow = (char *)"et_start";

  strcpy((char *)uniquename,(char *)"et_start_");
  strcat((char *)uniquename,et_nodename);
  strcat((char *)uniquename,(char *)"_");
  strcat((char *)uniquename,(char *)session);

  ssarg.unique_name = uniquename;
  printf("unique name >%s<\n",uniquename);


  /*set unique name*/
  status = ipc_init(ssarg.unique_name,uniquename); /* <=21 char */

  if(status < 0)
  {
    printf("\et_start: Unable to connect to server...probably duplicate unique name\n"
	 "   ...check for another %s connected using ipc_info\n\n",ssarg.unique_name);
    exit(1);
  }
  TipcSrvSubjectSetSubscribe((T_STR)"evt_system",TRUE);
  TipcSrvFlush();


  /* start ssipc_thread */
  nevents = 0;
  if(pthread_create(&sid, NULL, ssipc_thread, (void *)&ssarg) != 0)
  {
    printf("et_start: pthread_create(0x%08x,...) failure\n",sid);
    return(-1);
  }
  else
  {
    printf("et_start: pthread_create(0x%08x,...) done\n",sid);
  }
#endif


  while(1)
  {
    sleep(1);
#ifdef CODASSIPC
    /* fill 'ssarg' structure */
    ssarg.nev_rec   += 0/*ntransferred*/;
    ssarg.nev_proc  += 0/*ntransferred*/;

    pthread_mutex_lock(&mutex_nevents);

    /* just sum here, will normalize to time interval before sending it out */
    nevents += 0/*ntransferred*/;
    /*printf("1: nevents = %d\n",nevents);*/

    pthread_mutex_unlock(&mutex_nevents);

    if(ssarg.force_exit) break;
#endif
  }

#ifdef CODASSIPC
  ipc_close();
#endif

  /* turn this thread into a signal handler */
  /*
  sigwait(&sigwaitset, &sig_num);
  */
  printf("Interrupted by CONTROL-C\n");
  printf("ET is exiting\n");
  /* bugbug If no one attached, remove et file. */

  exit(0);  
}

