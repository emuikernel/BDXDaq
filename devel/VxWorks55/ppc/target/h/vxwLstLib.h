// VXWList/vxwLstLib.h - simple linked list class

// Copyright 1995-1999 Wind River Systems, Inc.

// modification history
// --------------------
// 01e,28mar99,jdi  doc: removed mention of support for Booch class.
// 01d,23feb99,fle  made it refgen parsable
// 01c,21feb99,jdi  added library section, checked in documentation.
// 01b,03oct95,rhp  moved "Using" instructions to library man page;
//                  documented constructors and member fns.
// 01a,15jun95,srh  written.

// DESCRIPTION
// The VXWList class supports the creation and maintenance of a doubly
// linked list.  The class contains pointers to the first and last
// nodes in the list, and a count of the number of nodes in the list.
// The nodes in the list are derived from the structure NODE, which
// provides two pointers: `NODE::next' and `NODE::previous'.
// Both the forward and backward chains are terminated with a NULL pointer.
//
// The VXWList class simply manipulates the linked-list data structures;
// no kernel functions are invoked.  In particular, linked lists by themselves
// provide no task synchronization or mutual exclusion.  If multiple tasks will
// access a single linked list, that list must be guarded with some
// mutual-exclusion mechanism (such as a mutual-exclusion semaphore).
//
// NON-EMPTY LIST:
// .CS
//    ---------             --------          --------
//    | head--------------->| next----------->| next---------
//    |       |             |      |          |      |      |
//    |       |       ------- prev |<---------- prev |      |
//    |       |       |     |      |          |      |      |
//    | tail------    |     | ...  |    ----->| ...  |      |
//    |       |  |    v                 |                   v
//    |count=2|  |  -----               |                 -----
//    ---------  |   ---                |                  ---
//               |    -                 |                   -
//               |                      |
//               ------------------------
// .CE
//
// EMPTY LIST:
// .CS
//         -----------
//         |  head------------------
//         |         |             |
//         |  tail----------       |
//         |         |     |       v
//         | count=0 |   -----   -----
//         -----------    ---     ---
//                         -    -
// .CE
//
// WARNINGS
// Use only single inheritance!  This class is an interface to the
// VxWorks library lstLib.  More sophisticated alternatives are
// available in the Tools.h++ class libraries.
//
// EXAMPLE
// The following example illustrates how to create a list by deriving
// elements from NODE and putting them on a VXWList.
//
// .CS
// class myListNode : public NODE
//     {
//   public:
//     myListNode ()
//      {
//      }
//   private:
//     };
//
// VXWList      myList;
// myListNode   a, b, c;
//
// NODE       * pEl = &c;
//
// void useList ()
//     {
//     myList.add (&a);
//     myList.insert (pEl, &b);
//     }
// .CE
//
// INCLUDE FILES: vxwLstLib.h
//
// SECTION: 1C
//

#ifndef vxwLstLib_h
#define vxwLstLib_h

#include "vxWorks.h"
#include "lstLib.h"
#include "vxwErr.h"

