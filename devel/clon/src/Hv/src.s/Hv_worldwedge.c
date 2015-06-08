/**
 * <EM>Deals with creating, drawing, etc. for world coordinate based wedges</EM>
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
#include "Hv_maps.h"
#include "Hv_xyplot.h"
#include "Hv_drag.h"

/*#define DEBUG 1*/

static void TextCB(Hv_Widget w);

static Hv_Widget AddTextEditTriplet(Hv_Widget       parent,
				    char            *label,
				    char            *unitstr,
				    Hv_FunctionPtr  vccb);

static float Hv_AzimuthFromWW(Hv_WorldWedgeData * ww);

static void       Hv_EditWorldWedgeColor(Widget);

static void       Hv_EditWorldWedgePattern(Widget);

static 	void Hv_GetWedgeMap(Hv_Item           Item,
			    Hv_WorldWedgeData *ww);


static void Hv_SetEllipseMapPoints(Hv_View View,
				   Hv_Map  map,
				   float latitude,
				   float longitude,
				   float xrad,
				   float yrad,
				   float azimuth);

/*------ variables global for this file --------*/

Boolean           ignorecb = True;

static short      newpat;
static Hv_Widget     patternlabel;

static short      newlinecolor,   newfillcolor;
static Hv_Widget     linecolorlabel, fillcolorlabel;

static Hv_Widget  innerradw;
static Hv_Widget  outerradw;
static Hv_Widget  azim1w;
static Hv_Widget  azim2w;
static Hv_Widget  azimuthw;
static int        wwdir;
static Hv_Widget  xradw;
static Hv_Widget  yradw;

static Boolean    yesdraw = True;


Hv_Item Hv_CreateMapEllipseWithParent(Hv_View View,
                                      Hv_Item parent,
                                      float   latitude,  /* in rads */
                                      float   longitude, /* in rads */
                                      float   xrad, /* in km */
                                      float   yrad, /* in km */
                                      float   azimuth /* in rads */) {
  Hv_WorldWedgeData   *ww;
  Hv_Item             newItem;
  
  ww = (Hv_WorldWedgeData *)Hv_Malloc(sizeof(Hv_WorldWedgeData));
  
  ww->innerrad = 0.0;
  ww->radius = 0.0;
  ww->azim1 = 0.0;
  ww->azim2 = 0.0;
  
  ww->xrad = xrad;
  ww->yrad = yrad;
  ww->map = NULL;
  ww->mode = Hv_ELLIPSEMODE;
  
  
  /* basepoint stores lat and long for map views */
  
  Hv_SetFPoint(&(ww->basepoint), latitude, longitude);
  
  newItem = Hv_VaCreateItem(View,
    Hv_PARENT,       parent,
    Hv_TAG,          Hv_DRAWINGTOOLTAG,
    Hv_TYPE,         Hv_WORLDWEDGEITEM,
    Hv_DATA,         ww,
    Hv_DOMAIN,       Hv_INSIDEHOTRECT,
    Hv_DRAWCONTROL,  Hv_ROTATABLE + Hv_ENLARGEABLE + Hv_ZOOMABLE + Hv_FANCYDRAG,
    Hv_AFTERDRAG,    Hv_WedgeAfterDrag,
    Hv_AFTERROTATE,  Hv_WedgeAfterRotate,
    Hv_AFTEROFFSET,  Hv_WedgeAfterOffset,
    Hv_FIXREGION,    Hv_FixWedgeRegion,
    NULL);
  
  /* now set the properties based on the view's defaults */
  
  if (Hv_usedNewDrawTools) {
    ww = (Hv_WorldWedgeData *)(newItem->data);
    ww->fillcolor = View->hotfillcolor;
    newItem->pattern = View->hotpattern;
    newItem->color = View->hotbordercolor;
    ww->thickness = View->hotlinewidth;
    ww->style = View->hotlinestyle;
    Hv_usedNewDrawTools = False;
  }


/*===
 * see if the user wants to be notified
 *===*/

    if (Hv_CreateDrawingItemCB != NULL)
	Hv_CreateDrawingItemCB(newItem);
    
    Hv_DrawItem(newItem, NULL); 

  return newItem;
}

/*--------------------------------------
 * Hv_CreateMapEllipse convenience routine
 * for creating an ellipse -- uses a world
 * wedge item
 *--------------------------------------*/


Hv_Item Hv_CreateMapEllipse(Hv_View View,
			    float   latitude,  /* in rads */
			    float   longitude, /* in rads */
			    float   xrad, /* in km */
			    float   yrad, /* in km */
			    float   azimuth /* in rads */) {

  return Hv_CreateMapEllipseWithParent(View,
				       NULL,
				       latitude,
				       longitude,
				       xrad,
				       yrad,
				       azimuth);
}


/*--------------------------------------
 * Hv_SetEllipseMapPoints used for ellipse placed
 * on map views
 *--------------------------------------*/

static void Hv_SetEllipseMapPoints(Hv_View View,
				   Hv_Map  map,
				   float latitude,
				   float longitude,
				   float xrad,
				   float yrad,
				   float azimuth)
     
