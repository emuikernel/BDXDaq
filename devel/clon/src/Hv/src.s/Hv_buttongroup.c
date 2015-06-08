/**
 * <EM>Functions used to support ButtonGroups.</EM>
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

#include "Hv.h"


/* ------ local prototypes --------*/


static void Hv_ButtonGroupPosButtonEffect(Hv_Item  Item,
					  int      pos,
					  Hv_Event hvevent);

static void Hv_ButtonGroupPosUpdate(Hv_Item Item,
			    int     pos);

static int Hv_ButtonGroupXYToIndex(Hv_Item Item,
				   short x,
				   short y);


static void Hv_ButtonGroupPosDrawButton(Hv_Item Item,
					Hv_Region region,
					Hv_ButtonGroupData *bgd,
					int pos);

static void Hv_ButtonGroupOffset3DRect(Hv_Item Item,
				       Hv_Rect *rect,
				       Boolean outsie,
				       short   color);

static void  Hv_ButtonGroupSingleClick(Hv_Event hvevent);

static short Hv_ButtonGroupStringWidth(Hv_Item Item,
				       int     pos);

static void Hv_ButtonGroupConfigure(Hv_Item Item);


static void Hv_DrawButtonGroupItem(Hv_Item     Item,
				   Hv_Region   region);

/**
 * Hv_ButtonGroupMouseMotion
 * @purpose Used to track the pointer so that the color of the
 * buttons font can change as the mouce moves over it.
 * @param Item    The ButtonGroup Item.
 * @param x       Pointer horizontal pixel location.
 * @param x       Pointer vertical pixel location.
 */

void Hv_ButtonGroupMouseMotion(Hv_Item Item,
			       short   x,
			       short   y) {

    int oldindex, index;
    Hv_ButtonGroupData  *bgd;

    if (Item == NULL)
	return;

    bgd = Hv_ButtonGroupGetData(Item);
    if (bgd == NULL)
	return;

/*
 * offset to relative top left of item
 */

    x = x - Item->area->left;
    y = y - Item->area->top;

    index = Hv_ButtonGroupXYToIndex(Item, x, y);
    
    if (index == bgd->hotindex)
	return;

    oldindex = bgd->hotindex;
    bgd->hotindex = index;

    Hv_ButtonGroupPosUpdate(Item, oldindex+1);

    Hv_ButtonGroupPosUpdate(Item, index+1);
}


/**
 * Hv_ButtonGroupChangeLabel
 * @purpose Change the label of one of the buttons on the ButtonGroup.
 * @param Item    The ButtonGroup Item.
 * @param pos     The 1-Based index [1..N].
 * @param label   The new label.
 */

void Hv_ButtonGroupChangeLabel(Hv_Item Item,
			       int     pos,
			       char   *label) {

  Hv_Rect   rect;
  Hv_Region region = NULL;

  int cindex = pos-1;
  Hv_ButtonGroupData *bgd = Hv_ButtonGroupGetData(Item);
  if ((bgd == NULL) || (pos > bgd->nbutton)  || (pos < 1))
    return;
  
  Hv_Free(bgd->label[cindex]);
  Hv_InitCharStr(&(bgd->label[cindex]), label);
  
  Hv_ButtonGroupConfigure(Item);
  
  Hv_CopyRect(&rect, Item->area);

  if (bgd->orientation == Hv_HORIZONTAL) {
    rect.right = 5000;
  }
  else {
    rect.bottom = 5000;
  }

  Hv_FixRectWH(&rect);
  region = Hv_RectRegion(&rect);
  Hv_DrawView(Item->view, region);
  Hv_DestroyRegion(region);
}


/**
 * Hv_ButtonGroupExit
 * @purpose Called when the pointer leaves the confines of the ButtonGroup.
 * @param Item    The ButtonGroup Item.
 */

void Hv_ButtonGroupExit(Hv_Item Item) {


    Hv_ButtonGroupData *bgd = Hv_ButtonGroupGetData(Item);
    int  oldindex;


    if (bgd == NULL)
	return;

    oldindex = bgd->hotindex;
    bgd->hotindex = -1;
    Hv_ButtonGroupPosUpdate(Item, oldindex+1);
}


