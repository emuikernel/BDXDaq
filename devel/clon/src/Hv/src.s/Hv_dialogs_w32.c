#ifdef WIN32

#define HV_W32MODAL         1
#define HV_W32MODELESS      2

#include "Hv.h"
#include "Hv_gateway_w32.h"
#include "Hv_pic.h"
#include "Hv_xyplot.h"
#include "Hv_maps.h"
#include <lmcons.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>    // includes common dialog functionality
#include <dlgs.h>       // includes common dialog template defines
#include <cderr.h>      // includes the common dialog error codes
#include "Hv_dlgtemplate.h"

static  Hv_OpenDialogs  *Hv_GetNextHwnd();

static  Hv_OpenVscrolls  *Hv_GetNextVscroll();

static void getFontRatio(short font,float *x,float *y);

static Hv_NullCallback();

static Boolean Hv_HandleDialogWidget(HWND hwnd
				     ,int win32Id,
				     UINT codeNotify);

static BOOL CALLBACK hvDlgProc (HWND hwnd, 
								UINT message, 
								WPARAM wParam, 
								LPARAM lParam);

static BOOL CALLBACK hvDlgPopupProc (HWND hwnd, 
								UINT message, 
								WPARAM wParam, 
								LPARAM lParam);

static void dialog_OnCommand (HWND hwnd,
								 int id,
								 HWND hwndCtl,
								 UINT codeNotify);


static void     Hv_SetColor(Hv_Widget,
			    int,
			    Hv_XEvent *);

static void	stuffDialogBox(Hv_Widget dialog,
						   HWND dhandle);

static void	unstuffDialogBox(Hv_Widget dialog,
							 HWND dhandle);



/* these color externs are declared in Hv_dlogs */

extern Hv_Widget Hv_newColorLabel;
extern short     Hv_newColor;
extern int       Hv_sbredrawcontrol;



static  Hv_OpenVscrolls  *openVscrolls = NULL;
static  Hv_OpenDialogs  *openDialogs = NULL;
static int               maxOpenDialogs = 0;

static int               maxOpenVscrolls = 0;


/*==============================================
 * DIALOG  RELATED SECTION OF GATEWAY
 *==============================================*/
static Hv_NullCallback(){}

/**
 * Hv_CloseOutCallback
 * @purpose Callback for one of the "action" buttons that closes the dialog.
 * @param   w       The button (widget)
 * @param   answer  e.g. Hv_OK or Hv_DONE.
 */

void  Hv_32CloseOutCallback(Hv_Widget w,
			  int       answer) {

    Hv_Widget      dialog;
    Hv_DialogData  ddata;
  
    dialog = Hv_GetOwnerDialog(w);
    
    if (dialog == NULL)
	return;

/* set the answer in the dialog data */
    
    ddata = Hv_GetDialogData(dialog);
    if (ddata != NULL)
	ddata->answer = answer;
}


/* ------- Hv_DoDialog --------*/

int  Hv_W32DoDialog(Hv_Widget dialog,
		 Hv_Widget def_btn)

{
  int   answer;

  Hv_OpenDialog(dialog, def_btn);

  while ((answer = Hv_GetDialogAnswer(dialog)) == Hv_RUNNING)
    Hv_ProcessEvent(Hv_IMAll);

/* if the answer is anything but apply, close the dialog */

  if (answer != Hv_APPLY)
    Hv_CloseDialog(dialog);

  return answer;

}

/**
 * Hv_W32Question
 * @purpose Windows 9X/NT specific version of Hv_Question.
 * @see The gateway routine Hv_Question.
 */


int Hv_W32Question(char *message) {

	char *caption;
	int   result;

	if (Hv_appName == NULL) {
		Hv_InitCharStr(&caption, "      Question      ");
	}
	else {
        caption = (char *)Hv_Malloc(strlen(Hv_programName) + 30);
        strcpy(caption, "     ");
        strcat(caption, Hv_programName);
        strcat(caption, " Question     ");
	}

	result = MessageBox(NULL, message, caption, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION);

	if (result == IDNO)
		return 0;
	else
		return 1;
}
/**
 * Hv_W32Warning
 * @purpose Windows 9X/NT specific version of Hv_Warning.
 * @see The gateway routine Hv_Warning.
 */


void Hv_W32Warning(char *message) {

	char *caption;

	if (Hv_appName == NULL) {
		Hv_InitCharStr(&caption, "      Warning      ");
	}
	else {
        caption = (char *)Hv_Malloc(strlen(Hv_programName) + 30);
        strcpy(caption, "     ");
        strcat(caption, Hv_programName);
        strcat(caption, " information     ");
	}

	MessageBox(NULL, message, caption, MB_OK | MB_DEFBUTTON1 | MB_ICONWARNING);
}

/**
 * Hv_W32Information
 * @purpose Windows 9X/NT specific version of Hv_Information.
 * @see The gateway routine Hv_Information.
 */


void Hv_W32Information(char *message) {

	char *caption;

	if (Hv_appName == NULL) {
		Hv_InitCharStr(&caption, "      Information      ");
	}
	else {
        caption = (char *)Hv_Malloc(strlen(Hv_programName) + 30);
        strcpy(caption, "     ");
        strcat(caption, Hv_programName);
        strcat(caption, " information     ");
	}

	MessageBox(NULL, message, caption, MB_OK | MB_DEFBUTTON1 | MB_ICONINFORMATION);
}

/**
 * Hv_W32CreateModalDialog
 * @purpose Windows 9X/NT specific version of Hv_CreateModalDialog.
 * @see The gateway routine Hv_CreateModalDialog.
 */

Hv_Widget Hv_W32CreateModalDialog(char *title,
				  int  ignoreclose) {

   Hv_Widget w32data = NULL;
   LPWSTR wtitle = NULL;
   char ltitle[400];

   strcpy(ltitle,title);
#ifndef _UNICODE
   ANSIToUnicode(ltitle,&wtitle);
#else
   wtitle = (LPWSTR *) Hv_Malloc(Hv_StrLen(title) +4);
   strcpy(wtitle,ltitle);
#endif
    w32data = (Hv_Widget) Hv_Malloc(sizeof(Hv_WidgetInfo));
	w32data->ignoreclose = ignoreclose;
    w32data->wlabel = wtitle;
	w32data->dialog = NULL;        /* pointer to the W32 DLGTEMPLATE structure*/
    w32data->num_controls = 0;     /* number of controls associated with the dialog*/
    w32data->num_entries = 0;      /* number of children associated with the dialog */
    w32data->modal = 1;            /* 1= modal 2 = modeless        */
    w32data->wchildren = NULL;     /* list of all cild widgets*/
	w32data->wtype = Hv_DIALOG;    /* Dialog type of course */
    w32data->wchildren = Hv_DlistCreateList(); /* start with an empty nest */
	w32data->parent = NULL;
    w32data->num_children = 0;
	return (Hv_Widget) w32data;
}


/**
 * Hv_W32CreateModelessDialog
 * @purpose Windows 9X/NT specific version of Hv_CreateModelessDialog.
 * @see The gateway routine Hv_CreateModelessDialog.
 */

Hv_Widget Hv_W32CreateModelessDialog(char *title,
				     int   ignoreclose) {
   Hv_Widget w = NULL;

   w = Hv_W32CreateModalDialog(title,ignoreclose);
   w->modal = HV_W32MODELESS;
   return w;
}


/**
 * Hv_W32EditPattern
 * @purpose Windows 9X/NT specific version of Hv_EditPattern.
 * @see The gateway routine Hv_EditPattern.
 */

int   Hv_W32EditPattern(short *pattern,
		       char  *prompt) {
	return 0;
}


/**
 * Hv_W32ChangeLabel
 * @purpose Windows 9X/NT specific version of Hv_ChangeLabel.
 * @see The gateway routine Hv_ChangeLabel.
 */

void  Hv_W32ChangeLabel(Hv_Widget w,
                        char      *label,
                        short      font) {

/* change a widget (typically a button) label */

    char              *newlabel;
	float             ww=1.0,wh=1.0;


	switch(w->wtype) {
	case MENUITEM: case TOGGLEMENUITEM: case SUBMENUITEM:
		Hv_W32SetMenuItemString(w, label, font);
		break;

	case Hv_USERITEM:
    case Hv_LABELDIALOGITEM:
	case Hv_TEXTDIALOGITEM:           
    case Hv_RADIODIALOGITEM:
    case Hv_TOGGLEDIALOGITEM:
		if(w->wfont != Hv_fixed2)
		  getFontRatio(w->wfont,&ww,&wh);

		if(label == NULL)
		  ANSIToUnicode(" ", &newlabel);
		else
		  ANSIToUnicode(label, &newlabel);
		if(w->wlabel)
			Hv_Free(w->wlabel);
		w->wlabel = newlabel;

		w->def_iwidth = 4 * Hv_StrLen(label) *ww;   /* return the width in dialog units  */
		w->def_iheight = 8 * wh;                    /* return the height in dialog units */
		if(	w->wstyle & WS_BORDER)// sunken box
			w->def_iheight +=2;

        if(w->wtype ==Hv_TEXTDIALOGITEM)
           w->def_iwidth +=2;            /* account for the edit box */
        if(w->wtype ==Hv_RADIODIALOGITEM)
           w->def_iwidth +=10;            /* account for the radio button box */
        if(w->wtype ==Hv_TOGGLEDIALOGITEM)
           w->def_iwidth +=10;            /* account for the check box */
		if(w->wisWisable)
		{
			SendDlgItemMessage( w->msdialog, w->wcallback_id, WM_SETTEXT , (WPARAM)0,(LPARAM)label);
		}

	}


}


/**
 * Hv_W32CreateThinOptionMenu
 * @purpose Windows 9X/NT specific version of Hv_CreateThinOptionMenu.
 * @see The gateway routine Hv_CreateThinOptionMenu.
 */

Hv_Widget Hv_W32CreateThinOptionMenu(Hv_Widget      parent,
				     short          font,
				     char          *title,
				     int            numitems,
				     char          *items[],
				     Hv_FunctionPtr CB,
				     short          margin) {

    int        i;
    Hv_Widget   rc= NULL,w32data = NULL,dummy=NULL;
    int        buttoncount = -1;
    Boolean    varyfont;
    Hv_AttributeArray attributes;
	int slen = 0;
    char *hstr = NULL;

//	return NULL;
    if (numitems < 1)
	return NULL;
    Hv_StandardLabel(parent, title, 0);  // create the title for the menu
    varyfont = (font < 0);

//	rc = Hv_VaCreateDialogItem(parent,   /* use the rowcolumn as a container widget */
//			       Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
//			       NULL); 
    rc = Hv_DialogColumn(parent, 0);
//    rc = Hv_DialogColumn(parent, 1);
 //   rc = Hv_DialogRow(rc, 0);

	/* now create the option menu */

  	w32data = createNewWidget(Hv_THINMENUOPTION);

    w32data->wstyle |= WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL;
	if(parent->wchildren == NULL)                /* make sure the parent knows   */ 
      parent->wchildren = Hv_DlistCreateList(); /* how to keep track of it's children  */

    Hv_DlistInsert(parent->wchildren,w32data,Hv_ENDLIST);
	parent->num_children++;
    w32data->wfont = font;
    w32data->parent = parent;

	attributes[Hv_FONT].s = font;
	attributes[Hv_LABEL].v = (void *)(items[0]);
	attributes[Hv_CALLBACK].fp = (Hv_FunctionPtr)(CB);
	attributes[Hv_DATA].v = (void *)(buttoncount);

    Hv_AddCallback(w32data,Hv_thinMenuCallback,CB,0);/* set up the master callback */

/* as we loop through,  look for "Hv_SEPARATOR" to flag
   the insertion of a separator */

    for (i = 0; i < numitems; i++)
	{
	
	  if (Hv_Contained(items[i], "Hv_SEPARATOR"))
	  {
	  }
	  else
	  {
	    buttoncount++;

	    attributes[Hv_FONT].s = font;
	    attributes[Hv_LABEL].v = (void *)(items[i]);
	    attributes[Hv_CALLBACK].fp = (Hv_FunctionPtr)(CB);
	    attributes[Hv_DATA].v = (void *)(buttoncount);
        /* These buttons won't show up in the dialog */\
		/* they are created to mimic MOTIFs way of handling thinmenus */
	    dummy = Hv_ButtonDialogItem(rc, attributes);
        dummy->disabled = 1;  /* none existant button */
        dummy->def_iheight = 0;
		dummy->def_iwidth = 0;
	    if (varyfont)
		{
		font = buttoncount*5 + 1;
		if (font < Hv_symbol11)
		    font += 25;
	    }

      	slen = Hv_StrLen(items[i]) +2;
        hstr = (char *)Hv_Malloc(slen+1);
	    strcpy(hstr,items[i]);
	    if(w32data->wdataList == NULL)                  /* create the list */ 
          w32data->wdataList = Hv_DlistCreateList(); 

        Hv_DlistInsert(w32data->wdataList,hstr,Hv_ENDLIST); /* always add to the end */
	    w32data->wdataNitems++;                            /* need to know how many items in the list */
        w32data->def_iwidth = max(w32data->def_iwidth,(4 * slen)+4);  /* return the width in dialog units */

      }
	}
	w32data->def_iheight = 8+3;
	w32data->wstate = 0;  // always start out at the begining of the list
	w32data->generic = rc;
    return  w32data;
}

static void Hv_thinMenuCB(Hv_Widget w32data)
{
	 int i=0;


     Hv_callbackData   *callback;
	 Hv_Widget parent = w32data->parent;

	 parent = w32data->parent;   /* get the thin menus parent */

	                             /* it should be a rowcolumn widget */
	 if(parent->wtype != Hv_ROWCOLUMNDIALOGITEM)
	 {
		 Hv_Warning("Another useless X like message (EXPECTED A ROWCOLUMN WIDGET)");
	 }

}

/**
 * Hv_W32GetOwnerDialog
 * @purpose Windows 9X/NT specific version of Hv_GetOwnerDialog.
 * @see The gateway routine Hv_GetOwnerDialog.
 */

Hv_Widget    Hv_W32GetOwnerDialog(Hv_Widget ditem) {
Hv_Widget   parent;

 if (ditem == NULL)
   return NULL;

  if(ditem->wtype == Hv_DIALOG)
	return ditem;    /* Dialog type of course */

 parent = Hv_Parent(ditem);
 if(parent == NULL)
	 return NULL;
 ditem = parent;  /* just incase the level is only one deep */
/* keep going up tree until the PARENT is a dialog SHELL */

 while (parent->wtype != Hv_DIALOG) {
     ditem = parent;
     parent = Hv_Parent(ditem);
     if (parent == NULL)
	 return NULL;
 }
 
 return parent;
}

/**
 * Hv_W32ActionAreaDialogItem
 * @purpose Windows 9X/NT specific version of Hv_ActionAreaDialogItem.
 * @see The gateway routine Hv_ActionAreaDialogItem.
 */

Hv_Widget       Hv_W32ActionAreaDialogItem(Hv_Widget         parent,
					   Hv_AttributeArray attributes) {
	return NULL;
}


/**
 * Hv_W32ButtonDialogItem
 * @purpose Windows 9X/NT specific version of Hv_ButtonDialogItem.
 * @see The gateway routine Hv_ButtonDialogItem.
 */

Hv_Widget       Hv_W32ButtonDialogItem(Hv_Widget         parent,
				       Hv_AttributeArray attributes) {
    LPWSTR              label;
    Hv_Widget           w32data;
    char           *labelstr = (char *)(attributes[Hv_LABEL].v);
    short           font     = attributes[Hv_FONT].s;
    Hv_CallbackProc callback = (Hv_CallbackProc)(attributes[Hv_CALLBACK].fp);
    Hv_Pointer      data     = attributes[Hv_DATA].v;
    float           wh=1.0,ww=1.0;

 	w32data = createNewWidget(Hv_BUTTONDIALOGITEM);

    w32data->parent = parent;
    w32data->wtype  = Hv_BUTTONDIALOGITEM;
    w32data->wfont  = font;
	ANSIToUnicode(labelstr, &label);
    w32data->wlabel = label;
    w32data->wtype  = Hv_BUTTONDIALOGITEM;

    if(w32data->wfont != Hv_fixed2)
		getFontRatio(w32data->wfont,&ww,&wh);


    w32data->def_iwidth = 4 * Hv_StrLen(labelstr) *ww;  /* return the width with the proper typecast */
    w32data->def_iheight = 8 *wh;  /* return the height with the proper typecast */

	/**
 	XtVaSetValues(w, Hv_NfontList, Hv_fontList, NULL);
    **/
	w32data->wdata     = data;

  	if(parent->wchildren == NULL)                 /* make sure the parent knows   */ 
        parent->wchildren = Hv_DlistCreateList(); /* how to keep track of it's children  */
 
    Hv_DlistInsert(parent->wchildren,w32data,Hv_ENDLIST);
	/*
	w32data->wcallback_id = Hv_NewWidgetCallback(callback,
						  w32data,
						  (int )data,
						  Hv_BUTTONDIALOGITEM);
						  */
	if (callback != NULL)
	   Hv_AddCallback(w32data,
		       Hv_activateCallback,
		       (Hv_CallbackProc)callback,
		       data);


	parent->num_children++;

    return w32data;
}

/**
 * Hv_W32CloseOutCallback
 * @purpose Windows 9X/NT specific version of Hv_CloseOutDialogItem.
 * @see The gateway routine Hv_CloseOutDialogItem.
 */

