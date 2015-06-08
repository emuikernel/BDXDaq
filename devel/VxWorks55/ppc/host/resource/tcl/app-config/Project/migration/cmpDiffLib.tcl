# cmpDiffLib.tcl - routines to enable telling of the differences between 
#                  components from two Tornado installations
#
# Copyright 2002 Wind River Systems, Inc.
#
# modification history
# ------------------------
# 01b,27mar02,j_s  fix cmpAttrDiff
# 01a,11dec01,j_s  if fdLog is 0 then set it to 'stdout'
#
# DESCRIPTION
# This file contains the TCL routines that tell the differences between
# components from two Tornado installations. Those routines are used by
# project migration tool.

namespace eval cmpDiff {
    set WIND_BASE1 ""
    set WIND_BASE2 ""
    array set argument {}
    set versionValid {2.0 2.0.2 2.1 2.2 AE1.0 AE1.1}
    set archValid(2.0) {arm i86 i960 mc68k mips ppc sh simhppa simnt \
			simsolaris sparc}
    set archValid(2.0.2) {arm i86 i960 mc68k mips ppc sh simhppa simnt \
			simsolaris sparc}
    set archValid(2.1) {arm i86 i960 mc68k mips ppc sh simhppa simnt \
			simsolaris sparc}
    set archValid(2.2) {arm i86 i960 mc68k mips ppc sh simhppa simnt \
			simsolaris sparc}
    set toolValid {diab gnu}
    set TOOL_FAMILY ""
    set cdfPath(2.0) {\
	       "set tmp /target/config/comps/vxWorks/" \
	       "set tmp /target/config/comps/vxWorks/arch/\$argument(arch)"\
	       }
    set cdfPath(2.0.2) {\
	       "set tmp /target/config/comps/vxWorks/" \
	       "set tmp /target/config/comps/vxWorks/arch/\$argument(arch)"\
	       "set tmp /target/config/comps/vxWorks/tool/\$TOOL_FAMILY"\
	       }
    set cdfPath(2.1) {\
	       "set tmp /target/config/comps/vxWorks/" \
	       "set tmp /target/config/comps/vxWorks/arch/\$argument(arch)"\
	       }
    set cdfPath(2.2) {\
	       "set tmp /target/config/comps/vxWorks/" \
	       "set tmp /target/config/comps/vxWorks/arch/\$argument(arch)"\
	       "set tmp /target/config/comps/vxWorks/tool/\$TOOL_FAMILY"\
	       }
    set cmpNamePrint TRUE
}

##############################################################################
#
# cmpDiff::argsGet - obtains and process all command line arguments
#
# SYNOPSIS
#   argsGet
#
# PARAMETERS: N / A
#
# RETURNS:   
#
# ERRORS:     N / A
#
# NOMANUAL
#

proc cmpDiff::argsGet {} {
    variable argument
    variable versionValid
    variable archValid
    variable toolValid
    variable WIND_BASE1
    variable WIND_BASE2

    set count [llength $::argv]

    #clear all variables

    set argument(arch) ""
    set argument(cpu) ""
    set argument(tool) ""
    set argument(v1) ""
    set argument(v2) ""
    set argument(names) ""

    while {$count > 0} {
	set arg [string tolower [lindex $::argv 0]]

	switch -regexp -- $arg {
	    ^-a {
		# shift
		set ::argv [lreplace $::argv 0 0]
		incr count -1
		set argument(arch) [lindex $::argv 0]
	    }
	    ^-c {
		#shift
		set ::argv [lreplace $::argv 0 0]
		incr count -1
		set argument(cpu) [lindex $::argv 0]
	    }
	    ^-t {
		#shift
		set ::argv [lreplace $::argv 0 0]
		incr count -1
		set argument(tool) [lindex $::argv 0]
	    }
	    ^-v1 {
		#shift
		set ::argv [lreplace $::argv 0 0]
		incr count -1
		set argument(v1) [lindex $::argv 0]
	    }
	    ^-v2 {
		#shift
		set ::argv [lreplace $::argv 0 0]
		incr count -1
		set argument(v2) [lindex $::argv 0]
	    }
	    default {
		lappend argument(names) [lindex $::argv 0]
	    }
	}
	# shift
	set ::argv [lreplace $::argv 0 0]
	incr count -1
    }

    # validate arguments
    if {[llength $argument(names)] < 2} {
	error "Missing argument: 2 Tornado installation directory are required."
    }
    set WIND_BASE1 [lindex $argument(names) 0]
    set WIND_BASE2 [lindex $argument(names) 1]
    if ![file isdirectory $WIND_BASE1] {
	error "Invalid argument: $WIND_BASE1 is not a directory."
    }
    if ![file isdirectory $WIND_BASE2] {
	error "Invalid argument: $WIND_BASE2 is not a directory."
    }
    if {[lsearch $versionValid $argument(v1)] == -1} {
	error "Invalid argument: $argument(v1) is not a vaild version of \
				 Tornado."
    }
    if {[lsearch $versionValid $argument(v2)] == -1} {
	error "Invalid argument: $argument(v2) is not a vaild version of \
				 Tornado."
    }
    if {[lsearch $archValid($argument(v1)) $argument(arch)] == -1} {
	error "Invalid argument: $argument(arch) is not a valid architecture \
				 for Tornado $argument(v1)."
    }
    if {[lsearch $archValid($argument(v2)) $argument(arch)] == -1} {
	error "Invalid argument: $argument(arch) is not a valid architecture \
				 for Tornado $argument(v2)."
    }
    if {[lsearch $toolValid $argument(tool)] == -1} {
	error "Invalid argument: $argument(tool) is not a valid toolchain."
    }

    return 1
}

