# wccLib.tcl - wind "component configuration" management library
#
# modification history
# --------------------
# 01v,03may02,cjs  fix spr 76601 -- problems w/ using component archive in
#                  bootable project
# 01u,08nov01,j_s  library names passed to mxrLibsContract should be in
#                  relative form
# 01u,06nov01,rbl  allow tool-dependent cdf paths
# 01t,02nov01,j_s  Added mxrLibsSearch to derive libraries from LD_LINK_PATH
#                  and LIBS; adapt mxrDocCreate to use both LIBS and MXR_LIBS
# 01s,03oct01,dat  Adding BSP_STUBS, SPR 69150
# 01r,01apr99,cjs  Comment out prjStatusLog() calls
# 01q,19feb99,ren Added handling of a parameter with no value
# 01p,10nov98,ms  updated for LINK_DATASYMS.
# 01o,26oct98,ms  removed useless wccHelpUrlsGet. Changed puts to prjStatusLog.
# 01n,25sep98,ms  library routines can now be added to a config via wccProc*
# 01m,15sep98,ms  beefed up cxrDocValidate (the .cdf file test routine).
# 01l,09sep98,cjs added wccErrorStatusSet/Get routines
# 01k,26aug98,ms  respect ARCHIVE attribute of components.
#		  updated cxrValidate.
# 01j,18aug98,ms  wccLoad fixes; when initializaing symTbl, use wtxDirectCall
#		  instead of slower wtxFuncCall. Use LD_PARTIAL_FLAGS instead
#	 	  of assuming "-r" (makes SIMSOLARIS work).
# 01j,11aug98,ms  fixed bugs in wccLoad.
# 01i,07aug98,ms  fixed bug in wccParamGet.
# 01h,06aug98,ms  added wccComponentLoad.
# 01h,31jul98,ms  check for components with missing parameters in wccValidate
# 01g,27jul98,ms  added wccMxrLibs[GS]et. [mc]xrDocCreate mods to log
#		  progress, and call [mc]xrDataDocCreate with new args.
#		  removed useless wccComponent*Info routines.
# 01g,14jul98,ms  added type checking in wccParamGet, ran through dos2unix,
#		  [cm]xrDocDelete now callout to dataDocLib procs.
#		  moved cxrConfigAllDiff cmpTestLib.tcl.
# 01f,07jul98,ms  cxrDocDelete and mxrDocDelete check if document exists
# 01e,29jun98,ms  added wccComponentChangeInfo
# 01d,25jun98,ms  fixed mxrDocCreate problem introduced by new build
# 01d,09jun98,ms  implemented the stubed wccParam* functions.
# 01c,24apr98,ms  added a bunch of component checking routines
# 01b,21apr98,ms  added wccCdfPath[Get&Set]
# 01a,19mar98,ms  written.
#
# DESCRIPTION
# This library manages vxWorks compononent configurations within
# a project. Component configrations consist of:
# * A reference to a component library
# * A selection of INCLUDE_* options from that library
# * A selection of routines within the vxWorks archive that should
#   be included (even if the routine is not part of a component
# * A set of overrides for some of the component configuration
#   parameters (e.g., NUM_FDS is a parameter for component INCLUDE_IO_SYSTEM).
#
# This library supports multiple component configurations within
# a single project. One of which is the default configuration.
#
# Each component configuration has a name (e.g., DEFAULT), and its
# information is stored in the project under tags
#	WCC_<name>_COMPONENTS
#	WCC_<name>_ROUTINES
#	WCC_<name>_PARAMS
# Global information about all wcc's are stored under tags:
#	WCC__LIST
#	WCC__CURRENT
#	WCC__CDF_PATH
#

global g_wccParamNoValue 
set g_wccParamNoValue "<no value>"

source [wtxPath]/host/resource/tcl/app-config/Project/dataDocLib.tcl

###############################################################################
#
# wccListGet - get a list of wcc's in the project
#

proc wccListGet hProj {
    return [prjTagDataGet $hProj WCC__LIST]
}

#########################################################################
#
# wccCdfPathSet - set the CDF search path
#
# PARAMETERS 
#   hProj:      project handle
#   cdfPath:    cdf path. Use $(TOOL_FAMILY) variable where appropriate
#
# RETURNS: N / A 
#
# ERRORS: N / A
#
proc wccCdfPathSet {hProj cdfPath} {
    prjTagDataSet $hProj WCC__CDF_PATH $cdfPath
}

#########################################################################
#
# wccCdfPathGet - get the CDF search path
#
# PARAMETERS 
#   hProj:      project handle
#   toolchain:  tool chain
#
# RETURNS: cdf path, with $(TOOL_FAMILY) variable expanded
#
# ERRORS: N / A
#
proc wccCdfPathGet {hProj} {
    set cdfPath [prjTagDataGet $hProj WCC__CDF_PATH]
    set tc [prjBuildTcGet $hProj [prjBuildCurrentGet $hProj]]
    set family [${tc}::family]
    regsub -all {\$\(TOOL_FAMILY\)} $cdfPath $family cdfPath
    return $cdfPath
}