{
  double  azimdel;
  int    i,  np, npm1;
  double  xo, yo, radius;
  float   fx, fy;
  double  x,  y;

  double  xc, yc;

  double  rinv;

  double  sa = 0.0;
  double  ca = 1.0;
  double  azim;

  double xr2, yr2;

  Boolean rotate;

  rotate = (fabs(azimuth) > 0.001);

  if (rotate) {
    Hv_LatLongToXY(View, &fx, &fy, latitude, longitude);
    xc = (double)fx;
    yc = (double)fy;
    sa = sin(azimuth);
    ca = cos(azimuth);
  }


  np = map->numpoints;
  npm1 = np - 1;

  azimdel = Hv_TWOPI/npm1;

  xr2 = xrad*xrad;
  yr2 = yrad*yrad;

  for (i = 0; i < np; i++) {

    if ((i == 0) || (i == npm1))
      azim = 0.0;
    else
      azim = i*azimdel;


    x = sin(azim);
    y = cos(azim);
    
    rinv = x*x/xr2 + y*y/yr2;
    radius = sqrt(1.0/rinv);

/*    radius = sqrt(x*x + y*y); */

    Hv_OffsetLatLong(latitude, longitude, (float)radius, (float)azim,
		     &(map->latitude[i]), &(map->longitude[i]));

    if (rotate) {
      Hv_LatLongToXY(View, &fx, &fy, map->latitude[i], map->longitude[i]);

      xo = (double)fx;
      yo = (double)fy;
      xo -= xc;
      yo -= yc;
      x = xo*ca + yo*sa;
      y = yo*ca - xo*sa;
      x += xc;
      y += yc;
      Hv_XYToLatLong(View, (float)x, (float)y, &(map->latitude[i]), &(map->longitude[i]));

    }

    map->mercatory[i] = Hv_MercatorLatToY(map->latitude[i]);

  }




  
}



/*--------------------------------------
 * Hv_SetWedgeMapPoints used for wedges placed
 * on map views
 *--------------------------------------*/

void Hv_SetWedgeMapPoints(Hv_View View,
			  Hv_Map  map,
			  float latitude,
			  float longitude,
			  float radinner, /* xrad if ellipse */
			  float radouter, /* yrad if ellipse */
			  float azim1,    /* azimuth if ellipse */
			  float azim2,
			  int   mode)

{

  int            i, np, npm1, nc, nc2;
  float          azimuth, azimuthdel, adel1, adel2;
  float          radius, raddel; 

  float          f;
  int            npmnb, nspnt, ns, nb, nt, nrem;

#ifdef DEBUG
  Hv_PointerReport("start of set wedge map points");
#endif

  if (mode == Hv_ELLIPSEMODE) {
    Hv_SetEllipseMapPoints(View,
			   map,
			   latitude,
			   longitude,
			   radinner,
			   radouter,
			   azim1);
    return;
  }


  np = map->numpoints;
  npm1 = np - 1;
  nc = Hv_NUMWEDGESTRAIGHT/2 - 1; /* eg 80 -> 39 */
  nc2 = np - nc - 1;  /* eg 80 -> np - 40 */

/*  if (radinner < 1.0) {  */
  if (radinner < 1.0) { 
    azimuthdel = (azim2 - azim1)/(np-(Hv_NUMWEDGESTRAIGHT-1));
    raddel = radouter/nc;

    for (i = 0; i < np; i++) {

      if ((i == 0) || (i == npm1)) {
	map->latitude[i]  = latitude;
	map->longitude[i] = longitude;
      }

      else if (i < nc) {
	radius = i*raddel;
	Hv_OffsetLatLong(latitude, longitude, radius, azim1,
			 &(map->latitude[i]), &(map->longitude[i]));
      }


      else if (i > nc2) {
	radius = radouter - (i-nc2)*raddel;
	Hv_OffsetLatLong(latitude, longitude, radius, azim2,
			 &(map->latitude[i]), &(map->longitude[i]));
      }

      else {
	azimuth = azim1 + (i-nc)*azimuthdel;

	Hv_OffsetLatLong(latitude, longitude, radouter, azimuth,
			 &(map->latitude[i]), &(map->longitude[i]));
      }

      map->mercatory[i] = Hv_MercatorLatToY(map->latitude[i]);
      
    }

  }
  else { /* has inner radius */

    f = (radinner/radouter);

    ns = Hv_iMax(5, (int)(nc*(1.0-f)));
    nrem = np - 2*ns;

    nb = Hv_iMax(2, (int)(f*nrem));
    nt = nrem - nb - 1;

    nspnt = ns + nt;
    npmnb = np - nb;

    adel1 = (azim2 - azim1)/(nt-1);
    adel2 = (azim2 - azim1)/(nb-1);

    raddel = (radouter - radinner)/(ns+1);

    for (i = 0; i < np; i++) {

/* 1st and last point  */

      if ((i == 0) || (i == npm1)) {

	Hv_OffsetLatLong(latitude, longitude, radinner, azim1,
			 &(map->latitude[i]), &(map->longitude[i]));

      }

/* up right leg */

      else if (i <= ns) {
	Hv_OffsetLatLong(latitude, longitude, radinner + i*raddel, azim1,
			 &(map->latitude[i]), &(map->longitude[i]));
      }

/* arount top */

      else if (i <= nspnt) {

	Hv_OffsetLatLong(latitude, longitude, radouter, azim1 + (i-ns-1)*adel1,
			 &(map->latitude[i]), &(map->longitude[i]));
      }

/* down right leg */


      else if (i < npmnb) {
	Hv_OffsetLatLong(latitude, longitude, radouter - (i-nspnt-1)*raddel, azim2,
			 &(map->latitude[i]), &(map->longitude[i]));
      }

/* around bottom*/

      else {
	Hv_OffsetLatLong(latitude, longitude, radinner, azim2 - (i-npmnb)*adel2,
			 &(map->latitude[i]), &(map->longitude[i]));
      }

      map->mercatory[i] = Hv_MercatorLatToY(map->latitude[i]);
      
    }


  }

#ifdef DEBUG
  Hv_PointerReport("end of set wedge map points");
#endif
}


/*--------------------------------------
 * Hv_DrawWedgeMap used for wedges placed
 * on map views
 *--------------------------------------*/


