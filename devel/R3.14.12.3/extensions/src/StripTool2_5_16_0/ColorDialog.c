/*************************************************************************\
* Copyright (c) 1994-2004 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 1997-2003 Southeastern Universities Research Association,
* as Operator of Thomas Jefferson National Accelerator Facility.
* Copyright (c) 1997-2002 Deutches Elektronen-Synchrotron in der Helmholtz-
* Gemelnschaft (DESY).
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

#include "ColorDialog.h"
#include "StripMisc.h"

#include <X11/Xlib.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/DrawingA.h>
#include <Xm/Scale.h>
#include <Xm/PushB.h>
#include <Xm/ArrowB.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/Separator.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#if !defined (PI)
#define PI      3.14159265358979323846
#endif

#define DEF_WOFFSET                     3

/* the minimum/maximum values for the sides of the palette entry rectangles */
#define PALETTE_ENTRY_SIDE_MM_MIN       3.5
#define PALETTE_ENTRY_SIDE_MM_MAX       4.5

typedef enum
{
  Red, Green, Blue, NUM_COLOR_COMPONENTS
}
ColorComponent;

#define Grey    NUM_COLOR_COMPONENTS

char    *ColorComponentStr[NUM_COLOR_COMPONENTS] =
{
  "Red", "Green", "Blue"
};

/*
 * Structure used for sorting colors into a useful visual palette
 *
 *      H, S, I ==> Hue, Saturation, Intensity
 */
typedef struct _ColorSortInfo
{
  XColor        xcolor;
  float         H, S, I;
}
ColorSortInfo;

typedef struct
{
  Display               *display;
  Colormap              cmap;
  GC                    gc;

  struct _palette
  {
    Widget              canvas;
    Pixmap              pixmap;
    Dimension           w, h;
    Dimension           w_entry, h_entry;
    XColor              colors[CCM_MAX_PALETTE_SIZE];
    int                 n_colors;
    int                 n_entries;
    int                 shift;
    int                 built;
  }                     palette;
    
  Widget                shell;
  Widget                lbl;
  Widget                cell_lbl;
  Widget                cell_btn;
  Widget                slider[NUM_COLOR_COMPONENTS];
  Widget                color_area;
  Widget                arrow_left, arrow_right;
  Widget                btn_ok, btn_apply, btn_dismiss, btn_cancel;
  Widget                message_box;
  int                   local_writable;
  cColor                color, orig_color;
  cColor                white, black;
  StripConfig           *config;
  CDcallback            user_func;
  void                  *user_arg;
}
ColorDialogInfo;

/* ====== Callback Functions ====== */
static void     cellbtn_event   (Widget, XtPointer, XtPointer);
static void     slider_event    (Widget, XtPointer, XtPointer);
static void     ctrlbtn_event   (Widget, XtPointer, XtPointer);
static void     canvas_event    (Widget, XtPointer, XtPointer);
static void     arrow_event     (Widget, XtPointer, XtPointer);


/* ====== static functions ====== */
/*
 * paint_palette
 */
static void     paint_palette   (ColorDialogInfo *cd)
{
  int           i, j, x, w;
  
  /* clear the pixmap by painting it black */
  XSetForeground
    (cd->display, cd->gc, BlackPixel (cd->display, cd->config->xvi.screen));
  XFillRectangle
    (cd->display, cd->palette.pixmap, cd->gc,
     0, 0, cd->palette.w + 1, cd->palette.h + 1);

  j = cd->palette.shift * cd->palette.n_entries;
  
  x = 0;
  w = (int)(cd->palette.w / (float)cd->palette.w_entry);/* entries per row */
  w *= cd->palette.w_entry;                             /* multiple of w_entry */
  for (i = 0;
       (i < cd->palette.n_entries) && ((i+j) < cd->palette.n_colors);
       i++)
  {
    XSetForeground (cd->display, cd->gc, cd->palette.colors[i+j].pixel);
    XFillArc
      (cd->display, cd->palette.pixmap, cd->gc,
       x % w,
       (x / w) * cd->palette.h_entry,
       cd->palette.w_entry, cd->palette.h_entry, 0, 360 * 64);
    x += cd->palette.w_entry;
  }
}


/*
 * set_sliders
 */
static void     set_sliders     (ColorDialogInfo *cd, XColor *xc)
{
  XmScaleSetValue (cd->slider[Red], xc->red);
  XmScaleSetValue (cd->slider[Green], xc->green);
  XmScaleSetValue (cd->slider[Blue], xc->blue);
}


