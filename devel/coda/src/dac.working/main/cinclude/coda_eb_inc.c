
/* UNIX version only */

#ifndef VXWORKS

#include "CDEB_class.c"
#include "CODA_class.c"
/*#include "LINK_class.c"*/

/* deb_component.c */

/*#define DEBUG*/

#define MAX_NODES 300


/* INCLUDES */

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <dlfcn.h>
#include <sys/mman.h>

#include "da.h"
#include "rc.h"
#include "circbuf.h"
#include "bosio.h"
#include "etbosio.h"
#include "CODA_format.h"
#include "libdb.h"

#define TCL_PROC(name) int name (objClass object, Tcl_Interp *interp, \
                                 int argc, char **argv)

void *handle_build();

/* NOTE: nthreads*chunk MUST BE LESS then total number of events in ET */
static int nthreads = 1;
static int chunk = 400; /* 100; MUST BE LESS THEN NCHUNKMAX !!! */




/*static*/ objClass localobject;
extern char    *session; /* coda_component.c */
#define DEB_ERROR 1
#define DEB_OK 0
extern Tcl_Interp *Main_Interp;

static int ended_loop_exit = 0;

int listSplit1(char *list, int flag,
           int *argc, char argv[LISTARGV1][LISTARGV2]);



/* used by debopenlinks() and debcloselinks() */
static int  linkArgc;
static char linkArgv[LISTARGV1][LISTARGV2];

DATA_LINK debOpenLink(char *fromname, char *toname, char *tohost, MYSQL *dbsock);
int debCloseLink(DATA_LINK theLink, MYSQL *dbsock);






/* queues for events ordering */
static int id_in_index, id_out_index;
static int id_in[NIDMAX];
static int id_out[NIDMAX];

static pthread_mutex_t id_out_lock; /* lock the 'idout' */
static pthread_cond_t id_out_empty; /* condition for 'idout' */

#define out_lock    pthread_mutex_lock(&id_out_lock)
#define out_unlock  pthread_mutex_unlock(&id_out_lock)

/* other shared variables */
static int nRUNEVENT;
static int nSCALERS;

/* DDL stuff (initialized during prestart, then read-only by BOS_format) */
extern int nddl; /* see LINK_support.c and BOS_format.c */
extern DDL ddl[NDDL]; /* see LINK_support.c and BOS_format.c */


extern int deflt = 0; /* 1 for CODA format, 0 for BOS format (see LINK_support.c) */
extern unsigned int roc_linked;   /* linked ROCs mask (see LINK_support.c)*/
extern CIRCBUF *roc_queues[MAX_ROCS]; /* see LINK_support.c */
extern int roc_queue_ix; /* cleaned up here, increment in LINK_support.c */
extern unsigned int *bufpool[MAX_ROCS][QSIZE];  /* see LINK_support.c */
extern int roc_queue_evsize[MAX_ROCS];  /* see LINK_support.c */

extern char configname[128]; /* coda_component.c */

typedef struct thread_args *trArg;
typedef struct thread_args
{
  objClass object;
  Tcl_Interp *interp;
  /*ROL_MEM_ID input;*/
  int *thread_exit;
  int id;
} TRARGS;

typedef struct EBpriv *EBp;
typedef struct EBpriv
{
  int nrocs;
  int roc_id[MAX_ROCS];
  int roc_nb[MAX_ROCS];
  CIRCBUF **roc_stream[MAX_ROCS];

  int active; /* obsolete ? */

  int ended;
  int ending;
  int end_event_done;
  int force_end;

  Tcl_Interp *interp;

  pthread_mutex_t active_mutex; /* obsolete ? */
  pthread_cond_t active_cond; /* obsolete ? */

  pthread_mutex_t data_mutex;
  pthread_cond_t data_cond;

  int nthreads;               /* the number of building threads */
  pthread_t idth[NTHREADMAX]; /* pointers to building threads */

  unsigned long roc_mask;
  unsigned long ctl_mask;
  unsigned long cur_cntl;

  void *out_id[NTHREADMAX];
  char out_name[200];

  /* pointers to the link structures */
  DATA_LINK links[MAX_ROCS];

} eb_priv;

typedef struct bank_part *bankPART;
typedef struct bank_part
{
  int bank;
  int length;
  unsigned long *data;
  evDesc desc;
} BANKPART;

typedef int (*IFUNCPTR) ();

#define EVENT_RESERV_FRAG 0
#define EVENT_RESERV_HEAD 1
#define EVENT_RESERV_DESC 2
#define EVENT_ENCODE_FRAG 3
#define EVENT_ENCODE_HEAD 4
#define EVENT_ENCODE_DESC 5
#define EVENT_ENCODE_SPEC 6
#define EVENT_DECODE_FRAG 7
#define EVENT_DECODE_HEAD 8
#define EVENT_DECODE_DESC 9
#define EVENT_DECODE_SPEC 10

static IFUNCPTR out_procs[NTHREADMAX][11];



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



/************/
/* ET stuff */

#include <et_private.h>
extern char	et_name[ET_FILENAME_LENGTH];
static et_sys_id	et_sys;
static int		et_locality, et_eventsize;
static int		et_init = 0, et_reinit = 0;

/* ET Initialization */    
int
eb_et_initialize(void)
{
  et_openconfig   openconfig;
  struct timespec timeout;
  int    events_total;

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

  if(et_open_config_init(&openconfig) != ET_OK)
  {
    printf("deb ET init: cannot allocate mem to open ET system\n");
    return TCL_ERROR;
  }
  et_open_config_setwait(openconfig, ET_OPEN_WAIT);
  et_open_config_settimeout(openconfig, timeout);
  if(et_open(&et_sys, et_name, openconfig) != ET_OK)
  {
    printf("deb ET init: cannot open ET system\n");
    return TCL_ERROR;
  }
  et_open_config_destroy(openconfig);

  /* set level of debug output */
  et_system_setdebug(et_sys, ET_DEBUG_ERROR);

  /* where am I relative to the ET system? */
  et_system_getlocality(et_sys, &et_locality);

  /* find out how many events in this ET system */
  if(et_system_getnumevents(et_sys, &events_total) != ET_OK)
  {
    et_close(et_sys);
    printf("deb ET init: can't find # events in ET system\n");
    return(TCL_ERROR);
  }

  /* find out event size in this ET system */
  if(et_system_geteventsize(et_sys, &et_eventsize) != ET_OK)
  {
    et_close(et_sys);
    printf("deb ET init: can't find event size in ET system\n");
    return(TCL_ERROR);
  }

  et_init++;
  et_reinit = 0;
  printf("deb ET init: ET fully initialized\n");

  return(TCL_OK);
}

/* end of ET stuff */
/*******************/






/*Sergey: temporary just for test
int
tmpUpdateStatistics()
{
  char tmp[1000];

  sprintf(tmp,"ts2: %d %d %d %d %d %d %d %d %d %d %d %d %d ",77,
    1,2,3,4,5,6,7,8,9,10,11,12);

  UDP_request(tmp);

  return(0);
}
*/








#define data_lock(ebp_m)  pthread_mutex_lock(&(ebp_m)->data_mutex)
#define data_unlock(ebp_m)  pthread_mutex_unlock(&(ebp_m)->data_mutex)

#define CONT_EV 0
#define PHYS_EV 1
#define USER_EV 2
#define SYNC_EV 3


/********************************************************************
 handle_build : This is the main routine of the "Build thread" it is
                executed as a detached thread.
*********************************************************************/

#define NPROF1 100
#define NPROF2 40000

