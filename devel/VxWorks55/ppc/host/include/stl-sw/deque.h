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

#ifndef __SGI_STL_DEQUE_H
#define __SGI_STL_DEQUE_H

#include <stddef.h>
# ifndef __SGI_STL_ALGOBASE_H
#  include <algobase.h>
# endif
# ifndef __SGI_STL_ALLOC_H
#include <alloc.h>
# endif

# if defined ( __STL_USE_ABBREVS )
#  define __deque_iterator         _dQ__It
#  define __deque_const_iterator   _dQ__cIt
# endif

__BEGIN_STL_NAMESPACE

inline size_t __deque_buf_size(size_t sz)
{
  return sz < 4096 ? size_t(4096 / sz) : size_t(1);
}

template <class T> struct __deque_iterator;
template <class T> struct __deque_const_iterator;
template <class T> struct __deque_data;

template <class T>
struct __deque_iterator_base 
# if defined ( __STL_DEBUG )
    : public __safe_base 
# endif
{
private:
  typedef __deque_iterator_base<T> self;
public:
  typedef T value_type;
  typedef value_type* pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef pointer* map_pointer;

  pointer current;
  pointer first;
  pointer last;
  map_pointer node;

  static size_type buffer_size() {
    return __deque_buf_size(sizeof(value_type)); }
    void construct(pointer x, map_pointer y) {
        current=x; first=*y; last=(*y + buffer_size()); node=y;
    }
    void construct() {
        current=0; first=0; last=0; node=0;
    }
    void construct(const self& x) {
        current=x.current; first=x.first; last=x.last; node=x.node;
    }
# if defined ( __STL_DEBUG )
  typedef const __deque_data<T>* base_ptr; 
  const self& get_iterator() const { return *this; }  
  const base_ptr owner() const {
      return base_ptr(__safe_base::owner());
  }
  __deque_iterator_base(const __safe_base* root, pointer x, map_pointer y) 
    : __safe_base(root) { construct(x,y);}
  __deque_iterator_base() : __safe_base(0), current(0), first(0), last(0), node(0) {}
# else
  __deque_iterator_base(pointer x, map_pointer y) 
    { construct(x,y);}
  __deque_iterator_base() : current(0), first(0), last(0), node(0) {}
# endif
  difference_type operator-(const self& x) const {
    return node == x.node 
      ? current - x.current 
      : difference_type(buffer_size() * (node - x.node - 1) +
                        (current - first) + (x.last - x.current));
  }
  void operator++() {
    __stl_debug_check(__check_advance(*this,1));    
    if (++current == last) {
      first = *(++node);
      current = first;
      last = first + buffer_size();
    }
  }
  void operator--() {
    __stl_debug_check(__check_advance(*this,-1));    
    if (current == first) {
      first = *(--node);
      last = first + buffer_size();
      current = last;
    }
    --current;
  }
  void operator+=(difference_type n) {
    __stl_debug_check(__check_advance(*this,n));    
    difference_type offset = n + (current - first);
    difference_type num_node_to_jump = offset >= 0
      ? offset / buffer_size()
      : -((-offset + (difference_type)buffer_size() - 1) / (difference_type)buffer_size());
    if (num_node_to_jump == 0)
      current += n;
    else {
      node = node + num_node_to_jump;
      first = *node;
      last = first + buffer_size();
      current = first + (offset - num_node_to_jump * buffer_size());
    }
  }

  bool operator==(const self& x) const {      
     __stl_debug_check(__check_same_owner(*this,x));    
     return current == x.current || 
      ((current == first || x.current == x.first) && 
       *this - x == 0);
  }
  bool operator!=(const self& x) const {return !(*this == x); }
  bool operator<(const self& x) const {
      __stl_debug_check(__check_same_owner(*this,x));    
      return (node == x.node) ? (current < x.current) : (node < x.node);
  }
};

