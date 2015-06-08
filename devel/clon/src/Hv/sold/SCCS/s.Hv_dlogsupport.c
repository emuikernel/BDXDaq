h41207
s 00000/00000/00000
d R 1.2 02/08/25 23:21:09 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_dlogsupport.c
e
s 00599/00000/00000
d D 1.1 02/08/25 23:21:08 boiarino 1 0
c date and time created 02/08/25 23:21:08 by boiarino
e
u
U
f e 0
t
T
I 1
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
#include "Xm/DialogS.h"

static void Hv_DeleteItemCB(Hv_Widget,
			    XtPointer);


/*------- Hv_GetOptionMenuButtons ------*/

void Hv_GetOptionMenuButtons(Hv_Widget       optmenu,
			     Hv_WidgetList   *buttons,
			     int             *num_buttons)

{
  Widget     menu;

  XtVaGetValues(optmenu, XmNsubMenuId, &menu, NULL);

  XtVaGetValues(menu, XmNnumChildren, num_buttons,
		XmNchildren, buttons, NULL) ;

}

/*------- Hv_SetOptionMenu -------*/


void Hv_SetOptionMenu(Hv_Widget     optmenu,
		      Hv_WidgetList buttons,
		      int           whichone)

{
  XtVaSetValues(optmenu, XmNmenuHistory, buttons[whichone], NULL ) ;
}

/*------- Hv_PosGetOptionMenu -------*/


short Hv_PosGetOptionMenu(Hv_Widget     optmenu,
			  Hv_WidgetList buttons,
			  int           num_buttons)

/*** Returns a C INDEX ***/

{
  int            i;
  Hv_Widget      w;

  XtVaGetValues(optmenu, XmNmenuHistory, &w, NULL);

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


/*------- Hv_GetOwnerDialog -----*/

Hv_Widget       Hv_GetOwnerDialog(Hv_Widget ditem)

/* given a dialog item, this returns the dialog
   that it is in, even if the dialog is not the immediate
   parent */

{
 Widget   parent;

 if (ditem == NULL)
   return NULL;

 parent = XtParent(ditem);

/* keep going up tree untill the PARENT is a dialog SHELL */

  while (!XmIsDialogShell(parent)) {
    ditem = parent;
    parent = XtParent(ditem);
    if (parent == NULL)
      return NULL;
  }

  return ditem;
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

  XtRemoveAllCallbacks(del, XmNactivateCallback);
  XtAddCallback(del,
		XmNactivateCallback,
		(XtCallbackProc)Hv_DeleteItemCB,
		(XtPointer)Item);
}


/* ----- Hv_DeleteItemCB ------- */

static void Hv_DeleteItemCB(Hv_Widget w,
			    XtPointer client_data)

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
				  char *label)

{
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
			     short     ncol)

/* a convenience function for creating a label and edit box  */

{
  Widget   w;
  short    bw = 1;

  if (Hv_simpleTextBorder < Hv_red)
    bw = -1;

  w = Hv_VaCreateDialogItem(parent,
			    Hv_TYPE,        Hv_TEXTDIALOGITEM,
			    Hv_DEFAULTTEXT, defaulttxt,
			    Hv_LABEL,       label,
			    Hv_TEXTCOLOR,   Hv_simpleTextFg,
			    Hv_BACKGROUND,  Hv_simpleTextBg, 
			    Hv_BORDERWIDTH, bw,
			    Hv_BORDERCOLOR, Hv_simpleTextBorder,
			    Hv_FONT,        Hv_fixed2,
			    Hv_NUMCOLUMNS,  ncol,
			    NULL);

  return w;
}

/*----------- Hv_SimpleColorLabel -------*/

Hv_Widget  Hv_SimpleColorLabel(Hv_Widget      parent,
			       char          *label,
			       Hv_FunctionPtr editor)

/* a convenience function for craeting a label for picking a color */

