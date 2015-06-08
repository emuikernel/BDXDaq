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

#define Hv_MBSEPGAP   8
#define Hv_MBMARGIN   3
#define Hv_MBEXTRA    10
#define Hv_MBPAD      6
#define Hv_MBACTIVEWIDTH 7
#define Hv_MBBUTACTRECTGAP 2

/*****************************************************
  This file contains routines for the on-view Hv
  (NOT Motif) menu buttons. If they are successful,
  we may eventually REPLACE the Motif main menu
  alltogether, but that's a ways off.
******************************************************/


/*---------- local prototypes --------------*/

static void Hv_IconDraw(Hv_Item   Item,
			Hv_Region region,
			short     choice,
			Hv_Rect   *rect);

static void Hv_GetIconDrawingRect(Hv_Item Item,
				  short   choice,
				  Hv_Rect *irect);

static short Hv_PresentChoice(Hv_Item  Item,
			      Hv_Point pp,
			      Hv_Rect  *rect);

static Hv_MenuButtonData  Hv_MallocMenuButtonData(short style);

static void Hv_SetMenuButtonDisplayRect(Hv_Item Item,
					 Hv_Point pp);

static void Hv_DisplayChoices(Hv_Item Item);

static void Hv_GetActiveRect(Hv_Item Item,
			     Hv_Rect *rect);

static void Hv_HandleMenuButton(Hv_Event hvevent);


/*------- Hv_RemoveMenuButtonEntry ----------*/

void Hv_RemoveMenuButtonEntry(Hv_Item  Item,
			      int      pos)

/*******************************************
  Removes entry at pos "pos" which is NOT a C Index.
  Valid only for text based menus. If pos <= 0,
  all entries are removed
***********************************/

{
  Hv_MenuButtonData   mbd;
  int i;

  mbd = Hv_GetMenuButtonData(Item);
  
 if (mbd->style != Hv_TEXTBASED)
   return;

 if (pos > mbd->numrow)
   return;

/* remove all? */

 if (pos <= 0) {
   for (i = 0; i < mbd->numrow; i++) {
     Hv_DestroyString(mbd->entries[i]);
     mbd->entries[i] = NULL;
   }
   mbd->numrow = 0;
   return;
 }
 
/* remove a specific entry */

 pos--;   /* convert to C index */
 mbd->numrow -= 1;
 for (i = pos; i < mbd->numrow; i++) {
   mbd->estyles[i] = mbd->estyles[i+1];
   Hv_DestroyString(mbd->entries[i]);
   Hv_CopyString(&(mbd->entries[i]), mbd->entries[i+1]);
 }

/* now get rid of the last one */

 Hv_DestroyString(mbd->entries[mbd->numrow]);
 mbd->entries[mbd->numrow] = NULL;
}

/*------- Hv_AddMenuButtonEntry ------------*/


void Hv_AddMenuButtonEntry(Hv_Item   Item,
			   char     *label,
			   short     font,
			   short     color,
			   short     style)

/*******************************************
  Adds entry entry at end.
  Valid only for text based menus
***********************************/

{
  Hv_MenuButtonData   mbd;

  mbd = Hv_GetMenuButtonData(Item);
  
 if (mbd->style != Hv_TEXTBASED)
   return;

 if (label != NULL) {
   mbd->entries[mbd->numrow] = Hv_CreateString(label);
   mbd->entries[mbd->numrow]->font = font;
   mbd->entries[mbd->numrow]->strcol = color;
 }

 mbd->estyles[mbd->numrow] = style;
 mbd->numrow += 1;
  
}

/*------- Hv_GetCurrentMenuButtonChoice ---------*/

short           Hv_GetCurrentMenuButtonChoice(Hv_Item Item)

/***********************************************
  Convenience routine for returning current choice
************************************************/

{
  Hv_MenuButtonData   mbd;

  mbd = Hv_GetMenuButtonData(Item);
  return mbd->current;
}

/*------- Hv_SetCurrentMenuButtonChoice ---------*/

void            Hv_SetCurrentMenuButtonChoice(Hv_Item Item,
					      short choice)

/***********************************************
  Sets the choice as indicated AND redraws the
  menu button. 
************************************************/

