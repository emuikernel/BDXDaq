h34847
s 00003/00003/00770
d D 1.3 07/11/03 11:44:54 boiarino 4 3
c *** empty log message ***
e
s 00009/00007/00764
d D 1.2 02/04/30 14:08:15 boiarino 3 1
c adjust for the new segment finding
c 
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 19:05:58 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/dcdsect.c
e
s 00771/00000/00000
d D 1.1 01/11/19 19:05:57 boiarino 1 0
c date and time created 01/11/19 19:05:57 by boiarino
e
u
U
f e 0
t
T
I 1
/*
   Purpose and Methods : draw sector gometry, tracks and hits (dirty stuff)

   Inputs  : isec -sector number
             sxy     -scale factor
             x0,y0   -position in HIGZ window of size 20x20 cm2
   Output  :
   Controls:

   Library belongs: libsda.a

   Calls: none

   Created   2-OCT-1991   Billy K. Leung
   Modified 15-DEC-1993   Bogdan B. Niczyporuk (for new vs. of SDA)
   C remake by Sergey Boyarinov

   Called by SDA_ENTER
*/

#include <stdio.h>
#include <math.h>

#include "dclib.h"
#include "prlib.h"
#include "sdageom.h"
#include "sdadraw.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define nsup          6
#define nlay          6
#define max_nwire   194
#define max_nhit 36*192

D 3
#include "prlib.h"

E 3
void
dcdsect_(int *jw, TRevent *ana, int *isec, float *sxy,float *x0,float *y0)
{
  dcdsect(jw, ana, *isec, *sxy, *x0, *y0);
}

