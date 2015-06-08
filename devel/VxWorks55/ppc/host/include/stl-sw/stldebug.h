/*
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
#ifndef __SGI_STL_STLDEBUG_H
# define __SGI_STL_STLDEBUG_H

# if defined ( __STL_DEBUG )
# ifndef __SGI_STL_ITERATOR_H
# include <iterator.h>
# endif
// stderr
# include <stdio.h>
# include <stdlib.h>

__BEGIN_STL_NAMESPACE

# define __STL_MSG_INVALID_ARGUMENT     "Invalid argument to operation (see operation documentation)"

# define __STL_MSG_INVALID_CONTAINER    "Taking an iterator out of destroyed (or otherwise corrupted) container"
# define __STL_MSG_EMPTY_CONTAINER      "Trying to extract an object out from empty container"
# define __STL_MSG_ERASE_PAST_THE_END   "Past-the-end iterator could not be erased"
# define __STL_MSG_OUT_OF_BOUNDS        "Index out of bounds"
# define __STL_MSG_NOT_OWNER            "Container doesn't own the iterator"
# define __STL_MSG_INVALID_ITERATOR     "Uninitialized or invalidated (by mutating operation)"\
       " iterator used"
# define __STL_MSG_INVALID_LEFTHAND_ITERATOR     "Uninitialized or invalidated (by mutating operation)"\
       " lefthand iterator in expression"
# define __STL_MSG_INVALID_RIGHTHAND_ITERATOR     "Uninitialized or invalidated (by mutating operation)"\
       " righthand iterator in expression"
# define __STL_MSG_DIFFERENT_OWNERS     "Iterators used in expression are from different owners"
# define __STL_MSG_NOT_DEREFERENCEABLE  "Iterator could not be dereferenced (past-the-end ?)"
# define __STL_MSG_INVALID_RANGE        "Range [first,last) is invalid"
# define __STL_MSG_NOT_IN_RANGE_1       "Iterator is not in range [first,last)"
# define __STL_MSG_NOT_IN_RANGE_2       "Range [first,last) is not in range [start,finish)"
# define __STL_MSG_INVALID_ADVANCE      "The advance would produce invalid iterator"

#  define __stl_verbose_check(expr,diagnostic) ((expr) ? true : \
(fprintf(stderr, "%s:%d STL error : %s\n", __FILE__, __LINE__, diagnostic ),false))

#  define __stl_verbose_return(expr,diagnostic) if (!(expr)) { \
    fprintf(stderr, "%s:%d STL error : %s\n", __FILE__, __LINE__, diagnostic ); return false; }

//============================================================================

template <class Iterator>
bool __check_same_owner( const Iterator& i1, const Iterator& i2)
{
    __stl_verbose_return(i1.valid(), __STL_MSG_INVALID_LEFTHAND_ITERATOR);
    __stl_verbose_return(i2.valid(), __STL_MSG_INVALID_RIGHTHAND_ITERATOR);
    __stl_verbose_return(i1.owner()==i2.owner(), __STL_MSG_DIFFERENT_OWNERS);
    return true;
}

template <class Iterator>
bool __check_if_owner( const void* owner, const Iterator& it)
{
    const void* owner_ptr = it.owner();
    __stl_verbose_return(owner_ptr!=0, __STL_MSG_INVALID_ITERATOR);
    __stl_verbose_return(owner==owner_ptr, __STL_MSG_NOT_OWNER);
    return true;
}

inline bool __check_bounds(int index, int size) 
{
    __stl_verbose_return(index<size, __STL_MSG_OUT_OF_BOUNDS);
    return true;
}

//=============================================================

template <class SafeIterator>
inline bool  __dereferenceable(const SafeIterator& it) { 
    return it.owner()->dereferenceable(it.get_iterator());
}

template <class Iterator>
inline bool __valid_range(const Iterator& i1 ,const Iterator& i2, 
                          random_access_iterator_tag)
{ 
    return i1<=i2;
}

template <class Iterator>
inline bool __valid_range(const Iterator& i1 ,const Iterator& i2, 
                          bidirectional_iterator_tag)
{ 
    // check if comparable
    bool dummy(i1==i2);
    return (dummy==dummy); 
}

template <class Iterator>
inline bool __valid_range(const Iterator& i1 ,const Iterator& i2, 
                          forward_iterator_tag)
{ 
    // check if comparable
    bool dummy(i1==i2);
    return (dummy==dummy); 
}

template <class Iterator>
inline bool __valid_range(const Iterator& i1 ,const Iterator& i2, 
                          input_iterator_tag)
{ 
    return false; 
}

template <class Iterator>
inline bool __valid_range(const Iterator& i1 ,const Iterator& i2, 
                          output_iterator_tag)
{ 
    return false; 
}

template <class Iterator>
inline bool __valid_range(const Iterator& i1, const Iterator& i2)
{ 
    return __valid_range(i1,i2,iterator_category(i1));
}

template <class Iterator>
inline bool __in_range(const Iterator& it, const Iterator& i1, const Iterator& i2)
{ 
    return __valid_range(i1,it,iterator_category(i1)) && 
        __valid_range(it,i2,iterator_category(it));
}

template <class Iterator>
inline bool __in_range(const Iterator& first, const Iterator& last, 
                       const Iterator& start, const Iterator& finish)
{ 
    return __valid_range(first,last,iterator_category(first)) &&
        __valid_range(start,first,iterator_category(first)) && 
        __valid_range(last,finish,iterator_category(last));
}

//=============================================================

template <class SafeIterator>
bool  __check_dereferenceable(const SafeIterator& it) { 
    __stl_verbose_return(it.valid(), __STL_MSG_INVALID_ITERATOR);
    __stl_verbose_return(__dereferenceable(it), __STL_MSG_NOT_DEREFERENCEABLE);
    return true;
}

template <class SafeIterator, class Distance>
bool __check_advance(const SafeIterator& it , Distance n )
{ 
    __stl_verbose_return(it.valid(), __STL_MSG_INVALID_ITERATOR);
    __stl_verbose_return(it.owner()->valid_advance(it.get_iterator(),n), 
                         __STL_MSG_INVALID_ADVANCE);
    return true;
}

template <class Iterator>
bool __check_range(const Iterator& first, const Iterator& last) {
    __stl_verbose_return(__valid_range(first,last), __STL_MSG_INVALID_RANGE );
    return true;
}

template <class Iterator>
bool __check_range(const Iterator& it, 
                   const Iterator& start, const Iterator& finish) {
    __stl_verbose_return(__in_range(it,start, finish), 
                         __STL_MSG_NOT_IN_RANGE_1);
    return true;
}

template <class Iterator>
bool __check_range(const Iterator& first, const Iterator& last, 
                   const Iterator& start, const Iterator& finish) {
    __stl_verbose_return(__in_range(first, last, start, finish), 
                         __STL_MSG_NOT_IN_RANGE_2);
    return true;
}
//===============================================================

// template <class Container>
class __safe_base {
protected:
    typedef void* owner_ptr;
    typedef __safe_base self;
    typedef self* self_ptr;
public:
    __safe_base(const void* c) : 
        owner_(owner_ptr(c)) {
            attach();
        }
    const self* owner() const { return (const self*)owner_; }
    bool  valid() const               { return owner()!=0; }
    __safe_base(const self& rhs) : owner_(rhs.owner_) {
        attach();
    }
    self& operator=(const self& rhs) {
        if ( owner_ != rhs.owner_ ) {
            detach();
            owner_ = rhs.owner_;
            attach();
        }
        return *this;
    }
    ~__safe_base() {
        detach();
        owner_  = 0;
    }
    void invalidate() { owner_=0; }
    void attach() {
        if (owner()) {
            self* owner_node=(self*)owner();
            __stl_verbose_assert(owner_node->owner()!=0, 
                                 __STL_MSG_INVALID_CONTAINER);
            self* list_begin = owner_node->next_;
            next_=list_begin;
            prev_ = list_begin->prev_;
            (list_begin->prev_)->next_=this;
            list_begin->prev_=this;
        }   
    }
    void detach() {
        if (owner()) {
            prev_->next_ = next_ ;
            next_->prev_ = prev_ ;
            owner_=0;
        }
    }
    self* prev() { return prev_; }
    self* next() { return next_; }
    void safe_init(const void* c) { 
        owner_=owner_ptr(c); 
        next_=prev_=this;
    }
    __safe_base() {}
    // note : must be performed on roots
    void swap_owners(self& x) {
        owner_ptr tmp = owner_;
        self* position;
        for (position = x.next(); 
             position!=&x; position=position->next()) {
            position->owner_ = tmp;
        }
        tmp=x.owner_;
        for (position = next(); 
             position!=this; position=position->next()) {
            position->owner_ = tmp;
        }
        x.owner_=owner_;
        owner_=tmp;
    }
    void invalidate_all() {
        for (__safe_base* position = next(); 
             position!=this; position=position->next()) {
            position->invalidate();
        }
        next_=prev_=this;;
    }
    mutable owner_ptr         owner_;
protected:
    mutable self_ptr          next_;
    mutable self_ptr          prev_; 
};

//============================================================

// _only_ for randomaccess iterators
template <class Iterator, class SafeIterator>
void __invalidate_range(__safe_base* base, 
                        const Iterator& first,
                        const Iterator& last, const SafeIterator&)
{
    typedef __safe_base* base_ptr;
    typedef SafeIterator* safe_iterator_ptr;
    base_ptr list_root = base;
    for (base_ptr position = list_root->next(); 
         position!=list_root;) {
        if (__in_range(((safe_iterator_ptr)position)->get_iterator(),first,last)) {
            base_ptr remove_position=position;
            position=position->next();
            remove_position->detach();
        }
        else
            position=position->next();
    }
}

template <class Iterator, class SafeIterator>
void __invalidate_iterator(__safe_base* base, 
                           const Iterator& it, const SafeIterator&)
{
    typedef __safe_base* base_ptr;
    typedef SafeIterator* safe_iterator_ptr;
    base_ptr list_root = base;
    for (base_ptr position = list_root->next(); 
         position!=list_root;) {
        if (((safe_iterator_ptr)position)->get_iterator()==it) {
            base_ptr remove_position=position;
            position=position->next();
            remove_position->detach();
        }
        else
            position=position->next();
    }
}
//============================================================

__END_STL_NAMESPACE

# endif /* __STL_DEBUG */

#endif



