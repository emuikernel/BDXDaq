h51108
s 00013/00006/00748
d D 1.2 03/04/17 16:51:04 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 18:58:49 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/recsis.s/dcgeom.c
e
s 00754/00000/00000
d D 1.1 01/11/19 18:58:48 boiarino 1 0
c date and time created 01/11/19 18:58:48 by boiarino
e
u
U
f e 0
t
T
I 1
/*      subroutine dc_nominal_geom */
/*
 *  Purpose:
 *  --------
 *      To return the nominal wire positions, directions, lengths, and layer
 *   information as well (Radius, midplane normal, etc.). The results can
 *   then be modified by the misorientation parameters by the calling routine.
 *
 *  Calling Sequence:
 *  ----------------
 *
 *  Input Parameters:  (Name - Type - Meaning)
 *  ----------------
 *
 *  Output Parameters:  (Name - Type - Meaning) all in SCS coord system.
 *  -----------------
 *    wire_pos[layer][wire][3] float   Midplane wire position of the wires
 *    wire_dir[layer][wire][3] float   Direction cosines of the wires
 *    wire_len[layer][wire][2] float   Half lengths: [0] to HV end (+y),
 *                                                   [1] to Amp end (-y)
 *            Center[layer][3] float   Center of Curvature (cm)
 *                 R[layer]    float   Radius of curvature of layer (cm)
 *            PhiLog[layer]    float   Phi first logical sense wire WRT
 *                                       the  Center of Curvature (radians)
 *              dPhi[layer]    float   Angular step bt wires in cyl part WRT
 *                                       the Center of Curvature.
 *           MinWire[layer]    int     First physical sense wire
 *           MaxWire[layer]    int     Last physical sense wire
 *            Stereo[layer]    float   Approx. stereo angle (radians)
 *          CellSize[layer]    float   Cell radius for cyl. part (cm)
 *            MidDir[layer][3] float   Normal to midplane (towards Amp end)
 *           CylWire[layer]    int     Last cylindrical wire before plane
 *             PlSep[layer]    float   Separation bt s. wires in plane (cm)
 *            PlNorm[layer][3] float   Normal vector to plane
 *            PlDist[layer]    float   Distance from plane to origin (cm)
 *  Called from:  dc_make_DCG
 *  ------------
 *
 *  Other routines:  dc_vrot, dc_vtranrot
 *  ---------------
 *
 *  Notes:  The SCS coordinate system has the nominal target at the origin,
 *  ------  the z-axis along the beamline, the x-axis parallel in the midplane,
 *          y-axis ZxX = pointing up (for Sector 1) parallel to axial wires
 *          pointing towards the HV side (at least for R1).
 *
 *  Author:   Robert Feuerbach      Created:  Thu Nov 13 09:37:42 EST 1997
 *  -------
 *
 *  Major revisions:
 *  ----------------
 *     
 *
 *_end_doc
 */
/*
 *_begin_inc
 *  include files :
 *  ---------------------
 */

I 3
#include <stdio.h>
E 3
#include <stdlib.h>
#include <math.h>
#include <string.h>
D 3
#include <stdio.h>
E 3

I 3
#include "ntypes.h"
#include "bostypes.h"


E 3
#define SQR(A)   ( (A)*(A) )

enum {MAX_LAY = 36, MAX_WIRE = 192, MAX_REG = 3};
enum {NSUP = 6,     /* number of superlayers */
      NLAY = 6};    /* number of layers in a superlayer */

extern float vdot_(float a[], float b[], int *c);

