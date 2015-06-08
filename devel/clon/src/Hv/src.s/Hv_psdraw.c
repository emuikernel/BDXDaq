/*	
-----------------------------------------------------------------------
File:     Hv_psdraw.c
======================================================================
	
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.
	
It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.
	
Questions or comments should be directed to heddle@cebaf.gov
=====================================================================
*/

#include "Hv.h"

#define SHADOW_SHIFT   0.5
#define SHADOW_COLOR   Hv_white

extern float         ps_xmax, ps_xmin, ps_ymax, ps_ymin; 

static void Hv_PSDoOval(Hv_Rect *rect,
			short   color,
			Boolean fill);


/* ------ Hv_PSFrameWedge -------*/

void Hv_PSFrameWedge(short xc,
		     short yc,
		     short rad,
		     int   start,
		     int   del,
		     short color)
    
/*  frames a circle. xc, yc and rad in PS hi res pixels. */
    
{
    float  psx, psy, psrad, fdum, psxo;
    short  sdum = 0;
    
    Hv_PSSetColor(color);
    Hv_PSLocalToPS(xc, yc, &psx, &psy);
    
    Hv_PSLocalToPS(0,   sdum, &psxo,  &fdum);
    Hv_PSLocalToPS(rad, sdum, &psrad, &fdum);
    psrad = (float)fabs(psrad - psxo);
    
    fprintf(Hv_psfp, "newpath %9.3f %9.3f %9.3f %d %d arc stroke\n",
	    psx, psy, psrad, start, start+del);
}


/* --------- Hv_PSDrawPoint ---------*/

void Hv_PSDrawPoint(short  x,
		    short  y,
		    short  color)
    
/* x, y in hi res PS pixels. */
    
{
    Hv_Rect     psrect;
    
    Hv_SetRect(&psrect, (short)(x-2), (short)(y-2), 4, 4);
    Hv_PSFillRect(&psrect, color);
    Hv_PSFrameRect(&psrect, Hv_black);
}

/* --------- Hv_PSDrawPoints ---------*/

void Hv_PSDrawPoints(Hv_Point *p,
		     short  n,
		     short  color)
    
/* p in hi res PS pixels */
    
{
    short   i;
    
    Hv_PSSetLineWidth(1);
    
    for (i = 0; i < n; i++)
	Hv_PSDrawPoint(p[i].x, p[i].y, color);
    Hv_PSSetLineWidth(3);
}


/* ------------- Hv_PSFillRect ------*/

void Hv_PSFillRect (Hv_Rect   *rect,
		    short     color)
    
/* Fills a rectangle in specified color
   rect should be in PS high res pixel coordinates  */
    
{
    short       L, T, R, B;
    float       l, t, r, b;
    
    if (color < 0)
	return;
    
    Hv_GetRectLTRB(rect, &L, &T, &R, &B);
    Hv_PSLocalToPS(L, T, &l, &t);
    Hv_PSLocalToPS(R, B, &r, &b);
    Hv_PSSetColor(color);
    fprintf(Hv_psfp, "%8.2f %8.2f %8.2f %8.2f Pr\n", l, t, r, b);
}

/* ------------- Hv_PSFrameRect ------*/

void Hv_PSFrameRect (Hv_Rect  *rect,
		     short    color)
    
/* Frame a rectangle in specified color.
   The rectangle's units are PS HIGH RES pixels*/
    
{
    short       L, T, R, B;
    float       l, t, r, b;
    
/* simple convert the corners from PS hi res pixels to PS true
   (float) coordinates and drawrect it */
    
    Hv_GetRectLTRB(rect, &L, &T, &R, &B);
    Hv_PSLocalToPS(L, T, &l, &t);
    Hv_PSLocalToPS(R, B, &r, &b);
    Hv_PSSetColor(color);
    fprintf(Hv_psfp, "%8.2f %8.2f %8.2f %8.2f Dr\n", l, t, r, b);
    
/* keep track of limit vars */
    
    ps_xmin = Hv_fMin(ps_xmin, l);
    ps_xmax = Hv_fMax(ps_xmax, r);
    ps_ymin = Hv_fMin(ps_ymin, b);
    ps_ymax = Hv_fMax(ps_ymax, t);
}

