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
#include "Hv_linklist.h"

extern char    **Hv_demoFile;

/* ----- local prototypes ------*/

static char *AddSpace(char *);

	static void Hv_ReadSpecialPlotRecords(FILE *fp,
				      Hv_View View,
				      char    *fname);

static void StuffCurveStyles(Hv_CurveData curve,
			     int          cindex);

static void    MallocCurveArrays(void);

static void    FreeCurveArrays(void);

static Hv_Item MakeThePlot(Hv_View View,
			   char *fname);

static void  FillPlot(Hv_Item plot);

static void DumpABigThreeLabel(FILE    *fp,
			       Hv_Item plot,
			       Hv_Item label,
			       char    *labstylestr,
			       char    *fontstylestr,
			       char    *colorstylestr,
			       char    *leftstylestr,
			       char    *topstylestr);

static void PlaceLabelFromDataVals(Hv_Item plot,
				   Hv_Item label,
				   float   fx,
				   float   fy);

static void DumpPlot(Hv_Item  plot,
		     FILE   *fp);

static void DumpLabel(Hv_Item  label,
		      FILE    *fp);

static void DumpCurve(Hv_Item       plot,
		      Hv_CurveData  curve,
		      Boolean       first,
		      FILE          *fp);

static void DumpPlotStyles(Hv_Item    plot,
			   FILE      *fp);

static void DumpCurveStyles(Hv_CurveData  curve,
			    FILE      *fp);

static void DumpBoolean(FILE   *fp,
			Boolean  isplot,
			char   *name,
			Boolean  val,
			Boolean  dflt);

static void DumpInt(FILE   *fp,
		    Boolean  isplot,
		    char   *name,
		    int    val,
		    int    dflt);

static void DumpFloat(FILE   *fp,
		      Boolean  isplot,
		      char   *name,
		      float   val,
		      float   dflt);

static void DumpColor(FILE   *fp,
		      Boolean  isplot,
		      char   *name,
		      short   val,
		      short   dflt);

static void SimpleDumpColor(FILE   *fp,
			    short   val);

static void DumpCurveData(Hv_CurveData  curve,
			  FILE      *fp);


static void ParseStyleLine(Hv_Item     lastplot,
			   char       *line);

static void ParseCurveStyleLine(char *line,
				int  cnum);

static int FitTypeFromText(char *line);

static  short  AlgorithmFromText(char *line);

static  short  LineStyleFromText(char *line);

static  short AxesStyleFromText(char *line);

static int PlotTypeFromText(char *line);

static Boolean BooleanFromText(char *line);

static  short  UnitsFromText(char *line);

static void SetPlotDefaults(Hv_View  View);

static void StuffPlot(Hv_Item  plot,
		      int    curveID,
		      int    npts);

static void Stuff1DHisto(Hv_Item  plot,
		      int    curveID,
		      int    npts);

static  void  RestOfString(char  **s);

static int PlotCopyMode(int numplots);

static  void MallocPlotLabel(float  x,
			     float  y,
			     short  font,
			     short  color,
			     char   *str,
			     Boolean horizontal);

static  void  DestroyPlotLabel(Hv_PlotLabel  plab);

static short AdjustValue(float  val,
			 short  units);

static  void BadLineMessage(char  *line);

static int MallocDataSets(int    ndatalines,
			  Boolean  *goodread,
			  char    *line);

#define   MAXSETS   150
#define   XDATA     1
#define   YDATA     2
#define   XERRDATA  3
#define   YERRDATA  4
#define   DUMMYDATA 5
#define   STRDATA   6
#define   N1DATA    7     /* histograms */
#define   N2DATA    8     /* for 2D histograms */

#define   Hv_PLOTCANCEL    0
#define   Hv_PLOTAPPEND    1
#define   Hv_PLOTOVERWRITE 2

/* global data */

static   char  *PlotTypeStr[7];
static   char  *LineStyleStr[4];
static   char  *AlgorithmStr[5];
static   char  *FitTypeStr[5];
static   char  *SymbolStr[10];
static   char  *PatternStr[Hv_NUMPATTERNS];
static   char  *AxesStr[3];

short    PlotRelX;
short    PlotRelY;
short    PlotW;
short    PlotH;

float    PlotLegendLeft;
float    PlotLegendTop;

float    PlotTitleLeft;
float    PlotTitleTop;
float    PlotXlabelLeft;
float    PlotXlabelTop;
float    PlotYlabelLeft;
float    PlotYlabelTop;

Boolean  PlotUseBackground;
Boolean  PlotShowBackground;
Boolean  PlotShowFileName;
Boolean  PlotYNoZoom;
Boolean  PlotXNoZoom;

/* These two are used as a hack to reset
   default precision for histos */

Boolean  readxprecision = False;
Boolean  readyprecision = False;

char    *PlotTitle = NULL;
char    *PlotXlab = NULL;
char    *PlotYlab = NULL;

Hv_PlotLabel PlotLabels = NULL;    /* annotations */

int      PlotNumSets;
int      PlotNumCurves;
int      PlotNumXSets;
int      PlotNumCSets;

Boolean  PlotFancySymbols;
Boolean  PlotXZeroLine;
Boolean  PlotYZeroLine;

short    PlotLegendFont;
short    PlotFitResultFont;

short    PlotTitleFont;
short    PlotXLabelFont;
short    PlotYLabelFont;

short    PlotTitleColor;
short    PlotXLabelColor;
short    PlotYLabelColor;

short    PlotAxesFont;

float    PlotXmin, PlotXmax, PlotYmin, PlotYmax;

Boolean  PlotXLog;
Boolean  PlotYLog;

Boolean  BrandNewPlot;
Boolean  PlotErrorBarTops;
Boolean  PlotDrawXAxisValues;
Boolean  PlotDrawYAxisValues;
Boolean  PlotDrawMinX;
Boolean  PlotDrawMinY;
Boolean  PlotMajXGrid;
Boolean  PlotMajYGrid;
Boolean  PlotMinXGrid;
Boolean  PlotMinYGrid;
Boolean  PlotUseMajXTick;
Boolean  PlotUseMajYTick;
Boolean  PlotUseMinXTick;
Boolean  PlotUseMinYTick;
short    PlotXMajTick;
short    PlotYMajTick;
short    PlotXMinTick;
short    PlotYMinTick;
short    PlotAxesStyle;
int      PlotPlotType;

Boolean  PlotAttachLegend;
Boolean  PlotAttachTitle;
Boolean  PlotAttachXlab;
Boolean  PlotAttachYlab;

short    PlotAxesColor;
short    PlotMinGridColor;
short    PlotMajGridColor;

float    PlotSciLowLim;
float    PlotSciHiLim;

int      PlotXTieTag;

float   *PlotLastX = NULL;
float   *PlotLastY = NULL;
float   *PlotLastV = NULL;
float   *PlotLastH = NULL;

Hv_PlotData     DefaultPlotData = NULL;

int      settype[MAXSETS];
void    *setdata[MAXSETS];

int      *CurvePolyOrder;
int      *CurveLegendreOrder;
int      *CurveNumGauss;
int      *CurveFitType;

short    *CurveLineStyle;
short    *CurveLineColor;
short    *CurveSymbolColor;
short    *CurveSymbolSize;
short    *CurveThickness;
char    **CurveName;
short    *CurveSymbol;
short    *CurvePattern;
short    *CurveAlgorithm;
float    *CurveXScale;
float    *CurveYScale;
Boolean  *CurveDisplay;
Boolean  *CurveOnLegend;
Boolean  *CurveUseXErrs;
Boolean  *CurveUseYErrs;
int      *CurveNumBin1;
int      *CurveNumBin2;
Boolean  *CurveYSpike;
Boolean  *CurveAbsVal;

/*--------- Hv_PrepareFile -----*/

FILE      *Hv_PrepareFile(char *fname)

/*-----------------------
prepare a file for READING
------------------------*/

{
  FILE      *fp = NULL;
  char      *stripfname, *substr;

/* open reopen for reading */

  if ((fp = fopen(fname, "r")) != NULL) {

/* copy the file name to the ps default file */

    Hv_Free(Hv_psDefaultFile);
    Hv_psDefaultFile = (char *)Hv_Malloc(strlen(fname) + 5);
    stripfname = fname;
    Hv_StripLeadingDirectory(&stripfname);
    strcpy(Hv_psDefaultFile, stripfname);
    substr = strstr(Hv_psDefaultFile, ".");
    if (substr != NULL)
      *substr = '\0';
    strcat(Hv_psDefaultFile, ".eps");
  }

  return fp;
}

/*------- Hv_DemoButtonCallback --------*/

void Hv_DemoButtonCallback(Hv_Event hvevent)

{
  Hv_Item         Item = hvevent->item;
  Hv_View         View = hvevent->view;
  char            *fname, *command;
  FILE            *fp;
  int             i;

  if (Hv_demoFile == NULL)
    return;

  Hv_ToggleButton(Item);

/* get a temp file name */

  fname = (char *)Hv_Malloc(L_tmpnam);
  tmpnam(fname);

  if ((fp = fopen(fname, "w")) == NULL) {
    Hv_ToggleButton(Item);
    return;
  }

/* write the temporary file */

  i = 0;
  while (Hv_demoFile[i] != NULL) {
    fprintf(fp, "%s\n", Hv_demoFile[i]);
    i++;
  }
  fclose(fp);

/* read the temporary file */

  if ((fp = Hv_PrepareFile(fname)) == NULL) {
    Hv_ToggleButton(Item);
    return;
  }

  Hv_ReadPlotFile(View, fp, fname);

  command = (char *)Hv_Malloc(12 + strlen(fname));
  strcpy(command, "chmod 777 ");
  strcat(command, fname);
  system(command);
  Hv_Free(command);
  Hv_Free(fname);
  
  Hv_ToggleButton(Item);
}

/*-------  Hv_CheckPlotViewSensitivity -------*/

void    Hv_CheckPlotViewSensitivity(Hv_View View)

{
  Hv_PlotViewData   pvdata;
  int               numplots;
  Hv_Region         region;

  if ((View == NULL) || (View->tag != Hv_XYPLOTVIEW))
    return;

  pvdata = Hv_GetPlotViewData(View);
  numplots = Hv_CountPlots(View);

  if ((pvdata == NULL) || (pvdata->savebutton == NULL))
    return;

  region = Hv_GetViewRegion(View);

  Hv_SetItemSensitivity(pvdata->savebutton,  numplots > 0, False);
  Hv_DrawItem(pvdata->savebutton,  region);

  Hv_DestroyRegion(region);

}

/*-------- Hv_WritePlotFile ------*/

void Hv_WritePlotFile(Hv_View  View,
		      FILE   *fp)

{
  Hv_Item         temp;
  static Boolean  veryfirst = True;

  if (veryfirst) {

    Hv_InitCharStr(&(PlotTypeStr[0]), "standard");
    Hv_InitCharStr(&(PlotTypeStr[1]), "hbar");
    Hv_InitCharStr(&(PlotTypeStr[2]), "vbar");
    Hv_InitCharStr(&(PlotTypeStr[3]), "hstack");
    Hv_InitCharStr(&(PlotTypeStr[4]), "vstack");
    Hv_InitCharStr(&(PlotTypeStr[5]), "histo");
    Hv_InitCharStr(&(PlotTypeStr[6]), "2dhisto");

    Hv_InitCharStr(&(LineStyleStr[0]), "solid");
    Hv_InitCharStr(&(LineStyleStr[1]), "dash");
    Hv_InitCharStr(&(LineStyleStr[2]), "longdash");
    Hv_InitCharStr(&(LineStyleStr[3]), "dotdash");

    Hv_InitCharStr(&(AlgorithmStr[0]), "noline");
    Hv_InitCharStr(&(AlgorithmStr[1]), "simple");
    Hv_InitCharStr(&(AlgorithmStr[2]), "stairs");
    Hv_InitCharStr(&(AlgorithmStr[3]), "spline");
    Hv_InitCharStr(&(AlgorithmStr[4]), "fit");

    Hv_InitCharStr(&(AxesStr[0]), "twoaxes");
    Hv_InitCharStr(&(AxesStr[1]), "simple");
    Hv_InitCharStr(&(AxesStr[2]), "fouraxes");

    Hv_InitCharStr(&(FitTypeStr[0]), "line");
    Hv_InitCharStr(&(FitTypeStr[1]), "exponential");
    Hv_InitCharStr(&(FitTypeStr[2]), "polynomial");
    Hv_InitCharStr(&(FitTypeStr[3]), "gaussian");
    Hv_InitCharStr(&(FitTypeStr[4]), "legendre");

    Hv_InitCharStr(&(SymbolStr[0]), "square");
    Hv_InitCharStr(&(SymbolStr[1]), "circle");
    Hv_InitCharStr(&(SymbolStr[2]), "diamond");
    Hv_InitCharStr(&(SymbolStr[3]), "cross");
    Hv_InitCharStr(&(SymbolStr[4]), "uptriangle");
    Hv_InitCharStr(&(SymbolStr[5]), "downtriangle");
    Hv_InitCharStr(&(SymbolStr[6]), "blockI");
    Hv_InitCharStr(&(SymbolStr[7]), "point");
    Hv_InitCharStr(&(SymbolStr[8]), "bigpoint");
    Hv_InitCharStr(&(SymbolStr[9]), "none");

    Hv_InitCharStr(&(PatternStr[0]), "solid");
    Hv_InitCharStr(&(PatternStr[1]), "80%");
    Hv_InitCharStr(&(PatternStr[2]), "50%");
    Hv_InitCharStr(&(PatternStr[3]), "20%");
    Hv_InitCharStr(&(PatternStr[4]), "hatch");
    Hv_InitCharStr(&(PatternStr[5]), "diagonal1");
    Hv_InitCharStr(&(PatternStr[6]), "diagonal2");
    Hv_InitCharStr(&(PatternStr[7]), "hstripe");
    Hv_InitCharStr(&(PatternStr[8]), "vstripe");
    Hv_InitCharStr(&(PatternStr[9]), "scale");
    Hv_InitCharStr(&(PatternStr[10]), "star");
    Hv_InitCharStr(&(PatternStr[11]), "hollow");

    veryfirst = False;
  }
  
  for (temp = View->items->first; temp != NULL; temp = temp->next)
    if (temp->type == Hv_PLOTITEM)
      DumpPlot(temp, fp);
}