void  Hv_W32CloseOutCallback(Hv_Widget w,
			  int       answer, HWND hwnd)
 {
	Hv_Widget      dialog;
    Hv_DialogData  ddata;
  
    dialog = Hv_GetOwnerDialog(w);
    
    if (dialog == NULL)
	return;

/* set the answer in the dialog data */
    
    ddata = Hv_GetDialogData(dialog);
    if (ddata != NULL)
      ddata->answer = answer;

//	if(answer == Hv_CANCEL || answer == Hv_OK ||answer == Hv_DONE )
//	{
	   unstuffDialogBox(dialog,dialog->msdialog);
 //      EndDialog(dialog->msdialog,answer);  /* tell mickysoft to close the dialog */
	                                        /* don't know what todo about the apply button just yet */
//	   }
}

/**
 * Hv_W32CloseOutDialogItem
 * @purpose Windows 9X/NT specific version of Hv_CloseOutDialogItem.
 * @see The gateway routine Hv_CloseOutDialogItem.
 */

Hv_Widget       Hv_W32CloseOutDialogItem(Hv_Widget         parent,
					 Hv_AttributeArray attributes) {
	Hv_Widget     rc, dummy;
    Boolean       ok, cancel, apply, del, done;
    Hv_Widget     dialog;
    Hv_DialogData ddata;

    short     spacing     = attributes[Hv_SPACING].s;
    short     orientation = attributes[Hv_ORIENTATION].s;
    short     font        = attributes[Hv_FONT].s;
    char     *donestr     = (char *)attributes[Hv_LABEL].v;
    int       whichones   = attributes[Hv_DIALOGCLOSEOUT].i;
	int       win32id     = 0;



/* get the dialog owner (might be the parent) */

    dialog = Hv_GetOwnerDialog(parent);

    if ((dialog == NULL) || ((ddata = Hv_GetDialogData(dialog)) == NULL)) {
	Hv_Println("Error in Hv_CreateManagedCloseOut");
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
	{
  	  ddata->ok = Hv_VaCreateDialogItem(rc,
					  Hv_TYPE,     Hv_BUTTONDIALOGITEM,
					  Hv_LABEL,    "   OK   ",
					  Hv_FONT,     font,
					  Hv_CALLBACK, Hv_W32CloseOutCallback,
					  Hv_DATA,     (void *)Hv_OK,
					  NULL);
	  dialog->generic = ddata->ok; /* save the closeout in order to process a WM_CLOSE */
	                               /* message                                          */
	}
    
    if (cancel) 
	{
	  ddata->cancel = Hv_VaCreateDialogItem(rc,
					      Hv_TYPE,     Hv_BUTTONDIALOGITEM,
					      Hv_LABEL,    " Cancel ",
					      Hv_FONT,     font,
					      Hv_CALLBACK, Hv_W32CloseOutCallback,
					      Hv_DATA,     (void *)Hv_CANCEL,
					      NULL);
	  	  dialog->generic = ddata->ok;
		  /* save the closeout in order to process a WM_CLOSE */
	      /* message. NOTE this overrides the OK (see above) button */
		  /* but a CANCEL is better than a OK for a WM_CLOSE*/
	}
    
    if (apply)
	ddata->apply = Hv_VaCreateDialogItem(rc,
					     Hv_TYPE,     Hv_BUTTONDIALOGITEM,
					     Hv_LABEL,    " Apply ",
					     Hv_FONT,     font,
					     Hv_CALLBACK, Hv_W32CloseOutCallback,
					     Hv_DATA,     (void *)Hv_APPLY,
					     NULL);
    
    if (done)
	{
	  dummy = Hv_VaCreateDialogItem(rc,
				      Hv_TYPE,     Hv_BUTTONDIALOGITEM,
				      Hv_LABEL,    donestr,
				      Hv_FONT,     font,
				      Hv_CALLBACK, Hv_W32CloseOutCallback,
				      Hv_DATA,     (void *)Hv_DONE,
				      NULL);
	  	  dialog->generic = dummy; /* save the closeout in order to process a WM_CLOSE */
	                               /* message                                          */

    }
    if (del)
	ddata->del = Hv_VaCreateDialogItem(rc,
					   Hv_TYPE,     Hv_BUTTONDIALOGITEM,
					   Hv_LABEL,    (char *)" Delete",
					   Hv_FONT,     font,
					   NULL);
    return rc;
}


/**
 * Hv_W32ColorSelectorDialogItem
 * @purpose Windows 9X/NT specific version of Hv_ColorSelectorDialogItem.
 * @see The gateway routine Hv_ColorSelectorDialogItem.
 */

Hv_Widget       Hv_W32ColorSelectorDialogItem(Hv_Widget         parent,
					      Hv_AttributeArray attributes) {
    Hv_Widget   colorrc, dummy;
    int      i;
    int      nr;
    short    dx = attributes[Hv_WIDTH].s;
    short    dy = attributes[Hv_HEIGHT].s;
    
    if ((dx < 2) || (dy < 2))
	return NULL;


    nr = Hv_numColors / 16;

    colorrc = Hv_VaCreateDialogItem(parent,
				    Hv_TYPE,        Hv_ROWCOLUMNDIALOGITEM,
				    Hv_PACKING,     Hv_PAD,
				    Hv_NUMROWS,     nr,
				    Hv_SPACING,     0,
				    NULL);

    for(i = 0;  i < Hv_numColors; i++) {
//	dummy = XmCreateDrawingArea(colorrc, " ", NULL, 0);
    dummy =   Hv_VaCreateDialogItem(colorrc,
			    Hv_TYPE,        Hv_LABELDIALOGITEM,
			    Hv_LABEL,       "  ",
			    Hv_BORDERWIDTH, 1,
			    Hv_BORDERCOLOR, Hv_white,
			    Hv_FONT,        Hv_helvetica17,
			    NULL);

//	Hv_VaSetValues(dummy,
//		       Hv_Nwidth,          (Hv_Dimension)dx,
//		       Hv_Nheight,         (Hv_Dimension)dy,
//		       Hv_Nbackground,     /*(Hv_Pixel)*/(Hv_colors[i]),
//			   Hv_Nbackground,     /*(Hv_Pixel)*/(Hv_colors[i]),
//			   Hv_Nforeground,     /*(Hv_Pixel)*/(Hv_colors[i]),

//		       NULL);
Hv_W32SetWidgetBackgroundColor(dummy,
				   (short) i);
	Hv_AddEventHandler(dummy,
			   Hv_buttonPressMask,
			   (Hv_EventHandler)Hv_SetColor,
			   (Hv_Pointer)i);  
    
//	Hv_ManageChild(dummy);
    }
  
    
    return colorrc;
}

/* ------ Hv_SetColor --- */

static void Hv_SetColor (Hv_Widget w,
			 int       cnumber,
			 Hv_XEvent    *event)
{
    Hv_newColor = (short) cnumber;
    Hv_SetWidgetBackgroundColor(Hv_newColorLabel, Hv_newColor);
}



/**
 * Hv_W32FileSelectorDialogItem
 * @purpose Windows 9X/NT specific version of Hv_FileSelectorDialogItem.
 * @see The gateway routine Hv_FileSelectorDialogItem.
 */

Hv_Widget       Hv_W32FileSelectorDialogItem(Hv_Widget         parent,
					     Hv_AttributeArray attributes) {
	LPWSTR              label;
    Hv_Widget           w32data,dbox;
	float               ww=1.0,wh=1.0;
    short    dx = attributes[Hv_WIDTH].s;
    short    dy = attributes[Hv_HEIGHT].s;

    char *labelstr = (char *)(attributes[Hv_LABEL].v);
    short font     = attributes[Hv_FONT].s;
    
 	w32data = createNewWidget(Hv_FILESELECTORDIALOGITEM);

    w32data->parent = parent;
    w32data->wtype  = Hv_FILESELECTORDIALOGITEM;
    w32data->wfont  = font;
	if(w32data->wfont != Hv_fixed2)
	  getFontRatio(w32data->wfont,&ww,&wh);

	if(labelstr == NULL)
	  ANSIToUnicode(" ", &label);
    else
	  ANSIToUnicode(labelstr, &label);
    w32data->wlabel = label;

    w32data->def_iwidth = 4 * Hv_StrLen(labelstr) *ww;  /* return the width in dialog units */
    w32data->def_iheight = 8 * wh;                    /* return the height in dialog units */


  	if(parent->wchildren == NULL)                  /* make sure the parent knows   */ 
        parent->wchildren = Hv_DlistCreateList(); /* how to keep track of it's children  */

    Hv_DlistInsert(parent->wchildren,w32data,Hv_ENDLIST);
	parent->num_children++;

    dbox = Hv_GetOwnerDialog(w32data);
	if(dbox!= NULL)  /* used by the popup function to determin what routine to call */
		dbox->wdata = 3031;
    return w32data;

	return NULL;
}



/**
 * Hv_W32LabelDialogItem
 * @purpose Windows 9X/NT specific version of Hv_LabelDialogItem.
 * @see The gateway routine Hv_LabelDialogItem.
 */

Hv_Widget       Hv_W32LabelDialogItem(Hv_Widget         parent,
				      Hv_AttributeArray attributes) {

	LPWSTR              label;
    Hv_Widget           w32data;
	float               ww=1.0,wh=1.0;
    short    dx = attributes[Hv_WIDTH].s;
    short    dy = attributes[Hv_HEIGHT].s;

    char *labelstr = (char *)(attributes[Hv_LABEL].v);
    short font     = attributes[Hv_FONT].s;
    short fg;
    short bg       = attributes[Hv_BACKGROUND].s;
    short bw       = attributes[Hv_BORDERWIDTH].s;
    short bc       = attributes[Hv_BORDERCOLOR].s;



    if (attributes[Hv_TEXTCOLOR].s >= 0)
      fg = attributes[Hv_TEXTCOLOR].s;
    else
	  fg = attributes[Hv_COLOR].s;

    
/**
    if ((bw < 0 ) || (bc < 0))
	w = XtVaCreateManagedWidget("label", 
				    Hv_LabelClass,          parent,
				    Hv_NlabelString,        label,
				    Hv_NhighlightThickness, 0,
				    NULL);
    else
	w = XtVaCreateManagedWidget("label", 
				     Hv_LabelClass,          parent,
				     Hv_NlabelString,        label,
				     Hv_NhighlightThickness, 0,
				     Hv_NborderWidth,        bw,
				     Hv_NborderColor,        Hv_colors[bc],
				     NULL);

    if (font != Hv_DEFAULT)
	XtVaSetValues(w, Hv_NfontList, Hv_fontList, NULL);

	WS_EX_DLGMODALFRAME | 
                    WS_EX_CLIENTEDGE | WS_EX_STATICEDGE
    
***/

 	w32data = createNewWidget(Hv_LABELDIALOGITEM);

    Hv_ChangeLabelColors(w32data, fg, bg);


    w32data->parent = parent;
    w32data->wtype  = Hv_LABELDIALOGITEM;
    w32data->wfont  = font;
	if(w32data->wfont != Hv_fixed2)
	  getFontRatio(w32data->wfont,&ww,&wh);

	if(labelstr == NULL)
	  ANSIToUnicode(" ", &label);
    else
	  ANSIToUnicode(labelstr, &label);
    w32data->wlabel = label;

    w32data->def_iwidth = 4 * Hv_StrLen(labelstr) *ww;  /* return the width in dialog units */
    w32data->def_iheight = 8 * wh;                    /* return the height in dialog units */
	if(dx!=0)
		w32data->def_iwidth = (dx) * ww;
	if(dy!=0)
		w32data->def_iheight = (dy) * wh;

	if(bc != Hv_DEFAULT && bw >0)
	{
		w32data->wstyle |= WS_BORDER;
		w32data->def_iheight +=2;
	}

  	if(parent->wchildren == NULL)                  /* make sure the parent knows   */ 
        parent->wchildren = Hv_DlistCreateList(); /* how to keep track of it's children  */

    Hv_DlistInsert(parent->wchildren,w32data,Hv_ENDLIST);
	parent->num_children++;

    return w32data;
;
}


/**
 * Hv_W32ListDialogItem
 * @purpose Windows 9X/NT specific version of Hv_ListDialogItem.
 * @see The gateway routine Hv_ListDialogItem.
 */

Hv_Widget       Hv_W32ListDialogItem(Hv_Widget         parent,
				     Hv_AttributeArray attributes) {
    Hv_Widget          w32data;
    Hv_CompoundString *xmstitems = NULL;
    int                i;
    int             maxlen =0;
    short           numitems     = attributes[Hv_NUMITEMS].s;
    short           numvisitems  = attributes[Hv_NUMVISIBLEITEMS].s;
    char            policy       = attributes[Hv_LISTSELECTION].c;
    char            resizepolicy = attributes[Hv_LISTRESIZEPOLICY].c;
    char          **items        = (char **)(attributes[Hv_LIST].v);
    Hv_CallbackProc callback     = (Hv_CallbackProc)(attributes[Hv_CALLBACK].fp);
    short font     = attributes[Hv_FONT].s;
    float  ww=1.0,wh=1.0;

  
    if ((numitems > 0) && (items == NULL))
	    return NULL;

    for (i = 0; i < numitems; i++)
	   maxlen = max(maxlen,(int)Hv_StrLen(items[i]));

    if(numvisitems<=0)
		numvisitems = 10;
    if(maxlen<=0)
		maxlen = 8;
    
	w32data = createNewWidget(Hv_LISTDIALOGITEM);
    w32data->wfont  = font;
	if(w32data->wfont != Hv_fixed2)
	  getFontRatio(w32data->wfont,&ww,&wh);

    if (policy == Hv_MULTIPLELISTSELECT)
	{
		w32data->wstyle |= LBS_MULTIPLESEL;
		if (callback != NULL)
			Hv_AddCallback(w32data,
				   Hv_multipleSelectionCallback,
				   (Hv_CallbackProc)callback,
				   NULL);
		else
			Hv_AddCallback(w32data,
				   Hv_multipleSelectionCallback,
				   (Hv_CallbackProc)Hv_NullCallback,
				   NULL);
    }
    else {
		if (callback != NULL)
			Hv_AddCallback(w32data,
				   Hv_singleSelectionCallback,
				   (Hv_CallbackProc)callback,
				   NULL);
		else
			Hv_AddCallback(w32data,
				   Hv_singleSelectionCallback,
				   (Hv_CallbackProc)Hv_NullCallback,
				   NULL);
    }
    


    w32data->parent = parent;
    w32data->wtype  = Hv_LISTDIALOGITEM;
	/*
	w32data->wcallback_id = Hv_NewWidgetCallback(callback,
						  w32data,
						  (int )NULL,
						  Hv_LISTDIALOGITEM);
*/
    w32data->def_iwidth = (4 * maxlen *ww) +2;  /* return the width in dialog units */
    w32data->def_iheight = (9 * numvisitems*wh) +3 ;         /* return the height in dialog units */
  	if(parent->wchildren == NULL)                /* make sure the parent knows   */ 
        parent->wchildren = Hv_DlistCreateList(); /* how to keep track of it's children  */
 
    Hv_DlistInsert(parent->wchildren,w32data,Hv_ENDLIST);
	parent->num_children++;

    return w32data;
}

/*---------- Hv_W32StandardRadionButtonCallback -------------*/

static void Hv_W32StandardRadioButtonCallback(Hv_Widget w,
					   Hv_Pointer client_data)

{
  Hv_RadioHeadPtr  radiohead = (Hv_RadioHeadPtr)(client_data);
  Hv_SetActiveRadioButton(w, radiohead);
}

/**
 * Hv_W32RadioDialogItem
 * @purpose Windows 9X/NT specific version of Hv_RadioDialogItem.
 * @see The gateway routine Hv_RadioDialogItem.
 */

Hv_Widget       Hv_W32RadioDialogItem(Hv_Widget         parent,
				      Hv_AttributeArray attributes) {
    LPWSTR  label;
    Hv_Widget          w32data;
    Hv_RadioListPtr    temp;
    short              count;
    float ww=1.0,wh=1.0;
	short height = attributes[Hv_HEIGHT].s;
    
    char            *labelstr   = (char *)(attributes[Hv_LABEL].v);
    short            font       = attributes[Hv_FONT].s;
    Hv_CallbackProc  callback   = (Hv_CallbackProc)(attributes[Hv_CALLBACK].fp);
    Hv_RadioHeadPtr *radiohead  = (Hv_RadioHeadPtr *)(attributes[Hv_RADIOLIST].v);
    void            *mydata     = attributes[Hv_DATA].v;

	if(height == 0)
		height = 8;


    if (*radiohead == NULL) {
	*radiohead = (Hv_RadioHeadPtr)Hv_Malloc(sizeof(Hv_RadioHead));
	(*radiohead)->head = NULL;
	(*radiohead)->activewidget = NULL;
	(*radiohead)->activepos = 0;
    }

	w32data =  createNewWidget(Hv_RADIODIALOGITEM);
	w32data->wfont  = font;
	if(w32data->wfont != Hv_fixed2)
	  getFontRatio(w32data->wfont,&ww,&wh);

	ANSIToUnicode(labelstr, &label);
    w32data->wlabel = label;
  /*  
    w = XtVaCreateManagedWidget(" ", 
				 Hv_ToggleButtonClass,        parent,
				 Hv_NlabelString,             label,
				 Hv_NhighlightThickness,      0,
				 Hv_NindicatorType,           XmONE_OF_MANY,
				 Hv_NvisibleWhenOff,          True,
				 NULL);
  */  
    w32data->parent = parent;
    w32data->wtype  = Hv_RADIODIALOGITEM;

    w32data->def_iwidth = Hv_StrLen(labelstr) *4*ww + 10;/// * maxlen;  /* return the width in dialog units */
    w32data->def_iheight = height*wh;// * numitems;         /* return the height in dialog units */
  	if(parent->wchildren == NULL)                /* make sure the parent knows   */ 
        parent->wchildren = Hv_DlistCreateList(); /* how to keep track of it's children  */
 
    Hv_DlistInsert(parent->wchildren,w32data,Hv_ENDLIST);
	parent->num_children++;
/*
    if (callback != NULL)
	   w32data->wcallback_id = Hv_NewWidgetCallback(callback,
						  w32data,
						  (int )(*radiohead),
						  Hv_RADIODIALOGITEM);
	
	else
	   w32data->wcallback_id = Hv_NewWidgetCallback((Hv_CallbackProc)Hv_W32StandardRadioButtonCallback,
						  w32data,
						  (int )(*radiohead),
						  Hv_RADIODIALOGITEM);

*/


    if (callback != NULL)
	Hv_AddCallback(w32data,
		       Hv_valueChangedCallback,
		       (Hv_CallbackProc)callback,
		       (Hv_Pointer)parent);
    else
	Hv_AddCallback(w32data,
		       Hv_valueChangedCallback,
		       (Hv_CallbackProc)Hv_StandardRadioButtonCallback,
		       (Hv_Pointer)(*radiohead));
/*
    if (font != Hv_DEFAULT)
	XtVaSetValues(w, Hv_NfontList, Hv_fontList, NULL);
    
    Hv_CompoundStringFree(label);


*/

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
    
    temp->radio = w32data;
    temp->next = NULL;
    temp->data = mydata;
    temp->pos = count;      
    return w32data;

}



/**
 * Hv_W32ScaleDialogItem
 * @purpose Windows 9X/NT specific version of Hv_ScaleDialogItem.
 * @see The gateway routine Hv_ScaleDialogItem.
 */

Hv_Widget       Hv_W32ScaleDialogItem(Hv_Widget         parent,
				      Hv_AttributeArray attributes) {
	return NULL;
}


/**
 * Hv_W32SeparatorDialogItem
 * @purpose Windows 9X/NT specific version of Hv_SeparatorDialogItem.
 * @see The gateway routine Hv_SeparatorDialogItem.
 */

Hv_Widget       Hv_W32SeparatorDialogItem(Hv_Widget         parent,
					  Hv_AttributeArray attributes) {
			Hv_Widget           w32data;


	w32data = createNewWidget(Hv_SEPARATORDIALOGITEM);

    w32data->wtype        = Hv_SEPARATORDIALOGITEM;
   
	if(parent->wchildren == NULL)                 /* make sure the parent knows   */ 
        parent->wchildren = Hv_DlistCreateList(); /* how to keep track of it's children  */

    Hv_DlistInsert(parent->wchildren,w32data,Hv_ENDLIST);
	w32data->def_iheight = 1;
    w32data->def_iwidth = 1;
	parent->num_children++;

    return w32data;
}


/**
 * Hv_W32TextDialogItem
 * @purpose Windows 9X/NT specific version of Hv_TextDialogItem.
 * @see The gateway routine Hv_TextDialogItem.
 */

Hv_Widget       Hv_W32TextDialogItem(Hv_Widget         parent,
				     Hv_AttributeArray attributes) {
 //   Dimension     mh = 2;
 //   Dimension     mw = 2;
    char         *label = NULL;

    char            *defaultstr = (char *)(attributes[Hv_DEFAULTTEXT].v);
    short            cols       = attributes[Hv_NUMCOLUMNS].s;
    Hv_CallbackProc  vchanged   = (Hv_CallbackProc)(attributes[Hv_VALUECHANGED].fp);
    short            font       = attributes[Hv_FONT].s;
    Boolean          editable   = (Boolean)attributes[Hv_EDITABLE].i;
    char             *value;
    Hv_Widget           w32data;
    float ww=1.0,wh=1.0;
    w32data = createNewWidget(Hv_TEXTDIALOGITEM);
    
    w32data->wtype        = Hv_TEXTDIALOGITEM;
    
    if(parent->wchildren == NULL)                 /* make sure the parent knows   */ 
        parent->wchildren = Hv_DlistCreateList(); /* how to keep track of it's children  */
    
    Hv_DlistInsert(parent->wchildren,w32data,Hv_ENDLIST);
    w32data->def_iheight = 1;
    w32data->def_iwidth = 1;
    parent->num_children++;
    

    if (defaultstr != NULL) {
//	Hv_InitCharStr(&value, defaultstr);
	ANSIToUnicode(defaultstr, &label);
    w32data->wlabel = label;
//    w32data->wtextString = value;

	//XtVaSetValues(w, Hv_Nvalue, (String)value, NULL);
    }


/*
    if (font != Hv_DEFAULT)
	XtVaSetValues(w, Hv_NfontList, Hv_fontList, NULL);
*/
	w32data->wfont  = font;
	if(w32data->wfont != Hv_fixed2)
	  getFontRatio(w32data->wfont,&ww,&wh);

    if (vchanged != NULL)
		
	Hv_AddCallback(w32data,
		       Hv_valueChangedCallback,
		       (Hv_CallbackProc)vchanged,
		       NULL);
/*			   
		   w32data->wcallback_id = Hv_NewWidgetCallback((Hv_CallbackProc)vchanged,
						  w32data,
						  NULL,
						  Hv_TEXTDIALOGITEM);
*/
	if(defaultstr != NULL)
       w32data->def_iwidth = Hv_StrLen(defaultstr) *4*ww;/// * maxlen;  /* return the width in dialog units */
	else 
       w32data->def_iwidth = 20 *4;
    w32data->def_iheight = 8*wh +2;// * numitems;         /* return the height in dialog units */

    return w32data;

;
}


/**
 * Hv_W32ToggleDialogItem
 * @purpose Windows 9X/NT specific version of Hv_ToggleDialogItem.
 * @see The gateway routine Hv_ToggleDialogItem.
 */

Hv_Widget       Hv_W32ToggleDialogItem(Hv_Widget         parent,
				       Hv_AttributeArray attributes) {
    LPWSTR              label;
    Hv_Widget           w32data;

    char            *labelstr = (char *)(attributes[Hv_LABEL].v);
    short            font     = attributes[Hv_FONT].s;
    Hv_CallbackProc  callback = (Hv_CallbackProc)(attributes[Hv_CALLBACK].fp);
    Hv_Pointer       data     = (Hv_Pointer)(attributes[Hv_DATA].v);

    float            ww=1.0,wh=1.0;
 
	w32data = createNewWidget(Hv_TOGGLEDIALOGITEM);
    w32data->wtype         = Hv_TOGGLEDIALOGITEM;
   
	if(parent->wchildren == NULL)                 /* make sure the parent knows   */ 
        parent->wchildren = Hv_DlistCreateList(); /* how to keep track of it's children  */

    Hv_DlistInsert(parent->wchildren,w32data,Hv_ENDLIST);
	w32data->def_iheight = 1;
    w32data->def_iwidth = 1;
	parent->num_children++;

	ANSIToUnicode(labelstr, &label);
    w32data->wlabel = label;
	w32data->wfont  = font;
	if(w32data->wfont != Hv_fixed2)
	  getFontRatio(w32data->wfont,&ww,&wh);

  //  label = Hv_CreateCompoundString(labelstr, font);
    /*
    w = XtVaCreateManagedWidget(" ", 
				 Hv_ToggleButtonClass,        parent,
				 Hv_NlabelString,             label,
				 Hv_NhighlightThickness,      0,
				 NULL);
  
    if (font != Hv_DEFAULT)
	XtVaSetValues(w, Hv_NfontList, Hv_fontList, NULL);
    
    Hv_CompoundStringFree(label);
  */  
    if (callback)
	Hv_AddCallback(w32data,
		       Hv_valueChangedCallback,
		       callback,
		       data);
 /*
	if (callback != NULL)
	   w32data->wcallback_id = Hv_NewWidgetCallback(callback,
						  w32data,
						  NULL,
						  Hv_TOGGLEDIALOGITEM);
*/
	if(labelstr != NULL)
       w32data->def_iwidth = (Hv_StrLen(labelstr) *4) + 10;/// * maxlen;  /* return the width in dialog units */
	else 
       w32data->def_iwidth = 10 *4*ww;
    w32data->def_iheight = 8*wh;// * numitems;         /* return the height in dialog units */

    return w32data;
}

/**
 * Hv_W32RowColDialogItem
 * @purpose Windows 9X/NT specific version of Hv_RowColDialogItem.
 * @see The gateway routine Hv_RowColDialogItem.
 */

Hv_Widget Hv_W32RowColDialogItem(Hv_Widget         parent,
				 Hv_AttributeArray attributes) {
	Hv_Widget           w32data;

    short spacing;
    short orientation;
    short packing;
    short alignment;
    short nrc;

/* extract the attributes */

    alignment   = attributes[Hv_ALIGNMENT].s;
    spacing     = attributes[Hv_SPACING].s;
    orientation = attributes[Hv_ORIENTATION].s;
    packing     = attributes[Hv_PACKING].s;

/* avoid confusion over which to use, NROWS or NCOLUMNS */
    
    nrc = Hv_sMax(attributes[Hv_NUMROWS].s, attributes[Hv_NUMCOLUMNS].s);

	w32data = createNewWidget(Hv_ROWCOLUMNDIALOGITEM);

    w32data->wtype        = Hv_ROWCOLUMNDIALOGITEM;
    w32data->dalignment   = attributes[Hv_ALIGNMENT].s;
    w32data->dspacing     = attributes[Hv_SPACING].s;
    w32data->dorientation = attributes[Hv_ORIENTATION].s;
    w32data->dpacking     = attributes[Hv_PACKING].s;
	w32data->num_children = 0;
    w32data->num_entries  = nrc;
	if(parent->wchildren == NULL)                  /* make sure the parent knows   */ 
        parent->wchildren = Hv_DlistCreateList(); /* how to keep track of it's children  */

    Hv_DlistInsert(parent->wchildren,w32data,Hv_ENDLIST);
	parent->num_children++;
	w32data->parent = parent;
    return w32data;
}
/**
 * Hv_W32ScrollBarDialogItem
 * @purpose Windows 9X/NT specific version of Hv_W32ScrollBarDialogItem.
 * @see The gateway routine Hv_ScrollBarDialogItem.
 */

Hv_Widget Hv_W32ScrollBarDialogItem(Hv_Widget         parent,
			Hv_AttributeArray attributes	 ) {
	Hv_Widget           w32data;

    short spacing;
    short orientation;
    short packing;
    short alignment;
    short nrc;

/* extract the attributes */
    Hv_CallbackProc callback = (Hv_CallbackProc)(attributes[Hv_CALLBACK].fp);
    alignment   = attributes[Hv_ALIGNMENT].s;
    spacing     = attributes[Hv_SPACING].s;
    orientation = attributes[Hv_ORIENTATION].s;
    packing     = attributes[Hv_PACKING].s;

/* avoid confusion over which to use, NROWS or NCOLUMNS */
    
    nrc = Hv_sMax(attributes[Hv_NUMROWS].s, attributes[Hv_NUMCOLUMNS].s);

	w32data = createNewWidget(Hv_SCROLLBARDIALOGITEM);

    w32data->dalignment   = attributes[Hv_ALIGNMENT].s;
    w32data->dspacing     = attributes[Hv_SPACING].s;
    w32data->dorientation = attributes[Hv_ORIENTATION].s;
    w32data->dpacking     = attributes[Hv_PACKING].s;
	w32data->num_children = 0;
	w32data->def_iwidth = 12;
	w32data->def_iheight = 130;

 //   w32data->num_entries  = nrc;
	if(parent->wchildren == NULL)                  /* make sure the parent knows   */ 
        parent->wchildren = Hv_DlistCreateList(); /* how to keep track of it's children  */

    Hv_DlistInsert(parent->wchildren,w32data,Hv_ENDLIST);
	parent->num_children++;
	w32data->parent = parent;
	if (callback != NULL)
	   Hv_AddCallback(w32data,
		       Hv_valueChangedCallback,
		       (Hv_CallbackProc)callback,
		       (Hv_Pointer)parent);

    return w32data;
}


/**
 * Hv_W32ScrolledTextDialogItem
 * @purpose Windows 9X/NT specific version of Hv_ScrolledTextDialogItem.
 * @see The gateway routine Hv_ScrolledTextDialogItem.
 */

Hv_Widget Hv_W32ScrolledTextDialogItem(Hv_Widget         parent,
				       Hv_AttributeArray attributes) {
	Hv_Widget w32data;

    int        nrow;
    int        ncol;
    short      bg;
    short      fg;
    Boolean    wordwrap;
    short            font       = attributes[Hv_FONT].s;
    float            ww=1.0,wh=1.0;

    Boolean    hscroll;

//	return Hv_W32ListDialogItem(parent, attributes) ;
	/* extract the attributes */

    nrow     = (int)(attributes[Hv_NUMROWS].s);
    ncol     = (int)(attributes[Hv_NUMCOLUMNS].s);
    bg       = attributes[Hv_BACKGROUND].s;
    fg       = attributes[Hv_TEXTCOLOR].s;
    wordwrap = (attributes[Hv_WORDWRAP].i != 0);
    hscroll  = ((attributes[Hv_SCROLLPOLICY].s == Hv_SCROLLHORIZONTAL) ||
		(attributes[Hv_SCROLLPOLICY].s == Hv_SCROLLBOTH));

//	w32data = createNewWidget(Hv_SCROLLEDTEXTDIALOGITEM);
//    w32data->wtype        = Hv_SCROLLEDTEXTDIALOGITEM;
	w32data = createNewWidget(Hv_LISTDIALOGITEM);
    w32data->wtype        = Hv_LISTDIALOGITEM;

    w32data->dalignment   = attributes[Hv_ALIGNMENT].s;
    w32data->dspacing     = attributes[Hv_SPACING].s;
    w32data->dorientation = attributes[Hv_ORIENTATION].s;
    w32data->dpacking     = attributes[Hv_PACKING].s;
	w32data->num_children = 0;
	w32data->def_iheight  = nrow * 8;
	w32data->def_iwidth   = ncol * 4;

	w32data->wfont  = font;
	if(w32data->wfont != Hv_fixed2)
	  getFontRatio(w32data->wfont,&ww,&wh);


	if(parent->wchildren == NULL)                  /* make sure the parent knows   */ 
        parent->wchildren = Hv_DlistCreateList(); /* how to keep track of it's children  */

    Hv_DlistInsert(parent->wchildren,w32data,Hv_ENDLIST);
	parent->num_children++;
	w32data->parent = parent;


/* create the widget accordingly */

	w32data->dnrc = nrow;
	w32data->dncol = ncol;
//	w32data->wstyle |= ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN  | ES_READONLY ;
/*
    n = 0;
    Hv_SetArg(args[n], Hv_Nrows,                   nrow);              n++;
    Hv_SetArg(args[n], Hv_Ncolumns,                ncol);              n++;
    Hv_SetArg(args[n], Hv_Neditable,               False);             n++;
    Hv_SetArg(args[n], Hv_NeditMode,               XmMULTI_LINE_EDIT); n++;
    Hv_SetArg(args[n], Hv_NscrollHorizontal,       hscroll);           n++;
    Hv_SetArg(args[n], Hv_NscrollBarDisplayPolicy, XmSTATIC);          n++;
    Hv_SetArg(args[n], Hv_NblinkRate,              0);                 n++;
    Hv_SetArg(args[n], Hv_NautoShowCursorPosition, True);              n++;
    Hv_SetArg(args[n], Hv_NcursorPositionVisible,  False);             n++;
    Hv_SetArg(args[n], Hv_NfontList,               Hv_fontList);       n++;
    Hv_SetArg(args[n], Hv_NwordWrap,               wordwrap);          n++;
    
    w = XmCreateScrolledText(parent, "scrolled", args, n);
    */
    if (bg != Hv_DEFAULT)
	Hv_SetWidgetBackgroundColor(w32data, bg);
    
    if (fg != Hv_DEFAULT)
	Hv_SetWidgetForegroundColor(w32data, fg);
    /*
    Hv_ManageChild(w);
    Hv_ManageChild(Hv_Parent(w));  /* the window that will hold the text*/
 
    return  w32data;


}



/**
 * Hv_W32GetScaleValue
 * @purpose Windows 9X/NT specific version of Hv_GetScaleValue.
 * @see The gateway routine Hv_GetScaleValue. 
 */


int  Hv_W32GetScaleValue(Hv_Widget   w) {
	return 0;
}



/**
 * Hv_W32SetScaleValue
 * @purpose Windows 9X/NT specific version of Hv_SetScaleValue.
 * @see The gateway routine Hv_SetScaleValue. 
 */


void Hv_W32SetScaleValue(Hv_Widget   w,
		      int         val) {
}


/**
 * Hv_W32ListItemPos
 * @purpose Windows 9X/NT specific version of Hv_ListItemPos.
 * @see The gateway routine Hv_ListItemPos.
 */

int Hv_W32ListItemPos(Hv_Widget list,
                      char *str)
{
   int ii              = 0;
   Hv_LinkList *lp     = NULL;
   char        *pstr   = NULL;

   /* If no string, it can't be in the list */
   if(str == NULL) return(0);

   /* If no list, nothing can be in it */
   if(list == NULL) return(0);

   /* If no data in list, nothing can be in it */
   if ( list->wdataList == NULL ) return(0);

   /* Loop though items in list */
   /* (This is a circular list, with no data in list->wdataList) */
   lp = list->wdataList->next; ii=1;
   while( lp != list->wdataList )
   {
      /* If list data matches string */
      pstr = (char *)lp->data;
      if(strcmp(pstr,str) ==0)
      {
         /* Return the number */
         return ii;
      }
      /* Next item in list */
      lp = lp->next; ii++;
   }
   /* End of loop through list items */

   /* No match was found, so return 0 */
   return(0);
}
/* End Hv_W32ListItemPos() */

/**
 * Hv_W32CheckListItem
 * @purpose Windows 9X/NT specific version of Hv_CheckListItem.
 * @see The gateway routine Hv_CheckListItem.
 */

Boolean Hv_W32CheckListItem(Hv_Widget list,
			    char *str,
			    int  **poslist,
			    int  *num) {
   return False;
}


/**
 * Hv_W32AddListItem
 * @purpose Windows 9X/NT specific version of Hv_AddListItem.
 * @see The gateway routine Hv_AddListItem.
 */


void  Hv_W32AddListItem(Hv_Widget list,
                        char      *str,
                        Boolean    selected) {

   int slen = 0;
   char *hstr = NULL;

   slen = Hv_StrLen(str);
   hstr = (char *)Hv_Malloc(slen+1);
   strcpy(hstr,str);
   if(list->wdataList == NULL)                  /* create the list */ 
      list->wdataList = Hv_DlistCreateList(); 

   Hv_DlistInsert(list->wdataList,hstr,Hv_ENDLIST); /* always add to the end */
   list->wdataNitems++;                            /* need to know how many items in the list */
   list->def_iwidth = max(list->def_iwidth,4 * slen);  /* return the width in dialog units */
   if(list->wisWisable) /* add the string */
   {
      SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_ADDSTRING, 0,(LPARAM) str);
   }

   if(selected)
   {
      Hv_W32NewPosSelectListItem( list ,list->wdataNitems, 1);
   }
}


