/**
 * <EM>Deals with Windows 9X/NT specific functions.</EM>
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


#ifdef WIN32

#include "Hv.h"
#include "Hv_gateway_w32.h"
#include "Hv_pointer.h"
#include "Hv_init.h"
#include "Hv_pic.h"
#include "Hv_xyplot.h"
#include "Hv_maps.h"
#include <lmcons.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>    // includes common dialog functionality
#include <dlgs.h>       // includes common dialog template defines
#include <cderr.h>      // includes the common dialog error codes
#include "Hv_dlgtemplate.h"

extern short  fbfillcolor;

ATOM  winclass;
static Hv_LinkList    *timerList = NULL;
static HDC       hdcmem = NULL; //memory device context for off screen stuff
static HBITMAP   logo = 0;

// related to highlight drawing

static Boolean HighlightMode = False;
static unsigned long highlightPixel = 0x00002F;

extern Boolean   Hv_noViewDraw;


static short     currentPattern = Hv_FILLSOLIDPAT;
static Boolean   patternsInited = False;

static HACCEL    haccel = NULL;
static HWND      Hv_PrintfWindow = NULL;

static Boolean   standardmenusdone = False;

static int       Hv_altKeyIsPressed = 0; // True when the ALT key is down
extern int       Hv_sbredrawcontrol;

extern Hv_Window  Hv_trueMainWindow;  /* unchanging (even while off screen draws) */


/*
 * callbacks hold callback functions and data
 * for a variety of widgets
 */

Hv_CallbackData  *callbacks = NULL;
int               maxCallbacks = 0;

      FILE *sfile;

static RGBQUAD   *Hv_colorTable = NULL;

HFONT     *Hv_fonts = NULL;

static Boolean Hv_clickFlag = False;
static Hv_IntervalId  Hv_clickId;

/* local prototypes */

static void Hv_FreeResources();

static void createPrintfWindow();


static void	Hv_SetBrush(short pat,
		                short color);

static void Hv_SetToNearestColor(RGBQUAD *rgb);

static short  Hv_KeepInRange(int v);

static void Hv_W32PopupThinDialog(Hv_Widget dialog,int ix,int iy);


static void Hv_W32DrawBackgroundItems(Hv_ItemList items,
					 Hv_Region region);

static Hv_Pixmap  Hv_W32CreatePixmapFromDC(HDC  hdc,
									unsigned int  w,
									unsigned int  h);



static void Hv_HighlightOff();


static void Hv_ChangeMenuToggleState(HMENU menu,
									 int ID);
static void Hv_thinMenuCB(Hv_Widget);

static BOOL CALLBACK ComDlg32DlgProc(HWND hDlg, 
									 UINT uMsg, 
									 WPARAM wParam, 
									 LPARAM lParam);

static Boolean Hv_CheckForMessage(int        hvtype,
								  int        w32type,
								  Hv_XEvent *event);

static void Hv_RadiusRectIntersection(int   left,
									  int   top,
									  int   right,
									  int   botton,
									  float theta,
									  float thetacrit,
									  int   *xint,
									  int   *yint);

static void CALLBACK TimerProc (HWND  hwnd,
		  				        UINT  message,
						        UINT  iTimerID,
						        DWORD dtime);  


void Hv_StuffXEvent(Hv_XEvent  *xevent,
					int        x,
					int        y,
					int        button,
					int        modifiers,
					int        type);

static void Hv_SetFont(short font);

static void Hv_SetBkMode(int mode);

static void Hv_SetBkColor(short color);

static void Hv_SetTextColor(short color);

static void Hv_SetPenColor(short color);

static Boolean Hv_GoodColor(short color);

static unsigned long Hv_pixelColor(short color);

static void Hv_SetPenWidth(int width);

static HFONT Hv_NewFont(int size,
					int weight,
					int family,
					char * name);


static POINT *Hv_PolyToPOINT(Hv_Point  *xp,
					  int       n);

static LOGPALETTE *Hv_CreateLogicalPalette();

static	void Hv_ColorTableToPalette();


static void    Hv_FillGaps(short,
			   short,
			   short,
			   short);

static void Hv_ChangeWindowBackground(short color);

static void    Hv_StuffColor(BYTE,
			     BYTE,
			     BYTE,
			     int);

static void Hv_FillColorTable();

static void Hv_FillW32Rectangle(RECT *w32rect,
								short color);

static void Hv_RectToW32Rect(Hv_Rect *hvr,
						     RECT *w32r);

static void Hv_XYToW32Rect(short x,
						   short y,
					     RECT *w32r);

static void Hv_PointToW32Rect(Hv_Point *xp,
						      RECT *w32r);

static void Hv_RectangleToW32Rect(Hv_Rectangle *hvr,
						          RECT *w32r);

static void Hv_W32RectToRect(RECT *w32rect,
						  Hv_Rect *hvr);

static void Hv_StuffAcceleratorData(LPACCEL lpaccel,
									int     win32Id,
									int     acctype,
									char    accchr);

static void Hv_AddAccelerator(int   win32Id,
					   	      int   acctype,
						      char  accchr);

static Hv_CallbackData Hv_GetCallbackData(int w);

static int   Hv_NewWidgetCallback(Hv_FunctionPtr   callback,
					      Hv_Widget        parent,
						  Hv_Widget        w,
						  int              userId,
						  short            type);

static Boolean Hv_HandleWidget(int win32Id);



static  int Hv_lastClickType;

static  Hv_CallbackData  Hv_GetNextCallback();

static void Hv_InsertStartupMenus();


static BOOL initInstance (HINSTANCE hinst,
						  int nCmdShow);

 static void DrawHatchRect(short,
	             short,
	             short,
	             short,
				 short,
				 short);

static LRESULT CALLBACK mainFrameWndProc (HWND hwnd,
								   UINT message,
								   WPARAM wParam,
								   LPARAM lParam);

static HWND createMainFrameWindow (HINSTANCE hinst,
								   int nCmdShow);

static void mainFrame_OnCommand (HWND hwnd,
								 int id,
								 HWND hwndCtl,
								 UINT codeNotify);

Hv_Widget 	getWidgetFromControl(HWND dhandle);



static void mainFrame_OnDestroy (HWND hwnd);

static int exitInstance (MSG* pmsg);

static BOOL registerWindowClasses (HINSTANCE hinst);
static void aboutDlg_OnCommand (HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) ;


Hv_Widget   createNewWidgetEx(Hv_FunctionPtr   callback,
					      Hv_Widget        parent,
						  int              userId,
						  short            type);

#ifdef _DEBUG

static BOOL assertFailedOnLine (LPCSTR lpszFileName,
								int nLine);





#define THIS_FILE          __FILE__
#define ASSERT(f) \
	do { \
	  if (!(f) && assertFailedOnLine (THIS_FILE, __LINE__)) \
        FatalExit (0); \
	} while (0) \

#define VERIFY(f)          ASSERT(f)

#else   // _DEBUG

#define ASSERT(f)          ((void)0)
#define VERIFY(f)          ((void)(f))
#endif


#ifdef WIN98
   #define WM_MOUSELEAVE   WM_USER+2
   #define TME_LEAVE               1

   typedef struct tagTRACKMOUSEEVENT {
       DWORD cbSize;
       DWORD dwFlags;
       HWND  hwndTrack;
	   Hv_Widget widget;
   } TRACKMOUSEEVENT, *LPTRACKMOUSEEVENT;
#endif



/**
 * Hv_W32DrawWorldImageItem
 * @purpose Windows 9X/NT specific version of Hv_DrawWorldImageItem.
 * @see The gateway routine Hv_DrawWorldImageItem.
 */

void Hv_W32DrawWorldImageItem(Hv_Item    Item,
			   Hv_Region   region) {


  Hv_View            View = Item->view;
  Hv_WorldImageData  *wimage;
  HBITMAP            hbm;
  short              extra = 0;
  Hv_Rect            area;
  Hv_FRect           warea;
  Hv_PicInfo         *pinfo;
  Hv_Rect            *hr;
  int                status;
  HDC                hdc, hdcm = NULL;

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

  Hv_SetPattern(Hv_80PERCENTPAT, Hv_black);
  Hv_FillRect(&area, Hv_gray10);
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

	hbm = (HBITMAP)(pinfo->image);

	hdc = GetDC(Hv_mainWindow);
	hdcm = CreateCompatibleDC(hdc);
	ReleaseDC(Hv_mainWindow, hdc);

	DeleteObject(SelectObject(hdcm, pinfo->image));
	BitBlt(Hv_gc, cL, cT, cw, ch, hdcm, 0, 0, SRCCOPY); 
	DeleteDC(hdcm);
  }

  if ((wimage != NULL) && (wimage->frame)) {
    Hv_SetLineStyle(wimage->thickness, Hv_SOLIDLINE);
    Hv_FrameRect(&area, Item->color);
    Hv_SetLineStyle(0, Hv_SOLIDLINE);
  }

}


/**
 * Hv_W32DrawItemOnPixmap
 * @purpose Windows 9X/NT specific version of Hv_DrawItemOnPixmap.
 * @see The gateway routine Hv_DrawItemOnPixmap.
 */

Hv_Pixmap  Hv_W32DrawItemOnPixmap(Hv_Item   Item,
			       Hv_Region region) {

    Hv_Pixmap        pmap;
    Hv_Rect         *area;
   Hv_Rect          prect;
    short            dh, dv;

    if (Item == NULL)
	return 0;
    
    area = Item->area;
    pmap = Hv_CreatePixmap(area->width+1, area->height+1);
	DeleteObject(SelectObject(hdcmem, (HBITMAP)pmap));

/* set the clip to the entire pixmap and draw onto it */

    Hv_SetRect(&prect, 0, 0, (short)(area->width+1), (short)(area->height+1));
    Hv_ClipRect(&prect);
  
    dh = area->left;
    dv = area->top;
    Hv_OffsetItem(Item, (short)(-dh), dv, True);
    Hv_drawingOffScreen = True;
    Hv_DrawItem(Item, NULL);

    Hv_drawingOffScreen = False;
    Hv_gc = (HANDLE)(-1);


    Hv_OffsetItem(Item, dh, dv, True);
    return pmap;
}



/**
 * Hv_W32SaveViewBackground
 * @purpose Windows 9X/NT specific version of Hv_SaveViewBackground.
 * @see The gateway routine Hv_SaveViewBackground.
 */


void Hv_W32SaveViewBackground(Hv_View   View,
			   Hv_Region  region) {

  Hv_Rect          *hr;
  Hv_Rect          prect;
  short            dh, dv;
  HDC              savehdc;

/* first free the old pixmap and get a new one of the proper size */

  Hv_FreePixmap(View->background);
  if (View->memoryhdc != 0)
	  DeleteObject(View->memoryhdc);

  hr = View->hotrectborderitem->area;
  View->background = Hv_CreatePixmap(hr->width+1, hr->height+1);
  if (View->background == 0)
    return;

  View->memoryhdc = CreateCompatibleDC(Hv_gc);
  savehdc = hdcmem;
  hdcmem = View->memoryhdc;


  DeleteObject(SelectObject(hdcmem, (HBITMAP)(View->background)));

/* set the clip to the entire pixmap and draw onto it */

  Hv_SetRect(&prect, 0, 0, (short)(hr->width+1), (short)(hr->height+1));
  Hv_ClipRect(&prect);
  
  dh = hr->left;
  dv = hr->top;
  Hv_OffsetView(View, (short)(-dh), (short)(-dv));
  Hv_W32DrawBackgroundItems(View->items, NULL);
  Hv_OffsetView(View, dh, dv);

/* restore the main window and clip region */

    Hv_SetViewClean(View);

  Hv_gc = (HANDLE)(-1);

  Hv_RestoreClipRegion(region);
  hdcmem = savehdc;
}


/**
 * Hv_W32SaveUnder
 * @purpose Windows 9X/NT specific version of Hv_SaveUnder.
 * @see The gateway routine Hv_SaveUnder.
 */


Hv_Pixmap   Hv_W32SaveUnder(short x,
			 short y,
			 unsigned int  w,
			 unsigned int  h) {

    Hv_Pixmap    pmap;
    HDC hdcpm = CreateCompatibleDC(Hv_gc);

    pmap = Hv_W32CreatePixmap(w+2, h+2);
//	DeleteObject(SelectObject(hdcmem, (HBITMAP)pmap));
//	BitBlt(hdcmem, 0, 0, w+2, h+2, Hv_gc, x-1, y-1, SRCCOPY);

 	DeleteObject(SelectObject(hdcpm, (HBITMAP)pmap));
	BitBlt(hdcpm, 0, 0, w+2, h+2, Hv_gc, x-1, y-1, SRCCOPY);
	DeleteObject(hdcpm);

    return pmap;
}


/**
 * Hv_W32RestoreSaveUnder
 * @purpose Windows 9X/NT specific version of Hv_RestoreSaveUnder.
 * @see The gateway routine Hv_RestoreSaveUnder.
 */


void  Hv_W32RestoreSaveUnder(Hv_Pixmap pmap,
			  short x,
			  short y,
			  unsigned int  w,
			  unsigned int  h) {


//	DeleteObject(SelectObject(hdcmem, (HBITMAP)pmap));
//	BitBlt(Hv_gc, x-1, y-1, w+2, h+2, hdcmem, 0, 0, SRCCOPY);	
	
    HDC hdcpm = CreateCompatibleDC(Hv_gc);
	DeleteObject(SelectObject(hdcpm, (HBITMAP)pmap));
	BitBlt(Hv_gc, x-1, y-1, w+2, h+2, hdcpm, 0, 0, SRCCOPY);	
	DeleteObject(hdcpm);

}


/**
 * Hv_W32UpdateItemOffScreen
 * @purpose Windows 9X/NT specific version of Hv_UpdateItemOffScreen.
 * @see The gateway routine Hv_UpdateItemOffScreen.
 */


void  Hv_W32UpdateItemOffScreen(Hv_Item   Item,
			     Hv_Region region) {

  Hv_Pixmap        pmap;
  Hv_Rect         *area;

  Hv_Rect          prect;
  short            dh, dv;
  Hv_Region        rgn = NULL;

  if (Item == NULL)
    return;

  if (!Hv_ViewIsVisible(Item->view))
    return;

  area = Item->area;
  pmap = Hv_CreatePixmap(area->width+1, area->height+1);
  DeleteObject(SelectObject(hdcmem, (HBITMAP)pmap));



/* set the clip to the entire pixmap and draw onto it */

  Hv_SetRect(&prect, 0, 0, (short)(area->width+1), (short)(area->height+1));
  Hv_ClipRect(&prect);

  dh = area->left;
  dv = area->top;
  Hv_OffsetItem(Item, (short)(-dh), (short)(-dv), True);
  Hv_drawingOffScreen = True;
  Hv_DrawItem(Item, NULL);
  

  Hv_drawingOffScreen = False;
  Hv_gc = (HANDLE)(-1);

  Hv_OffsetItem(Item, dh, dv, True);

/* now copy back on screen */

  rgn = Hv_GetMinimumItemRegion(Item);
  Hv_SetClippingRegion(rgn);

  BitBlt(Hv_gc, area->left, area->top, area->width, area->height, hdcmem, 0, 0, SRCCOPY);
 
  
  Hv_Flush();
  Hv_DestroyRegion(rgn);
  
  if (region != NULL)
    Hv_RestoreClipRegion(region);
  
  Hv_FreePixmap(pmap);
}



/**
 * Hv_W32OffScreenViewControlUpdate
 * @purpose Windows 9X/NT specific version of Hv_OffScreenViewControlUpdate.
 * @see The gateway routine Hv_OffScreenViewControlUpdate.
  */


void Hv_W32OffScreenViewControlUpdate(Hv_View    View,
				   Hv_Region  region) {

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
  	DeleteObject(SelectObject(hdcmem, (HBITMAP)(pmap)));

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
  Hv_gc = (HANDLE)(-1);
  
  Hv_OffsetView(View, dh, dv);

  Hv_DestroyRegion(drawrgn);
  rgn = Hv_RectRegion(local);
  drawrgn = Hv_IntersectRegion(rgn, region);

  Hv_SetClippingRegion(drawrgn);

/* now copy the pixmap on screen */

  	BitBlt(Hv_gc, local->left, local->top, local->width, local->height, hdcmem, 0, 0, SRCCOPY);	

 
  Hv_Flush();
  
  Hv_DestroyRegion(rgn);
  Hv_DestroyRegion(drawrgn);
  Hv_RestoreClipRegion(region);
  Hv_FreePixmap(pmap);

}






/**
 * Hv_W32RestoreViewBackground
 * @purpose Windows 9X/NT specific version of Hv_RestoreViewBackground.
 * @see The gateway routine Hv_RestoreViewBackground.
 */


void  Hv_W32RestoreViewBackground(Hv_View View) {
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
  Hv_SetMainHDC();

  BitBlt(Hv_gc, hr->left, hr->top, hr->width+1, hr->height+1,
	  (HDC)View->memoryhdc, 0, 0, SRCCOPY);	


  Hv_Flush();
}



/**
 * Hv_W32OffScreenViewUpdate
 * @purpose Windows 9X/NT specific version of Hv_OffScreenViewUpdate.
 * @see The gateway routine Hv_OffScreenViewUpdate.
 */

void Hv_W32OffScreenViewUpdate(Hv_View    View,
			    Hv_Region  region) {


  Hv_Rect          *hr;
  Hv_Rect          prect;
  short            dh, dv;
  Hv_Region        drawrgn = NULL;
  Hv_Region        hrrgn = NULL;
  Hv_Pixmap        pmap;
  Hv_Item          hrb, temp;

	if ((Hv_inPostscriptMode) || (Hv_mainWindow == 0) || !Hv_ViewIsVisible(View))
		return;

/* create pixmap, store the mainwindow */

  hrb = View->hotrectborderitem;
  hr = View->hotrect;
  pmap = Hv_CreatePixmap(hr->width, hr->height);
  DeleteObject(SelectObject(hdcmem, (HBITMAP)(pmap)));

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
  Hv_SetMainHDC();

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
  Hv_gc = (HANDLE)(-1);
  
  Hv_OffsetView(View, dh, dv);


  Hv_DestroyRegion(drawrgn);
  hrrgn = Hv_HotRectRegion(View);
  drawrgn = Hv_IntersectRegion(hrrgn, region);

  Hv_SetClippingRegion(drawrgn);

/* now copy the pixmap on screen */

 	
	BitBlt(Hv_gc, hr->left, hr->top, hr->width+1, hr->height+1, hdcmem, 0, 0, SRCCOPY);	


  Hv_Flush();
  
  Hv_DestroyRegion(hrrgn);
  Hv_DestroyRegion(drawrgn);
  Hv_RestoreClipRegion(region);
  Hv_FreePixmap(pmap);

}


/**
 * Hv_W32ScrollView
 * @purpose Windows 9X/NT specific version of Hv_ScrollView.
 * @see The gateway routine Hv_ScrollView.
 */

void Hv_W32ScrollView(Hv_View   View,
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
   
  Boolean           HorizontalScroll;
  short             efflen;
 
/* set defaults */

  Hv_SetClippingRegion(region);     /* set region to clipping region*/
  
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
  Hv_UpdateThumbs(View, HorizontalScroll, tl, tw);       /* update the slider thumbs */

  if (HorizontalScroll)
    Hv_sbredrawcontrol = 1;
  else
    Hv_sbredrawcontrol = 2;

  Hv_DrawScrollBars(View, region); 
  Hv_sbredrawcontrol = 0;


  Hv_scrollDH = -dh;
  Hv_scrollDV = -dv;

/* reposition hotrect items */

  Hv_offsetDueToScroll = True;
  Hv_OffsetHotRectItems(View->items, (short)(-dh), (short)(-dv));
  Hv_offsetDueToScroll = False;

    Hv_SetViewDirty(View);

  Hv_DrawViewHotRect(View);


  Hv_SetClippingRegion(region);               /* set region to clipping region*/
  Hv_Flush();
  Hv_CheckForExposures(Hv_GRAPHICSEXPOSE);
  Hv_CheckForExposures(Hv_NOEXPOSE);
}

/*==========================================
 * EVENT RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_W32Go
 * @purpose Windows 9X/NT specific version of Hv_Go.
 * @see The gateway routine Hv_Go.
 */

