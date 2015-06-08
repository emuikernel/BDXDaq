/**
 * <EM>Deals with writing (to a file) a postscript rendering of
 * the screen or a view.</EM>
 * <P>
 * The Hv library was developed at The Thomas Jefferson National
 * Accelerator Facility under contract to the
 * Department of Energy and is the property of they U.S. Government.
 * Partial support came from the National Science Foundation.
 * <P>
 * It may be used to develop commercial software, but the Hv
 * source code and/or compiled modules/libraries may not be sold.
 * <P>
 * Contains modifications provided by Ed Kearns -- said
 * modifications are marked with comments attributed
 * to "CEO". Don't know who CEO is, but THANKS.
 * <P>
 * Questions or comments should be directed
 * to <a href="mailto:heddle@cebaf.gov">heddle@cebaf.gov</a> <HR>
 */


/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.


**** Contains modifications provided by Ed Kearns -- said
     modifications are marked with comments attributed
     to "CEO". Don't know who CEO is, but THANKS  *************


It may be used to develop  commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/


/*
#define PSDEBUG
*/

#include "Hv.h"

static void Hv_PSGetBoundingBox(Hv_View   View,
				Hv_FRect *bb);

static void Hv_SetScale(float scalex,
			float scaley);

static void Hv_PSRectToPSLocalRect(Hv_FRect *psr,
				   Hv_Rect  *psloc);


static void Hv_PSGetScale(Hv_Rect  *br,
			  Hv_Rect  *psbr,
			  float    *scalex,
			  float    *scaley);

static void Hv_PSSetPSSystem(void);

static Boolean Hv_GetScreenBoundingRect(Hv_Rect  *br);

static void Hv_PSBlankLine(void);

static char  *Hv_PSTimeStamp(void);

static void Hv_PSHeader(char  *title,
			char  *creator,
			short  level,
			Hv_View View);

static void Hv_PSDefaults(void);

static void Hv_PSTrailer(void);

static void Hv_PSStartDef(char  *name);

static void Hv_PSDefLine(char  *line);

static void Hv_PSEndDef(void);

static void Hv_PSPrologProper(void);

static char *Hv_PSFooter(void);

static void Hv_CenterAndScale(Hv_Rect  *br,
			      Hv_Rect  *psbr,
			      Hv_FRect *bb);

static void Hv_PSDrawView(Hv_View   View);

static void Hv_PSCloseFile(void);

static Boolean Hv_PSInitFile(char   *fn, 
			     char   *footer,    
			     Hv_View View);

static int     savelevel = 1;

float         ps_xmax, ps_xmin, ps_ymax, ps_ymin; 
float         ps_scalex, ps_scaley, ps_delx, ps_dely;

Boolean       Hv_toPrinter = True;

/*-------- Hv_PSGetBoundingBox --------*/

static void Hv_PSGetBoundingBox(Hv_View   View,
				Hv_FRect *bb) {


  Hv_Rect    br, psbr;
  float      scalex, scaley;
  float      offset;
  short      coordTemp;        /* CEO */

/* if this is landscape, better rotate before calculating BB  CEO */

  if (!Hv_psTile) {

      if (View == NULL)  /* all views */
	  Hv_GetScreenBoundingRect(&br);
      else {
	  if (Hv_psEntireView)
	      Hv_CopyRect(&br, View->local);
	  else {
	      if (View->tag == Hv_XYPLOTVIEW)
		  Hv_HotRectItemsBoundary(View->items, &br);
	      else
		  Hv_CopyRect(&br, View->hotrect);
	  }
      }
      

      if (Hv_psOrientation == Hv_PSLANDSCAPE) {
	  coordTemp = br.left;
	  br.left = br.top;
	  br.top = coordTemp;
	  coordTemp = br.bottom;
	  br.bottom = br.right;
	  br.right = coordTemp;
	  br.width = br.right - br.left;
	  br.height = br.top - br.bottom;
      }                                                     /* CEO */
      
      Hv_LocalRectToPSLocalRect(&br, &psbr);

      Hv_PSLocalToPS(psbr.left,  psbr.bottom, &(bb->xmin), &(bb->ymin));
      Hv_PSLocalToPS(psbr.right, psbr.top,    &(bb->xmax), &(bb->ymax));
  }
  else { /* tiling */
      Hv_CopyFRect(bb, &Hv_psWorld);
  }


/* bb must be scaled so we do not have too much white space */

  Hv_PSGetScale(&br, &Hv_psLocal, &scalex, &scaley);
  
  bb->xmin *= scalex;
  bb->xmax *= scaley;
  offset = (float)((bb->xmin + bb->xmax - Hv_psWorld.width)*0.5);   /* CEO */
  offset = (float)(offset - 72.*Hv_PSLEFTMARGIN);                   /* CEO */
  bb->xmin -= offset;                                      /* CEO */
  bb->xmax -= offset;                                      /* CEO */
  
  bb->ymin *= scaley;
  bb->ymax *= scaley;
  offset = (float)((bb->ymin + bb->ymax - Hv_psWorld.height)*0.5);   /* CEO */
  offset = (float)(offset - 72.*Hv_PSBOTTOMMARGIN);                  /* CEO */
  bb->ymin -= offset;                                       /* CEO */
  bb->ymax -= offset;                                       /* CEO */
  Hv_FixFRectWH(bb);

#ifdef PSDEBUG
  Hv_Println("Bounding Rect [%d, %d, %d, %d] [%d, %d] ", br.left, 
	  br.top, br.right, br.bottom, br.height, br.width);
  
  Hv_Println("%%%%BoundingBox: %f %f %f %f",
	     bb->xmin, bb->ymin,
	     bb->xmax, bb->ymax);
#endif
  
  
}


/*-------- Hv_MakeProperPSString -------*/

char *Hv_MakeProperPSString(char *str)

/* add any needed '\'s */

{
  char  *pstr;
  char  *t, *s;

  if (str == NULL)
    return NULL;

  pstr = (char *)Hv_Malloc(2*(strlen(str) + 1));
  *pstr = '\0';
  t = str;
  s = str;
  
  while (*s != '\0') {
    while ((*t != '\0') && (*t != '(') && (*t != ')') && (*t != '\\'))
      t++;

    if (t > s)
      strncat(pstr, s, (t-s));
    
    if (*t != '\0') {
      strcat(pstr, "\\");
      strncat(pstr, t, 1);
      t++;
    }

    s = t;
  }

  return pstr;
}



/* --------- Hv_WorldToPSLocal --------------*/

void Hv_WorldToPSLocal(float     fx,
		       float     fy,
		       short     *x,
		       short     *y,
		       Hv_FRect	*world,
		       Hv_Rect     *local)

/* takes fx and fy, numbers relevant for a Views world
   system, and converts to the psLocal (hi res pixel) system

float	        fx, fy    passed world coordinates
short	        *x, *y    returned local coordinates
Hv_FRect	*world    world system 
Hv_Rect	        *local    local system 

*/

{
  float  xx, yy;

  xx = (float)(Hv_psLocal.left) + Hv_psResolutionFactor*((float)(local->left) + ((fx - world->xmin)/world->width)  * local->width);
  yy = (float)(Hv_psLocal.top)  + Hv_psResolutionFactor*((float)(local->top)  + ((world->ymax - fy)/world->height) * local->height);

  *x = Hv_InRange(xx);
  *y = Hv_InRange(yy);
}

