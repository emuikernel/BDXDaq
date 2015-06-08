# dataDocLib.tcl - TCL library for the project facilities dataDoc engine
#
# Copyright 1998-2001 Wind River Systems, Inc. Alameda, CA
#
# modification history
# --------------------
# 03j,29may02,rbl  fix problem where min variable ends up undefined when
#                  excluding certain components
# 03i,22may02,rbl  fix problem where Project Facility messes up REQUIRES on
#                  Selections in figuring out unavailable components
# 03i,22may02,cjs  Moved filenameQuoteIfNecessary() to Utils.tcl
# 03h,22may02,cjs  add call to filenameQuoteIfNecessary() to protect
#                  temporary file containing spaces from eval()
# 03g,14may02,rbl  fix problem where project facility messes up REQUIRES
#                  on a Selection when removing components - see SPR 77280
# 03f,07may02,cjs  Continued support for spr 76601 -- prevent C preprocessor
#                  from having its way with CPU and TOOL macros
# 03e,03may02,cjs  Fixing spr 76601.  Fixing bugs expanding and contracting
#                  archive paths
# 03d,02may02,cjs  Fix spr 74684 -- Excluding component of a Selection doesn't
#                  automatically exclude components that depends on that
#                  Selection
# 03c,19mar02,rbl  Fixing problem where removing "full C++ iostreams" 
#                  component removes "C++ string type"
#                  Look for configlettes in bsp directory and target/src/drv
# 03b,25mar02,cjs  Fix spr 74596 -- defaults for selections not pulled in 
# 03a,23mar02,cjs  Fix spr 73922 by initializing variable _requireUnresolved
# 02z,13feb02,cjs  Fix bug in handling 'Unresolved' objects for determining
#                  unavailable components
# 02y,11dec01,rbl  allow cxrDataDocCreate to report better error information
#                  on a preprocessor error
# 02x,10dec01,rbl  fixing problem where C++ drags in DCOM
# 02w,08nov01,j_s  fix mxrLibsContract: return full path instead of just file
#                  name; added expansion of VX_OS_LIBS to mxrLibsExpand
# 02v,02nov01,j_s  Rename cxrLibsExpand to mxrLibsExpand and modify it 
#		   for repackage; add mxrLibsContract and mxrPrjHandleGet
# 02u,29oct01,rbl  get cxrDataDocCreate working for diab
# 02u,29oct01,mht  add importCdf method back in, since datdocs will now be
#                  built
# 02t,29oct01,mht  back out changes till tomorrow, so datadocs have a chance
#                  to build
# 02s,18oct01,mht  backport T3 mxrDocCreation to T2
# 02r,27sep01,rbl  in toolchains, rename "vendor" to "tool" for consistency
#                  with the makefile TOOL variable
# 02r,03oct01,dat  Adding BSP_STUB support, SPR 69150
# 02q,31may01,t_m  fix -xc for CPP includes
# 02h,27feb01,sn   Improve error message filtering for nm
# 02q,10jan01,sn   Invoke the preprocessor with OPTION_LANG_C
# 02p,23mar99,ren  Fixing SPR 25788: errors in default selection analysis
# 02o,19mar99,ren  Filter error messages did not work on PC.  Fixed.
# 02n,18mar99,ren  Fixing problem with selections not including default
#                  components.  Fixes SPR 25788
# 02m,17mar99,ren  Filter error messages returned by size
# 02l,16mar99,ren  Filter error messages returned by nm
# 02k,04mar99,cjs  Fix alpha-numeric sorting of CDF files
# 02j,26feb99,cjs  Removing last fix.  Wasn't needed. 
# 02i,10feb99,cjs  Make cxrSubtree take into account selections
# 02h,19nov98,fle  doc : separated SEE ALSO elements with comas for
#                  linkability
# 02g,19nov98,ms    added documentation for refgen
# 02f,10nov98,ms    added LINK_DATASYMS handling.
# 02e,26oct98,ms    small tweeks for new build interface.
# 02d,11oct98,ms    mod* routines now take an MxrDoc param. Added modInfo.
# 02c,29sep98,ms    added mod* routines for simple module dependency queries.
#		    removed availableItemsInFolder bHasAvailableContents.
#		    cxrSubtree now optionally ignores INCLUDE_WHEN dependencies
# 02b,14sep98,ms    fixed cxrSizeGet for components with only LINK_SYMS
# 02a,03sep98,ms    fixed dll loading for HPUX as per Peter N.'s advice
# 01z,31aug98,yh    cleanup.
# 01y,26aug98,ms    added cxrLibsExpand. Also better checking for
#		    unavailable components in cxrUnavailableListGet
# 01x,17aug98,ren   Made mxrSupertreeTest much faster.  Added code to deal with
#                   module level linking in mxrSupertree
# 01w,14aug98,ren   Modified DocWalk.  Other code that uses docWalk may need to
#                   be modified.  Now, mxrSubtree nearly exactly matches the
#                   behavior of the linker, even when there are errors in
#                   the archive.
# 01v,13aug98,cjs   fixed defaultFolderSubTree() 
# 01u,12aug98,yh    fixed mxrSubtreeTest, added componentSizeTest
# 01t,07aug98,ms    fixed mxrSubtree[Test], started fix of mxrSupertree[Test].
# 01s,06aug98,ren   (maybe) fixed mxrSubtree
# 01r,05aug98,yh    fixed syntax error.
# 01q,04aug98,yh    bug fix for mxrSubtreeTest and mxrSupertreeTest.
#                   added mxrSizeTest.
# 01p,04aug98,cjs   removed common block size from mxrSizeGet() return value 
# 01o,31jul98,yh    added mxrSupertreeTest.
# 01n,31jul98,ms    fix mxrUsers ala 01l. better errors in cxtSetValidate.
# 01m,31jul98,yh    added mxrSubtreeTest.
# 01l,30jul98,ren   Fixed Uses so it pulls in common block symbols.
# 01k,27jul98,ms    new versions of [mc]xrDataDocCreate.
#		    removed useless cxrComponent*Info routines.
# 01j,22jul98,ms    cpp used via toolchain.
# 01i,20jul98,ms    fixed cSymsMangle, commSymsLink. bind toolchain to mxrDoc.
# 01h,14jul98,ms    real sizing info. Fixed bug in docWalk.
# 01g,14jul98,cjs   added foldersDeletableComponentsGet(); used  
#                   cxrSubfolders() in a few other routines  
# 01f,02jul98,ren   added load
# 01e,29jun98,ms    removed *PathGet routines. added cxrComponentChangeInfo.
# 01d,26may98,cjs   protect load commands from mixing debug/release cxrdoc,
#		    mxrdoc
# 01c,25may98,ms    added cxrUnavailListGet. Fixed but in cxrSupertree
# 01b,20may98,ms    added cxrSubfolders, cxrSize, cxrComponentInfo
# 01a,19mar98,ms    written
#
# DESCRIPTION
# This library contains the TCL API for accessing the configuration
# engine's dependency database.
# To use the procedures in this libary, you must first
# "source cmpScriptLib.tcl" into the wtxtcl interpreter.
# At the lowest level, the database is
# an in-memory object database. 
# Each object is identified by a string name, which must be unique
# within the dataBase.
# Each object can have a set of properties and a set of associations
# with other objects. The set of allowed object types, properties, and
# associations are defined by a schema.
#
# There are two schemas supported; MxrDoc (Module xref), and CxrDoc
# (Component xref). More schemas can be added later.
# The MxrDoc schema is quite simple. It supports only two types
# of objects; Module and Symbol. The associations are also quite simple,
# a Module can have an association with a symbol of type "imports",
# "exports", or "declares" (that last is for common block symbols).
# A symbol can have the corresponding association with a module of
# type "importedBy", "exportedBy", or "declaredBy".
# The properties of a module are it's textSize,
# dataSize, and bssSize. The properties of a symbol are it's size, which
# is only valid if the symbol is a common block symbol.
# The "import" command imports the output of "nm" to populate the
# database. The "importSize" command imports the output of "size" to
# finish populating the database.
# The CxrDoc schema is more complicated, and not covered here.
# The Tornado API guide has a section on component creation which
# describes the CxrDoc schema in more detail.
#
# This library provides a set of high-level routines for
# each schema. These routines can compute, for example,
# the modules called by a given module. These high-level
# routines are built on top of lower level routines.
# The simplest API to module xref information is provided by the mod*
# routines in this library. If you are just looking for a simple way to
# analyze module dependencies, then you don't need to read or understand
# the rest of the discussion below - just skip to the mod* library
# manual pages.
# To use any other procedure in the library, you'll first need to understand
# the low-level dataDoc interface described below.
#
# The low-level interface for dataDoc's does not depend on
# the schema. It allows one to ask; given a set of objects, what are it's
# properties and with what other objects is it associated?
# Before describing the interface, lets define some more terminology.
# A DataDoc is a database (MxrDoc or CxrDoc). A DataSet is a collection
# of objects in the database. DataDoc's and DataSet's can be created,
# deleted, and queried using the following API:
#
# DATADOC API
# 
#   docCreate <SchemaName> -> <DataDocument>
# 
#   Creates a data document object that utilizes the specified
#   schema.  The document will support the object types, attributes,
#   and relations described by the schema. For example, to create
#   an MxrDoc database, one could do "set d [docCreate MxrDoc]".
# 
#  <DataDocument> import <FileName>
# 
#   Reads an ASCII file containing descriptions of Data Objects. All
#   Data Object instances, and their attribute/association properties,
#   will be created and placed in the data document.
#   For example, "$d import <FileName>".
# 
#  <DataDocument> types -> {DObject TypeNames}
#
#   Returns a tcl a list of names of all the types of objects supported by
#   the data document.
# 
#  <DataDocument> instances [TypeName] -> {DObject Instance Names}
# 
#   Returns a tcl list of object "id names" for all the instances, of
#   the specified type, contained within the document.  If a TypeName
#   was not specified then the "id names" for all instances will
#   be returned.
# 
#  <DataDocument> setCreate -> <DObjectSet>
# 
#   Creates a set object, and associates with the given document.
#   A set object can be used to perform queries on any object in
#   the document that it is associated with.
# 
#  A DObjectSet can handle the following method calls from TCL.
# 
#  <DObjectSet> = {DObject Instances Names}
# 
#   Assigns objects to the set.  Queries can then be performed on those
#   objects using the other methods for <DObjectSet>.
# 
#   A <DObjectSet> can also be used in TCL as a set.  Tcl lists can be
#   assigned to the set, removing duplicates from the list. Set do not
#   perserve any order to their elements.
# 
#  <DObjectSet> instances [type] -> {DObject Names}
# 
#   Returns a TCL list of all the items that are in the set that are
#   names of objects in the document.
# 
#  <DObjectSet> contents -> {Object Names}
# 
#   Returns a TCL list of the contents of the set. Names will appear in
#   the list regardless of whether they are or are not the names of objects
#   in the document.
#
#  <DObjectSet> properties -> {Property Names}
# 
#   Returns a TCL list of all property names for all the objects
#   contained in the set.  Duplicate property names are not eliminated
#   from the list (a SET can be used to eliminate duplicates).
# 
#  <DObjectSet> get <propertyName> -> {Property Values}
# 
#   Returns a TCL list of the value of a given property for all the
#   objects in the set that support that property.
# 
#  <DObjectSet> types -> {DObject Type Names}
# 
#   Returns list of the type name for every object that is in the set.
#   Duplicate of a type name will appear when their is more than one
#   object of that type in the set.
# 
#  <DObjectSet> setCreate -> <DObjectSet>
# 
#   Creates an empty <DObjectSet> that is bound to the same data
#   document as the original set.
# 
# DATADOC SET OPERATIONS
# 
#   The syntax is <DObjectSet> <BinaryOp> <Tcl List> -> <Tcl List>
# 
#  <DObjectSet> - {Object Names} -> {List}
#   Set difference
# 
#  <DObjectSet> + {Object Names} -> {List}
#   Set union
# 
#  <DObjectSet> & {Object Names} -> {List}
#   Set intersection
# 
#  <DObjectSet> ^ {Object Names} -> {List}
#   Set symmetric difference (XOR)
#
# EXAMPLES
# suppose I have a file called "foo.nm", which is the output of
# "nm" on an archive. To find all symbols defined in the archive:
# .CS
#    set d [docCreate MxrDoc]   ;# create an empty database
#    $d import foo.nm           ;# import nm output
#    set s [$d setCreate]       ;# create a set
#    $s = [$d instances Module] ;# $s = all modules in database
#    $s = "[$s get exports] [$s get declares]" ;# all exported and
#                               ;# common block symbols
#    puts "exported symbols = [$s instances]" ;# print result
#    $s delete                  ;# delete the set
# .CE
#
# Suppose I want to find all undefined symbols in a set of modules:
# .CS
#    set s [$d setCreate]       ;# create a set
#    $s =  \<some set of modules\>
#    set defined "[$s get exports] [$s get declares]" ;# all exported syms
#    $s = [$s get imports]      ;# all imported syms
#    $s = [$s - $defined]       ;# remove all exported syms
#    puts "undefined symbols = [$s instances]" ;# print result
#    $s delete                  ;# delete the set
# .CE
#
# Of course lots of other examples can be found by reading the
# source code for the TCL routines provided by this library.
#

