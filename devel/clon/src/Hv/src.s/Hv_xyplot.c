/**
 * <EM>Routines dealing with the HvPlot views.</EM>
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
#include "Hv_drag.h"
#include "Hv_xyplot.h"
#include "Hv_linklist.h"

#define  Hv_BIGBUTTONWIDTH       44
#define  Hv_BIGBUTTONHEIGHT      24
#define  LEGENDITEM             -10001
#define  FITRESULTITEM          -10002


typedef float     (*Hv_FloatFunction)(float);      /*pointer to function returning a float */


Hv_Item           Hv_hotPlot;
extern  Boolean   Hv_nrerror;   /* signifies a numerical recipes error */
char    **Hv_demoFile = NULL;

/* ----- local prototypes ------*/

static void Hv_PlotZoomSelectCallback(Hv_DragData);

static void Hv_DrawFitString(Hv_PlotData plotdata,
			     char *fittext,
			     short x,
			     short y,
			     short *w,
			     short *h,
			     short gap);

static void  InitPlotViewItems(Hv_View View);

static void  DrawXTickItem(Hv_Item  Item,
			   Hv_Region region);

static void  DrawYTickItem(Hv_Item  Item,
			   Hv_Region region);

static float PlotFunction(float x);

static float LineFunction(float x);

static float ExponentialFunction(float x);

static float GaussFunction(float x);

static float PolyFunction(float x);

static float LegendreFunction(float x);

static void nrfgauss(float x,
		     float a[],
		     float *y,
		     float dyda[],
		     int  na);

static void nrfpoly(float  x,
		    float  *p,
		    int   n);

static void nrflegendre(float  x,
			float  *pl,
			int   n);

static void PlotSegment(Hv_Item      plot,
			float       x1,
			float       x2,
			short       color,
			float       bg,
			Hv_FloatFunction f);

static void PSPlotSegment(Hv_Item      plot,
			  float       x1,
			  float       x2,
			  short       color,
			  float       bg,
			  Hv_FloatFunction f);

static void  TurningPoints(float *varray,
			   int  *iarray,
			   int  n);

static void GetWorldBoundingRect(Hv_Item  plot,
				 Hv_FRect  *wbr);

static void Hv_DrawLegendItem(Hv_Item  Item,
			      Hv_Region region);

static void Hv_DrawFitResultItem(Hv_Item  Item,
				 Hv_Region region);

static void Hv_DrawPlotItem(Hv_Item  plot,
			    Hv_Region region);

static Hv_Region Hv_ClipPlotItem(Hv_Item  plot,
				 Hv_Region region);

static void Hv_DrawPlotTicks(Hv_Item  plot,
			     Boolean  drawx,
			     Boolean  drawy);

static void  Hv_XLogTicks(Hv_Item   plot,
			  Hv_Rect   *area);

static void  Hv_YLogTicks(Hv_Item    plot,
			  Hv_Rect   *area);

static void DrawCurve(Hv_Item    plot,
		      Hv_Region   region,
		      Hv_CurveData  curve);

static void TruncName(char  *dname,
		      char  *sname,
		      int  n);

static void  Hv_BasePlotSetup(Hv_View  View);

static void  Hv_PlotSetup(Hv_View  View);

static void  Hv_PlotFeedback(Hv_View       View,
			     Hv_Point      pp);

static Hv_Item  Hv_AddPlotBigButtons(Hv_View   View);

static void FirstTimeDraw(Hv_Item  Item,
			  Hv_Region region);

static void Hv_SaveCB(Hv_Event  hvevent);

static Hv_Item Hv_AddPlotFeedback(Hv_View   View,
				  Hv_Item   placementitem);

static void  Hv_GetCornerRects(Hv_Rect  *area,
			       Hv_Rect  *rects[4],
			       short   slop);

static void  Hv_PlotToPSLocal(Hv_Item  plot,
			      short   *x,
			      short   *y,
			      float   px,
			      float   py);

static void Hv_CheckPlotEnlarge(Hv_Item    Item,
				Hv_Point   StartPP,
				Boolean    ShowCorner,
				Boolean   *enlarged);

static Hv_Item StandardBigButton(Hv_View     View,
				 char      *label,
				 char      *balloon,
				 Hv_FunctionPtr callback);

static float CheckLog10(float x);

static void ReverseOrder(float  *darray,
			 float  *sarray,
			 int   n);

static void PeakPoint(float *x,
		      float *y,
		      int  n,
		      int  *pp);

static short  VerticalLine(Hv_Item  plot,
			   float   xval,
			   short   color);

static short HorizontalLine(Hv_Item  plot,
			    float   yval,
			    short   color);

static void LegendItemEdit(Hv_Event  hvevent);

static void LegendItemCB(Hv_Event  hvevent);

/*------ variables global for this file --------*/

float                  *splinex;
float                  *spliney;
float                  *spliney2;
int                    nspline;
short                  yfitresult;
Hv_CurveData           TheCurve;
extern Boolean         Hv_updateAfterEnlarge;

static Hv_Item         BaseLeft, BaseRight;


/*------- Hv_PlotItemInitialize -----*/

void Hv_PlotItemInitialize(Hv_Item           Item,
			   Hv_AttributeArray attributes)
    
{
    Hv_View      View;
    Hv_PlotData  plotdata;
    Hv_Rect     *hotrect;
    short        relx, rely, w, h, xc, yc, sw, sh, x, y;
    Hv_Item      child;
    char        *fname, *title, *xlab, *ylab;
    Hv_String   *hvstr;
    Hv_FRect     wbr;
    float        xmin, xmax, ymin, ymax;
    
    View = Item->view;
    hotrect = View->hotrect;
    
/* there can be no exceptions for the standard userdraw 
   if the user did supply a userdraw, plug it
   into a special slot for plotsin the plot data below
   (standard hvplots would not use this, only specialized
   apps using base hvplotting */
    
    Item->userdraw = Hv_DrawPlotItem;  /*standard userdraw */
    
    if (attributes[Hv_DOUBLECLICK].v == NULL)
	Item->doubleclick = Hv_EditPlot;
    
    if (attributes[Hv_SINGLECLICK].v == NULL)
	Item->singleclick = Hv_PlotItemCB;
    
    
/* first set the default drawcontrol */
    
    if (attributes[Hv_DRAWCONTROL].i == 0)
	Item->drawcontrol = Hv_DRAGGABLE + Hv_ENLARGEABLE + Hv_ZOOMABLE;
    
/* checkenlarge is probably not provided, but just in case */
    
    if (Item->checkenlarge == NULL)
	Item->checkenlarge = (Hv_FunctionPtr)Hv_CheckPlotEnlarge;
    
/* do we need to create a plot data record? */
    
    if (attributes[Hv_DATA].v == NULL) {
	plotdata =  Hv_MallocPlotData();
	
	plotdata->bgcolor = attributes[Hv_BACKGROUND].s;
	
	xmin = (float)(attributes[Hv_XMIN].f);
	xmax = (float)(attributes[Hv_XMAX].f);
	ymin = (float)(attributes[Hv_YMIN].f);
	ymax = (float)(attributes[Hv_YMAX].f);
	
	if (xmax < xmin) {
	    if (fabs(xmin) < 1.0e-20)
		xmax = 1.0;
	    else
		xmax = xmin + (float)((0.1*fabs(xmin)));
	}
	
	if (ymax < ymin) {
	    if (fabs(ymin) < 1.0e-20)
		ymax = 1.0;
	    else
		ymax = ymin + (float)((0.1*fabs(ymin)));
	}
	
	plotdata->xmin = xmin;
	plotdata->xmax = xmax;
	plotdata->ymin = ymin;
	plotdata->ymax = ymax;
	
/* here is that specialize drawing funct described earlier */
	
	plotdata->UserPlotDraw = (Hv_FunctionPtr)attributes[Hv_USERDRAW].fp;
	
	Hv_SetFRect(plotdata->defworld, xmin, xmax, ymin, ymax);
	
	Item->data = (void *)plotdata;
    }
    else {
	Item->data = attributes[Hv_DATA].v;
	plotdata = (Hv_PlotData)(Item->data);
    }
    
/* now the positioning */
    
    relx = attributes[Hv_LEFT].s;
    rely = attributes[Hv_TOP].s;
    w = attributes[Hv_WIDTH].s;
    h = attributes[Hv_HEIGHT].s;
    
    relx = Hv_sMax(-10000, Hv_sMin(relx, 10000));
    rely = Hv_sMax(-10000, Hv_sMin(rely, 10000));
    w = Hv_sMax(20, Hv_sMin(w, 5000));
    h = Hv_sMax(20, Hv_sMin(h, 5000));
    
    Item->area->left = hotrect->left + relx;
    Item->area->top = hotrect->top + rely;
    Item->area->width = w;
    Item->area->height = h;
    Hv_FixRectRB(Item->area);
    
/* now the children of this plot */
    
    Hv_GetItemCenter(Item, &xc, &yc);
    
/* the tick items  -- which will contain the tick
   marks and the axes values*/
    
    child = Hv_VaCreateItem(View,
			    Hv_TYPE,         Hv_USERITEM,
			    Hv_TAG,          Hv_XTICKTAG,
			    Hv_LEFT,         Item->area->left - 100,
			    Hv_TOP,          Item->area->bottom,
			    Hv_WIDTH,    	   Item->area->width + 160,
			    Hv_HEIGHT,       90,
			    Hv_PARENT,       Item,
			    Hv_DRAWCONTROL,  Hv_NOPOINTERCHECK,
			    Hv_USERDRAW,     DrawXTickItem,
			    NULL);
    plotdata->xtickitem = child;
    
/* y ticks */
    
    child = Hv_VaCreateItem(View,
			    Hv_TYPE,         Hv_USERITEM,
			    Hv_TAG,          Hv_YTICKTAG,
			    Hv_LEFT,         Item->area->left - 160,
			    Hv_TOP,          Item->area->top - 80,
			    Hv_WIDTH,        180,
			    Hv_HEIGHT,       Item->area->height + 120,
			    Hv_PARENT,       Item,
			    Hv_DRAWCONTROL,  Hv_NOPOINTERCHECK,
			    Hv_USERDRAW,     DrawYTickItem,
			    NULL);
    plotdata->ytickitem = child;
    
/* generate an item that holds the filename */
    
    fname = (char *)(attributes[Hv_FILENAME].v);
    
    if (fname != NULL) {
	Hv_StripLeadingDirectory(&fname);
	y = Item->area->bottom - 16;
	x = Item->area->left + 8;
	
	child = Hv_StandardPlotLabel(Item, x, y, fname, Hv_helvetica11);
	plotdata->filenameitem = child;
	
	if (attributes[Hv_SHOW].s == 0)
	    Hv_SetBit(&(child->drawcontrol), Hv_DONTDRAW);
	
    }
    
/* generate the title */
    
    title = (char *)(attributes[Hv_TITLE].v);
    
    if (title != NULL) {
	hvstr = Hv_CreateString(title);
	hvstr->font = Hv_helvetica20;
	Hv_CompoundStringDimension(hvstr, &sw, &sh);
	Hv_DestroyString(hvstr);
	
	y = Item->area->top - sh - 10;
	x = xc - (sw/2);
	
	child = Hv_StandardPlotLabel(Item, x, y, title, Hv_helvetica20);
	child->tag = Hv_XYTITLETAG;
    }
    
/* generate the x axis label */
    
    xlab = (char *)(attributes[Hv_XLABEL].v);
    
    if (xlab != NULL) {
	hvstr = Hv_CreateString(xlab);
	hvstr->font = Hv_helvetica17;
	Hv_CompoundStringDimension(hvstr, &sw, &sh);
	Hv_DestroyString(hvstr);
	
	y = Item->area->bottom + 25;
	x = xc - (sw/2);
	
	child = Hv_StandardPlotLabel(Item, x, y, xlab, Hv_helvetica17);
	child->tag = Hv_XLABELTAG;
    }
    
    
/* now the y axis label */
    
    ylab = (char *)(attributes[Hv_YLABEL].v);
    
    if (ylab != NULL) {
	hvstr = Hv_CreateString(ylab);
	hvstr->font = Hv_helvetica17;
	Hv_CompoundStringDimension(hvstr, &sw, &sh);
	Hv_DestroyString(hvstr);
	
	x = Item->area->left - 55;
	y = yc + (sw/2);
	
	child = Hv_StandardPlotLabel(Item, x, y, ylab, Hv_helvetica17);

	child->tag = Hv_YLABELTAG;
	child->str->horizontal = False;
	Hv_CompoundStringArea(child->str, x, y, child->area, Hv_STRINGEXTRA);
    }

/* for the fit results */

    child = Hv_VaCreateItem(View,
			    Hv_TYPE,         Hv_USERITEM,
			    Hv_LEFT,         Item->area->left+10,
			    Hv_TOP,          Item->area->top+10,
			    Hv_WIDTH,        50,
			    Hv_HEIGHT,       50,
			    Hv_TAG,          FITRESULTITEM,
			    Hv_PARENT,       Item,
			    Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_DRAGGABLE,
			    Hv_USERDRAW,     Hv_DrawFitResultItem,
			    Hv_DOUBLECLICK,  LegendItemEdit,    /*Yes LegendItemEdit*/
			    Hv_SINGLECLICK,  LegendItemCB,    /*Yes LegendItemCB*/
			    NULL);

    
    plotdata->fitresultitem = child;
    
/* finally, a legend */
    
    child = Hv_VaCreateItem(View,
			    Hv_TYPE,         Hv_USERITEM,
			    Hv_LEFT,         Item->area->left+10,
			    Hv_TOP,          Item->area->top+10,
			    Hv_WIDTH,        50,
			    Hv_HEIGHT,       50,
			    Hv_TAG,          LEGENDITEM,
			    Hv_PARENT,       Item,
			    Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_DRAGGABLE,
			    Hv_USERDRAW,     Hv_DrawLegendItem,
			    Hv_SINGLECLICK,  LegendItemCB,
			    Hv_DOUBLECLICK,  LegendItemEdit,
			    NULL);
    
    plotdata->legenditem = child;
    
    if (plotdata->uselegend)
	Hv_ClearBit(&(plotdata->legenditem->drawcontrol), Hv_NOPOINTERCHECK);
    else
	Hv_SetBit(&(plotdata->legenditem->drawcontrol), Hv_NOPOINTERCHECK);
    
    
/***** check if world system needs to be expanded to accomodate new plot */
    
    GetWorldBoundingRect(Item, &wbr);
    View->defaultworld->xmin = Hv_fMin(View->defaultworld->xmin, wbr.xmin);
    View->defaultworld->xmax = Hv_fMax(View->defaultworld->xmax, wbr.xmax);
    View->defaultworld->ymin = Hv_fMin(View->defaultworld->ymin, wbr.ymin);
    View->defaultworld->ymax = Hv_fMax(View->defaultworld->ymax, wbr.ymax);
    Hv_FixFRectWH(View->defaultworld);
    
    Hv_SetScrollActivity(View);
}

