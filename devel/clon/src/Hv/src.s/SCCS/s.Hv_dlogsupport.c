h46553
s 00000/00000/00000
d R 1.2 02/09/09 11:30:23 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/s/Hv_dlogsupport.c
e
s 00502/00000/00000
d D 1.1 02/09/09 11:30:22 boiarino 1 0
c date and time created 02/09/09 11:30:22 by boiarino
e
u
U
f e 0
t
T
I 1
/**
 * <EM>Various support routines for dialogs.</EM>
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

static void Hv_DeleteItemCB(Hv_Widget,
			    Hv_Pointer);


/*------- Hv_GetOptionMenuButtons ------*/

void Hv_GetOptionMenuButtons(Hv_Widget       optmenu,
			     Hv_WidgetList   *buttons,
			     int             *num_buttons)

{
  Hv_Widget     menu = NULL;

  Hv_VaGetValues(optmenu, Hv_NsubMenuId, &menu, NULL);

  Hv_VaGetValues(menu,
		 Hv_NnumChildren, num_buttons,
		 Hv_Nchildren,    buttons,
		 NULL) ;

}

/*------- Hv_SetOptionMenu -------*/


void Hv_SetOptionMenu(Hv_Widget     optmenu,
		      Hv_WidgetList buttons,
		      int           whichone)

{
  Hv_VaSetValues(optmenu, Hv_NmenuHistory, buttons[whichone], NULL ) ;
}

/*------- Hv_PosGetOptionMenu -------*/


short Hv_PosGetOptionMenu(Hv_Widget     optmenu,
			  Hv_WidgetList buttons,
			  int           num_buttons)

/*** Returns a C INDEX ***/

{
  int            i;
  Hv_Widget      w = NULL;

  Hv_VaGetValues(optmenu, Hv_NmenuHistory, &w, NULL);

  for (i = 0; i < num_buttons; i++)
    if (w == buttons[i])
      return i;

  return -1;  /* failure */
}


/*------- Hv_GetDialogAnswer -----*/

int Hv_GetDialogAnswer(Hv_Widget dialog)

{
  Hv_DialogData     ddata;

  ddata = Hv_GetDialogData(dialog);
  if (ddata == NULL)
    return -1;
  else
    return ddata->answer;
}



/*--------- Hv_GetDialogActionButton -----*/

Hv_Widget    Hv_GetDialogActionButton(Hv_Widget dialog,
				      int       whichone)

/* returns one of the action buttons: OK, Cancel, Apply or delete */

{
  Hv_DialogData     ddata;

  if (dialog == NULL)
    return NULL;

  ddata = Hv_GetDialogData(dialog);

  if (ddata == NULL)
    return NULL;

  if (whichone == Hv_OKBUTTON)
    return ddata->ok;
  else if (whichone == Hv_CANCELBUTTON)
    return ddata->cancel;
  else if (whichone == Hv_APPLYBUTTON)
    return ddata->apply;
  else if (whichone == Hv_DELETEBUTTON)
    return ddata->del;
  else
    return NULL;
}

/*------- Hv_SetDeleteItem -----*/


void   Hv_SetDeleteItem(Hv_Widget dialog,
			Hv_Item   Item)

/* This is for dialogs that have a delete item.
   It specifies what item should be deleted
   if the user hits delete */

{
  Hv_Widget   del;
  Hv_DialogData  ddata;

  if (dialog == NULL)
    return;

  ddata = Hv_GetDialogData(dialog);
  if (ddata == NULL)
      return;

  ddata->deleteitem = Item;
  if (Item == NULL)
      return;

  Hv_LockItem(Item);

  del = Hv_GetDialogActionButton(dialog, Hv_DELETEBUTTON);
  if (del == NULL)
    return;

  Hv_RemoveAllCallbacks(del, Hv_activateCallback);
  Hv_AddCallback(del,
		 Hv_activateCallback,
		 (Hv_CallbackProc)Hv_DeleteItemCB,
		 (Hv_Pointer)Item);
}


/* ----- Hv_DeleteItemCB ------- */

static void Hv_DeleteItemCB(Hv_Widget w,
			    Hv_Pointer client_data)

{
  Hv_Widget       dialog;
  Hv_DialogData   ddata;
  Hv_Item         Item;

  dialog = Hv_GetOwnerDialog(w);
  ddata = Hv_GetDialogData(dialog);

  if (ddata == NULL)
      return;

  Item = ddata->deleteitem;

  if (Item == NULL)
      return;

/* this will delete unlocked items, sincce the purpose
   of the lock is to prevent the item from being deleted
   while the dialog is up, not to prevent the dialog from deleting
   it */

  Hv_UnlockItem(Item);
  Hv_DeleteItem(Item);

  ddata->deleteitem = NULL;
  ddata->answer = Hv_DELETE;
}

