/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#ifndef __HVXYPLOTH__
#define __HVXYPLOTH__

#define  LEGENDLINELEN           54
#define  LEGENDVGAP              5


/* units */

#define PIXELUNITS  1
#define INCHUNITS   2
#define CMUNITS     3
#define MMUNITS     4
#define PLOTUNITS   5

/* main plot types */

#define Hv_STANDARDXY       0
#define Hv_HBAR             1
#define Hv_VBAR             2
#define Hv_HSTACK           3
#define Hv_VSTACK           4
#define Hv_1DHISTO          5
#define Hv_2DHISTO          6    /* note 2D histos are like 3D bar plots */

#define Hv_THEISSEN        1001  /* speciality plot: Theissen triangulation */

/* connection algorithms */

#define Hv_NOLINES          0
#define Hv_SIMPLECONNECT    1
#define Hv_STAIRCONNECT     2
#define Hv_SPLINE           3
#define Hv_CURVEFIT         4

/* fit types */

#define Hv_LINEFIT          0
#define Hv_EXPONENTIALFIT   1
#define Hv_POLYFIT          2
#define Hv_GAUSSIANFIT      3
#define Hv_LEGENDREFIT      4

/* feedback tags */

#define Hv_XYPLOTDATA      -1001

/* item tag */

#define Hv_XTICKTAG        -1098
#define Hv_YTICKTAG        -1097
#define Hv_XYTITLETAG      -1096
#define Hv_XLABELTAG       -1095
#define Hv_YLABELTAG       -1094

/* axes styles */

#define Hv_TWOAXES         0      /*standard x y axes */
#define Hv_FOURAXESSIMPLE  1      /*essentially two axes plus frame -- right and top have no tickmarks */
#define Hv_FOURAXES        2      /* all four axes have tick marks */

#define Hv_MAJTICKLEN      7 
#define Hv_MINTICKLEN      3 

typedef struct hvplotlabelrec *Hv_PlotLabel;

typedef struct hvplotlabelrec

{
  Hv_PlotLabel    next;
  char           *text;
  float           x, y;
  short           font, color;
  Boolean         horizontal;
} Hv_PlotLabelRec;


/* curve [raw data] structure. This corresponds to a single
   curve -- that is a set of xy points. There may be
   many raw data sets (curves) on a single plot, and
   many plots on a single view */

typedef struct hvcurvedatarec *Hv_CurveData;

typedef struct hvcurvedatarec
{
  Hv_CurveData  next;
  short         version;        /*100 * version of this data structure */
  char          *name;          /* data set name (appears on legend) */
  int           npts;           /* number of points */

  float         *x;             /* x data */
  float         *y;             /* y data */
  float         *z;             /* z data */

  float         *xerr;          /* for horiz error bars */
  float         *yerr;          /* for vert error bars */
  float         *zerr;          /* z errors */

  float         xmin, xmax;     /* actual data limits for x */
  float         ymin, ymax;     /* actual DATA limits for y*/
  float         zmin, zmax;     /* actual DATA limits for z*/

  short         algorithm;      /* algorithm for connecting the points */
  short         symbol;         /* symbol used on points */
  short         symbolsize;     /* size of plot symbols */
  short         linecolor;      /* line color */
  short         symbolcolor;    /* symbol fill color */
  short         thickness;      /* line thickness */
  short         style;          /* solid or dashed */
  Boolean       display;        /* display/hide this curve */

  Boolean       usexerrs;       /* draw xerrors if present */
  Boolean       useyerrs;       /* draw yerrors if present */
  Boolean       usezerrs;       /* draw zerrors if present */

  Boolean       onlegend;       /* used to exclude from legend */
  float         xscale, yscale, zscale;     /* overall scale of the data values */
  int           fittype;        /* e.g. polynomial, exponentials, etc. */
  int           polyorder;      /* order of ploynomial fit */
  int           legendreorder;  /* order of Legendre fit */
  int           numgauss;       /* #gaussians used in fit */
  Boolean       showfit;        /* show the fit results? */

/* data associated with a fit */

  Boolean       dirtyfit;
  float         slope, sigb;
  float         intercept, siga;
  float         chisq;
  int           ma;
  float         *a;                         /* fit parameters */

/* new fields added for cplot functionality. If yspike is
   set, a vertical line will be drawn from the x axis
   to the y value */

  Boolean       yspike;

/* new field -- array of string pointers for each curve 
   used primarily for category labels in bar plots */

  char          **categories;
  short         pattern;  /* for bar charts */

/* additions to deal with secondary axes */

  Boolean       use2ndy;  /*use secondary y axis */
  Boolean       use2ndx;  /*use secondary y axis */
  Boolean       use2ndz;  /*use secondary z axis */

/* additions for histograms */

  float        *n1;         /* raw data for 1D histo */
  float        *n2;         /* second raw data array for 2D histo */
  int          *binned1;    /* binned data for 1st data array */
  int          *binned2;    /* binned data for 2nd data array */
  int           numbin1;    /* bin size for 1D histo */
  int           numbin2;    /* bin size for 2D histo */
  int           overflow1;  /* number of overflows 1st count array */
  int           overflow2;  /* number of overflows 2nd count array */
  int           underflow1; /* number of underflows 1st count array */
  int           underflow2; /* number of underflows 2nd count array */
  Boolean       dirtybin;   /* if True, must rebin */

  Boolean       absval;     /* absolut value of data */

} Hv_CurveDataRec;