/*-------- Hv_BaseXYPlotView ----------------*/

Hv_View         Hv_BaseXYPlotView(char *title,
				  Hv_Item *lefttool,
				  Hv_Item *righttool,
				  short   hrrmargin)
    
/* instead of the standard plot view as used in hvplot, this
   provides a "base" plot view ripe for additional mods.
   It places the tools on the right so that additional
   control items can be added */
    
{
    Hv_View          View;
    static int       count = 0;
    short            left, top, width, height;
    char             *tchr = NULL;
    
    if (hrrmargin < 1)
	hrrmargin = 300;
    
/* offset the top left of each new plot */
    
    left = 10 + 8*(count % 10);
    top = left + 25;
    count++;
    
/* supply a title if none was provided */
    
    if (title == NULL) {
	tchr = (char *)Hv_Malloc(30);
	sprintf(tchr, "Plot %d", count);
    }
    else
	tchr = title;
    
    if (Hv_root_width > 1025) {
	width = 750;
	height = 850;
    }
    else {
	width = 550;
	height = 550;  
    }
    
    Hv_VaCreateView(&View,
		    Hv_TAG,                  Hv_XYPLOTVIEW,
		    Hv_DRAWCONTROL,          Hv_ZOOMONRESIZE |
                                               Hv_ZOOMABLE |
		                               Hv_ENLARGEABLE | 
                                               Hv_SCROLLABLE,
		    Hv_POPUPCONTROL,         Hv_PRINTVIEW | 
                                               Hv_SENDBACKVIEW |
		                               Hv_ZOOMRESIZETOGGLE | 
                                               Hv_REFRESHVIEW | 
                                               Hv_UNDOVIEW,
		    Hv_TITLE,                tchr,
		    Hv_INITIALIZE,           Hv_BasePlotSetup,
		    Hv_LEFT,                 left,
		    Hv_TOP,                  top,
		    Hv_HOTRECTWIDTH,         width,
		    Hv_HOTRECTHEIGHT,        height,
		    Hv_HOTRECTCOLOR,         Hv_white, 
		    Hv_HOTRECTRMARGIN,       hrrmargin,
		    Hv_MOTIONLESSFB,         True,
		    Hv_MOTIONLESSFBINTERVAL, 1000,
		    NULL);
    
    *lefttool  = BaseLeft;
    *righttool = BaseRight;
    return View;
}

/*------- Hv_NewXYPlotView -------*/

Hv_View Hv_NewXYPlotView(char  *title,
			 Boolean autoopen)
    
/* this creates a new view that is tailor made for displaying PlotItems.
   HOWEVER: A PlotItem can appear on ANY VIEW, just like any other item */
    
{
    Hv_View          View;
    static int       count = 0;
    short            left, top, width, height;
    char             *tchr = NULL;
    Hv_Rect          zoomrect;
    
/* offset the top left of each new plot */
    
    left = 12 + 10*(count % 10);
    top = left + 30;
    count++;
    
/* supply a title if none was provided */
    
    if (title == NULL) {
	tchr = (char *)Hv_Malloc(30);
	sprintf(tchr, "Plot %d", count);
    }
    else
	tchr = title;
    
    if (Hv_root_width > 1025) {
	width = 750;
	height = 800;
    }
    else {
	width = 550;
	height = 550;  
    }
    
    Hv_VaCreateView(&View,
		    Hv_TAG,                  Hv_XYPLOTVIEW,
		    Hv_DRAWCONTROL,          Hv_ZOOMABLE | 
		                               Hv_ENLARGEABLE |
		                               Hv_SCROLLABLE,
		    Hv_POPUPCONTROL,         Hv_CUSTOMIZEVIEW |
                                               Hv_ZOOMRESIZETOGGLE |
		                               Hv_DELETEVIEW | 
                                               Hv_PRINTVIEW |
		                               Hv_UNDOVIEW | 
                                               Hv_SENDBACKVIEW |
		                               Hv_REFRESHVIEW,
		    Hv_TITLE,                tchr,
		    Hv_INITIALIZE,           Hv_PlotSetup,
		    Hv_FEEDBACK,             Hv_PlotFeedback,
		    Hv_LEFT,                 left,
		    Hv_TOP,                  top,
		    Hv_HOTRECTWIDTH,         width,
		    Hv_HOTRECTHEIGHT,        height,
		    Hv_HOTRECTCOLOR,         Hv_white, 
		    Hv_HOTRECTTMARGIN,       30,
		    Hv_HOTRECTLMARGIN,       7,
		    Hv_HOTRECTRMARGIN,       7,
		    Hv_HOTRECTBMARGIN,       7,
		    Hv_MOTIONLESSFB,         True,
		    Hv_MOTIONLESSFBINTERVAL, 1000,
		    NULL);
    
    
    Hv_CopyRect(View->containeritem->area, View->local);
    
    if (title == NULL)
	Hv_Free(tchr);
    
    Hv_GetTemporaryWorld(View->defaultworld, View->hotrect, View->world, View->local);
    Hv_CopyRect(&zoomrect, View->hotrect);
    zoomrect.height/=4;
    zoomrect.width/=4;
    zoomrect.top += 1;
    zoomrect.left += 1;
    Hv_FixRectRB(&zoomrect);
    Hv_ZoomView(View, &zoomrect);
    
    Hv_SetScrollActivity(View);
    
    if (autoopen)
	Hv_OpenView(View);
    
    return View;
}


/*------ Hv_UpdateXYPlot --------*/

void Hv_UpdateXYPlot(Hv_Item plot)
    
{
    Hv_Region   preg;
    
    preg = Hv_TotalPlotRegion(plot);
    /*  Hv_DrawView(plot->view, preg); */
    Hv_OffScreenViewUpdate(plot->view, preg);
    Hv_DestroyRegion(preg);
}

/*------ Hv_AddDataSetToXYPlot --------*/

void  Hv_AddDataSetToXYPlot(Hv_Item plot)
    
{
}


/* ------- Hv_MallocPlotItem ------*/

Hv_Item   Hv_MallocPlotItem(Hv_View View)
    
{
    return NULL;
}

/*------- Hv_PointInPlotItem ------*/

Hv_Item Hv_PointInPlotItem(Hv_View  View,
			   Hv_Point  pp)
    
/* Note: Works ONLY for Plot Views. Returns the
   first plot item in the views list that contains
   the point x,y. */
    
{
    if (View->tag != Hv_XYPLOTVIEW)
	return NULL;
    
    return  Hv_PointInItemMatch(View->items, Hv_PLOTITEM, 0, Hv_INSIDEHOTRECT, pp);
}


/*------- Hv_ReasonablePlotLimits -------*/

void   Hv_ReasonablePlotLimits(Hv_PlotData plotdata)
    
{
    Hv_CurveData      tcurve;
    float             xmax, xmin, ymax, ymin;
    
    xmin = (float)(1.0e30);
    xmax = (float)(-1.0e30);
    ymin = (float)(1.0e30);
    ymax = (float)(-1.0e30);
    
    if ( (plotdata->plottype == Hv_HBAR) ||
	 (plotdata->plottype == Hv_VBAR) ||
	 (plotdata->plottype == Hv_HSTACK) ||
	 (plotdata->plottype == Hv_VSTACK)) {
	Hv_ReasonableBarPlotLimits(plotdata);
	return;
    }
    
    else if ((plotdata->plottype == Hv_1DHISTO)  || (plotdata->plottype == Hv_2DHISTO)) {
	Hv_ReasonableHistoPlotLimits(plotdata);
	return;
    }
    
    for (tcurve = plotdata->curves; tcurve != NULL; tcurve = tcurve->next) {
	
	Hv_PlotDataMinMax(tcurve->x,
			  tcurve->npts,
			  tcurve->xscale,
			  tcurve->absval,
			  &(tcurve->xmin),
			  &(tcurve->xmax));

	Hv_PlotDataMinMax(tcurve->y,
			  tcurve->npts,
			  tcurve->yscale,
			  tcurve->absval,
			  &(tcurve->ymin),
			  &(tcurve->ymax));
	
	xmin = Hv_fMin(xmin, tcurve->xmin);
	xmax = Hv_fMax(xmax, tcurve->xmax);
	ymin = Hv_fMin(ymin, tcurve->ymin);
	ymax = Hv_fMax(ymax, tcurve->ymax);
    }
    
    if (xmin >= 0.0) {
	if (plotdata->xlog)
	    xmin = (float)(0.95*xmin);
	else
	    xmin = 0.0;
	xmax = Hv_ConvertToANiceValue(xmax);
	
    }
    else if (xmax < 1.01e-25) {
	if (plotdata->xlog)
	    xmax = (float)(0.95*xmax);
	else
	    xmax = 0.0;
	xmin = Hv_ConvertToANiceValue(xmin);
    }
    else { /* one neg, one pos */
	xmax = Hv_ConvertToANiceValue(xmax);
	xmin = Hv_ConvertToANiceValue(xmin);
	if (plotdata->xlog)
	    xmin = (float)(1.0e-8*xmax);
    }
    
    
    if (ymin >= 0.0) {
	if (plotdata->ylog)
	    ymin = (float)(0.95*ymin);
	else
	    ymin = 0.0;
	ymax = Hv_ConvertToANiceValue(Hv_fMax((float)(1.0e-20), ymax));
    }
    else if (ymax < 1.01e-25) {
	if (plotdata->ylog)
	    ymax = (float)(0.95*ymax);
	else
	    ymax = 0.0;
	ymin = Hv_ConvertToANiceValue(ymin);
    }
    else { /* one neg, one pos */
	ymax = Hv_ConvertToANiceValue(ymax);
	ymin = Hv_ConvertToANiceValue(ymin);
	if (plotdata->ylog)
	    ymin = (float)(1.0e-8*ymax);
    }
    
    plotdata->xmin = xmin;
    plotdata->xmax = xmax;
    plotdata->ymin = ymin;
    plotdata->ymax = ymax;
}


/*------- Hv_ReasonableYPlotLimits -------*/

void   Hv_ReasonableYPlotLimits(Hv_PlotData plotdata)
    
{
    Hv_CurveData      tcurve;
    float             ymax, ymin;
    
    ymin = (float)(1.0e30);
    ymax = (float)(-1.0e30);
    
    if (plotdata->plottype != Hv_STANDARDXY)
	return;
    
    for (tcurve = plotdata->curves; tcurve != NULL; tcurve = tcurve->next) {
	Hv_PlotDataMinMax(tcurve->y,
			  tcurve->npts,
			  tcurve->yscale,
			  tcurve->absval,
			  &(tcurve->ymin),
			  &(tcurve->ymax));

	ymin = Hv_fMin(ymin, tcurve->ymin);
	ymax = Hv_fMax(ymax, tcurve->ymax);
    }
    
    
    if (ymin >= 0.0) {
	if (plotdata->ylog)
	    ymin = (float)(0.95*ymin);
	else
	    ymin = 0.0;
	ymax = Hv_ConvertToANiceValue(Hv_fMin((float)(1.0e-20), ymax));
    }
    else if (ymax < 1.01e-25) {
	if (plotdata->ylog)
	    ymax = (float)(0.95*ymax);
	else
	    ymax = 0.0;
	ymin = Hv_ConvertToANiceValue(ymin);
    }
    else { /* one neg, one pos */
	ymax = Hv_ConvertToANiceValue(ymax);
	ymin = Hv_ConvertToANiceValue(ymin);
	if (plotdata->ylog)
	    ymin = (float)(1.0e-8*ymax);
    }
    
    plotdata->ymin = ymin;
    plotdata->ymax = ymax;
}


