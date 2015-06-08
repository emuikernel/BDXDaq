# ffsw.tcl - TCL library for validating Tornado 2.2 component gallery 
#
# Copyright 1998-2001 Wind River Systems, Inc. Alameda, CA
#
# modification history
# --------------------
# 01g,15apr02,j_s  fix checking for components missing modules; expand
#		   optional component checking and cpu dependent component
#		   checking to configllettes
# 01f,01apr02,rbl  check for configlettes in bsp directory and target/src/drv
# 01e,20mar02,j_s  add cpu-dependent components description for SH7750
# 01d,08mar02,j_s  add cpu-dependent component awareness and optional product
#                  awareness
# 01c,28feb02,j_s  make it runnable from command line
# 01b,08feb02,j_s  add automatic reporting of cause of unavailability of each
#		   unavailable component
# 01a,06feb02,cjs  created
#
# Usage: wtxtcl
# <set WIND_HOST_TYPE>
# source ffsw.tcl
# ffsw <bootable-project-file> [<component-name>]
#
set path [wtxPath]/host/resource/tcl/app-config/Project
source $path/cmpScriptLib.tcl

global cpuDependentCmps
set cpuDependentCmps \
    {INCLUDE_DSP INCLUDE_DSP_SHOW INCLUDE_MMU_MPU INCLUDE_SW_FP INCLUDE_HW_FP \
     INCLUDE_MMU_FULL INCLUDE_MMU_FULL_SHOW}
global cpuDependentCmpAvailFor
array set cpuDependentCmpAvailFor {}
set cpuDependentCmpAvailFor(MC68000) {INCLUDE_SW_FP}
set cpuDependentCmpAvailFor(MC68020) {INCLUDE_SW_FP}
set cpuDependentCmpAvailFor(MC68040) \
    {INCLUDE_SW_FP INCLUDE_HW_SP INCLUDE_MMU_FULL INCLUDE_MMU_FULL_SHOW}
set cpuDependentCmpAvailFor(MC68LC040) \
    {INCLUDE_SW_FP INCLUDE_MMU_FULL INCLUDE_MMU_FULL_SHOW}
set cpuDependentCmpAvailFor(MC68060) \
    {INCLUDE_SW_FP INCLUDE_HW_SP INCLUDE_MMU_FULL INCLUDE_MMU_FULL_SHOW}
set cpuDependentCmpAvailFor(CPU32) {INCLUDE_SW_FP}
set cpuDependentCmpAvailFor(ARMARCH4) \
    {INCLUDE_MMU_MPU INCLUDE_SW_FP INCLUDE_MMU_FULL}
set cpuDependentCmpAvailFor(ARMARCH4_T) \
    {INCLUDE_MMU_MPU INCLUDE_SW_FP INCLUDE_MMU_FULL}
set cpuDependentCmpAvailFor(ARMARCH5) \
    {INCLUDE_MMU_MPU INCLUDE_SW_FP INCLUDE_MMU_FULL}
set cpuDependentCmpAvailFor(ARMARCH5_T) \
    {INCLUDE_MMU_MPU INCLUDE_SW_FP INCLUDE_MMU_FULL}
set cpuDependentCmpAvailFor(STRONGARM) {INCLUDE_SW_FP INCLUDE_MMU_FULL}
set cpuDependentCmpAvailFor(XScale) {INCLUDE_SW_FP INCLUDE_MMU_FULL}
set cpuDependentCmpAvailFor(MCF5200) {INCLUDE_SW_FP}
set cpuDependentCmpAvailFor(MCF5400) {INCLUDE_SW_FP}
set cpuDependentCmpAvailFor(PENTIUM) \
    {INCLUDE_SW_FP INCLUDE_HW_SP INCLUDE_MMU_FULL INCLUDE_MMU_FULL_SHOW}
set cpuDependentCmpAvailFor(PENTIUM) \
    {INCLUDE_SW_FP INCLUDE_HW_SP INCLUDE_MMU_FULL INCLUDE_MMU_FULL_SHOW}
set cpuDependentCmpAvailFor(PENTIUM2) \
    {INCLUDE_SW_FP INCLUDE_HW_SP INCLUDE_MMU_FULL INCLUDE_MMU_FULL_SHOW}
set cpuDependentCmpAvailFor(PENTIUM3) \
    {INCLUDE_SW_FP INCLUDE_HW_SP INCLUDE_MMU_FULL INCLUDE_MMU_FULL_SHOW}
