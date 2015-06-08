# ComponentFolder.psheet.tcl - generic property sheet for components
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01r,28oct98,cjs  Use NAME attribute if SYNOPSIS isn't defined 
# 01q,23oct98,cjs  Add static word 'Properties:' to title 
# 01p,30sep98,cjs  display component name without {}; make error desc.
#                  scrolling, multi-line 
# 01o,21sep98,cjs  display param synopses 
# 01n,11aug98,cjs  changed value displayed for param w/ no value to
#                  '<no value>'
# 01m,30jul98,cjs  upgrade for new component server architecture
# 01e,16jul98,cjs  make params writable 
# 01d,23jun98,cjs  updated to work with new, globally unique object IDs 
# 01c,18jun98,ms   updated for new wccParamGet syntax
# 01b,09jun98,cjs  added copyright, return value to dataOut()
# 01a,27may98,cjs  written.
#
# DESCRIPTION
#

namespace eval ::ComponentFolderPropertySheet {
	variable _ctrlList
	variable _cDoc
	variable _currentPage
	variable _localParamValues
	variable _semaphore 0

	proc titleGet {handle sheetId} {
		return "$sheetId"
	}

	proc dateStampGet {handle} {
		return 980527
	}

	proc pageMatch {handle} {

		if {![string compare [::Object::evaluate $handle Type] \
			componentFolderObject]} {

			# Do some one-time only initialization
			set prjObj [::Object::evaluate $handle Project]
			set hProj [::Object::evaluate $prjObj Handle]
			set prjName [::Object::evaluate $prjObj Name]
			set wcc [::Object::evaluate $handle ComponentSet]
			set folderName [::Object::evaluate $handle Name]
			set ::ComponentFolderPropertySheet::_cDoc \
				[::Workspace::cxrDocGet $prjName]

			# Determine all the folder's children that exist in the config,
			# then lump all their params together.  Use this to determine
			# if we need a params sheet
			set cSet [$::ComponentFolderPropertySheet::_cDoc setCreate] 
			$cSet = $folderName
			$cSet = [::cxrSubfolders $cSet]
			$cSet = [$cSet & [::wccComponentListGet $hProj $wcc]]
			set properties [$cSet get CFG_PARAMS]

			if {[llength $properties]} {
				lappend sheets Params
			}
			if {[::ComponentViewServer::bError $hProj $folderName]} {
				lappend sheets Errors
			}
			if {[info exists sheets]} {
				set retval [list 1 $sheets]
			} else {
				set retval 0
			}
			$cSet delete
		} else {
			set retval 0
		}
		return $retval 
	}

	proc OnShow {windowName sheet handle} {
		set name [::Object::evaluate $handle DisplayName]
		windowTitleSet $windowName "Properties: component folder '$name'"

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

		set ns ComponentFolderPropertySheet
		switch $sheet {
			"Params" {
				set _ctrlList($sheet) [list \
					[list text -name proptext -x 10 -y 22 -w 218 -h 14 \
						-title "<Param Value>" \
                        -callback \
                            [list ${ns}::onEditPropValue $windowName] \
					] \
					[list list -name proplist -x 10 -y 40 -w 218 -h 72 \
						-callback \
							[list ${ns}::onFolderParamValue $windowName] \
					] \
					[list text -name propsynopsistext -readonly -x 10 -y 104 \
						-w 218 -h 20 -vscroll] \
				] \
			}
			"Errors" {
				set _ctrlList($sheet) [list \
					[list label -name errorname -x 10 -y 20 -w 200 -h 20 \
						-title "please wait..."] \
					[list group -name errorgroup -x 10 -y 38 \
						-w 200 -h 62 -title "Problem"] \
					[list text -name errortext -readonly \
						-multiline -vscroll -x 18 -y 50 -w 182 -h 45] \
				]
			}
		}
		foreach ctrl $_ctrlList($sheet) {
			controlCreate $windowName $ctrl
		}
		OnHide $windowName $sheet $handle
	}

