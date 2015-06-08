
/* roc_component.c */

#ifdef VXWORKS

#include <stdio.h>
#include <string.h>
#include <sockLib.h>
#include <errno.h>
#include <errnoLib.h>

extern long vxTicks;

#define ulong  unsigned long

#define MYCLOCK 25

#else

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
/*
#include <dlfcn.h>
*/
#include <sys/mman.h>

#ifdef Linux
#include <unistd.h>
#include <linux/prctl.h>
#endif

#define MYCLOCK NANOMICRO

#if defined(Linux) || defined(Darwin)
#define ulong  unsigned long
#endif

#endif

#ifdef VXWORKS
static int iTaskROL=0;
static int iTaskPROC=0;
static int iTaskNET=0;
#else
static pthread_t iTaskROL;
static pthread_t iTaskPROC;
static pthread_t iTaskNET;
static pthread_attr_t detached_attr;
#endif

#ifdef Linux
int tcpServer(char *name);
#endif


#include "rc.h"
#include "da.h"
#include "circbuf.h"
#include "bigbuf.h"

#define CODA_ERROR 1
#define CODA_OK 0



#ifndef VXWORKS
#ifdef __cplusplus
typedef void 		(*VOIDFUNCPTR) (...); /* ptr to function returning void */
#else
typedef void 		(*VOIDFUNCPTR) (); /* ptr to function returning void */
#endif			/* _cplusplus */
#endif
#include "rolInt.h"



typedef struct roc_private_store *rocParam;
typedef struct roc_private_store
{
  ROLPARAMS *rolPs[2];       /* pointers to readout lists */
  int        async_roc_flag; /* if =1 then roc communication with eb inhibited */
  /*int        recNb; use global rocp_ recNb*/         /* number of records sent by this ROC */
  int        active;         /* set to 1 if we are supposed to take data */
  int        state;          /* current state as a numeric value */
  /*int        last_event; use global 'last_event_check'*/     /* number of last event sent on network */
  /*int        primefd; use global rocp_primefd*/
  int        output_switch;
} roc_priv;

/* following structure will be allocated once in constructor */
static ROLPARAMS *rolPs[2];

int rocMask = 0;

/*static*/ int object_nlongs; /* instead of object->nlongs*/
/*static*/ long clear_to_send = 0;
/*static*/ int last_event_check = 0;
/*static*/ int rocp_primefd;
/*static*/ int rocp_recNb;

#ifdef VXWORKS
/*static*/ int timeout;
#else
/*static*/ /*time_t*/hrtime_t timeout;
#endif


/* Global Symbols */
int token_interval;

/* for the time profiling */
static int icycle;
static int cycle;
static int icycle3;
static int cycle3;
#ifdef VXWORKS
static unsigned long time3;
#else
#ifndef Darwin
static hrtime_t time3;
#endif
#endif

extern char    *session; /* coda_component.c */

#include "libdb.h"


/* minimum memory space left after big buffers and event buffers allocation */
#define MIN_MEM_LEFT (1024 * 1024 * 3)

/* the number of output buffers */
/* buffer size defined in circbuf.h */
#ifdef VXWORKS
#define NUM_SEND_BUFS        8
#else
#define NUM_SEND_BUFS        16
#endif

static int print_output_buffers_are_full = 1; /* enable warning message */
/* was reenabled in LINK_sized_write, removed temporarily */

/*static*/ int tsendBufSize = 0; /* total buffer size; can be <= SEND_BUF_SIZE */

/*static*/ unsigned long g_events_in_buffer = 0;

/* socket number */
static int socketnum = 0;

/* ROC id */
/*static*/ int this_roc_id = 0;

/* two pointers used to store ROL2 output to 'big' buffers */
/*static*/ unsigned int *dabufp; /* non-static to be used by ROL1 */
/*static*/ unsigned int dataInBuf = BBHEAD_BYTES;

#ifndef VXWORKS
unsigned int dabufp_usermembase; /* non-static to be used by ROL1 */
unsigned int dabufp_physmembase; /* non-static to be used by ROL1 */
#endif



/* net_thread structure, buffers from it to be used by 'coda_proc', the rest by 'coda_net' */
/*static*/ BIGNET big0;

/* big buffers to be used between 'coda_proc' and 'coda_net' if both are on host*/
static BIGBUF *gbigBuffer1 = NULL;

/* net_thread structure, contains pointer to the gbigBuffer1 - to be used
 between 'coda_proc' and 'coda_net' if both are on host*/
static BIGNET big1;


extern char configname[128]; /* coda_component.c */

/*static*/extern objClass localobject;
void output_proc_network(int bla/*struct alist *output*/);

#ifdef VXWORKS
static volatile PMC localpmc; /* host-pmc communication structure */
#endif
static int proc_on_pmc = 0; /* 1 - run coda_proc on pmc, 0 - on host board */
static int net_on_pmc = 0; /* 1 - run coda_net on pmc, 0 - on host board */


int LINK_establish(char *host, int port);
int LINK_close(int socket);

#ifdef VXWORKS
void proc_thread(BIGNET *bigprocptr, unsigned int offset);
void net_thread(BIGNET *bignetptr, unsigned int offset);
#else
void proc_thread(BIGNET *bigprocptr);
void net_thread(BIGNET *bignetptr);
#endif


void rols_loop();
static int rols_loop_exit = 0;



/* semaphore to synchronize CODA transitions and output_proc_network;
second parameter in semTake() is in 'ticks', we'll wait 5 seconds */

#ifdef VXWORKS
static SEM_ID transition_lock;
#define TRANSITION_LOCK semTake(transition_lock, /*WAIT_FOREVER*/5*sysClkRateGet())
#define TRANSITION_UNLOCK semGive(transition_lock)
#else
static pthread_mutex_t transition_lock;
#define TRANSITION_LOCK pthread_mutex_lock(&transition_lock)
#define TRANSITION_UNLOCK pthread_mutex_unlock(&transition_lock)
#endif


#ifdef VXWORKS
/*static*/ SEM_ID sendbuffer_lock;
#define SENDBUFFER_LOCK semTake(sendbuffer_lock, WAIT_FOREVER)
#define SENDBUFFER_UNLOCK semGive(sendbuffer_lock)
#else
/*static*/ pthread_mutex_t sendbuffer_lock;
#define SENDBUFFER_LOCK pthread_mutex_lock(&sendbuffer_lock)
#define SENDBUFFER_UNLOCK pthread_mutex_unlock(&sendbuffer_lock)
#endif

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

#ifdef VXWORKS

#include <bootLib.h>

void
proconpmc()
{
  proc_on_pmc = 1;
  big0.proc_on_pmc = 1;
  big1.proc_on_pmc = 1;
}
void
proconhost()
{
  proc_on_pmc = 0;
  big0.proc_on_pmc = 0;
  big1.proc_on_pmc = 0;
}
int
getproconpmc()
{
  return(big0.proc_on_pmc);
}
void
printproconpmc()
{
  if(big0.proc_on_pmc == 1)
  {
    printf("will start 'coda_proc' on pmc board\n");
  }
  else
  {
    printf("will run 'coda_proc' on host board in ROL2\n");
  }
}



void
netonpmc()
{
  net_on_pmc = 1;
  big0.net_on_pmc = 1;
  big1.net_on_pmc = 1;
}
void
netonhost()
{
  net_on_pmc = 0;
  big0.net_on_pmc = 0;
  big1.net_on_pmc = 0;
}
int
getnetonpmc()
{
  return(big1.net_on_pmc);
}
void
printnetonpmc()
{
  if(big1.net_on_pmc == 1)
  {
    printf("will start 'coda_net' on pmc board\n");
  }
  else
  {
    printf("will start 'coda_net' on host board\n");
  }
}


/* local memory */
#define BOOTLEN 1024
static char boot_line[BOOTLEN];
static BOOT_PARAMS boot_params;
static char targetname[128];
static char hostname[128];
static char rocname[128];

/* name of the computer we are booting from (for example clon10-daq1) */
char *
hostName()
{
  /*printf("hostName reached\n");fflush(stdout);*/
  sysNvRamGet(boot_line,BOOTLEN,0);
  bootStringToStruct(boot_line,&boot_params);
  strcpy(hostname,boot_params.hostName);  

  return((char*)hostname);
}

/* name of current cpu (for example dc1, or dc1pmc1 etc) */
char *
targetName()
{
  /*printf("targetName reached\n");fflush(stdout);*/
  sysNvRamGet(boot_line,BOOTLEN,0);
  bootStringToStruct(boot_line,&boot_params);
  strcpy(targetname,boot_params.targetName);  
  /*printf("targetName reached >%s<\n",targetname);fflush(stdout);*/

  return((char*)targetname);
}

/* name of current roc, return the same name on roc itself or any pmc's (on dc1pmc1 it will return dc1) */
char *
rocName()
{
  /*printf("rocName reached\n");fflush(stdout);*/
  sysNvRamGet(boot_line,BOOTLEN,0);
  bootStringToStruct(boot_line,&boot_params);
  strcpy(rocname,boot_params.targetName);  
  /*printf("rocName reached >%s<\n",rocname);fflush(stdout);*/

  return((char*)targetname);  
}

void
targetName_test()
{
  char *myname;

  myname = targetName();
  printf("myname >%s<\n",myname);
}

#endif


int
rocId()
{
  return(this_roc_id);
}

int
rocIdprint()
{
  printf("rocId=%d\n",this_roc_id);
  return(this_roc_id);
}


/****************************************************************************/
/************************************ ROC ***********************************/

