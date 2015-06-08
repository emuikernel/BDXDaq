/*==============================================
 * OPEN GL RELATED SECTION OF GATEWAY
 *==============================================*/
#ifdef WIN32
#ifdef Hv_USEOPENGL

#include "Hv.h"
#include "Hv_gateway_w32.h"
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


static void Hv_OGLInitCB(Hv_Widget w,
			 Hv_Pointer client_data,
			 Hv_Pointer call_data);

static void Hv_OGLInputCB(Hv_Widget w,
			  Hv_Pointer client_data, 
			  void  *call_data);

/*****************************************************************************
 ____       _                  ____ _ __        ___     _            _   
/ ___|  ___| |_ _   _ _ __    / ___| |\ \      / (_) __| | __ _  ___| |_ 
\___ \ / _ \ __| | | | '_ \  | |  _| | \ \ /\ / /| |/ _` |/ _` |/ _ \ __|
 ___) |  __/ |_| |_| | |_) | | |_| | |__\ V  V / | | (_| | (_| |  __/ |_ 
|____/ \___|\__|\__,_| .__/___\____|_____\_/\_/  |_|\__,_|\__, |\___|\__|
                     |_| |_____|                          |___/          
*****************************************************************************/
ATOM opengl_winclass;

/*

 *
 *  void openglFrame_OnDestroy (HWND hwnd)
 *
 *  hwnd            Handle of window to which this message applies
 *
 *  PURPOSE:        Notification that the specified window is being destroyed.
 *                  The window is no longer visible to the user.
 *
 *  COMMENTS:
 *
 */

