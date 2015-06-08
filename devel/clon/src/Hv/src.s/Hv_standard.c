/**
 * <EM>Convenience routines for common methods of creating various widgets.</EM>
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

extern short desiredX;
extern short desiredY;


/***************************************
  A set of "standard" routines which
  are convenience functions covering many
  common cases.
  **************************************/

static void Hv_ClearListCB(Hv_Widget   w,
			   Hv_Pointer data);


/**
 * Hv_StandardTools
 * @purpose  Create the (original style) set of view drawing tools
 * @param left 
 * @param top
 * @param orientation
 * @param resizepolicy
 * @param whichtools
 * @param first  Upon return, this is the first tool added, 
 *  which may be useful for aligning other items
 * @param first  Upon return, this is the last tool added, 
 *  which may be useful for aligning other items
 */

void   Hv_StandardTools(Hv_View  View,
			short    left,
			short    top,
			short    orientation,
			short    resizepolicy,
			int      whichtools,
			Hv_Item *first,
			Hv_Item *last) {
    
/* add some/all of the standard tools. Return
   the first and last added so that other user
   buttons can be added nicely */
    
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
			       Hv_DRAWCONTROL,  Hv_LIGHTWEIGHT,
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
			       Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
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
			       Hv_DRAWCONTROL,          Hv_LIGHTWEIGHT,
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
			       Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
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
			       Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
			       Hv_SINGLECLICK,         Hv_WorldPolygonButtonCallback,
			       Hv_USERDRAW,            Hv_DrawWorldPolygonButton,
			       Hv_RELATIVEPLACEMENT,   placement,
			       Hv_RESIZEPOLICY,        resizepolicy,
			       Hv_BALLOON,             (char *)"Use this to create a \"world\" polygon.",
			       NULL);
    
    if (Hv_CheckBit(whichtools, Hv_WEDGETOOL))
	Item = Hv_VaCreateItem(View,
			       Hv_TYPE,                Hv_BUTTONITEM,
			       Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
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
			       Hv_DRAWCONTROL,         Hv_LIGHTWEIGHT,
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
			       Hv_Rect *area) {
    
/* archaic version of Hv_ButtonDrawingArea */

    Hv_ButtonDrawingArea(Item, area);
}


/**
 * Hv_StandardredoItem
 * @purpose Mostly used to redraw an item that is being edited and
 * the user has selected "Apply"
 * @param Item    The Hv_Item to "redo".
 */

void Hv_StandardRedoItem(Hv_Item Item) {
    
/* typically called to redraw an item in response
   to an "apply" selection */
    
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
    Hv_ShrinkRegion(totalregion, -2, -2);
    
/* if item is saved in background it needs special care */
    
    if (Hv_CheckItemDrawControlBit(Item, Hv_INBACKGROUND))
	Hv_OffScreenViewUpdate(Item->view, totalregion);
    else
	Hv_DrawView(Item->view, totalregion);
    
    Hv_DestroyRegion(oldregion);
    Hv_DestroyRegion(newregion);
    Hv_DestroyRegion(totalregion);
}

