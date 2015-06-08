#ifndef __HVGATEWAYH__
#define __HVGATEWAYH__

/**
 * <EM>Via "ifdef WIN32", this selectively includes and define each Windows or X/Motif
 * header file, typedef, etc. It also holds the prototypes for the gateway.</EM>
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
 * to <a href="mailto:heddle@cebaf.gov">heddle@cebaf.gov</a>  <HR>
 */


#ifdef Hv_USEOPENGL
#ifndef WIN32
#include <Xm/MainW.h>
#include <GL/glx.h>
#else
#endif
#endif 


/*
 * NOTE: In the block of comments below, the Win32 values
 * have been set arbitrarily, while the Xm vals are from motif macros
 */


#ifdef WIN32
#include "Hv_linklist.h"
#define  MAXPATHLEN            _MAX_PATH

#define  getcwd                _getcwd

#define Hv_INPUTCALLBACK                    1
#define Hv_ACTIVATECALLBACK                 2
#define Hv_VALUECHANGEDCALLBACK             3
#define Hv_OKCALLBACK                       4
#define Hv_CANCELCALLBACK                   5
#define Hv_MULTIPLESELECTIONCALLBACK        6
#define Hv_SINGLESELECTIONCALLBACK          7
#define Hv_THINMENUCALLBACK                 8


#define  Hv_NOEXPOSE           0
#define  Hv_EXPOSE             1
#define  Hv_GRAPHICSEXPOSE     2

#define  Hv_motionNotify       0
#define  Hv_buttonRelease      1
#define  Hv_buttonPress        2

#define  Hv_PIXMAP             1

#define  Hv_FillSolid          0
#define  Hv_FillTiled          1
#define  Hv_FillStippled       2

#define  Hv_buttonPressMask    01
#define  Hv_buttonReleaseMask  02


#define Hv_FormClass               920
#define Hv_LabelClass              921
#define Hv_PushButtonClass         922
#define Hv_RowColumnClass          923
#define Hv_ScaleClass              924
#define Hv_SeparatorClass          925
#define Hv_TextFieldClass          926
#define Hv_ToggleButtonClass       927

#define Hv_IMTimer                 01
#define Hv_IMAll                   0xFFFF

#define Hv_N_OF_MANY               30001
#define Hv_Naccelerator            30002
#define Hv_NacceleratorText        30003
#define Hv_NautoShowCursorPosition 30004
#define Hv_NautoUnmanage           30005
#define Hv_Nbackground             30006
#define Hv_NblinkRate              30007
#define Hv_NborderColor            30008
#define Hv_NborderWidth            30009
#define Hv_NbottomAttachment       30010
#define Hv_NcancelLabelString      30011
#define Hv_Nchildren               30012
#define Hv_Ncolormap               30013
#define Hv_Ncolumns                30014
#define Hv_NcursorPosition         30015
#define Hv_NcursorPositionVisible  30016
#define Hv_NdefaultButtonShadowThickness    30017
#define Hv_NdeleteResponse         30018
#define Hv_NdialogStyle            30019
#define Hv_NdialogTitle            30020
#define Hv_Ndirectory              30021
#define Hv_NeditMode               30022
#define Hv_Neditable               30023
#define Hv_NentryAlignment         30024
#define Hv_NfileTypeMask           30025
#define Hv_NfontList               30026
#define Hv_Nforeground             30027
#define Hv_NfractionBase           30028
#define Hv_Nheight                 30029
#define Hv_NhighlightThickness     30030
#define Hv_NindicatorType          30031
#define Hv_NindicatorSize          29999
#define Hv_NisHomogeneous          30032
#define Hv_NitemCount              30033
#define Hv_Nitems                  30034
#define Hv_NlabelPixmap            30035
#define Hv_NlabelString            30036
#define Hv_NlabelType              30037
#define Hv_NlabelTypeXmPIXMAPNULL  30038
#define Hv_NleftAttachment         30039
#define Hv_NleftPosition           30040
#define Hv_NlistSizePolicy         30041
#define Hv_NmarginHeight           30042
#define Hv_NmarginWidth            30043
#define Hv_Nmaximum                30044
#define Hv_NmenuHelpWidget         30045
#define Hv_NmenuHistory            30046
#define Hv_NmessageString          30047
#define Hv_Nminimum                30048
#define Hv_NnoResize               30049
#define Hv_NnumChildren            30050
#define Hv_NnumColumns             30051
#define Hv_NokLabelString          30052
#define Hv_Norientation            30053
#define Hv_Npacking                30054
#define Hv_Npattern                30055
#define Hv_NresizePolicy           30056
#define Hv_NrightAttachment        30057
#define Hv_NrightPosition          30058
#define Hv_Nrows                   30059
#define Hv_NscrollBarDisplayPolicy 30060
#define Hv_NscrollHorizontal       30061
#define Hv_NselectionPolicy        30062
#define Hv_Nsensitive              30063
#define Hv_Nset                    30064
#define Hv_NshadowThickness        30065
#define Hv_NshowAsDefault          30066
#define Hv_NshowValue              30067
#define Hv_Nspacing                30068
#define Hv_NsubMenuId              30069
#define Hv_NtextString             30070
#define Hv_Ntitle                  30071
#define Hv_NtopAttachment          30072
#define Hv_Nvalue                  30073
#define Hv_NvisibleItemCount       30074
#define Hv_NvisibleWhenOff         30075
#define Hv_NwhichButton            30076
#define Hv_Nwidth                  30077
#define Hv_NwordWrap               30078
#define Hv_Nx                      30079
#define Hv_Ny                      30080