/*-------- Hv_SimpleToggleButton ---------*/

Hv_Widget   Hv_SimpleToggleButton(Hv_Widget parent,
				  char *label) {

  Hv_Widget      w;

  w  = Hv_VaCreateDialogItem(parent,
			     Hv_TYPE,     Hv_TOGGLEDIALOGITEM,
			     Hv_LABEL,    label,
			     Hv_FONT,     Hv_fixed2,
			     NULL);

  return w;
}

/*----------- Hv_SimpleTextEdit -------*/

Hv_Widget  Hv_SimpleTextEdit(Hv_Widget parent,
			     char      *label,
			     char      *defaulttxt,
			     short     ncol) {

/* a convenience function for creating a label and edit box  */

  Hv_Widget   w;

  w = Hv_VaCreateDialogItem(parent,
			    Hv_TYPE,        Hv_TEXTDIALOGITEM,
			    Hv_DEFAULTTEXT, defaulttxt,
			    Hv_LABEL,       label,
			    Hv_FONT,        Hv_fixed2,
			    Hv_NUMCOLUMNS,  ncol,
			    NULL);

  return w;
}

/*----------- Hv_SimpleColorLabel -------*/

Hv_Widget  Hv_SimpleColorLabel(Hv_Widget      parent,
			       char          *label,
			       Hv_FunctionPtr editor) {

/* a convenience function for creating a label for picking a color */

  Hv_Widget     dummy, w, rc;
  
  rc = Hv_TightDialogRow(parent, 0);


  dummy = Hv_VaCreateDialogItem(rc,
				Hv_TYPE,        Hv_LABELDIALOGITEM,
				Hv_LABEL,       label,
				Hv_FONT,        Hv_fixed2,
				Hv_BORDERCOLOR, Hv_simpleTextBorder,
				NULL);


  w = Hv_VaCreateDialogItem(rc,
			    Hv_TYPE,        Hv_LABELDIALOGITEM,
			    Hv_LABEL,       "      ",
			    Hv_BORDERWIDTH, 1,
			    Hv_BORDERCOLOR, Hv_white,
			    Hv_FONT,        Hv_helvetica17,
			    NULL);

  if (editor != NULL)
    Hv_AddEventHandler(w, Hv_buttonPressMask, (Hv_EventHandler)editor, NULL);  
  
  return w;
}

/**
 * Hv_SimplePatternLabel
 * @purpose  A convenience function for creating a label for picking a pattern.
 * @param   parent   The parent, usually a dialog RowCol widget.
 * @param   label    Identifying label.
 * @param   editor   Function that will do the editing if this is "clicked".
 * @return    The widget for editing a pattern.
 */

Hv_Widget  Hv_SimplePatternLabel(Hv_Widget      parent,
				 char          *label,
				 Hv_FunctionPtr editor) {


  Hv_Widget     dummy, w;
  Hv_Widget     rc;

  rc = Hv_TightDialogRow(parent, 4);

  dummy = Hv_VaCreateDialogItem(rc,
				Hv_TYPE,        Hv_LABELDIALOGITEM,
				Hv_LABEL,       label,
				Hv_FONT,        Hv_fixed2,
				Hv_BORDERCOLOR, Hv_simpleTextBorder,
				NULL);



  w = Hv_VaCreateDialogItem(rc,
			    Hv_TYPE,        Hv_LABELDIALOGITEM,
			    Hv_LABEL,       "   ",
			    Hv_BORDERWIDTH, 1,

			    Hv_BORDERCOLOR, Hv_black,
			    Hv_FONT,        Hv_fixed2,
			    Hv_TEXTCOLOR,   Hv_black,
			    Hv_BACKGROUND,  Hv_white,
			    Hv_WIDTH,       22,
			    Hv_HEIGHT,      10,
			    NULL);

  Hv_VaSetValues(w, Hv_NlabelType, Hv_PIXMAP, NULL);
  Hv_ChangeLabelPattern(w, Hv_HOLLOWPAT);

  if (editor != NULL)
    Hv_AddEventHandler(w, Hv_buttonPressMask, (Hv_EventHandler)editor, NULL);  
  
  return (Hv_Widget)w;
}




/**
 *  Hv_VariableZoom 
 *  @purpose Ask the user for a factor and then zooms
 *  by that amount. If the factor is less
 *  than 1 it zooms in, otherwise zooms out.
 *  @param View    The View in question.
 */