/* ------------- Hv_PSDrawLine ----------- */

void	Hv_PSDrawLine(short	x1,
		      short	y1,
		      short	x2,
		      short	y2,
		      short      color)          
    
/* Draw line from  x1 y1  to  x2 y2 in specified color.
   "color" is a pixel value into the color map. In our
   case it is either one of the constant colors such
   as Hv_black or Hv_gray12 or Hv_colors[index] */
    
/* units of x1..y2 are PS HI RES Pixels */
    
{
    float  fx1, fx2, fy1, fy2;
    
    Hv_PSLocalToPS(x1, y1, &fx1, &fy1);
    Hv_PSLocalToPS(x2, y2, &fx2, &fy2);
    Hv_PSSetColor(color); 
    fprintf(Hv_psfp, "%8.2f %8.2f %8.2f %8.2f Mtlt\n", fx1, fy1, fx2, fy2);
}


/* ------ Hv_PSFrameCircle -------*/

void Hv_PSFrameCircle(short     xc,
		      short     yc,
		      short     rad,
		      short     color)
    
/*  frames a circle. xc, yc and rad in PS hi res pixels. */
    
{
    float  psx, psy, psrad, fdum, psxo;
    short  sdum = 0;
    
    Hv_PSSetColor(color);
    Hv_PSLocalToPS(xc, yc, &psx, &psy);
    
    Hv_PSLocalToPS(0,   sdum, &psxo,  &fdum);
    Hv_PSLocalToPS(rad, sdum, &psrad, &fdum);
    psrad = (float)fabs(psrad - psxo);
    
    fprintf(Hv_psfp, "newpath %9.3f %9.3f %9.3f 0 360 arc stroke\n", psx, psy, psrad);
}

/*-------- Hv_PSDoOval ----------*/

static void Hv_PSDoOval(Hv_Rect *rect,
			short   color,
			Boolean fill)
    
{
    
    short     l, t, r, b;
    float     xc, yc;
    float     psl, pst, psr, psb, xrad, yrad;
    
    if ((rect == NULL) || (rect->width < 1) || (rect->height < 1))
	return;
    
    Hv_PSComment("OVAL");
    
    Hv_PSSetColor(color);
    
    Hv_GetRectLTRB(rect, &l, &t, &r, &b);
    Hv_PSLocalToPS(l, t, &psl, &pst);
    Hv_PSLocalToPS(r, b, &psr, &psb);
    xc = (float)((psl + psr)/2.0);
    yc = (float)((pst + psb)/2.0);
    
    xrad = xc - psl;
    yrad = yc - psb;
    Hv_Println("do oval (l, t, r, b) = (%f, %f, %f, %f)\n(xc, yc) = (%f, %f)\n(xrad, yrad) = (%f, %f)",
	    psl, pst, psr, psb, xc, yc, xrad, yrad);
    
    
    fprintf(Hv_psfp, "newpath\n");
    fprintf(Hv_psfp, "%f %f %f %f 0 360 ellipse\n", xc, yc, xrad, yrad);
    
/* fill or frame? */
    
    if (fill)
	fprintf(Hv_psfp, "fill\n");
    else
	fprintf(Hv_psfp, "stroke\n");
}

/*----------- Hv_PSFrameOval ---------*/

void            Hv_PSFrameOval(Hv_Rect *rect,
			       short    color)
    
/***************************************
  Draws a ps oval by scaling and then
  drawing a circle. Note that the rect
  is in PS local (hi res pixel) coordinates
  ***************************************/
    
{
    Hv_PSDoOval(rect, color, False);
}

/*------ Hv_PSFillOval ----------*/

void 	       Hv_PSFillOval(Hv_Rect *rect,
			     short    color)
    
{
    Hv_PSDoOval(rect, color, True);
}

