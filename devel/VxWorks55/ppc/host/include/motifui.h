/* motifui.h - Motif-specific user interface specialization and features */

/* Copyright 1993 Wind River Systems, Inc. */

/* modification history
-----------------------
02b,18sep98,c_s  fix const problem

02a,09mar97,c_s  add extra options to setup method
01z,03dec95,c_s  add tornadoColormapPolicy resource.
01y,20nov95,c_s  add busyAnimSequence resource.
01x,19nov95,c_s  added xpm support for busy animations.
01w,12nov95,c_s  added busySpinMax parameter
01v,09nov95,p_m  removed un-necessary version defines.
01u,06nov95,c_s  added busyAnimDelayUsec to allow adjustment of busy 
                   animation rate.  Added flag to allow overriding 
		   olwm window-property meddling.
01t,02nov95,c_s  added busy animation support, dialogForce convenience 
                   function, ttyInit function.
01s,08aug95,c_s  added handleFlags method; UI_VERSION_STRING.
01r,17jul95,c_s  added _wsArg[cv] instance variables and restart method.
01q,19jun95,c_s  added chartBackground resource.
01p,13jun95,c_s  added _mainDialog private instance variables and access 
                   functions.
01o,04may95,c_s  added new chart resources, POPUP_SHELL destroy callback
                   capability.
01n,10apr95,c_s  added promptForTcl resource.
01m,06apr95,c_s  added window manager setup material, POPUP_SHELL::resize.
01l,27mar95,c_s  added chart resources, chartAdd prototype.
01k,22mar95,c_s  reformatted.  uses namelist library.  removed obsolete 
                   material.
01j,15mar95,c_s  removed license mgt variables.
01i,05mar95,c_s  added POPUP_SHELL constructor/destructor.
01h,22jan95,c_s  added listBoxAdd ().
01g,02jan94,c_s  made activeObjectIconInstall a method of ActiveObject.
01f,15dec94,c_s  added some new externals.
01e,06dec94,c_s  added popup() method to POPUP_SHELL.
01d,25nov94,c_s  added Hierarchy highlight resources.
01c,14nov94,c_s  added PopupShell service metods/strucutres, some new 
                   resources.  Removed obsolete routine.
01b,10nov94,c_s  reorganized, reformatted; added WPWR_RESOURCE structure
01a,21apr94,c_s  written.
*/

#ifndef __INCmotifuih
#define __INCmotifuih

#include "ui.h"
#include "namelist.h"

#include <Xm/Xm.h>

// flags for the createStandardFrame() method.  These control whether
// to create a menu bar, toolbar, and busybox.

#define FRAME_MENU	0x1		// configurable menu bar
#define FRAME_TOOLBAR	0x2		// configurable tool bar
#define FRAME_BUSYBOX	0x4		// busy animation
#define FRAME_ALL	(~0)		// all standard elements

typedef enum 		// WM_WINDOW_TYPE
    {
    WM_MAIN_WINDOW,				// main window
    WM_DIALOG_WINDOW				// dialog window
    } WM_WINDOW_TYPE;

class ActiveObject;				// forward declaration

