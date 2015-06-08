
/* sy527.c - EPICS driver support for CAEN SY527 HV mainframe */

#ifdef vxWorks

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#ifdef vxWorks
#include <vxWorks.h>
#endif

#include "v288.h"
#include "sy527.h"

#undef DEBUG

/* mainframe threads id's and statistic structures */
#ifndef vxWorks
static pthread_t idth[MAX_HVPS];
#endif
static THREAD stat[MAX_HVPS];

/* mutexes */
#ifdef vxWorks
static SEM_ID global_mutex;
static SEM_ID mainframe_mutex[MAX_HVPS];
#else
static pthread_mutexattr_t mattr;
static pthread_mutex_t global_mutex;       /* to access inter-mainframe info */
static pthread_mutex_t mainframe_mutex[MAX_HVPS]; /* to access one mainframe */
#endif


#ifdef VXWORKS
#define LOCK_GLOBAL            semTake(global_mutex, WAIT_FOREVER)
#define UNLOCK_GLOBAL          semGive(global_mutex)
#define LOCK_MAINFRAME(id_m)   semTake(mainframe_mutex[id_m], WAIT_FOREVER)
#define UNLOCK_MAINFRAME(id_m) semGive(mainframe_mutex[id_m])
#else
#define LOCK_GLOBAL            pthread_mutex_lock(&global_mutex)
#define UNLOCK_GLOBAL          pthread_mutex_unlock(&global_mutex)
#define LOCK_MAINFRAME(id_m)   pthread_mutex_lock(&mainframe_mutex[id_m])
#define UNLOCK_MAINFRAME(id_m) pthread_mutex_unlock(&mainframe_mutex[id_m])
#endif


/* flag to tell mainframe thread it is time to exit */
static int force_exit[MAX_HVPS];

/* mainframes */
static int nmainframes = -1;     /* the number of active mainframes */
static int mainframes[MAX_HVPS]; /* list of active mainframes */
static HV *Demand[MAX_HVPS];
static HV *Measure[MAX_HVPS];

/* board-dependent parameter sets */

#define V0Set   0
#define I0Set   1
#define V1Set   2
#define I1Set   3
#define RUp     4
#define RDWn    5
#define Trip    6
#define SVMax   7
#define VMon    8
#define IMon    9
#define Status  10
#define Pw      11 /* switch power ON/OFF */
#define PwEn    12 /* power ENABLE/DISABLE */
#define TripInt 13
#define TripExt 14
#define Tdrift  15

/* following arrays must have the same order as previous defines, for example
previously 'PwEn' is defined as 12, corresponding board-specific parameter
(with the same or different name) mast be in 12-th position (starting from 0)*/

static int nA1520param = 16;
static char A1520param[MAX_PARAM][MAX_NAME] = {
              "V0Set","I0Set","V1Set","I1Set","RUp","RDWn","Trip","SVMax",
              "VMon","IMon","Status","Pw","PwEn","TripInt","TripExt","Tdrift"};

static int nA1533_1733param = 16;
static char A1533_1733param[MAX_PARAM][MAX_NAME] = {
              "V0Set","I0Set","V1Set","I1Set","RUp","RDWn","Trip","SVMax",
              "VMon","IMon","Status","Pw","POn","PDwn","TripInt","TripExt"};

/*there is no enable ???*/
static int nA944Nparam = 16;
static char A944Nparam[MAX_PARAM][MAX_NAME] = {
              "V0Set","I0Set","V1Set","I1Set","Rup","Rdwn","Trip","SVMax",
              "VMon","IMon","Status","Pw","PrOn","PrOff","Pon","Pdwn"};

/**********************/
/* some usefun macros */

/* check if 'id' is reasonable */
#define CHECK_ID(id_m) \
  if(id_m < 0 || id_m >= MAX_HVPS) \
  { \
    printf("ERROR(sy527): bad id: 0<=%d<%d is not true\n",id_m,MAX_HVPS); \
    return(CAENHV_SYSERR); \
  }

/* check if that channel is free */
#define CHECK_FREE(id_m) \
  if(Measure[id_m]->id != -1) \
  { \
    printf("ERROR(sy527): communication #%d is not free\n",id_m); \
    return(CAENHV_SYSERR); \
  }

/* check if that channel is opened */
#define CHECK_OPEN(id_m) \
  if(Measure[id_m]->id == -1) \
  { \
    printf("ERROR(sy527): communication #%d is not opened\n",id_m); \
    return(CAENHV_SYSERR); \
  }


/*****************************************************************************/
/**************************** Low level functions ****************************/
/*****************************************************************************/