###############################################################################
#
# wccMxrLibsSet - set the libraries that are used for module xref
# 

proc wccMxrLibsSet {hProj mxrLibs} {
    regsub -all {[\{\}]} $mxrLibs "" mxrLibs
    prjTagDataSet $hProj WCC__MXR_LIBS $mxrLibs
}

###############################################################################
#
# wccMxrLibsGet - get the libraries that are used for module xref
# 

proc wccMxrLibsGet hProj {
    set retval [prjTagDataGet $hProj WCC__MXR_LIBS]
    regsub -all {[\{\}]} $retval "" retval
    return $retval 
}

###############################################################################
#
# wccCurrentGet - get the current build spec
#

proc wccCurrentGet hProj {
    return [prjTagDataGet $hProj WCC__CURRENT]
}

###############################################################################
#
# wccCurrentSet - set the current build spec
#

proc wccCurrentSet {hProj wcc} {
    if {[lsearch [prjTagDataGet $hProj WCC__LIST] $wcc] == -1} {
        error "component configuration $wcc doesn't exist"
    }

    prjTagDataSet $hProj WCC__CURRENT $wcc
}

###############################################################################
#
# wccCreate - create a wcc, optionally copying info from an existing wcc
#

proc wccCreate {hProj wcc {oldWcc ""}} {
    if {[lsearch [prjTagDataGet $hProj WCC__LIST] $wcc] != -1} {
        error "component configuration $wcc already exist"
    }

    prjTagDataSet $hProj WCC__CURRENT $wcc
    prjTagDataSet $hProj WCC__LIST \
                [concat [prjTagDataGet $hProj WCC__LIST] $wcc]

    if {"$oldWcc" == ""} {
	return
    }

    # copy all component data tags from the oldWcc

    foreach oldTag [prjTagListGet $hProj WCC_${oldWcc}_] {
	regsub $oldWcc $oldTag $wcc newTag
	prjTagDataSet $hProj $newTag [prjTagDataGet $hProj $oldWcc $oldTag]
    }
}

###############################################################################
#
# wccDelete - delete a wcc
#

proc wccDelete {hProj wcc} {
    set wccs [prjTagDataGet $hProj WCC__LIST]
    set ix [lsearch $wccs $wcc]
    if {$ix == -1} {
        error "component configuration $wcc doesn't exist"
    }

    set wccs [lreplace $wccs $ix $ix]
    prjTagDataSet $hProj WCC__LIST $wccs
    prjTagRemove $hProj WCC_${wcc}_COMPONENTS
    prjTagRemove $hProj WCC_${wcc}_ROUTINES
    prjTagRemove $hProj WCC_${wcc}_PARAMS

    set currentSpec [prjTagDataGet $hProj WCC__CURRENT]
    if {"$currentSpec" == $wcc} {
        if {[llength $wccs] == 0} {
            prjTagDataSet $hProj WCC__CURRENT ""
        } else {
            prjTagDataSet $hProj WCC__CURRENT [lindex $wccs 0]
        }
    }
}

###############################################################################
#
# wccComponentAdd - add components to a wcc
#

proc wccComponentAdd {hProj wcc componentList} {
    if {[lsearch [prjTagDataGet $hProj WCC__LIST] $wcc] == -1} {
        error "component configuration $wcc doesn't exist"
    }

    set cxrDoc [cxrDocCreate $hProj]
    set cmpSet [$cxrDoc setCreate]
    $cmpSet = "$componentList [prjTagDataGet $hProj WCC_${wcc}_COMPONENTS]"
    prjTagDataSet $hProj WCC_${wcc}_COMPONENTS [$cmpSet instances Component]

    # add new ARCHIVE's to the mxrDoc as needed

    set tc [prjBuildTcGet $hProj [prjBuildCurrentGet $hProj]]
    $cmpSet = [mxrLibsExpand $hProj [$cmpSet get ARCHIVE] $tc]
    set currentLibs [mxrLibsExpand $hProj [wccMxrLibsGet $hProj] $tc]
    set mxrDoc [mxrDocCreate $hProj]
    set extraLibs ""
    foreach lib [$cmpSet - $currentLibs] {
#	prjStatusLog "adding library $lib to configuration"
	lappend extraLibs $lib
    }
    $cmpSet delete

    if {[llength $extraLibs] != 0} {
	wccMxrLibsSet $hProj [mxrLibsContract $hProj \
				[prjRelPath $hProj \
				    [concat $extraLibs $currentLibs]] $tc]
	mxrDocDelete $hProj
	mxrDocCreate $hProj
    }
}

###############################################################################
#
# wccComponentRemove - remove a component from a wcc
#