#define Hv_NtextFontList           30081
#define Hv_NbuttonFontList         30082
#define Hv_NlabelFontList          30083
#define Hv_NselectedItems          30084
#define Hv_NselectedItemCount      30085
#define CVSCHKFORSTEVE             SPARTALAPTOP
#define Hv_FILE_REGULAR            30087
#define HvNselectedItems           30088
/* list selection */

#define Hv_SINGLELISTSELECT     0
#define Hv_MULTIPLELISTSELECT   1

/* list resize */

#define Hv_VARIABLE            0
#define Hv_CONSTANT            1
#define Hv_RESIZE_IF_POSSIBLE  2

/* for setting the drawing "function" */

#define Hv_GXCOPY            R2_COPYPEN
#define Hv_GXXOR             R2_XORPEN

/* for row column widgets */

#define Hv_BEGINNING          0
#define Hv_CENTER             1
#define Hv_END                2

#define Hv_TIGHT              0
#define Hv_PAD                1

#define Hv_VERTICAL           0
#define Hv_HORIZONTAL         1

#define Hv_ONEOFMANY            0

#define Hv_VaCreateManagedWidget Hv_W32VaCreateManagedWidget
#define Hv_VaCreateWidget    Hv_W32VaCreateWidget
#define Hv_VaGetValues       Hv_W32VaGetValues
#define Hv_VaSetValues       Hv_W32VaSetValues

typedef VOID     (*Hv_CallbackProc)();
typedef struct hvW32widgetinfo
{
	HWND            msdialog;          /* handle for the windows dialog box */
	HWND            mscontrol;         /* handle for the windows control */

	short           wtype;             /* widget type (dialog, control...) */
	short           wsubtype;          /* widget subtype (pattern, submenu ...) */
	short           wfont;             /* default widget font */
	short           wcallback_id;      /* callback for the widget */
	LPWSTR          wlabel;            /* UNICODE version of the title/label */
    PVOID           wdata; 
	Hv_LinkList     *wchildren;        /* list of all child widgets*/
	int             num_children;      /* for convience */
    struct hvW32widgetinfo  *parent;   /* owning widget */
	short           wfgColor;          /* Forground Color */
	short           wbgColor;          /* Background Color */
	DWORD           wstyle;            /* win32 style bits */
	DWORD           wstyleex;          /* win32 extended style bits */
	DWORD           wnstyle;           /* win32 style bits that are to be turned off */
	DWORD           wnstyleex;         /* win32 extended style bits thatare to be turned off*/
    short           disabled;          /* true if widget doesnt go on the dialog box */
//	union
//	{
	struct {
      LPDLGTEMPLATE   dialog;          /* pointer to the W32 DLGTEMPLATE structure*/
      int             num_controls;    /* number of controls associated with the dialog*/
      int             num_entries;    /* number of entries associated with the dialog */
      short           modal;           /* 1= modal 2 = modeless        */
	  int             ignoreclose;     /* ??? saved from the function call */
	  short           dspacing;        /* space between next row/column */
      short           dorientation;    /* horizontal or vertical */
      short           dpacking;        /* tight or loose */
      short           dalignment;      /* aligment (left ro right) */
      short           dnrc;            /* number of rows/columns */
      short           dncol;           /* number of columns */
	};
	struct {
	  LPDLGITEMTEMPLATE   item;        /* pointer to the W32 DLGITEMTEMPLATE structure*/
	  short               itype;       /* item type   */
	  short               iheight;     /* item height */
	  short               iwidth;      /* item width  */
	  short               ix;          /* x position  */
	  short               iy;          /* y position  */
	  short               set_ix;      /* user set x position  */
	  short               set_iy;      /* user set y position  */

	  short               def_iheight; /* item default height */
	  short               def_iwidth;  /* item default width  */
	  short               set_iheight; /* user set height */
	  short               set_iwidth;  /* user set width  */

	};
//	};
	short             wstate;          /* button state (for button widgets) 0 = off 1 = on */
    Hv_LinkList      *wdataList;       /* pointer to the widgets data (List Widget only) */
	short             wdataNitems;     /* number of entries in the wdatalist */
	PVOID             generic;         /* chatch all */
	short             wisWisable;      /* true if the widget is being displayed in an open dialog box*/
	char             *wtextString;     /* single item text string */
	short             wdisabled;       /* enabled/disabled status */
	Hv_LinkList      *wcallList;       /* pointer to the widgets callback structures*/
	short             wcallCount;      /* number of items in the wcallList */
	Hv_LinkList      *wgarbageList;    /* list of system garbage to free when the widget dies*/
	short             wgarbageCount;   /* number of items in the wgarbageList */

} Hv_WidgetInfo;

typedef struct hvw32callbackdata
{
	int             clientData;
	PVOID          *clientPointer;
	Hv_CallbackProc callback;
	char           *callbackType;
	int             icallbackType;
} Hv_callbackData;

#else  /* X windows defs */

#define  Hv_NOEXPOSE           NoExpose
#define  Hv_EXPOSE             Expose
#define  Hv_GRAPHICSEXPOSE     GraphicsExpose

#define  Hv_motionNotify       MotionNotify
#define  Hv_buttonRelease      ButtonRelease
#define  Hv_buttonPress        ButtonPress

#define  Hv_PIXMAP             XmPIXMAP

#define  Hv_FillSolid          FillSolid
#define  Hv_FillTiled          FillTiled
#define  Hv_FillStippled       FillStippled

#define  Hv_buttonPressMask    ButtonPressMask
#define  Hv_buttonReleaseMask  ButtonReleaseMask


#ifdef Hv_USEOPENGL
extern GLXContext       Hv_glxContext;
extern XVisualInfo     *Hv_3Dvi;
extern int              Hv_dblBuf[];
extern int             *Hv_snglBuf;
#endif