{
  Hv_MenuButtonData   mbd;

  mbd = Hv_GetMenuButtonData(Item);
  mbd->current = choice;
  Hv_DrawItem(Item, NULL);
}


/*------- Hv_GetMenuButtonData ---------*/

Hv_MenuButtonData      Hv_GetMenuButtonData(Hv_Item Item)

/***********************************************
  Convenience routine for getting the menubutton
  data structure
************************************************/

{
  return (Hv_MenuButtonData)(Item->data);
}

/*------- Hv_MenuButtonInitialize ---------*/

void            Hv_MenuButtonInitialize(Hv_Item Item,
					Hv_AttributeArray attributes)

/***********************************************
  Uses the attributes to initialize the
  Hv_MENUBUTTONITEM.
************************************************/

{
  short              style;
  Hv_MenuButtonData  mbd;
  int                i;

/* see which style was requested and allocate
   the data */

  style = attributes[Hv_MENUBUTTONSTYLE].s;
  mbd = Hv_MallocMenuButtonData(style);
  Item->data = (void *)mbd;
  Item->state = Hv_OFF;
  mbd->IconDraw = (Hv_FunctionPtr)(attributes[Hv_ICONDRAW].fp);
  mbd->hasactivearea = (attributes[Hv_MBNOACTIVEAREA].i == 0);
  mbd->tandem = (attributes[Hv_MBTANDEM].i != 0);

/* set the standard draw */

  Item->standarddraw = Hv_DrawMenuButtonItem;

/* set the current choice */

  if (style == Hv_TEXTBASED)
    mbd->current = Hv_sMax(1, attributes[Hv_CURRENTCHOICE].s);
  else if (style == Hv_ICONBASED)
    mbd->current = Hv_sMax(257, attributes[Hv_CURRENTCHOICE].s);
  
/* set the fillcolor */

  if (attributes[Hv_FILLCOLOR].s == Hv_DEFAULT)
    Item->color = Hv_gray12;
  else
    Item->color = attributes[Hv_FILLCOLOR].s;

/* set the arm color */

  if (attributes[Hv_ARMCOLOR].s == Hv_DEFAULT)
    mbd->armcolor = Hv_gray14;
  else
    mbd->armcolor = attributes[Hv_ARMCOLOR].s;

/* background color of present choice */

  mbd->ccolor = attributes[Hv_MBCHOICECOLOR].s;

/* set the text and icon background color */

  if (attributes[Hv_MBCOLOR].s == Hv_DEFAULT)
    mbd->fcolor = Hv_aliceBlue;
  else
    mbd->fcolor = attributes[Hv_MBCOLOR].s;

/* the single click is always Hv_HandleMenuButton.
   What was passed in Hv_CALLBACK becomes the
   choice callback */

  Item->singleclick = Hv_HandleMenuButton;
  mbd->AlternateCB = (Hv_FunctionPtr)(attributes[Hv_CALLBACK].fp);
  mbd->ChoiceCB = (Hv_FunctionPtr)(attributes[Hv_MENUCHOICECB].fp);

  mbd->iconwidth  = attributes[Hv_ICONWIDTH].s;
  mbd->iconheight = attributes[Hv_ICONHEIGHT].s;

/* some inits are style based. Note the width and
  height min reflect the need for enough space to
  differentiate this button from a noremal button */

  switch (style) {
  case Hv_TEXTBASED:
    Item->area->width  = Hv_sMax(15, Item->area->width);
    Item->area->height = Hv_sMax(15, Item->area->height);

/******* add all the entries ***********/

    mbd->numrow = Hv_vaNumOpt;
    mbd->numcol = 1;

    for (i = 0; i < Hv_vaNumOpt; i++) {
      mbd->estyles[i] = Hv_vaTags[i];

      if (Hv_vaTags[i] != Hv_SEPARATORTAG) {
	mbd->entries[i] = Hv_CreateString(Hv_vaText[i]);
	mbd->entries[i]->font = Hv_vaFonts[i];
	mbd->entries[i]->strcol = Hv_vaColors[i];
      }
    }
    
    break;

    case Hv_ICONBASED:
    mbd->numrow = attributes[Hv_NUMROWS].s;
    mbd->numcol = attributes[Hv_NUMCOLUMNS].s;
    
    if (mbd->hasactivearea)
      Item->area->width  = Hv_sMax(20,
	          (short)(attributes[Hv_MBICONWIDTH].s +
		      Hv_MBACTIVEWIDTH +
			  Hv_MBBUTACTRECTGAP +
			  3));
    else
      Item->area->width  = Hv_sMax(20, (short)(attributes[Hv_MBICONWIDTH].s + 3));
    
    Item->area->height = Hv_sMax(20, (short)(attributes[Hv_MBICONHEIGHT].s + 4));
    break;

  }


  Hv_FixRectRB(Item->area);
}