/*
 * display_color
 */
static void     display_color   (ColorDialogInfo *cd, XColor *xcolor)
{
  float x, y;
  float r, g, b;
  Pixel pixel;
  
  XtVaSetValues (cd->color_area, XmNbackground, xcolor->pixel, NULL);
  XtVaSetValues (cd->cell_lbl, XmNbackground, xcolor->pixel, NULL);
  XtVaSetValues (cd->lbl, XmNbackground, xcolor->pixel, NULL);

  r = (float)(cd->white.xcolor.red - xcolor->red);
  g = (float)(cd->white.xcolor.green - xcolor->green);
  b = (float)(cd->white.xcolor.blue - xcolor->blue);
  x = (r*r) + (g*g) + (b*b);
  
  r = (float)(cd->black.xcolor.red - xcolor->red);
  g = (float)(cd->black.xcolor.green - xcolor->green);
  b = (float)(cd->black.xcolor.blue - xcolor->blue);
  y = (r*r) + (g*g) + (b*b);

  pixel = (x > y? cd->white.xcolor.pixel : cd->black.xcolor.pixel);
  XtVaSetValues (cd->cell_lbl, XmNforeground, pixel, NULL);
  XtVaSetValues (cd->lbl, XmNforeground, pixel, NULL);
}


/*
 * apply_color
 */
static void     apply_color     (ColorDialogInfo *cd, XColor *xc)
{
  int   stat;
  
  cd->color.xcolor.red = xc->red;
  cd->color.xcolor.green = xc->green;
  cd->color.xcolor.blue = xc->blue;
  
  if (cd->color.writable)
    cColorManager_change_color (cd->config->scm, &cd->color);
  else
  {
    stat = cColorManager_make_color
      (cd->config->scm, &cd->color, 0, CCM_RO | CCM_MATCH_RGB);
    if (stat) display_color (cd, &cd->color.xcolor);
  }
}


/*
 * apply_cellstat
 */
static void     apply_cellstat  (ColorDialogInfo *cd, int writable)
{
  XmString      xstr_lbl;
  XmString      xstr_btn;

  if (writable)
  {
    xstr_lbl = XmStringCreateLocalized ("This cell is writable");
    xstr_btn = XmStringCreateLocalized ("Free cell to X-server");
  }
  else
  {
    xstr_lbl = XmStringCreateLocalized ("This cell is read-only");
    xstr_btn = XmStringCreateLocalized ("Get cell from X-server");
  }

  XtVaSetValues (cd->cell_lbl, XmNlabelString, xstr_lbl, NULL);
  XtVaSetValues (cd->cell_btn, XmNlabelString, xstr_btn, NULL);
  XmStringFree (xstr_lbl);
  XmStringFree (xstr_btn);
}


/*
 * reset_stuff
 */
static void     reset_stuff     (ColorDialogInfo *cd)
{
  /* if the current color cell was locally allocated, free it.
   * Likewise, if the current original color was writable, we
   * need to free it (i.e., decrement its reference count) */
  if (cd->local_writable)
    cColorManager_free_color (cd->config->scm, &cd->color);
  if (cd->orig_color.writable)
    cColorManager_free_color (cd->config->scm, &cd->orig_color);
  cd->local_writable = 0;
  cd->orig_color.writable = 0;
}


#define GRAY_SATURATION .05
#define GRAY_INTENSITY  .05
int     compare_hsi     (const void *pa, const void *pb)
{
  ColorSortInfo *a = (ColorSortInfo *)pa;
  ColorSortInfo *b = (ColorSortInfo *)pb;

  /* make sure that all greys clump together at the end */
  if ((a->I < GRAY_INTENSITY) || (a->S < GRAY_SATURATION))
  {
    if ((b->I < GRAY_INTENSITY) || (b->S < GRAY_SATURATION))
    {
      if (a->I < b->I)
        return -1;
      else return (a->I > b->I);
    }
    else return 1;
  }
  else if ((b->I < GRAY_INTENSITY) || (b->S < GRAY_SATURATION))
    return -1;

  /* now compare by Hue, then intensity then saturation */
  if (a->H < b->H)
    return -1;
  else if (a->H > b->H)
    return 1;
  else if (a->I < b->I)
    return -1;
  else if (a->I > b->I)
    return 1;
  else if (a->S < b->S)
    return -1;
  else return (a->S > b->S);
}