set cpuDependentCmpAvailFor(PENTIUM4) \
    {INCLUDE_SW_FP INCLUDE_HW_SP INCLUDE_MMU_FULL INCLUDE_MMU_FULL_SHOW}
set cpuDependentCmpAvailFor(MIPS32) {INCLUDE_SW_FP}
set cpuDependentCmpAvailFor(MIPS64) {INCLUDE_HW_FP}
set cpuDependentCmpAvailFor(PPC860) {INCLUDE_SW_FP}
set cpuDependentCmpAvailFor(PPC603) {INCLUDE_HW_FP}
set cpuDependentCmpAvailFor(PPC604) {INCLUDE_HW_FP}
set cpuDependentCmpAvailFor(PPC403) {INCLUDE_SW_FP}
set cpuDependentCmpAvailFor(PPC405) {INCLUDE_SW_FP}
set cpuDependentCmpAvailFor(PPC440) {INCLUDE_SW_FP}
set cpuDependentCmpAvailFor(SH7600) {INCLUDE_DSP INCLUDE_DSP_SHOW INCLUDE_SW_FP}
set cpuDependentCmpAvailFor(SH7700) \
    {INCLUDE_DSP INCLUDE_DSP_SHOW INCLUDE_SW_FP INCLUDE_MMU_FULL \
     INCLUDE_MMU_FULL_SHOW}
set cpuDependentCmpAvailFor(SH7750) \
    {INCLUDE_MMU_FULL INCLUDE_MMU_FULL_SHOW INCLUDE_HW_FP}

global cmpOptional
set cmpOptional \
    {INCLUDE_CODETEST INCLUDE_EGL INCLUDE_EX INCLUDE_ENP INCLUDE_IE \
     INCLUDE_ILAC INCLUDE_LNSGI INCLUDE_NIC INCLUDE_NIC_EVB INCLUDE_MED \
     INCLUDE_QU INCLUDE_OLI INCLUDE_LNEBSA INCLUDE_FN INCLUDE_SNMPD}
			   
