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

#define DEBUG_CONNECTING 0
#define DEBUG_ZEROHEIGHT 0

#define TRAP_XT_ERRORS
#if 0
/* KE: Destroying the top-level shell is not usually done, is not
 * necessary for StripTool, and seems to cause some hard-to-identify
 * problems, though it should be legal */
#define DESTROY_TOPLEVEL_SHELL
#endif

#if 0
/* KE: It should not be necessary to realize the top-level shell for StripTool */q
#define REALIZE_TOPLEVEL_SHELL
#endif

#undef FIX_FMM

#define USE_RIGHT_CLICK_ON_BUTTONS

#define LARGE_ZOOM_FACTOR 2.0;
#define SMALL_ZOOM_FACTOR 1.071773462536293 /* 2^(.1) */
#define LARGE_PAN_FACTOR  0.5
#define SMALL_PAN_FACTOR  0.05
#define ROFF              1.0e-10

#include "Strip.h"
#include "StripDialog.h"
#include "StripDataSource.h"
#include "StripHistory.h"
#include "StripGraph.h"
#include "StripDAQ.h"
#include "StripMisc.h"
#include "StripFallback.h"

#include "Annotation.h"

#include "pan_left.bm"
#include "pan_right.bm"
#include "zoom_in.bm"
#include "zoom_out.bm"
#include "auto_scroll.bm"
#include "pan.bm"
#include "reset.bm"
#include "refresh.bm"

#include <errno.h>

#ifdef STRIP_HISTORY   
#include "from_to.bm"      /* Albert */
#include "alg.bm"          /* Albert */
#include "replot.bm"       /* Albert */
#endif
#include "auto_scaleR.bm"  /* Albert */
#include "auto_scale.bm"   /* Albert */
#include "zoom_inY.bm"     /* Albert */
#include "zoom_outY.bm"    /* Albert */
#include "pan_up.bm"       /* Albert */
#include "pan_down.bm"     /* Albert */
#ifdef USE_AAPI
#include "AAPI.h"
#endif /* USE_AAPI */

#ifdef WIN32
/* There is no pwd.h on WIN32 */
#else
#  include <pwd.h>
#endif

#ifdef USE_XPM
#  include <X11/xpm.h>          
#  include "StripGraphIcon.xpm"         
#  include "StripDialogIcon.xpm"        
#else
#  include "StripGraphIcon.bm"
#  include "StripDialogIcon.bm"
#endif

#ifdef USE_XMU
#  include <X11/Xmu/Editres.h>
#endif

#ifdef USE_CLUES
#include "LiteClue.h"
#endif

#include "ComboBox.h"

#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/DrawingA.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>
#include <Xm/MessageB.h>
#include <Xm/List.h>
#include <Xm/TextF.h>
#include <Xm/FileSB.h>
#include <Xm/RowColumn.h>
#include <Xm/DragDrop.h>
#include <Xm/Protocols.h>
#include <X11/Xlib.h>
#include <Xm/ToggleB.h>

#ifdef WIN32
#else
# include <unistd.h>
#endif

#include <math.h>
#include <string.h>

#define DEF_WOFFSET             3
#define STRIP_MAX_FDS           64

#if DEBUG_TRANSLATIONS
/* From TMprint.c */
String _XtPrintXlations(Widget w, XtTranslations xlations,
  Widget accelWidget, _XtBoolean includeRHS);
#endif


/* ====== StripEvent stuff ====== */
typedef enum
{
  STRIPEVENT_SAMPLE = 0,
  STRIPEVENT_REFRESH,
  STRIPEVENT_CHECK_CONNECT,
  LAST_STRIPEVENT
} StripEventType;

typedef enum
{
  STRIPEVENTMASK_SAMPLE         = (1 << STRIPEVENT_SAMPLE),
  STRIPEVENTMASK_REFRESH        = (1 << STRIPEVENT_REFRESH),
  STRIPEVENTMASK_CHECK_CONNECT  = (1 << STRIPEVENT_CHECK_CONNECT)
} StripEventMask;

#if 0
/* KE: unused */
static char     *StripEventTypeStr[LAST_STRIPEVENT] =
{
  "Sample",
  "Refresh",
  "Check Connect"
};
#endif

/* ====== PrintInfo stuff ====== */
typedef struct _PrintInfo
{
  char  printer[128];
  char  device[128];
}
PrintInfo;

typedef struct _PrinterDialog
{
  Widget        msgbox;
  Widget        name_textf, device_combo;
}
PrinterDialog;

/* ====== Strip stuff ====== */
typedef enum
{
  STRIPSTAT_OK                  = (1 << 0),
  STRIPSTAT_PROCESSING          = (1 << 1),
  STRIPSTAT_TERMINATED          = (1 << 2),
  STRIPSTAT_BROWSE_MODE         = (1 << 3)
} StripStatus;

enum
{
  STRIPBTN_LEFT = 0,
  STRIPBTN_RIGHT,
  STRIPBTN_UP,         /* Albert */
  STRIPBTN_DOWN,       /* Albert */
  STRIPBTN_ZOOMIN,
  STRIPBTN_ZOOMOUT,
  STRIPBTN_ZOOMINY,    /* Albert */
  STRIPBTN_ZOOMOUTY,   /* Albert */
#ifdef STRIP_HISTORY
  STRIPBTN_FROM_TO,    /* Albert */
  STRIPBTN_ALG,        /* Albert */
  STRIPBTN_REPLOT,     /* Albert */
#endif /*STRIP_HISTORY*/
  STRIPBTN_RESET,
  STRIPBTN_REFRESH,
  STRIPBTN_AUTO_SCALE, /* Albert */
  STRIPBTN_AUTOSCROLL,
  STRIPBTN_COUNT
};
  
typedef struct _stripFdInfo
{
  XtInputId             id;
  int                   fd;
}
stripFdInfo;


typedef enum
{
  DFSDLG_TGL_ASCII = 0,
  DFSDLG_TGL_CSV,
#ifdef USE_SDDS
  DFSDLG_TGL_SDDS,
#endif
  DFSDLG_TGL_COUNT
}
DmpFsDlgTglFileTypes;

char    *DfsDlgTglStr[DFSDLG_TGL_COUNT] =
{
  "ASCII",
  "Comma Separated Values",
#ifdef USE_SDDS
  "SDDS binary"
#endif
};

typedef struct _StripInfo
{
  /* == X Stuff == */
  XtAppContext          app;
  Display               *display;
  Widget                toplevel, shell, canvas;
  Widget                graph_form, graph_panel;
  Widget                btn[STRIPBTN_COUNT];
  Widget                browse_lbl;
  Widget                popup_menu, message_box;
  Widget                fs_dlg;
  Widget                fs_tgl[DFSDLG_TGL_COUNT];
  char                  app_name[128];

  /* == file descriptor management ==  */
  stripFdInfo           fdinfo[STRIP_MAX_FDS];

  /* == Strip Components == */
  StripConfig           *config;
  StripDialog           dialog;
  StripCurveInfo        curves[STRIP_MAX_CURVES];
  StripDataSource       data;
  StripHistory          history;
  StripGraph            graph;
  StripDAQ              daq;
  unsigned              status;
  PrintInfo             print_info;
  PrinterDialog         *pd;
  void                  *annotation_info;

  /* == Callback stuff == */
  StripCallback         connect_func, disconnect_func, client_io_func;
  void                  *connect_data, *disconnect_data, *client_io_data;
  int                   client_registered;
  int                   grab_count;

  /* timing and event stuff */
  unsigned              event_mask;
  struct timeval        last_event[LAST_STRIPEVENT];
  struct timeval        next_event[LAST_STRIPEVENT];

  XtIntervalId          tid;
}
StripInfo;


/* ====== Miscellaneous stuff ====== */
typedef enum
{
  STRIPWINDOW_GRAPH = 0,
  STRIPWINDOW_COUNT
}
StripWindowType;

static char     *StripWindowTypeStr[STRIPWINDOW_COUNT] =
{
  "Graph",
};

typedef void    (*fsdlg_functype)       (Strip, char *);

/* ====== Static Data ====== */
static struct timeval   tv;
static XmString xstr_panning, xstr_notpanning;


/* ====== Static Function Prototypes ====== */
static void     Strip_watchevent        (StripInfo *, unsigned);
static void     Strip_ignoreevent       (StripInfo *, unsigned);

static void     Strip_dispatch          (StripInfo *);

#if 0
/* KE: unused */
static void     Strip_child_msg         (void *);
#endif
static void     Strip_config_callback   (StripConfigMask, void *);

static void     Strip_forgetcurve       (StripInfo *, StripCurve);

static void     Strip_graphdrop_handle  (Widget, XtPointer, XtPointer);
static void     Strip_graphdrop_xfer    (Widget, XtPointer, Atom *, Atom *,
                                         XtPointer, unsigned long *, int *);

static void     Strip_printer_init      (StripInfo *);

static void     Strip_setbrowsemode     (StripInfo *, int);

static void     callback                (Widget, XtPointer, XtPointer);
static int      X_error_handler         (Display *, XErrorEvent *);
static int      X_ignore_error          (Display *, XErrorEvent *);
#ifdef TRAP_XT_ERRORS
void            Xt_warning_handler      (String msg);
void            Xt_error_handler        (String msg);
#endif

static void     dlgrequest_connect      (void *, void *);
static void     dlgrequest_show         (void *, void *);
static void     dlgrequest_clear        (void *, void *);
static void     dlgrequest_delete       (void *, void *);
static void     dlgrequest_dismiss      (void *, void *);
static void     dlgrequest_quit         (void *, void *);
static void     dlgrequest_window_popup (void *, void *);

static Widget   PopupMenu_build         (Widget);
static void     PopupMenu_position      (Widget, XButtonPressedEvent *);
static void     PopupMenu_popup         (Widget);
#if 0
/* KE: unused */
static void     PopupMenu_popdown       (Widget);
#endif
static void     PopupMenu_cb            (Widget, XtPointer, XtPointer);

static PrinterDialog    *PrinterDialog_build    (Widget);
static void     PrinterDialog_popup     (PrinterDialog *, StripInfo *);
static void     PrinterDialog_cb        (Widget, XtPointer, XtPointer);

static void     fsdlg_popup             (StripInfo *, fsdlg_functype);
static void     fsdlg_cb                (Widget, XtPointer, XtPointer);

int auto_scaleTriger=-1;
static Pixmap auto_scalePixmap[2];
static Pixmap browsePixmap[2];
static int auto_scaleNoBrowse=1;
static int changeMinMax( Strip the_strip);
long radioChange=0;
Widget history_topShell;

#ifdef  STRIP_HISTORY
#include <Xm/ToggleBG.h>  /* Albert */ 
#include <ctype.h>
#include <Xm/Text.h>  /* Albert */ 
#include <Xm/LabelG.h>
#include <Xm/ArrowBG.h>
#include <Xm/ArrowBGP.h>

typedef enum {
  YEAR = 0,
  MONTH,
  DAY,
  HOUR,
  MINUTE,
  LAST_TIME_UNIT
} from_to_time_enum;

typedef struct _goData {
  StripInfo *si;
  Widget *time_textFrom;
  Widget *time_textTo;
} goData;

typedef struct _arrowData {
  int num;
  Widget w;
} arrowData;


int arch_flag =0;          /* small circle fitting */

#define MAX_HISTORY_SIZE 1000000
#define DEFAULT_HISTORY_SIZE 6000

#ifndef USE_AAPI
#define AVERAGE_METHOD 1
#endif

long radioBoxAlgorithm=AVERAGE_METHOD-1;  
unsigned int historySize=DEFAULT_HISTORY_SIZE;
/*
char *algorithmString[]={
  "Average History Algorithm",
  "(Tail) of raw data",
  "Sharp   History Algorithm",
  "Spline History Algorithm"
};
*/
char **algorithmString;
int algorithmLength=0;

static int timUnitMin[LAST_TIME_UNIT]={1990,1,1,0,0};
static int timUnitMax[LAST_TIME_UNIT]={2040,12,31,23,59};
static char *timeUnitString[LAST_TIME_UNIT]={"YYYY:","MM:","DD:","HH:","MI:"};
static Widget fromToShell=NULL; 
static Widget algShell=NULL; 
static Widget timeUnitTextWidget[2][LAST_TIME_UNIT];

static Widget fromToDialog_init (Widget, char*, StripInfo*); 
static void radio_toggled();
static void arch_callback();
static void radioClose();
static void radioHelp();
static void allDigit();
static void stepUp(); 
static void stepDown();
static void fromToGo();
static void fromToClose();
static void historyPoints();

#endif /* STRIP_HISTORY */