/* --------- Hv_LocalToPSLocal --------------*/

void Hv_LocalToPSLocal(short     xloc,
		       short     yloc,
		       short     *xps,
		       short     *yps)   

/* converts local coordinates xloc and yloc to PS (higher
   resolution) locals xps and yps

short	        xloc, yloc    passed local coordinates
short	        *xps, *yps    returned PSLocal coordinates

*/

{
  *xps = Hv_psLocal.left + Hv_InRange(Hv_psResolutionFactor*xloc);
  *yps = Hv_psLocal.top +  Hv_InRange(Hv_psResolutionFactor*yloc);
}


/* --------- Hv_PSLocalToPS --------------*/

void Hv_PSLocalToPS(short  x,
		    short  y,
		    float  *psx,
		    float  *psy)


/* converts a point in the psLocal (high res pixel) system to
   the actual ps page system (default units: 72nds of inch) */

{
  *psx = Hv_psWorld.xmin + ((x - Hv_psLocal.left) * Hv_psWorld.width)/Hv_psLocal.width;
  *psy = Hv_psWorld.ymin + ((Hv_psLocal.bottom - y) * Hv_psWorld.height)/Hv_psLocal.height;
}


/*--------- Hv_PSRectToPSLocalRect -------*/

static void Hv_PSRectToPSLocalRect(Hv_FRect *psr,
				   Hv_Rect  *psloc) {

    Hv_PSToPSLocal(psr->xmin, psr->ymin, &(psloc->left), &(psloc->bottom));
    Hv_PSToPSLocal(psr->xmax, psr->ymax, &(psloc->right), &(psloc->top));
    Hv_FixRectWH(psloc);
}


/* --------- Hv_PSToPSLocal --------*/

void Hv_PSToPSLocal(float psx,
		    float psy,
		    short *x,
		    short *y)

/* coverts from true PS system to PS hi res Pixels */

{
  *x = Hv_InRange((float)Hv_psLocal.left +
		  (((psx - Hv_psWorld.xmin)/Hv_psWorld.width) * Hv_psLocal.width));
  *y = Hv_InRange((float)Hv_psLocal.top + 
		  (((Hv_psWorld.ymax - psy)/Hv_psWorld.height) * Hv_psLocal.height));
}



/* ---------- Hv_LocalPolygonToPSLocalPolygon --------*/

void Hv_LocalPolygonToPSLocalPolygon(Hv_Point *xp,
				     Hv_Point *psxp,
				     int   np)

{
  int i;

  for (i = 0; i < np; i++)
    Hv_LocalToPSLocal(xp[i].x, xp[i].y, &(psxp[i].x), &(psxp[i].y));
}

/*------------ Hv_LocalRectToPSLocalRect ----------*/

void Hv_LocalRectToPSLocalRect(Hv_Rect    *local,
			       Hv_Rect    *ps)


{
  Hv_LocalToPSLocal(local->left, local->top, &(ps->left), &(ps->top));
  Hv_LocalToPSLocal(local->right, local->bottom, &(ps->right), &(ps->bottom));
  Hv_FixRectWH(ps);
}

/*------------ Hv_LocalSegmentsToPSLocalSegments ----------*/

void Hv_LocalSegmentsToPSLocalSegments(Hv_Segment    *local,
				       Hv_Segment    *ps,
				       short      n)
{
  int i;

  for (i = 0; i < n; i++) {
    Hv_LocalToPSLocal(local[i].x1, local[i].y1, &(ps[i].x1), &(ps[i].y1));
    Hv_LocalToPSLocal(local[i].x2, local[i].y2, &(ps[i].x2), &(ps[i].y2));
  }
}

/*------------ Hv_LocalRectangleToPSLocalRectangle ----------*/

void Hv_LocalRectangleToPSLocalRectangle(Hv_Rectangle *local,
					 Hv_Rectangle *ps)

{
  Hv_LocalToPSLocal(local->x, local->y, &(ps->x), &(ps->y));
  ps->width =  (short)(Hv_psResolutionFactor*(local->width));
  ps->height = (short)(Hv_psResolutionFactor*(local->height));
}


/* ------ Hv_PSFont ------------ */

void Hv_PSFont(short    font,
	       char    *fontname,
	       float    *fontsize)

/* given a bitmap font specified by font, this tries to determine
   a reasonable substitute ps font and fontsize */

{

  float     scale;
  short     family, style, size;

  Hv_FontToFontParameters(font, &family, &size, &style);

  scale = (float)(Hv_psResolutionFactor*72.0/Hv_PSPIXELDENSITY);    /*converts screen pixels to "points" */

  if (style == Hv_PLAINFACE)
    scale *= (float)(0.85);
  else
    scale *= (float)(0.95);


/* scale to get the size in postscript units */

  if (size == Hv_FONTSIZE11)
    *fontsize = (float)(11.0*scale);
  else if (size == Hv_FONTSIZE14)
    *fontsize = (float)(14.0*scale);
  else if (size == Hv_FONTSIZE17)
    *fontsize = (float)(17.0*scale);
  else if (size == Hv_FONTSIZE20)
    *fontsize = (float)(20.0*scale);
  else if (size == Hv_FONTSIZE25)
    *fontsize = (float)(25.0*scale);
  
/* now get the font name */
  
  if (style == Hv_PLAINFACE) {
    if ((family == Hv_FIXEDFAMILY) || (family == Hv_COURIERFAMILY))
      strcpy(fontname, "Courier");
    else if (family == Hv_HELVETICAFAMILY)
      strcpy(fontname, "Helvetica");
    else if (family == Hv_TIMESFAMILY)
      strcpy(fontname, "Times-Roman");
    else if (family == Hv_SYMBOLFAMILY)
      strcpy(fontname, "Symbol");
  }
  else if (style == Hv_BOLDFACE) {
    if ((family == Hv_FIXEDFAMILY) || (family == Hv_COURIERFAMILY))
      strcpy(fontname, "Courier-Bold");
    else if (family == Hv_HELVETICAFAMILY)
      strcpy(fontname, "Helvetica-Bold");
    else if (family == Hv_TIMESFAMILY)
      strcpy(fontname, "Times-Bold");
    else if (family == Hv_SYMBOLFAMILY)
      strcpy(fontname, "Symbol");
  }
}

/* ------ Hv_PSGetRGB -------*/

void Hv_PSGetRGB(short color,
		 double *r,
		 double *g,
		 double *b)

{
	unsigned short rs, gs, bs;
    Hv_GetRGBFromColor(color, &rs, &gs, &bs);
    *r = ((double)rs)   / 65535;   /* get the red   component and scale it between 0 and 1 */
    *g = ((double)gs) / 65535;   /* get the green component and scale it between 0 and 1 */
    *b = ((double)bs)  / 65535;   /* get the blue  component and scale it between 0 and 1 */

}

/**
 * Hv_PSSetColor
 * @purpose  Set the postscript pen color.
 * @param    color   The color index.
 */