/* ------ Hv_PSFillCircle -------*/

void Hv_PSFillCircle(short     xc,
		     short     yc,
		     short     rad,
		     short     color)
    
/*  fills a circle. xc, yc and rad in PS hi res pixels. */
    
{
    float  psx, psy, psrad, fdum, psxo;
    short  sdum = 0;
    
    if (color < 0)
	return;
    
    Hv_PSSetColor(color);
    Hv_PSLocalToPS(xc, yc, &psx, &psy);
    
    Hv_PSLocalToPS(0,   sdum, &psxo,  &fdum);
    Hv_PSLocalToPS(rad, sdum, &psrad, &fdum);
    psrad = (float)fabs(psrad - psxo);
    
    fprintf(Hv_psfp, "newpath %9.3f %9.3f %9.3f 0 360 arc gsave fill grestore\n", psx, psy, psrad);
}

/* ------ Hv_PSDrawLines ------ */


void Hv_PSDrawLines(Hv_Point *xp,
		    short   n,
		    short   color)
    
/* postscript equiv of XDrawLines
   points should be in the PSLocal (Hi res) system */
    
{
    short i;
    float  x, y;
    
    Hv_PSSetColor(color);
    fprintf(Hv_psfp, "newpath\n");
    
    Hv_PSLocalToPS(xp[0].x, xp[0].y, &x, &y);
    Hv_PSMoveTo(x, y);
    
    for (i = 1; i < n; i++){
	Hv_PSLocalToPS(xp[i].x, xp[i].y, &x, &y);
	fprintf(Hv_psfp, "%8.2f %8.2f lineto\n", x, y);
    }
    
    fprintf(Hv_psfp, "stroke\n");
}


/* ------ Hv_PSFramePolygon ------ */

void Hv_PSFramePolygon(Hv_Point    *xp,
		       short     np,
		       short     color)
    
/*  frames the poloygon */
    
/* postscript equiv of Hv_FramePolygon, points in hi res pixels */
    
{
    short   i;
    Hv_FPoint  *fpnts;
    
    fpnts = (Hv_FPoint *)Hv_Malloc(np*sizeof(Hv_FPoint));
    
    Hv_PSSetColor(color);
    
    for (i = 0; i < np; i++)
	Hv_PSLocalToPS(xp[i].x, xp[i].y, &(fpnts[i].h), &(fpnts[i].v));
    
    if (np == 4) {
	for (i = 0; i < 4; i++)
	    fprintf(Hv_psfp, "%8.2f %8.2f ", fpnts[i].h, fpnts[i].v);
	fprintf(Hv_psfp, "Dp4\n");
	return;
    }
    
    fprintf (Hv_psfp, "newpath\n  %8.2f %8.2f moveto\n", fpnts[0].h, fpnts[0].v);
    
    for (i = 1; i < np; i++)
	fprintf(Hv_psfp, "%8.2f %8.2f lineto\n", fpnts[i].h, fpnts[i].v);
    fprintf(Hv_psfp, "closepath\n");
    fprintf (Hv_psfp, "stroke\n");
    
    Hv_Free(fpnts);
    
}


/* ------ Hv_PSFillPolygon ------ */

void Hv_PSFillPolygon(Hv_Point    *xp,
		      short     np,
		      short     color)
    
/* postscript equiv of Hv_FillPolygon
   ASSUMPTION: points are in PS pixel system*/
    