void Hv_VariableZoom(Hv_View View) {

/* ask the user for a factor and then zooms
   by that amount. If the factor is less
   tahn 1 it zooms it, otherwise zoomsout */

  static  Hv_Widget   dialog = NULL;
  static  Hv_Widget   amount;
  Hv_Widget           rowcol, rc, dummy;      
  float            value;

  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, "Variable Zoom", NULL);

    rowcol = Hv_DialogColumn(dialog, 6);
    rc = Hv_DialogRow(rowcol, 6);
    amount = Hv_SimpleTextEdit(rc, "Scale Factor ", NULL, 4);
    dummy = Hv_StandardActionButtons(rowcol, 60, Hv_OKBUTTON + Hv_CANCELBUTTON);
  }

/* the dialog has been created */

  Hv_SetFloatText(amount, 1.0, 2);   /*default to 1.0*/

  if (Hv_DoDialog(dialog, NULL) == Hv_OK) {
    value = Hv_GetFloatText(amount);
    value = (float)fabs(value);
    Hv_PercentZoom(View, value);
  }
}


/*------ Hv_ChangeLabelColors ---- */

void Hv_ChangeLabelColors(Hv_Widget label,
			  short     fg,
			  short     bg)

{
  Hv_SetWidgetBackgroundColor(label, bg);
  Hv_SetWidgetForegroundColor(label, fg);
}

/* ---------- Hv_SetFloatText ----------- */

void	Hv_SetFloatText(Hv_Widget w,
			float     val,
			int       p)

/* place a string rep of the float "val" into the widget "w"
   using precision "p" */

#define    tiny   (float)1.0e-30
{
  char          str[60];

  if (w == NULL)
    return;

  Hv_NumberToString(str, val, (short)p, tiny);          /* convert val to string */
  Hv_SetString(w, str);                  /* copy temp string to button's string */

/* place the cursor at the end */

  Hv_VaSetValues(w, Hv_NcursorPosition, strlen(str), NULL);
}
#undef     tiny

/* ---------- Hv_GetFloatText ----------- */

float	Hv_GetFloatText(Hv_Widget w)

/* Convert the string field of passed button to a float
   -- i.e. retrieve number from a edit field */

{
  char	       *temptext;
  float		val;

  if (w == NULL)
    return 0.0;

  temptext = Hv_GetString(w);
  val = (float)atof(temptext);
  Hv_Free(temptext);
  return val;
}

/* ---------- Hv_SetIntText ----------- */

void	Hv_SetIntText(Hv_Widget w,
		      int       val)

/* place a string rep of the int "val" into the widget "w"*/

{
  char          str[60];

  if (w == NULL)
    return;

  Hv_NumberToString(str, (float)val, -1, 0.0);    /* convert val to string */
  Hv_SetString(w, str);                  /* copy temp string to button's string */

/* place the cursor at the end */

  Hv_VaSetValues(w, Hv_NcursorPosition, strlen(str), NULL);
}

/* ---------- Hv_GetIntText ----------- */

int	Hv_GetIntText(Hv_Widget w)

/* Convert the string field of passed button to a int
   -- i.e. retrieve number from a edit field */

{
  char		*temptext;
  int		val;

  if (w == NULL)
    return -1;
  
  temptext = Hv_GetString(w);
  val = atoi(temptext);
  Hv_Free(temptext);
  return val;
}


/**
 * Hv_SimpleDialogSeparator
 * @purpose Create a simple dialog separator
 * @param   The parent widget (usually a rowcol widget)
 */

Hv_Widget  Hv_SimpleDialogSeparator(Hv_Widget parent) {
  Hv_Widget     w;
  
  w  = Hv_VaCreateDialogItem(parent,
			     Hv_TYPE,     Hv_SEPARATORDIALOGITEM,
			     NULL);
  
  return w;
}

/**
 * Hv_NotYet
 * @purpose  Displays a dialog box that says that feature is not ready.
 */


void Hv_NotYet(void) {
  Hv_Warning ("Not Yet Implemented.");
}

/*------- Hv_SetScaleMinMax --------- (also exist in Hv_scaledlogs.c !!!)
 * @purpose Set the min and max value for a scale dialog item
 * @param   w   The scale widget
 * @param   min The new minimum value
 * @param   max The new maximum value
*/

void  Hv_SetScaleMinMax(Hv_Widget w,
			int min,
			int max)

{
  XtVaSetValues(w,
		XmNminimum, min,
		XmNmaximum, max,
		NULL);
}




E 1