/*
 * ColorDialog_init
 */
ColorDialog     ColorDialog_init        (Widget                 parent,
                                         char                   *title,
                                         StripConfig            *config)
{
  ColorDialogInfo       *cd;
  Widget                base_form, form, top, bot, rowcol;
  Widget                w;
  int                   i, a, b, l, z;

  if ((cd = (ColorDialogInfo *)malloc (sizeof (ColorDialogInfo))) != NULL)
  {
    cd->display = XtDisplay (parent);
    cd->config = config;
    cd->cmap = cColorManager_getcmap (config->scm);
    cd->palette.built = 0;
    cd->user_func = 0;
    cd->user_arg = 0;
    cd->local_writable = 0;
    cd->message_box = 0;
    cd->orig_color.writable = 0;

    /* determine the min and max RGB values for this display */
    cColorManager_make_color
      (cd->config->scm, &cd->black, "Black", CCM_RO | CCM_MATCH_STR);
    cColorManager_make_color
      (cd->config->scm, &cd->white, "White", CCM_RO | CCM_MATCH_STR);

    cd->shell = XtVaCreatePopupShell
      ("ColorDialog",
       topLevelShellWidgetClass,        parent,
       XmNdeleteResponse,               XmUNMAP,
       XmNmappedWhenManaged,            False,
       XmNtitle,                        title == NULL? "Modify Color" : title,
       XmNvisual,                       config->xvi.visual,
       XmNcolormap,                     cd->cmap,
       NULL);
    
    base_form = XtVaCreateManagedWidget
      ("form",
       xmFormWidgetClass,               cd->shell,
       XmNfractionBase,                 9,
       XmNnoResize,                     True,
       XmNresizable,                    False,
       XmNresizePolicy,                 XmRESIZE_NONE,
       XmNverticalSpacing,              DEF_WOFFSET,
       XmNhorizontalSpacing,            DEF_WOFFSET,
       NULL);

    /* button row along bottom
     */
    rowcol = XtVaCreateManagedWidget
      ("buttonRowColumn",
       xmRowColumnWidgetClass,          base_form,
       XmNtopAttachment,                XmATTACH_NONE,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_FORM,
       NULL);

    cd->btn_ok = XtVaCreateManagedWidget
      ("OK", xmPushButtonWidgetClass, rowcol, NULL);
    XtAddCallback (cd->btn_ok, XmNactivateCallback, ctrlbtn_event, cd);

    cd->btn_apply = XtVaCreateManagedWidget
      ("Apply", xmPushButtonWidgetClass, rowcol, NULL);
    XtAddCallback (cd->btn_apply, XmNactivateCallback, ctrlbtn_event, cd);
      
    cd->btn_dismiss = XtVaCreateManagedWidget
      ("Dismiss", xmPushButtonWidgetClass, rowcol, NULL);
    XtAddCallback (cd->btn_dismiss, XmNactivateCallback, ctrlbtn_event, cd);
      
    cd->btn_cancel = XtVaCreateManagedWidget
      ("Cancel", xmPushButtonWidgetClass, rowcol, NULL);
    XtAddCallback (cd->btn_cancel, XmNactivateCallback, ctrlbtn_event, cd);
    
    /* separator above button row
     */
    bot = XtVaCreateManagedWidget
      ("bottomSeparator",
       xmSeparatorWidgetClass,          base_form,
       XmNtopAttachment,                XmATTACH_NONE,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_WIDGET,
       XmNbottomWidget,                 rowcol,
       NULL);

    cd->cell_btn = XtVaCreateManagedWidget
      ("cellStatusButton",
       xmPushButtonWidgetClass,         base_form,
       XmNtopAttachment,                XmATTACH_NONE,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_WIDGET,
       XmNbottomWidget,                 bot,
       XmNbottomOffset,                 DEF_WOFFSET,
       XmNalignment,                    XmALIGNMENT_CENTER,
       XmNsensitive,
       (Bool) !cColorManager_readonly (cd->config->scm),
       NULL);
    XtAddCallback (cd->cell_btn, XmNactivateCallback, cellbtn_event, cd);

    bot = XtVaCreateManagedWidget
      ("bottomSeparator",
       xmSeparatorWidgetClass,          base_form,
       XmNtopAttachment,                XmATTACH_NONE,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_WIDGET,
       XmNbottomWidget,                 cd->cell_btn,
       NULL);


    /* color palette
     */
    form = XtVaCreateManagedWidget
      ("colorPaletteForm",
       xmFormWidgetClass,               base_form,
       XmNtopAttachment,                XmATTACH_FORM,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_NONE,
       NULL);

    cd->arrow_left = XtVaCreateManagedWidget
      ("arrowLeft",
       xmArrowButtonWidgetClass,        form,
       XmNtopAttachment,                XmATTACH_FORM,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_NONE,
       XmNbottomAttachment,             XmATTACH_FORM,
       XmNarrowDirection,               XmARROW_LEFT,
       NULL);
    cd->arrow_right = XtVaCreateManagedWidget
      ("arrowRight",
       xmArrowButtonWidgetClass,        form,
       XmNtopAttachment,                XmATTACH_FORM,
       XmNleftAttachment,               XmATTACH_NONE,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_FORM,
       XmNarrowDirection,               XmARROW_RIGHT,
       NULL);
    XtAddCallback (cd->arrow_left, XmNactivateCallback, arrow_event, cd);
    XtAddCallback (cd->arrow_right, XmNactivateCallback, arrow_event, cd);

    cd->palette.canvas = XtVaCreateManagedWidget
      ("colorPaletteDrawingArea",
       xmDrawingAreaWidgetClass,        form,
       XmNtopAttachment,                XmATTACH_FORM,
       XmNleftAttachment,               XmATTACH_WIDGET,
       XmNleftWidget,                   cd->arrow_left,
       XmNrightAttachment,              XmATTACH_WIDGET,
       XmNrightWidget,                  cd->arrow_right,
       XmNbottomAttachment,             XmATTACH_NONE,
       NULL);
    XtAddCallback (cd->palette.canvas, XmNinputCallback, canvas_event, cd);
    XtAddCallback (cd->palette.canvas, XmNexposeCallback, canvas_event, cd);

    /* separator underneath color palette
     */
    top = XtVaCreateManagedWidget
      ("topSeparator",
       xmSeparatorWidgetClass,          base_form,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    form,
       XmNtopOffset,                    DEF_WOFFSET,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_NONE,
       NULL);


    /* color status and RGB controls
     */
    form = XtVaCreateManagedWidget
      ("rgbForm",
       xmFormWidgetClass,               base_form,
       XmNshadowType,                   XmSHADOW_ETCHED_IN,
       XmNresizePolicy,                 XmRESIZE_NONE,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    top,
       XmNtopOffset,                    DEF_WOFFSET,
       XmNleftAttachment,               XmATTACH_NONE,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_WIDGET,
       XmNbottomWidget,                 bot,
       XmNbottomOffset,                 DEF_WOFFSET,
       NULL);
    w = XtVaCreateManagedWidget
      ("Color Definition",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_FORM,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_NONE,
       XmNbottomAttachment,             XmATTACH_NONE,
       NULL);

    cd->slider[Red] = XtVaCreateManagedWidget
      ("redSlider",
       xmScaleWidgetClass,              form,
       XmNorientation,                  XmHORIZONTAL,
       XmNminimum,                      cd->black.xcolor.red,
       XmNmaximum,                      cd->white.xcolor.red,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_NONE,
       NULL);
    cd->slider[Green] = XtVaCreateManagedWidget
      ("greenSlider",
       xmScaleWidgetClass,              form,
       XmNorientation,                  XmHORIZONTAL,
       XmNminimum,                      cd->black.xcolor.green,
       XmNmaximum,                      cd->white.xcolor.green,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    cd->slider[Red],
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_NONE,
       NULL);
    cd->slider[Blue] = XtVaCreateManagedWidget
      ("blueSlider",
       xmScaleWidgetClass,              form,
       XmNorientation,                  XmHORIZONTAL,
       XmNminimum,                      cd->black.xcolor.blue,
       XmNmaximum,                      cd->white.xcolor.blue,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    cd->slider[Green],
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_NONE,
       NULL);

    for (i = 0; i < NUM_COLOR_COMPONENTS; i++)
    {
      XtAddCallback (cd->slider[i], XmNdragCallback, slider_event, cd);
      XtAddCallback (cd->slider[i], XmNvalueChangedCallback, slider_event, cd);
    }

    w = form;
    cd->color_area = XtVaCreateManagedWidget
      ("cellColorInfoForm",
       xmFormWidgetClass,               base_form,
       XmNshadowType,                   XmSHADOW_ETCHED_IN,
       XmNresizePolicy,                 XmRESIZE_NONE,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    top,
       XmNtopOffset,                    DEF_WOFFSET,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_WIDGET,
       XmNrightWidget,                  form,
       XmNbottomAttachment,             XmATTACH_WIDGET,
       XmNbottomWidget,                 bot,
       XmNbottomOffset,                 DEF_WOFFSET,
       NULL);

    cd->lbl = XtVaCreateManagedWidget
      ("cellUserLabel",
       xmLabelWidgetClass,              cd->color_area,
       XmNtopAttachment,                XmATTACH_FORM,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_NONE,
       XmNbottomAttachment,             XmATTACH_NONE,
       NULL);
    cd->cell_lbl = XtVaCreateManagedWidget
      ("cellStatusLabel",
       xmLabelWidgetClass,              cd->color_area,
       XmNtopAttachment,                XmATTACH_NONE,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_FORM,
       NULL);
      
    XtRealizeWidget (cd->shell);

    /* create the palette pixmap, and graphics context */
    XtVaGetValues
      (cd->palette.canvas,
       XmNwidth,                        &cd->palette.w,
       XmNheight,                       &cd->palette.h,
       NULL);
    cd->palette.pixmap = XCreatePixmap
      (cd->display, XtWindow (cd->palette.canvas),
       cd->palette.w, cd->palette.h, config->xvi.depth);

    cd->gc = XCreateGC (cd->display, XtWindow (cd->palette.canvas), 0, 0);

    
    /* Determine how many colors to show in the palette window at a time.
     * Minimize horizontal and vertical space wasted by choosing optimal
     * width and height for palette entries */

    /* horizontal
     * a is number of boxes per row if maximal width,
     * b is number of boxes per row if minimal width */
    a = (int)
      (cd->palette.w / (PALETTE_ENTRY_SIDE_MM_MAX * horizontal_pixels_per_mm));
    b = (int)
      (cd->palette.w / (PALETTE_ENTRY_SIDE_MM_MIN * horizontal_pixels_per_mm));
    z = cd->palette.w;
    while (a <= b)
    {
      l = cd->palette.w / a;
      if ((cd->palette.w % l) < z)
      {
        z = cd->palette.w % l;
        cd->palette.w_entry = l;
      }
      a++;
    }

    /* vertical
     * a is number of boxes per column if maximal height,
     * b is number of boxes per column if minimal height */
    a = (int)
      (cd->palette.h / (PALETTE_ENTRY_SIDE_MM_MAX * vertical_pixels_per_mm));
    b = (int)
      (cd->palette.h / (PALETTE_ENTRY_SIDE_MM_MIN * vertical_pixels_per_mm));
    z = cd->palette.h;
    while (a <= b)
    {
      l = cd->palette.h / a;
      if ((cd->palette.h % l) < z)
      {
        z = cd->palette.h % l;
        cd->palette.h_entry = l;
      }
      a++;
    }
    
    cd->palette.n_entries =
      (cd->palette.w / cd->palette.w_entry) *
      (cd->palette.h / cd->palette.h_entry);
  }
  return (ColorDialog)cd;
}


