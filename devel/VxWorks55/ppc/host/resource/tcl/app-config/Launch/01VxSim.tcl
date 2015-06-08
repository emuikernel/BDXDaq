# 01VxSim.tcl - VxSim support
#
# modification history
# --------------------
# 01u,28jan03,rbl  fix SPR 65793, VxSim can sometimes fail to launch on Unix hosts
# 01t,30nov98,jmp  saved last started simulator to ~/.wind/.vxsim_last_started
# 01s,23nov98,jmp  changed default simulator to Standard simulator.
# 01r,13nov98,jmp  added TSFS options to Target Server Configuration by
#                  default.
#		   offer to launch Target Server if simulator is already
#		   running (SPR# 23159).
# 01q,09nov98,jmp  added the ability to launch a simulator built from the
#                  Project Tool rather than the default simulator.
# 01p,02nov98,ren  Made wdbpipe the default backend for VxSim
# 01o,18mar98,jmb  HPSIM merge of jmb patch 25sep97: Add parisc-hpux10 
#                  host type. 
# 01n,18mar98,jmb  HPSIM merge of jmp patch 14apr97: Use runTerminal since
#                  xterm doesn't work for VxSim on hpux10
# 01m,07oct97,cth  moved tmp file in launchVxSimConfirm to .wind directory,
#		   made exec of processListCmd work correctly, added procNum
#		   to dialog message (SPR 8839)
# 01l,01mar96,ism  added support for SIMSPARCSOLARIS.
# 01k,05jan96,jco  check for already running simulator.
# 01j,11nov95,c_s  add initial ./ to vxWorks call so you don't need . in 
#                   your path.
# 01i,07nov95,jco  changed WIND_HOST_TYPE to wtxHostType call, made the -r
#		    option only if memsize != defaultMemSize.
# 01h,20oct95,jco  added memory size option, cleaned up. 
# 01g,10aug95,jco  removed shwait when exec'ing vxWorks (SPR# 4562).
# 01f,18jul95,jco  fixed the script removing bug (no script is used now, instead
#		     we use cd first to allow the "one command" use of shwait. 
#		     Added many checks and messages.
# 01e,28jun95,jco  remove checking of pre-existing tgtsvr, added comments,
#		     and changed sleep duration (from 3 to 2)
# 01d,19jun95,jco  fit to modified shwait.
# 01c,16jun95,jco  change xterm window and icon titles.
# 01b,16jun95,jco  set default value of processor number to 0.
# 01a,15jun95,jco  written, derived from 01WView.tcl and 01Supprt.tcl.
#

# dialogs summary
#
# 1 - dialogLaunchVxSim
# 	Processor number
# 	Memory size (bytes)

set defaultMemSize	3000000 ; # default memory pool size for simulator

set prjToolSimulator ""		;# get last started vxsim path
set prjToolSimSaveFile $env(HOME)/.wind/.vxsim_last_started
if [file exists $prjToolSimSaveFile] {
    set prjToolSimulator [exec cat $prjToolSimSaveFile]
    if {$prjToolSimulator != ""} {
	if {! [file exists $prjToolSimulator]} {
	    set prjToolSimulator ""
	}
    }
}

################################################################################
#
# launchVxSimCancel - unpost dialogLaunchVxSim dialog.
#

proc launchVxSimCancel {} { 

    dialogUnpost dialogLaunchVxSim 
}

################################################################################
#
# LaunchVxSim - check the platform availability and post the main dialog
#
# NOTE: to exec vxWorks simulator, we need to cd to the correct directory
# WIND_BASE/target/config/[sunos|hpux] --> sunos or hpux BSP simulator. This 
# round of checks also builds the simulator path name.
#
# GLOBALS: (R): env; (W): simPath
#

proc LaunchVxSim {} {
    global env
    global simPath
    global defaultMemSize

    # set simulator pathName prefix

    set simPath $env(WIND_BASE)/target/config

    # check platform avaibility

    switch -exact [wtxHostType] {

	sun4-sunos4 {
	    if {![file isdirectory $simPath/sunos]} {
		noticePost error "sunos VxWorks simulator directory not found"
		return
            }
	    set simPath $simPath/sunos
	}

	parisc-hpux9 {
	    if {![file isdirectory $simPath/hpux]} {
		noticePost error "hpux VxWorks simulator directory not found"
		return
            }
	    set simPath $simPath/hpux
	}

	parisc-hpux10 {
	    if {![file isdirectory $simPath/hpux]} {
		noticePost error "hpux VxWorks simulator directory not found"
		return
            }
	    set simPath $simPath/hpux
	}

	sun4-solaris2 {
            if {![file isdirectory $simPath/solaris]} {
                noticePost error "solaris VxWorks simulator directory not found"
                return
           }
           set simPath $simPath/solaris
	}

	default {
	    noticePost error "Not yet supported or wrong host type."
	    return
        }
    }

    # initialize options values

    dialogSetValue dialogLaunchVxSim "Processor number" 0
    dialogSetValue dialogLaunchVxSim "Memory size (bytes)" $defaultMemSize
    dialogSetValue dialogLaunchVxSim "VxWorks Image" $simPath/vxWorks

    dialogSetValue dialogLaunchVxSim "Standard simulator" 1
    dialogSetValue dialogLaunchVxSim "Custom-built simulator" 0
    dialogPost dialogLaunchVxSim
}