void Hv_DrawWedgeMap(Hv_View    View,
		     Hv_Item    Item,
		     Hv_Region  region,
		     Hv_Map     map,
		     Boolean    dounion,
		     short      color,
		     short      fillcolor,
		     short      pattern) {

/* if color < 0, uses xor mode */


  Hv_Point        *xp;
  int             npoly;
  Hv_Region       preg = NULL;
  Hv_Region       hrr = NULL;
  Boolean         changepat;
  Hv_Region       cliprgn = NULL;

  if (map == NULL)
    return;

#ifdef DEBUG
  Hv_PointerReport("\tstart of draw wedge map ");
#endif

/* first the fill part */

  changepat = ((!Hv_extraFastPlease) && (fillcolor >= 0) && (pattern >= 0));

  if (changepat) {
    Hv_SetPattern(pattern, fillcolor);
  }
  
  if (yesdraw) {
      hrr = Hv_RectRegion(View->hotrect);
      cliprgn = Hv_IntersectRegion(region, hrr);
      Hv_DestroyRegion(hrr);
      Hv_SetClippingRegion(cliprgn);
  }

  while ((xp = Hv_MapPointsToPoly(View, map, &npoly, False)) != NULL) {
    if (npoly > 2) {

      if ((!Hv_extraFastPlease) && dounion && (Item != NULL)) {
	Hv_UnionPolygonWithRegion(xp, npoly, &(Item->region));
      }

      if (yesdraw) {

	if (!Hv_extraFastPlease && (color >= 0)) {
	  preg = Hv_CreateRegionFromPolygon(xp, (short)npoly);
	  Hv_FillRegion(preg, fillcolor, cliprgn);
	  Hv_DestroyRegion(preg);
	}
	
	Hv_SetPattern(-1, Hv_black);

	if (color < 0)
	  Hv_DrawXorLines(xp, (short)(npoly-1));  
	else {
		if (Hv_extraFastPlease) {
	        Hv_SetLineWidth(2); 
	        Hv_DrawLines(xp, (short)npoly, Hv_yellow); 
	        Hv_SetLineWidth(0); 
		}
		else 
	      Hv_DrawLines(xp, (short)npoly, color); 
	}

	if (changepat) 
	  Hv_SetPattern(pattern, fillcolor);
	
      }
    }  /* end npoly */
  
    Hv_Free(xp);
  }

  Hv_SetPattern(-1, Hv_black);
  
  Hv_DestroyRegion(cliprgn);

#ifdef DEBUG
  Hv_PointerReport("\tend of draw wedge map");
#endif

}

/*
 *  AddTextEditTriplet covenience routine
 */

static Hv_Widget AddTextEditTriplet(Hv_Widget     parent,
				    char          *label,
				    char          *unitstr,
				    Hv_FunctionPtr vccb) {

  Hv_Widget   w, w2;
  
  w2 = Hv_StandardLabel(parent, label, 0);

  if (vccb == NULL) {
    w  = Hv_VaCreateDialogItem(parent,
			       Hv_TYPE,           Hv_TEXTDIALOGITEM,
			       Hv_DEFAULTTEXT,    " ",
			       Hv_FONT,           Hv_fixed2,
			       Hv_EDITABLE,       True,
			       Hv_NUMCOLUMNS,     8,
			       NULL);
  }
  else {
    w  = Hv_VaCreateDialogItem(parent,
			       Hv_TYPE,           Hv_TEXTDIALOGITEM,
			       Hv_DEFAULTTEXT,    " ",
			       Hv_FONT,           Hv_fixed2,
			       Hv_EDITABLE,       True,
			       Hv_NUMCOLUMNS,     8,
			       Hv_VALUECHANGED,   vccb,
			       NULL);
  }

  w2 = Hv_StandardLabel(parent, unitstr, -1001);
  return w;
}


/*-----------------------------------------------------------
 *  Hv_EditWorldWedgeItem
 *-----------------------------------------------------------*/