/**
 * Hv_DestroyMenuButtonItem
 * @purpose Called when an Hv_MENUBUTTONITEM is deleted. As
 * usual, it must only worry about the data field.
 * Everthing else is handled automatically.
 * @param   The Item to destroy.
 */

 void Hv_DestroyMenuButtonItem(Hv_Item Item) {


  Hv_MenuButtonData   mbd;
  int                 i;

  mbd = Hv_GetMenuButtonData(Item);

/* free any auxillary memory allocated for text based */

  Hv_DestroyString(mbd->title);
  if (mbd->entries != NULL)
    for (i = 0; i < Hv_MAXCHOICE; i++)
      Hv_DestroyString(mbd->entries[i]);
  Hv_Free(mbd->estyles);

/* can now free the data record itself */

  Hv_Free(mbd);
  Item->data = NULL;
}

/*------- Hv_DrawMenuButtonItem ---------*/

void  Hv_DrawMenuButtonItem(Hv_Item Item,
			    Hv_Region region)

/***********************************************
  Basic drawing routine for menu button items.
  It will have to decide at the beginning whether
  this is text or icon based.
************************************************/

{
  Hv_MenuButtonData    mbd;
  Hv_Rect              rect, rect2;
  short                xc, yc, sh, sw;
  Hv_String            *str;

  mbd = Hv_GetMenuButtonData(Item);


/* always the right part of area is marked to
   distinguish from a normal button */

  if (mbd->hasactivearea) {
    Hv_GetActiveRect(Item, &rect);
    Hv_GetRectCenter(&rect, &xc, &yc);
  }

  if (Item->state == Hv_OFF) {
    Hv_Simple3DRect(Item->area, True, Item->color);

    if (mbd->hasactivearea) {
      Hv_Simple3DRect(&rect, False, Item->color);
      Hv_SetRect(&rect2, (short)(xc-1), (short)(yc-5), 3, 10);
      Hv_Simple3DRect(&rect2, True, Item->color);
    }

/* fill the background */

    Hv_CopyRect(&rect2, Item->area);
    Hv_ShrinkRect(&rect2, 2, 2);
    /*    rect2.bottom -= 1; */

    if (mbd->hasactivearea) 
      rect2.right = rect.left - Hv_MBBUTACTRECTGAP;

    Hv_FixRectWH(&rect2);
    Hv_Simple3DRect(&rect2, False, mbd->fcolor);
  }
  else {
    Hv_Simple3DRect(Item->area, False, -1);

    if (mbd->hasactivearea) {
      Hv_Simple3DRect(&rect, True, -1);
      Hv_SetRect(&rect, (short)(xc-2), (short)(yc-5), 4, 10);
      Hv_Simple3DRect(&rect, False, -1);
    }
    return;
  }

  switch (mbd->style) {
  case Hv_TEXTBASED:
    if (mbd->estyles[mbd->current -1] != Hv_SEPARATORTAG) {
      str = mbd->entries[mbd->current -1];
      Hv_GetRectCenter(&rect2, &xc, &yc);
      Hv_CompoundStringDimension(str, &sw, &sh);
      Hv_DrawCompoundString((short)(xc - sw/2), (short)(yc - sh/2), str);
    }

    break;

  case Hv_ICONBASED:
    if (mbd->IconDraw != NULL) {
      Hv_GetIconDrawingRect(Item, 0, &rect2);
      Hv_IconDraw(Item, region, mbd->current, &rect2);
    }
    break;

  }
}