/* ====== Functions ====== */
Strip   Strip_init      (int    *argc,
                         char   *argv[],
                         FILE   *logfile)
{
  StripInfo             *si;
#if 0
  Dimension             width, height;
#endif
  XSetWindowAttributes  xswa;
  XVisualInfo           xvi;
  Widget                form, rowcol, hintshell, w;
  Pixmap                pixmap;
  Pixel                 fg, bg;
  SDWindowMenuItem      wm_items[STRIPWINDOW_COUNT];
  cColorManager         scm;
  intptr_t              i;
  int                   n;
  Arg                   args[10];
  Atom                  import_list[10];
  StripConfigMask       scfg_mask;
  Atom                  WM_DELETE_WINDOW;
  XrmDatabase           db, db_fall, db_site, db_user;
  char                  *env;
  char                  path[STRIP_PATH_MAX];
  char                  **pstr;
#ifndef WIN32
  struct passwd         user;
  char                  *a, *b;
#endif
  char *envHelpPath = NULL;
#ifdef USE_RIGHT_CLICK_ON_BUTTONS
  String                rightBtnTranslations =
    "~Ctrl<Btn3Down>: Arm() \n\
     ~Ctrl<Btn3Down>,~Ctrl<Btn3Up>: Activate() Disarm()";
  XtTranslations parsedRightBtnTranslations;
#endif
  
  StripConfig_preinit();
  StripConfigMask_clear (&scfg_mask);
  
  if ((si = (StripInfo *)malloc (sizeof (StripInfo))) != NULL)
  {
    si->history = StripHistory_init ((Strip)si); /* Albert */
    /* initialize the X-toolkit */
    XtSetLanguageProc (0, 0, 0);
    XtToolkitInitialize();
    
    /* create the app context and open the display */
    si->app = XtCreateApplicationContext();
    si->display = XtOpenDisplay
      (si->app, 0, 0, STRIP_APP_CLASS, 0, 0, argc, argv);
    if (!si->display) {
      fprintf (stdout, "Strip_init: cannot initialize display.\n");
      free (si);
      return 0;
    }

#ifdef TRAP_XT_ERRORS
    /* replace default Xt handlers */
    XtAppSetErrorHandler (si->app, Xt_error_handler);
    XtAppSetWarningHandler (si->app, Xt_warning_handler);
#endif

    /* create a resource database from the fallbacks, then merge in
     * the site defaults, followed by the user defaults
     */
    
    /* current resource database */
    db = XrmGetDatabase (si->display);
    
    /* build database from fallback specs */
    db_fall = (XrmDatabase)0;
    pstr = fallback_resources;
    while (*pstr) XrmPutLineResource (&db_fall, *pstr++);
    
    /* merge into current database without overriding */
    XrmCombineDatabase (db_fall, &db, False);
    
    /* build site default resource database */
    env = getenv (STRIP_SITE_DEFAULTS_FILE_ENV);
    if (!env) env = STRIP_SITE_DEFAULTS_FILE;
    db_site = XrmGetFileDatabase (env);
    
    /* merge into current resource database, overrinding */
    if (db_site) XrmCombineDatabase (db_site, &db, True);
    
    /* build user default resource database
     */
    
#ifdef WIN32
    /* not sure where this file should be located in windows.  assume
       "\" for now. */
    strcpy(path,"\\");
#else
    /* first, get path of user's home directory and append the
     * resource file name to that */
    memcpy (&user, getpwuid (getuid()), sizeof (struct passwd));
    a = user.pw_dir;
    b = path;
    while (*a) *b++ = *a++;
    *b++ = '/';
    a = STRIP_USER_DEFAULTS_FILE;
    while (*a) *b++ = *a++;
    *b = 0;
#endif
    
    /* build the database */
    db_user = XrmGetFileDatabase (path);
    
    /* merge into current resource database, overrinding */
    if (db_user) XrmCombineDatabase (db_user, &db, True);
    
    
    /* create the top-level shell, but don't realize it until the
     * appropriate visual has been chosen.
     */
    si->toplevel = XtVaAppCreateShell
      (0, STRIP_APP_CLASS, applicationShellWidgetClass, si->display,
	  XmNmappedWhenManaged, False,
	  NULL);
    if(!si->toplevel)
    {
      fprintf (stdout, "Strip_init: cannot initialize toplevel shell.\n");
      free (si);
      return NULL;
    
    }
#if DEBUG_ZEROHEIGHT
    {
	Dimension width,height;

	printf("Test XtRealizeWidget [Before]\n");
#if 0
      XtRealizeWidget (si->toplevel);
	printf("Test XtRealizeWidget [After]\n");
#endif
	
	XtVaGetValues(si->toplevel,
	  XmNheight,&height,
	  XmNwidth,&width,
	  NULL);
	printf("Strip_init: si->toplevel: height=%u width=%u\n",
	  height,width);
    }
#endif
    
    
#ifdef USE_XMU
    /* editres support */
    XtAddEventHandler
      (si->toplevel, (EventMask)0, True,
	  (XtEventHandler)_XEditResCheckMessages, 0);
#endif
    
    /* replace default error handler so that we don't exit on
     * non-fatal exceptions */
    XSetErrorHandler (X_error_handler);
    
    /* initialize the color manager */
    scm = cColorManager_init (si->display, /*STRIPCONFIG_NUMCOLORS*/ 0);
    if (scm)
    {
      xvi = *cColorManager_get_visinfo (scm);
      XtVaSetValues
        (si->toplevel,
	    XmNvisual,     xvi.visual,
	    XmNcolormap,   cColorManager_getcmap (scm),
	    NULL);
#ifdef REALIZE_TOPLEVEL_SHELL

	/* KE: It is not necessary to realize this shell */
      XtRealizeWidget (si->toplevel);
#endif	
    }
    else
    {
      fprintf (stdout, "Strip_init: cannot initialize color manager.\n");
#ifdef DESTROY_TOPLEVEL_SHELL
	/* KE: This is not usually done, is not necessary here, and
	 * seems to cause some hard-to-identify problems, though it
	 * should be legal */
      XtDestroyWidget (si->toplevel);
#endif	  
      free (si);
      return NULL;
    }
    
    /* initialize any miscellaeous routines */
    StripMisc_init (si->display, xvi.screen);
    
    /* initialize the Config module. will set scfg->scm=scm. */
    if (!(si->config = StripConfig_init (scm, &xvi, NULL, scfg_mask)))
    {
      fprintf
        (stdout,
	    "Strip_init: cannot initialize configuration manager.\n");
      cColorManager_delete (scm);
#ifdef DESTROY_TOPLEVEL_SHELL
	/* KE: This is not usually done, is not necessary here, and
	 * seems to cause some hard-to-identify problems, though it
	 * should be legal */
      XtDestroyWidget (si->toplevel);
#endif
	free (si);
      return NULL;
    }
    si->config->logfile = logfile;
#if 0
  /* KE: Not used, not available on WIN32 */
    si->config->user = user;
#endif    
    
    /* construct help url */
    env = getenv (STRIP_SITE_DEFAULTS_FILE_ENV);
    if (!env) env = STRIP_SITE_DEFAULTS_FILE;
    db_site = XrmGetFileDatabase (env);

      /* help URL */
    envHelpPath = getenv("STRIP_HELP_PATH");
    if(envHelpPath != NULL) {
	strncpy(stripHelpPath, envHelpPath, STRIP_PATH_MAX);
    } else {
	strncpy(stripHelpPath, STRIP_HELP_PATH, STRIP_PATH_MAX);
    }
    stripHelpPath[STRIP_PATH_MAX-1] = '\0';

#ifdef USE_RIGHT_CLICK_ON_BUTTONS
    /* parse the translation table */
    parsedRightBtnTranslations =
	XtParseTranslationTable(rightBtnTranslations);
#endif
    
    /* build the other shells now that the toplevel
     * has been taken care of.
     */
#ifdef USE_CLUES
    hintshell = XtVaCreatePopupShell
      ("HintShell", xcgLiteClueWidgetClass, si->toplevel, NULL);
#endif
    
    si->shell = XtVaCreatePopupShell
      ("StripGraph",
	  topLevelShellWidgetClass, si->toplevel,
	  XmNdeleteResponse,        XmDO_NOTHING,
	  XmNmappedWhenManaged,     False,
	  XmNvisual,                si->config->xvi.visual,
	  XmNcolormap,              cColorManager_getcmap (scm),
	  NULL);
    
    history_topShell=si->shell;
    
#ifdef USE_XMU
    /* editres support */
    XtAddEventHandler
      (si->shell, (EventMask)0, True,
	  (XtEventHandler)_XEditResCheckMessages, 0);
#endif
    
    
    /* hook window manager delete message so we can shut down gracefully */
    WM_DELETE_WINDOW = XmInternAtom (si->display, "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback (si->shell, WM_DELETE_WINDOW, callback, si);
#if 0
    width = (Dimension)
      (((double)DisplayWidth (si->display, DefaultScreen (si->display)) /
        (double)DisplayWidthMM (si->display, DefaultScreen (si->display)))
	  * STRIP_GRAPH_WIDTH_MM);
    height = (Dimension)
      (((double)DisplayHeight (si->display, DefaultScreen (si->display)) /
        (double)DisplayHeightMM (si->display, DefaultScreen (si->display)))
	  * STRIP_GRAPH_HEIGHT_MM);
    XtVaSetValues
      (si->shell,
	  XmNwidth,        width,
	  XmNheight,       height,
	  NULL);
#endif
    XtRealizeWidget (si->shell);
    
    form = XtVaCreateManagedWidget
      ("graphBaseForm",
	  xmFormWidgetClass,       si->shell,
	  NULL);
    
    /* the the motif foreground and background colors */
    XtVaGetValues
      (form,
	  XmNforeground,           &fg,
	  XmNbackground,           &bg,
	  NULL);
    
    /* the graph control panel */
    si->graph_panel = XtVaCreateManagedWidget
      ("graphPanel",
	  xmFrameWidgetClass,              form,
	  XmNtopAttachment,                XmATTACH_NONE,
	  XmNleftAttachment,               XmATTACH_FORM,
	  XmNrightAttachment,              XmATTACH_FORM,
	  XmNbottomAttachment,             XmATTACH_FORM,
	  NULL);
    rowcol = XtVaCreateManagedWidget
      ("controlsRowColumn",
	  xmRowColumnWidgetClass,  si->graph_panel,
	  XmNorientation,          XmHORIZONTAL,
	  NULL);
    
    /* pan left button */
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
	  (char *)pan_left_bits,
	  pan_left_width, pan_left_height, fg, bg,
	  xvi.depth);
    si->btn[STRIPBTN_LEFT] = XtVaCreateManagedWidget
      ("panLeftButton",
	  xmPushButtonWidgetClass, rowcol,
	  XmNlabelType,            XmPIXMAP,
	  XmNlabelPixmap,          pixmap,
	  NULL);
    
    /* pan right button */
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
	  (char *)pan_right_bits, pan_right_width, pan_right_height, fg, bg,
	  xvi.depth);
    si->btn[STRIPBTN_RIGHT] = XtVaCreateManagedWidget
      ("panRightButton",
	  xmPushButtonWidgetClass, rowcol,
	  XmNlabelType,            XmPIXMAP,
	  XmNlabelPixmap,          pixmap,
	  NULL);
#if DEBUG_TRANSLATIONS
    {
	XtTranslations xlations=NULL;
	String xString=NULL;
	Widget w=si->btn[STRIPBTN_RIGHT];
	
	XtVaGetValues(w,XtNtranslations,&xlations,NULL);
#if 0
	/* Note: widget argument is needed, even if the
	   parsedTranslations are independent of it */
	xString= _XtPrintXlations(w,parsedTranslations,NULL,True);
	print("parsedTranslations:\n");
	print("%s\n",xString);
	XtFree(xString);
#endif
	xString= _XtPrintXlations(w,xlations,NULL,True);
	print("Default translations:\n");
	print("%s\n",xString);
	XtFree(xString);
    }
#endif
    
    /* pan up button  Albert*/
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
	  (char *)pan_up_bits, pan_up_width, pan_up_height, fg, bg,
	  xvi.depth);
    si->btn[STRIPBTN_UP] = XtVaCreateManagedWidget
      ("panUpButton",
	  xmPushButtonWidgetClass, rowcol,
	  XmNlabelType,            XmPIXMAP,
	  XmNlabelPixmap,          pixmap,
	  NULL);
    
    /* pan down button */
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
	  (char *)pan_down_bits, pan_down_width, pan_down_height, fg, bg,
	  xvi.depth);
    si->btn[STRIPBTN_DOWN] = XtVaCreateManagedWidget
      ("panDownButton",
	  xmPushButtonWidgetClass, rowcol,
	  XmNlabelType,            XmPIXMAP,
	  XmNlabelPixmap,          pixmap,
	  NULL);

    /* separator */
    XtVaCreateManagedWidget
      ("separator",
	  xmSeparatorWidgetClass,  rowcol,
	  XmNorientation,          XmVERTICAL,
	  NULL);
    
    /* zoom in button */
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
	  (char *)zoom_in_bits, zoom_in_width, zoom_in_height, fg, bg,
	  xvi.depth);
    si->btn[STRIPBTN_ZOOMIN] = XtVaCreateManagedWidget
      ("zoomInButton",
	  xmPushButtonWidgetClass, rowcol,
	  XmNlabelType,            XmPIXMAP,
	  XmNlabelPixmap,          pixmap,
	  NULL);
    
    /* zoom out button */
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
	  (char *)zoom_out_bits, zoom_out_width, zoom_out_height, fg, bg,
	  xvi.depth);
    si->btn[STRIPBTN_ZOOMOUT] = XtVaCreateManagedWidget
      ("zoomOutButton",
	  xmPushButtonWidgetClass, rowcol,
	  XmNlabelType,            XmPIXMAP,
	  XmNlabelPixmap,          pixmap,
	  NULL);
    
    /* zoom inY button  Albert*/
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
	  (char *)zoom_inY_bits, zoom_inY_width, zoom_inY_height, fg, bg,
	  xvi.depth);
    si->btn[STRIPBTN_ZOOMINY] = XtVaCreateManagedWidget
      ("zoomInYButton",
	  xmPushButtonWidgetClass, rowcol,
	  XmNlabelType,            XmPIXMAP,
	  XmNlabelPixmap,          pixmap,
	  NULL);
    
    /* zoomY out button Albert*/
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
	  (char *)zoom_outY_bits, zoom_outY_width, zoom_outY_height, fg, bg,
	  xvi.depth);
    si->btn[STRIPBTN_ZOOMOUTY] = XtVaCreateManagedWidget
      ("zoomOutYButton",
	  xmPushButtonWidgetClass, rowcol,
	  XmNlabelType,            XmPIXMAP,
	  XmNlabelPixmap,          pixmap,
	  NULL);
    
    /* separator */
    XtVaCreateManagedWidget
      ("separator",
	  xmSeparatorWidgetClass,  rowcol,
	  XmNorientation,          XmVERTICAL,
	  NULL);
    
#ifdef STRIP_HISTORY   
    /* FROM-TO button. Albert */
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen), from_to_bits,
	  from_to_width, from_to_height, fg, bg,
	  xvi.depth);
    si->btn[STRIPBTN_FROM_TO] = XtVaCreateManagedWidget
      ("fromToButton",
	  xmPushButtonWidgetClass, rowcol,
	  XmNlabelType,		XmPIXMAP,
	  XmNlabelPixmap,		pixmap,
	  NULL);
    
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen), alg_bits,
	  alg_width, alg_height, fg, bg,
	  xvi.depth);
    si->btn[STRIPBTN_ALG] = XtVaCreateManagedWidget
      ("newHistoryAlgorithmButton",
	  xmPushButtonWidgetClass, rowcol,
	  XmNlabelType,		XmPIXMAP,
	  XmNlabelPixmap,		pixmap,
	  NULL);
    
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen), replot_bits,
	  replot_width, replot_height, fg, bg,
	  xvi.depth);
    
    si->btn[STRIPBTN_REPLOT] = XtVaCreateManagedWidget
      ("realReplotButton",
	  xmPushButtonWidgetClass, rowcol,
	  XmNlabelType,		XmPIXMAP,
	  XmNlabelPixmap,		pixmap,
	  NULL);
    
    /* separator */
    XtVaCreateManagedWidget
      ("separator",
	  xmSeparatorWidgetClass,  rowcol,
	  XmNorientation,          XmVERTICAL,
	  NULL);
#endif /* STRIP_HISTORY */
    
    /* reset button */
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
	  (char *)reset_bits, reset_width, reset_height, fg, bg,
	  xvi.depth);
    si->btn[STRIPBTN_RESET] = XtVaCreateManagedWidget
      ("resetButton",
	  xmPushButtonWidgetClass, rowcol,
	  XmNlabelType,            XmPIXMAP,
	  XmNlabelPixmap,          pixmap,
	  NULL);
    
    /* refresh button */
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
	  (char *)refresh_bits, refresh_width, refresh_height, fg, bg,
	  xvi.depth);
    si->btn[STRIPBTN_REFRESH] = XtVaCreateManagedWidget
      ("refreshButton",
	  xmPushButtonWidgetClass, rowcol,
	  XmNlabelType,            XmPIXMAP,
	  XmNlabelPixmap,          pixmap,
	  NULL);
    
    /* separator */
    XtVaCreateManagedWidget
      ("separator",
	  xmSeparatorWidgetClass, 	rowcol,
	  XmNorientation,		XmVERTICAL,
	  NULL);
    
    /* autoscale button*/
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
	  (char *)auto_scaleR_bits, auto_scaleR_width, auto_scaleR_height, fg, bg,
	  xvi.depth);
    auto_scalePixmap[1] = pixmap ;
    
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
	  (char *)auto_scale_bits, auto_scale_width, auto_scale_height, fg, bg,
	  xvi.depth);
    auto_scalePixmap[0] = pixmap ;
    
    si->btn[STRIPBTN_AUTO_SCALE] = XtVaCreateManagedWidget
      ("autoRangeButton",
	  xmPushButtonWidgetClass, rowcol,
	  XmNlabelType,		XmPIXMAP,
	  XmNlabelPixmap,		pixmap,
	  NULL);
    
    /* auto scroll button */
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
	  (char *)pan_bits, pan_width, pan_height, fg, bg,
	  xvi.depth);
    browsePixmap[1] = pixmap;
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
	  (char *)auto_scroll_bits, auto_scroll_width, auto_scroll_height, fg, bg,
	  xvi.depth);
    browsePixmap[0] = pixmap;
    si->btn[STRIPBTN_AUTOSCROLL] = XtVaCreateManagedWidget
      ("autoScrollButton",
	  xmPushButtonWidgetClass, rowcol,
	  XmNlabelType,            XmPIXMAP,
	  XmNlabelPixmap,          pixmap,
	  NULL);
    
    for (i = 0; i < STRIPBTN_COUNT; i++)
      XtAddCallback (si->btn[i], XmNactivateCallback, callback, si);
    
#ifdef USE_RIGHT_CLICK_ON_BUTTONS
    XtOverrideTranslations(si->btn[STRIPBTN_LEFT],
	parsedRightBtnTranslations);
    XtOverrideTranslations(si->btn[STRIPBTN_RIGHT],
	parsedRightBtnTranslations);
    XtOverrideTranslations(si->btn[STRIPBTN_UP],
	parsedRightBtnTranslations);
    XtOverrideTranslations(si->btn[STRIPBTN_DOWN],
	parsedRightBtnTranslations);
    XtOverrideTranslations(si->btn[STRIPBTN_ZOOMIN],
	parsedRightBtnTranslations);
    XtOverrideTranslations(si->btn[STRIPBTN_ZOOMOUT],
	parsedRightBtnTranslations);
    XtOverrideTranslations(si->btn[STRIPBTN_ZOOMINY],
	parsedRightBtnTranslations);
    XtOverrideTranslations(si->btn[STRIPBTN_ZOOMOUTY],
	parsedRightBtnTranslations);
#endif

#ifdef USE_CLUES
    XcgLiteClueAddWidget
      (hintshell, si->btn[STRIPBTN_LEFT], "Pan Left", 0, 0);
    XcgLiteClueAddWidget
      (hintshell, si->btn[STRIPBTN_RIGHT], "Pan Right", 0, 0);
    XcgLiteClueAddWidget
      (hintshell, si->btn[STRIPBTN_UP], "Pan Up", 0, 0);          /*Albert */ 
    XcgLiteClueAddWidget
      (hintshell, si->btn[STRIPBTN_DOWN], "Pan Down", 0, 0);      /*Albert */
    XcgLiteClueAddWidget
      (hintshell, si->btn[STRIPBTN_ZOOMIN], "Zoom In X", 0, 0);
    XcgLiteClueAddWidget
      (hintshell, si->btn[STRIPBTN_ZOOMOUT], "Zoom Out X", 0, 0);
    XcgLiteClueAddWidget
      (hintshell, si->btn[STRIPBTN_ZOOMINY], "Zoom In Y", 0, 0);   /*Albert */
    XcgLiteClueAddWidget
      (hintshell, si->btn[STRIPBTN_ZOOMOUTY], "Zoom Out Y", 0, 0); /*Albert */
#ifdef STRIP_HISTORY   
    XcgLiteClueAddWidget 
      (hintshell, si->btn[STRIPBTN_FROM_TO], "From-To Interval", 0, 0);/*Albert */
    XcgLiteClueAddWidget
      (hintshell, si->btn[STRIPBTN_ALG], "History Algorithm", 0, 0);
    XcgLiteClueAddWidget
      (hintshell, si->btn[STRIPBTN_REPLOT], "Real Replot", 0, 0);
#endif /*STRIP_HISTORY   */
    XcgLiteClueAddWidget
      (hintshell, si->btn[STRIPBTN_RESET], "Reset", 0, 0);
    XcgLiteClueAddWidget
      (hintshell, si->btn[STRIPBTN_REFRESH], "Refresh", 0, 0);
    XcgLiteClueAddWidget
      (hintshell, si->btn[STRIPBTN_AUTO_SCALE], "Toggle AutoScale", 0, 0); /*Albert */
    XcgLiteClueAddWidget
      (hintshell, si->btn[STRIPBTN_AUTOSCROLL], "Toggle Scroll or Pan", 0, 0);
