
/* UNIX version only */

#if defined(VXWORKS) || defined(Linux_armv7l)

void
coda_mon()
{
  printf("coda_mon is dummy for VXWORKS\n");
}

#else


/* coda_mon.c - generic program to run monitoring processess */

#include <pthread.h>
#include <dlfcn.h>

#include "rc.h"
#include "da.h"
#include "libdb.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#if ((TRUE-FALSE)==0)
  True and False are defined to the same value. Is C not wonderful?
#endif

typedef int (*IFUNCPTR) ();

typedef struct MONpriv *MONp;
typedef struct MONpriv
{
  int fd;
  int buffer_count;
  int log_EvDumpLevel;
  char output_type[128];
  char *current_file;
  unsigned int output_switch;
  void *mod_id;
  char mod_name[30];
  int  record_length;
  int  split;
  char *filename;
  pthread_t write_thread;
  objClass object;
  IFUNCPTR write_proc;
  IFUNCPTR close_proc;
  IFUNCPTR open_proc;
  int splitnb;
  int nevents;
  int nlongs;
  int nerrors;
  int nend;
  char *runConfig;
} MON_priv;

static int PrestartCount = 0;
extern objClass localobject;
extern char configname[128]; /* coda_component.c */
extern char *session; /* coda_component.c */
#define MON_ERROR 1
#define MON_OK 0

int listSplit1(char *list, int flag,
           int *argc, char argv[LISTARGV1][LISTARGV2]);


#define FILE_OPEN_PROC   0
#define FILE_WRITE_PROC  1
#define FILE_CLOSE_PROC  2
#define FILE_FLUSH_PROC  3

/* Choose DD FIFO Access mode (ASYNC) */
#define USE_THREADED_CODE

/* Define time limit for staying in write loop (seconds) */
#define MON_WRITE_LOOP_TIMEOUT   10


#include <et_private.h> 
#include <time.h> 
#include <sys/times.h>
#define ET_EVENT_ARRAY_SIZE 100
extern char et_name[ET_FILENAME_LENGTH];
static et_stat_id  et_statid;
static et_sys_id   et_sys;
static et_att_id   et_attach;
static int         et_locality, et_init = 0, et_reinit = 0;

/* ET Initialization */    
int
mon_et_initialize(void)
{
  et_statconfig   sconfig;
  et_openconfig   openconfig;
  int             status;
  struct timespec timeout;

  timeout.tv_sec  = 2;
  timeout.tv_nsec = 0;

  /* Normally, initialization is done only once. However, if the ET
   * system dies and is restarted, and we're running on a Linux or
   * Linux-like operating system, then we need to re-initalize in
   * order to reestablish the tcp connection for communication etc.
   * Thus, we must undo some of the previous initialization before
   * we do it again.
   */
  if(et_init > 0)
  {
    /* unmap shared mem, detach attachment, close socket, free et_sys */
    et_forcedclose(et_sys);
  }
  
  printf("mon_et_initialize: start ET stuff\n");

  if(et_open_config_init(&openconfig) != ET_OK)
  {
    printf("ERROR: mon ET init: cannot allocate mem to open ET system\n");
    return MON_ERROR;
  }
  et_open_config_setwait(openconfig, ET_OPEN_WAIT);
  et_open_config_settimeout(openconfig, timeout);
  if(et_open(&et_sys, et_name, openconfig) != ET_OK)
  {
    printf("ERROR: mon ET init: cannot open ET system\n");
    return MON_ERROR;
  }
  et_open_config_destroy(openconfig);

  /* set level of debug output */
  et_system_setdebug(et_sys, ET_DEBUG_ERROR);

  /* where am I relative to the ET system? */
  et_system_getlocality(et_sys, &et_locality);

  et_station_config_init(&sconfig);
  et_station_config_setselect(sconfig,  ET_STATION_SELECT_ALL);
  et_station_config_setblock(sconfig,   ET_STATION_BLOCKING);
  et_station_config_setuser(sconfig,    ET_STATION_USER_MULTI);
  et_station_config_setrestore(sconfig, ET_STATION_RESTORE_OUT);
  et_station_config_setprescale(sconfig,1);

  if((status = et_station_create(et_sys, &et_statid, "TAPE", sconfig)) < 0)
  {
    if (status == ET_ERROR_EXISTS) {
      printf("mon ET init: station exists, will attach\n");
    }
    else
    {
      et_close(et_sys);
      et_station_config_destroy(sconfig);
      printf("ERROR: mon ET init: cannot create ET station (status = %d)\n",
        status);
      return(MON_ERROR);
    }
  }
  et_station_config_destroy(sconfig);

  if (et_station_attach(et_sys, et_statid, &et_attach) != ET_OK) {
    et_close(et_sys);
    printf("ERROR: mon ET init: cannot attach to ET station\n");
    return MON_ERROR;
  }

  et_init++;
  et_reinit = 0;
  printf("mon ET init: ET fully initialized\n");
  return MON_OK;
}


