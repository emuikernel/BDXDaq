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

/* local prototypes */

static void  Hv_SetCurveDlogItems(void);

static void  Hv_GetCurveDlogItems(int  answer);

static void  Hv_CurveListCB(Hv_Widget  w);

static void  LogToggleCB(Hv_Widget w);

static void  Hv_EditPlotColors (Hv_Widget  w);

static void  Hv_EditCurvePattern(Hv_Widget  w);

static void  Hv_CheckSwitchLabels(Hv_Item  plot,
				  int    oldtype,
				  int    newtype);

static void  SetDlogItems(Hv_PlotData  plotdata);

static short GetTheFont(Hv_Widget      family,
			Hv_Widget      size,
			Hv_Widget      style,
			Hv_WidgetList  family_btns,
			Hv_WidgetList  size_fbtns,
			Hv_WidgetList  style_fbtns);

static void  SetTheFont(short          font,
			Hv_Widget      family,
			Hv_Widget      size,
			Hv_Widget      style,
			Hv_WidgetList  family_btns,
			Hv_WidgetList  size_btns,
			Hv_WidgetList  style_btns);


/* curve editing widgets */


static Hv_Item         ThePlot;
static Hv_PlotData     ThePlotData;
static Hv_CurveData    TheCurve;

static short           newaxescolor, newmajcolor, newmincolor;
static Hv_Widget       majcolorlabel, mincolorlabel;
static Hv_Widget       xlogbutton = NULL;
static Hv_Widget       axescolorlabel, ylogbutton;
static Hv_Widget       majxtick, majytick, minytick, minxtick;
static Hv_Widget       ebartops, fancysymbols, opaquelegend, attachlegend;
static Hv_Widget       attachtitle, attachxlab, attachylab;
static Hv_Widget       xnozoom, ynozoom, xzeroline, yzeroline;
static short           newpattern, newlinecolor, newsymbcolor;
static Hv_Widget       patternlabel, linecolorlabel, symbcolorlabel;
static Hv_Widget       curvename;
static Hv_Widget       curvelist;
static Hv_Widget       cxmin = NULL;
static Hv_Widget       cxmax = NULL;
static Hv_Widget       cymin = NULL;
static Hv_Widget       cymax = NULL;
static Hv_Widget       cshowfit, conlegend, cdisplay, cyspike, cabsval, cusexerrs, cuseyerrs;
static Hv_Widget       cthickness, csymbsize, cxscale, cyscale, cpolyorder, cnumgauss;
static Hv_Widget       clegendreorder;
static Hv_Widget       background, usebackground, showbackground, showfilename;

static Hv_Widget       xtietag, scinothi, scinotlow;

static Hv_Widget       stylemenu = NULL;
static Hv_WidgetList   sbuttons;
static int             num_sbuttons;

static Hv_Widget       symbolmenu = NULL;
static Hv_WidgetList   sybuttons;
static int             num_sybuttons;

static Hv_Widget       connectmenu;
static Hv_WidgetList   cbuttons;
static int             num_cbuttons;

static Hv_Widget       fitmenu;
static Hv_WidgetList   fbuttons;
static int             num_fbuttons;

static Hv_Widget       axesmenu;
static Hv_WidgetList   abuttons;
static int             num_abuttons;

static Hv_Widget       plotmenu;
static Hv_WidgetList   pbuttons;
static int             num_pbuttons;


/*-------- Hv_EditPlot -------*/

void   Hv_EditPlot(Hv_Event hvevent)
    
/* item editor for the plot  */
    