void *
handle_build(trArg arg)
{
  objClass object;
  DATA_DESC descriptors[MAX_ROCS];
  BANKPART *build_node[MAX_NODES];
  BANKPART build_nodes[MAX_NODES];
  FILE *fdlog;
  EBp ebp;
  int id, in_error = 0, res = 0, i, j, k, ix, node_ix, cc, roc, lenbuf, rocid;
  DATA_DESC *desc1, desc2;
  long types[MAX_ROCS];
  unsigned int *evptr[MAX_ROCS][NCHUNKMAX];
  int evsize[MAX_ROCS];
  unsigned long fragment_mask, skip_mask, sync_mask, type_mask;
  long current_evnb;
  long current_evty;
  long current_sync;
  unsigned long current_syncerr, current_evtyerr;
  int nevents2put=0, nevents2dump=0, neventsfree=0, neventsnew = 0;
  int idin, idout, itmp;
  int print_rocs_report = 1;

  signed long nevbuf, nevrem;
  int nphys;
  unsigned int nevbuf_was_zero;

  hrtime_t start1, end1, start2, end2, time1=NULL, time2=NULL, time3=NULL;
  hrtime_t nevtime1=NULL, nevtime2=NULL, nevchun=NULL;


  unsigned long *total_length; /* WARNING: total_length is used in two ways:
                                 - it is initially set to NULL and then
                                   incremented by the number of long words in
                                   the event, and then
                                 - it is the event length in bytes */

  et_event *cevent = NULL;
  et_att_id	et_attach;
  et_event	**etevents = NULL, **etevents_dump = NULL;


  object = arg->object;
  id = arg->id;
  ebp = (void *) object->private;


  printf("[%1d] handle_build starting ..\n",id); fflush(stdout);

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &ix);
  pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, &ix);


  /* attachment */
  if(et_station_attach(et_sys, ET_GRANDCENTRAL, &et_attach) < 0)
  {
    et_close(et_sys);
    printf("[%1d] cannot attach to ET station - exit.\n",id);
    exit(0);
  }
  else
  {
    printf("[%1d] attached to ET\n",id);
  }

  /* allocate memory for events from ET */
  if((etevents = (et_event **) calloc(NCHUNKMAX, sizeof(et_event *))) == NULL)
  {
    et_close(et_sys);
    printf("[%1d] deb ET init: no mem left\n",id);
    exit(0);
  }
  if((etevents_dump =
                  (et_event **) calloc(NCHUNKMAX, sizeof(et_event *))) == NULL)
  {
    et_close(et_sys);
    free(etevents);
    printf("[%1d] deb ET init: no mem left\n",id);
    exit(0);
  }

  /* some initialization */
  for(i=0; i<MAX_ROCS; i++) types[i] = -1;
  for(i=0; i<MAX_NODES; i++)
  {
    build_node[i] = &build_nodes[i];
    bzero((void *)build_node[i],sizeof(BANKPART));
  }
  fragment_mask = 0;
  sync_mask = 0;
  type_mask = 0;
  skip_mask = 0;
  current_evty = -1;
  current_evnb = -1;
  current_sync =  0;
  current_syncerr = 0;
  current_evtyerr = 0;
  total_length = NULL;
  node_ix = 0;

  /* initialization for BOS_format.c */
  desc2.time = time(NULL);
  desc2.runty = object->runType;
  desc2.runnb = object->runNumber;
  desc2.nRUNEVENT = 0;
  desc2.nSCALERS  = 0;
  desc2.trigbits = 0;
  desc2.eventtype = 1;
  desc2.latefail = 0;
  desc2.rocpattern = 0;
  for(i=0; i<MAX_ROCS; i++)
  {
    desc2.rocstatus[i] = 0;
    desc2.rocexist[i] = 0;
  }

  /* start out with nothing in descriptors */
  bzero((void *)descriptors,sizeof(descriptors));

  /*******************
  * MAIN LOOP STARTS *
  *******************/

  nevbuf = 0;
  nevbuf_was_zero = 0;
  do
  {
    int bank, subBank, typ, issync;
    unsigned long *soe, *dabufp, *data, *temp;
    unsigned long roc_mask = ebp->roc_mask;
    unsigned long indx, indx2;
    BANKPART *prev_val, *cur_val, *temp_val, **This;
    char *ctype, *dtype;

    typ = -1; /* Sergey: to make debug messages happy .. */

top:

start2 = gethrtime();

    /* check if we need to end in a hurry (force_end);
    in that case we have to check if "end event" was already written */
    if(ebp->force_end)
    {
      printf("[%1d] ROC mask 0x%08x force_end %d end_event_done %d\n",id,
        ebp->roc_mask,ebp->force_end,ebp->end_event_done);
      fflush(stdout);

      if(ebp->end_event_done)
      {
        printf("[%1d] we are ended already, goto output_event\n",id);
        fflush(stdout);
        goto output_event;
      }

      printf("[%1d] NEED TO TEST AND PROBABLY REDESIGN THAT PART !!!\n",id);
      fflush(stdout);

      /* if there is no empty ET buffers go and get it */
      /* if ET system is dead or can't get event, don't put it */
      if(neventsfree < 1)
      {
        nevents2put = 0;
        neventsfree = 1;
        in_error=et_event_new(et_sys,et_attach,&etevents[0],ET_SLEEP,NULL,120);
        if(in_error != ET_OK)
        {
          /* need to reinit ET system since something's wrong */
          printf("[%1d] ERROR: et_event_new() returns %d\n",id,in_error);
          et_reinit = 1;
          break;
        }
        else
        {
          printf("[%1d] et_event_new() 1\n",id);
        }
      }

      soe = dabufp = (unsigned long *) etevents[nevents2put]->pdata;
      nevents2put++;
      neventsfree--;

      desc2.type = 20;
      desc2.time = time(NULL);
      desc2.rocs[0] = 0;
      desc2.evnb = object->nevents;
      desc2.runty = object->runType;
      desc2.runnb = object->runNumber;

#ifdef LINK_BOS_FORMAT
printf("[%1d] EVENT_ENCODE_SPEC 2 ..\n",id);fflush(stdout);
      BOS_encode_spec(&dabufp, &desc2);
printf("[%1d] .. done\n",id);fflush(stdout);
#else
#ifdef LINK_CODA_FORMAT
printf("[%1d] EVENT_ENCODE_SPEC 2 ..\n",id);fflush(stdout);
      CODA_encode_spec(&dabufp, &desc2);
printf("[%1d] .. done\n",id);fflush(stdout);
#else
printf("[%1d] EVENT_ENCODE_SPEC 2 .. 0x%08x .. runnb=%d\n",
id,(*out_procs[id][EVENT_ENCODE_SPEC]),desc2.runnb);fflush(stdout);
      (*out_procs[id][EVENT_ENCODE_SPEC])(&dabufp, &desc2);
printf("[%1d] .. done, runnb=%d\n",id,desc2.runnb);fflush(stdout);
#endif
#endif

      ebp->ended = 1;
      /*ebp->end_event_done = 1; ??? */
      bzero((void *) descriptors, sizeof(descriptors));
      node_ix = 0;

      /* Fix total_length for Control Events */
      total_length = (unsigned long *) desc2.length;

      goto output_event;
    }



    /* check if all ROCs are linked (roc_linked must be filled up
       by LINK_sized_read (LINK_support.c file)) */
    /* usually have to wait in Prestart */
    while((roc_mask!=roc_linked) && (ebp->ended==0) && (ebp->ending==0))
    {
      printf("[%1d] 0x%08x != 0x%08x, waiting for the following ROC IDs:\n",
			 arg->id,roc_mask,roc_linked); /*roc_linked hungs sometimes ..*/
      itmp = 1;
      for(i=0; i<32/*MAX_ROCS*/; i++)
      {
        /*printf("[%2d] -> %d %d\n",i,roc_mask&itmp,roc_linked&itmp);
        fflush(stdout);*/
        if( ((roc_mask&itmp)!=0) && ((roc_linked&itmp)==0) ) printf(" %2d",i);
        itmp = itmp << 1;
      }
      printf("\n\n");
      fflush(stdout);
      sleep(3);

      if(ebp->force_end)
      {
        printf("Stop waiting for the rocs because of force_end condition\n");
        goto top;
      }

    }

    if(print_rocs_report)
    {
      printf("[%1d] 0x%08x == 0x%08x, all ROCs reported\n",
        arg->id,roc_mask,roc_linked);
      fflush(stdout);
      print_rocs_report = 0;
	}

    /******************
    * ROC LOOP STARTS *
    ******************/

retry:


    /* if buffer is empty get a new chunk of events */
    if(nevbuf == 0)
    {

#ifdef DO_NOT_PUT
      printf("EB sleeps forever, all data go to trash !\n");
      while(1) sleep(10); /* sleep forever */
#endif

/*printf("[%1d] locking .......\n",id,nevbuf);*/
      data_lock(ebp);
/*printf("[%1d] .. locked\n",id,nevbuf);*/

/*printf("[%1d] geting data ..\n",id);*/

      start1 = gethrtime();

      nevbuf=cb_events_get(roc_queues,id,ebp->nrocs,chunk,evptr,evsize,&nphys);
      for(i=0; i<MAX_ROCS; i++) roc_queue_evsize[i] = evsize[i];
      idin = id_in[id_in_index];
      id_in_index = (id_in_index + 1) % NIDMAX;

      end1 = gethrtime();

      /* save counters updated by previous thread;
      will use them in this thread */
      desc2.nRUNEVENT = nRUNEVENT;
      desc2.nSCALERS = nSCALERS;

      /* update counters to be used by next thread */
      nRUNEVENT += nphys;
      nSCALERS += (nevbuf-nphys);
/*
printf("[%1d] .. got idin=%d, nevbuf=%d, nphys=%d, event no.=%d\n",
id,idin,nevbuf,nphys,evptr[0][0][1]&0xff);
*/

      time1 += ((end1-start1)/NANOMICRO);
      time3 -= ((end1-start1)/NANOMICRO);
      nevchun = nevchun + ((hrtime_t)nevbuf);
      if(++nevtime1 == NPROF1)
      {
        /*
        printf("2: time1=%7lld microsec, chuck=%7lld",
          time1/nevtime1,nevchun/nevtime1);
        if(nevchun > 0) printf(" -> %7lld microsec per event\n",time1/nevchun);
        else printf("\n");
        */
        nevtime1 = 0;
        time1 = 0;
        nevchun = 0;
      }

      if(nevbuf == -1)
      {
        ebp->roc_mask &= ~ (1<<roc); /* ERROR: 'roc' is not defined here !!! */
        printf("handle_build: cb_events_get() returned end of file\n");
        fflush(stdout);
        ebp->force_end = 2;
        data_unlock(ebp);
        goto top;
      }
      else if(nevbuf < 0)
      {
        printf("ERROR: nevbuf=%d\n",nevbuf);
        fflush(stdout);
      }

/*printf("[%1d] unlocking .......\n",id,nevbuf);*/
      data_unlock(ebp);
/*printf("[%1d] .. unlocked\n",id,nevbuf);*/

      /* if we've run out of available events, get more from ET */
      if(neventsfree < 1)
      {
        neventsnew = 0;
        nevrem = nevbuf;

        while(nevrem)
        {
/*printf("[%1d] geting %d events from ET\n",id,nevrem);*/
          in_error = et_events_new(et_sys, et_attach, &etevents[neventsnew],
                                   ET_SLEEP, NULL, et_eventsize, nevrem, &cc);
          if(in_error != ET_OK)
          {
            printf("et_events_new ERROR: length = %d bytes, status = %d\n",
                   (int) total_length,in_error); fflush(stdout);
            ebp->force_end = 4;
            et_reinit = 1;
            break;
          }

          neventsnew += cc;
          nevrem -= cc;
        }

        neventsfree = neventsnew;
        
/*printf("et_events_new(): neventsnew = %d, status = %d\n",
neventsnew,in_error); fflush(stdout);*/
        
        if(nevbuf!=neventsnew)
        {
          printf("deb_component ERROR: nevbuf=%d != neventsnew=%d\n",
                 nevbuf,neventsnew);
          fflush(stdout);
          exit(0);
        }

      }

      /* */
      for(roc=0; roc<ebp->nrocs; roc++)
      {
        desc1 = &descriptors[roc];

        desc1->rocid = roc_queues[roc]->rocid;
        desc1->nevbuf = nevbuf;

        /* some checks */
        if(desc1->rocid < 0 || desc1->rocid > 31)
          {printf("ERROR: rocid=%d\n",desc1->rocid);fflush(stdout);}

        desc1->soe = (unsigned long *)evptr[roc][0]; /* start of first event */
		/*bla
printf(" ===== soe3: 0x%08x (0x%08x) nev=%d\n",desc1->soe,*desc1->soe,nevbuf);fflush(stdout);
		*/
/*
printf("[%1d] deb0 roc=%d desc=0x%08x soe=0x%08x (0x%08x 0x%08x 0x%08x) evnb=%d rocid=%d\n",
id,roc,desc1,desc1->soe,*desc1->soe,*(desc1->soe+1),*(desc1->soe+2),
desc1->evnb, desc1->rocid);fflush(stdout);
*/
        desc1->user[0] = 0;
        desc1->totbuf = desc1->nevbuf; /* remember total amount of events */
        desc1->ievbuf = 0;
      }
    }


/***********************************/
/* former place of et_events_new() */
/***********************************/











/*printf("[%1d] deb1\n",id);fflush(stdout);*/
    for(roc=0; roc<ebp->nrocs; roc++)
    {
      desc1 = &descriptors[roc];

/*printf("[%1d] deb2 roc=%d desc=0x%08x\n",id,roc,desc1);fflush(stdout);*/
      /* Decode the event; returns with 'temp' pointing to the first data
      word AFTER the header, note also that if there is no data from this ROC
      EVENT_DECODE_FRAG should still behave itself and return a descriptor */
      temp = data = desc1->soe;
	  /*bla
printf("soe4: 0x%08x (0x%08x 0x%08x)\n",desc1->soe,*desc1->soe,*(desc1->soe+1));fflush(stdout);
	  */
/*printf("[%1d] deb21 desc1->soe=0x%08x, 1st=0x%08x, 2nd=0x%08x (before=0x%08x)\n",
id,desc1->soe,data[0],data[1],data[-1]);fflush(stdout);*/
      typ = (data[1] >> 16) & 0xff;       /* Temp storage of type info */
/*printf("[%1d] deb22\n",id);fflush(stdout);*/
      issync = (data[1] >> 24) & 0x01;    /* Temp storage of sync info */
/*printf("[%1d] deb3 typ=%d\n",id,typ);fflush(stdout);*/
      if(typ < 16)
      {
#ifdef LINK_BOS_FORMAT
        BOS_decode_frag(&temp,desc1);
#else
#ifdef LINK_CODA_FORMAT
        CODA_decode_frag(&temp,desc1);
#else
        (*out_procs[id][EVENT_DECODE_FRAG])(&temp,desc1);
#endif
#endif
      }
      else if((typ > 15) && (typ < 32))
      {
#ifdef LINK_BOS_FORMAT
        BOS_decode_spec(&temp,desc1);
#else
#ifdef LINK_CODA_FORMAT
        CODA_decode_spec(&temp,desc1);
#else
        printf("[%1d] EVENT_DECODE_SPEC (0x%08x) ..\n",
          arg->id,*out_procs[id][EVENT_DECODE_SPEC]); fflush(stdout);
        (*out_procs[id][EVENT_DECODE_SPEC])(&temp,desc1);
        printf(" .. almost (0x%08x) ..\n",desc1); fflush(stdout);
#endif
#endif

        desc1->evnb = -1;        /* to be sure */
        desc1->bankCount = 1;    /* Control events have one Bank */



        /*********************************************************************/
        /*********************************************************************/
        /*********************************************************************/
        /* following is a temporary solution for ending problem; current
        algorithm works only if all 'End' events have the same number so
        they are coming together; that behaviour inforced in ROCs and it is
        true for normal running conditions, but it is not guaranteed, and
        sometimes we have a problem if 'End' event from one ROC not in a
        sync with 'End' events from other ROC(s); temporary solution is that
        we are setting 'force_end=1' and going to 'top' where force end
        situation will be handled; what probably should be done instead: if
        we obtained first 'End' event from some ROC, loop over other ROCs
        and for every ROC skip non-End events until 'End' event obtained */

        if(typ==20)
        {
          if(ebp->ending==1)
          {
            printf("[%1d] WE ARE ENDING AND GOT END EVENT FROM ROC %d\n",
              id,roc);
          }
          else
          {
            printf("[%1d] WE ARE NOT ENDING BUT GOT END EVENT FROM ROC %d\n",
              id,roc);
          }
          ebp->force_end = 1;
          goto top;
        }

        /*********************************************************************/
        /*********************************************************************/
        /*********************************************************************/



        printf(" .. done.\n"); fflush(stdout);
      }
      else
      {
        printf("[%1d] User Events are NOT supported (typ=%d) -exit.\n",id,typ);
        fflush(stdout);
        exit(0);
      }
/*
printf("[%1d] ending=%d ended=%d end_event_done=%d\n",
id,ebp->ending,ebp->ended,ebp->end_event_done);
printf("[%1d] deb4\n",id);fflush(stdout);
*/
      /* sequence error check */
      if(current_evty == -1) /* event type -1 is used it to mark new event */
      {
		/*
printf("[%1d] deb41: rocid=%d type=%d evnb=%d\n",id,desc1->rocid,typ,desc1->evnb);fflush(stdout);
		*/
        current_evty = typ;
        current_evnb = desc1->evnb;
      }
      else
      {
		/*
printf("[%1d] deb42: rocid=%d type=%d evnb=%d\n",id,desc1->rocid,typ,desc1->evnb);fflush(stdout);
		*/
        /* we are already building so we can check some things... */
        if((current_evnb != desc1->evnb) && (in_error == 0))
        {
/*printf("[%1d] deb43\n",id);fflush(stdout);*/
          in_error = 1;
          printf("[%1d] FATAL: Event (Num %d type %d) NUMBER mismatch",
                 id,current_evnb,current_evty);
          fflush(stdout);
          printf(" -- %s (rocid %d) sent %d (type %d)\n",
                  (get_cb_name(*ebp->roc_stream[ebp->roc_nb[desc1->rocid]])), 
                  desc1->rocid,desc1->evnb,desc1->type);
          fflush(stdout);
          printf("[%1d] ERROR: Discard data until next control event\n",id);
          fflush(stdout);
          fragment_mask = 0;
          skip_mask = 0;
exit(0);
        }
        else if((current_evty != desc1->type) && (in_error == 0))
        {
/*printf("[%1d] deb44\n",id);fflush(stdout);*/
          current_evtyerr ++;   /* Count type mismatches from first fragment */
          type_mask |= (1<<desc1->rocid); /* Get mask of ROC IDs with mismatched
                                            types */
          types[desc1->rocid] = desc1->type; /* Store type for each ROC */
        }
      }
/*printf("[%1d] deb5\n",id);fflush(stdout);*/
      if(in_error) /* lets try to recover */
      {
        if((typ > 15) && (typ < 32))
        {
          printf("[%1d] WARN: Found sync point %d for ROC >%s<\n",
		          id,desc1->type,
                  (get_cb_name(*ebp->roc_stream[ebp->roc_nb[desc1->rocid]])));
          skip_mask |= (1 << roc);

          fragment_mask |= (1<< desc1->rocid);
          {
            /*register*/ BANKPART *bn = build_node[node_ix];
            bn->desc = desc1;
          }
          node_ix++;

          if(fragment_mask == roc_mask)
          {
            in_error = 0;
            skip_mask = 0;
            printf("[%1d] WARN: Resyncronised via control event\n",id);
            fflush(stdout);
            if(desc1->type == 20)
            {
              printf("got all end events !!!\n"); fflush(stdout);
              ebp->force_end = 3;
            }
            goto top;
          }
          goto retry;
        }
        else
        {
          printf("[%1d] requeue event - NEED TO BE TESTED !!! (typ=%d)\n",
                 id,typ);
          fflush(stdout);

          if(typ==20)
          {
            printf("[%1d] requeue event: it seems we have 'End' ..\n",id);
            fflush(stdout);
          }

          /* requeue event: lets take the current event and discards it by
          "bumping" the pointers to the start of the event to point to the next
          event. If this event is the last one in the buffer then do nothing.*/

          printf("[%1d] desc1->nevbuf=%d\n",id,desc1->nevbuf);
          fflush(stdout);
          if(desc1->nevbuf > 0)
          {
            /* note this routine returns with "temp" pointing to
            the first data word AFTER the header, note also that if there is
            no data from this ROC EVENT_DECODE_FRAG should still behave itself
            and return a descriptor, DECODE_FRAG can set user[0] equal to the
            event header length if it makes sense to do so. */

            desc1->nevbuf--; /* decrement the number of events left in the buffer */
            desc1->ievbuf++; /* increment the current event number in buffer */
            desc1->user[0] = 0;
            /* shift pointer to the next event */
            desc1->soe = (unsigned long *)evptr[roc][desc1->ievbuf];
printf("soe5: 0x%08x (0x%08x)\n",desc1->soe,*desc1->soe);fflush(stdout);
            temp = data = desc1->soe;
          }
        }
      }
      else /* there were no errors so do ... */
      {
        int ix;

        fragment_mask |= (1<< desc1->rocid);
        skip_mask |= (1 << roc);
        if(issync) sync_mask |= (1<< desc1->rocid);

        /* if there was no data from this ROC we need worry no more;
        this next "if" takes care of the situation where there was a
        CODA header from a ROC but no actual data */
        if(desc1->length > 0)
        {
          /*register*/ BANKPART *bn;

          /* loop over the banks from this ROC to reserve space in
          the buffer;  
             Bug Bug, if the buffers were always big enough we could
             ignore this?? */
          /*BOY(*out_procs[id][EVENT_RESERV_FRAG])(&total_length);*/

          /* convert from bytes to 32 bit long words. */
          /* Here desc1->length is in bytes, convert to long words
          and then increment the total_length pointer by this number. */
		  /* must add 8 words because 'desc1->length' counts 2-word
          headers, and later they will be expanded to 10-word headers;
          we are not taking into account that during glueing procedure
          some headers will be removed so 'total_length' contains bigger
          amount of words then actualy in buffer */

/*total_length += (desc1->length >> 2) + (NFPHEAD-NHEAD);*/
/* !!! ???
total_length += ((desc1->length >> 2) + 10);
*/
          for(ix=0; ix<desc1->bankCount; ix++)
          {
            bn = build_node[node_ix];
            bn->bank = desc1->bankTag[ix];
            bn->length = desc1->fragLen[ix];
total_length += ((bn->length >> 2) + 10);   /* NADO RAZOBRAT'SYA NAKONEC !! */
            bn->data = desc1->fragments[ix];
            bn->desc = desc1;	      
            node_ix++;
          }
        }
      } /* End of if (in_error) */
/*printf("[%1d] deb6\n",id);fflush(stdout);*/

    } /* end of for() over rocs */

/*printf("[%1d] deb9\n",id);fflush(stdout);*/
    /****************
    * ROC LOOP ENDS *
    ****************/










    /* if we get here we have one fragment for
       each ROC and fragment_mask == ebp->roc_mask */

    /* check for Event Type mismatches */
    if(current_evtyerr)
    {
	  
      printf("ERROR: Event (Num %d) TYPE mismatch -- %d ROC Banks\n",
              current_evnb, current_evtyerr); fflush(stdout);
      printf("   (ID Mask =0x%08x) differ from selected build type = %d\n",
              type_mask, current_evty); fflush(stdout);
      printf("  Event Type Mismatch info:\n"); fflush(stdout);
      for(ix=0; ix<MAX_ROCS; ix++)
      {
        if(type_mask&(1<<ix))
        {
          printf("    ROC ID = %d   Type = %d\n",ix,types[ix]);
          fflush(stdout);
        }
      }
	  
    }

    /* check for SYNC mismatches */
    if(sync_mask)
    {
      current_sync = 1;
      if(sync_mask != fragment_mask)
      {
        printf("ERROR: Event (Num %d type %d) SYNC mismatch, ROC mask",
               current_evnb, current_evty); fflush(stdout);
        printf(" = 0x%08x , SYNC mask = 0x%08x\n",
                fragment_mask, sync_mask); fflush(stdout);
        /* keep info on which ROCs missed the Sync Event */
        current_syncerr = fragment_mask&(~sync_mask);
      }
    }

    /* cleanup ... and reserve space */
    fragment_mask = skip_mask = 0;
    type_mask = sync_mask = 0;


/* ???????????????????????????????????????????????? */
/* below reserv again ????? */

    /*BOY(*out_procs[id][EVENT_RESERV_HEAD])(&total_length);*/
    total_length += 11; /* increment total_length pointer */
    /*BOY(*out_procs[id][EVENT_RESERV_DESC])(&total_length);*/
    total_length += 18; /* increment total_length pointer */


    /* then set 'soe' pointer and update event counters */
    soe = dabufp = (unsigned long *) etevents[nevents2put]->pdata;
/*printf("soe6: 0x%08x (0x%08x)\n",soe,soe);fflush(stdout);*/
    nevents2put++;
    neventsfree--;



    /* if physics event, reserve space for the event header
    and event 'ID bank' */
    if(current_evty < 16)
    {
/*printf("115: desc2.desc_start=0x%08x\n",desc2.desc_start);fflush(stdout);
printf("115: desc2.head_start=0x%08x\n",desc2.head_start);fflush(stdout);*/
#ifdef LINK_BOS_FORMAT
      BOS_reserv_head(&dabufp, &desc2);
      BOS_reserv_desc(&dabufp, &desc2);
#else
#ifdef LINK_CODA_FORMAT
      CODA_reserv_head(&dabufp, &desc2);
      CODA_reserv_desc(&dabufp, &desc2);
#else
      (*out_procs[id][EVENT_RESERV_HEAD])(&dabufp, &desc2);
      (*out_procs[id][EVENT_RESERV_DESC])(&dabufp, &desc2);
#endif
#endif
/*printf("116: desc2.desc_start=0x%08x\n",desc2.desc_start);fflush(stdout);
printf("116: desc2.head_start=0x%08x\n",desc2.head_start);fflush(stdout);*/

      object->nevents++;
      desc2.evnb = object->nevents;
      desc2.type = current_evty;
      desc2.syncev = current_sync;
      desc2.err[1] = current_syncerr;
    }



    bank = -1;

    /* if physics event, sort fragments (banks) */
    if(current_evty < 16)
    {
      /* if event too big, ignore it */
      if((int)total_length > et_eventsize)
      {
        printf("ERROR: event too big (%d > %d) - ignored\n",
               (int)total_length,et_eventsize);
        fflush(stdout);

        /* only FPACK header and HEAD bank will be created */
        total_length  = NULL;
        total_length += 11;
        total_length += 18;
      }
      else
      {
        if(node_ix > 1)
        {
          This = build_node;
          prev_val = build_node[0];
          for(indx = 1; indx < node_ix; ++indx)
          {
            cur_val = This[indx];
            if(prev_val->bank > cur_val->bank)
            {
              /* out of order */
              This[indx] = prev_val;
              for(indx2 = indx - 1; indx2 > 0; --indx2)
              {
                temp_val = This[indx2 - 1];
                if(temp_val->bank > cur_val->bank) This[indx2] = temp_val;
                else break;
              }
              This[indx2] = cur_val;
            }
            else
            {
              prev_val = cur_val;
            }
          }
        }


       /* Do the build */
        for(indx=0; indx<node_ix; indx++) /* loop over sorted banks */
        {
          cur_val = build_node[indx];/* pointer to the next bank structure */
          desc1 = cur_val->desc;     /* descriptor for that bank */
          if(bank != cur_val->bank)  /* meet that bank for the first time */
          {
/*printf("1: trigbits[%d]=0x%08x\n",indx,desc1->trigbits);*/
            /* reserv nothing, just remember fragment starting address */
#ifdef LINK_BOS_FORMAT
            BOS_reserv_frag(&dabufp, &desc2);
#else
#ifdef LINK_CODA_FORMAT
            CODA_reserv_frag(&dabufp, &desc2);
#else
            (*out_procs[id][EVENT_RESERV_FRAG])(&dabufp, &desc2);
#endif
#endif

		    /*printf("FIRST TIME\n");*/
            bank = cur_val->bank;
            /* full length (not used by BOS) */
            desc2.length = cur_val->length;
            /* data length: used by BOS only in _ENCODE_FRAG */
            desc2.data_length =(cur_val->length>>2)-NHEAD;
            if(desc2.head_length > 0) /* BOS format */
            {
              /* copy BOS bank header */
              memcpy((char *)dabufp, cur_val->data, NHEAD<<2);
              /* reserve space for FPACK header */
              dabufp += desc2.head_length;
              /* copy bank data */
              memcpy((char *)dabufp, cur_val->data+NHEAD,
                     cur_val->length-(NHEAD<<2)); 
              /* reserve space for bank data */
              dabufp += ( (cur_val->length>>2)-NHEAD );
            }
            else /* CODA format */
            {
              memcpy((char *)dabufp, cur_val->data, cur_val->length); 
              dabufp += (cur_val->length >> 2);
            }
            desc2.rocid = bank;
            desc2.trigbits |= desc1->trigbits;

            /* pick up maximum eventtype; will be used in _encode_desc() */
            if(desc1->eventtype > desc2.eventtype)
            {
/*
printf("[%1d]: eventtype=%d (%d)\n",id,desc2.eventtype,desc1->eventtype);
*/
              desc2.eventtype = desc1->eventtype;
            }
/*printf("2: trigbits[%d]=0x%08x\n",indx,desc2.trigbits);*/
          }
          else /* previous bank had the same 'id' -> glue them together */
          {
            /*printf("SECOND TIME\n");*/
            /* remove bank header length from total length; */
            /*  desc1->user[0] contains bank header length  */
            /*  in words, in case of BOS it is equal to 2   */
            cur_val->length -= (desc1->user[0]<<2);
            /* full length (not used by BOS) */
            desc2.length += cur_val->length;
            /* data length: used by BOS only in _ENCODE_FRAG */
            desc2.data_length += (cur_val->length>>2);
            /* copy bank data */
            memcpy((char *)dabufp, cur_val->data+desc1->user[0],
                                                    cur_val->length);
            /* reserve space for bank data */
            dabufp += (cur_val->length >> 2);
          }
/*printf("[%1d] 1025: 0x%08x 0x%08x 0x%08x 0x%08x\n",id,&dabufp,dabufp,&desc2,desc2);fflush(stdout);*/
#ifdef LINK_BOS_FORMAT
          BOS_encode_frag(&dabufp, &desc2);
#else
#ifdef LINK_CODA_FORMAT
          CODA_encode_frag(&dabufp, &desc2);
#else
          (*out_procs[id][EVENT_ENCODE_FRAG])(&dabufp, &desc2);
#endif
#endif
        }
      }
    }
    else if((current_evty > 15) && (current_evty < 32))
    {

      /* Control events need no building */
      printf("[%1d] Got control event fragments type = %d  node_ix=%d\n",
             id,current_evty,node_ix); fflush(stdout);
      for(indx=0; indx<node_ix; indx++)
      {
        cur_val = build_node[indx];
        desc1 = cur_val->desc;
        if(desc1)
        {
          /* handle control event; if 'End' set appropriate flags */

          /* Sergey */
          desc1->nRUNEVENT = desc2.nRUNEVENT;
          desc1->nSCALERS = desc2.nSCALERS;

          if(desc1->type > 31)       ctype = "User event";
          else if(desc1->type == 17) ctype = "prestart";
          else if(desc1->type == 18) ctype = "go";
          else if(desc1->type == 19) ctype = "pause";
          else if(desc1->type == 20) ctype = "end";  
          if(ebp->ctl_mask == 0)
          {
            ebp->cur_cntl = desc1->type;
printf("11: %d\n",desc1->rocid);fflush(stdout);
printf("12: %d\n",ebp->roc_nb[desc1->rocid]);fflush(stdout);
printf("13: %x\n",(*ebp->roc_stream[ebp->roc_nb[desc1->rocid]]));fflush(stdout);
/*
            printf("[%1d] Build control event: %s(%s",id,ctype,
                   (*ebp->roc_stream[ebp->roc_nb[desc1->rocid]])->name);
*/
          }
          else
          {
            if(ebp->cur_cntl != desc1->type)
            {
              if(ebp->cur_cntl > 31)       dtype = "User";
              else if(ebp->cur_cntl == 17) dtype = "prestart";
              else if(ebp->cur_cntl == 18) dtype = "go";
              else if(ebp->cur_cntl == 19) dtype = "pause";
              else if(ebp->cur_cntl == 20) dtype = "end";
printf("21: %d\n",desc1->rocid);fflush(stdout);
printf("22: %d\n",ebp->roc_nb[desc1->rocid]);fflush(stdout);
printf("23: %x\n",(*ebp->roc_stream[ebp->roc_nb[desc1->rocid]]));fflush(stdout);
              printf(", %s %s != %s ",
               (*ebp->roc_stream[ebp->roc_nb[desc1->rocid]])->name,ctype,dtype);
            }
            else
            {
printf("31: %d\n",desc1->rocid);fflush(stdout);
printf("32: %d\n",ebp->roc_nb[desc1->rocid]);fflush(stdout);
printf("321: %x\n",ebp->roc_stream[1]);fflush(stdout);
printf("33: %x\n",(*ebp->roc_stream[ebp->roc_nb[desc1->rocid]]));fflush(stdout);
/*
              printf(", %s",
               (*ebp->roc_stream[ebp->roc_nb[desc1->rocid]])->name);
*/
            }
          }
          ebp->ctl_mask |= (1<<(desc1->rocid));
          if(ebp->ctl_mask == ebp->roc_mask)
          {
            unsigned long *dabufp;
            printf(")\n-- Got all fragments of %s\n",ctype);
            ebp->ctl_mask = 0;

            /* if thread did not get control event, we still need it !!! */
            desc1->time = time(NULL);
            desc1->rocs[0] = ebp->roc_mask;
            desc1->evnb = object->nevents;
            desc1->runty = object->runType;
            desc1->runnb = object->runNumber;

            dabufp = soe;

#ifdef LINK_BOS_FORMAT
            BOS_encode_spec(&dabufp,desc1);
#else
#ifdef LINK_CODA_FORMAT
            CODA_encode_spec(&dabufp,desc1);
#else
printf("[%1d] EVENT_ENCODE_SPEC 1 .. 0x%08x .. runnb=%d\n",
id,(*out_procs[id][EVENT_ENCODE_SPEC]),desc1->runnb);fflush(stdout);
            (*out_procs[id][EVENT_ENCODE_SPEC])(&dabufp,desc1);
printf("[%1d] .. done, runnb=%d\n",id,desc1->runnb);fflush(stdout);
#endif
#endif

            if(desc1->type == 20)
            {
              ebp->ended = 1;
              ebp->end_event_done = 1;
            }
          }
          /* copy into desc2 */
          memcpy(&desc2,desc1,sizeof(DATA_DESC));


        }
      }
      /* Fix total_length for Control Events */
      total_length = (unsigned long *) desc1->length;
    }


    /* resync */
    node_ix = 0;
    if(current_evty < 16)
    {
      desc2.user[1] = 0;

#ifdef LINK_BOS_FORMAT
      BOS_encode_desc(&dabufp, &desc2);
      BOS_encode_head(&dabufp, &desc2);
#else
#ifdef LINK_CODA_FORMAT
      CODA_encode_desc(&dabufp, &desc2);
      CODA_encode_head(&dabufp, &desc2);
#else
      (*out_procs[id][EVENT_ENCODE_DESC])(&dabufp, &desc2);
      (*out_procs[id][EVENT_ENCODE_HEAD])(&dabufp, &desc2);
#endif
#endif

      desc2.eventtype = 1; /* restore default value */
    }

    /* go to the next event */
    for(roc=0; roc<ebp->nrocs; roc++)
    {
      desc1 = &descriptors[roc]; 
      /* if at least one event left in buffer goto the next event;
      if not - do nothing here, will get new buffer in the begining
      of the next itteration */
      if(desc1->nevbuf > 0)
      {
        desc1->nevbuf--; /* the number of events left in buffer */
        desc1->ievbuf++; /* current event number in buffer */
        desc1->user[0] = 0;

        /* get a pointer to the start of the next event; do not need
        to do that if(desc1->nevbuf==1) but 'if' will take more time */
        desc1->soe = (unsigned long *)evptr[roc][desc1->ievbuf];
		/*bla
if(desc1->soe!=NULL) {printf("next soe: 0x%08x (0x%08x)\n",desc1->soe,*desc1->soe);fflush(stdout);}
		*/
      }
    }
    if(nevbuf > 0) nevbuf--;
    if(nevbuf == 0) nevbuf_was_zero = 1;
    /*printf("[%1d] nevbuf=%d\n",id,nevbuf);fflush(stdout);*/
	/*bla
if(nevbuf == 0) {printf("pisecccccccccccccc\n");fflush(stdout);}
	*/
output_event:
if(ebp->end_event_done) printf("[%1d] output_event 1\n",id);fflush(stdout);

    /* if nothing wrong with ET system, output events */
    if(et_reinit == 0 && ebp->end_event_done == 0)
    {

      /* check for Prestart or Go Event and
      pup these events to ET immediately
      if( (current_evty == EV_PRESTART)||(current_evty == EV_GO) )
      {
        ;
      }
      else*/
      {
        /* fill ET control words */
        etevents[nevents2put-1]->control[0] = desc2.type;
        etevents[nevents2put-1]->control[1] = ebp->roc_mask;
        etevents[nevents2put-1]->control[3] = desc2.user[1];
        /* following can be longer then actual event, but not smaller !!! */
        etevents[nevents2put-1]->length     = (int) total_length;
      }







	  /* !!! SWAP events here if needed !!! */
      /*recordPrint(soe);*/

	  /*do not swap, it will be done by et_2_et !!!???*/
      /*recordSwap(soe);*/







      /* put events back if no free events left or we're forced to end */

/* NEED OUTPUT IF nevbuf==0 !!!!!!!!!!!!!! */

      if((neventsfree < 1) || ebp->force_end || ebp->ended)
      {
        /* put events into ET system */
        if(nevents2put > 0)
        {
          /* check if it is my turn */
          out_lock;
          {
/*
printf("[%1d]: %d ?? %d,%d,%d,%d,%d,%d,%d (nevbuf=%d)\n",
id,idin,id_out[0],id_out[1],id_out[2],id_out[3],
id_out[4],id_out[5],id_out[6],nevbuf);
fflush(stdout);
*/
            while(idin!=id_out[0])
            {
			  /*printf("[%1d]: cond_wait 1 ..\n",id);fflush(stdout);*/
              itmp = pthread_cond_wait(&id_out_empty, &id_out_lock);
			  /*printf("[%1d]: cond_wait 2 ..\n",id);fflush(stdout);*/
            }

/*printf("[%1d]: put data idin=%d\n",id,idin);*/
            in_error = et_events_put(et_sys,et_attach,etevents,nevents2put);

            j = id_out[0];
            for(i=0; i<NIDMAX-1; i++) id_out[i] = id_out[i+1];
            id_out[NIDMAX-1] = j;
          }
          pthread_cond_broadcast(&id_out_empty);
          out_unlock;

          if(in_error != ET_OK)
          {
            et_reinit = 1;
            ebp->force_end = 4;
          }
        }

        /* dump events we're not going to use */
        if(nevents2dump > 0)
        {
          /* printf("DEBUG: AAA: nevents2dump = %d.\n", nevents2dump); */
          in_error =
                 et_events_dump(et_sys,et_attach,etevents_dump,nevents2dump);
          if(in_error != ET_OK)
          {
            /* printf("DEBUG: et_events_dump failed..\n"); */
            et_reinit = 1;
            ebp->force_end = 4;
          }
        }

        /* reset variables */
        nevents2put  = 0;
        nevents2dump  = 0;
        if(neventsfree < 1) neventsfree = 0;
      }
    }
if(ebp->end_event_done) printf("[%1d] output_event 2\n",id);fflush(stdout);

    current_evty = -1;
    current_sync = 0;
    current_syncerr = 0;
    current_evtyerr = 0;

    total_length = 0;


end2 = gethrtime();
time2 += ((end2-start2)/NANOMICRO);
time3 += ((end2-start2)/NANOMICRO);
if(++nevtime2 == NPROF2)
{
  /*
  printf("<<< total=%7lld microsec, build=%7lld microsec >>>\n",
    time2/nevtime2,time3/nevtime2);
  */
  nevtime2 = 0;
  time2 = 0;
  time3 = 0;
}


  } while (!(ebp->force_end || ebp->ended));
  /*****************
  * MAIN LOOP ENDS *
  *****************/


  /* lock and clean everything up */
