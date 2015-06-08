#define CODA_OK 0
/* coda_erc.c - CODA format */

#if defined(VXWORKS) || defined(Linux_armv7l)

void
coda_er()
{
  printf("coda_er is dummy for VXWORKS\n");
}

#else

/*---------------------------------------------------------------------------*
 *  Copyright (c) 1991, 1992  Southeastern Universities Research Association,*
 *                            Continuous Electron Beam Accelerator Facility  *
 *                                                                           *
 *    This software was developed under a United States Government license   *
 *    described in the NOTICE file included as part of this distribution.    *
 *                                                                           *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606*
 *      heyes@cebaf.gov   Tel: (804) 249-7030    Fax: (804) 249-7363         *
 *---------------------------------------------------------------------------*
 * Discription: follows this header.
 *
 * Author:
 *	Graham Heyes
 *	CEBAF Data Acquisition Group
 *
 *----------------------------------------------------------------------------*/

/* INCLUDES */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#ifdef Linux
#include <linux/prctl.h>
#endif
#if defined __sun||LINUX
#include <dlfcn.h>
#endif
#ifdef Linux
#include <unistd.h> /* for usleep() */
#endif

#include "rc.h"
#include "da.h"
#include "libdb.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif



typedef int (*IFUNCPTR) ();

typedef struct ERpriv *ERp;
typedef struct ERpriv
{
  int fd;
  int buffer_count;
  int log_EvDumpLevel_obsolete;
  char output_type_obsolete[128];
  char current_file[256];
  unsigned int output_switch_obsolete;
  void *mod_id;
  char mod_name[30];
  int  record_length;
  int  split;
  char filename[128];
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
} ER_priv;

static int force_exit = 0;
static int PrestartCount = 0;
/*static*/extern objClass localobject;
extern char configname[128]; /* coda_component.c */
extern char *session; /* coda_component.c */
#define ER_ERROR 1
#define ER_OK 0

int listSplit1(char *list, int flag,
           int *argc, char argv[LISTARGV1][LISTARGV2]);

#define FILE_OPEN_PROC   0
#define FILE_WRITE_PROC  1
#define FILE_CLOSE_PROC  2
#define FILE_FLUSH_PROC  3


/* Define time limit for staying in write loop (seconds) */
#define ER_WRITE_LOOP_TIMEOUT   10



#define MAXBUF 100000
static unsigned int hpsbuf[MAXBUF];

/****************************************************************************/
/***************************** tcpServer functions **************************/

static int tcpState = DA_UNKNOWN;

void
rocStatus()
{
  /*
  printf("%d \n",tcpState);
  */
  switch(tcpState)
  {
    case DA_UNKNOWN:
      printf("unknown\n");
      break;
    case DA_BOOTING:
      printf("booting\n");
      break;
    case DA_BOOTED:
      printf("booted\n");
      break;
    case DA_CONFIGURING:
      printf("initing\n");
      break;
    case DA_CONFIGURED:
      printf("initied\n");
      break;
    case DA_DOWNLOADING:
      printf("loading\n");
      break;
    case DA_DOWNLOADED:
      printf("loaded\n");
      break;
    case DA_PRESTARTING:
      printf("prestarting\n");
      break; 
    case DA_PAUSED:
      printf("paused\n");
      break;
    case DA_PAUSING:
      printf("pausing\n");
      break;
    case DA_ACTIVATING:
      printf("activating\n");
      break;
    case DA_ACTIVE:
      printf("active\n");
      break;
    case DA_ENDING:
      printf("ending\n");
      break;
    case  DA_VERIFYING:
      printf("verifying\n");
      break;
    case DA_VERIFIED:
      printf("verified\n");
      break;
    case DA_TERMINATING:
      printf("terminating\n");
      break;
    case DA_PRESTARTED:
      printf("prestarted\n");
      break;
    case DA_RESUMING:
      printf("resuming\n");
      break;
    case DA_STATES:
      printf("states\n");
      break;
    default:
      printf("unknown\n");
  }
}


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/






