/*
-----------------------------------------------------------------------
File:     Hv_optionbutton.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

/* This file contains functions dealing with radiocontainer items */

#include "Hv.h"	  /* contains all necessary include files */

#define  OPTBUTTONSIZE 10
#define  OPTBUTTONSIZE2 5

/*------ local prototypes ---------*/

static void Hv_DrawOptionButtonItem(Hv_Item   Item,
				    Hv_Region  region);

static Boolean  Hv_BoxOnly = False;


/* ------ Hv_StandardOptionButton ------*/

Hv_Item  Hv_StandardOptionButton(Hv_View  View,
				 Hv_Item  parent,
				 Hv_Item  pitem, 
				 Boolean  *control,
				 char   *label,
				 int    drawcontrol)
/*
  Hv_Item   pitem         put below, unless null, then size to parent
  int       drawcontrol   sets auto hotrect redraw upon change
*/

{
  Hv_Item   Item;

/* backwards compatibility check */

  if (drawcontrol == 1)
    drawcontrol = Hv_REDRAWHOTRECTWHENCHANGED;

/* if pitem is NULL, assume upper left of parent, else place below pitem */

  if (pitem == NULL)
    Item = Hv_VaCreateItem(View,
			   Hv_PARENT,             parent,
			   Hv_TYPE,               Hv_OPTIONBUTTONITEM,
			   Hv_CONTROLVAR,         (void *)control,
			   Hv_TEXT,               label,
			   Hv_FONT,               Hv_fixed2,
			   Hv_ARMCOLOR,           Hv_red,
			   Hv_LEFT,               parent->area->left + 5,
			   Hv_TOP,                parent->area->top + 3,
			   NULL);
  else
    Item = Hv_VaCreateItem(View,
			   Hv_PARENT,             parent,
			   Hv_TYPE,               Hv_OPTIONBUTTONITEM,
			   Hv_CONTROLVAR,         (void *)control,
			   Hv_TEXT,               label,
			   Hv_FONT,               Hv_fixed2,
			   Hv_ARMCOLOR,           Hv_red,
			   Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			   Hv_PLACEMENTGAP,       -3,
			   NULL);

  parent->area->right  = Hv_sMax(parent->area->right,  Item->area->right + 5);
  parent->area->bottom = Hv_sMax(parent->area->bottom, Item->area->bottom + 2);


  if (drawcontrol > 0)
    Item->drawcontrol += drawcontrol;

  Hv_FixRectWH(parent->area);

  return  Item;
}


/* -------  Hv_OptionButtonInitialize --------*/

void Hv_OptionButtonInitialize(Hv_Item      Item,
			       Hv_AttributeArray attributes)

{
  Hv_OptionButtonPtr    obptr;
  short                 w,  h;
  
  Item->data = (void *)Hv_Malloc(sizeof(Hv_OptionButtonRec));
  obptr = (Hv_OptionButtonPtr)(Item->data);
  obptr->armcolor = attributes[Hv_ARMCOLOR].s;  /* this will be the "armed" color */
  obptr->control = (Boolean *)(attributes[Hv_CONTROLVAR].v);

  if (Item->singleclick == NULL)
    Item->singleclick = Hv_OptionButtonCallback;
  Item->standarddraw = Hv_DrawOptionButtonItem;
  Hv_DefaultSchemeOut(&(Item->scheme));
  Item->scheme->topcolor -= 1;
  Item->scheme->bottomcolor += 1;
  Item->str = Hv_CreateString(NULL);
  
/* if no control variable was provided, use the Item->state variable */
  
  if (obptr->control == NULL) {
    Item->state = attributes[Hv_STATE].s;
    obptr->control = (Boolean *)(&(Item->state));
  }
  else
    Item->state = (short)(*(obptr->control));

  Item->str = Hv_CreateString(NULL);
  Item->str->font = Hv_sMax(0, attributes[Hv_FONT].s);

  if (attributes[Hv_TEXTCOLOR].s >= 0)
    Item->str->strcol = attributes[Hv_TEXTCOLOR].s;

  Item->str->fillcol = attributes[Hv_FILLCOLOR].s;
  if (attributes[Hv_TEXT].v != NULL)
    Hv_InitCharStr(&(Item->str->text), (char *)(attributes[Hv_TEXT].v));

  Hv_CompoundStringDimension(Item->str,  &w,  &h);
  Item->area->width = w + 20;
  Item->area->height = h + 6;
  Hv_FixRectRB(Item->area);
}


/*------- Hv_OptionButtonCallback ------------*/

void Hv_OptionButtonCallback(Hv_Event hvevent)

{
  Hv_Item    Item = hvevent->item;
  Hv_OptionButtonPtr    obptr;

  obptr = (Hv_OptionButtonPtr)(Item->data);
 
  *(obptr->control) = !(*(obptr->control));  
  
  Hv_BoxOnly = True;
  Hv_DrawItem(Item,  NULL);
  Hv_BoxOnly = False;


/* if the appropriate draw control bit is set, redraw the hotrect */

  if (Hv_CheckBit(Item->drawcontrol, Hv_DIRTYWHENCHANGED)) {
    Hv_SetViewDirty(hvevent->view);
    Hv_DrawViewHotRect(hvevent->view);
    return;
  }

  if (Hv_CheckBit(Item->drawcontrol, Hv_REDRAWHOTRECTWHENCHANGED))
    Hv_DrawViewHotRect(hvevent->view);

}

/*-------- Hv_GetOptionButton ---------*/

Boolean Hv_GetOptionButton(Hv_Item Item)

{
  Hv_OptionButtonPtr    obptr;

  if (Item == NULL)
    return False;

  obptr = (Hv_OptionButtonPtr)(Item->data);
  return (*(obptr->control));
}

/*-------- Hv_SetOptionButton ---------*/

void Hv_SetOptionButton(Hv_Item   Item,
			Boolean   control)

{
  Hv_OptionButtonPtr    obptr;

  if (Item == NULL)
    return;

  obptr = (Hv_OptionButtonPtr)(Item->data);
  *(obptr->control) = control;
  Hv_BoxOnly = True;
  Hv_DrawItem(Item,  NULL);
  Hv_BoxOnly = False;
}


/* ------------- Hv_DrawOptionButtonItem -----*/

static void Hv_DrawOptionButtonItem(Hv_Item   Item,
				    Hv_Region  region)

{
  Hv_OptionButtonPtr    obptr;
  Boolean               state;
  Hv_Rect               rect;
  short                 l,  t,  yc;
  Boolean               disabled;
  
  l = Item->area->left;
  t = Item->area->top;
  yc = (Item->area->top + Item->area->bottom) / 2;
  
  obptr = (Hv_OptionButtonPtr)(Item->data);
  state = *(obptr->control);
  
  Hv_SetRect(&rect,  l + 2,  yc - OPTBUTTONSIZE2,
             OPTBUTTONSIZE,  OPTBUTTONSIZE);

  disabled = !Hv_ItemIsEnabled(Item);

  if (disabled) {
    Hv_SetPattern(1, Hv_black);
    Hv_Simple3DRect(&rect,  False,  Hv_black);
  }
  else {
    if (state)
      Hv_Simple3DRect(&rect,  False,  obptr->armcolor);
    else
      Hv_Simple3DRect(&rect,  True,  Hv_gray12);
  }
  
  if (!Hv_BoxOnly)
    Hv_DrawCompoundString(rect.right + 8, t + 3, Item->str);

  if (disabled)
    Hv_SetPattern(Hv_FILLSOLIDPAT, Hv_black);

}


#undef  OPTBUTTONSIZE
#undef  OPTBUTTONSIZE2
