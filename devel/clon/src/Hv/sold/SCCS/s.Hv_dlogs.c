h09900
s 00000/00000/00000
d R 1.2 02/08/25 23:21:08 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_dlogs.c
e
s 02111/00000/00000
d D 1.1 02/08/25 23:21:07 boiarino 1 0
c date and time created 02/08/25 23:21:07 by boiarino
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


/* ----- local variables ------*/

static   Widget   newcolorlabel, oldcolorlabel;
static   Widget   newpatternlabel, oldpatternlabel;
static   Boolean  AllowNone = False;

short    newcolor;
short    newpattern;

static   Widget     waitlabel;
XtIntervalId        updateid;


/*------ local prototypes ------*/

static void Hv_SelfDestructCB(Hv_TimerDialogData tdlogdata);

static void Hv_DialogTimerCB(XtPointer     xtptr,
			     XtIntervalId  *id);

static void Hv_SetWidgetPattern (Hv_Widget,
	   	                 int,
		                 XEvent *);

static Hv_DialogData  Hv_AddDialogToList(Hv_Widget);

static Hv_DialogData  Hv_MallocDialogData(Hv_Widget);

static void     Hv_NullCallback(Hv_Widget);

static void     Hv_WaitInfoTimeOut(XtPointer    xtptr,
				   XtIntervalId *id);

static void     Hv_WaitTimeOut(XtPointer,
			       XtIntervalId *);

static void     Hv_UpdateWait(XtPointer,
			       XtIntervalId *);

static Widget   Hv_CreateManagedSeparator(Hv_Widget);

static Widget   Hv_CreateManagedCloseOut(Hv_Widget,
					 short,
					 short,
					 short,
					 char *,
					 int);

static Widget   Hv_CreateManagedLabel(Hv_Widget,
				      char *,
				      short,
				      short,
				      short,
				      short,
				      short);

static Widget   Hv_CreateManagedList(Hv_Widget,
				     short,
				     short,
				     char,
				     char,
				     char **,
				     XtCallbackProc);

static Widget   Hv_CreateManagedToggleButton(Hv_Widget,
					     char *,
					     short,
					     XtCallbackProc,
					     void *);

static Widget   Hv_CreateManagedRadioButton(Hv_Widget,
					    char *,
					    short,
					    Hv_RadioHeadPtr *,
					    void *,
					    XtCallbackProc);

static void     Hv_StandardRadioButtonCallback(Hv_Widget,
					       XtPointer);

static Widget   Hv_CreateManagedTextField(Hv_Widget,
					  char *,
					  short,
					  short,
					  Boolean,
					  XtCallbackProc);

static Widget   Hv_CreateManagedLabelTextField(Hv_Widget,
					       char *,
					       short,
					       short,
					       short,
					       short,
					       short,
					       char *,
					       short,
					       Boolean,
					       XtCallbackProc);

static Widget   Hv_CreateManagedScrolledText(Hv_Widget,
					     short,
					     short,
					     short,
					     short);

static Widget   Hv_CreateManagedScale(Hv_Widget,
				      int,
				      int,
				      int,
				      int,
				      XtCallbackProc);

static Widget   Hv_CreateManagedColorSelector(Hv_Widget,
					      short,
					      short);

static Widget   Hv_CreateModalDialog(char *,
				     int);

static Widget   Hv_CreateModelessDialog(char *,
					int);

static Widget   Hv_CreateDialog(char *,
				unsigned char,
				int);

static void     Hv_SetColor(Hv_Widget,
			      int,
			      XEvent *);

static void     Hv_Response(Hv_Widget,
			    XtPointer,
			    XmAnyCallbackStruct *);

static void     Hv_CheckDefault(int *,
				int);

static Widget   Hv_CreateManagedRowCol(Hv_Widget,
				       short,
				       short,
				       short,
				       short,
				       short);

static int      Hv_EditPattern(short *,
			       char  *);

static int      Hv_EditColor(short *,
			     char  *);

/*----------- Hv_VaCreateDialog ---------*/

void Hv_VaCreateDialog(Hv_Widget *dialog,
		       ...)

/* the created dialog will be returned in the REQUIRED
   argument "dialog" */

{
  va_list           ap;
  Hv_AttributeArray attributes;

  va_start(ap, dialog);

/* Get the variable arguments */

  Hv_GetAttributes(ap, attributes, NULL, NULL, NULL, NULL, NULL);

  switch (attributes[Hv_TYPE].s) {
  case Hv_MODELESS:
    *dialog = Hv_CreateModelessDialog((char *)(attributes[Hv_TITLE].v),
				      attributes[Hv_WMIGNORECLOSE].i);
    break;
    
  default:
    *dialog = Hv_CreateModalDialog((char *)(attributes[Hv_TITLE].v),
				   attributes[Hv_WMIGNORECLOSE].i);
    break;
  }
  
}


/*----------- Hv_VaCreateDialogItem ---------*/

Hv_Widget  Hv_VaCreateDialogItem(Hv_Widget parent,
				      ...)