/* */
int
sy527Measure2Demand(unsigned int id, unsigned int board)
{
  printf("sy527Measure2Demand: befor: %d %d (size=%d)\n",
	 Demand[id]->board[board].nchannels,Measure[id]->board[board].nchannels,
     sizeof(BOARD));
  memcpy((BOARD *)&Demand[id]->board[board].nchannels,
         (BOARD *)&Measure[id]->board[board].nchannels,
         sizeof(BOARD));
  printf("sy527Measure2Demand: after: %d %d\n",
	 Demand[id]->board[board].nchannels,Measure[id]->board[board].nchannels);
  return(CAENHV_OK);
}


/* */
int
sy527GetBoard(unsigned int id, unsigned int board)
{
  int nXXXXXparam;
  char *XXXXparam[MAX_PARAM];

  char name[MAX_NAME];
  int i, ipar, ret;
  unsigned short Slot, ChNum, ChList[MAX_CHAN], Ch;
  float fParValList[MAX_CHAN];
  unsigned long	tipo, lParValList[MAX_CHAN];
  char ParName[MAX_NAME];

  CHECK_ID(id);
  CHECK_OPEN(id);
  strcpy(name, Measure[id]->name);

  nXXXXXparam = Measure[id]->board[board].nparams;
  for(i=0;i<nXXXXXparam;i++) XXXXparam[i]=Measure[id]->board[board].parnames[i];

  Slot = board;
  ChNum = Measure[id]->board[board].nchannels;

  for(i = 0; i<ChNum; i++)
  {
    Ch = i;
    ChList[i] = (unsigned short)Ch;
  }

  /* loop over parameters */
  for(ipar=0; ipar<nXXXXXparam; ipar++)
  {
    strcpy(ParName,XXXXparam[ipar]); /* Param name */
    tipo = Measure[id]->board[board].partypes[ipar];
#ifdef DEBUG
    printf("%s called ipar=%d\n",__FUNCTION__,ipar);
#endif
    if(tipo == PARAM_TYPE_NUMERIC)
    {
      ret = CAENHVGetChParam(name, Slot, ParName, ChNum, ChList, fParValList);
    }
    else
    {
      ret = CAENHVGetChParam(name, Slot, ParName, ChNum, ChList, lParValList);
    }

    if(ret != CAENHV_OK)
    {
      printf("CAENHVGetChParam: %s (num. %d)\n\n", CAENHVGetError(name), ret);
    }
    else
    {
      /*printf("PARAM VALUE\n");*/
      if(tipo == PARAM_TYPE_NUMERIC)
      {
        for(i=0; i<ChNum; i++)
        {
          Measure[id]->board[board].channel[i].fval[ipar] = fParValList[i];
#ifdef DEBUG
          printf("%s Slot: %2d  Ch: %3d  %s: %10.2f\n", __FUNCTION__, Slot, ChList[i],
            ParName, fParValList[i]);
#endif
        }
      }
      else
      {
        for(i=0; i<ChNum; i++)   
        {
          Measure[id]->board[board].channel[i].lval[ipar] = lParValList[i];
#ifdef DEBUG
          printf("%s Slot: %2d  Ch: %3d  %s: %lx\n", __FUNCTION__, Slot, ChList[i],
            ParName, lParValList[i]);
#endif
        }
      }
    }
  }

  return(CAENHV_OK);
}

