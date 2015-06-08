# 01TgtSvr.tcl - Target Server managing tcl library
#
# Copyright (C) 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 03k,18mar02,cmf  mods for SPR 73232 and 73502
# 03j,25jan02,sfp  merge from swindon to alameda
# 03i,21jan02,tcr  enable TSFS r/w by default
# 03h,17oct01,cmf  modify target server launch window for SPR 28406
# 03g,26sep01,c_c  Added -noG option.
# 03f,01feb99,jmp  modified to correctly update tgtsvr command line when core
#                  file is selected using browse dialog (SPR# 24754).
# 03e,28jan99,jmp  fixed Serial line speed field handling (SPR# 24723).
# 03d,23nov98,jmp  added default to the OMF list and to the Serial line speed
#                  list.
# 03c,19nov98,jmp  no longer source windHelpLib.tcl on startup, source it on
#                  first windHelp invocation.
# 03b,18nov98,jmp  no longer select Virtual console when Target I/O Redirect
#                  is selected.
# 03a,09nov98,jmp  increased Create Target Server dialog size.
# 02z,05nov98,jmp  added Target Server File System options: -R -RW (SPR# 22905).
#		   added file selection buttons for the core file...
# 02y,02nov98,ren  made wdbpipe the default backend for VxSim only
# 02x,28oct98,jmp  updated Help binding.
# 02w,24sep98,p_m  Changed tgtsvr log window title
# 02v,10apr98,fle  made dialog window bigger.
# 02u,30mar98,fle  added -use_portmapper and -Wf options
#                  + modified serialMgr to not ait for wdbserial backend when
#                    line speed is specified
#                  + history cleanup
# 02t,23mar98,fle  fixed unknown options recovering was hiding target name
# 02s,17mar98,fle  fixed IP Address recovering problem
# 02r,09mar98,fle  made Laucher able to accept backend-related unknown options
#                  + added new target server options (-r, -Bm, -Wm, -Wd)
# 02q,10oct97,elp  force backend log file creation before exec tail
# 02p,10feb97,jco  fixed the omf list intialization.
# 02o,05feb97,jco  fixing bug introduced by last mods.
# 02n,02oct96,elp  added -s option support (SPR# 6775).
# 02m,18jun96,jco  fixed spr number in previous history comment.
# 02l,18jun96,jco  made the tgtsvr config file more flexible (in particular 
#		    to allow non existing core file because of a bsp dir 
#		    displacement, spr# 6757).
# 02k,01feb96,jco  fixed Backend Viewer title (spr #6074), used runTerminal 
#		    interface instead of xterm, catched wtxInfoQ as the fix of 
#		    spr# 5750 makes it now able to return an error.
# 02j,28nov95,jco  catched mkdir and removed -p option to mkdir, updated
#		    first line's comment of this file.
# 02i,10nov95,jco  completed check removal while updating command line,
#		    fixed last launched tgtsvr support and accept hexadecimal
#		    format for -m option.
# 02h,08nov95,jco  added -m option support, typo: "an heavy" to "a heavy",
#		    moved all entries checking at launch time, added the 
#		    memory of the last launched tgtsvr in the file
#		    .wind/tgtsvr/.lastTgtSvr.
# 02g,07nov95,jco  changing WIND_HOST_TYPE to wtxHostType call.
# 02f,04nov95,jco  fixing null entry event for serial line device.
# 02e,03nov95,jco  changed "Backend Log file" in "Backend Log file", and 
#		    upercase typos in -V viewer header (target server --> 
#		    Target Server).
# 02d,03nov95,jco  fixed the ~ support (conditionnal expansion) and made
#		    conditional the viewers depending on the global noViewers.
#		    changed registeringDelay to scanMax.
# 02c,03nov95,jco  added support for ~ in file names (core and autho.).
# 02b,02nov95,jco  changed the viewer so that it does not kill the tgtsvr
#		    when exiting.
# 02a,10oct95,jco  redesigned, cleaned up, changed .wpwr into .wind, removed
#		    options -F and -defeat to tgtsvr command, added a notice
#		    when tgtsvr fails (spr# 5206).
# 01a,05mar95,jcf  extracted from Launch.tcl.
#*/

################################################################################
#
# 01TgtSvr_entry - 01TgtSvr.tcl entry point.
#

proc 01TgtSvr_entry {} {
    global env

    # creates .wind and .wind/tgtsvr if necessary

    if {![file exists $env(HOME)/.wind]} {
	if { [catch [file mkdir $env(HOME)/.wind] result] } {
	    error $result
	}
    }

    if {![file exists $env(HOME)/.wind/tgtsvr]} {
	if { [catch [file mkdir $env(HOME)/.wind/tgtsvr] result] } {
	    error $result
	}
    }

    # create global variables 

    globalsCreate

    # create dialogs

    dialogsCreate
}

################################################################################
#
# globalsCreate - create global variables for 01TgtSvr.tcl.
#