int
roc_constructor()
{
  int ix, res;
  unsigned int maxAvailBytes = 0;
  unsigned int maxNeededBytes = 0;
  rocParam rocp;
  char tmp[400];

  printf("roc_constructor: localobject=0x%08x\n",localobject);

  rocp = localobject->privated = (void *) malloc(sizeof (roc_priv));
  if(rocp == NULL)
  {
    printf("malloc error - return\n");
    return(CODA_ERROR);
  }
  memset((char *) localobject->privated, 0, sizeof (roc_priv));

  memset((char *) &big0, 0, sizeof(BIGNET));
  memset((char *) &big1, 0, sizeof(BIGNET));


  /*************************/
  /* allocate buffer space */

  /* memory size we need */
  maxNeededBytes =
    SEND_BUF_SIZE*NUM_SEND_BUFS*2 /* multiply by 2 because need pool for proc and net */
    + MIN_MEM_LEFT;
  printf("min=0x%08x (bufs 0x%08x x %d x 2, mem=0x%08x)\n",
    maxNeededBytes,SEND_BUF_SIZE,NUM_SEND_BUFS,MIN_MEM_LEFT);

#ifdef VXWORKS
  /* memory size we have */
  maxAvailBytes = memFindMax();
  printf("maxAvailBytes= 0x%08x\n",maxAvailBytes);

  if(maxAvailBytes > maxNeededBytes)
  {
    tsendBufSize = SEND_BUF_SIZE;
    printf("INFO: wants=0x%08x, maxAvailBytes=0x%08x, create=0x%08x\n",
        maxNeededBytes, maxAvailBytes, tsendBufSize);
  }
  else
  {
    tsendBufSize = (maxAvailBytes
		            - MIN_MEM_LEFT /* leave space for other data */
                     ) / (NUM_SEND_BUFS*2);
    printf("WARN: Reducing Standard Send Buffer Allocation\n");
    printf("WARN: wants=0x%08x, maxAvailBytes=0x%08x, create=0x%08x\n",
        maxNeededBytes, maxAvailBytes, tsendBufSize);
  }
#else
  maxAvailBytes = SEND_BUF_SIZE * NUM_SEND_BUFS;
  tsendBufSize = SEND_BUF_SIZE;
  printf("INFO: wants=0x%08x, maxAvailBytes=0x%08x, create=0x%08x\n",
      maxNeededBytes, maxAvailBytes, tsendBufSize);
#endif

  /* create input 'big' buffer pools for 'proc' and 'net'; input to the 'net' will be
  used as output from 'proc' if both of them are running on host */

#ifdef VXWORKS

  /* Does VxWorks have enough memory for output buffers ? */
  maxAvailBytes = memFindMax();
  if(maxAvailBytes < NUM_SEND_BUFS*2*tsendBufSize)
  {
    printf("ERROR: Do NOT allocate 'big' buffer - no memory !!!\n");
    printf("ERROR: maxAvailBytes=0x%08x, need 0x%08x x %d\n",
      maxAvailBytes,tsendBufSize,NUM_SEND_BUFS);
  }
  else
  {

#ifndef NEW_ROC
    /* input buffer for proc */
    big0.gbigBuffer = bb_new(NUM_SEND_BUFS,tsendBufSize);
    if(big0.gbigBuffer == NULL)
    {
      printf("ERROR in bb_new: Buffer allocation FAILED\n");
      return(CODA_ERROR);
    }
    else
    {
      printf("bb_new: big0 buffer allocated at 0x%08x\n",big0.gbigBuffer);
    }
#endif

    /* intermediate buffer */
    gbigBuffer1 = bb_new(NUM_SEND_BUFS,tsendBufSize);
    if(gbigBuffer1 == NULL)
    {
      printf("ERROR in bb_new: gbigBuffer1 allocation FAILED\n");
      return(CODA_ERROR);
    }
    else
    {
      printf("bb_new: gbigBuffer1 allocated at 0x%08x\n",gbigBuffer1);
    }

  }


#else /* not VxWorks */


  /* input buffer for proc */
#if defined(NEW_ROC) && defined(Linux_vme)

  big0.gbigBuffer = bb_new_rol1(NUM_SEND_BUFS,tsendBufSize);
  if(big0.gbigBuffer == NULL)
  {
    printf("ERROR in bb_new_rol1: Buffer allocation FAILED\n");
    return(CODA_ERROR);
  }
  else
  {
    printf("bb_new_rol1: big buffer allocated at 0x%08x\n",big0.gbigBuffer);
  }

#else

  big0.gbigBuffer = bb_new(NUM_SEND_BUFS,tsendBufSize);
  if(big0.gbigBuffer == NULL)
  {
    printf("ERROR in bb_new: Buffer allocation FAILED\n");
    return(CODA_ERROR);
  }
  else
  {
    printf("bb_new: big0 buffer allocated at 0x%08x\n",big0.gbigBuffer);
  }
#endif

  /* intermediate buffer */
  gbigBuffer1 = bb_new(NUM_SEND_BUFS,tsendBufSize);
  if(gbigBuffer1 == NULL)
  {
    printf("ERROR in bb_new: gbigBuffer1 allocation FAILED\n");
    return(CODA_ERROR);
  }
  else
  {
    printf("bb_new: gbigBuffer1 allocated at 0x%08x\n",gbigBuffer1);
  }

#endif






printf("=== %d %d\n",tcpState,rocp->state);
printf("=== %d %d\n",tcpState,rocp->state);
printf("=== %d %d\n",tcpState,rocp->state);
printf("=== %d %d\n",tcpState,rocp->state);
printf("=== %d %d\n",tcpState,rocp->state);

if(codaUpdateStatus("booted") != CODA_OK) return(CODA_ERROR);

  /* Sergey */
  tcpState = rocp->state = DA_BOOTED;

printf("--- %d %d\n",tcpState,rocp->state);
printf("--- %d %d\n",tcpState,rocp->state);
printf("--- %d %d\n",tcpState,rocp->state);
printf("--- %d %d\n",tcpState,rocp->state);
printf("--- %d %d\n",tcpState,rocp->state);

  for(ix=0; ix<2; ix++)
  {
    rolPs[ix] = (ROLPARAMS *) malloc(sizeof(ROLPARAMS));
    if(rolPs[ix]==NULL)
    {
      printf("ERROR: cannot allocate rolPs[ix] !!!\n");
      return(CODA_ERROR);
    }
  }


#ifdef VXWORKS
  transition_lock = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if(transition_lock == NULL)
  {
    printf("ERROR: could not allocate a semaphore\n");
    return(CODA_ERROR);
  }
#else
  pthread_mutex_init(&transition_lock, NULL);
#endif

#ifdef VXWORKS
  sendbuffer_lock = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if(sendbuffer_lock == NULL)
  {
    printf("ERROR: could not allocate a semaphore\n");
    return(CODA_ERROR);
  }
#else
  pthread_mutex_init(&sendbuffer_lock, NULL);
#endif


  /*
gethrtimetest();
  */

#ifndef VXWORKS
  tcpServer(localobject->name); /*start server to process non-coda commands sent by tcpClient*/
#endif

  return(CODA_OK);
}

int
roc_destructor()
{
  ROLPARAMS  *rolP;
  int         ix, res;
  rocParam    rocp;

printf("================== roc_destructor reached\n");
printf("================== roc_destructor reached\n");
printf("================== roc_destructor reached\n");

  if(localobject == NULL) return(CODA_OK);
  rocp = (rocParam) localobject->privated;

  if(rocp)
  {
    /* delete ROL1, free up memory etc... */
    if(rocp->rolPs[0] != NULL)
    {
      /* Must have called download once before */
      rolP = rocp->rolPs[0];

      /* ROL1: Delete buffer pools */
      if(rolP->inited)
      {
        rolP->daproc = DA_FREE_PROC;
        (*(rolP->rol_code)) (rolP);
      }

      /* unload ROL1 module if possible */
      if((rolP->id != 0) && (rolP->nounload == 0))
      {
        printf("Unloading old ROL object module 0x%08x\n",rolP->id);

		res = codaUnloadROL(rolP);
        if(res) return(CODA_ERROR);
      }

      rocp->rolPs[0] = NULL;

    }
    if(rocp) {free((char *)rocp);rocp=NULL;}

  }

  for(ix=0; ix<2; ix++) free(rolPs[ix]);


#ifdef VXWORKS
  semGive(transition_lock);
  semDelete(transition_lock);
#else
  pthread_mutex_unlock(&transition_lock);
  pthread_mutex_destroy(&transition_lock);
#endif

#ifdef VXWORKS
  semGive(sendbuffer_lock);
  semDelete(sendbuffer_lock);
#else
  pthread_mutex_unlock(&sendbuffer_lock);
  pthread_mutex_destroy(&sendbuffer_lock);
#endif


  return(CODA_OK);
}

int
rocCloseLink()
{
  printf("rocCloselink reached\n");


  /* do it on exit from net_thread */
  if(net_on_pmc)
  {
    ;
  }
  else
  {
    /* shutdown socket connection */
    /*socketnum = LINK_close(socketnum);*/
  }

  /*
  if(socketnum != 0)
  {
    if(shutdown(socketnum, 2)==0)
    {
      printf("roc_closelink: socket #%d connection closed\n",socketnum);
      close(socketnum);
      socketnum = 0;
    }
    else
    {
      printf("roc_closelink: ERROR in socket #%d connection closing\n",
        socketnum);
    }
  }
  */


  return(CODA_OK);
}



int
rocCleanup()
{
  objClass object = localobject;

  rocParam rocp;
#ifdef VXWORKS 
  int status;
  int ix;
  int  num_messages, length;
#endif

  printf("rocCleanup reached\n");
  rocp = (rocParam) object->privated;  
  big0.doclose = 0;
  big1.doclose = 0;
  big0.failure = 0;
  big1.failure = 0;
  rocp->active = 1;

  return(CODA_OK);
}


/* function called during 'Reset' transition; it will bring everything
   into initial state as it was before first 'Download' */

int
codaExit()
{
  objClass object = localobject;
#ifdef VXWORKS 
  int state;
#endif
  rocParam  rocp;
  ROLPARAMS *rolP;
  int       ix, ii;
  MYSQL *dbsock;
  char      tmpp[1000];

  rocp = (rocParam) object->privated;


  /* mark ROC as async */
  /*sergey temporary
  rocp->async_roc_flag = 1;
  */



  /* SERGEY: ??? do rols_loop_exit = 1; instead ??? */
  /*
  codaEnd();
  */
  rols_loop_exit = 1;
  ii = 6;
  while(rols_loop_exit)
  {
    printf("exit: wait for rols_loop to exit ..\n");
#ifdef VXWORKS
    taskDelay(sysClkRateGet());
#else
    sleep(1);
#endif
    ii --;
    if(ii<0) break;
  }


  rocCleanup();



#ifdef VXWORKS
  /* if have PMC, check it's status (net_on_pmc will be set in any case if PMC is in use) */
  if(net_on_pmc)
  {
    state = localpmc.csrr & DC_STATE_MASK;
    printf("PMC state is 0x%08x\n",state);
    localpmc.cmd = DC_RESET;
    while(localpmc.cmd != 0)
    {
      taskDelay(sysClkRateGet());
      printf("waiting for PMC to process transaction, localpmc.cmd=0x%08x\n",localpmc.cmd);
    }

  }
#endif



  if(codaUpdateStatus("configured") != CODA_OK) return(CODA_ERROR);
  /*
  UDP_reset();
  */
  return(CODA_OK);
}


/*
 * This routine is called by the RC system to handle DOWNLOAD.
 * 
 * Put here the code to handle downloading the readout list and calling
 * the user download list.
 */