{
    short   i;
    Hv_FPoint  *fpnts;
    
    if (color < 0)
	return;
    
    fpnts = (Hv_FPoint *)Hv_Malloc(np*sizeof(Hv_FPoint));
    
    Hv_PSSetColor(color);
    
    for (i = 0; i < np; i++)
	Hv_PSLocalToPS(xp[i].x, xp[i].y, &(fpnts[i].h), &(fpnts[i].v));
    
    if (np == 4) {
	for (i = 0; i < 4; i++)
	    fprintf(Hv_psfp, "%8.2f %8.2f ", fpnts[i].h, fpnts[i].v);
	fprintf(Hv_psfp, "Fp4\n");
    }
    else {
	fprintf (Hv_psfp, "newpath\n  %8.2f %8.2f moveto\n", fpnts[0].h, fpnts[0].v);
	
	for (i = 1; i < np; i++)
	    fprintf(Hv_psfp, "%8.2f %8.2f lineto\n", fpnts[i].h, fpnts[i].v);
	fprintf(Hv_psfp, "closepath\n");
	fprintf (Hv_psfp, " gsave fill grestore\n");
    }
    
    Hv_Free(fpnts);
}


/*-------- Hv_PSSetPattern ---------*/

void Hv_PSSetPattern(int   pattern,
                     short color)
    
{
    double r, g, b;
    
/* the first time a pattern is set, call the initializer */
    
    if (Hv_psFirstPattern) {
	Hv_PSPatternInit();
	Hv_psFirstPattern = False;
    }
    
    
    if ((pattern <= Hv_FILLSOLIDPAT) || (pattern >= Hv_HOLLOWPAT)) {
	Hv_PSRestore();
	Hv_okSetColor = True;   /*supress color changes */
    }
    else {
	Hv_PSSave();
	Hv_PSGetRGB(color, &r, &g, &b);
	Hv_okSetColor = False;   /*supress color changes */
	
	switch (pattern) {
	case Hv_80PERCENTPAT:
	    fprintf(Hv_psfp, "%6.4f %6.4f %6.4f Percent80 Sp\n", r, g, b);
	    break;
	    
	case Hv_50PERCENTPAT:
	    fprintf(Hv_psfp, "%6.4f %6.4f %6.4f Percent50 Sp\n", r, g, b);
	    break;
	    
	case Hv_20PERCENTPAT:
	    fprintf(Hv_psfp, "%6.4f %6.4f %6.4f Percent20 Sp\n", r, g, b);
	    break;
	    
	case Hv_HATCHPAT:
	    fprintf(Hv_psfp, "%6.4f %6.4f %6.4f Hatch Sp\n", r, g, b);
	    break;
	    
	case Hv_DIAGONAL1PAT:
	    fprintf(Hv_psfp, "%6.4f %6.4f %6.4f Dline1 Sp\n", r, g, b);
	    break;
	    
	case Hv_DIAGONAL2PAT:
	    fprintf(Hv_psfp, "%6.4f %6.4f %6.4f Dline2 Sp\n", r, g, b);
	    break;
	    
	case Hv_HSTRIPEPAT:
	    fprintf(Hv_psfp, "%6.4f %6.4f %6.4f Hline Sp\n", r, g, b);
	    break;
	    
	case Hv_VSTRIPEPAT:
	    fprintf(Hv_psfp, "%6.4f %6.4f %6.4f Vline Sp\n", r, g, b);
	    break;
	    
	case Hv_SCALEPAT:
	    fprintf(Hv_psfp, "%6.4f %6.4f %6.4f Scale Sp\n", r, g, b);
	    break;
	    
	case Hv_STARPAT:
	    fprintf(Hv_psfp, "%6.4f %6.4f %6.4f Star Sp\n", r, g, b);
	    break;
	    
	}
	
    }
}


/* ------ Hv_PSSetLineWidth ------- */

void Hv_PSSetLineWidth(int w)
    
/*set the line width to w hi res postscript pixels --
  not that a postscript pixel is 1/PS_PIXEL_DENSITY
  in 72nds of an inch -- thus one pixel is probably
  about 1/6th the size of a screen pixel */
    
{
    fprintf(Hv_psfp, "%f setlinewidth\n", ((w+3)*Hv_psUnitsPerInch)/Hv_PSPIXELDENSITY);
}

/* ------ Hv_PSSetLineStyle ------- */

void Hv_PSSetLineStyle(int w,
		       int style)
/*set the line width to w hi res postscript pixels --
  not that a postscript pixel is 1/PS_PIXEL_DENSITY
  in 72nds of an inch -- thus one pixel is probably
  about 1/6th the size of a screen pixel */
    
