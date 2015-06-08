# prjBuildLib.tcl - project build manager library
#
# Copyright 1998-2002 Wind River Systems, Inc.
#
# modification history
# --------------------
# 02b,29may02,rbl  fix problem where prjBuildCopy was not dealing properly
#                  with _'s in build names
# 02a,13may02,rbl  fix problem where vxWorks_rom.bin rule was appearing twice
#                  in project Makefile if this rule was defined in the bsp
#                  Makefile
# 01z,23mar02,cjs  Report build errors during autoscale (spr 73344)
# 01y,09jan02,rbl  support .hex and .bin targets from gui
# 01x,07dec01,rbl  fix error redirection - merge from T3
# 01w,24nov01,dat  individual build failure does not cause tcl error
# 01v,13nov01,rbl  add .pl partial link target to downloadable projects
# 01u,27sep01,rbl  in toolchains, rename "vendor" to "tool" for consistency
#                  with the makefile TOOL variable
# 01t,13jun01,agf  add logic to check host type on stdout/stderr syntax
# 01s,09may01,agf  undoing last change, @ breaks Win hosts
# 01r,16apr01,agf  fix syntax for redirecting output to stdout/stderr
# 01q,16apr99,j_k  back-off from previous fix and make it right.
# 01p,13apr99,j_k  part of fix for SPR #26620.
# 01o,10mar99,ren  Fixing SPR 25424 by naming the link output file to be the
#                  project name.
# 01n,11feb99,cjs  Changed buildRuleListGet, buildRuleDefault to operate on a
#                  project handle
# 01m,19jan99,cjs  make prjBuildRuleListGet() sensitive to bsp
# 01l,10nov98,ms   fixed bug where all buildSpec's used the same build rule.
# 01k,26oct98,ms   new build interface
# 01j,06aug98,ms   prjBuild now returns to build output
# 01j,29jul98,ms   updated calls to prj[Rel|Full]Path. Change default roPath.
# 01i,29jun98,jmp  added prjBuildRuleDescGet{}.
#		   added prjDepPath[Add|Remove|Get|Default].
# 01h,24jun98,ms   fixed prjBuildCopy().
# 01g,18jun98,jmp  added support for user defined rules.
# 01f,08jun98,cjs  fixed minor bug in prjBuildDelete
# 01e,05jun98,jmp  added prjBuildMacroCheck{}, prjBuildMacroDescGet{}. added
#                  control in prjBuildMacroSet{}.
# 01d,05jun98,jmp  called prjTagRemove instead of prjTagDataRemove in
#                  prjBuildMacroRemove.
# 01c,02jun98,ms   added prjBuildCopy, prjBuildRule* routines.
#		   removed prjBuildInfo*.
# 01b,19may98,ms   added prjBuildMacro* and prjBuildInfo* routines.
# 01a,18mar98,ms   written.
#
# DESCRIPTION
# This is the project build management library.
# This library provides an API for creating and managing "build specs."
# A build spec contains:
#	a toolchain selection
#	a set of flags for each tool in the toolchain
#	a build rule (e.g., vxWorks_rom)
#	a set of values for configurable aspects of the build (e.g.,
#		RAM_HIGH_ADRS).
#
# This library also provides an API for adding build rules to
# a project. The available build rules are those provided by the
# project manager, plus any custom rules added by the user.
#
# It also provides an API for toolChains to register themselves as follows;
# When this library starts up, it sources all files of the form tc_*.tcl.
# These files create a toolChain namespace and register it by calling
# prjTcAdd.
# Each toolChain (e.g., MC68040gnu) has a type. Currently the only
# type used is "C++", but new types will be added in the future.
# Each toolChain is expected to provide the following functions is
# a namespace:
#        type                           // type of toolchain. e.g., "C++"
#        cpu                       	// e.g. MC68040
#        tool                         // e.g., gnu
#        toolListGet                    // return a list of tool namespaces
#        toolFindbyFile fileName	// returns a tool namespace
#        toolFindbyType toolType	// returns a tool namespace
#        dependFiles    tool flags file // return a list of files that are
#                                   	// make-dependancies for the file
#	 macroListGet			// list of make-macros associated with
#					// the toolchain
#	 macroDefaultValGet macro	// default value for a macro
#
# Each tool is contained in a namespace an supports:
#        type                       // e.g., c compiler
#        inputSuffix                // e.g., .c
#        outputFiles     inFiles    // e.g., regsub .c $files .o
#        tool                       // e.g., cc68k
#        buildCmds       flags file // return a list of commands to build
#                                   // file with flags
#	 flagsAdd	 oldFlags type arg
#
# RESOURCE FILES
# [wtxPath]/host/resource/tcl/app-config/Project/tc_*.tcl
# [wtxPath]/host/resource/tcl/app-config/Project/macrosInfo.tcl
#

