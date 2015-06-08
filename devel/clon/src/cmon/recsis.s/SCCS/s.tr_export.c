h10601
s 00004/00000/00289
d D 1.5 02/05/09 14:38:44 boiarino 6 5
c define some etNformat's
c 
e
s 00002/00000/00287
d D 1.4 02/05/08 18:51:03 boiarino 5 4
c define HBTR/TBTR format
c 
e
s 00001/00006/00286
d D 1.3 02/04/30 12:53:03 boiarino 4 3
c some cleanup
c 
e
s 00004/00000/00288
d D 1.2 01/11/29 11:36:14 boiarino 3 1
c add etNformat ...
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 18:58:50 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/recsis.s/tr_export.c
e
s 00288/00000/00000
d D 1.1 01/11/19 18:58:49 boiarino 1 0
c date and time created 01/11/19 18:58:49 by boiarino
e
u
U
f e 0
t
T
I 1

/* tr_export.c - report results in a form for external packages (RECSIS etc) */

#include <stdio.h>
#include "prlib.h"
#include "dclib.h"

#define ncol_hbla 16
#define ncol_dpl   8
#define ncol_hbtr  9
#define ncol_tbla 17
#define ncol_tdpl  8
#define ncol_tbtr 10

void tr_makeTR(int *, TRevent *, int, int, float ev_out[ntrmx][36], float *);

void
tr_makeHBTR(int *jw, TRevent *ana, int ntr_out, float ev_out[ntrmx][36],
            float ev_outx[ntrmx])
{
  tr_makeTR(jw,ana,2,ntr_out,ev_out,ev_outx);
}

void
tr_makeTBTR(int *jw, TRevent *ana, int ntr_out, float ev_out[ntrmx][36],
            float ev_outx[ntrmx])
{
  tr_makeTR(jw,ana,4,ntr_out,ev_out,ev_outx);
}

