##############################################################################
# 01BuildOutput.unix.tcl
#
# Copyright (C) 1998-1999 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,11mar99,cjtc Replaced the old Build Boot Roms popup menu tree with a new
#		   dialog box invoked from Build->Build Boot ROM. SPR #25459
#		   Added Modhist to existing file element.
# 

proc isBuildActive {} {
	return 0
}

proc buildLaunch {args} {
    eval ::BuildLaunch::buildStart $args
}


#
# wrapper_buildLaunch
#
# emulates wrapper_buildLaunch for X86 host on a UNIX host
#

proc wrapper_buildLaunch {args} {
    set windowTitle [lindex $args 0]
    set target [lindex $args 1]
    set workingDir [lindex $args 2]
    
    set cmd "make $target"
    
    eval ::BuildLaunch::buildStart {$windowTitle $cmd $workingDir}
    
}



namespace eval BuildLaunch {

    proc buildStart {windowTitle cmd workingDir} {
	global env

	if {[info exists env(WIND_BUILDTOOL)]} {
 		set windBuildTool $env(WIND_BUILDTOOL)
   		regsub -all %CMD% $windBuildTool "cd $workingDir; $cmd" windBuildTool
 		exec /usr/bin/sh -c "$windBuildTool" &
	} else {
	    set currentDir [pwd]
	    cd $workingDir
	    runTerminal "$windowTitle" 1 /bin/sh -c "echo cd $workingDir\;echo $cmd\;$cmd\;echo hit ENTER to exit;read line" 
	    cd $currentDir
	}
    }

}