/*------ Hv_LoadPlotData ------*/

void   Hv_LoadPlotData(Hv_Event hvevent)

/*-----------------------------
This is actually the callback for
the "read" button
--------------------------------*/

{
  Hv_Item         Item = hvevent->item;
  Hv_View         View = hvevent->view;
  char            *fname;
  FILE            *fp;

  Hv_ToggleButton(Item);
  fname = Hv_FileSelect("plot data file", "*.plt", NULL);
  if(fname==NULL)
  {
	Hv_ToggleButton(Item);
    return;

  }
  if ((fp = Hv_PrepareFile(fname)) == NULL) {
    Hv_ToggleButton(Item);
    return;
  }

  Hv_ReadPlotFile(View, fp, fname);
  Hv_ToggleButton(Item);
}

/*------ Hv_ReadPlotFile  -----*/

void  Hv_ReadPlotFile(Hv_View   View,
		      FILE    *fp,
		      char    *fname)

/********************************************
  Reads and parses an hvplot plot file and
  creates all the plots it finds
********************************************/

{
  Hv_Item         newItem;
  char            *line = NULL;
  static char     *plotrecname;
  static char     *endrecname;
  int             numlines, numnclines;
  int             ndatalines = 0;
  int             i, j, k, numpts, numstylelines;
  Boolean         starteddata, goodread;
  static          Boolean veryfirst = True;
  char            *temp, *segment;
  float           *dataptr;
  char            **strarry;
  Hv_Item         plot = NULL;
  Hv_Item         title, xlab, ylab;
  Hv_PlotData     plotdata;
  int             mode, numplots;
  Hv_PlotLabel    plab;
  short           sx, sy;
  Hv_PlotViewData pvdata;
  Hv_Region       viewrgn;

/* very first time set up the
   record delimiter strings */

  if (veryfirst) {
    veryfirst = False;
    Hv_InitCharStr(&endrecname,  "[ENDREC]");
    Hv_InitCharStr(&plotrecname, "[PLOTREC]");
  }

/* pvdata is the Plot View Data */

  pvdata = Hv_GetPlotViewData(View);

/* see if there are any existing plots.
   If there area, see if the user wants to
   append or delete. */

  if ((numplots = Hv_CountPlots(View)) > 0) {
    mode = PlotCopyMode(numplots);
    if (mode == Hv_PLOTCANCEL)
      return;
    else if (mode == Hv_PLOTOVERWRITE)
      Hv_DeletePlots(View);
  }

/* malloc a line used throughout */

  line = (char *)Hv_Malloc(Hv_LINESIZE);

/* allocate the curve style arrays */

  MallocCurveArrays();

/* loop over all the plot records */

  while (Hv_FindRecord(fp, plotrecname, False, line)) {
    Hv_RecordLineCount(fp, plotrecname, endrecname, &numlines, &numnclines);


/* if there are not at least 2 non-comment
   lines then ignore this record */

    if (numnclines < 2) {
      Hv_Println("Only %d non-comment lines. Ignoring this record.",
		 numnclines);
      break;
    }

/* set plot defaults, which are later over written by style lines */

    SetPlotDefaults(View);

/* style lines should be at the top */

    starteddata = False;
    numpts = 0;
    numstylelines = 0;
    goodread = True;
    
/* read all the lines of the next plot. This includes style lines at the top
   followed by data lines ONCE THE DATA HAS STARTED TO BE READ, IGNORE
   ADDITIONAL STYLE LINES */

    while(Hv_ReadNextLine(fp, line, plotrecname, endrecname) == 0) {
      if(Hv_NotAComment(line)) {

	if (Hv_Contained(line, "style")) {
	  numstylelines++;
	  if (starteddata)
	    Hv_Println( "Ignored style line: [%s]", line);
	  else
	    ParseStyleLine(pvdata->lastplot, line);
	}

	else {  /* should be a data line */
	  if (!starteddata) {
	    ndatalines = numnclines-numstylelines;
	    if (MallocDataSets(ndatalines, &goodread, line) != 0)
	      goto donereading;
	    starteddata = True;
	  }
	  
/*  finally, we actually read the data! */

	  temp = line;
	  i = 0;


/* use quote delimiters so that categories can contain spaces */

	  Hv_quoteDelimitsSegment = True;
	  while (((segment = Hv_NextTextSegment(&temp)) != NULL) && (i < PlotNumSets)) {
	    if (settype[i] == STRDATA) {
	      strarry = (char **)setdata[i];
	      Hv_InitCharStr(&(strarry[numpts]), segment);
	    }
	    else {
	      dataptr = (float *)setdata[i];
	      sscanf(segment, "%f", &(dataptr[numpts]));
	    }
	    i++;
	    Hv_Free(segment);
	  }
	  Hv_quoteDelimitsSegment = False;


/* check wrong number of data points. Pad columns that are missing data*/

	  if (i != PlotNumSets) {
	    Hv_Println( "WARNING: found line with %d entries, expecting %d", i, PlotNumSets);
	    for (j = i; j < PlotNumSets; j++) {
	      if (settype[j] == STRDATA) {
		strarry = (char **)setdata[j];
		strarry[numpts] = NULL;
	      }
	      else {
		dataptr = (float *)setdata[j];
		if (numpts > 0)
		  dataptr[numpts] = dataptr[numpts-1];
		else 
		  dataptr[numpts] = 0.0;
	      }
	    }
	  }

	  numpts++;
	}
	
      }
    }  /* end read next line */

/* now done reading the record, ready to create a plot */

  donereading:

    if (goodread) {

/*********************************************
  ALL PLOT STYLES ARE IGNORED FOR CONTINUATION
  PLOTS. A Continuation plot will have caused
  BrandNewPlot to be set to False. A new plot
  will have BrandNewPlot set to True. The first
  plot always has pvdata->lastplot set to NULL.
*********************************************/

      if (BrandNewPlot || (pvdata->lastplot == NULL)) {

/* look for special full screen placement */

	if (PlotW < 0) {
	  PlotW = View->hotrect->width - 100;
	  PlotW = Hv_sMax(200, PlotW);
	  PlotRelX = 70;
	}
	if (PlotH < 0) {
	  PlotH = View->hotrect->height - 100;
	  PlotH = Hv_sMax(200, PlotH);
	  PlotRelY = 50;
	}

/* create the actual plot item */

	plot = MakeThePlot(View, fname);

/* now ready to stuff the plot (overwrite defaults) */

	pvdata->lastplot = plot;
	plotdata = Hv_GetPlotData(plot);
	FillPlot(plot);
      }  /* end brand new plot */


/* we get here regardless of whether this was a NEW plot
   of we added a continuation plot */

      plotdata = Hv_GetPlotData(pvdata->lastplot);

/* stuff the curves */

      for (i = 0; i < PlotNumCurves; i++) {
	if (PlotPlotType == Hv_1DHISTO)
	  Stuff1DHisto(pvdata->lastplot, i+1, numpts);
	else
	  StuffPlot(pvdata->lastplot, i+1, numpts);
      }
      
/* if haven't read plot lims from style, set them based on data */
      
      plotdata->xlog = PlotXLog;
      plotdata->ylog = PlotYLog;

      pvdata->plotxlog = PlotXLog;
      pvdata->plotylog = PlotYLog;

/* Hv_ReasonablePlotLimits will affect 
   plotdata->xmin, xmax, ymin, and ymax
   it will NOT affect pvdata->plotxmin etc */

      Hv_ReasonablePlotLimits(plotdata);

/* look for user provided values */

      if (PlotXmin > -1.0e30)
	plotdata->xmin = PlotXmin;
      if (PlotXmax < 1.0e30)
	plotdata->xmax = PlotXmax;
      if (PlotYmin > -1.0e30)
	plotdata->ymin = PlotYmin;
      if (PlotYmax < 1.0e30)
	plotdata->ymax = PlotYmax;


      pvdata->plotxmin = PlotXmin;
      pvdata->plotxmax = PlotXmax;
      pvdata->plotymin = PlotYmin;
      pvdata->plotymax = PlotYmax;


      Hv_SetFRect(plotdata->defworld,
		  plotdata->xmin,
		  plotdata->xmax,
		  plotdata->ymin,
		  plotdata->ymax);

      pvdata->plotlegendleft = PlotLegendLeft;
      pvdata->plotlegendtop = PlotLegendTop;

      plotdata->xlog = (PlotXLog && (plotdata->xmin > 0.0));
      plotdata->ylog = (PlotYLog && (plotdata->ymin > 0.0));

/* place the legend */

      PlaceLabelFromDataVals(pvdata->lastplot, plotdata->legenditem,
			     PlotLegendLeft, PlotLegendTop);

/* place big three labels if not default values */

	Hv_BigThreeItems(pvdata->lastplot, &title, &xlab, &ylab);

	PlaceLabelFromDataVals(pvdata->lastplot, title, PlotTitleLeft,  PlotTitleTop);
	PlaceLabelFromDataVals(pvdata->lastplot, xlab,  PlotXlabelLeft, PlotXlabelTop);
	PlaceLabelFromDataVals(pvdata->lastplot, ylab,  PlotYlabelLeft, PlotYlabelTop);

/* overide above placement because of attachments? */

	Hv_CheckPlotAttachments(pvdata->lastplot);


    }  /* end goodread */

/* add any annotation labels */

    for (plab = PlotLabels; plab != NULL; plab = plab->next) {
      Hv_PlotToLocal(pvdata->lastplot, &sx, &sy, plab->x, plab->y);

      newItem =   Hv_StandardPlotLabel(pvdata->lastplot, sx, sy,
				       plab->text, plab->font);
      newItem->str->strcol = plab->color;
      newItem->str->horizontal = plab->horizontal;
      Hv_CompoundStringArea(newItem->str, sx, sy, newItem->area, Hv_STRINGEXTRA);

    }

    Hv_Free(PlotTitle);
    Hv_Free(PlotXlab);
    Hv_Free(PlotYlab);

    PlotTitle = NULL;
    PlotXlab = NULL;
    PlotYlab = NULL;
    
    for (i = 0; i < PlotNumSets; i++) {
      if (settype[i] == STRDATA) {
	strarry = (char **)setdata[i];
	for (k = 0; k < ndatalines; k++)
	  Hv_Free(strarry[k]);
      }

      Hv_Free(setdata[i]);
      setdata[i] = NULL;
    }
    
  } /* end while FindRec */


  Hv_ReadSpecialPlotRecords(fp, View, fname);
  
  Hv_CheckPlotViewSensitivity(View);
  Hv_Free(line);
  fclose(fp);

  FreeCurveArrays();

  Hv_DrawViewHotRect(View);

  viewrgn = Hv_GetViewRegion(View);          /* get min region the View needs */
  Hv_DrawScrollBars(View, viewrgn);
  Hv_DestroyRegion(viewrgn);

}



/*---------- Hv_ReadSpecialPlotRecords --------------*/