{
  va_list           ap;
  Hv_AttributeArray attributes;
  short             nrowcol, textcolor;
  Hv_Widget         ditem = NULL;
  short             numopt = 0;
  char              *optlist[Hv_MAXCHOICE];
  int               i;
  Hv_RadioHeadPtr  *radiohead;

/* first argument is the parent , it is required */

  va_start(ap, parent);

/* Now get the variable arguments */

  Hv_GetAttributes(ap, attributes, optlist, NULL, NULL, NULL, &numopt);

  switch (attributes[Hv_TYPE].s) {

  case Hv_COLORSELECTORDIALOGITEM:
    if ((attributes[Hv_WIDTH].s > 1) && (attributes[Hv_HEIGHT].s > 1))
      ditem = Hv_CreateManagedColorSelector(parent,
					    attributes[Hv_WIDTH].s,
					    attributes[Hv_HEIGHT].s);
    break;
    
  case Hv_LABELDIALOGITEM:
    if (attributes[Hv_TEXTCOLOR].s >= 0)
      textcolor = attributes[Hv_TEXTCOLOR].s;
    else
      textcolor = attributes[Hv_COLOR].s;
    
    ditem = Hv_CreateManagedLabel(parent,
				  (char *)(attributes[Hv_LABEL].v),
				  attributes[Hv_FONT].s,
				  textcolor,
				  attributes[Hv_BACKGROUND].s,
				  attributes[Hv_BORDERWIDTH].s,
				  attributes[Hv_BORDERCOLOR].s);
    break;
    
  case Hv_SEPARATORDIALOGITEM:
    ditem = Hv_CreateManagedSeparator(parent);
    break;
    
  case Hv_LISTDIALOGITEM:
    ditem = Hv_CreateManagedList(parent,
				 attributes[Hv_NUMITEMS].s,
				 attributes[Hv_NUMVISIBLEITEMS].s,
				 attributes[Hv_LISTSELECTION].c,
				 attributes[Hv_LISTRESIZEPOLICY].c,
				 (char **)(attributes[Hv_LIST].v),
				 (XtCallbackProc)(attributes[Hv_CALLBACK].fp));
    break;
    
  case Hv_ROWCOLUMNDIALOGITEM:

/* avoid confusion over which to use, NROWS or NCOLUMNS */
    
    nrowcol = Hv_sMax(attributes[Hv_NUMROWS].s, attributes[Hv_NUMCOLUMNS].s);
    ditem =  Hv_CreateManagedRowCol(parent,
				    attributes[Hv_SPACING].s,
				    attributes[Hv_ORIENTATION].s,
				    attributes[Hv_PACKING].s,
				    nrowcol,
				    attributes[Hv_ALIGNMENT].s);
    break;
    
  case Hv_SCROLLEDTEXTDIALOGITEM:
    ditem = Hv_CreateManagedScrolledText(parent,
					 (int)(attributes[Hv_NUMROWS].s),
					 (int)(attributes[Hv_NUMCOLUMNS].s),
					 attributes[Hv_BACKGROUND].s,
					 attributes[Hv_TEXTCOLOR].s);
    break;
    
  case Hv_SCALEDIALOGITEM:
    ditem = Hv_CreateManagedScale(parent,
				  (int)(attributes[Hv_ORIENTATION].s),
				  attributes[Hv_MINVALUE].i,
				  attributes[Hv_MAXVALUE].i,
				  attributes[Hv_CURRENTVALUE].i,
				  (XtCallbackProc)(attributes[Hv_CALLBACK].fp));
    break;
    
  case Hv_RADIODIALOGITEM:  /* if numopt = 0, one ad a time, otherwise many at once */
    
    if (numopt == 0)
      ditem = Hv_CreateManagedRadioButton(parent,
					  (char *)(attributes[Hv_LABEL].v),
					  attributes[Hv_FONT].s,
					  (Hv_RadioHeadPtr *)(attributes[Hv_RADIOLIST].v),
					  attributes[Hv_DATA].v,
					  (XtCallbackProc)(attributes[Hv_CALLBACK].fp));
    else {

/* must be the "first" call so radiolist should be NULL */

      radiohead = (Hv_RadioHeadPtr *)(attributes[Hv_RADIOLIST].v);
      *radiohead = NULL;

      for (i = 0; i < numopt; i++) {
	ditem = Hv_CreateManagedRadioButton(parent,
					    optlist[i],
					    attributes[Hv_FONT].s,
					    radiohead,
					    attributes[Hv_DATA].v,  /* if mass created, all share data */
					    (XtCallbackProc)(attributes[Hv_CALLBACK].fp));
	
      }
      Hv_PosSetActiveRadioButton(attributes[Hv_NOWON].s, *radiohead);
    }
    break;
    
  case Hv_TOGGLEDIALOGITEM:
    ditem = Hv_CreateManagedToggleButton(parent,
					 (char *)(attributes[Hv_LABEL].v),
					 attributes[Hv_FONT].s,
					 (XtCallbackProc)(attributes[Hv_CALLBACK].fp),
					 attributes[Hv_DATA].v);
    break;
    
  case Hv_TEXTDIALOGITEM:
    if (attributes[Hv_LABEL].v == NULL)
      ditem = Hv_CreateManagedTextField(parent,
					(char *)(attributes[Hv_DEFAULTTEXT].v),
					attributes[Hv_NUMCOLUMNS].s,
					attributes[Hv_FONT].s,
					(Boolean)attributes[Hv_EDITABLE].i,
					(XtCallbackProc)(attributes[Hv_VALUECHANGED].fp));
    
    else {
      if (attributes[Hv_TEXTCOLOR].s >= 0)
	textcolor = attributes[Hv_TEXTCOLOR].s;
      else
	textcolor = attributes[Hv_COLOR].s;

      ditem = Hv_CreateManagedLabelTextField(parent,
					     (char *)(attributes[Hv_LABEL].v),
					     attributes[Hv_FONT].s,
					     textcolor,
					     attributes[Hv_BACKGROUND].s,
					     attributes[Hv_BORDERWIDTH].s,
					     attributes[Hv_BORDERCOLOR].s,
					     (char *)(attributes[Hv_DEFAULTTEXT].v),
					     attributes[Hv_NUMCOLUMNS].s,
					     (Boolean)attributes[Hv_EDITABLE].i,
					     (XtCallbackProc)(attributes[Hv_VALUECHANGED].fp));
    }
    break;
    
  case Hv_CLOSEOUTDIALOGITEM:
    ditem = Hv_CreateManagedCloseOut(parent,
				     attributes[Hv_SPACING].s,
				     attributes[Hv_ORIENTATION].s,
				     attributes[Hv_FONT].s,
				     (char *)attributes[Hv_LABEL].v,
				     attributes[Hv_DIALOGCLOSEOUT].i);
    break;

  case Hv_BUTTONDIALOGITEM:
  default:   /* standard push button  */
    ditem = Hv_CreateManagedPushButton(parent,
				       (char *)(attributes[Hv_LABEL].v),
				       attributes[Hv_FONT].s,
				       (XtCallbackProc)(attributes[Hv_CALLBACK].fp),
				       (XtPointer)(attributes[Hv_DATA].v));
    break;
  }

  if (ditem != NULL)
    if ((attributes[Hv_LEFT].s > -32700) && (attributes[Hv_TOP].s > -32700))
      XtVaSetValues(ditem,
		    XmNx,    (Position)(attributes[Hv_LEFT].s),
		    XmNy,    (Position)(attributes[Hv_TOP].s),
		    NULL);


/* if any option strings, delete them */

  for (i = 0; i < numopt; i++)
    Hv_Free(optlist[i]);

  return ditem;
}



/*------ Hv_ModifyColor -------*/

int   Hv_ModifyColor(Hv_Widget w,
		     short    *color,
		     char     *prompt,
		     Boolean  AllowNoColor)

{
  int result;
 
  AllowNone = AllowNoColor;

  result = Hv_EditColor(color, prompt);

  if (w != NULL)
    Hv_SetWidgetBackgroundColor(w, *color);

  AllowNone = False;
  return result;
}

/*------ Hv_ModifyPattern -------*/

int   Hv_ModifyPattern(Hv_Widget w,
		       short     *pattern,
		       char      *prompt)

{
  int result;

  result = Hv_EditPattern(pattern, prompt);
  if (w != NULL)
    Hv_ChangeLabelPattern(w, *pattern);

  return result;
}

/* -------- Hv_Information ------------ */

void Hv_Information(char *message)

{
  XmString             title, text;
  static Widget        dialog;
  
  if (!dialog) {
    dialog = XmCreateInformationDialog(Hv_toplevel, "dialog", NULL, 0);
    title = XmStringCreateSimple("information");
    
    XtVaSetValues(dialog,
		  XmNdialogTitle,       title,
		  XmNdialogStyle,       XmDIALOG_FULL_APPLICATION_MODAL,
		  NULL);

/* I do not want a help or cancel  button */

    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
    XmStringFree(title);
  }

/* the dialog has been created */

  text = XmStringCreateSimple(message);
  XtVaSetValues(dialog, XmNmessageString, text, NULL);
  XmStringFree(text);
  Hv_OpenDialog(dialog, NULL);
}

/* -------- Hv_TimedInformation ------------ */

void Hv_TimedInformation(char *message,
			 int   duration)

{
  static Hv_Widget     dialog, label;
  Hv_Widget            rowcol;
  XtIntervalId         waitid;

  if (!dialog) {
    Hv_VaCreateDialog(&dialog,
		      Hv_TITLE, " Information ",
		      NULL);
    
    rowcol = Hv_DialogColumn(dialog, 10);  /* rowcol to hold all other widgets */
    label = Hv_StandardLabel(rowcol, " ", 0);
  }
  
  if (duration < 1)
    return;
  
  duration = Hv_iMin(duration, 120);

  Hv_ChangeLabel(label, message, Hv_fixed2);
  
  Hv_OpenDialog(dialog, NULL);

  waitid = XtAppAddTimeOut(Hv_appContext, (unsigned long)(1000*duration), 
			   (XtTimerCallbackProc)Hv_WaitInfoTimeOut,  (XtPointer)dialog); 
  
}

/* -------- Hv_Warning ------------ */

void Hv_Warning(char *message)

