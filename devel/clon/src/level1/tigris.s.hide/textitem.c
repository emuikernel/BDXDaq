
#ifndef VXWORKS

/* textitem.c */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <Xm/ScrolledW.h>
#include "Hv.h"
#include "ced.h"
#include "tigris.h"

extern char *SECommentText[6][12]; /* defined in setup.c */
extern TriggerInfo Jti;            /* defined in setup.c */

/* local variables */

static Widget wScroll = NULL;
static Widget wText = NULL;

/* local prototypes */

static void doj_return(Widget tw, XtPointer unused, XmTextVerifyCallbackStruct *cbs);
static void doj_mouseDestroy();

/* --------------------------------------------------- */

void
JAM_FieldSingleClick(Hv_Event hevent)
{
  char *t;

  ViewData vdata = (ViewData)hevent->view->data ;

  vdata->activeTextItem = hevent->item;
  vdata->OldCommentIndex = hevent->item->user1+vdata->ButState6_12 ;
  Hv_SetItemText (vdata->evcommentbox, 
		  SECommentText[vdata->sectorNumber]
		  [vdata->OldCommentIndex] );
  
  Hv_DrawItem (vdata->evcommentbox, NULL);
  
  Hv_TextEntrySingleClick (hevent) ;
}

/* ----------------------------------------------------
 *  Create ONE motif text widget that will be shared by
 *  all the text widgets in this app.
 *
 *  having one should pose no problem as only one 
 *  "text entry/edit" field should be active at a time.
 *
 *  Notice that this is chained to the Hv_init routine
 *  to initially setup the Hv_item.
 *
 */

void
JAM_TextInit(Hv_Item Item, Hv_AttributeArray attributes)
{
  Hv_Rect area;

  Hv_CopyRect (&area, Hv_GetItemArea (Item)) ;

  Hv_TextItemInitialize (Item, attributes) ;

  if (wScroll == NULL) {
    printf ("hello from text init ... %d %d\n", area.left, area.top);
    wScroll = XtVaCreateManagedWidget ("scroll", xmScrolledWindowWidgetClass, Hv_canvas, 
				XmNx,                 area.left,
				XmNy,                 area.top, 0);
    
    wText = XtVaCreateManagedWidget ("text", xmTextWidgetClass, wScroll,
			      XmNrows,              18,
			      XmNcolumns,           60,
			      XmNeditMode,          XmMULTI_LINE_EDIT,
			      XmNwordWrap,          True, 0);
    XtUnmanageChild (wScroll) ;
  }

}

/* ---------------------------------------------------- */

void
JAM_CreateMTextEdit(Hv_Event hevent)
{
  Hv_Rect area;
  Arg args[8] ;
  int n=0, i, j, l, count=0;
  char *t, *nt ;
  
  Hv_activeTextItem = hevent->item ;

  /* call the dangling text item callback
   * to clean up this motif text widget.
   *
   * 1. set call back pointers and gather info.
   * 2. get existing text from hv text item
   * 3. edit for "Hv chars"
   * 4. put clean text into Xm text widget and map it
   *    into view.
   */

  Hv_DanglingTextItemCallback = doj_mouseDestroy ;
  Hv_CopyRect (&area, Hv_GetItemArea (hevent->item)) ;
  t = Hv_GetItemText (hevent->item) ;
  /* if I get any text use it. BUT if not fall thru */
  /* to Hv_sendview to front... */
  j=0;
  if (t != 0) {
    l = strlen (t) ;
    printf ("create: length of string = %d\n", l) ;
    for (i=0, j=0; i<l ; i++) {
      if (t[i] == '\\')
	j++ ;
    }
    nt = XtMalloc (l - j) ;
    for (i=0, j=0; i<l; i++) {
      if (t[i] == '\\') {
	nt[j++] = '\n' ;
	i+=1 ;
	continue ;
      } else
	nt[j++] = t[i] ;
    }
    nt[j] = 0x00 ;
  } else
    nt = 0x00 ;
  
  Hv_SendViewToFront (hevent->view) ;
  
  XmTextSetString (wText, nt) ;
  
  XtVaSetValues(wScroll,
		XmNx, area.left+1,
		XmNy, area.top+1,
		NULL);
  
  XtMapWidget (wScroll);
  XtFree (nt) ;
}