proc wccComponentRemove {hProj wcc componentList} {
    if {[lsearch [prjTagDataGet $hProj WCC__LIST] $wcc] == -1} {
        error "component configuration $wcc doesn't exist"
    }

    set cxrDoc [cxrDocCreate $hProj]
    set cmpSet [$cxrDoc setCreate]
    set cmpSetOld [$cxrDoc setCreate]
    $cmpSetOld = [wccComponentListGet $hProj $wcc]
    $cmpSet = [$cmpSetOld - $componentList]
    prjTagDataSet $hProj WCC_${wcc}_COMPONENTS [$cmpSet instances Component]

    # remove ARCHIVE's from the mxrDoc as needed

    set tc [prjBuildTcGet $hProj [prjBuildCurrentGet $hProj]]
    $cmpSetOld = [mxrLibsExpand $hProj [$cmpSetOld get ARCHIVE] $tc]
    $cmpSet = [mxrLibsExpand $hProj [$cmpSet get ARCHIVE] $tc]
    $cmpSet = [$cmpSetOld - [$cmpSet contents]]
    if {[$cmpSet contents] != ""} {
#	prjStatusLog "removing libraries [$cmpSet contents] from configuration"
	set currentLibs [mxrLibsExpand $hProj [wccMxrLibsGet $hProj] $tc]
	foreach lib [$cmpSet contents] {
	    set ix [lsearch $currentLibs $lib]
	    set currentLibs [lreplace $currentLibs $ix $ix]
	}
	wccMxrLibsSet $hProj [mxrLibsContract $hProj \
				 [prjRelPath $hProj $currentLibs] $tc]
	mxrDocDelete $hProj 
	mxrDocCreate $hProj 
    }

    $cmpSet delete
    $cmpSetOld delete
}

###############################################################################
#
# wccComponentListGet - get a list of components in a wcc
#

proc wccComponentListGet {hProj wcc} {
    return [prjTagDataGet $hProj WCC_${wcc}_COMPONENTS]
}

###############################################################################
#
# wccProcCompCreate - create a component corresponding to a routine name
#

proc wccProcCompCreate {hProj routine} {
    set cxrDoc [cxrDocCreate $hProj]
    set cxrSet [$cxrDoc setCreate]

    $cxrSet = INCLUDE_PROC_$routine
    if {[$cxrSet instances] != ""} {
	$cxrSet delete
	return
    }
    $cxrSet delete

    set prjDir [file dir [prjInfoGet $hProj fileName]]
    set procFile $prjDir/procs.cdf
    set fd [open $procFile "a+"]
    puts $fd "Component INCLUDE_PROC_$routine {"
    puts $fd "	NAME		$routine routine"
    puts $fd "	LINK_SYMS	$routine"
    puts $fd "}"
    close $fd

    $cxrDoc import $procFile
}

###############################################################################
#
# wccProcAdd - add a routine to a wcc
#

proc wccProcAdd {hProj wcc routineList} {
    foreach routine $routineList {
	wccProcCompCreate $hProj $routine
	wccComponentAdd $hProj $wcc INCLUDE_PROC_$routine
    }
}

###############################################################################
#
# wccProcDependsGet - get a list of dependencies for a given routine
#

proc wccProcDependsGet {hProj wcc routineList} {
    set cxrDoc [cxrDocCreate $hProj]
    set cxrSet [$cxrDoc setCreate]
    set mxrDoc [mxrDocCreate $hProj]
    foreach routine $routineList {
	wccProcCompCreate $hProj $routine
	$cxrSet = [$cxrSet + INCLUDE_PROC_$routine]
    }
    $cxrSet = [cxrSubtree $cxrSet $mxrDoc]
    set depends [$cxrSet - "INCLUDE_PROC_$routine \
	[wccComponentListGet $hProj [wccCurrentGet $hProj]]"]
    $cxrSet delete
    return $depends
}

###############################################################################
#
# wccProcRemove - remove a routine from a wcc
#

proc wccProcRemove {hProj wcc routineList} {
    foreach routine $routineList {
	wccComponentRemove $hProj $wcc INCLUDE_PROC_$routine
    }
}

###############################################################################
#
# wccProcListGet - get a list of routines in a wcc
#

proc wccProcListGet {hProj wcc} {
    set procList ""
    foreach cmp [wccComponentListGet $hProj $wcc] {
	if {[regsub INCLUDE_PROC_ $cmp {} procName] > 0} {
	    lappend procList $procName
	}
    }
    return $procList
}

###############################################################################
#
# wccParamCheck - typecheck a parameter
#
# This routine typechecks a parameter. If the procedure wccParamCheck_<type>
# exists, it is called. Otherwise no type checking is performed.
# New wccParamCheck_<type> procuderes can be added by putting them in
# a .addin.tcl file.
#
# RETURNS: an error string if the parameter is invalid, an empty string if OK
#
# ERRORS: the type checking procedures will throw errors if the
# parameter value is not of the correct type.
#

proc wccParamCheck {type value} {
    global g_wccParamNoValue
    if { $g_wccParamNoValue == [string trim $value] } {
	return ""
    }
    if {"[info procs wccParamCheck_$type]" != "wccParamCheck_$type"} {
	return ""
    }
    return [wccParamCheck_$type $value]
}

