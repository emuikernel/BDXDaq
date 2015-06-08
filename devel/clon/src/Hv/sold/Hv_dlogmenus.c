/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov


THIS FILE HANDLES THE "STANDARD" POPUP MENUS THAT YOU MAY WANT
TO ADD TO A DIALOG

==================================================================================
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
#define Hv_NUMSIZENAMES       5
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
  "14",
  "17",
  "20",
  "25"
};

static char *fontstylenames[Hv_NUMFONTSTYLENAMES] = {
  "Plain",
  "Bold"
};


static void Hv_DoFontPopupMenu(Widget	w,
			       XtPointer	client_data,
			       XtPointer	call_data);	

static void Hv_DoAxesTypePopupMenu(Widget	w,
				   XtPointer	client_data,
				   XtPointer	call_data);	

static void Hv_DoPlotTypePopupMenu(Widget	w,
				   XtPointer	client_data,
				   XtPointer	call_data);	

static void Hv_DoConnectionTypePopupMenu(Widget	w,
					 XtPointer	client_data,
					 XtPointer	call_data);	

static void Hv_DoFitTypePopupMenu(Widget	w,
				  XtPointer	client_data,
				  XtPointer	call_data);	


static void Hv_DoLineStylePopupMenu(Widget	w,
				    XtPointer	client_data,
				    XtPointer	call_data);	

static void Hv_DoArrowStylePopupMenu(Widget	w,
				     XtPointer	client_data,
				     XtPointer	call_data);	

static void Hv_DoSymbolPopupMenu(Widget	w,
				 XtPointer	client_data,
				 XtPointer	call_data);	

static void Hv_DoFontFamilyPopupMenu(Widget	w,
				     XtPointer	client_data,
				     XtPointer	call_data);	


static void Hv_DoFontSizePopupMenu(Widget	w,
				   XtPointer	client_data,
				   XtPointer	call_data);	


static void Hv_DoFontStylePopupMenu(Widget	w,
				    XtPointer	client_data,
				    XtPointer	call_data);	


/* ----- Hv_CreateSymbolPopup -----*/

Hv_Widget Hv_CreateSymbolPopup(Widget rc,
			       short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, "    symbol",
			     Hv_NUMSYMNAMES, symbolnames,
			     Hv_DoSymbolPopupMenu);
}

/* ----- Hv_CreateFontFamilyPopup -----*/

Hv_Widget Hv_CreateFontFamilyPopup(Widget rc,
				   short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, NULL,
			     Hv_NUMFAMILYNAMES, familynames,
			     Hv_DoFontFamilyPopupMenu);
}

/* ----- Hv_CreatePlotTypePopup -----*/

Hv_Widget Hv_CreatePlotTypePopup(Widget rc,
				 short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, "Plot type",
			     Hv_NUMPLOTTYPENAMES, plottypenames,
			     Hv_DoPlotTypePopupMenu);
}

/* ----- Hv_CreateAxesTypePopup -----*/

Hv_Widget Hv_CreateAxesTypePopup(Widget rc,
				 short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, "Axes type",
			     Hv_NUMAXESTYPENAMES, axestypenames,
			     Hv_DoAxesTypePopupMenu);
}

/* ----- Hv_CreateConnectionTypePopup -----*/

Hv_Widget Hv_CreateConnectionTypePopup(Widget rc,
				       short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, "Connection",
			     Hv_NUMCONNECTNAMES, connectnames,
			     Hv_DoConnectionTypePopupMenu);
}

/* ----- Hv_CreateFitTypePopup -----*/

Hv_Widget Hv_CreateFitTypePopup(Widget rc,
				short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, "  Fit type",
			     Hv_NUMFITNAMES, fitnames,
			     Hv_DoFitTypePopupMenu);
}

/* ----- Hv_CreateFontSizePopup -----*/

Hv_Widget Hv_CreateFontSizePopup(Widget rc,
				 short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, NULL,
			     Hv_NUMSIZENAMES, sizenames,
			     Hv_DoFontSizePopupMenu);
}

/* ----- Hv_CreateFontStylePopup -----*/

Hv_Widget Hv_CreateFontStylePopup(Widget rc,
				 short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, NULL,
			     Hv_NUMFONTSTYLENAMES, fontstylenames,
			     Hv_DoFontStylePopupMenu);
}

/* ----- Hv_CreateOptionMenu -----*/

Hv_Widget Hv_CreateOptionMenu(Widget         rc,   /*parent*/
			      short          font,
			      char          *title,
			      int            numitems,
			      char          *items[],
			      Hv_FunctionPtr CB)
{
  return Hv_CreateThinOptionMenu(rc, font, title, numitems, items, CB, 3);
}