{
    Hv_Item                 Item = hvevent->item;
    
    static Hv_Widget        dialog = NULL;
    static Hv_Widget        uselegend, framelegend;
    static Hv_Widget        drawxaxisvalues, drawyaxisvalues;
    static Hv_Widget        drawmajxticks, drawmajyticks;
    static Hv_Widget        drawminxticks, drawminyticks;
    static Hv_Widget        drawmajxgrid, drawmajygrid;
    static Hv_Widget        drawminxgrid, drawminygrid;
    static Hv_Widget        drawminxval, drawminyval;
    static Hv_Widget        xprecision, yprecision;
    Hv_Widget               dummy, rowcol, rc0, rc, rc1, rc2, rc3, rc4;
    Hv_Widget               rc00, rc0a, rc0b;
    Hv_Widget               rca, rcc;
    float                   check;
    Boolean                 warned = False;
    Hv_CurveData            tcurve;
    char                    text[30];
    int                     answer;
    
    int                     oldtype, newtype;
    
    Hv_Region               oldregion = NULL;
    Hv_Region               newregion = NULL;
    Hv_Region               totalregion = NULL;
    
/* these variables are used in conjuction with
   the new dialog option menus that replaced all the
   sets of radio buttons 7/6/96 */
    
    Hv_Item                 title, xlab, ylab;
    
/* axes vals font */
    
    static Hv_Widget       afamily, asize, astyle;
    static Hv_WidgetList   afamily_btns, asize_btns, astyle_btns;
    
/* legend font */
    
    static Hv_Widget       lfamily, lsize, lstyle;
    static Hv_WidgetList   lfamily_btns, lsize_btns, lstyle_btns;
    
/* fit font */
    
    static Hv_Widget       fitfamily, fitsize, fitstyle;
    static Hv_WidgetList   fitfamily_btns, fitsize_btns, fitstyle_btns;
    
/* title font */
    
    static Hv_Widget       tfamily, tsize, tstyle;
    static Hv_WidgetList   tfamily_btns, tsize_btns, tstyle_btns;
    
/* x label font */
    
    static Hv_Widget       xfamily, xsize, xstyle;
    static Hv_WidgetList   xfamily_btns, xsize_btns, xstyle_btns;
    
/* ylabel font */
    
    static Hv_Widget       yfamily, ysize, ystyle;
    static Hv_WidgetList   yfamily_btns, ysize_btns, ystyle_btns;
    
    short                  labx, laby;
    
    if (Item == NULL)
	return;
    
    
    Hv_PlotItemCB(hvevent);
    
/* get the old region so we can redraw properly */
    
    oldregion = Hv_TotalPlotRegion(Item);
    
/* note the Item is actually the axes item */
    
    ThePlot = Item;  /* so that other routines can use it */
    ThePlotData = Hv_GetPlotData(ThePlot);
    TheCurve = NULL;
    
    if (!dialog) {
	Hv_VaCreateDialog(&dialog,
			  Hv_TITLE, " Plot Editor ",
			  Hv_WMIGNORECLOSE, False,
			  NULL);
	
	rowcol = Hv_DialogColumn(dialog, 0);
	
	
	rc = Hv_TightDialogRow(rowcol, 10);
	rc0 = Hv_DialogColumn(rc, 0);
	rc1 = Hv_DialogColumn(rc, 0);
	rc2 = Hv_DialogColumn(rc, 0);
	rc3 = Hv_DialogColumn(rc, 0);
	
	dummy = Hv_StandardLabel(rc0, "-- Styles for the Entire Plot", 100);
	
	plotmenu = Hv_CreatePlotTypePopup(rc0, Hv_fixed2);  /* plot type menu */
	Hv_GetOptionMenuButtons(plotmenu,  &pbuttons, &num_pbuttons);
	Hv_SetSensitivity(pbuttons[Hv_HBAR], False);
	Hv_SetSensitivity(pbuttons[Hv_VBAR], False);
	
	axesmenu = Hv_CreateAxesTypePopup(rc0, Hv_fixed2);  /* axes type menu */
	Hv_GetOptionMenuButtons(axesmenu,  &abuttons, &num_abuttons);
	
	dummy = Hv_SimpleDialogSeparator(rc0);
	
	rc00 = Hv_TightDialogRow(rc0, 0);
	rc0a = Hv_DialogColumn(rc00, 0);
	rc0b = Hv_DialogColumn(rc00, 0);
	
/* various colors */
	
	
	Hv_StandardLabel(rc0a, "Colors", -1);
	rcc = Hv_DialogTable(rc0a, 3, 2);
	axescolorlabel = Hv_SimpleColorLabel(rcc,  "XY Axes",(Hv_FunctionPtr)Hv_EditPlotColors);
	majcolorlabel  = Hv_SimpleColorLabel(rcc,  "Maj Grid", (Hv_FunctionPtr)Hv_EditPlotColors);
	mincolorlabel  = Hv_SimpleColorLabel(rcc,  "Min Grid", (Hv_FunctionPtr)Hv_EditPlotColors);
	
	Hv_StandardLabel(rc0b, "Attach", -1);
	attachlegend = Hv_SimpleToggleButton(rc0b,   "Legend");
	attachtitle = Hv_SimpleToggleButton(rc0b,   "Title");
	attachxlab  = Hv_SimpleToggleButton(rc0b,   "X Label");
	attachylab  = Hv_SimpleToggleButton(rc0b,   "Y Label");
	
/* sets of toggle buttons */
	
	dummy = Hv_StandardLabel(rc1, "Axes Related", -1);
	xlogbutton = Hv_StandardToggleButton(rc1,    "X log axis", LogToggleCB);
	ylogbutton = Hv_StandardToggleButton(rc1,    "Y log axis", LogToggleCB);
	drawxaxisvalues = Hv_SimpleToggleButton(rc1, "X axis vals");
	drawyaxisvalues = Hv_SimpleToggleButton(rc1, "Y axis vals");
	
	dummy = Hv_StandardLabel(rc1, "Zeroes & Mins", -1);
	xzeroline = Hv_SimpleToggleButton(rc1,  "ZeroX line");
	yzeroline = Hv_SimpleToggleButton(rc1,  "ZeroY line");
	drawminxval = Hv_SimpleToggleButton(rc1,   "Show min X");
	drawminyval = Hv_SimpleToggleButton(rc1,   "Show min Y");
	
	dummy = Hv_StandardLabel(rc2, "Tick Marks", -1);
	drawmajxticks = Hv_SimpleToggleButton(rc2, "Maj X ticks");
	drawmajyticks = Hv_SimpleToggleButton(rc2, "Maj Y ticks");
	drawminxticks = Hv_SimpleToggleButton(rc2, "Min X ticks");
	drawminyticks = Hv_SimpleToggleButton(rc2, "Min Y ticks");

	dummy = Hv_StandardLabel(rc2, "Grid Lines", -1);
	drawmajxgrid = Hv_SimpleToggleButton(rc2,  "Maj X grid");
	drawmajygrid = Hv_SimpleToggleButton(rc2,  "Maj Y grid");
	drawminxgrid = Hv_SimpleToggleButton(rc2,  "Min X grid");
	drawminygrid = Hv_SimpleToggleButton(rc2,  "Min Y grid");
	
	
	dummy = Hv_StandardLabel(rc3, "Legend Related", -1);
	uselegend = Hv_SimpleToggleButton(rc3,      "Show legend");
	framelegend = Hv_SimpleToggleButton(rc3,    "Frame legend");
	opaquelegend = Hv_SimpleToggleButton(rc3,   "Opaque legend");
	dummy = Hv_StandardLabel(rc3, "Miscellaneous", -1);
	showfilename = Hv_SimpleToggleButton(rc3,   "Show filename");
	xnozoom = Hv_SimpleToggleButton(rc3, "No X Zoom");
	ynozoom = Hv_SimpleToggleButton(rc3, "No Y Zoom");
	ebartops = Hv_SimpleToggleButton(rc3,       "Errbar \"tops\"");
	fancysymbols = Hv_SimpleToggleButton(rc3,   "Fancy symbols");
	
	
	dummy = Hv_SimpleDialogSeparator(rowcol);
	
	rc = Hv_TightDialogRow(rowcol, 0);
	rc1 = Hv_DialogColumn(rc, 0);
	rc2 = Hv_DialogColumn(rc, 0);
	rc3 = Hv_DialogColumn(rc, 0);
	
	
/* font selectors */
	
	Hv_FontDialogSuite(rc1, "values",
			   &afamily, &asize, &astyle,
			   &afamily_btns, &asize_btns, &astyle_btns);
	
	Hv_FontDialogSuite(rc1, "legend",
			   &lfamily, &lsize, &lstyle,
			   &lfamily_btns, &lsize_btns, &lstyle_btns);
	
	Hv_FontDialogSuite(rc1, " title",
			   &tfamily, &tsize, &tstyle,
			   &tfamily_btns, &tsize_btns, &tstyle_btns);
	
	Hv_FontDialogSuite(rc1, " X lab",
			   &xfamily, &xsize, &xstyle,
			   &xfamily_btns, &xsize_btns, &xstyle_btns);
	
	Hv_FontDialogSuite(rc1, " Y lab",
			   &yfamily, &ysize, &ystyle,
			   &yfamily_btns, &ysize_btns, &ystyle_btns);

	Hv_FontDialogSuite(rc1, "   fit",
			   &fitfamily, &fitsize, &fitstyle,
			   &fitfamily_btns, &fitsize_btns, &fitstyle_btns);
	
	
	
/* text editors */
	
	rca = Hv_DialogTable(rc2, 8, 0);
	cxmin = Hv_SimpleTextEdit(rca,      "Xaxis Min", NULL, 4);
	cxmax = Hv_SimpleTextEdit(rca,      "Xaxis Max", NULL, 4);
	cymin = Hv_SimpleTextEdit(rca,      "Yaxis Min", NULL, 4);
	cymax = Hv_SimpleTextEdit(rca,      "Yaxis Max", NULL, 4);
	scinotlow  = Hv_SimpleTextEdit(rca, "SciLoLim", NULL, 4);
	scinothi   = Hv_SimpleTextEdit(rca, "SciHiLim", NULL, 4);
	xprecision = Hv_SimpleTextEdit(rca, "X Precis", NULL, 4);
	yprecision = Hv_SimpleTextEdit(rca, "Y Precis", NULL, 4);
	
	rca = Hv_DialogTable(rc3, 6, 0);
	majxtick = Hv_SimpleTextEdit(rca,   "X MajTk", NULL, 4);
	minxtick = Hv_SimpleTextEdit(rca,   "X MinTk", NULL, 4);
	majytick = Hv_SimpleTextEdit(rca,   "Y MajTk", NULL, 4);
	minytick = Hv_SimpleTextEdit(rca,   "Y MinTk", NULL, 4);
	xtietag    = Hv_SimpleTextEdit(rca, "X Tietag", NULL, 4);
	background = Hv_SimpleTextEdit(rca, "BackGrnd", NULL, 4);
	usebackground = Hv_SimpleToggleButton(rc3,  "Use backgrnd");
	showbackground = Hv_SimpleToggleButton(rc3, "Show backgrnd");
	
	
/*========= Curve Editing Section =========*/
	
	dummy = Hv_StandardLabel(rowcol, "-- Styles for the Selected Curve --------------", 100);
	
	rc = Hv_TightDialogRow(rowcol, 1);
	
	rc1 = Hv_DialogColumn(rc, 0);
	rc2 = Hv_DialogColumn(rc, 0);
	rc3 = Hv_DialogColumn(rc, 0);
	rc4 = Hv_DialogColumn(rc, 0);
	rca = Hv_DialogTable(rc4, 7, 0);
	
	curvelist = Hv_VaCreateDialogItem(rc1,
					  Hv_TYPE,            Hv_LISTDIALOGITEM,
					  Hv_NUMITEMS,        0,
					  Hv_NUMVISIBLEITEMS, 5,
					  Hv_CALLBACK,        Hv_CurveListCB,
					  NULL);
	
	curvename = Hv_SimpleTextEdit(rc1, "name (on legend)", NULL, 4);
	
	dummy = Hv_StandardLabel(rc1, "Colors/Pattern", -1);
	rcc = Hv_DialogTable(rc1, 2, 1);
	linecolorlabel = Hv_SimpleColorLabel(rcc,  "Line   ", (Hv_FunctionPtr)Hv_EditPlotColors);
	symbcolorlabel = Hv_SimpleColorLabel(rcc,  "Symbol ", (Hv_FunctionPtr)Hv_EditPlotColors);
	
	dummy = Hv_StandardLabel(rc2, "Controls", -1);
	cusexerrs  = Hv_SimpleToggleButton(rc2, "X Errors");
	cuseyerrs  = Hv_SimpleToggleButton(rc2, "Y Errors");
	cdisplay   = Hv_SimpleToggleButton(rc2, "Display");
	conlegend  = Hv_SimpleToggleButton(rc2, "On Legend");
	cshowfit   = Hv_SimpleToggleButton(rc2, "Fit Results");
	cyspike    = Hv_SimpleToggleButton(rc2, "Y Spikes");
	cabsval    = Hv_SimpleToggleButton(rc2, "Abs Value");
	
	rcc = Hv_DialogRow(rc2, 1);
	patternlabel   = Hv_SimplePatternLabel(rcc, "pattern",
					       Hv_EditCurvePattern);
	
/* some menu buttons */

	stylemenu = Hv_CreateLineStylePopup(rc3, Hv_fixed2);
	Hv_GetOptionMenuButtons(stylemenu,  &sbuttons, &num_sbuttons);
	
	symbolmenu = Hv_CreateSymbolPopup(rc3, Hv_fixed2);
	Hv_GetOptionMenuButtons(symbolmenu,  &sybuttons, &num_sybuttons);

	connectmenu = Hv_CreateConnectionTypePopup(rc3, Hv_fixed2);
	Hv_GetOptionMenuButtons(connectmenu,  &cbuttons, &num_cbuttons);
	
	fitmenu = Hv_CreateFitTypePopup(rc3, Hv_fixed2);
	Hv_GetOptionMenuButtons(fitmenu,  &fbuttons, &num_fbuttons);
		
		
	
	csymbsize      = Hv_SimpleTextEdit(rca,   "Symb Size", NULL, 4);
	cthickness     = Hv_SimpleTextEdit(rca,   "Line Size", NULL, 4);
	cxscale        = Hv_SimpleTextEdit(rca,   "X Scale ",   NULL, 4);
	cyscale        = Hv_SimpleTextEdit(rca,   "Y Scale ",   NULL, 4);
	cpolyorder     = Hv_SimpleTextEdit(rca,   "Poly Ord ", NULL, 4);
	clegendreorder = Hv_SimpleTextEdit(rca,   "Legde Ord", NULL, 4);
	cnumgauss      = Hv_SimpleTextEdit(rca,   "Num Gauss", NULL, 4);


	dummy = Hv_StandardActionButtons(rowcol, 20, Hv_ALLACTIONS);
    }
    
/* the dialog has been created */
    
/* remove existing list items from the curve selector; add those for
   this plot */
    
    Hv_BigThreeItems(Item, &title, &xlab, &ylab);
    
    Hv_DeleteAllListItems(curvelist);
    
    for (tcurve = ThePlotData->curves; tcurve != NULL; tcurve = tcurve->next) {
	strncpy(text, tcurve->name, 17);
	text[Hv_iMin(17, strlen(tcurve->name))] = '\0';
	Hv_AddListItem(curvelist, text, tcurve == ThePlotData->curves);
    }
    
/* set all the menus to their current values */
    
    Hv_SetOptionMenu(plotmenu,  pbuttons, (int)(ThePlotData->plottype));
    Hv_SetOptionMenu(axesmenu,  abuttons, (int)(ThePlotData->axesstyle));
    
/* set the fonts */
    
    
    SetTheFont(ThePlotData->axesfont, afamily, asize, astyle,
	       afamily_btns, asize_btns, astyle_btns);
    
    SetTheFont(ThePlotData->legendfont, lfamily, lsize, lstyle,
	       lfamily_btns, lsize_btns, lstyle_btns);
    
    SetTheFont(ThePlotData->fitresultfont, fitfamily, fitsize, fitstyle,
	       fitfamily_btns, fitsize_btns, fitstyle_btns);
    
    if ((title != NULL) && (title->str != NULL))
	SetTheFont(title->str->font, tfamily, tsize, tstyle,
		   tfamily_btns, tsize_btns, tstyle_btns);
    
    if ((xlab != NULL) && (xlab->str != NULL))
	SetTheFont(xlab->str->font, xfamily, xsize, xstyle,
		   xfamily_btns, xsize_btns, xstyle_btns);
    
    if ((ylab != NULL) && (ylab->str != NULL))
	SetTheFont(ylab->str->font, yfamily, ysize, ystyle,
		   yfamily_btns, ysize_btns, ystyle_btns);
    
/* draw the axes ? */
    
    Hv_SetToggleButton(drawxaxisvalues, ThePlotData->drawxaxisvalues);
    Hv_SetToggleButton(drawyaxisvalues, ThePlotData->drawyaxisvalues);
    
    Hv_SetToggleButton(uselegend,    ThePlotData->uselegend);
    Hv_SetToggleButton(framelegend,  ThePlotData->framelegend);
    Hv_SetToggleButton(ebartops,     ThePlotData->ebartops);
    Hv_SetToggleButton(opaquelegend, ThePlotData->opaquelegend);
    Hv_SetToggleButton(attachlegend, ThePlotData->attachlegend);
    Hv_SetToggleButton(attachtitle,  ThePlotData->attachtitle);
    Hv_SetToggleButton(attachxlab,   ThePlotData->attachxlab);
    Hv_SetToggleButton(attachylab,   ThePlotData->attachylab);
    Hv_SetToggleButton(xzeroline,    ThePlotData->xzeroline);
    Hv_SetToggleButton(yzeroline,    ThePlotData->yzeroline);
    
    Hv_SetToggleButton(fancysymbols, ThePlotData->fancysymbols);
    Hv_SetToggleButton(usebackground, ThePlotData->usebackground);
    Hv_SetToggleButton(showbackground, ThePlotData->showbackground);
    Hv_SetToggleButton(showfilename, ThePlotData->showfilename);
    
    Hv_SetToggleButton(xlogbutton, ThePlotData->xlog);
    Hv_SetToggleButton(ylogbutton, ThePlotData->ylog);
    
    Hv_SetToggleButton(drawmajxticks, ThePlotData->drawmajxticks);
    Hv_SetToggleButton(drawmajyticks, ThePlotData->drawmajyticks);
    Hv_SetToggleButton(drawminxticks, ThePlotData->drawminxticks);
    Hv_SetToggleButton(drawminyticks, ThePlotData->drawminyticks);
    
    Hv_SetToggleButton(drawmajxgrid, ThePlotData->drawmajxgrid);
    Hv_SetToggleButton(drawmajygrid, ThePlotData->drawmajygrid);
    Hv_SetToggleButton(drawminxgrid, ThePlotData->drawminxgrid);
    Hv_SetToggleButton(drawminygrid, ThePlotData->drawminygrid);
    
    Hv_SetToggleButton(drawminxval, ThePlotData->drawminxval);
    Hv_SetToggleButton(drawminyval, ThePlotData->drawminyval);
    
    newaxescolor = ThePlotData->axescolor;
    newmajcolor = ThePlotData->majgridcolor;
    newmincolor = ThePlotData->mingridcolor;
    
    Hv_ChangeLabelColors(axescolorlabel, -1, ThePlotData->axescolor);
    Hv_ChangeLabelColors(majcolorlabel,  -1, ThePlotData->majgridcolor);
    Hv_ChangeLabelColors(mincolorlabel,  -1, ThePlotData->mingridcolor);
    
    Hv_SetDeleteItem(dialog, ThePlot);  /* actually deleting the entire plot ! */
    
    Hv_SetIntText(majxtick, ThePlotData->majxtick);
    Hv_SetIntText(majytick, ThePlotData->majytick);
    Hv_SetIntText(minxtick, ThePlotData->minxtick);
    Hv_SetIntText(minytick, ThePlotData->minytick);
    Hv_SetIntText(xprecision, ThePlotData->xprecision);
    Hv_SetIntText(yprecision, ThePlotData->yprecision);
    
    
/* some dlogs items that might have to be updated dynamically
   are collected into the following routine */
    
    SetDlogItems(ThePlotData);
    
    Hv_SetIntText(xtietag,      ThePlotData->xtietag);
    Hv_SetFloatText(scinotlow,  ThePlotData->scilowlim, 101);
    Hv_SetFloatText(scinothi,   ThePlotData->scihilim,  101);
    Hv_SetFloatText(background, ThePlotData->background,  102);
    
    Hv_SetCurveDlogItems();
    
    Hv_PlotTypeSensitivities();
    
    while ((answer = Hv_DoDialog(dialog, NULL)) > 0) {
	
/* The font menus area bit tricky since there is more
   than one so can't use the Hv_hot variables */
	
	ThePlotData->axesfont = GetTheFont(afamily, asize, astyle,
					   afamily_btns, asize_btns, astyle_btns);
	
	ThePlotData->legendfont = GetTheFont(lfamily, lsize, lstyle,
					     lfamily_btns, lsize_btns, lstyle_btns);
	
	ThePlotData->fitresultfont = GetTheFont(fitfamily, fitsize, fitstyle,
					     fitfamily_btns, fitsize_btns, fitstyle_btns);
	
	if ((title != NULL) && (title->str != NULL)) {
	    title->str->font = GetTheFont(tfamily, tsize, tstyle,
					  tfamily_btns, tsize_btns, tstyle_btns);
	    Hv_CompoundStringXYFromArea(title->str, &labx, &laby, title->area, Hv_STRINGEXTRA);
	    Hv_SizeTextItem(title, labx, laby);
	}
	
	if ((xlab != NULL) && (xlab->str != NULL)) {
	    xlab->str->font = GetTheFont(xfamily, xsize, xstyle,
					 xfamily_btns, xsize_btns, xstyle_btns);
	    Hv_CompoundStringXYFromArea(xlab->str, &labx, &laby, xlab->area, Hv_STRINGEXTRA);
	    Hv_SizeTextItem(xlab, labx, laby);
	}
	
	if ((ylab != NULL) && (ylab->str != NULL)) {
	    ylab->str->font = GetTheFont(yfamily, ysize, ystyle,
					 yfamily_btns, ysize_btns, ystyle_btns);
	    Hv_CompoundStringXYFromArea(ylab->str, &labx, &laby, ylab->area, Hv_STRINGEXTRA);
	    Hv_SizeTextItem(ylab, labx, laby);
	}
	
	ThePlotData->drawminxval = Hv_GetToggleButton(drawminxval);
	ThePlotData->drawminyval = Hv_GetToggleButton(drawminyval);
	
	ThePlotData->drawmajxticks = Hv_GetToggleButton(drawmajxticks);
	ThePlotData->drawmajyticks = Hv_GetToggleButton(drawmajyticks);
	ThePlotData->drawminxticks = Hv_GetToggleButton(drawminxticks);
	ThePlotData->drawminyticks = Hv_GetToggleButton(drawminyticks);
	
	ThePlotData->drawxaxisvalues = Hv_GetToggleButton(drawxaxisvalues);
	ThePlotData->drawyaxisvalues = Hv_GetToggleButton(drawyaxisvalues);
	
	ThePlotData->drawmajxgrid   = Hv_GetToggleButton(drawmajxgrid);
	ThePlotData->drawmajygrid   = Hv_GetToggleButton(drawmajygrid);
	ThePlotData->drawminxgrid   = Hv_GetToggleButton(drawminxgrid);
	ThePlotData->drawminygrid   = Hv_GetToggleButton(drawminygrid);
	
	ThePlotData->xlog           = Hv_GetToggleButton(xlogbutton);
	ThePlotData->ylog           = Hv_GetToggleButton(ylogbutton);
	ThePlotData->uselegend      = Hv_GetToggleButton(uselegend);
	ThePlotData->framelegend    = Hv_GetToggleButton(framelegend);
	ThePlotData->ebartops       = Hv_GetToggleButton(ebartops);
	ThePlotData->xnozoom        = Hv_GetToggleButton(xnozoom);
	ThePlotData->ynozoom        = Hv_GetToggleButton(ynozoom);
	ThePlotData->opaquelegend   = Hv_GetToggleButton(opaquelegend);
	ThePlotData->attachlegend   = Hv_GetToggleButton(attachlegend);
	ThePlotData->attachtitle    = Hv_GetToggleButton(attachtitle);
	ThePlotData->attachxlab     = Hv_GetToggleButton(attachxlab);
	ThePlotData->attachylab     = Hv_GetToggleButton(attachylab);
	ThePlotData->xzeroline      = Hv_GetToggleButton(xzeroline);
	ThePlotData->yzeroline      = Hv_GetToggleButton(yzeroline);
	ThePlotData->fancysymbols   = Hv_GetToggleButton(fancysymbols);
	ThePlotData->usebackground  = Hv_GetToggleButton(usebackground);
	ThePlotData->showbackground = Hv_GetToggleButton(showbackground);
	
	ThePlotData->showfilename = Hv_GetToggleButton(showfilename);
	if (ThePlotData->filenameitem != NULL) {
	    if (ThePlotData->showfilename)
		Hv_ClearBit(&(ThePlotData->filenameitem->drawcontrol), Hv_DONTDRAW);
	    else
		Hv_SetBit(&(ThePlotData->filenameitem->drawcontrol), Hv_DONTDRAW);
	}
	
	ThePlotData->axescolor  = newaxescolor;
	ThePlotData->majgridcolor  = newmajcolor;
	ThePlotData->mingridcolor  = newmincolor;
	
	if (ThePlotData->uselegend)
	    Hv_ClearBit(&(ThePlotData->legenditem->drawcontrol), Hv_NOPOINTERCHECK);
	else
	    Hv_SetBit(&(ThePlotData->legenditem->drawcontrol), Hv_NOPOINTERCHECK);
	
	
	ThePlotData->majxtick = Hv_iMax(0, Hv_iMin(12, Hv_GetIntText(majxtick)));
	ThePlotData->majytick = Hv_iMax(0, Hv_iMin(12, Hv_GetIntText(majytick)));
	ThePlotData->minxtick = Hv_iMax(0, Hv_iMin(12, Hv_GetIntText(minxtick)));
	ThePlotData->minytick = Hv_iMax(0, Hv_iMin(12, Hv_GetIntText(minytick)));
	ThePlotData->xprecision = Hv_iMax(0, Hv_iMin(8, Hv_GetIntText(xprecision)));
	ThePlotData->yprecision = Hv_iMax(0, Hv_iMin(8, Hv_GetIntText(yprecision)));
	
	
	ThePlotData->background = Hv_GetFloatText(background);
	ThePlotData->xtietag = Hv_GetIntText(xtietag);
	ThePlotData->scilowlim = (float)(fabs(Hv_GetFloatText(scinotlow)));
	ThePlotData->scihilim  = (float)(fabs(Hv_GetFloatText(scinothi)));
	
	
	ThePlotData->xmin = Hv_GetFloatText(cxmin);
	check = Hv_fMax((float)(1.0e-10), (float)(0.001*fabs(ThePlotData->xmin)));
	ThePlotData->xmax = Hv_GetFloatText(cxmax);
	if (ThePlotData->xmax <= ThePlotData->xmin)
	    ThePlotData->xmax = ThePlotData->xmin + check;
	
	ThePlotData->ymin = Hv_GetFloatText(cymin);
	check = Hv_fMax((float)(1.0e-10), (float)(0.001*fabs(ThePlotData->ymin)));
	ThePlotData->ymax = Hv_GetFloatText(cymax);
	if (ThePlotData->ymax <= ThePlotData->ymin)
	    ThePlotData->ymax = ThePlotData->ymin + check;
	
/* check that no improper limits on a log plot */
	
	
	if ((ThePlotData->xmin <= 0.0) && (ThePlotData->xlog)) {
	    Hv_Warning("Cannot have a log axis with a nonpositive limit.");
	    ThePlotData->xlog = False;
	    warned = True;
	}
	
	if ((ThePlotData->ymin <= 0.0) && (ThePlotData->ylog)) {
	    if (!warned)
		Hv_Warning("Cannot have a log axis with a nonpositive limit.");
	    ThePlotData->ylog = False;
	}
	
	
	oldtype = Hv_GetPlotType(ThePlot);
	ThePlotData->plottype = Hv_PosGetOptionMenu(plotmenu, pbuttons,  num_pbuttons);
	newtype = ThePlotData->plottype;
	
/* under certain circumstances, we need to switch labels */
	
	Hv_CheckSwitchLabels(ThePlot, oldtype, newtype); 
	
	ThePlotData->axesstyle = Hv_PosGetOptionMenu(axesmenu, abuttons,  num_abuttons);
	
/* the curve data */
	
	Hv_GetCurveDlogItems(answer);
	
	Hv_CheckPlotAttachments(Item);
	Hv_SizeLegendItem(ThePlotData->legenditem);
	
	newregion = Hv_TotalPlotRegion(Item);
	totalregion = Hv_UnionRegion(oldregion, newregion);
	Hv_DestroyRegion(oldregion);
	Hv_CopyRegion(&oldregion, newregion);
	
	
	/*    Hv_DrawView(Item->view, totalregion); */
	Hv_OffScreenViewUpdate(Item->view, totalregion);
	
	Hv_DestroyRegion(newregion);
	Hv_DestroyRegion(totalregion);
	
	
	if (answer != Hv_APPLY)
	    break;
    }
    
    Hv_DestroyRegion(oldregion);
}


