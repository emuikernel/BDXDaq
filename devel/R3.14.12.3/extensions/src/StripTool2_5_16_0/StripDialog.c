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

#include "Strip.h"
#include "StripDialog.h"
#include "ColorDialog.h"
#include "Tabs.h"
#include "StripConfig.h"
#include "StripMisc.h"
#include "StripVersion.h"
#include <epicsVersion.h>

#include <stdlib.h>
#include <errno.h>
#if !defined(_MSC_VER)
#include <inttypes.h>
#else
#define PRIiPTR "i"
#endif


#ifdef USE_XMU
#  include <X11/Xmu/Editres.h>
#endif

#include <Xm/ScrolledW.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushBG.h>
#include <Xm/FileSB.h>
#include <Xm/Scale.h>
#include <Xm/Protocols.h>

#define DEF_WOFFSET             3
#define MAX_WINDOW_MENU_ITEMS   10
#define MAX_REALNUM_LEN         20

extern int auto_scaleTriger; /* Albert */

typedef enum _sdPage
{
  SDPAGE_CURVES = 0,
  SDPAGE_CONTROLS,
  NUM_SDPAGES
} sdPage;

typedef enum
{
  SDCALLBACK_CONNECT = 0,
  SDCALLBACK_SHOW,
  SDCALLBACK_CLEAR,
  SDCALLBACK_DELETE,
  SDCALLBACK_DISMISS,
  SDCALLBACK_QUIT,
  SDCALLBACK_COUNT
}
StripDialogCallback;

typedef enum
{
  SDCURVE_NAME,
  SDCURVE_COLOR,
  SDCURVE_PLOTSTAT,
  SDCURVE_SCALE,
  SDCURVE_PRECISION,
  SDCURVE_MIN,
  SDCURVE_MAX,
  SDCURVE_MODIFY,
  SDCURVE_REMOVE,
  SDCURVE_LAST_ATTRIBUTE
}
SDCurveAttribute;

typedef enum
{
  SDTMOPT_TSHOUR,
  SDTMOPT_TSMINUTE,
  SDTMOPT_TSSECOND,
  SDTMOPT_NUMSAMPLES,
  SDTMOPT_DS,
  SDTMOPT_GR,
  SDTMOPT_MODIFY,
  SDTMOPT_CANCEL,
  SDTMOPT_LAST_ATTRIBUTE
}
SDTmoptAttribute;

typedef enum
{
  SDGROPT_FG,
  SDGROPT_BG,
  SDGROPT_GRIDCLR,
  SDGROPT_GRIDX,
  SDGROPT_GRIDY,
  SDGROPT_YAXISCLR,
  SDGROPT_LINEWIDTH,
  SDGROPT_LAST_ATTRIBUTE
}
SDGroptAttribute;

typedef int     SDCurveMask;
typedef int     SDTimeMask;
typedef int     SDGraphMask;

typedef enum
{
  HELP_WEBHELP,
  HELP_HELPHELP,
  HELP_ABOUT
} HelpItems;

typedef enum
{
  FSDLG_TGL_TIME = 0,
  FSDLG_TGL_COLORS,
  FSDLG_TGL_OPTIONS,
  FSDLG_TGL_CURVES,
  FSDLG_TGL_COUNT
}
FsDlgTglAttributes;

char    *FsDlgTglStr[FSDLG_TGL_COUNT] =
{
  "Timing",
  "Colors",
  "Graph Attributes",
  "Curve Attributes"
};

StripConfigMask *FsDlgTglVal[FSDLG_TGL_COUNT] =
{
  &SCFGMASK_TIME,
  &SCFGMASK_COLOR,
  &SCFGMASK_OPTION,
  &SCFGMASK_CURVE
};

typedef enum
{
  FSDLG_SAVE,
  FSDLG_SAVE_AS,
  FSDLG_LOAD
}
FsDlgState;

#define FSDLG_OK        0
#define FSDLG_CANCEL    1


char    *SDCurveAttributeWidgetName[SDCURVE_LAST_ATTRIBUTE] =
{
  "nameRowLabel",
  "colorRowLabel",
  "plotRowLabel",
  "scaleRowLabel",
  "precisionRowLabel",
  "minRowLabel",
  "maxRowLabel",
  "modifyRowLabel",
  "removeRowLabel"
};

typedef struct _CBInfo
{
  char  *str;
  void  *p;
}
CBInfo;

typedef struct
{
  StripCurve    curve;
  Widget        widgets[SDCURVE_LAST_ATTRIBUTE];
  Widget        precision_txt, precision_lbl;
  Widget        min_txt, min_lbl;
  Widget        max_txt, max_lbl;
  Widget        top_sep;
  SDCurveMask   modified;
  int           modifying;
}
SDCurveInfo;

typedef struct
{
  Widget        widgets[SDTMOPT_LAST_ATTRIBUTE];
  Widget        ts_hour_txt, ts_hour_lbl;
  Widget        ts_minute_txt, ts_minute_lbl;
  Widget        ts_second_txt, ts_second_lbl;
  Widget        num_samples_txt, num_samples_lbl;
  Widget        ds_txt, ds_lbl;
  Widget        gr_txt, gr_lbl;
  SDTimeMask    modified;
  int           modifying;
}
SDTimeInfo;

typedef struct
{
  Widget        widgets[SDGROPT_LAST_ATTRIBUTE];
  Widget        xgrid[STRIPGRID_NUM_TYPES], ygrid[STRIPGRID_NUM_TYPES];
  CBInfo        cb_info[SDGROPT_LAST_ATTRIBUTE];
}
SDGraphInfo;

typedef struct _SDWindowMenuInfo
{
  Widget        menu;
  
  struct        _item
  {
    SDWindowMenuItem    info;
    Widget              entry;
  }
  items[MAX_WINDOW_MENU_ITEMS];

  int           count;
}
SDWindowMenuInfo;

typedef struct
{
  Display               *display;
  StripConfig           *config;
  ColorDialog           clrdlg;
  SDCurveInfo           curve_info[STRIP_MAX_CURVES];
  int                   sdcurve_count;
  SDTimeInfo            time_info;
  SDGraphInfo           graph_info;
  Widget                pages[NUM_SDPAGES], current_page;
  Widget                shell, curve_form, data_form, graph_form, connect_txt;
  Widget                msglog_list;
  Widget                message_box;
  SDWindowMenuInfo      window_menu_info;

  struct                _fs     /* file selection box */
  {
    Widget              dlg;
    Widget              tgl[FSDLG_TGL_COUNT];
    StripConfigMask     mask;
    FsDlgState          state;
  }
  fs;

  struct                _callback
  {
    SDcallback  func;
    void        *data;
  }
  callback[SDCALLBACK_COUNT];

  int                   color_edit_which; /* attr of color in color editor */
}
StripDialogInfo;


/* ====== static data ====== */
static XmString         modify_btn_str[2];
static XmString         gridstat_tgl_str[2];
static XmString         yaxisclr_tgl_str[2];

static char             char_buf[256];
static XmString         xstr;


/* ====== static function prototypes ====== */
static void     insert_sdcurve  (StripDialogInfo *, int, StripCurve, int);
static void     delete_sdcurve  (StripDialogInfo *, int);
static void     show_sdcurve    (StripDialogInfo *, int);
static void     hide_sdcurve    (StripDialogInfo *, int);

/* these functions operate on curve rows in the curve control area */
static void     setwidgetval_name       (StripDialogInfo *, int, char *);
static void     setwidgetval_status     (StripDialogInfo *, int,
                                         unsigned);
static void     setwidgetval_color      (StripDialogInfo *, int, Pixel);
static void     setwidgetval_plotstat   (StripDialogInfo *, int, int);
static void     setwidgetval_precision  (StripDialogInfo *, int, int);
static void     setwidgetval_min        (StripDialogInfo *, int, double);
static void     setwidgetval_max        (StripDialogInfo *, int, double);
static void     setwidgetval_scale      (StripDialogInfo *, int, int);
static void     setwidgetval_modify     (StripDialogInfo *, int, int);

#if 0
/* KE: unused */
static char     *getwidgetval_name      (StripDialogInfo *, int);
static char     *getwidgetval_status    (StripDialogInfo *, int);
static Pixel    getwidgetval_color      (StripDialogInfo *, int);
static int      getwidgetval_plotstat   (StripDialogInfo *, int);
static int      getwidgetval_scale      (StripDialogInfo *, int);
#endif

/*
 * these functions operate on StripConfig attribute control widgets
 */
/* time controls */
static void     setwidgetval_tm_tshour          (StripDialogInfo *, int);
static void     setwidgetval_tm_tsminute        (StripDialogInfo *, int);
static void     setwidgetval_tm_tssecond        (StripDialogInfo *, int);
static void     setwidgetval_tm_numsamples      (StripDialogInfo *, int);
static void     setwidgetval_tm_ds              (StripDialogInfo *, double);
static void     setwidgetval_tm_gr              (StripDialogInfo *, double);
static void     setwidgetval_tm_modify          (StripDialogInfo *, int);

static void     getwidgetval_tm_tshour          (StripDialogInfo *, int *);
static void     getwidgetval_tm_tsminute        (StripDialogInfo *, int *);
static void     getwidgetval_tm_tssecond        (StripDialogInfo *, int *);
static void     getwidgetval_tm_numsamples      (StripDialogInfo *, int *);
static void     getwidgetval_tm_ds              (StripDialogInfo *, double *);
static void     getwidgetval_tm_gr              (StripDialogInfo *, double *);

/* graph controls */
static void     setwidgetval_gr_fg              (StripDialogInfo *, Pixel);
static void     setwidgetval_gr_bg              (StripDialogInfo *, Pixel);
static void     setwidgetval_gr_gridclr         (StripDialogInfo *, Pixel);
static void     setwidgetval_gr_linewidth       (StripDialogInfo *, int);
static void     setwidgetval_gr_gridx           (StripDialogInfo *, int);
static void     setwidgetval_gr_gridy           (StripDialogInfo *, int);
static void     setwidgetval_gr_yaxisclr        (StripDialogInfo *, int);

#if 0
/* KE: unused */
static Pixel    getwidgetval_gr_fg              (StripDialogInfo *);
static Pixel    getwidgetval_gr_bg              (StripDialogInfo *);
static Pixel    getwidgetval_gr_gridclr         (StripDialogInfo *);
static int      getwidgetval_gr_linewidth       (StripDialogInfo *);
static int      getwidgetval_gr_gridx           (StripDialogInfo *);
static int      getwidgetval_gr_gridy           (StripDialogInfo *);
static int      getwidgetval_gr_yaxisclr        (StripDialogInfo *);
#endif

/* ====== static callback function prototypes ====== */
static void     connect_btn_cb  (Widget, XtPointer, XtPointer);

static void     color_btn_cb    (Widget, XtPointer, XtPointer);
static void     modify_btn_cb   (Widget, XtPointer, XtPointer);
static void     remove_btn_cb   (Widget, XtPointer, XtPointer);
static void     plotstat_tgl_cb (Widget, XtPointer, XtPointer);
static void     scale_tgl_cb    (Widget, XtPointer, XtPointer);

static void     text_focus_cb   (Widget, XtPointer, XtPointer);

static void     tmmodify_btn_cb (Widget, XtPointer, XtPointer);
static void     tmcancel_btn_cb (Widget, XtPointer, XtPointer);
static void     grcolor_btn_cb  (Widget, XtPointer, XtPointer);
static void     gropt_slider_cb (Widget, XtPointer, XtPointer);
static void     gropt_tgl_cb    (Widget, XtPointer, XtPointer);
static void     gropt_xgrid_cb  (Widget, XtPointer, XtPointer);
static void     gropt_ygrid_cb  (Widget, XtPointer, XtPointer);

static void     bogus_cb        (Widget, XtPointer, XtPointer);

static void     wmdelete_cb     (Widget, XtPointer, XtPointer);

static void     filemenu_cb     (Widget, XtPointer, XtPointer);
static void     windowmenu_cb   (Widget, XtPointer, XtPointer);
static void     helpmenu_cb     (Widget, XtPointer, XtPointer);

static void     ctrl_btn_cb     (Widget, XtPointer, XtPointer);
static void     tabs_cb         (Widget, XtPointer, XtPointer);

static void     fsdlg_cb        (Widget, XtPointer, XtPointer);
static void     clrdlg_cb       (void *, cColor *);

static void     StripDialog_cfgupdate   (StripConfigMask, void *);

static void     save_config     (StripDialogInfo *, char *, StripConfigMask);
static void     load_config     (StripDialogInfo *, char *, StripConfigMask);