proc _ffsw {cxrDoc mxrDoc component bspDir} {
    global cpuDependentCmps
    global cpuDependentCmpAvailFor
    global cmpOptional

    set tc [mxrTcGet $mxrDoc]
    set cpu  [${tc}::cpu]
    set uninstalledSet [$cxrDoc setCreate]

    # Check datadocs for contents
    checkDocs $cxrDoc $mxrDoc

    # check for objects of all types not correctly/incompletely defined
    set unresolved [getThoseUnresolved $cxrDoc $component]
    $uninstalledSet = $unresolved

    # start with components with unresolved references
    set dependentOnUnresolved [getThoseDependentOnUnresolved \
        $cxrDoc $mxrDoc $component]
    $uninstalledSet = $dependentOnUnresolved

    # add components with missing MODULES
    set missingModules [getThoseMissingModules $cxrDoc $mxrDoc $component]
    $uninstalledSet = [$uninstalledSet + $missingModules]

    # add components with missing archives
    set missingArchives [getThoseMissingArchives $cxrDoc $mxrDoc $component]
    $uninstalledSet = [$uninstalledSet + $missingArchives]

    # add components with missing configlettes
    set missingConfiglettes [getThoseMissingConfiglettes \
        $cxrDoc $mxrDoc $component $bspDir]
    $uninstalledSet = [$uninstalledSet + $missingConfiglettes]

    # add components with missing BSP stubs
    set missingStubs [getThoseMissingStubs $cxrDoc $mxrDoc $component]
    $uninstalledSet = [$uninstalledSet + $missingStubs]

    # Get components w/ missing LINK_SYMS or LINK_DATASYMS
    set missingLinkSyms [getThoseMissingLinkSymbols $cxrDoc $mxrDoc $component]
    $uninstalledSet = [$uninstalledSet + $missingLinkSyms]

    # If muxLib.o is missing, remove everything under FOLDER_NETWORK
    # This is for proper handing of the integrated simulators, which
    # should contain no network components.
    set dependentOnMuxLib [getThoseDependentOnMuxLib $cxrDoc $mxrDoc $component]
    $uninstalledSet = [$uninstalledSet + $dependentOnMuxLib]


    # Reporting
    if {$component == ""} {
        if {$unresolved != ""} {
            puts "    **** PROBLEM: these objects are incorrectly defined or missing:"
            puts "$unresolved\n"
	    foreach entry $unresolved {
 	        getThoseUnresolved $cxrDoc $entry
	    }
        }

        if {$dependentOnUnresolved != ""} {
            puts "    **** PROBLEM: these Components depend on objects that are\
                incorrectly defined or missing:"
            puts "$dependentOnUnresolved\n"
	    foreach entry $dependentOnUnresolved {
 	        getThoseDependentOnUnresolved $cxrDoc $mxrDoc $entry
	    }
        }

        # Do not report optional components or unavailable cpu dependent
	# components

	set missingModulesReally {}
	foreach entry $missingModules {
	    if {([lsearch $cmpOptional $entry] == -1) && \
		([lsearch $cpuDependentCmps $entry] == -1) ||
	        ([lsearch $cpuDependentCmpAvailFor($cpu) $entry] != -1)} {
	        lappend missingModulesReally $entry
	    }
	}
        if {$missingModulesReally != ""} {
            puts "    **** PROBLEM: these objects refer to missing modules:"
            puts "$missingModulesReally\n"
	    foreach entry $missingModulesReally {
 	        getThoseMissingModules $cxrDoc $mxrDoc $entry
	    }
        }
        if {$missingArchives != ""} {
            puts "    **** PROBLEM: these objects refer to missing archives:"
            puts "$missingArchives\n"
	    foreach entry $missingArchives {
 	        getThoseMissingArchives $cxrDoc $mxrDoc $entry
	    }
        }

        # Do not report optional components or unavailable cpu dependent
	# components

	set missingConfiglettesReally {}
	foreach entry $missingConfiglettes {
	    if {([lsearch $cmpOptional $entry] == -1) && \
		([lsearch $cpuDependentCmps $entry] == -1) ||
	        ([lsearch $cpuDependentCmpAvailFor($cpu) $entry] != -1)} {
	        lappend missingConfiglettesReally $entry
	    }
	}
        if {$missingConfiglettesReally != ""} {
            puts "    **** PROBLEM: these objects refer to missing configlettes:"
            puts "$missingConfiglettesReally\n"
	    foreach entry $missingConfiglettesReally {
 	        getThoseMissingConfiglettes $cxrDoc $mxrDoc $entry $bspDir
	    }
        }

        if {$missingStubs != ""} {
            puts "    **** PROBLEM: these objects refer to missing bsp stubs:"
            puts "$missingStubs\n"
	    foreach entry $missingStubs {
 	        getThoseMissingStubs $cxrDoc $mxrDoc $entry
	    }
        }
        if {$missingLinkSyms != ""} {
            puts "    **** PROBLEM: these objects refer to linkage symbols \
                (LINK_SYMS or LINK_DATASYMS)"
	    puts "$missingLinkSyms\n"
	    foreach entry $missingLinkSyms {
 	        getThoseMissingLinkSymbols $cxrDoc $mxrDoc $entry
	    }
        }

        if {$dependentOnMuxLib != ""} {
            puts "    **** PROBLEM: these objects are dependent on muxLib.o, \
	        which is missing"
	    puts "$dependentOnMuxLib\n"
	    foreach entry $dependentOnMuxLib {
 	        getThoseDependentOnMuxLib $cxrDoc $mxrDoc $entry
	    }
        }
    }

    set _notInstalled [$uninstalledSet instances Component]    
    $uninstalledSet delete
    return $_notInstalled
}

proc checkDocs {cxrDoc mxrDoc} {
    if {[$cxrDoc instances] == ""} {
        puts "Component cross reference document is empty.\
	   Check for syntax errors?"
    }
    if {[$mxrDoc instances] == ""} {
        puts "Module cross reference document is empty.\
	   Check toolchain installation?"
    }
}

proc getThoseUnresolved {cxrDoc component} {
    if {$component != ""} {
        puts "...checking component definition for $component..."
        set cmp [$cxrDoc setCreate]
	$cmp = [$cxrDoc instances Unresolved]
	$cmp = [$cmp & $component]
	if {[$cmp instances] != ""} {
	    puts "    **** PROBLEM: component definition for $component missing or \
	        didn't parse correctly"
	}
        puts "done."
    }

    return [$cxrDoc instances Unresolved] 
}