/**
 * Hv_ButtonGroupInitialize
 * @purpose Use the attributes array to initialize the ButtonGroup.
 * @param Item         The ButtonGroup Item.
 * @param attributes   The attributes array.
 */

void Hv_ButtonGroupInitialize(Hv_Item      Item,
			      Hv_AttributeArray attributes) {

    short                i;
    Hv_ButtonGroupData  *bgd;
    char                **sa;
    Hv_FunctionPtr      *fp;

    Hv_DefaultSchemeOut(&(Item->scheme));

    Item->standarddraw = Hv_DrawButtonGroupItem;

/* if no single click provided, give the default */

  if (attributes[Hv_SINGLECLICK].v == NULL)
    Item->singleclick = Hv_ButtonGroupSingleClick;

  if (attributes[Hv_FILLCOLOR].s != Hv_DEFAULT)
    Item->scheme->fillcolor = attributes[Hv_FILLCOLOR].s;

/*
 * create the data structure
 */

    bgd = (Hv_ButtonGroupData *)Hv_Malloc(sizeof(Hv_ButtonGroupData));
    Item->data = (void *)bgd; 

/*
 * get the number of buttons and other scalar attributes and create the arrays
 */


    bgd->orientation = (int)attributes[Hv_ORIENTATION].s;
    bgd->nbutton     = (int)attributes[Hv_NUMITEMS].s;
    bgd->pad         = (attributes[Hv_PADBUTTON].i != 0);
    bgd->gap         = (int)attributes[Hv_GAP].s;
    bgd->alignment   = (int)attributes[Hv_ALIGNMENT].s;
    bgd->font        = attributes[Hv_FONT].s;
    bgd->hotindex    = -1;

    bgd->fg = Hv_black;
    bgd->hotcolor = Hv_blue;

    if (attributes[Hv_COLOR].s != Hv_DEFAULT)
	bgd->fg = attributes[Hv_COLOR].s;

    if (attributes[Hv_TEXTCOLOR].s != Hv_DEFAULT)
	bgd->fg = attributes[Hv_TEXTCOLOR].s;
	
    if (attributes[Hv_HOTCOLOR].s != Hv_DEFAULT)
	bgd->hotcolor = attributes[Hv_TEXTCOLOR].s;
	


    if ((bgd->nbutton < 1) || (bgd->nbutton > 256)) {
	Hv_Println("Button group has bad number of buttons: %d", bgd->nbutton);
	return;
    }



    bgd->label = (char **)Hv_Malloc(bgd->nbutton * sizeof (char *));
    bgd->rect  = (Hv_Rect *)Hv_Malloc(bgd->nbutton * sizeof (Hv_Rect));
    bgd->enabled = (Boolean *)Hv_Malloc(bgd->nbutton * sizeof (Boolean));
    bgd->state = (int *)Hv_Malloc(bgd->nbutton * sizeof (int));
    bgd->callback = (Hv_FunctionPtr *)Hv_Malloc(bgd->nbutton * sizeof (Hv_FunctionPtr));
    

/*
 * Initialize the arrays
 */


   sa = (char **)attributes[Hv_STRINGARRAY].v;
   fp = (Hv_FunctionPtr *)attributes[Hv_CALLBACKARRAY].v;


   for (i = 0; i < bgd->nbutton; i++) {

       if ((sa == NULL) || (sa[i] == NULL))
	   Hv_InitCharStr(&(bgd->label[i]), " ");
       else
	   Hv_InitCharStr(&(bgd->label[i]), sa[i]);


       bgd->callback[i] = fp[i];
       bgd->enabled[i] = True;   /*default: is selectable*/
       bgd->state[i] = Hv_OFF;
   }


/*
 * Configure the buttongroup
 */


   Hv_ButtonGroupConfigure(Item);

}



/*-----------------------------------------------------------
 *  Hv_ButtonGroupSingleClick
 *
 * Figures out which button was hit and calls the appropriate
 * callback
 *-----------------------------------------------------------*/

static void  Hv_ButtonGroupSingleClick(Hv_Event hvevent)