###############################################################################
#
# wccParamTypeGet - get the type of a parameter
# 

proc wccParamTypeGet {hProj param} {
    set cxrDoc [cxrDocCreate $hProj]
    set cmpSet [$cxrDoc setCreate]
    $cmpSet = $param
    set type [$cmpSet get TYPE]
    $cmpSet delete
    return $type
}

###############################################################################
#
# wccParamSet - set a parameter in a wcc
#
# Set a parameter.
#
# RETURNS: N/A
# ERRORS: the typechecking procedure, wccParamCheck, will throw an error
# if the parameter is the wrong type.
#

proc wccParamSet {hProj wcc param value} {
    set err [wccParamCheck [wccParamTypeGet $hProj $param] $value]
    if {"$err" != ""} {
	prjWarnLog "Warning setting parameter $param: $err"
    }
    prjTagDataSet $hProj WCC_${wcc}_PARAM_${param} $value
}

###############################################################################
#
# wccParamRemove - remove a parameter setting to a wcc
#

proc wccParamRemove {hProj wcc param} {
    prjTagRemove $hProj WCC_${wcc}_PARAM_${param}
}

proc wccParamHasValue { hProj wcc param } {
	global g_wccParamNoValue
	return [expr { [prjTagDataGet $hProj WCC_${wcc}_PARAM_${param}] \
					   != $g_wccParamNoValue}]
}

proc wccParamClearValue { hProj wcc param } {
	global g_wccParamNoValue
    prjTagDataSet $hProj WCC_${wcc}_PARAM_${param} $g_wccParamNoValue
}

###############################################################################
#
# wccParamGet - get a parameter setting from a wcc
#

proc wccParamGet {hProj wcc param} {
    if {"[prjTagDataGet $hProj WCC_${wcc}_PARAM_${param}]" != ""} {
	return [prjTagDataGet $hProj WCC_${wcc}_PARAM_${param}]
    }

    set cxrDoc [cxrDocCreate $hProj]
    set cxrSet [$cxrDoc setCreate]
    $cxrSet = $param
    set value [$cxrSet get DEFAULT]
    if {"$value" == ""} {
	prjWarnLog "parameter $param has no default value"
    }
    $cxrSet delete
    return $value
}

###############################################################################
#
# wccParamListGet - get a list of parameter macros
# 

proc wccParamListGet {hProj wcc} {
    set cxrDoc [cxrDocCreate $hProj]
    set cxrSet [$cxrDoc setCreate]
    $cxrSet = [wccComponentListGet $hProj $wcc]
    $cxrSet = [$cxrSet get CFG_PARAMS]
    set params [$cxrSet instances Parameter]
    $cxrSet delete
    return $params
}

###############################################################################
#
# cxrDocCreate - create a Cxr document for a project
#
# DELAY - this routine can take a couple of seconds while importing
# the .cdf files.
#

proc cxrDocCreate hProj {
    global cxrDocs

    if {[info exists cxrDocs($hProj)]} {
	return $cxrDocs($hProj)
    }
    set toolchain [prjBuildTcGet $hProj [prjBuildCurrentGet $hProj]]
    set cdfPath   [wccCdfPathGet $hProj]
#    prjStatusLog "reading component descriptor files in $cdfPath..."
    set cxrDocs($hProj) [cxrDataDocCreate $cdfPath $toolchain]
#    prjStatusLog "done"
    return $cxrDocs($hProj)
}

###############################################################################
#
# cxrDocDelete - delete a Cxr document for a project
# 

proc cxrDocDelete hProj {
    global cxrDocs

    if {![info exists cxrDocs($hProj)]} {
	return
    }
    $cxrDocs($hProj) delete
    unset cxrDocs($hProj)
}

###############################################################################
#
# mxrLibsSearch - Derive the list of libraries for creation of mxrDoc from 
#                 linkPath and libs
#
# SYNOPSIS
#  mxrLibsSearch linkPath libs
#
# PARAMETERS
#  linkPath - link path
#  libs     - libraries
# 
# RETURN - a list of libraries
#
# ERROR N/A
#

proc mxrLibsSearch {linkPath libs} {
    set pathList {}
    foreach path $linkPath {
        regsub {\-L} $path "" path
	lappend pathList $path
    }
    set libList {}
    foreach lib $libs {
	if [regexp {\-l} $lib] {
	    regsub {\-l} $lib "" lib
	    set lib lib${lib}.a
	    foreach path $pathList {
		if {[file exist $path/$lib]} {
		    lappend libList $path/$lib
		    break
		}
	    }
	} else {
	    lappend libList $lib
	}
    }
    return $libList
}

###############################################################################
#
# mxrDocCreate - create an Mxr document for a project
#
# DELAY - this routine may take 10 seconds or more the read in
# the module and symbol info from the project archives.
# 

