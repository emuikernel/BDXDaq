/* Generated by wbuild from "XmTabs.w"
** (generator version $Revision: 1.4 $ of $Date: 1998/06/18 12:20:25 $)
*/
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include <X11/extensions/shape.h>
/*#include <Converters.h> */
#include <X11/Xmu/CharSet.h>
#include "rotated.h"
#include <XmTabsP.h>
#include <Balloon.h>
static void activate(
#if NeedFunctionPrototypes
Widget,XEvent*,String*,Cardinal*
#endif
);

static XtActionsRec actionsList[] = {
{"activate", activate},
};

static char defaultTranslations[] = "\
<Btn1Down>,<Btn1Up>: activate() \n\
";
static void _resolve_inheritance(
#if NeedFunctionPrototypes
WidgetClass
#endif
);
static void class_initialize(
#if NeedFunctionPrototypes
void
#endif
);
static void initialize(
#if NeedFunctionPrototypes
Widget ,Widget,ArgList ,Cardinal *
#endif
);

static Boolean  set_values(
#if NeedFunctionPrototypes
Widget ,Widget ,Widget,ArgList ,Cardinal *
#endif
);

static void realize(
#if NeedFunctionPrototypes
Widget,XtValueMask *,XSetWindowAttributes *
#endif
);

static void resize(
#if NeedFunctionPrototypes
Widget
#endif
);

static void expose(
#if NeedFunctionPrototypes
Widget,XEvent *,Region 
#endif
);

static void border_highlight(
#if NeedFunctionPrototypes
void
#endif
);

static void border_unhighlight(
#if NeedFunctionPrototypes
void
#endif
);

static void destroy(
#if NeedFunctionPrototypes
Widget
#endif
);

#define min(a, b) ((a )<(b )?(a ):(b ))



#define abs(x) ((x )<0 ?-(x ):(x ))



static void compute_tabsizes(
#if NeedFunctionPrototypes
Widget
#endif
);

static void comp_hor_tab_shape(
#if NeedFunctionPrototypes
Widget,int ,XPoint  p[12],int *,int *,int *
#endif
);

static void comp_ver_tab_shape(
#if NeedFunctionPrototypes
Widget,int ,XPoint  p[12],int *,int *,int *
#endif
);

static void draw_border(
#if NeedFunctionPrototypes
Widget,XPoint  poly[12]
#endif
);

static void draw_hor_tab(
#if NeedFunctionPrototypes
Widget,Region ,int 
#endif
);

static void draw_ver_tab(
#if NeedFunctionPrototypes
Widget,Region ,int 
#endif
);

static void create_topgc(
#if NeedFunctionPrototypes
Widget
#endif
);

static void create_bottomgc(
#if NeedFunctionPrototypes
Widget
#endif
);

static void create_textgc(
#if NeedFunctionPrototypes
Widget
#endif
);

static void create_fillgc(
#if NeedFunctionPrototypes
Widget
#endif
);

static void create_backgc(
#if NeedFunctionPrototypes
Widget
#endif
);

static void copy_bg(
#if NeedFunctionPrototypes
Widget,int ,XrmValue *
#endif
);

static void set_shape(
#if NeedFunctionPrototypes
Widget
#endif
);

#define done(type, value) do {\
	if (to->addr != NULL) {\
	    if (to->size < sizeof(type)) {\
	        to->size = sizeof(type);\
	        return False;\
	    }\
	    *(type*)(to->addr) = (value);\
        } else {\
	    static type static_val;\
	    static_val = (value);\
	    to->addr = (XtPointer)&static_val;\
        }\
        to->size = sizeof(type);\
        return True;\
    }while (0 )



static Boolean  cvtStringToTabsOrientation(
#if NeedFunctionPrototypes
Display *,XrmValuePtr ,Cardinal *,XrmValuePtr ,XrmValuePtr ,XtPointer *
#endif
);

static Boolean  cvtTabsOrientationToString(
#if NeedFunctionPrototypes
Display *,XrmValuePtr ,Cardinal *,XrmValuePtr ,XrmValuePtr ,XtPointer *
#endif
);

/*ARGSUSED*/
#if NeedFunctionPrototypes

static void compute_tabsizes(Widget self)
#else

static void compute_tabsizes(self)Widget self;
#endif

{
    int maxwd, basewidth, delta, i, n = ((XmTabsWidget)self)->xmTabs.lefttabs + ((XmTabsWidget)self)->xmTabs.righttabs + 1;
    int sum, len, h, length, breadth, shad = ((XmTabsWidget)self)->xmManager.shadow_thickness;

    if (((XmTabsWidget)self)->xmTabs.offsets) XtFree((XtPointer) ((XmTabsWidget)self)->xmTabs.offsets);
    if (((XmTabsWidget)self)->xmTabs.tabwidths) XtFree((XtPointer) ((XmTabsWidget)self)->xmTabs.tabwidths);
    ((XmTabsWidget)self)->xmTabs.offsets = (XtPointer) XtMalloc(n * sizeof(*((XmTabsWidget)self)->xmTabs.offsets));
    ((XmTabsWidget)self)->xmTabs.tabwidths = (XtPointer) XtMalloc(n * sizeof(*((XmTabsWidget)self)->xmTabs.tabwidths));

    if (((XmTabsWidget)self)->xmTabs.orientation == XfwfUpTabs || ((XmTabsWidget)self)->xmTabs.orientation == XfwfDownTabs) {
	length = ((XmTabsWidget)self)->core.width;
	breadth = ((XmTabsWidget)self)->core.height;
    } else {
	length = ((XmTabsWidget)self)->core.height;
	breadth = ((XmTabsWidget)self)->core.width;
    }

    basewidth = length/n;

    for (i = 0; i < n; i++) {
      ((XmTabsWidget)self)->xmTabs.tabwidths[i] = basewidth;
      ((XmTabsWidget)self)->xmTabs.offsets[i] = i * basewidth;
    }
}