{
    Hv_Item  Item = hvevent->item;
    short    x, y;
    int      index;
    Hv_ButtonGroupData  *bgd;

    bgd = Hv_ButtonGroupGetData(Item);
    if (bgd == NULL)
	return;

    x = hvevent->where.x - Item->area->left;
    y = hvevent->where.y - Item->area->top;

    index = Hv_ButtonGroupXYToIndex(Item, x, y);

    if ((index >= 0) && (index < bgd->nbutton))
	if (!(bgd->enabled[index]))
	    index = -1;

    if ((index >= 0) && (index < bgd->nbutton))
	Hv_ButtonGroupPosButtonEffect(Item, index+1, hvevent);


}

/*-----------------------------------------------------------
 *  Hv_ButtonGroupConfigure
 *
 * gets the boundary based on the orientation, gap, stringwidts etc.
 * also sets the button rects. All rects are RELATIVE to the top left
 * of the area
 *-----------------------------------------------------------*/

static void Hv_ButtonGroupConfigure(Hv_Item Item) {

    int     nb;
    short  *sw = NULL;
    short   maxw, cw, w, h, l, t;
    int     i;
    Hv_Rect *area;
    short   extrax = 0;


    Hv_ButtonGroupData *bgd = Hv_ButtonGroupGetData(Item);


    if (bgd == NULL)
	return;

    nb = bgd->nbutton;
    area = Item->area;


/* now deal with orientation */

    if (bgd->orientation == Hv_HORIZONTAL) {


	sw = (short *)Hv_Malloc(nb*sizeof(short));


	if (bgd->pad) { /* all buttons have the same width */
	    maxw = Hv_ButtonGroupStringWidth(Item, -1);
	    w = nb*maxw;
	    for (i = 0; i < nb; i++)
		sw[i] = maxw;
	}
	
	else { /* variable widths */
	    w = 0;
	    for (i = 0; i < nb; i++) {
		sw[i] = Hv_ButtonGroupStringWidth(Item, i+1);
		w += sw[i];
	    }
	}
	
	w = w +  (nb)*(bgd->gap) + 1;

	if (bgd->alignment == Hv_CENTER) {
	  extrax = (Item->area->width - w)/2;
	}


	Hv_SetRect(&(bgd->boundary), extrax, 0, w, (short)(area->height-1));

/* now the individual rects */

	l = extrax + 1;
	t = 2;
	h = area->height-4;

	for (i = 0; i < nb; i++) {
	    cw = sw[i]+bgd->gap;

	    if (i == 0)
	      Hv_SetRect(bgd->rect+i, (short)(l+1), t, (short)(cw-3), h);
	    else
	      Hv_SetRect(bgd->rect+i, l, t, (short)(cw-2), h);
	    l += cw;
	}


	if (sw != NULL)
	    Hv_Free(sw);

    } /* end horiz */


    else { /*vertical */
/* not implemented yet */
    }




}


/*-----------------------------------------------------------
 *  Hv_ButtonGroupStringWidth
 *
 * pos is NOT a C Index, if it is < 1, return max sw
 *-----------------------------------------------------------*/

static short Hv_ButtonGroupStringWidth(Hv_Item Item,
				       int     pos) {

    int     i;
    short   w, h, maxw;

    Hv_ButtonGroupData *bgd = Hv_ButtonGroupGetData(Item);
    if ((bgd == NULL) || (pos > bgd->nbutton))
	return 0;
    

    if (pos < 0) {
	maxw = 0;

	for (i = 0; i < bgd->nbutton; i++) {
	    Hv_CharDimension(bgd->label[i],
			     bgd->font,
			     &w, &h);
	    maxw = Hv_sMax(maxw, w);
	}

	return maxw;
    }

    Hv_CharDimension(bgd->label[pos-1],
		     bgd->font,
		     &w, &h);
    
    return w;
    
}



/*-----------------------------------------------------------
 *  Hv_ButtonGroupLabelToPos
 *
 * Returns the position (starting at 1) of the button
 * with the corresponding label, or -1 it it does not find one
 *-----------------------------------------------------------*/