void
dc_nominal_geom_( float wire_pos[MAX_LAY][MAX_WIRE][3],
		      float wire_dir[MAX_LAY][MAX_WIRE][3],
		      float wire_len[MAX_LAY][MAX_WIRE][2],
		      float Center[MAX_LAY][3], float R[MAX_LAY],
		      float PhiLog[MAX_LAY], float dPhi[MAX_LAY],
		      int MinWire[MAX_LAY], int MaxWire[MAX_LAY],
		      float Stereo[MAX_LAY], float CellSize[MAX_LAY],
		      float MidDir[MAX_LAY][3],
		      int CylWire[MAX_LAY],
		      float PlSep[MAX_LAY], float PlNorm[MAX_LAY][3],
		      float PlDist[MAX_LAY])
{
  /* parameters for each superlayer, keyed on layer 3,
   *  units in degrees and cm. */

  float rsupr[NSUP][7] = 
    /*         1st and last guard.w in each Region, including staggers */ 
    /*   R      PhiMin     PhiMax     X0        Y0    #s.wires    Phi_Plane */
  { { 110.9342, 35.93450, 124.07537, -0.0003, -53.0449, 128.,    89.980 },
    { 118.3130, 33.47888, 123.61834, -0.0003, -53.0449, 140.,    90. },
    { 235.26,   25.00,    133.70,    29.3,    -67.7,    192.,   180. },
    { 251.06,   23.90,    134.30,    29.3,    -67.7,    192.,   180. },
    { 338.2993, 21.18,    138.18,    48.0,    -69.6156, 192.,   180. },
    { 364.6003, 21.18,    138.18,    48.0,    -69.6156, 192.,   180. } };

  /* (X0,Y0) correspond to the Bogdan Center (center of curvature) for the
   * chambers, where X0 is the distance of the center downstream (+) from the
   * nominal target center, Y0 is the distance toward the sector (+) from the
   * same center. This is NOT the same coordinate system as in the rest of
   * this routine.
   */
  /* the BOGDAN center (X0,Y0) for R3 was moved from (48.0,-70.0) to (48.0,-69.6156)
   * due to a different length for the wire-holder internal to the chamber than in
   * the design. The value is from Alexander Skabelin, 24 OCT 97,
   * Results of DC Survey for Region 3.
   */
  /* Also, the trumpets of R2 are shorter than the design value, which causes
   * the wires to be 1 mm closer to the beamline (at the midplane). So the
   * Bogdan Center is moved from (29.3,-67.6) to (29.3,-67.7).
   * Numbers from Liming Qin, Apr 8, 1998.
   */


  /* distance of endplate to the coil plane (in cm) */
  float coil2end[MAX_REG] = { 3.213, 11.5199, 14.97 };

  /* first physical wire number offset for Region 1 */
  int w_offset[6] = { 4, 3, 3, 2, 2, 1 };

  /* last wire number for superlayer 2 */
  int w_last_sl2[6] = { 142, 142, 142, 126, 121, 120 };
  /* MUST USE !!! */
  int w_last_sl3[6] = { 184, 185, 186, 187, 188, 189 };
  /* MUST USE !!! */
  int w_last_sl4[6] = { 189, 189, 190, 191, 192, 192 };

  /* mini stagger info (cm) -- by Region */
  float ministag[6] = { 0.03, 0.03, 0., 0., 0., 0. };

  /* the stereo angle (deg) */
  float ster = 6.;

  /* stereo angle correction factors */
  float stercor[MAX_REG][2] = { { -0.79599, 6.3372 },
				{  0., 0. }, { 0., 0. } };
  int ilay;   /* layer number - 1 */
  
  int three=3;
  
  float *slinfo;
  float degrad, cos30, cot30;

  /* Now for the working code: */
  degrad = acos(-1.)/180.;
  cos30 = cos(30.*degrad);
  cot30 = 1/tan(30.*degrad);

  /* process each layer  -- realize that here layer runs from 0 to 35 */
  for (ilay = 0; ilay < MAX_LAY; ilay++) {
    int ireg, isprl, ila;   /* (region, superlayer, and layer in SL) - 1 */
    float *slinfo;
    float plphi, plsep, r, dphi, stereo, phi1, center[3], phi_offset;
    float phi_stag, cospl, sinpl, stc1, stc2, spacy;
    float vec[3];
    int ncell, maxw, minw, iw, cylw;

    ireg = ilay/(2*NLAY);    /* 0 to 2 */
    isprl = ilay/(NSUP);     /* 0 to 5 */
    ila = ilay % NLAY;       /* 0 to 5 */

    slinfo = rsupr[isprl];
    /*  angle of plane wrt beamline */
    plphi = slinfo[6] * degrad;

    /* store stereo angle */
    if (isprl==0 || isprl==3 || isprl==5 ) {
      Stereo[ilay] = ster;
      stereo = Stereo[ilay]*degrad;
    } else
      stereo = Stereo[ilay] = 0.;
    
    /* find dPhi and PlSep */
    ncell = slinfo[5] + 1;   /* phimin and phimax for guard wires, so
				the number of steps is (total#wires - 1),
				or (#sensew+2) - 1 */
    if (slinfo[6] < slinfo[2]) {  /* we have a plane in the back */
      dphi = dPhi[ilay] = (plphi - slinfo[1]*degrad
		    + tan(slinfo[2]*degrad - plphi))/ncell;
      plsep = PlSep[ilay] = dphi*slinfo[0];
    } else {                 /* just a simple cylinder */
      dphi = dPhi[ilay] = (slinfo[2]-slinfo[1])*degrad/ncell;
      plsep = PlSep[ilay] = 0.;
    }
    
    /* Layer radius */
    r = R[ilay] = cos30*slinfo[0]*dphi*(ila-2) + slinfo[0];

    /* cell size */
    CellSize[ilay] = 0.5*r*dphi;
printf("-+-+-%d %f\n",ilay,CellSize[ilay]);

    /* Dealing with the stagger: The 3rd layer includes all staggers, and
     * since the odd and even layers are ministaggered opposite ways, if
     * the layer is even we have to double ministagger and then do the
     * normal half-cell shift.
     */

    if ( (ilay+1) % 2 == 0 )
      phi_stag = -2.*ministag[isprl]/slinfo[0] + 0.5*dphi;
    else
      phi_stag = 0.;
    
    /* take care of MinWire, MaxWire and PhiLog for each layer */
    if (isprl < 2)
    { /* in region 1 -- special cases */
      minw = MinWire[ilay] = w_offset[ila];
      if (isprl == 1)    /* SL2, extra special case */
	    maxw = MaxWire[ilay] = w_last_sl2[ila];
      else               /* SL1 */
	    maxw = MaxWire[ilay] = slinfo[5] + w_offset[2] - 1;

      /* account for reference layer not starting at sense wire #1 */
      phi_offset = dphi*(w_offset[2] - 1);
      phi1 = PhiLog[ilay] = slinfo[1]*degrad + phi_stag + dphi - phi_offset;
    } else {  /* Region 2 or 3 are much simpler */
      minw = MinWire[ilay] = 1;
      maxw = MaxWire[ilay] = slinfo[5];
      phi1 = PhiLog[ilay] = slinfo[1]*degrad + dphi + phi_stag;
    }

    /* Center of curvature */
    center[0] = Center[ilay][0] = slinfo[4];
    center[1] = Center[ilay][1] = 0.;
    center[2] = Center[ilay][2] = slinfo[3];

    /* normal to plane  -- in case of cyl, it just points back upstream */
    PlNorm[ilay][0] = sin(plphi);
    PlNorm[ilay][1] = 0.;
    PlNorm[ilay][2] = cos(plphi);
    
    /* normal to midplane, pointing at HV chamber */
    MidDir[ilay][0] = 0.;
    MidDir[ilay][1] = 1.;
    MidDir[ilay][2] = 0.;

    /* prepare for setting up the wires in each layer */
    sinpl = sin(plphi);
    cospl = cos(plphi);
    stc1 = stercor[ireg][0]*degrad;
    stc2 = stercor[ireg][1];
    spacy = coil2end[ireg]/cos30;   /* used to get position of wire end */
    
    /* calculate the position and direction of each wire in this layer */
    for (iw=0; iw < maxw; iw++)
    {
      float *wp, *wd, *wl;
      float phiw, x, cosph, sinph, st, sinst, cosst, ymove;
      float dxdy, dzdy, wpend[3], wnend[3];

      wp = wire_pos[ilay][iw];
      phiw = phi1 + iw*dphi;
      if (phiw <= plphi)
      {   /* on cylindrical section */
	
	    cylw = iw+1;  /* keep track of maximum wire on cylinder */
	
	    /* wire position calculation */
	    cosph = cos(phiw);
	    sinph = sin(phiw);
	
	    wp[0] = x = r*sinph + center[0];
	    wp[1] = center[1];
	    wp[2] = r*cosph + center[2];
      }
      else
      {   /* on the plane section */
	    cosph = cospl;
	    sinph = sinpl;
	    wp[0] = x = wire_pos[ilay][iw-1][0] + plsep*cosph;
	    wp[1] = wire_pos[ilay][iw-1][1];
	    wp[2] = wire_pos[ilay][iw-1][2] - plsep*sinph;
      }

      /* wire direction part */
      wd = wire_dir[ilay][iw];
      if (stereo < 0.0001)
      {     /* Axial wires */
	    wd[0] = 0.;
	    wd[1] = 1.;
	    wd[2] = 0.;
      }
      else
      {                   /* Stereo wires */
	    st = (stereo + stc1/(x-stc2));  /* corrected stereo angle */
	    sinst = sin(st);
	    cosst = cos(st);
	    wd[0] = -cosph*sinst;
	    wd[1] =  cosst;
	    wd[2] =  sinph*sinst;
      }

      /* and finally, the wire lengths */
      /* slope in the xy and zy planes */
      dxdy = wd[0]/wd[1];
      dzdy = wd[2]/wd[1];

      /* the end with positive Y : HV end */
      wpend[1] = (ymove = (wp[0] - spacy*cot30)/(cot30 - dxdy)) + wp[1];
      wpend[0] = wp[0] + dxdy*ymove;
      wpend[2] = wp[2] + dzdy*ymove;

      /* the end with negative Y: Amp end */
      wnend[1] = (ymove = - (wp[0] - spacy*cot30)/(cot30 + dxdy)) + wp[1];
      wnend[0] = wp[0] + dxdy*ymove;
      wnend[2] = wp[2] + dzdy*ymove;

      /* And then the half lengths */
      wl = wire_len[ilay][iw];

      wl[0] = sqrt(   SQR(wp[0] - wpend[0]) + SQR(wp[1] - wpend[1])
		    + SQR(wp[2] - wpend[2]) );
      wl[1] = sqrt(   SQR(wp[0] - wnend[0]) + SQR(wp[1] - wnend[1])
		    + SQR(wp[2] - wnend[2]) );
    }

    /* That's all the ideal wire position calculations to make. */
    /* Now, just for some cleanup */

    CylWire[ilay] = cylw;
    /* the distance from the origin to the plane */
    vec[0] = r*sinpl + center[0];
    vec[1] = center[1];
    vec[2] = r*cospl + center[2];
    PlDist[ilay] = vdot_(vec,PlNorm[ilay],&three);
  }
  return;
}
/*
*-------------------------------------------------------------------------
*/