void Hv_PSSetColor(short color) {

/*pass the index color, NOT Hv_colors[color]. Will
  not do anything if color == Hv_psCurrentColor*/

  double  r, g, b;

  if (!Hv_okSetColor)
    return;

  if (Hv_psfp == NULL)
	  return;

/* sometimes (most) colors are set to black for printing on a B & W printer */
  
  if (Hv_printAllColorsBlack) {
    if ((color != Hv_white) && !((color >= Hv_gray15 ) && (color <= Hv_gray10)))
      color = Hv_black;
  }

  if ((color == Hv_psCurrentColor) || (color < 0))
    return;


  if (color == Hv_black)
    fprintf (Hv_psfp, "0 0 0 setrgbcolor\n");
  else if (color == Hv_white)
    fprintf (Hv_psfp, "1 1 1 setrgbcolor\n");
  else if (color == Hv_yellow)
    fprintf (Hv_psfp, "1 1 0 setrgbcolor\n");
  else if (color == Hv_red)
    fprintf (Hv_psfp, "1 0 0 setrgbcolor\n");
  else if (color == Hv_blue)
    fprintf (Hv_psfp, "0 0 1 setrgbcolor\n");
  else {
    Hv_PSGetRGB(color, &r, &g, &b);
    fprintf (Hv_psfp, "%6.4f %6.4f %6.4f setrgbcolor\n",r,g,b);
  }
  Hv_psCurrentColor = color;
}

/**
 * Hv_PSComment
 * @purpose   Print a comment in the postscript file.
 */

void Hv_PSComment(char *comment) {

/* just prints a comment in the default ps file Hv_psfp */

  if (Hv_psfp == NULL)
	  return;

  fprintf(Hv_psfp, "\n%%***  %s\n\n", comment);
}

/* -------- Hv_PSSave -----------*/

void Hv_PSSave(void) {

  if (Hv_psfp == NULL)
	  return;

#ifdef PSDEBUG  
  Hv_Println("  **calling SAVE    \t level: [%d]", savelevel);
#endif

  fprintf(Hv_psfp, "gsave\n");
  savelevel++;
}

/* -------- Hv_PSRestore -----------*/

void Hv_PSRestore(void) {

	if (Hv_psfp == NULL)
		return;

	if (savelevel == 1)
		return;

	savelevel--;

	Hv_psCurrentColor = -1;
	if (savelevel > 0)
		fprintf(Hv_psfp, "grestore\n");
#ifdef PSDEBUG  
	else
		Hv_Println("  **calling RESTORE \t level: [%d]", savelevel);
#endif
}


/*-------- Hv_PSFullClip ---------*/

void  Hv_PSFullClip(void) {

/*
 * note that if Hv_toPrinter is False (we are printing to a file)
 * then we CANNOT use initclip because that is not allowed in eps.
 */

	if (Hv_psfp == NULL)
	  return;

    if (Hv_toPrinter)
	fprintf(Hv_psfp, "initclip\n");
    else { /* EPS version */
	while (savelevel > 1)
	    Hv_PSRestore();
	Hv_PSSave();
    }
}


/* ------ Hv_PSClipPolygon ------ */

void Hv_PSClipPolygon(Hv_Point    *xp,
					  short     np) {

/* clips the poloygon */

  short   i;
  Hv_FPoint  *fpnts;

    if (Hv_psfp == NULL)
	  return;

#ifdef PSDEBUG
Hv_Println("\tin PSClipPolygon");
#endif

/*  Hv_PSSave(); */

  fpnts = (Hv_FPoint *)Hv_Malloc(np*sizeof(Hv_FPoint));

  for (i = 0; i < np; i++)
    Hv_PSLocalToPS(xp[i].x, xp[i].y, &(fpnts[i].h), &(fpnts[i].v));
  
  fprintf (Hv_psfp, "newpath\n  %8.2f %8.2f moveto\n", fpnts[0].h, fpnts[0].v);

  for (i = 1; i < np; i++)
    fprintf(Hv_psfp, "%8.2f %8.2f lineto\n", fpnts[i].h, fpnts[i].v);
  fprintf(Hv_psfp, "closepath\n");
  fprintf (Hv_psfp, "clip\n");

  Hv_Free(fpnts);
  
}

/* ------- Hv_PSClipRect ------- */

void      Hv_PSClipRect(Hv_Rect *rect) {

/*rect is in PS hi res pixels*/

  float       l, t, r, b;

  if (Hv_psfp == NULL)
	  return;

/*  Hv_PSSave(); */
  Hv_PSLocalToPS(rect->left, rect->top, &l, &t);
  Hv_PSLocalToPS(rect->right, rect->bottom, &r, &b);
  fprintf(Hv_psfp, "%8.2f %8.2f %8.2f %8.2f Cr\n", l, t, r, b);
}

/* ------- Hv_PSClipHotRect ------- */

void Hv_PSClipHotRect(Hv_View View) {

/* sets the clip region to the View's hotrect

   after all the graphics are drawn, should call
   grestore !!! */ 

  Hv_Rect   ps;

  Hv_LocalRectToPSLocalRect(View->hotrect, &ps);
  Hv_PSClipRect(&ps);
}



/* ------ Hv_PrintView --------- */
/* Modified to center/scale view -- CEO */

void Hv_PrintView (Hv_View   View,
		   Boolean   NotWholeScreen)
{
  char        *footer;
  Boolean      GoodOpen;
  char        *message;
  Hv_Rect     bndrect;

  Hv_inPostscriptMode = True;
  Hv_psFirstPattern = True;

/* note, if printing in B & W, turn off entire view printing
   (b&w always implies "contents only") */

  if (Hv_printAllColorsBlack)
    Hv_psEntireView = False;
  
  if (NotWholeScreen) {
    footer =  Hv_PSFooter();
    GoodOpen = Hv_PSInitFile(Hv_psFileName, footer, View);
    Hv_Free(footer);
/*    Hv_psEntireView = False; */  /* CEO added this to center figure */

    if (!GoodOpen) {
      Hv_inPostscriptMode = False;
      message = (char *)Hv_Malloc(50 + strlen(Hv_psFileName));
      sprintf(message, "Cannot open file %s", Hv_psFileName);
      Hv_Warning(message);
      Hv_Free(message);
      return;
    }

/* get the boundary rect, fix Hv_PSLocal to center/scale*/

    if (Hv_psEntireView)
	Hv_CenterAndScale(View->local, &Hv_psLocal, &Hv_psWorld);
    else {
	if (View->tag == Hv_XYPLOTVIEW) {
	    Hv_HotRectItemsBoundary(View->items, &bndrect);
	    Hv_CenterAndScale(&bndrect, &Hv_psLocal, &Hv_psWorld);
	}
	else
	    Hv_CenterAndScale(View->hotrect, &Hv_psLocal, &Hv_psWorld);
    }

  }  /*end of NotWholeScreen */


  if (Hv_psfp && Hv_inPostscriptMode)
    Hv_PSDrawView(View);
  
  if (NotWholeScreen)
    Hv_PSCloseFile(); 

  Hv_inPostscriptMode = False;
}

/* ------ Hv_PrintScreen --------- */

void Hv_PrintScreen (void)

