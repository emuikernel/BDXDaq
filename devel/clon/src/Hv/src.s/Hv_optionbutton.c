 /**
 * <EM>Various routines dealing with option buttons.</EM>
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

#define  OPTBUTTONSIZE 10
#define  OPTBUTTONSIZE2 5
#define  ROPTBUTTONSIZE  12
#define  ROPTBUTTONSIZE2 6
#define  MAXOBINCOL    255

/*------ local prototypes ---------*/

static void Hv_DrawOptionButtonItem(Hv_Item   Item,
				    Hv_Region  region);

static Boolean  Hv_BoxOnly = False;

static void Hv_DrawCheck(Hv_Rect   *rect,
			 short     color);

/**
 * Allocates data for an option button data structure.
 * @return   Pointer to newly allocated structure.
 */

Hv_OptionButtonData *Hv_MallocOptionButtonData() {

  int                  i;
  Hv_OptionButtonData *obd;

/* make one extra to ensure terminator */

  obd = (Hv_OptionButtonData *)Hv_Malloc((MAXOBINCOL+1)*sizeof(Hv_OptionButtonData));
  
  for (i = 0; i <= MAXOBINCOL; i++)
    obd[i].label = NULL;
  
  return obd;
}


/**
 * Frees an Hv_FreeOptionButtonData structure.
 * @param   obd   Pointer to the structure to be freed.
 */

void Hv_FreeOptionButtonData(Hv_OptionButtonData *obd) {

  int i;

  for (i = 0; i <= MAXOBINCOL; i++) {
    if (obd[i].label != NULL)
      Hv_Free(obd[i].label);
  }
  
  Hv_Free(obd);

}

/*
 *  Hv_SetOptionButtonData
 */

void Hv_SetOptionButtonData(Hv_OptionButtonData *obd,
			    char           *label,
			    Hv_FunctionPtr  callback,
			    Hv_Item        *item,
			    Boolean        *control,
			    Boolean        enabled,
			    Boolean        redraw) {
  int i;

  if ((obd == NULL) || (label == NULL))
    return;

/* find first empty slot */

  i = 0;

  while (obd[i].label != NULL) {
    i++;
    if (i >= MAXOBINCOL)
      return;
  }

  Hv_InitCharStr(&(obd[i].label), label);
  obd[i].callback = callback;
  obd[i].item     = item;
  obd[i].control  = control;
  obd[i].enabled  = enabled;
  obd[i].redraw   = redraw;
}


/*
 *  Hv_AddOptionButtonColumn
 *
 * Convenience routine for adding a column of
 * option buttons
 *
 * parent: usually an Hv_Box holding the buttons
 * rel_x, rel_y: position of 1st button rel to top left of parent
 */


Hv_Item Hv_AddOptionButtonColumn(Hv_Item             parent,
				 short               rel_x,
				 short               rel_y,
				 short               armcolor,
				 short               font,
				 short               gap,
				 Hv_OptionButtonData obd[],
				 short               *w,
				 short               *h) {

  int        n;
  int        i;
  short      sw, sh;
  Hv_Item    Item;

/* check for some pathologies and cave if encountered */


  if ((parent == NULL ) || (obd == NULL)) {
    Hv_Println("Bad input to Hv_AddOptionButtonColumn. Column not created.");
    return NULL;
  }


/* array must be terminated with a NULL in the label position */

  n = 0;
  while(obd[n].label != NULL) {
    n++;
    if (n > MAXOBINCOL) {
      Hv_Println("Bad input (n too big) to Hv_AddOptionButtonColumn. Column not created.");
      return NULL;
    }
  }

/* weird case where we got a label array with NULL in 1st slot */

  if (n == 0) {
    Hv_Println("Bad input (n = 0) to Hv_AddOptionButtonColumn. Column not created.");
    return NULL;
  }


/* OK should be golden now */

  *w = 0;
  *h = 0;

  for (i = 0; i < n; i++) {

    if (i == 0) {
      Item = Hv_VaCreateItem(parent->view,
			     Hv_PARENT,             parent,
			     Hv_TYPE,               Hv_OPTIONBUTTONITEM,
			     Hv_CONTROLVAR,         (void *)(obd[i].control),
			     Hv_LEFT,               parent->area->left + rel_x,
			     Hv_TOP,                parent->area->top + rel_y,
			     Hv_TEXT,               obd[i].label,
			     Hv_FONT,               font,
			     Hv_ARMCOLOR,           armcolor,
			     Hv_DRAWCONTROL,        obd[i].redraw ? 
			     Hv_REDRAWHOTRECTWHENCHANGED : 0,
				       Hv_CALLBACK,           obd[i].callback,
			     NULL);
    }
    else {

      Item = Hv_VaCreateItem(parent->view,
			     Hv_PARENT,             parent,
			     Hv_TYPE,               Hv_OPTIONBUTTONITEM,
			     Hv_CONTROLVAR,         (void *)(obd[i].control),
			     Hv_TEXT,               obd[i].label,
			     Hv_FONT,               font,
			     Hv_ARMCOLOR,           armcolor,
			     Hv_DRAWCONTROL,        obd[i].redraw ? 
			     Hv_REDRAWHOTRECTWHENCHANGED : 0,
			     Hv_CALLBACK,           obd[i].callback,
			     Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			     Hv_PLACEMENTGAP,       gap,
			     NULL);
    }


/* by default, item is enabled */

    if (!(obd[i].enabled))
      Hv_SetItemSensitivity(Item, False, False);

    if (obd[i].item != NULL)
      *(obd[i].item) = Item;

    Hv_CompoundStringDimension(Item->str,  &sw,  &sh);

    *w = Hv_sMax(*w, sw);
    *h = *h + sh + 6 + gap;

  } /* end loop over i */

/* add the space for the rectangle */

  *w += (10 + OPTBUTTONSIZE);

  return Item; /* last one created */
}