/*------ Hv_IconDraw -------*/

static void Hv_IconDraw(Hv_Item   Item,
			Hv_Region region,
			short     choice,
			Hv_Rect   *rect)

{
  Hv_MenuButtonData        mbd = Hv_GetMenuButtonData(Item);

  if (mbd->IconDraw == NULL)
    return;

  if (Hv_inPostscriptMode)
    Hv_PSComment("ICON button");
  Hv_ClipRect(rect);
  mbd->IconDraw(Item, choice, rect);
  Hv_RestoreClipRegion(region);
}


/*-------- Hv_GetIconDrawingRect ------*/

static void Hv_GetIconDrawingRect(Hv_Item Item,
				  short   choice,
				  Hv_Rect *irect)

/***********************************************
  Gets the rect that the icon should be confined
  to. If the choice is 0, gets the rect ON THE
  BUTTON that the current choice should be drawn
  to.
************************************************/

{
  Hv_MenuButtonData      mbd = Hv_GetMenuButtonData(Item);
  Hv_Rect   rect;

  if (choice <= 0) {
    Hv_CopyRect(irect, Item->area);
    Hv_ShrinkRect(irect, 3, 3);

    if (mbd->hasactivearea) {
      Hv_GetActiveRect(Item, &rect);
      irect->right = rect.left - (Hv_MBBUTACTRECTGAP - 3);
      Hv_FixRectWH(irect);
    }
    else {
      irect->right += 1;
      irect->bottom += 1;
      Hv_FixRectWH(irect);
    }

    return;
  }

  Hv_GetMenuButtonChoiceRect(Item, choice, irect);
  Hv_ShrinkRect(irect, 1, 1);
}

/*-------- Hv_GetActiveRect -------*/

static void Hv_GetActiveRect(Hv_Item Item,
			     Hv_Rect *rect)

/***********************************************
  Gets that special littile rect that activates
  the menu for iconbased (so that the main part
  of the button can activate something else)

************************************************/

{
  Hv_MenuButtonData     mbd;

  mbd = Hv_GetMenuButtonData(Item);

  Hv_CopyRect(rect, Item->area);
  Hv_ShrinkRect(rect, 2, 2);
  rect->left = rect->right - Hv_MBACTIVEWIDTH;
  Hv_FixRectWH(rect);
}



/*-------- Hv_MallocMenuButtonData ------*/

static Hv_MenuButtonData Hv_MallocMenuButtonData(short style)

/**********************************************
  Mallocs the data for the menu button. If
  the style is text based, it mallocs additional
  data
***********************************************/

{
  Hv_MenuButtonData     mbd;
  int                   i;

  mbd = (Hv_MenuButtonData)Hv_Malloc(sizeof(Hv_MenuButtonDataRec));

  mbd->style = style;

  switch (style) {
  case Hv_TEXTBASED:
    mbd->current = 1;
    mbd->estyles = (short *)Hv_Malloc(Hv_MAXCHOICE*sizeof(short));
    mbd->entries = (Hv_String **)Hv_Malloc(Hv_MAXCHOICE*sizeof(Hv_String *));

    for (i = 0; i < Hv_MAXCHOICE; i++) {
      mbd->estyles[i] = Hv_MENUBUTTONCHOICE;
      mbd->entries[i] = NULL;
    } 
    break;

  case Hv_ICONBASED:
    mbd->estyles = NULL;
    mbd->entries = NULL;
    break;
  }

/* common settings */

  mbd->title = NULL;
  mbd->ChoiceCB = NULL;  

  return mbd;
}

/*--------- Hv_SetMenuButtonDisplayRect ---------*/

static void Hv_SetMenuButtonDisplayRect(Hv_Item Item,
					Hv_Point pp)

/***********************************************
  This routine should be called prior to popping
  up the menu; it sets the rectangle where the
  choices will be displayed.
************************************************/