/**
 * Hv_W32PosReplaceListItem
 * @purpose Windows 9X/NT specific version of Hv_PosReplaceListItem.
 * @see The gateway routine Hv_PosReplaceListItem.
 */

void Hv_W32PosReplaceListItem(Hv_Widget list,
			      int       pos,
			      char      *str) {
   int policy = list->wstyle & LBS_MULTIPLESEL;  /* is it a multiple select list */
   if(list->wisWisable) 
   {
      if(policy)
      {
         SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_INSERTSTRING , 
                             (WPARAM)(UINT) pos-1,str);
         SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_DELETESTRING , 
                             (WPARAM)(UINT) pos,0);

      }
      else
      {
         SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_INSERTSTRING ,(WPARAM) pos-1,str);
         SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_DELETESTRING , 
                             (WPARAM) pos,0);

      }
   }

}
/**
 * Hv_W32ListItemExists
 * @purpose Windows 9X/NT specific version of Hv_ListItemExists.
 * @see The gateway routine Hv_ListItemExists.
 */

Boolean Hv_W32ListItemExists ( Hv_Widget list, char *str )
{
   if ( Hv_W32ListItemPos( list, str ) > 0 )
   {
      return True;
   }
   else
   {
      return False;
   }
}

/**
 * Hv_W32PosSelectListItem
 * @purpose Windows 9X/NT specific version of Hv_PosSelectListItem.
 * @see The gateway routine Hv_PosSelectListItem.
 */

