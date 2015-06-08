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










