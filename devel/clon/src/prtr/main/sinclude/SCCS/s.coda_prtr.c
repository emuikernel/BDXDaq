h47738
s 00002/00002/02524
d D 1.9 09/10/20 20:02:18 boiarino 9 8
c *** empty log message ***
e
s 00004/00002/02522
d D 1.8 09/10/20 11:50:22 boiarino 8 7
c comment out raster alarm
c 
e
s 00000/00000/02524
d D 1.7 09/09/09 09:33:02 boiarino 7 6
c *** empty log message ***
e
s 00001/00001/02523
d D 1.6 09/02/09 17:37:49 boiarino 6 5
c *** empty log message ***
e
s 00005/00000/02519
d D 1.5 09/02/09 17:34:50 boiarino 5 4
c *** empty log message ***
e
s 00006/00000/02513
d D 1.4 09/02/09 17:24:42 boiarino 4 3
c *** empty log message ***
e
s 00060/00007/02453
d D 1.3 09/02/09 17:06:10 boiarino 3 2
c add FBPM-related error message
c 
e
s 00021/00007/02439
d D 1.2 07/11/04 10:41:17 boiarino 2 1
c adjustment for photon run g9a
c switch prlib to single thread - seg fauling (trlib also single
c threading just in case, need check)
c 
e
s 02446/00000/00000
d D 1.1 07/06/28 23:26:31 boiarino 1 0
c date and time created 07/06/28 23:26:31 by boiarino
e
u
U
f e 0
t
T
I 1
/*
   coda_tr.c - Online L3LIB/PRLIB/TRLIB/ERLIB/DMLIB wrapper

   Author:      Sergey Boyarinov, boiarino@jlab.org

   Created:     Sep 23, 2000
   Last update: Jun 24, 2005

   ifdefs: L3LIB - for Level 3 Trigger
           PRLIB - for Track Reconstruction
           TRLIB - for Track Reconstruction
           ERLIB - for Event Recorder
           DMLIB - for Data Monitor
           MPROG - for multiprogram version (default is multithread version)

*/

#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <sys/times.h>
#include <limits.h>
#include <sys/statvfs.h>

#include "uthbook.h"
#include "prlib.h"
#include "dclib.h"
#include "bosio.h"
#include "et.h"
#include "etbosio.h"
#include "etmacros.h"
#ifdef DMLIB
#include "dmlib.h"
#endif

#ifdef SSIPC
#include "ssipc.h"
#endif

#ifdef L3LIB
/* delayed reporting */
#ifdef DELREP

#include "helicity.c"

#define FORCE_RECOVERY \
  forceRecovery(); \
  quad = -1; \
  strob = -1; \
  helicity = -1; \
  quad_old = -1; \
  strob_old = -1; \
  helicity_old = -1; \
  quad_old2 = -1; \
  strob_old2 = -1; \
  helicity_old2 = -1; \
  remember_helicity[0]=remember_helicity[1]=remember_helicity[2]=-1; \
  done = -1; /* will be change to '0' in the beginning of quartet and to '1' when prediction is ready */ \
  offset = 0

/*
#define DRDEBUG
*/

#endif
#endif

I 3
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define ABS(x)   ((x) < 0 ? -(x) : (x))
E 3

D 3

E 3
#ifdef PRLIB
typedef struct prevent
{
  UThistf *histf; /* histograms */
  int     ntrk;
  PRTRACK *trk;
} PRevent;
#endif

typedef struct thread
{
#ifdef PRLIB
  PRevent *evt;
#endif
#ifdef TRLIB
  TRevent *evt;
#endif
  struct timespec   start;
  struct timespec   end;
} THREAD;

#ifndef MPROG
static MM *mm;
#else

#ifdef L3LIB
char *map_file_name = "/tmp/coda_l3.mapfile";
#endif
#ifdef PRLIB
char *map_file_name = "/tmp/coda_pr.mapfile";
#endif
#ifdef TRLIB
char *map_file_name = "/tmp/coda_tr.mapfile";
#endif
#ifdef ERLIB
char *map_file_name = "/tmp/coda_er.mapfile";
#endif
#ifdef DMLIB
char *map_file_name = "/tmp/coda_dm.mapfile";
#endif

#endif

/* signal handler prototype */
static void *signal_thread (void *arg);

static int force_exit = 0;

/* input args */

char  project[1000];
char  session[1000];
char  unique_name[1000];
char  et_file_name[1000];

#ifdef L3LIB
int chunk = 100;
int nthreads=1;
char *et_station_name = "LEVEL3";
#endif
#ifdef PRLIB
int chunk = 100;
D 2
int nthreads=5;
E 2
I 2
int nthreads=1/*5*/;
E 2
char *et_station_name = "PRLIB";
#endif
#ifdef TRLIB
int chunk = 100;
D 2
int nthreads=3;
E 2
I 2
int nthreads=1/*3*/;
E 2
char *et_station_name = "TRLIB";
#endif
#ifdef ERLIB
int chunk = 100;
int nthreads=1;
char *et_station_name = "ERLIB";
#endif
#ifdef DMLIB
int chunk = 100;
int nthreads=1;
char *et_station_name = "DMLIB";
#endif

ETVARS1;

static int ntimeouts; /* the number of sequential ET timeouts before getting events */

#define MYGET \
  if(mm->nid > 0) \
  { \
    /* get 'myid' from the stack */ \
    myid = mm->idstack[--mm->nid]; \
    /*printf("[%1d] get myid=%d from idstack[%d]\n",threadid,myid,mm->nid);*/ \
    /* get the chunk of events; parameters go to ... */ \
    mm->attach1[myid] = attach; \
    ETGETEVENTS(mm->attach1[myid],mm->pe1[myid],mm->numread1[myid]); \
    mm->numread2[myid] = mm->numread3[myid] = 0; \
    mm->ready[myid] = 0; \
    /*if(mm->nfifo > 20) printf("[%1d] nfifo=%d numread=%d\n",threadid,mm->nfifo,mm->numread1[myid]);*/ \
    /* put 'myid' into fifo */ \
    if(mm->nfifo < NFIFOMAX) \
    { \
      mm->nfifo ++; \
      for(i=mm->nfifo-1; i>0; i--) mm->fifo[i] = mm->fifo[i-1]; \
      mm->fifo[0] = myid; \
    } \
    else \
    { \
      printf("[%1d] ERROR: no place in fifo, nfifo=%d\n",threadid,mm->nfifo); \
      printf("[%1d] fifo-> %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d\n",threadid, \
        mm->fifo[0],mm->fifo[1],mm->fifo[2],mm->fifo[3],mm->fifo[4], \
        mm->fifo[5],mm->fifo[6],mm->fifo[7],mm->fifo[8],mm->fifo[9],mm->fifo[10],mm->fifo[11]); \
      exit(0); \
    } \
  } \
  else \
  { \
    printf("[%1d] ERROR: no myid's in idstack, nid=%d\n",threadid,mm->nid); \
    exit(0); \
  }


