/**
 * <EM>Deals with slider items on <B>views.</B></EM>
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

/**
 * Hv_AddDrawingTools
 * @purpose  Create the draing tools.
 * @param   view
 * @param   left
 * @param   top
 * @param   orientation
 * @param   resizepolicy
 * @param   fillcolor
 * @param   cameratoo
 * @param   first
 * @param   last
 */

void   Hv_AddDrawingTools(Hv_View  View,
			  short    left,
			  short    top,
			  short    orientation,
			  short    resizepolicy,
			  short    fillcolor,
			  Boolean  cameratoo,
			  Hv_Item *first,
			  Hv_Item *last) {

  int whichtools;

  whichtools = Hv_ALLTOOLS;
  if (!cameratoo)
    Hv_ClearBit(&whichtools, Hv_CAMERATOOL);

  Hv_StandardTools(View, 
		   left,
		   top,
		   orientation,
		   resizepolicy,
		   whichtools,
		   first,
		   last);

}