StripDialog     StripDialog_init        (Widget parent, StripConfig *cfg)
{
  StripDialogInfo       *sd;
  Widget                frame;
  Widget                base_form, form, rowcol;
  Widget                w, lbl, txt, btn, tmp, pulldown;
  Widget                curve_column_lbl[SDCURVE_LAST_ATTRIBUTE];
  Widget                leftmost_col, rightmost_col;
  Widget                sep;
  Widget                menu, tabs;
  intptr_t              i, j;
  Dimension             dim1, dim2;
  Dimension             widths[SDCURVE_LAST_ATTRIBUTE];
  Dimension             row_height;
  StripConfigMask       scfg_mask;
  Atom                  WM_DELETE_WINDOW;
  Arg                   args[5];

  if ((sd = (StripDialogInfo *)malloc (sizeof (StripDialogInfo))) != NULL)
  {
    modify_btn_str[0]   = XmStringCreateLocalized ("Modify");
    modify_btn_str[1]   = XmStringCreateLocalized ("Update");
    gridstat_tgl_str[0] = XmStringCreateLocalized ("on");
    gridstat_tgl_str[1] = XmStringCreateLocalized ("off");
    yaxisclr_tgl_str[0] = XmStringCreateLocalized ("selected curve");
    yaxisclr_tgl_str[1] = XmStringCreateLocalized ("foreground");

    for (i = 0; i < STRIP_MAX_CURVES; i++)
      sd->curve_info[i].curve = 0;
    sd->sdcurve_count = 0;
    sd->time_info.modifying = 0;
    sd->message_box = (Widget)0;

    sd->display = XtDisplay (parent);

    for (i = 0; i < SDCALLBACK_COUNT; i++)
    {
      sd->callback[i].func = NULL;
      sd->callback[i].data = NULL;
    }
      
    /* store the configuration pointer*/ 
    sd->config = cfg;

    /* add all update callbacks */
    StripConfigMask_clear (&scfg_mask);
    StripConfigMask_or (&scfg_mask, &SCFGMASK_TIME);
    StripConfigMask_or (&scfg_mask, &SCFGMASK_OPTION);
    StripConfigMask_or (&scfg_mask, &SCFGMASK_CURVE);
    StripConfigMask_or (&scfg_mask, &SCFGMASK_COLOR);
    StripConfig_addcallback
      (sd->config, scfg_mask, StripDialog_cfgupdate, sd);

    /* initialize the color editor */
    sd->clrdlg = ColorDialog_init (parent, "Strip ColorEditor", cfg);

    /* top level shell */  
    sd->shell = XtVaCreatePopupShell
      ("StripDialog",   
       topLevelShellWidgetClass,        parent,
       XmNdeleteResponse,               XmDO_NOTHING,
       XmNmappedWhenManaged,            False,
       XmNvisual,                       cfg->xvi.visual,
       XmNcolormap,                     cColorManager_getcmap (cfg->scm),
       NULL);

#ifdef USE_XMU
    /* editres support */
    XtAddEventHandler
      (sd->shell, (EventMask)0, True, (XtEventHandler)_XEditResCheckMessages, 0);
#endif
    
    /* hook window manager delete message in order to perform delete instead */
    WM_DELETE_WINDOW = XmInternAtom (sd->display, "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback
      (sd->shell, WM_DELETE_WINDOW, wmdelete_cb, (XtPointer)sd);
    
    base_form = XtVaCreateManagedWidget
      ("baseForm",
       xmFormWidgetClass,               sd->shell,
       NULL);

    /* build the file selection box */
    sd->fs.dlg = XmCreateFileSelectionDialog
      (sd->shell, "Configuration File", NULL, 0);
    XtVaSetValues
      (sd->fs.dlg,
       XmNuserData,                     sd,
       XmNfileTypeMask,         XmFILE_REGULAR,
       NULL);
    xstr = XmStringCreateLocalized (STRIP_CONFIGFILE_DIR);
    XtVaSetValues (sd->fs.dlg, XmNdirectory, xstr, NULL);
    XmStringFree (xstr);
    xstr = XmStringCreateLocalized (STRIP_CONFIGFILE_PATTERN);
    XtVaSetValues (sd->fs.dlg, XmNpattern, xstr, NULL);
    XmStringFree (xstr);
    
    /* pattern */
    xstr = XmStringCreateLocalized (STRIP_CONFIGFILE_PATTERN);
    XtVaSetValues (sd->fs.dlg, XmNpattern, xstr, NULL);
    XmStringFree (xstr);

    XtAddCallback
      (sd->fs.dlg, XmNcancelCallback, fsdlg_cb, (XtPointer)FSDLG_CANCEL);
    XtAddCallback (sd->fs.dlg, XmNokCallback, fsdlg_cb, (XtPointer)FSDLG_OK);
    frame = XtVaCreateManagedWidget
      ("frame",
       xmFrameWidgetClass,              sd->fs.dlg,
       NULL);
    XtVaCreateManagedWidget
      ("Attribute Mask",
       xmLabelWidgetClass,              frame,
       XmNchildType,                    XmFRAME_TITLE_CHILD,
       NULL);
    w = XtVaCreateManagedWidget
      ("rowcol",
       xmRowColumnWidgetClass,  frame,
       XmNchildType,                    XmFRAME_WORKAREA_CHILD,
       NULL);
    StripConfigMask_clear (&sd->fs.mask);
    sd->fs.state = FSDLG_SAVE;
    for (i = 0; i < FSDLG_TGL_COUNT; i++)
    {
      xstr = XmStringCreateLocalized (FsDlgTglStr[i]);
      sd->fs.tgl[i] = XtVaCreateManagedWidget
        ("togglebutton",
         xmToggleButtonWidgetClass,     w,
         XmNlabelString,                xstr,
         XmNset,                        True,
         NULL);
      XmStringFree (xstr);
      StripConfigMask_or (&sd->fs.mask, FsDlgTglVal[i]);
    }
             

    /* build the menu bar and related components */
    menu = XmCreateMenuBar (base_form, "appMenubar", NULL, 0);
    XtVaSetValues
      (menu,
       XmNtopAttachment,                XmATTACH_FORM,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_FORM,
       NULL);
      
    w = XmCreatePulldownMenu (menu, "filePulldown", NULL, 0);
    XtVaCreateManagedWidget
      ("File",
       xmCascadeButtonGadgetClass,      menu,
       XmNmnemonic,                     'F',
       XmNsubMenuId,                    w,
       NULL);
    tmp = XtVaCreateManagedWidget
      ("loadPushB",
       xmPushButtonGadgetClass,         w,
       XmNuserData,                     sd,
       NULL);
    XtAddCallback
      (tmp, XmNactivateCallback, filemenu_cb, (XtPointer)FSDLG_LOAD);
    XtVaCreateManagedWidget
      ("separator",
       xmSeparatorGadgetClass,  w,
       NULL);
    tmp = XtVaCreateManagedWidget
      ("saveAsPushB",
       xmPushButtonGadgetClass,         w,
       XmNuserData,                     sd,
       NULL);
    XtAddCallback
      (tmp, XmNactivateCallback, filemenu_cb, (XtPointer)FSDLG_SAVE_AS);
    tmp = XtVaCreateManagedWidget
      ("savePushB",
       xmPushButtonGadgetClass,         w,
       XmNuserData,                     sd,
       NULL);
    XtAddCallback
      (tmp, XmNactivateCallback, filemenu_cb, (XtPointer)FSDLG_SAVE);
    XtVaCreateManagedWidget
      ("separator",
       xmSeparatorGadgetClass,  w,
       NULL);
    tmp = XtVaCreateManagedWidget
      ("clearPushB",
       xmPushButtonGadgetClass,         w,
       XmNuserData,                     sd,
       NULL);
    XtAddCallback
      (tmp, XmNactivateCallback, ctrl_btn_cb, (XtPointer)SDCALLBACK_CLEAR);
    XtVaCreateManagedWidget
      ("separator",
       xmSeparatorGadgetClass,  w,
       NULL);
    tmp = XtVaCreateManagedWidget
      ("exitPushB",
       xmPushButtonGadgetClass,     w,
       XmNuserData,                 sd,
       NULL);
    XtAddCallback
      (tmp, XmNactivateCallback, ctrl_btn_cb, (XtPointer)SDCALLBACK_QUIT);
    tmp = XtVaCreateManagedWidget
      ("dismissPushB",
       xmPushButtonGadgetClass,     w,
       XmNuserData,                 sd,
       NULL);
    XtAddCallback
      (tmp, XmNactivateCallback, ctrl_btn_cb, (XtPointer)SDCALLBACK_DISMISS);

    sd->window_menu_info.menu = XmCreatePulldownMenu
      (menu, "windowPulldown", NULL, 0);
    XtVaCreateManagedWidget
      ("Window",
       xmCascadeButtonGadgetClass,      menu,
       XmNmnemonic,                     'W',
       XmNsubMenuId,                    sd->window_menu_info.menu,
       NULL);
    for (i = 0; i < MAX_WINDOW_MENU_ITEMS; i++)
    {
      sprintf (char_buf, "window%"PRIiPTR"PushB", i);
      sd->window_menu_info.items[i].entry = w = XtVaCreateManagedWidget
        (char_buf,
         xmPushButtonGadgetClass,   sd->window_menu_info.menu,
         XmNuserData,               sd,
         NULL);
      XtAddCallback (w, XmNactivateCallback, windowmenu_cb, (XtPointer)i);
      XtUnmanageChild (sd->window_menu_info.items[i].entry);
    }
    sd->window_menu_info.count = 0;

    w = XmCreatePulldownMenu (menu, "helpPulldown", NULL, 0);
    tmp = XtVaCreateManagedWidget
      ("Help",
       xmCascadeButtonGadgetClass,      menu,
       XmNmnemonic,                     'H',
       XmNsubMenuId,                    w,
       NULL);
    XtVaSetValues
      (menu,
       XmNmenuHelpWidget,               tmp,
       NULL);

    /* web help */
    tmp = XtVaCreateManagedWidget
      ("webHelpPushB",
       xmPushButtonGadgetClass,         w,
       XmNuserData,                     sd,
       NULL);
    XtAddCallback
      (tmp, XmNactivateCallback, helpmenu_cb, (XtPointer)HELP_WEBHELP);
    
    /* help on help */
    tmp = XtVaCreateManagedWidget
      ("helpOnHelpPushB",
       xmPushButtonGadgetClass,         w,
       XmNuserData,                     sd,
       NULL);
    XtAddCallback
      (tmp, XmNactivateCallback, helpmenu_cb, (XtPointer)HELP_HELPHELP);
    
    /* about */
    tmp = XtVaCreateManagedWidget
      ("aboutPushB",
       xmPushButtonGadgetClass,         w,
       XmNuserData,                     sd,
       NULL);
    XtAddCallback
      (tmp, XmNactivateCallback, helpmenu_cb, (XtPointer)HELP_ABOUT);
      
    XtManageChild (menu);

    /* ====== curve connection area ====== */
    rowcol = XtVaCreateManagedWidget
      ("connectRowColumn",
       xmRowColumnWidgetClass,          base_form,
       XmNorientation,                  XmHORIZONTAL,
       XmNnumColumns,                   3,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    menu,
       XmNtopOffset,                    DEF_WOFFSET,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNleftOffset,                   DEF_WOFFSET,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNrightOffset,                  DEF_WOFFSET,
       XmNbottomAttachment,             XmATTACH_NONE,
       NULL);
    
    XtVaCreateManagedWidget("connectLabel",
	xmLabelWidgetClass,               rowcol,
	NULL);
    sd->connect_txt = XtVaCreateManagedWidget ("connectText",
	xmTextWidgetClass,                rowcol,
	NULL);
    XtAddCallback (sd->connect_txt, XmNactivateCallback, connect_btn_cb, sd);
    XtAddCallback (sd->connect_txt, XmNfocusCallback, text_focus_cb, 0);
    btn = XtVaCreateManagedWidget  ("connectButton",
	xmPushButtonWidgetClass,          rowcol,
	NULL);
    XtAddCallback (btn, XmNactivateCallback, connect_btn_cb, sd);

    /* make some nice tabs */
    tabs = XtVaCreateManagedWidget
      ("tabs",
       xgTabsWidgetClass,               base_form,
       XgNtabCount,                     NUM_SDPAGES,
       XgNcurrentTab,                   SDPAGE_CURVES,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    rowcol,
       XmNtopOffset,                    DEF_WOFFSET,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_NONE,
       NULL);
    XtAddCallback (tabs, XgNactivateCallback, tabs_cb, sd);

    /* create the curve area form and controls */
    sd->pages[SDPAGE_CURVES] = sd->curve_form = form = XtVaCreateManagedWidget
      ("curvePageForm",
       xmFormWidgetClass,               base_form,
       XmNfractionBase,                 (STRIP_MAX_CURVES + 1) * 10,
       XmNnoResize,                     False,
       XmNmappedWhenManaged,            False,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    tabs,
       XmNtopOffset,                    DEF_WOFFSET,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNleftOffset,                   DEF_WOFFSET,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNrightOffset,                  DEF_WOFFSET,
       XmNbottomAttachment,             XmATTACH_FORM,
       XmNbottomOffset,                 DEF_WOFFSET,
       NULL);
    
    curve_column_lbl[i = SDCURVE_NAME] = XtVaCreateManagedWidget
      (SDCurveAttributeWidgetName[SDCURVE_NAME],
       xmLabelWidgetClass,              form,
       XmNrecomputeSize,                False,
       XmNtopAttachment,                XmATTACH_FORM,
       XmNtopOffset,                    DEF_WOFFSET,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNleftOffset,                   DEF_WOFFSET,
       NULL);
    
    for (i = i+1, row_height = 0;  i < SDCURVE_LAST_ATTRIBUTE; i++)
    {
      curve_column_lbl[i] = XtVaCreateManagedWidget
        (SDCurveAttributeWidgetName[i],
         xmLabelWidgetClass,            form,
         XmNtopAttachment,              XmATTACH_POSITION,
         XmNtopPosition,                1,
         XmNleftAttachment,             XmATTACH_WIDGET,
         XmNleftWidget,                 curve_column_lbl[i-1],
         XmNleftOffset,                 DEF_WOFFSET,
         NULL);
      XtVaGetValues (curve_column_lbl[i], XmNheight, &dim1, NULL);
      row_height = max (row_height, dim1);
    }
    for (i = SDCURVE_PRECISION; i <= SDCURVE_MAX; i++)
      XtVaSetValues
        (curve_column_lbl[i],
         XmNalignment,                  XmALIGNMENT_END,
         XmNrecomputeSize,              False,
         NULL);

    leftmost_col = curve_column_lbl[0];
    rightmost_col = curve_column_lbl[SDCURVE_LAST_ATTRIBUTE-1];

    for (j = 0; j < STRIP_MAX_CURVES; j++)
    {
      sep = sd->curve_info[j].top_sep = XtVaCreateManagedWidget
        ("curveSeparator",
         xmSeparatorWidgetClass,        form,
         XmNmappedWhenManaged,          False,
         XmNtopAttachment,              XmATTACH_POSITION,
         XmNtopPosition,                ((j+1)*10) + 1,
         XmNleftAttachment,             XmATTACH_OPPOSITE_WIDGET,
         XmNleftWidget,                 leftmost_col,
         XmNleftOffset,                 DEF_WOFFSET,
         XmNrightAttachment,            XmATTACH_OPPOSITE_WIDGET,
         XmNrightWidget,                rightmost_col,
         NULL);

      /* ====== name ====== */
      sd->curve_info[j].widgets[SDCURVE_NAME] = XtVaCreateManagedWidget
        ("nameLabel",
         xmLabelWidgetClass,            form,
         XmNmappedWhenManaged,          False,
         XmNrecomputeSize,              False,
         XmNtopAttachment,              XmATTACH_WIDGET,
         XmNtopWidget,                  sep,
         XmNleftAttachment,             XmATTACH_OPPOSITE_WIDGET,
         XmNleftWidget,                 curve_column_lbl[SDCURVE_NAME],
         XmNleftOffset,                 DEF_WOFFSET,
         NULL);

      /* ====== color ====== */
      sd->curve_info[j].widgets[SDCURVE_COLOR] = XtVaCreateManagedWidget
        ("colorPushButton",
         xmPushButtonWidgetClass,       form,
         XmNmappedWhenManaged,          False,
         XmNtopAttachment,              XmATTACH_WIDGET,
         XmNtopWidget,                  sep,
         XmNleftAttachment,             XmATTACH_OPPOSITE_WIDGET,
         XmNleftWidget,                 curve_column_lbl[SDCURVE_COLOR],
         XmNleftOffset,                 DEF_WOFFSET,
         XmNuserData,                   sd,
         NULL);
      XtAddCallback
        (sd->curve_info[j].widgets[SDCURVE_COLOR],
         XmNactivateCallback, color_btn_cb, (XtPointer)j);

      /* ====== plot status ====== */
      sd->curve_info[j].widgets[SDCURVE_PLOTSTAT] = XtVaCreateManagedWidget
        ("plotToggle",
         xmToggleButtonWidgetClass,     form,
         XmNset,                        True,
         XmNmappedWhenManaged,          False,
         XmNtopAttachment,              XmATTACH_WIDGET,
         XmNtopWidget,                  sep,
         XmNleftAttachment,             XmATTACH_OPPOSITE_WIDGET,
         XmNleftWidget,                 curve_column_lbl[SDCURVE_PLOTSTAT],
         XmNleftOffset,                 DEF_WOFFSET,
         XmNuserData,                   sd,
         NULL);
      XtAddCallback
        (sd->curve_info[j].widgets[SDCURVE_PLOTSTAT],
         XmNvalueChangedCallback, plotstat_tgl_cb, (XtPointer)j);

      /* ====== scale ====== */
      sd->curve_info[j].widgets[SDCURVE_SCALE] = XtVaCreateManagedWidget
        ("scaleToggle",
         xmToggleButtonWidgetClass,     form,
         XmNset,                        False,
         XmNmappedWhenManaged,          False,
         XmNtopAttachment,              XmATTACH_WIDGET,
         XmNtopWidget,                  sep,
         XmNleftAttachment,             XmATTACH_OPPOSITE_WIDGET,
         XmNleftWidget,                 curve_column_lbl[SDCURVE_SCALE],
         XmNleftOffset,                 DEF_WOFFSET,
         XmNuserData,                   sd,
         NULL);
      XtAddCallback
        (sd->curve_info[j].widgets[SDCURVE_SCALE],
         XmNvalueChangedCallback, scale_tgl_cb, (XtPointer)j);
      
      /* ====== precision ====== */
      sd->curve_info[j].precision_lbl = XtVaCreateManagedWidget
        ("precisionLabel",
         xmLabelWidgetClass,            form,
         XmNrecomputeSize,              False,
         XmNmappedWhenManaged,          False,
         XmNtopAttachment,              XmATTACH_WIDGET,
         XmNtopWidget,                  sep,
         XmNleftAttachment,             XmATTACH_OPPOSITE_WIDGET,
         XmNleftWidget,                 curve_column_lbl[SDCURVE_PRECISION],
         XmNleftOffset,                 DEF_WOFFSET,
         NULL);
      sd->curve_info[j].widgets[SDCURVE_PRECISION] =
        sd->curve_info[j].precision_txt = XtVaCreateManagedWidget
        ("precisionText",
         xmTextWidgetClass,             form,
         XmNmappedWhenManaged,          False,
         XmNtopAttachment,              XmATTACH_WIDGET,
         XmNtopWidget,                  sep,
         XmNleftAttachment,             XmATTACH_OPPOSITE_WIDGET,
         XmNleftWidget,                 curve_column_lbl[SDCURVE_PRECISION],
         XmNleftOffset,                 DEF_WOFFSET,
         NULL);
      XtAddCallback
        (sd->curve_info[j].precision_txt, XmNfocusCallback,
         text_focus_cb, (XtPointer)0);
      
      /* ====== min ====== */
      sd->curve_info[j].min_lbl = XtVaCreateManagedWidget
        ("minLabel",
         xmLabelWidgetClass,            form,
         XmNrecomputeSize,              False,
         XmNmappedWhenManaged,          False,
         XmNtopAttachment,              XmATTACH_WIDGET,
         XmNtopWidget,                  sep,
         XmNleftAttachment,             XmATTACH_OPPOSITE_WIDGET,
         XmNleftWidget,                 curve_column_lbl[SDCURVE_MIN],
         XmNleftOffset,                 DEF_WOFFSET,
         NULL);
      sd->curve_info[j].widgets[SDCURVE_MIN] =
        sd->curve_info[j].min_txt = XtVaCreateManagedWidget
        ("minText",
         xmTextWidgetClass,             form,
         XmNmappedWhenManaged,          False,
         XmNtopAttachment,              XmATTACH_WIDGET,
         XmNtopWidget,                  sep,
         XmNleftAttachment,             XmATTACH_OPPOSITE_WIDGET,
         XmNleftWidget,                 curve_column_lbl[SDCURVE_MIN],
         XmNleftOffset,                 DEF_WOFFSET,
         NULL);
      XtAddCallback
        (sd->curve_info[j].min_txt, XmNfocusCallback,
         text_focus_cb, (XtPointer)0);

      /* ====== max ====== */
      sd->curve_info[j].max_lbl = XtVaCreateManagedWidget
        ("maxLabel",
         xmLabelWidgetClass,            form,
         XmNrecomputeSize,              False,
         XmNmappedWhenManaged,          False,
         XmNtopAttachment,              XmATTACH_WIDGET,
         XmNtopWidget,                  sep,
         XmNleftAttachment,             XmATTACH_OPPOSITE_WIDGET,
         XmNleftWidget,                 curve_column_lbl[SDCURVE_MAX],
         XmNleftOffset,                 DEF_WOFFSET,
         NULL);
      sd->curve_info[j].widgets[SDCURVE_MAX] = 
        sd->curve_info[j].max_txt = XtVaCreateManagedWidget
        ("maxText",
         xmTextWidgetClass,             form,
         XmNmappedWhenManaged,          False,
         XmNtopAttachment,              XmATTACH_WIDGET,
         XmNtopWidget,                  sep,
         XmNleftAttachment,             XmATTACH_OPPOSITE_WIDGET,
         XmNleftWidget,                 curve_column_lbl[SDCURVE_MAX],
         XmNleftOffset,                 DEF_WOFFSET,
         NULL);
      XtAddCallback
        (sd->curve_info[j].max_txt, XmNfocusCallback,
         text_focus_cb, (XtPointer)0);

      /* ====== modify ====== */
      sd->curve_info[j].widgets[SDCURVE_MODIFY] = XtVaCreateManagedWidget
        ("modifyPushButton",
         xmPushButtonWidgetClass,       form,
         XmNlabelString,                modify_btn_str[0],
         XmNmappedWhenManaged,          False,
         XmNtopAttachment,              XmATTACH_WIDGET,
         XmNtopWidget,                  sep,
         XmNleftAttachment,             XmATTACH_OPPOSITE_WIDGET,
         XmNleftWidget,                 curve_column_lbl[SDCURVE_MODIFY],
         XmNleftOffset,                 DEF_WOFFSET,
         XmNuserData,                   sd,
         NULL);
      XtAddCallback
        (sd->curve_info[j].widgets[SDCURVE_MODIFY],
         XmNactivateCallback, modify_btn_cb, (XtPointer)j);
      sd->curve_info[j].modifying = 0;
          
      /* ====== remove ====== */
      sd->curve_info[j].widgets[SDCURVE_REMOVE] = XtVaCreateManagedWidget
        ("removePushButton",
         xmPushButtonWidgetClass,       form,
         XmNmappedWhenManaged,          False,
         XmNtopAttachment,              XmATTACH_WIDGET,
         XmNtopWidget,                  sep,
         XmNleftAttachment,             XmATTACH_OPPOSITE_WIDGET,
         XmNleftWidget,                 curve_column_lbl[SDCURVE_REMOVE],
         XmNleftOffset,                 DEF_WOFFSET,
         XmNuserData,                   sd,
         NULL);
      XtAddCallback
        (sd->curve_info[j].widgets[SDCURVE_REMOVE],
         XmNactivateCallback, remove_btn_cb, (XtPointer)j);
    }
      
    /* set the column widths approriately */
    for (i = 0; i < SDCURVE_LAST_ATTRIBUTE; i++)
    {
      XtVaGetValues (sd->curve_info[0].widgets[i], XmNwidth, &dim1, NULL);
      XtVaGetValues (curve_column_lbl[i], XmNwidth, &dim2, NULL);
      widths[i] = max (dim1, dim2);
      XtVaSetValues (curve_column_lbl[i], XmNwidth, widths[i], NULL);
    }

    for (i = 0; i < STRIP_MAX_CURVES; i++)
      for (j = 0; j < SDCURVE_LAST_ATTRIBUTE; j++)
        XtVaSetValues
          (sd->curve_info[i].widgets[j], XmNwidth, widths[j], NULL);
      
    /* Point the Precision, Min and Max widgets to the label widgets, not
     * the text widgets.  Also set the width. */
    for (i = 0; i < STRIP_MAX_CURVES; i++)
    {
      sd->curve_info[i].widgets[SDCURVE_PRECISION] =
        sd->curve_info[i].precision_lbl;
      sd->curve_info[i].widgets[SDCURVE_MIN] =
        sd->curve_info[i].min_lbl;
      sd->curve_info[i].widgets[SDCURVE_MAX] =
        sd->curve_info[i].max_lbl;
      XtVaSetValues
        (sd->curve_info[i].widgets[SDCURVE_PRECISION],
         XmNwidth, widths[SDCURVE_PRECISION],
         NULL);
      XtVaSetValues
        (sd->curve_info[i].widgets[SDCURVE_MIN],
         XmNwidth, widths[SDCURVE_MIN],
         NULL);
      XtVaSetValues
        (sd->curve_info[i].widgets[SDCURVE_MAX],
         XmNwidth, widths[SDCURVE_MAX],
         NULL);
    }

      
    /* the data control form */
    sd->pages[SDPAGE_CONTROLS] = sd->data_form = XtVaCreateManagedWidget
      ("controlsPageForm",
       xmFormWidgetClass,               base_form,
       XmNmappedWhenManaged,            False,
       XmNnoResize,                     True,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    tabs,
       XmNtopOffset,                    DEF_WOFFSET,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNleftOffset,                   DEF_WOFFSET,
       XmNrightAttachment,              XmATTACH_NONE,
       XmNbottomAttachment,             XmATTACH_NONE,
       NULL);

    /* create the time control area */
    frame = XtVaCreateManagedWidget
      ("timeFrame",
       xmFrameWidgetClass,              sd->data_form,
       XmNtopAttachment,                XmATTACH_FORM,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_FORM,
       NULL);
    XtVaCreateManagedWidget
      ("title",
       xmLabelWidgetClass,              frame,
       XmNchildType,                    XmFRAME_TITLE_CHILD,
       XmNresizePolicy,                 XmRESIZE_NONE,
       NULL);
    form = XtVaCreateManagedWidget
      ("form",
       xmFormWidgetClass,               frame,
       XmNchildType,                    XmFRAME_WORKAREA_CHILD,
       XmNfractionBase,                 2,
       NULL);
      
    sd->time_info.ts_hour_txt = w = txt = XtVaCreateManagedWidget
      ("text",
       xmTextWidgetClass,               form,
       XmNcolumns,                      2,
       XmNmappedWhenManaged,            False,
       XmNtopAttachment,                XmATTACH_FORM,
       XmNleftAttachment,               XmATTACH_POSITION,
       XmNleftPosition,                 1,
       NULL);
    XtAddCallback (txt, XmNfocusCallback, text_focus_cb, (XtPointer)0);
    sd->time_info.ts_hour_lbl = XtVaCreateManagedWidget
      ("HH",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    sd->time_info.ts_hour_txt,
       XmNleftAttachment,               XmATTACH_OPPOSITE_WIDGET,
       XmNleftWidget,                   sd->time_info.ts_hour_txt,
       XmNrightAttachment,              XmATTACH_OPPOSITE_WIDGET,
       XmNrightWidget,                  sd->time_info.ts_hour_txt,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 sd->time_info.ts_hour_txt,
       NULL);
    sd->time_info.widgets[SDTMOPT_TSHOUR] = sd->time_info.ts_hour_lbl;
    lbl = XtVaCreateManagedWidget
      (":",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_WIDGET,
       XmNleftWidget,                   txt,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 w,
       NULL);
    sd->time_info.ts_minute_txt = txt = XtVaCreateManagedWidget
      ("text",
       xmTextWidgetClass,               form,
       XmNcolumns,                      2,
       XmNmappedWhenManaged,            False,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_WIDGET,
       XmNleftWidget,                   lbl,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 w,
       NULL);
    XtAddCallback (txt, XmNfocusCallback, text_focus_cb, (XtPointer)0);
    sd->time_info.ts_minute_lbl = XtVaCreateManagedWidget
      ("MM",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    sd->time_info.ts_minute_txt,
       XmNleftAttachment,               XmATTACH_OPPOSITE_WIDGET,
       XmNleftWidget,                   sd->time_info.ts_minute_txt,
       XmNrightAttachment,              XmATTACH_OPPOSITE_WIDGET,
       XmNrightWidget,                  sd->time_info.ts_minute_txt,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 sd->time_info.ts_minute_txt,
       NULL);
    sd->time_info.widgets[SDTMOPT_TSMINUTE] = sd->time_info.ts_minute_lbl;
    lbl = XtVaCreateManagedWidget
      (":",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_WIDGET,
       XmNleftWidget,                   txt,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 w,
       NULL);
    sd->time_info.ts_second_txt = txt = XtVaCreateManagedWidget
      ("text",
       xmTextWidgetClass,               form,
       XmNcolumns,                      2,
       XmNmappedWhenManaged,            False,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_WIDGET,
       XmNleftWidget,                   lbl,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 w,
       NULL);
    XtAddCallback (txt, XmNfocusCallback, text_focus_cb, (XtPointer)0);
    sd->time_info.ts_second_lbl = XtVaCreateManagedWidget
      ("SS",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    sd->time_info.ts_second_txt,
       XmNleftAttachment,               XmATTACH_OPPOSITE_WIDGET,
       XmNleftWidget,                   sd->time_info.ts_second_txt,
       XmNrightAttachment,              XmATTACH_OPPOSITE_WIDGET,
       XmNrightWidget,                  sd->time_info.ts_second_txt,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 sd->time_info.ts_second_txt,
       NULL);
    sd->time_info.widgets[SDTMOPT_TSSECOND] = sd->time_info.ts_second_lbl;
    lbl = XtVaCreateManagedWidget
      ("Time Span: ",
       xmLabelWidgetClass,              form,
       XmNalignment,                    XmALIGNMENT_BEGINNING,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_POSITION,
       XmNleftPosition,                 0,
       XmNrightAttachment,              XmATTACH_POSITION,
       XmNrightPosition,                1,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 w,
       NULL);
      
    sd->time_info.num_samples_txt = txt = XtVaCreateManagedWidget
      ("text",
       xmTextWidgetClass,               form,
       XmNcolumns,                      4,
       XmNmappedWhenManaged,            False,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_POSITION,
       XmNleftPosition,                 1,
       NULL);
    XtAddCallback (txt, XmNfocusCallback, text_focus_cb, (XtPointer)0);
    sd->time_info.num_samples_lbl = XtVaCreateManagedWidget
      ("????",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    sd->time_info.num_samples_txt,
       XmNleftAttachment,               XmATTACH_OPPOSITE_WIDGET,
       XmNleftWidget,                   sd->time_info.num_samples_txt,
       XmNrightAttachment,              XmATTACH_OPPOSITE_WIDGET,
       XmNrightWidget,                  sd->time_info.num_samples_txt,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 sd->time_info.num_samples_txt,
       NULL);
    sd->time_info.widgets[SDTMOPT_NUMSAMPLES] = sd->time_info.num_samples_lbl;
    w = txt;
    XtVaCreateManagedWidget
      ("samples",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_WIDGET,
       XmNleftWidget,                   txt,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 w,
       NULL);
    lbl = XtVaCreateManagedWidget
      ("Ring Buffer Size: ",
       xmLabelWidgetClass,              form,
       xmLabelWidgetClass,              form,
       XmNalignment,                    XmALIGNMENT_BEGINNING,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_POSITION,
       XmNleftPosition,                 0,
       XmNrightAttachment,              XmATTACH_POSITION,
       XmNrightPosition,                1,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 w,
       NULL);
      
    sd->time_info.ds_txt = txt = XtVaCreateManagedWidget
      ("text",
       xmTextWidgetClass,               form,
       XmNcolumns,                      4,
       XmNmappedWhenManaged,            False,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_POSITION,
       XmNleftPosition,                 1,
       NULL);
    XtAddCallback (txt, XmNfocusCallback, text_focus_cb, (XtPointer)0);
    sd->time_info.ds_lbl = XtVaCreateManagedWidget
      ("????",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    sd->time_info.ds_txt,
       XmNleftAttachment,               XmATTACH_OPPOSITE_WIDGET,
       XmNleftWidget,                   sd->time_info.ds_txt,
       XmNrightAttachment,              XmATTACH_OPPOSITE_WIDGET,
       XmNrightWidget,                  sd->time_info.ds_txt,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 sd->time_info.ds_txt,
       NULL);
    sd->time_info.widgets[SDTMOPT_DS] = sd->time_info.ds_lbl;
    w = txt;
    XtVaCreateManagedWidget
      ("seconds",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_WIDGET,
       XmNleftWidget,                   txt,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 w,
       NULL);
    lbl = XtVaCreateManagedWidget
      ("Data Sample Interval: ",
       xmLabelWidgetClass,              form,
       xmLabelWidgetClass,              form,
       XmNalignment,                    XmALIGNMENT_BEGINNING,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_POSITION,
       XmNleftPosition,                 0,
       XmNrightAttachment,              XmATTACH_POSITION,
       XmNrightPosition,                1,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 w,
       NULL);
      
    sd->time_info.gr_txt = txt = XtVaCreateManagedWidget
      ("text",
       xmTextWidgetClass,               form,
       XmNcolumns,                      4,
       XmNmappedWhenManaged,            False,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_POSITION,
       XmNleftPosition,                 1,
       NULL);
    XtAddCallback (txt, XmNfocusCallback, text_focus_cb, (XtPointer)0);
    sd->time_info.gr_lbl = XtVaCreateManagedWidget
      ("????",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    sd->time_info.gr_txt,
       XmNleftAttachment,               XmATTACH_OPPOSITE_WIDGET,
       XmNleftWidget,                   sd->time_info.gr_txt,
       XmNrightAttachment,              XmATTACH_OPPOSITE_WIDGET,
       XmNrightWidget,                  sd->time_info.gr_txt,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 sd->time_info.gr_txt,
       NULL);
    sd->time_info.widgets[SDTMOPT_GR] = sd->time_info.gr_lbl;
    w = txt;
    XtVaCreateManagedWidget
      ("seconds",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_WIDGET,
       XmNleftWidget,                   txt,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 w,
       NULL);
    lbl = XtVaCreateManagedWidget
      ("Graph Redraw Interval: ",
       xmLabelWidgetClass,              form,
       XmNalignment,                    XmALIGNMENT_BEGINNING,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_POSITION,
       XmNleftPosition,                 0,
       XmNrightAttachment,              XmATTACH_POSITION,
       XmNrightPosition,                1,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 w,
       NULL);
      
    sep = XtVaCreateManagedWidget
      ("separator",
       xmSeparatorWidgetClass,          form,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_FORM,
       NULL);

    xstr = modify_btn_str[sd->time_info.modifying? 1 : 0];
    sd->time_info.widgets[SDTMOPT_MODIFY] = w = XtVaCreateManagedWidget
      ("push button",
       xmPushButtonWidgetClass,         form,
       XmNlabelString,                  xstr,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    sep,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_FORM,
       NULL);
    XtAddCallback (w, XmNactivateCallback, tmmodify_btn_cb, sd);
    sd->time_info.widgets[SDTMOPT_CANCEL] = XtVaCreateManagedWidget
      ("Cancel",
       xmPushButtonWidgetClass, form,
       XmNmappedWhenManaged,            sd->time_info.modifying,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    sep,
       XmNrightAttachment,              XmATTACH_WIDGET,
       XmNrightWidget,                  w,
       XmNbottomAttachment,             XmATTACH_FORM,
       NULL);
    XtAddCallback
      (sd->time_info.widgets[SDTMOPT_CANCEL],
       XmNactivateCallback, tmcancel_btn_cb, sd);
    w = frame;
         
    /* create the graph form and control widgets */
    frame = XtVaCreateManagedWidget
      ("optionFrame",
       xmFrameWidgetClass,              sd->data_form,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    w,
       XmNtopAttachment,                DEF_WOFFSET,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNrightAttachment,              XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_FORM,
       NULL);
    XtVaCreateManagedWidget
      ("title",
       xmLabelWidgetClass,              frame,
       XmNchildType,                    XmFRAME_TITLE_CHILD,
       XmNresizePolicy,                 XmRESIZE_NONE,
       NULL);
    form = XtVaCreateManagedWidget
      ("appearanceForm",
       xmFormWidgetClass,               frame,
       XmNchildType,                    XmFRAME_WORKAREA_CHILD,
       XmNfractionBase,                 2,
       NULL);

    sd->graph_info.widgets[SDGROPT_FG] = btn = XtVaCreateManagedWidget
      ("fgColorPushButton",
       xmPushButtonWidgetClass, form,
       XmNleftAttachment,       XmATTACH_POSITION,
       XmNleftPosition,         1,
       XmNbackground,           sd->config->Color.foreground.xcolor.pixel,
       XmNuserData,             (XtPointer)STRIPCONFIG_COLOR_FOREGROUND,
       NULL);
    sd->graph_info.cb_info[SDGROPT_FG].str = "Graph Foreground";
    sd->graph_info.cb_info[SDGROPT_FG].p = sd;
    XtAddCallback
      (btn, XmNactivateCallback, grcolor_btn_cb,
       &sd->graph_info.cb_info[SDGROPT_FG]);
    lbl = XtVaCreateManagedWidget
      (sd->graph_info.cb_info[SDGROPT_FG].str,
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    btn,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 btn,
       NULL);
    w = btn;
      
    sd->graph_info.widgets[SDGROPT_BG] = btn = XtVaCreateManagedWidget
      ("bgColorPushButton",
       xmPushButtonWidgetClass, form,
       XmNtopAttachment,        XmATTACH_WIDGET,
       XmNtopWidget,            w,
       XmNleftAttachment,       XmATTACH_POSITION,
       XmNleftPosition,         1,
       XmNbackground,           sd->config->Color.background.xcolor.pixel,
       XmNuserData,             (XtPointer)STRIPCONFIG_COLOR_BACKGROUND,
       NULL);
    sd->graph_info.cb_info[SDGROPT_BG].str = "Graph Background";
    sd->graph_info.cb_info[SDGROPT_BG].p = sd;
    XtAddCallback
      (btn, XmNactivateCallback, grcolor_btn_cb,
       &sd->graph_info.cb_info[SDGROPT_BG]);
    lbl = XtVaCreateManagedWidget
      (sd->graph_info.cb_info[SDGROPT_BG].str,
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    btn,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 btn,
       NULL);
    w = btn;
      
    sd->graph_info.widgets[SDGROPT_GRIDCLR] = btn = XtVaCreateManagedWidget
      ("gridColorPushButton",
       xmPushButtonWidgetClass, form,
       XmNtopAttachment,        XmATTACH_WIDGET,
       XmNtopWidget,            w,
       XmNleftAttachment,       XmATTACH_POSITION,
       XmNleftPosition,         1,
       XmNbackground,           sd->config->Color.grid.xcolor.pixel,
       XmNuserData,             (XtPointer)STRIPCONFIG_COLOR_GRID,
       NULL);
    sd->graph_info.cb_info[SDGROPT_GRIDCLR].str = "Grid Color";
    sd->graph_info.cb_info[SDGROPT_GRIDCLR].p = sd;
    XtAddCallback
      (btn, XmNactivateCallback, grcolor_btn_cb,
       &sd->graph_info.cb_info[SDGROPT_GRIDCLR]);
    lbl = XtVaCreateManagedWidget
      (sd->graph_info.cb_info[SDGROPT_GRIDCLR].str,
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    btn,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 btn,
       NULL);
    w = btn;

    pulldown = XmCreatePulldownMenu (form, "xgridPulldownMenu", 0, 0);
    XtAddCallback (pulldown, XmNentryCallback, gropt_xgrid_cb, sd);
    
    for (i = 0; i < STRIPGRID_NUM_TYPES; i++)
    {
      sprintf (char_buf, "option%"PRIiPTR"PushBG", i);
      sd->graph_info.xgrid[i] = XtVaCreateManagedWidget
        (char_buf, xmPushButtonGadgetClass, pulldown, NULL);
      XtAddCallback
        (sd->graph_info.xgrid[i], XmNactivateCallback, bogus_cb, (XtPointer)i);
    }
    i = 0;
    XtSetArg (args[i], XmNsubMenuId, pulldown); i++;
    sd->graph_info.widgets[SDGROPT_GRIDX] = tmp = XmCreateOptionMenu
      (form, "xgridOptionMenu", args, i);
    XtVaSetValues
      (sd->graph_info.widgets[SDGROPT_GRIDX],
       XmNbuttonSet,                    0,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_POSITION,
       XmNleftPosition,                 1,
       NULL);
    XtManageChild (sd->graph_info.widgets[SDGROPT_GRIDX]);
    lbl = XtVaCreateManagedWidget
      ("x-grid lines: ",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    tmp,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 tmp,
       NULL);
    w = tmp;
      
    pulldown = XmCreatePulldownMenu (form, "ygridPulldownMenu", 0, 0);
    XtAddCallback (pulldown, XmNentryCallback, gropt_ygrid_cb, sd);
    for (i = 0; i < STRIPGRID_NUM_TYPES; i++)
    {
      sprintf (char_buf, "option%"PRIiPTR"PushBG", i);
      sd->graph_info.ygrid[i] = XtVaCreateManagedWidget
        (char_buf, xmPushButtonGadgetClass, pulldown, NULL);
      XtAddCallback
        (sd->graph_info.ygrid[i], XmNactivateCallback, bogus_cb, (XtPointer)i);
    }
    i = 0;
    XtSetArg (args[i], XmNsubMenuId, pulldown); i++;
    sd->graph_info.widgets[SDGROPT_GRIDY] = tmp = XmCreateOptionMenu
      (form, "ygridOptionMenu", args, i);
    XtVaSetValues
      (sd->graph_info.widgets[SDGROPT_GRIDY],
       XmNbuttonSet,                    0,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_POSITION,
       XmNleftPosition,                 1,
       NULL);
    XtManageChild (sd->graph_info.widgets[SDGROPT_GRIDY]);
    lbl = XtVaCreateManagedWidget
      ("y-grid lines: ",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    tmp,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 tmp,
       NULL);
    w = tmp;
      
    sd->graph_info.widgets[SDGROPT_YAXISCLR] = btn = XtVaCreateManagedWidget
      ("selected curve",
       xmToggleButtonWidgetClass,       form,
       XmNlabelString,          yaxisclr_tgl_str[0],
       XmNset,                  True,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_POSITION,
       XmNleftPosition,                 1,
       NULL);
    XtAddCallback (btn, XmNvalueChangedCallback, gropt_tgl_cb, sd);
    lbl = XtVaCreateManagedWidget
      ("y-axis label color: ",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    btn,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 btn,
       NULL);
    w = btn;
      
    sd->graph_info.widgets[SDGROPT_LINEWIDTH] = tmp = XtVaCreateManagedWidget
      ("slider",
       xmScaleWidgetClass,              form,
       XmNorientation,                  XmHORIZONTAL,
       XmNminimum,                      STRIPMIN_OPTION_GRAPH_LINEWIDTH,
       XmNmaximum,                      STRIPMAX_OPTION_GRAPH_LINEWIDTH,
       XmNtopAttachment,                XmATTACH_WIDGET,
       XmNtopWidget,                    w,
       XmNleftAttachment,               XmATTACH_POSITION,
       XmNleftPosition,                 1,
       NULL);
    XtAddCallback (tmp, XmNvalueChangedCallback, gropt_slider_cb, sd);
    lbl = XtVaCreateManagedWidget
      ("data line-width: ",
       xmLabelWidgetClass,              form,
       XmNtopAttachment,                XmATTACH_OPPOSITE_WIDGET,
       XmNtopWidget,                    tmp,
       XmNleftAttachment,               XmATTACH_FORM,
       XmNbottomAttachment,             XmATTACH_OPPOSITE_WIDGET,
       XmNbottomWidget,                 tmp,
       NULL);
    w = tmp;
        

    XtRealizeWidget (sd->shell);
    XtMapWidget (sd->current_page = sd->pages[SDPAGE_CURVES]);
    XmProcessTraversal (sd->connect_txt, XmTRAVERSE_CURRENT);    
    XUnmapWindow (sd->display, XtWindow (sd->shell));

    for (i = 0; i < STRIP_MAX_CURVES; i++)
      sd->curve_info[i].modified = 0;
    sd->time_info.modified = 0;

    /* invoke updates for everything but curve stuff */
    StripConfigMask_clear (&scfg_mask);
    StripConfigMask_or (&scfg_mask, &SCFGMASK_ALL);
    StripConfigMask_xor (&scfg_mask, &SCFGMASK_CURVE);
    StripDialog_cfgupdate (scfg_mask, sd);
  }
  return (StripDialog)sd;
}


/*
 * StripDialog_delete
 */
void    StripDialog_delete      (StripDialog the_sd)
{
  StripDialogInfo *sd = (StripDialogInfo *)the_sd;
  if (!sd) return;

  if(sd->clrdlg) ColorDialog_delete(sd->clrdlg);
  free (sd);
}


/*
 * StripDialog_setattr
 */
int     StripDialog_setattr     (StripDialog the_sd, ...)
{
  va_list               ap;
  int                   attrib;
  StripDialogInfo       *sd = (StripDialogInfo *)the_sd;
  int                   ret_val = 1;
  int                   count;
  int                   i;
  SDWindowMenuItem      *items;

  va_start (ap, the_sd);
  for (attrib = va_arg (ap, StripDialogAttribute);
       (attrib != 0);
       attrib = va_arg (ap, StripDialogAttribute))
  {
    if ((ret_val = ((attrib > 0) && (attrib < STRIPDIALOG_LAST_ATTRIBUTE))))
      switch (attrib)
      {
          case STRIPDIALOG_CONNECT_FUNC:
            sd->callback[SDCALLBACK_CONNECT].func = va_arg (ap, SDcallback);
            break;
          case STRIPDIALOG_CONNECT_DATA:
            sd->callback[SDCALLBACK_CONNECT].data = va_arg (ap, void *);
            break;
          case STRIPDIALOG_SHOW_FUNC:
            sd->callback[SDCALLBACK_SHOW].func = va_arg (ap, SDcallback);
            break;
          case STRIPDIALOG_SHOW_DATA:
            sd->callback[SDCALLBACK_SHOW].data = va_arg (ap, void *);
            break;
          case STRIPDIALOG_CLEAR_FUNC:
            sd->callback[SDCALLBACK_CLEAR].func = va_arg (ap, SDcallback);
            break;
          case STRIPDIALOG_CLEAR_DATA:
            sd->callback[SDCALLBACK_CLEAR].data = va_arg (ap, void *);
            break;
          case STRIPDIALOG_DELETE_FUNC:
            sd->callback[SDCALLBACK_DELETE].func = va_arg (ap, SDcallback);
            break;
          case STRIPDIALOG_DELETE_DATA:
            sd->callback[SDCALLBACK_DELETE].data = va_arg (ap, void *);
            break;
          case STRIPDIALOG_DISMISS_FUNC:
            sd->callback[SDCALLBACK_DISMISS].func = va_arg (ap, SDcallback);
            break;
          case STRIPDIALOG_DISMISS_DATA:
            sd->callback[SDCALLBACK_DISMISS].data = va_arg (ap, void *);
            break;
          case STRIPDIALOG_QUIT_FUNC:
            sd->callback[SDCALLBACK_QUIT].func = va_arg (ap, SDcallback);
            break;
          case STRIPDIALOG_QUIT_DATA:
            sd->callback[SDCALLBACK_QUIT].data = va_arg (ap, void *);
            break;
          case STRIPDIALOG_WINDOW_MENU:
            items = va_arg (ap, SDWindowMenuItem *);
            count = va_arg (ap, int);
            for (i = 0; (i < count) && (i < MAX_WINDOW_MENU_ITEMS); i++)
            {
              sd->window_menu_info.items[i].info = items[i];
              xstr = XmStringCreateLocalized (items[i].name);
              XtVaSetValues
                (sd->window_menu_info.items[i].entry,
                 XmNlabelString,                xstr,
                 NULL);
              XmStringFree (xstr);
              XtManageChild (sd->window_menu_info.items[i].entry);
            }
            sd->window_menu_info.count = i;
            
            for (; i < MAX_WINDOW_MENU_ITEMS; i++)
              XtUnmanageChild (sd->window_menu_info.items[i].entry);
      }
    else break;
  }

  va_end (ap);
  return ret_val;
}


/*
 * StripDialog_getattr
 */
int     StripDialog_getattr     (StripDialog the_sd, ...)
{
  va_list               ap;
  int                   attrib;
  StripDialogInfo       *sd = (StripDialogInfo *)the_sd;
  int                   ret_val = 1;

  va_start (ap, the_sd);
  for (attrib = va_arg (ap, StripDialogAttribute);
       (attrib != 0);
       attrib = va_arg (ap, StripDialogAttribute))
  {
    if ((ret_val = ((attrib > 0) && (attrib < STRIPDIALOG_LAST_ATTRIBUTE))))
      switch (attrib)
      {
          case STRIPDIALOG_SHELL_WIDGET:
            *(va_arg (ap, Widget *)) = sd->shell;
            break;
          case STRIPDIALOG_CONNECT_FUNC:
            *(va_arg (ap, SDcallback *)) =
              sd->callback[SDCALLBACK_CONNECT].func;
            break;
          case STRIPDIALOG_CONNECT_DATA:
            *(va_arg (ap, void **)) =
              sd->callback[SDCALLBACK_CONNECT].data;
            break;
          case STRIPDIALOG_SHOW_FUNC:
            *(va_arg (ap, SDcallback *)) =
              sd->callback[SDCALLBACK_SHOW].func;
            break;
          case STRIPDIALOG_SHOW_DATA:
            *(va_arg (ap, void **)) =
              sd->callback[SDCALLBACK_SHOW].data;
            break;
          case STRIPDIALOG_CLEAR_FUNC:
            *(va_arg (ap, SDcallback *)) =
              sd->callback[SDCALLBACK_CLEAR].func;
            break;
          case STRIPDIALOG_CLEAR_DATA:
            *(va_arg (ap, void **)) =
              sd->callback[SDCALLBACK_CLEAR].data;
            break;
          case STRIPDIALOG_DELETE_FUNC:
            *(va_arg (ap, SDcallback *)) =
              sd->callback[SDCALLBACK_DELETE].func;
            break;
          case STRIPDIALOG_DELETE_DATA:
            *(va_arg (ap, void **)) =
              sd->callback[SDCALLBACK_DELETE].data;
            break;
          case STRIPDIALOG_DISMISS_FUNC:
            *(va_arg (ap, SDcallback *)) =
              sd->callback[SDCALLBACK_DISMISS].func;
            break;
          case STRIPDIALOG_DISMISS_DATA:
            *(va_arg (ap, void **)) =
              sd->callback[SDCALLBACK_DISMISS].data;
            break;
          case STRIPDIALOG_QUIT_FUNC:
            *(va_arg (ap, SDcallback *)) =
              sd->callback[SDCALLBACK_QUIT].func;
            break;
          case STRIPDIALOG_QUIT_DATA:
            *(va_arg (ap, void **)) =
              sd->callback[SDCALLBACK_QUIT].data;
            break;
      }
    else break;
  }

  va_end (ap);
  return ret_val;
}


/*
 * StripDialog_popup
 */
void    StripDialog_popup       (StripDialog the_sd)
{
  StripDialogInfo       *sd = (StripDialogInfo *)the_sd;

  XMapRaised (XtDisplay (sd->shell), XtWindow(sd->shell));
}


/*
 * StripDialog_popdown
 */
void    StripDialog_popdown     (StripDialog the_sd)
{
  StripDialogInfo       *sd = (StripDialogInfo *)the_sd;

  XUnmapWindow (XtDisplay (sd->shell), XtWindow(sd->shell));
}


/*
 * StripDialog_addcurve
 */
int     StripDialog_addcurve            (StripDialog the_sd, StripCurve curve)
{
  StripDialogInfo       *sd = (StripDialogInfo *)the_sd;
  int                   ret_val;

  if ((ret_val = (sd->sdcurve_count < STRIP_MAX_CURVES)))
  {
    char *str;
    
    insert_sdcurve (sd, sd->sdcurve_count, curve, False);
    show_sdcurve (sd, sd->sdcurve_count);
    sd->sdcurve_count++;

    /* if the curve's name matches the string in the connect text box,
     * then clear the the text box
     */
    str =  XmTextGetString (sd->connect_txt);
    if (strcmp
	(str,
	  (char *)StripCurve_getattr_val (curve, STRIPCURVE_NAME))== 0)
    {
      XmTextSetString (sd->connect_txt, "");
    }
    XtFree(str);
  }
  return ret_val;
}


/*
 * StripDialog_removecurve
 */
int     StripDialog_removecurve (StripDialog the_sd, StripCurve curve)
{
  StripDialogInfo       *sd = (StripDialogInfo *)the_sd;
  int                   ret_val;
  int                   i;

  for (i = 0; i < sd->sdcurve_count; i++)
    if (sd->curve_info[i].curve == curve)
      break;

  if ((ret_val = (i < sd->sdcurve_count)))
    delete_sdcurve (sd, i);
  return ret_val;
}


/*
 * StripDialog_addsomecurves
 */
int     StripDialog_addsomecurves       (StripDialog    the_sd,
                                         StripCurve     curves[])
{
  StripDialogInfo       *sd = (StripDialogInfo *)the_sd;
  int                   i, n;
  int                   ret_val;

  for (n = 0; curves[n]; n++);  /* how many curves? */
  
  if ((ret_val = (n < (STRIP_MAX_CURVES - sd->sdcurve_count))))
  {
    XtUnmapWidget (sd->curve_form);
    for (i = 0; i < n; i++)
    {
      insert_sdcurve (sd, sd->sdcurve_count+i, curves[i], False);
      show_sdcurve (sd, sd->sdcurve_count+i);
    }
    sd->sdcurve_count += n;
    if (sd->current_page == sd->pages[SDPAGE_CURVES])
      XtMapWidget (sd->curve_form);
  }
  return ret_val;
}


/*
 * StripDialog_removesomecurves
 */
int     StripDialog_removesomecurves    (StripDialog    the_sd,
                                         StripCurve     curves[])
{
  StripDialogInfo       *sd = (StripDialogInfo *)the_sd;
  int                   i, j, n;

  XtUnmapWidget (sd->curve_form);
  
  for (i = 0,  n = 0; i < sd->sdcurve_count; i++)
    for (j = 0; curves[j]; j++)
      if (sd->curve_info[i].curve == curves[j])
      {
        /* must remove this curve */
        sd->curve_info[i].curve = 0;
        n++;
      }
  
  for (i = 0, j = 1; (i < sd->sdcurve_count) && (j < sd->sdcurve_count); i++)
    if (sd->curve_info[i].curve == 0)
    {
      for (j = i+1; j < sd->sdcurve_count; j++)
        if (sd->curve_info[j].curve != 0)
          break;
      if (j < sd->sdcurve_count)
      {
        insert_sdcurve
          (sd, i, sd->curve_info[j].curve,
           sd->curve_info[j].modifying);
        sd->curve_info[j].curve = 0;
      }
    }
  
  sd->sdcurve_count -= n;
  for (i = 0; i < n; i++)
    hide_sdcurve (sd, sd->sdcurve_count+i);
  
  if (sd->current_page == sd->pages[SDPAGE_CURVES])
    XtMapWidget (sd->curve_form);

  return 1;
}


/*
 * StripDialog_update_curvestat
 */
int     StripDialog_update_curvestat    (StripDialog the_sd, StripCurve curve)
{
  StripDialogInfo       *sd = (StripDialogInfo *)the_sd;
  StripCurveInfo        *cv;
  int                   ret_val;
  int                   i;

  for (i = 0; i < sd->sdcurve_count; i++)
    if (sd->curve_info[i].curve == curve)
      break;

  if ((ret_val = (i < sd->sdcurve_count)))
  {
    cv = (StripCurveInfo *)curve;
    setwidgetval_status (sd, i, cv->status);
    setwidgetval_precision (sd, i, cv->details->precision);
    setwidgetval_min (sd, i, cv->details->min);
    setwidgetval_max (sd, i, cv->details->max);
    setwidgetval_scale  (sd, i, cv->details->scale);
  }
  return ret_val;
}


/*
 * StripDialog_isviewable
 */
int     StripDialog_isviewable  (StripDialog the_sd)
{
  StripDialogInfo       *sd = (StripDialogInfo *)the_sd;

  return (window_isviewable (sd->display, XtWindow (sd->shell)));
}


/*
 * StripDialog_ismapped
 */
int     StripDialog_ismapped    (StripDialog the_sd)
{
  StripDialogInfo       *sd = (StripDialogInfo *)the_sd;

  return (window_ismapped (sd->display, XtWindow (sd->shell)));
}

/*
 * StripDialog_isiconic
 */
int     StripDialog_isiconic    (StripDialog the_sd)
{
  StripDialogInfo       *sd = (StripDialogInfo *)the_sd;

  return (window_isiconic (sd->display, XtWindow (sd->shell)));
}

/*
 * StripDialog_reset
 */
void            StripDialog_reset(StripDialog the_sd)
{
  int                   which;
  int                   h, m, s;
  double                a, b, tmp;
  StripConfigMask       mask;
  StripCurveInfo        *sc;
  StripDialogInfo       *sd = (StripDialogInfo *)the_sd;

  if(!sd) return;

  /* loop over curves */
  for (which = 0; which < sd->sdcurve_count; which++)
  {
    /* make sure that max > min */
    getwidgetval_min (sd, which, &a);
    getwidgetval_max (sd, which, &b);
    if (a > b) { tmp = a; a = b; b = tmp; }

    sc = (StripCurveInfo *)sd->curve_info[which].curve;
    
    if (a < b)
    {
      if (StripCurve_setattr (sc, STRIPCURVE_MIN, a, 0))
      {
        setwidgetval_min (sd, which, a);
        StripConfigMask_set (&mask, SCFGMASK_CURVE_MIN);
      }
      else setwidgetval_min (sd, which, sc->details->min);
        
      if (StripCurve_setattr (sc, STRIPCURVE_MAX, b, 0))
      {
        setwidgetval_max (sd, which, b);
        StripConfigMask_set (&mask, SCFGMASK_CURVE_MAX);
      }
      else setwidgetval_max (sd, which, sc->details->max);
    }
    else
    {
      setwidgetval_min (sd, which, sc->details->min);
      setwidgetval_max (sd, which, sc->details->max);
    }
  }

  /* get time values */
  getwidgetval_tm_tshour (sd, &h);
  getwidgetval_tm_tsminute (sd, &m);
  getwidgetval_tm_tssecond (sd, &s);
  
  h = max (0, h);
  m = max (0, m);
  s = max (0, s);
  
  if (StripConfig_setattr
    (sd->config,
	STRIPCONFIG_TIME_TIMESPAN,     (unsigned)(h*3600 + m*60 + s),
	0))
  {
    StripConfigMask_set (&mask, SCFGMASK_TIME_TIMESPAN);
  }
  else
  {
    sec2hms (sd->config->Time.timespan, &h, &m, &s);
    setwidgetval_tm_tshour (sd, h);
    setwidgetval_tm_tsminute (sd, m);
    setwidgetval_tm_tssecond (sd, s);
  }

  /* update */
  if (StripConfigMask_intersect (&mask, &SCFGMASK_ALL))
    StripConfig_update (sd->config, mask);
}

/* ====== Static Function Definitions ====== */
/*
 * insert_sdcurve
 */
static void     insert_sdcurve  (StripDialogInfo        *sd,
                                 int                    idx,
                                 StripCurve             curve,
                                 int                    modifying)
{
  StripCurveInfo        *sc;
  
  sc = (StripCurveInfo *)(sd->curve_info[idx].curve = curve);

  setwidgetval_name             (sd, idx, sc->details->name);
  setwidgetval_status           (sd, idx, sc->status);
  setwidgetval_color            (sd, idx, sc->details->color->xcolor.pixel);
  setwidgetval_plotstat         (sd, idx, sc->details->plotstat);
  setwidgetval_precision        (sd, idx, sc->details->precision);
  setwidgetval_min              (sd, idx, sc->details->min);
  setwidgetval_max              (sd, idx, sc->details->max);
  setwidgetval_scale            (sd, idx, sc->details->scale);
  
  XtUnmapWidget (sd->curve_info[idx].widgets[SDCURVE_PRECISION]);
  XtUnmapWidget (sd->curve_info[idx].widgets[SDCURVE_MIN]);
  XtUnmapWidget (sd->curve_info[idx].widgets[SDCURVE_MAX]);
  setwidgetval_modify           (sd, idx, modifying);
  XtMapWidget (sd->curve_info[idx].widgets[SDCURVE_PRECISION]);
  XtMapWidget (sd->curve_info[idx].widgets[SDCURVE_MIN]);
  XtMapWidget (sd->curve_info[idx].widgets[SDCURVE_MAX]);
}


/*
 * delete_sdcurve
 */
static void     delete_sdcurve  (StripDialogInfo *sd, int idx)
{
  int   i;

  XtUnmapWidget (sd->curve_form);
  /* shift all the remaining curves up one */
  for (i = idx+1; i < sd->sdcurve_count; i++)
  {
    insert_sdcurve
      (sd, i-1, sd->curve_info[i].curve, sd->curve_info[i].modifying);
  }
  sd->sdcurve_count--;
  hide_sdcurve (sd, sd->sdcurve_count);
  if (sd->current_page == sd->pages[SDPAGE_CURVES])
    XtMapWidget (sd->curve_form);
}


/*
 * show_sdcurve
 */
static void     show_sdcurve    (StripDialogInfo *sd, int idx)
{
  int   i;
  for (i = 0; i < SDCURVE_LAST_ATTRIBUTE; i++)
    XtMapWidget (sd->curve_info[idx].widgets[i]);
  XtMapWidget (sd->curve_info[idx].top_sep);
}
     

/*
 * hide_sdcurve
 */
static void     hide_sdcurve    (StripDialogInfo *sd, int idx)
{
  int   i;
  for (i = 0; i < SDCURVE_LAST_ATTRIBUTE; i++)
    XtUnmapWidget (sd->curve_info[idx].widgets[i]);
  XtUnmapWidget (sd->curve_info[idx].top_sep);
}


/*
 * setwidgetval_name
 */
static void     setwidgetval_name       (StripDialogInfo        *sd,
                                         int                    which,
                                         char                   *name)
{
  xstr = XmStringCreateLocalized (name);
  XtVaSetValues
    (sd->curve_info[which].widgets[SDCURVE_NAME],
     XmNlabelString,                    xstr,
     NULL);
  XmStringFree (xstr);
}


/*
 * setwidgetval_status
 */
static void     setwidgetval_status     (StripDialogInfo        *sd,
                                         int                    which,
                                         unsigned               stat)
{
  int   s = ((stat & STRIPCURVE_WAITING)? 0 : 1);
  
  /* if the curve is not connected, grey out its name */
  XtVaSetValues
    (sd->curve_info[which].widgets[SDCURVE_NAME],
     XmNsensitive,                      s,
     NULL);
}


/*
 * setwidgetval_color
 */
static void     setwidgetval_color      (StripDialogInfo        *sd,
                                         int                    which,
                                         Pixel                  color)
{
  XtVaSetValues
    (sd->curve_info[which].widgets[SDCURVE_COLOR],
     XmNbackground,                     color,
     NULL);
}


/*
 * setwidgetval_plotstat
 */
static void     setwidgetval_plotstat   (StripDialogInfo        *sd,
                                         int                    which,
                                         int                    stat)
{
  XtVaSetValues
    (sd->curve_info[which].widgets[SDCURVE_PLOTSTAT], XmNset, stat, NULL);
}


/*
 * setwidgetval_scale
 */
static void     setwidgetval_scale      (StripDialogInfo        *sd,
                                         int                    which,
                                         int                    scale)
{
  XtVaSetValues
    (sd->curve_info[which].widgets[SDCURVE_SCALE],
     XmNset,    (Boolean)(scale == STRIPSCALE_LOG_10),
     NULL);
}


/*
 * setwidgetval_precision
 */
static void     setwidgetval_precision  (StripDialogInfo        *sd,
                                         int                    which,
                                         int                    p)
{
  sprintf (char_buf, "%d", p);
  xstr = XmStringCreateLocalized (char_buf);
  XmTextSetString (sd->curve_info[which].precision_txt, char_buf);
  XtVaSetValues
    (sd->curve_info[which].precision_lbl,
     XmNlabelString,                    xstr,
     NULL);
  XmStringFree (xstr);
}


/*
 * setwidgetval_min
 */
static void     setwidgetval_min        (StripDialogInfo        *sd,
                                         int                    which,
                                         double                 val)
{
  StripCurveInfo        *sc = (StripCurveInfo *)sd->curve_info[which].curve;

  dbl2str (val, sc->details->precision, char_buf, MAX_REALNUM_LEN);  
  XmTextSetString (sd->curve_info[which].min_txt, char_buf);
  xstr = XmStringCreateLocalized (char_buf);
  XtVaSetValues
    (sd->curve_info[which].min_lbl,
     XmNlabelString,                    xstr,
     NULL);
  XmStringFree (xstr);
}


/*
 * setwidgetval_max
 */
static void     setwidgetval_max        (StripDialogInfo        *sd,
                                         int                    which,
                                         double                 val)
{
  StripCurveInfo        *sc = (StripCurveInfo *)sd->curve_info[which].curve;

  dbl2str (val, sc->details->precision, char_buf, MAX_REALNUM_LEN);  
  XmTextSetString (sd->curve_info[which].max_txt, char_buf);

  xstr = XmStringCreateLocalized (char_buf);
  XtVaSetValues
    (sd->curve_info[which].max_lbl,
     XmNlabelString,                    xstr,
     NULL);
  XmStringFree (xstr);
}


/*
 * setwidgetval_modify
 */
static void     setwidgetval_modify     (StripDialogInfo        *sd,
                                         int                    which,
                                         int                    modify)
{
  if ((sd->curve_info[which].modifying = modify))
  {
    sd->curve_info[which].widgets[SDCURVE_PRECISION] =
      sd->curve_info[which].precision_txt;
    sd->curve_info[which].widgets[SDCURVE_MIN] =
      sd->curve_info[which].min_txt;
    sd->curve_info[which].widgets[SDCURVE_MAX] =
      sd->curve_info[which].max_txt;
    xstr =  modify_btn_str[1];
  }
  else
  {
    sd->curve_info[which].widgets[SDCURVE_PRECISION] =
      sd->curve_info[which].precision_lbl;
    sd->curve_info[which].widgets[SDCURVE_MIN] =
      sd->curve_info[which].min_lbl;
    sd->curve_info[which].widgets[SDCURVE_MAX] =
      sd->curve_info[which].max_lbl;
    xstr =  modify_btn_str[0];
  }
  XtVaSetValues
    (sd->curve_info[which].widgets[SDCURVE_MODIFY],
     XmNlabelString,                    xstr,
     NULL);
}


#if 0
/* KE: unused */
/*
 * getwidgetval_name
 */
static char     *getwidgetval_name      (StripDialogInfo *sd, int which)
{
  return XmTextGetString (sd->curve_info[which].widgets[SDCURVE_NAME]);
}


/*
 * getwidgetval_status
 */
static char     *getwidgetval_status    (StripDialogInfo        *BOGUS(1),
                                         int                    BOGUS(2))
{
  return "unknown";
}


/*
 * getwidgetval_color
 */
static Pixel    getwidgetval_color      (StripDialogInfo *sd, int which)
{
  Pixel color;
  XtVaGetValues
    (sd->curve_info[which].widgets[SDCURVE_COLOR],
     XmNbackground,                     &color,
     NULL);
  return color;
}


/*
 * getwidgetval_plotstat
 */
static int      getwidgetval_plotstat   (StripDialogInfo *sd, int which)
{
  return XmToggleButtonGetState
    (sd->curve_info[which].widgets[SDCURVE_PLOTSTAT]);
}


/*
 * getwidgetval_scale
 */
static int      getwidgetval_scale      (StripDialogInfo *sd, int which)
{
  return XmToggleButtonGetState
    (sd->curve_info[which].widgets[SDCURVE_SCALE]);
}
#endif

/*
 * getwidgetval_min
 */
void getwidgetval_min (StripDialog the_sd, int which, double *val)
{
  StripDialogInfo *sd = (StripDialogInfo *)the_sd;
  char  *str;

  str = XmTextGetString (sd->curve_info[which].min_txt);
  *val = atof (str);
  XtFree (str);
}


/*
 * getwidgetval_max
 */
void getwidgetval_max (StripDialog the_sd, int which, double *val)
{
  StripDialogInfo *sd = (StripDialogInfo *)the_sd;
  char  *str;

  str = XmTextGetString (sd->curve_info[which].max_txt);
  *val = atof (str);
  XtFree (str);
}


/*
 * getwidgetval_precision
 */
void getwidgetval_precision (StripDialog the_sd, int which, int *val)
{
  StripDialogInfo *sd = (StripDialogInfo *)the_sd;
  char  *str;

  str = XmTextGetString (sd->curve_info[which].precision_txt);
  *val = atoi (str);
  XtFree (str);
}


static void     setwidgetval_tm_tshour          (StripDialogInfo *sd, int val)
{
  sprintf (char_buf, "%d", val);
  xstr = XmStringCreateLocalized (char_buf);
  XmTextSetString (sd->time_info.ts_hour_txt, char_buf);
  XtVaSetValues
    (sd->time_info.ts_hour_lbl,
     XmNlabelString,                    xstr,
     NULL);
  XmStringFree (xstr);
}


static void     setwidgetval_tm_tsminute        (StripDialogInfo *sd, int val)
{
  sprintf (char_buf, "%d", val);
  xstr = XmStringCreateLocalized (char_buf);
  XmTextSetString (sd->time_info.ts_minute_txt, char_buf);
  XtVaSetValues
    (sd->time_info.ts_minute_lbl,
     XmNlabelString,                    xstr,
     NULL);
  XmStringFree (xstr);
}


static void     setwidgetval_tm_tssecond        (StripDialogInfo *sd, int val)
{
  sprintf (char_buf, "%d", val);
  xstr = XmStringCreateLocalized (char_buf);
  XmTextSetString (sd->time_info.ts_second_txt, char_buf);
  XtVaSetValues
    (sd->time_info.ts_second_lbl,
     XmNlabelString,                    xstr,
     NULL);
  XmStringFree (xstr);
}


static void     setwidgetval_tm_numsamples      (StripDialogInfo *sd, int val)
{
  sprintf (char_buf, "%d", val);
  xstr = XmStringCreateLocalized (char_buf);
  XmTextSetString (sd->time_info.num_samples_txt, char_buf);
  XtVaSetValues
    (sd->time_info.num_samples_lbl,
     XmNlabelString,                    xstr,
     NULL);
  XmStringFree (xstr);
}


static void     setwidgetval_tm_ds      (StripDialogInfo        *sd,
                                         double                 val)
{
  sprintf(char_buf, "%.3g", val);
  xstr = XmStringCreateLocalized (char_buf);
  XmTextSetString (sd->time_info.ds_txt, char_buf);
  XtVaSetValues
    (sd->time_info.ds_lbl,
     XmNlabelString,                    xstr,
     NULL);
  XmStringFree (xstr);
}


static void     setwidgetval_tm_gr      (StripDialogInfo        *sd,
                                         double                 val)
{
  sprintf(char_buf, "%.3g", val);
  xstr = XmStringCreateLocalized (char_buf);
  XmTextSetString (sd->time_info.gr_txt, char_buf);
  XtVaSetValues
    (sd->time_info.gr_lbl,
     XmNlabelString,                    xstr,
     NULL);
  XmStringFree (xstr);
}


static void     setwidgetval_tm_modify  (StripDialogInfo        *sd,
                                         int                    modify)
{
  if ((sd->time_info.modifying = modify))
  {
    sd->time_info.widgets[SDTMOPT_TSHOUR] = sd->time_info.ts_hour_txt;
    sd->time_info.widgets[SDTMOPT_TSMINUTE] = sd->time_info.ts_minute_txt;
    sd->time_info.widgets[SDTMOPT_TSSECOND] = sd->time_info.ts_second_txt;
    sd->time_info.widgets[SDTMOPT_NUMSAMPLES] = sd->time_info.num_samples_txt;
    sd->time_info.widgets[SDTMOPT_DS] = sd->time_info.ds_txt;
    sd->time_info.widgets[SDTMOPT_GR] = sd->time_info.gr_txt;
    xstr = modify_btn_str[1];
  }
  else
  {
    sd->time_info.widgets[SDTMOPT_TSHOUR] = sd->time_info.ts_hour_lbl;
    sd->time_info.widgets[SDTMOPT_TSMINUTE] = sd->time_info.ts_minute_lbl;
    sd->time_info.widgets[SDTMOPT_TSSECOND] = sd->time_info.ts_second_lbl;
    sd->time_info.widgets[SDTMOPT_NUMSAMPLES] = sd->time_info.num_samples_lbl;
    sd->time_info.widgets[SDTMOPT_DS] = sd->time_info.ds_lbl;
    sd->time_info.widgets[SDTMOPT_GR] = sd->time_info.gr_lbl;
    xstr = modify_btn_str[0];
  }
  XtVaSetValues
    (sd->time_info.widgets[SDTMOPT_MODIFY],
     XmNlabelString,                    xstr,
     NULL);
}


static void     getwidgetval_tm_tshour         (StripDialogInfo        *sd,
                                                 int                    *val)
{
  char  *str;

  str = XmTextGetString (sd->time_info.ts_hour_txt);
  *val = atoi (str);
  XtFree (str);
}


static void     getwidgetval_tm_tsminute       (StripDialogInfo        *sd,
                                                 int                    *val)
{
  char  *str;

  str = XmTextGetString (sd->time_info.ts_minute_txt);
  *val = atoi (str);
  XtFree (str);
}


static void     getwidgetval_tm_tssecond       (StripDialogInfo        *sd,
                                                 int                    *val)
{
  char  *str;

  str = XmTextGetString (sd->time_info.ts_second_txt);
  *val = atoi (str);
  XtFree (str);
}


static void     getwidgetval_tm_numsamples     (StripDialogInfo        *sd,
                                                 int                    *val)
{
  char  *str;

  str = XmTextGetString (sd->time_info.num_samples_txt);
  *val = atoi (str);
  XtFree (str);
}


static void     getwidgetval_tm_ds             (StripDialogInfo        *sd,
                                                 double                 *val)
{
  char  *str;

  str = XmTextGetString (sd->time_info.ds_txt);
  *val = atof (str);
  XtFree (str);
}


static void     getwidgetval_tm_gr             (StripDialogInfo        *sd,
                                                 double                 *val)
{
  char  *str;

  str = XmTextGetString (sd->time_info.gr_txt);
  *val = atof (str);
  XtFree (str);
}


/* graph controls */
static void     setwidgetval_gr_fg              (StripDialogInfo *sd,
                                                 Pixel           pixel)
{
  XtVaSetValues
    (sd->graph_info.widgets[SDGROPT_FG], XmNbackground, pixel, NULL);
}


static void     setwidgetval_gr_bg              (StripDialogInfo *sd,
                                                 Pixel           pixel)
{
  XtVaSetValues
    (sd->graph_info.widgets[SDGROPT_BG], XmNbackground, pixel, NULL);
}


static void     setwidgetval_gr_gridclr         (StripDialogInfo *sd,
                                                 Pixel           pixel)
{
  XtVaSetValues
    (sd->graph_info.widgets[SDGROPT_GRIDCLR], XmNbackground, pixel, NULL);
}


static void     setwidgetval_gr_linewidth       (StripDialogInfo        *sd,
                                                 int                    val)
{
  XtVaSetValues
    (sd->graph_info.widgets[SDGROPT_LINEWIDTH],
     XmNvalue,                          val,
     NULL);
}


static void     setwidgetval_gr_gridx           (StripDialogInfo        *sd,
                                                 int                    idx)
{
  XtVaSetValues
    (sd->graph_info.widgets[SDGROPT_GRIDX],
     XmNmenuHistory,    sd->graph_info.xgrid[idx],
     NULL);
}


static void     setwidgetval_gr_gridy           (StripDialogInfo        *sd,
                                                 int                    idx)
{
  XtVaSetValues
    (sd->graph_info.widgets[SDGROPT_GRIDY],
     XmNmenuHistory,    sd->graph_info.ygrid[idx],
     NULL);
}


static void     setwidgetval_gr_yaxisclr        (StripDialogInfo        *sd,
                                                 int                    stat)
{
  XtVaSetValues
    (sd->graph_info.widgets[SDGROPT_YAXISCLR],
     XmNset,                            stat,
     XmNlabelString,                    yaxisclr_tgl_str[stat? 0 : 1],
     NULL);
}


#if 0
/* KE: unused */
static Pixel    getwidgetval_gr_fg              (StripDialogInfo *BOGUS(1))
{
  return (Pixel)0;
}


static Pixel    getwidgetval_gr_bg              (StripDialogInfo *BOGUS(1))
{
  return (Pixel)0;
}


static Pixel    getwidgetval_gr_gridclr         (StripDialogInfo *BOGUS(1))
{
  return (Pixel)0;
}


static int      getwidgetval_gr_linewidth       (StripDialogInfo *BOGUS(1))
{
  return (int)0;
}


static int      getwidgetval_gr_gridx           (StripDialogInfo *BOGUS(1))
{
  return (int)0;
}


static int      getwidgetval_gr_gridy           (StripDialogInfo *BOGUS(1))
{
  return (int)0;
}


static int      getwidgetval_gr_yaxisclr        (StripDialogInfo *BOGUS(1))
{
  return (int)0;
}
#endif


/* ====== Static Callback Function Definitions ====== */
static void     connect_btn_cb  (Widget         BOGUS(1),
                                 XtPointer      data,
                                 XtPointer      call)
{
  XmAnyCallbackStruct   *cbs = (XmAnyCallbackStruct *)call;
  XEvent                *event = cbs->event;
  StripDialogInfo       *sd = (StripDialogInfo *)data;
  char                  *str;
  Time                  when;

  when = (event->type == ButtonPress? event->xbutton.time : event->xkey.time);

  if (strlen (str = XmTextGetString (sd->connect_txt)) > 0)
    if (sd->callback[SDCALLBACK_CONNECT].func != NULL)
      sd->callback[SDCALLBACK_CONNECT].func
        (sd->callback[SDCALLBACK_CONNECT].data, str);

  XmTextSetSelection
    (sd->connect_txt, (XmTextPosition)0, (XmTextPosition)strlen(str), when);
}

static void     color_btn_cb    (Widget w, XtPointer data, XtPointer BOGUS(1))
{
  StripDialogInfo       *sd;
  StripCurveInfo        *sc;
  cColor                *pcolor;
  intptr_t              which = (intptr_t)data;
  int                   i;

  XtVaGetValues (w, XmNuserData, &sd, NULL);
  sc = (StripCurveInfo *)sd->curve_info[which].curve;

  /* need to find out which color in the config object corresponds
   * to the color used by this curve */
  for (i = 0; i < STRIP_MAX_CURVES; i++)
    if (sc->details->color == &sd->config->Color.color[i])
      break;

  if (i < STRIP_MAX_CURVES)
  {
    sd->color_edit_which = STRIPCONFIG_COLOR_COLOR1 + i;
    pcolor = sc->details->color;
    ColorDialog_popup (sd->clrdlg, sc->details->name, pcolor, clrdlg_cb, sd);
  }
}

static void     modify_btn_cb   (Widget w, XtPointer data, XtPointer BOGUS(1))
{
  StripDialogInfo       *sd;
  StripCurveInfo        *sc;
  StripConfigMask       mask;
  intptr_t              which = (intptr_t)data;
  int                   ival;
  double                a, b, tmp;

  StripConfigMask_clear (&mask);
  
  XtVaGetValues (w, XmNuserData, &sd, NULL);
  XtUnmapWidget (sd->curve_info[which].widgets[SDCURVE_PRECISION]);
  XtUnmapWidget (sd->curve_info[which].widgets[SDCURVE_MIN]);
  XtUnmapWidget (sd->curve_info[which].widgets[SDCURVE_MAX]);
  
  if (sd->curve_info[which].modifying)
  {
    sc = (StripCurveInfo *)sd->curve_info[which].curve;
      
    getwidgetval_precision (sd, which, &ival);
    if (StripCurve_setattr (sc, STRIPCURVE_PRECISION, ival, 0))
    {
      setwidgetval_precision    (sd, which, ival);
      StripConfigMask_set (&mask, SCFGMASK_CURVE_PRECISION);
    }
    else setwidgetval_precision (sd, which, sc->details->precision);

    /* make sure that max > min */
    getwidgetval_min (sd, which, &a);
    getwidgetval_max (sd, which, &b);
    if (a > b) { tmp = a; a = b; b = tmp; }

    if (a < b)
    {
      if (StripCurve_setattr (sc, STRIPCURVE_MIN, a, 0))
      {
        setwidgetval_min (sd, which, a);
        StripConfigMask_set (&mask, SCFGMASK_CURVE_MIN);
      }
      else setwidgetval_min (sd, which, sc->details->min);
        
      if (StripCurve_setattr (sc, STRIPCURVE_MAX, b, 0))
      {
        setwidgetval_max (sd, which, b);
        StripConfigMask_set (&mask, SCFGMASK_CURVE_MAX);
      }
      else setwidgetval_max (sd, which, sc->details->max);
    }
    else
    {
      setwidgetval_min (sd, which, sc->details->min);
      setwidgetval_max (sd, which, sc->details->max);
    }

    if (StripConfigMask_intersect (&mask, &SCFGMASK_ALL))
      StripConfig_update (sd->config, mask);
  }

  setwidgetval_modify (sd, which, !sd->curve_info[which].modifying);
  
  XtMapWidget (sd->curve_info[which].widgets[SDCURVE_PRECISION]);
  XtMapWidget (sd->curve_info[which].widgets[SDCURVE_MIN]);
  XtMapWidget (sd->curve_info[which].widgets[SDCURVE_MAX]);
}

static void     plotstat_tgl_cb (Widget w, XtPointer data, XtPointer call)
{
  XmToggleButtonCallbackStruct  *cbs = (XmToggleButtonCallbackStruct *)call;
  StripDialogInfo               *sd;
  StripCurve                    *sc;
  intptr_t                     idx = (intptr_t)data;
  StripConfigMask               mask;

  XtVaGetValues (w, XmNuserData, &sd, NULL);
  sc = (StripCurve *)sd->curve_info[idx].curve;
  setwidgetval_plotstat (sd, idx, cbs->set);
  StripCurve_setattr (sc, STRIPCURVE_PLOTSTAT, cbs->set, 0);
  
  StripConfigMask_clear (&mask);
  StripConfigMask_set (&mask, SCFGMASK_CURVE_PLOTSTAT);
  StripConfig_update (sd->config, mask);
}


static void     scale_tgl_cb    (Widget w, XtPointer data, XtPointer call)
{
  XmToggleButtonCallbackStruct  *cbs = (XmToggleButtonCallbackStruct *)call;
  StripDialogInfo               *sd;
  StripCurve                    *sc;
  intptr_t                      idx = (intptr_t)data;
  StripConfigMask               mask;

  XtVaGetValues (w, XmNuserData, &sd, NULL);
  sc = (StripCurve *)sd->curve_info[idx].curve;
  setwidgetval_scale (sd, idx, cbs->set);
  StripCurve_setattr
    (sc,
     STRIPCURVE_SCALE, cbs->set? STRIPSCALE_LOG_10 : STRIPSCALE_LINEAR,
     0);
  
  StripConfigMask_clear (&mask);
  StripConfigMask_set (&mask, SCFGMASK_CURVE_SCALE);
  StripConfig_update (sd->config, mask);
}


static void     text_focus_cb   (Widget         BOGUS(1),
                                 XtPointer      BOGUS(2),
                                 XtPointer      BOGUS(3))
{
}

static void     remove_btn_cb   (Widget w, XtPointer data, XtPointer BOGUS(1))
{
  StripDialogInfo       *sd;
  StripCurve            sc;
  intptr_t              idx = (intptr_t)data;
  
  XtVaGetValues (w, XmNuserData, &sd, NULL);
  sc = sd->curve_info[idx].curve;
  delete_sdcurve (sd, idx);
  if (sd->callback[SDCALLBACK_DELETE].func != NULL)
    sd->callback[SDCALLBACK_DELETE].func
      (sd->callback[SDCALLBACK_DELETE].data, sc);
}

static void     tmmodify_btn_cb (Widget         BOGUS(1),
                                 XtPointer      data,
                                 XtPointer      BOGUS(2))
{
  StripDialogInfo       *sd = (StripDialogInfo *)data;
  StripConfigMask       mask;
  int                   i;
  int                   h, m, s;
  double                dval;

  StripConfigMask_clear (&mask);

  for (i = SDTMOPT_TSHOUR; i <= SDTMOPT_GR; i++)
    XtUnmapWidget (sd->time_info.widgets[i]);
  
  if (sd->time_info.modifying)
  {
    XtUnmapWidget (sd->time_info.widgets[SDTMOPT_CANCEL]);
      
    getwidgetval_tm_tshour (sd, &h);
    getwidgetval_tm_tsminute (sd, &m);
    getwidgetval_tm_tssecond (sd, &s);

    h = max (0, h);
    m = max (0, m);
    s = max (0, s);
      
    if (StripConfig_setattr
        (sd->config,
         STRIPCONFIG_TIME_TIMESPAN,     (unsigned)(h*3600 + m*60 + s),
         0))
    {
      StripConfigMask_set (&mask, SCFGMASK_TIME_TIMESPAN);
    }
    else
    {
      sec2hms (sd->config->Time.timespan, &h, &m, &s);
      setwidgetval_tm_tshour (sd, h);
      setwidgetval_tm_tsminute (sd, m);
      setwidgetval_tm_tssecond (sd, s);
    }

    getwidgetval_tm_numsamples (sd, &s);
    if (StripConfig_setattr
        (sd->config,
         STRIPCONFIG_TIME_NUM_SAMPLES, s,
         0))
    {
      StripConfigMask_set (&mask, SCFGMASK_TIME_NUM_SAMPLES);
    }
    else setwidgetval_tm_ds (sd, sd->config->Time.num_samples);

      
    getwidgetval_tm_ds (sd, &dval);
    if (StripConfig_setattr
        (sd->config,
         STRIPCONFIG_TIME_SAMPLE_INTERVAL, dval,
         0))
    {
      StripConfigMask_set (&mask, SCFGMASK_TIME_SAMPLE_INTERVAL);
    }
    else setwidgetval_tm_ds (sd, sd->config->Time.sample_interval);
      
    getwidgetval_tm_gr (sd, &dval);
    if (StripConfig_setattr
        (sd->config,
         STRIPCONFIG_TIME_REFRESH_INTERVAL, dval,
         0))
    {
      StripConfigMask_set (&mask, SCFGMASK_TIME_REFRESH_INTERVAL);
    }
    else setwidgetval_tm_gr (sd, sd->config->Time.refresh_interval);

    if (StripConfigMask_intersect (&mask, &SCFGMASK_ALL))
      StripConfig_update (sd->config, mask);
  }
  else XtMapWidget (sd->time_info.widgets[SDTMOPT_CANCEL]);
  
  setwidgetval_tm_modify (sd, !sd->time_info.modifying);

  for (i = SDTMOPT_TSHOUR; i <= SDTMOPT_GR; i++)
    XtMapWidget (sd->time_info.widgets[i]);
}


static void     tmcancel_btn_cb (Widget         BOGUS(1),
                                 XtPointer      data,
                                 XtPointer      BOGUS(2))
{
  StripDialogInfo       *sd = (StripDialogInfo *)data;
  int                   i;
  int                   h, m, s;

  for (i = SDTMOPT_TSHOUR; i <= SDTMOPT_GR; i++)
    XtUnmapWidget (sd->time_info.widgets[i]);
  
  XtUnmapWidget (sd->time_info.widgets[SDTMOPT_CANCEL]);
      
  sec2hms (sd->config->Time.timespan, &h, &m, &s);
  setwidgetval_tm_tshour (sd, h);
  setwidgetval_tm_tsminute (sd, m);
  setwidgetval_tm_tssecond (sd, s);
      
  setwidgetval_tm_numsamples (sd, sd->config->Time.num_samples);
  setwidgetval_tm_ds (sd, sd->config->Time.sample_interval);
  setwidgetval_tm_gr (sd, sd->config->Time.refresh_interval);
  setwidgetval_tm_modify (sd, !sd->time_info.modifying);

  for (i = SDTMOPT_TSHOUR; i <= SDTMOPT_GR; i++)
    XtMapWidget (sd->time_info.widgets[i]);
}


static void     grcolor_btn_cb  (Widget w, XtPointer data, XtPointer BOGUS(1))
{
  CBInfo                *cbi = (CBInfo *)data;
  StripDialogInfo       *sd = (StripDialogInfo *)cbi->p;
  cColor                *pcolor;
  int                   which;

  XtVaGetValues (w, XmNuserData, &which, NULL);
  sd->color_edit_which = (StripConfigAttribute)which;
  StripConfig_getattr
    (sd->config, (StripConfigAttribute)sd->color_edit_which, &pcolor, 0);
  ColorDialog_popup (sd->clrdlg, cbi->str, pcolor, clrdlg_cb, sd);
}


static void     gropt_slider_cb (Widget         BOGUS(1),
                                 XtPointer      data,
                                 XtPointer      call)
{
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *)call;
  StripDialogInfo       *sd = (StripDialogInfo *)data;
  StripConfigMask       mask;

  if (StripConfig_setattr
      (sd->config, STRIPCONFIG_OPTION_GRAPH_LINEWIDTH, cbs->value, 0))
  {
    StripConfigMask_clear (&mask);
    StripConfigMask_set (&mask, SCFGMASK_OPTION_GRAPH_LINEWIDTH);
    StripConfig_update (sd->config, mask);
  }
  else setwidgetval_gr_linewidth (sd, sd->config->Option.graph_linewidth);
}