{
    
/* cache the old value to make as view changes as necessary */
    
    Hv_PSSetLineWidth(w);
    
    if (style == Hv_DASHED)
	fprintf(Hv_psfp, "[4] 0 setdash\n");
    else if (style == Hv_LONGDASHED)
	fprintf(Hv_psfp, "[8] 0 setdash\n");
    else if (style == Hv_DOTDASH)
	fprintf(Hv_psfp, "[8 3 2 3] 0 setdash\n");
    else
	fprintf(Hv_psfp, "[] 0 setdash\n");
}


/*------- Hv_PSMoveTo ------- */

void Hv_PSMoveTo(float x,
		 float y)
    
/* moves the pen to the specified location. x and y
   should be in PS (float) coordinates */
    
{
    fprintf(Hv_psfp, "%8.2f %8.2f moveto\n", x, y);
}
/*------- Hv_PSRMoveTo ------- */

void Hv_PSRMoveTo(float x,
		 float y)
    
/* moves the pen to the RELATIVE specified location. x and y
   should be in PS (float) coordinates */
    
{
    fprintf(Hv_psfp, "%8.2f %8.2f rmoveto\n", x, y);
}

/*------- Hv_PSDrawHCenteredString ------*/

void Hv_PSDrawHCenteredString(float xleft,
			      float xright ,
			      float y,
			      float fontsize,
			      char  *fontname,
			      char  *string,
			      short color)
    
{
    Hv_PSSetColor(color);
    fprintf(Hv_psfp, "\n/%s findfont %5.1f scalefont setfont\n", fontname, fontsize);
    fprintf(Hv_psfp, "(%s) %8.3f %8.3f %8.3f Hct\n", string, y, xleft, xright);
}

/*------- Hv_PSDrawCenteredString ------*/

void Hv_PSDrawCenteredString(float  xl,
			     float  xr,
			     float  yb,
			     float  yt,
			     float  fontsize,
			     char   *fontname,
			     char   *string,
			     short  color,
			     Boolean shadow,
			     short  shadowcolor)
    
/* centers both x and y */
    
{
    
/* if shadow, draw the shadow text first */
    
    if (shadow) {
	Hv_PSDrawCenteredString((float)(xl-SHADOW_SHIFT),
				(float)(xr-SHADOW_SHIFT),
				(float)(yb+SHADOW_SHIFT),
				(float)(yt+SHADOW_SHIFT),
				fontsize,
				fontname,
				string,
				shadowcolor,
				False,
				0);
	
	Hv_PSDrawCenteredString((float)(xl+SHADOW_SHIFT),
				(float)(xr+SHADOW_SHIFT),
				(float)(yb-SHADOW_SHIFT),
				(float)(yt-SHADOW_SHIFT),
				fontsize,
				fontname,
				string,
				shadowcolor,
				False,
				0);
    }
    
    Hv_PSSetColor(color);
    
/*only set font if not shadow, otherwise the shadow call would have already set it*/
    
    if (!shadow)
	fprintf(Hv_psfp, "\n/%s findfont %5.1f scalefont setfont\n", fontname, fontsize);
    
    fprintf(Hv_psfp, "(%s) %8.3f %8.3f %8.3f %8.3f %5.1f Ct\n", string, xl, xr, yb, yt, fontsize);
}

/*------- Hv_PSDrawString -------*/

void Hv_PSDrawString(float  x ,
		     float  y,
		     float  fontsize,
		     char   *fontname,
		     char   *string,
		     short  orientation,
		     short  color,
		     Boolean shadow,
		     short  shadowcolor)
    