void
dcdsect(int *jw, TRevent *ana, int isec, float sxy, float x0, float y0)
{
  PRTRACK *road;
  int i,j,k,nd,il,ip,iw,it,isc,ind,ig, nw,lmax,nwire,nwmin,nwmax,
D 4
      Nplane,j_plane, nstepm, npoint, nhit, indb, ind_sc[48][2],
E 4
I 4
      Nplane,j_plane, nstepm, npoint, nhit, indb, ind_sc[57][2],
E 4
      ifirst, ibgrd,ired,igreen,iblue,ipurp,ilblue, nslab[6], la;

  float *xmax, *ymax, deltaR, guardr, phi, degrad,
        phi_plane, x_plane, y_plane, delta_pl, xlast,ylast, xfirst,
        yfirst, phimin,phimax, phi25,d_out,dphi,Ndiv, phiort,
        x,y, x1,y1,x2,y2,x3,y3,x4,y4, dist,din,dout,d20, x00[3],
        f_s, fox, rad, angle,angle1,angle2, sas,sac;

  float x_hit[mxstep], y_hit[mxstep], x_bgrd[max_nhit],
        y_bgrd[max_nhit], gwire_x[max_nwire], gwire_y[max_nwire];

  char legend[2];


  /* if(first) */
  {
    xmax = &sdadraw_.sda_wc[1];
    ymax = &sdadraw_.sda_wc[3];
    nslab[0] = sdageom_.pln[isec-1][nsc_min-1][17];
    nslab[1] = sdageom_.pln[isec-1][nsc_min+1-1][17];
    nslab[2] = sdageom_.pln[isec-1][nsc_min+2-1][17];
    nslab[3] = sdageom_.pln[isec-1][nsc_min+2-1][18];
    nslab[4] = sdageom_.pln[isec-1][nsc_min+3-1][17];
    nslab[5] = sdageom_.pln[isec-1][nsc_min+3-1][18];
    isc = 0;
    for(ip=1; ip<=6; ip++)
	{
      for(i=1; i<=nslab[ip-1]; i++)
	  {
        isc ++;
        ind_sc[isc-1][0] = i;
        ind_sc[isc-1][1] = ip;
      }
    }
    ibgrd  = 1;
    ired   = 2;
    igreen = 3;
    iblue  = 4;
    ipurp  = 6;
    ilblue = 7;
    degrad = acos(-1.)/180.;
  }

  /* Set up a view box for the convenience of hard_copy output */
  ISPMCI(ibgrd);
  ISPLCI(ibgrd);
  if(ABS(sxy) != 1.) /* ????????????????????????????????????? */
  {
    IGBOX(0.,(*xmax),0.05,0.77*(*ymax));
    sprintf(legend,"%2d",isec);
    IGTEXT((*xmax)*.05,(*ymax)*.75,
      "SDA Program   Middle Plane View of the CLAS Detector",
      (*ymax)/100.,0.,"L");
	/* X-/Y- axes for the mid-plane */
    IGAXIS(x0,16.,y0,y0,0.,1.,0,"AU");
    IGTEXT(16.2,y0,"Xs  (beam)",(*ymax)/50.,0.,"L");
    IGAXIS(x0,x0,y0,12.,0.,1.,0,"AU");
    IGTEXT(x0,12.2,"Ys",(*ymax)/50.,0.,"L");
  }

  /* Draw geometry of ST (Start Counter) */

  if(sdakeys_.zbeam[0] == 0.)
  {
    x_hit[0] = sdageom_.pln[isec-1][1][13]*ABS(sxy)+x0;
    y_hit[0] = sdageom_.pln[isec-1][1][14]*sxy + y0;
    x_hit[1] = sdageom_.pln[isec-1][1][11]*ABS(sxy)+x0;
    y_hit[1] = sdageom_.pln[isec-1][1][12]*sxy + y0;
    x_hit[2] = sdageom_.pln[isec-1][2][11]*ABS(sxy) + x0;
    y_hit[2] = sdageom_.pln[isec-1][2][12]*sxy + y0;
    IPL(3,x_hit,y_hit);
  }

  /* Draw geometry of DC */

  ind = 0;
  for(i=1; i<=nsup; i++)
  {
    il = (i-1)*nlay + 1 + nst_max;
    deltaR = 0.866025*sdageom_.pln[isec-1][il+2-1][6]
                     *sdageom_.pln[isec-1][il+2-1][12];
    guardr = sdageom_.pln[isec-1][il-1][6] - deltaR;
    if(i < 3)
	{
      j = 0;
      phi_plane = 0.;
      while(phi_plane < 90.*degrad)
	  {
        j ++;
        phi_plane = sdageom_.pln[isec-1][il-1][11]
                  + (j-1)*sdageom_.pln[isec-1][il-1][12];
      }
      j_plane = j;
      x_plane = guardr*cos(phi_plane) + sdageom_.pln[isec-1][il-1][0];
      y_plane = guardr + sdageom_.pln[isec-1][il-1][1];
      delta_pl= guardr*sdageom_.pln[isec-1][il-1][12];
      nwire = sdageom_.pln[isec-1][il-1][13];
      nwmin = sdageom_.pln[isec-1][il-1][14];
      nwmax = sdageom_.pln[isec-1][il-1][13];
      nw = 0;
      for(j=1; j<=nwire; j++)
	  {
        if(j >= nwmin && j <= nwmax)
		{
          nw ++;
          phi = sdageom_.pln[isec-1][il-1][11]
              + (j-1)*sdageom_.pln[isec-1][il-1][12];
          if(phi < 90.*degrad)
		  {
            gwire_x[nw-1] = guardr*cos(phi)+sdageom_.pln[isec-1][il-1][0];
            gwire_y[nw-1] = guardr*sin(phi)+sdageom_.pln[isec-1][il-1][1];
		  }
          else
		  {
            gwire_x[nw-1] = x_plane - (j - j_plane)*delta_pl;
            gwire_y[nw-1] = guardr + sdageom_.pln[isec-1][il-1][1];
		  }
          gwire_x[nw-1] = gwire_x[nw-1] * ABS(sxy) + x0;
          gwire_y[nw-1] = gwire_y[nw-1] * sxy + y0;
		}
	  }
      IPL(nw, gwire_x,gwire_y);
      xlast = gwire_x[nw-1];
      ylast = gwire_y[nw-1];
      xfirst = gwire_x[0];
      yfirst = gwire_y[0];
      lmax = 5;
      if(i == 1) lmax = 3;
      guardr = sdageom_.pln[isec-1][il+lmax-1][6] + deltaR;
      nwmin = sdageom_.pln[isec-1][il+lmax-1][14];
      nwmax = sdageom_.pln[isec-1][il+lmax-1][13];
      nw = 0;
      for(j=1; j<=nwire; j++)
	  {
        if(j >= nwmin && j <= nwmax)
		{
          nw ++;
          phi = sdageom_.pln[isec-1][il+lmax-1][11]
              + (j-1)*sdageom_.pln[isec-1][il+lmax-1][12];
          if(phi < 90.*degrad)
		  {
            gwire_x[nw-1] = guardr*cos(phi) + sdageom_.pln[isec-1][il-1][0];
            gwire_y[nw-1] = guardr*sin(phi) + sdageom_.pln[isec-1][il-1][1];
		  }
          else
		  {
            gwire_x[nw-1] = x_plane - (j - j_plane)*delta_pl;
            gwire_y[nw-1] = guardr + sdageom_.pln[isec-1][il-1][1];
		  }
          gwire_x[nw-1] = gwire_x[nw-1]*ABS(sxy) + x0;
          gwire_y[nw-1] = gwire_y[nw-1]*sxy + y0;
		}
	  }
      IPL(nw,gwire_x,gwire_y);
      gwire_x[1] = xfirst;
      gwire_y[1] = yfirst;
      IPL(2,gwire_x,gwire_y);
      gwire_x[0] = gwire_x[nw-1];
      gwire_y[0] = gwire_y[nw-1];
      gwire_x[1] = xlast;
      gwire_y[1] = ylast;
      IPL(2,gwire_x,gwire_y);
	}
    else
	{
      for(j=1; j<=max_nwire; j++)
	  {
        phi = sdageom_.pln[isec-1][il-1][11]
            + (j-1)*sdageom_.pln[isec-1][il-1][12];
        gwire_x[j-1] = guardr*cos(phi) + sdageom_.pln[isec-1][il-1][0];
        gwire_y[j-1] = guardr*sin(phi) + sdageom_.pln[isec-1][il-1][1];
        gwire_x[j-1] = gwire_x[j-1] * ABS(sxy) + x0;
        gwire_y[j-1] = gwire_y[j-1] * sxy + y0;
	  }
      IPL(max_nwire,gwire_x,gwire_y);
      xlast = gwire_x[max_nwire-1];
      ylast = gwire_y[max_nwire-1];
      xfirst = gwire_x[0];
      yfirst = gwire_y[0];
      guardr = sdageom_.pln[isec-1][il+lmax-1][6] + deltaR;
      for(j=1; j<=max_nwire; j++)
	  {
        phi = sdageom_.pln[isec-1][il+lmax-1][11]
            + (j-1)*sdageom_.pln[isec-1][il+lmax-1][12];
        gwire_x[j-1] = guardr*cos(phi) + sdageom_.pln[isec-1][il+lmax-1][0];
        gwire_y[j-1] = guardr*sin(phi) + sdageom_.pln[isec-1][il+lmax-1][1];
        gwire_x[j-1] = gwire_x[j-1] * ABS(sxy) + x0;
        gwire_y[j-1] = gwire_y[j-1] * sxy + y0;
	  }
      IPL(max_nwire,gwire_x,gwire_y);
      gwire_x[1] = xfirst;
      gwire_y[1] = yfirst;
      IPL(2,gwire_x,gwire_y);
      gwire_x[0] = gwire_x[max_nwire-1];
      gwire_y[0] = gwire_y[max_nwire-1];
      gwire_x[1] = xlast;
      gwire_y[1] = ylast;
      IPL(2,gwire_x,gwire_y);
	}
  }

  /* Draw geometry of CC */

  d_out  = sdageom_.pln[isec-1][ncc_min-1][7];
  phi25  = atan2(sdageom_.pln[isec-1][ncc_min-1][4],
                 sdageom_.pln[isec-1][ncc_min-1][3]);
  phimin =  7.*degrad;
  phimax = 45.*degrad;
  Ndiv   = 100.;

  dphi = (sdageom_.pln[isec-1][ncc_min-1][16]
        - sdageom_.pln[isec-1][ncc_min-1][15]) / Ndiv;
  phi  = sdageom_.pln[isec-1][ncc_min-1][15];
  ind  = 0;
  k    = Ndiv + 1.;
  for(i=1; i<=k; i++)
  {
    ind ++;
    x = sdageom_.pln[isec-1][ncc_min-1][6]*cos(phi)
      + sdageom_.pln[isec-1][ncc_min-1][0];
    y = sdageom_.pln[isec-1][ncc_min-1][6]*sin(phi)
      + sdageom_.pln[isec-1][ncc_min-1][1];
    x_hit[ind-1] = x * ABS(sxy) + x0;
    y_hit[ind-1] = y * sxy      + y0;
    phi = phi + dphi;
  }
  ISPLCI(ibgrd);
  IPL(ind,x_hit,y_hit);

  x1  = x_hit[0];
  y1  = y_hit[0];
  x2  = x_hit[ind-1];
  y2  = y_hit[ind-1];
  d20 = d_out / cos(phi25-phimin);
  x3  = (d20*cos(phimin) + sdageom_.pln[isec-1][nec_min-1][0])*ABS(sxy)
       + x0;
  y3  = (d20*sin(phimin) + sdageom_.pln[isec-1][nec_min-1][1])*sxy + y0;
  d20 = d_out / cos(phimax-phi25);
  x4  = (d20*cos(phimax) + sdageom_.pln[isec-1][nec_min-1][0])*ABS(sxy)
        + x0;
  y4  = (d20*sin(phimax) + sdageom_.pln[isec-1][nec_min-1][1])*sxy + y0;
   x_hit[0] = x3;
   x_hit[1] = x4;
   y_hit[0] = y3;
   y_hit[1] = y4;
   IPL(2,x_hit,y_hit);
  x_hit[0] = x1;
  x_hit[1] = x3;
  y_hit[0] = y1;
  y_hit[1] = y3;
  IPL(2,x_hit,y_hit);
   x_hit[0] = x2;
   x_hit[1] = x4;
   y_hit[0] = y2;
   y_hit[1] = y4;
   IPL(2,x_hit,y_hit);

  /* Draw geometry of SC */

  for(isc=1; isc<=6; isc++)
  {
    k = nslab[isc-1] + 1;
    x_hit[0] = sdageom_.sc_grid[isec-1][isc-1][0][0] * ABS(sxy) + x0;
    x_hit[1] = sdageom_.sc_grid[isec-1][isc-1][k-1][0] * ABS(sxy) + x0;
    y_hit[0] = sdageom_.sc_grid[isec-1][isc-1][0][1] * sxy + y0;
    y_hit[1] = sdageom_.sc_grid[isec-1][isc-1][k-1][1] * sxy + y0;
    IPL(2,x_hit,y_hit);
    x_hit[0] = sdageom_.sc_grid[isec-1][isc-1][0][2] * ABS(sxy) + x0;
    x_hit[1] = sdageom_.sc_grid[isec-1][isc-1][k-1][2] * ABS(sxy) + x0;
    y_hit[0] = sdageom_.sc_grid[isec-1][isc-1][0][3] * sxy + y0;
    y_hit[1] = sdageom_.sc_grid[isec-1][isc-1][k-1][3] * sxy + y0;
    IPL(2,x_hit,y_hit);
	/* Draw division between slabs */
    for(i=1; i<=k; i++)
	{
      x_hit[0] = sdageom_.sc_grid[isec-1][isc-1][i-1][0] * ABS(sxy) + x0;
      x_hit[1] = sdageom_.sc_grid[isec-1][isc-1][i-1][2] * ABS(sxy) + x0;
      y_hit[0] = sdageom_.sc_grid[isec-1][isc-1][i-1][1] * sxy + y0;
      y_hit[1] = sdageom_.sc_grid[isec-1][isc-1][i-1][3] * sxy + y0;
      IPL(2,x_hit,y_hit);
    }
  }

  /* Draw geometry of EC */

  for(ip=nec_min; ip<=nec_max; ip++)
  {
    /* Offset center */
    x00[0] = sdageom_.pln[isec-1][ip-1][0];
    x00[1] = sdageom_.pln[isec-1][ip-1][1];
    x00[2] = sdageom_.pln[isec-1][ip-1][1];

    din    = sdageom_.pln[isec-1][ip-1][7];
    dout   = din + (sdageom_.pln[isec-1][ip-1][18]
            + sdageom_.pln[isec-1][ip-1][19])
            * sdageom_.pln[isec-1][ip-1][15];
    phiort = atan2(sdageom_.pln[isec-1][ip-1][4],
                   sdageom_.pln[isec-1][ip-1][3]);
    if(ip == nec_min)
	{
      phimin = phiort - 20.*degrad;
      phimax = phiort + 20.*degrad;
	}
    else
	{
      phimin = atan2(sdageom_.pln[isec-1][ip-1][12],
                     sdageom_.pln[isec-1][ip-1][11]);
      phimax = atan2(sdageom_.pln[isec-1][ip-1][14],
                     sdageom_.pln[isec-1][ip-1][13]);
    }
     dphi = ABS(phimin - phiort);
     x1 = (din * (cos(phimin) / cos(dphi)) + x00[0]) * ABS(sxy) + x0;
     y1 = (din * (sin(phimin) / cos(dphi)) + x00[1]) *     sxy  + y0;
     x3 = (dout* (cos(phimin) / cos(dphi)) + x00[0]) * ABS(sxy) + x0;
     y3 = (dout* (sin(phimin) / cos(dphi)) + x00[1]) *     sxy  + y0;
    dphi = ABS(phimax - phiort);
    x2 = (din * (cos(phimax) / cos(dphi)) + x00[0]) * ABS(sxy) + x0;
    y2 = (din * (sin(phimax) / cos(dphi)) + x00[1]) *     sxy  + y0;
    x4 = (dout* (cos(phimax) / cos(dphi)) + x00[0]) * ABS(sxy) + x0;
    y4 = (dout* (sin(phimax) / cos(dphi)) + x00[1]) *     sxy  + y0;
    if(ip == nec_max && isec > 2) continue;
     x_hit[0] = x1;
     x_hit[1] = x2;
     y_hit[0] = y1;
     y_hit[1] = y2;
     IPL(2,x_hit,y_hit);
     x_hit[0] = x3;
     x_hit[1] = x4;
     y_hit[0] = y3;
     y_hit[1] = y4;
     IPL(2,x_hit,y_hit);
     x_hit[0] = x1;
     x_hit[1] = x3;
     y_hit[0] = y1;
     y_hit[1] = y3;
     IPL(2,x_hit,y_hit);
     x_hit[0] = x2;
     x_hit[1] = x4;
     y_hit[0] = y2;
     y_hit[1] = y4;
     IPL(2,x_hit,y_hit);
  }
 
  /* Plot hits in DC (digitalization) */

  {
    int dcndig[36], dcdigi[36][192][2];
    GETDCDATA;

    indb = 0;
    for(i=1; i<=npl_dc; i++)
    {
      nhit = dcndig[i-1];
      for(j=1; j<=nhit; j++)
      {
        iw = dcdigi[i-1][j-1][0];
        if(indb < max_nhit)
	    {
          indb ++;
          x_bgrd[indb-1] = sdageom_.dc_wpmid[isec-1][i-1][iw-1][0] * ABS(sxy) + x0;
          y_bgrd[indb-1] = sdageom_.dc_wpmid[isec-1][i-1][iw-1][1] * sxy + y0;
	    }
	  }
    }
    if(indb > 0)
    {
      ISPMCI(ired);
      ISMK(5);
      IPM(indb,x_bgrd,y_bgrd);
    }
  }

  /* Draw pattern recognition roads */

  if((ind = etNlink(jw,"PATH",0)) > 0)
  {
    int nroad;

    road = (PRTRACK *)&jw[ind];
    nroad = (etNdata(jw,ind)*sizeof(int))/sizeof(PRTRACK);
    for(it=0; it<nroad; it++)
    {
      if(road[it].sector == isec)
      {
        int is;
        /*printf("sec = %1d\n",isec);*/
        for(is=0; is<6; is++)
        {
          int ll, iw;
I 3
          la = -1;
E 3
          for(ll=0; ll<NLAY; ll++)
          {
            if(road[it].cluster[is].layer[ll].nhit > 0)
            {
              iw = road[it].cluster[is].layer[ll].iw[0];
              la = ll;
              break;
            }
          }
D 3
          il = is*NLAY + la;
          x_hit[is] = sdageom_.dc_wpmid[isec-1][il][iw-1][0]*ABS(sxy)+x0;
          y_hit[is] = sdageom_.dc_wpmid[isec-1][il][iw-1][1]*sxy+y0;
          /*printf("  sl = %1d,  layer = %2d,  wire = %d, x=%f, y=%f\n",
              is+1,la+1,iw,x_hit[is],y_hit[is]);*/
E 3
I 3
          if(la>=0)
          {
            il = is*NLAY + la;
            x_hit[is] = sdageom_.dc_wpmid[isec-1][il][iw-1][0]*ABS(sxy)+x0;
            y_hit[is] = sdageom_.dc_wpmid[isec-1][il][iw-1][1]*sxy+y0;
            /*printf("  sl = %1d,  layer = %2d,  wire = %d, x=%f, y=%f\n",
                is+1,la+1,iw,x_hit[is],y_hit[is]);*/
          }
E 3
        }
        ISPLCI(igreen);
        IPL(6,x_hit,y_hit);
      }
    }
  }

  /* Plot hits in SC (digitalization) */

  {
D 4
    int scndig, scdigi[48][5];
E 4
I 4
    int scndig, scdigi[57][5];
E 4
    GETSCDATA;

    ISPLCI(ipurp);
    nhit = scndig;
    for(j=1; j<=nhit; j++)
    {
      if(scdigi[j-1][1] == 0 && scdigi[j-1][3] == 0) continue;
      isc = scdigi[j-1][0];
D 4
      if(isc < 1 || isc > 48) continue;
E 4
I 4
      if(isc < 1 || isc > 57) continue;
E 4
      ig = ind_sc[isc-1][0];
      ip = ind_sc[isc-1][1];
      x_hit[0] = sdageom_.sc_grid[isec-1][ip-1][ig-1  ][0] * ABS(sxy) + x0;
      x_hit[1] = sdageom_.sc_grid[isec-1][ip-1][ig+1-1][2] * ABS(sxy) + x0;
      y_hit[0] = sdageom_.sc_grid[isec-1][ip-1][ig-1  ][1] * sxy + y0;
      y_hit[1] = sdageom_.sc_grid[isec-1][ip-1][ig+1-1][3] * sxy + y0;
      IPL(2,x_hit,y_hit);
      x_hit[0] = sdageom_.sc_grid[isec-1][ip-1][ig-1  ][2] * ABS(sxy) + x0;
      x_hit[1] = sdageom_.sc_grid[isec-1][ip-1][ig+1-1][0] * ABS(sxy) + x0;
      y_hit[0] = sdageom_.sc_grid[isec-1][ip-1][ig-1  ][3] * sxy + y0;
      y_hit[1] = sdageom_.sc_grid[isec-1][ip-1][ig+1-1][1] * sxy + y0;
      IPL(2,x_hit,y_hit);
    }
  }

  /* Draw HBT trajectories */

  for(it=1; it<=sdadraw_.ntrdraw0; it++)
  {
    if(isec == sdadraw_.draw0_isec[it-1])
	{
      nstepm = sdadraw_.draw0_nstep[it-1];
      for(i=1; i<=nstepm; i++)
	  {
        npoint = i;
        x_hit[npoint-1] = sdadraw_.draw0_xyz[it-1][i-1][0] * ABS(sxy) + x0;
        y_hit[npoint-1] = sdadraw_.draw0_xyz[it-1][i-1][1] * sxy + y0;
	  }
      ISPLCI(ired);
      IPL(npoint,x_hit,y_hit);
      if(nstepm >= 1)
	  {
        sprintf(legend,"%2d",it);
        IGTEXT(x_hit[nstepm*3/4-1],y_hit[nstepm*3/4-1],legend,
           20.*ABS(sxy),0.,"L");
      }

	  /* Plot hits in 2xST, CC, 4xSC and 2xEC (total 9 planes) */
      Nplane = 9;
      ind = 0;
      for(ip=1; ip<=Nplane; ip++)
	  {
        if(ip == Nplane && isec > 2) continue;
        if(ip <= 2 && sdakeys_.zbeam[0] != 0.) continue; /* ????????????? */
        if(ip >= 4 && ip <= 7) continue;
        if(sdadraw_.draw0_hit[it-1][ip-1][1] > 999.) continue;
        if(ip <= 2)
        {
          k = ip + 1;
        }
        else
        { 
          k = ip + 37;          
        }
        ind ++;
        x_hit[ind-1] = sdadraw_.draw0_hit[it-1][ip-1][0] * ABS(sxy) + x0;
        y_hit[ind-1] = sdadraw_.draw0_hit[it-1][ip-1][1] * sxy      + y0;
	  }
      if(ind > 0)
	  {
        ISPMCI(igreen);
        ISMK(2);
        IPM(ind,x_hit,y_hit);
	  }
    } /* IF(isec ... */
  } /* EndDo over ntrdraw0 */


  /* Draw final trajectories (TBT) */

  for(it=1; it<=sdadraw_.ntrdraw; it++)
  {
    if(isec == sdadraw_.draw_isec[it-1])
	{
      nstepm = sdadraw_.draw_nstep[it-1];
      for(i=1; i<=nstepm; i++)
	  {
        npoint = i;
        x_hit[npoint-1] = sdadraw_.draw_xyz[it-1][i-1][0] * ABS(sxy) + x0;
        y_hit[npoint-1] = sdadraw_.draw_xyz[it-1][i-1][1] * sxy + y0;
	  }
      ISPLCI(iblue);
      IPL(npoint,x_hit,y_hit);
      if(nstepm >= 1)
	  {
        sprintf(legend,"%2d",it);
        IGTEXT(x_hit[nstepm*3/4-1],y_hit[nstepm*3/4-1],legend,
           20.*ABS(sxy),0.,"L");
      }

	  /* Plot hits in 2xST, CC, 4xSC and 2xEC (total 9 planes) */
      Nplane = 9;
      ind = 0;
      for(ip=1; ip<=Nplane; ip++)
	  {
        if(ip == Nplane && isec > 2) continue;
        if(ip <= 2 && sdakeys_.zbeam[0] != 0.) continue; /* ????????????? */
        if(ip >= 4 && ip <= 7) continue;
        if(sdadraw_.draw_hit[it-1][ip-1][1] > 999.) continue;
        if(ip <= 2)
        {
          k = ip + 1;
        }
        else
        { 
          k = ip + 37;          
        }
        ind ++;
        x_hit[ind-1] = sdadraw_.draw_hit[it-1][ip-1][0] * ABS(sxy) + x0;
        y_hit[ind-1] = sdadraw_.draw_hit[it-1][ip-1][1] * sxy      + y0;
	  }
      if(ind > 0)
	  {
        ISPMCI(ipurp);
        ISMK(2);
        IPM(ind,x_hit,y_hit);
	  }
    } /* IF(isec ... */
  } /* EndDo over ntrdraw */


  /* Draw Front View for the sector */

  if(ABS(sxy) != 1.) /* Not drawing Front View for sxy=1. */ /* ?????????? */
  {
    ISPMCI(ibgrd);
    ISPLCI(ibgrd);
    f_s    = 0.3;                   /* Additional scale factor */
    fox    = -300. * ABS(sxy);      /* X offset (no Y offset) */
    dist   = 300.;                  /* Dist from beam to the outline */

    rad    = dist / cos(30.*degrad);
    angle  = (isec-1)*60.*degrad;
    angle1 = ((isec-1)*60. - 30.)*degrad;
    angle2 = ((isec-1)*60. + 30.)*degrad;
    sas    = sin((isec-1)*60.*degrad);
    sac    = cos((isec-1)*60.*degrad);
    x_hit[0] = x0 + fox;
    y_hit[0] = y0;
    x_hit[1] = rad*cos(angle1)*f_s*ABS(sxy) + x0 + fox;
    y_hit[1] = rad*sin(angle1)*f_s*ABS(sxy) + y0;
    x_hit[2] = rad*cos(angle2)*f_s*ABS(sxy) + x0 + fox;
    y_hit[2] = rad*sin(angle2)*f_s*ABS(sxy) + y0;
    x_hit[3] = x0 + fox;
    y_hit[3] = y0;
    IPL(4,x_hit,y_hit);
    x_hit[0] = 1.15*rad*cos(angle)*f_s*ABS(sxy) + x0 + fox;
    y_hit[0] = 1.15*rad*sin(angle)*f_s*ABS(sxy) + y0;
    sprintf(legend,"%2d",isec);
    IGTEXT(x_hit[0],y_hit[0],legend,30.*ABS(sxy),0.,"C");

  /* Draw final trajectories (FRONT VIEW) */

    ISPMCI(ilblue);
    ISPLCI(ilblue);
    for(it=1; it<=sdadraw_.ntrdraw; it++)
	{
      if(isec == sdadraw_.draw_isec[it-1])
	  {
        nstepm = sdadraw_.draw_nstep[it-1];
        for(i=1; i<=nstepm; i++)
		{
          npoint = i;
          x_hit[npoint-1] = sdadraw_.draw_xyz[it-1][i-1][1] * sac -
                            sdadraw_.draw_xyz[it-1][i-1][2] * sas;
          y_hit[npoint-1] = sdadraw_.draw_xyz[it-1][i-1][1] * sas +
                            sdadraw_.draw_xyz[it-1][i-1][2] * sac;
          x_hit[npoint-1] = x_hit[npoint-1] * f_s * ABS(sxy) + x0 + fox;
          y_hit[npoint-1] = y_hit[npoint-1] * f_s * ABS(sxy) + y0;
		}
        IPL(npoint,x_hit,y_hit);
        if(nstepm >= 1)
		{
          sprintf(legend,"%2d",it);
          IGTEXT(x_hit[nstepm-1],y_hit[nstepm-1],legend,20.*ABS(sxy),0.,"L");
        }
	  }
	}
  }

  ISPMCI(ibgrd);
  ISPLCI(ibgrd);

  /* Draw a scale factor equivalent to 100cm on the bottom left corner. */

  x1 = 1.;
  x2 = x1 + ABS(sxy)*100.;
  if(ABS(sxy) != 1.) /* ?????????????????????????? */
  {
    IGSET("TMSI",(*ymax)/140);
    IGAXIS(x1,x2,.2,.2,0.,100.,502,"US");
    IGTEXT(x2,.4,"100 cm",(*ymax)/140.,0.,"L");
  }
  if(ABS(sxy) == 1.) /* ?????????????????????????? */
  {
    x1 = 300. + x0;
    x2 = x1 + 100. + x0;
    IGSET("TMSI",20.);
    IGAXIS(x1,x2,-550.+y0,-550.+y0,0.,100.,502,"US");
    IGTEXT(x2,-530.+y0,"100 cm",20.,0.,"L");
    /*  X-/Y- axes */
    IGAXIS(x0+400.,x0+550.,y0,y0,0.,1.,0,"AU");
    IGTEXT(x0+550.,y0-40.,"Z(X?S!)",30.,0.,"L");
    IGAXIS(x0,x0,y0+sxy*400.,y0+sxy*550.,0.,1.,0,"AU");
    sprintf(legend,"%2d",isec);
/*
    IGTEXT(x0+15.,y0+sxy*550.,'Y?SECTOR '//legend//'!',30.,0.,'L');
*/
  }

  return;
}


