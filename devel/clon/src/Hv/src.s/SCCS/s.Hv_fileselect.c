h10771
s 00000/00000/00000
d R 1.2 02/09/09 11:30:26 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/s/Hv_fileselect.c
e
s 00274/00000/00000
d D 1.1 02/09/09 11:30:25 boiarino 1 0
c date and time created 02/09/09 11:30:25 by boiarino
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
#include <Xm/FileSB.h>

/*-------- local prototypes -------*/


static void     Hv_FileSelectResponse(Widget,
				      XtPointer,
				      XmFileSelectionBoxCallbackStruct *);

static char    *Hv_GenericFileSelect1(char *,
				     char *,
				     unsigned char,
				     char *);

static char    *Hv_TextFromXmString(XmString);

/*------ local variables ------*/

char     *generic_filename;

/* ----------- Hv_FileSelect -------------*/

char *Hv_FileSelect(char *prompt,
		    char *mask,
		    char *deftext)

{
  return Hv_GenericFileSelect1(prompt, mask, XmFILE_REGULAR, deftext);
}

/* ----------- Hv_DirectorySelect -------------*/

char *Hv_DirectorySelect(char *prompt,
			 char *mask)

{
  return Hv_GenericFileSelect1(prompt, mask, XmFILE_DIRECTORY, NULL);
}


/* ----------- Hv_GenericFileSelect1 -------------*/

static char *Hv_GenericFileSelect1(char *prompt,
				  char *mask,
				  unsigned char filemask,
				  char *deftext)

/* A generic file selection dlog -- returns
   the name of the file selected or NULL
   if cancelled

   deftext: default selection */

{
  Widget   dialog = NULL;
  char            *fname = NULL;
  XmString         xmst, title;
  XmString         defstr = NULL;
  Hv_Widget        rowcol = NULL;
  Hv_Widget        label = NULL;
  Hv_Widget        sep1 = NULL;
  Hv_Widget        sep2 = NULL;
  Hv_Widget        fileList = NULL;
  static Boolean   fsisup = False;
  XmString         dirxmst;
  char            *dtextpdir;

  if (fsisup) {
    Hv_SysBeep();
    return NULL;
  }
  
  fsisup = True;

  dialog = XmCreateFileSelectionDialog(Hv_toplevel, "filesb", NULL, 0); 
  title =  XmStringCreateSimple("file selection");
  
  
  XtVaSetValues(dialog,
		XmNdialogTitle,  title,
		NULL);
  
  XmStringFree(title);      
  
  XtAddCallback(dialog,
		XmNokCallback, (XtCallbackProc)Hv_FileSelectResponse,
		NULL);
  
  XtAddCallback(dialog,
		XmNcancelCallback, (XtCallbackProc)Hv_FileSelectResponse,
		NULL);


/* the dialog has been created. set the mask.
   If a prompt was provide, create some additional widgets*/

  if (mask)
    xmst =  XmStringCreateSimple(mask);
  else
    xmst =  XmStringCreateSimple("*");



  XtVaSetValues(dialog,
		XmNfileTypeMask, filemask,
		XmNpattern,      xmst,      /* thanks Jack */
		NULL);  
  XmStringFree(xmst);

/* we have cached the directory */

  if (Hv_cachedDirectory != NULL) {
    xmst =  XmStringCreateSimple(Hv_cachedDirectory);
    XtVaSetValues(dialog, XmNdirectory, xmst, NULL);
    XmStringFree(xmst);
  }

  if (deftext != NULL) {

/* prepend directory? */

    if ((Hv_cachedDirectory != NULL) && (deftext[0] != '/')) {
      dtextpdir = (char *)Hv_Malloc(strlen(Hv_cachedDirectory) + strlen(deftext) + 5);
      strcpy(dtextpdir, Hv_cachedDirectory);
      strcat(dtextpdir, deftext);
    }
    else
      Hv_InitCharStr(&dtextpdir, deftext);

    defstr = XmStringCreateSimple(dtextpdir);
    XtVaSetValues(dialog,
		  XmNtextString,   defstr,
		  NULL);  
    XmStringFree(defstr);
    Hv_Free(dtextpdir);
  }


/* set up the user provided prompt */

  if (prompt) {
    rowcol = Hv_VaCreateDialogItem(dialog,
				   Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
				   Hv_ORIENTATION,  Hv_VERTICAL,
				   Hv_ALIGNMENT,    Hv_CENTER,
				   NULL);

    sep1 = Hv_SimpleDialogSeparator(rowcol);

    label = Hv_VaCreateDialogItem(rowcol,
				  Hv_TYPE,     Hv_LABELDIALOGITEM,
				  Hv_LABEL,    prompt,
				  Hv_FONT,     Hv_helvetica14,
				  NULL);

    sep2 = Hv_SimpleDialogSeparator(rowcol);
  }


/* deselect all files */

  fileList = XmFileSelectionBoxGetChild(dialog, XmDIALOG_LIST);
  XmListDeselectAllItems(fileList);

  if (Hv_DoDialog(dialog, NULL) == Hv_OK) {
    Hv_InitCharStr(&fname, generic_filename);
    Hv_Free(generic_filename);
    generic_filename = NULL;
  }

/* destroy the widgets that were created due to a prompt*/

  if (prompt) {
    XtDestroyWidget(label);
    XtDestroyWidget(sep1);
    XtDestroyWidget(sep2);
    XtDestroyWidget(rowcol);
  }


  Hv_Free(Hv_cachedDirectory);
  XtVaGetValues(dialog, XmNdirectory, &dirxmst, NULL);
  Hv_cachedDirectory = Hv_TextFromXmString(dirxmst);

  XmStringFree(dirxmst);

  fsisup = False;
  XtDestroyWidget(dialog);
  return fname;
}