# load the data documents

if {"[info procs docCreate]" == ""} {
    set ext    ""
    set libDir lib
    if {[wtxHostType] == "x86-win32"} {
	set libDir bin
	if {![catch {uitclinfo debugmode} debugMode]} {
            if {$debugMode} {
		set ext "-d"
	    }
        }
    }
    append ext [info sharedlibextension]
    load [wtxPath host [wtxHostType] $libDir]CxrDoc$ext
    load [wtxPath host [wtxHostType] $libDir]MxrDoc$ext
}

###############################################################################
#
# cSymsMangle - get the mangled symbol names
#
# NOMANUAL
#

proc cSymsMangle {symList toolchain} {
    set prependChar [${toolchain}::cPrependChar]

    if {"$prependChar" == ""} {
	return $symList
    }

    regsub -all {[^ ]+} $symList "${prependChar}&" symList
    return $symList
}

###############################################################################
#
# cSymsDemangle - get the demangled symbol names
#
# NOMANUAL
# 

proc cSymsDemangle {symList toolchain} {
    set prependChar [${toolchain}::cPrependChar]

    if {"$prependChar" == ""} {
	return $symList
    }

    regsub -all " $prependChar" $symList { } symList
    return [string range $symList 1 end]
}

###############################################################################
#
# docWalk - walk a document
#
# Recall that all dataDoc's contain objects and associations between
# them. This forms a graph.
# This routine walks the object graph starting with
# an initial set of nodes and visits the next set of nodes according
# to the TCL proc <query>. It continues visiting new nodes until
# such a time as no new nodes are visited.
#
# This is really a support routine for other high-level recursive queries.
# For example, if you want to know "what modules will be dragged
# in by dependancy if I drag in fooLib.o", then you want to perform
# a recursive walk of the MxrDoc. In fact, the routines modSubtree
# and mxrSubtree are built on top of docWalk (for the recursion),
# and mxrUses (for the walk).
#
# The routine <query> should must be a TCL proc that takes as input
# a DataSet and a 2nd optional argument which is proc-specific. It
# should return a list of node-names that are to be vistied next.
#
# PARAMETERS
#	inputSet : DataSet of objects
#	query : TCL proc that performs the graph walk
#	arg : (optional) argument to pass to query
#
# RETURNS
#	the set of node visited during the graph walk
#

proc docWalk {inputSet query {arg ""}} {
    set visitedSet [$inputSet setCreate]
    set fringeSet [$inputSet setCreate]

    $visitedSet = [list ]
    $fringeSet = [$inputSet contents]
    while { [$fringeSet size] != [$visitedSet size] } {
	$visitedSet = [$fringeSet contents]
	$fringeSet = [$fringeSet + [$query $fringeSet $arg]]
    }

    set result [$visitedSet contents]
    $visitedSet delete
    $fringeSet delete

    set result
}

