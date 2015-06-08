# nonGuiHost.tcl - tcl default host commands bindings
#
# Copyright 1994-1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01c,30sep98,jmp  removed useless browseWebPage{} routine, no longer set
#                  netNavigator here.
# 01b,07sep98,dbt  fixed runTerminal call in editFile routine.
# 01a,03sep98,p_m  Fixed SPR 22293 by using "ftp.wrs.com" instead of an ip 
#                  address

# Global definitions

set installTapeCmd	installOption
set wrsFtpCmd		"ftp ftp.wrs.com"
set mailCmd		/bin/mail
set viewer		more
set installCDCmd	/cdrom/SETUP
set remoteShellCmd      to_be_overwritten_with_host_specific_cmd
set processListCmd	to_be_overwritten_with_host_specific_cmd
set printCmd		to_be_overwritten_with_host_specific_cmd
set terminal		to_be_overwritten_with_host_specific_cmd
if [info exists env(EDITOR)] {set textEditor $env(EDITOR)} {set textEditor vi}

# source host specific definitions, overwritting some of the above

source [wtxPath host resource tcl app-config all][wtxHostType].tcl

# common routines, including:
#
# runTerminal - editFile -viewFile - browseWebPage 
# topLevelErrorHandler - tornadoVersion - mailSend

##############################################################################
#
# runTerminal - start an xterm (or xterm-like) terminal session
#
# PARAMETERS
#   title      - the title to display in the window titlebar
#   background - nonzero if the session should be started in the background
#   args       - the program to run in the terminal and its arguments.
#
#
# EXAMPLE
#
#   runTerminal "Edit" 1 vi /etc/passwd
#
# will bring up an xterm with a vi session on /etc/passwd in the background.
#


proc runTerminal {title background args} { 
    global terminal
    global env

    # Since we're about to start an xterm, put the user's original
    # XFILESEARCHPATH in place if one exists.

    if [info exists env(XFILESEARCHPATH)] {
	set xfsp $env(XFILESEARCHPATH)
    }

    if [info exists env(USER_XFILESEARCHPATH)] {
        set env(XFILESEARCHPATH) $env(USER_XFILESEARCHPATH)
    } {
	if [info exists env(XFILESEARCHPATH)] {
	    unset env(XFILESEARCHPATH)
	}
    }

    set cmdLine [concat exec $terminal]
    if {[string match "*shelltool*" [lindex $terminal 0] ]} {
	# shelltool uses -I to introduce command, and -Wl
	# for the title.
	lappend cmdLine -Wl $title -I
	# shelltool differs from the others in that what
	# trails -I is supposed to be all the arguments as 
	# one word.
	lappend cmdLine $args
    } { 
	if {! [string match "*cmdtool*" [lindex $terminal 0]]} {
	    # if it's not shelltool or cmdtool, assume it's
	    # xterm-like and uses -e to introduce commands
	    # and -T to specify the title.

	    lappend cmdLine -T $title -e
	    set cmdLine [concat $cmdLine $args]
	} {
	    # cmdtool uses no argument for the commands, and -Wl to 
	    # specify the title.
	    lappend cmdLine -Wl $title 
	    set cmdLine [concat $cmdLine $args]
	}
    }

    if {$background} {
 	lappend cmdLine "&"
    }

    eval $cmdLine

    if [info exists xfsp] {
	set env(XFILESEARCHPATH) $xfsp
    }
    return
}

##############################################################################
#
# editFile - edit a file, honoring the user's preferences
#
# USAGE
#     editFile [-t.title] [-b.ackground] filename [lineNumber]
#
# PARAMETERS
#     -title		- specify the text for the window titlebar
#     -background	- do not wait for the user to finish editing
#     filename		- path to file to edit
#     lineNumber	- line number to begin editing at
#     
# If the user's favorite text editor is not emacsclient, an terminal
# is started via runTerminal running the editor.  If emacsclient is 
# preferred, no terminal is started, just the emacsclient session.
# HP vuepad is also recognized; this does not get an xterm window,
# and does not support jumping to a line number.
#
# Unless -background is specified, the caller will block until the 
# user completes the editing session.
#

proc editFile {args} {
    global terminal
    global textEditor

    set bgnd 0
    set file ""
    set line ""
    set title ""
    set nArgs [llength $args]

    for {set ix 0} {$ix < $nArgs} {incr ix} { 
	switch -glob -- [lindex $args $ix] {
	    -b* {
	        set bgnd 1
	    }
	    -t* {
	        if {$ix < $nArgs - 1} {
		    incr ix
		    set title [lindex $args $ix]
		} {
		    error "missing title"
		}
	    }
	    default {
		if {$file == ""} {
		    set file [lindex $args $ix]
		} {
		    if {$line == ""} {
			set line [lindex $args $ix]
		    } {
			error "extra arguments"
		    }
		}
	    }
	}
    }

    if {$file == ""} {
	error "no file specified"
    }

    switch -glob $textEditor {
	*emacscli* {
	    set cmdList [list exec $textEditor]
	    if {$line != ""} {
		lappend cmdList +$line
	    }
	    lappend cmdList $file
	    if {$bgnd} {
		lappend cmdList &
	    }
	    eval $cmdList
	}

	*vuepad* {
	    set cmdList [list exec $textEditor $file]
	    if {$bgnd} {
		lappend cmdList &
	    }
	    eval $cmdList
	}

	*dtpad* {
	    set cmdList [list exec $textEditor $file]
	    if {$bgnd} {
		lappend cmdList &
	    }
	    eval $cmdList
	}

	default {
	    if {$title == ""} {
		set title "Edit $file"
	    }

	    if {$line == ""} {
		eval runTerminal {$title} $bgnd $textEditor $file
	    } {
		eval runTerminal {$title} $bgnd $textEditor +$line $file
	    }
	}
    }
}

##############################################################################
#
# viewFile - start a terminal containing a file viewing session
#
# A terminal is started with the `viewer' program, in the background.  Shwait
# is used to make sure the user can see the whole file.

proc viewFile {title file} { 
    global viewer
    runTerminal $title 1 shwait $viewer $file
}

##############################################################################
#
# mailSend - send fileName to "to" and "cc" addressees.
#

proc mailSend {to cc fileName} {
    global mailCmd
 
    # Temporary change the mail command to add a subject line for 
    # non /bin/mail users 

    set oldMail $mailCmd
    if {[file tail $mailCmd] != "mail"} {
        set mailCmd "$mailCmd -s \"[lreplace [exec head -1 $fileName] 0 0]\""
    }
 
    # send the file to all specified addressees

    lappend cc $to
    foreach elt $cc {
        if [catch {eval exec $mailCmd $elt < $fileName} retcall] {
            noticePost error "Error when sending mail: $retcall"
        }
    }
 
    # reset the mailCmd to its previous value

    set mailCmd $oldMail
}