#include <et_private.h> 
#include <time.h> 
#include <sys/times.h>

#define ET_EVENT_ARRAY_SIZE 100

extern char et_name[ET_FILENAME_LENGTH];
static et_stat_id  et_statid;

/*???*/
/*static*/ et_sys_id   et_sys;
/*static*/ et_att_id   et_attach;

static int         et_locality, et_init = 0, et_reinit = 0;

/* ET Initialization */    
int
er_et_initialize(void)
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
  
  printf("er_et_initialize: start ET stuff\n");

  if(et_open_config_init(&openconfig) != ET_OK)
  {
    printf("ERROR: er ET init: cannot allocate mem to open ET system\n");
    return ER_ERROR;
  }
  et_open_config_setwait(openconfig, ET_OPEN_WAIT);
  et_open_config_settimeout(openconfig, timeout);
  if(et_open(&et_sys, et_name, openconfig) != ET_OK)
  {
    printf("ERROR: er ET init: cannot open ET system\n");
    return ER_ERROR;
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
      printf("er ET init: station exists, will attach\n");
    }
    else
    {
      et_close(et_sys);
      et_station_config_destroy(sconfig);
      printf("ERROR: er ET init: cannot create ET station (status = %d)\n",
        status);
      return(ER_ERROR);
    }
  }
  et_station_config_destroy(sconfig);

  if (et_station_attach(et_sys, et_statid, &et_attach) != ET_OK) {
    et_close(et_sys);
    printf("ERROR: er ET init: cannot attached to ET station\n");
    return ER_ERROR;
  }

  et_init++;
  et_reinit = 0;
  printf("er ET init: ET fully initialized\n");
  return ER_OK;
}



int
ER_constructor()
{
  static ER_priv ERP;

  localobject->privated = (void *) &ERP;

  bzero ((char *) &ERP,sizeof(ERP));

  ERP.split = 512*1024*1024;

  tcpState = DA_BOOTED;
  if(codaUpdateStatus("booted") != ER_OK) return(ER_ERROR);

  tcpServer(localobject->name); /*start server to process non-coda commands sent by tcpClient*/

  return(ER_OK);
}





/*********************************************************/
/*********************************************************/
/******************** CODA format ************************/

int
CODA_open_file(ERp erp)
{
  objClass object = localobject;

  int tmp;
  int stat=0;

  erp->nlongs = 0;
  erp->nevents = 0;
  erp->splitnb = 0;

  printf("par1: >%s<\n",erp->filename);
  printf("par2: %d\n",object->runNumber);
  printf("par3: %d\n",erp->splitnb);
  printf("befor: >%s<\n",erp->current_file);
  sprintf(erp->current_file, "%s_%06d.evio.%d", erp->filename, object->runNumber, erp->splitnb);
  printf("after: >%s<\n",erp->current_file);
  printf("coda_er: opening file >%s<\n",erp->current_file);
  
  erp->fd = 0;
  stat = evOpen(erp->current_file,"w",&erp->fd);
  if (stat)
  {
    char *errstr = strerror(stat);
    printf("ERROR: Unable to open event file - %s : %s\n",
        erp->current_file,errstr);
    return ER_ERROR;
  }
  else
  {
    printf("evOpen(\"%s\",\"w\",%d)\n",erp->current_file,erp->fd);
    tmp = 2047; /*SHOULD GET IT FROM erp->split*/
	/*sergey: my stuff; tmp=1 will force raid partitions check/swap
    evIoctl(erp->fd,"s",&tmp);
    tmp=1;
    evIoctl(erp->fd,"d",&tmp);
    */
  }

  return(ER_OK);
}

int
CODA_close_file(ERp erp)
{
  objClass object = localobject;

  if(erp->fd > 0) evClose(erp->fd);

  return(ER_OK);
}


