h55721
s 00000/00000/00000
d R 1.2 98/01/22 12:45:30 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 info_server/server/lltest.cc
c Name history : 1 0 server/lltest.cc
e
s 00138/00000/00000
d D 1.1 98/01/22 12:45:29 jhardie 1 0
c setup
e
u
U
f e 0
t
T
I 1
#include <stdio.h>
#include <iostream.h>

#ifndef NULL
#define NULL 0
#endif

typedef enum { false, true } bool;

#include "linked_list.hh"


class mynode : public node
{
private:
    char *name;

public:
    char *getname() { return name; }
    void setname(char *s) { name = s; }
    mynode(char *n = NULL) { name = n; }

    // overload the destructor.  This is a little wierd because
    // the names of the overloaded function are different.
    // But it works.
    ~mynode() { cout << "Destroying " << name << endl; }

};

class mylinkedlist : public linkedlist {
public:
    ~mylinkedlist() { cout << "Destroying linked list" << endl;}
};

int main(int argc, char **argv)
{
    mylinkedlist thelist, *ml;
    jqueue thequeue;
    stack thestack;
    mynode *m,*n1,*n2,*n3,*n4;
    node *n;
    int i;

    // try to build a list, inserting at the head.
    cout << "Building the list" << endl;
    thelist.insert(n1 = new mynode("Node 1"));
    thelist.insert(n2 = new mynode("Node 2"));
    thelist.insert(n3 = new mynode("Node 3"));
    thelist.insert(n4 = new mynode("Node 4"));

    // now reset the list to the top and traverse it.
    cout << "Trying to traverse the list" << endl;
    if (thelist.set_top())
    {
	i = 0;
	cout << "Able to set TOP_OF_LIST" << endl;
	while (m = (mynode *)thelist.current())
	{
	    cout << "Node " << ++i << " = :" << m->getname() << ":" << endl;
	}
	cout << "Done printing list" << endl;

	thelist.set_top();
	cout << "Doing a back-and-forth" << endl;
	for (i=0; i < 3; i++)
	{
	    m = (mynode *)thelist.current();
	    cout << "Node " << i+1 << " = :" << m->getname() << ":" << endl;
	}
	for (i--; i >= 0; i--)
	{
	    m = (mynode *)thelist.previous();
	    cout << "Node " << i+1 << " = :" << m->getname() << ":" << endl;
	}
	cout << "Done printing list" << endl;

    }
    else
    {
	cout << "WARNING - unable to set TOP_OF_LIST" << endl;
    }

    // next, destroy the list, removing nodes from the tail
    cout << "Trying to destroy the list" << endl;
    i = 0;
    while (m = (mynode *)thelist.remove(false))
    {
	cout << "Node " << ++i << " = :" << m->getname() << ":" << endl;
    }
    cout << "All gone" << endl;

    // Now test the stack class
    cout << "Testing the stack" << endl;

    thestack.push(n1);
    thestack.push(n2);
    thestack.push(n3);
    thestack.push(n4);
    for (i=0; i<4; i++)
    {
	cout << "Node: " << ((mynode *)thestack.pop())->getname() ;
	cout << endl;
    }

    // Now test the queue class
    cout << "Testing the queue" << endl;

    thequeue.enqueue(n1);
    thequeue.enqueue(n2);
    thequeue.enqueue(n3);
    thequeue.enqueue(n4);
    for (i=0; i<4; i++)
    {
	cout << "Node: " << ((mynode *)thequeue.dequeue())->getname() ;
	cout << endl;
    }

// reinsert all the nodes so that we can check the destructors
    ml = new mylinkedlist;

    cout << "Reinserting all nodes into list" << endl;
    ml->insert(n1);
    ml->insert(n2);
    ml->insert(n3);
    ml->insert(n4);

    delete ml;
    cout << "Waiting...";
    cin >> i;

    return 0;
}






E 1