static void      gropt_xgrid_cb  (Widget w, XtPointer data, XtPointer call)
{
  XmRowColumnCallbackStruct   *cbs = (XmRowColumnCallbackStruct *)call;
  StripDialogInfo             *sd = (StripDialogInfo *)data;
  StripConfigMask              mask;

  if (StripConfig_setattr
      (sd->config, STRIPCONFIG_OPTION_GRID_XON, (intptr_t)cbs->data, 0))
  {
    StripConfigMask_clear (&mask);
    StripConfigMask_set (&mask, SCFGMASK_OPTION_GRID_XON);
    StripConfig_update (sd->config, mask);
  }
  else setwidgetval_gr_gridx (sd, sd->config->Option.grid_xon);
}
  
  
static void      gropt_ygrid_cb  (Widget w, XtPointer data, XtPointer call)
{
  XmRowColumnCallbackStruct   *cbs = (XmRowColumnCallbackStruct *)call;
  StripDialogInfo             *sd = (StripDialogInfo *)data;
  StripConfigMask              mask;

  if (StripConfig_setattr
      (sd->config, STRIPCONFIG_OPTION_GRID_YON, (intptr_t)cbs->data, 0))
  {
    StripConfigMask_clear (&mask);
    StripConfigMask_set (&mask, SCFGMASK_OPTION_GRID_YON);
    StripConfig_update (sd->config, mask);
  }
  else setwidgetval_gr_gridy (sd, sd->config->Option.grid_yon);
}