set prjToolChains ""

###############################################################################
#
# prjTcAdd - add a toolchain
#

proc prjTcAdd tc {
    global prjToolChains

    if {[catch "namespace eval $tc {info procs tool}" val]} {
        error "invalid toolChain namespace $tc"
    }
    if {$val != "tool"} {
        error "invalid toolChain namespace $tc"
    }

    lappend prjToolChains $tc
}

###############################################################################
#
# prjTcListGet - get a list of toolchains
#

proc prjTcListGet {cpu tool type} {
    global prjToolChains

    set tcList ""
    foreach tc $prjToolChains {
	if {("$cpu" != "") && ([::${tc}::cpu] != $cpu)} {
	    continue
	}
	if {("$tool" != "") && ([::${tc}::tool] != $tool)} {
	    continue
	}
	if {("$type" != "") && ([::${tc}::type] != $type)} {
	    continue
	}
	lappend tcList $tc
    }
    return $tcList
}

###############################################################################
#
# prjBuildListGet - get a list of project build specs
#

proc prjBuildListGet hProj {
    return [prjTagDataGet $hProj BUILD__LIST]
}

###############################################################################
#
# prjBuildCurrentGet - get the current build spec
#

proc prjBuildCurrentGet hProj {
    return [prjTagDataGet $hProj BUILD__CURRENT]
}

###############################################################################
#
# prjBuildCurrentSet - set the current build spec
#

proc prjBuildCurrentSet {hProj buildSpec} {
    if {[lsearch [prjTagDataGet $hProj BUILD__LIST] $buildSpec] == -1} {
	error "build spec $buildSpec doesn't exist"
    }

    prjTagDataSet $hProj BUILD__CURRENT $buildSpec
}

###############################################################################
#
# prjBuildCreate - create a build spec
#
# This routine creates a build spec using the toolChains default flags
# and then runs the project-type managers "buildModify" method to
# customize the buildSpec for the project.
# By convention, build specs should have the toolChain name at the
# begining. E.g., MC68040_debug.
#

proc prjBuildCreate {hProj buildSpec tc} {
    if {[lsearch [prjTagDataGet $hProj BUILD__LIST] $buildSpec] != -1} {
	error "build spec $buildSpec already exists"
    }

    # add the new buildSpec to the list of buildSpec's, and make it current

    prjTagDataSet $hProj BUILD__LIST \
		[concat [prjTagDataGet $hProj BUILD__LIST] $buildSpec]
    prjTagDataSet $hProj BUILD__CURRENT $buildSpec

    # associate the toolchain and default build flags with the new buildSpec.

    prjTagDataSet $hProj BUILD_${buildSpec}_TC $tc
    foreach macro [${tc}::macroListGet] {
	set macroVal [${tc}::macroDefaultValGet $macro]
	prjBuildMacroSet $hProj $buildSpec $macro $macroVal
    }

    # allow the project-type manager to modify the build as needed

    set prjType [prjTypeGet $hProj]
    ::${prjType}::buildModify $hProj $buildSpec

    # done

    return $buildSpec
}