void Hv_W32Go(void) {

    MSG	                msg;
	int                 status;
 
 // Main message loop:

    while ((status = GetMessage (&msg, NULL, 0, 0)) != 0) {

		if (status != -1)
			if ((haccel == NULL) || !TranslateAccelerator(Hv_mainWindow, haccel, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
    }

    exitInstance (&msg) ;


}

/**
 * Hv_W32ProcessEvent
 * @purpose Windows 9X/NT specific version of Hv_ProcessEvent.
 * @see The gateway routine Hv_ProcessEvent.
 */

void  Hv_W32ProcessEvent(int mask) {
	    MSG	                msg;
	    int                 status;
 
 

    status = GetMessage (&msg, NULL, 0, 0) ;

	if (status != -1)
	{
		if ((haccel == NULL) || !TranslateAccelerator(Hv_mainWindow, haccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

}

/**
 * Hv_W32AddEventHandler
 * @purpose Windows 9X/NT specific version of Hv_AddEventHandler.
 * @see The gateway routine Hv_AddEventHandler.
 */

void  Hv_W32AddEventHandler(Hv_Widget       w,
			    int             mask,
			    Hv_EventHandler handler,
			    Hv_Pointer      data) {

/** Hv only support spports the following masks
 Hv_BUTTONPRESSMASK      01
 Hv_BUTTONRELEASEMASK    02
 Hv_BUTTONMOTIONMASK     04
 Hv_BUTTON1MOTIONMASK   010
 **/

  Hv_W32AddCallback(w ,
		            "Hv_activateCallback" , // this will have to do for now
                    handler,
                    data);


}

/**
 * Hv_W32VaSetValues
 * @purpose Windows 9X/NT specific version of Hv_VaSetValues.
 * @see The gateway routine Hv_VaSetValues.
 */

void  Hv_W32VaSetValues(Hv_Widget w,
		      ...) {

	va_list           ap;

  int            argflag = -1;
  int            prevargflag;
  char           *errstr;
  short          cpos;
  int            i;
  int            found = 0;
  int            ci = 0;
  USHORT         Color = 0;
  LPWSTR         wtitle = NULL;
  char           *ltitle = NULL;
  Hv_Widget      widget,rwidget;
  Hv_LinkList    *wp = NULL;
  Hv_CompoundString             title;
  int baseunitX;  /* get the system metrics baseunit */
  int baseunitY; 
  int temp;

/* now get the attributes */
  va_start(ap, w);

  while (argflag != 0)
  {
    prevargflag = argflag;
    argflag = va_arg(ap, int);

    if ((argflag < 0) )
	{
      errstr = (char *)Hv_Malloc(80);
      sprintf(errstr, "Illegal attribute %d passed to Hv_VaSetValues.", argflag);
      Hv_Warning(errstr);
      Hv_Free(errstr);
      Hv_Println("Illegal attribute %d passed to Hv_VaSetValues", argflag);
      Hv_Println("Previously processed attribute %d", prevargflag);
    }

    else if (argflag > 0)
	
	{
		switch(argflag)
		{
		case Hv_N_OF_MANY:
			break;
	    case Hv_Naccelerator:
			break;
		case Hv_NacceleratorText:
			break;
		case Hv_NautoShowCursorPosition:
			break;
		case Hv_NautoUnmanage:
			break;
		case Hv_Nbackground:
			Color = 	(short)va_arg(ap, int);
			for(ci=0;ci< Hv_numColors;ci++)
			{
				if(Hv_colors[ci] == Color)
				{
					found = 1;
					break;
				}
			}
			if(found)
				w->wbgColor = ci;
			else
				w->wbgColor = 10;
			break;
		case Hv_NblinkRate:
			break;
		case Hv_NborderColor:
			break;
		case Hv_NborderWidth:
			break;
		case Hv_NbottomAttachment:
			break;
		case Hv_NcancelLabelString:
			break;
		case Hv_Nchildren:
			break;
		case Hv_Ncolormap:
			break;
		case Hv_Ncolumns:
			break;
		case Hv_NcursorPosition:
			 cpos = (short)va_arg(ap, int);
			break;
		case Hv_NcursorPositionVisible:
			break;
		case Hv_NdefaultButtonShadowThickness:
			break;
		case Hv_NdeleteResponse:
			break;
		case Hv_NdialogStyle:
			break;
		case Hv_NdialogTitle:
			title = (char *) va_arg(ap,   Hv_CompoundString);
			if(w->wisWisable)
			{
			 i = SetWindowText(  w->mscontrol,title->text);     
			}
//			else
			{
#ifndef _UNICODE
			   ANSIToUnicode(title->text,&wtitle);
#else
			   wtitle = (LPWSTR *) Hv_Malloc(StrLen(title->text) +4);
			   strcpy(wtitle,title->text);
#endif
              if(w->wlabel)
				Hv_Free(w->wlabel);
			  w->wlabel= wtitle;
			}

			break;
		case Hv_Ndirectory:
			break;
		case Hv_NeditMode:
			break;
		case Hv_Neditable:
			break;
		case Hv_NentryAlignment:
			break;
		case Hv_NfileTypeMask:
			break;
		case Hv_NfontList:
			break;
		case Hv_Nforeground:
			found = 0;
			Color = 	(short)va_arg(ap, int);
			for(ci=0;ci< Hv_numColors;ci++)
			{
				if(Hv_colors[ci] == Color)
				{
					found = 1;
					break;
				}
			}
			if(found)
				w->wbgColor = ci;
			else
				w->wbgColor = 30;

			break;
		case Hv_NfractionBase:
			break;
		case Hv_Nheight:
		      temp  = (short)va_arg(ap, int);
              baseunitY = HIWORD(GetDialogBaseUnits()); 
			  w->set_iheight = (temp * 8) / baseunitY;
			break;
		case Hv_NhighlightThickness:
			break;
		case Hv_NindicatorType:
			break;
		case Hv_NindicatorSize:
			break;
		case Hv_NisHomogeneous:
			break;
		case Hv_NitemCount:
			break;
		case Hv_Nitems:
			break;
		case Hv_NlabelPixmap:
			break;
		case Hv_NlabelString:
			break;
		case Hv_NlabelType:
			Color = 	(short)va_arg(ap, int);
			w->wsubtype = Color;
			break;
		case Hv_NlabelTypeXmPIXMAPNULL:
			break;
		case Hv_NleftAttachment:
			break;
		case Hv_NleftPosition:
			break;
		case Hv_NlistSizePolicy:
			break;
		case Hv_NmarginHeight:
			break;
		case Hv_NmarginWidth:
			break;
		case Hv_Nmaximum:
			break;
		case Hv_NmenuHelpWidget:
			break;
		case Hv_NmenuHistory:
			widget = va_arg(ap, Hv_Widget);
			if(w == NULL)
				break;
			if(widget == NULL)
				break;
			rwidget = (Hv_Widget)w->generic; // get the option submenu
			if(rwidget != NULL)
			  if(rwidget->wchildren)  
			  {
			    for (i=0,wp = rwidget->wchildren->next ;i<= rwidget->num_children ;wp = wp->next,i++)
				{
			  	if(widget  == (Hv_Widget) wp->data)
				{
					w->wstate = i;
					w->wdata = widget;
					break;
				}
			  }
			}
			break;
		case Hv_NmessageString:
			break;
		case Hv_Nminimum:
			break;
		case Hv_NnoResize:
			break;
		case Hv_NnumChildren:
			break;
		case Hv_NnumColumns:
			break;
		case Hv_NokLabelString:
			break;
		case Hv_Norientation:
			break;
		case Hv_Npacking:
			break;
		case Hv_Npattern:
			break;
		case Hv_NresizePolicy:
			break;
		case Hv_NrightAttachment:
			break;
		case Hv_NrightPosition:
			break;
		case Hv_Nrows:
			break;
		case Hv_NscrollBarDisplayPolicy:
			break;
		case Hv_NscrollHorizontal:
			break;
		case Hv_NselectionPolicy:
			break;
		case Hv_Nsensitive:
			break;
		case Hv_Nset:
			if(w->wtype == Hv_TOGGLEDIALOGITEM || w->wtype == Hv_RADIODIALOGITEM )
			      Hv_W32SetToggleButton(w,(short)va_arg(ap, int));
			else
				w->wstate = va_arg(ap, int);
			break;
		case Hv_NshadowThickness:
			break;
		case Hv_NshowAsDefault:
			break;
		case Hv_NshowValue:
			break;
		case Hv_Nspacing:
			break;
		case Hv_NsubMenuId:
			break;
		case Hv_NtextString:
			break;
		case Hv_Ntitle:
			break;
		case Hv_NtopAttachment:
			break;
		case Hv_Nvalue:
			break;
		case Hv_NvisibleItemCount:
			break;
		case Hv_NvisibleWhenOff:
			break;
		case Hv_NwhichButton:
			break;
		case Hv_Nwidth:
			  temp = (short)va_arg(ap, int);
			   baseunitX  = LOWORD(GetDialogBaseUnits()); /* get the system metrics baseunit */

              w->set_iwidth = (temp *4) / baseunitX;
			break;
		case Hv_NwordWrap:
			break;
		case Hv_Nx:
			   temp = (short)va_arg(ap, int);
			   baseunitX  = LOWORD(GetDialogBaseUnits()); /* get the system metrics baseunit */
			    w->set_ix = (short)((temp *4) / baseunitX);
			break;
		case Hv_Ny:
			    temp = (short)va_arg(ap, int);
			    baseunitY  = HIWORD(GetDialogBaseUnits()); /* get the system metrics baseunit */
			    w->set_iy = (short)((temp *8 ) / baseunitY);
			break;
		case Hv_NtextFontList:
			break;
		case Hv_NbuttonFontList:
			break;
		case Hv_NlabelFontList:
			break;
		case Hv_NselectedItems:
			break;
		case Hv_NselectedItemCount:
			break;
		case HvNselectedItems:
			break;
		default:
          errstr = (char *)Hv_Malloc(80);
          sprintf(errstr, "Illegal attribute %d passed to Hv_VaSetValues", argflag);
          Hv_Warning(errstr);
          Hv_Free(errstr);
          Hv_Println("Illegal attribute %d passed to Hv_VaSetValues", argflag);
          Hv_Println("Previously processed attribute %d", prevargflag);
		}
	}
  }
  va_end(ap);
}

/**
 * Hv_W32VaGetValues
 * @purpose Windows 9X/NT specific version of Hv_VaGetValues.
 * @see The gateway routine Hv_VaGetValues.
 */

void  Hv_W32VaGetValues(Hv_Widget w,
		      ...) {

	va_list           ap;

  int              argflag = -1;
  int              prevargflag;
  char             *errstr;
  int i;
  int found = 0;
  int ci = 0;
  USHORT  Color = 0;
  LPWSTR wtitle = NULL;
  char *ltitle = NULL;
  Hv_Widget *widgetList,swidget,*rwidget;
  int       *pint;
     Hv_LinkList *wp = NULL;
/* now get the attributes */
  va_start(ap, w);

  while (argflag != 0)
  {
    prevargflag = argflag;
    argflag = va_arg(ap, int);

    if ((argflag < 0) )
	{
      errstr = (char *)Hv_Malloc(80);
      sprintf(errstr, "Illegal attribute %d passed to Hv_VaSetValues.", argflag);
      Hv_Warning(errstr);
      Hv_Free(errstr);
      Hv_Println("Illegal attribute %d passed to Hv_VaSetValues", argflag);
      Hv_Println("Previously processed attribute %d", prevargflag);
    }

    else if (argflag > 0)
	
	{
		switch(argflag)
		{
		case Hv_N_OF_MANY:
			break;
	    case Hv_Naccelerator:
			break;
		case Hv_NacceleratorText:
			break;
		case Hv_NautoShowCursorPosition:
			break;
		case Hv_NautoUnmanage:
			break;
		case Hv_Nbackground:
			break;
		case Hv_NblinkRate:
			break;
		case Hv_NborderColor:
			break;
		case Hv_NborderWidth:
			break;
		case Hv_NbottomAttachment:
			break;
		case Hv_NcancelLabelString:
			break;
		case Hv_Nchildren:
			if(w== NULL)
				break;
			widgetList = va_arg(ap, Hv_Widget *);
			*widgetList = (Hv_Widget) Hv_Malloc(w->num_children*sizeof(Hv_Widget));
            rwidget = widgetList;
			if(w->wchildren)  
			{
			  for (wp = w->wchildren->next; wp != w->wchildren ;wp = wp->next)
			  {
				*rwidget++ = (Hv_Widget) wp->data;
			  }
			}
			break;
		case Hv_Ncolormap:
			break;
		case Hv_Ncolumns:
			break;
		case Hv_NcursorPosition:
			break;
		case Hv_NcursorPositionVisible:
			break;
		case Hv_NdefaultButtonShadowThickness:
			break;
		case Hv_NdeleteResponse:
			break;
		case Hv_NdialogStyle:
			break;
		case Hv_NdialogTitle:
			break;
		case Hv_Ndirectory:
			break;
		case Hv_NeditMode:
			break;
		case Hv_Neditable:
			break;
		case Hv_NentryAlignment:
			break;
		case Hv_NfileTypeMask:
			break;
		case Hv_NfontList:
			break;
		case Hv_Nforeground:
			break;
		case Hv_NfractionBase:
			break;
		case Hv_Nheight:
            pint = va_arg(ap, int *);
            *pint = w->iheight;
			break;
		case Hv_NhighlightThickness:
			break;
		case Hv_NindicatorType:
			break;
		case Hv_NindicatorSize:
			break;
		case Hv_NisHomogeneous:
			break;
		case Hv_NitemCount:
			pint = va_arg(ap, int *);
			*pint = w->wdataNitems;
			break;
		case Hv_Nitems:
			break;
		case Hv_NlabelPixmap:
			break;
		case Hv_NlabelString:
			break;
		case Hv_NlabelType:
			break;
		case Hv_NlabelTypeXmPIXMAPNULL:
			break;
		case Hv_NleftAttachment:
			break;
		case Hv_NleftPosition:
			break;
		case Hv_NlistSizePolicy:
			break;
		case Hv_NmarginHeight:
			break;
		case Hv_NmarginWidth:
			break;
		case Hv_Nmaximum:
			break;
		case Hv_NmenuHelpWidget:
			break;
		case Hv_NmenuHistory:
//			Hv_VaGetValues(optmenu, Hv_NmenuHistory, &w, NULL);
			rwidget  = va_arg(ap, Hv_Widget *);
			swidget = (Hv_Widget)w->generic; // get last container widget 
			if(swidget->wchildren)  
			{
			  for (i=0,wp = swidget->wchildren->next; i<w->wstate ;i++,wp = wp->next)
			  {				
			  }
			  *rwidget = (Hv_Widget) wp->data;
			}

			break;
		case Hv_NmessageString:
			break;
		case Hv_Nminimum:
			break;
		case Hv_NnoResize:
			break;
		case Hv_NnumChildren:
			if(w == NULL)
				break;
			 pint = va_arg(ap, int *);
            *pint = w->num_children;
			break;
		case Hv_NnumColumns:
			break;
		case Hv_NokLabelString:
			break;
		case Hv_Norientation:
			break;
		case Hv_Npacking:
			break;
		case Hv_Npattern:
			break;
		case Hv_NresizePolicy:
			break;
		case Hv_NrightAttachment:
			break;
		case Hv_NrightPosition:
			break;
		case Hv_Nrows:
			break;
		case Hv_NscrollBarDisplayPolicy:
			break;
		case Hv_NscrollHorizontal:
			break;
		case Hv_NselectionPolicy:
			break;
		case Hv_Nsensitive:
			break;
		case Hv_Nset:
			 pint = va_arg(ap, int *);
            *pint = w->wstate;
			break;
		case Hv_NshadowThickness:
			break;
		case Hv_NshowAsDefault:
			break;
		case Hv_NshowValue:
			break;
		case Hv_Nspacing:
			break;
		case Hv_NsubMenuId:
			rwidget = va_arg(ap, Hv_Widget *);
			if(w == NULL)
				break;
            *rwidget = (Hv_Widget)w->generic;
			break;
		case Hv_NtextString:
			break;
		case Hv_Ntitle:
			break;
		case Hv_NtopAttachment:
			break;
		case Hv_Nvalue:
			break;
		case Hv_NvisibleItemCount:
			break;
		case Hv_NvisibleWhenOff:
			break;
		case Hv_NwhichButton:
			break;
		case Hv_Nwidth:
			 pint = va_arg(ap, int *);
            *pint = w->iwidth;
			break;
		case Hv_NwordWrap:
			break;
		case Hv_Nx:
			break;
		case Hv_Ny:
			break;
		case Hv_NtextFontList:
			break;
		case Hv_NbuttonFontList:
			break;
		case Hv_NlabelFontList:
			break;
		case Hv_NselectedItemCount:
			break;
		case Hv_NselectedItems:
			break;

		default:
          errstr = (char *)Hv_Malloc(80);
          sprintf(errstr, "Illegal attribute %d passed to Hv_VaSetValues", argflag);
          Hv_Warning(errstr);
          Hv_Free(errstr);
          Hv_Println("Illegal attribute %d passed to Hv_VaSetValues", argflag);
          Hv_Println("Previously processed attribute %d", prevargflag);
		}
	}
  }
  va_end(ap);
}


/**
 * Hv_W32RemoveTimeOut
 * @purpose Windows 9X/NT specific version of Hv_RemoveTimeOut.
 * @see The gateway routine Hv_RemoveTimeOut.
 */

void  Hv_W32RemoveTimeOut(Hv_IntervalId id) {

// find the timer with this ID in the link list


	Hv_LinkList *temp;
	hvTimer     *timeStruct;
//return;
	if (timerList == NULL)
		return;

	for(temp = timerList->next; temp != timerList; temp = temp->next) {
		if (temp->data != NULL) {
			timeStruct = (hvTimer *)(temp->data);
			if (timeStruct->id == id) {
				Hv_DlistDeque(temp);
				Hv_Free(temp->data);
				Hv_Free(temp);
				return;
			}
		}
	}

}


/**
 * Hv_W32AddTimeOut
 * @purpose Windows 9X/NT specific version of Hv_AddTimeOut.
 * @see The gateway routine Hv_AddTimeOut.
 */

Hv_IntervalId  Hv_W32AddTimeOut(unsigned long interval,
				Hv_TimerCallbackProc  callback,
				Hv_Pointer            data) {

	static long id = 0;
	hvTimer *timeStruct;
	DWORD currentTime = GetTickCount();
    DWORD expireTime  = 0;

	expireTime = currentTime + interval;
	timeStruct = Hv_Malloc(sizeof(hvTimer));

	timeStruct->timoutTime = expireTime;
	timeStruct->callback   = callback;
	timeStruct->data       = data;
	timeStruct->id         = id;

    /* put the new timout in the list */
	/* when the timer function is called (every Hv_TIMEOUT milliseconds) it */
	/* will search the list and call all of the expired timers */

	Hv_DlistInsertSort(timerList,timeStruct,expireTime);
	id++;
    return timeStruct->id; 
}

/**
 * Hv_CheckForMessage
 * @purpose Windows 9X/NT specific version of Hv_CheckMaskEvent.
 * @local
 */

static Boolean Hv_CheckForMessage(int        hvtype,
								  int        w32type,
								  Hv_XEvent *event) {

	MSG   msg;
	POINT clp;
    int button;

	if (!PeekMessage (&msg, Hv_mainWindow, w32type, w32type, PM_REMOVE))
		return False;

	Hv_GetClientPosition(&clp);

	if(msg.message == WM_MBUTTONUP)
		button =2;
	else 
		button = 1;

	Hv_StuffXEvent(event,
					clp.x,
					clp.y,
					button,
					0,
					hvtype);

	return True;
}

/**
 * Hv_W32CheckMaskEvent
 * @purpose Windows 9X/NT specific version of Hv_CheckMaskEvent.
 * @see The gateway routine Hv_CheckMaskEvent.
 */

Boolean Hv_W32CheckMaskEvent(int        mask,
			     Hv_XEvent *event) {

	MSG   msg;
	POINT clp;

	if (!PeekMessage (&msg, Hv_mainWindow, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
		return False;

	switch (msg.message) {

		case WM_RBUTTONUP:  case WM_LBUTTONUP:
			if (Hv_CheckBit(mask, Hv_BUTTONRELEASEMASK)) {
			   Hv_GetClientPosition(&clp);

			   Hv_StuffXEvent(event,
					clp.x,
					clp.y,
					1,
					Hv_XModifiers(msg.wParam),
					Hv_buttonRelease);

			   return True;
			}
			break;
		case WM_MBUTTONUP:
			if (Hv_CheckBit(mask, Hv_BUTTONRELEASEMASK)) {
			   Hv_GetClientPosition(&clp);

			   Hv_StuffXEvent(event,
					clp.x,
					clp.y,
					2,
					Hv_XModifiers(msg.wParam),
					Hv_buttonRelease);

			   return True;
			}

			break;
		case WM_RBUTTONDOWN: case WM_MBUTTONDOWN: case WM_LBUTTONDOWN:

			if (Hv_CheckBit(mask, Hv_BUTTONPRESSMASK)) {
			   Hv_GetClientPosition(&clp);

			   Hv_StuffXEvent(event,
					clp.x,
					clp.y,
					1,
					Hv_XModifiers(msg.wParam),
					Hv_buttonPress);

			   return True;
			}
			break;


		case WM_MOUSEMOVE:
			if (Hv_CheckBit(mask, Hv_BUTTON1MOTIONMASK) ||
				Hv_CheckBit(mask, Hv_BUTTONMOTIONMASK)) {
			    Hv_GetClientPosition(&clp);

			    Hv_StuffXEvent(event,
					clp.x,
					clp.y,
					1,
					Hv_XModifiers(msg.wParam),
					Hv_motionNotify);

                return True;
			}
			break;
	  }


	return False;
}

/**
 * Hv_XModifiers
 * @purpose Converts the Win32 modifiers into X equivalents
 */

int Hv_XModifiers(int w32param) {
	int xmod = 0;

	if ((w32param & MK_CONTROL) == MK_CONTROL)
		xmod += ControlMask;

	if ((w32param & MK_SHIFT) == MK_SHIFT)
		xmod += ShiftMask;

  if (Hv_altKeyIsPressed) {
		xmod += Mod1Mask;
		Hv_altKeyIsPressed = 0;
  }

	return xmod;
}

/**
 * Hv_W32Initialize
 * @purpose Windows 9X/NT specific version of Hv_Initialize.
 * @see The gateway routine Hv_Initialize.
 */

int Hv_W32Initialize(unsigned int argc,
		     char       **argv,
		     char       *versStr) {

	BOOL   status;
    DWORD  bunsize = UNLEN + 1;
    DWORD  bcnsize = MAX_COMPUTERNAME_LENGTH + 1;
    DWORD  bosnsize = 40;

 

/* some strings used in widgets */

    Hv_inputCallback             = "Hv_inputCallback";
    Hv_activateCallback          = "Hv_activateCallback";
    Hv_valueChangedCallback      = "Hv_valueChangedCallback";
    Hv_okCallback                = "Hv_okCallback";
    Hv_cancelCallback            = "Hv_cancelCallback";
    Hv_multipleSelectionCallback = "Hv_multipleSelectionCallback";
    Hv_singleSelectionCallback   = "Hv_singleSelectionCallback";
	//
	//
    Hv_thinMenuCallback          = "Hv_thinMenuCallback";


/* some system-like parameters */

 Hv_userName = (char *) Hv_Malloc(bunsize);
  if (Hv_userName != NULL)
    status = GetUserName(Hv_userName, &bunsize);
  
  Hv_hostName = (char *) Hv_Malloc(bcnsize);
  if (Hv_hostName != NULL)
    status = GetComputerName(Hv_hostName, &bcnsize);
 
/* note: used to resuse bcnsize here, but the GetComputerName
 * call above will change the value of bcnsize. On my laptop
 * named "ASP", it came back as 3, which was too small to
 * hold "W32" below!
 */

  Hv_osName = (char *) Hv_Malloc(bosnsize);
  if (Hv_osName != NULL)
    strcpy(Hv_osName,"W32");


  Hv_display =  -1;

  return 0;
}

/**
 * Hv_W32SystemSpecificInit
 * @purpose Windows 9X/NT specific version of Hv_SystemSpecificInit.
 * @see The gateway routine Hv_SystemSpecificInit.
 */

void Hv_W32SystemSpecificInit() {

/*
 * In windows, I can initilize the fonts early
 */

	Hv_W32InitFonts();

/*
 * Initialize the main window
 */

initInstance (Hv_appContext,
		      Hv_iccmdShow);

  timerList = Hv_DlistCreateList (); /* setup the timer list */
  SetTimer(Hv_mainWindow, Hv_TIMERID, Hv_TIMERVALUE, TimerProc);

}

/**
 * Hv_W32InitWindow
 * @purpose Windows 9X/NT specific version of Hv_InitWindow.
 * @see The gateway routine Hv_InitWindow.
 */

void  Hv_W32InitWindow(void) {
}

/**
 * Hv_W32CreateAppContext
 * @purpose Windows 9X/NT specific version of Hv_CreateAppContext.
 * @see The gateway routine Hv_CreateAppContext.
 */

void  Hv_W32CreateAppContext() {
}

/**
 * Hv_W32CreateGraphicsContext
 * @purpose Windows 9X/NT specific version of Hv_CreateGraphicsContext.
 * @see The gateway routine Hv_CreateGraphicsContext.
 */

void  Hv_W32CreateGraphicsContext(void) {
}


/**
 * Hv_W32InitCursors
 * @purpose Windows 9X/NT specific version of Hv_InitCursors.
 * @see The gateway routine Hv_InitCursors.
 */

void  Hv_W32InitCursors() {
  Hv_defaultCursor     = LoadCursor(NULL, IDC_ARROW);  /* default cursor is an arrow */
  Hv_crossCursor       = LoadCursor(NULL, IDC_CROSS); /* small cross hair cursor for hot rects */
  Hv_waitCursor        = LoadCursor(NULL, IDC_WAIT);     /* watch cursor for waiting */
  Hv_dragCursor        = LoadCursor(NULL, IDC_SIZEALL);     /* cursor used when dragging */
  Hv_dotboxCursor      = LoadCursor(NULL, IDC_CROSS);    /* cursor used when dragging */
  Hv_bottomRightCursor = LoadCursor(NULL, IDC_SIZEALL);     /* cursor used when resizing */
}

/**
 * Hv_W32InitPaterns
 * @purpose Windows 9X/NT specific version of Hv_InitPatterns.
 * @see The gateway routine Hv_InitPatterns.
 */

void  Hv_W32InitPatterns() {
	int i;
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
}

/**
 * Hv_CreateBitMapFromPattern
 * @purpose   Create a handle to a bit map from a char array
 * @param     pat   The char array holding the 16x16 pattern
 * @return    The bitmap handle
 * @local
 */

HBITMAP Hv_CreateBitMapFromPattern(short pat,
										  short color) {

	return CreateBitmap(16, 16, 1, 1, Hv_bitmaps[pat]);
}

/**
 * Hv_W32InitGraphics
 * @purpose Windows 9X/NT specific version of Hv_InitGraphics.
 * @see The gateway routine Hv_InitGraphics.
 */

void Hv_W32InitGraphics(void) {
	Hv_gc = (HANDLE)(-1);
}

/**
 * Hv_W32InitColors
 * @purpose Windows 9X/NT specific version of Hv_InitColors.
 * @see The gateway routine Hv_InitColors.
 */

void Hv_W32InitColors(void) {

	Hv_colorMap = 0;

/*
 * Step 1, fill the color table with the Hv colors
 */

	Hv_FillColorTable();

	if (Hv_colorTable == NULL)
		return;

/*
 * Step 2, Create a palette from the color table
 */

	Hv_ColorTableToPalette();


	
}

/**
 * Hv_W32InitCanvas
 * @purpose Windows 9X/NT specific version of Hv_InitCanvas.
 * @see The gateway routine Hv_InitCanvas.
 */

void Hv_W32InitCanvas(void) {
}

/**
 * Hv_W32InitGlobalWidgets
 * @purpose Windows 9X/NT specific version of Hv_InitGlobalWidgets.
 * @see The gateway routine Hv_InitGlobalWidgets.
 */

void Hv_W32InitGlobalWidgets(void) {
}



/*==========================================
 * MENU RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_W32CreateMainMenuBar
 * @purpose Windows 9X/NT specific version of Hv_CreateMainMenuBar.
 * @see The gateway routine Hv_CreateMainMenuBar.
 */

Hv_Widget  Hv_W32CreateMainMenuBar(Hv_Widget   parent) {
	Hv_Widget w = createNewWidget(MENUBAR);
//	w->wdata = CreateMenu();
	SETHANDLE(w, CreateMenu());
	return w;
}


/**
 * Hv_W32CreatePopUp
 * @purpose Windows 9X/NT specific version of Hv_CreatePopup.
 * @see The gateway routine Hv_CreatePopup.
 */

Hv_Widget  Hv_W32CreatePopup(Hv_Widget   parent) {
	Hv_Widget w = createNewWidget(MENU);
//	w->wdata = CreatePopupMenu();
	SETHANDLE(w, CreatePopupMenu());
	return w;
}


/**
 * Hv_W32CreateSubmenu
 * @purpose Windows 9X/NT specific version of Hv_CreateSubmenu.
 * @see The gateway routine Hv_CreateSubmenu.
 */

Hv_Widget  Hv_W32CreateSubmenu(Hv_Widget   parent) {
	Hv_Widget w = createNewWidget(MENU);
//	w->wdata = CreatePopupMenu();
	SETHANDLE(w, CreatePopupMenu());
	return w;
}


/**
 * Hv_W32Popup1Callback
 * @purpose Windows 9X/NT specific version of Hv_Popup1Callback.
 * @see The gateway routine Hv_Popup1Callback.
 */


void Hv_W32Popup1Callback(Hv_Widget          dummy,
			  Hv_Widget          popup,
			  Hv_CanvasCallback *cbs) {

}


/**
 * Hv_W32Popup2Callback
 * @purpose Windows 9X/NT specific version of Hv_Popup2Callback.
 * @see The gateway routine Hv_Popup2Callback.
 */


void Hv_W32Popup2Callback(Hv_Widget          dummy,
			  Hv_Widget          popup,
			  Hv_CanvasCallback *cbs) {
}


/**
 * Hv_W32CreateUserMenus
 * @purpose Windows 9X/NT specific version of Hv_CreateUserMenus.
 * @see The gateway routine Hv_CreateUserMenus.
 */

void     Hv_W32CreateUserMenus(void) {

/* NOTE: This is purposely an empty routine for Win32 */

}

/**
 * Hv_W32SetMenuItemString
 * @purpose Windows 9X/NT specific version of Hv_SetMenuItemString.
 * @see The gateway routine Hv_SetMenuItemString.
 */


void  Hv_W32SetMenuItemString(Hv_Widget menuitem,
			                  char     *str,
			                  short     font) {

	Hv_CallbackData  wd = Hv_GetCallbackData(GETWIN32ID(menuitem));

	HMENU hmenu = NULL;

	if (wd == NULL)
		return;

	hmenu = GETMENUHANDLE(wd->parent);

	ModifyMenu(hmenu, menuitem->wcallback_id, MF_BYCOMMAND, menuitem->wcallback_id, str); 

}

/**
 * Hv_W32CreateHelpPullDown
 * @purpose Windows 9X/NT specific version of Hv_CreateHelpPullDown.
 * @see The gateway routine Hv_CreateHelpPullDown.
 */


Hv_Widget Hv_W32CreateHelpPullDown(void) {
	Hv_Widget w = createNewWidget(MENU);
//	w->wdata = CreateMenu();
	SETHANDLE(w, CreateMenu());
	return w;
}

/**
 * Hv_W32CreatePullDown
 * @purpose Windows 9X/NT specific version of Hv_CreatePullDown.
 * @see The gateway routine Hv_CreatePullDown.
 */

Hv_Widget	Hv_W32CreatePullDown(char *title) {

/* in Win32 this is used to create user menus */

	HMENU   hWndMenu; /* effectively the menu bar */
	int     itemCount;
    Hv_Widget w;

	if (Hv_mainWindow != 0)
	   hWndMenu = GetMenu((HWND)Hv_mainWindow);

	w = createNewWidget(MENU);
//	w->wdata = CreateMenu(); // this is a win32 ID
	SETHANDLE(w, CreateMenu());

	if (standardmenusdone) {

/*
 * NOTE: we are only here if the user is creating a menu.
 * In Win32, we don't pre-create the user menus like we do in X/Motif
 * The "-1" is because of the help menu
 */		
		itemCount = GetMenuItemCount((HMENU)hWndMenu);
		InsertMenu(hWndMenu,
			itemCount - 1,
			MF_POPUP | MF_BYPOSITION,
			(DWORD)(GETHANDLE(w)),
			title);
		DrawMenuBar(Hv_mainWindow); 
	}

	return w;
}

/**
 * Hv_W32AddMenuSeparator
 * @purpose Windows 9X/NT specific version of Hv_AddMenuSeparator.
 * @see The gateway routine Hv_AddMenuSeparator.
 */

Hv_Widget Hv_W32AddMenuSeparator(Hv_Widget menu) { 
	unsigned int itemCount;

    MENUITEMINFO minfo = {
		sizeof(MENUITEMINFO),
		MIIM_TYPE,
		MFT_SEPARATOR,
        MFS_ENABLED,
		-1,
		NULL,
		NULL,
		NULL,
		0,
		NULL,
		0};

    itemCount = GetMenuItemCount(GETMENUHANDLE(menu));

    InsertMenuItem(GETMENUHANDLE(menu),
		                itemCount,
					    True,
					    &minfo);

/* in windows, no need to cache the separator */

	 return NULL;
}


/**
 * Hv_W32AddSubMenuItem
 * @purpose Windows 9X/NT specific version of Hv_AddSubMenuItem.
 * @see The gateway routine Hv_AddSubMenuItem.
 */


Hv_Widget Hv_W32AddSubMenuItem(char *label,
			       Hv_Widget menu,
			       Hv_Widget submenu) {

	char         *menulabel = NULL;
	unsigned int itemCount;
 
 	Hv_Widget w = createNewWidgetEx(NULL, menu, -2, SUBMENUITEM);

	MENUITEMINFO minfo = {
		sizeof(MENUITEMINFO),
		MIIM_ID | MIIM_STATE | MIIM_TYPE | MIIM_SUBMENU | MIIM_DATA,
		MFT_STRING,
        MFS_ENABLED,
		(UINT)GETWIN32ID(w),
		GETMENUHANDLE(submenu),
		NULL,
		NULL,
		0,
		label,
		0};


/* Note that the ID (which is the unique internal ID)
   will serve as the "widget" identifier */

	itemCount = GetMenuItemCount(GETMENUHANDLE(menu));

	if (InsertMenuItem(GETMENUHANDLE(menu),
		                itemCount,
					    True,
						&minfo)) {
		Hv_Free(menulabel);
		return w;
	}
	else {
		Hv_Free(menulabel);
	    return NULL;
	}
}

/**
 * Hv_W32AddMenuToggle
 * @purpose Windows 9X/NT specific version of Hv_AddMenuToggle.
 * @see The gateway routine Hv_AddMenuToggle.
 */

Hv_Widget Hv_W32AddMenuToggle(char *label,
			      Hv_Widget        menu,
			      long             ID,
			      Hv_CallbackProc  callback,
			      Boolean	       state,
			      unsigned char    btype,
			      int	           acctype,
			      char	           accchr) {
	char         *menulabel = NULL;
	unsigned int itemCount;
	Boolean      hasaccel = False;

	Hv_Widget w = createNewWidgetEx(callback, menu, ID, TOGGLEMENUITEM);

	MENUITEMINFO minfo = {
		sizeof(MENUITEMINFO),
		MIIM_ID | MIIM_STATE | MIIM_TYPE | MIIM_DATA,
		MFT_STRING,
        MFS_ENABLED | (state ? MFS_CHECKED : MFS_UNCHECKED),
		(UINT)GETWIN32ID(w),
		NULL,
		NULL,
		NULL,
		0,
		label,
		0};


/* accelerator? */

	if ((accchr != ' ') && (acctype != Hv_NONE)) {

		hasaccel = True;
		menulabel = (char *)Hv_Malloc(Hv_StrLen(label) + 10);
		switch (acctype) {

		case Hv_ALT:
			sprintf(menulabel, "%s\tAlt+%c", label, accchr);
			break;
      
/* default case: CTRL */
	  
		default: 
 			sprintf(menulabel, "%s\tCtrl+%c", label, accchr);
 			break;
		}
		minfo.dwTypeData = menulabel;
	}

    itemCount = GetMenuItemCount(GETMENUHANDLE(menu));

/* Note that the ID (which is the unique internal ID)
   will serve as the "widget" identifier */

	if (InsertMenuItem(GETMENUHANDLE(menu),
		                itemCount,
					    True,
						&minfo)) {
		Hv_Free(menulabel);
		if (hasaccel)
			Hv_AddAccelerator(GETWIN32ID(menu), acctype, accchr);
		return w;
	}
	else {
		Hv_Free(menulabel);
	    return NULL;
	}

}

/**
 * Hv_W32AddMenuItem
 * @purpose Windows 9X/NT specific version of Hv_AddMenuItem.
 * @see The gateway routine Hv_AddMenuItem.
 */

Hv_Widget Hv_W32AddMenuItem(char *label,
			    Hv_Widget        menu,
			    long             userId,
			    Hv_CallbackProc  callback,
			    int	             acctype,
			    char	         accchr) {

	char         *menulabel = NULL;
	unsigned int itemCount;

	Boolean      hasaccel = False;

	Hv_Widget w = createNewWidgetEx(callback, menu, userId, MENUITEM);

	MENUITEMINFO minfo = {
		sizeof(MENUITEMINFO),
		MIIM_ID | MIIM_STATE | MIIM_TYPE | MIIM_DATA,
		MFT_STRING,
        MFS_ENABLED,
		(UINT)GETWIN32ID(w),
		NULL,
		NULL,
		NULL,
		0,
		label,
		0};

/* accelerator? */

	if ((accchr != ' ') && (acctype != Hv_NONE)) {

		hasaccel = True;
		menulabel = (char *)Hv_Malloc(strlen(label) + 10);
		switch (acctype) {

		case Hv_ALT:
			sprintf(menulabel, "%s\tAlt+%c", label, accchr);
			break;
      
/* default case: CTRL */
	  
		default: 
 			sprintf(menulabel, "%s\tCtrl+%c", label, accchr);
 			break;
		}
		minfo.dwTypeData = menulabel;
	}

    itemCount = GetMenuItemCount(GETMENUHANDLE(menu));

/* Deal with action menu special case -- do not want
   item to appear BELOW quit */

	if (Hv_userMenu && (menu == Hv_actionMenu) && (itemCount > 2))
		itemCount -= 2;


/* Note that the ID (which is the unique internal ID)
   will serve as the "widget" identifier */

	if (InsertMenuItem(GETMENUHANDLE(menu),
		                itemCount,
					    True,
						&minfo)) {
		Hv_Free(menulabel);
		if (hasaccel)
			Hv_AddAccelerator(GETWIN32ID(w), acctype, accchr);
		return w;
	}
	else {
		Hv_Free(menulabel);
	    return NULL;
	}
}





/*==========================================
 * MOUSE RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_W32GetCurrentPointerLocation
 * @purpose Windows 9X/NT specific version of Hv_GetCurrentPointerLocation.
 * @see The gateway routine Hv_GetCurrentPointerLocation.
 */

 void  Hv_W32GetCurrentPointerLocation(short *x,
				       short *y) {
 }


/*==========================================
 * COMPOUND STRING  RELATED SECTION OF GATEWAY
 *===========================================*/

/**
 * Hv_W32CreateSimpleCompoundString
 * @purpose Windows 9X/NT specific version of Hv_CreateSimpleCompoundString.
 * @see The gateway routine Hv_CreateSimpleCompoundString.
 */

Hv_CompoundString Hv_W32CreateSimpleCompoundString(char  *text) {
	return Hv_W32CreateCompoundString(text, Hv_fixed2);
}

/**
 * Hv_W32CreateCompoundString
 * @purpose Windows 9X/NT specific version of Hv_CreateCompoundString.
 * @see The gateway routine Hv_CreateCompoundString.
 */

Hv_CompoundString Hv_W32CreateCompoundString(char  *str,
						   short  font) {
	Hv_CompoundString cs;

	cs = (Hv_CompoundString)(Hv_Malloc(sizeof(Hv_CompoundStringData)));
	cs->fillcol = -1;
	cs->font = font;
	cs->horizontal = True;
	cs->offset = 0;
	cs->strcol = Hv_black;

	if (str == NULL)
		cs->text = NULL;
	else
		Hv_InitCharStr(&(cs->text), str);

	return cs;
}


/**
 * Hv_W32CreateAndSizeCompoundString
 * @purpose Windows 9X/NT specific version of Hv_CreateAndSizeCompoundString.
 * @see The gateway routine Hv_CreateAndSizeCompoundString.
 */

Hv_CompoundString Hv_W32CreateAndSizeCompoundString(char  *str,
							  short  font,
							  short *w,
							  short *h) {
	Hv_CompoundString cs;
 	SIZE       size;

	cs = Hv_W32CreateCompoundString(str, font);
	*w = 0;
	*h = 0;

	if ((cs != NULL) && (str != NULL)) {
		Hv_SetMainHDC();

		Hv_SetFont(font);
		GetTextExtentPoint32(Hv_gc, str, strlen(str), &size);
//	   *h = (short)(size.cy + 1);
	   *h = (short)(size.cy-1);
	   *w = (short)(size.cx);
	}

	return cs;
}


/**
 * Hv_W32CompoundStringFree
 * @purpose Windows 9X/NT specific version of Hv_CompoundStringFree.
 * @see The gateway routine Hv_CompoundStringFree.
 */

void Hv_W32CompoundStringFree(Hv_CompoundString cs) {
	if (cs == NULL)
		return;

	if (cs->text != NULL)
		Hv_Free(cs->text);

	Hv_Free(cs);
}

/**
 * Hv_W32CompoundStringDrawImage
 * @purpose Windows 9X/NT specific version of Hv_CompoundStringDrawImage.
 * @see The gateway routine Hv_CompoundStringDrawImage.
 */

void Hv_W32CompoundStringDrawImage(Hv_CompoundString cs,
				   short             x,
				   short             y,
				   short             sw) {
	int len;
	SIZE  size;
	RECT  rect;

	if ((cs == NULL) || (cs->text == NULL))
		return;


	len = strlen(cs->text);

	Hv_SetMainHDC();
	Hv_SetFont(cs->font);
//	Hv_SetBkColor(fbfillcolor);
	Hv_SetTextColor(cs->strcol);

	GetTextExtentPoint32(Hv_gc, cs->text, len, &size);
	rect.left = x;
	rect.top = y;
	rect.bottom = y + size.cy + 1;
//	rect.right = x + size.cx;
	rect.right = x + size.cx + 1;

	ExtTextOut(Hv_gc, x, y, ETO_OPAQUE, &rect, cs->text, len, NULL);
}


/**
 * Hv_W32CompoundStringWidth
 * @purpose Windows 9X/NT specific version of Hv_CompoundStringWidth.
 * @see The gateway routine Hv_CompoundStringWidth. 
 */


short Hv_W32CompoundStringWidth(Hv_CompoundString cs) {

	SIZE   size;

	if ((cs != NULL) && (cs->text != NULL)) {
		Hv_SetMainHDC();

		Hv_SetFont(cs->font);
		GetTextExtentPoint32(Hv_gc, cs->text, strlen(cs->text), &size);
//	    return (short)(size.cx);
	    return (short)(size.cx - 1);
	}
	return 0;
}

/**
 * Hv_W32CompoundStringBaseline
 * @purpose Windows 9X/NT specific version of Hv_CompoundStringBaseline.
 * @see The gateway routine Hv_CompoundStringBaseline. 
 */


short Hv_W32CompoundStringBaseline(Hv_CompoundString cs) {

	TEXTMETRIC   tm;

	if (cs != NULL) {
		Hv_SetMainHDC();
		Hv_SetFont(cs->font);
		GetTextMetrics(Hv_gc, &tm);
		return (short)(tm.tmHeight - tm.tmDescent);
	}

	return 0;
}

/**
 * Hv_W32CompoundStringHeight
 * @purpose Windows 9X/NT specific version of Hv_CompoundStringHeight.
 * @see The gateway routine Hv_CompoundStringHeight. 
 */


short Hv_W32CompoundStringHeight(Hv_CompoundString cs) {

	TEXTMETRIC   tm;

	if (cs != NULL) {
		Hv_SetMainHDC();
		Hv_SetFont(cs->font);
		GetTextMetrics(Hv_gc, &tm);
		return tm.tmHeight + 1;
	}

	return 0;
}

/**
 * Hv_W32CompoundStringDraw
 * @purpose Windows 9X/NT specific version of Hv_CompoundStringDraw.
 * @see The gateway routine Hv_CompoundStringDraw.
 */

void Hv_W32CompoundStringDraw(Hv_CompoundString cs,
			      short             x,
			      short             y,
			      short             sw) {

	int len;
	char msg[100];

	if ((cs == NULL) || (cs->text == NULL))
		return;

    if (Hv_extraFastPlease)
		return;

	len = strlen(cs->text);

	Hv_SetMainHDC();
	Hv_SetBkMode(TRANSPARENT); 
	Hv_SetFont(cs->font);


	if (strstr(cs->text, "Ghost") != NULL) {
		sprintf(msg, "[%s] (%d) (%d)", cs->text, cs->strcol, y);
	}


	Hv_SetTextColor(cs->strcol);
	ExtTextOut(Hv_gc, x, y, 0, NULL, cs->text, len, NULL);
	Hv_SetBkMode(OPAQUE); 
}

/*==========================================
 * REGION RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_W32UnionRectangleWithRegion
 * @purpose Windows 9X/NT specific version of Hv_UnionRectangleWithRegion.
 * @see The gateway routine Hv_UnionRectangleWithRegion.
 */

void Hv_W32UnionRectangleWithRegion(Hv_Rectangle  *xr,
				    Hv_Region     region) {
	
	Hv_Region rreg;
	int right, bottom;

	Hv_Rect rr;

	if (xr == NULL)
		return;

	right  = xr->x + xr->width;
	bottom = xr->y + xr->height;

	rreg = CreateRectRgn(xr->x,
			xr->y, 
			Hv_KeepInRange(right),
			Hv_KeepInRange(bottom));

	UnionRgn(region, region, rreg);

	Hv_W32ClipBox(region, &rr);
	Hv_DestroyRegion(rreg);
}
 

/**
 * Hv_W32IntersectRegion
 * @purpose Windows 9X/NT specific version of Hv_IntersectRegion.
 * @see The gateway routine Hv_IntersectRegion.
 */


Hv_Region Hv_W32IntersectRegion(Hv_Region reg1,
				Hv_Region reg2) {

	Hv_Region   reg3;

	reg3 = Hv_CreateRegion();

	if ((reg1 != 0) && (reg2 != 0))
		IntersectRgn(reg3, reg1, reg2);
	else if (reg1 != 0)
		CopyRgn(reg3, reg1);
	else if (reg2 != 0)
		CopyRgn(reg3, reg2);


	return reg3;
}


/**
 * Hv_W32SubtractRegion
 * @purpose Windows 9X/NT specific version of Hv_SubtractRegion.
 * @see The gateway routine Hv_SubtractRegion.
 */


Hv_Region Hv_W32SubtractRegion(Hv_Region reg1,
			       Hv_Region reg2) {

  Hv_Region   diffreg;

  diffreg = Hv_CreateRegion();

  if (reg1 == 0)
    return diffreg;

  if (reg2 == 0)
	  CopyRgn(diffreg, reg1);
   else {
	SubtractRgn(diffreg, reg1, reg2);
   }

  return diffreg;	
}


/**
 * Hv_W32UnionRegion
 * @purpose Windows 9X/NT specific version of Hv_UnionRegion.
 * @see The gateway routine Hv_UnionRegion.
 */

Hv_Region Hv_W32UnionRegion(Hv_Region reg1,
			    Hv_Region reg2) {

	Hv_Region reg3 = Hv_CreateRegion();

	if ((reg1 != 0) && (reg2 != 0))
		UnionRgn(reg3, reg1, reg2);
	else if (reg1 != 0)
		CopyRgn(reg3, reg1);
	else if (reg2 != 0)
		CopyRgn(reg3, reg2);

	return reg3;
}


/**
 * Hv_W32ShrinkRegion
 * @purpose Windows 9X/NT specific version of Hv_ShrinkRegion.
 * @see The gateway routine Hv_ShrinkRegion.
 */


void Hv_W32ShrinkRegion(Hv_Region region,
			short     h,
			short     v) {

	Hv_Region  rgn1, rgn2, rgn3, rgn4, rgna, rgnb, rgnc, rgnd;

	if (!region)
		return;

	if ((h == 0) && (v == 0))
      return;

	rgn1 = Hv_CreateRegion();
	rgn2 = Hv_CreateRegion();
	rgn3 = Hv_CreateRegion();
	rgn4 = Hv_CreateRegion();

	CopyRgn(rgn1, region);
	CopyRgn(rgn2, region);
	CopyRgn(rgn3, region);
	CopyRgn(rgn4, region);

	OffsetRgn(rgn1, h, v);
	OffsetRgn(rgn2, -h, v);
	OffsetRgn(rgn3, h, -v);
	OffsetRgn(rgn4, -h, -v);


	if ((h < 0) || (v < 0)) {  //grow
		rgna = Hv_UnionRegion(rgn1, region);
		rgnb = Hv_UnionRegion(rgn2, region);
		rgnc = Hv_UnionRegion(rgn3, region);
		rgnd = Hv_UnionRegion(rgn4, region);
		UnionRgn(region, rgna, rgnb);
		UnionRgn(region, region, rgnc);
		UnionRgn(region, region, rgnd);
	}

	else {  // shrink
		rgna = Hv_IntersectRegion(rgn1, region);
		rgnb = Hv_IntersectRegion(rgn2, region);
		rgnc = Hv_IntersectRegion(rgn3, region);
		rgnd = Hv_IntersectRegion(rgn4, region);
		IntersectRgn(region, rgna, rgnb);
		IntersectRgn(region, region, rgnc);
		IntersectRgn(region, region, rgnd);
	}


	Hv_DestroyRegion(rgn1);
	Hv_DestroyRegion(rgn2);
	Hv_DestroyRegion(rgn3);
	Hv_DestroyRegion(rgn4);

	Hv_DestroyRegion(rgna);
	Hv_DestroyRegion(rgnb);
	Hv_DestroyRegion(rgnc);
	Hv_DestroyRegion(rgnd);
}

/**
 * Hv_W32OffsetRegion
 * @purpose Windows 9X/NT specific version of Hv_OffsetRegion.
 * @see The gateway routine Hv_OffsetRegion.
 */


void Hv_W32OffsetRegion(Hv_Region region,
			short     h,
			short     v) {
	OffsetRgn(region, (int)h, (int)v);
}



/**
 * Hv_W32CreateRegionFromPolygon
 * @purpose Windows 9X/NT specific version of Hv_CreateRegionFromPolygon.
 * @see The gateway routine Hv_CreateRegionFromPolygon.
 */

Hv_Region Hv_W32CreateRegionFromPolygon(Hv_Point  *poly,
				       short      n) {
	POINT      *lp;
	Hv_Region   region;


	lp = Hv_PolyToPOINT(poly, (int)n);

	region = CreatePolygonRgn(lp, (int)n, WINDING);
	if (region == NULL) {
		Hv_Println("Null region from %d points", n);
	}

	Hv_Free(lp);
	return region;

}


/**
 * Hv_W32EmptyRegion 
 * @purpose Windows 9X/NT specific version of Hv_EmpytRegion.
 * @see The gateway routine Hv_EmptyRegion.
 */

Boolean  Hv_W32EmptyRegion(Hv_Region region) {

	Hv_Rect   rect;

	Hv_W32ClipBox(region, &rect);
	return ((rect.width < 1) && (rect.height < 1));
}

/**
 * Hv_W32CreateRegion
 * @purpose Windows 9X/NT specific version of Hv_CreateRegion.
 * @see The gateway routine Hv_CreateRegion.
 */
 
Hv_Region  Hv_W32CreateRegion(void) {
	return CreateRectRgn(0, 0, 0, 0);
}


/**
 * Hv_W32DestroyRegion
 * @purpose Windows 9X/NT specific version of Hv_DestroyRegion.
 * @see The gateway routine Hv_DestroyRegion.
 */
 
void  Hv_W32DestroyRegion(Hv_Region region) {
	if (region != 0)
		DeleteObject(region);
}


/**
 * Hv_W32PointInRegion
 * @purpose Windows 9X/NT specific version of Hv_PointInRegion.
 * @see The gateway routine Hv_PointInRegion.  
 */

Boolean Hv_W32PointInRegion(Hv_Point  pp,
			 Hv_Region region) {
	return PtInRegion(region, pp.x, pp.y);
}

/**
 * Hv_W32RectInRegion
 * @purpose Windows 9X/NT specific version of Hv_RectInRegion.
 * @see The gateway routine Hv_RectInRegion.
 */

Boolean  Hv_W32RectInRegion(Hv_Rect   *r,
			 Hv_Region region) {
	RECT   w32rect;

	if (r == NULL)
		return False;

	Hv_RectToW32Rect(r, &w32rect);
	return RectInRegion(region, &w32rect);
}


/**
 * Hv_W32SetFunction
 * @purpose Windows 9X/NT specific version of Hv_SetFunction.
 * @see The gateway routine Hv_SetFunction.
 */

void Hv_W32SetFunction(int funcbit) {

	Hv_SetMainHDC();
    SetROP2(Hv_gc, funcbit);
}


/**
 * Hv_W32GCValues
 * @purpose Windows 9X/NT specific version of Hv_GCValues.
 * @see The gateway routine Hv_GCValues.
 */

void Hv_W32GetGCValues(int *gcv) {

	Hv_SetMainHDC();
	*gcv = GetROP2(Hv_gc);
}


/**
 * Hv_W32ClipBox
 * @purpose Windows 9X/NT specific version of Hv_ClipBox.
 * @see The gateway routine Hv_ClipBox.
 */

void     Hv_W32ClipBox(Hv_Region  region,
		    Hv_Rect   *rect) {

	RECT  w32rect;

	if (region == 0) {
		Hv_SetRect(rect, 0, 0, 0, 0);
		return;
	}

	GetRgnBox(region, &w32rect);
	Hv_W32RectToRect(&w32rect, rect);
}

/**
 * Hv_W32CheckForExposures
 * @purpose Windows 9X/NT specific version of Hv_CheckForExposures.
 * @see The gateway routine Hv_CheckForExposures.  
 */

void Hv_W32CheckForExposures(int whattype) {
}

/*==========================================
 * DRAWING RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_W32SetLineStyle
 * @purpose Windows 9X/NT specific version of Hv_SetLineStyle.
 * @see The gateway routine Hv_SetLineStyle. 
 */


void Hv_W32SetLineStyle(int w,
		     int style) {

	HPEN      hpen;
	LOGPEN    logpen;

	Hv_SetLineWidth(w);
	hpen = GetCurrentObject(Hv_gc, OBJ_PEN);

	GetObject(hpen, sizeof(LOGPEN), &logpen);


	switch(style) {
	case Hv_SOLIDLINE:
		logpen.lopnStyle = PS_SOLID;
		break;

	case Hv_DASHED:
		logpen.lopnStyle = PS_DASH;
		break;

	case Hv_LONGDASHED:
		logpen.lopnStyle = PS_DASHDOTDOT;
		break;

	case Hv_DOTDASH:
		logpen.lopnStyle = PS_DASHDOT;
		break;

	default:
		logpen.lopnStyle = PS_SOLID;
	}

	DeleteObject(SelectObject(Hv_gc, CreatePenIndirect(&logpen)));

}


/**
 * Hv_W32SetPattern
 * @purpose Windows 9X/NT specific version of Hv_SetPattern.
 * @see The gateway routine Hv_SetPattern. 
 */


void Hv_W32SetPattern(int pattern,
		   short color) {

	if (!patternsInited) {
		Hv_W32InitPatterns();
		patternsInited = True;
	}

	if ((pattern < 0) || (pattern >= Hv_NUMPATTERNS))
		pattern = Hv_FILLSOLIDPAT;

	currentPattern = pattern;

	Hv_SetMainHDC();

}


/**
 * Hv_SetBrush
 * @purpose  Set the brush based on the pattern
 * and color
 * @param   pat    the pattern
 * @param   color  the brush
 */

static void Hv_SetBrush(short pat,
		short color) {


	HBITMAP hbm = 0;

	Hv_SetMainHDC();

	if (HighlightMode) {
		Hv_SetFunction(Hv_GXCOPY);     /* xor's the GC's drawing function */
		HighlightMode = False;
	}
	
     if (color == Hv_highlight) {
			Hv_SetFunction(Hv_GXXOR);     /* xor's the GC's drawing function */
			HighlightMode = True;
			DeleteObject(SelectObject(Hv_gc, CreateSolidBrush(highlightPixel)));
			return;
	 }


/*
 * For now, use one of the hatch patterns until I figure how to make
 * the bitmaps draw transparent
 */

	switch (pat) {
		case Hv_FILLSOLIDPAT:
			DeleteObject(SelectObject(Hv_gc, CreateSolidBrush(Hv_pixelColor(color))));
			break;

		case Hv_80PERCENTPAT:
			DeleteObject(SelectObject(Hv_gc, CreateHatchBrush(HS_DIAGCROSS, Hv_pixelColor(color))));
			break;

		case Hv_50PERCENTPAT:
			DeleteObject(SelectObject(Hv_gc, CreateHatchBrush(HS_DIAGCROSS, Hv_pixelColor(color))));
			break;

		case Hv_20PERCENTPAT:
			DeleteObject(SelectObject(Hv_gc, CreateHatchBrush(HS_CROSS, Hv_pixelColor(color))));
			break;

		case Hv_HATCHPAT:
			DeleteObject(SelectObject(Hv_gc, CreateHatchBrush(HS_CROSS, Hv_pixelColor(color))));
			break;

		case Hv_DIAGONAL1PAT:
			DeleteObject(SelectObject(Hv_gc, CreateHatchBrush(HS_BDIAGONAL, Hv_pixelColor(color))));
			break;

		case Hv_DIAGONAL2PAT:
			DeleteObject(SelectObject(Hv_gc, CreateHatchBrush(HS_FDIAGONAL, Hv_pixelColor(color))));
			break;

		case Hv_HSTRIPEPAT:
			DeleteObject(SelectObject(Hv_gc, CreateHatchBrush(HS_HORIZONTAL, Hv_pixelColor(color))));
			break;

		case Hv_VSTRIPEPAT:
			DeleteObject(SelectObject(Hv_gc, CreateHatchBrush(HS_VERTICAL, Hv_pixelColor(color))));
			break;

		case Hv_SCALEPAT:
		case Hv_STARPAT:
			hbm = Hv_CreateBitMapFromPattern(pat, color);
			DeleteObject(SelectObject(Hv_gc, CreatePatternBrush(hbm)));
			DeleteObject(hbm);
			break;

		case Hv_HOLLOWPAT:
		default:
			DeleteObject(SelectObject(Hv_gc, GetStockBrush(NULL_BRUSH)));
	}

}


/**
 * Hv_W32EraseRectangle
 * @purpose Windows 9X/NT  specific version of Hv_EraseRectangle.
 * @see The gateway routine Hv_EraseRectangle. 
 */

void	Hv_W32EraseRectangle(short left,
			  short top,
			  short width,
			  short height) {
    
	Hv_ClearArea(left, top, width, height);
}

/**
 * Hv_W32TileMainWindow
 * @purpose Windows 9X/NT specific version of Hv_TileMainWindow.
 * @see The gateway routine Hv_TileMainWindow. 
 */


void   Hv_W32TileMainWindow(Hv_Pixmap pmap) {
    if ((pmap == 0) || (Hv_display == NULL) || (Hv_mainWindow == 0))
	return;
    
	logo = pmap;
	InvalidateRect(Hv_mainWindow, NULL, True);

}

/**
 * Hv_W32ReTileMainWindow
 * @purpose Windows 9X/NT specific version of Hv_ReTileMainWindow.
 * @see The gateway routine Hv_TileMainWindow. 
 */

void Hv_W32ReTileMainWindow(void) {

    Hv_Rect   prect;

	if (Hv_mainWindow == 0) {
		return;
	}

  if (logo != 0)
	Hv_FreePixmap(logo);

  if ((Hv_TileDraw != NULL) &&(Hv_mainWindow != 0)) {
    logo = Hv_CreatePixmap(Hv_tileX, Hv_tileY);
	DeleteObject(SelectObject(hdcmem, (HBITMAP)logo));
    Hv_drawingOffScreen = True;

	Hv_SetRect(&prect, 0, 0, (short)Hv_tileX, (short)Hv_tileY);
    Hv_ClipRect(&prect);

    Hv_TileDraw();
    Hv_drawingOffScreen = False;
    Hv_gc = (HANDLE)(-1);
    Hv_TileMainWindow(logo);
	Hv_ChangeWindowBackground(Hv_canvasColor);
  }
}


/**
 * Hv_W32SetBackground
 * @purpose Windows 9X/NT specific version of Hv_SetBackground.
 * @see The gateway routine Hv_SetBackground. 
 */


void  Hv_W32SetBackground(unsigned long bg) {
	SetBkColor(Hv_gc, bg);
}


/**
 * Hv_W32SetForeground
 * @purpose Windows 9X/NT specific version of Hv_SetForeground.
 * @see The gateway routine Hv_SetForeground. 
 */


void  Hv_W32SetForeground(unsigned long fg) {
}


/**
 * Hv_W32SetFillStyle
 * @purpose Windows 9X/NT specific version of Hv_SetFillStyle.
 * @see The gateway routine Hv_SetFillStyle. 
 */


void  Hv_W32SetFillStyle(int fillstyle) {
}


/**
 * Hv_W32SetLineWidth
 * @purpose Windows 9X/NT specific version of Hv_SetLineWidth.
 * @see The gateway routine Hv_SetLineWidth. 
 */


void Hv_W32SetLineWidth(int w) {
	Hv_SetPenWidth(w);
}



/**
 * Hv_W32SetClippingRegion
 * @purpose Windows 9X/NT specific version of Hv_SetClippingRegion.
 * @see The gateway routine Hv_SetClippingRegion. 
 */


void  Hv_W32SetClippingRegion(Hv_Region region) {
	Hv_SetMainHDC();
	SelectClipRgn(Hv_gc, region);
//	DeleteObject(region);
}


/**
 * Hv_W32FreePixmap
 * @purpose  Windows 9X/NT specific version of Hv_FreePixmap. 
 * @see The gateway routine Hv_FreePixmap.  
 */


void  Hv_W32FreePixmap(Hv_Pixmap pmap) {
	if (pmap != 0)
		DeleteObject((HBITMAP)pmap);
}


/**
 * Hv_W32CreatePixmap
 * @purpose  Windows 9X/NT specific version of Hv_CreatePixmap. 
 * @see The gateway routine Hv_CreatePixmap.  
 */


Hv_Pixmap  Hv_W32CreatePixmap(unsigned int  w,
			      unsigned int  h) {
	Hv_SetMainHDC();
	return (HWND)(CreateCompatibleBitmap(Hv_gc, w, h));
}

/**
 * Hv_W32CreatePixmap
 * @purpose  Windows 9X/NT specific version of Hv_CreatePixmap. 
 * @see The gateway routine Hv_CreatePixmap.  
 */


static Hv_Pixmap  Hv_W32CreatePixmapFromDC(HDC  hdc,
									unsigned int  w,
									unsigned int  h) {
	return (HWND)(CreateCompatibleBitmap(hdc, w, h));
}

/**
 * Hv_W32ClearArea
 * @purpose Windows 9X/NT specific version of Hv_ClearArea.
 * @see The gateway routine Hv_ClearArea. 
 */

void Hv_W32ClearArea(short x,
				     short y,
				     short w,
					 short h) {
	RECT   w32r;
	w32r.left   = x;
	w32r.top    = y;
	w32r.right  = x + w;
	w32r.bottom = y + h;
		
	InvalidateRect(Hv_mainWindow, &w32r, True);
}

/**
 * Hv_W32CopyArea
 * @purpose Windows 9X/NT specific version of Hv_CopyArea.
 * @see The gateway routine Hv_CopyArea. 
 */

void Hv_W32CopyArea(Hv_Window src,
		 Hv_Window dest,
		 int       sx,
		 int       sy,
		 unsigned  int w,
		 unsigned  int h,
		 int       dx,
		 int       dy) {

   HDC hdcs, hdcd, hdcolds, hdcoldd, hdc;
   Hv_View   view;

	if ((src == Hv_trueMainWindow) && (dest == Hv_trueMainWindow)) {
		HDC hdc = GetDC(Hv_mainWindow);
		BitBlt(Hv_gc, dx, dy, w, h, Hv_gc, sx, sy, SRCCOPY);
		ReleaseDC(Hv_mainWindow, hdc);
	}
	else if ((src == Hv_trueMainWindow) && (dest != Hv_trueMainWindow)) { // main win to pm
        hdcd = CreateCompatibleDC(Hv_gc);
		hdcoldd = SelectObject(hdcd, (HBITMAP)dest);
		BitBlt(hdcd, dx, dy, w, h, Hv_gc, sx, sy, SRCCOPY);
		if (hdcoldd != 0)
			SelectObject(hdcoldd, (HBITMAP)dest);
		DeleteObject(hdcd);
	}
	else if ((src != Hv_trueMainWindow) && (dest == Hv_trueMainWindow)) { // pm to main win
		Hv_W32RestoreSaveUnder((HBITMAP)src, (short)(dx+1), (short)(dy+1), w-2, h-2);
	}
	else { // pixmap to pixmap

		for (view = Hv_views.first;  view != NULL;  view = view->next) {
			if (view->background == src) {

				hdc = GetDC(Hv_trueMainWindow);
				hdcd = CreateCompatibleDC(hdc);
				ReleaseDC(Hv_trueMainWindow, hdc);
        DeleteObject(SelectObject(view->memoryhdc, (HBITMAP)(view->background )));

				SelectObject(hdcd, (HBITMAP)dest);
				BitBlt(hdcd, dx, dy, w, h, view->memoryhdc, sx, sy, SRCCOPY);
				return;
			}

		}

        hdcs = CreateCompatibleDC(Hv_gc);
        hdcd = CreateCompatibleDC(Hv_gc);
		hdcolds = SelectObject(hdcs, (HBITMAP)src);
		hdcoldd = SelectObject(hdcd, (HBITMAP)dest);

		if (hdcolds != 0) {
			DeleteObject(SelectObject(hdcolds, (HBITMAP)src));
		    BitBlt(hdcd, dx, dy, w, h, hdcolds, sx, sy, SRCCOPY);
			DeleteObject(hdcolds);
		}
		
		else {
			BitBlt(hdcd, dx, dy, w, h, hdcs, sx, sy, SRCCOPY);
			DeleteObject(hdcs);
			DeleteObject(hdcd);
		}
	}
}


/**
 * Hv_W32DestroyImage
 * @purpose Windows 9X/NT  specific version of Hv_DestroyImage.
 * @see The gateway routine Hv_DestroyImage. 
 */

void Hv_W32DestroyImage(Hv_Image *image) {
	HBITMAP hbm = (HBITMAP)image;
	if (hbm != 0)
		DeleteObject(hbm);
}

/**
 * Hv_W32PutImage
 * @purpose Windows 9X/NT specific version of Hv_PutImage.
 * @see The gateway routine Hv_PutImage. 
 */


void Hv_W32PutImage(Hv_Window window,
		 Hv_Image *image,
		 int       src_x,
		 int       src_y,
		 int       dest_x,
		 int       dest_y,
		 unsigned  int w,
		 unsigned  int h) {
}


/**
 * Hv_W32DrawTextItemVertical
 * @purpose Windows 9X/NT specific version of Hv_DrawTextItemVertical.
 * @see The gateway routine Hv_DrawTextItemVertical. 
 */


void Hv_W32DrawTextItemVertical(Hv_Item    Item,
				Hv_Region  region) {

	HFONT hfnt, hfntPrev; 
    char  buffer[256];
	int   status;
    TEXTMETRIC tm;
 
	PLOGFONT plf = (PLOGFONT) LocalAlloc(LPTR, sizeof(LOGFONT));

    if ((Item == NULL) || (Item->str == NULL) || (Item->str->text == NULL)) {
        LocalFree((LOCALHANDLE) plf); 
		return;
	}
	
	Hv_SetMainHDC();
	Hv_SetFont(Item->str->font);
	
	status = GetTextFace(Hv_gc, 256, buffer);

/* Specify a font typeface name and weight. */
 
    lstrcpy(plf->lfFaceName, buffer);


	 GetTextMetrics(Hv_gc,&tm);
	 plf->lfHeight = tm.tmHeight;
	 plf->lfWeight = tm.tmWeight;
/*
	if (Item->str->font <= Hv_fixedBold11)
       plf->lfWeight = FW_NORMAL;
	else
		plf->lfWeight = FW_BOLD;*/
 
	// Set the background mode to transparent for the 
// text-output operation. 
 
    SetBkMode(Hv_gc, TRANSPARENT); 
 

    plf->lfEscapement = 900; 
    hfnt = CreateFontIndirect(plf); 
    hfntPrev = SelectObject(Hv_gc, hfnt); 
    TextOut(Hv_gc, Item->area->left, Item->area->bottom, 
        Item->str->text, lstrlen(Item->str->text)); 
    SelectObject(Hv_gc, hfntPrev); 
    DeleteObject(hfnt); 

 
// Reset the background mode to its default. 
 
    SetBkMode(Hv_gc, OPAQUE); 
 
// Free the memory allocated for the LOGFONT structure. 
 
    LocalFree((LOCALHANDLE) plf); 
}

/**
 * Hv_W32DrawPoint
 * @purpose Windows 9X/NT specific version of Hv_DrawPoint.
 * @see The gateway routine Hv_DrawPoint.
 */

void Hv_W32DrawPoint(short x,
		     short y,
		     short color) {

	RECT w32rect;

	Hv_XYToW32Rect(x, y, &w32rect);
	Hv_FillW32Rectangle(&w32rect, color);
}


/**
 * Hv_W32DrawPoints
 * @purpose Windows 9X/NT specific version of Hv_DrawPoints.
 * @see The gateway routine Hv_DrawPoints.
 */

void Hv_W32DrawPoints(Hv_Point *xp,
		      int       np,
		      short     color) {

	int i;

	for (i = 0; i < np; i++)
		Hv_W32DrawPoint(xp[i].x, xp[i].y, color);

}

/**
 * Hv_W32FrameArc
 * @purpose Windows 9X/NT specific version of Hv_FrameArc.
 * @see The gateway routine Hv_FrameArc.
 */

void	Hv_W32FrameArc(short   x,
		       short   y,
		       short   width,
		       short   height,
		       int     ang1,
		       int     ang2,
		       short   color) {

	int   right, bottom, temp;
	int   startx, starty, endx, endy;
	float theta1, theta2, thetacrit;


	if ((width < 1) || (height < 1))
		return;

	if (ang2 < ang1) {
		temp = ang2;
		ang2 = ang1;
		ang1 = temp;
	}

// ang1 and ang2 are in 64ths of a degree

	theta1 = (float)(ang1/64.0);
    theta2 = (float)(ang2/64.0);
	thetacrit = (float)(atan2((float)height, (float)width));


	Hv_SetPenColor(color);

	right  = (int)(x + width);
	bottom = (int)(y + height);

	Hv_RadiusRectIntersection(x, y, right, bottom,
		theta1, thetacrit, &startx, &starty);

	Hv_RadiusRectIntersection(x, y, right, bottom,
		theta2, thetacrit, &endx, &endy);
	
	Arc(Hv_gc, x, y, right, bottom, startx, starty, endx, endy);
}




/**
 * Hv_W32FillArc
 * @purpose Windows 9X/NT specific version of Hv_FillArc.
 * @see The gateway routine Hv_FillArc.
 */

void  Hv_W32FillArc(short   x,
		    short   y,
		    short   width,
		    short   height,
		    int     ang1,
		    int     ang2,
		    Boolean frame,
		    short   color,
		    short   framec) {

	int   temp, right, bottom;
	int   startx, starty, endx, endy;
	float theta1, theta2, thetacrit;


	if ((width < 1) || (height < 1))
		return;

	if (ang2 < ang1) {
		temp = ang2;
		ang2 = ang1;
		ang1 = temp;
	}

// ang1 and ang2 are in 64ths of a degree

	theta1 = (float)(ang1/64.0);
    theta2 = (float)(ang2/64.0);
	thetacrit = (float)(atan2((float)height, (float)width));

	Hv_SetBrush(currentPattern, color);

	if (frame) 
		Hv_SetPenColor(framec);
	else
		Hv_SetPenColor(color);

	right  = (int)(x + width);
	bottom = (int)(y + height);

	Hv_RadiusRectIntersection(x, y, right, bottom,
		theta1, thetacrit, &startx, &starty);

	Hv_RadiusRectIntersection(x, y, right, bottom,
		theta2, thetacrit, &endx, &endy);
	
	Pie(Hv_gc, x, y, right, bottom, startx, starty, endx, endy);

}



/**
 * Hv_W32DrawLines
 * @purpose Windows 9X/NT specific version of Hv_DrawLines.
 * @see The gateway routine Hv_DrawLines.
 */

void Hv_W32DrawLines(Hv_Point  *xp,
		     short      np,
		     short      color) {

	POINT  *lp;

	Hv_SetPenColor(color);
	lp = Hv_PolyToPOINT(xp, (int)np);
	Polyline(Hv_gc, lp, (int)np);
	Hv_Free(lp);
}

/**
 * Hv_W32FillPolygon
 * @purpose Windows 9X/NT specific version of Hv_FillPolygon.
 * @see The gateway routine Hv_FillPolygon.
 */

void Hv_W32FillPolygon(Hv_Point   *xp,
		       short       np,
		       Boolean     frame,
		       short       fillcolor,
		       short       framecolor) {
    
	POINT    *lp;

	if (fillcolor >= 0) {		
		Hv_SetBrush(currentPattern, fillcolor);
		Hv_SetPenColor(fillcolor);
 
		lp = Hv_PolyToPOINT(xp, (int)np);
		Polygon(Hv_gc, lp, (int)np);
		Hv_Free(lp);
	}

/* note: Hv_FramePolygon is NOT a gateway function */
    
  if (frame)
    Hv_FramePolygon(xp, np, framecolor);
  
}

/**
 * Hv_W32DrawSegments
 * @purpose Windows 9X/NT specific version of Hv_DrawSegments.
 * @see The gateway routine Hv_DrawSegments.
 */

void Hv_W32DrawSegments(Hv_Segment *xseg,
		     short       nseg,
		     short       color) {

	int i;
	int   np = 2*nseg;

	if (np < 2)
		return;

	for (i = 0; i < nseg; i++)

		Hv_W32DrawLine(xseg[i].x1,
			xseg[i].y1,
			xseg[i].x2,
			xseg[i].y2,
			color);
}

/**
 * Hv_W32FillRect
 * @purpose Windows 9X/NT specific version of Hv_FillRect.
 * @see The gateway routine Hv_FillRect.
 */


void	Hv_W32FillRect(Hv_Rect *rect,
		      short    color) {


	RECT   w32rect;
	Hv_RectToW32Rect(rect, &w32rect);
	Hv_FillW32Rectangle(&w32rect, color);	
}

/**
 * Hv_W32FrameRect
 * @purpose Windows 9X/NT specific version of Hv_FrameRect.
 * @see The gateway routine Hv_FrameRect.
 */

void Hv_W32FrameRect (Hv_Rect *rect,
		     short     color) {

	Hv_Point  pp[5];
	Hv_SetPoint(pp, rect->left,  rect->top);
	Hv_SetPoint(pp+1, rect->right, rect->top);
	Hv_SetPoint(pp+2, rect->right, rect->bottom);
	Hv_SetPoint(pp+3, rect->left,  rect->bottom);
	Hv_SetPoint(pp+4, rect->left,  rect->top);
	Hv_W32DrawLines(pp, 5, color);

/*
	HBRUSH hBrush;
	RECT   w32rect;

	Hv_SetMainHDC();

	hBrush = CreateSolidBrush(Hv_colors[color]);

	Hv_RectToW32Rect(rect, &w32rect);
	
	FrameRect(Hv_gc, &w32rect, hBrush);
	DeleteObject(hBrush);
	*/

}


/**
 * Hv_W32FillRectangle
 * @purpose Windows 9X/NT specific version of Hv_FillRectangle.
 * @see The gateway routine Hv_FillRectangle.
 */

void	Hv_W32FillRectangle(short left,
			    short top,
			    short width,
			    short height,
			    short color) {
	RECT rect;
	rect.left   = left;
	rect.top    = top;
	rect.right  = left + width;
	rect.bottom = top + height;
	Hv_FillW32Rectangle(&rect, color);

}


/**
 * Hv_W32FillRectangles
 * @purpose Windows 9X/NT specific version of Hv_FillRectangles.
 * @see The gateway routine Hv_FillRectangles.
 */

void   Hv_W32FillRectangles(Hv_Rectangle *rects,
			    int         np,
			    short       color) {

	RECT rect;
	int  i;


	if ((rects == NULL) || (color < 0) || (np < 1))
		return;

	Hv_SetPenColor(color);
	Hv_SetBrush(currentPattern, color);
 
	for (i = 0; i < np; i++) {
		Hv_RectangleToW32Rect(rects+i, &rect);
	    Rectangle(Hv_gc, rect.left, rect.top, rect.right, rect.bottom);
	}

	Hv_HighlightOff();
}


/**
 * Hv_W32FrameRectangle
 * @purpose Windows 9X/NT specific version of Hv_FrameRectangle.
 * @see The gateway routine Hv_FrameRectangle.
 */

void Hv_W32FrameRectangle (short x,
			   short y,
			   short w,
			   short h,
			   short color) {


	Hv_Point  pp[5];
	short   r = x + w;
	short   b = y + h;

	Hv_SetPoint(pp, x, y);
	Hv_SetPoint(pp+1, r, y);
	Hv_SetPoint(pp+2, r, b);
	Hv_SetPoint(pp+3, x, b);
	Hv_SetPoint(pp+4, x, y);
	Hv_W32DrawLines(pp, 5, color);
/*
	RECT rect;

	HBRUSH hBrush;

	if (color < 0)
		return;

	Hv_SetMainHDC();

	hBrush = CreateSolidBrush(Hv_colors[color]);

	rect.left   = x;
	rect.top    = y;
	rect.right  = x + w;
	rect.bottom = y + h;
	FrameRect(Hv_gc, &rect, hBrush);
	DeleteObject(hBrush);
	*/

}

/**
 * Hv_W32DrawLine
 * @purpose Windows 9X/NT specific version of Hv_DrawLine.
 * @see The gateway routine Hv_DrawLine.
 */

void Hv_W32DrawLine(short x1,
		    short y1,
		    short x2,
		    short y2,
		    short color) {

	POINT     points[2];

	Hv_SetPenColor(color);

	points[0].x = x1;   points[0].y = y1;
	points[1].x = x2;   points[1].y = y2;

	Polyline(Hv_gc, points, 2);

}


/**
 * Hv_W32SimpleDrawString
 * @purpose X Windows specific version of Hv_SimpleDrawString.
 * @see The gateway routine Hv_SimpleDrawString.  
 */


void    Hv_W32SimpleDrawString(short x,
			      short y,
			       Hv_String *str) {


    Hv_CompoundString cs;			/* Hv compound string */
    short             sw, sh;		        /* String width  in pixels */
    
    Hv_CompoundStringDimension(str, &sw, &sh);
    cs = Hv_CreateAndSizeCompoundString(str->text, str->font, &sw, &sh);
    
    cs->strcol = str->strcol;
        
    if (Hv_useDrawImage)
	Hv_CompoundStringDrawImage(cs, x, y, sw);
    else
	Hv_CompoundStringDraw(cs, x, y, sw);
    
    Hv_CompoundStringFree(cs);
}

/**
 * Hv_W32DrawString
 * @purpose Windows 9X/NT specific version of Hv_DrawString.
 * @see The gateway routine Hv_DrawString.
 */

void    Hv_W32DrawString(short x,
			 short y,
			 Hv_String *str,
			 Hv_Region region) {

	int len;
	Boolean  status;

	if ((str == NULL) || (str->text == NULL))
		return;

	len = strlen(str->text);

	if (Hv_useDrawImage) {
		Hv_SetBkMode(OPAQUE); 
	}
	else
		Hv_SetBkMode(TRANSPARENT);
	
	Hv_SetFont(str->font);
	Hv_SetTextColor(str->strcol);

	status = ExtTextOut(Hv_gc, x, y, 0, NULL, str->text, len, NULL);

}


/*==========================================
 * UTILITY SECTION OF GATEWAY
 *===========================================*/

/**
 * Hv_W32GetXEventX
 * @purpose Windows 9X/NT specific version of Hv_GetXEventX.
 * @see The gateway routine Hv_GetXEventX.
 */

short Hv_W32GetXEventX(Hv_XEvent *event) {
	if (event == NULL)
		return -32000;

	return (short)event->x;
}


/**
 * Hv_W32GetXEventY
 * @purpose Windows 9X/NT specific version of Hv_GetXEventY.
 * @see The gateway routine Hv_GetXEventY.
 */

short Hv_W32GetXEventY(Hv_XEvent *event) {
	if (event == NULL)
		return -32000;

	return (short)event->y;
}


/**
 * Hv_W32GetXEventButton
 * @purpose Windows 9X/NT specific version of Hv_GetXEventButton.
 * @see The gateway routine Hv_GetXEventButton.
 */

short Hv_W32GetXEventButton(Hv_XEvent *event) {
	if (event == NULL)
		return 0;

	return (short)event->button;
}

/**
 * Hv_W32AddModifier
 * @purpose Windows 9X/NT specific version of Hv_AddModifier.
 * @see The gateway routine Hv_AddModifier.
 */

void Hv_W32AddModifier(Hv_XEvent  *event,
		      int         modbit) {
    switch (modbit) {
		case Hv_SHIFT:
		event->modifiers |= ShiftMask;
		break;

		case Hv_MOD1:
		event->modifiers |= Mod1Mask;
		break;

		case Hv_CONTROL:
		event->modifiers |= ControlMask;
		break;
	}
}


/**
 * Hv_W32Shifted
 * @purpose Windows 9X/NT specific version of Hv_Shifted.
 * @see The gateway routine Hv_Shifted.
 */

Boolean Hv_W32Shifted(Hv_XEvent  *event) {
	if (event == NULL)
		return False;

	return ((event->modifiers & ShiftMask) == ShiftMask);
}

/**
 * Hv_W32AltPressed
 * @purpose Windows 9X/NT specific version of Hv_AltPressed.
 * @see The gateway routine Hv_AltPressed.
 */

Boolean Hv_W32AltPressed(Hv_XEvent  *event) {
	if (event == NULL)
		return False;

	return ((event->modifiers & Mod1Mask) == Mod1Mask);
}

/**
 * Hv_W32ControlPressed
 * @purpose Windows 9X/NT specific version of Hv_ControlPressed.
 * @see The gateway routine Hv_ControlPressed.
 */

Boolean Hv_W32ControlPressed(Hv_XEvent  *event) {
	if (event == NULL)
		return False;

	return ((event->modifiers & ControlMask) == ControlMask);
}


/**
 * Hv_W32Pause
 * @purpose Windows 9X/NT specific version of Hv_Pause.
 * @see The gateway routine Hv_Pause.
 */

 void  Hv_W32Pause(int    interval, // in millisec
		   Hv_Region region) {

	 Sleep(interval); //blocks

 }

/**
 * Hv_W32SysBeep
 * @purpose Windows 9X/NT specific version of Hv_SysBeep.
 * @see The gateway routine Hv_SysBeep.
 */

void  Hv_W32SysBeep(void) {
//	MessageBeep(0xFFFFFFFF);
	MessageBeep(MB_ICONQUESTION);
}

/**
 * Hv_W32Flush
 * @purpose Windows 9X/NT specific version of Hv_Flush.
 * @see The gateway routine Hv_Flush.
 */

 void  Hv_W32Flush(void) {
	 GdiFlush();
 }

/**
 * Hv_W32Sync
 * @purpose Windows 9X/NT specific version of Hv_Sync.
 * @see The gateway routine Hv_Sync.
 */

 void  Hv_W32Sync(void) {
 }


/**
 * Hv_W32SetCursor
 * @purpose Windows 9X/NT specific version of Hv_SetCursor.
 * @see The gateway routine Hv_SetCursor.
 */


void  Hv_W32SetCursor(Hv_Cursor curs) {
	SetCursor(curs);
}

/**
 * Hv_W32SetString
 * @purpose Windows 9X/NT specific version of Hv_SetString.
 * @see The gateway routine Hv_SetString.  
 */

void Hv_W32SetString(Hv_Widget w,
		  char      *s) {


	if (s == NULL)
		return;

	if(w->wtextString != NULL)
		Hv_Free(w->wtextString);

	w->wtextString = (char *) malloc(Hv_StrLen(s) +1);
	w->def_iwidth = max(Hv_StrLen(s) *4,w->def_iwidth);
	strcpy(w->wtextString,s);
	
	if(w->wisWisable)  /* dialog box is being display so update the string */
	{
		SetDlgItemText(  w->msdialog,  // handle to dialog box
                    w->wcallback_id,   // identifier of control
                    s);                // pointer to buffer for text

	}
	
}


/**
 * Hv_W32GetString
 * @purpose Windows 9X/NT specific version of Hv_GetString.
 * @see The gateway routine Hv_GetString.  
 */

char  *Hv_W32GetString(Hv_Widget w) {


	char *string = NULL;
	long tlength = 0;


	if(w->wisWisable)
	{
		tlength = SendDlgItemMessage( w->msdialog, w->wcallback_id, WM_GETTEXTLENGTH , 0,0);
		if(tlength >0)
			{
			  if(w->wtextString)
				  Hv_Free(w->wtextString);
			  w->wtextString = (char *)Hv_Malloc(tlength+1);
			  string = (char *)Hv_Malloc(tlength+1);
			  SendDlgItemMessage( w->msdialog, w->wcallback_id, WM_GETTEXT , 
				                        (WPARAM) tlength+1, (LPARAM) w->wtextString);
			  strcpy(string,w->wtextString);
			}

	}
    else
	{
	  if(w->wtextString)
	  {
	     string = (char *)Hv_Malloc(Hv_StrLen(w->wtextString)+1);
	     strcpy(string,w->wtextString);
	  }
	  else 
        string = " ";
	}
	return string;
}



/**
 * Hv_W32Parent
 * @purpose Windows 9X/NT specific version of Hv_Parent.
 * @see The gateway routine Hv_Parent.  
 */

Hv_Widget  Hv_W32Parent(Hv_Widget w) {
	return w->parent;
}



/**
 * Hv_W32RemoveAllCallbacks
 * @purpose Windows 9X/NT specific version of Hv_RemoveAllCallbacks.
 * @see The gateway routine Hv_RemoveAllCallbacks.  
 */

void  Hv_W32RemoveAllCallbacks(Hv_Widget w,
			       char *name) {

}

/**
 * Hv_W32VaCreateWidget
 * @purpose Windows 9X/NT specific version of Hv_VaCreateWidget.
 * @see The gateway routine VaCreateWidget.  
 */

Hv_Widget  Hv_W32VaCreateWidget(char            *name,
				       Hv_WidgetClass   widgetclass,
				       Hv_Widget        parent,
				       ...) {
	return NULL;
}

/**
 * Hv_W32VaCreateManagedWidget
 * @purpose Windows 9X/NT specific version of Hv_VaCreateManagedWidget.
 * @see The gateway routine VaCreateManagedWidget.  
 */

Hv_Widget  Hv_W32VaCreateManagedWidget(char            *name,
				       Hv_WidgetClass   widgetclass,
				       Hv_Widget        parent,
				       ...) {
	return NULL;
}

/**
 * Hv_W32ManangeChild
 * @purpose Windows 9X/NT specific version of Hv_ManangeChild.
 * @see The gateway routine Hv_ManangeChild.
 */

void  Hv_W32ManageChild(Hv_Widget w) {
}


/**
 * Hv_W32UnmanangeChild
 * @purpose Windows 9X/NT specific version of Hv_UnmanangeChild.
 * @see The gateway routine Hv_UnmanangeChild.
 */

void  Hv_W32UnmanageChild(Hv_Widget w) {

	HMENU hmenu = NULL;
    Hv_CallbackData  wd = NULL;

	if (w == NULL)
		return;

	wd = Hv_GetCallbackData(GETWIN32ID(w));
	if (wd == NULL)
		return;

	switch(w->wtype) {
		case MENUITEM: case TOGGLEMENUITEM: case SUBMENUITEM:
			hmenu = GETMENUHANDLE(wd->parent);

			if (hmenu != NULL) {
				RemoveMenu(hmenu, w->wcallback_id, MF_BYCOMMAND);
			}
			break;
	}


}


/**
 * Hv_W32AddCallback
 * @purpose Windows 9X/NT specific version of Hv_AddCallback.
 * @see The gateway routine Hv_AddCallback.
 */

void  Hv_W32AddCallback(Hv_Widget       w,
		     char           *cbname,
		     Hv_CallbackProc cbproc,
		     Hv_Pointer      data)
 {
	Hv_callbackData *newCallback;

	if(w==NULL)
		return;
	newCallback = (Hv_callbackData *) Hv_Malloc(sizeof(Hv_callbackData));
    newCallback->callback = cbproc;
    newCallback->clientPointer = data;
    newCallback->callbackType = cbname;

    if(strcmp(cbname,"Hv_inputCallback")==0)
       newCallback->icallbackType = Hv_INPUTCALLBACK;

    if(strcmp(cbname,"Hv_activateCallback")==0)
       newCallback->icallbackType = Hv_ACTIVATECALLBACK;

    if(strcmp(cbname,"Hv_valueChangedCallback")==0)
       newCallback->icallbackType = Hv_VALUECHANGEDCALLBACK;

    if(strcmp(cbname,"Hv_okCallback")==0)
       newCallback->icallbackType = Hv_OKCALLBACK;

    if(strcmp(cbname,"Hv_cancelCallback")==0)
       newCallback->icallbackType = Hv_CANCELCALLBACK;

    if(strcmp(cbname,"Hv_multipleSelectionCallback")==0)
       newCallback->icallbackType = Hv_MULTIPLESELECTIONCALLBACK;

    if(strcmp(cbname,"Hv_singleSelectionCallback")==0)
       newCallback->icallbackType = Hv_SINGLESELECTIONCALLBACK;

    if(strcmp(cbname,"Hv_thinMenuCallback")==0)
       newCallback->icallbackType = Hv_THINMENUCALLBACK;

/*    for when events a fully supported
   if(strcmp(cbname,"Hv_ButtonPressMask")==0)
       newCallback->icallbackType = Hv_BUTTONPRESSMASK;
*/
	if(w->wcallList == NULL)                  
        w->wcallList = Hv_DlistCreateList(); 
 
    Hv_DlistInsert(w->wcallList,newCallback,Hv_ENDLIST);
	w->wcallCount++;
}


/**
 * Hv_W32DestroyWidget
 * @purpose Windows 9X/NT specific version of Hv_DestroyWidget.
 * @see The gateway routine Hv_DestroyWidget.
 */



void  Hv_W32DestroyWidget(Hv_Widget w) {
	Hv_LinkList *lp;
    Hv_LinkList *temp;

	LPWSTR          wlabel;            /* UNICODE version of the title/label */

	if(w== NULL)
		return;

	if(w->wlabel != NULL)
		Hv_Free(w->wlabel);
	if(w->wtextString != NULL)
		Hv_Free(w->wtextString);

	if(w->wtype== Hv_LISTDIALOGITEM)
		if(w->wstyle & LBS_MULTIPLESEL)
		{
			if(w->wdata)
				Hv_Free(w->wdata);
		}

    if(w->wchildren)
	{
	  for (lp = w->wchildren->next; lp != w->wchildren; lp = lp->next)
	  {
		Hv_W32DestroyWidget((Hv_WidgetInfo *) lp->data);
		lp->data = NULL;
      }
     Hv_DlistFree(w->wchildren);
	}

    if(w->wdataList)
	{
	  for (lp = w->wdataList->next; lp != w->wdataList; lp = lp->next)
	  {
		Hv_Free(lp->data);
		lp->data = NULL;
      }
     Hv_DlistFree(w->wdataList);
	}
    if(w->wcallList)
	{
	  for (lp = w->wcallList->next; lp != w->wcallList; lp = lp->next)
	  {
		Hv_Free(lp->data);
		lp->data = NULL;
      }
     Hv_DlistFree(w->wcallList);
	}
    if(w->wgarbageList)
	{
	  for (lp = w->wgarbageList->next; lp != w->wgarbageList; lp = lp->next)
	  {
		Hv_Free(lp->data);
		lp->data = NULL;
      }
     Hv_DlistFree(w->wgarbageList);
	}
}


/**
 * Hv_W32SetSensitivity
 * @purpose Windows 9X/NT specific version of Hv_SetSensitivity.
 * @see The gateway routine Hv_SetSensitivity.
 */

void Hv_W32SetSensitivity(Hv_Widget  w,
			              Boolean   val) {

/* see if we can find the widget */

	Hv_CallbackData cd;


	if (w == NULL)
		return;

	cd = Hv_GetCallbackData(w->wcallback_id);

//	if (cd == NULL)
//		return;
// DAVID inpractice this w->wtype should be the same as cd->type. If not we need to talk

	switch (w->wtype) {
	case MENUITEM: case TOGGLEMENUITEM: case SUBMENUITEM:
		if(w == NULL)
			return;
		if (val)
			EnableMenuItem((HMENU)(cd->parent->wdata),
			     cd->win32Id, MF_BYCOMMAND | MF_ENABLED);

		else
			EnableMenuItem((HMENU)(cd->parent->wdata),
			     cd->win32Id, MF_BYCOMMAND | MF_GRAYED);
		break;
	case Hv_BUTTONDIALOGITEM:  
	case Hv_COLORSELECTORDIALOGITEM:  
	case Hv_LISTDIALOGITEM: 
	case Hv_SCALEDIALOGITEM: 
	case Hv_RADIODIALOGITEM: 
	case Hv_TOGGLEDIALOGITEM: 
	case Hv_TEXTDIALOGITEM:
	case Hv_SCROLLEDTEXTDIALOGITEM:  
	case Hv_LABELDIALOGITEM:  
	case Hv_FILESELECTORDIALOGITEM:  
	case Hv_ACTIONAREADIALOGITEM:
		 if (w->wisWisable)
			 if(val)
			 {
			    EnableWindow((HWND)(w->mscontrol), TRUE);
				w->wdisabled = !val;
			 }
			 else
			 {
			    EnableWindow((HWND)(w->mscontrol),  FALSE );
				w->wdisabled = !val;
			 }
	     else
		   w->wdisabled = !val;
		break;
	}
}


/**
 * Hv_W32IsSensitive
 * @purpose Windows 9X/NT specific version of Hv_IsSensitive.
 * @see The gateway routine Hv_IsSensitive.
 */

Boolean Hv_W32IsSensitive(Hv_Widget  w) {
	return w->wdisabled;
}


/**
 * Hv_W32SetWidgetBackgroundColor
 * @purpose Windows 9X/NT specific version of Hv_SetWidgetBackgroundColor.
 * @see The gateway routine Hv_SetWidgetBackgroundColor.
 */


void Hv_W32SetWidgetBackgroundColor(Hv_Widget   w,
				    short       color)
  {
	int i = 0;
    long error = 0;
    RECT Rect;
	if(w== NULL)
		return;
	w->wbgColor = color;
	if(w->wisWisable)
	{
		if(w->wbgColor != Hv_DEFAULT)
		{
	       i = GetClientRect(  w->mscontrol,&Rect);
		   error = GetLastError();
		   i = RedrawWindow(w->mscontrol, &Rect, NULL,  RDW_INVALIDATE    ); 
		   error = GetLastError();
		}
	}
}


/**
 * Hv_W32SetWidgetForegroundColor
 * @purpose Windows 9X/NT specific version of Hv_SetWidgetForegroundColor.
 * @see The gateway routine Hv_SetWidgetForegroundColor.
 */

void Hv_W32SetWidgetForegroundColor(Hv_Widget   w,
				    short       color) {
		if(w== NULL)
		return;

		w->wfgColor = color;
		if(w->wisWisable)
		{
		  if(w->wbgColor != Hv_DEFAULT)
		     RedrawWindow(w->mscontrol, NULL, NULL,  RDW_ERASE ); 
		}

}


/**
 * Hv_W32ClickType
 * @purpose Windows 9X/NT specific version of Hv_ClickType.
 * @see The gateway routine Hv_ClickType.
 */


int Hv_W32ClickType(Hv_Item Item,
		    short  button) {

	int			click = 0;
	Hv_XEvent   testevent;
	DWORD       tc;

	tc = GetTickCount() + 250;

	while (GetTickCount() < tc) {
		if (Hv_CheckForMessage(Hv_buttonPress, WM_LBUTTONDBLCLK, &testevent))
			return 2;
		if (Hv_CheckForMessage(Hv_buttonRelease, WM_LBUTTONUP, &testevent))
			click = 1;
		if (Hv_CheckForMessage(Hv_buttonPress, WM_LBUTTONDOWN, &testevent))
			return 2;
	}


	if ((Item != NULL) && (Item->domain == Hv_OUTSIDEHOTRECT))
		return 1;
	
	return click;

}


/**
 * Hv_W32Println
 * @purpose Windows 9X/NT specific version of Hv_Println.
 * @see The gateway routine Hv_Println.
 */

void    Hv_W32Println(char   *fmt,
			    va_list args) {

	char   line[1024];
    vsprintf(line, fmt, args);
    printf("%s", line); 
}



/*==========================================
 * COLOR RELATED SECTION OF GATEWAY
 *===========================================*/

/**
 * Hv_W32MakeColorMap
 * @purpose Windows 9X/NT specific version of Hv_MakeColorMap.
 * @see The gateway routine Hv_MakeColorMap.
 */


void Hv_W32MakeColorMap(void) {
}


/**
 * Hv_W32GetForeground
 * @purpose Windows 9X/NT specific version of Hv_GetForeground.
 * @see The gateway routine Hv_GetForeground.  
 */

unsigned long  Hv_W32GetForeground(void) {
	return 0;
}

/**
 * Hv_W32GetBackground
 * @purpose Windows 9X/NT specific version of Hv_GetBackground.
 * @see The gateway routine Hv_GetBackground.  
 */

unsigned long  Hv_W32GetBackground(void) {
	return 0;
}


/**
 * Hv_W32NearestColor
 * @purpose Windows 9X/NT specific version of Hv_NearestColor.
 * @see The gateway routine Hv_NearestColor.
 */

unsigned long   Hv_W32NearestColor(unsigned short r,
				   unsigned short g,
				   unsigned short b) {
	return 0;
}


/**
 * Hv_W32DarkenColor
 * @purpose Windows 9X/NT specific version of Hv_DarkenColor.
 * @see The gateway routine Hv_DarkenColor.
 */

void            Hv_W32DarkenColor(short color,
				  unsigned short del) {
}

/**
 * Hv_W32BrightenColor
 * @purpose Windows 9X/NT specific version of Hv_BrightenColor.
 * @see The gateway routine Hv_BrightenColor.
 */

void            Hv_W32BrightenColor(short color,
				    unsigned short del) {
}

/**
 * Hv_W32AdjustColor
 * @purpose Windows 9X/NT specific version of Hv_AdjustColor.
 * @see The gateway routine Hv_AdjustColor.
 */

void            Hv_W32AdjustColor(short color,
				  short r,
				  short g,
				  short b) {
}


/**
 * Hv_W32ChangeColor
 * @purpose Windows 9X/NT specific version of Hv_ChangeColor.
 * @see The gateway routine Hv_ChangeColor.
 */

void            Hv_W32ChangeColor(short color,
				  unsigned short r,
				  unsigned short g,
				  unsigned short b) {
}


/**
 * Hv_W32CopyColor
 * @purpose Windows 9X/NT specific version of Hv_CopyColor.
 * @see The gateway routine Hv_CopyColor.
 */

void  Hv_W32CopyColor(short dcolor,
		      short scolor) {

	RGBQUAD *drgb;
	RGBQUAD *srgb;

	if ((dcolor < 0) || (dcolor >= Hv_numColors))
		return;

	if ((scolor < 0) || (scolor >= Hv_numColors))
		return;

/*
 *  Step 1: Modify the color table
 */

	
	srgb = Hv_colorTable + scolor;
	drgb = Hv_colorTable + dcolor;

	drgb->rgbRed   = srgb->rgbRed;
	drgb->rgbGreen = srgb->rgbGreen;
	drgb->rgbBlue  = srgb->rgbBlue;

/*
 * Step 2: Change the palette
 */

	Hv_ColorTableToPalette();

}


/**
 * Hv_W32GetRGBFromColor
 * @purpose Windows 9X/NT specific version of Hv_GetRGBFromColor.
 * @see The gateway routine Hv_GetRGBFromColor.
 */

void Hv_W32GetRGBFromColor(short color,
					   	   unsigned short *r,
						   unsigned short *g,
						   unsigned short *b) {
}


/**
 * Hv_W32QueryColor
 * @purpose Windows 9X/NT specific version of Hv_QueryColor.
 * @see The gateway routine Hv_QueryColor.
 */

void  Hv_W32QueryColor(Hv_Display   *dpy,
		       Hv_ColorMap   cmap,
		       Hv_Color     *xcolor) {
}


/*==========================================
 * FILESELECTION RELATED SECTION OF GATEWAY
 *===========================================*/

//
//   FUNCTION: TestNotify( HWND hDlg, LPOFNOTIFY pofn)
//
//  PURPOSE:  Processes the WM_NOTIFY message notifications that is sent
//    to the hook dialog procedure for the File Open common dialog.
//
//

const char szmsgSHAREVIOLATION[] = "Share Violation"; //SHAREVISTRING;  // string for sharing violation
const char szmsgFILEOK[]         = "File is OK"; //FILEOKSTRING;   // string for OK button
const char szCommdlgHelp[]       = "Need Some Help?";//HELPMSGSTRING;  // string for Help button

UINT cdmsgShareViolation = 0;  // identifier from RegisterWindowMessage
UINT cdmsgFileOK         = 0;  // identifier from RegisterWindowMessage
UINT cdmsgHelp           = 0;  // identifier from RegisterWindowMessage

typedef struct _MYDATA
{
	char szTest1[80];		// a test buffer containing the file selected
	char szTest2[80];       // a test buffer containing the file path
} MYDATA, FAR * LPMYDATA;

	
MYDATA sMyData;			// an instance of a MYDATA


BOOL NEAR PASCAL TestNotify(HWND hDlg, LPOFNOTIFY pofn)
{
	switch (pofn->hdr.code)
	{
		// The selection has changed. 
		case CDN_SELCHANGE:
		{
			char szFile[MAX_PATH];

			// Get the file specification from the common dialog.
			if (CommDlg_OpenSave_GetSpec(GetParent(hDlg),
				szFile, sizeof(szFile)) <= sizeof(szFile))
			{
				// Set the dialog item to reflect this.
			//	SetDlgItemText(hDlg, IDE_SELECTED, szFile);
			}

			// Get the path of the selected file.
			if (CommDlg_OpenSave_GetFilePath(GetParent(hDlg),
				szFile, sizeof(szFile)) <= sizeof(szFile))
			{
				// Display this path in the appropriate box.
			//	SetDlgItemText(hDlg, IDE_PATH, szFile);
			}
		}
		break;

		// A new folder has been opened.
		case CDN_FOLDERCHANGE:
		{
			char szFile[MAX_PATH];

			if (CommDlg_OpenSave_GetFolderPath(GetParent(hDlg),
				szFile, sizeof(szFile)) <= sizeof(szFile))
			{
				// Display this new path in the appropriate box.
			//	SetDlgItemText(hDlg, IDE_SELECTED, szFile);
			}
		}
		break;

		// The "Help" pushbutton has been pressed.
		case CDN_HELP:
			MessageBox(hDlg, "Got the Help button notify.", "ComDlg32 Test", MB_OK);
			break;

		// The 'OK' pushbutton has been pressed.
		case CDN_FILEOK:
			// Update the appropriate box.
//			SetDlgItemText(hDlg,IDE_SELECTED, pofn->lpOFN->lpstrFile);
			SetWindowLong(hDlg, DWL_MSGRESULT, 1L);
			break;

		// Received a sharing violation.
		case CDN_SHAREVIOLATION:
			// Update the appropriate box.
//			SetDlgItemText(hDlg, IDE_SELECTED, pofn->pszFile);
			MessageBox(hDlg, "Got a sharing violation notify.", "ComDlg32 Test", MB_OK);
			break;
	}

	return(TRUE);
}

//
//   FUNCTION: ComDlg32DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
//
//  PURPOSE:  Processes messages for the File Open common dialog box.
//
//    MESSAGES:
//
//	WM_INITDIALOG - save pointer to the OPENFILENAME structure in User data
//	WM_DESTROY - get the text entered and fill in the MyData structure
//	WM_NOTIFY - pass this message onto the TestNotify function
//	default - check for a sharing violation or the OK button and
//    	display a message box.
//

static BOOL CALLBACK ComDlg32DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)
	{
		case WM_INITDIALOG:
			// Save off the long pointer to the OPENFILENAME structure.
			SetWindowLong(hDlg, DWL_USER, lParam);
			break;

		case WM_DESTROY:
			{
			LPOPENFILENAME lpOFN = (LPOPENFILENAME)GetWindowLong(hDlg, DWL_USER);
			LPMYDATA psMyData = (LPMYDATA)lpOFN->lCustData;

//			GetDlgItemText(hDlg, IDE_PATH, psMyData->szTest1, sizeof(psMyData->szTest1));
//			GetDlgItemText(hDlg, IDE_SELECTED, psMyData->szTest2, sizeof(psMyData->szTest2));
			}
			break;

		case WM_NOTIFY:
			TestNotify(hDlg, (LPOFNOTIFY)lParam);

		default:
			if (uMsg == cdmsgFileOK)
			{
//				SetDlgItemText(hDlg, IDE_SELECTED, ((LPOPENFILENAME)lParam)->lpstrFile);
				if (MessageBox(hDlg, "Got the OK button message.\n\nShould I open it?", "ComDlg32 Test", MB_YESNO)
					== IDNO)
				{
					SetWindowLong(hDlg, DWL_MSGRESULT, 1L);
				}
				break;
			}
			else if (uMsg == cdmsgShareViolation)
			{
//				SetDlgItemText(hDlg, IDE_SELECTED, (LPSTR)lParam);
				MessageBox(hDlg, "Got a sharing violation message.", "ComDlg32 Test", MB_OK);
				break;
			}
			return FALSE;
	}
	return TRUE;
}

/**if
 * Hv_W32DoubleFileSelect
 * @purpose Windows 9X/NT specific version of Hv_DoubleFileSelect.
 * @see The gateway routine Hv_DoubleFileSelect.
 */

void Hv_W32DoubleFileSelect(char *title,
			    char *prompt1 ,
			    char *prompt2 ,
			    char *mask1 , 
			    char *mask2 , 
			    char **fname1,
				char **fname2) {
	     Hv_Widget           dialog;


     Hv_Widget        dummy, rowcol, rc, rc1, rc2;
     int              reason;
     Hv_Widget        sb1;
     Hv_Widget        sb2;
     Hv_Widget           text1, text2;
     char             *temptext1, *temptext2;
     Hv_CompoundString         xmst;

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

//    text1 = XmFileSelectionBoxGetChild(sb1, XmDIALOG_TEXT);    
//    text2 = XmFileSelectionBoxGetChild(sb2, XmDIALOG_TEXT);    

/* add the action buttons -- note use of StandardActionArea*/

    Hv_StandardActionArea(rowcol, Hv_OKBUTTON + Hv_CANCELBUTTON);


/* cached first fs dir */

    if (Hv_cachedDirectory1 != NULL) {
	xmst =  Hv_CreateSimpleCompoundString(Hv_cachedDirectory1);
	Hv_VaSetValues(sb1, 
		      Hv_Ndirectory, xmst, 
		      NULL);
	Hv_CompoundStringFree(xmst);
    }

/* cached 2nd fs dir */

    if (Hv_cachedDirectory2 != NULL) {
	xmst =  Hv_CreateSimpleCompoundString(Hv_cachedDirectory2);
	Hv_VaSetValues(sb2, 
		      Hv_Ndirectory, xmst, 
		      NULL);
	Hv_CompoundStringFree(xmst);
    }

/* now process the dialog */

    reason  = Hv_DoDialog(dialog, NULL);

/* if hit OK, get the file names */

    if (reason == Hv_OK) {
	temptext1 = Hv_GetString(text1);
	temptext2 = Hv_GetString(text2);

	Hv_InitCharStr(fname1, temptext1);
	Hv_InitCharStr(fname2, temptext2);

	Hv_Free(temptext1);
	Hv_Free(temptext2);

    }
    else {
	*fname1 = NULL;
	*fname2 = NULL;
    }


    Hv_Free(Hv_cachedDirectory1);
    Hv_VaGetValues(sb1, Hv_Ndirectory, &xmst, NULL);
  //  Hv_cachedDirectory1 = Hv_TextFromXmString(xmst);
    Hv_CompoundStringFree(xmst);

    Hv_Free(Hv_cachedDirectory2);
    Hv_VaGetValues(sb2, Hv_Ndirectory, &xmst, NULL);
  //  Hv_cachedDirectory2 = Hv_TextFromXmString(xmst);
    Hv_CompoundStringFree(xmst);

    Hv_DestroyWidget(dialog);

}

/**
 * Hv_W32FileSelect
 * @purpose Windows 9X/NT specific version of Hv_FileSelect.
 * @see The gateway routine Hv_FileSelect.
 */

char *Hv_W32FileSelect(char *prompt,
		       char *mask,
		       char *deftext) {
	OPENFILENAME OpenFileName;
	static TCHAR         szFile[MAX_PATH]      = "\0";
	char *fname = NULL;
    static char filename[MAXPATHLEN];
    char filter[MAXPATHLEN];
	char *pfilter;
    static int initialize = 0;
	pfilter = filter;

	// as a default ad the users mask
	strcpy(pfilter,mask);
	pfilter += strlen(pfilter) +1;
	strcpy(pfilter,mask);
	pfilter += strlen(pfilter)+1;
    
	// for convience also add *.* to the list of filters
	strcpy(pfilter,"*.*");
	pfilter += strlen(pfilter) +1;
	strcpy(pfilter,"*.*");
	pfilter += strlen(pfilter)+1;

	*pfilter = '\0';
    strcpy( szFile, "");


	// Fill in the OPENFILENAME structure to support a template and hook.
	OpenFileName.lStructSize       = sizeof(OPENFILENAME);
    OpenFileName.hwndOwner         = Hv_mainWindow;
    OpenFileName.hInstance         = Hv_appContext;
    OpenFileName.lpstrFilter       = filter;
    OpenFileName.lpstrCustomFilter = NULL;
    OpenFileName.nMaxCustFilter    = 0;
    OpenFileName.nFilterIndex      = 0;
    OpenFileName.lpstrFile         = szFile;
    OpenFileName.nMaxFile          = sizeof(szFile);
    OpenFileName.lpstrFileTitle    = NULL;
    OpenFileName.nMaxFileTitle     = 0;
    OpenFileName.lpstrInitialDir   = deftext;
    OpenFileName.lpstrTitle        = prompt;
    OpenFileName.nFileOffset       = 0;
    OpenFileName.nFileExtension    = 0;
    OpenFileName.lpstrDefExt       = NULL;
    OpenFileName.lCustData         = 0;/*(LPARAM)&sMyData;*/
	OpenFileName.lpfnHook 		   = NULL; /*ComDlg32DlgProc; */
	OpenFileName.lpTemplateName    = 0; /*MAKEINTRESOURCE(IDD_COMDLG32);*/
    OpenFileName.Flags             = OFN_SHOWHELP | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_CREATEPROMPT;
									/*| OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;*/

	/* Call the common dialog function. */
    if (GetOpenFileName(&OpenFileName))
    {
		int szlen = strlen(szFile);
		char *pszFile = szFile + szlen;
	/*	strcpy(filename,(LPCTSTR)OpenFileName.lpstrFilep); */
		/* remove the leading directory  ********* NOT!!!!!!!! */
  /*           while(szlen--)
			 {
				 pszFile--;
				 if(*pszFile == '\\')
				 {
					 pszFile++;
					 break;
				 }
			 } 
			Hv_InitCharStr(&fname, pszFile); */
		Hv_InitCharStr(&fname, szFile);

		return fname;
    }
    else
   	{
	//	ProcessCDError(CommDlgExtendedError(), Hv_mainWindow);
		return NULL;
    }
	
}
/**
 * Hv_W32FileSelect
 * @purpose Windows 9X/NT specific version of Hv_FileSelect.
 * @see The gateway routine Hv_FileSelect.
 */

char *Hv_W32FileSave(char *prompt,
		       char *mask,
		       char *deftext) {
	OPENFILENAME OpenFileName;
	static TCHAR         szFile[MAX_PATH]      = "\0";
	char *fname = NULL;
    static char filename[MAXPATHLEN];
    char filter[MAXPATHLEN];
	char *pfilter;
    static int initialize = 0;
	pfilter = filter;

	// as a default ad the users mask
	strcpy(pfilter,mask);
	pfilter += strlen(pfilter) +1;
	strcpy(pfilter,mask);
	pfilter += strlen(pfilter)+1;
    
	// for convience also add *.* to the list of filters
	strcpy(pfilter,"*.*");
	pfilter += strlen(pfilter) +1;
	strcpy(pfilter,"*.*");
	pfilter += strlen(pfilter)+1;

	*pfilter = '\0';
    strcpy( szFile, "");


	// Fill in the OPENFILENAME structure to support a template and hook.
	OpenFileName.lStructSize       = sizeof(OPENFILENAME);
    OpenFileName.hwndOwner         = Hv_mainWindow;
    OpenFileName.hInstance         = Hv_appContext;
    OpenFileName.lpstrFilter       = filter;
    OpenFileName.lpstrCustomFilter = NULL;
    OpenFileName.nMaxCustFilter    = 0;
    OpenFileName.nFilterIndex      = 0;
    OpenFileName.lpstrFile         = szFile;
    OpenFileName.nMaxFile          = sizeof(szFile);
    OpenFileName.lpstrFileTitle    = NULL;
    OpenFileName.nMaxFileTitle     = 0;
    OpenFileName.lpstrInitialDir   = deftext;
    OpenFileName.lpstrTitle        = prompt;
    OpenFileName.nFileOffset       = 0;
    OpenFileName.nFileExtension    = 0;
    OpenFileName.lpstrDefExt       = NULL;
    OpenFileName.lCustData         = 0;/*(LPARAM)&sMyData;*/
	OpenFileName.lpfnHook 		   = NULL; /*ComDlg32DlgProc; */
	OpenFileName.lpTemplateName    = 0; /*MAKEINTRESOURCE(IDD_COMDLG32);*/
    OpenFileName.Flags             = OFN_SHOWHELP | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_CREATEPROMPT;
									/*| OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;*/

	/* Call the common dialog function. */
    if (GetSaveFileName(&OpenFileName))
    {
		int szlen = strlen(szFile);
		char *pszFile = szFile + szlen;
	/*	strcpy(filename,(LPCTSTR)OpenFileName.lpstrFilep); */
		/* remove the leading directory  ********* NOT!!!!!!!! */
  /*           while(szlen--)
			 {
				 pszFile--;
				 if(*pszFile == '\\')
				 {
					 pszFile++;
					 break;
				 }
			 } 
			Hv_InitCharStr(&fname, pszFile); */
		Hv_InitCharStr(&fname, szFile);

		return fname;
    }
    else
   	{
	//	ProcessCDError(CommDlgExtendedError(), Hv_mainWindow);
		return NULL;
    }
	
}


char *Hv_W32ProcessFileSelect(void *phandle)

 {
	OPENFILENAME OpenFileName;
	static TCHAR         szFile[MAX_PATH]      = "\0";
	char *fname = NULL;
    static char filename[MAXPATHLEN];
    char filter[MAXPATHLEN];
	char *pfilter;
    static int initialize = 0;
	pfilter = filter;

//	strcpy(pfilter,mask);
	pfilter += strlen(pfilter) +1;
//	strcpy(pfilter,mask);
	pfilter += strlen(pfilter)+1;
	*pfilter = '\0';
    strcpy( szFile, "");


	// Fill in the OPENFILENAME structure to support a template and hook.
	OpenFileName.lStructSize       = sizeof(OPENFILENAME);
    OpenFileName.hwndOwner         = Hv_mainWindow;
    OpenFileName.hInstance         = phandle;
    OpenFileName.lpstrFilter       = filter;
    OpenFileName.lpstrCustomFilter = NULL;
    OpenFileName.nMaxCustFilter    = 0;
    OpenFileName.nFilterIndex      = 0;
    OpenFileName.lpstrFile         = szFile;
    OpenFileName.nMaxFile          = sizeof(szFile);
    OpenFileName.lpstrFileTitle    = NULL;
    OpenFileName.nMaxFileTitle     = 0;
    OpenFileName.lpstrInitialDir   = NULL;
    OpenFileName.lpstrTitle        = "Hi";//prompt;
    OpenFileName.nFileOffset       = 0;
    OpenFileName.nFileExtension    = 0;
    OpenFileName.lpstrDefExt       = NULL;
    OpenFileName.lCustData         = 0;/*(LPARAM)&sMyData;*/
	OpenFileName.lpfnHook 		   = ComDlg32DlgProc; 
	OpenFileName.lpTemplateName    = 0; /*MAKEINTRESOURCE(IDD_COMDLG32);*/
    OpenFileName.Flags             = OFN_SHOWHELP | OFN_EXPLORER |
									 OFN_ENABLEHOOK | OFN_ENABLETEMPLATEHANDLE ;

	/* Call the common dialog function. */
    if (GetOpenFileName(&OpenFileName))
    {
	/*	strcpy(filename,(LPCTSTR)OpenFileName.lpstrFilep); */
			Hv_InitCharStr(&fname, szFile);

		return fname;
    }
    else
   	{
	//	ProcessCDError(CommDlgExtendedError(), Hv_mainWindow);
		return NULL;
    }
	
}



//  FUNCTION: ProcessCDError(DWORD) 
//
//  PURPOSE: Processes errors from the common dialog functions.
//
//  COMMENTS:
//
//        This function is called whenever a common dialog function
//        fails.  The CommonDialogExtendedError() value is passed to
//        the function which maps the error value to a string table.
//        The string is loaded and displayed for the user. 
//

#ifdef HV_RESOURCE
void ProcessCDError(DWORD dwErrorCode, HWND hWnd)
{
   WORD  wStringID;
   TCHAR  buf[MAX_PATH];

   switch(dwErrorCode)
      {
	 case CDERR_DIALOGFAILURE:   wStringID=IDS_DIALOGFAILURE;   break;
	 case CDERR_STRUCTSIZE:      wStringID=IDS_STRUCTSIZE;      break;
	 case CDERR_INITIALIZATION:  wStringID=IDS_INITIALIZATION;  break;
	 case CDERR_NOTEMPLATE:      wStringID=IDS_NOTEMPLATE;      break;
	 case CDERR_NOHINSTANCE:     wStringID=IDS_NOHINSTANCE;     break;
	 case CDERR_LOADSTRFAILURE:  wStringID=IDS_LOADSTRFAILURE;  break;
	 case CDERR_FINDRESFAILURE:  wStringID=IDS_FINDRESFAILURE;  break;
	 case CDERR_LOADRESFAILURE:  wStringID=IDS_LOADRESFAILURE;  break;
	 case CDERR_LOCKRESFAILURE:  wStringID=IDS_LOCKRESFAILURE;  break;
	 case CDERR_MEMALLOCFAILURE: wStringID=IDS_MEMALLOCFAILURE; break;
	 case CDERR_MEMLOCKFAILURE:  wStringID=IDS_MEMLOCKFAILURE;  break;
	 case CDERR_NOHOOK:          wStringID=IDS_NOHOOK;          break;
	 case PDERR_SETUPFAILURE:    wStringID=IDS_SETUPFAILURE;    break;
	 case PDERR_PARSEFAILURE:    wStringID=IDS_PARSEFAILURE;    break;
	 case PDERR_RETDEFFAILURE:   wStringID=IDS_RETDEFFAILURE;   break;
	 case PDERR_LOADDRVFAILURE:  wStringID=IDS_LOADDRVFAILURE;  break;
	 case PDERR_GETDEVMODEFAIL:  wStringID=IDS_GETDEVMODEFAIL;  break;
	 case PDERR_INITFAILURE:     wStringID=IDS_INITFAILURE;     break;
	 case PDERR_NODEVICES:       wStringID=IDS_NODEVICES;       break;
	 case PDERR_NODEFAULTPRN:    wStringID=IDS_NODEFAULTPRN;    break;
	 case PDERR_DNDMMISMATCH:    wStringID=IDS_DNDMMISMATCH;    break;
	 case PDERR_CREATEICFAILURE: wStringID=IDS_CREATEICFAILURE; break;
	 case PDERR_PRINTERNOTFOUND: wStringID=IDS_PRINTERNOTFOUND; break;
	 case CFERR_NOFONTS:         wStringID=IDS_NOFONTS;         break;
	 case FNERR_SUBCLASSFAILURE: wStringID=IDS_SUBCLASSFAILURE; break;
	 case FNERR_INVALIDFILENAME: wStringID=IDS_INVALIDFILENAME; break;
	 case FNERR_BUFFERTOOSMALL:  wStringID=IDS_BUFFERTOOSMALL;  break;

	 case 0:   //User may have hit CANCEL or we got a *very* random error
	    return;

	 default:
	    wStringID=IDS_UNKNOWNERROR;
      }

   LoadString(NULL, wStringID, buf, sizeof(buf));
   MessageBox(hWnd, buf, NULL, MB_OK);
   return;
}
#endif
/**
 * Hv_W32DirectorySelect
 * @purpose Windows 9X/NT specific version of Hv_DirectorySelect.
 * @see The gateway routine Hv_DirectorySelect.
 */

char *Hv_W32DirectorySelect(char *prompt,
			    char *mask) {
	return NULL;
}

/**
 * Hv_W32GenericFileSelect
 * @purpose Windows 9X/NT specific version of Hv_GenericFileSelect.
 * @see The gateway routine Hv_GenericFileSelect.
 */

char *Hv_W32GenericFileSelect(char          *prompt,
			      char          *mask,
			      unsigned char  filemask,
			      char          *deftext,
			      Hv_Widget     parent) {

	/** Needs to be updated for the filemask */

	return Hv_W32FileSelect(prompt,
		       mask,
		       deftext) ;


}

/**
 * Hv_W32GenericFileSelect
 * @purpose Windows 9X/NT specific version of Hv_GenericFileSelect.
 * @see The gateway routine Hv_GenericFileSelect.
 */

char *Hv_W32GenericFileSave(char          *prompt,
			      char          *mask,
			      unsigned char  filemask,
			      char          *deftext,
			      Hv_Widget     parent) {

	/** Needs to be updated for the filemask */

	return Hv_W32FileSave(prompt,
		       mask,
		       deftext) ;


}

/*==========================================
 * FONT RELATED SECTION OF GATEWAY
 *===========================================*/

/**
 * Hv_W32InitFonts
 * @purpose Windows 9X/NT specific version of Hv_InitFonts.
 * @see The gateway routine Hv_InitFonts.
 */

void Hv_W32InitFonts(void) {

	int i = 0;



/*	int fixed     = FIXED_PITCH | FF_MODERN;
	int courier   = FIXED_PITCH | FF_MODERN;
	int helvetica = VARIABLE_PITCH | FF_MODERN;
	int times     = VARIABLE_PITCH | FF_SWISS;*/
	int fixed     = DEFAULT_PITCH | FF_MODERN;
	int courier   = FIXED_PITCH | FF_MODERN;
	int helvetica = VARIABLE_PITCH | FF_ROMAN;
	int times     = VARIABLE_PITCH | FF_ROMAN;
	int symbol    = FF_DONTCARE;

	int bold = FW_BOLD;

	int s1 = 11;
	int s2 = 12;
	int s3 = 14;
	int s4 = 17;
	int s5 = 20;
	int s6 = 25;

/*	char *n1 = ""; // use for fixed
	char *n2 = "Courier New"; // use for courier
	char *n3 = "MS Serif"; //use for helvetica
	char *n4 = "MS Sans Serif"; //use for times
	char *n5 = ""; //use for symbol */

	char *n1 = "Bookman Old Style"; // use for fixed
	char *n2 = "Courier New"; // use for courier
	char *n3 = "Arial Narrow"; //use for helvetica
	char *n4 = "Times New Roman"; //use for times
	char *n5 = "Symbol"; //use for symbol
	char *n6 = "System"; //use for dialog boxes

	if (Hv_fonts != NULL)
		return;

//create a structure that holds handles to all the fonts

	Hv_fonts = (HFONT *)Hv_Malloc(Hv_NUMFONTS*sizeof(HFONT));
    Hv_fonts[i] = Hv_NewFont(s1, 0,fixed, n1);	Hv_fixed11 = i++;
    Hv_fonts[i] = Hv_NewFont(s2, 0,fixed, n1);	Hv_fixed12 = i++; 
    Hv_fonts[i] = Hv_NewFont(s3, 0,fixed, n1);	Hv_fixed14 = i++; 
    Hv_fonts[i] = Hv_NewFont(s4, 0,fixed, n1);	Hv_fixed17 = i++; 
    Hv_fonts[i] = Hv_NewFont(s5, 0,fixed, n1);	Hv_fixed20 = i++; 
    Hv_fonts[i] = Hv_NewFont(s6, 0,fixed, n1);	Hv_fixed25 = i++; 

    Hv_fonts[i] = Hv_NewFont(s1, 0,courier, n2);	Hv_courier11 = i++; 
    Hv_fonts[i] = Hv_NewFont(s2, 0,courier, n2);	Hv_courier12 = i++; 
    Hv_fonts[i] = Hv_NewFont(s3, 0,courier, n2);	Hv_courier14 = i++; 
    Hv_fonts[i] = Hv_NewFont(s4, 0,courier, n2);	Hv_courier17 = i++; 
    Hv_fonts[i] = Hv_NewFont(s5, 0,courier, n2);	Hv_courier20 = i++; 
    Hv_fonts[i] = Hv_NewFont(s6, 0,courier, n2);	Hv_courier25 = i++;
	
    Hv_fonts[i] = Hv_NewFont(s1+1, 0,helvetica, n3);	Hv_helvetica11 = i++; 
    Hv_fonts[i] = Hv_NewFont(s2+2, 0,helvetica, n3);	Hv_helvetica12 = i++; 
    Hv_fonts[i] = Hv_NewFont(s3+2, 0,helvetica, n3);	Hv_helvetica14 = i++; 
    Hv_fonts[i] = Hv_NewFont(s4+2, 0,helvetica, n3);	Hv_helvetica17 = i++;  
    Hv_fonts[i] = Hv_NewFont(s5+2, 0,helvetica, n3);	Hv_helvetica20 = i++;  
    Hv_fonts[i] = Hv_NewFont(s6+2, 0,helvetica, n3);	Hv_helvetica25 = i++;  

    Hv_fonts[i] = Hv_NewFont(s1, 0,times, n4);	Hv_times11 = i++; 
    Hv_fonts[i] = Hv_NewFont(s2, 0,times, n4);	Hv_times12 = i++; 
    Hv_fonts[i] = Hv_NewFont(s3, 0,times, n4);	Hv_times14 = i++; 
    Hv_fonts[i] = Hv_NewFont(s4, 0,times, n4);	Hv_times17 = i++;  
    Hv_fonts[i] = Hv_NewFont(s5, 0,times, n4);	Hv_times20 = i++;  
    Hv_fonts[i] = Hv_NewFont(s6, 0,times, n4);	Hv_times25 = i++; 
	
    Hv_fonts[i] = Hv_NewFont(s1, 0,symbol, n5);	Hv_symbol11 = i++; 
    Hv_fonts[i] = Hv_NewFont(s2, 0,symbol, n5);	Hv_symbol12 = i++; 
    Hv_fonts[i] = Hv_NewFont(s3, 0,symbol, n5);	Hv_symbol14 = i++; 
    Hv_fonts[i] = Hv_NewFont(s4, 0,symbol, n5);	Hv_symbol17 = i++;  
    Hv_fonts[i] = Hv_NewFont(s5, 0,symbol, n5);	Hv_symbol20 = i++;  
    Hv_fonts[i] = Hv_NewFont(s6, 0,symbol, n5);	Hv_symbol25 = i++;  

// now the bold fonts

    Hv_fonts[i] = Hv_NewFont(s1, bold, fixed, n1);	Hv_fixedBold11 = i++; //30
    Hv_fonts[i] = Hv_NewFont(s2, bold, fixed, n1);	Hv_fixedBold12 = i++; 
    Hv_fonts[i] = Hv_NewFont(s3, bold, fixed, n1);	Hv_fixedBold14 = i++; 
    Hv_fonts[i] = Hv_NewFont(s4, bold, fixed, n1);	Hv_fixedBold17 = i++;  
    Hv_fonts[i] = Hv_NewFont(s5, bold, fixed, n1);	Hv_fixedBold20 = i++;  
    Hv_fonts[i] = Hv_NewFont(s6, bold, fixed, n1);	Hv_fixedBold25 = i++;  

    Hv_fonts[i] = Hv_NewFont(s1, bold, courier, n2);	Hv_courierBold11 = i++; 
    Hv_fonts[i] = Hv_NewFont(s2, bold, courier, n2);	Hv_courierBold12 = i++; 
    Hv_fonts[i] = Hv_NewFont(s3, bold, courier, n2);	Hv_courierBold14 = i++; 
    Hv_fonts[i] = Hv_NewFont(s4, bold, courier, n2);	Hv_courierBold17 = i++;  
    Hv_fonts[i] = Hv_NewFont(s5, bold, courier, n2);	Hv_courierBold20 = i++;  //40
    Hv_fonts[i] = Hv_NewFont(s6, bold, courier, n2);	Hv_courierBold25 = i++; 
	
    Hv_fonts[i] = Hv_NewFont(s1+1, bold, helvetica, n3); Hv_helveticaBold11 = i++;
    Hv_fonts[i] = Hv_NewFont(s2+2, bold, helvetica, n3);	Hv_helveticaBold12 = i++; 
    Hv_fonts[i] = Hv_NewFont(s3+2, bold, helvetica, n3);	Hv_helveticaBold14 = i++; 
    Hv_fonts[i] = Hv_NewFont(s4+2, bold, helvetica, n3);	Hv_helveticaBold17 = i++; 
    Hv_fonts[i] = Hv_NewFont(s5+2, bold, helvetica, n3);	Hv_helveticaBold20 = i++; 
    Hv_fonts[i] = Hv_NewFont(s6+2, bold, helvetica, n3);	Hv_helveticaBold25 = i++; 

    Hv_fonts[i] = Hv_NewFont(s1, bold, times, n4);    Hv_timesBold11 = i++;
    Hv_fonts[i] = Hv_NewFont(s2, bold, times, n4);	Hv_timesBold12 = i++; 
    Hv_fonts[i] = Hv_NewFont(s3, bold, times, n4);	Hv_timesBold14 = i++; //50
    Hv_fonts[i] = Hv_NewFont(s4, bold, times, n4);	Hv_timesBold17 = i++; 
    Hv_fonts[i] = Hv_NewFont(s5, bold, times, n4);	Hv_timesBold20 = i++; 
    Hv_fonts[i] = Hv_NewFont(s6, bold, times, n4);	Hv_timesBold25 = i++;


	
	Hv_fixed  = Hv_fixed11;
	Hv_fixed2 = Hv_fixedBold12;
}

/**
 * Hv_W32AddNewFont
 * @purpose Windows 9X/NT specific version of Hv_AddNewFont.
 * @see The gateway routine Hv_AddNewFont.
 */

void Hv_W32AddNewFont(short font) {
}

static BOOL CALLBACK
printfDlgProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND stdoutWin = NULL;
	HWND strerrWin = NULL;

	int nWidth = 0;
	int nHeight = 0;

    switch (message) {

        case WM_COMMAND:               // Notification from a control
            if(wParam == Hv_PRINTFOK) 
			{
            DestroyWindow(Hv_PrintfWindow);
            Hv_PrintfWindow = NULL;
			}
			else
	      return  HANDLE_WM_COMMAND (hwnd, wParam, lParam, aboutDlg_OnCommand) ;
		//	  return  HANDLE_WM_COMMAND (hwnd, wParam, lParam, mainFrame_OnCommand) ;
		case WM_SIZE:
			stdoutWin = GetDlgItem(  Hv_PrintfWindow,Hv_STDOUTWIN);      
			strerrWin = GetDlgItem(  Hv_PrintfWindow,Hv_STDERRWIN);   
			
			nWidth = LOWORD(lParam);  // width of client area 
			nHeight = HIWORD(lParam); // height of client area 
  //          printf("new width %d   new height %d",nWidth,nHeight);
			MoveWindow(stdoutWin, 14, 40,  nWidth-10,245, TRUE); 
			MoveWindow(strerrWin, 14, 312, nWidth-10,245, TRUE);
			InvalidateRect(strerrWin, NULL, TRUE); 

			return 0;
    }

    return FALSE ;
}

//
//  void aboutDlg_OnCommand (HWND hwnd, int id, hwnd hwndCtl, UINT codeNotify)
//
//  hwnd            Window handle for the dialog box window
//  id              Specifies the identifier of the menu item, control, or 
//                  accelerator.
//  hwndCtl         Handle of the control sending the message if the message
//                  is from a control, otherwise, this parameter is NULL. 
//  codeNotify      Specifies the notification code if the message is from a 
//                  control. This parameter is 1 when the message is from an
//                  accelerator. This parameter is 0 when the message is from
//                  a menu.
//
//  PURPOSE:
//                  Handle the keyboard and control notifications.
//                  An OK button press, or Enter/Esc keypress
//                  all dismiss the About dialog box.
//                  
//
//  COMMENTS:
//

static
void aboutDlg_OnCommand (HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id) {
   //     case IDOK:                          // OK pushbutton/Enter keypress
   //     case IDCANCEL:                      // Esc keypress
    //        EndDialog (hwnd, TRUE) ;        // Dismiss the about dialog box
   //         break ;

        default:
            FORWARD_WM_COMMAND (hwnd, id, hwndCtl, codeNotify, DefWindowProc) ;
        }
}


/*==============================================
 * HELP RELATED SECTION OF GATEWAY
 *==============================================*/
/* ---- Hv_ReadHelpInfo --- */
static FILE         *helpfp = NULL; 
static Hv_Widget     helpText;


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


static void Hv_ChooseHelpTopic(Hv_Widget w,
			       Hv_Pointer client_data
			       /* ,XmListCallbackStruct *cbs*/) {

/* callback for selection of a help topic */
 
  char *choice;
  char *text;
  int index = 0;
  int len;
  int pos;

  char *pa,*pb;


  index = SendDlgItemMessage( w->msdialog, w->wcallback_id, LB_GETCURSEL,(WPARAM) 0,0);
  len = SendDlgItemMessage(w->msdialog, w->wcallback_id, LB_GETTEXTLEN, (WPARAM) index, (LPARAM)0); 
  choice = (char *)Hv_Malloc(len+1);
  SendDlgItemMessage(w->msdialog, w->wcallback_id, LB_GETTEXT, (WPARAM) index, (LPARAM)choice); 
 /* XmStringGetLtoR(cbs->item, XmSTRING_DEFAULT_CHARSET, &choice);*/
  Hv_ReadHelp(choice, &text);   /*mallocs space for text */

  Hv_W32ClearScrolledText(helpText,&pos);  // clear out the previous garbage

  pa = pb = text;
  while(*pb != NULL)
  {
	  if(*pb == '\n')
	  {
	   *pb = '\0';
       Hv_AppendLineToScrolledText(helpText, pa);
	   pa = pb+1;
	  }
	  pb++;
  }
  
  Hv_Free(choice); 
  Hv_Free(text);
}




/**
 * Hv_W32DoHelp
 * @purpose Windows 9X/NT specific version of Hv_DoHelp.
 * @see The gateway routine Hv_DoHelp. 
 */


void  Hv_W32DoHelp(void) {

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
	  if (strnicmp(topics[i], topics[j], 15) > 0) {
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
      
	i = 0;
    for (i = 0; i < numtopics; i++)   {
	  fprintf(stderr, "Adding [%s]\n", topics[i]);
	  Hv_AddListItem(topicsList, topics[i],     False);
	  i++;
    }

    for (i = 0; i < numtopics; i++)   
		  Hv_Free(topics[i]);
    }


/* dialog has been created */

    Hv_DoDialog(dialog, NULL);

  }
  
}

#undef  N_COLS1
#undef  N_ROWS1

/**
 * Hv_W32TextInsert
 * @purpose Windows 9X/NT specific version of Hv_TextInsert.
 * @see The gateway routine Hv_TextInsert. 
 */


void  Hv_W32TextInsert(Hv_Widget       w,
		    Hv_TextPosition pos,
		    char           *line) {
}

/**
 * Hv_W32SetInsertionPosition
 * @purpose Windows 9X/NT specific version of Hv_SetInsertionPosition.
 * @see The gateway routine Hv_SetInsertionPosition. 
 */


void  Hv_W32SetInsertionPosition(Hv_Widget       w,
				 Hv_TextPosition pos) {
	    SendDlgItemMessage(w->msdialog, w->wcallback_id, EM_SETSEL, (WPARAM) pos, (LPARAM) pos); 

}


/**
 * Hv_W32ShowPosition
 * @purpose Windows 9X/NT specific version of Hv_ShowPosition.
 * @see The gateway routine Hv_ShowPosition. 
 */


void  Hv_W32ShowPosition(Hv_Widget       w,
			 Hv_TextPosition pos) {
}



/**
 * Hv_W32GetInsertionPosition
 * @purpose Windows 9X/NT specific version of Hv_GetInsertionPosition.
 * @see The gateway routine Hv_GetInsertionPosition. 
 */


Hv_TextPosition  Hv_W32GetInsertionPosition(Hv_Widget  w) {
	DWORD dwStart = 0l;
    DWORD dwEnd = 0l;


    SendDlgItemMessage(w->msdialog, w->wcallback_id, EM_GETSEL, (WPARAM) (LPDWORD) &dwStart, (LPARAM) (LPDWORD)&dwEnd); 

	return (Hv_TextPosition ) dwEnd;
}



/**
 * Hv_W32UpdateWidget
 * @purpose Windows 9X/NT specific version of Hv_UpdateWidget.
 * @see The gateway routine Hv_UpdateWidget. 
 */


void  Hv_W32UpdateWidget(Hv_Widget w) {
}


/**
 * Hv_W32XMGetLastPosition
 * @purpose Windows 9X/NT specific version of Hv_XMGetLastPosition.
 * @see The gateway routine Hv_XMGetLastPosition. 
 */


Hv_TextPosition  Hv_W32GetLastPosition(Hv_Widget w) {
	return 0;
}



/**
 * Hv_W32GetToggleButton
 * @purpose Windows 9X/NT specific version of Hv_GetToggleButton.
 * @see The gateway routine Hv_GetToggleButton. 
 */


Boolean	Hv_W32GetToggleButton(Hv_Widget btn) {

	long lState;
	if(btn->wisWisable)
	{
	      lState = SendDlgItemMessage( btn->msdialog, btn->wcallback_id, BM_GETCHECK, 0, 0);
		  lState &= 0x03; /* mask of the useless bits */
	}
    else
	{
		lState = btn->wstate;
	}
	return (Boolean) (lState > 0 ? 1 : 0);
}

/**
 * Hv_W32SetToggleButton
 * @purpose X Windows  specific version of Hv_SetToggleButton.
 * @see The gateway routine Hv_SetToggleButton. 
 */

void	Hv_W32SetToggleButton(Hv_Widget btn,
			   Boolean   State)

/* Set the state of a toggle button  True --> selected */

{
	if(btn->wisWisable)
	{
	      SendDlgItemMessage( btn->msdialog, btn->wcallback_id, BM_SETCHECK, State, 0); 
		  btn->wstate = State;
	}
    else
	{
		btn->wstate = State;
	}
}

/**
 * Hv_W32Pic8ToImage
 * @purpose Windows 9X/NT  specific version of Hv_Pic8ToImage.
 * @see The gateway routine Hv_Pic8ToImage. 
 */

Hv_Image *Hv_W32Pic8ToImage(byte *pic8,
			    int   wide,
			    int   high,
			    unsigned long **mycolors,
			    byte *rmap,
			    byte *gmap,
			    byte *bmap) {

	int i;
	HBITMAP hbm;
    DIBSpec *mdib;

	byte  *imagedata, *ip, *pp, *pp2;
	int   j, nullCount;
	LONG  imWIDE;
    RGBQUAD  *ct;

    mdib = (DIBSpec *)calloc(1, sizeof(DIBSpec));

// use mycolors to convert color table used by
// image to use nearest Hv colors

    if (*mycolors == NULL) {
		Hv_Println("setting up colors");
       *mycolors = (malloc(256*sizeof(RGBQUAD)));
	  	ct = (RGBQUAD *)(*mycolors);

        for (i = 0; i < 256; i++) {
			ct->rgbRed = rmap[i];
			ct->rgbGreen = gmap[i];
			ct->rgbBlue = bmap[i];
			Hv_SetToNearestColor(ct);
			ct++;
        }
	}

//	nullCount = (4 - (wide % 4)) & 0x03;  /* # of padding bytes per line */
	nullCount = (8 - (wide % 8)) & 0x07;  /* # of padding bytes per line */

	fprintf(stderr, "Null Count: %d  wide: %d  high: %d\n", nullCount, wide, high);

	imWIDE = wide + nullCount;
	
/* Now create the image data - pad each scanline as necessary */
	imagedata = (void *) calloc(((imWIDE+1) * (high+1)), 1);
	if (!imagedata) {
	    Hv_Println("couldn't malloc imagedata");
	    return NULL;
	}

	pp =  pic8 + (wide * high) - 1;
    pp2 = pp;
	
	for (i=0, ip=imagedata; i<high; i++) {
	    pp2 = pp - wide;
	    for (j=0; j<wide; j++, ip++, pp2++) *ip = (byte) (*pp2);
	    
	    for (j=0; j<nullCount; j++, ip++) *ip = 0;
		pp -= wide;
	}

	Hv_SetMainHDC();
/*	hbm = CreateCompatibleBitmap(Hv_gc, imWIDE, high);

	if (hbm == 0) {
		fprintf(stderr, "Could not create bitmap for jpeg, last error: %d\n", GetLastError());
		return NULL;
	} */
	

// nasty job of converting the picture data to a bitmap


	mdib->ih.bV4Size = (DWORD)(sizeof(mdib->ih));
	mdib->ih.bV4Width = imWIDE;
	mdib->ih.bV4Height = (LONG)high;
	mdib->ih.bV4Planes = (WORD)1;
	mdib->ih.bV4BitCount = (WORD)8;
	mdib->ih.bV4V4Compression = BI_RGB;

	for (i = 0; i < 256; i++)
	 mdib->Colors[i] = ((RGBQUAD *)(*mycolors))[i];

	hbm =CreateDIBitmap(Hv_gc,
		&(mdib->ih),
		CBM_INIT,
		(CONST VOID *)imagedata,
		(CONST *)mdib,
		DIB_RGB_COLORS); 


	if (hbm == 0) {
		fprintf(stderr, "Could not create bitmap for jpeg, last error: %d\n", GetLastError());
		return NULL;
	} 

//	SetDIBits(Hv_gc, hbm, 0, high, imagedata, mdib, DIB_RGB_COLORS);
////	free(imagedata);
   	return (Hv_Image *)hbm; 
}
 


   
/**
 * Hv_W32GrabPointer
 * @purpose Windows 9X/NT  specific version of Hv_GrabPointer.
 * @see The gateway routine Hv_GrabPointer. 
 */

void            Hv_W32GrabPointer() {
}


/**
 * Hv_W32UngrabPointer
 * @purpose Windows 9X/NT  specific version of Hv_UngrabPointer.
 * @see The gateway routine Hv_UngrabPointer. 
 */

 void            Hv_W32UngrabPointer() {
}

/**
 * Hv_W32GetNextFile
 * @purpose Windows 9X/NT  specific version of Hv_GetNextFile.
 * @see The gateway routine Hv_GetNextFile. 
 */
#define Hv_MaxDirsOpen 60
 	static DIR local_dirs[Hv_MaxDirsOpen];

Boolean  Hv_W32GetNextFile(DIR *dp,
                           char *dirname,
                           char *extension,
                           FILE **fp,
                           char *fname) {

   char            *sstr;
   char            rfname[MAXPATHLEN+1];   
   int             nomorefiles = 1;
   Boolean         GoodFileName = False;
   WIN32_FIND_DATA FindData;

   while( !GoodFileName )
   {
      *fp = NULL;

      /* need to open the directory on the first call */
      /* Hv_opendir in W32 is a placebo */
      if( (DIR) -1 == dp) return False;

      if(local_dirs[(int)dp] == (DIR)-1) /* check to see if the dir has been opened */
      {
         strcpy(rfname,dirname);
         strcat(rfname,"/*");
         strcat(rfname,extension);
         local_dirs[(int)dp] = FindFirstFile( rfname,&FindData);
      }
      else
      {
         nomorefiles = FindNextFile( local_dirs[(int)dp], &FindData );
      }

  
      if(INVALID_HANDLE_VALUE == local_dirs[(int)dp] || nomorefiles == 0)
      {
         return False;
      }

      /* remove file extension */
      strcpy(fname, FindData.cFileName);
      sstr = strstr(fname, extension);
      if (sstr != NULL) sstr[0] = '\0';

      /* Ensure we don't have extra characters at end of file name */
      GoodFileName =  ( strlen(fname) + strlen(extension) == strlen(FindData.cFileName) );
   }

   strcpy(rfname,dirname);
   strcat(rfname,"/");
   strcat(rfname,FindData.cFileName);
   *fp = fopen(rfname, "r");
 
/* end of matched extension */
      
      
   return True;
}

/**
 * Hv_W32OpenDirectory
 * @purpose Windows 9X/NT specific version of Hv_OpenDirectory.
 * @see The gateway routine Hv_OpenDirectory. 
 */

DIR  *Hv_W32OpenDirectory(char *dname) {

	static initialize = 0;
    int i = 0;

	if(initialize == 0)
	{
		for(i=1;i<Hv_MaxDirsOpen;i++)
			local_dirs[i] = (DIR) -1;

		initialize = 1;
    }
		
	i = 1;
	while(i<Hv_MaxDirsOpen)
	{
		if(local_dirs[i] == (DIR) -1)
			break;
		i++;
	}

    if(i>=Hv_MaxDirsOpen)
		return (DIR) -1;
	else
		return (DIR) i;
     
 /* the open for W32 is handled by Hv_Findnextfile */
}
/**
 * Hv_W32CloseDirectory
 * @purpose Windows 9X/NT specific version of Hv_CloseDirectory.
 * @see The gateway routine Hv_CloseDirectory. 
 */

int  Hv_W32CloseDirectory(DIR *dp) {

int ldp ;

    ldp = (int) dp;

	if(ldp <0 || ldp > Hv_MaxDirsOpen)
		return -1;

	FindClose(  local_dirs[ldp]);  
	local_dirs[ldp] = (DIR) -1;
	return 1;
     
 /* the open for W32 is handled by Hv_Findnextfile */
}

/*==========================================
 * LOCAL (STATIC) ROUTINES
 *===========================================*/

/*
 * 
 *  BOOL initInstance (HINSTANCE hinst, int nCmdShow)
 *
 *  hinst           Application module (instance) handle
 *  nCmdShow        Initial show window state
 *
 *  PURPOSE:        Initialize the application
 *
 *  COMMENTS:
 *      Activate a previous instance, if any, by searching for a window
 *      created with the same window class name that this instance uses.
 *      If found, restore the previous instance's window, if necessary,
 *      and make that window the foreground window.
 *
 *      Register the window class(es) using a WNDCLASSEX structure.
 *
 */

static BOOL initInstance (HINSTANCE hinst,
						  int nCmdShow) {

    HWND  hwnd;

 /* Constrain this application to run single instance */

    hwnd = FindWindow (Hv_appName, NULL) ;
    if (hwnd != NULL) {

/*
 * A previous instance of this application is running.
 *
 * Activate the previous instance, tell it what the user
 * requested this instance to do, then abort initialization
 * of this instance.
 */
        if (IsIconic (hwnd))
            ShowWindow (hwnd, SW_RESTORE) ;

        SetForegroundWindow (hwnd) ;
/*
 * Send an application-defined message to the previous
 * instance (or use some other type of IPC mechanism)
 * to tell the previous instance what to do.
 *
 * Determining what to do generally depends on how the
 * user started this instance.
 * ... <some application-specific code here>
 * Abort this instance's initialization
 */

        return FALSE ;
    }

/* Register all application-specific window classes */

    if (!registerWindowClasses (hinst))
        return FALSE ;

/*
 * Initialize the Common Controls DLL
 * You must call this function before using any Common Control
 */

    InitCommonControls () ;

/* Create the application's main frame window */

    if (!createMainFrameWindow (hinst, nCmdShow))
        return FALSE ;

    return TRUE ;
}


static BOOL registerWindowClasses (HINSTANCE hinst) {


    WNDCLASSEX wcex ;

/*
 * Fill in window class structure with parameters that describe
 * the main window.
 */

	wcex.cbSize        = sizeof (WNDCLASSEX) ;
    wcex.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS ;
    wcex.lpfnWndProc   = mainFrameWndProc ;
    wcex.cbClsExtra    = 0 ;
    wcex.cbWndExtra    = 0 ;
    wcex.hInstance     = hinst ;
    wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW+1) ;

    wcex.lpszMenuName  = "Hv" ;
    wcex.lpszClassName = Hv_appName ;
    wcex.hIconSm       = NULL;
 
// Register the window class and return success/failure code.

	winclass = RegisterClassEx(&wcex);
    return (winclass != 0);
}



static HWND createMainFrameWindow (HINSTANCE hinst,
								   int nCmdShow) {
    HWND  hwnd ;
//    HMENU Hv_MainMenu = CreateMenu();

/* Create the main frame window */

    hwnd =
        CreateWindowEx (0,              // Extended window styles
                    MAKEINTATOM(winclass), 
                    Hv_appName,         // Address of window name
                    WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN ,// Window style
                    0,      // Horizontal position of window
                    0,                  // Vertical position of window
                    1000,      // Window width
                    700,                  // Window height
                    NULL,               // Handle of parent or owner window
                    (HMENU)(Hv_menuBar->wdata),  // Handle of menu for this window
                    hinst,              // Handle of application instance
                    NULL) ;             // Address of window-creation data

    ASSERT (NULL != hwnd) ;

/* tie it to the Hv main window variable */

	Hv_mainWindow = hwnd;

    if (!hwnd)
       return NULL ;

    ShowWindow (hwnd, nCmdShow) ;
    UpdateWindow (hwnd) ;
    DrawMenuBar(hwnd);

    return hwnd ;
}


/*
 *
 *  LRESULT CALLBACK
 *  mainFrameWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
 *
 *  hwnd            Handle of window to which this message applies
 *  message         Message number
 *  wParam          Message parameter
 *  lParam          Message parameter
 *
 *  PURPOSE:  Processes messages for the main window.
 *
 *  MESSAGES:
 *
 *      WM_COMMAND          - notification from the menu or controls
 *      WM_DESTROY          - window is being destroyed
 *
 */

static LRESULT CALLBACK mainFrameWndProc (HWND hwnd,
								   UINT message,
								   WPARAM wParam,
								   LPARAM lParam) {
	PAINTSTRUCT    ps;
	HRGN           hrgn;
	Hv_XEvent      xevent;
	POINT          clp; // position in client coordinate sys
	DWORD          pos; // gives coordinates in screen system

	static int     w, h;  //screen width & height

    HDC            hdc;
	static short   oldcanvascolor = -1;
  static int     paintcount = 0;


	switch (message) {

	case WM_CREATE:

/* add the standard menus here */

	  Hv_mainWindow = (PVOID)hwnd;
	  Hv_InsertStartupMenus();

// get the memory DC for offscreen drawing

	  hdc = GetDC(hwnd);
	  hdcmem = CreateCompatibleDC(hdc);
	  ReleaseDC(hwnd, hdc);

		return 0;

	case WM_TIMER:
	//	TimerProc(hwnd,wParam,wParam,lParam);
		return TRUE;
	case WM_COMMAND:                    /* Notification from menu or control */
	  return HANDLE_WM_COMMAND (hwnd, wParam, lParam, mainFrame_OnCommand) ;

	case WM_DESTROY: /* Window is being destroyed */
	  return HANDLE_WM_DESTROY (hwnd, wParam, lParam, mainFrame_OnDestroy) ;

	case WM_KEYUP:
		Hv_GetClientPosition(&clp);
		if ((wParam >= VK_F1) && (wParam < VK_F10))
			Hv_HandleFunctionKey(clp.x,
			                     clp.y,
								 wParam - VK_F1 + 1);
		return 0;
	  
	case WM_LBUTTONDBLCLK:
			Hv_lastClickType = 2;
//			Hv_Information("Double click!");
			return 0;
			
	case WM_LBUTTONDOWN:
	  Hv_allowMag = False;
	  Hv_lastClickType = 1;
	  
	  Hv_GetClientPosition(&clp);
	  
	  Hv_StuffXEvent(&xevent,
			 clp.x,
			 clp.y,
			 1,
			 Hv_XModifiers(wParam),
			 Hv_buttonPress);
	  
	  Hv_ButtonPress(NULL, NULL, &xevent);
	  Hv_allowMag = True;
	  return 0; // pass this event on to the dialog handlers
			
	case WM_MBUTTONDOWN:
			Hv_allowMag = False;
	        Hv_lastClickType = 1;

			Hv_GetClientPosition(&clp);
			
			Hv_StuffXEvent(&xevent,
				       clp.x,
				       clp.y,
				       2,  // middle mouse button
				       Hv_XModifiers(wParam),
				       Hv_buttonPress);
			
			Hv_ButtonPress(NULL, NULL, &xevent);
			Hv_allowMag = True;
	  return 0;

	case WM_RBUTTONDOWN:
	  Hv_allowMag = False;
	  Hv_lastClickType = 1;
	  pos = GetMessagePos();
	  Hv_GetClientPosition(&clp);
	  HandlePopupMenus(GET_X_LPARAM(pos), GET_Y_LPARAM(pos), &clp,(HWND)Hv_mainWindow);
	  Hv_allowMag = True;
	  return 0;

	case WM_LBUTTONUP:
	  Hv_allowMag = False;
	  Hv_lastClickType = 1;
	  Hv_GetClientPosition(&clp);
	  
	  Hv_StuffXEvent(&xevent,
			 clp.x,
			 clp.y,
			 1,
			 Hv_XModifiers(wParam),
			 Hv_buttonRelease);
	  
			Hv_ButtonRelease(NULL, NULL, &xevent);
			Hv_allowMag = True;
			return 0;

	case WM_MBUTTONUP:
	  Hv_allowMag = False;
	  Hv_lastClickType = 1;
	  Hv_GetClientPosition(&clp);
	  
	  Hv_StuffXEvent(&xevent,
			 clp.x,
			 clp.y,
			 2,
			 Hv_XModifiers(wParam),
			 Hv_buttonRelease);
	  
			Hv_ButtonRelease(NULL, NULL, &xevent);
			Hv_allowMag = True;
			return 0;

	case WM_MOUSEMOVE:
	  Hv_lastClickType = 1;
	  Hv_GetClientPosition(&clp);
	  
	  Hv_StuffXEvent(&xevent,
			 clp.x,
			 clp.y,
			 1,
			 Hv_XModifiers(wParam),
			 Hv_motionNotify);
	  
	  Hv_PointerMotion(NULL, NULL, &xevent);
	  return 0;

	case WM_PALETTECHANGED:
	  return 0;

	case WM_PAINT:

/*
 * Realize the palette
 */


		if ((Hv_gc != (HANDLE)(-1)) && (Hv_gc != 0)) {
		  ReleaseDC(hwnd, Hv_gc);
		  Hv_gc = (HANDLE)(-1);
		}

	  Hv_gc = BeginPaint(hwnd, &ps);

	  if (Hv_firstExposure) {
      Hv_screenResolution = GetDeviceCaps(Hv_gc, LOGPIXELSX);
      if(Hv_colorMap != NULL) {
        SelectPalette(Hv_gc, Hv_colorMap, False);
        RealizePalette(Hv_gc);
        Hv_firstExposure = False;
      }
    }
    
    if ((Hv_TileDraw != NULL) && (logo == 0)) {
      Hv_W32ReTileMainWindow();
    }
    
    if (oldcanvascolor != Hv_canvasColor) {
      Hv_ChangeWindowBackground(Hv_canvasColor);
      oldcanvascolor = Hv_canvasColor;
    }

// intialize hrgn to an arbitrary rectangle

    if (paintcount < 1) {
      Hv_noViewDraw = True;
      paintcount++;
    }
    else {
      Hv_noViewDraw = False;
      hrgn = CreateRectRgn(ps.rcPaint.left,
        ps.rcPaint.top,
        ps.rcPaint.right,
        ps.rcPaint.bottom);
     
      Hv_UpdateViews(hrgn);
    }

	  Hv_DestroyRegion(hrgn);
	  EndPaint(hwnd, &ps);
	  Hv_gc = (HANDLE)(-1);
	  return 0;
	  
	case WM_SIZE:
	  w = LOWORD(lParam);
	  h = HIWORD(lParam);

	  Hv_SetRect(&Hv_canvasRect, 0, 0, (short)w, (short)h);
	  return 0;

	case WM_SYSKEYDOWN:
		Hv_altKeyIsPressed = 1;
		return 0;
	case WM_SYSKEYUP:
		Hv_altKeyIsPressed = 0;
		return 0;
	default:
	  return DefWindowProc (hwnd, message, wParam, lParam) ;
	}
}

static void Hv_FreeResources() {
	int i;

// menus

	DestroyMenu(GETMENUHANDLE(Hv_menuBar));

// accel table

	if (haccel != NULL)
		DestroyAcceleratorTable(haccel);

// pallette

	if (Hv_colorMap != 0)
		DeletePalette(Hv_colorMap);

// callbacks

    for (i = 0; i < maxCallbacks; i++)
		if (callbacks[i] != NULL)
			free(callbacks[i]);

	free(callbacks);

// fonts

	for (i = 0; i < Hv_NUMFONTS; i++)
		DeleteObject(Hv_fonts[i]);

// device context

	DeleteDC(hdcmem);
	DeleteDC(Hv_gc);


}

/*
*
*  void mainFrame_OnCommand (HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
*
*  hwnd            Handle of window to which this message applies
*  id              Specifies the identifier of the menu item, control, or accelerator.
*  hwndCtl         Handle of the control sending the message if the message
*                  is from a control, otherwise, this parameter is NULL. 
*  codeNotify      Specifies the notification code if the message is from a control.
*                  This parameter is 1 when the message is from an accelerator.
*                  This parameter is 0 when the message is from a menu.                  
*
*  PURPOSE:        
*
*  COMMENTS:
*
*/

static void mainFrame_OnCommand (HWND hwnd,
								 int  id,
								 HWND hwndCtl,
								 UINT codeNotify) {
		Hv_HandleWidget(id);

}

/*

 *
 *  void mainFrame_OnDestroy (HWND hwnd)
 *
 *  hwnd            Handle of window to which this message applies
 *
 *  PURPOSE:        Notification that the specified window is being destroyed.
 *                  The window is no longer visible to the user.
 *
 *  COMMENTS:
 *
 */

static void mainFrame_OnDestroy (HWND hwnd) {

	Hv_FreeResources();
	PostQuitMessage (0) ;
}


/*
*  int exitInstance (PMSG msg)
*
*  hinst           Pointer to MSG structure
*
*  PURPOSE:        Perform deinitialization and return exit code
*
*  COMMENTS:
*
*/

static int exitInstance (MSG* pmsg) {
    return pmsg->wParam ;
}

/*
*
*  HWND createMainFrameWindow (HINSTANCE hinst, int nCmdShow)
*
*  hinst           Application module (instance) handle
*  nCmdShow        Initial show window state
*
*  PURPOSE:        Create the application's main frame window
*                  and show the window as requested
*
*  COMMENTS:
*
*/

#ifdef _DEBUG

/*
*
*  BOOL assertFailedOnLine (LPCSTR FileName, int Line)
*
*  FileName        Name of source file where assertion failed
*  Line            Line number of source line where assertion failed
*
*  PURPOSE:        Display assertion notification message box
*                  
*                  
*
*  COMMENTS:
*
*/

BOOL assertFailedOnLine (LPCSTR FileName,
						 int Line) {
    HWND   hWndParent ;
    int    Code ;
    TCHAR  Message [_MAX_PATH*2] ;

    /* Format the output string */
    wsprintf (Message, TEXT("File %hs, Line %d"), FileName, Line) ;

    /* Get the last active popup window for the current thread */
    hWndParent = GetActiveWindow () ;
    if (hWndParent != NULL)
        hWndParent = GetLastActivePopup (hWndParent) ;

    /* display the assert */
    Code = MessageBox (hWndParent, Message, TEXT("Assertion Failed!"),
            MB_ABORTRETRYIGNORE | MB_ICONERROR | 
            MB_SETFOREGROUND | MB_TASKMODAL) ;

    switch (Code) {
    case IDIGNORE:      /* Ignore */
	return FALSE ;

    case IDRETRY:       /* Retry */
	DebugBreak () ;
	return TRUE ;

    default:            /* Abort */
	return TRUE ;
    }
}

#endif                  /* _DEBUG */


/**
 *   HandlePopupMenus
 *   @param  x    screen X location of click
 *   @param  y    screen Y location of click
 *   @param  clp  client location of click
 */

void HandlePopupMenus(int x,
							 int y,
							 POINT *clp,
							 HWND hwnd) {

    Hv_XEvent         xevent;
 	Hv_Point          pp;
	int               win32Id;
	HMENU             hmenu;
 DWORD err;
/* see if we are on a view, or an inert part of the menu */

	Hv_SetPoint(&pp, (short)(clp->x), (short)(clp->y));
    Hv_WhichView(&Hv_hotView, pp);

    if (Hv_hotView != NULL)
	{
		hmenu = GETMENUHANDLE(Hv_popupMenu);
			  Hv_StuffXEvent(&xevent,
			 pp.x,
			 pp.y,
			 2,
			 0,
			 0);
	  
        Hv_ViewPopup(Hv_hotView,&xevent);
	}
    else
        hmenu = GETMENUHANDLE(Hv_popupMenu2);


/* Track the popup, which then returns the ID of the menu item */

	win32Id = (int)TrackPopupMenu(hmenu,
			            TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON | TPM_HORIZONTAL | TPM_RETURNCMD,
			            x, y,
						0,
						//hwnd,
						Hv_mainWindow,
						NULL);
    err = GetLastError();
	Hv_HandleWidget(win32Id);

}



/**
 *  Hv_InsertStartupMenus
 *  Insert the default menus
 */


static void Hv_InsertStartupMenus() {

	HMENU   hWndMenu;

	int  pos = 1;

/* hWndMenu is basically the menu bar */

	hWndMenu = GetMenu((HWND)Hv_mainWindow);

	InsertMenu(hWndMenu, ++pos, MF_POPUP | MF_BYPOSITION, (DWORD)(Hv_actionMenu->wdata), "Action");
	InsertMenu(hWndMenu, ++pos, MF_POPUP | MF_BYPOSITION, (DWORD)(Hv_viewMenu->wdata), "Views");
	InsertMenu(hWndMenu, ++pos, MF_POPUP | MF_BYPOSITION | MF_RIGHTJUSTIFY, (DWORD)(Hv_helpMenu->wdata), "Help");
	standardmenusdone = True;
}


/**
 * Hv_GetNextCallback
 * Get the next available callback rec, allocate if necessary
 */

#define CALLBACKNUMINC   1000    //# new ones created if necessary

static  Hv_CallbackData  Hv_GetNextCallback() {

	int i;

/*
 * first see if any slots can be reclaimed. This should
 * be reimplemented as a stack
 */

    for (i = 0; i < maxCallbacks; i++) {
		if (callbacks[i] == NULL) {
		     callbacks[i] = (Hv_CallbackData)Hv_Malloc(sizeof(Hv_CallbackRec));
	         callbacks[i]->win32Id = i;
			 return callbacks[i];
		}
	}

// If here, need more

    maxCallbacks += CALLBACKNUMINC;
    callbacks = (Hv_CallbackData *)realloc(callbacks,
		maxCallbacks * sizeof(Hv_CallbackData));
    for (i = maxCallbacks - CALLBACKNUMINC; i < maxCallbacks; i++) {
      callbacks[i] = NULL;
    }

    return Hv_GetNextCallback();
}

#undef CALLBACKNUMINC

/**
 *  widgetDispatcher
 *  @local
 */

void widgetDispatcher(Hv_Widget w32data,int userData,HWND hwnd,UINT notifyCode)
{
     int i=0;
     int sel = 0;
     Hv_LinkList *wp = NULL,*rp = NULL;
     Hv_callbackData   *callback;
	 Hv_Widget   menuWidget;
     static int deselect = FALSE;

//	 if(deselect == TRUE )
//	 {
//		 deselect = FALSE;
//		 return;
//	 }
	 if(w32data->wcallCount == 0) /* no callbacks for this widget so exit */
		 return;
	 switch(w32data->wtype)
	 {
		case Hv_BUTTONDIALOGITEM:  /* butons can only get activate callbacks */
            if (notifyCode == BN_PUSHED)
			{

			}
			if(w32data->wcallList)  
			{
		       for (wp = w32data->wcallList->next; wp != w32data->wcallList; wp = wp->next)
			   {
				   callback = (Hv_callbackData *)(wp->data);
				   if(callback->icallbackType == Hv_ACTIVATECALLBACK)
				   {
					   callback->callback(w32data,callback->clientPointer,callback->clientData,callback->callbackType);
				   }
			   }
			}

			break;
		case Hv_COLORSELECTORDIALOGITEM:  
			break;
		case Hv_SEPARATORDIALOGITEM:
			break;
		case Hv_LISTDIALOGITEM: 
		   /*
			switch (notifyCode)
			{
			case LBN_DBLCLK:
				i=1;
				break;
			case LBN_ERRSPACE:
					i=1;
				break;
			case LBN_KILLFOCUS:
					i=1;
				break;
			case LBN_SELCANCEL:
					i=1;
				break;
			case LBN_SELCHANGE:
					i=1;
				break;
			case LBN_SETFOCUS:
				i=1;
				break;
			}
			*/
			if(notifyCode !=LBN_SELCHANGE)
                     notifyCode = notifyCode;  // for debug purposes
			if(w32data->wcallList)  
			{
		       for (wp = w32data->wcallList->next; wp != w32data->wcallList; wp = wp->next)
			   {
				  callback = (Hv_callbackData *)(wp->data);
				  if((callback->icallbackType ==  Hv_MULTIPLESELECTIONCALLBACK) && (notifyCode ==LBN_SELCHANGE))
				   {
					   callback->callback(w32data,callback->clientPointer,callback->clientData,callback->callbackType);
				   }

				  if(   (callback->icallbackType ==  Hv_SINGLESELECTIONCALLBACK) && (notifyCode ==LBN_SELCHANGE))
				   {
					 if(w32data->wisWisable) // check to see if this was a "deselect"
					 {
                       sel = SendDlgItemMessage( w32data->msdialog, w32data->wcallback_id, LB_GETCURSEL, 0, 0);

					   if((w32data->wstate)-1 == sel)  // it was a deselect so fix things up
						 {
//Hv_W32NewPosSelectListItem(w32data,
//				sel+1,
//				FALSE) ;
//			   deselect = TRUE;
//                          Hv_W32DeselectAllListItems(w32data);
						   SendDlgItemMessage( w32data->msdialog, w32data->wcallback_id, LB_SETCURSEL, -1, 0);
						 }
                     }
					   callback->callback(w32data,callback->clientPointer,callback->clientData,callback->callbackType);
				   }
			   }
			}
			break;
		case Hv_ROWCOLUMNDIALOGITEM:
			break;
		case Hv_SCALEDIALOGITEM: 
			break;
		case Hv_RADIODIALOGITEM: 
			if(w32data->wcallList)  
			{
		       for (wp = w32data->wcallList->next; wp != w32data->wcallList; wp = wp->next)
			   {
				   callback = (Hv_callbackData *)(wp->data);
				   if(callback->icallbackType == Hv_VALUECHANGEDCALLBACK)
				   {
					   callback->callback(w32data,callback->clientPointer,callback->clientData,callback->callbackType);
				   }
			   }
			}

			break;
		case Hv_TOGGLEDIALOGITEM: 
			if(w32data->wcallList)  
			{
		       for (wp = w32data->wcallList->next; wp != w32data->wcallList; wp = wp->next)
			   {
				   callback = (Hv_callbackData *)(wp->data);
				   if(callback->icallbackType == Hv_VALUECHANGEDCALLBACK)
				   {
					   callback->callback(w32data,callback->clientPointer,callback->clientData,callback->callbackType);
				   }
			   }
			}

			break;
		case Hv_TEXTDIALOGITEM:
			if(w32data->wcallList)  
			{
		       for (wp = w32data->wcallList->next; wp != w32data->wcallList; wp = wp->next)
			   {
				   callback = (Hv_callbackData *)(wp->data);
				   if(callback->icallbackType == Hv_VALUECHANGEDCALLBACK)
				   {
					   callback->callback(w32data,callback->clientPointer,callback->clientData,callback->callbackType);
				   }
			   }
			}

			break;
		case Hv_CLOSEOUTDIALOGITEM:
			if(w32data->wcallList)  
			{
		       for (wp = w32data->wcallList->next; wp != w32data->wcallList; wp = wp->next)
			   {
				   callback = (Hv_callbackData *)(wp->data);
				   if(callback->icallbackType) /* call then all for a closeout */
				   {
					   callback->callback(w32data,callback->clientPointer,callback->clientData,callback->callbackType);
				   }
			   }
			}
			break;
		case Hv_SCROLLEDTEXTDIALOGITEM:  
			if(w32data->wcallList)  
			{
		       for (wp = w32data->wcallList->next; wp != w32data->wcallList; wp = wp->next)
			   {
				   callback = (Hv_callbackData *)(wp->data);
				   if(callback->icallbackType == Hv_VALUECHANGEDCALLBACK)
				   {
					   callback->callback(w32data,callback->clientPointer,callback->clientData,callback->callbackType);
				   }
			   }
			}

			break;
		case Hv_LABELDIALOGITEM: 
			if(w32data->wcallList)  
			{
		       for (wp = w32data->wcallList->next; wp != w32data->wcallList; wp = wp->next)
			   {
				   callback = (Hv_callbackData *)(wp->data);
				   if(callback->icallbackType == Hv_ACTIVATECALLBACK)
				   {
					   callback->callback(w32data,callback->clientPointer,callback->clientData,callback->callbackType);
				   }
			   }
			}
			break;
		case Hv_FILESELECTORDIALOGITEM:  
			break;
		case Hv_ACTIONAREADIALOGITEM:
			break;
		case Hv_DIALOG:   
			break;
		case Hv_THINMENUOPTION:
			if(notifyCode == CBN_SELCHANGE)
			{
                /* thin menus are a strange beast */
                /* the callback that needs to be called is one of its childrens */
				/* in particular the ith childrens callback */

				/* find out which option has been selected */
				w32data->wstate =	SendDlgItemMessage( w32data->msdialog, w32data->wcallback_id, CB_GETCURSEL  , 
				                        (WPARAM) NULL, (LPARAM) NULL);
                sel = w32data->wstate;
				w32data = (Hv_Widget) w32data->generic; // get the submenu widget 
				/* the selected item # corresponds to the child # */
                if(w32data->num_children < w32data->wstate)  /* funny error checking */
					Hv_Warning("LTL Warning: You don't have enough children");
				/* find the desired child */
				if(w32data->wchildren)  
				{
				   for (i=0,wp = w32data->wchildren->next; i != sel ;i++, wp = wp->next)
				   {
				   }
				   menuWidget = (Hv_Widget) wp->data;
				   /* now go down the childs call list to get the correct callback */
				   if(menuWidget->wcallList)  
				   {
				     for (rp = menuWidget->wcallList->next; rp != menuWidget->wcallList; rp = rp->next)
					 {
				  	   callback = (Hv_callbackData *)(rp->data);
					   if(callback->icallbackType == Hv_ACTIVATECALLBACK)
					   {
						 callback->callback(menuWidget,sel,sel,callback->callbackType);
					   }
					 }
				   }
				   
				}
			}
		default:
			break;
	 }



}

/**
 *  Hv_createNewWidgetEx
 *  @return  pointer to a Hv_WidgetInfo structure
 *  @local
 */

static Hv_Widget   createNewWidgetEx(Hv_FunctionPtr   callback,
					      Hv_Widget        parent,
						  int              userId,
						  short            type){


 	Hv_Widget w = (Hv_Widget) Hv_Malloc(sizeof(Hv_WidgetInfo));
	w->wcallback_id = Hv_NewWidgetCallback(callback, parent, w, userId, type);
	w->wtype = type;
	w->wbgColor = Hv_DEFAULT;
	w->wfgColor = Hv_DEFAULT;

	return w;
}


/**
 *  Hv_createNewWidget
 *  @return  pointer to a Hv_WidgetInfo structure
 *  @local
 */

Hv_Widget createNewWidget(short wtype)
{

 	Hv_Widget w = (Hv_Widget) Hv_Malloc(sizeof(Hv_WidgetInfo));

// put widget in for parent and w!

	w->wcallback_id = Hv_NewWidgetCallback(widgetDispatcher, w, w, NULL, wtype);
	w->wtype = wtype;
	w->wbgColor = Hv_DEFAULT;
	w->wfgColor = Hv_DEFAULT;
	return w;
}
/**
 *  Hv_NewWidgetCallback
 *  @param   callback    The actual callback;
 *  @param   parent      Parent widget
 *  @param   w           The widget itself
 *  @param   userId      Application assigned Id
 *  @param   type        Local type, such as MENUITEM
 *  @return  unique, internal Id
 *  @local
 */

static int   Hv_NewWidgetCallback(Hv_FunctionPtr   callback,
						  Hv_Widget        parent,
						  Hv_Widget        w,
						  int              userId,
						  short            type) {


	Hv_CallbackData   cd;

/*
 * call GetNextCallback first, since it will allocate
 * the callback rec if necessary
 */

	cd = Hv_GetNextCallback();
	if (cd == NULL)
		return -1;

	cd->callback = callback;
	cd->parent   = parent;
	cd->w        = w;
	cd->userId   = userId;
	cd->type     = type;
	return cd->win32Id;
}

/**
 * Hv_HandleWidget
 * @param win32Id   The unique internal Id
 * @return  True if sucessfully dispatched
 */

static Boolean Hv_HandleWidget(int win32Id) {

	Hv_CallbackData  cd;
    Hv_Widget w,ow;

	if ((win32Id < 0) || (win32Id >= maxCallbacks) || (callbacks == NULL))
		return False;

	cd = callbacks[win32Id];
	if ((cd == NULL) || (cd->callback == NULL))
		return False;
   w = (Hv_Widget)(cd->parent);
   ow = (Hv_Widget)w->generic;
/* was this a menu toggle? */

	if (cd->type == TOGGLEMENUITEM) {
		Hv_ChangeMenuToggleState((HMENU)(cd->parent->wdata), cd->win32Id);
	}

/* The unique, internal Id serves as the widget identifier */

	cd->callback(cd->w, cd->userId);
	return True;
}



/**
 * Hv_ChangeMenuToggleState
 */

static void Hv_ChangeMenuToggleState(HMENU menu,
									 int ID) {
	MENUITEMINFO  minfo;
	minfo.cbSize = sizeof(MENUITEMINFO);
	minfo.fMask = MIIM_STATE;
	GetMenuItemInfo(menu, ID, False, &minfo);
	minfo.fState ^= MFS_CHECKED;
	SetMenuItemInfo(menu, ID, False, &minfo);

}


/**
 *  Hv_GetCallbackData
 */

static Hv_CallbackData Hv_GetCallbackData(int win32Id) {

	if ((win32Id < 1) || (win32Id >= maxCallbacks))
		return NULL;

	return callbacks[win32Id];
}

/**
 * Hv_AddAccelerator
 * @param win32Id  The id to use (matching a menuitem Id)
 * @param acctype  Hv_ALT or Hv_CTRL
 * @param accchr   What character 
 */

static void Hv_StuffAcceleratorData(LPACCEL lpaccel,
									int     win32Id,
									int     acctype,
									char    accchr) {

	lpaccel->cmd = win32Id;
	lpaccel->key = accchr;

	switch (acctype) {

	case Hv_ALT:
		lpaccel->fVirt = FALT;
		break;

	default:
		lpaccel->fVirt = FCONTROL;
		break;

	}

}


static void Hv_AddAccelerator(int   win32Id,
						 int   acctype,
						 char  accchr) {


	static    int numaccel = -1;
    ACCEL     accel;
	LPACCEL   lpaccel;
	int       numac;

	numaccel++;


	if ((accchr >= 'A') && (accchr <= 'Z'))
		accchr += ('a' - 'A');

	if (numaccel == 0) {
		Hv_StuffAcceleratorData(&accel, win32Id, acctype, accchr);
		haccel = CreateAcceleratorTable(&accel, 1);
		return;
	}

	numac = CopyAcceleratorTable(haccel, NULL, 0);
	lpaccel = (LPACCEL)Hv_Malloc((numac+1)*sizeof(ACCEL));
	numac = CopyAcceleratorTable(haccel, lpaccel, numac);
	DestroyAcceleratorTable(haccel);
	Hv_StuffAcceleratorData(lpaccel + numac, win32Id, acctype, accchr);
	haccel = CreateAcceleratorTable(lpaccel, numaccel);

    Hv_Free(lpaccel);
}
 
#define STRICT
#include <windows.h>            // Fundamental Windows header file
#include <windowsx.h>           // Useful Windows programming extensions
#include <commctrl.h>           // Common Controls declarations

#include "Hv_dlgtemplate.h"

/*****************************************************************************
                                 appendString
*****************************************************************************/

__inline LPWORD appendString(LPWORD ptr, LPCWSTR text)
    {
     LPWSTR str = (LPWSTR)ptr;
     wcscpy(str, text);
     ptr = (LPWORD)(str + wcslen(str) + 1);
     return ptr;
    }


/*****************************************************************************
                                 setClassAtom
*****************************************************************************/

__inline LPWORD setClassAtom(LPDLGITEMTEMPLATE item, WORD classatom)
    {
     LPWORD ptr = (LPWORD)&item[1];
     *ptr++ = 0xFFFF;
     *ptr++ = classatom;
     return ptr;
    }

/*****************************************************************************
                                 SetClassName
*****************************************************************************/
__inline LPWORD SetClassName(LPDLGITEMTEMPLATE item, LPCWSTR classname)
    {
     LPWORD ptr = (LPWORD)&item[1];
     ptr = appendString(ptr, classname);
     return ptr;
    }

/*****************************************************************************
                                 setResourceID
*****************************************************************************/

__inline LPWORD setResourceID(LPWORD ptr, short id)
    {
     *ptr++ = 0xFFFF;
     *ptr++ = (WORD)id;
     return ptr;
    }

/*****************************************************************************
                                 noParms
*****************************************************************************/
__inline LPWORD noParms(LPDLGITEMTEMPLATE item, LPWORD ptr)
    {
     *ptr++ = 0;   // no parameters
	 /**
     if( (((LPWORD)item) - ptr) & 1)
        *ptr++ = 0;
     **/
	 ptr = ALIGN_DWORD(LPDLGITEMTEMPLATE, ptr);
     return ptr;
    }



/*****************************************************************************
                                  setItemPos
*****************************************************************************/

__inline void setItemPos(LPDLGITEMTEMPLATE item, int x, int y, int cx, int cy)
    {
      item->x = x;
      item->y = y;
      item->cx = cx;
      item->cy = cy;
    }


/*****************************************************************************
                                  AUTO3STATE
*****************************************************************************/

LPDLGITEMTEMPLATE AUTO3STATE(LPDLGITEMTEMPLATE item, LPCWSTR text,
                        short id, int x, int y, int cx, int cy, 
                        DWORD style, DWORD exstyle)
    {
     return CONTROL(item, text, id, MAKEINTRESOURCEW(0x0080), 
                    style | BS_AUTO3STATE,
                    x, y, cx, cy, 
                    exstyle);
    }

/*****************************************************************************
                                AUTOCHECKBOX
*****************************************************************************/

LPDLGITEMTEMPLATE AUTOCHECKBOX(LPDLGITEMTEMPLATE item, LPCWSTR text,
                        short id, int x, int y, int cx, int cy, 
                        DWORD style, DWORD exstyle)
    {
     return CONTROL(item, text, id, MAKEINTRESOURCEW(0x0080),
                    style | BS_AUTOCHECKBOX,
                    x, y, cx, cy, 
                    exstyle);
    }

/*****************************************************************************
                                AUTORADIOBUTTON
*****************************************************************************/

LPDLGITEMTEMPLATE AUTORADIOBUTTON(LPDLGITEMTEMPLATE item, LPCWSTR text,
                        short id, int x, int y, int cx, int cy, 
                        DWORD style, DWORD exstyle)
    {
     return CONTROL(item, text, id, MAKEINTRESOURCEW(0x0080),
                    style | BS_AUTORADIOBUTTON,
                    x, y, cx, cy, 
                    exstyle);
    }

/*****************************************************************************
                                   CHECKBOX
*****************************************************************************/

LPDLGITEMTEMPLATE CHECKBOX(LPDLGITEMTEMPLATE item, LPCWSTR text,
                        short id, int x, int y, int cx, int cy, 
                        DWORD style, DWORD exstyle)
    {
     return CONTROL(item, text, id, MAKEINTRESOURCEW(0x0080),
                    style | BS_CHECKBOX,
                    x, y, cx, cy, 
                    exstyle);
    }

/*****************************************************************************
                                   COMBOBOX
*****************************************************************************/

LPDLGITEMTEMPLATE COMBOBOX(LPDLGITEMTEMPLATE item, short id, 
                        int x, int y, int cx, int cy, DWORD style, 
                        DWORD exstyle)
    {

     return CONTROL(item, L"", id, MAKEINTRESOURCEW(0x0085),
                       style, 
                       x, y, cx, cy, 
                       exstyle);

    }

/*****************************************************************************
                                    CONTROL
*****************************************************************************/

LPDLGITEMTEMPLATE CONTROL(LPDLGITEMTEMPLATE item, LPCWSTR text, 
                                        short id, LPCWSTR classname, 
                                        DWORD style, 
                                        int x, int y, int cx, int cy, 
                                        DWORD exstyle)
    {
     LPWORD ptr = (LPWORD) &item[1];

     item->style = WS_CHILD | style;
     item->dwExtendedStyle = exstyle;
     setItemPos(item, x, y, cx, cy);
     item->id = (WORD)id;

     if(HIWORD(classname) != 0)
        ptr = SetClassName(item, classname);
     else
        ptr = setResourceID(ptr, LOWORD(classname)); // MAKEINTRESOURCEW(class)
     
     if(text == 0)
        ptr = appendString(ptr, L"");
     else
     if(HIWORD(text) != 0)
        ptr = appendString(ptr, text);
     else
        ptr = setResourceID(ptr, LOWORD(text));  // MAKEINTRESOURCEW(id)

     ptr = noParms(item, ptr);

     return (LPDLGITEMTEMPLATE)ptr;
    }

/*****************************************************************************
                                     CTEXT
*****************************************************************************/

LPDLGITEMTEMPLATE CTEXT(LPDLGITEMTEMPLATE item, LPCWSTR text, short id, 
                        int x, int y, int cx, int cy, DWORD style, 
                        DWORD exstyle)
    {

     return CONTROL(item, text, id, MAKEINTRESOURCEW(0x0082),
                    SS_CENTER | style, 
                    x, y, cx, cy, 
                    exstyle);

    }

/*****************************************************************************
                                 DEFPUSHBUTTON
*****************************************************************************/

LPDLGITEMTEMPLATE DEFPUSHBUTTON(LPDLGITEMTEMPLATE item, LPCWSTR text,
                        short id, int x, int y, int cx, int cy, 
                        DWORD style, DWORD exstyle)
    {
     return CONTROL(item, text, id, MAKEINTRESOURCEW(0x0080),
                    style | BS_DEFPUSHBUTTON,
                    x, y, cx, cy, 
                    exstyle);
    }

/*****************************************************************************
                                     EDITTEXT
*****************************************************************************/

LPDLGITEMTEMPLATE EDITTEXT(LPDLGITEMTEMPLATE item, short id, 
                        int x, int y, int cx, int cy, DWORD style, 
                        DWORD exstyle,LPCWSTR defaultText)
    {

     return CONTROL(item, defaultText, id, MAKEINTRESOURCEW(0x0081),
                     style, 
                     x, y, cx, cy, 
                     exstyle);

    }

/*****************************************************************************
                                   GROUPBOX
*****************************************************************************/

LPDLGITEMTEMPLATE GROUPBOX(LPDLGITEMTEMPLATE item, LPCWSTR text,
                        short id, int x, int y, int cx, int cy, 
                        DWORD style, DWORD exstyle)
    {
     return CONTROL(item, text, id, MAKEINTRESOURCEW(0x0080),
                    style | BS_GROUPBOX,
                    x, y, cx, cy, 
                    exstyle);
    }

 

/*****************************************************************************
                                     ICON
*****************************************************************************/

LPDLGITEMTEMPLATE ICON(LPDLGITEMTEMPLATE item, LPCWSTR icon, 
                                        short id, int x, int y, int cx, int cy,
                                        DWORD style, DWORD exstyle)
    {
     return CONTROL(item, icon, id, MAKEINTRESOURCEW(0x0082), 
                        SS_ICON | style,
                        x, y, cx, cy,
                        exstyle);
    }

/*****************************************************************************
                                     LTEXT
*****************************************************************************/

LPDLGITEMTEMPLATE LTEXT(LPDLGITEMTEMPLATE item, LPCWSTR text, short id, 
                        int x, int y, int cx, int cy, DWORD style, 
                        DWORD exstyle)
    {

     return CONTROL(item, text, id,  MAKEINTRESOURCEW(0x0082),
                    SS_LEFT | style, 
                    x, y, cx, cy, 
                    exstyle);
    }

/*****************************************************************************
                                    LISTBOX
*****************************************************************************/

LPDLGITEMTEMPLATE LISTBOX(LPDLGITEMTEMPLATE item, short id, 
                        int x, int y, int cx, int cy, DWORD style, 
                        DWORD exstyle)
    {

     return CONTROL(item, L"", id, MAKEINTRESOURCEW(0x0083), 
                    style, 
                    x, y, cx, cy, 
                    exstyle);

    }

/*****************************************************************************
                                  PUSHBUTTON
*****************************************************************************/

LPDLGITEMTEMPLATE PUSHBUTTON(LPDLGITEMTEMPLATE item, LPCWSTR text,
                        short id, int x, int y, int cx, int cy, 
                        DWORD style, DWORD exstyle)
    {
     return CONTROL(item, text, id, MAKEINTRESOURCEW(0x0080),
                    style | BS_PUSHBUTTON,
                    x, y, cx, cy, 
                    exstyle);
    }

/*****************************************************************************
                                    PUSHBOX
*****************************************************************************/

LPDLGITEMTEMPLATE PUSHBOX(LPDLGITEMTEMPLATE item, LPCWSTR text,
                        short id, int x, int y, int cx, int cy, 
                        DWORD style, DWORD exstyle)
    {
     return CONTROL(item, text, id, MAKEINTRESOURCEW(0x0080),
                    style | BS_PUSHLIKE,
                    x, y, cx, cy, 
                    exstyle);
    }

/*****************************************************************************
                                  RADIOBUTTON
*****************************************************************************/

LPDLGITEMTEMPLATE RADIOBUTTON(LPDLGITEMTEMPLATE item, LPCWSTR text,
                        short id, int x, int y, int cx, int cy, 
                        DWORD style, DWORD exstyle)
    {
     return CONTROL(item, text, id, MAKEINTRESOURCEW(0x0080),
                        style | BS_RADIOBUTTON,
                        x, y, cx, cy, 
                        exstyle);
    }

/*****************************************************************************
                                     RTEXT
*****************************************************************************/
LPDLGITEMTEMPLATE RTEXT(LPDLGITEMTEMPLATE item, LPCWSTR text, short id, 
                        int x, int y, int cx, int cy, DWORD style, 
                        DWORD exstyle)
    {

     return CONTROL(item, text, id, MAKEINTRESOURCEW(0x0082),
                    SS_RIGHT | style, 
                    x, y, cx, cy, 
                    exstyle);

    }

/*****************************************************************************
                                   SCROLLBAR
*****************************************************************************/

LPDLGITEMTEMPLATE SCROLLBAR(LPDLGITEMTEMPLATE item, short id, 
                        int x, int y, int cx, int cy, DWORD style, 
                        DWORD exstyle)
    {

     return CONTROL(item, L"", id, MAKEINTRESOURCEW(0x0084), 
                    style, 
                    x, y, cx, cy, 
                    exstyle);

    }

/*****************************************************************************
                                    STATE3
*****************************************************************************/

LPDLGITEMTEMPLATE STATE3(LPDLGITEMTEMPLATE item, LPCWSTR text,
                        short id, int x, int y, int cx, int cy, 
                        DWORD style, DWORD exstyle)
    {
     return CONTROL(item, text, id, MAKEINTRESOURCEW(0x0080),
                    style | BS_3STATE,
                    x, y, cx, cy, 
                    exstyle);
    }

/****************************************************************************
*                                   DIALOG
* Inputs:
*       LPDLGITEMTEMPLATE * item: On successful return, will be a pointer
*                       to the first place a DLGITEMTEMPLATE can be set
*       UINT size: Buffer size to allocate.  If 0, a default value of
*                       4096 is used.
*       int x: Origin of top left corner
*       int y: Origin of top left corner
*       int cx: Width in dialog units
*       int cy: Height in dialog units
*       DWORD style: Style bits
*       DWORD exstyle: Extended style bits
*       LPCWSTR menu:   NULL if no menu
*                       MAKEINTRESOURCEW(menuid) if by resource id
*                       L"menuname" for string menu
*       LPCWSTR class:  NULL if no class (default dialog box class)
*                       MAKEINTRESOURCEW(atom) if atom of class given
*                       L"classname" if class name of class given
*       LPCWSTR caption:NULL if no caption
*                       L"caption" if present
*       LPCWSTR font:   NULL if no font given (DS_SETFONT will be removed
*                               from styles)
*                       L"font" if font specified (DS_SETFONT will be added)
*       int height:     Font height in points (ignored if font is NULL)
* Result: LPDLGTEMPLATE
*       A dialog template, or NULL if an error occurred
* Effect: 
*       Allocates and initializes a DLGTEMPLATE structure
* Notes:
*       The caller must call free() to release the template
*       The cdit field will be set to 0; it is the caller's responsibility
*       to increment this field each time a control is added
****************************************************************************/


LPDLGTEMPLATE DIALOG(LPDLGITEMTEMPLATE * item,
                        UINT size, int x, int y, int cx, int cy,
                        DWORD style, DWORD exstyle, LPCWSTR menu,
                        LPCWSTR class, LPCWSTR caption, LPCWSTR font, 
                        int height)
    {
     LPDLGTEMPLATE dlg;

     LPWORD ptr;

     if(size == 0)
        size = DLGTEMPLATE_WORKING_SIZE;
     dlg = (LPDLGTEMPLATE) malloc(size);
     if(dlg == NULL)
        return NULL;
     
     dlg->x = x;
     dlg->y = y;
     dlg->cx = cx;
     dlg->cy = cy;

     dlg->cdit = 0;  // no dialog items yet

     dlg->style = style;
     if(font == NULL)
        dlg->style &= ~DS_SETFONT;
     else
        dlg->style |= DS_SETFONT;

     dlg->dwExtendedStyle = exstyle;
     dlg->cdit = 0;

     ptr = (LPWORD) &dlg[1];

     if(menu == NULL)
        *ptr++ = 0;     // no menu
     else
     if(HIWORD(menu) == 0)   
        ptr = setResourceID(ptr, LOWORD(menu)); // MAKEINTRESOURCEW
     else
        ptr = appendString(ptr, menu);


     if(class == NULL)
        *ptr++ = 0;
     else
     if(HIWORD(class) == 0)   
        ptr = setResourceID(ptr, LOWORD(class)); // MAKEINTRESOURCEW
     else
        ptr = appendString(ptr, class);
 
     ptr = appendString(ptr, (caption == NULL ? L"-" : caption));

     if(font != NULL)
        { /* has font */
         *ptr++ = height;
         ptr = appendString(ptr, font);
        } /* has font */

     if(item != NULL)
	 {
        //*item = (LPDLGITEMTEMPLATE) ptr;
	    *item = ALIGN_DWORD(LPDLGITEMTEMPLATE, ptr);
	 }
     return (LPDLGTEMPLATE)dlg;
    }


/****************************************************************************
*                                   DIALOGEX
* Inputs:
*       LPDLGITEMTEMPLATEEX * item: On successful return, will be a pointer
*                       to the first place a DLGITEMTEMPLATE can be set
*       UINT size: Buffer size to allocate.  If 0, a default value of
*                       4096 is used.
*       int x: Origin of top left corner
*       int y: Origin of top left corner
*       int cx: Width in dialog units
*       int cy: Height in dialog units
*       DWORD style: Style bits
*       DWORD exstyle: Extended style bits
*       LPCWSTR menu:   NULL if no menu
*                       MAKEINTRESOURCEW(menuid) if by resource id
*                       L"menuname" for string menu
*       LPCWSTR class:  NULL if no class (default dialog box class)
*                       MAKEINTRESOURCEW(atom) if atom of class given
*                       L"classname" if class name of class given
*       LPCWSTR caption:NULL if no caption
*                       L"caption" if present
*       LPCWSTR font:   NULL if no font given (DS_SETFONT will be removed
*                               from styles)
*                       L"font" if font specified (DS_SETFONT will be added)
*       int height:     Font height in points (ignored if font is NULL)
* Result: LPDLGTEMPLATEEX
*       A extended dialog template, or NULL if an error occurred
* Effect: 
*       Allocates and initializes a DLGTEMPLATEEX structure
* Notes:
*       The caller must call free() to release the template
*       The cdit field will be set to 0; it is the caller's responsibility
*       to increment this field each time a control is added
****************************************************************************/

LPDLGTEMPLATEEX DIALOGEX(LPDLGITEMTEMPLATEEX * item,
                        UINT size, int x, int y, int cx, int cy,
                        DWORD style, DWORD exstyle, LPCWSTR menu,
                        LPCWSTR class, LPCWSTR caption, LPCWSTR font, 
                        int height)
    {
     LPDLGTEMPLATEEX dlg;

     LPWORD ptr;

     if(size == 0)
        size = DLGTEMPLATE_WORKING_SIZE;
     dlg = (LPDLGTEMPLATEEX) Hv_Malloc(size);
     if(dlg == NULL)
        return NULL;
     dlg->dlgVer = 1;
	 dlg->signature = 0xffff;
     dlg->helpID = 0;   
     dlg->x = x;
     dlg->y = y;
     dlg->cx = cx;
     dlg->cy = cy;

     dlg->cDlgItems = 0;  // no dialog items yet

     dlg->style = style;
     if(font == NULL)
        dlg->style &= ~DS_SETFONT;
     else
        dlg->style |= DS_SETFONT;

//     dlg->exStyle = style;
 
	 
     ptr = (LPWORD) &dlg[1];

     if(menu == NULL)
        *ptr++ = 0;     // no menu
     else
     if(HIWORD(menu) == 0)   
        ptr = setResourceID(ptr, LOWORD(menu)); // MAKEINTRESOURCEW
     else
        ptr = appendString(ptr, menu);


     if(class == NULL)
        *ptr++ = 0;
     else
     if(HIWORD(class) == 0)   
        ptr = setResourceID(ptr, LOWORD(class)); // MAKEINTRESOURCEW
     else
        ptr = appendString(ptr, class);
 
  //   ptr = appendString(ptr, (caption == NULL ? L"" : caption));
/*
     if(font != NULL)
        { /* has font  
         *ptr++ = height;
         ptr = appendString(ptr, font);
        } /* has font */

     if(item != NULL)
        *item = (LPDLGITEMTEMPLATEEX) ptr;

     return (LPDLGTEMPLATEEX)dlg;
    }


#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#define MAXSTR INT_MAX

//============================================================================
// Data and macro definitions to make bitfield/flag decoding easier
//============================================================================
#define BOOL int
typedef struct
{
    DWORD   flag;
    PSTR    name;
} DWORD_FLAG_DESCRIPTIONS;

#define FLAG_AS_STRING( f ) { f, #f },

#define WS_STYLE_FLAGS              \
FLAG_AS_STRING( WS_POPUP )          \
FLAG_AS_STRING( WS_CHILD )          \
FLAG_AS_STRING( WS_MINIMIZE )       \
FLAG_AS_STRING( WS_VISIBLE )        \
FLAG_AS_STRING( WS_DISABLED )       \
FLAG_AS_STRING( WS_CLIPSIBLINGS )   \
FLAG_AS_STRING( WS_CLIPCHILDREN )   \
FLAG_AS_STRING( WS_MAXIMIZE )       \
FLAG_AS_STRING( WS_BORDER )         \
FLAG_AS_STRING( WS_DLGFRAME )       \
FLAG_AS_STRING( WS_VSCROLL )        \
FLAG_AS_STRING( WS_HSCROLL )        \
FLAG_AS_STRING( WS_SYSMENU )        \
FLAG_AS_STRING( WS_THICKFRAME )     \
FLAG_AS_STRING( WS_GROUP )          \
FLAG_AS_STRING( WS_TABSTOP )        \

DWORD_FLAG_DESCRIPTIONS WndStyleFlags[] =   // Style flags common to all wnds
{
WS_STYLE_FLAGS
};

#define NUMBER_WND_STYLE_FLAGS \
    (sizeof(WndStyleFlags) / sizeof(WndStyleFlags[0]))

DWORD_FLAG_DESCRIPTIONS DlgStyleFlags[] = // All style flags used by dialogs
{
WS_STYLE_FLAGS
FLAG_AS_STRING( DS_ABSALIGN )
FLAG_AS_STRING( DS_SYSMODAL )
FLAG_AS_STRING( DS_LOCALEDIT )
FLAG_AS_STRING( DS_SETFONT )
FLAG_AS_STRING( DS_MODALFRAME )
FLAG_AS_STRING( DS_NOIDLEMSG )
FLAG_AS_STRING( DS_SETFOREGROUND )
FLAG_AS_STRING( DS_3DLOOK )
FLAG_AS_STRING( DS_FIXEDSYS )
FLAG_AS_STRING( DS_NOFAILCREATE )
FLAG_AS_STRING( DS_CONTROL )
FLAG_AS_STRING( DS_CENTER )
FLAG_AS_STRING( DS_CENTERMOUSE )
FLAG_AS_STRING( DS_CONTEXTHELP )
};

#define NUMBER_DLG_STYLE_FLAGS \
    (sizeof(DlgStyleFlags) / sizeof(DlgStyleFlags[0]))

// Helper function that takes a set of flags, and constructs a string that
// contains the names of all the flags that are set
void GetBitfieldsAsString(  DWORD dwStyle,
                            DWORD_FLAG_DESCRIPTIONS *jk, unsigned maxFlags, 
                            PSTR pszBuffer, unsigned cbLen )
{
	    BOOL fAtLeastOneFlag = FALSE;
        unsigned i = 0;

    if ( !cbLen || !pszBuffer )
        return;
        
    pszBuffer[0] = 0;   // Start it out null terminated
    
    
    for (  i = 0; i < maxFlags; i++ )
    {
        if ( dwStyle & DlgStyleFlags[i].flag )
        {
            if ( fAtLeastOneFlag )
                pszBuffer += sprintf(pszBuffer, " | %s",DlgStyleFlags[i].name);
            else
            {
                fAtLeastOneFlag = TRUE;
                pszBuffer += sprintf( pszBuffer, "%s", DlgStyleFlags[i].name );
            }
        }       
    }
}

// Helper function that reads a unicode string out of a dialog template,
// and stores the results in an ANSI (8 bit) buffer.  Returns a pointer to
// the byte immediately following the string.
PWORD GetDlgEmbeddedString( PWORD pwsz, PSTR pszBuffer, unsigned cbLen )
{
    if ( !cbLen || !pszBuffer )
        return FALSE;
        
    *pszBuffer = 0;
        
    while ( *pwsz )     // Loop through all chars in the unicode string
    {
        if ( cbLen )    // While there is room, copy to the output buffer
            *pszBuffer++ = (BYTE)*pwsz;         
        pwsz++;
    }
    
    // Null terminate the output string
    if ( cbLen )                
        *pszBuffer = 0;         // Space was left over
    else
        *(pszBuffer-1) = 0;     // Wrote to the entire buffer.  Back up and
                                // null out the last character
        
    return pwsz+1;  // return next WORD past the null terminator
}

PSTR GetDlgItemClassIdAsString( WORD classId )
{
    switch ( classId )
    {
        case 0x0080: return "Button";
        case 0x0081: return "Edit";
        case 0x0082: return "Static";
        case 0x0083: return "List box";
        case 0x0084: return "Scroll bar";
        case 0x0085: return "Combo box";
        default: return "unknown";
    }
}

// Input: a ptr.  Output: the ptr aligned to a DWORD.
//#define ALIGN_DWORD( type, p ) ( (type)(((DWORD)p+3) & ~3 ))

// Displays all the fields of a dialog control.  Returns the next address
// following the controls data.
LPDLGITEMTEMPLATE DumpDialogItem( LPDLGITEMTEMPLATE pDlgItemTemplate )
{
    char szDlgItemStyles[512];
    PWORD pClass, pTitle, pCreationData;
    char szAnsiString[256];

    // Emite "warning" if the WS_VISIBLE flag is not set 
    if ( 0 == (pDlgItemTemplate->style & WS_VISIBLE) ) {
 //       printf( "  *** Dialog item not visible ***\n" );
    }

    //
    // Start out by printing all the fixed fields in the DLGITEMTEMPLATE
    //          
    GetBitfieldsAsString(   pDlgItemTemplate->style,
                            WndStyleFlags, NUMBER_WND_STYLE_FLAGS,
                            szDlgItemStyles, sizeof(szDlgItemStyles) );

/*    printf( "  style: %X (%s)\n", pDlgItemTemplate->style, szDlgItemStyles ); 
    printf( "  dwExtendedStyle: %X\n", pDlgItemTemplate->dwExtendedStyle); 

    printf( "  Coords: (%d,%d) - (%d,%d)\n",
                pDlgItemTemplate->x, pDlgItemTemplate->y,
                pDlgItemTemplate->x + pDlgItemTemplate->cx,
                pDlgItemTemplate->y + pDlgItemTemplate->cy );
    printf( "  id: %d\n", (signed short)pDlgItemTemplate->id);*/
    
    
    // Following the fixed DLGITEMTEMPLATE, first up is the window class
    pClass = (PWORD)(pDlgItemTemplate+1);   // ptr math!
    if ( *pClass )
    {
        if ( 0xFFFF == *pClass )
        {
            pClass++;
//            printf( "  Class: %s\n", GetDlgItemClassIdAsString(*pClass++) );
        }
        else
        {
            pClass = GetDlgEmbeddedString(  pClass, szAnsiString,
                                            sizeof(szAnsiString) );
                                            
 //           printf( "  Class: %s\n", szAnsiString );
        }
    }
    else
        pClass++;

    // Following the window class array is the title array
    pTitle = pClass;
    if ( *pTitle )
    {
        if ( 0xFFFF == *pTitle )
        {
            pTitle++;
//            printf( "  Title resource ID: %u\n", *pTitle++ );
        }
        else
        {
            pTitle = GetDlgEmbeddedString(  pTitle, szAnsiString,
                                            sizeof(szAnsiString) );
                                            
 //           printf( "  Title: %s\n", szAnsiString );
        }
    }
    else
        pTitle++;

    pCreationData = pTitle;
    
    // Following the title array is the (optional) creation data.  We will not
    // show it here, because it is usually not meaningful as ASCII text
    if ( *pCreationData )
    {
        // Adjust for the creation data.  *pCreationData is in bytes, rather
        // than WORDs, so we divide by sizeof(DWORD) to make the ptr addition
        // come out correctly
        pCreationData = (PWORD)( (PBYTE)pCreationData + *pCreationData );
    }
    else
        pCreationData++;

    // Return value is next byte after the DLGITEMTEMPLATE and its
    // variable len  fields     
    return (LPDLGITEMTEMPLATE)pCreationData;
}

// Displays all the fields of a dialog template, and then calls DumpDialogItem
// to display all the controls.
BOOL DumpDialog( PVOID pDlg )
{
    LPDLGTEMPLATE pDlgTemplate = (LPDLGTEMPLATE)pDlg;
    
    char szDlgStyles[512];
    PWORD pMenu, pClass, pTitle, pPointSize, pTypeface;
    char szAnsiString[256];
    RECT rectDlg;
    unsigned i;
     LPDLGITEMTEMPLATE pDlgItemTemplate;// = (LPDLGITEMTEMPLATE)pTypeface;
     POINT ctlPt = { 0,0 };
           
    if ( HIWORD(pDlgTemplate->style) != 0xFFFF )// Is it a regular DLGTEMPLATE?
    {
        //
        // Start out by printing all the fixed fields in the DLGTEMPLATE
        //          
        GetBitfieldsAsString(   pDlgTemplate->style,
                                DlgStyleFlags, NUMBER_DLG_STYLE_FLAGS,
                                szDlgStyles, sizeof(szDlgStyles) );
/*        printf( "style: %08X (%s)\n", pDlgTemplate->style, szDlgStyles );

        printf( "extended style: %08X\n", pDlgTemplate->dwExtendedStyle );          

        printf( "controls: %u\n", pDlgTemplate->cdit );
        printf( "Coords: (%d,%d) - (%d,%d)\n",
                    pDlgTemplate->x, pDlgTemplate->y,
                    pDlgTemplate->x + pDlgTemplate->cx,
                    pDlgTemplate->y + pDlgTemplate->cy );*/

        rectDlg.left = rectDlg.top = 0;
        rectDlg.right = pDlgTemplate->cx;
        rectDlg.bottom = pDlgTemplate->cy;
        
        // Following the fixed DLGTEMPLATE, first up is the menu
        pMenu = (PWORD)(pDlgTemplate + 1);  // ptr math!
        if ( *pMenu )
        {
            if ( 0xFFFF == *pMenu )
            {
                pMenu++;
//                printf( "Menu ID: %u\n", *pMenu++ );
            }
            else
            {
                pMenu = GetDlgEmbeddedString(   pMenu, szAnsiString,
                                                sizeof(szAnsiString) );
                                                
//                printf( "Menu Name: %s\n", szAnsiString );
            }
        }
        else
            pMenu++;
        
        // Following the menu array is the "window class" array
        pClass = pMenu;
        if ( *pClass )
        {
          if ( 0xFFFF == *pClass ) {
//                printf( "Class ID: %d\n", (signed short)*pClass++ );
          }
            else
            {
                pClass = GetDlgEmbeddedString(  pClass, szAnsiString,
                                                sizeof(szAnsiString) );
                                                
//                printf( "Class Name: %s\n", szAnsiString );
            }
        }
        else
            pClass++;
                    
        // Following the window class array is the title array
        pTitle = pClass;
        if ( *pTitle )
        {
            pTitle = GetDlgEmbeddedString(  pTitle, szAnsiString,
                                            sizeof(szAnsiString) );
                                            
//            printf( "Title: %s\n", szAnsiString );
        }
        else
            pTitle++;
            
        // Following the title array is the point size and font (if DS_SETFONT
        // is set in pDlgTemplate->Style
        if ( pDlgTemplate->style & DS_SETFONT )
        {       
            pPointSize = pTitle;
            pTypeface = pPointSize+1;
                        
            pTypeface = GetDlgEmbeddedString( pTypeface, szAnsiString,
                                              sizeof(szAnsiString) );
                                            
//            printf( "Font: %u point %s\n", *pPointSize, szAnsiString );
        }
        else
            pTypeface = pTitle; 

     pDlgItemTemplate =    pTypeface;
        // Now iterate through each of the controls, calling DumpDialogItem
        for ( i=0; i < pDlgTemplate->cdit; i++ )
        {
            pDlgItemTemplate = ALIGN_DWORD(LPDLGITEMTEMPLATE, pDlgItemTemplate);

//            printf( "  ----\n" );   // Print separator for next item

            // Check that the dialog item is within the bounds of the dialog.
            // If not, emit a warning message
 //           POINT ctlPt = { pDlgItemTemplate->x, pDlgItemTemplate->y };
			 ctlPt.x = pDlgItemTemplate->x; 
			 ctlPt.y = pDlgItemTemplate->y;

       if ( !PtInRect( &rectDlg, ctlPt ) ) {
//                printf( "  *** Item not within dialog ***\n" );
       }
                
            pDlgItemTemplate = DumpDialogItem( pDlgItemTemplate );
        }
    }
    else  {  // Left as an exercise for the reader... :-)
        printf( "DLGTEMPLATEEX type not yet handled\n" );
    }
    
    return TRUE;
}


/***
*ifndef _COUNT_
*int sprintf(string, format, ...) - print formatted data to string
*else
*int _snprintf(string, cnt, format, ...) - print formatted data to string
*endif
*
*Purpose:
*       Prints formatted data to the using the format string to
*       format data and getting as many arguments as called for
*       Sets up a FILE so file i/o operations can be used, make
*       string look like a huge buffer to it, but _flsbuf will
*       refuse to flush it if it fills up.  Appends '\0' to make
*       it a true string. _output does the real work here
*
*       Allocate the 'fake' _iob[] entry statically instead of on
*       the stack so that other routines can assume that _iob[]
*       entries are in are in DGROUP and, thus, are near.
*
*ifdef _COUNT_
*       The _snprintf() flavor takes a count argument that is
*       the max number of bytes that should be written to the
*       user's buffer.
*endif
*
*       Multi-thread: (1) Since there is no stream, this routine must
*       never try to get the stream lock (i.e., there is no stream
*       lock either). (2) Also, since there is only one statically
*       allocated 'fake' iob, we must lock/unlock to prevent collisions.
*
*Entry:
*       char *string - pointer to place to put output
*ifdef _COUNT_
*       size_t count - max number of bytes to put in buffer
*endif
*       char *format - format string to control data format/number
*       of arguments followed by list of arguments, number and type
*       controlled by format string
*
*Exit:
*       returns number of characters printed
*
*Exceptions:
*
*******************************************************************************/

char printf_string[1024];
int  _output(FILE *outfile,const char *format,va_list arglist);

int __cdecl printf (
        const char *format,
        ...
        )

{

        FILE str;
         FILE *outfile = &str;
        va_list arglist;
         int retval;
        long pos = 0;
	    int i;
/*return;*/
        va_start(arglist, format);

       for(i=0;i<400;printf_string[i++]='\0');
        outfile->_flag = _IOWRT|_IOSTRG;
        outfile->_ptr = outfile->_base = printf_string;
#ifndef _COUNT_
        outfile->_cnt = MAXSTR;
#else  /* _COUNT_ */
        outfile->_cnt = count;
#endif  /* _COUNT_ */

        retval = _output(outfile,format,arglist);

//        _putc_lk('\0',outfile); /* no-lock version */

		if(!Hv_PrintfWindow)
		{ // create the listbox dialog
			createPrintfWindow();
        }

     pos = SendDlgItemMessage(Hv_PrintfWindow, Hv_STDOUTWIN, LB_ADDSTRING, 0, (LPARAM)printf_string); 
	 if(pos == LB_ERRSPACE)
	 {
	   SendDlgItemMessage(Hv_PrintfWindow, Hv_STDOUTWIN, LB_RESETCONTENT , 0, 0); 
       pos = SendDlgItemMessage(Hv_PrintfWindow, Hv_STDOUTWIN, LB_ADDSTRING, 0, (LPARAM)printf_string); 

	 }
	SendDlgItemMessage(Hv_PrintfWindow, Hv_STDOUTWIN, LB_SETTOPINDEX, (WPARAM)pos,0); 


//	 SendMessage(Hv_PrintfWindow, WM_SETREDRAW, 1, 0); 
     return(retval);
}

int __cdecl fprintf (FILE *fil,
					 const char *format,
					 ...
					 )
					 
{
	
	FILE str;
	FILE *outfile = &str;
	va_list arglist;
	int retval;
	long pos = 0;
	int i;
 	
	va_start(arglist, format);
	if(fil != stderr) {
		vfprintf (fil, format, arglist);
		return 0;
	}
	
	for(i=0;i<400;printf_string[i++]='\0');
	outfile->_flag = _IOWRT|_IOSTRG;
	outfile->_ptr = outfile->_base = printf_string;
#ifndef _COUNT_
	outfile->_cnt = MAXSTR;
#else  /* _COUNT_ */
	outfile->_cnt = count;
#endif  /* _COUNT_ */
	
	retval = _output(outfile,format,arglist);
	

	//        _putc_lk('\0',outfile); /* no-lock version */
	
	if(!Hv_PrintfWindow)
	{ // create the listbox dialog
		createPrintfWindow();
	}
	

	pos = SendDlgItemMessage(Hv_PrintfWindow, Hv_STDERRWIN, LB_ADDSTRING, 0, (LPARAM)printf_string); 
	if(pos == LB_ERRSPACE)
	{
		SendDlgItemMessage(Hv_PrintfWindow, Hv_STDERRWIN, LB_RESETCONTENT , 0, 0); 
		pos = SendDlgItemMessage(Hv_PrintfWindow, Hv_STDERRWIN, LB_ADDSTRING, 0, (LPARAM)printf_string); 
		
	}
	SendDlgItemMessage(Hv_PrintfWindow, Hv_STDERRWIN, LB_SETTOPINDEX, (WPARAM)pos,0); 
	//	 SendMessage(Hv_PrintfWindow, WM_SETREDRAW, 1, 0); 
	return(retval);
}


static void createPrintfWindow()
{
	 LPDLGTEMPLATE templ;
     LPDLGITEMTEMPLATE item;
	 DWORD styles;
	 HINSTANCE hinst = NULL;
	 RECT    rc; 
	 long pos = 0;

	 if(Hv_mainWindow== NULL)   /* trying to print befor their is a main window */
        return ;

     hinst = GetWindowInstance (Hv_mainWindow) ;

	styles = DS_CONTEXTHELP | WS_POPUP |DS_3DLOOK |
		 WS_CAPTION | WS_OVERLAPPEDWINDOW   | WS_MINIMIZE   ;


   templ = DIALOG(&item, DLGTEMPLATE_WORKING_SIZE,
      		0, 0, 200, 354,
		styles,
		0,
		NULL,		// menu
		NULL,		// class
		L"PRINTF  ",// caption
		L"System",	// font
		8);	    	// font height

   if(templ == NULL)
	 return ;
   templ->cdit++;


    item = DEFPUSHBUTTON(item, L"OK", Hv_PRINTFOK, 7, 333, 50, 14, 
      		WS_GROUP | WS_TABSTOP | WS_VISIBLE, 0);
   	templ->cdit++;
    item = LTEXT(item, L"STDOUT", 3035,
				       7, 11,
				       7*4, 8,
					   WS_VISIBLE ,0);


   templ->cdit++;
   item = LISTBOX(item, Hv_STDOUTWIN, 7, 20,186, 125, 
      		 WS_VISIBLE | WS_CLIPSIBLINGS |  
              WS_BORDER | WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_HASSTRINGS, 0);

   templ->cdit++;
   item = LTEXT(item, L"STDERR", 3036,
				   7, 147,
				   7*4, 8,
				   WS_VISIBLE ,0);
   templ->cdit++;
   item = LISTBOX(item, Hv_STDERRWIN, 7, 156,186, 125, 
      		 WS_VISIBLE | WS_CLIPSIBLINGS |  
              WS_BORDER | WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_HASSTRINGS, 0);

   Hv_PrintfWindow = 
	 CreateDialogIndirect (hinst,templ,Hv_mainWindow,(DLGPROC) printfDlgProc);
   SendDlgItemMessage(Hv_PrintfWindow, Hv_STDOUTWIN, WM_SETFONT,(WPARAM) Hv_fonts[Hv_fixed12], 0);
   SendDlgItemMessage(Hv_PrintfWindow, Hv_STDERRWIN, WM_SETFONT,(WPARAM) Hv_fonts[Hv_fixed12], 0);
 
   GetWindowRect (Hv_mainWindow, &rc);
  ShowWindow(  Hv_PrintfWindow,  SW_MINIMIZE);
  ShowWindow(  Hv_PrintfWindow,  SW_SHOWNA);
  ShowWindow(  Hv_PrintfWindow,  SW_SHOW);

  Hv_Free(templ);

}
/***************************** Private Functions *****************************/ 
 
 
// 
// Create a dynamically allocated Unicode copy of an ANSI string. 
// 
HRESULT ANSIToUnicode(LPCSTR pcszANSI, LPWSTR *ppwszUnicode) 
{ 
  HRESULT hr = E_UNEXPECTED; 
  int ncchANSI; 
  int ncchUnicode; 
  int result;
  LPWSTR  wstr;

  // (+ 1) for null terminator. 
  ncchANSI = lstrlen(pcszANSI) + 1; 
  
  ncchUnicode = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pcszANSI, ncchANSI, NULL, 0); 
  
  if (ncchUnicode > 0) 
    { 
      wstr = (LPWSTR ) Hv_Malloc(ncchUnicode* sizeof(WCHAR)); 
      
      if (ppwszUnicode) 
	  { 
	    result = MultiByteToWideChar(CP_ACP, 0 /*MB_PRECOMPOSED*/, pcszANSI, ncchANSI, wstr, ncchUnicode);
	    if (result == ncchUnicode) 
            hr = S_OK; 
	    else 
		{ 
	      Hv_Free(wstr); 
	      ppwszUnicode = NULL; 
	    } 
	  } 
      else 
	    hr = E_OUTOFMEMORY; 
    } 
   else 
     ppwszUnicode = NULL; 
  *ppwszUnicode = wstr;
  return(hr); 
} 