static void Hv_ReadSpecialPlotRecords(FILE    *fp,
				      Hv_View  View,
				      char    *fname) {

/* read specialized records, such as Thieesen */

  Hv_PlotViewData pvdata;
  char           *line;
  int             numpts;
  int             numedge;
  Hv_PlotData     plotdata;
  size_t          size;
  Hv_CurveData    curve;
  int             i;
  int             e1, e2;
  Hv_Item         plot;

  float           xmin = (float)1.0e30;
  float           ymin = (float)1.0e30;
  float           xmax = (float)(-1.0e30);
  float           ymax = (float)(-1.0e30);

  float           dx, dy;
  int             numlines, numnclines;

  if (fp == NULL)
      return;

  rewind (fp);
  pvdata = Hv_GetPlotViewData(View);

/* malloc a line used throughout */

  line = (char *)Hv_Malloc(Hv_LINESIZE);

/* read the theissen triangulation records */

  while (Hv_FindRecord(fp, "THEISSEN", False, line)) {
      Hv_RecordLineCount(fp, "THEISSEN", "ENDREC", &numlines, &numnclines);


      SetPlotDefaults(View);

      
      if (Hv_GetNextNonComment(fp, line) != NULL) {
	  sscanf(line, "%d", &numpts); 

	  numedge = numnclines - numpts - 1;

	  Hv_Println( "Found Theissen record with %d data points and %d edges ", numpts, numedge);


/* create the actual plot item */

	  plot = MakeThePlot(View, fname);

/* now ready to stuff the plot (overwrite defaults) */

	  pvdata->lastplot = plot;
	  plotdata = Hv_GetPlotData(plot);

	  FillPlot(plot);

	  plotdata->plottype = Hv_THEISSEN;

/* if we made it here, we are ready to roll */

	  size = (size_t)(numpts*sizeof(float));

	  curve = Hv_MallocCurveData(NULL);
	  Hv_InsertListItem((Hv_ListPtr *)(&(plotdata->curves)), (Hv_ListPtr)curve);
	  
	  curve->npts = numpts;
	  curve->fittype = numedge;

	  
	  curve->x = (float *)Hv_Malloc(size);
	  curve->y = (float *)Hv_Malloc(size);
	  curve->z = (float *)Hv_Malloc(size);
	  curve->symbolcolor = Hv_red;

/* read data */

	  for (i = 0; i < numpts; i++) {
	      Hv_GetNextNonComment(fp, line);
	      sscanf(line, "%f %f %f", curve->x+i, curve->y+i, curve->z+i);
	      xmin = Hv_fMin(xmin, curve->x[i]);
	      ymin = Hv_fMin(ymin, curve->y[i]);

	      xmax = Hv_fMax(xmax, curve->x[i]);
	      ymax = Hv_fMax(ymax, curve->y[i]);

	      Hv_Println("[%d] %f %f %f", i+1, curve->x[i], curve->y[i], curve->z[i]);
	  }

/* read edges, store as floats */


	  size = (size_t)(numedge*sizeof(float));
	  curve->xerr = (float *)Hv_Malloc(size);   /* start of edge */
	  curve->yerr = (float *)Hv_Malloc(size);   /* end of edge */

	  for (i = 0; i < numedge; i++) {
	      Hv_GetNextNonComment(fp, line);
	      sscanf(line, "%d %d", &e1, &e2);
	      e1 -= 1;
	      e2 -= 1;

	      curve->xerr[i] = (float)e1;
	      curve->yerr[i] = (float)e2;
	      Hv_Println("[%d] %d %d", i+1, (int)(curve->xerr[i]), (int)(curve->yerr[i]));
	  }


	  dx = xmax - xmin;
	  dy = ymax - ymin;

	  xmax = (float)(xmin + 1.1*dx);
	  xmin = (float)(xmax - 1.2*dx);

	  ymax = (float)(ymin + 1.1*dy);
	  ymin = (float)(ymax - 1.2*dy);

	  pvdata->plotxmin = xmin;     plotdata->xmin = xmin;     curve->xmin = xmin;
	  pvdata->plotxmax = xmax;     plotdata->xmax = xmax;     curve->xmax = xmax;
	  pvdata->plotymin = ymin;     plotdata->ymin = ymin;     curve->ymin = ymin;
	  pvdata->plotymax = ymax;     plotdata->ymax = ymax;     curve->ymax = ymax;


      }
      

  }

  Hv_Free(line);
}


/*---------- FillPlot ----------*/

static void  FillPlot(Hv_Item plot)

/* (partially) stuffs plot with the values
   loaded into the global style variables */

{
  Hv_PlotData     plotdata;
  Hv_Item         title, xlab, ylab;

  plotdata = Hv_GetPlotData(plot);

  plotdata->framelegend = DefaultPlotData->framelegend;
  plotdata->uselegend = DefaultPlotData->uselegend;
  plotdata->legendfont = PlotLegendFont;
  plotdata->fitresultfont = PlotFitResultFont;
  plotdata->axesfont = PlotAxesFont;
  
  plotdata->opaquelegend = DefaultPlotData->opaquelegend;
  plotdata->fancysymbols = PlotFancySymbols;
  plotdata->xzeroline = PlotXZeroLine;
  plotdata->yzeroline = PlotYZeroLine;

  Hv_BigThreeItems(plot, &title, &xlab, &ylab);
  
/* set the "big three" labels: title, xaxis, and yaxis */
	
  if (title != NULL) {
    title->str->font = PlotTitleFont;
    title->str->strcol = PlotTitleColor;
  }
  if (xlab != NULL) {
    xlab->str->font = PlotXLabelFont;
    xlab->str->strcol = PlotXLabelColor;
    
  }
  if (ylab != NULL) {
    ylab->str->font = PlotYLabelFont;
    ylab->str->strcol = PlotYLabelColor;
  }
  
  plotdata->drawxaxisvalues = PlotDrawXAxisValues;
  plotdata->drawyaxisvalues = PlotDrawYAxisValues;
  
  plotdata->ebartops = PlotErrorBarTops;
  plotdata->drawmajxgrid = PlotMajXGrid;
  plotdata->drawmajygrid = PlotMajYGrid;
  plotdata->drawminxgrid = PlotMinXGrid;
  plotdata->drawminygrid = PlotMinYGrid;
  
  plotdata->drawmajxticks = PlotUseMajXTick;
  plotdata->drawmajyticks = PlotUseMajYTick;
  plotdata->drawminxticks = PlotUseMinXTick;
  plotdata->drawminyticks = PlotUseMinYTick;
  
  plotdata->scilowlim = PlotSciLowLim;
  plotdata->scihilim = PlotSciHiLim;
  
  plotdata->drawminxval = PlotDrawMinX;
  plotdata->drawminyval = PlotDrawMinY;
  plotdata->xtietag = PlotXTieTag;
  plotdata->xprecision = DefaultPlotData->xprecision;
  plotdata->yprecision = DefaultPlotData->yprecision;
  plotdata->axesstyle = PlotAxesStyle;
  plotdata->plottype = PlotPlotType;

  plotdata->attachlegend = PlotAttachLegend;
  plotdata->attachtitle = PlotAttachTitle;
  plotdata->attachxlab = PlotAttachXlab;
  plotdata->attachylab = PlotAttachYlab;

  plotdata->axescolor = PlotAxesColor;
  plotdata->majgridcolor = PlotMajGridColor;
  plotdata->mingridcolor = PlotMinGridColor;
  plotdata->majxtick = PlotXMajTick;
  plotdata->majytick = PlotYMajTick;
  plotdata->minxtick = PlotXMinTick;
  plotdata->minytick = PlotYMinTick;
  plotdata->background = DefaultPlotData->background;
  plotdata->usebackground = PlotUseBackground;
  plotdata->ynozoom = PlotYNoZoom;
  plotdata->xnozoom = PlotXNoZoom;
  plotdata->showbackground = PlotShowBackground;
  
  if (plotdata->uselegend)
    Hv_ClearBit(&(plotdata->legenditem->drawcontrol), Hv_NOPOINTERCHECK);
  else
    Hv_SetBit(&(plotdata->legenditem->drawcontrol), Hv_NOPOINTERCHECK);
  
  
}


/*--------- MakeThePlot -----------*/

static Hv_Item MakeThePlot(Hv_View View,
			   char *fname)

/* creates the plot item using values
   from the global style variables */

{
  Hv_Item    plot;

  plot = Hv_VaCreateItem(View,
			 Hv_DOMAIN,        Hv_INSIDEHOTRECT,
			 Hv_DRAWCONTROL,   Hv_DRAGGABLE + Hv_ENLARGEABLE + Hv_ZOOMABLE,
			 Hv_FILENAME,      fname,
			 Hv_TYPE,          Hv_PLOTITEM,
			 Hv_SHOW,          (short)PlotShowFileName,
			 Hv_LEFT,          PlotRelX,
			 Hv_TOP,           PlotRelY,
			 Hv_WIDTH,         PlotW,
			 Hv_HEIGHT,        PlotH,
			 Hv_XMIN,          PlotXmin,
			 Hv_XMAX,          PlotXmax,
			 Hv_YMIN,          PlotYmin,
			 Hv_YMAX,          PlotYmax,
			 Hv_TITLE,         PlotTitle,
			 Hv_XLABEL,        PlotXlab,
			 Hv_YLABEL,        PlotYlab,
			 NULL);
  
  return plot;

}

/*--------- PlaceLabelFromDataVals -----------*/

static void PlaceLabelFromDataVals(Hv_Item plot,
				   Hv_Item label,
				   float   fx,
				   float   fy)

/* position a label based on data coordinates
   (unless they are still default vals) */

{
  short           sx, sy, dum;
  Hv_PlotData     plotdata;

  plotdata = Hv_GetPlotData(plot);

  if (fx < 1.0e30) {
    Hv_PlotToLocal(plot, &sx, &dum, fx, plotdata->ymax);
    label->area->left = sx;
    Hv_FixRectRB(label->area);
  }
  
  if (fy < 1.0e30) {
    Hv_PlotToLocal(plot, &dum, &sy, plotdata->xmin, fy);
    label->area->top = sy;
    Hv_FixRectRB(label->area);
  }
  
}


/*------- Hv_CountPlots ---------*/

int Hv_CountPlots(Hv_View View)

{
  Hv_Item   temp;
  int       count = 0;

  for (temp = View->items->first; temp != NULL; temp = temp->next) {
    if (temp->type == Hv_PLOTITEM)
      count++;
  }

  return count;
}

/*------- Hv_DeletePlots ---------*/

void Hv_DeletePlots(Hv_View View)

/******************************************
  Deletes all plot items and their
  children from a view.
******************************************/


{
  Hv_Item   temp, temp2;
 
  for (temp = View->items->first; temp != NULL; temp = temp2) {
    temp2 = temp->next;

    if (temp->type == Hv_PLOTITEM)
      Hv_DestroyItem(temp);
  }

  Hv_InitPlotViewData(View);
/*   Hv_DrawViewHotRect(View); */
}



/*------- static functions ---*/


/*-------- DumpPlot --------*/

static void DumpPlot(Hv_Item  plot,
		     FILE   *fp)

/* write all the plot info to the file */

{
  Hv_PlotData    plotdata;
  Hv_CurveData   tcurve;

  if (plot == NULL)
    return;

  plotdata = Hv_GetPlotData(plot);

  if (plotdata == NULL)
    return;

  for (tcurve = plotdata->curves; tcurve != NULL; tcurve = tcurve->next)
    DumpCurve(plot, tcurve, tcurve == plotdata->curves, fp);
}

/*------- DumpLabel ------*/

static void DumpLabel(Hv_Item  label,
		      FILE    *fp)
{
  Hv_Item    plot;
  float      fx,  fy;
  short      x, y;

  if ((label == NULL) || (label->parent == NULL))
    return;

  plot = label->parent;

  Hv_CompoundStringXYFromArea(label->str, &x, &y, label->area, Hv_STRINGEXTRA);

  Hv_LocalToPlot(plot, x, y, &fx, &fy);

  if (label->str->horizontal) 
    fprintf(fp, "\tstyle\tlabel %-12.5e %-12.5e ", fx, fy);
  else
    fprintf(fp, "\tstyle\tvlabel %-12.5e %-12.5e ", fx, fy);

  fprintf(fp, " %s ", Hv_charSet[label->str->font]);

  SimpleDumpColor(fp, label->str->strcol);

  fprintf(fp, " %s\n", label->str->text);
}


/*------- DumpCurve --------*/

static void DumpCurve(Hv_Item    plot,
		      Hv_CurveData  curve,
		      Boolean    first,
		      FILE      *fp)

{
  if (first)
    fprintf(fp, "\n!**** New Plot ****\n");

  fprintf(fp, "[PLOTREC]\n");

  if (first)
    DumpPlotStyles(plot, fp);
  else
    fprintf(fp, "\tstyle\tcontinue\n");

  DumpCurveStyles(curve, fp);
  DumpCurveData(curve, fp);

  fprintf(fp, "[ENDREC]\n\n");
}

/*-------- DumpABigThreeLabel ----------*/

static void DumpABigThreeLabel(FILE    *fp,
			       Hv_Item plot,
			       Hv_Item label,
			       char    *labstylestr,
			       char    *fontstylestr,
			       char    *colorstylestr,
			       char    *leftstylestr,
			       char    *topstylestr)