/*
 * ColorDialog_delete
 */
void            ColorDialog_delete      (ColorDialog the_cd)
{
  ColorDialogInfo       *cd = (ColorDialogInfo *)the_cd;
  XtDestroyWidget (cd->shell);
  XFreePixmap (cd->display, cd->palette.pixmap);
  XFreeGC (cd->display, cd->gc);
  free (cd);
}


/*
 * ColorDialog_popup
 */
void            ColorDialog_popup       (ColorDialog    the_cd,
                                         char           *title,
                                         cColor *color,
                                         CDcallback     func,
                                         void           *arg)
{
  static ColorSortInfo  hsi[CCM_MAX_PALETTE_SIZE];
  ColorDialogInfo       *cd = (ColorDialogInfo *)the_cd;
  XmString              str;
  float                 r, g, b, z;
  int                   i;

  reset_stuff (cd);

  /* save call items */
  cd->user_func = func;
  cd->user_arg = arg;
  cd->orig_color = cd->color = *color;
  
  /* if the given color is writable, then we need to increase its
   * reference count in case it is freed and then this action is
   * cancelled. */
  if (cd->orig_color.writable)
    cColorManager_keep_color (cd->config->scm, &cd->orig_color);
  
  /* set the sliders and apply the new color */
  set_sliders (cd, &color->xcolor);
  display_color (cd, &color->xcolor);
  apply_cellstat (cd, color->writable);

  /* set the frame title */
  str = XmStringCreateLocalized (title);
  XtVaSetValues (cd->lbl, XmNlabelString, str, NULL);
  XmStringFree (str);

  
  /* build palette, if not already built */
  if (!cd->palette.built)
  {
    cd->palette.n_colors = cColorManager_build_palette
      (cd->config->scm, cd->palette.colors, CCM_MAX_PALETTE_SIZE);
    cd->palette.built = 1;
    cd->palette.shift = 0;
    XtVaSetValues (cd->arrow_left, XmNsensitive, False, NULL);
    XtVaSetValues
      (cd->arrow_right,
       XmNsensitive, (Bool)(cd->palette.n_colors > cd->palette.n_entries),
       NULL);

    /* sort the palette */
    for (i = 0; i < cd->palette.n_colors; i++)
    {
      /* formula for RGB -> HSI requires normalized rgb
       * (see pg. 230, "Digital Image Processing", Gonzales & Woods) */
      r = (float)(cd->palette.colors[i].red / (double)cd->white.xcolor.red);
      g = (float)(cd->palette.colors[i].green / (double)cd->white.xcolor.green);
      b = (float)(cd->palette.colors[i].blue / (double)cd->white.xcolor.blue);

      /* Intensity */
      hsi[i].I = (r + g + b) / 3;
      
      /* z = min {r, g, b} */
      z = (r < g? r : g);
      z = (z < b? z : b);
      
      /* Saturation */
      hsi[i].S = 1 - ((3 / (r + g + b)) * z);
      
      /* Hue is only meaningful if saturation > 0 */
      if (hsi[i].S > FLT_EPSILON)
      {
        /* Hue, normalized to [0,1] */
        hsi[i].H = (float)((0.5) * ((r - g) + (r - b)));
        hsi[i].H /= (float)(sqrt (((r - g)*(r - g)) + ((r - b)*(g - b))));
        hsi[i].H = (float)(acos (hsi[i].H));
        if ((b / hsi[i].I) > (g / hsi[i].I))
          hsi[i].H = (float)((2*PI) - hsi[i].H);
        hsi[i].H /= (float)(2*PI);
      }

      hsi[i].xcolor = cd->palette.colors[i];
    }

    qsort (hsi, i, sizeof (ColorSortInfo), compare_hsi);
    for (i = 0; i < cd->palette.n_colors; i++)
      cd->palette.colors[i] = hsi[i].xcolor;
    paint_palette (cd);
  }

  XMapRaised (XtDisplay (cd->shell), XtWindow (cd->shell));
}


