/**
 * <EM>Deals with placing menus on dialogs.</EM>
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
#include "Hv_undo.h"
#include "Hv_xyplot.h"

#define Hv_NUMSTYLENAMES      4
#define Hv_NUMARROWNAMES      4
#define Hv_NUMSYMNAMES        10
#define Hv_NUMFAMILYNAMES     5
#define Hv_NUMPLOTTYPENAMES   7
#define Hv_NUMFITNAMES        5
#define Hv_NUMAXESTYPENAMES   3
#define Hv_NUMCONNECTNAMES    5
#define Hv_NUMSIZENAMES       6
#define Hv_NUMFONTSTYLENAMES  2

static char *stylenames[Hv_NUMSTYLENAMES] = {
  "Solid",
  "Dashed",
  "Long Dashed",
  "Dot Dash",
};

static char *arrownames[Hv_NUMARROWNAMES] = {
  "No arrow",
  "Arrow at end",
  "Arrow at start",
  "Arrow at both ends",
};

static char *symbolnames[Hv_NUMSYMNAMES] = {
    "No symbol",
    "Square",
    "Circle",
    "Diamond",
    "Cross",
    "UpTriangle",
    "DownTriangle",
    "Block \"I\"",
    "Point",
    "Big Point"
  };

static  char *familynames[Hv_NUMFAMILYNAMES] = {
  "Fixed",
  "Courier",
  "Helvetica",
  "Times",
  "Symbol"
};

static char *fitnames[Hv_NUMFITNAMES] = {
  "Linear",
  "Exponential",
  "Polynomial",
  "Gaussian",
  "Legendre"
};

static  char *plottypenames[Hv_NUMPLOTTYPENAMES] = {
  " Standard XY",
  "Horiz bar",
  "Vert bar",
  "Horiz stack",
  "Vert stack",
  "1D Histogram",
  "2D Histogram"
};

static  char *axestypenames[Hv_NUMAXESTYPENAMES] = {
  "2 Axes",
  "2 axes/frame",
  "4 axes"
};

static   char *connectnames[Hv_NUMCONNECTNAMES] = {
  "No lines",
  "Simple",
  "Stairs",
  "Spline",
  "Fit"
};

static  char *sizenames[Hv_NUMSIZENAMES] = {
  "11",
  "12",
  "14",
  "17",
  "20",
  "25"
};

static char *fontstylenames[Hv_NUMFONTSTYLENAMES] = {
  "Plain",
  "Bold"
};


static void Hv_DoFontPopupMenu(Hv_Widget	w,
			       Hv_Pointer	data);

static void Hv_DoAxesTypePopupMenu(Hv_Widget	w,
				   Hv_Pointer	data);

static void Hv_DoPlotTypePopupMenu(Hv_Widget	w,
				   Hv_Pointer	data);

static void Hv_DoConnectionTypePopupMenu(Hv_Widget	w,
					 Hv_Pointer	data);

static void Hv_DoFitTypePopupMenu(Hv_Widget	w,
				  Hv_Pointer	data);


static void Hv_DoLineStylePopupMenu(Hv_Widget	w,
				    Hv_Pointer	data);

static void Hv_DoArrowStylePopupMenu(Hv_Widget	w,
				     Hv_Pointer	data);

static void Hv_DoSymbolPopupMenu(Hv_Widget	w,
				 Hv_Pointer	data);

static void Hv_DoFontFamilyPopupMenu(Hv_Widget	w,
				     Hv_Pointer	data);


static void Hv_DoFontSizePopupMenu(Hv_Widget	w,
				   Hv_Pointer	data);


static void Hv_DoFontStylePopupMenu(Hv_Widget	w,
				    Hv_Pointer	data);


/* ----- Hv_CreateSymbolPopup -----*/

Hv_Widget Hv_CreateSymbolPopup(Hv_Widget rc,
			       short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, "    symbol",
			     Hv_NUMSYMNAMES, symbolnames,
			     Hv_DoSymbolPopupMenu);
}

/* ----- Hv_CreateFontFamilyPopup -----*/

Hv_Widget Hv_CreateFontFamilyPopup(Hv_Widget rc,
				   short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, NULL,
			     Hv_NUMFAMILYNAMES, familynames,
			     Hv_DoFontFamilyPopupMenu);
}

/* ----- Hv_CreatePlotTypePopup -----*/

Hv_Widget Hv_CreatePlotTypePopup(Hv_Widget rc,
				 short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, "Plot type",
			     Hv_NUMPLOTTYPENAMES, plottypenames,
			     Hv_DoPlotTypePopupMenu);
}

/* ----- Hv_CreateAxesTypePopup -----*/

Hv_Widget Hv_CreateAxesTypePopup(Hv_Widget rc,
				 short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, "Axes type",
			     Hv_NUMAXESTYPENAMES, axestypenames,
			     Hv_DoAxesTypePopupMenu);
}

/* ----- Hv_CreateConnectionTypePopup -----*/

Hv_Widget Hv_CreateConnectionTypePopup(Hv_Widget rc,
				       short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, "Connection",
			     Hv_NUMCONNECTNAMES, connectnames,
			     Hv_DoConnectionTypePopupMenu);
}