#define Hv_FormClass               xmFormWidgetClass
#define Hv_LabelClass              xmLabelWidgetClass
#define Hv_PushButtonClass         xmPushButtonGadgetClass
#define Hv_RowColumnClass          xmRowColumnWidgetClass
#define Hv_ScaleClass              xmScaleWidgetClass
#define Hv_SeparatorClass          xmSeparatorGadgetClass
#define Hv_TextFieldClass          xmTextFieldWidgetClass
#define Hv_ToggleButtonClass       xmToggleButtonGadgetClass

#define Hv_IMTimer                 XtIMTimer
#define Hv_IMAll                   XtIMAll

#define Hv_N_OF_MANY               XmN_OF_MANY
#define Hv_Naccelerator            XmNaccelerator
#define Hv_NacceleratorText        XmNacceleratorText
#define Hv_NautoShowCursorPosition XmNautoShowCursorPosition
#define Hv_NautoUnmanage           XmNautoUnmanage
#define Hv_Nbackground             XmNbackground
#define Hv_NblinkRate              XmNblinkRate
#define Hv_NborderColor            XmNborderColor
#define Hv_NborderWidth            XmNborderWidth
#define Hv_NbottomAttachment       XmNbottomAttachment
#define Hv_NcancelLabelString      XmNcancelLabelString
#define Hv_Nchildren               XmNchildren
#define Hv_Ncolormap               XmNcolormap
#define Hv_Ncolumns                XmNcolumns
#define Hv_NcursorPosition         XmNcursorPosition
#define Hv_NcursorPositionVisible  XmNcursorPositionVisible
#define Hv_NdefaultButtonShadowThickness    XmNdefaultButtonShadowThickness
#define Hv_NdeleteResponse         XmNdeleteResponse
#define Hv_NdialogStyle            XmNdialogStyle
#define Hv_NdialogTitle            XmNdialogTitle
#define Hv_Ndirectory              XmNdirectory
#define Hv_NeditMode               XmNeditMode
#define Hv_Neditable               XmNeditable
#define Hv_NentryAlignment         XmNentryAlignment
#define Hv_NfileTypeMask           XmNfileTypeMask
#define Hv_NfontList               XmNfontList
#define Hv_Nforeground             XmNforeground
#define Hv_NfractionBase           XmNfractionBase
#define Hv_Nheight                 XmNheight
#define Hv_NhighlightThickness     XmNhighlightThickness
#define Hv_NindicatorType          XmNindicatorType
#define Hv_NindicatorSize          XmNindicatorSize
#define Hv_NisHomogeneous          XmNisHomogeneous
#define Hv_NitemCount              XmNitemCount
#define Hv_Nitems                  XmNitems
#define Hv_NlabelPixmap            XmNlabelPixmap
#define Hv_NlabelString            XmNlabelString
#define Hv_NlabelType              XmNlabelType
#define Hv_NlabelTypeXmPIXMAPNULL  XmNlabelTypeXmPIXMAPNULL
#define Hv_NleftAttachment         XmNleftAttachment
#define Hv_NleftPosition           XmNleftPosition
#define Hv_NlistSizePolicy         XmNlistSizePolicy
#define Hv_NmarginHeight           XmNmarginHeight
#define Hv_NmarginWidth            XmNmarginWidth
#define Hv_Nmaximum                XmNmaximum
#define Hv_NmenuHelpWidget         XmNmenuHelpWidget
#define Hv_NmenuHistory            XmNmenuHistory
#define Hv_NmessageString          XmNmessageString
#define Hv_Nminimum                XmNminimum
#define Hv_NnoResize               XmNnoResize
#define Hv_NnumChildren            XmNnumChildren
#define Hv_NnumColumns             XmNnumColumns
#define Hv_NokLabelString          XmNokLabelString
#define Hv_Norientation            XmNorientation
#define Hv_Npacking                XmNpacking
#define Hv_Npattern                XmNpattern
#define Hv_NresizePolicy           XmNresizePolicy
#define Hv_NrightAttachment        XmNrightAttachment
#define Hv_NrightPosition          XmNrightPosition
#define Hv_Nrows                   XmNrows
#define Hv_NscrollBarDisplayPolicy XmNscrollBarDisplayPolicy
#define Hv_NscrollHorizontal       XmNscrollHorizontal
#define Hv_NselectionPolicy        XmNselectionPolicy
#define Hv_Nsensitive              XmNsensitive
#define Hv_Nset                    XmNset
#define Hv_NshadowThickness        XmNshadowThickness
#define Hv_NshowAsDefault          XmNshowAsDefault
#define Hv_NshowValue              XmNshowValue
#define Hv_Nspacing                XmNspacing
#define Hv_NsubMenuId              XmNsubMenuId
#define Hv_NtextString             XmNtextString
#define Hv_Ntitle                  XtNtitle
#define Hv_NtopAttachment          XmNtopAttachment
#define Hv_Nvalue                  XmNvalue
#define Hv_NvisibleItemCount       XmNvisibleItemCount
#define Hv_NvisibleWhenOff         XmNvisibleWhenOff
#define Hv_NwhichButton            XmNwhichButton
#define Hv_Nwidth                  XmNwidth
#define Hv_NwordWrap               XmNwordWrap
#define Hv_Nx                      XmNx
#define Hv_Ny                      XmNy
#define Hv_NselectedItems          XmNselectedItems
#define Hv_NselectedItemCount      XmNselectedItemCount
#define Hv_FILE_REGULAR            XmFILE_REGULAR

#define Hv_NtextFontList           XmNtextFontList
#define Hv_NbuttonFontList         XmNbuttonFontList
#define Hv_NlabelFontList          XmNlabelFontList
#define Hv_NselectedItems          XmNselectedItems
#define Hv_NselectedItemCount      XmNselectedItemCount

/* list selection */