{
  Hv_MenuButtonData     mbd;
  short                 x, y;
  short                 w = 0;
  short                 h = 0;
  short                 sw = 0;
  short                 sh = 0;
  int                   i;
  Hv_String             *str;

  mbd = Hv_GetMenuButtonData(Item);

  switch (mbd->style) {
  case Hv_TEXTBASED:
    for (i = 0; i < mbd->numrow; i++) {

/* take a step dependendent on whether it is a separator
   and on the size of the string */

      if (mbd->estyles[i] == Hv_SEPARATORTAG) {
	h += Hv_MBSEPGAP;
      }
      else {
	str = mbd->entries[i];
	Hv_CompoundStringDimension(str, &sw, &sh);
	w = Hv_sMax(w, (short)(sw + 3*Hv_MBMARGIN)); 
	h += (sh + Hv_MBEXTRA);
      }
    }
    w = Hv_sMax(w, 20);
    h += Hv_MBMARGIN;
    break;
    
  case Hv_ICONBASED:
    w = 1 + (mbd->numcol) * (mbd->iconwidth + 1);
    h = 1 + (mbd->numrow) * (mbd->iconheight + 1);
    break;
  }

/* now that we have the w and h we only want
   to be sure that the rect is visible */

  if ((pp.x + w) < Hv_canvasRect.right)
    x = pp.x - Hv_MBPAD;
  else
    x = pp.x - w + Hv_MBPAD;

  if ((pp.y + h) < Hv_canvasRect.bottom)
    y = pp.y - Hv_MBPAD;
  else
    y = pp.y - h + Hv_MBPAD;

  Hv_SetRect(&(mbd->display), x, y, w, h);

/* final check: shouldn't be needed but it
   is a disaster if pp is not in the rect */

  if (!Hv_PointInRect(pp, &(mbd->display))) {
    Hv_OffsetRect(&(mbd->display),
		  (short)(pp.x - mbd->display.left - Hv_MBPAD),
		  (short)(pp.y - mbd->display.top - Hv_MBPAD));
    Hv_Println("Benign warning: seemingly unnecessary offset in Hv_SetMenuButtonDisplayRec.");
  }
}


/*--------- Hv_HandleMenuButton ---------*/

static void Hv_HandleMenuButton(Hv_Event hvevent)

/***********************************************
  This is the workhorse routine that will
  actually track the pointer and make the selection;
************************************************/