class MotifUserInterface:			// MotifUserInterface Class
    public UserInterface			// Inherits from UserInterface
    {
    public:				// PUBLIC			       

    MotifUserInterface ();			// constructor

    const char *	appName (void);		// application name
    char *		appTclPath (void);	// path to Tcl init files
    XtAppContext	appContext (void)	// get application context
        { return _appContext; }

	
    void		setup (void)
	{ setup (NULL, 0, NULL, 0); }

    void 		setup			// setup
	(
	XrmOptionDescRec * appOpts,
	int		nAppOpts,
	XtActionsRec *	actions,
	int		nActions
	);

    void 		tclInit (void);		// initialize Tcl

    void 		build (void);		// create appl frame
    void 		launch (void);		// run Xt main loop
    
    virtual const char * handleFlags		// take care of cmd line
        (
	char *		exeName,		// what to type to start tool
        char *		toolDescrip,		// tool description
	int		returnOneArg,		// return 1st nonflag arg
	char *          extraArgName		// name of arg returned
	);

    Widget		topLevel (void) 	// get toplevel widget handle
	{return _appShell;}

    Widget		standardFrameCreate	// menu, toolbar, busybox
	(
	int		frameFlags		// which of above to create
	);

    void 		menuBar			// set main menu handle
	(
	Widget 		mb			// menubar widget
	)
        {_menuBar = mb;}

    Widget menuBar (void)			// get main menu handle
        {return _menuBar;}

    void 		toolBar			// set main toolbar handle
	(
	Widget 		tb			// toolbar widget
	)
        {_toolBar = tb;}

    Widget 		toolBar (void)		// get main toolbar handle
        {return _toolBar;}

    void		busyBox			// set busy box widget
	(
	Widget		bbox
	);

    Widget		busyBox (void)
        {return _busyBox;}

    void 		mainDialog		// set main dialog handle
	(
	Widget 		md			// dialog container
	)
        {_mainDialog = md;}

    Widget 		mainDialog (void)	// get main dialog handle
        {return _mainDialog;}

    void 		tty			// set main TTY handle
	(
	Widget 		d			// TTY text widget
	)
        {_tty = d;}

    Widget 		tty (void) 		// get main TTY handle
        {return _tty;}

    Pixmap 		appIcon (void)		// get main application icon
        {return _appIcon;}

    void 		argListSet		// set argc/argv
        (
	int 		argc,
	char **		argv
	);

    void 		argListGet		// get argc/argv
	(
	int *		argc,
	char ***	argv
	)
        {*argc = _argc; *argv = _argv;}  

    void		wsArgList		// set "original" arg list
	(
	int		argc,
	char **		argv
	);

    char **		wsArgList (void);	// get "original" arg list

    void 		markInstall		// install a mark by name
        (
	char *		markName
	);

    void 		markDrawAt		// draw a mark in a window
        (
	char *		markName,
	Display *	pDisplay,
	Window		window,
	Pixel		background,
	int		x,
	int		y
	);

    Pixmap 		markPixMask		// get mark pixmask by name
        (
	char *		markName
	);

    ActiveObject *	markActiveObject	// get mark A.O. by name
        (
	const char *	markName
	);

    void		windowManagerSetup
        (
	Widget		shell,
	Widget		parent,
	WM_WINDOW_TYPE	type
	);

    void		restart 		// restart app from scratch
	(
	int		nExtraArgs,		// count of extra arguments
	char **		extraArgs		// extra arguments
	);

    void		busyAnimate		// perform busy animation
	(
	int		fromTimer		// !0 if called from timer
	);

    void		busyState
	(
	int		state
	);

    int			busyState (void)
        { return _busyWhenXIdle != 0; }

    void		busyTimeoutFunc (void);

    void		asyncAnimationStart (void);	// override base class
    void		asyncAnimationEnd (void);	// override base class

    struct		SizedPixmap		// a pixmap with its size
	{					// (local structure def)
	Pixmap		pm;
	int		width;
	int		height;
	int		mono;			// !0 if monochrome
	};

    void		animationLoad		// read vector of anim cels
	(	
	char *		type,			// which animation
	int *		pNcels,			// RETURN: no. of cels read
	SizedPixmap **	pVcels			// RETURN: alloc'd vec of cels
	);

    char *		customization (void);	// get value of *customization

    SizedPixmap *	vBusyPixmap;		// vector of animation pixmaps
    int			cBusyPixmap;		// count of above vector

    private:				// PRIVATE

    int			_busyWhenXIdle;		// run busy animation when idle
    XtIntervalId	_busyTimeout;		// timeout cookie
    int			_busyClock;		// counts busy cycle
    char **		_argv;		        // argv from main()
    int 		_argc;			// argc from main()

    char **		_wsArgv;		// saved argv, for restart
    int			_wsArgc;		// saved argc, for restart

    Display *		_pDisplay;		// X Display pointer
    XtAppContext 	_appContext;		// Xt AppContext

    Widget 		_appShell;		// Top-level shell
    Widget 		_menuBar;		// Main menu bar
    Widget 		_toolBar;		// ToolBar
    Widget		_busyBox;		// busy animation container
    Widget 		_mainDialog;		// Main dialog container
    Widget 		_tty;			// TTY window for subproc 

    Pixmap 		_appIcon;		// application WM icon

    GC			_markGC;		// GC for drawing marks
    GC			_busyGC;		// GC for busy animation

    struct		MOTIF_MARK		// little named icon
	{
	Pixel		foreground;		// foreground color
	ActiveObject *	pActObj;		// active object
	};

    NamedList		_motMarkList;		// list of MOTIF_MARKs
    };