int
mytest1(char *confname)
{
  MYSQL *dbsock;
  MYSQL_RES *result;
  MYSQL_ROW row;
  char tmp[1000], tmpp[1000];
  int numRows;

  /* connect to database */
  dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));
  printf("3122: dbsock=%d\n",dbsock);
  if(dbsock==NULL)
  {
    printf("cannot connect to the database 2 - exit\n");
    exit(0);
  }


  sprintf(tmpp,"SELECT id FROM runTypes WHERE name='%s'",confname);
  if(mysql_query(dbsock, tmpp) != 0)
  {
    printf("ERROR: cannot select id from runTypes\n");
    return(CODA_ERROR);
  }
  else
  {
    printf("id selected\n");
  }


  /* gets results from previous query */
  /* we assume that tableRes->numRows=1 */
  if( !(result = mysql_store_result(dbsock)) )
  {
    printf("ERROR in mysql_store_result()\n");
    return(CODA_ERROR);
  }
  else
  {
    numRows = mysql_num_rows(result);
    printf("nrow=%d\n",numRows);
    if(numRows == 1)
    {
      printf("=0=> >%s<\n",tmpp);
	  /*
      printf("=1=> 0x%08x\n",tableRes->queryData->data);
      printf("=1=> >%s< %d\n",*(tableRes->queryData->data),
        atoi(*(tableRes->queryData->data)));
      printf("=2=> %d %d %d >%s< >%s<\n",
                         tableRes->fieldData->field.type,
                         tableRes->fieldData->field.length,
                         tableRes->fieldData->field.flags,
                         tableRes->fieldData->field.name,
                         tableRes->fieldData->field.table);
      tableRes->fieldData = tableRes->fieldData->next;
      printf("=3=> %d %d %d >%s< >%s<\n",
                         tableRes->fieldData->field.type,
                         tableRes->fieldData->field.length,
                         tableRes->fieldData->field.flags,
                         tableRes->fieldData->field.name,
                         tableRes->fieldData->field.table);
	  */
    }
    else
    {
      printf("LINK_constructor_C: ERROR: unknown nrow=%d",numRows);
    }

    mysql_free_result(result);
  }

  /* disconnect from database */
  dbDisconnect(dbsock);

  return(0);
}

int listSplit1(char *list, int flag,
           int *argc, char argv[LISTARGV1][LISTARGV2]);

/* for example: ../dac/tcpClient croctest10 download\(\"test_ts2\"\)" */

int
codaDownload(char *confname)
{
  objClass object = localobject;
  int numRows, ii, iii;
  MYSQL *dbsock;
  MYSQL_RES *result;
  MYSQL_ROW row;
  char tmp[1000], tmpp[1000], pmcname[64];
  rocParam rocp;
  ROLPARAMS *rolP;
  int res, ix, state;

  TRANSITION_LOCK;
  if(codaUpdateStatus("downloading") != CODA_OK)
  {
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }

  /*********************************************************/
  /* get pointer to the structure allocated in constructor */
  /*********************************************************/
  rocp = (rocParam) object->privated;

  printf("download: object=0x%08x\n",object);
  printf("download: Downloading configuration >%s<\n",confname);
  fflush(stdout);

  /* set 'configname' */
  strcpy(configname,confname);
  printf("download: Downloading configuration >%s<\n",configname);
  fflush(stdout);


  UDP_start();

  /* update status again: UDP loop was just restarted so it does
	 not sends any messages at that point */
  if(codaUpdateStatus("downloading") != CODA_OK)
  {
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }

  /*********************************************************/
  /* before download, delete old lists, free up memory etc */
  /*********************************************************/
  if(rocp->rolPs[0] != NULL)
  {
    /* Must have called download once before */
    TRANSITION_UNLOCK;
    /* gracefully stop rol's loop */
    rols_loop_exit = 1;
    ii = 6;
    while(rols_loop_exit)
    {
      printf("download: wait for rols_loop to exit ..\n");
#ifdef VXWORKS
      taskDelay(sysClkRateGet());
#else
      sleep(1);
#endif
      ii --;
      if(ii<0) break;
    }

    if(ii<0)
	{
      printf("WARN: cannot exit rols_loop gracefully, will kill it\n");
#ifdef VXWORKS
	  vxworks_task_delete("ROLS_LOOP");
      iTaskROL = 0;
#else
      /* TODO: delete rols_loop thread */
      sleep(1);
#endif
      rols_loop_exit = 0; /* to let new ROLS_LOOP to start */
	}

    TRANSITION_LOCK;

    /* cleanup ROL1 */
    if((rolP = rocp->rolPs[0]) != NULL)
	{
      /* ROL1: Delete buffer pools */
      if(rolP->inited)
      {
        rolP->daproc = DA_FREE_PROC;
        (*(rolP->rol_code)) (rolP);
      }

      /* ROL1: unload module if possible */
      if((rolP->id != 0) && (rolP->nounload == 0))
      {
        printf("Unloading old ROL object module 0x%08x\n",rolP->id);
	    res = codaUnloadROL(rolP);
        if(res) return(CODA_ERROR);
      }

      rocp->rolPs[0] = NULL;
	}
  }

printf("31: >%s< >%s<\n",getenv("MYSQL_HOST"), getenv("EXPID"));fflush(stdout);


  /* connect to database */
  printf("MYSQL_HOST >%s<\n",getenv("MYSQL_HOST"));fflush(stdout);
  dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));
  printf("3123: dbsock=%d\n",dbsock);fflush(stdout);
  if(dbsock==NULL)
  {
    printf("cannot connect to the database 3 - exit\n");
    exit(0);
  }


  /******************************************************/
  /* get the run type number and save it somewhere safe */
  /******************************************************/
  sprintf(tmpp,"SELECT id FROM runTypes WHERE name='%s'",confname);
printf("3123: tmpp>%s<\n",tmpp);fflush(stdout);
  if(dbGetInt(dbsock, tmpp, &(object->runType))==CODA_ERROR)
  {
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }
  printf("====1==== !!! object->runType=%d\n",object->runType);


  /*******************************************************************/
  /* get the token Interval (ROC record size) from the configuration */
  /* options table of the database                                   */
  /*******************************************************************/
  token_interval = 64;
  sprintf(tmpp,"SELECT value FROM %s_option WHERE name='tokenInterval'",
    confname);
  if(dbGetInt(dbsock, tmpp, &token_interval)==CODA_ERROR)
  {
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }


  if(token_interval <= 0) token_interval = 64;
  big0.token_interval = token_interval;
  big1.token_interval = token_interval;
  printf("token_interval=%d\n",token_interval);
  printf("object->name >%s<\n",object->name);


  /*set proc and net locations one more time ..*/
  big0.proc_on_pmc = proc_on_pmc;
  big0.net_on_pmc = net_on_pmc;
  big1.proc_on_pmc = proc_on_pmc;
  big1.net_on_pmc = net_on_pmc;
  printf("big0.proc_on_pmc=%d, big0.net_on_pmc=%d\n",big0.proc_on_pmc,big0.net_on_pmc);
  printf("big1.proc_on_pmc=%d, big1.net_on_pmc=%d\n",big1.proc_on_pmc,big1.net_on_pmc);


  /*************************************************/
  /* check that we should be in this configuration */
  /*************************************************/
  sprintf(tmpp,"SELECT name FROM %s WHERE name='%s'",confname,object->name);
  if(mysql_query(dbsock, tmpp) != 0)
  {
    printf("ERROR: cannot select name from %s\n",confname);
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }
  else
  {
    printf("name selected\n");
  }
  /* gets results from previous query */
  if( !(result = mysql_store_result(dbsock)) )
  {
    printf("download: ERROR in mysql_store_result()\n");
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }
  else
  {
    numRows = mysql_num_rows(result);
    printf("nrow=%d\n",numRows);

    if(numRows == 1)
	{
      row = mysql_fetch_row(result);
      printf("download: name >%s<\n",row[0]);
      strcpy(tmpp, row[0]);
      if((strcmp (tmpp, "{}") == 0)||(strcmp (tmpp, "") == 0))
      {
        printf("ERROR: This component is not used in run type %s\n",
          confname);
        TRANSITION_UNLOCK;
        return(CODA_ERROR);
      }
    }
    else
    {
      printf("download: ERROR: unknown nrow=%d",numRows);
      TRANSITION_UNLOCK;
      return(CODA_ERROR);
    }

    mysql_free_result(result);
  }


  /***************************************************/
  /* get the list of readout-lists from the database */
  /***************************************************/  
  sprintf(tmpp,"SELECT code FROM %s WHERE name='%s'",confname,object->name);
  if(mysql_query(dbsock, tmpp) != 0)
  {
    printf("ERROR: cannot select code from %s\n",confname);
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }
  else
  {
    printf("code selected\n");
  }



  /* gets results from previous query */
  if( !(result = mysql_store_result(dbsock)) )
  {
    printf("download: ERROR in mysql_store_result()\n");
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }
  else
  {
    numRows = mysql_num_rows(result);
    printf("nrow=%d\n",numRows);
    if(numRows == 1)
    {
      row = mysql_fetch_row(result);
      printf("download: code >%s<\n",row[0]);
	  
      strcpy(tmpp, row[0]);
      if((strcmp (tmpp, "{}") == 0)||(strcmp (tmpp, "") == 0))
      {
        printf("ERROR: This component is not used in run type %s\n",
          confname);
        TRANSITION_UNLOCK;
        return(CODA_ERROR);
      }
    }
    else
    {
      printf("download: ERROR: unknown nrow=%d",numRows);
      TRANSITION_UNLOCK;
      return(CODA_ERROR);
    }

    mysql_free_result(result);
  }