int 
gotControlEvent(et_event **pe, int size)
{
    int i;
    
    for (i=0; i<size; i++) {
      if ((pe[i]->control[0] == 17) || (pe[i]->control[0] == 20)) {
        return 1;
      }
    }
    return 0;
}

int 
outputEvents(ERp erp, et_event **pe, int start, int stop)
{
  int handle1, i, buflen, len, tmp, status=0;
  unsigned int *buffer;

  /* evio file output */
  for (i=start; i <= stop; i++)
  {
    len = pe[i]->length>>2;
    /*printf("Got event of length %d bytes.\n", len);*/

	erp->object->nlongs += len;
    erp->nlongs += len;
    erp->object->nevents++;
    erp->nevents++;

    status = evOpenBuffer(pe[i]->pdata, MAXBUF, "r", &handle1);
    if(status!=0) {printf("evOpenBuffer returns %d\n",status);return(status);}



	/*
    status = evRead(handle1, hpsbuf, MAXBUF);
    if(status!=0) {printf("evRead returns %d\n",status);return(status);}
    status = evWrite(erp->fd, hpsbuf);
    if(status!=0) {printf("evWrite returns %d\n",status);return(status);}
	*/
    status = evReadNoCopy(handle1, &buffer, &buflen);
    if(status!=0) {printf("evReadNoCopy returns %d\n",status);return(status);}
    status = evWrite(erp->fd, buffer);
    if(status!=0) {printf("evWrite returns %d\n",status);return(status);}



    status = evClose(handle1);
    if(status!=0) {printf("evClose returns %d\n",status);return(status);}


	/*SPLIT !!!!!!!!!!!!!!!!!!!!!!! */	
	if(erp->split && (erp->nlongs >= (erp->split)>>2 ))
    {
	  evClose(erp->fd);
      erp->splitnb++;
	  erp->nlongs  = 0;
      erp->nevents = 0;

      sprintf(erp->current_file, "%s_%06d.evio.%d", erp->filename, erp->object->runNumber, erp->splitnb);

	  printf("coda_er(outputEvents): Opening file : %s\n",erp->current_file);
      erp->fd = 0;
	  evOpen(erp->current_file,"w",&erp->fd);
	  /*sergey: my stuff
      tmp = 2047;
      evIoctl(erp->fd,"s",&tmp);
      */
	}

  }

  /* if error writing coda file */ 
  if (status != 0)
  {
    return(status);
  }   
  
  return(CODA_OK);
}