/* */
int
sy527SetBoard(unsigned int id, unsigned int board)
{
  int nXXXXXparam;
  char *XXXXparam[MAX_PARAM];

  char name[MAX_NAME];
  int i, ipar, iparr, ret;
  unsigned short Slot, ChNum, ChList[MAX_CHAN], Ch;
  float fParVal; /*, fparval[MAX_CHAN];*/
  unsigned long	tipo, lParVal; /*, lparval[MAX_CHAN];*/
  char ParName[MAX_NAME];

  CHECK_ID(id);
  CHECK_OPEN(id);
  strcpy(name, Demand[id]->name);

  nXXXXXparam = Demand[id]->board[board].nparams;
  for(i=0;i<nXXXXXparam;i++) XXXXparam[i]=Demand[id]->board[board].parnames[i];

  Slot = board;
  ChNum = Demand[id]->board[board].nchannels;
  for(i = 0; i<ChNum; i++) ChList[i] = (unsigned short)i;

  /* loop over parameters */
  for(iparr=0; iparr<nXXXXXparam; iparr++)
  {
	/* patch to make sure 'PwEn' always executed before 'Pw' */
    if(iparr == Pw)        ipar = PwEn;
    else if(iparr == PwEn) ipar = Pw;
    else                   ipar = iparr;

    strcpy(ParName,XXXXparam[ipar]); /* Param name */
    tipo = Demand[id]->board[board].partypes[ipar];



    /* will be good to do it this way, but is does not work
    if(tipo == PARAM_TYPE_NUMERIC)
    {
      for(Ch=0; Ch<ChNum; Ch++)
      {
        fparval[Ch] = Demand[id]->board[board].channel[Ch].fval[ipar];
	    fparval[Ch] = 1.0*Ch+1.0;
        printf("Value %s: %f\n",ParName,fparval[Ch]);
      }
      ret = CAENHVSetChParam(name, Slot, ParName, ChNum, ChList, fparval);
    }
    else
    {
      for(Ch=0; Ch<ChNum; Ch++)
      {
        lparval[Ch] = Demand[id]->board[board].channel[Ch].lval[ipar];
        printf("Value %s: %ld\n",ParName,lparval[Ch]);
      }
      ret = CAENHVSetChParam(name, Slot, ParName, ChNum, ChList, lparval);
    }

    if(ret != CAENHV_OK)
    {
      printf("CAENHVSetChParam: %s (num. %d)\n",CAENHVGetError(name),ret);
      return(CAENHV_SYSERR);
    }
    */

    /* loop over all channels */
    for(Ch=0; Ch<ChNum; Ch++)
    {
      if(Demand[id]->board[board].channel[Ch].setflag[ipar] == 1)
      {
        if(tipo == PARAM_TYPE_NUMERIC)
        {
          fParVal = Demand[id]->board[board].channel[Ch].fval[ipar];
          /*printf("Set Value %s: %f\n",ParName,fParVal);*/
          ret = CAENHVSetChParam(name, Slot, ParName, 1, &Ch, &fParVal);
        }
        else
        {
          lParVal = Demand[id]->board[board].channel[Ch].lval[ipar];
          printf("Set Value %s: %ld\n",ParName,lParVal);
          ret = CAENHVSetChParam(name, Slot, ParName, 1, &Ch, &lParVal);
        }

        if(ret != CAENHV_OK)
        {
          /* set was unsuccessful so return error */
          printf("CAENHVSetChParam: %s (num. %d)\n",CAENHVGetError(name),ret);
          return(CAENHV_SYSERR);
        }
        else
        {
          /* set was successful so cleanup setflag */
          Demand[id]->board[board].channel[Ch].setflag[ipar] = 0;
        }
      }
    }
  }

  return(CAENHV_OK);
}

