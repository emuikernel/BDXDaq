h50466
s 00000/00000/00114
d D 1.4 97/06/05 11:58:19 jhardie 5 4
c fixed some small problems with smartsockets arrays
e
s 00000/00000/00114
d D 1.3 97/06/05 10:40:44 jhardie 4 3
c Version 1.0 of info_server and support programs
e
s 00057/00013/00057
d D 1.2 97/06/02 18:37:22 jhardie 3 1
c Rewrite parser for full C++.  Info messages input ok,
c output still broken
e
s 00000/00000/00000
d R 1.2 97/03/25 09:45:06 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 3 2 info_server/server/linked_list.hh
c Name history : 2 1 server/linked_list.hh
c Name history : 1 0 s/server/linked_list.hh
e
s 00070/00000/00000
d D 1.1 97/03/25 09:45:05 jhardie 1 0
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
#ifndef LINKED_LIST_H_J
#define LINKED_LIST_H_J
//**********************************************************************
// linked_list.h
//
// Definitions for a linked list class.  Defines basic list node and
D 3
// list classes
E 3
I 3
// list classes.  Note: the linked list class is a template for objects
// derived from class <node>.  That is, all the classes that you use to
// instantiate the linked list must be publically derived from class node.
E 3
//
//**********************************************************************

#ifndef NULL
#define NULL 0
#endif NULL

I 3
typedef enum { false, true } bool;



E 3
class node
{
private:
    node *next_node;			// next link in the list
    node *prev_node;			// the previous link in the list

I 3
    char *name;

E 3
public:

    // methods for list traversal.
    node *next();
    node *prev();

    // methods for list setup.
    node *setn(node *n);
    node *setp(node *p);
    node *set(node *n, node *p);

    // constructor
    node(node *n=NULL, node *p=NULL);

D 3
    // default destructor works fine.
E 3
I 3
    // We use a virtual destructor that does nothing - it doesn't
    // need to and we want to enable any derived classes to override
    // the destructor.  This will allow the node to be destroyed even
    // if it goes out of scope when it is known as a (node *)t rather
    // than a (derived_from_node *)t.
    virtual ~node() 
    { 
	if (name) delete [] name; 
	next_node = 0; 
	prev_node = 0; 
    };
E 3
};


D 3
class linkedlist
E 3
I 3
template<class T> class linkedlist
E 3
{
private:
D 3
    node *current_node;
    node *head;
    node *tail;
E 3
I 3
    T *current_node;
    T *head;
    T *tail;
E 3

public:
    
    // methods for list traversal
    bool set_top();
D 3
    node *current();
    node *previous();
E 3
I 3
    T *current();
    T *previous();
E 3
    
    // list insertion
D 3
    bool insert(node *n, bool athead=true);
E 3
I 3
    bool insert(T *n, bool athead=true);
E 3
    
    // list deletion
D 3
    node *remove(bool athead=true);
E 3
I 3
    T *remove(bool athead=true);
E 3

    // list lookup
D 3
    // node *findnode(node *n);
E 3
I 3
    T *findnode(char *n);
E 3

    // constructor
D 3
    linkedlist(node *n=NULL);
E 3
I 3
    linkedlist(T *n=0);
E 3

D 3
    // we use the default destructor
E 3
I 3
    // The destructor walks the list and destroys each node
    // first.  This means that the linked list should either only
    // be used for dynamically allocated nodes or it should not
    // be destroyed until the program exits.  Freeing objects on
    // the stack while a function is still using them is very bad
    // form.
    ~linkedlist();

E 3
};


I 3
// This linked list is flexible enough to be a queue or a stack.
//class stack : linkedlist<node>
//{
//public:
//  inline bool push(node *s) { return insert(s,true); }
//  inline node *pop() { return remove(true); }
//  stack(node *s=0) : linkedlist(s) {}
//};
//
//class jqueue : linkedlist<node>
//{
//public:
//  inline bool enqueue(node *s) { return insert(s,true); }
//  inline node *dequeue() { return remove(false); }
//  jqueue(node *s=0) : linkedlist(s) {}
//};
E 3

I 3

E 3
#endif LINKED_LIST_H_J
I 3


E 3
E 1
