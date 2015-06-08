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

/*---------- Hv_Bin1DHistogram -----------*/

void  Hv_Bin1DHistogram(Hv_PlotData    plotdata,
			Hv_CurveData   curve)


{
  int        nb, i, bin;
  float      binsize;

  if (!(curve->dirtybin))
    return;

  nb = curve->numbin1;
  binsize = (plotdata->histoxmax - plotdata->histoxmin)/nb;

  Hv_Free(curve->binned1);
  curve->binned1 = (int *)Hv_Malloc(nb*sizeof(int));

  curve->underflow1 = 0;
  curve->overflow1 = 0;

  for (i = 0; i < nb; i++)
    curve->binned1[i] = 0;

/* now the actual binning */

  for (i = 0; i < curve->npts; i++) {
    bin = (int)((curve->n1[i] - plotdata->histoxmin)/binsize);
    if (bin < 0)
      curve->underflow1++;
    else if (bin >= nb)
      curve->overflow1++;
    else
      curve->binned1[bin] += 1;
  }

  curve->dirtybin = False;

}

/*---------- Hv_Bin2DHistogram -----------*/

void  Hv_Bin2DHistogram(Hv_PlotData    plotdata,
			Hv_CurveData   curve)


{
}

/*-------- Hv_ReasonableHistoPlotLimits --------*/

void  Hv_ReasonableHistoPlotLimits(Hv_PlotData plotdata)

{
  Hv_CurveData      tcurve;
  float             xmin, xmax, ymax;
  int               i;

  if ((plotdata == NULL) || (plotdata->curves == NULL))
    return;

  plotdata->ymin = 0.0;

  xmin = (float)1.0e30;
  xmax = (float)(-1.0e30);
  ymax = (float)(-1.0e30);

  if (plotdata->plottype == Hv_1DHISTO) {
    for (tcurve = plotdata->curves; tcurve != NULL; tcurve = tcurve->next) {

      for (i = 0; i < tcurve->npts; i++) {
	xmin = Hv_fMin(xmin, tcurve->n1[i]);
	xmax = Hv_fMax(xmax, tcurve->n1[i]);
      }

      plotdata->xmin = xmin;
      plotdata->xmax = xmax;

      plotdata->histoxmin = xmin;
      plotdata->histoxmax = xmax;

/* need to have xmin and max set prior to binning */

      Hv_Bin1DHistogram(plotdata, tcurve);

      for (i = 0; i < tcurve->numbin1; i++)
	ymax = Hv_fMax(ymax, (float)(tcurve->binned1[i]));
    }

    plotdata->ymax = ymax+1;
    
  }  /* end 1D Histo */
  
}


/*---------- Hv_Draw1DHistograms -----------*/

void  Hv_Draw1DHistograms(Hv_Item  plot)

{
  Hv_CurveData      tcurve;
  Hv_Point          *pp, *ppstair;
  int               n, nstair;
  int               i, j;
  short             delR;
  Hv_PlotData       plotdata;
  float             binsize;

  plotdata = Hv_GetPlotData(plot);

  for (tcurve = plotdata->curves; tcurve != NULL; tcurve = tcurve->next) {
    binsize = (plotdata->histoxmax - plotdata->histoxmin)/tcurve->numbin1;

    Hv_SetLineStyle(tcurve->thickness, tcurve->style);

    Hv_Bin1DHistogram(plotdata, tcurve); /* does nothing if not "dirty" */
    n = tcurve->numbin1;
    nstair = 2*n + 2;

    pp = Hv_NewPoints(n);
    ppstair = Hv_NewPoints(nstair);

    for (i = 0; i < n; i++)
      Hv_PlotToLocal(plot, &(pp[i].x), &(pp[i].y),
		     plotdata->histoxmin + (i*binsize),
		     (float)(tcurve->binned1[i]));


    for (i = 0; i < n; i++) {
      j = 2*i + 1;

      if (i == (n-1))
	delR = (pp[i].x - pp[i-1].x);
      else
	delR = (pp[i+1].x - pp[i].x);

      Hv_SetPoint(ppstair+j,   pp[i].x,      pp[i].y);
      Hv_SetPoint(ppstair+j+1, (short)(pp[i].x+delR), pp[i].y);
    }

    Hv_SetPoint(ppstair,              pp[0].x,   plot->area->bottom);
    Hv_SetPoint(ppstair + (nstair-1), ppstair[nstair-2].x, plot->area->bottom);

    Hv_DrawLines(ppstair, (short)nstair, tcurve->linecolor);
    Hv_Free(ppstair);

    Hv_Free(pp);
    Hv_Free(ppstair);
    Hv_SetLineStyle(0, Hv_SOLIDLINE);
  }
}


/*---------- Hv_Draw2DHistograms -----------*/

void  Hv_Draw2DHistograms(Hv_Item  plot)

{
}




