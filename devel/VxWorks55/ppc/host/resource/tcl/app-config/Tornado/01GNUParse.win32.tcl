##############################################################################
# app-config/Tornado/01GNUParse.win32.tcl - Tornado (Windows) Build output
#
# Copyright (C) 1995-96 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,05nov01,rbl  start mod history. Get build output parsing working for
#                  diab
#


#
# buildStart - This procedure is called just before a build is started
# so that any call back routines can be setup
#
proc buildStart {} {
	# reset any exists tag info
	resetCurrentTagInfo
	
	# set the necessary callbacks
	setCallbacks -select lineSelected -prev previousTag -next nextTag
}


#
# lineSelected - This procedure is a callback when user double clicks on a
# line.  The GUI passes the indexed line number.
#
proc lineSelected {args} {
	set selLine [lindex $args 0]
	if {[llength $args] > 1} {
		set beep [lindex $args 1]
	} {
		set beep ""
	}

	# first retrive the output line 
	set errorLine [getOutputLine $selLine]

	# Next get the working directory for the build
	set workingDir [getOutputWorkingDirectory]

	set elements [splitMessage $errorLine]
	set line ""
	set text ""

	set filename [lindex $elements 0]
	set line [lindex $elements 1]
	set type [lindex $elements 2]
	set text [lindex $elements 3]

	if {$line != "" && $text != ""} {
		set errorFile [format "%s\\%s" $workingDir $filename]
		if ![file exists $errorFile] {
			set errorFile $filename
			# set errorFile [format "%s\\%s" $workingDir $filename]
		}

		# reset any tag info
		resetCurrentTagInfo
		onOpenFile output 1 $errorFile $line
	
		highlightOutputLine $selLine
		if [string match {*warning*} $type] {
			statusMessageSet -warning [concat $type$text]
			setCurrentTagInfo -warning -file $errorFile -line $line
		} {
			statusMessageSet -error [concat $type$text]
			setCurrentTagInfo -error -file $errorFile -line $line
		}

		return 1
	}

	if ![string match {*-ignore*} $beep] {
		messageBeep -hand
	}

	return 0
}


#
# previousTag - This procedure is again another call back to jump from tag to 
# tag backward.  This option appears as a menu item under edit menu
#
proc previousTag {} {
	# First get the currently highlighted line
	set tagLine [getOutputHighlightLine]

	# get the total number of output lines
	set lineCount [getOutputLineCount]

	# Now loop through the lines backwards and stop when
	# an error or warning is hit
	for {set i [expr $tagLine - 1]} {$i >= 0} {incr i -1} {
		if {[lineSelected $i -ignore] == 1} {
			return
		}
	}

	messageBeep -hand
	statusMessageSet "Beginning of tag list reached"
}


#
# nextTag - This procedure is again another call back to jump from tag to 
# tag foreward.  This option appears as a menu item under edit menu.
#
proc nextTag {} {
	# First get the currently highlighted line
	set tagLine [getOutputHighlightLine]

	# get the total number of output lines
	set lineCount [getOutputLineCount]

	# Now loop through the lines forekwards and stop when
	# an error or warning is hit
	for {set i [expr $tagLine + 1]} {$i < $lineCount} {incr i} {
		if {[lineSelected $i -ignore] == 1} {
			return
		}
	}

	messageBeep -hand
	statusMessageSet "End of tag list reached"
}

#######################################################################
#
# splitMessage - This procedure actually splits the error message and
# creates the vars that need to be passed back.  Users could overwrite
# this proc to parse an output line
#
# This procedure parses both gnu and diab error messages.
# 
# diab error message is of form
#
# "d:\fred\linkSyms.c", line 9: error (dcc:1056): illegal function definition
#
# gnu error message is of form
#
# d:\fred\linkSyms.c:9: syntax error before `typedef'
#
proc splitMessage {errorLine} {
	set errorInfoList ""

	if {$errorLine != ""} {
		set filename ""
		set line ""
		set type ""
		set text ""

		# regexp {(.*)\:([0-9]+)\:([ ]*[.a-z]*[\:]*)([ ]*.*)}
		# regexp {(([a-zA-Z]:)?[^:]*)\:([0-9]+)\:([ ]*[.a-z]*[\:]*)([ ]*.*)} $errorLine a filename drive line type text
		regexp {\"?(([a-zA-Z]:)?[^:"]*)\"?(:|, line )([0-9]+)\:([ ]*[.a-z]*[\:]*)([ ]*.*)} \
                    $errorLine a filename a drive line type text

		lappend errorInfoList $filename $line $type $text
	}

	return $errorInfoList
}