/**
 * Hv_StandardLabel
 * @purpose Convenience routine for adding a label to a dialog
 * @param   rc     The parent (usually a row-col widget)
 * @param   label  The label string
 * @param   type   Not so obvious control of font and color
 * <p>
 * @table
 * |R type   | Foreground  | background  | Font |r
 * |R -1003  | Hv_coral    | none        | Hv_times11 |r
 * |R -1002  | Hv_red      | none        | Hv_times11 |r
 * |R -1001  | Hv_darkBlue | none        | Hv_times11 |r
 * |R -1000  | Hv_black    | none        | Hv_times11 |r
 * |R -999   | Hv_red      | Hv_gray14   | Hv_times11 |r
 * |R -998   | Hv_black    | Hv_wheat    | Hv_times11 |r
 * |R -997   | Hv_black    | Hv_honeydew | Hv_times11 |r
 * |R -996   | Hv_red      | Hv_honeydew | Hv_times11 |r
 * |R -3     | Hv_coral    | none        | Hv_fixed2 |r
 * |R -2     | Hv_red      | none        | Hv_fixed2 |r
 * |R -1     | Hv_darkBlue | none        | Hv_fixed2 |r
 * |R 0      | Hv_black    | none        | Hv_fixed2 |r
 * |R 1      | Hv_red      | Hv_gray14   | Hv_fixed2 |r
 * |R 2      | Hv_black    | Hv_wheat    | Hv_fixed2 |r
 * |R 3      | Hv_black    | Hv_honeydew | Hv_fixed2 |r
 * |R 4      | Hv_red      | Hv_honeydew | Hv_fixed2 |r
 * |R 97     | Hv_coral    | none        | Hv_helveticaBold14 |r
 * |R 98     | Hv_red      | none        | Hv_helveticaBold14 |r
 * |R 99     | Hv_darkBlue | none        | Hv_helveticaBold14 |r
 * |R 100    | Hv_black    | none        | Hv_helveticaBold14 |r
 * |R 101    | Hv_red      | Hv_gray14   | Hv_helveticaBold14 |r
 * |R 102    | Hv_black    | Hv_wheat    | Hv_helveticaBold14 |r
 * |R 103    | Hv_black    | Hv_honeydew | Hv_helveticaBold14 |r
 * |R 104    | Hv_red      | Hv_honeydew | Hv_helveticaBold14 |r
 * |R 997    | Hv_coral    | none        | Hv_helveticaBold11 |r
 * |R 998    | Hv_red      | none        | Hv_helveticaBold11 |r
 * |R 999    | Hv_darkBlue | none        | Hv_helveticaBold11 |r
 * |R 1000   | Hv_black    | none        | Hv_helveticaBold11 |r
 * |R 1001   | Hv_red      | Hv_gray14   | Hv_helveticaBold11 |r
 * |R 1002   | Hv_black    | Hv_wheat    | Hv_helveticaBold11 |r
 * |R 1003   | Hv_black    | Hv_honeydew | Hv_helveticaBold11 |r
 * |R 1004   | Hv_red      | Hv_honeydew | Hv_helveticaBold11 |r
 * |R 9997   | Hv_coral    | none        | Hv_fixed |r
 * |R 9998   | Hv_red      | none        | Hv_fixed |r
 * |R 9999   | Hv_darkBlue | none        | Hv_fixed |r
 * |R 10000  | Hv_black    | none        | Hv_fixed |r
 * |R 10001  | Hv_red      | Hv_gray14   | Hv_fixed |r
 * |R 10002  | Hv_black    | Hv_wheat    | Hv_fixed |r
 * |R 10003  | Hv_black    | Hv_honeydew | Hv_fixed |r
 * |R 10004  | Hv_red      | Hv_honeydew | Hv_fixed |r
 * |R 97     | Hv_coral    | none        | Hv_helveticaBold14 |r
 * |R 98     | Hv_red      | none        | Hv_helveticaBold14 |r
 * |R 99     | Hv_darkBlue | none        | Hv_helveticaBold14 |r
 * |R 100    | Hv_black    | none        | Hv_helveticaBold14 |r
 * |R 101    | Hv_red      | Hv_gray14   | Hv_helveticaBold14 |r
 * |R 102    | Hv_black    | Hv_wheat    | Hv_helveticaBold14 |r
 * |R 103    | Hv_black    | Hv_honeydew | Hv_helveticaBold14 |r
 * |R 104    | Hv_red      | Hv_honeydew | Hv_helveticaBold14 |r
 * |R 99997  | Hv_coral    | none        | Hv_helveticaBold12 |r
 * |R 99998  | Hv_red      | none        | Hv_helveticaBold12 |r
 * |R 99999  | Hv_darkBlue | none        | Hv_helveticaBold12 |r
 * |R 100000 | Hv_black    | none        | Hv_helveticaBold12 |r
 * |R 100001 | Hv_red      | Hv_gray14   | Hv_helveticaBold12 |r
 * |R 100002 | Hv_black    | Hv_wheat    | Hv_helveticaBold12 |r
 * |R 100003 | Hv_black    | Hv_honeydew | Hv_helveticaBold12 |r
 * |R 100004 | Hv_red      | Hv_honeydew | Hv_helveticaBold12 |r
 * IR othe r | Hv_black    | none        | Hv_fixed2|r
 * <p>
 * @return  The label widget (dialog item).
 */