###############################################################################
#
# mxrDataDocCreate - create a module xref document
#
# This routine creates an mxrDoc from a set of object modules and
# archives. It runs the "nm" and "size" commands corresponding to
# the toolchain specified, and imports these into a new MxrDoc.
# This routine will fail if the object files and archives are not
# compatible with the given toolchain. The toolchain must be
# of the form ::tc_<CPUTOOL> (e.g., ::tc_PPC604gnu).
#
# Note: If you source cmpScriptLib.tcl, you can call "mxrDocCreate $hProj"
# to get the MxrDoc associated with a given project handle.
# mxrDocCreate is actually built on top of mxrDataDocCreate.
#
# PARAMETERS
#	toolchain : toolchain of the form ::tc_<CPUTOOL>
#	fileList : list of object files and archives
#
# RETURNS
#	an MxrDoc
#
# SEE ALSO
#	cmpScriptLib, cmpProjHandleGet
# 

proc mxrSplitErrorOutput { tool bufError varError varCleanOutput } {
	upvar $varError strError
	upvar $varCleanOutput strCleanOutput
	
	set strMatchErrorLine "[lindex $tool 0]((\\.exe)?):\[^\n]*(\n?)"
	regsub -all $strMatchErrorLine "$bufError\n" \x82\\0\x81 bufMarkedError1
	regsub -all "\x81\[^\x82]*\x82" "\x81$bufMarkedError1\x82" {} strError
        # Errors of the form "blah.o: no symbols" should be stripped from the output
        #  but need not be reported to the user.
        set strMatchErrorLine "\[^\n]*\.o: no symbols\[^\n]*(\n?)"
	regsub -all $strMatchErrorLine "$bufMarkedError1\n" \x82\\0\x81 bufMarkedError
        regsub -all "\x82\[^\x81]*\x81" "$bufMarkedError" {} strCleanOutput
}

proc mxrDataDocCreate {toolchain fileList} {
    set mxrDoc [docCreate MxrDoc]
    $mxrDoc importCdf "[wtxPath]host/resource/tcl/app-config/Project/00mxr_symbol_types.cdf"

	if { [catch {
		set nmBuf   "[eval exec [${toolchain}::macroDefaultValGet NM] $fileList]\n"
	} bufError] } {
		mxrSplitErrorOutput [${toolchain}::macroDefaultValGet NM] $bufError strError nmBuf
		append strFinalErrorMessage $strError
	}
	
	if { [catch {
		set sizeBuf \
			"[eval exec [${toolchain}::macroDefaultValGet SIZE] $fileList]\n"
	} bufError] } {
		mxrSplitErrorOutput [${toolchain}::macroDefaultValGet SIZE] $bufError strError sizeBuf
		append strFinalErrorMessage $strError
	}

		if {[llength $fileList] == 1} {
			set nmBuf "$fileList:\n$nmBuf"
		}

    $mxrDoc importRawNm $nmBuf
    $mxrDoc importRawSize $sizeBuf

    set s [$mxrDoc setCreate]
    $s = .
    $s set toolchain $toolchain
    $s = [$mxrDoc instances Module]
    mxrOnlyOne $s
    $s delete

	if { [info exists strFinalErrorMessage] && [string length [string trim $strFinalErrorMessage]]} {
	    if {[catch {messageBox "Warning while generating xref info:\n$strFinalErrorMessage"}] } {
		#we're in wtxtcl, not uitclsh, so just puts error
		puts $strFinalErrorMessage
	}
	}
    return $mxrDoc
}

###############################################################################
#
# mxrPrjHandleGet - get the project handle associated with an MxrDoc
#
# This routine only works on MxrDoc's created by mxrDocCreate.
#
# PARAMETERS
#	mxrDoc : MxrDoc
#
# RETURNS
#	project handle associated with this MxrDoc
# 

proc mxrPrjHandleGet {mxrDoc} {
    global mxrDocs

    set prjHandleList [array names mxrDocs]
    foreach prjHandle $prjHandleList {
	if {$mxrDocs($prjHandle) == $mxrDoc} {
	    return $prjHandle
	}
    }
    return ""
}

###############################################################################
#
# mxrTcGet - get the toolchain associated with an MxrDoc
#
# This routine only works on MxrDoc's created by mxrDataDocCreate.
#
# PARAMETERS
#	mxrDoc : MxrDoc
#
# RETURNS
#	toolchain associated with this MxrDoc
# 

proc mxrTcGet mxrDoc {
    set s [$mxrDoc setCreate]
    $s = .
    set tc [$s get toolchain]
    $s delete
    return $tc
}

###############################################################################
#
# mxrSupertree - recursively, what modules call a given set of modules
#
# Suppose module1 calls module2, and module2 calls module3.
# Then the supertree of module3 would contain both module2 and module1.
# This routine is just like modSupertree, but taks a DataSet as input
# instead of a list of module names.
#
# PARAMETERS
#	modSet : DataSet of modules
#
# RETURNS
#	list of modules in the supertree of the input set
#

proc mxrSupertree modSet {
    set visited [$modSet setCreate]
    set result [docWalk $modSet mxrUsers $visited]
    $visited delete
    return $result
}


###############################################################################
#
# mxrSubtree - recursively, what modules are called by a given set of modules
# 
# Suppose module1 calls module2, and module2 calls module3.
# Then the subtree of module1 would contain both module2 and module3.
# This routine is just like modSubtree, but taks a DataSet as input
# instead of a list of module names.
#
# PARAMETERS
#	modSet : DataSet of modules
#
# RETURNS
#	list of modules in the subtree of the input set
# 

proc mxrSubtree modSet {
    set visited [$modSet setCreate]
    set result [docWalk $modSet mxrUses $visited]
    $visited delete
    return $result
}

###############################################################################
#
# mxrOnlyOne - only one Common block symbol source, please
#
# There are several files in the vxWorks archive that all "weakly export"
# the same common block symbols. This routine makes only the first such
# member the "declarer".
# This is needed in order that the subtree/supertree routines work
# as expected (meaning work the way the linker works).
#
# NOMANUAL
#

proc mxrOnlyOne {setModules} {
    set setSymbol [$setModules setCreate]
    foreach strSymbol [$setModules get declares] {
	$setSymbol = $strSymbol
	$setSymbol set declaredBy [lindex [$setSymbol get declaredBy] 0]
    }
    $setSymbol delete
}    

###############################################################################
#
# mxrUses - modules directly called by a set of modules.
# 
# This routine returns the set of modules directly called by
# a given set of modules.
#
# PARAMETERS
#	modSet : DataSet of modules
#       arg    : not used
#
# RETURNS
#	list of modules called directly by the input set
#
# NOMANUAL
# 

proc mxrUses {inputSet {arg ""}} {
    set modules [$inputSet setCreate]
    set symbols [$inputSet setCreate]

    #
    # The algorithm is to preferentially satisify imports with exports, 
    # then use declares if necessary. This amounts to:
    # 
    # * get set of imported symbols
    # * look for modules exporting these symbols and pull them in
    # * remove the symbols exported by these modules because they are
    #   already satsified
    # * look for modules declaring the remaining symbols and pull them in
    #
    # NOTE: in the language used below in MxrUsers, another way of saying
    # this is:
    #
    # we find the set of modules that "define" (in the sense used below)
    # symbols imported by the input set
    #

    $symbols = [$inputSet get imports]	
    $modules = [$symbols get exportedBy]
    $symbols = [$symbols - [$modules get exports]]
    $modules = [$modules + [$symbols get declaredBy]]

    set result [$modules contents]
    $modules delete
    $symbols delete
    set result
}

###############################################################################
#
# mxrUsers - modules directly calling by a set of modules.
# 
# This routine returns the set of modules directly calling
# the given set of modules.
#
# PARAMETERS
#	modSet : DataSet of modules
#       arg    : not used
#
# RETURNS
#	list of modules directly calling the input set
# 
# NOMANUAL
# 

