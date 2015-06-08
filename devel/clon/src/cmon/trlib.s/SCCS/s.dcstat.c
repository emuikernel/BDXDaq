h12623
s 00000/00000/00000
d R 1.2 01/11/19 19:06:00 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/dcstat.c
e
s 00410/00000/00000
d D 1.1 01/11/19 19:05:59 boiarino 1 0
c date and time created 01/11/19 19:05:59 by boiarino
e
u
U
f e 0
t
T
I 1
/* files dcstatb.c, dcstati.c, dcstatp.c */


/*
   Purpose and Methods : keep tallies on different types of
                         analysis cuts

                         for multithreaded applications: allocate
                         TRstat structure localy for every thread
                         and use 'dcstatadd(TRstat *stat)' to accumulate
                         all statistic together


   Inputs  : icut       - cut type

   Outputs :

   Controls:

   Library belongs    : libsda.a

   Calls              : none

   Created  JUL-20-1992  Billy Leung
   C remake by Sergey Boyarinov

   Called by ana_xxxx routines
*/

#include <stdio.h>
#include "dclib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

static TRstat Stat;


void
dc_statreset_()
{
  dc_statreset();
  return;
}

void
dc_statreset()
{
  dcstatreset(&Stat);
}

void
dcstatreset(TRstat *stat)
{
  int i;

  for(i=0; i<10; i++)
  {
    stat->ack[i] = 0;  /* FALSE */
    stat->dtype[i] = 0;
    stat->type[i] = 0;
  }

  return;
}

void
dcstatclean(TRstat *stat)
{
  int i;

  for(i=0; i<10; i++)
  {
    stat->dtype[i] = 0;
    stat->type[i] = 0;
  }

  return;
}

void
dc_stataddstatic_()
{
  dc_stataddstatic();
  return;
}

void
dc_stataddstatic()
{
  dcstataddstatic(&Stat);
}

void
dcstataddstatic(TRstat *stat)
{
  int i;

  for(i=0; i<10; i++)
  {
    if(Stat.ack[i])
    {
      Stat.dtype[i] = stat->dtype[i];
      Stat.type[i] += stat->type[i];
      /*printf("%d --> %d %d  -  %d %d\n",i,Stat.type[i],stat->type[i],
          Stat.dtype[i],stat->dtype[i]);*/
	}
  }

  return;
}

void
dcstatadd(TRstat *dst, TRstat *src)
{
  int i;

  for(i=0; i<10; i++)
  {
    if(dst->ack[i])
    {
      dst->dtype[i] = src->dtype[i];
      dst->type[i] += src->type[i];
      /*printf("%d --> %d %d  -  %d %d\n",i,dst->type[i],src->type[i],
          dst->dtype[i],src->dtype[i]);*/
	}
  }

  return;
}

void
dc_statb_(int *icut)
{
  dc_statb(*icut);
  return;
}

void
dc_statb(int icut)
{
  dcstatb(&Stat,icut);
  return;
}

void
dcstatb(TRstat *stat, int icut)
{
  int i, cut;

  cut = icut - 1;
  if(cut >= 0 && cut < 10)
  {
    /*if(stat->ack[cut])*/
    {
      stat->dtype[cut] = 1;
      stat->type[cut] += 1;
    }
  }

  return;
}


/*
   Purpose and Methods : Initialize cut tallies.


   Inputs  : icut       - cut type
           : strg       - cut description

   Outputs :

   Controls:


   Library belongs    : libsda.a

   Calls              : none

   Created  JUL-20-1992  Billy Leung
   C remake by Sergey Boyarinov

   Called by sda_init
*/


#include <string.h>

void
dc_stati_(int *icut, char *strg, int len)
{
  char *s;
  s = (char *)malloc(len+1);
  strncpy((char *)s, (char *)strg, len);
  s[len] = '\0';

  dc_stati(*icut, s);

  free(s);
  return;
}

void
dc_stati(int icut, char *strg)
{
  dcstati(&Stat, icut, strg);
  return;
}

void
dcstati(TRstat *stat, int icut, char *strg)
{
  int cut;

  cut = icut - 1;
  if(cut >= 0 && cut < 10)
  {
    stat->ack[cut] = 1; /* TRUE */
    strncpy((char *)stat->caption[cut], (char *)strg, 24);
    stat->caption[cut][24] = '\0';
  }

  return;
}

