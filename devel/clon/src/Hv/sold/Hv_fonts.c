/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"


static void     Hv_FontReport(XFontStruct **,
			      char *);

static XFontStruct  *Hv_AddFont(short);


char *Hv_charSet[Hv_NUMFONTS] =  {
  "fixed11",
  "fixed14",
  "fixed17",
  "fixed20",
  "fixed25",
  "courier11",
  "courier14",
  "courier17",
  "courier20",
  "courier25",
  "helvetica11",
  "helvetica14",
  "helvetica17",
  "helvetica20",
  "helvetica25",
  "times11",
  "times14",
  "times17",
  "times20",
  "times25",
  "symbol11",
  "symbol14",
  "symbol17",
  "symbol20",
  "symbol25",
  "fixedBold11",
  "fixedBold14",
  "fixedBold17",
  "fixedBold20",
  "fixedBold25",
  "courierBold11",
  "courierBold14",
  "courierBold17",
  "courierBold20",
  "courierBold25",
  "helveticaBold11",
  "helveticaBold14",
  "helveticaBold17",
  "helveticaBold20",
  "helveticaBold25",
  "timesBold11",
  "timesBold14",
  "timesBold17",
  "timesBold20",
  "timesBold25"
};

Boolean  Hv_fontLoaded[Hv_NUMFONTS];

char *Hv_xFontNames[Hv_NUMFONTS] = {
  "-*-fixed-medium-r-normal--13-*-*-*-*-*-iso8859-*",
  "-*-fixed-medium-r-normal--15-*-*-*-*-*-iso8859-*",
  "-*-fixed-medium-r-normal--16-*-*-*-*-*-iso8859-*",
  "-*-fixed-medium-r-normal--20-*-*-*-*-*-iso8859-*",
  "-*-fixed-medium-r-normal--24-*-*-*-*-*-iso8859-*",
  "-*-courier-medium-r-normal--11-*-*-*-*-*-*-*",
  "-*-courier-medium-r-normal--14-*-*-*-*-*-*-*",
  "-*-courier-medium-r-normal--17-*-*-*-*-*-*-*",
  "-*-courier-medium-r-normal--20-*-*-*-*-*-*-*",
  "-*-courier-medium-r-normal--25-*-*-*-*-*-*-*",
  "-*-helvetica-medium-r-normal--11-*-*-*-*-*-*-*",
  "-*-helvetica-medium-r-normal--14-*-*-*-*-*-*-*",
  "-*-helvetica-medium-r-normal--17-*-*-*-*-*-*-*",
  "-*-helvetica-medium-r-normal--20-*-*-*-*-*-*-*",
  "-*-helvetica-medium-r-normal--25-*-*-*-*-*-*-*",
  "-*-times-medium-r-normal--11-*-*-*-*-*-*-*",
  "-*-times-medium-r-normal--14-*-*-*-*-*-*-*",
  "-*-times-medium-r-normal--17-*-*-*-*-*-*-*",
  "-*-times-medium-r-normal--20-*-*-*-*-*-*-*",
  "-*-times-medium-r-normal--25-*-*-*-*-*-*-*",
  "-*-symbol-*-r-normal--11-*-*-*-p-*-*-*",
  "-*-symbol-*-r-normal--14-*-*-*-p-*-*-*",
  "-*-symbol-*-r-normal--17-*-*-*-p-*-*-*",
  "-*-symbol-*-r-normal--20-*-*-*-p-*-*-*",
  "-*-symbol-*-r-normal--25-*-*-*-p-*-*-*",
  "-*-fixed-bold-r-normal--13-*-*-*-*-*-iso8859-*",
  "-*-fixed-bold-r-normal--15-*-*-*-*-*-iso8859-*",
  "-*-fixed-bold-r-normal--16-*-*-*-*-*-iso8859-*",
  "-*-fixed-bold-r-normal--20-*-*-*-*-*-iso8859-*",
  "-*-fixed-bold-r-normal--24-*-*-*-*-*-iso8859-*",
  "-*-courier-bold-r-normal--11-*-*-*-*-*-*-*",
  "-*-courier-bold-r-normal--14-*-*-*-*-*-*-*",
  "-*-courier-bold-r-normal--17-*-*-*-*-*-*-*",
  "-*-courier-bold-r-normal--20-*-*-*-*-*-*-*",
  "-*-courier-bold-r-normal--25-*-*-*-*-*-*-*",
  "-*-helvetica-bold-r-normal--11-*-*-*-*-*-*-*",
  "-*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
  "-*-helvetica-bold-r-normal--17-*-*-*-*-*-*-*",
  "-*-helvetica-bold-r-normal--20-*-*-*-*-*-*-*",
  "-*-helvetica-bold-r-normal--25-*-*-*-*-*-*-*",
  "-*-times-bold-r-normal--11-*-*-*-*-*-*-*",
  "-*-times-bold-r-normal--14-*-*-*-*-*-*-*",
  "-*-times-bold-r-normal--17-*-*-*-*-*-*-*",
  "-*-times-bold-r-normal--20-*-*-*-*-*-*-*",
  "-*-times-bold-r-normal--25-*-*-*-*-*-*-*"
};