/*------ GetTheFont --------*/

static short GetTheFont(Hv_Widget      family,
			Hv_Widget      size,
			Hv_Widget      style,
			Hv_WidgetList  family_btns,
			Hv_WidgetList  size_btns,
			Hv_WidgetList  style_btns)
    
{
    short  hotfamily, hotsize, hotstyle;
    short  font;
    
    hotfamily = Hv_PosGetOptionMenu(family, family_btns, 5);
    hotsize   = Hv_PosGetOptionMenu(size,   size_btns,   5);
    hotstyle  = Hv_PosGetOptionMenu(style,  style_btns,  2);
    font =  Hv_FontParametersToFont(hotfamily, hotsize, hotstyle);
    
    return font;
}

/*------ SetTheFont --------*/

static void SetTheFont(short          font,
		       Hv_Widget      family,
		       Hv_Widget      size,
		       Hv_Widget      style,
		       Hv_WidgetList  family_btns,
		       Hv_WidgetList  size_btns,
		       Hv_WidgetList  style_btns)
    
{
    short  hotfamily, hotsize, hotstyle;
    Hv_FontToFontParameters(font, &hotfamily, &hotsize, &hotstyle);
    Hv_SetOptionMenu(family, family_btns, (int)hotfamily);
    Hv_SetOptionMenu(size,   size_btns,   (int)hotsize);
    Hv_SetOptionMenu(style,  style_btns,  (int)hotstyle);
    
}

