// VXWSymTab/vxwSymLib.h - symbol table class

// Copyright 1995-1999 Wind River Systems, Inc.

// modification history
// --------------------
// 01c,09mar99,jdi  doc: fixed wrong cross-references.
// 01b,23feb99,fle  doc : made it refgen compliant
// 01c,21feb99,jdi  added library section, checked in documentation.
// 01b,30sep95,rhp  documented.
// 01a,15jun95,srh  written.

// DESCRIPTION
// This class library provides facilities for managing symbol tables.  A symbol
// table associates a name and type with a value.  A name is simply an
// arbitrary, null-terminated string.  A symbol type is a small integer
// (typedef SYM_TYPE), and its value is a character pointer.  Though commonly
// used as the basis for object loaders, symbol tables may be used whenever
// efficient association of a value with a name is needed.
//
// If you use the VXWSymTab class to manage symbol tables local to
// your own applications, the values for SYM_TYPE objects are completely
// arbitrary; you can use whatever one-byte integers are appropriate for
// your application.
//
// If the VxWorks system symbol table is configured into your target
// system, you can use the VXWSymTab class to manipulate it based on
// its symbol-table ID, recorded in the global \f3sysSymTbl\f1; see
// VXWSymTab::VXWSymTab() to construct an object based on this global.
// In the VxWorks target-resident global symbol table, the values for
// SYM_TYPE are N_ABS, N_TEXT, N_DATA, and N_BSS (defined in a_out.h);
// these are all even numbers, and any of them may be combined (via
// boolean or) with N_EXT (1).  These values originate in the section
// names for a.out object code format, but the VxWorks system symbol
// table uses them as symbol types across all object formats.  (The
// VxWorks system symbol table also occasionally includes additional
// types, in some object formats.)
//
// All operations on a symbol table are interlocked by means of a
// mutual-exclusion semaphore in the symbol table structure.
//
// Symbols are added to a symbol table with VXWSymTab::add().  Each
// symbol in the symbol table has a name, a value, and a type.
// Symbols are removed from a symbol table with VXWSymTab::remove().
//
// Symbols can be accessed by either name or value.  The routine
// VXWSymTab::findByName() searches the symbol table for a symbol of a
// specified name.  The routine VXWSymTab::findByValue() finds the
// symbol with the value closest to a specified value.  The routines
// VXWSymTab::findByNameAndType() and VXWSymTab::findByValueAndType()
// allow the symbol type to be used as an additional criterion in the
// searches.
//
// Symbols in the symbol table are hashed by name into a hash table
// for fast look-up by name, for instance with VXWSymTab::findByName().  The
// size of the hash table is specified during the creation of a symbol
// table.  Look-ups by value, such as with VXWSymTab::findByValue(), must
// search the table linearly; these look-ups can thus be much slower.
//
// The routine VXWSymTab::each() allows each symbol in the symbol
// table to be examined by a user-specified function.
//
// Name clashes occur when a symbol added to a table is identical in
// name and type to a previously added symbol.  Whether or not symbol
// tables can accept name clashes is set by a parameter when the
// symbol table is created with VXWSymTab::VXWSymTab().  If name
// clashes are not allowed, VXWSymTab::add() returns an error if
// there is an attempt to add a symbol with identical name and type.
// If name clashes are allowed, adding multiple symbols with the same
// name and type is not an error.  In such cases,
// VXWSymTab::findByName() returns the value most recently added,
// although all versions of the symbol can be found by
// VXWSymTab::each().
//
// INCLUDE FILES: vxwSymLib.h
//
// SEE ALSO: VXWModule
//

#ifndef vxwSymLib_h
#define vxwSymLib_h

#include "vxWorks.h"
#include "symLib.h"
#include "vxwObject.h"
#include "vxwErr.h"

