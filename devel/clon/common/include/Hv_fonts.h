/**
 * <EM>Header file for fonts.</EM>
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

#ifndef __HVFONTSH__
#define __HVFONTSH__


/* font styles */

#define Hv_PLAINFACE   0
#define Hv_BOLDFACE    1
#define Hv_ITALIC      2
#define Hv_BOLDITALIC  3

/* font families */

#define Hv_FIXEDFAMILY      0
#define Hv_COURIERFAMILY    1
#define Hv_HELVETICAFAMILY  2 
#define Hv_TIMESFAMILY      3
#define Hv_SYMBOLFAMILY     4

/* font sizes */

#define Hv_FONTSIZE11       0
#define Hv_FONTSIZE12       2
#define Hv_FONTSIZE14       2
#define Hv_FONTSIZE17       3
#define Hv_FONTSIZE20       4
#define Hv_FONTSIZE25       5

/* font editing and composite strings */

#define Hv_NEXTBIGGERFONT        1
#define Hv_NEXTSMALLERFONT       2
#define Hv_ROMANFONT             3
#define Hv_GREEKFONT             4
#define Hv_BOLDFONT              5
#define Hv_PLAINFONT             6
#define Hv_HELVETICAFONT         7

#define Hv_NUMFONTS            54       /* number of fonts available in Hv */     

/* Global font structures */

extern Hv_FontList	Hv_fontList;
extern char            *Hv_charSet[Hv_NUMFONTS];
extern Boolean          Hv_fontLoaded[Hv_NUMFONTS];

extern short  Hv_fixed;
extern short  Hv_fixed2;

extern short  Hv_fixed11;
extern short  Hv_fixed12;
extern short  Hv_fixed14;
extern short  Hv_fixed17;
extern short  Hv_fixed20;
extern short  Hv_fixed25;

extern short  Hv_courier11;
extern short  Hv_courier12;
extern short  Hv_courier14;
extern short  Hv_courier17;
extern short  Hv_courier20;
extern short  Hv_courier25;

extern short  Hv_helvetica11;
extern short  Hv_helvetica12;
extern short  Hv_helvetica14;
extern short  Hv_helvetica17;
extern short  Hv_helvetica20;
extern short  Hv_helvetica25;

extern short  Hv_times11;
extern short  Hv_times12;
extern short  Hv_times14;
extern short  Hv_times17;
extern short  Hv_times20;
extern short  Hv_times25;

extern short  Hv_symbol11;
extern short  Hv_symbol12;
extern short  Hv_symbol14;
extern short  Hv_symbol17;
extern short  Hv_symbol20;
extern short  Hv_symbol25;

extern short  Hv_fixedBold11;
extern short  Hv_fixedBold12;
extern short  Hv_fixedBold14;
extern short  Hv_fixedBold17;
extern short  Hv_fixedBold20;
extern short  Hv_fixedBold25;

extern short  Hv_courierBold11;
extern short  Hv_courierBold12;
extern short  Hv_courierBold14;
extern short  Hv_courierBold17;
extern short  Hv_courierBold20;
extern short  Hv_courierBold25;

extern short  Hv_helveticaBold11;
extern short  Hv_helveticaBold12;
extern short  Hv_helveticaBold14;
extern short  Hv_helveticaBold17;
extern short  Hv_helveticaBold20;
extern short  Hv_helveticaBold25;

extern short  Hv_timesBold11;
extern short  Hv_timesBold12;
extern short  Hv_timesBold14;
extern short  Hv_timesBold17;
extern short  Hv_timesBold20;
extern short  Hv_timesBold25;

extern short  Hv_bgFont;  /* for button groups */

/*-------- Hv_fonts.c ----------*/

extern short          Hv_FontParametersToFont(short family,
					      short size,
					      short style);

extern void           Hv_FontToFontParameters(short font,
					      short *family,
					      short *size,
					      short *style);

extern short          Hv_TextToFont(char *text);

extern void           Hv_FontDialogSuite(Hv_Widget       parent,
					 char           *title,
					 Hv_Widget      *family,
					 Hv_Widget      *size,
					 Hv_Widget      *style,
					 Hv_WidgetList  *family_btns,
					 Hv_WidgetList  *size_btns,
					 Hv_WidgetList  *style_btns);
     
#endif