template <class T>
struct __deque_iterator : public __deque_iterator_base<T> {
private:
  typedef __deque_iterator_base<T> super;
public:
  typedef __deque_iterator<T> iterator;
  typedef __deque_const_iterator<T> const_iterator;
  __deque_iterator() {}
# if defined ( __STL_DEBUG )
    __deque_iterator(const __safe_base* root, pointer x, map_pointer y) : 
        super(root,x,y) {}
# else
    __deque_iterator(pointer x, map_pointer y) : super(x,y) {}
# endif
  reference operator*() const { 
          __stl_debug_check(__check_dereferenceable(*this));    
          return *current; 
  }
  difference_type operator-(const iterator& x) const {
      return super::operator-(x);
  }
  iterator& operator++() {
    super::operator++();
    return *this; 
  }
  iterator operator++(int)  {
    iterator tmp = *this;
    ++*this;
    return tmp;
  }
  iterator& operator--() {
    super::operator--();
    return *this;
  }
  iterator operator--(int) {
    iterator tmp = *this;
    --*this;
    return tmp;
  }
  iterator& operator+=(difference_type n) {
    super::operator+=(n);
    return *this;
  }
  iterator& operator-=(difference_type n) { return *this += -n; }
  iterator operator+(difference_type n) const {
    iterator tmp = *this;
    return tmp += n;
  }
  iterator operator-(difference_type n) const {
    iterator tmp = *this;
    return tmp -= n;
  }
  reference operator[](difference_type n) const { return *(*this + n); }
  bool operator==(const iterator& x) const {      
      return super::operator==(x);
  }
  bool operator!=(const iterator& x) const { return !(*this == x); }
  bool operator<(const iterator& x) const {
     return super::operator<(x);
  }
};


template <class T>
struct __deque_const_iterator : public __deque_iterator_base<T> {
private:
    typedef __deque_iterator_base<T> super;
public:
  typedef __deque_iterator<T> iterator;
  typedef __deque_const_iterator<T> const_iterator;
  __deque_const_iterator() {}
# if defined ( __STL_DEBUG )
    __deque_const_iterator(const __safe_base* root, pointer x, map_pointer y) : 
        super(root,x,y) {}
    __deque_const_iterator(const iterator& x) : 
        super(x) {}     
# else
    __deque_const_iterator(pointer x, map_pointer y) : super(x,y) {}
    __deque_const_iterator(const iterator& x) : 
        super(x) {}     
# endif
  const_reference operator*() const { 
    return *current;
  }
  difference_type operator-(const const_iterator& x) const {
      return super::operator-(x);
  }
  const_iterator& operator++() {
    super::operator++();
    return *this; 
  }
  const_iterator operator++(int)  {
    const_iterator tmp = *this;
    ++*this;
    return tmp;
  }
  const_iterator& operator--() {
    super::operator--();
    return *this;
  }
  const_iterator operator--(int) {
    const_iterator tmp = *this;
    --*this;
    return tmp;
  }
  const_iterator& operator+=(difference_type n) {
    super::operator+=(n);
    return *this;
  }
  const_iterator& operator-=(difference_type n) { return *this += -n; }
  const_iterator operator+(difference_type n) const {
    const_iterator tmp = *this;
    return tmp += n;
  }
  const_iterator operator-(difference_type n) const {
    const_iterator tmp = *this;
    return tmp -= n;
  }
  const_reference operator[](difference_type n) const { return *(*this + n); }
  bool operator==(const const_iterator& x) const {      
      return super::operator==(x);
  }
  bool operator!=(const const_iterator& x) const {return !(*this == x); }
  bool operator<(const const_iterator& x) const {
      return super::operator<(x);
  }
};


template <class T>
inline random_access_iterator_tag
iterator_category(const __deque_iterator<T>&) {
  return random_access_iterator_tag();
}

template <class T>
inline T*
value_type(const __deque_iterator<T>&) {
  return (T*) 0;
}

template <class T>
inline ptrdiff_t*
distance_type(const __deque_iterator<T>&) {
  return (ptrdiff_t*) 0;
}

template <class T>
inline random_access_iterator_tag
iterator_category(const __deque_const_iterator<T>&) {
  return random_access_iterator_tag();
}

template <class T>
inline T*
value_type(const __deque_const_iterator<T>&) {
  return (T*) 0;
}

template <class T>
inline ptrdiff_t*
distance_type(const __deque_const_iterator<T>&) {
  return (ptrdiff_t*) 0;
}

