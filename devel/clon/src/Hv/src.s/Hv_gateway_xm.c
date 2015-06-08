/**
 * <EM>Deals with all commands specific for the
 * X Windows system and OSF Motif.</EM>
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
#ifdef HVNT
#define WIN32 1
#endif

#ifndef WIN32
#ifdef sparc
extern int gethostname( char *name, int namelen);
#endif

#include "Hv.h"
#include "Hv_gateway_xm.h"
#include "Hv_init.h"
#include "Hv_pointer.h"
#include "Hv_pic.h"
#include "Hv_xyplot.h"
#include "Hv_maps.h"
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <Xm/FileSB.h>
#include <Xm/Protocols.h>
#include <unistd.h>

#ifdef Hv_USEOPENGL /* open gl 1 */
#define EVARS
#include "Hv_opengl.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>
#ifdef __linux__
WidgetClass glwMDrawingAreaWidgetClass;
#endif  /* end __linux__ */
#ifdef __sun__
WidgetClass glwMDrawingAreaWidgetClass;
#endif  /* end __sun__ */

#endif  /* end Hv_USEOPENGL 1*/

/* 
 * max number of user added menus 
 * (needed because of weird Motif
 * geometry management) 
 */

#define Hv_MAXUSERMENUS     6


/*static XtActionsRec menuactions[] = {
  {"Hv_PopdownMenuShell", (XtActionProc)Hv_PopdownMenuShell},
};
*/

/*

static XtActionsRec canvasactions[] = {
  {"Hv_PopupMenuShell", (XtActionProc)Hv_PopupMenuShell},
};


static char  canvastranstable[] = 
   "<Btn3Down> : Hv_PopupMenuShell()";
   */

extern Hv_Widget    Hv_quitSepItem;

extern Boolean       dontPromptDir;

extern Hv_Window  Hv_trueMainWindow;  /* unchanging (even while off screen draws) */

extern short  Hv_mwLeft;       /* from attributes to Hv_VaInitialize */
extern short  Hv_mwTop;        /* from attributes to Hv_VaInitialize */
extern short  Hv_mwWidth;      /* from attributes to Hv_VaInitialize */
extern short  Hv_mwHeight;     /* from attributes to Hv_VaInitialize */
extern short *Hv_mwFillColor;  /* from attributes to Hv_VaInitialize */

/* these color externs are declared in Hv_dlogs */

extern Hv_Widget Hv_newColorLabel;
extern short     Hv_newColor;
extern int       Hv_sbredrawcontrol;

extern Boolean   Hv_altCO;

/* local variables */

static   Hv_Widget   newpatternlabel, oldpatternlabel;
static   short    newpattern;

static char *Hv_xFontNames[Hv_NUMFONTS] = {
  "-*-fixed-medium-r-normal--13-*-*-*-*-*-iso8859-*",
  "-*-fixed-medium-r-normal--14-*-*-*-*-*-iso8859-*",
  "-*-fixed-medium-r-normal--15-*-*-*-*-*-iso8859-*",
  "-*-fixed-medium-r-normal--16-*-*-*-*-*-iso8859-*",
  "-*-fixed-medium-r-normal--20-*-*-*-*-*-iso8859-*",
  "-*-fixed-medium-r-normal--24-*-*-*-*-*-iso8859-*",
  "-*-courier-medium-r-normal--11-*-*-*-*-*-*-*",
  "-*-courier-medium-r-normal--12-*-*-*-*-*-*-*",
  "-*-courier-medium-r-normal--14-*-*-*-*-*-*-*",
  "-*-courier-medium-r-normal--17-*-*-*-*-*-*-*",
  "-*-courier-medium-r-normal--20-*-*-*-*-*-*-*",
  "-*-courier-medium-r-normal--25-*-*-*-*-*-*-*",
  "-*-helvetica-medium-r-normal--11-*-*-*-*-*-*-*",
  "-*-helvetica-medium-r-normal--12-*-*-*-*-*-*-*",
  "-*-helvetica-medium-r-normal--14-*-*-*-*-*-*-*",
  "-*-helvetica-medium-r-normal--17-*-*-*-*-*-*-*",
  "-*-helvetica-medium-r-normal--20-*-*-*-*-*-*-*",
  "-*-helvetica-medium-r-normal--25-*-*-*-*-*-*-*",
  "-*-times-medium-r-normal--11-*-*-*-*-*-*-*",
  "-*-times-medium-r-normal--12-*-*-*-*-*-*-*",
  "-*-times-medium-r-normal--14-*-*-*-*-*-*-*",
  "-*-times-medium-r-normal--17-*-*-*-*-*-*-*",
  "-*-times-medium-r-normal--20-*-*-*-*-*-*-*",
  "-*-times-medium-r-normal--25-*-*-*-*-*-*-*",
  "-*-symbol-*-r-normal--11-*-*-*-p-*-*-*",
  "-*-symbol-*-r-normal--12-*-*-*-p-*-*-*",
  "-*-symbol-*-r-normal--14-*-*-*-p-*-*-*",
  "-*-symbol-*-r-normal--17-*-*-*-p-*-*-*",
  "-*-symbol-*-r-normal--20-*-*-*-p-*-*-*",
  "-*-symbol-*-r-normal--25-*-*-*-p-*-*-*",
  "-*-fixed-bold-r-normal--13-*-*-*-*-*-iso8859-*",
  "-*-fixed-bold-r-normal--14-*-*-*-*-*-iso8859-*",
  "-*-fixed-bold-r-normal--15-*-*-*-*-*-iso8859-*",
  "-*-fixed-bold-r-normal--16-*-*-*-*-*-iso8859-*",
  "-*-fixed-bold-r-normal--20-*-*-*-*-*-iso8859-*",
  "-*-fixed-bold-r-normal--24-*-*-*-*-*-iso8859-*",
  "-*-courier-bold-r-normal--11-*-*-*-*-*-*-*",
  "-*-courier-bold-r-normal--12-*-*-*-*-*-*-*",
  "-*-courier-bold-r-normal--14-*-*-*-*-*-*-*",
  "-*-courier-bold-r-normal--17-*-*-*-*-*-*-*",
  "-*-courier-bold-r-normal--20-*-*-*-*-*-*-*",
  "-*-courier-bold-r-normal--25-*-*-*-*-*-*-*",
  "-*-helvetica-bold-r-normal--11-*-*-*-*-*-*-*",
  "-*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*",
  "-*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
  "-*-helvetica-bold-r-normal--17-*-*-*-*-*-*-*",
  "-*-helvetica-bold-r-normal--20-*-*-*-*-*-*-*",
  "-*-helvetica-bold-r-normal--25-*-*-*-*-*-*-*",
  "-*-times-bold-r-normal--11-*-*-*-*-*-*-*",
  "-*-times-bold-r-normal--12-*-*-*-*-*-*-*",
  "-*-times-bold-r-normal--14-*-*-*-*-*-*-*",
  "-*-times-bold-r-normal--17-*-*-*-*-*-*-*",
  "-*-times-bold-r-normal--20-*-*-*-*-*-*-*",
  "-*-times-bold-r-normal--25-*-*-*-*-*-*-*"
};

Dimension       tlx, tly, tlw, tlh;
Arg		tlargs[5];
Hv_Font        *fontfixed = NULL;
Hv_Font        *fontfixed2 = NULL;
static int      Hv_visClass;

/* fall back resources if no resource file is found */

  static char *fallbackresources[] = {
    "*allowShellResize: false",
    "*borderWidth: 0",
    "*highlightThickness: 2",
    "*traversalOn: true",
    "*background: lightgray",
    "*foreground: black",
    "*canvas.background: cornflowerblue",
    "*popup.background: DodgerBlue",
    "*XmToggleButtonGadget*selectColor: wheat",

#ifndef _AIX
#ifndef sparc
#if XtSpecificationRelease > 4
    "*pulldown*tearOffModel: TEAR_OFF_ENABLED",
#endif
#endif
#endif  /* end __AIX__ */

    "*XmMessageBox.labelFontList: -*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*",
    "*XmMessageBox.buttonFontList: -*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*",
    "*XmPushButton.fontList: -*-helvetica-bold-r-normal--11-*-*-*-*-*-*-*",
    "*XmPushButtonGadget.fontList: -*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*",
    "*XmToggleButtonGadget.fontList: -*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*",
    "*XmCascadeButton.fontList: -*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*",
    "*XmCascadeButtonGadget.fontList: -*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*",
    "*XmFileSelectionBox.XmText.fontList: -*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*",
    "*XmFileSelectionBox.buttonFontList: -*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*",
    "*XmFileSelectionBox.labelFontList: -*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*",
    "*XmFileSelectionBox.textFontList: -*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*",
    "*XmLabel.fontList: -*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*",
    "*XmLabel.textFontList: -*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*",
    "*XmLabelGadget.fontList: -*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*",
    "*XmLabelGadget.textFontList: -*-helvetica-bold-r-normal--12-*-*-*-*-*-*-*",
    "*optionmenu*XmPushButtonGadget.fontList: -*-fixed-bold-r-*-*-12-*-*-*-*-70-*-*",
    "*popup*XmPushButtonGadget.fontList: -*-fixed-bold-r-*-*-12-*-*-*-*-70-*-*",
    "*popup*XmToggleButtonGadget.fontList: -*-fixed-bold-r-*-*-12-*-*-*-*-70-*-*",
    "*popup*XmCascadeGadget.fontList: -*-fixed-bold-r-*-*-12-*-*-*-*-70-*-*",
    "*popup*XmCascadeButtonGadget.fontList: -*-fixed-bold-r-*-*-12-*-*-*-*-70-*-*",
    "*textfield.fontList: -*-fixed-bold-r-*-*-12-*-*-*-*-70-*-*",
    (char *)NULL
  };

#define TIGHTNESS 5

static Boolean   Hv_pauseFlag;

static char     *generic_filename;

static Hv_Widget themtw;

static Hv_Widget     helpText;
static FILE         *helpfp = NULL;


static Hv_Widget          Hv_userMenus[Hv_MAXUSERMENUS];
static Hv_Widget          Hv_userPDBtns[Hv_MAXUSERMENUS];
static Hv_CompoundString  Hv_userMenuTitles[Hv_MAXUSERMENUS];
static int                Hv_userMenuCount = 0;

/* local prototypes */

static void Hv_XMDrawBackgroundItems(Hv_ItemList  items,
				   Hv_Region    region);

static void   Hv_ManageMenuItem(Hv_Widget menu,
			  Hv_Widget w);


static void       Hv_SetWidgetPattern (Hv_Widget    w,
				       int          pnumber,
				       XEvent      *event);


static Hv_Widget   Hv_CreateDialog(char *,
				   unsigned char,
				   int);


static void     Hv_NullCallback(Hv_Widget);

static void Hv_WMClose(Widget               w,
		       Hv_Pointer            client_data,
		       XmAnyCallbackStruct *cbs);


static Hv_Widget Hv_CreateActionButton(char       *label,
				       int            i,
				       int            na,
				       Hv_Widget      aa,
				       Hv_Pointer     data,
				       Hv_FunctionPtr cb);

static void     Hv_SetColor(Hv_Widget,
			    int,
			    XEvent *);

static void     Hv_CheckDefault(int *,
				int);

static void	Hv_AddEventHandlers(void);


static void     Hv_XMHandleExpose(Hv_Widget,
				  Hv_Pointer,
				  Hv_XEvent *);

static void Hv_XMHandleKeyRelease(Hv_Widget    w,
				  Hv_Pointer client_data,
				  XKeyEvent *event);

static void Hv_PauseTimeOut(Hv_Pointer    xtptr,
			    Hv_IntervalId  *id);

static void     Hv_CanvasResizeCB();

static void    Hv_ColorizeCursors(Display *);

static void    Hv_FillGaps(short,
			   short,
			   short,
			   short);

static void    Hv_StuffColor(char *,
			     unsigned int,
			     unsigned int,
			     unsigned int,
			     int,
                             Boolean);

static void    Hv_CheckColorMap(void);

static Boolean Hv_MatchVisual(Display *,
			      int scr,
			      int,
			      Visual *,
			      int);

static void     Hv_FileSelectResponse(Widget,
				      Hv_Pointer,
				      XmFileSelectionBoxCallbackStruct *);

static char    *Hv_TextFromXmString(XmString);


static void     Hv_FontReport(Hv_Font **,
			      char *);

static Hv_Font  *Hv_AddFont(short);

static void Hv_TECleanup(void);

static void Hv_TEDoReturn (Widget   tw,
		       Hv_Pointer  unused,
		       XmTextVerifyCallbackStruct *cbs);

static void Hv_TEMouseDestroy (void);


#ifdef Hv_USEOPENGL  /* open gl 2 */

static void Hv_OGLInitCB(Widget w,
			 Hv_Pointer client_data,
			 Hv_Pointer call_data);

static void Hv_OGLInputCB(Widget w,
			  Hv_Pointer client_data, 
			  GLwDrawingAreaCallbackStruct *call_data);

#endif  /* open gl 2 */

extern int strncasecmp(const char *s1, const char *s2, size_t n);

static void     Hv_ChooseHelpTopic(Widget,
				   Hv_Pointer,
				   XmListCallbackStruct	*);

static void     Hv_ReadHelp(char *,
			    char **);

static void     Hv_ReadHelpInfo(FILE *,
				char *);


static  XmFontList       fontlist;
static  XFontStruct      *xfont;

#ifdef Hv_USEOPENGL  /* open gl 3 */

XVisualInfo     *Hv_3Dvi;


int Hv_dblBuf[] = {
     GLX_DOUBLEBUFFER, GLX_RGBA, GLX_DEPTH_SIZE, 16,
     GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1,
     None
};
int *Hv_snglBuf = &Hv_dblBuf[1];
#endif  /* open gl 3 */


/**
 * Hv_XMDrawWorldImageItem
 * @purpose X Windows specific version of Hv_DrawWorldImageItem.
 * @see The gateway routine Hv_DrawWorldImageItem.
 */


void Hv_XMDrawWorldImageItem(Hv_Item    Item,
			   Hv_Region   region) {


  Hv_View            View = Item->view;
  Hv_WorldImageData  *wimage;
 
  Hv_Rect            area, prect;
  Hv_FRect           warea;
  Hv_PicInfo         *pinfo;
  Hv_Rect            *hr;
  int                status;

/* cropping variables */

  int                cx, cy, cw, ch;
  int                cL, cT, cR, cB;

  Boolean            mapview;


  if (Item == NULL)
    return;


/* is this a map view? If so tie location to lat long*/


  mapview = (Hv_IsMapView(Item->view));

  wimage = (Hv_WorldImageData *)(Item->userptr2);
  if (wimage == NULL)
    return;

  if (mapview) {
    Hv_LatLongRectToLocalRect(View, 
 			      wimage->latmin,
 			      wimage->latmax,
 			      wimage->longmin,
 			      wimage->longmax,
			      Item->area);

    Hv_LocalRectToWorldRect(View, Item->area, Item->worldarea);
  }
  else {
    Hv_WorldRectToLocalRect(View, Item->area, Item->worldarea);
  }

  if (Hv_RectInRegion(Item->area, region) == Hv_RECTANGLEOUT)
    return;


  Hv_CopyRect(&area, Item->area);

  if (Hv_extraFastPlease) {
    Hv_FrameRect(&area, Item->color);
    return;
  }

  if (!mapview)
    Hv_LocalRectToWorldRect(View, &area, &warea);

  Hv_SetPattern(Hv_50PERCENTPAT, Hv_black);
  Hv_FillRect(&area, Hv_gray3);
  Hv_SetPattern(Hv_FILLSOLIDPAT, Hv_black); 

  if ((area.width < 5) || (area.height < 5)) {
    Hv_FrameRect(&area, Item->color);
    return;
  }


  pinfo = wimage->pinfo;

  if ((pinfo != NULL) && (pinfo->image != NULL)) {

/* Note: Hv_ScaleAndCropPic won't do anything if it doesn't have to */

    
    hr = View->hotrect;

    cx = (int)(Hv_iMax(hr->left - area.left, 0));
    cL = (int)(Hv_iMax(hr->left, area.left));
    cR = (int)(Hv_iMin(hr->right, area.right));
    cw = cR - cL;

    cy = (int)(Hv_iMax(hr->top - area.top, 0));
    cT = (int)(Hv_iMax(hr->top, area.top));
    cB = (int)(Hv_iMin(hr->bottom, area.bottom));
    ch = cB - cT;

/*
 *  status:
 *  -1 on error
 *   0 had to scale or crop
 *   1 did not have to do anything
 */

    status = Hv_ScaleAndCropPic(pinfo,
				cx,
				cy,
				cw,
				ch,
				area.width,
				area.height);


    if ((status < 0) || (pinfo->image == NULL))
	return;

/* might be able to save time using background pixmap */

    if ((status == 1) && (wimage->background)) {

	Hv_CopyArea(wimage->background,
		    Hv_mainWindow,
		    0,
		    0,
		    cw,
		    ch,
		    cL,
		    cT);

    }
    else {
	wimage->background = Hv_CreatePixmap(cw+1, ch+1);
	if (wimage->background) {

	    Hv_SetRect(&prect, 0, 0, (short)(cw+1), (short)(ch+1));
	    Hv_ClipRect(&prect);

	    Hv_PutImage(wimage->background, pinfo->image,
			0, 0,
			0, 0,
			cw, ch);

	    Hv_SetClippingRegion(region);

	    Hv_CopyArea(wimage->background,
			Hv_mainWindow,
			0,
			0,
			cw,
			ch,
			cL,
			cT);


	}
	else
	    Hv_PutImage(Hv_mainWindow, pinfo->image,
			0, 0,
			cL, cT,
			cw, ch);
    }

    Hv_Flush();
  }


  if ((wimage != NULL) && (wimage->frame)) {
    Hv_SetLineStyle(wimage->thickness, Hv_SOLIDLINE);
    Hv_FrameRect(&area, Item->color);
    Hv_SetLineStyle(0, Hv_SOLIDLINE);
  }

}

/**
 * Hv_XMDrawItemOnPixmap
 * @purpose X Windows specific version of Hv_DrawItemOnPixmap.
 * @see The gateway routine Hv_DrawItemOnPixmap.
 */

Hv_Pixmap  Hv_XMDrawItemOnPixmap(Hv_Item   Item,
			       Hv_Region region) {
    Hv_Pixmap        pmap;
    Hv_Rect         *area;
    Hv_Window        tempwind;
    Hv_Rect          prect;
    short            dh, dv;

    if (Item == NULL)
	return 0;
    
    area = Item->area;
    pmap = Hv_CreatePixmap(area->width+1, area->height+1);

/* store the mainwindow */

    tempwind = Hv_mainWindow;
    Hv_mainWindow = pmap;

/* set the clip to the entire pixmap and draw onto it */

    Hv_SetRect(&prect, 0, 0, (short)(area->width+1), (short)(area->height+1));
    Hv_ClipRect(&prect);
  
    dh = area->left;
    dv = area->top;
    Hv_OffsetItem(Item, (short)(-dh), dv, True);
    Hv_drawingOffScreen = True;
    Hv_DrawItem(Item, NULL);
    Hv_mainWindow = tempwind;
    Hv_drawingOffScreen = False;
    Hv_OffsetItem(Item, dh, dv, True);
    return pmap;
}



/**
 * Hv_XMSaveViewBackground
 * @purpose X Windows specific version of Hv_SaveViewBackground.
 * @see The gateway routine Hv_SaveViewBackground.
 */


void Hv_XMSaveViewBackground(Hv_View   View,
			   Hv_Region  region) {
  Hv_Window        tempwind;
  Hv_Rect          *hr;
  Hv_Rect          prect;
  short            dh, dv;

/* first free the old pixmap and get a new one of the proper size */

  Hv_FreePixmap(View->background);

  hr = View->hotrectborderitem->area;
  View->background = Hv_CreatePixmap(hr->width+1, hr->height+1);

  if (View->background == 0)
    return;

/* store the mainwindow */

  tempwind = Hv_mainWindow;
  Hv_mainWindow = View->background;

/* set the clip to the entire pixmap and draw onto it */

  Hv_SetRect(&prect, 0, 0, (short)(hr->width+1), (short)(hr->height+1));
  Hv_ClipRect(&prect);
  
  dh = hr->left;
  dv = hr->top;
  Hv_OffsetView(View, (short)(-dh), (short)(-dv));
  Hv_XMDrawBackgroundItems(View->items, NULL);
  Hv_OffsetView(View, dh, dv);

/* restore the main window and clip region */

  Hv_SetViewClean(View);
  Hv_mainWindow = tempwind;

  Hv_RestoreClipRegion(region);
}



/**
 * Hv_XMSaveUnder
 * @purpose X Windows specific version of Hv_SaveUnder.
 * @see The gateway routine Hv_SaveUnder.
 */


Hv_Pixmap   Hv_XMSaveUnder(short x,
			 short y,
			 unsigned int  w,
			 unsigned int  h) {
    Hv_Pixmap    pmap;

    pmap = Hv_CreatePixmap(w+2, h+2);
    
    Hv_CopyArea(Hv_trueMainWindow,
	      pmap,
	      x-1,
	      y-1,
	      w+2,
	      h+2,
	      0,
	      0);
    
    return pmap;
}


/**
 * Hv_XMRestoreSaveUnder
 * @purpose X Windows specific version of Hv_RestoreSaveUnder.
 * @see The gateway routine Hv_RestoreSaveUnder. */


void  Hv_XMRestoreSaveUnder(Hv_Pixmap pmap,
			  short x,
			  short y,
			  unsigned int  w,
			  unsigned int  h) {
    Hv_CopyArea(pmap,
		Hv_trueMainWindow,
		0,
		0,
		w+2,
		h+2,
		x-1,
		y-1);

}


/**
 * Hv_XMUpdateItemOffScreen
 * @purpose X Windows specific version of Hv_UpdateItemOffScreen.
 * @see The gateway routine Hv_UpdateItemOffScreen.
 */


void  Hv_XMUpdateItemOffScreen(Hv_Item   Item,
			     Hv_Region region) {

  Hv_Pixmap        pmap;
  Hv_Rect         *area;
  Hv_Window        tempwind;
  Hv_Rect          prect;
  short            dh, dv;
  Hv_Region        rgn = NULL;

  if (Item == NULL)
    return;

  if (!Hv_ViewIsVisible(Item->view))
    return;

  area = Item->area;
  pmap = Hv_CreatePixmap(area->width+1, area->height+1);

/* store the mainwindow */

  tempwind = Hv_mainWindow;
  Hv_mainWindow = pmap;

/* set the clip to the entire pixmap and draw onto it */

  Hv_SetRect(&prect, 0, 0, (short)(area->width+1), (short)(area->height+1));
  Hv_ClipRect(&prect);

  dh = area->left;
  dv = area->top;
  Hv_OffsetItem(Item, (short)(-dh), (short)(-dv), True);
  Hv_drawingOffScreen = True;
  Hv_DrawItem(Item, NULL);
  
  Hv_mainWindow = tempwind;
  Hv_drawingOffScreen = False;
  Hv_OffsetItem(Item, dh, dv, True);

/* now copy back on screen */

  rgn = Hv_GetMinimumItemRegion(Item);
  Hv_SetClippingRegion(rgn);

  Hv_CopyArea(pmap, Hv_trueMainWindow, 0, 0,
	      area->width, area->height,
	      area->left, area->top);
  
  Hv_Flush();
  Hv_DestroyRegion(rgn);
  
  if (region != NULL)
    Hv_RestoreClipRegion(region);
  
  Hv_FreePixmap(pmap);
}



/**
 * Hv_XMOffScreenViewControlUpdate
 * @purpose X Windows specific version of Hv_OffScreenViewControlUpdate.
 * @see The gateway routine Hv_OffScreenViewControlUpdate.
 */


void Hv_XMOffScreenViewControlUpdate(Hv_View    View,
				   Hv_Region  region) {
  Hv_Window        tempwind;
  Hv_Rect          *local;
  Hv_Rect          prect;
  short            dh, dv;
  Hv_Region        drawrgn = NULL;
  Hv_Region        rgn = NULL;
  Hv_Pixmap        pmap;
  Hv_Item          temp;
  
/* check for pathologies */

  if ((Hv_inPostscriptMode) || (Hv_mainWindow == 0) || !Hv_ViewIsVisible(View))
    return;

  Hv_SetViewDirty(View);

/* create pixmap, store the mainwindow */

  local = View->local;
  pmap = Hv_CreatePixmap(local->width, local->height);
  tempwind = Hv_mainWindow;
  Hv_mainWindow = pmap;
  Hv_CopyRegion(&drawrgn, region);

/* now make preparations for offscreen drawing */

  Hv_SetRect(&prect, 0, 0, local->width, local->height);
  Hv_ClipRect(&prect);
  
  dh = local->left;
  dv = local->top;

  Hv_OffsetView(View, (short)(-dh), (short)(-dv));
  Hv_OffsetRegion(drawrgn, (short)(-dh), (short)(-dv));

/* now ready to draw */

  Hv_drawingOffScreen = True;


  for (temp = View->items->first;  temp != NULL;  temp = temp->next) 
    if (temp->domain != Hv_INSIDEHOTRECT) {
      Hv_MaybeDrawItem(temp, drawrgn);
	  
/* sublist? */
      
    if (temp->children != NULL)
      Hv_DrawItemList(temp->children, drawrgn);
  }

  Hv_drawingOffScreen = False;
  
  Hv_OffsetView(View, dh, dv);

  Hv_mainWindow = tempwind;

  Hv_DestroyRegion(drawrgn);
  rgn = Hv_RectRegion(local);
  drawrgn = Hv_IntersectRegion(rgn, region);

  Hv_SetClippingRegion(drawrgn);

/* now copy the pixmap on screen */

  Hv_CopyArea(pmap, Hv_trueMainWindow, 0, 0,
	      local->width, local->height,
	      local->left, local->top);
  
  Hv_Flush();
  
  Hv_DestroyRegion(rgn);
  Hv_DestroyRegion(drawrgn);
  Hv_RestoreClipRegion(region);
  Hv_FreePixmap(pmap);

}






/**
 * Hv_XMRestoreViewBackground
 * @purpose X Windows specific version of Hv_RestoreViewBackground.
 * @see The gateway routine Hv_RestoreViewBackground.
 */


void  Hv_XMRestoreViewBackground(Hv_View View) {
  Hv_Rect       *hr;
  Hv_Region      hrrgn = NULL;

  if (View->background == 0) {
    Hv_SetViewDirty(View);
    return;
  }

  if (!Hv_ViewIsVisible(View))
    return;


  if (Hv_IsViewDirty(View)) {
	  hrrgn = Hv_HotRectRegion(View);
	  Hv_SaveViewBackground(View, hrrgn);
	  Hv_DestroyRegion(hrrgn);
  }

  hr = View->hotrectborderitem->area;

  Hv_CopyArea(View->background,
	      Hv_trueMainWindow,
	      0, 0,
	      hr->width+1, hr->height+1,
	      hr->left, hr->top);
  Hv_Flush();
}






/**
 * Hv_XMOffScreenViewUpdate
 * @purpose X Windows specific version of Hv_OffScreenViewUpdate.
 * @see The gateway routine Hv_OffScreenViewUpdate.
 */

void Hv_XMOffScreenViewUpdate(Hv_View    View,
			    Hv_Region  region) {
  Hv_Window        tempwind;
  Hv_Rect          *hr;
  Hv_Rect          prect;
  short            dh, dv;
  Hv_Region        drawrgn = NULL;
  Hv_Region        hrrgn = NULL;
  Hv_Pixmap        pmap;
  Hv_Item          hrb, temp;
  
/* check for pathologies */

  if ((Hv_inPostscriptMode) || (Hv_mainWindow == 0) || !Hv_ViewIsVisible(View))
    return;

  Hv_SetViewDirty(View);

/* create pixmap, store the mainwindow */

  hrb = View->hotrectborderitem;
  hr = View->hotrect;
  pmap = Hv_CreatePixmap(hr->width, hr->height);
  tempwind = Hv_mainWindow;
  Hv_mainWindow = pmap;
  Hv_CopyRegion(&drawrgn, region);

/* now make preparations for offscreen drawing */

  Hv_SetRect(&prect, 0, 0, hr->width, hr->height);
  Hv_ClipRect(&prect);
  
  dh = hr->left;
  dv = hr->top;

  Hv_OffsetView(View, (short)(-dh), (short)(-dv));
  Hv_OffsetRegion(drawrgn, (short)(-dh), (short)(-dv));

/* now ready to draw */

  Hv_drawingOffScreen = True;

  if (hrb != NULL) {
    if (Hv_RectInRegion(hr, drawrgn) != Hv_RECTANGLEOUT)
      Hv_DrawItem(hrb, drawrgn);
  }

  for (temp = View->items->first;  temp != NULL;  temp = temp->next)
    if (temp->domain == Hv_INSIDEHOTRECT) {
      Hv_MaybeDrawItem(temp, drawrgn);
	  
/* sublist? */
      
      if (temp->children != NULL)
	Hv_DrawItemList(temp->children, drawrgn);
    }

  Hv_drawingOffScreen = False;
  
  Hv_OffsetView(View, dh, dv);

  Hv_mainWindow = tempwind;

  Hv_DestroyRegion(drawrgn);
  hrrgn = Hv_HotRectRegion(View);
  drawrgn = Hv_IntersectRegion(hrrgn, region);

  Hv_SetClippingRegion(drawrgn);

/* now copy the pixmap on screen */

  Hv_CopyArea(pmap, Hv_trueMainWindow, 0, 0,
	      hr->width, hr->height,
	      hr->left, hr->top);
  
  Hv_Flush();
  
  Hv_DestroyRegion(hrrgn);
  Hv_DestroyRegion(drawrgn);
  Hv_RestoreClipRegion(region);
  Hv_FreePixmap(pmap);

}

/**
 * Hv_XMScrollView
 * @purpose X Windows specific version of Hv_ScrollView.
 * @see The gateway routine Hv_ScrollView.
 */