void Hv_EditWorldWedgeItem(Hv_Event   hvevent) {
  Hv_Item              Item = hvevent->item;
  Hv_View              View = hvevent->view;

  Hv_Widget               dialog = NULL;
  Hv_Widget               thickness;
  Hv_Widget               stylebutton;
  Hv_Widget               dummy, rowcol, rc;
  Hv_WorldWedgeData   *ww;
  int                  answer;


/* these are used in "menu history" */

  static int         num_sbuttons = -1;
  static Hv_WidgetList  sbuttons;
  int                units = 0;

  char  *unames[]    = {"Unknown", "Kilometers", "Nautical Miles", "Miles", "Unknown"}; 
  float  convfact[]  = {(float)(1.0),
	  (float)(1.0),
	  (float)(1.854),
	  (float)(1.609),
	  (float)(1.0)};

  Boolean  mapview;

  if (Item == NULL)
    return;

  ww = (Hv_WorldWedgeData *)(Item->data);

/* map view will have a more impressive editor */

  mapview = (View->mapdata != NULL);

  Hv_VaCreateDialog(&dialog, Hv_TITLE, " Wedge Editor ", NULL);
  rowcol = Hv_DialogColumn(dialog, 6);

  ignorecb = True;

  rc = Hv_DialogTable(rowcol, 1, 5);
  linecolorlabel = Hv_SimpleColorLabel(rc, "   line ", Hv_EditWorldWedgeColor);
  fillcolorlabel = Hv_SimpleColorLabel(rc, "   fill ", Hv_EditWorldWedgeColor);

  patternlabel   = Hv_SimplePatternLabel(rc, "pattern",
					 Hv_EditWorldWedgePattern);


  dummy = Hv_SimpleDialogSeparator(rowcol);
  
  rc = Hv_TightDialogRow(rowcol, 4);
  thickness = Hv_SimpleTextEdit(rc, " Line Width ", NULL, 6);
  dummy = Hv_SimpleDialogSeparator(rowcol);
  
  
  stylebutton  = Hv_CreateLineStylePopup(rowcol, Hv_fixed2);    /* line style menu */

/* more widgets for map views */

  if (mapview) {
    dummy = Hv_SimpleDialogSeparator(rowcol);

    units = Hv_iMax(0, Hv_iMin(4, Hv_units));

    if (ww->mode == Hv_WEDGEMODE) {
      rc = Hv_DialogTable(rowcol, 5, 5);
      innerradw = AddTextEditTriplet(rc, "Inner Radius", unames[units], NULL);
      outerradw = AddTextEditTriplet(rc, "Outer Radius", unames[units], NULL);
      azim1w   = AddTextEditTriplet(rc, "Start Azimuth", "degrees", TextCB);
      azim2w   = AddTextEditTriplet(rc, "End Azimuth", "degrees", TextCB);
      azimuthw = AddTextEditTriplet(rc, "Central Azimuth", "degrees", TextCB);
    }
    else {
      rc = Hv_DialogTable(rowcol, 3, 5);
      xradw = AddTextEditTriplet(rc, "X Radius", unames[units], NULL);
      yradw = AddTextEditTriplet(rc, "Y Radius", unames[units], NULL);
      azimuthw = AddTextEditTriplet(rc, "Azimuth", "degrees", NULL);
    }

  }

  
  dummy = Hv_StandardActionButtons(rowcol, 20, Hv_ALLACTIONS);
  
/* for use in history */

  Hv_GetOptionMenuButtons(stylebutton,  &sbuttons, &num_sbuttons);

/* the dialog has been created */


/* the the current symbol in the option menu */


  wwdir = ww->direction;
  Hv_hotStyle      = ww->style;

  Hv_SetOptionMenu(stylebutton,  sbuttons, (int)Hv_hotStyle);

  Hv_SetDeleteItem(dialog, Item);

  newpat = Item->pattern;
  newlinecolor = Item->color;
  newfillcolor = ww->fillcolor;

  Hv_SetIntText(thickness, ww->thickness);

/* if map view, set extra fields */

  if (mapview) {
    if (ww->mode == Hv_WEDGEMODE) {
      Hv_SetFloatText(innerradw,   (ww->innerrad)/convfact[units], 3);
      Hv_SetFloatText(outerradw,   (ww->radius)/convfact[units], 3);
      Hv_SetFloatText(azim1w,   Hv_RADTODEG*(ww->azim1), 3);
      Hv_SetFloatText(azim2w,   Hv_RADTODEG*(ww->azim2), 3);
      Hv_SetFloatText(azimuthw, Hv_RADTODEG*(Item->azimuth), 3);
    }
    else {
      Hv_SetFloatText(xradw,   (ww->xrad)/convfact[units], 3);
      Hv_SetFloatText(yradw,   (ww->yrad)/convfact[units], 3);
      Hv_SetFloatText(azimuthw, Hv_RADTODEG*(Item->azimuth), 3);
    }

  }


  Hv_ChangeLabelPattern(patternlabel, Item->pattern);
  Hv_ChangeLabelColors(linecolorlabel, -1, Item->color);
  Hv_ChangeLabelColors(fillcolorlabel, -1, ww->fillcolor);

  ignorecb = False;

  while ((answer = Hv_DoDialog(dialog, NULL)) > 0) {

    if (mapview) {
      if (ww->mode == Hv_WEDGEMODE) {
	ww->azim1 = Hv_DEGTORAD*Hv_GetFloatText(azim1w);
	ww->azim2 = Hv_DEGTORAD*Hv_GetFloatText(azim2w);
	Item->azimuth = Hv_AzimuthFromWW(ww);
	ww->radius = (float)fabs(Hv_GetFloatText(outerradw)*convfact[units]);
	ww->innerrad = (float)fabs(Hv_GetFloatText(innerradw)*convfact[units]);
	ww->innerrad = Hv_fMin(ww->innerrad, (float)(0.95*(ww->radius)));
      }
      else {
	Item->azimuth = Hv_DEGTORAD*Hv_GetFloatText(azimuthw);
	ww->xrad = (float)fabs(Hv_GetFloatText(xradw)*convfact[units]);
	ww->yrad = (float)fabs(Hv_GetFloatText(yradw)*convfact[units]);
      }
    }


    Item->pattern = newpat;
    ww->style = Hv_hotStyle;

    Item->color  = newlinecolor;
    ww->fillcolor = newfillcolor;

    ww->thickness = Hv_sMax(1, Hv_sMin(6, (short)Hv_GetIntText(thickness)));

/* now handle the redraw -- including the fact the the size of the item probably changed */

    Hv_StandardRedoItem(Item); 

    if (answer != Hv_APPLY)
      break;
  }

  Hv_DestroyWidget(dialog);
}


