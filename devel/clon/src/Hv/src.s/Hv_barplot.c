/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"
#include "Hv_xyplot.h"

/*-------- Hv_DrawBarCurves ---------*/

void  Hv_DrawBarCurves(Hv_Item plot)


/****************************************
  draws the "curves" for bar charts
******************************************/

{
  Hv_PlotData    plotdata;
  Hv_CurveData   tcurve;
  int            plottype;
  Hv_Rect        rect;
  int            npts;
  int            i;
  float          x1, y1, x2, y2;
  short          l, t, r, b;

  plotdata = Hv_GetPlotData(plot);

/* get the number of points from the first
  curve. (All curves have the same number
  of points) */

  tcurve = plotdata->curves;
  if (tcurve == NULL)
    return;
  npts = tcurve->npts;

  if (npts < 1)
    return;

  plottype = Hv_GetPlotType(plot);
  
  switch (plottype) {
  case Hv_HBAR:
    break;
    
  case Hv_VBAR:
    break;
    
  case Hv_HSTACK:
    for (i = 0; i < npts; i++) {
      y1 = (float)(i + 0.6);
      y2 = (float)(i + 1.4);
      x2 = 0.0;
      for (tcurve = plotdata->curves; tcurve != NULL; tcurve = tcurve->next)
	if (tcurve->display) {
	  x1 = x2;
	  x2 = x1 + tcurve->y[i];
	  
	  Hv_PlotToLocal(plot, &l, &t, x1, y2);
	  Hv_PlotToLocal(plot, &r, &b, x2, y1);
	  Hv_SetRect(&rect, l, t, (short)(r-l), (short)(b-t));
	  Hv_FillPatternRect(&rect, tcurve->linecolor, tcurve->pattern);
	  Hv_FrameRect(&rect, Hv_black);
	}
    }
    
    break;

  case Hv_VSTACK:
    for (i = 0; i < npts; i++) {
      x1 = (float)(i + 0.6);
      x2 = (float)(i + 1.4);
      y2 = 0.0;
      for (tcurve = plotdata->curves; tcurve != NULL; tcurve = tcurve->next) 
	if (tcurve->display) {
	  y1 = y2;
	  y2 = y1 + tcurve->y[i];
	  
	  Hv_PlotToLocal(plot, &l, &t, x1, y2);
	  Hv_PlotToLocal(plot, &r, &b, x2, y1);
	  Hv_SetRect(&rect, l, t, (short)(r-l), (short)(b-t));
	  Hv_FillPatternRect(&rect, tcurve->linecolor, tcurve->pattern);
	  Hv_FrameRect(&rect, Hv_black);
	}
    }
    break;
    

  }

}

/*------ Hv_ReasonableBarPlotLimits --------*/

void  Hv_ReasonableBarPlotLimits(Hv_PlotData plotdata)

/***********************************************
  Computes reasonable plot limits for bar plots 
***********************************************/

{
  Hv_CurveData      tcurve;
  int               npts;
  float             *ysum;
  int               i;
  float             xmax, ymax;

  if ((plotdata == NULL) || (plotdata->curves == NULL))
    return;

/* for "stacked" plots, one axis is determined by the
   maximum sum all active curve y values for all points,
   while the other axis is simply the number of
   data points, since each data point will produce
   a new bar */

  if ((plotdata->plottype == Hv_HSTACK) || (plotdata->plottype == Hv_VSTACK)) {
    npts = plotdata->curves->npts;

/* malloc and init  array to hold the sums */

    ysum = (float *)Hv_Malloc(npts * sizeof(float));
    for (i = 0; i < npts; i++)
      ysum[i] = 0.0;

/* now collect the sums */

    for (i = 0; i < npts; i++)
      for (tcurve = plotdata->curves; tcurve != NULL; tcurve = tcurve->next) {
	if (i < tcurve->npts)
	  ysum[i] += tcurve->y[i];
      }

/* now get the max */

    ymax = (float)(-1.0e30);
    for (i = 0; i < npts; i++)
      ymax = Hv_fMax(ymax, ysum[i]);
    ymax = Hv_ConvertToANiceValue(ymax);

/* the other limit */

    xmax = (float)(npts+1);

/* where they get assigned depends of if we are v or h */

    plotdata->xmin = 0.0;
    plotdata->ymin = 0.0;

    if (plotdata->plottype == Hv_HSTACK) {
      plotdata->xmax = ymax;
      plotdata->ymax = xmax;
    }
    else {  /* vstack */
      plotdata->xmax = xmax;
      plotdata->ymax = ymax;
    }

    Hv_Free(ysum);
  } /* end stacked */

}

/*-------- Hv_DrawBarPlotTicks -------*/