/*ARGSUSED*/
#if NeedFunctionPrototypes

static void comp_hor_tab_shape(Widget self,int  i,XPoint  p[12],int * x0,int * x1,int * midy)
#else

static void comp_hor_tab_shape(self,i,p,x0,x1,midy)Widget self;int  i;XPoint  p[12];int * x0;int * x1;int * midy;
#endif

{

  p[0].x = 0;
  p[1].x = ((XmTabsWidget)self)->xmTabs.offsets[i];
  p[2].x = ((XmTabsWidget)self)->xmTabs.offsets[i];
  p[3].x = ((XmTabsWidget)self)->xmTabs.offsets[i] + ((XmTabsWidget)self)->xmTabs.tabwidths[i];
  p[4].x = ((XmTabsWidget)self)->xmTabs.offsets[i] + ((XmTabsWidget)self)->xmTabs.tabwidths[i];
  p[5].x = 0;
  p[6].x = 0;
  p[7].x = 0;
  p[8].x = 0;
  p[9].x = 0;
  p[10].x = 0;
  p[11].x = 0;
 
  p[0].y = 0;
  p[1].y = 0;
  p[2].y = 24;
  p[3].y = 24;
  p[4].y = 0;
  p[5].y = 0;
  p[6].y = 0;
  p[7].y = 0;
  p[8].y = 0;
  p[9].y = 0;
  p[10].y = 0;
  p[11].y = 0;
  
  *x0 = p[1].x;
  *x1 = p[3].x;
  *midy = (p[1].y + p[2].y)/2;

}

/*ARGSUSED*/
#if NeedFunctionPrototypes

static void comp_ver_tab_shape(Widget self,int  i,XPoint  p[12],int * y0,int * y1,int * midx)
#else

static void comp_ver_tab_shape(self,i,p,y0,y1,midx)Widget self;int  i;XPoint  p[12];int * y0;int * y1;int * midx;
#endif

{
    int shad = ((XmTabsWidget)self)->xmManager.shadow_thickness;
    int k = min(((XmTabsWidget)self)->xmTabs.cornerheight, (((XmTabsWidget)self)->core.width - shad)/2);
    /*
     *       0 o_o 11  11 o_o 0
     *         | |        | |
     *       1 o |        | o 1
     *     3 2/  |        |  \2 3
     *     o-o   |        |   o-o
     *    /      |        |      \
     * 4 o       |        |       o 4
     *   |       |        |       |
     * 5 o       |        |       o 5
     *    \      |        |      /
     *     o-o   |        |   o-o
     *     6 7\  |        |  /7 6  
     *       8 o |        | o 8
     *         | |        | |
     *       9 o_o 10  10 o_o 9
     */
    if (((XmTabsWidget)self)->xmTabs.orientation == XfwfLeftTabs) {
	p[0].x = ((XmTabsWidget)self)->core.width - shad;
	p[1].x = ((XmTabsWidget)self)->core.width - shad;
	p[2].x = ((XmTabsWidget)self)->core.width - shad - k;
	p[3].x = k;
	p[4].x = 0;
	p[5].x = 0;
	p[6].x = k;
	p[7].x = ((XmTabsWidget)self)->core.width - shad - k;
	p[8].x = ((XmTabsWidget)self)->core.width - shad;
	p[9].x = ((XmTabsWidget)self)->core.width - shad;
	p[10].x = ((XmTabsWidget)self)->core.width;
	p[11].x = ((XmTabsWidget)self)->core.width;
    } else {
	p[0].x = shad;
	p[1].x = shad;
	p[2].x = shad + k;
	p[3].x = ((XmTabsWidget)self)->core.width - k;
	p[4].x = ((XmTabsWidget)self)->core.width;
	p[5].x = ((XmTabsWidget)self)->core.width;
	p[6].x = ((XmTabsWidget)self)->core.width - k;
	p[7].x = shad + k;
	p[8].x = shad;
	p[9].x = shad;
	p[10].x = 0;
	p[11].x = 0;
    }
    p[0].y = 0;
    p[1].y = ((XmTabsWidget)self)->xmTabs.offsets[i];
    p[2].y = ((XmTabsWidget)self)->xmTabs.offsets[i] + ((XmTabsWidget)self)->xmTabs.cornerwidth;
    p[3].y = ((XmTabsWidget)self)->xmTabs.offsets[i] + ((XmTabsWidget)self)->xmTabs.cornerwidth;
    p[4].y = ((XmTabsWidget)self)->xmTabs.offsets[i] + 2 * ((XmTabsWidget)self)->xmTabs.cornerwidth;
    p[5].y = ((XmTabsWidget)self)->xmTabs.offsets[i] + ((XmTabsWidget)self)->xmTabs.tabwidths[i] - 2 * ((XmTabsWidget)self)->xmTabs.cornerwidth;
    p[6].y = ((XmTabsWidget)self)->xmTabs.offsets[i] + ((XmTabsWidget)self)->xmTabs.tabwidths[i] - ((XmTabsWidget)self)->xmTabs.cornerwidth;
    p[7].y = ((XmTabsWidget)self)->xmTabs.offsets[i] + ((XmTabsWidget)self)->xmTabs.tabwidths[i] - ((XmTabsWidget)self)->xmTabs.cornerwidth;
    p[8].y = ((XmTabsWidget)self)->xmTabs.offsets[i] + ((XmTabsWidget)self)->xmTabs.tabwidths[i];
    p[9].y = ((XmTabsWidget)self)->core.height;
    p[10].y = ((XmTabsWidget)self)->core.height;
    p[11].y = 0;
    *y0 = p[4].y;
    *y1 = p[5].y;
    *midx = (p[1].x + p[4].x)/2;
}

/*ARGSUSED*/
#if NeedFunctionPrototypes

static void draw_border(Widget self,XPoint  poly[12])
#else

static void draw_border(self,poly)Widget self;XPoint  poly[12];
#endif