###############################################################################
#
# prjBuildSpecificTagsGet - get list of BUILD_${build}_ tags in regsub-ready form
#
# Note: If you add another build-specific tag, you must add it to this list so
# that prjBuildCopy will correctly copy it to a new build
#
# RETURNS: list of build-specific tags in regsub "any of these" form, i.e. 
# \(tag1|tag2|...\). Currently the list is
# 
# \(TC|MACRO|BUILDRULE|RO_DEPEND_PATH\)
#

proc prjBuildSpecificTagsGet {} {
    return "\(TC|MACRO|BUILDRULE|RO_DEPEND_PATH\)"
}

###############################################################################
#
# prjBuildCopy - create a build spec by copying another
#

proc prjBuildCopy {hProj buildSpec newSpec} {
    if {[lsearch [prjTagDataGet $hProj BUILD__LIST] $buildSpec] == -1} {
        error "build spec $buildSpec doesn't exist"
    }
    if {[lsearch [prjTagDataGet $hProj BUILD__LIST] $newSpec] != -1} {
        error "build spec $newSpec already exist"
    }

    foreach tag [prjTagListGet $hProj BUILD_${buildSpec}_] {
	regsub BUILD_${buildSpec}_[prjBuildSpecificTagsGet] \
            $tag BUILD_${newSpec}_\\1 newTag
	prjTagDataSet $hProj $newTag [prjTagDataGet $hProj $tag]
    }

    prjTagDataSet $hProj BUILD__LIST \
    		[concat [prjTagDataGet $hProj BUILD__LIST] $newSpec]
}

###############################################################################
#
# prjBuildDelete - delete a build spec
#

proc prjBuildDelete {hProj buildSpec} {
    set buildSpecs [prjTagDataGet $hProj BUILD__LIST]
    set ix [lsearch $buildSpecs $buildSpec]
    if {$ix == -1} {
	error "build spec $buildSpec doesn't exist"
    }

    set buildSpecs [lreplace $buildSpecs $ix $ix]
    prjTagDataSet $hProj BUILD__LIST $buildSpecs
    foreach tag [prjTagListGet $hProj BUILD_${buildSpec}_] {
	prjTagRemove $hProj $tag
    }

    set currentSpec [prjTagDataGet $hProj BUILD__CURRENT]
    if {"$currentSpec" == $buildSpec} {
	if {[llength $buildSpecs] == 0} {
	    prjTagDataSet $hProj BUILD__CURRENT ""
	} else {
	    prjTagDataSet $hProj BUILD__CURRENT [lindex $buildSpecs 0]
	}
    }
}

###############################################################################
#
# prjBuildMacroListGet - get a list of build macros
# 

proc prjBuildMacroListGet {hProj hSpec {rule ""} {all 0}} {
    global macroHide
    global macroDesc
    global macroRule
    global macroCpu
    global macroComps
    global macrosOfProj

    set tc [prjBuildTcGet $hProj $hSpec]
    set cpu [${tc}::cpu]
    if {"$rule" == ""} {
	set rule [prjBuildRuleCurrentGet $hProj $hSpec]
    }
    set cmpList [wccComponentListGet $hProj [wccCurrentGet $hProj]]
    set prjType [prjTypeGet $hProj]

    set tags [prjTagListGet $hProj BUILD_${hSpec}_MACRO_]
    regsub -all BUILD_${hSpec}_MACRO_ $tags "" tags
    set macros ""
    foreach macro [concat $tags $macrosOfProj($prjType)] {
	if {[info exists macroRule($macro)]} {
	    if {[regexp $macroRule($macro) $rule] == 0} continue
	}
	if {[info exists macroCpu($macro)]} {
	    if {[regexp $macroCpu($macro) $cpu] == 0} continue
	}
	if {[info exists macroComps($macro)]} {
	    if {[lsearch -regexp $cmpList $macroComps($macro)] == -1} continue
	}
	if {($all == 0) && ([lsearch $macroHide $macro] != -1)} {
	    continue
	}
	if {[lsearch $macros $macro] >= 0} {
	    continue
	}
	lappend macros $macro
    }

    return $macros
}

