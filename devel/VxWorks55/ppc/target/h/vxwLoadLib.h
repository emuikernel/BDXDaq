// VXWModule/vxwLoadLib.h - object module class

// Copyright 1995-1999 Wind River Systems, Inc.

// modification history
// --------------------
// 01e,08mar99,jdi  doc: fixed wrong cross-references.
// 01d,23feb99,fle  made it refgen parsable
// 01c,21feb99,jdi  added library section, checked in documentation.
// 01b,03oct95,rhp  documented.
// 01a,15jun95,srh  written.

// DESCRIPTION
// The `VXWModule' class provides a generic object-module loading
// facility.  Any object files in a supported format may be loaded
// into memory, relocated properly, their external references
// resolved, and their external definitions added to the system symbol
// table for use by other modules.  Modules may be loaded from any I/O
// stream.
//
// INCLUDE FILE: vxwLoadLib.h
//
// SEE ALSO: usrLib, symLib, VXWMemPart,
// .pG "C++ Development"
//
// SECTION: 1C
//

#ifndef vxwLoadLib_h
#define vxwLoadLib_h

#include "vxWorks.h"
#include "loadLib.h"
#include "moduleLib.h"
#include "unldLib.h"
#include "vxwObject.h"
#include "vxwErr.h"

class VXWModule : virtual public VXWIdObject
    {
  public:

///////////////////////////////////////////////////////////////////////////////
//
// VXWModule::VXWModule - build module object from module ID
// 
// Use this constructor to manipulate a module that was not loaded
// using C++ interfaces.  The argument <id> is the module
// identifier returned and used by the C interface to the VxWorks
// target-resident load facility.
// 
// RETURNS: N/A.
// 
// SEE ALSO: loadLib

    VXWModule (MODULE_ID aModuleId)
	: mid_ (aModuleId)
	{
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWModule::VXWModule - load an object module at specified memory addresses
// 
// This constructor reads an object module from <fd>, and loads the
// code, data, and BSS segments at the specified load addresses in
// memory set aside by the caller using VXWMemPart::alloc(), or in the
// system memory partition as described below.  The module is properly
// relocated according to the relocation commands in the file.
// Unresolved externals will be linked to symbols found in the system
// symbol table.  Symbols in the module being loaded can optionally be
// added to the system symbol table.
//
// LINKING UNRESOLVED EXTERNALS
// As the module is loaded, any unresolved external references are resolved
// by looking up the missing symbols in the the system symbol table.
// If found, those references are correctly linked to the new module.
// If unresolved external references cannot be found in the system symbol
// table, then an error message ("undefined symbol: ...") is printed for
// the symbol, but the loading/linking continues.  In this case, NULL is
// returned after the module is loaded.
//
// ADDING SYMBOLS TO THE SYMBOL TABLE
// The symbols defined in the module to be loaded may be optionally added
// to the target-resident system symbol table, depending on the value of
// <symFlag>:
// .iP "LOAD_NO_SYMBOLS" 29
// add no symbols to the system symbol table
// .iP "LOAD_LOCAL_SYMBOLS"
// add only local symbols to the system symbol table
// .iP "LOAD_GLOBAL_SYMBOLS"
// add only external symbols to the system symbol table
// .iP "LOAD_ALL_SYMBOLS"
// add both local and external symbols to the system symbol table
// .iP "HIDDEN_MODULE"
// do not display the module via moduleShow().
// .LP
//
// In addition, the following symbols are also added to the symbol table
// to indicate the start of each segment:
// <filename>_text, <filename>_data, and <filename>_bss,
// where <filename> is the name associated with the fd.
//
// RELOCATION
// The relocation commands in the object module are used to relocate
// the text, data, and BSS segments of the module.  The location of each
// segment can be specified explicitly, or left unspecified in which
// case memory is allocated for the segment from the system memory
// partition.  This is determined by the parameters <ppText>, <ppData>, and
// <ppBss>, each of which can have the following values:
// .iP "NULL"
// no load address is specified, none will be returned;
// .iP "A pointer to LD_NO_ADDRESS"
// no load address is specified, the return address is referenced by the
// pointer;
// .iP "A pointer to an address"
// the load address is specified.
// .LP
//
// The <ppText>, <ppData>, and <ppBss> parameters specify where to load
// the text, data, and bss sections respectively.  Each of these
// parameters is a pointer to a  pointer; for example, **<ppText>
// gives the address where the text segment is to begin.
//
// For any of the three parameters, there are two ways to request that
// new memory be allocated, rather than specifying the section's
// starting address: you can either specify the parameter itself as
// NULL, or you can write the constant LD_NO_ADDRESS in place of an
// address.  In the second case, this constructor replaces the
// LD_NO_ADDRESS value with the address actually used for each section
// (that is, it records the address at *<ppText>, *<ppData>, or
// *<ppBss>).
//
// The double indirection not only permits reporting the addresses
// actually used, but also allows you to specify loading a segment
// at the beginning of memory, since the following cases can be
// distinguished:
// .IP (1) 4
// Allocate memory for a section (text in this example):  <ppText> == NULL
// .IP (2)
// Begin a section at address zero (the text section, below):  *<ppText> == 0
// .LP
// Note that loadModule() is equivalent to this routine if all three of the
// segment-address parameters are set to NULL.
//
// COMMON
// Some host compiler/linker combinations internally use another
// storage class known as \f2common\f1.  In the C language,
// uninitialized global variables are eventually put in the BSS
// segment.  However, in partially linked object modules they are
// flagged internally as common and the static linker on the host
// resolves these and places them in BSS as a final step in creating a
// fully linked object module.  However, the VxWorks target-resident
// dynamic loader is most often used to load partially linked object
// modules.  When the VxWorks loader encounters a variable labeled as
// common, memory for the variable is allocated, and the variable is
// entered in the system symbol table (if specified) at that address.
// Note that most static loaders have an option that forces resolution
// of the common storage while leaving the module relocatable.
//
// RETURNS: N/A.
//
// SEE ALSO
// .pG "C++ Development"

    VXWModule (int fd, int symFlag, char **ppText,
	       char **ppData=0, char **ppBss=0)
	: mid_ (loadModuleAt (fd, symFlag, ppText, ppData, ppBss))
	{
	if (mid_ == NULL)
	    vxwThrowErrno ();
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWModule::VXWModule - load an object module into memory
// 
// This constructor loads an object module from the file descriptor
// <fd>, and places the code, data, and BSS into memory allocated from
// the system memory pool.
//
// RETURNS: N/A

    VXWModule (int fd, int symFlag)
	: mid_ (loadModule (fd, symFlag))
	{
	if (mid_ == NULL)
	    vxwThrowErrno ();
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWModule::VXWModule - create and initialize an object module
// 
// This constructor creates an object module descriptor.  It is usually
// called from another constructor.
// 
// The arguments specify the name of the object module file, 
// the object module format, and a collection of options <flags>.
// 
// Space for the new module is dynamically allocated.
// 
// RETURNS: N/A

    VXWModule (char *name, int format, int flags)
	: mid_ (moduleCreate (name, format, flags))
	{
	if (mid_ == 0)
	    vxwThrowErrno ();
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWModule::~VXWModule - unload an object module
// 
// This destructor unloads the object module from the target system.  
// For a.out and ECOFF format modules, unloading does the following:
// .IP (1) 4
// It frees the space allocated for text, data,
// and BSS segments, unless VXWModule::VXWModule() was called with specific
// addresses, in which case the application is responsible for freeing space.
// .IP (2)
// It removes all symbols associated with the object module from the
// system symbol table.
// .IP (3)
// It removes the module descriptor from the module list.
// .LP
//
// For other modules of other formats, unloading has similar effects.
// 
// Unloading modules with this interface has no effect on breakpoints
// in other modules.
// 
// RETURNS: N/A

    ~VXWModule ()
	{
	if (unldByModuleId (mid_, UNLD_KEEP_BREAKPOINTS) != OK)
	    vxwThrowErrno ();
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWModule::flags - get the flags associated with this module
//
// This routine returns the flags associated with its module.
//
// RETURNS: The option flags.

    int flags () const
	{
	return moduleFlagsGet (mid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWModule::info - get information about object module
// 
// This routine fills in a MODULE_INFO structure with information about the
// object module.
// 
// RETURNS: OK or ERROR.

    STATUS info (MODULE_INFO * pModuleInfo) const
	{
	return moduleInfoGet (mid_, pModuleInfo);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWModule::name - get the name associated with module
//
// This routine returns a pointer to the name associated with its module.
//
// RETURNS: A pointer to the module name.

    char * name () const
	{
	return moduleNameGet (mid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWModule::segFirst - find the first segment in module
// 
// This routine returns information about the first segment of a module
// descriptor.
// 
// RETURNS: A pointer to the segment ID.
//
// SEE ALSO: VXWModule::segGet()

    SEGMENT_ID segFirst () const
	{
	return moduleSegFirst (mid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWModule::segGet - get (delete and return) the first segment from module
// 
// This routine returns information about the first segment of a module
// descriptor, and then deletes the segment from the module.
// 
// RETURNS: A pointer to the segment ID, or NULL if the segment list is empty.
// 
// SEE ALSO: VXWModule::segFirst()

    SEGMENT_ID segGet ()
	{
	return moduleSegGet (mid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWModule::segNext - find the next segment in module
// 
// This routine returns the segment in the list immediately following
// <segmentId>.
// 
// RETURNS: A pointer to the segment ID, or NULL if there is no next segment.

    SEGMENT_ID segNext (SEGMENT_ID segmentId) const
	{
	return moduleSegNext (segmentId);
	}
  protected:
    VXWModule ()
	{
	}
    VXWModule (const VXWModule &)
	{
	}
    VXWModule & operator = (const VXWModule &)
	{
	return *this;
	}
    virtual void * myValue ();
    MODULE_ID mid_;
    };

#endif /* ifndef vxwLoadLib_h */