/**
 * Hv_FillColorTable
 * @purpose   Put the Hv colors into a color table
 * @local
 */
 
static void Hv_FillColorTable() {
	RGBQUAD  *ct = NULL;
	int      j;
	BYTE     g;

/* create space for the Hv colors */

  Hv_colorTable = (RGBQUAD *)Hv_Malloc(Hv_numColors*sizeof(RGBQUAD));
  


  for (j = 0; j < 15; j++) {
    g = 220 - j*14;
    Hv_StuffColor(g, g, g, Hv_gray15 + j);
  }

  Hv_StuffColor(255, 0, 0,     Hv_red);
  Hv_StuffColor(255, 128, 0,   Hv_orange);
  Hv_StuffColor(255, 255, 0,   Hv_yellow);
/*  Hv_StuffColor(165, 42, 42,   Hv_brown);
  Hv_StuffColor(219, 147, 112, Hv_wheat); */
  Hv_StuffColor(147, 73, 0,   Hv_brown);
  Hv_StuffColor(241, 191, 116, Hv_wheat);
  Hv_StuffColor(0,   100, 0,   Hv_forestGreen);
  Hv_StuffColor(64, 219, 64,   Hv_green);  
  Hv_StuffColor(255, 255, 255, Hv_white);
  Hv_StuffColor(0, 0, 0,       Hv_black);
  Hv_StuffColor(160, 32, 240,  Hv_purple);
  Hv_StuffColor(238, 130, 238, Hv_violet);
  Hv_StuffColor(216, 216, 255, Hv_aliceBlue);
  Hv_StuffColor(150, 195, 200, Hv_honeydew);
  Hv_StuffColor(125, 196, 215, Hv_skyBlue);
  Hv_StuffColor(100, 116, 215, Hv_cornFlowerBlue);
  Hv_StuffColor(0, 0, 255,     Hv_blue);
  Hv_StuffColor(0, 0, 64,      Hv_navyBlue);
  Hv_StuffColor(70, 235, 150,  Hv_aquaMarine);
  Hv_StuffColor(83, 142, 87,   Hv_lightSeaGreen); /* used to hold bg color*/
 

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

  Hv_canvasColor = Hv_cadetBlue;
}