static void openglFrame_OnDestroy (HWND hwnd) {

	Hv_View View;
  for (View = Hv_views.first;  View != NULL;  View = View->next)
	{
		if (View->hwnd == hwnd)
		{
			View->hwnd = 0;
			if(View->glxContext != NULL)
			{

				if(View->CleanUp3d != NULL)
					View->CleanUp3d(View);
			}
			if(View->hdc != NULL)
			{
		      ReleaseDC(hwnd,View->hdc);
              View->hdc = NULL;
			}
		}
	}
	return 1;
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
 static Hv_openglCallData CallData;
 static Hv_openglEvent    openglEvent;


static LRESULT CALLBACK opengl_mainFrameWndProc (HWND hwnd,
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

    Hv_View View = NULL;

	switch (message) {

	case WM_CREATE:
      CallData.event = &openglEvent;  // setup for later use
/* add the standard menus here */

//	  Hv_mainWindow = (PVOID)hwnd;
//	  Hv_InsertStartupMenus();

// get the memory DC for offscreen drawing

//	  hdc = GetDC(hwnd);
//	  hdcmem = CreateCompatibleDC(hdc);
//	  ReleaseDC(hwnd, hdc);

		return 0;

	case WM_TIMER:
	//	TimerProc(hwnd,wParam,wParam,lParam);
		return TRUE;
	case WM_COMMAND:  return 0;                  /* Notification from menu or control */
	  return 1;// HANDLE_WM_COMMAND (hwnd, wParam, lParam, mainFrame_OnCommand) ;

	case WM_DESTROY: /* Window is being destroyed */
	  return HANDLE_WM_DESTROY (hwnd, wParam, lParam, openglFrame_OnDestroy) ;

	case WM_KEYUP:
			Hv_GetClientPosition(&clp);
		if ((wParam >= VK_F1) && (wParam < VK_F10))
			Hv_HandleFunctionKey(clp.x,
			clp.y,
			wParam - VK_F1 + 1);
		return 0;
	  
	case WM_LBUTTONDBLCLK: 
//			Hv_lastClickType = 2;
//			Hv_Information("Double click!");
			return 0;
			
	case WM_LBUTTONDOWN:
			openglEvent.xbutton.button = 1;
		    openglEvent.type = ButtonPress;
//	        Hv_lastClickType = 1;

			Hv_GetClientPosition(&clp);
			  openglEvent.xmotion.x = clp.x;
			  openglEvent.xmotion.y = clp.y;
			  openglEvent.x = clp.x;
			  openglEvent.y = clp.y;

			  for (View = Hv_views.first;  View != NULL;  View = View->next)
				{
					if (View->hwnd == hwnd)
					{
					  if(View->InputRoutine3d != 0)
						 View->InputRoutine3d(hwnd,View,&CallData);
					}
				}
			return 0; // pass this event on to the dialog handlers
			
	case WM_MBUTTONDOWN: 
	         openglEvent.xbutton.button = 3;
	         Hv_GetClientPosition(&clp);
			 openglEvent.xmotion.x = clp.x;
			 openglEvent.xmotion.y = clp.y;

	  	     openglEvent.x = clp.x;
			 openglEvent.y = clp.y;

	  		 for (View = Hv_views.first;  View != NULL;  View = View->next)
				{
					if (View->hwnd == hwnd)
					{
					  if(View->InputRoutine3d != 0)
						 View->InputRoutine3d(hwnd,View,&CallData);
					}
				}

//	  Hv_lastClickType = 1;
	  Hv_Information("Middle Button Click!");
	  return 0;

	case WM_RBUTTONDOWN: 
	 Hv_GetClientPosition(&clp);
	 openglEvent.xmotion.x = clp.x;
	 openglEvent.xmotion.y = clp.y;

	 openglEvent.x = clp.x;
	 openglEvent.y = clp.y;

	  openglEvent.xbutton.button = 2;
	  if ((wParam & MK_SHIFT) == MK_SHIFT)  // simulate the 3rd button
	  {
	    openglEvent.xbutton.button = 3;
        HandlePopupMenus(clp.x, clp.y, &clp,hwnd);
		break;
	  }

	  openglEvent.type = ButtonPress;
//	  Hv_lastClickType = 1;
	  pos = GetMessagePos();
	  Hv_GetClientPosition(&clp);
//	  openglEvent.x = clp.x;
//	  openglEvent.y = clp.y;

	  openglEvent.xmotion.x = clp.x;
	  openglEvent.xmotion.y = clp.y;
	  for (View = Hv_views.first;  View != NULL;  View = View->next)
		{
			if (View->hwnd == hwnd)
			{
			  if(View->InputRoutine3d != 0)
				 View->InputRoutine3d(hwnd,View,&CallData);
			}
		}

//	  HandlePopupMenus(GET_X_LPARAM(pos), GET_Y_LPARAM(pos), &clp);
	  return 0;

	case WM_RBUTTONUP: 
	  openglEvent.xbutton.button = 2;

	  openglEvent.type = ButtonRelease;;
//	  Hv_lastClickType = 1;
	  pos = GetMessagePos();
	  Hv_GetClientPosition(&clp);
	  openglEvent.xmotion.x = clp.x;
	  openglEvent.xmotion.y = clp.y;
	  for (View = Hv_views.first;  View != NULL;  View = View->next)
		{
			if (View->hwnd == hwnd)
			{
			  if(View->InputRoutine3d != 0)
				 View->InputRoutine3d(hwnd,View,&CallData);
			}
		}

//	  HandlePopupMenus(GET_X_LPARAM(pos), GET_Y_LPARAM(pos), &clp);
	  return 0;

	case WM_LBUTTONUP: 
	  openglEvent.type = ButtonRelease;
	  openglEvent.xbutton.button = 1;
//	  Hv_lastClickType = 1;
	  Hv_GetClientPosition(&clp);
	  
	  Hv_StuffXEvent(&xevent,
			 clp.x,
			 clp.y,
			 1,
			 Hv_XModifiers(wParam),
			 0);
			  openglEvent.xmotion.x = clp.x;
			  openglEvent.xmotion.y = clp.y;
			  for (View = Hv_views.first;  View != NULL;  View = View->next)
				{
					if (View->hwnd == hwnd)
					{
					  if(View->InputRoutine3d != 0)
						 View->InputRoutine3d(hwnd,View,&CallData);
					}
				}
			return 0;

	case WM_MOUSEMOVE: 
	  openglEvent.type = MotionNotify;
//	  Hv_lastClickType = 1;
	  Hv_GetClientPosition(&clp);
	  
	  Hv_StuffXEvent(&xevent,
			 clp.x,
			 clp.y,
			 1,
			 Hv_XModifiers(wParam),
			 0);
	  openglEvent.xmotion.x = clp.x;
      openglEvent.xmotion.y = clp.y;
	  CallData.event = &openglEvent;  // setup for later use
	  if ((wParam & MK_SHIFT) == MK_SHIFT)
	     openglEvent.xbutton.state == (ShiftMask + Button1MotionMask);
	  for (View = Hv_views.first;  View != NULL;  View = View->next)
		{
			if (View->hwnd == hwnd)
			{
              if(View->InputRoutine3d != 0)
                 View->InputRoutine3d(hwnd,View,&CallData);
			}
		}

//	  Hv_PointerMotion(NULL, NULL, &xevent);
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
/*
	  if (Hv_firstExposure && (Hv_colorMap != NULL)) {
	    SelectPalette(Hv_gc, Hv_colorMap, False);
	    RealizePalette(Hv_gc);
	    Hv_firstExposure = False;
	  }

	  if ((Hv_TileDraw != NULL) && (logo == 0)) {
			Hv_W32ReTileMainWindow();
	  }
	  
	  if (oldcanvascolor != Hv_canvasColor) {
	    Hv_ChangeWindowBackground(Hv_canvasColor);
		oldcanvascolor = Hv_canvasColor;
	  }

// intialize hrgn to an arbitrary rectangle
*/
	  hrgn = CreateRectRgn(ps.rcPaint.left,
			       ps.rcPaint.top,
			       ps.rcPaint.right,
			       ps.rcPaint.bottom);
	  
//	  Hv_UpdateViews(hrgn); 
	  // Find the view to draw
		for (View = Hv_views.first;  View != NULL;  View = View->next)
		{
			if (View->hwnd == hwnd)
			{
				View->userdraw(View,hrgn);
			}
		}
	  
	  Hv_DestroyRegion(hrgn);
	  EndPaint(hwnd, &ps);
	  Hv_gc = (HANDLE)(-1);

	  return 0;
	  
	case WM_SIZE:
	  w = LOWORD(lParam);
	  h = HIWORD(lParam);

	  //Hv_SetRect(&Hv_canvasRect, 0, 0, (short)w, (short)h);
	  return 0;

	default:
	  return DefWindowProc (hwnd, message, wParam, lParam) ;
	}
}

static BOOL opengl_registerWindowClasses (HINSTANCE hinst) {


    WNDCLASSEX wcex ;

/*
 * Fill in window class structure with parameters that describe
 * the main window.
 */

	wcex.cbSize        = sizeof (WNDCLASSEX) ;
    wcex.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS ;
    wcex.lpfnWndProc   = opengl_mainFrameWndProc ;
    wcex.cbClsExtra    = 0 ;
    wcex.cbWndExtra    = 0 ;
    wcex.hInstance     = hinst ;
    wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW+1) ;

    wcex.lpszMenuName  = NULL ;
    wcex.lpszClassName = "Hv_opengl" ;
    wcex.hIconSm       = NULL;
 
// Register the window class and return success/failure code.

	opengl_winclass = RegisterClassEx(&wcex);
    return (opengl_winclass != 0);
}