void Hv_XMScrollView(Hv_View   View,
		     int     ScrollPart,
		     Hv_Region    region,
		     Hv_Point   StartPP,
		     Hv_Point   StopPP) {
  short             tl, tw, left, top;

  short             dh;    /*horizontal shift in pixel coordinates due to scrolling*/
  short             dv;    /*vertical   shift in pixel coordinates due to scrolling*/

  float             fdh;   /*horizontal shift in world coordinates due to scrolling*/
  float             fdv;   /*vertical   shift in world coordinates due to scrolling*/

  int               src_x, src_y; 
  int               dest_x, dest_y;
  unsigned int      width, height;
  short             updwidth, updheight;
  Hv_Region            temp;
  
  Hv_Point          destP, srcP;
  Boolean           HorizontalScroll;
  short             efflen;
  Hv_Rect           rect;

/* set defaults */

  temp = Hv_RectRegion(View->hotrect);
  Hv_SetClippingRegion(temp);     /* set region to clipping region*/
  Hv_DestroyRegion(temp);

  Hv_CheckForExposures(Hv_EXPOSE); 

  dh = 0;
  dv = 0;
  fdh = 0.0;
  fdv = 0.0;
  src_x = View->hotrect->left+1;
  dest_x = src_x;
  left = src_x;
  src_y = View->hotrect->top+1;
  dest_y = src_y;
  top = src_y;

/* is this a horizontal or vertical scroll?*/

  HorizontalScroll = ((ScrollPart == Hv_INHORIZONTALTHUMB) ||
		      (ScrollPart == Hv_INRIGHTARROW) || 
		      (ScrollPart == Hv_INLEFTARROW) ||
		      (ScrollPart == Hv_INLEFTSLIDE) ||
		      (ScrollPart == Hv_INRIGHTSLIDE));

/* get geometric info abouth the thumbs */


  if (HorizontalScroll) {
    Hv_HorizThumbInfo(View, &tl, &tw, &efflen);
    if (efflen > 0) {
      dh = Hv_GetHScrollStep(View, ScrollPart, region, StartPP, StopPP, efflen);
      fdh = (dh*View->world->width)/View->local->width;
      Hv_CheckHShift(View, &dh, &fdh);
    }
  }

  else {
    Hv_VertThumbInfo(View, &tl, &tw, &efflen);
    if (efflen > 0) {
      dv = Hv_GetVScrollStep(View, ScrollPart, region, StartPP, StopPP, efflen);
      fdv = -(dv*View->world->height)/View->local->height;
      Hv_CheckVShift(View, &dv, &fdv);
    }
  }
  
/* adjust the world */

  if (dh != 0) {
    View->world->xmin += fdh;
    View->world->xmax += fdh;
  }
  
  if (dv != 0) {
    View->world->ymin += fdv;
    View->world->ymax += fdv;
  }

  Hv_FixFRectWH(View->world);


/*  Hv_UpdateThumbs(View, HorizontalScroll, tl, tw); */     /* update the slider thumbs */
  
  if (HorizontalScroll)
    Hv_sbredrawcontrol = 1;
  else
    Hv_sbredrawcontrol = 2;

  Hv_SetClippingRegion(region);     /* set region to clipping region*/

  Hv_DrawScrollBars(View, region); 
  Hv_sbredrawcontrol = 0;

/* do the actual adjustments to the offset amounts */

  if (dh < 0) {
    src_x = View->hotrect->left+1;
    dest_x = src_x - dh;
    updwidth = -dh;
    updheight = View->hotrect->height - 1;
    left = src_x;
  }
  else if (dh > 0) {
    dest_x = View->hotrect->left+1;
    src_x = dest_x + dh;
    updwidth = dh;
    updheight = View->hotrect->height - 1;
    left = Hv_RectRight(View->hotrect)-dh;
  }
  
  if (dv < 0) {
    src_y = View->hotrect->top+1;
    dest_y = src_y - dv;
    updwidth = View->hotrect->width - 1;
    updheight = -dv;
    top = src_y;
  }
  else if (dv > 0) {
    dest_y = View->hotrect->top+1;
    src_y = dest_y + dv;
    updwidth = View->hotrect->width - 1;
    updheight = dv;
    top = Hv_RectBottom(View->hotrect)-dv;
  }
  
  height = View->hotrect->height - abs(dv) - 2;
  if (height > 32000)
    height = 0;
  
  width =  View->hotrect->width -  abs(dh) - 2;
  if (width > 32000)
    width = 0;


/*
 * Note: the extra care taken below with
 * Hv_trueMainWindow is beacuse the "usual"
 * Hv_trueMainWindow may have been temporarily
 * reset due to offscreen drawing.
 */
  
  Hv_SetPoint(&destP, (short)(dest_x), (short)(dest_y));
  Hv_SetPoint(&srcP, (short)(src_x), (short)(src_y));
  if ((width > 0) && (height > 0) && (Hv_PointInRect(destP, View->hotrect))
      && (Hv_PointInRect(srcP, View->hotrect)))

    Hv_CopyArea(Hv_trueMainWindow, Hv_trueMainWindow,
		src_x, src_y,
		width, height,
		dest_x, dest_y);


/* what ever was not handled by CopyArea will be handled by an explicit
   redraw. Make sure we only redraw inside the hotrect */

  left = Hv_sMax(View->hotrect->left, Hv_sMin(left,View->hotrect->right));
  top =  Hv_sMax(View->hotrect->top,  Hv_sMin(top, View->hotrect->bottom));
  updwidth =  Hv_sMax(0, Hv_sMin(updwidth,  (short)(View->hotrect->right - left)));
  updheight = Hv_sMax(0, Hv_sMin(updheight, (short)(View->hotrect->bottom - top)));

  Hv_scrollDH = -dh;
  Hv_scrollDV = -dv;

/* reposition hotrect items */

  Hv_offsetDueToScroll = True;
  Hv_OffsetHotRectItems(View->items, (short)(-dh), (short)(-dv));
  Hv_offsetDueToScroll = False;

/* now redraw affected part */
  

  Hv_SetRect(&rect, left, top, updwidth, updheight);
  temp  = Hv_RectRegion(&rect);     /* malloc the region */

  Hv_ShrinkRegion(temp, -1, -1);

/* now the view is dirty (in case background save is used ) */

  Hv_SetViewDirty(View);  

  Hv_DrawView(View, temp);
  Hv_scrollDH = 0;
  Hv_scrollDV = 0;
  Hv_DestroyRegion(temp);

  Hv_SetClippingRegion(region);               /* set region to clipping region*/
  Hv_Flush();
  Hv_CheckForExposures(Hv_GRAPHICSEXPOSE);
  Hv_CheckForExposures(Hv_NOEXPOSE);
}

/*==========================================
 * EVENT RELATED SECTION OF GATEWAY
 *===========================================*/

/**
 * Hv_XMGo
 * @purpose X Windows specific version of Hv_Go.
 * @see The gateway routine Hv_Go.  
 */

void Hv_XMGo(void) {

  XEvent     event;

  Hv_LastStageInitialization();

  for(;;) {
      XtAppNextEvent(Hv_appContext, &event);
      XtDispatchEvent(&event);
  }
}

/**
 * Hv_XMProcessEvent
 * @purpose X Windows  specific version of Hv_ProcessEvent.
 * @see The gateway routine Hv_ProcessEvent.
 */

void  Hv_XMProcessEvent(int mask) {
  XtAppProcessEvent(Hv_appContext, mask);
}


/**
 * Hv_XMAddEventHandler
 * @purpose X Windows specific version of Hv_AddEventHandler.
 * @see The gateway routine Hv_AddEventHandler.
 */

void  Hv_XMAddEventHandler(Hv_Widget       w,
			   int             mask,
			   Hv_EventHandler handler,
			   Hv_Pointer      data) {

    XtAddEventHandler(w, mask, FALSE, handler, data);  
}

/**
 * Hv_XMRemoveTimeOut
 * @purpose X Windows specific version of Hv_RemoveTimeOut.
 * @see The gateway routine Hv_RemoveTimeOut.
 */

void  Hv_XMRemoveTimeOut(Hv_IntervalId id) {
  XtRemoveTimeOut(id);
}


/**
 * Hv_XMAddTimeOut
 * @purpose X Windows specific version of Hv_AddTimeOut.
 * @see The gateway routine Hv_AddTimeOut.
 */

Hv_IntervalId  Hv_XMAddTimeOut(unsigned long         interval,
			       Hv_TimerCallbackProc  callback,
			       Hv_Pointer            data) {

  if ((interval < 0) || (callback == NULL))
    return (Hv_IntervalId)0;

  return XtAppAddTimeOut(Hv_appContext,
			 interval,
			 callback,
			 data);
}

/**
 * Hv_XMCheckMaskEvent
 * @purpose X Windows specific version of Hv_CheckMaskEvent.
 * @see The gateway routine Hv_CheckMaskEvent.
 */

Boolean Hv_XMCheckMaskEvent(int         mask,
			    Hv_XEvent  *event) {

    int    xmask;

/* must convert hv mask to x mask */

    xmask = 0;

    if (Hv_CheckBit(mask, Hv_BUTTONPRESSMASK))
	xmask |= ButtonPressMask;
    if (Hv_CheckBit(mask, Hv_BUTTONRELEASEMASK))
	xmask |= ButtonReleaseMask;
    if (Hv_CheckBit(mask, Hv_BUTTONMOTIONMASK))
	xmask |= ButtonMotionMask;
    if (Hv_CheckBit(mask, Hv_BUTTON1MOTIONMASK))
	xmask |= Button1MotionMask;

    return XCheckMaskEvent(Hv_display, xmask, event);

}


/**
 * Hv_XMInitialize
 * @purpose X Windows specific version of Hv_Initialize.
 * @see The gateway routine Hv_Initialize.  
 */


int Hv_XMInitialize(unsigned int argc,
		    char       **argv,
		    char       *versStr) {

  char   *fbr;
  int     status;

#ifdef Hv_USEOPENGL  /* open gl 4 */
  Hv_Display     *dpy;
  GLboolean       doubleBuffer = GL_TRUE;
  GLboolean       moving = GL_FALSE;
  GLboolean       made_current = GL_FALSE;
  Colormap     cmap;
#endif  /* open gl 4 */

  fbr = (char *)fallbackresources;

/* enable tear away menus */

#ifndef _AIX
#ifndef sparc
#if XtSpecificationRelease > 4
  XmRepTypeInstallTearOffModelConverter(); /* for tear off menus */
#endif
#endif
#endif  /* end __AIX__ */
  

/* some strings used in callbacks */

  Hv_inputCallback             = XmNinputCallback;
  Hv_activateCallback          = XmNactivateCallback;
  Hv_valueChangedCallback      = XmNvalueChangedCallback;
  Hv_okCallback                = XmNokCallback;
  Hv_cancelCallback            = XmNcancelCallback;
  Hv_multipleSelectionCallback = XmNmultipleSelectionCallback;
  Hv_singleSelectionCallback   = XmNsingleSelectionCallback;
  
/* Initialize tookit. Returns Hv_toplevel An application shell) widget.
   Note that XtInitialize will automatically parse the command
   line for a set of standard options. See page 163 of the
   X toolkit Intrinsics reference manual. Note also that the
   defaults file for this application should be in:

	/usr/lib/X11/app-defaults/appName		*/

  Hv_toplevel = XtVaAppInitialize(&Hv_appContext,
				  (String)Hv_appName,
				  (XrmOptionDescList)NULL,
				  (Cardinal)0,
#if XtSpecificationRelease > 4
				  (int *)&argc,
#else
				  (Cardinal *)&argc,
#endif
				  (String *)argv,
				  (String *)fbr,
				  NULL);

/* create managed form. */

#ifdef Hv_USEOPENGL  /* open gl 5 */
  dpy = XtDisplay(Hv_toplevel);

/* find an OpenGL-capable RGB visual with depth buffer */

  Hv_3Dvi = glXChooseVisual(dpy, DefaultScreen(dpy), Hv_dblBuf);
  if (Hv_3Dvi == NULL) {
      Hv_3Dvi = glXChooseVisual(dpy, DefaultScreen(dpy), Hv_snglBuf);
      if (Hv_3Dvi == NULL)
	  XtAppError(Hv_appContext, "no RGB visual with depth buffer");
      doubleBuffer = GL_FALSE;
  }

/* create an X colormap since probably not using default visual */

  cmap = XCreateColormap(dpy, RootWindow(dpy, Hv_3Dvi->screen),
			 Hv_3Dvi->visual, AllocNone);

/*
 * Establish the visual, depth, and colormap of the toplevel
 * widget _before_ the widget is realized.
 */

  Hv_mainw = XmCreateMainWindow(Hv_toplevel, "mainw", NULL, 0);
  Hv_ManageChild(Hv_mainw);
  Hv_form = XmCreateForm(Hv_mainw, "form", NULL, 0);
  
  Hv_ManageChild(Hv_form);
  Hv_colorMap = cmap;

#else /* not open gl */
  Hv_form = XmCreateForm(Hv_toplevel, "form", NULL, 0);
  Hv_ManageChild(Hv_form);
#endif  /* open gl 5 */

  Hv_ManageChild(Hv_form);


/* some system-like parameters */

  Hv_userName = (char *)getenv("USER");
  
  if (!Hv_userName)
    Hv_userName = (char *)getenv("LOGNAME");
  
  Hv_hostName = (char *) Hv_Malloc(80);
  if (Hv_hostName)
    status = gethostname(Hv_hostName, 80);

  return status;
}


/**
 * Hv_XMSystemSpecificInit
 * @purpose X Windows specific version of Hv_SystemSpecificInit.
 * @see The gateway routine Hv_SystemSpecificInit.  
 */

void Hv_XMSystemSpecificInit() {

  Hv_AddEventHandlers();	/* add X event handlers for various events */

/* set the colormap attribute for the Hv_toplevel widget */

  XtVaSetValues(Hv_toplevel, Hv_Ncolormap, Hv_colorMap, NULL);

  XtRealizeWidget(Hv_toplevel);	/* realize the top level shell widget */
  XFlush(XtDisplay(Hv_toplevel));

  Hv_mainWindow = XtWindow(Hv_canvas);	  /* toplevel window */

  Hv_SetArg(tlargs[0], Hv_Nx, (Hv_ArgVal)(&tlx));
  Hv_SetArg(tlargs[1], Hv_Ny, (Hv_ArgVal)(&tly));
  Hv_SetArg(tlargs[2], Hv_Nwidth,  (Hv_ArgVal)(&tlw));
  Hv_SetArg(tlargs[3], Hv_Nheight, (Hv_ArgVal)(&tlh));
  XtGetValues(Hv_toplevel, tlargs, 4);

}

/**
 * Hv_XMInitWindow
 * @purpose X Windows specific version of Hv_InitWindow.
 * @see The gateway routine Hv_InitWindow.  
 */

void  Hv_XMInitWindow(void) {

/* expands the window - which insures an exposure event --
   which kick starts app */


/* get some useful geometry info.
   auto set main window to 95% of screen (centered) */
  
/* check for no or unreasonable val from resource file */

  if (tlh < 80) {
    tlx = (short) (Hv_root_width  * 0.015);
    tly = (short) (Hv_root_height * 0.035);
    tlw = (short) (Hv_root_width  * 0.95);
    tlh = (short) (Hv_root_height * 0.930);
  }

  tlw = Hv_sMin(1200, tlw);
  tlh = Hv_sMin(1000, tlh);

/* after all that, use args that were passed
   to Hv_VaInitialize, if any */

  if (Hv_mwWidth > 10)
    tlw = Hv_mwWidth;

  if (Hv_mwHeight > 10)
    tlh = Hv_mwHeight;

  if (Hv_mwLeft > -1000)
    tlx = Hv_mwLeft;

  if (Hv_mwTop > -1000)
    tly = Hv_mwTop;

  XtVaSetValues(Hv_toplevel,
		 Hv_Nx,  tlx,
		 Hv_Ny,  tly,
		 Hv_Nwidth,  tlw,
		 Hv_Nheight, tlh,
		 NULL);

}

/**
 * Hv_XMCreateAppContext
 * @purpose X Windows specific version of Hv_CreateAppContext.
 * @see The gateway routine Hv_CreateAppContext.  
 */

void  Hv_XMCreateAppContext() {
    if (Hv_appContext == NULL)
	Hv_appContext = XtWidgetToApplicationContext(Hv_toplevel);
}



/**
 * Hv_XMCreateGraphicsContext
 * @purpose X Windows specific version of Hv_CreateGraphicsContext.
 * @see The gateway routine Hv_CreateGraphicsContext.  
 */


void  Hv_XMCreateGraphicsContext(void) {

  XGCValues	 vals;		/* for setting GC attributes */
  unsigned long	 mainBg;	/* background color of main window */

  
/* retrieve fg and bg from Hv_canvas widget to use in gc */
  
  XtVaGetValues(Hv_canvas, Hv_Nforeground, &Hv_mainFg,
		 Hv_Nbackground, &mainBg, NULL);

/* see if attribute used to change bg color of main window */
  
  if (Hv_mwFillColor != NULL) {
    mainBg = Hv_colors[*Hv_mwFillColor];
    XtVaSetValues(Hv_canvas, Hv_Nbackground, &mainBg, NULL);
  }
 
/* set some default color info */
 
  vals.foreground = Hv_mainFg;
  vals.background = mainBg;
  Hv_colors[Hv_canvasColor] = mainBg;

  if (fontfixed != NULL)
    vals.font = fontfixed->fid;
  else if(fontfixed2 != NULL)
    vals.font = fontfixed2->fid;

  Hv_gc = XCreateGC(Hv_display, RootWindow(Hv_display, DefaultScreen(Hv_display)), 
		 (GCFont | GCForeground | GCBackground), &vals);
  XSetLineAttributes(Hv_display, Hv_gc, 0, LineSolid, CapButt, JoinMiter);
}


/**
 * Hv_XMInitCursors
 * @purpose X Windows specific version of Hv_InitCursors.
 * @see The gateway routine Hv_InitCursors.  
 */

void  Hv_XMInitCursors() {

  Hv_Display     *dpy;
  dpy = XtDisplay(Hv_toplevel);

  Hv_defaultCursor     = XCreateFontCursor(dpy, XC_left_ptr);  /* default cursor is an arrow */
  Hv_crossCursor       = XCreateFontCursor(dpy, XC_crosshair); /* small cross hair cursor for hot rects */
  Hv_waitCursor        = XCreateFontCursor(dpy, XC_watch);     /* watch cursor for waiting */
  Hv_dragCursor        = XCreateFontCursor(dpy, XC_fleur);     /* cursor used when dragging */
  Hv_dotboxCursor      = XCreateFontCursor(dpy, XC_dotbox);    /* cursor used when dragging */
  Hv_bottomRightCursor = XCreateFontCursor(dpy, XC_bottom_right_corner);     /* cursor used when resizing */
}

/**
 * Hv_XMInitPaterns
 * @purpose X Windows specific version of Hv_InitPatterns.
 * @see The gateway routine Hv_InitPatterns.
 */

void  Hv_XMInitPatterns() {
#define   pat_width    16
#define   pat_height   16

  int   i;

  for (i = 0; i < Hv_NUMPATTERNS; i++)
    Hv_bitmaps[i] = (unsigned char *)Hv_Malloc(33);

  
  memcpy(Hv_bitmaps[0],  pat0,  32);
  memcpy(Hv_bitmaps[1],  pat1,  32);
  memcpy(Hv_bitmaps[2],  pat2,  32);
  memcpy(Hv_bitmaps[3],  pat3,  32);
  memcpy(Hv_bitmaps[4],  pat4,  32);
  memcpy(Hv_bitmaps[5],  pat5,  32);
  memcpy(Hv_bitmaps[6],  pat6,  32);
  memcpy(Hv_bitmaps[7],  pat7,  32);
  memcpy(Hv_bitmaps[8],  pat8,  32);
  memcpy(Hv_bitmaps[9],  pat9,  32);
  memcpy(Hv_bitmaps[10], pat10, 32);  
  memcpy(Hv_bitmaps[11], pat11, 32);  

  for (i = 0; i < Hv_NUMPATTERNS; i++)
    Hv_patterns[i] = XCreateBitmapFromData(Hv_display, Hv_mainWindow,
					   (char *)Hv_bitmaps[i],
					   pat_width,
					   pat_height);
}


/**
 * Hv_XMInitGraphics
 * @purpose X Windows specific version of Hv_InitGraphics.
 * @see The gateway routine Hv_InitGraphics.  
 */

void Hv_XMInitGraphics(void) {

  Hv_Window		root;
  Hv_Dimension          crt;
  int                   dummy1;
  unsigned int          dummy2, mw, mh;
  int			n;
  Arg			args[10];
  float                 w_in;

/* get some screen information */

  XGetGeometry(Hv_display,
	       Hv_mainWindow,
	       &root,
	       &dummy1,
	       &dummy1,
	       &mw,
	       &mh,
	       &dummy2,
	       &Hv_colorDepth);
  Hv_mainWidth =  (short)mw;
  Hv_mainHeight = (short)mh;

/* get the canvas "super rect " which will be used for World <--> Local */

  n = 0;
  Hv_SetArg(args[n], Hv_Nheight, (Hv_ArgVal)(&crt));	n++;
  XtGetValues(Hv_menuBar, args, n);

  Hv_SetRect(&(Hv_canvasRect), 0, (short)crt, Hv_mainWidth, Hv_mainHeight - (short)crt);
  Hv_FixRectRB(&(Hv_canvasRect));

/* if resource file was not read in, window may have zero size */

  if (Hv_canvasRect.height <= 0) {
    XGetGeometry(Hv_display,
		 RootWindow(Hv_display, DefaultScreen(Hv_display)),
		 &root,
		 &dummy1,
		 &dummy1,
		 &mw,
		 &mh,
		 &dummy2,
		 &dummy2);
    
    Hv_mainWidth =  (short)mw;
    Hv_mainHeight = (short)mh;
    Hv_SetRect(&Hv_canvasRect, 0, (short)crt, Hv_mainWidth, Hv_mainHeight - (short)crt);
    Hv_FixRectRB(&Hv_canvasRect);
  }

/* get the screen pixel density and how much better our imaginary
   600 pix/inch postscript screen is*/

  w_in = (float)(DisplayWidthMM(Hv_display, DefaultScreen(Hv_display)))/25.4;

  Hv_screenResolution = (int)(((float)(DisplayWidth(Hv_display, DefaultScreen(Hv_display))))/w_in);
  if (Hv_screenResolution < 85)
    Hv_screenResolution = 72;
  else
    Hv_screenResolution = 100;


  Hv_psResolutionFactor = Hv_PSPIXELDENSITY/Hv_screenResolution;

/* add the canvas resize callback */

  Hv_AddCallback(Hv_canvas, XmNresizeCallback, Hv_CanvasResizeCB, 0);		

  XDefineCursor(Hv_display, XtWindow(Hv_toplevel), Hv_defaultCursor);

}



/**
 * Hv_XMInitColors
 * @purpose X Windows specific version of Hv_InitColors.
 * @see The gateway routine Hv_InitColors.  
 */

void Hv_XMInitColors(void) {
  Hv_Display	*dpy = XtDisplay(Hv_toplevel);	 /* display ID */
  int		scr = DefaultScreen(dpy);	 /* screen ID */
  unsigned int 	ncolors;		        /* number of colors */
  int           defdepth;
/* get number of colors supported by hardware */

/* version 10047 change: make it work when there are many colors 
   fix provided by Maurik Holtrop */


  ncolors = DisplayCells(dpy, scr);
  defdepth = DefaultDepth(dpy, scr);

/* Modified: 6-12-97 (Holtrop@cebaf.gov) 
 * Allow for 16k displays to work normally, and try to run on
 * 16 color displays anyway, but give a warning.
 */ 

  if (ncolors < Hv_numColors && defdepth <= 8) {
    Hv_Println("Sorry, Hv requires a monitor with at least %d colors.", Hv_numColors);
    Hv_Println("This monitor can only display %d colors.", ncolors);
    Hv_Println("But, in the hope that you have a non standard displayI will continue.");
  }

  Hv_MakeColorMap();
}


/**
 * Hv_XMInitCanvas
 * @purpose X Windows specific version of Hv_InitCanvas.
 * @see The gateway routine Hv_InitCanvas.  
 */

void Hv_XMInitCanvas(void) {

  int		n;
  Arg		args[6];

/*  XtAppAddActions(Hv_appContext, canvasactions, XtNumber(canvasactions)); */

  n = 0;
  Hv_SetArg(args[n], Hv_NtopAttachment,    XmATTACH_FORM);   n++;
  Hv_SetArg(args[n], Hv_NbottomAttachment, XmATTACH_FORM);   n++;
  Hv_SetArg(args[n], Hv_NrightAttachment,  XmATTACH_FORM);   n++;
  Hv_SetArg(args[n], Hv_NleftAttachment,   XmATTACH_FORM);   n++;

  Hv_canvas = XmCreateDrawingArea(Hv_form, "canvas", args, n);
/*  XtOverrideTranslations(Hv_canvas, XtParseTranslationTable(canvastranstable)); */
  Hv_ManageChild(Hv_canvas);
}


/**
 * Hv_XMInitGlobalWidgets
 * @purpose X Windows specific version of Hv_InitGlobalWidgets.
 * @see The gateway routine Hv_InitGlobalWidgets.  
 */


void Hv_XMInitGlobalWidgets(void) {

  int                   dummy1;
  unsigned int          dummy2, rw, rh;
  Hv_Window             root;

  Hv_display = XtDisplay(Hv_toplevel);	  /* find the display pointer for this particular cpu */

  Hv_visual = DefaultVisual(Hv_display, DefaultScreen(Hv_display));

  XDefineCursor(Hv_display, XtWindow(Hv_toplevel), Hv_waitCursor);


  XGetGeometry(Hv_display,
	       RootWindow(Hv_display, DefaultScreen(Hv_display)),
	       &root,
	       &dummy1,
	       &dummy1,
	       &rw,
	       &rh,
	       &dummy2,
	       &dummy2);

  Hv_root_width  = (short)rw;
  Hv_root_height = (short)rh;
}



/*==========================================
 * MENU RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_XMCreateMainMenuBar
 * @purpose X Windows specific version of Hv_CreateMainMenuBar.
 * @see The gateway routine Hv_CreateMainMenuBar.  
 */

Hv_Widget  Hv_XMCreateMainMenuBar(Hv_Widget   parent) {
    Hv_Widget  mb;

/*  XtAppAddActions(Hv_appContext, menuactions, XtNumber(menuactions));*/

    mb = XmCreateMenuBar(Hv_form, "menubar", NULL, 0);

    XtVaSetValues(mb,
		  Hv_NtopAttachment,   XmATTACH_FORM,
		  Hv_NrightAttachment, XmATTACH_FORM,
		  Hv_NleftAttachment,  XmATTACH_FORM,
		  NULL);

    return mb;
}

/**
 * Hv_XMCreatePopup
 * @purpose X Windows specific version of Hv_CreatePopup.
 * @see The gateway routine Hv_CreatePopup.  
 */

Hv_Widget  Hv_XMCreatePopup(Hv_Widget   parent) {
    Hv_Widget     menu;
    menu = XmCreatePopupMenu(parent, "popup", NULL, 0);
    XtVaSetValues(menu, Hv_NwhichButton, 3, NULL);
    return menu;
}


/**
 * Hv_XMCreateSubmenu
 * @purpose X Windows specific version of Hv_CreateSubmenu.
 * @see The gateway routine Hv_CreateSubmenu.  
 */

Hv_Widget  Hv_XMCreateSubmenu(Hv_Widget   parent) {
    if (parent == NULL)
	return NULL;
    return  XmCreatePulldownMenu(parent, "popup", NULL, 0);
}



/**
 * Hv_XMPopup1Callback
 * @purpose X Windows specific version of Hv_Popup1Callback.
 * @see The gateway routine Hv_Popup1Callback.  
 */


void Hv_XMPopup1Callback(Hv_Widget          dummy,
			 Hv_Widget          popup,
			 Hv_CanvasCallback *cbs) {
    
    Hv_Point    pp;
    XButtonPressedEvent *xbp;

    if ((cbs->event->xany.type != ButtonPress) ||
	(cbs->event->xbutton.button != 3))
	return;
    
    xbp = (XButtonPressedEvent *)(cbs->event);
  

    Hv_SetPoint(&pp, xbp->x, xbp->y);
    Hv_WhichView(&Hv_hotView, pp);

    if (Hv_hotView == NULL)
	return;

    Hv_ViewPopup(Hv_hotView, (Hv_XEvent *)xbp);

    XmMenuPosition(popup, xbp);
    Hv_ManageChild(popup);
}

/**
 * Hv_XMPopup2Callback
 * @purpose X Windows specific version of Hv_Popup2Callback.
 * @see The gateway routine Hv_Popup2Callback.  
 */


void Hv_XMPopup2Callback(Hv_Widget          dummy,
			 Hv_Widget          popup,
			 Hv_CanvasCallback *cbs) {
    
    Hv_Point    pp;
    XButtonPressedEvent *xbp;

    if ((cbs->event->xany.type != ButtonPress) ||
	(cbs->event->xbutton.button != 3))
	return;
    
    xbp = (XButtonPressedEvent *)(cbs->event);
  
    Hv_SetPoint(&pp, xbp->x, xbp->y);
    Hv_WhichView(&Hv_hotView, pp);

    if (Hv_hotView != NULL)
	return;

    XmMenuPosition(popup, xbp);
    Hv_ManageChild(popup);
}



/**
 * Hv_XMCreateUserMenus
 * @purpose X Windows specific version of Hv_CreateUserMenus.
 * @see The gateway routine Hv_CreateUserMenus.  
 */


void     Hv_XMCreateUserMenus(void) {

    Hv_CompoundString    cs;
    int         i;

/* creates the user menus with blank titles and makes them inactive */

    Hv_userMenuCount = 0;

    for (i = 0; i < Hv_MAXUSERMENUS; i++) {
	cs = Hv_CreateSimpleCompoundString("");
	
	Hv_userMenus[i] = XmCreatePulldownMenu(Hv_menuBar, "pulldown", NULL, 0);
	
	Hv_userPDBtns[i] = XtVaCreateManagedWidget("menubtn", 
						   xmCascadeButtonWidgetClass,  Hv_menuBar,
						   Hv_NsubMenuId,                Hv_userMenus[i],
						   Hv_NlabelString,              cs,
						   NULL);
	Hv_SetSensitivity(Hv_userPDBtns[i], False);
	
	
	Hv_CompoundStringFree(cs);
    }
}



/**
 * Hv_XMSetMenuItemString
 * @purpose X Windows specific version of Hv_SetMenuItemString.
 * @see The gateway routine Hv_SetMenuItemString.  
 */


void  Hv_XMSetMenuItemString(Hv_Widget menuitem,
			   char     *str,
			   short     font) {


  Arg                 arg[2]; /* arguments for widgets */
  int                 n;
  Hv_CompoundString   cs;

  n = 0;

  if (font == Hv_DEFAULT) {
    cs = Hv_CreateSimpleCompoundString(str);
    Hv_SetArg(arg[n], Hv_NlabelString, (Hv_ArgVal)cs);  n++;
  }
  else {
    cs = Hv_CreateCompoundString(str, font);
    Hv_SetArg(arg[n], Hv_NlabelString, (Hv_ArgVal)cs);       n++;
    Hv_SetArg(arg[n], Hv_NfontList, (Hv_ArgVal)Hv_fontList); n++;
  }

  XtSetValues(menuitem, arg, n);
  Hv_CompoundStringFree(cs);
  
}


/**
 * Hv_XMCreateHelpPullDown
 * @purpose X Windows specific version of Hv_CreateHelpPullDown.
 * @see The gateway routine Hv_CreateHelpPullDown.  
 */


Hv_Widget Hv_XMCreateHelpPullDown(void) {
  Hv_CompoundString   cs;
  Hv_Widget	      pdBtn;
  Hv_Widget           pd;

  pd = XmCreatePulldownMenu(Hv_menuBar, "pulldown", NULL, 0);
     
  cs = Hv_CreateSimpleCompoundString(" Help");

  pdBtn = XtVaCreateManagedWidget("menubtn", 
				  xmCascadeButtonWidgetClass,  Hv_menuBar,
				  Hv_NlabelString,              cs,
				  Hv_NsubMenuId,                pd,
				  NULL);
    
/* since  a help menu, set resource which will cause it to be placed at extreme right */

  XtVaSetValues(Hv_menuBar, Hv_NmenuHelpWidget, (XtArgVal)(pdBtn), NULL);
  
  Hv_CompoundStringFree(cs);
  return pd;
}



/**
 * Hv_XMCreatePullDown
 * @purpose X Windows specific version of Hv_CreatePullDown.
 * @see The gateway routine Hv_CreatePullDown.  
 */

Hv_Widget	Hv_XMCreatePullDown(char *title) {
    Hv_CompoundString  cs;
    Hv_Widget	       pdBtn;
    Hv_Widget          pd;

    cs = Hv_CreateSimpleCompoundString(title);

    if (Hv_userMenu) {
	if (Hv_userMenuCount >= Hv_MAXUSERMENUS) {
	    Hv_Println("Hv warning: cannot create more than %2d additional menus", Hv_MAXUSERMENUS);
	    pd = NULL;
	}
	else {
	    Hv_userMenuTitles[Hv_userMenuCount] = Hv_CreateSimpleCompoundString(title);
	    pd = Hv_userMenus[Hv_userMenuCount];
	    XtVaSetValues(Hv_userPDBtns[Hv_userMenuCount], Hv_NlabelString, cs, NULL);
	    Hv_SetSensitivity(Hv_userPDBtns[Hv_userMenuCount], True);
	    Hv_userMenuCount++;
	}
    }
  
  else {
      pd = XmCreatePulldownMenu(Hv_menuBar, "pulldown", NULL, 0);
      pdBtn = XtVaCreateManagedWidget("menubtn", 
				      xmCascadeButtonWidgetClass,  Hv_menuBar,
				      Hv_NlabelString,              cs,
				      Hv_NsubMenuId,                pd,
				      NULL);
      
  }
    
    Hv_CompoundStringFree(cs);
    return pd;
}