void
tr_makeTR(int *jw, TRevent *ana, int Level,
          int ntr_out, float ev_out[ntrmx][36], float ev_outx[ntrmx])
{
  int ind0, ind1, ind2, n_dclay, n_planes, itrk;
  int ced_trk[6], ced_lnk[6][ntrmx], ced_itr[6][ntrmx];
  int i, ilnk, it, isec, il, itmp;
  float tmp, *rw;

  rw = (float *) jw;

  if(Level == 2)
  {
I 5
    etNformat(jw,"HBTR","F");
E 5
    ind0 = etNcreate(jw,"HBTR",0,ncol_hbtr,ntr_out);
  }
  else
  {
I 5
    etNformat(jw,"TBTR","F");
E 5
    ind0 = etNcreate(jw,"TBTR",0,ncol_tbtr,ntr_out);
  }

  if(ind0 <=0)
  {
    printf("tr_makeTR: level=%d ind0=%d\n",Level,ind0);
    return; /* must delete all banks here !!! */
  }

  /* count tracks in each sector */
  for(i=0; i<6; i++) ced_trk[i] = 0;
  for(it = 1; it<=ntr_out; it++)
  {
    ilnk = ev_out[it-1][9];         /* ilnk for track it*/
    isec = ana->track[ilnk-1].sect; /* sector# for ilnk */
    ced_trk[isec-1] ++;             /* track# in current sector */
    itmp = ced_trk[isec-1];
    ced_lnk[isec-1][itmp-1] = ilnk;
    ced_itr[isec-1][itmp-1] = it;   /* absolute track# */
  }

/* !!!!!!!!!!!!!!!!!!!!!!!!!! */
/* LEVEL 2 DOES NOT WORK -> ev_out[][] IS NOT FILLED FOR Level 2 (!!!???) */
/* !!!!!!!!!!!!!!!!!!!!!!!!!! */

  /* creates and fills banks for CED */
  for(isec=1; isec<=6; isec++)
  {
    if(ced_trk[isec-1] != 0)
D 4
	{
E 4
I 4
    {
E 4
      n_dclay = ced_trk[isec-1] * (npl_dc - 2);
      n_planes = ced_trk[isec-1] * (1 + npl_st + npl_cc + npl_sc + npl_ec);
      if(Level == 2)
      {
I 6
        etNformat(jw,"HBLA","F");
E 6
I 3
D 4
        etNformat(jw,"HBLA","F");
        etNformat(jw,"HDPL","F");
E 4
E 3
        if((ind1 = etNcreate(jw,"HBLA",isec,ncol_hbla,n_dclay))<=0)
        {
          printf("tr_makeTR: level=%d ind1=%d\n",Level,ind1);
          return; /* must delete all banks here !!! */
        }
I 6
        etNformat(jw,"HDPL","F");
E 6
        if((ind2 = etNcreate(jw,"HDPL",isec,ncol_dpl,n_planes))<=0)
        {
          printf("tr_makeTR: level=%d ind2=%d\n",Level,ind2);
          return; /* must delete all banks here !!! */
        }
      }
      else
      {
I 6
        etNformat(jw,"TBLA","F");
E 6
I 3
D 4
        etNformat(jw,"TBLA","F");
        etNformat(jw,"TDPL","F");
E 4
E 3
        if((ind1 = etNcreate(jw,"TBLA",isec,ncol_tbla,n_dclay))<=0)
        {
          printf("tr_makeTR: level=%d ind1=%d\n",Level,ind1);
          return; /* must delete all banks here !!! */
        }
I 6
        etNformat(jw,"TDPL","F");
E 6
        if((ind2 = etNcreate(jw,"TDPL",isec,ncol_tdpl,n_planes))<=0)
        {
          printf("tr_makeTR: level=%d ind2=%d\n",Level,ind2);
          return; /* must delete all banks here !!! */
        }
      }
      for(it=1; it<=ced_trk[isec-1]; it++)
      {
        int ipl;

        ilnk = ced_lnk[isec-1][it-1];
        for(il=1; il<=npl_dc; il++)
        {
          if(il < 5 || il > 6)
          {
            ipl = nst_max + il; /* layer for trkp[][][] */
            /*printf("isec=%d it=%d\n",isec,ced_itr[isec-1][it-1]);*/
            jw[ind1]   = ced_itr[isec-1][it-1]*100+ipl; /* 100*track# + plane# */
            rw[ind1+1] = ana->track[ilnk-1].trkp[ipl-1].y; /* position */
            rw[ind1+2] = ana->track[ilnk-1].trkp[ipl-1].z;
            rw[ind1+3] = ana->track[ilnk-1].trkp[ipl-1].x;
            /*printf("x,y,z = %f %f %f\n",rw[ind1+1],rw[ind1+2],rw[ind1+3]);*/
            rw[ind1+4] = ana->track[ilnk-1].trkp[ipl-1].Cy; /* direction */
            rw[ind1+5] = ana->track[ilnk-1].trkp[ipl-1].Cz;
            rw[ind1+6] = ana->track[ilnk-1].trkp[ipl-1].Cx;
            rw[ind1+7] = ana->track[ilnk-1].trk[il-1].s;   /* track length */
            jw[ind1+8] = 0; /*trk_pt[ilnk-1][il-1];*/ /* ptr to DC1 */
            jw[ind1+9] = (int)( ana->track[ilnk-1].trk[il-1].status ); /* Status */
            jw[ind1+10]= (int)( ana->track[ilnk-1].trk[il-1].iw ); /* wire# */
            rw[ind1+11]= ana->track[ilnk-1].trk[il-1].Td; /* drift time */
            rw[ind1+12]= ana->track[ilnk-1].trk[il-1].alfa; /* track angle in Layer */
            rw[ind1+13]= ana->track[ilnk-1].trk[il-1].Wlen; /* wire length to preamp. */
            rw[ind1+14]= ana->track[ilnk-1].trk[il-1].SigDm; /* Sigma doca */
            rw[ind1+15]= ana->track[ilnk-1].trk[il-1].Df; /* fitted doca */
            if(Level == 2)
            {
              ind1 = ind1 + ncol_hbla;
            }
            else
            {
              rw[ind1+16]= ana->track[ilnk-1].trk[il-1].Dm; /* calculated doca */
              ind1 = ind1 + ncol_tbla;
            }
          }
        }

        for(ipl=1; ipl<=npln; ipl++)
        {
          if(ipl <= nst_max ||
            (ipl >= ncc_min && ipl <= ncc_max) ||
            (ipl >= nsc_min && ipl <= nsc_max) ||
            (ipl >= nec_min && ipl <= nec_max))
          {
            jw[ind2]   = ced_itr[isec-1][it-1]*100+ipl; /* 100*track# + plane# */
            rw[ind2+1] = ana->track[ilnk-1].trkp[ipl-1].y; /* position */
            rw[ind2+2] = ana->track[ilnk-1].trkp[ipl-1].z;
            rw[ind2+3] = ana->track[ilnk-1].trkp[ipl-1].x;
            rw[ind2+4] = ana->track[ilnk-1].trkp[ipl-1].Cy; /* direction */
            rw[ind2+5] = ana->track[ilnk-1].trkp[ipl-1].Cz;
            rw[ind2+6] = ana->track[ilnk-1].trkp[ipl-1].Cx;
            rw[ind2+7] = ana->track[ilnk-1].trkp[ipl-1].s; /* track length */
            if(Level == 2)
            {
              ind2 = ind2 + ncol_dpl;
            }
            else
            {
              ind2 = ind2 + ncol_tdpl;
            }
          }
        }

        itmp = ced_itr[isec-1][it-1];
        rw[ind0  ] = ev_outx[itmp-1]; /*ev_out[itmp-1][0];*/
        rw[ind0+1] = ev_out[itmp-1][1]; /* Vertex {x,y,z} */
        rw[ind0+2] = ev_out[itmp-1][2];
        tmp = ev_out[itmp-1][6]; /* Momentum (GeV) */
        rw[ind0+3] = ev_out[itmp-1][3] *tmp;
        rw[ind0+4] = ev_out[itmp-1][4] *tmp; /* Momentum {Px,Py,Pz} */
        rw[ind0+5] = ev_out[itmp-1][5] *tmp;
        rw[ind0+6] = ev_out[itmp-1][7]; /* Charge */
        rw[ind0+7] = ev_out[itmp-1][11]; /* Chisquare */
        jw[ind0+8] = isec*100 + it;
        if(Level == 2)
        {
          ind0 = ind0 + ncol_hbtr;
        }
        else
        {
          jw[ind0+9] = 0; /* SHOULD BE track# for this track in the */
          ind0 = ind0 + ncol_tbtr;      /* Hit Based Tracking Banks */
        }
      }
    }
  }

  return;
}


/* must be redone as soon as .segment does not exist any more (you wish !) 
void
tr_makeDC1(int *jw)
{
  PRTRACK *trkptr, *track;
  PRCLUSTER *clustptr;
D 4
  PRSEGMENT *sgmptr;
E 4
  int i, j, itr, itr1, nhits[6], isec, is, iv, icl, isg, ncoin, trkcount, io;
  int *ptri, ind, tmp, ntrack, nsegm[6];
  float *ptrf;

  if((ind = etNlink(jw,"PATH",0)) <= 0)
  {
    return;
  }
  ntrack = (etNdata(jw,ind)*sizeof(int))/sizeof(PRTRACK);
  track = (PRTRACK *)&jw[ind];

  for(i=0; i<6; i++) nhits[i] = 0;
  for(itr=0; itr<ntrack; itr++)
  {
    trkptr = &track[itr];
    isec = trkptr->sector;
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
            nhits[isec-1] ++;
          }
        }
      }
    }
  }

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
    }
  }

  return;
}
*/










E 1