/* ----------------------------------------------------
 *  Should ONLY be used for the motif editor widget.
 *
 *  The code assumes that the hv_widget is single line.
 *
 *  Basically, clean up the screen by removing the motif 
 *  widget; get the text out of it and pop it into the
 *  Hv_text item.
 *
 *  driven on return key press.
 *
 *
 *  JAM 7/18/1996 - this never seems to get called. 
 *                  I'll leave it in.
 */

void
doj_return(Widget tw, XtPointer unused, XmTextVerifyCallbackStruct *cbs) 
{
  char *t;
  
  Hv_DanglingTextItemCallback = NULL ;
  t = XmTextGetString (tw) ;
  
  Hv_SetItemText (Hv_activeTextItem, t);
  XtFree (t) ;
  XtUnmanageChild (wScroll) ;
  Hv_activeTextItem = NULL ;
  printf ("return key hit...\n");
}

/* ----------------------------------------------------
 *  clean up; remove the motif widget get the text out
 *  of it and pop it into the Hv_text item.
 *
 *  handles the multi-line setup for Hv_text gadget.
 *
 *  driven on a mouse click outside of the motif widget.
 */

void
doj_mouseDestroy() 
{
  char *t, *nt;
  static char smallpatch[5];
  int i, j=0, l ;
  
  ViewData vdata;

  /*
   * 1. undo the pointer stuff started above.
   * 2. get Xm text and edit to put "Hv chars" in.
   * 3. leave text in the Hv text item.
   * 4. remove motif widget from view.
   */
  
  Hv_DanglingTextItemCallback = NULL ;
  t = XmTextGetString (wText) ;
  j=0;
  nt = smallpatch;
  if (t != 0) {
    l = strlen (t) ;
  
    for (i=0, j=0; i<l ; i++)
      if (t[i] == '\012') {
	j++ ;
      }
    printf ("found %d eol's\n", j) ;
    nt = XtMalloc (l+j+1) ;
    for (i=0, j=0; i<l; i++) {
      if (t[i] == '\012') {
        nt[j++] = '\\' ; 
        nt[j++] = 'n' ;
      } else
        nt [j++] = t[i] ;
    }
  }

  nt[j] = 0x00 ;
  Hv_SetItemText (Hv_activeTextItem, nt);
  vdata = Hv_activeTextItem->view->data;
     
  free (SECommentText[vdata->sectorNumber]
      [vdata->OldCommentIndex]);
 
  if (t == 0)
    SECommentText[vdata->sectorNumber]
      [vdata->OldCommentIndex] = 0;
  else
    SECommentText[vdata->sectorNumber]
      [vdata->OldCommentIndex] = nt;

  XtFree (t) ;
  XtUnmapWidget (wScroll) ;

  Hv_activeTextItem = NULL ;
  printf ("mouse press...\n");
}

/* ---------------------------------------------------- */

void
TRIG_SingleClick(Hv_Event hevent)
{
  switch (hevent->item->user1) {
  case 0:
    Jti.butSet1 = 0;
    break;
  case 1:
    Jti.butSet1 = 1;
    break;
  case 2:
    Jti.butSet2 = 0;
    break;
  case 3:
    Jti.butSet2 = 1;
    break;
  default:
    printf ("*********** strange ***** [%d]\n", hevent->item->user1);
    break;
  }
  Hv_TextEntrySingleClick (hevent) ;
  return;
}

#else /* UNIX only */

void
textitem_dummy()
{
}

#endif