dbDisconnect(dbsock);



  /********************************************/
  /* decode configuration string and download */
  /********************************************/
  strcpy(tmp, tmpp);
  if(!((strcmp (tmp, "{}") == 0)||(strcmp (tmp, "") == 0)))
  {
    int ix, jx;
    int listArgc;
    char listArgv[LISTARGV1][LISTARGV2];
    int tmpArgc;
    char tmpArgv[LISTARGV1][LISTARGV2];
    char name[20];
	/**
    ROL_MEM_PART **last_output;
	**/



    if(listSplit1(tmp, 1, &listArgc, listArgv))
    {
      TRANSITION_UNLOCK;
      return(CODA_ERROR);
    }

    /* the number of ROLs cannot exceed 2 */
    if(listArgc>2)
	{
      printf("roc_component ERROR: listArgc=%d, set it to 2\n",listArgc);
      listArgc = 2;
	}

    for(ix=0; ix<listArgc; ix++) printf("nrols [%1d] >%s<\n",ix,listArgv[ix]);

printf("listArgc=%d listArgv >%s< >%s<\n",listArgc,listArgv[0],listArgv[1]);

    /* set ROCid */
    this_roc_id = object->codaid;
    big0.rocid = this_roc_id;
    big1.rocid = this_roc_id;
    printf("set this_roc_id = %d, rocId() can be called from now on\n",this_roc_id);

    /* zero output pointer; will be used to link ROLs */
	/**
    last_output = (ROL_MEM_PART **) NULL;
	**/

    /* loop over readout lists */
    for(ix=0, jx=0; ix<listArgc; ix++, jx++)
    {
      rolP = rocp->rolPs[jx] = rolPs[jx];
      if(rolP == NULL)
      {
        printf("download: malloc error - return\n");
        TRANSITION_UNLOCK;
        return(CODA_ERROR);
      }

      /* cleanup rolP structure */
      memset((char *) rolP, 0, sizeof(ROLPARAMS));

      /* Split list into rol object file and user string */
      if(listSplit1(listArgv[ix], 0, &tmpArgc, tmpArgv))
      {
        TRANSITION_UNLOCK;
        return(CODA_ERROR);
	  }
      if(tmpArgc != 2)
      {
        printf("ERROR: Incorrect number of Arguments passed for ROL = %d\n",
                 tmpArgc);
        TRANSITION_UNLOCK;
        return(CODA_ERROR);
      }

      if(ix==0)
      {
        /* set parent component name in readout list */
        rolP->name = object->name;
        sprintf(name,"rol%d",jx);
        strcpy(rolP->tclName,name);

        /* initialize rol parameters structure, memory partitions */
        rolP->pid = object->codaid;

        /* setup pointers to global ROC information */
        rolP->nevents = (unsigned int *) &(object->nevents);
		/* NOT IN USE !!!???
        rolP->async_roc = &(rocp->async_roc_flag);
		*/

        /* load ROL1 */
        res = codaLoadROL(rolP, tmpArgv[0], tmpArgv[1]);
        if(res)
	    {
          TRANSITION_UNLOCK;
          return(CODA_ERROR);
	    }

        /* execute ROL init procedure (described in 'rol.h') */
        rolP->daproc = DA_INIT_PROC;
        (*(rolP->rol_code)) (rolP);

        /* check if initialization was successful */
        if(rolP->inited != 1)
        {
          tcpState = rocp->state = DA_CONFIGURED;
          printf ("ERROR: ROL initialization failed\n");
          TRANSITION_UNLOCK;
          return(CODA_ERROR);
        }

        /* execute ROL1 download procedure */
        rolP->daproc = DA_DOWNLOAD_PROC;
        (*(rolP->rol_code)) (rolP);
	  }

      if(ix==1)
      {
        /* copy rol2's name into BIGNET structures to be executed from 'proc' */
        strncpy(big0.rolname,tmpArgv[0],255);
        strncpy(big0.rolparams,tmpArgv[1],127);
        strncpy(big1.rolname,tmpArgv[0],255);
        strncpy(big1.rolparams,tmpArgv[1],127);
      }

    }

    printf("downloaded\n");
  }
  else
  {
    printf("WARN: no readout lists in current configuration\n");
  }


/* connect to database */
printf("MYSQL_HOST >%s<\n",getenv("MYSQL_HOST"));fflush(stdout);
dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));
printf("3123-1: dbsock=%d\n",dbsock);fflush(stdout);
if(dbsock==NULL)
{
  printf("cannot connect to the database 3 - exit\n");
  exit(0);
}


  /* Sergey: define 'rocp->async_roc_flag'  using DB where that information
  exists; that setting was removed from ROL1's Download() procedure */
  sprintf(tmpp,"SELECT outputs FROM %s WHERE name='%s'",
    confname,object->name);

  if(dbGetStr(dbsock, tmpp, tmp)==CODA_ERROR)
  {
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }

  if(strlen(tmp)==0)
  {
    rocp->output_switch = 3;
    rocp->async_roc_flag = 1;
  }
  else if(strncmp(tmp,"none",4)==0)
  {
    rocp->output_switch = 3;
    rocp->async_roc_flag = 1;
  }
  else
  {
    rocp->output_switch = 0;
    rocp->async_roc_flag = 0;
  }

  printf("++++++++++++++++++++ outputs >%s< -> async_flag=%d\n",
    tmp,rocp->async_roc_flag);


  /*sergey temporary*/
rocp->output_switch = 0;
rocp->async_roc_flag = 0;



  /**************************************************************/
  /* update 'inuse' column in 'configname' table in database:   */
  /* for the sync ROC, set 'inuse'=rocid, otherwise set it 'no' */
  /**************************************************************/
  if(rocp->async_roc_flag == 0)
  {
    sprintf(tmpp,"UPDATE %s SET inuse='%d' WHERE name='%s'",
      configname,object->codaid,object->name);

    printf("DB command >%s<\n",tmpp);
  }
  else
  {
    sprintf(tmpp,"UPDATE %s SET inuse='no' WHERE name='%s'",
      configname,object->name);
    printf("DB command >%s<\n",tmpp);
  }
  if(mysql_query(dbsock, tmpp) != 0)
  {
    printf("ERROR: cannot UPDATE 'configname' SET inuse='yes'\n");
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }


  /*****/
  /*   */
  /*****/
  tcpState = rocp->state = DA_DOWNLOADED;
  rocp->active = 0;
/**
  partReInitAll();
**/
  /*rocp->last_event = 0;*/ /* will do it in Prestart again ??? */
  last_event_check = 0;


  /******************** VERY IMPORTANT:
  here we will set how 'coda_proc' and 'coda_net' will communicate to surrounding
  and/or to each other; 'dabufp' is set here !!!!!!!!!!!!*/

  if(net_on_pmc==0 && proc_on_pmc==0) /* both 'coda_proc' and 'coda_net' are on host */
  {
    /* 'big0' will be used as 'proc_thread' structure, 'dabufp' will be pointing
    to it's buffer, gbigBuffer1 will be used to pass data from coda_proc to coda_net */
    if(big0.gbigBuffer != NULL)
    {
      bb_init(&big0.gbigBuffer);
      dabufp = bb_write_current(&big0.gbigBuffer);
      if(dabufp == NULL)
      {
        printf("ERROR in bb_write_current: FAILED\n");
        TRANSITION_UNLOCK;
        return(CODA_ERROR);
      }
    }

    if(gbigBuffer1 != NULL)
    {
      bb_init(&gbigBuffer1);

      /* output from coda_proc goes to gbigBuffer1 */
      big0.gbigBuffer1 = gbigBuffer1;
	}

	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

  }
  else if(proc_on_pmc==0 && net_on_pmc==1) /* 'coda_proc' on host, 'coda_net' on PMC */
  {
    /* 'big0' data buffer initialization */
    if(big0.gbigBuffer != NULL)
    {
      bb_init(&big0.gbigBuffer);
      dabufp = bb_write_current(&big0.gbigBuffer);
      if(dabufp == NULL)
      {
        printf("ERROR in bb_write_current: FAILED\n");
        TRANSITION_UNLOCK;
        return(CODA_ERROR);
      }
    }

    /* output from 'proc' must go over PCI bus, so ... */
    if(gbigBuffer1 != NULL)
    {
      bb_init(&gbigBuffer1);

      /* output from coda_proc goes to gbigBuffer1 */
      big0.gbigBuffer1 = gbigBuffer1;
	}


/* ERRORRRRRRRRRR !!!!!!!!!????????????? */
big1.gbigBuffer = gbigBuffer1;
/* ERRORRRRRRRRRR !!!!!!!!!????????????? */


	printf("!!!???!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!???!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!???!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

  }
  else if(net_on_pmc==1 && proc_on_pmc==1) /* both 'coda_proc' and 'coda_net' are on PMC */
  {
    /* 'big0' will be used to pass data to the PMC, 'dabufp' will be pointing
    to it's input buffer */
    if(big0.gbigBuffer != NULL)
    {
      bb_init(&big0.gbigBuffer);
      dabufp = bb_write_current(&big0.gbigBuffer);
      if(dabufp == NULL)
      {
        printf("ERROR in bb_write_current: FAILED\n");
        TRANSITION_UNLOCK;
        return(CODA_ERROR);
      }
    }

  }
  else
  {
    printf("ERROR: illegal combination of proc_on_pmc=%d and net_on_pmc=%d\n",
      proc_on_pmc,net_on_pmc);
  }

  dabufp += BBHEAD;
  printf("1: dabufp set to 0x%x\n",dabufp);

  rocp->active = 1;

printf("999: %d %d %d %d\n",rocp->state,DA_ENDING,clear_to_send,rocp_primefd);

/*sergey*/
big0.failure = 0;
big1.failure = 0;

  /* Spawn the rols_loop Thread */
#ifdef VXWORKS

  {
    /*taskSpawn(name,pri,opt,stk,adr,args) 10 args required */
    /* priority must be lowest if there is no sleep in rols_loop routine ! */
    /* (the lowest priority can be 255 - VXWORKS limit !!!) */
    iTaskROL = taskSpawn("ROLS_LOOP", /*121*/255, 0, 200000, rols_loop,
                          0, 0, 0,0,0,0,0,0,0,0);
    printf("taskSpawn(\"ROLS_LOOP\") returns %d\n",iTaskROL);
  }

#else
  {
    pthread_attr_init(&detached_attr);
    pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setscope(&detached_attr,PTHREAD_SCOPE_SYSTEM/*PTHREAD_SCOPE_PROCESS*/);
    pthread_create( (unsigned int *) &iTaskROL, &detached_attr,
		   (void *(*)(void *)) rols_loop, (void *) NULL);
  }
#endif


/* use codaUpdateStatus here ??????? */
  /* update 'state' column in 'process' table in database */
  sprintf(tmpp,"UPDATE process SET state='downloaded' WHERE name='%s'",
    object->name);
printf("DB command >%s<\n",tmpp);
  if(mysql_query(dbsock, tmpp) != 0)
  {
    printf("ERROR: cannot UPDATE process SET state='downloaded'\n");
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }
  else
  {
    printf("UPDATE process success\n");
  }

  /* disconnect from database */
  dbDisconnect(dbsock);





#ifdef VXWORKS

  printf("sysClkRate set to %d ticks\n",sysClkRateGet());
  printf("proc_on_pmc=%d\n",proc_on_pmc);
  printf("net_on_pmc=%d\n",net_on_pmc);

  if(proc_on_pmc == 0)
  {
    proc_download(big0.rolname,big0.rolparams,rocId());
  }

#ifdef PMCOFFSET
  if(net_on_pmc)
  {
    printf("!!!!!!!!!!!!!! proc_on_pmc=%d, net_on_pmc=%d\n",proc_on_pmc,net_on_pmc);
    printf("!!!!!!!!!!!!!! proc_on_pmc=%d, net_on_pmc=%d\n",proc_on_pmc,net_on_pmc);
    printf("!!!!!!!!!!!!!! proc_on_pmc=%d, net_on_pmc=%d\n",proc_on_pmc,net_on_pmc);
    printf("!!!!!!!!!!!!!! proc_on_pmc=%d, net_on_pmc=%d\n",proc_on_pmc,net_on_pmc);
    printf("!!!!!!!!!!!!!! proc_on_pmc=%d, net_on_pmc=%d\n",proc_on_pmc,net_on_pmc);


    /* let pmc card know the address of 'pmc' structure (init it first !) */
    localpmc.dataReady = 0;
    if(proc_on_pmc)
	{
      localpmc.bignetptr = &big0; /* pass 'big0' to PMC */
    }
	else
	{
      localpmc.bignetptr = &big1; /* pass 'big1' to pmc */
	}


#ifdef USE_PCI_BRIDGE_REGISTER
    usrWriteGPR(&localpmc);
#else  /* use database */

    /*********************************************************/
    /* update 'PMCs' table in the database with 'StructAddr' */
    sprintf(pmcname,"%spmc1",targetName());
    printf("pmcname >%s<\n",pmcname);
    dbsock = dbConnect(getenv("MYSQL_HOST"), "daq");
    if(dbsock==NULL)
    {
      printf("cannot connect to the database 4 - exit\n");
      exit(0);
    }
    /* trying to select our name from 'PMCs' table */
    sprintf(tmp,"SELECT Name FROM PMCs WHERE name='%s'",pmcname);
    if(mysql_query(dbsock, tmp) != 0)
    {
      printf("mysql error (%s)\n",mysql_error(dbsock));
      return(ERROR);
    }
    /* gets results from previous query */
    /* we assume that numRows=0 if our Name does not exist,
       or numRows=1 if it does exist */
    if( !(result = mysql_store_result(dbsock)) )
    {
      printf("ERROR in mysql_store_result (%)\n",mysql_error(dbsock));
      return(ERROR);
    }
    else
    {
      numRows = mysql_num_rows(result);
      mysql_free_result(result);
      /* NOTE: for VXWORKS 'Host' the same as 'Name' */
      /*printf("nrow=%d\n",numRows);*/
      if(numRows == 0)
      {
        printf("ERROR: PMC does not exist, turn it off and start from Download again !!!\n");
        return(ERROR);
      }
      else if(numRows == 1)
      {
        sprintf(tmp,"UPDATE PMCs SET StructAddr=0x%08x WHERE name='%s'",&localpmc,pmcname);
        printf("execute >%s<\n",tmp);
      }
      else
      {
        printf("ERROR: PMCs: unknown nrow=%d",numRows);
        return(ERROR);
      }
      if(mysql_query(dbsock, tmp) != 0)
      {
 	   printf("ERROR\n");
        return(ERROR);
      }
      else
      {
        printf("Query >%s< succeeded\n",tmp);
      }
    }
    dbDisconnect(dbsock);

#endif


    localpmc.cmd = DC_DOWNLOAD;
    printf("coda_roc: localpmc.cmd=0x%08x (addr=0x%08x)\n",localpmc.cmd,&localpmc);
    printf("coda_roc: sent structure address is 0x%08x\n",&localpmc);
    printf("coda_roc: big0 at 0x%08x, big0.gbigBuffer at 0x%08x\n",
      big0, &big0.gbigBuffer);


    while(localpmc.cmd != 0)
    {
      taskDelay(sysClkRateGet());
      printf("waiting for PMC to process transaction, localpmc.cmd=0x%08x\n",localpmc.cmd);
    }

  }

#endif

#else

  proc_download(big0.rolname,big0.rolparams,rocId());

#endif

printf("POLLS: %d %d\n",rocp->rolPs[0]->poll,rocp->rolPs[1]->poll);

  if(codaUpdateStatus("downloaded") != CODA_OK)
  {
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }

  TRANSITION_UNLOCK;

  return(CODA_OK);
}