# compute uninstalled components - those that are missing
# object modules or REQUIRE an Unresolved component
proc getThoseDependentOnUnresolved {cxrDoc mxrDoc component} {
    set cmp [$cxrDoc setCreate]

    # Get list of those that require unresolved components
    set _requireUnresolved ""

    if {$component != ""} {
        puts "...checking $component for dependencies on missing objects..."
        $cmp = $component
        $cmp = [$cmp get REQUIRES]
	if {[$cmp instances Unresolved] != ""} {
	    if {$component != ""} {
	        set unres [$cmp instances Unresolved]
	        puts "    **** PROBLEM: $component depends on missing item(s) $unres"
	    }
	    lappend _requireUnresolved $component
	}
        puts "done."
    } else {
        foreach item [$cxrDoc instances Component] { 
            $cmp = $item
            $cmp = [$cmp get REQUIRES]
	    if {[$cmp instances Unresolved] != ""} {
	        lappend _requireUnresolved $item
	    }
        }
        $cmp = $_requireUnresolved
        $cmp = [cxrSupertree $cmp $mxrDoc]
        set _requireUnresolved [$cmp instances Component]
    }

    $cmp delete

    return $_requireUnresolved
}

proc getThoseMissingModules {cxrDoc mxrDoc component} {
    if {$component != ""} {
        puts "...checking $component for missing modules..."
    }

    set cmpSet [$cxrDoc setCreate]
    set modSet [$mxrDoc setCreate]

    set allMods [$mxrDoc instances Module]
    $modSet = [$cxrDoc instances Module]
    $cmpSet = [$modSet - $allMods]
    $cmpSet = [$cmpSet get _MODULES]
    set _missingModules [$cmpSet instances Component]

    if {$component != ""} {
        $cmpSet = $component
        $cmpSet = [$cmpSet get MODULES]
	$cmpSet = [$cmpSet & [$modSet - $allMods]]
	if {[$cmpSet instances Module] != ""} {
	    puts "    **** PROBLEM: component requires modules \
	        [$cmpSet instances Module], but they are not in an archive"
	}
        puts "done."
    }

    $cmpSet delete
    $modSet delete

    return $_missingModules
}

proc getThoseMissingArchives {cxrDoc mxrDoc component} {
    if {$component != ""} {
        puts "...checking $component for missing archives..."
    }
    set cmpSet [$cxrDoc setCreate]
    set tc [mxrTcGet $mxrDoc]
    set hProj [mxrPrjHandleGet $mxrDoc]
    set cpu  [${tc}::cpu]
    set tool [${tc}::tool]
    set _missing ""

    if {$component != ""} {
	$cmpSet = $component
	if {[set lib [$cmpSet get ARCHIVE]] != ""} {
	    set lib [mxrLibsExpand $hProj $lib $tc]
	    if {![file exists $lib]} {
	        puts "    **** PROBLEM: archive for $component missing"	
	    }
	}
        puts "done."
    }
    foreach component [$cxrDoc instances Component] {
	$cmpSet = $component
	if {[set lib [$cmpSet get ARCHIVE]] != ""} {
	    set lib [mxrLibsExpand $hProj $lib $tc]
	    if {![file exists $lib]} {
		lappend _missing $component
	    }
	}
    }
    $cmpSet delete 
    return $_missing
}

proc getThoseMissingConfiglettes {cxrDoc mxrDoc component bspDir} {
    if {$component != ""} {
        puts "...checking $component for missing configlettes..."
    }
    set cmpSet [$cxrDoc setCreate]
    set tc [mxrTcGet $mxrDoc]
    set hProj [mxrPrjHandleGet $mxrDoc]
    set cpu  [${tc}::cpu]
    set tool [${tc}::tool]
    set _missing ""
    if {$component != ""} { 
        $cmpSet = $component
	set configlettes [$cmpSet get CONFIGLETTES]
	if {$configlettes != ""} {
	    foreach file $configlettes {
 		if {![file exists [wtxPath]target/config/comps/src/$file] &&
		    ![file exists $bspDir/$file] &&
		    ![file exists [wtxPath]target/src/drv/$file]} {
		     puts "    **** PROBLEM: $component specifies configlette \
		         $file, which is not present on your \
			 file system" 
		}
	    }
	}
        puts "done."
    }
    foreach component [$cxrDoc instances Component] {
        $cmpSet = $component
	set configlettes [$cmpSet get CONFIGLETTES]
	if {$configlettes != ""} {
	    foreach file $configlettes {
 		if {![file exists [wtxPath]target/config/comps/src/$file] &&
		    ![file exists $bspDir/$file] &&
		    ![file exists [wtxPath]target/src/drv/$file]} {
		     lappend _missing $component
		}
	    }
	}
    }
    $cmpSet delete 
    return $_missing
}