/*----- Hv_GetCurvePos -------*/

int   Hv_GetCurvePos(void)
    
{
    int           pos = 1;
    Hv_CurveData  tcurve;
    
    for (tcurve = ThePlotData->curves; tcurve != NULL; tcurve = tcurve->next) {
	if (tcurve == TheCurve)
	    return pos;
	pos++;
    }
    
    return -1;
}

/*----- Hv_SetCurveDlogItems ------*/

static void   Hv_SetCurveDlogItems(void)
    
{
    Boolean  havecurves, havecurve;
    int      alg = -1;
    Boolean  fitalg = False;
    int      fittype = 0;
    Boolean  usesymsize = False;
    Boolean  standardxy;
    Boolean  histo;
    Boolean  xyorhisto;
    
    havecurves = (ThePlotData->curves != NULL);
    havecurve =  (TheCurve != NULL);
    
    if (havecurve) {
	usesymsize = (TheCurve->symbol < Hv_POINTSYMBOL);
	alg = TheCurve->algorithm;
	fitalg = (alg == Hv_CURVEFIT);
	if (fitalg)
	    fittype = TheCurve->fittype;
    }
    
/* curve name */
    
    if (!havecurve)
	Hv_SetString(curvename, " None (Null)");
    else if (TheCurve->name == NULL)
	Hv_SetString(curvename, " No name");
    else
	Hv_SetString(curvename, TheCurve->name);
    
/* colors */
    
    if (havecurve) {
	
	Hv_SetOptionMenu(stylemenu,   sbuttons,  (int)(TheCurve->style));
	Hv_SetOptionMenu(symbolmenu,  sybuttons, (int)(TheCurve->symbol));
	Hv_SetOptionMenu(connectmenu, cbuttons,  (int)(TheCurve->algorithm));
	Hv_SetOptionMenu(fitmenu,     fbuttons,  (int)(TheCurve->fittype));
	
	newpattern = TheCurve->pattern;
	newlinecolor = TheCurve->linecolor;
	newsymbcolor = TheCurve->symbolcolor;
	Hv_ChangeLabelPattern(patternlabel, TheCurve->pattern);
	Hv_ChangeLabelColors(linecolorlabel,  -1, TheCurve->linecolor);
	Hv_ChangeLabelColors(symbcolorlabel,  -1, TheCurve->symbolcolor);
	Hv_SetToggleButton(cdisplay,   TheCurve->display);
	Hv_SetToggleButton(cyspike,    TheCurve->yspike);
	Hv_SetToggleButton(cabsval,    TheCurve->absval);
	Hv_SetToggleButton(conlegend,  TheCurve->onlegend);
	Hv_SetToggleButton(cshowfit,   TheCurve->showfit);
	Hv_SetToggleButton(cusexerrs,  TheCurve->usexerrs);
	Hv_SetToggleButton(cuseyerrs,  TheCurve->useyerrs);
	Hv_SetIntText(csymbsize,  TheCurve->symbolsize);
	Hv_SetIntText(cthickness, TheCurve->thickness);
	Hv_SetIntText(cpolyorder, TheCurve->polyorder);
	Hv_SetIntText(clegendreorder, TheCurve->legendreorder);
	Hv_SetIntText(cnumgauss, TheCurve->numgauss);
	
	Hv_SetFloatText(cxscale,   TheCurve->xscale, 2);
	Hv_SetFloatText(cyscale,   TheCurve->yscale, 2);
	
	
	Hv_PlotDataMinMax(TheCurve->x,
			  TheCurve->npts,
			  TheCurve->xscale,
			  TheCurve->absval,
			  &(TheCurve->xmin),
			  &(TheCurve->xmax));
	
	
	Hv_PlotDataMinMax(TheCurve->y,
			  TheCurve->npts,
			  TheCurve->yscale,
			  TheCurve->absval,
			  &(TheCurve->ymin),
			  &(TheCurve->ymax));
    }
    else {
	Hv_ChangeLabelPattern(patternlabel, 0);
	Hv_ChangeLabelColors(linecolorlabel,  -1, Hv_gray9);
	Hv_ChangeLabelColors(symbcolorlabel,  -1, Hv_gray9);
	Hv_SetToggleButton(cdisplay,   False);
	Hv_SetToggleButton(cyspike,    False);
	Hv_SetToggleButton(cabsval,    False);
	Hv_SetToggleButton(conlegend,  False);
	Hv_SetToggleButton(cshowfit,   False);
	Hv_SetToggleButton(cusexerrs,  False);
	Hv_SetToggleButton(cuseyerrs,  False);
	Hv_SetIntText(csymbsize, 0);
	Hv_SetIntText(cthickness, 1);
	Hv_SetIntText(cpolyorder, 0);
	Hv_SetIntText(clegendreorder, 0);
	Hv_SetIntText(cnumgauss, 0);
	
	Hv_SetFloatText(cxscale,   1.0, 2);
	Hv_SetFloatText(cyscale,   1.0, 2);
	
	Hv_SetOptionMenu(symbolmenu,  sybuttons, 0);
	Hv_SetOptionMenu(connectmenu, cbuttons,  1);
	Hv_SetOptionMenu(fitmenu,     fbuttons,  1);
    }
    
/* sensitivities */
    
    standardxy = (ThePlotData->plottype == Hv_STANDARDXY);
    standardxy = standardxy && havecurve;
    
    histo = (ThePlotData->plottype == Hv_1DHISTO);
    histo = histo && havecurve;
    
    xyorhisto = standardxy || histo;
    
    Hv_SetSensitivity(curvelist,      havecurves);
    Hv_SetSensitivity(patternlabel,   havecurves);
    Hv_SetSensitivity(linecolorlabel, havecurves);
    Hv_SetSensitivity(symbcolorlabel, havecurves);
    
    Hv_SetSensitivity(curvename,  havecurve);
    Hv_SetSensitivity(cdisplay,   havecurve);
    
    Hv_SetSensitivity(cyspike,    standardxy);
    Hv_SetSensitivity(cabsval,    standardxy);
    Hv_SetSensitivity(conlegend,  xyorhisto);
    Hv_SetSensitivity(cusexerrs,  standardxy);
    Hv_SetSensitivity(cuseyerrs,  standardxy);
    Hv_SetSensitivity(cthickness, xyorhisto);
    Hv_SetSensitivity(csymbsize,  standardxy && usesymsize);
    Hv_SetSensitivity(cxscale,    standardxy);
    Hv_SetSensitivity(cyscale,    standardxy);
    
    Hv_SetSensitivity(connectmenu, xyorhisto);
    Hv_SetSensitivity(stylemenu,   standardxy);
    Hv_SetSensitivity(symbolmenu,  standardxy);
    Hv_SetSensitivity(fitmenu,     xyorhisto && fitalg);
    
    Hv_SetSensitivity(clegendreorder, 
		      (xyorhisto && fitalg && (fittype == Hv_LEGENDREFIT)));
    Hv_SetSensitivity(cpolyorder,
		      (xyorhisto && fitalg && (fittype == Hv_POLYFIT)));
    Hv_SetSensitivity(cnumgauss,
		      (xyorhisto && fitalg && (fittype == Hv_GAUSSIANFIT)));
    Hv_SetSensitivity(cshowfit,   xyorhisto && fitalg);
}