{
  static Widget        dialog = NULL;
  Hv_Widget            dummy, rowcol;
  static Hv_Widget     messlab;
  char                 *pn;
  char                 *mcopy;


  if (!dialog) {
    Hv_VaCreateDialog(&dialog,
		      Hv_TITLE, "Warning",
		      NULL);

    rowcol = Hv_DialogColumn(dialog, 10);

    pn = (char *)Hv_Malloc(strlen(Hv_programName) + 30);
    strcpy(pn, "     ");
    strcpy(pn, Hv_programName);
    strcat(pn, " warning!     ");

    dummy = Hv_StandardLabel(rowcol, pn, 4);
    Hv_Free(pn);
    messlab = Hv_StandardLabel(rowcol, "                      ", 0);
    dummy = Hv_SimpleDialogSeparator(rowcol);
    dummy = Hv_StandardDoneButton(rowcol, " OK ");
  }
  
  Hv_InitCharStr(&mcopy, message);
  Hv_ReplaceNewLine(mcopy);
  Hv_ChangeLabel(messlab, mcopy, Hv_fixed2);
  Hv_Free(mcopy);
  Hv_DoDialog(dialog, NULL);
}

/*-------- Hv_Wait ------------*/

int Hv_Wait(int interval)

/*  RETURNS:    0 if timed out
    interval is in SECONDS  */

{
  static Widget        dialog = NULL;
  Widget               rowcol;
  char                 comment[40];
  static Widget        abort;
  XtIntervalId         waitid;
  int                  answer;

  if (!dialog) {
    Hv_VaCreateDialog(&dialog,
		      Hv_TITLE, " Wait ",
		      NULL);

    rowcol = Hv_DialogColumn(dialog, 10);  /* rowcol to hold all other widgets */
    waitlabel = Hv_StandardLabel(rowcol, " Paused for       seconds", 2);
    abort = Hv_StandardDoneButton(rowcol, " Abort ");
  }
  
  sprintf(comment, "Paused for %5d seconds", interval);
  Hv_ChangeLabel(waitlabel, comment, Hv_helvetica17);
  
  updateid = XtAppAddTimeOut(Hv_appContext, (unsigned long)1000, 
			     (XtTimerCallbackProc)Hv_UpdateWait, (XtPointer)(&interval)); 
  waitid = XtAppAddTimeOut(Hv_appContext, (unsigned long)(1000*interval), 
			   (XtTimerCallbackProc)Hv_WaitTimeOut,  (XtPointer)dialog); 

/* if aborted, the answer will be Hv_DONE.
   If timed out, the answer will be Hv_OK */

  answer = Hv_DoDialog(dialog, NULL); 

  if (answer == Hv_DONE) {    /* aborted */
    XtRemoveTimeOut(waitid);
    answer = 0;
  }

  if (updateid)
    XtRemoveTimeOut(updateid);

  return answer;
}

/*--------- Hv_UpdateWait -------*/

static void Hv_UpdateWait(XtPointer     xtptr,
			  XtIntervalId  *id)

{
  int   *interval;
  char  comment[40];

  interval = (int *)xtptr;
  (*interval)--;

  if (*interval == 1)
    sprintf(comment, "Paused for 1 more second");
  else 
    sprintf(comment, "Paused for %5d seconds", *interval);
  
  Hv_ChangeLabel(waitlabel, comment, Hv_helvetica17);

  if (*interval > 0)
    updateid = XtAppAddTimeOut(Hv_appContext, (unsigned long)1000, 
			       (XtTimerCallbackProc)Hv_UpdateWait, xtptr); 
  else
    updateid = 0;
}

/*--------- Hv_WaitTimeOut -------*/

static void Hv_WaitTimeOut(XtPointer    xtptr,
			   XtIntervalId *id)

{
  Hv_Widget      dialog = (Hv_Widget)xtptr;
  Hv_DialogData  ddata;

  ddata = Hv_GetDialogData(dialog);
  if (ddata == NULL)
    fprintf(stderr, "Error in Hv_WaitTimeOut\n");
  else
    ddata->answer = Hv_OK;
}
/*--------- Hv_WaitInfoTimeOut -------*/

static void Hv_WaitInfoTimeOut(XtPointer    xtptr,
			   XtIntervalId *id)

{
  Hv_Widget      dialog = (Hv_Widget)xtptr;
  if (dialog != NULL)
    Hv_CloseDialog(dialog);
}


/*----------- Hv_SelfDestructInformation ---------*/

void Hv_SelfDestructInformation(char           **lines,
				int              flags[],
				int              nline,
				int              duration,
				char            *title,
				char            *donestring,
				Hv_FunctionPtr   callback) {
/*
 *   creates a multi line info dialog that goes away by itself
 *
 *       char  **lines         array of strings comprising the message
 *
 *       int     flags[]       array of flags to send to Hv_StandardLabel to
 *                             control font and color, if NULL, all labels will
 *                             be black
 *
 *       int     nline         number of entries in lines and, if not NULL, flags
 *
 *       int     duration   lifetime in seconds of the dialog
 *
 *       char   *title         title of this dialog
 *
 *       char   *donestring    done button with this label added (uses "OK" if NULL)
 *
 *       Hv_FunctionPtr  callback   called if user short circiut exits via the button
 */

    Hv_Widget          dialog, dummy, rowcol, rc;
    int                i, answer;
    Hv_DialogData      ddata;
    Hv_Widget          sdlabel;
    char               text[80];

    if ((nline < 1) || (nline > 100) || (duration < 1))
	return;

    if (title == NULL)
	Hv_VaCreateDialog(&dialog,
			  Hv_TITLE, "Information",
			  Hv_TYPE,   Hv_MODELESS,
			  NULL);
    else
	Hv_VaCreateDialog(&dialog,
			  Hv_TITLE, title,
			  Hv_TYPE,   Hv_MODELESS,
			  NULL);

    rowcol = Hv_DialogColumn(dialog, 0);

/* add the info lines */

    for (i = 0; i < nline; i++) {
	if (flags == NULL)
	    dummy = Hv_StandardLabel(rowcol, lines[i], 0);
	else
	    dummy = Hv_StandardLabel(rowcol, lines[i], flags[i]);

    }

    dummy = Hv_StandardLabel(rowcol, " ", 0);

    sprintf(text, "Dialog will self-destruct in %d seconds", duration);
    sdlabel = Hv_StandardLabel(rowcol, text, 1004);


    dummy = Hv_SimpleDialogSeparator(rowcol);

/* add a done button */

    rc = Hv_DialogRow(rowcol, 10);

    if (donestring != NULL)
	dummy = Hv_StandardDoneButton(rc, donestring);
    else
	dummy = Hv_StandardDoneButton(rc, " OK ");


    Hv_ChangeLabelColors(dummy, Hv_black, Hv_gray12);

/* use the user1 field in the dialog data to store the "time remaining" */

    ddata = Hv_GetDialogData(dialog);
    ddata->ok = dummy;
    ddata->user1 = duration;

    answer = Hv_DoDialogWithTimer(dialog, NULL,
				  Hv_SelfDestructCB,
				  NULL,
				  1000,
				  sdlabel,
				  Hv_helveticaBold11);

    if (answer == Hv_DONE)
	if (callback != NULL)
	    callback();

    Hv_DestroyWidget(dialog);
}


/*----------- Hv_SelfDestructCB ----------*/

static void Hv_SelfDestructCB(Hv_TimerDialogData tdlogdata)
    