#endif

    /* strings for the browse-mode indicator */
    xstr_panning=XmStringCreateLocalized ("PANNING");
    xstr_notpanning=XmStringCreateLocalized ("SCROLLING");

    /* the browse mode indicator widget */
    si->browse_lbl = XtVaCreateManagedWidget
      ("browseIndicator",
       xmLabelWidgetClass,              rowcol,
       XmNlabelString,                  xstr_notpanning,
       NULL);

    /* the graph base widget */
    si->graph_form = XtVaCreateManagedWidget
      ("graphForm",
       xmFormWidgetClass,       form,
       XmNtopAttachment,        XmATTACH_FORM,
       XmNleftAttachment,       XmATTACH_FORM,
       XmNrightAttachment,      XmATTACH_FORM,
       XmNbottomAttachment,     XmATTACH_WIDGET,
       XmNbottomWidget,         si->graph_panel,
       XmNbackground,           si->config->Color.background.xcolor.pixel,
       NULL);

    si->graph = StripGraph_init (si->graph_form, si->config);
    StripGraph_getattr (si->graph, STRIPGRAPH_WIDGET, &si->canvas, 0);
       
    /* register the drawing area as a drop site accepting compound text
     * and string type data.  Note that, since there is no way to
     * register client data fo the callback, we need to store the
     * StripInfo pointer as the widget's user data */
    i = 0; n = 0;
    import_list[i++] = XmInternAtom (si->display, "COMPOUND_TEXT", False);
    XtSetArg (args[n], XmNimportTargets, import_list); n++;
    XtSetArg (args[n], XmNnumImportTargets, i); n++;
    XtSetArg (args[n], XmNdropSiteOperations, XmDROP_COPY); n++;
    XtSetArg (args[n], XmNdropProc, Strip_graphdrop_handle); n++;
    XmDropSiteRegister (si->canvas, args, n);
    XtVaSetValues (si->canvas, XmNuserData, (XtPointer)si, NULL);
      

    /* popup menu, etc. */
    si->popup_menu = PopupMenu_build (si->canvas);
    si->message_box = (Widget)0;
    XtVaSetValues (si->popup_menu, XmNuserData, si, NULL);

    si->fs_dlg = 0;

    for (i = 0; i < STRIPWINDOW_COUNT; i++)
    {
      strcpy (wm_items[i].name, StripWindowTypeStr[i]);
      wm_items[i].window_id = (void *)i;
      wm_items[i].cb_func = dlgrequest_window_popup;
      wm_items[i].cb_data = (void *)si;
    }
          
    si->dialog  = StripDialog_init (si->toplevel, si->config);
    StripDialog_setattr
      (si->dialog,
       STRIPDIALOG_CONNECT_FUNC,        dlgrequest_connect,
       STRIPDIALOG_CONNECT_DATA,        si,
       STRIPDIALOG_SHOW_FUNC,           dlgrequest_show,
       STRIPDIALOG_SHOW_DATA,           si,
       STRIPDIALOG_CLEAR_FUNC,          dlgrequest_clear,
       STRIPDIALOG_CLEAR_DATA,          si,
       STRIPDIALOG_DELETE_FUNC,         dlgrequest_delete,
       STRIPDIALOG_DELETE_DATA,         si,
       STRIPDIALOG_DISMISS_FUNC,        dlgrequest_dismiss,
       STRIPDIALOG_DISMISS_DATA,        si,
       STRIPDIALOG_QUIT_FUNC,           dlgrequest_quit,
       STRIPDIALOG_QUIT_DATA,           si,
       STRIPDIALOG_WINDOW_MENU, wm_items, STRIPWINDOW_COUNT,
       0);

    /* si->history = StripHistory_init ((Strip)si); */
    si->data = StripDataSource_init (si->history);
    StripDataSource_setattr
      (si->data, SDS_NUMSAMPLES, (size_t)si->config->Time.num_samples, 0);

    StripGraph_setattr (si->graph, STRIPGRAPH_DATA_SOURCE, si->data, 0);

    si->connect_func = si->disconnect_func = si->client_io_func = NULL;
    si->connect_data = si->disconnect_data = si->client_io_data = NULL;

    si->tid = (XtIntervalId)0;

    si->event_mask = 0;
    for (i = 0; i < LAST_STRIPEVENT; i++)
    {
      si->last_event[i].tv_sec  = 0;
      si->last_event[i].tv_usec = 0;
      si->next_event[i].tv_sec  = 0;
      si->next_event[i].tv_usec = 0;
    }

    for (i = 0; i < STRIP_MAX_CURVES; i++)
    {
      si->curves[i].scfg                        = si->config;
      si->curves[i].details                     = NULL;
      si->curves[i].func_data                   = NULL;
      si->curves[i].get_value                   = NULL;
      si->curves[i].connect_request.tv_sec      = 0;
      si->curves[i].id                          = NULL;
      si->curves[i].status                      = 0;
    }

    /* load the icon pixmap */
    StripDialog_getattr (si->dialog, STRIPDIALOG_SHELL_WIDGET, &w, 0);
    
#ifndef WIN32
    /* KE: These are trashed when used on WIN32.  Use the Exceed
       default, which is an X icon. */
#ifdef USE_XPM
    /* graph icon */
    if (XpmSuccess == XpmCreatePixmapFromData 
        (si->display, RootWindow (si->display, xvi.screen),
         StripGraphIcon_xpm, &pixmap, 0, 0))
      XtVaSetValues (si->shell, XtNiconPixmap, pixmap, NULL);

    /* dialog icon */
    if (XpmSuccess == XpmCreatePixmapFromData 
        (si->display, RootWindow (si->display, xvi.screen),
         StripDialogIcon_xpm, &pixmap, 0, 0))
      XtVaSetValues (w, XtNiconPixmap, pixmap, NULL);
#else
    /* graph icon */
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
       (char *)StripGraphIcon_bm_bits,
       StripGraphIcon_bm_width, StripGraphIcon_bm_height,
       fg, bg, xvi.depth);
    if (pixmap)
      XtVaSetValues(si->shell, XtNiconPixmap, pixmap, NULL);

    /* dialog icon */
    pixmap = XCreatePixmapFromBitmapData
      (si->display, RootWindow (si->display, xvi.screen),
       (char *)StripDialogIcon_bm_bits,
       StripDialogIcon_bm_width, StripDialogIcon_bm_height,
       fg, bg, xvi.depth);
    if (pixmap)
      XtVaSetValues(w, XtNiconPixmap, pixmap, NULL);
#endif
#endif
      

    si->grab_count = 0;

    StripConfig_addcallback
      (si->config, SCFGMASK_ALL, Strip_config_callback, si);

    /* this little snippet insures that an expose event will be generated
     * every time the window is resized --not just when its size increases
     */
    xswa.bit_gravity = ForgetGravity;
    xswa.background_pixel = si->config->Color.background.xcolor.pixel;
    XChangeWindowAttributes
      (si->display, XtWindow (si->canvas),
       CWBitGravity | CWBackPixel, &xswa);
    XChangeWindowAttributes
      (si->display, XtWindow (si->shell),
       CWBitGravity | CWBackPixel, &xswa);
    XtAddCallback (si->canvas, XmNinputCallback, callback, si);
    XUnmapWindow (si->display, XtWindow (si->shell));

    si->status = STRIPSTAT_OK;
    memset (si->fdinfo, 0, STRIP_MAX_FDS * sizeof (stripFdInfo));

    Strip_printer_init (si);
    si->pd = PrinterDialog_build (si->shell);

    si->annotation_info = Annotation_init (si->app,si->canvas,si->shell,
                           si->display, si->graph, &si->curves[0]);

  }

  return (Strip)si;
}


/*
 * Strip_delete
 */
void    Strip_delete    (Strip the_strip)
{
  StripInfo     *si = (StripInfo *)the_strip;

  if (!si) return;

  if (si->graph) StripGraph_delete (si->graph);
  if (si->data) StripDataSource_delete (si->data);
  if (si->history) StripHistory_delete (si->history);
  if (si->dialog) StripDialog_delete (si->dialog);
  if (si->config) StripConfig_delete (si->config);
  if (si->pd) free (si->pd);
#ifdef DESTROY_TOPLEVEL_SHELL
  /* KE: This is not usually done, is not necessary here, and
   * seems to cause some hard-to-identify problems, though it
   * should be legal */
#ifndef FIX_FMM
  if (si->toplevel) XtDestroyWidget (si->toplevel);
#endif
#endif
  free (si);
}


/*
 * Strip_setattr
 */
int     Strip_setattr   (Strip the_strip, ...)
{
  va_list       ap;
  StripInfo     *si = (StripInfo *)the_strip;
  int           attrib;
  int           ret_val = 1;


  va_start (ap, the_strip);
  for (attrib = va_arg (ap, StripAttribute);
       (attrib != 0);
       attrib = va_arg (ap, StripAttribute))
  {
    if ((ret_val = ((attrib > 0) && (attrib < STRIP_LAST_ATTRIBUTE))))
      switch (attrib)
      {
	case STRIP_CONNECT_FUNC:
	  si->connect_func = va_arg (ap, StripCallback);
	  break;
	case STRIP_CONNECT_DATA:
	  si->connect_data = va_arg (ap, void *);
	  break;
	case STRIP_DISCONNECT_FUNC:
	  si->disconnect_func = va_arg (ap, StripCallback);
	  break;
	case STRIP_DISCONNECT_DATA:
	  si->disconnect_data = va_arg (ap, void *);
	  break;
	case STRIP_DAQ:
	  si->daq = va_arg (ap, void *);
	  break;
      }
  }

  va_end (ap);
  return ret_val;
}


/*
 * Strip_getattr
 */
int     Strip_getattr   (Strip the_strip, ...)
{
  va_list       ap;
  StripInfo     *si = (StripInfo *)the_strip;
  int           attrib;
  int           ret_val = 1;


  va_start (ap, the_strip);
  for (attrib = va_arg (ap, StripAttribute);
       (attrib != 0);
       attrib = va_arg (ap, StripAttribute))
  {
    if ((ret_val = ((attrib > 0) && (attrib < STRIP_LAST_ATTRIBUTE))))
      switch (attrib)
      {
	case STRIP_CONNECT_FUNC:
	  *(va_arg (ap, StripCallback *)) = si->connect_func;
	  break;
	case STRIP_CONNECT_DATA:
	  *(va_arg (ap, void **)) = si->connect_data;
	  break;
	case STRIP_DISCONNECT_FUNC:
	  *(va_arg (ap, StripCallback *)) = si->disconnect_func;
	  break;
	case STRIP_DISCONNECT_DATA:
	  *(va_arg (ap, void **)) = si->disconnect_data;
	  break;
      }
  }

  va_end (ap);
  return ret_val;
}


/*
 * Strip_addfd
 */
int     Strip_addfd     (Strip                  the_strip,
  int                    fd,
  XtInputCallbackProc    func,
  XtPointer              data)
{
  StripInfo     *si = (StripInfo *)the_strip;
  int           i;

  for (i = 0; i < STRIP_MAX_FDS; i++)
    if (!si->fdinfo[i].id) break;

  if (i < STRIP_MAX_FDS)
  {
    si->fdinfo[i].id = XtAppAddInput
#ifdef WIN32
      (si->app, fd, (XtPointer)XtInputReadWinsock, func, data);
#else
      (si->app, fd, (XtPointer)XtInputReadMask, func, data);
#endif	
    si->fdinfo[i].fd = fd;
  }

  return (i < STRIP_MAX_FDS);
}


/*
 * Strip_addtimeout
 */
XtIntervalId    Strip_addtimeout        (Strip                  the_strip,
  double                 sec,
  XtTimerCallbackProc    cb_func,
  XtPointer              cb_data)
{
  StripInfo             *si = (StripInfo *)the_strip;

  return XtAppAddTimeOut
    (si->app, (unsigned long)(sec * 1000), cb_func, cb_data);
}


/*
 * Strip_clearfd
 */
int     Strip_clearfd   (Strip the_strip, int fd)
{
  StripInfo     *si = (StripInfo *)the_strip;
  int           i;

  for (i = 0; i < STRIP_MAX_FDS; i++)
    if (si->fdinfo[i].fd == fd)
      break;

  if (i < STRIP_MAX_FDS)
  {
    XtRemoveInput (si->fdinfo[i].id);
    si->fdinfo[i].id = 0;
  }
  
  return (i < STRIP_MAX_FDS);
}



/*
 * Strip_go
 */
void    Strip_go        (Strip the_strip)
{
  StripInfo     *si = (StripInfo *)the_strip;
  int           some_curves_connected = 0;
  int           some_curves_waiting = 0;
  int           i;
  XEvent        xevent;

  for (i = 0; i < STRIP_MAX_CURVES; i++)
  {
    if (si->curves[i].status & STRIPCURVE_CONNECTED)
      some_curves_connected = 1;
    if (si->curves[i].connect_request.tv_sec != 0)
      some_curves_waiting = 1;
  }

  if (some_curves_connected)
    window_map (si->display, XtWindow(si->shell));
  else if (some_curves_waiting)
    fprintf (stdout, "Strip_go: waiting for connection...\n");
  else StripDialog_popup (si->dialog);

  /* register the events */
  Strip_dispatch (si);

  XFlush (si->display);

  /* wait for file IO activities on all registered file descriptors */
  while (!(si->status & STRIPSTAT_TERMINATED))
  {
    XtAppNextEvent (si->app, &xevent);
    XtDispatchEvent (&xevent);
  }
}



/*
 * Strip_getcurve
 */
StripCurve      Strip_getcurve  (Strip the_strip)
{
  StripInfo     *si = (StripInfo *)the_strip;
  StripCurve    ret_val = NULL;
  int           i, j;

  for (i = 0; i < STRIP_MAX_CURVES; i++)
    if (!si->curves[i].details)
      break;

  if (i < STRIP_MAX_CURVES)
  {
    /* now find an available StripCurveDetail in the StripConfig */
    for (j = 0; j < STRIP_MAX_CURVES; j++)
      if (si->config->Curves.Detail[j].id == NULL)
        break;

    if (j < STRIP_MAX_CURVES)
    {
      si->curves[i].details = &si->config->Curves.Detail[j];
      si->curves[i].details->id = &si->curves[i];
      ret_val = (StripCurve)&si->curves[i];
    }
  }

  return ret_val;
}


/*
 * Strip_freecurve
 */
void    Strip_freecurve (Strip the_strip, StripCurve the_curve)
{
  StripInfo             *si = (StripInfo *)the_strip;
  StripCurveInfo        *sci = (StripCurveInfo *)the_curve;
  StripCurve            curve[2];
  int                   i;

  for (i = 0; (i < STRIP_MAX_CURVES) && (sci != &si->curves[i]); i++);

  if (i < STRIP_MAX_CURVES)
  {
    curve[0] = the_curve;
    curve[1] = (StripCurve)0;
    Strip_freesomecurves (the_strip, curve);
  }
}


/*
 * Strip_freesomecurves
 */
void    Strip_freesomecurves    (Strip the_strip, StripCurve curves[])
{
  StripInfo             *si = (StripInfo *)the_strip;
  int                   i;

  for (i = 0; curves[i]; i++)
  {
    StripGraph_removecurve (si->graph, curves[i]);
    StripDataSource_removecurve (si->data, curves[i]);
    if (si->disconnect_func != NULL)
      si->disconnect_func (curves[i], si->disconnect_data);
  }

  if (i > 0)
  {
    StripDialog_removesomecurves (si->dialog, curves);
    StripGraph_draw
      (si->graph,
	  SGCOMPMASK_LEGEND | SGCOMPMASK_DATA | SGCOMPMASK_YAXIS,
	  (Region *)0);

    for (i = 0; curves[i]; i++)
    {
      StripCurve_clearstat
        ((StripCurve)curves[i], STRIPCURVE_CONNECTED | STRIPCURVE_WAITING);
      Strip_forgetcurve (si, curves[i]);
    }
  }
}


/*
 * Strip_connectcurve
 */
int     Strip_connectcurve      (Strip the_strip, StripCurve the_curve)
{
  StripInfo             *si = (StripInfo *)the_strip;
  StripCurveInfo        *sci = (StripCurveInfo *)the_curve;
  int                   ret_val;

#if DEBUG_CONNECTING
  print("%s Strip_connectcurve\"  %s\n",
    timeStamp(), sci->details->name);
#endif	

  if (si->connect_func != NULL)
  {
#if 0
    StripCurve_setstat
      (the_curve, STRIPCURVE_WAITING | STRIPCURVE_CHECK_CONNECT);
#else
    StripCurve_setstat (the_curve, STRIPCURVE_WAITING);
#endif
    get_current_time (&sci->connect_request);
      
    if ((ret_val = si->connect_func (the_curve, si->connect_data)))
      if (!StripCurve_getstat (the_curve,  STRIPCURVE_CONNECTED))
        Strip_setwaiting (the_strip, the_curve);
    StripDialog_addcurve (si->dialog, the_curve);
  }
  else
  {
    fprintf (stderr,
	"Strip_connectcurve:\n"
	"  Warning! no connection routine\n");
    ret_val = 0;
  }

  return ret_val;
}


/*
 * Strip_setconnected
 */
void    Strip_setconnected      (Strip the_strip, StripCurve the_curve)
{
  StripInfo             *si = (StripInfo *)the_strip;

#if DEBUG_CONNECTING
  print("%s Strip_setconnected\"  %s\n",
    timeStamp(), sci->details->name);
#endif	

/* add the curve to the various modules only once --the first time it
 * is connected
 */
  if (!StripCurve_getstat (the_curve, STRIPCURVE_CONNECTED))
  {
    StripDataSource_addcurve (si->data, the_curve);
    StripGraph_addcurve (si->graph, the_curve);
    /* KE: This causes a problem with CDE if you are in another
	 workspace when it occurs.  The si->shell window is unmapped
	 (why you don't see it) even if it is mapped when you are in the
	 StripTool workspace.  This call causes it to be mapped in the
	 present workspace, which is a nuisance.  Strip_setconnected
	 gets called on both connection and on reconnection.  By putting
	 this logic here, the popup occurs on conection when you are
	 likely still in the Strip Tool workspace and doesn't occur on
	 reconnection when you are likely to be in another workspace.  */
    if (!window_ismapped (si->display, XtWindow (si->shell)))
	window_map (si->display, XtWindow (si->shell));
  }
  StripCurve_clearstat
    (the_curve, STRIPCURVE_WAITING | STRIPCURVE_CHECK_CONNECT);
  StripCurve_setstat (the_curve, STRIPCURVE_CONNECTED);
  
  Strip_watchevent (si, STRIPEVENTMASK_SAMPLE | STRIPEVENTMASK_REFRESH);
  
  StripGraph_draw
    (si->graph,
	SGCOMPMASK_DATA | SGCOMPMASK_LEGEND | SGCOMPMASK_YAXIS,
	(Region *)0);

  StripDialog_update_curvestat (si->dialog, the_curve);
}


/*
 * Strip_setdescconnected
 */
void    Strip_setdescconnected      (Strip the_strip, StripCurve the_curve)
{
  StripInfo             *si = (StripInfo *)the_strip;

#if DEBUG_CONNECTING
  print("%s Strip_setdescconnected\"  %s\n",
    timeStamp(), sci->details->name);
#endif	

 /* don't do anything unless it is already connected */
  if (StripCurve_getstat (the_curve, STRIPCURVE_CONNECTED))
  {
    StripGraph_draw
	(si->graph,
	  SGCOMPMASK_LEGEND,
	  (Region *)0);
  }
}


/*
 * Strip_setwaiting
 */
void    Strip_setwaiting        (Strip the_strip, StripCurve the_curve)
{
  StripInfo             *si = (StripInfo *)the_strip;

#if DEBUG_CONNECTING
  print("%s Strip_setwaiting\"  %s\n",
    timeStamp(), sci->details->name);
#endif	

#if 0
  StripCurve_setstat
    (the_curve, STRIPCURVE_WAITING | STRIPCURVE_CHECK_CONNECT);
#else
  StripCurve_setstat (the_curve, STRIPCURVE_WAITING);
  Strip_watchevent (si, STRIPEVENTMASK_CHECK_CONNECT);
#endif
  
  StripDialog_update_curvestat (si->dialog, the_curve);
}


/*
 * Strip_clear
 */