/** 
 * Hv_ZoomPlot
 * @purpose   Zoom a plot. The zoom rect will be confined to the plot rect.
 * @param     view   The View containing the plot.
 * @param     pp     Initial click point.
 * @param     evevnt Causal event.
 */

void Hv_ZoomPlot(Hv_View   view,
		 Hv_Point  pp,
		 Hv_XEvent   *event) {


    Hv_Item     plot;
    Hv_Rect     limitRect;
    short       policy;
    Hv_PlotData plotdata;

	if (view == NULL)
		return;
    
    if ((plot = Hv_PointInPlotItem(view, pp)) == NULL)
	return;
    
    plotdata = Hv_GetPlotData(plot);
    
/* set if we are actually restoring the default world */
    
    if (Hv_Shifted(event)) {
	
	plotdata->xmin = plotdata->defworld->xmin;
	plotdata->xmax = plotdata->defworld->xmax;
	plotdata->ymin = plotdata->defworld->ymin;
	plotdata->ymax = plotdata->defworld->ymax;
	
/* Hv_TieXAxis will cause the plots to be redrawn */
	
	Hv_TieXAxis(plot);
	
	if (Hv_UserZoom != NULL)
	    Hv_UserZoom(plot->view);
	
	return;
    }  /* end default world restore */
    
    Hv_CopyRect(&limitRect, plot->area);
    
/* see if y/x can change according to the
   value of the y/x nozoom field */
    
    if ((plotdata->ynozoom) && (plotdata->xnozoom))
	return;  /* do nothing */
    
    else if (plotdata->ynozoom)
		policy = Hv_FIXEDYPOLICY;
    else if (plotdata->xnozoom)
		policy = Hv_FIXEDXPOLICY;
   else
		policy = Hv_NOSELECTPOLICY;
    
 
	Hv_LaunchSelectRect(view,
		plot,
		&limitRect,
		&pp,
		policy,
		1.0,
		Hv_PlotZoomSelectCallback);
}

/*------- Hv_TieXAxis -------*/

void Hv_TieXAxis(Hv_Item splot)
    
/******************************************
  find plots with same x tag and give them
  the same x axis limits
  *****************************************/
    
{
    Hv_View            View = splot->view;
    Hv_Item            dplot;
    Hv_PlotData        splotdata;
    Hv_PlotData        dplotdata;
    Boolean            tied = False;
    
    splotdata = Hv_GetPlotData(splot);
    
    
    if (splotdata->xtietag != 0)
	for (dplot = View->items->first; dplot != NULL; dplot = dplot->next)
	    if (dplot->type == Hv_PLOTITEM)
		if (dplot != splot) {
		    dplotdata = Hv_GetPlotData(dplot);
		    if (splotdata->xtietag == dplotdata->xtietag) {
			dplotdata->xmin = splotdata->xmin;
			dplotdata->xmax = splotdata->xmax;
			tied = True;
		    }
		}
    
    if (tied)
	Hv_UpdatePlotView(splot->view);
    else
	Hv_UpdateXYPlot(splot);
}

/*------- Hv_UpdatePlotView ----------*/

void Hv_UpdatePlotView(Hv_View View)
    
{
    Hv_Region hrrgn;
    
    hrrgn = Hv_RectRegion(View->hotrect);
    Hv_OffScreenViewUpdate(View, hrrgn);
    Hv_DestroyRegion(hrrgn);
}


/*------- Hv_ZoomPlotToRect ------*/

void Hv_ZoomPlotToRect(Hv_Item   plot,
		       Hv_Rect   *rect)
    
{
    Hv_PlotData     plotdata;
    short           l, t, r, b;
    float           xmin, xmax, ymin, ymax;
    
    if ((plot == NULL) || (rect == NULL))
	return;
    
    plotdata = Hv_GetPlotData(plot);
    
    if((rect->width > Hv_MINVIEWMOVE) && (rect->height > Hv_MINVIEWMOVE)) {
	if (!Hv_dontZoom) {
	    Hv_GetRectLTRB(rect, &l, &t, &r, &b);
	    
	    if (plotdata->ynozoom) {
		t = plot->area->top;
		b = plot->area->bottom;
	    }
	    if (plotdata->xnozoom) {
		l = plot->area->left;
		r = plot->area->right;
	    }
	    
	    Hv_LocalToPlot(plot, l, b, &xmin, &ymin);
	    Hv_LocalToPlot(plot, r, t, &xmax, &ymax);
	    
	    plotdata->xmin = xmin;
	    plotdata->xmax = xmax;
	    plotdata->ymin = ymin;
	    plotdata->ymax = ymax;
	    
/* now see if other plots have a tied x axis
   (Hv_TieXAxis will cause a redraw) */
	    
	    Hv_TieXAxis(plot);
	    
/* see if user set up additional action */
	    
	    if (Hv_UserZoom != NULL)
		Hv_UserZoom(plot->view);
	    
	}
	else
	    Hv_Information("Zooming is disabled when a tool is active.");
    } /* end of routine zoom */
    
    
    Hv_UpdateVirtualView();
    
}


/*---------- Hv_DestroyPlotItem --------*/

void   Hv_DestroyPlotItem(Hv_Item Item)
    
{
    Hv_PlotData      plot;
    Hv_View          View;
    
    if  (Item == NULL)
	return;
    
    View = Item->view;
    
    plot = (Hv_PlotData)Item->data;
    
    Hv_DestroyPlotData(plot);
    Item->data = NULL;
/*  Hv_CheckPlotViewSensitivity(View); */
}

/*------ Hv_GetPlotData ------*/

Hv_PlotData   Hv_GetPlotData(Hv_Item plot)
    
{
    if ((plot == NULL) || (plot->type != Hv_PLOTITEM))
	return NULL;
    
    return (Hv_PlotData)(plot->data);
}

/*------ Hv_GetPlotViewData ------*/

Hv_PlotViewData   Hv_GetPlotViewData(Hv_View View)
    
{
    if ((View == NULL) || (View->data == NULL))
	return NULL;
    
    return (Hv_PlotViewData)(View->data);
}

/*------ Hv_GetPlotViewUserData ------*/

void  *Hv_GetPlotViewUserData(Hv_View View)
    
/* returns the pointer that can be used
   by applications to attach data structures
   to plot views */
    
{
    Hv_PlotViewData    pvdata;
    
    if (View == NULL)
	return NULL;
    
    pvdata = Hv_GetPlotViewData(View);
    if (pvdata == NULL)
	return NULL;
    
    return pvdata->data;
}

/*------ Hv_SetPlotViewUserData ------*/

void  Hv_SetPlotViewUserData(Hv_View View,
			     void *data)
    
/* sets the pointer that can be used
   by applications to attach data structures
   to plot views */
    
{
    Hv_PlotViewData    pvdata;
    
    if (View == NULL)
	return;
    
    pvdata = Hv_GetPlotViewData(View);
    if (pvdata == NULL)
	return;
    
    pvdata->data = data;
}

/*--------- InitPlotViewItems ------*/

static void  InitPlotViewItems(Hv_View View)
    
/* NULL out the items used for on-view
   editing */
    
{
    Hv_PlotViewData    pvdata;
    
    if (View == NULL)
	return;
    
    pvdata = Hv_GetPlotViewData(View);
    
/* these are for the on-view editor */
    
    pvdata->activeplot = NULL;
    pvdata->activename = NULL;
    pvdata->bin1slider = NULL;
    pvdata->bin2slider = NULL;
    pvdata->viewbox = NULL;
    pvdata->curvemenu = NULL;
}


/*------ Hv_InitPlotViewData ------*/

void   Hv_InitPlotViewData(Hv_View View)
    
{
    Hv_PlotViewData    pvdata;
    
    if (View == NULL)
	return;
    
    if (View->data == NULL) {
	pvdata = (Hv_PlotViewData)Hv_Malloc(sizeof(Hv_PlotViewDataRec));
	pvdata->savebutton = NULL;
	View->data = (void *)(pvdata);
    }
    else
	pvdata = Hv_GetPlotViewData(View);
    
    pvdata->lastplot = NULL;
    pvdata->plotxmin =  (float)(-2.0e30);
    pvdata->plotxmax =  (float)(2.0e30);
    pvdata->plotymin =  (float)(-2.0e30);
    pvdata->plotymax =  (float)(2.0e30);
    pvdata->plotxlog = False;
    pvdata->plotylog = False;
    pvdata->plotlegendleft = (float)(2.0e30);
    pvdata->plotlegendtop =  (float)(2.0e30);
    pvdata->data = NULL;
}


/*------ Hv_StandardPlotLabel ------- */

Hv_Item Hv_StandardPlotLabel(Hv_Item  plot,
			     short   x,
			     short   y,
			     char   *text,
			     short   font)
    
{
    Hv_Item       newItem;
    short         fcolor;
    
    if (text == NULL)
	return NULL; 
    
    fcolor = Hv_GetViewBackgroundColor(plot->view);
    
    newItem = Hv_VaCreateItem(plot->view,
			      Hv_PARENT,       plot,
			      Hv_TYPE,         Hv_TEXTITEM,
			      Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			      Hv_DRAWCONTROL,  Hv_DRAGGABLE,
			      Hv_LEFT,         x,
			      Hv_TOP,          y,
			      Hv_FONT,         font,
			      Hv_TEXT,         text,
			      Hv_FILLCOLOR,    fcolor,
			      NULL);

    
    return newItem;
}


/*-------- Hv_LocalToPlot ------*/

void  Hv_LocalToPlot(Hv_Item  plot,
		     short   x,
		     short   y,
		     float   *px,
		     float   *py)
    
{
    float        rx, ry;
    short        xoff, yoff;
    Hv_Rect      *local;
    Hv_PlotData  plotdata;
    float        w,h;
    float        xmin, xmax, ymin, ymax;
    
    plotdata = Hv_GetPlotData(plot);
    
    local = plot->area;
    
    xmin = plotdata->xmin;
    xmax = plotdata->xmax;
    ymin = plotdata->ymin;
    ymax = plotdata->ymax;
    
/* need to correct for log plots */
    
    if (plotdata->xlog) {
	xmin = CheckLog10(xmin);
	xmax = CheckLog10(xmax);
    }
    if (plotdata->ylog) {
	ymin = CheckLog10(ymin);
	ymax = CheckLog10(ymax);
    }
    
    w = xmax - xmin;
    h = ymax - ymin;
    
    xoff = local->left;
    yoff = local->top;
    
    rx = w/((float)local->width);
    ry = h/((float)local->height);
    
    *px = xmin + rx*(x - xoff);
    *py = ymax + ry*(yoff - y);
    
/* need to correct for log plots */
    
    if (plotdata->xlog)
	*px = (float)(pow((double)10.0, (double)(*px)));
    if (plotdata->ylog)
	*py = (float)(pow((double)10.0, (double)(*py)));
    
}

/*-------- Hv_PlotToLocal -------*/

void  Hv_PlotToLocal(Hv_Item  plot,
		     short   *x,
		     short   *y,
		     float   px,
		     float   py)
    
    
{
    float        rx, ry, xx, yy;
    short        xoff, yoff;
    Hv_Rect      *local;
    Hv_PlotData  plotdata;
    float        w,h;
    float        xmin, xmax, ymin, ymax;
    
    plotdata = Hv_GetPlotData(plot);
    
    local = plot->area;
    
    xmin = plotdata->xmin;
    xmax = plotdata->xmax;
    ymin = plotdata->ymin;
    ymax = plotdata->ymax;
    
/* need to correct for log plots */
    
    if (plotdata->xlog) {
	xmin = CheckLog10(xmin);
	xmax = CheckLog10(xmax);
	px = CheckLog10(px);
    }
    if (plotdata->ylog) {
	ymin = CheckLog10(ymin);
	ymax = CheckLog10(ymax);
	py = CheckLog10(py);
    }
    
    w = xmax - xmin;
    h = ymax - ymin;
    
    xoff = local->left;
    yoff = local->top;
    
    rx = ((float)local->width)/w;
    ry = ((float)local->height)/h;
    
    xx = xoff + rx*(px - xmin);
    yy = yoff + ry*(ymax - py);
    
    *x = Hv_InRange(xx);
    *y = Hv_InRange(yy);
    
}


/*-------- Hv_PlotDataMinMax -----*/

void  Hv_PlotDataMinMax(float  *vals,
			int     n,
			float   scale,
			Boolean absval,
			float  *dmin,
			float  *dmax)
    
/******************************
  modified 6/6/96 for zero scale
  ********************************/
    
{
    int   i;
    float temp;
    float fscale;
    float val;

    *dmin = (float)(1.0e36);
    *dmax = (float)(-1.0e36);
    
    if (vals == NULL)
	return;
    
/* handle zero scale */
    
    if (scale < 0.0)
	fscale = -scale;
    else
	fscale = scale;
    
    if (fscale < 1.0e-30) {
	if (scale < 0.0)
	    scale = (float)(-1.0e-30);
	else
	    scale = (float)(1.0e-30);
    }
    
    for (i = 0; i < n; i++) {

        val = vals[i];

	if (absval)
	  val = (float)fabs(val);

	if (val < *dmin)
	    *dmin = val;
	if (val > *dmax)
	    *dmax = val;
    }
    
    if (scale < 0.0) {
	temp = *dmin;
	*dmin = *dmax;
	*dmax = *dmin;
    }
    
    *dmin *= scale;
    *dmax *= scale;
}

/*------ Hv_BigThreeItems ------*/