/*
 * ColorDialog_popdown
 */
void    ColorDialog_popdown     (ColorDialog the_cd)
{
  ColorDialogInfo       *cd = (ColorDialogInfo *)the_cd;

  XUnmapWindow (XtDisplay (cd->shell), XtWindow (cd->shell));

  reset_stuff (cd);
  cColorManager_free_palette (cd->config->scm);
  cd->palette.built = 0;
}


/* ====== Callback Functions ====== */
/*
 * cellbtn_event
 */
static void     cellbtn_event   (Widget         BOGUS(w),
                                 XtPointer      client_data,
                                 XtPointer      BOGUS(call_data))
{
  ColorDialogInfo       *cd = (ColorDialogInfo *)client_data;
  int                   stat;
  cColor                sc;

  /* if the current color is writable, we need to free it,
   * otherwise, try to grab a writable color cell */
  if (cd->color.writable)
  {
    /* only free writable colors which we allocate */
    if (cd->local_writable)
      cColorManager_free_color (cd->config->scm, &cd->color);
    cd->local_writable = 0;
    stat = cColorManager_make_color
      (cd->config->scm, &cd->color, 0, CCM_RO | CCM_MATCH_RGB);
    if (!stat) cd->color = cd->orig_color;
  }
  else
  {
    sc = cd->color;
    stat = cColorManager_make_color
      (cd->config->scm, &sc, 0, CCM_RW_FORCE | CCM_MATCH_RGB);
    if (stat)
    {
      cd->color = sc;
      cColorManager_keep_color (cd->config->scm, &cd->color);
      cd->local_writable = 1;
    }
    else
      MessageBox_popup
        (cd->shell, &cd->message_box, XmDIALOG_WARNING, "No Colors",
         "Yikes", "No available color cells!");
  }
  
  set_sliders (cd, &cd->color.xcolor);
  apply_cellstat (cd, cd->color.writable);
  display_color (cd, &cd->color.xcolor);
}


