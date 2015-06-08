###############################################################################
#
# app-config/Tornado/01VersionControl.win32.tcl - support routines for 
#			 version control
#
# Copyright (C) 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,09sep98,jak restructuring for UNIX
#				  moved configuration out of here 
# 01a,04jun98,jak created 
#

# DESCRIPTION
# 	This script contains the routines to launch version control commands
# When the command is launched a UI appears for comments to be entered.
#


#///////////////////////////////////
# global procs 

#
# vcIsConfigured - returns whether the ci & co commands have been chosen
#
proc vcIsConfigured {} {
	set tmpVal [appRegistryEntryRead VersionControl checkinCmd]
	if {($tmpVal == "") || ($tmpVal == "<undefined>")} { return 0}
	set tmpVal [appRegistryEntryRead VersionControl checkoutCmd]
	if {($tmpVal == "") || ($tmpVal == "<undefined>")} { return 0}
	return 1
}

#
# vcCmdLaunch - performs macro substitution, then passes cmd to runCmd
#
proc vcCmdLaunch {cmd comment fullFilePath} {
	set cmdLine "command"
	set windowTitle "title" 

	set fileName [file tail $fullFilePath]
	set workingDir [file dirname $fullFilePath]

	if { $cmd == "checkin" } {
		set cmdLine [appRegistryEntryRead VersionControl checkinCmd]
		set windowTitle "checkin $fileName"
	}

	if { $cmd == "checkout" } {
		set cmdLine [appRegistryEntryRead VersionControl checkoutCmd]
		set windowTitle "checkout $fileName"
	}

	# substitute filename and comment macros (multi-word comments) 
	regsub -all {\$comment} $cmdLine "\"$comment\"" cmdLine 
	regsub -all {\$filename} $cmdLine "$fileName" cmdLine 

	if {[catch {ExtCmdInterface::runCmd tool \
				$cmdLine $workingDir $windowTitle 1} err]} {
		messageBox "error: '$err'"
	}
}

#
# vcUIShow - displays a dialog for the comment for the vc command 
#
proc vcUIShow {command fullFilePath} {
    set fileName [file tail $fullFilePath]
    dialogCreate \
        -name IDD_VC_DLG \
        -title "$command $fileName" \
        -width 283 -height 47 \
        -init "initVcUI $command $fullFilePath" \
        -controls { \
             { button -name IDOK -default \
                        -xpos 192 -ypos 26 -width 40 -height 14  \
             }
             { button -title "Cancel" -name IDCANCEL  \
            -xpos 241 -ypos 26 -width 35 -height 14  \
            -callback onVcUICancel \
             }
             { label -title "Comment" -name IDC_STATIC  \
                      -xpos 7 -ypos 7 -width 39 -height 14  \
             }
             { text -name IDC_EDIT1  \
             -xpos 55 -ypos 7 -width 221 -height 14  \
             }
        }
}

#
# vcUIShow - displays a dialog for the comment for the vc command 
#
proc initVcUI {command fullFilePath} {
    controlTextSet IDD_VC_DLG.IDOK $command
    controlCallbackSet IDD_VC_DLG.IDOK "onVcUIOk $command $fullFilePath"
}

proc onVcUIOk {command fullFilePath} {
    set comment [controlTextGet  IDD_VC_DLG.IDC_EDIT1]
    windowClose IDD_VC_DLG
    vcCmdLaunch $command $comment $fullFilePath
}
proc onVcUICancel {} {
    windowClose IDD_VC_DLG
}