template <class T>
struct __deque_data 
# if defined ( __STL_DEBUG )
    : public __safe_base 
# endif
{
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T** map_pointer;
protected:
    __deque_iterator<T> start;
    __deque_iterator<T> finish;
    size_type length;
    map_pointer map;
    size_type map_size;
public:
    __deque_data() : start(), finish(), length(0), map(0), map_size(0) {
            __stl_debug_do(safe_init(this));
            __stl_debug_do(start.safe_init(this));
            __stl_debug_do(finish.safe_init(this));
    }
    ~__deque_data() {
        __stl_debug_do(invalidate()); __stl_debug_do(start.invalidate()); 
        __stl_debug_do(finish.invalidate());
    }
# if defined (__STL_DEBUG)
public:
    inline bool dereferenceable(const __deque_iterator_base<T>& it) const {
        return (it!=finish);
    }
    inline bool valid_advance(const __deque_iterator_base<T>& it, difference_type n) const {
        difference_type new_pos((it-start)+n);
        return  (new_pos>=0 && new_pos <= (difference_type)length);
    }
    void invalidate_iterator(const __deque_iterator_base<T>& it) { 
        __invalidate_iterator(this,it,it);
    }
# endif
};

template <class T, class Alloc>
class __deque_base : public __deque_data <T> {
    typedef __deque_base<T,Alloc> self;
    typedef T value_type;
    typedef value_type* pointer;
    typedef size_t size_type;
    typedef Alloc allocator_type;
protected:
    static size_type buffer_size() {
        return __deque_buf_size(sizeof(value_type)); }
    static size_type init_map_size() {
        return __deque_buf_size(sizeof(pointer)); }
    void deallocate_at_begin();
public:
    typedef simple_alloc<value_type*, allocator_type> map_allocator;
    typedef simple_alloc<value_type, allocator_type> data_allocator;
    __deque_base() {}
    ~__deque_base() { clear(); }
    void pop_front() {
        destroy(start.current);
        ++start.current;
        --length;
        if ((length == 0) || start.current == start.last)
            deallocate_at_begin();
    }
    void clear() { while (length!=0) pop_front(); }
};

template <class T , class Alloc>
void __deque_base<T, Alloc>::deallocate_at_begin() {
	data_allocator::deallocate(*start.node++, buffer_size());
	if (length==0) {
		if (finish.current == finish.first) 
			data_allocator::deallocate(*start.node, buffer_size());
		start.construct();
		finish.construct();
		map_allocator::deallocate(map, map_size);
	} else
		start.construct(*start.node, start.node);
}


__BEGIN_STL_FULL_NAMESPACE
#  define deque __WORKAROUND_RENAME(deque)

template <class T, __DFL_TYPE_PARAM(Alloc,alloc) >
class deque : public __deque_base<T,Alloc> {
    typedef __deque_base<T, Alloc> super;
    typedef deque<T, Alloc> self;
public:
    typedef T value_type;
    typedef size_t size_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef ptrdiff_t difference_type;
    typedef __deque_iterator<T> iterator;
    typedef __deque_const_iterator<T> const_iterator;
    typedef reverse_iterator<const_iterator, value_type, const_reference, 
    difference_type>  const_reverse_iterator;
    typedef reverse_iterator<iterator, value_type, reference, difference_type>
    reverse_iterator; 
protected:	
    typedef pointer* map_pointer;
    void allocate_at_begin();
    void allocate_at_end();
    void deallocate_at_end();
public:
    deque() { }
    iterator begin() { return start; }
    const_iterator begin() const { return start; }
    iterator end() { return finish; }
    const_iterator end() const { return finish; }
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { 
        return const_reverse_iterator(end()); 
    }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { 
        return const_reverse_iterator(begin()); 
    }
    bool empty() const { return length == 0; }
    size_type size() const { return length; }
    size_type max_size() const { return size_type(-1); }
    reference operator[](size_type n) { return *(begin() + n); }
    const_reference operator[](size_type n) const { return *(begin() + n); }
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference back() { return *(end() - 1); }
    const_reference back() const { return *(end() - 1); }
private:

#  if defined (__STL_USE_EXCEPTIONS) 
    void push_back_cleanup(int steps_remaining);
    void push_front_cleanup(int steps_remaining, bool allocated_at_begin);
    class push_back_protector;
    friend class push_back_protector;
    class push_back_protector
    {
        typedef deque<T,Alloc> deque_type;
        deque_type *container;
        int steps_remaining;
    public:
        push_back_protector(deque_type* d) : container(d), steps_remaining(2) {}
        ~push_back_protector() { 
            if (steps_remaining) 
                container->push_back_cleanup(steps_remaining); 
        }
        void constructed() { steps_remaining = 1; }
        void done() { steps_remaining = 0; }
    };
	
