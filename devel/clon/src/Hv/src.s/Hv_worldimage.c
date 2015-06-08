
/**
 * <EM>This file deals with placing pictures (JPEG only) into a
 * world rectangle </EM>
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
#include "Hv_pic.h"
#include "Hv_xyplot.h"
#include "Hv_maps.h"



/**
 * Hv_EditWorldImageItem 
 * @purpose Edit world image items (presently this does nothing,
 * just a placeholder.
 * @param hvevevnt   The causal event
 */


void   Hv_EditWorldImageItem(Hv_Event hvevent) {
}




/**
 * Hv_CheckWorldImageEnlarge
 * @purpose  Check if a image item is being enlarged
 * @param Item        The Item to check
 * @param StartPP     The initial click position
 * @param ShowCorner  If true, show size during resize
 * @param enlarged    If true (upon return), the item was enlarged
 */

void Hv_CheckWorldImageEnlarge(Hv_Item    Item,
			       Hv_Point   StartPP,
			       Boolean    ShowCorner,
			       Boolean   *enlarged) {
  Hv_CheckWorldRectEnlarge(Item, StartPP, ShowCorner, enlarged);
}


/**
 * Hv_WorldImageInitialize
 * @purpose   General initialization of image items
 * @param Item       The new image item
 * @param attributes The attribute array
 */

void Hv_WorldImageInitialize(Hv_Item      Item,
			     Hv_AttributeArray attributes) {

  Hv_WorldImageData     *wimage;
  Hv_View                View = Item->view;
  Boolean                fromfile;
  char                  *fn;
  Hv_PicInfo            *pinfo;

  short                  left, top;

  Item->standarddraw = Hv_DrawWorldImageItem;
  Item->domain = Hv_INSIDEHOTRECT;

  Hv_SetItemDrawControlBit(Item, Hv_ALWAYSREDRAW);

/* enlargeble? */

  if (Hv_CheckItemDrawControlBit(Item, Hv_ENLARGEABLE))
    if (attributes[Hv_CHECKENLARGE].v == NULL) {
      Item->checkenlarge = (Hv_FunctionPtr)Hv_CheckWorldImageEnlarge;
    }

  left = attributes[Hv_LEFT].s;
  top  = attributes[Hv_TOP].s;


  Item->worldarea = Hv_NewFRect();
  Hv_DefaultSchemeIn(&(Item->scheme));
  wimage = (Hv_WorldImageData *)Hv_Malloc(sizeof(Hv_WorldImageData));
  Item->userptr2 = (void *)wimage;

  wimage->background = 0;
  wimage->frame = False;
  wimage->thickness = 1;
  wimage->pinfo = NULL;

  Hv_SetFRect(Item->worldarea, 
	      (float)(attributes[Hv_XMIN].f),
	      (float)(attributes[Hv_XMAX].f),
	      (float)(attributes[Hv_YMIN].f),
	      (float)(attributes[Hv_YMAX].f));
  
  if (Item->doubleclick == NULL)
    Item->doubleclick = Hv_EditWorldImageItem;

/* now see if we are to read from a file */

  fromfile = (attributes[Hv_INITFROMFILE].i != 0);

  if (fromfile) {

    fn = Hv_FileSelect("Pick an image file.", "*.jp*", NULL);

    if (fn == NULL)
      return;

    Hv_PicFromFile(fn, &(pinfo));
    
    if ((pinfo != NULL) && (pinfo->image != NULL)) {
      wimage->pinfo = pinfo;
      Hv_SetRect(Item->area, left, top,  (short)(pinfo->normw), (short)(pinfo->normh));
    }

  }

  wimage->latmin  = (float)(Hv_DEGTORAD*attributes[Hv_LATMIN].f);
  wimage->latmax  = (float)(Hv_DEGTORAD*attributes[Hv_LATMAX].f);
  wimage->longmin = (float)(Hv_DEGTORAD*attributes[Hv_LONGMIN].f);
  wimage->longmax = (float)(Hv_DEGTORAD*attributes[Hv_LONGMAX].f);


  Hv_LocalRectToWorldRect(View, Item->area, Item->worldarea);

}

/**
 * Hv_LatLongRectToLocalRect
 * @purpose Convert latitude longitude limits to a rectangle.
 * @param latmin  The minimum latitude
 * @param latmax  The maximum latitude
 * @param longmin  The minimum longitude
 * @param longmax  The maximum longitude
 * @param rect   Upon return, the corresponding rect
 */

void Hv_LatLongRectToLocalRect(Hv_View View,
				      float   latmin,
				      float   latmax,
				      float   longmin,
				      float   longmax,
				      Hv_Rect *rect) {
  short l, t, r, b, w, h;



  Hv_LatLongToLocal(View, &r, &t, latmax, longmax);
  Hv_LatLongToLocal(View, &l, &b, latmin, longmin);

  if ((r < l) || (b < t)) {
    Hv_Println("Bad Rect in LatLongToLocalRect [%d %d %d %d]",
	    l, t, r, b);
    Hv_SetRect(rect, 0, 0, 0, 0);
  }
  else {
    w = Hv_sMax((short)(r-l), 2);
    h = Hv_sMax((short)(b-t), 2);
    Hv_SetRect(rect, l, t, w, h);
  }

       
}


/*-------- Hv_DestroyWorldImage --------*/


void Hv_DestroyWorldImage(Hv_Item Item) {

  Hv_WorldImageData *wimage;
      
  
  wimage = (Hv_WorldImageData *)(Item->userptr2);

  if (wimage != NULL) {
      Hv_DestroyPic(wimage->pinfo);

      if (wimage->background)
	  Hv_FreePixmap(wimage->background);

  }



  Hv_Free(wimage);
  
  Item->userptr2 = NULL;
  
}