void Hv_W32PosSelectListItem(Hv_Widget list,
			     int       pos) {

   Hv_W32NewPosSelectListItem(list,pos,1);

//	int policy = list->wstyle & LBS_MULTIPLESEL;  /* is it a multiple select list */

//	if(list->wisWisable) /* find the string in question */
//	{
//	if(policy)
//		SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_SETSEL, 1,
//		                     (LPARAM)(UINT) pos-1);
//	else
//		SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_SETCURSEL, pos-1, 0);
//	list->wstate = pos;
//	}
}


/**
 * Hv_W32NewPosSelectListItem
 * @purpose Windows 9X/NT specific version of Hv_NewPosSelectListItem.
 * @see The gateway routine Hv_NewPosSelectListItem.
 */

void Hv_W32NewPosSelectListItem(Hv_Widget list,
				int       pos,
				Boolean   cbflag) {
   int     *pint = NULL;
   int     *ptemp = NULL;
   int     *newWdata = NULL;
   int      howmany = 0;
   int      ii;
   Hv_LinkList *wp = NULL,*rp = NULL;
   Boolean  selected = 0;
   Boolean  isMultipleSelectList;
   Hv_callbackData   *callback;
   /* is it a multiple select list */
   isMultipleSelectList = list->wstyle & LBS_MULTIPLESEL;

   /* If the list is visible */
   if(list->wisWisable)
   {
      /* Is the item in this position selected? */
      selected = SendDlgItemMessage( list->msdialog,
                                     list->wcallback_id,
                                     LB_GETSEL,
                                     (WPARAM)(UINT)pos-1,0 );

      /* If this is a multiple selection list */
      if(isMultipleSelectList)
      {
         /* Toggle the item on or off */
         SendDlgItemMessage( list->msdialog,
                             list->wcallback_id,
                             LB_SETSEL,
                             !selected,
                             (LPARAM)(UINT)pos-1 );
      }
      /* Otherwise, this is a single selection list */
      else
      {
         /* If the item is selected */
         if(selected)
         {
            /* Deselect all items */
            SendDlgItemMessage( list->msdialog,
                                list->wcallback_id,
                                LB_SETCURSEL,
                                -1,
                                0);
         }
         /* Otherwise */
         else
         {
            /* Select the item (note the -1 for Win32) */
            SendDlgItemMessage( list->msdialog,
                                list->wcallback_id,
                                LB_SETCURSEL,
                                pos-1,
                                0);
         }
         /* end if (item is selected) */
      }
      /* End if (multiple selection list) */
      list->wstate = pos;
   }
   /* List is not visible.  Save data to be used by stuffdialog() */
   else    
   {
      /* If this is a multiple selection list */
      if(isMultipleSelectList)
      {
         /* Assume pos is not already selected */
         selected = False;

         /* find out how many items are in list, and how many will be */
         /* (do not count if entry matches pos, because we will toggle it off) */
         pint = (int *)list->wdata;
         if(pint != NULL)
         {
            howmany = 0; ii=0;
            while(pint[ii] != -1) 
            {
               if ( pint[ii] == pos )
               {
                  selected = True;
               }
               else
               {
                  howmany++;
               }
               ii++;
            }
         }
         else
         {
            howmany = 0;
         }

         /* If item at pos is already selected */
         if ( selected )
         {
            /* Allocate a new array */
            newWdata = Hv_Malloc( (howmany+1)*sizeof(int) );

            /* Copy original wdata to newWdata, leaving out pos */
            pint = (int*)list->wdata;

            for( ii=0; ii<howmany; ii++ )
            {
               /* Do not copy entry of selected position */
               while( *pint == pos ) pint++;

               /* Copy data to new array */
               newWdata[ii] = *pint;

               /* Next data point */
               pint++;
            }
            /* End of loop copying data */

            /* Put on terminator */
            newWdata[howmany] = -1;

            /* Free old data */
            Hv_Free( list->wdata );

            /* Assign list->wdata to new array */
            list->wdata = newWdata;
         }
         /* Else current pos is not selected */
         else
         {
            /* account for the end of list and the current entry */
            howmany +=2;
            list->wdata = realloc(list->wdata,howmany*sizeof(int));

            /* now add the new item to the end of the list */
            pint = (int *)list->wdata;
            pint[howmany-2] = pos;
            pint[howmany-1] = -1;
         }
         /* End if (current pos is selected) */
      }
      /* Otherwise, it's a single selection list */
      else
      {
         /* save the position for future use in "stuffdialogbox()" */
         list->wstate = pos;
      }
      /* end if (multiple selection list) */
   }
   /* End if (list is visible) */

   /* If callback flag is set, dispatch the callback  */
   if(cbflag)
   {
	   	if(list->wcallList)  
			{
		       for (wp = list->wcallList->next; wp != list->wcallList; wp = wp->next)
			   {
				  callback = (Hv_callbackData *)(wp->data);
				  if((callback->icallbackType ==  Hv_MULTIPLESELECTIONCALLBACK) ||
					 (callback->icallbackType ==  Hv_SINGLESELECTIONCALLBACK))
				   {
					   callback->callback(list,callback->clientPointer,callback->clientData,callback->callbackType);
				   }

			   }
			}

//   widgetDispatcher(list,pos,NULL,LBN_SELCHANGE);
   }
}

void Hv_W32NewNewPosSelectListItem(Hv_Widget list,
                                   int       pos,
                                   Boolean   cbflag) {
   int     *pint = NULL;
   int     *ptemp = NULL;
   int     *newWdata = NULL;
   int      howmany = 0;
   int      ii;
   Boolean  selected = False;
   Boolean  isMultipleSelectList;

   /* is it a multiple select list */
   isMultipleSelectList = list->wstyle & LBS_MULTIPLESEL;

   /* If the list is visible */
   if(list->wisWisable)
   {
      selected = SendDlgItemMessage( list->msdialog,
                                     list->wcallback_id,
                                     LB_GETSEL,
                                     (WPARAM)(UINT)pos-1,
                                     0);

      if(isMultipleSelectList)
      {
         SendDlgItemMessage( list->msdialog,
                             list->wcallback_id,
                             LB_SETSEL,
                             !selected,
                             (LPARAM)(UINT)pos-1 );
      }
      else
      {
         if(selected)
         {
            SendDlgItemMessage( list->msdialog,
                                list->wcallback_id,
                                LB_SETCURSEL,
                                -1,
                                0);
         }
         else
         {
            SendDlgItemMessage( list->msdialog,
                                list->wcallback_id,
                                LB_SETCURSEL,
                                pos-1,
                                0);
         }
      }
      list->wstate = pos;
   }
   else    /* save data to be used by stuffdialog() */
   {
      if(isMultipleSelectList)
      {
         /* Assume pos is not already selected */
         selected = False;

         /* find out how many items are already selected */
         /* (do not count if entry matches pos, because we will toggle it off) */
         pint = (int *)list->wdata;
         if(pint != NULL)
         {
            howmany = 0; ii=0;
            while(pint[ii] != -1) 
            {
               if ( pint[ii] == pos )
               {
                  selected = True;
               }
               else
               {
                  howmany++;
               }
               ii++;
            }
         }
         else
         {
            howmany = 0;
         }

         /* If item at pos is already selected */
         if ( selected )
         {
            /* Allocate a new array */
            newWdata = Hv_Malloc( (howmany+1)*sizeof(int) );

            /* Copy original wdata to newWdata, leaving out pos */
            pint = (int*)list->wdata;

            for( ii=0; ii<howmany; ii++ )
            {
               /* Do not copy entry of selected position */
               while( *pint == pos ) pint++;

               /* Copy data to new array */
               newWdata[ii] = *pint;

               /* Next data point */
               pint++;
            }
            /* End of loop copying data */

            /* Put on terminator */
            newWdata[howmany] = -1;

            /* Free old data */
            Hv_Free( list->wdata );

            /* Assign list->wdata to new array */
            list->wdata = newWdata;
         }
         /* Else current pos is not selected */
         else
         {
            /* account for the end of list and the current entry */
            howmany +=2;
            list->wdata = realloc(list->wdata,howmany*sizeof(int));

            /* now add the new item to the end of the list */
            pint = (int *)list->wdata;
            pint[howmany-2] = pos;
            pint[howmany-1] = -1;
         }
         /* End if (current pos is selected) */
      }
      else
      {
         /* save the position for future use in "stuffdialogbox()" */
         list->wstate = pos;
      }
      widgetDispatcher(list,pos,NULL,LBN_SELCHANGE);
   }
}


/**
 * Hv_W32ListSelectItem
 * @purpose Windows 9X/NT specific version of Hv_ListSelectItem.
 * @see The gateway routine Hv_SelectListItem.
 */

void  Hv_W32ListSelectItem(Hv_Widget list,
                           char     *str,
                           Boolean  callit)
{
   Hv_LinkList *ilist;
   Hv_LinkList *lp;
   int sel;
   int found = 0;
   int i = 0;
   char *pstr = NULL;

   if(list->wisWisable) /* find the string in question */
   {
      sel = SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_FINDSTRING, -1,(LPARAM) str);
      Hv_W32NewPosSelectListItem(list,sel+1,callit);
   }
   else
   {
      if(str == NULL)
      {
         found = 1;
         i = -1;
      }
      else
      {
         ilist = list->wdataList;

         if(ilist == NULL)                  /* nothing to do */ 
            return;

         for (i=1,lp = ilist->next; lp != ilist; lp=lp->next,i++)
         {
            pstr = (char *)lp->data;
            if(strcmp(pstr,str) ==0)
            {
               found = 1;
               break;
            }
         }
      }
      if(found)
      {
        Hv_Println("FOUND LIST ITEM %s", str);
         list->wstate = i;
         Hv_W32NewPosSelectListItem(list,i,callit);
         /*widgetDispatcher(list,i,NULL,LBN_SELCHANGE);*/
      }
      else {
        Hv_Println("did NOT find LIST ITEM %s", str);
       }
   }
}


/**
 * Hv_W32DeleteAllListItems
 * @purpose Windows 9X/NT specific version of Hv_DeleteAllListItems.
 * @see The gateway routine Hv_DeleteAllListItems.
 */

void  Hv_W32DeleteAllListItems(Hv_Widget list) {

   Hv_LinkList *ilist;
   Hv_LinkList *lp,*rp;
   int i;



   ilist = list->wdataList;

   if(ilist == NULL)                  /* nothing to do */ 
      return;

   for (i=0,lp = ilist->next; lp != ilist; i++)
   {
      Hv_Free((char *)lp->data);
      rp = lp;
      lp = lp->next;
      Hv_Free(rp);
   }
   Hv_Free(ilist);

   if(i != list->wdataNitems)
      Hv_Warning("Useless error message (trying to replicate X) too few list items");
   list->wdataNitems = 0;                            /* need to know how many items in the list */
   list->wdataList = NULL;
   if(list->wisWisable) /* delete the contents of the control */
   {
      SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_RESETCONTENT, 0, 0);
   }
}


/**
 * Hv_W32DeselectAllListItems
 * @purpose Windows 9X/NT specific version of Hv_DeselectAllListItems.
 * @see The gateway routine Hv_DeselectAllListItems.
 */

void  Hv_W32DeselectAllListItems(Hv_Widget list) {
   int isMultipleSelectList = list->wstyle & LBS_MULTIPLESEL;  /* is it a multiple select list */

   list->wstate = 0;
   if(list->wisWisable)
   {
      list->wstate = 0;
      if(isMultipleSelectList)
         SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_SETSEL,0, -1);
      else
         SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_SETCURSEL, (WPARAM)-1, 0);

   }
   else
   {
      if( list->wdata != NULL )
      {
         Hv_Free( list->wdata );
         list->wdata = NULL;
      }
   }
}



/**
 * Hv_W32GetListSelectedPos
 * @purpose Windows 9X/NT specific version of Hv_.GetListSelectedPos
 * @see The gateway routine Hv_.GetListSelectedPos
 */

int Hv_W32GetListSelectedPos(Hv_Widget list) {
   int sel;
   if(list->wisWisable)
   {
      sel = SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_GETCURSEL, 0, 0);
      list->wstate = sel+1;
      sel++;
   }
   else
   {
      sel = list->wstate;
   }
   return sel;
} 



/**
 * Hv_W32GetAllListSelectedPos
 * @purpose Windows 9X/NT specific version of Hv_GetAllListSelectedPos.
 * @see The gateway routine Hv_GetAllListSelectedPos.
 */

int  *Hv_W32GetAllListSelectedPos(Hv_Widget list) {
   long    count = 0,count1 = 0;
   int     *lpnItems = NULL;
   int     *pint = NULL;
   int     ii;
   Boolean isMultipleSelectList;

   /* is it a multiple select list */
   isMultipleSelectList = list->wstyle & LBS_MULTIPLESEL;

   /* If the list is visible */
   if(list->wisWisable)
   {
      /* Get number of selected items */
      count = SendDlgItemMessage( list->msdialog,
                                  list->wcallback_id,
                                  LB_GETSELCOUNT, 0, 0);

      /* Check return value for error */
      if ( count < 0 ) return NULL;

      /* Allocate an array for the positions, plus 1 for the terminating -1 */
      lpnItems = Hv_Malloc((count+1)*sizeof(int));

      /* Put array of selected item numbers into lpnItems */
      count1= SendDlgItemMessage( list->msdialog,
                                  list->wcallback_id,
                                  LB_GETSELITEMS,
                                  (WPARAM) count,
                                  (LPARAM) (LPINT) lpnItems);

      /* Check return value for error again */
      if ( count1 < 0 )
      {
         Hv_Free( lpnItems );
         return NULL;
      }

      /* Check for error in number retrieved */
      if(count != count1)
      {
         Hv_Warning("Something amiss in multiple selections");
         Hv_Free( lpnItems );
         return NULL;
      }

      /* Hv and X use 1 based indeces, Mickeysoft uses zero based indeces, */
      /* so we have to add 1 to each position value */
      for(ii=0;ii<count;ii++)
      {
         /* Increment the index number for Hv */
         lpnItems[ii] += 1;
      }

      /* Put in the terminating -1 */
      lpnItems[count] = -1;
   }
   /* Otherwise, the list is not visible */
   /* Copy list->wdata, so the calling routine */
   /* may safely free the returned array */
   else
   {
      /* Get count of items in list->wdata */
      count = 0;
      pint = list->wdata;
      if ( pint != NULL )
      {
         while ( pint[count] != -1 )
         {
            count++;
         }
      }

      /* Allocate an array for the positions, plus 1 for the terminating -1 */
      lpnItems = Hv_Malloc((count+1)*sizeof(int));

      /* Loop through array, copying from list->wdata to lpnItems */
      pint = (int*)list->wdata;
      for (ii = 0; ii<count; ii++ )
      {
         lpnItems[ii] = pint[ii];
      }

      /* Put in the terminating -1 */
      lpnItems[count] = -1;
   }

   /* Return array of selected item indeces */
   return lpnItems;
}