void *
l3_thread(void *arg)
{
#ifdef L3LIB
  int cand, ind_tgbi;
  int oldevnum;

#ifdef L3NOTHREADS
  UThistf *histf;
  float l1ungated[12], l1ungated_old[12], l1ungated_diff[12];
  float l1gated[12], l1gated_old[12], l1gated_diff[12];
  float l2gated[6], l2gated_old[6], l2gated_diff[6];
  float l2fpsc[4], l2fpsc_old[4], l2fpsc_diff[4];
  float clock, clock_old, clock_diff;
#endif

#ifdef DELREP
  int ind10, ind11, ind12, ind13, ncol, nrow;
  unsigned int hel,count1,str,strob,helicity,quad;
  unsigned int strob_old,helicity_old,quad_old,strob_old2,helicity_old2,quad_old2;
  unsigned int helicity1, strob1, quad1, offset, quadextr[4];
  int done;
  int present_reading;
  int skip = 0;
  int final_helicity;
  int remember_helicity[3];

  int predicted_reading;      /* prediction of present reading */
  int present_helicity;       /* present helicity (using prediction) */
  unsigned int iseed;         /* value of iseed for present_helicity */
  unsigned int iseed_earlier; /* iseed for predicted_reading */

  /* following for 1 flip correction */
  unsigned int offset1;
  int pred_read;
  int pres_heli;
  int tmp0, temp0;
#endif
#endif

#ifdef PRLIB
  PRTRACK *track;
  PRevent *evt;
  float threshold[3];
  const int option[3] = {0,0,0};
I 3

  int ifbpmevent=0, ixmin=10000, ixmax=0, iymin=10000, iymax=0; /*for FBPM*/
E 3
#endif

#ifdef TRLIB
  TRevent *evt;
  /* for helicity */
  float content11[400], content12[400], content13[400];
  float content21[400], content22[400], content23[400];
#endif

#ifdef ERLIB
  FILE *fn;
  BOSIO *fd = NULL;
  int stat3, stat4, stat5;
  int runNumber, split, record_length;
  int changepartition = 0, partition = 0;
  char *dir = "/mnt/raid8/dlt/";
  char filename[1000];
  char *dirfn, RunNumFile[1000];
  char current_file[1000], temp[1000];
#endif

  ETSYS *sysptr;
  et_att_id  attach;
  struct timespec timeout;
  int lock, keep_event;
D 3
  int nevents1_chunk,nevents2_chunk; /* local event counters for one chunk */
E 3
I 3
  int nevents1_chunk, nevents2_chunk; /* local event counters for one chunk */
E 3

  int i, threadid, myid, hisid, mynev, mynev0, ififo, tmp;
  float fnev;
  const int forever = 1;
  int numread, status, j, jj, ind, *jw, len, ntrk, size;

#ifdef MPROG
  /* map */
  MM *mm;
  tmp = mm_mem_create(map_file_name,sizeof(MM),(void **) &mm, &size);
  if(tmp == ET_ERROR_EXISTS)
  {
    mm = (MM *) mm_mem_attach(map_file_name,sizeof(MM));
  }
  else
  {
    printf("mm_mem_create returns %d\n",tmp);
    exit(0);
  }
#endif


#ifdef L3LIB

#ifdef DELREP
  FORCE_RECOVERY;
#endif

#ifdef L3NOTHREADS
  histf = (UThistf *) malloc(NHIST*sizeof(UThistf));
  printf("coda_l3: histf ->%08x\n",histf);  /* to be filled by 'coda_l3' */

  uthbook1(histf, 32, "EVELB 1L STIAGNU DET", 12, 0., 12.);
  uthbook1(histf, 33, "EVELB 1L STIETAG   D", 12, 0., 12.);
  uthbook1(histf, 34, "EVELB 2L STIETAG   D", 6, 0., 6.);
  uthbook1(histf, 35, "F 2L LIASSAPATS C TRRAEL", 4, 0., 4.);
  /*
  uthbook1(histf, 36, " ", 6, 0., 6.);
  uthbook1(histf, 37, " ", 6, 0., 6.);
  uthbook1(histf, 38, " ", 6, 0., 6.);
  uthbook1(histf, 39, " ", 6, 0., 6.);
  */
#endif
#endif

  /* space allocated in main() */

#ifdef PRLIB
  printf("0: evt->%08x\n",((THREAD *)arg)->evt); fflush(stdout);
  evt = (PRevent *)((THREAD *)arg)->evt;
  printf("1: evt->%08x\n",evt); fflush(stdout);

  printf("0: track->%08x\n",((THREAD *)arg)->evt->trk); fflush(stdout);
  track = (PRTRACK *)((THREAD *)arg)->evt->trk;
  printf("1: track->%08x\n",track); fflush(stdout);

  dchbook(evt->histf);
#endif
#ifdef TRLIB
  printf("0: evt->%08x\n",((THREAD *)arg)->evt); fflush(stdout);
  evt = (TRevent *)((THREAD *)arg)->evt;
  printf("1: evt->%08x\n",evt); fflush(stdout);
  
  dchbook(evt->histf);
#endif
#ifdef ERLIB
  if((dirfn = (char *)getenv("CLON_PARMS")) == NULL)
  {
    printf("coda_er: CLON_PARMS not defined !!!\n");
    exit(0);
  }
  sprintf(RunNumFile,"%s/ERLIB/runNumber",dirfn);
  printf("coda_er: reading run number file >%s<\n",RunNumFile);  
  if( (fn = fopen(RunNumFile,"r")) == NULL )
  {
    printf("coda_er: ERROR : Can't open run number file %s\n",RunNumFile);
    exit(0);
  }
  else
  {
    fscanf(fn,"%d",&runNumber);
    printf("coda_er: Open run number file %s, read run number %d\n",
      RunNumFile,runNumber);
    fclose(fn);
  }
#endif

  /* set 'threadid' */
  threadid = ++ mm->nthread;
  printf("[%1d] set 'threadid'\n",threadid);

  ETATTACH(attach); /* using local 'attach' */

  /* lock input */
  printf("[%1d] 0: nfifo=%d fifo=%d %d %d %d\n",
     threadid,mm->nfifo,mm->fifo[0],mm->fifo[1],mm->fifo[2],mm->fifo[3]);

  pthread_mutex_lock(&mm->mutex_fifo);
  printf("[%1d] 1: nfifo=%d fifo=%d %d %d %d\n",
     threadid,mm->nfifo,mm->fifo[0],mm->fifo[1],mm->fifo[2],mm->fifo[3]);

  MYGET;
  mm->nevents += mm->numread1[myid];
  mm->Nevents += mm->numread1[myid];
  mm->time0 = time(NULL);

  pthread_mutex_unlock(&mm->mutex_fifo);

  while(forever)
  {
    if(mm->nfifo>=NFIFOMAX || mm->nid>=NIDMAX || mm->nfifo<0 || mm->nid<0)
    {
      printf("ERROR: [%1d] nfifo=%d nid=%d\n",threadid,mm->nfifo,mm->nid);
      fflush(stdout);
    }
    nevents1_chunk = nevents2_chunk = 0;
#ifdef ERLIB
    stat4 = stat5 = 0;
#endif
    /* loop over all events in chunk */
    for(jj=0; jj<mm->numread1[myid]; jj++)
    {
      keep_event = 1; /* keep event by default */
      ETGETDATA(mm->pe1[myid]);
	  /*printf("local no. %d\n",jj);*/

      if((ind = etNlink(jw,"HEAD",0)) != 0)
      {
		/*printf("event no. %d\n",jw[ind+2]);*/

        if(jw[ind+4] == 10) /* scaler event */
        {

#ifdef L3NOTHREADS
          /* trigger bits etc histos */
          ind = etNlink(jw,"TRGS",0);
          if(ind > 0)
          {
            float xx;
            int i, nrow, ncol;
            unsigned int *jw32;

	        /*
            nrow = etNrow(jw,ind);
            ncol = etNcol(jw,ind);
	        */

            jw32 = (unsigned int *)&jw[ind];

            /* get scalers */
            for(i=0; i<12; i++) l1ungated[i] = ((float)jw32[48+i]);
            for(i=0; i<12; i++) l1gated[i]   = ((float)jw32[80+i]);
            for(i=0; i<6; i++)  l2gated[i]   = ((float)jw32[72+i]);
            for(i=0; i<4; i++)  l2fpsc[i]    = ((float)jw32[64+i]);
            clock                            = ((float)jw32[30]);

            /* calculate differences */
            for(i=0; i<12; i++) l1ungated_diff[i] = l1ungated[i] - l1ungated_old[i];
            for(i=0; i<12; i++) l1gated_diff[i]   = l1gated[i] - l1gated_old[i];
            for(i=0; i<6; i++)  l2gated_diff[i]   = l2gated[i] - l2gated_old[i];
            for(i=0; i<4; i++)  l2fpsc_diff[i]    = l2fpsc[i] - l2fpsc_old[i];
            clock_diff                            = clock - clock_old;

            /* remember values for next entry */
            for(i=0; i<12; i++) l1ungated_old[i] = l1ungated[i];
            for(i=0; i<12; i++) l1gated_old[i]   = l1gated[i];
            for(i=0; i<6; i++)  l2gated_old[i]   = l2gated[i];
            for(i=0; i<4; i++)  l2fpsc_old[i]    = l2fpsc[i];
            clock_old                            = clock;

	        printf("l1gated: %f %f %f\n",l1gated[0],l1gated[1],l1gated[2]);
			

printf("fill 32\n");
            if(clock_diff > 0.0)
            {

              for(i=0; i<12; i++)
		      {
                xx = ((float)i)+0.5;
                uthfill(histf, 32, xx, 0.0, (l1ungated_diff[i]*100000.0)/clock_diff);
                uthfill(histf, 33, xx, 0.0, (l1gated_diff[i]*100000.0)/clock_diff);
              }

              for(i=0; i<6; i++)
		      {
                xx = ((float)i)+0.5;
                uthfill(histf, 34, xx, 0.0, (l2gated_diff[i]*100000.0)/clock_diff);
              }

              for(i=0; i<4; i++)
		      {
                xx = ((float)i)+0.5;
                uthfill(histf, 35, xx, 0.0, (l2fpsc_diff[i]*100000.0)/clock_diff);
              }
            }
			
	      }
#endif
          ;
        }
        else if(jw[ind+4] < 10) /* physics event */
        {
          if(mynev == 0)
          {
            mynev ++;
            mynev0 = jw[ind+2] - 1;
          }
          else
          {
            mynev ++;
            /*printf("mynev=%d mynev0=%d event#=%d\n",mynev,mynev0,jw[ind+2]);*/
            if((jw[ind+2]-mynev0)!=0) fnev=((float)mynev/((float)(jw[ind+2]-mynev0)))*100.;
          }

          /*printf("===> got %d(%6.2f)%% events\n",mynev,fnev);*/
          if(jw[ind+6] == 17)
          {
            printf("Prestart ..\n"); fflush(stdout);
            mynev = 0;
            mm->nevents = mm->nevents1 = mm->nevents2 = 0;
            mm->Nevents = mm->Nevents1 = mm->Nevents2 = 0;
            for(i=0; i<8; i++)
            {
              mm->Count[i] = 0;
              mm->Ratio[i] = -1.0;
            }
I 3
#ifdef PRLIB
            /*for FBPM*/
            ifbpmevent = 0;
            ixmin = 10000;
            ixmax = 0;
            iymin = 10000;
            iymax = 0;
I 4
D 8
            /* always cleanup fbpm-related alarm */
E 8
I 8
            /* always cleanup fbpm-related alarm 
E 8
            system("error_msg coda_pr clonalarm coda_pr online 0 INFO 58 \"Raster Ok\"");
I 8
			*/
E 8
E 4
#endif
E 3
#ifdef TRLIB
			
            dcstatclean(&mm->trstat);
            dcstatclean(&evt->stat);

            for(i=0; i<200; i++)
            {
              content11[i] = 0;
              content12[i] = 0;
              content13[i] = 0;
              content21[i] = 0;
              content22[i] = 0;
              content23[i] = 0;
			}
			
#endif
#ifdef L3LIB

#ifdef L3NOTHREADS
            for(i=0; i<12; i++)
			{
              l1ungated_old[i] = 0.0;
              l1gated_old[i] = 0.0;
			}
            for(i=0; i<6; i++)
			{
              l2gated_old[i] = 0.0;
			}
            for(i=0; i<4; i++)
			{
              l2fpsc_old[i] = 0.0;
			}
			clock_old = 0.0;
#endif

            /* event order check */
            if((oldevnum+1) != jw[ind+2])
            {
              printf("l3 error: event no %d, old no %d\n",jw[ind+2],oldevnum);
              printf("prestart: event type = %d\n",jw[ind+4]); fflush(stdout);
            }
            oldevnum = jw[ind+2];
#endif
          }
          else if(jw[ind+6] == 18)
          {
            printf("Go ..\n"); fflush(stdout);
#ifdef L3LIB

goto skiponclon101;
            l3_reset();
skiponclon101:

            /* event order check */
            if((oldevnum+1) != jw[ind+2])
            {
              printf("l3 error: event no %d, old no %d\n",jw[ind+2],oldevnum);
              printf("go: event type = %d\n",jw[ind+4]); fflush(stdout);
            }
            oldevnum = jw[ind+2];
#endif
          }
          else if(jw[ind+6] == 20)
          {
            printf("End ..\n"); fflush(stdout);
I 4
#ifdef PRLIB
D 8
            /* always cleanup fbpm-related alarm */
E 8
I 8
            /* always cleanup fbpm-related alarm
E 8
            system("error_msg coda_pr clonalarm coda_pr online 0 INFO 58 \"Raster Ok\"");
I 8
			*/
E 8
#endif
E 4
#ifdef L3LIB
            oldevnum = 0;
#endif
          }
#ifdef L3LIB
          else
          {
            /* events order check */
            if((oldevnum+1) != jw[ind+2])
            {
              printf("l3 error: event no %d, old no %d\n",jw[ind+2],oldevnum);
              printf("event: event type = %d (coda type = %d)\n",jw[ind+4],jw[ind+6]); fflush(stdout);
            }
            oldevnum = jw[ind+2];
          }
#endif

          if(mm->runnum != jw[ind+1])
          {
            pthread_mutex_lock(&mm->newmap);
            if(mm->runnum != jw[ind+1]) /* check if it is done already by another thread */
            {
              printf("[%1d] current run number = %d\n", threadid,mm->runnum);
                fflush(stdout);
              mm->runnum = jw[ind+1];
              printf("[%1d] set run number = %d\n",threadid,mm->runnum);
                fflush(stdout);

#ifdef L3LIB

goto skiponclon102;

              trconfig(); /* get configuration */
              l3_init(mm->runnum); /* initialization */

skiponclon102:

#endif
#ifdef PRLIB
              threshold[0] = 0.0001;
              threshold[1] = 0.0001;
              threshold[2] = 0.0003;
              ecinit(mm->runnum, option[0], option[1], option[2]);
#endif
#ifdef TRLIB
              /*trinit_();*/
#endif
D 2

E 2
I 2
              ;
E 2
            }
            pthread_mutex_unlock(&mm->newmap);
          }


          /* data analysis */

/* about 1kHz on mizar */
/*for(i=0; i<10000; i++) i=i;*/



#ifdef L3LIB



          /*******************************************************/
          /*******************************************************/
          /*******************************************************/
          /* copy first hit from pipeline tdcs into old banks;
          that code should be probably in another place, but
          we'll execute it here for a while - Sergey 14-mar-2005 */
          {
            int isec, indold, indnew, i, j, id, nrowold, nrownew, itmp;
            unsigned short *bufold, *bufnew;

            /* ECT->EC */
            for(isec=1; isec<=6; isec++)
            {
              if((indold=etNlink(jw,"EC  ",isec)) <= 0) continue;
              if((indnew=etNlink(jw,"ECT ",isec)) <= 0) continue;
              nrowold = etNrow(jw,indold);
              nrownew = etNrow(jw,indnew);
/*
printf("\n\n\nbefor\n");
bufold = (unsigned short *)&jw[indold];
for(i=0; i<nrowold; i++)
{
  printf("id=0x%08x tdc=%5d adc=%5d\n",bufold[0],bufold[1],bufold[2]);
  bufold += 3;
}
*/
              bufold = (unsigned short *)&jw[indold];
              for(i=0; i<nrowold; i++)
              {
                id = bufold[0];
I 3
				{
                  int lay,str;
                  lay = (id>>8)&0xFF;
                  str = id&0xFF;
                  if(lay<1||lay>6||str<1||str>36)
                  {
                    printf("ERROR in EC bank: sec=%d, lay+5d, str=%d\n",
                      isec,lay,str);
                  }
				}
E 3
                bufnew = (unsigned short *)&jw[indnew];
                for(j=0; j<nrownew; j++)
                {
                  if(bufnew[0] == id)
                  {
                    itmp = bufnew[1];
                    if(itmp>0)
                    {
                      bufold[1] = itmp; /* take first not-zero hit */
                      break;            /* and break !!?? */
                    }
                    else
                    {
                      printf("ECT error: id=%d (0x%08x) tdc=%d\n",id,id,itmp);
                    }
                  }
                  bufnew += 2;
                }
                bufold += 3;
              }
/*
printf("\n\n\nafter\n");
bufold = (unsigned short *)&jw[indold];
for(i=0; i<nrowold; i++)
{
  printf("id=0x%08x tdc=%5d adc=%5d\n",bufold[0],bufold[1],bufold[2]);
  bufold += 3;
}
*/
            }






            /* CCT->CC */
            for(isec=1; isec<=6; isec++)
            {
              if((indold=etNlink(jw,"CC  ",isec)) <= 0) continue;
              if((indnew=etNlink(jw,"CCT ",isec)) <= 0) continue;
              nrowold = etNrow(jw,indold);
              nrownew = etNrow(jw,indnew);
/*
printf("\n\n\nbefor\n");
bufold = (unsigned short *)&jw[indold];
for(i=0; i<nrowold; i++)
{
  printf("id=0x%08x tdc=%5d adc=%5d\n",bufold[0],bufold[1],bufold[2]);
  bufold += 3;
}
*/
              bufold = (unsigned short *)&jw[indold];
              for(i=0; i<nrowold; i++)
              {
                id = bufold[0];
                bufnew = (unsigned short *)&jw[indnew];
                for(j=0; j<nrownew; j++)
                {
                  if(bufnew[0] == id)
                  {
                    itmp = bufnew[1];
                    if(itmp>0)
                    {
                      bufold[1] = itmp; /* take first not-zero hit */
                      break;            /* and break !!?? */
                    }
                    else
                    {
                      printf("CCT error: id=%d (0x%08x) tdc=%d\n",id,id,itmp);
                    }
                  }
                  bufnew += 2;
                }
                bufold += 3;
              }
/*
printf("\n\n\nafter\n");
bufold = (unsigned short *)&jw[indold];
for(i=0; i<nrowold; i++)
{
  printf("id=0x%08x tdc=%5d adc=%5d\n",bufold[0],bufold[1],bufold[2]);
  bufold += 3;
}
*/
            }



            /* TCT->DSTC */
            for(isec=0; isec<1; isec++)
            {
              if((indold=etNlink(jw,"DSTC",isec)) <= 0) continue;
              if((indnew=etNlink(jw,"TCT ",isec)) <= 0) continue;
              nrowold = etNrow(jw,indold);
              nrownew = etNrow(jw,indnew);
/*
printf("\n\n\nbefor\n");
bufold = (unsigned short *)&jw[indold];
for(i=0; i<nrowold; i++)
{
  printf("id=0x%08x tdc=%5d adc=%5d\n",bufold[0],bufold[1],bufold[2]);
  bufold += 3;
}
*/
              bufold = (unsigned short *)&jw[indold];
              for(i=0; i<nrowold; i++)
              {
                id = bufold[0];
                bufnew = (unsigned short *)&jw[indnew];
                for(j=0; j<nrownew; j++)
                {
                  if(bufnew[0] == id)
                  {
                    itmp = bufnew[1];
                    if(itmp>0)
                    {
                      bufold[1] = itmp; /* take first not-zero hit */
                      break;            /* and break !!?? */
                    }
                    else
                    {
                      printf("TCT error: id=%d (0x%08x) tdc=%d\n",id,id,itmp);
                    }
                  }
                  bufnew += 2;
                }
                bufold += 3;
              }
/*
printf("\n\n\nafter\n");
bufold = (unsigned short *)&jw[indold];
for(i=0; i<nrowold; i++)
{
  printf("id=0x%08x tdc=%5d adc=%5d\n",bufold[0],bufold[1],bufold[2]);
  bufold += 3;
}
*/
            }




            /* SCT->SC */
            for(isec=1; isec<=6; isec++)
            {
              int idnew, idhnew, idlnew, place;
              if((indold=etNlink(jw,"SC  ",isec)) <= 0) continue;
              if((indnew=etNlink(jw,"SCT ",isec)) <= 0) continue;
              nrowold = etNrow(jw,indold);
              nrownew = etNrow(jw,indnew);
/*
printf("\n\n\nbefor\n");
bufold = (unsigned short *)&jw[indold];
for(i=0; i<nrowold; i++)
{
  printf("id=0x%08x tdcl=%5d adcl=%5d tdcr=%5d adcr=%5d\n",
    bufold[0],bufold[1],bufold[2],bufold[3],bufold[4]);
  bufold += 5;
}
*/
              /* cleanup TDCs in old bank; will check for 0 ... */
              bufold = (unsigned short *)&jw[indold];
              for(i=0; i<nrowold; i++)
              {
                bufold[1] = 0;
                bufold[3] = 0;
                bufold += 5;                
              }
              bufold = (unsigned short *)&jw[indold];
              for(i=0; i<nrowold; i++)
              {
                id = bufold[0];
I 2
                id = id & 0xFF; /* only lowest byte (new SCT fix) */
/*if(id>40) printf("id=%d\n",id);*/
E 2
                bufnew = (unsigned short *)&jw[indnew];
                for(j=0; j<nrownew; j++)
                {
                  idnew = bufnew[0];
                  idlnew = idnew&0xFF;
                  idhnew = (idnew>>8)&0xFF;
I 2
                  idhnew = idhnew&0x1; /* use only lowest bit (new SCT fix) */
/*if(idlnew>40) printf("idlnew=%d\n",idlnew);*/
E 2
                  if(idhnew==0) place = 0;
                  else if(idhnew==1) place = 2;
                  else
                  {
                    printf("ERROR: SCT: idhnew=%d\n",idhnew);
                    break;
                  }

                  if(idlnew == id)
                  {
I 2
/*if(idlnew>40) printf("id=%d idlnew=%d\n",id,idlnew);*/
E 2
                    if(bufold[1+place]==0) /* replace old TDC only if it =0 */
					{
                      itmp = bufnew[1]; /* new TDC value */
I 2
/*if(idlnew>40) printf("itmp=%d ???\n",itmp);*/
E 2
                      if(itmp>0)
                      {
I 2
/*if(idlnew>40) printf("itmp=%d !!! (place=%d)\n",itmp,place);*/
/*if(idlnew>40&&(idlnew%2)) printf("id=%d itmp=%d !!! (place=%d)\n",idlnew,itmp,place);*/
/*if(idlnew==41||idlnew==43||idlnew==45) printf("id=%d itmp=%d !!! (place=%d)\n",idlnew,itmp,place);*/
E 2
                        bufold[1+place] = itmp; /* take first not-zero hit */
                        /*break;*/            /* and break !!?? */
                      }
                      else
                      {
                        printf("SCT error: id=%d (0x%08x) tdc=%d\n",id,id,itmp);
                      }
					}
                  }
                  bufnew += 2;
                }
                bufold += 5;
              }

/*
printf("\n\n\nafter\n");
bufold = (unsigned short *)&jw[indold];
for(i=0; i<nrowold; i++)
{
  printf("id=0x%08x tdcl=%5d adcl=%5d tdcr=%5d adcr=%5d\n",
    bufold[0],bufold[1],bufold[2],bufold[3],bufold[4]);
  bufold += 5;
}
*/
            }


            /* EC1T->EC1 */
            for(isec=1; isec<=2; isec++)
            {
              int idnew, idhnew, idlnew, place, idhold, idlold;
              if((indold=etNlink(jw,"EC1 ",isec)) <= 0) continue;
              if((indnew=etNlink(jw,"EC1T",isec)) <= 0) continue;
              nrowold = etNrow(jw,indold);
              nrownew = etNrow(jw,indnew);
/*
printf("\n\n\nbefor\n");
bufold = (unsigned short *)&jw[indold];
for(i=0; i<nrowold; i++)
{
  printf("id=0x%08x tdcl=%5d adcl=%5d tdcr=%5d adcr=%5d\n",
    bufold[0],bufold[1],bufold[2],bufold[3],bufold[4]);
  bufold += 5;
}
*/
              /* cleanup TDCs in old bank; will check for 0 ... */
              bufold = (unsigned short *)&jw[indold];
              for(i=0; i<nrowold; i++)
              {
                bufold[1] = 0;
                bufold[3] = 0;
                bufold += 5;                
              }
              bufold = (unsigned short *)&jw[indold];
              for(i=0; i<nrowold; i++)
              {
                id = bufold[0];
                bufnew = (unsigned short *)&jw[indnew];
                for(j=0; j<nrownew; j++)
                {
                  idnew = bufnew[0];
                  idlnew = idnew&0xFF;
                  idhnew = (idnew>>8)&0xFF;
                  if(idhnew>=1 && idhnew<=4)      place = 0;
                  else if(idhnew>=5 && idhnew<=8) place = 2;
                  else
                  {
                    printf("ERROR: EC1T: idhnew=%d\n",idhnew);
                    break;
                  }
                  idlold = id&0xFF;
                  idhold = (id>>8)&0xFF;
                  if(idhnew > 4) idhnew = idhnew - 4;
                  if( (idlnew==idlold) && (idhnew==idhold) )
                  {
                    if(bufold[1+place]==0) /* replace old TDC only if it =0 */
					{
                      itmp = bufnew[1]; /* new TDC value */
                      if(itmp>0)
                      {
                        bufold[1+place] = itmp; /* take first not-zero hit */
                        /*break;*/            /* and break !!?? */
                      }
                      else
                      {
                        printf("EC1T error: id=%d (0x%08x) tdc=%d\n",id,id,itmp);
                      }
					}
                  }
                  bufnew += 2;
                }
                bufold += 5;
              }
/*
printf("\n\n\nafter\n");
bufold = (unsigned short *)&jw[indold];
for(i=0; i<nrowold; i++)
{
  printf("id=0x%08x tdcl=%5d adcl=%5d tdcr=%5d adcr=%5d\n",
    bufold[0],bufold[1],bufold[2],bufold[3],bufold[4]);
  bufold += 5;
}
*/
            }

          }
          /*******************************************************/
          /*******************************************************/
          /*******************************************************/





          nevents1_chunk ++;


goto skiponclon10;

          cand = l3_event(jw, &keep_event); /* process one event */
                                           /* if keep_event=0 - will dump it */
          if(!(cand&0x00008000)) nevents2_chunk ++;
          if((ind_tgbi = etNlink(jw,"TGBI",0)) > 0)
          {
            /*printf("ncol=%d\n",etNcol(jw,ind_tgbi));*/
            /* fill 5th word by level3 trigger info */
            jw[ind_tgbi+4] = cand;
            /*printf("===> %08x\n",jw[ind_tgbi+4]);*/
          }
skiponclon10:




          /* delayed reporting */
#ifdef DELREP
          {
            ind11 = etNlink(jw,"HEAD",0);

            if((ind12 = etNlink(jw,"TGBI",0))>0)
            {
              quad = (jw[ind12]&0x00001000)>>12;
              strob = (jw[ind12]&0x00004000)>>14;
              helicity = (jw[ind12]&0x00008000)>>15;
	        }
            else
	        {
              printf("DELREP ERROR: TGBI\n");
	        }

            if((ind10=etNlink(jw,"HLS ",1))>0)
            {
              /*printf("===== event %8d type# %2d\n",iev,jw[ind11+4]);*/
              ncol = etNcol(jw,ind10);
              nrow = etNrow(jw,ind10);
	          /*printf("ncol=%d nrow=%d\n",ncol,nrow);*/
              for(i=0; i<nrow; i++)
              {
                str = (jw[ind10+i*ncol]&0x80000000)>>31;
                hel = (jw[ind10+i*ncol]&0x40000000)>>30;
                count1 = jw[ind10+i*ncol]&0xFFFFFF;

                if(count1>10) /* ignore 500us intervals */
                {
#ifdef DRDEBUG		  
                  printf("event# %8d type# %2d row# %1d count=%7d: str=%1d hel=%1d -> ",
                    jw[ind11+2],jw[ind11+4],i,count1,str,hel);
#endif
                  if(ind12>0)
		          {
                    if((strob == str) && (helicity == hel))
                    {
#ifdef DRDEBUG		  
                      printf("strob=%1d helicity=%1d (current ) [%1d]\n",
                        strob,helicity,quad);
#endif
                      helicity1 = helicity;
                      strob1 = strob;
                      quad1 = quad;
                    }
                    else if((strob_old == str) && (helicity_old == hel))
                    {
#ifdef DRDEBUG		  
                      printf("strob=%1d helicity=%1d (previous) [%1d]\n",
                        strob_old,helicity_old,quad_old);
#endif
                      helicity1 = helicity_old;
                      strob1 = strob_old;
                      quad1 = quad_old;
                    }
                    else if((strob_old2 == str) && (helicity_old2 == hel))
                    {
#ifdef DRDEBUG		  
                      printf("strob=%1d helicity=%1d (prepre--) [%1d]\n",
                        strob_old2,helicity_old2,quad_old2);
#endif
                      helicity1 = helicity_old2;
                      strob1 = strob_old2;
                      quad1 = quad_old2;
                    }
                    else
                    {
                      printf("strob=%1d helicity=%1d (err =========) [%1d]\n",
                        strob,helicity,quad);
                      helicity1 = hel;   /* from HLS */
                      strob1 = str;      /* from HLS */
                      quad1 = quad_old2; /* ??? */
                      FORCE_RECOVERY;
                    }


                    /*flip helicity if necessary */
                    helicity1 ^= 1;

                    if(done==1)
                    {
                      if(quad1==0)
                      {

                        /* if we are here, 'offset' must be 3, otherwise most likely
                        we've got unexpected quad1==0; we'll try to set quad1=1 and
                        see what happens */
                        if(offset<3)
                        {
                          printf("ERROR: unexpected offset=%d - trying to recover --------------------\n",offset);
                          quad1 = 1;
                          offset++;
	                      /*FORCE_RECOVERY;*/
	                    }
                        else if(offset>3)
                        {
                          printf("ERROR: unexpected offset=%d - forcing recovery 1 -------------------\n",offset);
	                      FORCE_RECOVERY;
                        }
                        else
                        {
                          offset=0;
	                    }
                      }
                      else
                      {
                        offset++;
                        if(offset>3)
                        {
                          /* if we are here, 'offset' must be <=3, otherwise most likely
                          we've got unexpected quad1==1; we'll try to set quad1=0, offset=0
                          and see what happens */
                          printf("ERROR: unexpected offset=%d - forcing recovery 2 -------------------\n",offset);
                          quad1 = 0;
                          offset = 0;
	                      /*FORCE_RECOVERY;*/
                        }
                      }
                    }

#ifdef DRDEBUG
                    printf("quad=%1d(%1d) hel=%1d\n",quad1,offset,helicity1);
#endif

                    /* looking for the beginning of quartet; set done=0 when found */
                    if(done==-1)
                    {
                      printf("looking for the begining of quartet ..\n");
                      if(quad1==0)
                      {
                        done=0; /* quad1==0 means first in quartet */
                        printf("found the begining of quartet !\n");
	                  }
                    }

                    /* search for pattern: use first 24 quartets (not flips!) to get all necessary info */
                    if(done==0)
                    {
                      if(quad1==0)
	                  {
                        done = loadHelicity(helicity1, &iseed, &iseed_earlier);
                        printf("loaded one, done=%d\n",done);
                      }

                      if(done==1)
                      {
                        printf("=============== READY TO PREDICT (ev=%6d) =============== \n",jw[ind11+2]);
	                  }
                    }



                    /* pattern is found, can determine helicity */
                    if(done==1)
                    {
                      if(quad1==0)
                      {
                        /* following two must be the same */
                        present_reading = helicity1;
                        predicted_reading = ranBit(&iseed_earlier);

                        present_helicity = ranBit(&iseed);
	        
#ifdef DRDEBUG		  
                        printf("helicity: predicted=%d reading=%d corrected=%d\n",
                          predicted_reading,present_reading,present_helicity);
#endif


                        /****************/
                        /****************/
                        /* direct check */
                        remember_helicity[0] = remember_helicity[1];
                        remember_helicity[1] = remember_helicity[2];
                        remember_helicity[2] = present_helicity;
#ifdef DEBUG		
                        printf("============================== %1d %1d\n",remember_helicity[0],present_reading);
#endif
                        if( remember_helicity[0] != -1 )
				        {
                          if( remember_helicity[0] != present_reading )
                          {
                            printf("ERROR: direct check failed !!! %1d %1d\n",remember_helicity[0],present_reading);
                            FORCE_RECOVERY;
                          }
			            }
                        /****************/
                        /****************/



                        if(predicted_reading != present_reading)
                        {
                          printf("ERROR !!!!!!!!! predicted_reading != present_reading\n");
                          FORCE_RECOVERY;
                        }
                      }
                    }
	              }
                  else
                  {
                    printf("ERROR: TGBI bank does not exist !!!!! - exit\n");
                    exit(0);
                  }
		        }
              }
            }


            /*******************************/
            /* we are here for every event */

            if(done==1)
            {
              /* first check, if we are still in sync */
              tmp0 = helicity; /* helicity from current event */

              /* 1 flip correction */
              if(offset<3)
              {
                offset1 = offset+1;
                pred_read = predicted_reading;
                pres_heli = present_helicity;
	          }
              else
              {
                offset1 = 0;
                pred_read = ranBit0(&iseed_earlier);
                pres_heli = ranBit0(&iseed);
              }

	  
              if((offset1==0) || (offset1==3))
              {
                temp0 = pred_read^1;
                final_helicity = pres_heli;
              }
              else if((offset1==1) || (offset1==2))
              {
                temp0 = pred_read;
                final_helicity = pres_heli^1;
              }
              else
              {
                printf("ERROR: illegal offset=%d at temp0\n",offset1);
                FORCE_RECOVERY;
              }
              final_helicity ^= 1; /* flip it back */
	  

#ifdef DEBUG
              printf("tmp0=%d temp0=%d final=%d\n",tmp0,temp0,final_helicity);
#endif  

              if(tmp0 != temp0)
	          {
                printf("ERROR: ev %6d: reading=%d predicted=%d\n",
                  jw[ind11+2],tmp0,temp0);
	          }


              /* update helicity info in databank */
              if((ind13=etNlink(jw,"RC26",0))>0)
              {
                unsigned int tmp;
#ifdef DRDEBUG
                printf("befor: 0x%08x (event=%d)\n",jw[ind13],jw[ind11+2]);
#endif

                tmp = jw[ind13];
                if(final_helicity==1) tmp = tmp | 0x00008000; /* set helicity */
                if(strob1==1) tmp = tmp | 0x00004000; /* set strob */
                if(quad1==1) tmp = tmp | 0x00001000; /* set quad */
                tmp = tmp | 0x80000000; /* always set signature bit */
                jw[ind13] = tmp;
#ifdef DRDEBUG
                printf("after: 0x%08x (event=%d)\n",jw[ind13],jw[ind11+2]);
#endif

	          }
              else
	          {
                printf("ERROR: no RC26 bank !!!\n");
	          }
	        }



            /*******************************/
            /*******************************/


            if(ind12>0)
	        {
              quad_old2 = quad_old;
              strob_old2 = strob_old;
              helicity_old2 = helicity_old;

              quad_old = quad;
              strob_old = strob;
              helicity_old = helicity;
	        }

          }

#endif /* DELREP */










#endif

#ifdef PRLIB

          /* pattern recognition */



          /*because we do not have calibration for new TDCs, convert it to old values*/
          {
            int isec, indold, nrowold;
            unsigned short *bufold;

            for(isec=1; isec<=6; isec++)
            {
              if((indold=etNlink(jw,"SC  ",isec)) <= 0) continue;
              nrowold = etNrow(jw,indold);
/*
printf("\n\n\nbefor mycalib\n");
bufold = (unsigned short *)&jw[indold];
for(i=0; i<nrowold; i++)
{
  printf("id=0x%08x tdcl=%5d adcl=%5d tdcr=%5d adcr=%5d\n",
    bufold[0],bufold[1],bufold[2],bufold[3],bufold[4]);
  bufold += 5;
}
*/
              bufold = (unsigned short *)&jw[indold];
              for(i=0; i<nrowold; i++)
              {
                bufold[1] = (int)(8550.-((float)bufold[1])*1.965);
                bufold[3] = (int)(8550.-((float)bufold[3])*1.965);
                bufold += 5;                
              }
/*
printf("\n\n\nafter mycalib\n");
bufold = (unsigned short *)&jw[indold];
for(i=0; i<nrowold; i++)
{
  printf("id=0x%08x tdcl=%5d adcl=%5d tdcr=%5d adcr=%5d\n",
    bufold[0],bufold[1],bufold[2],bufold[3],bufold[4]);
  bufold += 5;
}
*/
            }
          }



          /******************/
          /******************/
          /******************/
          /*FBPM - temporary*/

I 2
D 3
          /*
E 3
I 3
          
E 3
E 2
          ind = etNlink(jw,"FBPM",0);
          if(ind > 0)
          {
            float fx, fy;
            int i, nrow, ncol;
            unsigned short *jw16;

            nrow = etNrow(jw,ind);
            ncol = etNcol(jw,ind);
D 2
            /*printf("FBPM: ncol=%d nrow=%d\n",ncol,nrow);*/
E 2

            jw16 = (unsigned short *)&jw[ind];
            fx = fy = 100000.0;
            for(i=0; i<nrow; i++)
            {
              if(jw16[0]==1) fx = (float)jw16[2];
              if(jw16[0]==2) fy = (float)jw16[2];
              jw16 += 3;
            }

D 3
            if(fx<10000.0 && fy<10000.0)
E 3
I 3
			/*printf("-- %f %f\n",fx,fy);*/
            if(fx<10000.0 && fy<10000.0 && fx>1000.0 && fy>1000.0)
E 3
            {
D 2
              /*printf("FBPM(RASTER): fx=%f fy=%f\n",fx,fy);*/
E 2
              uthfill(evt->histf, 250,fx,fy,1.);
              uthfill(evt->histf, 251,fx,0.,1.);
              uthfill(evt->histf, 252,fy,0.,1.);
I 3

              ixmin = MIN(ixmin,(int)fx);
              ixmax = MAX(ixmax,(int)fx);
              iymin = MIN(iymin,(int)fy);
              iymax = MAX(iymax,(int)fy);
              ifbpmevent ++;
              /*printf("ifbpmevent=%d\n",ifbpmevent);*/

E 3
	        }
            else
	        {
D 3
              printf("FBPM: ERROR: fx=%f fy=%f\n",fx,fy);
E 3
I 3
              /*printf("FBPM: ERROR: fx=%f fy=%f\n",fx,fy)*/;
E 3
	        }
          }
I 2
D 3
		  */
E 3
E 2

I 3
D 6
          if(ifbpmevent >= 10000)
E 6
I 6
          if(ifbpmevent >= 1000)
E 6
          {
            /*printf("raster is not rastering\n");*/
            if( ABS(ixmax-ixmin)<1000 || ABS(iymax-iymin)<1000 )
			{
              printf("ixmin=%d ixmax=%d iymin=%d iymax=%d (%d %d) - bad\n",
                ixmin,ixmax,iymin,iymax,ABS(ixmax-ixmin),ABS(iymax-iymin));
              fflush(stdout);
D 9
              system("error_msg coda_pr clonalarm coda_pr online 2 ERROR 58 \"Raster problem\"");
E 9
I 9
              /*system("error_msg coda_pr clonalarm coda_pr online 2 ERROR 58 \"Raster problem\"");*/
E 9
			}
            else
			{
              printf("ixmin=%d ixmax=%d iymin=%d iymax=%d (%d %d) - good\n",
                ixmin,ixmax,iymin,iymax,ABS(ixmax-ixmin),ABS(iymax-iymin));
              fflush(stdout);
D 9
              system("error_msg coda_pr clonalarm coda_pr online 0 INFO 58 \"Raster Ok\"");
E 9
I 9
              /*system("error_msg coda_pr clonalarm coda_pr online 0 INFO 58 \"Raster Ok\"");*/
E 9
			}
            ifbpmevent = 0;
            ixmin = 10000;
            ixmax = 0;
            iymin = 10000;
            iymax = 0;
I 5
			/*
            uthreset(evt->histf,250);
            uthreset(evt->histf,251);
            uthreset(evt->histf,252);
			*/
E 5
          }

E 3
          /******************/
          /******************/
          /******************/


D 3

E 3
          /* as soon as no level3 here, first two are the same */
          uthfill(evt->histf, 181, 0.5, 0., 1.);
          uthfill(evt->histf, 191, 0.5, 0., 1.);
          uthfill(evt->histf, 181, 1.5, 0., 1.);
          uthfill(evt->histf, 191, 1.5, 0., 1.);
          nevents1_chunk ++;
          if((ntrk=prlib(jw,track)) > 0)
          {
            uthfill(evt->histf, 181, 2.5, 0., 1.);
            uthfill(evt->histf, 191, 2.5, 0., 1.);
            eclib(jw, threshold, &ntrk, track);
            prbos(jw, &ntrk, track);
            nevents2_chunk ++;
          }
#endif
#ifdef TRLIB
          /* track reconstraction */
          /* as soon as no level3 here, first two are the same */
          uthfill(evt->histf, 18, 0.5, 0., 1.);
          uthfill(evt->histf, 19, 0.5, 0., 1.);
          uthfill(evt->histf, 18, 1.5, 0., 1.);
          uthfill(evt->histf, 19, 1.5, 0., 1.);
          nevents1_chunk ++;
          if(etNlink(jw,"PATH",0) > 0)
          {
            uthfill(evt->histf, 18, 2.5, 0., 1.);
            uthfill(evt->histf, 19, 2.5, 0., 1.);

            /*printf(" 1%1d",threadid); fflush(stdout);*/
            trlib(jw,evt,4,1.0,1.0,0);
            /*printf(" 2%1d",threadid); fflush(stdout);*/

            dchfill(jw,evt,evt->ntr_out,evt->itag0,evt->ev_out,evt->tag_res);
            if(evt->ntr_out > 0) nevents2_chunk ++;
          }
		  
#endif
#ifdef DMLIB
          dmlib(jw);
#endif

        } /* event type from HEAD bank */


      } /* if((ind = etNlink(jw,"HEAD",0)) != 0)*/




#ifdef ERLIB
      /* open/close file */
      if((ind = etNlink(jw,"+DST",17)) > 0)
      {
        if(fd > (BOSIO *)0)
        {
          printf("====>> Exec FPACK Command \"CLOSE UNIT=11\"\n");
          if( (stat3 = FParm("CLOSE UNIT=11",&fd)) !=0)
          {
            printf("coda_er: Error in FParm, status %d \n",stat3);
          }
        }

        /* geting file parameters from +DST bank - TO DO !!! */

        /* read run number */
        if( (fn = fopen(RunNumFile,"r")) == NULL)
        {
          printf("coda_er: ERROR : Can't open run number file %s\n",
             RunNumFile);
          runNumber ++;
        }
        else
        {
          fscanf(fn,"%d",&runNumber);
          printf("coda_er: Open run number file %s, read run number %d\n",
            RunNumFile,runNumber);
          fclose(fn);

          /* write run number */
          if( (fn = fopen(RunNumFile,"w")) == NULL )
          {
            printf("coda_er: ERROR : Can't open run number file %s\n",
               RunNumFile);
          }
          else
          {
            fprintf(fn,"%d",runNumber+1);
            printf("coda_er: Open run number file %s, write run number %d\n",
              RunNumFile,runNumber+1);
            fclose(fn);
          }
		}


        strcpy(filename,dir);
        strcat(filename,"DST_%06d.A00");

        split = 100; /* MBytes */
        record_length = 32768; /* KBytes */

        sprintf(current_file,filename,runNumber);
        printf("====>> FILE name would be \"%s\" \n",current_file);
        sprintf(temp,
          "OPEN UNIT=11 FILE='%s' WRITE RECL=%d SPLITMB=%d RAW SEQ NEW BINARY",
          current_file,record_length,split);

        printf("====>> Exec FPACK Command \"%s\"\n",temp);
        if ( (status = FParm(temp,&fd)) !=0)
        {
          printf("FParm status %d \n",status);
          fd = (BOSIO *)(-1);
        }

      }

      if((ind = etNlink(jw,"+DST",20)) > 0)
      {
        if(fd > (BOSIO *)0)
        {
          fd = NULL;
          printf("====>> Exec FPACK Command \"CLOSE UNIT=11\"\n");
          if ( (stat3 = FParm("CLOSE UNIT=11",&fd)) !=0)
          {
            printf("coda_er: Error in FParm, status %d \n",stat3);
          }
        }
      }

      /* event recorder */
      if(fd > (BOSIO *)0 /*&& etNlink(jw,"EVNT",0) > 0*/)
      {
        nevents1_chunk ++;
        if( (stat4=etWrite(fd,jw)) == EBIO_NEWFILEOPEN)
        {
	      stat5 = 1; /* new file was opened in etWrite() - set new file flag */
        }

        if(1>0) nevents2_chunk ++;
      } 
#endif


      /* put event into 'dump' or 'put' lists */
	  if(keep_event == 0)
      {
        mm->pe3[myid][mm->numread3[myid]++] = mm->pe1[myid][jj]; /* dump list*/
      }
	  else
      {
        mm->pe2[myid][mm->numread2[myid]++] = mm->pe1[myid][jj]; /* put list */
      }




      ETPUTDATA(mm->pe1[myid]); /* call it for accepted and rejected events */
    }

#ifdef ERLIB
    /* new file was opened - dump files to the tape if necessary */
    if (stat5)
    {
      int status;
      long avail, used;
      struct statvfs buf;

      printf("coda_er: checking how much space left in directory %s\n",dir);
      if(status=statvfs(dir,&buf))
      {
        printf("coda_er: ERROR=%d calling statvfs(),"
               "so can not check how much space left ...\n",status);
      }
      else
      {
        printf("coda_er: total %d blocks, free %d blocks,"
               " available %d blocks (1 block = %d bytes)\n",
                buf.f_blocks,buf.f_bfree,buf.f_bavail,buf.f_frsize);
        avail = buf.f_bavail*(buf.f_frsize/1024);
        used  = (buf.f_blocks-buf.f_bfree)*(buf.f_frsize/1024);
        printf("coda_er: so we have %d kbytes available space\n",avail);
        printf("                and %d kbytes used space\n",used);
        if(avail > split*1024)
        {
          printf("coda_er: we can continue to write file.\n");
        }
        else
        {
          printf("coda_er: ERROR: there is no enough space for writing.\n");
          printf("coda_er: ERROR: disk could be overflow in any moment !!!\n");
          printf("coda_er: ERROR: !!!!! STOP WRITING IMMIDEATELY !!!\n");
        }
      }



      if(used >= 400000) /* kbytes !!! (40000000 is about 40Gbytes) */
      {
        if(changepartition == 0)
        {
          changepartition = 1; /* next time will start dumping to the tape */
          if(partition == 0)
          {
            partition = 1;
            dir = "/mnt/raid9/dlt/";
          }
          else if(partition == 1)
          {
            partition = 0;
            dir = "/mnt/raid8/dlt/";
          }
        }
        else /* start actual dumping to the tape */ /* MUST CHECK IF IT IS DONE !!! */
        {
          changepartition = 0;
          if(partition == 0)
          {
            system("/u/home/boiarino/clasrun/clas/bosio/bostest/dumpdlt 1");
          }
          else if(partition == 1)
          {
            system("/u/home/boiarino/clasrun/clas/bosio/bostest/dumpdlt 0");
          }
        }




      }



    }
#endif



	/*printf("[%1d] %d %d\n",threadid,nevents1_chunk,nevents2_chunk);*/

    /* put events back */

    pthread_mutex_lock(&mm->mutex_fifo);

    {
      int time1;
      float tmp, tmp1, tmp2, tmp3, tmp4;

      time1 = time(NULL) - mm->time0;
      mm->nevents1 += nevents1_chunk;
      mm->nevents2 += nevents2_chunk;
      mm->Nevents1 += nevents1_chunk;
      mm->Nevents2 += nevents2_chunk;
      if(time1 > 60)
      {
        tmp  = mm->Rate  = ((float)mm->nevents)/((float)time1);
        tmp1 = mm->Rate1 = ((float)mm->nevents1)/((float)time1);
        tmp2 = mm->Rate2 = ((float)mm->nevents2)/((float)time1);
/*temporary*/
tmp2 = mm->Rate2 = tmp;
/*temporary*/
        tmp3 = tmp1/tmp*100.;
        tmp4 = tmp2/tmp1*100.;
        /*
        printf("\nEvent rate: overall %4.0f Hz\n"
                 "            accepted  %4.0f Hz [%4.1f %% from overall]\n"
                 "            processed %4.0f Hz [%4.1f %% from accepted]\n",
          tmp,tmp1,tmp3,tmp2,tmp4);
		*/
        mm->time0 = time(NULL);
        mm->nevents = mm->nevents1 = mm->nevents2 = 0;


#ifdef L3LIB
#ifdef L3NOTHREADS
        /* if some events were obtained then get again
        last accepted event from last chunk and put stat bank there */
		
        if(mm->numread2[myid] > 0)
        {
          ETGETDATALAST(mm->pe2[myid]);

          etNformat(jw,"HISF","F");

printf("put 32\n");
          uth2bos(histf,32,(long *)jw);
          uth2bos(histf,33,(long *)jw);
          uth2bos(histf,34,(long *)jw);
          uth2bos(histf,35,(long *)jw);

          ETPUTDATALAST(mm->pe2[myid]);
        }
		
#endif
#endif

#ifdef PRLIB
        uthunpak(evt->histf, 181, mm->Ratio, " ", 0);
        for(i=3; i<8; i++) mm->Ratio[i] = -1.0;
        if(mm->Ratio[0] > 0.0)
        {
          for(i=0; i<3; i++)
          {
            mm->Count[i] = (int)mm->Ratio[i];
            mm->Ratio[i] = (float)mm->Count[i]/(float)mm->Count[0];
          }
        }

        /* if some events were obtained then get again
        last accepted event from last chunk and put stat bank there */
        if(mm->numread2[myid] > 0)
        {
          ETGETDATALAST(mm->pe2[myid]);

          etNformat(jw,"HISF","F");

          uth2bos(evt->histf,181,(long *)jw);
          uth2bos(evt->histf,191,(long *)jw);

		  /*FBPM-temporary*/
          uth2bos(evt->histf,250,(long *)jw);
          uth2bos(evt->histf,251,(long *)jw);
          uth2bos(evt->histf,252,(long *)jw);
		  /****************/

          ETPUTDATALAST(mm->pe2[myid]);
        }
#endif
#ifdef TRLIB

        uthunpak(evt->histf, 18, mm->Ratio, " ", 0);
        mm->Ratio[0] = mm->Ratio[1] = -1.0;
        if(mm->Ratio[2] > 0.0)
        {
          for(i=2; i<8; i++)
          {
            mm->Count[i] = (int)mm->Ratio[i];
            mm->Ratio[i] = (float)mm->Count[i]/(float)mm->Count[2];
          }
        }
		/*
        printf("count: %d %d %d %d %d %d %d %d\n",
          mm->Count[0],mm->Count[1],mm->Count[2],mm->Count[3],
          mm->Count[4],mm->Count[5],mm->Count[6],mm->Count[7]);
        printf("ratio: %f %f %f %f %f %f %f %f\n",
          mm->Ratio[0],mm->Ratio[1],mm->Ratio[2],mm->Ratio[3],
          mm->Ratio[4],mm->Ratio[5],mm->Ratio[6],mm->Ratio[7]);
		*/
        dcstatadd(&mm->trstat, &evt->stat);
        /*dcstatp(&mm->trstat);*/

        /* if some events were obtained then get again
        last event from last chunk and put stat bank there */
        if(mm->numread2[myid] > 0)
        {
          ETGETDATALAST(mm->pe2[myid]);
		
          etNformat(jw,"+STA","I");
          if((ind=etNcreate(jw,"+STA",0,8,1)) > 0)
          {
            for(i=0; i<8; i++) jw[ind+i] = mm->trstat.type[i];
          }

          etNformat(jw,"HISF","F");



		  /* temporary: fill 230=(228-229) 
          {
            int i;
            float xbin;
            float content1[400], content2[400];

            uthunpak(evt->histf, 228, content1, " ", 0);
            uthunpak(evt->histf, 229, content2, " ", 0);

            for(i=0; i<200; i++)
            {
              content11[i] += content1[i];
              content12[i] += content2[i];
			}

            for(i=0; i<200; i++)
            {
              content13[i] = content11[i] - content12[i];
			}

            for(i=0; i<200; i++)
            {
              xbin = ((float)i)/50.;
              uthfill(evt->histf, 230, xbin, 0., content13[i]);
			}
          }
		  */

		  /* temporary: fill 240=(238-239) 
          {
            int i;
            float xbin;
            float content1[400], content2[400];

            uthunpak(evt->histf, 238, content1, " ", 0);
            uthunpak(evt->histf, 239, content2, " ", 0);
            for(i=0; i<200; i++)
            {
              content21[i] += content1[i];
              content22[i] += content2[i];
			}

            for(i=0; i<200; i++)
            {
              content23[i] = content21[i] - content22[i];
              xbin = ((float)i)/50.;
              uthfill(evt->histf, 240, xbin, 0., content23[i]);
			}
          }
		  */

		
          uth2bos(evt->histf,11,(long *)jw);
          uth2bos(evt->histf,12,(long *)jw);
          uth2bos(evt->histf,13,(long *)jw);
          uth2bos(evt->histf,131,(long *)jw);
          uth2bos(evt->histf,132,(long *)jw);
          uth2bos(evt->histf,133,(long *)jw);
          uth2bos(evt->histf,134,(long *)jw);
          uth2bos(evt->histf,135,(long *)jw);
          uth2bos(evt->histf,136,(long *)jw);
          uth2bos(evt->histf,14,(long *)jw);
          uth2bos(evt->histf,15,(long *)jw);
          uth2bos(evt->histf,151,(long *)jw);
          uth2bos(evt->histf,152,(long *)jw);
          uth2bos(evt->histf,153,(long *)jw);
          uth2bos(evt->histf,154,(long *)jw);
          uth2bos(evt->histf,155,(long *)jw);
          uth2bos(evt->histf,156,(long *)jw);
          uth2bos(evt->histf,157,(long *)jw);
I 2
          uth2bos(evt->histf,158,(long *)jw);
E 2
          uth2bos(evt->histf,16,(long *)jw);
          uth2bos(evt->histf,111,(long *)jw);
          uth2bos(evt->histf,112,(long *)jw);
          uth2bos(evt->histf,113,(long *)jw);
          uth2bos(evt->histf,114,(long *)jw);
          uth2bos(evt->histf,115,(long *)jw);
          uth2bos(evt->histf,116,(long *)jw);
          uth2bos(evt->histf,17,(long *)jw);
          uth2bos(evt->histf,18,(long *)jw);
          uth2bos(evt->histf,19,(long *)jw);
          uth2bos(evt->histf,21,(long *)jw);
          uth2bos(evt->histf,211,(long *)jw);
          uth2bos(evt->histf,212,(long *)jw);
          uth2bos(evt->histf,213,(long *)jw);
          uth2bos(evt->histf,214,(long *)jw);
          uth2bos(evt->histf,215,(long *)jw);
          uth2bos(evt->histf,216,(long *)jw);
          uth2bos(evt->histf,22,(long *)jw);
          uth2bos(evt->histf,221,(long *)jw);
          uth2bos(evt->histf,222,(long *)jw);
          uth2bos(evt->histf,223,(long *)jw);
          uth2bos(evt->histf,224,(long *)jw);
          uth2bos(evt->histf,225,(long *)jw);
          uth2bos(evt->histf,226,(long *)jw);
          uth2bos(evt->histf,227,(long *)jw);

          uth2bos(evt->histf,261,(long *)jw);
          uth2bos(evt->histf,262,(long *)jw);
          uth2bos(evt->histf,271,(long *)jw);
          uth2bos(evt->histf,272,(long *)jw);

          uth2bos(evt->histf,228,(long *)jw);
          uth2bos(evt->histf,229,(long *)jw);
		  
          uth2bos(evt->histf,238,(long *)jw);
          uth2bos(evt->histf,239,(long *)jw);

          uth2bos(evt->histf,230,(long *)jw);
          uth2bos(evt->histf,240,(long *)jw);
		  
          uth2bos(evt->histf,27,(long *)jw);

          uth2bos(evt->histf,901,(long *)jw);
          uth2bos(evt->histf,902,(long *)jw);
          uth2bos(evt->histf,903,(long *)jw);
          uth2bos(evt->histf,904,(long *)jw);
          uth2bos(evt->histf,905,(long *)jw);
          uth2bos(evt->histf,906,(long *)jw);
          uth2bos(evt->histf,911,(long *)jw);
          uth2bos(evt->histf,912,(long *)jw);
          uth2bos(evt->histf,913,(long *)jw);
          uth2bos(evt->histf,914,(long *)jw);
          uth2bos(evt->histf,915,(long *)jw);
          uth2bos(evt->histf,916,(long *)jw);
          uth2bos(evt->histf,921,(long *)jw);
          uth2bos(evt->histf,922,(long *)jw);
          uth2bos(evt->histf,923,(long *)jw);
          uth2bos(evt->histf,924,(long *)jw);
          uth2bos(evt->histf,925,(long *)jw);
          uth2bos(evt->histf,926,(long *)jw);

          uth2bos(evt->histf,951,(long *)jw);
          uth2bos(evt->histf,952,(long *)jw);
          uth2bos(evt->histf,953,(long *)jw);
          uth2bos(evt->histf,954,(long *)jw);
          uth2bos(evt->histf,955,(long *)jw);
          uth2bos(evt->histf,956,(long *)jw);
          uth2bos(evt->histf,961,(long *)jw);
          uth2bos(evt->histf,962,(long *)jw);
          uth2bos(evt->histf,963,(long *)jw);
          uth2bos(evt->histf,964,(long *)jw);
          uth2bos(evt->histf,965,(long *)jw);
          uth2bos(evt->histf,966,(long *)jw);

          uth2bos(evt->histf,971,(long *)jw);
          uth2bos(evt->histf,972,(long *)jw);
          uth2bos(evt->histf,973,(long *)jw);
          uth2bos(evt->histf,974,(long *)jw);
          uth2bos(evt->histf,975,(long *)jw);
          uth2bos(evt->histf,976,(long *)jw);
          uth2bos(evt->histf,981,(long *)jw);
          uth2bos(evt->histf,982,(long *)jw);
          uth2bos(evt->histf,983,(long *)jw);
          uth2bos(evt->histf,984,(long *)jw);
          uth2bos(evt->histf,985,(long *)jw);
          uth2bos(evt->histf,986,(long *)jw);
          uth2bos(evt->histf,991,(long *)jw);
          uth2bos(evt->histf,992,(long *)jw);
          uth2bos(evt->histf,993,(long *)jw);
          uth2bos(evt->histf,994,(long *)jw);
          uth2bos(evt->histf,995,(long *)jw);
          uth2bos(evt->histf,996,(long *)jw);

          /* reset prompt histograms - uth2bos() does reset !!!
          uthreset(evt->histf, 19, " ");
          uthreset(evt->histf, 191, " "); */

          ETPUTDATALAST(mm->pe2[myid]);
        }
        
#endif


      }

    }




	/*printf("10: threadid=%d fifo=%d %d %d %d\n",
	  threadid,mm->fifo[0],mm->fifo[1],mm->fifo[2],mm->fifo[3]); fflush(stdout);*/
	/*printf("MYTURN?: threadid=%d myid=%d nfifo=%d\n",threadid,myid,mm->nfifo); fflush(stdout);*/
    if(mm->fifo[mm->nfifo-1] == myid) /* my turn */
    {
	  /*printf("MYTURN: threadid=%d myid=%d fifo=%d\n",threadid,myid,mm->fifo[mm->nfifo-1]); fflush(stdout);*/

if(mm->numread1[myid]!=(mm->numread2[myid]+mm->numread3[myid]))
printf("coda_prtr: ERROR in put-dump 1: %d! = (%d+%d)\n",
mm->numread1[myid],mm->numread2[myid],mm->numread3[myid]);
      ETPUTEVENTS(mm->attach1[myid],mm->pe2[myid],mm->numread2[myid]);
      ETDUMPEVENTS(mm->attach1[myid],mm->pe3[myid],mm->numread3[myid]);
      /*printf("threadid=%d myid=%d -> puts back 1.\n",threadid,myid); fflush(stdout);*/

      mm->numread1[myid] = 0;
      if(mm->nfifo>0){mm->nfifo--; mm->fifo[mm->nfifo]=0;}else{printf("ERROR1\n");exit(0);}
      /* put 'myid' back to stack */
      if(mm->nid < NIDMAX)
      {
        mm->idstack[mm->nid++] = myid;
      }
      /* check if somebody is waiting to put events */
      while(mm->nfifo > 0)
	  {
        hisid = mm->fifo[mm->nfifo-1];
		/*printf("threadid=%d hisid=%d -> puts back ???\n",threadid,hisid); fflush(stdout);*/
        if(mm->ready[hisid] == 0) break;

if(mm->numread1[hisid]!=(mm->numread2[hisid]+mm->numread3[hisid]))
printf("coda_prtr: ERROR in put-dump 2: %d! = (%d+%d)\n",
mm->numread1[hisid],mm->numread2[hisid],mm->numread3[hisid]);
        ETPUTEVENTS(mm->attach1[hisid],mm->pe2[hisid],mm->numread2[hisid]);
        ETDUMPEVENTS(mm->attach1[hisid],mm->pe3[hisid],mm->numread3[hisid]);
        /*printf("threadid=%d myid=%d -> puts back 2.\n",threadid,myid); fflush(stdout);*/

        mm->numread1[hisid] = 0;
        if(mm->nfifo>0){mm->nfifo--; mm->fifo[mm->nfifo]=0;}else{printf("ERROR1\n");exit(0);}
        /* put 'hisid' back to stack */
        if(mm->nid < NIDMAX)
        {
          mm->idstack[mm->nid++] = hisid;
        }
	  }
    }
    else /* not my turn */
	{
	  /*printf("NOT MYTURN: threadid=%d myid=%d lastfifo=%d\n",threadid,myid,mm->fifo[mm->nfifo-1]); fflush(stdout);*/
      mm->ready[myid] = 1;

      if(force_exit)
      {
        /* wait until all 'my' chunks will be released; otherwise
           after we will detach they can not be released !!! */
        int remains;
        pthread_mutex_unlock(&mm->mutex_fifo);
        do
        {
          remains = 0;
          sleep(1);
          pthread_mutex_lock(&mm->mutex_fifo);
          for(i=0; i<mm->nfifo;i++)
          {
            if(mm->attach1[mm->fifo[i]] == attach)
            {
              printf("[%1d] chunk attached to 0x%08x remains - waiting ...\n",
                        threadid,attach); 
              remains = 1;
              break;
            }
          }
          pthread_mutex_unlock(&mm->mutex_fifo);
        } while(remains);
	  }

    }

    if(force_exit)
    {
      /* lock, cleanup, unlock, detach and exit */
      /*pthread_mutex_lock(&mm->mutex_fifo);*/
      /* return 'threadid'
      threadid = ++ mm->nthread;
      printf("[%1d] return 'threadid'\n",threadid);*/
      pthread_mutex_unlock(&mm->mutex_fifo);
#ifdef MPROG
      tmp = mm_mem_unmap(map_file_name, mm);
      if(tmp != 0)
      {
        printf("coda_tr: ERROR: mm_mem_unmap() returns %d\n",tmp);
      }
      else
      {
        printf("coda_tr: file >%s< unmapped successfully\n",map_file_name);
      }
#endif
      break;
    }

    MYGET;
    mm->nevents += mm->numread1[myid];
    mm->Nevents += mm->numread1[myid];

    pthread_mutex_unlock(&mm->mutex_fifo);

  } /* end forever loop */

  ETDETACH;

  return;
}