/*
 *_begin_doc
 *  RCS ID string
 *  $Id: dc_geom.c,v 1.10 2000/06/02 17:27:06 feuerbac Exp $
*/
/*
 *  Documentation for DC_GEOM packages
 *
 *  Purpose:
 *  --------
 *    Set up the DC geometry, including mapmanager modifications, into the
 *    DCGM and DCGW banks. Also will store the misorientation parameters
 *    in the DGEO bank.
 *    The small mis-alignment parameters, put into DGEO, position the wires
 *    and the wire directions in the HSCS.
 * 
 *    Each sector has its own DGEO bank(# = sector). Torus has bank 7.
 *  Calling Sequence:
 *  ----------------
 *  1.  dc_make_dgeo_(int *run_number, int *force);
 *      to create the DGEO geometry offset banks with information from
 *      the mapmanager for run_number, *force = 1 to make sure to write
 *      new DGEO banks, overwriting the old if necessary.
 *  2.  dc_geom_banks_();
 *      to then fill the DCGW and DCGM banks.
 *
 *  Notes:
 *  ------
 *
 *  Author:   Robert Feuerbach      Created:  Fri Nov 14 10:31:49 EST 1997
 *  -------
 *
 *  Major revisions:
 *  ----------------
 *     
 *
 *_end_doc
 */