void Hv_BigThreeItems(Hv_Item  plot,
		      Hv_Item  *title,
		      Hv_Item  *xlab,
		      Hv_Item  *ylab)
    
{
    Hv_Item   temp;
    
    *title = NULL;
    *xlab = NULL;
    *ylab = NULL;
    
    if ((plot == NULL) || (plot->children == NULL))
	return;
    
    for (temp = plot->children->first; temp != NULL; temp = temp->next) {
	if (temp->tag == Hv_XYTITLETAG)
	    *title = temp;
	else if (temp->tag == Hv_XLABELTAG)
	    *xlab = temp;
	else if (temp->tag == Hv_YLABELTAG)
	    *ylab = temp;
    }
}

/*------- static functions -------*/


/*-------- DrawXTickItem ----------*/

static void  DrawXTickItem(Hv_Item  Item,
			   Hv_Region region)
{
    Hv_DrawPlotTicks(Item->parent, True, False);
}

/*-------- DrawYTickItem ----------*/

static void  DrawYTickItem(Hv_Item  Item,
			   Hv_Region region)
{
    Hv_DrawPlotTicks(Item->parent, False, True);
}


/*------- PlotFunction ------*/

static float PlotFunction(float x)
    
{
    float  y;
    Hv_Splint(splinex, spliney, spliney2, nspline, x, &y);
    return y;
}

/*------- LineFunction ------*/

static float LineFunction(float x)
    
{
    float  y;
    y = TheCurve->intercept + TheCurve->slope*x;
    return y;
}

/*------- ExponentialFunction ------*/

static float ExponentialFunction(float x)
    
{
    float  y;
    y = (TheCurve->intercept)*((float)(pow((double)10.0, (double)(TheCurve->slope*x))));
    return y;
}

/*------- GaussFunction ------*/

static float GaussFunction(float x)
    
{
    float    y, arg;
    int      i, j, n;
    float    *a;
    
    
    n = (TheCurve->ma)/3;
    a = TheCurve->a;
    
    y = 0.0;
    
    for (i = 0; i < n; i++) {
	j = 3*i;
	arg = (x-a[j+1])/a[j+2];
	arg = -arg*arg;
	y = y + (float)((a[j]*exp(arg)));
    }
    
    return  y;
    
}

/*------- PolyFunction ------*/

static float PolyFunction(float x)
    
/* For polynomial fit. ma will be "one more" than the order of
   the fit -- if polyorder = 4 then ma will be 5 corresponding
   to a0 + a1*x + a2*x^2 + a3*x^3 + a4*x^4 */
    
{
    float  y, xx, *a;
    int    i;
    
    if (TheCurve->ma < 1)
	return 0.0;
    
    a = TheCurve->a;
    
    y = a[0];
    xx = 1.0;
    
    for (i = 1; i < TheCurve->ma; i++) {
	xx *= x;
	y = y + a[i]*xx;
    }
    
    return y;
}


/*------- LegendreFunction ------*/

static float LegendreFunction(float x)
    
/* For legendre fit. ma will be "one more" than the order of
   the fit -- if legndreorder = 4 then ma will be 5 corresponding
   to terms up to x^4 */
    
{
    float  y, pl[15], *a;
    int    i;
    
    if (TheCurve->ma < 1)
	return 0.0;
    
    a = TheCurve->a;
    nrflegendre(x, pl-1, TheCurve->ma+1);
    
    y = 0.0;
    for (i = 0; i < TheCurve->ma; i++)
	y += a[i]*pl[i];
    
    return y;
}


/*------- nrfgauss -------*/

static void nrfgauss(float x,
		     float a[],
		     float *y,
		     float dyda[],
		     int  na)
    
{
    int i;
    float fac, ex, arg;
    
    *y=0.0;
    for (i=1; i<=na-1; i+=3) {
	arg=(x-a[i+1])/a[i+2];
	ex = (float)(exp(-arg*arg));
	fac = (float)(a[i]*ex*2.0*arg);
	*y += a[i]*ex;
	dyda[i] = ex;
	dyda[i+1] = fac/a[i+2];
	dyda[i+2] = fac*arg/a[i+2];
    }
}

/*--------- nrfpoly -------*/

static void nrfpoly(float  x,
		    float  *p,
		    int   n)
{
    int  j;
    
    p[1] = 1.0;
    for (j = 2; j <= n; j++)
	p[j] = p[j-1]*x;
}


/*--------- nrflegendre -------*/

static void nrflegendre(float  x,
			float  *pl,
			int   n)
    
{
    int    j;
    float  twox, f2, f1, d;
    
    pl[1] = 1.0;
    pl[2] = x;
    
    if (n > 2) {
	twox = (float)(2.0*x);
	f2 = x;
	d = 1.0;
	
	for (j = 3; j <= n; j++){
	    f1 = d++;
	    f2 += twox;
	    pl[j]=(f2*pl[j-1] - f1*pl[j-2])/d;
	}
    }
}


/*------ PlotSegment ----------*/

static void PlotSegment(Hv_Item      plot,
			float       x1,
			float       x2,
			short       color,
			float       bg,
			Hv_FloatFunction f)
    
/* plot from x1 to x2 using the function f */
    
{
    short        sx1, sx2, xt, yt;
    Hv_PlotData  plotdata;
    Hv_Point     *xp;
    short        np;
    float        temp, x;
    float        dx;
    int          i;
    
    if (Hv_nrerror)
	return;
    
    if (Hv_inPostscriptMode) {
	PSPlotSegment(plot, x1, x2, color, bg, f);
	return;
    }
    
    plotdata = Hv_GetPlotData(plot);
    
/* make sure x1 is < x2 */
    
    if (x1 > x2) {
	temp = x1;
	x1 = x2;
	x2 = temp;
    }
    
/* no need to go outside the plot */
    
    x1 = Hv_fMax(x1, plotdata->xmin);
    x2 = Hv_fMin(x2, plotdata->xmax);
    
    Hv_PlotToLocal(plot, &sx2, &yt, x2, plotdata->ymin);
    Hv_PlotToLocal(plot, &sx1, &yt, x1, plotdata->ymin);
    
    if (sx2 <= sx1) {
	Hv_Println("[%d] [%d] [%f] [%f] premature return in PlotSegment", sx1, sx2, x1, x2);
	return;
    }
    
    np = sx2 - sx1 + 1;
    
    dx = (x2 - x1)/np;
    xp = Hv_NewPoints(np);
    
    for (i = 0; i < np; i++) {
	x = x1 + i*dx;
	Hv_PlotToLocal(plot, &xt, &yt, x, f(x) + bg);
	Hv_SetPoint(xp+i, xt, yt);
    }
    
    Hv_DrawLines(xp, np, color);
    Hv_Free(xp);
}

/*------ PSPlotSegment ----------*/

static void PSPlotSegment(Hv_Item      plot,
			  float       x1,
			  float       x2,
			  short       color,
			  float       bg,
			  Hv_FloatFunction f)
    
/* plot from x1 to x2 using the function f */
    
{
    short        sx1, sx2, xt, yt;
    Hv_PlotData  plotdata;
    Hv_Point     *xp;
    short        np;
    float        temp, x;
    float        dx;
    int          i;
    
    plotdata = Hv_GetPlotData(plot);
    
/* make sure x1 is < x2 */
    
    if (x1 > x2) {
	temp = x1;
	x1 = x2;
	x2 = temp;
    }
    
/* no need to go outside the plot */
    
    x1 = Hv_fMax(x1, plotdata->xmin);
    x2 = Hv_fMin(x2, plotdata->xmax);
    
    Hv_PlotToPSLocal(plot, &sx2, &yt, x2, plotdata->ymin);
    Hv_PlotToPSLocal(plot, &sx1, &yt, x1, plotdata->ymin);
    
    if (sx2 <= sx1) {
	Hv_Println("premature return in PSPlotSegment");
	return;
    }
    
    np = sx2 - sx1 + 1;
    
    dx = (x2 - x1)/np;
    xp = Hv_NewPoints(np);
    
    
    for (i = 0; i < np; i++) {
	x = x1 + i*dx;
	Hv_PlotToPSLocal(plot, &xt, &yt, x, f(x)+bg);
	Hv_SetPoint(xp+i, xt, yt);
    }
    
    Hv_PSDrawLines(xp, np, color);
    Hv_Free(xp);
}

/*--------- TurningPoints ---------*/

static void  TurningPoints(float *varray,
			   int  *iarray,
			   int  n)
    
/* given the float array varray, and a malloced int array
   iarray, this will load into iarray the turning points.
   the last meaningfule entry in iarray will be n-1.
   
   ---- if no turning points, iarray[0] = n-1
   
   */
    
{
    Boolean direction;
    int nm1;
    int i = 1;
    int j = 0;
    
    nm1 = n - 1;
    
    if ((varray == NULL) || (iarray == NULL) || (n < 1))
	return;
    
    direction = (varray[1] > varray[0]); 
    
    while (i < nm1) {
	if ((fabs(varray[i+1] - varray[i]) <= 1.0e-6*fabs(varray[i]))
	    || (fabs(varray[i-1] - varray[i]) <= 1.0e-6*fabs(varray[i]))
	    || (direction != (varray[i+1] > varray[i]))) {
	    direction = !direction;
	    iarray[j] = i;
	    j++;
	}
	i++;
    }
    
    iarray[j] = nm1;
}


/*-------- GetWorldBoundingRect ---*/

static void GetWorldBoundingRect(Hv_Item  plot,
				 Hv_FRect  *wbr)
    
{
    Hv_Item    temp;
    Hv_FRect   crect;
    
    if ((plot == NULL) || (wbr == NULL))
	return;
    
    Hv_LocalRectToWorldRect(plot->view, plot->area, wbr);
    if (plot->children != NULL)
	for (temp = plot->children->first; temp != NULL; temp = temp->next) {
	    Hv_LocalRectToWorldRect(temp->view, temp->area, &crect);
	    wbr->xmin = Hv_fMin(wbr->xmin, crect.xmin);
	    wbr->xmax = Hv_fMax(wbr->xmax, crect.xmax);
	    wbr->ymin = Hv_fMin(wbr->ymin, crect.ymin);
	    wbr->ymax = Hv_fMax(wbr->ymax, crect.ymax);
	    
	    Hv_FixFRectWH(wbr);
	}
}


/*------- Hv_DrawFitResultItem -------*/

static void Hv_DrawFitResultItem(Hv_Item  Item,
				 Hv_Region region)
    
{
    Hv_Item       plot;
    Hv_PlotData   plotdata;
    Hv_CurveData  tcurve;
    short         i, j, n, w, h, x, y;
    char          fittext[256];
    float         a, b;
    char          truncname[11];
    
    if ((Item == NULL)  || (Item->parent == NULL))
	return;
    
    plot = Item->parent;
    plotdata = Hv_GetPlotData(plot);
    
    if ((plotdata == NULL) || !(plotdata->uselegend))
	return;

/* first size it */

    n = 0;
    w = 0;
    h = 0;
    x = Item->area->left;
    y = Item->area->top;

    for (tcurve = plotdata->curves; tcurve != NULL; tcurve = tcurve->next) 
	if ((tcurve->algorithm == Hv_CURVEFIT) && (tcurve->showfit)) {
	    n++;

	    b = tcurve->slope;
	    a = tcurve->intercept;

	    TruncName(truncname, tcurve->name, 10);

	    switch (tcurve->fittype) {
	    case Hv_LINEFIT:

		sprintf(fittext, "%s [\\sy=mx+b\\s] (m: %6.3f b: %6.3f)  \\gc\\^2\\d: %6.2f",
			truncname, b, a, tcurve->chisq);

		Hv_DrawFitString(plotdata, fittext, x, y, &w, &h, 4);
		sprintf(fittext, "\\g\\+s\\-\\d\\_m\\^: %6.2f  \\g\\+s\\-\\d\\_b\\^: %6.2f",
			tcurve->sigb, tcurve->siga);

		Hv_DrawFitString(plotdata, fittext, x, y, &w, &h, 0);
		break;


	    case Hv_EXPONENTIALFIT:
		if ((fabs(b) > 1.0e-20) && (fabs(b) < 0.005))
		    sprintf(fittext, "%s [\\sy=a\\s10\\^bx\\d\\s] (a: %6.3f b: %-9.2e)",
			    truncname, a, b);
		else
		    sprintf(fittext, "%s [\\sy=a\\s10\\^bx\\d\\s] (a: %6.3f b: %6.3f)",
			    truncname, a, b);
		
		Hv_DrawFitString(plotdata, fittext, x, y, &w, &h, 4);
		break;

	    case Hv_GAUSSIANFIT:
		sprintf(fittext, "%s [\\sy=\\g\\+\\_S\\d\\sa\\_i\\d\\sexp\\+[\\d-\\+(\\d(x-\\g\\+m\\d\\_i\\d)/\\g\\+s\\d\\_i\\d\\+)\\d\\^2\\d\\+]\\d\\s] \\gc\\^2\\d: %6.2f",
			truncname, tcurve->chisq);
		Hv_DrawFitString(plotdata, fittext, x, y, &w, &h, 4);
		for (i=0; i < tcurve->numgauss; i++) {
		    j = i*3;
		    sprintf(fittext, "\ta\\_%-2d\\d: %6.3f \\g\\+m\\d\\_%-2d\\d: %6.3f \\g\\+s\\d\\_%-2d\\d: %6.3f",
			    i, tcurve->a[j], i, tcurve->a[j+1], i, tcurve->a[j+2]);
		    
		    Hv_DrawFitString(plotdata, fittext, x, y, &w, &h, 0);
		}
		break;

	    case Hv_LEGENDREFIT:
		sprintf(fittext, "%s [\\sy=\\g\\+\\_S\\d\\sa\\_l\\d\\sP\\_l\\d\\s(x)] \\gc\\^2\\d: %6.2f",
			truncname, tcurve->chisq);
		Hv_DrawFitString(plotdata, fittext, x, y, &w, &h, 4);
		for (i=0; i < (tcurve->ma); i++) {
		    sprintf(fittext, "\ta\\_%-2d\\d: %-9.2e", i, tcurve->a[i]);
		    Hv_DrawFitString(plotdata, fittext, x, y, &w, &h, 0);
		}
		break;

	    case Hv_POLYFIT:
		sprintf(fittext, "%s [\\sy=\\g\\+\\_S\\d\\sa\\_i\\d\\sx\\^i\\d\\s] \\gc\\^2\\d: %6.2f",
			truncname, tcurve->chisq);
		Hv_DrawFitString(plotdata, fittext, x, y, &w, &h, 4);
		for (i=0; i < (tcurve->ma); i++) {
		    sprintf(fittext, "\ta\\_%-2d\\d: %6.3f", i, tcurve->a[i]);
		    Hv_DrawFitString(plotdata, fittext, x, y, &w, &h, 0);
		}
		break;

	    }
	}

    Item->area->width = w;
    Item->area->height = h;
    Hv_FixRectRB(Item->area);
/*    Hv_FrameRect(Item->area, Hv_red); */
}