##############################################################################
#
# cmpDiff::cmpAttrDiff - tell the differences between two Tornados on an 
#			 attribute of a component
#
# SYNOPSIS
#   cmpAttrDiff cmp attr cxrSet1 cxrSet2
#
# PARAMETERS 
#   cmp - component
#   attr - an attribute of component
#   cxrSet1 - a CxrDoc set of the first Tornado
#   cxrSet2 - a CxrDoc set of the other Tornado
#
# RETURNS: N / A
#
# ERRORS:  N / A
#
# NOMANUAL
#

proc cmpDiff::cmpAttrDiff {cmp attr cxrSet1 cxrSet2} {
    variable argument
    variable cmpNamePrint
    global fdLog
    
    $cxrSet1 = $cmp
    $cxrSet2 = $cmp
    set attrVal1 [$cxrSet1 get $attr]
    set attrVal2 [$cxrSet2 get $attr]
    if {$attr == "INIT_RTN"} {
	regsub -all {\([^\)]*\);} $attrVal1 "" attrVal1
	regsub -all {\([^\)]*\);} $attrVal2 "" attrVal2
    }
    set diff FALSE
    foreach val $attrVal1 {
        if {[lsearch $attrVal2 $val] == -1} {
	    set diff TRUE
	    break
   	}
    }
    if {$diff == "FALSE"} {
        foreach val $attrVal2 {
            if {[lsearch $attrVal1 $val] == -1} {
	        set diff TRUE
	        break
   	    }
        }
    }
    if {$diff == "TRUE"} {
        if {$cmpNamePrint} {
            puts $fdLog "\n${cmp}:"
    	    set cmpNamePrint FALSE
        }
        puts $fdLog "\t$attr $attrVal1 in $argument(v1)"
	puts $fdLog "\t$attr $attrVal2 in $argument(v2)"
    }
}

##############################################################################
#
# cmpDiff::cmpDiff - entry point from other routine
#
# SYNOPSIS
#   cmpDiff cmpList1
#
# PARAMETERS 
#   cmpList1 - optional, a list of components to compare; default is empty,
#	       when empty, comparison is done for all components
#
# RETURNS: N / A
#
# ERRORS:  N / A
#
# NOMANUAL
#

proc cmpDiff::cmpDiff {{cmpList1 ""}} {
    variable argument
    variable cmpNamePrint
    variable WIND_BASE1
    variable WIND_BASE2
    variable cdfPath
    variable TOOL_FAMILY
    variable cxrDoc1
    variable cxrDoc2
    variable cxrdoc1Set
    variable cxrdoc2Set
    global fdLog

    if [regexp {gnu} $argument(tool)] {
	set TOOL_FAMILY gnu
    } elseif [regexp {diab} $argument(tool)] {
	set TOOL_FAMILY diab
    }

    if [info exists prjMigrate::fdLog] {
	if {$prjMigrate::fdLog != 0} {
	    set fdLog $prjMigrate::fdLog
	} else {
	    set fdLog stdout
	}
    } else {
	set fdLog stdout
    }
    set pathList ""
    foreach pathEval $cdfPath($argument(v1)) {
    	if [catch { \
	    lappend pathList $WIND_BASE1[eval $pathEval]/ \
	} errorCmpDiff] {
	    puts $fdLog "$errorCmpDiff"
        }
    }
    set cxrDoc1 [cxrDataDocCreate $pathList ::tc_$argument(cpu)$argument(tool)]
    set pathList ""
    foreach pathEval $cdfPath($argument(v2)) {
	if [catch { \
	    lappend pathList $WIND_BASE2[eval $pathEval]/ \
	} errorCmpDiff] {
	    puts $fdLog "$errorCmpDiff"
        }
    }
    set cxrDoc2 [cxrDataDocCreate $pathList ::tc_$argument(cpu)$argument(tool)]
    set cxrdoc1Set [$cxrDoc1 setCreate]
    set cxrdoc2Set [$cxrDoc2 setCreate]
    if {$cmpList1 == ""} {
        set cmpList1 [$cxrDoc1 instances Component]
    }
    set cmpList2 [$cxrDoc2 instances Component]
    set cmpCommon ""

    foreach cmp $cmpList1 {
	if {[lsearch $cmpList2 $cmp] == -1} {
	    puts $fdLog "No information of $cmp in your T$argument(v2) installation."
	} else {
	    lappend cmpCommon $cmp
        }
    }
    puts $fdLog "-----------------------"

    # compare components between the two versions

    foreach cmp $cmpCommon {
	set cmpNamePrint TRUE 

	# compare header files
	
	cmpAttrDiff $cmp HDR_FILES $cxrdoc1Set $cxrdoc2Set

	# compare configlettes

	cmpAttrDiff $cmp CONFIGLETTES $cxrdoc1Set $cxrdoc2Set

	# compare configuration parameters

	cmpAttrDiff $cmp CFG_PARAMS $cxrdoc1Set $cxrdoc2Set

	# compare init routine

	cmpAttrDiff $cmp INIT_RTN $cxrdoc1Set $cxrdoc2Set
    }
}