Hv_Widget Hv_StandardLabel(Hv_Widget  rc,
			   char      *label,
			   int        type) {
    
/* a simple label with different foregrounds/backgrounds
   depending on type */
    
    short      font = Hv_fixed2;
    short      fg;
    short      bg;
    Hv_Widget  dummy;
    
    fg = Hv_black;
    bg = -1;
    
    if (type <= -995) {
	font = Hv_times11;
	type += 1000;
    }

    if (type >= 99900) {
	font = Hv_fixed;
	type -= 100000;
    }
    
    if (type >= 9990) {
	font = Hv_helveticaBold11;
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



/**
 * Hv_StandardClearListButton
 * @purpose Add a button to a dialog that will clear a list
 * @param parent  Parent widget, usually a row-col
 * @param list    The list to be "cleared".
 * @param label   The label on the button
 * @param font    The font used on the button
 * @return  The button dialog item.
 */


Hv_Widget Hv_StandardClearListButton(Hv_Widget  parent,
				     Hv_Widget  list,
				     char    *label,
				     short    font) {
    
    Hv_Widget  w;
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

static void Hv_ClearListCB(Hv_Widget   w,
			   Hv_Pointer data) {
    
    Hv_Widget    list;
    
    if (data == NULL)
	return;
    
    list = (Hv_Widget)data;
    Hv_DeselectAllListItems(list);
}

/*------ Hv_StandardFileSelectorItem --------*/

Hv_Widget  Hv_StandardFileSelectorItem(Hv_Widget parent,
				       short      width,
				       char      *mask) {
    
/* adds a Dat Cao file selector */
    
    Hv_Widget    w;
    
    w = Hv_VaCreateDialogItem(parent,
			      Hv_TYPE,      Hv_FILESELECTORDIALOGITEM,
			      Hv_MASK,      mask,
			      Hv_WIDTH,     width,
			      Hv_FONT,      Hv_fixed2,
			      NULL);
    return w;
}


/*------ Hv_StandardDeleteButton --------*/

Hv_Widget  Hv_StandardDeleteButton(Hv_Widget parent) {
    
/* adds a delete button */
    
    Hv_Widget    w;
    
    w = Hv_VaCreateDialogItem(parent,
			      Hv_TYPE,      Hv_BUTTONDIALOGITEM,
			      Hv_LABEL,     (char *)" Delete ",
			      Hv_FONT,      Hv_fixed2,
			      NULL);
    return w;
}

/*------ Hv_StandardButton --------*/

Hv_Widget Hv_StandardButton(Hv_Widget      parent,
			    char          *label,
			    Hv_FunctionPtr CB) {
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
			   Hv_FunctionPtr CB) {
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

Hv_Widget  Hv_StandardTextField(Hv_Widget parent) {
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

Hv_Widget Hv_StandardToggleButton(Hv_Widget       parent,
				  char           *label,
				  Hv_FunctionPtr  CB) {
    Hv_Widget    w;
    
    w  = Hv_VaCreateDialogItem(parent,
			       Hv_TYPE,     Hv_TOGGLEDIALOGITEM,
			       Hv_LABEL,    label,
			       Hv_FONT,     Hv_fixed2,
			       Hv_CALLBACK, CB,
			       NULL);
    
    return  w;
}





/**
 * Hv_StandardActionButtons
 * @purpose Standard closeout buttons for a dialog.
 * @param   parent   Usually the dialog
 * @param   whichones  Bitwise description of which buttons to add. Comprised of the bits
 * (Hv_OKBUTTON, Hv_CANCELBUTTON, Hv_APPLYBUTTON, Hv_DELETEBUTTON);
 * @return  The action area widget
 */


Hv_Widget Hv_StandardActionButtons(Hv_Widget  parent,
				   short      spacing,
				   int        whichones) {
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



/**
 * Hv_StandardActionArea
 * @purpose Replacement for Hv_StandardActionButtons which produces
 * equally spaced action buttons but CANNOT be added to
 * so for example the color picker which might add a nocolor
 * button cannot use this method
 * @param   parent   Usually the dialog
 * @param   whichones  Bitwise description of which buttons to add. Comprised of the bits
 * (Hv_OKBUTTON, Hv_CANCELBUTTON, Hv_APPLYBUTTON, Hv_DELETEBUTTON);
 * @return  The action area widget
 */


Hv_Widget Hv_StandardActionArea(Hv_Widget  parent,
				int     whichones) {
    
    Hv_Widget    w;



#ifdef WIN32
     w = Hv_StandardActionButtons(parent, 20, whichones);
#else

    Hv_SimpleDialogSeparator(parent);
    w  = Hv_VaCreateDialogItem(parent,
			       Hv_TYPE,            Hv_ACTIONAREADIALOGITEM,
			       Hv_DIALOGCLOSEOUT,  whichones,
			       NULL);
#endif
    return  w;
}

/**
 * Hv_DialogTable
 * @purpose Create a table to hold other dialog items.
 * Items will be put in so as to have the number of rows
 * specified. Thus if nrows = 4 and you add 8
 * widgets, you'll get 4 rows and two columns.
 * Rows will be filled first.
 * @param   parent   Usually the dialog
 * @param   numrow   The number of rows
 * @param   spacing  Cell spacing in pixels
 * @return  The row column table
 * @deprecated Use Hv_DialogTable
 */


Hv_Widget Hv_DialogTable(Hv_Widget  parent,
			 short      numrow,
			 short      spacing) {
    
    Hv_Widget    w;
    
    w = Hv_VaCreateDialogItem(parent,
			      Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
			      Hv_NUMROWS,      numrow,
			      Hv_PACKING,      Hv_PAD,
			      Hv_SPACING,      spacing,
			      NULL);
    return  w;
}

/**
 * Hv_TightDialogTable
 * @purpose Create a tightly packed table to hold other dialog items.
 * Items will be put in so as to have the number of rows
 * specified. Thus if nrows = 4 and you add 8
 * widgets, you'll get 4 rows and two columns.
 * Rows will be filled first.
 * @param   parent   Usually the dialog
 * @param   numrow   The number of rows
 * @param   spacing  Cell spacing in pixels
 * @return  The row column table
 * @deprecated Use Hv_DialogTable
 */

Hv_Widget Hv_TightDialogTable(Hv_Widget  parent,
			      short      numrow,
			      short      spacing) {
    
    Hv_Widget    w;
    
    w = Hv_VaCreateDialogItem(parent,
			      Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
			      Hv_NUMROWS,      numrow,
			      Hv_PACKING,      Hv_TIGHT,
			      Hv_SPACING,      spacing,
			      NULL);
    return  w;
}


/**
 * Hv_StandardPackingRowCol
 * @purpose Create a table to hold other dialog items
 * @param   parent   Usually the dialog
 * @param   numrow   The number of rows
 * @param   spacing  Cell spacing in pixels
 * @return  The row column table
 * @deprecated Use Hv_DialogTable
 */

Hv_Widget Hv_StandardPackingRowCol(Hv_Widget  parent,
				   short      numrow,
				   short      spacing) {
    return Hv_DialogTable(parent, numrow, spacing);
}


/**
 * Hv_DialogRow
 * @purpose Used for creating container rows to
 * hold dialog widgets.
 * @param   parent   A dialog or another row-col widget,
 * @param   spacing  Cell spacing in pixels
 * @return  The row column widget
 */

Hv_Widget Hv_DialogRow(Hv_Widget  parent,
		       short    spacing) {
    return Hv_DialogTable(parent, 1, spacing);
}


/**
 * Hv_DialogColumn
 * @purpose Used for creating container columns to
 * hold dialog widgets. The main row-col for a dialog is
 * often one of these.
 * @param   parent   A dialog or another row-col widget,
 * @param   spacing  Cell spacing in pixels
 * @return  The row column widget
 */

Hv_Widget Hv_DialogColumn(Hv_Widget  parent,
			  short      spacing) {
    Hv_Widget    w;
    
    w = Hv_VaCreateDialogItem(parent,
			      Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
			      Hv_ORIENTATION,  Hv_VERTICAL,
			      Hv_SPACING,      spacing,
			      NULL);
    
    return w;
}


/**
 * Hv_TightDialogRow
 * @purpose Used for creating tightly packed container rows to
 * hold dialog widgets.
 * @param   parent   A dialog or another row-col widget,
 * @param   spacing  Cell spacing in pixels
 * @return  The row column widget
 */

Hv_Widget Hv_TightDialogRow(Hv_Widget  parent,
			    short    spacing) {
    
    return Hv_TightDialogTable(parent, 1, spacing);
}


/**
 * Hv_TightDialogColumn
 * @purpose Used for creating tightly packed container columns to
 * hold dialog widgets.
 * @param   parent   A dialog or another row-col widget,
 * @param   spacing  Cell spacing in pixels
 * @return  The row column widget
 */

Hv_Widget Hv_TightDialogColumn(Hv_Widget  parent,
			       short      spacing) {
    Hv_Widget    w;
    
    w = Hv_VaCreateDialogItem(parent,
			      Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
			      Hv_ORIENTATION,  Hv_VERTICAL,
			      Hv_SPACING,      spacing,
			      Hv_PACKING,      Hv_TIGHT,
			      NULL);
    
    return w;
}


/**
 * Hv_StandardMainRowCol
 * @purpose Create a column for holding other dialog items. 
 * @param   parent   Usually the dialog
 * @param   spacing  Cell spacing in pixels
 * @return  The row column widget suitable for many simple dialogs
 * @deprecated Use Hv_DialogColumn
 */

Hv_Widget Hv_StandardMainRowCol(Hv_Widget  parent,
				short     spacing) {
    return Hv_DialogColumn(parent, spacing);
}