{
    char  *pstr;
    
    if ((string == NULL) || (strlen(string) < 1))
	return;
    
    pstr = Hv_MakeProperPSString(string);
    
/* if shadow, draw the shadow text first */
    
    if (shadow) {
	Hv_PSDrawString((float)(x-SHADOW_SHIFT),
			(float)(y+SHADOW_SHIFT),
			fontsize,
			fontname,
			string,
			orientation,
			shadowcolor,
			False,
			0);
	
	Hv_PSDrawString((float)(x+SHADOW_SHIFT),
			(float)(y-SHADOW_SHIFT),
			fontsize,
			fontname,
			string,
			orientation,
			shadowcolor,
			False,
			0);
    }
    
    Hv_PSSetColor(color);
    
/*only set font if not shadow, otherwise the shadow call would have already set it*/
    
    
    
    if (!shadow)
	fprintf(Hv_psfp, "\n/%s findfont %5.1f scalefont setfont\n", fontname, fontsize);
    
    if (orientation == Hv_PSVERTICAL) {
/*	Hv_PSSave(); */
	fprintf (Hv_psfp, "\t90 rotate\n");
	
	if (!Hv_psnomoveto)
	    Hv_PSMoveTo(y, -x);
	else {
	    if ((Hv_psrelx != 0) || (Hv_psrely != 0)) {
		Hv_PSRMoveTo((float)Hv_psrelx, (float)Hv_psrely);
		Hv_psrelx = 0;
		Hv_psrely = 0;
	    }

	}
	

	fprintf(Hv_psfp, "(%s) show\n", pstr);
/*	Hv_PSRestore(); */
	fprintf (Hv_psfp, "\t-90 rotate\n"); 
    }
    else {
	if (!Hv_psnomoveto)
	    Hv_PSMoveTo(x, y);
	else {
	    if ((Hv_psrelx != 0) || (Hv_psrely != 0)) {
		Hv_PSRMoveTo((float)Hv_psrelx, (float)Hv_psrely);
		Hv_psrelx = 0;
		Hv_psrely = 0;
	    }
	}    
	fprintf(Hv_psfp, "(%s) show\n", pstr);
    }
    Hv_Free(pstr);
}

/* ------ Hv_PSDrawSegments ------ */

void Hv_PSDrawSegments(Hv_Segment  *segs,
		       short    n,
		       short    color)
    
/*   *segs in hi res PS pixels */
    
{
    short  i;
    
    Hv_PSComment("starting segments...");
    for (i = 0; i < n; i++)
	Hv_PSDrawLine(segs[i].x1, segs[i].y1, segs[i].x2, segs[i].y2, color);
    Hv_PSComment("done with segments.");
}


/*------- Hv_PSDrawWorldCircle -----*/

void Hv_PSDrawWorldCircle(Hv_View View,
			  float   fx,
			  float   fy,
			  float   frad,     
			  Boolean  Fill,
			  short   framecolor,
			  short   fillcolor)
    
/*
  
  Hv_View View           View pointer
  float     fx, fy       center
  float     frad         world radius
  Boolean   Fill
  short     framecolor   color of circle (frame)
  short     fillcolor    color of circle (fill)
  
  */
    
{
    short         xt, yt;     
    float         psx, psy, psrad, x2, y2;   
    
    
    Hv_WorldToPSLocal(fx, fy, &xt, &yt, View->world, View->local);
    Hv_PSLocalToPS(xt, yt, &psx, &psy);
    
    Hv_WorldToPSLocal(fx+frad, fy, &xt, &yt, View->world, View->local);
    Hv_PSLocalToPS(xt, yt, &x2, &y2);
    
    psrad = (float)fabs(x2 - psx);
    frad = (float)(100.0*frad*72.0/2.54);
    
    if (Fill && (fillcolor >= 0)) {
	Hv_PSSetColor(fillcolor);
	fprintf(Hv_psfp, "newpath %9.3f %9.3f %9.3f 0 360 arc gsave fill grestore\n", psx, psy, psrad);
    }
    
    Hv_PSSetColor(framecolor);
    fprintf(Hv_psfp, "newpath %9.3f %9.3f %9.3f 0 360 arc stroke\n", psx, psy, psrad);
    
}

#undef SHADOW_SHIFT
#undef SHADOW_COLOR