{
  Hv_View      temp;
  char        *footer;
  Hv_Rect      bigrect, psrect;
  Boolean      GoodOpen;
  char        *message;
  int          count, nr, row, col, nc;
  Hv_FRect     bb, psworld;
  float        xt, yt, xg, yg, w, h;
  


  footer = Hv_PSFooter();
  GoodOpen = Hv_PSInitFile(Hv_psFileName, footer, NULL);
  Hv_Free(footer);

  
  if (!GoodOpen) {
	Hv_inPostscriptMode = False;
    message = (char *)Hv_Malloc(50 + strlen(Hv_psFileName));
    sprintf(message, "Cannot open file %s", Hv_psFileName);
    Hv_Warning(message);
    Hv_Free(message);
    return;
  }


/* as of 1.0054, added ability to tile views */

  if (Hv_psTile) {

      count = 0;

      for (temp = Hv_views.first; temp != NULL; temp = temp->next) {
	  if ((temp->tag != Hv_WELCOMEVIEW) && (temp->tag != Hv_VIRTUALVIEW)) {
	      if (Hv_ViewIsActive(temp))
		  count++;
	  }
      }
      
      if (count < 1) {
	  Hv_Println("No nontrivial views open. Not tiling.");
      }

      else { /* tiling */

/* figure out how many rows and columns */


	  Hv_CopyFRect(&bb, &Hv_psWorld);

	  Hv_PSRectToPSLocalRect(&bb, &psrect);

	  nc = Hv_iMin(count, Hv_iMax(1, (int)(floor(0.5 + sqrt((double)count)))));
	  nr = (count-1)/nc + 1;

	  Hv_CopyFRect(&psworld, &bb);

/* set up some tiling params */

	  xg = (float)(0.025*(psworld.width));  /*h gap */
	  yg = (float)(0.025*(psworld.height)); /*v gap */
	  w = (psworld.width  - (nc-1)*xg)/nc;
	  h = (psworld.height - (nr-1)*yg)/nr;

	  count = 0;
	  for (temp = Hv_views.first; temp != NULL; temp = temp->next) {
	      if ((temp->tag != Hv_WELCOMEVIEW) && (temp->tag != Hv_VIRTUALVIEW)) {
		  if (Hv_ViewIsActive(temp)) {
		      row = count/nc;
		      col = count%nc;

		      xt = psworld.xmin + col*(w + xg);
		      yt = psworld.ymax - h - row*(h + yg);

		      Hv_SetFRect(&bb, xt, xt+w, yt, yt+h);

		      Hv_PSRectToPSLocalRect(&bb, &psrect);


/* now draw the view in this reduced area */

		      
		      Hv_CenterAndScale(temp->hotrect, &psrect, &bb); 

		      Hv_PSComment("");
		      Hv_PSComment(Hv_GetViewTitle(temp));
		      Hv_PSComment("START TILED VIEW");
		      Hv_PSFullClip();

		      Hv_PrintView(temp, False); 
		      Hv_PSComment("FINISH TILED VIEW");

		      
		      fprintf(Hv_psfp, "\t%f %f translate\n", -ps_delx, -ps_dely);
		      Hv_SetScale((float)(1.0/ps_scalex), (float)(1.0/ps_scaley));


		      count++;
		  }
	      }
	      
	  }
	  Hv_PSCloseFile();

	  return;
      }  
  }

  if (!Hv_GetScreenBoundingRect(&bigrect))
    return;

  Hv_CenterAndScale(&bigrect, &Hv_psLocal, &Hv_psWorld);

  if (Hv_psfp != NULL) {
    for (temp = Hv_views.first; temp != NULL; temp = temp->next) {
      if (Hv_ViewIsVisible(temp))
	Hv_PrintView(temp, False);
    }
    
    Hv_PSCloseFile();
  }
}


/* ------ Hv_PSSetPSSystem ------*/

static void Hv_PSSetPSSystem(void)

/* set up the overall coordinate system */

/* Hv_psLocal system: a pixel system  containing roughly 600 pixels per inch */
/* Hv_psWorld system: a world system (aka PS) in units of 72ths of an inch */
{

  if (Hv_psOrientation == Hv_PSPORTRAIT) {
    
    Hv_SetFRect(&Hv_psWorld,
		  (float)(72*Hv_PSLEFTMARGIN), 
		  (float)(72*(8.5 - Hv_PSRIGHTMARGIN)),
		  (float)(72*Hv_PSBOTTOMMARGIN),
		  (float)(72*(11.0 - Hv_PSTOPMARGIN)));
    
    Hv_SetRect(&Hv_psLocal,
		 (short)(Hv_PSPIXELDENSITY*Hv_PSLEFTMARGIN),
		 (short)(Hv_PSPIXELDENSITY*Hv_PSTOPMARGIN),
		 (short)(Hv_PSPIXELDENSITY*Hv_psWorld.width/72),
		 (short)(Hv_PSPIXELDENSITY*Hv_psWorld.height/72));
    
  }
  
  else{ 
    
    Hv_SetFRect(&Hv_psWorld,
		  (float)(72*Hv_PSTOPMARGIN), 
		  (float)(72*(11.0 - Hv_PSBOTTOMMARGIN)),
		  (float)(72*Hv_PSLEFTMARGIN),
		  (float)(72*(8.5 - Hv_PSRIGHTMARGIN)));
    
    Hv_SetRect(&Hv_psLocal,
		 (short)(Hv_PSPIXELDENSITY*Hv_PSTOPMARGIN),
		 (short)(Hv_PSPIXELDENSITY*Hv_PSRIGHTMARGIN),
		 (short)(Hv_PSPIXELDENSITY*Hv_psWorld.width/72),
		 (short)(Hv_PSPIXELDENSITY*Hv_psWorld.height/72));

  }
}


/* ------ Hv_GetScreenBoundingRect ------ */

static Boolean Hv_GetScreenBoundingRect(Hv_Rect *br)

{
  Boolean      found =  False;
  Hv_View      temp;
  Hv_Rect      *srp;


  for (temp = Hv_views.first; temp != NULL; temp = temp->next) {
    if (Hv_ViewIsVisible(temp)) {
      if (!found) {
	found = True;

	if (Hv_psEntireView)
	  Hv_CopyRect(br, temp->local);
	else
	  Hv_CopyRect(br, temp->hotrect);
      }
      else {
	if (Hv_psEntireView)
	  srp = temp->local;
	else
	  srp = temp->hotrect;

	br->left   = Hv_sMin(br->left,   srp->left);
	br->top    = Hv_sMin(br->top,    srp->top);
	br->right  = Hv_sMax(br->right,  srp->right);
	br->bottom = Hv_sMax(br->bottom, srp->bottom);
	Hv_FixRectWH(br);
      }
    }
  }
  return found;
}


/*------ Hv_PSBlankLine ----------*/

static void Hv_PSBlankLine(void)

/* merely writes a blankline (i.e. "%   ") to the Hv_psfp */

{
	if (Hv_psfp == NULL)
		  return;

	fprintf(Hv_psfp, "%%\n");
}



/*------ Hv_PSTimeStamp  --------*/

static char   *Hv_PSTimeStamp(void)

/* returns a 26 char representation of the time
   DO NOT malloc a string before, the library
   routine ctime mallocs it */

{
  time_t     clock;
  char       *tstr;

  time(&clock);
  tstr = (char *)ctime(&clock);
  tstr[24] = ' ';
  return(tstr);
}


/*------- Hv_PSHeader --------*/