{
    Hv_DialogData      ddata;
    Hv_Widget          dialog = tdlogdata->dialog;

    ddata = Hv_GetDialogData(dialog);

    if (ddata->user1 <= 1) { /* done ? */
	
/* make sure Hv_DialogTimerCB does not set another timer */
	
	tdlogdata->waitid = 0;
	Hv_CloseOutCallback(ddata->ok, Hv_OK);
	return;
    }
    
/* update the label */

    ddata->user1 -= 1;
    
    Hv_Free(tdlogdata->updatetext);
    tdlogdata->updatetext = (char *)Hv_Malloc(60);

    if (ddata->user1 == 0)
	sprintf(tdlogdata->updatetext, " ");
    else if (ddata->user1 == 1)
	sprintf(tdlogdata->updatetext, "Dialog will self-destruct in 1 second");
    else if (ddata->user1 > 1)
	sprintf(tdlogdata->updatetext, "Dialog will self-destruct in %d seconds", ddata->user1);
}



/* -------- Hv_question -> yes/no question ------------ */

int  Hv_Question(char *question)

{
  XmString             yes, no, title, text;
  static Widget        dialog = NULL;

  if (!dialog) {
    dialog = XmCreateQuestionDialog(Hv_toplevel, "dialog", NULL, 0);
    title = XmStringCreateSimple("question");
    yes =   XmStringCreateSimple("Yes");
    no =    XmStringCreateSimple("No");

    XtVaSetValues(dialog,
		  XmNdialogTitle,       title,
		  XmNdialogStyle,       XmDIALOG_FULL_APPLICATION_MODAL,
		  XmNokLabelString,     yes,
		  XmNcancelLabelString, no,
		  NULL);

/* I do not want a help button */

    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));

    XtAddCallback(dialog,
		  XmNokCallback,
		  (XtCallbackProc)Hv_Response,
		  NULL);

    XtAddCallback(dialog,
		  XmNcancelCallback,
		  (XtCallbackProc)Hv_Response,
		  NULL);

    XmStringFree(title);
    XmStringFree(yes);
    XmStringFree(no);
  }

/* the dialog has been created */

  text = XmStringCreateSimple(question);
  XtVaSetValues(dialog, XmNmessageString, text, NULL);
  XmStringFree(text);
  
  return Hv_DoDialog(dialog, NULL);
}

/* ------ Hv_Response ----- */

static void Hv_Response(Hv_Widget           w,
			XtPointer           cdata,
			XmAnyCallbackStruct *cbs)

{
  Hv_Widget           dialog;
  Hv_DialogData       ddata;

  dialog = Hv_GetOwnerDialog(w);
  ddata = Hv_GetDialogData(dialog);

  if (ddata == NULL) {
    fprintf(stderr, "Error in Hv_Response\n");
    return;
  }

  switch (cbs->reason) {
  case XmCR_OK:
    ddata->answer = Hv_OK;
    break;
    
  case XmCR_CANCEL:
    ddata->answer = Hv_CANCEL;
    break;
  }
}

/* ----- Hv_OpenDialog -----*/

void Hv_OpenDialog(Hv_Widget dlog,
		   Hv_Widget def_btn)

{
  int            i;
  Hv_DialogData  ddata;

  ddata = Hv_GetDialogData(dlog);

/* stop any old timer, not a likely possibility */

  Hv_StopDialogTimer(dlog);

  XtManageChild(dlog);

/* added to fix lesstif problem and in strict accordance with
   Motif programming manual */

  XtPopup(XtParent(dlog), XtGrabNone);

  ddata->opened = True;
  ddata->answer = Hv_RUNNING;

/* if def_btn is NULL, set it to the dialogs ok button */

  if (def_btn == NULL)
    def_btn = Hv_GetDialogActionButton(dlog, Hv_OKBUTTON);

  if (def_btn != NULL)
    for (i = 0; i < 3; i++)
      XmProcessTraversal(def_btn, XmTRAVERSE_CURRENT);
}


/* ----- Hv_CloseDialog -----*/

void Hv_CloseDialog(Hv_Widget dlog)

/* remove the dialog from the screen by "unmanaging" it */

{
  Hv_DialogData  ddata;

  ddata = Hv_GetDialogData(dlog);
  if (ddata == NULL)
      return;

  Hv_StopDialogTimer(dlog); /*most dialogs have no timer */

  ddata->opened = False;

  ddata->deleteitem = NULL;

/*  XtUnmanageChild(dlog);*/       /* close the dialog box */

/* added to fix lesstif problem and in strict accordance with
   Motif programming manual */

  XtPopdown(XtParent(dlog));

  XSync(Hv_display, False); 
  XFlush(Hv_display);

  Hv_CheckForExposures(Expose);     /* handle resulting exposures immediately*/
 }


/*------ Hv_CloseOutCallback -------*/

void  Hv_CloseOutCallback(Hv_Widget w,
			  int       answer)


{
  Hv_Widget      dialog;
  Hv_DialogData  ddata;
  
  dialog = Hv_GetOwnerDialog(w);
  
  if (dialog == NULL)
    return;
  
  ddata = Hv_GetDialogData(dialog);
  if (ddata != NULL)
    ddata->answer = answer;
}


/* ------ Hv_SetColor --- */

static void Hv_SetColor (Hv_Widget w,
			 int       cnumber,
			 XEvent    *event)
{
  newcolor = (short) cnumber;
  Hv_SetWidgetBackgroundColor(newcolorlabel, newcolor);
}

/* ------ Hv_SetWidgetPattern --- */

static void Hv_SetWidgetPattern (Hv_Widget w,
	   	                 int       pnumber,
		                 XEvent    *event)
{
  newpattern = (short) pnumber;
  Hv_ChangeLabelPattern(newpatternlabel, newpattern);
}


/* ------- Hv_DoDialog --------*/

int  Hv_DoDialog(Hv_Widget dialog,
		 Hv_Widget def_btn)

{
  int   answer;

  Hv_OpenDialog(dialog, def_btn);

  while ((answer = Hv_GetDialogAnswer(dialog)) == Hv_RUNNING)
    XtAppProcessEvent(Hv_appContext, XtIMAll);

/* if the answer is anything but apply, close the dialog */

  if (answer != Hv_APPLY)
    Hv_CloseDialog(dialog);

  return answer;
}

/*------------ Hv_StopDialogTimer ---------*/

extern void            Hv_StopDialogTimer(Hv_Widget dialog) {

  Hv_DialogData  ddata;

  if (dialog == NULL)
      return;

  ddata = Hv_GetDialogData(dialog);

  if ((ddata == NULL) || (ddata->timerdata == NULL) || (ddata->timerdata->waitid == 0))
      return;

  XtRemoveTimeOut(ddata->timerdata->waitid);
  ddata->timerdata->waitid = 0;
}


/* ------- Hv_DoDialogWithTimer --------*/

int  Hv_DoDialogWithTimer(Hv_Widget      dialog,
			  Hv_Widget      def_btn,
			  Hv_FunctionPtr tfunc,
			  Hv_Pointer     ptr,
			  unsigned long  ms_interval,
			  Hv_Widget      updatelabel,
			  short          updatelabelfont)


/* like Hv_DoDialog except every ms_interval (in
   msec) the routine tfunc is called */

/*
 *        def_btn         default button
 *        tfunc           timer callback
 *        ptr             arbitrary data
 *        ms_interval     timer duration in ms
 *        updatelabel     a label that will be updated
 *        updatelabelfont font used for update label
 */


