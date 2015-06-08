
/* tttrans.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* to get NWBOS 
#ifndef VXWORKS
#include <pthread.h>
#endif
#include "circbuf.h"
*/
#include "ttbosio.h"

#include "uthbook.h"
#include "coda.h"
#include "tt.h"

#undef DEBUG

/*int nevent;*/

#undef DBG
#define DBG 0


/* time profiling: 0-OFF, 1-ON */
static int timeprofile = 1;

/* histogram area */
static hstr histi[NHIST];
static int neventh = 0;

/*TEMPORARY !!! extern*/ unsigned long user_flag3; /* defined in coda_component.c */
static unsigned int userflag3size=0, userflag3nbanks=0;

/*****************************************************************************
  Globals; all other functions have to declare them as 'extern'
 *****************************************************************************/
/*int TT_nev = NULL;*/ /* event counter */
extern TTSPtr ttp; /* pointer to the translation table */
TrFun TrFunPtr = NULL; /* pointer to the translation function */
/* global pointers to be used in ROL's */
int *userbuffers[NUSERBUFFERS] = {
                            NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                            NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                            NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                            NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                            NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                            NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                            NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                            NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                            NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                            NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
                                 };

/*****************************************************************************
 Static data definition
 *****************************************************************************/
static char* Version = " VERSION: tttrans.c v1.0 - initial release " ;
static char* CompileTime = DAYTIME;


/*RCST rcst[NUM_SLOTS];*/

#ifdef VXWORKS
tst1()
  {
    int status,i;
    sysClkRateSet(5000);

    printf("start ..\n");fflush(stdout);
    for(i=0; i<10; i++) taskDelay(5000);
    printf(".. done.\n");
  }
#endif



/*****************************************************************************

  Routine             : TT_Prestart

  Parameters          : none

  Discription         : This routine will be called in "Prestart" transition

******************************************************************************/

void
TT_Prestart(int pid)
{
  char *env;
  char histname[16];

  strcpy(histname,"ROL2 RC00");

  if( (env=getenv("PROFILE")) != NULL )
  {
    if(*env == 'T')
    {
      timeprofile = 1;
      printf("tttrans: time profiling is ON\n");
    }
    else
    {
      timeprofile = 0;
      printf("tttrans: time profiling is OFF\n");
    }
  }

  if(timeprofile)
  {
    sprintf((char *)&histname[7],"%02d",ROC);
    uthbook1(histi, 1040+pid, histname, 300, 0, 300);
  }

  return;
}

/******************************************************************************

  Routine             : TT_Go

  Parameters          : none

  Discription         : This routine will be called in "Go" transition

******************************************************************************/

void
TT_Go()
{
  /*nevent = 0;*/
  return;
}

/******************************************************************************

  Routine             : TT_Main

  Parameters          : none

  Discription         : This routine translates one fragment from CODA to
                        BOS format

******************************************************************************/