/*data_lock(ebp);*/

  /************************/
  /* handle_build cleanup */
  /************************/
  printf("[%1d] handle_build_cleanup\n",arg->id);fflush(stdout);

  /* May still be events not written to ET system - flush; */
  /* that part is completely local, do not need to lock,   */
  /* we do not care about event ordering at that moment    */
  if(et_reinit == 0 && ebp->end_event_done == 0)
  {
    printf("[%1d] flush ET system\n",id);
    fflush(stdout);

    /* real events to be put into the system */
    if(nevents2put > 0)
    {
      et_events_put(et_sys, et_attach, etevents, nevents2put);
      printf("[%1d] put %d events to ET system\n",id,nevents2put);
    }
    /* events that won't be used and will be dumped */
    if(nevents2dump > 0)
    {
      et_events_dump(et_sys, et_attach, etevents_dump, nevents2dump);
      printf("[%1d] dump %d events to ET system\n",id,nevents2dump);
    }
    /* new events left over after end event */
    if(neventsfree > 0)
    {
      et_events_dump(et_sys, et_attach, &etevents[(neventsnew-neventsfree)],
                     neventsfree);
      printf("[%1d] dump %d free events to ET system\n",id,neventsfree);
    }
  }
  free(etevents);
  free(etevents_dump);

  /* detach from ET system */
  if(et_station_detach(et_sys, et_attach) < 0)
  {
    printf("[%1d] ERROR: cannot detach from ET station\n",id);
  }
  else
  {
    printf("[%1d] detached from ET\n",id);
  }

  /* shutdown fifos; it is enough to do it by one thread,
  but lets all threads do it, it is not harm (is it ?) */
  printf("[%1d] remove mutex locks and shutdown fifos\n",arg->id);
  for(i=0; i<MAX_ROCS; i++)
  {
    CIRCBUF *f = roc_queues[i];
    f->deleting = 1;

    /* remove 'read' locks */
    if(pthread_mutex_trylock(&f->read_lock))
    {
      printf("[%1d] Mutex for %s was read-locked so unlock it\n",
        arg->id,get_cb_name(f));
      fflush(stdout);
    }
    pthread_mutex_unlock(&f->read_lock);

    /* remove 'write' locks */
    if(pthread_mutex_trylock(&f->write_lock))
    {
      printf("[%1d] Mutex for %s was write-locked so unlock it\n",
        arg->id,get_cb_name(f));
      fflush(stdout);
    }
    pthread_mutex_unlock(&f->write_lock);

    /* flush input streams */
    printf("[%1d] flush input streams\n",arg->id);
    do
    {
      printf("[%1d] count for %s = %d\n",
        arg->id,get_cb_name(f),get_cb_count(&f));
      fflush(stdout);
      if(get_cb_count(&f) <= 0) break;
      nevbuf = get_cb_data(&f,arg->id,chunk,evptr[i],&lenbuf,&rocid);
    } while(nevbuf != -1);
  }

  printf("[%1d] ============= Build threads cleaned\n",arg->id);
  printf("[%1d] ============= Build threads cleaned\n",arg->id);
  printf("[%1d] ============= Build threads cleaned\n",arg->id);
  printf("[%1d] ============= Build threads cleaned\n",arg->id);
  printf("[%1d] ============= Build threads cleaned\n",arg->id);
  printf("[%1d] build thread exiting: %d %d\n",
    id,ebp->force_end,ebp->ended); fflush(stdout);
  ebp->force_end = 0;
  roc_queue_ix = 0;