{
  int                  answer;

  Hv_DialogData  ddata;

  ddata = Hv_GetDialogData(dialog);
  if ((ddata == NULL) || (ddata->timerdata == NULL))
      return 0;

/* kill any old timer */

  Hv_StopDialogTimer(dialog);

  Hv_OpenDialog(dialog, def_btn);

  if ((tfunc != NULL) && (ms_interval > 0)) {
      ddata->timerdata->dialog   = dialog;
      ddata->timerdata->callback = tfunc;
      ddata->timerdata->interval = ms_interval;
      ddata->timerdata->userptr  = ptr;
      ddata->timerdata->waitid   = XtAppAddTimeOut(Hv_appContext, ms_interval, 
					  (XtTimerCallbackProc)Hv_DialogTimerCB,
					  (XtPointer)(ddata->timerdata)); 

/* for an optionally updated label */
	  
      ddata->timerdata->font = updatelabelfont;
      ddata->timerdata->label = updatelabel;
      ddata->timerdata->updatetext = NULL;
  }
  else
      ddata->timerdata->waitid = 0;

  while ((answer = Hv_GetDialogAnswer(dialog)) == Hv_RUNNING)
      XtAppProcessEvent(Hv_appContext, XtIMAll);

/* if the answer is anything but apply, close the dialog */

  if (answer != Hv_APPLY)
      Hv_CloseDialog(dialog);
  
  return answer;
}

/*--------- Hv_DialogTimerCB ---------------*/

static void Hv_DialogTimerCB(XtPointer     xtptr,
			     XtIntervalId  *id)

{
  Hv_TimerDialogData   tdlogdata = (Hv_TimerDialogData)xtptr;
  
  if (tdlogdata == NULL)
    return;

/* call the users callback */

  if (tdlogdata->callback != NULL)
    tdlogdata->callback(tdlogdata);

/* now reset the timer (user can set interval to 0 to prevent) */

  if (tdlogdata->interval > 0)
      tdlogdata->waitid = XtAppAddTimeOut(Hv_appContext, tdlogdata->interval, 
					  (XtTimerCallbackProc)Hv_DialogTimerCB,
					  (XtPointer)tdlogdata); 
  else
      tdlogdata->waitid = 0;


/* update a label ? */

  if ((tdlogdata->label != NULL) && (tdlogdata->updatetext != NULL) &&
      (tdlogdata->font >= 0) && (tdlogdata->font < Hv_NUMFONTS))
      Hv_ChangeLabel(tdlogdata->label, tdlogdata->updatetext, tdlogdata->font);
  
}



/*--------- Hv_CheckDefault ------*/

static void    Hv_CheckDefault(int *param,
			       int deflt)

/* changes param to default if it was = Hv_DEFAULT*/

{
  if (*param == Hv_DEFAULT)
    *param = deflt;
}



/*--------------- Hv_CreateManagedSeparator -------*/

static Widget Hv_CreateManagedSeparator(Hv_Widget parent)

{
  Widget   w;

  w = XtVaCreateManagedWidget(" ", 
			      xmSeparatorGadgetClass,  parent,
			      NULL);

  return w;
}


/*--------------- Hv_CreateManagedScale -------*/

static Widget Hv_CreateManagedScale(Hv_Widget      parent,
				    int            orientation,
				    int            minval,
				    int            maxval,
				    int            startval,
				    XtCallbackProc callback)

{
  Widget   w;

/* default values:

   orientation:    vertical
   minval          0
   maxval          100
   startval        0
*/

/* consider the possibility that Hv_DEFAULT was used */

  Hv_CheckDefault(&minval,      0); 
  Hv_CheckDefault(&startval,    0); 
  Hv_CheckDefault(&maxval,      100); 
  Hv_CheckDefault(&orientation, Hv_VERTICAL); 

  w =  XtVaCreateManagedWidget("rowcolumn",
			       xmScaleWidgetClass,      parent,
			       XmNorientation,          (unsigned char)orientation,
			       XmNminimum,              minval,
			       XmNmaximum,              maxval,
			       XmNvalue,                startval,
			       XmNsensitive,            True,
			       XmNshowValue,            True,
			       NULL);
  

  if (callback != NULL)
      XtAddCallback(w,
		    XmNvalueChangedCallback,
		    (XtCallbackProc)callback,
		    NULL);

  return w;
}

/*--------------- Hv_CreateManagedRowCol -------*/

static Widget Hv_CreateManagedRowCol(Hv_Widget parent,
				     short     spacing,
				     short     orientation,
				     short     packing,
				     short     numcolumn,
				     short     alignment)

{

/* x, y give upper left corner relative to parent */

/* default values:

   orientation:    vertical
   spacing         0
   packing         tight (minimum size)
   alignment       beginning   (relevant for labels)
   numcolumn       1  (= #cols for vert orient, # rows for horiz)
*/

/* consider the possibility that Hv_DEFAULT was used */

  return XtVaCreateManagedWidget("rowcolumn",
				 xmRowColumnWidgetClass, parent,
				 XmNorientation,         orientation,
				 XmNspacing,             spacing,
				 XmNpacking,             packing,
				 XmNnumColumns,          numcolumn,
				 XmNentryAlignment,      alignment,
				 XmNisHomogeneous,       False,
				 NULL);
}


/*--------- Hv_CreateManagedColorSelector -------*/

static   Widget   Hv_CreateManagedColorSelector(Hv_Widget parent,
						short     dx,
						short     dy) 

/* add the color selectors */  

{
  Widget   colorrc, dummy;
  int      i;
  int      nr;


  nr = Hv_numColors / 16;

  colorrc = Hv_VaCreateDialogItem(parent,
				  Hv_TYPE,        Hv_ROWCOLUMNDIALOGITEM,
				  Hv_PACKING,     Hv_PAD,
				  Hv_NUMROWS,     nr,
				  Hv_SPACING,     0,
				  NULL);

  for(i = 0;  i < Hv_numColors; i++) {
    dummy = XmCreateDrawingArea(colorrc, " ", NULL, 0);
    
    XtVaSetValues(dummy,
		  XmNwidth,          (Dimension)dx,
		  XmNheight,         (Dimension)dy,
		  XmNbackground,     (Pixel)(Hv_colors[i]),
		  NULL);
    
    XtAddEventHandler(dummy, ButtonPressMask, FALSE,
		      (XtEventHandler)Hv_SetColor, (XtPointer)i);  
    XtManageChild(dummy);
  }
  
  
  return colorrc;
}


/*---------------- Hv_CreateDialog ----------------- */

static Widget Hv_CreateDialog(char         *title,
			      unsigned char dstyle,
			      int           ignoreclose)

/* if ignoreclose != 0, set attribute so that the
   close from the motif window menu is ignored */

{
  Widget          w;
  char           *tstr;
  String          es = "  ";
  Arg		  args[10];
  int		  n = 0;


  XtSetArg(args[n], XmNmarginHeight,      4);	     n++;
  XtSetArg(args[n], XmNmarginWidth,       6);	     n++;
  XtSetArg(args[n], XmNdialogStyle,       dstyle);   n++;
  XtSetArg(args[n], XmNnoResize,          True);     n++;
  XtSetArg(args[n], XmNautoUnmanage,      False);    n++;

/* don't want the Motif close to work */

  if (ignoreclose != 0) {
    XtSetArg(args[n], XmNdeleteResponse, XmDO_NOTHING);
    n++;
  }


  w = XmCreateBulletinBoardDialog(Hv_toplevel, es,
				  (ArgList)args, (Cardinal)n);

/*  XtVaSetValues(w,
		XmNmarginHeight,      4,
		XmNmarginWidth,       6,
		XmNdialogStyle,       dstyle,
		XmNnoResize,          True,
		XmNautoUnmanage,      False,
		XmNcolormap,          Hv_colorMap,
		NULL); */
  
  XtVaSetValues(w, XmNcolormap, Hv_colorMap, NULL);
  
  if (title != NULL) {
    Hv_InitCharStr(&tstr, title);
    XtVaSetValues(XtParent(w), XtNtitle, tstr, NULL);
    Hv_Free(tstr);
  }

  return w;
}


/*-------- Hv_GetDialogData ------*/

Hv_DialogData   Hv_GetDialogData(Hv_Widget dialog)