/* */
int
sy527GetMap(unsigned int id)
{ 
  unsigned short NrOfSl, *SerNumList, *NrOfCh, ChList[MAX_CHAN];
  char *ModelList, *DescriptionList;
  unsigned char	*FmwRelMinList, *FmwRelMaxList;
  char name[MAX_NAME];
  int i, j, ret;
  unsigned long tipo;
  char ParName[MAX_NAME];

  CHECK_ID(id);
  CHECK_OPEN(id);
  strcpy(name, Measure[id]->name);

  ret = CAENHVGetCrateMap(name, &NrOfSl, &NrOfCh, &ModelList,
                          &DescriptionList, &SerNumList,
                          &FmwRelMinList, &FmwRelMaxList );
  if(ret != CAENHV_OK)
  {
    printf("ERROR(sy527): %s (num. %d)\n\n", CAENHVGetError(name), ret);
  }
  else
  {
    char *m = ModelList, *d = DescriptionList;

    /*printf("Measure MAP:\n\n");*/
    Measure[id]->nslots = NrOfSl;
    printf("=========================> %d %d\n",id,Measure[id]->nslots);
    Demand[id]->nslots = NrOfSl;
    for(i=0; i<NrOfSl; i++, m+=strlen(m)+1, d+=strlen(d)+1)
    {
      if(*m == '\0')
      {
        Measure[id]->board[i].nchannels = 0;
        Demand[id]->board[i].nchannels = 0;
        printf("Board %2d: Not Present\n", i);
      }
      else
      {
        strncpy(Measure[id]->board[i].modelname,m,MAX_NAME-1);
        strncpy(Measure[id]->board[i].description,d,MAX_NAME-1);
        Measure[id]->board[i].nchannels = NrOfCh[i];
        Measure[id]->board[i].sernum = SerNumList[i];
        Measure[id]->board[i].relmax = FmwRelMaxList[i];
        Measure[id]->board[i].relmin = FmwRelMinList[i];
        strncpy(Demand[id]->board[i].modelname,m,MAX_NAME-1);
        strncpy(Demand[id]->board[i].description,d,MAX_NAME-1);
        Demand[id]->board[i].nchannels = NrOfCh[i];
        Demand[id]->board[i].sernum = SerNumList[i];
        Demand[id]->board[i].relmax = FmwRelMaxList[i];
        Demand[id]->board[i].relmin = FmwRelMinList[i];
        printf("Board %2d: %s %s  Nr. Ch: %d  Ser. %d   Rel. %d.%d\n",
                i, m, d, NrOfCh[i], SerNumList[i], FmwRelMaxList[i], 
                FmwRelMinList[i]);
		
		

        /* get params info */
        for(j=0; j<Demand[id]->board[i].nchannels; j++)
          ChList[j] = (unsigned short)j;
        printf("The board is: >%s<\n",Measure[id]->board[i].modelname);
        if( !strcmp(Measure[id]->board[i].modelname,"A1520") )
        {
    	  printf("The board %s is connecting\n",Measure[id]->board[i].modelname);
          Measure[id]->board[i].nparams = nA1520param;
          Demand[id]->board[i].nparams = nA1520param;
          for(j=0; j<Measure[id]->board[i].nparams; j++)
          {
            strcpy(Measure[id]->board[i].parnames[j],A1520param[j]);
            strcpy( Demand[id]->board[i].parnames[j],A1520param[j]);

            strcpy(ParName,Measure[id]->board[i].parnames[j]);
            ret=CAENHVGetChParamProp(name,i,ChList[0],ParName,"Type",&tipo);
            if(ret != CAENHV_OK)
            {
              printf("CAENHVGetChParamProp: %s (num. %d) ParName=>%s<\n",
                     CAENHVGetError(name),ret,ParName);
              Measure[id]->board[i].nchannels = 0;
              Demand[id]->board[i].nchannels = 0;
              return(CAENHV_SYSERR);
            }
            else
            {
              Measure[id]->board[i].partypes[j] = tipo;
              Demand[id]->board[i].partypes[j] = tipo;
            }
          }
        }
        else if( !strcmp(Measure[id]->board[i].modelname,"A1533") ||
		         !strcmp(Measure[id]->board[i].modelname,"A1733") )
        {
    	  printf("The board %s is connecting\n",Measure[id]->board[i].modelname);
          Measure[id]->board[i].nparams = nA1533_1733param;
          Demand[id]->board[i].nparams = nA1533_1733param;
          for(j=0; j<Measure[id]->board[i].nparams; j++)
          {
            strcpy(Measure[id]->board[i].parnames[j],A1533_1733param[j]);
            strcpy( Demand[id]->board[i].parnames[j],A1533_1733param[j]);

            strcpy(ParName,Measure[id]->board[i].parnames[j]);
            ret=CAENHVGetChParamProp(name,i,ChList[0],ParName,"Type",&tipo);
            if(ret != CAENHV_OK)
            {
              printf("CAENHVGetChParamProp: %s (num. %d) ParName=>%s<\n",
                     CAENHVGetError(name),ret,ParName);
              Measure[id]->board[i].nchannels = 0;
              Demand[id]->board[i].nchannels = 0;
              return(CAENHV_SYSERR);
            }
            else
            {
              Measure[id]->board[i].partypes[j] = tipo;
              Demand[id]->board[i].partypes[j] = tipo;
            }
          }
        }
        else if( !strcmp(Measure[id]->board[i].modelname,"A944N") )
        {
    	  printf("The board %s is connecting\n",Measure[id]->board[i].modelname);
          Measure[id]->board[i].nparams = nA944Nparam;
          Demand[id]->board[i].nparams = nA944Nparam;
          for(j=0; j<Measure[id]->board[i].nparams; j++)
          {
            strcpy(Measure[id]->board[i].parnames[j],A944Nparam[j]);
            strcpy( Demand[id]->board[i].parnames[j],A944Nparam[j]);

            strcpy(ParName,Measure[id]->board[i].parnames[j]);
            ret=CAENHVGetChParamProp(name,i,ChList[0],ParName,"Type",&tipo);
            if(ret != CAENHV_OK)
            {
              printf("CAENHVGetChParamProp: %s (num. %d) ParName=>%s<\n",
                     CAENHVGetError(name),ret,ParName);
              Measure[id]->board[i].nchannels = 0;
              Demand[id]->board[i].nchannels = 0;
              return(CAENHV_SYSERR);
            }
            else
            {
              Measure[id]->board[i].partypes[j] = tipo;
              Demand[id]->board[i].partypes[j] = tipo;
            }
          }
        }
        else
        {
          printf("Unknown board >%s<\n",Measure[id]->board[i].modelname);
          Measure[id]->board[i].nchannels = 0;
          Demand[id]->board[i].nchannels = 0;
        }
      }
    }
    /*printf("\n");*/
    free(SerNumList);
    free(ModelList);
    free(DescriptionList);
    free(FmwRelMinList);
    free(FmwRelMaxList);
    free(NrOfCh);
  }

  return(CAENHV_OK);
}