#define Hv_SINGLELISTSELECT     XmSINGLE_SELECT
#define Hv_MULTIPLELISTSELECT   XmMULTIPLE_SELECT

/* list resize */

#define Hv_VARIABLE            XmVARIABLE
#define Hv_CONSTANT            XmCONSTANT
#define Hv_RESIZE_IF_POSSIBLE  XmRESIZE_IF_POSSIBLE

/* for setting the drawing "function" */

/*#define Hv_GXCOPY              GXcopy
  #define Hv_GXXOR               GXxor */

#define Hv_GXCOPY            0x3
#define Hv_GXXOR             0x6

/* for row column widgets */

#define Hv_BEGINNING          XmALIGNMENT_BEGINNING   /* default */
#define Hv_CENTER             XmALIGNMENT_CENTER
#define Hv_END                XmALIGNMENT_END

#define Hv_TIGHT              XmPACK_TIGHT
#define Hv_PAD                XmPACK_COLUMN

#define Hv_VERTICAL           XmVERTICAL
#define Hv_HORIZONTAL         XmHORIZONTAL

#define Hv_ONEOFMANY          (short)XmONE_OF_MANY

#define Hv_VaCreateManagedWidget XtVaCreateManagedWidget
#define Hv_VaCreateWidget    XtVaCreateWidget
#define Hv_VaGetValues       XtVaGetValues
#define Hv_VaSetValues       XtVaSetValues

#endif



#ifdef WIN32

/* create synonyms and definitions for Win32 datatypes */

#define  ShiftMask        01
#define  Mod1Mask         02
#define  ControlMask      04

#define  Hv_MAXWIDGETSINLIST  50
#define  True           TRUE
#define  False          FALSE

/* HV_Opengl types */

#define ButtonPress 1
#define MotionNotify 2
#define ButtonRelease 3
#define Button1MotionMask 1
#define Button2MotionMask 2


typedef  RGBQUAD        Hv_Color;
typedef  HPALETTE       Hv_ColorMap;
typedef  BOOL           Boolean;


// in windows, give compound strings the same definition
// as Hv_Strings

typedef  struct  hvcompoundstring *Hv_CompoundString;

typedef  struct  hvcompoundstring {
    short         font;		/* font */
    short         strcol;         /* string color (the text) */
    short         fillcol;        /* fillcolor */
    short         offset;         /* for "scrolling" */
    char          *text;		/* for items using text */
    Boolean       horizontal;	/* orientation flag */
}  Hv_CompoundStringData;

typedef  Hv_CompoundString  *Hv_StringTable;
typedef  PVOID          Hv_VisualInfo;
typedef  PVOID          Hv_Visual;
typedef  PVOID          Hv_Display;
typedef  HCURSOR        Hv_Cursor;
typedef  HWND           Hv_Window;

typedef  SHORT          Hv_Dimension;

typedef  HDC            Hv_GC;
typedef  HINSTANCE      Hv_AppContext;

typedef struct hvxevent {
	INT      type;
	int      modifiers;
	int      x;
	int      y;
	int      button;
} Hv_XEvent;

typedef struct hvopenglbutton {
	short button;
	short state;
} Hv_openglButton;

typedef struct hvopenglmotion {
	short x;
	short y;
} Hv_openglMotion;

typedef struct hvopenglevent {
	short           type;
	Hv_openglButton xbutton;
	Hv_openglMotion xmotion;
	short x;
	short y;
} Hv_openglEvent;

typedef struct hvopenglcalldata {
	Hv_openglEvent *event;
	int      modifiers;
	int      x;
	int      y;
	int      button;
} Hv_openglCallData;

typedef  HRGN           Hv_Region;

typedef  Hv_WidgetInfo *Hv_Widget;
typedef  PSTR           Hv_WidgetClass;


typedef  PVOID          Hv_Pointer;
typedef  HWND           Hv_Pixmap;
typedef  INT            Hv_TextPosition;

typedef  LONG           Hv_IntervalId;
typedef  VOID         (*Hv_TimerCallbackProc)();

typedef  VOID         (*Hv_EventHandler)();

typedef  PVOID           DIR;

typedef  struct  hvarg *Hv_ArgList;

typedef struct hvarg {
	PSTR     name;
	INT      ival;
	PVOID    pval;
} Hv_Arg;


typedef struct hvpoint {
	SHORT   x;
	SHORT   y;
} Hv_Point;

typedef struct hvrectangle {
	SHORT   x;
	SHORT   y;
	USHORT   width;
	USHORT   height;
} Hv_Rectangle;

typedef struct hvsegment {
	SHORT   x1;
	SHORT   y1;
	SHORT   x2;
	SHORT   y2;
} Hv_Segment;

typedef   Hv_Widget   Hv_WidgetList[Hv_MAXWIDGETSINLIST];

typedef  PVOID          Hv_Image;
typedef  HFONT          Hv_Font;
typedef  PVOID          Hv_FontList;


typedef VOID     (*Hv_CanvasCallback)();


/* callbac rec used in Win32 only */

typedef  struct  widgetrec *Hv_CallbackData;

typedef struct widgetrec {
	VOID              (*callback)();
	Hv_Widget         parent;
	Hv_Widget         w;
	int               win32Id; /*unique*/
	long              userId;  /*not nec unique*/
	short             type;
} Hv_CallbackRec;

#else  /*   X and Motif section */

/* create synonyms to some X, Xt & Xm datatypes */

typedef  XColor         Hv_Color;
typedef  Colormap       Hv_ColorMap;
typedef  XmString       Hv_CompoundString;
typedef  XmStringTable  *Hv_StringTable;
typedef  XVisualInfo    Hv_VisualInfo;
typedef  Visual         Hv_Visual;
typedef  Display        Hv_Display;
typedef  Window         Hv_Window;
typedef  Cursor         Hv_Cursor;
typedef  Dimension      Hv_Dimension;