static void Hv_PSHeader(char  *title,
			char  *creator,
			short  level,
			Hv_View View) {

/* generate the postscript header. ASSUMPTIONS:
   1) userid in For:userid is same as creator
   2) pages and bounding box are deferred via (atend)
      to the trailer
   3) Language level is 2


   NOTE if Hv_toPrinter is False, then we are printing to 
   a file and should use the EPS header.

*/

   Hv_FRect  bb;

	if (Hv_psfp == NULL)
		  return;

    if (Hv_toPrinter) {
	fprintf (Hv_psfp, "%%!PS-Adobe-3.0\n");
	fprintf (Hv_psfp, "%%%%Title: %s\n", title);
	fprintf (Hv_psfp, "%%%%Creator: %s\n", creator);
	fprintf (Hv_psfp, "%%%%Pages: (atend)\n");
	fprintf (Hv_psfp, "%%%%BoundingBox: (atend)\n");
	fprintf (Hv_psfp, "%%%%LanguageLevel: %2d\n", level);
    }
    else {  /* EPS header */
	fprintf (Hv_psfp, "%%!PS-Adobe-3.0 EPSF-3.0\n");

      Hv_PSGetBoundingBox(View, &bb);

/* bounding box args are xmin, ymin xmax, ymax */

      fprintf (Hv_psfp, "%%%%BoundingBox: %f %f %f %f\n",
	       bb.xmin, bb.ymin,
	       bb.xmax, bb.ymax);

    }

}

/*-------Hv_PSDefaults -------*/

static void Hv_PSDefaults(void) {
	if (Hv_psfp == NULL)
		  return;

  fprintf(Hv_psfp, "%%%%BeginDefaults\n");
  fprintf(Hv_psfp, "%%%%EndDefaults\n");
  fprintf(Hv_psfp, "%%%%EndComments\n");
}

/*-------Hv_PSTrailer -------*/

static void Hv_PSTrailer(void) {
  if (Hv_psfp == NULL)
	  return;

  fprintf(Hv_psfp, "%%%%Trailer\n");
  fprintf(Hv_psfp, "%%%%EOF\n");  
}

/*------- Hv_PSStartDef -------- */

static void Hv_PSStartDef(char *name)

/* call when starting defintion for procedure named "name" */

{
  if (Hv_psfp == NULL)
	  return;

  Hv_PSBlankLine();
  fprintf(Hv_psfp, "/%s\n{\n", name);
}

/*------- Hv_PSDefLine ------- */

static void Hv_PSDefLine(char *line)

/* add a line to a definition of a procedure */

{
  if (Hv_psfp == NULL)
	  return;
  fprintf(Hv_psfp, "\t%s\n", line);
}

/*------- Hv_PSEndDef -------- */

static void Hv_PSEndDef(void)

/* call when done defining a procedure */


{
  if (Hv_psfp == NULL)
	  return;
  fprintf(Hv_psfp, "} def\n");
}

/*------- Hv_PSPrologProper -------- */

static void Hv_PSPrologProper(void)

/* generate the macros etc that go in the prolog */

{
  if (Hv_psfp == NULL)
	  return;

  fprintf(Hv_psfp,"%%%%BeginProlog\n");

/* the ellipse uses a dictionary */

  fprintf(Hv_psfp, "/ellipsedict 8 dict def\n");
  fprintf(Hv_psfp, "ellipsedict /mtrx matrix put\n");
  fprintf(Hv_psfp, "/ellipse\n");
  fprintf(Hv_psfp, " { ellipsedict begin\n");
  fprintf(Hv_psfp, "   /endangle exch def \n");
  fprintf(Hv_psfp, "   /startangle exch def \n");
  fprintf(Hv_psfp, "   /yrad exch def \n");
  fprintf(Hv_psfp, "   /xrad exch def \n");
  fprintf(Hv_psfp, "   /y exch def \n");
  fprintf(Hv_psfp, "   /x exch def \n");
  fprintf(Hv_psfp, "   /savematrix mtrx currentmatrix def \n");
  fprintf(Hv_psfp, "   x y translate \n");
  fprintf(Hv_psfp, "   xrad yrad scale \n");
  fprintf(Hv_psfp, "   0 0 1 startangle endangle arc \n");
  fprintf(Hv_psfp, "   savematrix setmatrix \n");
  fprintf(Hv_psfp, " end\n");
  Hv_PSEndDef();

/* Mtlt (move to line to) is self explanatory */

  Hv_PSStartDef("Mtlt");
  Hv_PSDefLine("newpath");
  Hv_PSDefLine("moveto");
  Hv_PSDefLine("lineto stroke");
  Hv_PSEndDef();


/* Dr draws a rectangle args: l,t,r,b in PS real coordinates */

  Hv_PSStartDef("Dr");
  Hv_PSDefLine("3 index 1 index 3 index 5 index 6 2 roll newpath");
  Hv_PSDefLine("moveto");
  Hv_PSDefLine("lineto");
  Hv_PSDefLine("lineto");
  Hv_PSDefLine("lineto closepath stroke");
  Hv_PSEndDef();

/* Cr sets a clip rectangle */

  Hv_PSStartDef("Cr");
  Hv_PSDefLine("3 index 1 index 3 index 5 index 6 2 roll newpath");
  Hv_PSDefLine("moveto");
  Hv_PSDefLine("lineto");
  Hv_PSDefLine("lineto");
  Hv_PSDefLine("lineto closepath clip");
  Hv_PSEndDef();

/* Pr paints a rectangle */

  Hv_PSStartDef("Pr");
  Hv_PSDefLine("3 index 1 index 3 index 5 index 6 2 roll newpath");
  Hv_PSDefLine("moveto");
  Hv_PSDefLine("lineto");
  Hv_PSDefLine("lineto");
  Hv_PSDefLine("lineto closepath gsave fill grestore");
  Hv_PSEndDef();

/* Dp4 frames any 4 sided polygon */

  Hv_PSStartDef("Dp4");
  Hv_PSDefLine("newpath");
  Hv_PSDefLine("moveto");
  Hv_PSDefLine("lineto");
  Hv_PSDefLine("lineto");
  Hv_PSDefLine("lineto closepath stroke");
  Hv_PSEndDef();

/* Fp4 fills any 4 sided polygon */

  Hv_PSStartDef("Fp4");
  Hv_PSDefLine("newpath");
  Hv_PSDefLine("moveto");
  Hv_PSDefLine("lineto");
  Hv_PSDefLine("lineto");
  Hv_PSDefLine("lineto closepath gsave fill grestore");
  Hv_PSEndDef();

/* Hct centers string between spec. margins at spec. height.
   EXAMPLE  [string y leftmargin rightmargin Hct -] Units:points*/

  Hv_PSStartDef("Hct");
  Hv_PSDefLine("3 index");
  Hv_PSDefLine("stringwidth pop");
  Hv_PSDefLine("2 div");
  Hv_PSDefLine("3 1 roll");
  Hv_PSDefLine("1 index sub");
  Hv_PSDefLine("2 div");
  Hv_PSDefLine("3 -1 roll");
  Hv_PSDefLine("sub add exch moveto show");
  Hv_PSEndDef();

/* Ct centers in specified box
   EXAMPLE  [string xl xr  yb yt fontsize Ct]*/

  Hv_PSStartDef("Ct");
  Hv_PSDefLine("0.8 mul");
  Hv_PSDefLine("sub");
  Hv_PSDefLine("add");
  Hv_PSDefLine("2 div");
  Hv_PSDefLine("3 1 roll");
  Hv_PSDefLine("exch");
  Hv_PSDefLine("Hct");
  Hv_PSEndDef();

  fprintf(Hv_psfp,"%%%%EndProlog\n");
}

