/* hierview.h - HierarchyView (subclass of Viewer object) */

/* Copyright 1994 Wind River Systems, Inc. */

/* modification history
-----------------------
01f,24feb95,c_s  added setValuesOnly.
01e,19jan95,c_s  added HierarchyView destructor prototype.
01d,02jan95,c_s  added levelOfLine method prototype.
01c,23nov94,c_s  added highligting support and new traversal behavior 
                   setting methods; added some debug printing support.
01b,14nov94,c_s  added callback support; moved hierarchy style #defines here.
                   Reorganized callback methods a bit.
01a,07nov94,c_s  written.
*/

#ifndef __INChierviewh
#define __INChierviewh

// The HierarchyView Class

#include <Xm/Xm.h>

#include "viewer.h"
#include "motifui.h"
#include "hier.h"

//
// Constants defining the various "styles" (font, color) that a hierarchy
// element can appear in.
//

#define HIER_BASIC_STYLE	0
#define HIER_EXP_STYLE		1
#define HIER_CHANGED_STYLE	2
#define HIER_SPECIAL_STYLE	3
#define HIER_HIGHLIGHT_STYLE	4
#define HIER_NUM_STYLES		5

#define HIER_TRAVERSE_SPECIAL	0
#define HIER_TRAVERSE_ANY	1

class HierarchyView : public Viewer
    {
    public:

    // Instantiate a code view.

    HierarchyView
        (
	char *		name,
	char *		callback,
	Widget 		parent,
	XFontStruct **	ppFont,
	int 		nFonts,
	Pixel *		pColor,
	int 		nColors
	);

    ~HierarchyView ();

    STATUS setStructure
        (
	char *		tclList
	);

    void setTraverseBehavior
        (
	int		travBehavior
	)
	{
	traverseBehavior = travBehavior;
	}

    void setChangeHighlights
        (
	int		chgHighlights
	)
	{
	changeHighlights = chgHighlights;
	}

    void setValuesOnly
        (
	int		valOnly
	)
	{
	valuesOnly = valOnly;
	}

    STATUS setValues
        (
	char *		tclList
	);

    void highlightSet
        (
	char *		path
	);
	
    void highlightUnset
        (
	char *		path
	);
	
    void action
        (
	int		physLine
	);

    char *callback (void)
        {
	return _callback;
	}

    void print (void)
	{
	cout << *pHierarchy;
	}

    protected:

    // Overrides base class.  Used to connect the Viewer's requests for
    // text to display.

    char *textOfLine
        (
	int 		physLine,
	int *		fontIx,
	int *		colorIx
	);

    // Overrides base class.  Used to interface the globalMarkList to the 
    // Viewer's requests for marks to display.

    int marksOfLine
        (
	int		physLine,
	int		maxMarks,
	char **		markNameV,
	char **		markTypeV
	);

    // Overrides base class.  Returns the indentation level in the hierarchy
    // of the indicated physical line.

    int levelOfLine
        (
	int		physLine
	);

    // Overrides base class.  Used to connect the Viewer's requests for
    // the length of the provided text to the characteristics of our
    // attached CodeSource.

    int physLineCount (void);

    private:

    void 		refigureSize (void);

    Hierarchy *		pHierarchy;	// hierarchy we're viewing
    char *		_callback;	// callback when traverse event arrives

    int			maxWidth;	// maximum width we will grow to
    int			maxHeight;	// maximum height we will grow to

    int			traverseBehavior;  // one of HIER_TRAVERSE_*
    int			changeHighlights;  // whether to hilite chg'd entries
    int			valuesOnly;	   // suppress names, just show vals

    int			haveSetValues;	// if we have ever set values
    };

#endif /* __INChierviewh */


