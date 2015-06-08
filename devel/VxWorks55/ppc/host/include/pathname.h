/* pathname.h - pathname utility class */

/* Copyright (c) 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,21may98,dbs  fix comparison ops

01a,dbs,21may98  created.
*/

/*
  DESCRIPTION

  A C++ utility-class that implements pathnames for files, and
  overcomes the problems inherent in differences between Win32 and
  Unix, and provides conversion methods to return the path as either a
  DOS-style (backslashes) or a Unix-style (forward slashes) string. It
  maintains the path internally as a std::string and always holds the
  path with one kind of slash (fwd or back) depending on the usage.

  It uses templates to achieve this, and typedefs some useful Win32
  and Unix standard implementations called CPathname, a la MFC.

  The default for Win32 is to use Unix-style forward-slashes
  internally, as this helps with passing paths to Tcl (which is happy
  with either style, as long as it is consistent within the entire
  string) but avoids quoting complications.

  It can also handle wide-strings, by using the right template
  parameter (e.g. wstring) for the 'String' type.

  It can be constructed from a 'C' string, a std::string or from
  another CPathname.

  It provides += operations (but no + operations) accepting all 3
  kinds of arguments again, as with construction.

  It provides equality-tests against all 3 types of argument,
  converting them all to CPathname before comparison to ensure the
  slash-type does not interfere with equality.

  It provides explicit conversions to Unix and DOS formats. The DOS
  format uses single backslashes.

  It provides methods to extract the filename (without path), the
  file's extension (which returns "tar.gz" for a file called
  "foo.tar.gz"), the path (with trailing slash, for building other
  pathnames from), and the directory (without trailing slashes).

  It provides conversion operators for converting to raw char pointers
  (both const, and non-const for passing to Tcl functions) which
  precludes the use of tests like "if (path) ..." because the pointer
  conversions provide an implicit conversion to bool - pointer
  conversions are frowned upon in the C++ world for this reason, yet
  their utility is so great that I have included them for
  convenience. To test if a pathname has any data, use length ().

  It also provides a method to return a quoted-string rendition of the
  pathname, where the open and close quotation marks can be
  specified. They default to {} which is ideal for Tcl.
  
  */


#ifndef __PATHNAME_H__
#define __PATHNAME_H__

#include <string.h>			// std C strings
#include <string>			// std C++ strings

// case-insensitive compare
struct strcmp_ci
    {
    int operator () (const std::string &lhs, const std::string &rhs) const
	{
	return stricmp (lhs.c_str (), rhs.c_str ());
	}
    };

// case-sensitive compare
struct strcmp_cs
    {
    int operator () (const std::string &lhs, const std::string &rhs) const
	{
	return strcmp (lhs.c_str (), rhs.c_str ());
	}
    };

// wide case-insensitive compare
struct wstrcmp_ci
    {
    int operator () (const std::wstring &lhs, const std::wstring &rhs) const
	{
	return wcsicmp (lhs.c_str (), rhs.c_str ());
	}
    };

// wide case-sensitive compare
struct wstrcmp_cs
    {
    int operator () (const std::wstring &lhs, const std::wstring &rhs) const
	{
	return wcscmp (lhs.c_str (), rhs.c_str ());
	}
    };

