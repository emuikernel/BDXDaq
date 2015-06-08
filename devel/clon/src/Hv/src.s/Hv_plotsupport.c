/**
 * <EM>Various plot support routines.</EM>
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

#include "Hv.h"
#include "Hv_xyplot.h"

static void Hv_AttachLegend(Hv_Item   plot);

static void Hv_CheckLabelAttach(Hv_Item   plot);

static void Hv_DrawAxisValue(Hv_View View,
		       short   x,
		       short   y,
		       Hv_String *hvstr);

/**
 * Hv_ConvertToANiceValue
 * @purpose    hueristic for converting a plot limit,
 *  such as 187.2, into a nicer value, say 190
 * @param   The value, probably an value maximum.
 * @return  A "nice" value, suitable for an axis limit.
 */

float Hv_ConvertToANiceValue(float val) {


  float    nice, mant;
  Boolean  pos;
  int      decade;
  float    pow10;
  int      mant10, dig1, dig2;

  pos = (val >= 0.0);

  nice = (float)(1.05*fabs(val));

  decade = Hv_Decade(nice);
  pow10 = (float)(pow((double)10.0, (double)decade));
  mant = nice/pow10;
  mant10 = (int)(10.0*mant);

  dig1 = mant10 / 10;
  dig2 = mant10 % 10;

  if (dig2 == 9) {
    dig1++;
    dig2 = 0;
  }
  else
    dig2++;

  mant = (float)((float)dig1 + (0.1*(float)(dig2)));
  nice = mant*pow10;

  if (pos)
    return nice;
  else
    return -nice;
}

/**
 * Hv_Decade 
 * @purpose Determine what log "decade" the value passed is in.
 * @param  x    The value.
 * @return  The decade in a log10 sense. 
 */

int   Hv_Decade(float x) {

  if (x > 0.0)
    return (int)floor(log10(x));
  else
    return -32767;
}

/*----------- Hv_GetPlotType -------*/

int   Hv_GetPlotType(Hv_Item plot)

{
  Hv_PlotData       plotdata;

  if (plot == NULL)
    return -1;

  plotdata = Hv_GetPlotData(plot);

  if (plotdata == NULL)
    return -1;
  else
    return plotdata->plottype;
}

/*----------- Hv_AttachLegend ---------*/

static void   Hv_AttachLegend(Hv_Item plot)

{
  Hv_PlotData       plotdata;

  plotdata = Hv_GetPlotData(plot);
  if (plotdata->legenditem == NULL)
    return;

/* if it is not attached, it should be draggable */

  if (!(plotdata->attachlegend)) {
    Hv_FixRectRB(plotdata->legenditem->area);
    Hv_SetItemDrawControlBit(plotdata->legenditem, Hv_DRAGGABLE);
    return;
  }

  plotdata->legenditem->area->left = plot->area->right + 5;
  plotdata->legenditem->area->top = plot->area->top;
  Hv_FixRectRB(plotdata->legenditem->area);
  Hv_ClearItemDrawControlBit(plotdata->legenditem, Hv_DRAGGABLE);
}

/*----------- Hv_CheckLabelAttach---------*/

static void   Hv_CheckLabelAttach(Hv_Item plot)

/* see if any of the three main
   labels are to be attached */