/*
 *_begin_inc
 *  include files :
 *  ---------------------
 * BOS common block  uncomment the next line for BOS include file
 */

D 3
#include <ntypes.h>
#include <bostypes.h>

E 3
/*_end_inc */
/*_begin_var
 *  input/output variables:
 *  -----------------------
 *
 *  Local pre-defined variables:
 *  ---------------------------  
 *  RCS information: 
 */

enum {MAX_SEC = 6};
/*enum {MAX_REG = 3, MAX_LAY = 36, MAX_WIRE = 192, NSUP = 6};*/

extern float vdot_(float v1[], float v2[], int *n);


/*-------------------------------------------*/
/*----------- vtrans ------------------------*/
void
vtrans(float v1[3], float v2[3], float vout[3])
{
  vout[0] = v1[0] + v2[0];
  vout[1] = v1[1] + v2[1];
  vout[2] = v1[2] + v2[2];
  return;
}

/*-------------------------------------------*/
/*----------- vrot --------------------------*/
void
vrot(float v1[3], float rot[3][3], float vout[3])
{
  int i,j;
  for (i=0; i<3; i++) {
    vout[i] = 0.;
    for (j = 0; j < 3; j++)
      vout[i] += rot[i][j]*v1[j];
  }
  return;
}

/*---------------------------------------------------*/
/* ----------- dc_make_dgeo -------------*/
/* callable from FORTRAN
 *  Purpose: use the given run number to get offsets from the
 *           mapmanager and put them into the DGEO bank, so
 *           these offsets may be used by future calls.
 *  input: runno for MapManager, force != 0 to overwrite old DGEO bank
 *  output: DGEO bank
 */