proc globalsCreate {} {
    global env			;# environment variables 
    global pairFlagList		;# flag list for valued options
    global pairFlagInfo		;# entry type and GUI label for each non
				;# boolean option
    global newPairFlagList	;# new flags (post 1.x) for target servers
    global newPairFlagInfo	;# new flags (post 1.x) info for ne option flags
    global typesOfCheck		;# entry types list
    global typesOfCheckMsg	;# entry type error messages 
    global boolFlagList		;# flag list for boolean options
    global newBoolFlagList	;# new boolean flags (post 1.x) for servers
    global brList		;# serial line speed list
    global bkList		;# backend list
    global omflist		;# object module format list
    global scanMax 		;# register scanning maximum number
    global noViewers		;# flag on if we don't want viewers for -V -Wd
				;# and -Bd

    # setting noViewers and scanMax (can be overwritten with users init value)

    set noViewers 0
    set scanMax 5

    # set flag lists for parsing purpose.

    set typesOfCheck {file fileE integer integerDH}

    set typesOfCheckMsg(file)    "must be a name with no space"
    set typesOfCheckMsg(fileE)   "no such file."
    set typesOfCheckMsg(integer) "must be a positive integer."
    set typesOfCheckMsg(integerDH) "must be a positive integer \
	(decimal or hexa. format)."

    # don't add new pair flags in this variable, but lower in newPairFlagList

    set pairFlagList "-n -B -u -display -Bt -Br -d -b -f -Bd -m -c"

    set pairFlagInfo(-n)	"file {Target server name}"
    set pairFlagInfo(-B)	"none {Backend list}"
    set pairFlagInfo(-u)	"fileE {Authorized users file}"
    set pairFlagInfo(-c)	"fileE {Core file}"
    set pairFlagInfo(-display)	"none {Console display}"
    set pairFlagInfo(-Bt)	"integer {Backend timeout}"
    set pairFlagInfo(-Br)	"integer {Backend resend}"
    set pairFlagInfo(-d)	"none {Serial line device}"
    set pairFlagInfo(-b)	"integer {Serial line speed}"
    set pairFlagInfo(-f)	"none {Object module format}"
    set pairFlagInfo(-Bd)	"file {Backend Log file}"
    set pairFlagInfo(-m)	"integerDH {Memory cache size}"

    # don't add new bool flags in this variable, but lower in newBoolFlagList

    set boolFlagList "-L -C -N -A -V -h -v -s"

    # new options flags

    set newPairFlagList "-Root -Bm -Wd -Wf -Wm"

    set newPairFlagInfo(-Root)	"file {TSFS root directory}"
    set newPairFlagInfo(-Bm)	"integer {Backend Log file max size}"
    set newPairFlagInfo(-Wd)	"file {WTX Log file}"
    set newPairFlagInfo(-Wf)	"none {WTX Log file filter}"
    set newPairFlagInfo(-Wm)	"integer {WTX Log file max size}"

    set newBoolFlagList "-redirectIO -redirectShell -use_portmapper -RW -noG"

    # initialize the serial line speed list

    set brList "default 1200 2400 4800 9600 19200 38400 other..."

    # initialize the backend list

    set bkList {default}
    set backenddir [wtxPath host [wtxHostType] lib backend]*
    foreach file [glob -nocomplain $backenddir] {
	lappend bkList [file rootname [file tail $file]]
    }

    # use a resource file to determine the list of available object formats

    set omfresfile $env(WIND_BASE)/host/resource/loader/omflist
    
    # initialize the object module format list

    set omflist [exec cat $omfresfile | grep -v '^#' | grep load | awk "\{print \$2\}" | sort -u]
    set omflist [linsert $omflist 0 "default"]
# XXX : fle : this would be a solution to avoid using grep, awk and sort
# commands in scrpit, it is longer, but portable ...
#
#    set omflist ""
#    set fileId [open $omfresfile "r"]
#    set line [gets $fileId]
#
#    while {! [eof $fileId] } {
#	if { [regexp {^#} $line] == 0 && [regexp {load} $line] == 1 } {
#	    # check if it is not already in list
#
#	    if {! [regexp [lindex $line 1] $omflist] } {
#		lappend omflist [lindex $line 1]
#	    }
#	}
#	set line [gets $fileId]
#    }
#    close $fileId
#    set omflist [lsort $omflist]
}

################################################################################
#
# dialogsCreate - create dialogs for 01TgtSvr.tcl.
#

proc dialogsCreate {} {

    # create dialogTgtsvrMgr dialog
    # XXX : fle : dialog size is set to 840/740 so that users of Exceed
    # can work in 1024x768 resolution

    dialogCreate dialogTgtsvrMgr -size 840 740		                       \
    {									       \
    {list "Saved configurations" -y 5 -height 80 -left 0 -right 50 readConfig} \
    {text "Target name or IP address" -y 5 -left 50 -right 100  cmdLineUpdate} \
    {label "Target Server Options" -top "Saved configurations+15" -left 39}    \
    {text "Target server name" -top "Target Server Options" -left 0 -right 33  \
    								cmdLineUpdate} \
    {text "Authorized users file" -top "Target Server Options" -left 33        \
    						-right 95	cmdLineUpdate} \
    {button "..." -top "Target Server Options+24" -right 100 -width 30         \
						-height 30  authUsrFileSelect} \
    {list "Object module format" -top "Authorized users file" -left 0          \
					-right 33 -height 65	cmdLineUpdate} \
    {text "Core file" -top "Authorized users file" -left 33 -right 95          \
								cmdLineUpdate} \
    {button "..." -top "Authorized users file+24" -right 100 -width 30         \
						-height 30	coreFileSelect}\
    {boolean "Target I/O Redirect" -top "Object module format+25"  -left 0     \
								consoleMgr}    \
    {boolean "Shell I/O Redirect" -top "Object module format+25"  -left 20     \
                                                                consoleMgr}    \
    {boolean "Virtual console" -top "Object module format+25" -left 40	       \
								consoleMgr}    \
    {text "Console display" -top "Object module format" -left 60 -right 80     \
								displayMgr}    \
    {boolean "No symbols" -top "Virtual console+15" -left 0	noSymMgr}      \
    {boolean "All symbols" -top "Virtual console+15" -left 20	allSymMgr}     \
    {boolean "Target/Host symbol tables synchronization"                       \
			-top "Virtual console+15"  -left 40	cmdLineUpdate} \
    {boolean "Use port mapper" -top "Virtual console+15" -left 80
                                                        	cmdLineUpdate} \
    {boolean "Verbose" -top "No symbols+15" -left 0 		cmdLineUpdate} \
    {boolean "Locked" -top "No symbols+15" -left 20	  	cmdLineUpdate} \
    {boolean "TSFS Read/Write" -top "No symbols+15" -left 40    cmdLineUpdate} \
    {text "TSFS root directory" -top "No symbols" -left 60 -right 95           \
								cmdLineUpdate} \
    {button "..." -top "No symbols+24" -right 100 -width 30                    \
						-height 30	tsfsRootSelect}\
    {text "Memory cache size" -top "Verbose" -left 0  -right 25 cacheMgr}      \
    {boolean "Do not allow automatic growth of target server memory pool" -top 
                                          "Verbose+24" -left 25 cmdLineUpdate} \
    {list "Backend list" -top "Memory cache size" -height 65 -left 0           \
							-right 25 backendMgr}  \
    {list "Serial line speed" -top "Memory cache size" -height 65 -left 25     \
							-right 50 serialMgr}   \
    {text "Serial line device" -top "Memory cache size" -left 50               \
    							-right 75 serialMgr}   \
    {text "Backend timeout" -top "Backend list" -left 0 -right 25              \
								cmdLineUpdate} \
    {text "Backend resend" -top "Backend list" -left 25 -right 50              \
								cmdLineUpdate} \
    {text "Backend Log file" -top "Backend list" -left 50 -right 75            \
								cmdLineUpdate} \
    {text "Backend Log file max size" -top "Backend list" -left 75             \
						-right 100	cmdLineUpdate} \
    {text "WTX Log file" -top "Backend Log file" -left 0 -right 25             \
								cmdLineUpdate} \
    {text "WTX Log file max size" -top "Backend Log file" -left 25 -right 50   \
								cmdLineUpdate} \
    {text "WTX Log file filter" -top "Backend Log file" -left 50 -right 75      \
								cmdLineUpdate} \
    {text "Target server launch command" -top @+15 -hspan 	}              \
    {button "Help..." -top "Target server launch command+15" -left 0 -right 25 \
    								helpCmd}       \
    {button "Delete" -top "Target server launch command+15" -left 25 -right 50 \
    								tgtsvrDelete}  \
    {button "Launch" -top "Target server launch command+15" -left 50 -right 75 \
    								tgtsvrLaunch}  \
    {button "Quit" -top "Target server launch command+15" -left 75	       \
						    -right 100 tgtsvrUnpost}   \
    }
   
    # create "Pick out-of-list value" dialog

    dialogCreate "Pick out-of-list value" -size 200 100 		       \
    {                                                                          \
    {text "Serial line speed" -y 5 -left 0 -right 100 newbpsValue}             \
    {button "Apply" -top "Serial line speed" -left 10 -right 50 newbpsValue}   \
    {button "Cancel" -top "Serial line speed" -left 50 -right 90               \
	{dialogUnpost "Pick out-of-list value"}}                               \
    }
}

