# 01Cplus.tcl - C++ support routines host shell Tcl implementation */
#
# Copyright 1995-2002 Wind River Systems, Inc. */
#
# modification history
# -----------------------
# 01k,06mar02,fmk  cplusXtors mistakenly calls Ctors and Dtors of vxworks -
#                  SPR 74041
# 01j,20jan99,dra  added -- to switch on $strategy.
# 01i,03dec98,jmp  fixed shellproc arguments.
# 01h,26nov96,dbt  removed erroneous comment in cplusXtorSet.
# 01g,20nov96,dbt  revowrked previous modification.
# 01f,15nov96,dbt  undo 01e change, removed use of shParse in cplusXtorSet and
#                  modified cplusStratShow to avoid error if cplus is not
#                  included on target.
# 01e,24oct96,dbt  removed print of new lines in cplusStratShow.
# 01d,17oct96,pad  Changed messages in cplusStratShow. Also make it use
#                  shSymValue instead of shParse.
# 01c,25jun96,pad  Fixed failure on Windows hosts (SPR #6461).
# 01b,08mar96,pad  Modified to handle either one or two leading underscores in
#		   front of the ctor and dtor symbols.
# 01a,21jul95,pad  written.
#*/

##############################################################################
#
# cplusXtorSet - change C++ static constructor calling strategy (C++)
#
# This command sets the C++ static constructor calling strategy
# to <strategy>.  The default strategy is 0.
#
# There are two static constructor calling strategies: <automatic>
# and <manual>.  These modes are represented by numeric codes:
# .TS
# center,tab(|);
# lf3 lf3
# l l.
# Strategy   | Code
# _
# manual     | 0
# automatic  | 1
# .TE
#
# Under the manual strategy, a module's static constructors and
# destructors are called by cplusCtors() and cplusDtors(), which are
# themselves invoked manually.
#
# Under the automatic strategy, a module's static constructors are
# called as a side-effect of loading the module using the target server
# loader.  A module's static destructors are called as a side-effect of
# unloading the module.
#

shellproc cplusXtorSet {args} {

    # Get the strategy value

    set strategy [stringGet [lindex $args 0]]

    # Do some check

    if {([scan $strategy "%d" dummy] != 1) || ($strategy > 1) || \
	($strategy < 0)} {
	error "Valid arguments are 0 (manual) and 1 (automatic)"
	}

    set addr [shSymAddr cplusXtorStrategy]
    set blockId [memBlockCreate -[string range [targetEndian] 0 0] 4]
    memBlockSet -l $blockId 0 $strategy
    wtxMemWrite $blockId $addr 0 4
    memBlockDelete $blockId

    return 0
    }

##############################################################################
#
# cplusStratShow - show C++ ctors/dtors strategy
#

shellproc cplusStratShow {args} {

    if {[catch {set strategy [shSymValue cplusXtorStrategy]}]} {
	return 0
	}

    switch -- $strategy {
	0 {
	  puts "\nC++ Constructors/Destructors Strategy is MANUAL\n"
	  }
	1 {
	  puts "\nC++ Constructors/Destructors Strategy is AUTOMATIC\n"
	  }
	default {
	        puts "\nInvalid C++ Constructors/Destructors Strategy: $strategy\n"
		}
	}

    return 0
    }

##############################################################################
#
# cplusXtors - support routine for cplusCtors and cplusDtors
#
# This routine gets all constructor/destructor function addresses from the
# ctors/dtors arrays, and calls them in turn.
#
# SYNOPSIS:
#   cplusXtors symbol moduleName
#
# PARAMETERS:
#   symbol: symbol which value represents either the ctors or the dtors array
#           address in the module. Must be 'ctors' or 'dtors'.
#   moduleName: module name. May be null if no module name is specified
#		(all modules are looked for then).
#

