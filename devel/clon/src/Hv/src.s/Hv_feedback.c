/**
 * <EM>Deals with creating, drawing,
 * etc for Hv's pointer tracking feedback.</EM>
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

#include "Hv.h"	                 /* contains all necessary include files */

#define   Hv_UPDLABEL   1
#define   Hv_UPDDATA    2
#define   Hv_UPDBOTH    3

/*---- local prototypes -----*/

static void            Hv_UpdateFeedbackTextAndData(Hv_Item, 
                                                    Hv_FeedbackPtr,
                                                    int);

static void            Hv_FreeFeedback(Hv_FeedbackPtr *);

static Hv_FeedbackPtr  Hv_MallocFeedback(short,
                                         char *,
                                         short,
                                         short,
                                         short);

static Hv_FeedbackPtr  Hv_GetFeedbackPtr(Hv_Item,
                                         short,
                                         Hv_FeedbackPtr *);

static void            Hv_ResetFeedbackSizes(Hv_Item);

static void            Hv_DrawFeedbackText(Hv_String *,
                                           Hv_Rect *,
                                           Hv_Rect *,
                                           short *,
                                           short *);

static void            Hv_GetFeedbackRect(Hv_Item,
                                          Hv_FeedbackPtr,
                                          Hv_Rectangle *);


short  fbfillcolor = 0;

/**
 * Hv_DrawFeedbackItem
 * @purpose  Draw the feedback item.
 * @param  Item    The feedback item.
 * @param  region  The clipping region.
 */

void  Hv_DrawFeedbackItem(Hv_Item   Item,
			  Hv_Region region) {

/* draw the feedback item */

  Hv_Rect             *area;
  Hv_ColorScheme      *scheme;
  Hv_FeedbackPtr      temp;
  short               yt;
  short               r;

  Hv_Region           totrgn = NULL;
  Hv_Rect             rect, psrect;

  area   = Item->area;
  scheme = Item->scheme;
  totrgn = region;

  if (Hv_inPostscriptMode) {
    Hv_CopyRect(&rect, area);
    Hv_RectWithinContainer(Item->view, &rect);
    Hv_LocalRectToPSLocalRect(&rect, &psrect);
    Hv_PSClipRect(&psrect);
  }
  else {
    if (Hv_CheckBit(Item->view->drawcontrol, Hv_SCROLLABLE))
      totrgn = Hv_ItemDrawingRegion(Item, -1, -1);
    Hv_SetClippingRegion(totrgn);
  }

  if (Hv_inPostscriptMode)
    Hv_PSComment("Set Clip in DrawFeedback");


  Hv_Simple3DRect(area, False, scheme->fillcolor);
  
  if (Hv_inPostscriptMode)
    Hv_PSComment("FrameRect in DrawFeedback");

/* draw the separators */

  for (temp = (Hv_FeedbackPtr)(Item->data); temp != NULL; temp=temp->next)
    if (temp->type == Hv_FEEDBACKSEPARATOR) {
      r = area->left + area->width;
      yt = area->top + temp->labelrect->top+2;
      Hv_DrawLine((short)(area->left+4), yt,   (short)(r-4), yt,   Hv_white);
      Hv_DrawLine((short)(area->left+4), (short)(yt+1), (short)(r-4), (short)(yt+1), Hv_black);
    }

  Hv_SetBackground(Hv_colors[scheme->fillcolor]);
  Hv_UpdateFeedbackText(Item);

  if (totrgn != region)
    Hv_DestroyRegion(totrgn);

  Hv_RestoreClipRegion(region);

}

/**
 * Hv_FeedbackInitialize
 * @purpose  Initialize the feedback item.
 * @param  Item       The feedback item.
 * @param  attributes The attribute array.
 */