/*------- Hv_DrawFitString -----------*/

static void Hv_DrawFitString(Hv_PlotData plotdata,
			     char *fittext,
			     short x,
			     short y,
			     short *w,
			     short *h,
			     short gap)
{
    Hv_String   *hvstr;
    short       sw, sh;

    hvstr = Hv_CreateString(fittext);
    hvstr->font = plotdata->fitresultfont;
    Hv_CompoundStringDimension(hvstr, &sw, &sh);

    Hv_DrawCompoundString(x, (short)(y + *h + gap), hvstr);
    Hv_DestroyString(hvstr);

    *w = Hv_sMax(*w, sw);
    *h += sh + gap;
}

/*------- Hv_DrawLegendItem -------*/

static void Hv_DrawLegendItem(Hv_Item  Item,
			      Hv_Region region)
    
{
    Hv_Item       plot;
    Hv_PlotData   plotdata;
    Hv_CurveData  tcurve;
    int           offset;
    Hv_String     *hvstr;
    short         w, h, x, y, yc, x1, x2, sx;
    Hv_Point      pp;
    Hv_Rect       rect;
    
    if ((Item == NULL)  || (Item->parent == NULL))
	return;
    
    plot = Item->parent;
    plotdata = Hv_GetPlotData(plot);
    
    if ((plotdata == NULL) || !(plotdata->uselegend))
	return;
    
    offset = Hv_SizeLegendItem(Item);
    
    x = Item->area->left + offset;
    sx = Item->area->left + (offset/2);
    y = Item->area->top + LEGENDVGAP;
    x1 = Item->area->left+10;
    x2 = x - 10;
    
    
    Hv_simpleSymbols = !(plotdata->fancysymbols);
    Hv_fancySymbolLite = Hv_gray10;
    
    if (plotdata->opaquelegend)
	Hv_FillRect(Item->area, Hv_white);
    
    for (tcurve = plotdata->curves; tcurve != NULL; tcurve = tcurve->next) 
	if ((tcurve->name != NULL) && (tcurve->display) && (tcurve->onlegend)){
	    hvstr = Hv_CreateString(tcurve->name);
	    hvstr->font = plotdata->legendfont;
	    Hv_CompoundStringDimension(hvstr, &w, &h);
	    Hv_DrawCompoundString(x, y, hvstr);
	    
	    yc = y + (h/2);
	    
/* what we draw here depends on the type of the plot */
	    
	    if ((plotdata->plottype == Hv_HSTACK) || (plotdata->plottype == Hv_VSTACK)) {
		Hv_SetRect(&rect, x1, y, (short)(x2-x1), h);
		Hv_FillPatternRect(&rect, tcurve->linecolor, tcurve->pattern);
		Hv_FrameRect(&rect, Hv_black);
		
	    } /* stack */
	    
	    else if (plotdata->plottype == Hv_STANDARDXY) {
		
		if (tcurve->algorithm != Hv_NOLINES) {
		    Hv_SetLineStyle(tcurve->thickness, tcurve->style);
		    Hv_DrawLine(x1, yc, x2, yc, tcurve->linecolor);
		    Hv_SetLineStyle(0, Hv_SOLIDLINE);
		}
		
/* need to consider the spike case */
		
		if ((tcurve->algorithm == Hv_NOLINES) && (tcurve->yspike))
		    Hv_DrawLine(x1, yc, x2, yc, tcurve->linecolor);
		
		if (tcurve->symbol != Hv_NOSYMBOL) {
		    Hv_SetPoint(&pp, sx, yc);
		    if (tcurve->symbol == Hv_POINTSYMBOL)
			Hv_DrawPoint(pp.x, pp.y, tcurve->symbolcolor);
		    else if (tcurve->symbol == Hv_BIGPOINTSYMBOL)
			Hv_DrawSymbolsOnPoints(&pp,
					       1,
					       3,
					       tcurve->symbolcolor,
					       Hv_RECTSYMBOL);
		    else
			Hv_DrawSymbolsOnPoints(&pp,
					       1,
					       tcurve->symbolsize,
					       tcurve->symbolcolor,
					       tcurve->symbol);
		}
	    } /* standard xy */
	    
	    y = y + h + LEGENDVGAP;
	    Hv_DestroyString(hvstr);
	}
    
    Hv_simpleSymbols = False;
    Hv_fancySymbolLite = Hv_white;
    
    if (plotdata->framelegend)
	Hv_FrameRect(Item->area, Hv_black);
}

/*-------- Hv_DrawPlotItem -------*/

static void Hv_DrawPlotItem(Hv_Item  plot,
			    Hv_Region region)
    
{
    int                 i, e1, e2;
    short               x1, x2, y1, y2;
    Hv_PlotData         plotdata;
    short               l, t, r, b;
    short               ybg;
    Hv_Region           plotreg;
    Hv_CurveData        tcurve;
    Hv_Rect             *rects[4];
    Hv_Rect             bgrect;
    
    int                 plottype;
    Boolean             standardxy;  /* true for standard plots */
    Boolean             barplot;
    Boolean             histo1, histo2;
    
    Boolean             specialplot;

    Hv_Point            *pp;

    plotdata = Hv_GetPlotData(plot);
    plottype = Hv_GetPlotType(plot);

    specialplot = (plottype > 1000);

    standardxy = (plottype == Hv_STANDARDXY);
    barplot = ((plottype == Hv_HBAR) || (plottype == Hv_VBAR) || (plottype == Hv_HSTACK) || (plottype == Hv_VSTACK));
    histo1 = (plottype == Hv_1DHISTO);
    histo2 = (plottype == Hv_2DHISTO);
    
    Hv_GetRectLTRB(plot->area, &l, &t, &r, &b);
    
/* now it is time to draw each curve ! */
    
    Hv_simpleSymbols = !(plotdata->fancysymbols);
    Hv_fancySymbolLite = Hv_gray10;
    
    plotreg = Hv_ClipPlotItem(plot, region);
    
    if (plotdata->bgcolor > 0) {
	Hv_FillRect(plot->area, plotdata->bgcolor);
	Hv_DrawLegendItem(plotdata->legenditem, plotreg);
    }
    
    Hv_DrawPlotTicks(plot, True, True);
    
    if (plotdata->xzeroline)
	VerticalLine(plot, 0.0, Hv_black);
    
    if (plotdata->yzeroline) 
	HorizontalLine(plot, 0.0, Hv_black);
    
/* background used for standard plots only */
    
    if ((plotdata->showbackground) && standardxy) {
	ybg = HorizontalLine(plot, plotdata->background, Hv_gray13);
	if ((ybg > t) && (ybg < b)) {
	    Hv_SetRect(&bgrect, (short)(l+1), (short)ybg, (short)(r-l-2), (short)(b-ybg-1));
	    Hv_FillRect(&bgrect, Hv_gray13);
	}
    }
    
/* yfitresult is the vert location where the "results of the
   fit (if any) are drawn */
    
    if (standardxy) {
	yfitresult = plot->area->top + 4;
	for (tcurve = plotdata->curves; tcurve != NULL; tcurve = tcurve->next)
	    DrawCurve(plot, plotreg, tcurve);
    }
    else if (barplot)
	Hv_DrawBarCurves(plot);
    else if (histo1) {
	Hv_Draw1DHistograms(plot);
	
/* still call drawcurve for fits */
	
	yfitresult = plot->area->top + 4;
	for (tcurve = plotdata->curves; tcurve != NULL; tcurve = tcurve->next)
	    if (tcurve->algorithm == Hv_CURVEFIT) {
		DrawCurve(plot, plotreg, tcurve);
	    }
    }
    
    else if (specialplot) {
	
	Hv_Println("drawing special plot");

	switch (plottype) {
	case Hv_THEISSEN:

/* only one curve */

/* number of edges stored in fittype, edges stored in xerr and yerr */

	    tcurve = plotdata->curves;

	    pp = Hv_NewPoints(tcurve->npts);

	    for (i = 0; i < tcurve->npts; i++)
		Hv_PlotToLocal(plot, &(pp[i].x), &(pp[i].y), tcurve->x[i], tcurve->y[i]);


	    for (i = 0; i < tcurve->fittype; i++) {
		e1 = (int)(tcurve->xerr[i]);
		e2 = (int)(tcurve->yerr[i]);

		Hv_PlotToLocal(plot, &x1, &y1, tcurve->x[e1], tcurve->y[e1]);
		Hv_PlotToLocal(plot, &x2, &y2, tcurve->x[e2], tcurve->y[e2]);
		Hv_DrawLine(x1, y1, x2, y2, tcurve->linecolor);
	    }

	    Hv_DrawSymbolsOnPoints(pp,
				   tcurve->npts,
				   4,
				   tcurve->symbolcolor,
				   Hv_CIRCLESYMBOL);


	    break;

	}
	
    }
    

    else if (histo2)
	Hv_Draw2DHistograms(plot);
    
/* any user draw? */
    
    if (plotdata->UserPlotDraw != NULL)
	plotdata->UserPlotDraw(plot, plotreg);
    
    Hv_simpleSymbols = False;
    Hv_fancySymbolLite = Hv_white;
    Hv_DestroyRegion(plotreg);
    
/* now some adornements. First the grow rects (if appropriate) */
    
    if ((!Hv_inPostscriptMode) && Hv_CheckItemDrawControlBit(plot, Hv_ENLARGEABLE)) {
	for (i = 0; i < 4; i++)
	    rects[i] = Hv_NewRect();
	
	Hv_GetCornerRects(plot->area, rects, 6);
	for (i = 0; i < 4; i++) {
	    Hv_FillRect(rects[i], Hv_gray12);
	    Hv_Free(rects[i]);
	}
    }
    
/* draw axes based on axes style */
    
    switch (plotdata->axesstyle) {
    case Hv_TWOAXES:
	Hv_DrawLine(r, b, l, b, plotdata->axescolor);
	Hv_DrawLine(l, b, l, t, plotdata->axescolor);
	break;
	
    case Hv_FOURAXESSIMPLE:
	Hv_FrameRect(plot->area,  plotdata->axescolor);
	break;
	
    case Hv_FOURAXES:
	Hv_FrameRect(plot->area,  plotdata->axescolor);
	break;
    }
    
    Hv_RestoreClipRegion(region); 
}

/*------ Hv_ClipPlotItem ------ */

static Hv_Region Hv_ClipPlotItem(Hv_Item  plot,
				 Hv_Region region)
    
{
    Hv_Region           preg, totreg;
    Hv_Rect             area;
    Hv_Rect             psrect;
    
    if (plot == NULL)
	return NULL;
    
    Hv_CopyRect(&area, plot->area);
    
/* must check for the weird case where the plot area
   is smaller than the axes area */
    
    area.left   = Hv_sMax(area.left,   plot->area->left);
    area.top    = Hv_sMax(area.top,    plot->area->top);
    area.right  = Hv_sMin(area.right,  plot->area->right);
    area.bottom = Hv_sMin(area.bottom, plot->area->bottom);
    
    if (!Hv_inPostscriptMode) {
	area.right += 1;
	area.bottom += 1;
    }
    
    Hv_FixRectWH(&area);
    
    if (Hv_inPostscriptMode) {
	Hv_LocalRectToPSLocalRect(&area, &psrect);
	Hv_PSClipRect(&psrect);
	return NULL;
    }
    
/* don't draw outside */
    
    preg = Hv_RectRegion(&area);
    totreg = Hv_IntersectRegion(region, preg);
    
    Hv_SetClippingRegion(totreg);
    Hv_DestroyRegion(preg);
    
    return totreg;
}

/*---------- Hv_DrawPlotTicks ------*/

