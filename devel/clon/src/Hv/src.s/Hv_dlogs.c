/**
 * <EM>Deals with <B>non-gateway</B> dialog routines.</EM>
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

Hv_Widget      Hv_newColorLabel;
short          Hv_newColor;

/* ----- local variables ------*/


static   Hv_Widget  oldcolorlabel;
static   Boolean  AllowNone = False;

static   Hv_Widget     waitlabel;
Hv_IntervalId        updateid;

Boolean   Hv_altCO = False;



/*------ local prototypes ------*/

static Hv_Widget  Hv_LabelAndTextField(Hv_Widget         parent,
				       Hv_AttributeArray attributes);

static void       Hv_SelfDestructCB(Hv_TimerDialogData tdlogdata);

static void       Hv_DialogTimerCB(Hv_Pointer     hvptr,
				   Hv_IntervalId  *id);

static Hv_DialogData  Hv_AddDialogToList(Hv_Widget);

static Hv_DialogData  Hv_MallocDialogData(Hv_Widget);


static void     Hv_WaitInfoTimeOut(Hv_Pointer    hvptr,
				   Hv_IntervalId *id);

static void     Hv_WaitTimeOut(Hv_Pointer,
			       Hv_IntervalId *);

static void     Hv_UpdateWait(Hv_Pointer,
			       Hv_IntervalId *);

static int      Hv_EditColor(short *,
			     char  *);


/**
 * Hv_VaCreateDialog
 * @purpose Create a dialog. For both modeless and modal dialogs. (Modal
 * dialogs block until they are closed.)
 * @param   dialog   Upon return, this will point to the dialog
 * @param   ...      Variable length list of attribute-value pairs
 * <p>
 * @table
 * |R Attribute         | Type     | Default    | Comment |r
 * |R Hv_TITLE          | char *   | NULL       | Title on dialog frame |r
 * |R Hv_TYPE           | short    | Hv_MODAL   | Other option is Hv_MODELESS |r
 * |R Hv_WMIGNORGECLOSE | Boolean  | False      | If True, ignore the windows close box |r
 * @endtable
 */