/*********************************
  Dumps one of the big three labels,
  which are handled differently from
  generic labels
***********************************/


{
  float   fx, fy;

  if ((label != NULL)  && (label->str != NULL) && (label->str->text != NULL)) {
    fprintf(fp, "\tstyle\t%s\t%s\n", labstylestr, label->str->text);
    fprintf(fp, "\tstyle\t%s\t%s\n", fontstylestr, Hv_charSet[label->str->font]);
    DumpColor(fp, True, colorstylestr, label->str->strcol, Hv_black);

/* position based on data values */

    Hv_LocalToPlot(plot, label->area->left, label->area->top, &fx, &fy);

    if (fabs(label->area->left) < 31999)
      fprintf(fp, "\tstyle\t%s\t%f\n", leftstylestr, fx);
    if (fabs(label->area->top) < 31999)
      fprintf(fp, "\tstyle\t%s\t%f\n", topstylestr, fy);
  }
  
}


/*-------- DumpPlotStyles --------*/

static void DumpPlotStyles(Hv_Item    plot,
			   FILE      *fp)

{
  Hv_PlotData    plotdata, defpd;
  Hv_View        View = plot->view;
  float          fx, fy;
  Hv_Item        temp, title, xlab, ylab;

  plotdata = Hv_GetPlotData(plot);

  defpd = Hv_MallocPlotData();    /* ensured to have the defaults! */

/* dump the "big three" labels: title, x axis and y axis */

  Hv_BigThreeItems(plot, &title, &xlab, &ylab);

  DumpABigThreeLabel(fp, plot, title, "title", "titlefont",
		     "titlecolor", "titleleft",   "titletop");

  DumpABigThreeLabel(fp, plot, xlab,  "xlabel", "xlabelfont",
		     "xlabelcolor", "xlabelleft", "xlabeltop");

  DumpABigThreeLabel(fp, plot, ylab,  "ylabel", "ylabelfont",
		     "ylabelcolor", "ylabelleft", "ylabeltop");

/* dump plot position */

  fprintf(fp, "\tstyle \tleft\t%d\n",   plot->area->left - View->hotrect->left);
  fprintf(fp, "\tstyle \ttop\t%d\n",    plot->area->top - View->hotrect->top);
  fprintf(fp, "\tstyle \twidth\t%d\n",  plot->area->width);
  fprintf(fp, "\tstyle \theight\t%d\n", plot->area->height);

  fprintf(fp, "\tstyle \txmin\t%-12.5e\n",   plotdata->xmin);
  fprintf(fp, "\tstyle \txmax\t%-12.5e\n",   plotdata->xmax);
  fprintf(fp, "\tstyle \tymin\t%-12.5e\n",   plotdata->ymin);
  fprintf(fp, "\tstyle \tymax\t%-12.5e\n",   plotdata->ymax);

  DumpInt(fp, True, "majxtick",   plotdata->majxtick,   defpd->majxtick);
  DumpInt(fp, True, "majytick",   plotdata->majytick,   defpd->majytick);
  DumpInt(fp, True, "minxtick",   plotdata->minxtick,   defpd->minxtick);
  DumpInt(fp, True, "minytick",   plotdata->minytick,   defpd->minytick);
  DumpInt(fp, True, "xtietag",    plotdata->xtietag,    defpd->xtietag);

/* dump precision no matter what (hack for histograms) */

  DumpInt(fp, True, "xprecision", plotdata->xprecision, -99);
  DumpInt(fp, True, "yprecision", plotdata->yprecision, -99);

  DumpFloat(fp, True, "background", plotdata->background, defpd->background);
  DumpFloat(fp, True, "scilolim", plotdata->scilowlim, defpd->scilowlim);
  DumpFloat(fp, True, "scihilim", plotdata->scihilim, defpd->scihilim);

/* dump the legend position -- no need to if it is attached*/

  if (plotdata->legenditem != NULL) 
    if (!plotdata->attachlegend) {
      Hv_LocalToPlot(plot, plotdata->legenditem->area->left,
		     plotdata->legenditem->area->top, &fx, &fy);
      DumpFloat(fp, True, "legendleft", fx, (float)(-1.0e30));
      DumpFloat(fp, True, "legendtop",  fy, (float)(-1.0e30));
    }

  DumpColor(fp, True, "majgridcolor", plotdata->majgridcolor, defpd->majgridcolor);
  DumpColor(fp, True, "mingridcolor", plotdata->mingridcolor, defpd->mingridcolor);
  DumpColor(fp, True, "axescolor",    plotdata->axescolor, defpd->axescolor);

  DumpBoolean(fp, True, "ynozoom", plotdata->ynozoom, defpd->ynozoom);
  DumpBoolean(fp, True, "xnozoom", plotdata->xnozoom, defpd->xnozoom);

  DumpBoolean(fp, True, "attachlegend", plotdata->attachlegend,
	      defpd->attachlegend);

  DumpBoolean(fp, True, "attachtitle", plotdata->attachtitle,
	      defpd->attachtitle);

  DumpBoolean(fp, True, "attachxlab", plotdata->attachxlab,
	      defpd->attachxlab);

  DumpBoolean(fp, True, "attachylab", plotdata->attachylab,
	      defpd->attachylab);

  DumpBoolean(fp, True, "useback", plotdata->usebackground, defpd->usebackground);
  DumpBoolean(fp, True, "showback", plotdata->showbackground, defpd->showbackground);
  DumpBoolean(fp, True, "filename", plotdata->showfilename, defpd->showfilename);
  DumpBoolean(fp, True, "xlog", plotdata->xlog, defpd->xlog);
  DumpBoolean(fp, True, "ylog", plotdata->ylog, defpd->ylog);
  DumpBoolean(fp, True, "opaquelegend", plotdata->opaquelegend, defpd->opaquelegend);
  DumpBoolean(fp, True, "fancysymbols", plotdata->fancysymbols, defpd->fancysymbols);
  DumpBoolean(fp, True, "xzeroline",    plotdata->xzeroline,    defpd->xzeroline);
  DumpBoolean(fp, True, "yzeroline",    plotdata->yzeroline,    defpd->yzeroline);
  DumpBoolean(fp, True, "legend",       plotdata->uselegend,    defpd->uselegend);
  DumpBoolean(fp, True, "legendframe",  plotdata->framelegend,  defpd->framelegend);

  DumpBoolean(fp, True, "majxgrid",     plotdata->drawmajxgrid, defpd->drawmajxgrid);
  DumpBoolean(fp, True, "majygrid",     plotdata->drawmajygrid, defpd->drawmajygrid);
  DumpBoolean(fp, True, "minxgrid",     plotdata->drawminxgrid, defpd->drawminxgrid);
  DumpBoolean(fp, True, "minygrid",     plotdata->drawminygrid, defpd->drawminygrid);

  DumpBoolean(fp, True, "usemajxtick",  plotdata->drawmajxticks, defpd->drawmajxticks);
  DumpBoolean(fp, True, "usemajytick",  plotdata->drawmajyticks, defpd->drawmajyticks);
  DumpBoolean(fp, True, "useminxtick",  plotdata->drawminxticks, defpd->drawminxticks);
  DumpBoolean(fp, True, "useminytick",  plotdata->drawminyticks, defpd->drawminyticks);
  DumpBoolean(fp, True, "errorbartop",  plotdata->ebartops,      defpd->ebartops);
  DumpBoolean(fp, True, "drawxaxisvalues",  plotdata->drawxaxisvalues, defpd->drawxaxisvalues);
  DumpBoolean(fp, True, "drawyaxisvalues",  plotdata->drawyaxisvalues, defpd->drawyaxisvalues);
  DumpBoolean(fp, True, "drawminx",     plotdata->drawminxval, defpd->drawminxval);
  DumpBoolean(fp, True, "drawminy",     plotdata->drawminyval, defpd->drawminyval);

  if (plotdata->plottype != defpd->plottype)
    fprintf(fp, "\tstyle\tplottype\t%s\n", PlotTypeStr[plotdata->plottype]);

  if (plotdata->axesstyle != defpd->axesstyle)
    fprintf(fp, "\tstyle\taxesstyle\t%s\n", AxesStr[plotdata->axesstyle]);

  if (plotdata->axesfont != defpd->axesfont)
    fprintf(fp, "\tstyle\taxesfont\t%s\n", Hv_charSet[plotdata->axesfont]);

  if (plotdata->legendfont != defpd->legendfont)
    fprintf(fp, "\tstyle\tlegendfont\t%s\n", Hv_charSet[plotdata->legendfont]);

  if (plotdata->fitresultfont != defpd->fitresultfont)
    fprintf(fp, "\tstyle\tfitfont\t%s\n", Hv_charSet[plotdata->fitresultfont]);

/* now dump annotation items */

  if (plot->children != NULL)
    for (temp = plot->children->first; temp != NULL; temp = temp->next) {
      if ((temp->type == Hv_TEXTITEM) && (temp != title) && (temp != xlab) && (temp != ylab) && (temp != plotdata->filenameitem))
	DumpLabel(temp, fp);
    }
  

  Hv_DestroyPlotData(defpd);

}


/*-------- DumpCurveStyles --------*/

static void DumpCurveStyles(Hv_CurveData  curve,
			    FILE      *fp)

{
  Hv_CurveData    defcurve;

  defcurve = Hv_MallocCurveData("x");

  fprintf(fp, "\tstyle\tcurve 1\tname\t%s\n", curve->name);
  DumpBoolean(fp, False, "display",  curve->display,    defcurve->display);
  DumpBoolean(fp, False, "yspike",   curve->yspike,     defcurve->yspike);
  DumpBoolean(fp, False, "absval",   curve->absval,     defcurve->absval);
  DumpBoolean(fp, False, "onlegend", curve->onlegend,   defcurve->onlegend);
  DumpBoolean(fp, False, "usexerrs", curve->usexerrs,   defcurve->usexerrs);
  DumpBoolean(fp, False, "useyerrs", curve->useyerrs,   defcurve->useyerrs);
  DumpInt(fp, False, "thickness",    curve->thickness,  defcurve->thickness);
  DumpInt(fp, False, "numgauss",     curve->numgauss,   defcurve->numgauss);
  DumpInt(fp, False, "polyorder",    curve->polyorder,  defcurve->polyorder);
  DumpInt(fp, False, "legendreorder",    curve->legendreorder,  defcurve->legendreorder);
  DumpInt(fp, False, "symbolsize",   curve->symbolsize, defcurve->symbolsize);

  DumpFloat(fp, False, "xscale", curve->xscale, defcurve->xscale);
  DumpFloat(fp, False, "yscale", curve->yscale, defcurve->yscale);

  DumpColor(fp, False, "linecolor", curve->linecolor, defcurve->linecolor);
  DumpColor(fp, False, "symbolcolor", curve->symbolcolor, defcurve->symbolcolor);

  if (curve->style != defcurve->style)
    fprintf(fp, "\tstyle curve 1 \tlinestyle\t%s\n", LineStyleStr[curve->style]);
  
  if (curve->algorithm != defcurve->algorithm)
    fprintf(fp, "\tstyle curve 1 \talgorithm\t%s\n", AlgorithmStr[curve->algorithm]);

  if (curve->fittype != defcurve->fittype)
    fprintf(fp, "\tstyle curve 1 \tfittype\t%s\n", FitTypeStr[curve->fittype]);

  if (curve->symbol != defcurve->symbol)
    fprintf(fp, "\tstyle curve 1 \tsymbol\t%s\n", SymbolStr[curve->symbol-1]);

  if (curve->pattern != defcurve->pattern)
    fprintf(fp, "\tstyle curve 1 \tpattern\t%s\n", PatternStr[curve->pattern]);

  Hv_DestroyCurveData(defcurve);
}

/*------ DumpBoolean ----*/

static void DumpBoolean(FILE   *fp,
			Boolean  isplot,
			char   *name,
			Boolean  val,
			Boolean  dflt)

/* isplot:  if true, this is a plot value else its a curve value*/

{
  char   *header;

/* no need to print defaults */

  if (val == dflt)
    return;

  if (isplot)
    Hv_InitCharStr(&header, "\tstyle \t");
  else
    Hv_InitCharStr(&header, "\tstyle curve 1 \t");

  if (val)
    fprintf(fp, "%s%s\ton\n", header, name);
  else
    fprintf(fp, "%s%s\toff\n", header, name);

  Hv_Free(header);
}

/*------ DumpInt ----*/

static void DumpInt(FILE   *fp,
		    Boolean  isplot,
		    char   *name,
		    int    val,
		    int    dflt)

{
  char   *header;

/* no need to print defaults */

  if (val == dflt)
    return;

  if (isplot)
    Hv_InitCharStr(&header, "\tstyle \t");
  else
    Hv_InitCharStr(&header, "\tstyle curve 1 \t");

  fprintf(fp, "%s%s\t%d\n", header, name, val);

  Hv_Free(header);
}


/*------ DumpFloat -------*/