################################################################################
#
# cacheMgr - handle default value display and fire the cmdLineUpdate.
#

proc cacheMgr {} {

    if {[dialogGetValue dialogTgtsvrMgr "Memory cache size"] == ""} {
	 dialogSetValue dialogTgtsvrMgr "Memory cache size" "default"
    }
    cmdLineUpdate
}

################################################################################
#
# newbpsValue - get out-of-list serial speed value.
#

proc newbpsValue {} {
    global brList

    set exoticValue \
	[dialogGetValue "Pick out-of-list value" "Serial line speed"]

    cmdLineUpdate "exoticSerialSpeedValue" $exoticValue

    if {$exoticValue == "" || [lsearch $brList $exoticValue] != -1} {
        listSelectionChange dialogTgtsvrMgr "Serial line speed" $exoticValue
    } else {
        listSelectionChange dialogTgtsvrMgr "Serial line speed" "other..."
    }

    dialogUnpost "Pick out-of-list value"
}

################################################################################
#
# serialMgr - control backend selection.
#

proc serialMgr {args} {

    if {[dialogGetValue dialogTgtsvrMgr "Serial line speed"] == "other..."} {
	dialogPost "Pick out-of-list value"
	return
    } else {
        cmdLineUpdate
    }
}
   
################################################################################
#
# backendMgr - control serial items selection.
#

proc backendMgr {value} {

    set backend [dialogGetValue dialogTgtsvrMgr "Backend list"]
    if {$backend != "wdbserial" && $backend != "wdbportsvr"} {
	if {[dialogGetValue dialogTgtsvrMgr "Serial line speed"] != ""} {
    	    listSelectionChange dialogTgtsvrMgr "Serial line speed" "" 
	}
	if {[dialogGetValue dialogTgtsvrMgr "Serial line device"] != ""} {
	    dialogSetValue dialogTgtsvrMgr "Serial line device" ""
	}
    }

    cmdLineUpdate
}

################################################################################
#
# displayMgr - control console flag.
#

proc displayMgr {} {

    if {[dialogGetValue dialogTgtsvrMgr "Console display"] != "" && \
	[dialogGetValue dialogTgtsvrMgr "Virtual console"] == 0} {
	dialogSetValue dialogTgtsvrMgr "Virtual console" 1
    }

    if {[dialogGetValue dialogTgtsvrMgr "Console display"] == "" && \
	[dialogGetValue dialogTgtsvrMgr "Virtual console"] == 1} {
	dialogSetValue dialogTgtsvrMgr "Virtual console" 0
    }

    cmdLineUpdate
}

################################################################################
#
# consoleMgr - control virtual display content.
#

proc consoleMgr {value} {

    if {$value == 0 && \
	[dialogGetValue dialogTgtsvrMgr "Console display"] != ""} {
	dialogSetValue dialogTgtsvrMgr "Console display" ""
    }

    if {[dialogGetValue dialogTgtsvrMgr "Shell I/O Redirect"] == 1 && \
	[dialogGetValue dialogTgtsvrMgr "Virtual console"] == 0 } {
	dialogSetValue dialogTgtsvrMgr "Virtual console" 1
    }

    cmdLineUpdate
}

################################################################################
#
# noSymMgr - noSym/allSym Toggle buttons XOR managing.
#

proc noSymMgr {value} {

    if {$value == 1 && [dialogGetValue dialogTgtsvrMgr "All symbols"] == 1} {
	dialogSetValue dialogTgtsvrMgr "All symbols" 0
    }
    cmdLineUpdate
}

################################################################################
#
# allSymMgr - noSym/allSym Toggle buttons XOR managing.
#

proc allSymMgr {value} {

    if {$value == 1 && [dialogGetValue dialogTgtsvrMgr "No symbols"] == 1} {
	dialogSetValue dialogTgtsvrMgr "No symbols" 0
    }
    cmdLineUpdate
}

################################################################################
#
# listSelectionChange - change list selection.
#
# change current selection to new one and scroll list with new selection on top.
#

proc listSelectionChange {dialog dialogList newSelection} { 

    set cs [dialogGetValue $dialog $dialogList]
    dialogListDeselect $dialog $dialogList $cs
    dialogListSelect $dialog -top $dialogList $newSelection
}

################################################################################
#
# helpCmd - call the tornado help tool.
#

