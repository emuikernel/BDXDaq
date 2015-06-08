h15784
s 00000/00000/00000
d R 1.2 02/08/25 23:21:23 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_textentryitem.c
e
s 00279/00000/00000
d D 1.1 02/08/25 23:21:22 boiarino 1 0
c date and time created 02/08/25 23:21:22 by boiarino
e
u
U
f e 0
t
T
I 1
/*
-----------------------------------------------------------------------
File:     Hv_textentry.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

/* This file contains functions dealing with slider items */

#include "Hv.h"	  /* contains all necessary include files */

static  XmFontList       fontlist;
static  XFontStruct      *xfont;

static void cleanup(void);

static void do_return (Widget   tw,
		       XtPointer  unused,
		       XmTextVerifyCallbackStruct *cbs);

static void do_mouseDestroy (void);


extern  char *Hv_xFontNames[];

static Widget themtw;

/* ------------- Hv_DrawTextEntryItem ------------*/

void Hv_DrawTextEntryItem(Hv_Item    Item,
			  Hv_Region   region)

{
  Hv_DrawTextItem(Item, region);
}

/*-------- Hv_TextEntryItemInitialize --------*/

void Hv_TextEntryItemInitialize(Hv_Item           Item,
				Hv_AttributeArray attributes)

{
  int         editmode;
  Hv_Widget   w;

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
  
  if (attributes[Hv_FILLCOLOR].s != Hv_DEFAULT)
    Item->scheme->fillcolor = attributes[Hv_FILLCOLOR].s;

/* allocate the tag along motif widget */

  editmode = XmSINGLE_LINE_EDIT; 
  if (attributes[Hv_TEXTENTRYMODE].s == Hv_MULTILINEEDIT)
    editmode = XmMULTI_LINE_EDIT; 

  w = XtVaCreateWidget("textfield", 
		       xmTextWidgetClass,      Hv_toplevel,
		       XmNshadowThickness,     0,
		       XmNwidth,               Item->area->width - 2,
		       XmNheight,              Item->area->height - 2,
		       XmNhighlightThickness,  0,
		       XmNeditMode,            editmode,
		       NULL) ;

  Item->data = (void *)w;
  
/* value changed callback ? If so
   add one with the Hv_Item as the data*/


  if ((XtCallbackProc)(attributes[Hv_VALUECHANGED].fp) != NULL)
    XtAddCallback(w,
		  XmNvalueChangedCallback,
		  (XtCallbackProc)(attributes[Hv_VALUECHANGED].fp),
		  (XtPointer)Item);


/******************************************************
  The next two translation overrides were added 5/26/96
  to try to fix the wrongway delete on all platforms
*******************************************************/

  XtOverrideTranslations(w, XtParseTranslationTable(transtable));
  XtOverrideTranslations(w, XtParseTranslationTable(trans2table));

  Item->standarddraw = Hv_DrawTextEntryItem;
  Hv_DefaultSchemeIn(&(Item->scheme));

  if (attributes[Hv_FILLCOLOR].s != Hv_DEFAULT)
    Item->scheme->fillcolor = attributes[Hv_FILLCOLOR].s;
  else
    Item->scheme->fillcolor -= 1;
}

/* ------ Hv_TextEntrySingleClick -------  */

void Hv_TextEntrySingleClick(Hv_Event hevent)

{
  Hv_Item   Item = hevent->item;
  Hv_Rect area;
  char *t ;
  Position   cx, cy, left, top;
  XmTextPosition   textpos;

  if (Item->str == NULL)
    return;

  themtw = (Widget)(Item->data);

/* if text is NULL go ahead an give them a string */

  if (Item->str->text == NULL) {
    Hv_InitCharStr(&(Item->str->text), " ");
    *(Item->str->text) = '\0';
  }

  Hv_activeTextItem = Item ;
  Hv_DanglingTextItemCallback = do_mouseDestroy ;
  Hv_CopyRect (&area, Hv_GetItemArea (Item)) ;
  t = Hv_GetItemText (Item) ;

  left = area.left+2;
  top  = area.top+4; 


  xfont  = XLoadQueryFont(Hv_display,  Hv_xFontNames[Item->str->font]);
  fontlist = XmFontListCreate(xfont, Hv_charSet[Item->str->font]);

  XtVaSetValues (themtw, XmNvalue, (String)t,
		 XmNfontList, fontlist,
		 XmNbackground, Hv_colors[Item->scheme->fillcolor],
		 XmNforeground, Hv_colors[Item->scheme->darktrimcolor],
/*		 XmNeditMode, XmSINGLE_LINE_EDIT, */
		 NULL);

  XtVaSetValues(themtw,
		XmNx, left-1,
		XmNy, top-1,
		NULL);
		
/* try to get the cursor in the proper place */

  cx = (Position)(hevent->where.x - left);
  cy = (Position)(hevent->where.y - top);
  textpos = XmTextXYToPos(themtw, cx, cy);

  XtAddCallback (themtw,
		 XmNactivateCallback, (XtCallbackProc)do_return,
		 NULL) ;

  XmTextSetInsertionPosition(themtw, textpos);
  XtManageChild(themtw);
}

/*--------- cleanup --------*/

static void cleanup(void)

{
  Hv_DanglingTextItemCallback = NULL ;
  XtUnmanageChild (themtw) ;
  XmFontListFree(fontlist);

/* next fix submitted by Anton Mezger from PSI */

  XtRemoveAllCallbacks(themtw, XmNactivateCallback);
}


/* ----------------------------------------------------
 *  Should ONLY be used for the motif editor widget.
 *
 *  Basically, clean up the screen by removing the motif 
 *  widget; get the text out of it and pop it into the
 *  Hv_text item.
 *
 *  driven on return key press.
 */

/*-------- do_return -----------*/

static void do_return (Widget   tw,
		       XtPointer  unused,
		       XmTextVerifyCallbackStruct *cbs)

{
  char *t;

  t = XmTextGetString (tw) ;
  Hv_SetItemText (Hv_activeTextItem, t);
  XtFree (t) ;
  cleanup();

/* next addtion submitted by Anton Mezger from PSI:
   a user routine that can be called here */

  if (Hv_AfterTextEdit != NULL)
    Hv_AfterTextEdit(Hv_activeTextItem);

}

/* ----------------------------------------------------
 *  clean up; remove the motif widget get the text out
 *  of it and pop it into the Hv_text item.
 *
 *  driven on a mouse click outside of the motif widget.
 */

/*-------- do_MouseDestroy -------*/

static void do_mouseDestroy (void) 

{
  char *t, *nt;
  int i, j=0, len ;
  
  t = XmTextGetString (themtw) ;
  len = strlen (t) ;
  for (i=0, j=0; i<len ; i++)
    if (t[i] == '\012') {
      j++ ;
    }
  nt = XtMalloc (len+j+1) ;
  for (i=0, j=0; i<len; i++) {
    if (t[i] == '\012') {
      nt[j++] = '\\' ; 
      nt[j++] = 'n' ;
    } else
      nt [j++] = t[i] ;
  }
  nt[j] = 0x00 ;
  Hv_SetItemText (Hv_activeTextItem, nt);
  XtFree (t) ;
  XtFree (nt) ;
  
  cleanup();

/* next addtion submitted by Anton Mezger from PSI:
   a user routine that can be called here */

  if (Hv_AfterTextEdit != NULL)
    Hv_AfterTextEdit(Hv_activeTextItem);
}










E 1