proc mxrDocCreate hProj {
    global mxrDocs

    if {[info exists mxrDocs($hProj)]} {
	return $mxrDocs($hProj)
    }
    set build [prjBuildCurrentGet $hProj]
    set libs [prjBuildMacroGet $hProj $build LIBS]
    set tc     [prjBuildTcGet $hProj $build]
    set libs   [concat [wccMxrLibsGet $hProj] $libs]
    set libs [mxrLibsExpand $hProj $libs $tc]
#    prjStatusLog "reading module info from $libs..."
    set mxrDocs($hProj) [mxrDataDocCreate $tc $libs]
#    prjStatusLog "done"
    return $mxrDocs($hProj)
}

###############################################################################
#
# mxrDocDelete - delete an Mxr document for a project
# 

proc mxrDocDelete hProj {
    global mxrDocs

    if {![info exists mxrDocs($hProj)]} {
	return
    }
    $mxrDocs($hProj) delete
    unset mxrDocs($hProj)
}

###############################################################################
#
# cxrDocValidate - validate a cxr document
#
# This routine can be used by component authors to validate .cdf files:
# * every Component, Selection, and Folder is the child of
#   exactly one other (except for FOLDER_ROOT and FOLDER_HIDDEN).
# * every InitGroup and component with an INIT_ROUTINE is in exactly one
#   InitGroup (except for usrRoot and usrInit), and
#   every item in an InitGroup has an InitRtn
# * all configlettes, modules, header files, and linkSyms exist
# * every component with an initRtn or configlette can build
# 

proc cxrDocValidate {hProj {components ""} {buildTest ""}} {
    set cxrDoc [cxrDocCreate $hProj]
    set mxrDoc [mxrDocCreate $hProj]
    set prjDir [file dirname [prjInfoGet $hProj fileName]]
    set bspDir [prjTagDataGet $hProj BSP_DIR]
    if {"$components" == ""} {
	set components [$cxrDoc instances Component]
    }

    set objSet [$cxrDoc setCreate]
    set moduleSet [$mxrDoc setCreate]
    set srcDir [wtxPath]/target/config/comps/src
    set hDir [wtxPath]/target/h

    puts "checking for unresolved object references..."
    $objSet = [$cxrDoc instances Component]
    $objSet = [$objSet get CFG_PARAMS]
    set unresolved [$cxrDoc instances Unresolved]
    set unresolvedParams [$objSet & $unresolved]
    $objSet = $unresolved
    set unresolved [lrange [lsort [$objSet - $unresolvedParams]] 1 end]
    if {"$unresolved" != ""} {
	puts "ERROR: unresolved objects $unresolved"
    }
    if {"$unresolvedParams" != ""} {
	puts "ERROR: unresolved Parameters [lsort $unresolvedParams]"
    }
    puts "done\n"

    puts "checking that all objects are contained in exactly one folder..."
    $objSet = $components
    $objSet = [$objSet + [$cxrDoc instances Folder]]
    $objSet = [$objSet + [$cxrDoc instances Selection]]
    foreach object [$objSet instances] {
	$objSet = $object
	set containers [$objSet get _CHILDREN]
	if {[llength $containers] == 0} {
	    if {"$object" == "FOLDER_ROOT"} {
	        continue
	    }
	    puts "ERROR: $object belongs to no containers"
	}
	if {[llength $containers] > 1} {
	    puts "ERROR: $object belongs to $containers"
	}
    }
    puts "done\n"
	
    puts "checking that all objects belong to exactly one InitGroup..."
    $objSet = $components
    set objects [$objSet + [$cxrDoc instances InitGroup]]
    foreach object $objects {
	if {("$object" == "usrInit") || ("$object" == "usrRoot")} {
	    continue
	}
	$objSet = $object
	set initRtn [$objSet get INIT_RTN]
	if {"$initRtn" == ""} {
	    if {("[$objSet types]" == "InitGroup")} {
		puts "ERROR: InitGroup $object has no INIT_RTN"
	    }
	    continue
	}
	set containers [$objSet get _INIT_ORDER]
	if {[llength $containers] == 0} {
	    puts "ERROR: $object belongs to no InitGroup"
	}
	if {[llength $containers] > 1} {
	    puts "ERROR: $object belongs to $containers"
	}
    }
    puts "done\n"

    puts "checking that all components have a NAME..."
    foreach component [concat $components [$cxrDoc instances Folder] \
		[$cxrDoc instances Selection]] {
	$objSet = $component
	if {"[$objSet get NAME]" == "$component"} {
	    puts "ERROR: $component has no NAME"
	}
    }
    puts "done\n"

    puts "checking that all component software exists..."
    $objSet = $components
    set bspStubs	[join [$objSet get BSP_STUBS]]
    set configlettes	[join [$objSet get CONFIGLETTES]]
    set headers		[join [$objSet get HDR_FILES]]
    set linkSyms	[join [$objSet get LINK_SYMS] \
				[$objSet get  LINK_DATASYMS]]
    set modules		[$objSet get MODULES]

    set bad ""
    foreach stub $bspStubs {
	if { ![file exists $srcDir/$stub] } {
	    lappend bad $stub
	}
    }
    if {"$bad" != ""} {
	puts "ERROR: missing BSP_STUB's $bad"
	set bad ""
    }

    set bad ""
    foreach configlette $configlettes {
	if {(![file exists $srcDir/$configlette])	&& \
		(![file exists $configlette])} {
	    lappend bad $configlette
	}
    }
    if {"$bad" != ""} {
	puts "ERROR: missing CONFIGLETTE's $bad"
	set bad ""
    }
    foreach header $headers {
	if {(![file exists $hDir/$header]) 		&& \
		(![file exists $bspDir/$header]) 	&& \
		(![file exists $header])} {
	    lappend bad $header
	}
    }
    if {"$bad" != ""} {
	puts "ERROR: missing HDR_FILE's $bad"
	set bad ""
    }
    foreach module $modules {
	$moduleSet = $module
	if {"[$moduleSet instances Module]" != "$module"} {
	    lappend bad $module
	}
    }
    if {"$bad" != ""} {
	$objSet = $bad
	$objSet = [$objSet get _MODULES]
	puts "WARNING: components [lsort [$objSet contents]] not installed"
	puts "WARNING: missing MODULE's [lsort $bad]"
	set bad ""
    }
    foreach linkSym [cSymsMangle $linkSyms [mxrTcGet $mxrDoc]] {
	$moduleSet = $linkSym
	if {"[$moduleSet instances Symbol]" != "$linkSym"} {
	    lappend bad $linkSym
	}
    }
    if {"$bad" != ""} {
	puts "ERROR: missing LINK_SYMS's $bad"
    }
    puts "done\n"

    if {"$buildTest" == ""} {
	$objSet delete
	return
    }
    puts "checking that all components can build..."
    set prjDir [file dirname [prjInfoGet $hProj fileName]]
    set badList ""
    set wcc [wccCurrentGet $hProj]
    foreach component $components {
	$objSet = $component
	set missingParams ""
	foreach param [$objSet get CFG_PARAMS] {
	    if {[wccParamGet $hProj $wcc $param] == ""} {
		lappend missingParams $param
	    }
	}
	if {[llength $missingParams] > 0} {
	    puts "WARNING: skipping $component because it needs the following"
	    puts "parameters, but they have no default values: $missingParams\n"
	    continue
	}
	::prj_vxWorks_hidden::configGen $hProj $component
	set prjType [prjTypeGet $hProj]
	set info [::${prjType}::buildCmdGet $hProj prjConfig.o]
	set cmd [lindex $info 0]
	set buildDir [lindex $info 1]

	set dir [pwd]
	cd $buildDir
	if {[catch "eval exec $cmd" errMsg]} {
	    puts $errMsg
	    file delete $prjDir/$component.c
	    file rename $prjDir/prjConfig.c $prjDir/$component.c
	    lappend badList $component
	}

	cd $dir
	continue
    }
    if {[llength $badList] > 0} {
	puts "Summary of failures: $badList"
    }
    puts "done\n"

    $objSet delete
}