/* plot data -- attached to the data field of a plot Item */

typedef struct hvplotdatarec *Hv_PlotData;

typedef struct hvplotdatarec
{
  short             version;                 /*100 * version of this data structure */
  Hv_CurveData      curves;                  /* the curves on this plot */
  Boolean           xlog;                    /* x axis log/linear */
  Boolean           ylog;                    /* y axis log/linear */
  Boolean           uselegend;               /* show/hide legend */
  Boolean           framelegend;             /* draw a box around the legend? */
  Boolean           opaquelegend;            /* see through the legend ? */
  Boolean           xzeroline;               /* horizontal line at y = 0 */
  Boolean           yzeroline;               /* vertical line at x = 0 */
  Boolean           fancysymbols;            /* use fancy version of symbol points */

  Boolean           drawmajxticks;           /* draw the major x tick marks */
  Boolean           drawmajyticks;           /* draw the minor x tick marks */
  Boolean           drawminxticks;           /* draw the minor y tick marks */
  Boolean           drawminyticks;           /* draw the minor y tick marks */

  Boolean           drawmajxgrid;            /* draw lines at major (primary) ticks */
  Boolean           drawmajygrid;            /* draw lines at major (primary) ticks */
  Boolean           drawminxgrid;            /* draw lines at minor (primary) ticks */
  Boolean           drawminygrid;            /* draw lines at minor (primary) ticks */

  Boolean           drawminxval;             /* draw the minimum x value */
  Boolean           drawminyval;             /* draw the minimum y value */
  Boolean           drawxaxisvalues;         /* draw x axis values */
  Boolean           drawyaxisvalues;         /* draw y axis values */

  Boolean           ebartops;                /* draw horiz lines on ends of error bars*/

  Boolean           usebackground;           /* subtract uniform background from fits? */
  Boolean           showbackground;          /* show a constant background */
  float             background;              /* constant background subtraction */

  Boolean           showfilename;            /* show the name of the file used to read data*/
  short             majxtick, majytick;      /* number of MAJOR marks INCLUSIVE eg: |----|----|----| = 4*/
  short             minxtick, minytick;      /* number of MINOR marks between najor marks */
  short             axesstyle;               /*i.e two or four axes */
  short             axesfont;                /* font used to display axes */
  short             axescolor;               /* color of axis lines */

  short             majgridcolor;            /* color used for major grid */
  short             mingridcolor;            /* color used for minor grid */

  short             xprecision;              /*numerical precision on labels*/
  short             yprecision;              /*numerical precision on labels*/

  short             legendfont;              /* font used to display legend */
  Hv_Item           legenditem;              /* the legend */
  Hv_Item           filenameitem;            /* the data file name text item */

  Hv_Item           xtickitem;               /* item holding values text */
  Hv_Item           ytickitem;               /* item holding values text */
  float             xmin, xmax, ymin, ymax;  /* axes extrema */
  float             scilowlim, scihilim;     /* limits to where Sci not is used */

/*  "xtietag" ties the x axis for all plots sharing
  the same (nonzero) xtietag so that a zoom on one will
  affect the x axes of all. "ynozoom", if set, restricts
  the rubber banding so that only the x direction is zoomed,
  similarly for xnozoom*/

  int               xtietag;                /* to tie x (primary) axis with another plots's x axis */
  Boolean           xnozoom;                /* cannot zoom x */
  Boolean           ynozoom;                /* cannot zoom y */
  int               plottype;               /* standard, stack bar, etc */

/* the attach booleans keep the objectd attached in thier usual places */

  Boolean           attachlegend;           /* tie legend to RT of plot frame */
  Boolean           attachtitle;            /* fix title at center top */
  Boolean           attachxlab;             /* attach xlabel */
  Boolean           attachylab;             /* attach y label */

  Hv_FRect         *defworld;               /* for getting back to original plot */
  short             bgcolor;                /* plot background color */
  Hv_FunctionPtr    UserPlotDraw;           /* optional user routine */

  short            activecurve;             /*NON c index of active curve for on-view edit*/

/* for histos */

  float            histoxmin, histoxmax;

/* so that fit results are nmore flexible & draggable */

    short          fitresultfont;
    Hv_Item        fitresultitem;

} Hv_PlotDataRec;