/*data_unlock(ebp);*/


  /* zero thread pointer; 'deb_end' will wait
  until all thread pointers are zero */
  ebp->idth[id] = NULL;


  /* detach a thread */
  itmp = pthread_detach(pthread_self());
  if(itmp==0)
  {
    printf("[%1d] handle_build thread detached\n",id); fflush(stdout);  
  }
  else if(itmp==EINVAL)
  {
    printf("[%1d] The implementation has detected that the value",id);
    printf("  specified by thread does not refer to a joinable thread.\n");
  }
  else if(itmp==ESRCH)
  {
    printf("[%1d] No thread could be found corresponding to that",id);
    printf("  specified by the given thread  ID.");
  }

  /* terminate calling thread */
  pthread_exit(NULL);

  return;
}


/* it effects all threads, should NOT be called from particular thread ! */
#define SHUTDOWN_BUILD \
  if(ebp->nthreads != 0) \
  { \
    int itmp = 0; \
    printf("cancel building threads\n"); \
    for(id=0; id<ebp->nthreads; id++) \
    { \
      if(ebp->idth[id] != NULL) \
      { \
        pthread_t build_thread = ebp->idth[id]; \
        ebp->idth[id] = NULL; \
        pthread_cancel(build_thread); \
        itmp = pthread_join(ebp->idth[id], &status); \
        printf("status is 0x%08x, itmp=%d\n",status,itmp); \
        if(itmp==EINVAL) \
        { \
          printf("ERROR in pthread_join(): The implementation has detected that the \n"); \
          printf("   value specified by thread does not refer to a joinable thread.\n"); \
        } \
        else if(itmp==ESRCH) \
        { \
          printf("ERROR in pthread_join(): No thread could be found corresponding to \n"); \
          printf("   that specified by the given thread ID.\n"); \
        } \
        else if(itmp==EDEADLK) \
        { \
          printf("ERROR in pthread_join(): A recursive deadlock was detected, the value \n"); \
          printf("   of thread specifies the calling thread.\n"); \
        } \
        if(status && (!itmp)) free(status); \
      } \
    } \
    ebp->nthreads = 0; \
  } \
  debcloselinks()