##############################################################################
#
# cmpDiff::cmpDiffCmd - entry point for command line operation
#
# SYNOPSIS
#   cmpDiffCmd
#
# PARAMETERS 
#
# RETURNS: N / A
#
# ERRORS:  N / A
#
# NOMANUAL
#

proc cmpDiff::cmpDiffCmd {} {
    variable argument
    variable cmpNamePrint
    variable WIND_BASE1
    variable WIND_BASE2
    variable TOOL_FAMILY
    variable cdfPath

    if {$::env(WIND_HOST_TYPE) == "x86-win32"} {
	load CxrDoc.dll
    } else {
	load CxrDoc.so
    }

    argsGet

    if [regexp {gnu} $argument(tool)] {
	set TOOL_FAMILY gnu
    } elseif [regexp {diab} $argument(tool)] {
	set TOOL_FAMILY diab
    }

    global fdLog

    if [info exists prjMigrate::fdLog] {
	set fdLog $prjMigrate::fdLog
    } else {
	set fdLog stdout
    }
    set pathList ""
    foreach pathEval $cdfPath($argument(v1)) {
    	if [catch { \
	    lappend pathList $WIND_BASE1[eval $pathEval]/ \
	} errorCmpDiff] {
	    puts "$errorCmpDiff"
        }
    }
    set cxrDoc1 [cxrDataDocCreate $pathList ::tc_$argument(cpu)$argument(tool)]
    set pathList ""
    foreach pathEval $cdfPath($argument(v2)) {
	if [catch { \
	    lappend pathList $WIND_BASE2[eval $pathEval]/ \
	} errorCmpDiff] {
	    puts "$errorCmpDiff"
        }
    }
    set cxrDoc2 [cxrDataDocCreate $pathList ::tc_$argument(cpu)$argument(tool)]
    set cxrdoc1Set [$cxrDoc1 setCreate]
    set cxrdoc2Set [$cxrDoc2 setCreate]
    set cmpList1 [$cxrDoc1 instances Component]
    set cmpList2 [$cxrDoc2 instances Component]
    set cmpCommon ""

    foreach cmp $cmpList1 {
	if {[lsearch $cmpList2 $cmp] == -1} {
	    puts "No information of $cmp in your T$argument(v2) installation"
	} else {
	    lappend cmpCommon $cmp
        }
    }
    puts "-----------------------"

    # compare components between the two versions

    foreach cmp $cmpCommon {
	set cmpNamePrint TRUE 

	# compare header files
	
	cmpAttrDiff $cmp HDR_FILES $cxrdoc1Set $cxrdoc2Set

	# compare configlettes

	cmpAttrDiff $cmp CONFIGLETTES $cxrdoc1Set $cxrdoc2Set

	# compare linkage data

	cmpAttrDiff $cmp LINK_DATASYMS $cxrdoc1Set $cxrdoc2Set

	# compare linkage symbols

	cmpAttrDiff $cmp LINK_SYMS $cxrdoc1Set $cxrdoc2Set

	# compare modules

	cmpAttrDiff $cmp MODULES $cxrdoc1Set $cxrdoc2Set

	# compare configuration parameters

	cmpAttrDiff $cmp CFG_PARAMS $cxrdoc1Set $cxrdoc2Set

	# compare init routine

	cmpAttrDiff $cmp INIT_RTN $cxrdoc1Set $cxrdoc2Set
    }
}