{
    Display *dpy = XtDisplay(self);
    Window win = XtWindow(self);

    if (((XmTabsWidget)self)->xmTabs.orientation == XfwfUpTabs) {
	XDrawLines(dpy, win, ((XmTabsWidget)self)->xmTabs.topgc, poly, 6, CoordModeOrigin);
	XDrawLines(dpy, win, ((XmTabsWidget)self)->xmTabs.bottomgc, poly + 5, 4, CoordModeOrigin);
	XDrawLines(dpy, win, ((XmTabsWidget)self)->xmTabs.topgc, poly + 8, 2, CoordModeOrigin);
    } else {
	XDrawLines(dpy, win, ((XmTabsWidget)self)->xmTabs.bottomgc, poly, 2, CoordModeOrigin);
	XDrawLines(dpy, win, ((XmTabsWidget)self)->xmTabs.topgc, poly + 1, 4, CoordModeOrigin);
	XDrawLines(dpy, win, ((XmTabsWidget)self)->xmTabs.bottomgc, poly + 4, 6, CoordModeOrigin);
    }
}

/*ARGSUSED*/
#if NeedFunctionPrototypes

static void draw_hor_tab(Widget self,Region  region,int  i)
#else

static void draw_hor_tab(self,region,i)Widget self;Region  region;int  i;
#endif

{
    XPoint p[12];
    Display *dpy = XtDisplay(self);
    Window win = XtWindow(self);
    Region clip;
    int x0, y0, x1, midy;
    Pixmap pix;

    comp_hor_tab_shape(self, i, p, &x0, &x1, &midy);
   
    clip = XPolygonRegion(p, XtNumber(p), WindingRule);

    /*if (region) XIntersectRegion(clip, region, clip);*/

    if (XEmptyRegion(clip)) return;

    /* XSetRegion(dpy, ((XmTabsWidget)self)->xmTabs.textgc, clip);
       XSetRegion(dpy, ((XmTabsWidget)self)->xmTabs.topgc, clip);
       XSetRegion(dpy, ((XmTabsWidget)self)->xmTabs.bottomgc, clip);*/

    if (i == ((XmTabsWidget)self)->xmTabs.lefttabs) {
      pix = ((XmTabsWidget)self)->xmTabs.pass;
    } else {
      pix = ((XmTabsWidget)self)->xmTabs.act;
    }

    /*XSetRegion(dpy, ((XmTabsWidget)self)->xmTabs.textgc, clip);*/

    
    if ((x1-x0) <=97) {
      XCopyArea(dpy,pix,win,((XmTabsWidget)self)->xmTabs.topgc,
		0,0,97,24,x0,0);
    } else {
      int wid = x1-x0;
      int ix,frags,rem,x;
      
      frags = (wid - 64)/32;
      rem = (wid - 64) - (frags * 32);
      
      x = x0;
      
      XCopyArea(dpy,pix,win,((XmTabsWidget)self)->xmTabs.topgc,
		0,0,32,24,x,0);

      x += 32;
      if (frags) {
	for (ix = 0;ix<frags;ix++) {
	  XCopyArea(dpy,pix,win,((XmTabsWidget)self)->xmTabs.topgc,
		    32,0,32,24,x,0);
	  x += 32;
	}
      }
      if (rem) {
	XCopyArea(dpy,pix,win,((XmTabsWidget)self)->xmTabs.topgc,
		  32,0,rem,24,x,0);
	x += rem;
      }
      XCopyArea(dpy,pix,win,((XmTabsWidget)self)->xmTabs.topgc,
		64,0,33,24,x,0);
      x += 32;

    }

    if (((XmTabsWidget)self)->xmTabs.labels) {
	int w, y, x, len = strlen(((XmTabsWidget)self)->xmTabs.labels[i]);
	y = midy - (((XmTabsWidget)self)->xmTabs.font->ascent + ((XmTabsWidget)self)->xmTabs.font->descent)/2 + ((XmTabsWidget)self)->xmTabs.font->ascent;
	w = XTextWidth(((XmTabsWidget)self)->xmTabs.font, ((XmTabsWidget)self)->xmTabs.labels[i], len);
	x = x0 + ((XmTabsWidget)self)->xmTabs.textmargin;		/* Left aligned text */
	XDrawString(dpy, win, ((XmTabsWidget)self)->xmTabs.textgc, x, y, ((XmTabsWidget)self)->xmTabs.labels[i], len);
    }
    /*draw_border(self, p);*/
    XDestroyRegion(clip);
}

/*ARGSUSED*/
#if NeedFunctionPrototypes

static void draw_ver_tab(Widget self,Region  region,int  i)
#else

static void draw_ver_tab(self,region,i)Widget self;Region  region;int  i;
#endif

{
    Display *dpy = XtDisplay(self);
    Window win = XtWindow(self);
    XPoint p[12];
    Region clip;
    int y0, y1, midx;

    comp_ver_tab_shape(self, i, p, &y0, &y1, &midx);
    clip = XPolygonRegion(p, XtNumber(p), WindingRule);
    if (region) XIntersectRegion(clip, region, clip);
    if (XEmptyRegion(clip)) return;

    XSetRegion(dpy, ((XmTabsWidget)self)->xmTabs.textgc, clip);
    XSetRegion(dpy, ((XmTabsWidget)self)->xmTabs.topgc, clip);
    XSetRegion(dpy, ((XmTabsWidget)self)->xmTabs.bottomgc, clip);
    if (i == ((XmTabsWidget)self)->xmTabs.lefttabs) {
	XSetRegion(dpy, ((XmTabsWidget)self)->xmTabs.backgc, clip);
	XFillPolygon(dpy, win, ((XmTabsWidget)self)->xmTabs.backgc,
		     p, XtNumber(p), Convex, CoordModeOrigin);
    } else {
	XSetRegion(dpy, ((XmTabsWidget)self)->xmTabs.fillgc, clip);
	XFillPolygon(dpy, win, ((XmTabsWidget)self)->xmTabs.fillgc,
		     p, XtNumber(p), Convex, CoordModeOrigin);
    }
    if (((XmTabsWidget)self)->xmTabs.labels) {
	int y, align;
	float angle = ((XmTabsWidget)self)->xmTabs.orientation == XfwfLeftTabs ? 90.0 : -90.0;

	y = y0 + ((XmTabsWidget)self)->xmTabs.textmargin;
	align = ((XmTabsWidget)self)->xmTabs.orientation == XfwfLeftTabs ? MRIGHT : MLEFT;

	XRotDrawAlignedString
	    (dpy, ((XmTabsWidget)self)->xmTabs.font, angle, win, ((XmTabsWidget)self)->xmTabs.textgc, midx, y, ((XmTabsWidget)self)->xmTabs.labels[i], align);
    }
    /* draw_border(self, p); */
    XDestroyRegion(clip);
}