###############################################################################
#
# wccDependsGet - compute the dependancies of a component configuration
#

proc wccDependsGet {hProj wcc} {
    set cxrDoc [cxrDocCreate $hProj]
    set mxrDoc [mxrDocCreate $hProj]
    set cmpSet [$cxrDoc setCreate]
    set cmpSubtree [$cxrDoc setCreate]
    $cmpSet = [wccComponentListGet $hProj $wcc]

    $cmpSubtree = [cxrSubtree $cmpSet $mxrDoc]
    set dependancies [$cmpSubtree - [$cmpSet instances]]

    $cmpSet delete
    $cmpSubtree delete

#    prjStatusLog "wccDependsGet: $dependancies"

    return $dependancies
}

###############################################################################
#
# wccValidate - validate a component configuration
#
# RETURNS: 1
# EXCEPTIONS: configuration error
# 

proc wccValidate {hProj wcc} {
    set d [cxrDocCreate $hProj]
    set s [$d setCreate]
    set wcc [wccCurrentGet $hProj]
    $s = [wccComponentListGet $hProj $wcc]

    # find components that are misconfigured

    set info [cxrSetValidate $s]

    # find all Components that have undefined Parameters

    foreach component [wccComponentListGet $hProj $wcc] {
	set params ""
	$s = $component
	$s = [$s get CFG_PARAMS]
	foreach param [$s instances Parameter] {
	    if {[wccParamGet $hProj $wcc $param] == ""} {
		lappend params $param
	    }
	}
	if {"$params" != ""} {
	    lappend info "$component {undefined parameters $params}"
	}
    }

    # find all missing dependencies

    $s = [wccComponentListGet $hProj $wcc]
    $s = [cxrSubtree $s [mxrDocCreate $hProj] minimal]
    $s = [$s - [wccComponentListGet $hProj $wcc]]
    foreach cmp [$s instances Component] {
        lappend info "$cmp {needs to be included}"
    }

    # Cache the error status for anyone interested
    if {$info == ""} {
	set status 0
    } else {
	set status 1
    }
    wccErrorStatusSet $hProj $status

    $s delete
    return $info
}