static void TextCB(Hv_Widget w) {

  float              azim1, azim2; 
  Hv_WorldWedgeData  ww;
  float              oldazim, newazim, daz;
  Hv_TextPosition    pos;

  pos = Hv_GetInsertionPosition(w);

  if (ignorecb)
    return;

    ww.azim1 = Hv_DEGTORAD*Hv_GetFloatText(azim1w);
    ww.azim2 = Hv_DEGTORAD*Hv_GetFloatText(azim2w);
    ww.direction = wwdir;

  if ((w == azim1w) || (w == azim2w)) {
    ww.azim1 = Hv_DEGTORAD*Hv_GetFloatText(azim1w);
    ww.azim2 = Hv_DEGTORAD*Hv_GetFloatText(azim2w);
    ww.direction = wwdir;

    ignorecb = True;
    Hv_SetFloatText(azim1w, Hv_RADTODEG*(ww.azim1), 3);
    Hv_SetFloatText(azim2w, Hv_RADTODEG*(ww.azim2), 3);
    Hv_SetFloatText(azimuthw, Hv_RADTODEG*Hv_AzimuthFromWW(&ww), 3);
    ignorecb = False;
  }

  else if (w == azimuthw) {
    oldazim = Hv_AzimuthFromWW(&ww);
    newazim = Hv_DEGTORAD*Hv_GetFloatText(azimuthw);
    azim1 = Hv_DEGTORAD*Hv_GetFloatText(azim1w);
    azim2 = Hv_DEGTORAD*Hv_GetFloatText(azim2w);
    daz = newazim - oldazim;

    azim1 += daz;
    azim2 += daz;

    if (azim1 > Hv_TWOPI)
      azim1 -= (float)Hv_TWOPI;
    else if (azim1 < 0.0)
      azim1 += (float)Hv_TWOPI;

    if (azim2 > Hv_TWOPI)
      azim2 -= (float)Hv_TWOPI;
    else if (azim2 < 0.0)
      azim2 += (float)Hv_TWOPI;

    ignorecb = True;
    Hv_SetFloatText(azim1w, Hv_RADTODEG*azim1, 3);
    Hv_SetFloatText(azim2w, Hv_RADTODEG*azim2, 3);
    Hv_SetFloatText(azimuthw, Hv_RADTODEG*newazim, 3);
    ignorecb = False;

  }

  Hv_SetInsertionPosition(w, pos);

}


/*-----------------------------------------------------------
 *  Hv_DrawWorldWedgeItem
 *-----------------------------------------------------------*/

void  Hv_DrawWorldWedgeItem(Hv_Item    Item,
			    Hv_Region   region) {

    Hv_WorldWedgeData   *ww;
    Hv_Point            *poly;
    short               np;
    Hv_Point            rrp;
    Boolean             mapview;
    short               x, y;
    float               lat2, long2;
    float               latitude, longitude;
    Hv_Region           cliprgn = NULL;
    Hv_ViewMapData  mdata;

    
    mapview = (Item->view->mapdata != NULL);
    ww = (Hv_WorldWedgeData *)(Item->data);
    
    if (mapview) {
      
      mdata = Hv_GetViewMapData(Item->view);
      
      if (ww->map == NULL) {
        ww->map = (void *)Hv_MallocMap(Hv_NUMWEDGEPOINTS+1);
        Hv_GetWedgeMap(Item, ww);
      }
      
      if (mdata->projection != Hv_MERCATOR) {
        cliprgn = Hv_IntersectRegion(region, mdata->GlobeItem->region);
        Hv_SetClippingRegion(cliprgn);
      }
      
      
    }
    else {
      Hv_GetWedgePoly(Item, &poly, &np, &rrp);
      
      
      if (ww->fillcolor >= 0) {
        if (Item->pattern >= 0) 
          Hv_SetPattern(Item->pattern, ww->fillcolor);
        
        if (Item->pattern != Hv_HOLLOWPAT)
          Hv_FillPolygon(poly, np, False, ww->fillcolor, Item->color);
        
        if (Item->pattern >= 0)
          Hv_SetPattern(-1, Hv_black);
      }
    }
    
    Hv_SetLineStyle(ww->thickness, ww->style);
    
    if (Item->region != NULL)
      Hv_DestroyRegion(Item->region);
    
    
    if (mapview) {
      Item->region = Hv_CreateRegion();
      Hv_DrawWedgeMap(Item->view,
        Item,
        (cliprgn != NULL) ? cliprgn : region,
        (Hv_Map)(ww->map),
        True,
        Item->color,
        ww->fillcolor,
        Item->pattern);
    }
    else {
      Item->region = Hv_CreateRegionFromPolygon(poly, np);
      Hv_FramePolygon(poly, np, Item->color);
    }
    
    Hv_SetLineStyle(0, Hv_SOLIDLINE);
    
    
    if (mapview) { /* basepoint stores lat,long in h,v */
      latitude  = ww->basepoint.h;
      longitude = ww->basepoint.v;
      
      if (ww->mode == Hv_WEDGEMODE) {
        
        Hv_OffsetLatLong(latitude, longitude,
          ww->radius, Hv_AzimuthFromWW(ww),
          &lat2, &long2);
      }
      else {
        Hv_OffsetLatLong(latitude, longitude,
          ww->yrad, Item->azimuth,
          &lat2, &long2);
      }
      
      
      Hv_LatLongToLocal(Item->view, &(rrp.x), &(rrp.y), lat2, long2);
    }
    
    
    if (Hv_CheckItemDrawControlBit(Item, Hv_ROTATABLE)) {
      Hv_SetRect(Item->rotaterect, (short)(rrp.x-3), (short)(rrp.y-3), 7, 7);
      Hv_FillRect(Item->rotaterect, Hv_orange);
      Hv_FrameRect(Item->rotaterect, Hv_blue);
    }
    
    if (mapview) { /* basepoint stores lat in h and longitude in v */
      Hv_LatLongToLocal(Item->view, &x, &y, ww->basepoint.h, ww->basepoint.v);
      Hv_SetPoint(Item->rotaterp, x, y);
    }
    
    
    if (Hv_CheckItemDrawControlBit(Item, Hv_ROTATABLE) && (Item->rotaterect != NULL))
      Hv_UnionRectWithRegion(Item->rotaterect, Item->region);
    
    Hv_ClipBox(Item->region, Item->area);
    
    Hv_LocalRectToWorldRect(Item->view, Item->area, Item->worldarea);
    
    Hv_SetPattern(-1, Hv_black);
    Hv_SetLineStyle(0, Hv_SOLIDLINE);
    
    if (!mapview)
      Hv_Free(poly);
    
    Hv_DestroyRegion(cliprgn);
    Hv_RestoreClipRegion(region);

}

/**
 * Hv_DrawWorldWedgeButton
 * @purpose Draw the world wedge drawing tool.
 * @param   Item     The world wedge drawing tool button.
 * @param   region   The clipping region.
 */

