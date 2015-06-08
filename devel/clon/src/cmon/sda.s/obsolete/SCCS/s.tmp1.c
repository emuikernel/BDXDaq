h09079
s 00000/00000/00000
d R 1.2 01/11/19 19:02:42 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/tmp1.c
e
s 00179/00000/00000
d D 1.1 01/11/19 19:02:41 boiarino 1 0
c date and time created 01/11/19 19:02:41 by boiarino
e
u
U
f e 0
t
T
I 1
/* TEMPORARY */

#include "dclib.h"

void tmp2ana_(SDAanal *ana, int *itrk0, float *beta0, int *itag0,
   int *ntagh, float *tzero, int *ntr_link,
   int itr_level[ntrmx], int itr_sect[ntrmx],
   int lnk_clust[ntrmx][6], int lnk_segm[ntrmx][6],
   int itr_scid[ntrmx][3], int itr_ecid[ntrmx][3],
   float trk[ntrmx][36][12], float trkp[ntrmx][nplane][12],
   int segm[6][6][nsgmx][12], int clust[6][6][nclmx][nsgcmx],
   int nclust[6][6], int nsegmc[6][6][nclmx],
   float tag_res[10][7], int *ntr_out, float ev_out[ntrmx][36],
   float lnk_vect[ntrmx][nlnkvect])
{
  int i,j,k,l,is;
  
  ana->itrk0 = *itrk0;
  ana->beta0 = *beta0;

  ana->itag0 = *itag0;
  ana->ntagh = *ntagh;
  for(i=0; i<10; i++)
    for(j=0; j<7; j++)
      ana->tag_res[i][j] = tag_res[i][j];

  ana->ntr_out = *ntr_out;

  for(i=0; i<ntrmx; i++)
    for(j=0; j<36; j++)
      ana->ev_out[i][j] = ev_out[i][j];

  ana->tzero = *tzero;

  ana->ntrack = *ntr_link;
  for(i=0; i<ana->ntrack; i++)
  {
    ana->track[i].level = itr_level[i];
    ana->track[i].sect  = itr_sect[i];
    for(j=0; j<nlnkvect; j++) ana->track[i].vect[j] = lnk_vect[i][j];

    for(j=0; j<6; j++)
    {
      ana->track[i].segm[j] = lnk_segm[i][j];
      ana->track[i].clust[j] = lnk_clust[i][j];
    }

    ana->track[i].scid.slab  = itr_scid[i][0];
    ana->track[i].scid.plane = itr_scid[i][1];
    ana->track[i].scid.hit   = itr_scid[i][2];

    ana->track[i].ecid.u = itr_ecid[i][0];
    ana->track[i].ecid.v = itr_ecid[i][1];
    ana->track[i].ecid.w = itr_ecid[i][2];

    for(j=0; j<36; j++)
	{
      ana->track[i].trk[j].iw     = trk[i][j][0];
      ana->track[i].trk[j].tdc    = trk[i][j][1];
      ana->track[i].trk[j].Td     = trk[i][j][2];
      ana->track[i].trk[j].Dm     = trk[i][j][3];
      ana->track[i].trk[j].SigDm  = trk[i][j][4];
      ana->track[i].trk[j].beta   = trk[i][j][5];
      ana->track[i].trk[j].Df     = trk[i][j][6];
      ana->track[i].trk[j].s      = trk[i][j][7];
      ana->track[i].trk[j].Wlen   = trk[i][j][8];
      ana->track[i].trk[j].alfa   = trk[i][j][9];
      ana->track[i].trk[j].sector = trk[i][j][10];
      ana->track[i].trk[j].status = trk[i][j][11];
    }

    for(j=0; j<nplane; j++)
    {
      ana->track[i].trkp[j].x      = trkp[i][j][0];
      ana->track[i].trkp[j].y      = trkp[i][j][1];
      ana->track[i].trkp[j].z      = trkp[i][j][2];
      ana->track[i].trkp[j].Cx     = trkp[i][j][3];
      ana->track[i].trkp[j].Cy     = trkp[i][j][4];
      ana->track[i].trkp[j].Cz     = trkp[i][j][5];
      ana->track[i].trkp[j].p      = trkp[i][j][6];
      ana->track[i].trkp[j].beta   = trkp[i][j][7];
      ana->track[i].trkp[j].q      = trkp[i][j][8];
      ana->track[i].trkp[j].s      = trkp[i][j][9];
      ana->track[i].trkp[j].sector = trkp[i][j][10];
      ana->track[i].trkp[j].chisq  = trkp[i][j][11];
    }

  }  

  return;
}

