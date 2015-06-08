/**
 * <EM>Various windowing system independent (non-gateway) initializations.</EM>
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
#include "Hv_pic.h"
#include "Hv_pointer.h"
#include <ctype.h>



/*---- local prototypes -----*/

static void     Hv_CreateWelcomeView(void);

static void	Hv_InitControls(void);



short              Hv_mwLeft;       /* from attributes to Hv_VaInitialize */
short              Hv_mwTop;        /* from attributes to Hv_VaInitialize */
short              Hv_mwWidth;      /* from attributes to Hv_VaInitialize */
short              Hv_mwHeight;     /* from attributes to Hv_VaInitialize */
short             *Hv_mwFillColor;  /* from attributes to Hv_VaInitialize */


/*------- global variables --------*/

/* use to disable magnification stuff */

Boolean     Hv_allowMag = True;

/* used to control amount of magnification */

int         Hv_magfact = 2;

/* used to draw NonBackground items into the background pixmap */

Boolean      Hv_drawNBItemsToo = False;

/* used to globally disable continuous zoom */

Boolean      Hv_allowContinuousZoom = True;

/* used to signal continous zoom mode */

Boolean   Hv_inContinuousZoom = False;

/* used by Windows apps */

int Hv_iccmdShow = 0;

/* these used to be macros, but for consistency changed to vars.
   they still masquerade as macros */

float Hv_RADTODEG;
float Hv_DEGTORAD;


/* file separator variables */

char *Hv_FileSeparatorString = NULL;
char  Hv_FileSeparator;

/* path separator variables */

char *Hv_PathSeparatorString = NULL;
char  Hv_PathSeparator;

/* some strings used in callbacks */

char           *Hv_inputCallback = NULL;
char           *Hv_activateCallback = NULL;
char           *Hv_valueChangedCallback = NULL;
char           *Hv_okCallback = NULL;
char           *Hv_cancelCallback = NULL;
char           *Hv_multipleSelectionCallback = NULL;
char           *Hv_singleSelectionCallback = NULL;
char           *Hv_thinMenuCallback = NULL;

Hv_View         Hv_modalView = NULL;            /*if not NULL, modal view up*/

int             Hv_units = Hv_KILOMETERS;       /* of use in map views */

Boolean         Hv_usedNewDrawTools = False; /* used to distinguish old & new drawing tools */
Boolean         Hv_okSetColor = True;  /* used in conjunction with pats in PS */


char          **Hv_configuration = NULL;        /* null terminated string array holding current config file */

Hv_FunctionPtr  Hv_AlternateEventHandler = NULL; /* alternate event handler */

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
char            *Hv_cachedDirectory1 = NULL;
char            *Hv_cachedDirectory2 = NULL;
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

/* Unix command line arguments */

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
int             Hv_version = 10930;    /*10000 * version*/

Hv_FunctionPtr  Hv_userItemName;
Hv_FunctionPtr  Hv_FunctionKeyCallback[9];
Hv_FunctionPtr  Hv_HighlightDraw = NULL;
Hv_Widget       Hv_quitItem;
Hv_Widget       Hv_quitPopupItem;

Boolean         Hv_handlingExposure;
Hv_Item         Hv_excludeFromExposure = NULL;
unsigned int    Hv_tileX, Hv_tileY;
Hv_FunctionPtr  Hv_TileDraw;
Hv_AppContext   Hv_appContext = NULL;
Boolean         Hv_fKeysActive;         /* if true, fkeys provide diagnostic messages */
Boolean         Hv_addWelcomeRainbow;   /* if false, no welcome rainbow */ 
Boolean         Hv_offsetDueToScroll;   /* indicates a call to OffSetItem is due to a scroll, not moving a view */
short           Hv_scrollDH;            /* used as hint that redraw is result of scroll */
short           Hv_scrollDV;            /* used as hint that redraw is result of scroll */
Boolean         Hv_offsetDueToDrag;     /* indicates a call to OffSetItem is due to a drag, not moving a view */ 


unsigned int    Hv_activePointers;      /* total # active pointers malloced via malloc & Hv_CreateRegion*/

