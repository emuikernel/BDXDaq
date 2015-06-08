
/* UNIX version only */

#ifndef VXWORKS

#include "ER_class.c"
#include "CODA_class.c"
/*#include "LINK_class.c"*/

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
 * Revision History:
 *      $Log: er_component.c,v $
 *      Revision 2.52  2000/07/07 13:09:36  abbottd
 *      added read/write loop timeout so ER will update status promptly to RunControl
 *
 *      Revision 2.51  2000/02/17 19:36:46  rwm
 *      Fix bug in er_flush.
 *
 *      Revision 2.50  2000/01/14 21:49:49  abbottd
 *      indicate error on evOpen() failure - update counters for debug and none output options
 *
 *      Revision 2.49  1999/12/16 17:25:23  timmer
 *      change ET_ERROR_DETACH to ET_ERROR_DEAD and ET_ERROR_WAKEUP
 *
 *      Revision 2.48  1999/12/10 15:47:14  timmer
 *      reinit ET in prestart for Linux
 *
 *      Revision 2.46  1999/12/09 14:45:18  abbottd
 *      Initialize nevents=0 in flush rotutine
 *
 *      Revision 2.45  1999/11/22 16:42:11  timmer
 *      new ET api changes
 *
 *      Revision 2.44  1999/08/27 14:22:17  timmer
 *      upgrade to new api for et_open
 *
 *      Revision 2.43  1999/07/19 19:10:31  timmer
 *      use latest ET with remote stuff
 *
 *      Revision 2.42  1999/05/20 18:52:59  rwm
 *      Fixed len's and tested in ET/DD with binary/CODA output.
 *
 *      Revision 2.41  1999/05/13 16:46:47  rwm
 *      Fixed bug: binary output with ET, write length was wrong. len is in bytes unless indicated.
 *
 *      Revision 2.40  1999/04/16 19:02:55  timmer
 *      new ET api/capabilities
 *
 *      Revision 2.39  1999/04/06 15:30:02  timmer
 *      handle control events correctly
 *
 *      Revision 2.38  1999/03/03 19:53:51  timmer
 *      fix handling of prestart, end events for et
 *
 *      Revision 2.37  1999/02/09 20:23:03  timmer
 *      another api change
 *
 *      Revision 2.36  1999/02/03 13:19:41  timmer
 *      change static et variables to global
 *
 *      Revision 2.34  1999/01/08 20:09:44  timmer
 *      upgrade to ET version 3.0
 *
 *      Revision 2.33  1998/11/06 17:28:56  timmer
 *      Linux port
 *
 *      Revision 2.32  1998/10/13 15:43:46  abbottd
 *      changed default to  #undef USE_THREADED_CODE
 *
 *      Revision 2.31  1998/09/11 18:24:16  timmer
 *      migrated to ET2.0
 *
 *      Revision 2.30  1998/08/25 19:06:51  rwm
 *      Protected the definition of TRUE and FALSE.
 *
 *      Revision 2.29  1998/08/17 20:07:19  timmer
 *      change NEWDD to WITH_ET
 *
 *      Revision 2.28  1998/08/06 20:23:44  timmer
 *      few more NEWDD changes
 *
 *      Revision 2.27  1998/08/05 18:44:13  timmer
 *      add NEWDD or ET system mods
 *
 *      Revision 2.26  1998/07/27 19:17:53  heyes
 *      added support for xfrag.tcl, turned off evsize debugging in ROC
 *
 *      Revision 2.25  1998/05/27 20:19:24  abbottd
 *      cvs changes 2.20,2.21,2.22 back in again
 *
 *      Revision 2.22  1998/04/21 14:22:21  abbottd
 *      Fixed problems with SPLITMB and opening multiple files during the run
 *
 *      Revision 2.21  1998/04/06 20:15:28  abbottd
 *      Fixed threaded mode and made default, cleaned up flush command
 *
 *      Revision 2.20  1998/04/02 21:55:48  abbottd
 *      Fixed debug output, nlongs calculation, added flush routine for DD system, added multiple end event feature
 *
 *      Revision 2.19  1998/04/02 13:38:31  heyes
 *      modify poll interval
 *
 *      Revision 2.18  1998/03/09 18:56:21  heyes
 *      the fastest EB in the galaxy
 *
 *      Revision 2.17  1998/03/03 19:53:00  heyes
 *      fix requeue for vlad
 *
 *      Revision 2.15  1997/08/04 01:58:07  abbottd
 *          Fixed bug with file output options
 *
 *      Revision 2.14  1997/07/23 14:24:57  heyes
 *      add @ trick to filename parsing
 *
 *      Revision 2.13  1997/06/20 15:31:12  abbottd
 *      fixed problem with runcontrol reset (ER end) when ER is in downloaded state
 *
 *      Revision 2.12  1997/05/19 19:39:49  heyes
 *      fixed ER multi restart problem
 *
 *      Revision 2.11  1997/04/23 17:33:54  heyes
 *      cmlog added and libmsg removed
 *
 *      Revision 2.10  1997/03/19 13:34:50  heyes
 *      add nerrors to ER
 *
 *      Revision 2.9  1997/03/13 16:58:23  heyes
 *      trying to get this stuff in cvs
 *
 *      Revision 2.8  1997/02/27 02:22:01  heyes
 *      remove dd close from ER
 *
 *      Revision 2.7  1997/02/12 15:32:09  heyes
 *      add version method
 *
 *      Revision 2.6  1997/02/12 14:31:29  heyes
 *      tcl_modules variable
 *
 *      Revision 2.5  1997/02/11 20:14:39  heyes
 *      error in coda_component.c
 *
 *      Revision 2.4  1997/02/11 20:02:07  heyes
 *      tedious isn't it...
 *
 *      Revision 2.2  1997/01/16 15:30:40  heyes
 *      Increase speed of EB, inc. changes after Dec run.
 *
 *      Revision 2.1  1996/11/01 13:39:15  heyes
 *      initial revision of event recorder
 *
 *	  Initial revision
 *
 *
 *----------------------------------------------------------------------------*/