/**
 * Hv_W32ChangeLabelPattern
 * @purpose Windows 9X/NT specific version of Hv_ChangeLabelPattern.
 * @see The gateway routine Hv_ChangeLabelPattern.
 */

void  Hv_W32ChangeLabelPattern(Hv_Widget w,
			       short     pattern) {
	int i = 0;
    long error = 0;
    RECT Rect;
	if(w== NULL)
		return;
	w->wdata = pattern;
	if(w->wisWisable)
	{
		if(w->wbgColor != Hv_DEFAULT)
		{
	       i = GetClientRect(  w->mscontrol,&Rect);
		   error = GetLastError();
		   i = RedrawWindow(w->mscontrol, &Rect, NULL,  RDW_INVALIDATE    ); 
		   error = GetLastError();
		}
	}
}


/**
 * Hv_W32AddLineToScrolledText
 * @purpose Windows 9X/NT specific version of Hv_AddLineToScrolledText.
 * @see The gateway routine Hv_AddLineToScrolledText. 
 */


void  Hv_W32AddLineToScrolledText(Hv_Widget        list,
				  Hv_TextPosition *pos,
				  char     *line) {
	int slen = 0;
    char *hstr = NULL;

	slen = Hv_StrLen(line);
    hstr = (char *)Hv_Malloc(slen+1);
	strcpy(hstr,line);
	if(list->wdataList == NULL)                  /* create the list */ 
        list->wdataList = Hv_DlistCreateList(); 

    Hv_DlistInsert(list->wdataList,hstr,Hv_ENDLIST); /* always add to the end */
	list->wdataNitems++;                            /* need to know how many items in the list */
    list->def_iwidth = max(list->def_iwidth,4 * slen);  /* return the width in dialog units */
	if(list->wisWisable) /* add the string */
	{
		SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_ADDSTRING, 0,(LPARAM) line);
	}

}


/**
 * Hv_W32AppendLineToScrolledText
 * @purpose Windows 9X/NT specific version of Hv_AppendLineToScrolledText.
 * @see The gateway routine Hv_AppendLineToScrolledText. 
 */


void Hv_W32AppendLineToScrolledText(Hv_Widget     list,
				 char          *line) {
	int slen = 0;
    char *hstr = NULL;
    DWORD err =0;
	long i;
	slen = Hv_StrLen(line);
    hstr = (char *)Hv_Malloc(slen+1);
	strcpy(hstr,line);
	if(list->wdataList == NULL)                  /* create the list */ 
        list->wdataList = Hv_DlistCreateList(); 

    Hv_DlistInsert(list->wdataList,hstr,Hv_ENDLIST); /* always add to the end */
	list->wdataNitems++;                            /* need to know how many items in the list */
    list->def_iwidth = max(list->def_iwidth,4 * slen);  /* return the width in dialog units */
	if(list->wisWisable) /* add the string */
	{
    SetLastError(0);
    i =	SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_ADDSTRING,(WPARAM) 0,(LPARAM) line);
   
    i =	SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_SETCURSEL,(WPARAM) i,(LPARAM) 0);
 //   i =	SendDlgItemMessage( list->msdialog, list->wcallback_id, LB_ADDSTRING,(WPARAM) 0,(LPARAM) line);

	err = GetLastError();	
	}


}



/**
 * Hv_W32JumpToBottomOfScrolledText
 * @purpose Windows 9X/NT specific version of Hv_JumpToBottomOfScrolledText.
 * @see The gateway routine Hv_JumpToBottomOfScrolledText. 
 */


void Hv_W32JumpToBottomOfScrolledText(Hv_Widget        w,
				   Hv_TextPosition *pos) {
}


/**
 * Hv_W32DeleteLinesFromTop
 * @purpose Windows 9X/NT specific version of Hv_DeleteLinesFromTop.
 * @see The gateway routine Hv_DeleteLinesFromTop. 
 */


void  Hv_W32DeleteLinesFromTop(Hv_Widget        w,
                               int              nlines) {
	Hv_LinkList *wp,*lp;
	int i = 0;
	int remaining = 0;
        DWORD err = 0;

	if(w->wdataList != NULL)
	{
           if(nlines == -1)  // calculate the number of lkines in the list
           {
              for (nlines=0,lp = w->wdataList->next; lp != w->wdataList; lp = lp->next,nlines++);
           }

           if(w->wisWisable)  // delete from the screen if visable 
           {
              for(i=0;i<nlines;i++)
              {
                 remaining = SendDlgItemMessage(w->msdialog, w->wcallback_id, LB_DELETESTRING,(WPARAM)0,(LPARAM) 0);
                 err = GetLastError();
              }
           }

           /* point the the first itme in the list */
           wp = w->wdataList->next;
           for(i=0;i<nlines;i++)
           {
              lp = wp->next;
              if(wp->data != NULL) Hv_Free(wp->data);
              Hv_DlistDeque ( wp );
              Hv_Free(wp);
              wp = lp;
              /* don't delete the head of the list */
              if(wp == w->wdataList) break;
           }
	}
}


/**
 * Hv_W32DeleteScrolledTextLines
 * @purpose Windows 9X/NT specific version of Hv_DeleteScrolledTextLines.
 * @see The gateway routine Hv_DeleteScrolledTextLines. 
 */

void  Hv_W32DeleteScrolledTextLines(Hv_Widget        w,
				 int              nlines,
				 Hv_TextPosition *endpos) {
 Hv_W32DeleteLinesFromTop(w,nlines);
 *endpos = 0;
}



/**
 * Hv_W32ClearScrolledText
 * @purpose Windows 9X/NT specific version of Hv_ClearScrolledText.
 * @see The gateway routine Hv_ClearScrolledText. 
 */

void Hv_W32ClearScrolledText(Hv_Widget        w,
			  Hv_TextPosition *pos) {
	  Hv_DeleteScrolledTextLines(w, -1, pos);
      *pos = 0;
}




static void	stuffDialogBox(Hv_Widget dialog,HWND dhandle)
{
   Hv_LinkList *lp,*wp;
   Hv_WidgetInfo *w;
   LPWSTR wdefaultString = NULL;
   long error = 0;
   int *pint = 0;
   static int depth = 0;
   Hv_OpenVscrolls  vv;

   if(dialog == NULL)
      return;
   dialog->msdialog = dhandle;
   if(dialog->wchildren && (dialog->num_children >0))
   {
      for (lp = dialog->wchildren->next; lp != dialog->wchildren; lp = lp->next)
      {
         w = (Hv_WidgetInfo *) lp->data;
         w->msdialog  = dhandle;
         if(w->wtype != Hv_ROWCOLUMNDIALOGITEM)          /* container only not a real dialog control */
            w->mscontrol =  GetDlgItem(  dhandle,           /* handle of dialog box  */
                                         w->wcallback_id    /* identifier of control */
               );
         w->wisWisable = 1;

         switch(w->wtype)
         {
         case Hv_BUTTONDIALOGITEM:
            if(w->wfont != Hv_fixed2)
               SendDlgItemMessage(dhandle, w->wcallback_id, WM_SETFONT,(WPARAM) Hv_fonts[w->wfont], 0);
            break;
         case Hv_COLORSELECTORDIALOGITEM:  
            break;
         case Hv_SEPARATORDIALOGITEM:
            break;
         case Hv_LISTDIALOGITEM: 
            if(w->wdataList)  /* see if we need to stuff the list */
            {
               for (wp = w->wdataList->next; wp != w->wdataList; wp = wp->next)
               {
                  SendDlgItemMessage(dhandle,
                                     w->wcallback_id,
                                     LB_ADDSTRING,
                                     0,
                                     (LPARAM)wp->data); 
                  // SendMessage(dhandle, WM_SETREDRAW, 1, 0);
               }
               if(w->wstyle & LBS_MULTIPLESEL)
               {
                  pint = (int *)w->wdata;
                  if(pint != NULL)
                  {
                     while( *pint != -1 )
                     { 
                        Hv_W32NewNewPosSelectListItem(w,*pint,1);
                        pint++;
                     }
                  }
               }
               else
               {
                  Hv_W32NewNewPosSelectListItem(w,w->wstate,1);
               }
            }
            if(w->wfont != Hv_fixed2)
            {
               SendDlgItemMessage(dhandle,
                                  w->wcallback_id,
                                  WM_SETFONT,(WPARAM)
                                  Hv_fonts[w->wfont],
                                  0);
            }
            break;
         case Hv_ROWCOLUMNDIALOGITEM:
            break;
         case Hv_SCALEDIALOGITEM: 
            break;
         case Hv_SCROLLBARDIALOGITEM:
            vv = Hv_GetNextVscroll();
            if(vv != NULL)
            {
               vv->hwnd   = w->mscontrol;
               vv->w      = w;
               vv->id     = w->wcallback_id;
               vv->dialog = dhandle;
            }
            break;
         case Hv_RADIODIALOGITEM:
         case Hv_TOGGLEDIALOGITEM: 
            SendDlgItemMessage( dhandle,
                                w->wcallback_id,
                                BM_SETCHECK,
                                w->wstate,
                                0);
            if(w->wfont != Hv_fixed2)
            {
               SendDlgItemMessage(dhandle,
                                  w->wcallback_id,
                                  WM_SETFONT,
                                  (WPARAM)Hv_fonts[w->wfont],
                                  0); 
            }
            error = GetLastError();
            break;
         case Hv_TEXTDIALOGITEM:
            if(w->wfont != Hv_fixed2)
            {
               SendDlgItemMessage(dhandle,
                                  w->wcallback_id,
                                  WM_SETFONT,
                                  (WPARAM) Hv_fonts[w->wfont],
                                  0); 
            }
            break;
         case Hv_CLOSEOUTDIALOGITEM:
            break;
         case Hv_SCROLLEDTEXTDIALOGITEM:
            if(w->wfont != Hv_fixed2)
            {
               SendDlgItemMessage(dhandle,
                                  w->wcallback_id,
                                  WM_SETFONT,
                                  (WPARAM) Hv_fonts[w->wfont],
                                  0);
            }
            break;
         case Hv_LABELDIALOGITEM: 
            if(w->wfont != Hv_fixed2)
            {
               SendDlgItemMessage(dhandle,
                                  w->wcallback_id,
                                  WM_SETFONT,
                                  (WPARAM) Hv_fonts[w->wfont],
                                  0);
            } 
            break;
         case Hv_FILESELECTORDIALOGITEM:  
            break;
         case Hv_ACTIONAREADIALOGITEM:
            break;
         case Hv_DIALOG:   
            break;
         case Hv_THINMENUOPTION:
            if(w->wdataList)  /* see if we need to stuff the box */
            {
               for (wp = w->wdataList->next; wp != w->wdataList; wp = wp->next)
               {
                  error =   SendDlgItemMessage(dhandle,
                                               w->wcallback_id,
                                               CB_ADDSTRING,
                                               0,
                                               (LPARAM)wp->data); 
                  error = GetLastError();
               }

               if(w->wfont != Hv_fixed2)
               {
                  SendDlgItemMessage(dhandle,
                                     w->wcallback_id,
                                     WM_SETFONT,
                                     (WPARAM) Hv_fonts[w->wfont],
                                     0);
               }
               SendDlgItemMessage(dhandle,
                                  w->wcallback_id,
                                  CB_SETCURSEL,(WPARAM)
                                  w->wstate,
                                  0);
            }
            break;
         default:
            break;
         }

         if(w->wchildren)
         {
            stuffDialogBox( w, dhandle);
         }

      }
   }
}

static void	unstuffDialogBox(Hv_Widget dialog,HWND dhandle)
{
   Hv_LinkList *lp;
   Hv_WidgetInfo *w;
   LPWSTR wdefaultString = NULL;
   long tlength;
   static int depth = 0;
   long lState = 0;;
   
   if(dialog->wchildren)
   {
      for (lp = dialog->wchildren->next; lp != dialog->wchildren; lp = lp->next)
      {
         w = (Hv_WidgetInfo *) lp->data;
         
         w->wisWisable = 0;

         switch(w->wtype)
         {
         case Hv_BUTTONDIALOGITEM:  
            break;
         case Hv_COLORSELECTORDIALOGITEM:  
            break;
         case Hv_SEPARATORDIALOGITEM:
            break;
         case Hv_LISTDIALOGITEM:
            w->wisWisable = 1;
            if(w->wstyle & LBS_MULTIPLESEL)
            {
               if(w->wdata)
                  Hv_Free(w->wdata);
               w->wdata = Hv_W32GetAllListSelectedPos(w);
            }
            else
            {
               w->wstate = (int)SendDlgItemMessage( w->msdialog,
                                                    w->wcallback_id,
                                                    LB_GETCURSEL, 0, 0);
               w->wstate++ ;
            }
            w->wisWisable = 0;
            break;
         case Hv_ROWCOLUMNDIALOGITEM:
            break;
         case Hv_SCALEDIALOGITEM: 
            break;
         case Hv_RADIODIALOGITEM: 
         case Hv_TOGGLEDIALOGITEM:
            lState = SendDlgItemMessage( w->msdialog, w->wcallback_id, BM_GETCHECK, 0, 0);
            lState &= 0x03; /* mask of the useless bits */
            w->wstate = (int)lState;
            break;

         case Hv_TEXTDIALOGITEM:
            tlength = SendDlgItemMessage( w->msdialog,
                                          w->wcallback_id,
                                          WM_GETTEXTLENGTH , 0, 0);
            if(tlength >0)
            {
               if(w->wtextString)
                  Hv_Free(w->wtextString);
               w->wtextString = (char *)Hv_Malloc(tlength+1);
               SendDlgItemMessage( w->msdialog,
                                   w->wcallback_id,
                                   WM_GETTEXT, 
                                   (WPARAM) tlength+1,
                                   (LPARAM) w->wtextString);
            }
            break;
         case Hv_CLOSEOUTDIALOGITEM:
            break;
         case Hv_SCROLLEDTEXTDIALOGITEM:  
            break;
         case Hv_LABELDIALOGITEM:  
            break;
         case Hv_FILESELECTORDIALOGITEM:  
            break;
         case Hv_ACTIONAREADIALOGITEM:
            break;
         case Hv_DIALOG:   
            break;
         case Hv_THINMENUOPTION:
            w->wstate =	SendDlgItemMessage( w->msdialog,
                                            w->wcallback_id,
                                            CB_GETCURSEL, 
                                            (WPARAM) NULL,
                                            (LPARAM) NULL);
            break;
         default:
            break;
         }

         if(w->wchildren)
         {
            unstuffDialogBox( w, dhandle);
         }
      }
   }
}


static BOOL CALLBACK
hvDlgProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Hv_Item              Item= NULL;
	long                 tlength;
    int                  redo = 0;
	int                  win32Id;
	Hv_Widget            control;
	static 	HBITMAP      hbm = 0;
    short                color = 0;
    int                  i = 0;
    int                  nScrollCode = 0; // scroll bar value 
    int                  nPos = 0;  // scroll box position 
    HWND                 hwndScrollBar = NULL;      // handle to scroll bar 
    Hv_Widget            w32data = NULL;
    Hv_LinkList          *wp = NULL;
	Hv_callbackData      *callback = NULL;

	static HBRUSH hBrushDlg = NULL; 
	static HBRUSH hBrushEdit = NULL;    //brush handles for new colors done with hook proc 
	static HBRUSH hBrushButton = NULL; 