###############################################################################
#
# wccErrorStatusSet - cache whether or not this project
# has configuration errors 
#

proc wccErrorStatusSet {hProj status} {
    set oldStatus [::prjTagDataGet $hProj COMPONENT_ERROR_STATUS]
    if {$oldStatus != $status} {
	::prjTagDataSet $hProj COMPONENT_ERROR_STATUS $status
    }
}

###############################################################################
#
# wccErrorStatusGet - 
#

proc wccErrorStatusGet {hProj} {
    set status [::prjTagDataGet $hProj COMPONENT_ERROR_STATUS]
    if {$status == ""} {
	set status -1
    }
    return $status
}

###############################################################################
#
# wccParamCheck_uint - typecheck a uint parameter
# 

proc wccParamCheck_uint param {
    if {[catch "expr $param >= 0" val]} {
	# A hack: if we get a string value like (A + B),
	# expr doesn't handle it, so we look for the error string
	# from the catch statement instead
	if {![regexp {^syntax error.*} $val dummy]} {
	    set val "bad value $param - must be a non-negative integer"
	} else {
	    set val "" 
	}
    } elseif {$val == 0} {
	return "bad value $param - must be a non-negative integer"
    } else {
	set val "" 
    }
}

###############################################################################
#
# wccParamCheck_exists - typecheck an "exists" type parameter
# 

proc wccParamCheck_exists param {
    if {("$param" != "TRUE") && ("$param" != "FALSE")} {
	return "bad value $param - must be either TRUE or FALSE"
    }
}

###############################################################################
#
# wccParamCheck_bool - typecheck an "exists" type parameter
# 

proc wccParamCheck_bool param {
    if {("$param" != "TRUE") && ("$param" != "FALSE") && \
        ("$param" != "1") && ("$param" != "0")} {
	return "bad value $param - must be either TRUE or FALSE"
    }
}

###############################################################################
#
# wccComponentLoad - dynamically load components
#