/***************************/
/* following is for ending */
/***************************/
int
polling_routine()
{
  objClass object = localobject;

  EBp ebp = (EBp) object->private;
  void *status;
  int id;
  int *ptr;

  if(ebp->ended == 1)
  {
printf("polling_routine ================!!!!!!!!!!!!!!!!!!=====\n");
fflush(stdout);

	SHUTDOWN_BUILD;

    printf("INFO: ended5\n");fflush(stdout);
    ebp->ended = 0;
    printf("INFO: ended4\n");fflush(stdout);
    ebp->ending = 0;
    printf("INFO: ended3\n");fflush(stdout);
    printf("INFO: ended31 (0x%08x)\n",object);fflush(stdout);
    printf("INFO: ended32 (0x%08x)\n",object->state);fflush(stdout);
	/*sergey*/
    ckfree(object->state);
    printf("INFO: ended2 (0x%08x)\n",object->state);fflush(stdout);
    object->state = ckalloc(12);
    printf("INFO: ended1\n");fflush(stdout);
    tcpState = DA_DOWNLOADED;
    codaUpdateStatus("downloaded");
    printf("INFO: ended\n");fflush(stdout);
  }

  return(TCL_OK);
}

void
eb_ended_loop()
{
  printf("eb_ended_loop started\n");fflush(stdout);
  printf("eb_ended_loop started\n");fflush(stdout);
  printf("eb_ended_loop started\n");fflush(stdout);
  printf("eb_ended_loop started\n");fflush(stdout);
  printf("eb_ended_loop started\n");fflush(stdout);

  while(1)
  {
    if(ended_loop_exit)
    {
      ended_loop_exit = 0;
      return;
    }

    polling_routine();

    /* wait 1 sec */
    sleep(1);
  }

  pthread_exit(NULL);

  return;
}



/***************/
/* TCL methods */
/***************/