class VXWSymTab : virtual public VXWIdObject
    {
  public:
//_ VXWSymTab Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWSymTab::VXWSymTab - create a symbol table
//
// This constructor creates and initializes a symbol table with a hash table of
// a specified size.  The size of the hash table is specified as a power of two.
// For example, if <hashSizeLog2> is 6, a 64-entry hash table is created.
//
// If <sameNameOk> is FALSE, attempting to add a symbol with
// the same name and type as an already-existing symbol results in an error.
//
// Memory for storing symbols as they are added to the symbol table will be
// allocated from the memory partition <symPartId>.  The ID of the system
// memory partition is stored in the global variable `memSysPartId', which
// is declared in memLib.h.
//
// RETURNS: N/A

    VXWSymTab (int hashSizeLog2, BOOL sameNameOk, PART_ID symPartId)
	: stid_ (symTblCreate (hashSizeLog2, sameNameOk, symPartId))
	{
	if (stid_ == 0)
	    vxwThrowErrno ();
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWSymTab::VXWSymTab - create a symbol-table object 
// 
// This constructor creates a symbol table object based on an existing
// symbol table.  For example, the following statement creates a
// symbol-table object for the VxWorks system symbol table (assuming
// you have configured a target-resident symbol table into your
// VxWorks system):
// .CS
// VXWSymTab sSym;
// ...
// sSym = VXWSymTab (sysSymTbl);
// .CE

    VXWSymTab (SYMTAB_ID aSymTabId)
	: stid_ (aSymTabId)
	{
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWSymTab::~VXWSymTab - delete a symbol table
// 
// This routine deletes a symbol table; it deallocates all memory
// associated with its symbol table, including the hash table, and
// marks the table as invalid.
//
// Deletion of a table that still contains symbols throws an error.
// Successful deletion includes the deletion of the internal hash table and
// the deallocation of memory associated with the table.  The table is marked
// invalid to prohibit any future references.
//
// RETURNS: OK, or ERROR if the table still contains symbols.

    ~VXWSymTab ()
	{
	if (symTblDelete (stid_) != OK)
	    vxwThrowErrno ();
	}

//_ VXWSymTab Public Member Functions

///////////////////////////////////////////////////////////////////////////////
//
// VXWSymTab::add - create and add a symbol to a symbol table, including a group number
//
// This routine allocates a symbol <name> and adds it to its symbol
// table with the specified parameters <value>, <type>, and <group>.
// The <group> parameter specifies the group number assigned to a module when
// it is loaded on the target; see the manual entry for moduleLib.
//
// RETURNS: OK, or ERROR if there is insufficient memory for the symbol to be
// allocated.
//
// SEE ALSO: moduleLib

    STATUS add (char *name, char *value, SYM_TYPE type, UINT16 group)
	{
	return symAdd (stid_, name, value, type, group);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWSymTab::each - call a routine to examine each entry in a symbol table
//
// This routine calls a user-supplied routine to examine each entry in the
// symbol table; it calls the specified routine once for each entry.  The
// routine must have the following type signature:
// .CS
//     BOOL routine
//         (
//         char *	name,	/@ entry name                  @/
//         int		val,	/@ value associated with entry @/
//         SYM_TYPE	type,	/@ entry type                  @/
//         int		arg,	/@ arbitrary user-supplied arg @/
//         UINT16	group	/@ group number                @/
//         )
// .CE
// The user-supplied routine must return TRUE if VXWSymTab::each() is to
// continue calling it for each entry, or FALSE if it is done and
// VXWSymTab::each() can exit.
//
// RETURNS: A pointer to the last symbol reached, or NULL if all symbols are
// reached.

    SYMBOL * each (FUNCPTR routine, int routineArg)
	{
	return symEach (stid_, routine, routineArg);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWSymTab::findByName - look up a symbol by name
//
// This routine searches its symbol table for a symbol matching a specified
// name.  If the symbol is found, its value and type are copied to <pValue>
// and <pType>.  If multiple symbols have the same name but differ in type,
// the routine chooses the matching symbol most recently added to the symbol
// table.
//
// RETURNS: OK, or ERROR if the symbol cannot be found.

    STATUS findByName (char *name, char **pValue, SYM_TYPE *pType) const
	{
	return symFindByName (stid_, name, pValue, pType);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWSymTab::findByNameAndType - look up a symbol by name and type
//
// This routine searches its symbol table for a symbol matching both name and
// type (<name> and <goalType>).  If the symbol is found, its value and type are
// copied to <pValue> and <pType>.  The <mask> parameter can be used to match
// sub-classes of type.
//
// RETURNS: OK, or ERROR if the symbol is not found.

    STATUS findByNameAndType (char *name, char **pValue, SYM_TYPE *pType,
			      SYM_TYPE goalType, SYM_TYPE mask) const
	{
	return symFindByNameAndType (stid_, name, pValue,
				       pType, goalType, mask);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWSymTab::findByValue - look up a symbol by value
//
// This routine searches its symbol table for a symbol matching a specified
// value.  If there is no matching entry, it chooses the table entry with the
// next lower value.  The symbol name (with terminating EOS), the actual
// value, and the type are copied to <name>, <pValue>, and <pType>.
//
// RETURNS: OK, or ERROR if <value> is less than the lowest value in the table.

    STATUS findByValue (UINT value, char *name, int *pValue,
			SYM_TYPE *pType) const
	{
	return symFindByValue (stid_, value, name, pValue, pType);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWSymTab::findByValueAndType - look up a symbol by value and type
// 
// This routine searches a symbol table for a symbol matching both
// value and type (<value> and <goalType>).  If there is no matching
// entry, it chooses the table entry with the next lower value.  The
// symbol name (with terminating EOS), the actual value, and the type
// are copied to <name>, <pValue>, and <pType>.  The <mask> parameter
// can be used to match sub-classes of type.
//
// RETURNS: OK, or ERROR if <value> is less than the lowest value in the table.

    STATUS findByValueAndType (UINT value, char *name, int *pValue,
			       SYM_TYPE *pType, SYM_TYPE goalType,
			       SYM_TYPE mask) const
	{
	return symFindByValueAndType (stid_, value, name, pValue,
					pType, goalType, mask);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWSymTab::remove - remove a symbol from a symbol table
//
// This routine removes a symbol of matching name and type from
// its symbol table.  The symbol is deallocated if found.
// Note that VxWorks symbols in a standalone VxWorks image (where the 
// symbol table is linked in) cannot be removed.
//
// RETURNS: OK, or ERROR if the symbol is not found
// or could not be deallocated.

    STATUS remove (char *name, SYM_TYPE type)
	{
	return symRemove (stid_, name, type);
	}
    
  protected:
    VXWSymTab ()
	{
	}
    VXWSymTab (const VXWSymTab &)
	{
	}
    VXWSymTab & operator = (const VXWSymTab &)
	{
	return *this;
	}
    virtual void * myValue ();
    SYMTAB_ID stid_;
    };

#endif /* ifndef vxwSymLib_h */
