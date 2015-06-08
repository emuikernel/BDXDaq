/**
 * <EM>Deals with creating, manipulating, etc. for the Hv_Region.</EM>
 * <P>
 * The Hv library was developed at The Thomas Jefferson National
 * Accelerator Facility under contract to the
 * Department of Energy and is the property of they U.S. Government.
 * Partial support came from the National Science Foundation.
 * <P>
 * It may be used to develop commercial software, but the Hv
 * source code and/or compiled modules/libraries may not be sold.
 * <P>
 * Questions or comments should be directed
 * to <a href="mailto:heddle@cebaf.gov">heddle@cebaf.gov</a> <HR>
 */


#include "Hv.h"	  /* contains all necessary include files */


/**
 * Hv_EraseRegion
 * @param   Erase a region.
 * @param   Region to Erase
 */

void Hv_EraseRegion (Hv_Region region) {

/*erases a region -- algorithm: a) fine smallest rect that encloses
  the region b) set clip region to be the region and c) erase the rect*/

  Hv_Rect r;

  Hv_SetClippingRegion(region);  /* set the clipping are to the region */
  Hv_ClipBox(region, &r);        /* clip that area area to the rect */
  Hv_EraseRect(&r);              /* erase the rect surrounding the region */
  Hv_FullClip();                 /* restore to full clip */
  Hv_Flush();
}


/**
 * Hv_PrintRegionBounds
 * @purpose Print the boundary of a region for diagnostics
 * @param   file    Output stream.
 * @param   region  Region in question.
 * @param   message Optional information string.
 */

void            Hv_PrintRegionBounds(FILE     *file,
				     Hv_Region region,
				     char     *message) {

  Hv_Rect rect;

  if (region == NULL)
    return;

  Hv_ClipBox(region, &rect);
  Hv_PrintRect(file, &rect, message);
}

/**
 * Hv_FillRegion     Basically a region painter.
 * @param      region      The region to paint
 * @param      color       The color to paint
 * @param      clipregion  A clipping region
 */


void   Hv_FillRegion(Hv_Region  region,
		     short      color,
		     Hv_Region  clipregion) {
  Hv_Region      treg;
  Hv_Rect        r;

/* note: postscript cannot do this */

  if (Hv_inPostscriptMode)
    return;

/* since the rect surrounds the clipping area and only
 * the space in the clipping area
 * will be painted, this is a viable algorithm */

  if ((color < 0) || (region == NULL))
    return;


  treg = Hv_IntersectRegion(region, clipregion);

  Hv_SetClippingRegion(treg);        /* set the region */
  Hv_ClipBox(treg, &r);              /* get the cooresponding rect */
  Hv_FillRect(&r, color);            /* paint the rect */
  Hv_DestroyRegion(treg);
  Hv_SetClippingRegion(clipregion);  /* set the region back */
}

/*------ Hv_KillRegion ------*/

void  Hv_KillRegion(Hv_Region *region)

/* like destroy, but accepts a pointer
   to a region and then sets it to NULL */

{

  Hv_DestroyRegion(*region);
  *region = NULL;
}


/* ------- Hv_UnionRectWithRegion ------------ */

void Hv_UnionRectWithRegion(Hv_Rect   *sr,
			    Hv_Region   region)

{
  Hv_Rectangle     xrect;

  if((sr == NULL) || (region == NULL))
    return;

  Hv_SetXRectangle(&xrect, sr->left, sr->top, sr->width, sr->height);
  Hv_UnionRectangleWithRegion(&xrect, region);
}

/*----- Hv_CreateRegionFromLine -----*/

Hv_Region  Hv_CreateRegionFromLine(short  x1,
				   short  y1,
				   short  x2,
				   short  y2,
				   short  slop)

/* uses the two pixel endpoints to create a region about
   the line. slop should be at least = line width + 1 */

{
  Hv_Point       xp[4];
  short          delx, dely;
  short          xa, ya, xb, yb;
  Boolean        morevertical, morehorizontal;

  delx = abs(x2 - x1);
  dely = abs(y2 - y1);


  if ((delx == 0) && (dely == 0))
    return NULL;

  slop = Hv_sMax(2, Hv_sMin(8, slop));
  slop++;

  morevertical = (delx < dely);
  morehorizontal = !morevertical;

  if ((morevertical && (y1 > y2)) || (morehorizontal && (x1 < x2))) {
    xa = x1;
    ya = y1;
    xb = x2;
    yb = y2;
  }
  else {
    xa = x2;
    ya = y2;
    xb = x1;
    yb = y1;
  }
  
  if (morevertical) {
    Hv_SetPoint(xp,   (short)(xa-slop), (short)(ya+1));
    Hv_SetPoint(xp+1, (short)(xa+slop), (short)(ya+1));
    Hv_SetPoint(xp+2, (short)(xb+slop), (short)(yb-1));
    Hv_SetPoint(xp+3, (short)(xb-slop), (short)(yb-1));
  }
  else {
    Hv_SetPoint(xp,   (short)(xa-1), (short)(ya-slop));
    Hv_SetPoint(xp+1, (short)(xa-1), (short)(ya+slop));
    Hv_SetPoint(xp+2, (short)(xb+1), (short)(yb+slop));
    Hv_SetPoint(xp+3, (short)(xb+1), (short)(yb-slop));
  }

  return Hv_CreateRegionFromPolygon(xp, 4);
}