void
dcdvert_(int *jw, TRevent *ana, int *isec, float *sxy,float *x0,float *y0)
{
  dcdvert(jw, ana, *isec, *sxy, *x0, *y0);
}

void
dcdvert(int *jw, TRevent *ana, int isec, float sxy, float x0, float y0)
{
  int i,it,npoint,nstepm;
  float x1, x2, x_hit[mxstep], y_hit[mxstep];

  static int first = 1;
  static int ibgrd, ired, igreen, iblue, ipurp, ilblue;
  static float *xmax, *ymax, degrad;
  static char legend[2];

  if(first)
  {
    xmax = &sdadraw_.sda_wc[1];
    ymax = &sdadraw_.sda_wc[3];
    ibgrd  = 1;
    ired   = 2;
    igreen = 3;
    iblue  = 4;
    ipurp  = 6;
    ilblue = 7;
    degrad = acos(-1.)/180.;

    first = 0;
  }

  /* Set up a view box for the convenience of hard_copy output */
  ISPMCI(ibgrd);
  ISPLCI(ibgrd);
  if(ABS(sxy) != 1.) /* ????????????????????????????????????? */
  {
    IGBOX(0.,(*xmax),0.05,0.77*(*ymax));
    sprintf(legend,"%2d",isec);
    IGTEXT((*xmax)*.05,(*ymax)*.75,
      "SDA Program   Along The Beam View of the CLAS Detector",
      (*ymax)/100.,0.,"L");
	/* Z-/Y- axes */
    IGAXIS(x0,16.,y0,y0,0.,1.,0,"AU");
    IGTEXT(16.2,y0,"Zs  (WIRE)",(*ymax)/50.,0.,"L");
    IGAXIS(x0,x0,y0,12.,0.,1.,0,"AU");
    IGTEXT(x0,12.2,"Ys",(*ymax)/50.,0.,"L");
  }

  /* Draw a scale factor equivalent to 100cm on the bottom left corner. */

  x1 = 1.;
  x2 = x1 + ABS(sxy)*100.;
  if(ABS(sxy) != 1.) /* ?????????????????????????? */
  {
    IGSET("TMSI",(*ymax)/140);
    IGAXIS(x1,x2,.2,.2,0.,100.,502,"US");
    IGTEXT(x2,.4,"100 cm",(*ymax)/140.,0.,"L");
  }

printf("ntrdraw=%d isec=%d\n",sdadraw_.ntrdraw,isec);
  for(it=1; it<=sdadraw_.ntrdraw; it++)
  {
printf("draw_isec=%d\n",sdadraw_.draw_isec[it-1]);
    if(isec == sdadraw_.draw_isec[it-1])
	{
      nstepm = sdadraw_.draw_nstep[it-1];
printf("nstepm=%d\n",nstepm);
      for(i=1; i<=nstepm; i++)
	  {
        npoint = i;
        x_hit[npoint-1] = sdadraw_.draw_xyz[it-1][i-1][2] * ABS(sxy) + x0;
        y_hit[npoint-1] = sdadraw_.draw_xyz[it-1][i-1][1] * ABS(sxy) + y0;
printf("x=%8.3f y=%8.3f\n",x_hit[npoint-1],y_hit[npoint-1]);
	  }
      ISPLCI(iblue);
      IPL(npoint,x_hit,y_hit);

    }
  }

  return;
}
E 1