/**
 * Hv_XMAddMenuSeparator
 * @purpose X Windows specific version of Hv_AddMenuSeparator.
 * @see The gateway routine Hv_AddMenuSeparator.  
 */

Hv_Widget Hv_XMAddMenuSeparator(Hv_Widget menu) { 

    Hv_Widget		w;		/* returned widget */

    if (menu == NULL)
	return NULL;
    
    w = (Hv_Widget)XmCreateSeparatorGadget(menu, "sep", NULL, 0);
    Hv_ManageMenuItem(menu, w);
    return w;
}


/**
 * Hv_XMAddSubMenuItem
 * @purpose X Windows specific version of Hv_AddSubMenuItem.
 * @see The gateway routine Hv_AddSubMenuItem.  
 */


Hv_Widget Hv_XMAddSubMenuItem(char *label,
			      Hv_Widget menu,
			      Hv_Widget	submenu) {

/* add a sub (hierarchical) menu to a parent menu */


  Hv_Widget	w;		/* returned widget */
  Hv_CompoundString      cs;

  w = (Hv_Widget)XmCreateCascadeButtonGadget(menu, "submenuitem", NULL, 0);

  cs = Hv_CreateSimpleCompoundString(label);
  XtVaSetValues(w,
		Hv_NlabelString, cs,
		Hv_NsubMenuId, submenu,
		NULL);
  

  Hv_CompoundStringFree(cs);
  Hv_ManageMenuItem(menu, w);
  return w;
}


/**
 * Hv_XMAddMenuToggle
 * @purpose X Windows specific version of Hv_AddMenuToggle.
 * @see The gateway routine Hv_AddMenuToggle.  
 */

Hv_Widget Hv_XMAddMenuToggle(char *label,
			     Hv_Widget        menu,
			     long             ID,
			     Hv_CallbackProc  callback,
			     Boolean	      state,
			     unsigned char    btype,
			     int	      acctype,
			     char	      accchr) {


  Arg		args[10];	    /* Args are used to set/retrieve properties of resources*/
  int		n = 0;		    /* argument counter */
  Hv_Widget	w;		    /* returned widget */
  char	        *str1;		    /* test string */
  char  	str2[11];		    /* test string */
  Hv_CompoundString      cs1 = NULL;
  Hv_CompoundString      cs2 = NULL;
  char          *acstr;

/* set the resources for the toggle item */

  cs1 = Hv_CreateSimpleCompoundString(label);
  Hv_SetArg(args[n], Hv_NlabelString, (Hv_ArgVal)cs1);	n++;
  Hv_SetArg(args[n], Hv_Nset, state);	        n++;
  Hv_SetArg(args[n], Hv_NindicatorSize, 14);	n++;
  Hv_SetArg(args[n], Hv_NindicatorType, btype);	n++;
  Hv_SetArg(args[n], Hv_NvisibleWhenOff, True);	n++;


/* see if an accelerator is requested */

  if ((accchr != ' ') && (acctype != Hv_NONE)) {
    
    acstr = (char *)Hv_Malloc(2);
    acstr[0] = accchr;
    acstr[1] = '\0';


    switch (acctype) {
    case Hv_ALT:
      str1 = (char *) Hv_Malloc(strlen("Alt/") + 3);
      strcpy(str1, "Alt/");

      strcpy(str2, "Mod1<Key> ");
      break;

/* default case: CTRL */

    default: 
      str1 = (char *) Hv_Malloc(strlen("Ctrl/") + 3);
      strcpy(str1,"Ctrl/");
      
      strcpy(str2, "Ctrl<Key> ");
      break;
      
    }

    strcat(str1, acstr);
    Hv_Free(acstr);

    cs2 = Hv_CreateSimpleCompoundString(str1);
    Hv_SetArg(args[n], Hv_NacceleratorText, (Hv_ArgVal)cs2); n++;
    
    str2[9]  = accchr;
    str2[10] = '\0';

    Hv_SetArg(args[n], Hv_Naccelerator, (Hv_ArgVal)str2);    n++;
    Hv_Free(str1);
  }
  
  w = XmCreateToggleButtonGadget(menu, "toggleitem", args, n);
  Hv_AddCallback(w,
		 Hv_valueChangedCallback,
		 callback,
		 (Hv_Pointer)ID);
  
  Hv_CompoundStringFree(cs1);
  Hv_CompoundStringFree(cs2);

  Hv_ManageMenuItem(menu, w);
  return w;
}


/**
 * Hv_XMAddMenuItem
 * @purpose X Windows specific version of Hv_AddMenuItem.
 * @see The gateway routine Hv_AddMenuItem.  
 */

Hv_Widget Hv_XMAddMenuItem(char *label,
			   Hv_Widget        menu,
			   long             ID,
			   Hv_CallbackProc  callback,
			   int	            acctype,
			   char	            accchr) {		


  Hv_Arg	args[10];	   /* Args are used to set/retrieve properties of resources*/
  int		n = 0;		   /* argument counter */
  char		*str1;		   /* test string */
  char  	str2[11];	   /* test string */
  Hv_Widget	w;		   /* returned widget */
  Hv_CompoundString      cs1 = NULL;
  Hv_CompoundString      cs2 = NULL;
  char          acstr[2];

  if (menu == NULL)
    return NULL;

/* now set the resources for the menuitem */

  cs1 = Hv_CreateSimpleCompoundString(label);
  Hv_SetArg(args[n], Hv_NlabelString, (Hv_ArgVal)cs1);	n++;

/* see if an accelerator is requested */

  if ((accchr != ' ') && (acctype != Hv_NONE)) {
    switch (acctype) {
    case Hv_ALT:
      str1 = (char *) Hv_Malloc(strlen("Alt/") + 3);
      strcpy(str1,"Alt/");
      
      strcpy(str2, "Mod1<Key> ");
      break;
      
/* default case: CTRL */
	  
    default: 
      str1 = (char *) Hv_Malloc(strlen("Ctrl/") + 3);
      strcpy(str1,"Ctrl/");
      
      strcpy(str2, "Ctrl<Key> ");
      break;
    }
      
    acstr[0] = accchr;
    acstr[1] = '\0';
    strcat(str1, acstr);

    cs2 = Hv_CreateSimpleCompoundString(str1);
    Hv_SetArg(args[n], Hv_NacceleratorText, (Hv_ArgVal)cs2); n++;
    
    str2[9]  = accchr;
    str2[10] = '\0';

    Hv_SetArg(args[n], Hv_Naccelerator, (Hv_ArgVal)str2);    n++;
    Hv_Free(str1);
  }
  
  w = XmCreatePushButtonGadget(menu, "menuitem", args, n);
  Hv_AddCallback(w,
		 Hv_activateCallback,
		 callback,
		 (Hv_Pointer)ID);
  
  
  Hv_CompoundStringFree(cs1);
  Hv_CompoundStringFree(cs2);

  Hv_ManageMenuItem(menu, w);
  return w;
}


/*==========================================
 * MOUSE RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_XMGetCurrentPointerLocation
 * @purpose X Windows specific version of Hv_GetCurrentPointerLocation.
 * @see The gateway routine Hv_XGetCurrentPointerLocation.  
 */

 void  Hv_XMGetCurrentPointerLocation(short *x,
				      short *y) {
     Boolean      samescreen;
     Window       root, child;

     int          rootx, rooty, wx, wy;
     unsigned int keys;

     samescreen = XQueryPointer(Hv_display,
				Hv_mainWindow,
				&root, &child,
				&rootx, &rooty,
				&wx, &wy,
				&keys);

     
     if (samescreen) {
       *x = (short)wx;
       *y = (short)wy;
     }
     else {
       *x = -32766;
       *y = -32766;
     }
 }


/*==========================================
 * COMPOUND STRING  RELATED SECTION OF GATEWAY
 *===========================================*/

/**
 * Hv_XMCreateSimpleCompoundString
 * @purpose X Windows specific version of Hv_CreateSimpleCompoundString.
 * @see The gateway routine Hv_CreateSimpleCompoundString.  
 */

Hv_CompoundString Hv_XMCreateSimpleCompoundString(char  *text) {
  return Hv_XMCreateCompoundString(text, Hv_DEFAULT);
}


/**
 * Hv_XMCreateCompoundString
 * @purpose X Windows specific version of Hv_CreateCompoundString.
 * @see The gateway routine Hv_CreateCompoundString.
 */

Hv_CompoundString Hv_XMCreateCompoundString(char  *str,
					    short  font) {
    Hv_CompoundString   cs;
    
    if (str == NULL) {
	Hv_Println("Hv benign warning: tried to create a NULL Motif string.");
	return NULL;
    }
    
    if (font == Hv_DEFAULT)
	cs = XmStringCreateSimple(str);
    else {
	Hv_CheckFont(font);
	cs = XmStringCreate(str, Hv_charSet[font]);
    }
    
    return  cs;
}


/**
 * Hv_XMCreateAndSizeCompoundString
 * @purpose X Windows specific version of Hv_CreateAndSizeCompoundString.
 * @see The gateway routine Hv_CreateAndSizeCompoundString.
 */

Hv_CompoundString Hv_XMCreateAndSizeCompoundString(char  *text,
						   short  font,
						   short *w,
						   short *h) {
  Hv_CompoundString   cs;
  
  cs = Hv_CreateCompoundString(text, font);
  
  if (cs == NULL) {
    *w = 0;
    *h = 0;
    return NULL;
  }
  
  if (w)
    *w = (short)XmStringWidth (Hv_fontList, cs);
  
  if (h)
    *h = (short)XmStringHeight(Hv_fontList, cs);
  
  return  cs;
}

/**
 * Hv_XMCompoundStringFree
 * @purpose X Windows specific version of Hv_CompoundStringFree.
 * @see The gateway routine Hv_CompoundStringFree.  
 */

void Hv_XMCompoundStringFree(Hv_CompoundString cs) {
  XmStringFree(cs);
}

/**
 * Hv_XMCompoundStringDrawImage
 * @purpose X Windows specific version of Hv_CompoundStringDrawImage.
 * @see The gateway routine Hv_CompoundStringDrawImage.  
 */

void Hv_XMCompoundStringDrawImage(Hv_CompoundString cs,
				  short             x,
				  short             y,
				  short             sw) {

  XmStringDrawImage(Hv_display,
		    Hv_mainWindow,
		    Hv_fontList,
		    cs,
		    Hv_gc,
		    x,
		    y,
		    sw,
		    XmALIGNMENT_BEGINNING,
		    XmALIGNMENT_BEGINNING,
		    NULL);
  
}


/**
 * Hv_XMCompoundStringWidth
 * @purpose X Windows  specific version of Hv_CompoundStringWidth.
 * @see The gateway routine Hv_CompoundStringWidth. 
 */


short Hv_XMCompoundStringWidth(Hv_CompoundString cs) {
    return (short)(XmStringWidth(Hv_fontList, cs));
}

/**
 * Hv_XMCompoundStringBaseline
 * @purpose X Windows  specific version of Hv_CompoundStringBaseline.
 * @see The gateway routine Hv_CompoundStringBaseline. 
 */


short Hv_XMCompoundStringBaseline(Hv_CompoundString cs) {
    return (short)(XmStringBaseline(Hv_fontList, cs));
}

/**
 * Hv_XMCompoundStringHeight
 * @purpose X Windows  specific version of Hv_CompoundStringHeight.
 * @see The gateway routine Hv_CompoundStringHeight. 
 */


short Hv_XMCompoundStringHeight(Hv_CompoundString cs) {
    return (short)(XmStringHeight(Hv_fontList, cs));
}

/**
 * Hv_XMCompoundStringDraw
 * @purpose X Windows specific version of Hv_CompoundStringDraw.
 * @see The gateway routine Hv_CompoundStringDraw.
 */

void Hv_XMCompoundStringDraw(Hv_CompoundString cs,
			     short             x,
			     short             y,
			     short             sw) {
  XmStringDraw(Hv_display,
	       Hv_mainWindow,
	       Hv_fontList,
	       cs,
	       Hv_gc,
	       x,
	       y,
	       sw,
	       XmALIGNMENT_BEGINNING,
	       XmALIGNMENT_BEGINNING,
	       NULL);
}

/*==========================================
 * REGION RELATED SECTION OF GATEWAY
 *===========================================*/

/**
 * Hv_XMUnionRectangleWithRegion
 * @purpose X Windows specific version of Hv_UnionRectangleWithRegion.
 * @see The gateway routine Hv_UnionRectangleWithRegion.
 */

void Hv_XMUnionRectangleWithRegion(Hv_Rectangle  *xr,
				   Hv_Region     region) {

  if ((xr == NULL) || (region == NULL))
    return;

  XUnionRectWithRegion(xr, (Region)region, (Region)region);
}
 

/**
 * Hv_XMIntersectRegion
 * @purpose X Windows specific version of Hv_IntersectRegion.
 * @see The gateway routine Hv_IntersectRegion.  
 */


Hv_Region Hv_XMIntersectRegion(Hv_Region reg1,
			       Hv_Region reg2) {
  Hv_Region   intersectreg;
  
  if ((reg1 == NULL) && (reg2 == NULL))
    intersectreg = Hv_CreateRegion();
  else {
    if (reg1 == NULL)
      Hv_CopyRegion(&intersectreg, reg2);
    
    else if (reg2 == NULL)
      Hv_CopyRegion(&intersectreg, reg1);
    
    else {
      intersectreg = Hv_CreateRegion();
      XIntersectRegion((Region)reg1, (Region)reg2, (Region)intersectreg);
    }
  }
  
  return intersectreg;
}


/**
 * Hv_XMSubtractRegion
 * @purpose X Windows specific version of Hv_SubtractRegion.
 * @see The gateway routine Hv_SubtractRegion.  
 */


Hv_Region Hv_XMSubtractRegion(Hv_Region reg1,
			      Hv_Region reg2) {
  Hv_Region   diffreg;

  if (reg1 == NULL)
    return NULL;

  if (reg2 == NULL)
    Hv_CopyRegion(&diffreg, reg1);
  else {
    diffreg = Hv_CreateRegion();
    XSubtractRegion((Region)reg1, (Region)reg2, (Region)diffreg);
  }

  return diffreg;
}


/**
 * Hv_XMUnionRegion
 * @purpose X Windows specific version of Hv_UnionRegion.
 * @see The gateway routine Hv_UnionRegion.
 */

Hv_Region Hv_XMUnionRegion(Hv_Region reg1,
			   Hv_Region reg2) {
  Hv_Region   unionreg;

  if ((reg1 == NULL)  && (reg2 == NULL))
    return Hv_CreateRegion();

  if (reg1 == NULL)
    Hv_CopyRegion(&unionreg, reg2);
  else if (reg2 == NULL)
    Hv_CopyRegion(&unionreg, reg1);
  else {
    unionreg = Hv_CreateRegion();
    XUnionRegion((Region)reg1, (Region)reg2, (Region)unionreg);
  }

  return unionreg;
}


/**
 * Hv_XMShrinkRegion
 * @purpose X Windows specific version of Hv_ShrinkRegion.
 * @see The gateway routine Hv_ShrinkRegion.  
 */


void Hv_XMShrinkRegion(Hv_Region region,
		       short     h,
		       short     v) {
  if(region==NULL)
    return;
  XShrinkRegion((Region)region, h, v);
}

/**
 * Hv_XMOffsetRegion
 * @purpose X Windows specific version of Hv_OffsetRegion.
 * @see The gateway routine Hv_OffsetRegion.  
 */


void Hv_XMOffsetRegion(Hv_Region region,
		       short     h,
		       short     v) {
  XOffsetRegion((Region)region, h, v);
}


/**
 * Hv_XMCreateRegionFromPolygon
 * @purpose X Windows specific version of Hv_CreateRegionFromPolygon.
 * @see The gateway routine Hv_CreateRegionFromPolygon.  
 */

Hv_Region Hv_XMCreateRegionFromPolygon(Hv_Point  *poly,
				       short      n) {
  Hv_Region    region;

  if (n < 2)
    return NULL;
  else if (n == 2) {
    region = Hv_CreateRegionFromLine(poly[0].x, poly[0].y,
				     poly[1].x, poly[1].y, 3);
    return region;
  }

  region = XPolygonRegion(poly, (int)n, (int)EvenOddRule);
  if (region == NULL) {
    Hv_Println("Allocation failure in Hv_XMCreateRegion. Exiting.");
    exit (-1);
  }

  Hv_activePointers++;
  return region;
}


/**
 * Hv_XMEmptyRegion 
 * @purpose X Windows specific version of Hv_EmpytRegion.
 * @see The gateway routine Hv_EmptyRegion.
 */

Boolean  Hv_XMEmptyRegion(Hv_Region region) {
	return XEmptyRegion(region);
}

/**
 * Hv_XMCreateRegion
 * @purpose X Windows specific version of Hv_CreateRegion.
 * @see The gateway routine Hv_CreateRegion.  
 */
 
Hv_Region  Hv_XMCreateRegion(void) {
  Hv_Region    region;

  region = XCreateRegion();
  if (region == NULL) {
    Hv_Println("Allocation failure in Hv_XMCreateRegion. Exiting.");
    exit (-1);
  }

  Hv_activePointers++;
  return region;
}

/**
 * Hv_XMDestroyRegion
 * @purpose X Windows specific version of Hv_DestroyRegion.
 * @see The gateway routine Hv_DestroyRegion.  
 */
 
void  Hv_XMDestroyRegion(Hv_Region region) {

  if (region == NULL)
    return;
  
  Hv_activePointers--;
  XDestroyRegion(region);
}


/**
 * Hv_XMPointInRegion
 * @purpose X Windows specific version of Hv_PointInRegion.
 * @see The gateway routine Hv_PointInRegion.  
 */

Boolean Hv_XMPointInRegion(Hv_Point  pp,
			 Hv_Region region) {
    return  XPointInRegion(region, pp.x, pp.y);
}


/**
 * Hv_XMRectInRegion
 * @purpose X Windows specific version of Hv_RectInRegion.
 * @see The gateway routine Hv_RectInRegion.  
 */

Boolean  Hv_XMRectInRegion(Hv_Rect   *r,
			   Hv_Region region) {

  return (XRectInRegion((Region)region,
			r->left, r->top,
			r->width+1, r->height+1) != RectangleOut);
}


/**
 * Hv_XMSetFunction
 * @purpose X Windows specific version of Hv_SetFunction.
 * @see The gateway routine Hv_SetFunction.  
 */

void Hv_XMSetFunction(int funcbit) {
  XSetFunction(Hv_display, Hv_gc, funcbit);
}


/**
 * Hv_XMGCValues
 * @purpose X Windows specific version of Hv_GCValues.
 * @see The gateway routine Hv_GCValues.
 */

void Hv_XMGetGCValues(int *gcv) {
  XGetGCValues(Hv_display,
	       Hv_gc,
	       GCFunction,
	       (XGCValues *)gcv); 
}

/**
 * Hv_XMClipBox
 * @purpose X Windows specific version of Hv_ClipBox.
 * @see The gateway routine Hv_ClipBox.  
 */

void     Hv_XMClipBox(Hv_Region  region,
		      Hv_Rect   *rect) {

   Hv_Rectangle    xrect;
   XClipBox((Region)region, &xrect);
   Hv_SetRect(rect, xrect.x, xrect.y, xrect.width, xrect.height);
}

/**
 * Hv_XMCheckForExposures
 * @purpose X Windows specific version of Hv_CheckForExposures.
 * @see The gateway routine Hv_CheckForExposures.  
 */

void Hv_XMCheckForExposures(int whattype) {
    
/* int    whattype  (either Expose or DrawExpose) */
    
  XEvent     event;
  Hv_Region  updregion = NULL;
  
  XSync(Hv_display, False); 
  
  while(XCheckTypedWindowEvent(Hv_display, Hv_mainWindow, whattype, &event)) {
      if (!updregion)
	  updregion = Hv_CreateRegion();
      XtAddExposureToRegion(&event, (Region)updregion);
      XSync(Hv_display, False);
  }
  
  if (updregion) {	
    Hv_handlingExposure = True;
    Hv_UpdateViews(updregion);
    Hv_excludeFromExposure = NULL;
    Hv_handlingExposure = False;
    Hv_KillRegion(&updregion);
  }
}

/*==========================================
 * DRAWING RELATED SECTION OF GATEWAY
 *===========================================*/



/**
 * Hv_XMSetLineStyle
 * @purpose X Windows  specific version of Hv_SetLineStyle.
 * @see The gateway routine Hv_SetLineStyle. 
 */


void Hv_XMSetLineStyle(int w,
		       int style) {

  static int oldw = -99;
  static int oldstyle = -99;
  int        offset = 0;
  int        dashlen = 2;
  int        dotdashlen = 4;

  static char dashed[2] = {4, 4};
  static char longdashed[2] = {8, 8};
  static char dotdash[4] = {8, 4, 3, 4};


  if (w < 2)
      w = 0;

  if ((w != oldw) || (style != oldstyle)) {
      if (style <= Hv_SOLIDLINE)
	  XSetLineAttributes(Hv_display, Hv_gc, w, LineSolid, CapButt, JoinMiter);
      else {
	  XSetLineAttributes(Hv_display, Hv_gc, w, LineOnOffDash, CapButt, JoinMiter); 
	  if (style == Hv_DASHED)
	      XSetDashes(Hv_display, Hv_gc, offset, dashed, dashlen);
	  else if (style == Hv_LONGDASHED)
	      XSetDashes(Hv_display, Hv_gc, offset, longdashed, dashlen);
	  else if (style == Hv_DOTDASH)
	      XSetDashes(Hv_display, Hv_gc, offset, dotdash, dotdashlen);
      }
      
      
      oldw = w;
      oldstyle = style;
  }
}



/**
 * Hv_XMSetPattern
 * @purpose X Windows  specific version of Hv_SetPattern.
 * @see The gateway routine Hv_SetPattern. 
 */


void Hv_XMSetPattern(int pattern,
		     short color) {

    if ((pattern <= Hv_FILLSOLIDPAT) || (pattern >= Hv_NUMPATTERNS))
	Hv_SetFillStyle(Hv_FillSolid);
    else {
	XSetStipple(Hv_display, Hv_gc, Hv_patterns[pattern]);
	Hv_SetFillStyle(Hv_FillStippled);
    }
}

/**
 * Hv_XMEraseRectangle
 * @purpose X Windows  specific version of Hv_EraseRectangle.
 * @see The gateway routine Hv_EraseRectangle. 
 */

void	Hv_XMEraseRectangle(short left,
			  short top,
			  short width,
			  short height) {
    
/* erase specified rect on specified window
   by filling the rect with the background color */
    
    if (Hv_TileDraw)
	Hv_SetFillStyle(Hv_FillTiled);
    
    Hv_FillRectangle(left, top, width, height, Hv_canvasColor);
    Hv_SetFillStyle(Hv_FillSolid);
}

/**
 * Hv_XMTileMainWindow
 * @purpose X Windows  specific version of Hv_TileMainWindow.
 * @see The gateway routine Hv_TileMainWindow. 
 */


void   Hv_XMTileMainWindow(Hv_Pixmap pmap) {
    if ((pmap == 0) || (Hv_display == NULL) || (Hv_mainWindow == 0))
	return;
    
    XSetWindowBackgroundPixmap(Hv_display, Hv_mainWindow, (Pixmap)pmap);
    XSetTile(Hv_display, Hv_gc, (Pixmap)pmap);
    XClearArea(Hv_display, Hv_mainWindow, 0, 0, 0, 0, True);
}

/**
 * Hv_XMReTileMainWindow
 * @purpose X Windows  specific version of Hv_ReTileMainWindow.
 * @see The gateway routine Hv_TileMainWindow. 
 */

void Hv_XMReTileMainWindow(void) {

  Hv_Window      twind;
  Hv_Pixmap      pmap;

  if ((Hv_TileDraw != NULL) &&(Hv_mainWindow != 0)) {
    pmap = Hv_CreatePixmap(Hv_tileX, Hv_tileY);
    twind = Hv_mainWindow;
    Hv_mainWindow = pmap;
    Hv_TileDraw();
    Hv_mainWindow = twind;
    Hv_TileMainWindow(pmap);
    Hv_FreePixmap(pmap);
  }
}

/**
 * Hv_XMSetBackground
 * @purpose X Windows  specific version of Hv_SetBackground.
 * @see The gateway routine Hv_SetBackground. 
 */


void  Hv_XMSetBackground(unsigned long bg) {
    XSetBackground(Hv_display, Hv_gc, bg);
}



/**
 * Hv_XMSetForeground
 * @purpose X Windows  specific version of Hv_SetForeground.
 * @see The gateway routine Hv_SetForeground. 
 */


void  Hv_XMSetForeground(unsigned long fg) {
    XSetForeground(Hv_display, Hv_gc, fg);
}


/**
 * Hv_XMSetFillStyle
 * @purpose X Windows  specific version of Hv_SetFillStyle.
 * @see The gateway routine Hv_SetFillStyle. 
 */


void  Hv_XMSetFillStyle(int fillstyle) {
    XSetFillStyle(Hv_display, Hv_gc, fillstyle);
}



/**
 * Hv_XMSetLineWidth
 * @purpose X Windows  specific version of Hv_SetLineWidth.
 * @see The gateway routine Hv_SetLineWidth. 
 */


void Hv_XMSetLineWidth(int w) {
    if (w < 2)
	w = 0;
    XSetLineAttributes(Hv_display, Hv_gc, w, LineSolid, CapButt, JoinMiter); 
}



/**
 * Hv_XMSetClippingRegion
 * @purpose X Windows  specific version of Hv_SetClippingRegion.
 * @see The gateway routine Hv_SetClippingRegion. 
 */


void  Hv_XMSetClippingRegion(Hv_Region region) {
    XSetRegion(Hv_display, Hv_gc, (Region)region);   /* set the clipping area to the region */
    Hv_Sync();
}


/**
 * Hv_XMFreePixmap
 * @purpose X Windows  specific version of Hv_FreePixmap.
 * @see The gateway routine Hv_FreePixmap. 
 */


void   Hv_XMFreePixmap(Hv_Pixmap pmap) {

  if (pmap == 0)
    return;

  XFreePixmap(Hv_display, (Pixmap)pmap);
}


/**
 * Hv_XMCreatePixmap
 * @purpose  X Windows specific version of Hv_CreatePixmap. 
 * @see The gateway routine Hv_CreatePixmap.  
 */


Hv_Pixmap  Hv_XMCreatePixmap(unsigned int  w,
			     unsigned int  h) {

/*
 * Note: the extra care taken below with
 * Hv_trueMainWindow is because the "usual"
 * Hv_trueMainWindow may have been temporarily
 * reset due to offscreen drawing.
 */


  Hv_Pixmap           pmap;
  unsigned int        depth;
  Window              twin;
  Hv_Rect             rect;

  if (Hv_inPostscriptMode)
    return 0;

  if (Hv_trueMainWindow == 0) {
    Hv_Println("in Hv_CreatePixmap with Hv_trueMainWindow = 0");
    return 0;
  }

  if (Hv_display == NULL)
    return 0;

  depth = DefaultDepthOfScreen(DefaultScreenOfDisplay(Hv_display));

  pmap = XCreatePixmap(Hv_display,
		       Hv_trueMainWindow,
		       w, h, depth);

  twin = Hv_mainWindow;
  Hv_mainWindow = pmap;

  Hv_SetFunction(Hv_GXCOPY);
  
  Hv_SetRect(&rect, 0, 0, (short)w, (short)h);
  Hv_ClipRect(&rect);
  Hv_FillRect(&rect, Hv_white);
  Hv_mainWindow = twin;
  return  pmap;
}

/**
 * Hv_XMClearArea
 * @purpose Windows 9X/NT specific version of Hv_ClearArea.
 * @see The gateway routine Hv_ClearArea. 
 */

void Hv_XMClearArea(short x,
				    short y,
				    short w,
				    short h) {
	XClearArea(Hv_display, Hv_mainWindow, x, y, w, h, True);
}

/**
 * Hv_XMCopyArea
 * @purpose X Windows  specific version of Hv_CopyArea.
 * @see The gateway routine Hv_CopyArea. 
 */


void Hv_XMCopyArea(Hv_Window src,
		 Hv_Window dest,
		 int       sx,
		 int       sy,
		 unsigned  int w,
		 unsigned  int h,
		 int       dx,
		 int       dy) {

    XCopyArea(Hv_display,
	      src,
	      dest,
	      Hv_gc,
	      sx,
	      sy,
	      w,
	      h,
	      dx,
	      dy);
}



/**
 * Hv_XMDestroyImage
 * @purpose X Windows  specific version of Hv_DestroyImage.
 * @see The gateway routine Hv_DestroyImage. 
 */

void Hv_XMDestroyImage(Hv_Image *image) {

  if (image->data)
    free(image->data);
  image->data = NULL;
  XDestroyImage(image);
}

/**
 * Hv_XMPutImage
 * @purpose X Windows  specific version of Hv_PutImage.
 * @see The gateway routine Hv_PutImage. 
 */


void Hv_XMPutImage(Hv_Window window,
		 Hv_Image *image,
		 int       sx,
		 int       sy,
		 int       dx,
		 int       dy,
		 unsigned  int w,
		 unsigned  int h) {
    
    XPutImage(Hv_display, window, Hv_gc,
	      image,
	      sx, sy,
	      dx, dy,
	      w, h);
}


/**
 * Hv_XMDrawTextItemVertical
 * @purpose X Windows  specific version of Hv_DrawTextItemVertical.
 * @see The gateway routine Hv_DrawTextItemVertical. 
 */


void Hv_XMDrawTextItemVertical(Hv_Item    Item,
			       Hv_Region  region) {

    Hv_Pixmap	    pmap1, pmap2;  /* off screen pixmaps required for rotating text */
    Hv_Rect         parea;
    int             w, h;
    Hv_ColorScheme *scheme;
    Hv_String      *str;
    Hv_Rect        *area;
    Hv_Window       twin;
    short           x, y, extra;
    XImage         *image1, *image2;
    int		   i,j;			        /* loopers */
    short	   desty;		        /* destination vert pixel */
    Hv_Region      temp, temp2;

    if (Item == NULL)
	return;

    if (Item->type == Hv_TEXTENTRYITEM)
	extra = Hv_TEXTENTRYEXTRA;
    else
	extra = Hv_STRINGEXTRA;

    str    = Item->str;
    if (str == NULL)
	return;

    area   = Item->area;
    scheme = Item->scheme;

    w = (int)area->height;
    h = (int)area->width;
	
    pmap1 = Hv_CreatePixmap(w, h);  /* horizontal */
    pmap2 = Hv_CreatePixmap(h, w);  /* vertical */
	
/* Draw string onto offscreen horizontal pixmap */
	
    twin = Hv_mainWindow;
    Hv_mainWindow = pmap1;
	
    Hv_SetRect(&parea, 0, 0, w+1, h+1);
	
/* note: I will not be here if in postscript mode */
	
    Hv_ClipRect(&parea);
	
    if (str->fillcol >= 0)
	Hv_FillRect(&parea, str->fillcol);
    else {
	if (Item->domain != Hv_INSIDEHOTRECT)
	    Hv_FillRect(&parea, scheme->fillcolor);
	else { /* inside hotrect */
	  if (Item->view->hotrectborderitem)
	    Hv_FillRect(&parea, Item->view->hotrectborderitem->color);
	}
    }

    Hv_CompoundStringXYFromArea(Item->str, &x, &y, Item->area, extra);
    x = x - Item->area->left;
    y = Item->area->bottom - y;
    
    Hv_DrawCompoundString(y, x, str);
    Hv_mainWindow = twin;
	
/* ALGORITHM:
   pmap1 -> image1;
   pmap2 -> imag2;
   image1 -> image2;
   image2 -> screen
   
   Manipulate the images, which are stored in the client */
	
    image1 = XGetImage(Hv_display,
		       (Pixmap)pmap1,
		       0,
		       0,
		       (int)w,
		       (int)h,
		       AllPlanes,
		       ZPixmap);
    
    image2 = XGetImage(Hv_display,
		       (Pixmap)pmap2,
		       0,
		       0,
		       (int)h,
		       (int)w,
		       AllPlanes,
		       ZPixmap);
	
/* now rotate text pixels from image1 onto image 2 */
	
    for (i = 1; i < (int)w; i++) {
	desty = w-i;
	for (j = 1; j < (int)h; j++)
	    XPutPixel(image2, j, desty, XGetPixel(image1, i, j));
    }
	
	
/* Now that image2 contains the rotated text, put it onto the screen */
	
    temp2 = Hv_RectRegion(area);       /* set a region to the rect */
    Hv_ShrinkRegion(temp2, -1, -1);
    temp = Hv_IntersectRegion(region, temp2);
    Hv_SetClippingRegion(temp);
    Hv_DestroyRegion(temp);
    Hv_DestroyRegion(temp2);
    
    if (str->fillcol >= 0)
	Hv_FillRect(area, str->fillcol);
    else {
	if (Item->domain != Hv_INSIDEHOTRECT)
	    Hv_FillRect(area, scheme->fillcolor);
    }
	
    Hv_PutImage(Hv_mainWindow, image2,
		1, 1,
		area->left, area->top,
		h, w); 
    Hv_Flush();
	
/* add a 3D frame? */
	
    if (Hv_CheckBit(Item->drawcontrol, Hv_FRAMEAREA))
	Hv_Simple3DRect(area, scheme->topcolor < scheme->bottomcolor, -1);
	
/* free memory used by images and pixmaps */
	
    Hv_FreePixmap(pmap2);
    Hv_DestroyImage(image2);
    Hv_DestroyImage(image1);
    Hv_FreePixmap(pmap1);
}