static void    Hv_StuffColor(BYTE r,
			     BYTE g,
			     BYTE b,
			     int i) {

	RGBQUAD   *rgb;

	if ((i < 0) || (i > Hv_numColors))
			return;

	rgb = Hv_colorTable + i;

	rgb->rgbRed   = r;
	rgb->rgbGreen = g;
	rgb->rgbBlue  = b;

	Hv_colors[i] = PALETTERGB(rgb->rgbRed, rgb->rgbGreen, rgb->rgbBlue);
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

  BYTE        Ro, Go, Bo;
  BYTE        Rf, Gf, Bf;

  short      stindex;
  RGBQUAD    color1, color2;
  RGBQUAD    *rgb;

  int Rdel, Gdel, Bdel, i;
  int    steps, sp1;

  if (start < 0) {
      steps = (int)(cEnd - cStart - 1);
      stindex = cStart;
  }
  else {
      steps  = (int)nstep;
      stindex = start - 1;
  }

  sp1 = steps + 1;
  color1 = Hv_colorTable[cStart];
  color2 = Hv_colorTable[cEnd];

  Ro = color1.rgbRed;  Go = color1.rgbGreen;  Bo = color1.rgbBlue;
  Rf = color2.rgbRed;  Gf = color2.rgbGreen;  Bf = color2.rgbBlue;

  Rdel = (int)(Rf - Ro);
  Gdel = (int)(Gf - Go);
  Bdel = (int)(Bf - Bo);

  for (i = 1; i <= steps; i++) {
	rgb = Hv_colorTable + cStart + i;
	rgb->rgbRed   = Ro + (BYTE)((i*Rdel)/sp1);
    rgb->rgbGreen = Go + (BYTE)((i*Gdel)/sp1);
    rgb->rgbBlue  = Bo + (BYTE)((i*Bdel)/sp1);
	Hv_colors[cStart + i] = PALETTERGB(rgb->rgbRed, rgb->rgbGreen, rgb->rgbBlue);

  }
}