proc helpCmd {} { 

    if {[info proc windHelpDisplay] == ""} {
    	# Source windHelp library
	uplevel #0 source [wtxPath host resource doctools]windHelpLib.tcl
    }

    set htmlAddr [windHelpLinkGet -name tgtsvr]

    if {$htmlAddr != ""} {
    	windHelpDisplay $htmlAddr
    } else {
    	error "Tornado help files not installed"
    }
}

################################################################################
#
# tgtsvrSave - save a target server configuration file.
#
# SYNOPSIS:
#   tgtsvrSave cmdLine fileName
#
# PARAMETERS:
#   cmdLine : command line to launch target server
#   fileName : name of the file to save command line in
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc tgtsvrSave {cmdLine fileName} {
    global env				;# environment variables
    global boolFlagList			;# boolean flag list (1.x)
    global pairFlagList			;# pair flags list (1.x)
    global newBoolFlagList		;# new boolean flags list (post 1.x)
    global newPairFlagList		;# new pair flags list (post 1.x)
    global newPairFlagInfo		;# new pair flags list (post 1.x)

    # save cmd line in fileName. This is the V 1.x file, so save only 1.x
    # options

    if [catch {open $env(HOME)/.wind/tgtsvr/$fileName w} fileId] {
	noticePost error "tgtsvrSave: Can't open file: $fileName" 
	return
    }
    set oldCmdLine ""			;# V 1.x command line

    # compute old command line from new one

    set locusMin [llength $cmdLine]

    # get pair flags

    foreach pairFlag $pairFlagList {
	set locus [lsearch $cmdLine $pairFlag]
	if { $locus != -1 } {
	    if { $locusMin > $locus } {
		set locusMin $locus
	    }
	    set oldCmdLine [format "%s %s %s" $oldCmdLine $pairFlag \
			    [lindex $cmdLine [expr $locus + 1]]]
	}
    }

    # get boolean flags

    foreach boolFlag $boolFlagList {
	set locus [lsearch $cmdLine $boolFlag]
	if { $locus != -1 } {
	    if { $locusMin > $locus } {
		set locusMin $locus
	    }
	    set oldCmdLine [format "%s %s" $oldCmdLine $boolFlag]
	}
    }

    # we've got the options lets get the rest

    incr locusMin -1
    set oldCmdLine [format "%s %s" [lrange $cmdLine 0 $locusMin] $oldCmdLine]
    puts $fileId $oldCmdLine
    close $fileId

    # save target name in $env(HOME)/.wind/tgtsvr/$fileName.V_2
    # if we want to always be backward compatible, the only way is to write each
    # flag and its associated value(s) a line.
    #
    # If a later version comes with n option flags while V2 has only m, a V2+
    # launcher version may save new options if file, cause even if a V2 launcher
    # tries to use this file, it will only try to access the first m flags.

    if { [catch {open $env(HOME)/.wind/tgtsvr/.$fileName.V_2+ w} fileId]} {
	noticePost error "tgtsvrSave: Can't open file: .$fileName.V_2+"
	return
    }

    # first save command line (for unknown options), target name, then flags

    puts $fileId $cmdLine
    puts $fileId [dialogGetValue dialogTgtsvrMgr "Target name or IP address"]

    foreach flag $newPairFlagList {	;# pair flags
	puts $fileId "$flag [dialogGetValue dialogTgtsvrMgr \
			    [lindex $newPairFlagInfo($flag) 1]]"
    }

    foreach boolFlag $newBoolFlagList {	;# boolean flags
	set boolFlagValue 0
	set locus [lsearch $cmdLine $boolFlag]
	if { $locus != -1 } {
	    set boolFlagValue 1
	}
	puts $fileId "$boolFlag $boolFlagValue"
    }

    close $fileId

    # update Configurations list

    set tgtsvrs [exec ls $env(HOME)/.wind/tgtsvr]
    dialogSetValue dialogTgtsvrMgr "Saved configurations" $tgtsvrs

    # put the current target server name in a file to select it again next time

    if [catch {open $env(HOME)/.wind/tgtsvr/.lastTgtSvr w+} fileId] {
	noticePost error "tgtsvrSave: Can't open file: .lastTgtSvr" 
	return
    }
    puts $fileId $fileName
    close $fileId
}

################################################################################
#
# cmdLineParse - parse command line and extract target name.
#
# If a syntax error is detected, an access to help window is suggested.
#
# RETURNS: target name, "" if no target name can be extracted, "ERROR" on flag
#          errors
#

proc cmdLineParse {} {

    # read Command Line

    set cmd [dialogGetValue dialogTgtsvrMgr "Target server launch command"]

    set values() {}

    if [catch {targetNameInCmdLineGet $cmd values} result] {
	if {[noticePost question "Tgtsvr command syntax error:\n$result" \
	    "Help" "Cancel"] == 1} {
	    helpCmd
	}
	return "ERROR"
    }

    if {$result == "BAD_ENTRY"} {
	return "ERROR"
    }

    return $values(name)
}

################################################################################
#
# cmdLineUpdate - update the command line.
#

