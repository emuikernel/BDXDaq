# Derived.psheet.tcl - generic property sheet for derived (object) files 
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01k,17may02,cjs  Fix spr 76286: mention that object size can be misleading
# 01j,22mar99,ren  Fixing dialog title.
# 01i,17mar99,ren  Changing label which desribes wrong thing.
# 01h,03mar99,ren  Adding Size tab and info.
# 01g,23oct98,cjs  Add static word 'Properties:' to title 
# 01f,16oct98,ren  re-did layout of derived object property sheet
# 01e,21sep98,cjs  added file size info 
# 01d,30jul98,cjs  upgraded for new file view server architecture 
# 01c,27jul98,jak  changed label to text widget for long filenames
# 01b,09jun98,cjs  added copyright, return value to dataOut()
# 01a,23apr98,cjs  written.
#
# DESCRIPTION
#
#############################################################

namespace eval ::DerivedPropertySheet {
	array set _ctrlList [list Size {} General {}]
	set leftMargin 11
	set indentedMargin 15
	set labelVspace 12
	set labelPos 46
	set _ctrlList(General) \
	    [list \
			 [list label -name lblpath -x $leftMargin -y 26 -w 70 -h 10 \
				  -title "&Derived object file:"] \
			 [list text -name txtpath -x $leftMargin -y 38 -width 210 -h 14 \
				  -readonly ] \
			 [list label -name lblstatus -x $indentedMargin \
				  -y [incr labelPos $labelVspace] -w 200 -h 10 -title ""] \
			 [list label -name lblattributes -x $indentedMargin \
				  -y [incr labelPos $labelVspace] -w 200 -h 10 -title ""] \
			 [list label -name lblsize -x $indentedMargin \
				  -y [incr labelPos $labelVspace] -w 200 -h 10 -title ""] \
			 [list label -name lblatime -x $indentedMargin \
				  -y [incr labelPos $labelVspace] -w 200 -h 10 -title ""] ]

	set _ctrlList(Size) \
		[list \
			 [list group -name group1 \
				  -title "Size of this Object Module (bytes)*" \
				  -x 7 -y 22 -w 202 -h 48] \
			 [list label -name lbl1 -title "Text" \
				  -x 15 -y 35 -w 21 -h 10] \
			 [list label -name lbl2 -title "Data" \
				  -x 99 -y 35 -w 21 -h 10] \
			 [list label -name lbl3 -title "Bss" \
				  -x 15 -y 48 -w 21 -h 10] \
			 [list label -name lbl5 -title "Total" \
				  -x 99 -y 48 -w 48 -h 10] \
			 [list text -name texttext -readonly \
				  -x 37 -y 35 -w 48 -h 10] \
			 [list text -name textbss -readonly \
				  -x 37 -y 48 -w 48 -h 10] \
			 [list text -name textdata -readonly \
				  -x 154 -y 35 -w 48 -h 10] \
			 [list text -name texttotal -readonly \
				  -x 154 -y 48 -w 48 -h 10] \
			 [list label -name warning \
                                  -title [concat "* For object modules, " \
                                      "size doesn't include common block " \
                                      "symbols, symbol table and other" \
                                      " dynamic allocations, and alignment " \
                                      "considerations.  These may " \
				      "substantially increase footprint of " \
				      "final image."] \
				  -x 7 -y 75 -w 195 -h 55] \
			]

	proc titleGet {handle sheet} {
		return $sheet
	}

	proc dateStampGet {handle} {
		return 980424
	}

	proc pageMatch {handle} {
		if {[string compare [::Object::evaluate $handle Type] \
			derivedObject] == 0} {
			variable _ctrlList
			return [list 1 [lsort [array names _ctrlList]]]
		} else {
			return 0
		}
	}

	proc OnShow {windowName sheet handle} {
		set fileName [::Object::evaluate $handle DisplayName]
		set caption "Properties: derived object '$fileName'"
		windowTitleSet $windowName $caption 

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
		foreach ctrl $_ctrlList($sheet) {
			append ctrl " -hidden"
			controlCreate $windowName $ctrl
		}
	}

	proc controlsDestroy {windowName sheet handle} {
		variable _ctrlList
		foreach ctrl $_ctrlList($sheet) {
			set name [::flagValueGet $ctrl -name]
			controlDestroy ${windowName}.$name
		}
	}

	proc parseSizeOutput { bufferSizeOutput } {
		lrange [lindex [split $bufferSizeOutput \n] 1] 0 3
	}

	proc getSizeInfo { strFullPath handle } {
		set theProjectObject [Object::evaluate $handle Project]
		set hProj [Object::evaluate $theProjectObject Handle]
		set buildSpec [::prjBuildCurrentGet $hProj]
		set tc [prjBuildTcGet $hProj $buildSpec]
		set bufferSizeOutput \
			[eval exec [${tc}::macroDefaultValGet SIZE] $strFullPath]
		parseSizeOutput $bufferSizeOutput
	}

	proc fillSheetSize { windowName handle } {
		::Workspace::statusMessagePush "Calculating image size..."
		set strFullPath [::Object::evaluate $handle Name]

		set sizeData [list {} {} {} {}]
		catch { set sizeData [getSizeInfo $strFullPath \
								  $handle]}

		
		controlValuesSet ${windowName}.texttext \
			[lindex $sizeData 0]
		controlValuesSet ${windowName}.textdata \
			[lindex $sizeData 1]
		controlValuesSet ${windowName}.textbss \
			[lindex $sizeData 2]
		controlValuesSet ${windowName}.texttotal \
			[lindex $sizeData 3]

		::Workspace::statusMessagePush "Calculating image size...done"		
	}

	proc dataIn {windowName handle sheet} {
		::beginWaitCursor
		if [catch { fillSheet$sheet $windowName $handle }] {
			global errorInfo
			puts $errorInfo
		}
		::endWaitCursor
	}			

	proc fillSheetGeneral { windowName handle } {
		set sheet General
		set fileName [::Object::evaluate $handle Name]
		set tmp $fileName
		filePathUnFix tmp

		# Set the full path of the file
		controlTextSet ${windowName}.txtpath $tmp

		# Set the file's status (built or not)
		if {[file exists $fileName]} {
			set status "Built"
		} else {
			set status "Not Built"
		}
		controlValuesSet ${windowName}.lblstatus "Status: $status"

		# Set the file's attributes, size and timestamp
		if {[file exists $fileName]} {
			set attributes "Attributes: [::FileLaunch::fileDescribe $fileName]"
			set size [::FileLaunch::fileSizeGet $fileName]
			set timestamp [::mTimeGet $fileName] 
		} else {
			set attributes ""
			set size ""
			set timestamp ""
		}
		controlValuesSet ${windowName}.lblattributes $attributes 
		controlValuesSet ${windowName}.lblsize $size 
		controlValuesSet ${windowName}.lblatime $timestamp 
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
		::DerivedPropertySheet} \
		errorMsg]} {
		messageBox $errorMsg
	}
}
