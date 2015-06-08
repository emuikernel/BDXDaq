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

#ifndef __SGI_STL_LIST_H
#define __SGI_STL_LIST_H

#include <stddef.h>
# ifndef __SGI_STL_ALGOBASE_H
#include <algobase.h>
# endif
# ifndef __SGI_STL_ITERATOR_H
#  include <iterator.h>
# endif
# ifndef __SGI_STL_ALLOC_H
#  include <alloc.h>
# endif

# if defined ( __STL_USE_ABBREVS )
#  define __list_iterator         _L__It
#  define __list_const_iterator   _L__cIt
# endif

__BEGIN_STL_NAMESPACE

template <class T> struct __list_iterator;
template <class T> struct __list_const_iterator;

template <class T>
struct __list_node {
  typedef void* void_pointer;
  void_pointer next;
  void_pointer prev;
  T data;
};

template<class T>
# if defined ( __STL_DEBUG )
struct __list_iterator : public __safe_base {
# else
struct __list_iterator {
# endif
  typedef __list_iterator<T> iterator;
  typedef __list_const_iterator<T> const_iterator;
  typedef T value_type;
  typedef value_type* pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;    
  typedef __list_node<T>* link_type;

  link_type node;

# if defined ( __STL_DEBUG )
  link_type get_iterator() const { return node; }  
  link_type owner() const {
      const __safe_base* ptr = __safe_base::owner();
      return ptr ? link_type(ptr->owner_) : link_type(0); 
  }
  __list_iterator(const __safe_base* root, link_type x) : __safe_base(root), node(x) {}
  __list_iterator() : __safe_base(0) {}
# else
  __list_iterator(link_type x) : node(x) {}
  __list_iterator() {}
# endif
  bool operator==(const iterator& x) const { 
    __stl_debug_check(__check_same_owner(*this,x));                         
    return node == x.node; 
  }
  bool operator!=(const iterator& x) const { 
    __stl_debug_check(__check_same_owner(*this,x));                         
    return node != x.node; 
  }
  reference operator*() const { 
      __stl_verbose_assert(node!=owner(), __STL_MSG_NOT_DEREFERENCEABLE); 
      return (*node).data; 
  }
  iterator& operator++() { 
    __stl_verbose_assert(node!=owner(), __STL_MSG_INVALID_ADVANCE); 
    node = (link_type)((*node).next);
    return *this;
  }
  iterator operator++(int) { 
    iterator tmp = *this;
    ++*this;
    return tmp;
  }
  iterator& operator--() { 
    node = (link_type)((*node).prev);
    __stl_verbose_assert(node!=owner(), __STL_MSG_INVALID_ADVANCE); 
    return *this;
  }
  iterator operator--(int) { 
    iterator tmp = *this;
    --*this;
    return tmp;
  }
};

template<class T>
# if defined ( __STL_DEBUG )
struct __list_const_iterator : public __safe_base {
# else
struct __list_const_iterator {
# endif
  typedef __list_iterator<T> iterator;
  typedef __list_const_iterator<T> const_iterator;
  typedef T value_type;
  typedef value_type* pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;    
  typedef __list_node<T>* link_type;