/*------- Hv_PSFooter --------- */

static char *Hv_PSFooter(void)

{
  char   *tstr;    /*returned string*/
  char   *timestr;
  int    ulen = 0;
  int    nlen = 0;

/* if the user doesn't even want a footer,
   return NULL */

  if (!Hv_psUseFooter)
    return NULL;

  timestr = Hv_PSTimeStamp();

/* Hv_userName holds the user's name (or NULL).
  Hv_psNote is a extra comment available for the user */

  if (Hv_userName)
    ulen = strlen(Hv_userName);
  if (Hv_psNote)
    nlen = strlen(Hv_psNote);

  tstr = (char *)Hv_Malloc(strlen(Hv_programName) + strlen(timestr) + ulen + nlen + 20);

  strcpy(tstr, Hv_programName);
  if (Hv_userName) {
    strcat(tstr, "[");
    strcat(tstr, Hv_userName);
    strcat(tstr, "]  ");
  }

  strcat(tstr, "  ");
  strcat(tstr, timestr);
  strcat(tstr, "  ");

  if (Hv_psNote) {
    strcat(tstr, "  ");
    strcat(tstr, Hv_psNote);
  }

  return tstr;
}


/* -------- Hv_PSGetScale -------*/
/* Modified to allow for scaling > 1 to fill the page -- CEO */

static void Hv_PSGetScale(Hv_Rect  *br,
			  Hv_Rect  *psbr,
			  float    *scalex,
			  float    *scaley) {

/*************************************
  Gets overall scale factor. br
  is the bounding rect in NORMAL
  SCREEN COORDINATES. psbr is the rect
  in hi res pixels that br should map to
*************************************/

  float scale;

  *scalex = (Hv_psResolutionFactor*br->width)/psbr->width;
  *scaley = (Hv_psResolutionFactor*br->height)/psbr->height;

/* I want to fill the page.  Don't limit this to a maximum of 1. CEO */

  scale = (float)(1.0/Hv_fMax(*scalex, *scaley));

/* yes limit if expand to fit not selected */

  if (!Hv_psExpandToFit) {
      scale = Hv_fMin(1.0, scale);
      *scalex = scale;
      *scaley = scale;
  }
  else {
      if (Hv_psTile) {
	  *scalex = (float)(1.0/(*scalex));
	  *scaley = (float)(1.0/(*scaley));
      }
      else {
	  *scalex = scale;
	  *scaley = scale;
      }
  }

}

/*--------- Hv_SetScale ----------*/

static void Hv_SetScale(float scalex,
			float scaley) {
  if (Hv_psfp == NULL)
	  return;

    ps_scalex = scalex;
    ps_scaley = scaley;

    Hv_psUnitsPerInch = (float)(72.0/Hv_fMin(scalex, scaley));
    Hv_PSSetLineWidth(3);
    Hv_PSComment("scale to fit");
    fprintf(Hv_psfp, "\t%f %f scale\n", ps_scalex, ps_scaley);
}


/* ------ Hv_CenterAndScale -------- */

static void Hv_CenterAndScale(Hv_Rect  *br,
			      Hv_Rect  *psbr,
			      Hv_FRect *bb)

{
  float psxc, psyc;
  short brxc, bryc;   /*centers */
  float fbrx, fbry;
  float scalex, scaley;

    if (Hv_psfp == NULL)
	  return;

/* scale to fit */

  Hv_PSGetScale(br, psbr, &scalex, &scaley);
  Hv_SetScale(scalex, scaley);

  Hv_PSComment("translate to center");

  psxc = (float)((bb->xmin + bb->xmax) / (2.0 * ps_scalex));
  psyc = (float)((bb->ymin + bb->ymax) / (2.0 * ps_scaley));
  brxc = (br->left + br->right) / 2; 
  bryc = (br->top  + br->bottom) / 2; 

  Hv_LocalToPSLocal(brxc, bryc, &brxc, &bryc);  /*center of bounding rect in high res pix*/
  Hv_PSLocalToPS(brxc, bryc, &fbrx, &fbry);     /*center of bounding rect in ps coordinates */

  ps_delx = psxc - fbrx;
  ps_dely = psyc - fbry;

  fprintf(Hv_psfp, "\t%f %f translate\n", ps_delx, ps_dely);

}

/*------- Hv_PSDrawView -----*/

static void Hv_PSDrawView(Hv_View View)

{
  Hv_Rect       pshr;

  if (!Hv_psEntireView && (View->tag == Hv_WELCOMEVIEW))
    return;
  
  Hv_PSSave();
  Hv_PSComment("Starting a new View");

/* if contents only, paint a white background */

  if (!Hv_psEntireView){
    Hv_LocalRectToPSLocalRect(View->hotrect, &pshr);    /*hot rect in PS hi res pixels*/
    Hv_PSComment("painting view background");
    Hv_PSFillRect(&pshr, Hv_white);
  }

  Hv_DrawView(View, NULL);

/* if not drawing everything, at least should put a box */

  if (!Hv_psEntireView)
    if (View->tag != Hv_XYPLOTVIEW) {
      Hv_PSComment("framing view");
      Hv_PSFrameRect(&pshr, Hv_black);
    }
  
  Hv_PSRestore();
}


/*------- Hv_PSCloseFile -------*/


static void Hv_PSCloseFile(void)

{
  if (Hv_psfp == NULL)
	  return;

  fprintf (Hv_psfp, "\tshowpage\n");
  Hv_PSTrailer();
  fclose(Hv_psfp);
}

/*------- Hv_PSInitFile ---------*/

static Boolean Hv_PSInitFile(char   *fn, 
			     char   *footer,    
			     Hv_View View)
/**************
  char     *fn        filename
  char     *footer    writes at bottom of page
  Hv_View  View       NULL-> hole screen
**************/

{
  short   x1, y1, x2, y2;


  if ((Hv_psfp = fopen (fn, "w")) == NULL)
    return False;

#ifdef PSDEBUG
  Hv_Println("ps file: [%s]", fn);
#endif

/* first, generate the comments and the prolog */

  if (Hv_toPrinter) {
      Hv_Println("Printing to printer.");
  }
  else {
      Hv_Println("Printing to file only.");
  }




  Hv_psUnitsPerInch = 72.0;
  Hv_psCurrentColor = -2;
  Hv_PSSetPSSystem();
  Hv_PSHeader("", "", 2, View);
  Hv_PSDefaults();
  Hv_PSPrologProper();

/* now some orientation specific initialization */

  if (Hv_psOrientation == Hv_PSLANDSCAPE) {
    fprintf(Hv_psfp, "%% landscape translate & rotate  origin ");
    fprintf(Hv_psfp, "\t612  0 translate\n"); /*612 -> 8.5 inches*/
    fprintf(Hv_psfp, "\t90 rotate\n");
  }

/* init the limit variables */

  ps_xmin =  (float)(1.0e30);
  ps_xmax = (float)(-1.0e30);
  ps_ymin =  (float)(1.0e30);
  ps_ymax = (float)(-1.0e30);
  ps_scalex = (float)(1.0);
  ps_scaley = (float)(1.0);

  Hv_PSSetLineWidth(3);
  fprintf(Hv_psfp, "%% End of Initialization\n");
  
/* if a footer was provided, write it at the bottom (not for eps)*/

  if (footer != NULL) {
      Hv_PSToPSLocal(Hv_psWorld.xmin, Hv_psWorld.ymin-3, &x1, &y1);
      Hv_PSToPSLocal(Hv_psWorld.xmax, Hv_psWorld.ymin-3, &x2, &y2);
      Hv_PSDrawLine(x1, y1, x2, y2, Hv_black);
      Hv_PSDrawHCenteredString(Hv_psWorld.xmin, Hv_psWorld.xmax, (float)(Hv_psWorld.ymin-12.0),
			       (float)(9.1), "Times-Roman", footer,  Hv_black);
  }
  
  return True;
}