###############################################################################
#
# prjBuildMacroGet - get the value of a build macro
# 

proc prjBuildMacroGet {hProj hSpec macro} {
    global macroDefault
    if {[info exists macroDefault($macro)] && \
			[lsearch [prjTagListGet $hProj] BUILD_${hSpec}_MACRO_${macro}] \
			== -1} {
	return $macroDefault($macro)
    }
    return [prjTagDataGet $hProj BUILD_${hSpec}_MACRO_${macro}]
}

###############################################################################
#
# prjBuildMacroSet - set the value of a build macro
# 

proc prjBuildMacroSet {hProj hSpec macro value} {

    prjTagDataSet $hProj BUILD_${hSpec}_MACRO_${macro} $value
}

###############################################################################
#
# prjBuildMacroRemove - remove a build macro
# 

proc prjBuildMacroRemove {hProj hSpec macro} {
    prjTagRemove $hProj BUILD_${hSpec}_MACRO_${macro}
}

###############################################################################
#
# prjBuildMacroDescGet - get description of a build macro
# 
# RETURN:  "macroComment"
#

proc prjBuildMacroDescGet {hProj hSpec macro} {
    variable macroDesc

    set desc ""
    if [info exists macroDesc($macro)] {
    	set desc $macroDesc($macro)
    }

    return $desc
}

###############################################################################
#
# prjBuildRuleDescGet - get description of a default rule
# 
# RETURN:  "ruleComment"
# Note: this is a temporary solution.  Rules should come from the bsp.

proc prjBuildRuleDescGet {hProj hSpec rule} {
	set ruleDesc(clean) \
		"Clean project."
	set ruleDesc(vxWorks) \
		"RAM based VxWorks image, linked to RAM_LOW_ADRS.\
		It is loaded into RAM via some external program\
		such as a bootROM. This is the default development image."
	set ruleDesc(vxWorks_romResident) \
		"ROM resident image. The program text remains in ROM, only\
		the data is copied to RAM. This image has the\
		fastest boot time and uses the least amount of RAM, but\
		runs slower on boards with slow ROM access."
	set ruleDesc(vxWorks_rom) \
		"RAM based image that starts in ROM. The ROM startup code\
		copies the entire image to RAM and then jumps to it. This\
		image generally has a slower startup time, but faster\
		execution time, than vxWorks_romResident"
	set ruleDesc(vxWorks_romCompress) \
		"Compressed RAM based image that starts in ROM. This\
		image can fit almost twice the code as other ROM images.\
		But it has the slowest boot time, since the image must be\
		uncompressed. The run-time speed is the same as for vxWorks_rom"
	set ruleDesc(objects) \
		"Build object files from project source."
	set ruleDesc(archive) \
		"Build project objects, then put them in an archive"
	set ruleDesc([constructProjectOutFileName $hProj]) [concat \
		"Build project objects, then link them together for download.\n" \
		"This rule also performs C++ munching needed to support\n"	\
		"static constructors"]
	set ruleDesc([constructProjectPlFileName $hProj]) \
		"Build project objects, then link them together. \
		This rule does not perform C++ munching needed to support \
		static constructors. It is intended to support sub-projects. \
                It is not intended for downloading. \
                The correct object to download is \
                [constructProjectOutFileName $hProj]."

    set desc ""
    set ext [file extension $rule]
    if {$ext == ".hex" || $ext == ".bin" || $ext == ".old"} {
        set rule [file rootname $rule]
    }

    if [info exists ruleDesc($rule)] {
    	set desc $ruleDesc($rule)
    }

    return $desc
}

