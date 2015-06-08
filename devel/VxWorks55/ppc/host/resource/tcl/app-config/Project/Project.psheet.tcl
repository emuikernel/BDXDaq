# Project.psheet.tcl - property sheet for component sets 
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01l,01jul02,rbl  fix SPR 79059, data and bss fields reversed.
# 01k,16mar99,cjs  Fixing bug in size calculation; protecting from projects
#                  with missing toolchains
# 01j,12mar99,rbl  making "description" text box into -wantenter
# 01i,02mar99,ren  Slightly modified description field
# 01h,02feb99,rbl  Changes to handle missing or read-only projects
# 01g,11nov98,cjs  replace statusMessagePop() with ...done 
# 01f,23oct98,cjs  Add static word 'Properties:' to title 
# 01e,15oct98,cjs  changed misleading size info to indicate exactly what's
#                  being measured 
# 01d,30sep98,cjs  put error status in titlebar; reorganized presentation;
#                  added field to accept/display user comments
# 01c,21aug98,cjs  add display of project error status 
# 01b,13aug98,cjs  changed title to reflect project, not comp set;
#                  added project file path
# 01a,01aug98,cjs  written. 
#
# DESCRIPTION
# Property sheet for projects.  Shows image size for projects
# with one or more component sets
#

namespace eval ::ProjectPropertySheet {
	variable _ctrlList
	variable _currentPage

	proc titleGet {handle sheetId} {
		return "$sheetId"
	}

	proc dateStampGet {handle} {
		return 980921
	}

	proc pageMatch {handle} {
		set type [::Object::evaluate $handle Type]
		if {[string match $type projectObject] || \
			[string match $type deadProjectObject]} {
			set hProj [::Object::evaluate $handle Handle]
			if {[::wccCurrentGet $hProj] != ""} {
				set retval [list 1 {Size General}]
			} else {
				set retval [list 1 {General}]
			}
		} else {
			set retval 0
		}
		return $retval
	}

	proc OnShow {windowName sheet handle} {
		set name [::Object::evaluate $handle Name]
		set displayName [::Object::evaluate $handle DisplayName]
		if {[::Workspace::projectErrorStateGet $name]} {
			windowTitleSet $windowName \
				"Properties: project '$displayName' (configuration errors)"
		} else {
			windowTitleSet $windowName "Properties: project '$displayName'"
		}

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
			"Size" {
				set _ctrlList($sheet) [list \
    				[list group -name group1 \
						-title "Size of included components, bytes*" \
						-x 7 -y 22 -w 202 -h 68] \
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
    				[list label -name lbl6 \
						-title "* excluding BSP and application code" \
    					-x 15 -y 74 -w 180 -h 10] \
				]
			}
			"General" {
				set _ctrlList($sheet) [list \
					[list label -name lblprojfile \
						-title "File:" \
						-x 10 -y 20 -w 30 -h 14] \
	    			[list text -name textprojfile \
	    				-readonly -x 32 -y 20 -w 194 -h 14] \
	    			[list text -name textprojerror -multiline \
	    				-readonly -x 10 -y 38 -w 217 -h 20] \
	    			[list group -name grpprojinfo \
						-title "Description" \
	    				-x 10 -y 60 -w 217 -h 70] \
	    			[list text -name textprojinfo -multiline \
	    				-readonly -x 14 -y 70 -w 208 -h 20 \
						-multiline -vscroll] \
					[list text -name textcomment -x 14 -y 94 \
						-w 208 -h 30 -multiline -vscroll -wantenter \
						-callback ::ProjectPropertySheet::onEditComment] \
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
			"Size" {
				# Get the current object's project, then
				# set the size values for that project's current
				# config 
			
				::Workspace::statusMessagePush "Calculating image size..."
				set projObj [::Object::evaluate $handle Project]
				set projName [::Object::evaluate $projObj Name]
				set hProj [::Object::evaluate $projObj Handle]
				set build [::prjBuildCurrentGet $hProj]
				set tc [::prjBuildTcGet $hProj $build]
				if {[::Workspace::isToolchainInstalled $tc]} {
					if {[catch {::Workspace::currentCSetSizeGet $projName} \
						sizeData]} {
						set sizeData {n/a n/a n/a}
					}
				} else {
					set sizeData {n/a n/a n/a}
				}
				controlValuesSet ${windowName}.texttext \
					[lindex $sizeData 0]
				controlValuesSet ${windowName}.textdata \
					[lindex $sizeData 1]
				controlValuesSet ${windowName}.textbss \
					[lindex $sizeData 2]
				if {[::Workspace::isToolchainInstalled $tc] && \
					[lsearch $sizeData "n/a"] == -1} {
					set total [expr [lindex $sizeData 0] \
						+ [lindex $sizeData 1] \
						+ [lindex $sizeData 2]]
				} else {
					set total n/a
				}
				controlValuesSet ${windowName}.texttotal $total 
				::Workspace::statusMessagePush "Calculating image size...done"
			}
			"General" {
				set hProj [::Object::evaluate $handle Handle]
				set prjFile [::prjInfoGet $hProj fileName]
				filePathUnFix prjFile
				controlTextSet ${windowName}.textprojfile $prjFile

				set objectType [::Object::evaluate $handle Type]
				if {$objectType != "deadProjectObject"} {
					set prjType [::prjTypeGet $hProj]
					set prjDesc [${prjType}::name]
					regsub {^Create[ a]*} $prjDesc {Type: } prjDesc
				} else {
					set prjDesc "Could not load Project file $prjFile."
				}

				controlValuesSet ${windowName}.textprojinfo $prjDesc

				# XXX VxWorks pane name hardcoded below
				if {[::prjHasComponents $hProj]} {
					if {[::wccErrorStatusGet $hProj]} {
						controlTextSet ${windowName}.textprojerror \
							"Warning! This project has configuration errors. \
							Fix them by following the highlighted components \
							in the 'VxWorks' pane" 
					} else {
						controlTextSet ${windowName}.textprojerror \
							"No component configuration errors."
					}
				} elseif {$objectType == "deadProjectObject"} {
					controlTextSet ${windowName}.textprojerror \
						"Status: Project File Missing."
				} else {
					controlTextSet ${windowName}.textprojerror \
						"Status: OK."
				}

				# Get user comments
				set comments [::prjTagDataGet $hProj userComments]
				controlTextSet ${windowName}.textcomment $comments
			}
		}
		::endWaitCursor
	}

	proc dataOut {windowName handle sheet} {
		set retval 1
		switch $sheet {
			"General" {
				# Get user comments
				set hProj [::Object::evaluate \
					[::Object::evaluate $handle Project] Handle]
				set comments [controlTextGet ${windowName}.textcomment]
				::prjTagDataSet $hProj userComments $comments
			}
			default {}
		}

		# Success
		return $retval 
	}

	proc dataCommit {handle} {

	}

	proc onEditComment {} {
		::PSViewer::dirtyFlagSet 1
	}

	# This must be done last, to allow interface validation
	if {[catch {::Workspace::propPageRegister \
		::ProjectPropertySheet} \
		errorMsg]} {
		messageBox $errorMsg
	}
}