/******************************************************************************
 *
 * This routine informs the EB of state changes in a particular ROC
 */

void
informEB(objClass object, ulong mTy, ulong mA, ulong mB)
{
  unsigned long *data,len;
#ifdef VXWORKS
  int status;
  int old = intLock();
#endif
  rocParam rocp = (rocParam) object->privated;
  int res;
  unsigned long id;
  unsigned int *bigbuf;

  printf("informEB reached\n");


  /*ERRORRRRRRRRRRRRRRRRRRRRRRR*/
  bigbuf = bb_write_current(&big0.gbigBuffer);
  if(bigbuf == NULL)
  {
    printf("informEB: alloc error - return\n");
    return;
  }


  bigbuf[BBIWORDS] = (BBHEAD_BYTES+20)>>2;
  bigbuf[BBIBUFNUM] = -1;
  bigbuf[BBIROCID]  = object->codaid;
  bigbuf[BBIEVENTS] = 1;  /* # events in buffer = 1 */
  bigbuf[BBIFD]     = rocp_primefd;
  if(mTy == (ulong) EV_END)
  {
    bigbuf[BBIEND]  = 1;
  }
  else
  {
    bigbuf[BBIEND]  = 0;
  }
  bigbuf[BBHEAD]    = 4;  /* event starts here; contains # words in event */
  bigbuf[BBHEAD+1]  = CTL_BANK_HDR(mTy);  /* control bank header */
  bigbuf[BBHEAD+2]  = 1200; /* some junk */
  bigbuf[BBHEAD+3]  = mA;   /* parameter a */
  bigbuf[BBHEAD+4]  = mB;   /* parameter b */
  
printf("informEB: %d %d %d %d %d %d - 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
bigbuf[0],bigbuf[1],bigbuf[2],bigbuf[3],bigbuf[4],bigbuf[5],
bigbuf[6],bigbuf[7],bigbuf[8],bigbuf[9],bigbuf[10],bigbuf[11]);
  
  bigbuf = bb_write(&big0.gbigBuffer);

#ifdef VXWORKS
  intUnlock(old);
#endif

  return;
}


/******************************************************************************
 * This routine is called by RC as the prestart transition
 */

int
codaPrestart()
{
  objClass object = localobject;

  rocParam    rocp;
  ROLPARAMS  *rolP;
  int         ix, state, iii, port,ret;
  MYSQL *dbsock;
  char        tmp[100];
  char        tmpp[1000];
  char host[128];

  TRANSITION_LOCK;

  printf("roc_prestart reached\n");

  rocp = (rocParam) object->privated;
  big0.doclose = 0;
  big1.doclose = 0;


/*sergey*/
bb_init(&big0.gbigBuffer);
big0.failure = 0;
big1.failure = 0;
last_event_check = 0;

/**
  partReInitAll();
**/

  tcpState = rocp->state = DA_PRESTARTING;

  rocCleanup();



  /***********************/
  /* connect to database */
  /***********************/

  dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));
  printf("315: dbsock=%d\n",dbsock);
  if(dbsock==NULL)
  {
    printf("cannot connect to the database 5 - exit\n");
    exit(0);
  }


  /**********************/
  /* Get the run number */
  /**********************/
  sprintf(tmpp,"SELECT runNumber FROM sessions WHERE name='%s'",session);
  if(dbGetInt(dbsock, tmpp, &(object->runNumber))==CODA_ERROR)
  {
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }
  printf(">>> prestarting, run %d, type %d\n",object->runNumber,object->runType);


  /* set token Interval (it was obtained from DB at 'Download') */
  big0.token_interval = token_interval;
  big1.token_interval = token_interval;


  /* get rocMask from database's options table; it must be set by EB;
  will be used in TS ROC only, other ROCs do not need that information */
  sprintf(tmpp,"SELECT value FROM %s_option WHERE name='rocMask'",configname);
  if(dbGetInt(dbsock, tmpp, &rocMask)==CODA_ERROR)
  {
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }
  printf("rocMask=0x%08x\n",rocMask);


  /* open TCP link to the Event Builder */
  /* if PMC is used, 'localpmc' structure will be updated with 'host' and 'port' - not anymore ??? */
  sprintf(tmpp,"SELECT outputs FROM %s WHERE name='%s'",configname,object->name);

  printf("!!!!!!!!!!!!! tmpp >%s<\n",tmpp);
  printf("!!!!!!!!!!!!! tmpp >%s<\n",tmpp);
  printf("!!!!!!!!!!!!! tmpp >%s<\n",tmpp);

  if(dbGetStr(dbsock, tmpp, tmp)==CODA_ERROR)
  {
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }

  /* disconnect from database */
  dbDisconnect(dbsock);



  if(rocp->async_roc_flag == 0)
  {
    printf("++++> output to network\n");

    for(ix=0; ix<strlen(tmp); ix++)
    {
      if(tmp[ix]==':')
      {
        tmp[ix] = '\0';
        break;
      }
    }

    rocCloseLink(); /* empty anyway */

    printf("!!!!!!!!!!!!! for rocOpenLink: >%s< >%s< %d\n",object->name,tmp,
      strlen(tmp));
    printf("!!!!!!!!!!!!! for rocOpenLink: >%s< >%s<\n",object->name,tmp);
    printf("!!!!!!!!!!!!! for rocOpenLink: >%s< >%s<\n",object->name,tmp);
    printf("!!!!!!!!!!!!! for rocOpenLink: >%s< >%s<\n",object->name,tmp);

    /* if net or both proc/net on pmc - just pass CODA names for roc and event builder,
     TCP to EB will be opened from PMC */
    if(net_on_pmc==1 || proc_on_pmc==1)
    {
      strcpy((char *)big0.roc_name, object->name);
      strcpy((char *)big1.roc_name, object->name);
      strcpy((char *)big0.eb_name, tmp);
      strcpy((char *)big1.eb_name, tmp);
    }
    else /* open TCP to EB here */
	{
      ret = rocOpenLink(object->name, tmp, host, &port, &socketnum);
      if(ret<0)
	  {
        /*MAKE IT FUNCTION: UDP_USER_REQUEST, similar to UDP_standard_request !!!*/
        /* then call it from coda_roc */
        char tmpp[1000];
        strcpy(tmpp,"err:");
        strcat(tmpp,object->name);
        strcat(tmpp," ");
        strcat(tmpp,"CANNOT ESTABLISH TCP TO EB");
        UDP_request(tmpp);
#ifdef VXWORKS
    taskDelay(2*sysClkRateGet());
#else
    sleep(2);
#endif
       UDP_cancel(tmpp);

        printf("roc_component ERROR: CANNOT ESTABLISH TCP TO EB !!!\n");
        return(CODA_ERROR);
	  }
      if(socketnum>0)
	  {
        /* why needs that ???*/
        big0.socket = socketnum;
        big1.socket = socketnum;
        big0.port = port;
        strcpy((char *)big0.host, host);
        big1.port = port;
        strcpy((char *)big1.host, host);
	  }
	}
  }
  else
  {
    printf("coda_roc: ++++> output to nowhere\n");
  }



  printf("Spawn proc/net threads\n");
  printf("Spawn proc/net threads\n");
  printf("Spawn proc/net threads\n");


  /* spawn 'proc' or/and 'net' threads */
  /* IMPORTANT: 'net' must be started first !!! (realy ???) */