/*----- Hv_GetCurveDlogItems ------*/

static void   Hv_GetCurveDlogItems(int answer)
    
{
    int     pos;
    float   oldyscale, oldxscale;
    int     oldpolyorder, oldnumgauss, oldlegendreorder;
    char    text[30];
    
    if (TheCurve == NULL)
	return;
    
    TheCurve->pattern = newpattern;
    TheCurve->linecolor = newlinecolor;
    TheCurve->symbolcolor = newsymbcolor;
    
    oldxscale = TheCurve->xscale;
    oldyscale = TheCurve->yscale;
    oldpolyorder = TheCurve->polyorder;
    oldlegendreorder = TheCurve->legendreorder;
    oldnumgauss = TheCurve->numgauss;
    
    TheCurve->display    = Hv_GetToggleButton(cdisplay);
    TheCurve->yspike     = Hv_GetToggleButton(cyspike);
    TheCurve->absval     = Hv_GetToggleButton(cabsval);
    TheCurve->onlegend   = Hv_GetToggleButton(conlegend);
    TheCurve->showfit    = Hv_GetToggleButton(cshowfit);
    TheCurve->usexerrs   = Hv_GetToggleButton(cusexerrs);
    TheCurve->useyerrs   = Hv_GetToggleButton(cuseyerrs);
    TheCurve->symbolsize = Hv_iMax(3, Hv_iMin(12, Hv_GetIntText(csymbsize)));
    TheCurve->thickness  = Hv_iMax(1, Hv_iMin(6, Hv_GetIntText(cthickness)));
    TheCurve->polyorder  = Hv_iMax(2, Hv_iMin(10, Hv_GetIntText(cpolyorder)));
    TheCurve->polyorder  = Hv_iMin(TheCurve->npts-1, TheCurve->polyorder);
    TheCurve->legendreorder  = Hv_iMax(2, Hv_iMin(10, Hv_GetIntText(clegendreorder)));
    TheCurve->legendreorder  = Hv_iMin(TheCurve->npts-1, TheCurve->legendreorder);
    
    TheCurve->numgauss   = Hv_iMax(1, Hv_iMin(10, Hv_GetIntText(cnumgauss)));
    TheCurve->xscale     = Hv_GetFloatText(cxscale);
    TheCurve->yscale     = Hv_GetFloatText(cyscale);
    
    TheCurve->algorithm  = Hv_PosGetOptionMenu(connectmenu, cbuttons,  num_cbuttons);
    TheCurve->fittype    = Hv_PosGetOptionMenu(fitmenu,     fbuttons,  num_fbuttons);
    TheCurve->style      = Hv_PosGetOptionMenu(stylemenu,   sbuttons,  num_sbuttons);
    TheCurve->symbol     = Hv_PosGetOptionMenu(symbolmenu,  sybuttons, num_sybuttons);
    
    if ((TheCurve->polyorder != oldpolyorder) || (TheCurve->numgauss != oldnumgauss) ||
	(TheCurve->xscale != oldxscale) || (TheCurve->yscale != oldyscale) ||
	(TheCurve->legendreorder != oldlegendreorder))
	TheCurve->dirtyfit = True;
    
    if (TheCurve->xscale != oldxscale)
	Hv_PlotDataMinMax(TheCurve->x,
			  TheCurve->npts,
			  TheCurve->xscale,
			  TheCurve->absval,
			  &(TheCurve->xmin),
			  &(TheCurve->xmax));
    
    if (TheCurve->yscale != oldyscale)
	Hv_PlotDataMinMax(TheCurve->y,
			  TheCurve->npts,
			  TheCurve->yscale,
			  TheCurve->absval,
			  &(TheCurve->ymin),
			  &(TheCurve->ymax));
    
    Hv_Free(TheCurve->name);
    TheCurve->name = Hv_GetString(curvename);
    
    pos = Hv_GetCurvePos();
    strncpy(text, TheCurve->name, 17);
    text[Hv_iMin(17, strlen(TheCurve->name))] = '\0';
    Hv_PosReplaceListItem(curvelist, pos, text);
    
    if (answer == Hv_RUNNING)
	Hv_UpdateXYPlot(ThePlot);
    
}