proc getThoseMissingStubs {cxrDoc mxrDoc component} {
    if {$component != ""} { 
        puts "...checking $component for missing BSP stubs..."
    }
    set cmpSet [$cxrDoc setCreate]
    set tc [mxrTcGet $mxrDoc]
    set hProj [mxrPrjHandleGet $mxrDoc]
    set cpu  [${tc}::cpu]
    set tool [${tc}::tool]
    set _missing ""
    if {$component != ""} { 
        $cmpSet = $component 
	set bspStubs [$cmpSet get BSP_STUBS]
	if {$bspStubs != ""} {
	    foreach file $bspStubs {
		set stub [wtxPath]target/config/comps/src/$file]
		if {![file exists $stub]} { 
		    puts "    **** PROBLEM: $component specifies BSP stub $stub, \
		       which is not present on your file system"
		}
	    }
	}
        puts "done."
    }
    foreach component [$cxrDoc instances Component] {
        $cmpSet = $component 
	set bspStubs [$cmpSet get BSP_STUBS]
	if {$bspStubs != ""} {
	    foreach file $bspStubs {
		if {![file exists [wtxPath]target/config/comps/src/$file]} {
		     lappend _missing $component
		}
	    }
	}
    }
    $cmpSet delete 
    return $_missing
}

# Get components w/ missing LINK_SYMS or LINK_DATASYMS
proc getThoseMissingLinkSymbols {cxrDoc mxrDoc component} {
    if {$component != ""} { 
        puts "...checking $component for missing linkage symbols..."
    }
    set modSet [$mxrDoc setCreate]
    set cmp [$cxrDoc setCreate]
    set _missingLinkSyms ""

    $modSet = [$mxrDoc instances Module]
    if {$component != ""} {
        $cmp = $component
    } else {
        $cmp = [$cxrDoc instances Component]
    }
    $cmp = "[$cmp get LINK_SYMS] [$cmp get LINK_DATASYMS]"
    $cmp = [$cmp - [cSymsDemangle [concat [$modSet get defines] \
		[$modSet get exports]] [mxrTcGet $mxrDoc]]]

    if {$component != ""} {
        if {[$cmp instances] != ""} {
            puts "****    PROBLEM: $component specifies these link symbols \
	        [$cmp instances], which were not found in the archive"
        }
        puts "done."
    }

    $cmp = "[$cmp get _LINK_SYMS] [$cmp get _LINK_DATASYMS]"
    set _missingLinkSyms [$cmp instances Component] 

    $modSet delete
    $cmp delete
    return $_missingLinkSyms
}

# If muxLib.o is missing, remove everything under FOLDER_NETWORK
# This is for proper handing of the integrated simulators, which
# should contain no network components.
proc getThoseDependentOnMuxLib {mxrDoc cxrDoc component} {
    set _dependent ""
    set modSet [$mxrDoc setCreate]
    set cmp [$cxrDoc setCreate]

    $modSet = muxLib.o
    if {[$modSet instances] == ""} {
	$cmp = FOLDER_NETWORK
	set _dependent [cxrSubfolders $cmp]
    }

    if {$component != ""} {
        puts "...checking $component for muxLib.o..."
        $cmp = $_dependent
        $cmp = [$cmp & $component]
        if {[$cmp instances] == $component} {
	    puts "    **** PROBLEM: $component depends on muxLib.o, \
		      which was not found in an archive."
        } else {
	    puts "...ok."
	}
        puts "done."
    }

    $modSet delete
    $cmp delete

    return $_dependent
}

proc ffsw {project {component ""}} {
    global env

    if {$env(WIND_HOST_TYPE) == ""} {
        puts "WIND_HOST_TYPE not defined"
	return
    }
    if {$project == "-h"} {
        puts "ffsw <project file> \[<component name>\]"
	return
    } elseif {$project == "-?"} {
        puts "ffsw <project file> \[<component name>\]"
	return
    }
    set hProj [prjOpen $project]
    set cxrDoc [cxrDocCreate $hProj]
    set mxrDoc [mxrDocCreate $hProj]
    set bspDir [prjTagDataGet $hProj BSP_DIR]

    _ffsw $cxrDoc $mxrDoc $component $bspDir

    prjClose $hProj
}

if {$argc == 1} {
    ffsw [lindex $::argv 0]
} elseif {$argc >=2} {
    ffsw [lindex $::argv 0] [lindex $::argv 1]
}