/**
 * Hv_W32SetupGLWidget
 * @purpose WIN32 Windows  specific version of Hv_SetupGLWidget.
 * @see The gateway routine Hv_SetupGLWidget. 
 */

void Hv_W32SetupGLWidget(Hv_View View,char *title) {
    int n;
	static int classRegistered = 0;
	HWND hwnd =  NULL;
	HDC	hdc = NULL;
	HDC	hdcm = NULL;
	PIXELFORMATDESCRIPTOR pfd ;
	int nPixelFormat = 0;
    BOOL bResult = FALSE;
	BITMAP binfo ;
    DWORD  dwflags = 0;

#define OPENGL_USEBITMAPS_NOT

#ifdef OPENGL_USEBITMAPS
	HDC	hdc = GetDC(Hv_mainWindow);
	HDC hdcm = CreateCompatibleDC(hdc);
	HBITMAP  oglBitmap = CreateCompatibleBitmap(  hdc,
									800,
									800);
#else
	if(classRegistered == 0)
	{
	  classRegistered = 1;
      opengl_registerWindowClasses (Hv_appContext);
	}
    hwnd =  
        CreateWindowEx (0,              // Extended window styles
                    MAKEINTATOM(opengl_winclass), 
                    title,// Address of window name
                    WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN 
					| WS_BORDER | WS_VISIBLE ,// Window style
                    CW_USEDEFAULT,      // Horizontal position of window
                    0,                  // Vertical position of window
                    600,      // Window width
                    600,                  // Window height
                    NULL,               // Handle of parent or owner window
                    NULL,  // Handle of menu for this window
                    Hv_appContext,              // Handle of application instance
                    NULL) ;             // Address of window-creation data
				
/*** works
		HDC	hdc = GetDC(Hv_mainWindow);
  //  	HDC hdcm = CreateCompatibleDC(hdc);
		HDC	hdcm = GetDC(Hv_mainWindow);//GetDC(Hv_mainWindow);
***/
	    hdc = GetDC(hwnd);
  //  	HDC hdcm = CreateCompatibleDC(hdc);
		hdcm = GetDC(hwnd);//GetDC(Hv_mainWindow);

#endif

	//
	// Fill in the Pixel Format Descriptor
	//

	memset(&pfd,0, sizeof(PIXELFORMATDESCRIPTOR)) ;
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);   
    pfd.nVersion = 1 ; 
#ifdef OPENGL_USEBITMAPS
	SelectObject( hdcm, oglBitmap   );
//	GetObject(oglBitmap,sizeof(BITMAP),&binfo);

	dwflags = PFD_SUPPORT_OPENGL | 
		      PFD_DRAW_TO_BITMAP ;
#else
	        ShowWindow(hwnd, SW_SHOW );

	dwflags = PFD_DOUBLEBUFFER   | // Use double buffer
		      PFD_SUPPORT_OPENGL | // Use OpenGL
			  PFD_DRAW_TO_WINDOW ;//| // Pixel format is for a window.
  //            PFD_SUPPORT_GDI ;
		      