/* INCLUDES */

#include <pthread.h>

#if defined __sun||LINUX
#include <dlfcn.h>
#endif

#ifdef Linux
#include <unistd.h> /* for usleep() */
#endif

#include "da.h"
#include "rc.h"
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

#define TCL_PROC(name) int name (objClass object, Tcl_Interp *interp, int argc, char **argv)

typedef int (*IFUNCPTR) ();

typedef struct ERpriv *ERp;
typedef struct ERpriv
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
} ER_priv;

static int force_exit = 0;
static int PrestartCount = 0;
/*static*/ objClass localobject;
extern char configname[128]; /* coda_component.c */
extern char *session; /* coda_component.c */
#define ER_ERROR 1
#define ER_OK 0

int listSplit1(char *list, int flag,
           int *argc, char argv[LISTARGV1][LISTARGV2]);

#ifdef LINK_BOS_FORMAT
extern int FPACK_open_file(ERp erp);
extern int FPACK_close_file(ERp erp);
extern int FPACK_write_event(ERp erp, int);
#endif


#define FILE_OPEN_PROC   0
#define FILE_WRITE_PROC  1
#define FILE_CLOSE_PROC  2
#define FILE_FLUSH_PROC  3

/* Choose DD FIFO Access mode (ASYNC) */
#define USE_THREADED_CODE

/* Define time limit for staying in write loop (seconds) */
#define ER_WRITE_LOOP_TIMEOUT   10






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

#ifdef LINK_BOS_FORMAT
/*static*/ et_sys_id   et_sys;
/*static*/ et_att_id   et_attach;
#else
#ifdef LINK_CODA_FORMAT /*???*/
/*static*/ et_sys_id   et_sys;
/*static*/ et_att_id   et_attach;
#else
static et_sys_id   et_sys;
static et_att_id   et_attach;
#endif
#endif

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