/**
 * Hv_CreateLogicalPalette
 */

static LOGPALETTE *Hv_CreateLogicalPalette() {

	DWORD        dwSize;
	LOGPALETTE   *plp = NULL;
	int          i;

	dwSize = sizeof(LOGPALETTE) + ((Hv_numColors-1) * sizeof(PALETTEENTRY));


	if ((plp = (LOGPALETTE *) HeapAlloc (GetProcessHeap(), 0, dwSize)) == NULL) {
		HeapFree(GetProcessHeap(), 0, Hv_colorTable);
		return NULL;
	}

	plp->palVersion = 0x300;
	plp->palNumEntries = (WORD)Hv_numColors;

	for (i = 0; i < Hv_numColors; i++) {
		plp->palPalEntry[i].peRed   = Hv_colorTable[i].rgbRed;
		plp->palPalEntry[i].peGreen = Hv_colorTable[i].rgbGreen;
		plp->palPalEntry[i].peBlue  = Hv_colorTable[i].rgbBlue;
		plp->palPalEntry[i].peFlags = 0;
	}


	return plp;
}

/**
 * Hv_ChangeWindowBackground
 */

static void Hv_ChangeWindowBackground(short color) {

	HBRUSH oldbrush = 0;
	oldbrush = (HBRUSH)GetClassLong(Hv_mainWindow, GCL_HBRBACKGROUND);

	if (logo == 0) {
		SetClassLong(Hv_mainWindow,
					GCL_HBRBACKGROUND,
					(LONG)CreateSolidBrush(Hv_pixelColor(color)));
	}
	else { // paint with a logo
		SetClassLong(Hv_mainWindow,
					GCL_HBRBACKGROUND,
					(LONG)CreatePatternBrush(logo));
	
	}

	if (oldbrush != 0)
			DeleteObject(oldbrush);

//	InvalidateRect(Hv_mainWindow, NULL, True);

}