/*	    Hv_Println("SHW ID = %x %d %d", message,wParam,lParam);

            if (message == BN_PUSHED)
			{
		  	   printf("somebody pushed me %d",message);
			} */

	switch(message)
	{	
	case STN_CLICKED:
	case STN_DBLCLK:
/*		printf("Iam a static click %ld",message); */
		return TRUE;
		break;
	case WM_CTLCOLORMSGBOX:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORDLG: 
	case WM_CTLCOLORSCROLLBAR:
		return False;
		break;
	case WM_CTLCOLORSTATIC:
		 win32Id =  GetDlgCtrlID( (HWND)  lParam);   // handle of control);
		 if(win32Id == Hv_stc32)
			 return FALSE;
		 control =  callbacks[win32Id]->parent;
		 if(hBrushDlg)
		 {
			 DeleteObject(hBrushDlg);
			 hBrushDlg = NULL;
		 }
		 if(control->wbgColor != Hv_DEFAULT)
			color = control->wbgColor;
		 else
			color = Hv_black;
			color = Hv_black;

		  SetBkMode((HDC)wParam, TRANSPARENT);
          SetTextColor((HDC)wParam, RGB(0, 0, 0));
	

		 if(control->wsubtype != NULL)  // it is mostlily a patern label
		 {
			 switch((INT)control->wdata)
			 {
				case Hv_FILLSOLIDPAT:
					hBrushDlg = CreateSolidBrush(Hv_colors[color]);
					break;
				case Hv_80PERCENTPAT:
				    hBrushDlg = CreateHatchBrush(HS_DIAGCROSS, Hv_colors[color]);
					break;

				case Hv_50PERCENTPAT:
					hBrushDlg = CreateHatchBrush(HS_DIAGCROSS,Hv_colors[color]);
					break;

				case Hv_20PERCENTPAT:
					hBrushDlg = CreateHatchBrush(HS_CROSS, Hv_colors[color]);
					break;

				case Hv_HATCHPAT:
					hBrushDlg = CreateHatchBrush(HS_CROSS, Hv_colors[color]);
					break;

				case Hv_DIAGONAL1PAT:
					hBrushDlg = CreateHatchBrush(HS_BDIAGONAL,Hv_colors[color]);
					break;

				case Hv_DIAGONAL2PAT:
					hBrushDlg = CreateHatchBrush(HS_FDIAGONAL,Hv_colors[color]);
					break;

				case Hv_HSTRIPEPAT:
					hBrushDlg = CreateHatchBrush(HS_HORIZONTAL, Hv_colors[color]);
					break;

				case Hv_VSTRIPEPAT:
					hBrushDlg = CreateHatchBrush(HS_VERTICAL, Hv_colors[color]);
					break;

				case Hv_SCALEPAT:
				case Hv_STARPAT:
						hbm = Hv_CreateBitMapFromPattern(control->wdata, Hv_colors[color]);
						hBrushDlg = CreatePatternBrush(hbm);
						DeleteObject(hbm);
					break;

				case Hv_HOLLOWPAT:
				default:
					hBrushDlg = CreateSolidBrush(Hv_colors[color]);
			 }
		   return hBrushDlg;
		 }
		 if(control->wfgColor != Hv_DEFAULT)
	       SetTextColor((HDC) wParam,  Hv_colors[control->wfgColor]) ;  

         if(control->wbgColor != Hv_DEFAULT)
		 {
	       SetBkColor((HDC) wParam,  Hv_colors[control->wbgColor]) ;
		   hBrushDlg = CreateSolidBrush(Hv_colors[control->wbgColor]) ;
		 }
		 else
		 {
		//	 hBrushDlg = GetStockObject(LTGRAY_BRUSH); 
		//	 SetBkMode((HDC) wParam, TRANSPARENT) ;               //transparent text 
			 return FALSE;
		 }
         return (UINT) hBrushDlg ; 
		 break;

    case WM_INITDIALOG: /* before the dialog open stuff all of the controls */
	   stuffDialogBox((Hv_Widget)lParam,hwnd);
	   return TRUE;

	case WM_CLOSE: 
		for (i = 0; i < maxOpenDialogs; i++)
		{ /* find the open window */
		  if ((openDialogs[i] != NULL) && (openDialogs[i]->hwnd == hwnd))
		  {
			 /* now call the close routine for that window */
             Hv_Widget  closeout;
             closeout = (Hv_Widget) openDialogs[i]->dialog->generic;
			 if(closeout == NULL)  // never found out what the close was so call the generic popdown version
				                   // this could wreck havoc with the users program but who cares
								   // since they saw fit to no use a standard closeout
				 Hv_W32PopdownDialog(openDialogs[i]->dialog);
			 else
			    Hv_HandleDialogWidget(hwnd,closeout->wcallback_id,0);
			break;
		  }
		}

	   return TRUE;

    case WM_LBUTTONDOWN:

       return FALSE;
	case WM_COMMAND:

         HANDLE_WM_COMMAND (hwnd, wParam, lParam, dialog_OnCommand) ;
		 return 1;
	case WM_VSCROLL:

        nScrollCode = (int) LOWORD(wParam); // scroll bar value 
        nPos = (short int) HIWORD(wParam);  // scroll box position 
        hwndScrollBar = (HWND) lParam;      // handle to scroll bar 
		for (i = 0; i < maxOpenVscrolls; i++)
		{ /* find the open window */
		  if ((openVscrolls[i] != NULL) && (openVscrolls[i]->hwnd == hwndScrollBar))
		  {
			 /* now call the vscroll routine for that window */
            w32data = openVscrolls[i]->w;
			if(w32data->wcallList)  
			{
		       for (wp = w32data->wcallList->next; wp != w32data->wcallList; wp = wp->next)
			   {
				   callback = (Hv_callbackData *)(wp->data);
				   if(callback->icallbackType == Hv_VALUECHANGEDCALLBACK)
				   {
					  return  callback->callback(w32data,nScrollCode,nPos,hwnd);//hwndScrollBar);
				   }
			   }
			}

			break;

			break;
		  }
		}
//	   return TRUE;

		return 0;
	case WM_MOUSELEAVE:
		if(Hv_activeTextItem != NULL)   // textentry 
		{
		   redo = 1;
		   Item = 	Hv_activeTextItem;
		   if (Item->str->text != NULL)
			   {
				   Hv_Free (Item->str->text);
			   }
			tlength = SendDlgItemMessage( hwnd, 3033, WM_GETTEXTLENGTH , 0,0);
			if(tlength >0)
			{
			  Item->str->text = (char *)Hv_Malloc(tlength+1);
			  SendDlgItemMessage( hwnd, 3033, WM_GETTEXT , 
										(WPARAM) tlength+1, (LPARAM) Item->str->text);
			}
		Hv_activeTextItem = NULL;
		}
	   EndDialog(hwnd,1);
	   if(redo)
	     Hv_StandardRedoItem(Item);
       return TRUE;
	default:
		return FALSE;
	}
}

static BOOL CALLBACK
hvDlgPopupProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Hv_Item Item= NULL;
	long tlength;
    int redo = 0;
	int win32Id;
	Hv_Widget control;
	static HBRUSH hBrushDlg = NULL; 
	static HBRUSH hBrushEdit = NULL;    //brush handles for new colors done with hook proc 
	static HBRUSH hBrushButton = NULL; 

/*	printf("id = %d",message); */
	switch(message)
	{	
	case STN_CLICKED:
	case STN_DBLCLK:
/*		printf("Iam a static click %ld",message);*/
		return TRUE;
		break;
	case WM_CTLCOLORMSGBOX:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORDLG: 
	case WM_CTLCOLORSCROLLBAR:
		return False;
		break;
	case WM_CTLCOLORSTATIC:
		 win32Id =  GetDlgCtrlID(   lParam);   // handle of control);
		 control =  callbacks[win32Id]->parent;
         if(control->wbgColor != Hv_DEFAULT)
		 {
	       SetBkColor((HDC) wParam,  Hv_colors[control->wbgColor]) ;
		   hBrushDlg = CreateSolidBrush(Hv_colors[control->wbgColor]) ;
		  if(control->wgarbageList == NULL)                  
             control->wgarbageList = Hv_DlistCreateList(); 
          Hv_DlistInsert(control->wgarbageList,hBrushDlg,Hv_ENDLIST);
	      control->wgarbageCount++;

		 }
		 else
		 {
			 hBrushDlg = GetStockObject(LTGRAY_BRUSH); 
			 SetBkMode((HDC) wParam, TRANSPARENT) ;               //transparent text 
		 }
         if(control->wfgColor != Hv_DEFAULT)
	       SetTextColor((HDC) wParam,  Hv_colors[control->wfgColor]) ;  
         return (UINT) hBrushDlg ; 
		 break;
    case WM_LBUTTONDOWN:

       return FALSE;

    case WM_INITDIALOG: /* before the dialog open stuff all of the controls */
	   stuffDialogBox((Hv_Widget)lParam,hwnd);
	   return TRUE;

	case WM_CLOSE: 
	   return TRUE;
	case WM_COMMAND:
       return  HANDLE_WM_COMMAND (hwnd, wParam, lParam, dialog_OnCommand) ;
	case WM_MOUSELEAVE:
		if(Hv_activeTextItem != NULL)   // textentry 
		{
		   redo = 1;
		   Item = 	Hv_activeTextItem;
		   if (Item->str->text != NULL)
			   {
				   Hv_Free (Item->str->text);
			   }
			tlength = SendDlgItemMessage( hwnd, 3033, WM_GETTEXTLENGTH , 0,0);
			if(tlength >0)
			{
			  Item->str->text = (char *)Hv_Malloc(tlength+1);
			  SendDlgItemMessage( hwnd, 3033, WM_GETTEXT , 
										(WPARAM) tlength+1, (LPARAM) Item->str->text);
			}
		Hv_activeTextItem = NULL;
		}
	   EndDialog(hwnd,1);
	   if(redo)
	     Hv_StandardRedoItem(Item);
       return TRUE;
	default:
		return FALSE;
	}
}

/*==============================================
 * TEXTENTRY WIDGET  RELATED SECTION OF GATEWAY
 *==============================================*/


/**
 * Hv_W32TextEntryItemInitialize
 * @purpose Windows 9X/NT specific version of Hv_TextEntryItemInitialize.
 * @see The gateway routine Hv_TextEntryItemInitialize.
 */


//#if(_WIN32_WINNT < 0x0100)

   VOID CALLBACK
   TrackMouseTimerProc(HWND hWnd,UINT uMsg,UINT idEvent,DWORD dwTime) {
      RECT rect;
      POINT pt;

      GetClientRect(hWnd,&rect);
      MapWindowPoints(hWnd,NULL,(LPPOINT)&rect,2);
      GetCursorPos(&pt);
      if (!PtInRect(&rect,pt))
	  {
		  if( (WindowFromPoint(pt) != hWnd))
		  {
             if (!KillTimer(hWnd,idEvent))
			 {
            // Error killing the timer!
			 }
			 PostMessage(hWnd,WM_MOUSELEAVE,(WPARAM)hWnd,(LPARAM)hWnd);
		  }
      }
   }

   BOOL
   TrackMouseEvent(LPTRACKMOUSEEVENT ptme) {
      OutputDebugString(TEXT("TrackMouseEvent\n"));

      if (!ptme || ptme->cbSize < sizeof(TRACKMOUSEEVENT)) {
         OutputDebugString(TEXT("TrackMouseEvent: invalid "
                                "TRACKMOUSEEVENT structure\n"));
         return FALSE;
      }

      if (!IsWindow(ptme->hwndTrack)) {
         OutputDebugString(
            TEXT("TrackMouseEvent: invalid hwndTrack\n"));
         return FALSE;
      }

      if (!(ptme->dwFlags & TME_LEAVE)) {
         OutputDebugString(TEXT("TrackMouseEvent: invalid dwFlags\n"));
         return FALSE;
      }
      return SetTimer(ptme->hwndTrack, ptme->dwFlags,
                      100,(TIMERPROC)TrackMouseTimerProc);
   }


   LRESULT CALLBACK
   MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
      TRACKMOUSEEVENT tme;
static int fInWindow = TRUE;
      switch (uMsg) {
         case WM_CREATE:
            fInWindow = FALSE;
            return 0;

         case WM_MOUSEMOVE:
            if (!fInWindow) {
               fInWindow = TRUE;
               tme.cbSize = sizeof(TRACKMOUSEEVENT);
               tme.dwFlags = TME_LEAVE;
               tme.hwndTrack = hWnd;
               if (!TrackMouseEvent(&tme)) {
                  MessageBox(hWnd,
                             TEXT("TrackMouseEvent Failed"),
                             TEXT("Mouse Leave"),MB_OK);
               }
            }
            break;

         case WM_MOUSELEAVE:
            fInWindow = FALSE;
            MessageBox(hWnd,TEXT("Elvis has left the building"),
                       TEXT("Mouse Leave"),MB_OK);
            break;

         default:
            return DefWindowProc(hWnd,uMsg,wParam,lParam);
      }
      return FALSE;
   }


void Hv_W32TextEntryItemInitialize(Hv_Item           Item,
				  Hv_AttributeArray attributes) {

  Hv_Widget   dialog = NULL,row= NULL,column = NULL;



  static char  transtable[] =
    "<Key>osfBackSpace: delete-previous-character()";
  
  static char  trans2table[] =
    "<Key>osfDelete: delete-previous-character()";


/* first initialize as a normal text item*/

  Hv_TextItemInitialize (Item, attributes) ;

/* override the text item double click */

  Item->doubleclick = (Hv_FunctionPtr)(attributes[Hv_DOUBLECLICK].fp);

  if (attributes[Hv_SINGLECLICK].v == NULL)
    Item->singleclick = Hv_TextEntrySingleClick;
  

/* allocate the tag along Hv widget */
  Hv_VaCreateDialog(&dialog, Hv_TITLE, "", Hv_TYPE,   Hv_MODELESS, NULL);
  Item->data=dialog;/* set the sub type to textentry widget */
  dialog->wdata = (void *)3030; /* set the subtype to texten */

  column = Hv_DialogColumn(dialog, 1);
  row    = Hv_DialogRow(column, 1);
  dialog->generic = Hv_VaCreateDialogItem(row,
                                      Hv_TYPE,        Hv_TEXTDIALOGITEM,
                                      Hv_FONT,        Hv_fixed2,
                                      Hv_TEXTCOLOR,   Hv_black,
                                      Hv_NUMCOLUMNS,  2,
                                      NULL);

/*
  editmode = XmSINGLE_LINE_EDIT; 
  if (attributes[Hv_TEXTENTRYMODE].s == Hv_MULTILINEEDIT)
    editmode = XmMULTI_LINE_EDIT; 

  w = XtVaCreateWidget("textfield", 
		       xmTextWidgetClass,      Hv_toplevel,
		       Hv_NshadowThickness,     0,
		       Hv_Nwidth,               Item->area->width - 2,
		       Hv_Nheight,              Item->area->height - 2,
		       Hv_NhighlightThickness,  0,
		       Hv_NeditMode,            editmode,
		       NULL) ;
*/
 
/* value changed callback ? If so
   add one with the Hv_Item as the data*/

/*
  if ((attributes[Hv_VALUECHANGED].fp) != NULL)
    Hv_AddCallback(w,
		   Hv_valueChangedCallback,
		   (Hv_CallbackProc)(attributes[Hv_VALUECHANGED].fp),
		   (Hv_Pointer)Item);

*/
/*
 *  The next two translation overrides were added 5/26/96
 *  to try to fix the wrongway delete on all platforms
 */
/*
  XtOverrideTranslations(w, XtParseTranslationTable(transtable));
  XtOverrideTranslations(w, XtParseTranslationTable(trans2table));
*/

/**
IDD_DIALOG1 DIALOGEX 0, 0, 188, 94
STYLE WS_POPUP | WS_DISABLED
EXSTYLE WS_EX_TRANSPARENT
FONT 8, "MS Sans Serif"
BEGIN
    EDITTEXT        IDC_EDIT1,7,7,174,80,ES_AUTOHSCROLL | NOT WS_BORDER,
                    WS_EX_TRANSPARENT
END
**/
  

  Item->standarddraw = Hv_DrawTextEntryItem;
  Hv_DefaultSchemeIn(&(Item->scheme));

  if (attributes[Hv_FILLCOLOR].s != Hv_DEFAULT)
    Item->scheme->fillcolor = attributes[Hv_FILLCOLOR].s;
  else
    Item->scheme->fillcolor -= 1;
	  return;
}


/**
 * Hv_W32TextEntrySingleClick
 * @purpose Windows 9X/NT specific version of Hv_TextEntrySingleClick.
 * @see The gateway routine Hv_TextEntrySingleClick.
 */