#ifdef VXWORKS

  if(proc_on_pmc == 0)
  {
    proc_prestart(rocId());
  }

  if(net_on_pmc) /* if have PMC, send 'prestart' command there and wait for completion */
  {
    big0.socket = 0;
    big1.socket = 0;
    state = localpmc.csrr & DC_STATE_MASK;
    printf("PMC state is 0x%08x\n",state);
    if(state != DC_ACTIVE)
    {
      localpmc.cmd = DC_PRESTART;
      while(localpmc.cmd != 0)
      {
        taskDelay(sysClkRateGet());
        printf("waiting for PMC to process transaction, localpmc.cmd=0x%08x\n",localpmc.cmd);
      }
    }
    else
    {
      printf("Cannot Prestart PMC - current state is ACTIVE\n");
    }
  }

  /* delete and restart 'net' if it is on host */
  if(net_on_pmc == 0)
  {
	vxworks_task_delete("coda_net");
    iTaskNET = 0;

    /* input to coda_net goes from gbigBuffer1, and no output from coda_net */
    big1.gbigBuffer = gbigBuffer1;
    big1.gbigBuffer1 = NULL;

    bb_init(&big1.gbigBuffer); /*init input buffer*/
    big1.failure = 0;

    iTaskNET = taskSpawn("coda_net", 120, 0, 500000, net_thread,
                          &big1, 0, 0,0,0,0,0,0,0,0);
    printf("taskSpawn(\"coda_net\",0x%08x) returns %d\n",iTaskNET,&big1);
  }


  /* delete and restart 'proc' if it is on host */
  if(proc_on_pmc == 0)
  {
	vxworks_task_delete("coda_proc");
    iTaskPROC = 0;

    bb_init(&big0.gbigBuffer); /*init input buffer*/
    bb_init(&big0.gbigBuffer1); /*init output buffer*/
    big0.failure = 0;

    /*taskSpawn(name,pri,opt,stk,adr,args) 10 args required */
    iTaskPROC = taskSpawn("coda_proc", 115, 0, 500000, proc_thread,
                          &big0, 0, 0,0,0,0,0,0,0,0);
    printf("taskSpawn(\"coda_proc\",0x%08x) returns %d\n",iTaskPROC,&big0);
  }

#else

printf("unix1\n");
  proc_prestart(rocId());
printf("unix2\n");

  /* TODO: delete 'net_thread' if running */
  sleep(1);

  /* input to coda_net goes from gbigBuffer1, and no output from coda_net */
  big1.gbigBuffer = gbigBuffer1;
  big1.gbigBuffer1 = NULL;
  bb_init(&big1.gbigBuffer);
  big1.failure = 0;

printf("unix3\n");
  pthread_attr_init(&detached_attr);
  pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);
  pthread_attr_setscope(&detached_attr,PTHREAD_SCOPE_SYSTEM/*PTHREAD_SCOPE_PROCESS*/);
  iii=pthread_create( (unsigned int *) &iTaskNET, &detached_attr,
		   (void *(*)(void *)) net_thread, (void *) &big1);

printf("unix4: net thread returned %d\n",iii);

  /* TODO: delete 'proc_thread' if running */
  sleep(1);

  bb_init(&big0.gbigBuffer);
  big0.failure = 0;
printf("unix5\n");

  pthread_attr_init(&detached_attr);
  pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);
  pthread_attr_setscope(&detached_attr,PTHREAD_SCOPE_SYSTEM/*PTHREAD_SCOPE_PROCESS*/);
  iii=pthread_create( (unsigned int *) &iTaskPROC, &detached_attr,
		   (void *(*)(void *)) proc_thread, (void *) &big0);

printf("unix6: proc thread returned %d\n",iii);

#endif

  object->nevents = 0; /* zero bookeeping counters */
  object->nlongs = object_nlongs = 0;
  rocp_recNb = 0;
  /*rocp->last_event = 0;*/
  last_event_check = 0;

  /* ROL1: set bookeeping parameters then call prestart routine */
  if((rolP = rocp->rolPs[0]) != NULL)
  {
    rolP->runNumber = object->runNumber;
    rolP->runType = object->runType;
    rolP->recNb = 0;

    rolP->daproc = DA_PRESTART_PROC;
    (*rolP->rol_code) (rolP);
  }


  /* save socket number */
  if(rocp->output_switch == 0)
  {
    printf("111\n");
#ifdef VXWORKS
    if(net_on_pmc)
    {
      /* get socket number back from pmc; if zero - wait */
	  /* should we use 'localpmc.bignetptr->socket' instead of big0/big1 ?? */
	  if(proc_on_pmc)
	  {
        while(big0.socket == 0)
        {
          printf("waiting for socket number from pmc ..\n");
          taskDelay(1);
	    }
        socketnum = big0.socket;
	  }
	  else
	  {
        while(big1.socket == 0)
        {
          printf("waiting for socket number from pmc ..\n");
          taskDelay(1);
	    }
        socketnum = big1.socket;
	  }
      rocp_primefd = socketnum;

    }
    else
#endif
    {
      rocp_primefd = socketnum;
    }
    printf("333: rocp_primefd=%d\n",rocp_primefd);
  }
  else
  {
    rocp_primefd = 0;
  }

  /* If the async_roc flag is set we don't send control events */
  tcpState = rocp->state = DA_PRESTARTED;
  if(rocp->async_roc_flag == 0)
  {
    informEB(object, (ulong) EV_PRESTART, (ulong) object->runNumber,
             (ulong) object->runType);
  }

  if(codaUpdateStatus("paused") != CODA_OK)
  {
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }
  printf("prestarted\n");fflush(stdout);

  TRANSITION_UNLOCK;

  return(CODA_OK);
}





/******************************************************************************
 * This routine is called by RC
 * to implement end transition
 */

int
codaEnd()
{
  objClass object = localobject;

  rocParam    rocp;
  ROLPARAMS  *rolP;
  int         ix, state;
  MYSQL *dbsock;
  char      tmpp[1000];

  printf("codaEnd reached ??\n");fflush(stdout);

  TRANSITION_LOCK;

  rocp = (rocParam) object->privated;

  printf("codaEnd reached !!\n");fflush(stdout);

  /* end ROL1 */
  if((rolP=rocp->rolPs[0]) != NULL)
  {
    if(rolP->inited)
    {
      printf("codaEnd: calling ROL1's end\n");fflush(stdout);
      rolP->daproc = DA_END_PROC;
      (*rolP->rol_code) (rolP);
    }
  }

  if((rocp->async_roc_flag == 1))
  {
    printf("codaEnd: go to Downloaded for async roc\n");fflush(stdout);
    if(codaUpdateStatus("downloaded") != CODA_OK)
    {
      TRANSITION_UNLOCK;
      return(CODA_ERROR);
	}
    tcpState = rocp->state = DA_DOWNLOADED;
    rocp->active = 1;
  }
  else
  {

    /******************************/
    /* delay inside following ??? */
    /******************************/
    if(codaUpdateStatus("ending") != CODA_OK)
    {
printf("ERROR_ENDING ...\n");
printf("ERROR_ENDING ...\n");
printf("ERROR_ENDING ...\n");
printf("ERROR_ENDING ...\n");
printf("ERROR_ENDING ...\n");fflush(stdout);

      TRANSITION_UNLOCK;
      return(CODA_ERROR);
	}

    printf("codaEnd: NOW !!!!!!!!!!!!!!!!!!!\n");
    printf("codaEnd: NOW !!!!!!!!!!!!!!!!!!!\n");
    printf("codaEnd: NOW !!!!!!!!!!!!!!!!!!!\n");
    printf("codaEnd: NOW !!!!!!!!!!!!!!!!!!!\n");
    printf("codaEnd: NOW !!!!!!!!!!!!!!!!!!!\n");fflush(stdout);
    tcpState = rocp->state = DA_ENDING;

    rocp->active = 2; /* ??? it must go AFTER setting 'ending', otherwise it may
                      set 'downloaded' really fast and then we will rewrite
                      it by 'ending' so 'ending' will be last status to be
                      reported to EB (must be 'downloaded') */
  }

#ifdef VXWORKS

  if(proc_on_pmc == 0)
  {
    /*DELETE_PROC*/;
  }

  if(net_on_pmc)
  {
    state = localpmc.csrr & DC_STATE_MASK;
    printf("PMC state is 0x%08x\n",state);
    if(state == DC_ACTIVE)
    {
      localpmc.cmd = DC_END;
      while(localpmc.cmd != 0)
      {
        taskDelay(sysClkRateGet());
        printf("waiting for PMC to process transaction, localpmc.cmd=0x%08x\n",localpmc.cmd);
      }
    }
    else
    {
      printf("Cannot End PMC - current state is not ACTIVE\n");
    }
  }

#else

  /*DELETE_PROC*/;

#endif

  printf("codaEnd: unlocking\n");fflush(stdout);
  TRANSITION_UNLOCK;
  printf("codaEnd: done\n");fflush(stdout);

  return(CODA_OK);
}







/******************************************************************************
 * This routine is called by RC
 * to implement the pause transition
 */

int
codaPause()
{
  objClass object = localobject;

  rocParam        rocp;
  ROLPARAMS      *rolP;
  int             ix;

  TRANSITION_LOCK;

  rocp = (rocParam) object->privated;

  /* pause ROL1 */
  if((rolP = rocp->rolPs[0]) != NULL)
  {
    rolP->daproc = DA_PAUSE_PROC;
    (*rolP->rol_code) (rolP);
  }

  rocp->active = 2;

  if((rocp->async_roc_flag == 1))
  {
    tcpState = rocp->state = DA_PAUSED;
    if(codaUpdateStatus("paused") != CODA_OK)
    {
      TRANSITION_UNLOCK;
      return(CODA_ERROR);
	}
  }
  else
  {
    tcpState = rocp->state = DA_PAUSING;
    if(codaUpdateStatus("pausing") != CODA_OK)
    {
      TRANSITION_UNLOCK;
      return(CODA_ERROR);
	}
  }

  TRANSITION_UNLOCK;

  return(CODA_OK);
}

/******************************************************************************
 * This routine is called by RC
 * to implement the go transition
 */