/**
 * Hv_ColorTableToPalette
 */

static	void Hv_ColorTableToPalette() {

    LOGPALETTE   *plp;


/*
 * Step 1, Create a logical palette from the color table
 */

	plp = Hv_CreateLogicalPalette();
	if (plp == NULL)
			return;


/*
 * Step 2, Create the palette
 */

	if (Hv_colorMap != 0)
		DeletePalette(Hv_colorMap);

	Hv_colorMap = CreatePalette(plp);
	HeapFree(GetProcessHeap(), 0, plp);

}


/**
 * Hv_XYToW32Rect
 * @purpose Converts  x and y into a small Win32 RECT
 * @param x      x coordinate
 * @param y      y coordinate
 * @param w32r   Pointer to Win32 
 * @local
 */

	static void Hv_XYToW32Rect(short x,
		short y,
		RECT *w32r) {

		w32r->left   = x;
		w32r->top    = y;
		w32r->right  = x + 1;
		w32r->bottom = y + 1;
	}

/**
 * Hv_PointToW32Rect
 * @purpose Converts an Hv_Point into a small Win32 RECT
 * @param xp    Pointer to Hv_Point
 * @param w32r   Pointer to Win32 
 * @local
 */

	static void Hv_PointToW32Rect(Hv_Point *xp,
		RECT *w32r) {
	    Hv_XYToW32Rect(xp->x, xp->y, w32r);
	}