static void     gropt_tgl_cb    (Widget w, XtPointer data, XtPointer call)
{
  XmToggleButtonCallbackStruct  *cbs = (XmToggleButtonCallbackStruct *)call;
  StripDialogInfo       *sd = (StripDialogInfo *)data;
  StripConfigMask       mask;

  StripConfigMask_clear (&mask);
  
  if (w == sd->graph_info.widgets[SDGROPT_GRIDX])
  {
    if (StripConfig_setattr
        (sd->config, STRIPCONFIG_OPTION_GRID_XON, cbs->set, 0))
    {
      StripConfigMask_set (&mask, SCFGMASK_OPTION_GRID_XON);
      StripConfig_update (sd->config, mask);
    }
    else setwidgetval_gr_gridx (sd, sd->config->Option.grid_xon);
  }
  else if (w == sd->graph_info.widgets[SDGROPT_GRIDY])
  {
    if (StripConfig_setattr
        (sd->config, STRIPCONFIG_OPTION_GRID_YON, cbs->set, 0))
    {
      StripConfigMask_set (&mask, SCFGMASK_OPTION_GRID_YON);
      StripConfig_update (sd->config, mask);
    }
    else setwidgetval_gr_gridy (sd, sd->config->Option.grid_yon);
  }
  else if (w == sd->graph_info.widgets[SDGROPT_YAXISCLR])
  {
    if (StripConfig_setattr
        (sd->config, STRIPCONFIG_OPTION_AXIS_YCOLORSTAT, cbs->set, 0))
    {
      StripConfigMask_set (&mask, SCFGMASK_OPTION_AXIS_YCOLORSTAT);
      StripConfig_update (sd->config, mask);
    }
    else setwidgetval_gr_yaxisclr (sd, sd->config->Option.axis_ycolorstat);
  }
}