void    Strip_clear     (Strip the_strip)
{
  StripInfo             *si = (StripInfo *)the_strip;
  StripCurve            curves[STRIP_MAX_CURVES+1];
  int                   i, j;
  StripConfigMask       mask;

  if(auto_scaleTriger==1) Strip_auto_scale(the_strip);

  Strip_ignoreevent
    (si,
	STRIPEVENTMASK_SAMPLE |
	STRIPEVENTMASK_REFRESH |
	STRIPEVENTMASK_CHECK_CONNECT);
  
  for (i = 0, j = 0; i < STRIP_MAX_CURVES; i++)
    if (si->curves[i].details)
    {
      curves[j] = (StripCurve)(&si->curves[i]);
      j++;
    }

  curves[j] = (StripCurve)0;
  Strip_freesomecurves ((Strip)si, curves);
#if 0
  /* KE: This causes a segmentation fault when the first new pv is
     added after a File|Clear.  It doesn't seem to be necessary.  This
     is the only place it is used. */
  StripDataSource_removecurveAll(si->data); /* Albert */
#endif  

  StripConfig_setattr (si->config, STRIPCONFIG_TITLE, 0, 0);
  StripConfig_setattr (si->config, STRIPCONFIG_FILENAME, 0, 0);
  StripConfigMask_clear (&mask);
  StripConfigMask_set (&mask, SCFGMASK_TITLE);
  StripConfigMask_set (&mask, SCFGMASK_FILENAME);
  StripConfig_update (si->config, mask);
}

/* -----------------Albert : Strip_refresh  ------- */
void    Strip_refresh     (Strip the_strip)
{
  StripInfo             *si = (StripInfo *)the_strip;

  StripDataSource_refresh(si->data);

  StripGraph_setstat (si->graph, SGSTAT_GRAPH_REFRESH);
  StripGraph_setstat (si->graph, SGSTAT_LEGEND_REFRESH);
  StripGraph_draw
    (si->graph,
	SGCOMPMASK_TITLE | SGCOMPMASK_DATA | SGCOMPMASK_LEGEND | SGCOMPMASK_YAXIS,
	(Region *)0);
}

int    Strip_auto_scale     (Strip the_strip)
{
  StripInfo             *si = (StripInfo *)the_strip;
  
  if(auto_scaleTriger==1)       auto_scaleTriger=0;
  else if(auto_scaleTriger==0)  auto_scaleTriger=1;
  else if(auto_scaleTriger==-1) auto_scaleTriger=1;
  
  XtVaSetValues
    (si->btn[STRIPBTN_AUTO_SCALE],
	XmNlabelType,		XmPIXMAP,
	XmNlabelPixmap,		auto_scalePixmap[auto_scaleTriger],
	NULL);

  
  if (changeMinMax(the_strip) == 1) {
    StripGraph_setstat (si->graph, SGSTAT_GRAPH_REFRESH);
    StripGraph_setstat (si->graph, SGSTAT_LEGEND_REFRESH);
    StripGraph_draw
      (si->graph,
	  SGCOMPMASK_DATA | SGCOMPMASK_LEGEND | SGCOMPMASK_YAXIS,
	  (Region *)0);
  }
  XtRealizeWidget (si->btn[STRIPBTN_AUTO_SCALE]); 
  return (0);
}

static int changeMinMax( Strip the_strip)
{
  StripInfo             *si = (StripInfo *)the_strip;
  int i;
  double  widgetMin, widgetMax;
  int need_refresh = 0;

  if (auto_scaleTriger == 1)
  {
    need_refresh=StripAuto_min_max(si->data,(char *)si->graph);
  }
  else if (auto_scaleTriger == 0)
  {
    for (i = 0; i < STRIP_MAX_CURVES; i++)
    {
	if (!si->curves[i].details) continue;
	if (si->curves[i].details->plotstat != STRIPCURVE_PLOTTED) continue;
	getwidgetval_min(si->dialog,i,&widgetMin);
	getwidgetval_max(si->dialog,i,&widgetMax);
	if (si->curves[i].details->min != widgetMin) 
	{si->curves[i].details->min=widgetMin;need_refresh =1;}
	if (si->curves[i].details->max != widgetMax) 
	{si->curves[i].details->max=widgetMax;need_refresh =1;}
    }
  }
  else
  {
    return (0);
  }

  return(need_refresh);
}


/* -----------------Albert end -------------------------------------*/


/*
 * Strip_dump
 */
int     Strip_dumpdata  (Strip the_strip, char *fname)
{
  StripInfo             *si = (StripInfo *)the_strip;
  FILE                  *f;
  int                   ret_val = 0;
  int                   i = 0;

  if ((ret_val = ((f = fopen (fname, "w")) != NULL)))
  {
    if (DFSDLG_TGL_COUNT > 1)
    {
      for (i = 0; i < DFSDLG_TGL_COUNT; i++)
        if (XmToggleButtonGetState(si->fs_tgl[i])) break;
	switch (i)
	{
	case DFSDLG_TGL_ASCII:
	  ret_val = StripGraph_dumpdata (si->graph, f);
	  break;
	case DFSDLG_TGL_CSV:
	  ret_val = StripGraph_dumpdata_csv (si->graph, f);
	  break;
#ifdef USE_SDDS
	case DFSDLG_TGL_SDDS:
	  ret_val = StripGraph_dumpdata_sdds (si->graph, fname);
	  break;
#endif
	}
    }
    else
      ret_val = StripGraph_dumpdata (si->graph, f);
    if (!ret_val)
      MessageBox_popup
        (si->shell, &si->message_box, XmDIALOG_ERROR, "File I/O", "OK",
	    "Unable to dump data");
    fclose (f);
  }
  else
    MessageBox_popup
      (si->shell, &si->message_box, XmDIALOG_ERROR, "File I/O", "OK",
	  "Unable to open file for writing.\nname: %s\nerror: %s",
	  fname, strerror (errno));
  return ret_val;
}


/*
 * Strip_writeconfig
 */
int     Strip_writeconfig       (Strip                  the_strip,
  FILE                   *f,
  StripConfigMask        m,
  char                   *fname)
{
  StripInfo     *si = (StripInfo *)the_strip;
  int           ret_val;

  if ((ret_val = StripConfig_write (si->config, f, m)))
  {
    StripConfig_setattr (si->config, STRIPCONFIG_FILENAME, fname, 0);
    StripConfig_setattr (si->config, STRIPCONFIG_TITLE, basename (fname), 0);
    StripConfigMask_set (&m, SCFGMASK_FILENAME);
    StripConfigMask_set (&m, SCFGMASK_TITLE);
    StripConfig_update (si->config, m);
  }
  return ret_val;
}

/*
 * Strip_readconfig
 */
int     Strip_readconfig        (Strip                  the_strip,
  FILE                   *f, 
  StripConfigMask        m,
  char                   *fname)
{
  StripInfo     *si = (StripInfo *)the_strip;
  int           ret_val;
  
  if ((ret_val = StripConfig_load (si->config, f, m)))
  {
    StripConfig_setattr (si->config, STRIPCONFIG_FILENAME, fname, 0);
    StripConfig_setattr (si->config, STRIPCONFIG_TITLE, basename (fname), 0);
    StripConfigMask_set (&m, SCFGMASK_FILENAME);
    StripConfigMask_set (&m, SCFGMASK_TITLE);
    StripConfig_update (si->config, m);
  }
  return ret_val;
}



/* ====== Static Functions ====== */

/*
 * Strip_forgetcurve
 */
static void     Strip_forgetcurve       (StripInfo      *si,
  StripCurve     the_curve)
{
  StripCurveInfo        *sci = (StripCurveInfo *)the_curve;

  StripCurve_clearstat
    (the_curve,
	STRIPCURVE_EGU_SET | STRIPCURVE_COMMENT_SET | STRIPCURVE_PRECISION_SET |
	STRIPCURVE_MIN_SET | STRIPCURVE_MAX_SET | STRIPCURVE_SCALE_SET);
  StripConfig_reset_details (si->config, sci->details);
  sci->details = 0;
  sci->get_value = 0;
  sci->func_data = 0;
}


/*
 * Strip_graphdrop_handle
 *
 *      Handles drop events on the graph drawing area widget.
 */