{
  Hv_PlotData       plotdata;
  Hv_Item           title, xlab, ylab;
  short             xc, yc, L, T, R, B;
  short             xtarg, ytarg;
  short             dh, dv;
  short             sw, sh;

  plotdata = Hv_GetPlotData(plot);

  Hv_GetRectLTRB(plot->area, &L, &T, &R, &B);
  Hv_GetRectCenter(plot->area, &xc, &yc);

  Hv_BigThreeItems(plot, &title, &xlab, &ylab);

/* if it is not attached, it should be draggable */

  if (title) {
    if (!(plotdata->attachtitle))
      Hv_SetItemDrawControlBit(title, Hv_DRAGGABLE);
    else {
      xc = (plot->area->left + plot->area->right)/2;

      ytarg = T - title->area->height;
      xtarg = xc - (title->area->width / 2);
      
      dh = xtarg - title->area->left;
      dv = ytarg - title->area->top;
      
      Hv_OffsetItem(title, dh, dv, False);
      Hv_ClearItemDrawControlBit(title, Hv_DRAGGABLE);
    }
  }

  if (xlab) {
    if (!(plotdata->attachxlab))
      Hv_SetItemDrawControlBit(xlab, Hv_DRAGGABLE);
    else {
      Hv_CompoundStringDimension(xlab->str, &sw, &sh);
      ytarg = B + 20;
      xtarg = xc - (sw/2);
      
      Hv_SizeTextItem(xlab, xtarg, ytarg);
      Hv_ClearItemDrawControlBit(xlab, Hv_DRAGGABLE);
    }
  }

  if (ylab) {
    if (!(plotdata->attachylab))
      Hv_SetItemDrawControlBit(ylab, Hv_DRAGGABLE);
    else {
      Hv_CompoundStringDimension(ylab->str, &sw, &sh);

      xtarg = L - 55;
      ytarg = yc - (sw/2);
      
      Hv_SizeTextItem(ylab, xtarg, ytarg);
      Hv_ClearItemDrawControlBit(ylab, Hv_DRAGGABLE);
    }
  }
}

/* ---------- Hv_CheckPlotAttachments -------*/

void   Hv_CheckPlotAttachments(Hv_Item plot)

{
  Hv_AttachLegend(plot);
  Hv_CheckLabelAttach(plot);
}

/*------- Hv_SizeLegendItem -------*/

int Hv_SizeLegendItem(Hv_Item Item)

/********************************************
  Size the legend area so that it fits around
  all entries
**********************************************/
{
  Hv_Item       plot;
  Hv_PlotData   plotdata;
  short         w, h, maxw, legh;
  Boolean       hasanyline = False;
  Hv_CurveData  tcurve;
  int           offset;
  Hv_String     *hvstr;


  if ((Item == NULL)  || (Item->parent == NULL))
    return 0;

  plot = Item->parent;
  plotdata = Hv_GetPlotData(plot);

  if (plotdata == NULL)
    return 0;

  maxw = 0;
  legh = 0;

  for (tcurve = plotdata->curves; tcurve != NULL; tcurve = tcurve->next) 
    if ((tcurve->name != NULL) && (tcurve->display) && (tcurve->onlegend)){
      hasanyline = ((tcurve->algorithm != Hv_NOLINES) || tcurve->yspike);

      hvstr = Hv_CreateString(tcurve->name);
      hvstr->font = plotdata->legendfont;
      Hv_CompoundStringDimension(hvstr, &w, &h);
      Hv_DestroyString(hvstr);

      maxw = Hv_sMax(maxw, w);
      legh = legh + h + LEGENDVGAP;
    }

/* bar charts have a simple constant offset */

  if (Hv_GetPlotType(plot) != Hv_STANDARDXY)
    offset = 50;
  else {
    if (hasanyline)
      offset = 20 + LEGENDLINELEN;
    else /* no line, just symbol */
      offset = 30;
  }
  
  Item->area->width = maxw + offset + 4;
  Item->area->height = legh + LEGENDVGAP;
  Hv_FixRectRB(Item->area);
  
  return offset;
}

/*------ Hv_TotalPlotRegion -------*/

Hv_Region  Hv_TotalPlotRegion(Hv_Item plot)

/*************************************
  Mallocs and returns the total plot
  region including all children
**************************************/

{
  Hv_Region     totalregion = NULL;
  Hv_Item       temp;

  if (plot == NULL)
    return NULL;

  totalregion = Hv_RectRegion(plot->area);
  
  if (plot->children)
    for (temp = plot->children->first; temp != NULL; temp = temp->next) {
      if (temp->region != NULL)
	Hv_AddRegionToRegion(&totalregion, temp->region);
      else 
	Hv_UnionRectWithRegion(temp->area, totalregion);
    }

  Hv_ShrinkRegion(totalregion, -2, -2);
  return totalregion;
}


