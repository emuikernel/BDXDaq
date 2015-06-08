/* factory.cpp - implements an object factory */

/* Copyright 1993-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,05oct98,sn   wrote
*/

/*
DESCRIPTION

We implement an "object factory". The first step is to give
classes human-readable names by registering them with a 
"global class registry". Then objects of a named type may be 
created and registered with an "object registry". 

This gives us an opportunity to exercise various C++ features:

* Standard Template Library
    A "map" is used as the basis for the various registries.
* User defined templates
    The class registry and object registry are both based on a 
    generic registry type.
* Run Time Type Checking
    We provide a function to determine the type of a registered 
    object using "dynamic_cast".
* Exception Handling
    If we attempt to cast to the "wrong" type we have to handle a 
    C++ exception.

We provide a C interface to facilitate easy testing from the Wind Shell.

Here is an example test run (you can run this whole test through the function
testFactory()):

-> classRegistryShow
Showing Class Registry ...
Name 	Address
=========================================================
blue_t 	0x6b1c7a0
green_t 0x6b1c790
red_t 	0x6b1c7b0

-> objectRegistryShow
Showing Object Registry ...
Name 	Address
=========================================================

-> objectCreate "green_t", "bob"
Creating an object called 'bob' of type 'green_t'

-> objectCreate "red_t", "bill"
Creating an object called 'bill' of type 'red_t'

-> objectRegistryShow
Showing Object Registry ...
Name 	Address
=========================================================
bill 	0x6b1abf8
bob 	0x6b1ac18

-> objectTypeShowByName "bob"
Looking up object 'bob'
Attempting to ascertain type of object at 0x6b1ac18
Attempting a dynamic_cast to red_t ...
dynamic_cast threw an exception ... caught here!
Attempting a dynamic_cast to blue_t ...
dynamic_cast threw an exception ... caught here!
Attempting a dynamic_cast to green_t ...
Cast to green_t succeeded!
green.

*/

/* includes */
#include "factory.h"

/* locals */

/* pointer to the global class registry */
LOCAL class_registry_t* pClassRegistry;

/* pointer to the global object registry */ 
LOCAL object_registry_t* pObjectRegistry;

/******************************************************************************
*
* testFactory - an example test run
*
*/
void testFactory ()
{
    cout << "classRegistryShow ()" << endl;
    classRegistryShow ();
    cout << "objectRegistryShow ()" << endl;
    objectRegistryShow ();
    cout << "objectCreate (\"green_t\", \"bob\")" << endl;
    objectCreate ("green_t", "bob");
    cout << "objectCreate (\"red_t\", \"bill\")" << endl;
    objectCreate ("red_t", "bill");
    cout << "objectRegistryShow ()" << endl;
    objectRegistryShow ();
    cout << "objectTypeShowByName (\"bob\")" << endl;
    objectTypeShowByName ("bob");
}

/******************************************************************************
*
* class_registry_t::create - create an object of type className
*
* Look up 'className' in this registry. If it exists then
* create an object of this type by using the registered class factory;
* otherwise return NULL.  
*
* RETURNS : pointer to new object of type className or NULL.
*/

object_t* class_registry_t::create 
    (
    string className
    ) 
    { 
    object_factory_t* pFactory = lookup (className);
    if (pFactory != NULL)
        {
        return lookup (className)-> create (); 
        }
    else
        {
        cout << "No such class in Class Registry. " << endl;
        return NULL;
        }
    }

/******************************************************************************
*
* classRegistryGet - get a reference to the global class registry
* 
* Create and populate a new class registry if necessary.
*
* RETURNS : a reference to the global class registry
*/

LOCAL class_registry_t& classRegistryGet ()
    {
    if (pClassRegistry == NULL)
        {
        pClassRegistry = new class_registry_t;
        pClassRegistry -> insert ("red_t", new red_factory_t);
        pClassRegistry -> insert ("blue_t", new blue_factory_t);
        pClassRegistry -> insert ("green_t", new green_factory_t);
        }
    return *pClassRegistry;
    }

/******************************************************************************
*
* objectRegistryGet - get a reference to the global object registry
*
* Create a new object registry if necessary.
*
* RETURNS : a reference to the global object registry
*/