proc cplusXtors {symbol moduleName} {

    # Convenient stuff

    set bufferSize 400
    set moduleList ""

    switch $symbol {
	ctors	{
		set lookFor constructor
		}
	dtors	{
		set lookFor destructor
		}
	default {
		error "Only handle symbols ctors and dtors"
		}
	}

    # Check whether all modules are concerned, or one is specified

    if {$moduleName == 0} {

	# Get all loaded module names

	set moduleIdList [wtxObjModuleList]
	foreach moduleId $moduleIdList {
	    set module [lindex [wtxObjModuleInfoGet $moduleId] 1]
	    if { [string first vxWorks $module] == -1 } {
	        lappend moduleList $module
	        }
	}
        } else {

	# Check if module exists

	if {[catch {wtxObjModuleInfoGet $moduleName} status]} {
	    error "Object module $moduleName not found"
	    }

	# Prepare module name list (only 1 element here).

	lappend moduleList $moduleName
        }

    # Call constructors/destructors for all modules in list
    
    if { $moduleList == "" } {
        error "Cannot call $symbol, there are no downloaded modules"
        }

    foreach module $moduleList {

	# Reset the constructor/destructor address list

	set xtorFuncList {}

	# Get address of constructors/destructors array. Try first with two
	# leading underscores, then with only one (PowerPC).

	set xtors [lindex [split [wtxSymListGet -module $moduleName \
						-name __$symbol]] 1]

	if {$xtors == ""} {
	    set xtors [lindex [split [wtxSymListGet -module $moduleName \
						    -name _$symbol]] 1]
	}

	# If module has no constructors/destructors, jump to next module
	# in list

	if {$xtors == ""} {
	    continue
	    }

	# Loop thru constructor/destructor addresses until we reach the
	# sentinel (0).

	set loopAgain 1
	while {$loopAgain != 0} {

	    # Read current constructor/destructor addresses. Reading is
	    # optimized (100 addresses in one shot) to save time.

	    if {[catch {wtxMemRead $xtors $bufferSize} memBlk]} {
		error "Can't read $lookFor addresses in target memory"
		}
	    set xtorFuncVector [memBlockGet -l $memBlk]
	    
	    memBlockDelete $memBlk

	    set sentinelIdx [lsearch $xtorFuncVector 0x00000000]

	    if {$sentinelIdx < 0} {

		# All addresses in block belong to constructors/destructors.
		# Store them in a list and loop again to get the remaining
		# addresses.

		set xtorFuncList [concat $xtorFuncList $xtorFuncVector]

		} else {

		# Not all addresses belong to constructors/destructors. Add
		# only constructor/destructor addresses to the list.

		set lastAddrIdx [incr sentinelIdx -1]
		set xtorFuncList [concat $xtorFuncList \
				    [lrange $xtorFuncVector 0 $lastAddrIdx]]
		set loopAgain 0
		}

	    set xtors [incr xtors $bufferSize]
	    }

	# If managing destructors, the list is then reverted so that 
	# destructors can be called in the reverse order of the constructors.

	if {$lookFor == "destructor"} {
	    set tmpList {}
	    for {set i [expr [llength $xtorFuncList] - 1]} {$i >= 0} \
		{incr i -1} {
		lappend tmpList [lindex $xtorFuncList $i]
		}
	    set xtorFuncList $tmpList
	    }

	# Call each constructor/destructor function

	foreach xtorFunc $xtorFuncList {
	    shFuncCall -int $xtorFunc 0 0 0 0 0 0 0 0 0 0
	    }
	}

    return 0
    }

##############################################################################
#
# cplusCtors - call static constructors (C++)
#
# This function is used to call static constructors under the manual
# strategy (see cplusXtorSet).  <moduleName> is the name of an
# object module that was "munched" before loading.  If <moduleName> is 0,
# then all static constructors, in all modules loaded by the target server
# loader, are called.
#
# EXAMPLES
# The following example shows how to initialize the static objects in
# modules called "applx.out" and "apply.out".
#
# .CS
#     -> cplusCtors "applx.out"
#     value = 0 = 0x0
#     -> cplusCtors "apply.out"
#     value = 0 = 0x0
#     -> cplusCtors "apply.out"
#     value = 0 = 0x0
# .CE
#
# The following example shows how to initialize all the static objects that are
# currently loaded, with a single invocation of cplusCtors:
#
# .CS
#     -> cplusCtors
#     value = 0 = 0x0
# .CE
#
# RETURNS: N/A
#
# SEE ALSO: cplusXtorSet
#

shellproc cplusCtors {args} {

    # Call routine that does actual work with required parameters

    cplusXtors ctors [stringGet [lindex $args 0]]

    return 0
    }

##############################################################################
#
# cplusDtors - call static destructors (C++)
#
# This function is used to call static destructors under the manual
# strategy (see cplusXtorSet).  <moduleName> is the name of an
# object module that was "munched" before loading.  If <moduleName> is 0,
# then all static destructors, in all modules loaded by the Target server
# loader, are called.
#
# EXAMPLES
# The following example shows how to destroy the static objects in
# modules called "applx.out" and "apply.out":
#
# .CS
#     -> cplusDtors "applx.out"
#     value = 0 = 0x0
#     -> cplusDtors "apply.out"
#     value = 0 = 0x0
# .CE
#
# The following example shows how to destroy all the static objects that are
# currently loaded, with a single invocation of cplusDtors:
#
# .CS
#     -> cplusDtors
#     value = 0 = 0x0
# .CE
#
# RETURNS: N/A
#
# SEE ALSO: cplusXtorSet
#

shellproc cplusDtors {args} {

    # Call routine that does actual work with required parameters

    cplusXtors dtors [stringGet [lindex $args 0]]

    return 0
    }