void
dc_make_dgeo_(int *runno, int *force, float r2r3cor[2][6])
{
  float xgeom[MAX_REG][MAX_SEC][3], sgeom[MAX_REG][MAX_SEC][3];
  float xt[3],st[3];
  int itmp, ok[MAX_REG][MAX_SEC];
  char mess[80];
  clasDGEO_t *DGEO=NULL;
  
D 3
  if (DGEO = getGroup(&wcs_, "DGEO",1))
E 3
I 3
/*printf("-1\n");fflush(stdout);*/
  if(DGEO = getGroup(&wcs_, "DGEO",1))
E 3
  {
I 3
/*printf("-2\n");fflush(stdout);*/
E 3
    /* the DGEO bank already exists. 
     * We're going to overwrite it if force is != 0.
     */
D 3
    if (*force)
E 3
I 3
    if(*force)
E 3
    {
      printf("dc_make_dgeo: Overwriting old DGEO banks.\n");
    }
    else
    {
      printf("dc_make_dgeo: Using old DGEO bank.\n");
      return; /* don't overwrite old DGEO if we're not suppose to. */
    }
  }
I 3
/*printf("-3\n");fflush(stdout);*/
E 3

  /* get parameters from mapmanger */
  printf("dc_make_dgeo: Getting DC Geometry from Run %d.\n",*runno);
  if(itmp=get_dc_map_geom_(runno,xgeom,sgeom,xt,st,ok))
  {
    int sector;
I 3
/*printf("-4\n");fflush(stdout);*/
E 3
    for (sector = 1; sector <= MAX_SEC; sector++)
    {
I 3
/*printf("-5\n");fflush(stdout);*/
E 3
      if (DGEO = makeBank(&wcs_,"DGEO",sector,sizeof(dgeo_t)/sizeof(int),
			  MAX_REG))
      {
	    int region;
	    int isec=sector-1;
I 3
/*printf("-6\n");fflush(stdout);*/
E 3
	    for(region = 0; region < MAX_REG; region++)
        {
	      DGEO->dgeo[region].id_sec = sector;
	      DGEO->dgeo[region].id_reg = region+1;
	      DGEO->dgeo[region].xpos = xgeom[region][isec][0];
	      DGEO->dgeo[region].ypos = xgeom[region][isec][1];
	      DGEO->dgeo[region].zpos = xgeom[region][isec][2];
	      DGEO->dgeo[region].sxpos = sgeom[region][isec][0];
	      DGEO->dgeo[region].sypos = sgeom[region][isec][1];
	      DGEO->dgeo[region].szpos = sgeom[region][isec][2];
		  
          if((*force)==2 && region!=0)
		  {
			
	        DGEO->dgeo[region].xpos += r2r3cor[region-1][0];
	        DGEO->dgeo[region].ypos += r2r3cor[region-1][1];
	        DGEO->dgeo[region].zpos += r2r3cor[region-1][2];
	        DGEO->dgeo[region].sxpos += r2r3cor[region-1][3];
	        DGEO->dgeo[region].sypos += r2r3cor[region-1][4];
	        DGEO->dgeo[region].szpos += r2r3cor[region-1][5];
			/*
	        DGEO->dgeo[region].xpos = r2r3cor[region-1][0];
	        DGEO->dgeo[region].ypos = r2r3cor[region-1][1];
	        DGEO->dgeo[region].zpos = r2r3cor[region-1][2];
	        DGEO->dgeo[region].sxpos = r2r3cor[region-1][3];
	        DGEO->dgeo[region].sypos = r2r3cor[region-1][4];
	        DGEO->dgeo[region].szpos = r2r3cor[region-1][5];
            */
		  }
		  
	    }
      }
      else
      {        /* problems making the bank */
	    printf("dc_make_dgeo: Cannot create Sector %1i DGEO bank.\n",sector);
      }
    }
    /* fill in the torus bank */
    sector = 7;
    if (DGEO = makeBank(&wcs_,"DGEO",sector,sizeof(dgeo_t)/sizeof(int),1))
    {
      DGEO->dgeo[0].id_sec = 0;
      DGEO->dgeo[0].id_reg = 0;
      DGEO->dgeo[0].xpos = xt[0];
      DGEO->dgeo[0].ypos = xt[1];
      DGEO->dgeo[0].zpos = xt[2];
      DGEO->dgeo[0].sxpos = st[0];
      DGEO->dgeo[0].sypos = st[1];
      DGEO->dgeo[0].szpos = st[2];
    }
    else
    {
      printf("dc_make_dgeo: Cannot create Toroid DGEO bank.\n");
    }
  }
  else
  {
    printf("dc_make_dgeo: get_dc_map_geom() returns %d\n",itmp);
  }
}