void Hv_VaCreateDialog(Hv_Widget *dialog,
		       ...) {

    va_list           ap;
    Hv_AttributeArray attributes;

    va_start(ap, dialog);

/* Get the variable arguments */

    Hv_GetAttributes(ap, attributes, NULL, NULL, NULL, NULL, NULL);

/* two general types, modeless and modal */

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



/**
 * Hv_VaCreateDialogItem
 * @purpose Creates Items that go into a dialog.
 * @param   parent  This with be either the dialog or a container widget.
 * @return  The created dialog item (widget).
 * <p>
 * The table below contains all attributes for dialog items. Not all are applicable for
 * any given item; see the documentation for a specific type of dialog item to
 * see what attributes are relevant.
 * @table
 * |R Attribute           | Type     | Default    | Comment |r
 * |R Hv_ALIGNMENT        |   |   |  |r
 * |R Hv_BACKGROUND       | short  |  Calculated | Background color |r
 * |R Hv_BORDERCOLOR      |   |   |  |r
 * |R Hv_BORDERWIDTH      |   |   |  |r
 * |R Hv_CALLBACK         |   |   |  |r
 * |R Hv_COLOR            |   |   |  |r
 * |R Hv_CURRENTVALUE     |   |   |  |r
 * IR Hv_DATA             |  Hv_Pointer | NULL  | User supplied data |r
 * IR Hv_DEFAULTTEXT      |   |   |  |r
 * IR Hv_DIALOGCLOSEOUT   |   |   |  |r
 * |R Hv_EDITABLE         |   |   |  |r
 * |R Hv_FONT             |  short | Hv_fixed2  | Text font |r
 * |R Hv_HEIGHT           | short  | Calculated  | Pixel height (applies to ALL item types) |r
 * |R Hv_LABEL            |   |   |  |r
 * |R Hv_LIST             |   |   |  |r
 * |R Hv_LEFT             |   |   |  |r
 * |R Hv_LISTSELECTION    |   |   |  |r
 * |R Hv_LISTRESIZEPOLICY |   |   |  |r
 * |R Hv_MAXVALUE         |   |   |  |r
 * |R Hv_MASK             |   |   |  |r
 * |R Hv_MINVALUE         |   |   |  |r
 * |R Hv_NOWON            |   |   |  |r
 * |R Hv_NUMCOLUMNS       |   |   |  |r
 * |R Hv_NUMITEMS         |   |   |  |r
 * |R Hv_NUMROWS          |   |   |  |r
 * |R Hv_NUMVISITEMS      |   |   |  |r
 * |R Hv_ORIENTATION      |   |   |  |r
 * |R Hv_PACKING          |   |   |  |r
 * |R Hv_RADIOLIST        |   |   |  |r
 * |R Hv_SPACING          | short    | 0          | Pixel spacing |r
 * |R Hv_TEXTCOLOR        | short    | black      | Text color |r
 * |R Hv_TOP              |   | Calculated  | Top (applies to ALL item types) |r
 * |R Hv_TYPE             | short    | button     | Type of dialog item (see table below)
 * |R Hv_VALUECHANGED     |   |   |  |r
 * |R Hv_WORDWRAP         |   |   |  |r
 * |R Hv_WIDTH            | short  | Calculated | Pixel width (applies to ALL item types) |r
 * @endtable
 * <p>
 * The type should be one of the following:
 * <p>
 * @table
 * |R Type                       | Comment |r
 * |R Hv_ACTIONAREADIALOGITEM    | An upgrade of Hv_CLOSEOUTDIALOGITEM |r
 * |R Hv_BUTTONDIALOGITEM        | Normal button |r
 * |R Hv_CLOSEOUTDIALOGITEM      | A button that causes the dialog to close |r
 * |R Hv_COLORSELECTORDIALOGITEM | For selecting a color |r
 * |R Hv_FILESELECTORDIALOGITEM  | A file selection item |r
 * |R Hv_LABELDIALOGITEM         | A simple label |r
 * |R Hv_LISTDIALOGITEM          | Standard list |r
 * |R Hv_RADIODIALOGITEM         | Radio buttons |r
 * |R Hv_ROWCOLUMNDIALOGITEM     | Container for other items |r
 * |R Hv_SCALEDIALOGITEM         | A scale |r
 * |R Hv_SCROLLEDTEXTDIALOGITEM  | A area of scrollable text |r
 * |R Hv_SEPARATORDIALOGITEM     | Inert separator |r
 * |R Hv_TEXTDIALOGITEM          | A Text item |r
 * |R Hv_TOGGLEDIALOGITEM        | A toggle button |r
 * @endtable
 */


Hv_Widget  Hv_VaCreateDialogItem(Hv_Widget parent,
				 ...) {

  va_list           ap;
  Hv_AttributeArray attributes;
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

  case Hv_FILESELECTORDIALOGITEM:
      ditem = Hv_FileSelectorDialogItem(parent, attributes);
      break;
      
  case Hv_COLORSELECTORDIALOGITEM:
      ditem = Hv_ColorSelectorDialogItem(parent, attributes);
      break;
    
  case Hv_LABELDIALOGITEM:
      ditem = Hv_LabelDialogItem(parent, attributes);
      break;
    
  case Hv_SEPARATORDIALOGITEM:
      ditem = Hv_SeparatorDialogItem(parent, attributes);
      break;

   case Hv_SCROLLBARDIALOGITEM:
      ditem = Hv_ScrollBarDialogItem(parent, attributes);
      break;
   
  case Hv_LISTDIALOGITEM:
      ditem = Hv_ListDialogItem(parent, attributes);
      break;
    
  case Hv_ROWCOLUMNDIALOGITEM:
      ditem = Hv_RowColDialogItem(parent, attributes);
      break;
    
  case Hv_SCROLLEDTEXTDIALOGITEM:
      ditem = Hv_ScrolledTextDialogItem(parent, attributes);
      break;
    
  case Hv_SCALEDIALOGITEM:
      ditem = Hv_ScaleDialogItem(parent, attributes);
      break;
    
  case Hv_RADIODIALOGITEM:  /* if numopt = 0, one ad a time, otherwise many at once */
    
      if (numopt == 0)
	  ditem = Hv_RadioDialogItem(parent, attributes);
      else {

/* must be the "first" call so radiolist should be NULL */

	  radiohead = (Hv_RadioHeadPtr *)(attributes[Hv_RADIOLIST].v);
	  *radiohead = NULL;

	  for (i = 0; i < numopt; i++) {
	      attributes[Hv_LABEL].v = optlist[i];
	      ditem = Hv_RadioDialogItem(parent, attributes);
	  }
	  Hv_PosSetActiveRadioButton(attributes[Hv_NOWON].s, *radiohead);
      }
      break;
    
  case Hv_TOGGLEDIALOGITEM:
      ditem = Hv_ToggleDialogItem(parent, attributes);
      break;
    
  case Hv_TEXTDIALOGITEM:
      if (attributes[Hv_LABEL].v == NULL)
	  ditem = Hv_TextDialogItem(parent, attributes);
      else
	  ditem = Hv_LabelAndTextField(parent, attributes);
      break;
    
  case Hv_CLOSEOUTDIALOGITEM:
      ditem = Hv_CloseOutDialogItem(parent, attributes);
      break;

  case Hv_ACTIONAREADIALOGITEM:
      ditem = Hv_ActionAreaDialogItem(parent, attributes);
      break;

  case Hv_BUTTONDIALOGITEM:
  default:   /* standard push button  */
      ditem = Hv_ButtonDialogItem(parent, attributes);
      break;
  }

  if (ditem != NULL)
    if ((attributes[Hv_LEFT].s > -32700) && (attributes[Hv_TOP].s > -32700))
      Hv_VaSetValues(ditem,
		    Hv_Nx,    attributes[Hv_LEFT].s,
		    Hv_Ny,    attributes[Hv_TOP].s,
		    NULL);


/* if any option strings, delete them */

  for (i = 0; i < numopt; i++)
    Hv_Free(optlist[i]);

  return ditem;
}


