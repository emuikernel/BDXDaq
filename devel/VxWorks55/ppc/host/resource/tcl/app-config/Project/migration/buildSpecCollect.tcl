# buildSpecCollect.tcl - get the contents of all default build specifications
#
# Copyright 2001 Wind River Systems, Inc.
#
# modification history
# ------------------------
# 01g,05jun02,j_s  'rule' instead of 'ruleList' should be checked for build
#                  target list
# 01f,04jun02,j_s  just copy build rules if target is a list like "a b: <...>"
# 01e,09apr02,j_s  collect default value of BUILD_RULE_*
# 01d,24jan02,j_s  improve error reporting when $bspDir/Makefile doesn't exist
# 01c,17jan02,j_s  also collect default value of PROJECT_FILES
# 01b,15jan02,j_s  substitute '/' for '\' in file path
# 01a,11jan02,j_s  give warning instead of error when Makefile is missing from
#                  a bsp
#
# DESCRIPTION
#   This script works with Project Migration Tool. It must be run in the
#   environment of the source Tornado. So prjMigrate.tcl creates a 
#   sub-process to run this script.
#
#   Content of a build specification can be different depending on project type
#   and bsp used. This script gets the content of each combination of
#   build specification,project type and bsp(for bootable project only).
#
# SYNOPSIS
#   wtxtcl buildSpecCollect.tcl buildSpecSourceType buildSpecSource
#
source [wtxPath]host/resource/tcl/app-config/Project/cmpScriptLib.tcl
source [wtxPath]host/resource/tcl/app-config/Project/wccLib.tcl
source [wtxPath]host/resource/tcl/app-config/Project/dataDocLib.tcl
source [wtxPath]host/resource/tcl/app-config/Project/prjLib.tcl
source [wtxPath]host/resource/tcl/app-config/Project/prjFileLib.tcl
source [wtxPath]host/resource/tcl/app-config/Project/prjBuildLib.tcl
source [wtxPath]host/resource/tcl/app-config/Project/tc_cplus.tcl
source [wtxPath]host/resource/tcl/app-config/Project/prj_vxWorks.tcl
source [wtxPath]host/resource/tcl/app-config/Project/Utils.tcl
if [file exists $::env(WIND_BASE)/buildSpecContent] {
    source $::env(WIND_BASE)/buildSpecContent
}

set macroToSkip {\
		CC CFLAGS AS CFLAGS_AS CXX C++FLAGS LD LDFLAGS EXTRA_CFLAGS \
		NM SIZE AR CPU TOOL MACH_DEP MACH_EXTRA LIB_EXTRA \
	        EXTRA_DEFINE \
		}

set buildSpecSourceType [lindex $::argv 0]
set buildSpecSource [lindex $::argv 1]
global prjTagData

if {$buildSpecSourceType == "bsp"} {
    set bspDir $::env(WIND_BASE)/target/config/$buildSpecSource
    regsub -all {\\} $bspDir "/" bspDir
    if [file exists $bspDir/Makefile] {

        # for bsp, create a vxWorks project to get build specification

        set bspName [file tail $bspDir]
	set prjFile $::env(WIND_BASE)/target/proj/migration_${bspName}/migration_${bspName}.wpj
	regsub -all {\\} $prjFile "/" prjFile
	if {![file exists $prjFile]} {
	    ::prj_vxWorks_hidden::vxProjCreate $prjFile $bspDir
	}
	set hProj [prjOpen $prjFile]
	::prj_vxWorks_hidden::bspMakeParse $bspDir

	# content of build specification may vary from bsp to bsp
	# so use bsp name to differentiate them

	set tc [prjBuildTcGet $hProj [prjBuildCurrentGet $hProj]]
	regsub {::} $tc "" tcContent
	set buildId ${tcContent}${bspName}

 	if ![info exists $buildId] {
	    # Contents of all build specification will be put into one file, 
	    # all contents are appended at the end of the file

	    set dataFile $::env(WIND_BASE)/buildSpecContent
	    regsub -all {\\} $dataFile "/" dataFile
	    set fd [open $dataFile "a+"]

	    # get the content of build specification from project

	    set mList [array names prjTagData $hProj,BUILD_default_MACRO_*]
	    regsub -all ${hProj},BUILD_default_MACRO_ $mList "" mList
	    foreach macro $mList {
	        puts $fd "set ${buildId}($macro) \
	        \{[prjRelPath $hProj [prjBuildMacroGet $hProj default $macro]]\}"
	    }

	    # get additional content of build specification from bsp

	    foreach macro [::prj_vxWorks_hidden::bspMakeGet] {
	        if {[lsearch $macroToSkip $macro] == -1} {
	            puts $fd "set ${buildId}($macro) \
		        \{[prjRelPath $hProj \
				 [::prj_vxWorks_hidden::bspMakeGet $macro]]\}"
	        }
	    }

	    # collect default value of PROJECT_FILES and WCC_*_COMPONENTS

	    puts $fd "set ${buildId}(PROJECT_FILES) \{[prjRelPath $hProj \
			  $prjTagData($hProj,PROJECT_FILES)]\}"
	    set wcc [wccCurrentGet $hProj]
	    puts $fd "set ${buildId}(WCC_${wcc}_COMPONENTS) \
			  \{[wccComponentListGet $hProj $wcc]\}"

            # collect default value of BUILD_RULE_*

	    set ruleList [array names prjTagData $hProj,BUILD_RULE_*]
	    regsub -all ${hProj},BUILD_RULE_ $ruleList "" ruleList
	    foreach rule $ruleList {

		# XXX - need to find a better solution for the case where
		#       a list of targets share build rule, e.g.
		#       'vxWorks_low vxWorks_low.sym'

		if {[llength "$rule"] == 1} {
	            puts $fd "set ${buildId}($rule) \
	            \{[prjRelPath $hProj [prjBuildRuleGet $hProj default $rule]]\}"
                }
	    }

	    prjClose $hProj
	    close $fd
	}
    } else {
	puts "Warning: ${bspDir}/Makefile doesn't exist!"
    }
} elseif {$buildSpecSourceType == "vxApp"} {

    set tc $buildSpecSource
    regsub {::} $tc "" tcContent

    if ![info exists ${tcContent}vxApp] {
        # content of build specification is different for projects of different
        # type get content of build specification for vxApp project

        set prjFileApp $::env(WIND_BASE)/target/proj/migration_${tcContent}/migration_${tcContent}.wpj
	regsub -all {\\} $prjFileApp "/" prjFileApp
        if {![file exists $prjFileApp]} {
            ::prj_vxApp_hidden::projCreate $prjFileApp $tc
        }
        set hProj [prjOpen $prjFileApp]
        set build [prjBuildCurrentGet $hProj]
        set mList [array names prjTagData $hProj,BUILD_${build}_MACRO_*]
        regsub -all ${hProj},BUILD_${build}_MACRO_ $mList "" mList
	set dataFile $::env(WIND_BASE)/buildSpecContent
	regsub -all {\\} $dataFile "/" dataFile
        set fd [open $dataFile "a+"]
        foreach macro $mList {
            puts $fd "set ${tcContent}vxApp($macro) \
	        \{[prjRelPath $hProj [prjBuildMacroGet $hProj $build $macro]]\}"
        }
        prjClose $hProj
        close $fd
    }
}