TCL_PROC(deb_constructor)
{
  int i, j;
  char tmp[400];
  EBp ebp, *ebh;
  pthread_mutexattr_t mattr;

  /* tell anyone watching */
  sprintf(tmp,"%s {%s} %s {%s}",__FILE__,"Tue Aug  9 11:07:14 EST 2005","boiarino",
          "$Id: deb_component.c,v 2.118 2000/02/22 20:37:28 rwm Exp $");
  /*
  sprintf(tmp,"%s {%s} %s {%s}",__FILE__,DAYTIME,CODA_USER,
          "$Id: deb_component.c,v 2.118 2000/02/22 20:37:28 rwm Exp $");
  */
  Tcl_SetVar(interp,"tcl_modules",tmp,
             TCL_LIST_ELEMENT|TCL_APPEND_VALUE|TCL_GLOBAL_ONLY);



  localobject = object;



  /* allocate and clean up structure 'eb_priv' */
  /* will be accessed by 'object->private' */
  ebp = object->private = (void *) ckalloc(sizeof(eb_priv));
  bzero((char *)ebp, sizeof(eb_priv));

  /* allocate and activate pointer to structure 'eb_priv' */
  ebh = (EBp *) ckalloc(sizeof(EBp));
  *ebh = ebp;

  /* building threads did not started yet */
  ebp->nthreads = 0;
  for(i=0; i<NTHREADMAX; i++) ebp->idth[i] = NULL;

  /* allocate and initialize .. */
  for(i=0; i<MAX_ROCS; i++)
  {
    roc_queues[i] = new_cb(i,"roc","EventBuilder");
    ebp->roc_stream[i] = &roc_queues[i];
  }
  cb_events_init(roc_queues); /* dummy */



#ifdef NOALLOC
  printf("creating pool of buffers ..\n"); fflush(stdout);
  for(j=0; j<MAX_ROCS; j++)
  {
    for(i=0; i<QSIZE; i++)
    {
      bufpool[j][i] = (unsigned int *) malloc(TOTAL_RECEIVE_BUF_SIZE+128);
      if(bufpool[j][i] == NULL)
      {
        printf("ERROR: cannot allocate buffer - exit.\n");
        fflush(stdout);
        exit(0);
      }
    }
  }
  printf(".. done.\n"); fflush(stdout);
#endif



  roc_queue_ix = 0; /* the number of ROCs; incremented in LINK_support.c */

  /* initialize mutex etc */
  pthread_mutexattr_init(&mattr);
  pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init (&ebp->active_mutex, &mattr); /* obsolete ? */
  pthread_cond_init (&ebp->active_cond, NULL); /* obsolete ? */
  pthread_mutex_init (&ebp->data_mutex, &mattr);
  pthread_cond_init (&ebp->data_cond, NULL);

  ebp->interp = interp; /* TCL interpreter */


  /* set 'session' variable in TCL ??? */
  if(Tcl_VarEval(interp, "set session ",argv[1], NULL) != TCL_OK)
    return(TCL_ERROR);


  /* start ending thread; it will check in loop if we are ended; if so,
     it will shutdown building thread etc */

  {
    int res;
    pthread_t thread1;
    pthread_attr_t detached_attr;

    pthread_attr_init(&detached_attr);
    pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setscope(&detached_attr, PTHREAD_SCOPE_SYSTEM);

    res = pthread_create( (unsigned int *) &thread1, &detached_attr,
		   (void *(*)(void *)) eb_ended_loop, (void *) NULL);

    printf("pthread_create returned %d\n",res);fflush(stdout);
    perror("pthread_create");
  }

  tcpState = DA_BOOTED;
  if(codaUpdateStatus("booted") != TCL_OK) return(TCL_ERROR);

  tcpServer(localobject->name); /*start server to process non-coda commands sent by tcpClient*/

  return(TCL_OK);
}


TCL_PROC(deb_destructor)
{
  EBp ebp = (void *) object->private;
  void *status;
  int id;

  ebp->active = 0; /* obsolete ? */

  SHUTDOWN_BUILD;
  ended_loop_exit = 1;

  return(TCL_OK);
}







int
codaDownload(char *confname)
{
  objClass object = localobject;

  EBp  ebp = (void *) object->private;
  int  id, ix;
  char tmp[1000], tmp2[1000], tmpp[1000];
  int  listArgc;
  char listArgv[LISTARGV1][LISTARGV2];
  MYSQL *dbsock;


/* swap following ??? */
  UDP_start();
  tcpState = DA_DOWNLOADING;
  if(codaUpdateStatus("downloading") != DEB_OK) return(DEB_ERROR);


  ebp->force_end = 0;
  if(ebp->nthreads != 0)
  {
    printf("WARN: Can't download while %d build threads are active, END first.\n",
             ebp->nthreads);
    return(DEB_ERROR);
  }

  for(id=0; id<nthreads; id++)
  {
    if(ebp->out_id[id])
    {
      printf("INFO: Unloading event encode module %x\n",ebp->out_id[id]);
      if(dlclose ((void *) ebp->out_id[id]) != 0)
      {
        printf("download: failed to unload module to encode >%s<\n",
          ebp->out_name);
        return(DEB_ERROR);
      }
    }
  }


  strcpy(configname,confname); /* Sergey: save CODA configuration name */
  printf("INFO: Downloading configuration '%s'\n",configname);
  strcpy(ebp->out_name,"CODA");




  /* connect to database */
  dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));
  if(dbsock==NULL)
  {
    printf("cannot connect to the database 1 - exit\n");
    exit(0);
  }
  printf("312: dbsock=0x%08x\n",dbsock);

  /* get the list of readout-lists from the database */
  sprintf(tmpp,"SELECT code FROM %s WHERE name='%s'",configname,object->name);
  if(dbGetStr(dbsock, tmpp, tmp)==DEB_ERROR) return(DEB_ERROR);
  printf("code >%s< selected\n",tmp);

  /* disconnect from database */
  dbDisconnect(dbsock);



  /*
   * Decode configuration string...
   */
  listArgc = 0;
  if(!((strcmp (tmp, "{}") == 0)||(strcmp (tmp, "") == 0)))
  {
    if(listSplit1(tmp, 1, &listArgc, listArgv)) return(DEB_ERROR);
    for(ix=0; ix<listArgc; ix++) printf("nrols [%1d] >%s<\n",ix,listArgv[ix]);
  }
  else
  {
    printf("download: do not split list >%s<\n",tmp);
  }


#ifdef LINK_BOS_FORMAT
  printf("Use linked-in BOS format\n");
  deflt = 0;
#else
#ifdef LINK_CODA_FORMAT
  printf("Use linked-in CODA format\n");
  deflt = 0;
#else
  /*
   * Get object filename in order to find the loadable file
   */
  if(listArgc == 2)
  {
    strcpy(ebp->out_name,listArgv[1]);
    /*
     * Load the encode module
     */
    for(id=0; id<nthreads; id++)
    {
      sprintf(tmp,"%s/%s_format_%1d.so",getenv("CODA_LIB"),listArgv[1],id);
      ebp->out_id[id] = dlopen ((char *) tmp, RTLD_NOW | RTLD_GLOBAL);
      if(ebp->out_id[id] == 0)
      {
        printf("WARN: dlopen failed on Output file >%s<\n",tmp);
        printf("WARN: >%s<\n",dlerror());
        deflt = 1;
      }
      else
      {
        printf("INFO: file(out) >%s< is loaded at 0x%08x\n",
          tmp,ebp->out_id[id]);
      }
    }
  }
  else
  {
    deflt = 1;
	/*UNEFINED argv ..
    printf("WARN: row %s table %s no code entry CODA fmt is deflt.\n",
              object->name,argv[1]);
	*/
  }
  
  /*
   * Now look up the routines in the library and fill in the tables
   *
   */
  if(deflt)
  {
    printf("INFO: Using inbuilt (CODA) format\n");
    for(id=0; id<nthreads; id++)
    {
      out_procs[id][EVENT_RESERV_FRAG] = CODA_reserv_frag;
      out_procs[id][EVENT_RESERV_HEAD] = CODA_reserv_head;
      out_procs[id][EVENT_RESERV_DESC] = CODA_reserv_desc;
      out_procs[id][EVENT_ENCODE_FRAG] = CODA_encode_frag;
      out_procs[id][EVENT_ENCODE_HEAD] = CODA_encode_head;
      out_procs[id][EVENT_ENCODE_DESC] = CODA_encode_desc;
      out_procs[id][EVENT_ENCODE_SPEC] = CODA_encode_spec;
      out_procs[id][EVENT_DECODE_FRAG] = CODA_decode_frag;
      out_procs[id][EVENT_DECODE_HEAD] = CODA_decode_head;
      out_procs[id][EVENT_DECODE_DESC] = CODA_decode_desc;
      out_procs[id][EVENT_DECODE_SPEC] = CODA_decode_spec;
    }
  }
  else
  {
    IFUNCPTR proc;
    for(id=0; id<nthreads; id++)
    {
      sprintf(tmp,"%s_reserv_frag",ebp->out_name);
      out_procs[id][EVENT_RESERV_FRAG] = (IFUNCPTR) dlsym(ebp->out_id[id],tmp);
      sprintf(tmp,"%s_reserv_head",ebp->out_name);
      out_procs[id][EVENT_RESERV_HEAD] = (IFUNCPTR) dlsym(ebp->out_id[id],tmp);
      sprintf(tmp,"%s_reserv_desc",ebp->out_name);
      out_procs[id][EVENT_RESERV_DESC] = (IFUNCPTR) dlsym(ebp->out_id[id],tmp);
      sprintf(tmp,"%s_encode_frag",ebp->out_name);
      out_procs[id][EVENT_ENCODE_FRAG] = (IFUNCPTR) dlsym(ebp->out_id[id],tmp);
      sprintf(tmp,"%s_encode_head",ebp->out_name);
      out_procs[id][EVENT_ENCODE_HEAD] = (IFUNCPTR) dlsym(ebp->out_id[id],tmp);
      sprintf(tmp,"%s_encode_desc",ebp->out_name);
      out_procs[id][EVENT_ENCODE_DESC] = (IFUNCPTR) dlsym(ebp->out_id[id],tmp);
      sprintf(tmp,"%s_encode_spec",ebp->out_name);
      out_procs[id][EVENT_ENCODE_SPEC] = (IFUNCPTR) dlsym(ebp->out_id[id],tmp);
      sprintf(tmp,"%s_decode_frag",ebp->out_name);
      out_procs[id][EVENT_DECODE_FRAG] = (IFUNCPTR) dlsym(ebp->out_id[id],tmp);
      sprintf(tmp,"%s_decode_head",ebp->out_name);
      out_procs[id][EVENT_DECODE_HEAD] = (IFUNCPTR) dlsym(ebp->out_id[id],tmp);
      sprintf(tmp,"%s_decode_desc",ebp->out_name);
      out_procs[id][EVENT_DECODE_DESC] = (IFUNCPTR) dlsym(ebp->out_id[id],tmp);
      sprintf(tmp,"%s_decode_spec",ebp->out_name);
      out_procs[id][EVENT_DECODE_SPEC] = (IFUNCPTR) dlsym(ebp->out_id[id],tmp);
    }
    printf("INFO: Loaded event building format %s\n",ebp->out_name);
  }
#endif
#endif


/* do we still need it ??? */

#ifdef SunOS
  printf("thread concurrency level is %d\n",thr_getconcurrency());
  thr_setconcurrency(40);
  printf("thread concurrency level set to %d\n",thr_getconcurrency());
#endif



  tcpState = DA_DOWNLOADED;
  if(codaUpdateStatus("downloaded") != DEB_OK) return(DEB_ERROR);


/*sergey: just test
tmpUpdateStatistics();
*/

  return(DEB_OK);
}