Hv_Visual      *Hv_visual = NULL;	/* default visual */
Hv_Display     *Hv_display = NULL;	/* what display (i.e. cpu) is used */
Hv_Window	Hv_mainWindow = 0;    	/* main application window */
Hv_Window	Hv_trueMainWindow = 0;  /* unchanging (even while off screen draws) */
Hv_Cursor       Hv_defaultCursor;       /* default cursor */
Hv_Cursor       Hv_crossCursor;         /* crosshair cursor */
Hv_Cursor       Hv_waitCursor;          /* wait cursor */
Hv_Cursor       Hv_dragCursor;          /* for dragging a view*/
Hv_Cursor       Hv_bottomRightCursor;   /* for resizing a view*/
Hv_Cursor       Hv_dotboxCursor;        /* a box with a dot in the middle */
char           *Hv_hostName = NULL;     /* e.g. "clas01" */
char           *Hv_userName = NULL;     /* e.g. "yomama" */
char           *Hv_osName   = NULL;     /* e.g. "IRIX" */

/* function pointers for specialized use responses following
   some action */

Hv_FunctionPtr       Hv_CustomizeQuickZoom = NULL;  /*user provided to modify quickzoom labels */
Hv_FunctionPtr       Hv_QuickZoom = NULL;           /*should be redirected to app specific*/
Hv_FunctionPtr       Hv_UserZoom = NULL;            /*can be used for specialized zoom AFTER standard zoom*/
Hv_FunctionPtr       Hv_UserMove = NULL;            /*can be used for specialized zoom AFTER standard move*/
Hv_FunctionPtr       Hv_UserResize = NULL;          /*can be used for specialized zoom AFTER standard resize*/
Hv_FunctionPtr       Hv_ClickIntercept = NULL;      /*if set, a click is redirected directly to this function*/
Boolean              Hv_ignoreClickIntercept = False; /*used to ignore above */

Hv_FunctionPtr       Hv_AfterItemRotate = NULL;     /*called after usual rotate*/
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

Hv_ColorMap	Hv_colorMap;	      /* this program's color map */
short		Hv_rainbowFirst;      /* index of first color used for "thermal" plots*/
short		Hv_rainbowLast;	      /* index of last color used for "thermal" plots*/
short           Hv_numColors;
unsigned long	Hv_colors[Hv_COLORS]; /* actual colors (pixel values into the colormap) */
Boolean         Hv_readOnlyColors = True;

short           Hv_globalFeedbackLabelColor = -1;
short           Hv_globalFeedbackDataFont = -1;

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

Hv_GC		Hv_gc;	              /* main graphics context */
View_ListHead	Hv_views;	      /* bookkeeper for our View list */

/* Global Widgets */

Hv_Widget	Hv_toplevel;	      /* highest level widget  (Shell widget) */
Hv_Widget	Hv_form;              /* form widget for main window */
Hv_Widget	Hv_canvas;	      /* main drawing area */
Hv_Widget	Hv_menuBar;	      /* Main Menu bar in main window */


Hv_View         Hv_welcomeView = NULL;
Hv_View         Hv_virtualView = NULL;

/* standard menus */

Hv_Widget   Hv_helpMenu = NULL;
Hv_Widget   Hv_actionMenu = NULL;
Hv_Widget   Hv_viewMenu = NULL;
Hv_Widget   Hv_popupMenu = NULL;
Hv_Widget   Hv_popupMenu2 = NULL;
Hv_Widget   Hv_quickZoomSubmenu = NULL;
Hv_Widget   Hv_undoSubmenu = NULL;

Hv_Widget   Hv_popupItems[Hv_NUMPOPUP]; 
Hv_Widget   Hv_quickZoomItems[Hv_NUMQUICKZOOM]; 

/* user menus */

Boolean            Hv_userMenu;