typedef  GC             Hv_GC;
typedef  XtAppContext   Hv_AppContext;

typedef  XEvent         Hv_XEvent;
typedef  Region         Hv_Region;
typedef  XPoint         Hv_Point;
typedef  XSegment       Hv_Segment;
typedef  XRectangle     Hv_Rectangle;
typedef  Widget         Hv_Widget;
typedef  WidgetClass    Hv_WidgetClass;

/*typedef  XtPointer      caddr_t;*/
typedef  XtPointer      Hv_Pointer;
typedef  Pixmap         Hv_Pixmap;
typedef  XmTextPosition Hv_TextPosition;

typedef  XtIntervalId   Hv_IntervalId;
typedef  XtTimerCallbackProc Hv_TimerCallbackProc;
typedef  XtEventHandler Hv_EventHandler;

typedef  Arg            Hv_Arg;
typedef  ArgList        Hv_ArgList;
typedef  XtArgVal       Hv_ArgVal;

typedef  WidgetList     Hv_WidgetList;
typedef  XImage         Hv_Image;
typedef  XFontStruct    Hv_Font;
typedef  XmFontList     Hv_FontList;
typedef  XmDrawingAreaCallbackStruct Hv_CanvasCallback;

typedef  XtCallbackProc Hv_CallbackProc;

#endif

#include "Hv_data.h"       /* data structure defs */

extern unsigned char pat0[];
extern unsigned char pat1[];
extern unsigned char pat2[];
extern unsigned char pat3[];
extern unsigned char pat4[];
extern unsigned char pat5[];
extern unsigned char pat6[];
extern unsigned char pat7[];
extern unsigned char pat8[];
extern unsigned char pat9[];
extern unsigned char pat10[];
extern unsigned char pat11[];

#ifdef WIN32
#include "Hv_gateway_w32.h"
#else
#include "Hv_gateway_xm.h"
#endif


#define Hv_SetArg(arg, n, d) ((void)( (arg).name = (n), (arg).value = (Hv_ArgVal)(d) ))

extern void           Hv_DrawWorldImageItem(Hv_Item    Item,
					   Hv_Region   region);

extern Hv_Pixmap       Hv_DrawItemOnPixmap(Hv_Item   Item,
					   Hv_Region region);

extern void            Hv_SaveViewBackground(Hv_View   View,
					     Hv_Region  region);


extern Hv_Pixmap       Hv_SaveUnder(short x,
				                    short y,
				                    unsigned int  w,
				                    unsigned int  h);

extern void            Hv_RestoreSaveUnder(Hv_Pixmap pmap,
					                       short x,
					                       short y,
					                       unsigned int  w,
					                       unsigned int  h);

extern void            Hv_UpdateItemOffScreen(Hv_Item Item,
					                          Hv_Region region);

extern void            Hv_OffScreenViewControlUpdate(Hv_View    View,
						                             Hv_Region  region);

extern void            Hv_RestoreViewBackground(Hv_View  View);


extern void            Hv_OffScreenViewUpdate(Hv_View    View,
					                          Hv_Region  region);

extern void           Hv_ScrollView(Hv_View   View,
			                        int     ScrollPart,
			                        Hv_Region    region,
			                        Hv_Point   StartPP,
			                        Hv_Point   StopPP);

extern void            Hv_ShutDown();

extern void            Hv_Go(void);

extern void            Hv_ProcessEvent(int mask);

extern void            Hv_AddEventHandler(Hv_Widget       w,
					  int             mask,
					  Hv_EventHandler handler,
					  Hv_Pointer      data);

extern void            Hv_RemoveTimeOut(Hv_IntervalId id);

extern Hv_IntervalId   Hv_AddTimeOut(unsigned long          interval,
				     Hv_TimerCallbackProc   callback,
				     Hv_Pointer             data);

extern Boolean         Hv_CheckMaskEvent(int        mask,
					 Hv_XEvent *event);

extern int             Hv_Initialize(unsigned int,
				     char **,
				     char *);
     
extern  void           Hv_SystemSpecificInit();

extern   Boolean       Hv_IsTopWindow();

extern  void           Hv_InitWindow(void);

extern  void           Hv_CreateAppContext();

extern  void           Hv_CreateGraphicsContext(void);

extern  void           Hv_InitCursors();

extern void            Hv_InitPatterns(void);

extern  void           Hv_InitGraphics(void);

extern  void           Hv_InitColors(void);

extern  void           Hv_InitCanvas(void);

extern  void           Hv_InitGlobalWidgets(void);

extern Hv_Widget       Hv_CreateMainMenuBar(Hv_Widget parent);

extern Hv_Widget       Hv_CreatePopup(Hv_Widget  parent);

extern Hv_Widget       Hv_CreateSubmenu(Hv_Widget  parent);

extern void            Hv_Popup1Callback(Hv_Widget          widget,
					 Hv_Widget          popup,
					 Hv_CanvasCallback *cbs);

extern void            Hv_Popup2Callback(Hv_Widget          widget,
					 Hv_Widget          popup,
					 Hv_CanvasCallback *cbs);

extern void            Hv_CreateUserMenus(void);

extern void            Hv_SetMenuItemString(Hv_Widget menuitem,
					    char     *str,
					    short     font);

extern Hv_Widget       Hv_CreateHelpPullDown(void);

extern Hv_Widget       Hv_CreatePullDown(char *title);

extern Hv_Widget       Hv_AddMenuSeparator(Hv_Widget	menu);

extern Hv_Widget       Hv_AddSubMenuItem(char *label,
				    Hv_Widget   menu,
				    Hv_Widget	submenu);