void  Hv_FeedbackInitialize(Hv_Item           Item,
			                      Hv_AttributeArray attributes) {

  Hv_View     View = Item->view;
  int         i;

  if (View->feedbackitem != NULL) {
    Hv_Warning("Hv FATAL ERROR: Only one feedback per view is supported.");
    exit (-1);
  }

  View->feedbackitem = Item;
  Item->user1 = attributes[Hv_FEEDBACKDATACOLS].s;
  Item->standarddraw = Hv_DrawFeedbackItem;
  Hv_DefaultSchemeIn(&(Item->scheme));
  Item->scheme->fillcolor -= 1;

/* see if a different fill color is desired */

  if (attributes[Hv_FILLCOLOR].s != Hv_DEFAULT)
    Item->scheme->fillcolor = attributes[Hv_FILLCOLOR].s;

/* now add any auto entries */

  for (i = 0; i < Hv_vaNumOpt; i++) {
    if (Hv_vaTags[i] == Hv_SEPARATORTAG) 
      Hv_AddFeedbackSeparator(Item);
    else
      Hv_AddFeedbackEntry(Item,
			  (short)(Hv_vaTags[i]),
			  Hv_vaText[i],
			  (short)(Hv_vaFonts[i]),
			  (short)(Hv_vaColors[i]));  
  }
}


/**
 * Hv_UpdateFeedbackText
 * @purpose  Refreshes all the feedback entries.
 * @param  fb       The feedback item.
 */

void Hv_UpdateFeedbackText(Hv_Item fb) {

  short   left = -32000;

  Hv_FeedbackPtr   temp;

  if ((fb == NULL) || Hv_CheckItemDrawControlBit(fb, Hv_DONTDRAW))
    return;

/*
 * Align all rects to compensate for proportional fonts.
 */

   for (temp = (Hv_FeedbackPtr)(fb->data); temp != NULL; temp=temp->next) {
     if (temp->type == Hv_FEEDBACKTEXT) {
       left = Hv_sMax(left, temp->datarect->left);
     }
   }


   for (temp = (Hv_FeedbackPtr)(fb->data); temp != NULL; temp=temp->next)
     if (temp->type == Hv_FEEDBACKTEXT) {
       temp->datarect->left = left;
       Hv_FixRectWH(temp->datarect);
       Hv_UpdateFeedbackTextAndData(fb, temp, Hv_UPDBOTH);
     }
}

/**
 * Hv_RefreshFeedbackRegion
 * @purpose  Refresh a region the feedback item.
 * @param  fb       The feedback item.
 * @param  BigBox
 */

void Hv_RefreshFeedbackRegion(Hv_Item fb,
			      Hv_Rect *BigBox) {

/* blanks out the difference before and the current fb_region */
/* returns the feedback region */

  Hv_Region        reg;
  Hv_Rect          fbrect;
  Hv_Rect          *area;
  Hv_View          View = Hv_GetItemView(fb);

  if ((fb == NULL) || Hv_CheckItemDrawControlBit(fb, Hv_DONTDRAW))
    return;

  area = fb->area;
  
  if (Hv_ViewIsVisible(View))
    if (BigBox->height > area->height) {
      fbrect.left   = area->left;
      fbrect.top    = area->top + area->height - 1;
      fbrect.width  = area->width + 1;
      fbrect.height = BigBox->height - area->height + 2;
      reg = Hv_RectRegion(&fbrect);
      Hv_DrawView(View, reg);
      Hv_DestroyRegion(reg);
    }
}

/* ------ Hv_UpdateFeedbackArea ------ */

void   Hv_UpdateFeedbackArea(Hv_Item  fb)

/* this was extracted from ResetFeedbackSizes below
   so that multiple feedback items could be added folowed by
   a single update, rather than mutiple updates leading
   to a "flashing" phenomenon */

{
  if (!Hv_ViewIsVisible(fb->view))
    return;

  if ((fb == NULL) || Hv_CheckItemDrawControlBit(fb, Hv_DONTDRAW))
    return;

  Hv_DrawItem(fb, NULL);
}

/* ------ Hv_AddFeedbackSeparator ---------- */

void   Hv_AddFeedbackSeparator(Hv_Item fb)