void Hv_DrawWorldWedgeButton(Hv_Item     Item,
			     Hv_Region   region) {
  Hv_Rect          area;
  int              i, xc;
  int              ang = 56; /* in deg */
  int              del;
  int              step;

  del = 2*(90-ang);

/* convert to 64ths */

  ang *= 64;
  del *= 64;

  Hv_ButtonDrawingArea(Item, &area);

  xc = (area.left + area.right)/2;

  Hv_FillArc((short)(area.left+3),
	     (short)(area.top+2),
	     (short)(area.width-6),
	     (short)(area.height+14),
	     ang, del,
	     True, Hv_cornFlowerBlue, Hv_black);

  step = del/8;

  for (i = 0; i < 8; i++) {
    
    Hv_FrameArc((short)(area.left+3),
		(short)(area.top+2),
		(short)(area.width-6),
		(short)(area.height+14),
		ang + del - (i+1)*step, step,
		(short)(Hv_white + 2*i));
  }

}


/*-----------------------------------------------------------
 * Hv_WorldWedgeButtonCallback
 *-----------------------------------------------------------*/


void Hv_WorldWedgeButtonCallback(Hv_Event  hvevent) {
  Hv_View              View = hvevent->view;
  static Hv_View       activeView = NULL;
  static Hv_Item       activeItem = NULL;
  Hv_Item              newItem;
  static Hv_Cursor        tempcursor;
  Hv_Point             StartPP;
  Hv_Rect              limitrect;

  Hv_Item              parent = NULL;
  Hv_WorldWedgeData   *ww;

  if (activeView == NULL) {
    Hv_StandardFirstEntry(hvevent, Hv_WorldWedgeButtonCallback,
			  &activeView, &activeItem, &tempcursor);
    return;
  }
  
  if (View != activeView) {
    Hv_Warning("World Wedge selector active on another view!");
    return;
  }


/* see if click occured inside hotrect */

  Hv_SetPoint(&StartPP, hvevent->where.x, hvevent->where.y); 
  if (Hv_PointInRect(StartPP, View->hotrect)) {


/* for certain items, we assume the desire is to contain the
   new item */

    parent = Hv_PointInPlotItem(View, StartPP);
    
    if (parent == NULL)
      Hv_CopyRect(&limitrect, View->hotrect);
    else
      Hv_CopyRect(&limitrect, parent->area);

    ww = (Hv_WorldWedgeData *)Hv_Malloc(sizeof(Hv_WorldWedgeData));

    ww->innerrad = 0.0;
    ww->xrad = 0.0;
    ww->yrad = 0.0;
    ww->map = NULL;
    ww->mode = Hv_WEDGEMODE;

    Hv_SelectWedge(View, limitrect, StartPP,
		   &(ww->basepoint),
		   &(ww->azim1),
		   &(ww->azim2),
		   &(ww->radius),
		   &(ww->direction));


    newItem = Hv_VaCreateItem(View,
			      Hv_PARENT,       parent,
			      Hv_TAG,          Hv_DRAWINGTOOLTAG,
			      Hv_TYPE,         Hv_WORLDWEDGEITEM,
			      Hv_DATA,         ww,
			      Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			      Hv_DRAWCONTROL,  Hv_ROTATABLE + Hv_ENLARGEABLE + Hv_ZOOMABLE + Hv_FANCYDRAG,
			      Hv_AFTERDRAG,    Hv_WedgeAfterDrag,
			      Hv_AFTERROTATE,  Hv_WedgeAfterRotate,
			      Hv_AFTEROFFSET,  Hv_WedgeAfterOffset,
			      Hv_FIXREGION,    Hv_FixWedgeRegion,
			      NULL);

/* now set the properties based on the view's defaults */
      
    if (Hv_usedNewDrawTools) {
	ww = (Hv_WorldWedgeData *)(newItem->data);
        ww->fillcolor = View->hotfillcolor;
	newItem->pattern = View->hotpattern;
	newItem->color = View->hotbordercolor;
	ww->thickness = View->hotlinewidth;
	ww->style = View->hotlinestyle;
	Hv_usedNewDrawTools = False;
    }


/*===
 * see if the user wants to be notified
 *===*/

    if (Hv_CreateDrawingItemCB != NULL)
	Hv_CreateDrawingItemCB(newItem);
    
    Hv_DrawItem(newItem, NULL); 

  } /* end in hotrect */
  
  Hv_ResetActiveView(hvevent, &activeView, &activeItem, tempcursor);

}



/*-----------------------------------------------------------
 *  Hv_WedgeAfterRotate
 *-----------------------------------------------------------*/

void Hv_WedgeAfterRotate(Hv_Item Item) {

    Hv_WorldWedgeData  *ww;
    float              oldaz, daz;

#ifdef DEBUG
  Hv_PointerReport("start of after wedge rotate");
#endif

    ww = (Hv_WorldWedgeData *)(Item->data);

    if (ww->mode == Hv_WEDGEMODE) {

      oldaz = Hv_AzimuthFromWW(ww);
      daz = Item->azimuth - oldaz;
      
      ww->azim1 += daz;
      ww->azim2 += daz;
      
      if (ww->azim1 > Hv_TWOPI)
	ww->azim1 -= (float)Hv_TWOPI;
      else if (ww->azim1 < 0.0)
	ww->azim1 += (float)Hv_TWOPI;
      
      if (ww->azim2 > Hv_TWOPI)
	ww->azim2 -= (float)Hv_TWOPI;
      else if (ww->azim2 < 0.0)
	ww->azim2 += (float)Hv_TWOPI;

    }

    Hv_FixWedgeRegion(Item);
#ifdef DEBUG
  Hv_PointerReport("end of after wedge rotate");
#endif
    
}


