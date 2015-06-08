/**
 * <EM>Deals deals with choice items on <B>views</B>.</EM>
 * <P>
 *  NOTE: in prerelease versions of Hv, <CODE>Hv_CHOICESETITEMs</CODE>
 *  and <CODE>Hv_CHOICEITEMs</CODE> were called <CODE>Hv_RADIOCONTAINER</CODE> items
 *  and <CODE>Hv_RADIOBUTTONITEMS</CODE>. The names were changed to avoid
 *  confusion with the Motif radio widgets used in dialogs.
 *  For a while, backward compatibility with the old
 *  names will be maintained.
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


#include "Hv.h"	  /* contains all necessary include files */

static Hv_Item  Hv_GetChildChoiceItem(short     pos,
				      Hv_Item   choiceset);

static void  Hv_DestroyChoiceItem(Hv_Item Item);

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

/**
 * Hv_DrawChoiceItem
 * @purpose  Draw the Choice Item.
 * @param   Item      The Item in question.
 * @param   region    The clipping region.
 */

void Hv_DrawChoiceItem(Hv_Item   Item,
		       Hv_Region  region) {

/* this draws the diamond shaped chice (radio)
   button that is a one-of-many selector */

  short   x, y, w, h;
  short   size = 12;
  short   size2;
  Boolean optdraw;
  Hv_Rect rect;

  size2 = size/2;


  x = Item->area->left + size2+1;
  y = (Item->area->top + Item->area->bottom)/2;

  optdraw = Hv_CheckItemDrawControlBit(Item, Hv_OPTIONALDRAW);
  if (optdraw)
      Hv_SetRect(&rect, (short)(x - size2), (short)(y - size2),  size, size);

  if ((!Hv_ItemIsEnabled(Item)) || (Item->state == Hv_OFF)) {
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

  x = (short)(Item->area->left + size + 6);
  y = (short)(Item->area->top + 1);


  if (!Hv_ItemIsEnabled(Item))
    Hv_DrawGhostedText(x, y, Item->str, Hv_white, Hv_gray7);
  else
    Hv_DrawCompoundString(x, y, Item->str);

  if (Item->str != NULL) {
    Hv_CompoundStringDimension(Item->str, &w, &h);
    Item->area->right = x + w;
  }
  else
    Item->area->right = x;

  Hv_FixRectWH(Item->area);


}

/**
 * Hv_DrawChoiceSetItem
 * @purpose Draw the container that holds a set of choice buttons.
 * @param    Item    The Item in question.
 * @param    region  The clipping region.
 */

void Hv_DrawChoiceSetItem(Hv_Item   Item,
			  Hv_Region  region) {

/* draws the container that holds a set of choice buttons */


  if (Hv_CheckItemDrawControlBit(Item, Hv_FRAMEAREA))
      Hv_Simple3DRect(Item->area, False, Item->scheme->fillcolor);
  else
    Hv_FillRect(Item->area, Item->scheme->fillcolor);

}

/**
 * Hv_ChoiceInitialize 
 * @purpose    Initialize a Choice Item.
 * @param      Item       Item being initialized.
 * @param      attributes attribute array.
 */

void Hv_ChoiceInitialize(Hv_Item      Item,
			 Hv_AttributeArray attributes) {

/* initialization procedure for choice items */

  Hv_SetItemDrawControlBit(Item, Hv_FRAMEAREA);

  Item->standarddraw = Hv_DrawChoiceItem;
  Hv_DefaultSchemeOut(&(Item->scheme));
  Item->scheme->topcolor -= 1;
  Item->scheme->bottomcolor += 1;
  Item->str = Hv_CreateString(NULL);
  Item->state = attributes[Hv_STATE].s;
  

  if (Item->state == Hv_ON)
    Item->parent->data = (void *)Item;
}

/**
 * Hv_ChoiceInitialize 
 * @purpose    Initialize a ChoiceSet Item. This includes creating
 *  any choice buttons passed in tha attributes array.
 * @param      Item       Item being initialized.
 * @param      attributes attribute array.
 */

void Hv_ChoiceSetInitialize(Hv_Item      Item,
							Hv_AttributeArray attributes) {



  Hv_View      View = Item->view;
  short        n;
  int          i;
  char        **list;
  short        hotcolor;
  short        font;
  short        wmax = 1;
  short        hmax = 0;
  short        hextra = 0;
  short        w, h, tw, th;
  Hv_CompoundString   cs;
  short        rcw, rch;
  short        gap=5;
  short        vgap = 1;
  short        hgap=25;
  Hv_Item      child;
  short        left, top;
  short        nowon;
  Boolean      hastitle = False;

  int          orientation;

  n = Hv_vaNumOpt;
  list = Hv_vaText;

  gap = 5 + attributes[Hv_GAP].s;
  vgap = attributes[Hv_VGAP].s;

  hgap = hgap + attributes[Hv_GAP].s;

/* horiz or vert? */

  orientation = attributes[Hv_ORIENTATION].s;

  if ((n <= 0) || (list == NULL)) {
    Hv_Println("Hv warning: trying to add choice set with no choice items");
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
    cs = Hv_CreateAndSizeCompoundString(attributes[Hv_TITLE].v, 
					font, 
					&tw,
					&th);
    wmax = tw + 20;
    hextra = th + gap + gap;
  }

/* see how big we are going to have to make the choice set box */

  for (i = 0; i < n; i++) {
    cs = Hv_CreateAndSizeCompoundString(list[i],
					font,
					&w,
					&h);

    if (orientation == Hv_HORIZONTAL) {
	wmax = wmax + w + hgap;
	hmax = Hv_sMax(hmax, (short)(h + gap + vgap));
    }
    else {
	w += 40;   /* allow for button and margin */
	wmax = Hv_sMax(w, wmax);
	hmax = hmax + h + gap;
    }

    Hv_CompoundStringFree(cs);
  }

  hmax += hextra;
  hmax += h;
  rcw = Hv_sMax(wmax, attributes[Hv_WIDTH].s);
  rch = Hv_sMax(hmax, attributes[Hv_HEIGHT].s);

  Hv_SetRect(Item->area, Item->area->left, Item->area->top, rcw, rch);

  if (orientation == Hv_HORIZONTAL)
      left = Item->area->left + 5;
  else
      left = Item->area->left + 8;

  top = Item->area->top + 9 + vgap;

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

    cs = Hv_CreateAndSizeCompoundString(list[i], font, &w, &h);

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
				Hv_DESTROY,         Hv_DestroyChoiceItem,
			    NULL);
    child->scheme->hotcolor = hotcolor;

    if (attributes[Hv_COLOR].s != Hv_DEFAULT)
      child->scheme->fillcolor = attributes[Hv_COLOR].s;
    else
      child->scheme->fillcolor = Hv_gray10;

    if (orientation == Hv_HORIZONTAL)
	left = left + w + hgap;
    else
	top = top + gap + h;


    Hv_CompoundStringFree(cs);

  }

}