// pathname class
template <class Char, class String, int Sep, class Strcmp> class pathname
    {
    String		txt;
    Strcmp		strcmp_;

    enum { DosSep = '\\', UnixSep = '/', DefSep = Sep };

    static const size_t NPOS;
    
    // convert to given slash-type - although declared as const,
    // this function does modify the internal string representation
    void slashify (int sep = DefSep) const
	{
	pathname& p = const_cast<pathname&> (*this);
	const int Other = (sep == DosSep ? UnixSep : DosSep);
	for (int n=0; n < length (); ++n)
	    {
	    if (p.txt [n] == Other)
		p.txt [n] = sep;
	    }
	}


  public:

    pathname (const String& s) : txt (s) { slashify (); }

    pathname (const Char *s) : txt (s) { slashify (); }

    pathname (const pathname& p) : txt (p.txt) {}

    pathname () {}

    ~pathname () {}

    size_t length () const { return txt.length (); }
    size_t size () const { return txt.size (); }

    // Equality tests...
    bool operator == (const pathname& rhs) const
	{ return (strcmp_ (txt, rhs.txt) == 0); }

    bool operator == (const String& rhs) const
	{ return (*this == pathname (rhs)); }

    bool operator == (const Char* rhs) const
	{ return (*this == pathname (rhs)); }

    // Assignments...
    pathname& operator = (const pathname& rhs)
	{ txt = rhs.txt; return *this; }

    pathname& operator = (const String& rhs)
	{ txt = rhs; slashify (); return *this; }

    pathname& operator = (const Char *rhs)
	{ txt = rhs; slashify (); return *this; }

    // Append operations...
    pathname& operator += (const pathname& rhs)
	{ txt += rhs.txt; return *this; }
    
    pathname& operator += (const String& rhs)
	{ txt += rhs; slashify (); return *this; }

    pathname& operator += (const Char *rhs)
	{ txt += rhs; slashify (); return *this; }

    // Ordering operations...
    bool operator < (const pathname& rhs) const
	{ return (txt < rhs.txt); }
    
    bool operator < (const String& rhs) const
	{ return (txt < rhs); }
    
    bool operator < (const Char *rhs) const
	{ return (txt < String (rhs)); }

    // Conversion to OS-specific formats...
    pathname as_unix () const
	{ pathname p (*this); p.slashify (UnixSep); return p; }

    pathname as_dos () const
	{ pathname p (*this); p.slashify (DosSep); return p; }

    // Return file name, with no path
    String filename () const
	{
	size_t n = txt.find_last_of (DefSep);
	if (n == NPOS)
	    return txt;
	return txt.substr (n+1, NPOS);
	}

    // Return file path, including trailing separator if one is present
    String path () const
	{
	size_t n = txt.find_last_of (DefSep);
	if (n == NPOS)
	    return "";
	return txt.substr (0, n+1);
	}

    // Return file directory, no trailing separator, unless its root
    String dir () const
	{
	size_t n = txt.find_last_of (DefSep);
	if (n == NPOS)
	    return "";
	if (n == 0)
	    ++n;
	return txt.substr (0, n);
	}

    // Return file's extension - this is defined as the string
    // following the *first* dot, hence file.tar.gz will return
    // "tar.gz" as its extension.
    String extension () const
	{
	String fn (filename ());
	size_t n = fn.find_first_of ('.');
	if (n == NPOS)
	    return "";
	return fn.substr (n+1, NPOS);
	}

    // Define conversions to pointers - this means that objects of
    // this class *cannot* be tested for validity with 'if (pn)' where
    // pn is a pathname object...
    operator const Char* () const
	{ return txt.c_str (); }

    // Define conversion to Char* (non-const) which is useful for
    // passing to Tcl API functions.
    operator Char * () const
	{ return const_cast<Char*> (txt.c_str ()); }

    // Return a quoted version of the path, where the quotation
    // characters are specified. It defaults to {} which is useful for
    // Tcl if nothing else.
    String quoted (Char a = '{', Char b = '}')
	{
	String	s;
	s  = a;
	s += txt;
	s += b;
	return s;
	}
    
    };

template <class Char, class String, int Sep, class Strcmp> const size_t
    pathname<Char, String, Sep, Strcmp>::NPOS = String::npos;

#ifdef _MSC_VER // Win32 native
#ifdef _UNICODE
typedef pathname<wchar_t, std::wstring, '/', wstrcmp_ci> CPathname;
#else
typedef pathname<char, std::string, '/', strcmp_ci> CPathname;
#endif
#else // Unix (assumes no namespaces)

typedef pathname<char, string, '/', strcmp_cs> CPathname;

#endif


#endif