/*-----------------------------------------------------------
 *  Hv_WorldWedgeInitialize
 *-----------------------------------------------------------*/


void Hv_WorldWedgeInitialize(Hv_Item           Item,
			     Hv_AttributeArray attributes) {

  Hv_WorldWedgeData  *ww;

  float  latb, longb;

#ifdef DEBUG
  Hv_PointerReport("start of wedge initialize");
#endif

  Item->rotaterp = (Hv_Point *)Hv_Malloc(sizeof(Hv_Point));

  Item->standarddraw = Hv_DrawWorldWedgeItem;
  Item->worldarea = Hv_NewFRect();

  ww = (Hv_WorldWedgeData *)(attributes[Hv_DATA].v);
  Item->data = (void *)ww;

  if (ww->mode == Hv_WEDGEMODE)
    Item->azimuth = Hv_AzimuthFromWW(ww);

/* basepoint stores lat,long in h,v */

  latb  = ww->basepoint.h;
  longb = ww->basepoint.v;

  ww->thickness = 1;
  ww->style = Hv_SOLIDLINE;

  ww->fillcolor = attributes[Hv_FILLCOLOR].s;
  ww->user1 = attributes[Hv_USER1].s;
  ww->userdata = attributes[Hv_USERDATA].v;

  if (attributes[Hv_FIXREGION].v == NULL)
    Item->fixregion = Hv_FixWedgeRegion;

  if (Item->doubleclick == NULL)
    Item->doubleclick = Hv_EditWorldWedgeItem;  /* default doubleclick is the world rect item editor */
  
  if (Item->fixregion != NULL)
    Item->fixregion(Item);

#ifdef DEBUG
  Hv_PointerReport("end of wedge initialize");
#endif

}



/* -------- Hv_WedgeAfterDrag -----*/

void Hv_WedgeAfterDrag(Hv_Item  Item,
			 short   dh,
			 short   dv)
{
}


/* -------- Hv_WedgeAfterOffset -----*/

void Hv_WedgeAfterOffset(Hv_Item  Item,
			 short   dh,
			 short   dv)

{
  Hv_WorldWedgeData  *ww;
  short              x, y;

  if (Item == NULL)
    return;

  if ((dh == 0) && (dv == 0))
    return;


#ifdef DEBUG
  Hv_PointerReport("start of wedge after offset");
#endif

  ww = (Hv_WorldWedgeData *)(Item->data);

  if (Hv_offsetDueToDrag) { /* basepoint stores lat,long in h,v */
      Hv_LatLongToLocal(Item->view, &x, &y, ww->basepoint.h, ww->basepoint.v);
      Hv_LocalToLatLong(Item->view, (short)(x+dh), (short)(y+dv), &(ww->basepoint.h), &(ww->basepoint.v));
  }

  Hv_FixWedgeRegion(Item);

#ifdef DEBUG
  Hv_PointerReport("end of wedge after offset");
#endif
}



/*------- Hv_FixWedgeRegion -------*/

void  Hv_FixWedgeRegion(Hv_Item Item)

{
  Hv_View             View = Item->view;
  Hv_Point           *poly;
  short               np;
  Hv_Point            rrp;
  Boolean             mapview = (Item->view->data != NULL);
  Hv_WorldWedgeData * ww;
  short                x, y;

#ifdef DEBUG
  Hv_PointerReport(">>start of fix wedge region");
#endif
  
  if (Item->region != NULL)
      Hv_DestroyRegion(Item->region);
  Item->region = NULL;

  ww =(Hv_WorldWedgeData *)(Item->data);

  if (mapview) {

    Item->region = Hv_CreateRegion();

    if (ww->map == NULL)
      ww->map = (void *)Hv_MallocMap(Hv_NUMWEDGEPOINTS+1);


    Hv_GetWedgeMap(Item, ww);

    yesdraw = False;

    Hv_DrawWedgeMap(Item->view,
		    Item,
		    NULL,
		    (Hv_Map)(ww->map),
		    True,
		    0,
		    -1,
		    -1);

    yesdraw = True;

/* basepoint stores lat,long in h,v */

    Hv_LatLongToLocal(Item->view, &x, &y, ww->basepoint.h, ww->basepoint.v);
    Hv_SetPoint(Item->rotaterp, x, y);
    Hv_SetPoint(&rrp, x, y);

  }
  else {
    Hv_GetWedgePoly(Item, &poly, &np, &rrp);
    Item->region = Hv_CreateRegionFromPolygon(poly, np);
  }

  if (Hv_CheckItemDrawControlBit(Item, Hv_ROTATABLE)) {
      Hv_SetRect(Item->rotaterect, (short)(rrp.x-3), (short)(rrp.y-3), 7, 7);
      Hv_UnionRectWithRegion(Item->rotaterect, Item->region);
  }

  Hv_ClipBox(Item->region, Item->area);
  Hv_LocalRectToWorldRect(View, Item->area, Item->worldarea);

#ifdef DEBUG
  Hv_PointerReport(">>end of fix wedge region");
#endif

}



/*-----------------------------------------------------------
 *  Hv_GetWedgePoly uses the wedge parameters
 *  to make a polygon arch
 *-----------------------------------------------------------*/