static void     Strip_graphdrop_handle  (Widget         w,
  XtPointer      BOGUS(1),
  XtPointer      call)
{
  StripInfo                     *si;
  Display                       *dpy;
  Widget                        dc;
  XmDropProcCallback            drop_data;
  XmDropTransferEntryRec        xfer_entries[10];
  XmDropTransferEntry           xfer_list;
  Cardinal                      n_export_targets;
  Atom                          *export_targets;
  Atom                          COMPOUND_TEXT;
  Arg                           args[10];
  Cardinal                      i;
  int                           n;
  Boolean                       ok;

  dpy = XtDisplay (w);

  XtVaGetValues (w, XmNuserData, &si, NULL);
  drop_data = (XmDropProcCallback) call;
  dc = drop_data->dragContext;

  /* retrieve the data targets, and search for COMPOUND_TEXT */
  COMPOUND_TEXT = XmInternAtom (dpy, "COMPOUND_TEXT", False);
  
  n = 0;
  XtSetArg (args[n], XmNexportTargets, &export_targets); n++;
  XtSetArg (args[n], XmNnumExportTargets, &n_export_targets); n++;
  XtGetValues (dc, args, n);

  for (i = 0, ok = False; (i < n_export_targets) && !ok; i++)
    if ((ok = (export_targets[i] == COMPOUND_TEXT)))
      break;

  n = 0;
  if (!ok || (drop_data->dropAction != XmDROP))
  {
    XtSetArg (args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
    XtSetArg (args[n], XmNnumDropTransfers, 0); n++;
  }
  else
  {
    xfer_entries[0].target = COMPOUND_TEXT;
    xfer_entries[0].client_data = (char *)si;
    xfer_list = xfer_entries;
    XtSetArg (args[n], XmNdropTransfers, xfer_entries); n++;
    XtSetArg (args[n], XmNnumDropTransfers, 1); n++;
    XtSetArg (args[n], XmNtransferProc, Strip_graphdrop_xfer); n++;
  }

  XmDropTransferStart (dc, args, n);
}



/*
 * Strip_graphdrop_xfer
 *
 *      Handles the transfer of data initiated by the drop handler
 *      routine.
 */
static void     Strip_graphdrop_xfer    (Widget         BOGUS(w),
  XtPointer      client,
  Atom           *BOGUS(sel_type),
  Atom           *type,
  XtPointer      value,
  unsigned long  *BOGUS(length),
  int            *BOGUS(format))
{
  StripInfo     *si = (StripInfo *)client;
  StripCurve    curve;
  Atom          COMPOUND_TEXT;
  XmString      xstr;
  char          *name;

  COMPOUND_TEXT = XmInternAtom (si->display, "COMPOUND_TEXT", False);
  if (*type == COMPOUND_TEXT)
  {
    xstr = XmCvtCTToXmString ((char *)value);
    XmStringGetLtoR (xstr, XmFONTLIST_DEFAULT_TAG, &name);

    if ((curve = Strip_getcurve ((Strip)si)))
    {
      StripCurve_setattr (curve, STRIPCURVE_NAME, name, 0);
      Strip_connectcurve ((Strip)si, curve);
    }
  }
}


/*
 * Strip_config_callback
 */
static void     Strip_config_callback   (StripConfigMask mask, void *data)
{
  StripInfo             *si = (StripInfo *)data;
  StripCurveInfo        *sci;
  char                  str_buf[512];
  Widget                w_dlg;

  struct                _dcon
  {
    StripCurve          curves[STRIP_MAX_CURVES+1];
    int                 n;
  } dcon;
  
  struct                _conn
  {
    StripCurve          curves[STRIP_MAX_CURVES+1];
    int                 n;
  } conn;
  
  unsigned      comp_mask = 0;
  int           i, j;


  if (StripConfigMask_stat (&mask, SCFGMASK_TITLE))
  {
    StripGraph_setattr
      (si->graph, STRIPGRAPH_HEADING, si->config->title, 0);
    comp_mask |= SGCOMPMASK_TITLE;

    StripDialog_getattr (si->dialog, STRIPDIALOG_SHELL_WIDGET, &w_dlg, 0);

    /* update the window and icon titles */
    if (si->config->title)
    {
      sprintf (str_buf, "%s Graph", si->config->title);
      XtVaSetValues
        (si->shell,
	    XmNtitle,      str_buf,
	    XmNiconName,   si->config->title,
	    NULL);
      sprintf (str_buf, "%s Controls", si->config->title);
      XtVaSetValues
        (w_dlg,
	    XmNtitle,      str_buf,
	    XmNiconName,   si->config->title,
	    NULL);
    }
    else
    {
      XtVaSetValues
        (si->shell,
	    XmNtitle,      STRIPGRAPH_TITLE,
	    XmNiconName,   STRIPGRAPH_ICON_NAME,
	    NULL);
      XtVaSetValues
        (w_dlg,
	    XmNtitle,      STRIPDIALOG_TITLE,
	    XmNiconName,   STRIPDIALOG_ICON_NAME,
	    NULL);
    }
  }

  if (StripConfigMask_intersect (&mask, &SCFGMASK_TIME))
  {
    /* If the plotted timespan has changed, then the graph needs to be
     * redrawn and the data buffer resized.  Force a refresh event by
     * setting the last refresh time to 0 and calling dispatch().  If
     * the sample interval has changed then the data buffer must be
     * resized.  In any time-related event, the dispatch function needs
     * to be called.
     *
     * 8/18/97: if in browse mode, do not force refresh, but set the
     *          graph's time range appropriately, and then force it
     *          to redraw by setting the component mask.
     */
    if (StripConfigMask_stat (&mask, SCFGMASK_TIME_TIMESPAN))
    {
      if (si->status & STRIPSTAT_BROWSE_MODE)
      {
        struct timeval  t0, t1;
	  
        StripGraph_getattr (si->graph, STRIPGRAPH_END_TIME, &t1, 0);
        dbl2time (&tv, si->config->Time.timespan);
        subtract_times (&t0, &tv, &t1);
	  
        StripGraph_setattr
          (si->graph,
		STRIPGRAPH_BEGIN_TIME,       &t0,
		STRIPGRAPH_END_TIME,         &t1,
		0);
        comp_mask |=  SGCOMPMASK_DATA | SGCOMPMASK_XAXIS;
      }
      else
	{
	  si->last_event[STRIPEVENT_REFRESH].tv_sec = 0;
	}
    }
    if (StripConfigMask_stat (&mask, SCFGMASK_TIME_NUM_SAMPLES))
    {
      StripDataSource_setattr
        (si->data, SDS_NUMSAMPLES, (size_t)si->config->Time.num_samples, 0);
    }
      
    Strip_dispatch (si);
  }


  /* colors */
  if (StripConfigMask_intersect (&mask, &SCFGMASK_COLOR))
  {
    if (StripConfigMask_stat (&mask, SCFGMASK_COLOR_BACKGROUND))
    {
      /* set the canvas' background color so that when an expose
       * occurs we won't see ugly colors */
      XtVaSetValues
        (si->graph_form,
	    XmNbackground, si->config->Color.background.xcolor.pixel,
	    NULL);
      XtVaSetValues
        (si->canvas,
	    XmNbackground, si->config->Color.background.xcolor.pixel,
	    NULL);
      
      /* have to redraw everything when the background color changes */
      comp_mask |= SGCOMPMASK_ALL;
      StripGraph_setstat
        (si->graph, SGSTAT_GRAPH_REFRESH | SGSTAT_LEGEND_REFRESH);
    }
    if (StripConfigMask_stat (&mask, SCFGMASK_COLOR_FOREGROUND))
    {
      comp_mask |= SGCOMPMASK_YAXIS | SGCOMPMASK_XAXIS | SGCOMPMASK_LEGEND;
      StripGraph_setstat (si->graph, SGSTAT_LEGEND_REFRESH);
    }
    if (StripConfigMask_stat (&mask, SCFGMASK_COLOR_GRID))
    {
      comp_mask |= SGCOMPMASK_GRID;
    }

    /* if any of the curves have changed color, must redraw */
    for (i = 0; i < STRIP_MAX_CURVES; i++)
      if (StripConfigMask_stat
	  (&mask, (StripConfigMaskElement)SCFGMASK_COLOR_COLOR1+i))
      {
        /* no way of knowing if the curve whose color changed was the
         * one defining the yaxis color */
        comp_mask |= SGCOMPMASK_DATA | SGCOMPMASK_LEGEND | SGCOMPMASK_YAXIS;
        StripGraph_setstat
          (si->graph, SGSTAT_GRAPH_REFRESH | SGSTAT_LEGEND_REFRESH);
      }
  }

  
  /* graph options */
  if (StripConfigMask_intersect (&mask, &SCFGMASK_OPTION))
  {
    if (StripConfigMask_stat (&mask, SCFGMASK_OPTION_GRID_XON))
    {
      comp_mask |= SGCOMPMASK_GRID;
    }
    if (StripConfigMask_stat (&mask, SCFGMASK_OPTION_GRID_YON))
    {
      comp_mask |= SGCOMPMASK_GRID;
    }
    if (StripConfigMask_stat (&mask, SCFGMASK_OPTION_AXIS_YCOLORSTAT))
    {
      comp_mask |= SGCOMPMASK_YAXIS;
    }
    if (StripConfigMask_stat (&mask, SCFGMASK_OPTION_GRAPH_LINEWIDTH))
    {
      comp_mask |= SGCOMPMASK_DATA;
      StripGraph_setstat (si->graph, SGSTAT_GRAPH_REFRESH);
    }
  }

  if (StripConfigMask_stat (&mask, SCFGMASK_CURVE_NAME))
  {
    dcon.n = conn.n = 0;
    for (i = 0; i < STRIP_MAX_CURVES; i++)
    {
      if (StripConfigMask_stat
	  (&si->config->Curves.Detail[i].update_mask,
	    SCFGMASK_CURVE_NAME))
      {
        if ((sci = (StripCurveInfo *)si->config->Curves.Detail[i].id)
	    != NULL)
        {
          /* the detail structure is used by some StripCurve */
          if (StripCurve_getstat
		(sci, STRIPCURVE_CONNECTED | STRIPCURVE_WAITING))
            dcon.curves[dcon.n++] = (StripCurve)sci;
        }
        else
        {
          /* find an available StripCurve to contain the details */
          for (j = 0; j < STRIP_MAX_CURVES; j++)
            if (!si->curves[j].details)
            {
              sci = &si->curves[j];
              sci->details = &si->config->Curves.Detail[i];
              sci->details->id = sci;
              break;
            }
        }
              
        if (sci != NULL)
          conn.curves[conn.n++] = (StripCurve)sci;
      }
    }

    /* disconnect any currently connected curves whose names have changed */
    for (i = 0; i < dcon.n; i++)
    {
      StripGraph_removecurve (si->graph, dcon.curves[i]);
      StripDataSource_removecurve (si->data, dcon.curves[i]);
      if (si->disconnect_func != NULL)
        si->disconnect_func (dcon.curves[i], si->disconnect_data);
      StripCurve_clearstat
        (dcon.curves[i], STRIPCURVE_CONNECTED | STRIPCURVE_WAITING);
      sci = (StripCurveInfo *)dcon.curves[i];
    }
    /* remove the disconnected curves from the dialog */
    if (dcon.n > 0)
    {
      dcon.curves[dcon.n] = (StripCurve)0;
      StripDialog_removesomecurves (si->dialog, dcon.curves);
    }
    /* finally attempt to connect all new curves */
    for (i = 0; i < conn.n; i++)
    {
      if (!Strip_connectcurve ((Strip)si, conn.curves[i]))
      {
        fprintf
          (stderr,
		"Strip_config_callback:\n"
		"  Warning! connect failed for curve, %s\n",
		(char *) StripCurve_getattr_val
		(conn.curves[i], STRIPCURVE_NAME));
        Strip_forgetcurve (si, conn.curves[i]);
      }
    }
  }

  if (StripConfigMask_intersect (&mask, &SCFGMASK_CURVE))
  {
    if (StripConfigMask_stat (&mask, SCFGMASK_CURVE_NAME))
    {
      comp_mask |= SGCOMPMASK_LEGEND | SGCOMPMASK_DATA | SGCOMPMASK_YAXIS;
      StripGraph_setstat
        (si->graph, SGSTAT_GRAPH_REFRESH | SGSTAT_LEGEND_REFRESH);
    }
    if (StripConfigMask_stat (&mask, SCFGMASK_CURVE_EGU))
    {
      comp_mask |= SGCOMPMASK_LEGEND;
      StripGraph_setstat (si->graph, SGSTAT_LEGEND_REFRESH);
    }
    if (StripConfigMask_stat (&mask, SCFGMASK_CURVE_PRECISION))
    {
      comp_mask |= SGCOMPMASK_LEGEND | SGCOMPMASK_YAXIS;
      StripGraph_setstat (si->graph, SGSTAT_LEGEND_REFRESH);
    }
    if (StripConfigMask_stat (&mask, SCFGMASK_CURVE_MIN))
    {
      comp_mask |= SGCOMPMASK_LEGEND | SGCOMPMASK_DATA | SGCOMPMASK_YAXIS;
      StripGraph_setstat
        (si->graph, SGSTAT_GRAPH_REFRESH | SGSTAT_LEGEND_REFRESH);
    }
    if (StripConfigMask_stat (&mask, SCFGMASK_CURVE_MAX))
    {
      comp_mask |= SGCOMPMASK_LEGEND | SGCOMPMASK_DATA | SGCOMPMASK_YAXIS;
      StripGraph_setstat
        (si->graph, SGSTAT_GRAPH_REFRESH | SGSTAT_LEGEND_REFRESH);
    }
      
    if (StripConfigMask_stat (&mask, SCFGMASK_CURVE_SCALE))
    {
      comp_mask |= SGCOMPMASK_LEGEND | SGCOMPMASK_DATA | SGCOMPMASK_YAXIS;
      StripGraph_setstat
        (si->graph, SGSTAT_GRAPH_REFRESH | SGSTAT_LEGEND_REFRESH);
    }
      
    if (StripConfigMask_stat (&mask, SCFGMASK_CURVE_PLOTSTAT))
    {
      /* re-arrange the plot order */
      int shift;
          
      for (i = 0; i < STRIP_MAX_CURVES; i++)
        if (si->curves[i].details)
          if (StripConfigMask_stat
		(&si->curves[i].details->update_mask, SCFGMASK_CURVE_PLOTSTAT))
          {
            for (j = 0, shift = 0; j < STRIP_MAX_CURVES; j++) {
              if (j == i) shift = 1;
		  si->config->Curves.plot_order[j] = j + shift;
            } 
            si->config->Curves.plot_order[STRIP_MAX_CURVES-1] = i;
            break;
          }
      
      comp_mask |= SGCOMPMASK_DATA;
      StripGraph_setstat (si->graph, SGSTAT_GRAPH_REFRESH);
    }
  }
#if 1
  /* Albert : */
  if ( StripConfigMask_stat(&mask, SCFGMASK_TIME_TIMESPAN) )
    if (auto_scaleTriger == 1)
    {
	if (changeMinMax(si)) { /* Albert */
	  StripGraph_setstat (si->graph, SGSTAT_GRAPH_REFRESH);
	  StripGraph_setstat (si->graph, SGSTAT_LEGEND_REFRESH);
	    
	  comp_mask=SGCOMPMASK_DATA | SGCOMPMASK_LEGEND | SGCOMPMASK_YAXIS | SGCOMPMASK_XAXIS;
	}
    }
#endif
  if (comp_mask) StripGraph_draw (si->graph, comp_mask, (Region *)0);
}



/*
 * Strip_eventmgr
 */
static void     Strip_eventmgr          (XtPointer arg, XtIntervalId *BOGUS(id))
{
  StripInfo             *si = (StripInfo *)arg;
  struct timeval        event_time;
  struct timeval        t;
  double                diff;
  unsigned              event;
  int                   i, n;

  for (event = 0; event < LAST_STRIPEVENT; event++)
  {
    /* only process desired events */
    if (!(si->event_mask & (1 << event))) continue;
    
    get_current_time (&event_time);
    diff = subtract_times (&tv, &event_time, &si->next_event[event]);
    if (diff <= STRIP_TIMER_ACCURACY)
    {
      si->last_event[event] = event_time;
      
      switch (event)
      {
	case STRIPEVENT_SAMPLE:
	  /* StripDataSource_sample (si->data); Albert*/
	  StripDataSource_sample (si->data,(char *)si->graph); /* Albert */
	  break;
	  
	case STRIPEVENT_REFRESH:
	  /* if we are in browse mode, then only refresh the graph if the
	   * current time falls within the displayed time range */
	  if (si->status & STRIPSTAT_BROWSE_MODE)
	  {
	    struct timeval    t0, t1;
	    
	    StripGraph_getattr
		(si->graph,
		  STRIPGRAPH_BEGIN_TIME, &t0,
		  STRIPGRAPH_END_TIME,   &t1,
		  0);
	    if ((compare_times (&event_time, &t0) >= 0) &&
		(compare_times (&event_time, &t1) <= 0))
		StripGraph_draw (si->graph, SGCOMPMASK_DATA, (Region *)0);
	  }
	  /* if we are not in browse mode then refresh the graph with
	   * the current time as the rightmost value */
	  else
	  {
	    dbl2time (&tv, si->config->Time.timespan);
	    subtract_times (&t, &tv, &event_time);
	    StripGraph_setattr
		(si->graph,
		  STRIPGRAPH_BEGIN_TIME, &t,
		  STRIPGRAPH_END_TIME,   &event_time,
		  0);
	    
	    if (auto_scaleNoBrowse == 1) 
	    {
		auto_scaleNoBrowse=0;
		if (auto_scaleTriger == 1) {
		  if (changeMinMax(si)) { /* Albert */
		    StripGraph_setstat (si->graph, SGSTAT_GRAPH_REFRESH);
		    StripGraph_setstat (si->graph, SGSTAT_LEGEND_REFRESH);
		    StripGraph_draw
			(si->graph,
			  SGCOMPMASK_DATA | SGCOMPMASK_LEGEND | SGCOMPMASK_YAXIS | SGCOMPMASK_XAXIS,
			  (Region *)0);
		  }
		}
	    }
	    else
		StripGraph_draw
		  (si->graph, SGCOMPMASK_XAXIS | SGCOMPMASK_DATA, (Region *)0);
	  }
	  break;
	  
	case STRIPEVENT_CHECK_CONNECT:
	  for (i = 0, n = 0; i < STRIP_MAX_CURVES; i++)
	    if (si->curves[i].details)
		if (StripCurve_getstat
		  ((StripCurve)&si->curves[i], STRIPCURVE_WAITING) &&
		  StripCurve_getstat
		  ((StripCurve)&si->curves[i], STRIPCURVE_CHECK_CONNECT))
		{
		  diff = subtract_times
		    (&tv, &si->curves[i].connect_request, &event_time);
		  if (diff >= STRIP_CONNECTION_TIMEOUT)
		  {
		    StripCurve_clearstat
			((StripCurve)&si->curves[i],
			  STRIPCURVE_CHECK_CONNECT);
		    n++;
		  }
		  if (n > 0) ; /* do nothing */
		}
	  break;
      }
    }
  }
  
  si->tid = (XtInputId)0;
  Strip_dispatch (si);
}


/*
 * Strip_watchevent
 */
static void     Strip_watchevent        (StripInfo *si, unsigned mask)
{
  si->event_mask |= mask;
  Strip_dispatch (si);
}

/*
 * Strip_ignoreevent
 */
static void     Strip_ignoreevent       (StripInfo *si, unsigned mask)
{
  si->event_mask &= ~mask;
  Strip_dispatch (si);
}


/*
 * Strip_dispatch
 */
static void     Strip_dispatch          (StripInfo *si)
{
  struct timeval        t;
  struct timeval        now;
  struct timeval        *next;
  double                interval[LAST_STRIPEVENT];
  int                   i;
  double                sec;

  /* first, if a timer has already been registered, disable it */
  if (si->tid != (XtIntervalId)0) XtRemoveTimeOut (si->tid);
  
  interval[STRIPEVENT_SAMPLE]           = si->config->Time.sample_interval;
  interval[STRIPEVENT_REFRESH]          = si->config->Time.refresh_interval;
  interval[STRIPEVENT_CHECK_CONNECT]    = (double)STRIP_CONNECTION_TIMEOUT;

  next = NULL;
  
  /* first determine the event time for each possible next event */
  for (i = 0; i < LAST_STRIPEVENT; i++)
  {
    if (!(si->event_mask & (1 << i))) continue;
      
#ifdef DEBUG_EVENT
    fprintf
      (stdout, "=> last %-15s:\n%s\n",
	  StripEventTypeStr[i],
	  time2str (&si->last_event[i]));
#endif
    dbl2time (&t, interval[i]);
    add_times (&si->next_event[i], &si->last_event[i], &t);

    if (next == NULL)
      next = &si->next_event[i];
    else if (compare_times (&si->next_event[i], next) <= 0)
      next = &si->next_event[i];
  }
  
  /* event time */
  /* now calculate the number of milliseconds until the next event */
  get_current_time (&now);

  if (next != NULL)
  {
#ifdef DEBUG_EVENT
    fprintf (stdout, "============\n");
    for (i = 0; i < LAST_STRIPEVENT; i++)
    {
      if (si->event_mask & (1 << i))
        fprintf
          (stdout, "=> Next %s\n%s\n",
		StripEventTypeStr[i],
		time2str (&si->next_event[i]));
    }
    fprintf (stdout, "=> Next Event\n%s\n", time2str (next));
    fprintf (stdout, "=> Current Time\n%s\n", time2str (&now));
    fprintf (stdout, "=> si->next_event - current_time = %lf (seconds)\n",
	sec = subtract_times (&t, &now, next));
    fprintf (stdout, "============\n");
#endif

    /* finally, register the timer callback */
    sec = subtract_times (&t, &now, next);
    if (sec < 0) sec = 0;
    si->tid = Strip_addtimeout ((Strip)si, sec, Strip_eventmgr, (XtPointer)si);
  }

#ifdef DEBUG_EVENT
  fflush (stdout);
#endif
}


/*
 * Strip_setup_printer
 */
static void     Strip_printer_init      (StripInfo *si)
{
  char  *printer, *device;
  char  *s;
  
  /* printer name */
  if ((printer = getenv (STRIP_PRINTER_NAME_ENV)))
  {
    device = getenv (STRIP_PRINTER_DEVICE_ENV);
    if (!device) device = STRIP_PRINTER_DEVICE_FALLBACK;
  }

  /* fallback printer info */
  else if ((printer = getenv (STRIP_PRINTER_NAME_FALLBACK_ENV)))
    device = STRIP_PRINTER_DEVICE_FALLBACK;

  else
  {
    printer = STRIP_PRINTER_NAME;
    device = STRIP_PRINTER_DEVICE;
  }

  s = si->print_info.printer;
  while (*printer) *s++ = *printer++;
  *s = 0;

  s = si->print_info.device;
  while (*device) *s++ = *device++;
  *s = 0;
}

/*
 * Strip_ignorexerror
 */
void     Strip_ignorexerror     (StripInfo *si, int browse)
{
}



/*
 * Strip_setbrowsemode
 */
static void     Strip_setbrowsemode     (StripInfo *si, int browse)
{
  if(browse)
  {
    si->status |= STRIPSTAT_BROWSE_MODE;
    XtVaSetValues (si->browse_lbl, XmNlabelString, xstr_panning, NULL);
    XtVaSetValues	(si->btn[STRIPBTN_AUTOSCROLL],
	XmNlabelType,		XmPIXMAP,
	XmNlabelPixmap,		browsePixmap[1],
	NULL);
  }
  else
  {
    auto_scaleNoBrowse = 1;
    si->status &= ~STRIPSTAT_BROWSE_MODE;
    XtVaSetValues (si->browse_lbl, XmNlabelString, xstr_notpanning, NULL);
    XtVaSetValues	(si->btn[STRIPBTN_AUTOSCROLL],
	XmNlabelType,		XmPIXMAP,
	XmNlabelPixmap,		browsePixmap[0],
	NULL);
  }
}


/*
 * callback
 */
static void     callback        (Widget w, XtPointer client, XtPointer call)
{
  static int                    x, y;

  XmDrawingAreaCallbackStruct   *cbs;
  XEvent                        *event;
  StripInfo                     *si;
#if 0
  StripConfigMask               scfg_mask;
#endif
  struct timeval                t, tb, t0, t1;
  Boolean                       statusChanged;


  cbs = (XmDrawingAreaCallbackStruct *)call;
  event = cbs->event;
  si = (StripInfo *)client;

  switch (cbs->reason)
  {
  case XmCR_PROTOCOLS:
#if 0
    dlgrequest_quit (si, 0);
#else
    /* Make it dismiss, not quit the application, the same as for the
       Controls dialog */
    if (StripDialog_ismapped (si->dialog) || StripDialog_isiconic (si->dialog))
    {
	window_unmap (si->display, XtWindow (si->shell));
    }
    else
    {
	int status = Question_popup(si->app, si->shell,
	  "This is the only remaining window and can't be dismissed.\n"
	  "Do you want to exit?");
	if(status == 1)
	{
	  dlgrequest_quit ((void *)si, NULL);
	}
    }
#endif    
    break;
    
  case XmCR_ACTIVATE:
    
    /*
     * Pan left or right
     */
    if (w == si->btn[STRIPBTN_LEFT] || w == si->btn[STRIPBTN_RIGHT])
    {
	if (event->xany.type == ButtonRelease &&
	  ((XButtonEvent *)event)->button == Button3)
	{
	  /* pan by twentieth a screen */
	  dbl2time (&t, si->config->Time.timespan * SMALL_PAN_FACTOR);
	}
	else
	{
	  /* pan by half a screen */
	  dbl2time (&t, si->config->Time.timespan * LARGE_PAN_FACTOR);
	}
	StripGraph_getattr (si->graph, STRIPGRAPH_END_TIME, &tb, 0);
	
	if (w == si->btn[STRIPBTN_LEFT])
	  /* t1 = tb - t */
	  subtract_times (&t1, &t, &tb);
	else
	  /* t1 = tb + t */
	  add_times (&t1, &t, &tb);
	
	/* go into browse mode, and redraw the graph with the new range */
	Strip_setbrowsemode (si, True);
	
	dbl2time (&t, si->config->Time.timespan);
	subtract_times (&t0, &t, &t1);
	
	StripGraph_setattr
	  (si->graph,
	    STRIPGRAPH_BEGIN_TIME,     &t0,
	    STRIPGRAPH_END_TIME,       &t1,
	    0);
	if (auto_scaleTriger != 1)
	  StripGraph_draw
	    (si->graph, SGCOMPMASK_DATA | SGCOMPMASK_XAXIS, (Region *)0);
	else {
	  if (changeMinMax(si)) { /* Albert */
	    StripGraph_setstat (si->graph, SGSTAT_GRAPH_REFRESH);
	    StripGraph_setstat (si->graph, SGSTAT_LEGEND_REFRESH);
	    StripGraph_draw
		(si->graph,
		  SGCOMPMASK_DATA | SGCOMPMASK_LEGEND | SGCOMPMASK_YAXIS | SGCOMPMASK_XAXIS,
		  (Region *)0);
	  }
	  else  StripGraph_draw
		    (si->graph,SGCOMPMASK_DATA|SGCOMPMASK_XAXIS,(Region *)0);
	}
	
    }
    
    else if (w == si->btn[STRIPBTN_UP] ||
	w == si->btn[STRIPBTN_DOWN])
    {
	int i;
	double width;
	double factor;
	for (i = 0; i < STRIP_MAX_CURVES; i++)
	{
	  if (!si->curves[i].details) continue;
	  if (si->curves[i].details->plotstat != STRIPCURVE_PLOTTED) 
	    continue;
	  width=si->curves[i].details->max - si->curves[i].details->min;
	  if (event->xany.type == ButtonRelease &&
	    ((XButtonEvent *)event)->button == Button3)
	  {
	    factor=SMALL_PAN_FACTOR;
	  }
	  else
	  {
	    factor=LARGE_PAN_FACTOR;
	  }
	  width *= factor;
	  if (w == si->btn[STRIPBTN_UP])
	  {
	    si->curves[i].details->max += width;
	    si->curves[i].details->min += width;
	  }
	  else 
	  {
	    si->curves[i].details->max -= width;
	    si->curves[i].details->min -= width;
	  }
	}
	StripGraph_setstat (si->graph, SGSTAT_GRAPH_REFRESH);
	StripGraph_setstat (si->graph, SGSTAT_LEGEND_REFRESH);
	StripGraph_draw
	  (si->graph,
	    SGCOMPMASK_DATA | SGCOMPMASK_LEGEND | SGCOMPMASK_YAXIS | 
	    SGCOMPMASK_XAXIS,(Region *)0);
    }
    
    /*
     * Zoom in or out
     *
     *      (1) set new end time of graph object (begin time is calculated
     *          as offset from end time)
     *      (2) turn on browse mode
     *      (3) set new timespan via StripConfig_setattr()
     *      (4) generate configuration update callbacks, causing
     *          the graph to be updated.
     */
    else if (w == si->btn[STRIPBTN_ZOOMIN] ||
	w == si->btn[STRIPBTN_ZOOMOUT])
    {
	struct timeval        t, tb, t1;
	unsigned              t_new;
	double factor;
	
	StripGraph_getattr (si->graph, STRIPGRAPH_END_TIME, &tb, 0);
	
	if (event->xany.type == ButtonRelease &&
	  ((XButtonEvent *)event)->button == Button3)
	{
	  factor=SMALL_ZOOM_FACTOR;
	}
	else
	{
	  factor=LARGE_ZOOM_FACTOR;
	}
	if (w == si->btn[STRIPBTN_ZOOMIN])
	{
	  /* subtract the appropriate amount from the end point and
           divide the range by the factor */
	  dbl2time (&t, si->config->Time.timespan * (.5 * (1. - 1. / factor)));
	  subtract_times (&t1, &t, &tb);
	  t_new = (unsigned int)(si->config->Time.timespan / factor);
	}
	else
	{
	  /* add the appropriate amount to the end point and multiply
           the range by the factor */
	  dbl2time (&t, si->config->Time.timespan * (.5 * (factor - 1.)));
	  add_times (&t1, &t, &tb);
	  t_new = (unsigned int)(si->config->Time.timespan * factor);
	}
	
	StripGraph_setattr (si->graph, STRIPGRAPH_END_TIME, &t1, 0);
#if 0
	/* KE: There is no reason to go in browse mode */
	Strip_setbrowsemode (si, True);
#endif
	StripConfig_setattr
	  (si->config, STRIPCONFIG_TIME_TIMESPAN, t_new, 0);
#if 0
	/* KE: This sets new values in the Control Dialog.  We don't
         want to do this.  These temporary values may be saved
         unknowingly by the user and changing them also keep us from
         doing a reset.  */
	StripConfigMask_clear (&scfg_mask);
	StripConfigMask_set (&scfg_mask, SCFGMASK_TIME_TIMESPAN);
	StripConfig_update (si->config, scfg_mask);
#else	
	StripGraph_setstat (si->graph, SGSTAT_GRAPH_REFRESH);
	StripGraph_draw
	  (si->graph, SGCOMPMASK_XAXIS, (Region *)0);
#endif
    }
    else if (w == si->btn[STRIPBTN_ZOOMINY] ||
	w == si->btn[STRIPBTN_ZOOMOUTY])
    {
	int i;
	double width;
	double factor;
	for (i = 0; i < STRIP_MAX_CURVES; i++)
	{
	  if (!si->curves[i].details) continue;
#if 0
	  /* KE: This means the scale will not change as the plot zooms
           if the selected curve is not plotted.  This is not what you
           want.  The max and min should change, but the curve should
           not be drawn is what you want. */
	  if (si->curves[i].details->plotstat != STRIPCURVE_PLOTTED) 
	    continue;
#endif
	  width = si->curves[i].details->max - si->curves[i].details->min;
	  if (event->xany.type == ButtonRelease &&
	    ((XButtonEvent *)event)->button == Button3)
	  {
	    factor=SMALL_ZOOM_FACTOR;
	  }
	  else
	  {
	    factor=LARGE_ZOOM_FACTOR;
	  }
	  if (w == si->btn[STRIPBTN_ZOOMOUTY])
	  {
	    width *= (.5 * (factor - 1.));
	    si->curves[i].details->max += width;
	    si->curves[i].details->min -= width;
	  }
	  else 
	  {
	    width *= (.5 * (1. - 1. / factor));
	    si->curves[i].details->max -= width;
	    si->curves[i].details->min += width;
	  }
	  /* fix roundoff near zero */
	  width = si->curves[i].details->max - si->curves[i].details->min;
	  if(width && fabs(si->curves[i].details->max/width) < ROFF)
	    si->curves[i].details->max = 0.;
	  if(width && fabs(si->curves[i].details->min/width) < ROFF)
	    si->curves[i].details->min = 0.;
	}
	StripGraph_setstat (si->graph, SGSTAT_GRAPH_REFRESH);
	StripGraph_setstat (si->graph, SGSTAT_LEGEND_REFRESH);
	StripGraph_draw
	  (si->graph,
	    SGCOMPMASK_DATA | SGCOMPMASK_LEGEND | SGCOMPMASK_YAXIS | 
	    SGCOMPMASK_XAXIS,(Region *)0);
    }
    
#ifdef  STRIP_HISTORY
    else if (w == si->btn[STRIPBTN_FROM_TO])   /* Albert */
    {
	time_t t;
	struct tm *tms;
	static char buf[32];
	int i;
	static char defaultTUString[LAST_TIME_UNIT][5];
	
	if(fromToShell == NULL)
	{ 
	  fromToShell=fromToDialog_init (si->toplevel, "From-To Window",si);
	}
	
	
	StripGraph_getattr (si->graph, STRIPGRAPH_BEGIN_TIME, &t, 0);
	tms=localtime((const time_t *) &t);
	sprintf(buf,"%04d%02d%02d%02d%02d",
	  1900+tms->tm_year,1+tms->tm_mon,tms->tm_mday,tms->tm_hour,tms->tm_min);
	sscanf(buf,"%4s%2s%2s%2s%2s",
	  &defaultTUString[0][0],
	  &defaultTUString[1][0],
	  &defaultTUString[2][0],
	  &defaultTUString[3][0],
	  &defaultTUString[4][0]);
	
	for(i=0;i<LAST_TIME_UNIT;i++)
	  XmTextSetString(timeUnitTextWidget[0][i],&defaultTUString[i][0]);
	
	StripGraph_getattr (si->graph, STRIPGRAPH_END_TIME, &t, 0);
	tms=localtime(&t);
	sprintf(buf,"%04d%02d%02d%02d%02d",
	  1900+tms->tm_year,1+tms->tm_mon,tms->tm_mday,tms->tm_hour,tms->tm_min);
	sscanf(buf,"%4s%2s%2s%2s%2s",
	  &defaultTUString[0][0],
	  &defaultTUString[1][0],
	  &defaultTUString[2][0],
	  &defaultTUString[3][0],
	  &defaultTUString[4][0]);
	
	for(i=0;i<LAST_TIME_UNIT;i++)
	  XmTextSetString(timeUnitTextWidget[1][i],&defaultTUString[i][0]);
	
	XMapRaised (XtDisplay (fromToShell), XtWindow (fromToShell));
	
    }
    
    else if (w == si->btn[STRIPBTN_ALG])   /* Albert */
    {
	static Widget tmp;
	int i;
	static Widget panel;
	static Widget radio_box;
	static Widget arch_btn;
	static Widget btn_radio_close;
	static Widget btn_radio_help;
	
	static Widget nopPanel;
	static Widget text_nop;
	static Widget label_nop;
	static Widget refresh_nop;
	static char buf[32];
	if(algShell == NULL) 
	{
	  algShell = XtVaCreatePopupShell
	    ("AlgDialog",
		topLevelShellWidgetClass, 	si->toplevel,
		XmNtitle,			"AlgDialog", 
		NULL);
	  panel = XmCreateRowColumn(algShell , "panel", NULL, 0);
	  XtManageChild(panel);
	  
	  radio_box = XmCreateRadioBox(panel,"radio_box",NULL,0);
	  XtVaSetValues(radio_box,
	    XmNnumColumns,algorithmLength,
	    XmNpacking,XmPACK_TIGHT,
	    XmNuserData, (XtPointer)si,
	    NULL);
	  for (i = 0; i < algorithmLength; i++) {
	    tmp=XtVaCreateManagedWidget(algorithmString[i],
		xmToggleButtonGadgetClass, radio_box,
		XmNset, i == AVERAGE_METHOD-1,
		NULL);
	    XtAddCallback(tmp,XmNvalueChangedCallback,radio_toggled,
		(XtPointer) i );
	  }
#ifndef USE_AAPI/*only under AAPI possible choose reduction algorithms right now */ 
	  XtSetSensitive(radio_box,False);
#endif
	  XtManageChild(radio_box);
	  
	  arch_btn = XtVaCreateManagedWidget("Show points",
	    xmToggleButtonGadgetClass,panel, NULL);
	  XtAddCallback (arch_btn,XmNvalueChangedCallback,arch_callback,NULL);
	  XtVaSetValues (arch_btn, XmNuserData, (XtPointer)si, NULL);
	  
	  nopPanel = XmCreateRowColumn(panel, "panel", NULL, 0);
	  XtVaSetValues(nopPanel, XmNorientation,XmHORIZONTAL,NULL);
	  
	  label_nop=XtVaCreateManagedWidget("Points:",xmLabelGadgetClass,
	    nopPanel,NULL);
	  
	  sprintf(buf,"%d",DEFAULT_HISTORY_SIZE);
	  
	  text_nop=XtVaCreateManagedWidget("tex_nop",
	    xmTextFieldWidgetClass,nopPanel,
	    XmNcolumns,6,
	    NULL);
	  XmTextSetString(text_nop,buf);
	  XtAddCallback(text_nop,XmNmodifyVerifyCallback,allDigit,NULL);
	  
	  refresh_nop=XtVaCreateManagedWidget("Refresh #",
	    xmPushButtonWidgetClass, nopPanel, 0);
	  
	  XtVaSetValues (refresh_nop, XmNuserData, (XtPointer)si, NULL);     
	  
	  XtAddCallback (refresh_nop,XmNactivateCallback,historyPoints,
	    text_nop);
	  
	  XtManageChild(nopPanel );
	  
	  btn_radio_help = XtVaCreateManagedWidget
	    ("Help", xmPushButtonWidgetClass,panel, 0);
	  XtAddCallback (btn_radio_help,XmNactivateCallback,radioHelp,NULL);
	  btn_radio_close = XtVaCreateManagedWidget
	    ("Close", xmPushButtonWidgetClass,panel, NULL);
	  XtAddCallback (btn_radio_close,XmNactivateCallback,radioClose,NULL);
	  
	  XtManageChild(btn_radio_close);
	  XtManageChild(btn_radio_help);
	  XtRealizeWidget (algShell);
	}
	
	XMapRaised (XtDisplay (algShell), XtWindow (algShell));
    }

    else if (w == si->btn[STRIPBTN_REPLOT])
    {
	if (auto_scaleTriger != 1) Strip_refresh(si);
	else 
	{
	  if(StripAuto_min_max(si->data,(char *)si->graph))
	  {
	    StripGraph_setstat (si->graph, SGSTAT_GRAPH_REFRESH);
	    StripGraph_setstat (si->graph, SGSTAT_LEGEND_REFRESH);
	    StripGraph_draw
		(si->graph,
		  SGCOMPMASK_DATA | SGCOMPMASK_LEGEND | SGCOMPMASK_YAXIS,
		  (Region *)0);
	  }
	}
    }
    
#endif /* STRIP_HISTORY */
    
    /*
     * Reset the screen to values in the StripDialog
     */
    else if (w == si->btn[STRIPBTN_RESET])
    {
	if(si->dialog) StripDialog_reset(si->dialog);
	else XBell(si->display, 50);
    }

    /*
     * Refresh the screen
     */
    else if (w == si->btn[STRIPBTN_REFRESH])
    {
#if 1
	Strip_refresh(si);	
#else
	/* KE: This only refreshes the data.  It doesn't fix legend
         problems, etc. */
	StripGraph_setstat (si->graph, SGSTAT_GRAPH_REFRESH);
	StripGraph_draw (si->graph, SGCOMPMASK_DATA, (Region *)0);
#endif	
    }

    /*
     *Toggle auto scale
     */
    else if (w == si->btn[STRIPBTN_AUTO_SCALE])
    {
	Strip_auto_scale(si);
    }    
    
    /*
     * Toggle scroll/pan
     */
    else if (w == si->btn[STRIPBTN_AUTOSCROLL])
    {
	Strip_setbrowsemode (si, !(si->status & STRIPSTAT_BROWSE_MODE));
    }
    
    break;
    
  case XmCR_INPUT:
    
    if (event->xany.type == ButtonPress)
    {
	x = event->xbutton.x;
	y = event->xbutton.y;
	
	/* if this is the third button, then popup the menu */
	if (event->xbutton.button == Button3)
	{
	  PopupMenu_position
	    (si->popup_menu, (XButtonPressedEvent *)cbs->event);
	  PopupMenu_popup (si->popup_menu);
	}
	
        else if (event->xbutton.button == Button1)
        {
          if (w == si->canvas ) {
            /* Toggle annotation select/deselect */
            Annotation_select ((XButtonEvent *)event,si->annotation_info);

            /* refresh the graph */
            /* time to 0 and calling dispatch(). */
            StripGraph_draw (si->graph, SGCOMPMASK_DATA, (Region *)0);
          }
        }
        else if (event->xbutton.button == Button2)
        {
          if (w == si->canvas  &&
            Annotation_select((XButtonEvent *)event,si->annotation_info)) {

            statusChanged = False;
            /* if not paused, pause the graph by putting it in browse mode */
            if (!si->status & STRIPSTAT_BROWSE_MODE) {
              statusChanged = True;
              Strip_setbrowsemode (si, True);
            }
	  
            /* Move annotation */
            Annotation_move ((XButtonEvent *)event,si->annotation_info);

	    /* resume the graph by taking it out of  browse mode */
            if (statusChanged) {
              Strip_setbrowsemode (si, False);
            } else {
              /* refresh the graph */
              /* time to 0 and calling dispatch(). */
              StripGraph_draw (si->graph, SGCOMPMASK_DATA, (Region *)0);
            }
          }
        }
    }
    
    else if (event->xany.type == ButtonRelease)
    {
	/* nothing here at the moment! */
    }
    break;
  }
}

void    Strip_handlexerrors (void)
{
    XSetErrorHandler (X_error_handler);
}


void    Strip_ignorexerrors (void)
{
    XSetErrorHandler (X_ignore_error);
}


/* X_error_handler
 */
static int      X_ignore_error          (Display * BOGUS(display),
  XErrorEvent * BOGUS(error))
{
  return 0;     /* not used? */
}

/* X_error_handler
 */
static int      X_error_handler         (Display *display, XErrorEvent *error)
{
  char  msg[128];

  XGetErrorText (display, error->error_code, msg, 128);
#ifdef WIN32
  fprintf
    (stderr,
	"==== StripTool X Error Handler ====\n"
	"error:         %s\n"
	"major opcode:  %d\n"
	"serial:        %lu\n",
	msg, (int)error->request_code, error->serial);
#else
  fprintf
    (stderr,
	"==== StripTool X Error Handler ====\n"
	"error:         %s\n"
	"major opcode:  %d\n"
	"serial:        %lu\n"
	"process ID:    %d\n",
	msg, (int)error->request_code, error->serial, (int)getpid());
#endif
  
  /* remember error */
  Strip_x_error_code = error->error_code;
  
  return 0;     /* not used? */
}

#ifdef TRAP_XT_ERRORS
/* Xt_error_handler
 */
void            Xt_error_handler        (String msg)
{
  fprintf
    (stderr,
	"==== StripTool Xt Error Handler ====\n"
	"error:         %s\n",
	msg);
#if 1
  exit(1);
#endif  
}

/* Xt_warning_handler
 */
void            Xt_warning_handler      (String msg)
{
  fprintf
    (stderr,
	"==== StripTool Xt Warning Handler ====\n"
	"warning:         %s\n",
	msg);
}
#endif

/*
 * dlgrequest_connect
 */
static void     dlgrequest_connect      (void *client, void *call)
{
  StripInfo     *si = (StripInfo *)client;
  char          *name = (char *)call;
  StripCurve    curve;
  
  if ((curve = Strip_getcurve ((Strip)si)) != 0)
  {
    StripCurve_setattr (curve, STRIPCURVE_NAME, name, 0);
    Strip_connectcurve ((Strip)si, curve);
  }
}


/*
 * dlgrequest_show
 */
static void     dlgrequest_show         (void *client, void *BOGUS(1))
{
  StripInfo     *si = (StripInfo *)client;
  window_map (si->display, XtWindow (si->shell));
}


/*
 * dlgrequest_clear
 */
static void     dlgrequest_clear        (void *client, void *BOGUS(1))
{
  StripInfo     *si = (StripInfo *)client;
  Strip_clear ((Strip)si);
}


/*
 * dlgrequest_delete
 */
static void     dlgrequest_delete       (void *client, void *call)
{
  StripInfo     *si = (StripInfo *)client;
  StripCurve    curve  = (StripCurve)call;

  Strip_freecurve ((Strip)si, curve);
}


/*
 * dlgrequest_dismiss
 */
static void     dlgrequest_dismiss      (void *client, void *BOGUS(1))
{
  StripInfo     *si = (StripInfo *)client;
  Widget        tmp_w;
  
  if (window_ismapped (si->display, XtWindow (si->shell)) ||
    window_isiconic (si->display, XtWindow (si->shell)))
  {
    StripDialog_popdown (si->dialog);
  }  
  else
  {
    int status;
    
    StripDialog_getattr (si->dialog, STRIPDIALOG_SHELL_WIDGET, &tmp_w, 0);
    status = Question_popup(si->app, tmp_w,
	"This is the only remaining window and can't be dismissed.\n"
	"Do you want to exit?");
    if(status == 1)
    {
	dlgrequest_quit ((void *)si, NULL);
    }
  }
}


/*
 * dlgrequest_quit
 */
static void     dlgrequest_quit (void *client, void *BOGUS(1))
{
  StripInfo     *si = (StripInfo *)client;
  
  Annotation_deleteAll(si->annotation_info);
  dlgrequest_clear (client, NULL);
  window_unmap (si->display, XtWindow(si->shell));
  StripDialog_popdown (si->dialog);
  si->status |= STRIPSTAT_TERMINATED;
  Strip_ignoreevent
    (si,
	STRIPEVENTMASK_SAMPLE |
	STRIPEVENTMASK_REFRESH |
	STRIPEVENTMASK_CHECK_CONNECT);
}


/*
 * dlgrequest_window_popup
 */
static void     dlgrequest_window_popup (void *client, void *call)
{
  
  StripInfo             *si = (StripInfo *)client;
  StripWindowType       which = (StripWindowType)call;

  switch (which)
  {
  case STRIPWINDOW_GRAPH:
    window_map (si->display, XtWindow (si->shell));
    break;
  case STRIPWINDOW_COUNT:
    break;
  }
}



/* ====== Popup Menu stuff ====== */
typedef enum
{
  POPUPMENU_CONTROLS = 0,
  POPUPMENU_TOGGLE_SCROLL,
  POPUPMENU_NEW_ANNOTATION,
  POPUPMENU_DELETE_ANNOTATION,
  POPUPMENU_EDIT_ANNOTATION,
  POPUPMENU_PRINTER_SETUP,
  POPUPMENU_PRINT,
  POPUPMENU_SNAPSHOT,
  POPUPMENU_DUMP,
  POPUPMENU_RETRY,
  POPUPMENU_DISMISS,
  POPUPMENU_QUIT,
  POPUPMENU_ITEMCOUNT
}
PopupMenuItem;

char    *PopupMenuItemStr[POPUPMENU_ITEMCOUNT] =
{
  "Controls Dialog...",
  "Toggle Buttons",
  "Annotate selected curve",
  "Delete selected annotation",
  "Edit selected annotation",
  "Printer Setup...",
  "Print",
  "Snapshot",
  "Dump Data...",
  "Retry Connections",
  "Dismiss",
  "Quit",
};

char    PopupMenuItemMnemonic[POPUPMENU_ITEMCOUNT] =
{
  'C',
  'T',
  'A',
  'l',
  'E',
  'r',
  'P',
  'S',
  'D',
  'R',
  'm',
  'Q'
};

char    *PopupMenuItemAccelerator[POPUPMENU_ITEMCOUNT] =
{
  " ",
  " ",
  " ",
  " ",
  " ",
  " ",
  " ",
  " ",
  " ",
  " ",
  " ",
  "Ctrl<Key>c"
};

char    *PopupMenuItemAccelStr[POPUPMENU_ITEMCOUNT] =
{
  " ",
  " ",
  " ",
  " ",
  " ",
  " ",
  " ",
  " ",
  " ",
  " ",
  " ",
  "Ctrl+C"
};

/*
 * PopupMenu_build
 */
static Widget   PopupMenu_build (Widget parent)
{
  Widget        menu;
  XmString      label[POPUPMENU_ITEMCOUNT+3];
  XmString      acc_lbl[POPUPMENU_ITEMCOUNT+3];
  KeySym        keySyms[POPUPMENU_ITEMCOUNT+3];
  XmButtonType  buttonType[POPUPMENU_ITEMCOUNT+3];
  int           i;
  Arg           args[16];
  int           n;


  for (i = 0; i < POPUPMENU_ITEMCOUNT+3; i++)
    buttonType[i] = XmPUSHBUTTON;

  buttonType[2] = buttonType[6] = buttonType[10] = XmSEPARATOR;

  for (i = 0, n = 0; i < POPUPMENU_ITEMCOUNT+3; i++)
  {
    if (buttonType[i] == XmPUSHBUTTON) {
      label[i]   = XmStringCreateLocalized (PopupMenuItemStr[n]);
      acc_lbl[i] = XmStringCreateLocalized (PopupMenuItemAccelStr[n]);
      keySyms[i] = PopupMenuItemMnemonic[n];
      n++;
    }
    else {
      label[i]   = XmStringCreateLocalized ("Separator");
      acc_lbl[i] = XmStringCreateLocalized (" ");
      keySyms[i] = ' ';
    }
  }

  n = 0;
  XtSetArg(args[n], XmNbuttonCount,           POPUPMENU_ITEMCOUNT+3); n++;
  XtSetArg(args[n], XmNbuttonType,            buttonType); n++;
  XtSetArg(args[n], XmNbuttons,               label); n++;
  XtSetArg(args[n], XmNbuttonMnemonics,       keySyms); n++;
  XtSetArg(args[n], XmNbuttonAcceleratorText, acc_lbl); n++;
  XtSetArg(args[n], XmNsimpleCallback,        PopupMenu_cb); n++;

  menu = XmCreateSimplePopupMenu(parent, "popup", args, n);
 
  for (i = 0; i < POPUPMENU_ITEMCOUNT+3; i++)
  {
    if (label[i])   XmStringFree (label[i]);
    if (acc_lbl[i]) XmStringFree (acc_lbl[i]);
  }

  return menu;
}

/*
 * PopupMenu_position
 */
static void     PopupMenu_position      (Widget                 menu,
  XButtonPressedEvent    *event)
{
  XmMenuPosition (menu, event);
}


/*
 * PopupMenu_popup
 */
static void     PopupMenu_popup (Widget menu)
{
  XtManageChild (menu);
}


#if 0
/* KE: Not used */
/*
 * PopupMenu_popdown
 */
static void     PopupMenu_popdown (Widget menu)
{
  XtUnmanageChild (menu);
}
#endif

/*
 * PopupMenu_cb
 */
static void     PopupMenu_cb    (Widget w, XtPointer client, XtPointer BOGUS(1))
{
  PopupMenuItem item = (PopupMenuItem)client;
  StripInfo     *si;
#ifndef WIN32
  char          cmd_buf[256];
  pid_t         pid;
#endif  

  XtVaGetValues (XtParent(w), XmNuserData, &si, NULL);

  switch (item)
  {
  case POPUPMENU_CONTROLS:
    StripDialog_popup (si->dialog);
    break;
        
  case POPUPMENU_TOGGLE_SCROLL:
    if (XtIsManaged (si->graph_panel))
    {
	XtUnmanageChild (si->graph_panel);
	XtVaSetValues
	  (si->graph_form, XmNbottomAttachment, XmATTACH_FORM, NULL);
    }
    else
    {
	XtManageChild (si->graph_panel);
	XtVaSetValues
	  (si->graph_form,
	    XmNbottomAttachment,       XmATTACH_WIDGET,
	    XmNbottomWidget,           si->graph_panel,
	    NULL);
    }
    break;


  case POPUPMENU_PRINTER_SETUP:
    PrinterDialog_popup (si->pd, si);
    break;
        
  case POPUPMENU_NEW_ANNOTATION:

    /* popup annotation dialog */
    AnnotateDialog_popup (si->annotation_info, (XtPointer)1);
    break;
        
  case POPUPMENU_DELETE_ANNOTATION:

    /* delete selected annotation */
    Annotation_deleteSelected (si->annotation_info);
    break;
        
  case POPUPMENU_EDIT_ANNOTATION:

    /* popup annotation dialog */
    AnnotateDialog_popup (si->annotation_info, (XtPointer)0);
    break;
        
  case POPUPMENU_PRINT:
    window_map (si->display, XtWindow(si->shell));
#ifdef DESY_PRINT   
    sprintf
	(cmd_buf,
	  "xwpick -window %ld"
	  " -format %s"
	  " | lpr -P%s ",
	  XtWindow (si->graph_form),
	  si->print_info.device,
	  si->print_info.printer);
    /*printf(" cmd_buf=%s\n",cmd_buf );*/
    if (!(pid = fork ()))
    {
	/* Child (pid=0) */
	execl ("/bin/sh", "sh", "-c", cmd_buf, 0);
	exit (0);
    }
#elif defined(WIN32) /* DESY_PRINT */
    /* KE: This could be made to work on WIN32 by using the routines
     * in MEDM rather than using xwd.  For now just ding.  Note that
     * Alt-PrintScreen dumps the window to the clipboard, where a
     * graphics routine such as Paint can retrieve and print it.  */
    XBell (si->display,50); XBell (si->display,50); XBell (si->display,50);
    MessageBox_popup
	(si->shell, &si->message_box, XmDIALOG_INFORMATION, "Print", "OK",
	  "Printing is not available on WIN32.\n\n"
	  "Alt+PrintSceen will put the window in the clipboard\n"
	  "where you can use Paint or another program to\n"
	  "display or print it.");
#else /* DESY_PRINT */
# if defined(SOLARIS)
    if (strcmp (si->print_info.device, "ps") == 0)
	sprintf
	  (cmd_buf,
	    "xwd -id %u | xwdtopnm | pnmtops | lp -d%s",
	    (unsigned)XtWindow (si->graph_form),
	    si->print_info.printer);
    else
# endif
	sprintf
	  (cmd_buf,
	    "xwd -id %d | xpr -device %s | lp -d%s",
	    (int)XtWindow (si->graph_form),
	    si->print_info.device,
	    si->print_info.printer);
    if (!(pid = fork ()))
    {
	/* Child (pid=0) */
	execl ("/bin/sh", "sh", "-c", cmd_buf, NULL);
	exit (0);
    }
#endif /* DESY_PRINT */
    break;
        
  case POPUPMENU_SNAPSHOT:
    window_map (si->display, XtWindow(si->shell));
#ifdef WIN32
    XBell (si->display,50); XBell (si->display,50); XBell (si->display,50);
    MessageBox_popup
	(si->shell, &si->message_box, XmDIALOG_INFORMATION, "Snapshot", "OK",
	  "Snapshot is not available on WIN32.\n\n"
	  "Alt+PrintSceen will put the window in the clipboard\n"
	  "where you can use Paint or another program to\n"
	  "display or print it.");
#else    
    sprintf
	(cmd_buf,
	  "xwd -id %d | xwud -raw",
	  (int)XtWindow (si->graph_form));
    if (!(pid = fork ()))
    {
	/* Child (pid=0) */
	execl ("/bin/sh", "sh", "-c", cmd_buf, NULL);
	exit (0);
    }
#endif
    break;
        
  case POPUPMENU_DUMP:
    fsdlg_popup ((Strip)si, (fsdlg_functype)Strip_dumpdata);
    break;
        
  case POPUPMENU_RETRY:
    StripDAQ_retry_connections(si->daq, si->display);
    break;
    
  case POPUPMENU_DISMISS:
    if (StripDialog_ismapped (si->dialog) || StripDialog_isiconic (si->dialog))
    {
	window_unmap (si->display, XtWindow (si->shell));
    }
    else
    {
	int status = Question_popup(si->app, si->shell,
	  "This is the only remaining window and can't be dismissed.\n"
	  "Do you want to exit?");
	if(status == 1)
	{
	  dlgrequest_quit ((void *)si, NULL);
	}
    }
    break;
        
  case POPUPMENU_QUIT:
    dlgrequest_quit ((void *)si, NULL);
    break;
        
  default:
    fprintf (stderr, "Argh!\n");
    exit (1);
  }
}

static PrinterDialog    *PrinterDialog_build    (Widget parent)
{
  PrinterDialog *pd;
  Widget        base;
  Pixel         bg, fg;

  pd = (PrinterDialog *)malloc (sizeof (PrinterDialog));
  if (!pd) return 0;
  
#ifndef WIN32
  pd->msgbox = XmCreateTemplateDialog (parent, "PrinterDialog", 0, 0);
#else
  pd->msgbox = XmCreateMessageDialog (parent, "PrinterDialog", 0, 0);
#endif
  XtAddCallback (pd->msgbox, XmNokCallback, PrinterDialog_cb, 0);

  base = XtVaCreateManagedWidget
    ("baseForm", xmFormWidgetClass, pd->msgbox, NULL);

  pd->name_textf = XtVaCreateManagedWidget
    ("nameTextF", xmTextFieldWidgetClass, base, NULL);
  pd->device_combo = XtVaCreateManagedWidget
    ("deviceCombo", xgComboBoxWidgetClass, base, NULL);
  XtVaCreateManagedWidget ("nameLabel", xmLabelWidgetClass, base, NULL);
  XtVaCreateManagedWidget ("deviceLabel", xmLabelWidgetClass, base, NULL);

  /* combo box won't use default CDE colors for text & list */
  XtVaGetValues (pd->name_textf, XmNbackground, &bg, XmNforeground, &fg, NULL);
  XtVaSetValues
    (pd->device_combo,
	XgNlistForeground,         fg,
	XgNtextForeground,         fg,
	XgNlistBackground,         bg,
	XgNtextBackground,         bg,
	NULL);

  return pd;
}


static void     PrinterDialog_popup     (PrinterDialog *pd, StripInfo *si)
{
#ifdef WIN32
    XBell (si->display,50); XBell (si->display,50); XBell (si->display,50);
    MessageBox_popup
	(si->shell, &si->message_box, XmDIALOG_INFORMATION, "Snapshot", "OK",
	  "Printing is not available on WIN32.\n\n"
	  "Alt+PrintSceen will put the window in the clipboard\n"
	  "where you can use Paint or another program to\n"
	  "display or print it.");
#else    
  Window                root, child;
  Dimension             width, height;
  int                   root_x, root_y;
  int                   win_x, win_y;
  unsigned              mask;
  
  /* set the data fields appropriately */
  XmTextFieldSetString (pd->name_textf, si->print_info.printer);
  XgComboBoxSetString (pd->device_combo, si->print_info.device, False);

  /* find out where the pointer is */
  XQueryPointer
    (XtDisplay (pd->msgbox), DefaultRootWindow (XtDisplay (pd->msgbox)),
	&root, &child, &root_x, &root_y, &win_x, &win_y, &mask);

  /* place dialog box so it centers the window on the screen */
  XtVaGetValues (pd->msgbox, XmNwidth, &width, XmNheight, &height, NULL);

  win_x = root_x - (width / 2);         if (win_x < 0) win_x = 0;
  win_y = root_y - (height / 2);        if (win_y < 0) win_y = 0;

  XtVaSetValues
    (pd->msgbox,
	XmNx,              (Dimension)win_x,
	XmNy,              (Dimension)win_y,
	XmNuserData,       si,                     /* need for callback */
	NULL);

  /* pop it up! */
  XtManageChild (pd->msgbox);
#endif
}


static void     PrinterDialog_cb        (Widget         w,
  XtPointer      BOGUS(client),
  XtPointer      call)
{
  XmAnyCallbackStruct   *cbs = (XmAnyCallbackStruct *)call;
  StripInfo             *si;
  char                  *str, *b;

  if (cbs->reason == XmCR_OK)
  {
    XtVaGetValues (w, XmNuserData, &si, NULL);

    /* get printer name, if not empty string */
    str = XmTextFieldGetString (si->pd->name_textf);
    b = si->print_info.printer;
    if (str)
    {
	char *a = str;
	while (*a) *b++ = *a++; *b = 0;
	XtFree (str);
    }
    
    /* get device name, if not empty string */
    str = XgComboBoxGetString (si->pd->device_combo);
    b = si->print_info.device;
    if (str)
    {
	char *a = str;
	while (*a) *b++ = *a++; *b = 0;
	XtFree (str);
    }
  }
}



static void     fsdlg_popup     (StripInfo *si, fsdlg_functype func)
{
  Widget        w,frame;
  int           i;
  XmString      xstr;
  char          *ftype;

  if (!si->fs_dlg)
  {
    si->fs_dlg = XmCreateFileSelectionDialog
      (si->shell, "Data dump file", NULL, 0);
    XtVaSetValues (si->fs_dlg, XmNfileTypeMask, XmFILE_REGULAR, NULL);
    XtAddCallback (si->fs_dlg, XmNokCallback, fsdlg_cb, si);
    XtAddCallback (si->fs_dlg, XmNcancelCallback, fsdlg_cb, NULL);

    if (DFSDLG_TGL_COUNT > 1)
    {
      frame = XtVaCreateManagedWidget
        ("frame",
	    xmFrameWidgetClass,              si->fs_dlg,
	    NULL);
      XtVaCreateManagedWidget
        ("File Type",
	    xmLabelWidgetClass,              frame,
	    XmNchildType,                    XmFRAME_TITLE_CHILD,
	    NULL);
      w = XtVaCreateManagedWidget
        ("rowcol",
	    xmRowColumnWidgetClass,  frame,
	    XmNchildType,                    XmFRAME_WORKAREA_CHILD,
	    XmNradioBehavior,                True,
	    NULL);
      for (i = 0; i < DFSDLG_TGL_COUNT; i++)
      {
        xstr = XmStringCreateLocalized (DfsDlgTglStr[i]);
        si->fs_tgl[i] = XtVaCreateManagedWidget
          ("togglebutton",
		xmToggleButtonWidgetClass,     w,
		XmNlabelString,                xstr,
		NULL);
        XmStringFree (xstr);
      }
	XmToggleButtonSetState(si->fs_tgl[0],True,True);

	/* set default dump file type */
	if ((ftype = getenv (STRIP_DUMP_TYPE_DEFAULT_ENV)))
	  for (i = 0; i < DFSDLG_TGL_COUNT; i++)
	    if (strcmp(DfsDlgTglStr[i],ftype) == 0)
		XmToggleButtonSetState(si->fs_tgl[i],True,True);
    }
  }
  XtVaSetValues (si->fs_dlg, XmNuserData, func, NULL);
  XtManageChild (si->fs_dlg);
}


static void     fsdlg_cb        (Widget w, XtPointer data, XtPointer call)
{
  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call;
  
  StripInfo             *si = (StripInfo *)data;
  fsdlg_functype        func;
  char                  *fname;

  XtUnmanageChild (w);

  if (si)
  {
    if (XmStringGetLtoR (cbs->value, XmFONTLIST_DEFAULT_TAG, &fname))
    {
      if (fname != NULL)
      {
        XtVaGetValues (w, XmNuserData, &func, NULL);
        func ((Strip)si, fname);
	  XtFree(fname);
      }
    }
  }
}


/* Albert: */
#define DEBUG 0

#ifdef STRIP_HISTORY

static Widget fromToDialog_init (Widget parent, char* title, StripInfo* si)
{
  static Widget shell,btn_ok,btn_close, base_form, rowBase,rowcol[3];
  static Widget timeUnitArrowUpWidget  [2][LAST_TIME_UNIT];
  static Widget timeUnitArrowDownWidget[2][LAST_TIME_UNIT];
  static Widget rowcolForArrows        [2][LAST_TIME_UNIT];
  static goData gData;
  static arrowData aData[2][LAST_TIME_UNIT];
  static int i,j;
  
  shell = XtVaCreatePopupShell
    ("FromToDialog",
	topLevelShellWidgetClass, 	parent,
	XmNdeleteResponse,		XmUNMAP,
	XmNmappedWhenManaged,		False,
	XmNtitle,			title == NULL? "FromToDialog" : title,
	NULL);
  
  base_form = XtVaCreateManagedWidget
    ("form",
	xmFormWidgetClass,		shell,
	/*       XmNfractionBase,			9, */
	XmNnoResize,			True,
	XmNresizable,			False,
	XmNresizePolicy,			XmRESIZE_NONE,
	XmNverticalSpacing,		DEF_WOFFSET,
	XmNhorizontalSpacing,		DEF_WOFFSET,
	NULL);
  
  rowBase = XtVaCreateManagedWidget
    ("rowBase",
	xmRowColumnWidgetClass,		base_form,
	XmNtopAttachment,		XmATTACH_FORM,
	XmNbottomAttachment,		XmATTACH_FORM,
	XmNleftAttachment,		XmATTACH_FORM,
	XmNrightAttachment,		XmATTACH_FORM,
	XmNorientation,                  XmVERTICAL,
	NULL);
  
  for(i=0;i<2;i++)
  {
    rowcol[i] = XtVaCreateManagedWidget
      ("TimeUnitsRowColumn",
	  xmRowColumnWidgetClass,		rowBase,
	  XmNtopAttachment,		(i==0)?XmATTACH_FORM:XmATTACH_NONE,
	  XmNbottomAttachment,		XmATTACH_NONE,
	  XmNleftAttachment,		XmATTACH_FORM,
	  XmNrightAttachment,		XmATTACH_FORM,
	  XmNorientation,                  XmHORIZONTAL,
	  NULL);
    
    XtVaCreateManagedWidget((i == 0)?"From:":"   To:",
	xmLabelGadgetClass,rowcol[i],NULL);
    
    /* separator */
    XtVaCreateManagedWidget
      ("separator",
	  xmSeparatorWidgetClass, 	rowcol[i],
	  XmNorientation,		XmVERTICAL,
	  NULL);
    for(j=0;j<LAST_TIME_UNIT;j++)
    {
	
	/* Label */
	XtVaCreateManagedWidget(timeUnitString[j],xmLabelGadgetClass,rowcol[i],
	  NULL);
	/* Text */
	
	timeUnitTextWidget[i][j]=
	  XtVaCreateManagedWidget("Text",xmTextFieldWidgetClass,
	    rowcol[i],
	    XmNcolumns,   (j == 0)?4:2, 
	    XmNmaxLength, (j == 0)?4:2,
	    NULL);
	
	XtAddCallback(timeUnitTextWidget[i][j], XmNmodifyVerifyCallback, 
	  allDigit, NULL);
	
	rowcolForArrows[i][j] = XtVaCreateManagedWidget
	  ("ArrowRowColumn",
	    xmRowColumnWidgetClass,		rowcol[i],
	    XmNorientation,                  XmVERTICAL,
	    NULL);
	
	
	/* Increment/Decrement arrows */
      timeUnitArrowUpWidget[i][j]= 
	  XtVaCreateManagedWidget ("arr1", xmArrowButtonGadgetClass,
	    rowcolForArrows[i][j],
	    XmNarrowDirection, XmARROW_UP,
	    NULL);
	
      aData[i][j].num=j;
      aData[i][j].w=timeUnitTextWidget[i][j];
	
      XtAddCallback (timeUnitArrowUpWidget[i][j], XmNactivateCallback, 
	  stepUp, &aData[i][j]);
	
	
      timeUnitArrowDownWidget[i][j]= 
	  XtVaCreateManagedWidget ("arr2", xmArrowButtonGadgetClass, 
	    rowcolForArrows[i][j],
	    XmNarrowDirection, XmARROW_DOWN,
	    NULL);
      XtAddCallback (timeUnitArrowDownWidget[i][j], XmNactivateCallback, 
	  stepDown,&aData[i][j] );
	
      XtVaCreateManagedWidget
	  ("separator",
	    xmSeparatorWidgetClass, 	rowcol[i],
	    XmNorientation,		XmVERTICAL,
	    NULL);
	
	
    }

    /* separator */
    XtVaCreateManagedWidget
      ("separator",
	  xmSeparatorWidgetClass, 	rowBase,
	  XmNorientation,		XmHORIZONTAL,
	  NULL);
    
  }
  
  rowcol[3] = XtVaCreateManagedWidget
    ("TimeUnitsRowColumn",
	xmRowColumnWidgetClass,		rowBase,
	XmNtopAttachment,		XmATTACH_NONE,
	XmNbottomAttachment,		XmATTACH_FORM,
	XmNleftAttachment,		XmATTACH_FORM,
	XmNrightAttachment,		XmATTACH_FORM,
	XmNorientation,                  XmHORIZONTAL,
	NULL);
  
  gData.si=si;
  gData.time_textFrom= &timeUnitTextWidget[0][0];
  gData.time_textTo  = &timeUnitTextWidget[1][0];
  
  btn_ok = XtVaCreateManagedWidget
    ("GO!", xmPushButtonWidgetClass, rowcol[3], NULL);
  XtAddCallback (btn_ok, XmNactivateCallback, fromToGo,
    &gData);
  
  
  btn_close = XtVaCreateManagedWidget
    ("Close", xmPushButtonWidgetClass, rowcol[3], 0);
  XtAddCallback (btn_close, XmNactivateCallback,fromToClose , NULL);
  
  XtRealizeWidget (shell);
  return shell;
}

static void allDigit(text_w, unused, cbs)
  Widget      text_w;
  XtPointer   unused;
  XmTextVerifyCallbackStruct *cbs;
{
  char c;
  int len = XmTextGetLastPosition(text_w);
  if (cbs->text->ptr == NULL) return; /* backspace */
  /* don't allow non-digits or let the input exceed 10 chars */
  if (!isdigit( (int) c = cbs->text->ptr[0]) || len >= 10) 
    cbs->doit = False;
}

static void stepUp (widget, aData, call_data)
  Widget                widget;
  arrowData             *aData;
  XmAnyCallbackStruct  *call_data;
{
  char buff[5];
  unsigned int data;
  Widget rec=aData->w;
  int num=aData->num;
  
  data = atoi( (const char *) XmTextGetString(rec));
  
  if(DEBUG) printf("stepUp data =%d\n",data);
  
  if (data < timUnitMax[num])
  {
    sprintf(buff,"%d",++data);
    XmTextSetString(rec,buff);
  }
/* check for unnumeric in dated string */
  if ( atoi( (const char *) XmTextGetString(rec)) <= 0) 
    XmTextSetString(rec,"0");
  
  if(DEBUG) printf("stepUp data =%d\n",data);
  
}

static void stepDown (widget, aData, call_data)
  Widget                widget;
  arrowData             *aData;
  XmAnyCallbackStruct  *call_data;
{
  char buff[5];
  unsigned int data;
  Widget rec=aData->w;
  int num=aData->num;
  
  data = atoi( (const char *) XmTextGetString(rec));
  
  if(DEBUG) printf("stepDown data =%d\n",data);
  
  if (data > timUnitMin[num])
  {
    sprintf(buff,"%d",--data);
    XmTextSetString(rec,buff);
  }
/* check for unnumeric in dated string */
  if ( atoi( (const char *) XmTextGetString(rec)) <= 0) 
    XmTextSetString(rec,"0");
  
  if(DEBUG) printf("stepDown data =%d\n",data);
  
}


static void fromToClose(widget, unused , call_data)
  Widget                widget;
  XtPointer             unused;
  XmAnyCallbackStruct  *call_data;
{
  XUnmapWindow (XtDisplay (fromToShell), XtWindow (fromToShell));
}

static void fromToGo(wdgt, gData , call_data)
  Widget                 wdgt;
  goData             *gData;
  XmAnyCallbackStruct  *call_data;
{
  struct tm       from_tm;
  struct tm         to_tm;
  long from_epoch;
  long   to_epoch;
  Widget *w0, *w1;
  struct timeval t_from;
  struct timeval t_to;
#if 0
  double                dval;  /* Albert */
  unsigned int          int_dval; /* Albert */
  unsigned int	       t_new;
#endif   
  long tmp_time;
  struct tm *tm;
  
  StripInfo *si= (StripInfo *) gData->si;
  StripConfigMask	scfg_mask;
  
  w0=gData->time_textFrom;
  w1=gData->time_textTo;
  
  from_tm.tm_sec = 0; 
  from_tm.tm_min  = atoi((const char *) XmTextGetString(w0[MINUTE]));
  from_tm.tm_hour = atoi((const char *) XmTextGetString(w0[HOUR])); 
  from_tm.tm_mday = atoi((const char *) XmTextGetString(w0[DAY])); 
  from_tm.tm_mon  = atoi((const char *) XmTextGetString(w0[MONTH])) -1;
  from_tm.tm_year = atoi((const char *) XmTextGetString(w0[YEAR])) -1900;
  from_tm.tm_isdst=-1;
  from_epoch = mktime( &from_tm );
  /* printf("from_epoch1=%d,%s",from_epoch,ctime(&from_epoch));*/
  
  if (from_epoch <= 0) {  
    MessageBox_popup
      (fromToShell,(Widget *)XmCreateMessageDialog(fromToShell,"Oops",NULL,0), 
	  XmDIALOG_WARNING, "TIME PROBLEM", "OK","From-Time Error\n");
    return;
  }
  
  to_tm.tm_sec = 0; 
  to_tm.tm_min  = atoi((const char *) XmTextGetString(w1[MINUTE]));
  to_tm.tm_hour = atoi((const char *) XmTextGetString(w1[HOUR])); 
  to_tm.tm_mday = atoi((const char *) XmTextGetString(w1[DAY])); 
  to_tm.tm_mon  = atoi((const char *) XmTextGetString(w1[MONTH])) -1; 
  to_tm.tm_year = atoi((const char *) XmTextGetString(w1[YEAR])) -1900;
  to_tm.tm_isdst=-1;
  
  to_epoch = mktime( &to_tm );
  /* printf("  to_epoch1=%d,%s",to_epoch,ctime(&to_epoch));  */
  if (to_epoch <= 0)  {
    MessageBox_popup
      (fromToShell,(Widget *)XmCreateMessageDialog(fromToShell,"Oops",NULL,0), 
	  XmDIALOG_WARNING, "TIME PROBLEM", "OK","To-Time Error\n");
    return;
  }
  
  if(from_epoch >= to_epoch) {
    MessageBox_popup
      (fromToShell,(Widget *)XmCreateMessageDialog(fromToShell,"Oops",NULL,0), 
	  XmDIALOG_WARNING, "TIME PROBLEM", "OK","From > TO!!! \n");
    return;
  } 
  t_from.tv_sec=from_epoch; t_from.tv_usec=0;
  t_to.tv_sec=  to_epoch;   t_to.tv_usec=0;
  
  if(DEBUG) printf("from=%s",ctime(&from_epoch));
  if(DEBUG) printf("to  =%s",ctime(&to_epoch));
  
  
#if 0
  /* For memory problem Albert: */
  t_new=to_epoch - from_epoch;
  
  StripConfig_getattr
    (si->config, STRIPCONFIG_TIME_SAMPLE_INTERVAL, &dval, 0);        
  
  if (t_new > MEMORY_RATIO)  /* Albert */ 
  {
    int_dval=(int) (t_new/MEMORY_RATIO);
    if (int_dval < 1) int_dval=1;
    dval = (double) int_dval ;           /* End Albert */
    
/* ?????          si->status |= STRIPSTAT_BROWSE_MODE;  */
    StripConfig_setattr
	(si->config,STRIPCONFIG_TIME_SAMPLE_INTERVAL,dval,0);
    StripConfigMask_clear (&scfg_mask);
    StripConfigMask_set 
	(&scfg_mask,STRIPCONFIG_TIME_SAMPLE_INTERVAL);
    StripConfig_update (si->config, scfg_mask);
  }
  
  if(dval > t_new/3 )
  {
    fprintf(stderr,"StripHistoryTool: Data_Sample_Interval SO BIG...\n");
    int_dval= (int) t_new/3;
    if (int_dval < 1) int_dval=1;
    dval = (double) int_dval ;           /* End Albert */
    
/* ?????          si->status |= STRIPSTAT_BROWSE_MODE;  */
    StripConfig_setattr
	(si->config,STRIPCONFIG_TIME_SAMPLE_INTERVAL,dval,0);
    StripConfigMask_clear (&scfg_mask);
    StripConfigMask_set 
	(&scfg_mask,STRIPCONFIG_TIME_SAMPLE_INTERVAL);
    StripConfig_update (si->config, scfg_mask);
  }
/* End of memory allocation problem */ 
#endif /* 0 */
  
  StripGraph_setattr (si->graph, STRIPGRAPH_END_TIME, &t_to, 0);
  StripGraph_setattr (si->graph, STRIPGRAPH_BEGIN_TIME, &t_from, 0);
  Strip_setbrowsemode (si, True);
  StripGraph_draw(si->graph,SGCOMPMASK_DATA|SGCOMPMASK_XAXIS,(Region *)0);
  
  if(DEBUG) printf("to_epoch - from_epoch =%ld",to_epoch - from_epoch);
  
  StripConfig_setattr
    (si->config, STRIPCONFIG_TIME_TIMESPAN,to_epoch-from_epoch, 0);
  StripConfigMask_clear (&scfg_mask);
  StripConfigMask_set (&scfg_mask, SCFGMASK_TIME_TIMESPAN);
  StripConfig_update (si->config, scfg_mask);
  
  
  if (auto_scaleTriger == 1) {
    if (changeMinMax(si)) {
	StripGraph_setstat (si->graph, SGSTAT_GRAPH_REFRESH);
	StripGraph_setstat (si->graph, SGSTAT_LEGEND_REFRESH);
	StripGraph_draw
	  (si->graph,
	    SGCOMPMASK_DATA | SGCOMPMASK_LEGEND | SGCOMPMASK_YAXIS | SGCOMPMASK_XAXIS,
	    (Region *)0);
    }
  }
  XUnmapWindow (XtDisplay (fromToShell), XtWindow (fromToShell));
}

static void radio_toggled(Widget widget,int which,XmToggleButtonCallbackStruct *state)
{
  StripInfo *si;
  StripConfigMask mask;
  char *titlePt;
  
  if(DEBUG)  printf("RADIO_BOX:which=%d\n",which);
  if(which != radioBoxAlgorithm)
  {
    
    radioBoxAlgorithm=which;
    XtVaGetValues (XtParent(widget), XmNuserData, &si, NULL);
    
    radioChange=1;
    
    StripConfig_getattr (si->config, STRIPCONFIG_TITLE,&titlePt, 0);
    StripConfig_setattr (si->config, STRIPCONFIG_TITLE,titlePt, 0);
    StripConfigMask_clear (&mask);
    StripConfigMask_set (&mask, SCFGMASK_TITLE);
    StripConfig_update (si->config, mask);
    
    /* new Albert */
    if (auto_scaleTriger == 1) {
	changeMinMax(si);/* Albert */
    }
    /* end new Albert */
    Strip_refresh((Strip) si);  
    
  }     
}

static void radioClose(widget, unused , call_data)
  Widget                widget;
  XtPointer             unused;
  XmAnyCallbackStruct  *call_data;
{
  XUnmapWindow( XtDisplay (XtParent((XtParent(widget)))), 
    XtWindow(XtParent((XtParent(widget)))) );
}

static void radioHelp(widget, unused , call_data)
  Widget                widget;
  XtPointer             unused;
  XmAnyCallbackStruct  *call_data;
{
  MessageBox_popup(history_topShell,
    (Widget *) XmCreateMessageDialog(history_topShell,"Help",NULL,0),
#ifndef WIN32
    XmDIALOG_INFORMATION, 
#else
    XmDIALOG_MESSAGE, 
#endif
    "ArchiveMethods", 
    "OK",
    "You can \n"
    " 1) choose   History   Reduction   Algorithm:\n"
    "     (available only in AAPI version)\n"
    "       a) Average Method -- IDL-style\n"
    "       b) Tail of raw data -- Show last 1024 real data from Archive \n"
    "       c) Sharp Method - try show all big (max/min)\n"
    "       d) Spline Method - smothh curve using cubic spline \n"
    "\n"
    " 2) show all reall points using little circle around all real point\n"
    "\n"
    " 3) change History Buffer Size (default is 1Kb)\n"
    "\n" );
}


static void arch_callback(widget, bit, toggle_data)
  Widget widget;
  int bit;
  XmToggleButtonCallbackStruct *toggle_data;
{
  StripInfo *si;
  arch_flag=1-arch_flag;
  
  XtVaGetValues (widget, XmNuserData, &si, NULL);
  
  radioChange=1;
  Strip_refresh((Strip) si); 
  /*
    StripGraph_setstat (si->graph, SGSTAT_GRAPH_REFRESH);
    StripGraph_setstat (si->graph, SGSTAT_LEGEND_REFRESH);
    StripGraph_draw
    (si->graph,
    SGCOMPMASK_DATA | SGCOMPMASK_LEGEND | SGCOMPMASK_YAXIS,
    (Region *)0);
  */
}

static void historyPoints(widget, textField , call_data)
  Widget                widget;
  XtPointer             textField;
  XmAnyCallbackStruct  *call_data;
{
  
  StripInfo *si;
  static char buf[32];
  Widget w= (Widget) textField;
  unsigned int tmp;
  
  tmp = (unsigned int) atoi(XmTextGetString(w));
  if((tmp==0) || (tmp>MAX_HISTORY_SIZE) ) {
    sprintf(buf,"%d",historySize);
    XmTextSetString(w,buf);
    XMapRaised (XtDisplay ( algShell), XtWindow ( algShell));
    return;
  }
  
  XtVaGetValues (widget, XmNuserData, &si, NULL);
  historySize=tmp;
  Strip_refresh((Strip) si);
}
#endif /* STRIP_HISTORY */

/* **************************** Emacs Editing Sequences ***************** */
/* Local Variables: */
/* tab-width: 6 */
/* c-basic-offset: 2 */
/* c-comment-only-line-offset: 0 */
/* c-indent-comments-syntactically-p: t */
/* c-label-minimum-indentation: 1 */
/* c-file-offsets: ((substatement-open . 0) (label . 2) */
/* (brace-entry-open . 0) (label .2) (arglist-intro . +) */
/* (arglist-cont-nonempty . c-lineup-arglist) ) */
/* End: */