proc cmdLineUpdate {args} {
    global pairFlagList
    global boolFlagList
    global newPairFlagList
    global newBoolFlagList

    # initialize form array

    foreach flag $pairFlagList {set form($flag) ""}
    foreach flag $boolFlagList {set form($flag) "0"}

    # read form's entries

    set form() {}
    bindNamesToList { \
	form(cfg) form(name) form(-n) form(-u) form(-f) form(-c) \
	form(-redirectIO) form(-redirectShell) form(-C) form(-display) \
	form(-N) form(-A) form(-s) form(-use_portmapper) form(-V) form(-L) \
	form(-RW) form(-Root) form(-m) form(-noG) form(-B) form(-b) form(-d) form(-Bt) \
	form(-Br) form(-Bd) form(-Bm) form(-Wd) form(-Wm) form(-Wf) \
	} \
	[dialogGetValues dialogTgtsvrMgr]

    if {[lindex $args 0] == "exoticSerialSpeedValue"} {
	set form(-b) [lindex $args 1]
    }

    # expand the ~ for the path fields:
    # Authorized users, Core file, Backend Log, WTX Log, TSFS Root

    foreach flag "-u -c -Bd -Wd -Root" {
	set homePath "[file dirname ~]/[file tail ~]"
	regsub -all "\~" $form($flag) $homePath form($flag)
    }



    # get rid of default values

    if {$form(-f) == "default"} {set form(-f) ""}
    if {$form(-B) == "default"} {set form(-B) ""}
    if {$form(-m) == "default"} {set form(-m) ""}
    if {$form(-b) == "default"} {set form(-b) ""}
    if {$form(-b) == "other..."} {
        set cmd  [dialogGetValue dialogTgtsvrMgr "Target server launch command"]
	set locus [lsearch $cmd "-b"]
	set exoticSpeed [lindex $cmd [incr locus]]
	set form(-b) $exoticSpeed 
    }

    # build command

    set cmd "tgtsvr $form(name)"

    foreach flag $boolFlagList {
	if {$form($flag) != "0"} {
	    lappend cmd $flag 
	}
    }

    foreach flag $pairFlagList {
	if {$form($flag) != ""} {
	    lappend cmd $flag  $form($flag)
	}
    }

    # new flags (post 1.x)

    foreach flag $newBoolFlagList {
	if {$form($flag) != "0"} {
	    lappend cmd $flag 
	}
    }

    foreach flag $newPairFlagList {
	if {$form($flag) != ""} {
	    lappend cmd $flag  $form($flag)
	}
    }

    # display command line

    dialogSetValue dialogTgtsvrMgr "Target server launch command" $cmd
}

################################################################################
#
# entryCheck - check entry according to its type
#

proc entryCheck {label value type} {
    global typesOfCheck

    if {$type == "none"} {return}

    if {[lsearch $typesOfCheck $type] == -1} {
	return ERROR
    }

    if {$type == "file" || $type == "fileE"} {
	if {[llength $value] > 1} {
	    return ERROR
	}
    }

    if {$type == "fileE"} {
	if {$value != "" && ![file isfile $value]} {
	    return ERROR
	}
    }

    if {$type == "integer"} {
       if {[regexp {[^0-9]+} $value] != 0} {
	   return ERROR
       }
    }

    if {$type == "integerDH"} {
	if {[regexp -nocase {^0x.*} $value] != 1} {
	    if {[regexp {[^0-9]+} $value] != 0} {
	        return ERROR
	    }
	} else {
	    # get rid of two first characters
	    set remainder [string range $value 2 end]
	    if {[string length $remainder] < 1} {
		return ERROR
	    }
	    # analyse remainder part
	    if {[regexp -nocase {[^0-9a-f]+} $remainder] != 0} {
		return ERROR
	    }
	}
    }

    return
}

################################################################################
#
# newFlagsParse - parse the new flags in command line
#
# SYNOPSIS:
#   newFlagsParse wconfig retArray parse
#
# PARAMETERS:
#   wconfig : command line coming from UI
#   retArray : returned array
#   parse : display error flag location
#
# RETURNS: command line without new flags
#
# ERRORS:
#   attribute is required with flag $flag
#   command must start with 'tgtsvr'
#   entry error
#

proc newFlagsParse {wconfig retArray {parse 1}} {
    upvar $retArray valArr
    global newPairFlagList		;# new pair flags list (post 1.x)
    global newPairFlagInfo		;# new pair flags info (post 1.x)
    global newBoolFlagList		;# new boolean flag list (post 1.x)
    global typesOfCheckMsg

    # parse the command line to retrieve the fields used in the forms

    if {[lindex $wconfig 0] != "tgtsvr"} {
	error "command must start with 'tgtsvr'."
    }

    set wconfig [lreplace $wconfig 0 0]

    foreach flag $newPairFlagList {

	set valArr($flag) ""
	set locus [lsearch $wconfig $flag]

	if {$locus != -1} {
	    set locusp [expr $locus +1]

	    # check attribute presence 

	    set value [lindex $wconfig $locusp]

	    if {$value == ""} {
		error "attribute is required with flag $flag."
	    }

	    # check the attribute

	    set label [lindex $newPairFlagInfo($flag) 1]
	    set type  [lindex $newPairFlagInfo($flag) 0]

	    if {[entryCheck $label $value $type] == "ERROR"} {
		if {$parse} {
		    noticePost error "Entry error in \[$label\]\n\
				  \"$value\": $typesOfCheckMsg($type)"
		    return "BAD_ENTRY"
		}
		noticePost warning "Entry error in \[$label\]\n\
				\"$value\": $typesOfCheckMsg($type)"
	    }

	    # assign returned variable

	    set valArr($flag) $value

	    # erase 2 fields in the working list

	    set wconfig [lreplace $wconfig $locus $locusp]
        }
    }

    foreach flag $newBoolFlagList {
	set valArr($flag) 0
	set locus [lsearch $wconfig $flag]
	if {$locus != -1} {

	    # assign returned variable

	    set valArr($flag) 1

	    # erase 1 field in the working list

	    set wconfig [lreplace $wconfig $locus $locus]
	}
    }

    return $wconfig
}

################################################################################
#
# targetNameInCmdLineGet - get the target name field in the command line.
#
# SYNOPSYS:
#   targetNameInCmdLineGet wconfig retArray parse
#
# PARAMETERS:
#   wconfig : command line coming from UI
#   retArray : returned array
#   parse : display error flag location
#
# RETURNS: nothing on success, BAD_ENTRY else
#
# ERRORS:
#   attribute is required with flag $flag
#   entry error
#   target name is missing
#