/*--------------------------------------------------*/
/*-------------- get_DGEO --------------------------*/
/*  inputs: sector #
 *  output: disp[region][3]     : displacement "vector" for regions
 *          rot[region][3][3]   : rotation matrix for regions
 *          ok[region][sector]  : if info was found okay. (0 = no)
 *  returns: for the sector: AND of ok[region]'s
 */
int
get_DGEO(int sector, float disp[MAX_REG][3], float rot[MAX_REG][3][3],
	     int ok[MAX_REG][MAX_SEC])
{
  clasDGEO_t *DGEO;
  int i;
  int isec = sector-1;

  for (i=0; i < MAX_REG; i++) ok[i][isec] = 0;
  if (DGEO = getGroup(&wcs_,"DGEO",sector))
  {
    int ireg;
    for ( ireg=0; ireg < MAX_REG; ireg++)
    {
      disp[ireg][0] = DGEO->dgeo[ireg].xpos;
      disp[ireg][1] = DGEO->dgeo[ireg].ypos;
      disp[ireg][2] = DGEO->dgeo[ireg].zpos;
      rot[ireg][0][0] =  1.;
      rot[ireg][0][1] = -DGEO->dgeo[ireg].szpos;
      rot[ireg][0][2] =  DGEO->dgeo[ireg].sypos;
      rot[ireg][1][0] =  DGEO->dgeo[ireg].szpos;
      rot[ireg][1][1] =  1.;
      rot[ireg][1][2] = -DGEO->dgeo[ireg].sxpos;
      rot[ireg][2][0] = -DGEO->dgeo[ireg].sypos;
      rot[ireg][2][1] =  DGEO->dgeo[ireg].sxpos;
      rot[ireg][2][2] =  1.;
      ok[ireg][isec] = 1;
    }
  }
  else
  {
    printf("get_DGEO ERROR: DGEO=0x%08x - exit.\n",DGEO);
    exit(0);
  }  
  return (ok[0][isec] && ok[1][isec] && ok[2][isec]);
}