/* main function */

#ifndef PRLIB
#include "wcs.h"
#endif

main(int argc, char **argv)
{
#ifdef SSIPC
  SSTHREAD ssarg;
#endif
  sigset_t  sigblock;
  pthread_t tid;

  pthread_t idth[NTHREADMAX];
  THREAD stats[NTHREADMAX];
  pthread_mutexattr_t mattr;
  int status,i,j,ith,tmp,size;
  struct tms t1,t2;
  double ddd, utime;

#ifdef MPROG
  MM *mm;
#endif


  if(argc > 1 && !strncmp("-h",argv[1],2))
  {
#ifdef L3LIB
    printf("Usage: coda_l3 -a <project> -s <session>\n");
#endif
#ifdef PRLIB
    printf("Usage: coda_pr -a <project> -s <session>\n");
#endif
#ifdef TRLIB
    printf("Usage: coda_tr -a <project> -s <session>\n");
#endif
#ifdef ERLIB
    printf("Usage: clon_er -a <project> -s <session>\n");
#endif
#ifdef DMLIB
    printf("Usage: coda_dm -a <project> -s <session>\n");
#endif
    printf("       If no parameters specified 'sertest' will be used\n");
    exit(0);
  }

  /* default settings */
  strcpy(project,"sertest");
  strcpy(session,"sertest");
  strcpy(et_file_name,"/tmp/et_sys_sertest");

  /* change defaults if parameters are specified */
  for(i=1; i<argc; i+=2)
  {
    if(!strncmp("-a",argv[i],2) && argc > i+1)
    {
      strcpy(project,argv[i+1]);
    }
    else if(!strncmp("-s",argv[i],2) && argc > i+1)
    {
      strcpy(session,argv[i+1]);
      strcpy(et_file_name,"/tmp/et_sys_");
      strcat(et_file_name,session);
    }
  }

  if(chunk > NCHUNKMAX) chunk = NCHUNKMAX;
  if(nthreads > NTHREADMAX)
  {
    printf("coda_prtr: maximum number of threads is %d\n",NTHREADMAX);
    nthreads = NTHREADMAX;
  }

  printf("coda_prtr:  project >%s<   session >%s<   ET file >%s<\n",
    project,session,et_file_name);
  printf("            chunk size = %d   nthreads = %d\n",chunk,nthreads);


  /* setup signal handling */
  sigfillset(&sigblock);
  /* block all signals */
  tmp = pthread_sigmask(SIG_BLOCK, &sigblock, NULL);
  if(tmp != 0)
  {
    printf("coda_prtr: pthread_sigmask failure\n");
    exit(1);
  }
D 2
/*#ifdef SunOS*/
E 2
I 2
#ifdef SunOS
E 2
  thr_setconcurrency(thr_getconcurrency() + 1);
D 2
/*#endif*/
E 2
I 2
#endif
E 2
  /* spawn signal handling thread */
  pthread_create(&tid, NULL, signal_thread, (void *)NULL);



  /* allocate and initialize mm */
#ifdef MPROG
  tmp = mm_mem_create(map_file_name,sizeof(MM),(void **) &mm, &size);
  if (tmp == ET_OK)
#else
  size = sizeof(MM);
  if((mm = (MM *) calloc(1,sizeof(MM))) != NULL)
#endif
  {
    printf("coda_prtr: allocate and initialize new mm at 0x%08x\n",mm);

    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&(mm->newmap), &mattr);
    pthread_mutex_init(&(mm->mutex_fifo), &mattr);

    mm->totalsize = size;

    mm->nid = NIDMAX;
    for(i=0; i<NIDMAX; i++) mm->idstack[i] = i;

    mm->nfifo = 0;
    for(i=0; i<NFIFOMAX; i++) mm->fifo[i] = 0;

    mm->runnum = 0;
    mm->nthread = 0;
    mm->nevents = mm->nevents1 = mm->nevents2 = 0;
    mm->Nevents = mm->Nevents1 = mm->Nevents2 = 0;
    for(i=0; i<8; i++)
    {
      mm->Count[i] = 0;
      mm->Ratio[i] = -1.0;
    }

#ifdef TRLIB
	
    dcstatinit(&mm->trstat);
	
#endif
  }
