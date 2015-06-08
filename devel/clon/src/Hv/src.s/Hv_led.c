/*
-----------------------------------------------------------------------
File:     Hv_led.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

/* This file contains functions dealing with LED items */

#include "Hv.h"	  /* contains all necessary include files */

#define Hv_LEDWIDTH  20
#define Hv_LEDHEIGHT 7
#define Hv_LEDGAP    4

static Hv_LEDData   *Hv_MallocLED(short,
				  char **);

static Hv_LEDData   *Hv_GetLEDData(Hv_Item);

static void Hv_EditLEDItem(Hv_Event);

static void Hv_EditLEDColor(Widget);

static Hv_Widget  oncolor, offcolor, bgcolor, neutralcolor;
static short      newoncolor, newoffcolor, newneutralcolor, newbgcolor;

/*-------- Hv_EditLEDITem -------*/

static void Hv_EditLEDItem(Hv_Event hvevent)

{
  Hv_Item           Item =  hvevent->item;
  Hv_LEDData       *led;
  static Hv_Widget  dialog = NULL;
  Hv_Widget         rowcol, rc;
  int               answer;

  if (Item == NULL)
    return;

  led = Hv_GetLEDData(Item);

  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " LED Editor ", NULL);

    rowcol = Hv_DialogColumn(dialog, 8);
    rc = Hv_DialogTable(rowcol, 4, 8);

    oncolor  = Hv_SimpleColorLabel(rc, " on  color ", (Hv_FunctionPtr)Hv_EditLEDColor);
    offcolor = Hv_SimpleColorLabel(rc, " off color ", (Hv_FunctionPtr)Hv_EditLEDColor);
    neutralcolor = Hv_SimpleColorLabel(rc, " neutral color ", (Hv_FunctionPtr)Hv_EditLEDColor);
    bgcolor  = Hv_SimpleColorLabel(rc, " background", (Hv_FunctionPtr)Hv_EditLEDColor);
    rc = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON + Hv_CANCELBUTTON + Hv_APPLYBUTTON);
  }

/* the dialog has been created */

  newoncolor  = led->oncolor;
  newoffcolor = led->offcolor;
  newneutralcolor = led->neutralcolor;
  newbgcolor  = Item->color;

  Hv_ChangeLabelColors(oncolor,  -1, led->oncolor);
  Hv_ChangeLabelColors(offcolor, -1, led->offcolor);
  Hv_ChangeLabelColors(neutralcolor, -1, led->neutralcolor);
  Hv_ChangeLabelColors(bgcolor,  -1, Item->color);

  while ((answer = Hv_DoDialog(dialog, NULL)) > 0) {
    led->oncolor  = newoncolor;
    led->offcolor = newoffcolor;
    led->neutralcolor = newneutralcolor;
    Item->color   = newbgcolor;

    Hv_DrawItem(Item, NULL);

    if (answer != Hv_APPLY)
      break;
  }
}


/* ------------- Hv_DrawLEDItem ------------*/

void Hv_DrawLEDItem(Hv_Item   Item,
		    Hv_Region region)

{
  int          i;
  Hv_LEDData   *led;
  short        x, y, w, h;
  Hv_Rect      rect;
  Hv_String    *hvstr;

  led = Hv_GetLEDData(Item);

  Hv_Simple3DRect(Item->area, False, Item->color);

  x = Item->area->left + Hv_LEDGAP;
  y = Item->area->top  + Hv_LEDGAP;

  for (i = 0; i < led->numled; i++) {

    Hv_SetRect(&rect, x, y, Hv_LEDWIDTH, Hv_LEDHEIGHT);
    if (led->states[i] == Hv_ON)
      Hv_Simple3DRect(&rect, False, led->oncolor);
    else if (led->states[i] == Hv_OFF)
      Hv_Simple3DRect(&rect, False, led->offcolor);
    else
      Hv_Simple3DRect(&rect, False, led->neutralcolor);

    hvstr = Hv_CreateString(led->labels[i]);
    hvstr->font = led->font;
    hvstr->strcol = led->textcolor;

    Hv_CompoundStringDimension(hvstr, &w, &h);
    
    if (led->orientation == Hv_HORIZONTAL) {
      Hv_DrawCompoundString((short)(x + (Hv_LEDWIDTH - w)/2), (short)(rect.bottom+1), hvstr);
      x += (Hv_LEDWIDTH + Hv_LEDGAP);
    }
    else {
      Hv_DrawCompoundString((short)(rect.right+2), (short)(y+(Hv_LEDHEIGHT-h)/2), hvstr);
      y += (Hv_LEDHEIGHT + Hv_LEDGAP);
    }
    Hv_DestroyString(hvstr);
  } 

}

/*-------- Hv_UpdateLED --------*/

void Hv_UpdateLED(Hv_Item Item,
		  int     pos,
		  int     state)

/* state can be state or count */

{
  Hv_LEDData    *led;
  int           i, oldstate, newstate;
  int           n;


  if ((Item == NULL) || (Item->type != Hv_LEDBARITEM))
    return;

  led = Hv_GetLEDData(Item);
  n = led->numled;

  if (pos > n)
    return;

  if (state > 0)
    newstate = Hv_ON;
  else if (state < 0)
    newstate = Hv_NEUTRAL;
  else
    newstate = Hv_OFF;

/* zero  apply to all */
  
  if (pos == 0)
    for (i = 0; i < n; i++)
      led->states[i] = newstate;
  else  if (pos > 0) {
    oldstate = led->states[pos-1];
    
    if (oldstate == newstate)
      return;
    
    led->states[pos-1] = newstate;
  }
  else {   /* pos < 0, make it a "sequencer" treating state as a count */
    state %= n;
    
    for (i = 0; i < n; i++)
      led->states[i] = (state == i);
  } /* end of sequencer */
  

  Hv_DrawItem(Item, NULL);
}


