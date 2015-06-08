h33079
s 00000/00000/00000
d R 1.2 01/11/19 18:57:35 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/prlib.s/obsolete/proutput.c
e
s 00115/00000/00000
d D 1.1 01/11/19 18:57:34 boiarino 1 0
c date and time created 01/11/19 18:57:34 by boiarino
e
u
U
f e 0
t
T
I 1

/* copy data from Pattern Recognition structure to 'sdaanal.inc' arrays */

#include <stdio.h>
#include "prlib.h"

void
proutput_(int *iw, int *sec, int segm[6][nsgmx][12], int clust[6][nclmx][nsgcmx],
          int nsegmc[6][nclmx], int nclust[6],
          int *ntr_link, int itr_level[ntrmx], int itr_sect[ntrmx],
          int lnk_segm[ntrmx][6], int lnk_clust[ntrmx][6],
          float lnk_vect[ntrmx][nlnkvect])
{
  PRTRACK *trkptr, *trkptr1, *track;
  PRCLUSTER *clustptr;
  PRSEGMENT *sgmptr;
  int i, j, itr, itr1, is, iv, icl, isg, ncoin, trkcount, io;
  int *ptri, ind, nhits, tmp, ntrack, nsegm[6];
  float *ptrf;

  if((ind = etNlink(iw,"PATH",0)) <= 0)
  {
    return;
  }
  ntrack = (etNdata(iw,ind)*sizeof(int))/sizeof(PRTRACK);
  track = (PRTRACK *)&iw[ind];

  /* copy results to SDA-RECSIS data structures */
  nhits = 0;
  trkcount = 0;
  for(itr=0; itr<ntrack; itr++)
  {
    trkptr = &track[itr];
    if(trkptr->sector != *sec) continue;

    itr_level[itr] = 1;
    itr_sect[itr]  = trkptr->sector;

    for(iv=0; iv<6; iv++) lnk_vect[itr][iv] = trkptr->vect[iv];
    /* correction !!! */
    lnk_vect[itr][0] = lnk_vect[itr][0] - 3.2;
    lnk_vect[itr][1] = lnk_vect[itr][1] + 0.8;

    lnk_vect[itr][6] = trkptr->p;

    lnk_vect[itr][7] = (float)trkptr->ntof;
    lnk_vect[itr][8] = (float)trkptr->nu / 2.;
    lnk_vect[itr][9] = (float)trkptr->nv / 2.;
    lnk_vect[itr][10] = (float)trkptr->nw / 2.;
    lnk_vect[itr][11] = (float)trkptr->charge;

    trkcount ++;
  }


  (*ntr_link) += trkcount;
  goto dc1;

exit:

  (*ntr_link) = itr;

dc1:
  /* make DC1 bank */

  /*printf("proutput: nhits=%d sec=%d\n",nhits,*sec);*/
  if(nhits)
  {
    etnformat_(iw,"DC1 ","I,F",4,3);
    tmp=2;
    if((ind = etncreate_(iw,"DC1 ",sec,&tmp,&nhits,4)) > 0)
    {
      /*printf(">>> sec=%d nhits=%d(bytes=%d) ind=%d\n",*sec,nhits,nhits*8,ind);*/
      ptri = (int *)&iw[ind];
      ptrf = (float *)&iw[ind+1];
      for(itr=0; itr<(*ntr_link); itr++)
      {
        trkptr = &track[itr];
        if(trkptr->sector != *sec) continue;
        for(is=0; is<6; is++)
        {
          clustptr = &trkptr->cluster[is];
          for(isg=0; isg<clustptr->nsegment; isg++)
          {
            sgmptr = &clustptr->segment[isg];
            for(i=0; i<6; i++)
            {
              if(sgmptr->iw[i] > 0)
              {
		
                *ptri = sgmptr->iw[i] + 256*(6*is + i+1);
                *ptrf = sgmptr->tdc[i];
		
                ptri+=2;
                ptrf+=2;
              }
            }
          }
        }
      }
      /*
      ptri = (int *)&iw[ind];
      ptrf = (float *)&iw[ind+1];
      for(i=0; i<nhits; i++)
      {
        printf(">>> %6d(iw=%3d  l=%3d %f\n",*ptri,(*ptri)&0xff,((*ptri)&0xff00)>>8,*ptrf);
        ptri+=2; ptrf+=2;
      }
      */
    }
  }

  return;
}

E 1