int 
TT_Main(long *bufin, long *bufout, int pid)
{
  TIMER_VAR;
  unsigned long *fb, *in;     /* input data pointer */
  unsigned long *hit, *iw;    /* output data pointers */
  int i, id, ind, nr, ncol, nrow, lfmt, count;
  long len, len1, lentmp, *jw;
  unsigned long nama;
  char name[9];
  char rcname[8];

  strcpy(rcname,"RC00");

#ifdef VXWORKS
  if(timeprofile)
  {
    TIMER_NORMALIZE;
    TIMER_START;
  }
#endif

  if(bufout[0]!=NWBOS) printf("ERR in TT_Main: bufout[0]=%d (0x%08x)\n",
    bufout[0],bufout);

  jw = bufin + 2;
  bufout[2] = *bufout;           /* How long is free space */
if(bufout[2]!=NWBOS) printf("ERR in TT_Main: bufout[2]=%d (0x%08x)\n",bufout[2],bufout);
  len = 2;                       /* CODA header only - two long CODA words */
  bosinit(&bufout[2],bufout[2]); /* initialize output BOS array */
  iw = (unsigned long *)&bufout[2]; /* for h2bos etc */

  sprintf((char *)&rcname[2],"%02d",ROC); /* BAD:called for every event !!! */
  /*printf("rol2: rcname >%4.4s<\n",rcname);*/


  if(DBG)
  {
    MSG2("\nTT_Main: got event of length %i for ROC # %i\n",bufin[0],ROC);
    MSG1("TT_Main: Ttp structure at 0x%8.8X\n",ttp);
    MSG3("TT_Main: bufout[0]=%i --> processing for ROC#%i(%i)\n",
         bufout[0],ROC,pid);
  }

  /*if(ROC==25||ROC==18||ROC==19||ROC==23||ROC==30||ROC==26)*/
  if(TrFunPtr==NULL)
  {
    len += TT_MakeVmeBank(bufin,bufout,ttp);  /* VME crates for any event type */
  }
  else
  {


    if(((bufin[1] >> 16 ) & 0xff) == 0) /* force_sync (scaler) event */
    {
	  /*
      printf("tttrans: force_sync (scaler) event\n");
	  */
      /*printf("event %d, type 0: bufin[1]=%x\n",nevent,bufin[1]);*/
      /* creates and send statistic bank if event type = 0 */
      /*if(ROC < 12)  len += TT_MakeDCStatBank(bufout, rcst);*/

      /* set input and output data pointers */
      fb = (unsigned long *)&bufin[NHC];

      /* scan input buffer assuming it contains banks created by bosMopen() */
      while((long *)fb < (&bufin[1] + bufin[0]))
      {
        /* get info from bank header (mini-format, 2 header words) */
        id = ((*fb) >> 16) & 0xffff;
        nr = (*fb++) & 0xffff;
        nrow = *fb++;
		/*
		printf("tttrans (scaler event): id=%d nr=%d nrow=%d (0x%08x < 0x%08x + %d)\n",id,nr,nrow,fb,bufin[0],bufin[1]);
		*/
        if(nrow == 0) continue;
        if(bosMgetname(id, name)<0)
        {
          printf("tttrans: ERROR in bosMgetname()\n");
          goto exit;
		}
		else
        {
          ncol = bosMgetncol(name);
          lfmt = bosMgetlfmt(name);
          if(lfmt > 0)
            count = (ncol * nrow * (4/lfmt) + 3) / 4;
          else
            printf("tttrans: ERROR: lfmt=%d\n",lfmt);

          /*printf("id=%d nr=%d count=%x hex (%d decimal) -> name=>%s<\n",
                      id,nr,count,count,name); fflush(stdout);*/
          nama = *((unsigned long *)name);

          if(nama == *((unsigned long *)"PTRN") )
	      {
            fb += count; /* ignore; will be processed in the TT_Main() */
          }
          else /* nothing special for that bank - pass it (TEST FOR B16,B08 !) */
          {
            /*printf("passing bank: nama=>%4.4s< nr=%d nw=%d ncol=%d nrow=%d\n",
		      &nama,nr,count,ncol,nrow);*/
            if( (ind = bosNopen(iw,nama,nr,ncol,nrow)) <= 0 )
            {
              printf("tttrans: ERROR: bosNopen returns %d >%4.4s< nr=%d nw=%d\n",
                          ind,(char *)&nama,nr,count);
              goto exit;
            }
            hit = &iw[ind+1];
            for(i=0; i<count; i++) *hit++ = *fb++;
            len += bosNclose(iw,ind,ncol,nrow);
          }
		}

      } /* end of loop over input buffer */
    }
    else if(bufin[0] > (NHC-1)) /* not empty CODA fragment */
    {
      if((ind = bosMlink(jw,rcname,0)) > 0) /* get raw data bank */
      {
        if(WANNARAW) len += TT_MakeRawBank(&jw[ind],bufout);
        if(!ttp->notrans)
        {
          len1 = TrFunPtr(&jw[ind],bufout,ttp);
#ifdef DEBUG
  printf("len1=%d, bufout->0x%08x 0x%08x 0x%08x 0x%08x 0x%08x ..\n",
    len1,bufout[0],bufout[1],bufout[2],bufout[3],bufout[4]);
#endif
          if (len1 == 2)
          {
            printf("tttrans ERROR: len1=%d\n",len1);
            len1 = 0;
		  }

          len += len1;
        }
      }
    }



  }/*End if there is aconversion function*/

  /*printf("1: ROC=%d\n",ROC);fflush(stdout);*/
  if((ind = bosMlink(jw,"PTRN",ROC)) > 0)
  {
    /*printf("mask=%08x ROC=%d\n",jw[ind+1],ROC);*/
    if((jw[ind+1] & 0x20000000) != 0)
    {
      /*printf("sync event: mask=%x ...\n",jw[ind+1]);*/
      if(((bufin[1] >> 16 ) & 0xff) != 0)
      {
        /*printf("sync event: mask=%x !!!\n",jw[ind+1]);*/
        len += TT_MakeSyncBank(bufout, jw[ind+1]);
      }
    }
  }
  /*printf("2\n");fflush(stdout);*/



  /*boy if (DBG) TT_PrintROCBuffer(len, bufout, TTP);*/

  /* arbitrary bank to increase event size !!! */
  if(user_flag3 > 0)
  {
    userflag3nbanks = user_flag3 / 1000;
    userflag3size = user_flag3 - userflag3nbanks * 1000;
    if(userflag3nbanks > 99) userflag3nbanks = 99;
    for(i=0; i<userflag3nbanks; i++)
    {
      ind = bosNopen(&bufout[2],*((unsigned long *)"RWM "),i,1,userflag3size);
      if(ind <= 0)
      {
        printf("tttrans(RWM bank): on request %d bosNopen returned %d !!!\n",
                userflag3size,ind);
        goto exit;
      }
      len += bosNclose(&bufout[2],ind,1,userflag3size);
    }
    if(len > 7000) printf("ERRRRRRRRRRR: len=%d\n",len);
  }

exit:
#ifdef VXWORKS
  if(timeprofile) TIMER_STOP(90000/*0*/,1040+pid);
/*
if(dTim/25 > 1000) logMsg("%d microsec\n",dTim/25);
*/
#endif


#ifdef VXWORKS
  if(timeprofile)
  {
    neventh ++;
    if(neventh > 100000)
	{
      neventh = 0;
      len += uth2bos1(histi, 1040+pid, iw);
    }
  }
#endif


  return(len);
}