int
MON_constructor()
{
  static MON_priv MONP;

  localobject->privated = (void *) &MONP;

  bzero ((char *) &MONP,sizeof(MONP));

  MONP.split = 512*1024*1024;

  if(codaUpdateStatus("booted") != MON_OK) return(MON_ERROR);

  return(MON_OK);
}

/*********************************************************/
/*********************************************************/
/*********************************************************/

int
codaDownload(char *conf)
{
  objClass object = localobject;

  MONp monp = (MONp) object->privated;
  int deflt = 0;
  static char tmp[1000];
  static char tmp2[1000];
  int  ix;  
  int  listArgc;
  char listArgv[LISTARGV1][LISTARGV2];

  MYSQL *dbsock;
  char tmpp[1000];

  
  monp->object = object;


  /***************************************************/
  /* extract all necessary information from database */


  /*****************************/
  /*****************************/
  /* FROM CLASS (former conf1) */

  strcpy(configname,conf); /* Sergey: save CODA configuration name */

  UDP_start();

  /* connect to database */
  dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));

  sprintf(tmp,"SELECT value FROM %s_option WHERE name='SPLITMB'",
    configname);
  if(dbGetInt(dbsock, tmp, &monp->split)==MON_ERROR)
  {
    monp->split = 2047 << 20;
    printf("cannot get SPLITMB, set monp->split=%d\n",monp->split);
  }
  else
  {
    printf("get monp->split = %d\n",monp->split);
    monp->split = monp->split<<20;
    printf("set monp->split = %d\n",monp->split);
  }

  sprintf(tmp,"SELECT value FROM %s_option WHERE name='RECL'",
    configname);
  if(dbGetInt(dbsock, tmp, &monp->record_length)==MON_ERROR)
  {
    monp->record_length = 32768;
    printf("cannot get RECL, set monp->record_length=%d\n",monp->record_length);
  }
  else
  {
    printf("get monp->record_length = %d\n",monp->record_length);
  }


  sprintf(tmp,"SELECT value FROM %s_option WHERE name='EvDumpLevel'",
    configname);
  if(dbGetInt(dbsock, tmp, &monp->log_EvDumpLevel)==MON_ERROR)
  {
    monp->log_EvDumpLevel = 0;
    printf("cannot get EvDumpLevel, set monp->log_EvDumpLevel=%d\n",monp->log_EvDumpLevel);
  }
  else
  {
    printf("get monp->log_EvDumpLevel = %d\n",monp->log_EvDumpLevel);
  }


  /* do not nned that !!!???
  sprintf(tmp,"SELECT inputs FROM %s WHERE name='%s'",configname,object->name);
  if(dbGetStr(dbsock, tmp, tmpp)==MON_ERROR)
  {
    printf("cannot get 'inputs' from table>%s< for the name>%s<\n",configname,object->name);
  }
  else
  {
    printf("inputs >%s<\n",tmpp);
  }
  */

  sprintf(tmp,"SELECT outputs FROM %s WHERE name='%s'",configname,object->name);
  if(dbGetStr(dbsock, tmp, tmpp)==MON_ERROR)
  {
    printf("cannot get 'outputs' from table>%s< for the name>%s<\n",configname,object->name);
    return(MON_ERROR);
  }
  else
  {
    strcpy(monp->output_type,tmpp);
    printf("get monp->output_type >%s<\n",monp->output_type);
  }

  /* default output to none */
  monp->output_switch = 3;

  if( !strncmp(monp->output_type,"file",4) ) /* output to binary file */
  {
    sprintf(tmp,"SELECT value FROM %s_option WHERE name='dataFile'",configname);
    if(dbGetStr(dbsock, tmp, tmpp)==MON_ERROR)
    {
      printf("cannot get 'dataFile' from table >%s_option<\n",configname);
      return(MON_ERROR);
    }
    else
    {
      monp->filename = strdup(tmpp); /* Sergey: change it to strcpy(monp->filename,tmpp);*/
      printf("get monp->filename >%s<\n",monp->filename);
    }
    monp->output_switch = 1;
  }
  else if( !strncmp(monp->output_type,"debug",5) ) /* debug dump */
  {
    monp->output_switch = 2;
  }
  else if( !strncmp(monp->output_type,"none",4) ) /* output to /dev/null */
  {
    monp->output_switch = 3;
  }
  else if( !strncmp(monp->output_type,"coda",4) ) /* output in CODA format */
  {
    sprintf(tmp,"SELECT value FROM %s_option WHERE name='dataFile'",configname);
    if(dbGetStr(dbsock, tmp, tmpp)==MON_ERROR)
    {
      printf("cannot get 'dataFile' from table >%s_option<\n",configname);
      return(MON_ERROR);
    }
    else
    {
      monp->filename = strdup(tmpp); /* Sergey: change it to strcpy(monp->filename,tmpp);*/
      printf("get monp->filename >%s<\n",monp->filename);
    }
  	printf("coda format will be used\n");
    monp->output_switch = 4;
  }
  else
  {
    printf("invalid monp->output_type >%s<\n",monp->output_type);
    return(MON_ERROR);
  }

  /*****************************/
  /*****************************/
  /*****************************/

  monp->fd = -1;

  if (monp->mod_id) {
    printf("INFO: Unloading module %x\n", monp->mod_id);

#if defined __sun||LINUX
    if (dlclose ((void *) monp->mod_id) != 0)
    {
      printf("ERROR: failed to unload module to decode >%s<\n",monp->mod_name);
      return MON_ERROR;
    }
#else
    printf("WARN: dynamic loading not yet supported on this platform\n");
#endif
  }
  
  printf("INFO: Downloading configuration '%s'\n", configname);

  strcpy(monp->mod_name,"CODA");


  /* Get the list of readout-lists from the database */
  sprintf(tmpp,"SELECT code FROM %s WHERE name='%s'",configname,object->name);
  if(dbGetStr(dbsock, tmpp, tmp)==MON_ERROR) return(MON_ERROR);