  link_type node;

# if defined ( __STL_DEBUG )
  link_type owner() const {
      const __safe_base* ptr = __safe_base::owner();
      return ptr ? link_type(ptr->owner_) : link_type(0); 
  }
  link_type get_iterator() const { return node; }  
  __list_const_iterator(const __safe_base* root, link_type x) : __safe_base(root), node(x) {}
  __list_const_iterator(const iterator& x) : __safe_base(x), node(x.node) {}
  __list_const_iterator() : __safe_base(0) {}
# else
  __list_const_iterator(link_type x) : node(x) {}
  __list_const_iterator(const iterator& x) : node(x.node) {}
  __list_const_iterator() {}
# endif
  bool operator==(const const_iterator& x) const { 
      __stl_debug_check(__check_same_owner(*this,x));                         
      return node == x.node; 
  } 
  bool operator!=(const const_iterator& x) const { 
      __stl_debug_check(__check_same_owner(*this,x));                         
      return node != x.node; 
  } 
  const_reference operator*() const { 
      __stl_verbose_assert(node!=owner(), __STL_MSG_NOT_DEREFERENCEABLE); 
      return (*node).data; }
  const_iterator& operator++() { 
    __stl_verbose_assert(node!=owner(), __STL_MSG_INVALID_ADVANCE); 
    node = (link_type)((*node).next);
    return *this;
  }
  const_iterator operator++(int) { 
    const_iterator tmp = *this;
    ++*this;
    return tmp;
  }
  const_iterator& operator--() { 
    node = (link_type)((*node).prev);
    __stl_verbose_assert(node!=owner(), __STL_MSG_INVALID_ADVANCE); 
    return *this;
  }
  const_iterator operator--(int) { 
    const_iterator tmp = *this;
    --*this;
    return tmp;
  }
};

template <class T>
inline bidirectional_iterator_tag
iterator_category(const __list_iterator<T>&) {
  return bidirectional_iterator_tag();
}

template <class T>
inline T*
value_type(const  __list_iterator<T>&) {
  return (T*) 0;
}

template <class T>
inline ptrdiff_t*
distance_type(const  __list_iterator<T>&) {
  return (ptrdiff_t*) 0;
}

template <class T>
inline bidirectional_iterator_tag
iterator_category(const  __list_const_iterator<T>&) {
  return bidirectional_iterator_tag();
}

template <class T>
inline T*
value_type(const __list_const_iterator<T>&) {
  return (T*) 0;
}

template <class T>
inline ptrdiff_t*
distance_type(const __list_const_iterator<T>&) {
  return (ptrdiff_t*) 0;
}

template <class T, class Alloc>
class __list_base  {
    typedef __list_base<T,Alloc> self;
    typedef T value_type;
    typedef size_t size_type;	
    typedef __list_node<T> list_node;
    typedef list_node* link_type;
protected:
    typedef simple_alloc<list_node, Alloc> list_node_allocator;
    link_type node;
    size_type length;
public:
    __list_base() {
        node = get_node();
        (*node).next = node;
        (*node).prev = node;
        length=0;
        __stl_debug_do(iter_list.safe_init(node));
    }
	