    class push_front_protector;
    friend class push_front_protector;
    class push_front_protector
    {
        typedef deque<T,Alloc> deque_type;
        deque_type *container;
        int steps_remaining;
        bool allocated_at_begin;
    public:
        push_front_protector(deque_type* d, bool alloc_at_begin)
            : container(d), steps_remaining(2), allocated_at_begin(alloc_at_begin) {}
        ~push_front_protector() {
            if (steps_remaining )
                container->push_front_cleanup(steps_remaining, allocated_at_begin);
        }
        void constructed() { steps_remaining = 1; }
        void done() { steps_remaining = 0; }
    };
#  else	
    class push_front_protector;
    class push_front_protector
    {
    public:
        push_front_protector(void*, bool=bool()){}
        ~push_front_protector() {}
        void constructed() {}
        void done() {}
    };
    typedef push_front_protector push_back_protector;
#  endif

public:
    void push_back(const T& x) {
        if (empty()) allocate_at_end();
        push_back_protector protector(this);
        construct(finish.current, x);
        protector.constructed();
        ++finish.current;
        ++length;
        if (finish.current == finish.last) allocate_at_end();
        protector.done();
        __stl_debug_do(invalidate_all());
    }
    void push_front(const T& x) {
        bool alloc_at_begin = empty() || start.current == start.first;
        if (alloc_at_begin) allocate_at_begin();
        push_front_protector protector(this, alloc_at_begin);
        --start.current;
        construct(start.current, x);
        protector.constructed();
        ++length;
        if (finish.current == finish.last) allocate_at_end();
        protector.done();
        __stl_debug_do(invalidate_all());
    }
    void pop_front() {
        __stl_debug_do(invalidate_iterator(start));        
        super::pop_front();
    }
    void pop_back() {
        __stl_debug_do(invalidate_iterator(finish));
        if (finish.current == finish.first) deallocate_at_end();
        --finish.current;
        destroy(finish.current);
        --length; 
        if (empty()) deallocate_at_end();
    }
    void swap(deque<T, Alloc>& x) {
        __STL_NAMESPACE::swap(start, x.start);
        __STL_NAMESPACE::swap(finish, x.finish);
        __STL_NAMESPACE::swap(length, x.length);
        __STL_NAMESPACE::swap(map, x.map);
        __STL_NAMESPACE::swap(map_size, x.map_size);
        __stl_debug_do(swap_owners(x));
    }
    iterator insert(iterator position, const T& x);
    iterator insert(iterator position) { return insert(position, T()); }
    void insert(iterator position, size_type n, const T& x);
//  template <class Iterator> void insert(iterator position,
//                                        Iterator first, Iterator last);
    void insert(iterator position, const T* first, const T* last);
    void insert(iterator position, const_iterator first, const_iterator last);
    void erase(iterator position);
    void erase(iterator first, iterator last);    
    void resize(size_type new_size, const T& x) {
        if (new_size < size()) 
            erase(begin() + new_size, end());
        else
            insert(end(), new_size - size(), x);
    }
    void resize(size_type new_size) { resize(new_size, T()); }
public:
    deque(size_type n, const T& value) {
        insert(begin(), n, value);
    }
    explicit deque(size_type n) {
        insert(begin(), n, T());
    }
//  template <class Iterator> deque(Iterator first, Iterator last);
    deque(const T* first, const T* last) {
        copy(first, last, back_inserter(*this));
    }
    deque(const_iterator first, const_iterator last) {
        copy(first, last, back_inserter(*this));
    }
    deque(const self& x)  {
        copy(x.begin(), x.end(), back_inserter(*this));
    }
    self& operator=(const self& x) {
        if (this != &x) {
            if (size() >= x.size()) 
                erase(copy(x.begin(), x.end(), begin()), end());
            else 
                copy(x.begin() + size(), x.end(),
                     inserter(*this, copy(x.begin(), x.begin() + size(),
                                          begin())));
            __stl_debug_do(invalidate_all());
        }
        return *this;
    }
    ~deque() {}
};