class VXWList : virtual public LIST
    {
  public:

//_ VXWList Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::VXWList - initialize a list
//
// This constructor initializes a list as an empty list.
//
// RETURNS: N/A

    VXWList ()
	{
	lstInit (&list_);
	}

    VXWList & operator = (const VXWList &);

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::VXWList - initialize a list as a copy of another
// 
// This constructor builds a new list as a copy of an existing list.
// 
// RETURNS: N/A

    VXWList (const VXWList &);

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::~VXWList - free up a list
//
// This destructor frees up memory used for nodes.
//
// RETURNS: N/A

    ~VXWList ()
	{
	lstFree (&list_);
	}

//_ VXWList Public Member Functions

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::add - add a node to the end of list
//
// This routine adds a specified node to the end of the list.
//
// RETURNS: N/A

    void add (NODE *pNode)
	{
	lstAdd (&list_, pNode);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::concat - concatenate two lists
//
// This routine concatenates the specified list to the end of the current list.
// The specified list is left empty.  Either list (or both) can be
// empty at the beginning of the operation.
//
// RETURNS: N/A

    void concat (VXWList &aList)
	{
	lstConcat (&list_, &aList.list_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::count - report the number of nodes in a list
//
// This routine returns the number of nodes in a specified list.
//
// RETURNS:
// The number of nodes in the list.

    int count () const
	{
	return lstCount ((LIST *) &list_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::extract - extract a sublist from list
//
// This routine extracts the sublist that starts with <pStart> and ends
// with <pEnd>.  It returns the extracted list.
//
// RETURNS: The extracted sublist.

    LIST extract (NODE *pStart, NODE *pEnd)
	{
	LIST rVal;
	lstExtract (&list_, pStart, pEnd, &rVal);
	return rVal;
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::find - find a node in list
//
// This routine returns the node number of a specified node (the 
// first node is 1).
//
// RETURNS:
// The node number, or
// ERROR if the node is not found.

    int find (NODE *pNode) const
	{
	return lstFind ((LIST *) &list_, pNode);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::first - find first node in list
//
// This routine finds the first node in its list.
//
// RETURNS
// A pointer to the first node in the list, or
// NULL if the list is empty.

    NODE * first () const
	{
	return lstFirst ((LIST *) &list_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::get - delete and return the first node from list
//
// This routine gets the first node from its list, deletes the node
// from the list, and returns a pointer to the node gotten.
//
// RETURNS
// A pointer to the node gotten, or
// NULL if the list is empty.

    NODE * get ()
	{
	return lstGet (&list_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::insert - insert a node in list after a specified node
//
// This routine inserts a specified node into the list.
// The new node is placed following the list node <pPrev>.
// If <pPrev> is NULL, the node is inserted at the head of the list.
//
// RETURNS: N/A

    void insert (NODE *pPrev, NODE *pNode)
	{
	lstInsert (&list_, pPrev, pNode);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::last - find the last node in list
//
// This routine finds the last node in its list.
//
// RETURNS
// A pointer to the last node in the list, or
// NULL if the list is empty.

    NODE * last () const
	{
	return lstLast ((LIST *) &list_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::next - find the next node in list
//
// This routine locates the node immediately following a specified node.
//
// RETURNS:
// A pointer to the next node in the list, or
// NULL if there is no next node.

    NODE * next (NODE *pNode) const
	{
	return lstNext (pNode);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::nStep - find a list node <nStep> steps away from a specified node
//
// This routine locates the node <nStep> steps away in either direction from 
// a specified node.  If <nStep> is positive, it steps toward the tail.  If
// <nStep> is negative, it steps toward the head.  If the number of steps is
// out of range, NULL is returned.
//
// RETURNS:
// A pointer to the node <nStep> steps away, or
// NULL if the node is out of range.

    NODE * nStep (NODE *pNode, int nStep) const
	{
	return lstNStep (pNode, nStep);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::nth - find the Nth node in a list
//
// This routine returns a pointer to the node specified by a number <nodeNum>
// where the first node in the list is numbered 1.
// Note that the search is optimized by searching forward from the beginning
// if the node is closer to the head, and searching back from the end
// if it is closer to the tail.
//
// RETURNS:
// A pointer to the Nth node, or
// NULL if there is no Nth node.

    NODE * nth (int nodeNum) const
	{
	return lstNth ((LIST *) &list_, nodeNum);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::previous - find the previous node in list
//
// This routine locates the node immediately preceding the node pointed to 
// by <pNode>.
//
// RETURNS:
// A pointer to the previous node in the list, or
// NULL if there is no previous node.

    NODE * previous (NODE *pNode) const
	{
	return lstPrevious (pNode);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWList::remove - delete a specified node from list
//
// This routine deletes a specified node from its list.
//
// RETURNS: N/A

    void remove (NODE *pNode)
	{
	lstDelete (&list_, pNode);
	}
  protected:
    LIST list_;
    };

#endif /* ifndef vxwLstLib_h */