int
codaGo()
{
  objClass object = localobject;

  rocParam    rocp;
  ROLPARAMS  *rolP;
  int         ix, state, ticks;

  TRANSITION_LOCK;

  rocp = (rocParam) object->privated;

  printf("activating ..\n");

#ifdef VXWORKS
  ticks = vxTicks;
#endif
  if(rocp->async_roc_flag == 0)
  {
    informEB(object, (ulong) EV_GO, (ulong) 0, (ulong) object->nevents);
  }



  g_events_in_buffer = 0;
  /* set pointer to the first buffer (must be set BEFORE ROLs are executed) */

  /*ERRORRRRRRRRR*/
  dabufp = bb_write_current(&big0.gbigBuffer);
  if(dabufp == NULL)
  {
    printf("ERROR in bb_write_current: FAILED\n");
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }
  dabufp += BBHEAD;
  printf("1-1: dabufp set to 0x%x\n",dabufp);


  /* Go ROL1 - old location */


#ifdef VXWORKS

  if(proc_on_pmc == 0)
  {
    printf("calls 'proc_go', rocid=%d\n",rocId());
    proc_go(rocId());
  }

  if(net_on_pmc)
  {
    state = localpmc.csrr & DC_STATE_MASK;
    printf("PMC state is 0x%08x\n",state);
    if(state != DC_ACTIVE)
    {
      localpmc.cmd = DC_GO;
      while(localpmc.cmd != 0)
      {
        taskDelay(sysClkRateGet());
        printf("waiting for PMC to process transaction, localpmc.cmd=0x%08x\n",localpmc.cmd);
      }
    }
    else
    {
      printf("Cannot Go PMC - current state is not ACTIVE\n");
    }
  }

#else
    printf("calls 'proc_go', rocid=%d\n",rocId());
    proc_go(rocId());
#endif

  TRANSITION_UNLOCK;


  /***********/
  /* Go ROL1 */

  /* in polling mode, go() will be executed before starting rols_loop,
	 in interrupt mode - in opposite order */

  if((rolP = rocp->rolPs[0]) != NULL)
  {
    if(rolP->poll)
    {
      rolP->daproc = DA_GO_PROC;
      (*rolP->rol_code) (rolP);
    }    
  }


  rocp->active = 2; /* will start ROLS_LOOP */
  tcpState = rocp->state = DA_ACTIVE;
  if(codaUpdateStatus("active") != CODA_OK)
  {
    TRANSITION_UNLOCK;
    return(CODA_ERROR);
  }
  printf("active, events so far %d\n",object->nevents);fflush(stdout);


  if((rolP = rocp->rolPs[0]) != NULL)
  {
    if(!rolP->poll)
    {
      /*without following delay we are hunging on the first event in standalone mode with interrupts;
      probably have to give a time to rols_loop to start after setting 'rocp->active=2'
      and before first interrupt; need to synchronize them better ... */
#ifdef VXWORKS
      taskDelay(sysClkRateGet());
#else
      sleep(1);
#endif
      rolP->daproc = DA_GO_PROC;
      (*rolP->rol_code) (rolP);
    }
  }

  return(CODA_OK);
}



/*************/
/* MAIN LOOP */
/*************/

#ifdef VXWORKS
static int intLockKey;
#define LOCKINTS intLockKey = intLock();
#define UNLOCKINTS intUnlock(intLockKey);
#else
#define LOCKINTS
#define UNLOCKINTS
#endif

#ifdef VXWORKS

#define SET_TIMEOUT(cba) \
    if(object->nevents < 10) \
    { \
      timeout = vxTicks + sysClkRateGet()/* /6 */; /*1 sec */ \
      /*printf("timeout 1: %d + %d = %d [%d]\n",vxTicks,sysClkRateGet(),timeout,cba);*/ \
    } \
    else \
    { \
      timeout = vxTicks + token_interval; \
      /*printf("timeout 2: %d + %d = %d [%d]\n",vxTicks,token_interval,timeout,cba);*/ \
    }

#else

#define SET_TIMEOUT(cba) \
    if(object->nevents < 10) \
    { \
      timeout = /*time(0)*/gethrtime() + 1*100000; \
    } \
    else \
    { \
      timeout = /*time(0)*/gethrtime() + (token_interval/60)*100000;	\
    }

#endif

/* NOTES:

 1. loop over readout lists will be performed if(rocp->active > 1); 
   'Go' transition sets rocp->active=2

 2. 

*/

void
rols_loop()
{
  objClass object = localobject;
#ifdef Linux
  prctl(PR_SET_NAME,"coda_rols");
#endif
  printf("rols_loop started\n");

  /* some initialization */
  icycle3=0;
  timeout = 0;
  dataInBuf = BBHEAD_BYTES;
  g_events_in_buffer = 0;
  clear_to_send = 0;
  SET_TIMEOUT(88);

  while(1)
  {
    setHeartBeat(HB_ROL,0,5);
    if(rols_loop_exit)
    {
      setHeartBeat(HB_ROL,0,-1);
      rols_loop_exit = 0;
      return;
    }
    setHeartBeat(HB_ROL,1,5);

    output_proc_network(0);

  }

  setHeartBeat(HB_ROL,0,-1);

  return;
}

#define SEND_BUFFER_(abc) \
  /*SENDING big buffer*/ \
  if(clear_to_send && (rocp_primefd>=0)) \
  { \
    /*setHeartBeat(HB_ROL,14,5);*/ \
    /* send current output buffer */ \
    /* set now 'dabufp' to the beginning of */ \
    /* 'big' buffer just to fill a header   */ \
    dabufp = bb_write_current(&big0.gbigBuffer); \
    /*printf("SEND_BUFFER_ROC %d (0x%08x %d) (conditions %d %d)\n",abc,dabufp,dabufp[0],clear_to_send,rocp_primefd);*/ \
    /*setHeartBeat(HB_ROL,15,5);*/ \
    if(dabufp == NULL) \
    { \
      printf("ERROR in bb_write: FAILED1\n"); \
      /*setHeartBeat(HB_ROL,0,-1);*/ \
      SENDBUFFER_UNLOCK; \
      return; \
    } \
    /* fill 'big' buffer header */ \
    /*printf("FILLING HEADER: dataInBuf>>2=%d, g_events_in_buffer=%d\n",dataInBuf>>2,g_events_in_buffer);*/ \
    dabufp[BBIWORDS] = dataInBuf>>2;        /* buffer length in words */ \
    /*printf("SSS1 %d (0x%08x %d)\n",abc,dabufp,dabufp[0]);*/ \
    dabufp[BBIBUFNUM] = ++ rocp_recNb;      /* buffer number */ \
    dabufp[BBIROCID]  = this_roc_id/*object->codaid*/;     /* ROC id */ \
    dabufp[BBIEVENTS] = g_events_in_buffer; /* the number of events */ \
    dabufp[BBIFD]     = rocp_primefd; \
    dabufp[BBIEND]    = 0; \
	/* main send */ \
    /*printf("SSS2 %d (0x%08x %d)\n",abc,dabufp,dabufp[0]);*/ \
    if(dabufp[BBIWORDS] > BBHEAD) \
    { \
      dabufp = bb_write(&big0.gbigBuffer); \
      if(dabufp == NULL) \
      { \
        printf("INFO from bb_write: RETURN 0\n"); \
        /*setHeartBeat(HB_ROL,0,-1);*/ \
        SENDBUFFER_UNLOCK; \
        return; \
      } \
    } \
    else \
    { \
      printf("attempt to send short buffer failed !!!, dabufp[BBIWORDS]=%d\n",dabufp[BBIWORDS]); \
    } \
    /* cleanup 'event counter in buffer' */ \
    g_events_in_buffer = 0; \
    /* determine total # of bytes to be sent */    \
    object_nlongs += dataInBuf >> 2; \
    /* pickup next buffer */ \
    dabufp += BBHEAD; \
    /* reserve space for header */ \
    dataInBuf = BBHEAD_BYTES; \
    clear_to_send = 0; \
    SET_TIMEOUT(abc); \
  } \
  /*setHeartBeat(HB_ROL,0,-1)*/