/**
 * Hv_ButtonDialogItem
 * @purpose Create a push button for a dialog. 
 * @param parent    The parent widget (either a dialog or a rowcol)
 * @param labelstr  Button label.
 * @param font      Button label font.
 * @param callback  Button callback.
 * @param data      Pointer to user data.
 * @return The button dialog item
 * @deprecated   Use Hv_VaCreateDialogItem. Only Ray Prouty uses this.
 */

Hv_Widget       Hv_CreateManagedPushButton(Hv_Widget       parent,
					   char           *labelstr,
					   short           font,
					   Hv_CallbackProc callback,
					   Hv_Pointer      data) {

  return Hv_VaCreateDialogItem(parent,
			       Hv_TYPE,     Hv_BUTTONDIALOGITEM,
			       Hv_LABEL,    labelstr,
			       Hv_CALLBACK, callback,
			       Hv_DATA,     data,
			       NULL);
}


/**
 * Hv_ModifiyColor 
 * @purpose A convenience routine for modifying the background
 * color of a widget through a standard color selector dialog.
 * @param w            The widget being modified (can be NULL)
 * @param color        The color (index) variable
 * @param prompt       A text prompt
 * @param allowNoColor If true, the user will be able to select "no color"
 * @return  0 if the color was NOT changed
 */


int   Hv_ModifyColor(Hv_Widget w,
		     short    *color,
		     char     *prompt,
		     Boolean  allowNoColor) {
    int result;
    
    AllowNone = allowNoColor;
    
    result = Hv_EditColor(color, prompt);
    
    if (w != NULL)
	Hv_SetWidgetBackgroundColor(w, *color);
    
    AllowNone = False;
    return result;
}

/**
 * Hv_ModifiyPattern 
 * @purpose A convenience routine for modifying the background
 * pattern of a widget through a standard pattern selector dialog.
 * @param w            The widget being modified (can be NULL)
 * @param pattern      The pattern (index) variable
 * @param prompt       A text prompt
 * @return  0 if the pattern was NOT changed
 */

int   Hv_ModifyPattern(Hv_Widget w,
		       short     *pattern,
		       char      *prompt) {
  int result;

  result = Hv_EditPattern(pattern, prompt);
  if (w != NULL)
    Hv_ChangeLabelPattern(w, *pattern);

  return result;
}




/* -------- Hv_TimedInformation ------------ */

/**
 * Hv_TimedInformation
 * @purpose A simple informational dialog that will go away by itself
 * (self destruct) if left up for a certain duration.
 * @param message  The text of the message.
 * @param duration The time until self destruction in seconds
 */

void Hv_TimedInformation(char *message,
			 int   duration)

{
  static Hv_Widget     dialog, label;
  Hv_Widget            rowcol;
  Hv_IntervalId         waitid;

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

  waitid = Hv_AddTimeOut((unsigned long)(1000*duration), 
			 (Hv_TimerCallbackProc)Hv_WaitInfoTimeOut,
			 (Hv_Pointer)dialog); 
  
}


/*-------- Hv_Wait ------------*/

int Hv_Wait(int interval)

/*  RETURNS:    0 if timed out
    interval is in SECONDS  */

