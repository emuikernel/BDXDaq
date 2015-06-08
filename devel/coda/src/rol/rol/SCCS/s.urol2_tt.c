h53975
s 00362/00000/00000
d D 1.1 10/09/27 09:58:17 boiarino 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1

/* urol2.c - UNIX second readout list */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifndef VXWORKS
#include <sys/types.h>
#include <time.h>
#endif

#include "circbuf.h"


/*****************************/
/* former 'crl' control keys */

/* readout list UROL2 */
#define ROL_NAME__ "LIST2"

/* polling */
#define POLLING_MODE


#define EVENT_MODE



#ifdef TEST_SETUP
#define INIT_NAME urol2_tt_testsetup__init
#else
#define INIT_NAME urol2_tt__init
#endif

#include "rol.h"

/* event readout */
#include "EVENT_source.h"

/************************/
/************************/


#include "coda.h"
#include "tt.h"

/*int TT_nev;*/
extern TTSPtr ttp; /* pointer to TTS structures for current roc */
extern TrFun TrFunPtr; /* pointer to the translation function */

long decrement;     /* local decrement counter */
/*long nevent;*/ /* local event counter */
long mynev;



/* user routines */



void
rol2trig(int a, int b)
{
  int len;
  /*
  printf("rol2trig reached\n");fflush(stdout);
  */

  rol->dabufp[0] = NWBOS; /* for buffer length checking in TT_Main() */
  len = TT_Main(rol->dabufpi, rol->dabufp, rol->pid);
  /* create CODA fragment header */
  rol->dabufp[0] = len - 1;
  rol->dabufp[1] = rol->dabufpi[1];

  /*
  printf("rol2trig done\n");fflush(stdout);
  */
  return;
}
  
void
rol2trig_done()
{
  return;
}  





static void
__download()
{

#ifdef POLLING_MODE
   rol->poll = 1;
#endif

  printf("INFO: User Download 2 Executed\n");
  printf("rol2: downloading DDL table ...\n");
  clonbanks();
  printf("rol2: ... done.\n");

  printf("rol2: downloading translation table for roc=%d (ttp=0x%08x)\n",rol->pid,ttp);
  ttp = TT_LoadROCTT(rol->pid, ttp);
  printf("User Download 2: ttp=0x%08x\n",ttp);fflush(stdout);
  printf("rol2: ... done.\n");

  return;
}