void Hv_W32TextEntrySingleClick(Hv_Event hevent) {
  LPDLGTEMPLATE templ;
  LPDLGITEMTEMPLATE item;

  HINSTANCE hinst = GetWindowInstance (Hv_mainWindow) ;
  Hv_Item Item = hevent->item;
  Hv_Rect area;
  Hv_Widget dialog = NULL;
  Hv_Widget textarea = NULL;
  TRACKMOUSEEVENT tme;
  HWND lwnd;

  char *t ;
  int  left, top;
  long l= 0;
  int baseunitX  = LOWORD(GetDialogBaseUnits()); 
  int baseunitY = HIWORD(GetDialogBaseUnits()); 

  int templateunitX = 0; 
  int templateunitY = 0;
  long error = 0;
  LPWSTR wtitle = NULL;
  if (Item->str == NULL)
    return;

#ifndef _UNICODE
   ANSIToUnicode(Item->str->text,&wtitle);
#else
   wtitle = (LPWSTR *) Hv_Malloc(StrLen(Item->str->text) +4);
   strcpy(wtitle,Item->str->text);
#endif

  if (Item->str == NULL)
    return;

  dialog = (Hv_Widget)(Item->data);
  textarea = (Hv_Widget)(dialog->generic);

/* if text is NULL go ahead an give them a string */
  /* this prevents a lot of NULL checking later */

  if (Item->str->text == NULL) {
    Hv_InitCharStr(&(Item->str->text), " ");
    *(Item->str->text) = '\0';
  }

  Hv_activeTextItem = Item ;
//  Hv_DanglingTextItemCallback = Hv_TEMouseDestroy ;
  Hv_CopyRect (&area, Hv_GetItemArea (Item)) ;
  t = Hv_GetItemText (Item) ;

  left = area.left+2;
  top  = area.top+4; 

  dialog->ix  = (left * 4) / baseunitX ;
  dialog->iy = (top * 8) / baseunitY ;
  textarea->def_iheight = 8;
  textarea->def_iwidth = ((Item->area->width - 2) *4)   / baseunitX;
			 	         templ = DIALOG(&item, DLGTEMPLATE_WORKING_SIZE,
      			dialog->ix, dialog->iy, 
				((Item->area->width - 2) *4)   / baseunitX,
				((Item->area->height - 2) *8)  / baseunitY,
		   WS_POPUP | WS_VISIBLE,
			0,
			NULL,		// menu
			NULL,		// class
			NULL,// caption
			NULL, //"System",	// font
			8);	    	// font height

       if(templ == NULL)
	     return;
       templ->cdit++;	 
		EDITTEXT(item,3033, 
                       1, 1, ((Item->area->width - 2) *4)   / baseunitX, 8, WS_VISIBLE ,
						 
                        0,wtitle );
//		Hv_DoDialog((Hv_Widget)Item->data,NULL);
        lwnd = CreateDialogIndirectParam(hinst,templ,Hv_mainWindow,
								 (DLGPROC) hvDlgProc,(LPARAM)NULL);
        error = GetLastError();
         
		tme.cbSize      = sizeof(TRACKMOUSEEVENT); 
		tme.dwFlags     = TME_LEAVE; 
		tme.hwndTrack   = (HWND)lwnd;//textarea->msdialog; 
		TrackMouseEvent(&tme);                      
		if(wtitle)
		  Hv_Free(wtitle);
		if(templ)
			free(templ);
}


static int level = 0;
static void getWidgetRowsCols(Hv_WidgetInfo *w,int *num_rows, int *num_cols)
{
	int num_entries;

	if(w->num_entries == 0)
		num_entries = 1;
	else
		num_entries = w->num_entries;

  if(w->dorientation == Hv_VERTICAL)
	  {
	   *num_cols = num_entries;
	   *num_rows = w->num_children/num_entries;
	   if(*num_rows <=0)
		 *num_rows = 1;
	  }
 	  else
	  if(w->dorientation == Hv_HORIZONTAL)
	  {
	   *num_cols = w->num_children/num_entries;
	   *num_rows = num_entries;
	   if(*num_cols <=0)
		   *num_cols = 1;
	  }
	  else
	  {
	    *num_cols = 1;
	    *num_rows = 1;
	  }




}

static int getWidgetHeight(Hv_WidgetInfo *w)
{
	
	Hv_LinkList *list;
	Hv_LinkList *lp;
    Hv_WidgetInfo *resetWidgets[400];
	int seperatorCount = 0;
    int widgetPointer = 0;
    int h = 0;
    int tot_h = 0;
    Hv_WidgetInfo *cw;
	int num_cols = 1;
	int num_rows = 1;
	int ccount = 1;
    int max_height = 0;
    int i;
	char lstr[190];

	strcpy(lstr,"");

    level +=2;
	for (i=0;i<level;i++)
		strcat(lstr,"");

	list = w->wchildren;
	if(list == NULL)
	{
		return(w->def_iheight);
	}

    getWidgetRowsCols(w,&num_rows,&num_cols);
	if(list && (w->num_children >0))
	{
	  for (i=0,lp = list->next; lp != list; lp = lp->next,i++);
	
      for (lp = list->next; lp != list; lp = lp->next) /* process all the childen for this widget*/
	  {
		cw = (Hv_WidgetInfo *) lp->data;
        resetWidgets[widgetPointer++] = cw;

		if(cw->wchildren)  /* does it have children */
		{
          h =  getWidgetHeight(cw);
		  cw->iheight = h;
		}
		else
		{
		  h = cw->def_iheight;
		  cw->iheight = h;
		}

 		max_height = max(max_height,h);

		if(((ccount % num_cols) == 0) || (cw->wtype == Hv_SEPARATORDIALOGITEM)) /* a new row so reset the row height */
		{
			// this gaurentees that we will be as high as the highest widget */
			// however if the user set a height (will be small or same size)  then 
			// set the height to the user default

			for(i=0;i<widgetPointer;i++)
			{
			  if(resetWidgets[i]->set_iheight != 0)
                resetWidgets[i]->iheight =resetWidgets[i]->set_iheight;
			  else		
                resetWidgets[i]->iheight = max_height;
			}
			tot_h = tot_h + max_height + (w->dspacing/8) + 1;
			max_height = 0;
			widgetPointer = 0;
		}
        if(cw->wtype == Hv_SEPARATORDIALOGITEM)
			seperatorCount++;
		ccount++;
	  }
	  if(widgetPointer)
	  {
	    for(i=0;i<widgetPointer;i++) /* catch the odd number of rows */
		{
		  if(resetWidgets[i]->set_iheight != 0)
            resetWidgets[i]->iheight =resetWidgets[i]->set_iheight;
		  else		
            resetWidgets[i]->iheight = max_height;
		}
		tot_h = tot_h + max_height + (w->dspacing/8);

	  }
	}
	level -=2;
	return tot_h + 2*seperatorCount;
}

static int getWidgetWidth(Hv_WidgetInfo *w)
{

	Hv_LinkList *list;
	Hv_LinkList *lp;
    Hv_WidgetInfo *resetWidgets[40][100];
	int col_widths[400];

    int widgetPointer = 0;
    int wi = 0;
    int tot_w = 0;
    Hv_WidgetInfo *cw;
	int num_cols = 1;
	int num_rows = 1;
    int row_cnt = 0;
	int ccount = 1;
    int max_width = 0;
	int num_child = 0;
    int i,j;
    int new_row = 0;
	int new_col = 0;
	int col_cnt = 0;
	int row_num = 0;

	for(i=0;i<40;i++)
		for(j=0;j<100;j++)
          resetWidgets[i][j] = NULL;
	for(i=0;i<400;i++)
		col_widths[i] = 0;

	getWidgetRowsCols(w,&num_rows,&num_cols);
	list = w->wchildren;
    num_child = w->num_children;
	if(list && (num_child > 0))     // this catchs the idots who put no widgets in the dialog box
	{
	  for (i=0,lp = list->next; lp != list; lp = lp->next,i++);
	
      for (lp = list->next; lp != list; lp = lp->next) /* process all the childen for this widget*/
	  {
		cw = (Hv_WidgetInfo *) lp->data;

		if(cw->wchildren)  /* does it have children */
		{

          wi =  getWidgetWidth(cw);
		  cw->iwidth = wi;

		}
		else
		{
		  wi = cw->def_iwidth;
		  cw->iwidth = wi;
		}

		if(cw->wtype == Hv_SEPARATORDIALOGITEM) /* a new col so reset the row width */
		{
			new_col = 1;
		}
		if(col_cnt == num_cols)
		{
		    col_cnt = 0;
		    new_row = 1;
		    row_num++;
		}
		new_col = 1;


 		col_widths[col_cnt] = max(col_widths[col_cnt],wi);
        resetWidgets[row_num][col_cnt] = cw;
        if(new_col == 1)
			col_cnt++;
		ccount++;
		new_row = 0;
		new_col = 0;
	  }

	}
	for(i=0;i<num_cols;i++)
	{
		tot_w += col_widths[i] + (w->dspacing/*4*/);
		for(j=0;j<num_rows;j++)
		{
         if(resetWidgets[j][i] != NULL)
		 {
			 // this gaurentees that we will be as wide as the widest widget */
			// however if the user set a width (will be small or same size)  then 
			// set the width to the user default
			  if(resetWidgets[j][i]->set_iwidth != 0)
                 resetWidgets[j][i]->iwidth = resetWidgets[j][i]->set_iwidth;
			  else		
                resetWidgets[j][i]->iwidth = col_widths[i]; 
			}
		}
	}
	return tot_w;
}

static void sety(Hv_Widget w,int startDepth)
{
	Hv_LinkList *list;
	Hv_LinkList *lp;
    int widgetPointer = 0;
    int old_height = 0;
    int tot_w = 0;
    Hv_WidgetInfo *cw;
	int num_cols = 1;
	int num_rows = 1;
	int ccount = 1;
    int max_width = 0;
    int i;
    int currentStartHeight= startDepth;
	int new_row = 0;
	int new_col = 0;
    int col_cnt = 0;
	int row_num = 0;


	list = w->wchildren;
    getWidgetRowsCols(w,&num_rows,&num_cols);
    currentStartHeight = startDepth;
	if(list && (w->num_children > 0))
	{
	  for (i=0,lp = list->next; lp != list; lp = lp->next,i++);
	
      for (lp = list->next; lp != list; lp = lp->next) /* process all the childen for this widget*/
	  {
		cw = (Hv_WidgetInfo *) lp->data;

		    if(cw->wtype == Hv_SEPARATORDIALOGITEM)
			{
				col_cnt = 0;
				row_num++;
	    		new_row = 1;
			}
		if(col_cnt == num_cols)
		{
			col_cnt = 0;
			new_row = 1;
			row_num++;
		}
			  new_col = 1;

		if(new_row == 1)
		{
		   if(old_height == 0)
				old_height = cw->iheight;
		   currentStartHeight = currentStartHeight + old_height + (w->dspacing/8) +1;
		   cw->iy = currentStartHeight;
		   old_height = cw->iheight;
		}

        cw->iy = currentStartHeight;

		if(cw->wchildren)  /* does it have children */
		{
            sety(cw,currentStartHeight);//  + ((new_row ==1) ? (old_height+ w->dspacing/8) : 0) );
		}

        if(new_col == 1)
			col_cnt++;
		ccount++;
		new_row = 0;
		new_col = 0;
        old_height = cw->iheight;
	    if(cw->set_iy != 0)
		  cw->iy = cw->set_iy;

	  }
	}
	return ;

}
static void setx(Hv_Widget w,int startWidth)
{
	Hv_LinkList *list;
	Hv_LinkList *lp;
    int widgetPointer = 0;
    int old_width = 0;
    int tot_w = 0;
    Hv_WidgetInfo *cw;
	int ccount = 1;
	int num_cols = 1;
	int num_rows = 1;
    int max_width = 0;
    int i;
    int currentStartWidth = startWidth;
	int new_row = 1;
	int new_col = 0;
    int col_cnt = 0;
	int row_cnt = 0;
   

	list = w->wchildren;

    getWidgetRowsCols(w,&num_rows,&num_cols);
	if(list && (w->num_children >0))
	{
	  for (i=0,lp = list->next; lp != list; lp = lp->next,i++);
	
      for (lp = list->next; lp != list; lp = lp->next) /* process all the childen for this widget*/
	  {
		cw = (Hv_WidgetInfo *) lp->data;


	if(cw->wtype == Hv_SEPARATORDIALOGITEM) /* a new col so reset the row width */
	{
	    new_col = 1;
	}
			if(col_cnt == num_cols)
			{
				col_cnt = 0;
				new_row = 1;
			}
			new_col = 1;

		ccount++;
		if(new_col == 1)
		{
		   currentStartWidth = currentStartWidth + old_width + w->dspacing /*4*/;
		   cw->ix = currentStartWidth;

		   col_cnt++;
		}
		if(new_row == 1)
		{
		   cw->ix = startWidth;
		   currentStartWidth = startWidth;
		}
		if(cw->wchildren)  /* does it have children */
		{
            setx(cw,new_row == 1 ? startWidth : currentStartWidth  );
			old_width = cw->iwidth;
		}


		new_row = 0;
		new_col=0;
		old_width = cw->iwidth;
		if(cw->set_ix != 0)
		  cw->ix = cw->set_ix;
	  }
	}
	return ;


}

static void buildDialogDef(Hv_Widget dialog)
{


    static int depth = 0;
    char pstring[200];
	int i;
    int width,height;

	depth+=2;
    strcpy(pstring," ");

	/* this also sets the widhts and heights of the children widgets */
	width = getWidgetWidth(dialog) ;
	height = getWidgetHeight(dialog)  ;

	/* check to see if the user set the width and height of the dialog box */
	/* if they did then use their values rather than the computed ones */

	if(dialog->set_iheight == 0)
  	  dialog->iheight =height  ;
	else
	  dialog->iheight =dialog->set_iheight  ;

	if(dialog->set_iwidth == 0)
	  dialog->iwidth = width ;
	else
      dialog->iwidth = dialog->set_iwidth;
/*****
Using this information, you can compute the dialog base units for a dialog box using font other than system font: 

horz pixels == 2 * horz dialog units * (average char width of dialog font 

                        / average char width of system font)
vert pixels == 2 * vert dialog units * (average char height of dialog font 
                        / average char height of system font)

As the font of a dialog box changes, the actual size and position of a control also changes. 
One dialog box base unit is equivalent to the number of pixels per dialog box unit as follows: 

1 horz dialog base unit == (2 * average char width dialog font / 

                                average char width  system font) pixels
1 vert dialog base unit == (2 * average char height dialog font / 
                                average char height system font) pixels

Average character width and height of a font can be computed as follows: 
hFontOld = SelectObject(hdc,hFont); GetTextMetrics(hdc,&tm); GetTextExtentPoint32(hdc,"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrst" 

            "uvwxyz",52,&size);
avgWidth = (size.cx/26+1)/2; avgHeight = (WORD)tm.tmHeight; 
The tmAveCharWidth field of the TEXTMETRIC structure only approximates the actual average character width
 (usually it gives the width of the letter "x") and so the true average character width must be calculated 
 to match the value used by the system. 

You can use the MapDialogRect function to convert dialog box units into pixels, but there is no function
 that will convert pixels into dialog box 
*****/
	  setx(dialog,0);
	  sety(dialog,0);
    
}

