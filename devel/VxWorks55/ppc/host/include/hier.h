/* hier.h - Hierarchy class interface */

/* Copyright 1994 Wind River Systems, Inc. */

/* modification history
-----------------------
01d,18sep98,c_s  fix const problem

01c,23nov94,c_s  added array, debugging, and highlighting support.
01b,14nov94,c_s  added callback support; added parentIx private member for 
                   backward traversals.
01a,08nov94,c_s  written.
*/

#ifndef __INChierh
#define __INChierh

// The Hierarchy class 

class Hierarchy
    {
    public:

    Hierarchy				// construct, given Tcl-syntax list
	(
	char *		list,		// Tcl structured list
	Hierarchy *	parent = 0,	// if subhierarchy: parent
	int		parIx = 0	// if subhierarchy: index w/in parent
	);

    ~Hierarchy ();			// destroy

    STATUS setTerminalValues
        (
	char *		list
	);

    Hierarchy * nth
	(
	int	       	n,		// item number sought
	int *		pIx,		// RETURN: index in H. of found elt.
	int *		pLevel		// RETURN: level where hit was found
	);

    const char * name
        (
	int 		ix
	)
	{
	return (ix > count) ? "" : pName [ix];
	}

    const char * value
        (
	int 		ix
	)
	{
	return (ix > count) ? "" : pValue [ix];
	}

    int collapsed
        (
	int 		ix
	)
	{
	return (ix > count) ? 0 : pCollapsed [ix];
	}

    int expandable 
        (
	int		ix
	)
	{
	return (ix > count) ? 0 : (ppSubItem [ix] != 0);
	}

    int changed
	(
	int 		ix
	)
        {
	return (ix > count) ? 0 : (pChanged [ix] != 0);
	}

    int special
        (
	int 		ix
	)
	{
	return (ix > count) ? 0 : (pSpecial [ix] != 0);
	}

    int highlight
        (
	int 		ix
	)
	{
	return (ix > count) ? 0 : (pHighlight [ix] != 0);
	}

    void toggleExpansion
        (
	int		ix
	);

    void invokeCallback
	(
	int 		ix,
	char *		callback
	);

    int closureCount (void)
        {
	return _closureCount;
	}

    void highlightSet
        (
	char *		path
	);

    void highlightUnset
        (
	char *		path
	);

    // for debugging: dump operator.

    friend ostream & operator <<
        (
	ostream &o,
	const Hierarchy &h
	);

    private:

    Hierarchy *nthRecurse
	(
	Hierarchy *	pHierarchy,	// node to start at
	int	       	n,		// item number sought
	int		currentPoint,	// how far we've traversed
	int		level,		// level of recursion
	int *		pIx,		// RETURN: index in H. of found elt.
	int *		pLevel		// RETURN: level where hit was found
	);

    Hierarchy *findByPath
        (
	char *		path,		// path to element (e.g., "a b c")
	int *		pIx		// index in returned Hier. of element
	);

    void arrayResize
        (
	int		newSize		// new size of array node 
	);

    void *resize
        (
	void *		oldArray,       // existing array 
	int		oldSize,	// # elements in above
	int 		newSize,	// # elements desired
	int		eltSize,	// size of each element
	int		free		// whether things in array need freeing
	);

    void adjustClosureCounts
        (
	int		delta		// amount to recursively bias cc's by
	);

    void dumpRecurse
        (
        ostream &	o,
        int		level
        ) const;

    void setup
        (
	int		n
	);

    int			count;		// number of nodes at this level
    int			_closureCount;	// totalled number of sub. nodes
    int			array;		// true if this is an array node
    char *		arrayStruct;	// structure of array elements

    char **		pName;		// vector of string names
    char **		pValue;		// vector of string values
    int * 		pCollapsed;	// vector of "collapsed" state
    int * 		pSpecial;	// vector of "special" state
    int *		pHighlight;	// vector of "highlighed" state;
    int *		pChanged;	// vector of "changed" state
    Hierarchy **	ppSubItem;	// vector of subitem vectors

    Hierarchy *		pParent;	// parent of this subitem (0 if root)
    int			parentIx;	// index of this within parent array
    };

#endif /* !__INChierh */
