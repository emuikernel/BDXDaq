/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov

*** version 10058
      -> save configuration option
      -> tiled printing finished

*** version 10057
      -> repairs text items woes

*** version 10056
      -> minor mods for map views

*** version 10055
      -> minor mods for map views

*** version 10054
      -> minor mods to polygon for map views
      -> full screen print has tile option

*** version 10053
      -> fixed memory leak in wedge
      -> fixed unlock when item was deleted by a dialog

*** version 10052
      -> fixed bug in Hv_DeleteItem so if view is not
         visible it wont bomb
      -> add the LOCKED draw control bit

*** version 10051 
      -> added WorldWedge item

*** version 10050
      -> added void * userptr2 to item data structure
      -> changed Hv_Malloc to use calloc (with zeroes)
      -> fixes to LED item so that it used TEXTCOLOR attribute
      -> fixes for connections to use 4 multiplexes
      -> added Hv_CreateDrawingItemCB, a global function ptr that if not
         NULL (its default) will be called after an item is created with
	 a drawing tool but before it is drawn

*** version 10049
      -> placed under CVS
      -> added the Hv_SelfDestructDialog
      -> add the Hv_VIEWFINALINIT attribute useful for prezooming

*** version 10048
     -> changed choice drawing from diamonds to circles
     -> fixed a bug in the virtual view that sometimes required a refresh

*** version 10047
     used Maurik Holtrop's fix for high end displays with lots o' colors

==================================================================================
*/

#include "Hv.h"
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <ctype.h>
 
#ifdef sparc
extern int gethostname( char *name, int namelen);
#endif
#ifdef __linux__
extern int gethostname( char *name, int namelen);
#endif

/*---- local prototypes -----*/

static void     Hv_CreateWelcomeView(void);

static void	Hv_AddEventHandlers(void);

static void     Hv_HandleKeyRelease(Widget,
				    XtPointer,
				    XKeyEvent *);

static void     Hv_HandleExpose(Widget,
				XtPointer,
				XEvent *);

static int      Hv_Initialize(unsigned int,
			      char **,
			      char *);
     
static void     Hv_MainInit(void);

static void	Hv_InitControls(void);

static void     Hv_InitColors(void);

static void     Hv_InitCanvas(void);

static void     Hv_InitGlobalWidgets(void);

static void     Hv_InitWindow(void);

static void     Hv_InitGraphics(void);

static void     Hv_InitPatterns(void);

static void     Hv_CanvasResizeCB(Widget,
				  XtPointer,
				  XtPointer);

static void     Hv_InitAppName(char **,
			       char *);

static void     Hv_CreateGraphicsContext(void);


static void     Hv_LastStageInitialization(void);

static XtActionsRec canvasactions[] = {
  {"Hv_PopupMenuShell", (XtActionProc)Hv_PopupMenuShell},
};


static char  canvastranstable[] = 
   "<Btn3Down> : Hv_PopupMenuShell()";

/* the predefined Hv patterns originate in the
   foloowing 16x16 bitmaps */