{
  Hv_FeedbackPtr  temp;
  
  if (fb == NULL) {
    Hv_Println("Hv warning: cannot add feedback data to a view with no feedback item.");
    return;
  }
  
/* Add a feedback item with the specified information to the end of the list */

  if (fb->data == NULL)
    fb->data = (void *)Hv_MallocFeedback(-32765, NULL, -1, -1, (short)(fb->user1));
  else {
    for (temp = (Hv_FeedbackPtr)(fb->data); temp->next != NULL; temp = temp->next)
      ;
    temp->next = Hv_MallocFeedback(-32765, NULL, -1, -1, (short)(fb->user1));
  }
  
  Hv_ResetFeedbackSizes(fb);
}


/* ------ Hv_AddFeedbackEntry ---------- */

void Hv_AddFeedbackEntry(Hv_Item fb,
			 short   tag,
			 char   *text,
			 short   textfont,
			 short   color)

/*
Hv_Item   fb
short     tag        add feedback item with this tag
char      *text      Text string of feedback
short     textfont
short     color      color of text
*/

{
  Hv_FeedbackPtr  temp;
  
  if (fb == NULL) {
    Hv_Println("Hv warning: cannot add feedback data to a view with no feedback item.");
    return;
  }
  
/* Add a feedback item with the specified information to the end of the list */

  if (fb->data == NULL)
    fb->data = (void *)Hv_MallocFeedback(tag, text, textfont, color, (short)(fb->user1));
  else {
    for (temp = (Hv_FeedbackPtr)(fb->data); temp->next != NULL; temp = temp->next)
      ;
    temp->next = Hv_MallocFeedback(tag, text, textfont, color, (short)(fb->user1));
  }
  
  Hv_ResetFeedbackSizes(fb);
}

/*------ Hv_DestroyFeedbackItem -------*/

void   Hv_DestroyFeedbackItem(Hv_Item fb)

{
  Hv_FeedbackPtr   temp, next;

  if (fb == NULL)
    return;
  
  for (temp = (Hv_FeedbackPtr)(fb->data); temp != NULL; temp = next) {
    next = temp->next;
    Hv_FreeFeedback(&temp);
  }

  fb->data = NULL;
}


/* ----- Hv_DeleteFeedbackEntry -------- */

void Hv_DeleteFeedbackEntry(Hv_Item fb,
			    short   tag)

{

/* delete a feedback item from the list --- deletes only the first
   one that it finds of that tag */


  Hv_FeedbackPtr       temp, prev;

  if (fb == NULL)
    return;

  temp = Hv_GetFeedbackPtr(fb, tag, &prev);
  if (temp != NULL) {
    if (prev == NULL)  /* deleting the first */
      fb->data = (void *)temp->next;
    else
      prev->next = temp->next;
    Hv_FreeFeedback(&temp);
  }
}

/*---------- Hv_ModifyFeedback ---------*/

/**
 * Hv_ModifyFeedback
 * @purpose   Modify a feedback entry.
 * @param     tag    Identifies which entry.
 * @param     label  New label text.
 * @param     data   New data text.
 */

void            Hv_ModifyFeedback(Hv_View View,
                                  short tag,
                                  char *label,
                                  char *data) {
                                  
                                  
  if (label != NULL)
    Hv_ChangeFeedbackLabel(View, tag, label);
  if (data != NULL)
    Hv_ChangeFeedback(View, tag, data);
}


/**
 * Hv_ChangeFeedback
 * @purpose   Changes just the data field of a feedback entry.
 * @param     tag    Identifies which entry.
 * @param     tstr   New data text.
 */

void Hv_ChangeFeedback (Hv_View View,
                        short   tag,
                        char   *tstr) {
  
  Hv_Item   fb = View->feedbackitem;
  
/* change the data field of a feedback item to reflect new information */
  
  Hv_FeedbackPtr temp, prev;
  
/* note that the number of columns is place in fb->user1 */
  
  temp = Hv_GetFeedbackPtr(fb, tag, &prev);
  if (temp == NULL)
    return;
  
  if (tstr == NULL)  {
    strcpy(temp->data->text, " ");
    return;
  }
  
/* do nothing if the strings are identical */
  
  if (strcmp(temp->data->text, tstr) != 0) {
    Hv_Free(temp->data->text);
    Hv_InitCharStr(&(temp->data->text), tstr);
    Hv_UpdateFeedbackTextAndData(fb, temp, Hv_UPDDATA);
  }
}