/*------ Hv_CurveListCB ------*/

static void Hv_CurveListCB(Hv_Widget w)
    
{
    int pos, i;
    
/* get the data for the present curve */
    
    Hv_GetCurveDlogItems(Hv_RUNNING);
    
/* now change curves */
    
    pos = Hv_GetListSelectedPos(curvelist);
    if (pos < 1)
	TheCurve = NULL;
    else {
	TheCurve = ThePlotData->curves;
	
	for (i = 1; i < pos; i++)
	    TheCurve = TheCurve->next;
    }
    
    Hv_SetCurveDlogItems();
}

/*------ LogToggleCB -------*/

static void LogToggleCB(Hv_Widget w)
    
{
    Boolean  xlog, ylog;
    
    xlog = Hv_GetToggleButton(xlogbutton);
    ylog = Hv_GetToggleButton(ylogbutton);
    
    Hv_SetSensitivity(majxtick, !xlog);
    Hv_SetSensitivity(minxtick, !xlog);
    Hv_SetSensitivity(majytick, !ylog);
    Hv_SetSensitivity(minytick, !ylog);
}


/*------ Hv_PlotFitSensitivity ------*/

void Hv_PlotFitSensitivity()
    
{
    int fittype;
    
    fittype = Hv_PosGetOptionMenu(fitmenu,  fbuttons,  num_fbuttons);
    Hv_SetSensitivity(clegendreorder, fittype == Hv_LEGENDREFIT);
    Hv_SetSensitivity(cpolyorder, fittype == Hv_POLYFIT);
    Hv_SetSensitivity(cnumgauss,  fittype == Hv_GAUSSIANFIT);
    
    if (TheCurve != NULL)
	TheCurve->dirtyfit = True;
}