int Hv_ButtonGroupLabelToPos(Hv_Item Item,
			     char    *label) {

    int i;
    Hv_ButtonGroupData *bgd = Hv_ButtonGroupGetData(Item);

    if ((bgd == NULL) || (label == NULL))
	return -1;

    for (i = 0; i < bgd->nbutton; i++) {
	if ((bgd->label[i] != NULL) && (strcmp(label, bgd->label[i]) == 0))
	    return (i+1);
    }

    return -1;

}



/*-----------------------------------------------------------
 *  Hv_ButtonGroupGetData
 *-----------------------------------------------------------*/

Hv_ButtonGroupData *Hv_ButtonGroupGetData(Hv_Item Item) {

    Hv_ButtonGroupData * bgd;;

    if ((Item == NULL) || (Item->type != Hv_BUTTONGROUPITEM))
	return NULL;

    bgd = (Hv_ButtonGroupData *)(Item->data);
    
    if (bgd == NULL) {
	Hv_Println("Bad ButtonGroup Data (null)");
	return NULL;
    }

    if ((bgd->nbutton < 1) || (bgd->nbutton > 256)) {
	Hv_Println("Bad ButtonGroup Data (nb = %d)", bgd->nbutton);
	return NULL;
    }

    return bgd;
}
 

/*-----------------------------------------------------------
 *  Hv_DrawButtonGroupItem
 *-----------------------------------------------------------*/

static void Hv_DrawButtonGroupItem(Hv_Item     Item,
				   Hv_Region   region) {

    Hv_ButtonGroupData *bgd = Hv_ButtonGroupGetData(Item);
    int i;
    Hv_Region reg1 = NULL;
    Hv_Region reg2 = NULL;
    Hv_Rect   trect;

    if (bgd == NULL)
	return;


	if ((Item == NULL) || !(Hv_ViewIsVisible(Item->view)))
		return;

/* clip against item's area */

    reg1 = Hv_RectRegion(Item->area);
    reg2 = Hv_IntersectRegion(reg1, region);

    Hv_SetClippingRegion(reg2);

    Hv_FillRect(&(bgd->boundary), Item->scheme->fillcolor);

    Hv_CopyRect(&trect, &(bgd->boundary));
    Hv_OffsetRect(&trect, Item->area->left, Item->area->top);
    Hv_FillRect(&trect, (short)(Item->scheme->fillcolor-2));

    trect.width -= 1;
    trect.height -= 1;
    Hv_FixRectRB(&trect);

    Hv_OffsetRect(&trect, 1, 1);
    Hv_FrameRect(&trect, Hv_white);
    Hv_OffsetRect(&trect, -1, -1);
    Hv_FrameRect(&trect, Hv_black);

/*    Hv_ButtonGroupOffset3DRect(Item,
			       &(bgd->boundary),
			       False,
			       (short)(Item->scheme->fillcolor-2));*/

/* draw the individual buttons */

    for (i = 0; i < bgd->nbutton; i++) {
	Hv_ButtonGroupPosDrawButton(Item, reg2, bgd, i+1);
    }

    Hv_DestroyRegion(reg1);
    Hv_DestroyRegion(reg2);

    Hv_SetClippingRegion(region);

}


/*-----------------------------------------------------------
 *  Hv_ButtonGroupXYToIndex
 *
 *  returns C index [0..] or -1 on failure
 *-----------------------------------------------------------*/

static int Hv_ButtonGroupXYToIndex(Hv_Item Item,
				   short x,
				   short y) {

    Hv_Point             pp;
    Hv_ButtonGroupData  *bgd;
    int                  i;


    bgd = Hv_ButtonGroupGetData(Item);

    if (bgd == NULL)
	return -1;


    Hv_SetPoint(&pp, x, y);


    if (!Hv_PointInRect (pp, &(bgd->boundary)))
	return -1;

    for (i = 0; i < bgd->nbutton; i++) 
	if (Hv_PointInRect (pp, bgd->rect + i))
	    return i;
	

    return -1;
}



/*-----------------------------------------------------------
 *  Hv_ButtonGroupPosUpdate
 *-----------------------------------------------------------*/

