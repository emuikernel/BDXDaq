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

extern short desiredX;
extern short desiredY;


/***************************************
  A set of "standard" routines which
  are convenience functions covering many
  common cases.
  **************************************/

static void Hv_ClearListCB(Widget   w,
			   XtPointer data);

/*--------- Hv_StandardTools ---------*/

void   Hv_StandardTools(Hv_View  View,
			short    left,
			short    top,
			short    orientation,
			short    resizepolicy,
			int      whichtools,
			Hv_Item *first,
			Hv_Item *last)
    
/* add some/all of the standard tools. Return
   the first and last added so that other user
   buttons can be added nicely */
    
{
    short       placement;
    Hv_Item     Item = NULL;
    
    if (orientation == Hv_VERTICAL)
	placement = Hv_POSITIONBELOW;
    else
	placement = Hv_POSITIONRIGHT;
    
    *first = NULL;
    
    if (Hv_CheckBit(whichtools, Hv_CAMERATOOL))
	Item = Hv_VaCreateItem(View,
			       Hv_TYPE,         Hv_BUTTONITEM,
			       Hv_SINGLECLICK,  Hv_CameraCallback,
			       Hv_USERDRAW,     Hv_DrawCamera,
			       Hv_LEFT,         left,
			       Hv_TOP,          top,
			       Hv_RESIZEPOLICY, resizepolicy,
			       Hv_BALLOON,     (char *)"Use this \"camera\" to print this view.",
			       NULL);
    
    if (*first == NULL)
	*first = Item;
    
/* the text annotator */
    
    if (Hv_CheckBit(whichtools, Hv_TEXTTOOL))
	Item = Hv_VaCreateItem(View,
			       Hv_TYPE,                Hv_BUTTONITEM,
			       Hv_SINGLECLICK,         Hv_TextButtonCallback,
			       Hv_USERDRAW,            Hv_DrawTextButton,
			       Hv_RELATIVEPLACEMENT,   placement,
			       Hv_RESIZEPOLICY,        resizepolicy,
			       Hv_BALLOON,             (char *)"Use this to annotate the hotrect.",
			       NULL);
    
    if (*first == NULL)
	*first = Item;
    
/* world rect tool */
    
    if (Hv_CheckBit(whichtools, Hv_RECTTOOL))
	Item = Hv_VaCreateItem(View,
			       Hv_TYPE,                Hv_BUTTONITEM,
			       Hv_SINGLECLICK,         Hv_WorldRectButtonCallback,
			       Hv_USERDRAW,            Hv_DrawWorldRectButton,
			       Hv_RELATIVEPLACEMENT,   placement,
			       Hv_RESIZEPOLICY,        resizepolicy,
			       Hv_BALLOON,             (char *)"Use this to create a \"world\" rectangle.",
			       NULL);
    
    if (*first == NULL)
	*first = Item;
    
/* world line tool */
    
    
    if (Hv_CheckBit(whichtools, Hv_LINETOOL))
	Item = Hv_VaCreateItem(View,
			       Hv_TYPE,                Hv_BUTTONITEM,
			       Hv_SINGLECLICK,         Hv_WorldLineButtonCallback,
			       Hv_USERDRAW,            Hv_DrawWorldLineButton,
			       Hv_RELATIVEPLACEMENT,   placement,
			       Hv_RESIZEPOLICY,        resizepolicy,
			       Hv_BALLOON,             (char *)"Use this to create a \"world\" line.",
			       NULL);
    
    if (*first == NULL)
	*first = Item;
    
/* world polygon tool */
    
    
    if (Hv_CheckBit(whichtools, Hv_POLYGONTOOL))
	Item = Hv_VaCreateItem(View,
			       Hv_TYPE,                Hv_BUTTONITEM,
			       Hv_SINGLECLICK,         Hv_WorldPolygonButtonCallback,
			       Hv_USERDRAW,            Hv_DrawWorldPolygonButton,
			       Hv_RELATIVEPLACEMENT,   placement,
			       Hv_RESIZEPOLICY,        resizepolicy,
			       Hv_BALLOON,             (char *)"Use this to create a \"world\" polygon.",
			       NULL);
    
    if (Hv_CheckBit(whichtools, Hv_WEDGETOOL))
	Item = Hv_VaCreateItem(View,
			       Hv_TYPE,                Hv_BUTTONITEM,
			       Hv_SINGLECLICK,         Hv_WorldWedgeButtonCallback,
			       Hv_USERDRAW,            Hv_DrawWorldWedgeButton,
			       Hv_RELATIVEPLACEMENT,   placement,
			       Hv_RESIZEPOLICY,        resizepolicy,
			       Hv_BALLOON,             (char *)"Use this to create a \"world\" polygon.",
			       NULL);
    
    if (*first == NULL)
	*first = Item;
    
/* world oval tool */
    
    
    if (Hv_CheckBit(whichtools, Hv_OVALTOOL))
	Item = Hv_VaCreateItem(View,
			       Hv_TYPE,                Hv_BUTTONITEM,
			       Hv_SINGLECLICK,         Hv_WorldOvalButtonCallback,
			       Hv_USERDRAW,            Hv_DrawWorldOvalButton,
			       Hv_RELATIVEPLACEMENT,   placement,
			       Hv_RESIZEPOLICY,        resizepolicy,
			       Hv_BALLOON,             (char *)"Use this to create a \"world\" oval.",
			       NULL);
    
    if (*first == NULL)
	*first = Item;
    
    *last = Item;
    
}