/* ----- Hv_CreateThinOptionMenu -----*/

Hv_Widget Hv_CreateThinOptionMenu(Widget         rc,   /*parent*/
				  short          font,
				  char          *title,
				  int            numitems,
				  char          *items[],
				  Hv_FunctionPtr CB,
				  short          margin)

/********************************************************
   convenience function for creating an option menu popup
   on a dialog. 

   *  rc is the rowcolumn parent
   *  font is usually Hv_fixed2
   *  title is self explanatory
   *  numitems INCLUDES separators
   *  items is an array of strings, if you want a
      separator, put "Hv_SEPERATOR" in the appropriate slot
   * CB is a common callback

**********************************************************/     
{
  Widget     popup;
  int        i, n;
  XmString   mainlabel = NULL;
  Widget     submenu, label, dummy;
  Arg        arg[2]; /* arguments for widgets */
  int        buttoncount = -1;
  Boolean    varyfont;


  if (numitems < 1)
    return NULL;

  varyfont = (font < 0);

  submenu = XmCreatePulldownMenu(rc, "optionsubmenu", NULL, 0);

  XtVaSetValues(submenu,
		XmNmarginHeight, 0,
		NULL);

/* as we loop through,  look for "Hv_SEPARATOR" to flag
   the insertion of a separator */

  for (i = 0; i < numitems; i++) {
    
    if (Hv_Contained(items[i], "Hv_SEPARATOR")) {
    }
    else {
      buttoncount++;

      if (varyfont) {
	font = buttoncount*5 + 1;
	if (font < Hv_symbol11)
	  font += 25;
      }
      
      dummy = Hv_CreateManagedPushButton(submenu, items[i], font,
					 (XtCallbackProc)CB, (XtPointer)buttoncount);
      XtVaSetValues(dummy,
		    XmNborderWidth,  0,
		    XmNmarginWidth,  margin,
		    XmNmarginHeight,  margin,
		    XmNshadowThickness,  1,
		    NULL);

    }
    
  }
  
  n = 0;
  if (title != NULL) {
    mainlabel = Hv_CreateMotifString(title, font);
    XtSetArg(arg[n], XmNlabelString, mainlabel);
    n++;
  }

  XtSetArg(arg[n], XmNsubMenuId,   submenu);
  n++;

  popup = XmCreateOptionMenu(rc, "option_menu", arg, n);
  
  XtVaSetValues(popup,
		XmNfontList, Hv_fontList,
		NULL);
  XtManageChild(popup);
  
  label = XmOptionLabelGadget(popup);
  XtVaSetValues(label, XmNfontList, Hv_fontList, NULL);
  Hv_ChangeLabelColors(label, Hv_black, -1);
  
  if (mainlabel != NULL)
    XmStringFree(mainlabel);
  return  popup;
}


/* ----- Hv_CreateLineStylePopup -----*/

Hv_Widget Hv_CreateLineStylePopup(Widget rc,
			       short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, "line style",
			     Hv_NUMSTYLENAMES, stylenames,
			     Hv_DoLineStylePopupMenu);
}



/* ----- Hv_CreateArrowStylePopup -----*/

Hv_Widget Hv_CreateArrowStylePopup(Widget rc,
			       short  font)
     
{
  return Hv_CreateOptionMenu(rc, font, "arrow head", 
			     Hv_NUMARROWNAMES, arrownames, 
			     Hv_DoArrowStylePopupMenu);
}



/* ----- Hv_CreateFontPopup -----*/

Hv_Widget Hv_CreateFontPopup(Widget rc,
			     short  font)

/**********************************************
  This is the archaic font popup that is just kept
  for backward compatibility
*************************************************/