XmFontList	Hv_fontList = NULL;      /* Global font list */


short  Hv_fixed11         = 0;
short  Hv_fixed14         = 1;
short  Hv_fixed17         = 2;
short  Hv_fixed20         = 3;
short  Hv_fixed25         = 4;
short  Hv_courier11       = 5;
short  Hv_courier14       = 6;
short  Hv_courier17       = 7;
short  Hv_courier20       = 8;
short  Hv_courier25       = 9;
short  Hv_helvetica11     = 10;
short  Hv_helvetica14     = 11;
short  Hv_helvetica17     = 12;
short  Hv_helvetica20     = 13;
short  Hv_helvetica25     = 14;
short  Hv_times11         = 15;
short  Hv_times14         = 16;
short  Hv_times17         = 17;
short  Hv_times20         = 18;
short  Hv_times25         = 19;
short  Hv_symbol11        = 20;
short  Hv_symbol14        = 21;
short  Hv_symbol17        = 22;
short  Hv_symbol20        = 23;
short  Hv_symbol25        = 24;

short  Hv_fixedBold11     = 25;
short  Hv_fixedBold14     = 26;
short  Hv_fixedBold17     = 27;
short  Hv_fixedBold20     = 28;
short  Hv_fixedBold25     = 29;
short  Hv_courierBold11   = 30;
short  Hv_courierBold14   = 31;
short  Hv_courierBold17   = 32;
short  Hv_courierBold20   = 33;
short  Hv_courierBold25   = 34;
short  Hv_helveticaBold11 = 35;
short  Hv_helveticaBold14 = 36;
short  Hv_helveticaBold17 = 37;
short  Hv_helveticaBold20 = 38;
short  Hv_helveticaBold25 = 39;
short  Hv_timesBold11     = 40;
short  Hv_timesBold14     = 41;
short  Hv_timesBold17     = 42;
short  Hv_timesBold20     = 43;
short  Hv_timesBold25     = 44;

short  Hv_fixed;
short  Hv_fixed2;


extern XFontStruct     *fontfixed;
extern XFontStruct     *fontfixed2;


/*------- Hv_GetFontExtra ------------*/

short Hv_GetFontExtra(short font)

{
  short    extra;

  extra = 2;

/* hack for symbol font */

  if ((font >= Hv_symbol11) && (font <= Hv_symbol25))
    extra += 3;
  
  return   extra;
}


/* ----------- Hv_FontReport --------------- */

static void Hv_FontReport(XFontStruct **fnt,
			  char         *fname)

/* make a diagnostic report regarding whether or not the font was loaded */

{
  char   *message;

  if ((*fnt) == NULL) {
    message = (char *)Hv_Malloc(132);
    strcpy(message, "did NOT find ");
    strcat(message, fname);
    fprintf(stderr, "%s\n", message);
    Hv_Free(message);
    *fnt = fontfixed;
  }

}


/*------- Hv_AddNewFont --------*/

void Hv_AddNewFont(short font)

{
  XFontStruct   *dummy;

  if ((font < 0) || (font >= Hv_NUMFONTS))
    return;

  dummy = Hv_AddFont(font);
}

/* ----------- Hv_AddFont --------*/

static XFontStruct  *Hv_AddFont(short font)