/* ------ Hv_FileSelectResponse ----- */

static void Hv_FileSelectResponse(Widget w,
				  XtPointer cdata,
				  XmFileSelectionBoxCallbackStruct *cbs)

{
  XmStringContext   context;      /* string context */
  XmStringCharSet   charset;      /* string charset */
  char              *text;        /* text of file name selected */
  XmStringDirection direction;    /* direction of string (LtoR or RtoL) */
  Boolean           separator;    /* is it a separator */

  Hv_Widget         dialog = Hv_GetOwnerDialog(w);
  Hv_DialogData     ddata;

  ddata = Hv_GetDialogData(dialog);
  if(ddata == NULL) {
    fprintf(stderr, "Error in Hv_FileSelectResponse\n");
    return;
  }

  switch (cbs->reason) {
  case XmCR_OK:
    ddata->answer = Hv_OK;
    if (generic_filename)
      Hv_Free(generic_filename);
    
    XmStringInitContext    (&context, cbs->value);  /* get the context */
    XmStringGetNextSegment (context, &text, &charset, &direction, &separator);  /* get the text */
    
    XmStringFreeContext(context);
    Hv_InitCharStr(&generic_filename, text);
    free(text);
    break;
    
  case XmCR_CANCEL:
    ddata->answer = Hv_CANCEL;
    break;
  }
}

/*--------- Hv_TextFromXmString --------*/

static char *Hv_TextFromXmString(XmString xmst)

{
  XmStringContext   context;      /* string context */
  XmStringCharSet   charset;      /* string charset */
  char              *text, *tstr;        /* text of file name selected */
  XmStringDirection direction;    /* direction of string (LtoR or RtoL) */
  Boolean           separator;    /* is it a separator */

  if (xmst == NULL)
      return NULL;   /* thanks Jack */

  XmStringInitContext(&context, xmst);  /* get the context */
  XmStringGetNextSegment(context, &tstr, &charset, &direction, &separator);  /* get the text */
  XmStringFreeContext(context);

  Hv_InitCharStr(&text, tstr);
  free(tstr);
  return text;
}





E 1