/**
 * Hv_XMDrawPoint
 * @purpose X Windows specific version of Hv_DrawPoint.
 * @see The gateway routine Hv_DrawPoint.  
 */

void Hv_XMDrawPoint(short x,
		    short y,
		    short color) {

  unsigned long old_fg;
  
/* draws one pixel point on the screen of a given color at (x,y) */
    
  old_fg = Hv_SwapForeground(color);
  XDrawPoint(Hv_display, Hv_mainWindow, Hv_gc, x, y);
  Hv_SetForeground(old_fg);
}

/**
 * Hv_XMDrawPoints
 * @purpose X Windows specific version of Hv_DrawPoints.
 * @see The gateway routine Hv_DrawPoints.
 */

void Hv_XMDrawPoints(Hv_Point *xp,
		     int       np,
		     short     color) {

  unsigned long  old_fg;
  
  old_fg = Hv_SwapForeground(color);
  
  XDrawPoints(Hv_display,
	      Hv_mainWindow,
	      Hv_gc,
	      xp,
	      np,
	      CoordModeOrigin);
  
  Hv_SetForeground(old_fg);
}

/**
 * Hv_XMFrameArc
 * @purpose X Windows specific version of Hv_FrameArc.
 * @see The gateway routine Hv_FrameArc.  
 */

void	Hv_XMFrameArc(short   x,
		    short     y,
		    short     width,
		    short     height,
		    int       ang1,
		    int       ang2,
		    short     color) {

    unsigned long   old_fg;
    
    old_fg = Hv_SwapForeground(color);
    XDrawArc(Hv_display,
	     Hv_mainWindow,
	     Hv_gc,
	     x, y,
	     width, height,
	     ang1, ang2);
    Hv_SetForeground(old_fg);
}

/**
 * Hv_XMFillArc
 * @purpose X Windows specific version of Hv_FillArc.
 * @see The gateway routine Hv_FillArc.  
 */

void  Hv_XMFillArc(short   x,
		   short   y,
		   short   width,
		   short   height,
		   int     ang1,
		   int     ang2,
		   Boolean frame,
		   short   color,
		   short   framec) {

  unsigned long   old_fg;
    
  old_fg = Hv_SwapForeground(color);
  XFillArc(Hv_display,
	   Hv_mainWindow,
	   Hv_gc,
	   x, y,
	   width, height,
	   ang1, ang2);
  Hv_SetForeground(old_fg);

  if (frame)
    Hv_FrameArc(x, y, width, height, ang1, ang2, framec);

}

/**
 * Hv_XMDrawLines
 * @purpose X Windows specific version of Hv_DrawLines.
 * @see The gateway routine Hv_DrawLines.  
 */

void Hv_XMDrawLines(Hv_Point  *xp,
		    short      np,
		    short      color) {
    
  unsigned long   old_fg;
  
  old_fg = Hv_SwapForeground(color);
  XDrawLines(Hv_display,
	     Hv_mainWindow,
	     Hv_gc,
	     xp,
	     np,
	     CoordModeOrigin);
  Hv_SetForeground(old_fg);
}

/**
 * Hv_XMFillPolygon
 * @purpose X Windows specific version of Hv_FillPolygon.
 * @see The gateway routine Hv_FillPolygon.  
 */

void Hv_XMFillPolygon(Hv_Point   *xp,
		      short       np,
		      Boolean     frame,
		      short       fillcolor,
		      short       framecolor) {
    
   
  unsigned long   old_fg;
  old_fg = Hv_SwapForeground(fillcolor);
    
  if (fillcolor >= 0)
    XFillPolygon(Hv_display,
		 Hv_mainWindow,
		 Hv_gc,
		 xp,
		 np,
		 Nonconvex,
		 CoordModeOrigin); 

/* note: Hv_FramePolygon is NOT a gateway function */
    
  if (frame)
    Hv_FramePolygon(xp, np, framecolor);
  
  Hv_SetForeground(old_fg);
}

/**
 * Hv_XMDrawSegments
 * @purpose X Windows specific version of Hv_DrawSegments
 * @see The gateway routine Hv_DrawSegments.  
 */

void Hv_XMDrawSegments(Hv_Segment *xseg,
		       short       nseg,
		       short       color) {
    
    unsigned long   old_fg;
    
    old_fg = Hv_SwapForeground(color);
    XDrawSegments(Hv_display, Hv_mainWindow, Hv_gc, xseg, nseg);
    Hv_SetForeground(old_fg);
}

/**
 * Hv_XMFillRect
 * @purpose X Windows specific version of Hv_FillRect.
 * @see The gateway routine Hv_FillRect.  
 */



void	Hv_XMFillRect(Hv_Rect *rect,
		      short    color) {

  unsigned long   old_fg;
    
  old_fg = Hv_SwapForeground(color);
  
  XFillRectangle(Hv_display,
		 Hv_mainWindow,
		 Hv_gc,
		 rect->left,
		 rect->top,
		 rect->width,
		 rect->height);
  
  Hv_SetForeground(old_fg);
}

/**
 * Hv_XMFrameRect
 * @purpose X Windows specific version of Hv_FrameRect.
 * @see The gateway routine Hv_FrameRect.  
 */

void Hv_XMFrameRect (Hv_Rect *rect,
		     short    color) {
    
  unsigned long   old_fg;
    
  old_fg = Hv_SwapForeground(color);
  
/* draw the rectangle */
  
  XDrawRectangle(Hv_display,
		 Hv_mainWindow,
		 Hv_gc,
		 rect->left,
		 rect->top,
		 rect->width,
		 rect->height);
  
/* revert to the old foreground color */
  
  Hv_SetForeground(old_fg);
}


/**
 * Hv_XMFillRectangle
 * @purpose X Windows specific version of Hv_FillRectangle.
 * @see The gateway routine Hv_FillRectangle.
 */

void	Hv_XMFillRectangle(short left,
			   short top,
			   short width,
			   short height,
			   short color) {
    unsigned long   old_fg;
    
    old_fg = Hv_SwapForeground(color);
    XFillRectangle(Hv_display, Hv_mainWindow, Hv_gc, left, top, width, height);
    Hv_SetForeground(old_fg);
}


/**
 * Hv_XMFillRectangles
 * @purpose X Windows specific version of Hv_FillRectangles.
 * @see The gateway routine Hv_FillRectangles.
 */

void   Hv_XMFillRectangles(Hv_Rectangle *rects,
			   int            np,
			   short          color) {
    
    unsigned long   old_fg;
    
    old_fg = Hv_SwapForeground(color);
    XFillRectangles(Hv_display, Hv_mainWindow, Hv_gc, rects, np);
    Hv_SetForeground(old_fg);
}

/**
 * Hv_XMFrameRectangle
 * @purpose X Windows specific version of Hv_FrameRectangle.
 * @see The gateway routine Hv_FrameRectangle.
 */

void Hv_XMFrameRectangle (short x,
			  short y,
			  short w,
			  short h,
			  short color) {
    
    unsigned long   old_fg;
    
    old_fg = Hv_SwapForeground(color);
    XDrawRectangle(Hv_display, Hv_mainWindow, Hv_gc, x, y, w, h);
    Hv_SetForeground(old_fg);
}

/**
 * Hv_XMDrawLine
 * @purpose X Windows specific version of Hv_DrawLine.
 * @see The gateway routine Hv_DrawLine.  
 */

void Hv_XMDrawLine(short x1,
		   short y1,
		   short x2,
		   short y2,
		   short color) {

  unsigned long   old_fg;

  if (color < 0)
    return;
    
  old_fg = Hv_SwapForeground(color);

  XDrawLine(Hv_display,
	    Hv_mainWindow,
	    Hv_gc,
	    x1, y1,
	    x2, y2);  /* draw the line */

/* revert the color to thd old foreground color */

  Hv_SetForeground(old_fg);
}


/**
 * Hv_XMSimpleDrawString
 * @purpose X Windows specific version of Hv_SimpleDrawString.
 * @see The gateway routine Hv_SimpleDrawString.  
 */


void    Hv_XMSimpleDrawString(short x,
			      short y,
			    Hv_String *str) {


    Hv_CompoundString cs;			/* Motif compound string */
    short             sw, sh;		        /* String width  in pixels */
    unsigned long     oldfg;
    
    Hv_CompoundStringDimension(str, &sw, &sh);
    cs = Hv_CreateAndSizeCompoundString(str->text, str->font, &sw, &sh);
    
    oldfg = Hv_SwapForeground(str->strcol);
    
    if (Hv_useDrawImage)
	Hv_CompoundStringDrawImage(cs, x, y, sw);
    else
	Hv_CompoundStringDraw(cs, x, y, sw);
    
    Hv_CompoundStringFree(cs);
    Hv_SetForeground(oldfg);
}

/**
 * Hv_XMDrawString    draws a string at a specified location. The
 * x and y parameters give the TOP-LEFT which is unconventional
 * (sorry about that) so be careful
 * @param   x           pixel location of left edge
 * @param   y           pixel location of top edge
 * @param   str         Hv_String to be drawn
 * @param   region      a clipping region (safe to pass NULL) 
 */

void    Hv_XMDrawString(short x,
			short y,
			Hv_String *str,
			Hv_Region region) {

  XmString	    xmst;		   /* Motif compound string */
  short             xmw, xmh;		   /* String width  in pixels */
  unsigned long     oldfg;

  Hv_Pixmap	    pmap1, pmap2;
  XImage           *image1, *image2;
  Hv_Rect           area, parea;
  Window            twin;
  int               i, j;
  short	            desty;		   /* destination vert pixel */
    
    if ((str == NULL) || (str->text == NULL))
	return;
    
    oldfg = Hv_SwapForeground(str->strcol);

    xmst = Hv_CreateAndSizeMotifString(str->text, str->font, &xmw, &xmh);

    
    if (str->horizontal) {
	
	if (Hv_useDrawImage)
	    XmStringDrawImage(Hv_display,
			      Hv_mainWindow,
			      Hv_fontList,
			      xmst,
			      Hv_gc,
			      x,
			      y,
			      xmw,
			      XmALIGNMENT_BEGINNING,
			      XmALIGNMENT_BEGINNING,
			      NULL);
	else
	    XmStringDraw(Hv_display,
			 Hv_mainWindow,
			 Hv_fontList,
			 xmst,
			 Hv_gc,
			 x,
			 y,
			 xmw,
			 XmALIGNMENT_BEGINNING,
			 XmALIGNMENT_BEGINNING,
			 NULL);
	
    } /* end horizontal */
    else {  /* vertical, non postscript */
	pmap1 = Hv_CreatePixmap((int)xmw, (int)xmh);  /* horizontal */
	pmap2 = Hv_CreatePixmap((int)xmh, (int)xmw);  /* vertical */
	twin = Hv_mainWindow;
	Hv_mainWindow = pmap1;
	Hv_SetRect(&area, x, y, xmw+1, xmh+1);
	Hv_SetRect(&parea, 0, 0, xmw+1, xmh+1);
	Hv_ClipRect(&parea);
	
	if (str->fillcol >= 0)
	    Hv_FillRect(&parea, str->fillcol);
	
	str->horizontal = True;
	Hv_DrawCompoundString(1, 1, str);
	str->horizontal = False;
	Hv_mainWindow = twin;
	
	image1 = XGetImage(Hv_display,
			   (Pixmap)pmap1,
			   0,
			   0,
			   (int)xmw,
			   (int)xmh,
			   AllPlanes,
			   ZPixmap);
	
	image2 = XGetImage(Hv_display,
			   (Pixmap)pmap2,
			   0,
			   0,
			   (int)xmh,
			   (int)xmw,
			   AllPlanes,
			   ZPixmap);
	
/* now rotate text pixels from image1 onto image 2 */
	
	for (i = 1; i < (int)xmw; i++) {
	    desty = xmw-i;
	    for (j = 1; j < (int)xmh; j++)
		XPutPixel(image2, j, desty, XGetPixel(image1, i, j));
	}
	
/* Now that image2 contains the rotated text, put it onto the screen */
	
/* restore the clip region */
	
	if (region != NULL)
	  Hv_SetClippingRegion(region);
	
	if (str->fillcol >= 0)
	  Hv_FillRect(&area, str->fillcol);
	
	XPutImage(Hv_display,
		  Hv_mainWindow,
		  Hv_gc,
		  image2,
		  1,
		  1,
		  area.left,
		  area.top,
		  xmh,
		  xmw); 
	Hv_Flush();
	
/* free memory used by images and pixmaps */
	
	Hv_FreePixmap(pmap2);
	Hv_DestroyImage(image2);
	Hv_DestroyImage(image1);
	Hv_FreePixmap(pmap1);
	
    }
    
    XmStringFree(xmst);
    Hv_SetForeground(oldfg);
}

/*==========================================
 * UTILITY SECTION OF GATEWAY
 *===========================================*/

/**
 * Hv_XMGetXEventX
 * @purpose X Windows specific version of Hv_GetXEventX.
 * @see The gateway routine Hv_GetXEventX.
 */

short Hv_XMGetXEventX(Hv_XEvent *event) {
    return event->xbutton.x;
}


/**
 * Hv_XMGetXEventY
 * @purpose X Windows specific version of Hv_GetXEventY.
 * @see The gateway routine Hv_GetXEventY.
 */

short Hv_XMGetXEventY(Hv_XEvent *event) {
    return event->xbutton.y;
}


/**
 * Hv_XMGetXEventButton
 * @purpose X Windows specific version of Hv_GetXEventButton.
 * @see The gateway routine Hv_GetXEventButton.
 */

short Hv_XMGetXEventButton(Hv_XEvent *event) {
    return event->xbutton.button;
}


/**
 * Hv_XMAddModifier
 * @purpose X Windows specific version of Hv_AddModifier.
 * @see The gateway routine Hv_AddModifier.
 */

void Hv_XMAddModifier(Hv_XEvent  *event,
		      int         modbit) {

    switch (modbit) {
    case Hv_SHIFT:
	event->xbutton.state |= ShiftMask;
	break;

    case Hv_MOD1:
	event->xbutton.state |= Mod1Mask;
	break;

    case Hv_CONTROL:
	event->xbutton.state |= ControlMask;
	break;

    }
}


/**
 * Hv_XMShifted
 * @purpose X Windows specific version of Hv_Shifted.
 * @see The gateway routine Hv_Shifted.
 */

Boolean Hv_XMShifted(Hv_XEvent  *event) {
    return ((event->xbutton.state & ShiftMask) == ShiftMask);
}

/**
 * Hv_XMAltPressed
 * @purpose X Windows specific version of Hv_AltPressed.
 * @see The gateway routine Hv_AltPressed.
 */

Boolean Hv_XMAltPressed(Hv_XEvent  *event) {
    return ((event->xbutton.state & Mod1Mask) == Mod1Mask);
}

/**
 * Hv_XMControlPressed
 * @purpose X Windows specific version of Hv_ControlPressed.
 * @see The gateway routine Hv_ControlPressed.
 */

Boolean Hv_XMControlPressed(Hv_XEvent  *event) {
    return ((event->xbutton.state & ControlMask) == ControlMask);
}

/**
 * Hv_XMPause
 * @purpose X Windows specific version of Hv_Pause.
 * @see The gateway routine Hv_Pause.  
 */

 void  Hv_XMPause(int       interval,
		  Hv_Region region) {

   Hv_IntervalId         waitid;


/*
 * Algorithm 
 *
 *  1) Set Hv_pauseFlag to True
 *  2) Set A Timeout for given interval
 *  3) Event loop blocking all but timeouts
 *  Note: only timeout set in step 2 will reset
 *  Hv_pauseFlag to false.
 */

     Hv_Flush();

     Hv_pauseFlag = True;

     waitid = XtAppAddTimeOut(Hv_appContext,
			      (unsigned long)(interval), 
			      (XtTimerCallbackProc)Hv_PauseTimeOut,
			      NULL); 

     while (Hv_pauseFlag)
       XtAppProcessEvent(Hv_appContext, XtIMTimer);

     Hv_SetClippingRegion(region);
 }

/**
 * Hv_XMSysBeep
 * @purpose X Windows specific version of Hv_SysBeep.
 * @see The gateway routine Hv_SysBeep.  
 */

void  Hv_XMSysBeep(void) {
    Display    *dpy = XtDisplay(Hv_toplevel);
    
    XBell(dpy, 50);
}

/**
 * Hv_XMFlush
 * @purpose X Windows specific version of Hv_Flush.
 * @see The gateway routine Hv_Flush.  
 */

 void  Hv_XMFlush(void) {
     XFlush(Hv_display);
 }


/**
 * Hv_XMSync
 * @purpose X Windows specific version of Hv_Sync.
 * @see The gateway routine Hv_Sync.  
 */

 void  Hv_XMSync(void) {
     XSync(Hv_display, False);
 }


/**
 * Hv_XMSetCursor
 * @purpose X Windows specific version of Hv_SetCursor.
 * @see The gateway routine Hv_SetCursor.  
 */


 void  Hv_XMSetCursor(Hv_Cursor curs) {
   XDefineCursor(Hv_display, Hv_trueMainWindow, curs);
 }


/**
 * Hv_XMSetString
 * @purpose X Windows specific version of Hv_SetString.
 * @see The gateway routine Hv_SetString.  
 */

void Hv_XMSetString(Hv_Widget w,
		  char      *s) {
  XmTextSetString(w, s);
}


/**
 * Hv_XMGetString
 * @purpose X Windows specific version of Hv_GetString.
 * @see The gateway routine Hv_GetString.  
 */

char  *Hv_XMGetString(Hv_Widget w) {
  char  *tempstr;
  char  *s;

  tempstr = XmTextGetString(w);
  Hv_InitCharStr(&s, tempstr);
  XtFree(tempstr);
  return s;
}


/**
 * Hv_XMParent
 * @purpose X Windows specific version of Hv_Parent.
 * @see The gateway routine Hv_Parent.  
 */

Hv_Widget  Hv_XMParent(Hv_Widget w) {
    return XtParent(w);
}



/**
 * Hv_XMRemoveAllCallbacks
 * @purpose X Windows specific version of Hv_RemoveAllCallbacks.
 * @see The gateway routine Hv_RemoveAllCallbacks.  
 */

void  Hv_XMRemoveAllCallbacks(Hv_Widget w,
			      char *name) {
    XtRemoveAllCallbacks(w, name);
}


/**
 * Hv_XMManangeChild
 * @purpose X Windows specific version of Hv_ManangeChild.
 * @see The gateway routine Hv_ManangeChild.  
 */

void  Hv_XMManageChild(Hv_Widget w) {
    XtManageChild(w);
}


/**
 * Hv_XMUnmanangeChild
 * @purpose X Windows specific version of Hv_UnmanangeChild.
 * @see The gateway routine Hv_UnmanangeChild.  
 */

void  Hv_XMUnmanageChild(Hv_Widget w) {
    XtUnmanageChild(w);
}


/**
 * Hv_XMAddCallback
 * @purpose X Windows specific version of Hv_AddCallback.
 * @see The gateway routine Hv_AddCallback.  
 */

void  Hv_XMAddCallback(Hv_Widget       w,
		     char           *cbname,
		     Hv_CallbackProc cbproc,
		     Hv_Pointer      data) {
    XtAddCallback(w, cbname, cbproc, data);
}

/**
 * Hv_XMDestroyWidget
 * @purpose X Windows specific version of Hv_DestroyWidget.
 * @see The gateway routine Hv_DestroyWidget.  
 */


void  Hv_XMDestroyWidget(Hv_Widget w) {
  if (w == NULL)
    return;
  
  XtDestroyWidget(w);
}


/**
 * Hv_XMSetSensitivity
 * @purpose X Windows specific version of Hv_SetSensitivity.
 * @see The gateway routine Hv_SetSensitivity.  
 */

void Hv_XMSetSensitivity(Hv_Widget  w,
			 Boolean   val) {
   if (w == NULL)
     return;
   XtSetSensitive(w, val);
}


/**
 * Hv_XMIsSensitive
 * @purpose X Windows specific version of Hv_IsSensitive.
 * @see The gateway routine Hv_IsSensitive.  
 */

Boolean Hv_XMIsSensitive(Hv_Widget  w) {
    return XtIsSensitive(w);
}


/**
 * Hv_XMSetWidgetBackgroundColor
 * @purpose X Windows specific version of Hv_SetWidgetBackgroundColor.
 * @see The gateway routine Hv_SetWidgetBackgroundColor.  
 */


void Hv_XMSetWidgetBackgroundColor(Hv_Widget   w,
				   short       color)  {
   unsigned long dbg;
   
   if (w == NULL)
     return;
   
   if (color >= 0)
     XtVaSetValues(w, Hv_Nbackground, Hv_colors[color], NULL);
   else {
     XtVaGetValues(XtParent(w), Hv_Nbackground, &dbg, NULL);
     XtVaSetValues(w, Hv_Nbackground, dbg, NULL);
   }
   
}


/**
 * Hv_XMSetWidgetForegroundColor
 * @purpose X Windows specific version of Hv_SetWidgetForegroundColor.
 * @see The gateway routine Hv_SetWidgetForegroundColor.  
 */


void Hv_XMSetWidgetForegroundColor(Hv_Widget   w,
				   short       color) {
  unsigned long dbg;
  
  if (w == NULL)
    return;
  
  if (color >= 0)
    XtVaSetValues(w, Hv_Nforeground, Hv_colors[color], NULL);
  else {
    XtVaGetValues(XtParent(w), Hv_Nforeground, &dbg, NULL);
    XtVaSetValues(w, Hv_Nforeground, dbg, NULL);
  }
  
}


/**
 * Hv_XMClickType
 * @purpose X Windows specific version of Hv_ClickType.
 * @see The gateway routine Hv_ClickType.  
 */


int Hv_XMClickType(Hv_Item Item,
		   short  button) {

  int       click = 0;
  XEvent    testevent;
    
  Hv_Pause(XtGetMultiClickTime(Hv_display), NULL);
    
  if (Hv_CheckMaskEvent(Hv_BUTTONPRESSMASK, &testevent)) {
    if (Hv_GetXEventButton(&testevent) == button) 
      return 2;  /* double click */
  }

  if ((Item != NULL) && (Item->domain == Hv_OUTSIDEHOTRECT))
    return 1;
    
  if (Hv_CheckMaskEvent(Hv_BUTTONRELEASEMASK, &testevent)) {
    if (Hv_GetXEventButton(&testevent) == button)
      return 1;  /* single click */
  }

  return click;
}


/**
 * Hv_XMPrintln
 * @purpose X Windows specific version of Hv_Println.
 * @see The gateway routine Hv_Println.  
 */


void    Hv_XMPrintln(char   *fmt,
		     va_list args) {
    
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}



/*==========================================
 * COLOR RELATED SECTION OF GATEWAY
 *===========================================*/




/**
 * Hv_XMMakeColorMap
 * @purpose X Windows specific version of Hv_MakeColorMap.
 * @see The gateway routine Hv_MakeColorMap.  
 */


void Hv_XMMakeColorMap(void) {

  int		i, j;
  Colormap      defcmap;
  Display	*dpy = XtDisplay(Hv_toplevel);		/* display ID */
  int		scr = DefaultScreen(dpy);	/* screen ID */
  unsigned int  g;

  Visual       *defvis;
  int          defdepth;
  Boolean      foundvis;


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
    Hv_Println("Hv Warning: Unknown visual type. Will try to continue.");
    Hv_visClass = PseudoColor;
  }
  
  Hv_readOnlyColors = 
    ((Hv_visClass == StaticGray) ||
     (Hv_visClass == StaticColor) ||
     (Hv_visClass == TrueColor));

  Hv_CheckColorMap();


  for (j = 0; j < 15; j++) {
    g = 220 - j*14;
    Hv_StuffColor(NULL, g, g, g, Hv_gray15 + j, True);
  }


  Hv_StuffColor(NULL, 255, 0, 0,     Hv_red, True);
  Hv_StuffColor(NULL, 255, 128, 0,   Hv_orange, True);
  Hv_StuffColor(NULL, 255, 255, 0,   Hv_yellow, True);
  Hv_StuffColor(NULL, 147, 73, 0,   Hv_brown, True);
  Hv_StuffColor(NULL, 241, 191, 116, Hv_wheat, True);
  Hv_StuffColor(NULL, 0,   100, 0,   Hv_forestGreen, True);
  Hv_StuffColor(NULL, 64, 219, 64,   Hv_green, True);  
  Hv_StuffColor(NULL, 255, 255, 255, Hv_white, True);
  Hv_StuffColor(NULL, 0, 0, 0,       Hv_black, True);
  Hv_StuffColor(NULL, 160, 32, 240,  Hv_purple, True);
  Hv_StuffColor(NULL, 238, 130, 238, Hv_violet, True);
  Hv_StuffColor(NULL, 216, 216, 255, Hv_aliceBlue, True);
  Hv_StuffColor(NULL, 150, 195, 200, Hv_honeydew, True);
  Hv_StuffColor(NULL, 125, 196, 215, Hv_skyBlue, True);
  Hv_StuffColor(NULL, 100, 116, 215, Hv_cornFlowerBlue, True);
  Hv_StuffColor(NULL, 0, 0, 255,     Hv_blue, True);
  Hv_StuffColor(NULL, 0, 0, 64,      Hv_navyBlue, True);
  Hv_StuffColor(NULL, 70, 235, 150,  Hv_aquaMarine, True);
  Hv_StuffColor(NULL, 83, 142, 87,   Hv_lightSeaGreen, True); /* used to hold bg color*/

  Hv_canvasColor = Hv_navyBlue+10;   /* slot for changing the background*/

/* Fill in the gaps */

  Hv_FillGaps(Hv_red,            Hv_orange, -1, -1);
  Hv_FillGaps(Hv_orange,         Hv_yellow, -1, -1);
  Hv_FillGaps(Hv_yellow,         Hv_green,  -1, -1);
  Hv_FillGaps(Hv_green,          Hv_forestGreen,  -1, -1);
  Hv_FillGaps(Hv_forestGreen,    Hv_wheat,  -1, -1);
  Hv_FillGaps(Hv_wheat,          Hv_brown,  -1, -1);
  Hv_FillGaps(Hv_brown,          Hv_white,  -1, -1);
  Hv_FillGaps(Hv_black,          Hv_purple, -1, -1);
  Hv_FillGaps(Hv_purple,         Hv_violet, -1, -1);
  Hv_FillGaps(Hv_violet,         Hv_aliceBlue, -1, -1);
  Hv_FillGaps(Hv_aliceBlue,      Hv_honeydew, -1, -1);
  Hv_FillGaps(Hv_honeydew,       Hv_skyBlue, -1, -1);
  Hv_FillGaps(Hv_skyBlue,        Hv_cornFlowerBlue, -1, -1);
  Hv_FillGaps(Hv_cornFlowerBlue, Hv_blue, -1, -1);
  Hv_FillGaps(Hv_blue,           Hv_navyBlue, -1, -1);
  Hv_FillGaps(Hv_navyBlue,       Hv_aquaMarine, -1, -1);

/* defaults for simple text editing */


  Hv_ColorizeCursors(dpy);
}


/**
 * Hv_XMGetForeground
 * @purpose X Windows specific version of Hv_GetForeground.
 * @see The gateway routine Hv_GetForeground.  
 */

unsigned long  Hv_XMGetForeground(void) {

  XGCValues  values;
  
  XGetGCValues (Hv_display, Hv_gc, GCForeground, &values);
  return(values.foreground);
}

/**
 * Hv_XMGetBackground
 * @purpose X Windows specific version of Hv_GetBackground.
 * @see The gateway routine Hv_GetBackground.  
 */

unsigned long  Hv_XMGetBackground(void) {

  XGCValues  values;
  
  XGetGCValues (Hv_display, Hv_gc, GCBackground, &values);
  return(values.background);
}


/**
 * Hv_XMNearestColor
 * @purpose X Windows specific version of Hv_NearestColor.
 * @see The gateway routine Hv_NearestColor.  
 */

unsigned long   Hv_XMNearestColor(unsigned short r,
				  unsigned short g,
				  unsigned short b) {
  int status = 0;
  Display	*dpy = XtDisplay(Hv_toplevel);
  unsigned long pixel;
  long          diff, diff2;
  int           i;

  Hv_Color   xcolor;

  xcolor.red = r;
  xcolor.blue = b;
  xcolor.green = g;
  xcolor.flags = DoRed | DoGreen | DoBlue;

  status = XAllocColor(dpy, Hv_colorMap, &xcolor);

  if (status != 0) 
    return xcolor.pixel;

  pixel = Hv_colors[0];
  xcolor.pixel = Hv_colors[0];
  Hv_QueryColor(Hv_display, Hv_colorMap, &xcolor);
  diff = abs(xcolor.red-r) + abs(xcolor.green-g) + abs(xcolor.blue-b);
  
  for (i = 1; i < Hv_numColors; i++) {
    xcolor.pixel = Hv_colors[i];
    Hv_QueryColor(Hv_display, Hv_colorMap, &xcolor);
    diff2 = abs(xcolor.red-r) + abs(xcolor.green-g) + abs(xcolor.blue-b);
    if (diff2 < diff) {
      diff = diff2;
      pixel = xcolor.pixel;
    }
    
  }

  return pixel;
}


/**
 * Hv_XMDarkenColor
 * @purpose X Windows specific version of Hv_DarkenColor.
 * @see The gateway routine Hv_DarkenColor.  
 */

