/*
   pr_export.c - interface functions for OFFLINE packages

   Created:     May 1, 1998
   Last update: Mar 1, 2002

   Sergey Boyarinov
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prlib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;



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

/*** iclib() only test
static first = 1;
if(first)
{
  icinit(runnum);
  first=0;
}
iclib(jw);
return;
*/

  *ifail = 0;
  if((ntrk=prlib(jw,track)) > 0)
  {
    *ifail = ntrk;
/*if(ntrk > 10) {*ifail=0;return;}*/
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
      /*icinit(runnum);*/
    }

    eclib(jw, threshold, &ntrk, track);
    /*iclib(jw);*/

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

  if((ind = etNlink(jw,"PATH",0)) <= 0)
  {
    return;
  }
  ntrack = (etNdata(jw,ind)*sizeof(int))/sizeof(PRTRACK);
  track = (PRTRACK *)&jw[ind];

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