# if defined ( __STL_NESTED_TYPE_PARAM_BUG )
// qualified references 
#   define __iterator__           __deque_iterator<T>
#   define iterator               __iterator__
#   define const_iterator         __deque_const_iterator<T> 
#  define size_type              size_t
# else
#  define __iterator__           deque<T,Alloc>::iterator
# endif

#  if defined (__STL_USE_EXCEPTIONS) 
template <class T , class Alloc>
inline void 
deque<T, Alloc>::push_front_cleanup(int steps_remaining, bool allocated_at_begin)
{
    if (steps_remaining == 1) {	// construct succeeded?
        destroy(start.current);
        --length; 
    }
    ++start.current;
    if (allocated_at_begin)
        deallocate_at_begin();
}

template <class T , class Alloc>
inline void 
deque<T, Alloc>::push_back_cleanup(int steps_remaining)
{
    if (steps_remaining == 1) {
        destroy(finish.current - 1);
        --length;
    }
    if (empty()) deallocate_at_end();
}

#  endif

template <class T , class Alloc>
void deque<T, Alloc>::allocate_at_begin() {
//    typedef typename self::map_allocator map_allocator;
    pointer p = data_allocator::allocate(buffer_size());
    __TRY {
        if (!empty()) {
            if (start.node == map) {
                difference_type i = finish.node - start.node;
                size_type old_map_size = map_size;
                map_pointer tmp = map_allocator::allocate((i+1)*2);
                map_size = (i+1)*2;
                // need not worry on pointers copy
                copy(start.node, finish.node + 1, tmp + map_size / 4 + 1);
                map = tmp;
                map_allocator::deallocate(map, old_map_size);
                map[map_size / 4] = p;
                start.construct(p + buffer_size(), map + map_size / 4);
                finish.construct(finish.current, map + map_size / 4 + i + 1);
            } else {
                *--start.node = p;
                start.construct(p + buffer_size(), start.node);
            }
        } else {
            size_type new_map_size = init_map_size();
            map = map_allocator::allocate(new_map_size);
            map_size = new_map_size;
            map[map_size / 2] = p;
            start.construct(p + buffer_size() / 2 + 1, map + map_size / 2);
            finish.construct(start);
        }
    }
#  if defined (__STL_USE_EXCEPTIONS) 
    catch(...) {
        data_allocator::deallocate(p, buffer_size());
        throw;
    }
#endif
}

template <class T , class Alloc>
void deque<T, Alloc>::allocate_at_end() {
//    typedef typename self::map_allocator map_allocator;
    pointer p = data_allocator::allocate(buffer_size());
    __TRY {
        if (!empty()) {
            if (finish.node == map + map_size - 1) {
                difference_type i = finish.node - start.node;
                size_type old_map_size = map_size;
                map_pointer tmp = map_allocator::allocate((i + 1) * 2);
                map_size = (i + 1) * 2;
                copy(start.node, finish.node + 1, tmp + map_size / 4);
                map_allocator::deallocate(map, old_map_size);
                map = tmp;
                map[map_size / 4 + i + 1] = p;
                start.construct(start.current, map + map_size / 4);
                finish.construct(p, map + map_size / 4 + i + 1);
            } else {
                *++finish.node = p;
                finish.construct(p, finish.node);
            }
        } else {
            size_type new_map_size = init_map_size();
            map = map_allocator::allocate(new_map_size);
            map_size = new_map_size;
            map[map_size / 2] = p;
            start.construct(p + buffer_size() / 2, map + map_size / 2);
            finish.construct(start);
        }
    }
#  if defined (__STL_USE_EXCEPTIONS) 
    catch(...) {
        data_allocator::deallocate(p, buffer_size());
        throw;
    }
#  endif
}