static void DumpFloat(FILE   *fp,
		      Boolean  isplot,
		      char   *name,
		      float   val,
		      float   dflt)


{
  char   *header;

/* no need to print defaults */

  if (val == dflt)
    return;

  if (isplot)
    Hv_InitCharStr(&header, "\tstyle \t");
  else
    Hv_InitCharStr(&header, "\tstyle curve 1 \t");

  fprintf(fp, "%s%s\t%-12.5e\n", header, name, val);

  Hv_Free(header);
}

/*------ DumpColor ----*/

static void DumpColor(FILE   *fp,
		      Boolean  isplot,
		      char   *name,
		      short   val,
		      short   dflt)


{
  char   *header;

/* no need to print defaults */

  if (val == dflt)
    return;

  if (isplot)
    Hv_InitCharStr(&header, "\tstyle \t");
  else
    Hv_InitCharStr(&header, "\tstyle curve 1 \t");

  if (val == Hv_red)
    fprintf(fp, "%s%s\tred\n", header, name);
  else if (val == Hv_black)
    fprintf(fp, "%s%s\tblack\n", header, name);
  else if (val == Hv_brown)
    fprintf(fp, "%s%s\tbrown\n", header, name);
  else if (val == Hv_gray10)
    fprintf(fp, "%s%s\tgray\n", header, name);
  else if (val == Hv_forestGreen)
    fprintf(fp, "%s%s\tgreen\n", header, name);

  else if (val == Hv_orange)
    fprintf(fp, "%s%s\torange\n", header, name);
  else if (val == Hv_purple)
    fprintf(fp, "%s%s\tpurple\n", header, name);
  else if (val == Hv_yellow)
    fprintf(fp, "%s%s\tyellow\n", header, name);
  else if (val == Hv_white)
    fprintf(fp, "%s%s\twhite\n", header, name);
  else if (val == Hv_powderBlue)
    fprintf(fp, "%s%s\tlightblue\n", header, name);
  else if (val < 0)
    fprintf(fp, "%s%s\tnone\n", header, name);

  else
    fprintf(fp, "%s%s\t%d\n", header, name, val - Hv_red);

  Hv_Free(header);
}

/*------ SimpleDumpColor ----*/

static void SimpleDumpColor(FILE   *fp,
			    short   val)

{

  if (val == Hv_red)
    fprintf(fp, " red ");
  else if (val == Hv_black)
    fprintf(fp, " black ");
  else if (val == Hv_blue)
    fprintf(fp, " blue ");
  else if (val == Hv_brown)
    fprintf(fp, " brown ");
  else if (val == Hv_gray10)
    fprintf(fp, " gray ");
  else if (val == Hv_forestGreen)
    fprintf(fp, " green ");
  else if (val == Hv_orange)
    fprintf(fp, " orange ");
  else if (val == Hv_purple)
    fprintf(fp, " purple ");
  else if (val == Hv_yellow)
    fprintf(fp, " yellow ");
  else if (val == Hv_white)
    fprintf(fp, " white ");
  else if (val == Hv_powderBlue)
    fprintf(fp, " lightblue ");
  else if (val < 0)
    fprintf(fp, " none ");
  else
    fprintf(fp, " black ");
}


/*-------- DumpCurveData --------*/

static void DumpCurveData(Hv_CurveData  curve,
			  FILE      *fp)

{
  char   *order;
  int    i;
  float  *x, *y, *v, *h;
  char   **c;

  if ((curve == NULL) ||
      ((curve->x == NULL) && (curve->categories == NULL)) ||
      (curve->y == NULL))
    return;

  x = curve->x;
  y = curve->y;
  h = curve->xerr;
  v = curve->yerr;
  c = curve->categories;

  order = (char *)Hv_Malloc(60);
  strcpy(order, "X Y");
  if (h != NULL)
    strcat(order, " H"); 
  if (c != NULL)
    strcat(order, " C"); 
  if (v != NULL)
    strcat(order, " V");
  fprintf(fp, "\tstyle\torder\t%s\n", order);
  Hv_Free(order);
 
  for (i = 0; i < curve->npts; i++) {
    fprintf(fp, "\t\t%-12.5e\t%-12.5e", x[i], y[i]); 
    if (c != NULL)
      fprintf(fp, "\t%s", c[i]); 
    if (h != NULL)
      fprintf(fp, "\t%-12.5e", h[i]); 
    if (v != NULL)
      fprintf(fp, "\t%-12.5e", v[i]); 
    fprintf(fp, "\n");
  }

}


/*----- ParseStyleLine -------*/

static void ParseStyleLine(Hv_Item     lastplot,
			   char       *line)

{
  char             *temp, *temp2;
  char             *segment, *key;
  int               cnum, dummy;
  char             *line2;
  short             font, color;
  float             vx, vy, fdummy;
  short             units;

  Hv_PlotViewData   pvdata;
  temp = line;

  font = Hv_helvetica14;
  color = Hv_black;

/* first segment should be the "style" word.
   second segment should be the key */
  
  segment = Hv_NextTextSegment(&temp);

  if (!Hv_Contained(segment, "style")) {
    Hv_Free(segment);
    return;
  }

  Hv_Free(segment);

  key = Hv_NextTextSegment(&temp);

  if (Hv_Contained(key, "order")) {
    PlotNumSets = 0;
    PlotNumCurves = 0;
    PlotNumXSets = 0;
    PlotNumCSets = 0;

    while ((segment = Hv_NextTextSegment(&temp)) != NULL) {

      if (Hv_Contained(segment, "D"))
	settype[PlotNumSets] = DUMMYDATA;

      else if (Hv_Contained(segment, "Y")) {
	settype[PlotNumSets] = YDATA;
	PlotNumCurves++;
      }

      else if (Hv_Contained(segment, "X")) {
	settype[PlotNumSets] = XDATA;
	PlotNumXSets++;
      }

      else if (Hv_Contained(segment, "C")) {
	settype[PlotNumSets] = STRDATA;
	PlotNumCSets++;
      }

      else if (Hv_Contained(segment, "N2")) {
	settype[PlotNumSets] = N2DATA;
	PlotNumCurves++;
      }

      else if (Hv_Contained(segment, "N")) {
	settype[PlotNumSets] = N1DATA;
	PlotNumCurves++;
	Hv_Println("found histo data");
      }

      else if (Hv_Contained(segment, "V"))
	settype[PlotNumSets] = YERRDATA;
      else if (Hv_Contained(segment, "H"))
	settype[PlotNumSets] = XERRDATA;
      else
	settype[PlotNumSets] = DUMMYDATA;
      Hv_Free(segment);
      PlotNumSets++;
    }
  }

/* continuation plot? */

  else if (Hv_Contained(key, "contin")) {
    if (lastplot != NULL) {
      BrandNewPlot = False;
      pvdata = Hv_GetPlotViewData(lastplot->view);
      PlotXmin = pvdata->plotxmin;
      PlotXmax = pvdata->plotxmax;
      PlotYmin = pvdata->plotymin;
      PlotYmax = pvdata->plotymax;
      PlotXLog = pvdata->plotxlog;
      PlotYLog = pvdata->plotylog;
      PlotLegendLeft = pvdata->plotlegendleft;
      PlotLegendTop = pvdata->plotlegendtop;
      PlotXlabelLeft = (float)(2.0e30);
      PlotXlabelTop  = (float)(2.0e30);
      PlotYlabelLeft = (float)(2.0e30);
      PlotYlabelTop  = (float)(2.0e30);
    }
  }

/******************************************************
  NOTE: be carefule how things ared added. For example
  If "xlabelleft" and "xlabel" are both keywords, then
  we MUST check for xlabelleft before xlabel, etc.
*****************************************************/

  else if (Hv_Contained(key, "titleleft"))
    sscanf(temp, "%f", &PlotTitleLeft);

  else if (Hv_Contained(key, "titletop"))
    sscanf(temp, "%f", &PlotTitleTop);

  else if (Hv_Contained(key, "xlabelleft"))
    sscanf(temp, "%f", &PlotXlabelLeft);

  else if (Hv_Contained(key, "xlabeltop"))
    sscanf(temp, "%f", &PlotXlabelTop);

  else if (Hv_Contained(key, "ylabelleft"))
    sscanf(temp, "%f", &PlotYlabelLeft);

  else if (Hv_Contained(key, "ylabeltop"))
    sscanf(temp, "%f", &PlotYlabelTop);

  else if (Hv_Contained(key, "legendfont"))
    PlotLegendFont = Hv_TextToFont(temp);

  else if (Hv_Contained(key, "fitfont"))
    PlotFitResultFont = Hv_TextToFont(temp);

  else if (Hv_Contained(key, "axesfont"))
    PlotAxesFont = Hv_TextToFont(temp);

  else if (Hv_Contained(key, "titlefont"))
    PlotTitleFont = Hv_TextToFont(temp);

  else if (Hv_Contained(key, "xlabelfont"))
    PlotXLabelFont = Hv_TextToFont(temp);

  else if (Hv_Contained(key, "ylabelfont"))
    PlotYLabelFont = Hv_TextToFont(temp);

  else if (Hv_Contained(key, "titlecolor"))
    PlotTitleColor = Hv_ColorFromText(temp);

  else if (Hv_Contained(key, "xlabelcolor"))
    PlotXLabelColor = Hv_ColorFromText(temp);

  else if (Hv_Contained(key, "ylabelcolor"))
    PlotYLabelColor = Hv_ColorFromText(temp);

  else if (Hv_Contained(key, "title")) {
    RestOfString(&temp);
	temp2 = AddSpace(temp);
    Hv_Free(PlotTitle);
    Hv_InitCharStr(&PlotTitle, temp2);
	Hv_Free(temp2);
  }

  else if (Hv_Contained(key, "xlabel")) {
    RestOfString(&temp);
	temp2 = AddSpace(temp);
    Hv_Free(PlotXlab);
    Hv_InitCharStr(&PlotXlab, temp2);
	Hv_Free(temp2);
  }
  else if (Hv_Contained(key, "ylabel")) {
    RestOfString(&temp);
	temp2 = AddSpace(temp);
    Hv_Free(PlotYlab);
    Hv_InitCharStr(&PlotYlab, temp2);
	Hv_Free(temp2);
  }

  else if (Hv_Contained(key, "label")) {
    segment = Hv_NextTextSegment(&temp);
    if (segment != NULL) {
      sscanf(segment, "%f", &vx);
      Hv_Free(segment);
    }

    segment = Hv_NextTextSegment(&temp);
    if (segment != NULL) {
      sscanf(segment, "%f", &vy);
      Hv_Free(segment);
    }

    segment = Hv_NextTextSegment(&temp);
    if (segment != NULL) {
      font = Hv_TextToFont(segment);
      Hv_Free(segment);
    }

    segment = Hv_NextTextSegment(&temp);
    if (segment != NULL) {
      color = Hv_ColorFromText(segment);
      Hv_Free(segment);
    }

    RestOfString(&temp);
	temp2 = AddSpace(temp);

/* see if it was really a vertical label */

    if (Hv_Contained(key, "vlabel"))
      MallocPlotLabel(vx, vy, font, color, temp2, False);
    else
      MallocPlotLabel(vx, vy, font, color, temp2, True);
	Hv_Free(temp2);
      
  }


  else if (Hv_Contained(key, "legendbox"))
    DefaultPlotData->framelegend = BooleanFromText(temp);

  else if (Hv_Contained(key, "opaque"))
    DefaultPlotData->opaquelegend = BooleanFromText(temp);

  else if (Hv_Contained(key, "fancy"))
    PlotFancySymbols = BooleanFromText(temp);

  else if (Hv_Contained(key, "xzeroline"))
    PlotXZeroLine = BooleanFromText(temp);

  else if (Hv_Contained(key, "yzeroline"))
    PlotYZeroLine = BooleanFromText(temp);

  else if (Hv_Contained(key, "legendframe"))
    DefaultPlotData->framelegend = BooleanFromText(temp);

  else if (Hv_Contained(key, "legendleft"))
    sscanf(temp, "%f", &PlotLegendLeft);

  else if (Hv_Contained(key, "legendtop"))
    sscanf(temp, "%f", &PlotLegendTop);

  else if (Hv_Contained(key, "attachleg"))
    PlotAttachLegend = BooleanFromText(temp);

  else if (Hv_Contained(key, "attachtit"))
    PlotAttachTitle = BooleanFromText(temp);

  else if (Hv_Contained(key, "attachxl"))
    PlotAttachXlab = BooleanFromText(temp);

  else if (Hv_Contained(key, "attachyl"))
    PlotAttachYlab = BooleanFromText(temp);

  else if (Hv_Contained(key, "legend"))
    DefaultPlotData->uselegend = BooleanFromText(temp);

  else if (Hv_Contained(key, "majxgrid"))
    PlotMajXGrid = BooleanFromText(temp);
  else if (Hv_Contained(key, "majygrid"))
    PlotMajYGrid = BooleanFromText(temp);
  else if (Hv_Contained(key, "minxgrid"))
    PlotMinXGrid = BooleanFromText(temp);
  else if (Hv_Contained(key, "minygrid"))
    PlotMinYGrid = BooleanFromText(temp);

  else if (Hv_Contained(key, "usemajxtick"))
    PlotUseMajXTick = BooleanFromText(temp);
  else if (Hv_Contained(key, "usemajytick"))
    PlotUseMajYTick = BooleanFromText(temp);
  else if (Hv_Contained(key, "useminxtick"))
    PlotUseMinXTick = BooleanFromText(temp);
  else if (Hv_Contained(key, "useminytick"))
    PlotUseMinYTick = BooleanFromText(temp);


  else if (Hv_Contained(key, "errorbartop"))
    PlotErrorBarTops = BooleanFromText(temp);
  else if (Hv_Contained(key, "drawxaxisv"))
    PlotDrawXAxisValues = BooleanFromText(temp);
  else if (Hv_Contained(key, "drawyaxisv"))
    PlotDrawYAxisValues = BooleanFromText(temp);
  else if (Hv_Contained(key, "drawyaxisv"))
    PlotDrawYAxisValues = BooleanFromText(temp);
  else if (Hv_Contained(key, "drawminx"))
    PlotDrawMinX = BooleanFromText(temp);
  else if (Hv_Contained(key, "drawminy"))
    PlotDrawMinY = BooleanFromText(temp);
  else if (Hv_Contained(key, "xlog"))
    PlotXLog = BooleanFromText(temp);
  else if (Hv_Contained(key, "ylog"))
    PlotYLog = BooleanFromText(temp);

  else if (Hv_Contained(key, "majxtick")) {
    sscanf(temp, "%d", &dummy);
    PlotXMajTick = (short)dummy;
    PlotXMajTick = Hv_sMax(0, Hv_sMin(20, PlotXMajTick));
  }
  else if (Hv_Contained(key, "majytick")) {
    sscanf(temp, "%d", &dummy);
    PlotYMajTick = (short)dummy;
    PlotYMajTick = Hv_sMax(0, Hv_sMin(20, PlotYMajTick));
  }

  else if (Hv_Contained(key, "minxtick")) {
    sscanf(temp, "%d", &dummy);
    PlotXMinTick = (short)dummy;
    PlotXMinTick = Hv_sMax(0, Hv_sMin(20, PlotXMinTick));
  }
  else if (Hv_Contained(key, "minytick")) {
    sscanf(temp, "%d", &dummy);
    PlotYMinTick = (short)dummy;
    PlotYMinTick = Hv_sMax(0, Hv_sMin(20, PlotYMinTick));
  }

  else if (Hv_Contained(key, "axessty"))
    PlotAxesStyle = AxesStyleFromText(temp);

  else if (Hv_Contained(key, "plottype")) {
    PlotPlotType = PlotTypeFromText(temp);

/* this hack effectively makes the default precisions
  zero (instead of 1) for histograms */

    if ((PlotPlotType == Hv_1DHISTO) || (PlotPlotType == Hv_2DHISTO)) {
      if (!readxprecision)
	DefaultPlotData->xprecision = 0;
      if (!readyprecision)
	DefaultPlotData->yprecision = 0;
    }
  }
  else if (Hv_Contained(key, "axescol"))
    PlotAxesColor = Hv_ColorFromText(temp);

  else if (Hv_Contained(key, "majgridcol"))
    PlotMajGridColor = Hv_ColorFromText(temp);

  else if (Hv_Contained(key, "mingridcol"))
    PlotMinGridColor = Hv_ColorFromText(temp);

  else if (Hv_Contained(key, "xtietag")) {
    sscanf(temp, "%d", &dummy);
    PlotXTieTag = (short)dummy;
  }

  else if (Hv_Contained(key, "xprecis")) {
    sscanf(temp, "%d", &dummy);
    readxprecision = True;
    DefaultPlotData->xprecision = (short)dummy;
    DefaultPlotData->xprecision = Hv_sMax(0, Hv_sMin(6, DefaultPlotData->xprecision));
  }

  else if (Hv_Contained(key, "yprecis")) {
    sscanf(temp, "%d", &dummy);
    readyprecision = True;
    DefaultPlotData->yprecision = (short)dummy;
    DefaultPlotData->yprecision = Hv_sMax(0, Hv_sMin(6, DefaultPlotData->yprecision));
  }

  else if (Hv_Contained(key, "useback"))
    PlotUseBackground = BooleanFromText(temp);

  else if (Hv_Contained(key, "ynozoom"))
    PlotYNoZoom = BooleanFromText(temp);
  else if (Hv_Contained(key, "xnozoom"))
    PlotXNoZoom = BooleanFromText(temp);

  else if (Hv_Contained(key, "showback"))
    PlotShowBackground = BooleanFromText(temp);
  else if (Hv_Contained(key, "filename"))
    PlotShowFileName = BooleanFromText(temp);
  else if (Hv_Contained(key, "background"))
    sscanf(temp, "%f", &(DefaultPlotData->background));

  else if (Hv_Contained(key, "scilo"))
    sscanf(temp, "%f", &PlotSciLowLim);

  else if (Hv_Contained(key, "scihi"))
    sscanf(temp, "%f", &PlotSciHiLim);

  else if (Hv_Contained(key, "left")) {
    sscanf(temp, "%f", &fdummy);
    units = UnitsFromText(temp);
    PlotRelX = AdjustValue(fdummy, units);
  }

  else if (Hv_Contained(key, "top")) {
    sscanf(temp, "%f", &fdummy);
    units = UnitsFromText(temp);
    PlotRelY = AdjustValue(fdummy, units);
  }

  else if (Hv_Contained(key, "width")) {
    sscanf(temp, "%f", &fdummy);
    units = UnitsFromText(temp);
    PlotW = AdjustValue(fdummy, units);
  }
  else if (Hv_Contained(key, "height")) {
    sscanf(temp, "%f", &fdummy);
    units = UnitsFromText(temp);
    PlotH = AdjustValue(fdummy, units);
  }

/* user provided plot limits ? */

  else if (Hv_Contained(key, "xmin"))
    sscanf(temp, "%f", &PlotXmin);

  else if (Hv_Contained(key, "xmax"))
    sscanf(temp, "%f", &PlotXmax);

  else if (Hv_Contained(key, "ymin"))
    sscanf(temp, "%f", &PlotYmin);

  else if (Hv_Contained(key, "ymax"))
    sscanf(temp, "%f", &PlotYmax);

/* see if it is a CURVE stle line instead of a PLOT style line */

  else if (Hv_Contained(key, "curve")) {

/* get the curve number */

    segment = Hv_NextTextSegment(&temp);
    sscanf(segment, "%d", &cnum);
    Hv_Free(segment);

    if ((cnum > 0) && (cnum < MAXSETS)) {
      Hv_InitCharStr(&line2, temp);
      ParseCurveStyleLine(line2, cnum);
      Hv_Free(line2);
    }
  }

/* give up */

  else
    BadLineMessage(line);

  Hv_Free(key);
}