class ActiveObject
    {
    public:				// PUBLIC

    ActiveObject				// constructor
        (
	Widget 		widget,			// widget
	const char *	name,			// name of widget
	const char *	binding = 0,		// Tcl binding
	const char *	dragBinding = 0,	// Tcl drag binding
	const char *	dropBinding = 0,	// Tcl drop binding
	Pixmap 		pix = 0,		// Pixmap
	Pixmap 		pixMask = 0		// Pixmask (for dragging)
	);

    ~ActiveObject (void);			// destructor

    char *		name (void) 		// get name
	{return _name;}

    void 		name			// set name 
        (
	const char *	n
	);

    const char *	binding (void) 		// get binding
	{return _binding;}

    void 		binding			// set binding
        (
	const char *	b
	);

    const char *	dragBinding (void) 	// get dragBinding
	{return _dragBinding;}

    void 		dragBinding		// set dragBinding
        (
	const char *	db
	);

    const char *	dropBinding (void) 	// get dropBinding
	{return _dropBinding;}

    void 		dropBinding		// set dropBinding
        (
	const char *	db
	);

    void 		establishStandardDropTarget (void);

    Pixmap 		pixmap (void) 		// get pixmap
	{return _pixmap;}

    void 		pixmap 			// set pixmap
        (
	Pixmap 		p
	) 
	{_pixmap = p;}

    Pixmap 		pixMask (void) 	        // get pixmask
	{return _pixMask;}

    void 		pixMask			// set pixmask
        (
	Pixmap p
	) 
	{_pixMask = p;}
    int 		iconWidth (void) 	// get iconWidth
	{return _iconWidth;}
    int 		iconHeight (void) 	// get iconHeight
	{return _iconHeight;}
    Widget 		widget (void) 		// get widget
	{return _widget;}
    void 		hook		 	// set hook
        (
	void *hook
	)
	{_hook = hook;}

    void *		hook (void) 		// get hook
	{return _hook;}

    STATUS 		iconInstall		// install an icon from disk
        (
	char *		iconDir,		// directory
	char *		iconName,		// icon name
	Widget 		widget = 0,		// widget (for color info)
	Pixel 		foreground = ~0,	// foreground color
	Pixel 		background = ~0		// background color
	);
    
    void 		dragInitiate		// start a drag
        (
	XEvent *	pXEvent,		// X Event initiating drag
	char *		icon 			// drag icon
	);

    void dragCoordinateHook 			// set hook for drag text
	(
	char *		(*dcHook)		// funcptr
	    (
	    ActiveObject *,			// active object dragged from
	    int 	x,			// x coord of drag initiation
	    int 	y			// y coord of drag initiation
	    )
	)
        {_dcHook = dcHook;}   

    char *		(*dragCoordinateHook (void))	// get drag cd. hook
	(
	ActiveObject *	pActObj,
	int 		x,
	int 		y
	)
        {return _dcHook;}  

    private:				// PRIVATE

    Widget		_widget;		// widget implementation
    char *		_name;			// name (for bookkeeping)
    char *		_binding;		// Tcl expr when "clicked"
    char *		_dragBinding;		// Tcl expr when dragged
    char *		_dropBinding;		// Tcl expr when dropped on
    Pixmap		_pixmap;        	// icon
    Pixmap		_pixMask;		// B/W icon
    int			_iconWidth;		// icon width;
    int			_iconHeight;		// icon height;
    char *		(*_dcHook)		// drag coordinate hook
        (
	ActiveObject *	pActObj,		// A.O. being dragged
	int 		x,			// drag start x coord
	int 		y			// drag start y coord
	);
    void *		_hook;			// hook (client-defined)
    int 		_stdDropTgtInstalled;	// if std. drop tgt. instld.
    };

//
// WPWR_RESOURCE is a structure of X resource values that is common to all 
// applications using the common Motif UI.
//