/*
 * slider_event
 */
static void     slider_event    (Widget         BOGUS(w),
                                 XtPointer      client_data,
                                 XtPointer      BOGUS(call_data))
{
  ColorDialogInfo       *cd = (ColorDialogInfo *)client_data;
  int                   val;
  XColor                xcolor;

  XmScaleGetValue (cd->slider[Red], &val);
  xcolor.red = val; 
  XmScaleGetValue (cd->slider[Green], &val);
  xcolor.green = val; 
  XmScaleGetValue (cd->slider[Blue], &val);
  xcolor.blue = val; 

  apply_color (cd, &xcolor);
}


/*
 * ctrlbtn_event
 */
static void     ctrlbtn_event   (Widget         w,
                                 XtPointer      client_data,
                                 XtPointer      BOGUS(1))
{
  ColorDialogInfo       *cd = (ColorDialogInfo *)client_data;

  if (w == cd->btn_dismiss)
  {
    ColorDialog_popdown (cd);
  }
  else if (w == cd->btn_ok)
  {
    cd->user_func (cd->user_arg, &cd->color);
    ColorDialog_popdown (cd);
  }
  else if (w == cd->btn_apply)
  {
    cd->user_func (cd->user_arg, &cd->color);
  }
  else if (w == cd->btn_cancel)
  {
    /* if we've modified the colormap entry referenced by the
     * original color, we need to reset it */
    cd->user_func (cd->user_arg, &cd->orig_color);
    cColorManager_change_color (cd->config->scm, &cd->orig_color);
    ColorDialog_popdown (cd);
  }
  else fprintf (stdout, "ColorDialog:ctrlbtn_event() ARGH!\n");
}