#ifdef MPROG
  else if(tmp == ET_ERROR_EXISTS)
  {
    mm = (MM *) mm_mem_attach(map_file_name,sizeof(MM));
    printf("coda_prtr: memory exist at 0x%08x, nid=%d\n",mm,mm->nid);
  }
#endif
  else
  {
    printf("coda_prtr: malloc returns %d\n",mm);
    exit(0);
  }


  printf("coda_prtr starts, et_file_name=>%s< chunk=%d nthreads=%d\n",
              et_file_name,chunk,nthreads);

  /* open/create ET station: ETOPEN - non-blocking, ETOPENB - blocking */
#ifdef L3LIB
  ETOPENB;
#endif
#ifdef PRLIB
  ETOPEN;
#endif
#ifdef TRLIB
  ETOPENB;
#endif
#ifdef ERLIB
  ETOPENB;
#endif
#ifdef DMLIB
  ETOPENB;
#endif

  /* initialization */
#ifdef PRLIB
  trconfig();
  prinit_();
#endif
#ifdef TRLIB
  bosInit(wcs_.jw,JNDIMB);
  trconfig();
  trinit();
#endif

#ifdef MPROG
  l3_thread((void *)&stats[ith]);
#else
  for(ith=0; ith<nthreads; ith++)
  {

#ifdef PRLIB
    stats[ith].evt = (PRevent *) malloc(sizeof(PRevent));
    printf("coda_prtr: stats[%d].evt ->%08x\n",ith,stats[ith].evt);
    stats[ith].evt->trk = (PRTRACK *) malloc(NTRACK*sizeof(PRTRACK));
    printf("coda_prtr: stats[%d].evt->trk ->%08x\n",ith,stats[ith].evt->trk);
    stats[ith].evt->histf = (UThistf *) malloc(NHIST*sizeof(UThistf));
    printf("coda_prtr: stats[%d].evt->histf ->%08x\n",ith,stats[ith].evt->histf);
#endif
#ifdef TRLIB
    stats[ith].evt = (TRevent *) malloc(sizeof(TRevent));
    printf("coda_prtr: stats[%d].evt ->%08x\n",ith,stats[ith].evt);
    stats[ith].evt->histf = (UThistf *) malloc(NHIST*sizeof(UThistf));
    printf("coda_prtr: stats[%d].evt->histf ->%08x\n",ith,stats[ith].evt->histf);
#endif

    if(pthread_create(&idth[ith], NULL, l3_thread, (void *)&stats[ith]) != 0)
    {
      printf("coda_prtr: pthread_create(0x%08x[%d],...) failure\n",idth[ith],ith);
      return(-1);
    }
    else
	{
      printf("coda_prtr: pthread_create(0x%08x[%d],...) done\n",idth[ith],ith);
    }
  }