{
  Hv_DialogData     temp;

/* if it is not really a dialog, report an error */

  if (dialog != Hv_GetOwnerDialog(dialog)) {
    fprintf(stderr, "Error in Hv_GetDialogData\n");
    return NULL;
  }

  for (temp = Hv_dialogList; temp != NULL; temp = temp->next) {
    if (temp->dialog == dialog)
      return temp;
  }

/* if haven't found it, then add it */

  return Hv_AddDialogToList(dialog);
}

/*-------- Hv_AddDialogToList -------*/

static Hv_DialogData Hv_AddDialogToList(Hv_Widget dialog)


{
  Hv_DialogData    temp;

  if (Hv_dialogList == NULL) {
    Hv_dialogList = Hv_MallocDialogData(dialog);
    return Hv_dialogList;
  }

  for (temp = Hv_dialogList; temp->next != NULL; temp = temp->next)
    ;
  temp->next = Hv_MallocDialogData(dialog);
  return temp->next;
}

/*---------------- Hv_CreateModalDialog ----------------- */

static Widget Hv_CreateModalDialog(char *title,
				   int  ignoreclose)

/* creates a bulletin board dlog. Any arguments passed as NULL will not be used,
i.e. the default values will be used */

{
  return Hv_CreateDialog(title,
			 XmDIALOG_FULL_APPLICATION_MODAL,
			 ignoreclose);
}


/*---------------- Hv_CreateModelessDialog ----------------- */

static Widget Hv_CreateModelessDialog(char *title,
				      int   ignoreclose)

/* creates a bulletin board dlog. Any arguments passed as NULL will not be used,
i.e. the default values will be used */

{
  return Hv_CreateDialog(title,
			 XmDIALOG_MODELESS,
			 ignoreclose);
}

/*-------------- Hv_CreateManagedCloseOut --------*/

static Widget  Hv_CreateManagedCloseOut(Hv_Widget parent,
					short     spacing, 
					short     orientation,
					short     font, 
					char      *donestr,
					int       whichones)


{
  Widget        rc, dummy;
  Boolean       ok, cancel, apply, del, done;
  Hv_Widget     dialog;
  Hv_DialogData ddata;

/* get the dialog owner (might be the parent) */

  dialog = Hv_GetOwnerDialog(parent);

  if ((dialog == NULL) || ((ddata = Hv_GetDialogData(dialog)) == NULL)) {
    fprintf(stderr, "Error in Hv_CreateManagedCloseOut\n");
    return NULL;
  }

  ok     = Hv_CheckBit(whichones, Hv_OKBUTTON);
  cancel = Hv_CheckBit(whichones, Hv_CANCELBUTTON);
  apply  = Hv_CheckBit(whichones, Hv_APPLYBUTTON);
  del    = Hv_CheckBit(whichones, Hv_DELETEBUTTON);
  done   = (Hv_CheckBit(whichones, Hv_DONEBUTTON) && (donestr != NULL));
  
  rc = Hv_VaCreateDialogItem(parent,
			     Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
			     Hv_ORIENTATION,  orientation,
			     Hv_SPACING,      spacing,
			     NULL);

  if (ok)
    ddata->ok = Hv_VaCreateDialogItem(rc,
				  Hv_TYPE,     Hv_BUTTONDIALOGITEM,
				  Hv_LABEL,    (char *)"   OK   ",
				  Hv_FONT,     font,
				  Hv_CALLBACK, Hv_CloseOutCallback,
				  Hv_DATA,     (void *)Hv_OK,
				  NULL);

  if (cancel) 
    ddata->cancel = Hv_VaCreateDialogItem(rc,
				  Hv_TYPE,     Hv_BUTTONDIALOGITEM,
				  Hv_LABEL,    (char *)" Cancel ",
				  Hv_FONT,     font,
				  Hv_CALLBACK, Hv_CloseOutCallback,
				  Hv_DATA,     (void *)Hv_CANCEL,
				  NULL);

  if (apply)
    ddata->apply = Hv_VaCreateDialogItem(rc,
				  Hv_TYPE,     Hv_BUTTONDIALOGITEM,
				  Hv_LABEL,    (char *)" Apply ",
				  Hv_FONT,     font,
				  Hv_CALLBACK, Hv_CloseOutCallback,
				  Hv_DATA,     (void *)Hv_APPLY,
				  NULL);

  if (done)
    dummy = Hv_VaCreateDialogItem(rc,
				  Hv_TYPE,     Hv_BUTTONDIALOGITEM,
				  Hv_LABEL,    donestr,
				  Hv_FONT,     font,
				  Hv_CALLBACK, Hv_CloseOutCallback,
				  Hv_DATA,     (void *)Hv_DONE,
				  NULL);

  if (del)
    ddata->del = Hv_VaCreateDialogItem(rc,
				       Hv_TYPE,     Hv_BUTTONDIALOGITEM,
				       Hv_LABEL,    (char *)" Delete",
				       Hv_FONT,     font,
				       NULL);
  return rc;
}

/* ---------- Hv_CreateManagedLabel -----*/

static Widget  Hv_CreateManagedLabel(Hv_Widget parent,
				     char      *labelstr,
				     short     font,
				     short     fg,
				     short     bg,
				     short     bw,
				     short     bc)

{
  XmString   label;
  Widget     w;


  label = Hv_CreateMotifString(labelstr, font);

  if ((bw < 0 ) || (bc < 0))
    w = XtVaCreateManagedWidget("label", 
				xmLabelWidgetClass,    parent,
				XmNlabelString,        label,
				XmNhighlightThickness, 0,
				NULL);
  else
    w = XtVaCreateManagedWidget("label", 
				xmLabelWidgetClass,    parent,
				XmNlabelString,        label,
				XmNhighlightThickness, 0,
				XmNborderWidth,        bw,
				XmNborderColor,        Hv_colors[bc],
				NULL);

  if (font != Hv_DEFAULT)
    XtVaSetValues(w, XmNfontList, Hv_fontList, NULL);
  
  XmStringFree(label);

  Hv_ChangeLabelColors(w, fg, bg);
  return w;
}

/* ---------- Hv_CreateManagedPushButton -----*/

Hv_Widget  Hv_CreateManagedPushButton(Hv_Widget      parent,
				      char           *labelstr,
				      short          font,
				      XtCallbackProc callback,
				      XtPointer      data)
     
{
  XmString   label;
  Widget     w;

  label = Hv_CreateMotifString(labelstr, font);

  w = XtVaCreateManagedWidget(" ", 
			      xmPushButtonGadgetClass,  parent,
			      XmNlabelString,           label,
			      XmNhighlightThickness,    0,
			      NULL);
  
  if (font != Hv_DEFAULT)
    XtVaSetValues(w, XmNfontList, Hv_fontList, NULL);

  if (callback != NULL)
    XtAddCallback(w,
		  XmNactivateCallback,
		  (XtCallbackProc)callback,
		  data);

  XmStringFree(label);
  return w;
}


/*--------- Hv_CreateManagedList --------*/

static Widget Hv_CreateManagedList(Hv_Widget      parent,
				   short          numitems,
				   short          numvisitems,
				   char           policy, 
				   char           resizepolicy,
				   char           **items,
				   XtCallbackProc callback)


