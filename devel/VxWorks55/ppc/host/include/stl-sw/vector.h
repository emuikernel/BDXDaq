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
 *
 * Copyright (c) 1996
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
 * Exception Handling:
 * Copyright (c) 1997
 * Mark of the Unicorn, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Mark of the Unicorn makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * Adaptation:
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

#ifndef __SGI_STL_VECTOR_H
#define __SGI_STL_VECTOR_H

#include <stddef.h>
# ifndef __SGI_STL_ALGOBASE_H
#  include <algobase.h>
# endif
# ifndef __SGI_STL_ALLOC_H
#  include <alloc.h>
# endif

__BEGIN_STL_NAMESPACE

template <class T, class Alloc>
class __vector_base  
# if defined (__STL_DEBUG)
    : public __safe_base
# endif
{
    typedef __vector_base<T,Alloc> self;
public:	
    typedef T value_type;
    typedef size_t size_type;
    typedef T* pointer;
    typedef const T* const_pointer;
protected:
    typedef simple_alloc<value_type, Alloc> data_allocator;
    pointer start;
    pointer finish;
    pointer end_of_storage;

    void deallocate() {
        if (start) {
            __stl_debug_do(invalidate_all());
            data_allocator::deallocate(start, end_of_storage - start);
        }
    }
public:
    __vector_base() : start(0), finish(0), end_of_storage(0) {
        __stl_debug_do(safe_init(this));
    }
	
    __vector_base(size_type n) :
        start( data_allocator::allocate(n) ),
        finish(start),
        end_of_storage(start + n) {
        __stl_debug_do(safe_init(this));
    }

    ~__vector_base() { 
        destroy(start, finish);
        deallocate();
        __stl_debug_do(invalidate());
    }
# if defined (__STL_DEBUG)
public:
    bool dereferenceable(const value_type* it) const {
        return (it>=start && it < finish);
    }
    bool valid_advance(const value_type* iter, ptrdiff_t n) const {
        const value_type* advanced=iter+n;
        return advanced>=start && advanced <=finish;
    }
# endif
};

# if defined ( __STL_NESTED_TYPE_PARAM_BUG )
#  define __pointer__             T*
#  define __const_pointer__       const T*
#  define __size_type__           size_t
#  define __difference_type__     ptrdiff_t
# else
#  define __pointer__         pointer
#  define __const_pointer__   const_pointer
#  define __size_type__       size_type
#  define __difference_type__ difference_type
# endif

# if defined ( __STL_USE_ABBREVS )
#  define  __vector_iterator       _V__It
#  define  __vector_const_iterator _V__cIt
# endif

# if defined (__STL_DEBUG)
template <class T, class Alloc>
struct __vector_const_iterator;

template <class T, class Alloc>
struct __vector_iterator : public __safe_base {
private:
    typedef __vector_iterator<T,Alloc> self;
    typedef __safe_base super;
    typedef __vector_base<T,Alloc> container;
public:
    typedef T value_type;
    typedef value_type* iterator;
    typedef value_type& reference;
    typedef ptrdiff_t difference_type;
    iterator iterator_;
public:
    __vector_iterator() : super(0)  {}
    __vector_iterator(const super* c, const iterator& it) :
        super(c), iterator_(it) {}
    const container* owner() const { return (const container*)owner_; }
    iterator  get_iterator() const { return iterator_; }

    reference operator*() const {
        __stl_debug_check(__check_dereferenceable(*this));
        return *iterator_;
    }
    self& operator++() {
        __stl_debug_check(__check_advance(*this,1));
        ++iterator_;
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++iterator_;
        return tmp;
    }
    self& operator--() {
        __stl_debug_check(__check_advance(*this,-1));
        --iterator_;
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        --iterator_;
        return tmp;
    }
    bool operator<(const self& y)  const {
        __stl_debug_check(__check_same_owner(*this,y));
        return (get_iterator() < y.get_iterator());
    }
    difference_type operator-(const self& x) const {
        __stl_debug_check(__check_same_owner(*this,x));
        return get_iterator()-x.get_iterator();
    }
    self& operator+=(difference_type n) {
        __stl_debug_check(__check_advance(*this,n));
        iterator_+=n;
        return *this;
    }
    self& operator-=(difference_type n) { return *this += -n; }
    self operator+(difference_type n) const {
        self tmp = *this;
        return tmp += n;
    }
    self operator-(difference_type n) const {
        self tmp = *this;
        return tmp -= n;
    }
    reference operator[](difference_type n) const { return *(*this + n); }