void  Hv_DrawBarPlotTicks(Hv_Item plot,
			  Boolean drawx,
			  Boolean drawy)

{
  Boolean       horiz;
  Hv_PlotData   plotdata;
  float         fval, f2val;
  float         del, smalldel;
  int           n, np1, i, m, j;
  float         xmin, xmax, ymin, ymax;
  Hv_Rect       *area;
  float         tscilow, tscihi;
  Hv_CurveData  tcurve;
  char          *tstr;

  plotdata = Hv_GetPlotData(plot);

  Hv_SetLineStyle(0, Hv_SOLIDLINE);
  horiz = Hv_PlotIsHorizontal(plot);

/* switch the local sci notation lims with
   the globals -- restore later */

  tscihi  = Hv_sciNotHiLimit;
  tscilow = Hv_sciNotLowLimit;
  Hv_sciNotHiLimit  = plotdata->scihilim;
  Hv_sciNotLowLimit = plotdata->scilowlim;

  xmin = plotdata->xmin;
  xmax = plotdata->xmax;
  ymin = plotdata->ymin;
  ymax = plotdata->ymax;
  area = plot->area;

/* for HOROZONTAL bar charts, only draw xticks.
   draw CATEGORIES on y axis. Vice versa for 
   VERTICAL bar charts */

  if (horiz) {

    if (drawx)
      if ((n = plotdata->majxtick) > 0) {
	np1 = n+1;
	del = (xmax-xmin)/(np1);
	for (i = 0; i <= np1; i++) {
	  if (i == np1)
	    fval = xmax;
	  else
	    fval = xmin + i*del;

/* sneak in the minor ticks */

	  if (plotdata->drawminxticks) {
	    if ((m = plotdata->minxtick) > 0) {
	      smalldel = del/(m+1);
	      for (j = 1; j <= m; j++) {
		f2val = fval + j*smalldel;
		Hv_DrawXTick(plot, f2val, Hv_MINTICKLEN, area, NULL);
	      }
	    }
	  }
	  if ((i > 0) || (plotdata->drawminxval))
	    Hv_DrawXTick(plot, fval, Hv_MAJTICKLEN, area, NULL);
	}  /* end for i */
      } /* end n > 0 */

/* now the y "categories" */

    if (plotdata->drawyaxisvalues) 
      if ((tcurve = plotdata->curves) != NULL) 
	for (i = 0; i < tcurve->npts; i++) {
		if(tcurve->categories != NULL) {
	      if ((tcurve->categories[i]) != NULL) {

      /* check for "NULL" string */

	        Hv_InitCharStr(&tstr, tcurve->categories[i]);
	        Hv_UpperCase(tstr);
	        if(strcmp(tstr, "NULL") != 0)
	          Hv_DrawYTick(plot, (float)(i+1), Hv_MAJTICKLEN, area, tcurve->categories[i]);
	        Hv_Free(tstr);
		  }
	  }
	}

  }  /* horiz */

  else { /*vert */
    if (drawy) 
      if ((n = plotdata->majytick) > 0) {
	np1 = n+1;
	del = (ymax-ymin)/(np1);
	for (i = 0; i <= np1; i++) {
	  if (i == np1)
	    fval = ymax;
	  else
	    fval = ymin + i*del;
	  
/* sneak in the minor ticks */

	  if (plotdata->drawminyticks) {
	    if ((m = plotdata->minytick) > 0) {
	      smalldel = del/(m+1);
	      for (j = 1; j <= m; j++) {
		f2val = fval + j*smalldel;
		Hv_DrawYTick(plot, f2val, Hv_MINTICKLEN, area, NULL);
	      }
	    }
	  }
	  
	  if ((i > 0) || (plotdata->drawminyval))
	    Hv_DrawYTick(plot, fval, Hv_MAJTICKLEN, area, NULL);
	}
      } /* end n > 0 */ 

/* now the x categories */

    if (plotdata->drawxaxisvalues)
      if ((tcurve = plotdata->curves) != NULL) 
	for (i = 0; i < tcurve->npts; i++) {
      if ((tcurve->categories) != NULL) {
	    if ((tcurve->categories[i]) != NULL) {

/* check for "NULL" string */

	      Hv_InitCharStr(&tstr, tcurve->categories[i]);
	      Hv_UpperCase(tstr);
	      if(strcmp(tstr, "NULL") != 0)
	        Hv_DrawXTick(plot, (float)(i+1), Hv_MAJTICKLEN, area, tcurve->categories[i]);
	      Hv_Free(tstr);
		}
	  }
	}
    


  } /* vert */

/* restore the global limits */

  Hv_sciNotHiLimit  = tscihi;
  Hv_sciNotLowLimit = tscilow;
  
}