/*------- Hv_StandardToolBackground ------*/

void Hv_StandardToolBackground(Hv_Item Item,
			       Hv_Rect *area)
    
/* archaic version of Hv_ButtonDrawingArea */
    
{
    Hv_ButtonDrawingArea(Item, area);
}

/*------- Hv_StandardRedoItem ---------*/

void Hv_StandardRedoItem(Hv_Item Item)
    
/* typically called to redraw an item in response
   to an "apply" selection */
    
{
    Hv_Region    oldregion, newregion, totalregion;

    oldregion = Hv_CopyItemRegion(Item, True);
    Hv_UnionRectWithRegion(Item->area, oldregion);
    
    if ((Item->type == Hv_TEXTITEM) && (desiredX > -32500)) {
	Hv_SizeTextItem(Item, desiredX, desiredY);
    }
    
    if (Item->fixregion != NULL)
	Item->fixregion(Item);
    
    newregion = Hv_CopyItemRegion(Item, True);
    Hv_UnionRectWithRegion(Item->area, newregion);
    
    totalregion = Hv_UnionRegion(oldregion, newregion);
    Hv_ShrinkRegion((Region)totalregion, -2, -2);
    
/* if item is saved in background it needs special care */
    
    if (Hv_CheckItemDrawControlBit(Item, Hv_INBACKGROUND))
	Hv_OffScreenViewUpdate(Item->view, totalregion);
    else
	Hv_DrawView(Item->view, totalregion);
    
    Hv_DestroyRegion(oldregion);
    Hv_DestroyRegion(newregion);
    Hv_DestroyRegion(totalregion);
}

/*------- Hv_StandardLabel --------*/

Hv_Widget Hv_StandardLabel(Hv_Widget  rc,
			   char     *label,
			   int     type)
    
/* a simple label with different foregrounds/backgrounds
   depending on type */
    
{
    short      font = Hv_fixed2;
    short      fg;
    short      bg;
    Hv_Widget  dummy;
    
    fg = Hv_black;
    bg = -1;
    
    if (type <= -1000) {
	font = Hv_times11;
	type += 1000;
    }
    
    if (type >= 9900) {
	font = Hv_fixed;
	type -= 10000;
    }
    
    if (type >= 990) {
	font = Hv_helveticaBold11;
	type -= 1000;
    }
    
    else if (type >= 90) {
	font = Hv_helveticaBold14;
	type -= 100;
    }
    
    
    switch (type) {
    case -3:
	fg = Hv_coral;
	break;
	
    case -2:
	fg = Hv_red;
	break;
	
    case -1:
	fg = Hv_darkBlue;
	break;
	
    case 1:
	fg = Hv_red;
	bg = Hv_gray14;
	break;
	
    case 2:
	fg = Hv_black;
	bg = Hv_wheat;
	break;
	
    case 3:
	bg = Hv_honeydew;
	break;

    case 4:
	fg = Hv_red;
	bg = Hv_honeydew;
	break;
    }
    
    if (bg < 0)
	dummy = Hv_VaCreateDialogItem(rc,
				      Hv_TYPE,        Hv_LABELDIALOGITEM,
				      Hv_LABEL,       label,
				      Hv_FONT,        font,
				      Hv_TEXTCOLOR,   fg,
				      NULL);
    else
	dummy = Hv_VaCreateDialogItem(rc,
				      Hv_TYPE,        Hv_LABELDIALOGITEM,
				      Hv_LABEL,       label,
				      Hv_FONT,        font,
				      Hv_TEXTCOLOR,   fg,
				      Hv_BACKGROUND,  bg,
				      Hv_BORDERWIDTH, 1,
				      Hv_BORDERCOLOR, Hv_black,
				      NULL);
    return dummy;
}