###############################################################################
#
# prjBuildRuleSet - set or add a custom build rule
#

proc prjBuildRuleSet {hProj hSpec rule cmds} {

    prjTagDataSet $hProj BUILD_RULE_${rule} $cmds
}

###############################################################################
#
# prjBuildRuleGet - get the value of a custom build rule
#

proc prjBuildRuleGet {hProj hSpec rule} {

    return [prjTagDataGet $hProj BUILD_RULE_${rule}]
}

###############################################################################
#
# prjBuildRuleDelete - delete a custom build rule
# 
# RETURN: prjTagRemove return code
#

proc prjBuildRuleDelete {hProj hSpec rule} {
    
    return [prjTagRemove $hProj BUILD_RULE_${rule}]
}

###############################################################################
#
# prjBuildRuleListGet - get a list of build rules
# 

proc prjBuildRuleListGet {hProj hSpec} {
    # get project-type rules
    set prjRules [[prjTypeGet $hProj]::buildRuleListGet $hProj]

    # get custom build rules
    set customRules [prjTagListGet $hProj  BUILD_RULE_]
    regsub -all BUILD_RULE_ $customRules "" customRules
    set ruleList "$prjRules $customRules"
    removeDuplicatesFromList ruleList
    return $ruleList
}

###############################################################################
#
# prjBuildRuleCurrentSet - set the current build rule
# 

proc prjBuildRuleCurrentSet {hProj hSpec rule} {
    prjTagDataSet $hProj BUILD_${hSpec}_BUILDRULE $rule
}

###############################################################################
#
# prjBuildRuleCurrentGet - get the current build rule
# 

proc prjBuildRuleCurrentGet {hProj hSpec} {
    set rule [prjTagDataGet $hProj BUILD_${hSpec}_BUILDRULE]
    if {"$rule" == ""} {
	set rule [[prjTypeGet $hProj]::buildRuleDefault $hProj]
    }
    return $rule
}

###############################################################################
#
# prjBuildTcGet - get the toolChain associated with a build spec
#

proc prjBuildTcGet {hProj buildSpec} {
    if {[lsearch [prjTagDataGet $hProj BUILD__LIST] $buildSpec] == -1} {
	error "build spec $buildSpec doesn't exists"
    }

    return [prjTagDataGet $hProj BUILD_${buildSpec}_TC]
}

###############################################################################
#
# prjBuildFlagsSet - set a tool's flags in a build spec
#

proc prjBuildFlagsSet {hProj buildSpec tool flags} {
    if {[lsearch [prjTagDataGet $hProj BUILD__LIST] $buildSpec] == -1} {
	error "build spec $buildSpec doesn't exists"
    }

    set macro [${tool}::flagsMacro]
    prjBuildMacroSet $hProj $buildSpec $macro $flags
}

###############################################################################
#
# prjBuildFlagsGet - get the tool's flags from a build spec
#
# XXX - currenlty ignores the file
#

proc prjBuildFlagsGet {hProj buildSpec tool {file ""}} {
    if {[lsearch [prjTagDataGet $hProj BUILD__LIST] $buildSpec] == -1} {
	error "build spec $buildSpec doesn't exists"
    }

    set macro [${tool}::flagsMacro]
    return [prjBuildMacroGet $hProj $buildSpec $macro]
}

###############################################################################
#
# prjBuildCmdsGet - get the build commands for building a file
#

proc prjBuildCmdsGet {hProj build file} {
    set tc    [prjBuildTcGet $hProj $build]
    set tool  [::${tc}::toolFindByFile $file]
    set flags [prjBuildFlagsGet $hProj $build $tool $file]

    return [::${tool}::buildCmds $flags $file]
}