/**
 * Hv_RectangleToW32Rect
 * @purpose Converts an Hv_Rectangle into a Win32 RECT
 * @param hvr     Pointer to Hv_Rectangle
 * @param w32r    Pointer to Win32 
 * @local
 */

	static void Hv_RectangleToW32Rect(Hv_Rectangle *hvr,
		RECT *w32r) {

		w32r->left   = hvr->x;
		w32r->top    = hvr->y;
		w32r->right  = hvr->x + hvr->width;
		w32r->bottom = hvr->y + hvr->height;
	}

/**
 * Hv_RectToW32Rect
 * @purpose Converts an Hv_Rect into a Win32 RECT
 * @param hvr     Pointer to Hv_Rect
 * @param w32r    Pointer to Win32 
 * @local
 */

	static void Hv_RectToW32Rect(Hv_Rect *hvr,
		RECT *w32r) {

		w32r->left   = hvr->left;
		w32r->top    = hvr->top;
		w32r->right  = hvr->right;
		w32r->bottom = hvr->bottom;
	}
	
/**
 * Hv_W32RectToRect
 * @purpose Converts a Win32 RECT into an Hv_Rect
 * @param w32r    Pointer to Win32 
 * @param hvr     Pointer to Hv_Rect
 * @local
 */

	static void Hv_W32RectToRect(RECT *w32r,
		Hv_Rect *hvr) {

		hvr->left   = (short)(w32r->left);
		hvr->top    = (short)(w32r->top);
		hvr->right  = (short)(w32r->right);
		hvr->bottom = (short)(w32r->bottom);
		Hv_FixRectWH(hvr);
	}