int
debopenlinks()
{
  objClass object = localobject;

  EBp ebp = (void *) localobject->private;
  DATA_LINK links[MAX_ROCS];
  int nrocs, rocid, i, len, ix, numRows;
  MYSQL *dbsock;
  MYSQL_RES *result;
  MYSQL_ROW row;
  char tmp[2000], tmpp[1000], tohost[100];

printf("=o=============================================\n");fflush(stdout);
printf("=o=============================================\n");fflush(stdout);
printf("=o=============================================\n");fflush(stdout);
printf("debopenlinks reached\n");fflush(stdout);


  for(ix=0; ix<MAX_ROCS; ix++)
  {
	ebp->roc_id[ix] = -1;
	ebp->links[ix] = NULL;
  }



/*sergey*/
roc_queue_ix = 0;





  dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));
  if(dbsock==NULL)
  {
    printf("cannot connect to the database 2 - exit\n");
    exit(0);
  }

  sprintf(tmp,"SELECT name,inputs,outputs,next FROM %s WHERE name='%s'",
    configname,object->name);
  if(mysql_query(dbsock, tmp) != 0)
  {
    printf("ERROR: cannot select\n");
    return(DEB_ERROR);
  }
  else
  {
    printf("selected\n");
  }



  /* gets results from previous query */
  if( !(result = mysql_store_result(dbsock)) )
  {
    printf("ERROR in mysql_store_result()\n");
    return(TCL_ERROR);
  }
  else
  {
    numRows = mysql_num_rows(result);
    printf("nrow=%d\n",numRows);
    if(numRows != 1)
    {
      printf("ERROR: numRows=%d, must be 1\n",numRows);
	}
    else
    {
      if((row = mysql_fetch_row(result)))
	  {
        /* extract fields from the Event Builder line in config table, for example:
	         name                  inputs                    outputs       next 
	        >EB5< >croctest1:croctest1 croctest2:croctest2<     ><           ><   */

        printf("name>%s< inputs>%s< outputs>%s< next>%s<\n",
          row[0],row[1],row[2],row[3]);

        strcpy(tmp,row[1]);
        printf("tmp >%s<\n",tmp);

        linkArgc = 0;
        if(!((strcmp(tmp, "") == 0)||(strlen(tmp) < 3)))
        {
          if(listSplit1(tmp, 0, &linkArgc, linkArgv)) return(DEB_ERROR);
          for(ix=0; ix<linkArgc; ix++)
		  {
            printf("input1 [%1d] >%s<\n",ix,linkArgv[ix]);
		  }
          /* replace ':' by the end of string */
          for(ix=0; ix<linkArgc; ix++)
		  {
            len = strlen(linkArgv[ix]);
            for(i=0; i<len; i++)
            {
              if(linkArgv[ix][i] == ':')
              {
                linkArgv[ix][i] = '\0';
                break;
              }
            }
		  }
          for(ix=0; ix<linkArgc; ix++)
		  {
            printf("input2 [%1d] >%s<\n",ix,linkArgv[ix]);
		  }
	    }
      }
      else
      {
        printf("ERROR: do not split inputs >%s<\n",tmp);
      }

    }

    mysql_free_result(result);
  }


  /* for every ROC */
  for(ix=0; ix<linkArgc; ix++)
  {
    /* get the host name where it suppose to send data */
	sprintf(tmp,"SELECT outputs FROM %s WHERE name ='%s'",
      configname,linkArgv[ix]);
    if(dbGetStr(dbsock, tmp, tmpp)==DEB_ERROR) return(DEB_ERROR);
    printf("tmpp>%s<\n",tmpp);
    len = strlen(tmpp);
    for(i=0; i<len; i++)
    {
      if(tmpp[i]==':')
      {
        strncpy(tohost,&tmpp[i+1],(len-i)); /*copy starting from the symbol right after ':' */
        tohost[len-i] = '\0';
        break;
      }
    }
    printf("tohost>%s<\n",tohost);

    /* create link */
	printf("debOpenLink: >%s< >%s< >%s< 0x%08x\n",linkArgv[ix],object->name,tohost, dbsock);

    ebp->links[ix] = debOpenLink(linkArgv[ix], object->name, tohost, dbsock);
    printf(">>>>> open link from >%s< link=0x%08x\n",linkArgv[ix],ebp->links[ix]);
  }


  /* for every ROC */
  nrocs = 0;
  for(ix=0; ix<linkArgc; ix++)
  {    
    /* get 'first' field, can be 'yes' or 'no' 
	sprintf(tmp,"SELECT first FROM %s WHERE name ='%s'",
      configname,linkArgv[ix]);
    if(dbGetStr(dbsock, tmp, tmpp)==DEB_ERROR) return(DEB_ERROR);
    printf("tmpp>%s<\n",tmpp);
*/

    /* get ROC id from process table */
	sprintf(tmp,"SELECT id FROM process WHERE name ='%s'",linkArgv[ix]);
    if(dbGetInt(dbsock, tmp, &rocid)==DEB_ERROR) return(DEB_ERROR);
    printf("rocid=%d\n",rocid);

    /* check status of 'first' ROC (WHAT ABOUT OTHERS ?????!!!!!) 
    if(!strncmp(tmpp,"yes",3))
    {*/
      /*
	  if { [catch "DP_ask $linkArgv[ix] status"] } {
		puts "ERROR: Data link $linkArgv[ix] cannot be started, target system down"
	  }
	   */
    /*}*/

	ebp->roc_id[nrocs] = rocid; /* our rocid: DC1 is 1, CC1 is 12 etc */



	ebp->roc_nb[rocid] = nrocs; /* roc numbers: 0,1,2,... */




	nrocs ++;
  }
  ebp->nrocs = nrocs;

  dbDisconnect(dbsock);








printf("debopenlinks done\n");fflush(stdout);
printf("=o=============================================\n");fflush(stdout);
printf("=o=============================================\n");fflush(stdout);
printf("=o=============================================\n");fflush(stdout);

  return(DEB_OK);
}



int
debcloselinks()
{
  int ix;
  MYSQL *dbsock;
  EBp ebp = (void *) localobject->private;

printf("=c=============================================\n");fflush(stdout);
printf("=c=============================================\n");fflush(stdout);
printf("=c=============================================\n");fflush(stdout);

  dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));
  if(dbsock==NULL)
  {
    printf("cannot connect to the database 3 - exit\n");
    exit(0);
  }

  for(ix=0; ix<linkArgc; ix++)
  {
	printf("=c====\n");fflush(stdout);
    printf(">>>>> close link from >%s< link=0x%08x\n",
      linkArgv[ix],ebp->links[ix]);
    debCloseLink(ebp->links[ix], dbsock);
  }
  linkArgc = 0;

  dbDisconnect(dbsock);

printf("=cc============================================\n");fflush(stdout);
printf("=cc============================================\n");fflush(stdout);
printf("=cc============================================\n");fflush(stdout);

  return(DEB_OK);
}










int
codaPrestart()
{
  objClass object = localobject;

  EBp ebp = (void *) object->private;
  int i, ix, id, async, numRows;
  int waitforET = 2*(ET_MON_SEC + 1);
static char temp[100];
static trArg args[NTHREADMAX];
  void *status;
  char tmp[1000], tmpp[1000];
  MYSQL *dbsocket;
  MYSQL_RES *result;
  MYSQL_ROW row;
  unsigned int roc_mask_local;
  int roc_id_local;

#ifdef SunOS
  printf("thread concurrency level is %d\n",thr_getconcurrency());
  thr_setconcurrency(30);
#endif



  debcloselinks();
  sleep(1);

  /* cleanup another ROCs mask; will be filled by 'handle_link' calles
  started by 'debopenlinks' and compared with 'ebp->roc_mask' in the begining
  of building thread */
  roc_linked = 0;
  sleep(1);

  debopenlinks();


  printf("INFO: Prestarting (C)\n");

  /*******************************/
  /* initialize circular buffers */

  for(i=0; i<MAX_ROCS; i++) cb_init(i);

  /*******************/
  /* setup ET system */

  /* If we need to initialize, reinitialize */
  if((et_init == 0) || (et_reinit == 1))
  {
    if(eb_et_initialize() != DEB_OK)
    {
      printf("ERROR: deb prestart: cannot initialize ET system\n");
      return(DEB_ERROR);
    }
  }

  /* If this is a Linux system check server */
  if((!et_alive(et_sys)) && (et_locality == ET_LOCAL_NOSHARE))
  {	
    if(eb_et_initialize() != DEB_OK)
    {
      printf("ERROR: deb prestart: cannot initialize ET system\n");
      return(DEB_ERROR);
    }
  }
  else if((!et_alive(et_sys)))
  {
    if(waitforET < 5) waitforET = 5;
    sleep(waitforET);
    if(!et_alive(et_sys))
    {
      printf("ERROR: deb prestart: ET system is not responding\n");
      return(DEB_ERROR);
    }
  }
  else
  {
    printf("INFO: deb prestart: ET is alive - EB attached\n");
  }

  /* init events ordering queues */
  id_in_index = NULL;
  id_out_index = NULL;
  for(id=0; id<NIDMAX; id++)
  {
    id_in[id] = id;
    id_out[id] = id;
  }
  pthread_mutex_init(&id_out_lock, NULL);
  pthread_cond_init(&id_out_empty, NULL);

  /* some cleanups */
  ebp->force_end = 0;
  ebp->ended = 0;
  ebp->end_event_done = 0;


  /* Initialize queues */
  for(ix=0; ix<MAX_ROCS; ix++) roc_queues[ix]->deleting = 0;

  /* cleanup event counters */
  nRUNEVENT = 0;
  nSCALERS = 0;

  nddl = NDDL;
  etDDLInit(nddl, ddl);
  printf("ddl=0x%08x size=%d\n",ddl,sizeof(DDL));

  {
    DDL *ddlptr;
    ddlptr = (DDL *)&ddl[83];
    printf("===> id=%d ddlptr=0x%08x\n",83,ddlptr);
    printf("===> ncol=%d\n",ddlptr->ncol);
  }




  /* connect to database */
  dbsocket = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));
  if(dbsocket==NULL)
  {
    printf("cannot connect to the database 4 - exit\n");
    exit(0);
  }

  /**********************/
  /* Get the run number */
  /**********************/
  sprintf(tmpp,"SELECT runNumber FROM sessions WHERE name='%s'",session);
  if(dbGetInt(dbsocket, tmpp, &(object->runNumber))==DEB_ERROR) return(DEB_ERROR);

  /******************************************************/
  /* get the run type number and save it somewhere safe */
  /******************************************************/
  sprintf(tmpp,"SELECT id FROM runTypes WHERE name='%s'",configname);
  if(dbGetInt(dbsocket, tmpp, &(object->runType))==DEB_ERROR) return(DEB_ERROR);

  printf("INFO: prestarting, run=%d, type=%d\n",object->runNumber,object->runType);

  object->nevents = 0;
  object->nlongs = 0;


  /**************************************************/
  /* check which rocs are active and setup roc mask */
  /**************************************************/


  /****************************/
  /* get 'inuse' ROCs from DB */
  /****************************/

  /* maybe better check for EB5:... in 'output' field ??? */

  /* get all rows with 'inuse'='yes' */
  /*sprintf(tmp,"SELECT name,outputs,inuse FROM %s WHERE inuse='yes'",configname);*/
  sprintf(tmp,"SELECT name,outputs,inuse FROM %s",configname);
  if(mysql_query(dbsocket, tmp) != 0)
  {
    printf("ERROR: cannot select\n");
    return(DEB_ERROR);
  }
  else
  {
    printf("selected\n");
  }


  /* gets results from previous query */
  if( !(result = mysql_store_result(dbsocket)) )
  {
    printf("ERROR in mysql_store_result()\n");
    return(TCL_ERROR);
  }
  else
  {
    numRows = mysql_num_rows(result);
    printf("nrow=%d\n",numRows);

    roc_mask_local = 0;
	for(ix=0; ix<numRows; ix++)
    {
      row = mysql_fetch_row(result);
      printf("[%1d] received from DB >%s< >%s< >%s<\n",ix,
        row[0],row[1],row[2]);

      if( strncmp(row[2],"no",2) != 0 ) /* 'inuse' != 'no' */
      {
        roc_id_local = atoi(row[2]);
        if((roc_id_local>=0) && (roc_id_local<=31))
        {
          roc_mask_local |= (1<<roc_id_local);
        }
      }
    }

    mysql_free_result(result);
  }


  ebp->roc_mask = roc_mask_local;
  printf("++++++++++++++++ roc_mask_local = 0x%08x\n",roc_mask_local);




  /**********************************************/
  /* set roc mask in mysql so TS can read it out */
  /**********************************************/
  sprintf(temp,"%d",ebp->roc_mask);
  sprintf(tmpp,"SELECT name,value FROM %s_option WHERE name='rocMask'",configname);