LOCAL object_registry_t& objectRegistryGet ()
    {
    if (pObjectRegistry == NULL)
        {
        pObjectRegistry = new object_registry_t;
        }
    return *pObjectRegistry;
    }

/******************************************************************************
*
* objectCreate - create an object of a given type
*
* Use the class factory registered in the global class registry
* under 'className' to create a new object. Register this object
* in the global object registry under 'object name'.
*
* RETURNS : object of type className
*/

object_t* objectCreate 
    (
    char* className, 
    char* objectName
    )
    {
    cout << "Creating an object called '" << objectName << "'" 
         << " of type '" << className << "'" << endl;
    object_t* pObject = classRegistryGet().create (className);
    if (pObject != NULL)
        {
        objectRegistryGet().insert(objectName, pObject);
        }
    else
        {
        cout << "Could not create object. Sorry. " << endl;
        }
    return pObject;
    }

/******************************************************************************
*
* isRed   
* isBlue   - is anObject a reference to an object of the specified type?
* isGreen 
*
* Try a dynamic_cast. If this succeeds then return TRUE. If it fails
* then catch the resulting exception and return FALSE.
*
* RETURNS : TRUE or FALSE
*/

/* isRed */

LOCAL BOOL isRed (object_t& anObject)
    {
    try
        {
	cout << "Attempting a dynamic_cast to red_t ..." << endl;
        dynamic_cast<red_t&> (anObject);
        cout << "Cast to red_t succeeded!" << endl;
        return TRUE;
        }
    catch (exception)
        {
        cout << "dynamic_cast threw an exception ... caught here!" << endl;
        return FALSE;
        }    
    }

/* isBlue */

LOCAL BOOL isBlue (object_t& anObject)
    {
    try
        {
	cout << "Attempting a dynamic_cast to blue_t ..." << endl;
        dynamic_cast<blue_t&> (anObject);
        cout << "Cast to blue_t succeeded!" << endl;
        return TRUE;
        }
    catch (exception)
        {
        cout << "dynamic_cast threw an exception ... caught here!" << endl;
        return FALSE;
        }    
    }

/* isGreen */

LOCAL BOOL isGreen (object_t& anObject)
    {
    try
        {
	cout << "Attempting a dynamic_cast to green_t ..." << endl;
        dynamic_cast<green_t&> (anObject);
        cout << "Cast to green_t succeeded!" << endl;
        return TRUE;
        }
    catch (exception)
        {
        cout << "dynamic_cast threw an exception ... caught here!" << endl;
        return FALSE;
        }    
    }


/******************************************************************************
*
* objectTypeShow - ascertain the type of an object
*
* Use dynamic type checking to determine the type of an object.
*
* RETURNS : N/A
*/

LOCAL void objectTypeShow (object_t* pObject)
    {
    cout << "Attempting to ascertain type of object at " << "0x" << hex 
         << (int) pObject << endl;
    if (isRed (*pObject))
        {
        cout << "red." << endl;
        }
    else if (isBlue (*pObject))
        {
        cout << "blue." << endl;
        }
    else if (isGreen (*pObject))
        {
        cout << "green." << endl;
        }
    }

/******************************************************************************
*
* objectTypeShowByName - ascertain the type of a registered object
*
* Lookup 'objectName' in the global object registry and 
* print the type of the associated object.
*
* RETURNS : N/A
*/

void objectTypeShowByName 
    (
    char* objectName
    )
    {
    cout << "Looking up object '" << objectName << "'" << endl;
    object_t *pObject = objectRegistryGet ().lookup (objectName);
    if (pObject != NULL)
        {
        objectTypeShow (pObject);
        }
    else
        {
        cout << "No such object in the Object Registry." << endl;
        }

    }

/******************************************************************************
*
* objectRegistryShow - show contents of global object registry
*
* RETURNS : N/A
*/

void objectRegistryShow ()
    {
    cout << "Showing Object Registry ..." << endl;
    objectRegistryGet ().list ();
    }

/******************************************************************************
*
* classRegistryShow - show contents of global class registry
*
* RETURNS : N/A
*/

void classRegistryShow ()
    {
    cout << "Showing Class Registry ..." << endl;
    classRegistryGet ().list ();
    }