static void
__prestart()
{
  unsigned long jj, adc_id, sl;
  char *env;

  printf("INFO: Entering User Prestart 2\n");

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  printf("11\n");fflush(stdout);

  /* need it ??? it is not allocated in ROLPARAMS structure in roc_process ...
  *(rol->nevents) = 0;
  */

  printf("12\n");fflush(stdout);

  /*
  tttest("\ntrans_rol2:");
  */
  decrement = 0;
  ttp->wannaraw=0;
  ttp->profile=1;
  ttp->notrans =0;
  if( (env=getenv("RAW")) != NULL )
  {
    if( *env == 'T' ) ttp->wannaraw = 1;
  }

  TT_Prestart(rol->pid);

  printf("RAW=%d  PROFILE=%d  NOTRANS=%d\n",ttp->wannaraw,ttp->profile,ttp->notrans);
  printf("MAX_EVENT_LENGTH = %d NWBOS = %d\n",MAX_EVENT_LENGTH,NWBOS);

  /* set pointers to the translation routines and translation flags */
  switch(rol->pid)
  {
#ifdef VXWORKS 
    case 0:
       TrFunPtr = TT_TranslateVMEBank; /* croctest2 */
       ttp->wannaraw=1;
       ttp->notrans=1; 
       ttp->profile=1;
#else
    case 0:
       TrFunPtr = TT_TranslateTPCBank; /* bonuspc2 */
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
#endif
       break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
       TrFunPtr = TT_TranslateDCBank;      /* DC's */
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
       break;
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
       TrFunPtr = TT_TranslateNonDCBank;   /* CC1,SC1,EC1,EC2,LAC1 */
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
       break;
    case 17:
       TrFunPtr = TT_TranslateTAGEBank;    /* TAGE */
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
       break;
    case 18:
    case 19:
    case 23:
    case 25:
    case 26:
    case 30:
#ifdef TEST_SETUP
       TrFunPtr = TT_TranslateTPCBank; /* croctest5 */
       ttp->wannaraw=1;
       ttp->notrans=1; 
#else
       TrFunPtr = NULL; /* scaler2,scaler4,scaler3,polar,clastrig2,scaler1 */
       ttp->wannaraw=0;
       ttp->notrans=0; 
#endif
       ttp->profile=1;
       break;
    case 20:
    case 21:
    case 22:
    case 24:
       TrFunPtr = TT_TranslateVMEBank;     /* EC3,EC4,SC2,LAC2 */
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
       break;
    case 27:
       TrFunPtr = TT_TranslateVMEBank;     /* DVCS2 */
       ttp->wannaraw=0;
       ttp->notrans=0;
#ifdef TEST_SETUP
       ttp->wannaraw=1; /* croctest10 temporary !!! */
       ttp->notrans=1;  /* croctest10 temporary !!! */
#endif
       ttp->profile=1;
       break;
#ifdef VXWORKS
    case 28:
       TrFunPtr = TT_TranslateVMETAGEBank; /* TAGE3 */
#else
    case 28:
       TrFunPtr = TT_TranslateTPCBank; /* bonuspc3 */
#endif
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
       break;
#ifdef VXWORKS
    case 29:
       TrFunPtr = TT_TranslateVMETAGEBank; /* TAGE2 */
#else
    case 29:
#ifdef TEST_SETUP
       TrFunPtr = NULL; /* croctest3 */
       ttp->wannaraw=1;
       ttp->notrans=1; 
#else
       TrFunPtr = TT_TranslateTPCBank; /* bonuspc0 */
#endif
#endif
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
       break;
    case 31:
       TrFunPtr = TT_TranslateTPCBank; /* bonuspc1 */
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
       break;
   default:
	 printf("ERROR: illegal roc id = %d\n",rol->pid);
  }


  /* init trig source EVENT */
  EVENT_INIT;

  /* Register a sync trigger source (up to 32 sources) */
  CTRIGRSS(EVENT, 1, rol2trig, rol2trig_done); /* second arg=1 - what is that ? */

  rol->poll = 1;

  printf("33\n");fflush(stdout);

  /* from parser (do we need that in rol2 ???) */
  /*???crashes in linux
  *(rol->nevents) = 0;
  */
  rol->recNb = 0;

  printf("User Prestart 2: ttp=0x%08x\n",ttp);fflush(stdout);
  printf("INFO: User Prestart 2 executed\n");

  return;
}

static void
__end()
{
  printf("INFO: User End 2 Executed\n");

  return;
}

static void
__pause()
{
  printf("INFO: User Pause 2 Executed\n");

  return;
}

static void
__go()
{
  char *env;

  printf("User Go 2 Reached\n");fflush(stdout);

  printf("User Go 2: ttp=0x%08x\n",ttp);fflush(stdout);
  ttp->wannaraw = 0;
  printf("User Go 2: 1\n");fflush(stdout);
  if( (env=getenv("RAW")) != NULL )
  {
    if( *env == 'T' )  ttp->wannaraw = 1;
  }
  if( (env=getenv("PROFILE")) != NULL )
  {
    if( *env == 'T' )  ttp->profile = 1;
  }
  printf("WANNARAW=%d  PROFILE =%d\n",ttp->wannaraw,ttp->profile);fflush(stdout);

  if(TrFunPtr==NULL)
  {
    TT_VmeGo();
    printf("vme ----------------------------\n");fflush(stdout);
  }
  else
  {
    TT_Go();
    printf("regular -------------------------\n");fflush(stdout);
  }

  mynev = 0;

  printf("INFO: User Go 2 Executed\n");fflush(stdout);

  return;
}

void
__done()
{
  /* from parser */
  poolEmpty = 0; /* global Done, Buffers have been freed */

  return;
}
  
static void
__status()
{
  return;
}  
E 1