BalloonData     Hv_theBalloon;                      /* one global balloon for ced */
char           *Hv_balloonStrs[Hv_MAXBALLOONSTR];   /* balloon displayer breaks down the one giant string int these bite sized pieces */
Hv_CompoundString Hv_balloonCstr[Hv_MAXBALLOONSTR];   /* Motif strings for balloon text */
Boolean         Hv_showBalloons;                    /* if true, some items will display "balloon" help */
Boolean         Hv_balloonUp;                       /* program name */
char           *Hv_programName;
char           *Hv_versionString;
char           *Hv_appName;


/**
 * Hv_VaInitialize 
 * @purpose Thisis the main initialization routine. The first two arguments
 * are required and are the usual ar
 * called by the applivation.
 * @param        argc      usual number of command line args
 * @param        argv      usual array of string arguments
 * @param        ...       NULL terminated list of attribute value pairs
 * <p>
 * @table
 * |R Attribute                 | Type     | Default    | Comment |r
 * |R Hv_LEFT                   | short    | calculated | Left pixel of main window |r
 * |R Hv_TOP                    | short    | calculated | Top pixel of main window |r
 * |R Hv_WIDTH                  | short    | calculated | main window width |r
 * |R Hv_HEIGHT                 | short    | calculated | Main window height |r
 * |R Hv_MAINWINDOWCOLOR        | short *  | blue-ish   | Main window color. Note this really <BR>requires a pointer, such as &Hv__peachPuff |r
 * |R Hv_VERSIONNUMBER          | short    | 0          | 100 * application version< |r
 * |R Hv_WELCOMETEXT            | char *   | NULL       | Text in the welcome view |r
 * |R Hv_USEVIRTUALVIEW         | Boolean  | False      | Use the virtual desktop |r
 * |R Hv_USEWELCOMEVIEW         | Boolean  | False      | Use a welcome view |r
 * |R Hv_NORAINBOW              | Boolean  | False      | No rainbow on the welcome view |r
 * |R Hv_VIRTUALWIDTH           | short    | 5000       | Virtual desktop width |r
 * |R Hv_VIRTUALHEIGHT          | short    | 4000       | Virtual desktop height |r
 * |R Hv_VIRTUALVIEWHRW         | short    | 140        | Virtual view hot rect width |r
 * |R Hv_VIRTUALVIEWHRFILL      | short    | brown-ish  | Virtual view hot rect color |r
 * |R Hv_VIRTUALVIEWFBFILL      | short    | tan-ish    | Virtual view feedback color |r
 * |R Hv_VIRTUALVIEWFBFONT      | short    | helvetica  | Virtual view feedback font |r
 * |R Hv_VIRTUALVIEWFBTEXTCOLOR | short    | black      | Virtual view text color |r
 * |R Hv_VIRTUALVIEWLMARGIN     | short    | 4          | Virtual view left margin |r
 * |R Hv_VIRTUALVIEWTMARGIN     | short    | 30         | Virtual view top margin |r
 * |R Hv_VIRTUALVIEWRMARGIN     | short    | 4          | Virtual view right margin |r
 * |R Hv_VIRTUALVIEWBMARGIN     | short    | 4          | Virtual view bottom margin |r
 * |R Hv_VIRTUALVIEWFBDATACOLS  | short    | 25         | Virtual view feedback columns |r
 * @endtable
 */