void Hv_GetWedgePoly(Hv_Item    Item,
		     Hv_Point   **poly,
		     short      *np,
		     Hv_Point   *rrp) {


    Hv_WorldWedgeData  *ww;
    Hv_FPoint          *fp;
    float              endang;
    Hv_View            View = Item->view;

    float dx, dy;
    float rrx, rry;
    float dang, ang;
    float latitude, longitude;
    float lat2, long2;
    int   i;
    short npm1, npm2;

    ww = (Hv_WorldWedgeData *)(Item->data);

    *np   = Hv_NUMWEDGEPOINTS+1;
    fp    = Hv_NewFPoints(*np);
    *poly = Hv_NewPoints(*np);

    Hv_SetFPoint(&(fp[0]), ww->basepoint.h, ww->basepoint.v);

    npm1 = *np-1;
    npm2 = *np-2;

    endang = ww->azim2; 
    
/* for map views we want the base lat long */

    if (View->mapdata != NULL) {
      Hv_XYToLatLong(View, fp[0].h, fp[0].v, &latitude, &longitude);
    }


/* usual direction worries */


    if (ww->direction == Hv_COUNTERCLOCKWISE) {
      if (ww->azim2 > ww->azim1) {
	endang -= (float)Hv_TWOPI;
      }
    }
    else {
      if (ww->azim2 < ww->azim1) {
	endang += (float)Hv_TWOPI;
      }
    }

    dang = (endang - ww->azim1)/npm2;

    for (i = 1; i < *np; i++) {
      ang = ww->azim1 + (i-1)*dang;

/* what we do here depends on whether we
   are in a map view */

      if (View->mapdata != NULL) {
	Hv_OffsetLatLong(latitude, longitude,
			 ww->radius, ang,
			 &lat2, &long2);
	Hv_LatLongToXY(View, &(fp[i].h), &(fp[i].v), lat2, long2);
      }
      else {
	dx = (float)((ww->radius)*cos(ang));
	dy = (float)((ww->radius)*sin(ang));
	fp[i].h = fp[0].h + dx;
	fp[i].v = fp[0].v + dy;
      }

    }
 
/* get the rotate rect point */

    ang = Hv_AzimuthFromWW(ww);

    if (View->mapdata != NULL) {
      Hv_OffsetLatLong(latitude, longitude,
		       ww->radius, ang,
		       &lat2, &long2);
      Hv_LatLongToLocal(Item->view, &(rrp->x), &(rrp->y), lat2, long2);
    }
    else {
      rrx = fp[0].h + (float)((ww->radius)*cos(ang));
      rry = fp[0].v + (float)((ww->radius)*sin(ang));
      Hv_WorldToLocal(Item->view, rrx, rry, &(rrp->x),  &(rrp->y));
    }

    Hv_WorldPolygonToLocalPolygon(Item->view, *np, *poly, fp);
    Hv_SetPoint(Item->rotaterp, (*poly)[0].x, (*poly)[0].y);
    Hv_Free(fp);

}


/*------- Hv_DestroyWorldWedge ---------*/

void Hv_DestroyWorldWedge(Hv_Item Item)

{
  Hv_WorldWedgeData        *ww;
  Hv_Map                   map;

#ifdef DEBUG
  Hv_PointerReport("Start of Destroy");
#endif

  ww = (Hv_WorldWedgeData *)(Item->data);

  if (ww->map != NULL) {
    map = (Hv_Map)(ww->map);
    Hv_DestroyMap(&map);
  }


  Hv_Free(ww);
  Item->data = NULL;
#ifdef DEBUG
  Hv_PointerReport("end of destroy");
#endif

}




/* ------ Hv_EditWorldWedgeColor --- */

static void Hv_EditWorldWedgeColor (Hv_Widget w)

{
  if (w == linecolorlabel)
    Hv_ModifyColor(w, &newlinecolor, "Edit Line Color", False);
  else if (w == fillcolorlabel)
    Hv_ModifyColor(w, &newfillcolor, "Edit Fill Color", True);
}

/* ------ Hv_EditWorldWedgePattern --- */

static void Hv_EditWorldWedgePattern(Hv_Widget w)

{
  if (w == patternlabel)
    Hv_ModifyPattern(w, &newpat, "Fill Pattern");

}

/*------- Hv_AzimuthFromWW --------*/

static float Hv_AzimuthFromWW(Hv_WorldWedgeData * ww) {

  float endang;
  float azim;

  endang = ww->azim2;

  if (ww->direction == Hv_COUNTERCLOCKWISE) {
    if (ww->azim2 > ww->azim1) {
      endang -= (float)Hv_TWOPI;
    }
  }
  else {
    if (ww->azim2 < ww->azim1) {
      endang += (float)Hv_TWOPI;
    }
  }

  

  azim = (float)((ww->azim1 + endang)/2.0);

  if (azim > Hv_TWOPI)
    azim -= (float)Hv_TWOPI;
  else if (azim < 0.0)
    azim += (float)Hv_TWOPI;

  return azim;
}

/*--------- Hv_GetWedgeMap ----------*/

static 	void Hv_GetWedgeMap(Hv_Item   Item,
			    Hv_WorldWedgeData *ww) {
     
  float  latitude, longitude;
  float  endang;


  Hv_Map map = (Hv_Map)(ww->map);

#ifdef DEBUG
  Hv_PointerReport("start of get wedge map");
#endif

/* basepoint stores lat,long in h,v */

  latitude  = ww->basepoint.h;
  longitude = ww->basepoint.v;

  if (ww->mode == Hv_WEDGEMODE) {

    endang = ww->azim2;
  
    if (ww->direction == Hv_COUNTERCLOCKWISE) {
      if (ww->azim2 > ww->azim1) {
	endang -= (float)Hv_TWOPI;
      }
    }
    else {
      if (ww->azim2 < ww->azim1) {
	endang += (float)Hv_TWOPI;
      }
    }


    Hv_SetWedgeMapPoints(Item->view,
			 map,
			 latitude,
			 longitude,
			 ww->innerrad,
			 ww->radius,
			 ww->azim1,
			 endang,
			 Hv_WEDGEMODE);
  }
  else { /* ellipse */
    Hv_SetEllipseMapPoints(Item->view,
			   map,
			   latitude,
			   longitude,
			   ww->xrad,
			   ww->yrad,
			   Item->azimuth);

  }

#ifdef DEBUG
  Hv_PointerReport("end of get wedge map");
#endif

}