typedef struct					// WPWR_RESOURCE
    {
    Pixel		chartBorderColor;	// color of chart "bar" border
    int			chartBorderWidth;	// border width of "bar"
    Pixel		chartBarColor;		// color of chart "bar"
    Pixel		chartBackground;	// color behind chart "bar"
    XFontStruct *	pChartNameFont;		// font for quantity names
    Pixel		chartNameColor;		// color for quantity names
    XFontStruct *	pChartScaleFont;	// font for min/max annotation
    Pixel		chartScaleColor;	// color for min/max annotation
    XFontStruct *	pChartValFont;		// font for value annotation
    Pixel		chartValColor;		// color for value annotation
    Pixel		chartMark1Color;	// for top mark
    Pixel		chartMark2Color;	// for bottom mark
    int			chartMarkSize;		// size of mark divot

    XFontStruct *	pHierFont;		// font for hierarchy windows
    Pixel		hierColor;		// color for hierarhcy items

    XFontStruct *	pHierExpFont;		// hier expansible nodes font
    Pixel		hierExpColor;		// hier expansible nodes color

    XFontStruct *	pHierSpecialFont;	
    Pixel		hierSpecialColor;	// hier special nodes color

    XFontStruct *	pHierChgFont;		// hier changed nodes font
    Pixel		hierChgColor;		// hier changed nodes color

    XFontStruct *	pHierHighlightFont;	// hier highlighted nodes font
    Pixel		hierHighlightColor;	// hier highlighted nodes font

    int			indentWidth;		// indentation step for viewers
    int			hierMaxWidth;		// max width of hier windows
    int			hierMaxHeight;		// max height of hier windows

    Boolean		independentDialogs;	// dialog stack/button behav.
    Boolean		promptForTcl;		// read Tcl from stdin
    Boolean		busyAnimation;		// perform busybox animation
    int			busyAnimationStyle;	// what to draw for busy anim
    Pixel		busyAnimForeground;	// animation foreground color
    Pixel		busyAnimBackground;	// animation background color
    int			busyAnimDelayUsec;	// delay betwn. anim's in usec
    int			busySpinMax;		// max times to intr. timeout
    Boolean		busyAnimStabilize;	// show 0'th image when not busy
    String		busyAnimSequence;	// filename prefix of anim seq

    Boolean		adjustOlwmProperties;	// T->fixup win props if olwm
    int			tornadoColormapPolicy;	// how to allocate colors
    int			textFocusLossCallback;	// if callback on focus loss
    } WPWR_RESOURCE;

extern WPWR_RESOURCE *	pWpwrResource;

typedef struct					// COORDINATE
    {
    int 		x;			// x coordinate of event
    int			y;			// y coordinate of event
    ActiveObject *	pActObj;		// ActiveObject receiving event
    } COORDINATE;

struct POPUP_SHELL				// POPUP_SHELL
    {
    POPUP_SHELL					// constructor
        (
	char *		name,			// name
	char *		title,			// titlebar text
	char *		wmDestroyCb = 0		// WM destroy Tcl callback
	);

    ~POPUP_SHELL ();

    void		resize			// resize shell
	(
	int		width,
	int		height
	);

    void 		popup (void);		// method to pop one up
    void 		popdown (void);		// method to pop one down
    void 		title (char *);		// set title

    Widget		widget;			// PopupShell Widget
    int			displayed;		// whether displayed (tracked)
    char *		_destroyCb;		// Tcl destroy callback
    char *		_name;			// name given at construction
    };

extern Widget 		dialogToplevel (char *name);
extern void		dialogForce (Widget);
extern STATUS		ttyInit (XtAppContext);
extern STATUS 		ttyConnect (int *pPid, char *argv[]);
extern STATUS 		ttySend (char *string);
extern STATUS 		textWindowAdd (char *name, Widget textWidget, 
				       POPUP_SHELL *pShell);
extern STATUS		listBoxAdd (char *name, Widget listBoxWidget,
				    char *selectCallback);
extern STATUS		hierarchyAdd (char *name, void *pHierView);
extern STATUS		chartAdd (char *name, Widget parent, int showScale,
                                  char *valueFmt);

extern void 		activeObjectTclCallback (Widget, XtPointer client,
						 XtPointer call);

extern void		activeObjectDragSource (Widget widget,
						XEvent *pXEvent, String *pArgs,
						Cardinal *pNumArgs);

extern int		tclReturnStatus (Tcl_Interp *, STATUS);

extern void 		dropTransferSetup
    (
    Widget 		widget,
    void *		clientData,
    void *		callData,
    void 		(*dropTransferFunc)
	(
	Widget		widget,
	XtPointer	clientData,
	Atom *		selection,
	Atom *		type,
	XtPointer	value,
	unsigned long *	length,
	int *		format
	)
    );

extern void 		stdActiveObjectDropTransfer (Widget, XtPointer,
						     Atom *, Atom *,
						     XtPointer, 
						     unsigned long *, int *);

extern char *		dropTransferText 
    (
    Widget		widget,
    XtPointer		clientData,
    Atom *		selection,
    Atom *		type,
    XtPointer		value,
    unsigned long *	length,
    int *		format,
    COORDINATE *	pCoord
    );

#endif /* !__INCMotifUIh */