/*
 * canvas_event
 */
static void     canvas_event    (Widget         BOGUS(w),
                                 XtPointer      client_data,
                                 XtPointer      call_data)
{
  ColorDialogInfo               *cd = (ColorDialogInfo *)client_data;
  XmDrawingAreaCallbackStruct   *cbs = (XmDrawingAreaCallbackStruct *)call_data;
  int                           x, y, i;

  if (cbs->reason == XmCR_INPUT)
  {
    if (cbs->event->xany.type == ButtonPress)
    {
      x = cbs->event->xbutton.x;
      y = cbs->event->xbutton.y;

      x /= cd->palette.w_entry;                 /* column slected */
      y /= cd->palette.h_entry;                 /* row selected */

      i = (cd->palette.w / cd->palette.w_entry);/* num entries in a row */
      i *= y;
      i += x;
      i += (cd->palette.n_entries * cd->palette.shift);

      if (i < cd->palette.n_colors)
      {
        set_sliders (cd, &cd->palette.colors[i]);
        apply_color (cd, &cd->palette.colors[i]);
      }
    }
  }
  else if (cbs->reason == XmCR_EXPOSE)
  {
    XCopyArea
      (cd->display, cd->palette.pixmap, XtWindow(cd->palette.canvas),
       cd->gc, 0, 0, cd->palette.w, cd->palette.h, 0, 0);
  }
}


/*
 * arrow_event
 */
static void     arrow_event     (Widget         w,
                                 XtPointer      client_data,
                                 XtPointer      BOGUS(call_data))
{
  ColorDialogInfo       *cd = (ColorDialogInfo *)client_data;

  if (w == cd->arrow_left)
  {
    if (--cd->palette.shift == 0)
      XtVaSetValues (w, XmNsensitive, False, NULL);
    XtVaSetValues (cd->arrow_right, XmNsensitive, True, NULL);
  }
  else if (w == cd->arrow_right)
  {
    if (++cd->palette.shift == (cd->palette.n_colors / cd->palette.n_entries))
      XtVaSetValues (w, XmNsensitive, False, NULL);
    XtVaSetValues (cd->arrow_left, XmNsensitive, True, NULL);
  }

  paint_palette (cd);
  XCopyArea
    (cd->display, cd->palette.pixmap, XtWindow(cd->palette.canvas),
     cd->gc, 0, 0, cd->palette.w, cd->palette.h, 0, 0);
}