	proc onFolderParamValue {windowName} {
		variable _localParamValues
		variable _semaphore

		set curSel [controlSelectionGet ${windowName}.proplist -string]
		if {$curSel != ""} {
			set value $_localParamValues($curSel)
			if {$value == ""} {
				set value "<no value>"
			}
			set _semaphore 1
			controlValuesSet ${windowName}.proptext $value
			set _semaphore 0

			# Set the synopsis
			set cSet [$::ComponentFolderPropertySheet::_cDoc setCreate] 
			$cSet = $curSel
			set synopsis [$cSet get SYNOPSIS]
			if {$synopsis == ""} {
				set synopsis [$cSet get NAME]
			}
			$cSet delete
			controlTextSet ${windowName}.propsynopsistext $synopsis
		}
	}

	proc onEditPropValue {windowName} {
		variable _localParamValues
		variable _semaphore

		# Set the dirty flag only if the user has touched the edit
		# control; otherwise, it's just a selection change
		if {$_semaphore == 0} {
			::PSViewer::dirtyFlagSet 1
		}

		set curSel [controlSelectionGet ${windowName}.proplist -string]
		if {$curSel != ""} {
			set _localParamValues($curSel) \
				[controlTextGet ${windowName}.proptext]
		} else {
			messageBeep -ok
		}
	}

	proc controlsDestroy {windowName sheet handle} {
		variable _ctrlList
		foreach ctrl $_ctrlList($sheet) {
			set name [::flagValueGet $ctrl -name]
			controlDestroy ${windowName}.${name}
		}
	}

	proc dataIn {windowName handle sheet} {
		variable _localParamValues

		::beginWaitCursor

		set prjObj [::Object::evaluate $handle Project]
		set hProj [::Object::evaluate $prjObj Handle]
		set prjName [::Object::evaluate $prjObj Name]
		set folderName [::Object::evaluate $handle Name]
		set wcc [::Object::evaluate $handle ComponentSet]
		set ::ComponentFolderPropertySheet::_cDoc \
			[::Workspace::cxrDocGet $prjName]
		set cSet [$::ComponentFolderPropertySheet::_cDoc setCreate] 

		switch $sheet {
			"Params" {
				catch {unset _localParamValues}

				# Determine all the folder's children that exist in the config,
				# then lump all their params together.
				$cSet = $folderName
				$cSet = [::cxrSubfolders $cSet]
				$cSet = [$cSet & [::wccComponentListGet $hProj $wcc]]
				set properties [lsort [$cSet get CFG_PARAMS]]

				# Cache the initial values an array
				foreach prop $properties {
					set val [::wccParamGet $hProj $wcc $prop] 
					set _localParamValues($prop) $val
				}

				# Populate the list w/ the param names, and set
				# a selection, which triggers an update of the edit field
				controlValuesSet ${windowName}.proplist $properties
				controlSelectionSet ${windowName}.proplist 0
			}
			"Errors" {
				set error [::ComponentViewServer::errorLookup \
					$hProj $folderName]
				set errorItem [lindex $error 0]
				$cSet = $errorItem
				set displayName \
					[lindex [::ComponentViewServer::idToName $cSet] 0]
				set errorMsg [lindex $error 1]
				
				controlTextSet ${windowName}.errorname $displayName
				controlTextSet ${windowName}.errortext $errorMsg
			}
		}

		::endWaitCursor

		$cSet delete
	}

	proc dataOut {windowName handle sheet} {
		variable _localParamValues
		set retcode 1

		::beginWaitCursor

		set prjName [::Object::evaluate \
			[::Object::evaluate $handle Project] Name]
		set hProj [::Workspace::projectHandleGet $prjName]
		set wcc [::Object::evaluate $handle ComponentSet]

		switch $sheet {
			"Params" {
				foreach prop [array names _localParamValues] {
			
					if {[catch {::wccParamSet $hProj $wcc $prop \
						$_localParamValues($prop)} error]} {
						controlSelectionSet ${windowName}.proplist -string $prop
						messageBox -ok $error
						set retcode 0
					}
				}
			}
			"Errors" {
				::ComponentViewServer::errorsUpdate $handle
			}
			default {}
		}

		::endWaitCursor
		return $retcode 
	}

	# This gets called as the final step to dataOut.
	proc dataCommit {handle} {
	}

	# This must be done last, to allow interface validation
	if {[catch {::Workspace::propPageRegister \
		::ComponentFolderPropertySheet} \
		errorMsg]} {
		messageBox $errorMsg
	}
}
