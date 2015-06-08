h25383
s 00000/00000/00000
d R 1.2 02/08/25 23:21:06 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_color.c
e
s 00733/00000/00000
d D 1.1 02/08/25 23:21:05 boiarino 1 0
c date and time created 02/08/25 23:21:05 by boiarino
e
u
U
f e 0
t
T
I 1
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

/*------- local prototypes --------*/

static Boolean Hv_MatchVisual(Display *,
			      int scr,
			      int,
			      Visual *,
			      int);

static void    Hv_ColorizeCursors(Display *);

static void    Hv_FillGaps(short,
			   short,
			   short,
			   short);

static void    Hv_StuffColor(char *,
			     unsigned int,
			     unsigned int,
			     unsigned int,
			     int);

static void    Hv_CheckColorMap(void);

static int     Hv_visClass;
static Boolean Hv_readOnlyColors;

/*------ Hv_ChangeBackgroundColor ------*/

void   Hv_ChangeBackgroundColor(short color)

{
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

/* ---------- Hv_AdjustColor ----------- */

void   Hv_AdjustColor(short color,
		      short r,
		      short g,
		      short b)

{
  XColor   xcolor;
  xcolor.pixel = Hv_colors[color];
  XQueryColor(Hv_display, Hv_colorMap, &xcolor);

  xcolor.red += r;
  xcolor.green += g;
  xcolor.blue += b;
  
  if (XAllocColor(Hv_display, Hv_colorMap, &xcolor))
    Hv_colors[color] = xcolor.pixel;
}

/* ---------- Hv_ChangeColor ----------- */

void   Hv_ChangeColor(short color,
		      unsigned short r,
		      unsigned short g,
		      unsigned short b)
     
{
  XColor   xcolor;

  xcolor.pixel = Hv_colors[color];
  XQueryColor(Hv_display, Hv_colorMap, &xcolor);
  
  xcolor.red = r;
  xcolor.green = g;
  xcolor.blue = b;
  
  if (XAllocColor(Hv_display, Hv_colorMap, &xcolor))
    Hv_colors[color] = xcolor.pixel;
}

/* ---------- Hv_DarkenColor ----------- */

void  Hv_DarkenColor(short color,
		     unsigned short del)

{
  XColor   xcolor;

  xcolor.pixel = Hv_colors[color];
  XQueryColor(Hv_display, Hv_colorMap, &xcolor);

  if (xcolor.red < del)
    xcolor.red = 0;
  else
    xcolor.red -= del;

  if (xcolor.green < del)
    xcolor.green = 0;
  else
    xcolor.green -= del;

  if (xcolor.blue < del)
    xcolor.blue = 0;
  else
    xcolor.blue -= del;

  xcolor.flags = DoRed | DoGreen | DoBlue;

/* if read only colors, all we can do is allocate a different color.
   If read/write, then change the entry */

  if (!Hv_readOnlyColors)
    XStoreColor(Hv_display, Hv_colorMap, &xcolor);
  else if (XAllocColor(Hv_display, Hv_colorMap, &xcolor))
    Hv_colors[color] = xcolor.pixel;
}

/* ---------- Hv_BrightenColor ----------- */

void  Hv_BrightenColor(short color,
		       unsigned short del)

{
  XColor           xcolor;
  unsigned short   limval;

  limval = 65535 - del;

  xcolor.pixel = Hv_colors[color];
  XQueryColor(Hv_display, Hv_colorMap, &xcolor);
  
  if (xcolor.red > limval)
    xcolor.red = 65535;
  else
    xcolor.red += del;
  
  if (xcolor.green > limval)
    xcolor.green = 65535;
  else
    xcolor.green += del;

  if (xcolor.blue > limval)
    xcolor.blue = 65535;
  else
    xcolor.blue += del;

  xcolor.flags = DoRed | DoGreen | DoBlue;
  
/* if read only colors, all we can do is allocate a different color.
   If read/write, then change the entry */

  if (!Hv_readOnlyColors)
    XStoreColor(Hv_display, Hv_colorMap, &xcolor);
  else if (XAllocColor(Hv_display, Hv_colorMap, &xcolor))
    Hv_colors[color] = xcolor.pixel;
}


/* ---------- Hv_AdjustAdjacentColors ----------- */

void  Hv_AdjustAdjacentColors(short color,
			      short del)

/* used in conjunction with highlighting */

{

  if (Hv_readOnlyColors) {
    fprintf(stderr, "Hv warning: cannot adjust colors for read-only visuals.\n");
    return;
  }

  if ((color < 1) || (color >= (Hv_numColors-1)))
    return;

  Hv_CopyColor(color-1, color);
  Hv_CopyColor(color+1, color);

  if (del > 0) {
    Hv_BrightenColor(color-1, del);
    Hv_BrightenColor(color+1, del);
  }
  else {
    Hv_DarkenColor(color-1, -del);
    Hv_DarkenColor(color+1, -del);
  }

}


/* ---------- Hv_CopyColor ----------- */

void  Hv_CopyColor(short dcolor,
		   short scolor)

{
  XColor   dxcolor, sxcolor;

/* if read only, the opoeration is exceedingly simple */

  if (Hv_readOnlyColors) {
    Hv_colors[dcolor] = Hv_colors[scolor];
    return;
  }

  sxcolor.pixel = Hv_colors[scolor];
  XQueryColor(Hv_display, Hv_colorMap, &sxcolor);

  dxcolor.pixel = Hv_colors[dcolor];
  dxcolor.red   = sxcolor.red;
  dxcolor.green = sxcolor.green;
  dxcolor.blue  = sxcolor.blue;
  dxcolor.flags = sxcolor.flags;

  XStoreColor(Hv_display, Hv_colorMap, &dxcolor);

}

/*-------- Hv_MatchVisual ----------*/

static Boolean Hv_MatchVisual(Display *dpy,
			      int scr,
			      int defdepth,
			      Visual *defvis,
			      int vclass)

{
  XVisualInfo    visinfo;
  
  if (XMatchVisualInfo(dpy, scr, defdepth, vclass, &visinfo))
    if (defvis == visinfo.visual) {
      Hv_visClass = vclass;
      return True;
    }

  return False;
}

/* ---------- Hv_MakeColorMap ----------- */

void Hv_MakeColorMap(void)

{

  int		i;
  Colormap      defcmap;
  Display	*dpy = XtDisplay(Hv_toplevel);		/* display ID */
  int		scr = DefaultScreen(dpy);	/* screen ID */

  Visual       *defvis;
  int          defdepth;
  Boolean      foundvis, extended;


/* get the default colormap */

  defcmap = DefaultColormap(dpy, scr);
  defdepth = DefaultDepth(dpy, scr);
  defvis = DefaultVisual(dpy, scr);
  Hv_colorMap = defcmap;

  foundvis = Hv_MatchVisual(dpy, scr, defdepth, defvis,
			     StaticColor);
  if (!foundvis)
    foundvis = Hv_MatchVisual(dpy, scr, defdepth, defvis,
			      PseudoColor);
  if (!foundvis)
    foundvis = Hv_MatchVisual(dpy, scr, defdepth, defvis,
			      TrueColor);
  if (!foundvis)
    foundvis = Hv_MatchVisual(dpy, scr, defdepth, defvis,
			      DirectColor);
  if (!foundvis)
    foundvis = Hv_MatchVisual(dpy, scr, defdepth, defvis,
			      GrayScale);
  if (!foundvis)
    foundvis = Hv_MatchVisual(dpy, scr, defdepth, defvis,
			      StaticGray);

  if (!foundvis) {
    fprintf(stderr, "Hv Warning: Unknown visual type. Will try to continue.\n");
    Hv_visClass = PseudoColor;
  }
  
  Hv_readOnlyColors = 
    ((Hv_visClass == StaticGray) ||
     (Hv_visClass == StaticColor) ||
     (Hv_visClass == TrueColor));

  Hv_CheckColorMap();

/* these colors go behind the rainbow */
  
  extended = (Hv_numColors == Hv_EXTENDEDCOLORS);

  if (extended)
      i = Hv_numColors - 83;
  else
      i = Hv_numColors - 50;

  Hv_StuffColor(NULL, 63350,  63350,  63350, i);  Hv_gray15 = i++;
  Hv_StuffColor(NULL, 61166,  61166,  61166, i);  Hv_gray14 = i++;
  Hv_StuffColor(NULL, 56797,  56797,  56797, i);  Hv_gray13 = i++;
  Hv_StuffColor(NULL, 52428,  52428,  52428, i);  Hv_gray12 = i++;
  Hv_StuffColor(NULL, 48059,  48059,  48059, i);  Hv_gray11 = i++;
  Hv_StuffColor(NULL, 43690,  43690,  43690, i);  Hv_gray10 = i++;
  Hv_StuffColor(NULL, 39321,  39321,  39321, i);  Hv_gray9 = i++;
  Hv_StuffColor(NULL, 34952,  34952,  34952, i);  Hv_gray8 = i++;
  Hv_StuffColor(NULL, 30583,  30583,  30583, i);  Hv_gray7 = i++;
  Hv_StuffColor(NULL, 26214,  26214,  26214, i);  Hv_gray6 = i++;
  Hv_StuffColor(NULL, 21845,  21845,  21845, i);  Hv_gray5 = i++;
  Hv_StuffColor(NULL, 17476,  17476,  17476, i);  Hv_gray4 = i++;
  Hv_StuffColor(NULL, 13107,  13107,  13107, i);  Hv_gray3 = i++;
  Hv_StuffColor(NULL, 8738,   8738,   8738,  i);  Hv_gray2 = i++;
  Hv_StuffColor(NULL, 4369,   4369,   4369,  i);  Hv_gray1 = i++;

  Hv_StuffColor("Black",           0, 0, 0, i);  Hv_black = i++;
  Hv_StuffColor("NavyBlue",        0, 0, 0, i);  Hv_navyBlue = i++;

  Hv_canvasColor = i++;   /* 18th for changing the background*/


/* some colors are defined for the non-extended case, in the extended case they
   are synonyms and thus free us up for better gradients */

  if (!extended) {
      Hv_StuffColor("DarkOliveGreen3", 0, 0, 0, i);  Hv_olive = i++;
      Hv_StuffColor("green2",          0, 0, 0, i);  Hv_mediumForestGreen = i++;
      Hv_StuffColor("green3",          0, 0, 0, i);  Hv_darkGreen = i++;
      Hv_StuffColor("SeaGreen",        0, 0, 0, i);  Hv_seaGreen = i++;
  }

  Hv_StuffColor("DarkGreen",     0, 0, 0, i);  Hv_forestGreen = i++;

  if (extended)
      i+= 17;  /* for green  to wheat transition */

  Hv_StuffColor("wheat3",          0, 0, 0, i);  Hv_wheat = i++;

  if (extended)
      i+= 8;  /* for wheat to brown transition */

  Hv_StuffColor("Brown",           0, 0, 0, i);  Hv_brown = i++;

  if (extended)
      i+= 7;  /* for br to coral transition */

/*  Hv_StuffColor("lightcoral",           0, 0, 0, i);  Hv_coral = i++;*/
  Hv_StuffColor("brown1",           0, 0, 0, i);  Hv_coral = i++;

  if (extended)
      i+= 12;  /* for coral to gray15 transition */


/* some colors are defined for the non-extended case, in the extended case they
   are synonyms and thus free us up for better gradients */


  if (!extended) {
      Hv_StuffColor("lavender",        0, 0, 0, i);  Hv_lavender = i++;
      Hv_StuffColor("SkyBlue",         0, 0, 0, i);  Hv_skyBlue = i++;
      Hv_StuffColor("CadetBlue",       0, 0, 0, i);  Hv_cadetBlue = i++;
      Hv_StuffColor("RoyalBlue",       0, 0, 0, i);  Hv_royalBlue = i++;
      Hv_StuffColor("SteelBlue",       0, 0, 0, i);  Hv_steelBlue = i++;
      Hv_StuffColor("DodgerBlue2",     0, 0, 0, i);  i++;
      Hv_StuffColor("lightsteelblue",  0, 0, 0, i);  Hv_slateBlue = i++;
  }

  Hv_StuffColor("honeydew2",       0, 0, 0, i);  Hv_honeydew = i++; 

  if (extended)
      i+= 9;  /* for honeydew to skyblue transition */

  Hv_StuffColor("AliceBlue",       0, 0, 0, i);  Hv_aliceBlue = i++;
  Hv_StuffColor("PowderBlue",      0, 0, 0, i);  Hv_powderBlue = i++; 
  Hv_StuffColor("DodgerBlue",         0, 0, 0, i);  Hv_dodgerBlue = i++;
  Hv_StuffColor("DarkSeaGreen3",   0, 0, 0, i);  Hv_darkSeaGreen = i++;
  Hv_StuffColor("LightSeaGreen",   0, 0, 0, i);  Hv_lightSeaGreen = i++;
  Hv_StuffColor("LawnGreen",       0, 0, 0, i);  Hv_lawnGreen = i++;
  Hv_StuffColor("Aquamarine",      0, 0, 0, i);  Hv_aquaMarine = i++;


/* some colors are defined for the non-extended case, in the extended case they
   are synonyms and thus free us up for better gradients */
  
  if (!extended) {
      Hv_StuffColor("RosyBrown",       0, 0, 0, i);  Hv_rosyBrown = i++;
      Hv_StuffColor("SandyBrown",      0, 0, 0, i);  Hv_sandyBrown = i++;
      Hv_StuffColor("Tan3",            0, 0, 0, i);  Hv_tan3 = i++;
      Hv_StuffColor("Tan",             0, 0, 0, i);  Hv_tan = i++;
      Hv_StuffColor("Chocolate",       0, 0, 0, i);  Hv_chocolate = i++;
      Hv_StuffColor("Maroon",          0, 0, 0, i);  Hv_maroon = i++;
      Hv_StuffColor("Peru",            0, 0, 0, i);  Hv_peru = i++;
      Hv_StuffColor("PeachPuff",       0, 0, 0, i);  Hv_peachPuff = i++;
      Hv_StuffColor("MistyRose",       0, 0, 0, i);  Hv_mistyRose = i++;
  }




/* now make the rainbow */

  Hv_rainbowFirst = 0;

/* these will be part of the rainbow (the "anchors") */

  i = Hv_rainbowFirst;

  if (extended) {
      Hv_rainbowLast = 60;
      Hv_StuffColor("Red",    0, 0, 0, i);      Hv_red = i;         i = Hv_rainbowFirst + 5;
      Hv_StuffColor("Orange", 0, 0, 0, i);      Hv_orange = i;      i = Hv_rainbowFirst + 11;
      Hv_StuffColor("Yellow", 0, 0, 0, i);      Hv_yellow = i;      i = Hv_rainbowFirst + 21;
      Hv_StuffColor("SeaGreen", 0, 0, 0, i);      Hv_green = i;       i = Hv_rainbowFirst + 30;


      Hv_StuffColor("SkyBlue",      0, 0, 0, i);  Hv_skyBlue = i;   i = Hv_rainbowFirst + 35;
      Hv_steelBlue = Hv_skyBlue + 4;


      Hv_StuffColor("CornflowerBlue", 0, 0, 0, i);  Hv_cornFlowerBlue = i;  i = Hv_rainbowFirst + 46;
      Hv_StuffColor("mediumblue", 0, 0, 0, i);        Hv_blue = i;        i = Hv_rainbowFirst + 50;

      Hv_StuffColor("purple", 0, 0, 0, i);  Hv_purple = i;  i = Hv_rainbowFirst + 54;


      Hv_StuffColor("Violet", 0, 0, 0, i);      Hv_violet =  i;     i = Hv_rainbowFirst + 60;
      Hv_StuffColor("White", 0, 0, 0, i);       Hv_white = i;
  }
  else {
      Hv_rainbowLast = 29;
      Hv_StuffColor("Red",    0, 0, 0, i);      Hv_red = i;         i = Hv_rainbowFirst + 3;
      Hv_StuffColor("Orange", 0, 0, 0, i);      Hv_orange = i;      i = Hv_rainbowFirst + 7;
      Hv_StuffColor("Yellow", 0, 0, 0, i);      Hv_yellow = i;      i = Hv_rainbowFirst + 12;
      Hv_StuffColor("ForestGreen", 0, 0, 0, i);      Hv_green = i;       i = Hv_rainbowFirst + 16;
      Hv_StuffColor("CornflowerBlue", 0, 0, 0, i);  Hv_cornFlowerBlue = i;  i = Hv_rainbowFirst + 20;
      Hv_StuffColor("mediumblue", 0, 0, 0, i);        Hv_blue = i;        i = Hv_rainbowFirst + 24;

      Hv_StuffColor("Purple", 0, 0, 0, i);  Hv_purple = i;  i = Hv_rainbowFirst + 26;

      Hv_StuffColor("Violet", 0, 0, 0, i);      Hv_violet =  i;     i = Hv_rainbowFirst + 29;
      Hv_StuffColor("White", 0, 0, 0, i);       Hv_white = i;
  }


/* Fill in the gaps */

  Hv_FillGaps(Hv_red,    Hv_orange, -1, -1);
  Hv_FillGaps(Hv_orange, Hv_yellow, -1, -1);
  Hv_FillGaps(Hv_yellow, Hv_green, -1, -1);


  if (extended) {
      Hv_FillGaps(Hv_green,  Hv_skyBlue, -1, -1);
      Hv_FillGaps(Hv_skyBlue, Hv_cornFlowerBlue, -1, -1);
  }
  else {
      Hv_FillGaps(Hv_green,  Hv_cornFlowerBlue, -1, -1);
  }

  Hv_FillGaps(Hv_cornFlowerBlue,  Hv_blue, -1, -1);
  Hv_FillGaps(Hv_blue,  Hv_purple, -1, -1);

  Hv_FillGaps(Hv_purple,   Hv_violet, -1, -1);
  Hv_FillGaps(Hv_violet, Hv_white, -1, -1);


  if (extended) {
      Hv_FillGaps(Hv_forestGreen, Hv_wheat, Hv_forestGreen, 17);
      Hv_FillGaps(Hv_wheat, Hv_brown, Hv_wheat, 8);
      Hv_FillGaps(Hv_brown, Hv_coral, Hv_brown, 7);
      Hv_FillGaps(Hv_coral, Hv_gray15, Hv_coral, 12);
      Hv_FillGaps(Hv_honeydew, Hv_skyBlue, Hv_honeydew, 9);
  }

/* synonyms */

  Hv_gray0 = Hv_black;
  Hv_gray16 = Hv_white;
  if (extended) {
      Hv_lavender = Hv_violet+4;
      Hv_slateBlue = Hv_green+6;
      Hv_darkBlue = Hv_navyBlue;
      Hv_royalBlue = Hv_blue-5;
      Hv_cadetBlue = Hv_blue-11;
      Hv_olive = Hv_forestGreen + 5;
      Hv_mediumForestGreen = Hv_forestGreen + 3;
      Hv_darkGreen = Hv_forestGreen + 1;
      Hv_tan = Hv_wheat+3;
      Hv_tan = Hv_wheat+5;
      Hv_sandyBrown = Hv_wheat+6;
      Hv_peru = Hv_brown + 4;
      Hv_maroon = Hv_brown + 7;
      Hv_chocolate = Hv_brown + 2;
      Hv_rosyBrown = Hv_coral + 4;
      Hv_peachPuff = Hv_coral + 4;
      Hv_mistyRose = Hv_coral + 4;
  }

  Hv_highlight = 1001;

/* defaults for simple text editing */

  Hv_simpleTextBg = Hv_cadetBlue;
  Hv_simpleTextFg = Hv_black;
  Hv_simpleTextBorder = Hv_black;

  Hv_ColorizeCursors(dpy);
}




/*------- Hv_ColorizeCursors --------*/

static void Hv_ColorizeCursors(Display *dpy)

/* colorize the cross cursor so that it shows up better*/

{
  XColor  black, yellow, Dummy;


  XLookupColor(dpy, Hv_colorMap, "Black",  &black,  &Dummy);
  XLookupColor(dpy, Hv_colorMap, "Yellow", &yellow, &Dummy);

  XRecolorCursor(dpy, Hv_crossCursor,   &black, &yellow);
  XRecolorCursor(dpy, Hv_waitCursor,    &black, &yellow);
  XRecolorCursor(dpy, Hv_dotboxCursor,  &black, &yellow);

  XFlush(dpy);
}

/* -------- Hv_StuffColor --------------- */

static void  Hv_StuffColor(char *name,
			   unsigned int R,
			   unsigned int G,
			   unsigned int B,
			   int i)

/* Take the RGB components and stuff them into a color structure */

{
  XColor         color, dummy;
  int            status;
  Display	*dpy = XtDisplay(Hv_toplevel);

  color.red =   (unsigned short)R;
  color.green = (unsigned short)G;
  color.blue =  (unsigned short)B;
  color.flags = DoRed | DoGreen | DoBlue;

/* for read only colormaps, attempt to allocate a read only cell.
   For ReadWrite maps, the cells have been allocated and the
   pixels are already in Hv_colors */

  if (Hv_readOnlyColors) {
    if (name)
      status = XAllocNamedColor(dpy, Hv_colorMap, name, &color, &dummy);
    else
      status = XAllocColor(dpy, Hv_colorMap, &color);
    
    if (status != 0)
      Hv_colors[i] = color.pixel;
    else {   /*FAILURE */
      fprintf(stderr, "Hv Error: cannot allocate required color.\n");
      exit (0);
    }
  }


  else { /* read write colormap */
    color.pixel = Hv_colors[i];
    if (name)
      XStoreNamedColor(dpy, Hv_colorMap, name,
				color.pixel, color.flags);
    else
      XStoreColor(dpy, Hv_colorMap, &color);
  }
  XFlush(dpy);
}


/* -------- Hv_FillGaps ------ */

static void Hv_FillGaps(short cStart,
			short cEnd,
			short start,
			short nstep)

/* if start < 0, fills in place between cstart and cend, otherwise
   starts at "start" and creates nstep transtional colors */

{
  int        Ro, Go, Bo;
  int        Rf, Gf, Bf;
  int        R, G, B;
  short      stindex;
  XColor     color1, color2;
  Display   *dpy = XtDisplay(Hv_toplevel);		/* display ID */

  int Rdel, Gdel, Bdel, i;
  short    steps;

  if (start < 0) {
      steps = cEnd - cStart - 1;
      stindex = cStart;
  }
  else {
      steps  = nstep;
      stindex = start - 1;
  }

  color1.pixel = Hv_colors[cStart];
  color2.pixel = Hv_colors[cEnd];

  XQueryColor(dpy, Hv_colorMap, &color1);
  XQueryColor(dpy, Hv_colorMap, &color2);

  Ro = (int)(color1.red);  Go = (int)(color1.green);  Bo = (int)(color1.blue);
  Rf = (int)(color2.red);  Gf = (int)(color2.green);  Bf = (int)(color2.blue);

  Rdel = (Rf - Ro)/(steps + 1);
  Gdel = (Gf - Go)/(steps + 1);
  Bdel = (Bf - Bo)/(steps + 1);

  for (i = 1; i <= steps; i++) {
    R = Ro + i*Rdel;
    G = Go + i*Gdel;
    B = Bo + i*Bdel;
    Hv_StuffColor(NULL, (unsigned int)R, (unsigned int)G, (unsigned int)B, cStart+i); 
  }
}

#define   UPFRONTCOLORS   80

/*-------- Hv_CheckColorMap ------*/

static void Hv_CheckColorMap(void)

{
  int           status, status2;
  Display      *dpy = XtDisplay(Hv_toplevel);
  int	        scr = DefaultScreen(dpy);
  Colormap      defcmap;
  XColor        tempcolors[UPFRONTCOLORS];
  int           upfrontcolors = UPFRONTCOLORS;
  unsigned long pixels[UPFRONTCOLORS];
  int           i;

  if (Hv_readOnlyColors)
    return;

  defcmap = DefaultColormap(dpy, scr);

  status = XAllocColorCells(dpy, defcmap, True, NULL, 0,
			    Hv_colors, Hv_numColors);

/* if status == 0 we will need a new color map */

  if (status == 0) {
    fprintf(stderr, "Hv will create a new virtual colormap.\n");
    Hv_colorMap = XCreateColormap(dpy, DefaultRootWindow(dpy),
				  DefaultVisual(dpy,scr), AllocNone);

    status  = XAllocColorCells(dpy, Hv_colorMap, True, NULL, 0,
			      pixels, upfrontcolors);

    status2 = XAllocColorCells(dpy, Hv_colorMap, True, NULL, 0,
			      Hv_colors, Hv_numColors);

    if ((status == 0) || (status2 == 0)) {
      fprintf(stderr, "Hv failed to allocate colors in its own colormap.\n");
      exit (0);
    }

    for (i = 0; i < upfrontcolors; i++) {
      tempcolors[i].pixel = pixels[i];
      tempcolors[i].flags = DoRed | DoGreen | DoBlue;
    }

    XQueryColors(dpy, defcmap, tempcolors, upfrontcolors);
    XStoreColors(dpy, Hv_colorMap, tempcolors, upfrontcolors);

    XtVaSetValues(Hv_toplevel, XmNcolormap, Hv_colorMap, NULL);
    XtVaSetValues(Hv_form, XmNcolormap, Hv_colorMap, NULL);
    
  }
  else
    fprintf(stderr, "Hv will use the default colormap.\n");
}

#undef UPFRONTCOLORS
E 1
