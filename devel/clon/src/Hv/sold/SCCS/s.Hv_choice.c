h59357
s 00000/00000/00000
d R 1.2 02/08/25 23:21:05 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_choice.c
e
s 00343/00000/00000
d D 1.1 02/08/25 23:21:04 boiarino 1 0
c date and time created 02/08/25 23:21:04 by boiarino
e
u
U
f e 0
t
T
I 1
/*
-----------------------------------------------------------------------
File:     Hv_choice.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

/* This file contains functions dealing with choice items 

   NOTE: in prerelease versions of Hv, Hv_CHOICESETITEMs
   and Hv_CHOICEITEMs were called Hv_RADIOCONTAINER items
   and Hv_RADIOBUTTONITEMS. The names were changed to avoid
   confusion with the Motif radio widgets used in dialogs.

   For a while, backward compatibility with the old
   names will be maintained.

*/

#include "Hv.h"	  /* contains all necessary include files */


/****************** BACKWARD COMPATIBILITY WRAPPERS ***********/

void Hv_DrawRadioButtonItem(Hv_Item   Item,
			    Hv_Region  region)
{
  Hv_DrawChoiceItem(Item, region);
}

void Hv_DrawRadioContainerItem(Hv_Item   Item,
			       Hv_Region  region)
{
  Hv_DrawChoiceSetItem(Item, region);
}

void Hv_RadioButtonInitialize(Hv_Item      Item,
			      Hv_AttributeArray attributes)

{
  Hv_ChoiceInitialize(Item, attributes);
}


void Hv_RadioContainerInitialize(Hv_Item      Item,
				 Hv_AttributeArray attributes)

{
  Hv_ChoiceSetInitialize(Item, attributes);
}

void     Hv_PosSetActiveRadio(short       pos,
			      Hv_Item     radiocont)
{
  Hv_PosSetChoice(pos, radiocont);
}

short Hv_HandleRadioButton(Hv_Item Item)
{
  return Hv_HandleChoice(Item);
}


/*************************************************************/


/* ------------- Hv_DrawChoiceItem -----*/

void Hv_DrawChoiceItem(Hv_Item   Item,
		       Hv_Region  region)

/* this draws the diamond shaped chice (radio)
   button that is a one-of-many selector */

{
  short   x, y;
  short   size = 12;
  short   size2;
  Boolean optdraw;
  Hv_Rect rect;

  size2 = size/2;


  x = Item->area->left + size2+1;
  y = (Item->area->top + Item->area->bottom)/2;

  optdraw = Hv_CheckItemDrawControlBit(Item, Hv_OPTIONALDRAW);
  if (optdraw)
      Hv_SetRect(&rect, x - size2, y - size2,  size, size);

  if (Item->state == Hv_OFF) {
      if (optdraw)
	  Hv_Simple3DRect(&rect, True, Item->scheme->fillcolor);
      else
	  Hv_Simple3DCircle(x, y, size2, Item->scheme->fillcolor, True);
  }
  else {
      if (optdraw)
	  Hv_Simple3DRect(&rect, False, Item->scheme->hotcolor);
      else
	  Hv_Simple3DCircle(x, y, size2, Item->scheme->hotcolor, False);
  }

  Hv_DrawCompoundString(Item->area->left + size + 6, Item->area->top + 1, Item->str);
}

/* ------------- Hv_DrawChoiceSetItem -----*/

void Hv_DrawChoiceSetItem(Hv_Item   Item,
			       Hv_Region  region)

/* draws the container that holds a set of choice buttons */

{

/* lightweight? */

  if (Hv_CheckItemDrawControlBit(Item, Hv_LIGHTWEIGHT))
      Hv_Simple3DRect(Item->area, False, Item->scheme->fillcolor);
  else {
      Hv_FillRect(Item->area, Item->scheme->fillcolor); 
      Hv_Draw3DRect(Item->area, Item->scheme); 
      Hv_FrameRect(Item->area, Hv_black); 
  }

}

/* -------  Hv_ChoiceInitialize --------*/

void Hv_ChoiceInitialize(Hv_Item      Item,
			      Hv_AttributeArray attributes)

/* initialization procedure for choice items */

{
  Item->standarddraw = Hv_DrawChoiceItem;
  Hv_DefaultSchemeOut(&(Item->scheme));
  Item->scheme->topcolor -= 1;
  Item->scheme->bottomcolor += 1;
  Item->str = Hv_CreateString(NULL);
  Item->state = attributes[Hv_STATE].s;
  

  if (Item->state == Hv_ON)
    Item->parent->data = (void *)Item;
}

/* -------  Hv_ChoiceSetInitialize --------*/

void Hv_ChoiceSetInitialize(Hv_Item      Item,
			    Hv_AttributeArray attributes)


/* initializes a chioce set -- including creating
   any choice buttons passed in tha attributes array */