/*-------- Hv_PlotIsHorizontal --------*/

Boolean   Hv_PlotIsHorizontal(Hv_Item plot)

/**************************************************
  returns true for horiz oriented plots.
  Note: the standard xy plot is considered "vertical"
*****************************************************/

{
  int    plottype;

  plottype = Hv_GetPlotType(plot);
  return ((plottype == Hv_HBAR) || (plottype == Hv_HSTACK));

}


/*-------- Hv_DrawXTick -------*/

void Hv_DrawXTick(Hv_Item   plot,
		  float     val,
		  short     len,
		  Hv_Rect   *area,
		  char      *category)

/* A NON NULL category signals that we want just
   that string displayed as the axis value
   with NO TICKMARK */

{
  short        x, y;
  Hv_PlotData  plotdata; 
  Hv_String    *hvstr;
  char         text[30];
  short        w, h;
  int          decade;
  Boolean      justlab;
  int          precision;

  justlab = (category != NULL);
  plotdata = Hv_GetPlotData(plot);

  Hv_PlotToLocal(plot, &x, &y, val, plotdata->ymin);
  if ((x < area->left) || (x > area->right))
    return;

  if (!justlab) {
    if ((x > area->left) && (x < (area->right-1))) {
	if ((len == Hv_MAJTICKLEN) && (plotdata->drawmajxgrid))
	    Hv_DrawLine(x, area->top, x, area->bottom, plotdata->majgridcolor);
	else if ((len == Hv_MINTICKLEN) && (plotdata->drawminxgrid))
	    Hv_DrawLine(x, area->top, x, area->bottom, plotdata->mingridcolor);
	else { /* normal tick line */
	    Hv_DrawLine(x, area->bottom, x, (short)(area->bottom-len), plotdata->axescolor);
	    if (plotdata->axesstyle == Hv_FOURAXES) 
		Hv_DrawLine(x, area->top, x, (short)(area->top+len), plotdata->axescolor);
	}
    }
    if (len < Hv_MAJTICKLEN)
      return;
  }
    
/* now the value  (unless the user doesn't want vals drawn)*/


  if (plotdata->drawxaxisvalues) {
    if (justlab) {
      hvstr = Hv_CreateString(category);
      hvstr->font = plotdata->axesfont;
      hvstr->strcol = plotdata->axescolor;
      
      Hv_CompoundStringDimension(hvstr, &w, &h);
      
      Hv_DrawAxisValue(plot->view, (short)(x-(w/2)), (short)(area->bottom+4), hvstr);
      Hv_DestroyString(hvstr);
    }

    else if (plotdata->xlog) {
      decade = Hv_Decade((float)(1.01*val));
      Hv_NumberToString(text, 10.0, 0, 0.0);
      
      hvstr = Hv_CreateString(text);
      hvstr->font = plotdata->axesfont;
      hvstr->strcol = plotdata->axescolor;
      Hv_CompoundStringDimension(hvstr, &w, &h);
      
      Hv_DrawAxisValue(plot->view, (short)(x-(w/2)), (short)(area->bottom+4), hvstr);
      Hv_DestroyString(hvstr);
      
      x = x + (w/2) + 1;
      
      Hv_NumberToString(text, (float)(1.0*decade), 0, 0.0);
      
      hvstr = Hv_CreateString(text);
      hvstr->font =  plotdata->axesfont;
      hvstr->strcol = plotdata->axescolor;
      
      Hv_DrawAxisValue(plot->view, x, (short)(area->bottom+1), hvstr);
      Hv_DestroyString(hvstr);
    }
    else {    
      precision = plotdata->xprecision;
      sprintf(text, "%.*f", precision, val);
      
      hvstr = Hv_CreateString(text);
      hvstr->font =  plotdata->axesfont;
      hvstr->strcol = plotdata->axescolor;
      
      Hv_CompoundStringDimension(hvstr, &w, &h);
      Hv_DrawAxisValue(plot->view, (short)(x-(w/2)), (short)(area->bottom+4), hvstr);
      Hv_DestroyString(hvstr);
    }
  }
}