proc mxrUsers {inputSet {arg ""}} {
    set modules [$inputSet setCreate]
    set symbols [$inputSet setCreate]

    #
    # get the set of symbols that these modules "define"
    # by which we mean either:
    # * the symbol is explicitly exported by a module, or
    # * the symbol is declared by a module and not exported
    #   by any other module
    #

    # get declares
    $symbols = [$inputSet get declares]

    # remove symbols exported by another module
    $modules = [$symbols get exportedBy]
    $symbols = [$symbols - [$modules get exports]]

    # add exports
    $symbols = [$symbols + [$inputSet get exports]]

    # get modules which import these symbols
    $modules = [$symbols get importedBy]

    set result [$modules contents]
    $modules delete
    $symbols delete
    set result
}

###############################################################################
#
# mxrSizeGet - return the size of a set of modules
#
# PARAMETERS
#	modSet : DataSet of modules
#
# RETURNS: a list of three elements: text, data, bss sizes.
# 

proc mxrSizeGet modSet {
    set textSize 0
    set dataSize 0
    set bssSize  0
    foreach tSize [$modSet get textSize] {
	incr textSize $tSize
    }
    foreach dSize [$modSet get dataSize] {
	incr dataSize $dSize
    }
    foreach bSize [$modSet get bssSize] {
	incr bssSize $bSize
    }
    set comSet [$modSet setCreate]
    $comSet = [$modSet get declares]
    foreach cSize [$comSet get size] {
	incr bssSize $cSize
    }
    $comSet delete

    return [list $textSize $dataSize $bssSize]
}

###############################################################################
#
# mxrDocValidate - test for duplicate symbol names and other bugs
#
# This routine checks for bugs in an MxrDoc, and prints any errors
# found to stdout. The only errors currently tested for are
# symbols exported by more than one module.
#
# PARAMTERS
#	mxrDoc : MxrDoc to test
#
# RETURNS: N/A
#

proc mxrDocValidate iSet {
    set tSet [$iSet setCreate]

    foreach i [$iSet instances] {
	$tSet = $i
	$tSet = [$tSet get exportedBy]
	if {[$tSet size] > 1} {
	    puts "ERROR: symbol $i multiply defined by [$tSet contents]"
	}
    }
    $tSet delete
}

###############################################################################
#
# mxrSubtreeTest - test code for mxrSubtree
#
# Compare the set of modules from module dependency analysis
# against the set of modules from the actual linking.
#
# NOMANUAL
#

proc mxrSubtreeTest {mod cpu {tool "gnu"}} {

    set tc [prjTcListGet $cpu $tool "C++"]
    set cc ${tc}::tool_cc
    set cccmd [set ${cc}::tool]

    set linkType [${tc}::commSymLink]
    
    set lib [wtxPath]/target/lib/lib${cpu}${tool}vx.a
    set mxrDoc [mxrDataDocCreate tc_${cpu}${tool} $lib]
    
    set modLib ${mod}.o
    set mxrSet [$mxrDoc setCreate]
    $mxrSet = $mod
	
    # build the partially linked obj module by forcing a symbol undefined

    set mxrSym  [lindex [$mxrSet get exports] 0]
    if {$mxrSym == ""} {
	$mxrSet delete
	$mxrDoc delete
	puts "Since $mod doesn't define any symbol, no test is performed\n"
        return 0	
    } else {
	catch {exec $cccmd -nostdlib -r -u $mxrSym $lib -o $modLib} err
    }
    
    $mxrSet = [mxrSubtree $mxrSet] 
    
    # get the set of modules from the library
    
    set libDoc [mxrDataDocCreate tc_${cpu}${tool} $modLib]
    set s [$libDoc setCreate]
    $s = [$libDoc instances Module]
    set libSet [$mxrDoc setCreate]
    set libExports [$mxrDoc setCreate]
    $libExports = [$s get exports]
    $libSet = [$libExports get exportedBy]
    
    set mxrExtra [list [$mxrSet - [$libSet instances]]]
    set libExtra [list [$libSet - [$mxrSet instances]]]

    # print out the test result
    
    if {$mxrExtra == "{}" && $libExtra == "{}"} {
	puts "mxrSubtreeTest passed for $mod"
	set result 0
    } else {
	# if it is module level linking, ignore all .o that only contains
	# common block symbols

	set thisMod [$mxrDoc setCreate]
	if {$linkType == "module"} {
	    
	    foreach extra $mxrExtra {
		$thisMod = $extra

		if { "[$thisMod get exports] [$thisMod get imports]" == " "} {
		    set mxrExtra [list [lreplace $mxrExtra [lsearch $mxrExtra $extra] [lsearch $mxrExtra $extra]]]
		}
	    }
	}
	
	$thisMod delete

	if { $mxrExtra != "{}" || $libExtra != "{}"} {
	    puts "ERROR: mxrSubtreeTest failed for $mod"
	    puts "The extra modules from module dependency analysis are: $mxrExtra"
	    puts "The extra modules from library are: $libExtra"
	    set result -1
	} else {
	    puts "mxrSubtreeTest passed for $mod"
	    set result 0
	}
    }

    $mxrSet delete
    $s delete
    $libExports delete
    $libSet delete
    $mxrDoc delete
    $libDoc delete
    exec rm -rf $modLib
    return $result
}

###############################################################################
#
# mxrSizeTest - test code for mxrSize
#
# Given a module, test the size of the subtree  
#
# NOMANUAL
#

proc mxrSizeTest {mod cpu {tool "gnu"}} {
    set tc [prjTcListGet $cpu $tool "C++"]
    set cc [${tc}::toolFindByType "C/C++ compiler"]
    set cccmd [set ${cc}::tool]
    set arcmd [set ${tc}::util_ar]
    set sizecmd [set ${tc}::util_size]

    set lib [wtxPath]/target/lib/lib${cpu}${tool}vx.a
    set mxrDoc [mxrDataDocCreate tc_${cpu}${tool} $lib]
    
    set modLib ${mod}.o
    set modAr ${mod}.a
    set mxrSet [$mxrDoc setCreate]
    $mxrSet = $mod
	
    # build the partially linked obj module by forcing a symbol undefined

    set mxrSym  [lindex [$mxrSet get exports] 0]
    if {$mxrSym == ""} {
	$mxrSet delete
	$mxrDoc delete
	puts "Since $mod doesn't define any symbol, no test is performed\n"
    } else {
	exec $cccmd -nostdlib -r -u $mxrSym $lib -o $modLib
	exec $arcmd -crus $modAr $modLib 
    }
    
    $mxrSet = [mxrSubtree $mxrSet]
    puts "Size from the subtree analysis: [mxrSizeGet $mxrSet]"
    
    # get the size info from the library
    
    puts "Size from the library:\n [exec $sizecmd $modAr]"

    $mxrSet delete
    exec rm -rf $modLib $modAr
    $mxrDoc delete

}

###############################################################################
#
# mxrSupertreeTest - test code for mxrSupertree
#
# NOMANUAL
#

proc mxrSupertreeTest {mod cpu {tool "gnu"}} {
    set tc [prjTcListGet $cpu $tool "C++"]
    set lib [wtxPath]target/lib/lib${cpu}${tool}vx.a
    set mxrDoc [mxrDataDocCreate tc_${cpu}${tool} $lib]

    set s [$mxrDoc setCreate]
    $s = $mod
    set superSet [mxrSupertree $s]

    # verify that for each module in $mod's supertree, its subtree 
    # contains $mod

    set m [$mxrDoc setCreate]
    foreach elem $superSet {
	$m = $elem
	if {[lsearch [mxrSubtree $m] $mod] == -1} {
	    puts "ERROR: mxrSupertreeTest failed for $mod."
	    puts "$elem is in $mod 's supertree but $mod is not in $elem 's subtree"
	    $s delete
	    $m delete
	    $mxrDoc delete
	    return -1 
	}
    }

    # verify that for each module not in $mod's supertree, its subtree not 
    # contains $mod

    set mxrSet [$mxrDoc setCreate]
    $mxrSet = [$mxrDoc instances Module]
    
    $mxrSet = [$mxrSet - $superSet]
    $mxrSet = [$mxrSet - "$lib"]
    
    $m = [$mxrSet instances Module]
    set elem [$m contents]
    if {[lsearch [mxrSubtree $m] $mod] != -1} {
	puts "ERROR: mxrSupertreeTest failed for $mod in complement set test."
	$s delete
	$m delete
	$mxrSet delete
	$mxrDoc delete
	return -1 
    }

    $s delete
    $m delete
    $mxrSet delete
    $mxrDoc delete
    puts "mxrSupertreeTest passed for $mod"
    return 0
}