/* */
int
sy527PrintMap(unsigned int id)
{
  int i, j;

  CHECK_ID(id);
  CHECK_OPEN(id);

  printf("\n\nMAP for mainframe %d, nslots=%d\n\n",id,Measure[id]->nslots);
  for(i=0; i<Measure[id]->nslots; i++)
  {
    if(Measure[id]->board[i].nchannels == 0)
    {
      printf("Board %2d: Not Present\n", i);
    }
    else
    {
      printf("Board %2d: %s %s  Nr. Ch: %d  Ser. %d   Rel. %d.%d\n",
              i,
              Measure[id]->board[i].modelname,
              Measure[id]->board[i].description,
              Measure[id]->board[i].nchannels,
              Measure[id]->board[i].sernum,
              Measure[id]->board[i].relmax,
              Measure[id]->board[i].relmin);
      printf("  nparams = %d\n",Measure[id]->board[i].nparams);
      for(j=0; j<Measure[id]->board[i].nparams; j++)
      {
        printf("    >%s<\t\t%ld",
          Measure[id]->board[i].parnames[j],Measure[id]->board[i].partypes[j]);
        if(Measure[id]->board[i].partypes[j]==PARAM_TYPE_NUMERIC)
          printf(" (PARAM_TYPE_NUMERIC)\n");
        else if(Measure[id]->board[i].partypes[j]==PARAM_TYPE_ONOFF)
          printf(" (PARAM_TYPE_ONOFF)\n");
        else if(Measure[id]->board[i].partypes[j]==PARAM_TYPE_CHSTATUS)
          printf(" (PARAM_TYPE_CHSTATUS)\n");
        else if(Measure[id]->board[i].partypes[j]==PARAM_TYPE_BDSTATUS)
          printf(" (PARAM_TYPE_BDSTATUS)\n");
        else
          printf(" (? ? ?  UNKNOWN  ? ? ?)\n");
      }
    }
  }
  printf("\n");
 
  return(CAENHV_OK);
}

/* mainframe thread */
void *
sy527Thread(void *arg)
{
  int id, i, ret, status;

  id=((THREAD *)arg)->threadid;
  printf("[%2d] starts thread\n",id);

  while(1)
  {
    printf("sy527Thread 1\n");
#ifdef vxWorks
    ttsleep(100);
#else
    sleep(1);
#endif
    LOCK_MAINFRAME(id);

    if(force_exit[id])
    {
      UNLOCK_MAINFRAME(id);

      break;
    }

    /***** talk to mainframe here *****/

    /* sets all existing boards in mainframe 'id' with 'setflag' */
    if(Demand[id]->setflag == 1)
    {
      status = CAENHV_OK;
      for(i=0; i<Measure[id]->nslots; i++)
      {
        if(Demand[id]->board[i].nchannels > 0)
        {
          if(Demand[id]->board[i].setflag == 1)
          {
            /*printf("[%2d] sets board %d\n",id,i);*/
            ret = sy527SetBoard(id,i);
            if(ret == CAENHV_OK) Demand[id]->board[i].setflag = 0;
            else                 status |= CAENHV_SYSERR;
          }
        }
      }
      if(status == CAENHV_OK) Demand[id]->setflag = 0;
    }

    /* gets all existing boards in mainframe 'id' */
    for(i=0; i<Measure[id]->nslots; i++)
    {
      /* measure all active boards */
      if(Measure[id]->board[i].nchannels > 0)
      {
        /*printf("[%2d] reads out board %d\n",id,i);*/
        ret = sy527GetBoard(id,i);
      }
    }
    /**********************************/

    UNLOCK_MAINFRAME(id);
  }

  printf("[%2d] exit thread\n",id);

  /*return;*/
}

/* initialization */
int
sy527Init()
{
  int i;

  printf("sy527Init: 1: sizeof(HV)=%d\n",sizeof(HV));fflush(stdout);
  for(i=0; i<MAX_HVPS; i++)
  {
  printf("sy527Init: 11: %d\n",i);fflush(stdout);
    Demand[i] = calloc(1,sizeof(HV));
  printf("sy527Init: 12: %d\n",i);fflush(stdout);
    Measure[i] = calloc(1,sizeof(HV));
  printf("sy527Init: 13: %d\n",i);fflush(stdout);
  }
  printf("sy527Init: 2\n");fflush(stdout);

  nmainframes = 0;
  memset(Measure[0],0,sizeof(HV));
  printf("sy527Init: 3\n");fflush(stdout);
  memset(Demand[0],0,sizeof(HV));
  printf("sy527Init: 4\n");fflush(stdout);
  for(i=0; i<MAX_HVPS; i++)
  {
  printf("sy527Init: 5\n");fflush(stdout);
    Measure[i]->id = -1;
  printf("sy527Init: 6\n");fflush(stdout);
    Demand[i]->id = -1;
  printf("sy527Init: 7\n");fflush(stdout);
    mainframes[i] = -1;
  }
  printf("sy527Init: 8\n");fflush(stdout);

#ifndef vxWorks
  /* set mutex attributes */
  pthread_mutexattr_init(&mattr);
  pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
  /* init global mutex */
  pthread_mutex_init(&global_mutex, &mattr);
#else
  global_mutex = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if(global_mutex == NULL)
  {
    printf("ERROR: could not allocate 'global_mutex' semaphore\n");
    return(0);
  }
#endif

  printf("sy527Init: 9\n");fflush(stdout);

  return(CAENHV_OK);
}

