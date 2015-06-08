
/* rol2_tt.c */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

/* readout list LIST2 */
#define ROL_NAME__ "LIST2"

/* polling */
#define POLLING_MODE


#define EVENT_MODE



#ifdef TEST_SETUP
#define INIT_NAME rol2_tt_testsetup__init
#else
#ifdef PRIMEX
#define INIT_NAME rol2_tt_primex__init
#else
#define INIT_NAME rol2_tt__init
#endif
#endif

#include "rol.h"

/* event readout */
#include "EVENT_source.h"

/************************/
/************************/


#include "coda.h"
#include "tt.h"

/*extern int TT_nev;*/
extern TTSPtr ttp; /* pointer to TTS structures for current roc */
extern TrFun TrFunPtr; /* pointer to the translation function */

long decrement;     /* local decrement counter */
/*extern long nevent;*/ /* local event counter */
long mynev;


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
  printf("rol2: ... done, ttp=0x%08x\n",ttp);

  return;
}

static void
__prestart()
{
  unsigned long jj, adc_id, sl;
  char *env;

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;

  /* need it ??? it is not allocated in ROLPARAMS structure in roc_process ...*/
  *(rol->nevents) = 0;

  printf("INFO: Entering User Prestart 2\n");

  tttest("\ntrans_rol2:");
  decrement=0;
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
       TrFunPtr = NULL/*TT_TranslateVMEBank*/; /* croctest2 */
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
#ifdef PRIMEX
       TrFunPtr = 1; /*primexroc4,primexroc5,primexroc6,primexts2,*/
       ttp->wannaraw=1;
       ttp->notrans=1; 
       ttp->profile=1;
#else
       TrFunPtr = TT_TranslateFADCBank;      /* DC's */
       ttp->wannaraw=1;
       ttp->notrans=0; 
       ttp->profile=1;
#endif
       break;
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
#ifdef PRIMEX
       TrFunPtr = 1;
       ttp->wannaraw=1;
       ttp->notrans=1; 
       ttp->profile=1;
#else
       TrFunPtr = TT_TranslateNonDCBank;   /* CC1,SC1,EC1,EC2,LAC1 */
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
#endif
       break;
    case 17:
#ifdef PRIMEX
       TrFunPtr = 1;
       ttp->wannaraw=1;
       ttp->notrans=1; 
       ttp->profile=1;
#else
       TrFunPtr = TT_TranslateVMEBank;     /* TAGE*/
       /*TrFunPtr = TT_TranslateTAGEBank;*/    /* TAGE for FASTBUS */
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
#endif
       break;
    case 18:
#ifdef TEST_SETUP
       TrFunPtr = NULL;
       ttp->wannaraw=1;
       ttp->notrans=1; 
       ttp->profile=1;
       break;
#endif
#ifdef PRIMEX
       TrFunPtr = 1;
       ttp->wannaraw=1;
       ttp->notrans=1; 
       ttp->profile=1;
#endif
    case 19:
#ifdef PRIMEX
       TrFunPtr = 1;
       ttp->wannaraw=1;
       ttp->notrans=1; 
       ttp->profile=1;
#endif
    case 23:
    case 25:
    case 26:
    case 30:
#ifdef TEST_SETUP
       TrFunPtr = TT_TranslateTPCBank; /* croctest5 */
#else
       TrFunPtr = NULL; /* scaler2,scaler4,scaler3,polar,clastrig2,scaler1 */
#endif
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
       break;
    case 20:
       TrFunPtr = TT_TranslateVMEBank;     /* EC3,EC4,SC2,LAC2 */
       ttp->wannaraw=0;
/* FADC250 in EC3
TrFunPtr = NULL;
ttp->wannaraw=1;
*/
       ttp->notrans=0; 
       ttp->profile=1;
       break;
    case 21:
    case 22:
    case 24:
       TrFunPtr = TT_TranslateVMEBank;     /* EC3,EC4,SC2,LAC2 */
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
       break;
    case 27:
	   TrFunPtr = NULL; /*dc12*/ /*TT_TranslateVMEBank; DVCS2 */
       ttp->wannaraw=1;
       ttp->notrans=1;
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
#ifdef TEST_SETUP
       TrFunPtr = NULL; /* croctest3 */
#else
       TrFunPtr = TT_TranslateVMETAGEBank; /* TAGE2 */
#endif
#else
    case 29:
#ifdef TEST_SETUP
       TrFunPtr = NULL; /* croctest3 */
#else
       TrFunPtr = TT_TranslateTPCBank; /* bonuspc0 */
#endif
#endif
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
       break;
    case 31:
#ifdef TEST_SETUP
       TrFunPtr = NULL; /* croctest1 */
#else
       TrFunPtr = TT_TranslateTPCBank; /* bonuspc1 */
#endif
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
  /* CTRIGRSS(source,code,handler,done)*/
  /*link sync trig source EVENT 1 to rol2trig and rol2trig_done*/
  CTRIGRSS(EVENT, 1, rol2trig, rol2trig_done); /* second arg=1 - what is that ? */

  rol->poll = 1;

  printf("INFO: User Prestart 2 executed\n");

  /* from parser (do we need that in rol2 ???) */
  *(rol->nevents) = 0;
  rol->recNb = 0;

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

  ttp->wannaraw = 0;
  if( (env=getenv("RAW")) != NULL ) {
    if( *env == 'T' )  ttp->wannaraw = 1;
  }
  if( (env=getenv("PROFILE")) != NULL ) {
    if( *env == 'T' )  ttp->profile = 1;
  }
  printf("WANNARAW=%d  PROFILE =%d\n",ttp->wannaraw,ttp->profile);

  if(TrFunPtr==NULL)
  {
    TT_VmeGo();
    printf("vme ----------------------------\n");
  }
  else
  {
    TT_Go();
    printf("regular -------------------------\n");
  }

  mynev = 0;

  printf("INFO: User Go 2 Executed\n");

  return;
}

void
rol2trig(int a, int b)
{
  int len;

  /*printf("rol2trig reached\n");*/

  rol->dabufp[0] = NWBOS; /* for buffer length checking in TT_Main() */

  len = TT_Main(rol->dabufpi, rol->dabufp, rol->pid);
  if(len < 2)
  {
    printf("ERROR in rol2trig: len=%d\n",len);
    len=2;
  }
  else if(len > NWBOS)
  {
    printf("ERROR in rol2trig: event length=%d (longwords) is too big\n",len);
  }

  /* create CODA fragment header */
  rol->dabufp[0] = len - 1;
  rol->dabufp[1] = rol->dabufpi[1];

  /*printf("rol2trig done\n");*/

  return;
}
  
void
rol2trig_done()
{
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