printf("++++++======>%s<\n",tmp);


  /* disconnect from database */
  dbDisconnect(dbsock);


  /* Decode configuration string */
  listArgc = 0;
  if(!((strcmp (tmp, "{}") == 0)||(strcmp (tmp, "") == 0)))
  {
    if(listSplit1(tmp, 1, &listArgc, listArgv)) return(MON_ERROR);
    for(ix=0; ix<listArgc; ix++) printf("nrols [%1d] >%s<\n",ix,listArgv[ix]);
  }
  else
  {
    printf("download: do not split list >%s<\n",tmp);
  }


  /* Get object filename in order to find the ROLs __init routine */
  if(listArgc)
  {
    strcpy(monp->mod_name,listArgv[0]);

    /* Load the decode module */
    sprintf(tmp,"%s/%s_file.so",getenv("CODA_LIB"),listArgv[0]);
#if defined __sun||LINUX
    if (monp->mod_id) dlclose(monp->mod_id);
    monp->mod_id = dlopen ((const char *) tmp, RTLD_NOW | RTLD_GLOBAL);
    if (monp->mod_id == 0) {
      printf("WARN: dlopen failed to open >%s<\n",tmp);
      printf("WARN: >%s<\n",dlerror());
      deflt = 1;
    }
#else
    printf("WARN: dynamic loading not supported\n");
    deflt = 1;
#endif
    
  }
  else
  {
    deflt = 1;
    printf("WARN: row %s table %s no code entry, use CODA file fmt.\n",
      object->name,configname);
  }


  /******************************************************************/
  /* Now look up the routines in the library and fill in the tables */
  if(deflt)
  {
    /* default to CODA format */
    printf("INFO: Using inbuilt (CODA) format\n");
	/*
    monp->open_proc = CODA_open_file;
    monp->close_proc = CODA_close_file;
    monp->write_proc = CODA_write_event;
    */
  }
  else
  {
    IFUNCPTR proc;
    /* find input formatting procs */
    sprintf(tmp,"%s_open_file",monp->mod_name);
    proc = (IFUNCPTR) dlsym (monp->mod_id, tmp);
    monp->open_proc = proc;
    sprintf(tmp,"%s_close_file",monp->mod_name);
    proc = (IFUNCPTR) dlsym (monp->mod_id, tmp);
    monp->close_proc = proc;
    sprintf(tmp,"%s_write_event",monp->mod_name);
    proc = (IFUNCPTR) dlsym (monp->mod_id, tmp);
    monp->write_proc = proc;

    printf("INFO: Loaded module for format %s\n",monp->mod_name);
  }

  /* If we need to initialize, reinitialize, or
   * if et_alive fails on Linux, then initialize.
   */
  if( (et_init == 0)   ||
      (et_reinit == 1) ||
      ((!et_alive(et_sys)) && (et_locality == ET_LOCAL_NOSHARE))
     )
  {
    if(mon_et_initialize() != MON_OK)
    {
      printf("ERROR: mon download: cannot initalize ET system\n");
      return(MON_ERROR);
    }
  }

  codaUpdateStatus("downloaded");

  return(MON_OK);
}