/**
 * Hv_ChangeFeedbackLabel
 * @purpose   Changes just the label field of a feedback entry.
 * @param     tag    Identifies which entry.
 * @param     label  New label text.
 */

void Hv_ChangeFeedbackLabel(Hv_View View,
                            short   tag,
                            char   *label) {
  Hv_Item   fb = View->feedbackitem;
  Hv_Rect   *lr;
  
  Hv_Region rgn = NULL;

/* change the label field of a feedback item to reflect new information */

  Hv_FeedbackPtr temp, prev;

  if (fb == NULL)
	  return;

/* note that the number of columns is place in fb->user1 */
  


  temp = Hv_GetFeedbackPtr(fb, tag, &prev);
  if (temp == NULL)
    return;

  if (label == NULL)  {
    strcpy(temp->label->text, " ");
    return;
  }

/* do nothing if the strings are identical */

  Hv_RemoveTrailingBlanks(label);
  
  rgn = Hv_ItemDrawingRegion(fb, -1, -1);
  Hv_SetClippingRegion(rgn);
  
  if (strcmp(temp->label->text, label) != 0) {
    Hv_Free(temp->label->text);
    Hv_InitCharStr(&(temp->label->text), label);
    lr = temp->labelrect;
    Hv_FillRectangle((short)(fb->area->left + lr->left),
      (short)(fb->area->top + lr->top),
      lr->width, lr->height, fb->scheme->fillcolor);
    Hv_UpdateFeedbackTextAndData(fb, temp, Hv_UPDLABEL);
  }
  
  Hv_DestroyRegion(rgn);
}



/**
 * Hv_GetFeedbackRect
 * @purpose   Get the rectangle used for drawing the data field. This rect
 *            is in ABSOLUTE (root) coordinates. Thus, it compensates for
 *            the fact that the rects in the feedback data structure are
 *            stored relative to the feedback item area.
 * @param     Item    The feedback item.
 * @param     temp    Feedback data pointer.
 * @param     rect    Upon return, holds the rectangle.
 * @local
 */

static void  Hv_GetFeedbackRect(Hv_Item        Item,
                                Hv_FeedbackPtr temp,
                                Hv_Rectangle   *rect)
                                

{
  if (temp->type != Hv_FEEDBACKTEXT)
    return;

  rect->x =      temp->datarect->left;
  rect->y =      temp->datarect->top;
  rect->width =  temp->datarect->width;
  rect->height = temp->datarect->height;

/* compensate for the fact that the feedback items are stored
   relative to the feedback area */

  rect->x += Item->area->left;
  rect->y += Item->area->top;
}

/**
 * Hv_UpdateFeedbackTextAndData
 * @purpose   Update Feedback Text and Data
 * @param     fb   The FeedBack Item.
 * @param     temp Pointer to particular feedback data.
 * @param     what Option specifier, can be Hv_UPDLABEL or Hv_UPDBOTH.
 * @local
 */