extern Hv_Widget       Hv_AddMenuToggle(char *label,
				   Hv_Widget	   menu,
				   long            ID,
				   Hv_CallbackProc callback,
				   Boolean	   state,
				   unsigned char   btype,
				   int	           acctype,
				   char	           accchr);

extern Hv_Widget       Hv_AddMenuItem(char *label,
				      Hv_Widget	      menu,
				      long            ID,
				      Hv_CallbackProc callback,
				      int	      acctype,
				      char	      accchr);		

extern  void           Hv_GetCurrentPointerLocation(short *x,
						   short  *y);

extern Hv_CompoundString Hv_CreateSimpleCompoundString(char  *str);

extern Hv_CompoundString Hv_CreateCompoundString(char  *str,
						 short  font);

extern Hv_CompoundString Hv_CreateAndSizeCompoundString(char    *text,
							short   font,
							short   *w,
							short   *h);

extern Hv_CompoundString Hv_CreateMotifString(char  *str,
					      short  font);

extern Hv_CompoundString Hv_CreateAndSizeMotifString(char    *text,
						     short   font,
						     short   *w,
						     short   *h);


extern void            Hv_CompoundStringFree(Hv_CompoundString cs);

extern void            Hv_CompoundStringDrawImage(Hv_CompoundString cs,
						  short             x,
						  short             y,
						  short             sw);

extern short           Hv_CompoundStringWidth(Hv_CompoundString cs);

extern short           Hv_CompoundStringBaseline(Hv_CompoundString cs);

extern short           Hv_CompoundStringHeight(Hv_CompoundString cs);

extern void            Hv_CompoundStringDraw(Hv_CompoundString cs,
					     short             x,
					     short             y,
					     short             sw);

extern void            Hv_UnionRectangleWithRegion(Hv_Rectangle  *xr,
						   Hv_Region     region);

extern Hv_Region       Hv_IntersectRegion(Hv_Region reg1,
					   Hv_Region reg2);

extern Hv_Region       Hv_SubtractRegion(Hv_Region reg1,
					 Hv_Region reg2);

extern Hv_Region       Hv_UnionRegion(Hv_Region reg1,
				      Hv_Region reg2);

extern void            Hv_ShrinkRegion(Hv_Region region,
				       short   h,
				       short   v);

extern void            Hv_OffsetRegion(Hv_Region region,
				       short   dh,
				       short   dv);

extern Hv_Region       Hv_CreateRegionFromPolygon(Hv_Point  *poly,
						  short      n);

extern Boolean         Hv_EmptyRegion(Hv_Region region);

extern Hv_Region       Hv_CreateRegion(void);

extern void            Hv_DestroyRegion(Hv_Region  region);

extern Boolean         Hv_PointInRegion(Hv_Point,
					Hv_Region);

extern Boolean         Hv_RectInRegion(Hv_Rect *,
				       Hv_Region);

extern void            Hv_SetFunction(int funcbit);

extern void            Hv_GetGCValues(int *gcv);

extern void            Hv_ClipBox(Hv_Region  region,
				  Hv_Rect   *rect);

extern void            Hv_CheckForExposures(int);

extern void            Hv_SetPattern(int pattern,
				     short color);

extern void            Hv_SetLineStyle(int,
				       int);

extern void 	       Hv_EraseRectangle(short,
					 short,
					 short,
					 short);

extern void            Hv_ReTileMainWindow(void);

extern void            Hv_TileMainWindow(Hv_Pixmap);

extern void            Hv_SetBackground(unsigned long);

extern void            Hv_SetForeground(unsigned long);

extern void            Hv_SetFillStyle(int);

extern void            Hv_SetLineWidth(int);

extern void            Hv_SetClippingRegion(Hv_Region);

extern void            Hv_FreePixmap(Hv_Pixmap pmap);

extern Hv_Pixmap       Hv_CreatePixmap(unsigned int  w,
				       unsigned int  h);

extern void            Hv_ClearArea(short x,
									short y,
									short w,
									short h);

extern void            Hv_CopyArea(Hv_Window src,
				   Hv_Window dest,
				   int       src_x,
				   int       src_y,
				   unsigned  int w,
				   unsigned  int h,
				   int       dest_x,
				   int       dest_y);

extern void            Hv_DestroyImage(Hv_Image *image);

extern void            Hv_PutImage(Hv_Window window,
				   Hv_Image *image,
				   int       src_x,
				   int       src_y,
				   int       dest_x,
				   int       dest_y,
				   unsigned  int w,
				   unsigned  int h);

extern void            Hv_DrawTextItemVertical(Hv_Item    Item,
					       Hv_Region  region);

extern void            Hv_DrawPoint(short,
				    short,
				    short);

extern void            Hv_DrawPoints(Hv_Point *,
				     int,
				     short);

extern void	       Hv_FrameArc(short,
				   short,
				   short,
				   short,
				   int,
				   int,
				   short);

extern void	       Hv_FillArc(short x,
				  short y,
				  short width,
				  short height,
				  int   ang1,
				  int   ang2,
				  Boolean frame,
				  short color,
				  short framec);
    


extern void            Hv_DrawLines(Hv_Point *xp,
				    short     np,
				    short     color);

extern void            Hv_FillPolygon(Hv_Point *xp,
				      short     np,
				      Boolean   frame,
				      short     fillcolor,
				      short     framecolor);

extern void            Hv_DrawSegments(Hv_Segment *xseg,
				       short       nseg,
				       short       color);

extern void            Hv_FrameRectangle(short,
					 short,
					 short,
					 short,
					 short);

extern void 	       Hv_FillRectangle(short,
					short,
					short,
					short,
					short);