/*--------- Hv_DrawAxisValue ---------*/

static void Hv_DrawAxisValue(Hv_View View,
			     short   x,
			     short   y,
			     Hv_String *hvstr)

{
  Hv_SetBackground(Hv_colors[View->hotrectborderitem->color]);
  Hv_useDrawImage = True;
  Hv_DrawCompoundString(x, y, hvstr);
  Hv_useDrawImage = False;
  Hv_SetBackground(Hv_colors[Hv_canvasColor]);
}

/*-------- Hv_DrawYTick -------*/

void Hv_DrawYTick(Hv_Item   plot,
		  float    val,
		  short    len,
		  Hv_Rect   *area,
		  char     *category)

/* A NON NULL category signals that we want just
   that string displayed as the axis value
   with NO TICKMARK */

{
  short        x, y;
  Hv_PlotData  plotdata; 
  Hv_String    *hvstr, *hvstr2;
  char         text[30];
  short        w, h, w2, h2;
  int          decade;
  float        fact;
  Boolean      justlab;

  justlab = (category != NULL);

  plotdata = Hv_GetPlotData(plot);

  Hv_PlotToLocal(plot, &x, &y, plotdata->xmin, val);
  if ((y < area->top) || (y > area->bottom))
    return;

  if (!justlab) {
      if ((y > area->top) && (y < (area->bottom-1))) {
	  if ((len == Hv_MAJTICKLEN) && (plotdata->drawmajygrid))
	      Hv_DrawLine(area->left, y, (short)(area->right-1), y, plotdata->majgridcolor);
	  else if ((len == Hv_MINTICKLEN) && (plotdata->drawminygrid))
	      Hv_DrawLine(area->left, y, (short)(area->right-1), y, plotdata->mingridcolor);

	  else {
	      Hv_DrawLine(area->left, y, (short)(area->left+len), y, plotdata->axescolor);
	      if (plotdata->axesstyle == Hv_FOURAXES) 
		  Hv_DrawLine(area->right, y, (short)(area->right-len), y, plotdata->axescolor);
	  }
      }

      if (len < Hv_MAJTICKLEN)
	  return;
  }

/* now the value */


  if (plotdata->drawyaxisvalues) {
    if (justlab) {
      hvstr = Hv_CreateString(category);
      hvstr->font =  plotdata->axesfont;
      hvstr->strcol = plotdata->axescolor;
      
      Hv_CompoundStringDimension(hvstr, &w, &h);
      Hv_DrawAxisValue(plot->view, (short)(area->left - w - 3), (short)(y - (h/2) - 1), hvstr);
      Hv_DestroyString(hvstr);
    }

    else if (plotdata->ylog) {
      decade = Hv_Decade((float)(1.01*val));
      
      Hv_NumberToString(text, (float)(1.0*decade), 0, 0.0);
      
/* exponent string */

      hvstr2 = Hv_CreateString(text);
      hvstr2->font =  plotdata->axesfont;
      hvstr2->strcol = plotdata->axescolor;
      Hv_CompoundStringDimension(hvstr2, &w2, &h2);

/* now the "10" string */

      Hv_NumberToString(text, 10.0, 0, 0.0);
      
      hvstr = Hv_CreateString(text);
      hvstr->font =  plotdata->axesfont;
      hvstr->strcol = plotdata->axescolor;
      Hv_CompoundStringDimension(hvstr, &w, &h);

/* first to the left */

      Hv_DrawAxisValue(plot->view, (short)(area->left - w - w2  - 5), (short)(y - (h/2) - 1), hvstr);
      Hv_DrawAxisValue(plot->view, (short)(area->left - w2  - 3),     (short)(y - (h/2) - 4), hvstr2);
      
      Hv_DestroyString(hvstr);
      Hv_DestroyString(hvstr2);
    }
    else {
      fact = (float)(2.0*(pow((double)10.0, (double)(-(plotdata->yprecision+1)))));
      fact /= (1+plotdata->majytick);
      
      Hv_NumberToString(text, val,  plotdata->yprecision,  (float)(fact*fabs(plotdata->ymax - plotdata->ymin)));
      
      hvstr = Hv_CreateString(text);
      hvstr->font =  plotdata->axesfont;
      hvstr->strcol = plotdata->axescolor;
      
      Hv_CompoundStringDimension(hvstr, &w, &h);
      Hv_DrawAxisValue(plot->view, (short)(area->left - w - 3), (short)(y - (h/2) - 1), hvstr);
      Hv_DestroyString(hvstr);
    }
  }
}


