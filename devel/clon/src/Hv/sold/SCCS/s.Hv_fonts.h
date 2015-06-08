h51094
s 00000/00000/00000
d R 1.2 02/08/25 23:21:11 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_fonts.h
e
s 00150/00000/00000
d D 1.1 02/08/25 23:21:10 boiarino 1 0
c date and time created 02/08/25 23:21:10 by boiarino
e
u
U
f e 0
t
T
I 1
/*
==================================================================================
Hv_fonts.h is the font related header file for the Hv graphical library

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of they U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
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
#define Hv_FONTSIZE14       1
#define Hv_FONTSIZE17       2
#define Hv_FONTSIZE20       3
#define Hv_FONTSIZE25       4

/* font editing and composite strings */

#define Hv_NEXTBIGGERFONT        1
#define Hv_NEXTSMALLERFONT       2
#define Hv_ROMANFONT             3
#define Hv_GREEKFONT             4
#define Hv_BOLDFONT              5
#define Hv_PLAINFONT             6
#define Hv_HELVETICAFONT         7

#define Hv_NUMFONTS            45       /* number of fonts available in Hv */     

/* Global font structures */

extern XmFontList	Hv_fontList;
extern char            *Hv_charSet[Hv_NUMFONTS];
extern Boolean          Hv_fontLoaded[Hv_NUMFONTS];

extern short  Hv_fixed;
extern short  Hv_fixed2;

extern short  Hv_fixed11;
extern short  Hv_fixed14;
extern short  Hv_fixed17;
extern short  Hv_fixed20;
extern short  Hv_fixed25;

extern short  Hv_courier11;
extern short  Hv_courier14;
extern short  Hv_courier17;
extern short  Hv_courier20;
extern short  Hv_courier25;

extern short  Hv_helvetica11;
extern short  Hv_helvetica14;
extern short  Hv_helvetica17;
extern short  Hv_helvetica20;
extern short  Hv_helvetica25;

extern short  Hv_times11;
extern short  Hv_times14;
extern short  Hv_times17;
extern short  Hv_times20;
extern short  Hv_times25;

extern short  Hv_symbol11;
extern short  Hv_symbol14;
extern short  Hv_symbol17;
extern short  Hv_symbol20;
extern short  Hv_symbol25;

extern short  Hv_fixedBold11;
extern short  Hv_fixedBold14;
extern short  Hv_fixedBold17;
extern short  Hv_fixedBold20;
extern short  Hv_fixedBold25;

extern short  Hv_courierBold11;
extern short  Hv_courierBold14;
extern short  Hv_courierBold17;
extern short  Hv_courierBold20;
extern short  Hv_courierBold25;

extern short  Hv_helveticaBold11;
extern short  Hv_helveticaBold14;
extern short  Hv_helveticaBold17;
extern short  Hv_helveticaBold20;
extern short  Hv_helveticaBold25;

extern short  Hv_timesBold11;
extern short  Hv_timesBold14;
extern short  Hv_timesBold17;
extern short  Hv_timesBold20;
extern short  Hv_timesBold25;


/*-------- Hv_fonts.c ----------*/

extern void           Hv_InitFonts(void);

extern void           Hv_AddNewFont(short);

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






E 1
