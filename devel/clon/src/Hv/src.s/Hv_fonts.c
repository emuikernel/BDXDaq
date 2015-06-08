/**
 * <EM>Deals with fonts.</EM>
 * <P> <B>
 * NOTE: THIS IS A NOT-READY-FOR PRIME-TIME PACKAGE AND SHOULD
 * NOT BE USED AT THIS TIME.
 * <B>
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

char *Hv_charSet[Hv_NUMFONTS] =  {
  "fixed11",
  "fixed12",
  "fixed14",
  "fixed17",
  "fixed20",
  "fixed25",
  "courier11",
  "courier12",
  "courier14",
  "courier17",
  "courier20",
  "courier25",
  "helvetica11",
  "helvetica12",
  "helvetica14",
  "helvetica17",
  "helvetica20",
  "helvetica25",
  "times11",
  "times12",
  "times14",
  "times17",
  "times20",
  "times25",
  "symbol11",
  "symbol12",
  "symbol14",
  "symbol17",
  "symbol20",
  "symbol25",
  "fixedBold11",
  "fixedBold12",
  "fixedBold14",
  "fixedBold17",
  "fixedBold20",
  "fixedBold25",
  "courierBold11",
  "courierBold12",
  "courierBold14",
  "courierBold17",
  "courierBold20",
  "courierBold25",
  "helveticaBold11",
  "helveticaBold12",
  "helveticaBold14",
  "helveticaBold17",
  "helveticaBold20",
  "helveticaBold25",
  "timesBold11",
  "timesBold12",
  "timesBold14",
  "timesBold17",
  "timesBold20",
  "timesBold25"
};

Boolean  Hv_fontLoaded[Hv_NUMFONTS];

Hv_FontList	Hv_fontList = NULL;      /* Global font list */


short  Hv_fixed11         = 0;
short  Hv_fixed12         = 1;
short  Hv_fixed14         = 2;
short  Hv_fixed17         = 3;
short  Hv_fixed20         = 4;
short  Hv_fixed25         = 5;

short  Hv_courier11       = 6;
short  Hv_courier12       = 7;
short  Hv_courier14       = 8;
short  Hv_courier17       = 9;
short  Hv_courier20       = 10;
short  Hv_courier25       = 11;

short  Hv_helvetica11     = 12;
short  Hv_helvetica12     = 13;
short  Hv_helvetica14     = 14;
short  Hv_helvetica17     = 15;
short  Hv_helvetica20     = 16;
short  Hv_helvetica25     = 17;

short  Hv_times11         = 18;
short  Hv_times12         = 19;
short  Hv_times14         = 20;
short  Hv_times17         = 21;
short  Hv_times20         = 22;
short  Hv_times25         = 23;

short  Hv_symbol11        = 24;
short  Hv_symbol12        = 25;
short  Hv_symbol14        = 26;
short  Hv_symbol17        = 27;
short  Hv_symbol20        = 28;
short  Hv_symbol25        = 29;

short  Hv_fixedBold11     = 30;
short  Hv_fixedBold12     = 31;
short  Hv_fixedBold14     = 32;
short  Hv_fixedBold17     = 33;
short  Hv_fixedBold20     = 34;
short  Hv_fixedBold25     = 35;

short  Hv_courierBold11   = 36;
short  Hv_courierBold12   = 37;
short  Hv_courierBold14   = 38;
short  Hv_courierBold17   = 39;
short  Hv_courierBold20   = 40;
short  Hv_courierBold25   = 41;

short  Hv_helveticaBold11 = 42;
short  Hv_helveticaBold12 = 43;
short  Hv_helveticaBold14 = 44;
short  Hv_helveticaBold17 = 45;
short  Hv_helveticaBold20 = 46;
short  Hv_helveticaBold25 = 47;

short  Hv_timesBold11     = 48;
short  Hv_timesBold12     = 49;
short  Hv_timesBold14     = 50;
short  Hv_timesBold17     = 51;
short  Hv_timesBold20     = 52;
short  Hv_timesBold25     = 53;

short  Hv_fixed;
short  Hv_fixed2;


/**
 * Hv_GetFontExtra
 * @purpose This routine is for internal use. It returns "extra"
 * pixels forvarious fonts to try to make nice margins and line
 * spaces.
 * @param     font    The font index
 * @return    The number of "extra" pixels for that font
 */


short Hv_GetFontExtra(short font) {
  short    extra;

  extra = 2;

/* hack for symbol font */

  if ((font >= Hv_symbol11) && (font <= Hv_symbol25))
    extra += 3;
  
  return   extra;
}