    bool operator==(const self& y) const {
        __stl_debug_check(__check_same_owner(*this,y));
        return get_iterator() == y.get_iterator();
    }
    bool operator!=(const self& y) const {
        __stl_debug_check(__check_same_owner(*this,y));
        return get_iterator() != y.get_iterator();
    }
};

template <class T, class Alloc>
struct __vector_const_iterator : public __safe_base {
private:
    typedef __vector_const_iterator<T,Alloc> self;
    typedef __safe_base super;
    typedef __vector_base<T,Alloc> container;
public:
    typedef T value_type;
    typedef const value_type* const_iterator;
    typedef const value_type& const_reference;
    typedef ptrdiff_t difference_type;
    const_iterator iterator_;
public:                                     
    __vector_const_iterator() : super(0) {}
    __vector_const_iterator(const super* c, const_iterator it) :
        super(c), iterator_(it) {}
    __vector_const_iterator(const __vector_iterator<T,Alloc>& it) :
        super(it), iterator_(it.get_iterator()) {}
    const container* owner() const { return (const container*)owner_; }
    const_iterator  get_iterator() const { return iterator_; }

    const_reference operator*() const {
        __stl_debug_check(__check_dereferenceable(*this));
        return *iterator_;
    }
    self& operator++() {
        __stl_debug_check(__check_advance(*this,1));
        ++iterator_;
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++iterator_;
        return tmp;
    }
    self& operator--() {
        __stl_debug_check(__check_advance(*this,-1));
        --iterator_;
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        --iterator_;
        return tmp;
    }
    bool operator<(const self& y)  const {
        __stl_debug_check(__check_same_owner(*this,y));
        return (get_iterator() < y.get_iterator());
    }
    difference_type operator-(const self& x) const {
        __stl_debug_check(__check_same_owner(*this,x));
        return get_iterator()-x.get_iterator();
    }
    self& operator+=(difference_type n) {
        __stl_debug_check(__check_advance(*this,n));
        iterator_+=n;
        return *this;
    }
    self& operator-=(difference_type n) { return *this += -n; }
    self operator+(difference_type n) const {
        self tmp = *this;
        return tmp += n;
    }
    self operator-(difference_type n) const {
        self tmp = *this;
        return tmp -= n;
    }
    const_reference operator[](difference_type n) const { return *(*this + n); }

    bool operator==(const self& y) const {
        __stl_debug_check(__check_same_owner(*this,y));
        return get_iterator() == y.get_iterator();
    } 
    bool operator!=(const self& y) const {
        __stl_debug_check(__check_same_owner(*this,y));
        return get_iterator() != y.get_iterator();
    }
};

# define __iterator__       __vector_iterator<T,Alloc>
# define __const_iterator__ __vector_const_iterator<T,Alloc>
template <class T, class Alloc>
inline T* value_type(const  __iterator__&) { return (T*) 0; }
template <class T, class Alloc>
inline ptrdiff_t* distance_type(const  __iterator__&) { return (ptrdiff_t*) 0; }
template <class T, class Alloc>
inline random_access_iterator_tag
iterator_category(const __iterator__&) { return random_access_iterator_tag();}
template <class T, class Alloc>
inline T* value_type(const  __const_iterator__&) { return (T*) 0; }
template <class T, class Alloc>
inline ptrdiff_t* distance_type(const  __const_iterator__&) { return (ptrdiff_t*) 0; }
template <class T, class Alloc>
inline random_access_iterator_tag
iterator_category(const __const_iterator__&) { return random_access_iterator_tag();}
# else
#  define __iterator__       __pointer__
#  define __const_iterator__ __const_pointer__  
# endif

__BEGIN_STL_FULL_NAMESPACE
#  define vector __WORKAROUND_RENAME(vector)