static void Hv_UpdateFeedbackTextAndData(Hv_Item        fb,
                                         Hv_FeedbackPtr temp,
                                         int            what) 
{

/* refreshes a particular feedback item. if 
   Both = false, update data only */


  Hv_Region           reg1;
  Hv_Rectangle        rect;
  short               x, y, oldw, neww, newh;
  Hv_Rect             *area;
  Hv_Rect             trect;
  Hv_ColorScheme      *scheme;
  short               fw, savecolor, savefont;
  
  if (temp->type != Hv_FEEDBACKTEXT)
    return;
  
  if ((fb == NULL) || Hv_CheckItemDrawControlBit(fb, Hv_DONTDRAW))
    return;
  
  if (fb->view != Hv_virtualView) {
    Hv_CopyRect(&trect, fb->area);
    
    Hv_RectWithinContainer(fb->view, &trect);
    if ((trect.width < 8) || (trect.height < 8))
      return;	
  }
  
  
  area   = fb->area;
  scheme = fb->scheme;
  fw = scheme->framewidth + scheme->borderwidth;
  
  reg1 = Hv_ItemDrawingRegion(fb, fw, fw);
  
  if (Hv_EmptyRegion(reg1)) {
    Hv_DestroyRegion(reg1);
    return;
  }
  
  Hv_SetClippingRegion(reg1);              /*set clipping region*/
  
  
  Hv_SetBackground(Hv_colors[scheme->fillcolor]);
  fbfillcolor = scheme->fillcolor;
  
  if ((what == Hv_UPDBOTH) || (what == Hv_UPDLABEL)) {
    if (temp->label != NULL) {
      savecolor = temp->label->strcol;
      if (Hv_globalFeedbackLabelColor >= 0)
        temp->label->strcol = Hv_globalFeedbackLabelColor;
      
      savefont = temp->label->font;
      if (Hv_globalFeedbackDataFont >= 0)
        temp->label->font = Hv_globalFeedbackDataFont;
      
      Hv_DrawFeedbackText(temp->label, temp->labelrect, area, &neww, &newh);
      
      temp->label->strcol = savecolor;
      temp->label->font = savefont;
    }
  }
  
  
  if (what == Hv_UPDLABEL) {
    Hv_DestroyRegion(reg1);
    return;
  }
  
  Hv_GetFeedbackRect(fb, temp, &rect); 
  
  /* erase the part of the rectangle that feedback (data)  won't be drawn in */
  
  oldw = rect.width;   /*old width*/
  x = rect.x;
  y = rect.y;
  
  
  if (temp->data != NULL) {
    savefont = temp->data->font;
    if (Hv_globalFeedbackDataFont >= 0)
      temp->data->font = Hv_globalFeedbackDataFont;
    
    Hv_DrawFeedbackText(temp->data, temp->datarect, area, &neww, &newh);
    temp->data->font = savefont;
  }
  
  
  if (oldw > neww)
    Hv_FillRectangle((short)(x + neww), 
    y,
    (short)(oldw - neww),
    newh, scheme->fillcolor);
  
  Hv_DestroyRegion(reg1);
  
  Hv_SetForeground(Hv_mainFg);
  Hv_SetBackground(Hv_colors[Hv_canvasColor]);
}

/* ------ Hv_FreeFeedback --------- */

static void Hv_FreeFeedback(Hv_FeedbackPtr *fbdata)

/* deallocate the space for a feedback item */

{
  Hv_Free((*fbdata)->labelrect);
  
  if ((*fbdata)->type == Hv_FEEDBACKTEXT) {
    Hv_Free((*fbdata)->datarect);
    Hv_DestroyString((*fbdata)->label);
    Hv_DestroyString((*fbdata)->data);
  }
  Hv_Free(*fbdata);
  *fbdata = NULL;
}


/* ------ Hv_MallocFeedback ------- */

static Hv_FeedbackPtr Hv_MallocFeedback(short tag,
					char  *text,
					short textfont,
					short color,
					short datacols)

{

/* allocate the space for a feedback item */

  Hv_FeedbackPtr   temp;
  short            w1, w2, h, hh;
  short            fw1, fw2, fh;
  char             *text1;
  int              i;

  temp = (Hv_FeedbackPtr)Hv_Malloc(sizeof(Hv_Feedback));
  temp->next = NULL;
  temp->tag = tag;
  temp->labelrect = Hv_NewRect();

  if ((textfont < 0) || (color < 0)) {
    temp->type = Hv_FEEDBACKSEPARATOR;
    temp->datarect = NULL;
    temp->label = NULL;
    temp->data = NULL;
    Hv_SetRect(temp->labelrect, 0, 0, 1, 6);
  }
  else {
    temp->type = Hv_FEEDBACKTEXT;
    text1 = (char *)Hv_Malloc(datacols+1);

    for (i = 0; i < datacols; i++)
      text1[i] = '0';

    text1[datacols] = '\0';

    temp->datarect = Hv_NewRect();
  
    temp->label = Hv_CreateString(text);
    temp->data = Hv_CreateString(text1);

    temp->label->strcol = color;
    temp->data->strcol = color;


    temp->label->font = Hv_fixed2;
    temp->data->font = Hv_fixed2;

	Hv_CompoundStringDimension(temp->label, &fw1, &fh);
    Hv_CompoundStringDimension(temp->data, &fw2, &fh);

    temp->label->font = textfont;
    temp->data->font = textfont;

    
	Hv_CompoundStringDimension(temp->label, &w1, &h);
    Hv_CompoundStringDimension(temp->data, &w2, &h);

	w1 = Hv_sMax(w1, fw1);
	w2 = Hv_sMax(w2, fw2);
	h = Hv_sMax(h, fh);

    if (Hv_root_width > 1025)
      hh = h-1;
    else
      hh = h-2;

    temp->labelrect->width  = w1;
    temp->labelrect->height = hh;
    temp->datarect->width  = w2;
    temp->datarect->height = hh;

    strcpy(temp->data->text, (char *)" ");
    Hv_Free(text1);
  }
  return temp;
}