static void Hv_ButtonGroupPosUpdate(Hv_Item Item,
			    int     pos) {

    Hv_ButtonGroupData  *bgd;

    Hv_Region reg = NULL;

    if ((Item == NULL) || (pos < 1))
	return;

    bgd = Hv_ButtonGroupGetData(Item);

    if ((bgd == NULL) || (pos > bgd->nbutton))
	return ;

    reg = Hv_GetItemDrawingRegion(Item, 0, 0);

    Hv_SetClippingRegion(reg);
    Hv_ButtonGroupPosDrawButton(Item, reg, bgd, pos);
    Hv_DestroyRegion(reg);
}


/*-----------------------------------------------------------
 *  Hv_ButtonGroupPosDrawButton
 *
 *  draws the individual button
 *-----------------------------------------------------------*/

static void Hv_ButtonGroupPosDrawButton(Hv_Item Item,
					Hv_Region region,
					Hv_ButtonGroupData *bgd,
					int pos) {


    int cindex = pos-1;
    Hv_String   *str;
    short        xmw, xmh, dummy;		        /* String width  in pixels */
    int          x, y;
    Hv_Rect      *rect;
    char        *teststr = "Xg";


    rect = &(bgd->rect[cindex]);

    str = Hv_CreateString(teststr);
    str->font = bgd->font;
    Hv_CompoundStringDimension(str, &dummy, &xmh);
    Hv_DestroyString(str);


    str = Hv_CreateString(bgd->label[cindex]);
    str->font = bgd->font;

    if ((bgd->enabled[cindex]) && (cindex == bgd->hotindex)) {
	Hv_ButtonGroupOffset3DRect(Item,
				   rect,
				   bgd->state[cindex] == Hv_OFF,
				   (short)(Item->scheme->fillcolor-3));
	str->strcol = bgd->hotcolor;
    }

    else {
	Hv_FillRectangle((short)(Item->area->left + rect->left),
			 (short)(Item->area->top + rect->top),
			 rect->width, (short)(rect->height-1),
			 (short)(Item->scheme->fillcolor-2));
	Hv_FrameRectangle((short)(Item->area->left + rect->left),
			  (short)(Item->area->top + rect->top),
			  rect->width, (short)(rect->height-1),
			  (short)(Item->scheme->fillcolor-2));
    }


    Hv_CompoundStringDimension(str, &xmw, &dummy);

    rect = bgd->rect + cindex;
    x = Item->area->left + rect->left + (rect->width - xmw)/2;
    y = Item->area->top + rect->top + (rect->height - xmh)/2;
/*    y = Item->area->top + rect->bottom - 1 - xmh; */
/*    y = Item->area->top + 5; */


    if (bgd->enabled[cindex])
	Hv_DrawCompoundString((short)x, (short)y, str);
    else
	Hv_DrawGhostedText(x, y, str, Hv_white, Hv_gray7);

    Hv_DestroyString(str);

}


/*-----------------------------------------------------------
 *  Hv_ButtonGroupPosButtonEffect
 *-----------------------------------------------------------*/


static void Hv_ButtonGroupPosButtonEffect(Hv_Item  Item,
					  int      pos,
					  Hv_Event hvevent) {
    
    Hv_ButtonGroupData *bgd = Hv_ButtonGroupGetData(Item);
    int cindex = pos-1;
    Hv_ButtonGroupCallbackData *cbd = NULL;

    if ((bgd == NULL) || (pos > bgd->nbutton) || (pos < 1))
	return;

    if (!(bgd->enabled[cindex]))
	return;
    
    Hv_ButtonGroupPosSetState(Item, pos, Hv_ON, True);

    Hv_Pause(100, NULL);

    if ((hvevent != NULL) && (bgd->callback[pos-1] != NULL)) {


	cbd = (Hv_ButtonGroupCallbackData *)Hv_Malloc(sizeof(Hv_ButtonGroupCallbackData));
	cbd->pos = pos;
	cbd->label = bgd->label[pos-1];
	hvevent->callbackdata = (void *)cbd;
	bgd->callback[pos-1](hvevent);
	Hv_Free(cbd);
	hvevent->callbackdata = NULL;
    }

    Hv_ButtonGroupPosSetState(Item, pos, Hv_OFF, True);
}