int 
CODA_write_event(ERp erp)
{
  const int prestartEvent=17, endEvent=20, true=1, false=0;
  int status1, status2;
  int nevents=0, i, ii, status, len, done=false, start, stop;
  et_event *pe[ET_EVENT_ARRAY_SIZE];
  struct timespec waitfor;
  struct tms tms_start, tms_end;
  clock_t start_ticks, stop_ticks;
  
  waitfor.tv_sec  = 0;
  waitfor.tv_nsec = 10000000L; /* .010 sec */
  
  if (!et_alive(et_sys))
  {
    printf("CODA_write_event: not attached to ET system\n");
    et_reinit = 1;
    return(0);
  }

  if( (start_ticks = times(&tms_start)) == -1)
  {
    printf("CODA_write_event: ERROR getting start time\n");
  }

  do
  {
    status1 = et_events_get(et_sys, et_attach, pe, ET_SLEEP,
                            NULL, ET_EVENT_ARRAY_SIZE, &nevents);

    /* if no events or error ... */
    if ((nevents < 1) || (status1 != ET_OK))
    {
      /* if status1 == ET_ERROR_EMPTY or ET_ERROR_BUSY, no reinit is necessary */
      
      /* will wake up with ET_ERROR_WAKEUP only in threaded code */
      if (status1 == ET_ERROR_WAKEUP)
      {
        printf("CODA_write_event: forcing writer to quit read, status = ET_ERROR_WAKEUP\n");
      }
      else if (status1 == ET_ERROR_DEAD)
      {
        printf("CODA_write_event: forcing writer to quit read, status = ET_ERROR_DEAD\n");
        et_reinit = 1;
      }
      else if (status1 == ET_ERROR)
      {
        printf("CODA_write_event: error in et_events_get, status = ET_ERROR \n");
        et_reinit = 1;
      }
      done = true;
    }
    else    /* if we got events ... */
    {
      /* by default (no control events) write everything */
      start = 0;
      stop  = nevents - 1;
      
      /* if we got control event(s) ... */
      if (gotControlEvent(pe, nevents))
      {
        /* scan for prestart and end events */
        for (i=0; i<nevents; i++)
        {
	      if (pe[i]->control[0] == prestartEvent)
          {
	        printf("Got Prestart Event!!\n");
	        /* look for first prestart */
	        if (PrestartCount == 0)
            {
	          /* ignore events before first prestart */
	          start = i;
	          if (i != 0)
              {
	            printf("CODA_write_event: ignoring %d events before prestart\n",i);
	          }
	        }
            PrestartCount++;
	      }
	      else if (pe[i]->control[0] == endEvent)
         {
            erp->nend--;
	        /* ignore events after last end event & quit */
	        if (erp->nend <= 0)
            {
	          stop = i;
	          done = true;
	        }
            printf("Got End event, Need %d more\n",erp->nend);
	      }
        }
      } 
      
      /* write events to output (ignore events before first prestart) */
      if (PrestartCount != 0)
      {
        if (outputEvents(erp, pe, start, stop) != 0)
        {
	      /* error writing coda file so quit */
	      printf("ERROR: Error writing events... Cancel ET read loop!\n");
          done = true;
        }
      }

      /* put et events back into system */
      status2 = et_events_put(et_sys, et_attach, pe, nevents);            
      if (status2 != ET_OK)
      {
	    printf("CODA_write_event: error in et_events_put, status = %i \n",status2);
        et_reinit = 1;
        done = true;
      }	
    }

    if( (stop_ticks = times(&tms_end)) == -1)
    {
      printf("CODA_write_event: ERROR getting stop time\n");
    }
    else
    {
      /*sergey: why HZ ??? it does not defined in Darwin anyway .. */
#ifndef Darwin
      if( ((stop_ticks-start_ticks)/60) > ER_WRITE_LOOP_TIMEOUT)
      {
#else
      if( ((stop_ticks-start_ticks)/CLK_TCK) > ER_WRITE_LOOP_TIMEOUT)
      {
#endif
	    printf("WARN: ER is backed up! This may be causing system deadtime\n");
	    done = true;
      }
    }

  } while(done == false);
  
  if(erp->nend <= 0)
  { 
    return(0);
  }
  else
  {
    return(1);
  }
}

/*********************************************************/
/*********************************************************/
/*********************************************************/




int
codaDownload(char *conf)
{
  objClass object = localobject;

  ERp erp = (ERp) object->privated;
  static char tmp[1000];
  static char tmp2[1000];
  int  ix;  
  int  listArgc;
  char listArgv[LISTARGV1][LISTARGV2];

  MYSQL *dbsock;
  char tmpp[1000];

  
  erp->object = object;


  /***************************************************/
  /* extract all necessary information from database */


  /*****************************/
  /*****************************/
  /* FROM CLASS (former conf1) */

  strcpy(configname,conf); /* Sergey: save CODA configuration name */

  UDP_start();
  tcpState = DA_DOWNLOADING;
  if(codaUpdateStatus("downloading") != ER_OK) return(ER_ERROR);

  /* connect to database */
  dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));

  sprintf(tmp,"SELECT value FROM %s_option WHERE name='SPLITMB'",
    configname);
  if(dbGetInt(dbsock, tmp, &erp->split)==ER_ERROR)
  {
    erp->split = 2047 << 20;
    printf("cannot get SPLITMB, set erp->split=%d Bytes\n",erp->split);
  }
  else
  {
    printf("get erp->split = %d MBytes\n",erp->split);
    erp->split = erp->split<<20;
    printf("set erp->split = %d Bytes\n",erp->split);
  }

  sprintf(tmp,"SELECT value FROM %s_option WHERE name='RECL'",
    configname);
  if(dbGetInt(dbsock, tmp, &erp->record_length)==ER_ERROR)
  {
    erp->record_length = 32768;
    printf("cannot get RECL, set erp->record_length=%d\n",erp->record_length);
  }
  else
  {
    printf("get erp->record_length = %d\n",erp->record_length);
  }


  /* do not need that !!!???
  sprintf(tmp,"SELECT value FROM %s_option WHERE name='EvDumpLevel'",
    configname);
  if(dbGetInt(dbsock, tmp, &erp->log_EvDumpLevel)==ER_ERROR)
  {
    erp->log_EvDumpLevel = 0;
    printf("cannot get EvDumpLevel, set erp->log_EvDumpLevel=%d\n",erp->log_EvDumpLevel);
  }
  else
  {
    printf("get erp->log_EvDumpLevel = %d\n",erp->log_EvDumpLevel);
  }
  */


  /* do not need that !!!???
  sprintf(tmp,"SELECT inputs FROM %s WHERE name='%s'",configname,object->name);
  if(dbGetStr(dbsock, tmp, tmpp)==ER_ERROR)
  {
    printf("cannot get 'inputs' from table>%s< for the name>%s<\n",configname,object->name);
  }
  else
  {
    printf("inputs >%s<\n",tmpp);
  }
  */


  /* do not need that !!!???
  sprintf(tmp,"SELECT outputs FROM %s WHERE name='%s'",configname,object->name);
  if(dbGetStr(dbsock, tmp, tmpp)==ER_ERROR)
  {
    printf("cannot get 'outputs' from table>%s< for the name>%s<\n",configname,object->name);
    return(ER_ERROR);
  }
  */

  /* default output to evio */
  printf("evio format will be used\n");

  sprintf(tmp,"SELECT value FROM %s_option WHERE name='dataFile'",configname);
  if(dbGetStr(dbsock, tmp, tmpp)==ER_ERROR)
  {
    printf("cannot get 'dataFile' from table >%s_option<\n",configname);
    return(ER_ERROR);
  }
  else
  {
    strcpy(erp->filename,tmpp);
    printf("got erp->filename >%s<\n",erp->filename);
  }



  /*****************************/
  /*****************************/
  /*****************************/

  erp->fd = -1;

  printf("INFO: Downloading configuration '%s'\n", configname);

  strcpy(erp->mod_name,"CODA");


  /* Get the list of readout-lists from the database */
  sprintf(tmpp,"SELECT code FROM %s WHERE name='%s'",configname,object->name);
  if(dbGetStr(dbsock, tmpp, tmp)==ER_ERROR) return(ER_ERROR);