###############################################################################
#
# cxrDataDocCreate - create a component xref document from .cdf files
#
# This routine takes a path to look for .cdf files, and returns
# a the corresponding CxrDoc. A toolchain must be specified of the
# form ::tc_<CPUTOOL> (e.g., ::tc_PPC604gnu). Before importing the
# .cdf files into a CxrDoc, the toolchain's preprocessor
# is run on the .cdf files with -DCPU=<CPU>. This allows CPU-specific
# overrides of the component descriptors to be made in the .cdf files.
#
# Note: If you source cmpScriptLib.tcl, you can call "cxrDocCreate $hProj"
# to get the CxrDoc associated with a given project handle.
# cxrDocCreate is actually built on top of cxrDataDocCreate.
#
# PARAMETERS
#	cdfPath : path to search for .cdf files
#	toolchain : toolchain of the form ::tc_<CPUTOOL>
#
# RETURNS
#	a CxrDoc
#
# SEE ALSO
#	cmpScriptLib, cmpProjHandleGet
#

proc cxrDataDocCreate {cdfPath toolchain} {
    set cpu  [${toolchain}::cpu]
    set tool [${toolchain}::tool]
    set cpp  [${toolchain}::macroDefaultValGet CPP]
    set langc [${toolchain}::macroDefaultValGet OPTION_LANG_C]
    set undef [${toolchain}::macroDefaultValGet OPTION_UNDEFINE_MACRO]
    set def [${toolchain}::macroDefaultValGet OPTION_DEFINE_MACRO]
    set ccarchspec [${toolchain}::macroDefaultValGet CC_ARCH_SPEC]
    set cxrDoc  [docCreate CxrDoc]
    set cxrFiles ""
    foreach dir $cdfPath {
	set cxrFiles "$cxrFiles [lsort [glob -nocomplain $dir/*.cdf]]"
    }
    # $langc instructs the preprocessor to treat all input files
    # as .c files regardless of the extension (which will typically
    # be .cdf)
    set errorFile [tempFileGet] 
    if {[catch {
        set buf [eval exec $cpp $ccarchspec $langc ${undef}CPU ${def}$cpu \
            ${undef}TOOL ${def}$tool $cxrFiles \
            2> [filenameQuoteIfNecessary $errorFile]]
        }]} {
        set fd [open $errorFile r]
        set dependError [read $fd]
        close $fd
        file delete $errorFile
        if {$dependError != ""} {
            error "cxrDataDocCreate: dependency generation failed: \
                $dependError"
        } else {
            # find which .cdf file caused the preprocessor to choke
            foreach cdfFile $cxrFiles {
                if {[catch {eval exec $cpp $ccarchspec $langc ${undef}CPU \
                    $def$cpu ${undef}TOOL $def$tool $cdfFile}]} {
                    puts stderr "cxrDataDocCreate: dependency generation \
                        failed on file $cdfFile"
                }
            }
            error "cxrDataDocCreate: dependency generation failed..."
        }
    }
    file delete $errorFile
    $cxrDoc importRaw $buf
    return $cxrDoc
}

###############################################################################
#
# cxrSupertree - what components require a given set of components
#
# If a user wants to exclude a set of components, this routine
# can be used to tell what other components also need to be
# excluded by dependency.
# Because component dependencies are built on top of module
# dependencies, this routine also requires an MxrDoc containing
# information about the component's modules.
# The config engine uses this routine with parameter <mxrDoc> as
# the return value of "mxrDocCreate $hProj".
#
# PARAMETERS
#	cmpSet : DataSet of components
#	mxrDoc : MxrDoc
#       cmpsCurrent:   (optional, "" by default) list of components
#           that are already in the configuration. If this is not
#           specified, REQUIRES on Selections are not taken into 
#           account. 
#           If this is specified, this state information
#           is used to figure out whether any selections involved in
#           cmpSet are already satisfied by components in cmpsCurrent. 
#           Components REQUIRE - ing these selections will
#           not be dragged in to the superset.  
#
# RETURNS
#	the list of all components that require components in cmpSet
#
# SEE ALSO
#	mxrDataDocCreate
#

proc cxrSupertree {cmpSet mxrDoc {cmpsCurrent ""}} {

    set cmpSetIncluded [$cmpSet setCreate]
    $cmpSetIncluded = $cmpsCurrent

    proc walk {cmpSetOrig arg} {
        set mxrDoc [lindex $arg 0]
        set cmpSetIncluded [lindex $arg 1]
        # update included comps to remove the working set
        $cmpSetIncluded = [$cmpSetIncluded - \
            [$cmpSetOrig instances Component]]

	set toolchain [mxrTcGet $mxrDoc]

        set cmpSet     [$cmpSetOrig setCreate]
        set cmpSet2    [$cmpSetOrig setCreate]
        set modSet     [$mxrDoc setCreate]
        set cxrSet     [$cmpSetOrig setCreate]

        # add all components that REQUIRE us

        $cmpSet = [$cmpSetOrig instances Component]
        $cmpSet2 = [$cmpSet get _REQUIRES]
        $cmpSet = [$cmpSet + [$cmpSet2 instances Component]]

        # add all components that REQUIRE the selection we're in
        if {$cmpSetIncluded != ""} {
	    $cmpSet2 = [$cmpSetOrig instances Component]
            $cmpSet2 = [$cmpSet2 get _CHILDREN]
	    $cmpSet2 = [$cmpSet2 instances Selection]

            # remove already satisified selections from this list
            foreach selection [$cmpSet2 instances] {
	        $cxrSet = $selection
                set min 0
	        set max 100000
	        if {[scan [$cxrSet get COUNT] "%d-%d" min max] == 0} {
	            continue
	        }
	        $cxrSet = [$cmpSetIncluded & [$cxrSet get CHILDREN]]
	        set nChildren [$cxrSet size]
	        if {$nChildren >= $min} {
                    $cmpSet2 = [$cmpSet2 - $selection]
	        }
            }
            $cmpSet2 = [$cmpSet2 get _REQUIRES]
            $cmpSet = [$cmpSet + [$cmpSet2 instances Component]]
        }

        # add all components that drag us in by module dependancy

	$modSet = [$cmpSet get MODULES]
	$modSet = [mxrSupertree $modSet]

        $cmpSet2 = [$modSet instances Module]
        $cmpSet = [$cmpSet + [$cmpSet2 get _MODULES]]

        $cmpSet2 = [cSymsDemangle [$modSet get exports] $toolchain]
        $cmpSet = [$cmpSet + \
		"[$cmpSet2 get _LINK_SYMS] [$cmpSet2 get _LINK_DATASYMS]"]

        set components [$cmpSet instances Component]

        $cmpSet delete
        $cmpSet2 delete
        $modSet delete
        $cxrSet delete

        return $components
    }

    set returnValue [docWalk $cmpSet walk "$mxrDoc $cmpSetIncluded"]
    $cmpSetIncluded delete
    return $returnValue
}

###############################################################################
#
# cxrSubtree - what components are required by a given set of components
#
# If a user wants to include a set of components, this routine
# can be used to tell what other components also need to be
# included by dependecy.
# Because component dependencies are built on top of module
# dependencies, this routine also requires an MxrDoc containing
# information about the component's modules.
# The config engine uses this routine with parameter <mxrDoc> as
# the return value of "mxrDocCreate $hProj".
#
# PARAMETERS
#	cmpSet : DataSet of components
#	mxrDoc : MxrDoc
#
# RETURNS
#	the list of all components that are required by components in cmpSet
#
# SEE ALSO
#	mxrDataDocCreate
#