/*----------------------------------------*/
/* ----------- dc_geom_banks_ -------------*/
/* callable from FORTRAN */
/* input: DGEO bank
 * output: DCGM and DCGW  banks
 * Calculates the wire positions, directions and lengths and layer
 * dependent information, using misorientation offsets applied to the
 * nominal values.
 */
void
dc_geom_banks_(void)
{
  clasDCGM_t *DCGM=NULL;
  clasDCGW_t *DCGW=NULL;
  dcgw_t dcgw[MAX_WIRE];
  dcgm_t dcgm;
  float disp[MAX_REG][3];
  float rot[MAX_REG][3][3];
  int sector,itmp;
  int ok[3][6];
  float pi;

  /* Nominal values for geometry info */
  float nwp[MAX_LAY][MAX_WIRE][3], nwd[MAX_LAY][MAX_WIRE][3];
  float nwl[MAX_LAY][MAX_WIRE][2], ncnt[MAX_LAY][3], nr[MAX_LAY];
  float nth1[MAX_LAY], ndth[MAX_LAY], nster[MAX_LAY], ncell[MAX_LAY];
  float nmddr[MAX_LAY][3], nplsp[MAX_LAY], nplnrm[MAX_LAY][3], npld[MAX_LAY];
  int minw[MAX_LAY],maxw[MAX_LAY],cylw[MAX_LAY];
  /* get nominal geometry */
  dc_nominal_geom_(nwp,nwd,nwl,ncnt,nr,nth1,ndth,minw,maxw,nster,ncell,
		   nmddr,cylw,nplsp,nplnrm,npld);

  pi = acos(-1.);

  /* second BOS array initialization - ALL IT TEMPORARILY !!!!!!!! */

  for(sector=1; sector <= MAX_SEC; sector++)
  {
    int colDCGM = sizeof(dcgm_t)/sizeof(int);
    int colDCGW = sizeof(dcgw_t)/sizeof(int);

    /* read in DGEO bank for this sector and set up displacement vectors
     * and rotation matrices. */
    itmp = get_DGEO(sector, disp, rot, ok);
	DCGM = makeBank(&wcs_, "DCGM", sector, colDCGM, MAX_LAY);
	DCGW = makeBank(&wcs_, "DCGW", sector, colDCGW, MAX_LAY*MAX_WIRE);
    if (itmp && DCGM && DCGW )
    {
	  int layer;
	  for (layer=0; layer < MAX_LAY; layer++)
      {
	    int wire;
        int minwire = minw[layer]-1;
	    int maxwire = maxw[layer]-1;
	    int ireg = layer/(2*NSUP);
	    float vec1[3],vec2[3];
	    int three=3;
	    int layer_offset = layer*MAX_WIRE;

	    /* first do the layer dependent stuff */
	    /*     doesn't change with re-orientation */
	    dcgm.r_curve = nr[layer];
	    dcgm.min_wire = minw[layer];
	    dcgm.max_wire = maxw[layer];
	    dcgm.stereo = nster[layer];
	    dcgm.cell_size = ncell[layer];
	    dcgm.max_cylw = cylw[layer];
	    dcgm.p_sep = nplsp[layer];

	    /*      Center of curvature */
	    vtrans(ncnt[layer],disp[ireg],vec1);
	    vrot(vec1,rot[ireg],vec2);
	    dcgm.x_curve = vec2[0];
	    dcgm.y_curve = vec2[1];
	    dcgm.z_curve = vec2[2];
	    /*      the midplane normal */
	    vrot(nmddr[layer],rot[ireg],vec1);
	    dcgm.x_nmid = vec1[0];
	    dcgm.y_nmid = vec1[1];
	    dcgm.z_nmid = vec1[2];
	    /*      the plane normal */
	    vrot(nplnrm[layer],rot[ireg],vec1);
	    dcgm.x_norm = vec1[0];
	    dcgm.y_norm = vec1[1];
	    dcgm.z_norm = vec1[2];
	    /* distance from plane to origin */
	    vec2[0] = dcgm.r_curve*dcgm.x_norm + dcgm.x_curve;
	    vec2[1] = dcgm.r_curve*dcgm.y_norm + dcgm.y_curve;
	    vec2[2] = dcgm.r_curve*dcgm.z_norm + dcgm.z_curve;
	    dcgm.p_dist = vdot_(vec1,vec2,&three);
	    /*      theta1 -- taking into account rotations */
	    vec1[0] = sin(nth1[layer]);
	    vec1[1] = 0.;
	    vec1[2] = cos(nth1[layer]);
	    vrot(vec1,rot[ireg],vec2);
	    dcgm.theta_start = atan2(vec2[0],vec2[2]);
	    /*      dth  -- taking into account rotations */
	    dcgm.d_theta = ndth[layer]*dcgm.y_nmid;
	    /* Now for the individual wires */
	    for (wire=0; wire < minwire; wire++)
        {
	      dcgw[wire].x_mid = dcgw[wire].y_mid = dcgw[wire].z_mid = 0.;
	      dcgw[wire].x_dir = dcgw[wire].y_dir = dcgw[wire].z_dir = 0.;
	      dcgw[wire].w_len = dcgw[wire].w_len_hv = 0.;
	    }
	    for (wire = minwire; wire <= maxwire; wire++)
        {
	      float dist;
	      float wlp = nwl[layer][wire][0];
	      float wln = nwl[layer][wire][1];
	      vtrans(nwp[layer][wire],disp[ireg],vec1);
	      vrot(vec1,rot[ireg],vec2);
	      dcgw[wire].x_mid = vec2[0];
	      dcgw[wire].y_mid = vec2[1];
	      dcgw[wire].z_mid = vec2[2];
	      vrot(nwd[layer][wire],rot[ireg],vec1);
	      dcgw[wire].x_dir = vec1[0];
	      dcgw[wire].y_dir = vec1[1];
	      dcgw[wire].z_dir = vec1[2];
	     /* actually, these (above) are the points on the wire in
	      * the center of the chamber. To get the points in the
	      * Sector midplane, we have to move along the wire to
	      * where y=0 (definition of the Sector midplane).
	      * Adjust the wire lengths as well.
	      */
	      dist = - dcgw[wire].y_mid/dcgw[wire].y_dir; /* signed distance to move */
	      dcgw[wire].x_mid += dist*dcgw[wire].x_dir;  /* slide along the wire */
	      dcgw[wire].y_mid += dist*dcgw[wire].y_dir;  /* should end up 0. */
	      dcgw[wire].z_mid += dist*dcgw[wire].z_dir;
	    
	      wlp -= dist;        /* the wire 1/2 lengths on the 2 sides also changes */
	      wln += dist;

	      /* Region 1, layers 1-12 have the STB on the opposite side */
	      /* of the sector. */
	      if (layer <= 12)
          {            /* Region 1 */
	        dcgw[wire].w_len = wln;
	        dcgw[wire].w_len_hv = wlp;
	      }
          else
          {                      /* Regions 2 and 3 */
	        dcgw[wire].w_len = wlp;
	        dcgw[wire].w_len_hv = wln;
	      }
	    }
	    for (wire=maxwire+1; wire < MAX_WIRE; wire++)
        {
	      dcgw[wire].x_mid = dcgw[wire].y_mid = dcgw[wire].z_mid = 0.;
	      dcgw[wire].x_dir = dcgw[wire].y_dir = dcgw[wire].z_dir = 0.;
	      dcgw[wire].w_len = dcgw[wire].w_len_hv = 0.;
	    }

	    /* minimum and maximum angle of wires at midplane */
	    dcgm.theta_min=atan2(dcgw[minwire].x_mid,dcgw[minwire].z_mid);
	    dcgm.theta_max=atan2(dcgw[maxwire].x_mid,dcgw[maxwire].z_mid);

	    /* Now copy into the BosBank itself  */
	    DCGM->dcgm[layer] = dcgm;
	    for (wire=0; wire < MAX_WIRE; wire++)
		{
	      DCGW->dcgw[layer_offset+wire] = dcgw[wire];
		}
	  }
    }
    else
    {
      printf("itmp=%d DCGM=0x%08x DCGW=0x%08x\n",itmp,DCGM,DCGW);
      printf("Error: creating DCGW and DCGM Banks - exit.\n");
      exit(0);
    }
  }
}

E 1