{
  Widget      w;
  XmString   *xmstitems = NULL;
  int         i;
  Arg         args[2];
  
  if ((numitems > 0) && (items == NULL))
    return NULL;

  if (numitems > 0)	
    xmstitems = (XmString *)Hv_Malloc(numitems*sizeof(XmString));
    
  for (i = 0; i < numitems; i++)
    xmstitems[i] = XmStringCreateSimple(items[i]);

  XtSetArg(args[0], XmNlistSizePolicy, (unsigned char)resizepolicy);
  w = XmCreateScrolledList(parent, " ", args, 1);
    
  XtVaSetValues(w,
		XmNitemCount,               numitems,
		XmNvisibleItemCount,        numvisitems,
		XmNfontList,                Hv_fontList,
		XmNhighlightThickness,      0,
		XmNselectionPolicy,         (unsigned char)policy,
		XmNitems,                   xmstitems,
		NULL);

  if (policy == Hv_MULTIPLELISTSELECT) {
    if (callback != NULL)
      XtAddCallback(w,
		    XmNmultipleSelectionCallback,
		    (XtCallbackProc)callback,
		    NULL);
    else
      XtAddCallback(w,
		    XmNmultipleSelectionCallback,
		    (XtCallbackProc)Hv_NullCallback,
		    NULL);
  }
  else {
    if (callback != NULL)
      XtAddCallback(w,
		    XmNsingleSelectionCallback,
		    (XtCallbackProc)callback,
		    NULL);
    else
      XtAddCallback(w,
		    XmNsingleSelectionCallback,
		    (XtCallbackProc)Hv_NullCallback,
		    NULL);
  }
   
  XtManageChild (w);
  return  w;
}


/* ---------- Hv_CreateManagedToggleButton -----*/

static Widget  Hv_CreateManagedToggleButton(Hv_Widget      parent,
					    char           *labelstr,
					    short          font,
					    XtCallbackProc callback,
					    void          *data)
     
{
  XmString   label;
  Widget     w;

  label = Hv_CreateMotifString(labelstr, font);

  w = XtVaCreateManagedWidget(" ", 
			      xmToggleButtonGadgetClass,  parent,
			      XmNlabelString,             label,
			      XmNhighlightThickness,      0,
			      NULL);
  
  if (font != Hv_DEFAULT)
    XtVaSetValues(w, XmNfontList, Hv_fontList, NULL);
  
  XmStringFree(label);

  if (callback)
    XtAddCallback(w,
		  XmNvalueChangedCallback,
		  (XtCallbackProc)callback,
		  (XtPointer)data);

  return w;
}


/* ---------- Hv_CreateManagedRadioButton -----*/

static Widget  Hv_CreateManagedRadioButton(Hv_Widget        parent,
					   char            *labelstr,
					   short            font,
					   Hv_RadioHeadPtr *radiohead,
					   void            *mydata,
					   XtCallbackProc  vccallback)
     

{
  XmString          label;
  Widget            w;
  Hv_RadioListPtr   temp;
  short             count;

  if (*radiohead == NULL) {
    *radiohead = (Hv_RadioHeadPtr)Hv_Malloc(sizeof(Hv_RadioHead));
    (*radiohead)->head = NULL;
    (*radiohead)->activewidget = NULL;
    (*radiohead)->activepos = 0;
  }

  label = Hv_CreateMotifString(labelstr, font);

  w = XtVaCreateManagedWidget(" ", 
			      xmToggleButtonGadgetClass,  parent,
			      XmNlabelString,             label,
			      XmNhighlightThickness,      0,
			      XmNindicatorType,           XmONE_OF_MANY,
			      XmNvisibleWhenOff,          True,
			      NULL);
  
  if (vccallback != NULL)
    XtAddCallback(w,
		  XmNvalueChangedCallback,
		  (XtCallbackProc)vccallback,
		  (XtPointer)parent);
  else
    XtAddCallback(w,
		  XmNvalueChangedCallback,
		  (XtCallbackProc)Hv_StandardRadioButtonCallback,
		  (XtPointer)(*radiohead));

  if (font != Hv_DEFAULT)
    XtVaSetValues(w, XmNfontList, Hv_fontList, NULL);
  
  XmStringFree(label);


/* now add to list */

  count = 1;

  if ((*radiohead)->head == NULL) {
    (*radiohead)->head = (Hv_RadioListPtr)Hv_Malloc(sizeof(Hv_RadioList));
    temp = (*radiohead)->head;
  }
  
  else {
    count = 2;
    for (temp = (*radiohead)->head; temp->next != NULL; temp = temp->next)
      count++;
    temp->next = (Hv_RadioListPtr)Hv_Malloc(sizeof(Hv_RadioList));
    temp = temp->next;
  }
  
  temp->radio = w;
  temp->next = NULL;
  temp->data = mydata;
  temp->pos = count;      /* not a C index */
  return w;
}

/*---------- Hv_StandardRadionButtonCallback -------------*/

static void Hv_StandardRadioButtonCallback(Hv_Widget w,
					   XtPointer client_data)

{
  Hv_RadioHeadPtr  radiohead = (Hv_RadioHeadPtr)(client_data);
  Hv_SetActiveRadioButton(w, radiohead);
}


/* ---------- Hv_CreateManagedTextField -----*/

static Widget  Hv_CreateManagedTextField(Hv_Widget      parent,
					 char          *defaultstr,
					 short          cols,
					 short          font,
					 Boolean        editable,
					 XtCallbackProc vchanged)
{
  Widget     w;
  Dimension  mh = 2;
  Dimension  mw = 2;
  char       *value = NULL;

  static char  transtable[] =
    "<Key>osfBackSpace: delete-previous-character()";
  
  static char  trans2table[] =
    "<Key>osfDelete: delete-previous-character()";

/* Insidious bug fixed by Jack Scheible of Sparta Inc. */  

  w = XtVaCreateManagedWidget("textedit", 
			      xmTextFieldWidgetClass,   parent,
			      XmNcolumns,               cols,
			      XmNeditable,              editable,
			      XmNmarginHeight,          mh,
			      XmNmarginWidth,           mw,
			      XmNcursorPositionVisible, editable,
			      NULL);


  if (defaultstr != NULL) {
    Hv_InitCharStr(&value, defaultstr);
    XtVaSetValues(w, XmNvalue, (String)value, NULL);
  }


/******************************************************
  The next two translation overrides were added 5/26/96
  to try to fix the wrongway delete on all platforms
*******************************************************/

  XtOverrideTranslations(w, XtParseTranslationTable(transtable));
  XtOverrideTranslations(w, XtParseTranslationTable(trans2table));


  if (font != Hv_DEFAULT)
    XtVaSetValues(w, XmNfontList, Hv_fontList, NULL);

  if (vchanged != NULL)
    XtAddCallback(w,
		  XmNvalueChangedCallback,
		  (XtCallbackProc)vchanged,
		  NULL);

  Hv_Free(value);
  return w;
}

/* ---------- Hv_CreateManagedLabelTextField -----*/

static Widget  Hv_CreateManagedLabelTextField(Hv_Widget       parent,
					      char           *labelstr,
					      short           font,
					      short           fg,
					      short           bg,
					      short           bw,
					      short           bc,
					      char           *defaultstr,
					      short           cols,
					      Boolean         editable,
					      XtCallbackProc  vchanged)
    

/* creates a label and a text field with same parent */

{
  Widget   w;

/* first the label */

  Hv_lastLabel = Hv_VaCreateDialogItem(parent,
				Hv_TYPE,        Hv_LABELDIALOGITEM,
				Hv_LABEL,       labelstr,
				Hv_FONT,        font,
				Hv_BORDERWIDTH, bw,
				Hv_BORDERCOLOR, bc,
				NULL);

  Hv_ChangeLabelColors(Hv_lastLabel, fg, bg);
  
/* now the editable text */

  w  = (Widget)Hv_VaCreateDialogItem(parent,
				     Hv_TYPE,        Hv_TEXTDIALOGITEM,
				     Hv_DEFAULTTEXT, defaultstr,
				     Hv_NUMCOLUMNS,  cols,
				     Hv_EDITABLE,    editable,
				     Hv_FONT,        font,
				     NULL);


  if (vchanged != NULL)
    XtAddCallback(w,
		  XmNvalueChangedCallback,
		  (XtCallbackProc)vchanged,
		  NULL);

  return w;
}