template <class T , class Alloc>
void deque<T, Alloc>::deallocate_at_end() {
    data_allocator::deallocate(*finish.node--, buffer_size());
    if (empty()) {
        start.construct();
        finish.construct();
        map_allocator::deallocate(map, map_size);
    } else
        finish.construct(*finish.node + buffer_size(), finish.node);
}

template <class T , class Alloc>
__iterator__ 
deque<T, Alloc>::insert(iterator position, const T& x) {
    __stl_verbose_assert(position.owner()==this,__STL_MSG_NOT_OWNER);    
    if (position == begin()) {
        push_front(x);
        return begin();
    } else if (position == end()) {
        push_back(x);
        return end() - 1;
    } else {
	difference_type index = position - begin();
	if ((size_type)index < length / 2) {
	    push_front(*begin());
	    copy(begin() + 2, begin() + index + 1, begin() + 1); 
	} else {
	    push_back(*(end() - 1));
	    copy_backward(begin() + index, end() - 2, end() - 1); 
        }
        *(begin() + index) = x;
        return begin() + index;
    }
}

template <class T , class Alloc>
void deque<T, Alloc>::insert(iterator position, size_type n, const T& x) {
    __stl_verbose_assert(position.owner()==this,__STL_MSG_NOT_OWNER);    
    difference_type index = position - begin();
    difference_type remainder = length - index;
    if (remainder > index) {
	if (n > (size_type)index) {
	    difference_type m = n - index;
	    while (m-- > 0) push_front(x);
	    difference_type i = index;
	    while (i--) push_front(*(begin() + n - 1));
	    fill(begin() + n, begin() + n + index, x);
	} else {
	    difference_type i = n;
	    while (i--) push_front(*(begin() + n - 1));
	    copy(begin() + n + n, begin() + n + index, begin() + n);
	    fill(begin() + index, begin() + n + index, x);
	}
    } else {
	difference_type orig_len = index + remainder;
	if (n > (size_type)remainder) {
	    difference_type m = n - remainder;
	    while (m-- > 0) push_back(x);
	    difference_type i = 0;
	    while (i < remainder) push_back(*(begin() + index + i++));
	    fill(begin() + index, begin() + orig_len, x);
	} else {
	    difference_type i = 0;
	    while ((size_type)i < n) push_back(*(begin() + orig_len - n + i++));
	    copy_backward(begin() + index, begin() + orig_len - n, 
			  begin() + orig_len);
	    fill(begin() + index, begin() + index + n, x);
	}
    }
}

template <class T , class Alloc>
void deque<T, Alloc>::insert(iterator position, const T* first, const T* last) {
    __stl_verbose_assert(position.owner()==this,__STL_MSG_NOT_OWNER);    
    __stl_debug_check(__check_range(first,last));    
    difference_type index = position - begin();
    difference_type remainder = length - index;
    size_type n = 0;
    distance(first, last, n);
    if (remainder > index) {
	if (n > (size_type)index) {
	    const T* m = last - index;
	    while (m != first) push_front(*--m);
	    difference_type i = index;
	    while (i--) push_front(*(begin() + n - 1));
	    copy(last - index, last, begin() + n);
	} else {
	    difference_type i = n;
	    while (i--) push_front(*(begin() + n - 1));
	    copy(begin() + n + n, begin() + n + index, begin() + n);
	    copy(first, last, begin() + index);
	}
    } else {
	difference_type orig_len = index + remainder;
	if (n > (size_type)remainder) {
	    const T* m = first + remainder;
	    while (m != last) push_back(*m++);
	    difference_type i = 0;
	    while (i < remainder) push_back(*(begin() + index + i++));
	    copy(first, first + remainder, begin() + index);
	} else {
	    difference_type i = 0;
	    while ((size_type)i < n) push_back(*(begin() + orig_len - n + i++));
	    copy_backward(begin() + index, begin() + orig_len - n, 
			  begin() + orig_len);
	    copy(first, last, begin() + index);
	}
    }
}