/*----- Hv_CreateRegionFromLines -----*/

Hv_Region  Hv_CreateRegionFromLines(Hv_Point   *pp,
				    short       nlines,
				    short       slop)

/*  short        nlines    should be nlines+1 points */

/* uses the points in pp like an unclosed
   poly. This is more expensive than
   Hv_CreateRegionFromPoly, but has the
   nice benefit of excluding the interior.*/

{
  int          i;
  Hv_Region    rgn = NULL;
  Hv_Region    trgn = NULL;

  rgn = Hv_CreateRegion();

  for (i = 0; i < nlines; i++) {
    trgn = Hv_CreateRegionFromLine(pp[i].x, pp[i].y, pp[i+1].x, pp[i+1].y, slop);
    Hv_AddRegionToRegion(&rgn, trgn);
    Hv_DestroyRegion(trgn);
  }

  return rgn;
  
}



/*------- Hv_IntersectRegionWithRegion -------*/

void Hv_IntersectRegionWithRegion(Hv_Region *thereg,
				  Hv_Region ireg)

/********************************************
  An "in place" version of Hv_IntersectRegion
*********************************************/

{
  Hv_Region    temp;


  temp = Hv_IntersectRegion(*thereg, ireg);
  Hv_DestroyRegion(*thereg);
  *thereg = temp;
}



/*-------- Hv_RemoveRectFromRegion -------*/

void   Hv_RemoveRectFromRegion(Hv_Region  *reg,
			       Hv_Rect   *rect  )

{
  Hv_Region    diffrgn;

  if ((*reg == NULL) || (rect == NULL))
    return;

  diffrgn = Hv_SubtractRectFromRegion(*reg, rect);
  Hv_DestroyRegion(*reg);
  *reg = diffrgn;
}


/*------- Hv_SubtractRectFromRegion ------*/

Hv_Region Hv_SubtractRectFromRegion(Hv_Region reg,
				    Hv_Rect  *rect)

{
  Hv_Region   rectreg, diffreg;

  rectreg = Hv_RectRegion(rect);
  diffreg = Hv_SubtractRegion(reg, rectreg);
  Hv_DestroyRegion(rectreg);
  return diffreg;
}


/*----------- Hv_CopyRegion ------------ */

void Hv_CopyRegion(Hv_Region *dreg,
		   Hv_Region sreg)

/* copies region sreg onto region dreg. dreg
   is created here -- do NOT malloc in call */

{
  Hv_Region empty;

  empty = Hv_CreateRegion();

  *dreg = Hv_UnionRegion(sreg, empty);
  Hv_DestroyRegion(empty);
}

/*-------- Hv_AddRegionToRegion ---------*/

void Hv_AddRegionToRegion(Hv_Region  *mainreg,
			  Hv_Region  addreg)

/* adds addreg to mainreg */

{
  Hv_Region    temp;
  
  temp = Hv_UnionRegion(*mainreg, addreg);
  Hv_DestroyRegion(*mainreg);
  *mainreg = temp;
}

/*-------- Hv_SubtractRegionFromRegion ---------*/

void Hv_SubtractRegionFromRegion(Hv_Region  *mainreg,
				 Hv_Region  subreg)

/* removes subreg to mainreg */

{
  Hv_Region    temp;
  
  temp = Hv_SubtractRegion(*mainreg, subreg);
  Hv_DestroyRegion(*mainreg);
  *mainreg = temp;
}


/* -------- Hv_UnionPolygonWithRegion -------*/

void   Hv_UnionPolygonWithRegion(Hv_Point   *poly,
				 int     np,
				 Hv_Region  *region)

{
  Hv_Region   polyreg;

  if (np < 3)
    return;

  polyreg = Hv_CreateRegionFromPolygon(poly, (short)np);
  Hv_AddRegionToRegion(region, polyreg);
  Hv_DestroyRegion(polyreg);
}


/*-------- Hv_RectRegion ----- */

Hv_Region Hv_RectRegion(Hv_Rect *rect)

/* Mallocs & returns a region and sets it to specified rect */

{
  Hv_Region region;      /* region to return*/
 
  region = Hv_CreateRegion();  /* beg for the memory of the region */
  Hv_UnionRectWithRegion(rect, region);  /* set the region to the rect size */
  return region;  /* return the created region of size rect */
}


/*-------- Hv_RectangleRegion ----- */

Hv_Region Hv_RectangleRegion(short  x,
			     short  y,
			     short  w,
			     short  h)

/* Mallocs & returns a region and sets it to specified rect */

{
  Hv_Rectangle xrect;   /* rectangle to create from ltwh */
  Hv_Region region;      /* region to return*/
 
  region = Hv_CreateRegion();  /* beg for the memory of the region */

/* set the xrect to the points supplied */

  xrect.x = x;              
  xrect.y = y;
  xrect.width = w;
  xrect.height = h;

  Hv_UnionRectangleWithRegion(&xrect, region);  /* set the region to the rect size */

  return region;  /* return the created region of size rect */
}