proc cxrSubtree {cmpSet mxrDoc {how max}} {

    proc walk {cmpSetOrig arg} {
        set mxrDoc [lindex $arg 0]
        set how [lindex $arg 1]
        set toolchain [mxrTcGet $mxrDoc]

        set cmpSet     [$cmpSetOrig setCreate]
        set cmpSet2    [$cmpSetOrig setCreate]
        set modSet     [$mxrDoc setCreate]

        # add all REQUIRED components

        $cmpSet = [$cmpSetOrig instances Component]
        $cmpSet2 = [$cmpSet get REQUIRES]
        set components [$cmpSet2 instances Component]
        $cmpSet = [$cmpSet + $components]

        # Add anything needed for required selections

        if {"$how" == "max"} {
            # add all INCLUDE_WHEN components

            $cmpSet2 = [$cmpSet get _INCLUDE_WHEN]
            foreach cmp [$cmpSet2 instances Component] {
            	$cmpSet2 = $cmp
            	$cmpSet2 = [$cmpSet2 get INCLUDE_WHEN]
            	if {[llength [$cmpSet2 & [$cmpSet instances]]] == \
                        [$cmpSet2 size]} {
            	    $cmpSet = [$cmpSet + $cmp]
            	}
            }
        }

        # add all components dragged in by module dependancy

        set syms [join "[$cmpSet get LINK_SYMS] [$cmpSet get LINK_DATASYMS]"]
        $modSet = [cSymsMangle $syms $toolchain]

        $modSet = [$modSet get exportedBy]
        $modSet = [$modSet + [$cmpSet get MODULES]]
        $modSet = [mxrSubtree $modSet]
        $cmpSet2 = [$modSet instances Module]
        $cmpSet2 = [$cmpSet2 get _MODULES]

		$cmpSet = [$cmpSet2 + [$cmpSet instances Component]]
		if {"$how" == "max"} {
			$cmpSet = [$cmpSet + [cxrIncludeDefaultComponentsOfIncludedSelections $cmpSet]]
		}
        set components [$cmpSet instances Component]

        $cmpSet delete
        $cmpSet2 delete
        $modSet delete

        return $components
    }

    docWalk $cmpSet walk "$mxrDoc $how"
}


# helper proc to cxrSubtree
proc cxrIncludeDefaultComponentsOfIncludedSelections { \
    setOriginalBaseComponents } {

    # allocate needed sets
    set setSelections [$setOriginalBaseComponents setCreate]
    set setKillSelections [$setOriginalBaseComponents setCreate]
    set setCfgdSelComps [$setOriginalBaseComponents setCreate]

    # Add to these selections any that are actually required by our
    # components
    $setSelections = [$setOriginalBaseComponents get REQUIRES]
    $setSelections = [$setSelections instances Selection]

    # now remove any selections which already have components selected in them
    foreach selection [$setSelections instances] {
        $setCfgdSelComps = $selection    
        $setCfgdSelComps = [$setCfgdSelComps get CHILDREN]
        $setCfgdSelComps = \
            [$setCfgdSelComps & [$setOriginalBaseComponents instances]]

        if {[$setCfgdSelComps size] > 0} {
            $setKillSelections = [$setKillSelections + $selection]
        } 
    }

    $setSelections = [$setSelections - [$setKillSelections instances]]

    # Now we have all unsatisfied required selections
    # of included components
    # Get the list of defaults for those selections
    set listDefaultSelections [$setSelections get DEFAULTS]
    
    # clean up memory
    $setKillSelections delete
    $setSelections delete
    $setCfgdSelComps delete

    return $listDefaultSelections
}

###############################################################################
#
# cxrDependsGet - get dependencies when adding cmpAddSet to cmpExistSet
#
# XXX - not optimized - should be faster
#
# NOMANUAL
#

proc cxrDependsGet {cmpAddSet cmpExistSet mxrDoc} {
    set cmpSet [$cmpAddSet setCreate]
    $cmpSet = [$cmpAddSet + [$cmpExistSet contents]]
    $cmpSet = [cxrSubtree $cmpSet $mxrDoc max]
    set dependencies \
	[$cmpSet - [$cmpAddSet + [cxrSubtree $cmpExistSet $mxrDoc minimal]]]
    $cmpSet delete
    return $dependencies
}

###############################################################################
#
# cxrSubfolders - find everything under a given folder recursivly
#
# Components are grouped into folders. This routine returns the
# set of componenents and folders (recursively) under a given folder.
#
# PARAMETERS
#	cmpSet : DataSet of component Folder objects.
#
# RETURNS
#	the list of components and subfolders of the given folder
# 

proc cxrSubfolders {cmpSet} {
    set cmpSetNext [$cmpSet setCreate]
    set children ""
    set nextChildren [$cmpSet instances Folder]
    while {"$nextChildren" != ""} {
	set children [concat $children $nextChildren]
	$cmpSetNext = $nextChildren
	set nextChildren [$cmpSetNext get CHILDREN]
    }
    
    $cmpSetNext delete
    return $children
}

###############################################################################
#
# cxrSizeGet - get the size of a set of components
#
# This routine returns the appoximate size of the smallest kernel
# that contains the given set of components.
# The size is always an underestimate, because it only takes into account
# the size of the .o's in the vxWorks archive - and not the size
# of the BSP and configuration files.
# The config engine uses this routine with parameter <mxrDoc> as
# the return value of "mxrDocCreate $hProj".
#
# PARAMETERS
#	cmpSet : DataSet of components
#	mxrDoc : DataDoc corresponding the the vxWorks archive
#
# RETURNS: three sizes; text, data, and bss
# 
# SEE ALSO
#	mxrDataDocCreate
#

proc cxrSizeGet {cmpSet mxrDoc} {
    set modules [$mxrDoc setCreate]
    $modules = [cSymsMangle \
      "[$cmpSet get LINK_SYMS] [$cmpSet get LINK_DATASYMS]" [mxrTcGet $mxrDoc]]
    $modules = [$modules get exportedBy]
    $modules = [$modules + [$cmpSet get MODULES]]
    $modules = [mxrSubtree $modules]
    set size [mxrSizeGet $modules]
    $modules delete
    return $size
}

###############################################################################
#
# cxrSetValidate - validate a component set
#
# This is a support routine for wccValidate. It is not meant to
# be called directly.
#
# PARAMETERS
#	cmpSet : DataSet of components
#
# RETURNS: a list of pairs {badComponent reason}, or an empty string
#
# NOMANUAL
# 

proc cxrSetValidate {cmpSet} {
    set cxrSet [$cmpSet setCreate]

    set val ""

    # find all Selections with the wrong # of children

    $cxrSet = "[$cmpSet get REQUIRES] [$cmpSet get _CHILDREN]"
    $cxrSet = [$cxrSet instances Selection]
    foreach selection [$cxrSet instances Selection] {
	$cxrSet = $selection
        set min 0
	set max 100000
	if {[scan [$cxrSet get COUNT] "%d-%d" min max] == 0} {
	    puts "ERROR: can't read COUNT from $selection"
	    continue
	}
	$cxrSet = [$cmpSet & [$cxrSet get CHILDREN]]
	set nChildren [$cxrSet size]
	if {$nChildren < $min} {
	    lappend val "$selection {you must include at least $min components from this selection, but currently your configuration contains $nChildren}"
	}
	if {$nChildren > $max} {
	    lappend val "$selection {you must include at most $max components from this selection, but currently your configuration contains $nChildren}"
	}
    }

    # find all components that EXCLUDE each other

    $cxrSet = [$cmpSet get EXCLUDES]
    $cxrSet = [$cxrSet & [$cmpSet instances Component]]
    foreach cmp [$cxrSet contents] {
	$cxrSet = $cmp
	$cxrSet = "[$cxrSet get EXCLUDES] [$cxrSet get _EXCLUDES]"
	set cmp2 [$cmpSet & [$cxrSet instances]]

	$cxrSet = $cmp2
	if {[set cmp2Name [$cxrSet get NAME]] == ""} {
	    set cmp2Name $cmp2
	}
	$cxrSet = $cmp
	if {[set cmpName [$cxrSet get NAME]] == ""} {
	    set cmpName $cmp
	}

	lappend val "$cmp {incompatible with $cmp2Name}"
	lappend val "$cmp2 {incompatible with $cmpName}"
    }
    $cxrSet delete

    return $val
}