/* ------ Hv_ResetFeedbackSizes ------- */

static void Hv_ResetFeedbackSizes(Hv_Item fb)

{
  Hv_FeedbackPtr temp, first;
  short           yt = 5;
  short           r;
  Hv_Rect         *labelrect, *datarect;

  if (fb == NULL)
    return;

  first = (Hv_FeedbackPtr)(fb->data);
  if (first == NULL)
    return;

  for (temp = first; temp != NULL; temp=temp->next) {
    labelrect = temp->labelrect;
    Hv_SetRect(labelrect, 5, yt, labelrect->width, labelrect->height);

    if (temp->type == Hv_FEEDBACKTEXT) {
      datarect = temp->datarect;
      r = labelrect->left + labelrect->width;
      Hv_SetRect(datarect,
		 (short)(r+3),
		 labelrect->top,
		 datarect->width,
		 labelrect->height);
      fb->area->width  = Hv_sMax(fb->area->width,
				 (short)(labelrect->width+datarect->width+15));
    }

    yt += (labelrect->height+1);
  }
  
  fb->area->height = yt+5;
  Hv_FixRectRB(fb->area);
}

/*------- Hv_GetFeedbackPtr --------*/

static Hv_FeedbackPtr  Hv_GetFeedbackPtr(Hv_Item       fb,
					 short          tag,
					 Hv_FeedbackPtr *prev)

{
  Hv_FeedbackPtr     temp;

  *prev = NULL;
  temp = (Hv_FeedbackPtr)(fb->data);

  while (temp != NULL) {
    if (temp->tag == tag)
      return temp;
    else {
      *prev = temp;
      temp = temp->next;
    }
  }
  
  return  NULL;
}


/*------- Hv_DrawFeedbackText ----------- */

static void  Hv_DrawFeedbackText(Hv_String *str,
				 Hv_Rect   *srect,
				 Hv_Rect   *area,
				 short     *w,
				 short     *h)
{

/* draws the feedback text pointed to by the str */

  Hv_CompoundString   cs;
  unsigned long       oldfg;
  short               sb;


  oldfg = Hv_SwapForeground(str->strcol);

  Hv_RemoveTrailingBlanks(str->text);
  cs = Hv_CreateSimpleCompoundString(str->text);

  Hv_CompoundStringDimension(str, w, h);

/*modify the srect to just enclose the cuurent string*/

  srect->width = *w;
  srect->height = *h;
  Hv_FixRectRB(srect);
      
/* note that the feedback string's srect is relative to the feedbacks area */

  sb = srect->top + srect->height;
  if (Hv_inPostscriptMode)
    Hv_DrawCompoundString((short)(area->left + srect->left),
			  (short)(area->top + sb - *h), str);
  else {    
    Hv_useDrawImage = True;
    Hv_DrawCompoundString((short)(area->left + srect->left),
			  (short)(area->top + sb - *h), str);
    Hv_useDrawImage = False;
  }
  Hv_CompoundStringFree(cs);
  Hv_SetForeground(oldfg);

}

#undef   Hv_UPDLABEL
#undef   Hv_UPDDATA
#undef   Hv_UPDBOTH