/* open communication with mainframe under logical number 'id',
   do all necessary initialization and start thread */
int
sy527Start(unsigned id, char *ip_address)
{
  char arg[30], userName[20], passwd[30], name[MAX_NAME];
  int link, ret;

  printf("123\n");fflush(stdout);

  /* do global initialization on first call */
  if(nmainframes == -1) sy527Init();

  printf("456\n");fflush(stdout);

  /* lock global mutex to prevent other mainframes to be started
     until we are done with this one */
  LOCK_GLOBAL;

  if(nmainframes >= MAX_HVPS)
  {
    printf("ERROR: no more empty slots\n");
    UNLOCK_GLOBAL;

    return(CAENHV_SYSERR);
  }

  CHECK_ID(id);
  CHECK_FREE(id);

  sprintf(name,"sy527_%03d",id);
  printf("System Name set as >%s<\n",name);

  /*strcpy(arg,ip_address); for sy1527*/
  /*printf("TCP/IP address set as >%s<\n",arg);*/

  strcpy(arg,"sy527_0x100000_9");

  /*link = LINKTYPE_TCPIP; for sy1527*/
  link = LINKTYPE_CAENET;

  strcpy(userName, "user");
  strcpy(passwd, "user");

  ret = CAENHVInitSystem(name, link, arg, userName, passwd);

  printf("\nCAENHVInitSystem: %s (num. %d)\n\n", CAENHVGetError(name),ret);

  if(ret == CAENHV_OK)
  {
    Measure[id]->id = ret;
    strcpy(Measure[id]->name, name); 
    Demand[id]->id = ret;
    strcpy(Demand[id]->name, name); 
  }
  else
  {
    printf("\n CAENHVInitSystem returned %d\n\n",ret);    
    UNLOCK_GLOBAL;

    return(CAENHV_SYSERR);
  }


#ifndef vxWorks
  /* init mainframe mutex */
  pthread_mutex_init(&mainframe_mutex[id], &mattr);
#else
  mainframe_mutex[id] = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if(mainframe_mutex[id] == NULL)
  {
    printf("ERROR: could not allocate 'mainframe_mutex' semaphore\n");
    return(0);
  }
#endif


  /* start thread */
  stat[id].threadid = id;
  force_exit[id] = 0;

#ifdef vxWorks
  {
    int iTaskStat;
    iTaskStat = taskSpawn("SY527THRD", 150, 0, 800000, sy527Thread,
                          &stat[id], 0, 0,0,0,0,0,0,0,0);
    printf("taskSpawn(\"SY527THRD\") returns %d\n",iTaskStat);
    if(iTaskStat == ERROR)
    {
	  perror("taskSpawn"); 
      UNLOCK_GLOBAL;
    }
  }
#else
  if(pthread_create(&idth[id],NULL,sy527Thread,(void *)&stat[id])!=0)
  {
    printf("ERROR: pthread_create(0x%08lx[%d],...) failure\n",idth[id],id);
    UNLOCK_GLOBAL;
    return(CAENHV_SYSERR);
  }
  else
  {
    printf("INFO: pthread_create(0x%08lx[%d],...) done\n",idth[id],id);
  }
#endif

  /* register mainframe */
  mainframes[nmainframes++] = id;

  /* get mainframe map */
  sy527GetMap(id);

  UNLOCK_GLOBAL;

  return(CAENHV_OK);
}

/* close communication with mainframe under logical number 'id'
   and stop thread */
int
sy527Stop(unsigned id)
{
  char name[MAX_NAME];
  int i, j, ret;

  /* lock global mutex to prevent other mainframes to be stoped
     until we are done with this one */
  LOCK_GLOBAL;

  CHECK_ID(id);
  CHECK_OPEN(id);
  strcpy(name, Measure[id]->name);


  /* stop thread */
  force_exit[id] = 1;


  ret = CAENHVDeinitSystem(name);

  if(ret == CAENHV_OK)
  {
    printf("\nConnection >%s< closed (num. %d)\n\n", name,ret);
    Measure[id]->id = -1;
  }
  else
  {
    printf("\nERROR(sy527): %s (num. %d)\n\n", CAENHVGetError(name), ret);
    UNLOCK_GLOBAL;

    return(CAENHV_SYSERR);
  }

  /* unregister mainframe */
  j = -1;
  for(i=0; i<nmainframes; i++)
  {
    if(mainframes[i] == id)
    {
      j = i;
      break;
    }
  }
  if(j==-1)
  {
    printf("ERROR: mainframe %d was not registered\n",id);
    UNLOCK_GLOBAL;

    return(CAENHV_SYSERR);
  }
  for(i=j; i<nmainframes; i++) mainframes[i] = mainframes[i+1];
  nmainframes --;

  UNLOCK_GLOBAL;

  return(CAENHV_OK);
}


