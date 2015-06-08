/* factory.h - class declarations for the object factory */

/* Copyright 1993-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,05oct98,sn   wrote
*/

#include <vxWorks.h>
#include <iostream.h>
#include <string>
#include <typeinfo>
#include <map>

/* 
 * object_t hierarchy
 *
 *             object_t
 *                 |
 *    +------------+------------+
 *    |            |            |
 *  red_t        blue_t       green_t
 *
 */

struct object_t
    {
    virtual void method () {}
    };

struct red_t : object_t
    {
    };

struct blue_t : object_t
    {
    };

struct green_t : object_t
    {
    };

/*
 * object_factory_t hierarchy
 *
 *  
 *                    object_factory_t
 *                         |
 *    +--------------------+--------------------+
 *    |                    |                    |
 *  red_factory_t      blue_factory_t     green_factory_t

 */

struct object_factory_t
    {
    virtual object_t* create () = 0;
    };

struct red_factory_t : object_factory_t
    {
    red_t* create () { return new red_t; }
    };

struct blue_factory_t : object_factory_t
    {
    blue_t* create () { return new blue_t; }
    };

struct green_factory_t : object_factory_t
    {
    green_t* create () { return new green_t; }
    };

/*
 * registry_t<T> - a registry of objects of type T
 *
 * The registry maps user readable names to pointers to objects.
 *
 */

template <class T> class registry_t
    {
private:
    typedef map <string, T*> map_t;
    map_t registry;
public:
    void insert (string objectName, T* pObject);
    T* lookup (string objectName);
    void list ();
    };

/* object_registry_t - a registry of objects derived from object_t */

typedef registry_t <object_t> object_registry_t;

/* class_registry_t - a registry of object factories ('classes') */

class class_registry_t : public registry_t <object_factory_t>
    {
public:
    object_t* create (string className) ;
    };
    
/*
 * template method definitions 
 *
 * It is common to put template method definitions in header
 * files so that they may be instantiated whenever necessary.
 *
 */

/******************************************************************************
*
* registry_t<T>::insert - register an object
*
* Register object pointed to by pObject under 'objectName'.
*
* RETURNS : N/A
*/

template <class T> 
void registry_t<T>::insert 
    (
    string objectName,
    T* pObject
    ) 
    { 
    registry [objectName] = pObject; 
    }

/******************************************************************************
*
* registry_t<T>::lookup - lookup an object by name
*
* Lookup 'objectName' in this registry and return a pointer
* to the corresponding object. 
*
* RETURNS : a pointer to an object or NULL
*/

template <class T> 
T* registry_t<T>::lookup 
    (
    string objectName
    ) 
    {
    return registry [objectName]; 
    }


/******************************************************************************
*
* registry_t<T>::list - list objects in this registry 
*
* RETURNS : N/A
*/

template <class T>
void registry_t<T>::list () 
    {
    cout << "Name \t" << "Address" << endl;
    cout << "=========================================================" << endl;
    for (map_t::iterator i = registry.begin (); 
         i != registry.end (); ++i) 
        {
        cout << i -> first << " \t" 
             << "0x" << hex << (int) i -> second << endl;
        }
    }

/* function declarations */

/* objectCreate - create an object of a given type */
object_t* objectCreate (char* className, char* objectName);

/* objectTypeShowByName - ascertain the type of a registered object */
void objectTypeShowByName (char* objectName);

/* objectRegistryShow - show contents of global object registry */
void objectRegistryShow ();

/* classRegistryShow - show contents of global class registry */
void classRegistryShow ();