/*------- Hv_MallocPlotData --------*/

Hv_PlotData  Hv_MallocPlotData(void)
     
{
  Hv_PlotData   plot;

  plot = (Hv_PlotData)Hv_Malloc(sizeof(Hv_PlotDataRec));
  plot->version = 100;
  plot->UserPlotDraw = NULL;

  plot->xtietag = 0;        /*default: no ties to other plots */
  plot->ynozoom = False;    /*default: y axis zooms */
  plot->xnozoom = False;    /*default: x axis zooms */

  plot->xlog = False;
  plot->ylog = False;

  plot->drawmajxticks = True;
  plot->drawmajyticks = True;
  plot->drawminxticks = True;
  plot->drawminyticks = True;

  plot->ebartops = False;
  plot->fancysymbols = False;
  plot->opaquelegend = True;

  plot->drawxaxisvalues = True;
  plot->drawyaxisvalues = True;

  plot->drawmajxgrid = False;
  plot->drawmajygrid = False;
  plot->drawminxgrid = False;
  plot->drawminygrid = False;

  plot->drawminxval = True;
  plot->drawminyval = True;

  plot->xprecision = 1;
  plot->yprecision = 1;

  plot->background = 0.0;
  plot->usebackground = True;
  plot->showbackground = False;
  plot->showfilename = False;

  plot->scilowlim = (float)1.0e-2;
  plot->scihilim = (float)1.0e5;

  plot->plottype = Hv_STANDARDXY;
  plot->attachlegend = False;
  plot->attachtitle  = False;
  plot->attachxlab   = False;
  plot->attachylab   = False;

  plot->axesstyle = Hv_FOURAXESSIMPLE;
  plot->axescolor = Hv_black;
  plot->majgridcolor = Hv_gray7;
  plot->mingridcolor = Hv_gray12;

  plot->majxtick = 4;
  plot->majytick = 4;
  plot->minxtick = 0;
  plot->minytick = 0;

/* default  axes extrema */

  plot->xmin = (float)2.0e30;
  plot->ymin = (float)2.0e30;
  plot->xmax = (float)(-2.0e30);
  plot->ymax = (float)(-2.0e30);

  plot->histoxmin =  (float)2.0e30;
  plot->histoxmax = (float)(-2.0e30);

  plot->uselegend = False;
  plot->framelegend = True;

  plot->xzeroline = False;
  plot->yzeroline = False;

  plot->legendfont = Hv_helvetica11;
  plot->axesfont = Hv_helvetica11;

  plot->filenameitem = NULL;
  plot->legenditem = NULL;
  plot->ytickitem = NULL;
  plot->xtickitem = NULL;
  plot->curves = NULL;

  plot->activecurve = 1;

  plot->defworld = Hv_NewFRect();

  return plot;
}

/*-------  Hv_DestroyPlotData --------*/

void  Hv_DestroyPlotData(Hv_PlotData plotdata)
     
{
  Hv_CurveData   curve, next;

  if (plotdata == NULL)
    return;

  Hv_Free(plotdata->defworld);

  for(curve = plotdata->curves; curve != NULL; curve = next) {
    next = curve->next;
    Hv_DestroyCurveData(curve);
  }

  plotdata->curves = NULL;
  Hv_Free(plotdata);
  plotdata = NULL;
}