/**
 * Hv_FontParametersToFont
 * @purpose   converts a family, size and style into an Hv font
 * index.
 * @note If size < 11 it is assumed to refer to
 *  one of the Hv_FONTSIZE macros. If it is
 *  >= 11, it is assumed to refer to the size
 *  directly.
 * @param family
 * @param size
 * @param style
 * @return    The corresponding font index
 */


short Hv_FontParametersToFont(short family,
			      short size,
			      short style) {

  short  font;

/* if size bigger than biggest size macro,
   convert it */

  if (size > Hv_FONTSIZE25) {
    if (size < 11)
      size = Hv_FONTSIZE11;
    if (size < 14)
      size = Hv_FONTSIZE12;
    else if (size < 15)
      size = Hv_FONTSIZE14;
    else if (size < 18)
      size = Hv_FONTSIZE17;
    else if (size < 21)
      size = Hv_FONTSIZE20;
    else
      size = Hv_FONTSIZE25;
  }

  
  font = (family - Hv_FIXEDFAMILY)*6;
  if ((style == Hv_BOLDFACE) && (family != Hv_SYMBOLFAMILY))
    font += 30;
  
  return font + size;
}

/**
 * Hv_FontToFontParameters
 * @purpose converts a standard font index into three
 * font parameters where size will be one
 * of the Hv_FONTSIZE macro values, not the
 * numeric size
 * @param font    The index to convert
 * @param family  Upon return, the corresponding family
 * @param size    Upon return, the corresponding size
 * @param style   Upon return, the corresponding style
 */


void Hv_FontToFontParameters(short font,
			     short *family,
			     short *size,
			     short *style) {


  *size = Hv_FONTSIZE11 + (font % 6);
  *style = Hv_PLAINFACE + (font / 30);
  font = font % 30;
  *family = Hv_FIXEDFAMILY + (font / 6);
  
}


/**
 * Hv_TextToFont
 * @purpose Converts a string to a font index. Often this is for
 * reading a description file, such as for hvplot, and converting
 * strings such as "hevetica14" to a font.
 * @param   text   The string to "read".
 * @return  The font index (if no match, returns Hv_hevetiacBold14)
 */


short Hv_TextToFont(char *text) {
  int i;
  short font;

/************************************************
  converts a line of text into an Hv font
  index.
***********************************************/

/* must handle anachronism associated with Hv_plot */

  font = Hv_helveticaBold14;

  if (Hv_Contained(text, "small")) 
    font =  Hv_helvetica14;

  else if (Hv_Contained(text, "medium"))
    font =  Hv_helvetica17;

  else if (Hv_Contained(text, "large"))
    font = Hv_helvetica20;

  else if (Hv_Contained(text, "huge"))
    font = Hv_helvetica25;

  else if (Hv_Contained(text, "tiny"))
    font =  Hv_helvetica11;

  else {
    for (i = 0; i < Hv_NUMFONTS; i++)
      if (Hv_Contained(text, Hv_charSet[i]))
	font = (short)i;
  }

  return font;
}

/**
 * Hv_FontDialogSuite
 * @purpose Return a standard set of font selection dialog widgets
 * @param parent       widget (usually a rowcol) 
 * @param title        An optional title (label)
 * @param family       Upon return, a widget for selecting the font family 
 * @param size         Upon return, a widget for selecting the font size
 * @param style        Upon return, a widget for selecting the font style
 * @param family_btns  Upon return, widget list (one per family)
 * @param size_btns    Upon return, widget list (one per size)
 * @param style_btns   Upon return, widget list (one per style)
 */


void Hv_FontDialogSuite(Hv_Widget       parent,
			char           *title,
			Hv_Widget      *family,
			Hv_Widget      *size,
			Hv_Widget      *style,
			Hv_WidgetList  *family_btns,
			Hv_WidgetList  *size_btns,
			Hv_WidgetList  *style_btns) {

  Hv_Widget      rc;
  int            num_buttons;

  rc = Hv_TightDialogRow(parent, 0);

/* if the title is not null, add a label */

  if (title != NULL)
    Hv_StandardLabel(rc, title, 0);


  *family = Hv_CreateFontFamilyPopup(rc, Hv_fixed2); /* font family menu */
  *size   = Hv_CreateFontSizePopup(rc, Hv_fixed2);   /* font size menu */
  *style  = Hv_CreateFontStylePopup(rc, Hv_fixed2);  /* font style menu */

  Hv_GetOptionMenuButtons(*family, family_btns, &num_buttons);
  Hv_GetOptionMenuButtons(*size,   size_btns,   &num_buttons);
  Hv_GetOptionMenuButtons(*style , style_btns,  &num_buttons);

}