printf("++++++======>%s<\n",tmp);


  /* disconnect from database */
  dbDisconnect(dbsock);


  /* Decode configuration string */
  listArgc = 0;
  if(!((strcmp (tmp, "{}") == 0)||(strcmp (tmp, "") == 0)))
  {
    if(listSplit1(tmp, 1, &listArgc, listArgv)) return(ER_ERROR);
    for(ix=0; ix<listArgc; ix++) printf("nrols [%1d] >%s<\n",ix,listArgv[ix]);
  }
  else
  {
    printf("download: do not split list >%s<\n",tmp);
  }

  /******************************************************************/
  /* Now look up the routines in the library and fill in the tables */
  printf("INFO: Using inbuilt (CODA) format\n");
  erp->open_proc = CODA_open_file;
  erp->close_proc = CODA_close_file;
  erp->write_proc = CODA_write_event;

  /* If we need to initialize, reinitialize, or
   * if et_alive fails on Linux, then initialize.
   */
  if( (et_init == 0)   ||
      (et_reinit == 1) ||
      ((!et_alive(et_sys)) && (et_locality == ET_LOCAL_NOSHARE))
     )
  {
    if(er_et_initialize() != ER_OK)
    {
      printf("ERROR: er download: cannot initalize ET system\n");
      return(ER_ERROR);
    }
  }

  tcpState = DA_DOWNLOADED;
  if(codaUpdateStatus("downloaded") != ER_OK) return(ER_ERROR);

  return(ER_OK);
}