static void     filemenu_cb     (Widget w, XtPointer data, XtPointer BOGUS(1))
{
  StripDialogInfo       *sd;
  int                   i;
  intptr_t              which = (intptr_t)data;

  XtVaGetValues (w, XmNuserData, &sd, NULL);

  /* if a regular save is requested and we already have a filename,
   * we don't have to prompt the user. */
  if ((which == FSDLG_SAVE) && sd->config->filename) 
    {

      if(auto_scaleTriger==1) 
	Strip_auto_scale(sd->callback[SDCALLBACK_CLEAR].data);
    save_config (sd, sd->config->filename, SCFGMASK_ALL);
  }
  else
  {
    sd->fs.state = (FsDlgState)data;
    
    for (i = 0; i < FSDLG_TGL_COUNT; i++)
      XmToggleButtonSetState (sd->fs.tgl[i], True, False);
    
    XtManageChild (sd->fs.dlg);
  }
}



static void     windowmenu_cb   (Widget w, XtPointer data, XtPointer BOGUS(1))
{
  StripDialogInfo       *sd;
  intptr_t              i = (intptr_t)data;

  XtVaGetValues (w, XmNuserData, &sd, NULL);
  sd->window_menu_info.items[i].info.cb_func
    (sd->window_menu_info.items[i].info.cb_data,
     sd->window_menu_info.items[i].info.window_id);
}


