# Workspace.psheet.tcl - property sheet for component sets 
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,10mar99,cjs  written. 
#
# DESCRIPTION
# Property sheet for workspaces.
#

namespace eval ::WorkspacePropertySheet {
	variable _ctrlList
	variable _currentPage

	proc titleGet {handle sheetId} {
		return "$sheetId"
	}

	proc dateStampGet {handle} {
		return 990410
	}

	proc pageMatch {handle} {
		set type [::Object::evaluate $handle Type]
		if {[string match $type workspaceObject]} {
			set retval [list 1 {General}]
		} else {
			set retval 0
		}
		return $retval
	}

	proc OnShow {windowName sheet handle} {
		set name [::Object::evaluate $handle Name]
		set displayName [::Object::evaluate $handle DisplayName]
		windowTitleSet $windowName "Properties: workspace '$displayName'"

		variable _ctrlList
		foreach ctrl $_ctrlList($sheet) {
			controlHide [format "%s.%s" $windowName \
				[::flagValueGet $ctrl -name]] 0
		}
	}

	proc OnHide {windowName sheet handle} {
		variable _ctrlList
		foreach ctrl $_ctrlList($sheet) {
			controlHide [format "%s.%s" $windowName \
				[::flagValueGet $ctrl -name]] 1
		}
	}

	proc controlsCreate {windowName sheet handle} {
		variable _ctrlList
		variable _currentPage

		set hProj [::Object::evaluate $handle Handle] 
		switch $sheet {
			"General" {
				set _ctrlList($sheet) [list \
					[list label -name lblprojfile \
						-title "File:" \
						-x 10 -y 35 -w 30 -h 14] \
	    			[list text -name textprojfile -multiline \
	    				-readonly -x 32 -y 35 -w 194 -h 14] \
				]
			}
		}
		foreach ctrl $_ctrlList($sheet) {
			controlCreate $windowName $ctrl
		}
		OnHide $windowName $sheet $handle
	}

	proc controlsDestroy {windowName sheet handle} {
		variable _ctrlList
		foreach ctrl $_ctrlList($sheet) {
			set name [::flagValueGet $ctrl -name]
			controlDestroy ${windowName}.${name}
		}
	}

	proc dataIn {windowName handle sheet} {
		::beginWaitCursor
		switch $sheet {
			"General" {
				set hProj [::Object::evaluate $handle Handle]
				set prjFile [::prjInfoGet $hProj fileName]
				filePathUnFix prjFile
				controlTextSet ${windowName}.textprojfile $prjFile
			}
		}
		::endWaitCursor
	}

	proc dataOut {windowName handle sheet} {
		set retval 1
		switch $sheet {
			default {}
		}

		# Success
		return $retval 
	}

	proc dataCommit {handle} {

	}

	# This must be done last, to allow interface validation
	if {[catch {::Workspace::propPageRegister \
		::WorkspacePropertySheet} \
		errorMsg]} {
		messageBox $errorMsg
	}
}