/* do we need some sleep in do-while loop ? we are eating whole cpu ... */
void *
er_write_thread(objClass object)
{
  ERp erp;
  int ix, status = 0;

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &ix);
  
  erp = (ERp) object->privated;
  force_exit = 0;
  do
  {
    status = (*(erp->write_proc))(erp,force_exit);

/*
printf("er_write_thread looping .. (%d %d)\n",status,erp->fd);
sleep(1);
*/
    /*usleep(1000);*/ /* sleep for N microsec */

  } while (status && (erp->fd > 0));

  printf("er_write_thread loop ended (%d %d)\n",status,erp->fd);

  pthread_exit(0);
}



int
erDaqCmd(char *param)
{
  objClass object = localobject;

  ERp erp = (ERp) object->privated;
  switch(param[0])
  {
  case 'o': /*open*/
    (*(erp->open_proc))(erp);

    if(erp->fd)
    {
      pthread_attr_t detached_attr;
      pthread_attr_init(&detached_attr);
      pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);
      pthread_attr_setscope(&detached_attr, PTHREAD_SCOPE_SYSTEM);
      pthread_create( &erp->write_thread, &detached_attr,
                      (void *(*)(void *)) er_write_thread, (void *) object);
    }else{
      printf("No output (erp->fd = %d)\n",erp->fd);
      return ER_ERROR;
    }
    break;

  case 'c': /*close*/
    {
      void *status;
      printf("waiting for write thread 1 ..\n");fflush(stdout);

      et_wakeup_attachment(et_sys, et_attach);
      /*pthread_cancel(erp->write_thread);*/

      printf("waiting for write thread 2 ..\n");fflush(stdout);

/*Sergey: sometimes stuck here, for example if EB crashed at Prestart
and operator hit Reset (ER prestart was Ok) */

/* to prevent that set force_exit=1 to tell writer that it has to exit */
      force_exit = 1;

      pthread_join(erp->write_thread, &status);

      printf("write thread is done\n");fflush(stdout);

      if(erp->close_proc)
      {
        (*(erp->close_proc))(erp);
        printf("force shutdown of write thread\n");
      }
      erp->fd = -1;

/* restore force_exit - just in case */
      force_exit = 0;

    }
    break;

  case 'r': /*resume*/
    if(PrestartCount > 0)
    {
      erp->nend = PrestartCount;
    }
    else
    {
      erp->nend = 1;
    }
    printf("INFO: ER will require %d End event(s) to close output\n",erp->nend);
    break;

  case 'p': /*pause*/
    break;

  default:
    printf("erDaqCmd: unknown param=>%s<\n",param);
  }
  return(ER_OK);
}


int
codaPrestart()
{
  MYSQL *dbsock;
  char tmpp[1000];

  objClass object = localobject;

  ERp erp = (ERp) object->privated;

  printf("INFO: Prestarting\n");

  erFlushET();
  erp->nend = 1;

  /* connect to database */
  dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));

  /* Get the run number */
  sprintf(tmpp,"SELECT runNumber FROM sessions WHERE name='%s'",session);
  if(dbGetInt(dbsock,tmpp,&(object->runNumber))==ER_ERROR) return(ER_ERROR);

  /* DO WE NEED TO GET runType AS WELL ??? */

  /* disconnect from database */
  dbDisconnect(dbsock);

  printf("INFO: prestarting,run %d, type %d\n",
    object->runNumber, object->runType);

  PrestartCount = 0;
  object->nevents = 0;
  object->nlongs = 0;

  erDaqCmd("open");

  tcpState = DA_PAUSED;
  codaUpdateStatus("paused");

  return(ER_OK);
}

