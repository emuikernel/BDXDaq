#include "Hv.h"
#include "Hv_pic.h"
#define EVARS 
#include "Hv_opengl.h"
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <ctype.h>

#include <Xm/MainW.h>
#include <Xm/Frame.h>

#define Hv_opengl

#ifdef Hv_USEOPENGL

#include <GL/GLwMDrawA.h>
#ifdef __linux__
WidgetClass glwMDrawingAreaWidgetClass;
#endif


/*----- 3D variables                       ------*/

/*XVisualInfo  *vi; */
Display      *display;

static void initCB(Widget w,caddr_t client_data, caddr_t call_data);
static void inputCB(Widget w,caddr_t client_data, 
                                     GLwDrawingAreaCallbackStruct *call_data);

void Setup_GLWidget(Hv_View View);

/*****************************************************************************
 ____       _                  ____ _ __        ___     _            _   
/ ___|  ___| |_ _   _ _ __    / ___| |\ \      / (_) __| | __ _  ___| |_ 
\___ \ / _ \ __| | | | '_ \  | |  _| | \ \ /\ / /| |/ _` |/ _` |/ _ \ __|
 ___) |  __/ |_| |_| | |_) | | |_| | |__\ V  V / | | (_| | (_| |  __/ |_ 
|____/ \___|\__|\__,_| .__/___\____|_____\_/\_/  |_|\__,_|\__, |\___|\__|
                     |_| |_____|                          |___/          
*****************************************************************************/

void Setup_GLWidget(Hv_View View)
{
  int n;
  Arg      args[20];
  extern Widget Hv_mainw;

/**
  vi = glXChooseVisual(XtDisplay(Hv_toplevel),DefaultScreen(XtDisplay(Hv_toplevel)),Hv_dblbuf);
**/
  if(Hv_3Dvi == NULL)
    fprintf(stderr,"Visual error\n");


  View->frame3D = XtVaCreateWidget("doublebuf",
			      glwMDrawingAreaWidgetClass, XtParent(Hv_canvas),
			      GLwNvisualInfo,             Hv_3Dvi,
			      NULL);
  XtAddCallback(View->frame3D, GLwNinputCallback, (XtCallbackProc)inputCB,(XtPointer)View) ;
  initCB(View->frame3D,(caddr_t) View,(caddr_t) View);

  glXWaitX();
  glFlush();
  XFlush(XtDisplay(Hv_toplevel));
  return;
} /* End Setup_BasicWin*/



/*-------------------------------------------------------------------*/
/*       _       _ _    ____ ____                                    */
/*      (_)_ __ (_) |_ / ___| __ )                                   */
/*      | | '_ \| | __| |   |  _ \                                   */
/*      | | | | | | |_| |___| |_) |                                  */
/*      |_|_| |_|_|\__|\____|____/                                   */
/*-------------------------------------------------------------------*/
GLXContext          Hv_glxContext = NULL;
static void initCB(w, client_data, call_data)
    Widget w;
    caddr_t client_data;
    caddr_t call_data;
{
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
static void inputCB(w,client_data,call_data)
Widget w;
caddr_t client_data;
GLwDrawingAreaCallbackStruct *call_data;
{
   Hv_View View;


   View = (Hv_View) client_data;

   if(View->InputRoutine3d != 0)
       View->InputRoutine3d(w,View,call_data);
   return;
}   /* End inputCB */
#endif
