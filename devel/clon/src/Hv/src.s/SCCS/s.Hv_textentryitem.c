h28444
s 00000/00000/00000
d R 1.2 02/09/09 11:30:52 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/s/Hv_textentryitem.c
e
s 00038/00000/00000
d D 1.1 02/09/09 11:30:51 boiarino 1 0
c date and time created 02/09/09 11:30:51 by boiarino
e
u
U
f e 0
t
T
I 1
/**
 * <EM>Deals with the text entry widget.</EM>
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


/**
 * Hv_DrawTextEntryItem
 * @purpose  Draw the text entry item
 * @param   Item      The item
 * @param   region    The clipping region
 */

void Hv_DrawTextEntryItem(Hv_Item    Item,
			  Hv_Region   region) {
  Hv_DrawTextItem(Item, region);
}










E 1