/* ----- Hv_CreateFitTypePopup -----*/

Hv_Widget Hv_CreateFitTypePopup(Hv_Widget rc,
				short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, "  Fit type",
			     Hv_NUMFITNAMES, fitnames,
			     Hv_DoFitTypePopupMenu);
}

/* ----- Hv_CreateFontSizePopup -----*/

Hv_Widget Hv_CreateFontSizePopup(Hv_Widget rc,
				 short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, NULL,
			     Hv_NUMSIZENAMES, sizenames,
			     Hv_DoFontSizePopupMenu);
}

/* ----- Hv_CreateFontStylePopup -----*/

Hv_Widget Hv_CreateFontStylePopup(Hv_Widget rc,
				 short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, NULL,
			     Hv_NUMFONTSTYLENAMES, fontstylenames,
			     Hv_DoFontStylePopupMenu);
}

/* ----- Hv_CreateOptionMenu -----*/

Hv_Widget Hv_CreateOptionMenu(Hv_Widget         rc,   /*parent*/
			      short          font,
			      char          *title,
			      int            numitems,
			      char          *items[],
			      Hv_FunctionPtr CB)
{
  return Hv_CreateThinOptionMenu(rc, font, title, numitems, items, CB, 3);
}


/* ----- Hv_CreateLineStylePopup -----*/

Hv_Widget Hv_CreateLineStylePopup(Hv_Widget rc,
			       short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, "line style",
			     Hv_NUMSTYLENAMES, stylenames,
			     Hv_DoLineStylePopupMenu);
}



/* ----- Hv_CreateArrowStylePopup -----*/

Hv_Widget Hv_CreateArrowStylePopup(Hv_Widget rc,
			       short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, "arrow head", 
			     Hv_NUMARROWNAMES, arrownames, 
			     Hv_DoArrowStylePopupMenu);
}


/******************************************
  Below here are the "Hv_Do" menu handlers
  for the standard option popups
********************************************/


/* ---- Hv_DoFontPopupMenu -----*/

static void Hv_DoFontPopupMenu(Hv_Widget	w,
			       Hv_Pointer	data)

{
  Hv_hotFont = (short)((int)data);    /* this will be the item selected */
}

/* ---- Hv_DoSymbolPopupMenu -----*/

static void Hv_DoSymbolPopupMenu(Hv_Widget	w,
				 Hv_Pointer	data)

{
  Hv_hotSymbol = (short)((int)data);    /* this will be the item selected */
}

/* ---- Hv_DoFontFamilyPopupMenu -----*/

static void Hv_DoFontFamilyPopupMenu(Hv_Widget	w,
				     Hv_Pointer	data)

{
  Hv_hotFontFamily = (short)((int)data);    /* this will be the item selected */
}

/* ---- Hv_DoFontSizePopupMenu -----*/

static void Hv_DoFontSizePopupMenu(Hv_Widget	w,
				   Hv_Pointer	data)

{
  Hv_hotFontSize = (short)((int)data);    /* this will be the item selected */
}

/* ---- Hv_DoFontStylePopupMenu -----*/

static void Hv_DoFontStylePopupMenu(Hv_Widget	w,
				    Hv_Pointer	data)

{
  Hv_hotFontStyle = (short)((int)data);    /* this will be the item selected */
}

/* ---- Hv_DoLineStylePopupMenu -----*/

static void Hv_DoLineStylePopupMenu(Hv_Widget	w,
				 Hv_Pointer	data)

{
  Hv_hotStyle = (short)((int)data);    /* this will be the item selected */
}


/* ---- Hv_DoArrowStylePopupMenu -----*/

static void Hv_DoArrowStylePopupMenu(Hv_Widget	w,
				 Hv_Pointer	data)
{
  Hv_hotArrowStyle = (short)((int)data);
}

/* ---- Hv_DoPlotTypePopupMenu -----*/

static void Hv_DoPlotTypePopupMenu(Hv_Widget	w,
			       Hv_Pointer	data)
{
  Hv_PlotTypeSensitivities();
}

/* ---- Hv_DoAxesTypePopupMenu -----*/

static void Hv_DoAxesTypePopupMenu(Hv_Widget	w,
			       Hv_Pointer	data)
{
}

/* ---- Hv_DoTypeConnectionPopupMenu -----*/

static void Hv_DoConnectionTypePopupMenu(Hv_Widget	w,
			       Hv_Pointer	data)
{
  Hv_PlotConnectSensitivity();
}

/* ---- Hv_DoFitTypePopupMenu -----*/

static void Hv_DoFitTypePopupMenu(Hv_Widget	w,
				  Hv_Pointer	data)
{
  Hv_PlotFitSensitivity();
}

#undef Hv_NUMSYMNAMES
#undef Hv_NUMFAMILYNAMES
#undef Hv_NUMPLOTTYPENAMES
#undef Hv_NUMAXESTYPENAMES
#undef Hv_NUMCONNECTNAMES
#undef Hv_NUMSIZENAMES
#undef Hv_NUMSTYLENAMES
#undef Hv_NUMFONTSTYLENAMES
#undef Hv_NUMARROWNAMES
#undef Hv_NUMFITNAMES