/*------ Hv_PlotConnectSensitivity ------*/

void Hv_PlotConnectSensitivity(void)
    
{
    int   alg;
    int   fittype;
    
    alg = Hv_PosGetOptionMenu(connectmenu,  cbuttons,  num_cbuttons);
    Hv_SetSensitivity(fitmenu, (alg == Hv_CURVEFIT));
    
    if (alg == Hv_CURVEFIT) {
	fittype = Hv_PosGetOptionMenu(fitmenu,  fbuttons,  num_fbuttons);
	Hv_SetSensitivity(clegendreorder, fittype == Hv_LEGENDREFIT);
	Hv_SetSensitivity(cpolyorder, fittype == Hv_POLYFIT);
	Hv_SetSensitivity(cnumgauss,  fittype == Hv_GAUSSIANFIT);
	Hv_SetSensitivity(cshowfit,   True);
    }
    else {
	Hv_SetSensitivity(clegendreorder, False);
	Hv_SetSensitivity(cpolyorder, False);
	Hv_SetSensitivity(cnumgauss,  False);
	Hv_SetSensitivity(cshowfit,   False);
    }
    
    if (TheCurve != NULL)
	TheCurve->dirtyfit = True;
}

/* ------ Hv_EditPlotColors --- */

static void Hv_EditPlotColors (Hv_Widget w)
    