#endif


#ifdef SSIPC

  ssarg.project    = project;
  ssarg.session    = session;
  ssarg.rate_rec   = 0.0; 
  ssarg.rate_proc  = 0.0; 
  ssarg.nev_rec    = 0;
  ssarg.nev_proc   = 0;
  ssarg.et_ok      = 1; /* 1 if connected, 0 if not; we'll always set it 1 */
  ssarg.force_exit = 0;
  ssarg.last_time  = time(NULL);

  /*  set ipc parameters, connect to server, etc. */
  ipc_set_application(ssarg.project);
  ipc_set_quit_callback(ssipc_quit_callback);
  ipc_set_user_status_poll_callback(ssipc_status_poll_callback);
  ipc_set_control_message_callback(ssipc_control_message_callback);
#ifdef L3LIB
  ssarg.data_flow = (char *)"et:LEVEL3";
  strcpy((char *)unique_name,(char *)"coda_l3");
#endif
#ifdef PRLIB
  ssarg.data_flow = (char *)"et:PRLIB";
  strcpy((char *)unique_name,(char *)"coda_pr");
#endif
#ifdef TRLIB
  ssarg.data_flow = (char *)"et:TRLIB";
  strcpy((char *)unique_name,(char *)"coda_tr");
#endif
#ifdef ERLIB
  ssarg.data_flow = (char *)"et:ERLIB";
  strcpy((char *)unique_name,(char *)"coda_er");