/*-------- Hv_LEDInitialize --------*/

void Hv_LEDInitialize(Hv_Item           Item,
		      Hv_AttributeArray attributes)

{
  Hv_LEDData   *led;
  short         n;
  short         w, h, maxwid, i;
  char         *teststring = "GgZxaQj";

  n = Hv_vaNumOpt;

  led = Hv_MallocLED(n, Hv_vaText);
  Item->data = (void *)led;
  if (led == NULL)
    return;
  
  led->orientation = attributes[Hv_ORIENTATION].s;

  if (attributes[Hv_COLOR].s == Hv_black)
    Item->color = Hv_gray6;
  else
    Item->color = attributes[Hv_COLOR].s;

  if (attributes[Hv_TEXTCOLOR].s != Hv_DEFAULT)
    led->textcolor = attributes[Hv_TEXTCOLOR].s;
  else
    led->textcolor = Hv_black;
  
  if (attributes[Hv_ONCOLOR].s != Hv_DEFAULT)
    led->oncolor = attributes[Hv_ONCOLOR].s;
  if (attributes[Hv_OFFCOLOR].s != Hv_DEFAULT)
    led->offcolor = attributes[Hv_OFFCOLOR].s;
  if (attributes[Hv_NEUTRALCOLOR].s != Hv_DEFAULT)
    led->neutralcolor = attributes[Hv_NEUTRALCOLOR].s;
  led->font = attributes[Hv_FONT].s;


  if (led->orientation == Hv_HORIZONTAL) {
    Hv_CharDimension(teststring, led->font, &w, &h);
    Item->area->width = n*(Hv_LEDWIDTH + Hv_LEDGAP) + Hv_LEDGAP;
    Item->area->height = Hv_LEDHEIGHT + Hv_LEDGAP + 2 + h;
  }
  else { 
    maxwid = 0;

    for (i = 0; i < n; i++) {
      Hv_CharDimension(led->labels[i], led->font, &w, &h);
      maxwid = Hv_sMax(maxwid, w);
    }

    Item->area->height = n*(Hv_LEDHEIGHT + Hv_LEDGAP) + Hv_LEDGAP;
    Item->area->width = Hv_LEDWIDTH + Hv_LEDGAP + 2 + maxwid;
  }

  Hv_FixRectRB(Item->area);

  Item->standarddraw = Hv_DrawLEDItem;

  if (attributes[Hv_DOUBLECLICK].v == NULL)
    Item->doubleclick = Hv_EditLEDItem;

}

/*-------- Hv_MallocLED -------*/

static Hv_LEDData   *Hv_MallocLED(short num,
				  char **labels)

{
  int i;
  Hv_LEDData    *led;

  if (num <= 0)
    return NULL;

  led = (Hv_LEDData *)Hv_Malloc(sizeof(Hv_LEDData));

  led->numled = num;
  led->oncolor = Hv_lawnGreen;
  led->offcolor = Hv_navyBlue;
  led->neutralcolor = Hv_yellow;
  led->labels = (char **)Hv_Malloc(num*sizeof(char *));
  led->states = (short *)Hv_Malloc(num*sizeof(short));

  for (i = 0; i < num; i++) {
    led->states[i] = Hv_OFF;
    Hv_InitCharStr(&(led->labels[i]), labels[i]);
  }

  return led;
}


/*--------- Hv_GetLEDData -------*/

static Hv_LEDData   *Hv_GetLEDData(Hv_Item Item)

{
  if ((Item == NULL) || (Item->type != Hv_LEDBARITEM))
    return NULL;
  else
    return (Hv_LEDData *)(Item->data);
}


/**
 * Hv_DestroyLEDItem
 * @purpose Called when an Hv_LEDITEM is deleted. As
 * usual, it must only worry about the data field.
 * Everthing else is handled automatically.
 * @param   The Item to destroy.
 */

void  Hv_DestroyLEDItem(Hv_Item Item) {

  Hv_LEDData   *led;
  int          i;

  if ((Item == NULL) || (Item->type != Hv_LEDBARITEM))
    return;

  led = Hv_GetLEDData(Item);
  for (i = 0; i < led->numled; i++) {
    Hv_Free(led->labels[i]);
  }

  Hv_Free(led->states);
  Hv_Free(led);
  Item->data = NULL;
}


/* ------ Hv_EditLEDColor --- */

static void Hv_EditLEDColor(Hv_Widget w)

{
  if (w == oncolor)
    Hv_ModifyColor(w, &newoncolor, "Edit LED \"ON\" color", False);
  else if (w == offcolor)
    Hv_ModifyColor(w, &newoffcolor, "Edit LED \"OFF\" color", False);
  else if (w == neutralcolor)
    Hv_ModifyColor(w, &newneutralcolor, "Edit LED \"NEUTRAL\" color", False);
  else if (w == bgcolor)
    Hv_ModifyColor(w, &newbgcolor, "Edit LED background color", False);
}

#undef Hv_LEDWIDTH
#undef Hv_LEDHEIGHT
#undef Hv_LEDGAP