static void Hv_DrawPlotTicks(Hv_Item  plot,
			     Boolean  drawx,
			     Boolean  drawy)
{
    Hv_PlotData   plotdata;
    float         fval, f2val;
    float         del, smalldel;
    int           n, np1, i, m, j;
    float         xmin, xmax, ymin, ymax;
    Hv_Rect       *area;
    float         tscilow, tscihi;
    int           plottype;
    Boolean       barplot, histo1, histo2;
    
    plotdata = Hv_GetPlotData(plot);
    plottype = Hv_GetPlotType(plot);
    
    barplot = ((plottype == Hv_HBAR) || (plottype == Hv_VBAR) ||
	       (plottype == Hv_HSTACK) || (plottype == Hv_VSTACK));
    
    histo1 = (plottype == Hv_1DHISTO);
    histo2 = (plottype == Hv_2DHISTO);
    
    if (plotdata == NULL)
	return;
    
    
/* bar plot? */
    
    if (barplot) {
	Hv_DrawBarPlotTicks(plot, drawx, drawy);
	return;
    }
    
    /*  else if (histo1) {
	return;
	}
	
	else if (histo2) {
	return;
	}
	*/
    
    Hv_SetLineStyle(0, Hv_SOLIDLINE);
    
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
    
/* first the x ticks */
    
    if (drawx && (plotdata->drawmajxticks)) {
	if (plotdata->xlog)
	    Hv_XLogTicks(plot, area);
	else if ((n = plotdata->majxtick) > 0) {
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
    } /* end draw x */
    
/* now the y ticks */
    
    if (drawy && (plotdata->drawmajyticks)) {
	if (plotdata->ylog)
	    Hv_YLogTicks(plot, area);
	else if ((n = plotdata->majytick) > 0) {
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
    }  /* end draw y*/
    
/* restore the global limits */
    
    Hv_sciNotHiLimit  = tscihi;
    Hv_sciNotLowLimit = tscilow;
}

/*----- Hv_XLogTicks ---------*/

static void  Hv_XLogTicks(Hv_Item   plot,
			  Hv_Rect   *area)
{
    Hv_PlotData  plotdata; 
    int          mindecade, maxdecade, i, j;
    float        fval, fval2;
    
    plotdata = Hv_GetPlotData(plot);
    mindecade = Hv_Decade(plotdata->xmin);
    maxdecade = Hv_Decade(plotdata->xmax)+1;
    
    for (i = mindecade; i < maxdecade; i++) {
	fval = (float)(pow((double)10.0, (double)i));
	
	if (plotdata->drawminxticks) {
	    for (j = 2; j < 10; j++) {
		fval2 = j*fval;
		Hv_DrawXTick(plot, fval2, Hv_MINTICKLEN, area, NULL);
	    }
	}
	
	if ((i > mindecade) || (plotdata->drawminxval))
	    Hv_DrawXTick(plot, fval, Hv_MAJTICKLEN, area, NULL);
    }
}

/*----- Hv_YLogTicks ---------*/

static void  Hv_YLogTicks(Hv_Item   plot,
			  Hv_Rect   *area)
    
{
    Hv_PlotData  plotdata; 
    int          mindecade, maxdecade, i, j;
    float        fval, fval2;
    
    plotdata = Hv_GetPlotData(plot);
    mindecade = Hv_Decade(plotdata->ymin);
    maxdecade = Hv_Decade(plotdata->ymax)+1;
    
    for (i = mindecade; i < maxdecade; i++) {
	fval = (float)(pow((double)10.0, (double)i));
	
	if (plotdata->drawminxticks) {
	    for (j = 2; j < 10; j++) {
		fval2 = j*fval;
		Hv_DrawYTick(plot, fval2, Hv_MINTICKLEN, area, NULL);
	    }
	}    
	
	if ((i > mindecade) || (plotdata->drawminyval))
	    Hv_DrawYTick(plot, fval, Hv_MAJTICKLEN, area, NULL);
    }
}

/*-------- DrawCurve --------- */

static void DrawCurve(Hv_Item       plot,
		      Hv_Region     region,
		      Hv_CurveData  curve)
    
{
    float           *x;
    float           *y;
    int             n, nseg, i, nstair, j;
    float           *xerr = NULL;
    float           *yerr = NULL;
    Hv_Point        *pp, *ppstair;
    float           xscale, yscale;
    short           delR, x1, x2, y1, y2;
    int            *iarray;
    Boolean         more;
    float           yp1 = 0.0;
    float           yp2 = 0.0;
    float           *xxx, *yyy, bg;
    Boolean         flipped, fakeerr;  
    Hv_Segment     *errseg;
    Hv_PlotData     plotdata;
    void           (*nrfunc)(float, float*, int);
    short           y0, dummy;
    Boolean         histo;
    float           binsize = 0.0;
    
/* fit vars */
    
    int   peak;
    float a, b, q;
    float xdel, fxa, fxb, fya, fyb;
    short xs;
    
    plotdata = Hv_GetPlotData(plot);
    
/* is this a histogram? */
    
    histo = (plotdata->plottype == Hv_1DHISTO);
    
    Hv_nrerror = False;
    
    if ((curve == NULL) || (!(curve->display)))
	return;
    
    if (histo) {
	if ((curve->numbin1 < 1) || (curve->binned1 == NULL))
	    return;
    }
    else {
	if ((curve->npts < 1) || (curve->x == NULL) || (curve->y == NULL))
	    return;
    }
    
    TheCurve = curve;
    
/* for 1D histos, use number of bins */
    
    if (histo)
	n = curve->numbin1;
    else
	n = curve->npts;
    
    xscale = curve->xscale;
    yscale = curve->yscale;
    
    x = Hv_NewVector(n);
    y = Hv_NewVector(n);
    
    if (curve->xerr != NULL)
	xerr = Hv_NewVector(n);
    else
	xerr = NULL;
    
    if (curve->yerr != NULL)
	yerr = Hv_NewVector(n);
    else
	yerr = NULL;
    
    if (histo)
	binsize = (plotdata->histoxmax - plotdata->histoxmin)/curve->numbin1;
    
    for (i = 0; i < n; i++) {
	if (histo) {
	    x[i] = plotdata->histoxmin + (float)(i*binsize);
	    y[i] = (float)(curve->binned1[i]);
	}
	else {
	    x[i] = xscale*(curve->x[i]);
	    y[i] = yscale*(curve->y[i]);
	    if (curve->absval)
	      y[i] = (float)(fabs(y[i]));
	}
	
	if (xerr != NULL)
	    xerr[i] = xscale*(curve->xerr[i]);
	if (yerr != NULL)
	    yerr[i] = yscale*(curve->yerr[i]);
    }
    
/* convert the data to plot coordinates */
    
    pp = Hv_NewPoints(n);
    for (i = 0; i < n; i++)
	Hv_PlotToLocal(plot, &(pp[i].x), &(pp[i].y), x[i], y[i]);
    
    Hv_SetLineStyle(curve->thickness, curve->style);
    
    switch (curve->algorithm) {
    case Hv_NOLINES:
	break;
	
    case Hv_SIMPLECONNECT:
	Hv_DrawLines(pp, (short)n, curve->linecolor);
	break;
	
    case Hv_STAIRCONNECT:
	nstair = 2*n;
	ppstair = Hv_NewPoints(nstair);
	
	for (i = 0; i < n; i++) {
	    j = 2*i;
	    
	    if (i == (n-1))
		delR = (pp[i].x - pp[i-1].x)/2;
	    else
		delR = (pp[i+1].x - pp[i].x)/2;
	    
	    if (j == 0)
		Hv_SetPoint(ppstair+j,   (short)(pp[0].x - delR),   pp[i].y);
	    else
		Hv_SetPoint(ppstair+j,   ppstair[j-1].x,   pp[i].y);
	    Hv_SetPoint(ppstair+j+1, (short)(pp[i].x + delR), pp[i].y);
	}
	
	Hv_DrawLines(ppstair, (short)nstair, curve->linecolor);
	Hv_Free(ppstair);
	break;
	
    case Hv_SPLINE:
	iarray = Hv_NewIVector(n);
	TurningPoints(curve->x, iarray, n);
	
	i = 0;
	more = True;
	
	while (more) {
	    more = (iarray[i] != n-1);
	    
/* spline and draw each segment */
	    
	    if (i == 0) {
		nspline = iarray[0] + 1;
		xxx = x;
		yyy = y;
		yp1 = (float)(1.0e31);
		yp2 = (float)(1.0e31);
	    }
	    else {
		nspline = iarray[i] - iarray[i-1] + 1;
		xxx = x + iarray[i-1];
		yyy = y + iarray[i-1];
	    }
	    
	    if (nspline == 2) {
		Hv_PlotToLocal(plot, &x1, &y1, xxx[0], yyy[0]);
		Hv_PlotToLocal(plot, &x2, &y2, xxx[1], yyy[1]);
		Hv_DrawLine(x1, y1, x2, y2, curve->linecolor);
	    }
	    else if (nspline > 2) {
		spliney2 = Hv_NewVector(nspline);
		
/* check ordering */
		
		if (xxx[nspline-1] > xxx[0]) {
		    splinex = xxx;
		    spliney = yyy;
		    flipped = False;
		}
		else {
		    splinex = Hv_NewVector(nspline);
		    spliney = Hv_NewVector(nspline);
		    ReverseOrder(splinex, xxx, nspline);
		    ReverseOrder(spliney, yyy, nspline);
		    flipped = True;
		}
		
		Hv_Spline(splinex, spliney, nspline, yp1, yp2, spliney2);
		
		PlotSegment(plot,
			    splinex[0],
			    splinex[nspline-1],
			    curve->linecolor,
			    0.0,
			    (Hv_FloatFunction)PlotFunction);
		
		Hv_Free(spliney2);
		if (flipped) {
		    Hv_Free(splinex);
		    Hv_Free(spliney);
		}
	    }
	    
	    i++;
	}
	
	
	Hv_Free(iarray);
	break;
	
    case Hv_CURVEFIT:
	
	if (plotdata->usebackground) {
	    bg = plotdata->background;
	    for (i = 0; i < n; i++)
		y[i] -= bg;
	}
	else
	    bg = 0.0;
	
	xdel = (float)((plotdata->xmax - plotdata->xmin)/30.0);
	fxa = plotdata->xmin + xdel;
	fxb = plotdata->xmax - xdel;
	xs = plot->area->left+8;
	
	switch (curve->fittype) {
	case Hv_LINEFIT:
	    if (curve->dirtyfit) {
		if (yerr == NULL)
		    Hv_Fit(x, y, n, NULL, 0, &(TheCurve->intercept), &(TheCurve->slope),
			   &(TheCurve->siga), &(TheCurve->sigb), &(TheCurve->chisq), &q);
		else
		    Hv_Fit(x, y, n, yerr, 1, &(TheCurve->intercept), &(TheCurve->slope),
			   &(TheCurve->siga), &(TheCurve->sigb), &(TheCurve->chisq), &q);
		TheCurve->dirtyfit = False;
	    }
	    
	    b = TheCurve->slope;
	    a = TheCurve->intercept;
	    
	    if ((plotdata->xlog) || (plotdata->ylog))
		PlotSegment(plot,
			    fxa,
			    fxb,
			    curve->linecolor,
			    bg,
			    (Hv_FloatFunction)LineFunction);
	    else {
		fya = a + b*fxa;
		fyb = a + b*fxb;
		Hv_PlotToLocal(plot, &x1, &y1, fxa, fya);
		Hv_PlotToLocal(plot, &x2, &y2, fxb, fyb);
		Hv_DrawLine(x1, y1, x2, y2, curve->linecolor);
	    }
	    
	    break;
	    
	case Hv_EXPONENTIALFIT:
	    
/* exponential fit does NOT presently use error bars */
	    
	    if (curve->dirtyfit) {
		yyy = Hv_NewVector(n);
		for (i = 0; i < n; i++)
		    yyy[i] = CheckLog10(y[i]);
		Hv_Fit(x, yyy, n, NULL, 0, &a, &(TheCurve->slope), &(TheCurve->siga),
		       &(TheCurve->sigb), &(TheCurve->chisq), &q);
		TheCurve->intercept = (float)(pow((double)10.0, (double)a));
		Hv_Free(yyy);
		TheCurve->dirtyfit = False;
	    }
	    
	    a = TheCurve->intercept;
	    b = TheCurve->slope;
	    
	    PlotSegment(plot,
			fxa,
			fxb,
			curve->linecolor,
			bg,
			(Hv_FloatFunction)ExponentialFunction);
	    
	    
	    break;
	    
	case Hv_POLYFIT:  case Hv_LEGENDREFIT:
	    
	    if (curve->dirtyfit) {
		fakeerr = False;
		if (yerr == NULL) {
		    fakeerr = True;
		    yerr = Hv_NewVector(n);
		    for (i = 0; i < n; i++)
			yerr[i] = 1.0;
		}
		
		if (curve->fittype == Hv_LEGENDREFIT) {
		    TheCurve->ma = curve->legendreorder + 1;
		    nrfunc = nrflegendre;
		}
		else {
		    TheCurve->ma = curve->polyorder + 1;
		    nrfunc = nrfpoly;
		}
		
		Hv_Free(TheCurve->a);
		TheCurve->a = Hv_NewVector(TheCurve->ma);
		Hv_LFit(x, y, yerr, n, TheCurve->a, TheCurve->ma, &(TheCurve->chisq), nrfunc);
		
/* if an error occurred, try svdfit */
		
		if (Hv_nrerror) {
		    Hv_nrerror = False;
		    Hv_Println("NR routine lfit failed. Trying svdfit.");
		    Hv_SVDFit(x, y, yerr, n, TheCurve->a, TheCurve->ma, &(TheCurve->chisq), nrfunc);
		}
		
		if (fakeerr) {
		    Hv_Free(yerr);
		    yerr = NULL;
		}
	    }
	    
	    if (curve->fittype == Hv_LEGENDREFIT)
		PlotSegment(plot,
			    fxa,
			    fxb,
			    curve->linecolor,
			    bg,
			    (Hv_FloatFunction)LegendreFunction);
	    else
		PlotSegment(plot,
			    fxa,
			    fxb,
			    curve->linecolor,
			    bg,
			    (Hv_FloatFunction)PolyFunction);
	    
	    
	    break;
	    
	case Hv_GAUSSIANFIT:
	    if (curve->dirtyfit) {
		fakeerr = False;
		if (yerr == NULL) {
		    fakeerr = True;
		    yerr = Hv_NewVector(n);
		    for (i = 0; i < n; i++)
			yerr[i] = 1.0;
		}
		
		TheCurve->ma = 3*(curve->numgauss);
		Hv_Free(TheCurve->a);
		TheCurve->a = Hv_NewVector(TheCurve->ma);
		PeakPoint(x, y, n, &peak);
		
		for (i = 0; i < curve->numgauss; i++) {
		    j = 3*i;
		    TheCurve->a[j] = y[peak];
		    TheCurve->a[j+1] = x[peak];
		    TheCurve->a[j+2] = (float)(0.5*(fxb-fxa));
		}
		
		Hv_MRQMin(x,
			  y,
			  yerr,
			  n,
			  TheCurve->a,
			  TheCurve->ma,
			  &(TheCurve->chisq),
			  nrfgauss);
		
		if (fakeerr) {
		    Hv_Free(yerr);
		    yerr = NULL;
		}
		
	    }
	    
	    PlotSegment(plot,
			fxa,
			fxb,
			curve->linecolor,
			bg,
			(Hv_FloatFunction)GaussFunction);
	    
	    break;
	}
	
	if (plotdata->usebackground)
	    for (i = 0; i < n; i++)
		y[i] += bg;
	
	break;   /* end of alg = Hv_CURVEFIT */
	
    }  /* end of alg switch */
    
    Hv_SetLineStyle(0, Hv_SOLIDLINE);
    
/* error bars ? */
    
    if ((xerr != NULL) && (curve->usexerrs)) {
	if (plotdata->ebartops)
	    nseg = 3*n;
	else
	    nseg = n;
	
	errseg = (Hv_Segment *)Hv_Malloc(nseg*sizeof(Hv_Segment));
	
	for (i = 0; i < n; i++) {
	    Hv_PlotToLocal(plot, &x1, &y1, x[i]-xerr[i], y[i]);
	    Hv_PlotToLocal(plot, &x2, &y2, x[i]+xerr[i], y[i]);
	    Hv_SetSegment(errseg+i, x1, y1, x2, y2);
	    if (plotdata->ebartops) {
		Hv_SetSegment(errseg+i+n, x1, (short)(y1-4), x1, (short)(y1+4));
		Hv_SetSegment(errseg+i+(2*n), x2, (short)(y2-4), x2, (short)(y2+4));
	    }
	}
	
	Hv_DrawSegments(errseg, (short)nseg, curve->linecolor);
	Hv_Free(errseg);
    }
    
    if ((yerr != NULL) && (curve->useyerrs)) {
	if (plotdata->ebartops)
	    nseg = 3*n;
	else
	    nseg = n;
	
	errseg = (Hv_Segment *)Hv_Malloc(nseg*sizeof(Hv_Segment));
	
	for (i = 0; i < n; i++) {
	    Hv_PlotToLocal(plot, &x1, &y1, x[i], y[i]-yerr[i]);
	    Hv_PlotToLocal(plot, &x2, &y2, x[i], y[i]+yerr[i]);
	    Hv_SetSegment(errseg+i, x1, y1, x2, y2);
	    if (plotdata->ebartops) {
		Hv_SetSegment(errseg+i+n, (short)(x1-4), y1, (short)(x1+4), y1);
		Hv_SetSegment(errseg+i+(2*n), (short)(x2-4), y2, (short)(x2+4), y2);
	    }
	}
	
	Hv_DrawSegments(errseg, (short)nseg, curve->linecolor);
	Hv_Free(errseg);
    }
    
/* symbols? */
    
    if (curve->symbol != Hv_NOSYMBOL) {
	if (curve->symbol == Hv_POINTSYMBOL)
	    Hv_DrawPoints(pp, n, curve->symbolcolor);
	else if (curve->symbol == Hv_BIGPOINTSYMBOL)
	    Hv_DrawSymbolsOnPoints(pp,
				   n,
				   3,
				   curve->symbolcolor,
				   Hv_RECTSYMBOL);
	else
	    Hv_DrawSymbolsOnPoints(pp,
				   n,
				   curve->symbolsize,
				   curve->symbolcolor,
				   curve->symbol);
    }
    
/* spikes ? */
    
    if (curve->yspike) {
	Hv_PlotToLocal(plot, &dummy, &y0, x[0], 0.0);
	for (i = 0; i < n; i++) {
	    Hv_DrawLine(pp[i].x, y0, pp[i].x, pp[i].y, curve->linecolor);
	}
    }
    
    Hv_Free(pp);
    Hv_Free(x);
    Hv_Free(y);
    Hv_Free(xerr);
    Hv_Free(yerr);
}

/*----- TruncName --------*/

static void TruncName(char  *dname,
		      char  *sname,
		      int  n)
    
{
    int namelen;
    
    namelen = Hv_iMin(n, strlen(sname));
    strncpy(dname, sname, namelen);
    dname[namelen] = '\0';
}


/*------- Hv_BasePlotSetup --------*/

static void   Hv_BasePlotSetup(Hv_View View)
    
{
    Hv_Rect   *hotrect = View->hotrect;
    
    Hv_InitPlotViewData(View);
    InitPlotViewItems(View);
    
/* add the standard set of tools */
    
    Hv_AddDrawingTools(View,
		       (short)(hotrect->right+10),
		       (short)(hotrect->top-3),
		       Hv_HORIZONTAL,
		       Hv_SHIFTXONRESIZE,
		       Hv_lightSeaGreen,
		       True,
		       &BaseLeft,
		       &BaseRight);
}

/*------- Hv_PlotSetup --------*/

static void   Hv_PlotSetup(Hv_View View)
    
{
    Hv_Item   Item, Camera;
    Hv_Item   rightmost;
    short     x, y;
    
    Hv_InitPlotViewData(View);
    InitPlotViewItems(View);
    
    rightmost = Hv_AddPlotBigButtons(View);
    
/* add the standard set of tools */
    
    
    x = rightmost->area->right+1;
    y = rightmost->area->top;

    Hv_StandardTools(View, x, y,
		   Hv_HORIZONTAL, Hv_DONOTHINGONRESIZE,
		   Hv_ALLTOOLS, &Camera, &rightmost);

    
    Item = Hv_AddPlotFeedback(View, rightmost);
}

/*------- Hv_PlotFeedback --------*/

static void  Hv_PlotFeedback(Hv_View       View,
			     Hv_Point      pp)
    
{
    char         text[50];
    char         xtext[20], ytext[20];
    float        fx, fy;
    Hv_Item      plot;
    Hv_PlotData  plotdata;
    int          precision;
    
    plot = Hv_PointInPlotItem(View, pp);
    plotdata = Hv_GetPlotData(plot);
    
    if (plot == NULL) {
	Hv_ChangeFeedback(View, Hv_XYPLOTDATA, " ");        
	return;
    }
    
    Hv_LocalToPlot(plot, pp.x, pp.y, &fx, &fy);
    
    precision = plotdata->xprecision+3;
    sprintf(xtext, "%.*g", precision, fx);
    
    precision = plotdata->yprecision+3;
    sprintf(ytext, "%.*g", precision, fy);
    
    sprintf(text, "(%s, %s)", xtext, ytext);
    Hv_ChangeFeedback(View, Hv_XYPLOTDATA, text);
}


/*------- Hv_AddPlotBigButtons ------*/

static Hv_Item   Hv_AddPlotBigButtons(Hv_View View)
    
{
    short             left, top;
    Hv_Item           Item;
    Hv_PlotViewData   pvdata;
    static Boolean    veryfirst = True;
    
    left = View->hotrect->left-4;
    top  = View->hotrect->top-29;
    
    pvdata = Hv_GetPlotViewData(View);
    
    Item = Hv_VaCreateItem(View,
			   Hv_RESIZEPOLICY,      Hv_DONOTHINGONRESIZE,
			   Hv_DRAWCONTROL,       Hv_LIGHTWEIGHT,
			   Hv_TYPE,              Hv_BUTTONITEM,
			   Hv_LEFT,              left,
			   Hv_TOP,               top,
			   Hv_SINGLECLICK,       Hv_LoadPlotData,
			   Hv_LABEL,             "Read",
			   Hv_FONT,              Hv_fixed2,
			   Hv_BALLOON,           "Read saved plot(s)",
			   Hv_WIDTH,             Hv_BIGBUTTONWIDTH,
			   Hv_HEIGHT,            Hv_BIGBUTTONHEIGHT,
			   NULL);
    
    if ((veryfirst) && (Hv_demoFile != NULL)) {
	Item = StandardBigButton(View, "Demo",
				 "Display a sample plot.", Hv_DemoButtonCallback);
	Item->userdraw = FirstTimeDraw;
	veryfirst = False;
    }
    
    Item = StandardBigButton(View, "Save", "Save plot(s) on this view.", Hv_SaveCB);
    
    Hv_SetItemSensitivity(Item, False, False);
    
    pvdata->savebutton = Item;
    return Item;
}

/*------ FirstTimeDraw -------*/

static void FirstTimeDraw(Hv_Item  Item,
			  Hv_Region region)
    
/*------------------------
  A very dirty trick to get the first view to display
  a file passed as a command line argument
  --------------------------*/
    
{
    FILE    *fp;
    char    *message;
    
/* if any left over args, treat as a file */
    
    if (Hv_leftOverArgs) {
	if ((fp = Hv_PrepareFile(Hv_leftOverArgs)) != NULL)
	    Hv_ReadPlotFile(Item->view, fp, Hv_leftOverArgs);
	else {
	    message = (char *)Hv_Malloc(strlen(Hv_leftOverArgs) + 40);
	    strcpy(message, "Ignored command line args: ");
	    strcat(message, Hv_leftOverArgs);
	    Hv_Warning(message);
	    Hv_Free(message);
	}
	Hv_Free(Hv_leftOverArgs);
	Hv_leftOverArgs = NULL;
    }
    
    
    Item->userdraw = NULL;
}

/*------ Hv_SaveCB ------*/

static void Hv_SaveCB(Hv_Event hvevent)
    
/* save a .plt file */
    
{
    char      *fname;
    Hv_Item    Item = hvevent->item;
    Hv_View    View = hvevent->view;
    FILE      *fp;
    
    Hv_ToggleButton(Item);
    
    fname = Hv_FileSelect("hvplot filename", "*.plt", "myplot.plt");
    
    if (Hv_CheckFileExists(fname)) {
	fp = fopen(fname, "w");
	Hv_Free(fname);
	
	if (fp != NULL)
	    Hv_WritePlotFile(View, fp);
	fclose(fp);
    }
    
    Hv_ToggleButton(Item);
}


/*------- Hv_AddPlotFeedback ------*/

static Hv_Item Hv_AddPlotFeedback(Hv_View   View,
				  Hv_Item   placementitem)
    
{
    short     x, y;
    char      *text0  = " (x, y): ";
    
    Hv_Item    Item;
    
    x = placementitem->area->right+3;
    y = placementitem->area->top-1;
    
    Item = Hv_VaCreateItem(View,
			   Hv_FEEDBACKDATACOLS,    20,
			   Hv_TYPE,                Hv_FEEDBACKITEM,
			   Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
			   Hv_LEFT,                x,
			   Hv_TOP,                 y,
			   Hv_RESIZEPOLICY,        Hv_DONOTHINGONRESIZE,
		  	   Hv_FILLCOLOR,           Hv_gray15,
			   NULL);
    
/*stuff the feedback labels*/
    
    Hv_AddFeedbackEntry(Item, Hv_XYPLOTDATA, text0,  Hv_fixed2, Hv_black);

	Item->area->bottom = placementitem->area->bottom + 1;
	Hv_FixRectWH(Item->area);
    return Item;
}


/*-------- Hv_GetCornerRects -------*/

static void  Hv_GetCornerRects(Hv_Rect  *area,
			       Hv_Rect  *rects[4],
			       short   slop)
    
{
    short    l, t, r, b;
    
    Hv_GetRectLTRB(area, &l, &t, &r, &b);
    
    Hv_SetRect(rects[0], l,      t,      slop, slop);
    Hv_SetRect(rects[1], (short)(r-slop), t,      slop, slop);
    Hv_SetRect(rects[2], (short)(r-slop), (short)(b-slop), slop, slop);
    Hv_SetRect(rects[3], l,      (short)(b-slop), slop, slop);
}

/*-------- Hv_PlotToPSLocal -------*/

static void  Hv_PlotToPSLocal(Hv_Item  plot,
			      short   *x,
			      short   *y,
			      float   px,
			      float   py)
    
{
    float        rx, ry, xx, yy;
    short        xoff, yoff;
    Hv_Rect      local;
    Hv_PlotData  plotdata;
    float        w,h;
    float        xmin, xmax, ymin, ymax;
    
    plotdata = Hv_GetPlotData(plot);
    
    Hv_LocalRectToPSLocalRect(plot->area, &local);
    
    xmin = plotdata->xmin;
    xmax = plotdata->xmax;
    ymin = plotdata->ymin;
    ymax = plotdata->ymax;
    
/* need to correct for log plots */
    
    if (plotdata->xlog) {
	xmin = CheckLog10(xmin);
	xmax = CheckLog10(xmax);
	px = CheckLog10(px);
    }
    if (plotdata->ylog) {
	ymin = CheckLog10(ymin);
	ymax = CheckLog10(ymax);
	py = CheckLog10(py);
    }
    
    w = xmax - xmin;
    h = ymax - ymin;
    
    xoff = local.left;
    yoff = local.top;
    
    rx = ((float)local.width)/w;
    ry = ((float)local.height)/h;
    
    xx = xoff + rx*(px - xmin);
    yy = yoff + ry*(ymax - py);
    
    *x = Hv_InRange(xx);
    *y = Hv_InRange(yy);
    
}

/*------- Hv_CheckPlotEnlarge ---------*/

static void Hv_CheckPlotEnlarge(Hv_Item    Item,
				Hv_Point   StartPP,
				Boolean    ShowCorner,
				Boolean   *enlarged)
    
{
    Hv_View       View = Item->view;
    Hv_Rect       oldarea;
    Hv_PlotData   plotdata;
    short         dl, dt, dr, db;
    Hv_Item       legend, title, xlab, ylab, xtick, ytick;
    Hv_Region     oldregion = NULL;
    Hv_Region     newregion = NULL;
    Hv_Region     totalregion = NULL;
    
    
    plotdata = Hv_GetPlotData(Item);
    
    Hv_BigThreeItems(Item, &title, &xlab, &ylab);
    
    xtick = plotdata->xtickitem;
    ytick = plotdata->ytickitem;
    legend = plotdata->legenditem;
    
    Hv_CopyRect(&oldarea, Item->area);
    oldregion = Hv_TotalPlotRegion(Item);
    
    Hv_updateAfterEnlarge = False;
    Hv_CheckWorldRectEnlarge(Item, StartPP, ShowCorner, enlarged);
    Hv_updateAfterEnlarge = True;
    
    if (*enlarged) {
	dl = Item->area->left - oldarea.left;
	dt = Item->area->top - oldarea.top;
	dr = Item->area->right - oldarea.right;
	db = Item->area->bottom - oldarea.bottom;
	
	if (dl != 0) {
	    if (legend) legend->area->left += dl;
	    if (ylab)   ylab ->area->left += dl;
	    if (title)  title->area->left += dl/2;
	    if (xtick)  xtick->area->left += dl;
	    if (xtick)  xtick->area->width -= dl;
	    if (ytick)  ytick->area->left += dl;
	}
	if (dt != 0) { 
	    if (legend) legend->area->top += dt;
	    if (title)  title->area->top += dt;
	    if (ylab)   ylab->area->top += dt/2;
	    if (ytick)  ytick->area->top += dt;
	    if (ytick)  ytick->area->height -= dt;
	}
	if (dr != 0) { 
	    if (title)  title->area->left += dr/2;
	    if (xlab)   xlab->area->left += dr/2;
	    if (xtick)  xtick->area->width += dr;
	}
	if (db != 0) {
	    if (xlab)   xlab->area->top += db;
	    if (ylab)   ylab->area->top += db/2;
	    if (xtick)  xtick->area->top += db;
	    if (ytick)  ytick->area->height += db;
	}
	
	if (title)  Hv_FixRectRB(title->area);
	if (xlab)   Hv_FixRectRB(xlab->area);
	if (ylab)   Hv_FixRectRB(ylab->area);
	if (xtick)  Hv_FixRectRB(xtick->area);
	if (ytick)  Hv_FixRectRB(ytick->area);
	
/* check attachments */
	
	Hv_CheckPlotAttachments(Item);
	
	newregion = Hv_TotalPlotRegion(Item);
	totalregion = Hv_UnionRegion(oldregion, newregion);
	Hv_DrawView(View, totalregion);
    }
    
    Hv_DestroyRegion(oldregion);
    Hv_DestroyRegion(newregion);
    Hv_DestroyRegion(totalregion);
}

/*------- StandardBigButton --------*/

static Hv_Item StandardBigButton(Hv_View     View,
				 char      *label,
				 char      *balloon,
				 Hv_FunctionPtr callback)
    
{
    Hv_Item   Item;
    
    Item = Hv_VaCreateItem(View,
			   Hv_RESIZEPOLICY,      Hv_DONOTHINGONRESIZE,
			   Hv_TYPE,              Hv_BUTTONITEM,
			   Hv_DRAWCONTROL,       Hv_LIGHTWEIGHT,
			   Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
			   Hv_SINGLECLICK,       callback,
			   Hv_LABEL,             label,
			   Hv_FONT,              Hv_fixed2,
			   Hv_BALLOON,           balloon,
			   Hv_WIDTH,             Hv_BIGBUTTONWIDTH,
			   Hv_HEIGHT,            Hv_BIGBUTTONHEIGHT,
			   NULL);
    
    return Item;
}


/*------ CheckLog10 ------*/

static float  CheckLog10(float x)
    
{
    if (x > 0.0)
	return (float)log10(x);
    else
	return (float)(-1.0e30);
}

/*------ ReverseOrder ------*/

static void ReverseOrder(float  *darray,
			 float  *sarray,
			 int   n)
    
{
    int  nm1;
    int  i;
    
    if ((darray == NULL) || (sarray == NULL) || (n < 2))
	return;
    
    nm1 = n - 1;
    
    for (i = 0; i < n; i++) {
	darray[i] = sarray[nm1 - i];
    }
    
}

/*------ PeakPoint ------*/

static void PeakPoint(float *x,
		      float *y,
		      int  n,
		      int  *pp)
    
{
    int     i;
    float   ymax = (float)(-1.0e30);
    
    for (i = 0; i < n; i++)
	if (y[i] > ymax) {
	    *pp = i;
	    ymax = y[i];
	}
}


/*-------- VerticalLine -------*/

static short  VerticalLine(Hv_Item  plot,
			   float   xval,
			   short   color)
    
{
    Hv_PlotData   plotdata;
    short  x, y1, y2;
    
    if (plot == NULL)
	return -32000;
    
    plotdata = Hv_GetPlotData(plot);
    if ((plotdata == NULL) || (xval < plotdata->xmin) || (xval > plotdata->xmax))
	return -32000;
    
    Hv_PlotToLocal(plot, &x, &y1, xval, plotdata->ymin);
    Hv_PlotToLocal(plot, &x, &y2, xval, plotdata->ymax);
    Hv_DrawLine(x, y1, x, y2, color);
    return x;
}


/*-------- HorizontalLine ------*/

static short  HorizontalLine(Hv_Item  plot,
			     float   yval,
			     short   color)
    
{
    Hv_PlotData   plotdata;
    short  x1, x2, y;
    
    if (plot == NULL)
	return -32000;
    
    plotdata = Hv_GetPlotData(plot);
    if ((plotdata == NULL) || (yval < plotdata->ymin) || (yval > plotdata->ymax))
	return - 32000;
    
    Hv_PlotToLocal(plot, &x1, &y, plotdata->xmin, yval);
    Hv_PlotToLocal(plot, &x2, &y, plotdata->xmax, yval);
    Hv_DrawLine(x1, y, x2, y, color);
    return y;
}


/*-------- LegendItemEdit ---------*/

static void LegendItemEdit(Hv_Event hvevent)
    
/************************************
  merely redirects to the plot editor
  *************************************/
    
{
    hvevent->item = hvevent->item->parent;
    Hv_EditPlot(hvevent);
}

/*-------- LegendItemCB ---------*/

static void LegendItemCB(Hv_Event hvevent)
    
/************************************
  merely redirects to the plot's callback
  *************************************/
    
{
    hvevent->item = hvevent->item->parent;
    if (hvevent->item && hvevent->item->singleclick)
	hvevent->item->singleclick(hvevent);
}


/*-------- Hv_PlotItemCB -----------*/

void  Hv_PlotItemCB(Hv_Event hvevent)
    
{
    Hv_PlotViewData   pvdata;
    Hv_Item           oldactive;
    Hv_Item           newactive;
    Hv_Item           title, xlab, ylab;
    short             family,size,style;
    Hv_PlotData       plotdata = NULL;
    Hv_SliderData    *data;
    Hv_Region         rectreg = NULL;
    Hv_View           View = hvevent->view;
    Hv_CurveData      curve;
    char              text[11];
    float             fdel;
    
    short             desiredX, desiredY;
    
/*  Hv_DefaultSingleClickCallback(hvevent); */
    
    pvdata = Hv_GetPlotViewData(View);
    
    
    oldactive = pvdata->activeplot;
    newactive = hvevent->item;
    
/* if the active plot hasn't changed, or if the
   extra items for on-view editing haven't been created,
   immediately cave */
    
    if ((oldactive == newactive) || (pvdata->viewbox == NULL))
	return;
    
    if (pvdata->activename == NULL)
	return;
    
    Hv_CompoundStringXYFromArea(pvdata->activename->str, &desiredX,
				&desiredY, pvdata->activename->area, Hv_STRINGEXTRA);
    
    
    text[10] = '\0';
    pvdata->activeplot = newactive;
    
/* update active plot name if not null */
    
    Hv_DestroyString(pvdata->activename->str);
    
    if (newactive == NULL)
	pvdata->activename->str = Hv_CreateString("No active plot");
    else {
	Hv_BigThreeItems(newactive, &title, &xlab, &ylab);
	if ((title != NULL) && (title->str != NULL)) {
	    Hv_CopyString(&(pvdata->activename->str), title->str);
	    if (pvdata->activename->str != NULL) {
		Hv_FontToFontParameters(pvdata->activename->str->font, &family, &size, &style);
		if (size > Hv_FONTSIZE14) {
		    size = Hv_FONTSIZE14;
		    pvdata->activename->str->font = Hv_FontParametersToFont(family, size, style);
		}
	    }
	    pvdata->activename->str->strcol = Hv_black;
	    pvdata->activename->str->fillcol = -1;
	}
	else
	    pvdata->activename->str = Hv_CreateString("Active plot has no title");
	
    }
    
    
    Hv_SizeTextItem(pvdata->activename, desiredX, desiredY);
    
/* now the histo slider */
    
    if (pvdata->bin1slider != NULL) {
	plotdata = Hv_GetPlotData(pvdata->activeplot);
	
	
	if ((plotdata == NULL) || (plotdata->plottype != Hv_1DHISTO))
	    Hv_SetItemSensitivity(pvdata->bin1slider, False, False);
	else {
	    data = (Hv_SliderData  *)(pvdata->bin1slider->data);
	    data->valmin = 1;
	    
	    fdel = (float)((plotdata->histoxmax - plotdata->histoxmin)*2.0);
	    
	    if (fdel < 250.0)
		data->valmax = (short)fdel;
	    else
		data->valmax = 250;
	    
	    Hv_SetSliderCurrentValue(pvdata->bin1slider,
				     (short)plotdata->curves->numbin1,
				     False);
	    
	    Hv_SetItemSensitivity(pvdata->bin1slider, True, False);
	    
	}
	
    } /* end bin1 slider */
    
/* now the curve menubutton */

    
    Hv_RemoveMenuButtonEntry(pvdata->curvemenu, 0);

    if (plotdata != NULL) {
	for (curve = plotdata->curves; curve!=NULL; curve = curve->next) {
	    strncpy(text, curve->name, 10);
	    Hv_AddMenuButtonEntry(pvdata->curvemenu, text, Hv_fixed2,
				  Hv_black, Hv_MENUBUTTONCHOICE);
	}
	Hv_SetCurrentMenuButtonChoice(pvdata->curvemenu, plotdata->activecurve);
    }
    
/* redraw the entire edit on-view edit area */
    
    rectreg = Hv_RectRegion(pvdata->viewbox->area);
    Hv_OffScreenViewControlUpdate(View, rectreg);
    Hv_DestroyRegion(rectreg); 
}



/**
 * Hv_PlotZoomSelectCallback
 * @purpose  Callback when done rubber-banding for a zoom.
 * @param   dd   The drag data.
 * @local
 */

static void Hv_PlotZoomSelectCallback(Hv_DragData dd) {

  if ((dd == NULL) || (dd->view == NULL)) {
    fprintf(stderr, "Serious problem in Hv_PlotZoomSelectCallback.");
    return;
  }


  Hv_ZoomPlotToRect(dd->item, &(dd->finalrect));

}


#undef  Hv_BIGBUTTONWIDTH
#undef  Hv_BIGBUTTONHEIGHT
#undef  LEGENDITEM