int
erFlushET()
{
  objClass object = localobject;

  ERp erp = (ERp) object->privated;
  int stat1 = 0;
  int stat2 = 0;
  int evCnt = 0;

  et_event *pe[ET_EVENT_ARRAY_SIZE];
  int nevents=0, try=0, try_max=5;

  /* Don't flush ET system if there is a non-null output descripter */
  if (erp->fd > 0) {
    printf("ER has valid output channel - Cannot flush ET\n");
    return 0;
  }

  /* - wait for ET system here since this is first item in prestart
   * - wait 5 sec min or 2X monitor period
   */
  if (!et_alive(et_sys)) {
    int waitforET = 2*(ET_MON_SEC + 1);

    /* if this is Linux ... */
    if (et_locality == ET_LOCAL_NOSHARE) {
      /* The ET system has died and our tcp connection has
       * been broken as evidenced by the failure of et_alive.
       * So reinitialize ET to reestablish the link.
       */
      if (er_et_initialize() != ER_OK) {
        printf("ERROR: ER_flush: cannot reinitalize ET system\n");
        return 0;
      }
    }
    /* if Solaris, wait for ET system to come back */
    else {
      if (waitforET < 5) waitforET = 5;
      sleep(waitforET);
      if (!et_alive(et_sys)) {
	printf("ER_flush: ET system is dead\n");
	return 0;
      }
    }
  }



  do
  {
    stat1 = et_events_get(et_sys, et_attach, pe, ET_ASYNC,
                          NULL, ET_EVENT_ARRAY_SIZE, &nevents);

    if(stat1 == ET_OK && (nevents > 0))
    {
      /* put events back */
      stat2 = et_events_put(et_sys, et_attach, pe, nevents);
      printf("ER_flush: flushed %d events from ET system\n", nevents);
    }
    else if(stat1 == ET_ERROR_EMPTY)
    {
      printf("ER_flush: no (more) ET events to flush\n");
    }
    else if (stat1 == ET_ERROR_BUSY)
    {
      printf("ER_flush: ET is busy, wait and try again\n");
      sleep(1);
      if(try++ < try_max)
      {
        stat1 = ET_OK;
      }
      else
      {
        printf("ER_flush: ET station is too busy, can't flush\n");
      }
    }
    else
    {
      printf("ER_flush: error in getting ET events\n");
      et_reinit = 1;
      return(0);
    }
  } while(stat1 == ET_OK);

  return(ER_OK);
}

int
codaGo()
{
  erDaqCmd("resume");
  tcpState = DA_ACTIVE;
  codaUpdateStatus("active");
  return(ER_OK);
}

int
codaEnd()
{
  erDaqCmd("close");
  tcpState = DA_DOWNLOADED;
  codaUpdateStatus("downloaded");
  return(ER_OK);  
}

int
codaPause()
{
  erDaqCmd("pause");
  tcpState = DA_PAUSED;
  codaUpdateStatus("paused");
  return(ER_OK);
}

int
codaExit()
{
  /*Sergey: if end failed, must do it here !!!*/erDaqCmd("close");
  tcpState = DA_CONFIGURED;
  codaUpdateStatus("configured");
  /*
  UDP_reset();
  */
  return(ER_OK);
}


/****************/
/* main program */
/****************/

void
main (int argc, char **argv)
{
  CODA_Init(argc, argv);

ER_constructor();

  /* CODA_Service ("ROC"); */
  CODA_Execute ();
}

#endif