void
dcstatinit(TRstat *stat)
{
  dcstati(stat,1,"Total:     Ntrig (Nevt) =");
  dcstati(stat,2,"Level 3:   Nevt w.l3lib =");
  dcstati(stat,3,"Patt.Rec:  Nevt w.prlib =");
  dcstati(stat,4,"HBFit:     Nevt PR chi2 <");
  dcstati(stat,5,"dcfinde:   Nevt beta0   =");
  dcstati(stat,6,"dcdcam:    Nevt LR chi2 <");
  dcstati(stat,7,"TBFit:     Nevt FF chi2 <");
  dcstati(stat,8,"sda_anal:  Nevt w. Ntrk =");

  return;
}

void
dc_statinit_()
{
  dcstatinit(&Stat);

  return;
}


/*
   Purpose and Methods : Prints No. of events left after each cut.


   Inputs  :

   Outputs :

   Controls:

   Library belongs    : libsda.a

   Calls              : none

   Created  JUL-20-1992  Billy Leung
   C remake by Sergey Boyarinov

   Called by sda_run
*/


int iucomp_();

void
dc_statp_()
{
  dc_statp();
  return;
}

void
dc_statp()
{
  dcstatp(&Stat);
  return;
}

void
dcstatp(TRstat *stat)
{
  int i, inter, itmp;

  inter = 0;
  itmp = 1;
  if(iucomp_("INT ",&sdakeys_.lmode,&itmp,4) != 0) inter = 1;

  i = 1;
  while(!stat->ack[i-1] && i <= 10) i++;
  if(i <= 10)
  {
    if(inter == 1)
    {
      printf("\n\n Cut# Description of cut        Value    Passed  Yes/No\n");
    }
    else
    {
      printf("\n\n Cut# Description of cut        Value    Passed\n");
    }
  }

  /* Report tallied analysis failures */

  for(i=0; i<10; i++)
  {
    if(stat->ack[i])
    {
      if(inter == 1)
      {
        printf(" %4d %s%6.1f%10d%6d\n",i+1,stat->caption[i],
                sdakeys_.zcut[i],stat->type[i],stat->dtype[i]);
        stat->dtype[i] = 0;
      }
      else
      {
        printf(" %4d %s%6.1f%10d\n",i+1,stat->caption[i],
                sdakeys_.zcut[i],stat->type[i]);
      }
    }
  }

  return;
}


/* prints for "BAT " */

void
dc_statp1_()
{
  dc_statp1();
  return;
}

void
dc_statp1()
{
  dcstatp1(&Stat);
  return;
}

void
dcstatp1(TRstat *stat)
{
  float time_evt;

  timex_(&sdakeys_.time[9]);
  time_evt = 0.;
  if(stat->type[0] > 0) time_evt =
            (sdakeys_.time[9] - sdakeys_.time[8]) / (float)(stat->type[0]);
 printf("\n Number of events:%8d  Total time [s]:%9.3f Time/event [s]:%6.3f\n",
              stat->type[0],sdakeys_.time[9] - sdakeys_.time[8],time_evt);

  return;
}



/* prints for "SIM " */

#include "sdaevgen.h"

void
dc_statp2_()
{
  dc_statp2();
  return;
}

void
dc_statp2()
{
  dcstatp2(&Stat);
  return;
}

void
dcstatp2(TRstat *stat)
{
  int Ntrig, Nrec;
  float Etrig, Eacc, Erec;

  Ntrig = stat->type[0];
  Etrig = Ntrig / (float)(sdaevgen_.Nevin_prod);
  Eacc  = sdaevgen_.Nevin_acc / (float)(sdaevgen_.Nevin_prod);
  Nrec  = stat->type[7];
  Erec  = Nrec / (float)(sdaevgen_.Nevin_acc);
  printf("\n Nprod =%7d\n"
           " Ntrig =%7d     Etrig = Ntrig/Nprod =%10.4f\n"
           " Nacc  =%7d     Eacc  = Nacc/Nprod  =%10.4f\n"
           " Nrec  =%7d     Erec  = Nrec/Nacc   =%10.4f\n",
       sdaevgen_.Nevin_prod, Ntrig,Etrig, sdaevgen_.Nevin_acc,Eacc, Nrec,Erec);

  return;
}







E 1