/*---------- Hv_PSPatternInit -------*/

void    Hv_PSPatternInit(void)

{
  if (Hv_psfp == NULL)
	  return;

  fprintf(Hv_psfp, "/Percent80Proto 8 dict def\n");
  fprintf(Hv_psfp, "Percent80Proto begin\n");
  fprintf(Hv_psfp, "  /PatternType 1 def\n");
  fprintf(Hv_psfp, "  /PaintType 2 def\n");
  fprintf(Hv_psfp, "  /TilingType 1 def\n");
  fprintf(Hv_psfp, "  /BBox [-200 -200 200 200] def\n");
  fprintf(Hv_psfp, "  /XStep 400  def\n");
  fprintf(Hv_psfp, "  /YStep 400 def\n");
  fprintf(Hv_psfp, "  /PaintProc {\n");
  fprintf(Hv_psfp, "    pop\n");
  fprintf(Hv_psfp, "    100 setlinewidth\n");
  fprintf(Hv_psfp, "   -100 200 200 200 Mtlt\n");
  fprintf(Hv_psfp, "   -200 100 0 100 Mtlt\n");
  fprintf(Hv_psfp, "   100 100 200 100 Mtlt\n");
  fprintf(Hv_psfp, "   -100 0 200 0 Mtlt\n");
  fprintf(Hv_psfp, "   -200 -100 0 -100 Mtlt\n");
  fprintf(Hv_psfp, "   100 -100 200 -100 Mtlt\n");
  fprintf(Hv_psfp, "  } def\n");
  fprintf(Hv_psfp, "end\n");
  fprintf(Hv_psfp, "\n");
  fprintf(Hv_psfp, "/Percent50Proto 8 dict def\n");
  fprintf(Hv_psfp, "Percent50Proto begin\n");
  fprintf(Hv_psfp, "  /PatternType 1 def\n");
  fprintf(Hv_psfp, "  /PaintType 2 def\n");
  fprintf(Hv_psfp, "  /TilingType 1 def\n");
  fprintf(Hv_psfp, "  /BBox [-100 -100 100 100] def\n");
  fprintf(Hv_psfp, "  /XStep 200  def\n");
  fprintf(Hv_psfp, "  /YStep 200 def\n");
  fprintf(Hv_psfp, "  /PaintProc {\n");
  fprintf(Hv_psfp, "    pop\n");
  fprintf(Hv_psfp, "    100 setlinewidth\n");
  fprintf(Hv_psfp, "   -100 100 0 100 Mtlt\n");
  fprintf(Hv_psfp, "    0 0 100 10 Mtlt\n");
  fprintf(Hv_psfp, "  } def	\n");
  fprintf(Hv_psfp, "end\n");
  fprintf(Hv_psfp, "\n");
  fprintf(Hv_psfp, "/Percent20Proto 8 dict def\n");
  fprintf(Hv_psfp, "Percent20Proto begin\n");
  fprintf(Hv_psfp, "  /PatternType 1 def\n");
  fprintf(Hv_psfp, "  /PaintType 2 def\n");
  fprintf(Hv_psfp, "  /TilingType 1 def\n");
  fprintf(Hv_psfp, "  /BBox [-200 -200 200 200] def\n");
  fprintf(Hv_psfp, "  /XStep 400  def\n");
  fprintf(Hv_psfp, "  /YStep 400 def\n");
  fprintf(Hv_psfp, "  /PaintProc {\n");
  fprintf(Hv_psfp, "    pop\n");
  fprintf(Hv_psfp, "    100 setlinewidth\n");
  fprintf(Hv_psfp, "   -200 200 -100 200 Mtlt\n");
  fprintf(Hv_psfp, "    0 100 100 200 Mtlt\n");
  fprintf(Hv_psfp, "   -200 0 -100 0 Mtlt\n");
  fprintf(Hv_psfp, "    0 -100 100 -100 Mtlt\n");
  fprintf(Hv_psfp, "  } def\n");
  fprintf(Hv_psfp, "end\n");
  fprintf(Hv_psfp, "\n");
  fprintf(Hv_psfp, "/ScaleProto 8 dict def\n");
  fprintf(Hv_psfp, "ScaleProto begin\n");
  fprintf(Hv_psfp, "  /PatternType 1 def\n");
  fprintf(Hv_psfp, "  /PaintType 2 def\n");
  fprintf(Hv_psfp, "  /TilingType 1 def\n");
  fprintf(Hv_psfp, "  /BBox [-400 -400 400 400] def\n");
  fprintf(Hv_psfp, "  /XStep 800  def\n");
  fprintf(Hv_psfp, "  /YStep 800 def\n");
  fprintf(Hv_psfp, "  /PaintProc {\n");
  fprintf(Hv_psfp, "    pop\n");
  fprintf(Hv_psfp, "    100 setlinewidth\n");
  fprintf(Hv_psfp, "    -400 400 -300 400 Mtlt\n");
  fprintf(Hv_psfp, "    -400 300 -300 300 Mtlt\n");
  fprintf(Hv_psfp, "    -300 200 -200 200 Mtlt\n");
  fprintf(Hv_psfp, "     300 200 400 200 Mtlt\n");
  fprintf(Hv_psfp, "    -200 100 300 100 Mtlt\n");
  fprintf(Hv_psfp, "    0 0 100 0 Mtlt\n");
  fprintf(Hv_psfp, "    0 -100 100 -100 Mtlt\n");
  fprintf(Hv_psfp, "    -100 -200 0 -200 Mtlt\n");
  fprintf(Hv_psfp, "    100 -200 200 -200 Mtlt\n");
  fprintf(Hv_psfp, "    -400 -300 -100 -300 Mtlt\n");
  fprintf(Hv_psfp, "    200 -300 400 -300 Mtlt\n");
  fprintf(Hv_psfp, "  } def\n");
  fprintf(Hv_psfp, "end\n");
  fprintf(Hv_psfp, "\n");
  fprintf(Hv_psfp, "/HatchProto 8 dict def\n");
  fprintf(Hv_psfp, "HatchProto begin\n");
  fprintf(Hv_psfp, "  /PatternType 1 def\n");
  fprintf(Hv_psfp, "  /PaintType 2 def\n");
  fprintf(Hv_psfp, "  /TilingType 1 def\n");
  fprintf(Hv_psfp, "  /BBox [-400 -400 400 400] def\n");
  fprintf(Hv_psfp, "  /XStep 800  def\n");
  fprintf(Hv_psfp, "  /YStep 800 def\n");
  fprintf(Hv_psfp, "  /PaintProc {\n");
  fprintf(Hv_psfp, "    pop\n");
  fprintf(Hv_psfp, "    100 setlinewidth\n");
  fprintf(Hv_psfp, "    -400 400 0 400 Mtlt\n");
  fprintf(Hv_psfp, "    300  400  400 400 Mtlt\n");
  fprintf(Hv_psfp, "    -400 300  100 300 Mtlt\n");
  fprintf(Hv_psfp, "    -300 200  -100 200 Mtlt\n");
  fprintf(Hv_psfp, "    0 200  200 200 Mtlt\n");
  fprintf(Hv_psfp, "    -400 100  -200 100 Mtlt\n");
  fprintf(Hv_psfp, "    100 100  300 100 Mtlt\n");
  fprintf(Hv_psfp, "    -400 0  -300 0 Mtlt\n");
  fprintf(Hv_psfp, "    0 0  400 0 Mtlt\n");
  fprintf(Hv_psfp, "    -100 -100  400 -100 Mtlt\n");
  fprintf(Hv_psfp, "    -200 -200 0 -200 Mtlt\n");
  fprintf(Hv_psfp, "    100 -200 300 -200 Mtlt\n");
  fprintf(Hv_psfp, "    -300 -300 -100 -300 Mtlt\n");
  fprintf(Hv_psfp, "    200 -300 400 -300 Mtlt\n");
  fprintf(Hv_psfp, "  } def\n");
  fprintf(Hv_psfp, "end\n");
  fprintf(Hv_psfp, "\n");
  fprintf(Hv_psfp, "/OneLineProto 8 dict def\n");
  fprintf(Hv_psfp, "OneLineProto begin\n");
  fprintf(Hv_psfp, "  /PatternType 1 def\n");
  fprintf(Hv_psfp, "  /PaintType 2 def\n");
  fprintf(Hv_psfp, "  /TilingType 1 def\n");
  fprintf(Hv_psfp, "  /BBox [-250 -250 250 250] def\n");
  fprintf(Hv_psfp, "  /XStep 500  def\n");
  fprintf(Hv_psfp, "  /YStep 500 def\n");
  fprintf(Hv_psfp, "  /PaintProc {\n");
  fprintf(Hv_psfp, "    pop\n");
  fprintf(Hv_psfp, "    100 setlinewidth\n");
  fprintf(Hv_psfp, "    -250 -125 250 -125  Mtlt\n");
  fprintf(Hv_psfp, "    -250 125 250 125  Mtlt\n");
  fprintf(Hv_psfp, "  } def\n");
  fprintf(Hv_psfp, "end\n");
  fprintf(Hv_psfp, "\n");
  fprintf(Hv_psfp, "/OneStarProto 8 dict def\n");
  fprintf(Hv_psfp, "OneStarProto begin\n");
  fprintf(Hv_psfp, "  /PatternType 1 def\n");
  fprintf(Hv_psfp, "  /PaintType 2 def\n");
  fprintf(Hv_psfp, "  /TilingType 1 def\n");
  fprintf(Hv_psfp, "  /BBox [-250 -250 250 250] def\n");
  fprintf(Hv_psfp, "  /XStep 600  def\n");
  fprintf(Hv_psfp, "  /YStep 600 def\n");
  fprintf(Hv_psfp, "  /PaintProc {\n");
  fprintf(Hv_psfp, "    pop\n");
  fprintf(Hv_psfp, "    0 -250 0 -80  Mtlt\n");
  fprintf(Hv_psfp, "    -250 0 -80 0  Mtlt\n");
  fprintf(Hv_psfp, "    0 80 0 250  Mtlt\n");
  fprintf(Hv_psfp, "    80 0 250 0  Mtlt\n");
  fprintf(Hv_psfp, "    -177 -177 -56.6 -56.6  Mtlt\n");
  fprintf(Hv_psfp, "    -177  177 -56.6 56.6 Mtlt\n");
  fprintf(Hv_psfp, "    177 177 56.6 56.6  Mtlt\n");
  fprintf(Hv_psfp, "    177  -177 56.6 -56.6 Mtlt\n");
  fprintf(Hv_psfp, "    newpath 0 0 80 0 360 arc stroke\n");
  fprintf(Hv_psfp, "\n");
  fprintf(Hv_psfp, "  } def\n");
  fprintf(Hv_psfp, "end\n");
  fprintf(Hv_psfp, "\n");

  fprintf(Hv_psfp, "Percent80Proto\n");
  fprintf(Hv_psfp, "[0.0075 0 0 0.0075 0 0]\n");
  fprintf(Hv_psfp, "makepattern\n");
  fprintf(Hv_psfp, "/Percent80 exch def\n\n");

  fprintf(Hv_psfp, "Percent50Proto\n");
  fprintf(Hv_psfp, "[0.0075 0 0 0.0075 0 0]\n");
  fprintf(Hv_psfp, "makepattern\n");
  fprintf(Hv_psfp, "/Percent50 exch def\n\n");

  fprintf(Hv_psfp, "Percent20Proto\n");
  fprintf(Hv_psfp, "[0.0075 0 0 0.0075 0 0]\n");
  fprintf(Hv_psfp, "makepattern\n");
  fprintf(Hv_psfp, "/Percent20 exch def\n\n");

  fprintf(Hv_psfp, "OneStarProto\n");
  fprintf(Hv_psfp, "[0.015 0 0 0.015 0 0]\n");
  fprintf(Hv_psfp, "makepattern\n");
  fprintf(Hv_psfp, "/Star exch def\n\n");

  fprintf(Hv_psfp, "OneLineProto\n");
  fprintf(Hv_psfp, "[0.015 0 0 0.015 0 0]\n");
  fprintf(Hv_psfp, "makepattern\n");
  fprintf(Hv_psfp, "/Hline exch def\n\n");

  fprintf(Hv_psfp, "OneLineProto\n");
  fprintf(Hv_psfp, "[0 0.015 -0.015 0 0 0]\n");
  fprintf(Hv_psfp, "makepattern\n");
  fprintf(Hv_psfp, "/Vline exch def\n\n");

  fprintf(Hv_psfp, "OneLineProto\n");
  fprintf(Hv_psfp, "[0.010607 -0.010607 0.010607 0.010607 0 0]\n");
  fprintf(Hv_psfp, "makepattern\n");
  fprintf(Hv_psfp, "/Dline1 exch def\n\n");

  fprintf(Hv_psfp, "OneLineProto\n");
  fprintf(Hv_psfp, "[0.010607 0.010607 -0.010607 0.010607 0 0]\n");
  fprintf(Hv_psfp, "makepattern\n");
  fprintf(Hv_psfp, "/Dline2 exch def\n\n");

  fprintf(Hv_psfp, "HatchProto\n");
  fprintf(Hv_psfp, "[0.0075 0 0 0.0075 0 0]\n");
  fprintf(Hv_psfp, "makepattern\n");
  fprintf(Hv_psfp, "/Hatch exch def\n\n");

  fprintf(Hv_psfp, "ScaleProto\n");
  fprintf(Hv_psfp, "[0.0075 0 0 0.0075 0 0]\n");
  fprintf(Hv_psfp, "makepattern\n");
  fprintf(Hv_psfp, "/Scale exch def\n\n");

/* Sp sets a pattern */

  Hv_PSStartDef("Sp");
  Hv_PSDefLine("[");
  Hv_PSDefLine("/Pattern");
  Hv_PSDefLine("/DeviceRGB");
  Hv_PSDefLine("] setcolorspace");
  Hv_PSDefLine("setcolor");
  Hv_PSEndDef();

}
