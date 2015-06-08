/* namelist.h - string-keyed double linked list interface */

/* Copyright 1994 Wind River Systems, Inc. */

/* modification history
-----------------------
01b,18sep98,c_s  ansi C++ fixes

01a,20mar95,c_s  written.
*/

#ifndef __INCnamelisth
#define __INCnamelisth

#include "tcl.h"

// class NamedList implements a simple string-keyed linked list.

class NamedList
    {
    public:				// PUBLIC

    NamedList ()				// constructor
        {
	_pHead = 0;				// initialize pointers
	_pTail = 0;
	}

    ~NamedList ();				// destructor

    void 		add			// add a node
        (
	const char *	name,			// name
	void *		pObj			// object pointer
	);

    void *		find
        (
	const char *	name			// find node by name
	);

    void *		remove			// remove node by name
        (
	const char *	name
	);
     
    void		tclAppendList		// append names to Tcl list
        (
	Tcl_Interp *	pInterp			// interpreter context
	);

    private:				// PRIVATE

    class NamedListNode				// node structure
	{
	public:

	NamedListNode 				// node constructor
	    (
	    const char * name,			// node name
	    void *	 pOb			// object pointer
	    );

	~NamedListNode ();			// node destuctor

	char *		name (void)		// get name
	    {return _name;}

	NamedListNode *	pPrev;			// prev pointer
	NamedListNode *	pNext;			// next pointer
	void *		pObj;			// object pointer
	
	private:

	char *		_name;			// node name storage
	};

    NamedListNode *	nodeFind		// find node by name
        (
	const char *	name
	);

    void *		remove			// remove by node
        (
	NamedListNode *	pNode
	);

    NamedListNode *	_pHead;			// head pointer
    NamedListNode *	_pTail;			// tail pointer
    };

#endif /* __INCnamelisth */