extern void 	       Hv_FillRectangles(Hv_Rectangle *rects,
					 int           np,
					 short         color);

extern void            Hv_FillRect(Hv_Rect *rect,
				   short    color);

extern void            Hv_FrameRect(Hv_Rect *rect,
				    short    color);

extern void	       Hv_DrawLine(short x1,
				   short y1,
				   short x2,
				   short y2,
				   short color);

extern void            Hv_SimpleDrawString(short,
					   short,
					   Hv_String *);

extern void            Hv_DrawString(short       x,
				     short       y,
				     Hv_String  *str);

extern void            Hv_StringDraw(short       x,
				     short       y,
				     Hv_String  *str,
				     Hv_Region   region);

extern void           Hv_AddModifier(Hv_XEvent  *event,
				     int         modbit);


extern short          Hv_GetXEventX(Hv_XEvent *event);

extern short          Hv_GetXEventY(Hv_XEvent *event);

extern short          Hv_GetXEventButton(Hv_XEvent *event);

extern Boolean        Hv_Shifted(Hv_XEvent  *event);

extern Boolean        Hv_AltPressed(Hv_XEvent  *event);

extern Boolean        Hv_ControlPressed(Hv_XEvent  *event);

extern void           Hv_Pause(int       interval,
			       Hv_Region region);

extern void           Hv_SysBeep(void);


extern void           Hv_Flush(void);

extern void           Hv_Sync(void);

extern void           Hv_SetCursor(Hv_Cursor  curs);

extern void	      Hv_SetString(Hv_Widget,
				    char *);

extern char          *Hv_GetString(Hv_Widget);

extern Hv_Widget      Hv_Parent(Hv_Widget w);

extern void           Hv_RemoveAllCallbacks(Hv_Widget w,
					    char *name);

extern void           Hv_ManageChild(Hv_Widget w);

extern void           Hv_UnmanageChild(Hv_Widget w);

extern void           Hv_AddCallback(Hv_Widget       w,
				     char           *cbname,
				     Hv_CallbackProc cbproc,
				     Hv_Pointer      data);

extern void           Hv_DestroyWidget(Hv_Widget  w);

extern void           Hv_SetSensitivity(Hv_Widget  w,
					Boolean   val);

extern Boolean        Hv_IsSensitive(Hv_Widget  w);

extern void           Hv_SetWidgetBackgroundColor(Hv_Widget   w,
						  short     color);

extern void           Hv_SetWidgetForegroundColor(Hv_Widget   w,
						  short     color);

extern int            Hv_ClickType(Hv_Item Item,
				   short   button);

extern void           Hv_Println(char *fmt, ...);

extern void            Hv_MakeColorMap(void);

extern unsigned long   Hv_GetForeground(void);

extern unsigned long   Hv_GetBackground(void);


extern unsigned long   Hv_NearestColor(unsigned short r,
				       unsigned short g,
				       unsigned short b);


extern void            Hv_DarkenColor(short color,
				      unsigned short del);

extern void            Hv_BrightenColor(short color,
					unsigned short del);

extern void            Hv_AdjustColor(short color,
				      short r,
				      short g,
				      short b);

extern void            Hv_ChangeColor(short color,
				      unsigned short r,
				      unsigned short g,
				      unsigned short b);

extern void            Hv_CopyColor(short dcolor,
				    short scolor);

extern void            Hv_GetRGBFromColor(short color,
										  unsigned short *r,
										  unsigned short *g,
										  unsigned short *b);

extern void            Hv_QueryColor(Hv_Display   *dpy,
				     Hv_ColorMap   cmap,
				     Hv_Color     *xcolor);


extern void            Hv_DoubleFileSelect(char *title,
					   char *prompt1 ,
					   char *prompt2 ,
					   char *mask1 , 
					   char *mask2 , 
					   char **fname1 , 
					   char **fname2);

extern char           *Hv_FileSelect(char *,
				     char *,
				     char *);

extern char           *Hv_DirectorySelect(char *,
					  char *);

extern char           *Hv_GenericFileSelect(char *,
					    char *,
					    unsigned char,
					    char *,
					    Hv_Widget);

extern char           *Hv_GenericFileSave(char *,
					    char *,
					    unsigned char,
					    char *,
					    Hv_Widget);

extern void            Hv_InitFonts(void);

extern void            Hv_AddNewFont(short);

extern void            Hv_TextEntryItemInitialize(Hv_Item      Item,
						  Hv_AttributeArray attributes);

extern void            Hv_TextEntrySingleClick(Hv_Event hevent);

extern void            Hv_Information(char *);

extern void            Hv_Warning(char *);

extern int             Hv_DoDialog(Hv_Widget,
				       Hv_Widget);


extern int             Hv_Question(char *question);

extern Hv_Widget       Hv_CreateModalDialog(char *,
					    int);

extern Hv_Widget       Hv_CreateModelessDialog(char *,
					       int);

extern int             Hv_EditPattern(short *,
				      char  *);

extern void            Hv_ChangeLabel(Hv_Widget,
				      char *,
				      short);

extern Hv_Widget       Hv_CreateThinOptionMenu(Hv_Widget      rc,
					       short          font,
					       char          *title,
					       int            numitems,
					       char          *items[],
					       Hv_FunctionPtr CB,
					       short          margin);

extern Hv_Widget       Hv_GetOwnerDialog(Hv_Widget);

extern void            Hv_CloseOutCallback(Hv_Widget,
					   int);

extern Hv_Widget       Hv_ActionAreaDialogItem(Hv_Widget         parent,
					       Hv_AttributeArray attributes);

extern Hv_Widget       Hv_ButtonDialogItem(Hv_Widget         parent,
					   Hv_AttributeArray attributes);

extern Hv_Widget       Hv_CloseOutDialogItem(Hv_Widget         parent,
					     Hv_AttributeArray attributes);