    ~__list_base() {
        clear();
        put_node(node);
        __stl_debug_do(iter_list.invalidate());
    }
protected:    
    link_type get_node() { return list_node_allocator::allocate(); }
    void put_node(link_type p) { list_node_allocator::deallocate(p); }	
    void clear();
# if defined (__STL_DEBUG)
protected:
    friend class __safe_base;
    mutable __safe_base iter_list;
    void invalidate_all() { iter_list.invalidate_all();}
# endif
};

template <class T, class Alloc> 
void __list_base<T, Alloc>::clear()
{
  link_type cur = (link_type) node->next;
  while (cur != node) {
	link_type tmp = cur;
	cur = (link_type) cur->next;
	destroy(&(tmp->data));
	put_node(tmp);
  }
  __stl_debug_do(invalidate_all());
}

__BEGIN_STL_FULL_NAMESPACE
#  define  list  __WORKAROUND_RENAME(list)

template <class T, __DFL_TYPE_PARAM(Alloc,alloc) >
class list : protected __list_base<T, Alloc> {
    typedef __list_base<T, Alloc> super;
    typedef list<T, Alloc> self;
protected:
    typedef void* void_pointer;
public:      
    typedef T value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef __list_node<T> list_node;
    typedef list_node* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef __list_iterator<T>             iterator;
    typedef __list_const_iterator<T>       const_iterator;
    typedef reverse_bidirectional_iterator<const_iterator, value_type,
                                           const_reference, difference_type>
	const_reverse_iterator;
    typedef reverse_bidirectional_iterator<iterator, value_type, reference,
                                           difference_type>
        reverse_iterator; 

protected:
    link_type make_node(const T& x) {
        link_type tmp = get_node();
        __TRY {
            construct(&((*tmp).data), x);
        } 
#  if defined ( __STL_USE_EXCEPTIONS )
        catch(...) {
            put_node(tmp);
            throw;
        }
#  endif
        return tmp;
    }
public:
    list() {}
# if defined (__STL_DEBUG)
    iterator begin() { return iterator(&iter_list,(link_type)((*node).next)); }
    const_iterator begin() const { return const_iterator(&iter_list,(link_type)((*node).next)); }
    iterator end() { return iterator(&iter_list,node); }
    const_iterator end() const { return iterator(&iter_list,node); }
    void invalidate_iterator(const iterator& it) { 
        __invalidate_iterator(&iter_list, it.node, it); }
# else
    iterator begin() { return (link_type)((*node).next); }
    const_iterator begin() const { return (link_type)((*node).next); }
    iterator end() { return node; }
    const_iterator end() const { return node; }
# endif
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
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference back() { return *(--end()); }
    const_reference back() const { return *(--end()); }
    void swap(list<T, Alloc>& x) {
        __stl_debug_do(iter_list.swap_owners(x.iter_list));
	__STL_NAMESPACE::swap(node, x.node);
	__STL_NAMESPACE::swap(length, x.length);
    }
    iterator insert(iterator position, const T& x) {
        __stl_debug_check(__check_if_owner(node,position));
	link_type tmp = make_node(x);
	(*tmp).next = position.node;
	(*tmp).prev = (*position.node).prev;
	(*(link_type((*position.node).prev))).next = tmp;
	(*position.node).prev = tmp;
	++length;
#  if defined ( __STL_DEBUG )
	return iterator(&iter_list,tmp);
#  else
	return tmp;
#  endif
    }
    iterator insert(iterator position) { return insert(position, T()); }
    void insert(iterator position, const T* first, const T* last);
    void insert(iterator position, const_iterator first,
		const_iterator last);
    void insert(iterator position, size_type n, const T& x);
    void push_front(const T& x) { insert(begin(), x); }
    void push_back(const T& x) { insert(end(), x); }
    void erase(iterator position) {
        __stl_debug_check(__check_if_owner(node,position));
        __stl_verbose_assert(position.node!=node, __STL_MSG_ERASE_PAST_THE_END);
	(*(link_type((*position.node).prev))).next = (*position.node).next;
	(*(link_type((*position.node).next))).prev = (*position.node).prev;
	destroy(&(*position.node).data);
	put_node(position.node);
	--length;
        __stl_debug_do(invalidate_iterator(position));
    }
    void erase(iterator first, iterator last);
    void resize(size_type new_size, const T& x);
    void resize(size_type new_size) { resize(new_size, T()); }
    void clear();

