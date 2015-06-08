# Depend.psheet.tcl - generic property sheet for dependency (include) files 
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01f,23oct98,cjs  Add static word 'Properties:' to title 
# 01e,21sep98,cjs  added file size info 
# 01d,30jul98,cjs  upgraded for new file view server architecture 
# 01c,27jul98,jak  changed label to text widget for long filenames
# 01b,09jun98,cjs  added copyright, return value to dataOut()
# 01a,23apr98,cjs    written.
#
# DESCRIPTION
#
#############################################################

namespace eval ::DependPropertySheet {
	variable _ctrlList

	proc titleGet {handle sheet} {
		return "General"
	}

	proc dateStampGet {handle} {
		return 980424
	}

	proc pageMatch {handle} {
		if {[string compare [::Object::evaluate $handle Type] \
			dependObject] == 0} {
			return [list 1 [titleGet $handle ""]]
		} else {
			return 0
		}
	}

	proc OnShow {windowName sheet handle} {
		set fileName [::Object::evaluate $handle DisplayName]
		set caption "Properties: external dependency file '$fileName'"
		windowTitleSet $windowName $caption 

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
		set leftMargin 11
		set indentedMargin 15
		set labelVspace 12
		set labelPos 46
		set _ctrlList [list \
				[list label -name lblpath -x $leftMargin -y 26 -w 70 -h 10 \
				    -title "&Header file:"] \
        	    [list text -name txtpath -x $leftMargin -y 38 -width 210 -h 14 \
 				    -readonly ] \
				[list label -name lblattributes -x $indentedMargin \
				    -y [incr labelPos $labelVspace] -w 200 -h 10 -title ""] \
				[list label -name lblsize -x $indentedMargin \
					 -y [incr labelPos $labelVspace] -w 200 -h 10 -title ""] \
				[list label -name lblatime -x $indentedMargin \
					 -y [incr labelPos $labelVspace] -w 200 -h 10 -title ""] \
			]
		foreach ctrl $_ctrlList {
			controlCreate $windowName $ctrl
		}
	}

	proc controlsDestroy {windowName sheet handle} {
		variable _ctrlList
		foreach ctrl $_ctrlList {
			set name [::flagValueGet $ctrl -name]
			controlDestroy ${windowName}.$name
		}
	}

	proc dataIn {windowName handle sheet} {
		set fileName [::Object::evaluate $handle Name]

		# Set the full path of the file
		set tmp $fileName
		filePathUnFix tmp
		controlTextSet ${windowName}.txtpath $tmp

		controlTextSet ${windowName}.lblattributes \
			"Attributes: [::FileLaunch::fileDescribe $fileName]"

		controlTextSet ${windowName}.lblsize \
			[::FileLaunch::fileSizeGet $fileName]

		# Set the file's timestamp
		controlTextSet ${windowName}.lblatime [::mTimeGet $fileName]
	}

	proc dataOut {windowName handle sheet} {
		# Return success
		return 1
	}

	# This gets called as the final step to dataOut.
	proc dataCommit {handle} {
	}

	# This must be done last, to allow interface validation
	if {[catch {::Workspace::propPageRegister \
		::DependPropertySheet} \
		errorMsg]} {
		messageBox $errorMsg
	}
}