{
  Hv_View      View = Item->view;
  short        n;
  int          i;
  char        **list;
  short        hotcolor;
  short        font;
  short        wmax = 1;
  short        hmax = 0;
  short        w, h, tw, th;
  XmString     xmst;
  short        rcw, rch;
  short        gap=5;
  Hv_Item      child;
  short        left, top;
  short        nowon;
  Boolean      hastitle = False;

  n = Hv_vaNumOpt;
  list = Hv_vaText;

  gap = 5 + attributes[Hv_GAP].s;


  if ((n <= 0) || (list == NULL)) {
    fprintf(stderr, "Hv warning: trying to add choice set with no choice items\n");
    return;
  }

  Item->standarddraw = Hv_DrawChoiceSetItem;
  Hv_DefaultSchemeIn(&(Item->scheme));
  Item->scheme->fillcolor -= 1;

/* item cannot be selected */

  Hv_SetBit(&(Item->drawcontrol), Hv_NOPOINTERCHECK);

  font = attributes[Hv_FONT].s;
  nowon = attributes[Hv_NOWON].s;

  if (nowon > 0)
    nowon--;        /* C index stuff */

  if (attributes[Hv_FILLCOLOR].s != Hv_DEFAULT)
    Item->scheme->fillcolor = attributes[Hv_FILLCOLOR].s;

/* use the fillcolor attribute to set the scheme hot color.  This
  is the color used to fill it in when it is selected */

  if (attributes[Hv_ARMCOLOR].s != Hv_DEFAULT)
    hotcolor = attributes[Hv_ARMCOLOR].s;
  else
    hotcolor = Hv_red;

/* see if we have a title */

  if (attributes[Hv_TITLE].v != NULL) {
    hastitle = TRUE;
    xmst = Hv_CreateAndSizeMotifString(attributes[Hv_TITLE].v, font, &tw, &th);
    wmax = tw + 20;
    hmax = th + gap + gap;
  }

/* see how big we are going to have to make the choice set box */

  for (i = 0; i < n; i++) {
    xmst = Hv_CreateAndSizeMotifString(list[i], font, &w, &h);
    w += 40;   /* allow for button and margin */
    wmax = Hv_sMax(w, wmax);
    hmax = hmax + h + gap;
    XmStringFree(xmst);
  }

  hmax += h;
  rcw = Hv_sMax(wmax, attributes[Hv_WIDTH].s);
  rch = Hv_sMax(hmax, attributes[Hv_HEIGHT].s);

  Hv_SetRect(Item->area, Item->area->left, Item->area->top, rcw, rch);

  left = Item->area->left + 8;
  top = Item->area->top + 10;

/* add a title */

  if (hastitle) {
    child = Hv_VaCreateItem(View,
			    Hv_TYPE,            Hv_TEXTITEM,
			    Hv_PARENT,          Item,
			    Hv_LEFT,            left+2,
			    Hv_TOP,             top,
			    Hv_FONT,            font,
			    Hv_FILLCOLOR,       Item->scheme->fillcolor,
			    Hv_TEXT,            attributes[Hv_TITLE].v,
			    NULL);
    top = top + th + gap + gap;
  }

/* now add the choice buttons as children*/

  for (i = 0; i < n; i++) {
    child = Hv_VaCreateItem(View,
			    Hv_TYPE,            Hv_CHOICEITEM,
			    Hv_PARENT,          Item,
			    Hv_LEFT,            left+2,
			    Hv_TOP,             top,
			    Hv_WIDTH,           rcw-2,
			    Hv_HEIGHT,          h+2,
			    Hv_FONT,            font,
			    Hv_SINGLECLICK,     attributes[Hv_SINGLECLICK].v,    
			    Hv_STATE,           (i == nowon),
			    Hv_TEXT,            list[i],
			    Hv_USER1,           (short)(i+1),
			    NULL);
    child->scheme->hotcolor = hotcolor;

    if (attributes[Hv_COLOR].s != Hv_DEFAULT)
      child->scheme->fillcolor = attributes[Hv_COLOR].s;
    else
      child->scheme->fillcolor = Hv_gray10;
    
    top = top + gap + h;
  }

}

/*------- Hv_PosSetChoice ----------*/

void     Hv_PosSetChoice(short       pos,
			 Hv_Item     choiceset)

/* This is the equivalent (For Hv choice sets) of
   Hv_PosSetActiveRadioButton (which is for motif radios) */

{
}

/*------- Hv_HandleChoice ------------*/

short Hv_HandleChoice(Hv_Item Item)

/* should be called when a
   user has selected a choice
   button */

{
  Hv_Item      parent;
  Hv_Item      nowon;
  Hv_Region    prgn, viewrgn, udrgn;

  if ((Item == NULL) || (Item->type != Hv_CHOICEITEM))
    return -1;

  parent = Item->parent;
  nowon = (Hv_Item)(parent->data); 
  
  if (nowon == Item)
    return Item->user1;

  nowon->state = Hv_OFF;
  Item->state = Hv_ON;
  parent->data = (void *)Item;

  prgn = Hv_CopyItemRegion(parent, True);
  viewrgn = Hv_GetViewRegion(Item->view);
  udrgn = Hv_IntersectRegion(prgn, viewrgn);

  Hv_DrawItem(nowon, udrgn);
  Hv_DrawItem(Item,  udrgn);

  Hv_DestroyRegion(prgn);
  Hv_DestroyRegion(viewrgn);
  Hv_DestroyRegion(udrgn);

  return Item->user1;
}



E 1