void Hv_VaInitialize(unsigned int argc,
		     char       **argv,
		                  ...)  {
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
    Hv_Println("Hv failed to initialize. Exiting.");
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


/**
 * Hv_LastStageInitialization 
 */


void Hv_LastStageInitialization(void) {
  Hv_InitWindow();                      /* make the window bigger */
  Hv_InitGraphics();	                /* initializations of global graphics variables */
  Hv_InitPatterns();	                /* initializations of fill patterns */
  Hv_CreateAppContext();                /* create the application context */
}


/**
 * Hv_CreateWelcomeView    Creates the optional welcome view.
 */

static void Hv_CreateWelcomeView(void) {
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



/**
 * Hv_mainInit   Multitudinous initialization calls
 */

void Hv_MainInit(void) {
  Hv_RADTODEG = (float)(180.0/Hv_PI);
  Hv_DEGTORAD = (float)(Hv_PI/180.0);


  Hv_InitBalloons();            /* initialize baloon help */
  Hv_InitControls();	        /* default values for control vars */
  Hv_InitColors();      	/* initialize colors */
  Hv_CreateMenuBar();		/* create menubar and standard menus*/
  Hv_InitCanvas();		/* create main drawing area */
  Hv_CreatePopups();            /* create generic popups used by all views */

  Hv_ReadConfigurationFile(NULL);   /* read default configuration, if any */

  Hv_SystemSpecificInit();

  Hv_trueMainWindow = Hv_mainWindow;
  Hv_InitGlobalWidgets();		/* creation of global widgets */
  Hv_InitFonts();	                /* initialize global fonts */
  Hv_CreateGraphicsContext();	        /* create a graphics context*/
}


/* ----- Hv_InitControls -----*/

static void	Hv_InitControls(void) {

/* Initialize the variables that define the state and determine flow */

  int   i;

/* some systems disable terminal output
   for bg jobs by default */

#ifdef __hpux
 system("stty -tostop");
#endif

#ifdef WIN32
  Hv_Println("Running under Windows");
#endif
#ifdef __linux__
  Hv_Println("Running under Linux");
#endif
#ifdef VAX
  Hv_Println("Running under VMS");
#endif
#ifdef sparc
  Hv_Println("Running on a Sun");
#endif
#ifdef ultrix
  Hv_Println("Running under ULTRIX");
#endif
#ifdef _AIX
  Hv_Println("Running under AIX");
#endif
#ifdef __hpux
  Hv_Println("Running under HP-UX");
#endif
#ifdef __sgi
  Hv_Println("Running under IRIX");
#endif
#ifdef __osf__
  Hv_Println("Running under OSF");
#endif

  Hv_Println("Hv version: %-7.4f", ((float)Hv_version)/10000.0);

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
  Hv_sciNotHiLimit = (float)(1.001e5);
  Hv_sciNotLowLimit = (float)(0.01);
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

  Hv_InitCursors();


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


/*------ Hv_InitAppName ------*/

void Hv_InitAppName(char **argv,
		    char *versStr) {
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


/**
 * Hv_AdFunctionKeyCallback
 * @param    num       the function key number [1..9]
 * @param    callback  the callback function
 */

void    Hv_AddFunctionKeyCallback(int num,
				  Hv_FunctionPtr callback) {
    if ((num > 0) && (num < 10))
	Hv_FunctionKeyCallback[num-1] = callback;
}


/**
 * Hv_HandleFunctionKey
 * @param     key   an integer 1-9 specifying which function key
 */

void Hv_HandleFunctionKey(short x,
						  short y,
						  int  key) {

  Hv_View   temp;
  Hv_Point  pp;
  
  if ((!Hv_fKeysActive) || (key < 1) || (key > 9))
    return;
  
  Hv_SetPoint(&pp, x, y);

/* if a  user defined one is present, use it, but do
	not allow overwrite of fk1 -- always use to magnify */

  if ((key > 1) && (Hv_FunctionKeyCallback[key-1] != NULL)) {
    Hv_FunctionKeyCallback[key-1](&pp);
    return;
  }
  
  
  switch (key) {
  case 1:
    Hv_LaunchMagnify(&pp);
    break;
    
  case 2:
    for (temp = Hv_views.last; temp != NULL; temp = temp->prev) 
      if (Hv_ViewIsVisible(temp)) 
        Hv_DumpView(temp, stderr);
      break;
      
  case 3:
    break;
    
  case 4:
    break;
    
  case 5:
    Hv_Println("Total number of active pointers = %d", Hv_activePointers);
    break;
    
  case 6:
    Hv_Println("List of Item Connections");
    for (temp = Hv_views.last; temp != NULL; temp = temp->prev) 
      if (Hv_ViewIsVisible(temp)) 
        Hv_DumpViewConnections(temp, stderr);
      break;
      
  case 7:
    break;
    
  case 8:
    break;
    
  case 9:
    break;
    
  }
}


/*------ Hv_SetFKeysActive -------*/

void  Hv_SetFKeysActive(Boolean active)

/* way to turn fkeys diagnostics off and on */

{
  Hv_fKeysActive = active;
}