proc targetNameInCmdLineGet {wconfig retArray {parse 1}} {
    upvar $retArray valArr
    global pairFlagList			;# pair flag list (1.x)
    global pairFlagInfo			;# pair flag info (1.x)
    global newPairFlagList		;# new pair flags list (post 1.x)
    global newPairFlagInfo		;# new pair flags info (post 1.x)
    global boolFlagList			;# boolean flag list (1.x)
    global newBoolFlagList		;# new boolean flag list (post 1.x)
    global typesOfCheckMsg

    # get rid of the new flags

    set wconfig [newFlagsParse $wconfig $retArray parse]
    if { $wconfig == "BAD_ENTRY" } {
	return "BAD_ENTRY"
    }

    # retrieve flags in command line

    foreach flag $pairFlagList {

	set valArr($flag) ""
	set locus [lsearch $wconfig $flag]

	if {$locus != -1} {
	    set locusp [expr $locus +1]

	    # check attribute presence 

	    set value [lindex $wconfig $locusp]

	    if {$value == ""} {
		error "attribute is required with flag $flag."
	    }

	    # check the attribute

	    set label [lindex $pairFlagInfo($flag) 1]
	    set type  [lindex $pairFlagInfo($flag) 0]

	    if {[entryCheck $label $value $type] == "ERROR"} {
		if {$parse} {
		    noticePost error "Entry error in \[$label\]\n\
				  \"$value\": $typesOfCheckMsg($type)"
		    return "BAD_ENTRY"
		}
		noticePost warning "Entry error in \[$label\]\n\
				\"$value\": $typesOfCheckMsg($type)"
	    }

	    # assign returned variable

	    set valArr($flag) $value

	    # erase 2 fields in the working list

	    set wconfig [lreplace $wconfig $locus $locusp]
        }
    }

    foreach flag $boolFlagList {
	set valArr($flag) 0
	set locus [lsearch $wconfig $flag]
	if {$locus != -1} {

	    # assign returned variable

	    set valArr($flag) 1

	    # erase 1 field in the working list

	    set wconfig [lreplace $wconfig $locus $locus]
	}
    }

    if {$wconfig == ""} {
	error "target name is missing."
    }

    # if it only remains one item in line, it must be target name, if not, there
    # must be some backend-related unknown options.

    if { [llength $wconfig] == 1 } {
	set valArr(name) $wconfig
    } elseif {! [info exists valArr(name)] } {
	set valArr(name) ""
    }

    return 
}

################################################################################
#
# readConfig - read configuration file.
#
# read target server configuration file and repopulate form's fields.
#
# SYNOPSIS:
#   readConfig selection
#
# PARAMETERS:
#   selection : selected target server file name
#
# RETURNS: ERROR on failure, nothing else
#
# ERRORS:
#   Reading configuration file error
#   Syntax error in configuration file
#

proc readConfig {selection} {
    global env
    global brList
    global newPairFlagList			;# new pair flags (post 1.x)
    global newBoolFlagList			;# new bool flags (post 1.x)

    # read configuration file

    if {![file exists ~/.wind/tgtsvr/$selection]} {
	noticePost error \
	    "Reading configuration file error.\n$selection: No such file."
	return ERROR
    }

    set valuArr() {}

    set fileId [open $env(HOME)/.wind/tgtsvr/$selection "r"]
    set cmd [gets $fileId]
    close $fileId

    # get V 2+ options and target name in .<selection>.V_2+ if existing

    if { [file exists $env(HOME)/.wind/tgtsvr/.$selection.V_2+] } {
	set fileId [open $env(HOME)/.wind/tgtsvr/.$selection.V_2+ "r"]
	set cmd [gets $fileId]
	set valuArr(name) [gets $fileId]

	# get the remaining items as flags

	set flagsList ""
	set savedFlag [gets $fileId]
	while { ! [eof $fileId] } {
	    lappend flagsList $savedFlag
	    set savedFlag [gets $fileId]
	}
	close $fileId

	# get pair flags

	foreach pairFlag $newPairFlagList {

	    set valuArr($pairFlag) ""

	    foreach savedFlag $flagsList {
		if { [lindex $savedFlag 0] == $pairFlag &&
		     [llength $savedFlag] > 1 } {
		    set valuArr($pairFlag) [lindex $savedFlag 1]
		    break
		}
	    }
	}

	# get boolean flags

	foreach boolFlag $newBoolFlagList {

	    set valuArr($boolFlag) 0

	    foreach savedFlag $flagsList {
		if { [lindex $savedFlag 0] == $boolFlag &&
		     [llength $savedFlag] > 1 } {
		    set valuArr($boolFlag) [lrange $savedFlag 1 end]
		    break
		}
	    }
	}
    } else {					;# file doesn't exist

	# initialize pair flags with null strings

	foreach pairFlag $newPairFlagList {
	    set valuArr($pairFlag) ""
	}

	# initialize bool flags

	foreach boolFlag $newBoolFlagList {
	    set valuArr($boolFlag) 0
	}
    }

    # parse command line

    if [catch {targetNameInCmdLineGet $cmd valuArr 0} result] {
	if {[noticePost question "Syntax error in configuration file \
	    ~/.wind/tgtsvr/$selection:\n $result\n" "Help" "Cancel"] == 1} {
	    helpCmd
	}
	return "ERROR"
    } 

    if {$result == "BAD_ENTRY"} {
	return
    }

    # take care of default values

    if {$valuArr(-m) == ""} {set valuArr(-m) default} 
    if {$valuArr(-m) == 0}  {set valuArr(-m) default}

    # populate form's fields and select list's items as necessary

    dialogSetValue dialogTgtsvrMgr "Target server launch command" $cmd
    dialogSetValue dialogTgtsvrMgr "Target name or IP address" $valuArr(name)
    dialogSetValue dialogTgtsvrMgr "Memory cache size" $valuArr(-m) 
    dialogSetValue dialogTgtsvrMgr "Target server name" $valuArr(-n) 
    dialogSetValue dialogTgtsvrMgr "Authorized users file" $valuArr(-u)
    dialogSetValue dialogTgtsvrMgr "Core file" $valuArr(-c)
    dialogSetValue dialogTgtsvrMgr "Backend Log file" $valuArr(-Bd)
    dialogSetValue dialogTgtsvrMgr "Console display" $valuArr(-display)
    dialogSetValue dialogTgtsvrMgr "Backend timeout" $valuArr(-Bt)
    dialogSetValue dialogTgtsvrMgr "Backend resend" $valuArr(-Br)
    dialogSetValue dialogTgtsvrMgr "Serial line device" $valuArr(-d)
    dialogSetValue dialogTgtsvrMgr "No symbols" $valuArr(-N)
    dialogSetValue dialogTgtsvrMgr "Verbose" $valuArr(-V)
    dialogSetValue dialogTgtsvrMgr "Use port mapper" $valuArr(-use_portmapper)
    dialogSetValue dialogTgtsvrMgr \
                   "Do not allow automatic growth of target server memory pool" $valuArr(-noG)
    dialogSetValue dialogTgtsvrMgr "Locked" $valuArr(-L)
    dialogSetValue dialogTgtsvrMgr "TSFS Read/Write" $valuArr(-RW)
    dialogSetValue dialogTgtsvrMgr "TSFS root directory" $valuArr(-Root)
    dialogSetValue dialogTgtsvrMgr "All symbols" $valuArr(-A)
    dialogSetValue dialogTgtsvrMgr "Backend Log file max size" $valuArr(-Bm)
    dialogSetValue dialogTgtsvrMgr "WTX Log file" $valuArr(-Wd)
    dialogSetValue dialogTgtsvrMgr "WTX Log file max size" $valuArr(-Wm)
    dialogSetValue dialogTgtsvrMgr "WTX Log file filter" $valuArr(-Wf)
    dialogSetValue dialogTgtsvrMgr "Target/Host symbol tables synchronization" \
		   $valuArr(-s)
 
    dialogSetValue dialogTgtsvrMgr "Virtual console" $valuArr(-C)
    dialogSetValue dialogTgtsvrMgr "Target I/O Redirect" $valuArr(-redirectIO)
    dialogSetValue dialogTgtsvrMgr "Shell I/O Redirect" $valuArr(-redirectShell)

    # NB: -b flag can have out of list value. 

    if {[lsearch $cmd "-b"] != -1} {
    	if {[lsearch $brList $valuArr(-b)] == -1} {
	    set valuArr(-b) "other..."
	}
    } else {
    	set valuArr(-b) "default"
    }

    if {[lsearch $cmd "-B"] == -1} {
	set valuArr(-B) "default"
    }

    if {[lsearch $cmd "-f"] == -1} {
	set valuArr(-f) "default"
    }

    listSelectionChange dialogTgtsvrMgr "Serial line speed" $valuArr(-b)
    listSelectionChange dialogTgtsvrMgr "Backend list" $valuArr(-B)
    listSelectionChange dialogTgtsvrMgr "Object module format" $valuArr(-f)
}

