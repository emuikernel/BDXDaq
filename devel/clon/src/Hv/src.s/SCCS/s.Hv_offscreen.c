h36889
s 00000/00000/00000
d R 1.2 02/09/09 11:30:41 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/s/Hv_offscreen.c
e
s 00070/00000/00000
d D 1.1 02/09/09 11:30:40 boiarino 1 0
c date and time created 02/09/09 11:30:40 by boiarino
e
u
U
f e 0
t
T
I 1
/**
 * <EM>Deals with off screen drawing.</EM>
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


/**
 * Hv_ViewSavesBackground
 * @purpose Checks whether a view saves its background in an offscreen pixmap.
 * @return True if the view does save its background. 
 */

Boolean    Hv_ViewSavesBackground(Hv_View View)

{
  return Hv_CheckBit(View->drawcontrol, Hv_SAVEBACKGROUND);
}

/**
 * Hv_ViewSavesBackground
 * @purpose Checks whether a view is "dirty" and should have its background restored.
 * @return True if the view is dirty. 
 */


Boolean   Hv_IsViewDirty(Hv_View View)

{
  return Hv_CheckBit(View->drawcontrol, Hv_DIRTY);
}


/**
 * Hv_SetViewDirty
 * @purpose Sets a view as dirty, because something has corrupted its background.
 * This is only meaningful for views that save their background.
 */

void      Hv_SetViewDirty(Hv_View View) {
  if (Hv_ViewSavesBackground(View))
    Hv_SetBit(&(View->drawcontrol), Hv_DIRTY);
}




/**
 * Hv_Hv_SetViewClean
 * @purpose Mark a view as clean, after its background has been restored.
 * This is only meaningful for views that save their background.
 */

void      Hv_SetViewClean(Hv_View View) {
    Hv_ClearBit(&(View->drawcontrol), Hv_DIRTY);
}


E 1