static void helpmenu_cb (Widget w, XtPointer data, XtPointer BOGUS(1))
{		
  StripDialogInfo       *sd;
  intptr_t              i = (intptr_t)data;

  XtVaGetValues (w, XmNuserData, &sd, NULL);
  switch(i)
  {
  case HELP_WEBHELP:
    callBrowser(sd->display, stripHelpPath, "");
    break;

  case HELP_HELPHELP:
    XtVaGetValues (w, XmNuserData, &sd, NULL);
    MessageBox_popup (sd->shell, &sd->message_box, XmDIALOG_INFORMATION,
	"Help on Help", "OK",
#ifdef WIN32
	"     Web Help is implemented using your default browser\n"
	"or whatever program is in the environmental variable\n"
	"NETSCAPEPATH.\n"
	"     The URL is:\n"
	"%s\n"
	"     You can specify the URL in STRIP_HELP_PATH before\n"
	"starting Strip Tool.",
#else	  
	"     Web Help is implemented using Netscape or whatever\n"
	"program is in the environmental variable NETSCAPEPATH.\n"
	"     The URL is:\n"
	"%s\n"
	"     You can specify the URL in STRIP_HELP_PATH before\n"
	"starting Strip Tool.\n"
	"     If Netscape is running when MEDM first calls it,\n"
	"then the response should be fairly quick.  Otherwise,\n"
	"it may take a while.",
#endif	  
	stripHelpPath);
    break;

  case HELP_ABOUT:
    XtVaGetValues (w, XmNuserData, &sd, NULL);
    MessageBox_popup (sd->shell, &sd->message_box, XmDIALOG_INFORMATION,
	"About", "OK",
	"%s\n"
	"%s"
	"%s\n"
	"\nBuilt: %s %s\n"
	"\nAuthors:\n"
	"  Christopher Larrieu, TJNAF\n"
	"  Albert Kagarmanov, DESY\n"
	"  Janet Anderson, APS\n"
	"  Kenneth Evans, Jr., APS\n",
	STRIPTOOL_VERSION_STRING,
	EPICS_VERSION_STRING,
#if defined (USE_CDEV)
	"CDEV: "CDEV_VERSION_STRING"\n",
#else
	"",
#endif
	__DATE__,__TIME__);
    break;
  }
}