static unsigned char pat0[] = {  /* black */
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static unsigned char pat1[] = { /* 80% */
  0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
  0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
  0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa};

static unsigned char pat2[] = { /* 50 % */
  0x33, 0x33, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xcc, 0x33, 0x33, 0x33, 0x33,
  0xcc, 0xcc, 0xcc, 0xcc, 0x33, 0x33, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xcc,
  0x33, 0x33, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xcc};

static unsigned char pat3[] = {  /* 20% */
  0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00};
  
static unsigned char pat4[] = { /* hatch */
  0x11, 0x11, 0xb8, 0xb8, 0x7c, 0x7c, 0x3a, 0x3a, 0x11, 0x11, 0xa3, 0xa3,
  0xc7, 0xc7, 0x8b, 0x8b, 0x11, 0x11, 0xb8, 0xb8, 0x7c, 0x7c, 0x3a, 0x3a,
  0x11, 0x11, 0xa3, 0xa3, 0xc7, 0xc7, 0x8b, 0x8b};

static unsigned char pat5[] = { /* diagonal 1*/
  0x3c, 0x3c, 0x78, 0x78, 0xf0, 0xf0, 0xe1, 0xe1, 0xc3, 0xc3, 0x87, 0x87,
  0x0f, 0x0f, 0x1e, 0x1e, 0x3c, 0x3c, 0x78, 0x78, 0xf0, 0xf0, 0xe1, 0xe1,
  0xc3, 0xc3, 0x87, 0x87, 0x0f, 0x0f, 0x1e, 0x1e};

static unsigned char pat6[] = { /*diagonal 2*/
  0x1e, 0x1e, 0x0f, 0x0f, 0x87, 0x87, 0xc3, 0xc3, 0xe1, 0xe1, 0xf0, 0xf0,
  0x78, 0x78, 0x3c, 0x3c, 0x1e, 0x1e, 0x0f, 0x0f, 0x87, 0x87, 0xc3, 0xc3,
  0xe1, 0xe1, 0xf0, 0xf0, 0x78, 0x78, 0x3c, 0x3c};

static unsigned char pat7[] = { /* hstripe */
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


static unsigned char pat8[] = {  /*vstripe*/
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};

static unsigned char pat9[] = {  /* scales */
  0x10, 0x10, 0x10, 0x10, 0x28, 0x28, 0xc7, 0xc7, 0x01, 0x01, 0x01, 0x01,
  0x82, 0x82, 0x7c, 0x7c, 0x10, 0x10, 0x10, 0x10, 0x28, 0x28, 0xc7, 0xc7,
  0x01, 0x01, 0x01, 0x01, 0x82, 0x82, 0x7c, 0x7c};

static unsigned char pat10[] = {  /* stars */
  0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x88, 0x08, 0x90, 0x04, 0xa0, 0x02,
  0x40, 0x01, 0x3e, 0x3e, 0x40, 0x01, 0xa0, 0x02, 0x90, 0x04, 0x88, 0x08,
  0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00};

static unsigned char pat11[] = {  /* white */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


static short              Hv_mwLeft;       /* from attributes to Hv_VaInitialize */
static short              Hv_mwTop;        /* from attributes to Hv_VaInitialize */
static short              Hv_mwWidth;      /* from attributes to Hv_VaInitialize */
static short              Hv_mwHeight;     /* from attributes to Hv_VaInitialize */
static short             *Hv_mwFillColor;  /* from attributes to Hv_VaInitialize */

XFontStruct     *fontfixed = NULL;
XFontStruct     *fontfixed2 = NULL;

/*------- global variables --------*/

/* these used to be macros, but for consistency changed to vars.
   they still masquerade as macros */

float Hv_RADTODEG;
float Hv_DEGTORAD;

int             Hv_units = Hv_KILOMETERS;       /* of use in map views */

Boolean         Hv_usedNewDrawTools = False; /* used to distinguish old & new drawing tools */
Boolean         Hv_okSetColor = True;  /* used in conjunction with pats in PS */


char          **Hv_configuration = NULL;        /* null terminated string array holding current config file */
Hv_FunctionPtr  Hv_UserReadConfiguration = NULL;   /* if !null, config file (if found) is passed here */
Hv_FunctionPtr  Hv_UserWriteConfiguration = NULL;   /* if !null, app can write some to config file */

Hv_FunctionPtr  Hv_CreateDrawingItemCB = NULL;  /* if not null, called after a drawing tool created an item */
Hv_FunctionPtr  Hv_UserDrawSymbolsOnPoints;
Boolean         Hv_dontSendBack = False;   /*prevent view from being sent back on shift click*/
unsigned char * Hv_bitmaps[Hv_NUMPATTERNS];
Hv_Pixmap       Hv_patterns[Hv_NUMPATTERNS];

Boolean         Hv_quoteDelimitsSegment = False;
short           Hv_lastDraggedVertex = -1;  /*related to changing world poly */
short           Hv_simpleTextBg;
short           Hv_simpleTextFg;
short           Hv_simpleTextBorder;

Hv_Widget       Hv_lastLabel = NULL;
Boolean         Hv_drawingOffScreen = False;
char            *Hv_cachedDirectory = NULL;
char            **Hv_vaText = NULL;
int             *Hv_vaTags = NULL;
int             *Hv_vaFonts = NULL;
int             *Hv_vaColors = NULL;
short           Hv_vaNumOpt = 0;

Boolean         Hv_feedbackWhileSelecting = False;
Hv_DialogData   Hv_dialogList = NULL;
Boolean         Hv_drawingOn = True;
int             Hv_worldLinePolicy = Hv_NORESTRICTION;
Hv_Item         Hv_printThisPlotOnly = NULL;
Boolean         Hv_useDrawImage = False;
Boolean         Hv_trapCommandArgs = False;
char           *Hv_leftOverArgs = NULL;

/* try another approach, namely do NOT concatenate
   left over args for easier processing */

int             Hv_argc = 0;
char           **Hv_argv = NULL;

short           Hv_fancySymbolLite;
Hv_Item         Hv_activeTextItem = NULL;

int             Hv_theEnlargedPolyNp = 0;
Hv_Point        *Hv_theEnlargedPoly = NULL;         /* poly being enlarged */
Hv_FunctionPtr  Hv_UserLineEnlargeCheck = NULL;     /* called when enlarging poly*/
Hv_FunctionPtr  Hv_FinalUserInit = NULL;
Hv_FunctionPtr  Hv_DuringRectDrag = NULL;
Hv_FunctionPtr  Hv_DanglingTextItemCallback = NULL;
int             Hv_screenResolution;               /* pixels per inch*/
Boolean         Hv_printAllColorsBlack;
float           Hv_sciNotHiLimit;
float           Hv_sciNotLowLimit;
Boolean         Hv_firstExposure;
short		Hv_root_width;		/* width of display in pixels */
short		Hv_root_height;		/* height of display in pixels */
Boolean         Hv_simpleSymbols = False;
int             Hv_version = 10058;    /*10000 * version*/

Hv_FunctionPtr  Hv_userItemName;
Hv_FunctionPtr  Hv_FunctionKeyCallback[9];
Hv_FunctionPtr  Hv_HighlightDraw = NULL;
Widget          Hv_quitItem;
Widget          Hv_quitPopupItem;
Boolean         Hv_libMode = False;    /* set to true if using Hv to build another library */
Boolean         Hv_libDone = False;
Boolean         Hv_handlingExposure;
Hv_Item         Hv_excludeFromExposure = NULL;
unsigned int    Hv_tileX, Hv_tileY;
Hv_FunctionPtr  Hv_TileDraw;
XtAppContext    Hv_appContext = NULL;
Boolean         Hv_fKeysActive;         /* if true, fkeys provide diagnostic messages */
Boolean         Hv_addWelcomeRainbow;   /* if false, no welcome rainbow */ 
Boolean         Hv_offsetDueToScroll;   /* indicates a call to OffSetItem is due to a scroll, not moving a view */
short           Hv_scrollDH;            /* used as hint that redraw is result of scroll */
short           Hv_scrollDV;            /* used as hint that redraw is result of scroll */
Boolean         Hv_offsetDueToDrag;     /* indicates a call to OffSetItem is due to a drag, not moving a view */ 


unsigned int    Hv_activePointers;      /* total # active pointers malloced via malloc & Hv_CreateRegion*/

Display        *Hv_display = NULL;	/* what display (i.e. cpu) is used */
Window		Hv_mainWindow = 0;    	/* main application window */
Window		Hv_trueMainWindow = 0;  /* unchanging (even while off screen draws) */
Cursor          Hv_defaultCursor;       /* default cursor */
Cursor          Hv_crossCursor;         /* crosshair cursor */
Cursor          Hv_waitCursor;          /* wait cursor */
Cursor          Hv_dragCursor;          /* for dragging a view*/
Cursor          Hv_bottomRightCursor;   /* for resizing a view*/
Cursor          Hv_dotboxCursor;        /* a box with a dot in the middle */
char           *Hv_hostName;            /* e.g. "clas01" */
char           *Hv_userName;            /* e.g. "yomama" */

/* function pointers for specialized use responses following
   some action */

Hv_FunctionPtr       Hv_CustomizeQuickZoom;  /*user provided to modify quickzoom labels */
Hv_FunctionPtr       Hv_QuickZoom;           /*should be redirected to app specific*/
Hv_FunctionPtr       Hv_UserZoom;            /*can be used for specialized zoom AFTER standard zoom*/
Hv_FunctionPtr       Hv_UserMove;            /*can be used for specialized zoom AFTER standard move*/
Hv_FunctionPtr       Hv_UserResize;          /*can be used for specialized zoom AFTER standard resize*/
Hv_FunctionPtr       Hv_ClickIntercept;      /*if set, a click is redirected directly to this function*/
Hv_FunctionPtr       Hv_AfterItemRotate;     /*called after usual rotate*/
Hv_FunctionPtr       Hv_AfterTextEdit = NULL;       /*called after a text edit*/

Hv_FunctionPtr       Hv_UserDefaultWorld = NULL;    /*to intercept standard default world */

Boolean   Hv_dontZoom;        /* disable zoom when tool is selected */
Boolean   Hv_timedRedraw;

/* Pointer Stuff */

Hv_Item         Hv_hotItem;        /* item selected by button press */
Hv_View         Hv_hotView;        /* View selected by button press */

Boolean         Hv_completeFeedback;      /*set to false for faster feedback */
Boolean         Hv_extraFastPlease;

char           *Hv_ioPaths[Hv_NUMSEARCHPATH + 1];

/* Global Screen Vars */

unsigned int	Hv_colorDepth;		/* color depth of this monitor */
short		Hv_mainWidth;		/* width of main window in pixels */
short		Hv_mainHeight;		/* height of main window in pixels */
unsigned long	Hv_mainFg;		/* foreground color of main window */
Hv_Rect    	Hv_canvasRect;	        /* canvas geometery */

/* postscript global variables */

Boolean         Hv_inPostscriptMode;        /* True only during postscript drawing */

FILE            *Hv_psfp;                   /* postscript file pointer */

int             Hv_psOrientation;             /* landscape or portrait */
char           *Hv_psDefaultFile;

Hv_FRect        Hv_psWorld;                 /*world system for postscript page, in 72nds of inch*/
Hv_Rect         Hv_psLocal;                 /*local system for postscript page, in PS pixels*/
float           Hv_psResolutionFactor;      /* how much better out PS screen is than the real screen*/
float           Hv_psUnitsPerInch;          /*default = 72 -> one unit = 1/72inch*/

Boolean         Hv_psWholeScreen;           /* True to capture the whole screen */

short           Hv_psCurrentColor;
char            *Hv_psFileName;             /* file name of postscript file */
Boolean         Hv_psFirstPattern;          /* to delay pattern init until necessary */
Boolean         Hv_psnomoveto = False;           /* used to align compound strings */
short           Hv_psrelx = 0;               /* for relative offset (compound strings) */
short           Hv_psrely = 0;               /* for relative offset (compound strings) */
Boolean         Hv_psUseFooter = True;      /* use footer on printout? */
char            *Hv_psNote;                 /* a extra string added at the bottom */

Boolean         Hv_psEntireView;
Boolean         Hv_psTile;
Boolean         Hv_psExpandToFit;        /* expand a small picture? */


/* Color Stuff */

Colormap	Hv_colorMap;	      /* this program's color map */
short		Hv_rainbowFirst;      /* index of first color used for "thermal" plots*/
short		Hv_rainbowLast;	      /* index of last color used for "thermal" plots*/
short           Hv_numColors;
unsigned long	Hv_colors[Hv_EXTENDEDCOLORS]; /* actual colors (pixel values into the colormap) */

/* define some constant colors-- these correspond to indices
   in the colors array, i.e. colors[Hv_green] means that
   colors[Hv_green].pixel is the corresponding pixel in
   the color map */

short           Hv_scrollInactiveColor;       /* for inactive scroll arrows */
short           Hv_scrollActiveColor;         /* for active scroll arrows */
short           Hv_scrollBarColor;    /*color used by active scroll bars*/


short           Hv_hotFont = 0;     /* used for selecting fonts from a menu */
short           Hv_hotFontFamily = 0; /* used for selecting fonts from a menu */
short           Hv_hotFontSize = 0;   /* used for selecting fonts from a menu */
short           Hv_hotFontStyle = 0;  /* used for selecting fonts from a menu */
short           Hv_hotSymbol = 0;   /* used for selecting symbols from a menu */
short           Hv_hotStyle = 0;
short           Hv_hotArrowStyle = 0;

/* Global graphics variables */

GC		Hv_gc;	              /* main graphics context */
View_ListHead	Hv_views;	      /* bookkeeper for our View list */

/* Global Widgets */

Widget		Hv_toplevel;	      /* highest level widget  (Shell widget) */
Widget		Hv_form;              /* form widget for main window */
Widget		Hv_canvas;	      /* main drawing area */
Widget		Hv_menuBar;	      /* Main Menu bar in main window */


Hv_View         Hv_welcomeView = NULL;
Hv_View         Hv_virtualView = NULL;

/* standard menus */

Widget   Hv_helpMenu = NULL;
Widget   Hv_actionMenu = NULL;
Widget   Hv_viewMenu = NULL;
Widget	 Hv_popupMenu = NULL;
Widget	 Hv_popupMenu2 = NULL;
Widget   Hv_quickZoomSubmenu = NULL;
Widget   Hv_undoSubmenu = NULL;

Widget   Hv_popupItems[Hv_NUMPOPUP]; 
Widget   Hv_quickZoomItems[Hv_NUMQUICKZOOM]; 

BalloonData     Hv_theBalloon;                      /* one global balloon for ced */
char           *Hv_balloonStrs[Hv_MAXBALLOONSTR];   /* balloon displayer breaks down the one giant string int these bite sized pieces */
XmString        Hv_balloonXmst[Hv_MAXBALLOONSTR];   /* Motif strings for balloon text */
Boolean         Hv_showBalloons;                    /* if true, some items will display "balloon" help */
Boolean         Hv_balloonUp;                       /* program name */
char           *Hv_programName;
char           *Hv_versionString;
char           *Hv_appName;


/*----- variables global to this file only ------*/

Dimension       tlx, tly, tlw, tlh;
Arg		tlargs[5];


/* fall back resources if no resource file is found */

  static char *fallbackresources[] = {


/************************************************
 The next block (ifndef __sgi) has been commented
 out (5/16/96) in an effort to solve the problem
 of the wayward delete experienced at U Va. It
 has bee replaced with mods to Hv_CreateManagedTextField
 in Hv_dlogs.c
***********************************************/

/*
#ifndef __sgi
    "*defaultVirtualBindings:\
       osfLeft : <Key>Left \\n\
       osfRight : <Key>Right \\n\
       osfUp : <Key>Up \\n\
       osfDown : <Key>Down \\n\
       osfBackSpace : <Key>Delete ",
#endif
*/
    "*allowShellResize: true",
    "*borderWidth: 0",
    "*highlightThickness: 2",
    "*traversalOn: true",
    "*background: lightseagreen",
    "*foreground: white",
    "*canvas.background: cornflowerblue",
    "*popup.background: DodgerBlue",
    "*XmToggleButtonGadget*selectColor: wheat",
#ifndef _AIX
#ifndef sparc
#if XtSpecificationRelease > 4
    "*pulldown*tearOffModel: TEAR_OFF_ENABLED",
#endif
#endif
#endif
    "*XmMessageBox.labelFontList: -*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
    "*XmMessageBox.buttonFontList: -*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
    "*XmPushButton.fontList: -*-helvetica-bold-r-normal--11-*-*-*-*-*-*-*",
    "*XmPushButtonGadget.fontList: -*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
    "*XmToggleButtonGadget.fontList: -*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
    "*XmCascadeButton.fontList: -*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
    "*XmCascadeButtonGadget.fontList: -*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
    "*XmFileSelectionBox.XmText.fontList: -*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
    "*XmFileSelectionBox.buttonFontList: -*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
    "*XmFileSelectionBox.labelFontList: -*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
    "*XmFileSelectionBox.textFontList: -*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
    "*XmLabel.fontList: -*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
    "*XmLabel.textFontList: -*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
    "*XmLabelGadget.fontList: -*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
    "*XmLabelGadget.textFontList: -*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
    "*optionmenu*XmPushButtonGadget.fontList: -*-fixed-bold-r-*-*-13-*-*-*-*-70-*-*",
    "*popup*XmPushButtonGadget.fontList: -*-fixed-bold-r-*-*-13-*-*-*-*-70-*-*",
    "*popup*XmToggleButtonGadget.fontList: -*-fixed-bold-r-*-*-13-*-*-*-*-70-*-*",
    "*popup*XmCascadeGadget.fontList: -*-fixed-bold-r-*-*-13-*-*-*-*-70-*-*",
    "*popup*XmCascadeButtonGadget.fontList: -*-fixed-bold-r-*-*-13-*-*-*-*-70-*-*",
    "*textfield.fontList: -*-fixed-bold-r-*-*-13-*-*-*-*-70-*-*",
    (char *)NULL
  };



/*------- Hv_VaInitialize ----------*/

void Hv_VaInitialize(unsigned int argc,
		     char **argv,
		     ...)

{
  va_list       ap;            /* traverses the list */
  char          *versStr;
  char          autovers[50];
  short         versnum;

  Hv_AttributeArray attributes;

  va_start(ap, argv);
  Hv_GetAttributes(ap, attributes, NULL, NULL, NULL, NULL, NULL);

/* get attributes relevant for initialization starting
   with width and height */

  
  Hv_mwLeft      = attributes[Hv_LEFT].s;
  Hv_mwTop       = attributes[Hv_TOP].s;
  Hv_mwWidth     = attributes[Hv_WIDTH].s;
  Hv_mwHeight    = attributes[Hv_HEIGHT].s;
  Hv_mwFillColor = (short *)attributes[Hv_MAINWINDOWCOLOR].v;


/* use extended colors? */

  if (attributes[Hv_USEEXTENDEDCOLORS].i != 0)
      Hv_numColors = Hv_EXTENDEDCOLORS;
  else
      Hv_numColors = Hv_COLORS;
  
/* if the version number is used, we need to create the version string */

  versnum = attributes[Hv_VERSIONNUMBER].s;
  if (versnum >= 0) {
    if ((versnum % 10) == 0)
      sprintf(autovers, "v\\h\\_\\+\\+\\p%-4.1f", ((float)versnum/100.0));
    else
      sprintf(autovers, "v\\h\\_\\+\\+\\p%-4.2f", ((float)versnum/100.0));
    versStr = autovers;
  }
  else
    versStr = (char *)(attributes[Hv_WELCOMETEXT].v);

  Hv_trapCommandArgs = (attributes[Hv_TRAPCOMMANDARGS].s != 0);
  
  if (Hv_Initialize(argc, argv, versStr) != 0) {
    fprintf(stderr, "Hv failed to initialize. Exiting.\n");
    exit(1);
  }

/* add virtual view? */

  if (attributes[Hv_USEVIRTUALVIEW].s != 0)
    Hv_CreateVirtualView(attributes);
  
  if ((attributes[Hv_USEWELCOMEVIEW].s != 0)) {
    Hv_addWelcomeRainbow = (attributes[Hv_NORAINBOW].s == 0);
    Hv_CreateWelcomeView();
  }
}

/*------- Hv_Initialize ----------*/

static int Hv_Initialize(unsigned int argc,
			 char       **argv,
			 char       *versStr)

{
  char   *fbr;

  if (Hv_libMode)
    fbr = NULL;
  else
    fbr = (char *)fallbackresources;

  Hv_offsetDueToScroll = False;
  Hv_offsetDueToDrag = False;

/* memory related variables */

  Hv_activePointers = 0;    /* total # active pointers malloced via malloc & Hv_CreateRegion*/
  Hv_InitAppName(argv, versStr);

#ifndef _AIX
#ifndef sparc
#if XtSpecificationRelease > 4
  XmRepTypeInstallTearOffModelConverter(); /* for tear off menus */
#endif
#endif
#endif
  
/* Initialize tookit. Returns Hv_toplevel An application shell) widget.
   Note that XtInitialize will automatically parse the command
   line for a set of standard options. See page 163 of the
   X toolkit Intrinsics reference manual. Note also that the
   defaults file for this application should be in:

	/usr/lib/X11/app-defaults/appName		*/


  Hv_toplevel = XtVaAppInitialize(&Hv_appContext, (String)Hv_appName,
				  (XrmOptionDescList)NULL, (Cardinal)0,
#if XtSpecificationRelease > 4
				  (int *)&argc,
#else
				  (Cardinal *)&argc,
#endif
				  (String *)argv, (String *)fbr,
				  NULL);

/* create managed form. */

  Hv_form = XmCreateForm(Hv_toplevel, "form", NULL, 0);
  XtManageChild(Hv_form);

  Hv_MainInit();

/* If argc is greater than 1, then unknown command line
   options were specified */

  if (argc > 1)
    Hv_InvalidOptions(argc, argv);

  return  0;
}


/*------ Hv_Go -------*/

void Hv_Go(void)

/* essentially the same as XtMainLoop. We
   do it ourselves to maintain more control */ 
     
{
  XEvent     event;

  Hv_LastStageInitialization();

  if (!Hv_libMode)
    for(;;) {
      XtAppNextEvent(Hv_appContext, &event);
      XtDispatchEvent(&event);
    }
}

/* ------ Hv_LastStageInitialization -------*/

static void Hv_LastStageInitialization(void)

{
  Hv_InitWindow();                      /* make the window bigger */
  Hv_InitGraphics();	                /* initializations of global graphics variables */
  Hv_InitPatterns();	                /* initializations of fill patterns */

  if (!Hv_appContext)
    Hv_appContext = XtWidgetToApplicationContext(Hv_toplevel);
  
  if (Hv_libMode) {
    Hv_SetMenuItemString(Hv_quitItem, "Back to Calling Program", Hv_DEFAULT);
    Hv_SetMenuItemString(Hv_quitPopupItem, "Back to Calling Program", Hv_DEFAULT);
  }
}

/* ------- Hv_AddEventHandlers ---------- */

static void	Hv_AddEventHandlers(void)

{
  XtAddEventHandler(Hv_canvas, ExposureMask,      False,  (XtEventHandler)Hv_HandleExpose,     NULL);
  XtAddEventHandler(Hv_canvas, KeyReleaseMask,    False,  (XtEventHandler)Hv_HandleKeyRelease, NULL);
  XtAddEventHandler(Hv_canvas, PointerMotionMask, False,  (XtEventHandler)Hv_PointerMotion,    NULL);
  XtAddEventHandler(Hv_canvas, ButtonReleaseMask, False,  (XtEventHandler)Hv_ButtonRelease,    NULL); 
  XtAddEventHandler(Hv_canvas, ButtonPressMask,   False,  (XtEventHandler)Hv_ButtonPress,      NULL); 
}

/*------- Hv_CreateWelcomeView -----*/

static void Hv_CreateWelcomeView(void)

{
  char   *title;
  short  l, t;

/* welcome view title = user@host */

  title = (char *)Hv_Malloc(256);
  
  if (Hv_userName) {
    strcpy(title , " ");
    strncat(title, Hv_userName, 40);
    if (Hv_hostName) {
      strcat(title, "@");
      strncat(title, Hv_hostName, 40);
    }
    strcat(title, " ");
  }
  else
    strcpy(title, Hv_programName);

/* where the welcome view goews depends on whether there
   was a virtual view */

  if (Hv_virtualView != NULL) {
    l = Hv_virtualView->local->left;
    t = Hv_virtualView->local->top + Hv_virtualView->local->height + 10;
  }
  else {
    l = 10;
    t = 40;
  }
    
  
  Hv_VaCreateView(&Hv_welcomeView,
		  Hv_TAG,            Hv_WELCOMEVIEW,
		  Hv_LEFT,           l,
		  Hv_TOP,            t,
		  Hv_TITLE,          title,
		  Hv_INITIALIZE,     Hv_SetupWelcomeView,
		  Hv_POPUPCONTROL,   Hv_MINPOPUPCONTROL - Hv_UNDOVIEW,
		  Hv_OPENATCREATION, True,
		  Hv_ADDHOTRECT,     False,
		  NULL);
  
  Hv_Free(title);
}


/* ------ Hv_MainInit ------*/

static void Hv_MainInit(void)

{
  Hv_RADTODEG = 180.0/Hv_PI;
  Hv_DEGTORAD = Hv_PI/180.0;


  Hv_InitBalloons();            /* initialize baloon help */
  Hv_InitControls();	        /* default values for control vars */
  Hv_InitColors();      	/* initialize colors */
  Hv_CreateMenuBar();		/* create menubar and standard menus*/
  Hv_InitCanvas();		/* create main drawing area */
  Hv_CreatePopups();            /* create generic popups used by all views */
  Hv_AddEventHandlers();	/* add event handlers for various events */
  Hv_ReadConfigurationFile(NULL);   /* read default configuration, if any */

  

/* set the colormap attribute for the Hv_toplevel widget */

  XtVaSetValues(Hv_toplevel, XmNcolormap, Hv_colorMap, NULL);
  XtRealizeWidget(Hv_toplevel);	/* realize the top level shell widget */
  XFlush(XtDisplay(Hv_toplevel));

  Hv_mainWindow = XtWindow(Hv_canvas);	  /* toplevel window */
  Hv_trueMainWindow = Hv_mainWindow;

  Hv_InitGlobalWidgets();		/* creation of global widgets */

  XtSetArg (tlargs[0], XmNx, &tlx);
  XtSetArg (tlargs[1], XmNy, &tly);
  XtSetArg (tlargs[2], XmNwidth,  &tlw);
  XtSetArg (tlargs[3], XmNheight, &tlh);
  XtGetValues(Hv_toplevel, tlargs, 4);

  Hv_InitFonts();	                /* initialize global fonts */
  Hv_CreateGraphicsContext();	        /* create a graphics context*/
}


/* ----- Hv_InitControls -----*/

static void	Hv_InitControls(void)

/* Initialize the variables that define the state and determine flow */

{ 
  int   i, status;

/* some systems disable terminal output
   for bg jobs by default */

#ifdef __hpux
 system("stty -tostop");
#endif

#ifdef __linux__
  fprintf(stdout, "Running under Linux\n");
#endif
#ifdef VAX
  fprintf(stdout, "Running under VMS\n");
#endif
#ifdef sparc
  fprintf(stdout, "Running on a Sun\n");
#endif
#ifdef ultrix
  fprintf(stdout, "Running under ULTRIX\n");
#endif
#ifdef _AIX
  fprintf(stdout, "Running under AIX\n");
#endif
#ifdef __hpux
  fprintf(stdout, "Running under HP-UX\n");
#endif
#ifdef __sgi
  fprintf(stdout, "Running under IRIX\n");
#endif
#ifdef __osf__
  fprintf(stdout, "Running under OSF\n");
#endif

  fprintf(stderr, "X11 release: %2d\n", XtSpecificationRelease);
  fprintf(stderr, "Hv version: %-7.4f\n", ((float)Hv_version)/10000.0);

/* intialize all control variables */

  Hv_firstExposure = True;
  Hv_userItemName = NULL;
  Hv_handlingExposure = False;
  Hv_tileX = 32;
  Hv_tileY = 32;
  Hv_TileDraw = NULL;
  Hv_fKeysActive = True;                   /*default fkey diagnostics ON */
  Hv_scrollDH = 0;
  Hv_scrollDV = 0;
  Hv_sciNotHiLimit = 1.001e5;
  Hv_sciNotLowLimit = 0.01;
  Hv_printAllColorsBlack = False;

/* init postscript controls */

  Hv_psFileName = NULL;
  Hv_psEntireView = False;
  Hv_psTile = False;
  Hv_psExpandToFit = False;        /* expand a small picture? */
  Hv_psNote = NULL;
  Hv_inPostscriptMode = False;
  Hv_psOrientation = Hv_PSPORTRAIT;
  Hv_psDefaultFile = NULL;

  Hv_completeFeedback = True;           /* DragItem sets this temporarily to false for faster feedback */
  Hv_extraFastPlease = False;

/* Initialize Cursors */

  Hv_defaultCursor     = XCreateFontCursor(XtDisplay(Hv_toplevel), XC_left_ptr);  /* default cursor is an arrow */
  Hv_crossCursor       = XCreateFontCursor(XtDisplay(Hv_toplevel), XC_crosshair); /* small cross hair cursor for hot rects */
  Hv_waitCursor        = XCreateFontCursor(XtDisplay(Hv_toplevel), XC_watch);     /* watch cursor for waiting */
  Hv_dragCursor        = XCreateFontCursor(XtDisplay(Hv_toplevel), XC_fleur);     /* cursor used when dragging */
  Hv_dotboxCursor      = XCreateFontCursor(XtDisplay(Hv_toplevel), XC_dotbox);    /* cursor used when dragging */
  Hv_bottomRightCursor = XCreateFontCursor(XtDisplay(Hv_toplevel), XC_bottom_right_corner);     /* cursor used when resizing */

/* try to get user and host name */

  Hv_userName = (char *)getenv("USER");
  
  if (!Hv_userName)
    Hv_userName = (char *)getenv("LOGNAME");
  
  Hv_hostName = (char *) Hv_Malloc(80);
  if (Hv_hostName)
    status = gethostname(Hv_hostName, 80);

/* init the spcial user functions */

  Hv_CustomizeQuickZoom = NULL;  /*user provided to modify quickzoom labels */
  Hv_QuickZoom          = NULL;
  Hv_UserZoom           = NULL;
  Hv_UserMove           = NULL;
  Hv_UserResize         = NULL;
  Hv_AfterItemRotate    = NULL;
  Hv_ClickIntercept     = NULL;

  Hv_dontZoom = False;
  Hv_timedRedraw = False;

/* init the view list */
  Hv_views.first = NULL;
  Hv_views.last  = NULL;

/* initialize the search paths */

  for (i = 0; i <= Hv_NUMSEARCHPATH; i++)
    Hv_ioPaths[i] = NULL;

  for (i = 0; i < 9; i++)
    Hv_FunctionKeyCallback[i] = NULL;
}


/* ------- Hv_InitColors ------------ */

static void Hv_InitColors(void)

{
  Display	*dpy = XtDisplay(Hv_toplevel);	 /* display ID */
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
    fprintf(stderr, "Sorry, Hv requires a monitor with at least %d colors.\n", Hv_numColors);
    fprintf(stderr, "This monitor can only display %d colors.\n", ncolors);
    fprintf(stderr, "But, in the hope that you have a non standard displayI will continue.\n");
  }

  Hv_MakeColorMap();
  Hv_scrollBarColor = Hv_gray8;
  Hv_fancySymbolLite = Hv_white;
  Hv_scrollInactiveColor = Hv_aliceBlue;
  Hv_scrollActiveColor = Hv_skyBlue;
}


/* -------- Hv_InitCanvas -------------- */

static void Hv_InitCanvas(void)

/* create main drawing area */

{
  int		n;
  Arg		args[6];

  XtAppAddActions(Hv_appContext, canvasactions, XtNumber(canvasactions));

  n = 0;
  XtSetArg(args[n], XmNtopAttachment,    XmATTACH_FORM);   n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);   n++;
  XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM);   n++;
  XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM);   n++;

  Hv_canvas = XmCreateDrawingArea(Hv_form, "canvas", args, n);
  XtOverrideTranslations(Hv_canvas, XtParseTranslationTable(canvastranstable));
  XtManageChild(Hv_canvas);
}


