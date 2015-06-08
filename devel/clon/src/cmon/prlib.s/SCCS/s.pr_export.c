h36283
s 00080/00001/00121
d D 1.5 07/10/21 23:08:10 boiarino 6 5
c *** empty log message ***
e
s 00004/00004/00118
d D 1.4 04/02/03 20:46:47 boiarino 5 4
c *** empty log message ***
e
s 00013/00000/00109
d D 1.3 03/12/16 22:18:04 boiarino 4 3
c .
e
s 00002/00052/00107
d D 1.2 02/04/30 14:16:58 boiarino 3 1
c adjust for the new segment finding
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 18:57:06 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/prlib.s/pr_export.c
e
s 00159/00000/00000
d D 1.1 01/11/19 18:57:05 boiarino 1 0
c date and time created 01/11/19 18:57:05 by boiarino
e
u
U
f e 0
t
T
I 1
/*
   pr_export.c - interface functions for OFFLINE packages

   Created:     May 1, 1998
D 3
   Last update: Aug 1, 1999
E 3
I 3
   Last update: Mar 1, 2002
E 3

D 3
   Serguei Boiarinov
E 3
I 3
   Sergey Boyarinov
E 3
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prlib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;


D 3
int
praxial_(char *ntest0, char *ntest1, char *ntest2)
{
  return(praxial(*ntest0,*ntest1,*ntest2));
}
E 3

D 3
int
prstereo_(char *ntest0, char *ntest1, char *ntest2)
{
  return(prstereo(*ntest0,*ntest1,*ntest2));
}

/* changed !!! */
void
prscan_(int *mtest1, int *mtest2, int *isec, short idtime[6][192][6], int *found)
{
  *found = prscan(*mtest1, *mtest2, *isec, idtime);
  return;
}

#ifdef SL5

void
praxialscan_(int *mtest1, int *mtest2, int *found, int *p)
{
  *found = praxialscan(*mtest1, *mtest2, p);
  return;
}

void
prstereoscan_(int *mtest1, int *mtest2, int *found, int *p)
{
  *found = prstereoscan(*mtest1, *mtest2, p);
  return;
}

#endif

E 3
int prupdatelink(int, int, float vect1[6], float, int, int ecdigi2[3], int nw[6][6], float *);
void
prupdatelink_(int *mtest1, int *mtest2, float vect1[6], float *pin, int *ntof, int ecdigi2[3], int nw[6][6], int *found, float *p)
{
  *found = prupdatelink(*mtest1, *mtest2, vect1, *pin, *ntof, ecdigi2, nw, p);
  return;
}

void
prinit_()
{
  FILE *fd;
  char *parm;
  char str[256];

  /* get road dictionary file name */
  if((parm = (char *)getenv("CLON_PARMS")) != NULL)
  {
    sprintf(str,"%s/PRLIB/DICT/%s",parm,sdakeys_.dictfile);
  }
  else
  {
    printf("prinit_(): ERROR: CLON_PARMS is not defined - exit.\n");
    exit(0);
  }

  printf("prinit_(): >%s< %d %d %d\n",str,
    sdakeys_.lanal[1],sdakeys_.lanal[2],sdakeys_.lanal[5]);

  prinit(str,sdakeys_.lanal[1],sdakeys_.lanal[2],sdakeys_.lanal[5]);
  return;
}