/*-----------------------------------------------------------
 *  Hv_ButtonGroupGetCallbackData
 *-----------------------------------------------------------*/

Hv_ButtonGroupCallbackData *Hv_ButtonGroupGetCallbackData(Hv_Event hvevent) {
    if (hvevent == NULL)
	return NULL;
    return (Hv_ButtonGroupCallbackData *)(hvevent->callbackdata);
}


/*-----------------------------------------------------------
 *  Hv_ButtonGroupOffset3DRect
 *
 * Accounts for fact that internal BG rects are relative
 *-----------------------------------------------------------*/

static void Hv_ButtonGroupOffset3DRect(Hv_Item Item,
				       Hv_Rect *rect,
				       Boolean outsie,
				       short   color) {


  Hv_Simple3DRectangle((short)(rect->left + Item->area->left),
		       (short)(rect->top + Item->area->top),
		       rect->width,
		       (short)(rect->height-1),
		       outsie,
		       color);
}


/*-----------------------------------------------------------
 * Hv_DestroyButtonGroup
 *-----------------------------------------------------------*/

void Hv_DestroyButtonGroup(Hv_Item Item) {

    int  i;
    Hv_ButtonGroupData *bgd = Hv_ButtonGroupGetData(Item);
    
    for (i = 0; i < bgd->nbutton; i++)
	Hv_Free(bgd->label[i]);

    Hv_Free(bgd->label);
    Hv_Free(bgd->rect);
    Hv_Free(bgd->enabled);
    Hv_Free(bgd->state);
    Hv_Free(bgd->callback);
    Hv_Free(bgd);
    Item->data = NULL;
}

/*-----------------------------------------------------------
 * Hv_ButtonGroupPosSetEnabled
 *
 * pos = [1,2,..] if pos < 0 apply to ALL  
 *-----------------------------------------------------------*/

void Hv_ButtonGroupPosSetEnabled(Hv_Item  Item,
				 int      pos,
				 Boolean  enab,
				 Boolean  redraw) {
    
    Hv_ButtonGroupData *bgd;
    int                 i;

    if (Item == NULL)
	return;

	redraw = (redraw && Hv_ViewIsVisible(Item->view));

    bgd = Hv_ButtonGroupGetData(Item);

    if ((bgd == NULL) || (pos > bgd->nbutton))
	return;

    if (pos < 1) { /* all */

	for (i = 0; i < bgd->nbutton; i++)
	    bgd->enabled[i] = enab;

	if (redraw)
	    Hv_DrawItem(Item, NULL);
    } /* end pos < 1 */
    else {
	bgd->enabled[pos-1] = enab;
	if (redraw)
	    Hv_ButtonGroupPosUpdate(Item, pos);
    }


    
}

/*-----------------------------------------------------------
 * Hv_ButtonGroupPosSetState  
 *-----------------------------------------------------------*/
    
void Hv_ButtonGroupPosSetState(Hv_Item  Item,
			       int      pos,
			       int      state,
			       Boolean  redraw) {

    Hv_ButtonGroupData *bgd;
    int                 i;

    if (Item == NULL)
	return;

    bgd = Hv_ButtonGroupGetData(Item);

    if ((bgd == NULL) || (pos > bgd->nbutton))
	return;

    if (pos < 1) {

	for (i = 0; i < bgd->nbutton; i++)
	    bgd->state[i-1] = state;

	if (redraw)
	    Hv_DrawItem(Item, NULL);
    }
    else {
	bgd->state[pos-1] = state;
	if (redraw)
	    Hv_ButtonGroupPosUpdate(Item, pos);
    }


 }

/*-----------------------------------------------------------
 * Hv_ButtonGroupToggleState  
 *-----------------------------------------------------------*/
    
void Hv_ButtonGroupToggleState(Hv_Item  Item,
			       int      pos) {

    Hv_ButtonGroupData *bgd = Hv_ButtonGroupGetData(Item);
}