{
    if (w == axescolorlabel)
	Hv_ModifyColor(w, &newaxescolor, "Edit Axes Color", False);
    else if (w == majcolorlabel)
	Hv_ModifyColor(w, &newmajcolor, "Edit Major Gridline Color", False);
    else if (w == mincolorlabel)
	Hv_ModifyColor(w, &newmincolor, "Edit Minor Gridline Color", False);
    else if (w == linecolorlabel)
	Hv_ModifyColor(w, &newlinecolor, "Edit Curve Line Color", False);
    else if (w == symbcolorlabel)
	Hv_ModifyColor(w, &newsymbcolor, "Edit Symbol Fill Color", True);
}


/* ------ Hv_EditCurvePattern --- */

static void Hv_EditCurvePattern(Hv_Widget w)
    
{
    if (w == patternlabel)
	Hv_ModifyPattern(w, &newpattern, "Fill Pattern");
    
}



/*-------- Hv_PlotTypeSensitivities -----------*/

void Hv_PlotTypeSensitivities(void)
    
{
    Boolean isstandard;
    Boolean histo;
    Boolean xyorhisto;
    
    int     ptype;
    
    if (xlogbutton == NULL)
	return;
    
	if(plotmenu != NULL)
      ptype = Hv_PosGetOptionMenu(plotmenu, pbuttons,  num_pbuttons);
    isstandard = (ptype == Hv_STANDARDXY);
    histo = (ptype == Hv_1DHISTO);
    xyorhisto = (isstandard || histo);
    
    Hv_SetSensitivity(xlogbutton,       isstandard);
    Hv_SetSensitivity(ylogbutton,       isstandard);
    Hv_SetSensitivity(cyspike,          isstandard);
    Hv_SetSensitivity(cabsval,          isstandard);
    Hv_SetSensitivity(cshowfit,         xyorhisto);
    Hv_SetSensitivity(cusexerrs,        isstandard);
    Hv_SetSensitivity(cuseyerrs,        isstandard);
    Hv_SetSensitivity(cthickness,       xyorhisto);
    Hv_SetSensitivity(csymbsize,        isstandard);
    Hv_SetSensitivity(cxscale,          isstandard);
    Hv_SetSensitivity(cyscale,          isstandard);
    Hv_SetSensitivity(cshowfit,         xyorhisto);
    Hv_SetSensitivity(fancysymbols,     isstandard);
    Hv_SetSensitivity(background,       isstandard);
    Hv_SetSensitivity(usebackground,    isstandard);
    Hv_SetSensitivity(showbackground,   isstandard);
    Hv_SetSensitivity(symbcolorlabel,   isstandard);
    Hv_SetSensitivity(patternlabel,     !xyorhisto);
    
    Hv_SetSensitivity(stylemenu,   isstandard);
    Hv_SetSensitivity(symbolmenu,  isstandard);
    Hv_SetSensitivity(connectmenu, xyorhisto);
    Hv_SetSensitivity(fitmenu,     xyorhisto);
}


/*------- Hv_CheckSwitchLabels --------*/

static void Hv_CheckSwitchLabels(Hv_Item  plot,
				 int    oldtype,
				 int    newtype)
    
/**************************************************
  If plot type changed from a horiz orientation to
  a vert orientation (or vice versa) then we switch
  the axes labels. The standard type plot is taken
  to be "vertical" NOTE: also switches data mins/maxes
  *************************************************/
    
{
    Boolean       oldh, newh;   
    Hv_Item       dummy, xlab, ylab;
    char          *tstr = NULL;
    Hv_PlotData   plotdata;
    Boolean       tnz;
    
    if ((plot == NULL) || (oldtype == newtype))
	return;
    
    plotdata = Hv_GetPlotData(plot);
    
    oldh = ((oldtype == Hv_HBAR) || (oldtype == Hv_HSTACK));
    newh = ((newtype == Hv_HBAR) || (newtype == Hv_HSTACK));
    
    if (oldh == newh)
	return;
    
    Hv_BigThreeItems(plot, &dummy, &xlab, &ylab);
    
    if ((xlab == NULL) || (ylab == NULL))
	return;
    
    Hv_InitCharStr(&tstr, xlab->str->text);
    Hv_ChangeTextItemText(xlab, ylab->str->text, False);
    Hv_ChangeTextItemText(ylab, tstr, False);
    Hv_Free(tstr);
    
/* switch no zoom values */
    
    tnz = plotdata->xnozoom;
    plotdata->xnozoom = plotdata->ynozoom;
    plotdata->ynozoom = tnz;
    
/* now switch the data limits */
    
    Hv_ReasonablePlotLimits(plotdata);
    SetDlogItems(plotdata);
}

/*-------- SetDlogItems ----------*/

static void SetDlogItems(Hv_PlotData plotdata)
    
{
    
    if (cxmax == NULL)
	return;
    
    Hv_SetToggleButton(xnozoom,  plotdata->xnozoom);
    Hv_SetToggleButton(ynozoom,  plotdata->ynozoom);
    
    Hv_SetFloatText(cxmin, plotdata->xmin, 2);
    Hv_SetFloatText(cxmax, plotdata->xmax, 2);
    Hv_SetFloatText(cymin, plotdata->ymin, 2);
    Hv_SetFloatText(cymax, plotdata->ymax, 2);
}
