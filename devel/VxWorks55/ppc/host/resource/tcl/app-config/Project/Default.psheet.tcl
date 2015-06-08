# Default.psheet.tcl - default property page -- matches anything 
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01c,23oct98,cjs  Add static word 'Properties:' to title 
# 01b,09jun98,cjs  added copyright, return value to dataOut()
# 01a,09apr98,cjs    created. 
#
# DESCRIPTION
# This sheet serves by default when no other sheet volunteers.
#

namespace eval ::PropertyPageDefault {
	variable _ctrlList

	proc titleGet {handle sheetId} {
		return "<none>"
	}

	proc dateStampGet {handle} {
		return 900409
	}

	proc pageMatch {handle} {
		return [list 1 <cookie>]
	}

	proc OnShow {windowName sheet handle} {
		set name [::Object::evaluate $handle DisplayName]
		windowTitleSet $windowName "Properties: object '$name'"

		variable _ctrlList
		foreach ctrl $_ctrlList {
			controlHide [format "%s.%s" $windowName \
				[::flagValueGet $ctrl -name]] 0
		}
	}

	proc OnHide {windowName sheet handle} {
		variable _ctrlList
		foreach ctrl $_ctrlList {
			controlHide [format "%s.%s" $windowName \
				[::flagValueGet $ctrl -name]] 1
		}
	}

	proc controlsCreate {windowName sheet handle} {
		variable _ctrlList
		set _ctrlList {
			{label -name lbl1 -x 5 -y 20 -w 150 -h 14 \
				-title "No displayable properties"}
		}
		foreach ctrl $_ctrlList {
			controlCreate $windowName $ctrl
		}
	}

	proc controlsDestroy {windowName sheet handle} {
		variable _ctrlList
		foreach ctrl $_ctrlList {
			set name [::flagValueGet $ctrl -name]
			controlDestroy ${windowName}.${name}
		}
	}

	proc dataIn {windowName handle sheet} {
	}

	proc dataOut {windowName handle sheet} {
		# Success
		return 1
	}

	# This must be done last, to allow interface validation
	::Workspace::propPageRegister ::PropertyPageDefault
}