/**
 * Convenience routine for an option button.
 * @param   View          View owner.
 * @param   parent        Parent item.
 * @param   pitem         Placement item.
 * @param   control       Boolean controlled by this option button.
 * @param   label         Label for this option button.
 * @param   drawcontrol   Additional draw control bits.
 * @return  The option button.
 */

Hv_Item  Hv_StandardOptionButton(Hv_View  View,
				 Hv_Item  parent,
				 Hv_Item  pitem, 
				 Boolean  *control,
				 char     *label,
				 int      drawcontrol) {
/*
  Hv_Item   pitem         put below, unless null, then size to parent
  int       drawcontrol   sets auto hotrect redraw upon change
*/


  Hv_Item   Item;
  Hv_OptionButtonPtr    obptr;

/* default to armcolor of -1, which will use win32 like checks */

  short armcolor = -1;

  if ((pitem != NULL) && (pitem->type == Hv_OPTIONBUTTONITEM)) {
    obptr = (Hv_OptionButtonPtr)(pitem->data);
    if (obptr != NULL)
      armcolor = obptr->armcolor;
  }

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
			   Hv_ARMCOLOR,           armcolor,
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
			   Hv_ARMCOLOR,           armcolor,
			   Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			   Hv_PLACEMENTGAP,       -3,
			   NULL);

  parent->area->right  = Hv_sMax(parent->area->right,  (short)(Item->area->right + 5));
  parent->area->bottom = Hv_sMax(parent->area->bottom, (short)(Item->area->bottom + 2));


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
  short                 l,  t,  yc, xc;
  Boolean               disabled;

  l = Item->area->left;
  t = Item->area->top;
  xc = l + 1 + ROPTBUTTONSIZE2;
  yc = (t + Item->area->bottom) / 2;
  
  obptr = (Hv_OptionButtonPtr)(Item->data);
  state = *(obptr->control);
  
  Hv_SetRect(&rect,  (short)(l + 2),  (short)(yc - OPTBUTTONSIZE2),
             OPTBUTTONSIZE,  OPTBUTTONSIZE);

  disabled = !Hv_ItemIsEnabled(Item);

  if (disabled) {
 /*   Hv_SetPattern(Hv_80PERCENTPAT, Hv_black); */

/* round or rectangular ? */

    if(Hv_CheckItemDrawControlBit(Item, Hv_ROUNDED))
      Hv_Simple3DCircle(xc, yc, ROPTBUTTONSIZE2, Hv_gray10, True);
    else
      Hv_Simple3DRect(&rect,  False,  Hv_gray10);

/*    Hv_SetPattern(Hv_FILLSOLIDPAT, Hv_black); */

    if (!Hv_BoxOnly) {
	if ((Item->parent != NULL) && (Item->parent->scheme != NULL))
	    Hv_DrawGhostedText(rect.right + 8, t + 3, Item->str,
			       (short)(Item->parent->scheme->fillcolor-4), 
			       (short)(Item->parent->scheme->fillcolor+4));
	else
	    Hv_DrawGhostedText((short)(rect.right + 8), (short)(t + 3), Item->str, Hv_white, Hv_gray8);
    }
  }
  else {
    if (state) {
      if(Hv_CheckItemDrawControlBit(Item, Hv_ROUNDED)) {
	Hv_Simple3DCircle(xc, yc, ROPTBUTTONSIZE2, Hv_gray12, False);
	Hv_FillCircle(xc, yc, ROPTBUTTONSIZE2-2, False, obptr->armcolor, -1);
      }
      else {
	if (obptr->armcolor < 0) {
	  Hv_Simple3DRect(&rect,  False,  Hv_gray14);
	  Hv_DrawCheck(&rect, Hv_black);
	}
	else
	    Hv_Simple3DRect(&rect,  False,  obptr->armcolor);
      }
    }
    else {
      if(Hv_CheckItemDrawControlBit(Item, Hv_ROUNDED))
	Hv_Simple3DCircle(xc, yc, ROPTBUTTONSIZE2, Hv_gray12, True);
      else
	Hv_Simple3DRect(&rect,  True,  Hv_gray12);
    }
    if (!Hv_BoxOnly) {
      if(Hv_CheckItemDrawControlBit(Item, Hv_ROUNDED))
	Hv_DrawCompoundString((short)(xc + ROPTBUTTONSIZE2 + 5), (short)(t + 4), Item->str);
      else
	Hv_DrawCompoundString((short)(rect.right + 8), (short)(t + 3), Item->str);
    }
  }
  


}

/**
 * Draws a windows like check within an option button
 */


static void Hv_DrawCheck(Hv_Rect   *rect,
			 short     color) {

    Hv_Point   xp[6];

    int l, t, r, b;

    if (rect == NULL)
	return;


    l = rect->left+2;
    r = rect->left + rect->width-3;
    t = rect->top+2;
    b = rect->top + rect->height-3;

    xp[0].x = l;       xp[0].y = b-3;
    xp[5].x = l;       xp[5].y = b-4;

    xp[1].x = l+2;     xp[1].y = b;
    xp[4].x = l+2;     xp[4].y = b-2;

    xp[2].x = r;       xp[2].y = t+2;
    xp[3].x = r;       xp[3].y = t;

    Hv_FillPolygon(xp, 6, True, color, color);
}


#undef  OPTBUTTONSIZE
#undef  OPTBUTTONSIZE2
#undef  MAXOBINCOL