#endif

	pfd.dwFlags =  dwflags;
	pfd.iPixelType = PFD_TYPE_RGBA ;
    pfd.cColorBits = 24;                         // 24-bit color
	pfd.cDepthBits = 32 ;					   	 // 32-bit depth buffer
    pfd.iLayerType = PFD_MAIN_PLANE ;            // Layer type

    nPixelFormat = ChoosePixelFormat(hdc, &pfd);
#ifdef OPENGL_USEBITMAPS
	ReleaseDC(Hv_mainWindow, hdc);
#else
//    ReleaseDC(Hv_mainWindow, hwnd);
#endif

	if (nPixelFormat == 0)
	{
		printf("ChoosePixelFormat Failed %d\r\n",GetLastError()) ;
		return -1 ;
	}
	printf("Pixel Format %d\r\n", nPixelFormat) ;

    bResult = SetPixelFormat(hdcm, nPixelFormat, &pfd);
	if (!bResult)
	{
		printf("SetPixelFormat Failed %d\r\n",GetLastError()) ;
		return -1 ;
	}
	
	//
    // Create a rendering context.
    //
	if(View->glxContext == NULL)
       View->glxContext = wglCreateContext(hdcm);
	if (!View->glxContext)
	{
		printf("wglCreateContext Failed %x\r\n", GetLastError()) ;
		return -1;
	}
    View->frame3D = View;
	View->hdc     = hdcm;   // save the device context
	View->hwnd    = hwnd;
	// Create the palette
//	CreateRGBPalette(dc) ;
 /*   
    Hv_AddCallback(View->frame3D,
		   GLwNinputCallback,
		   (Hv_CallbackProc)Hv_OGLInputCB,
		   (Hv_Pointer)View) ;
  */  
    Hv_OGLInitCB(View->frame3D,(Hv_Pointer) View,(Hv_Pointer) View);
    
//    glFlush();
    Hv_Flush();
	ReleaseDC(Hv_mainWindow, hdc);
//	ReleaseDC(Hv_mainWindow, hdcm);

    return;
} /* End Setup_BasicWin*/

/*-------------------------------------------------------------------*/
/*       _       _ _    ____ ____                                    */
/*      (_)_ __ (_) |_ / ___| __ )                                   */
/*      | | '_ \| | __| |   |  _ \                                   */
/*      | | | | | | |_| |___| |_) |                                  */
/*      |_|_| |_|_|\__|\____|____/                                   */
/*-------------------------------------------------------------------*/

static void Hv_OGLInitCB(Hv_Widget w,
			 Hv_Pointer client_data,
			 Hv_Pointer call_data) {

    int int_client;
    short data;
    Hv_View View;

    View = (Hv_View) call_data;

    int_client=(int)client_data;

/**
   if ( Hv_glxContext == NULL)
     {
     Hv_glxContext = glXCreateContext(XtDisplay(Hv_toplevel), Hv_3Dvi, 0, GL_FALSE);
     glDepthRange(0,1);
     glEnable(GL_DEPTH_TEST);
     glMatrixMode(GL_MODELVIEW);
     }
   View->glxContext = Hv_glxContext;
**/
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
static void Hv_OGLInputCB(Hv_Widget w,
			  Hv_Pointer client_data, 
			  void *call_data) {


   Hv_View View;


   View = (Hv_View) client_data;

   if(View->InputRoutine3d != 0)
       View->InputRoutine3d(w,View,call_data);
   return;
}   /* End inputCB */






/**
 * Hv_W32Set3DFrame
 * @purpose Windows 9X/NT specific version of Hv_Set3DFrame.
 * @see The gateway routine Hv_Set3DFrame. 
 */


void Hv_W32Set3DFrame(Hv_View View) {

	Hv_Rect   *hr;
    BOOL res = 0;
	POINT          clp; // position in client coordinate sys
    RECT       Rect;

//	return;
    if (!Hv_ViewIs3D(View))
	  return;
    
    if (View->frame3D == NULL)
	  return;

    GetWindowRect(  Hv_mainWindow,      
                         &Rect   );
    hr = Hv_GetViewHotRect(View);
	clp.x = hr->left + Rect.left;
	clp.y = hr->top  + Rect.top; 

	ScreenToClient(Hv_mainWindow, &clp);

  res = SetWindowPos(  View->hwnd,             // handle to window
  HWND_TOP,
  clp.x,                 // horizontal position
  clp.y,                 // vertical position
  hr->width,                // width
  hr->height,                // height
  SWP_NOOWNERZORDER |   SWP_NOZORDER    | SWP_NOMOVE  | SWP_NOSIZE  // window-positioning flags
  );

}
#endif
#endif