/*****************************************************************************/
/**************************** High level functions ***************************/
/*****************************************************************************/
/*** CAN BE BOARD-DEPENDANT STUFF HERE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ***/
/*****************************************************************************/

/* some useful macros */
#define SET_FVALUE(prop_name_m, value_m) \
  Demand[id]->board[board].channel[chan].fval[prop_name_m] = value_m; \
  Demand[id]->board[board].channel[chan].setflag[prop_name_m] = 1; \
  Demand[id]->board[board].setflag = 1; \
  Demand[id]->setflag = 1

#define SET_LVALUE(prop_name_m, value_m) \
  Demand[id]->board[board].channel[chan].lval[prop_name_m] = value_m; \
  Demand[id]->board[board].channel[chan].setflag[prop_name_m] = 1; \
  Demand[id]->board[board].setflag = 1; \
  Demand[id]->setflag = 1

#define GET_FVALUE(prop_name_m, value_m) \
  value_m = Measure[id]->board[board].channel[chan].fval[prop_name_m]

#define GET_LVALUE(prop_name_m, value_m) \
  value_m = Measure[id]->board[board].channel[chan].lval[prop_name_m]

/* loop over all available channels and set them on or off */
int
sy527SetMainframeOnOff(unsigned int id, unsigned int on_off)
{
  int i, board, chan;

  LOCK_GLOBAL;

  /* check if it is active */
  for(i=0; i<nmainframes; i++)
  {
    /*printf("-> check mainframe %d\n",i);*/
    if(mainframes[i] == id)
    {
      /*printf("-> mainframe %d has id=%d, so loop over %d boards\n",
        i,id,Measure[id]->nslots);*/
      /* loop over all channels in all boards and set it to 'on_off' */

      for(board=0; board<Measure[id]->nslots; board++)
      {
        /*printf("-> slot %d: nchannels=%d\n",
          board,Measure[id]->board[board].nchannels);*/
        for(chan=0; chan<Measure[id]->board[board].nchannels; chan++)
        {
          /*printf("-> set channel %d to %d\n",chan, on_off);*/
          SET_LVALUE(Pw, on_off);
        }
      }
    }
  }

  UNLOCK_GLOBAL;

  return(CAENHV_OK);
}

/* */
int
sy527GetMainframeStatus(unsigned int id, int *active, int *onoff, int *alarm)
{
  int i, board, chan;
  unsigned int u;

  LOCK_GLOBAL;

  *active = *onoff = *alarm = 0;

  /* check if it is active */
  for(i=0; i<nmainframes; i++)
  {
    if(mainframes[i] == id)
    {
      *active = 1;


      /* check if it is ON: loop over all boards and channels */
      /* and if at least one channel is ON, report mainframe  */
      /* status as ON, overwise report it as OFF */

      for(board=0; board<Measure[id]->nslots; board++)
      {
        for(chan=0; chan<Measure[id]->board[board].nchannels; chan++)
        {
          /* check on/off status */
          GET_LVALUE(Pw, u);
          if(u) *onoff = 1;

          /* check I-tripped bit */
          GET_LVALUE(Status, u);
          if(u & 0x200) *alarm = 1;
        }
      }

    }
  }

  UNLOCK_GLOBAL;

  return(CAENHV_OK);
}

/* sets demand voltage for one channel */
int
sy527SetChannelDemandVoltage(unsigned int id, unsigned int board,
                              unsigned int chan, float u)
{
  LOCK_MAINFRAME(id);
  SET_FVALUE(V0Set, u);
  UNLOCK_MAINFRAME(id);
  return(0);
}

/* returns demand voltage for one channel */
float
sy527GetChannelDemandVoltage(unsigned int id, unsigned int board,
                              unsigned int chan)
{
  float u;
  LOCK_MAINFRAME(id);
  GET_FVALUE(V0Set, u);
  UNLOCK_MAINFRAME(id);
  return(u);
}

/* sets maximum voltage for one channel */
int
sy527SetChannelMaxVoltage(unsigned int id, unsigned int board,
                           unsigned int chan, float u)
{
  LOCK_MAINFRAME(id);
  SET_FVALUE(SVMax, u);
  UNLOCK_MAINFRAME(id);
  return(0);
}

/* returns maximum voltage for one channel */
float
sy527GetChannelMaxVoltage(unsigned int id, unsigned int board,
                           unsigned int chan)
{
  float u;
  LOCK_MAINFRAME(id);
  GET_FVALUE(SVMax, u);
  UNLOCK_MAINFRAME(id);
  return(u);
}

/* sets maximum current for one channel */
int
sy527SetChannelMaxCurrent(unsigned int id, unsigned int board,
                           unsigned int chan, float u)
{
  LOCK_MAINFRAME(id);
  SET_FVALUE(I0Set, u);
  UNLOCK_MAINFRAME(id);
  return(0);
}