int
TT_MakeDCStatBank(long *bufout, RCST *rcst)
{
  long *iw;
  int ind, i, ncol, nstrwd, nrow, len;

  len = 0;
  iw = &bufout[2];
  nstrwd = sizeof(RCST)/4;
  if((ncol = nstrwd - (MAXSTATWD - ttp->nstatwd)) <= nstrwd)
  {
    /* ttp->statname == "DCST", so 'ncol' must be taken from
     ddl file; right now it is calculated here which is wrong !!! */
    ind = bosNopen(iw,ttp->statname,ttp->roc,ncol,NUM_SLOTS);
    if(ind <= 0)
    {
      printf("tttrans(RCST): on request %d bosNopen returned %d !!!\n",
              NUM_SLOTS,ind);
      return(len);
    }
    nrow = 0;
    /*printf("\n   <<< Statistic >>>\n");*/
    for(i=0; i<NUM_SLOTS; i++)
    {
      if(rcst[i].event > 0)
      {
		/*
        printf("sl=%2d ev=%6d ovfl=%6d",rcst[i].slot,rcst[i].event,rcst[i].overflow);
        printf("  errors->%6d%6d%6d%6d%6d%6d%6d%6d%6d%6d%6d\n",rcst[i].stat[0],rcst[i].stat[1],
               rcst[i].stat[2],rcst[i].stat[3],rcst[i].stat[4],rcst[i].stat[5],rcst[i].stat[6],
               rcst[i].stat[7],rcst[i].stat[8],rcst[i].stat[9],rcst[i].stat[10]);
		*/
        bcopy((char *)&rcst[i].slot, (char *)&iw[ind+nrow*ncol+1], ncol<<2);
        bzero((char *)&rcst[i].slot, ncol<<2);
        nrow++;
      }
    }
    len = bosNclose(iw,ind,ncol,nrow);
    /*printf("\n");*/
  }
  
  return(len);
}


/* makes sync bank */

