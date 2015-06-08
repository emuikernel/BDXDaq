/*
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

#ifndef __SGI_STL_HASH_MAP_H
#define __SGI_STL_HASH_MAP_H

#ifndef __SGI_STL_HASHTABLE_H
#include <hashtable.h>
#endif /* SGI_STL_HASHTABLE_H */

__BEGIN_STL_NAMESPACE
__BEGIN_STL_FULL_NAMESPACE
# define  hash_map      __WORKAROUND_RENAME(hash_map)
# define  hash_multimap __WORKAROUND_RENAME(hash_multimap)

template <class Key, class T, __DFL_TMPL_PARAM(HashFcn,hash<Key>),
          __DFL_TMPL_PARAM(EqualKey,equal_to<Key>),
          __DFL_TYPE_PARAM(Alloc,alloc) >
class hash_map
{
private:
  typedef hashtable<pair<const Key, T>, Key, HashFcn,
      select1st<pair<const Key, T> >, EqualKey, Alloc> ht;
  typedef hash_map<Key, T, HashFcn, EqualKey, Alloc> self;
public:
  __IMPORT_CONTAINER_TYPEDEFS(ht)
  __IMPORT_ITERATORS(ht)
  typedef typename ht::key_type key_type;
  typedef typename ht::hasher hasher;
  typedef typename ht::key_equal key_equal;
  typedef T data_type;
  typedef typename ht::pointer pointer;
  typedef typename ht::const_pointer const_pointer;
private:
  ht rep;

public:
  hasher hash_funct() const { return rep.hash_funct(); }
  key_equal key_eq() const { return rep.key_eq(); }

public:
  hash_map() : rep(100, hasher(), key_equal()) {}
  hash_map(size_type n) : rep(n, hasher(), key_equal()) {}
  hash_map(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
  hash_map(size_type n, const hasher& hf, const key_equal& eql)
    : rep(n, hf, eql) {}

  hash_map(const value_type* f, const value_type* l)
    : rep(100, hasher(), key_equal()) { rep.insert_unique(f, l); }
  hash_map(const value_type* f, const value_type* l, size_type n)
    : rep(n, hasher(), key_equal()) { rep.insert_unique(f, l); }
  hash_map(const value_type* f, const value_type* l, size_type n,
           const hasher& hf)
    : rep(n, hf, key_equal()) { rep.insert_unique(f, l); }
  hash_map(const value_type* f, const value_type* l, size_type n,
           const hasher& hf, const key_equal& eql)
    : rep(n, hf, eql) { rep.insert_unique(f, l); }

  hash_map(const_iterator f, const_iterator l)
    : rep(100, hasher(), key_equal()) { rep.insert_unique(f, l); }
  hash_map(const_iterator f, const_iterator l, size_type n)
    : rep(n, hasher(), key_equal()) { rep.insert_unique(f, l); }
  hash_map(const_iterator f, const_iterator l, size_type n,
           const hasher& hf)
    : rep(n, hf, key_equal()) { rep.insert_unique(f, l); }
  hash_map(const_iterator f, const_iterator l, size_type n,
           const hasher& hf, const key_equal& eql)
    : rep(n, hf, eql) { rep.insert_unique(f, l); }

public:
  size_type size() const { return rep.size(); }
  size_type max_size() const { return rep.max_size(); }
  bool empty() const { return rep.empty(); }
  void swap(self& hs) { rep.swap(hs.rep); }
  friend bool operator==(const hash_map<Key,T,HashFcn,EqualKey,Alloc>&,
                         const hash_map<Key,T,HashFcn,EqualKey,Alloc>&);

  iterator begin() { return rep.begin(); }
  iterator end() { return rep.end(); }
  const_iterator begin() const { return rep.begin(); }
  const_iterator end() const { return rep.end(); }

public:
  pair<iterator, bool> insert(const value_type& obj)
    { return rep.insert_unique(obj); }
  void insert(const value_type* f, const value_type* l) { rep.insert_unique(f,l); }
  void insert(const_iterator f, const_iterator l) { rep.insert_unique(f, l); }
  pair<iterator, bool> insert_noresize(const value_type& obj)
    { return rep.insert_unique_noresize(obj); }    

  iterator find(const key_type& key) { return rep.find(key); }
  const_iterator find(const key_type& key) const { return rep.find(key); }

  T& operator[](const key_type& key)
  {
      value_type val(key, T());
      return rep.find_or_insert(val).second;
  }

  size_type count(const key_type& key) const { return rep.count(key); }
  
  pair<iterator, iterator> equal_range(const key_type& key)
    { return rep.equal_range(key); }
  pair<const_iterator, const_iterator> equal_range(const key_type& key) const
    { return rep.equal_range(key); }

  size_type erase(const key_type& key) {return rep.erase(key); }
  void erase(iterator it) { rep.erase(it); }
  void erase(iterator f, iterator l) { rep.erase(f, l); }
  void clear() { rep.clear(); }

public:
  void resize(size_type hint) { rep.resize(hint); }
  size_type bucket_count() const { return rep.bucket_count(); }
  size_type max_bucket_count() const { return rep.max_bucket_count(); }
  size_type elems_in_bucket(size_type n) const
    { return rep.elems_in_bucket(n); }
};


template <class Key, class T, __DFL_TMPL_PARAM(HashFcn,hash<Key>),
          __DFL_TMPL_PARAM(EqualKey,equal_to<Key>),
          __DFL_TYPE_PARAM(Alloc,alloc) >
class hash_multimap
{
private:
  typedef hashtable<pair<const Key, T>, Key, HashFcn,
      select1st<pair<const Key, T> >, EqualKey, Alloc> ht;
  typedef hash_multimap<Key, T, HashFcn, EqualKey, Alloc> self;
public:
  __IMPORT_CONTAINER_TYPEDEFS(ht)
  __IMPORT_ITERATORS(ht)
  typedef typename ht::key_type key_type;
  typedef typename ht::hasher hasher;
  typedef typename ht::key_equal key_equal;
  typedef T data_type;
  typedef typename ht::pointer pointer;
  typedef typename ht::const_pointer const_pointer;