template <class T, __DFL_TYPE_PARAM(Alloc,alloc)>
class vector : protected __vector_base<T, Alloc> {
    typedef __vector_base<T,Alloc> super;
    typedef vector<T,Alloc> self;
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef __iterator__ iterator;
    typedef __const_iterator__ const_iterator;
    typedef reverse_iterator<const_iterator, value_type, const_reference, 
            difference_type>  const_reverse_iterator;
    typedef reverse_iterator<iterator, value_type, reference, difference_type>
    reverse_iterator;
protected:
    void insert_aux(pointer position, const T& x);
public:
    pointer begin_() { return start; }
    const_pointer begin_() const { return start; }
    pointer end_() { return finish; }
    const_pointer end_() const { return finish; }
# if defined (__STL_DEBUG)
#   define __ptr(x) x.get_iterator()  
    iterator begin() { return iterator(this,start); }
    const_iterator begin() const { return const_iterator(this,start); }
    iterator end() { return iterator(this,finish); }
    const_iterator end() const { return const_iterator(this,finish); }
    void invalidate(const pointer& first, const pointer& last) {
        __invalidate_range(this, first, last, iterator());
    }
# else
#   define __ptr(x) x    
    iterator begin() { return start; }
    const_iterator begin() const { return start; }
    iterator end() { return finish; }
    const_iterator end() const { return finish; }
# endif
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { 
        return const_reverse_iterator(end()); 
    }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { 
        return const_reverse_iterator(begin()); 
    }
    size_type size() const { return size_type(end_() - begin_()); }
    size_type max_size() const { return size_type(-1)/sizeof(T); }
    size_type capacity() const { return size_type(end_of_storage - start); }
    bool empty() const { return begin_() == end_(); }
    reference operator[](size_type n) { 
        __stl_verbose_assert(n<size(), __STL_MSG_OUT_OF_BOUNDS);
        return *(start+n); 
    }
    const_reference operator[](size_type n) const { 
        __stl_verbose_assert(n<size(), __STL_MSG_OUT_OF_BOUNDS);
        return *(start+n); 
    }
    vector() {}
    vector(size_type n, const T& value) : super(n) {
        finish = uninitialized_fill_n(start, n, value);
    }
    explicit vector(size_type n) : super(n) {
        finish = __default_initialize_n(start, n);
    }
    vector(const self& x) : super(x.end_() - x.begin_()) {
        finish = uninitialized_copy(x.begin_(), x.end_(), start);
    }
    vector(const_iterator first, const_iterator last) {
        __stl_debug_check(__check_range(first,last));
        size_type n = 0;
        distance(__ptr(first), __ptr(last), n);
        start = finish = data_allocator::allocate(n);
        end_of_storage = start+n;
        finish = uninitialized_copy(__ptr(first), __ptr(last), start);        
    }
# if defined ( __STL_DEBUG )
    vector(const_pointer first, const_pointer last) {
        __stl_debug_check(__check_range(first,last));
        size_type n = last-first;
        start = finish = data_allocator::allocate(n);
        end_of_storage = start+n;
        finish = uninitialized_copy(first, last, start);
    }
# endif
    ~vector() {}
    self& operator=(const self& x);
    void reserve(size_type n) {
        const size_type old_size(size());
        if (capacity() < n) {
            pointer tmp = data_allocator::allocate(n);
            __TRY {
                uninitialized_copy(begin_(), end_(), tmp);
                destroy(start, finish);
                deallocate();
                finish = tmp + old_size;
                start = tmp;
                end_of_storage = begin_() + n;
            }
#  if defined (__STL_USE_EXCEPTIONS)
            catch(...) {
                data_allocator::deallocate(tmp, n);
                throw;
            }
#  endif
        }
    }
    reference front() { 
        __stl_verbose_assert(!empty(), __STL_MSG_EMPTY_CONTAINER);
        return *begin_(); 
    }
    const_reference front() const { 
        __stl_verbose_assert(!empty(), __STL_MSG_EMPTY_CONTAINER);
        return *begin_(); 
    }
    reference back() { 
        __stl_verbose_assert(!empty(), __STL_MSG_EMPTY_CONTAINER);
        return *(end_() - 1); 
    }
    const_reference back() const { 
        __stl_verbose_assert(!empty(), __STL_MSG_EMPTY_CONTAINER);
        return *(end_() - 1); 
    }
    void push_back(const T& x) {
        if (finish != end_of_storage) {
            construct(finish, x);
            ++finish;
        } else
            insert_aux(end_(), x);
    }
    void swap(self& x) {
        __stl_debug_do(swap_owners(x));
        __STL_NAMESPACE::swap(start, x.start);
        __STL_NAMESPACE::swap(finish, x.finish);
        __STL_NAMESPACE::swap(end_of_storage, x.end_of_storage);
    }
    iterator insert(iterator position, const T& x) {
        __stl_debug_check(__check_if_owner(this,position));
        size_type n = __ptr(position) - begin_();
        if (finish != end_of_storage && __ptr(position) == end_()) {
            construct(finish, x);
            ++finish;
        } else
            insert_aux(__ptr(position), x);
        return begin()+n;
    }
    iterator insert(iterator position) { return insert(position, T()); }
    void insert (iterator position, size_type n, const T& x);
    void insert (iterator position, const_iterator first, const_iterator last) 
# if defined ( __STL_DEBUG )
    {    
        __stl_debug_check(__check_same_owner(first,last));
        insert(position, __ptr(first), __ptr(last));
    }
    void insert (iterator position, const_pointer first, 
                 const_pointer last);
# else
    ;
# endif