{
  XFontStruct  *xfont;
  char         *substr;

  xfont  = XLoadQueryFont(Hv_display,  Hv_xFontNames[font]);

/* try a fix if the font is not found */

  if (xfont == NULL) {
    fprintf(stderr, "could not find font: %s\n", Hv_xFontNames[font]);
    if ((substr = strstr(Hv_xFontNames[font], "11")) != NULL)
      substr[1] = '0';
    else if ((substr = strstr(Hv_xFontNames[font], "14")) != NULL)
      substr[1] = '2';
    else if ((substr = strstr(Hv_xFontNames[font], "15")) != NULL)
      substr[1] = '4';
    else if ((substr = strstr(Hv_xFontNames[font], "17")) != NULL)
      substr[1] = '4';
    else if ((substr = strstr(Hv_xFontNames[font], "20")) != NULL) {
      substr[0] = '1';
      substr[1] = '8';
    }
    else if ((substr = strstr(Hv_xFontNames[font], "25")) != NULL)
      substr[1] = '4';
    fprintf(stderr, "trying a substitute: %s\n", Hv_xFontNames[font]);
    xfont  = XLoadQueryFont(Hv_display,  Hv_xFontNames[font]);
  }

  Hv_FontReport(&xfont,    Hv_xFontNames[font]);

  if (Hv_fontList == NULL)
    Hv_fontList = XmFontListCreate(xfont, Hv_charSet[font]);
  else
    Hv_fontList = XmFontListAdd(Hv_fontList, xfont, Hv_charSet[font]);

  Hv_fontLoaded[font] = True;
  return  xfont;
}

/* ----------- Hv_InitFonts ---------- */

void Hv_InitFonts(void)

{
  int    i;

  Hv_fixed  = Hv_fixed11;
  Hv_fixed2 = Hv_fixedBold11;

  for (i = 0; i < Hv_NUMFONTS; i++)
    Hv_fontLoaded[i] = False;

  fontfixed  = Hv_AddFont(Hv_fixed);
  fontfixed2 = Hv_AddFont(Hv_fixed2);

}

/*-------- Hv_FontParametersToFont -------*/

short Hv_FontParametersToFont(short family,
			      short size,
			      short style)

/************************************************
  converts a family, size and style into an Hv font
  index.

  NOTE: If size < 11 it is assumed to refer to
  one of the Hv_FONTSIZE macros. If it is
  >= 11, it is assumed to refer to the size
  directly.
***********************************************/



{

  short  font;

/* if size bigger than biggest size macro,
   convert it */

  if (size > Hv_FONTSIZE25) {
    if (size < 12)
      size = Hv_FONTSIZE11;
    else if (size < 15)
      size = Hv_FONTSIZE14;
    else if (size < 18)
      size = Hv_FONTSIZE17;
    else if (size < 21)
      size = Hv_FONTSIZE20;
    else
      size = Hv_FONTSIZE25;
  }

  
  font = (family - Hv_FIXEDFAMILY)*5;
  if ((style == Hv_BOLDFACE) && (family != Hv_SYMBOLFAMILY))
    font += 25;
  
  return font + size;
}


/*-------- Hv_FontToFontParameters ---------*/

void Hv_FontToFontParameters(short font,
			     short *family,
			     short *size,
			     short *style)
{

/************************************************
  converts a standard font index into three
  font parameters where size will be one
  of the Hv_FONTSIZE macro values, not the
  numeric size
***********************************************/


  *size = Hv_FONTSIZE11 + (font % 5);
  *style = Hv_PLAINFACE + (font / 25);
  font = font % 25;
  *family = Hv_FIXEDFAMILY + (font / 5);
  
}


/*--------- Hv_TextToFont -------*/

short Hv_TextToFont(char *text)

{
  int i;
  short font;

/************************************************
  converts a line of text into an Hv font
  index.
***********************************************/

/* must handle anachronism associate with Hv_plot */

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

/*------------ Hv_FontDialogSuite -----*/

void Hv_FontDialogSuite(Hv_Widget       parent,
			char           *title,
			Hv_Widget      *family,
			Hv_Widget      *size,
			Hv_Widget      *style,
			Hv_WidgetList  *family_btns,
			Hv_WidgetList  *size_btns,
			Hv_WidgetList  *style_btns)

/***********************************************
  returns a standard suite of font selection dialog widgets
************************************************/

{
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