extern Hv_Widget       Hv_ColorSelectorDialogItem(Hv_Widget         parent,
						 Hv_AttributeArray attributes);

extern Hv_Widget       Hv_FileSelectorDialogItem(Hv_Widget         parent,
						 Hv_AttributeArray attributes);

extern Hv_Widget       Hv_LabelDialogItem(Hv_Widget         parent,
					  Hv_AttributeArray attributes);

extern Hv_Widget       Hv_ListDialogItem(Hv_Widget         parent,
					 Hv_AttributeArray attributes);


extern Hv_Widget       Hv_RadioDialogItem(Hv_Widget         parent,
					  Hv_AttributeArray attributes);

extern Hv_Widget       Hv_ScaleDialogItem(Hv_Widget         parent,
					  Hv_AttributeArray attributes);

extern Hv_Widget       Hv_SeparatorDialogItem(Hv_Widget         parent,
					      Hv_AttributeArray attributes);

extern Hv_Widget Hv_ScrollBarDialogItem(Hv_Widget         parent,
		   	             Hv_AttributeArray attributes	 );

extern Hv_Widget       Hv_TextDialogItem(Hv_Widget         parent,
					 Hv_AttributeArray attributes);

extern Hv_Widget       Hv_ToggleDialogItem(Hv_Widget         parent,
					   Hv_AttributeArray attributes);

extern Hv_Widget       Hv_RowColDialogItem(Hv_Widget         parent,
					   Hv_AttributeArray attributes);



extern Hv_Widget       Hv_ScrolledTextDialogItem(Hv_Widget         parent,
						 Hv_AttributeArray attributes);

extern int             Hv_GetScaleValue(Hv_Widget   w);

extern void            Hv_SetScaleValue(Hv_Widget   w,
					int      val);

extern int             Hv_ListItemPos(Hv_Widget list,
				      char     *str);

extern Boolean         Hv_CheckListItem(Hv_Widget list,
					char *str,
					int  **poslist,
					int  *num);

extern void            Hv_PosReplaceListItem(Hv_Widget,
					    int,
					    char *);

extern void            Hv_DeleteAllListItems(Hv_Widget);

extern void            Hv_DeselectAllListItems(Hv_Widget);

extern void            Hv_AddListItem(Hv_Widget list,
				      char *str,
				      Boolean selected);

extern Boolean        Hv_ListItemExists ( Hv_Widget 
										 , char * );


extern void            Hv_PosSelectListItem(Hv_Widget,
					    int);

extern void            Hv_NewPosSelectListItem(Hv_Widget,
					       int,
					       Boolean);

extern void            Hv_SelectListItem(Hv_Widget,
					 char *);

extern void            Hv_ListSelectItem (Hv_Widget list,
			                  char     *str,
			                  Boolean   select);

extern int             Hv_GetListSelectedPos(Hv_Widget);

extern int            *Hv_GetAllListSelectedPos(Hv_Widget);

extern Boolean Hv_ListPosSelected(Hv_Widget list, int pos) ;

extern void            Hv_ChangeLabelPattern(Hv_Widget,
					     short);

extern void            Hv_AppendLineToScrolledText(Hv_Widget w,
						   char      *line);

extern void            Hv_DeleteLinesFromTop(Hv_Widget w,
					     int       nlines);

extern void            Hv_JumpToBottomOfScrolledText(Hv_Widget        w,
						     Hv_TextPosition *pos);

extern void            Hv_ClearScrolledText(Hv_Widget,
					    Hv_TextPosition *);

extern void            Hv_AddLineToScrolledText(Hv_Widget,
						Hv_TextPosition *,
						char *);

extern void            Hv_DeleteScrolledTextLines(Hv_Widget,
						  int,
						  Hv_TextPosition *);

extern void            Hv_PopupDialog(Hv_Widget dialog);

extern void            Hv_PopdownDialog(Hv_Widget dialog);

extern void            Hv_OverrideTranslations(Hv_Widget w,
					       char     *table);

extern void            Hv_DoHelp();

extern void            Hv_SetupGLWidget(Hv_View View,char *title);

extern void            Hv_Set3DFrame(Hv_View View);

extern void            Hv_TextInsert(Hv_Widget          w,
				     Hv_TextPosition    pos,
				     char              *line);

extern void            Hv_SetInsertionPosition(Hv_Widget       w,
					       Hv_TextPosition pos);

extern void            Hv_ShowPosition(Hv_Widget       w,
				       Hv_TextPosition pos);

extern Hv_TextPosition  Hv_GetInsertionPosition(Hv_Widget       w);

extern void             Hv_UpdateWidget(Hv_Widget w);

extern Hv_TextPosition  Hv_GetLastPosition(Hv_Widget       w);

extern Boolean          Hv_GetToggleButton(Hv_Widget);

extern void 	        Hv_SetToggleButton(Hv_Widget btn,
										   Boolean   State);

extern Hv_Image        *Hv_Pic8ToImage(byte *pic8,
				       int wide,
				       int high,
				       unsigned long **mycolors,
				       byte *rmap,
				       byte *gmap,
				       byte *bmap);

extern void            Hv_GrabPointer();

extern void            Hv_UngrabPointer();

extern Boolean         Hv_GetNextFile(DIR *,
				      char *,
				      char *,
				      FILE **,
				      char *);

extern int             Hv_AlphaSortDirectory(char *dirname,
					     char *extension,
					     char ***names,
					     Boolean prepend,
					     Boolean remext);

extern void Hv_FixFileSeparator(char *s);

extern DIR            *Hv_OpenDirectory(char *);

extern int             Hv_CloseDirectory(DIR *);

extern void    Hv_SetMainHDC();

#endif