{
  Hv_Item            Item = hvevent->item;
  Boolean            done = False;
  Boolean            outrange = False;
  Hv_MenuButtonData  mbd;
  Hv_Pixmap          pmap;
  Hv_XEvent          Event;
  Hv_Rect            rect;
  Hv_Point           pp;
  short              newchoice = -32000;
  short              oldchoice = -32000;
  Hv_Rect            oldrect;
  short              origchoice;


  mbd = Hv_GetMenuButtonData(Item);
  origchoice = mbd->current;

  if (mbd->hasactivearea) 
    Hv_GetActiveRect(Item, &rect);

/* if not in active rect, see if there is an alternate CB,
   if not, go ahead and process the menu */

  if (!(mbd->hasactivearea) || (!Hv_PointInRect(hvevent->where, &rect))) {
    if (mbd->AlternateCB != NULL) {
      mbd->AlternateCB(hvevent);
      return;
    }
  }

/* from here on we are in the "MenuCB" */

  Item->state = Hv_ON;
  Hv_DrawItem(Item, NULL);

  Hv_SetMenuButtonDisplayRect(Item, hvevent->where);

/* save what is under the display of choices */

	Hv_Println("calling save under in menubutton %d %d %d %d",
	  mbd->display.left,
	  mbd->display.top,
	  mbd->display.width,
	  mbd->display.height);

  pmap = Hv_SaveUnder(mbd->display.left,
			mbd->display.top,
			mbd->display.width,
			mbd->display.height);

  Hv_FullClip();
  Hv_DisplayChoices(Item);

/* initialize */

  newchoice = Hv_PresentChoice(Item, hvevent->where, &rect);
  if (mbd->style == Hv_TEXTBASED)
    Hv_ShrinkRect(&rect, 2, 2);
  Hv_Simple3DRect(&rect, True, -1);
  Hv_CopyRect(&oldrect, &rect);
  oldchoice = newchoice;
  
/* now track the cursor until mouse up
   or left display rect */

  while (!done) {

/* if Check For Motion or Button Releases */

      if (Hv_CheckMaskEvent(Hv_BUTTONRELEASEMASK | Hv_BUTTON1MOTIONMASK,
			    &Event)) {

	switch(Event.type) {

/* Motion Notify means the mouse is moving */

	case Hv_motionNotify:
	  Hv_SetPoint(&pp,
		      Hv_GetXEventX(&Event),
		      Hv_GetXEventY(&Event));

	  if (!Hv_PointInRect(pp, &(mbd->display))) {
	    done = True;
	    outrange = True;
	  }


/* highlight the current choice */

	  if (!done) {
	    newchoice = Hv_PresentChoice(Item, pp, &rect);
	    if (mbd->style == Hv_TEXTBASED)
	      Hv_ShrinkRect(&rect, 2, 2);

	    if (newchoice != oldchoice) {
	      if (oldchoice > 0) {

/* textbased just "erase" the old one, icon based
   draw a 3D rect */

		if (mbd->style == Hv_TEXTBASED) {
		  Hv_SetLineWidth(2);
		  Hv_FrameRect(&oldrect, mbd->armcolor);
		  Hv_SetLineWidth(0);
		}
		else {
		  /*		  Hv_Simple3DRect(&oldrect, False, -1); */
		  Hv_FrameRect(&oldrect, Hv_black);
		}
	      }

	      if (newchoice > 0)  {
		Hv_Simple3DRect(&rect, True, -1);
		Hv_CopyRect(&oldrect, &rect);
	      }
	      oldchoice = newchoice;

	    }
	  }

	  break;
	  
/* if button 1 released, we are done */

	case Hv_buttonRelease:
	  if (Hv_GetXEventButton(&Event) == 1) {	      

	    done = True;
	    if (newchoice > 0)
	      mbd->current = newchoice;

/* call the callback if the choice is not the same
   as the original choice */

	    if ((mbd->current != origchoice) && (mbd->ChoiceCB != NULL))
	      mbd->ChoiceCB(Item, mbd->current);

	    break;
	  }

	} /* end switch event type */
      }  /* end  if if CheckMask */
      
    }   /*end of while !done */


/* restore */

	Hv_Println("calling restore save under in menubutton %d %d %d %d",
	  mbd->display.left,
	  mbd->display.top,
	  mbd->display.width,
	  mbd->display.height);

	Hv_RestoreSaveUnder(pmap,
	  mbd->display.left,
	  mbd->display.top,
	  mbd->display.width,
	  mbd->display.height);
  Hv_FreePixmap(pmap);

/* Hv_FillRectangle(mbd->display.left,
				mbd->display.top,
		      mbd->display.width,
			  mbd->display.height,
			  Hv_red);*/

  Item->state = Hv_OFF;
  Hv_DrawItem(Item, NULL);

/* tandem CB? */

  if (!outrange && mbd->tandem && mbd->AlternateCB)
    mbd->AlternateCB(hvevent);
  
}

/*------ Hv_PresentChoice -------*/

static short Hv_PresentChoice(Hv_Item Item,
			      Hv_Point pp,
			      Hv_Rect  *rect)

/***********************************************
  Returns what would be the value of "current"
  if the user released the button at this point.
  Sets rect to the rect that should be highlighted.
************************************************/

{
  Hv_MenuButtonData    mbd = Hv_GetMenuButtonData(Item);
  int                  choice;
  int                  i, j;

  switch(mbd->style) {
  case Hv_TEXTBASED:
    for (choice = 1; choice <= mbd->numrow; choice++) {
      Hv_GetMenuButtonChoiceRect(Item, choice, rect);
      if (Hv_PointInRect(pp, rect)) {
	if (mbd->estyles[choice-1] == Hv_SEPARATORTAG)
	  return 0;
	else
	  return choice;
      }
    }

    break;
    
  case Hv_ICONBASED:
    for (i = 0; i < mbd->numrow; i++)
      for (j = 0; j < mbd->numcol; j++) {
	choice = 256*(i+1) + (j+1);
	Hv_GetMenuButtonChoiceRect(Item, choice, rect);
	if (Hv_PointInRect(pp, rect))
	  return choice;
      }
    break;
  }

  return -1;
}


