/*
 *
 * Copyright (c) 1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * Copyright (c) 1997
 * Moscow Center for SPARC Technology
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Moscow Center for SPARC Technology makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

#ifndef __SGI_STL_TYPE_TRAITS_H
#define __SGI_STL_TYPE_TRAITS_H

/*
This header file provides a framework for allowing compile time dispatch
based on type attributes. This is useful when writing template code.
For example, when making a copy of an array of an unknown type, it helps
to know if the type has a trivial copy constructor or not, to help decide
if a memcpy can be used.

The class template __type_traits provides a series of typedefs each of
which is either __true_type or __false_type. The argument to
__type_traits can be any type. The typedefs within this template will
attain their correct values by one of these means:
    1. The general instantiation contain conservative values which work
       for all types.
    2. Specializations may be __DECLAREd to make distinctions between types.
    3. Some compilers (such as the Silicon Graphics N32 and N64 compilers)
       will automatically provide the appropriate specializations for all
       types.

EXAMPLE:

//Copy an array of elements which have non-trivial copy constructors
template <class T> void copy(T* source,T* destination,int n,__false_type);
//Copy an array of elements which have trivial copy constructors. Use memcpy.
template <class T> void copy(T* source,T* destination,int n,__true_type);

//Copy an array of any type by using the most efficient copy mechanism
template <class T> inline void copy(T* source,T* destination,int n) {
   copy(source,destination,n,__type_traits<T>::has_trivial_copy_constructor());
}
*/

#include <bool.h>

// no namespaces there

struct __true_type {
};

struct __false_type {
};

template <class type>
struct __type_traits { 
   typedef __true_type     this_dummy_member_must_be_first;
                   /* Do not remove this member. It informs a compiler which
                      automatically specializes __type_traits that this
                      __type_traits template is special. It just makes sure that
                      things work if an implementation is using a template
                      called __type_traits for something unrelated. */

   /* The following restrictions should be observed for the sake of
      compilers which automatically produce type specific specializations 
      of this class:
          - You may reorder the members below if you wish
          - You may remove any of the members below if you wish
          - You must not rename members without making the corresponding
            name change in the compiler
          - Members you add will be treated like regular members unless
            you add the appropriate support in the compiler. */
 

   typedef __false_type    has_trivial_default_constructor;
   typedef __false_type    has_trivial_copy_constructor;
   typedef __false_type    has_trivial_assignment_operator;
   typedef __false_type    has_trivial_destructor;
   typedef __false_type    is_class_aggregate;
   typedef __false_type    is_POD_type;
    // fbp: additional fields for shallow copy semantics
   typedef __false_type    has_shallow_copy_constructor;
   typedef __false_type    has_shallow_destructor;
};

# if ! defined __STL_AUTOMATIC_TYPE_TRAITS

# define __BUILTIN_TRAITS_BODY(type)                                    \
struct __type_traits<type> {                                            \
   typedef __true_type     this_dummy_member_must_be_first;             \
   typedef __true_type     has_trivial_default_constructor;             \
   typedef __true_type     has_trivial_copy_constructor;                \
   typedef __true_type     has_trivial_assignment_operator;             \
   typedef __true_type     has_trivial_destructor;                      \
   typedef __false_type    is_class_aggregate;                          \
   typedef __true_type     is_POD_type;                                 \
   typedef __false_type    has_shallow_copy_constructor;                \
   typedef __false_type    has_shallow_destructor;                      \
}

# define __DECLARE_BUILTIN_TRAITS(type) \
 __STL_FULL_SPECIALIZATION __BUILTIN_TRAITS_BODY(type)

__DECLARE_BUILTIN_TRAITS(signed char);
__DECLARE_BUILTIN_TRAITS(signed short);
__DECLARE_BUILTIN_TRAITS(signed int);
__DECLARE_BUILTIN_TRAITS(signed long);

__DECLARE_BUILTIN_TRAITS(unsigned char);
__DECLARE_BUILTIN_TRAITS(unsigned short);
__DECLARE_BUILTIN_TRAITS(unsigned int);
__DECLARE_BUILTIN_TRAITS(unsigned long);

__DECLARE_BUILTIN_TRAITS(signed char*);
//__DECLARE_BUILTIN_TRAITS(signed short*);
//__DECLARE_BUILTIN_TRAITS(signed int*);
//__DECLARE_BUILTIN_TRAITS(signed long*);

__DECLARE_BUILTIN_TRAITS(unsigned char*);
//__DECLARE_BUILTIN_TRAITS(unsigned short*);
//__DECLARE_BUILTIN_TRAITS(unsigned int*);
//__DECLARE_BUILTIN_TRAITS(unsigned long*);

__DECLARE_BUILTIN_TRAITS(const signed char*);
//__DECLARE_BUILTIN_TRAITS(const signed short*);
//__DECLARE_BUILTIN_TRAITS(const signed int*);
//__DECLARE_BUILTIN_TRAITS(const signed long*);

__DECLARE_BUILTIN_TRAITS(const unsigned char*);
//__DECLARE_BUILTIN_TRAITS(const unsigned short*);
//__DECLARE_BUILTIN_TRAITS(const unsigned int*);
//__DECLARE_BUILTIN_TRAITS(const unsigned long*);

# if defined ( __STL_LONG_LONG )
__DECLARE_BUILTIN_TRAITS(signed long long);
__DECLARE_BUILTIN_TRAITS(unsigned long long);
//__DECLARE_BUILTIN_TRAITS(signed long long*);
//__DECLARE_BUILTIN_TRAITS(unsigned long long*);
//__DECLARE_BUILTIN_TRAITS(const signed long long*);
//__DECLARE_BUILTIN_TRAITS(const unsigned long long*);
# endif

# if defined ( __STL_WCHAR_T )
__DECLARE_BUILTIN_TRAITS(wchar_t);
//__DECLARE_BUILTIN_TRAITS(wchar_t*);
//__DECLARE_BUILTIN_TRAITS(const wchar_t*);
# endif

__DECLARE_BUILTIN_TRAITS(float);
__DECLARE_BUILTIN_TRAITS(double);
//__DECLARE_BUILTIN_TRAITS(float*);
//__DECLARE_BUILTIN_TRAITS(double*);
//__DECLARE_BUILTIN_TRAITS(const float*);
//__DECLARE_BUILTIN_TRAITS(const double*);

# if defined ( __STL_LONG_DOUBLE )
__DECLARE_BUILTIN_TRAITS(long double);
//__DECLARE_BUILTIN_TRAITS(long double*);
//__DECLARE_BUILTIN_TRAITS(const long double*);
# endif

# if defined (__STL_CLASS_PARTIAL_SPECIALIZATION)
template <class T>  __BUILTIN_TRAITS_BODY(T*)
# endif

# undef __BUILTIN_TRAITS_BODY
# undef __DECLARE_BUILTIN_TRAITS
 
# endif
#endif