  hasher hash_funct() const { return rep.hash_funct(); }
  key_equal key_eq() const { return rep.key_eq(); }
private:
  ht rep;

public:
  hash_multimap() : rep(100, hasher(), key_equal()) {}
  hash_multimap(size_type n) : rep(n, hasher(), key_equal()) {}
  hash_multimap(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
  hash_multimap(size_type n, const hasher& hf, const key_equal& eql)
    : rep(n, hf, eql) {}

  hash_multimap(const value_type* f, const value_type* l)
    : rep(100, hasher(), key_equal()) { rep.insert_equal(f, l); }
  hash_multimap(const value_type* f, const value_type* l, size_type n)
    : rep(n, hasher(), key_equal()) { rep.insert_equal(f, l); }
  hash_multimap(const value_type* f, const value_type* l, size_type n,
           const hasher& hf)
    : rep(n, hf, key_equal()) { rep.insert_equal(f, l); }
  hash_multimap(const value_type* f, const value_type* l, size_type n,
           const hasher& hf, const key_equal& eql)
    : rep(n, hf, eql) { rep.insert_equal(f, l); }

  hash_multimap(const_iterator f, const_iterator l)
    : rep(100, hasher(), key_equal()) { rep.insert_equal(f, l); }
  hash_multimap(const_iterator f, const_iterator l, size_type n)
    : rep(n, hasher(), key_equal()) { rep.insert_equal(f, l); }
  hash_multimap(const_iterator f, const_iterator l, size_type n,
           const hasher& hf)
    : rep(n, hf, key_equal()) { rep.insert_equal(f, l); }
  hash_multimap(const_iterator f, const_iterator l, size_type n,
           const hasher& hf, const key_equal& eql)
    : rep(n, hf, eql) { rep.insert_equal(f, l); }

public:
  size_type size() const { return rep.size(); }
  size_type max_size() const { return rep.max_size(); }
  bool empty() const { return rep.empty(); }
  void swap(self& hs) { rep.swap(hs.rep); }
  friend bool operator==(const hash_multimap<Key,T,HashFcn,EqualKey,Alloc>&,
                         const hash_multimap<Key,T,HashFcn,EqualKey,Alloc>&);

  iterator begin() { return rep.begin(); }
  iterator end() { return rep.end(); }
  const_iterator begin() const { return rep.begin(); }
  const_iterator end() const { return rep.end(); }

public:
  iterator insert(const value_type& obj) { return rep.insert_equal(obj); }
  void insert(const value_type* f, const value_type* l) { rep.insert_equal(f,l); }
  void insert(const_iterator f, const_iterator l) { rep.insert_equal(f, l); }
  iterator insert_noresize(const value_type& obj)
    { return rep.insert_equal_noresize(obj); }    

  iterator find(const key_type& key) { return rep.find(key); }
  const_iterator find(const key_type& key) const { return rep.find(key); }

  size_type count(const key_type& key) const { return rep.count(key); }
  
  pair<iterator, iterator> equal_range(const key_type& key)
    { return rep.equal_range(key); }
  pair<const_iterator, const_iterator> equal_range(const key_type& key) const
    { return rep.equal_range(key); }

  size_type erase(const key_type& key) {return rep.erase(key); }
  void erase(iterator it) { rep.erase(it); }
  void erase(iterator f, iterator l) { rep.erase(f, l); }
  void clear() { rep.clear(); }

public:
  void resize(size_type hint) { rep.resize(hint); }
  size_type bucket_count() const { return rep.bucket_count(); }
  size_type max_bucket_count() const { return rep.max_bucket_count(); }
  size_type elems_in_bucket(size_type n) const
    { return rep.elems_in_bucket(n); }
};

__END_STL_FULL_NAMESPACE

// do a cleanup
# undef hash_map
# undef hash_multimap

# define __hash_map__ __FULL_NAME(hash_map)
# define __hash_multimap__ __FULL_NAME(hash_multimap)

template <class Key, class T, class HashFcn, class EqualKey, class Alloc>
inline bool operator==(const __hash_map__<Key, T, HashFcn, EqualKey, Alloc>& hm1,
                       const __hash_map__<Key, T, HashFcn, EqualKey, Alloc>& hm2)
{
  return hm1.rep == hm2.rep;
}

template <class Key, class T, class HashFcn, class EqualKey, class Alloc>
inline bool operator==(const __hash_multimap__<Key, T, HashFcn, EqualKey, Alloc>& hm1,
                       const __hash_multimap__<Key, T, HashFcn, EqualKey, Alloc>& hm2)
{
  return hm1.rep == hm2.rep;
}

# if defined (__STL_CLASS_PARTIAL_SPECIALIZATION )
template <class Key, class T, class HashFcn, class EqualKey, class Alloc>
inline void swap(__hash_map__<Key, T, HashFcn, EqualKey, Alloc>& a,
                 __hash_map__<Key, T, HashFcn, EqualKey, Alloc>& b) { a.swap(b); }
template <class Key, class T, class HashFcn, class EqualKey, class Alloc>
inline void swap(__hash_multimap__<Key, T, HashFcn, EqualKey, Alloc>& a,
                 __hash_multimap__<Key, T, HashFcn, EqualKey, Alloc>& b) { a.swap(b); }
# endif

# ifndef __STL_DEFAULT_TYPE_PARAM
// provide a "default" hash_map adaptor
template <class Key, class T, class HashFcn,
          class EqualKey >
class hash_map : public __hash_map__<Key, T, HashFcn, EqualKey, alloc>
{
  typedef hash_map<Key, T, HashFcn, EqualKey> self;
public:
  typedef typename __hash_map__<Key, T, HashFcn, EqualKey, alloc> super;
  __IMPORT_CONTAINER_TYPEDEFS(super)
  typedef typename super::key_type key_type;
  typedef typename super::hasher hasher;
  typedef typename super::key_equal key_equal;
  typedef typename T data_type;
  typedef typename super::pointer pointer;
  typedef typename super::const_pointer const_pointer;
  hash_map() {}
  hash_map(size_type n) : super(n) {}
  hash_map(size_type n, const hasher& hf) : super(n, hf) {}
  hash_map(size_type n, const hasher& hf, const key_equal& eql): super(n, hf, eql) {}
  hash_map(const value_type* f, const value_type* l) : super(f,l) {}
  hash_map(const value_type* f, const value_type* l, size_type n): super(f,l,n) {}
  hash_map(const value_type* f, const value_type* l, size_type n, 
           const hasher& hf) : super(f,l,n,hf) {}
  hash_map(const value_type* f, const value_type* l, size_type n,
           const hasher& hf, const key_equal& eql) : super(f,l,n,hf, eql) {}
  hash_map(const_iterator f, const_iterator l) : super(f,l) { }
  hash_map(const_iterator f, const_iterator l, size_type n) : super(f,l,n) { }
  hash_map(const_iterator f, const_iterator l, size_type n,
           const hasher& hf) : super(f, l, n, hf) { }
  hash_map(const_iterator f, const_iterator l, size_type n,
           const hasher& hf, const key_equal& eql) : super(f, l, n, hf, eql) { }
  friend bool operator==(const self& hm1, const self& hm2);
};


template <class Key, class T, class HashFcn,
          class EqualKey >
inline bool operator==(const hash_map<Key, T, HashFcn,EqualKey>& hm1, 
                       const hash_map<Key, T, HashFcn,EqualKey>& hm2)
{
    typedef hash_map<Key, T, HashFcn,EqualKey>::super super;
    return (const super&)hm1 == (const super&)hm2; 
}

// provide a "default" hash_multimap adaptor
template <class Key, class T, class HashFcn,
          class EqualKey >
class hash_multimap : public __hash_multimap__<Key, T, HashFcn, EqualKey, alloc>
{
  typedef hash_multimap<Key, T, HashFcn, EqualKey> self;
public:
  typedef __hash_multimap__<Key, T, HashFcn, EqualKey, alloc> super;
  __IMPORT_CONTAINER_TYPEDEFS(super)
  typedef typename super::key_type key_type;
  typedef typename super::hasher hasher;
  typedef typename super::key_equal key_equal;
  typedef T data_type;
  typedef typename super::pointer pointer;
  typedef typename super::const_pointer const_pointer;
  hash_multimap() {}
  hash_multimap(size_type n) : super(n) {}
  hash_multimap(size_type n, const hasher& hf) : super(n, hf) {}
  hash_multimap(size_type n, const hasher& hf, const key_equal& eql): super(n, hf, eql) {}
  hash_multimap(const value_type* f, const value_type* l) : super(f,l) {}
  hash_multimap(const value_type* f, const value_type* l, size_type n): super(f,l,n) {}
  hash_multimap(const value_type* f, const value_type* l, size_type n, 
           const hasher& hf) : super(f,l,n,hf) {}
  hash_multimap(const value_type* f, const value_type* l, size_type n,
           const hasher& hf, const key_equal& eql) : super(f,l,n,hf, eql) {}

  hash_multimap(const_iterator f, const_iterator l) : super(f,l) { }
  hash_multimap(const_iterator f, const_iterator l, size_type n) : super(f,l,n) { }
  hash_multimap(const_iterator f, const_iterator l, size_type n,
           const hasher& hf) : super(f, l, n, hf) { }
  hash_multimap(const_iterator f, const_iterator l, size_type n,
           const hasher& hf, const key_equal& eql) : super(f, l, n, hf, eql) { }
  friend bool operator==(const self& hm1, const self& hm2);
};

template <class Key, class T, class HashFcn,
          class EqualKey >
inline bool operator==(const hash_multimap<Key, T, HashFcn,EqualKey>& hm1, 
                       const hash_multimap<Key, T, HashFcn,EqualKey>& hm2)
{
    typedef hash_multimap<Key, T, HashFcn,EqualKey>::super super;
    return (const super&)hm1 == (const super&)hm2; 
}

# endif /*  __STL_DEFAULT_TEMPLATE_PARAM */

__END_STL_NAMESPACE

#endif /* SGI_STL_HASH_MAP_H */
