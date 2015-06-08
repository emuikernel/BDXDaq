/**
 * <EM>This is the "router" that passes off all gateway routines
 * to the corresponding native gateway. For example, if we are
 * running unix/Motif, this will simply hand off to the XM gateway.</EM>
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


#include "Hv.h"
#include "Hv_init.h"


extern Hv_Widget Hv_W32ScrollBarDialogItem(Hv_Widget         parent,
			                               Hv_AttributeArray attributes);

extern Boolean Hv_W32ListItemExists (Hv_Widget list,
									 char *str);

extern void  Hv_W32ListSelectItem(Hv_Widget list,
								  char     *str,
                                  Boolean  callit);

/* the predefined Hv patterns originate in the
   foloowing 16x16 bitmaps */

unsigned char pat0[] = {  /* black */
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

unsigned char pat1[] = { /* 80% */
  0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
  0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
  0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa};

unsigned char pat2[] = { /* 50 % */
  0x33, 0x33, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xcc, 0x33, 0x33, 0x33, 0x33,
  0xcc, 0xcc, 0xcc, 0xcc, 0x33, 0x33, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xcc,
  0x33, 0x33, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xcc};

unsigned char pat3[] = {  /* 20% */
  0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00};
  
unsigned char pat4[] = { /* hatch */
  0x11, 0x11, 0xb8, 0xb8, 0x7c, 0x7c, 0x3a, 0x3a, 0x11, 0x11, 0xa3, 0xa3,
  0xc7, 0xc7, 0x8b, 0x8b, 0x11, 0x11, 0xb8, 0xb8, 0x7c, 0x7c, 0x3a, 0x3a,
  0x11, 0x11, 0xa3, 0xa3, 0xc7, 0xc7, 0x8b, 0x8b};

unsigned char pat5[] = { /* diagonal 1*/
  0x3c, 0x3c, 0x78, 0x78, 0xf0, 0xf0, 0xe1, 0xe1, 0xc3, 0xc3, 0x87, 0x87,
  0x0f, 0x0f, 0x1e, 0x1e, 0x3c, 0x3c, 0x78, 0x78, 0xf0, 0xf0, 0xe1, 0xe1,
  0xc3, 0xc3, 0x87, 0x87, 0x0f, 0x0f, 0x1e, 0x1e};

unsigned char pat6[] = { /*diagonal 2*/
  0x1e, 0x1e, 0x0f, 0x0f, 0x87, 0x87, 0xc3, 0xc3, 0xe1, 0xe1, 0xf0, 0xf0,
  0x78, 0x78, 0x3c, 0x3c, 0x1e, 0x1e, 0x0f, 0x0f, 0x87, 0x87, 0xc3, 0xc3,
  0xe1, 0xe1, 0xf0, 0xf0, 0x78, 0x78, 0x3c, 0x3c};

unsigned char pat7[] = { /* hstripe */
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


unsigned char pat8[] = {  /*vstripe*/
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};

unsigned char pat9[] = {  /* scales */
  0x10, 0x10, 0x10, 0x10, 0x28, 0x28, 0xc7, 0xc7, 0x01, 0x01, 0x01, 0x01,
  0x82, 0x82, 0x7c, 0x7c, 0x10, 0x10, 0x10, 0x10, 0x28, 0x28, 0xc7, 0xc7,
  0x01, 0x01, 0x01, 0x01, 0x82, 0x82, 0x7c, 0x7c};

unsigned char pat10[] = {  /* stars */
  0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x88, 0x08, 0x90, 0x04, 0xa0, 0x02,
  0x40, 0x01, 0x3e, 0x3e, 0x40, 0x01, 0xa0, 0x02, 0x90, 0x04, 0x88, 0x08,
  0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00};

unsigned char pat11[] = {  /* white */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/**
 * Hv_DrawWorldImageItem
 * @purpose  Draw the world image item.
 * @param   Item    The world image item
 * @param   region  The clipping region
 */


void Hv_DrawWorldImageItem(Hv_Item    Item,
			   Hv_Region   region) {
#ifdef WIN32
    Hv_W32DrawWorldImageItem(Item, region);
#else
    Hv_XMDrawWorldImageItem(Item, region);
#endif
}

/**
 * Hv_SaveViewBackground
 * @purpose Save the view background into its pixmap.
  * @param   View      The view to save
 * @param   region    Clipping region
 */


void Hv_SaveViewBackground(Hv_View   View,
			   Hv_Region  region) {
#ifdef WIN32
    Hv_W32SaveViewBackground(View, region);
#else
    Hv_XMSaveViewBackground(View, region);
#endif
}



/**
 * Hv_DrawItemOnPixmap
 * @purpose   Malloc a pixmap an draw an item
 * onto it.  Does NOT draw children.
 * @param Item      The item to draw on the offscreen pixmap
 * @param region    Clipping region
 */


Hv_Pixmap  Hv_DrawItemOnPixmap(Hv_Item   Item,
			       Hv_Region region) {
#ifdef WIN32
    return Hv_W32DrawItemOnPixmap(Item, region);
#else
    return Hv_XMDrawItemOnPixmap(Item, region);
#endif
}
  
/**
 * Hv_SaveUnder
 * @purpose Saves the indicated area into a pixmap (which
 *  it allocates). This is used, for example, by
 *  the native Hv menu routines which must save what is
 *  underneath before popping up. 
 * @param x   Left pixel of save area 
 * @param y   Top pixel of save area 
 * @param w   width of save area 
 * @param h   height of save area 
 */


Hv_Pixmap   Hv_SaveUnder(short x,
			 short y,
			 unsigned int  w,
			 unsigned int  h) {
#ifdef WIN32
    return Hv_W32SaveUnder(x, y, w, h);
#else
    return Hv_XMSaveUnder(x, y, w, h);
#endif
}

/**
 * Hv_RestoreSaveUnder
 * @purpose Restore the indicated area from a pixmap, which
 *  was presumably created by a previous call to
 *  Hv_SaveUnder. This is used, for example, by
 *  the menu routines which must restore what was
 *  underneath after popping down. 
 * @param x   Left pixel of save area 
 * @param y   Top pixel of save area 
 * @param w   width of save area 
 * @param h   height of save area 
 */


void  Hv_RestoreSaveUnder(Hv_Pixmap pmap,
			  short x,
			  short y,
			  unsigned int  w,
			  unsigned int  h) {
#ifdef WIN32
    Hv_W32RestoreSaveUnder(pmap, x, y, w, h);
#else
    Hv_XMRestoreSaveUnder(pmap, x, y, w, h);
#endif
}


/**
 * Hv_UpdateItemOffScreen
 * @purpose   Updates an item offscreeen (and when done,
 * copies it back onscreen). Does NOT update children.
 * @param Item      The item to update offscreen
 * @param region    Clipping region
 */


void  Hv_UpdateItemOffScreen(Hv_Item   Item,
			     Hv_Region region) {
#ifdef WIN32
    Hv_W32UpdateItemOffScreen(Item, region);
#else
    Hv_XMUpdateItemOffScreen(Item, region);
#endif
}

/**
 * Hv_OffScreenViewControlUpdate
 * @purpose This is the offscreen equivalent of
 *  Hv_DrawView that updates a portion
 *  of a view's offscreen buffer.
 *  If in postscript mode, this does nothing
 *  DRAWS ONLY CONTROL ITEMS INDEPENDENT
 *  OF WHETHER OR NOT THE VIEW SAVES THE BACKGROUND
 *  AND INDEPENDENTLY OF WHETHER THE ITEMS HAVE
 *  THEIR "IN BACKGROUND" BIT SET
 * @param   View      The view to update
 * @param   region    Clipping region
 */


void Hv_OffScreenViewControlUpdate(Hv_View    View,
				   Hv_Region  region) {
#ifdef WIN32
    Hv_W32OffScreenViewControlUpdate(View, region);
#else
    Hv_XMOffScreenViewControlUpdate(View, region);
#endif
}


/**
 * Hv_RestoreViewBackground
 * @purpose Copies a view's pffscreen background onto the
 * real screen, effectively "restoring" the view.
 * @param View    The view to restore.
 */


void  Hv_RestoreViewBackground(Hv_View View) {
#ifdef WIN32
    Hv_W32RestoreViewBackground(View);
#else
    Hv_XMRestoreViewBackground(View);
#endif
}



/**
 * Hv_OffScreenViewControlUpdate
 * @purpose This is the offscreen equivalent of
 *  Hv_DrawView that updates a portion
 *  of a view's offscreen buffer.
 *  If in postscript mode, this does nothing
 *  DRAWS ONLY HOTRECT ITEMS INDEPENDENT
 *  OF WHETHER OR NOT THE VIEW SAVES THE BACKGROUND
 *  AND INDEPENDENTLY OF WHETHER THE ITEMS HAVE
 *  THEIR "IN BACKGROUND" BIT SET
 * @param   View      The view to update
 * @param   region    Clipping region
 */

void Hv_OffScreenViewUpdate(Hv_View    View,
			    Hv_Region  region) {
#ifdef WIN32
    Hv_W32OffScreenViewUpdate(View, region);
#else
    Hv_XMOffScreenViewUpdate(View, region);
#endif
  }



/**
 * Hv_ScrollView
 * @purpose   Scrolls a view based on which control was used for scrolling.
 * @param   View        View being scrolled
 * @param   ScrollPart  Control that caused the scroll
 * @param   region      Clipping region
 * @param   StartPP     Starting point
 * @param   StopPP      Stopping point
 */

void Hv_ScrollView(Hv_View   View,
			  int     ScrollPart,
			  Hv_Region    region,
			  Hv_Point   StartPP,
			  Hv_Point   StopPP) {
#ifdef WIN32
    Hv_W32ScrollView(View,
			ScrollPart,
			region,
			StartPP,
			StopPP);
#else
    Hv_XMScrollView(View,
			ScrollPart,
			region,
			StartPP,
			StopPP);
#endif
}


/**
 *  Hv_ShutDown
 */

void Hv_ShutDown() {
#ifdef WIN32
	DestroyWindow(Hv_mainWindow);
#else
	exit(0);
#endif
}

/*==========================================
 * EVENT RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_Go 
 * @purpose This is the never ending main event loop.
 */

void Hv_Go(void) {
#ifdef WIN32
    Hv_W32Go();
#else
    Hv_XMGo();
#endif
}

/**
 * Hv_ProcessEvent
 * @purpose Block until a given event type (matching a
 * bit in mask) is processed. Used for dialog processing.
 * @param   mask   An event mask. Only Hv_IMTimer and 
 * Hv_IMAll are recognized.
 */

void  Hv_ProcessEvent(int mask) {
#ifdef WIN32
    Hv_W32ProcessEvent(mask);
#else
    Hv_XMProcessEvent(mask);
#endif
}


/**
 * Hv_AddEventHandler
 * @purpose  Add an event handler to a widget
 * @param w       The widget getting the handler
 * @param mask    Specifies the type of handler
 * @param handler The actual handler routine
 */

void  Hv_AddEventHandler(Hv_Widget       w,
			 int             mask,
			 Hv_EventHandler handler,
			 Hv_Pointer      data) {

  if ((w == NULL) || (handler == NULL))
    return;

#ifdef WIN32
  Hv_W32AddEventHandler(w, mask, handler, data);
#else
  Hv_XMAddEventHandler(w, mask, handler, data);
#endif

}



/**
 * Hv_RemoveTimeOut
 * @purpose  Remove a timer that is no longer needed
 * @param   id  The id (which was assigned by Hv_AddTimeOut)
 */

void  Hv_RemoveTimeOut(Hv_IntervalId id) {
#ifdef WIN32
   Hv_W32RemoveTimeOut(id);
#else
   Hv_XMRemoveTimeOut(id);
#endif
}


/**
 * Hv_AddTimeOut
 * @purpose Adds a timer
 * @param   interval    The duration in milliseconds
 * @param   callback    The function to call when the timer "goes off"
 * @param   data        User data passed to the callback
 */

Hv_IntervalId  Hv_AddTimeOut(unsigned long          interval,
			     Hv_TimerCallbackProc   callback,
			     Hv_Pointer             data) {
#ifdef WIN32
   return Hv_W32AddTimeOut(interval, callback, data);
#else
   return Hv_XMAddTimeOut(interval, callback, data);
#endif
}



/**
 * Hv_CheckMaskEvent
 * @purpose  To look for events of a given type
 * @param    mask  The desired mask
 * @param    event The returned event
 * @return   True if found one
 */

Boolean Hv_CheckMaskEvent(int        mask,
			  Hv_XEvent *event) {
#ifdef WIN32
   return Hv_W32CheckMaskEvent(mask, event);
#else
   return Hv_XMCheckMaskEvent(mask, event);
#endif
}



/**
 * Hv_Initialize 
 * @purpose application initialization
 * @param        argc      usual number of command line args
 * @param        argv      usual array of string arguments
 * @param        versStr   a version string for the application
 * @return 0 on success
 */

int Hv_Initialize(unsigned int argc,
		  char       **argv,
		  char       *versStr) {

  Hv_offsetDueToScroll = False;
  Hv_offsetDueToDrag = False;
  Hv_activePointers = 0;

/*
 * file separator and path separator variables
 * are initialized here, since they get used in Hv_InitAppName
 */

#ifdef WIN32
  Hv_InitCharStr(&Hv_FileSeparatorString, "\\");
  Hv_FileSeparator = '\\';
  Hv_InitCharStr(&Hv_PathSeparatorString, ";");
  Hv_PathSeparator = ';';
#else
  Hv_InitCharStr(&Hv_FileSeparatorString, "/");
  Hv_FileSeparator = '/';
  Hv_InitCharStr(&Hv_PathSeparatorString, ":");
  Hv_PathSeparator = ':';
#endif

  Hv_InitAppName(argv, versStr);

#ifdef WIN32
   Hv_W32Initialize(argc, argv, versStr);
#else
   Hv_XMInitialize(argc, argv, versStr);
#endif

  Hv_MainInit();

/* If argc is greater than 1, then unknown command line
   options were specified */

  if (argc > 1)
    Hv_InvalidOptions(argc, argv);

  return  0;

}

/**
 * Hv_SystemSpecificInit 
 * @purpose Some more specific initializations
 */

void Hv_SystemSpecificInit() {
#ifdef WIN32
   Hv_W32SystemSpecificInit();
#else
   Hv_XMSystemSpecificInit();
#endif
}

/**
 * Hv_IsTopWindow
 * @purpose  Check the Hv_mainWindow to see if it is on top
 * @return True if main window is on top.
 */

Boolean Hv_IsTopWindow() {
#ifdef WIN32
	return (GetForegroundWindow() == Hv_mainWindow);
#else
	return True;
#endif
}

/**
 * Hv_InitWindow 
 * @purpose Initialize the single "real" window
 */

void  Hv_InitWindow(void) {
#ifdef WIN32
   Hv_W32InitWindow();
#else
   Hv_XMInitWindow();
#endif
}


/**
 * Hv_CreateAppContext 
 * @purpose Creates the application context.
 */

void  Hv_CreateAppContext() {
#ifdef WIN32
   Hv_W32CreateAppContext();
#else
   Hv_XMCreateAppContext();
#endif
}

/**
 * Hv_CreateGraphicsContext 
 * @purpose Create the graphics context
 */

void  Hv_CreateGraphicsContext(void) {
#ifdef WIN32
    Hv_W32CreateGraphicsContext();
#else
    Hv_XMCreateGraphicsContext();
#endif
}

/**
 * Hv_InitCursors 
 * @purpose Initialize the cursors
 */

void  Hv_InitCursors() {
#ifdef WIN32
   Hv_W32InitCursors();
#else
   Hv_XMInitCursors();
#endif
}

/**
 * Hv_InitPatterns 
 * @purpose Initialize the patterns
 */

void  Hv_InitPatterns() {
#ifdef WIN32
   Hv_W32InitPatterns();
#else
   Hv_XMInitPatterns();
#endif
}

/**
 * Hv_InitGraphics 
 * @purpose Late stage graphics initializization.
 */

void  Hv_InitGraphics() {
#ifdef WIN32
   Hv_W32InitGraphics();
#else
   Hv_XMInitGraphics();
#endif
}


/**
 * Hv_InitColors 
 * @purpose Late stage colors initialization
 */

void Hv_InitColors(void) {

    Hv_rainbowFirst = 0;
  Hv_rainbowLast = 159;

  Hv_red               = Hv_rainbowFirst;
  Hv_orange            = Hv_red + 5;
  Hv_yellow            = Hv_red + 11;
  Hv_green             = Hv_red + 22;
  Hv_forestGreen       = Hv_red + 37;
  Hv_wheat             = Hv_red + 48;
  Hv_brown             = Hv_red + 62;
  Hv_white             = Hv_red + 79;
  Hv_gray15            = Hv_red + 80;
  Hv_gray14            = Hv_red + 81;
  Hv_gray13            = Hv_red + 82;
  Hv_gray12            = Hv_red + 83;
  Hv_gray11            = Hv_red + 84;
  Hv_gray10            = Hv_red + 85;
  Hv_gray9             = Hv_red + 86;
  Hv_gray8             = Hv_red + 87;
  Hv_gray7             = Hv_red + 88;
  Hv_gray6             = Hv_red + 89;
  Hv_gray5             = Hv_red + 90;
  Hv_gray4             = Hv_red + 91;
  Hv_gray3             = Hv_red + 92;
  Hv_gray2             = Hv_red + 93;
  Hv_gray1             = Hv_red + 94;
  Hv_black             = Hv_red + 95;
  Hv_purple            = Hv_red + 102;
  Hv_violet            = Hv_red + 114;
  Hv_aliceBlue         = Hv_red + 122;
  Hv_honeydew          = Hv_red + 127;
  Hv_skyBlue           = Hv_red + 130;
  Hv_cornFlowerBlue    = Hv_red + 137;
  Hv_blue              = Hv_red + 150;
  Hv_navyBlue          = Hv_red + 157;
  Hv_lightSeaGreen     = Hv_red + 166;
  Hv_aquaMarine        = Hv_red + 175;
  Hv_coral             = Hv_orange - 1;
  Hv_powderBlue        = Hv_skyBlue-2; 
  Hv_dodgerBlue        = Hv_skyBlue;
  Hv_darkSeaGreen      = Hv_green-10;
  Hv_lawnGreen         = Hv_green-4;
  Hv_steelBlue         = Hv_skyBlue + 4;
  Hv_gray0             = Hv_black;
  Hv_gray16            = Hv_white;
  Hv_lavender          = Hv_violet+4;
  Hv_slateBlue         = Hv_green+6;
  Hv_darkBlue          = Hv_navyBlue;
  Hv_royalBlue         = Hv_blue-5;
  Hv_cadetBlue         = Hv_blue-11;
  Hv_olive             = Hv_forestGreen + 5;
  Hv_mediumForestGreen = Hv_forestGreen + 3;
  Hv_darkGreen         = Hv_forestGreen + 1;
  Hv_tan               = Hv_brown+6;
  Hv_sandyBrown        = Hv_brown+12;
  Hv_peru              = Hv_brown + 4;
  Hv_maroon            = Hv_brown + 7;
  Hv_chocolate         = Hv_brown + 2;
  Hv_rosyBrown         = Hv_coral + 4;
  Hv_peachPuff         = Hv_coral + 4;
  Hv_mistyRose         = Hv_coral + 4;

  Hv_simpleTextBg      = Hv_cadetBlue;
  Hv_simpleTextFg      = Hv_black;
  Hv_simpleTextBorder  = Hv_black;

	Hv_scrollBarColor      = Hv_gray8;
	Hv_fancySymbolLite     = Hv_white;
	Hv_scrollInactiveColor = Hv_aliceBlue;
	Hv_scrollActiveColor   = Hv_skyBlue;
  Hv_highlight           = 1001;

#ifdef WIN32
   Hv_W32InitColors();
#else
   Hv_XMInitColors();
#endif
}

/**
 * Hv_InitCanvas 
 * @purpose Initialize main drawing area.
 */

void Hv_InitCanvas(void) {
#ifdef WIN32
   Hv_W32InitCanvas();
#else
   Hv_XMInitCanvas();
#endif
}

/**
 * Hv_InitGlobalWidgets 
 * @purpose Initialize global widgets.
 */

void Hv_InitGlobalWidgets(void) {
#ifdef WIN32
   Hv_W32InitGlobalWidgets();
#else
   Hv_XMInitGlobalWidgets();
#endif
}

/*==========================================
 * MENU RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_CreateMainMenuBar 
 * @purpose Creates the main menu bar
 * @param parent   The parent widget                 
 * @return   The menubar widget
 */

Hv_Widget  Hv_CreateMainMenuBar(Hv_Widget   parent) {
#ifdef WIN32
   return Hv_W32CreateMainMenuBar(parent);
#else
   return Hv_XMCreateMainMenuBar(parent);
#endif
}

/**
 * Hv_CreatePopup 
 * @purpose  Creates a popup menu
 * @param parent   The parent widget                 
 * @return   The popup widget
 */

Hv_Widget  Hv_CreatePopup(Hv_Widget   parent) {
#ifdef WIN32
   return Hv_W32CreatePopup(parent);
#else
   return Hv_XMCreatePopup(parent);
#endif
}


/**
 * Hv_CreateSubmenu 
 * @purpose Creates a submenu
 * @param parent   The parent menu                 
 * @return   The submenu widget
 */

Hv_Widget  Hv_CreateSubmenu(Hv_Widget   parent) {
#ifdef WIN32
   return Hv_W32CreateSubmenu(parent);
#else
   return Hv_XMCreateSubmenu(parent);
#endif
}


/**
 * Hv_Popup1Callback 
 * @purpose  Callback for the first global popup menu
 * @param    dummy       A placeholder (not used, actually will be Hv_canvas)  
 * @param    popup       The popup
 * @param    cbs         Pointer to struct that can tell us where to pop the menu
 */


void Hv_Popup1Callback(Hv_Widget          dummy,
		       Hv_Widget          popup,
		       Hv_CanvasCallback *cbs) {
#ifdef WIN32
   Hv_W32Popup1Callback(dummy, popup, cbs);
#else
   Hv_XMPopup1Callback(dummy, popup, cbs);
#endif
}


/**
 * Hv_Popup2Callback 
 * @purpose  Callback for the second global popup menu
 * @param    dummy       A placeholder (not used, actually will be Hv_canvas)  
 * @param    popup       The popup
 * @param    cbs         Pointer to struct that can tell us where to pop the menu
 */


void Hv_Popup2Callback(Hv_Widget          dummy,
		       Hv_Widget          popup,
		       Hv_CanvasCallback *cbs) {
#ifdef WIN32
   Hv_W32Popup2Callback(dummy, popup, cbs);
#else
   Hv_XMPopup2Callback(dummy, popup, cbs);
#endif
}

/**
 * Hv_CreateUserMenus 
 * @purpose Creates space for Hv_MAXUSERMENUS additional user menus
 */

void     Hv_CreateUserMenus(void) {
#ifdef WIN32
   Hv_W32CreateUserMenus();
#else
   Hv_XMCreateUserMenus();
#endif
}

/**
 * Hv_SetMenuItemString 
 * @purpose Changes a menu item's label
 * @param       menuitem     The affected menu item
 * @param       str          The new label
 * @param       font         The font index 
 */


void  Hv_SetMenuItemString(Hv_Widget menuitem,
			   char                 *str,
			   short                 font) {

	if ((menuitem == NULL) || (str == NULL))
		return;

#ifdef WIN32
   Hv_W32SetMenuItemString(menuitem, str, font);
#else
   Hv_XMSetMenuItemString(menuitem, str, font);
#endif
}

/**
 * Hv_CreateHelpPullDown 
 * @purpose Creates the help menu
 */


Hv_Widget Hv_CreateHelpPullDown(void) {
#ifdef WIN32
   return Hv_W32CreateHelpPullDown();
#else
   return Hv_XMCreateHelpPullDown();
#endif
}

/**
 * Hv_CreatePullDown 
 * @purpose Creates a pulldown menu
 * @param     title    Label for the pulldown menu
 */

Hv_Widget	Hv_CreatePullDown(char *title) {
#ifdef WIN32
   return Hv_W32CreatePullDown(title);
#else
   return Hv_XMCreatePullDown(title);
#endif
}

/**
 * Hv_AddMenuSeparator 
 * @purpose  Adds a separator to a menu
 * @param    menu     The parent menu
 * @return   The newly created sepatator widget
 */

Hv_Widget Hv_AddMenuSeparator(Hv_Widget menu) { 
#ifdef WIN32
   return Hv_W32AddMenuSeparator(menu);
#else
   return Hv_XMAddMenuSeparator(menu);
#endif
}


/**
 * Hv_AddSubMenuItem 
 * @purpose  Create a submenu item and add it to a menu.
 * @param    label    The label for the submenu
 * @param    menu     The parent menu
 * @return   The newly created submenu
 */


Hv_Widget Hv_AddSubMenuItem(char    *label,
			    Hv_Widget menu,
			    Hv_Widget submenu) {
#ifdef WIN32
   return Hv_W32AddSubMenuItem(label, menu, submenu);
#else
   return Hv_XMAddSubMenuItem(label, menu, submenu);
#endif
}


/**
 * Hv_AddMenuToggle 
 * @purpose  Create a toggle item and add it to a menu.
 * @param    label        The label for the submenu
 * @param    menu         The parent menu
 * @param    ID           The ID of the new item
 * @param    callback     The callback function
 * @param    state        The default state
 * @param    btype        The indicator type
 * @param    acctype      The accelerator type
 * @param    accchr       The accelerator character
 * @return   The newly created submenu
 */

Hv_Widget Hv_AddMenuToggle(char *label,
			   Hv_Widget        menu,
			   long             ID,
			   Hv_CallbackProc  callback,
			   Boolean	    state,
			   unsigned char    btype,
			   int	            acctype,
			   char	            accchr) {
#ifdef WIN32
   return Hv_W32AddMenuToggle(label, menu, ID, callback, state, btype, acctype, accchr);
#else
   return Hv_XMAddMenuToggle(label, menu, ID, callback, state, btype, acctype, accchr);
#endif
}

/**
 * Hv_AddMenuItem 
 * @purpose  Create a submenu item and add it to a menu.
 * @param    label        The label for the submenu
 * @param    menu         The parent menu
 * @param    ID           The ID of the new item
 * @param    callback     The callback function
 * @param    acctype      The accelerator type
 * Qparam    accchr       The accelerator character
 * @return   The newly created submenu
 */

Hv_Widget Hv_AddMenuItem(char *label,
			 Hv_Widget       menu,
			 long            ID,
			 Hv_CallbackProc callback,
			 int	         acctype,
			 char	         accchr) {		
#ifdef WIN32
   return Hv_W32AddMenuItem(label, menu, ID, callback, acctype, accchr);
#else
   return Hv_XMAddMenuItem(label, menu, ID, callback, acctype, accchr);
#endif
}



/*==========================================
 * MOUSE RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_GetCurrentPointerLocation 
 * @purpose   Returns the coordinate pixels of the mouse.
 * @param x        pointer to the variable to hold the x pixel
 * @param y        pointer to the variable to hold the y pixel
 */


 void  Hv_GetCurrentPointerLocation(short *x,
				    short *y) {
#ifdef WIN32
   Hv_W32GetCurrentPointerLocation(x, y);
#else
   Hv_XMGetCurrentPointerLocation(x, y);
#endif
 }


/*==========================================
 * COMPOUND STRING  RELATED SECTION OF GATEWAY
 *===========================================*/

/**
 * Hv_CreateSimpleCompoundString
 
 * @purpose A convenience routine for a compound string 
 * @param   text    pointer to the text of the compound string
 * @return  The compound string
 */

Hv_CompoundString Hv_CreateSimpleCompoundString(char  *text) {
#ifdef WIN32
  return Hv_W32CreateSimpleCompoundString(text);
#else
  return Hv_XMCreateSimpleCompoundString(text);
#endif
}

/**
 * Hv_CreateCompoundString
 
 * @purpose Create a compound string
 * @param   text    pointer to the text of the compound string
 * @param   font    font index
 * @return  The compound string
 */

Hv_CompoundString Hv_CreateCompoundString(char  *text,
					  short  font) {
#ifdef WIN32
  return Hv_W32CreateCompoundString(text, font);
#else
  return Hv_XMCreateCompoundString(text, font);
#endif
}


/**
 * Hv_CreateAndSizeCompoundStrig
 
 * @purpose Create a compound string and return its size in pixels
 * @param   text    pointer to the text of the compound string
 * @param   font    font index
 * @param   w       pointer to width
 * @param   h       pointer to height
 * @return  The compound string
 */


Hv_CompoundString  Hv_CreateAndSizeCompoundString(char    *text,
						  short   font,
						  short   *w,
						  short   *h) {
#ifdef WIN32
  return Hv_W32CreateAndSizeCompoundString(text, font, w, h);
#else
  return Hv_XMCreateAndSizeCompoundString(text, font, w, h);
#endif
}


/**
 * Hv_CompoundStringFree 
 * @purpose    Frees the memory associated with a compound string. 
 * <p>
 * NOTE: This is safe against cs = NULL.
 * <p>
 * CompoundStrings are an unfortunate part of Hv. Most developers
 * can get by using the simpler Hv_String type.
 * @param cs     The compound string input.
 */

void Hv_CompoundStringFree(Hv_CompoundString cs) {
  if (cs == NULL)
    return;

#ifdef WIN32
  Hv_W32CompoundStringFree(cs);
#else
  Hv_XMCompoundStringFree(cs);
#endif
}


/**
 * Hv_CompoundStringWidth
 * @purpose returns the string width in pixels
 * @return The string width
 */


short Hv_CompoundStringWidth(Hv_CompoundString cs) {
    if (cs == NULL)
	return 0;

#ifdef WIN32
    return Hv_W32CompoundStringWidth(cs);
#else
    return Hv_XMCompoundStringWidth(cs);
#endif
}


/**
 * Hv_CompoundStringBaseline
 * @purpose returns the baseline of the string
 * @return The string baseline
 */


short Hv_CompoundStringBaseline(Hv_CompoundString cs) {
    if (cs == NULL)
	return 0;

#ifdef WIN32
    return Hv_W32CompoundStringBaseline(cs);
#else
    return Hv_XMCompoundStringBaseline(cs);
#endif
}

/**
 * Hv_CompoundStringHeight
 * @purpose returns the string height in pixels
 * @return The string height
 */

short Hv_CompoundStringHeight(Hv_CompoundString cs) {
    if (cs == NULL)
	return 0;

#ifdef WIN32
    return Hv_W32CompoundStringHeight(cs);
#else
    return Hv_XMCompoundStringHeight(cs);
#endif
}



/**
 * Hv_CompoundStringDrawImage 
 * @purpose   Draws a compound string using an image.
 * <p>
 * CompoundStrings are an unfortunate part of Hv. Most developers
 * can get by using the simpler Hv_String type.
 * @param cs     The compound string input.
 * @param x      Horizontal pixel location.
 * @param y      Vertical   pixel location.
 * @param sw     String pixel width.
 */

void Hv_CompoundStringDrawImage(Hv_CompoundString cs,
				short             x,
				short             y,
				short             sw) {
#ifdef WIN32
  Hv_W32CompoundStringDrawImage(cs, x, y, sw);
#else
  Hv_XMCompoundStringDrawImage(cs, x, y, sw);
#endif
}

/**
 * Hv_CompoundStringDraw
 
 * @purpose Draws a compound string
 * <p>
 * CompoundStrings are an unfortunate part of Hv. Most developers
 * can get by using the simpler Hv_String type.
 * @param cs     The compound string input.
 * @param x      Horizontal pixel location.
 * @param y      Vertical   pixel location.
 * @param sw     String pixel width.
 */

void Hv_CompoundStringDraw(Hv_CompoundString cs,
			   short             x,
			   short             y,
			   short             sw) {
#ifdef WIN32
  Hv_W32CompoundStringDraw(cs, x, y, sw);
#else
  Hv_XMCompoundStringDraw(cs, x, y, sw);
#endif
}


/**
 * Hv_CreateMotifString
 
 * @purpose Create a compound string
 * @param   text    pointer to the text of the compound string
 * @param   font    font index
 * @return  The compound string
 * @deprecated (Use Hv_CreateAndSizeCompoundString)
 */

Hv_CompoundString Hv_CreateMotifString(char  *text,
				       short  font) {
  return Hv_CreateCompoundString(text, font);
}


/**
 * Hv_CreateAndSizeMotifStrig
 
 * @purpose Create and size a compound string
 * @param   text    pointer to the text of the compound string
 * @param   font    font index
 * @param   w       pointer to width
 * @param   h       pointer to height
 * @return  The compound string
 * @deprecated (Use Hv_CreateAndSizeCompoundString)
 */


Hv_CompoundString  Hv_CreateAndSizeMotifString(char    *text,
					       short   font,
					       short   *w,
					       short   *h) {
  return Hv_CreateAndSizeCompoundString(text, font, w, h);
}



/*==========================================
 * REGION RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_UnionRectangleWithRegion 
 * @purpose Adds a rectangle's area into a region.
 * @param     xr      the rectangle
 * @param    region   the region
 */

void Hv_UnionRectangleWithRegion(Hv_Rectangle  *xr,
				 Hv_Region     region) {
#ifdef WIN32
  Hv_W32UnionRectangleWithRegion(xr, region);
#else
  Hv_XMUnionRectangleWithRegion(xr, region);
#endif
}
 

/**
 * Hv_IntersectRegion 
 * @purpose  Intersect two regions
 * @param      reg1    one region
 * @param      reg2    the other region
 * @return     the intersection of the two regions
 */


Hv_Region Hv_IntersectRegion(Hv_Region reg1,
			     Hv_Region reg2) {
#ifdef WIN32
  return Hv_W32IntersectRegion(reg1, reg2);
#else
  return Hv_XMIntersectRegion(reg1, reg2);
#endif
}


/**
 * Hv_SubtractRegion 
 * @purpose Subtracts two regions (reg1 - reg2)
 * @param      reg1    one region
 * @param      reg2    the other region
 * @return     reg1 minus reg2
 */


Hv_Region Hv_SubtractRegion(Hv_Region reg1,
			    Hv_Region reg2) {
#ifdef WIN32
  return Hv_W32SubtractRegion(reg1, reg2);
#else
  return Hv_XMSubtractRegion(reg1, reg2);
#endif
}


/**
 * Hv_UnionRegion 
 * @purpose Adds two regions together
 * @param      reg1    one region
 * @param      reg2    the other region
 * @return     reg1 plus reg2
 */

Hv_Region Hv_UnionRegion(Hv_Region reg1,
			 Hv_Region reg2) {
#ifdef WIN32
  return Hv_W32UnionRegion(reg1, reg2);
#else
  return Hv_XMUnionRegion(reg1, reg2);
#endif
}

/**
 * Hv_ShrinkRegion 
 * @purpose Shrinks a region (or exapands it,
 * if dh or dv are negative)
 * @param  region     The region to shrink
 * @param  dh         horizontal shrinkage (pixels)
 * @param  dv         vertical shrinkage (pixels)
 */


void Hv_ShrinkRegion(Hv_Region region,
		     short     dh,
		     short     dv) {
#ifdef WIN32
  Hv_W32ShrinkRegion(region, dh, dv);
#else
  Hv_XMShrinkRegion(region, dh, dv);
#endif
}

/**
 * Hv_OffsetRegion 
 * @purpose Offsets a region 
 * @param  region     The region to shrink
 * @param  dh         horizontal offset (pixels)
 * @param  dv         vertical offset (pixels)
 */


void Hv_OffsetRegion(Hv_Region region,
		     short     dh,
		     short     dv) {
#ifdef WIN32
  Hv_W32OffsetRegion(region, dh, dv);
#else
  Hv_XMOffsetRegion(region, dh, dv);
#endif
}

/**
 * Hv_CreateRegionFromPolygon 
 * @purpose Create a region from a set of points.
 * @param  poly    the set of points
 * @param  n       tne number of points
 * @return the created region
 */

Hv_Region Hv_CreateRegionFromPolygon(Hv_Point  *poly,
				     short      n) {
#ifdef WIN32
  return Hv_W32CreateRegionFromPolygon(poly, n);
#else
  return Hv_XMCreateRegionFromPolygon(poly, n);
#endif
}

/**
 * Hv_EmptyRegion 
 * @purpose  Test whether a region is empty.
 * @param    region    The region to test.
 * @return  True if the region is empty
 */

Boolean  Hv_EmptyRegion(Hv_Region region) {

	if (region == NULL)
		return True;

#ifdef WIN32
  return Hv_W32EmptyRegion(region);
#else
  return Hv_XMEmptyRegion(region);
#endif
}

/**
 * Hv_CreateRegion 
 * @purpose  Creates a new empty region
 * @return  a newly created (empty) region
 */
 
Hv_Region  Hv_CreateRegion(void) {
#ifdef WIN32
  return Hv_W32CreateRegion();
#else
  return Hv_XMCreateRegion();
#endif
}

/**
 * Hv_DestroyRegion 
 * @purpose  Frees a region
 * @param  region     the region to free
 */
 
void  Hv_DestroyRegion(Hv_Region region) {
#ifdef WIN32
  Hv_W32DestroyRegion(region);
#else
  Hv_XMDestroyRegion(region);
#endif
}

/**
 * Hv_SetFunction 
 * @purpose  Sets the drawing "function".
 * @param funcbit   Should be either Hv_GXCOPY for normal
 * drawing or Hv_GXXOR for XOR drawing
 */

void Hv_SetFunction(int funcbit) {
#ifdef WIN32
  Hv_W32SetFunction(funcbit);
#else
  Hv_XMSetFunction(funcbit);
#endif
}

/**
 * Hv_GetGCValues 
 * @purpose  gets the current GC values.
 * @param gcv   Upon return will contain the GC setting
 */
void Hv_GetGCValues(int *gcv) {
#ifdef WIN32
	Hv_W32GetGCValues(gcv);
#else
	Hv_XMGetGCValues(gcv);
#endif
}


/**
 * Hv_PointInRegion
 * @purpose Check if a given point is inside a region
 * @return True if the point is inside
 */

Boolean Hv_PointInRegion(Hv_Point  pp,
			 Hv_Region region) {

    if (region == NULL)
	return False;

#ifdef WIN32
  return Hv_W32PointInRegion(pp, region);
#else
  return Hv_XMPointInRegion(pp, region);
#endif
}

/**
 * Hv_RectInRegion 
 * @purpose Returns true if the rect is at least
 * partially inside the region.
 * @param     r       The rectangle to check
 * @param     region  The region to check against
 * @return    true if at least partial intersection
 */

Boolean  Hv_RectInRegion(Hv_Rect   *r,
			 Hv_Region region) {

#ifdef WIN32
    return Hv_W32RectInRegion(r, region);
#else
    return Hv_XMRectInRegion(r, region);
#endif
}



/**
 * Hv_ClipBox 
 * @purpose Returns a rectangle that encloses a region
 * @param   region   The region to find an enclosing rect
 * @param   rect     The returned, enclosing rect.
 */

void     Hv_ClipBox(Hv_Region  region,
		    Hv_Rect   *rect) {

#ifdef WIN32
    Hv_W32ClipBox(region, rect);
#else
    Hv_XMClipBox(region, rect);
#endif
}

/**
 * Hv_CheckForExposures
 * @purpose Check for exposures and try to process them before
 * they show up in the normal event queue (probably only needs to
 * be implemented for X)
 * @param     whattype      Type of exposure
 */

void Hv_CheckForExposures(int whattype) {
#ifdef WIN32
    Hv_W32CheckForExposures(whattype);
#else
    Hv_XMCheckForExposures(whattype);
#endif
}

/*==========================================
 * DRAWING RELATED SECTION OF GATEWAY
 *===========================================*/

/**
 * Hv_SetLineStyle
 * @purpose Set the current line drawing style
 * @param      w     The new line width
 * @param      style The new line style from the set [Hv_SOLIDLINE, 
 * Hv_DASHED, Hv_LONGDASHED, Hv_DOTDASH].
 */

void Hv_SetLineStyle(int w,
		     int style) {


    if (Hv_inPostscriptMode) {
	if (w < 1)
	    w = 1;
	w *= 3;
	
	Hv_PSSetLineStyle(w, style);
	return;
    }

#ifdef WIN32
    Hv_W32SetLineStyle(w, style);
#else
    Hv_XMSetLineStyle(w, style);
#endif
}

/**
 * Hv_SetPattern
 * @purpose Set the current pattern for the pen
 * @param    pattern from the set [Hv_FILLSOLIDPAT, Hv_80PERCENTPAT, 
 * Hv_50PERCENTPAT, Hv_20PERCENTPAT, Hv_HATCHPAT, Hv_DIAGONAL1PAT, Hv_DIAGONAL2PAT, 
 * Hv_HSTRIPEPAT, Hv_VSTRIPEPAT, Hv_SCALEPAT, Hv_STARPAT, Hv_HOLLOWPAT].
 * @param    background color
 */

void Hv_SetPattern(int pattern,
		   short color) {

    if (Hv_inPostscriptMode) {
	Hv_PSSetPattern(pattern, color);
	return;
    }

#ifdef WIN32
    Hv_W32SetPattern(pattern, color);
#else
    Hv_XMSetPattern(pattern, color);
#endif
}

/**
 * Hv_EraseRectangle
 * @purpose   Erase a rectangular area, filing in
 * background (which may be a tile)
 * @param left    Left of rectangle
 * @param top     Top of rectangle
 * @param width   Width of rectangle
 * @param height  Height of rectangle
 */

void	Hv_EraseRectangle(short left,
			  short top,
			  short width,
			  short height) {
#ifdef WIN32
    Hv_W32EraseRectangle(left, top, width, height);
#else
    Hv_XMEraseRectangle(left, top, width, height);
#endif
}

/**
 * Hv_TileMainWindow
 * @purpose Tile the main window with a pixmap, usually some sort of logo.
 * @param   pmap   The pixmap to tile
 */

void   Hv_TileMainWindow(Hv_Pixmap pmap) {
#ifdef WIN32
    Hv_W32TileMainWindow(pmap);
#else
    Hv_XMTileMainWindow(pmap);
#endif
}

/**
 * Hv_ReTileMainWindow
 * @purpose Retile the main window with a pixmap, usually some sort of logo.
 */

void Hv_ReTileMainWindow(void) {
#ifdef WIN32
    Hv_W32ReTileMainWindow();
#else
    Hv_XMReTileMainWindow();
#endif
}

/**
 * Hv_SetBackground
 * @purpose Set the current background for drawing.
 * @note the argument is NOT a color index. This routine
 * is for internal use only.
 * @param  bg   The background pixel 
 */

void  Hv_SetBackground(unsigned long bg) {
#ifdef WIN32
    Hv_W32SetBackground(bg);
#else
    Hv_XMSetBackground(bg);
#endif
}


/**
 * Hv_SetForeground
 * @purpose Set the current foreground for drawing.
 * @note the argument is NOT a color index. This routine
 * is for internal use only.
 * @param  bg   The foreground pixel 
*/

void  Hv_SetForeground(unsigned long fg) {
#ifdef WIN32
    Hv_W32SetForeground(fg);
#else
    Hv_XMSetForeground(fg);
#endif
}


/**
 * Hv_SetFillStyle
 * @purpose Set the current fill style for drawing -- fillstyle
 * should be from the set [Hv_FillSolid, Hv_FillTiled, Hv_Stippled].
 * @note This routine is for internal use only.
 * @param  bg   The foreground pixel 
*/

void  Hv_SetFillStyle(int fillstyle) {
#ifdef WIN32
    Hv_W32SetFillStyle(fillstyle);
#else
    Hv_XMSetFillStyle(fillstyle);
#endif
}


/**
 * Hv_SetLineWidth
 * @purpose Set the current line width of the pen
 * @param   w   line width
 */

void Hv_SetLineWidth(int w) {

    if (Hv_inPostscriptMode) {
	if (w < 1)
	    w = 1;
	w *= 3;
	Hv_PSSetLineWidth(w);    
	return;
    }

#ifdef WIN32
    Hv_W32SetLineWidth(w);
#else
    Hv_XMSetLineWidth(w);
#endif
}


/**
 * Hv_SetClippingRegion
 * @purpose Set the current clipping region
 */

void  Hv_SetClippingRegion(Hv_Region region) {
  if (region == NULL)
      return;

#ifdef WIN32
    Hv_W32SetClippingRegion(region);
#else
    Hv_XMSetClippingRegion(region);
#endif
}


/**
 * Hv_FreePixmap
 * @purpose Free a pixmap allocated previously with Hv_CreatePixmap
 */


void   Hv_FreePixmap(Hv_Pixmap pmap) {
#ifdef WIN32
    Hv_W32FreePixmap(pmap);
#else
    Hv_XMFreePixmap(pmap);
#endif
}


/**
 * Hv_CreatePixmap
 * @purpose Create a pixmap, often for offscreen drawing
 * @param   w   The width  in pixels
 * @param   h   The height in pixels
 * @return  The allocated pixmap
 */

Hv_Pixmap  Hv_CreatePixmap(unsigned int  w,
			   unsigned int  h) {

#ifdef WIN32
    return Hv_W32CreatePixmap(w, h);
#else
    return Hv_XMCreatePixmap(w, h);
#endif
}

/**
 * Hv_ClearArea
 * @purpose Copmpletely clear a rectangular area, usually 
 * on a pixmap.
 * @param x       Left of source rectangle
 * @param y       Top of source rectangle
 * @param w       Width of source rectangle
 * @param h       Height of source rectangle
  */
void Hv_ClearArea(short x,
				  short y,
				  short w,
				  short h) {
#ifdef WIN32
    Hv_W32ClearArea(x, y, w, h);
#else
    Hv_XMClearArea(x, y, w, h);
#endif
}


/**
 * Hv_CopyArea
 * @purpose Copy rectangular region of pixels from one window to another. Usually
 * used in Hv to copy to and from off screen pixmaps.
 * @param src     Source window or pixmap
 * @param dest    Destination window or pixmap
 * @param sx      Left of source rectangle
 * @param sy      Top of source rectangle
 * @param w       Width of source rectangle
 * @param h       Height of source rectangle
 * @param dx      Left of destination rectangle
 * @param dy      Top of destination rectangle
 */

void Hv_CopyArea(Hv_Window src,
		 Hv_Window dest,
		 int       sx,
		 int       sy,
		 unsigned  int w,
		 unsigned  int h,
		 int       dx,
		 int       dy) {
#ifdef WIN32
  Hv_W32CopyArea(src, dest, sx, sy, w, h, dx, dy);
#else
  Hv_XMCopyArea(src, dest, sx, sy, w, h, dx, dy);
#endif
}


/**
 * Hv_DestroyImage
 * @purpose Destroys an Hv_Image
 * @param image   The image to destroy
 */

void Hv_DestroyImage(Hv_Image *image) {


  if (image == NULL)
    return;

#ifdef WIN32
  Hv_W32DestroyImage(image);
#else
  Hv_XMDestroyImage(image);
#endif
}


/**
 * Hv_PutImage
 * @purpose Place an image onto a window or pixmap.
 * @param window  Destination window or pixmap
 * @param sx      Left of source rectangle
 * @param sy      Top of source rectangle
 * @param dx      Left of destination rectangle
 * @param dy      Top of destination rectangle
 * @param w       Width of source rectangle
 * @param h       Height of source rectangle
 */

void Hv_PutImage(Hv_Window window,
		 Hv_Image *image,
		 int       sx,
		 int       sy,
		 int       dx,
		 int       dy,
		 unsigned  int w,
		 unsigned  int h) {
#ifdef WIN32
  Hv_W32PutImage(window, image, sx, sy, dx, dy, w, h);
#else
  Hv_XMPutImage(window, image, sx, sy, dx, dy, w, h);
#endif
}



/**
 * Hv_DrawTextItemVertical
 * @purpose Draw the text item rotated 90 degrees CCW
 * @param  Item   The text item
 * @param  region The clipping region
 */


void Hv_DrawTextItemVertical(Hv_Item    Item,
			     Hv_Region  region) {
#ifdef WIN32
  Hv_W32DrawTextItemVertical(Item, region);
#else
  Hv_XMDrawTextItemVertical(Item, region);
#endif
}

/**
 * Hv_DrawPoint
 
 * @purpose Draw a single pixel point
 * @param    x      the horizontal pixel
 * @param    y      the vertical pixel
 * @param    color  the color index
 */

void Hv_DrawPoint(short x,
		  short y,
		  short color) {
  
  if (Hv_inPostscriptMode) {
    Hv_LocalToPSLocal(x, y, &x, &y);
    Hv_PSDrawPoint(x, y, color);
    return;
  }

#ifdef WIN32
  Hv_W32DrawPoint(x, y, color);
#else
  Hv_XMDrawPoint(x, y, color);
#endif
}


/**
 * Hv_DrawPoints
 
 * @purpose Draw an array of points (not connected).
 * @param    xp     the array of points
 * @param    np     the number of points
 * @param    color  the color index
 */

void Hv_DrawPoints(Hv_Point *xp,
		   int       np,
		   short     color) {

  Hv_Point      *psxp;
  
  if (Hv_inPostscriptMode) {
    psxp = Hv_NewPoints(np);
    Hv_LocalPolygonToPSLocalPolygon(xp, psxp, np);
    Hv_PSDrawPoints(psxp, (short)np, color);
    Hv_Free(psxp);
    return;
  }

#ifdef WIN32
  Hv_W32DrawPoints(xp, np, color);
#else
  Hv_XMDrawPoints(xp, np, color);
#endif
}


/**
 * Hv_FrameArc 
 * @purpose   Gateway routine for framing an arc
 * @param x       left pixel of bounding rect
 * @param y       top  pixel of bounding rect
 * @param width   pixel width  of bounding rect
 * @param height  pixel height of bounding rect
 * @param ang1    start angle (in 64ths of a degree)
 * @param ang2    stop angle (in 64ths of a degree) RELATIVE to ang1
 * @param color   color index of frame color
 */

void	Hv_FrameArc(short   x,
		    short   y,
		    short   width,
		    short   height,
		    int     ang1,
		    int     ang2,
		    short   color) {
  
  if (Hv_inPostscriptMode) {
    return;
  }

#ifdef WIN32
  Hv_W32FrameArc(x, y, width, height, ang1, ang1+ang2, color);
#else
  Hv_XMFrameArc(x, y, width, height, ang1, ang2, color);
#endif

}

/**
 * Hv_FillArc 
 * @purpose    Gateway routine for filling an arc
 * @param x       left pixel of bounding rect
 * @param y       top  pixel of bounding rect
 * @param width   pixel width  of bounding rect
 * @param height  pixel height of bounding rect
 * @param ang1    start angle (in 64ths of a degree)
 * @param ang2    stop angle (in 64ths of a degree) RELATIVE to start angle
 * @param frame   if True, frame arc too
 * @param color   color index of FILL color
 * @param framec  color index of FRAME color
 */

void	Hv_FillArc(short   x,
		   short   y,
		   short   width,
		   short   height,
		   int     ang1,
		   int     ang2,
		   Boolean frame,
		   short   color,
		   short   framec) {

  
  if (Hv_inPostscriptMode) {
    return;
  }

#ifdef WIN32
  Hv_W32FillArc(x, y, width, height, ang1, ang1+ang2, frame, color, framec);
#else
  Hv_XMFillArc(x, y, width, height, ang1, ang2, frame, color, framec);
#endif

}
    

/**
 * Hv_DrawLines 
 * @purpose    In a connect-the-dots fashion, draws a set
 * of lines.
 *
 * @param        xp      pointer to an array of lines
 * @param        np      the number of points
 * @param        color   the index for the line color
 */

void Hv_DrawLines(Hv_Point  *xp,
		  short      np,
		  short      color) {
    
  Hv_Point       *psxp;
    
  if (Hv_inPostscriptMode) {
    psxp = Hv_NewPoints((int)np);
    Hv_LocalPolygonToPSLocalPolygon(xp, psxp, np);
    Hv_PSDrawLines(psxp, np, color);
    Hv_Free(psxp);
    return;
  }

/*
 * If made it here, we are not postscript
 */

#ifdef WIN32
    Hv_W32DrawLines(xp, np, color);
#else
    Hv_XMDrawLines(xp, np, color);
#endif
}

/*--------------------------------------
 *
 * Hv_FillPolygon
 *
 * A Gateway routine for filling and
 * optionally framing a polygon.
 *
 *--------------------------------------*/

void Hv_FillPolygon(Hv_Point   *xp,
		    short       np,
		    Boolean     frame,
		    short       fillcolor,
		    short       framecolor) {
   
  Hv_Point       *psxp;
    
    
  if (Hv_inPostscriptMode) {
    psxp = Hv_NewPoints((int)np);
    Hv_LocalPolygonToPSLocalPolygon(xp, psxp, np);
    
    if (fillcolor >= 0)
      Hv_PSFillPolygon(psxp, np, fillcolor);
    
    if (frame)
      Hv_PSFramePolygon(psxp, np, framecolor);
    Hv_Free(psxp);
    return;
  }

/*
 * If made it here, we are not postscript
 */

#ifdef WIN32
    Hv_W32FillPolygon(xp, np, frame, fillcolor, framecolor);
#else
    Hv_XMFillPolygon(xp, np, frame, fillcolor, framecolor);
#endif
    
}

/*--------------------------------------
 *
 * Hv_DrawSegments
 *
 * A Gateway routine for drawing line
 * segments in a specified color.
 *
 *--------------------------------------*/

void Hv_DrawSegments(Hv_Segment *xseg,
		     short       nseg,
		     short       color) {
    
  Hv_Segment        *psseg;
    
  if (Hv_inPostscriptMode) {
    psseg = (Hv_Segment *)Hv_Malloc(nseg*sizeof(Hv_Segment));
    Hv_LocalSegmentsToPSLocalSegments(xseg, psseg, nseg);
    Hv_PSDrawSegments(psseg, nseg, color);
    Hv_Free(psseg);
    return;
  }
    
/*
 * If made it here, we are not postscript
 */

#ifdef WIN32
  Hv_W32DrawSegments(xseg, nseg, color);
#else
  Hv_XMDrawSegments(xseg,  nseg, color);
#endif
}

/*--------------------------------------
 *
 * Hv_FillRect
 *
 * A Gateway routine for filling a rect
 * in a specified color.
 *
 *--------------------------------------*/

void	Hv_FillRect(Hv_Rect *rect,
		    short    color) {
  Hv_Rect         psr;
    
  if (color < 0)
    return;
    
  if (Hv_inPostscriptMode) {
    Hv_LocalRectToPSLocalRect(rect, &psr);
    Hv_PSFillRect(&psr, color);
    return;
  }
    
/*
 * If made it here, we are not postscript
 */

#ifdef WIN32
    Hv_W32FillRect(rect, color);
#else
    Hv_XMFillRect(rect,  color);
#endif
}

/**
 * Hv_FillRectangle
 
 * @purpose Fill a rectangle with a given color
 * @param    x          the pixel left coordinate 
 * @param    y          the pixel top coordinate 
 * @param    w          the pixel width coordinate 
 * @param    h          the pixel height coordinate 
 * @param    color      the color index used for filling
 */

void	Hv_FillRectangle(short x,
			 short y,
			 short w,
			 short h,
			 short color) {
    Hv_Rect         r;

    if (color < 0)
      return;
    
    if (Hv_inPostscriptMode) {
	Hv_SetRect(&r, x, y, w, h);
	Hv_LocalRectToPSLocalRect(&r, &r);
	Hv_PSFillRect(&r, color);
	return;
    }

/*
 * If made it here, we are not postscript
 */

#ifdef WIN32
    Hv_W32FillRectangle(x, y, w, h, color);
#else
    Hv_XMFillRectangle(x, y, w, h,  color);
#endif
    
}


/**
 * Hv_FillRectangles
 * @purpose Fill an array of rectangles with a given color 
 * @param    rects      the array of rectangles
 * @param    np          the number of rectangles 
 * @param    color      the color index used for filling
 */

void 	       Hv_FillRectangles(Hv_Rectangle *rects,
				 int           np,
				 short         color) {

  int           i;
  Hv_Rect       r;
  Hv_Rectangle *xr;

  if (Hv_inPostscriptMode) {
    xr = rects;
    for (i = 0; i < np; i++) {
      Hv_SetRect(&r, xr->x, xr->y, xr->width, xr->height);
      Hv_LocalRectToPSLocalRect(&r, &r);
      Hv_PSFillRect(&r, color);
      xr++;
    }
    return;
  }

/*
 * If made it here, we are not postscript
 */

#ifdef WIN32
    Hv_W32FillRectangles(rects, np, color);
#else
    Hv_XMFillRectangles(rects, np, color);
#endif

}



/**
 * Hv_FrameRectangle
 * @purpose Frame a rectangle with a given color
 * @param    x          the pixel left coordinate 
 * @param    y          the pixel top coordinate 
 * @param    w          the pixel width coordinate 
 * @param    h          the pixel height coordinate 
 * @param    color      the color index used for framing
 */

void Hv_FrameRectangle (short x,
			short y,
			short w,
			short h,
			short color) {
    
    Hv_Rect         r;
    
    if (Hv_inPostscriptMode) {
	Hv_SetRect(&r, x, y, w, h);
	Hv_LocalRectToPSLocalRect(&r, &r);
	Hv_PSFrameRect(&r, color);
	return;
    }
    
/*
 * If made it here, we are not postscript
 */

#ifdef WIN32
    Hv_W32FrameRectangle(x, y, w, h, color);
#else
    Hv_XMFrameRectangle(x, y, w, h,  color);
#endif
    
    
}


/*--------------------------------------
 *
 * Hv_FrameRect
 *
 * A Gateway routine for framing a rect
 * in a specified color.
 *
 *--------------------------------------*/

void Hv_FrameRect (Hv_Rect *rect,
		   short    color)
    
{
    Hv_Rect         psr;

    if (color < 0)
      return;
    
    if (Hv_inPostscriptMode) {
	Hv_LocalRectToPSLocalRect(rect, &psr);
	Hv_PSFrameRect(&psr, color);
	return;
    }
    
/*
 * If made it here, we are not postscript
 */

#ifdef WIN32
    Hv_W32FrameRect(rect, color);
#else
    Hv_XMFrameRect(rect,  color);
#endif
}


/**
 * Hv_DrawLine 
 * @purpose  draws a line from point to point in specified color.
 * @param    x1       x pixel of start
 * @param    y1       y pixel of start
 * @param    x2       x pixel of end
 * @param    y2       y pixel of end
 * @param    color   index of line color
 */

void	Hv_DrawLine(short x1,
		    short y1,
		    short x2,
		    short y2,
		    short color) {
  if (Hv_inPostscriptMode) {
    Hv_LocalToPSLocal(x1, y1, &x1, &y1);
    Hv_LocalToPSLocal(x2, y2, &x2, &y2);
    Hv_PSDrawLine(x1, y1, x2, y2, color);
    return;
  }
    
/*
 * If made it here, we are not postscript
 */

#ifdef WIN32
    Hv_W32DrawLine(x1, y1, x2, y2, color);
#else
    Hv_XMDrawLine(x1, y1, x2, y2, color);
#endif
}


/**
 * Hv_SimpleDrawString    draws a string at a specified location. The
 * x and y parameters give the TOP-LEFT which is unconventional
 * (sorry about that) so be careful
 * @param   x           pixel location of left edge
 * @param   y           pixel location of top edge
 * @param   str         Hv_String to be drawn
 */

void    Hv_SimpleDrawString(short x,
			    short y,
			    Hv_String *str) {

    float             extrah = 0.0;
    float             extraw = 0.0;
    float             fontsize, fx, fy;
    char              *fontname;
    Boolean           shadow;
    short             sw, sh;		        /* String width  in pixels */


    if ((str == NULL) || (str->text == NULL))
	return;

    if (Hv_inPostscriptMode) {

	Hv_CompoundStringDimension(str, &sw, &sh);

	fontname = (char *)Hv_Malloc(80);
	Hv_PSFont(str->font, fontname, &fontsize);
	
	shadow = False;
	
	if (str->horizontal) {
	    extrah = (float)(sh*0.75*Hv_psResolutionFactor*72.0/Hv_PSPIXELDENSITY);
	    Hv_LocalToPSLocal(x, y, &x, &y);
	    Hv_PSLocalToPS(x, y, &fx, &fy);
	    Hv_PSDrawString(fx, fy - extrah, fontsize, fontname, str->text,
			    Hv_PSHORIZONTAL, str->strcol, shadow, Hv_white);
	}
	else  {  /* vertical string */
	    extrah = (float)(sh*0.25*Hv_psResolutionFactor*72.0/Hv_PSPIXELDENSITY);
	    extraw = (float)(sw*Hv_psResolutionFactor*72.0/Hv_PSPIXELDENSITY);
	    Hv_LocalToPSLocal((short)(x+sh), y, &x, &y);
	    Hv_PSLocalToPS(x, y, &fx, &fy);
	    Hv_PSDrawString(fx - extrah, fy - extraw, fontsize, fontname, str->text,
			    Hv_PSVERTICAL, str->strcol, False, Hv_white);
	}
	Hv_Free(fontname);
	return;
    }


#ifdef WIN32
    Hv_W32SimpleDrawString(x, y, str);
#else
    Hv_XMSimpleDrawString(x, y, str);
#endif

}

 
/**
 * Hv_DrawString 
 * @purpose   Draws a string at a specified location. The
 * x and y parameters give the TOP-LEFT which is unconventional
 * (sorry about that) so be careful
 * @param   x           pixel location of left edge
 * @param   y           pixel location of top edge
 * @param   str         Hv_String to be drawn
 */

void    Hv_DrawString(short       x,
		      short       y,
		      Hv_String  *str) {

    Hv_StringDraw(x, y, str, NULL);
}


/**
 * Hv_StringDraw 
 * @purpose   Draws a string at a specified location. The
 * x and y parameters give the TOP-LEFT which is unconventional
 * (sorry about that) so be careful
 * @param   x           pixel location of left edge
 * @param   y           pixel location of top edge
 * @param   str         Hv_String to be drawn
 * @param   region      a clipping region (safe to pass NULL) 
 * @deprecated
 */


void    Hv_StringDraw(short x,
		      short y,
		      Hv_String *str,
		      Hv_Region region) {
		      

    short             w, h;		        /* String width  in pixels */
    char              *fontname;
    float             fontsize, fx, fy;
    float             extrah = 0.0;
    float             extraw = 0.0;

/* x and y represent the LEFT TOP of the string */
    
    if ((str == NULL) || (str->text == NULL))
	return;

    Hv_StringDimension(str, &w, &h);
    
    
    if (Hv_inPostscriptMode) {
	fontname = (char *)Hv_Malloc(80);
	Hv_PSFont(str->font, fontname, &fontsize);
	
	
	if (str->horizontal) {
	    extrah = (float)(h*0.75*Hv_psResolutionFactor*72.0/Hv_PSPIXELDENSITY);
	    Hv_LocalToPSLocal(x, y, &x, &y);
	    Hv_PSLocalToPS(x, y, &fx, &fy);
	    Hv_PSDrawString(fx,
			    fy - extrah,
			    fontsize,
			    fontname,
			    str->text,
			    Hv_PSHORIZONTAL,
			    str->strcol,
			    False,
			    Hv_white);
	}
	else  {
	    extrah = (float)(h*0.25*Hv_psResolutionFactor*72.0/Hv_PSPIXELDENSITY);
	    extraw = (float)(w*Hv_psResolutionFactor*72.0/Hv_PSPIXELDENSITY);
	    Hv_LocalToPSLocal((short)(x+h), y, &x, &y);
	    Hv_PSLocalToPS(x, y, &fx, &fy);
	    Hv_PSDrawString(fx - extrah,
			    fy - extraw,
			    fontsize,
			    fontname,
			    str->text,
			    Hv_PSVERTICAL,
			    str->strcol,
			    False,
			    Hv_white);
	}
	Hv_Free(fontname);
	return;
    }

/*
 * If made it here, we are not postscript
 */

#ifdef WIN32
    Hv_W32DrawString(x, y, str, region);
#else
    Hv_XMDrawString(x, y, str, region);
#endif
}



/*==========================================
 * UTILITY SECTION OF GATEWAY
 *===========================================*/

/**
 * Hv_GetXEventX
 * @purpose   To get the x position from the event
 * @param     event     The event in question
 * @return    The x pixel location
 */

short Hv_GetXEventX(Hv_XEvent *event) {
#ifdef WIN32
    return Hv_W32GetXEventX(event);
#else
    return Hv_XMGetXEventX(event);
#endif
}


/**
 * Hv_GetXEventY
 * @purpose   To get the y position from the event
 * @param     event     The event in question
 * @return    The y pixel location
 */

short Hv_GetXEventY(Hv_XEvent *event) {
#ifdef WIN32
    return Hv_W32GetXEventY(event);
#else
    return Hv_XMGetXEventY(event);
#endif
}


/**
 * Hv_GetXEventButton 
 * @purpose   To get which button caused event
 * @param     event     The event in question
 * @return    The mouse button that caused the event
 */

short Hv_GetXEventButton(Hv_XEvent *event) {
#ifdef WIN32
    return Hv_W32GetXEventButton(event);
#else
    return Hv_XMGetXEventButton(event);
#endif
}


/**
 * Hv_AddModifier
 * @purpose Very limited: add a modifier into an event, usually to "trick" something.
 * @param    event    The event to change
 * @param    modbit   The bit to add (e.g. Hv_SHIFT)
 */

void Hv_AddModifier(Hv_XEvent  *event,
		    int         modbit) {
#ifdef WIN32
    Hv_W32AddModifier(event, modbit);
#else
    Hv_XMAddModifier(event, modbit);
#endif
}

/**
 * Hv_Shifted
 * @purpose   To see if a given event was modified by a shift
 * @param     event     The event to test
 * @return    True if shift was held down
 */

Boolean Hv_Shifted(Hv_XEvent  *event) {
#ifdef WIN32
    return Hv_W32Shifted(event);
#else
    return Hv_XMShifted(event);
#endif
}


/**
 * Hv_AltPressed
 * @purpose   To see if a given event was modified by an "alt".
 * @param     event     The event to test
 * @return    True if alt was held down
 */


Boolean Hv_AltPressed(Hv_XEvent  *event) {
#ifdef WIN32
    return Hv_W32AltPressed(event);
#else
    return Hv_XMAltPressed(event);
#endif
}


/**
 * Hv_ControlPressed
 * @purpose   To see if a given event was modified by a "control".
 * @param     event     The event to test
 * @return    True if control was held down
 */


Boolean Hv_ControlPressed(Hv_XEvent  *event) {
#ifdef WIN32
    return Hv_W32ControlPressed(event);
#else
    return Hv_XMControlPressed(event);
#endif
}



/**
 * Hv_Pause 
 * @purpose Pause for the given interval (in milliseconds).
 * @param      interval     pause time (ms)
 * @param      region       restores this as clipping region (NULL safe).
 */

 void  Hv_Pause(int    interval,
		Hv_Region region) {
#ifdef WIN32
    Hv_W32Pause(interval, region);
#else
    Hv_XMPause(interval, region);
#endif
 }

/**
 * Hv_SysBeep 
 * @purpose  A simple system beep for diagnostics
 */

void  Hv_SysBeep(void) {
#ifdef WIN32
    Hv_W32SysBeep();
#else
    Hv_XMSysBeep();
#endif
}

/**
 * Hv_Flush 
 * @purpose    Flush the queue of all requests.
 */

 void  Hv_Flush(void) {
#ifdef WIN32
    Hv_W32Flush();
#else
    Hv_XMFlush();
#endif
 }

/**
 * Hv_Sync 
 * @purpose    Synchronize the queue.
 */

 void  Hv_Sync(void) {
#ifdef WIN32
    Hv_W32Sync();
#else
    Hv_XMSync();
#endif
 }


/**
 * Hv_SetCursor 
 * @purpose  Change the cursor
 * @param   curs   the cursor to set
 */


 void  Hv_SetCursor(Hv_Cursor curs) {
#ifdef WIN32
    Hv_W32SetCursor(curs);
#else
    Hv_XMSetCursor(curs);
#endif
 }

/**
 * Hv_SetString
 * @purpose To put text into a widget (usually a TextEntry)
 */

void Hv_SetString(Hv_Widget w,
		  char      *s) {

  char *es = "";

  if (w == NULL)
    return;

  if (s == NULL)
    s = es;

#ifdef WIN32
    Hv_W32SetString(w, s);
#else
    Hv_XMSetString(w, s);
#endif
}


/**
 * Hv_GetString
 * @purpose To extract the text from a widget (usually a TextEntry)
 * @return  The text
 */

char  *Hv_GetString(Hv_Widget w) {
  if (w == NULL)
    return NULL;

#ifdef WIN32
    return Hv_W32GetString(w);
#else
    return Hv_XMGetString(w);
#endif

}

/**
 * Hv_Parent
 * @purpose To return the parent of a widget
 * @param    w    The widget in question
 * @return  The parent (might be NULL)
 */

Hv_Widget  Hv_Parent(Hv_Widget w) {
    if (w == NULL)
	return NULL;

#ifdef WIN32
    return Hv_W32Parent(w);
#else
    return Hv_XMParent(w);
#endif
}



/**
 * Hv_RemoveAllCallbacks
 * @purpose Remove all callbacks of a certain type registered by a widget
 * @param    w    The widget in question
 * @param    name The name of the callback to remove
 */

void  Hv_RemoveAllCallbacks(Hv_Widget w,
			    char *name) {

    if ((w == NULL) || (name == NULL))
	return;

#ifdef WIN32
    Hv_W32RemoveAllCallbacks(w, name);
#else
    Hv_XMRemoveAllCallbacks(w, name);
#endif

}


/**
 * Hv_ManangeChild  
 * @purpose  Manage a child widget
 * @param     w        The widget which is being managed
 */

void  Hv_ManageChild(Hv_Widget w) {
#ifdef WIN32
    Hv_W32ManageChild(w);
#else
    Hv_XMManageChild(w);
#endif
}


/**
 * Hv_UnmanangeChild 
 * @purpose Unmanage a child widget
 * @param     w        The widget which is being unmanaged
 */

void  Hv_UnmanageChild(Hv_Widget w) {
#ifdef WIN32
    Hv_W32UnmanageChild(w);
#else
    Hv_XMUnmanageChild(w);
#endif
}


/**
 * Hv_AddCallback 
 * @purpose Add a callback to a widget
 * @param     w        The widget which is getting the callback
 * @param     cbname   String name of the callback
 * @param     cbproc   Callback process
 * @param     data     Client data
 */

void  Hv_AddCallback(Hv_Widget       w,
		     char           *cbname,
		     Hv_CallbackProc cbproc,
		     Hv_Pointer      data) {
#ifdef WIN32
    Hv_W32AddCallback(w, cbname, cbproc, data);
#else
    Hv_XMAddCallback(w, cbname, cbproc, data);
#endif
}


/**
 * Hv_DestroyWidget 
 * @purpose Frees memory associated with a widget
 * @param   w    The widget to free (NULL safe).
 */


void  Hv_DestroyWidget(Hv_Widget w) {
#ifdef WIN32
    Hv_W32DestroyWidget(w);
#else
    Hv_XMDestroyWidget(w);
#endif
}


/**
 * Hv_SetSensitivity 
 * @purpose Sets the sensitivity for widget
 * @param  w     The widget in question
 * @param  val   If True, widget is enabled, if False, ghosted.
 */

void Hv_SetSensitivity(Hv_Widget  w,
		       Boolean    val) {

  if (w == NULL) {
    return;
  }

#ifdef WIN32
    Hv_W32SetSensitivity(w, val);
#else
    Hv_XMSetSensitivity(w, val);
#endif
}

/**
 * Hv_IsSensitive 
 * @purpose  Checks if a widget is selectable (sensitive)
 * @param  w     The widget in question
 * @return True if selectable
 */

Boolean Hv_IsSensitive(Hv_Widget  w) {

#ifdef WIN32
    return Hv_W32IsSensitive(w);
#else
    return Hv_XMIsSensitive(w);
#endif
}


/**
 * Hv_SetWidgetBackgroundColor 
 * @purpose Sets the background color of a widget.
 * @param      w        the widget
 * @param      color    the color index
 */

void Hv_SetWidgetBackgroundColor(Hv_Widget   w,
				 short       color)  {
#ifdef WIN32
    Hv_W32SetWidgetBackgroundColor(w, color);
#else
    Hv_XMSetWidgetBackgroundColor(w, color);
#endif
}


/**
 * Hv_SetWidgetForegroundColor 
 * @purpose Sets the foreground color of a widget.
 * @param      w        the widget
 * @param      color    the color index
 */

void Hv_SetWidgetForegroundColor(Hv_Widget   w,
				 short       color) {
#ifdef WIN32
    Hv_W32SetWidgetForegroundColor(w, color);
#else
    Hv_XMSetWidgetForegroundColor(w, color);
#endif
}


/**
 * Hv_ClickType 
 * @purpose  Determines if we single or double clicked
 * @param      Item    The item we clicked on
 * @param      which button to test
 * @return     0 for undetermined, 1 for single click, 2 for double
 */


int Hv_ClickType(Hv_Item Item,
		 short  button) {
#ifdef WIN32
    return Hv_W32ClickType(Item, button);
#else
    return Hv_XMClickType(Item, button);
#endif
}


/**
 * Hv_Println 
 * @purpose  Prints a line of text. Will AUTOMATICALLY
 * append a newline.
 * @param       fmt    the format
 * param        ...    variable length argument list
 */


extern void           Hv_Println(char *fmt, ...) {
    va_list      args;

    va_start(args, fmt);
    
#ifdef WIN32
    Hv_W32Println(fmt, args);
#else
    Hv_XMPrintln(fmt, args);
#endif

    va_end(args);
}


/*==========================================
 * COLOR RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_MakeColorMap 
 * @purpose   Creates the applications colormap
 */


void Hv_MakeColorMap(void) {
#ifdef WIN32
    Hv_W32MakeColorMap();
#else
    Hv_XMMakeColorMap();
#endif
}


/**
 * Hv_GetForeground 
 * @purpose Get the foreground pixel.
 * @return  the foreground pixel.
 */

unsigned long   Hv_GetForeground(void) {
#ifdef WIN32
    return Hv_W32GetForeground();
#else
    return Hv_XMGetForeground();
#endif
}

/**
 * Hv_GetBackground 
 * @purpose Get the background pixel.
 * @return  the background pixel.
 */

unsigned long   Hv_GetBackground(void) {
#ifdef WIN32
    return Hv_W32GetBackground();
#else
    return Hv_XMGetBackground();
#endif
}


/**
 * Hv_NearestColor 
 * @purpose Find the pixel value of the nearest color
 * @param   r      red component
 * @param   g      green component
 * @param   b      blue component
 * @return  the pixel of the nearest color in the colormap
 */

unsigned long   Hv_NearestColor(unsigned short r,
				unsigned short g,
				unsigned short b) {
#ifdef WIN32
    return Hv_W32NearestColor(r, g, b);
#else
    return Hv_XMNearestColor(r, g, b);
#endif
}


/**
 * Hv_DarkenColor 
 * @purpose Darkens a color
 * @param   color  index to the color in question
 * @param   del    amount each component is darkened
 */

void            Hv_DarkenColor(short color,
			       unsigned short del) {
#ifdef WIN32
    Hv_W32DarkenColor(color, del);
#else
    Hv_XMDarkenColor(color, del);
#endif
}

/**
 * Hv_BrightenColor 
 * @purpose Brightens a color
 * @param   color  index to the color in question
 * @param   del    amount each component is darkened
 */

void            Hv_BrightenColor(short color,
				 unsigned short del) {
#ifdef WIN32
    Hv_W32BrightenColor(color, del);
#else
    Hv_XMBrightenColor(color, del);
#endif
}

/**
 * Hv_AdjustColor 
 * @purpose Adjusts a color
 * @param   color  index to the color in question
 * @param   r      change in red component
 * @param   g      change in green component
 * @param   b      change in blue component
 */

void            Hv_AdjustColor(short color,
			       short r,
			       short g,
			       short b) {
#ifdef WIN32
    Hv_W32AdjustColor(color, r, g, b);
#else
    Hv_XMAdjustColor(color, r, g, b);
#endif
}


/**
 * Hv_ChangeColor 
 * @purpose Redefines a color
 * @param   color  index to the color in question
 * @param   r      new red component
 * @param   g      green green component
 * @param   b      new blue component
 */

void            Hv_ChangeColor(short color,
			       unsigned short r,
			       unsigned short g,
			       unsigned short b) {
#ifdef WIN32
    Hv_W32ChangeColor(color, r, g, b);
#else
    Hv_XMChangeColor(color, r, g, b);
#endif
}


/**
 * Hv_CopyColor 
 * @purpose  Copies a color
 * @param     dcolor   the destination index
 * @param     scolor   the source index
 */

void  Hv_CopyColor(short dcolor,
		   short scolor) {
#ifdef WIN32
    Hv_W32CopyColor(dcolor, scolor);
#else
    Hv_XMCopyColor(dcolor, scolor);
#endif
}

/**
 * Hv_GetRGBFromColor
 * @purpose  Given color index, return the r g b components
 * @param     color   index into the Hv color array
 * @param     r       upon return, the red component
 * @param     g       upon return, the green component
 * @param     b       upon return, the blue component
 */

void Hv_GetRGBFromColor(short color,
						unsigned short *r,
						unsigned short *g,
						unsigned short *b) {
#ifdef WIN32
    Hv_W32GetRGBFromColor(color, r, g, b);
#else
    Hv_XMGetRGBFromColor(color, r, g, b);
#endif
}


/**
 * Hv_QueryColor 
 * @purpose  Given the pixel stored in xcolor, this
 * will put the exact rgb components into the same structure (xcolor)
 * @param     dpy     the display
 * @param     cmap    the color map
 * @param     xcolor  the color structure
 */

void  Hv_QueryColor(Hv_Display   *dpy,
		    Hv_ColorMap   cmap,
		    Hv_Color     *xcolor) {
#ifdef WIN32
    Hv_W32QueryColor(dpy, cmap, xcolor);
#else
    Hv_XMQueryColor(dpy, cmap, xcolor);
#endif
}


/*==========================================
 * FILESELECTION RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_DoubleFileSelect
 * @purpose A Fileselector that allows two file selections at once
 * @param title    Overall title of the dialog
 * @param prompt1  Prompt for the left fileselector
 * @param prompt2  Prompt for the right fileselector
 * @param mask1    Mask for the left fileselector (e.g. "*.doc")
 * @param mask2    Mask for the right fileselector
 * @param fname1   Returns with name of file selected in left fileselector
 * @param fname2   Returns with name of file selected in right fileselector
 */

void Hv_DoubleFileSelect(char *title,
			 char *prompt1,
			 char *prompt2,
			 char *mask1, 
			 char *mask2, 
			 char **fname1, 
			 char **fname2) {
#ifdef WIN32
    Hv_W32DoubleFileSelect(title, prompt1, prompt2, mask1, mask2, fname1, fname2);
#else
    Hv_XMDoubleFileSelect(title, prompt1, prompt2, mask1, mask2, fname1, fname2);
#endif
}

/**
 * Hv_FileSelect
 * @purpose        To select a file
 * @param prompt   Prompt for the fileselector
 * @param mask     Mask for fileselector (e.g. "*.doc")
 * @param deftext  Default text in file name text area
 * @return The fully qualified name of the selected file
 */

char *Hv_FileSelect(char *prompt,
		    char *mask,
		    char *deftext) {
#ifdef WIN32
    return Hv_W32FileSelect(prompt, mask, deftext);
#else
    return Hv_XMFileSelect(prompt, mask, deftext);
#endif
}

/**
 * Hv_DirectorySelect
 * @purpose       To select a directory
 * @param prompt  A string prompt (e.g., "Pick a directory");
 * @param mask    A mask on the directory names
 * @return The fully qualified name of the selected directory
 */

char *Hv_DirectorySelect(char *prompt,
			 char *mask) {
#ifdef WIN32
    return Hv_W32DirectorySelect(prompt, mask);
#else
    return Hv_XMDirectorySelect(prompt, mask);
#endif
}

/**
 * Hv_GenericFileSelect
 * @purpose        To select a file (ore general form of Hv_FileSelect)
 * @param prompt   Prompt for the fileselector
 * @param mask     Mask for fileselector (e.g. "*.doc")
 * @param filemask Can be used to select files or directories
 * @param deftext  Default text in file name text area
 * @param parent   A parent widget for the resulting dialog
 * @return The fully qualified name of the selected file
 */

char *Hv_GenericFileSelect(char         *prompt,
			   char         *mask,
			   unsigned char filemask,
			   char         *deftext,
			   Hv_Widget     parent) {
#ifdef WIN32
    return Hv_W32GenericFileSelect(prompt, mask, filemask, deftext, parent);
#else
    return Hv_XMGenericFileSelect(prompt, mask, filemask, deftext, parent);
#endif
}
/**
 * Hv_GenericFileSave
 * @purpose        To select a file (ore general form of Hv_FileSelect)
 * @param prompt   Prompt for the fileselector
 * @param mask     Mask for fileselector (e.g. "*.doc")
 * @param filemask Can be used to select files or directories
 * @param deftext  Default text in file name text area
 * @param parent   A parent widget for the resulting dialog
 * @return The fully qualified name of the selected file
 *
 * Note: For XM this function is the same as calling Hv_GenericFileSelect
 */

char *Hv_GenericFileSave(char         *prompt,
			   char         *mask,
			   unsigned char filemask,
			   char         *deftext,
			   Hv_Widget     parent) {
#ifdef WIN32
    return Hv_W32GenericFileSave(prompt, mask, filemask, deftext, parent);
#else
    return Hv_XMGenericFileSelect(prompt, mask, filemask, deftext, parent);
#endif
}


/*==========================================
 * FONT RELATED SECTION OF GATEWAY
 *===========================================*/


/**
 * Hv_InitFonts
 * @purpose Internal use only. Initializes fonts at startup.
 */

void Hv_InitFonts(void) {
  int    i;

  Hv_fixed  = Hv_fixed11;
  Hv_fixed2 = Hv_fixedBold11;

  for (i = 0; i < Hv_NUMFONTS; i++)
    Hv_fontLoaded[i] = False;

/* now some specific initializations */

#ifdef WIN32
    Hv_W32InitFonts();
#else
    Hv_XMInitFonts();
#endif

}

/**
 * Hv_AddNewFont
 * @purpose Loads a new font that is now needed. Internal
 * use only.
 * @param font   The index of the font to load, if necessary.
 */

void Hv_AddNewFont(short font) {
#ifdef WIN32
    Hv_W32AddNewFont(font);
#else
    Hv_XMAddNewFont(font);
#endif
}



/*==============================================
 * TEXTENTRY WIDGET  RELATED SECTION OF GATEWAY
 *==============================================*/


/**
 * Hv_TextEntryItemInitialize
 * @purpose  Initialization of the text entry item
 * @param     Item        The wheel item
 * @param     attributes  The attribute array
 * @note Specific attributes (in addition to those common to all items)
 * @table
 * |R Attribute        | Type           | Default                 | Comment |r
 * |R Hv_FILLCOLOR     | short          | none                    | Background golor |r
 * |R Hv_SINGLECLICK   | Hv_FunctionPtr | Hv_TextEntrySingleClick | Handles cursor positioning |r
 * |R Hv_TEXTENTRYMODE | int            | Hv_SINGLELINEEDIT       | Other choice is Hv_MULTILINEEDIT |r
 * |R Hv_VALUECHANGED  | Hv_FunctionPtr | NULL                    | Callback as the value changes |r
 * @endtable
 */

void Hv_TextEntryItemInitialize(Hv_Item           Item,
				Hv_AttributeArray attributes) {
#ifdef WIN32
    Hv_W32TextEntryItemInitialize(Item, attributes);
#else
    Hv_XMTextEntryItemInitialize(Item, attributes);
#endif
}



/**
 * Hv_TextEntrySingleClick
 * @purpose Handle response to a click in a TextEntry item. The underlying
 * widget will have to "popped" in place.
 * @param hvevent     The causal event
 */


void Hv_TextEntrySingleClick(Hv_Event hvevent) {
#ifdef WIN32
    Hv_W32TextEntrySingleClick(hvevent);
#else
    Hv_XMTextEntrySingleClick(hvevent);
#endif
}

/**
 * Hv_Set3DFrame
 * @purpose  Configure the 3D frame to match the Hv View
 * @param    View   The View to match.
 */


void Hv_Set3DFrame(Hv_View View) {

    if (View == NULL)
	return;

#ifdef WIN32
    Hv_W32Set3DFrame(View);
#else
    Hv_XMSet3DFrame(View);
#endif
}


/*==============================================
 * DIALOG  RELATED SECTION OF GATEWAY
 *==============================================*/

/**
 * Hv_OpenDialog
 * @purpose Opens the dialog box and starts its message loop
 * @param   w       The dialog (widget)
 * @param   def_btn  button that will close the box e.g. Hv_OK or Hv_DONE.
 */

void Hv_OpenDialog(Hv_Widget dlog,
		   Hv_Widget def_btn)

{
  Hv_DialogData  ddata;
  Hv_allowMag = False;

  ddata = Hv_GetDialogData(dlog);

/* stop any old timer, not a likely possibility */

  Hv_StopDialogTimer(dlog);

  Hv_ManageChild(dlog);

/* added to fix lesstif problem and in strict accordance with
   Motif programming manual */

  Hv_PopupDialog(dlog);

  ddata->opened = True;
  ddata->answer = Hv_RUNNING;

/* if def_btn is NULL, set it to the dialogs ok button */

  if (def_btn == NULL)
    def_btn = Hv_GetDialogActionButton(dlog, Hv_OKBUTTON);

}

/**
 * Hv_CloseOutCallback
 * @purpose Callback for one of the "action" buttons that closes the dialog.
 * @param   w       The button (widget)
 * @param   answer  e.g. Hv_OK or Hv_DONE.
 */

void  Hv_CloseOutCallback(Hv_Widget w,
			  int       answer) {

#ifdef WIN32
    Hv_W32CloseOutCallback(w,answer,NULL);
#else
    Hv_XMCloseOutCallback(w,answer);
#endif
}


/* ------- Hv_DoDialog --------*/

int  Hv_DoDialog(Hv_Widget dialog,
		 Hv_Widget def_btn)

{
#ifdef WIN32
    return Hv_W32DoDialog(dialog,def_btn);
#else
    return Hv_XMDoDialog(dialog,def_btn);
#endif
}

/**
 * @purpose Ask a simple yes/no question
 * @param   question    The question string
 * @return  nonzero if the user selects "Yes"
 */


int Hv_Question(char *question) {

	if (question == NULL)
		return 0;

#ifdef WIN32
    return Hv_W32Question(question);
#else
    return Hv_XMQuestion(question);
#endif
}

/**
 * Hv_Information
 * @purpose A informational dialog.
 * @param message The text of the information.
 */


void Hv_Information(char *message) {

	if (message == NULL)
		return;

#ifdef WIN32
    Hv_W32Information(message);
#else
    Hv_XMInformation(message);
#endif
}

/**
 * Hv_Warning
 * @purpose A warning dialog.
 * @param message The text of the warning.
 */


void Hv_Warning(char *message) {

	if (message == NULL)
		return;

#ifdef WIN32
    Hv_W32Warning(message);
#else
    Hv_XMWarning(message);
#endif
}

/**
 * Hv_CreateModalDialog
 * @purpose Create a modal dialog
 * @param  title       The dialog title
 * @param  ignoreclose If nonzero, the window manager's close button is ignored
 * @return the dialog, cast as an Hv_Widget
 */


Hv_Widget Hv_CreateModalDialog(char *title,
			       int  ignoreclose) {
#ifdef WIN32
    return Hv_W32CreateModalDialog(title, ignoreclose);
#else
    return Hv_XMCreateModalDialog(title, ignoreclose);
#endif

}

/**
 * Hv_CreateModalDialog
 * @purpose Create a modal dialog
 * @param  title       The dialog title
 * @param  ignoreclose If nonzero, the window manager's close button is ignored
 * @return the dialog, cast as an Hv_Widget
 */

Hv_Widget Hv_CreateModelessDialog(char *title,
				  int   ignoreclose) {
#ifdef WIN32
    return Hv_W32CreateModelessDialog(title, ignoreclose);
#else
    return Hv_XMCreateModelessDialog(title, ignoreclose);
#endif
}


/**
 * Hv_EditPattern
 * @purpose Edit a pattern
 * @param   pattern    The pattern to edit
 * @param   prompt     A prompt in the dialog
 * @return  0 if the pattern was NOT changed, 1 if it was.
 */

static   Hv_Widget   newpatternlabel, oldpatternlabel;
static   short    newpattern;

/* ------ Hv_SetWidgetPattern --- */

static void Hv_SetWidgetPattern (Hv_Widget w,
	   	                 int       pnumber
		                 ) {
    newpattern = (short) pnumber;
    Hv_ChangeLabelPattern(newpatternlabel, newpattern);
}


int  Hv_EditPattern(short *pattern,
		    char  *prompt) {
    
     
  static Hv_Widget        dialog = NULL;
  static Hv_Widget        mynewpattern, myoldpattern;
  
  Hv_Widget            rowcol, rc, dummy;

  short                dx  = 16;  /*width  of pattern label */
  short                dy  = 16;  /*height of pattern label */
  Hv_CompoundString             title;
  int                  i, answer;

  if (prompt == NULL)
	return 0;

/* returns 0 if pattern was not changed */

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

      Hv_VaSetValues(dummy, Hv_NlabelType, Hv_PIXMAP, NULL);

      Hv_AddEventHandler(dummy,
			 Hv_buttonPressMask,
			 (Hv_EventHandler)Hv_SetWidgetPattern,
			 (Hv_Pointer)i);  
    

      Hv_ChangeLabelPattern(dummy, (short)i);
    }

    rc = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON + Hv_CANCELBUTTON);
  }

  newpatternlabel = mynewpattern;
  oldpatternlabel = myoldpattern;

/* the dialog has been created */

  newpattern = *pattern;
  
  if (prompt) {
    title = Hv_CreateSimpleCompoundString(prompt);
    Hv_VaSetValues(dialog, Hv_NdialogTitle, title, NULL);
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

    
#ifdef WIN32
    return Hv_W32EditPattern(pattern, prompt);
#else
    return Hv_XMEditPattern(pattern, prompt);
#endif
}


/**
 * Hv_ChangeLabel
 * @purpose change a widget (typically a button) label
 * @param    w      The widget to modify
 * @param    label  The new label
 * @param    font   The font to use
 */

void  Hv_ChangeLabel(Hv_Widget w,
		     char      *label,
			 short      font) {


#ifdef WIN32
    Hv_W32ChangeLabel(w, label, font);
#else
    Hv_XMChangeLabel(w, label, font);
#endif
}



/**
 * Hv_CreateThinOptionMenu
 * @purpose  Convenience function for creating an option menu popup
 * on a dialog.
 * @param rc The rowcolumn parent
 * @param font Usually Hv_fixed2
 * @param title Menu title
 * @param numitems Number of items
 * @param items Array of Items (can be the string "Hv_SEPARATOR")
 * @param CB Common callback for all items
 * @param margin Affects margins around menu on a dialog
 * @return   The popup menu widget
 */


Hv_Widget Hv_CreateThinOptionMenu(Hv_Widget      rc,
				  short          font,
				  char          *title,
				  int            numitems,
				  char          *items[],
				  Hv_FunctionPtr CB,
				  short          margin) {

    if ((numitems < 1) || (items == NULL))
	return NULL;

#ifdef WIN32
    return Hv_W32CreateThinOptionMenu(rc, font, title, numitems, items, CB, margin);
#else
    return Hv_XMCreateThinOptionMenu(rc, font, title, numitems, items, CB, margin);
#endif
}



/**
 * Hv_GetOwnerDialog
 * @purpose Return the dialog that "owns" this widget, even if
 * it is not the immediate parent.
 * @param   ditem    The dialog Item (widget)
 * @return  The dialog owner (or NULL)
 */


Hv_Widget       Hv_GetOwnerDialog(Hv_Widget ditem) {

    if (ditem == NULL)
	return NULL;

#ifdef WIN32
    return Hv_W32GetOwnerDialog(ditem);
#else
    return Hv_XMGetOwnerDialog(ditem);
#endif
}



/**
 * Hv_ActionAreaDialogItem
 * @purpose Create an Action Area (very similar to a Close-Out item), which is a row of up to four
 * predefined closout buttons: OK, Cancel, Apply, and Delete.
 * @param parent   The parent widget (either a dialog or a rowcol)
 * @param attributes The attribute array
 * @return The Action Area dialog item
 * <p>
 * Relevant attributes are
 * @table
 * |R Attribute      | Type    | Default       | Comment |r
 * |R Hv_DIALOGCLOSEOUT | int     | Hv_OKBUTTON + Hv_CANCELBUTTON | Other available bits are Hv_APPLYBUTTON and Hv_DELETEBUTTON |r
 * |R Hv_LABEL          | char *  | NULL          | label   |r
 * @endtable
 */

Hv_Widget       Hv_ActionAreaDialogItem(Hv_Widget         parent,
					Hv_AttributeArray attributes) {

    if ((parent == NULL) || (attributes == NULL))
	return NULL;

#ifdef WIN32
    return Hv_W32ActionAreaDialogItem(parent, attributes);
#else
    return Hv_XMActionAreaDialogItem(parent, attributes);
#endif
}

/**
 * Hv_ButtonDialogItem
 * @purpose Create a normal dialog button with a callback.
 * @param parent   The parent widget (either a dialog or a rowcol)
 * @param attributes The attribute array
 * @return The button dialog item
 * <p>
 * Relevant attributes are
 * @table
 * |R Attribute      | Type           | Default       | Comment             |r
 * |R Hv_CALLBACK       | Hv_FunctionPtr | NULL          | Callback function   |r
 * |R Hv_DATA           | Hv_Pointer     | NULL          | User data           |r
 * |R Hv_FONT           | short          | Hv_fixed2     | Button font         |r
 * |R Hv_LABEL          | char *         | NULL          | Button label        |r
 * @endtable
 */

Hv_Widget       Hv_ButtonDialogItem(Hv_Widget         parent,
				    Hv_AttributeArray attributes) {
    if ((parent == NULL) || (attributes == NULL))
	return NULL;

#ifdef WIN32
    return Hv_W32ButtonDialogItem(parent, attributes);
#else
    return Hv_XMButtonDialogItem(parent, attributes);
#endif
}

/**
 * Hv_CloseOutDialogItem
 * @purpose Create a Close-Out item, which is a row of up to four
 * predefined closout buttons: OK, Cancel, Apply, and Delete.
 * @param parent   The parent widget (either a dialog or a rowcol)
 * @param attributes The attribute array
 * @return The close-out dialog item
 * <p>
 * Relevant attributes are
 * @table
 * |R Attribute      | Type    | Default       | Comment |r
 * |R Hv_DIALOGCLOSEOUT | int     | Hv_OKBUTTON + Hv_CANCELBUTTON | Other available bits ar Hv_APPLYBUTTON and Hv_DELETEBUTTON |r
 * |R Hv_FONT           | short          | Hv_fixed2     | Button font         |r
 * |R Hv_LABEL          | char *         | NULL          | Label               |r
 * |R Hv_ORIENTATION    | short          | Hv_HORIZONTAL | Other choice: Hv_VERTICAL |r
 * |R Hv_SPACING        | short          | 1             | Button spacing         |r
 * @endtable
 */

Hv_Widget       Hv_CloseOutDialogItem(Hv_Widget         parent,
				      Hv_AttributeArray attributes) {
    if ((parent == NULL) || (attributes == NULL))
	return NULL;

#ifdef WIN32
    return Hv_W32CloseOutDialogItem(parent, attributes);
#else
    return Hv_XMCloseOutDialogItem(parent, attributes);
#endif
}

/**
 * Hv_ColorSelectorDialogItem
 * @purpose Create a color selector
 * @param parent   The parent widget (either a dialog or a rowcol)
 * @param attributes The attribute array
 * @return The color selector dialog item
 * <p>
 * There are presentlty no recognized attributes for this item, however
 * we have kept the attributes array as an argument for future growth.
 */

Hv_Widget       Hv_ColorSelectorDialogItem(Hv_Widget         parent,
					   Hv_AttributeArray attributes) {
    if ((parent == NULL) || (attributes == NULL))
	return NULL;

#ifdef WIN32
    return Hv_W32ColorSelectorDialogItem(parent, attributes);
#else
    return Hv_XMColorSelectorDialogItem(parent, attributes);
#endif
}

/**
 * Hv_FileSelectorDialogItem
 * @purpose Create a file selector widget
 * @param parent   The parent widget (either a dialog or a rowcol)
 * @param attributes The attribute array
 * @return The file selector dialog item
 * <p>
 * Relevant attributes are
 * @table
 * |R Attribute      | Type    | Default       | Comment |r
 * |R Hv_MASK        | char *  | NULL          | Filter, e.g., "*.txt" |r
 * @endtable
 */

Hv_Widget       Hv_FileSelectorDialogItem(Hv_Widget         parent,
					  Hv_AttributeArray attributes) {
    if ((parent == NULL) || (attributes == NULL))
	return NULL;

#ifdef WIN32
    return Hv_W32FileSelectorDialogItem(parent, attributes);
#else
    return Hv_XMFileSelectorDialogItem(parent, attributes);
#endif
}

/**
 * Hv_LabelDialogItem
 * @purpose Create a label
 * @param parent   The parent widget (either a dialog or a rowcol)
 * @param attributes The attribute array
 * @return The label dialog item
 * <p>
 * Relevant attributes are
 * @table
 * |R Attribute      | Type    | Default       | Comment |r
 * |R Hv_BACKGROUND     | short          | white     | Background color         |r
 * |R Hv_BORDERCOLOR    | short          | Hv_gray5  | Border color         |r
 * |R Hv_BORDERWIDTH    | short          | 0         | Border pixel width         |r
 * |R Hv_FONT           | short          | Hv_fixed2     | Button font         |r
 * |R Hv_LABEL          | char *         | NULL          | Label               |r
 * @endtable
 */

Hv_Widget       Hv_LabelDialogItem(Hv_Widget         parent,
				   Hv_AttributeArray attributes) {
    if ((parent == NULL) || (attributes == NULL))
	return NULL;

#ifdef WIN32
    return Hv_W32LabelDialogItem(parent, attributes);
#else
    return Hv_XMLabelDialogItem(parent, attributes);
#endif
}

/**
 * Hv_ListDialogItem
 * @purpose Create a list for a dialog
 * @param parent   The parent widget (either a dialog or a rowcol)
 * @param attributes The attribute array
 * @return The list dialog item
 * <p>
 * Relevant attributes are
 * @table
 * |R Attribute           | Type    | Default       | Comment |r
 * |R Hv_CALLBACK         | Hv_FunctionPtr | NULL            | Callback function when list item is selected  |r
 * |R Hv_LIST             | char **        | NULL            | Array of initial list items
 * |R Hv_LISTSELECTION    | char           | Hv_SINGLELISTSELECT | Other option is Hv_MULTIPLELISTSELECT.
 * |R Hv_LISTRESIZEPOLICY | char           | Hv_VARIABLE     | Other options: Hv_CONSTANT or Hv_RESIZE_IF_POSSIBLE
 * |R Hv_NUMITEMS         | short          | 0               | Number of items in the list         |r
 * |R Hv_NUMVISITEMS      | short          | 0               | Number of items in the list that should be visible |r
 * @endtable
 */

Hv_Widget       Hv_ListDialogItem(Hv_Widget         parent,
				  Hv_AttributeArray attributes) {
    if ((parent == NULL) || (attributes == NULL))
	return NULL;

#ifdef WIN32
    return Hv_W32ListDialogItem(parent, attributes);
#else
    return Hv_XMListDialogItem(parent, attributes);
#endif
}


/**
 * Hv_RadioDialogItem
 * @purpose Create a radio set for a dialog 
 * @param parent   The parent widget (either a dialog or a rowcol)
 * @param attributes The attribute array
 * @return The radio dialog item
 * <p>
 * Relevant attributes are
 * @table
 * |R Attribute      | Type    | Default       | Comment |r
 * |R Hv_CALLBACK    | Hv_FunctionPtr  | NULL          | Callback function when radio button is selected  |r
 * |R Hv_DATA        | Hv_Pointer      | NULL          | User data           |r
 * |R Hv_FONT        | short           | Hv_fixed2     | Button font         |r
 * |R Hv_LABEL       | char *          | NULL          | Label               |r
 * |R Hv_NOWON       | short           | 1             | Which button (1, ... N) is on  |r
 * |R Hv_RADIOLIST   | Hv_RadioHeadPtr | NULL          | A list of mutually exclusive radio buttons |r
 * @endtable
 */

Hv_Widget       Hv_RadioDialogItem(Hv_Widget         parent,
				   Hv_AttributeArray attributes) {
    if ((parent == NULL) || (attributes == NULL))
	return NULL;

#ifdef WIN32
    return Hv_W32RadioDialogItem(parent, attributes);
#else
    return Hv_XMRadioDialogItem(parent, attributes);
#endif
}

/**
 * Hv_ScaleDialogItem
 * @purpose Create a scale widget for a dialog
 * @param parent   The parent widget (either a dialog or a rowcol)
 * @param attributes The attribute array
 * @return The scale dialog item
 * <p>
 * Relevant attributes are
 * @table
 * |R Attribute       | Type    | Default       | Comment |r
 * |R Hv_CALLBACK     | Hv_FunctionPtr  | NULL          | Callback function when scale is adjusted  |r
 * |R Hv_CURRENTVALUE | int             | 50            | Current (starting) value of scale |r
 * |R Hv_MINVALUE     | int             | 0             | Minimum value of scale |r
 * |R Hv_MAXVALUE     | int             | 100           | Maximum value of scale |r
 * |R Hv_ORIENTATION  | short           | Hv_HORIZONTAL | Other choice: Hv_VERTICAL |r
 * @endtable
 */

Hv_Widget       Hv_ScaleDialogItem(Hv_Widget         parent,
				   Hv_AttributeArray attributes) {
    if ((parent == NULL) || (attributes == NULL))
	return NULL;

#ifdef WIN32
    return Hv_W32ScaleDialogItem(parent, attributes);
#else
    return Hv_XMScaleDialogItem(parent, attributes);
#endif
}
/**
 * Hv_ScrollBarDialogItem
 * @purpose Create a scroll bar 
 * @param parent   The parent widget (either a dialog or a rowcol)
 * @param attributes The attribute array
 * @return The scrollbar dialog item
 * <p>
 * There are presentlty no recognized attributes specific for this item, however
 * we have kept the attributes array as an argument for future growth.
 * ar present the scrollbar is vertical in orientation
 */

Hv_Widget Hv_ScrollBarDialogItem(Hv_Widget         parent,
			Hv_AttributeArray attributes	 ) {

    if (parent == NULL)
	return NULL;

#ifdef WIN32
    return Hv_W32ScrollBarDialogItem(parent, attributes);
#else
    return NULL;
/*    return Hv_XMScrollBarDialogItem(parent, attributes);*/
#endif
}

/**
 * Hv_SeparatorDialogItem
 * @purpose Create a separator
 * @param parent   The parent widget (either a dialog or a rowcol)
 * @param attributes The attribute array
 * @return The separatoe dialog item
 * <p>
 * There are presentlty no recognized attributes specific for this item, however
 * we have kept the attributes array as an argument for future growth.
 */

Hv_Widget       Hv_SeparatorDialogItem(Hv_Widget         parent,
				       Hv_AttributeArray attributes) {

    if (parent == NULL)
	return NULL;

#ifdef WIN32
    return Hv_W32SeparatorDialogItem(parent, attributes);
#else
    return Hv_XMSeparatorDialogItem(parent, attributes);
#endif
}

/**
 * Hv_TextDialogItem
 * @purpose Create a text item
 * @param parent   The parent widget (either a dialog or a rowcol)
 * @param attributes The attribute array
 * @return The text dialog item
 * <p>
 * Relevant attributes are
 * @table
 * |R Attribute      | Type    | Default       | Comment |r
 * |R Hv_BACKGROUND     | short          | white     | Background color         |r
 * |R Hv_BORDERCOLOR    | short          | Hv_gray5  | Border color         |r
 * |R Hv_BORDERWIDTH    | short          | 0         | Border pixel width         |r
 * |R Hv_EDITABLE       | Hv_Pointer     | NULL      | User data           |r
 * |R Hv_FONT           | short          | Hv_fixed2 | Toggle button font         |r
 * |R Hv_NUMCOLUMNS     | short          | 1         | Number of text columns         |r
 * |R Hv_TEXTCOLOR      | short          | black     | Text color         |r
 * |R Hv_VALUECHANGED   | Hv_FunctionPtr | NULL      | Callback function for each keystroke as text is entered  |r
 * @endtable
 */

Hv_Widget       Hv_TextDialogItem(Hv_Widget         parent,
				  Hv_AttributeArray attributes) {
    if ((parent == NULL) || (attributes == NULL))
	return NULL;

#ifdef WIN32
    return Hv_W32TextDialogItem(parent, attributes);
#else
    return Hv_XMTextDialogItem(parent, attributes);
#endif
}

/**
 * Hv_ToggleDialogItem
 * @purpose Create a toggle button
 * @param parent   The parent widget (either a dialog or a rowcol)
 * @param attributes The attribute array
 * @return The toggle button dialog item
 * <p>
 * Relevant attributes are
 * @table
 * |R Attribute      | Type    | Default       | Comment |r
 * |R Hv_CALLBACK       | Hv_FunctionPtr | NULL          | Callback function   |r
 * |R Hv_DATA           | Hv_Pointer     | NULL          | User data           |r
 * |R Hv_FONT           | short          | Hv_fixed2     | Toggle button font         |r
 * |R Hv_LABEL          | char *         | NULL          | Toggle button label        |r
 * @endtable
 */

Hv_Widget       Hv_ToggleDialogItem(Hv_Widget         parent,
				    Hv_AttributeArray attributes) {
    if ((parent == NULL) || (attributes == NULL))
	return NULL;

#ifdef WIN32
    return Hv_W32ToggleDialogItem(parent, attributes);
#else
    return Hv_XMToggleDialogItem(parent, attributes);
#endif
}

/**
 * Hv_RowColDialogItem
 * @purpose Create a row-column container widget
 * @param parent   The parent widget (either a dialog or a rowcol)
 * @param attributes The attribute array
 * @return The row-column dialog item
 * <p>
 * Relevant attributes are
 * @table
 * |R Attribute      | Type    | Default       | Comment |r
 * |R Hv_ALIGNMENT   | short   | Hv_BEGINNING  | Other choices: Hv_CENTER and Hv_END |r
 * |R Hv_NUMCOLUMNS  | short   | 1             | Number of columns |r
 * |R Hv_NUMROWS     | short   | 1             | Number of rows |r
 * |R Hv_ORIENTATION | short   | Hv_HORIZONTAL | Other choice: Hv_VERTICAL |r
 * |R Hv_PACKING     | short   | Hv_TIGHT      | Other choice: Hv_PAD |r
 * |R Hv_SPACING     | short   | 1             | Cell spacing |r
 * @endtable
 */

Hv_Widget Hv_RowColDialogItem(Hv_Widget         parent,
				    Hv_AttributeArray attributes) {

    if ((parent == NULL) || (attributes == NULL))
	return NULL;

#ifdef WIN32
    return Hv_W32RowColDialogItem(parent, attributes);
#else
    return Hv_XMRowColDialogItem(parent, attributes);
#endif
}

/**
 * Hv_ScrolledTextDialogItem
 * @purpose Create a scrolled text widget in a dialog
 * @param parent   The parent widget (either a dialog or a rowcol)
 * @param attributes The attribute array
 * @return The scrolled text dialog item
 * <p>
 * Relevant attributes are
 * @table
 * |R Attribute     | Type    | Default  | Comment |r
 * |R Hv_BACKGROUND | short   | white    | Background color |r
 * |R Hv_NUMCOLUMNS | short   | 1        | Number of columns |r
 * |R Hv_NUMROWS    | short   | 1        | Number of rows |r
 * |R Hv_TEXTCOLOR  | short   | black    | Foreground color |r
 * |R Hv_WORDWRAP   | Boolean | False    | If true, word wrap is enabled |r
 * @endtable
 */

Hv_Widget Hv_ScrolledTextDialogItem(Hv_Widget         parent,
				    Hv_AttributeArray attributes) {

    if ((parent == NULL) || (attributes == NULL))
	return NULL;

#ifdef WIN32
    return Hv_W32ScrolledTextDialogItem(parent, attributes);
#else
    return Hv_XMScrolledTextDialogItem(parent, attributes);
#endif
}


/**
 * Hv_GetScaleValue
 * @purpose Get the current value of a scale widget on a dialog
 * @param   w   The scale widget
 * @return  The current value
 */


int  Hv_GetScaleValue(Hv_Widget   w) {
    if (w == NULL)
	return -1;
#ifdef WIN32
    return Hv_W32GetScaleValue(w);
#else
    return Hv_XMGetScaleValue(w);
#endif
}


/**
 * Hv_SetScaleValue
 * @purpose Set the current value of a scale widget on a dialog
 * @param   w   The scale widget
 * @param   val The value to set as the scales's current value
 */


void Hv_SetScaleValue(Hv_Widget   w,
		      int         val) {
    if (w == NULL)
	return;

#ifdef WIN32
    Hv_W32SetScaleValue(w, val);
#else
    Hv_XMSetScaleValue(w, val);
#endif
}

/**
 * Hv_ListItemPos
 * @purpose    Return the position of an item in a list.
 * @param      list      The list widget.
 * @param      str       The item label.
 */

int             Hv_ListItemPos(Hv_Widget list,
			       char     *str) {
#ifdef WIN32
    return Hv_W32ListItemPos(list, str);
#else
    return Hv_XMListItemPos(list, str);
#endif
}
 


/**
 * Hv_CheckListItem
 * @purpose   Sees if given str appears in the list.
   Caller must free poslist.
 * @param     list    The list widget
 * @param     str     The label to match
 * @param     poslist a (Non C-INDEX) array of positions (1..N) where
 * the string was matched.
 * @param     Upon return, the number of matches found.
 * @return    True if found any matches
 */


Boolean  Hv_CheckListItem(Hv_Widget list,
			  char *str,
			  int  **poslist,
			  int  *num) {


    if (list == NULL)
	return False;

#ifdef WIN32
    return Hv_W32CheckListItem(list, str, poslist, num);
#else
    return Hv_XMCheckListItem(list, str, poslist, num);
#endif
}



/**
 * Hv_AddListItem
 * @purpose  Add an item to a list
 * @param    list      The list widget
 * @param    str       The label for the new item
 * @Boolean  selected  If True, the item will be in a selected state
 */


void    Hv_AddListItem(Hv_Widget list,
		       char      *str,
		       Boolean   selected) {
    if (list == NULL)
	return;

#ifdef WIN32
    Hv_W32AddListItem(list, str, selected);
#else
    Hv_XMAddListItem(list, str, selected);
#endif
}

/**
 * Hv_PosReplaceListItem
 * @purpose  Replace item at given position with a new label.
 * <B> The index is not a "C" index, it ranges from 1.. N. </B>
 * @param     list    The list widget
 * @param     pos     The index [1..N]
 * @param     str     The new label
 */


void  Hv_PosReplaceListItem(Hv_Widget list,
			    int       pos,
			    char      *str) {
    if (list == NULL)
	return;

#ifdef WIN32
    Hv_W32PosReplaceListItem(list, pos, str);
#else
    Hv_XMPosReplaceListItem(list, pos, str);
#endif
}

/**
 * Hv_ListItemExists 
 * @purpose Check a list to see if a string exits in the list
 * @param     list    The list widget
 * @param     str     The string to check for 
 */


Boolean Hv_ListItemExists ( Hv_Widget list, char *str )
{
#ifdef WIN32
    if ( str == NULL ) return False;
    return Hv_W32ListItemExists(list,  str);
#else
    XmString xmstr;
    Boolean  retval;
    
    if ( str == NULL ) return False;
    xmstr = XmStringCreateSimple(str);
    retval = XmListItemExists(list, xmstr);
    XmStringFree( xmstr );
    return retval;
#endif

}


/**
 * Hv_PosSelectListItem
 * @purpose Select a list item based on an index.
 * <B> The index is not a "C" index, it ranges from 1.. N. </B>
 * @param     list    The list widget
 * @param     pos     The index [1..N]
 */


void  Hv_PosSelectListItem(Hv_Widget list,
			   int       pos) {
    if (list == NULL)
	return;

#ifdef WIN32
    Hv_W32PosSelectListItem(list, pos);
#else
    Hv_XMPosSelectListItem(list, pos);
#endif
}

/**
 * Hv_NewPosSelectListItem
 * @purpose Select a list item based on an index,
 * and optionally call a callback. <B> The index is
 *  not a "C" index, it ranges from 1.. N. </B>
 * @param     list      The list widget
 * @param     pos       The index [1..N]
 * param      cbflag    If True, callback any registered callback
 */

void  Hv_NewPosSelectListItem(Hv_Widget list,
			      int       pos,
			      Boolean   cbflag) {
    if (list == NULL)
	return;

#ifdef WIN32
    Hv_W32NewPosSelectListItem(list, pos, cbflag);
#else
    Hv_XMNewPosSelectListItem(list, pos, cbflag);
#endif
}

/**
 * Hv_ListSelectItem
 * @purpose   Select the list item whose label matches the given string.
 * @param     list    The list widget
 * @param     str     Text of list item to select (based on a match).
 * @param     call    True is the select callback should be called when the match in found
 */

void Hv_ListSelectItem (Hv_Widget list,
			char     *str,
			Boolean   select)
{
if (list == NULL) return;
#ifdef WIN32
    Hv_W32ListSelectItem(list, str,select);
#else
    Hv_XMListSelectItem(list, str, select);
#endif

}

/**
 * Hv_SelectListItem
 * @purpose   Select the list item whose label matches the given string.
 * @param     list    The list widget
 * @param     str     Text of list item to select (based on a match).
 */


void    Hv_SelectListItem(Hv_Widget list,
			  char     *str) {
        Hv_ListSelectItem(list,str,TRUE);  /* always call the select callback */
}

/**
 * Hv_DeleteAllListItems
 * @purpose   Delete all items in a list.
 * @param     list    The list widget
 */


void    Hv_DeleteAllListItems(Hv_Widget list) {
    if (list == NULL)
	return;

#ifdef WIN32
    Hv_W32DeleteAllListItems(list);
#else
    Hv_XMDeleteAllListItems(list);
#endif
}

/**
 * Hv_DeselectAllListItems
 * @purpose   Deselect all items in a list.
 * @param     list    The list widget
 */


void    Hv_DeselectAllListItems(Hv_Widget list) {
    if (list == NULL)
	return;

#ifdef WIN32
    Hv_W32DeselectAllListItems(list);
#else
    Hv_XMDeselectAllListItems(list);
#endif
}


/**
 * Hv_GetListSelectedPos
 * @purpose Return the index of the selected item.
 * @param     list    The list widget
 * @return  A -1 if no item is selected, otherwise
 * returns the index of the selected item. <B> The index is
 *  not a "C" index, it ranges from 1.. N. </B>
 */


int  Hv_GetListSelectedPos(Hv_Widget list) {

    if (list == NULL)
	return -1;

#ifdef WIN32
    return Hv_W32GetListSelectedPos(list);
#else
    return Hv_XMGetListSelectedPos(list);
#endif
} 


/**
 * Hv_GetAllListSelectedPos
 * @purpose   Replacement for Hv_GetListSelectedPos. This routine
 * returns all selected items so that it can be used
 * in lists where you need to choose more than one.
 * @param     list    The list widget
 * @return    NULL if no selected, otherwise an int array with
 * one enetry for each selected item. The array is terminated
 * by a -1. <B> The indices are not "C" indices, they range from 1.. N. </B>
 */


int  *Hv_GetAllListSelectedPos(Hv_Widget list) {

    if (list == NULL)
	return NULL;

#ifdef WIN32
    return Hv_W32GetAllListSelectedPos(list);
#else
    return Hv_XMGetAllListSelectedPos(list);
#endif
}

/******************************************************************/
/*                                                                */
/* Hv_ListPosSelected() - Returns True if the position indicated  */
/*                        by pos is selected.  If pos is 0, it    */
/*                        returns whether the last item in the    */
/*                        list is selected.                       */
/*                                                                */
/******************************************************************/

Boolean Hv_ListPosSelected(Hv_Widget list, int pos) {

   int *posArray = NULL;   /* Array of selected positions */
   int *posPtr;            /* Pointer into posArray */
   int  nItems;            /* Total number of items in list */

   /* If no list, the position can't be selected */
   if ( list == NULL ) return False;

   /* Negative positions can't be selected, either */
   if ( pos < 0 ) return False;

   /* If testing last item in list */
   if ( pos == 0 )
   {
      /* Get how many items are in the list */
      Hv_VaGetValues( list, Hv_NitemCount, &nItems, NULL );

      /* Set pos to that number */
      pos = nItems;
   }
   /* End if (testing last item in list) */

   /* If pos is still 0, there is nothing in the list */
   if ( pos == 0 ) return False;

   /* Get the array of selected positions */
   posArray = Hv_GetAllListSelectedPos( list );

   /* If there's no list, nothing can be selected */
   if ( posArray == NULL ) return False;

   /* Look through array of selected positions */
   /* (List is terminated by a -1 entry.)      */
   posPtr = posArray;
   while( (*posPtr) != -1 )
   {
      /* If a position in the array matches the one we want */
      if (*posPtr == pos)
      {
         /* Free the position array */
         Hv_Free( posArray );

         /* Return True */
         return True;
      }
      /* End if (found matching position) */

      /* Next position in array */
      posPtr++;
   }
   /* End of loop through array of selected positions */

   /* Free the position array */
   Hv_Free( posArray );

   /* If we had found a match, we wouldn't be here.  Return False */
   return False;
}
/* End Hv_ListPosSelected() */

/**
 * Hv_ChangeLabelPattern 
 * @purpose   Change the background pattern associated with a label widget.
 * @param     w        The label widget
 * @param     pattern  The index for the new background pattern.
 */


void  Hv_ChangeLabelPattern(Hv_Widget w,
			    short     pattern) {

    if ((w == NULL) || (pattern < 0) || (pattern >= Hv_NUMPATTERNS))
	return;

#ifdef WIN32
    Hv_W32ChangeLabelPattern(w, pattern);
#else
    Hv_XMChangeLabelPattern(w, pattern);
#endif
}


/**
 * Hv_AddLineToScrolledText
 * @purpose Insert a line into the text widget
 * @param   w    The text widget
 * @param   pos  The insertion position
 * @param   line The line to add
 */

void  Hv_AddLineToScrolledText(Hv_Widget        w,
			       Hv_TextPosition *pos,
			       char            *line) {
    if (w == NULL)
	return;

#ifdef WIN32
    Hv_W32AddLineToScrolledText(w, pos, line);
#else
    Hv_XMAddLineToScrolledText(w, pos, line);
#endif

}


/**
 * Hv_AppendLineToScrolledText
 * @purpose Append a line to the bottom of the text widget
 * @param   w    The text widget
 * @param   line The line to add
 */


void Hv_AppendLineToScrolledText(Hv_Widget     w,
				 char          *line) {
    if (w == NULL)
	return;

#ifdef WIN32
    Hv_W32AppendLineToScrolledText(w, line);
#else
    Hv_XMAppendLineToScrolledText(w, line);
#endif

}



/**
 * Hv_JumpToBottomOfScrolledText
 * @purpose Move the insertion point to the end of the text widget
 * @param   w    The text widget
 * @param   pos  Upon return, insertion position corresponding to the bottom
 */


void Hv_JumpToBottomOfScrolledText(Hv_Widget        w,
				   Hv_TextPosition *pos) {
    if (w == NULL)
	return;

#ifdef WIN32
    Hv_W32JumpToBottomOfScrolledText(w, pos);
#else
    Hv_XMJumpToBottomOfScrolledText(w, pos);
#endif

}


/**
 * Hv_DeleteLinesFromTop
 * @purpose Delete lines from the top of the text widget
 * @param   w        The text widget
 * @param   nlines   The number of lines to delete off the top
 */


void  Hv_DeleteLinesFromTop(Hv_Widget        w,
			    int              nlines) {
    if (w == NULL)
	return;

#ifdef WIN32
    Hv_W32DeleteLinesFromTop(w, nlines);
#else
    Hv_XMDeleteLinesFromTop(w, nlines);
#endif

}


/**
 * Hv_DeleteScrolledTextLines
 * @purpose Delete lines from the top  scrolled text widget.
 * If nlines < 0, all text up to *endpos is deleted,
 * @param   w        The text widget
 * @param   nlines   Number of lines to delete
 * @param   endpos   Upon return, the adjusted end position 
 */


void  Hv_DeleteScrolledTextLines(Hv_Widget        w,
				 int              nlines,
				 Hv_TextPosition *endpos) {
    if (w == NULL)
	return;

#ifdef WIN32
    Hv_W32DeleteScrolledTextLines(w, nlines, endpos);
#else
    Hv_XMDeleteScrolledTextLines(w, nlines, endpos);
#endif

}


/**
 * Hv_ClearScrolledtext
 * @purpose Clear all the text from the widget 
 * @param   w    The text widget
 * @param   pos  Upon return, the adjusted insertion position
 */


void Hv_ClearScrolledText(Hv_Widget        w,
			  Hv_TextPosition *pos) {
    if (w == NULL)
	return;

#ifdef WIN32
    Hv_W32ClearScrolledText(w, pos);
#else
    Hv_XMClearScrolledText(w, pos);
#endif

}


/**
 * Hv_PopupDialog
 * @purpose  Pop up the dialog
 * @param dialog    The dialog
 */


void Hv_PopupDialog(Hv_Widget dialog) {
    if (dialog == NULL)
	return;

#ifdef WIN32
    Hv_W32PopupDialog(dialog);
#else
    Hv_XMPopupDialog(dialog);
#endif
    
}

/**
 * Hv_PopdownDialog
 * @purpose  Pop down (close) the dialog
 * @param dialog    The dialog
 */

void Hv_PopdownDialog(Hv_Widget dialog) {
    if (dialog == NULL)
	return;

#ifdef WIN32
    Hv_W32PopdownDialog(dialog);
#else
    Hv_XMPopdownDialog(dialog);
#endif

}

/* ------- Hv_StandardDoneButton ------*/

Hv_Widget Hv_StandardDoneButton(Hv_Widget  parent,
				char    *donestr) {
    Hv_Widget   w;
    Hv_Widget   dialog;

    w  = Hv_VaCreateDialogItem(parent,
			       Hv_TYPE,            Hv_BUTTONDIALOGITEM,
			       Hv_FONT,            Hv_fixed2,
			       Hv_LABEL,           donestr,
			       Hv_DATA,            (void *)Hv_DONE,
			       Hv_CALLBACK,        Hv_CloseOutCallback,
			       NULL);
#ifdef WIN32
	dialog = Hv_GetOwnerDialog(w);
    dialog->generic = w;
#endif
   
    return w;
}

/**
 * Hv_OverrideTranslations
 * @purpose To change some keyboard mappings for a widget. This
 * is for internal use only. Probably only need for Motif dialogs
 * to make sure the delete key works in the usual direction (left).
 * @param w      The widget being overridden
 * @param table  The translation table
 */


void Hv_OverrideTranslations(Hv_Widget w,
			     char     *table) {
    if (w == NULL)
	return;

#ifdef WIN32
    Hv_W32OverrideTranslations(w, table);
#else
    Hv_XMOverrideTranslations(w, table);
#endif

}


/*==============================================
 * HELP RELATED SECTION OF GATEWAY
 *==============================================*/

/**
 * Hv_DoHelp
 * @purpose Bring up the online help
 */


void 
        Hv_DoHelp(void) {
#ifdef WIN32
    Hv_W32DoHelp();
#else

    Hv_XMDoHelp();
#endif

}

/*==============================================
 * OPEN GL RELATED SECTION OF GATEWAY
 *==============================================*/

#ifdef Hv_USEOPENGL

/*****************************************************************************
 ____       _                  ____ _ __        ___     _            _   
/ ___|  ___| |_ _   _ _ __    / ___| |\ \      / (_) __| | __ _  ___| |_ 
\___ \ / _ \ __| | | | '_ \  | |  _| | \ \ /\ / /| |/ _` |/ _` |/ _ \ __|
 ___) |  __/ |_| |_| | |_) | | |_| | |__\ V  V / | | (_| | (_| |  __/ |_ 
|____/ \___|\__|\__,_| .__/___\____|_____\_/\_/  |_|\__,_|\__, |\___|\__|
                     |_| |_____|                          |___/          
*****************************************************************************/



/**
 * Hv_SetupGLWidget
 * @purpose Create the OPENGL drawing widget for a view
 * @param   The view in question
 */


void Hv_SetupGLWidget(Hv_View View,char *title) {
    if (View == NULL)
	return;

#ifdef WIN32
    Hv_W32SetupGLWidget(View,title);
#else
    Hv_XMSetupGLWidget(View,title);
#endif

}

#endif

/**
 * Hv_TextInsert
 * @purpose Set text at a given position in a text widget
 * @param  w   The text widget
 * @param  pos  The insertion position
 * @param  line The text to insert
 */


void  Hv_TextInsert(Hv_Widget       w,
		    Hv_TextPosition pos,
		    char           *line) {

    if ((w == NULL) || (line == NULL))
	return;

#ifdef WIN32
    Hv_W32TextInsert(w, pos, line);
#else
    Hv_XMTextInsert(w, pos, line);
#endif
}


/**
 * Hv_SetInsertionPosition
 * @purpose Set the insertion position of a text widget 
 * @param  w    The text widget
 * @param  pos  The insertion position
 */


void  Hv_SetInsertionPosition(Hv_Widget       w,
			      Hv_TextPosition pos) {

    if (w == NULL)
	return;

#ifdef WIN32
    Hv_W32SetInsertionPosition(w, pos);
#else
    Hv_XMSetInsertionPosition(w, pos);
#endif

}


/**
 * Hv_ShowPosition
 * @purpose Make sure the insertion position is visible
 * @param  w    The text widget
 * @param  pos  The insertion position
 */


void  Hv_ShowPosition(Hv_Widget       w,
		      Hv_TextPosition pos) {

    if (w == NULL)
	return;

#ifdef WIN32
    Hv_W32ShowPosition(w, pos);
#else
    Hv_XMShowPosition(w, pos);
#endif

}


/**
 * Hv_GetInsertionPosition
 * @purpose Get the insertion position of a text widget
 * @param  w   The text widget
 * @return The insertion position
 */


Hv_TextPosition  Hv_GetInsertionPosition(Hv_Widget  w) {

    if (w == NULL)
	return -1;

#ifdef WIN32
    return Hv_W32GetInsertionPosition(w);
#else
    return Hv_XMGetInsertionPosition(w);
#endif
}


/**
 * Hv_UpdateWidget
 * @purpose Basically repaint a widget
 * @param   w   The widget to update.
 */

void  Hv_UpdateWidget(Hv_Widget w) {
    if (w == NULL)
	return;
#ifdef WIN32
    Hv_W32UpdateWidget(w);
#else
    Hv_XMUpdateWidget(w);
#endif
}


/**
 * Hv_GetLastPosition
 * @purpose Get the position at the end of the text in a text widget
 * @param   w   The text widget
 * @return  The position at the end
 */

Hv_TextPosition  Hv_GetLastPosition(Hv_Widget w) {
#ifdef WIN32
    return Hv_W32GetLastPosition(w);
#else
    return Hv_XMGetLastPosition(w);
#endif
}


/**
 * Hv_GetToggleButton
 * @purpose Get the state of a toggle button dialog item
 * @param   btn   The toggle dialog widget
 * @return  True if it is "on"
 */



Boolean	Hv_GetToggleButton(Hv_Widget btn) {

    if (btn == NULL)
	return False;

#ifdef WIN32
    return Hv_W32GetToggleButton(btn);
#else
    return Hv_XMGetToggleButton(btn);
#endif
}

/**
 * Hv_SetToggleButton
 * @purpose Get the state of a toggle button dialog item
 * @param   btn   The toggle dialog widget
 */

void	Hv_SetToggleButton(Hv_Widget btn,Boolean   State) {

    if (btn == NULL)
	return;

#ifdef WIN32
    Hv_W32SetToggleButton(btn,State);
#else
    Hv_XMSetToggleButton(btn,State);
#endif
}


/**
 * Hv_Pic8ToImage
 * @purpose Covert a raw 8 bit picture stored in a byte array
 * into an Hv_Image
 * @param pic8 The raw picture data
 * @param wide width inpixels
 * @param high height in pixels
 * @param mycolors colormap pixels
 * @param rmap red   components
 * @param gmap blue  components
 * @param bmap green components
 * @return The Hv_Image (or NULL on failure)
 */


Hv_Image *Hv_Pic8ToImage(byte *pic8,
			 int   wide,
			 int   high,
			 unsigned long **mycolors,
			 byte *rmap,
			 byte *gmap,
			 byte *bmap) {


    if ((pic8 == NULL) ||
	(mycolors == NULL) ||
	(rmap == NULL) ||
	(gmap == NULL) ||
	(bmap == NULL))
	
	return NULL;
    
#ifdef WIN32
    return Hv_W32Pic8ToImage(pic8, wide, high, mycolors, rmap, gmap, bmap);
#else
    return Hv_XMPic8ToImage(pic8, wide, high, mycolors, rmap, gmap, bmap);
#endif
}

/**
 * Hv_GrabPointer
 * @purpose  Grab the pointer for current shell
 */

void            Hv_GrabPointer() {
#ifdef WIN32
    Hv_W32GrabPointer();
#else
    Hv_XMGrabPointer();
#endif
}


/**
 * Hv_UngrabPointer
 * @purpose  Release the pointer
 */

void            Hv_UngrabPointer() {
#ifdef WIN32
    Hv_W32UngrabPointer();
#else
    Hv_XMUngrabPointer();
#endif
}
/**
 * Hv_opendir
 * @purpose  returns the first file in a directory scan
 * @param    dirname    The full path name of the directory
 * @param    extension  The extension to mask for
 * @param    fp         The returned file pointer
 * @param    fname      The returned file name
 * @return   True if a file was found
 */



 Boolean  Hv_GetNextFile(DIR *dp,
			char *dirname,
			char *extension,
			FILE **fp,
			char *fname) {

#ifdef WIN32
    return Hv_W32GetNextFile(dp, dirname, extension, fp, fname);
#else
    return Hv_XMGetNextFile(dp, dirname, extension, fp, fname);
#endif
 }


/**
 * Hv_OpenDirectory
 * @purpose  Open a directory
 * @param    dname   The full path name of the directory
 * @return   a dir structure suitable for looping through the files in this dir
 */
 
 DIR   *Hv_OpenDirectory(char *dname) {
	 if (dname == NULL)
		 return NULL;

#ifdef WIN32
    return Hv_W32OpenDirectory(dname);
#else
    return Hv_XMOpenDirectory(dname);
#endif
}

/**
 * Hv_CloseDirectory
 * @purpose  Open a directory
 * @param    dp   handle returned by Hv_OpenDirectory
 * @return   success of failure
 */
 
int   Hv_CloseDirectory(DIR *dp) {

#ifdef WIN32
    return Hv_W32CloseDirectory(dp);
#else
    return Hv_XMCloseDirectory(dp);
#endif
}