proc wccComponentLoad {hProj compList {appModules ""}} {
    wtxMemRead [lindex [wtxSymFind -name vxTicks] 1] 4
    set dynLoadIx [llength [wtxObjModuleList]]
    set dynLoadFile dynLoad${dynLoadIx}
    set libs ""
    foreach lib [wccMxrLibsGet $hProj] {
	lappend libs [wtxPath]/target/lib/$lib
    }
    set wcc [wccCurrentGet $hProj]
    set build [prjBuildCurrentGet $hProj]
    set tc [prjBuildTcGet $hProj $build]
    set ar [prjBuildMacroGet $hProj $build AR]
    set tool_cc [${tc}::toolFindByFile foo.c]
    set tool_ld [${tc}::toolFindByType linker]
    set cflags [prjBuildFlagsGet $hProj $build ${tool_cc}]
    set cc [set ${tool_cc}::tool]
    set ld [set ${tool_ld}::tool]

    # compute the loaded modules

    prjStatusLog "computing already loaded modules..."
    set symList "" 
    foreach symInfo [wtxSymListGet -name .*] {
	lappend symList [lindex $symInfo 0]
    }
    set mxrDoc [mxrDocCreate $hProj]
    set mxrSet [$mxrDoc setCreate]
    $mxrSet = $symList
    set loadedModules [$mxrSet get exportedBy]

    # compute the loaded components

    prjStatusLog "computing already loaded components..."
    set cxrDoc [cxrDocCreate $hProj]
    set cxrSet [$cxrDoc setCreate]
    $cxrSet = $loadedModules
    $cxrSet = "[wccComponentListGet $hProj $wcc] [$cxrSet get _MODULES]"
    set loadedComponents [cxrSubtree $cxrSet $mxrDoc]

    # compute required components for compList

    prjStatusLog "computing components needed..."
    $cxrSet = $compList
    $cxrSet = [cxrSubtree $cxrSet $mxrDoc]
    $cxrSet = [$cxrSet - $loadedComponents]
    set symTblInit \
	[expr [lsearch [$cxrSet get REQUIRES] SELECT_SYMTBL_INIT] >= 0]
    set requiredComponents [$cxrSet instances]
    prjStatusLog "requiredComponents = [lsort $requiredComponents]"

    # compute required modules for compList

    prjStatusLog "computing modules needed..."
    $mxrSet = "[$cxrSet get LINK_SYMS] [$cxrSet get LINK_DATASYMS]"
    $mxrSet = "[$mxrSet get defines] [$cxrSet get MODULES]"
    $mxrSet = [mxrSubtree $mxrSet]
    set requiredModules [$mxrSet - $loadedModules]
    prjStatusLog "requiredModules = [lsort $requiredModules]"

    # generate the configuration code needed for the components

    if {"$requiredComponents" == ""} {
	set requiredComponents BOGUS_COMPONENT
    }
    ::prj_vxWorks_hidden::configGen $hProj $requiredComponents

    # extract required modules from the archive and link them together

    prjStatusLog "creating $dynLoadFile..."
    set prjDir [file dirname [prjInfoGet $hProj fileName]]
    set origDir [pwd]
    cd $prjDir
    if {"$requiredModules" != ""} {
	eval exec $ar -x $libs $requiredModules
    }

    set ldPartialFlags [prjBuildMacroGet $hProj $build LD_PARTIAL_FLAGS]
    if {"$ldPartialFlags" == ""} {
	set ldPartialFlags "-r"
    }
    eval exec $cc $cflags -c prjConfig.c
    eval exec $ld $ldPartialFlags prjConfig.o $requiredModules \
		$appModules -o $dynLoadFile
    cd $origDir

    # add missing common block symbols as needed

    if {"[${tc}::::commSymLink]" == "symbol"} {
	prjStatusLog "adding missing common block symbols..."

	$mxrSet = [$mxrDoc instances Module]
	set commSyms [$mxrSet get declares]

	set d [mxrDataDocCreate $tc $prjDir/$dynLoadFile]
	set s [$d setCreate]
	$s = [$d instances Module]
	$mxrSet = [$s get imports]
	$s delete
	$d delete

	$mxrSet = [$mxrSet contents]		;# undefed syms in dynLoadFile
	$mxrSet = [$mxrSet - $symList]		;# remove already loaded syms
	$mxrSet = [$mxrSet & $commSyms]		;# only add common block syms

	foreach sym [$mxrSet instances] {
	    puts -nonewline "$sym "
	    $mxrSet = $sym
	    set size [$mxrSet get size]
	    set addr [wtxMemAlloc $size]
	    wtxMemSet $addr $size 0
	    wtxSymAdd $sym $addr 0x11
	}
	puts ""
    }

    prjStatusLog "loading $dynLoadFile..."
    set usrRootAddr [lindex [wtxSymFind -name usrRoot] 1]
    set ldFlags "LOAD_COMMON_MATCH_ALL|LOAD_GLOBAL_SYMBOLS"
    set badSyms [lrange [wtxObjModuleLoad $ldFlags $prjDir/$dynLoadFile] 4 end]
    if {[llength $badSyms] > 0} {
        puts "warning: module contains undefined symbols:"
        puts $badSyms
    } else {
	set newUsrRootAddr [lindex [wtxSymFind -name usrRoot] 1]
	if {"$newUsrRootAddr" != "$usrRootAddr"} {
	    if {[llength $badSyms] > 0} {
		puts "skipping initialization call to usrRoot"
	    } else {
		prjStatusLog "calling usrRoot..."
		wtxFuncCall $newUsrRootAddr
		prjStatusLog "done"
	    }
	}
    }

    # initialize the target symbol table as needed

    set sysSymTblAddr [lindex [wtxSymFind -name sysSymTbl] 1]
    set memBlk [wtxMemRead $sysSymTblAddr 4]
    set sysSymTbl [memBlockGet -l $memBlk]
    memBlockDelete $memBlk
    if {($symTblInit) && ($sysSymTbl == 0)} {
	prjStatusLog "initializing the target symbol table..."
	set symTblCreateRtn [lindex [wtxSymFind -name symTblCreate] 1]
	set memSysPartIdAddr [lindex [wtxSymFind -name memSysPartId] 1]
	set memBlk [wtxMemRead $memSysPartIdAddr 4]
	set memSysPartId [memBlockGet -l $memBlk]
	memBlockDelete $memBlk
	set hashSizeLog2 8
	set sysSymTbl [wtxDirectCall $symTblCreateRtn $hashSizeLog2 \
		1 $memSysPartId]

	set memBlk [memBlockCreate]
	memBlockSet -l $memBlk 0 $sysSymTbl
	wtxMemWrite $memBlk $sysSymTblAddr
	memBlockDelete $memBlk

	set symAddRtn [lindex [wtxSymFind -name symAdd] 1]
	set symNameAddr [wtxMemAlloc 100]

	set symInfoList [wtxSymListGet -name .*]
	prjStatusLog "adding [llength $symInfoList] symbols..."
	set ix 0
	foreach symInfo $symInfoList {
	    incr ix
	    if {($ix % 100) == 0} {
		prjStatusLog " $ix"
	    }
	    set symNameMemBlock [memBlockCreate -string [lindex $symInfo 0]]
	    wtxMemWrite $symNameMemBlock $symNameAddr
	    memBlockDelete $symNameMemBlock
	    set symVal  [lindex $symInfo 1]
	    set symType [lindex $symInfo 2]
	    set symGrp  [lindex $symInfo 3]
	    wtxDirectCall $symAddRtn $sysSymTbl $symNameAddr $symVal \
		$symType $symGrp
	}
    }

    # done 

    prjStatusLog "done"
    foreach file [glob -nocomplain $prjDir/*.o $prjDir/prj*.*] {
	file delete -force $file
    }
    $mxrSet delete
    $cxrSet delete
}


