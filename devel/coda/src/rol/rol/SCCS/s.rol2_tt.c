h14687
s 00009/00003/00367
d D 1.5 10/09/27 09:57:40 boiarino 5 4
c *** empty log message ***
e
s 00025/00000/00345
d D 1.4 10/09/01 14:38:47 boiarino 4 3
c add primex
c 
e
s 00158/00129/00187
d D 1.3 10/05/12 12:16:45 boiarino 3 2
c *** empty log message ***
e
s 00001/00001/00315
d D 1.2 08/10/23 23:03:56 boiarino 2 1
c *** empty log message ***
e
s 00316/00000/00000
d D 1.1 06/05/26 16:10:48 boiarino 1 0
c date and time created 06/05/26 16:10:48 by boiarino
e
u
U
f e 0
t
T
I 1

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


D 3
/* need in one place in trigger_dispatch.h; must be undefined in rol1 */
/* it sets in 'event readout'; rol1 rols 'vme readout','gen readout',
'sfi readout' etc keeps that undefined */
E 3
#define EVENT_MODE



I 3
#ifdef TEST_SETUP
#define INIT_NAME rol2_tt_testsetup__init
#else
I 4
#ifdef PRIMEX
#define INIT_NAME rol2_tt_primex__init
#else
E 4
E 3
#define INIT_NAME rol2_tt__init
I 3
#endif
I 4
#endif
E 4
E 3

D 3

E 3
#include "rol.h"

/* event readout */
#include "EVENT_source.h"

/************************/
/************************/


#include "coda.h"
#include "tt.h"

/*extern int TT_nev;*/
extern TTSPtr ttp; /* pointer to TTS structures for current roc */
I 3
extern TrFun TrFunPtr; /* pointer to the translation function */
E 3

long decrement;     /* local decrement counter */
/*extern long nevent;*/ /* local event counter */
long mynev;

D 3
static int proc_on_pmc = 0; /* 1 - coda_proc on pmc, 0 - on host board */
static int net_on_pmc = 0; /* 1 - coda_net on pmc, 0 - on host board */
E 3

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
  printf("rol2: ... done.\n");

  return;
}

static void
__prestart()
{
  unsigned long jj, adc_id, sl;
  char *env;

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  printf("INFO: Entering User Prestart 2\n");

  tttest("\ntrans_rol2:");
D 3
  decrement = 0;
  ttp->wannaraw = 0;
  ttp->profile  = 0;
  ttp->notrans  = 0;
E 3
I 3
  decrement=0;
  ttp->wannaraw=0;
  ttp->profile=1;
  ttp->notrans =0;
E 3
  if( (env=getenv("RAW")) != NULL )
  {
    if( *env == 'T' ) ttp->wannaraw = 1;
  }

D 3
  net_on_pmc = getnetonpmc();
  proc_on_pmc = getproconpmc();
  printf("netonpmc=%d, proconpmc=%d\n",net_on_pmc,proc_on_pmc);

E 3
  TT_Prestart(rol->pid);

  printf("RAW=%d  PROFILE=%d  NOTRANS=%d\n",ttp->wannaraw,ttp->profile,ttp->notrans);
  printf("MAX_EVENT_LENGTH = %d NWBOS = %d\n",MAX_EVENT_LENGTH,NWBOS);

I 3

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
I 4
#ifdef PRIMEX
D 5
       TrFunPtr = NULL; /* croctest5 */
E 5
I 5
       TrFunPtr = 1; /*primexroc4,primexroc5,primexroc6,primexts2,*/
E 5
       ttp->wannaraw=1;
       ttp->notrans=1; 
       ttp->profile=1;
#else
E 4
       TrFunPtr = TT_TranslateDCBank;      /* DC's */
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
I 4
#endif
E 4
       break;
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
I 4
#ifdef PRIMEX
D 5
       TrFunPtr = NULL;
E 5
I 5
       TrFunPtr = 1;
E 5
       ttp->wannaraw=1;
       ttp->notrans=1; 
       ttp->profile=1;
#else
E 4
       TrFunPtr = TT_TranslateNonDCBank;   /* CC1,SC1,EC1,EC2,LAC1 */
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
I 4
#endif
E 4
       break;
    case 17:
I 4
#ifdef PRIMEX
D 5
       TrFunPtr = NULL;
E 5
I 5
       TrFunPtr = 1;
E 5
       ttp->wannaraw=1;
       ttp->notrans=1; 
       ttp->profile=1;
#else
E 4
       TrFunPtr = TT_TranslateTAGEBank;    /* TAGE */
       ttp->wannaraw=0;
       ttp->notrans=0; 
       ttp->profile=1;
I 4
#endif
E 4
       break;
    case 18:
I 5
#ifdef PRIMEX
       TrFunPtr = 1;
       ttp->wannaraw=1;
       ttp->notrans=1; 
       ttp->profile=1;
#endif
E 5
    case 19:
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





E 3
  /* init trig source EVENT */
  EVENT_INIT;

  /* Register a sync trigger source (up to 32 sources) */
  /* CTRIGRSS(source,code,handler,done)*/
D 3
  /*link sync trig source EVENT 1 to davetrig and davetrig_done*/
  CTRIGRSS(EVENT, 1, davetrig, davetrig_done); /* second arg=1 - what is that ? */
E 3
I 3
  /*link sync trig source EVENT 1 to rol2trig and rol2trig_done*/
  CTRIGRSS(EVENT, 1, rol2trig, rol2trig_done); /* second arg=1 - what is that ? */
E 3

D 3
  /*
  event type 1 then read EVENT 1
  event type 2 then read EVENT 1
  event type 3 then read EVENT 1
  event type 4 then read EVENT 1
  event type 5 then read EVENT 1
  event type 6 then read EVENT 1
  event type 7 then read EVENT 1
  event type 8 then read EVENT 1
  event type 9 then read EVENT 1
  */
  /* Register a physics event type (up to 16 types) */
  CRTTYPE(1, EVENT, 1);
  CRTTYPE(2, EVENT, 1);
  CRTTYPE(3, EVENT, 1);
  CRTTYPE(4, EVENT, 1);
  CRTTYPE(5, EVENT, 1);
  CRTTYPE(6, EVENT, 1);
  CRTTYPE(7, EVENT, 1);
  CRTTYPE(8, EVENT, 1);
  CRTTYPE(9, EVENT, 1);

E 3
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

D 3
  if(rol->pid == 25 || rol->pid == 18 || rol->pid == 19 || rol->pid == 23 ||
D 2
     rol->pid == 30 || rol->pid == 27 || rol->pid == 0)
E 2
I 2
     rol->pid == 30 || rol->pid == 0)