{
  Widget     dummy, w, rc;
  short    bw = 1;

  if (Hv_simpleTextBorder < Hv_red)
    bw = -1;
  


  dummy = (Widget)Hv_VaCreateDialogItem(parent,
					Hv_TYPE,        Hv_LABELDIALOGITEM,
					Hv_LABEL,       label,
					Hv_FONT,        Hv_fixed2,
					Hv_TEXTCOLOR,   Hv_simpleTextFg,
					Hv_BACKGROUND,  Hv_simpleTextBg,
					Hv_BORDERWIDTH, bw,
					Hv_BORDERCOLOR, Hv_simpleTextBorder,
					NULL);

  rc = Hv_DialogRow(parent, 0);

  w = (Widget)Hv_VaCreateDialogItem(rc,
				    Hv_TYPE,        Hv_LABELDIALOGITEM,
				    Hv_LABEL,       (char *)"    ",
				    Hv_BORDERWIDTH, 1,
				    Hv_BORDERCOLOR, Hv_white,
				    Hv_FONT,        Hv_fixed2,
				    NULL);

  if (editor != NULL)
    XtAddEventHandler(w, ButtonPressMask, FALSE, (XtEventHandler)editor, NULL);  
  
  return (Hv_Widget)w;
}

/*----------- Hv_SimplePatternLabel -------*/

Hv_Widget  Hv_SimplePatternLabel(Hv_Widget      parent,
				 char          *label,
				 Hv_FunctionPtr editor)

/* a convenience function for creating a label for picking a color */

{
  Widget     dummy, w;
  Widget     rc;
  short    bw = 1;

  if (Hv_simpleTextBorder < Hv_red)
    bw = -1;

  dummy = (Widget)Hv_VaCreateDialogItem(parent,
					Hv_TYPE,        Hv_LABELDIALOGITEM,
					Hv_LABEL,       label,
					Hv_FONT,        Hv_fixed2,
					Hv_TEXTCOLOR,   Hv_simpleTextFg,
					Hv_BACKGROUND,  Hv_simpleTextBg,
					Hv_BORDERWIDTH, bw,
					Hv_BORDERCOLOR, Hv_simpleTextBorder,
					NULL);

  rc = Hv_DialogRow(parent, 0);

  w = (Widget)Hv_VaCreateDialogItem(rc,
				    Hv_TYPE,        Hv_LABELDIALOGITEM,
				    Hv_LABEL,       (char *)"  ",
				    Hv_BORDERWIDTH, 1,
				    Hv_BORDERCOLOR, Hv_white,
				    Hv_FONT,        Hv_fixed2,
				    Hv_TEXTCOLOR,   Hv_black,
				    Hv_BACKGROUND,  Hv_white,
				    Hv_WIDTH,       16,
				    Hv_HEIGHT,      16,
				    NULL);

  XtVaSetValues(w, XmNlabelType, XmPIXMAP, NULL);

  if (editor != NULL)
    XtAddEventHandler(w, ButtonPressMask, FALSE, (XtEventHandler)editor, NULL);  
  
  return (Hv_Widget)w;
}


/* ---------- Hv_SetToggleButton ----------- */

void	Hv_SetToggleButton(Hv_Widget btn,
			   Boolean   State)

/* Set the state of a toggle button  True --> selected */

{
  XtVaSetValues(btn, XmNset, State, NULL);
}

/* ---------- Hv_GetToggleButton ----------- */

Boolean	Hv_GetToggleButton(Hv_Widget btn)

/* find the state of the passed toggle button (Selected --> True) */
  
{
  return  XmToggleButtonGadgetGetState(btn);
}


/* ----------- Hv_VariableZoom -------------*/

void Hv_VariableZoom(Hv_View View)

/* ask the user for a factor and then zooms
   by that amount. If the factor is less
   tahn 1 it zooms it, otherwise zoomsout */

{
  static  Widget   dialog = NULL;
  static  Widget   amount;
  Widget           rowcol, rc, dummy;      
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
    value = fabs(value);
    Hv_PercentZoom(View, value);
  }
}


/*--------- Hv_ChangeLabel --------*/

void  Hv_ChangeLabel(Hv_Widget w,
		     char      *label,
		     short      font)

/* change a widget (typically a button) label */

{
  XmString	xmst;             /* motif string */

  xmst = Hv_CreateMotifString(label, font);
  XtVaSetValues(w, XmNlabelString, xmst, NULL);
  XmStringFree(xmst);
}