{
  Widget     popup;
  int        i;
  XmString   mainlabel;
  XmString   fontxms[Hv_NUMFONTS];
  Widget     label;
  mainlabel = Hv_CreateMotifString("font:  ", Hv_fixed2);

  for (i = 0; i < Hv_NUMFONTS; i++)
     fontxms[i] = Hv_CreateMotifString(Hv_charSet[i], font);

  popup = XmVaCreateSimpleOptionMenu(rc, "optionmenu", mainlabel, 'f', 0, Hv_DoFontPopupMenu,
				     XmVaPUSHBUTTON, fontxms[0],  NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[1],  NULL, NULL, NULL,
#ifndef sparc
				     XmVaSEPARATOR,
#endif
				     XmVaPUSHBUTTON, fontxms[2],  NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[3],  NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[4],  NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[5],  NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[6],  NULL, NULL, NULL,
#ifndef sparc
				     XmVaSEPARATOR,
#endif
				     XmVaPUSHBUTTON, fontxms[7],  NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[8],  NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[9],  NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[10], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[11], NULL, NULL, NULL,
#ifndef sparc
				     XmVaSEPARATOR,
#endif
				     XmVaPUSHBUTTON, fontxms[12], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[13], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[14], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[15], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[16], NULL, NULL, NULL,
#ifndef sparc
				     XmVaSEPARATOR,
#endif
				     XmVaPUSHBUTTON, fontxms[17], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[18], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[19], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[20], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[21], NULL, NULL, NULL,
#ifndef sparc
				     XmVaSEPARATOR,
#endif
				     XmVaPUSHBUTTON, fontxms[22], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[23], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[24], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[25], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[26], NULL, NULL, NULL,
#ifndef sparc
				     XmVaSEPARATOR,
#endif
				     XmVaPUSHBUTTON, fontxms[27], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[28], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[29], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[30], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[31], NULL, NULL, NULL,
#ifndef sparc
				     XmVaSEPARATOR,
#endif
				     XmVaPUSHBUTTON, fontxms[32], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[33], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[34], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[35], NULL, NULL, NULL,
				     XmVaPUSHBUTTON, fontxms[36], NULL, NULL, NULL,
				     NULL);


  XtManageChild(popup);

  label = XmOptionLabelGadget(popup);
  XtVaSetValues(label, XmNfontList, Hv_fontList, NULL);

  XmStringFree(mainlabel);
  for (i = 0; i < Hv_NUMFONTS; i++)
    XmStringFree(fontxms[i]);


  return  popup;
}

/******************************************
  Below here are the "Hv_Do" menu handlers
  for the standard option popups
********************************************/


/* ---- Hv_DoFontPopupMenu -----*/

static void Hv_DoFontPopupMenu(Widget	w,
			       XtPointer	client_data,
			       XtPointer	call_data)	

{
  Hv_hotFont = (short)((int)client_data);    /* this will be the item selected */
}

/* ---- Hv_DoSymbolPopupMenu -----*/

static void Hv_DoSymbolPopupMenu(Widget	w,
				 XtPointer	client_data,
				 XtPointer	call_data)	

{
  Hv_hotSymbol = (short)((int)client_data);    /* this will be the item selected */
}

/* ---- Hv_DoFontFamilyPopupMenu -----*/

static void Hv_DoFontFamilyPopupMenu(Widget	w,
				     XtPointer	client_data,
				     XtPointer	call_data)	

{
  Hv_hotFontFamily = (short)((int)client_data);    /* this will be the item selected */
}

/* ---- Hv_DoFontSizePopupMenu -----*/

static void Hv_DoFontSizePopupMenu(Widget	w,
				   XtPointer	client_data,
				   XtPointer	call_data)	

{
  Hv_hotFontSize = (short)((int)client_data);    /* this will be the item selected */
}

/* ---- Hv_DoFontStylePopupMenu -----*/

static void Hv_DoFontStylePopupMenu(Widget	w,
				    XtPointer	client_data,
				    XtPointer	call_data)	

{
  Hv_hotFontStyle = (short)((int)client_data);    /* this will be the item selected */
}

/* ---- Hv_DoLineStylePopupMenu -----*/

static void Hv_DoLineStylePopupMenu(Widget	w,
				 XtPointer	client_data,
				 XtPointer	call_data)	

{
  Hv_hotStyle = (short)((int)client_data);    /* this will be the item selected */
}


/* ---- Hv_DoArrowStylePopupMenu -----*/

static void Hv_DoArrowStylePopupMenu(Widget	w,
				 XtPointer	client_data,
				 XtPointer	call_data)	
{
  Hv_hotArrowStyle = (short)((int)client_data);
}

/* ---- Hv_DoPlotTypePopupMenu -----*/

static void Hv_DoPlotTypePopupMenu(Widget	w,
			       XtPointer	client_data,
			       XtPointer	call_data)	
{
  Hv_PlotTypeSensitivities();
}

/* ---- Hv_DoAxesTypePopupMenu -----*/

static void Hv_DoAxesTypePopupMenu(Widget	w,
			       XtPointer	client_data,
			       XtPointer	call_data)	
{
}

/* ---- Hv_DoTypeConnectionPopupMenu -----*/

static void Hv_DoConnectionTypePopupMenu(Widget	w,
			       XtPointer	client_data,
			       XtPointer	call_data)	
{
  Hv_PlotConnectSensitivity();
}

/* ---- Hv_DoFitTypePopupMenu -----*/

static void Hv_DoFitTypePopupMenu(Widget	w,
				  XtPointer	client_data,
				  XtPointer	call_data)	
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