void
prlib_(int *jw, int *ifail)
{
  static int runnum = 0;
  /* EC parameters */
  const float threshold[3] = {0.0001,0.0001,0.0003};
  const int option[3] = {0,0,0};
  int ntrk, ind, rnum;
  static int opt=1;
  static PRTRACK track[NTRACK];

I 4
D 5

E 5
I 5
/*** iclib() only test
E 5
static first = 1;
if(first)
{
  icinit(runnum);
  first=0;
}
iclib(jw);
return;
I 5
*/
E 5

D 5

E 5
E 4
  *ifail = 0;
  if((ntrk=prlib(jw,track)) > 0)
  {
    *ifail = ntrk;
D 6
if(ntrk > 10) {*ifail=0;return;}
E 6
I 6
/*if(ntrk > 10) {*ifail=0;return;}*/
E 6
    /* ec reconstruction */
    if((ind = etNlink(jw,"HEAD",0)) > 0)
    {
      rnum = jw[ind+1];
rnum=14317;
    }
    else
    {
      rnum = 9999;
    }
    if(rnum != runnum)
    {
      runnum = rnum;
      /*printf("eclib: set run number = %d\n",runnum);*/
      ecinit(runnum, option[0], option[1], option[2]);
I 4
D 5
      icinit(runnum);
E 5
I 5
      /*icinit(runnum);*/
E 5
E 4
    }

    eclib(jw, threshold, &ntrk, track);
I 4
D 5
    iclib(jw);
E 5
I 5
    /*iclib(jw);*/
E 5
E 4

    prbos(jw, &ntrk, track); /* fill output BOS bank */
  }

  return;
}


void
prwrite_()
{
  prwrite("prlink.bos");
  return;
}

D 3

void
prsegmlist_(int *iw3, short idtime[192][6], int *ifound, int *ngr, int *min_hit_seg)
{
  prsegmlist(*iw3, idtime, ifound, *ngr, *min_hit_seg);
}

void
prupdatesegm_(int nw[6])
{
 prupdatesegm(nw);
}

E 3


I 6
/* create DC1 bank with pattern recognition results */
void
pr_makeDC1(int *jw)
{
  PRTRACK *trkptr, *track;
  PRCLUSTER *clustptr;
  PRLAYER *layerptr;
  int i, j, itr, itr1, nhits[6], isec, is, iv, icl, il, ih, isg, ncoin, trkcount, io;
  int *ptri, ind, tmp, ntrack, nsegm[6];
  float *ptrf;
E 6

I 6
  if((ind = etNlink(jw,"PATH",0)) <= 0)
  {
    return;
  }
  ntrack = (etNdata(jw,ind)*sizeof(int))/sizeof(PRTRACK);
  track = (PRTRACK *)&jw[ind];
E 6

I 6
  /* count hits */
  for(i=0; i<6; i++) nhits[i] = 0;
  for(itr=0; itr<ntrack; itr++)
  {
    trkptr = &track[itr];
    isec = trkptr->sector;
    for(is=0; is<6; is++)
    {
      clustptr = &trkptr->cluster[is];
      for(il=0; il<6; il++)
	  {
        layerptr = &clustptr->layer[il];
        for(ih=0; ih<layerptr->nhit; ih++)
		{
          printf("itr=%d isec=%d is=%d il=%d ih=%d -> %d %d\n",itr,isec,is,il,ih,layerptr->iw[ih],layerptr->tdc[ih]);
          nhits[isec-1] ++;
	    }
	  }
    }
  }

  /*create bank*/
  etNformat(jw,"DC1 ","I,F");
  for(isec=1; isec<=6; isec++)
  {
    if(nhits[isec-1] <= 0) continue;
    if((ind = etNcreate(jw,"DC1 ",isec,2,nhits[isec-1])) > 0)
    {
      ptri = (int *)&jw[ind];
      ptrf = (float *)&jw[ind+1];
      for(itr=0; itr<ntrack; itr++)
      {
        trkptr = &track[itr];
        if(trkptr->sector != isec) continue;
        for(is=0; is<6; is++)
        {
          clustptr = &trkptr->cluster[is];

          for(il=0; il<6; il++)
	      {
            layerptr = &clustptr->layer[il];
            for(ih=0; ih<layerptr->nhit; ih++)
		    {
              printf("itr=%d isec=%d is=%d il=%d ih=%d (highid=%d) -> %d %d\n",
                itr,isec,is,il,ih,(6*is+il+1),layerptr->iw[ih],layerptr->tdc[ih]);

              *ptri = layerptr->iw[i] + 256*(6*is+il+1);
              *ptrf = layerptr->tdc[i];
              ptri+=2;
              ptrf+=2;

	        }
	      }
        }
      }
    }
  }


  return;
}


E 6




E 1