/* ---------- Hv_InitGlobalWidgets ------------- */

static void Hv_InitGlobalWidgets(void)

/* create widgets that can be used anytime */

{
  int                   dummy1;
  unsigned int          dummy2, rw, rh;
  Window       root;

  Hv_display = XtDisplay(Hv_toplevel);	  /* find the display pointer for this particular cpu */

  XDefineCursor(Hv_display, XtWindow(Hv_toplevel), Hv_waitCursor);


  XGetGeometry(Hv_display, RootWindow(Hv_display, DefaultScreen(Hv_display)), &root,
	       &dummy1, &dummy1, &rw, &rh, &dummy2, &dummy2);
  Hv_root_width  = (short)rw;
  Hv_root_height = (short)rh;
}


/*------ Hv_InitAppName ------*/

static void Hv_InitAppName(char **argv,
			   char *versStr)

{
  char *tstr, *tcpy;

/* copy the program name into the global variable Hv_progamName
   and the local var appname (which must have its first letter
   upper case. Strip any prepended path */


  Hv_InitCharStr(&tstr, argv[0]);
  tcpy = tstr;
  Hv_StripLeadingDirectory(&tcpy);
  Hv_InitCharStr(&Hv_programName, tcpy);
  Hv_InitCharStr(&Hv_appName, tcpy);

  Hv_Free(tstr);

  Hv_appName[0] = toupper(Hv_appName[0]);  /* used to get resource file */

/* copy the appname and version string */
  
  Hv_InitCharStr(&Hv_versionString, versStr);
}