int
codaPrestart()
{
  MYSQL *dbsock;
  char tmpp[1000];

  objClass object = localobject;

  MONp monp = (MONp) object->privated;

  printf("INFO: Prestarting\n");

  monp->nend = 1;

  /* connect to database */
  dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));

  /* Get the run number */
  sprintf(tmpp,"SELECT runNumber FROM sessions WHERE name='%s'",session);
  if(dbGetInt(dbsock,tmpp,&(object->runNumber))==MON_ERROR) return(MON_ERROR);

  /* DO WE NEED TO GET runType AS WELL ??? */

  /* disconnect from database */
  dbDisconnect(dbsock);

  printf("INFO: prestarting,run %d, type %d\n",
    object->runNumber, object->runType);

  PrestartCount = 0;
  object->nevents = 0;
  object->nlongs = 0;

  codaUpdateStatus("paused");

  return(MON_OK);
}

int
codaGo()
{
  codaUpdateStatus("active");
  return(MON_OK);
}

int
codaEnd()
{
  codaUpdateStatus("downloaded");
  return(MON_OK);  
}

int
codaPause()
{
  codaUpdateStatus("paused");
  return(MON_OK);
}

int
codaExit()
{
  codaUpdateStatus("configured");
  return(MON_OK);
}



/****************/
/* main program */
/****************/

void
main(int argc, char **argv)
{
  CODA_Init(argc, argv);
  MON_constructor();
  CODA_Execute ();
}

#endif