void            Hv_XMDarkenColor(short color,
				 unsigned short del) {
  Hv_Color   xcolor;

  xcolor.pixel = Hv_colors[color];
  Hv_QueryColor(Hv_display, Hv_colorMap, &xcolor);

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

/**
 * Hv_XMBrightenColor
 * @purpose X Windows specific version of Hv_BrightenColor.
 * @see The gateway routine Hv_BrightenColor.  
 */

void            Hv_XMBrightenColor(short color,
				   unsigned short del) {
  Hv_Color           xcolor;
  unsigned short   limval;

  limval = 65535 - del;

  xcolor.pixel = Hv_colors[color];
  Hv_QueryColor(Hv_display, Hv_colorMap, &xcolor);
  
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

/**
 * Hv_XMAdjustColor
 * @purpose X Windows specific version of Hv_AdjustColor.
 * @see The gateway routine Hv_AdjustColor.  
 */

void            Hv_XMAdjustColor(short color,
				 short r,
				 short g,
				 short b) {
    Hv_Color   xcolor;
    xcolor.pixel = Hv_colors[color];
    Hv_QueryColor(Hv_display, Hv_colorMap, &xcolor);
    
    xcolor.red += r;
    xcolor.green += g;
    xcolor.blue += b;
    
    if (XAllocColor(Hv_display, Hv_colorMap, &xcolor))
	Hv_colors[color] = xcolor.pixel;
}


/**
 * Hv_XMChangeColor
 * @purpose X Windows specific version of Hv_ChangeColor.
 * @see The gateway routine Hv_ChangeColor.  
 */

void            Hv_XMChangeColor(short color,
				 unsigned short r,
				 unsigned short g,
				 unsigned short b) {
  Hv_Color   xcolor;

  xcolor.pixel = Hv_colors[color];
  Hv_QueryColor(Hv_display, Hv_colorMap, &xcolor);
  
  xcolor.red = r;
  xcolor.green = g;
  xcolor.blue = b;
  
  if (XAllocColor(Hv_display, Hv_colorMap, &xcolor))
    Hv_colors[color] = xcolor.pixel;
}


/**
 * Hv_XMCopyColor
 * @purpose X Windows specific version of Hv_CopyColor.
 * @see The gateway routine Hv_CopyColor.  
 */

void  Hv_XMCopyColor(short dcolor,
		     short scolor) {
    Hv_Color   dxcolor, sxcolor;

/* if read only, the opoeration is exceedingly simple */

  if (Hv_readOnlyColors) {
    Hv_colors[dcolor] = Hv_colors[scolor];
    return;
  }

  sxcolor.pixel = Hv_colors[scolor];
  Hv_QueryColor(Hv_display, Hv_colorMap, &sxcolor);
  
  dxcolor.pixel = Hv_colors[dcolor];
  dxcolor.red   = sxcolor.red;
  dxcolor.green = sxcolor.green;
  dxcolor.blue  = sxcolor.blue;
  dxcolor.flags = sxcolor.flags;
  
  XStoreColor(Hv_display, Hv_colorMap, &dxcolor);
  
}

/**
 * Hv_XMGetRGBFromColor
 * @purpose X Windows specific version of Hv_GetRGBFromColor.
 * @see The gateway routine Hv_GetRGBFromColor.
 */

void Hv_XMGetRGBFromColor(short color,
					  	  unsigned short *r,
						  unsigned short *g,
						  unsigned short *b) {

Hv_Color cdef;
 	
 
  if (color == Hv_black) {
    *r = 0; *g = 0; *b = 0;
  }
  else if (color == Hv_white) {
    *r = 65535; *g = 65535; *b = 65535;
  }
  else if (color == Hv_yellow) {
    *r = 65535; *g = 65535; *b = 0;
  }
  else if (color == Hv_red) {
    *r = 65535; *g = 0; *b = 0;
  }
  else {

    cdef.pixel = Hv_colors[color];

    XQueryColor(Hv_display, Hv_colorMap, &cdef);  /* get the color information */

	*r = cdef.red;
	*g = cdef.green;
	*b = cdef.blue;
  }
}

/**
 * Hv_XMQueryColor
 * @purpose X Windows specific version of Hv_QueryColor.
 * @see The gateway routine Hv_QueryColor.  
 */

void  Hv_XMQueryColor(Hv_Display   *dpy,
		      Hv_ColorMap   cmap,
		      Hv_Color     *xcolor) {
    XQueryColor(dpy, cmap, xcolor);
}



/*==========================================
 * FILESELECTION RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_XMDoubleFileSelect
 * @purpose X Windows specific version of Hv_DoubleFileSelect.
 * @author Dat Cao of Sparta, Inc
 * @see The gateway routine Hv_DoubleFileSelect.
 */

void Hv_XMDoubleFileSelect(char *title,
			   char *prompt1,
			   char *prompt2,
			   char *mask1, 
			   char *mask2, 
			   char **fname1, 
			   char **fname2) {

     Hv_Widget           dialog;


     Hv_Widget        dummy, rowcol, rc, rc1, rc2;
     int              reason;
     Hv_Widget        sb1;
     Hv_Widget        sb2;
     Widget           text1, text2;
     char             *temptext1, *temptext2;
     XmString         xmst;

/* create dialog */
 

    Hv_VaCreateDialog(&dialog,
		      Hv_TITLE, (title == NULL) ? " Double File Selection " : title, 
		      NULL);

    rowcol = Hv_DialogColumn(dialog, 6);

    rc = Hv_DialogRow(rowcol, 15);
    rc1 = Hv_DialogColumn(rc, 4);
    rc2 = Hv_DialogColumn(rc, 4);

/* add prompts */

    dummy = Hv_StandardLabel(rc1, prompt1, 0);
    dummy = Hv_StandardLabel(rc2, prompt2, 0);

/* add file selectors */

    sb1 = Hv_StandardFileSelectorItem(rc1, 325, mask1); 
    sb2 = Hv_StandardFileSelectorItem(rc2, 325, mask2); 

/* grab the text areas of the file selectors */

    text1 = XmFileSelectionBoxGetChild(sb1, XmDIALOG_TEXT);    
    text2 = XmFileSelectionBoxGetChild(sb2, XmDIALOG_TEXT);    

/* add the action buttons -- note use of StandardActionArea*/

    Hv_StandardActionButtons(rowcol,20, Hv_OKBUTTON + Hv_CANCELBUTTON);


/* cached first fs dir */

    if (Hv_cachedDirectory1 != NULL) {
	xmst =  Hv_CreateSimpleCompoundString(Hv_cachedDirectory1);
	XtVaSetValues(sb1, 
		      Hv_Ndirectory, xmst, 
		      NULL);
	Hv_CompoundStringFree(xmst);
    }

/* cached 2nd fs dir */

    if (Hv_cachedDirectory2 != NULL) {
	xmst =  Hv_CreateSimpleCompoundString(Hv_cachedDirectory2);
	XtVaSetValues(sb2, 
		      Hv_Ndirectory, xmst, 
		      NULL);
	Hv_CompoundStringFree(xmst);
    }

/* now process the dialog */

    reason  = Hv_DoDialog(dialog, NULL);

/* if hit OK, get the file names */

    if (reason == Hv_OK) {
	temptext1 = XmTextGetString(text1);
	temptext2 = XmTextGetString(text2);

	Hv_InitCharStr(fname1, temptext1);
	Hv_InitCharStr(fname2, temptext2);

	XtFree(temptext1);
	XtFree(temptext2);

    }
    else {
	*fname1 = NULL;
	*fname2 = NULL;
    }


    Hv_Free(Hv_cachedDirectory1);
    XtVaGetValues(sb1, Hv_Ndirectory, &xmst, NULL);
    Hv_cachedDirectory1 = Hv_TextFromXmString(xmst);
    Hv_CompoundStringFree(xmst);

    Hv_Free(Hv_cachedDirectory2);
    XtVaGetValues(sb2, Hv_Ndirectory, &xmst, NULL);
    Hv_cachedDirectory2 = Hv_TextFromXmString(xmst);
    Hv_CompoundStringFree(xmst);

    Hv_DestroyWidget(dialog);

}

/**
 * Hv_XMFileSelect
 * @purpose X Windows specific version of Hv_FileSelect.
 * @see The gateway routine Hv_FileSelect.
 */

char *Hv_XMFileSelect(char *prompt,
		      char *mask,
		      char *deftext) {
    char *fname;

    fname =  Hv_XMGenericFileSelect(prompt,
				    mask,
				    XmFILE_REGULAR,
				    deftext,
				    Hv_toplevel);
    return fname;
}

/**
 * Hv_XMDirectorySelect
 * @purpose X Windows specific version of Hv_DirectorySelect.
 * @see The gateway routine Hv_DirectorySelect.
 */

char *Hv_XMDirectorySelect(char *prompt,
			   char *mask) {
  return Hv_XMGenericFileSelect(prompt, mask, XmFILE_DIRECTORY, NULL, Hv_toplevel);
}

/**
 * Hv_XMGenericFileSelect
 * @purpose X Windows specific version of Hv_GenericFileSelect.
 * @see The gateway routine Hv_GenericFileSelect.
 */

char *Hv_XMGenericFileSelect(char         *prompt,
			     char         *mask,
			     unsigned char filemask,
			     char          *deftext,
			     Hv_Widget     parent) {
/* A generic file selection dlog -- returns
   the name of the file selected or NULL
   if cancelled
   
   deftext: default selection */


    Hv_Widget        dialog = NULL;
    char            *fname = NULL;
    XmString         xmst, title;
    XmString         defstr = NULL;
    Hv_Widget        rowcol = NULL;
    Hv_Widget        label = NULL;
    Hv_Widget        sep1 = NULL;
    Hv_Widget        sep2 = NULL;
    Hv_Widget        fileList = NULL;
    static Boolean   fsisup = False;
    XmString         dirxmst;
    char            *dtextpdir;

    if (fsisup) {
	Hv_SysBeep();
	return NULL;
    }
  
    fsisup = True;
    
    dialog = XmCreateFileSelectionDialog(parent, "filesb", NULL, 0);
    title =  Hv_CreateSimpleCompoundString("file selection");
    
    XtVaSetValues(dialog,
		  Hv_NdialogTitle,  title,
		  NULL);
    
    Hv_CompoundStringFree(title);      
    
    Hv_AddCallback(dialog,
		   Hv_okCallback,
		   (Hv_CallbackProc)Hv_FileSelectResponse,
		   NULL);
    
    Hv_AddCallback(dialog,
		   Hv_cancelCallback,
		   (Hv_CallbackProc)Hv_FileSelectResponse,
		   NULL);
    

/* the dialog has been created. set the mask.
   If a prompt was provide, create some additional widgets*/
    
    if (mask)
	xmst =  Hv_CreateSimpleCompoundString(mask);
    else
	xmst =  Hv_CreateSimpleCompoundString("*");



    XtVaSetValues(dialog,
		  Hv_NfileTypeMask, filemask,
		  Hv_Npattern,      xmst,      /* thanks Jack */
		  NULL);  
    Hv_CompoundStringFree(xmst);

/* we have cached the directory */

    if (Hv_cachedDirectory != NULL) {
	xmst =  Hv_CreateSimpleCompoundString(Hv_cachedDirectory);
	XtVaSetValues(dialog, Hv_Ndirectory, xmst, NULL);
	Hv_CompoundStringFree(xmst);
  }

    if (deftext != NULL) {

/* prepend directory? */

	if ((Hv_cachedDirectory != NULL) && (deftext[0] != Hv_FileSeparator)) {
	    dtextpdir = (char *)Hv_Malloc(strlen(Hv_cachedDirectory) + strlen(deftext) + 5);
	    strcpy(dtextpdir, Hv_cachedDirectory);
	    strcat(dtextpdir, deftext);
    }
	else
	    Hv_InitCharStr(&dtextpdir, deftext);
	
	defstr = Hv_CreateSimpleCompoundString(dtextpdir);
	XtVaSetValues(dialog,
		      Hv_NtextString,   defstr,
		      NULL);  
	Hv_CompoundStringFree(defstr);
	Hv_Free(dtextpdir);
    }


/* set up the user provided prompt */

    if (prompt) {
	rowcol = Hv_VaCreateDialogItem(dialog,
				       Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
				       Hv_ORIENTATION,  Hv_VERTICAL,
				       Hv_ALIGNMENT,    Hv_CENTER,
				       NULL);
	
	sep1 = Hv_SimpleDialogSeparator(rowcol);
	
	label = Hv_VaCreateDialogItem(rowcol,
				      Hv_TYPE,     Hv_LABELDIALOGITEM,
				      Hv_LABEL,    prompt,
				      Hv_FONT,     Hv_helvetica14,
				      NULL);
	
	sep2 = Hv_SimpleDialogSeparator(rowcol);
    }


/* deselect all files */

    fileList = XmFileSelectionBoxGetChild(dialog, XmDIALOG_LIST);
    XmListDeselectAllItems(fileList);
    
    if (Hv_DoDialog(dialog, NULL) == Hv_OK) {
	Hv_InitCharStr(&fname, generic_filename);
	Hv_Free(generic_filename);
	generic_filename = NULL;
    }

/* destroy the widgets that were created due to a prompt*/

    if (prompt) {
	Hv_DestroyWidget(label);
	Hv_DestroyWidget(sep1);
	Hv_DestroyWidget(sep2);
	Hv_DestroyWidget(rowcol);
    }


    Hv_Free(Hv_cachedDirectory);
    XtVaGetValues(dialog, Hv_Ndirectory, &dirxmst, NULL);
    Hv_cachedDirectory = Hv_TextFromXmString(dirxmst);
    
    Hv_CompoundStringFree(dirxmst);
    
    fsisup = False;
    Hv_DestroyWidget(dialog);
    return fname;

}


/*==========================================
 * FONT RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_XMInitFonts
 * @purpose X Windows specific version of Hv_InitFonts.
 * @see The gateway routine Hv_InitFonts.
 */

void Hv_XMInitFonts(void) {
  fontfixed  = Hv_AddFont(Hv_fixed);
  fontfixed2 = Hv_AddFont(Hv_fixed2);
}


/**
 * Hv_XMAddNewFont
 * @purpose X Windows specific version of Hv_AddNewFont.
 * @see The gateway routine Hv_AddNewFont.
 */

void Hv_XMAddNewFont(short font) {
  Hv_Font   *dummy;

  if ((font < 0) || (font >= Hv_NUMFONTS))
    return;
  
  dummy = Hv_AddFont(font);
}


/*==============================================
 * TEXTENTRY WIDGET  RELATED SECTION OF GATEWAY
 *==============================================*/

/**
 * Hv_XMTextEntryItemInitialize
 * @purpose X Windows specific version of Hv_TextEntryItemInitialize.
 * @see The gateway routine Hv_TextEntryItemInitialize.
 */

void Hv_XMTextEntryItemInitialize(Hv_Item           Item,
				  Hv_AttributeArray attributes) {
  int         editmode;
  Hv_Widget   w;

  static char  transtable[] =
    "<Key>osfBackSpace: delete-previous-character()";
  
  static char  trans2table[] =
    "<Key>osfDelete: delete-previous-character()";


/* first initialize as a normal text item*/

  Hv_TextItemInitialize (Item, attributes) ;

/* override the text item double click */

  Item->doubleclick = (Hv_FunctionPtr)(attributes[Hv_DOUBLECLICK].fp);

  if (attributes[Hv_SINGLECLICK].v == NULL)
    Item->singleclick = Hv_TextEntrySingleClick;
  

/* allocate the tag along motif widget */

  editmode = XmSINGLE_LINE_EDIT; 
  if (attributes[Hv_TEXTENTRYMODE].s == Hv_MULTILINEEDIT)
    editmode = XmMULTI_LINE_EDIT; 

  w = XtVaCreateWidget("textfield", 
		       xmTextWidgetClass,      Hv_toplevel,
		       Hv_NshadowThickness,     0,
		       Hv_Nwidth,               Item->area->width,
		       Hv_Nheight,              Item->area->height,
		       Hv_NhighlightThickness,  0,
		       Hv_NeditMode,            editmode,
		       NULL) ;

  Item->data = (void *)w;
  
/* value changed callback ? If so
   add one with the Hv_Item as the data*/


  if ((attributes[Hv_VALUECHANGED].fp) != NULL)
    Hv_AddCallback(w,
		   Hv_valueChangedCallback,
		   (Hv_CallbackProc)(attributes[Hv_VALUECHANGED].fp),
		   (Hv_Pointer)Item);


/*
 *  The next two translation overrides were added 5/26/96
 *  to try to fix the wrongway delete on all platforms
 */

  XtOverrideTranslations(w, XtParseTranslationTable(transtable));
  XtOverrideTranslations(w, XtParseTranslationTable(trans2table));

  Item->standarddraw = Hv_DrawTextEntryItem;
  Hv_DefaultSchemeIn(&(Item->scheme));

  if (attributes[Hv_FILLCOLOR].s != Hv_DEFAULT)
    Item->scheme->fillcolor = attributes[Hv_FILLCOLOR].s;
  else
    Item->scheme->fillcolor -= 1;
}


/**
 * Hv_XMTextEntrySingleClick
 * @purpose X Windows specific version of Hv_TextEntrySingleClick.
 * @see The gateway routine Hv_TextEntrySingleClick.
 */


void Hv_XMTextEntrySingleClick(Hv_Event hvevent) {
  Hv_Item   Item = hvevent->item;
  Hv_Rect area;
  char *t ;
  Position   cx, cy, left, top;
  XmTextPosition   textpos;

  if (Item->str == NULL)
    return;

  themtw = (Widget)(Item->data);

/* if text is NULL go ahead an give them a string */

  if (Item->str->text == NULL) {
    Hv_InitCharStr(&(Item->str->text), " ");
    *(Item->str->text) = '\0';
  }

  Hv_activeTextItem = Item ;
  Hv_DanglingTextItemCallback = Hv_TEMouseDestroy ;
  Hv_CopyRect (&area, Hv_GetItemArea (Item)) ;
  t = Hv_GetItemText (Item) ;

  left = area.left+2;
  top  = area.top+4; 


  xfont  = XLoadQueryFont(Hv_display,  Hv_xFontNames[Item->str->font]);
  fontlist = XmFontListCreate(xfont, Hv_charSet[Item->str->font]);

  XtVaSetValues (themtw, Hv_Nvalue, (String)t,
		 Hv_NfontList, fontlist,
		 Hv_Nbackground, Hv_colors[Item->scheme->fillcolor],
		 Hv_Nforeground, Hv_colors[Item->scheme->darktrimcolor],
/*		 Hv_NeditMode, XmSINGLE_LINE_EDIT, */
		NULL);

  XtVaSetValues(themtw,
		Hv_Nx, left-1,
		Hv_Ny, top-1,
		Hv_Nwidth, area.width-1,
		NULL);
		
/* try to get the cursor in the proper place */

  cx = (Position)(hvevent->where.x - left);
  cy = (Position)(hvevent->where.y - top);
  textpos = XmTextXYToPos(themtw, cx, cy);

  Hv_AddCallback(themtw,
		 Hv_activateCallback,
		 (Hv_CallbackProc)Hv_TEDoReturn,
		 NULL) ;

  XmTextSetInsertionPosition(themtw, textpos);
  Hv_ManageChild(themtw);
}


/*==============================================
 * DIALOG RELATED SECTION OF GATEWAY
 *==============================================*/

/**
 * Hv_XMCloseOutCallback
 * @purpose Callback for one of the "action" buttons that closes the dialog.
 * @param   w       The button (widget)
 * @param   answer  e.g. Hv_OK or Hv_DONE.
 */

void  Hv_XMCloseOutCallback(Hv_Widget w,
			  int       answer) {

    Hv_Widget      dialog;
    Hv_DialogData  ddata;
  
    dialog = Hv_GetOwnerDialog(w);
    
    if (dialog == NULL)
	return;

/* set the answer in the dialog data */
    
    ddata = Hv_GetDialogData(dialog);
    if (ddata != NULL)
	ddata->answer = answer;
}


/* ------- Hv_DoDialog --------*/

int  Hv_XMDoDialog(Hv_Widget dialog,
		 Hv_Widget def_btn)

{
  int   answer;

  Hv_OpenDialog(dialog, def_btn);

  while ((answer = Hv_GetDialogAnswer(dialog)) == Hv_RUNNING)
    Hv_ProcessEvent(Hv_IMAll);

/* if the answer is anything but apply, close the dialog */

  if (answer != Hv_APPLY)
    Hv_CloseDialog(dialog);

  return answer;
}

/**
 * Hv_XMQuestion
 * @purpose X Windows specific version of Hv_Question.
 * @see The gateway routine Hv_Question.
 */

int  Hv_XMQuestion(char *question) {

    Hv_Widget            dialog = NULL, dummy, rowcol;
    int                  answer;

    Hv_VaCreateDialog(&dialog,
		      Hv_TITLE, "Question",
		      NULL);


    rowcol = Hv_DialogColumn(dialog, 6);

    dummy = Hv_StandardLabel(rowcol, question, 100000);


/* Hv_altCO will signal to use "Yes" "No" instead of "OK" "Cancel" */

    Hv_altCO = True;
    dummy = Hv_StandardActionButtons(rowcol,20, Hv_OKBUTTON + Hv_CANCELBUTTON);
    Hv_altCO = False;

/* the dialog has been created */

    answer = Hv_DoDialog(dialog, NULL);
    Hv_DestroyWidget(dialog);
  
    return answer;
}

/**
 * Hv_XMWarning
 * @purpose X Windows specific version of Hv_Warning.
 * @see The gateway routine Hv_Warning.
 */

void Hv_XMWarning(char *message) {

  static Hv_Widget     dialog = NULL;
  Hv_Widget            dummy, rowcol;
  static Hv_Widget     messlab;
  char                 *pn;
  char                 *mcopy;


  if (!dialog) {
    Hv_VaCreateDialog(&dialog,
		      Hv_TITLE, "Warning",
		      NULL);

    rowcol = Hv_DialogColumn(dialog, 10);

    pn = (char *)Hv_Malloc(strlen(Hv_programName) + 30);
    strcpy(pn, "     ");
    strcpy(pn, Hv_programName);
    strcat(pn, " warning!     ");

    dummy = Hv_StandardLabel(rowcol, pn, 4);
    Hv_Free(pn);
    messlab = Hv_StandardLabel(rowcol, "                      ", 0);
    dummy = Hv_SimpleDialogSeparator(rowcol);
    dummy = Hv_StandardDoneButton(rowcol, " OK ");
  }
  
  Hv_InitCharStr(&mcopy, message);
  Hv_ReplaceNewLine(mcopy);
  Hv_ChangeLabel(messlab, mcopy, Hv_fixed2);
  Hv_Free(mcopy);
  Hv_DoDialog(dialog, NULL);
}


/**
 * Hv_XMInformation
 * @purpose X Windows specific version of Hv_Information.
 * @see The gateway routine Hv_Information.
 */


void Hv_XMInformation(char *message) {

  Hv_Widget            dialog = NULL;
  Hv_Widget            dummy, rowcol;
  Hv_Widget            messlab;
  char                 *pn;
  char                 *mcopy;


  Hv_VaCreateDialog(&dialog,
		    Hv_TITLE, "Information",
		    NULL);

  rowcol = Hv_DialogColumn(dialog, 10);

  pn = (char *)Hv_Malloc(strlen(Hv_programName) + 30);
  strcpy(pn, "     ");
  strcat(pn, Hv_programName);
  strcat(pn, " information     ");

  dummy = Hv_StandardLabel(rowcol, pn, 4);
  Hv_Free(pn);
  messlab = Hv_StandardLabel(rowcol, "                      ", 0);
  dummy = Hv_SimpleDialogSeparator(rowcol);
  dummy = Hv_StandardDoneButton(rowcol, " OK ");
  
  Hv_InitCharStr(&mcopy, message);
  Hv_ReplaceNewLine(mcopy);
  Hv_ChangeLabel(messlab, mcopy, Hv_fixed2);
  Hv_Free(mcopy);
  Hv_DoDialog(dialog, NULL);
  Hv_DestroyWidget(dialog);
}




/**
 * Hv_XMCreateModalDialog
 * @purpose X Windows specific version of Hv_CreateModalDialog.
 * @see The gateway routine Hv_CreateModalDialog.
 */

Hv_Widget Hv_XMCreateModalDialog(char *title,
				 int   ignoreclose) {

    return Hv_CreateDialog(title,
			   XmDIALOG_FULL_APPLICATION_MODAL,
			   ignoreclose);
}



/**
 * Hv_XMCreateModelessDialog
 * @purpose X Windows specific version of Hv_CreateModelessDialog.
 * @see The gateway routine Hv_CreateModelessDialog.
 */

Hv_Widget Hv_XMCreateModelessDialog(char *title,
				    int   ignoreclose) {
    
    return Hv_CreateDialog(title,
			   XmDIALOG_MODELESS,
			   ignoreclose);
}



/**
 * Hv_XMEditPattern
 * @purpose X Windows specific version of Hv_EditPattern.
 * @see The gateway routine Hv_EditPattern.
 */

int   Hv_XMEditPattern(short *pattern,
		       char  *prompt) {

/* returns 0 if pattern was not changed */
     
  static Hv_Widget        dialog = NULL;
  static Hv_Widget        mynewpattern, myoldpattern;
  
  Hv_Widget            rowcol, rc, dummy;

  short                dx  = 16;  /*width  of pattern label */
  short                dy  = 16;  /*height of pattern label */
  Hv_CompoundString             title;
  int                  i, answer;

  if (!dialog) {
    Hv_VaCreateDialog(&dialog,
		      Hv_TITLE, " ",
		      NULL);
    rowcol = Hv_DialogColumn(dialog, 10); 
    rc = Hv_DialogTable(rowcol, 2, 10);
    mynewpattern = Hv_SimplePatternLabel(rc, " new pattern ", NULL);
    myoldpattern = Hv_SimplePatternLabel(rc, " old pattern ", NULL);

/* the pattern selectors */

    dummy = Hv_SimpleDialogSeparator(rowcol);

    rc = Hv_DialogTable(rowcol, 2, 0);

    for (i = 0; i < Hv_NUMPATTERNS; i++) {
      dummy = (Hv_Widget)Hv_VaCreateDialogItem(rc,
					    Hv_TYPE,        Hv_LABELDIALOGITEM,
					    Hv_LABEL,       (char *)"  ",
					    Hv_BORDERWIDTH, 1,
					    Hv_BORDERCOLOR, Hv_white,
					    Hv_FONT,        Hv_fixed2,
					    Hv_TEXTCOLOR,   Hv_black,
					    Hv_BACKGROUND,  Hv_white,
					    Hv_WIDTH,       dx,
					    Hv_HEIGHT,      dy,
					    NULL);

      XtVaSetValues(dummy, Hv_NlabelType, XmPIXMAP, NULL);

      Hv_AddEventHandler(dummy,
			 Hv_buttonPressMask,
			 (Hv_EventHandler)Hv_SetWidgetPattern,
			 (Hv_Pointer)i);  
    

      Hv_ChangeLabelPattern(dummy, i);
    }

    rc = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON + Hv_CANCELBUTTON);
  }

  newpatternlabel = mynewpattern;
  oldpatternlabel = myoldpattern;

/* the dialog has been created */

  newpattern = *pattern;
  
  if (prompt) {
    title = Hv_CreateSimpleCompoundString(prompt);
    XtVaSetValues(dialog, Hv_NdialogTitle, title, NULL);
    Hv_CompoundStringFree(title);
  }
  

/* set old & new pattern label to reflect current pattern     */

  Hv_ChangeLabelPattern(oldpatternlabel, *pattern);
  Hv_ChangeLabelPattern(newpatternlabel, *pattern);

  answer = Hv_DoDialog(dialog, NULL);

/* even if person hit ok, if the pattern was not changed treat like a cancel */
  
  if (answer == Hv_OK) {
    if (*pattern == newpattern)
      answer = 0;
    else {
      *pattern = newpattern;

/* a choice of 0 (black) is reset to -1, which
   is a flag indicating solid fill */

      if (*pattern == 0)
	*pattern = -1;

    }
  }

  return answer;
}




/**
 * Hv_XMChangeLabel
 * @purpose X Windows specific version of Hv_ChangeLabel.
 * @see The gateway routine Hv_ChangeLabel.
 */

void  Hv_XMChangeLabel(Hv_Widget w,
		     char      *label,
			 short      font) {

/* change a widget (typically a button) label */

  Hv_CompoundString	cs;             /* motif string */

  cs = Hv_CreateCompoundString(label, font);
  Hv_VaSetValues(w, Hv_NlabelString, cs, NULL);
  Hv_CompoundStringFree(cs);
}


/**
 * Hv_XMCreateThinOptionMenu
 * @purpose X Windows specific version of Hv_CreateThinOptionMenu.
 * @see The gateway routine Hv_CreateThinOptionMenu.
 */

Hv_Widget Hv_XMCreateThinOptionMenu(Hv_Widget      rc,
				    short          font,
				    char          *title,
				    int            numitems,
				    char          *items[],
				    Hv_FunctionPtr CB,
				    short          margin) {

    Widget     popup;
    int        i, n;
    Hv_CompoundString   mainlabel = NULL;
    Widget     submenu, label, dummy;
    Arg        arg[2]; /* arguments for widgets */
    int        buttoncount = -1;
    Boolean    varyfont;

    Hv_AttributeArray attributes;

    if (numitems < 1)
	return NULL;

    varyfont = (font < 0);
    
    submenu = XmCreatePulldownMenu(rc, "optionsubmenu", NULL, 0);

    XtVaSetValues(submenu,
		   Hv_NmarginHeight, 0,
		   NULL);

/* as we loop through,  look for "Hv_SEPARATOR" to flag
   the insertion of a separator */

    for (i = 0; i < numitems; i++) {
	
	if (Hv_Contained(items[i], "Hv_SEPARATOR")) {
	}
	else {
	    buttoncount++;

	    if (varyfont) {
		font = buttoncount*5 + 1;
		if (font < Hv_symbol11)
		    font += 25;
	    }

	    attributes[Hv_FONT].s = font;
	    attributes[Hv_LABEL].v = (void *)(items[i]);
	    attributes[Hv_CALLBACK].fp = (Hv_FunctionPtr)(CB);
	    attributes[Hv_DATA].v = (void *)(buttoncount);
      
	    dummy = Hv_ButtonDialogItem(submenu, attributes);

	    XtVaSetValues(dummy,
			   Hv_NborderWidth,  0,
			   Hv_NmarginWidth,  margin,
			   Hv_NmarginHeight,  margin,
			   Hv_NshadowThickness,  1,
			   NULL);

	}
    
    }
  
    n = 0;
    if (title != NULL) {
	mainlabel = Hv_CreateCompoundString(title, font);
	Hv_SetArg(arg[n], Hv_NlabelString, mainlabel);
	n++;
    }
    
    Hv_SetArg(arg[n], Hv_NsubMenuId,   submenu);
    n++;
    
    popup = XmCreateOptionMenu(rc, "option_menu", arg, n);
    
    XtVaSetValues(popup,
		   Hv_NfontList, Hv_fontList,
		   NULL);
    Hv_ManageChild(popup);
    
    label = XmOptionLabelGadget(popup);
    XtVaSetValues(label, Hv_NfontList, Hv_fontList, NULL);
    Hv_ChangeLabelColors(label, Hv_black, -1);
    
    if (mainlabel != NULL)
	Hv_CompoundStringFree(mainlabel);
    return  popup;
    
}

/**
 * Hv_XMGetOwnerDialog
 * @purpose X Windows specific version of Hv_GetOwnerDialog.
 * @see The gateway routine Hv_GetOwnerDialog.
 */

Hv_Widget    Hv_XMGetOwnerDialog(Hv_Widget ditem) {

 Widget   parent;

 if (ditem == NULL)
   return NULL;

 parent = Hv_Parent(ditem);

/* keep going up tree until the PARENT is a dialog SHELL */

 while (!XmIsDialogShell(parent)) {
     ditem = parent;
     parent = Hv_Parent(ditem);
     if (parent == NULL)
	 return NULL;
 }
 
 return ditem;
}

/**
 * Hv_XMActionAreaDialogItem
 * @purpose X Windows specific version of Hv_ActionAreaDialogItem.
 * @see The gateway routine Hv_ActionAreaDialogItem.
 */

Hv_Widget       Hv_XMActionAreaDialogItem(Hv_Widget         parent,
					  Hv_AttributeArray attributes) {
    Hv_Widget        rc, dummy;
    Boolean          ok, cancel, apply, del, done;
    Hv_Widget        dialog;
    Hv_DialogData    ddata;
    
    int              i, nc;

    char     *donestr   = (char *)attributes[Hv_LABEL].v;
    int       whichones = attributes[Hv_DIALOGCLOSEOUT].i;
  

/* get the dialog owner (might be the parent) */

    dialog = Hv_GetOwnerDialog(parent);

    if ((dialog == NULL) || ((ddata = Hv_GetDialogData(dialog)) == NULL)) {
	Hv_Println("Error in Hv_CreateManagedActionArea");
	return NULL;
    }

    nc = 0;

    ok     = Hv_CheckBit(whichones, Hv_OKBUTTON);
    cancel = Hv_CheckBit(whichones, Hv_CANCELBUTTON);
    apply  = Hv_CheckBit(whichones, Hv_APPLYBUTTON);
    del    = Hv_CheckBit(whichones, Hv_DELETEBUTTON);
    done   = (Hv_CheckBit(whichones, Hv_DONEBUTTON) && (donestr != NULL));


    if (ok)     nc++;
    if (cancel) nc++;
    if (apply)  nc++;
    if (del)    nc++;
    if (done)   nc++;
    
    if (nc < 1)
	return NULL;
    
    nc += 2; /* space b4 and after */
    
    rc = XtVaCreateWidget("action_area",
			  Hv_FormClass,
			  parent,
			  Hv_NfractionBase, TIGHTNESS*nc -1,
			   NULL);


    i = 0;
    if (ok)
	ddata->ok     = Hv_CreateActionButton(Hv_altCO ? "  Yes   " : "   OK   ",
					      1+i++,
					      nc,
					      rc,
					      (Hv_Pointer)Hv_OK,
					      Hv_CloseOutCallback);
    if (cancel)
	ddata->cancel = Hv_CreateActionButton(Hv_altCO ? " No " : " Cancel ",
					      1+i++,
					      nc,
					      rc,
					      (Hv_Pointer)Hv_CANCEL,
					      Hv_CloseOutCallback);

    if (apply)
	ddata->apply  = Hv_CreateActionButton(" Apply  ",
					      1+i++,
					      nc,
					      rc,
					      (Hv_Pointer)Hv_APPLY,
					      Hv_CloseOutCallback);
    if (done)
	dummy  = Hv_CreateActionButton(donestr,
				       1+i++,
				       nc,
				       rc,
				       (Hv_Pointer)Hv_DONE,
				       Hv_CloseOutCallback);
    if (del)
	ddata->del  = Hv_CreateActionButton(" Delete", 
					    1+i++, 
					    nc,
					    rc,
					    NULL,
					    NULL);


    Hv_ManageChild(rc);
    return rc;


}


/**
 * Hv_XMButtonDialogItem
 * @purpose X Windows specific version of Hv_ButtonDialogItem.
 * @see The gateway routine Hv_ButtonDialogItem.
 */

Hv_Widget       Hv_XMButtonDialogItem(Hv_Widget         parent,
				      Hv_AttributeArray attributes) {

    Hv_CompoundString   label;
    Hv_Widget           w;

    char           *labelstr = (char *)(attributes[Hv_LABEL].v);
    short           font     = attributes[Hv_FONT].s;
    Hv_CallbackProc callback = (Hv_CallbackProc)(attributes[Hv_CALLBACK].fp);
    Hv_Pointer      data     = attributes[Hv_DATA].v;


    label = Hv_CreateCompoundString(labelstr, font);

    w = XtVaCreateManagedWidget(" ", 
				Hv_PushButtonClass,        parent,
				Hv_NlabelString,           label,
				Hv_NhighlightThickness,    0,
				NULL);
  
    if (font != Hv_DEFAULT)
	XtVaSetValues(w, Hv_NfontList, Hv_fontList, NULL);
    
    if (callback != NULL)
	Hv_AddCallback(w,
		       Hv_activateCallback,
		       (Hv_CallbackProc)callback,
		       data);
    
    Hv_CompoundStringFree(label);
    return w;
    
}



/**
 * Hv_XMCloseOutDialogItem
 * @purpose X Windows specific version of Hv_CloseOutDialogItem.
 * @see The gateway routine Hv_CloseOutDialogItem.
 */

Hv_Widget       Hv_XMCloseOutDialogItem(Hv_Widget         parent,
					Hv_AttributeArray attributes) {

    Hv_Widget     rc, dummy;
    Boolean       ok, cancel, apply, del, done;
    Hv_Widget     dialog;
    Hv_DialogData ddata;

    short     spacing     = attributes[Hv_SPACING].s;
    short     orientation = attributes[Hv_ORIENTATION].s;
    short     font        = attributes[Hv_FONT].s;
    char     *donestr     = (char *)attributes[Hv_LABEL].v;
    int       whichones   = attributes[Hv_DIALOGCLOSEOUT].i;


/* get the dialog owner (might be the parent) */

    dialog = Hv_GetOwnerDialog(parent);

    if ((dialog == NULL) || ((ddata = Hv_GetDialogData(dialog)) == NULL)) {
	Hv_Println("Error in Hv_CreateManagedCloseOut");
	return NULL;
    }

    ok     = Hv_CheckBit(whichones, Hv_OKBUTTON);
    cancel = Hv_CheckBit(whichones, Hv_CANCELBUTTON);
    apply  = Hv_CheckBit(whichones, Hv_APPLYBUTTON);
    del    = Hv_CheckBit(whichones, Hv_DELETEBUTTON);
    done   = (Hv_CheckBit(whichones, Hv_DONEBUTTON) && (donestr != NULL));
  

    rc = Hv_VaCreateDialogItem(parent,
			       Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
			       Hv_ORIENTATION,  orientation,
			       Hv_SPACING,      spacing,
			       NULL); 
    
    if (ok)
	ddata->ok = Hv_VaCreateDialogItem(rc,
					  Hv_TYPE,     Hv_BUTTONDIALOGITEM,
					  Hv_LABEL,    Hv_altCO ? "  Yes   " : "   OK   ",
					  Hv_FONT,     font,
					  Hv_CALLBACK, Hv_CloseOutCallback,
					  Hv_DATA,     (void *)Hv_OK,
					  NULL);
    
    if (cancel) 
	ddata->cancel = Hv_VaCreateDialogItem(rc,
					      Hv_TYPE,     Hv_BUTTONDIALOGITEM,
					      Hv_LABEL,    Hv_altCO ? " No " : " Cancel ",
					      Hv_FONT,     font,
					      Hv_CALLBACK, Hv_CloseOutCallback,
					      Hv_DATA,     (void *)Hv_CANCEL,
					      NULL);
    
    if (apply)
	ddata->apply = Hv_VaCreateDialogItem(rc,
					     Hv_TYPE,     Hv_BUTTONDIALOGITEM,
					     Hv_LABEL,    " Apply ",
					     Hv_FONT,     font,
					     Hv_CALLBACK, Hv_CloseOutCallback,
					     Hv_DATA,     (void *)Hv_APPLY,
					     NULL);
    
    if (done)
	dummy = Hv_VaCreateDialogItem(rc,
				      Hv_TYPE,     Hv_BUTTONDIALOGITEM,
				      Hv_LABEL,    donestr,
				      Hv_FONT,     font,
				      Hv_CALLBACK, Hv_CloseOutCallback,
				      Hv_DATA,     (void *)Hv_DONE,
				      NULL);
    
    if (del)
	ddata->del = Hv_VaCreateDialogItem(rc,
					   Hv_TYPE,     Hv_BUTTONDIALOGITEM,
					   Hv_LABEL,    (char *)" Delete",
					   Hv_FONT,     font,
					   NULL);
    return rc;
}


/**
 * Hv_XMColorSelectorDialogItem
 * @purpose X Windows specific version of Hv_ColorSelectorDialogItem.
 * @see The gateway routine Hv_ColorSelectorDialogItem.
 */

Hv_Widget       Hv_XMColorSelectorDialogItem(Hv_Widget         parent,
					     Hv_AttributeArray attributes) {

    Hv_Widget   colorrc, dummy;
    int      i;
    int      nr;
    short    dx = attributes[Hv_WIDTH].s;
    short    dy = attributes[Hv_HEIGHT].s;
    
    if ((dx < 2) || (dy < 2))
	return NULL;


    nr = Hv_numColors / 16;

    colorrc = Hv_VaCreateDialogItem(parent,
				    Hv_TYPE,        Hv_ROWCOLUMNDIALOGITEM,
				    Hv_PACKING,     Hv_PAD,
				    Hv_NUMROWS,     nr,
				    Hv_SPACING,     0,
				    NULL);

    for(i = 0;  i < Hv_numColors; i++) {
	dummy = XmCreateDrawingArea(colorrc, " ", NULL, 0);
    
	XtVaSetValues(dummy,
		       Hv_Nwidth,          (Dimension)dx,
		       Hv_Nheight,         (Dimension)dy,
		       Hv_Nbackground,     (Pixel)(Hv_colors[i]),
		       NULL);

	Hv_AddEventHandler(dummy,
			   Hv_buttonPressMask,
			   (Hv_EventHandler)Hv_SetColor,
			   (Hv_Pointer)i);  
    
	Hv_ManageChild(dummy);
    }
  
    
    return colorrc;

}


/**
 * Hv_XMFileSelectorDialogItem
 * @purpose X Windows specific version of Hv_FileSelectorDialogItem.
 * @see The gateway routine Hv_FileSelectorDialogItem.
 */

Hv_Widget       Hv_XMFileSelectorDialogItem(Hv_Widget         parent,
					    Hv_AttributeArray attributes) {

    short     width = attributes[Hv_WIDTH].s;
    char     *mask = (char *)(attributes[Hv_MASK].v);

    Hv_Widget w;

    Hv_Widget separator = NULL;
    Hv_Widget ok        = NULL;
    Hv_Widget filter    = NULL;
    Hv_Widget cancel    = NULL;
    Hv_Widget help      = NULL;
    Arg       args[10];
    int	      n = 0;

    Hv_CompoundString         xmst;


/* Dat Cao of Sparta, Inc's widget */
 
    Hv_SetArg(args[n], Hv_NnoResize,     True);	             n++;
    Hv_SetArg(args[n], Hv_NresizePolicy, XmRESIZE_NONE);	     n++;
    Hv_SetArg(args[n], Hv_Nwidth,        Hv_sMax(300, width)); n++;


    w = XmCreateFileSelectionBox(parent, "fsb", args, n);

/* get rid of stuff that will be handled by the dialogs standard actions */

     separator = XmFileSelectionBoxGetChild(w, XmDIALOG_SEPARATOR);    
     ok        = XmFileSelectionBoxGetChild(w, XmDIALOG_OK_BUTTON);
     filter    = XmFileSelectionBoxGetChild(w, XmDIALOG_APPLY_BUTTON);
     cancel    = XmFileSelectionBoxGetChild(w, XmDIALOG_CANCEL_BUTTON);
     help      = XmFileSelectionBoxGetChild(w, XmDIALOG_HELP_BUTTON);
     Hv_UnmanageChild(separator);    
     Hv_UnmanageChild(ok);

     Hv_UnmanageChild(cancel);
     Hv_UnmanageChild(help);

     if (mask != NULL)
	  xmst =  Hv_CreateSimpleCompoundString(mask);
     else
	  xmst =  Hv_CreateSimpleCompoundString("*");
     
     XtVaSetValues(w,
		   Hv_Npattern,      xmst,
		   NULL);  
     Hv_CompoundStringFree(xmst);

     Hv_ManageChild(w);
     return w;

}



/**
 * Hv_XMLabelDialogItem
 * @purpose X Windows specific version of Hv_LabelDialogItem.
 * @see The gateway routine Hv_LabelDialogItem.
 */

Hv_Widget       Hv_XMLabelDialogItem(Hv_Widget         parent,
				     Hv_AttributeArray attributes) {

    Hv_CompoundString   label;
    Hv_Widget           w;
    char               *lab;

    char *labelstr = (char *)(attributes[Hv_LABEL].v);
    short font     = attributes[Hv_FONT].s;
    short fg;
    short bg       = attributes[Hv_BACKGROUND].s;
    short bw       = attributes[Hv_BORDERWIDTH].s;
    short bc       = attributes[Hv_BORDERCOLOR].s;



    if (attributes[Hv_TEXTCOLOR].s >= 0)
	fg = attributes[Hv_TEXTCOLOR].s;
    else
	fg = attributes[Hv_COLOR].s;

    if (labelstr == NULL)
	Hv_InitCharStr(&lab, " ");
    else
	lab = labelstr;

    label = Hv_CreateCompoundString(lab, font);

    if ((bw < 0 ) || (bc < 0))
	w = XtVaCreateManagedWidget("label", 
				    Hv_LabelClass,          parent,
				    Hv_NlabelString,        label,
				    Hv_NhighlightThickness, 0,
				    NULL);
    else
	w = XtVaCreateManagedWidget("label", 
				     Hv_LabelClass,          parent,
				     Hv_NlabelString,        label,
				     Hv_NhighlightThickness, 0,
				     Hv_NborderWidth,        bw,
				     Hv_NborderColor,        Hv_colors[bc],
				     NULL);

    if (font != Hv_DEFAULT)
	XtVaSetValues(w, Hv_NfontList, Hv_fontList, NULL);
    
    Hv_CompoundStringFree(label);

    Hv_ChangeLabelColors(w, fg, bg);
    return w;
}


/**
 * Hv_XMListDialogItem
 * @purpose X Windows specific version of Hv_ListDialogItem.
 * @see The gateway routine Hv_ListDialogItem.
 */

Hv_Widget       Hv_XMListDialogItem(Hv_Widget         parent,
				    Hv_AttributeArray attributes) {

    Hv_Widget          w;
    Hv_CompoundString *xmstitems = NULL;
    int                i;
    Arg                args[2];

    short           numitems     = attributes[Hv_NUMITEMS].s;
    short           numvisitems  = attributes[Hv_NUMVISIBLEITEMS].s;
    char            policy       = attributes[Hv_LISTSELECTION].c;
    char            resizepolicy = attributes[Hv_LISTRESIZEPOLICY].c;
    char          **items        = (char **)(attributes[Hv_LIST].v);
    Hv_CallbackProc callback     = (Hv_CallbackProc)(attributes[Hv_CALLBACK].fp);

  
    if ((numitems > 0) && (items == NULL))
	return NULL;

    if (numitems > 0)	
	xmstitems = (Hv_CompoundString *)Hv_Malloc(numitems*sizeof(Hv_CompoundString));
    
    for (i = 0; i < numitems; i++)
	xmstitems[i] = Hv_CreateSimpleCompoundString(items[i]);
    
    Hv_SetArg(args[0], Hv_NlistSizePolicy, (unsigned char)resizepolicy);
    w = XmCreateScrolledList(parent, " ", args, 1);
    
    XtVaSetValues(w,
		   Hv_NitemCount,               numitems,
		   Hv_NvisibleItemCount,        numvisitems,
		   Hv_NfontList,                Hv_fontList,
		   Hv_NhighlightThickness,      0,
		   Hv_NselectionPolicy,         (unsigned char)policy,
		   Hv_Nitems,                   xmstitems,
		   NULL);
    
    if (policy == Hv_MULTIPLELISTSELECT) {
	if (callback != NULL)
	    Hv_AddCallback(w,
			   Hv_multipleSelectionCallback,
			   (Hv_CallbackProc)callback,
			   NULL);
	else
	    Hv_AddCallback(w,
			   Hv_multipleSelectionCallback,
			   (Hv_CallbackProc)Hv_NullCallback,
			   NULL);
    }
    else {
	if (callback != NULL)
	    Hv_AddCallback(w,
			   Hv_singleSelectionCallback,
			   (Hv_CallbackProc)callback,
			   NULL);
	else
	    Hv_AddCallback(w,
			   Hv_singleSelectionCallback,
			   (Hv_CallbackProc)Hv_NullCallback,
			   NULL);
    }
    
    Hv_ManageChild(w);
    return  w;
}

/**
 * Hv_XMRadioDialogItem
 * @purpose X Windows specific version of Hv_RadioDialogItem.
 * @see The gateway routine Hv_RadioDialogItem.
 */

Hv_Widget       Hv_XMRadioDialogItem(Hv_Widget         parent,
				     Hv_AttributeArray attributes) {

    Hv_CompoundString  label;
    Hv_Widget          w;
    Hv_RadioListPtr    temp;
    short              count;


    char            *labelstr   = (char *)(attributes[Hv_LABEL].v);
    short            font       = attributes[Hv_FONT].s;
    Hv_CallbackProc  callback   = (Hv_CallbackProc)(attributes[Hv_CALLBACK].fp);
    Hv_RadioHeadPtr *radiohead  = (Hv_RadioHeadPtr *)(attributes[Hv_RADIOLIST].v);
    void            *mydata     = attributes[Hv_DATA].v;


    if (*radiohead == NULL) {
	*radiohead = (Hv_RadioHeadPtr)Hv_Malloc(sizeof(Hv_RadioHead));
	(*radiohead)->head = NULL;
	(*radiohead)->activewidget = NULL;
	(*radiohead)->activepos = 0;
    }

    label = Hv_CreateCompoundString(labelstr, font);
    
    w = XtVaCreateManagedWidget(" ", 
				 Hv_ToggleButtonClass,        parent,
				 Hv_NlabelString,             label,
				 Hv_NhighlightThickness,      0,
				 Hv_NindicatorType,           XmONE_OF_MANY,
				 Hv_NvisibleWhenOff,          True,
				 NULL);
    
    if (callback != NULL)
	Hv_AddCallback(w,
		       Hv_valueChangedCallback,
		       (Hv_CallbackProc)callback,
		       (Hv_Pointer)parent);
    else
	Hv_AddCallback(w,
		       Hv_valueChangedCallback,
		       (Hv_CallbackProc)Hv_StandardRadioButtonCallback,
		       (Hv_Pointer)(*radiohead));
    
    if (font != Hv_DEFAULT)
	XtVaSetValues(w, Hv_NfontList, Hv_fontList, NULL);
    
    Hv_CompoundStringFree(label);


/* now add to list */

    count = 1;
    
    if ((*radiohead)->head == NULL) {
	(*radiohead)->head = (Hv_RadioListPtr)Hv_Malloc(sizeof(Hv_RadioList));
	temp = (*radiohead)->head;
    }
    
    else {
	count = 2;
	for (temp = (*radiohead)->head; temp->next != NULL; temp = temp->next)
	    count++;
	temp->next = (Hv_RadioListPtr)Hv_Malloc(sizeof(Hv_RadioList));
	temp = temp->next;
    }
    
    temp->radio = w;
    temp->next = NULL;
    temp->data = mydata;
    temp->pos = count;      /* not a C index */
    return w;
}



/**
 * Hv_XMScaleDialogItem
 * @purpose X Windows specific version of Hv_ScaleDialogItem.
 * @see The gateway routine Hv_ScaleDialogItem.
 */

Hv_Widget       Hv_XMScaleDialogItem(Hv_Widget         parent,
				     Hv_AttributeArray attributes) {

    Hv_Widget        w;
    int              orientation  = (int)(attributes[Hv_ORIENTATION].s); 
    int              startval     = attributes[Hv_CURRENTVALUE].i; 
    int              minval       = attributes[Hv_MINVALUE].i; 
    int              maxval       = attributes[Hv_MAXVALUE].i; 
    Hv_CallbackProc  callback     = (Hv_CallbackProc)(attributes[Hv_CALLBACK].fp);

/* consider the possibility that Hv_DEFAULT was used */

    Hv_CheckDefault(&minval,      0); 
    Hv_CheckDefault(&startval,    0); 
    Hv_CheckDefault(&maxval,      100); 
    Hv_CheckDefault(&orientation, Hv_VERTICAL); 

    w =  XtVaCreateManagedWidget("scale",
				  Hv_ScaleClass,            parent,
				  Hv_Norientation,          (unsigned char)orientation,
				  Hv_Nminimum,              minval,
				  Hv_Nmaximum,              maxval,
				  Hv_Nvalue,                startval,
				  Hv_Nsensitive,            True,
				  Hv_NshowValue,            True,
				  NULL);
  

    if (callback != NULL)
	Hv_AddCallback(w,
		       Hv_valueChangedCallback,
		       callback,
		       NULL);

    return w;

}


/**
 * Hv_XMSeparatorDialogItem
 * @purpose X Windows specific version of Hv_SeparatorDialogItem.
 * @see The gateway routine Hv_SeparatorDialogItem.
 */

Hv_Widget       Hv_XMSeparatorDialogItem(Hv_Widget         parent,
					 Hv_AttributeArray attributes) {

    Hv_Widget   w;

    w = XtVaCreateManagedWidget(" ", 
				 Hv_SeparatorClass,
				 parent,
				 NULL);
    
    return w;
}


/**
 * Hv_XMTextDialogItem
 * @purpose X Windows specific version of Hv_TextDialogItem.
 * @see The gateway routine Hv_TextDialogItem.
 */

Hv_Widget       Hv_XMTextDialogItem(Hv_Widget         parent,
				    Hv_AttributeArray attributes) {

    Hv_Widget     w;
    Dimension     mh = 2;
    Dimension     mw = 2;
    char         *value = NULL;

    char            *defaultstr = (char *)(attributes[Hv_DEFAULTTEXT].v);
    short            cols       = attributes[Hv_NUMCOLUMNS].s;
    Hv_CallbackProc  vchanged   = (Hv_CallbackProc)(attributes[Hv_VALUECHANGED].fp);
    short            font       = attributes[Hv_FONT].s;
    Boolean          editable   = (Boolean)attributes[Hv_EDITABLE].i;

    static char  transtable[] =
	"<Key>osfBackSpace: delete-previous-character()";
  
    static char  trans2table[] =
	"<Key>osfDelete: delete-next-character()";

/* Insidious bug fixed by Jack Scheible of Sparta Inc. */  

    w = XtVaCreateManagedWidget("textedit", 
				 Hv_TextFieldClass,         parent,
				 Hv_Ncolumns,               cols,
				 Hv_Neditable,              editable,
				 Hv_NmarginHeight,          mh,
				 Hv_NmarginWidth,           mw,
				 Hv_NcursorPositionVisible, editable,
				 NULL);
    
    
    if (defaultstr != NULL) {
	Hv_InitCharStr(&value, defaultstr);
	XtVaSetValues(w, Hv_Nvalue, (String)value, NULL);
    }


/******************************************************
  The next two translation overrides were added 5/26/96
  to try to fix the wrongway delete on all platforms
*******************************************************/

    Hv_OverrideTranslations(w, transtable);
    Hv_OverrideTranslations(w, trans2table);


    if (font != Hv_DEFAULT)
	XtVaSetValues(w, Hv_NfontList, Hv_fontList, NULL);

    if (vchanged != NULL)
	Hv_AddCallback(w,
		       Hv_valueChangedCallback,
		       (Hv_CallbackProc)vchanged,
		       NULL);
    
    Hv_Free(value);
    return w;
    
}


/**
 * Hv_XMToggleDialogItem
 * @purpose X Windows specific version of Hv_ToggleDialogItem.
 * @see The gateway routine Hv_ToggleDialogItem.
 */

Hv_Widget       Hv_XMToggleDialogItem(Hv_Widget         parent,
				      Hv_AttributeArray attributes) {

    Hv_CompoundString   label;
    Hv_Widget           w;

    char            *labelstr = (char *)(attributes[Hv_LABEL].v);
    short            font     = attributes[Hv_FONT].s;
    Hv_CallbackProc  callback = (Hv_CallbackProc)(attributes[Hv_CALLBACK].fp);
    Hv_Pointer       data     = (Hv_Pointer)(attributes[Hv_DATA].v);



    label = Hv_CreateCompoundString(labelstr, font);
    
    w = XtVaCreateManagedWidget(" ", 
				 Hv_ToggleButtonClass,        parent,
				 Hv_NlabelString,             label,
				 Hv_NhighlightThickness,      0,
				 NULL);
  
    if (font != Hv_DEFAULT)
	XtVaSetValues(w, Hv_NfontList, Hv_fontList, NULL);
    
    Hv_CompoundStringFree(label);
    
    if (callback)
	Hv_AddCallback(w,
		       Hv_valueChangedCallback,
		       callback,
		       data);
  
    return w;

}

/**
 * Hv_XMRowColDialogItem
 * @purpose X Windows specific version of Hv_RowColDialogItem.
 * @see The gateway routine Hv_RowColDialogItem.
 */

Hv_Widget Hv_XMRowColDialogItem(Hv_Widget         parent,
				Hv_AttributeArray attributes) {

    short spacing;
    short orientation;
    short packing;
    short alignment;
    short nrc;

/* extract the attributes */

    alignment   = attributes[Hv_ALIGNMENT].s;
    spacing     = attributes[Hv_SPACING].s;
    orientation = attributes[Hv_ORIENTATION].s;
    packing     = attributes[Hv_PACKING].s;

/* avoid confusion over which to use, NROWS or NCOLUMNS */
    
    nrc = Hv_sMax(attributes[Hv_NUMROWS].s, attributes[Hv_NUMCOLUMNS].s);

/* create the widget accordingly */

    return XtVaCreateManagedWidget("rowcolumn",
				    Hv_RowColumnClass,       parent,
				    Hv_Norientation,         orientation,
				    Hv_Nspacing,             spacing,
				    Hv_Npacking,             packing,
				    Hv_NnumColumns,          nrc,
				    Hv_NentryAlignment,      alignment,
				    Hv_NisHomogeneous,       False,
				    NULL);

}

/**
 * Hv_XMScrolledTextDialogItem
 * @purpose X Windows specific version of Hv_ScrolledTextDialogItem.
 * @see The gateway routine Hv_ScrolledTextDialogItem.
 */

Hv_Widget Hv_XMScrolledTextDialogItem(Hv_Widget         parent,
				      Hv_AttributeArray attributes) {

    int        nrow;
    int        ncol;
    short      bg;
    short      fg;
    Boolean    wordwrap;
    Hv_Widget  w;
    Arg        args[20];
    int        n;
    Boolean    hscroll;

/* extract the attributes */

    nrow     = (int)(attributes[Hv_NUMROWS].s);
    ncol     = (int)(attributes[Hv_NUMCOLUMNS].s);
    bg       = attributes[Hv_BACKGROUND].s;
    fg       = attributes[Hv_TEXTCOLOR].s;
    wordwrap = (attributes[Hv_WORDWRAP].i != 0);
    hscroll  = ((attributes[Hv_SCROLLPOLICY].s == Hv_SCROLLHORIZONTAL) ||
		(attributes[Hv_SCROLLPOLICY].s == Hv_SCROLLBOTH));

/* create the widget accordingly */

    n = 0;
    Hv_SetArg(args[n], Hv_Nrows,                   nrow);              n++;
    Hv_SetArg(args[n], Hv_Ncolumns,                ncol);              n++;
    Hv_SetArg(args[n], Hv_Neditable,               False);             n++;
    Hv_SetArg(args[n], Hv_NeditMode,               XmMULTI_LINE_EDIT); n++;
    Hv_SetArg(args[n], Hv_NscrollHorizontal,       hscroll);           n++;
    Hv_SetArg(args[n], Hv_NscrollBarDisplayPolicy, XmSTATIC);          n++;
    Hv_SetArg(args[n], Hv_NblinkRate,              0);                 n++;
    Hv_SetArg(args[n], Hv_NautoShowCursorPosition, True);              n++;
    Hv_SetArg(args[n], Hv_NcursorPositionVisible,  False);             n++;
    Hv_SetArg(args[n], Hv_NfontList,               Hv_fontList);       n++;
    Hv_SetArg(args[n], Hv_NwordWrap,               wordwrap);          n++;
    
    w = XmCreateScrolledText(parent, "scrolled", args, n);
    
    if (bg != Hv_DEFAULT)
	Hv_SetWidgetBackgroundColor(w, bg);
    
    if (fg != Hv_DEFAULT)
	Hv_SetWidgetForegroundColor(w, fg);
    
    Hv_ManageChild(w);
    Hv_ManageChild(Hv_Parent(w));  /* the window that will hold the text*/
    
    return  w;
}




/**
 * Hv_XMGetScaleValue
 * @purpose X Windows  specific version of Hv_GetScaleValue.
 * @see The gateway routine Hv_GetScaleValue. 
 */


int  Hv_XMGetScaleValue(Hv_Widget   w) {
  int   val;

  XmScaleGetValue(w,  &val);
  return val;
}



/**
 * Hv_XMSetScaleValue
 * @purpose X Windows  specific version of Hv_SetScaleValue.
 * @see The gateway routine Hv_SetScaleValue. 
 */


void Hv_XMSetScaleValue(Hv_Widget   w,
		      int         val) {
  XmScaleSetValue(w, val);
}

/**
 * Hv_XMListItemPos
 * @purpose X Windows specific version of Hv_ListItemPos.
 * @see The gateway routine Hv_ListItemPos.
 */

int       Hv_XMListItemPos(Hv_Widget list,
			   char     *str) {
  XmString    xmst;
  int         pos;
 
  xmst = Hv_CreateMotifString(str, Hv_fixed2);
  pos =  XmListItemPos(list, xmst);

  XmStringFree(xmst);
  return pos;
}

/**
 * Hv_XMCheckListItem
 * @purpose X Windows specific version of Hv_CheckListItem.
 * @see The gateway routine Hv_CheckListItem.
 */

Boolean         Hv_XMCheckListItem(Hv_Widget list,
				   char *str,
				   int  **poslist,
				   int  *num) {

  Hv_CompoundString    cs;
  Boolean     check;
 
  cs = Hv_CreateCompoundString(str, Hv_fixed2);
  check =  XmListGetMatchPos(list, cs, poslist, num);

/* Motif seems to have num as undefined if not found which seems
   a little silly */

  if (!check) {
    *num = 0;
    *poslist = NULL;
  }

  Hv_CompoundStringFree(cs);
  return check;
}




/**
 * Hv_XMAddListItem
 * @purpose X Windows specific version of Hv_AddListItem.
 * @see The gateway routine Hv_AddListItem.
 */


void    Hv_XMAddListItem(Hv_Widget list,
			 char      *str,
			 Boolean   selected) {

  Hv_CompoundString    cs;
 
  cs = Hv_CreateCompoundString(str, Hv_fixed2);
 
  if (selected) {
    XmListAddItem(list, cs, 0);
    XmListSelectItem(list, cs, True);
  }
  else
    XmListAddItemUnselected(list, cs, 0);

  Hv_CompoundStringFree(cs);
}


/**
 * Hv_XMPosReplaceListItem
 * @purpose X Windows specific version of Hv_PosReplaceListItem.
 * @see The gateway routine Hv_PosReplaceListItem.
 */

void  Hv_XMPosReplaceListItem(Hv_Widget list,
			      int       pos,
			      char      *str) {

  Hv_CompoundString    cs;
 
  cs = Hv_CreateCompoundString(str, Hv_fixed2);
 
  XmListReplaceItemsPos(list, &cs, 1, pos);
  Hv_CompoundStringFree(cs);
}


/**
 * Hv_XMPosSelectListItem
 * @purpose X Windows specific version of Hv_PosSelectListItem.
 * @see The gateway routine Hv_PosSelectListItem.
 */

void  Hv_XMPosSelectListItem(Hv_Widget list,
			     int       pos) {
    
/* note pos is NOT a C index */

  if (pos <= 0) {
    Hv_DeselectAllListItems(list);
    return;
  }
  
  XmListSelectPos(list, pos, True);
}


/**
 * Hv_XMNewPosSelectListItem
 * @purpose X Windows specific version of Hv_NewPosSelectListItem.
 * @see The gateway routine Hv_NewPosSelectListItem.
 */


void  Hv_XMNewPosSelectListItem(Hv_Widget list,
				int       pos,
				Boolean   cbflag) {

/* note pos is NOT a C index */

  XmListSelectPos(list, pos, cbflag);
}


/**
 * Hv_XMListSelectItem
 * @purpose X Windows specific version of Hv_ListSelectItem.
 * @see The gateway routine Hv_ListSelectItem.
 */

void    Hv_XMListSelectItem(Hv_Widget list,
			    char     *str,
			    Boolean callit) {

  Hv_CompoundString    cs;
 
  if (str == NULL) {
    Hv_DeselectAllListItems(list);
    return;
  }

  cs = Hv_CreateCompoundString(str, Hv_fixed2);
  XmListSelectItem(list, cs, callit);
  Hv_CompoundStringFree(cs);
}


/**
 * Hv_XMDeleteAllListItems
 * @purpose X Windows specific version of Hv_DeleteAllListItems.
 * @see The gateway routine Hv_DeleteAllListItems.
 */

void    Hv_XMDeleteAllListItems(Hv_Widget list) {
  XmListDeleteAllItems(list);
}


/**
 * Hv_XMDeselectAllListItems
 * @purpose X Windows specific version of Hv_DeselectAllListItems.
 * @see The gateway routine Hv_DeselectAllListItems.
 */

void    Hv_XMDeselectAllListItems(Hv_Widget list) {
  XmListDeselectAllItems(list);
}



/**
 * Hv_XMGetListSelectedPos
 * @purpose X Windows specific version of Hv_GetListSelectedPos
 * @see The gateway routine Hv_GetListSelectedPos
 */

int  Hv_XMGetListSelectedPos(Hv_Widget list) {

  int     *pos;
  int     count;
  int     result;

  if (XmListGetSelectedPos(list, &pos, &count)) {
    result =  pos[0];
    XtFree((char *)pos);
  }
  else
    result =  -1;

  return result;
} 



/**
 * Hv_XMGetAllListSelectedPos
 * @purpose X Windows specific version of Hv_GetAllListSelectedPos.
 * @see The gateway routine Hv_GetAllListSelectedPos.
 */

int  *Hv_XMGetAllListSelectedPos(Hv_Widget list) {


  int     *pos;
  int     count;
  int     *result;
  int     i;

  if (XmListGetSelectedPos(list, &pos, &count)){

/* copy to the returned array */

    result = (int *)Hv_Malloc((count+1)*sizeof(int));
    for (i = 0; i < count; i++)
      result[i] = pos[i];
    result[count] = -1;
    XtFree((char *)pos);
    return result;
  }
  else
    return NULL;

}



/**
 * Hv_XMChangeLabelPattern
 * @purpose X Windows specific version of Hv_ChangeLabelPattern.
 * @see The gateway routine Hv_ChangeLabelPattern.
 */

void  Hv_XMChangeLabelPattern(Hv_Widget w,
			      short     pattern) {
    

  Pixmap       pix;
  unsigned int depth;
  int          wfg, wbg;

  if ((pattern < 0) || (pattern >= Hv_NUMPATTERNS))
      pattern = 0;

  XtVaGetValues(w,
		 Hv_Nforeground, &wfg,
		 Hv_Nbackground, &wbg,
		 NULL);
  
  depth = DefaultDepthOfScreen(DefaultScreenOfDisplay(Hv_display));
  
  pix = XCreatePixmapFromBitmapData(XtDisplay(Hv_toplevel),
				    RootWindowOfScreen(XtScreen(Hv_toplevel)),
				    (char *)Hv_bitmaps[pattern],
				    16, 16, wfg, wbg, depth);


  XtVaSetValues(w, Hv_NlabelPixmap, pix, NULL);

}


/**
 * Hv_XMAddLineToScrolledText
 * @purpose X Windows  specific version of Hv_AddLineToScrolledText.
 * @see The gateway routine Hv_AddLineToScrolledText. 
 */


void  Hv_XMAddLineToScrolledText(Hv_Widget        w,
			       Hv_TextPosition *pos,
			       char            *line) {

  XmTextInsert(w, *pos, line);
  *pos += strlen(line);
  XmTextSetInsertionPosition(w, *pos);
  XmUpdateDisplay(w);

  if (*pos > Hv_MAX_TEXTSIZE)
    Hv_DeleteScrolledTextLines(w, 1 + (*pos - Hv_MAX_TEXTSIZE)/100, pos);

}



/**
 * Hv_XMAppendLineToScrolledText
 * @purpose X Windows  specific version of Hv_AppendLineToScrolledText.
 * @see The gateway routine Hv_AppendLineToScrolledText. 
 */


void Hv_XMAppendLineToScrolledText(Hv_Widget     w,
				 char          *line) {

/* add a line to bottom of scrolled text widget */

  Boolean            newline = False;
  int                len;
  Hv_TextPosition    pos;
  char               c;

  if ((w == NULL) || (line == NULL))
    return;

  len = strlen(line);
  if (len > 0) {
    c = line[len-1];
    newline = (c != '\n');
  }

  pos = XmTextGetLastPosition(w);

  XmTextInsert(w, pos, line);

  if (newline) {
    pos = XmTextGetLastPosition(w);
    XmTextInsert(w, pos, "\n");
  }

  XmUpdateDisplay(w);

  if (pos > Hv_MAX_TEXTSIZE) {
    Hv_DeleteScrolledTextLines(w, 1 + (pos - Hv_MAX_TEXTSIZE)/100, &pos);
  }

}




/**
 * Hv_XMJumpToBottomOfScrolledText
 * @purpose X Windows  specific version of Hv_JumpToBottomOfScrolledText.
 * @see The gateway routine Hv_JumpToBottomOfScrolledText. 
 */


void Hv_XMJumpToBottomOfScrolledText(Hv_Widget        w,
				   Hv_TextPosition *pos) {

/* place cursor at end of buffer and set pos accoringly */


  *pos = XmTextGetLastPosition(w);
  XmTextSetInsertionPosition(w, *pos);
}



/**
 * Hv_XMDeleteLinesFromTop
 * @purpose X Windows  specific version of Hv_DeleteLinesFromTop.
 * @see The gateway routine Hv_DeleteLinesFromTop. 
 */


void  Hv_XMDeleteLinesFromTop(Hv_Widget        w,
			    int              nlines) {
  Hv_TextPosition    pos;

  pos = XmTextGetLastPosition(w);
  Hv_DeleteScrolledTextLines(w, nlines, &pos);
}



/**
 * Hv_XMDeleteScrolledTextLines
 * @purpose X Windows  specific version of Hv_DeleteScrolledTextLines.
 * @see The gateway routine Hv_DeleteScrolledTextLines. 
 */


void  Hv_XMDeleteScrolledTextLines(Hv_Widget        w,
				 int              nlines,
				 Hv_TextPosition *endpos) {

/* delete n lines from text, presumably from a text widget.
   if nlines is < 0, all text up to endpos is deleted */

  char  *buffer;
  char  *newstr;
  int   i;

  buffer = XmTextGetString(w);
  newstr = buffer;

  if (nlines < 0)
    newstr += *endpos;
  else {
    i = 0;
    while ((i < nlines) && (newstr != NULL)) {
      newstr = strstr(newstr, "\n") + 1;
      i++;
    }
  }  
  XmTextSetString(w, newstr);

  *endpos = XmTextGetLastPosition(w);

  XtFree(buffer);

}

/**
 * Hv_XMClearScrolledText
 * @purpose X Windows  specific version of Hv_ClearScrolledText.
 * @see The gateway routine Hv_ClearScrolledText. 
 */


void Hv_XMClearScrolledText(Hv_Widget        w,
			  Hv_TextPosition *pos) {
  Hv_DeleteScrolledTextLines(w, -1, pos);
  XmTextShowPosition(w, *pos);
}



/**
 * Hv_XMPopupDialog
 * @purpose X Windows  specific version of Hv_PopupDialog.
 * @see The gateway routine Hv_PopupDialog. 
 */


void Hv_XMPopupDialog(Hv_Widget dialog) {
    XtPopup(Hv_Parent(dialog), XtGrabNone);
}


/**
 * Hv_XMPopdownDialog
 * @purpose X Windows  specific version of Hv_PopdownDialog.
 * @see The gateway routine Hv_PopdownDialog. 
 */


void Hv_XMPopdownDialog(Hv_Widget dialog) {
  XtPopdown(Hv_Parent(dialog));
}



/**
 * Hv_XMOverrideTranslations
 * @purpose X Windows  specific version of Hv_OverrideTranslations.
 * @see The gateway routine Hv_OverrideTranslations. 
 */


void Hv_XMOverrideTranslations(Hv_Widget w,
			       char     *table) {
    XtOverrideTranslations(w, XtParseTranslationTable(table));
}




/*==============================================
 * HELP RELATED SECTION OF GATEWAY
 *==============================================*/


/**
 * Hv_XMDoHelp
 * @purpose X Windows  specific version of Hv_DoHelp.
 * @see The gateway routine Hv_DoHelp. 
 */

void         Hv_XMDoHelp(void) {

#define  N_COLS1      63
#define  N_ROWS1      20

/* the help dialog */

  static    Hv_Widget     dialog = NULL;
  static    Hv_Widget     topicsList;
  static    int           numtopics = -1;
  static    char          **topics;
  Hv_Widget               rowcol, rc, dummy;
  int                     i, j;
  char                    *tstr;
  

  char	*fname0;
  char  *helpfilename;
  char  *line;

/* if there is no file, open it and get the help
   topics */

  if (!helpfp) {

/* look in several places for the help file */

    fname0 = (char *)Hv_Malloc(strlen(Hv_programName) + 6);
    strcpy(fname0, Hv_programName);
    strcat(fname0, ".help");

    helpfp = Hv_LookForNamedFile(fname0, Hv_ioPaths, "r");

    Hv_Free(fname0);
    if (!helpfp) {

/* try one last trick */

      fname0 = Hv_FindFileWithExtension(".help", Hv_ioPaths, "Help file");

      if (fname0 != NULL) {
	helpfp = fopen(fname0, "r");
	Hv_Free(fname0);
      }

/* still no? */

      if (!helpfp) {
	if (Hv_Question("Can't find the help file. Do you want to look for it?")) {
	  helpfilename = Hv_FileSelect("Please locate the help file.", "*.help", NULL);


	  if (helpfilename == NULL)
	    return;


	  helpfp = fopen(helpfilename, "r");
	  Hv_Free(helpfilename);
	}
      }

/* if still don't have it, give up */

      if (!helpfp)
	return;
      
    }
  }

/* count the number of topics */

  if (numtopics < 1) {
    line = (char *)Hv_Malloc(Hv_LINESIZE);

    numtopics = 0;
    rewind(helpfp);

    while(Hv_FindRecord(helpfp, "help", False, line))
      numtopics++;


    topics = (char **)Hv_Malloc(numtopics*sizeof(char *));
    
    i = 0;
    rewind(helpfp);
    while(Hv_FindRecord(helpfp, "help_", False, line)) {
      topics[i] = (char *)Hv_Malloc(strlen(line) + 1);
      strncpy(topics[i], line + 6, strlen(line) - 7);
      topics[i][strlen(line) - 8] = '\0';
      i++;
    }
    Hv_Free(line);


/* bubble sort the topics */

    if (numtopics > 1) {
      for (i = 0; i < (numtopics - 1); i++)
	for (j = i+1; j < numtopics; j++)
	  if (strncasecmp(topics[i], topics[j], 15) > 0) {
	    tstr = topics[i];
	    topics[i] = topics[j];
	    topics[j] = tstr; 
	  }
    }
  }


/* if we have help topics, it is worthwhile to
   create the dialog box */

  if (numtopics > 0) {

    if (!dialog) {
      Hv_VaCreateDialog(&dialog,
			Hv_TITLE,  "help",
			Hv_TYPE,   Hv_MODELESS,
			NULL);

      rowcol = Hv_DialogColumn(dialog, 10);  /* rowcol to hold all other widgets */

/* a horizontal rowcol to hold the lists and text */

      rc = Hv_VaCreateDialogItem(rowcol,
				 Hv_TYPE,         Hv_ROWCOLUMNDIALOGITEM,
				 Hv_SPACING,      10,
				 NULL);

/* now the list of topics */

      topicsList = Hv_VaCreateDialogItem(rc,
					 Hv_TYPE,             Hv_LISTDIALOGITEM,
					 Hv_LIST,             (void *)topics,
					 Hv_NUMITEMS,         numtopics,
					 Hv_NUMVISIBLEITEMS,  Hv_sMin(N_ROWS1, numtopics),
					 Hv_CALLBACK,         Hv_ChooseHelpTopic,
					 NULL);


/* now the scrolled text for the help itself */

      helpText = Hv_VaCreateDialogItem(rc,
				       Hv_TYPE,        Hv_SCROLLEDTEXTDIALOGITEM,
				       Hv_NUMROWS,     N_ROWS1,
				       Hv_NUMCOLUMNS,  N_COLS1,
				       NULL);

/* now the ok button */

      dummy = Hv_SimpleDialogSeparator(rowcol);
      rc = Hv_DialogRow(rowcol, 10);
      dummy = Hv_StandardDoneButton(rc, " close ");
      
      for (i = 0; i < numtopics; i++)
	Hv_Free(topics[i]);
    }


/* dialog has been created */

    Hv_DoDialog(dialog, NULL);

  }
  
}

#undef  N_COLS1
#undef  N_ROWS1


/*==============================================
 * OPEN GL RELATED SECTION OF GATEWAY
 *==============================================*/

#ifdef Hv_USEOPENGL  /* open gl 6 */

/*****************************************************************************
 ____       _                  ____ _ __        ___     _            _   
/ ___|  ___| |_ _   _ _ __    / ___| |\ \      / (_) __| | __ _  ___| |_ 
\___ \ / _ \ __| | | | '_ \  | |  _| | \ \ /\ / /| |/ _` |/ _` |/ _ \ __|
 ___) |  __/ |_| |_| | |_) | | |_| | |__\ V  V / | | (_| | (_| |  __/ |_ 
|____/ \___|\__|\__,_| .__/___\____|_____\_/\_/  |_|\__,_|\__, |\___|\__|
                     |_| |_____|                          |___/          
*****************************************************************************/

/**
 * Hv_XMSetupGLWidget
 * @purpose X Windows  specific version of Hv_SetupGLWidget.
 * @see The gateway routine Hv_SetupGLWidget. 
 */

void Hv_XMSetupGLWidget(Hv_View View,char *title) {
    int n;
    Arg      args[20];
    extern Widget Hv_mainw;


    if(Hv_3Dvi == NULL)
	Hv_Println("Visual error");
    
    
    View->frame3D = XtVaCreateWidget("doublebuf",
				     glwMDrawingAreaWidgetClass, Hv_Parent(Hv_canvas),
				     GLwNvisualInfo,             Hv_3Dvi,
				     NULL);
    
    Hv_AddCallback(View->frame3D,
		   GLwNinputCallback,
		   (Hv_CallbackProc)Hv_OGLInputCB,
		   (Hv_Pointer)View) ;
    
    Hv_OGLInitCB(View->frame3D,(Hv_Pointer) View,(Hv_Pointer) View);
    
    glXWaitX();
    glFlush();
    
    Hv_Flush();
    return;
} /* End Setup_BasicWin*/

#endif  /* open gl 6 */



/**
 * Hv_XMSet3DFrame
 * @purpose X Windows  specific version of Hv_Set3DFrame.
 * @see The gateway routine Hv_Set3DFrame. 
 */


void Hv_XMSet3DFrame(Hv_View View) {

    Hv_Rect   *hr;

    if (!Hv_ViewIs3D(View))
	return;
    
    if (View->frame3D == NULL)
	return;
    
    
    hr = Hv_GetViewHotRect(View);
    
    XtVaSetValues(View->frame3D,
		   Hv_NtopAttachment, XmATTACH_NONE,
		   Hv_NleftAttachment,XmATTACH_NONE,
		   Hv_Nx,      hr->left,
		   Hv_Ny,      hr->top,
		   Hv_Nwidth,  hr->width,
		   Hv_Nheight, hr->height,
		   NULL);


    if(Hv_IsHotRectOccluded(View)) {
    /* fill in the pixmap later */ 
	Hv_UnmanageChild(View->frame3D);
/**
    if(View->Pix3D != NULL)
      {
      Hv_Println(Restoring a view");
      Hv_RestoreSaveUnder(View->Pix3D,hr->left,hr->top,hr->width,hr->height);
      } 
**/
    }
    else {
	if (Hv_ViewIsVisible(View))
	    Hv_ManageChild(View->frame3D);
    }
  
}


/**
 * Hv_XMTextInsert
 * @purpose X Windows  specific version of Hv_TextInsert.
 * @see The gateway routine Hv_TextInsert. 
 */


void  Hv_XMTextInsert(Hv_Widget       w,
		      Hv_TextPosition pos,
		      char           *line) {
  XmTextInsert(w, pos, line);
}

/**
 * Hv_XMSetInsertionPosition
 * @purpose X Windows  specific version of Hv_SetInsertionPosition.
 * @see The gateway routine Hv_SetInsertionPosition. 
 */


void  Hv_XMSetInsertionPosition(Hv_Widget       w,
				Hv_TextPosition pos) {
    XmTextSetInsertionPosition(w, pos);
}

/**
 * Hv_XMShowPosition
 * @purpose X Windows  specific version of Hv_ShowPosition.
 * @see The gateway routine Hv_ShowPosition. 
 */


void  Hv_XMShowPosition(Hv_Widget       w,
				Hv_TextPosition pos) {
    XmTextShowPosition(w, pos);
}



/**
 * Hv_XMGetInsertionPosition
 * @purpose X Windows  specific version of Hv_GetInsertionPosition.
 * @see The gateway routine Hv_GetInsertionPosition. 
 */


Hv_TextPosition  Hv_XMGetInsertionPosition(Hv_Widget  w) {
    return XmTextGetInsertionPosition(w);
}

/**
 * Hv_XMUpdateWidget
 * @purpose X Windows  specific version of Hv_UpdateWidget.
 * @see The gateway routine Hv_UpdateWidget. 
 */


void  Hv_XMUpdateWidget(Hv_Widget w) {
  XmUpdateDisplay(w);
}


/**
 * Hv_XMGetLastPosition
 * @purpose X Windows  specific version of Hv_GetLastPosition.
 * @see The gateway routine Hv_GetLastPosition. 
 */


Hv_TextPosition  Hv_XMGetLastPosition(Hv_Widget w) {
  return  XmTextGetLastPosition(w);
}


/**
 * Hv_XMGetToggleButton
 * @purpose X Windows  specific version of Hv_GetToggleButton.
 * @see The gateway routine Hv_GetToggleButton. 
 */

Boolean	Hv_XMGetToggleButton(Hv_Widget btn) {
  return  XmToggleButtonGadgetGetState(btn);
}

/**
 * Hv_XMSetToggleButton
 * @purpose X Windows  specific version of Hv_SetToggleButton.
 * @see The gateway routine Hv_SetToggleButton. 
 */

void	Hv_XMSetToggleButton(Hv_Widget btn,
			   Boolean   State)

/* Set the state of a toggle button  True --> selected */

{
  Hv_VaSetValues(btn, Hv_Nset, State, NULL);
}


/**
 * Hv_XMPic8ToImage
 * @purpose X Windows  specific version of Hv_Pic8ToImage.
 * @see The gateway routine Hv_Pic8ToImage. 
 */

Hv_Image *Hv_XMPic8ToImage(byte *pic8,
			   int   wide,
			   int   high,
			   unsigned long **mycolors,
			   byte *rmap,
			   byte *gmap,
			   byte *bmap) {
    
/*
 * this has to do the tricky bit of converting the data in 'pic8'
 * into something usable for X.
 */


  int      i;
  unsigned long xcol;
  Hv_Image *xim;
  
  
  if (*mycolors == NULL) {
    *mycolors = (unsigned long *)(malloc(256*sizeof(unsigned long)));
    for (i = 0; i < 256; i++) {

	(*mycolors)[i] = Hv_NearestColor(rmap[i] << 8,
					 gmap[i] << 8,
					 bmap[i] << 8);
	}
    }
  
    xim = (Hv_Image *) NULL;
    
    if (!pic8) {
	Hv_Println("Error NULL image (A).");
	return xim;  /* shouldn't happen */
    }
  
    switch (Hv_colorDepth) {
	
    case 8: {
	byte  *imagedata, *ip, *pp;
	int   j, imWIDE, nullCount;
	
	nullCount = (4 - (wide % 4)) & 0x03;  /* # of padding bytes per line */
	imWIDE = wide + nullCount;
	
/* Now create the image data - pad each scanline as necessary */
	imagedata = (byte *) malloc((size_t) (imWIDE * high));
	if (!imagedata) {
	    Hv_Println("couldn't malloc imagedata");
	    return NULL;
	}

	pp =  pic8;
    
	
	for (i=0, ip=imagedata; i<high; i++) {
	    
	    for (j=0; j<wide; j++, ip++, pp++) *ip = (byte) (*mycolors)[*pp];
	    
	    for (j=0; j<nullCount; j++, ip++) *ip = 0;
	}
	
	xim = XCreateImage(Hv_display,
			   Hv_visual,
			   Hv_colorDepth,
			   ZPixmap,
			   0,
			   (char *) imagedata,
			   wide,
			   high, 
			   32,
			   imWIDE);
	if (!xim) {
	    Hv_Println("couldn't create xim!");
	    return NULL;
	}
    }
    break;
    
    
  case 4: {
      byte  *imagedata, *ip, *pp;
      byte *lip;
      int  bperline, half, j;
      
      xim = XCreateImage(Hv_display,
			 Hv_visual,
			 Hv_colorDepth,
			 ZPixmap,
			 0,
			 NULL, 
			 wide,
			 high,
			 8,
			 0);
      
      if (!xim) {
	  Hv_Println("couldn't create xim!");
	  return NULL;
      }
      
      bperline = xim->bytes_per_line;
      imagedata = (byte *) malloc((size_t) (bperline * high));
      if (!imagedata) {
	  Hv_Println("couldn't malloc imagedata");
	  return NULL;
      }
      
      xim->data = (char *) imagedata;
      
      
      pp =  pic8;
      
      if (xim->bits_per_pixel == 4) {
	  for (i=0, lip=imagedata; i<high; i++, lip+=bperline) {
	      
	      for (j=0, ip=lip, half=0; j<wide; j++,pp++,half++) {
		  xcol =  (*mycolors)[*pp] & 0x0f;
		  
		  if (ImageByteOrder(Hv_display) == LSBFirst) {
		      if (half&1) { *ip = *ip + (xcol<<4);  ip++; }
		      else *ip = xcol;
		  }
		  else {
		      if (half&1) { *ip = *ip + xcol;  ip++; }
		      else *ip = xcol << 4;
		  }
	      }
	  }
      }
      
      else if (xim->bits_per_pixel == 8) {
	  for (i=wide*high, ip=imagedata; i>0; i--,pp++,ip++) {
	      *ip =  (byte) (*mycolors)[*pp];
	  }
      }
      
      else {
	  Hv_Println("This display's too bizarre.  Can't create Image.");
	  return NULL;
      }
      
  }
  break;
      

    /*********************************/
      
  case 2: {  /* by M.Kossa@frec.bull.fr (Marc Kossa) */
    /* MSBFirst mods added by dale@ntg.com (Dale Luck) */
    /* additional fixes by  evol@infko.uni-koblenz.de 
       (Randolf Werner) for NeXT 2bit grayscale with MouseX */
    
      byte  *imagedata, *ip, *pp;
      byte *lip;
      int  bperline, half, j;
      
      xim = XCreateImage(Hv_display,
			 Hv_visual,
			 Hv_colorDepth,
			 ZPixmap,
			 0,
			 NULL, 
			 wide,
			 high,
			 8,
			 0);
      if (!xim) {
	  Hv_Println("couldn't create xim!");
	  return NULL;
      }
      
      bperline = xim->bytes_per_line;
      imagedata = (byte *) malloc((size_t) (bperline * high));
      if (!imagedata) {
	  Hv_Println("couldn't malloc imagedata");
	  return NULL;
      }
      
      xim->data = (char *) imagedata;
      
      pp =  pic8;
      
      if (xim->bits_per_pixel == 2) {
	  for (i=0, lip=imagedata; i<high; i++, lip+=bperline) {
	      
	      for (j=0, ip=lip, half=0; j<wide; j++,pp++,half++) {
		  xcol = ((*mycolors)[*pp]) & 0x03;
		  
		  if (xim->bitmap_bit_order == LSBFirst) {
		      if      (half%4==0) *ip  = xcol;
		      else if (half%4==1) *ip |= (xcol<<2);
		      else if (half%4==2) *ip |= (xcol<<4);
		      else              { *ip |= (xcol<<6); ip++; }
		  }
		  
		  else {  /* MSBFirst.  NeXT, among others */
		      if      (half%4==0) *ip  = (xcol<<6);
		      else if (half%4==1) *ip |= (xcol<<4);
		      else if (half%4==2) *ip |= (xcol<<2);
		      else              { *ip |=  xcol;     ip++; }
		  }
	      }
	  }
      }
      
      else if (xim->bits_per_pixel == 4) {
	  for (i=0, lip=imagedata; i<high; i++, lip+=bperline) {
	      
	      for (j=0, ip=lip, half=0; j<wide; j++,pp++,half++) {
		  xcol = ((*mycolors)[*pp]) & 0x0f;
		  
		  if (xim->bitmap_bit_order == LSBFirst) {
		      if (half&1) { *ip |= (xcol<<4);  ip++; }
		      else *ip = xcol;
		  }
		  
		  else { /* MSBFirst */
		      if (half&1) { *ip |= xcol;  ip++; }
		      else *ip = xcol << 4;
		  }
	      }
	  }
      }
      
      else if (xim->bits_per_pixel == 8) {
	  for (i=wide*high, ip=imagedata; i>0; i--,pp++,ip++) {
	      *ip = (byte) (*mycolors)[*pp];
	  }
      }
      
      else { 
	  Hv_Println("This display's too bizarre.  Can't create Image.");
	  return NULL;
      }
      
  }
  break;

  case 5:
    case 6: {
	byte  *imagedata, *ip, *pp;
	int  bperline;
	
	xim = XCreateImage(Hv_display,
			   Hv_visual,
			   Hv_colorDepth,
			   ZPixmap,
			   0,
			   NULL, 
			   wide,
			   high,
			   8,
			   0);
	if (!xim) {
	    Hv_Println("couldn't create xim!");
	    return NULL;
	}
	
	
	if (xim->bits_per_pixel != 8) {
	    Hv_Println("This display's too bizarre.  Can't create Image.");
	    return NULL;
	}
	
	
	bperline = xim->bytes_per_line;
	imagedata = (byte *) malloc((size_t) (bperline * high));
	if (!imagedata) {
	    Hv_Println("couldn't malloc imagedata");
	    return NULL;
	}
	
	xim->data = (char *) imagedata;
	
	pp = pic8;
	
	for (i=wide*high, ip=imagedata; i>0; i--,pp++,ip++) {
	    *ip = (byte) (*mycolors)[*pp];
	}
    }
    break;
      

  case 12:
  case 15:
  case 16: {
      unsigned short  *imagedata, *ip;
      byte  *pp;
      
      imagedata = (unsigned short *) malloc((size_t) (2*wide*high));
      if (!imagedata) {
	  Hv_Println("couldn't malloc imagedata");
	  return NULL;
      }
      
      xim = XCreateImage(Hv_display,
			 Hv_visual,
			 Hv_colorDepth,
			 ZPixmap,0,
			 (char *) imagedata,
			 wide,
			 high,
			 16,
			 0);
      if (!xim) {
	  Hv_Println("couldn't create xim!");
	  return NULL;
      }
      
      if (Hv_colorDepth == 12 && xim->bits_per_pixel != 16) {
	  Hv_Println("No code for this type of display (depth=%d, bperpix=%d)",
		     Hv_colorDepth, xim->bits_per_pixel);
	  return NULL;;
      }
      
      pp =  pic8;
      
      
      if (xim->byte_order == MSBFirst) {
	  for (i=wide*high, ip=imagedata; i>0; i--,pp++) {
	      *ip++ = (*mycolors)[*pp] & 0xffff;
	  }
      }
      else {   /* LSBFirst */
	  for (i=wide*high, ip=imagedata; i>0; i--,pp++) {
	      *ip++ = (unsigned short) ((*mycolors)[*pp]);
	  }
      }
  }
  break;

      
  case 24:
  case 32: {
      byte  *imagedata, *ip, *pp, *tip;
      int    j, do32;
      
      imagedata = (byte *) malloc((size_t) (4*wide*high));
      if (!imagedata) {
	  Hv_Println("couldn't malloc imagedata");
	  return NULL;
      }
      
      xim = XCreateImage(Hv_display,
			 Hv_visual,
			 Hv_colorDepth,
			 ZPixmap,
			 0,
			 (char *) imagedata,
			 wide,
			 high,
			 32,
			 0);
      if (!xim) {
	  Hv_Println("couldn't create xim!");
	  return NULL;
      }
      
      do32 = (xim->bits_per_pixel == 32);
      
      pp =  pic8;
      
      if (xim->byte_order == MSBFirst) {
	  for (i=0, ip=imagedata; i<high; i++) {
	      for (j=0, tip=ip; j<wide; j++, pp++) {
		  xcol = (* mycolors)[*pp];
		  
		  if (do32) *tip++ = 0;
		  *tip++ = (xcol>>16) & 0xff;
		  *tip++ = (xcol>>8) & 0xff;
		  *tip++ =  xcol & 0xff;
	      }
	      ip += xim->bytes_per_line;
	  }
      }
      
      else {  /* LSBFirst */
	  for (i=0, ip=imagedata; i<high; i++) {
	      for (j=0, tip=ip; j<wide; j++, pp++) {
		  xcol = (* mycolors)[*pp];
		  
		  *tip++ =  xcol & 0xff;
		  *tip++ = (xcol>>8) & 0xff;
		  *tip++ = (xcol>>8) & 0xff;
		  *tip++ = (xcol>>16) & 0xff;
		  if (do32) *tip++ = 0;
	      }
	      ip += xim->bytes_per_line;
	  }
      }
  }
  break;
  
  
    default: 
	Hv_Println("no code to handle this display type (%d bits deep)",
		   Hv_colorDepth);
	break;
    }
    
    
    return(xim);
}

/**
 * Hv_XMGrabPointer
 * @purpose X Windows specific version of Hv_GrabPointer.
 * @see The gateway routine Hv_GrabPointer. 
 */

void            Hv_XMGrabPointer() {

  
  Hv_XMUngrabPointer();


  XGrabPointer(Hv_display,
	       Hv_trueMainWindow,
	       True,
	       ButtonPressMask | ButtonReleaseMask,
	       GrabModeAsync,
	       GrabModeAsync,
	       None,
	       None,
	       CurrentTime);
}


/**
 * Hv_XMUngrabPointer
 * @purpose X Windows specific version of Hv_UngrabPointer.
 * @see The gateway routine Hv_UngrabPointer. 
 */

 void            Hv_XMUngrabPointer() {
	XUngrabPointer(Hv_display, CurrentTime);
}




/**
 * Hv_XMGetNextFile
 * @purpose X Windows specific version of Hv_GetNextFile.
 * @see The gateway routine Hv_GetNextFile. 
 */
 
Boolean  Hv_XMGetNextFile(DIR *dp,
			char *dirname,
			char *extension,
			FILE **fp,
			char *fname) {

   struct dirent   *dir;
   char            *rfname;
   char            *sstr;
   
   
   *fp = NULL;

   dir = readdir(dp);

   if (dir == NULL)
     return False;
  

/* skip removed files */

    if (dir->d_ino == 0)
      return True;

/*    if (strstr(dir->d_name, extension) != 0) { */
    
    if (strcmp((dir->d_name) + strlen(dir->d_name) - strlen(extension), extension) == 0) {


/* rfname is "real file name", may have to prepend "dirname" and "/" */

      rfname = (char *)Hv_Malloc(strlen(dir->d_name) + strlen(dirname) + 5);
      if ((dir->d_name[0] != '.') && (dir->d_name[0] != '/')) {
        strcpy(rfname, dirname);
	strcat(rfname, "/");
        strcat(rfname, dir->d_name);
      }
      else
        strcpy(rfname, dir->d_name);

/* remove file extension */

      strcpy(fname, dir->d_name);
      sstr = strstr(fname, extension);
      if (sstr != NULL)
        sstr[0] = '\0';

      *fp = fopen(rfname, "r");

      Hv_Free(rfname);
    }  /* end of matched extension */
      
      
   return True;
}

/**
 * Hv_XMOpenDirectory
 * @purpose X Windows specific version of Hv_OpenDirectory.
 * @see The gateway routine Hv_OpenDirectory. 
 */

DIR  *Hv_XMOpenDirectory(char *dname) {
  DIR  *dp;
  char *tstr;
  char *tname;
  
  if ((dp = opendir(dname)) == NULL) {

    if (dontPromptDir)
      return NULL;

    tstr = (char *)Hv_Malloc(120 + strlen(dname));
    sprintf(tstr,  "Can't find %s. Do you want to look for it?",  dname);
    
    if (Hv_Question(tstr)) {
      sprintf(tstr,  "Please locate %s.",  dname);
      tname = Hv_DirectorySelect(tstr, "*");

      if (tname == NULL) {
	Hv_Free(tstr);
	return NULL;
      }

      dp = opendir(tname);
      Hv_Free(tname);
    }  /* end of yes response */


    Hv_Free(tstr);
  } /*end dp == NULL*/
  return dp;
}

/**
 * Hv_XMCloseDirectory
 * @purpose X Windows specific version of Hv_CloseDirectory.
 * @see The gateway routine Hv_OpenDirectory. 
 */

int Hv_XMCloseDirectory(DIR *dp) {
  return closedir(dp);
}

/*==========================================
 * LOCAL (STATIC) ROUTINES
 *===========================================*/


/**
 * Hv_PauseTimeOut 
 * @purpose  Local routime that deals with pauses.
 * @param    xtpr    passed by the event handler - ignored
 * @param    id      passed by the event handler - ignored
 * @local
 */


static void Hv_PauseTimeOut(Hv_Pointer    xtptr,
			    Hv_IntervalId  *id) {
  Hv_pauseFlag = False;
}


/**
 * Hv_AddEventHandlers 
 * @purpose Adds X Windows event handlers to the canvas.
 * @local
 */

static void	Hv_AddEventHandlers(void) {
    Hv_AddEventHandler(Hv_canvas, ExposureMask,      (Hv_EventHandler)Hv_XMHandleExpose,   NULL);
    Hv_AddEventHandler(Hv_canvas, KeyReleaseMask,    (Hv_EventHandler)Hv_XMHandleKeyRelease, NULL);
    Hv_AddEventHandler(Hv_canvas, PointerMotionMask, (Hv_EventHandler)Hv_PointerMotion,    NULL);
    Hv_AddEventHandler(Hv_canvas, ButtonReleaseMask, (Hv_EventHandler)Hv_ButtonRelease,    NULL); 
    Hv_AddEventHandler(Hv_canvas, ButtonPressMask,   (Hv_EventHandler)Hv_ButtonPress,      NULL); 
}



/**
 * Hv_XMHandleExpose 
 * @purpose Handles exposures on X Windows   
 * @param      w  The widget that was exposed (not used)
 * @parameter  client_data  (not used)
 * event       The exposure event
 * @local
 */

static void Hv_XMHandleExpose(Hv_Widget    w,
			      Hv_Pointer   client_data,
			      Hv_XEvent    *event) {

/* Handles exposure events by accumulating them unil there are no more,
  the updating all affected Views*/

  static Region     TheUpdateRegion = NULL;
  Hv_Pixmap         pmap;  
  Window            twind;

  
  if (Hv_firstExposure) {
    Hv_firstExposure = False;
    if (Hv_TileDraw != NULL) {
      pmap = Hv_CreatePixmap(Hv_tileX, Hv_tileY);
      twind = Hv_mainWindow;
      Hv_mainWindow = pmap;
      Hv_TileDraw();
      Hv_mainWindow = twind;
      Hv_TileMainWindow(pmap);
      Hv_FreePixmap(pmap);

/* last possible user initialization */
      
      if (Hv_FinalUserInit != NULL)
	  Hv_FinalUserInit();
      
      return;
    }
  }
  
  
/* if no region yet, then create one */
	
  if (!TheUpdateRegion)
      TheUpdateRegion = Hv_CreateRegion();
  
  XtAddExposureToRegion(event, TheUpdateRegion);    /* add region to clip mask */
  
/* if not last expose event, then exit */	

  if (event->xexpose.count != 0)
      return;


  Hv_handlingExposure = True;
  Hv_UpdateViews(TheUpdateRegion);	           /* redraw the items from the View list */
  Hv_excludeFromExposure = NULL;
  Hv_handlingExposure = False;
  if (Hv_showBalloons && Hv_balloonUp)
      Hv_ShowBalloon();                              /* see if balloon help needs updating */
  Hv_DestroyRegion(TheUpdateRegion);                 /*free region memory*/
  TheUpdateRegion = NULL;
}


/**
 * Hv_XMHandleKeyRelease 
 * @purpose Handles key releases. We don't
 * use them much, except for "hidden" features,
 * such as dumping a list of items.
 * @param      w  The widget that was exposed (not used)
 * @parameter  client_data  (not used)
 * event       The key event
 * @local
 */


static void Hv_XMHandleKeyRelease(Hv_Widget    w,
				  Hv_Pointer client_data,
				  XKeyEvent *event) {

/* Handles keyboard presses.  */

  KeySym    keysym;
  char      buffer[20];
  int       num_bytes = 20;
  short     x, y;

  Hv_XMUngrabPointer();


  x = event->x;
  y = event->y;

  XLookupString(event, buffer, num_bytes, &keysym, NULL);

  switch (keysym) {

  case XK_F1:
      Hv_HandleFunctionKey(x, y,1);
      break;
    
  case XK_F2:
      Hv_HandleFunctionKey(x, y,2);
      break;
    
  case XK_F3:
      Hv_HandleFunctionKey(x, y,3);
      break;
    
  case XK_F4:
      Hv_HandleFunctionKey(x, y,4);
      break;
    
  case XK_F5:
      Hv_HandleFunctionKey(x, y,5);
      break;

  case XK_F6:
      Hv_HandleFunctionKey(x, y,6);
      break;
    
  case XK_F7:
      Hv_HandleFunctionKey(x, y,7);
      break;
        
  case XK_F8:
      Hv_HandleFunctionKey(x, y,8);
      break;
    
  case XK_F9:
      Hv_HandleFunctionKey(x, y,9);
      break;
    
  }
}


/**
 * Hv_CanvasResizeCB 
 * @purpose  Callback when main window is resized.
 * @local
 */


static void Hv_CanvasResizeCB() {

  int          dummy1;
  unsigned int dummy2;
  Hv_Window    root;
  unsigned int mw, mh;
  Hv_Dimension crt;

  XGetGeometry(Hv_display,
	       Hv_mainWindow,
	       &root,
	       &dummy1,
	       &dummy1,
	       &mw,
	       &mh,
	       &dummy2,
	       &Hv_colorDepth);

  Hv_mainWidth =  (short)mw;
  Hv_mainHeight = (short)mh;

/* fix the canvas "super rect " which will be used for World <--> Local */

  XtVaGetValues(Hv_menuBar, Hv_Nheight, &crt, NULL);
  Hv_canvasRect.top = (short)crt;
  Hv_canvasRect.left = 0;
  Hv_canvasRect.right = Hv_mainWidth - 1;
  Hv_canvasRect.width = Hv_mainWidth;
  Hv_canvasRect.bottom = Hv_mainHeight - 1;
  Hv_canvasRect.height = Hv_mainHeight - Hv_canvasRect.top;

  Hv_UpdateVirtualView();
  Hv_FullClip();
}


/**
 * Hv_MatchVisual 
 * @purpose   Tries to find a visual with the correct properties
 * (depth and class)
 * @param    dpy       The display
 * @param    scr       The screen
 * @param    defdepth  desired depth
 * @param    defvis    default visual
 * @param    vclass    desired vusual class
 * @local
 */


static Boolean Hv_MatchVisual(Display *dpy,
			      int      scr,
			      int      defdepth,
			      Visual  *defvis,
			      int      vclass) {
    XVisualInfo    visinfo;
    
    if (XMatchVisualInfo(dpy, scr, defdepth, vclass, &visinfo))
	if (defvis == visinfo.visual) {
	    Hv_visClass = vclass;
	    return True;
	}
    
    return False;
}




/**
 * Hv_ColorizeCursors 
 * @purpose  Colorizes cursors so they show up better
 * @param    dpy    The display
 * @local
 */


static void Hv_ColorizeCursors(Hv_Display *dpy) {
    Hv_Color  black, yellow, Dummy;


  XLookupColor(dpy, Hv_colorMap, "Black",  &black,  &Dummy);
  XLookupColor(dpy, Hv_colorMap, "Yellow", &yellow, &Dummy);

  XRecolorCursor(dpy, Hv_crossCursor,   &black, &yellow);
  XRecolorCursor(dpy, Hv_waitCursor,    &black, &yellow);
  XRecolorCursor(dpy, Hv_dotboxCursor,  &black, &yellow);

  XFlush(dpy);
}

/**
 * Hv_StuffColor 
 * @purpose Take the RGB components and stuff them into a color structure 
 * For read only colormaps, attempt to allocate a read only cell.
 * For ReadWrite maps, the cells have been allocated and the
 * pixels are already in Hv_colors
 * @param name   If not NULL, will try looking up the named color
 * @param R      red component
 * @param G      green component
 * @param B      blue component
 * @param i      index to Hv_Colors array where color is "stuffed"
 * @param scale  If true, multiply by 256
 * @local
 */


static void  Hv_StuffColor(char *name,
			   unsigned int R,
			   unsigned int G,
			   unsigned int B,
			   int i,
			   Boolean scale) {


  Hv_Color       color, dummy;
  int            status;
  Display	*dpy = XtDisplay(Hv_toplevel);

  if (scale) {
    R = R << 8;
    G = G << 8;
    B = B << 8;
  }

  color.red =   R;
  color.green = G;
  color.blue =  B;
  color.flags = DoRed | DoGreen | DoBlue;

  if (Hv_readOnlyColors) {
    if (name)
      status = XAllocNamedColor(dpy, Hv_colorMap, name, &color, &dummy);
    else
      status = XAllocColor(dpy, Hv_colorMap, &color);
    
    if (status != 0) {
      Hv_colors[i] = color.pixel;
    }
    else {   /*FAILURE */
      Hv_Println("Hv Error: cannot allocate required color.");
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



/**
 * Hv_FillGaps 
 * @purpose  Fills in "gaps" in the colormap allowing us to
 * create "gradients". If start < 0, fills in place between cstart and cend,
 * otherwise starts at "start" and creates nstep transtional colors.
 * @param cStart starting color index (if start < 0)
 * @param cEnd ending color index (if start < 0)
 * @param start alternative start index (if >= 0)
 * @param nstep number of steps of transition (if start >= 0)
 * @local
 */


static void Hv_FillGaps(short cStart,
			short cEnd,
			short start,
			short nstep) {
  int        Ro, Go, Bo;
  int        Rf, Gf, Bf;
  int        R, G, B;
  short      stindex;
  Hv_Color   color1, color2;
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

  Hv_QueryColor(dpy, Hv_colorMap, &color1);
  Hv_QueryColor(dpy, Hv_colorMap, &color2);

  Ro = (int)(color1.red);  Go = (int)(color1.green);  Bo = (int)(color1.blue);
  Rf = (int)(color2.red);  Gf = (int)(color2.green);  Bf = (int)(color2.blue);

  Rdel = (Rf - Ro)/(steps + 1);
  Gdel = (Gf - Go)/(steps + 1);
  Bdel = (Bf - Bo)/(steps + 1);

  for (i = 1; i <= steps; i++) {
    R = Ro + i*Rdel;
    G = Go + i*Gdel;
    B = Bo + i*Bdel;

    Hv_StuffColor(NULL, 
		  (unsigned int)R, 
		  (unsigned int)G, 
		  (unsigned int)B, 
		  cStart+i,
		  False); 
  }
}

#define   UPFRONTCOLORS   80


/**
 * Hv_CheckColorMap 
 * @purpose Checks if we can use the default
 * colormap or if we must create our own
 * @local
 */


static void Hv_CheckColorMap(void) {
  int           status, status2;
  Display      *dpy = XtDisplay(Hv_toplevel);
  int	        scr = DefaultScreen(dpy);
  Colormap      defcmap;
  Hv_Color      tempcolors[UPFRONTCOLORS];
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
    Hv_Println("Hv will create a new virtual colormap.");
    Hv_colorMap = XCreateColormap(dpy, DefaultRootWindow(dpy),
				  DefaultVisual(dpy,scr), AllocNone);

    status  = XAllocColorCells(dpy, Hv_colorMap, True, NULL, 0,
			      pixels, upfrontcolors);

    status2 = XAllocColorCells(dpy, Hv_colorMap, True, NULL, 0,
			      Hv_colors, Hv_numColors);

    if ((status == 0) || (status2 == 0)) {
      Hv_Println("Hv failed to allocate colors in its own colormap.");
      exit (0);
    }

    for (i = 0; i < upfrontcolors; i++) {
      tempcolors[i].pixel = pixels[i];
      tempcolors[i].flags = DoRed | DoGreen | DoBlue;
    }

    XQueryColors(dpy, defcmap, tempcolors, upfrontcolors);
    XStoreColors(dpy, Hv_colorMap, tempcolors, upfrontcolors);

    XtVaSetValues(Hv_toplevel, Hv_Ncolormap, Hv_colorMap, NULL);
    XtVaSetValues(Hv_form, Hv_Ncolormap, Hv_colorMap, NULL);
    
  }
  else
    Hv_Println("Hv will use the default colormap.");
}



/**
 * Hv_FileSelectResponse
 * @purpose Callback for widgets on the file selector
 * @param   w      The widget registering the callback
 * @param   dummy  Not used
 * @param   cbs    Will contain the "reason" for the callback
 * @local
 */


static void Hv_FileSelectResponse(Widget w,
				  Hv_Pointer cdata,
				  XmFileSelectionBoxCallbackStruct *cbs) {

  XmStringContext   context;      /* string context */
  XmStringCharSet   charset;      /* string charset */
  char              *text;        /* text of file name selected */
  XmStringDirection direction;    /* direction of string (LtoR or RtoL) */
  Boolean           separator;    /* is it a separator */

  Hv_Widget         dialog = Hv_GetOwnerDialog(w);
  Hv_DialogData     ddata;

  ddata = Hv_GetDialogData(dialog);
  if(ddata == NULL) {
    Hv_Println("Error in Hv_FileSelectResponse");
    return;
  }

  switch (cbs->reason) {
  case XmCR_OK:
    ddata->answer = Hv_OK;
    if (generic_filename)
      Hv_Free(generic_filename);
    
    XmStringInitContext    (&context, cbs->value);  /* get the context */
    XmStringGetNextSegment (context, &text, &charset, &direction, &separator);  /* get the text */
    
    XmStringFreeContext(context);
    Hv_InitCharStr(&generic_filename, text);
    free(text);
    break;
    
  case XmCR_CANCEL:
    ddata->answer = Hv_CANCEL;
    break;
  }
}


/**
 * Hv_TextFromXmString
 * @purpose  To get the "text" out of a Motif string
 * @return   Simple text extracted from the Motif string
 * @local
 */

static char *Hv_TextFromXmString(XmString xmst) {
  XmStringContext   context;      /* string context */
  XmStringCharSet   charset;      /* string charset */
  char              *text, *tstr;        /* text of file name selected */
  XmStringDirection direction;    /* direction of string (LtoR or RtoL) */
  Boolean           separator;    /* is it a separator */

  if (xmst == NULL)
      return NULL;   /* thanks Jack */

  XmStringInitContext(&context, xmst);  /* get the context */
  XmStringGetNextSegment(context, &tstr, &charset, &direction, &separator);  /* get the text */
  XmStringFreeContext(context);

  Hv_InitCharStr(&text, tstr);
  free(tstr);
  return text;
}

/**
 * Hv_FontReport
 * @purpose Diagnostic report about font load status
 * @param fnt       The font structure
 * @param fname     The font name
 * @local
 */

static void Hv_FontReport(Hv_Font **fnt,
			  char         *fname) {

  char   *message;

  if ((*fnt) == NULL) {
    message = (char *)Hv_Malloc(132);
    strcpy(message, "did NOT find ");
    strcat(message, fname);
    Hv_Println("%s", message);
    Hv_Free(message);
    *fnt = fontfixed;
  }

}

/**
 * Hv_AddFont
 * @purpose Make a new font avaliable
 * @param   The font index
 * @return  The Hv_Font corresponding to this font
 */

static Hv_Font  *Hv_AddFont(short font) {
  Hv_Font      *xfont;
  char         *substr;

  xfont  = XLoadQueryFont(Hv_display,  Hv_xFontNames[font]);

/* try a fix if the font is not found */

  if (xfont == NULL) {
    Hv_Println("could not find font: %s", Hv_xFontNames[font]);
    if ((substr = strstr(Hv_xFontNames[font], "11")) != NULL)
      substr[1] = '0';
    else if ((substr = strstr(Hv_xFontNames[font], "12")) != NULL)
      substr[1] = '3';
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
    Hv_Println("trying a substitute: %s", Hv_xFontNames[font]);
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


/**
 * Hv_TECleanup
 * @purpose Cleanup for the text entry widget.
 * @local
 */

static void Hv_TECleanup(void) {
  Hv_DanglingTextItemCallback = NULL ;
  Hv_UnmanageChild (themtw) ;
  XmFontListFree(fontlist);

/* next fix submitted by Anton Mezger from PSI */

  XtRemoveAllCallbacks(themtw, Hv_activateCallback);
}


/**
 * Hv_TEDoReturn
 * @purpose Callback from "return" in the Text Entry widget
 * Should ONLY be used for the motif editor widget.
 * Basically, clean up the screen by removing the motif 
 * widget; get the text out of it and pop it into the
 * Hv_text item.
 *
 *  driven on return key press.
 * @param   w    The widget
 * @param   dummy unused
 * @param   cbs   The callback data
 * @local
 */


static void Hv_TEDoReturn(Hv_Widget      w,
			  Hv_Pointer     dummy,
			  XmTextVerifyCallbackStruct *cbs) {
  char *t;

  t = Hv_GetString(w) ;
  Hv_SetItemText (Hv_activeTextItem, t);
  Hv_Free (t) ;
  Hv_TECleanup();

/* next addtion submitted by Anton Mezger from PSI:
   a user routine that can be called here */

  if (Hv_AfterTextEdit != NULL)
    Hv_AfterTextEdit(Hv_activeTextItem);

}

/**
 * Hv_TEMouseDestroy
 * @purpose Part of the cleanup of using the text entry widget
 * @local
 */

static void Hv_TEMouseDestroy (void)  {
  char *t, *nt;
  int i, j=0, len ;
  
  t = Hv_GetString(themtw) ;
  len = strlen (t) ;
  for (i=0, j=0; i<len ; i++)
    if (t[i] == '\012') {
      j++ ;
    }
  nt = Hv_Malloc(len+j+1) ;
  for (i=0, j=0; i<len; i++) {
    if (t[i] == '\012') {
      nt[j++] = '\\' ; 
      nt[j++] = 'n' ;
    } else
      nt [j++] = t[i] ;
  }
  nt[j] = 0x00 ;
  Hv_SetItemText (Hv_activeTextItem, nt);
  Hv_Free(t) ;
  Hv_Free(nt) ;
  
  Hv_TECleanup();

/* next addtion submitted by Anton Mezger from PSI:
   a user routine that can be called here */

  if (Hv_AfterTextEdit != NULL)
    Hv_AfterTextEdit(Hv_activeTextItem);
}


#ifdef Hv_USEOPENGL   /* open gl 7*/
/*-------------------------------------------------------------------*/
/*       _       _ _    ____ ____                                    */
/*      (_)_ __ (_) |_ / ___| __ )                                   */
/*      | | '_ \| | __| |   |  _ \                                   */
/*      | | | | | | |_| |___| |_) |                                  */
/*      |_|_| |_|_|\__|\____|____/                                   */
/*-------------------------------------------------------------------*/

static void Hv_OGLInitCB(Widget w,
			 Hv_Pointer client_data,
			 Hv_Pointer call_data) {

    int int_client;
    short data;
    Arg args[1];
    Hv_View View;

    View = (Hv_View) call_data;

    int_client=(int)client_data;

   if ( Hv_glxContext == NULL)
     {
     Hv_glxContext = glXCreateContext(XtDisplay(Hv_toplevel), Hv_3Dvi, 0, GL_FALSE);
     glDepthRange(0,1);
     glEnable(GL_DEPTH_TEST);
     glMatrixMode(GL_MODELVIEW);
     }
   View->glxContext = Hv_glxContext;
   return;
}  /* End initCB  */


/*-------------------------------------------------------------------------*/
/*          _                   _    ____ ____                             */
/*         (_)_ __  _ __  _   _| |_ / ___| __ )                            */
/*         | | '_ \| '_ \| | | | __| |   |  _ \                            */
/*         | | | | | |_) | |_| | |_| |___| |_) |                           */
/*         |_|_| |_| .__/ \__,_|\__|\____|____/                            */
/*                 |_|                                                     */
/*-------------------------------------------------------------------------*/


static void Hv_OGLInputCB(Widget w,
			  Hv_Pointer client_data, 
			  GLwDrawingAreaCallbackStruct *call_data) {


   Hv_View View;


   View = (Hv_View) client_data;

   if(View->InputRoutine3d != 0)
       View->InputRoutine3d(w,View,call_data);
   return;
}   /* End inputCB */
#endif /* end open gl 7 */


static void Hv_ChooseHelpTopic(Widget w,
			       Hv_Pointer client_data,
			       XmListCallbackStruct *cbs) {

/* callback for selection of a help topic */
 
  char *choice;
  char *text;


  XmStringGetLtoR(cbs->item, XmSTRING_DEFAULT_CHARSET, &choice);
  Hv_ReadHelp(choice, &text);   /*mallocs space for text */

  Hv_SetString(helpText, text);
  XtFree(choice);
  Hv_Free(text);
}


/* ---- Hv_ReadHelp ------- */

static void Hv_ReadHelp(char *choice,
			char **text) {
  char *RecName;
  char *line;
  int  numlines, numchars;

  line = (char *)Hv_Malloc(Hv_LINESIZE);
  RecName  = (char *)Hv_Malloc(strlen(choice) + 8);
  strcpy(RecName, "[HELP_");
  strcat(RecName, choice);

/* read the appropriate help blurb */

  if (Hv_FindRecord(helpfp, RecName, True, line)) {
    Hv_NextRecordSize(helpfp, "[HELP_", "[END]", &numlines, &numchars);
    Hv_FindRecord(helpfp, RecName, True, line);
    *text = (char *)Hv_Malloc(numchars);
    Hv_ReadHelpInfo(helpfp, *text);
  }
  else
    Hv_Println("could not find record %s", choice);

  

  Hv_Free(RecName);
  Hv_Free(line);
}


/*--------- Hv_CheckDefault ------*/

static void    Hv_CheckDefault(int *param,
			       int deflt) {

/* changes param to default if it was = Hv_DEFAULT*/

  if (*param == Hv_DEFAULT)
    *param = deflt;
}

/* ---- Hv_ReadHelpInfo --- */

static void Hv_ReadHelpInfo(FILE *fp,
			    char *text) {
  char            *line;      /*input line*/
  char            *RECNAME_END      = "[END]";
  Boolean         found;

  line = (char *) Hv_Malloc(Hv_LINESIZE);

  strcpy (text, " ");

/*keep in mind that "C" forces us to start indices at zero*/
  
  found = False;
  while (fgets(line, Hv_LINESIZE, fp) && (!found)) {
    found = (strstr(line, RECNAME_END) != NULL);
    if (!found)
      strcat(text, line);
  }

  Hv_Free(line);
}


static Hv_Widget Hv_CreateActionButton(char          *label,
				       int            i,
				       int            na,
				       Hv_Widget      aa,
				       Hv_Pointer     data,
				       Hv_FunctionPtr cb) {

    Hv_Widget w;

    w = XtVaCreateManagedWidget(label,
				 Hv_PushButtonClass,    aa,
				 Hv_NleftAttachment,    i ? XmATTACH_POSITION : XmATTACH_FORM,
				 Hv_NleftPosition,      TIGHTNESS*i,
				 Hv_NtopAttachment,     XmATTACH_FORM,
				 Hv_NbottomAttachment,  XmATTACH_FORM,
				 Hv_NrightAttachment,   i != na-1 ? XmATTACH_POSITION : XmATTACH_FORM,
				 Hv_NrightPosition,     TIGHTNESS*i + (TIGHTNESS-1),
				 Hv_NshowAsDefault,     False,
				 Hv_NdefaultButtonShadowThickness, 0,
				 NULL);

  if (cb != NULL)
    Hv_AddCallback(w,
		   Hv_activateCallback,
		   (Hv_CallbackProc)cb,
		   data);



    return w;

}


/* ------ Hv_SetColor --- */

static void Hv_SetColor (Hv_Widget w,
			 int       cnumber,
			 XEvent    *event)
{
    Hv_newColor = (short) cnumber;
    Hv_SetWidgetBackgroundColor(Hv_newColorLabel, Hv_newColor);
}


static void Hv_WMClose(Widget               w,
		       Hv_Pointer           client_data,
		       XmAnyCallbackStruct *cbs) {

    Hv_Widget dialog = (Hv_Widget)client_data;
    Hv_DialogData  ddata;

    if (dialog == NULL)
	return;

    
    ddata = Hv_GetDialogData(dialog);

    if (ddata != NULL)
	ddata->answer = Hv_OK;

}

/*-------- Hv_NullCallback ------*/

static void Hv_NullCallback(Hv_Widget w)

{
}



/*---------------- Hv_CreateDialog ----------------- */

static Hv_Widget Hv_CreateDialog(char         *title,
			      unsigned char dstyle,
			      int           ignoreclose)

/* if ignoreclose != 0, set attribute so that the
   close from the motif window menu is ignored */

{
  Hv_Widget          w, shell;
  char           *tstr;
  String          es = "  ";
  Arg	          args[10];
  int		  n = 0;
  Atom            WM_DELETE_WINDOW;

  Hv_SetArg(args[n], Hv_NmarginHeight,      4);	     n++;
  Hv_SetArg(args[n], Hv_NmarginWidth,       6);	     n++;
  Hv_SetArg(args[n], Hv_NdialogStyle,       dstyle);   n++;
  Hv_SetArg(args[n], Hv_NnoResize,          True);     n++;
  Hv_SetArg(args[n], Hv_NautoUnmanage,      False);    n++;


  Hv_SetArg(args[n], Hv_NdeleteResponse, XmDO_NOTHING);

  w = XmCreateBulletinBoardDialog(Hv_toplevel, es,
				  (ArgList)args, (Cardinal)n);

/* add a callback for the wm close */

  shell = Hv_Parent(w);
  WM_DELETE_WINDOW = XmInternAtom(Hv_display, "WM_DELETE_WINDOW", False);
  XmAddWMProtocolCallback(shell, WM_DELETE_WINDOW, (Hv_CallbackProc)Hv_WMClose, (Hv_Pointer)w); 

  
  XtVaSetValues(w, Hv_Ncolormap, Hv_colorMap, NULL);
  
  if (title != NULL) {
    Hv_InitCharStr(&tstr, title);
    XtVaSetValues(Hv_Parent(w), Hv_Ntitle, tstr, NULL);
    Hv_Free(tstr);
  }

  return w;
}

/* ------ Hv_SetWidgetPattern --- */

static void Hv_SetWidgetPattern (Hv_Widget w,
	   	                 int       pnumber,
		                 XEvent    *event) {
    newpattern = (short) pnumber;
    Hv_ChangeLabelPattern(newpatternlabel, newpattern);
}




/**
 * Hv_ManageMenuItem  When adding to some standard menus, rearrange some
 * items. For example, when adding to the Action menu,
 * make sure that the last two items are always a separator
 * and Quit (generic)
 * @param      menu       The parent menu
 * @param      w          The menu item
 * @local
 */


static void   Hv_ManageMenuItem(Hv_Widget menu,
			  Hv_Widget w) {


     if (!Hv_userMenu) {
	 Hv_ManageChild(w);
	 return;
     }

     if (menu == Hv_actionMenu) {
	 Hv_UnmanageChild(Hv_quitSepItem);
	 Hv_UnmanageChild(Hv_quitItem);
	 Hv_DestroyWidget(Hv_quitSepItem);
	 Hv_DestroyWidget(Hv_quitItem);
	 Hv_ManageChild(w);
	 
	 Hv_userMenu = False;
	 
	 Hv_quitSepItem = Hv_VaCreateMenuItem(Hv_actionMenu, Hv_TYPE, Hv_SEPARATOR, NULL); 
	 
	 Hv_quitItem = Hv_VaCreateMenuItem(Hv_actionMenu,
					   Hv_LABEL,           "Quit...",
					   Hv_ID,              Hv_QUIT,
					   Hv_CALLBACK,        Hv_DoPopup2AndActionMenu,
					   Hv_ACCELERATORTYPE, Hv_ALT,
					   Hv_ACCELERATORCHAR, 'Q',
					   NULL);
	 Hv_userMenu = True;
     }
     else
	 Hv_ManageChild(w);
     
}


/*----- Hv_PopupMenuShell ------*/

/*

void  Hv_PopupMenuShell(Widget        w,
			XButtonPressedEvent *event,
			String        *params,
			Cardinal      *num_params)


{
    fprintf(stderr, "In deprecated Hv_popupMenuShell\n");
}
*/

/*------- Hv_PopdownMenuShell ------*/

/*
void   Hv_PopdownMenuShell(void)

{

}
*/

/**
 * Hv_XMDrawBackgroundItems
 * @purpose Draw hotrect items that have their Hv_INBACKGROUND draw control bit set.
 * @param items    List of items (probably ia view's item list).
 * @param region   Clipping region
 * @local 
 */


static void Hv_XMDrawBackgroundItems(Hv_ItemList items,
				   Hv_Region region) {
  Hv_View     View;
  Hv_Item     temp;
  static Hv_Region   hrrgn = NULL;
  static Hv_Region   hrbrgn = NULL;
  Hv_Rect     hrect;

  Boolean     drawingchildren;

  if ((items == NULL) || (items->first == NULL))
    return;

  View = items->first->view;

  drawingchildren = (View->items != items);

  if (!drawingchildren) {
    drawingchildren = (View->items != items);
    
    Hv_CopyRect(&hrect, View->hotrect);
    Hv_ShrinkRect(&hrect, 1, 1);
  
    hrrgn  =  Hv_RectRegion(&hrect);
    hrbrgn =  Hv_RectRegion(View->hotrectborderitem->area);
    Hv_drawingOffScreen = True;

/* if there really was a region, don't draw outside of it */

    if (region != NULL) {
      Hv_IntersectRegionWithRegion(&hrrgn, region);
      Hv_IntersectRegionWithRegion(&hrbrgn, region);
    }
  }

  for (temp = items->first; temp != NULL; temp = temp->next)
    
    if (Hv_drawNBItemsToo || Hv_CheckItemDrawControlBit(temp, Hv_INBACKGROUND))
      if ((temp->domain == Hv_INSIDEHOTRECT) || (temp == View->hotrectborderitem)) {
	
	if (region == NULL) {
	  
	  if (temp == View->hotrectborderitem)
	    Hv_DrawItem(temp, hrbrgn); 
	  else
	    Hv_DrawItem(temp, hrrgn);
	}
	else {
	  if (temp == View->hotrectborderitem)
	    Hv_MaybeDrawItem(temp, hrbrgn);
	  else
	    Hv_MaybeDrawItem(temp, hrrgn);
	}
	
	if (temp->children != NULL)
	  Hv_XMDrawBackgroundItems(temp->children, region);
      }

/* off screen user drawing? */

  if (!drawingchildren) {
    if (View->offscreenuserdraw != NULL)
      View->offscreenuserdraw(View, hrrgn);

    Hv_drawingOffScreen = False;
    Hv_DestroyRegion(hrrgn);
    Hv_DestroyRegion(hrbrgn);
  }

}




#undef TIGHTNESS
#undef UPFRONTCOLORS
#undef Hv_MAXUSERMENUS
#endif /* end MAIN ifndef Win32 */