################################################################################
#
# tgtsvrUnpost - unpost the tgtsvr create dialog.
#

proc tgtsvrUnpost {} {

    dialogUnpost dialogTgtsvrMgr
}

################################################################################
#
# tgtsvrNew - reset full_form's fields.
#

proc tgtsvrNew {} {
    global env
    global bkList
    global brList
    global omflist

    # get configurations list

    set cfList [exec ls $env(HOME)/.wind/tgtsvr]

    # reset form's fields

    dialogSetValues dialogTgtsvrMgr [list $cfList {} {} {} $omflist {} 0 0 0 {}\
				     0 0 0 0 0 0 1 $env(HOME) default 0 $bkList $brList \
				     {} {} {} {} {} {} {} {} {}]
    dialogListSelect dialogTgtsvrMgr "Object module format" "default"
    dialogListSelect dialogTgtsvrMgr "Backend list" "default"
    dialogListSelect dialogTgtsvrMgr "Serial line speed" "default"
}

################################################################################
#
# tgtsvrInit - reset full_form's fields.
#
# SYNOPSIS:
#   tgtsvrInit tgtName(opt) [-backend theBackend]
#
# PARAMETERS:
#   tgtName : target server name defaulted to "none"
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc tgtsvrInit { args} {
    global env
    global bkList
    global brList
    global omflist

    #process args
    if { [llength $args] == 0 || [lindex $args 0] == "-backend" } {
	set tgtName "none"
    } else {
	set tgtName [lindex $args 0]
    }
    set backEnd "default"
    set backEndIndex [lsearch -exact $args "-backend"]
    if { $backEndIndex >= 0 } {
	incr backEndIndex
	catch { set backEnd [lindex $args $backEndIndex] }
    }

    # post the main dialog

    dialogPost dialogTgtsvrMgr

    # reset fields 

    tgtsvrNew

    # init the backend

    dialogListSelect dialogTgtsvrMgr "Backend list" $backEnd

    # init. the current list of existing tgtsvr config. files

    set cfList [exec ls $env(HOME)/.wind/tgtsvr]

    if {$tgtName != "none"} {
	if {[lsearch $cfList $tgtName] != -1} {
	    readConfig $tgtName
    	    dialogListSelect dialogTgtsvrMgr -top "Saved configurations" \
		$tgtName
	} else {
	    dialogSetValue dialogTgtsvrMgr "Target server launch command" \
		"tgtsvr $tgtName"
	    dialogSetValue dialogTgtsvrMgr "Target name or IP address" \
		$tgtName
	}
    } else {
	if {[file exists $env(HOME)/.wind/tgtsvr/.lastTgtSvr]} {
	    set tgtName [exec cat $env(HOME)/.wind/tgtsvr/.lastTgtSvr]
	    if {[lsearch $cfList $tgtName] != -1} {
		readConfig $tgtName
		dialogListSelect dialogTgtsvrMgr -top "Saved configurations" \
		    $tgtName
	    }
	}
    }
}

################################################################################
#
# tgtsvrPost - prepare and post the tgtsvr create dialog.
#

proc tgtsvrPost {procNum args} {
    global env

    # build the simulator name if we've been called from the vxsim button

    if {$procNum != "none"} {
	eval tgtsvrInit [list "vxsim$procNum"] $args
    } else {
	eval tgtsvrInit [list "none"] $args
    }
}

################################################################################
#
# tgtsvrLaunch - execute tgtsvr command.
#

