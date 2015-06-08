/**
 * <EM>Deals with the rainbow scale (which is placed on a <B>view</B>.</EM>
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

/*------ local prototypes -------*/

static void Hv_EditRainbow(Hv_Event     hvevent);

static void Hv_LimitedEditRainbow(Hv_Event hvevent);

static void Hv_SetRainbowLabels(Hv_Item  Item);

static void Hv_EditRBColor(Hv_Widget  w);


/*------ variables global for this file --------*/

static   short      newmincolor,   newmaxcolor;
static   Hv_Widget  mincolorlabel, maxcolorlabel;

/* ------------- Hv_DrawRainbowItem ------------*/

void Hv_DrawRainbowItem(Hv_Item Item,
			Hv_Region region)

{
  short                 i;
  Hv_Region             reg1, reg2;
  Hv_RainbowData        *data;
  short                 mincolor, maxcolor;
  Hv_Rect               *area;
  short                 steps, color, cstep, cwide;
  short                 l, t, b;
  Hv_Rect               rect;

  area   = Item->area;

  l = area->left;
  t = area->top;
  b = area->bottom;

  data = (Hv_RainbowData *) (Item->data);
 
  mincolor = data->mincolor;
  maxcolor = data->maxcolor;
  cwide = data->colorwidth;

  if (mincolor < maxcolor) {
    cstep = 1;
    steps = maxcolor - mincolor + 1;
  }
  else {
    cstep = -1;
    steps = mincolor - maxcolor + 1;
  }

  reg2 = Hv_RectRegion(area);
  reg1 = Hv_IntersectRegion(region, reg2);
  Hv_SetClippingRegion(reg1);
  Hv_DestroyRegion(reg1);  
  Hv_DestroyRegion(reg2);

  if (data->orientation == Hv_HORIZONTAL)
    Hv_SetRect(&rect, l, t, cwide, area->height);
  else
    Hv_SetRect(&rect, l, (short)(b - cwide), area->width, cwide);
  
  color = mincolor;
  for (i = 0; i < steps; i++, color += cstep) {
    Hv_FillRect(&rect, color);
    if (data->orientation == Hv_HORIZONTAL)
      Hv_OffsetRect(&rect, cwide, 0);
    else
      Hv_OffsetRect(&rect, 0, (short)(-cwide));
  }
    

  if (region != NULL)
    Hv_SetClippingRegion(region);
  Hv_Simple3DRect(area, False, -1);
  
}


/*-------- Hv_RainbowInitialize --------*/

void Hv_RainbowInitialize(Hv_Item      Item,
			  Hv_AttributeArray attributes)
{
  Hv_View              View = Item->view;
  Hv_RainbowData      *data;
  Hv_Item              parent;
  Boolean              inbox;
  short                boxfill = 0;
  int                  i;
  short                placement;         
  short                font, font2;

  Item->standarddraw = Hv_DrawRainbowItem;
  Hv_DefaultSchemeIn(&(Item->scheme));
  Item->scheme->fillcolor -= 1;
  Item->data = (void *) Hv_Malloc(sizeof(Hv_RainbowData));

  parent = Item->parent;

/* see if this rainbow is the child of a BOX parent */

  inbox = ((parent != NULL) &&
	   ((parent->type == Hv_BOXITEM) || (parent->type == Hv_RADIOCONTAINERITEM)));
  if (inbox)
    boxfill = parent->scheme->fillcolor;

  font = attributes[Hv_FONT].s;
  if (font < 0)
      font = Hv_fixed2;

  font2 = attributes[Hv_FONT2].s;
  if (font2 < 0)
      font2 = Hv_fixed2;

    
  data = (Hv_RainbowData *)(Item->data);

  data->titlefont = font;
  data->labelfont = font2;

  data->orientation = attributes[Hv_ORIENTATION].s;
  data->valmin = (short)attributes[Hv_MINVALUE].i;
  data->valmax = (short)attributes[Hv_MAXVALUE].i;
  data->mincolor = attributes[Hv_MINCOLOR].s;
  data->maxcolor = attributes[Hv_MAXCOLOR].s;
  data->colorwidth = attributes[Hv_COLORWIDTH].s;
  data->numlab = Hv_sMin(Hv_MAXRAINBOWLABEL, attributes[Hv_NUMLABELS].s);
  data->dx = Hv_sMax(2, attributes[Hv_PIXELDX].s);
  data->dy = Hv_sMax(2, attributes[Hv_PIXELDY].s);
  data->precision = attributes[Hv_PRECISION].s;
  data->scalefactor = (float)(attributes[Hv_SCALEFACTOR].f);
  data->limitededit = False;

  for (i = 0; i < Hv_MAXRAINBOWLABEL; i++)
    data->labels[i] = NULL;

  if ((attributes[Hv_EDITABLE].i != 0) && (Item->doubleclick == NULL)) {
    Item->doubleclick = Hv_EditRainbow;
    data->limitededit = (attributes[Hv_EDITABLE].i == Hv_LIMITEDEDIT);
  }

  if (data->orientation == Hv_HORIZONTAL) {
    Item->area->width = data->colorwidth*((short)(abs(data->maxcolor - data->mincolor)) + 1);
    Item->area->height = Hv_sMax(12, Item->area->height);
    placement = 2;
  }
  else {
    Item->area->height = data->colorwidth*((short)(abs(data->maxcolor - data->mincolor)) + 1);
    Item->area->width = Hv_sMax(12, Item->area->width);
    placement = 5;
  }
  
  Hv_FixRectRB(Item->area);
  
/* place the title string above regardless of whether it is horiz or vert */

 if ((char *)(attributes[Hv_TITLE].v) != NULL)
     data->titlestr = Hv_VaCreateItem(View,
				      Hv_TYPE,            Hv_TEXTITEM,
				      Hv_PARENT,          Item,
				      Hv_FONT,            data->titlefont,
				      Hv_TEXT,            (char *)(attributes[Hv_TITLE].v),
				      Hv_CHILDPLACEMENT,  attributes[Hv_TITLEPLACEMENT].i,
				      Hv_PLACEMENTGAP,    placement,
				      NULL);

/* if rainbow is in a box, need the proper background */

  if (inbox)
    data->titlestr->scheme->fillcolor = boxfill;


/* now the labels */

  Hv_SetRainbowLabels(Item);
}


