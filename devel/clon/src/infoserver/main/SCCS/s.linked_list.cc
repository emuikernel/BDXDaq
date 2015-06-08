h42220
s 00000/00000/00258
d D 1.4 97/06/05 11:58:18 jhardie 5 4
c fixed some small problems with smartsockets arrays
e
s 00000/00000/00258
d D 1.3 97/06/05 10:40:43 jhardie 4 3
c Version 1.0 of info_server and support programs
e
s 00039/00014/00219
d D 1.2 97/06/02 18:37:21 jhardie 3 1
c Rewrite parser for full C++.  Info messages input ok,
c output still broken
e
s 00000/00000/00000
d R 1.2 97/03/25 09:45:05 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 3 2 info_server/server/linked_list.cc
c Name history : 2 1 server/linked_list.cc
c Name history : 1 0 s/server/linked_list.cc
e
s 00233/00000/00000
d D 1.1 97/03/25 09:45:04 jhardie 1 0
c New C++ version of info-server program.  Translation
c from C because of complexity of internal structures.
c These are better handled with classes.
e
u
U
f e 0
t
T
I 1
//**********************************************************************
// linked_list.cc
//
// methods and interpretations for a simple linked list class.  This
// linked list class is meant to be used as a member of other classes
// while the node class is meant to be used as a base class in other
// classes.  Thus you can have classes which "are" nodes and classes which
// "have" linked lists of nodes.
//
// The nodes contain methods for changing the "next" pointer and for
// returning the next pointer.
//
// The linked list class contains methods which allow list traversal, 
// list insertion (currently stack like, but eventually who knows), list
// lookup and list deletion.
//
//**********************************************************************

D 3
typedef enum { false, true } bool;

E 3
#include "linked_list.hh"

//----------------------------------------------------------------------
// Methods for the node class
//----------------------------------------------------------------------


// List traversal.
inline node *node::next()
{
    return next_node;
}

inline node *node::prev()
{
    return prev_node;
}

// Pointer definition - set the appropriate pointers and return
// the old ones.  set() returns the old value of next_node

inline node *node::setn(node *n)
{
    node *old = next_node;
    next_node = n;
    return old;
}

inline node *node::setp(node *p)
{
    node *old = prev_node;
    prev_node = p;
    return old;
}

inline node *node::set(node *n, node *p)
{
    node *old = next_node;
    next_node = n;
    prev_node = p;
    return old;
}

// Constructor
node::node(node *n, node *p)
{
    next_node = n;
    prev_node = p;
}


//----------------------------------------------------------------------
// Methods for class linkedlist
//----------------------------------------------------------------------

//****
// Set the list pointer to the first node in the list.  We get
// this by following the "next_node" pointer in the linkedlist node.
//
// If the list is empty (i.e. next_node == NULL) we return false so that
// no traversal will be done.
//****
D 3
bool linkedlist::set_top()
E 3
I 3
template<class T> bool linkedlist<T>::set_top()
E 3
{
    current_node = head;
    if (!current_node)
	return false;
    else
	return true;
}


//****
// Return a pointer to the current node in the list.  This also updates
// the current pointer to point to the next node in the list.
//
// If the current node is NULL we are either at the beginning of an
// empty list or at the end of a full one.  In either case, we return
// NULL and don't update the current_node pointer.
//
// This could also be implemented as a friend class and an iterator, but
// I thought this was simpler.
//****
D 3
node *linkedlist::current()
E 3
I 3
template<class T> T *linkedlist<T>::current()
E 3
{
D 3
    node *old = current_node;
E 3
I 3
    T *old = current_node;
E 3

    if (old)
    {
D 3
	current_node = old->next();
E 3
I 3
	current_node = (T *)old->next();
E 3
    }
    return old;
}


//****
// Return a pointer to the previous node.  We have to be a little careful
// here because the list might be empty.
//
// If the current_node is NULL we are either at the beginning of a full 
// list or the end of an empty one.  In either case, we return NULL and
// don't update the current_node variable.
//
// Otherwise, we return the previous node pointer and update the
// current_node variable, moving backwards in the list.
//
// Note: this is not exactly the inverse of the linkedlist::current
// function, but it is more useful for traversing a list in two directions
// since we don't get the same node returned twice.
//****
D 3
node *linkedlist::previous()
E 3
I 3
template<class T> T *linkedlist<T>::previous()
E 3
{
    if (current_node)
    {
	current_node = current_node->prev();
    }
    return current_node;
}


//****
// Insert the given node at either the head or the tail
// of the list.  If the second argument is true the new node will
// be inserted at the head of the list, otherwise it will be inserted
// at the back of the list.  The "head" and "tail" pointers are
// updated appropriately
//****
D 3
bool linkedlist::insert(node *n, bool athead)
E 3
I 3
template<class T> bool linkedlist<T>::insert(T *n, bool athead)
E 3
{
    // check for an empty list...
    if (!head)
    {
	head = tail = n;
	n->set(NULL,NULL);
    }
    else
    {
	if (athead)
	{
	    n->set(head,NULL);
	    head->setp(n);
	    head = n;
	}
	else
	{
	    n->set(NULL,tail);
	    tail->setn(n);
	    tail = n;
	}
    }
    return true;
}



//****
// Remove a node from either the head or tail of a list.  If the
// argument is true the node is removed from the head, otherwise
// it is removed from the tail.
//****
D 3
node *linkedlist::remove(bool athead)
E 3
I 3
template<class T> T *linkedlist<T>::remove(bool athead)
E 3
{
D 3
    node *old;
E 3
I 3
    T *old;
E 3

    // check for an empty list
    if (!head)
	return NULL;

    if (athead)
    {
	old = head;
D 3
	head = head->next();
E 3
I 3
	head = (T *)head->next();
E 3
	if (head) head->setp(NULL);
	if (!head)
	    tail = NULL;
    }
    else
    {
	old = tail;
D 3
	tail = tail->prev();
E 3
I 3
	tail = (T *)tail->prev();
E 3
	if (tail) tail->setn(NULL);
	if (!tail)
	    head = NULL;
    }

    old->set(NULL,NULL);	// don't allow links to be followed
    return old;
}


//****
// Construct a linked list.  This is easy enough.  If a pointer
// to a node is passed, this is used as the initial value of the
// list.  Not used often, but this allows the list to be initialized
// with a previously constructed list of nodes.
//****
D 3
linkedlist::linkedlist(node *n)
E 3
I 3
template<class T> linkedlist<T>::linkedlist(T *n)
E 3
{
    if (n)
    {
	head = tail = n;
	while (tail->next())
D 3
	    tail = tail->next();
E 3
I 3
	    tail = (T *)tail->next();
E 3
    }
    else
	head = tail = NULL;
}

I 3
// Search the linked list for a node with a particular name.
template<class T> T *linkedlist<T>::findnode(char *n)
{
    T *t;
E 3

I 3
    set_top();
    while (t = current())
    {
	if (!strcmp(t->name, n))
	    return t;
    }
    return 0;
}
E 3


I 3
// Destructor for the linked list.  Destroys all the nodes
// in the list first.  *WATCH OUT!* - THE LIST ASSUMES THAT IT
// OWNS ALL OBJECTS INSERTED IN IT.
template<class T> linkedlist<T>::~linkedlist()
{
    T *n;

    set_top();
    while (n = remove(true))
    {
	delete n;
    }
}


E 3


E 1