proc tgtsvrLaunch {} {
    global env
    global scanMax

    # get the command line

    set cmdLine [dialogGetValue dialogTgtsvrMgr "Target server launch command"]

    if {$cmdLine == ""} {
	noticePost error "No command to launch!"
	return
    }

    # get the target server name and check both syntax and entries

    set name [cmdLineParse]
    if {$name== "ERROR"} {
        return 
    }

    # target name may have been impossible to extract from command line

    if { $name == "" } {
	set name [dialogGetValue dialogTgtsvrMgr "Target name or IP address"]
    }

    # if -n option is empty, update it with name

    if { [dialogGetValue dialogTgtsvrMgr "Target server name"] == "" } {
	dialogSetValue dialogTgtsvrMgr "Target server name" $name
	cmdLineUpdate
    }

    # refresh command line

    set cmdLine [dialogGetValue dialogTgtsvrMgr "Target server launch command"]

    # if cmd line contains "-n", next field is the target server name.

    set locus [lsearch $cmdLine "-n"]
    if {$locus != -1} {
	set name [lindex $cmdLine [incr locus]]
    } 

    # check if name is not already used. 
    # check the target server name against the existing tgtsvr list.
    # the following assumes that the uname -n returns the host name.
    # if name already existing then doesn't launch.

    set fullName $name@[exec uname -n]
    if [catch "wtxInfoQ" lstSvr] {
	    noticePost error "wtxInfoQ error: $lstSvr"
	    exit
    }

    if {[lsearch [join $lstSvr] $fullName] != -1} {
	noticePost error "$fullName already exists!"
	return
    }

    # save the target server configuration in file <$name>

    tgtsvrSave $cmdLine $name

    tgtsvrUnpost
    wsSync

    # manage viewers on -V and -Bd options

    viewersMgr $cmdLine $name

    # is the new target server alive or did it fail

    set registerScanNum 0
    while {$registerScanNum < $scanMax} {
	if [catch "wtxInfoQ" lstSvr] {
	    noticePost error "wtxInfoQ error: $lstSvr"
	    exit
	}
	if {[lsearch [join $lstSvr] $fullName] == -1} {
	    exec sleep 1
	} else {
	    break
	}
        incr registerScanNum
    }

    if {$registerScanNum >= $scanMax} {
	set extraText ""
	if {[lsearch $cmdLine "-V"] == -1} {
	    set extraText \
	       "\nFor more details, please try again with the -V.erbose option."
	}

	noticePost warning "Target server: $name is not alive \
	    $scanMax \
	    seconds after the tgtsvr command was invoked.\nThis indicates \
	    a heavy load on the host or network or an error.$extraText"
    }

    targetServerListUpdate 1
}

################################################################################
#
# viewersMgr - manage viewers on -V and -Bd and -Wd options.
#

proc viewersMgr {cmdLine name} {
    global env
    global noViewers

    if {$noViewers == 0} {	; # noViewers code

	# log file viewers for -V and -Bd <logFile> and -Wd <logFile> options
	# of tgtsvr command

	# backend log file

	set locus [lsearch $cmdLine "-Bd"]
	if {$locus != -1} {
	    set bdLog [lindex $cmdLine [incr locus]]
	    if {! [file exists $bdLog]} {
		close [open $bdLog a+]
	    }
	    runTerminal "$name - Backend Log file ($bdLog)" 1 tail -f $bdLog
	}

	# WTX log file

	set locus [lsearch $cmdLine "-Wd"]
	if {$locus != -1} {
	    set WLog [lindex $cmdLine [incr locus]]
	    if {! [file exists $WLog]} {
		close [open $WLog a+]
	    }
	    runTerminal "$name - WTX Log file ($WLog)" 1 tail -f $WLog
	}

	# verbose mode

	if {[lsearch $cmdLine "-V"] != -1} {

	    foreach file [glob -nocomplain /tmp/launchLog*] {
		exec /bin/rm -f $file
	    }

	    set launchLogTmpFile $env(HOME)/.wind/launchLog.$name

	    set fd [open $launchLogTmpFile w+]
            puts $fd \
		"Killing this session log viewer will not kill the Target Server."
	    puts $fd ""
	    puts $fd "Current session log is available in:"
	    puts $fd "$launchLogTmpFile"
	    puts $fd ""
	    puts $fd "Command: $cmdLine"
	    puts $fd ""
	    close $fd
     
	    runTerminal "$name - Session Log Viewer" 1 tail -100f $launchLogTmpFile
	    eval exec $cmdLine >>& $launchLogTmpFile &

	} else {
	    eval exec $cmdLine &
	}
    } else { 			; # noViewers code 
        eval exec $cmdLine &	; # noViewers code
    }			 	; # noViewers code	
}

################################################################################
#
# tgtsvrDelete - delete a saved target server configuration file.
#

proc tgtsvrDelete {} {
    global env

    set fileName [dialogGetValue dialogTgtsvrMgr "Saved configurations"]

    if {$fileName == ""} {
	noticePost error "Select a Configuration first."
	return
    }

    if { [file exists $env(HOME)/.wind/tgtsvr/$fileName] } {
	file delete $env(HOME)/.wind/tgtsvr/$fileName
    }

    if { [file exists $env(HOME)/.wind/tgtsvr/.$fileName.V_2+] } {
	file delete $env(HOME)/.wind/tgtsvr/.$fileName.V_2+
    }

    # update the saved configurations list

    set tgtsvrs [exec ls $env(HOME)/.wind/tgtsvr]
    dialogSetValue dialogTgtsvrMgr "Saved configurations" $tgtsvrs

    # reset forms

    tgtsvrNew
}



################################################################################
#
# coreFileSelect - Core file selection
#

proc coreFileSelect {} {

    global env

    fileSelect "Core file" "Select target core filename" \
    	$env(WIND_BASE)/target
}

################################################################################
#
# authUsrFileSelect - Authorized users file selection 
#

proc authUsrFileSelect {} {
    fileSelect "Authorized users file" "Select authorized users file"
}

################################################################################
#
# tsfsRootSelect - TSFS Root directory selection 
#

proc tsfsRootSelect {} {
    fileSelect "TSFS root directory" "Select TSFS root directory"
}

################################################################################
#
# fileSelect - target server generic file selection
#
# ARGUMENTS:
#
proc fileSelect {fileDialog fileSelectTitle {defaultDir ""}} {

    global env

    # save current directory
    set saveDir [pwd]

    # set default directory for browse start
    if {$defaultDir == ""} {
    	if [file exists $env(HOME)] {
	    set defaultDir $env(HOME)
	} else {
	    set defaultDir $saveDir
	}
    }

    # get current value
    set curValue [dialogGetValue dialogTgtsvrMgr $fileDialog]

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
    set corefile [noticePost fileselect $fileSelectTitle]

    # restore directory
    cd $saveDir

    if {$corefile != ""} {
	dialogSetValue dialogTgtsvrMgr $fileDialog $corefile
    }

    # update tgtsvr command line
    cmdLineUpdate
}

################################################################################
#									       #
#                            01TgtSvr.tcl entry point.			       #
#									       #
################################################################################

01TgtSvr_entry 