/*--------------- Hv_CreateManagedScrolledText -------*/

static Widget Hv_CreateManagedScrolledText(Hv_Widget parent,
					   short     nrow,
					   short     ncol,
					   short     bg,
					   short     fg)
     
{
  Widget  w;

  w = XmCreateScrolledText(parent, "scrolled", NULL, 0);
  XtVaSetValues(w,
		XmNrows,                   nrow,
		XmNcolumns,                ncol,
		XmNeditable,               False,
		XmNeditMode,               XmMULTI_LINE_EDIT,
		XmNwordWrap,               True,
		XmNscrollHorizontal,       False,
		XmNblinkRate,              0,
		XmNautoShowCursorPosition, True,
		XmNcursorPositionVisible,  False,
		XmNfontList,               Hv_fontList, NULL);


  if (bg != Hv_DEFAULT)
    Hv_SetWidgetBackgroundColor(w, bg);

  if (fg != Hv_DEFAULT)
    Hv_SetWidgetForegroundColor(w, fg);

  XtManageChild(w);
  XtManageChild(XtParent(w));  /* the window that will hold the text*/

  return  w;
}


/*-------- Hv_NullCallback ------*/

static void Hv_NullCallback(Hv_Widget w)

{
}

/*-------- Hv_MallocDialogData -----*/

static Hv_DialogData Hv_MallocDialogData(Hv_Widget dialog)

{
  Hv_DialogData     ddata;

  ddata = (Hv_DialogData)Hv_Malloc(sizeof(Hv_DialogDataRec));

  ddata->next = NULL;
  ddata->dialog = dialog;
  ddata->answer = 0;
  ddata->user1 = 0;
  ddata->user2 = 0;
  ddata->data = NULL;
  ddata->opened = False;

  ddata->timerdata = (Hv_TimerDialogData)Hv_Malloc(sizeof(Hv_TimerDialogDataRec));
  ddata->timerdata->waitid = 0;
  ddata->timerdata->userptr = NULL;

  ddata->ok = NULL;
  ddata->cancel = NULL;
  ddata->apply = NULL;
  ddata->del = NULL;

  ddata->deleteitem = NULL;

  return ddata;
}

/*------ Hv_EditColor -------*/

static int   Hv_EditColor(short *color,
			  char  *prompt)

/* returns 0 if color was not changed */
     
{
  static Widget        dialog = NULL;
  static Widget        mynewcolor, myoldcolor, ncolor;
  
  Hv_Widget            rowcol, rc, dummy;

  short                dx  = 14;  /*width  of color button*/
  short                dy  = 14;  /*height of color button*/
  XmString             title;
  int                  answer;

  if (!dialog) {
    Hv_VaCreateDialog(&dialog, 
		      Hv_TITLE, " ", 
		      NULL);

    rowcol = Hv_DialogColumn(dialog, 10);
    rc = Hv_DialogTable(rowcol, 2, 10);
    mynewcolor = Hv_SimpleColorLabel(rc, " new color ", NULL);
    myoldcolor = Hv_SimpleColorLabel(rc, " old color ", NULL);


/* the color selectors */

    dummy = Hv_SimpleDialogSeparator(rowcol);

    dummy = Hv_VaCreateDialogItem(rowcol,
				  Hv_TYPE,     Hv_COLORSELECTORDIALOGITEM,
				  Hv_WIDTH,    dx,
				  Hv_HEIGHT,   dy,
				  NULL);

    rc = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON + Hv_CANCELBUTTON);

/* add no-color button  */

    ncolor = Hv_StandardDoneButton(rc, "No Color");

  }

  newcolorlabel = mynewcolor;
  oldcolorlabel = myoldcolor;

  XtSetSensitive(ncolor, AllowNone);
  
/* the dialog has been created */

  newcolor = *color;
  
  if (prompt) {
    title = XmStringCreateSimple(prompt);
    XtVaSetValues(dialog, XmNdialogTitle, title, NULL);
    XmStringFree(title);
  }
  

/* set old & new color label to reflect current color */

  Hv_SetWidgetBackgroundColor(oldcolorlabel, *color);
  Hv_SetWidgetBackgroundColor(newcolorlabel, *color);

  answer = Hv_DoDialog(dialog, NULL);

/* even if person hit ok, if the color was not changed treat like a cancel */
  
  if (answer == Hv_OK) {
    if (*color == newcolor)
      answer = 0;
    else
      *color = newcolor;
  }
  else if ((answer == Hv_DONE)  && AllowNone)
    *color = -1;

  AllowNone = False;
  return answer;
}


/*------ Hv_EditPattern -------*/

static int   Hv_EditPattern(short *pattern,
			    char  *prompt)

/* returns 0 if pattern was not changed */
     
{
  static Widget        dialog = NULL;
  static Widget        mynewpattern, myoldpattern;
  
  Hv_Widget            rowcol, rc, dummy;

  short                dx  = 16;  /*width  of pattern label */
  short                dy  = 16;  /*height of pattern label */
  XmString             title;
  int                  i, answer;

  if (!dialog) {
    Hv_VaCreateDialog(&dialog,
		      Hv_TITLE, " ",
		      NULL);
    rowcol = Hv_DialogColumn(dialog, 10); 
    rc = Hv_DialogTable(rowcol, 2, 10);
    mynewpattern = Hv_SimplePatternLabel(rc, " new pattern ", NULL);
    myoldpattern = Hv_SimplePatternLabel(rc, " old pattern ", NULL);

/* the pattern selectors */

    dummy = Hv_SimpleDialogSeparator(rowcol);

    rc = Hv_DialogTable(rowcol, 2, 0);

    for (i = 0; i < Hv_NUMPATTERNS; i++) {
      dummy = (Widget)Hv_VaCreateDialogItem(rc,
					    Hv_TYPE,        Hv_LABELDIALOGITEM,
					    Hv_LABEL,       (char *)"  ",
					    Hv_BORDERWIDTH, 1,
					    Hv_BORDERCOLOR, Hv_white,
					    Hv_FONT,        Hv_fixed2,
					    Hv_TEXTCOLOR,   Hv_black,
					    Hv_BACKGROUND,  Hv_white,
					    Hv_WIDTH,       dx,
					    Hv_HEIGHT,      dy,
					    NULL);

      XtVaSetValues(dummy, XmNlabelType, XmPIXMAP, NULL);

      XtAddEventHandler(dummy, ButtonPressMask,
			FALSE,
			(XtEventHandler)Hv_SetWidgetPattern,
			(XtPointer)i);  

      Hv_ChangeLabelPattern(dummy, i);
    }

    rc = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON + Hv_CANCELBUTTON);
  }

  newpatternlabel = mynewpattern;
  oldpatternlabel = myoldpattern;

/* the dialog has been created */

  newpattern = *pattern;
  
  if (prompt) {
    title = XmStringCreateSimple(prompt);
    XtVaSetValues(dialog, XmNdialogTitle, title, NULL);
    XmStringFree(title);
  }
  

/* set old & new pattern label to reflect current pattern     */

  Hv_ChangeLabelPattern(oldpatternlabel, *pattern);
  Hv_ChangeLabelPattern(newpatternlabel, *pattern);

  answer = Hv_DoDialog(dialog, NULL);

/* even if person hit ok, if the pattern was not changed treat like a cancel */
  
  if (answer == Hv_OK) {
    if (*pattern == newpattern)
      answer = 0;
    else {
      *pattern = newpattern;

/* a choice of 0 (black) is reset to -1, which
   is a flag indicating solid fill */

      if (*pattern == 0)
	*pattern = -1;

    }
  }

  return answer;
}







E 1