/**
 * Hv_PosEnableChoice
 * @param   pos        Ordinal position (1, 2, ...).
 * @param   choiceset  The ChoiceSet Item.
 * @param   enable     If True, enable.
 */

void            Hv_PosEnableChoice(short       pos,
				   Hv_Item     choiceset,
				   Boolean     enabled) {

  Hv_Item    temp;
  Hv_Region  rgn;

  temp = Hv_GetChildChoiceItem(pos, choiceset);
  if (temp != NULL) {
    if (enabled)
      Hv_EnableItem(temp);
    else
      Hv_DisableItem(temp);

    if (Hv_ViewIsVisible(temp->view)) {
      rgn = Hv_GetItemDrawingRegion(temp, 0, 0);
      Hv_DrawItem(temp, rgn);
      Hv_DestroyRegion(rgn);
    }

  }

}


/*------- Hv_PosSetChoice ----------*/

void     Hv_PosSetChoice(short       pos,
			 Hv_Item     choiceset)

/* This is the equivalent (For Hv choice sets) of
   Hv_PosSetActiveRadioButton (which is for motif radios) */

{
  Hv_Item Item = Hv_GetChildChoiceItem(pos, choiceset);
  Hv_HandleChoice(Item);

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

  if (Item == NULL)
    return -1;

  if ((!Hv_ItemIsEnabled(Item)) || (Item->type != Hv_CHOICEITEM))
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


/**
 * Hv_DestroyChoiceItem
 * @purpose Called when an Hv_CHOICEITEM is deleted. This is needed because
 * of a hack. We used the "data" field of the ChoiceSet (parent) item to hold the "nowon"
 * CHOICEITEM. That item, being a child, will have already been deleted. So we
 * must set the data field to NULL so that the standard Item deleter doesn't
 * crash when it calls Hv_Free(Item->data)
 * @param   The Item to destroy.
 * @local
 */

static void  Hv_DestroyChoiceItem(Hv_Item Item) {
	if ((Item->parent != NULL) && (Item->parent->data == Item))
		Item->parent->data = NULL;
}

/**
 * Hv_GetChildChoiceItem
 * @param   pos   The ordinal position.
 * @param   choiceset  The ChoiceSet Item.
 * @return  The child item, or NULL
 * @local
 */

static Hv_Item  Hv_GetChildChoiceItem(short     pos,
				      Hv_Item   choiceset) {
  Hv_Item   temp = NULL;
  int       i = 1;

  if ((pos < 1) || (choiceset == NULL) || (choiceset->children == NULL))
    return NULL;

  temp = choiceset->children->first;

  while ((i <= pos) && (temp != NULL)) {
    if (i == pos)
      return temp;
    i++;
    temp = temp->next;
  }


  return temp;
}