static void bogus_cb (Widget BOGUS(1), XtPointer BOGUS(2), XtPointer BOGUS(3))
{
  fprintf (stderr, "StripDialog: Bogus callback called!\n");
  fflush (stderr);
}


static void     wmdelete_cb     (Widget w, XtPointer data, XtPointer BOGUS(1))
{
  StripDialogInfo       *sd = (StripDialogInfo *)data;

  if (sd->callback[SDCALLBACK_DISMISS].func)
    sd->callback[SDCALLBACK_DISMISS].func
      (sd->callback[SDCALLBACK_DISMISS].data, 0);
}


static void     ctrl_btn_cb     (Widget w, XtPointer data, XtPointer BOGUS(1))
{
  StripDialogInfo       *sd;
  StripDialogCallback   which = (StripDialogCallback)data;

  XtVaGetValues (w, XmNuserData, &sd, NULL);
  if (sd->callback[which].func)
    sd->callback[which].func (sd->callback[which].data, 0);
}


static void     tabs_cb (Widget w, XtPointer data, XtPointer call)
{
  XgTabsCallbackStruct  *cbs = (XgTabsCallbackStruct *)call;
  StripDialogInfo       *sd = (StripDialogInfo *)data;

  XtUnmapWidget (sd->pages[cbs->old_tab]);
  XtMapWidget (sd->pages[cbs->tab]);
  sd->current_page = sd->pages[cbs->tab];
  cbs->doit = True;
}