TCL_PROC(ER_constructor)
{
  static ER_priv ERP;

  localobject = object;

  object->private = (void *) &ERP;

  /* tell anyone watching */
 {
    char tmp[400];
    
    sprintf(tmp,"%s {%s} %s {%s}",
	    __FILE__,
	    DAYTIME,
	    CODA_USER,
	    "$Id: er_component.c,v 2.52 2000/07/07 13:09:36 abbottd Exp $");
    Tcl_SetVar (interp, "tcl_modules",tmp,TCL_LIST_ELEMENT|TCL_APPEND_VALUE|TCL_GLOBAL_ONLY);
  }

  bzero ((char *) &ERP,sizeof(ERP));

  if( Tcl_VarEval(interp, "set session ",argv[1], NULL) != ER_OK)
    return ER_ERROR;

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
  
  if (erp->filename[0] == '@') {
    char temp[500];
    printf("Executing file : %s\n",erp->filename);
      sprintf(temp,"%s %d %d $config %d",
	      &erp->filename[1],
	      object->runNumber, 
	      object->runType,
	      erp->splitnb);

/* Sergey: !!!!!!!!!!!!! MUST BE REDONE WITHOUT TCL !!!!!!!!!!!!!!!
    if (Tcl_VarEval(interp,temp,NULL) != ER_OK) {
      daLogMsg ("ERROR", "Filename generation script %s failed",erp->filename);
      return ER_ERROR;
      }
    if (erp->current_file)
      ckfree(erp->current_file);
    erp->current_file = strdup(interp->result);
*/
  } else {
    if (erp->current_file)
      ckfree(erp->current_file);

    erp->current_file = ckalloc((strlen(erp->filename)+100));
    sprintf(erp->current_file, erp->filename, object->runNumber,erp->splitnb);
  }
  
  printf("coda_er: opening file : %s\n",erp->current_file);
  
  if (erp->output_switch == 4) {
    erp->fd = 0;
    stat = evOpen(erp->current_file,"w",&erp->fd);
    if (stat) {
      char *errstr = strerror(stat);
      printf("ERROR: Unable to open event file - %s : %s\n",
        erp->current_file,errstr);
      return ER_ERROR;
    }else{
      printf("evOpen(\"%s\",\"w\",%d)\n",erp->current_file,erp->fd);
      tmp = 2047; /*SHOULD GET IT FROM erp->split*/
      evIoctl(erp->fd,"s",&tmp);
      tmp = 1; /*force raid partitions check/swap*/
      evIoctl(erp->fd,"d",&tmp);
    }
  } else if (erp->output_switch == 1) {
    erp->fd = open(erp->current_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (erp->fd == -1) {
      perror("open failed: ");
      return ER_ERROR;
    }
  }

  return(ER_OK);
}

int
CODA_close_file(ERp erp)
{
  objClass object = localobject;

  if (erp->output_switch == 4) {
    if(erp->fd > 0) {
      evClose(erp->fd);
    }
  } else if (erp->output_switch == 1) {
    if(erp->fd > 0) {
      close(erp->fd);
    }
  }

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
  int i, len, tmp, status=0;
  
  switch(erp->output_switch) {
    default:
    case 0: 
      break;
      
    case 1:
      /* binary file output */
      for (i=start; i <= stop; i++) {

        len = pe[i]->length>>2;
/* 	printf("ER got event of length %d bytes (nlongs = %d).\n", pe[i]->length, len); */

	erp->object->nlongs += len;
        erp->nlongs += len;
        erp->object->nevents++;
        erp->nevents++;

	write(erp->fd, (char *) pe[i]->pdata, pe[i]->length);
	
	if(erp->split && (erp->nlongs >= (erp->split)>>2 ))
    {
	  close(erp->fd);
	  erp->splitnb++;
	  erp->nlongs  = 0;
	  erp->nevents = 0;

	  /* MUST BE REDONE WITHOUT TCL !!!!!!!!!!!!!
	  if(erp->filename[0] == '@')
      {
	    char temp[500];
	    printf("Executing file : %s\n",erp->filename);
	    sprintf(temp,"%s %d %d %s %d",
		    &erp->filename[1],
		    erp->object->runNumber, 
		    erp->object->runType,
		    erp->runConfig,
		    erp->splitnb);
	    if(Tcl_VarEval(erp->object->interp,temp,NULL) != ER_OK)
        {
	      daLogMsg ("ERROR", "Filename generation script %s failed",erp->filename);
	      return ER_ERROR;
	    }
	    if(erp->current_file) ckfree(erp->current_file);
	    erp->current_file = strdup(erp->object->interp->result);
	  }
      else*/
      {
	    char temp[500];
	    sprintf(erp->current_file, erp->filename, erp->object->runNumber,erp->splitnb);
	  }
    
	  printf("Opening file : %s\n",erp->current_file);
	  erp->fd = open(erp->current_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	  if (erp->fd == -1) {
	    perror("open failed: ");
	    return ER_ERROR;
	  }
	}
      }
      break;
      
    case 4:
      /* coda file output */
      for (i=start; i <= stop; i++)
      {
/* 	int *pFirstWord; */
        len = pe[i]->length>>2;

/* 	printf("Got event of length %d bytes.\n", len); */
/* 	pFirstWord = pe[i]->pdata; */
/* 	printf("First word is at %X.\n", pFirstWord); */
/* 	printf("First word is %d.\n", *pFirstWord); */

	    erp->object->nlongs += len;
        erp->nlongs += len;
        erp->object->nevents++;
        erp->nevents++;

	    status = evWrite(erp->fd, pe[i]->pdata);




goto skip_old_split;
	
	if(erp->split && (erp->nlongs >= (erp->split)>>2 ))
    {
	  evClose(erp->fd);
	  erp->splitnb++;
	  erp->nlongs  = 0;
	  erp->nevents = 0;

      /* MUST BE REDONE WITHOUT TCL !!!!!!!!!
	  if(erp->filename[0] == '@')
      {
	    char temp[500];
	    printf("Executing file : %s\n",erp->filename);
	    sprintf(temp,"%s %d %d %s %d",
		    &erp->filename[1],
		    erp->object->runNumber, 
		    erp->object->runType,
		    erp->runConfig,
		    erp->splitnb);
	    if(Tcl_VarEval(erp->object->interp,temp,NULL) != ER_OK)
        {
	      printf("ERROR: Filename generation script %s failed",erp->filename);
	      return ER_ERROR;
	    }
	    if (erp->current_file) {
	      ckfree(erp->current_file);
	    }
	    erp->current_file = strdup(erp->object->interp->result);
	  }
      else*/
      {
	    char temp[500];
	    sprintf(erp->current_file, erp->filename, erp->object->runNumber,
          erp->splitnb);
	  }
    
	  printf("coda_er(outputEvents): Opening file : %s\n",erp->current_file);
      erp->fd = 0;
	  evOpen(erp->current_file,"w",&erp->fd);
      tmp = 2047; /*SHOULD GET IT FROM erp->split*/
      evIoctl(erp->fd,"s",&tmp);
	}

skip_old_split:



	/* if error writing file */
	    if (status != 0) break;
      }
      break;
      
    case 2:
      /* debug output */
      {
	 int ix;


         for (i=start; i <= stop; i++) {
          len = pe[i]->length;

	  erp->object->nlongs += len<<2;
	  erp->nlongs += len<<2;
	  erp->object->nevents++;
	  erp->nevents++;
	  	  
	  printf("event type %d length %d (bytes)\n",
		 pe[i]->control[0], pe[i]->length);

	  if (len > 64) len = 64;
	  
	  for (ix=0;ix<len;ix++) {
	    if ((ix % 8) == 0 ) printf("\n%3d : ",ix);
	    printf("%08x ",((unsigned long *) pe[i]->pdata)[ix]);
	  }
          printf("\n");
	 }
      }
      printf("\n");
      break;
      
    case 3:
      /* no output */
      for (i=start; i <= stop; i++) {
        len = pe[i]->length>>2;
	erp->object->nlongs += len;
        erp->nlongs += len;
        erp->object->nevents++;
        erp->nevents++;
      }
      break;
    
  }
  
  /* if error writing coda file */ 
  if (status != 0) {
    return status;
  }   
  
  return 0;
}


int 
CODA_write_event(ERp erp)
{
  const int prestartEvent=17, endEvent=20, true=1, false=0;
  int status1, status2;
  int nevents=0, i, len, done=false, start, stop;
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
#ifdef USE_THREADED_CODE
    status1 = et_events_get(et_sys, et_attach, pe, ET_SLEEP,
                            NULL, ET_EVENT_ARRAY_SIZE, &nevents);
#else
    status1 = et_events_get(et_sys, et_attach, pe, ET_ASYNC,
                            NULL, ET_EVENT_ARRAY_SIZE, &nevents);
#endif

    /* if no events or error ... */
    if ((nevents < 1) || (status1 != ET_OK))
    {
      /* if status1 == ET_ERROR_EMPTY or ET_ERROR_BUSY, no reinit is necessary */
      
      /* will wake up with ET_ERROR_WAKEUP only in threaded code */
      if (status1 == ET_ERROR_WAKEUP)
      {
printf("CODA_write_event: forcing writer to quit read, status = ET_ERROR_WAKEUP\n");
      }
      else if (status1 == ET_ERROR_DEAD) {
printf("CODA_write_event: forcing writer to quit read, status = ET_ERROR_DEAD\n");
        et_reinit = 1;
      }
      else if (status1 == ET_ERROR) {
printf("CODA_write_event: error in et_events_get, status = ET_ERROR \n");
        et_reinit = 1;
      }
      done = true;
    }
    
    /* if we got events ... */
    else {
      /* by default (no control events) write everything */
      start = 0;
      stop  = nevents - 1;
      
      /* if we got control event(s) ... */
      if (gotControlEvent(pe, nevents)) {
        /* scan for prestart and end events */
        for (i=0; i<nevents; i++) {
	  if (pe[i]->control[0] == prestartEvent) {
	    printf("Got Prestart Event!!\n");
	    /* look for first prestart */
	    if (PrestartCount == 0) {
	      /* ignore events before first prestart */
	      start = i;
	      if (i != 0)
          {
	        printf("CODA_write_event: ignoring %d events before prestart\n",i);
	      }
	    }
            PrestartCount++;
	  }
	  else if (pe[i]->control[0] == endEvent) {
            erp->nend--;
	    /* ignore events after last end event & quit */
	    if (erp->nend <= 0) {
	      stop = i;
	      done = true;
	    }
            printf("Got End event, Need %d more\n",erp->nend);
	  }
        }
      } 
      
      /* write events to output (ignore events before first prestart) */
      if (PrestartCount != 0) {
        if (outputEvents(erp, pe, start, stop) != 0) {
	  /* error writing coda file so quit */
	  printf("ERROR: Error writing events... Cancel ET read loop!\n");
          done = true;
        }
      }
	
      /* put et events back into system */
      status2 = et_events_put(et_sys, et_attach, pe, nevents);            
      if (status2 != ET_OK) {
	printf("CODA_write_event: error in et_events_put, status = %i \n",status2);
        et_reinit = 1;
        done = true;
      }	
    }

    if( (stop_ticks = times(&tms_end)) == -1) {
      printf("CODA_write_event: ERROR getting stop time\n");
    } else {

      /*sergey: why HZ ??? it does not defined in Darwin anyway .. */
#ifndef Darwin
      if( ((stop_ticks-start_ticks)/HZ) > ER_WRITE_LOOP_TIMEOUT) {
#else
      if( ((stop_ticks-start_ticks)/CLK_TCK) > ER_WRITE_LOOP_TIMEOUT) {
#endif
	printf("WARN: ER is backed up! This may be causing system deadtime\n");
	done = true;
      }
    }


  } while (done == false);
  
  if (erp->nend <= 0) { 
    return 0;
  } else {
    return 1;
  }
}

/*********************************************************/
/*********************************************************/
/*********************************************************/




int
codaDownload(char *conf)
{
  objClass object = localobject;

  ERp erp = (ERp) object->private;
  int deflt = 0;
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

  sprintf(tmp,"SELECT outputs FROM %s WHERE name='%s'",configname,object->name);
  if(dbGetStr(dbsock, tmp, tmpp)==ER_ERROR)
  {
    printf("cannot get 'outputs' from table>%s< for the name>%s<\n",configname,object->name);
    return(ER_ERROR);
  }
  else
  {
    strcpy(erp->output_type,tmpp);
    printf("get erp->output_type >%s<\n",erp->output_type);
  }

  /* default output to none */
  erp->output_switch = 3;

  if( !strncmp(erp->output_type,"file",4) ) /* output to binary file */
  {
    sprintf(tmp,"SELECT value FROM %s_option WHERE name='dataFile'",configname);
    if(dbGetStr(dbsock, tmp, tmpp)==ER_ERROR)
    {
      printf("cannot get 'dataFile' from table >%s_option<\n",configname);
      return(ER_ERROR);
    }
    else
    {
      erp->filename = strdup(tmpp); /* Sergey: change it to strcpy(erp->filename,tmpp);*/
      printf("get erp->filename >%s<\n",erp->filename);
    }
    erp->output_switch = 1;
  }
  else if( !strncmp(erp->output_type,"debug",5) ) /* debug dump */
  {
    erp->output_switch = 2;
  }
  else if( !strncmp(erp->output_type,"none",4) ) /* output to /dev/null */
  {
    erp->output_switch = 3;
  }
  else if( !strncmp(erp->output_type,"coda",4) ) /* output in CODA format */
  {
    sprintf(tmp,"SELECT value FROM %s_option WHERE name='dataFile'",configname);
    if(dbGetStr(dbsock, tmp, tmpp)==ER_ERROR)
    {
      printf("cannot get 'dataFile' from table >%s_option<\n",configname);
      return(ER_ERROR);
    }
    else
    {
      erp->filename = strdup(tmpp); /* Sergey: change it to strcpy(erp->filename,tmpp);*/
      printf("get erp->filename >%s<\n",erp->filename);
    }
  	printf("coda format will be used\n");
    erp->output_switch = 4;
  }
  else
  {
    printf("invalid erp->output_type >%s<\n",erp->output_type);
    return(ER_ERROR);
  }


  /*****************************/
  /*****************************/
  /*****************************/



  erp->fd = -1;

#ifndef LINK_BOS_FORMAT
#ifndef LINK_CODA_FORMAT
  if (erp->mod_id)
  {
    printf("INFO: Unloading module %x\n", erp->mod_id);

#if defined __sun||LINUX
    if (dlclose ((void *) erp->mod_id) != 0)
    {
      printf("ERROR: failed to unload module to decode >%s<\n",erp->mod_name);
      return ER_ERROR;
    }
#else
    printf("WARN: dynamic loading not yet supported on this platform\n");
#endif
  }
#endif
#endif

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


  deflt = 0;
#ifndef LINK_BOS_FORMAT
#ifndef LINK_CODA_FORMAT
  /* Get object filename in order to find the ROLs __init routine */
  if(listArgc)
  {
    strcpy(erp->mod_name,listArgv[0]);

    /* Load the decode module */
    sprintf(tmp,"%s/%s_file.so",getenv("CODA_LIB"),listArgv[0]);
#if defined __sun||LINUX
    if (erp->mod_id) dlclose(erp->mod_id);
    erp->mod_id = dlopen ((const char *) tmp, RTLD_NOW | RTLD_GLOBAL);
    if (erp->mod_id == 0) {
      printf("WARN: !! dlopen failed to open >%s<\n",tmp);
      printf("WARN: !! >%s<\n",dlerror());
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
#endif
#endif


  /******************************************************************/
  /* Now look up the routines in the library and fill in the tables */
  if(deflt)
  {
    /* default to CODA format */
    printf("INFO: Using inbuilt (CODA) format\n");
    erp->open_proc = CODA_open_file;
    erp->close_proc = CODA_close_file;
    erp->write_proc = CODA_write_event;
    
  }
  else
  {
#ifdef LINK_BOS_FORMAT
    printf("INFO: Using BOS (FPACK) format\n");
    erp->open_proc = FPACK_open_file;
    erp->close_proc = FPACK_close_file;
    erp->write_proc = FPACK_write_event;
#else
#ifdef LINK_CODA_FORMAT
    printf("INFO: Using CODA format\n");
    erp->open_proc = CODA_open_file;
    erp->close_proc = CODA_close_file;
    erp->write_proc = CODA_write_event;
#else
    IFUNCPTR proc;
    /* find input formatting procs */
    sprintf(tmp,"%s_open_file",erp->mod_name);
    proc = (IFUNCPTR) dlsym (erp->mod_id, tmp);
    erp->open_proc = proc;
    sprintf(tmp,"%s_close_file",erp->mod_name);
    proc = (IFUNCPTR) dlsym (erp->mod_id, tmp);
    erp->close_proc = proc;
    sprintf(tmp,"%s_write_event",erp->mod_name);
    proc = (IFUNCPTR) dlsym (erp->mod_id, tmp);
    erp->write_proc = proc;
    printf("INFO: Loaded module for format %s\n",erp->mod_name);
#endif
#endif
  }

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

#ifdef USE_THREADED_CODE
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &ix);
#endif
  
  erp = (ERp) object->private;
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

#ifdef USE_THREADED_CODE
  pthread_exit(0);
#endif
}

#ifndef USE_THREADED_CODE
void 
write_proc(ERp erp)
{
  if(erp->fd > 0)
  {
    if((*(erp->write_proc))(erp,0))
    {
      Tk_CreateTimerHandler(1,(Tk_TimerProc *) write_proc, (ClientData) erp);
    }
  }
}
#endif


int
erDaqCmd(char *param)
{
  objClass object = localobject;

  ERp erp = (ERp) object->private;
  switch(param[0])
  {
  case 'o':
    (*(erp->open_proc))(erp);

    if(erp->fd)
    {
#ifdef USE_THREADED_CODE
      pthread_attr_t detached_attr;
      pthread_attr_init(&detached_attr);
      pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);
      pthread_attr_setscope(&detached_attr, PTHREAD_SCOPE_SYSTEM);
      pthread_create( &erp->write_thread, &detached_attr,
                      (void *(*)(void *)) er_write_thread, (void *) object);
#else
      Tk_CreateTimerHandler(1,(Tk_TimerProc *) write_proc, (ClientData) erp);
#endif
    }else{
      printf("No output (erp->fd = %d)\n",erp->fd);
      return ER_ERROR;
    }
    break;
  case 'd':
  case 'n':

    erp->fd = 0xc0da;   /* Use dummy file descripter */
    /* (*(erp->write_proc))(erp,0); */

    {
#ifdef USE_THREADED_CODE
      pthread_attr_t detached_attr;
      pthread_attr_init(&detached_attr);
      pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);
      pthread_attr_setscope(&detached_attr, PTHREAD_SCOPE_SYSTEM);
      pthread_create( &erp->write_thread, &detached_attr,
                      (void *(*)(void *)) er_write_thread, (void *) object);
#else
      Tk_CreateTimerHandler(1,(Tk_TimerProc *) write_proc, (ClientData) erp);
#endif
    }
    break;
  case 'c':
    {
      void *status;
#ifdef USE_THREADED_CODE
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
#endif



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

  case 'r':
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

  case 'p':
    break;
  }
  return(ER_OK);
}


int
codaPrestart()
{
  MYSQL *dbsock;
  char tmpp[1000];

  objClass object = localobject;

  ERp erp = (ERp) object->private;

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

  printf("output_switch = %d\n",erp->output_switch);
  if(erp->output_switch == 2)
  {
	erDaqCmd("debug");
  }
  else if(erp->output_switch == 3)
  {
	erDaqCmd("none");
  }
  else
  {
    erDaqCmd("open");
  }

  tcpState = DA_PAUSED;
  codaUpdateStatus("paused");

  return(ER_OK);
}

int
erFlushET()
{
  objClass object = localobject;

  ERp erp = (ERp) object->private;
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

    /* debug output */
    if(stat1 == ET_OK && (nevents > 0))
    {
      int i, ix, len;
      
      if(erp->log_EvDumpLevel)
      {
        for(i=0; i<nevents; i++)
        {
          len = pe[i]->length>>2;
          if(len > 64)
          {
	        len = 64;
	      }
	      printf("event type %d length %d (bytes)\n",
	         pe[i]->control[0], pe[i]->length);

	      for(ix=0; ix<len; ix++)
          {
	        if((ix % 8) == 0 ) printf("\n%3d : ",ix);
	        printf("%08x ",((unsigned long *) (pe[i]->pdata))[ix]);
	      }
	      printf("\n");
	    }
      }
	 
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

/*****************************************************/
/*****************************************************/
/****************** TCL interface ********************/

TCL_PROC(ER_download)
{
  return(codaDownload(argv[1]));
}
TCL_PROC(ER_prestart)
{
  return(codaPrestart());
}
TCL_PROC(ER_go)
{
  return(codaGo());
}
TCL_PROC(ER_end)
{
  return(codaEnd());
}
TCL_PROC(ER_pause)
{
  return(codaPause());
}
TCL_PROC(ER_exit)
{
  return(codaExit());
}


/****************/
/* main program */
/****************/

void
main (int argc, char **argv)
{
  CODA_Init(argc, argv);
  /* CODA_Service ("ROC"); */
  CODA_Execute ();
}

#else

/* dummy VXWORKS version */

void
coda_er()
{
  printf("coda_er is dummy for VXWORKS\n");
}

#endif