#endif
#ifdef DMLIB
  ssarg.data_flow = (char *)"et:DMLIB";
  strcpy((char *)unique_name,(char *)"coda_dm");
#endif
  strcat((char *)unique_name,(char *)"_");
  strcat((char *)unique_name,(char *)session);
  ssarg.unique_name = unique_name;
#ifdef L3LIB
  status = ipc_init(ssarg.unique_name,"Level 3 Trigger"); /* <=21 char */
#endif
#ifdef PRLIB
  status = ipc_init(ssarg.unique_name,"Pattern Recognition"); /* <=21 char */
#endif
#ifdef TRLIB
  status = ipc_init(ssarg.unique_name,"Track Reconstruction"); /* <=21 char */
#endif
#ifdef ERLIB
  status = ipc_init(ssarg.unique_name,"SECOND Event Recorder"); /* <=21 char */
#endif
#ifdef DMLIB
  status = ipc_init(ssarg.unique_name,"Data Monitor"); /* <=21 char */
#endif

  if(status < 0)
  {
    printf("\ncoda_prtr: Unable to connect to server...probably duplicate unique name\n"
	 "   ...check for another %s connected using ipc_info\n\n",ssarg.unique_name);
    exit(1);
  }
  TipcSrvSubjectSetSubscribe((T_STR)"evt_system",TRUE);
  TipcSrvFlush();