    void pop_back() {
        __stl_verbose_assert(!empty(), __STL_MSG_EMPTY_CONTAINER);
        --finish;
        destroy(finish);
    }
    void erase(iterator position) {
        __stl_debug_check(__check_if_owner(this,position));
        __stl_verbose_assert(__ptr(position)!=finish,__STL_MSG_ERASE_PAST_THE_END);
        if (__ptr(position) + 1 != end_())
            copy(__ptr(position) + 1, end_(), __ptr(position));
        __stl_debug_do(invalidate(__ptr(position),finish));
        --finish;
        destroy(finish);
    }
    void erase(iterator first, iterator last) {
        __stl_debug_check(__check_if_owner(this,first)
                          &&__check_range(first,last));
        pointer i = copy(__ptr(last), end_(), __ptr(first));
        destroy(i, finish);
        __stl_debug_do(invalidate(__ptr(first),finish));
        finish = finish - (__ptr(last) - __ptr(first)); 
    }
    void resize(size_type new_size, const T& x) {
        if (new_size < size()) 
            erase(begin() + new_size, end());
        else
            insert(end(), new_size - size(), x);
    }
    void resize(size_type new_size) { resize(new_size, T()); }
    void clear() { erase(begin(), end()); }
};

template <class T, class Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(const vector<T, Alloc>& x) {
    if (&x == this) return *this;
    __stl_debug_do(invalidate_all());
    if (x.size() > capacity()) {
        destroy(start, finish);
        deallocate();
        start = finish = 0;
        start = finish = data_allocator::allocate(x.size());
        end_of_storage = start + (x.size());
        uninitialized_copy(x.begin_(), x.end_(), start);
    } else if (size() >= x.size()) {
        pointer i = copy(x.begin_(), x.end_(), begin_());
        destroy(i, finish);
    } else {
        copy(x.begin_(), x.begin_() + size(), begin_());
        uninitialized_copy(x.begin_() + size(), x.end_(), begin_() + size());
    }
    finish = begin_() + x.size();
    return *this;
}

template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(__pointer__ position, const T& x) {
    if (finish != end_of_storage) {
        construct(finish, *(finish - 1));
        ++finish;
        T x_copy = x;
        copy_backward(position, finish - 2, finish-1);
        *position = x_copy;
    } else {
        const size_type old_size = size();
        const size_type len = old_size != 0 ? 2 * old_size : 1;
        pointer tmp = data_allocator::allocate(len);
        pointer tmp_end = tmp;
        __TRY {
            tmp_end = uninitialized_copy(begin_(), position, tmp);
            construct(tmp_end, x);
            ++tmp_end;
            tmp_end = uninitialized_copy(position, end_(), tmp_end);
            destroy(begin_(), end_());
            deallocate();
            end_of_storage = tmp + len;
            finish = tmp_end;
            start = tmp;
        }
#  if defined (__STL_USE_EXCEPTIONS)
        catch(...) {
            destroy(tmp, tmp_end);
            data_allocator::deallocate(tmp, len);
            throw;
        }
#  endif
    }
}