E 3
I 3
  if(TrFunPtr==NULL)
E 3
E 2
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
D 3
davetrig(unsigned long EVTYPE, unsigned long EVSOURCE)
E 3
I 3
rol2trig(int a, int b)
E 3
{
D 3
  long EVENT_LENGTH;
  int ii;
  unsigned int recall[2];
E 3
I 3
  int len;
E 3

D 3
  recall[0] = rol->dabufpi[0];
  recall[1] = rol->dabufpi[1];
E 3
I 3
  /*printf("rol2trig reached\n");*/
E 3

D 3
  /*nevent++;*/
E 3
I 3
  rol->dabufp[0] = NWBOS; /* for buffer length checking in TT_Main() */
E 3

D 3
  if(rol->dabufp != NULL)
E 3
I 3
  len = TT_Main(rol->dabufpi, rol->dabufp, rol->pid);
  if(len < 2)
E 3
  {
D 3
    if(input_event__)
    {
      int i,len,tmp;
E 3
I 3
    printf("ERROR in rol2trig: len=%d\n",len);
    len=2;
  }
  else if(len > NWBOS)
  {
    printf("ERROR in rol2trig: event length=%d (longwords) is too big\n",len);
  }
E 3

D 3
      /* trying step back ... */
E 3
I 3
  /* create CODA fragment header */
  rol->dabufp[0] = len - 1;
  rol->dabufp[1] = rol->dabufpi[1];
E 3

D 3
      if( syncFlag==0 && ((rol->dabufpi[1] >> 16 ) & 0xffff) == 0 )
      {
        /*TT_nev = *(rol->nevents);*/
        printf("illegal event number %d !!! second coda header word = %8x\n",
          *(rol->nevents),rol->dabufpi[1]);
        decrement++;
        if(decrement > 1) printf("ILLEGAL EVENT NUMBER %d - trying to recover ...\n",decrement);
        rol->dabufp[0] = 0;
        (*(rol->nevents))--;
      }
      else
      {
E 3
I 3
  /*printf("rol2trig done\n");*/
E 3

D 3
mynev ++;
if(mynev == 256) mynev = 0;

        if(proc_on_pmc == 0) /* run processing code here */
        {
          /*evstart = rol->dabufp;*/
          rol->dabufp[0] = NWBOS; /* for buffer length checking in TT_Main() */
          if(rol->dabufp[0]!=NWBOS) printf("ERR in ROL2: rol->dabufp[0]=%d (0x%08x)\n",
                                           rol->dabufp[0],rol->dabufp);

          /* print input buffer (skip 2 coda header words and 2 BOS header words */
          /*TT_PrintRawBank(rol->dabufpi+4, rol->dabufpi[0], ttp);*/

          len = TT_Main(rol->dabufpi, rol->dabufp, rol->pid);
          /*TT_nev = input_event__->nevent;*/

          /* printf ("ROL2: TT_nev = %d\n",TT_nev); */
          /* if (TT_nev < 0 ) TT_PrintBufOut(len, rol->dabufp);  */
		
          if(len < 2)
          {
            printf("ERROR in list2: len=%d\n",len);
            len=2;
          }

          if(len > NWBOS)
          {
            printf("Error in list2: Event length=%d (longwords) is too big\n",len);
          }

          if(recall[0] != rol->dabufpi[0] || recall[1] != rol->dabufpi[1])
          {
            printf("ERRRR: %d != %d || %d != %d\n",
                   recall[0],rol->dabufpi[0],recall[1],rol->dabufpi[1]);
          }

          rol->dabufp[0] = len - 1;
          rol->dabufp[1] = rol->dabufpi[1];
        }
        else /* processing code will be executed on pmc board, so just copy */
        {
/*printf("use pmc board !!!\n");*/
          len = rol->dabufpi[0]+1;
		  /*
printf("rol2: len=%d\n",rol->dabufpi[0]);
printf("rol2: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
rol->dabufpi[0],rol->dabufpi[1],rol->dabufpi[2],
rol->dabufpi[3],rol->dabufpi[4],rol->dabufpi[5]);
		  */
          for(ii=0; ii<len; ii++)
          {
            *rol->dabufp++ = rol->dabufpi[ii];
          }
		
		  /*
          memcpy(rol->dabufp,rol->dabufpi,len<<2);
          rol->dabufp += len;
		  */
        }
      }

    }
  }

E 3
  return;
}
  
void
D 3
davetrig_done()
E 3
I 3
rol2trig_done()
E 3
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


E 1