printf("mysql request >%s< (temp>%s<)\n",tmpp,temp);
  if(mysql_query(dbsocket, tmpp) != 0)
  {
    printf("ERROR in mysql_query\n");
  }
  else
  {
    if( !(result = mysql_store_result(dbsocket)) )
    {
      printf("ERROR in mysqlStoreResult()\n");
      return(TCL_ERROR);
    }
    else
    {
      numRows = mysql_num_rows(result);
      printf("nrow=%d\n",numRows);
      if(numRows == 0)
      {
        sprintf(tmpp,"INSERT INTO %s_option (name,value) VALUES ('rocMask','%s')",configname,temp);
        printf(">%s<\n",tmpp);
        if(mysql_query(dbsocket,tmpp) != 0)
        {
          printf("ERROR in INSERT\n");
        }
      }
      else if(numRows == 1)
      {
        sprintf(tmpp,"UPDATE %s_option SET value='%s' WHERE name='rocMask'\n",configname,temp);
        printf(">%s<\n",tmpp);
        if(mysql_query(dbsocket,tmpp) != 0)
        {
          printf("ERROR in UPDATE\n");
        }
      }
      else
      {
        printf("ERROR !!\n");
      }

      mysql_free_result(result);
	}
  }


  /* disconnect from database */
  dbDisconnect(dbsocket);









  /**************************/
  /* start building threads */
  /**************************/

  ebp->nthreads = nthreads;
  for(id=0; id<ebp->nthreads; id++)
  {
    pthread_attr_t detached_attr;

    pthread_attr_init(&detached_attr);
    pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setscope(&detached_attr, PTHREAD_SCOPE_SYSTEM);

    printf("Start building thread [%1d]\n",id);

    args[id] = (trArg) ckalloc(sizeof(TRARGS));
    args[id]->object = object;
    args[id]->interp = Main_Interp;
    args[id]->id = id;
    pthread_create(&ebp->idth[id], /*Sergey: try to detach NULL*/&detached_attr,
                   (void *(*)(void *)) handle_build, (void *) args[id]);
  }

  /*sergey: we are telling rcServer that EB is ready, so rcServer can go with rocs prestarting;
  'handle_build' will wait for all connections from all rocs; it may happens that some connection(s)
  will not be established, but all rocs will report to rcServer, in that case 'Go' button will
  appeares while EB still complaining 'waiting for the following ROC IDs'; need to do something
  about that ... */
  tcpState = DA_PAUSED;
  if(codaUpdateStatus("paused") != DEB_OK)
  {
    printf("--> return ERROR\n"); fflush(stdout);
    return(DEB_ERROR); 
  }

  printf("--> codaPrestart returns OK\n"); fflush(stdout);

  return(DEB_OK);
}

int
codaGo()
{
  objClass object = localobject;

  EBp ebp = (void *) object->private;

  printf("INFO: Activating (C)\n");
  ebp->ended = 0;
  ebp->end_event_done = 0;

  tcpState = DA_ACTIVE;
  if(codaUpdateStatus("active") != DEB_OK)

  return(DEB_OK);
}  


/* called ad 'End' transition; building thread must receive 'end'
command INDEPENDENTLY (through event type 20 or error condition)
and polling_routine will see it and end, so here we just waiting
for 'downloaded'; if it is not happeded during ... sec, we will
force end (TO DO) */
int
codaEnd()
{
  objClass object = localobject;

  int i, j, itmp, count;
  MYSQL *dbsocket;
  char tmp[1000], tmpp[1000];
  EBp ebp = (void *) object->private;

printf("codaEnd 1\n");fflush(stdout);

/*kuku: always downloaded*/
  /* get current state */
printf("codaEnd 2\n");fflush(stdout);
  dbsocket = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));
  if(dbsocket==NULL)
  {
    printf("cannot connect to the database 5 - exit\n");
    exit(0);
  }
printf("codaEnd 3\n");fflush(stdout);
  sprintf(tmp,"SELECT state FROM process WHERE name='%s'",object->name);
printf("codaEnd 4\n");fflush(stdout);
  if(dbGetStr(dbsocket, tmp, tmpp)==DEB_ERROR) return(DEB_ERROR);
printf("codaEnd 5\n");fflush(stdout);
printf(">>>>>>>>>> name >%s< state >%s<\n",object->name,tmpp);
  dbDisconnect(dbsocket);
printf("codaEnd 6\n");fflush(stdout);

  printf("deb_end reached\n");fflush(stdout);
  printf("deb_end: state is %s\n",tmpp);
printf("codaEnd 7\n");fflush(stdout);
  fflush(stdout);
  printf("===============================================\n");
  printf("===============================================\n");
  printf("===============================================\n");
  printf("===============================================\n");
  printf("===============================================\n");
  fflush(stdout);
  
printf("codaEnd 8\n");fflush(stdout);
  if(strcmp(tmpp,"downloaded") == 0)
  { 
    printf("INFO: already ended, got all end(s) from ROC(s)\n");
printf("codaEnd 81\n");fflush(stdout);
    tcpState = DA_DOWNLOADED;
    if(codaUpdateStatus("downloaded") != DEB_OK) return(DEB_ERROR);
printf("codaEnd 82\n");fflush(stdout);
  }
  else
  {
    printf("INFO: Ending (C)\n");


	/* SHOULD WAIT .. SEC and INITIATE FORCE END - TO DO */
	/*
    tcpState = DA_ENDING;
    if(codaUpdateStatus("ending") != DEB_OK) return(DEB_ERROR);
    ebp->ending = 1;
	*/

/*kuku: sometimes ending goes after downloaded; is it here ?? */
	/* Sergey: commented out for a while ..
Tcl_Eval(Main_Interp, "dp_after 60000 $this force_end");
	*/

  }
  







  /* wait until all threads are ended */
  count = 0;
printf("codaEnd 9\n");fflush(stdout);
  do
  {
    itmp = 0;
    for(i=0; i<ebp->nthreads; i++)
    {
      if(ebp->idth[i] != NULL) itmp ++;
    }
    if(itmp>0)
    {
      printf("deb_end: %d threads still alive - waiting %d times ..\n",
             itmp,count);
      count ++;
      sleep(1);
    }
    else
    {
      printf("deb_end: all threads are exited\n");
    }
printf("codaEnd 10\n");fflush(stdout);
	
    if(count > 5)
    {
	  /*
      pthread_cond_broadcast(&id_out_empty);
	  */
      printf("deb_end: set 'deleting=1' for all rocs\n");

      /* should call cb_delete() instead of following piece .. */
      for(j=0; j<MAX_ROCS; j++)
      {
        CIRCBUF *f = roc_queues[j];
        f->deleting = 1;
        pthread_cond_broadcast(&f->read_cond);
        pthread_cond_broadcast(&f->write_cond);
      }

    }
printf("codaEnd 11\n");fflush(stdout);

    if(count > 10) /* ????? */
    {
      printf("thread(s) do not respond - do not wait any more\n");
      for(i=0; i<NTHREADMAX; i++) ebp->idth[i] = NULL;
      break;
	}
	
  } while(itmp>0);
  ebp->nthreads = 0;
  /*
  for(i=0; i<NTHREADMAX; i++) ebp->idth[i] = NULL;
  */
  printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  fflush(stdout);

  return(DEB_OK);
}

int
codaPause()
{
  return(DEB_OK);
}


/* called from 'deb_end' on timeout */
int
debForceEnd()
{
  objClass object = localobject;

  EBp ebp = (void *) object->private;

  if(ebp->ending)
  {
    printf("Force end, close datalinks ..\n"); fflush(stdout);
    ebp->force_end = 99;
  }

  return(DEB_OK);
}


/* 'Reset' transition ('exit' message) */
int
debShutdownBuild()
{
  objClass object = localobject;

  EBp ebp = (void *) object->private;
  void *status;
  int id, i;

printf("debShutdownBuild 1\n");

  /* set force end flag */
  ebp->force_end = 1;

  for(i=0; i<MAX_ROCS; i++) cb_delete(i);
  /*SHUTDOWN_BUILD; do not need: cb_delete will force handle_build to exit*/
printf("debShutdownBuild 2\n");

  sleep(3); /* handle_build thread will change status to 'downloaded';
			   wait and change it to 'configured' */

printf("debShutdownBuild 3\n");
  tcpState = DA_CONFIGURED;
  codaUpdateStatus("configured");
printf("debShutdownBuild 4\n");

  return(DEB_OK);
}

int
codaExit()
{
  debShutdownBuild();
  /*
  UDP_reset();
  */
  return(DEB_OK);
}

/*******************************************************/
/*******************************************************/
/*******************************************************/


/*
void
eb_udp_loop()
{
  printf("eb_udp_loop started\n");fflush(stdout);
  printf("eb_udp_loop started\n");fflush(stdout);
  printf("eb_udp_loop started\n");fflush(stdout);
  printf("eb_udp_loop started\n");fflush(stdout);
  printf("eb_udp_loop started\n");fflush(stdout);

  udpsocket = UDP_establish(udphost, udpport);

  while(1)
  {
    if(udp_loop_exit)
    {
      udp_loop_exit = 0;
      UDP_close(udpsocket);
      return;
    }
    UDP_send(udpsocket);

    sleep(1);
  }

  pthread_exit(NULL);

  return;
}
*/




/***************************************************************************/
/********************* TCL interface functions *****************************/
/***************************************************************************/

TCL_PROC(deb_download)
{
  int ix;

  printf("deb_download: argc=%d\n",argc);
  for(ix=0; ix<argc; ix++) printf("deb_download: argv[%1d]=>%s<\n",ix,argv[ix]);

  if(argc != 2)
  {
    printf("deb_download: wrong # argums: should be %d\n",argv[0]);
    return(DEB_ERROR);
  }

  codaDownload(argv[1]);

  return(DEB_OK);
}

TCL_PROC(deb_prestart)
{
  codaPrestart();
}

TCL_PROC(deb_go)
{
  codaGo();
}

TCL_PROC(deb_end)
{
  codaEnd();
}


/**************/
/*EB-specifics*/

TCL_PROC(force_end)
{
  debForceEnd();
}

TCL_PROC(shutdown_build)
{
  debShutdownBuild();
}


/****************/
/* main program */
/****************/

void
main (int argc, char **argv)
{
printf("1\n");fflush(stdout);
  CODA_Init(argc, argv);
printf("2\n");fflush(stdout);
  /* CODA_Service ("ROC"); */
  CODA_Execute ();
}

#else

/* dummy VXWORKS version */

void
coda_eb()
{
  printf("coda_eb is dummy for VXWORKS\n");
}

#endif