/*----- ParseCurveStyleLine -------*/

static void ParseCurveStyleLine(char *line,
				int  cnum)

/* line should have "style curve N" removed; next seg should be key 
   cnum is not a c index */ 

{
  char   *key;
  char   *temp;
  int    dummy;
  float  fdummy;
  int    cindex;

  temp = line;
  cindex = cnum - 1;

  key = Hv_NextTextSegment(&temp);

  if (Hv_Contained(key, "linecol"))
    CurveLineColor[cindex] = Hv_ColorFromText(temp);

  else if (Hv_Contained(key, "fittype"))
    CurveFitType[cindex] = FitTypeFromText(temp);

  else if (Hv_Contained(key, "thick")) {
    sscanf(temp, "%d", &dummy);
    CurveThickness[cindex] = (short)dummy;
  }

  else if (Hv_Contained(key, "xscale")) {
    sscanf(temp, "%f", &fdummy);
    CurveXScale[cindex] = fdummy;
  }

  else if (Hv_Contained(key, "yscale")) {
    sscanf(temp, "%f", &fdummy);
    CurveYScale[cindex] = fdummy;
  }
 
  else if (Hv_Contained(key, "polyord")) {
    sscanf(temp, "%d", &dummy);
    CurvePolyOrder[cindex] = dummy;
  }

  else if (Hv_Contained(key, "legendreord")) {
    sscanf(temp, "%d", &dummy);
    CurveLegendreOrder[cindex] = dummy;
  }
 
  else if (Hv_Contained(key, "numgauss")) {
    sscanf(temp, "%d", &dummy);
    CurveNumGauss[cindex] = dummy;
  }

  else if (Hv_Contained(key, "name")) {
    Hv_Free(CurveName[cindex]);
    RestOfString(&temp);
    Hv_InitCharStr(&(CurveName[cindex]), temp);
  }

  else if (Hv_Contained(key, "style"))
    CurveLineStyle[cindex] = LineStyleFromText(temp);

  else if (Hv_Contained(key, "display"))
    CurveDisplay[cindex] = BooleanFromText(temp);

  else if (Hv_Contained(key, "yspike"))
    CurveYSpike[cindex] = BooleanFromText(temp);

  else if (Hv_Contained(key, "absval"))
    CurveAbsVal[cindex] = BooleanFromText(temp);

  else if (Hv_Contained(key, "legend"))
    CurveOnLegend[cindex] = BooleanFromText(temp);

  else if (Hv_Contained(key, "usexerrs"))
    CurveUseXErrs[cindex] = BooleanFromText(temp);

  else if (Hv_Contained(key, "useyerrs"))
    CurveUseYErrs[cindex] = BooleanFromText(temp);

  else if ((Hv_Contained(key, "algor")) || (Hv_Contained(key, "connect")))
    CurveAlgorithm[cindex] = AlgorithmFromText(temp);

  else if (Hv_Contained(key, "symbolcolor"))
    CurveSymbolColor[cindex] = Hv_ColorFromText(temp);

  else if (Hv_Contained(key, "numbin2")){
    sscanf(temp, "%d", &dummy);
    CurveNumBin2[cindex] = dummy;
  }

  else if (Hv_Contained(key, "numbin")) {
    sscanf(temp, "%d", &dummy);
    CurveNumBin1[cindex] = dummy;
  }

  else if (Hv_Contained(key, "symbolsize")) {
    sscanf(temp, "%d", &dummy);
    CurveSymbolSize[cindex] = (short)dummy;
  }

  else if (Hv_Contained(key, "symbol"))
    CurveSymbol[cindex] = Hv_SymbolFromText(temp);

  else if (Hv_Contained(key, "pattern"))
    CurvePattern[cindex] = Hv_PatternFromText(temp);


/* give up */

  else
    BadLineMessage(line);

  Hv_Free(key);
}



/*------- FitTypeFromText ----*/

static  int  FitTypeFromText(char *line)

{
  if (Hv_Contained(line, "line"))
    return Hv_LINEFIT;
  else if (Hv_Contained(line, "expon"))
    return Hv_EXPONENTIALFIT;
  else if (Hv_Contained(line, "gauss"))
    return Hv_GAUSSIANFIT;
  else if (Hv_Contained(line, "legendre"))
    return Hv_LEGENDREFIT;
  else
    return Hv_POLYFIT;
}


/*------- AlgorithmFromText ----*/

static   short   AlgorithmFromText(char *line)

{
  if (Hv_Contained(line, "none")) 
    return Hv_NOLINES;
  if (Hv_Contained(line, "no line")) 
    return Hv_NOLINES;
  if (Hv_Contained(line, "noline")) 
    return Hv_NOLINES;
  else if (Hv_Contained(line, "stair"))
    return Hv_STAIRCONNECT;

/* next line is for backward compatibility */

  else if (Hv_Contained(line, "histo"))
    return Hv_STAIRCONNECT;

  else if (Hv_Contained(line, "spline"))
    return Hv_SPLINE;
  else if (Hv_Contained(line, "fit"))
    return Hv_CURVEFIT;
  else if (Hv_Contained(line, "hbar"))
    return Hv_HBAR;
  else if (Hv_Contained(line, "vbar"))
    return Hv_VBAR;
  else if (Hv_Contained(line, "hstack"))
    return Hv_HSTACK;
  else if (Hv_Contained(line, "vstack"))
    return Hv_VSTACK;
  else
    return Hv_SIMPLECONNECT;
}

/*------- LineStyleFromText ----*/

static   short   LineStyleFromText(char *line)

{
  if (Hv_Contained(line, "dot")) 
    return Hv_DOTDASH;
  else if (Hv_Contained(line, "long")) 
    return Hv_LONGDASHED;
  else if (Hv_Contained(line, "dash")) 
    return Hv_DASHED;
  else
    return Hv_SOLIDLINE;
}


