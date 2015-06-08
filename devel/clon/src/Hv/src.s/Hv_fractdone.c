/**
 * <EM>Holds many of the platform independent
 * drawing primitives.</EM>
 * <p>
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

#define Hv_FDMINWIDTH  10

/**
 * Hv_FractionDoneUpdate
 * @purpose Update the item, showing the progress.
 * @param  Item      The fraction done item in question. 
 * @param  fraction  The fraction [0.0 .. 1.0] done.
 */

extern void  Hv_FractionDoneUpdate(Hv_Item Item,
				   float   fraction) {
  Hv_FractionDoneData   *fdone;
  Hv_Rect               area;
  Hv_Region             rrgn = NULL;

  if (Item == NULL)
    return;

  fdone = (Hv_FractionDoneData *)(Item->data);
  fdone->fraction = fraction;

  Hv_CopyRect(&area, Item->area);
  Hv_ShrinkRect(&area, 1, 1);

  rrgn = Hv_RectRegion(&area);
  Hv_DrawItem(Item, rrgn);
  Hv_DestroyRegion(rrgn);

/* also check for exposures, since it may have been a long
   time since last update */

  Hv_CheckForExposures(Hv_EXPOSE);
}


/**
 * Hv_DrawFractionDoneItem
 * @purpose Draw the fraction done item.
 * @param  Item      The fraction done item in question. 
 * @param  region    The clipping region.
 */

void Hv_DrawFractionDoneItem(Hv_Item   Item,
			     Hv_Region region) {

  Hv_FractionDoneData   *fdone;
  Hv_Rect               *area;
  Hv_Rect                a1, a2;
  short                  w, w1, w2;
  float                  fraction;


  fdone = (Hv_FractionDoneData *)(Item->data);
  fraction = Hv_fMax(0.0, Hv_fMin(1.0, fdone->fraction));
  area = Item->area;

  w = area->width - 1;
  w1 = (short)(fraction*w);
  Hv_SetRect(&a1, 
	     (short)(area->left+1), 
	     (short)(area->top+1), 
	     w1, 
	     (short)(area->height-2));

  w2 = (short)(area->right - a1.right - 1);
  Hv_SetRect(&a2, 
	     (short)(a1.right+1), 
	     (short)(area->top+1), 
	     w2, 
	     (short)(area->height-2));

  Hv_Simple3DRect(&a1, True, Item->color);
  Hv_FillRect(&a2, Hv_gray10);

  Hv_Simple3DRect(Item->area, False, -1);
}

/**
 * Hv_FractionDoneInitialize
 * @purpose Initialzation for the fraction done item.
 * @param Item        The fraction done item in question.
 * @param attributes  The attribute array.
 */

void Hv_FractionDoneInitialize(Hv_Item Item,
			       Hv_AttributeArray attributes) {

  Hv_FractionDoneData  *fdone;

/* use a custom color scheme */

  fdone = (Hv_FractionDoneData *)Hv_Malloc(sizeof(Hv_FractionDoneData));
  fdone->fraction = 0.0;
  fdone->topcolor = Hv_gray7;
  fdone->bottomcolor = Hv_gray14;
  fdone->fillcolor = Hv_red;

  Item->data = (void *)fdone;

/* see if user provided width and height */

  Item->area->height = 10;
  Item->area->width = Hv_sMax(Hv_FDMINWIDTH, Item->area->width);
  Hv_FixRectRB(Item->area);

  Item->standarddraw = Hv_DrawFractionDoneItem;
}

#undef Hv_FDMINWIDTH