/*ARGSUSED*/
#if NeedFunctionPrototypes

static void create_topgc(Widget self)
#else

static void create_topgc(self)Widget self;
#endif

{
    XtGCMask mask = GCForeground | GCLineWidth;
    XGCValues values;

    if (((XmTabsWidget)self)->xmTabs.topgc != NULL) XFreeGC(XtDisplay(self), ((XmTabsWidget)self)->xmTabs.topgc);
    values.foreground = ((XmTabsWidget)self)->xmManager.top_shadow_color;
    values.line_width = 2 * ((XmTabsWidget)self)->xmManager.shadow_thickness;
    ((XmTabsWidget)self)->xmTabs.topgc = XCreateGC(XtDisplay(self), RootWindowOfScreen(XtScreen(self)),
		       mask, &values);
}

/*ARGSUSED*/
#if NeedFunctionPrototypes

static void create_bottomgc(Widget self)
#else

static void create_bottomgc(self)Widget self;
#endif

{
    XtGCMask mask = GCForeground | GCLineWidth;
    XGCValues values;

    if (((XmTabsWidget)self)->xmTabs.bottomgc != NULL) XFreeGC(XtDisplay(self), ((XmTabsWidget)self)->xmTabs.bottomgc);
    values.foreground = ((XmTabsWidget)self)->xmManager.bottom_shadow_color;
    values.line_width = 2 * ((XmTabsWidget)self)->xmManager.shadow_thickness;
    ((XmTabsWidget)self)->xmTabs.bottomgc = XCreateGC(XtDisplay(self), RootWindowOfScreen(XtScreen(self)),
			  mask, &values);
}