/**
 * Hv_SetMainHDC
 * @purpose Does nothing if global hdc (Hv_gc) is set, otherwise gets it from the
 * main window.
 * @local
 */


void Hv_SetMainHDC() {

	if (Hv_drawingOffScreen) {
		Hv_gc = hdcmem;
		return;
	}

	if ((Hv_gc == (HANDLE)(-1)) || (Hv_gc == 0) || (Hv_gc == hdcmem)) {
		if (Hv_mainWindow == Hv_trueMainWindow)
			Hv_gc = GetDC(Hv_mainWindow);
	}


}


/**
 *  Hv_PolyToPOINT
 *  @purpose  Convert an Hv_Point poly to a Win32 POINT array
 *  @param  xp    The Hv poly
 *  @param  n     The number of points
 *  @param  The Win32 POINT array
 */

static POINT *Hv_PolyToPOINT(Hv_Point  *xp,
					  int       n) {

	POINT  *lp;
	int    i;

	if ((xp == NULL) || (n < 1))
		return NULL;

	lp = (POINT *)Hv_Malloc(n*sizeof(POINT));

	for (i = 0; i < n; i++) {
		lp[i].x = xp[i].x;
		lp[i].y = xp[i].y;
	}

	return lp;

}

/**
 * Hv_SetBkColor
 * @purpose Set the background color
 * @param color  The color index
 * @local
 */

static void Hv_SetBkColor(short color) {
	SetBkColor(Hv_gc, Hv_pixelColor(color));
}

/**
 * Hv_SetTextColor
 * @purpose Set the text color
 * @param color  The color index
 * @local
 */

static void Hv_SetTextColor(short color) {

	if ((color < 0) || (color >= Hv_numColors))
		color = Hv_black;

	SetTextColor(Hv_gc, Hv_pixelColor(color));

}



/**
 * Hv_setBkMode
 * @purpose Set the background mode
 * @param mode  The background mode
 * @local
 */

static void Hv_SetBkMode(int mode) {
	Hv_SetMainHDC();
	SetBkMode(Hv_gc, TRANSPARENT); 
}

/**
 * Hv_SetFont
 * @purpose Set the font
 * @param font The font index
 * @local
 */

static void Hv_SetFont(short font) {

	if (Hv_fonts == NULL)
		return;

	Hv_SetMainHDC();

	if ((font < 0) || (font >= Hv_NUMFONTS))
		font = Hv_fixed2;

	SelectObject(Hv_gc, Hv_fonts[font]);

}

/**
 * Hv_SetPenWidth
 * @purpose Set the pen width
 * @param width  The width in pixels
 * @local
 */

static void Hv_SetPenWidth(int width) {

	HPEN      hpen;
	LOGPEN    logpen;

	width = Hv_iMax(1, Hv_iMin(12, width));

	Hv_SetMainHDC();
	hpen = GetCurrentObject(Hv_gc, OBJ_PEN);
	GetObject(hpen, sizeof(LOGPEN), &logpen);

	logpen.lopnWidth.x = width;

	DeleteObject(SelectObject(Hv_gc, CreatePenIndirect(&logpen)));

}

/**
 * Hv_SetPenColor
 * @purpose Set the pen color
 * @param color  The color index
 * @local
 */

static void Hv_SetPenColor(short color) {

	HPEN      hpen;
	LOGPEN    logpen;

	Hv_SetMainHDC();
	hpen = GetCurrentObject(Hv_gc, OBJ_PEN);
	GetObject(hpen, sizeof(LOGPEN), &logpen);

	logpen.lopnColor = Hv_pixelColor(color);

	DeleteObject(SelectObject(Hv_gc, CreatePenIndirect(&logpen)));
//	DeleteObject(&logpen);

}

static HFONT Hv_NewFont(int size,
					int weight,
					int family,
					char * name) {
	HFONT hfont;
	hfont = CreateFont(-size, // logical height of font
			0,
			0,
			0,
			weight,
			0,
			0,
			0,       // strikeout attribute flag
			ANSI_CHARSET, // character set identifier
			0,       // output precision
			0,       // clipping precision
			0,       // output quality
			family,  //pitch and family
			name     // pointer to typeface name string
			);

	return hfont;
}

/**
 * Hv_StuffXEvent
 */

void Hv_StuffXEvent(Hv_XEvent  *xevent,
					int        x,
					int        y,
					int        button,
					int        modifiers,
					int        type) {
	if (xevent == NULL)
		return;

	xevent->x = x;
	xevent->y = y;
	xevent->button = button;
	xevent->modifiers = modifiers;
	xevent->type = type;

}

void Hv_GetClientPosition(POINT *pp) {

	DWORD          pos;

	pos = GetMessagePos();

	pp->x = GET_X_LPARAM(pos);
	pp->y = GET_Y_LPARAM(pos);

	ScreenToClient(Hv_mainWindow, pp);
}

/**
 * TimerProc
 */
int timercallbackdebugflagfordavid = 1;


static void CALLBACK TimerProc (HWND  hwnd,
						        UINT  message,
						        UINT  iTimerID,
						        DWORD dtime) {
	hvTimer *timeStruct;
    Hv_LinkList    *startList   = timerList->next; // first data entry in the list
    Hv_LinkList    *endList     = timerList->prev; // last data entry in the list

    Hv_LinkList    *saveList    = timerList->next;
    Hv_LinkList    *tempList    = timerList->next;
    int            endoflist = 0;
	static int inTimerProc = 0;
    static int entry = 0;

    entry++;
	if(inTimerProc == 1)
	{
		printf("Trouble in paradise");
        inTimerProc = 0;
		return;
	}

    inTimerProc = 1;
	if(timerList->next == NULL  || timerList->next == timerList)
	{
		inTimerProc = 0;
		return;  /* empty list so return */
	}



    while(tempList->value <= (unsigned long) dtime) // find all the events to expire
	{

	  if(tempList == endList) /* scaned the whole list so backup to the end of the list */
	  {
		  endoflist = 1;
		  endList->next = startList;
		  break;
	  }
	  tempList = tempList->next;
	}

	if( tempList == timerList->next) // nothing to do (start of list)

	{
		inTimerProc = 0;
		return;
	}

	/* fix up the timer list before we call any callback because the callbacks */
	/* will add new timers to the timer list */

	if(endoflist == 1) // this meas that we will delete the whole list
	{

	  timerList->next = NULL;  // reset to a empty list
  	  timerList->prev = NULL;
	}
	else
	{
	  endList = tempList->prev; // setup endlist to point to the end of the list that will beb expired
	  timerList->next = tempList ;       // fixup the head of the list to point to the new staring point
	  timerList->next->prev = timerList;

	}


	while(startList != endList)
	{
      tempList = startList->next;
	  timeStruct = (hvTimer *) (startList->data);
	  if ((timeStruct != NULL) && (timeStruct->callback != NULL))
	  {
	    timeStruct->callback((Hv_Item)(timeStruct->data),timeStruct->timoutTime);
	  }
	  Hv_Free(startList->data);
	  Hv_Free(startList);
      startList = tempList;
	}

	timeStruct = (hvTimer *) (startList->data);
	if ((timeStruct != NULL) && (timeStruct->callback != NULL))
	{
	    timeStruct->callback((Hv_Item)(timeStruct->data),
				timeStruct->id);
	}

	Hv_Free(startList->data);
    Hv_Free(startList);
    inTimerProc = 0;
	/*
	if ((cd == NULL) || (cd->callback == NULL))
		return;
	timeStruct->timoutTime = expireTime;
	timeStruct->callback   = callback;
	timeStruct->data       = data;

	cd->callback((Hv_Item)(cd->parent), cd->userId);
	*/
}


/**
 *  Hv_RadiusRectIntersection
 *
 * theta should be 0 to 360
 * thetacrit should be 0 to 90
 */

static void Hv_RadiusRectIntersection(int   left,
									  int   top,
									  int   right,
									  int   bottom,
									  float theta,
									  float thetacrit,
									  int   *xint,
									  int   *yint) {

	float theta1, theta2, theta3, theta4, tantheta;
    int   xc, yc, width, height;

	width = right - left;
	height = bottom - top;

	while (theta < -1.0e-8)
		theta += 360.0;

	while (theta > 360.0)
		theta -= 360.0;

	tantheta = (float)(tan(Hv_DEGTORAD*theta));

	xc = (left + right)/2;
	yc = (top  + bottom)/2;

	theta1 = thetacrit;
	theta2 = (float)(180.0 - thetacrit);
	theta3 = (float)(180.0 + thetacrit);
	theta4 = (float)(360.0 - thetacrit);

	if ((theta <= theta1) || (theta > theta4)) { //isetcs right
		*xint = right;
		*yint = yc + (int)(width*tantheta/2.0);
	}
	else if (theta <= theta2) { // isects top
		*yint = top;
		*xint = xc + (int)(height/(2.0*tantheta));
	}
	else if (theta <= theta3) { // isects left
		*xint = left;
		*yint = yc - (int)(width*tantheta/2.0);
	}
	else { //isects bottom
		*yint = bottom;
		*xint = xc - (int)(height/(2.0*tantheta));
	}


}

/**
 * Hv_W32DrawBackgroundItems
 * @purpose Draw hotrect items that have their Hv_INBACKGROUND draw control bit set.
 * @param items    List of items (probably ia view's item list).
 * @param region   Clipping region
 * @local 
 */


static void Hv_W32DrawBackgroundItems(Hv_ItemList items,
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
	  Hv_W32DrawBackgroundItems(temp->children, region);
      }

/* off screen user drawing? */

  if (!drawingchildren) {
    if (View->offscreenuserdraw != NULL)
      View->offscreenuserdraw(View, hrrgn);
    
    Hv_drawingOffScreen = False;
    Hv_gc = (HANDLE)(-1);
    Hv_DestroyRegion(hrrgn);
    Hv_DestroyRegion(hrbrgn);
  }
}

/**
 * Hv_KeepInRange
 * @purpose  Keep an int value in range
 * @param   v   The value to keep in range
 * @local
 */

#define  MAXPIXVAL  32765

static short  Hv_KeepInRange(int v) {

	return (short)Hv_iMax(-MAXPIXVAL, Hv_iMin(MAXPIXVAL, v));
 
}

#undef MAXPIXVAL

/**
 * Hv_FillW32Rectangle
 * @purpose Convenience routine for filling a rectangle
 * @param   w32rect   The rectangle to fill
 * @param   color     The fill color
 * @local   
 */

static void Hv_FillW32Rectangle(RECT *w32rect,
				short color) {

  if ((color < 0) || (currentPattern == Hv_HOLLOWPAT))
    return;

  
  if (
	  (currentPattern >= Hv_80PERCENTPAT) &&
	  (currentPattern <= Hv_VSTRIPEPAT)) {
   DrawHatchRect((short)(w32rect->left),
	             (short)(w32rect->top),
	             (short)(w32rect->right),
	             (short)(w32rect->bottom),
				 currentPattern,
				 color);
    Hv_HighlightOff();
	return;
  }

  Hv_SetBrush(currentPattern, color);
  Hv_SetPenColor(color);



  
  Rectangle(Hv_gc,
	    w32rect->left,
	    w32rect->top,
	    w32rect->right,
	    w32rect->bottom);
   
 

  Hv_HighlightOff();
}


/**
 * DrawHatchRect
 * @purpose  Used to manually draw hatch because of propblem that
 *           patterns are not "hollow" in off screen drawing.
 * @param   l      Left edge.
 * @param   t      Top edge.
 * @param   r      Right edge.
 * @param   b      Bottom edge.
 * @param   pat    The pattern
 * @param   color  The color
 * @local
 */

  static void DrawHatchRect(short l,
	             short t,
	             short r,
	             short b,
				 short pat,
				 short color) {

	  short np = 0;
	  int i, np2;
	  short x1, x2, y1, y2;
	  short xx1, xx2, yy1, yy2;
	  short w, h, gap;
	  Hv_Segment *seg = NULL;


	  w = r - l;
	  h = b - t;

	  if ((w < 2) || (h < 2))
		  return;



	  switch (pat) {
	  case Hv_80PERCENTPAT: case Hv_50PERCENTPAT:
          gap = 4;
		  if (pat == Hv_50PERCENTPAT)
			  gap = 6;

		  np = 2*((w+h)/gap);
		  np2 = np/2;
          seg = (Hv_Segment *)(Hv_Malloc(np*sizeof(Hv_Segment)));

		  y1 = t;
		  y2 = b;
          x1 = l;
		  while ((x1 % gap) != 0)
			  x1++;

		  for (i = 0; i < np2; i++) {

			  xx1 = x1;
			  xx2 = x1 - h;
			  yy1 = y1;
			  yy2 = y2;

			  if (xx1 > r) {
				  yy1 = yy1 + (xx1-r);
				  xx1 = r;
			  }

			  if (xx2 < l) {
				  yy2 = y2 - (l-xx2);
				  xx2 = l;
			  }

			  Hv_SetSegment(seg+i, xx1, yy1, xx2, yy2);
			  x1 += gap;
		  }

		  x2 = r + h;
 		  while ((x2 % gap) != 0)
			  x2--;

		  for (i = np2; i < np; i++) {
			  xx2 = x2;
			  xx1 = x2 - h;
			  yy1 = y1;
			  yy2 = y2;

			  if (xx2 > r) {
				  yy2 = y2 - (xx2 - r);
				  xx2 = r;
			  }

			  if (xx1 < l) {
				  yy1 = yy1 + (l - xx1);
				  xx1 = l;
			  }

			  Hv_SetSegment(seg+i, xx1, yy1, xx2, yy2);
			  x2 -= gap; 
		  } 

		  break;

	  
	  case Hv_20PERCENTPAT: case Hv_HATCHPAT:
		  gap = 8;
		  if (pat == Hv_HATCHPAT)
			  gap = 4;
		  np = (w+h)/gap;
		  np2 = w/gap;
          seg = (Hv_Segment *)(Hv_Malloc(np*sizeof(Hv_Segment)));

		  y1 = t;
		  y2 = b;
		  x1 = l + gap/2;
		  for (i = 0; i < np2; i++) {
			  Hv_SetSegment(seg+i, x1, y1, x1, y2);
			  x1 += gap;
		  }

		  x1 = l;
		  x2 = r;
		  y1 = t + gap/2;
		  for (i = np2; i < np; i++) {
			  Hv_SetSegment(seg+i, x1, y1, x2, y1);
			  y1 += gap;
		  }


		  break;
	  
	  
	  case Hv_DIAGONAL1PAT:
          gap = 6;

		  np = (w+h)/gap;
          seg = (Hv_Segment *)(Hv_Malloc(np*sizeof(Hv_Segment)));

		  y1 = t;
		  y2 = b;
          x1 = l;

		  for (i = 0; i < np; i++) {

			  xx1 = x1;
			  xx2 = x1 - h;
			  yy1 = y1;
			  yy2 = y2;

			  if (xx1 > r) {
				  yy1 = yy1 + (xx1-r);
				  xx1 = r;
			  }

			  if (xx2 < l) {
				  yy2 = y2 - (l-xx2);
				  xx2 = l;
			  }

			  Hv_SetSegment(seg+i, xx1, yy1, xx2, yy2);
			  x1 += gap;
		  }


		  break;
	  
	  case Hv_DIAGONAL2PAT:
	      gap = 6;

		  np = (w+h)/gap;
          seg = (Hv_Segment *)(Hv_Malloc(np*sizeof(Hv_Segment)));

		  y1 = t;
		  y2 = b;
          x2 = r + h;

		  for (i = 0; i < np; i++) {
			  xx2 = x2;
			  xx1 = x2 - h;
			  yy1 = y1;
			  yy2 = y2;

			  if (xx2 > r) {
				  yy2 = y2 - (xx2 - r);
				  xx2 = r;
			  }

			  if (xx1 < l) {
				  yy1 = yy1 + (l - xx1);
				  xx1 = l;
			  }

			  Hv_SetSegment(seg+i, xx1, yy1, xx2, yy2);
			  x2 -= gap; 
		  } 

		  break;
	  
	  case Hv_HSTRIPEPAT:
		  gap = 6;
		  np = h/gap;
          seg = (Hv_Segment *)(Hv_Malloc(np*sizeof(Hv_Segment)));

		  x1 = l;
		  x2 = r;
		  y1 = t + gap/2;
		  for (i = 0; i < np; i++) {
			  Hv_SetSegment(seg+i, x1, y1, x2, y1);
			  y1 += gap;
		  }

		  break;
	  
	  case Hv_VSTRIPEPAT:
		  gap = 6;
		  np = w/gap;
         seg = (Hv_Segment *)(Hv_Malloc(np*sizeof(Hv_Segment)));

		  y1 = t;
		  y2 = b;
		  x1 = l + gap/2;
		  for (i = 0; i < np; i++) {
			  Hv_SetSegment(seg+i, x1, y1, x1, y2);
			  x1 += gap;
		  }

		  break;
	  }

	  if ((np > 0) && (seg != NULL)) {
		  Hv_W32DrawSegments(seg, np, color);
		  Hv_Free(seg);
	  }

  }


/**
 * Hv_GoodColor
 * @purpose  See if a color is in range.
 * @return   True if color is in range.
 */

Boolean Hv_GoodColor(short color) {
	return ((color >= 0) && (color < Hv_numColors));
}

/**
 * Hv_SetToNearestColor
 */

static void Hv_SetToNearestColor(RGBQUAD *rgb) {
	int min = 0;
	int i;
	int diff = 32767;
	int del;
	RGBQUAD *ct;

	for (i = 0; i < Hv_numColors; i++) {
		ct = Hv_colorTable + i;
		del = abs(ct->rgbRed - rgb->rgbRed) +
			abs(ct->rgbGreen - rgb->rgbGreen) +
			abs(ct->rgbBlue - rgb->rgbBlue);
		if (del < diff) {
			diff = del;
			min = i;
		}

	}

	ct = Hv_colorTable + min;

	rgb->rgbRed = ct->rgbRed;
	rgb->rgbBlue = ct->rgbBlue;
	rgb->rgbGreen = ct->rgbGreen;
	rgb->rgbReserved = ct->rgbReserved;
	

}

static unsigned long Hv_pixelColor(short color) {
	if (Hv_GoodColor(color))
		return Hv_colors[color];
	else if (color == Hv_highlight)
		return highlightPixel;
	else
		return Hv_colors[Hv_black];

}


static void Hv_HighlightOff() {
	if (HighlightMode) {
		Hv_SetFunction(Hv_GXCOPY);     /* xor's the GC's drawing function */
		HighlightMode = False;
	}

}


#undef    MENUBARITEM
#undef    MENU
#undef    MENUITEM
#undef    TOGGLEMENUITEM
#undef    SUBMENUITEM
#undef    Hv_STDOUTWIN  
#undef    Hv_STDERRWIN  
#undef    Hv_PRINTFOK   

#endif
