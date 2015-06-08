/* viewer.h - Interface for Viewer object: a flexible text display for Motif. */

/* Copyright 1994 Wind River Systems, Inc. */

/* modification history
-----------------------
01f,20feb96,jco  added horizontal scrollBar support (SPR 4623).
01e,02nov95,c_s  adjusted Viewer constructor protype so subclasses can append
                   an action block to the initialization.
01d,23jan95,c_s  made Viewer destructor virtual.  made displayLineNumbers ()
		   void.
01c,19jan95,c_s  added Viewer destructor prototype.
01b,02jan95,c_s  added levelOfLine method, with a trivial default 
                   implementation.
01a,04nov94,c_s  written.
*/

#ifndef __INCviewerh
#define __INCviewerh

// The Viewer Class

#include <Xm/Xm.h>

#include "motifui.h"

class Viewer
    {
    public:

    // Instantiate a Viewer

    Viewer
        (
	char *		name,		// name 
	Widget 		parent,		// above root of viewer hierarchy
	XFontStruct **	ppFont,		// font vector
	int 		nFonts,		// count of above
	Pixel *		pColor,		// color vector
	int 		nColors,	// count of above
	XtActionsRec *	actions = 0,	// subclass-specific actions
	int		nActions = 0	// count of elts in above array
	);

    virtual ~Viewer ();

    // round the horizontal scrollBar position to a multiple of the biggest
    // font width.

    int hScrollBarValueRound 
	(
	int value
	);

    // If this is called by the creator of a viewer,  a special region
    // at the left of the viewer will be reserved for line numbers.  The
    // width of the region is the fence parameter.

    void displayLineNumbers
        (
	int		fence
	)
	{
	lineLabelFence = fence;
	}

    // Set the first column displayed in the Viewer.  Return nonzero
    // if Viewer was redrawn as a result.

    int firstColumn
        (
	int 		firstCol,	// new physical first column 
	int 		forbidScroll = 0 // don't try to smooth scroll if true
	);

    // Return the first column displayed in the Viewer

    int firstColumn (void)
	{
	return _firstColumn;
	}

    // Set the first line displayed in the Viewer.  Return nonzero
    // if Viewer was redrawn as a result.

    int topLine
        (
	int 		topLine,	// new physical line for top
	int 		forbidScroll = 0 // don't try to smooth scroll if true
	);


    // Return the top line displayed in the Viewer

    int topLine (void)
	{
	return _topLine;
	}

    // Find out the y coordinate of the given line.

    int lineToY
        (
	int		line
	);

    // Map from a y coordinate to a physical line number.

    int yToLine
        (
	int		position
	);

    // Repaint the Viewer

    void redraw (void);

    // Repaint certain lines of the Viewer

    void partialRedraw
        (
	int		skipColumnAtLeft = 0,
	int		skipColumnAtRight = 0,
	int		skipLinesAtTop = 0,
	int		skipLinesAtBottom = 0,
	int		hScrolling = 0
	);

    // Redraw just one line of the Viewer.

    void redrawOneLine
        (
	int		physLine
	);

    // An extra redraw hook, which may be redefined by subclasses as necessary.

    virtual void specialRedraw
        (
	int		skipColumnAtLeft = 0,
	int		skipColumnAtRight = 0,
	int		skipLinesAtTop = 0,
	int		skipLinesAtBottom = 0
	)
        {}
    
    // Service an exposure event.  (Also does some smooth-scroll processing).

    void serviceExposure
        (
	XEvent *
	);

    // Refigure things after a resize.

    void serviceResize (void);

    // Select the word at the given mouse coordinates.

    void selectWordAtPoint
        (
	int		x,
	int		y
	);

    // return the string selected, or 0 if nothing is selected in the 
    // Viewer.

    char *selectionText (void);

    // A routine to convert between x,y coordinates representing the
    // location where something has been dropped or clicked in the
    // viewer to a string that should be passed as an argument to the
    // Tcl procedure bound to the action (click, drag, etc.)
    // This (base class) implementation returns the empty string.

    virtual char *coordinatesToName
        (
	int		x,
	int		y
	)
	{
	return "";
	}

    int positionToMark
        (
	int		x,
	int		y,
	char **		pMarkName,
	char **		pMarkType
	);
 
    // If this is called by the creator of a Viewer, the Viewer will be
    // configured as a drop site and the dropBinding will be attached
    // to the Viewer's ActiveObject.

    void dropEnable
        (
	char *		dropBinding
	);

    protected:

    // Provide access to the Viewer's form widget, so that subclasses 
    // can modify the arrangement of the widgets created by the Viewer.

    Widget viewerForm (void)
	{
	return formWidget;
	}

    Widget viewerText (void)
	{
	return textDaWidget;
	}

    // Provide access to our ActiveObject, so the various drag/drop
    // bindings can be modified by subclasses.

    ActiveObject *activeObject (void)
	{
	return pViewerActObj;
	}

    // Prototypes for line-query functions.  These are to be overridden 
    // by subclasses to supply the text and style information so that the 
    // base class redraw method can render the data.

    virtual char *textOfLine
        (
	int 		physLine,
	int *		fontIx,
	int *		colorIx
	);

    virtual char *labelOfLine
        (
	int 		physLine,
	int *		fontIx,
	int *		colorIx
	);

    virtual int marksOfLine
        (
	int		physLine,
	int		maxMarks,
	char **		markNameV,
	char **		markTypeV
	);

    virtual int levelOfLine
        (
        int		physLine
	)
	{
	return 0;
	}

    // To be redefined by subclasses.  If scrollable (), this is used
    // to configure the scrollbar so that the extent of the text source
    // may be panned over.

    virtual int physLineCount (void)
        {
	return 100;			// just for debugging
	}

    // Whether a scrollbar should be displayed.  The default version of 
    // this function returns true (which means that the scrollbar will 
    // appear whenever physLineCount () > windowLines (); this is 
    // appropriate except for text sources that cannot pan over their 
    // extent).

    virtual int scrollable (void)
        {
	return 1;
	}

    virtual int scrollableH (void)
        {
	return 1;
	}

    // If necessary, scroll so that the given line is visible.

    int lineMakeVisible
        (
	int		line,
	int		forbidScroll = 0
	);

    // Set the selection to the specified physical line, starting at 
    // the indicated character position and extending for the specified
    // length.

    void setSelection 
        (
	int		physLine,
	int		charPos,
	int		len,
	char *		text
	);

    // Unset the selection, if there is one.

    void unsetSelection (void);


    // Computes how many lines of text will fit in the Viewer

    int windowLines (void);

    // Computed baseline height of fonts (taken together).

    int baselineHeight (void)
	{
	return _baselineHeight;
	}

    private:

    void scrollBarReconfigure (void);
    void scrollBarReconfigureV (void);
    void scrollBarReconfigureH (void);
    long longestVisibleLineInPixels (void);

    XFontStruct *styleFontGet
        (
	int 		styleIx
	);

    Pixel styleColorGet
        (
	int		styleIx
	);

    // IDs of the four Motif widgets that are used as the graphical
    // implementation of the widget.

    Widget		formWidget;	// widget for aligning text & scroll
    Widget		textDaWidget;	// widget where text is drawn
    Widget		vSBarWidget;	// the vertical scrollbar widget
    Widget		hSBarWidget;	// the horizontal scrollbar widget

    ActiveObject *	pViewerActObj;	// ActiveObject for viewer text window
    int			lineLabelFence;	// space reserved for line nos. (or 0)
    int 		_topLine;	// the current top line of window
    int 		_firstColumn;	// the current first column of window
    GC			_gc;		// GC for drawing text
    GC			_bltGc;		// GC for smooth-scrolling
    XFontStruct **	_ppFont;	// Vector of fonts for text drawing
    int			_nFonts;	// Length of above
    Pixel *		_pColor;	// Vector of colors for text drawing
    int			_nColors;	// Length of above
    int			_baselineHeight;  // Max. baseline of all fonts
    int			_descent;	// Descent of baseline font
    int			_width;		// Max. width of all fonts
    int			dropEnabled;	// whether we have configured for drop
    int			reconfiguring;	// Boolean set when redoing scrollbar
    
    // Selection Data

    int			selectionExists; // true if there is a selection now
    int			selPhysLine;	// physical line number of selection
    int			selWordStartIx;	// first character of selected word
    int			selWordLen;	// length in char's of selected word
    char		selText [80];	// text of selected word
    };

#endif /* __INCviewerh */