/*-------- Hv_DisplayChoices --------*/

static void Hv_DisplayChoices(Hv_Item Item)

/***********************************************
  Displays the choices in the display rect
************************************************/

{
  Hv_MenuButtonData   mbd = Hv_GetMenuButtonData(Item);
  int                 i, j;
  short               sw, sh;
  Hv_String           *str;
  Hv_Rect             rect;
  int                 choice;

  switch (mbd->style) {
  case Hv_TEXTBASED:
    Hv_Simple3DRect(&(mbd->display), False, mbd->armcolor);
    for (i = 0; i < mbd->numrow; i++) {
      choice = i+1;
      Hv_GetMenuButtonChoiceRect(Item, choice, &rect);

      if (mbd->estyles[i] == Hv_SEPARATORTAG) {
	Hv_DrawLine((short)(rect.left+3), (short)(rect.top+3), (short)(rect.right-3), (short)(rect.top+3), Hv_black);
	Hv_DrawLine((short)(rect.left+3), (short)(rect.top+4), (short)(rect.right-3), (short)(rect.top+4), Hv_white);
      }
      else {
	str = mbd->entries[i];
	Hv_CompoundStringDimension(str, &sw, &sh);
	Hv_DrawCompoundString((short)(rect.left+Hv_MBMARGIN),
		(short)(rect.top+Hv_MBMARGIN), str);
      }
    }

    break;

  case Hv_ICONBASED:
    for (i = 0; i < mbd->numrow; i++)
      for (j = 0; j < mbd->numcol; j++) {
	choice = 256*(i+1) + (j+1);
	Hv_GetMenuButtonChoiceRect(Item, choice, &rect);

	if(mbd->current == choice)
	  Hv_Simple3DRect(&rect, False, mbd->ccolor);
	else {
	  /*	  Hv_Simple3DRect(&rect, False, mbd->armcolor);*/
	  Hv_FillRect(&rect, mbd->armcolor);
	  Hv_FrameRect(&rect, Hv_black);
	}
	Hv_ShrinkRect(&rect, 1, 1);
	Hv_IconDraw(Item, NULL, (short)choice, &rect);
	Hv_FullClip(); 
      }
    break;
    
  }
}

/*-------- Hv_GetMenuButtonChoiceRect -------*/

void Hv_GetMenuButtonChoiceRect(Hv_Item  Item,
				int      choice,
				Hv_Rect  *rect)

/***********************************************
  This reurns the rect that encloses the
  "choice" coresponding to the given choice,
  which is a NON C index and is composite
  for ICON based
************************************************/


{
  Hv_MenuButtonData    mbd = Hv_GetMenuButtonData(Item);

  short                w = 0;
  short                x = 0;
  short                y = 0;

  short                sw, h;
  int                  i, j, row, col;
  Hv_String            *str;

  switch(mbd->style) {
  case Hv_TEXTBASED:
    w = mbd->display.width;
    x = mbd->display.left;
    y = mbd->display.top + Hv_MBMARGIN;
    for (i = 1; i <= choice; i++) {
      j = i-1;

      if (mbd->estyles[j] == Hv_SEPARATORTAG)
	h = Hv_MBSEPGAP;
      else {
	str = mbd->entries[j];
	Hv_CompoundStringDimension(str, &sw, &h);
	h += Hv_MBEXTRA;
      }

      if (i < choice)
	y += h;
    }

    break;
    
  case Hv_ICONBASED:
    w = mbd->iconwidth;
    h = mbd->iconheight;
    row = choice / 256;
    col = choice % 256;
    
    y = mbd->display.top  + 1 + (row-1)*(h+1);
    x = mbd->display.left + 1 + (col-1)*(w+1);
    break;
  }

  Hv_SetRect(rect, x, y, w, h);
}


#undef Hv_MBSEPGAP
#undef Hv_MBEXTRA
#undef Hv_MBPAD
#undef Hv_MBMARGIN
#undef Hv_MBACTIVEWIDTH
#undef Hv_MBBUTACTRECTGAP