void 
output_proc_network(int dummy)
{
  objClass object = localobject;
  rocParam rocp;
  ROLPARAMS *rolP;
  int i, lockKey, status, len, ii;
  int itmp;
  char tmpp[1000];

/* timing */
#ifdef VXWORKS
  static unsigned long start, end, time1, time2, time03, nevent;
#else
#ifndef Darwin
  static hrtime_t start, end, time1, time2, time03, nevent;
  static int nev;
  static hrtime_t sum;
#endif
#endif
  /*
  printf("00: rocp->active=%d\n",rocp->active);fflush(stdout);
  rocStatus();fflush(stdout);
  */
  /*
printf("output_proc_network reached\n");fflush(stdout);
  */
  cycle = 10;
  cycle3 = 100000;

  /* Restore pointer to roc private storage */
  rocp = (rocParam) object->privated;
  /*
printf("000: %d %d %d %d\n",rocp->state,DA_ENDING,clear_to_send,rocp_primefd);
  */
  /* if thread(s) returns error: */
  if(big0.failure || big1.failure)
  {
    printf("ERROR: big0.failure=%d, big1.failure=%d\n",big0.failure,big1.failure);fflush(stdout);
    fflush(stdout);
    rocp->active = 0;
    big0.failure = 0;
    big1.failure = 0;

	/* do not call transitions from here - it is deadlock !!!
    set some flag or something - TODO !!!!!!!!!!!!!!!!!
    codaEnd();
	*/

    setHeartBeat(HB_ROL,0,-1);
    return;
  }


  /* Check if we have any events on output queue */
  if(rocp->active > 1)
  {
    setHeartBeat(HB_ROL,2,5);

    /* We should always have a primary list but allow for not just
    in case. We also should stop polling if ending. */
    rolP = rocp->rolPs[0]; /* rol1 */
    if(rolP==0)
    {
      printf("coda_roc: ERROR FATAL: no ROL1: rol1=0x%08x\n",rolP);fflush(stdout);
      fflush(stdout);
      return;
    }
 

    /************************************************************************/
    /* Readout list Polling Loop -- Stay in this loop until we need to send */


    /**********************************************/
    /**********************************************/
    /**********************************************/
    nevent = time1 = time2 = 0;
    do
    {
#ifdef VXWORKS
      start = sysTimeBaseLGet();
#else
      start = gethrtime();
#endif
      setHeartBeat(HB_ROL,3,5);

	  /*
	  printf("11: rocp->active=%d\n",rocp->active);fflush(stdout);
      rocStatus();fflush(stdout);
	  */

	  /*****************/
      /* start of ROL1 */
      setHeartBeat(HB_ROL,4,5);
      if(rocp->state != DA_ENDING)
      {
        /* Do we actually need to poll ?? 
        Don't poll if this is an interrupt driven list
        Don't poll if we have no free buffers ('pool->list.c' is # of event bufs) */
        if(rolP->poll/* && rolP->pool->list.c*/)
        {
          rolP->daproc = DA_POLL_PROC;
          setHeartBeat(HB_ROL,21,5);
#ifdef Linux_vme
#ifdef NEW_ROC
          dabufp_usermembase = bb_get_usermembase(&big0.gbigBuffer);
          dabufp_physmembase = bb_get_physmembase(&big0.gbigBuffer);
#endif
#endif
	      /* printf("11: befor ROL1\n");fflush(stdout);*/
          (*rolP->rol_code) (rolP); /* pseudo-trigger cdopoll */
	      /* printf("11: after ROL1\n");fflush(stdout);*/
          setHeartBeat(HB_ROL,22,5);
        }

        /* 'delayed' done */
        if(rolP->doDone)
        {
          printf("ROLS_LOOP calls __done()\n");fflush(stdout);
          /* "__done()" routine have to be called to enable next trigger;
          we are here if it was not called by 'WRITE_EVENT_'; it happened
          because it was no space in buffer for the next event */
          /* here we wait for buffer become available, and call "__done()" ourself */
          SENDBUFFER_LOCK;
          SEND_BUFFER_(3);
          SENDBUFFER_UNLOCK;
#ifdef VXWORKS
          LOCKINTS;
#endif
		  /* call done only if writing was successful !!!???*/
          rolP->daproc = DA_DONE_PROC;
          rolP->doDone = 0;
          setHeartBeat(HB_ROL,23,5);
          (*rolP->rol_code) (rolP);
          setHeartBeat(HB_ROL,24,5);
#ifdef VXWORKS
          UNLOCKINTS;
#endif
		}
      }
      /* end of ROL1 */
	  /***************/

      /* update statistics: 'object_nlongs' updated by ROL1, we just copying to old location*/
	  object->nlongs = object_nlongs;

      /* check for timeout - NEED TO SYNCHRONIZE WITH ROL1 !!! */
      /* on timeout condition we are sending ONLY if buffer is not empty - this is what we want ? */
#ifdef VXWORKS
      SENDBUFFER_LOCK;
      if(vxTicks > timeout)
      {
        /*printf("timeout: %d %d\n",vxTicks,timeout);*/
        if(dataInBuf > BBHEAD_BYTES)
        {
#ifdef VXWORKS
          LOCKINTS;
#endif
          clear_to_send  = 1;
          SEND_BUFFER_(4);
#ifdef VXWORKS
          UNLOCKINTS;
#endif
        }
        else
		{
          SET_TIMEOUT(89);
		}
        SENDBUFFER_UNLOCK;
        /*printf(" ..break\n");fflush(stdout);*/
        break;
      }
      SENDBUFFER_UNLOCK;
#else
      if(/*time(0)*/gethrtime() > timeout)
      {
		/*printf("timeout..\n");*/
        if(dataInBuf > BBHEAD_BYTES)
        {
		  /*printf("..sent !\n");*/
          clear_to_send  = 1;
      SENDBUFFER_LOCK;
          SEND_BUFFER_(4);
      SENDBUFFER_UNLOCK;
        }
        else
		{
          SET_TIMEOUT(89);
		}
        /*printf(" ..break\n");fflush(stdout);*/
        break;
      }
#endif
      
    } while(1); /* 'do' loop */
    /**********************************************/
    /**********************************************/
    /**********************************************/

    setHeartBeat(HB_ROL,7,1);

#ifndef Darwin
    if(nevent!=0)
    {
      icycle ++;
      if(icycle>=cycle)
      {
#ifdef VXWORKS
        printf("rols_thread: waiting=%7lu processing=%7lu microsec per event (nev=%d)\n",
          time1/nevent,time2/nevent,nevent);
#else
        printf("rols_thread: waiting=%7llu processing=%7llu microsec per event (nev=%d)\n",
          time1/nevent,time2/nevent,nevent);
#endif
        icycle = 0;
      }
      nevent = time1 = time2 = 0;
    }
    else
    {
      /*printf("rols_thread: nevent==0 !!! (end=???) %d %d\n")*/;
    } 
#endif

    setHeartBeat(HB_ROL,8,5);

  } /* if(rocp->active > 1) */


  /********************/
  /********************/
  /*SENDING big buffer - old location*/
  /*
  SEND_BUFFER_(9);
  */
  /********************/
  /********************/

  /************************/
  /* Handle state changes */
  setHeartBeat(HB_ROL,9,5);
  switch(rocp->state)
  {
  case DA_DOWNLOADED: 
    /*printf("DA_DOWNLOADED\n");*/
#ifdef VXWORKS
    taskDelay(sysClkRateGet());
#else
    sleep(1);
#endif
    TRANSITION_LOCK;
	/* ERRORRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR */
    setHeartBeat(HB_ROL,10,5);
    if((rocp->output_switch == 0) && (big1.doclose == 1))
    {
      /* printf("delete \n"); */
      printf("call: '%s close_links'\n", object->name);
      rocCloseLink();

      big1.doclose = 0;
    }
    TRANSITION_UNLOCK;
    break;

  case DA_PRESTARTING:
    /*do nothing, we just prestarting*/
    break;

  case DA_PRESTARTED:
    /*do nothing, we just prestarted and are waiting for 'Go'*/
    break;

  case DA_ACTIVE:
    /*if we are active then all we need to do is pass data to EB*/
    break;

  case DA_PAUSING:
    printf("DA_PAUSING\n");
#ifdef VXWORKS
    taskDelay(sysClkRateGet());
#else
    sleep(1);
#endif
    TRANSITION_LOCK;
    /*
     * If we got this far then either the last event is either on
     * the iov or was the last event on the last iov.
     */
    setHeartBeat(HB_ROL,11,5);
    if(rocp->active != 1)
    {
      if(rocp->async_roc_flag == 0)
      {
        /*ERROR??? can be a problem if ROL1 still filling big buffer ???*/
        informEB(object, (ulong) EV_PAUSE, (ulong) 0, (ulong) object->nevents);
      }
    }
    if(codaUpdateStatus("paused") != CODA_OK)
    {
      setHeartBeat(HB_ROL,0,-1);
      TRANSITION_UNLOCK;
      return;
	}
    tcpState = rocp->state = DA_PAUSED;
    /*
     * in pause we leave the partial iov hanging in the air until
     * someone hits go
     */
    rocp->active = 1;
    TRANSITION_UNLOCK;
    break;

  case DA_ENDING:
    printf("coda_roc: DA_ENDING before lock\n"); fflush(stdout);
    TRANSITION_LOCK;
    printf("coda_roc: DA_ENDING after lock\n"); fflush(stdout);
    setHeartBeat(HB_ROL,12,5);

    /* make sure the output buffer gets sent if there is any data in it */     
    if(dataInBuf > BBHEAD_BYTES)
    {
      clear_to_send = 1;
      printf("coda_roc: in DA_ENDING dataInBuf=%d BBHEAD_BYTES=%d\n",dataInBuf,BBHEAD_BYTES);
      TRANSITION_UNLOCK;
      break;
    }

    /* make sure we have the last event in the output buffer */
    if(/*rocp->last_event*/last_event_check < object->nevents)
    {
      printf("coda_roc: in DA_ENDING last event=%d nevents=%d\n",
			 /*rocp->last_event*/last_event_check,object->nevents);
      TRANSITION_UNLOCK;
      break;
    }

	/* sergey: at that point rocp->last_event == object->nevents ???!!! */
    printf("coda_roc: in DA_ENDING last event=%d nevents=%d\n",
		   /*rocp->last_event*/last_event_check,object->nevents);
    printf("coda_roc: in DA_ENDING rocp->active=%d\n",rocp->active);

    /*if we got this far then either the last event sent was the last event taken
     OR the last event taken is sat at the end of the output queue waiting to go */
    if(rocp->active == 2)
    {
      if(rocp->async_roc_flag == 0)
      {
        printf("coda_roc: call informEB(EV_END)\n");
        informEB(object, (ulong) EV_END, (ulong) object->runNumber,
                 (ulong) object->nevents);
      }
      printf("Inserted End event on queue\n");
      rocp->active = 1;
      if(codaUpdateStatus("downloaded") != CODA_OK)
      {
        setHeartBeat(HB_ROL,0,-1);
        TRANSITION_UNLOCK;
        return;
	  }
      tcpState = rocp->state = DA_DOWNLOADED;
      printf("ended after %d events\n",object->nevents);
      TRANSITION_UNLOCK;
      break;
    }

    TRANSITION_UNLOCK;
    break;

  default:
    printf("default: rocp->state=%d, you better describe it in 'roc_component'\n",rocp->state);
    break;
  }
  setHeartBeat(HB_ROL,13,5);


  return;
}


/* Recover_Init() must be called for following to work !!! (see coda_constructor.c) */
#ifdef Linux_vme
__attribute__((destructor)) void end (void)
{
   printf("coda_roc is exiting, clear dma memory\n");
   bb_dma_free();
}
#endif



/****************/
/* main program */
/****************/

#ifdef VXWORKS

void
coda_roc (char *arg1, char *arg2, char *arg3, char *arg4, char *arg5, char *arg6, char *arg7)
{
	int            argc = 1;
	char           *argv[8];

	/* Copy arguments into argv array */

	argv[0] = "coda_roc";
	if(arg1) {
	  argv[1] = arg1;
	  argc++;
	}
	if(arg2) {
	  argv[2] = arg2;
	  argc++;
	}
	if(arg3) {
	  argv[3] = arg3;
	  argc++;
	}
	if(arg4) {
	  argv[4] = arg4;
	  argc++;
	}
	if(arg5) {
	  argv[5] = arg5;
	  argc++;
	}
	if(arg6) {
	  argv[6] = arg6;
	  argc++;
	}
	if(arg7) {
	  argv[7] = arg7;
	  argc++;
	}
	
	if (argc < 5) {
	  printf (" Wrong number of arguments (must be >= 5) argc = %d\n",argc);
	}else if (argc == 5) { 
	  printf (" Args = %s %s %s %s \n", argv[1], argv[2], argv[3], argv[4]);
	} else if (argc == 6) {
	  printf (" Args = %s %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4], argv[5]);
	} else if (argc == 7){
	  printf (" Args = %s %s %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
	} else {
	  printf (" Args = %s %s %s %s %s %s %s\n", 
		  argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
	}

/**********/


#else


void
main (int argc, char **argv)
{
#endif

  CODA_Init (argc, argv);

  roc_constructor();

printf("main 1\n");
  CODA_Execute ();
printf("main 2\n");
}