#endif


  while(1)
  {
#ifdef SSIPC
    /* fill and send 'ssarg' structure to 'ssipc' */
#ifdef PRLIB
    if(mm->Ratio[0] > 0.0)
#endif
#ifdef TRLIB
    if(mm->Ratio[2] > 0.0)
#endif
    {
      ssarg.nev_rec   = mm->Nevents;
      ssarg.nev_proc  = mm->Nevents2;
      ssarg.rate_rec  = mm->Rate;
      ssarg.rate_proc = mm->Rate2;
      for(i=0; i<8; i++)
      {
        ssarg.Ratio[i] = mm->Ratio[i];
      }
    }


    /* if we are not recieving events for a while, do not report mm->Rate etc,
	it is not correct (it remembers last processed chunk of data ..) */
I 2
#ifdef L3LIB
E 2
	printf("event rate -> %f Hz (ntimeouts=%d)\n",ssarg.rate_rec,ntimeouts);
I 2
#endif
E 2
    if(ntimeouts > 10000)
    {
I 2
#ifdef L3LIB
E 2
      printf("CLEANUP !!!\n");
I 2
#endif
E 2
      ssarg.nev_rec   = 0.0;
      ssarg.nev_proc  = 0.0;
      ssarg.rate_rec  = 0.0;
      ssarg.rate_proc = 0.0;
      for(i=0; i<8; i++)
      {
        ssarg.Ratio[i] = -1.0;
      }
    }


    ssipc_put_args(&ssarg);
    for(i=0; i<8; i++)
    {
      ssarg.Ratio[i] = -1.0;
    }

    /* sleep and check for incoming ipc messages */
    ipc_check(5.0);
    ssipc_get_args(&ssarg);
    if(ssarg.force_exit) force_exit = ssarg.force_exit; /* can be 1 already */
#else
    sleep(5);
#endif /*SSIPC*/

    if(force_exit)
    {
      printf("coda_prtr: exit main() function\n");
      break;
    }
  }

#ifdef SSIPC
  ipc_close();
#endif /*SSIPC*/

#ifndef MPROG

#ifdef PRLIB
  for(ith=0; ith<nthreads; ith++)
  {
    free(stats[ith].evt->histf);
    free(stats[ith].evt->trk);
    free(stats[ith].evt);
  }
#endif
#ifdef TRLIB
  for(ith=0; ith<nthreads; ith++)
  {
    free(stats[ith].evt->histf);
    free(stats[ith].evt);
  }
#endif

#endif

  /* remove ET station 
  ETCLOSE;*/

  printf("coda_prtr done.\n");
  exit(0);
}



/************************************************************/
/*              separate thread to handle signals           */
/************************************************************/

static void *
signal_thread (void *arg)
{
  int status;
  sigset_t   signal_set;
  int        sig_number;

  sigemptyset(&signal_set);
  sigaddset(&signal_set, SIGINT);
  
  sigwait(&signal_set, &sig_number);
  printf("coda_prtr: got a control-C, exiting ...\n");

  /* set condition to clean up from the everything
     related to the current program */
  force_exit = 1;  
  sleep(1);

  return;
}


E 1