/*-------- Hv_EditRainbow -------*/

static void Hv_EditRainbow(Hv_Event hvevent)

{
  Hv_View      View = hvevent->view;
  Hv_Item      Item = hvevent->item;

  static Hv_Widget     dialog = NULL;
  static Hv_Widget     dx, dy, min, max, precision, colorwidth;
  Hv_Widget            dummy, rowcol, rc;
  Hv_RainbowData      *data;
  Hv_Region            oldregion, newregion, totalregion;
  short                newsize, del;
  int                  answer;

  if (Item == NULL)
    return;

  data = (Hv_RainbowData *) (Item->data);

  if (data->limitededit) {
    Hv_LimitedEditRainbow(hvevent);
    return;
  }

  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " Rainbow Scale Editor ", NULL);

    rowcol = Hv_VaCreateDialogItem(dialog,
				   Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
				   Hv_SPACING,      10,
				   Hv_ORIENTATION,  Hv_VERTICAL,
				   Hv_ALIGNMENT,    Hv_CENTER,
				   NULL);

    rc = Hv_VaCreateDialogItem(rowcol,
			       Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
			       Hv_NUMROWS,      2,
			       Hv_PACKING,      Hv_PAD,
			       Hv_SPACING,      8,
			       NULL);

    mincolorlabel = Hv_SimpleColorLabel(rc, " min color ", (Hv_FunctionPtr)Hv_EditRBColor);
    maxcolorlabel = Hv_SimpleColorLabel(rc, " max color ", (Hv_FunctionPtr)Hv_EditRBColor);

    dummy = Hv_SimpleDialogSeparator(rowcol);


    rc = Hv_VaCreateDialogItem(rowcol,
			       Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
			       Hv_NUMROWS,      6,
			       Hv_PACKING,      Hv_PAD,
			       Hv_SPACING,      4,
			       NULL);

    precision  = Hv_SimpleTextEdit(rc, "Label Precision", NULL, 5);
    colorwidth = Hv_SimpleTextEdit(rc, "Color Pix Width", NULL, 5);
    min        = Hv_SimpleTextEdit(rc, "Scale Min Value", NULL, 5);
    max        = Hv_SimpleTextEdit(rc, "Scale Max Value", NULL, 5);
    dx         = Hv_SimpleTextEdit(rc, "X Pixel Spacing", NULL, 5);
    dy         = Hv_SimpleTextEdit(rc, "X Pixel Spacing", NULL, 5);

    dummy = Hv_StandardActionButtons(rowcol, 26, Hv_OKBUTTON + Hv_CANCELBUTTON + Hv_APPLYBUTTON);
  }