/*ARGSUSED*/
#if NeedFunctionPrototypes
static void create_textgc(Widget self)
#else
static void create_textgc(self)Widget self;
#endif
{
    XtGCMask mask = GCForeground | GCFont;
    XGCValues values;

    if (((XmTabsWidget)self)->xmTabs.textgc != NULL) XFreeGC(XtDisplay(self), ((XmTabsWidget)self)->xmTabs.textgc);
    values.foreground = ((XmTabsWidget)self)->xmManager.foreground;
    values.font = ((XmTabsWidget)self)->xmTabs.font->fid;
    ((XmTabsWidget)self)->xmTabs.textgc = XCreateGC(XtDisplay(self), RootWindowOfScreen(XtScreen(self)),
			mask, &values);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void create_fillgc(Widget self)
#else
static void create_fillgc(self)Widget self;
#endif
{
    XtGCMask mask = GCForeground;
    XGCValues values;

    if (((XmTabsWidget)self)->xmTabs.fillgc != NULL) XFreeGC(XtDisplay(self), ((XmTabsWidget)self)->xmTabs.fillgc);
    values.foreground = ((XmTabsWidget)self)->xmTabs.tabcolor;
    ((XmTabsWidget)self)->xmTabs.fillgc = XCreateGC(XtDisplay(self), RootWindowOfScreen(XtScreen(self)),
			mask, &values);
}

/*ARGSUSED*/
#if NeedFunctionPrototypes
static void create_backgc(Widget self)
#else
static void create_backgc(self)Widget self;
#endif
{
    XtGCMask mask = GCForeground;
    XGCValues values;

    if (((XmTabsWidget)self)->xmTabs.backgc != NULL) XFreeGC(XtDisplay(self), ((XmTabsWidget)self)->xmTabs.backgc);
    values.foreground = ((XmTabsWidget)self)->xmManager.foreground;
    ((XmTabsWidget)self)->xmTabs.backgc = XCreateGC(XtDisplay(self), RootWindowOfScreen(XtScreen(self)),
			mask, &values);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void copy_bg(Widget self,int  offset,XrmValue * value)
#else
static void copy_bg(self,offset,value)Widget self;int  offset;XrmValue * value;
#endif
{
    value->addr = (XtPointer) &((XmTabsWidget)self)->core.background_pixel;
}

/*ARGSUSED*/
#if NeedFunctionPrototypes
static void set_shape(Widget self)
#else
static void set_shape(self)Widget self;
#endif
{
    int x0, x1, midy, y0, y1, midx, i;
    Region region, clip;
    XPoint poly[12];
    return;
    if (! XtIsRealized(self)) return;

    region = XCreateRegion();

    switch (((XmTabsWidget)self)->xmTabs.orientation) {
    case XfwfUpTabs:
    case XfwfDownTabs:
	for (i = 0; i <= ((XmTabsWidget)self)->xmTabs.lefttabs + ((XmTabsWidget)self)->xmTabs.righttabs; i++) {
	    comp_hor_tab_shape(self, i, poly, &x0, &x1, &midy);
	    clip = XPolygonRegion(poly, XtNumber(poly), WindingRule);
	    XUnionRegion(region, clip, region);
	    XDestroyRegion(clip);
	}
	break;
    case XfwfLeftTabs:
    case XfwfRightTabs:
	for (i = 0; i <= ((XmTabsWidget)self)->xmTabs.lefttabs + ((XmTabsWidget)self)->xmTabs.righttabs; i++) {
	    comp_ver_tab_shape(self, i, poly, &y0, &y1, &midx);
	    clip = XPolygonRegion(poly, XtNumber(poly), WindingRule);
	    XUnionRegion(region, clip, region);
	    XDestroyRegion(clip);
	}
	break;
    }
    /*XShapeCombineRegion(XtDisplay(self), XtWindow(self), ShapeBounding,
			0, 0, region, ShapeSet);*/
    XDestroyRegion(region);
}

/*ARGSUSED*/
#if NeedFunctionPrototypes
static Boolean  cvtStringToTabsOrientation(Display * display,XrmValuePtr  args,Cardinal * num_args,XrmValuePtr  from,XrmValuePtr  to,XtPointer * converter_data)
#else
static Boolean  cvtStringToTabsOrientation(display,args,num_args,from,to,converter_data)Display * display;XrmValuePtr  args;Cardinal * num_args;XrmValuePtr  from;XrmValuePtr  to;XtPointer * converter_data;
#endif
{
    TabsOrientation a = XfwfUpTabs;
    char *s = (char*) from->addr;

    if (*num_args != 0)
	XtAppErrorMsg
	    (XtDisplayToApplicationContext(display),
	     "cvtStringToTabsOrientation", "wrongParameters", "XtToolkitError",
	     "String to TabsOrientation conversion needs no arguments",
	     (String*) NULL, (Cardinal*) NULL);

    if (XmuCompareISOLatin1(s, "up") == 0) a = XfwfUpTabs;
    else if (XmuCompareISOLatin1(s, "uptabs") == 0) a |= XfwfUpTabs;
    else if (XmuCompareISOLatin1(s, "down") == 0) a |= XfwfDownTabs;
    else if (XmuCompareISOLatin1(s, "downtabs") == 0) a |= XfwfDownTabs;
    else if (XmuCompareISOLatin1(s, "left") == 0) a |= XfwfLeftTabs;
    else if (XmuCompareISOLatin1(s, "lefttabs") == 0) a |= XfwfLeftTabs;
    else if (XmuCompareISOLatin1(s, "right") == 0) a |= XfwfRightTabs;
    else if (XmuCompareISOLatin1(s, "righttabs") == 0) a |= XfwfRightTabs;
    else XtDisplayStringConversionWarning(display, s, "TabsOrientation");
    done(TabsOrientation, a);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static Boolean  cvtTabsOrientationToString(Display * display,XrmValuePtr  args,Cardinal * num_args,XrmValuePtr  from,XrmValuePtr  to,XtPointer * converter_data)
#else
static Boolean  cvtTabsOrientationToString(display,args,num_args,from,to,converter_data)Display * display;XrmValuePtr  args;Cardinal * num_args;XrmValuePtr  from;XrmValuePtr  to;XtPointer * converter_data;
#endif
{
    TabsOrientation *a = (TabsOrientation*) from->addr;

    if (*num_args != 0)
	XtAppErrorMsg
	    (XtDisplayToApplicationContext(display),
	     "cvtTabsOrientationToString", "wrongParameters", "XtToolkitError",
	     "TabsOrientation to String conversion needs no arguments",
	     (String*) NULL, (Cardinal*) NULL);
    switch (*a) {
    case XfwfUpTabs: done(String, "up");
    case XfwfDownTabs: done(String, "down");
    case XfwfLeftTabs: done(String, "left");
    case XfwfRightTabs: done(String, "right");
    }
    XtAppErrorMsg
	(XtDisplayToApplicationContext(display),
	 "cvtTabsOrientationToString", "illParameters", "XtToolkitError",
	     "TabsOrientation to String conversion got illegal argument",
	     (String*) NULL, (Cardinal*) NULL);
    return TRUE;
}

static XtResource resources[] = {

{XtNorientation,XtCOrientation,XtRTabsOrientation,sizeof(((XmTabsRec*)NULL)->xmTabs.orientation),XtOffsetOf(XmTabsRec,xmTabs.orientation),XtRImmediate,(XtPointer)XfwfUpTabs },

{XtNlefttabs,XtCLefttabs,XtRInt,sizeof(((XmTabsRec*)NULL)->xmTabs.lefttabs),XtOffsetOf(XmTabsRec,xmTabs.lefttabs),XtRImmediate,(XtPointer)0 },

{XtNrighttabs,XtCRighttabs,XtRInt,sizeof(((XmTabsRec*)NULL)->xmTabs.righttabs),XtOffsetOf(XmTabsRec,xmTabs.righttabs),XtRImmediate,(XtPointer)0 },

{XtNlabels,XtCLabels,XtRStringArray,sizeof(((XmTabsRec*)NULL)->xmTabs.labels),XtOffsetOf(XmTabsRec,xmTabs.labels),XtRImmediate,(XtPointer)NULL },

{XtNtabWidthPercentage,XtCTabWidthPercentage,XtRInt,sizeof(((XmTabsRec*)NULL)->xmTabs.tabWidthPercentage),XtOffsetOf(XmTabsRec,xmTabs.tabWidthPercentage),XtRImmediate,(XtPointer)50 },

{XtNcornerwidth,XtCCornerwidth,XtRCardinal,sizeof(((XmTabsRec*)NULL)->xmTabs.cornerwidth),XtOffsetOf(XmTabsRec,xmTabs.cornerwidth),XtRImmediate,(XtPointer)3 },

{XtNcornerheight,XtCCornerheight,XtRCardinal,sizeof(((XmTabsRec*)NULL)->xmTabs.cornerheight),XtOffsetOf(XmTabsRec,xmTabs.cornerheight),XtRImmediate,(XtPointer)3 },

{XtNtextmargin,XtCTextmargin,XtRInt,sizeof(((XmTabsRec*)NULL)->xmTabs.textmargin),XtOffsetOf(XmTabsRec,xmTabs.textmargin),XtRImmediate,(XtPointer)3 },

{XtNtabcolor,XtCTabcolor,XtRPixel,sizeof(((XmTabsRec*)NULL)->xmTabs.tabcolor),XtOffsetOf(XmTabsRec,xmTabs.tabcolor),XtRCallProc,(XtPointer)copy_bg },

{XtNfont,XtCFont,XtRFontStruct,sizeof(((XmTabsRec*)NULL)->xmTabs.font),XtOffsetOf(XmTabsRec,xmTabs.font),XtRString,(XtPointer)XtDefaultFont },

{XtNactivateCallback,XtCActivateCallback,XtRCallback,sizeof(((XmTabsRec*)NULL)->xmTabs.activateCallback),XtOffsetOf(XmTabsRec,xmTabs.activateCallback),XtRImmediate,(XtPointer)NULL },
};

XmTabsClassRec xmTabsClassRec = {
{ /* core_class part */
/* superclass   	*/  (WidgetClass) &xmManagerClassRec,
/* class_name   	*/  "XmTabs",
/* widget_size  	*/  sizeof(XmTabsRec),
/* class_initialize 	*/  class_initialize,
/* class_part_initialize*/  _resolve_inheritance,
/* class_inited 	*/  FALSE,
/* initialize   	*/  initialize,
/* initialize_hook 	*/  NULL,
/* realize      	*/  realize,
/* actions      	*/  actionsList,
/* num_actions  	*/  1,
/* resources    	*/  resources,
/* num_resources 	*/  11,
/* xrm_class    	*/  NULLQUARK,
/* compres_motion 	*/  True ,
/* compress_exposure 	*/  XtExposeCompressMultiple ,
/* compress_enterleave 	*/  True ,
/* visible_interest 	*/  False ,
/* destroy      	*/  destroy,
/* resize       	*/  resize,
/* expose       	*/  expose,
/* set_values   	*/  set_values,
/* set_values_hook 	*/  NULL,
/* set_values_almost 	*/  XtInheritSetValuesAlmost,
/* get_values+hook 	*/  NULL,
/* accept_focus 	*/  XtInheritAcceptFocus,
/* version      	*/  XtVersion,
/* callback_private 	*/  NULL,
/* tm_table      	*/  defaultTranslations,
/* query_geometry 	*/  XtInheritQueryGeometry,
/* display_acceleator 	*/  XtInheritDisplayAccelerator,
/* extension    	*/  NULL 
},
{ /* composite_class part */
XtInheritGeometryManager,
XtInheritChangeManaged,
XtInheritInsertChild,
XtInheritDeleteChild,
NULL
},
{ /* constraint_class part */
/* constraint_resources     */  NULL,
/* num_constraint_resources */  0,
/* constraint_size          */  sizeof(XmTabsConstraintRec),
/* constraint_initialize    */  NULL,
/* constraint_destroy       */  NULL,
/* constraint_set_values    */  NULL,
/* constraint_extension     */  NULL 
},
{ /* XmManager class part */
#define manager_extension extension
/* translations                 */  XtInheritTranslations ,
/* syn_resources                */  NULL ,
/* num_syn_resources            */  0 ,
/* syn_constraint_resources     */  NULL ,
/* num_syn_constraint_resources */  0 ,
/* parent_process               */  XmInheritParentProcess,
/* manager_extension            */  NULL ,
},
{ /* XmTabs_class part */
border_highlight,
border_unhighlight,
},
};
WidgetClass xmTabsWidgetClass = (WidgetClass) &xmTabsClassRec;
/*ARGSUSED*/

static void activate(self,event,params,num_params)Widget self;XEvent*event;String*params;Cardinal*num_params;
{
    int x0, x1, dummy, i, x, y;
    XPoint poly[12];

    switch (((XmTabsWidget)self)->xmTabs.orientation) {
    case XfwfUpTabs:
    case XfwfDownTabs:
	x = event->xbutton.x;
	comp_hor_tab_shape(self, ((XmTabsWidget)self)->xmTabs.lefttabs, poly, &x0, &x1, &dummy);
	if (x0 <= x && x < x1) {
	    XtCallCallbackList(self, ((XmTabsWidget)self)->xmTabs.activateCallback, (XtPointer) 0);
	    return;
	}
	for (i = -1; i >= -((XmTabsWidget)self)->xmTabs.lefttabs; i--) {
	    comp_hor_tab_shape(self, i + ((XmTabsWidget)self)->xmTabs.lefttabs, poly, &x0, &x1, &dummy);
	    if (x0 <= x && x < x1) {
		XtCallCallbackList(self, ((XmTabsWidget)self)->xmTabs.activateCallback, (XtPointer) i);
		return;
	    }
	}
	for (i = 1; i <= ((XmTabsWidget)self)->xmTabs.righttabs; i++) {
	    comp_hor_tab_shape(self, i + ((XmTabsWidget)self)->xmTabs.lefttabs, poly, &x0, &x1, &dummy);
	    if (x0 <= x && x < x1) {
		XtCallCallbackList(self, ((XmTabsWidget)self)->xmTabs.activateCallback, (XtPointer) i);
		return;
	    }
	}
	break;
    case XfwfLeftTabs:
    case XfwfRightTabs:
	y = event->xbutton.y;
	comp_ver_tab_shape(self, ((XmTabsWidget)self)->xmTabs.lefttabs, poly, &x0, &x1, &dummy);
	if (x0 <= y && y < x1) {
	    XtCallCallbackList(self, ((XmTabsWidget)self)->xmTabs.activateCallback, (XtPointer) 0);
	    return;
	}
	for (i = -1; i >= -((XmTabsWidget)self)->xmTabs.lefttabs; i--) {
	    comp_ver_tab_shape(self, i + ((XmTabsWidget)self)->xmTabs.lefttabs, poly, &x0, &x1, &dummy);
	    if (x0 <= y && y < x1) {
		XtCallCallbackList(self, ((XmTabsWidget)self)->xmTabs.activateCallback, (XtPointer) i);
		return;
	    }
	}
	for (i = 1; i <= ((XmTabsWidget)self)->xmTabs.righttabs; i++) {
	    comp_ver_tab_shape(self, i + ((XmTabsWidget)self)->xmTabs.lefttabs, poly, &x0, &x1, &dummy);
	    if (x0 <= y && y < x1) {
		XtCallCallbackList(self, ((XmTabsWidget)self)->xmTabs.activateCallback, (XtPointer) i);
		return;
	    }
	}
	break;
    }
}

static void _resolve_inheritance(class)
WidgetClass class;
{
  XmTabsWidgetClass c = (XmTabsWidgetClass) class;
  XmTabsWidgetClass super;
  static CompositeClassExtensionRec extension_rec = {
    NULL, NULLQUARK, XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec), True};
  CompositeClassExtensionRec *ext;
  ext = (XtPointer)XtMalloc(sizeof(*ext));
  *ext = extension_rec;
  ext->next_extension = c->composite_class.extension;
  c->composite_class.extension = ext;
  if (class == xmTabsWidgetClass) return;
  super = (XmTabsWidgetClass)class->core_class.superclass;
  if (c->xmTabs_class.border_highlight == XtInherit_border_highlight)
    c->xmTabs_class.border_highlight = super->xmTabs_class.border_highlight;
  if (c->xmTabs_class.border_unhighlight == XtInherit_border_unhighlight)
    c->xmTabs_class.border_unhighlight = super->xmTabs_class.border_unhighlight;
}

/*ARGSUSED*/
#if NeedFunctionPrototypes
static void class_initialize(void)
#else
static void class_initialize()
#endif

{
    XtSetTypeConverter(XtRString, "StringArray",
		       cvtStringToStringArray,
		       NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter(XtRString, "TabsOrientation",
		       cvtStringToTabsOrientation,
		       NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter("TabsOrientation", XtRString,
		       cvtTabsOrientationToString,
		       NULL, 0, XtCacheNone, NULL);
}

/*ARGSUSED*/
#if NeedFunctionPrototypes
static void initialize(Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
static void initialize(request,self,args,num_args)Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
{
    String *h;
    int i;

    ((XmTabsWidget)self)->xmManager.traversal_on = FALSE;
    ((XmTabsWidget)self)->xmTabs.topgc = NULL;
    create_topgc(self);
    ((XmTabsWidget)self)->xmTabs.bottomgc = NULL;
    create_bottomgc(self);
    ((XmTabsWidget)self)->xmTabs.textgc = NULL;
    create_textgc(self);
    ((XmTabsWidget)self)->xmTabs.fillgc = NULL;
    create_fillgc(self);
    ((XmTabsWidget)self)->xmTabs.backgc = NULL;
    create_backgc(self);
    if (((XmTabsWidget)self)->xmTabs.labels) {
	h = (String*) XtMalloc((((XmTabsWidget)self)->xmTabs.lefttabs + ((XmTabsWidget)self)->xmTabs.righttabs + 1) * sizeof(*h));
	for (i = ((XmTabsWidget)self)->xmTabs.lefttabs + ((XmTabsWidget)self)->xmTabs.righttabs; i >= 0; i--)
	    h[i] = XtNewString(((XmTabsWidget)self)->xmTabs.labels[i]);
	((XmTabsWidget)self)->xmTabs.labels = h;
    }
    if (((XmTabsWidget)self)->xmTabs.tabWidthPercentage < 0 || ((XmTabsWidget)self)->xmTabs.tabWidthPercentage > 100) {
	XtAppWarning(XtWidgetToApplicationContext(self),
		     "tabWidthPercentage out of range; reset to 50");
	((XmTabsWidget)self)->xmTabs.tabWidthPercentage = 50;
    }
    ((XmTabsWidget)self)->xmTabs.offsets = NULL;
    ((XmTabsWidget)self)->xmTabs.tabwidths = NULL;
    {
#include "Button1.xpm"
      ((XmTabsWidget)self)->xmTabs.act = XcodaCreatePixmapFromXpm(self,magick,1);
    }
    {
#include "Button2.xpm"
      ((XmTabsWidget)self)->xmTabs.pass = XcodaCreatePixmapFromXpm(self,magick,1);
    }
    
    compute_tabsizes(self);
}

/*ARGSUSED*/
#if NeedFunctionPrototypes
static Boolean  set_values(Widget  old,Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
static Boolean  set_values(old,request,self,args,num_args)Widget  old;Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
{
    Bool redraw = FALSE, resize_labels = FALSE;
    String *h;
    int i;

    if (((XmTabsWidget)self)->core.background_pixel != ((XmTabsWidget)old)->core.background_pixel
	|| ((XmTabsWidget)self)->core.background_pixmap != ((XmTabsWidget)old)->core.background_pixmap
	|| ((XmTabsWidget)self)->xmManager.shadow_thickness != ((XmTabsWidget)old)->xmManager.shadow_thickness) {
	create_topgc(self);
	create_bottomgc(self);
	create_backgc(self);
    }
    if (((XmTabsWidget)self)->xmManager.foreground != ((XmTabsWidget)old)->xmManager.foreground || ((XmTabsWidget)self)->xmTabs.font != ((XmTabsWidget)old)->xmTabs.font) {
	create_textgc(self);
	redraw = TRUE;
    }
    if (((XmTabsWidget)self)->xmTabs.tabcolor != ((XmTabsWidget)old)->xmTabs.tabcolor) {
	create_fillgc(self);
	redraw = TRUE;
    }
    if ((((XmTabsWidget)self)->xmTabs.textmargin != ((XmTabsWidget)old)->xmTabs.textmargin && ((XmTabsWidget)self)->xmTabs.tabWidthPercentage == 0)
	|| ((XmTabsWidget)self)->xmTabs.cornerwidth != ((XmTabsWidget)old)->xmTabs.cornerwidth
	|| ((XmTabsWidget)self)->xmTabs.cornerheight != ((XmTabsWidget)old)->xmTabs.cornerheight) {
	resize_labels = TRUE;
    }
    if (((XmTabsWidget)self)->xmTabs.labels != ((XmTabsWidget)old)->xmTabs.labels) {
	if (((XmTabsWidget)self)->xmTabs.labels) {
	    h = (String*) XtMalloc((((XmTabsWidget)self)->xmTabs.lefttabs + ((XmTabsWidget)self)->xmTabs.righttabs + 1) * sizeof(*h));
	    for (i = ((XmTabsWidget)self)->xmTabs.lefttabs + ((XmTabsWidget)self)->xmTabs.righttabs; i >= 0; i--)
		h[i] = XtNewString(((XmTabsWidget)self)->xmTabs.labels[i]);
	    ((XmTabsWidget)self)->xmTabs.labels = h;
	}
	if (((XmTabsWidget)old)->xmTabs.labels) {
	    for (i = ((XmTabsWidget)old)->xmTabs.lefttabs + ((XmTabsWidget)old)->xmTabs.righttabs; i >= 0; i--)
		XtFree(((XmTabsWidget)old)->xmTabs.labels[i]);
	    XtFree((XtPointer) ((XmTabsWidget)old)->xmTabs.labels);
	}
	resize_labels = TRUE;
    }
    if (((XmTabsWidget)self)->xmTabs.tabWidthPercentage < 0 || ((XmTabsWidget)self)->xmTabs.tabWidthPercentage > 100) {
	XtAppWarning(XtWidgetToApplicationContext(self),
		     "tabWidthPercentage out of range; reset to 50");
	((XmTabsWidget)self)->xmTabs.tabWidthPercentage = 50;
    }
    if (((XmTabsWidget)old)->xmTabs.tabWidthPercentage != ((XmTabsWidget)self)->xmTabs.tabWidthPercentage)
	resize_labels = TRUE;
    if (((XmTabsWidget)self)->xmTabs.lefttabs != ((XmTabsWidget)old)->xmTabs.lefttabs || ((XmTabsWidget)self)->xmTabs.righttabs != ((XmTabsWidget)old)->xmTabs.righttabs)
	redraw = TRUE;
    if (resize_labels) {
	compute_tabsizes(self);
	redraw = TRUE;
    }
    return redraw;
}

/*ARGSUSED*/
#if NeedFunctionPrototypes
static void realize(Widget self,XtValueMask * mask,XSetWindowAttributes * attributes)
#else
static void realize(self,mask,attributes)Widget self;XtValueMask * mask;XSetWindowAttributes * attributes;
#endif
{
    *mask |= CWBitGravity;
    attributes->bit_gravity = ForgetGravity;
    xmManagerClassRec.core_class.realize(self, mask, attributes);
    set_shape(self);
}

/*ARGSUSED*/
#if NeedFunctionPrototypes
static void resize(Widget self)
#else
static void resize(self)Widget self;
#endif
{
    if (XtIsRealized(self))
	XClearArea(XtDisplay(self), XtWindow(self), 0, 0, 0, 0, True);
    compute_tabsizes(self);
    set_shape(self);
    xmManagerClassRec.core_class.resize(self);
}

/*ARGSUSED*/
#if NeedFunctionPrototypes
static void expose(Widget self,XEvent * event,Region  region)
#else
static void expose(self,event,region)Widget self;XEvent * event;Region  region;
#endif
{
    int i;

    if (! XtIsRealized(self)) return;

    compute_tabsizes(self);
    set_shape(self);
    switch (((XmTabsWidget)self)->xmTabs.orientation) {
    case XfwfUpTabs:
    case XfwfDownTabs:
	for (i = 0; i < ((XmTabsWidget)self)->xmTabs.lefttabs; i++)
	    draw_hor_tab(self, region, i);
	for (i = ((XmTabsWidget)self)->xmTabs.lefttabs + ((XmTabsWidget)self)->xmTabs.righttabs; i > ((XmTabsWidget)self)->xmTabs.lefttabs; i--)
	    draw_hor_tab(self, region, i);
	draw_hor_tab(self, region, ((XmTabsWidget)self)->xmTabs.lefttabs);
	break;
    case XfwfLeftTabs:
    case XfwfRightTabs:
	for (i = 0; i < ((XmTabsWidget)self)->xmTabs.lefttabs; i++)
	    draw_ver_tab(self, region, i);
	for (i = ((XmTabsWidget)self)->xmTabs.lefttabs + ((XmTabsWidget)self)->xmTabs.righttabs; i > ((XmTabsWidget)self)->xmTabs.lefttabs; i--)
	    draw_ver_tab(self, region, i);
	draw_ver_tab(self, region, ((XmTabsWidget)self)->xmTabs.lefttabs);
	break;
    }
    /* Focus highlight? */
}

/*ARGSUSED*/

#if NeedFunctionPrototypes
static void border_highlight(void)
#else
static void border_highlight()
#endif
{
}

/*ARGSUSED*/

#if NeedFunctionPrototypes
static void border_unhighlight(void)
#else
static void border_unhighlight()
#endif
{
}

/*ARGSUSED*/

#if NeedFunctionPrototypes
static void destroy(Widget self)
#else
static void destroy(self)Widget self;
#endif
{
    int i;

    if (((XmTabsWidget)self)->xmTabs.labels) {
	for (i = ((XmTabsWidget)self)->xmTabs.lefttabs + ((XmTabsWidget)self)->xmTabs.righttabs; i >= 0; i--)
	    XtFree(((XmTabsWidget)self)->xmTabs.labels[i]);
	XtFree((XtPointer) ((XmTabsWidget)self)->xmTabs.labels);
    }
    if (((XmTabsWidget)self)->xmTabs.offsets)
	XtFree((XtPointer) ((XmTabsWidget)self)->xmTabs.offsets);
    if (((XmTabsWidget)self)->xmTabs.tabwidths)
	XtFree((XtPointer) ((XmTabsWidget)self)->xmTabs.tabwidths);
}