/* PlotViewData is attached to each plot view */

typedef struct hvplotviewdatarec *Hv_PlotViewData;

typedef struct hvplotviewdatarec
{
  Hv_Item          lastplot;   /* last plot read in */

/*
  theses plot* variables are really just used in the
  io as placeholders when "continuation" plots
  are encountered. They are NOT used in anyway
  in the drawing routines
*/
  
  float            plotxmin, plotxmax, plotymin, plotymax;
  Boolean          plotxlog;
  Boolean          plotylog;
  float            plotlegendleft;
  float            plotlegendtop;

  Hv_Item          savebutton;  /* view's save button */
  void            *data;        /* arbitrary data */

  Hv_Item          activeplot;  /* for on-screen editing */
  Hv_Item          activename;  /* name of active plot */
  Hv_Item          bin1slider;  /* slider to change binsize */
  Hv_Item          bin2slider;  /* slider to change binsize */
  Hv_Item          viewbox;     /* parent for all onscreen edit items */
  Hv_Item          curvemenu;   /* onscreen curve menu */
} Hv_PlotViewDataRec;


extern      Hv_Item        Hv_hotPlot;

#ifdef __cplusplus
extern "C"
{
#endif

/*--------- Hv_barplot.c ----------*/

void  Hv_ReasonableBarPlotLimits(Hv_PlotData);

void  Hv_DrawBarPlotTicks(Hv_Item,
			  Boolean,
			  Boolean);

void  Hv_DrawBarCurves(Hv_Item);

/*--------- Hv_histo.c ------------*/

void  Hv_ReasonableHistoPlotLimits(Hv_PlotData plotdata);

void  Hv_Bin1DHistogram(Hv_PlotData    plotdata,
			Hv_CurveData   curve);

void  Hv_Bin2DHistogram(Hv_PlotData    plotdata,
			Hv_CurveData   curve);


void  Hv_Draw1DHistograms(Hv_Item  plot);

void  Hv_Draw2DHistograms(Hv_Item  plot);

/*-------- Hv_xyplot.c --------*/

extern void            Hv_PlotItemCB(Hv_Event hvevent);

extern void            Hv_PlotItemInitialize(Hv_Item      Item,
					     Hv_AttributeArray attributes);

extern Hv_View         Hv_NewXYPlotView(char  *title,
					Boolean autoopen);

extern Hv_View         Hv_BaseXYPlotView(char *title,
					 Hv_Item *lefttool,
					 Hv_Item *righttool,
					 short    hrrmargin);

extern void            Hv_UpdatePlotView(Hv_View View);

extern void            Hv_UpdateXYPlot(Hv_Item   plot);

extern void            Hv_AddDataSetToXYPlot(Hv_Item   plot);

extern Hv_Item         Hv_MallocPlotItem(Hv_View View);

extern Hv_Item         Hv_PointInPlotItem(Hv_View  View,
					  Hv_Point  pp);

extern void            Hv_ReasonablePlotLimits(Hv_PlotData  plotdata);

extern void            Hv_ReasonableYPlotLimits(Hv_PlotData  plotdata);

extern void            Hv_ZoomPlot(Hv_View   View,
				   Hv_Point  pp,
				   Hv_XEvent   *event);

extern void            Hv_TieXAxis(Hv_Item  splot);

extern void            Hv_ZoomPlotToRect(Hv_Item   plot,
					 Hv_Rect   *rect);

extern void            Hv_DestroyPlotItem(Hv_Item  Item);

extern Hv_PlotData        Hv_GetPlotData(Hv_Item  plot);

extern Hv_PlotViewData    Hv_GetPlotViewData(Hv_View  View);

extern void           *Hv_GetPlotViewUserData(Hv_View View);

extern void            Hv_SetPlotViewUserData(Hv_View View,
					      void *data);

extern void            Hv_InitPlotViewData(Hv_View  View);


extern Hv_Item         Hv_StandardPlotLabel(Hv_Item  plot,
					    short   x,
					    short   y,
					    char   *text,
					    short   font);

extern void            Hv_LocalToPlot(Hv_Item  plot,
				      short   x,
				      short   y,
				      float   *px,
				      float   *py);

extern void            Hv_PlotToLocal(Hv_Item  plot,
				      short   *x,
				      short   *y,
				      float   px,
				      float   py);

extern void            Hv_PlotDataMinMax(float  *vals,
					 int     n,
					 float   scale,
					 Boolean absval,
					 float  *dmin,
					 float  *dmax);

extern void            Hv_BigThreeItems(Hv_Item  plot,
					Hv_Item  *title,
					Hv_Item  *xlab,
					Hv_Item  *ylab);


/*-------- Hv_xyplotedit.c --------*/

extern void             Hv_EditPlot(Hv_Event   hvevent);

extern int              Hv_GetCurvePos(void);

extern void             Hv_PlotTypeSensitivities(void);

extern void             Hv_PlotConnectSensitivity(void);

extern void             Hv_PlotFitSensitivity(void);


/*-------- Hv_xyplotio.c --------*/

extern FILE         *Hv_PrepareFile(char *fname);

extern void          Hv_DemoButtonCallback(Hv_Event   hvevent);

extern void          Hv_CheckPlotViewSensitivity(Hv_View  View);

extern void          Hv_WritePlotFile(Hv_View  View,
				       FILE   *fp);


extern void          Hv_LoadPlotData(Hv_Event   hvevent);

extern void          Hv_ReadPlotFile(Hv_View   View,
				     FILE    *fp,
				     char    *fname);


extern int          Hv_CountPlots(Hv_View  View);

extern void         Hv_DeletePlots(Hv_View  View);


/*--------- Hv_plotsupport.c ------*/

extern float       Hv_ConvertToANiceValue(float val);

extern int         Hv_Decade(float x);

extern int         Hv_GetPlotType(Hv_Item   plot);

extern void        Hv_CheckPlotAttachments(Hv_Item plot);

extern int         Hv_SizeLegendItem(Hv_Item  Item);

extern Hv_Region   Hv_TotalPlotRegion(Hv_Item  plot);

extern Boolean     Hv_PlotIsHorizontal(Hv_Item  plot);

extern void        Hv_DrawXTick(Hv_Item   plot,
				float    val,
				short    len,
				Hv_Rect   *area,
				char     *category);

extern void         Hv_DrawYTick(Hv_Item   plot,
				 float    val,
				 short    len,
				 Hv_Rect   *area,
				 char     *category);

extern Hv_PlotData  Hv_MallocPlotData(void);

extern void         Hv_DestroyPlotData(Hv_PlotData   plotdata);

extern Hv_CurveData Hv_MallocCurveData(char  *name);

extern void         Hv_DestroyCurveData(Hv_CurveData   curve);

#ifdef __cplusplus
}
#endif

#endif