/* ----Hv_CanvasResizeCB ----- */

static void Hv_CanvasResizeCB(Widget    w,
			      XtPointer client_data,
			      XtPointer call_data)

{
  int          dummy1;
  unsigned int dummy2;
  Window       root;
  unsigned int mw, mh;
  Dimension    crt;

  XGetGeometry(Hv_display, Hv_mainWindow, &root, &dummy1, &dummy1,
	       &mw, &mh, &dummy2, &Hv_colorDepth);
  Hv_mainWidth =  (short)mw;
  Hv_mainHeight = (short)mh;

/* fix the canvas "super rect " which will be used for World <--> Local */

  XtVaGetValues(Hv_menuBar, XmNheight, &crt, NULL);
  Hv_canvasRect.top = (short)crt;
  Hv_canvasRect.left = 0;
  Hv_canvasRect.right = Hv_mainWidth - 1;
  Hv_canvasRect.width = Hv_mainWidth;
  Hv_canvasRect.bottom = Hv_mainHeight - 1;
  Hv_canvasRect.height = Hv_mainHeight - Hv_canvasRect.top;

  Hv_UpdateVirtualView();
  Hv_FullClip();
}

/* -------- Hv_InitPatterns ----------*/

static void Hv_InitPatterns()

{

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


/* ------- Hv_InitGraphics ------------ */

static void Hv_InitGraphics(void)

{
  Window		root;
  Dimension             crt;
  int                   dummy1;
  unsigned int          dummy2, mw, mh;
  int			n;
  Arg			args[10];
  float                 w_in;

/* get some screen information */

  XGetGeometry(Hv_display, Hv_mainWindow, &root, &dummy1, &dummy1,
	       &mw, &mh, &dummy2, &Hv_colorDepth);
  Hv_mainWidth =  (short)mw;
  Hv_mainHeight = (short)mh;

/* get the canvas "super rect " which will be used for World <--> Local */

  n = 0;
  XtSetArg(args[n], XmNheight, &crt);	n++;
  XtGetValues(Hv_menuBar, args, n);

  Hv_SetRect(&(Hv_canvasRect), 0, (short)crt, Hv_mainWidth, Hv_mainHeight - (short)crt);
  Hv_FixRectRB(&(Hv_canvasRect));

/* if resource file was not read in, window may have zero size */

  if (Hv_canvasRect.height <= 0) {
    XGetGeometry(Hv_display, RootWindow(Hv_display, DefaultScreen(Hv_display)), &root,
		 &dummy1, &dummy1,
		 &mw, &mh, &dummy2, &dummy2);
    
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

  XtAddCallback(Hv_canvas, XmNresizeCallback, Hv_CanvasResizeCB, 0);		

  XDefineCursor(Hv_display, XtWindow(Hv_toplevel), Hv_defaultCursor);

/* some final color manipulations */

  Hv_BrightenColor(Hv_navyBlue, 3000);
  Hv_DarkenColor(Hv_lawnGreen, 12000);
  Hv_AdjustColor(Hv_aliceBlue, -10000, -3500, 0);
}


/* ------- Hv_InitWindow ---------- */

static void Hv_InitWindow(void)

/* expands the window - which insures an exposure event --
   which kick starts app */
{
  Arg			args[5];

/* get some useful geometry info.
   auto set main window to 95% of screen (centered) */
  
/* check for no or unreasonable val from resource file */

  if (tlh < 80) {
    tlx = (short) (Hv_root_width  * 0.015);
    tly = (short) (Hv_root_height * 0.035);
    if (Hv_libMode) {
      tlw = (short) (Hv_root_width  * 0.808);
      tlh = (short) (Hv_root_height * 0.791);
    }
    else {
      tlw = (short) (Hv_root_width  * 0.95);
      tlh = (short) (Hv_root_height * 0.930);
    }
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

  XtSetArg (args[0], XmNx,  tlx);
  XtSetArg (args[1], XmNy,  tly);
  XtSetArg (args[2], XmNwidth,  tlw);
  XtSetArg (args[3], XmNheight, tlh);
  XtSetValues (Hv_toplevel, args, 4);

}


/* ---------- Hv_CreateGraphicsContext ----------- */

static void  Hv_CreateGraphicsContext(void)

/* create a graphics context */

{
  XGCValues	 vals;		/* for setting GC attributes */
  unsigned long	 mainBg;	/* background color of main window */

  
/* retrieve fg and bg from Hv_canvas widget to use in gc */
  
  XtVaGetValues(Hv_canvas, XmNforeground, &Hv_mainFg,
		XmNbackground, &mainBg, NULL);

/* see if attribute used to change bg color of main window */
  
  if (Hv_mwFillColor != NULL) {
    mainBg = Hv_colors[*Hv_mwFillColor];
    XtVaSetValues(Hv_canvas, XmNbackground, &mainBg, NULL);
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

/*-------- Hv_AddFunctionKeyCallback -----*/

void    Hv_AddFunctionKeyCallback(int num,
				  Hv_FunctionPtr CB)

/*
  int             num   which key 1..9
  Hv_FunctionPtr  CB
*/

{
  if ((num > 0) && (num < 10))
    Hv_FunctionKeyCallback[num-1] = CB;
}


/*-------- Hv_HandleKeyRelease --------- */

static void Hv_HandleKeyRelease(Widget    w,
				XtPointer client_data,
				XKeyEvent *event)

/* Handles keyboard presses. We don't
   use them much, except for "hidden" features,
   such as dumping the pointer table */

{
  KeySym    keysym;
  char      buffer[20];
  int       num_bytes = 20;
  static    Boolean  SyncOn = True;
  Hv_View   temp;
  Boolean   DoIt;

  XLookupString(event, buffer, num_bytes, &keysym, NULL);

  switch (keysym) {

  case XK_F1:
    if (Hv_FunctionKeyCallback[0] != NULL)
      Hv_FunctionKeyCallback[0]();
    break;
    
  case XK_F2:
    if (Hv_FunctionKeyCallback[1] != NULL)
      Hv_FunctionKeyCallback[1]();
    else if (Hv_fKeysActive) {
      for (temp = Hv_views.last; temp != NULL; temp = temp->prev) 
	if (Hv_ViewIsVisible(temp)) 
	  Hv_DumpView(temp, stderr);
    }
    break;
 
  case XK_F3:
    if (Hv_FunctionKeyCallback[2] != NULL)
      Hv_FunctionKeyCallback[2]();
    else if (Hv_fKeysActive) {
      if (SyncOn)
	DoIt = Hv_Question("F3 toggles graphic synch, GREATLY slows drawing (useful for diagnostics). Do it?");
      else
	DoIt = True;
      
      if (DoIt) {
	fprintf(stderr, "SyncOn = %d\n", SyncOn);
	XSynchronize(Hv_display, SyncOn);
	SyncOn = !SyncOn;
      }
    }
    break;
    
  case XK_F4:
    if (Hv_FunctionKeyCallback[3] != NULL)
      Hv_FunctionKeyCallback[3]();
    break;
    
  case XK_F5:
    if (Hv_FunctionKeyCallback[4] != NULL)
      Hv_FunctionKeyCallback[4]();
    else if (Hv_fKeysActive) {
      fprintf(stderr, "Total number of active pointers = %d\n", Hv_activePointers);

    }
    break;

  case XK_F6:
    if (Hv_FunctionKeyCallback[5] != NULL)
      Hv_FunctionKeyCallback[5]();
    else {
      fprintf(stderr, "List of Item Connections\n");
      for (temp = Hv_views.last; temp != NULL; temp = temp->prev) 
	if (Hv_ViewIsVisible(temp)) 
	  Hv_DumpViewConnections(temp, stderr);
    }
    break;
    
    
  case XK_F7:
    if (Hv_FunctionKeyCallback[6] != NULL)
      Hv_FunctionKeyCallback[6]();
    break;
    
    
  case XK_F8:
    if (Hv_FunctionKeyCallback[7] != NULL)
    Hv_FunctionKeyCallback[7]();
    break;
    
    
  case XK_F9:
    if (Hv_FunctionKeyCallback[8] != NULL)
      Hv_FunctionKeyCallback[8]();
    else if (Hv_fKeysActive) {
      for (temp = Hv_views.last; temp != NULL; temp = temp->prev) 
	if (Hv_ViewIsVisible(temp)) 
	  Hv_DumpView(temp, stderr);
    }
    break;
    
  }
}

/*------ Hv_SetFKeysActive -------*/

void  Hv_SetFKeysActive(Boolean active)

/* way to turn fkeys diagnostics off and on */

{
  Hv_fKeysActive = active;
}


/*-------- Hv_HandleExpose --------- */

static void Hv_HandleExpose(Widget    w,
			    XtPointer client_data,
			    XEvent    *event)

/* Handles exposure events by accumulating them unil there are no more,
  the updating all affected Views*/
{
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