/* returns maximum current for one channel */
float
sy527GetChannelMaxCurrent(unsigned int id, unsigned int board,
                           unsigned int chan)
{
  float u;
  LOCK_MAINFRAME(id);
  GET_FVALUE(I0Set, u);
  UNLOCK_MAINFRAME(id);
  return(u);
}

/* returns measured voltage for one channel */
float
sy527GetChannelMeasuredVoltage(unsigned int id, unsigned int board,
                                unsigned int chan)
{
  float u;
  LOCK_MAINFRAME(id);
  GET_FVALUE(VMon, u);
  UNLOCK_MAINFRAME(id);
  return(u);
}

/* returns measured current for one channel */
float
sy527GetChannelMeasuredCurrent(unsigned int id, unsigned int board,
                                unsigned int chan)
{
    int ret;
  float u;
/*  CHECK_OPEN(id);
    sy527Start(id,"129.57.69.101");
    sy527PrintMap(id);
    sy527GetMap(id);
    ret = sy527GetBoard(id, board); */
  LOCK_MAINFRAME(id);



  GET_FVALUE(IMon, u);
  /*GET_FVALUE(V0Set, u);*/



  UNLOCK_MAINFRAME(id);
#ifdef DEBUG
  printf("%s:%s(%d): MF id=%u board=%u Chan=%u Val=%f Status=%d\n",
    __FILE__, __FUNCTION__, __LINE__, id, board, chan, u, ret);
#endif
  return(u);
}

/* sets Ramp-up speed for one channel */
int
sy527SetChannelRampUp(unsigned int id, unsigned int board, unsigned int chan,
                       float u)
{
  LOCK_MAINFRAME(id);
  SET_FVALUE(RUp, u);
  UNLOCK_MAINFRAME(id);
  return(0);
}

/* returns Ramp-up speed for one channel */
float
sy527GetChannelRampUp(unsigned int id, unsigned int board, unsigned int chan)
{
  float u;
  LOCK_MAINFRAME(id);
  GET_FVALUE(RUp, u);
  UNLOCK_MAINFRAME(id);
  return(u);
}

/* sets Ramp-down speed for one channel */
int
sy527SetChannelRampDown(unsigned int id, unsigned int board,
                         unsigned int chan, float u)
{
  LOCK_MAINFRAME(id);
  SET_FVALUE(RDWn, u);
  UNLOCK_MAINFRAME(id);
  return(0);
}

/* returns Ramp-down speed for one channel */
float
sy527GetChannelRampDown(unsigned int id, unsigned int board,
                         unsigned int chan)
{
  float u;
  LOCK_MAINFRAME(id);
  GET_FVALUE(RDWn, u);
  UNLOCK_MAINFRAME(id);
  return(u);
}

/* sets on/off status for one channel */
int
sy527SetChannelOnOff(unsigned int id, unsigned int board,
                      unsigned int chan, unsigned int u)
{
  LOCK_MAINFRAME(id);
  SET_LVALUE(Pw, u);
  UNLOCK_MAINFRAME(id);
  return(0);
}

/* returns on/off status for one channel */
unsigned int
sy527GetChannelOnOff(unsigned int id, unsigned int board,
                      unsigned int chan)
{
  unsigned int u;
  LOCK_MAINFRAME(id);
  GET_LVALUE(Pw, u);
  UNLOCK_MAINFRAME(id);
  return(u);
}








#define SET_LVALUE1(prop_name_m, value_m) \
printf("SET_LVALUE1: id=%d board=%d chan=%d\n",id,board,chan); \
printf("SET_LVALUE1: prop=%d value=%d\n",prop_name_m, value_m); \
  Demand[id]->board[board].channel[chan].lval[prop_name_m] = value_m; \
  Demand[id]->board[board].channel[chan].setflag[prop_name_m] = 1; \
  Demand[id]->board[board].setflag = 1; \
  Demand[id]->setflag = 1


/* sets enable/disable for one channel */
int
sy527SetChannelEnableDisable(unsigned int id, unsigned int board,
                              unsigned int chan, unsigned int u)
{
  LOCK_MAINFRAME(id);
  SET_LVALUE1(PwEn, u);
  UNLOCK_MAINFRAME(id);
  return(0);
}

/* returns enable/disable for one channel */
unsigned int
sy527GetChannelEnableDisable(unsigned int id, unsigned int board,
                              unsigned int chan)
{
  unsigned int u;
  LOCK_MAINFRAME(id);
  GET_LVALUE(PwEn, u);
  UNLOCK_MAINFRAME(id);
  return(u);
}

/* returns status for one channel */
unsigned int
sy527GetChannelStatus(unsigned int id, unsigned int board,
                       unsigned int chan)
{
  unsigned int u;
  LOCK_MAINFRAME(id);
  GET_LVALUE(Status, u);
  UNLOCK_MAINFRAME(id);
  return(u);
}

#else

void
sy527_dummy()
{
  return;
}

#endif