template <class T , class Alloc>
void deque<T, Alloc>::insert(iterator position, const_iterator first, const_iterator last) {
    __stl_verbose_assert(position.owner()==this,__STL_MSG_NOT_OWNER);    
    __stl_debug_check(__check_range(first,last));    
    difference_type index = position - begin();
    difference_type remainder = length - index;
    size_type n = 0;
    distance(first, last, n);
    if (remainder > index) {
	if (n > (size_type)index) {
	    const_iterator m = last - index;
	    while (m != first) push_front(*--m);
	    difference_type i = index;
	    while (i--) push_front(*(begin() + n - 1));
	    copy(last - index, last, begin() + n);
	} else {
	    difference_type i = n;
	    while (i--) push_front(*(begin() + n - 1));
	    copy(begin() + n + n, begin() + n + index, begin() + n);
	    copy(first, last, begin() + index);
	}
    } else {
	difference_type orig_len = index + remainder;
	if (n > (size_type)remainder) {
	    const_iterator m = first + remainder;
	    while (m != last) push_back(*m++);
	    difference_type i = 0;
	    while (i < remainder) push_back(*(begin() + index + i++));
	    copy(first, first + remainder, begin() + index);
	} else {
	    difference_type i = 0;
	    while ((size_type)i < n) push_back(*(begin() + orig_len - n + i++));
	    copy_backward(begin() + index, begin() + orig_len - n, 
			  begin() + orig_len);
	    copy(first, last, begin() + index);
	}
    }
}

template <class T , class Alloc>
void deque<T, Alloc>::erase(iterator position) {
    __stl_debug_check(__check_range(position,begin(), end()-1));    
    if (end() - position > position - begin()) {
        copy_backward(begin(), position, position + 1);
        pop_front();
    } else {
        copy(position + 1, end(), position);
        pop_back();
    }
}
	
template <class T , class Alloc>
void deque<T, Alloc>::erase(iterator first, iterator last) {
    __stl_debug_check(__check_range(first,last, start, finish));    
    difference_type n = last - first;
    if (end() - last > first - begin()) {
        copy_backward(begin(), first, last);
        while(n-- > 0) pop_front();
    } else   {
        copy(last, end(), first);
        while(n-- > 0) pop_back();
    }
}


# undef __iterator__
# undef iterator
# undef const_iterator
# undef size_type

// do a cleanup
# undef deque
__END_STL_FULL_NAMESPACE
# define __deque__ __FULL_NAME(deque)

# if !defined ( __STL_DEFAULT_TYPE_PARAM)
// provide a "default" deque adaptor
template <class T>
class deque : public __deque__<T,alloc>
{
    typedef deque<T> self;
public:
    typedef __deque__<T,alloc> super;
    __CONTAINER_SUPER_TYPEDEFS
    __IMPORT_SUPER_COPY_ASSIGNMENT(deque)
    deque() : super() { }
    explicit deque(size_type n, const T& value) : super(n, value) { }
    explicit deque(size_type n) : super(n) { }
    deque(const T* first, const T* last) : super(first, last) { }
    deque(const_iterator first, const_iterator last) : super(first, last) { }
    ~deque() { }
};

#  if defined (__STL_BASE_MATCH_BUG)
template <class T>
inline bool 
operator==(const deque<T>& x, const deque<T>& y) {
    typedef typename deque<T>::super super;
    return operator == ((const super&)x,(const super&)y);
}

template <class T>
inline bool 
operator<(const deque<T>& x, const deque<T>& y) {
    typedef typename deque<T>::super super;
    return operator < ((const super&)x,(const super&)y);
}
#  endif
# endif /* __STL_DEFAULT_TYPE_PARAM */

template <class T, class Alloc>
bool operator==(const __deque__<T, Alloc>& x, const __deque__<T, Alloc>& y) {
    return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T, class Alloc>
bool operator<(const __deque__<T, Alloc>& x, const __deque__<T, Alloc>& y) {
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

# if defined (__STL_CLASS_PARTIAL_SPECIALIZATION )
template <class T, class Alloc>
inline void swap(__deque__<T,Alloc>& a, __deque__<T,Alloc>& b) { a.swap(b); }
# endif

__END_STL_NAMESPACE

#endif /* __SGI_STL_DEQUE_H */