{
  static Hv_Widget        dialog = NULL;
  Hv_Widget               rowcol;
  char                 comment[40];
  static Hv_Widget        abort;
  Hv_IntervalId         waitid;
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
  
  updateid = Hv_AddTimeOut((unsigned long)1000, 
			   (Hv_TimerCallbackProc)Hv_UpdateWait,
			   (Hv_Pointer)(&interval)); 
  
  waitid = Hv_AddTimeOut((unsigned long)(1000*interval), 
			 (Hv_TimerCallbackProc)Hv_WaitTimeOut, 
			 (Hv_Pointer)dialog); 

/* if aborted, the answer will be Hv_DONE.
   If timed out, the answer will be Hv_OK */

  answer = Hv_DoDialog(dialog, NULL); 

  if (answer == Hv_DONE) {    /* aborted */
    Hv_RemoveTimeOut(waitid);
    answer = 0;
  }

  if (updateid)
    Hv_RemoveTimeOut(updateid);

  return answer;
}

/*--------- Hv_UpdateWait -------*/

static void Hv_UpdateWait(Hv_Pointer     hvptr,
			  Hv_IntervalId  *id)

{
  int   *interval;
  char  comment[40];

  interval = (int *)hvptr;
  (*interval)--;

  if (*interval == 1)
    sprintf(comment, "Paused for 1 more second");
  else 
    sprintf(comment, "Paused for %5d seconds", *interval);
  
  Hv_ChangeLabel(waitlabel, comment, Hv_helvetica17);

  if (*interval > 0)
    updateid = Hv_AddTimeOut((unsigned long)1000, 
			     (Hv_TimerCallbackProc)Hv_UpdateWait,
			     hvptr); 
  else
    updateid = 0;
}

/*--------- Hv_WaitTimeOut -------*/

static void Hv_WaitTimeOut(Hv_Pointer    hvptr,
			   Hv_IntervalId *id)

{
  Hv_Widget      dialog = (Hv_Widget)hvptr;
  Hv_DialogData  ddata;

  ddata = Hv_GetDialogData(dialog);
  if (ddata == NULL)
    Hv_Println("Error in Hv_WaitTimeOut");
  else
    ddata->answer = Hv_OK;
}
/*--------- Hv_WaitInfoTimeOut -------*/

static void Hv_WaitInfoTimeOut(Hv_Pointer    hvptr,
			   Hv_IntervalId *id)

{
  Hv_Widget      dialog = (Hv_Widget)hvptr;
  if (dialog != NULL)
    Hv_CloseDialog(dialog);
}


/**
 * Hv_SelfDestructInformation
 * @purpose creates a multi line info dialog that goes away by itself 
 * @param lines array of strings comprising the message
 * @param flags array of flags to send to Hv_StandardLabel to
 * control font and color, if NULL, all labels will be black.
 * @param nline number of entries in lines and, if not NULL, flags
 * @param duration lifetime in seconds of the dialog
 * @param title title of this dialog
 * @param doenstring done button with this label added (uses "OK" if NULL)
 * @param callback called if user short circiut exits via the button 
 */



void Hv_SelfDestructInformation(char           **lines,
				int              flags[],
				int              nline,
				int              duration,
				char            *title,
				char            *donestring,
				Hv_FunctionPtr   callback) {

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

/*  Hv_UnmanageChild(dlog);*/       /* close the dialog box */

/* added to fix lesstif problem and in strict accordance with
   Motif programming manual */

  Hv_PopdownDialog(dlog);

  Hv_Sync(); 
  Hv_Flush();

  Hv_CheckForExposures(Hv_EXPOSE);     /* handle resulting exposures immediately*/

  Hv_allowMag = True;

 }




/*------------ Hv_StopDialogTimer ---------*/

extern void            Hv_StopDialogTimer(Hv_Widget dialog) {

  Hv_DialogData  ddata;

  if (dialog == NULL)
      return;

  ddata = Hv_GetDialogData(dialog);

  if ((ddata == NULL) || (ddata->timerdata == NULL) || (ddata->timerdata->waitid == 0))
      return;

  Hv_RemoveTimeOut(ddata->timerdata->waitid);
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
      ddata->timerdata->waitid   = Hv_AddTimeOut(ms_interval, 
						 (Hv_TimerCallbackProc)Hv_DialogTimerCB,
						 (Hv_Pointer)(ddata->timerdata)); 

/* for an optionally updated label */
	  
      ddata->timerdata->font = updatelabelfont;
      ddata->timerdata->label = updatelabel;
      ddata->timerdata->updatetext = NULL;
  }
  else
      ddata->timerdata->waitid = 0;

  while ((answer = Hv_GetDialogAnswer(dialog)) == Hv_RUNNING)
      Hv_ProcessEvent(Hv_IMAll);