static void processChildren(Hv_Widget dialog,LPDLGITEMTEMPLATE *item,LPDLGTEMPLATE templ)
{
	Hv_LinkList *lp;
    Hv_WidgetInfo *w;
    LPWSTR wdefaultString = NULL;

    static int depth = 0;

	int i;
    int disabled = 0;

	if(dialog == NULL)
		i=0;
	if(dialog->wchildren && (dialog->num_children >0))
	{
      for (lp = dialog->wchildren->next; lp != dialog->wchildren; lp = lp->next)
	  {
		w = (Hv_WidgetInfo *) lp->data;
//		disabled = w->wdisabled ? WS_DISABLED : 0;
		if(w->disabled) // this should only be thinoption menu buttons
			continue;   // also note that this is not the same as "wdisabled"
		                // as wdisabled simply created a greyed out widget
		                // where as disabled doesn;t create the widget at all
         if(w->wdisabled )
			 disabled = WS_DISABLED;
		 else
			 disabled = 0;
		switch(w->wtype)
		{
		case Hv_BUTTONDIALOGITEM:  
			  templ->cdit++;
              *item =  PUSHBUTTON(*item, w->wlabel, w->wcallback_id,
				                w->ix, w->iy,
				                 w->iwidth, w->iheight,
      		                	/*WS_GROUP | */WS_TABSTOP | WS_VISIBLE | disabled
								| w->wstyle    ,w->wstyleex );
			break;
		case Hv_COLORSELECTORDIALOGITEM:  
			break;
		case Hv_SEPARATORDIALOGITEM:
			templ->cdit++;
            *item = CONTROL(*item, L"", w->wcallback_id, L"Static", 
      					SS_GRAYFRAME | SS_SUNKEN | WS_VISIBLE | disabled, 
      					w->ix, w->iy, w->iwidth, w->iheight, 0);

			break; 
		case Hv_LISTDIALOGITEM:  
			templ->cdit++;
			*item =  LISTBOX(*item, w->wcallback_id, 

                        w->ix, w->iy, w->iwidth, w->iheight, WS_VISIBLE |// WS_CLIPSIBLINGS |
						LBS_NOTIFY |  WS_BORDER | WS_VISIBLE | WS_CHILD | WS_VSCROLL | 
						LBS_HASSTRINGS | disabled 
                        | w->wstyle,w->wstyleex);
			break;
		case Hv_ROWCOLUMNDIALOGITEM:
			break;
		case Hv_SCALEDIALOGITEM: 
			break;
		case Hv_SCROLLBARDIALOGITEM:
			templ->cdit++;
			*item =  SCROLLBAR(*item, w->wcallback_id,
                        w->ix, w->iy, w->iwidth, w->iheight, 
						WS_VISIBLE | WS_TABSTOP | SBS_VERT | SBS_RIGHTALIGN  |
						disabled ,
                        w->wstyleex);
			break;
		case Hv_RADIODIALOGITEM: 
			templ->cdit++;
			*item =  RADIOBUTTON(*item, w->wlabel,w->wcallback_id, 
                        w->ix, w->iy, w->iwidth, w->iheight,
						WS_VISIBLE | WS_TABSTOP | disabled  
                        | w->wstyle,w->wstyleex);

			break;
		case Hv_TOGGLEDIALOGITEM: 
			templ->cdit++;
			*item =  AUTOCHECKBOX(*item, w->wlabel,w->wcallback_id, 
                        w->ix, w->iy, w->iwidth, w->iheight,
						WS_VISIBLE | WS_TABSTOP | disabled  
                        | w->wstyle,w->wstyleex);

			break;

		case Hv_TEXTDIALOGITEM:
			wdefaultString = NULL;
			if(w->wtextString != NULL)
			{
		      ANSIToUnicode(w->wtextString, &wdefaultString);
			}

			templ->cdit++;
			*item =  EDITTEXT(*item,w->wcallback_id, 
                        w->ix, w->iy, w->iwidth, w->iheight, WS_VISIBLE | WS_BORDER|
						ES_AUTOHSCROLL  | WS_TABSTOP | disabled | w->wstyle, 
                        w->wstyleex,wdefaultString );
			if (wdefaultString != NULL)
				Hv_Free(wdefaultString);
			break;
		case Hv_CLOSEOUTDIALOGITEM:
			break;
		case Hv_SCROLLEDTEXTDIALOGITEM:  
			templ->cdit++;

			*item =  LISTBOX(*item, w->wcallback_id, 

                        w->ix, w->iy, w->iwidth, w->iheight, WS_VISIBLE |// WS_CLIPSIBLINGS |
						LBS_NOTIFY |  WS_BORDER | WS_VISIBLE | WS_CHILD | WS_VSCROLL | 
						LBS_HASSTRINGS | disabled 
                        | w->wstyle,w->wstyleex);
            break;

			*item =  EDITTEXT(*item,w->wcallback_id, 
                        w->ix, w->iy, w->iwidth, w->iheight,
						WS_VISIBLE | WS_BORDER   |
						LBS_HASSTRINGS   | WS_TABSTOP | disabled | w->wstyle, 
                        /*ES_AUTOHSCROLL |*/ w->wstyleex,NULL);

			break;
		case Hv_LABELDIALOGITEM:  
			templ->cdit++;
            *item = LTEXT(*item, w->wlabel, w->wcallback_id,
				               w->ix, w->iy,
				               w->iwidth, w->iheight,
							   WS_VISIBLE | disabled | SS_NOTIFY | w->wstyle,w->wstyleex);

			break;
		case Hv_FILESELECTORDIALOGITEM: //
	// NOTE: the fileselector item is kind of strange
	         // we use the standard fileselector that MICKEYSOFT provides
			// however inorder to do that we need to call OPENFILE instead of 
			// CreateDialog. Also we need to use the IDs supplied by MICKEY
			// in particular stc32 wich is defined in <DLGS.H>
			// Mickey will place all of the fileopen dialog controls in the 
			// relitive position of where stc32 is put in the template
			// Mickey will then resize the dialog (and reposition) as neccessary

	// ALSO NOTE: the dialog box itself must have certian properties set
/***********
			    WS_CHILD | WS_VISIBLE | DS_3DLOOK | DS_CONTROL | WS_CLIPSIBLINGS

				WS_CHILD is specified because without it, the call to GetOpenFileName() fails.
				COMMDLG creates the dialog specified as a child of the standard FileOpen
				common dialog box. As a result, the hDlg passed to the application's hook
				function will be the child of the standard FileOpen dialog box.
				To get a handle to the standard dialog box from the hook function,
				call GetParent (hDlg). 
				WS_CLIPSIBLINGS is specified so that overlapping controls paint properly. 
				DS_3DLOOK is a new style for Windows 95 that gives the dialog
				box a non-bold font, and gives all the controls the 3D look. 
				DS_CONTROL is another new style that among other things
				allows the user to tab between the controls of a dialog box to
				the controls of a child dialog box. As mentioned above,
				the dialog template will be created as a child of
				the standard FileOpen common dialog box,
				so specifying this style will allow tabbing from
				the application-defined controls to the standard controls. 
********/

// this is defined at the top of thte file 
// #define Hv_stc32       0x045f 
			templ->cdit++;
            *item = LTEXT(*item, w->wlabel, Hv_stc32 ,
				               w->ix, w->iy,
				               w->iwidth, w->iheight,
							   WS_VISIBLE | disabled | SS_NOTIFY | w->wstyle,w->wstyleex);


			break;
		case Hv_ACTIONAREADIALOGITEM:
			break;
		case Hv_DIALOG:   
			break;
		case Hv_THINMENUOPTION:  /* this item starts off visible */
			   templ->cdit++;
              *item =  COMBOBOX(*item,  w->wcallback_id,
				                w->ix, w->iy,
				                 w->iwidth, w->iheight +200, /* note the 200 is arbitrary*/
      		                    WS_TABSTOP  | disabled | WS_VISIBLE | CBS_DROPDOWNLIST      
								| CBS_HASSTRINGS | w->wstyle   ,w->wstyleex );

			break;
		default:
			break;
		}

	  if(w->wchildren)
		{
          processChildren(w, item, templ);
		}
	if(dialog == NULL)
		i=0;

	  }
	}
}

/**
 * Hv_W32PopupDialog
 * @purpose Windows 9X/NT specific version of Hv_PopupDialog.
 * @see The gateway routine Hv_PopupDialog. 
 */

void Hv_W32PopupDialog(Hv_Widget dialog) {
	/* traverse the dialogs children to determine the layout */
     //construct_dloglayout();
	/* now traverse again to create the layout               */
    // construct_dlog();
	/* and finally popup the dialog */

      LPDLGTEMPLATE templ;
      LPDLGITEMTEMPLATE item;
	  DWORD styles;

      int ix = 27,iy = 17; // arbitrary 
	  HINSTANCE hinst = GetWindowInstance (Hv_mainWindow) ;

      if( dialog->msdialog != NULL)  // the dialog is already poped up so don;t do it again
	  {
		  return;
	  }


	   if((int)dialog->wdata == 3030)  /* this is for the text entry widget */
	   {
		   styles = WS_POPUP   |WS_VISIBLE ;
		   ix = dialog->ix;
		   iy = dialog ->iy;
	   }
	   else
         if((int)dialog->wdata == 3031) /* file selector subclassed window */
		 {
		   styles =WS_CHILD | WS_VISIBLE | DS_3DLOOK | DS_CONTROL | WS_CLIPSIBLINGS;
		 }
		 else
	       styles = WS_POPUP   |WS_VISIBLE | WS_CAPTION |DS_3DLOOK | WS_SYSMENU;
			/*WS_VISIBLE | WS_CAPTION ;//| WS_SYSMENU */;

	   if(dialog->modal == HV_W32MODAL)
		   styles |= DS_MODALFRAME;

	   buildDialogDef(dialog);
       templ = DIALOG(&item, DLGTEMPLATE_WORKING_SIZE,
      			ix, iy,
			dialog->iwidth,  // x width
			dialog->iheight, // y height 
			styles,
			0,
			NULL,		// menu
			NULL,		// class
			dialog->wlabel,	// caption
			NULL,        //L"System",	// font
			8);	     	// font height

     if(templ == NULL)
	     return ;
     templ->style &= ~dialog->wnstyle;  /* remove unwanted sytles */
     processChildren(dialog,&item,templ);


//DumpDialog( templ );  // for test purposes

	 if(dialog->wdata ==3031) // fileselector subclass window 
	 {
		 Hv_W32ProcessFileSelect(templ);
         Hv_Free(templ);
		 return;
	 }

	 if(dialog->modal == HV_W32MODELESS) /* modless dialog box */
	 {
	    HWND hwnd;
	    Hv_OpenDialogs  dd;

	    dialog->msdialog = CreateDialogIndirectParam(hinst,templ,Hv_mainWindow,
	                       (DLGPROC) hvDlgProc,(LPARAM)dialog);
	   	dd = Hv_GetNextHwnd();

		if(dd != NULL)  /* keep track of the dialog widget with its window */
		{
			dd->dialog = dialog;
			dd->hwnd = dialog->msdialog;
		}

	 }
	 else
	 {
		long error;
		HWND hwnd;
	    Hv_OpenDialogs  dd;

	    dialog->msdialog = CreateDialogIndirectParam(hinst,templ,Hv_mainWindow,
		                                         (DLGPROC) hvDlgProc,(LPARAM)dialog);
		error = GetLastError();
		hwnd = GetParent(dialog->msdialog); /* disable the parent window */
		EnableWindow(hwnd,FALSE);           /* until the dialog box is closed*/
		dd = Hv_GetNextHwnd();

		if(dd != NULL)  /* keep track of the dialog widget with its window */
		{
			dd->dialog = dialog;
			dd->hwnd = dialog->msdialog;
		}
	 }
	 Hv_Free(templ);

}

void Hv_W32PopupThinDialog(Hv_Widget dialog,int ix,int iy)
 {

	/* traverse the dialogs children to determine the layout */
     //construct_dloglayout();
	/* now traverse again to create the layout               */
    // construct_dlog();
	/* and finally popup the dialog */

      LPDLGTEMPLATE templ;
      LPDLGITEMTEMPLATE item;
	  DWORD styles;
      long error;


	  HINSTANCE hinst = GetWindowInstance (Hv_mainWindow) ;

		   styles = WS_POPUP   |WS_VISIBLE ;
		   ix = dialog->ix;
		   iy = dialog ->iy;


	   buildDialogDef(dialog);
       templ = DIALOG(&item, DLGTEMPLATE_WORKING_SIZE,
      			ix, iy,
			dialog->iwidth,  // x width
			dialog->iheight, // y height 
			styles,
			0,
			NULL,		// menu
			NULL,		// class
			dialog->wlabel,	// caption
			L"System",	// font
			8);		// font height

     if(templ == NULL)
	     return ;
     templ->style &= ~dialog->wnstyle;  /* remove unwanted sytles */
     processChildren(dialog,&item,templ);


	 /* popups are always modeless */

	 dialog->msdialog = CreateDialogIndirectParam(hinst,templ,Hv_mainWindow,
		 (DLGPROC) hvDlgPopupProc,(LPARAM)dialog);
	 error = GetLastError();
	 Hv_Free(templ);

}

void Hv_W32PopupDialogEX(Hv_Widget dialog) {
	/* traverse the dialogs children to determine the layout */
     //construct_dloglayout();
	/* now traverse again to create the layout               */
    // construct_dlog();
	/* and finally popup the dialog */

      LPDLGTEMPLATE templ;
      LPDLGITEMTEMPLATE item;
	  DWORD styles;

      int ix = 27,iy = 17; // arbitrary 


	  HINSTANCE hinst = GetWindowInstance (Hv_mainWindow) ;

	   if(dialog->wdata == 3030)  /* this is for the text entry widget */
	   {
		   styles = WS_POPUP   |WS_VISIBLE ;
		   ix = dialog->ix;
		   iy = dialog ->iy;
	   }
	   else
	       styles = WS_POPUP   |WS_VISIBLE | WS_CAPTION |DS_3DLOOK | WS_SYSMENU;

			/*WS_VISIBLE | WS_CAPTION ;//| WS_SYSMENU */;

	   if(dialog->modal == HV_W32MODAL)
		   styles |= DS_MODALFRAME;

	   buildDialogDef(dialog);
       templ = DIALOG(&item, DLGTEMPLATE_WORKING_SIZE,
      			ix, iy,
			dialog->iwidth,  // x width
			dialog->iheight, // y height 
			styles,
			0,
			NULL,		// menu
			NULL,		// class
			dialog->wlabel,	// caption
			L"System",	// font
			8);		// font height

     if(templ == NULL)
	     return ;

     processChildren(dialog,&item,templ);


	 if(dialog->modal == 2) /* modless dialog box */
	   dialog->msdialog = CreateDialogIndirectParam(hinst,templ,Hv_mainWindow,
	                (DLGPROC) hvDlgProc,(LPARAM)dialog);
	 else
	 {
		long error;
//        dialog->msdialog = DialogBoxIndirectParam(hinst,templ,Hv_mainWindow,
			   dialog->msdialog = CreateDialogIndirectParam(hinst,templ,Hv_mainWindow,
		                                         (DLGPROC) hvDlgProc,(LPARAM)dialog);
		error = GetLastError();
	 }
	 Hv_Free(templ);

}

/**
 * Hv_W32PopdownDialog
 * @purpose Windows 9X/NT specific version of Hv_PopdownDialog.
 * @see The gateway routine Hv_PopdownDialog. 
 */


void Hv_W32PopdownDialog(Hv_Widget dialog) {
	HWND hwnd = NULL;
	int i;

	hwnd = GetParent(dialog->msdialog); /* re-enable the parent window */
	EnableWindow(hwnd,TRUE);           
	EndDialog(dialog->msdialog,1);
	dialog->msdialog = NULL;   // dialog is down so let it be popup again

	for (i = 0; i < maxOpenDialogs; i++)
	{ /* release the open window tracker */
	  if ((openDialogs[i] != NULL )&& (openDialogs[i]->dialog == dialog))
	  {
		 Hv_Free(openDialogs[i]);
         openDialogs[i] = NULL;
		 return;
	  }
	}

	// release any Vscroll map entries 
	for (i = 0; i < maxOpenVscrolls; i++)
	{ 
	  if ((openVscrolls[i] != NULL )&& (openVscrolls[i]->dialog == dialog))
	  {
		 Hv_Free(openVscrolls[i]);
         openVscrolls[i] = NULL;
		 return;
	  }
	}


}



/**
 * Hv_W32OverrideTranslations
 * @purpose Windows 9X/NT specific version of Hv_OverrideTranslations.
 * @see The gateway routine Hv_OverrideTranslations. 
 */


void Hv_W32OverrideTranslations(Hv_Widget w,
				char     *table) {
}


/*
*
*  void dialog_OnCommand (HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
*
*  hwnd            Handle of window to which this message applies
*  id              Specifies the identifier of the menu item, control, or accelerator.
*  hwndCtl         Handle of the control sending the message if the message
*                  is from a control, otherwise, this parameter is NULL. 
*
*  PURPOSE:        
*
*  COMMENTS:
*
*/

static void dialog_OnCommand (HWND hwnd,
								 int  id,
								 HWND hwndCtl,
								 UINT codeNotify) {
/*  Hv_Println("HW ID = %d %d %d", id,hwndCtl,codeNotify);*/
//  if(id==0)
//    return;
  if(id==2) /* they want to close the dialog */
    return;
  Hv_HandleDialogWidget(hwnd,id,codeNotify);
}

/**
 * Hv_GetNextHwnd
 * Get the next available opendialog structure, allocate if necessary
 */

#define HWNDNUMINC   30    //# new ones created if necessary

static  Hv_OpenDialogs  *Hv_GetNextHwnd() {

	int i;

/*
 * first see if any slots can be reclaimed. This should
 * be reimplemented as a stack
 */

    for (i = 0; i < maxOpenDialogs; i++) {
		if (openDialogs[i] == NULL) {
		     openDialogs[i] = (Hv_OpenDialogs)Hv_Malloc(sizeof(hvOpenDialog));
			 return openDialogs[i];
		}
	}

// If here, need more

    maxOpenDialogs += HWNDNUMINC;
    openDialogs = (Hv_OpenDialogs *)realloc(openDialogs,
		maxOpenDialogs * sizeof(Hv_OpenDialogs));
    for (i = maxOpenDialogs - HWNDNUMINC; i < maxOpenDialogs; i++) {
      openDialogs[i] = NULL;
    }

    return Hv_GetNextHwnd();
}

#undef HWNDNUMINC

/**
 * Hv_GetNextVscroll
 * Get the next available openvscroll structure, allocate if necessary
 */

#define HWNDNUMINC   30    //# new ones created if necessary

static  Hv_OpenVscrolls  *Hv_GetNextVscroll() {

	int i;

/*
 * first see if any slots can be reclaimed. This should
 * be reimplemented as a stack
 */

    for (i = 0; i < maxOpenVscrolls; i++) {
		if (openVscrolls[i] == NULL) {
		     openVscrolls[i] = (Hv_OpenVscrolls)Hv_Malloc(sizeof(hvVscrollWidget));
			 return openVscrolls[i];
		}
	}

// If here, need more

    maxOpenVscrolls += HWNDNUMINC;
    openVscrolls = (Hv_OpenVscrolls *)realloc(openVscrolls,
		maxOpenVscrolls * sizeof(Hv_OpenVscrolls));
    for (i = maxOpenVscrolls - HWNDNUMINC; i < maxOpenVscrolls; i++) {
      openVscrolls[i] = NULL;
    }

    return Hv_GetNextVscroll();
}

#undef HWNDNUMINC


static void getFontRatio(short font,float *x,float *y)
{
TEXTMETRIC tm;
SIZE size;
HFONT hFontOld;
float avgWidth,avgHeight;

Hv_SetMainHDC();

hFontOld = SelectObject(Hv_gc,Hv_fonts[font]);
 GetTextMetrics(Hv_gc,&tm);
 GetTextExtentPoint32(Hv_gc,"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",52,&size);
 avgWidth = (size.cx/26+1)/2;
 avgHeight = (WORD)tm.tmHeight; 
 *x = (avgWidth)/SYSFONT_WIDTH;
 *y = (avgHeight)/SYSFONT_HEIGHT;
}

/**
 * Hv_HandleDialogWidget
 * @param win32Id   The unique internal Id
 * @return  True if sucessfully dispatched
 */

static Boolean Hv_HandleDialogWidget(HWND hwnd,int win32Id,UINT codeNotify) {

	Hv_CallbackData  cd;
//  Hv_Println("Hv ID = %d %d %d", win32Id,hwnd,codeNotify);

	if ((win32Id < 0) || (win32Id >= maxCallbacks) || (callbacks == NULL))
		return False;

	cd = callbacks[win32Id];
	if ((cd == NULL) || (cd->callback == NULL))
		return False;

	cd->callback(cd->w, cd->userId,hwnd,codeNotify);
	return True;
}

#endif








