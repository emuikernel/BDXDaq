
/* histrol2.c - second readout list: creates histograms for test setup
 boards testing procedure */

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



#define INIT_NAME histrol2__init


#include "rol.h"

/* event readout */
#include "EVENT_source.h"

/************************/
/************************/

#include "coda.h"
#include "tt.h"

extern TTSPtr ttp; /* pointer to TTS structures for current roc */
extern TrFun TrFunPtr; /* pointer to the translation function */


/* testsetup stuff*/
#include "uthbook.h"
#include "fbutil.h"
/* # of events to wait until histograms injection to the data stream */
#define HREPORT (300+slot*10)
static int nreport;
extern struct hardmap map;
int TT_nev1 = NULL;
static hstr histi[NHIST]; /* histogram area */
/* testsetup stuff*/



/*extern int TT_nev;*/

long decrement;     /* local decrement counter */
/*extern long nevent;*/ /* local event counter */
long mynev;


static void
__download()
{

#ifdef POLLING_MODE
   rol->poll = 1;
#endif

  printf("rol2: downloading DDL table ...\n");
  clonbanks();
  printf("rol2: ... done.\n");

  printf("INFO: User Download 2 Executed\n");

  return;
}

#define SLOT1 7
#define SLOTN 17

static void
__prestart()
{
  unsigned long jj, adc_id, sl;
  char *env;

  /* testsetup stuff */
  int i, slot;
  static char *title = "sl00ch00";
  /* book histograms */
  for(slot=SLOT1; slot<=SLOTN; slot++)
  {
    if(rol->pid == 0)
	{
      for(i=0; i<32; i++)
      {
        printf("booking histogram %d\n",(slot-SLOT1)*100+(i+1));
        sprintf((char *)title,"sl%02dch%02d",slot,i);
        uthbook1(histi, (slot-SLOT1)*100+(i+1), title, 4000, 0, 4000/*8200*/);
      }
	}
    else if(map.map_adc[slot]==1||map.map_tdc72[slot]==1||map.map_tdc75[slot]==1)
    {
      for(i=0; i<64; i++)
      {
        sprintf((char *)title,"sl%02dch%02d",slot,i);
        uthbook1(histi, (slot-SLOT1)*100+(i+1), title, 1000, 0, 4000/*8200*/);
      }
    }
    else if(map.map_tdc77[slot]==1||map.map_tdc77S[slot]==1)
    {
      for(i=0; i<96; i++)
      {
        sprintf((char *)title,"sl%02dch%02d",slot,i);
        uthbook1(histi, (slot-SLOT1)*100+(i+1), title, 1000, 0, 4000/*8200*/);
      }
    }
  }

  nreport = 0;
  /* testsetup stuff */


  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  printf("INFO: Entering User Prestart 2\n");

  tttest("\ntrans_rol2:");
  decrement = 0;

  TT_Prestart(rol->pid);

  printf("RAW=%d  PROFILE=%d  NOTRANS=%d\n",ttp->wannaraw,ttp->profile,ttp->notrans);
  printf("MAX_EVENT_LENGTH = %d NWBOS = %d\n",MAX_EVENT_LENGTH,NWBOS);




  /* set pointers to the translation routines and translation flags */
  switch(rol->pid)
  {
    case 0:
       TrFunPtr = TT_TranslateVMEBank; /* croctest2 */
       ttp->wannaraw=1;
       ttp->notrans=1; 
       ttp->profile=1;
    default:
	  printf("ERROR: illegal roc id = %d\n",rol->pid);
  }


  /* init trig source EVENT */
  EVENT_INIT;

  /* Register a sync trigger source (up to 32 sources) */
  CTRIGRSS(EVENT, 1, davetrig, davetrig_done); /* second arg=1 - what is that ? */

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
davetrig(int a, int b)
{
  int ii, len;

  /* testsetup stuff */
  unsigned long *fb, *in, *end;     /* input data pointer */
  unsigned long *hit, *iw;    /* output data pointers */
  int i, slot, old_slot, chan, id, ind, nr, ncol, nrow, lfmt, count;
  long len1, *jw, *bufout, *bufin, data, ref;
  unsigned long nama;
  char name[9];
  static char *rcname = "RC00";
  /* testsetup stuff */



		/* trans */
        rol->dabufp[0] = NWBOS;

        TT_nev1 = *(rol->nevents);
        bufin = rol->dabufpi;
        bufout = rol->dabufp;
        jw = rol->dabufpi + 2;
        bufout[2] = *bufout; /* How long is free space */
if(bufout[2]!=NWBOS)
  printf("ERR in histrol2: bufout[2]=%d (0x%08x)\n",bufout[2],bufout);
        len = 2; /* CODA header only - two long CODA words */
        bosinit(&bufout[2],bufout[2]); /* initialize output BOS array */
        iw = (unsigned long *)&bufout[2]; /* for h2bos etc */

        if(((bufin[1] >> 16 ) & 0xff) == 0) /* force_sync (scaler) event */
        {
          ;
        }
		else if(rol->pid==0) /*croctest2, v1190/v1290 TDC boards*/
        {





          if(nreport++ >= 560) nreport = 1;

          /* fill histograms */
          bufin += 4; /* first data word */
          old_slot = -1;
          end = bufin + bufin[-1];
          /*printf("in=0x%08x end=0x%08x len=%d\n",bufin,end,bufin[-1]);*/
          ref = 0;
          while(bufin < end)
          {
            slot = ((*bufin)>>27)&0x1F;
            if(slot != old_slot) /* new slot */
            {
              if(slot==0)
              {
                /*printf("skip header 0x%08x (slot=%d, count=%d)\n",
                  bufin[0],bufin[0]&0x1F,(bufin[0]>>5)&0x3FFFFF);*/
                ref = 0; /*COMMENT OUT IF WANT TO USE REF FROM THE FIRST BOARD ONLY*/
                bufin ++;
                continue; /* skip header word */
              }
            }
            if(slot != old_slot) /* new slot */
            {
              old_slot = slot;
            }

            data = -1;
            chan = ((*bufin)>>21)&0x1F;
            data = (*bufin)&0x1FFFFF;
            if(chan == 0 && ref == 0)
            {
              ref = data;
            }
            data = data - ref + 500;
	        /*printf("1: slot=%d chan=%d data=%d\n",slot,chan,data);*/
            if(data >= 0)
            {
              if(slot>=SLOT1 && slot<=SLOTN)
                uthfill(histi, (slot-SLOT1)*100+(chan+1), data, 0, 1);
			}
            bufin ++;
          }

          for(slot=SLOT1; slot<=SLOTN; slot++)
		  {
            if(nreport%(HREPORT+0)==0)
            {
              /*printf("histogram %d for slot %d, event %d\n",
				(slot-SLOT1)*100+(i+1),slot,nreport);*/
              /*if(slot>=(SLOT1+1))*/ for(i= 0; i<8; i++)
                len += uth2bos1(histi, (slot-SLOT1)*100+(i+1), iw);
            }
            else if(nreport%(HREPORT+1)==0)
            {
              /*printf("histogram for slot %d, event %d\n",slot,nreport);*/
              /*if(slot>=(SLOT1+1))*/ for(i=8; i<16; i++)
                len += uth2bos1(histi, (slot-SLOT1)*100+(i+1), iw);
            }
            else if(nreport%(HREPORT+2)==0)
            {
              /*if(slot>=(SLOT1+1))*/ for(i=16; i<24; i++)
                len += uth2bos1(histi, (slot-SLOT1)*100+(i+1), iw);
            }
            else if(nreport%(HREPORT+3)==0)
            {
              /*if(slot>=(SLOT1+1))*/ for(i=24; i<32; i++)
                len += uth2bos1(histi, (slot-SLOT1)*100+(i+1), iw);
            }
          }








        }
        else
        {
          if(nreport++ >= 560) nreport = 1;

          /* fill histograms */
          bufin += 4; /* first data word */
          old_slot = -1;
          end = bufin + bufin[-1];
          /*printf("in=0x%08x end=0x%08x len=%d\n",bufin,end,bufin[-1]);*/
          while(bufin < end)
          {
            slot = ((*bufin)>>27)&0x1F;
            if(slot != old_slot) /* new slot */
            {
              old_slot = slot;
              if(map.map_adc[slot]==1||map.map_tdc77[slot]==1||
                 map.map_tdc77S[slot]==1)
              {
                bufin ++;
                continue; /* skip header word */
              }
            }

            data = -1;
            if(map.map_adc[slot]==1||map.map_tdc77[slot]==1||
               map.map_tdc77S[slot]==1)
            {
              chan = ((*bufin)>>17)&0x7F;
              data = (*bufin)&0xFFFF;
	          /*printf("1: slot=%d chan=%d data=%d\n",slot,chan,data);*/
            }
            else if(map.map_tdc72[slot]==1||map.map_tdc75[slot]==1)
            {
              chan = ((*bufin)>>16)&0x3F;
              data = (*bufin)&0xFFF;
	          /*printf("2: slot=%d chan=%d data=%d\n",slot,chan,data);*/
            }
            else
            {
              printf("tthist: ERROR: slot=%d\n",slot);
            }
            if(data >= 0) uthfill(histi, (slot-SLOT1)*100+(chan+1), data, 0, 1);
            bufin ++;
          }

          for(slot=SLOT1; slot<=SLOTN; slot++)
          {
            if(map.map_adc[slot]==1||map.map_tdc72[slot]==1||
               map.map_tdc75[slot]==1||map.map_tdc77[slot]==1||
               map.map_tdc77S[slot]==1)
            {
              if(nreport%(HREPORT+0)==0)
              {
                /*printf("histogram for slot %d, event %d\n",slot,nreport);*/
                for(i= 0; i<16; i++)
                  len += uth2bos1(histi, (slot-SLOT1)*100+(i+1), iw);
              }
              else if(nreport%(HREPORT+1)==0)
              {
                /*printf("histogram for slot %d, event %d\n",slot,nreport);*/
                for(i=16; i<32; i++)
                  len += uth2bos1(histi, (slot-SLOT1)*100+(i+1), iw);
              }
              else if(nreport%(HREPORT+2)==0)
              {
                /*printf("histogram for slot %d, event %d\n",slot,nreport);*/
                for(i=32; i<48; i++)
                  len += uth2bos1(histi, (slot-SLOT1)*100+(i+1), iw);
              }
              else if(nreport%(HREPORT+3)==0)
              {
                /*printf("histogram for slot %d, event %d\n",slot,nreport);*/
                for(i=48; i<64; i++)
                  len += uth2bos1(histi, (slot-SLOT1)*100+(i+1), iw);
              }
            }

            if(map.map_tdc77[slot]==1||map.map_tdc77S[slot]==1)
            {
              if(nreport%(HREPORT+4)==0)
              {
                /*printf("histogram for slot %d, event %d\n",slot,nreport);*/
                for(i=64; i<80; i++)
                  len += uth2bos1(histi, (slot-SLOT1)*100+(i+1), iw);
              }
              else if(nreport%(HREPORT+5)==0)
              {
                /*printf("histogram for slot %d, event %d\n",slot,nreport);*/
                for(i=80; i<96; i++)
                  len += uth2bos1(histi, (slot-SLOT1)*100+(i+1), iw);
              }
            }
          }
        }





        /*???TT_nev1 = input_event__->nevent;*/
        if(len < 2)
        {
          printf("ERROR in list2: len=%d\n",len);
          len=2;
        }

        if(len > NWBOS)
        {
          printf("Error in list2: Event length=%d (longwords) is too big\n",
                  len);
        }

        rol->dabufp[0] = len - 1;
        rol->dabufp[1] = rol->dabufpi[1];

		/*?????????????????????????????????
        rol->dabufp   += len;
		*/







  return;
}
  
void
davetrig_done()
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