/*------- AxesStyleFromText ----*/

static   short  AxesStyleFromText(char *line)

{
  if (Hv_Contained(line, "two")) 
    return Hv_TWOAXES;
  else if (Hv_Contained(line, "simple")) 
    return Hv_FOURAXESSIMPLE;
  else
    return Hv_FOURAXES;
}


/*------- PlotTypeFromText ----*/

static  int  PlotTypeFromText(char *line)

{
  if (Hv_Contained(line, "hbar")) 
    return Hv_HBAR;
  else if (Hv_Contained(line, "vbar")) 
    return Hv_VBAR;
  else if (Hv_Contained(line, "hstack")) 
    return Hv_HSTACK;
  else if (Hv_Contained(line, "vstack")) 
    return Hv_VSTACK;
  else if (Hv_Contained(line, "2dhisto")) 
    return Hv_2DHISTO;
  else if (Hv_Contained(line, "histo2d")) 
    return Hv_2DHISTO;
  else if (Hv_Contained(line, "histo")) 
    return Hv_1DHISTO;
  else
    return Hv_STANDARDXY;
}


/*------- BooleanFromText ----*/

static  Boolean  BooleanFromText(char *line)

{
  if (Hv_Contained(line, "t")) 
    return True;
  else if (Hv_Contained(line, "on"))
    return True;
  else if (Hv_Contained(line, "y"))
    return True;
  else if (Hv_Contained(line, "1"))
    return True;
  else
    return False;
}


/*------- UnitsFromText ----*/

static   short   UnitsFromText(char *line)

{
  if (Hv_Contained(line, "in")) 
    return INCHUNITS;
  else if (Hv_Contained(line, "mm"))
    return CMUNITS;
  else if (Hv_Contained(line, "cm"))
    return MMUNITS;
  else if (Hv_Contained(line, "wor"))
    return PLOTUNITS;
  else if (Hv_Contained(line, "plot"))
    return PLOTUNITS;
  else if (Hv_Contained(line, "data"))
    return PLOTUNITS;
  else
    return PIXELUNITS;
}


/*------- SetPlotDefaults ---------*/

static void SetPlotDefaults(Hv_View View)

{
  short            stagger;
  short            cnt, row, col, square;
  int              i;
  Hv_PlotLabel     plab, next;
  Hv_Rect          parea;
  Hv_FRect         pworld;
  float            fgap = (float)(0.15);
  float            fsize = (float)(0.115);

/* destroy previous plot labels */

  for(plab = PlotLabels; plab != NULL; plab = next) {
    next = plab->next;
    DestroyPlotLabel(plab);
  }
  PlotLabels = NULL;
  

/* default ordering is X Y Y Y ... */

  BrandNewPlot = True;
  Hv_DestroyPlotData(DefaultPlotData);
  DefaultPlotData = Hv_MallocPlotData();  


  settype[0] = XDATA;
  setdata[0] = NULL;

  for (i = 1; i < MAXSETS; i++) {
    settype[i] = YDATA;
    setdata[i] = NULL;
  }

  for (i = 0; i < MAXSETS; i++) {
    CurveLineColor[i] = Hv_black;
    CurveSymbolColor[i] = Hv_powderBlue;
    CurveThickness[i] = 1;
    CurveXScale[i] = 1.0;
    CurveYScale[i] = 1.0;
    CurveSymbol[i] = Hv_NOSYMBOL;
    CurvePattern[i] = -1;
    CurveSymbolSize[i] = 8;
    CurveAlgorithm[i] = Hv_SIMPLECONNECT;
    CurvePolyOrder[i] = 2;
    CurveLegendreOrder[i] = 5;
    CurveNumGauss[i] = 1;
    CurveFitType[i] = Hv_POLYFIT;
    CurveLineStyle[i] = Hv_SOLIDLINE;
    CurveName[i] = NULL;
    CurveDisplay[i] = True;
    CurveYSpike[i] = False;
    CurveAbsVal[i] = False;
    CurveOnLegend[i] = True;
    CurveUseXErrs[i] = True;
    CurveUseYErrs[i] = True;
  }

  PlotXmin = (float)(-2.0e30);
  PlotXmax = (float)(2.0e30);
  PlotYmin = (float)(-2.0e30);
  PlotYmax = (float)(2.0e30);

  PlotYNoZoom = False;
  PlotXNoZoom = False;

  PlotAttachLegend = False;
  PlotAttachTitle = False;
  PlotAttachXlab = False;
  PlotAttachYlab = False;

  PlotUseBackground = True;
  PlotShowBackground = False;
  PlotShowFileName = False;

  PlotLegendLeft = (float)(2.0e30);
  PlotLegendTop =  (float)(2.0e30);

  PlotLegendFont = Hv_helvetica14;
  PlotFitResultFont = Hv_helvetica11;
  PlotAxesFont = Hv_helvetica14;
  PlotTitleFont = Hv_helvetica20;
  PlotXLabelFont = Hv_helvetica17;
  PlotYLabelFont = Hv_helvetica17;
  PlotTitleColor = Hv_black;
  PlotXLabelColor = Hv_black;
  PlotYLabelColor = Hv_black;

  PlotXLog = False;
  PlotYLog = False;

  PlotFancySymbols = False;
  PlotXZeroLine = False;
  PlotYZeroLine = False;

  PlotDrawXAxisValues = True;
  PlotDrawYAxisValues = True;

  PlotErrorBarTops = False;
  PlotDrawMinX = True;
  PlotDrawMinY = True;

  PlotTitleLeft  = (float)(2.0e30);
  PlotTitleTop   = (float)(2.0e30);
  PlotXlabelLeft = (float)(2.0e30);
  PlotXlabelTop  = (float)(2.0e30);
  PlotYlabelLeft = (float)(2.0e30);
  PlotYlabelTop  = (float)(2.0e30);

  PlotMajXGrid = False;
  PlotMajYGrid = False;
  PlotMinXGrid = False;
  PlotMinYGrid = False;

  PlotUseMajXTick = True;
  PlotUseMajYTick = True;
  PlotUseMinXTick = True;
  PlotUseMinYTick = True;

  PlotXMajTick = 4;
  PlotYMajTick = 4;
  PlotXMinTick = 0;
  PlotYMinTick = 0;

  PlotSciLowLim = (float)(1.0e-2);
  PlotSciHiLim  = (float)(1.0e5);

  PlotXTieTag      = 0;
  PlotAxesStyle    = Hv_FOURAXESSIMPLE;
  PlotPlotType     = Hv_STANDARDXY;
  PlotAxesColor    = Hv_black;
  PlotMajGridColor = Hv_gray7;
  PlotMinGridColor = Hv_gray12;

  PlotNumSets   = 0;
  PlotNumCurves = 0;
  PlotNumXSets  = 0;
  PlotNumCSets  = 0;
  PlotLastX = NULL;
  PlotLastY = NULL;
  PlotLastV = NULL;
  PlotLastH = NULL;

/* now work on relative position -- assume
   a tiling as the default */

  cnt = (short)(Hv_CountPlots(View));

  stagger = cnt/64;
  cnt = cnt % 64;

  square = cnt / 4;
  row = (2*(square % 4)) + ((cnt % 4) / 2);
  col =  (2*(square / 4)) + (cnt % 2);

  fgap *= fsize;

  Hv_SetFRect(&pworld, 
		col*fsize + fgap,
		(col+1)*fsize - (float)(0.4*fgap),
		(float)(1.0 - (row+1)*fsize + (float)(1.8*fgap)),
		(float)(1.0 - row*fsize - (float)(0.3*fgap)));

  Hv_WorldRectToLocalRect(View, &parea, &pworld);

  PlotW  = parea.width;
  PlotH  = parea.height;
  PlotRelX = parea.left + 50*stagger;
  PlotRelY = parea.top  + 50*stagger;
}


/*------- StufPlot ------*/


static void StuffPlot(Hv_Item  plot,
		      int    curveID,
		      int    npts)

/*     curveID: NOT a C Index*/

/* stuffing the i'th curve with the following algorithm: 

 1)   the Y data is the ith Y column read in.
 2)   the X data is the last X data BEFORE this column, or, if NONE BEFORE, the first
      X data AFTER this column
 3)   H (Xerror) data between the X for this column and the next x (or end)
 4)   V (Yerror) data between this column and the next y (or end)
 5)   the C (str) data is the last C data BEFORE this column, or, if NONE BEFORE, the first
      c data AFTER this column

*/

{
  Hv_PlotData    plotdata;
  Hv_CurveData   curve;
  short          i, k;
  short          ycol;
  short          xcol;
  short          hcol;
  short          vcol;
  short          ccol;
  short          curvecount;
  size_t         size;
  int            cindex;
  char           **strarry;

  if (plot == NULL)
    return;

  plotdata = Hv_GetPlotData(plot);

  cindex = curveID - 1;

/* find the right Y */

  curvecount = 0;
  ycol = -1;

  for (i = 0; i < PlotNumSets; i++) {
    if (settype[i] == YDATA)
      curvecount++;
    if (curvecount == curveID) {
      ycol = i;
      break;
    }
  }

  if (ycol < 0) {
    Hv_Println("Error in finding y data for curve %d", curveID);
    return;
  }


/* find the right X; note that We Would NOT reach here if there are NO X columns */

  xcol = -1;

/* find the last x before this y */

  for (i = 0; i < ycol; i++) {
    if (settype[i] == XDATA)
      xcol = i;
  }

/* if no x before, search for the next x */
  
  if (xcol < 0)
    for (i = (ycol+1); i < PlotNumSets; i++) {
      if (settype[i] == XDATA) {
	xcol = i;
	break;
      }
    }

/* search for error bars as per algorithm */

  hcol = -1;

/* find the first h after x but before next x  */

  for (i = (xcol+1); i < PlotNumSets; i++) {
    if (settype[i] == XERRDATA) {
      hcol = i;
      break;
    }
    if (settype[i] == XDATA)
      break;
  }


  vcol = -1;


/* find the first v after y but before next y  */

  for (i = (ycol+1); i < PlotNumSets; i++) {
    if (settype[i] == YERRDATA) {
      vcol = i;
      break;
    }
    if (settype[i] == YDATA)
      break;
  }


/* find the right C (str) column */

  ccol = -1;

/* find the last c before this y */

  for (i = 0; i < ycol; i++) {
    if (settype[i] == STRDATA)
      ccol = i;
  }

  
  if (ccol < 0)
    for (i = (ycol+1); i < PlotNumSets; i++) {
      if (settype[i] == STRDATA) {
	ccol = i;
	break;
      }
    }


  if ((xcol < 0) && (ccol < 0)) {
    Hv_Println("Error in finding x data for curve %d (ycol = %d)", curveID, ycol+1);
    return;
  }

/* if we made it here, we are ready to roll */

  size = (size_t)(npts*sizeof(float));

  curve = Hv_MallocCurveData(NULL);
  Hv_InsertListItem((Hv_ListPtr *)(&(plotdata->curves)), (Hv_ListPtr)curve);

  curve->npts = npts;

  curve->x = (float *)Hv_Malloc(size);
  curve->y = (float *)Hv_Malloc(size);

  if (xcol >= 0)
    memcpy(curve->x, (float *)setdata[xcol], size);
  else
    for (i = 0; i < npts; i++)
      curve->x[i] = (float)i;

  if (ycol >= 0)
    memcpy(curve->y, (float *)setdata[ycol], size);
  else
    for (i = 0; i < npts; i++)
      curve->y[i] = (float)i;


/* string data gets special treatment */

  if (ccol >= 0) {
    curve->categories = (char **)Hv_Malloc(npts*sizeof(char *));
    strarry = (char **)setdata[ccol];
    for (k = 0; k < npts; k++) {
      Hv_InitCharStr(&(curve->categories[k]), strarry[k]);
    }
  }

  if (hcol >= 0) {
    curve->xerr = (float *)Hv_Malloc(size);
    memcpy(curve->xerr, (float *)setdata[hcol], size);
  }

  if (vcol >= 0) {
    curve->yerr = (float *)Hv_Malloc(size);
    memcpy(curve->yerr, (float *)setdata[vcol], size);
  }

/* now stuff the styles */

  StuffCurveStyles(curve, cindex);

}


/*------- Stuff1DHisto ------*/


static void Stuff1DHisto(Hv_Item  plot,
		      int         curveID,
		      int         npts)

/*     curveID: NOT a C Index*/