template <class T, class Alloc>
# if defined ( __STL_DEBUG )
void vector<T, Alloc>::insert(__iterator__ pos, __size_type__ n, const T& x) {
    __stl_debug_check(__check_if_owner(this,pos));
    pointer position=__ptr(pos);
# else
void vector<T, Alloc>::insert(__iterator__ position, __size_type__ n, const T& x) {
# endif
    if (n == 0) return;
    if (end_of_storage - finish >= (difference_type)n) {
        pointer old_end = end_();
        size_type distance_to_end = end_() - position;
        if (distance_to_end > n) {
            uninitialized_copy(end_() - n, end_(), end_());
            finish += n;
            copy_backward(position, old_end - n, old_end);
            fill(position, position + n, x);
        } else {
            uninitialized_fill_n(end_(), n - distance_to_end, x);
            finish += n - distance_to_end;
            uninitialized_copy(position, old_end, end());
            finish += distance_to_end;
            fill(position, old_end, x);
        }
        __stl_debug_do(invalidate(position,old_end));
    } else {
        const size_type old_size = size();        
        const size_type len = old_size + max(old_size, n);
        pointer tmp = data_allocator::allocate(len);
        pointer tmp_end = tmp;
        __TRY {
            tmp_end = uninitialized_copy(begin_(), position, tmp);
            tmp_end = uninitialized_fill_n(tmp_end, n, x);
            tmp_end = uninitialized_copy(position, end_(), tmp_end);
            destroy(begin_(), end_());
            deallocate();
            end_of_storage = tmp + len;
            finish = tmp_end;
            start = tmp;
        } 
#  if defined (__STL_USE_EXCEPTIONS)
        catch(...) {
            destroy(tmp, tmp_end);
            data_allocator::deallocate(tmp, len);
            throw;
        }
#  endif
    }
}
    
template <class T, class Alloc>
# if defined ( __STL_DEBUG )
void vector<T, Alloc>::insert(__iterator__ pos, __const_pointer__ first,
                              __const_pointer__ last) {
    __stl_debug_check(__check_if_owner(this,pos)
                      &&__check_range(first,last));
    pointer position=__ptr(pos);
# else
void vector<T, Alloc>::insert(__iterator__ position, __const_iterator__ first,
                              __const_iterator__ last) {
# endif
    if (first == last) return;
    size_type n = 0;
    distance(first, last, n);
    if (end_of_storage - finish >= (difference_type)n) {
        pointer old_end = end_();
        size_type distance_to_end = end_() - position;
        if (end_() - position > (difference_type)n) {
            uninitialized_copy(end_() - n, end_(), end_());
            finish += n;
            copy_backward(position, old_end - n, old_end);
            copy(first, last, position);
        } else {
            uninitialized_copy(first + distance_to_end, last, end_());
            finish += n - distance_to_end;
            uninitialized_copy(position, old_end, end_());
            finish += distance_to_end;
            copy(first, first + distance_to_end, position);
        }
        __stl_debug_do(invalidate(position,old_end));
    } else {
        const size_type old_size = size();
        const size_type len = old_size + max(old_size, n);
        pointer tmp = data_allocator::allocate(len);
        pointer tmp_end = tmp;
        __TRY {
            tmp_end = uninitialized_copy(begin_(), position, tmp);
            tmp_end = uninitialized_copy(first, last, tmp_end);
            tmp_end = uninitialized_copy(position, end_(), tmp_end);
            destroy(begin_(), end_());
            deallocate();
            end_of_storage = tmp + len;
            finish = tmp_end;
            start = tmp;
        }
#  if defined (__STL_USE_EXCEPTIONS)
        catch(...) {
            destroy(tmp, tmp_end);
            data_allocator::deallocate(tmp, len);
            throw;
        }
#  endif
    }
}

// do a cleanup
# undef  vector
# undef  __iterator__
# undef  __const_iterator__
# undef  __pointer__
# undef  __const_pointer__
# undef  __size_type__
# undef  __ptr
# undef  __difference_type__

// provide a uniform way to access full funclionality
#  define __vector__ __FULL_NAME(vector)

__END_STL_FULL_NAMESPACE

# if ! defined (__STL_DEFAULT_TYPE_PARAM)
// provide a "default" vector adaptor
template <class T>
class vector : public __vector__<T,alloc>
{
    typedef vector<T> self;
public:
    typedef __vector__<T,alloc> super;
    __CONTAINER_SUPER_TYPEDEFS
    __IMPORT_SUPER_COPY_ASSIGNMENT(vector)
    vector() {}
    explicit vector(size_type n, const T& value) : super(n, value) { }
    explicit vector(size_type n) : super(n) { }
# if defined ( __STL_DEBUG )
    vector(const value_type* first, const value_type* last) : super(first,last) { }
# endif
    vector(const_iterator first, const_iterator last) : super(first,last) { }
    ~vector() {}
};

#  if defined (__STL_BASE_MATCH_BUG)
template <class T>
    inline bool operator==(const vector<T>& x, const vector<T>& y) {
    typedef  __vector__<T,alloc> super;
    return operator == ((const super&)x,(const super&)y);
}

template <class T>
    inline bool operator<(const vector<T>& x, const vector<T>& y) {
    typedef  __vector__<T,alloc> super;
    return operator < ((const super&)x,(const super&)y);
}
#  endif /* __STL_BASE_MATCH_BUG */
# endif /* __STL_DEFAULT_TEMPLATE_PARAM */

template <class T, class Alloc>
    inline bool operator==(const __vector__<T, Alloc>& x, const __vector__<T, Alloc>& y) {
    return x.size() == y.size() && equal(x.begin_(), x.end_(), y.begin_());
}

template <class T, class Alloc>
    inline bool operator<(const __vector__<T, Alloc>& x, const __vector__<T, Alloc>& y) {
    return lexicographical_compare(x.begin_(), x.end_(), y.begin_(), y.end_());
}

# if defined (__STL_CLASS_PARTIAL_SPECIALIZATION )
template <class T, class Alloc>
    inline void swap(__vector__<T,Alloc>& a, __vector__<T,Alloc>& b) { a.swap(b); }
# endif
// close std namespace
__END_STL_NAMESPACE

#endif