    void pop_front() { erase(begin()); }
    void pop_back() { 
	iterator tmp = end();
	erase(--tmp);
    }
    explicit list(size_type n, const T& value) {
	insert(begin(), n, value);
    }
    explicit list(size_type n) {
	insert(begin(), n, T());
    }
    list(const T* first, const T* last) {
	insert(begin(), first, last);
    }
    list(const_iterator first, const_iterator last) {
	insert(begin(), first, last);
    }
    list(const self& x) {
	insert(begin(), x.begin(), x.end());
    }
    ~list() {}
    self& operator=(const self& x);

protected:
    void transfer(iterator position, iterator first, iterator last) {
      if (position.node != last.node) {
	(*(link_type((*last.node).prev))).next = position.node;
	(*(link_type((*first.node).prev))).next = last.node;
	(*(link_type((*position.node).prev))).next = first.node;  
	link_type tmp = link_type((*position.node).prev);
	(*position.node).prev = (*last.node).prev;
	(*last.node).prev = (*first.node).prev; 
	(*first.node).prev = tmp;
      }
    }

public:
    void splice(iterator position, list<T, Alloc>& x) {
        __stl_verbose_assert(&x!=this, __STL_MSG_INVALID_ARGUMENT);
        __stl_debug_check(__check_if_owner(node,position));
	if (!x.empty()) {
	    transfer(position, x.begin(), x.end());
	    length += x.length;
	    x.length = 0;
            __stl_debug_do(x.invalidate_all());
	}
    }
    void splice(iterator position, list<T, Alloc>& x, iterator i) {
        __stl_debug_check(__check_if_owner(node,position) &&
                          __check_if_owner(x.node ,i));
        __stl_verbose_assert(i.node!=i.owner(), __STL_MSG_NOT_DEREFERENCEABLE); 
	iterator j = i;
#  if defined ( __STL_DEBUG )
        ++j;
        if (( &x == this ) && (position == i || position == j)) return;
#  else
        if (position == i || position == ++j) return;
#  endif
	transfer(position, i, j);
	++length;
	--x.length;
        __stl_debug_do(x.invalidate_iterator(i));
    }
    void splice(iterator position, list<T, Alloc>& x, iterator first, iterator last) {
        __stl_debug_check(__check_if_owner(node, position));
        __stl_verbose_assert(first.owner()==x.node && last.owner()==x.node, 
                             __STL_MSG_NOT_OWNER);
	if (first != last) {
	    if (&x != this) {
		difference_type n = 0;
	    	distance(first, last, n);
	    	x.length -= n;
	    	length += n;
	    }
	    transfer(position, first, last);
            __stl_debug_do(x.invalidate_all());
	}
    }
    void remove(const T& value);
    void unique();
    void merge(list<T, Alloc>& x);
    void reverse();
    void sort();
};

# if defined (__STL_NESTED_TYPE_PARAM_BUG) 
#  define iterator       __list_iterator<T>
#  define const_iterator __list_const_iterator<T>
#  define size_type      size_t
# endif

template <class T, class Alloc>
INLINE_LOOP void list<T, Alloc>::insert(iterator position, const T* first, const T* last) {
    for (; first != last; ++first) insert(position, *first);
}
	 
template <class T, class Alloc>
INLINE_LOOP void list<T, Alloc>::insert(iterator position, const_iterator first,
		     const_iterator last) {
    for (; first != last; ++first) insert(position, *first);
}

template <class T, class Alloc>
INLINE_LOOP void list<T, Alloc>::insert(iterator position, size_type n, const T& x) {
    while (n--) insert(position, x);
}

template <class T, class Alloc>
INLINE_LOOP void list<T, Alloc>::erase(iterator first, iterator last) {
    while (first != last) erase(first++);
}

template <class T, class Alloc>
void list<T, Alloc>::resize(size_type new_size, const T& x)
{
  if (new_size < size()) {
    iterator f;
    if (new_size < size() / 2) {
      f = begin();
      advance(f, new_size);
    }
    else {
      f = end();
      advance(f, difference_type(size()) - difference_type(new_size));
    }
    erase(f, end());
  }
  else
    insert(end(), new_size - size(), x);
}

template <class T, class Alloc> 
void list<T, Alloc>::clear()
{
  super::clear();
  node->next = node;
  node->prev = node;
  length = 0;
}

template <class T, class Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(const list<T, Alloc>& x) {
    if (this != &x) {
	iterator first1 = begin();
	iterator last1 = end();
	const_iterator first2 = x.begin();
	const_iterator last2 = x.end();
	while (first1 != last1 && first2 != last2) *first1++ = *first2++;
	if (first2 == last2)
	    erase(first1, last1);
	else
	    insert(last1, first2, last2);
        __stl_debug_do(invalidate_all());
    }
    return *this;
}

template <class T, class Alloc>
void list<T, Alloc>::remove(const T& value) {
    iterator first = begin();
    iterator last = end();
    while (first != last) {
	iterator next = first;
	++next;
	if (*first == value) erase(first);
	first = next;
    }
}

template <class T, class Alloc>
void list<T, Alloc>::unique() {
    iterator first = begin();
    iterator last = end();
    if (first == last) return;
    iterator next = first;
    while (++next != last) {
	if (*first == *next)
	    erase(next);
	else
	    first = next;
	next = first;
    }
}

template <class T, class Alloc>
void list<T, Alloc>::merge(list<T, Alloc>& x) {
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = x.end();
    while (first1 != last1 && first2 != last2)
	if (*first2 < *first1) {
	    iterator next = first2;
	    transfer(first1, first2, ++next);
	    first2 = next;
	} else
	    ++first1;
    if (first2 != last2) transfer(last1, first2, last2);
    length += x.length;
    x.length= 0;
    __stl_debug_do(x.invalidate_all());
}

template <class T, class Alloc>
void list<T, Alloc>::reverse() {
    if (size() < 2) return;
    iterator first(begin());
    for (++first; first != end();) {       
	iterator old = first++;
	transfer(begin(), old, first);
    }
    __stl_debug_do(invalidate_all());
}    

template <class T, class Alloc>
void list<T, Alloc>::sort() {
    if (size() < 2) return;
    list<T, Alloc> carry;
    list<T, Alloc> counter[64];
    int fill = 0;
    while (!empty()) {
	carry.splice(carry.begin(), *this, begin());
	int i = 0;
	while(i < fill && !counter[i].empty()) {
	    counter[i].merge(carry);
	    carry.swap(counter[i++]);
	}
	carry.swap(counter[i]);         
	if (i == fill) ++fill;
    } 

    for (int i = 1; i < fill; ++i) counter[i].merge(counter[i-1]);
    swap(counter[fill-1]);
    __stl_debug_do(invalidate_all());
}

# if defined ( __STL_NESTED_TYPE_PARAM_BUG )
#  undef  iterator
#  undef  const_iterator
#  undef  size_type
# endif

// do a cleanup
# undef list
#  define __list__ __FULL_NAME(list)
__END_STL_FULL_NAMESPACE

# if !defined ( __STL_DEFAULT_TYPE_PARAM )
// provide a "default" list adaptor
template <class T>
class list : public __list__<T,alloc>
{
    typedef list<T> self;
public:
    typedef __list__<T,alloc> super;
    __CONTAINER_SUPER_TYPEDEFS
    __IMPORT_SUPER_COPY_ASSIGNMENT(list)
    typedef super::link_type link_type;
    list() { }
    explicit list(size_type n, const T& value) : super(n, value) { }
    explicit list(size_type n) :  super(n) { } 
    list(const T* first, const T* last) : super(first, last) { } 
    list(const_iterator first, const_iterator last) : super(first, last) { }
};

#  if defined (__STL_BASE_MATCH_BUG)
template <class T>
inline bool operator==(const list<T>& x, const list<T>& y) {
    typedef typename list<T>::super super;
    return operator == ((const super&)x,(const super&)y);
}

template <class T>
inline bool operator<(const list<T>& x, const list<T>& y) {
    typedef typename list<T>::super super;
    return operator < ((const super&)x,(const super&)y);
}
#  endif
# endif /*  __STL_DEFAULT_TEMPLATE_PARAM */

template <class T, class Alloc>
inline bool operator==(const __list__<T, Alloc>& x, const __list__<T, Alloc>& y) {
    return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T, class Alloc>
inline bool operator<(const __list__<T, Alloc>& x, const __list__<T, Alloc>& y) {
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

# if defined (__STL_CLASS_PARTIAL_SPECIALIZATION )
template <class T, class Alloc>
inline void swap(__list__<T,Alloc>& a, __list__<T,Alloc>& b) { a.swap(b); }
# endif

__END_STL_NAMESPACE

#endif /* __SGI_STL_LIST_H */
