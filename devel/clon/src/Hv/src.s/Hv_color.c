/**
 * <EM>Various color utilization routines.</EM>
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

short   	Hv_red,           Hv_blue,        Hv_green,         Hv_yellow,       Hv_olive,
                Hv_slateBlue,     Hv_orange,      Hv_purple,        Hv_aquaMarine,   Hv_peru,
                Hv_violet,        Hv_brown,       Hv_darkSeaGreen,  Hv_mistyRose,    Hv_peachPuff,
                Hv_sandyBrown,
                Hv_lavender,      Hv_powderBlue,  Hv_forestGreen,   Hv_aliceBlue,    Hv_rosyBrown,
                Hv_gray0,         Hv_gray1,       Hv_gray2,         Hv_gray3,        Hv_gray4,
                Hv_gray5,         Hv_gray6,       Hv_gray7,         Hv_gray8,        Hv_gray9,
                Hv_gray10,        Hv_gray11,      Hv_gray12,        Hv_gray13,       Hv_gray14,
                Hv_gray15,        Hv_black,       Hv_white,         Hv_darkGreen,    Hv_honeydew,
                Hv_gray16,        Hv_coral,       Hv_maroon,        Hv_skyBlue,      Hv_darkBlue,
                Hv_steelBlue,     Hv_tan,         Hv_wheat,         Hv_highlight,    Hv_lightSeaGreen,
                Hv_tan3,          Hv_chocolate,   Hv_lawnGreen,     Hv_seaGreen,     Hv_mediumForestGreen,        
                Hv_cadetBlue,     Hv_royalBlue,   Hv_dodgerBlue,    Hv_navyBlue,     Hv_cornFlowerBlue, 
                Hv_canvasColor;


/**
 * Hv_ChangeBackgroundColor   Change the background color (generic)
 * @param    color      The color index for the new background color
 */


void   Hv_ChangeBackgroundColor(short color) {
    Hv_Item   version;

    Hv_SetWidgetBackgroundColor(Hv_canvas, color);
    Hv_canvasColor = color;

/* fix the version string */


    if (Hv_welcomeView != NULL)
	if ((version = Hv_GetItemFromTag(Hv_welcomeView->items, Hv_VERSIONSTRINGTAG)) != NULL) {
      
	    version->str->fillcol = Hv_canvasColor;
	    if (Hv_canvasColor == Hv_black)
		version->str->strcol = Hv_white;
	}
  
}


/**
 * Hv_AdjustAdjacentColors    Adjusts adjacent colors for use in highlighting (generic)
 * @param       color   the central color
 * @param       del     the amount to deviate from base color
 */


void  Hv_AdjustAdjacentColors(short color,
			      short del) {

/* used in conjunction with highlighting */

  if (Hv_readOnlyColors) {
    Hv_Println("Hv warning: cannot adjust colors for read-only visuals.");
    return;
  }

  if ((color < 1) || (color >= (Hv_numColors-1)))
    return;

  Hv_CopyColor((short)(color-1), color);
  Hv_CopyColor((short)(color+1), color);

  if (del > 0) {
    Hv_BrightenColor((short)(color-1), del);
    Hv_BrightenColor((short)(color+1), del);
  }
  else {
    Hv_DarkenColor((short)(color-1), (short)(-del));
    Hv_DarkenColor((short)(color+1), (short)(-del));
  }

}

