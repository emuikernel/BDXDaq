/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
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

#ifndef __SGI_STL_DEFALLOC_H
#define __SGI_STL_DEFALLOC_H

#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
# ifndef __SGI_STL_ALLOC_H 
#  include <alloc.h>
# endif

# ifndef __SGI_STL_ALGOBASE_H 
#  include <algobase.h>
# endif

__BEGIN_STL_NAMESPACE

// This file is obsolete; provided only for backward compatibility
// with code that use allocator<T>

template <class T>
inline T* allocate(size_t size, T*) {
    return 0 == size ? 0 : ::operator new(size*sizeof(T));
}

template <class T>
inline void deallocate(T* buffer) {
    ::operator delete(buffer);
}

template <class T>
inline void deallocate(T* buffer, size_t) {
    ::operator delete(buffer);
}

template <class T>
class allocator : public alloc {
    typedef alloc super;
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    static T* allocate(size_t n=1) { return (T*)super::allocate(n * sizeof(T));}
    static void deallocate(T *p, size_t n=1) { super::deallocate(p, n * sizeof(T)); }
    static pointer address(reference x) { return (pointer)&x; }
    static const_pointer address(const_reference x) { 
	return (const_pointer)&x; 
    }
    static size_type max_size() { 
        size_type sz((size_t)(-1)/sizeof(T));
        size_type msz(1);
	return max(msz, sz);
    }
    // CD2 requires that
    static T* allocate(size_t n, const void* ) { return (T*)super::allocate(n * sizeof(T));}
    void construct(pointer p, const value_type& val) { construct(p, val); }
    void destroy(pointer p) { destroy(p); }
};

template<class T1, class T2> inline
bool operator==(const allocator<T1>&, const allocator<T2>&) { return true; }
template<class T1, class T2> inline
bool operator!=(const allocator<T1>&, const allocator<T2>&) { return false; }

__STL_FULL_SPECIALIZATION class allocator<void> {
public:
    typedef void* pointer;
    typedef const void* const_pointer;
};

__END_STL_NAMESPACE

#endif