###############################################################################
#
# mxrLibsExpand - get the full path of the mxr libs
#
# Each project has a set of archives used to create the
# MxrDoc for the given project.
# 
# NOMANUAL
#

proc mxrLibsExpand {hProj libs tc} {
    set cpu  [${tc}::cpu]
    set tool [${tc}::tool]
    set bspDir [prjTagDataGet $hProj BSP_DIR]

    # expand VX_OS_LIBS if present
    if [regexp {\$\(VX_OS_LIBS\)} $libs] {
        set vxOsLibs \
	    [prj_vxWorks_hidden::bspVxOsLibsGet $hProj $bspDir $cpu $tool]
        regsub {\$\(VX_OS_LIBS\)} $libs $vxOsLibs libs
    }
    set expandedLibs ""
    foreach lib $libs {
	regsub {\$\(CPU\)} $lib $cpu lib
	regsub {\$\(TOOL\)} $lib $tool lib
        set lib [prjFullPathRelativeTo $hProj $lib]
	lappend expandedLibs $lib
    }
    set expandedLibs \
	[mxrLibsSearch \
	    [prjBuildMacroGet $hProj [prjBuildCurrentGet $hProj] LD_LINK_PATH] \
	    $expandedLibs]
    regsub -all {[\{\}]} $expandedLibs "" expandedLibs
    return $expandedLibs
}

###############################################################################
#
# mxrLibsContract - get the tail part of the mxr libs and replace cpu and tool
#		    with $(CPU) and $(TOOL)
#
# Each project has a set of archives used to create the
# MxrDoc for the given project.
# 
# NOMANUAL
#

proc mxrLibsContract {hProj libs tc} {
    set cpu  [${tc}::cpu]
    set tool [${tc}::tool]
    set contractedLibs ""
    foreach lib $libs {
	regsub $cpu $lib "\$\(CPU\)" lib
	regsub $tool $lib "\$\(TOOL\)" lib
        set lib [prjRelPathRelativeTo $hProj $lib]
	lappend contractedLibs $lib
    }
    regsub -all {[\{\}]} $contractedLibs "" contractedLibs
    return $contractedLibs
}

###############################################################################
#
# cxrCmpInfoGet - get info about a component set
#
# This routine prints information about a set of components to stdout.
# Currently it just prints the subtree and supertree, but more
# information will be added in the future.
# This routine is not used by anyone.
#
# NOMANUAL
# 

proc cxrCmpInfoGet {cmpSet mxrDoc} {
    set info ""

    lappend info "subtree [cxrSubtree $cmpSet $mxrDoc]"
    lappend info "supertree [cxrSupertree $cmpSet $mxrDoc]"
    lappend info "module [$cmpSet get MODULES]"
    return $info
}

###############################################################################
#
# cxrUnavailListGet - get a list of unavailable components
#
# This routine returns the list of components that are either not installed
# in the tree, or that require uninstalled components.
#
# PARAMETERS
#	cxrDoc : CxrDoc
#	mxrDoc : MxrDoc
#
# SEE ALSO
#	cmpUnavailListGet
#
# NOMANUAL
# 

proc cxrUnavailListGet {cxrDoc mxrDoc} {
    global cxrUnavailInfo

    if {[info exists cxrUnavailInfo]} {
	if {("$cxrDoc" == [lindex $cxrUnavailInfo 0]) && \
		("$mxrDoc" == [lindex $cxrUnavailInfo 1])} {
	    return [lindex $cxrUnavailInfo 2]
	}
    }

    set cmpSet [$cxrDoc setCreate]
    set cmp    [$cxrDoc setCreate]
    set modSet [$mxrDoc setCreate]

    # start with components with unresolved references

    set _requireUnresolved "" 
    foreach comp [$cxrDoc instances Component] {
        $cmpSet = $comp
        $cmpSet = [$cmpSet get REQUIRES]
        if {[$cmpSet instances Unresolved] != ""} {
            lappend _requireUnresolved $comp
        }
    }

    $cmpSet = $_requireUnresolved
    $cmpSet = [$cmpSet instances Component]

    # add components with missing MODULES

    set allMods [$mxrDoc instances Module]
    $modSet = [$cxrDoc instances Module]
    $cmp = [$modSet - $allMods]
    $cmpSet = [$cmpSet + [$cmp get _MODULES]]

    # add components with missing configlettes or archives

    set tc [mxrTcGet $mxrDoc]
    set hProj [mxrPrjHandleGet $mxrDoc]
    set cpu  [${tc}::cpu]
    set tool [${tc}::tool]
    set others ""
    set bspDir [prjTagDataGet $hProj BSP_DIR]
    foreach component [$cxrDoc instances Component] {
	$cmp = $component
	if {[set lib [$cmp get ARCHIVE]] != ""} {
	    set lib [mxrLibsExpand $hProj $lib $tc]
	    if {![file exists $lib]} {
		lappend others $component
	    }
	}
	if {[set configlettes [$cmp get CONFIGLETTES]] != ""} {
	    foreach file $configlettes {
		if {![file exists [wtxPath]target/config/comps/src/$file] &&
		    ![file exists $bspDir/$file] &&
			![file exists [wtxPath]target/src/drv/$file]} {
		     lappend others $component
		     break
		}
	    }
	}
	if {[set bspStubs [$cmp get BSP_STUBS]] != ""} {
	    foreach file $bspStubs {
		if {![file exists [wtxPath]target/config/comps/src/$file]} {
		     lappend others $component
		     break
		}
	    }
	}
    }
    $cmpSet = [$cmpSet + $others]

    $modSet = [$mxrDoc instances Module]
    $cmp = [$cxrDoc instances Component]
    $cmp = "[$cmp get LINK_SYMS] [$cmp get LINK_DATASYMS]"
    $cmp = [$cmp - [cSymsDemangle [concat [$modSet get defines] \
		[$modSet get exports]] [mxrTcGet $mxrDoc]]]
    $cmpSet = [$cmpSet + "[$cmp get _LINK_SYMS] [$cmp get _LINK_DATASYMS]"]

    # If muxLib.o is missing, remove everything under FOLDER_NETWORK
    # This is for proper handing of the integrated simulators, which
    # should contain no network components.

    $modSet = muxLib.o
    if {[$modSet instances] == ""} {
	$cmp = FOLDER_NETWORK
	$cmpSet = [$cmpSet + [cxrSubfolders $cmp]]
    }

    # get all components which we currently think are installed
    # for use as an argument to cxrSupertree

    $cmp = [$cxrDoc instances Component]
    $cmp = [$cmp - $cmpSet]
    set installedComponents [$cmp instances]

    # return all components that use the unavailable components

    $cmpSet = [cxrSupertree $cmpSet $mxrDoc $installedComponents]
    set components [$cmpSet instances Component]
    $cmpSet delete
    $cmp    delete
    $modSet delete

    set cxrUnavailInfo [list $cxrDoc $mxrDoc $components]
    return $components
}

###############################################################################
#
# modAnalyze - analyze archives and/or object modules
#
# Create an MxrDoc corresponding to a set of object modules and archives.
# This routine is just like mxrDataDocCreate, but the toolchain
# parameter does not require a "tc_" prepended.
#
# PARAMETERS
#	fileList : list of object moduless and archives to analyze
#	toolchain : toolchain used to build the objects (e.g., PPC604gnu)
#
# RETURNS: an MxrDoc handle
#
# SEE ALSO
#	mxrDataDocCreate
#

proc modAnalyze {fileList toolchain} {
    return [mxrDataDocCreate ::tc_${toolchain} $fileList]
}