{
  Hv_PlotData    plotdata;
  Hv_CurveData   curve;
  short          i;
  short          n1col;
  short          curvecount;
  size_t         size;
  int            cindex;

  if (plot == NULL)
    return;

  plotdata = Hv_GetPlotData(plot);

  cindex = curveID - 1;


/* find the right Y */

  curvecount = 0;
  n1col = -1;

  for (i = 0; i < PlotNumSets; i++) {
    if (settype[i] == N1DATA)
      curvecount++;
    if (curvecount == curveID) {
      n1col = i;
      break;
    }
  }

  if (n1col < 0) {
    Hv_Println("Error in finding histo data for curve %d", curveID);
    return;
  }

  size = (size_t)(npts*sizeof(float));

  curve = Hv_MallocCurveData(NULL);
  Hv_InsertListItem((Hv_ListPtr *)(&(plotdata->curves)), (Hv_ListPtr)curve);

  curve->npts = npts;

  curve->n1 = (float *)Hv_Malloc(size);

  if (n1col >= 0) {
    memcpy(curve->n1, (float *)setdata[n1col], size);
    Hv_Println("Allocated 1D histo array with %d points", npts);
  }
  else {
    Hv_Println("Serious problems in reading histo data. Trust nothing.");
    for (i = 0; i < npts; i++)
      curve->n1[i] = 0.0;
  }

/* now stuff the styles */

  StuffCurveStyles(curve, cindex); 

  Hv_Println("Exiting Stuff1DHisto");
}

/*------- RestOfString ----*/

static   void   RestOfString(char **s)

{
  Hv_RemoveLeadingBlanks(s);
  Hv_ReplaceNewLine(*s);
}

/*----------- PlotCopyMode ------*/

static int PlotCopyMode(int numplots)

{
  static Hv_Widget        dialog = NULL;
  static Hv_Widget        dummy, rowcol, prompt;
  static Hv_RadioHeadPtr  modelist = NULL;
  char   text[60];

  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " read mode ", NULL);
    rowcol = Hv_DialogColumn(dialog, 8);
    
    prompt = Hv_StandardLabel(rowcol, "What about the XX existing plots?", 0);


    dummy = Hv_VaCreateDialogItem(rowcol,
				  Hv_TYPE,         Hv_RADIODIALOGITEM,
				  Hv_RADIOLIST,    &modelist,
				  Hv_FONT,         Hv_fixed2,
				  Hv_OPTION,       (char *)"Append",
				  Hv_OPTION,       (char *)"Delete",
				  Hv_NOWON,        1,   /* sets append */
				  NULL);

/* close out with just an ok button */

    dummy = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON + Hv_CANCELBUTTON);
  }
  
  if (numplots == 1)
    sprintf(text, "What about the existing plot?");
  else 
    sprintf(text, "What about the %-2d existing plots?", numplots);
  Hv_ChangeLabel(prompt, text, Hv_fixed2);

  
  Hv_PosSetActiveRadioButton(1, modelist);
  if (Hv_DoDialog(dialog, NULL) == Hv_OK)
    return  Hv_GetActiveRadioButtonPos(modelist);
  else
    return Hv_PLOTCANCEL;
}

/*------ MallocPlotLabel ------*/

static  void MallocPlotLabel(float  x,
			     float  y,
			     short  font,
			     short  color,
			     char   *str,
			     Boolean horizontal)
{
  Hv_PlotLabel    plab;

  plab = (Hv_PlotLabel)Hv_Malloc(sizeof(Hv_PlotLabelRec));

  plab->horizontal = horizontal;
  plab->x = x;
  plab->y = y;
  plab->font = font;
  plab->color = color;
  Hv_InitCharStr(&(plab->text), str);

  Hv_InsertListItem((Hv_ListPtr *)(&(PlotLabels)), (Hv_ListPtr)plab);
}

/*------- DestroyPlotLabel ------*/

static   void   DestroyPlotLabel(Hv_PlotLabel plab)

{
  Hv_Free(plab->text);
  Hv_Free(plab);
}

/*----- AdjustValue -----*/

static short AdjustValue(float  val,
			 short  units)
{
  float scres;
  float dummy;
  float fudge = (float)(1.05);
  
  dummy = val;
  scres = fudge*(float)Hv_screenResolution;

  switch (units) {
  case INCHUNITS:
    dummy *= scres; 
    break;
    
  case CMUNITS:
    dummy = (float)(dummy*scres/2.54);
    break;
    
  case MMUNITS:
    dummy = (float)(dummy*scres/25.4);
    break;
  }
  
  return  (short)dummy;
}

/*------ BadLineMessage ----------*/

static   void  BadLineMessage(char *line)

{
  char   *message;

  message = Hv_Malloc(strlen(line) + 20);
  strcpy(message, "BAD LINE: ");
  strcat(message, line);
  Hv_Warning(message);
  Hv_Free(message);
}

/*----- MallocDataSets ----------*/

static int MallocDataSets(int    ndatalines,
			  Boolean  *goodread,
			  char    *line)
{
  int    i, k;
  char   **strarry;

  if (PlotNumSets < 1)
    Hv_Println("No STYLE ORDER line read. Will assume X Y Y Y ... ");

/* of NumPlotSets < 1, must use first line to figure the number of sets */
	    
  if (PlotNumSets < 1) {
    PlotNumSets = Hv_CountSegments(line);
    PlotNumCurves = PlotNumSets - 1;
    PlotNumXSets = 1;  /* assumed as the first column */
  }

  PlotNumSets = Hv_iMin(MAXSETS, PlotNumSets);

/* dont read a solitary column */

  if (PlotPlotType == Hv_1DHISTO) {
    Hv_Println("Will malloc %d  1D histo data sets", PlotNumSets);
    if (PlotNumSets < 1)
      return 1;
  }
  
  else {
    if ((PlotNumSets < 2) ||
	(PlotNumCurves < 1) ||
	((PlotNumXSets < 1) && (PlotNumCSets < 1))) {
      Hv_Println("No point to read in %d data sets and %d curves and %d X sets.",
		 PlotNumSets, PlotNumCurves, PlotNumXSets);
      *goodread = False;
      return 1;
    }
  }

/* now malloc the data sets. For most, malloc an array of floats.
   for STRDATA, mallock an array of string pointers  */

  for (i = 0; i < PlotNumSets; i++) {
    if (settype[i] == STRDATA) {
      setdata[i] = (void *)Hv_Malloc(ndatalines * sizeof(char *));
      strarry = (char **)setdata[i];

/* initailize all strings in the array to NULL */

      for (k = 0; k < ndatalines; k++)
	strarry[k] = NULL;
    }
    else
      setdata[i] = (void *)Hv_Malloc(ndatalines * sizeof(float));
  }

  return 0;
}

/*--------- MallocCurveArrays -----------*/

static void    MallocCurveArrays(void)

{
  int    i;

  CurveLineStyle   = (short *)Hv_Malloc(MAXSETS*sizeof(short));
  CurveLineColor   = (short *)Hv_Malloc(MAXSETS*sizeof(short));
  CurveSymbolColor = (short *)Hv_Malloc(MAXSETS*sizeof(short));
  CurveSymbolSize  = (short *)Hv_Malloc(MAXSETS*sizeof(short));
  CurveThickness   = (short *)Hv_Malloc(MAXSETS*sizeof(short));
  CurveSymbol      = (short *)Hv_Malloc(MAXSETS*sizeof(short));
  CurvePattern     = (short *)Hv_Malloc(MAXSETS*sizeof(short));
  CurveAlgorithm   = (short *)Hv_Malloc(MAXSETS*sizeof(short));

  CurveDisplay = (Boolean *)Hv_Malloc(MAXSETS*sizeof(Boolean));
  CurveOnLegend = (Boolean *)Hv_Malloc(MAXSETS*sizeof(Boolean));
  CurveUseXErrs = (Boolean *)Hv_Malloc(MAXSETS*sizeof(Boolean));
  CurveUseYErrs = (Boolean *)Hv_Malloc(MAXSETS*sizeof(Boolean));
  CurveYSpike = (Boolean *)Hv_Malloc(MAXSETS*sizeof(Boolean));
  CurveAbsVal = (Boolean *)Hv_Malloc(MAXSETS*sizeof(Boolean));

  CurveName = (char **)Hv_Malloc(MAXSETS*sizeof(char *));
  for (i = 0; i < MAXSETS; i++)
    CurveName[i] = NULL;

  CurveXScale = (float *)Hv_Malloc(MAXSETS*sizeof(float));
  CurveYScale = (float *)Hv_Malloc(MAXSETS*sizeof(float));
  CurveNumBin1 = (int *)Hv_Malloc(MAXSETS*sizeof(int));
  CurveNumBin2 = (int *)Hv_Malloc(MAXSETS*sizeof(int));

  CurvePolyOrder = (int *)Hv_Malloc(MAXSETS*sizeof(int));
  CurveLegendreOrder = (int *)Hv_Malloc(MAXSETS*sizeof(int));
  CurveNumGauss = (int *)Hv_Malloc(MAXSETS*sizeof(int));
  CurveFitType = (int *)Hv_Malloc(MAXSETS*sizeof(int));
}

/*---------- FreeCurveArrays ----------------*/

static void    FreeCurveArrays(void)

{
  int i;

  Hv_Free(CurveLineStyle);
  Hv_Free(CurveLineColor);
  Hv_Free(CurveSymbolColor);
  Hv_Free(CurveSymbolSize);
  Hv_Free(CurveThickness);

  if (CurveName)
    for (i = 0; i < MAXSETS; i++)
      Hv_Free(CurveName[i]);

  Hv_Free(CurveName);
  Hv_Free(CurveSymbol);
  Hv_Free(CurvePattern);
  Hv_Free(CurveAlgorithm);
  Hv_Free(CurveXScale);
  Hv_Free(CurveYScale);
  Hv_Free(CurveDisplay);
  Hv_Free(CurveOnLegend);
  Hv_Free(CurveYSpike);
  Hv_Free(CurveAbsVal);
  Hv_Free(CurveUseXErrs);
  Hv_Free(CurveUseYErrs);
  Hv_Free(CurveNumBin1);
  Hv_Free(CurveNumBin2);
  Hv_Free(CurvePolyOrder);
  Hv_Free(CurveLegendreOrder);
  Hv_Free(CurveNumGauss);
  Hv_Free(CurveFitType);
}


/*------- StuffCurveStyles --------*/

static void StuffCurveStyles(Hv_CurveData curve,
			     int          cindex)

{
  curve->thickness   = Hv_sMax(1, Hv_sMin(6, CurveThickness[cindex])); 
  curve->symbolsize  = Hv_sMax(3, Hv_sMin(12, CurveSymbolSize[cindex])); 
  curve->algorithm   = CurveAlgorithm[cindex]; 
  curve->style       = CurveLineStyle[cindex]; 
  curve->xscale      = CurveXScale[cindex];
  curve->yscale      = CurveYScale[cindex];
  curve->symbol      = CurveSymbol[cindex]; 
  curve->pattern     = CurvePattern[cindex]; 
  curve->linecolor   = CurveLineColor[cindex]; 
  curve->symbolcolor = CurveSymbolColor[cindex];
  curve->display     = CurveDisplay[cindex];
  curve->yspike      = CurveYSpike[cindex];
  curve->absval      = CurveAbsVal[cindex];
  curve->onlegend    = CurveOnLegend[cindex];
  curve->usexerrs    = CurveUseXErrs[cindex];
  curve->useyerrs    = CurveUseYErrs[cindex];
  curve->fittype     = CurveFitType[cindex];

  curve->numbin1 = Hv_iMax(2, Hv_iMin(1000, CurveNumBin1[cindex]));
  curve->numbin2 = Hv_iMax(2, Hv_iMin(1000, CurveNumBin2[cindex]));

  curve->polyorder  = Hv_iMax(2, Hv_iMin(10, CurvePolyOrder[cindex]));
  curve->polyorder  = Hv_iMin(curve->npts-1, curve->polyorder);

  curve->legendreorder  = Hv_iMax(2, Hv_iMin(10, CurveLegendreOrder[cindex]));
  curve->legendreorder  = Hv_iMin(curve->npts-1, curve->legendreorder);

  curve->numgauss   = Hv_iMax(1, Hv_iMin(10, CurveNumGauss[cindex]));
  
  if (CurveName[cindex] != NULL) {
    Hv_Free(curve->name);
    Hv_InitCharStr(&(curve->name), CurveName[cindex]);
    Hv_Free(CurveName[cindex]);
    CurveName[cindex] = NULL;
  }

}



/**
 * AddASpace
 * @purpose Hack to prevent some inadvertant clipping esp on windows
 * @param  s   Original string
 * @return string with space added
 */

static char *AddSpace(char *s) {
	char *t;
	int  len;

	if (s == NULL)
		return NULL;

	len = strlen(s);

	if (len < 1) {
		Hv_InitCharStr(&t, " ");
	}
	else {
		t = (char *)Hv_Malloc(len + 5);
		sprintf(t, "%s ", s);
	}

	return t;
}


#undef   MAXSETS
#undef   XDATA     
#undef   YDATA     
#undef   XERRDATA  
#undef   YERRDATA  
#undef   DUMMYDATA
#undef   STRDATA

#undef   Hv_PLOTAPPEND
#undef   Hv_PLOTOVERWRITE
#undef   Hv_PLOTCANCEL