/*------ Hv_StandardClearListButton -------*/

Hv_Widget Hv_StandardClearListButton(Hv_Widget  parent,
				     Hv_Widget  list,
				     char    *label,
				     short    font)
    
{
    Widget  w;
    char    *tstr;
    
    Hv_InitCharStr(&tstr, label);
    
    w = Hv_VaCreateDialogItem(parent,
			      Hv_TYPE,     Hv_BUTTONDIALOGITEM,
			      Hv_LABEL,    tstr,
			      Hv_FONT,     font,
			      Hv_CALLBACK, Hv_ClearListCB,
			      Hv_DATA,     list,
			      NULL);
    
    Hv_Free(tstr);
    return w;
}

/*------ Hv_ClearListCB ---------*/

static void Hv_ClearListCB(Widget   w,
			   XtPointer data)
    
{
    Widget    list;
    
    if (data == NULL)
	return;
    
    list = (Widget)data;
    XmListDeselectAllItems(list);
}


/*------ Hv_StandardDeleteButton --------*/

Hv_Widget  Hv_StandardDeleteButton(Hv_Widget parent)
    
/* adds a delete button */
    
{
    Hv_Widget    w;
    
    w = Hv_VaCreateDialogItem(parent,
			      Hv_TYPE,      Hv_BUTTONDIALOGITEM,
			      Hv_LABEL,     (char *)" Delete ",
			      Hv_FONT,      Hv_fixed2,
			      NULL);
    return w;
}

/*------ Hv_StandardButton --------*/

Hv_Widget Hv_StandardButton(Hv_Widget    parent,
			    char      *label,
			    Hv_FunctionPtr CB)
    
{
    Hv_Widget        w;
    
    w = Hv_VaCreateDialogItem(parent,
			      Hv_TYPE,      Hv_BUTTONDIALOGITEM,
			      Hv_LABEL,     label,
			      Hv_FONT,      Hv_fixed2,
			      Hv_CALLBACK,  CB,
			      Hv_DATA,      Hv_CANCEL,
			      NULL);
    
    return w;
}

/*------ Hv_StandardScale --------*/

Hv_Widget Hv_StandardScale(Hv_Widget    parent,
			   int       min,
			   int       max,
			   int       current,
			   Hv_FunctionPtr CB)
    
{
    Hv_Widget        w;
    
    w = Hv_VaCreateDialogItem(parent,
			      Hv_TYPE,          Hv_SCALEDIALOGITEM,
			      Hv_MINVALUE,      min,
			      Hv_MAXVALUE,      max,
			      Hv_CURRENTVALUE,  current,
			      Hv_CALLBACK,      CB,
			      NULL);
    
    return w;
}

/*------ Hv_StandardTextField --------*/

Hv_Widget  Hv_StandardTextField(Hv_Widget parent)
    
{
    Hv_Widget     w;
    w  = Hv_VaCreateDialogItem(parent,
			       Hv_TYPE,           Hv_TEXTDIALOGITEM,
			       Hv_DEFAULTTEXT,    "     ",
			       Hv_FONT,           Hv_fixed2,
			       Hv_EDITABLE,       True,
			       Hv_NUMCOLUMNS,     6,
			       NULL);
    return w;
}

/*------ Hv_StandardToggleButton --------*/

Hv_Widget Hv_StandardToggleButton(Hv_Widget    parent,
				  char      *label,
				  Hv_FunctionPtr  CB)
{
    Hv_Widget    w;
    
    w  = Hv_VaCreateDialogItem(parent,
			       Hv_TYPE,     Hv_TOGGLEDIALOGITEM,
			       Hv_LABEL,    label,
			       Hv_FONT,     Hv_fixed2,
			       Hv_CALLBACK, CB,
			       NULL);
    
    return  w;
}


/* ------- Hv_StandardDoneButton ------*/

Hv_Widget Hv_StandardDoneButton(Hv_Widget  parent,
				char    *donestr)
    
    
{
    Hv_Widget   w;
/*
  w  = Hv_VaCreateDialogItem(parent,
  Hv_TYPE,            Hv_CLOSEOUTDIALOGITEM,
  Hv_FONT,            Hv_fixed2,
  Hv_LABEL,           donestr,
  Hv_DIALOGCLOSEOUT,  Hv_DONEBUTTON,
  NULL);
  */
    
    w  = Hv_VaCreateDialogItem(parent,
			       Hv_TYPE,            Hv_BUTTONDIALOGITEM,
			       Hv_FONT,            Hv_fixed2,
			       Hv_LABEL,           donestr,
			       Hv_DATA,            (void *)Hv_DONE,
			       Hv_CALLBACK,        Hv_CloseOutCallback,
			       NULL);
    
    return w;
}