void ana2tmp_(SDAanal *ana, int *itrk0, float *beta0, int *itag0,
   int *ntagh, float *tzero, int *ntr_link,
   int itr_level[ntrmx], int itr_sect[ntrmx],
   int lnk_clust[ntrmx][6], int lnk_segm[ntrmx][6],
   int itr_scid[ntrmx][3], int itr_ecid[ntrmx][3],
   float trk[ntrmx][36][12], float trkp[ntrmx][nplane][12],
   int segm[6][6][nsgmx][12], int clust[6][6][nclmx][nsgcmx],
   int nclust[6][6], int nsegmc[6][6][nclmx],
   float tag_res[10][7], int *ntr_out, float ev_out[ntrmx][36],
   float lnk_vect[ntrmx][nlnkvect])
{
  int i,j,k,l,is;

  *itrk0 = ana->itrk0;
  *beta0 = ana->beta0;

  *itag0 = ana->itag0;
  *ntagh = ana->ntagh;
  for(i=0; i<10; i++)
    for(j=0; j<7; j++)
      tag_res[i][j] = ana->tag_res[i][j];

  *ntr_out = ana->ntr_out;
  for(i=0; i<ntrmx; i++)
    for(j=0; j<36; j++)
      ev_out[i][j] = ana->ev_out[i][j];

  *tzero = ana->tzero;

  *ntr_link = ana->ntrack;
  for(i=0; i<ana->ntrack; i++)
  {
    itr_level[i] = ana->track[i].level;
    itr_sect[i] = ana->track[i].sect;
    for(j=0; j<nlnkvect; j++) lnk_vect[i][j] = ana->track[i].vect[j];
    for(j=0; j<6; j++)
    {
      lnk_segm[i][j] = ana->track[i].segm[j];
      lnk_clust[i][j] = ana->track[i].clust[j];
    }

    itr_scid[i][0] = ana->track[i].scid.slab;
    itr_scid[i][1] = ana->track[i].scid.plane;
    itr_scid[i][2] = ana->track[i].scid.hit;

    itr_ecid[i][0] = ana->track[i].ecid.u;
    itr_ecid[i][1] = ana->track[i].ecid.v;
    itr_ecid[i][2] = ana->track[i].ecid.w;

    for(j=0; j<36; j++)
	{
      trk[i][j][0]  = ana->track[i].trk[j].iw;
      trk[i][j][1]  = ana->track[i].trk[j].tdc;
      trk[i][j][2]  = ana->track[i].trk[j].Td;
      trk[i][j][3]  = ana->track[i].trk[j].Dm;
      trk[i][j][4]  = ana->track[i].trk[j].SigDm;
      trk[i][j][5]  = ana->track[i].trk[j].beta;
      trk[i][j][6]  = ana->track[i].trk[j].Df;
      trk[i][j][7]  = ana->track[i].trk[j].s;
      trk[i][j][8]  = ana->track[i].trk[j].Wlen;
      trk[i][j][9]  = ana->track[i].trk[j].alfa;
      trk[i][j][10] = ana->track[i].trk[j].sector;
      trk[i][j][11] = ana->track[i].trk[j].status;
    }

    for(j=0; j<nplane; j++)
    {
      trkp[i][j][0]  = ana->track[i].trkp[j].x;
      trkp[i][j][1]  = ana->track[i].trkp[j].y;
      trkp[i][j][2]  = ana->track[i].trkp[j].z;
      trkp[i][j][3]  = ana->track[i].trkp[j].Cx;
      trkp[i][j][4]  = ana->track[i].trkp[j].Cy;
      trkp[i][j][5]  = ana->track[i].trkp[j].Cz;
      trkp[i][j][6]  = ana->track[i].trkp[j].p;
      trkp[i][j][7]  = ana->track[i].trkp[j].beta;
      trkp[i][j][8]  = ana->track[i].trkp[j].q;
      trkp[i][j][9]  = ana->track[i].trkp[j].s;
      trkp[i][j][10] = ana->track[i].trkp[j].sector;
      trkp[i][j][11] = ana->track[i].trkp[j].chisq;
    }

  }  

  return;
}

/* TEMPORARY */
E 1