###############################################################################
#
# modTrace - trace module dependencies
#
# Given two object modules within an archive, find the shortest path
# between them. The result is printed to stdout in the form
# "fromMod mod1 mod2 ... toMod". In verbose mode it also prints
# the reason for the dependencies by inserting a list of symbols
# between each module dependecy. That way you can find out what
# symbols caused the dependencies between, for example, mod1 and
# mod2.
#
# This routine can be useful when tracking scalability bugs.
# For example, if you look at the output of "modSubtree" on
# a given object module, and find it contains many more module
# dependencies than you expected, then this routine can show
# you why those dependencies occured.
#
# PARAMETERS
#	modDataDoc : MxrDoc handle returned from modAnalyze
#	fromMod : module to trace from
#	toMod : module to trace to
#	verbose : (optional) set to verbose to print sym info
#
# RETURNS: N/A
#

proc modTrace {modDataDoc fromMod toMod {verbose ""}} {
    set mxrDoc $modDataDoc
    set worthVisting [$mxrDoc setCreate]
    set visited [$mxrDoc setCreate]

    $worthVisting = [modSubtree $modDataDoc $fromMod]
    if {"[$worthVisting & $toMod]" != $toMod} {
	puts "no path between $fromMod and $toMod"
	return
    }

    # starting with fromMod, create a bunch of trace paths.
    # only pursue shortest paths.

    set nextTraceId 0
    # set traces(traceId) "fromMod mod1 mod2 ..."

    proc traceCreate {modList} {
	upvar nextTraceId nextTraceId
	upvar traces traces
	incr nextTraceId
	set traces($nextTraceId) $modList
	return $nextTraceId
    }

    proc traceDelete {traceId} {
	upvar traces traces
	unset traces($traceId)
    }

    proc traceListGet {} {
	upvar traces traces
	return [array names traces]
    }

    proc traceGet traceId {
	upvar traces traces
	return $traces($traceId)
    }

    traceCreate $fromMod
    $visited = $fromMod
    set theTrace ""

    while {"[$visited & $toMod]" != "$toMod"} {
	foreach trace [traceListGet] {
	    set modList [traceGet $trace]
	    traceDelete $trace

	    set lastMod [lindex $modList end]
	    foreach mod [modCalls $modDataDoc $lastMod] {
		if {"[$visited & $mod]" == $mod} {
		    continue
		}
		if {"[$worthVisting & $mod]" != $mod} {
		    continue
		}
		if {"$mod" == "$toMod"} {
		    set theTrace "$modList $mod"
		}
		$visited = [$visited + $mod]
		traceCreate "$modList $mod"
	    }
	}
    }

    set toMod $visited
    set fromMod $worthVisting
    if {"$verbose" != ""} {
	for {set ix [llength $theTrace]} {$ix > 1} {incr ix -1} {
	    $toMod = [lindex $theTrace [expr $ix - 1]]
	    $fromMod = [lindex $theTrace [expr $ix - 2]]
	    $fromMod = "[$fromMod get imports] [$fromMod get declares]"
	    set syms [$fromMod & "[$toMod get exports] [$toMod get declares]"]
	    set theTrace [linsert $theTrace [expr $ix - 1] $syms]
	}
    }

    $worthVisting delete
    $visited delete
    return $theTrace
}

###############################################################################
#
# modSubtree - find (recursivley) all modules called by a set of modules
#
# This routine is a recusive version of modCalls. It lets you know what
# modules are required by a given set of modules.
# Suppose module1 calls module2, and module2 calls module3.
# Then the subtree of module1 would contain both module2 and module3.
#
# PARAMETERS
#	modDataDoc : return value of modAnalyze
#	mods : list of module
#
# RETURNS
#	list of modules called by mods (recursive)
# 

proc modSubtree {modDataDoc mods} {
    set mxrDoc $modDataDoc
    set mxrSet [$mxrDoc setCreate]
    $mxrSet = $mods
    set subtree [mxrSubtree $mxrSet]
    $mxrSet delete
    return $subtree
}

###############################################################################
#
# modSupertree - find (recursivley) all modules that call a set of modules
#
# This routine is a recusive version of modCalledBy. It lets you know what
# modules require a given set of modules.
# Suppose module1 calls module2, and module2 calls module3.
# Then the supertree of module3 would contain both module2 and module1.
#
# PARAMETERS
#	modDataDoc : return value of modAnalyze
#	mods : list of module
#
# RETURNS
#	list of modules that call mods (recursive)
# 

proc modSupertree {modDataDoc mods} {
    set mxrDoc $modDataDoc
    set mxrSet [$mxrDoc setCreate]
    $mxrSet = $mods
    set supertree [mxrSupertree $mxrSet]
    $mxrSet delete
    return $supertree
}

###############################################################################
#
# modCalledBy - find all modules that call a given set of modules
#
# PARAMETERS
#	modDataDoc : return value of modAnalyze
#	mods : list of module
#
# RETURNS
#	list of modules that call mods
# 

proc modCalledBy {modDataDoc mods} {
    set mxrDoc $modDataDoc
    set mxrSet [$mxrDoc setCreate]
    $mxrSet = $mods
    set users [mxrUsers $mxrSet]
    $mxrSet delete
    return $users
}

###############################################################################
#
# modCalls - find all modules that are called by a given set of modules
#
# PARAMETERS
#	modDataDoc : return value of modAnalyze
#	mods : list of module
#
# RETURNS
#	list of modules that mod calls
# 

proc modCalls {modDataDoc mods} {
    set mxrDoc $modDataDoc
    set mxrSet [$mxrDoc setCreate]
    $mxrSet = $mods
    set uses [mxrUses $mxrSet]
    $mxrSet delete
    return $uses
}

###############################################################################
#
# modInfo - get info about module relationships
#
# This routine can be used to get any other information
# associated with a set of modules, according to the MxrDoc schema.
# 
# PARAMETERS
#	modDataDoc : return value of modAnalyze
#	mods : list of module (typically just one)
#	info : info to get (e.g., exports, imports, declares, textSize, ...)
#
# RETURNS
#	list of modules that mod calls
#

proc modInfo {modDataDoc modSet info} {
    set s [$modDataDoc setCreate]
    $s = $modSet
    $s = [$s get $info]
    set info [$s instances]
    $s delete
    return $info
}

###############################################################################
#
# defaultFolderSubTree - recursively descend and collect default components
# for this folder and its children.  resultSet must contain the folder name.
# result is returned in 'resultSet' (use [$resultSet instances])
#
# NOMANUAL
#

proc ::defaultFolderSubTree {resultSet} {
    while {[set children [$resultSet get DEFAULTS]] != ""} {
        $resultSet = [$resultSet instances Component]
        $resultSet = [$resultSet + $children]
    }
}

###############################################################################
#
# foldersDeletableComponentsGet - determine components in folder
# (specified by cSet) that are recommended (safe) for deletion.
#
# RETURNS
# Two lists: safe components, and unsafe components
#
# NOMANUAL
#

proc ::foldersDeletableComponentsGet {cSet wccList} {
    $cSet = [::cxrSubfolders $cSet]
    $cSet = [$cSet instances Component] 
    $cSet = [$cSet & $wccList] 
	return [list [$cSet instances Component] ""]
}

###############################################################################
#
# componentSizeTest -  Compute the smallest kernel for a given a list of 
#                      components or FOLDERs.
#
# NOMANUAL
#

proc componentSizeTest {cmpList mxrDoc cxrDoc} {
    set mxrSet [$mxrDoc setCreate]
    set cmpSet [$cxrDoc setCreate]

    $cmpSet = $cmpList
    
    # if any component is a FOLDER, get their DEFAULTS subcomponents

    while  {[set children [$cmpSet get DEFAULTS]] != ""} {
	$cmpSet = [$cmpSet instances Component]
	$cmpSet = [$cmpSet + $children]
    }

    $cmpSet = [cxrSubtree $cmpSet $mxrDoc]
    $mxrSet = [$cmpSet get MODULES]
    $mxrSet = [mxrSubtree $mxrSet]
    
    set size [mxrSizeGet $mxrSet]
    puts "The smallest kernel is $size"
    puts "with $cmpList"
}