/* if the answer is anything but apply, close the dialog */

  if (answer != Hv_APPLY)
      Hv_CloseDialog(dialog);
  
  return answer;
}

/*--------- Hv_DialogTimerCB ---------------*/

static void Hv_DialogTimerCB(Hv_Pointer     hvptr,
			     Hv_IntervalId  *id)

{
  Hv_TimerDialogData   tdlogdata = (Hv_TimerDialogData)hvptr;
  
  if (tdlogdata == NULL)
    return;

/* call the users callback */

  if (tdlogdata->callback != NULL)
    tdlogdata->callback(tdlogdata);

/* now reset the timer (user can set interval to 0 to prevent) */

  if (tdlogdata->interval > 0)
      tdlogdata->waitid = Hv_AddTimeOut(tdlogdata->interval, 
					(Hv_TimerCallbackProc)Hv_DialogTimerCB,
					(Hv_Pointer)tdlogdata); 
  else
      tdlogdata->waitid = 0;


/* update a label ? */

  if ((tdlogdata->label != NULL) && (tdlogdata->updatetext != NULL) &&
      (tdlogdata->font >= 0) && (tdlogdata->font < Hv_NUMFONTS))
      Hv_ChangeLabel(tdlogdata->label, tdlogdata->updatetext, tdlogdata->font);
  
}



/*-------- Hv_GetDialogData ------*/

Hv_DialogData   Hv_GetDialogData(Hv_Widget dialog)

{
  Hv_DialogData     temp;

/* if it is not really a dialog, report an error */

  if (dialog != Hv_GetOwnerDialog(dialog)) {
    Hv_Println("Error in Hv_GetDialogData");
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


/* ---------- Hv_LabelAndTextField -----*/

static Hv_Widget  Hv_LabelAndTextField(Hv_Widget       parent,
				       Hv_AttributeArray attributes) {

/* creates a label and a text field with same parent */

  Hv_Widget   w;

/* first the label */

  Hv_lastLabel = Hv_LabelDialogItem(parent, attributes);
  
/* now the editable text */

  w = Hv_TextDialogItem(parent, attributes);
  return w;
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
			  char  *prompt) {

/* returns 0 if color was not changed */
     
  static Hv_Widget        dialog = NULL;
  static Hv_Widget        mynewcolor, myoldcolor, ncolor;
  
  Hv_Widget            rowcol, rc, dummy;

  short                dx  = 14;  /*width  of color button*/
  short                dy  = 14;  /*height of color button*/
  Hv_CompoundString             title;
  int                  answer;

  if (!dialog) {
	  Hv_VaCreateDialog(&dialog, Hv_TITLE, "  ", NULL);
	  rowcol   = Hv_DialogColumn(dialog, 8);
	  rc       = Hv_DialogTable(rowcol, 2, 8);
	  mynewcolor = Hv_SimpleColorLabel(rc, " new color ", NULL);
      myoldcolor = Hv_SimpleColorLabel(rc, " old color ", NULL);

/*
//    rowcol = Hv_DialogColumn(dialog, 10);
//   rc = Hv_DialogTable(rowcol, 2, 10);
//	  rc = Hv_DialogRow(rowcol, 10);

//    mynewcolor = Hv_SimpleColorLabel(rc, " new color ", NULL);
//    myoldcolor = Hv_SimpleColorLabel(rc, " old color ", NULL);
*/

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

  Hv_newColorLabel = mynewcolor;
  oldcolorlabel = myoldcolor;

  Hv_SetSensitivity(ncolor, AllowNone);
  
/* the dialog has been created */

  Hv_newColor = *color;
  
  if (prompt) {
    title = Hv_CreateSimpleCompoundString(prompt);
    Hv_VaSetValues(dialog, Hv_NdialogTitle, title, NULL);
    Hv_CompoundStringFree(title);
  }
 

/* set old & new color label to reflect current color */

  Hv_SetWidgetBackgroundColor(oldcolorlabel, *color);
  Hv_SetWidgetBackgroundColor(Hv_newColorLabel, *color);

  answer = Hv_DoDialog(dialog, NULL);

/* even if person hit ok, if the color was not changed treat like a cancel */
  
  if (answer == Hv_OK) {
    if (*color == Hv_newColor)
      answer = 0;
    else
      *color = Hv_newColor;
  }
  else if ((answer == Hv_DONE)  && AllowNone)
    *color = -1;

  AllowNone = False;
  return answer;
}