int
TT_MakeSyncBank(long *bufout, long pattern)
{
  long *iw;
  short *out;
  int ind, i, nrow, len;
  long mask;

  len = 0;
  iw = &bufout[2];

  if(pattern & 0xfffffff) /* we have some bad slots ... */
  {
	/*
    printf("bad slots pattern: 0x%08x\n",pattern);
	*/
    if((ind = bosNopen(iw,*((unsigned long *)"SYNC"),ttp->roc,3,26)) <= 0)
    {
      printf("TT_MakeSyncBank: ERROR1: bosNopen returned %d !!!\n",ind);
      return(len);
    }
    out = (short *)&iw[ind+1];
    nrow = 0;
    mask = 1;
    for(i=0; i<26; i++)
    {
      if(pattern & mask)
      {
        out[0] = i;
        out[1] = 0; /* temporary */
        out[2] = 1;
        out += 3;
        nrow ++;
      }
      mask = mask << 1;
    }
    len += bosNclose(iw,ind,3,nrow);
  }
  else  /* if everything fine, creates +SYN bank */
  {
    if((ind = bosNopen(iw,*((unsigned long *)"+SYN"),ttp->roc,3,1)) <= 0)
    {
      printf("TT_MakeSyncBank: ERROR2: bosNopen returned %d !!!\n",ind);
      return(len);
    }
    out = (short *)&iw[ind+1];
    out[0] = ttp->roc;
    out[1] = 0;
    out[2] = 0;
    len += bosNclose(iw,ind,3,1);
  }

  /*printf("TT_MakeSyncBank: return len = %d\n",len);*/

  return(len);
}



/****************************/
/* RAW DATA PRINT FUNCTIONS */
/****************************/

void 
TT_PrintHead(int *head,TTSPtr ttp)
{
  INDPtr fb=(INDPtr)head;    /* input data pointer */
  char *type;
  int count=0;

  if(SLTYPE[SLOT] == 1)
  {
	type = "1881";
	count= ((ADCHeadPtr)head)->count & 0x3F;
  }
  else if(SLTYPE[SLOT] == 2)
  {
	type = "1877";
	count= ((MTDCHeadPtr)head)->count;
  }
  else
  {
    type = " !!! UNKNOWN !!! ";
  }
  printf("  %s HEADER slot=%3i lng %3d        ",type,SLOT,count);
}

void 
TT_Print1881(int *data,TTSPtr ttp)
{
  ADCPtr adc=(ADCPtr)data;
  INDPtr fb=(INDPtr)data;

  printf("  1881 %2i:%2i=%5d -> %4.4s#%d ID=%4.4X:%1d",
	adc->slot,adc->channel,adc->data,&NAME[SLOT][CHANNEL],NR[SLOT][CHANNEL],
	ID[SLOT][CHANNEL],PLACE[SLOT][CHANNEL]);
}

void 
TT_Print1872(int *data,TTSPtr ttp)
{
  TDCPtr tdc=(TDCPtr)data;
  INDPtr fb=(INDPtr)data;

  printf("  1872 %2i:%2i=%5d -> %4.4s#%d ID=%4.4X:%1d",
	tdc->slot,tdc->channel,tdc->data,&NAME[SLOT][CHANNEL],NR[SLOT][CHANNEL],
	ID[SLOT][CHANNEL],PLACE[SLOT][CHANNEL]);
}

void 
TT_Print1877(int *data,TTSPtr ttp)
{
  MTDCPtr fb=(MTDCPtr)data;

  printf("  1877 %2i:%2i=%5d -> %4.4s#%d ID=%4.4X:%1d",
	SLOT,CHANNEL,fb->data,&NAME[SLOT][CHANNEL],NR[SLOT][CHANNEL],
	ttp->id2[SLOT][0][CHANNEL],PLACE[SLOT][CHANNEL]);
}

void
TT_PrintRawBank(int *p, int lng, TTSPtr ttp)
{
  int *end, slotold=-1, rowitems,j;
  INDPtr fb;    /* input data pointer */
  int PageWidth = 90;

  rowitems = PageWidth / (37) ;
  /* printf("PageWidth=%d rowitems=%d\n",PageWidth,rowitems) ; */

  for (j=0; j < rowitems; j++) {
	printf("  Type sl:ch=data  -> BOSBank   ID:ind");
  }
  puts ("");
  for(end=p+lng; p<end; )
  {
	for(j=0; j < rowitems; j++)
    {
	  fb = (INDPtr)p;
	  if (SLOT != slotold && SLTYPE[SLOT] > 0)  TT_PrintHead((int *)fb,ttp); 
	  else if (SLTYPE[SLOT] == 0)                TT_Print1872((int *)fb,ttp);
	  else if (SLTYPE[SLOT] == 1)                TT_Print1881((int *)fb,ttp);
	  else if (SLTYPE[SLOT] == 2)                TT_Print1877((int *)fb,ttp);
	  else  printf("Unknown type of board %d in slot %d\n",SLTYPE[SLOT],SLOT);
	  slotold = SLOT;
	  p++;
	  if (p>=end) break;	  
	}
	puts ("");
  }	
  puts ("");
  
}