/**
 * Hv_MallocCurveData
 * @param    Allocate memory for a curve.
 * @param    name   Name of the curve.
 * @return   A newly allocated curve.
 */

Hv_CurveData  Hv_MallocCurveData(char *name) {


  Hv_CurveData    curve;
  static int      count = 0;
  static int      nncnt = 0;
  char            text[20];
 
  count = (count % 6);
  
  curve = (Hv_CurveData)Hv_Malloc(sizeof(Hv_CurveDataRec));

  curve->next = NULL;

  curve->version = 100;

  if (name == NULL) {
    nncnt++;
    sprintf(text, "No Name %d", nncnt);
    Hv_InitCharStr(&(curve->name), text);
  }
  else 
    Hv_InitCharStr(&(curve->name), name);

  curve->yspike = False;
  curve->pattern = Hv_FILLSOLIDPAT;
  curve->categories = NULL;
  curve->algorithm = Hv_SIMPLECONNECT;

  curve->symbol = Hv_NOSYMBOL;
  curve->symbolsize = 8;
  curve->linecolor = Hv_black;
  curve->symbolcolor = Hv_red;
  curve->thickness = 1;
  curve->style = Hv_SOLIDLINE;
  curve->display = True;
  curve->onlegend = True;

  curve->usexerrs = True;
  curve->useyerrs = True;
  curve->usezerrs = True;

  curve->fittype = Hv_POLYFIT;
  curve->numgauss = 1;
  curve->showfit = False;
  curve->polyorder = 2;
  curve->legendreorder = 5;

  curve->xscale = 1.0;
  curve->yscale = 1.0;
  curve->zscale = 1.0;

  curve->use2ndx = False;
  curve->use2ndy = False;
  curve->use2ndz = False;

  curve->npts = 0;
  curve->x = NULL;
  curve->y = NULL;
  curve->z = NULL;

/* histogram inits */

  curve->n1 = NULL;
  curve->n2 = NULL;
  curve->binned1 = NULL;
  curve->binned2 = NULL;
  curve->overflow1 = 0;
  curve->overflow2 = 0;
  curve->numbin1 = 100;
  curve->numbin2 = 100;
  curve->underflow1 = 0;
  curve->underflow2 = 0;
  curve->dirtybin = True;

/* error bar data */

  curve->xerr = NULL;
  curve->yerr = NULL;
  curve->zerr = NULL;

  curve->xmin = (float)2.0e30;
  curve->ymin = (float)2.0e30;
  curve->zmin = (float)2.0e30;

  curve->xmax = (float)(-1.0e30);
  curve->ymax = (float)(-1.0e30);
  curve->zmax = (float)(-1.0e30);

  curve->dirtyfit = True;
  curve->a = NULL;
  curve->ma = 0;
  
  count++;
  return curve;
}

/**
 * Hv_DestroyCurveData
 * @purpose   Free memory associated with a curve.
 * @param     curve  Pointer to curve data to be freed.
 */

void  Hv_DestroyCurveData(Hv_CurveData curve) {
  int   i;

  if (curve == NULL)
    return;

  Hv_Free(curve->name);
  Hv_Free(curve->x);
  Hv_Free(curve->y);
  Hv_Free(curve->z);

  Hv_Free(curve->n1);
  Hv_Free(curve->n2);
  Hv_Free(curve->binned1);
  Hv_Free(curve->binned2);

  Hv_Free(curve->xerr);
  Hv_Free(curve->yerr);
  Hv_Free(curve->zerr);

  Hv_Free(curve->a);  /* fit parameters */

/* categories is an array of strings */

  if (curve->categories != NULL) {
    for (i = 0; i < curve->npts; i++)
      Hv_Free(curve->categories[i]);
    Hv_Free(curve->categories);
  }

  Hv_Free(curve);
}