###############################################################################
#
# prjBuild - build the project with the current settings
#
# This routine builds the project with the current settings.
# It also creates a new configuration out of the tuple
# (buildSpecCurrent, imageTypeCurrent, imageConfigCurrent).
# The GUI will show this configuration in a drop-down menu of
# recently used configurations in this project (say up to 10
# of them are saved per workspace?).
#
# XXX - the last point is a TODO item
#

proc prjBuild {hProj {rule ""} {spec ""} {throwError ""}} {
    set prjType [prjTypeGet $hProj]
    set info [::${prjType}::buildCmdGet $hProj $rule $spec]
    set cmd [lindex $info 0]
    set prjDir [lindex $info 1]

    # invoke make to do the build

    set dir [pwd]
    cd $prjDir

    #
    # If any of the exec'd commands writes to standard error 
    # (e.g. a compiler warning) and standard error isn't redirected, 
    # then exec will return an error. To avoid this we use 
    # "2>@ stderr", which redirects standard error to 
    # standard error. This has no effect other than to prevent 
    # exec from returning an error on e.g. a compiler warning
    #
    # Update (cjs 23mar2002): not exactly true :-)
    # on real error, error message is no longer returned
    # in its entirety with this redirection in place
    #
    
    set cmdString "eval exec $cmd >@ stdout 2>@ stderr"

    # if throwError == "", catch make errors and pass them
    # up as the value of this routine.
    # otherwise, try the command again without redirecting stdout
    # and stderror, so we can throw complete error to caller
    if {[catch $cmdString errMsg]} {
        if {$throwError != ""} {

            # try again w/o redirection so that when we throw an
            # error, it winds up in our error message
            catch {eval exec $cmd} errorMsg
            cd $dir
            error $errorMsg
        }
    } else {
        cd $dir
        return $errMsg
    }
}

###############################################################################
#
# prjDepPathAdd - add a directory to the read-only dependancy path
#

proc prjDepPathAdd {hProj buildSpec newDirList} {

    set roDepPath [prjDepPathGet $hProj $buildSpec]

    foreach dir $newDirList {
		lappend roDepPath [prjRelPath $hProj $dir]
    }
    prjTagDataSet $hProj BUILD_${buildSpec}_RO_DEPEND_PATH $roDepPath
}

###############################################################################
#
# prjDepPathRemove - remove a directory from the read-only dependancy path
#
proc prjDepPathRemove {hProj buildSpec dir} {

    set roDepPath [prjDepPathGet $hProj $buildSpec]
    set newList ""

    foreach depDir $roDepPath {
	if {"$depDir" != "$dir"} {
	    lappend newList $depDir
	}
    }
    prjTagDataSet $hProj BUILD_${buildSpec}_RO_DEPEND_PATH $newList
}

###############################################################################
#
# prjDepPathGet - get the read-only dependancy path
#

proc prjDepPathGet {hProj buildSpec} {

    set path [prjTagDataGet $hProj BUILD_${buildSpec}_RO_DEPEND_PATH]
    set fullPath ""
    
    foreach dir $path {
	lappend fullPath [prjFullPath $hProj $dir]
    }

    return $fullPath
}

###############################################################################
#
# prjDepPathSet - set the read-only dependancy path
#

proc prjDepPathSet {hProj buildSpec listPaths} {
	set listRelativePaths [list ]
	foreach dir $listPaths {
		lappend listRelativePaths [prjRelPath $hProj $dir]
	}
	prjTagDataSet $hProj BUILD_${buildSpec}_RO_DEPEND_PATH $listRelativePaths
}


###############################################################################
#
# prjDepPathDefaults - get the default read-only dependancy paths
#

proc prjDepPathDefaults {} {
    return [list [wtxPath target h] [wtxPath target src] [wtxPath target config]]
}

# source resources files
set prjTclDir [wtxPath]/host/resource/tcl/app-config/Project
foreach file [glob $prjTclDir/tc_*.tcl] {
    source $file
}

# source macros information file
source $prjTclDir/macrosInfo.tcl