#if 0
/* KE: unused */
static void     dismiss_btn_cb  (Widget         BOGUS(1),
                                 XtPointer      data,
                                 XtPointer      BOGUS(2))
{
  StripDialogInfo       *sd = (StripDialogInfo *)data;

  StripDialog_popdown (sd);
}
#endif

static void     fsdlg_cb        (Widget w, XtPointer data, XtPointer call)
{
  XmFileSelectionBoxCallbackStruct      *cbs;
  StripDialogInfo                       *sd;
  intptr_t                              mode = (intptr_t)data;
  char                                  *fname;
  int                                   i;

  XtUnmanageChild (w);
  cbs = (XmFileSelectionBoxCallbackStruct *)call;
  XtVaGetValues (w, XmNuserData, &sd, NULL);

  if (mode == FSDLG_OK)
  {
    if (XmStringGetLtoR (cbs->value, XmFONTLIST_DEFAULT_TAG, &fname))
    {
      if (fname)
	{
	  if(*fname)
	  {
	    /* get the attribute mask */
	    StripConfigMask_clear (&sd->fs.mask);
	    for (i = 0; i < FSDLG_TGL_COUNT; i++)
		if (XmToggleButtonGetState (sd->fs.tgl[i]))
		  StripConfigMask_or (&sd->fs.mask, FsDlgTglVal[i]);
	    
	    /* do save or load */
	    if (sd->fs.state == FSDLG_LOAD)
	    {
		sd->callback[SDCALLBACK_CLEAR].func(sd->callback[SDCALLBACK_CLEAR].data,NULL);
		load_config (sd, fname, sd->fs.mask);
		Strip_refresh(sd->callback[SDCALLBACK_CLEAR].data); /* perror Albert */
		/*GraphLoadRefresh(sd->callback[SDCALLBACK_CLEAR].data);*/
	    }
	    else
	    {
		if(auto_scaleTriger==1) 
		  Strip_auto_scale(sd->callback[SDCALLBACK_CLEAR].data);
		save_config (sd, fname, sd->fs.mask);
	    }
	  }
	  XtFree(fname);
	}
    }
  }
}


static void     clrdlg_cb       (void *arg, cColor *pcolor_new)
{
  StripDialogInfo       *sd = (StripDialogInfo *)arg;
  cColor                *pcolor_old;
  StripConfigMask       mask;

  StripConfig_getattr
    (sd->config, (StripConfigAttribute)sd->color_edit_which, &pcolor_old, 0);
  
  /* if the referenced color cells differ, then we need to
   * flag an update, otherwise we can just assign the
   * new value to the old */
  if (pcolor_new->xcolor.pixel != pcolor_old->xcolor.pixel)
  {
    /* keep new color, free old */
    cColorManager_keep_color (sd->config->scm, pcolor_new);
    cColorManager_free_color (sd->config->scm, pcolor_old);

    /* set the new color */
    StripConfig_setattr
      (sd->config, (StripConfigAttribute)sd->color_edit_which, pcolor_new, 0);

    /* initiate update */
    StripConfigMask_clear (&mask);
    StripConfigMask_set (&mask, (StripConfigMaskElement)sd->color_edit_which);
    StripConfig_update (sd->config, mask);
  }
  else *pcolor_old = *pcolor_new;
}

static void     StripDialog_cfgupdate   (StripConfigMask mask, void *data)
{
  StripDialogInfo       *sd = (StripDialogInfo *)data;
  StripCurveInfo        *sc;
  int                   i, j;
  int                   h, m, s;

  if (StripConfigMask_stat (&mask, SCFGMASK_TIME_TIMESPAN))
  {
    sec2hms (sd->config->Time.timespan, &h, &m, &s);
    setwidgetval_tm_tshour (sd, h);
    setwidgetval_tm_tsminute (sd, m);
    setwidgetval_tm_tssecond (sd, s);
  }

  if (StripConfigMask_stat (&mask, SCFGMASK_TIME_NUM_SAMPLES))
  {
    setwidgetval_tm_numsamples (sd, sd->config->Time.num_samples);
  }
  
  if (StripConfigMask_stat (&mask, SCFGMASK_TIME_SAMPLE_INTERVAL))
  {
    setwidgetval_tm_ds (sd, sd->config->Time.sample_interval);
  }
  
  if (StripConfigMask_stat (&mask, SCFGMASK_TIME_REFRESH_INTERVAL))
  {
    setwidgetval_tm_gr (sd, sd->config->Time.refresh_interval);
  }

  /* colors */
  if (StripConfigMask_intersect (&mask, &SCFGMASK_COLOR))
  {
    if (StripConfigMask_stat (&mask, SCFGMASK_COLOR_BACKGROUND))
    {
      setwidgetval_gr_bg (sd, sd->config->Color.background.xcolor.pixel);
    }
    if (StripConfigMask_stat (&mask, SCFGMASK_COLOR_FOREGROUND))
    {
      setwidgetval_gr_fg (sd, sd->config->Color.foreground.xcolor.pixel);
    }
    if (StripConfigMask_stat (&mask, SCFGMASK_COLOR_GRID))
    {
      setwidgetval_gr_gridclr (sd, sd->config->Color.grid.xcolor.pixel);
    }

    /* find the curve whose color corresponds with the config color index */
    for (i = 0; i < STRIP_MAX_CURVES; i++)
      if (StripConfigMask_stat
          (&mask, (StripConfigMaskElement)SCFGMASK_COLOR_COLOR1+i))
        for (j = 0; j < sd->sdcurve_count; j++)
        {
          sc = (StripCurveInfo *)sd->curve_info[j].curve;
          if (sc->details->color == &sd->config->Color.color[i])
            setwidgetval_color (sd, j, sc->details->color->xcolor.pixel);
        }
  }
  
  if (StripConfigMask_stat (&mask, SCFGMASK_OPTION_GRID_XON))
  {
    setwidgetval_gr_gridx (sd, sd->config->Option.grid_xon);
  }
  
  if (StripConfigMask_stat (&mask, SCFGMASK_OPTION_GRID_YON))
  {
    setwidgetval_gr_gridy (sd, sd->config->Option.grid_yon);
  }
  
  if (StripConfigMask_stat (&mask, SCFGMASK_OPTION_AXIS_YCOLORSTAT))
  {
    setwidgetval_gr_yaxisclr (sd, sd->config->Option.axis_ycolorstat);
  }

  if (StripConfigMask_stat (&mask, SCFGMASK_OPTION_GRAPH_LINEWIDTH))
  {
    setwidgetval_gr_linewidth (sd, sd->config->Option.graph_linewidth);
  }

  if (StripConfigMask_intersect (&mask, &SCFGMASK_CURVE))
  {
    for (i = 0; i < sd->sdcurve_count; i++)
    {
      sc = (StripCurveInfo *)sd->curve_info[i].curve;
      if (StripConfigMask_intersect (&mask, &sc->details->update_mask))
      {
        if (StripConfigMask_stat
            (&sc->details->update_mask, SCFGMASK_CURVE_EGU))
        {
        }
            
        if (StripConfigMask_stat
            (&sc->details->update_mask, SCFGMASK_CURVE_PRECISION))
        {
          setwidgetval_precision (sd, i, sc->details->precision);
          if (!(StripConfigMask_stat
                (&sc->details->update_mask, SCFGMASK_CURVE_MIN)))
            setwidgetval_min (sd, i,  sc->details->min);
          if (!(StripConfigMask_stat
                (&sc->details->update_mask, SCFGMASK_CURVE_MAX)))
            setwidgetval_max (sd, i,  sc->details->max);
        }
            
        if (StripConfigMask_stat (&mask, SCFGMASK_CURVE_MIN))
        {
          setwidgetval_min (sd, i,  sc->details->min);
        }
            
        if (StripConfigMask_stat (&mask, SCFGMASK_CURVE_MAX))
        {
          setwidgetval_max (sd, i,  sc->details->max);
        }
            
        if (StripConfigMask_stat (&mask, SCFGMASK_CURVE_PLOTSTAT))
        {
          setwidgetval_plotstat (sd, i, sc->details->plotstat);
        }
            
        if (StripConfigMask_stat (&mask, SCFGMASK_CURVE_SCALE))
        {
          setwidgetval_scale (sd, i, sc->details->scale);
        }
      }
    }
  }
}


static void     save_config     (StripDialogInfo        *sd,
                                 char                   *fname,
                                 StripConfigMask        mask)
{
  FILE  *f;

  if ((f = fopen (fname, "w")))
  {
    if (StripConfig_write (sd->config, f, mask))
    {
      StripConfig_setattr (sd->config, STRIPCONFIG_FILENAME, fname, 0);
      StripConfig_setattr (sd->config, STRIPCONFIG_TITLE, basename (fname), 0);
      StripConfigMask_clear (&mask);
      StripConfigMask_set (&mask, SCFGMASK_FILENAME);
      StripConfigMask_set (&mask, SCFGMASK_TITLE);
      StripConfig_update (sd->config, mask);
    }
    else
      MessageBox_popup
        (sd->shell, &sd->message_box, XmDIALOG_ERROR, "File I/O", "OK",
         "Unable to save configuration");

    fclose (f);
  }
  else
    MessageBox_popup
      (sd->shell, &sd->message_box, XmDIALOG_ERROR, "File I/O", "OK",
       "Unable to open file for writing.\nname: %s\nerror: %s",
       fname, strerror (errno));
}


static void     load_config     (StripDialogInfo        *sd,
                                 char                   *fname,
                                 StripConfigMask        mask)
{
  FILE  *f;

  if ((f = fopen (fname, "r")))
  {
    if (StripConfig_load (sd->config, f, mask))
    {
      StripConfig_setattr (sd->config, STRIPCONFIG_FILENAME, fname, 0);
      StripConfig_setattr (sd->config, STRIPCONFIG_TITLE, basename (fname), 0);
      StripConfigMask_set (&mask, SCFGMASK_FILENAME);
      StripConfigMask_set (&mask, SCFGMASK_TITLE);
      StripConfig_update (sd->config, mask);
    }
    else
      MessageBox_popup
        (sd->shell, &sd->message_box, XmDIALOG_ERROR, "File I/O", "OK",
         "Unable to load configuration");

    fclose (f);
  }
  else
    MessageBox_popup
      (sd->shell, &sd->message_box, XmDIALOG_ERROR, "File I/O", "OK",
       "Unable to open file for reading.\nname: %s\nerror: %s",
       fname, strerror (errno));
}


int countCurve(void *the_dialog)  /* Albert */
{
  StripDialogInfo *sd = (StripDialogInfo *)the_dialog;
  return(sd->sdcurve_count);
}

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