/*------ Hv_ChangeLabelColors ---- */

void Hv_ChangeLabelColors(Hv_Widget label,
			  short     fg,
			  short     bg)

{
  Hv_SetWidgetBackgroundColor(label, bg);
  Hv_SetWidgetForegroundColor(label, fg);
}

/*------ Hv_ChangeLabelPattern ---- */

void Hv_ChangeLabelPattern(Hv_Widget label,
			   short     pattern)

{
  Pixmap       pix;
  unsigned int depth;
  int          wfg, wbg;
  Arg          args[10];
  int          ac = 0;

  if ((pattern < 0) || (pattern >= Hv_NUMPATTERNS))
    pattern = 0;

  ac = 0;
  XtSetArg(args[ac], XmNforeground, &wfg); ac++;
  XtSetArg(args[ac], XmNbackground, &wbg); ac++;
  XtGetValues(label, args, ac);
  depth = DefaultDepthOfScreen(DefaultScreenOfDisplay(Hv_display));

  pix = XCreatePixmapFromBitmapData(XtDisplay(Hv_toplevel),
				    RootWindowOfScreen(XtScreen(Hv_toplevel)),
				    (char *)Hv_bitmaps[pattern],
				    16, 16, wfg, wbg, depth);


  XtVaSetValues(label, XmNlabelPixmap, pix, NULL);
}


/* ---------- Hv_SetFloatText ----------- */

void	Hv_SetFloatText(Hv_Widget w,
			float     val,
			int       p)

/* place a string rep of the float "val" into the widget "w"
   using precision "p" */

#define    tiny   1.0e-30
{
  char          str[60];

  Hv_NumberToString(str, val, p, tiny);          /* convert val to string */
  Hv_SetString(w, str);                  /* copy temp string to button's string */

/* place the cursor at the end */

  XtVaSetValues(w, XmNcursorPosition, strlen(str), NULL);
}
#undef     tiny

/* ---------- Hv_GetFloatText ----------- */

float	Hv_GetFloatText(Hv_Widget w)

/* Convert the string field of passed button to a float
   -- i.e. retrieve number from a edit field */

{
  char	       *temptext;
  float		val;

  temptext = XmTextGetString(w);
  val = (float)atof(temptext);
  XtFree(temptext);
  return val;
}

/* ---------- Hv_SetIntText ----------- */

void	Hv_SetIntText(Hv_Widget w,
		      int       val)

/* place a string rep of the int "val" into the widget "w"*/

{
  char          str[60];

  Hv_NumberToString(str, (float)val, -1, 0.0);    /* convert val to string */
  Hv_SetString(w, str);                  /* copy temp string to button's string */

/* place the cursor at the end */

  XtVaSetValues(w, XmNcursorPosition, strlen(str), NULL);
}

/*---- Hv_SetString ----*/

void Hv_SetString(Hv_Widget w,
		  char      *s)

{
  XmTextSetString(w, s);
}

/*---- Hv_GetString ----*/

char  *Hv_GetString(Hv_Widget w)

{
  char  *tempstr;
  char  *s;

  tempstr = XmTextGetString(w);
  Hv_InitCharStr(&s, tempstr);
  XtFree(tempstr);
  return s;
}

/* ---------- Hv_GetIntText ----------- */

int	Hv_GetIntText(Hv_Widget w)

/* Convert the string field of passed button to a int
   -- i.e. retrieve number from a edit field */

{
  char		*temptext;
  int		val;
  
  temptext = XmTextGetString(w);
  val = atoi(temptext);
  XtFree(temptext);
  return val;
}


/* ----- Hv_NotYet ----------- */

void Hv_NotYet(void)

/* this routne displays a not yet implemented Warning dlog */

{
  Hv_Warning ("Not Yet Implemented.");
}


/* ---------- Hv_SimpleDialogSeparator -----*/

Hv_Widget  Hv_SimpleDialogSeparator(Hv_Widget parent)

{
  Hv_Widget     w;

  w  = Hv_VaCreateDialogItem(parent,
			     Hv_TYPE,     Hv_SEPARATORDIALOGITEM,
			     NULL);

  return w;
}






E 1