/*------ Hv_StandardActionButtons --------*/

Hv_Widget Hv_StandardActionButtons(Hv_Widget  parent,
				   short    spacing,
				   int     whichones)
    
    
{
    Hv_Widget    dummy, w;
    
    dummy = Hv_SimpleDialogSeparator(parent);
    w  = Hv_VaCreateDialogItem(parent,
			       Hv_TYPE,            Hv_CLOSEOUTDIALOGITEM,
			       Hv_SPACING,         spacing,
			       Hv_FONT,            Hv_fixed2,
			       Hv_DIALOGCLOSEOUT,  whichones,
			       NULL);
    
    return  w;
}

/*------ Hv_DialogTable --------*/

Hv_Widget Hv_DialogTable(Hv_Widget  parent,
			 short    numrow,
			 short    spacing)
    
    
/* creates a table container for dlog widgets. Dlogs
   will be put in so as to have the number of rows
   specified. Thus if nrows = 4 and you add 8
   widgets, you'll get 4 rows and two columns.
   Rows will be filled first. */
    
{
    Hv_Widget    w;
    
    w = Hv_VaCreateDialogItem(parent,
			      Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
			      Hv_NUMROWS,      numrow,
			      Hv_PACKING,      Hv_PAD,
			      Hv_SPACING,      spacing,
			      NULL);
    return  w;
}

/*------ Hv_TightDialogTable --------*/

Hv_Widget Hv_TightDialogTable(Hv_Widget  parent,
			      short    numrow,
			      short    spacing)
    
    
/* creates a table container for dlog widgets. Dlogs
   will be put in so as to have the number of rows
   specified. Thus if nrows = 4 and you add 8
   widgets, you'll get 4 rows and two columns.
   Rows will be filled first.
   
   THIS IS THE "TIGHT" no-pad version  */
    
{
    Hv_Widget    w;
    
    w = Hv_VaCreateDialogItem(parent,
			      Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
			      Hv_NUMROWS,      numrow,
			      Hv_PACKING,      Hv_TIGHT,
			      Hv_SPACING,      spacing,
			      NULL);
    return  w;
}


/*------ Hv_StandardPackingRowCol --------*/

Hv_Widget Hv_StandardPackingRowCol(Hv_Widget  parent,
				   short    numrow,
				   short    spacing)
    
/* archaic form of Hv_DialogTable, kept for
   compatibility */
    
{
    return Hv_DialogTable(parent, numrow, spacing);
}


/*------ Hv_DialogRow --------*/

Hv_Widget Hv_DialogRow(Hv_Widget  parent,
		       short    spacing)
    
/* used for creating horizontal columns to
   hold dialog widgets. */
    
{
    return Hv_DialogTable(parent, 1, spacing);
}

/*------ Hv_DialogColumn --------*/

Hv_Widget Hv_DialogColumn(Hv_Widget  parent,
			  short    spacing)
    
/* used for creating vertical columns to
   hold dialog widgets. The "main" rowcol
   for a dialog is almost always one of these. */
    
{
    Hv_Widget    w;
    
    w = Hv_VaCreateDialogItem(parent,
			      Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
			      Hv_ORIENTATION,  Hv_VERTICAL,
			      Hv_SPACING,      spacing,
			      NULL);
    
    return w;
}

/*------ Hv_TightDialogRow --------*/

Hv_Widget Hv_TightDialogRow(Hv_Widget  parent,
			    short    spacing)
    
/* used for creating horizontal columns to
   hold dialog widgets. */
    
{
    return Hv_TightDialogTable(parent, 1, spacing);
}

/*------ Hv_TightDialogColumn --------*/

Hv_Widget Hv_TightDialogColumn(Hv_Widget  parent,
			       short    spacing)
    
/* used for creating vertical columns to
   hold dialog widgets. The "main" rowcol
   for a dialog is almost always one of these. */
    
{
    Hv_Widget    w;
    
    w = Hv_VaCreateDialogItem(parent,
			      Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
			      Hv_ORIENTATION,  Hv_VERTICAL,
			      Hv_SPACING,      spacing,
			      Hv_PACKING,      Hv_TIGHT,
			      NULL);
    
    return w;
}

/*------ Hv_StandardMainRowCol --------*/

Hv_Widget Hv_StandardMainRowCol(Hv_Widget  parent,
				short    spacing)
    
    
/* archaic form of Hv_DialogColumn kept for compatibility */
    
{
    return Hv_DialogColumn(parent, spacing);
}