/* the dialog has been created */

  newmincolor = data->mincolor;
  newmaxcolor = data->maxcolor;

  Hv_ChangeLabelColors(mincolorlabel, -1, newmincolor);
  Hv_ChangeLabelColors(maxcolorlabel, -1, newmaxcolor);

  Hv_SetIntText(precision, data->precision);
  Hv_SetIntText(colorwidth, data->colorwidth);
  Hv_SetIntText(dx, data->dx);
  Hv_SetIntText(dy, data->dy);
  Hv_SetFloatText(min, ((float)(data->valmin))*data->scalefactor, data->precision);
  Hv_SetFloatText(max, ((float)(data->valmax))*data->scalefactor, data->precision);

  while ((answer = Hv_DoDialog(dialog, NULL)) > 0) {
    oldregion = Hv_CopyItemRegion(Item, True);

    data->mincolor = newmincolor;
    data->maxcolor = newmaxcolor;
    data->dx  = Hv_sMax(1, Hv_sMin(10, (short)Hv_GetIntText(dx)));
    data->dy  = Hv_sMax(1, Hv_sMin(10, (short)Hv_GetIntText(dy)));
    data->precision  = Hv_sMax(0, Hv_sMin(5, (short)Hv_GetIntText(precision)));
    data->colorwidth = Hv_sMax(1, Hv_sMin(10, (short)Hv_GetIntText(colorwidth)));
    data->valmin = (short)(Hv_GetFloatText(min)/data->scalefactor);
    data->valmax = (short)(Hv_GetFloatText(max)/data->scalefactor);

/* size change ? */

    newsize  = data->colorwidth*((short)(abs(data->maxcolor - data->mincolor)) + 1);
    if (data->orientation == Hv_HORIZONTAL) {
      if (newsize != Item->area->width) {
	del = (Item->area->width - newsize)/2;
	Item->area->width = newsize;
	Hv_FixRectRB(Item->area);
	Hv_OffsetItem(Item, del, 0, True); 
      }
    }
    else {
      if (newsize != Item->area->height) {
	del = (Item->area->height - newsize)/2;
	Item->area->height = newsize;
	Hv_FixRectRB(Item->area);
	Hv_OffsetItem(Item, 0, del, True); 
      }
    }

/* automaticaly redraw hotrect? */
    
    if (Hv_CheckBit(Item->drawcontrol, Hv_REDRAWHOTRECTWHENCHANGED))
      Hv_DrawViewHotRect(View);

    Hv_SetRainbowLabels(Item);

    newregion = Hv_CopyItemRegion(Item, True);
    totalregion = Hv_UnionRegion(oldregion, newregion);
    Hv_ShrinkRegion(totalregion, -2, -2);
    Hv_DrawView(Item->view, totalregion);

    Hv_DestroyRegion(oldregion);
    Hv_DestroyRegion(newregion);
    Hv_DestroyRegion(totalregion);

    if (answer != Hv_APPLY)
      break;
  }
}


/*-------- Hv_LimitedEditRainbow -------*/

static void Hv_LimitedEditRainbow(Hv_Event hvevent)

{
  Hv_View      View = hvevent->view;
  Hv_Item      Item = hvevent->item;

  static Hv_Widget     dialog = NULL;
  static Hv_Widget     min, max, precision;
  Hv_Widget            dummy, rowcol, rc;
  Hv_RainbowData      *data;
  Hv_Region            oldregion, newregion, totalregion;
  int                  answer;

  data = (Hv_RainbowData *) (Item->data);

  if (Item == NULL)
    return;

  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " Rainbow Scale Editor ", NULL);

    rowcol = Hv_VaCreateDialogItem(dialog,
				   Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
				   Hv_SPACING,      10,
				   Hv_ORIENTATION,  Hv_VERTICAL,
				   Hv_ALIGNMENT,    Hv_CENTER,
				   NULL);

    rc = Hv_VaCreateDialogItem(rowcol,
			       Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
			       Hv_NUMROWS,      3,
			       Hv_PACKING,      Hv_PAD,
			       Hv_SPACING,      4,
			       NULL);

    precision  = Hv_SimpleTextEdit(rc, "Label Precision", NULL, 5);
    min        = Hv_SimpleTextEdit(rc, "Scale Min Value", NULL, 5);
    max        = Hv_SimpleTextEdit(rc, "Scale Max Value", NULL, 5);

    dummy = Hv_StandardActionButtons(rowcol, 26, Hv_OKBUTTON + Hv_CANCELBUTTON + Hv_APPLYBUTTON);
  }