################################################################################
#
# launchVxSimConfirm - get options and execute the simulator.
#
# GLOBALS: (R) env, simPath
# RETURN: n/a
#

proc launchVxSimConfirm {} {
    global env
    global defaultMemSize
    global processListCmd
    global prjToolSimSaveFile prjToolSimulator

    if {[set entries [readEntries]] == ""} {return}

    set procNum [lindex $entries 0]
    set memSize [lindex $entries 1]

    # read selected image
    set vxWorksExecutable [dialogGetValue dialogLaunchVxSim "VxWorks Image"]
    set vxsimName [file tail $vxWorksExecutable]
    set vxsimPath [file dirname $vxWorksExecutable]

    if {![file exists $vxWorksExecutable] || \
    	![file isfile $vxWorksExecutable]} {
        noticePost error "no such file: $vxWorksExecutable"
	return
    }

    set prjToolSimulator $vxWorksExecutable
    if [dialogGetValue dialogLaunchVxSim "Custom-built simulator"] {
    	set fd [open $prjToolSimSaveFile w]
	puts $fd $prjToolSimulator
	close $fd
    }

    # check for already existing vxsim process with this processor number
    # not yet
    set vxsimRunning 0
    set tmpFile "$env(HOME)/.wind/vxsim[pid].tmp"
    eval [concat exec $processListCmd] > $tmpFile
    set fd [open $tmpFile r]
    while {![eof $fd]} {
        set line [gets $fd]
        if {[string first vxWorks $line] != -1} {
            set ix [lsearch $line "-p"]
	    if {$ix != -1} {
		if {[lindex $line [incr ix]] == $procNum} {
		    set reply [noticePost question \
	    "A VxWorks simulator $procNum is already running on this machine.
	    Connect to this simulator?" Yes No]

		    if {$reply} {
		    	set vxsimRunning 1
			break
		    } else {
		    	close $fd
			exec rm $tmpFile
		    	return
		    }
		}
	    }
	}
    }
    close $fd
    exec rm $tmpFile

    if {! $vxsimRunning} {
	# store initial directory

	set pwd [pwd]

	# change to simulator directory (simulator program requirement)

	if [catch {cd $vxsimPath} err] {
	    noticePost error "Changing to simulator directory error: $err"
	    dialogUnpost dialogLaunchVxSim
	    return
	}

	# exec the simulator program

	set cmd "./$vxsimName -p $procNum"
	if {$memSize != $defaultMemSize} {lappend cmd -r $memSize}

	eval [concat {runTerminal "Vxsim $procNum" 1} $cmd]

	# change back to the initial directory

	if [catch {cd $pwd} err] {noticePost warning "$err"}
    }

    # unpost current dialog and post the target server create dialog

    dialogUnpost dialogLaunchVxSim
    tgtsvrPost $procNum -backend wdbpipe

    # set TSFS root directory default
    set tsfsRoot $env(HOME)
    if {[file exists $env(HOME)/tmp]} {
    	set tsfsRoot $env(HOME)/tmp
    } elseif {[file exists $(HOME)/temp]} {
	set tsfsRoot $env(HOME)/temp
    } elseif {[file exists $env(HOME)/TMP]} {
	set tsfsRoot $env(HOME)/TMP
    }

    set tsfsRoot $tsfsRoot/tsfs
    if {![file exists $tsfsRoot]} {
	if [catch {file mkdir $tsfsRoot} err] {noticePost warning "$err"}
    }
    	
    dialogSetValue dialogTgtsvrMgr "TSFS Read/Write" 1
    dialogSetValue dialogTgtsvrMgr "TSFS root directory" $tsfsRoot
    cmdLineUpdate
}

################################################################################
#
# readEntries - read the forms entries.
#
# RETURN: a list of checked entries or ""
#

proc readEntries {{num 0}} {

    if [catch {checkEntries $num} result] {
	return
    }
    return $result
}

################################################################################
#
# checkEntries - check processor number and memory size fields.
#
# RETURN: checked value(s) if ok or raises an error.
#

proc checkEntries {num} {
    global defaultMemSize

    set result ""

    if {$num == 0 || $num == 1} {
	set errmesg1 "Processor number must be a positive number ranging from \
		      0 to 15."
	set procNum [dialogGetValue dialogLaunchVxSim "Processor number"]

	if {$procNum == ""} {set procNum 0}

	if {[regexp {[^0-9]+} $procNum] != 0 || $procNum < 0 || $procNum > 15} {
	    noticePost error $errmesg1
	    error $errmesg1
	}
	lappend result $procNum
    }

    if {$num == 0 || $num == 2} {
	set errmesg2 "Memory size must be a positive number."
	set memSize [dialogGetValue dialogLaunchVxSim "Memory size (bytes)"]

	if {$memSize == ""} {set memSize $defaultMemSize}

	if {[regexp {[^0-9]+} $memSize] != 0} {
	    noticePost error $errmesg2
	    error $errmesg2
	}
	lappend result $memSize
    }

    return $result
}

################################################################################
#
# defaultBooleanChange - binding for the "Standard simulator" boolean
#
proc defaultBooleanChange {defaultBool} {

    global simPath prjToolSimulator

    if {$defaultBool} {
	dialogSetValue dialogLaunchVxSim "Custom-built simulator" 0
	dialogSetValue dialogLaunchVxSim "Standard simulator" 1
	dialogSetValue dialogLaunchVxSim "VxWorks Image" $simPath/vxWorks
    } else {
    	dialogSetValue dialogLaunchVxSim "Custom-built simulator" 1
	dialogSetValue dialogLaunchVxSim "Standard simulator" 0
	dialogSetValue dialogLaunchVxSim "VxWorks Image" $prjToolSimulator
    }

}

################################################################################
#
# customBooleanChange - binding for the "Custom-built simulator" boolean
#
proc customBooleanChange {customBool} {

    global simPath prjToolSimulator

    if {$customBool} {
	dialogSetValue dialogLaunchVxSim "Standard simulator" 0
    	dialogSetValue dialogLaunchVxSim "Custom-built simulator" 1
	dialogSetValue dialogLaunchVxSim "VxWorks Image" $prjToolSimulator
    } else {
	dialogSetValue dialogLaunchVxSim "Standard simulator" 1
	dialogSetValue dialogLaunchVxSim "Custom-built simulator" 0
	dialogSetValue dialogLaunchVxSim "VxWorks Image" $simPath/vxWorks
    }
}

################################################################################
#
# simulatorSelect - simulator executable selection dialog
#
proc simulatorSelect {} {

    global env
    global prjToolSimulator

    customBooleanChange 1

    # save current directory
    set saveDir [pwd]

    # set default directory for browse start
    set defaultDir $env(WIND_BASE)/target/proj

    # get current value
    set curValue [dialogGetValue dialogLaunchVxSim "VxWorks Image"]

    # if current value is coherent use it otherwise use the default value
    if {$curValue != "" && [file exists $curValue]} {
	if [file isdirectory $curValue] {
	    set browseDir $curValue
	} else {
	    set browseDir [file dirname $curValue]
	}
    } else {
    	set browseDir $defaultDir
    }

    # call fileselect
    cd $browseDir
    set file [noticePost fileselect "Select VxWorks Image"]

    # restore directory
    cd $saveDir

    if {$file != ""} {
	dialogSetValue dialogLaunchVxSim "VxWorks Image" $file
	set prjToolSimulator $file
    }
}

################################################################################
#
# 01VxSim_entry - 01VxSim.tcl entry point
#

proc 01VxSim_entry {} {

    # Add a VxSim activation button at the bottom of the launcher

    objectCreate app vxsim    VxSim 	{LaunchVxSim}

    # create dialogLaunchVxSim dialog 
     
    dialogCreate dialogLaunchVxSim -size 450 300			      \
        {								      \
	{label "Select the executable VxWorks image to be launched:"          \
	    -y 10 -left 0}						      \
	{boolean "Standard simulator" -top 10 -left 0 defaultBooleanChange}   \
	{boolean "Custom-built simulator" -top "Standard simulator+5"         \
	    -left 0 customBooleanChange}				      \
	{button "Browse..." -top "Standard simulator"                         \
	    -left 55 -right 90 simulatorSelect}				      \
	{text "VxWorks Image" -top "Custom-built simulator+10"		      \
	    -left 0 -right 100 {}}					      \
	{text "Processor number" -top "VxWorks Image+15"       		      \
	    -left 0 -right 50 {readEntries 1}}                     	      \
	{text "Memory size (bytes)" -top "VxWorks Image+15"         	      \
	    -left 50 -right 100 {readEntries 2}}			      \
	{button "Confirm" -top "Processor number+15" -left 10 -right 45       \
	    launchVxSimConfirm} 					      \
	{button "Cancel" -top "Processor number+15" -left 55 -right 90 	      \
	    launchVxSimCancel} 						      \
	}
    }

################################################################################
#
# entry point
#

01VxSim_entry