/* the dialog has been created */

  Hv_SetIntText(precision, data->precision);
  Hv_SetFloatText(min, ((float)(data->valmin))*data->scalefactor, data->precision);
  Hv_SetFloatText(max, ((float)(data->valmax))*data->scalefactor, data->precision);

  while ((answer = Hv_DoDialog(dialog, NULL)) > 0) {
    oldregion = Hv_CopyItemRegion(Item, True);

    data->precision  = Hv_sMax(0, Hv_sMin(5, (short)Hv_GetIntText(precision)));
    data->valmin = (short)(Hv_GetFloatText(min)/data->scalefactor);
    data->valmax = (short)(Hv_GetFloatText(max)/data->scalefactor);

/* automaticaly redraw hotrect? */
    
    if (Hv_CheckBit(Item->drawcontrol, Hv_REDRAWHOTRECTWHENCHANGED))
      Hv_DrawViewHotRect(View);

    Hv_SetRainbowLabels(Item);

    newregion = Hv_CopyItemRegion(Item, True);
    totalregion = Hv_UnionRegion(oldregion, newregion);
    Hv_ShrinkRegion(totalregion, -2, -2);
    Hv_DrawView(Item->view, totalregion);

    Hv_DestroyRegion(oldregion);
    Hv_DestroyRegion(newregion);
    Hv_DestroyRegion(totalregion);

    if (answer != Hv_APPLY)
      break;
  }
}


/*-------- Hv_SetRainbowLabels ------*/

static void Hv_SetRainbowLabels(Hv_Item Item)

{
  Hv_View               View = Item->view;
  int                   i;
  short                 boxfill = 0;
  float                 minf, maxf, delf, val;
  Boolean               inbox;
  char                  tstr[40];
  Hv_CompoundString     cs;
  short                 w, h, xt, yt;
  Hv_RainbowData        *data;
  Hv_Item               parent = Item->parent;
  Boolean               textlabels = False;

  data = (Hv_RainbowData *) (Item->data);

/* see if this rainbow is the child of a BOX parent */

  inbox = ((parent != NULL) &&
	   ((parent->type == Hv_BOXITEM) || (parent->type == Hv_RADIOCONTAINERITEM)));

  if (inbox)
    boxfill = parent->scheme->fillcolor;

  for (i = 0; i < Hv_MAXRAINBOWLABEL; i++) {
    Hv_DestroyItem(data->labels[i]);
    data->labels[i] = NULL;
  }


/* numlab == -1 means put "min" "max" label */

  if (data->numlab == -1) {
      textlabels = True;
      data->numlab = 2; 
  }


/* only write if at least two labels */

  if (data->numlab < 2)
    return;

  minf = ((float)(data->valmin))*data->scalefactor;
  maxf = ((float)(data->valmax))*data->scalefactor;
  delf = (maxf - minf)/(data->numlab - 1); 
  for (i = 0; i < data->numlab; i++) {
    val = minf + i*delf;

    if (textlabels && (i == 0)) 
	sprintf(tstr, "min");
    
    else if (textlabels && (i == (data->numlab-1)))
	sprintf(tstr, "max");
    else
	Hv_NumberToString(tstr, val, data->precision, (float)(1.0e-6));

    cs = Hv_CreateAndSizeCompoundString(tstr, Hv_fixed2, &w, &h);
    Hv_CompoundStringFree(cs);
    
    if (data->orientation == Hv_HORIZONTAL) {
      yt = Item->area->bottom + 4 + Hv_STRINGEXTRA;
      xt = Item->area->left +
	(short)Hv_nint(((val-minf)*(Item->area->width))/(maxf-minf));
      xt = xt - (w/2);
    }
    else {
      xt = Item->area->right + 4;
      yt = Item->area->bottom - 
	(short)Hv_nint(((val-minf)*(Item->area->height))/(maxf-minf));
      yt = yt - (h/2);
    }


    data->labels[i] = Hv_VaCreateItem(View,
				     Hv_TYPE,              Hv_TEXTITEM,
				     Hv_PARENT,            Item,
				     Hv_TEXT,              tstr,
				     Hv_FONT,              data->labelfont,
				     Hv_LEFT,              xt,
				     Hv_TOP,               yt,
				     Hv_WIDTH,             w,
				     Hv_HEIGHT,            h,
				     Hv_TEXTCOLOR,         Hv_black,
				     NULL);
/* if slider is in a box, need the proper background */

  if (inbox)
    data->labels[i]->scheme->fillcolor = boxfill;


  }

/* restore if changed for textlabels */

  if (textlabels)
      data->numlab = -1;
  
}


/* ------ Hv_EditRBColor --- */

static void Hv_EditRBColor(Hv_Widget w)

{
  
  if (w == mincolorlabel)
    Hv_ModifyColor(w, &newmincolor, "Edit Min Color", False);
  else if (w == maxcolorlabel)
    Hv_ModifyColor(w, &newmaxcolor, "Edit Max Color", False);
}





