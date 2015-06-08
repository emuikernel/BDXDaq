# Component.psheet.tcl - generic property sheet for components
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01z,31mar99,cjs  Fix update problem after param value set, and fix bad
#                  callback assignment for control listhelptopics
# 01y,02mar99,ren  Changing Help Topics to Help Links
# 01x,19feb99,ren Added handling of a parameter with no value
# 01w,15jan99,jmp  fixed on-line help when the HELP macro of a component
#                  contains several keyWords.
# 01v,23nov98,cjs  Remove extra brace causing obscure bug
# 01u,28oct98,cjs  Use NAME attribute if SYNOPSIS isn't defined 
# 01t,23oct98,cjs  Add static word 'Properties:' to title 
# 01s,30sep98,cjs  display component name without {}; make error desc.
#                  scrolling, multi-line 
# 01r,21sep98,cjs  display param synopses 
# 01q,21sep98,cjs  add vscroll bar to Synopsis; adjust width of help topics
# 01p,31aug98,cjs  fixed typo that prevented definition from showing up 
# 01o,25aug98,jmp  replaced helpLinkGet by windHelpLinkGet.
#		   fixed bug with [get INIT_RTN] return value.
# 01n,20aug98,jmp  added on-line help.
# 01m,11aug98,cjs  fixed bug causing param values not to update; changed
#                  value displayed for param w/ no value to '<no value>'
# 01l,06aug98,cjs  added 'General' sheet 
# 01k,30jul98,cjs  upgraded for new component view server architecture 
# 01j,17jul98,cjs  fixing dataOut bug 
# 01i,14jul98,cjs  implemented dataOut() 
# 01h,23jun98,cjs  updated to work with new, globally unique object IDs 
# 01g,18jun98,ms   updated for new wccParamGet syntax
# 01f,09jun98,cjs  added copyright, return value to dataOut()
# 01e,04may98,cjs  display properties, not just attributes 
# 01d,23apr98,cjs  various improvements 
# 01c,09apr98,cjs  changed function signatures for better support
#                  of multiple sheets 
# 01b,31mar98,cjs  got rid of unnecessary format() statements.
# 01a,27mar98,cjs  written.
#
# DESCRIPTION
# Property sheet for components.
#

namespace eval ::ComponentPropertySheet {
	variable _ctrlList
	variable _cDoc
	variable _currentPage
	variable _localParamValues
	variable _semaphore 0
	variable _helpLink()
	variable _strNoValue
	set _strNoValue "<no value>"

	proc titleGet {handle sheetId} {
		return "$sheetId"
	}

	proc dateStampGet {handle} {
		return 980409
	}

	proc pageMatch {handle} {
		variable _localParamValues

		if {[string match [::Object::evaluate $handle Type] \
			componentObject]} {

			# Do some one-time only initialization
			set prjName [::Object::evaluate \
				[::Object::evaluate $handle Project] Name]
			set hProj [::Object::evaluate \
				[::Object::evaluate $handle Project] Handle]
			set componentName [::Object::evaluate $handle Name]
			set ::ComponentPropertySheet::_cDoc \
				[::Workspace::cxrDocGet $prjName]

			# Determine if there are any params to display;
			set cSet [$::ComponentPropertySheet::_cDoc setCreate] 
			$cSet = $componentName
			set properties [$cSet get CFG_PARAMS]

			# If defPageShowRead is set in the registry, then show the
			# component definition page
			if {[::PSViewer::defPageShowRead]} {
				lappend sheets "Definition"
			}
			# If there are no properties, don't display a properties page
			if {[llength $properties]} {
				lappend sheets "Params"
			}
			if {[::ComponentViewServer::bError $hProj $componentName]} {
				lappend sheets "Errors"
			}

			lappend sheets "General"

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
		windowTitleSet $windowName "Properties: component '$name'"

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

		switch $sheet {
			"Params" {
				set _ctrlList($sheet) [list \
					[list text -name proptext -x 10 -y 22 -w 218 -h 14 \
						-title "<Param Value>" \
						-callback \
							[list ::ComponentPropertySheet::onEditPropValue \
							$windowName] \
					] \
					[list list -name proplist -x 10 -y 40 -w 218 -h 60 \
						-callback \
							[list ::ComponentPropertySheet::onCompParamValue \
							$windowName] \
					] \
					[list text -name propsynopsistext -readonly -x 10 -y 104 \
						-w 218 -h 20 -vscroll] \
				] \
			}
			"Definition" {
				set _ctrlList($sheet) [list \
					[list label -name deflbl -x 10 -y 20 -w 200 -h 14 \
						-title "This is a component"] \
					[list list -name deflist -x 10 -y 32 -w 80 -h 80 \
						-callback [list ::ComponentPropertySheet::onCompPropDef \
						$windowName]] \
					[list text -name defvaluelist \
						-readonly -multiline -x 100 -y 32 -w 126 -h 68] \
				]
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
			"General" {
				set _ctrlList($sheet) [list \
					[list label -name lblcompname -x 10 -y 30 -w 26 -h 12 \
						-title "Name"] \
					[list text -name txtcompname -x 58 -y 30  -w 150 -h 12 \
						-readonly] \
					[list label -name lblcompmacro -x 10 -y 44 -w 26 -h 12 \
						-title "Macro"] \
					[list text -name txtcompmacro -x 58 -y 44  -w 150 -h 12 \
						-readonly] \
					[list label -name lblcompsynopsis -x 10 -y 58 -w 38 -h 12 \
						-title "Synopsis"] \
					[list text -name txtcompsynopsis -x 58 -y 58  -w 150 -h 24 \
						-readonly -multiline -vscroll] \
					[list label -name lblcompstatus -x 10 -y 84 -w 26 -h 12 \
						-title "Status"] \
					[list text -name txtcompstatus -x 58 -y 84  -w 150 -h 12 \
						-readonly] \
					[list label -name lblhelptopics -x 10 -y 98 -w 56 -h 12 \
						-title "Help Links"] \
					[list list -name listhelptopics -x 58 -y 98 -w 150 -h 36 \
						-callback \
							[list ::ComponentPropertySheet::onHelpTopics \
							$windowName]] \
				]
			}
		}
		foreach ctrl $_ctrlList($sheet) {
			controlCreate $windowName $ctrl
		}
		OnHide $windowName $sheet $handle
	}

	proc onCompPropDef {windowName} {
		set handle [::Workspace::selectionGet]
		set componentName [::Object::evaluate $handle Name]
		set curSel [controlSelectionGet ${windowName}.deflist -string]
		if {$curSel != ""} {
			set cSet [$::ComponentPropertySheet::_cDoc setCreate] 
			$cSet = $componentName
			set values [$cSet get $curSel]
			if {$values == ""} {
				set values "N/A"
			}
			$cSet delete
			controlValuesSet ${windowName}.defvaluelist $values
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

	proc onCompParamValue {windowName} {
		variable _localParamValues
		variable _semaphore
		set curSel [controlSelectionGet ${windowName}.proplist -string]
		if {$curSel != ""} {
			set value $_localParamValues($curSel)
			variable _strNoValue
			if {$value == ""} {
				set value $_strNoValue
			}
			set _semaphore 1
			controlValuesSet ${windowName}.proptext $value
			set _semaphore 0

			# Set the synopsis
			set cSet [$::ComponentPropertySheet::_cDoc setCreate] 
			$cSet = $curSel
			set synopsis [$cSet get SYNOPSIS]
			if {$synopsis == ""} {
				set synopsis [$cSet get NAME]
			}
			$cSet delete
			controlTextSet ${windowName}.propsynopsistext $synopsis
		}
	}

	proc onHelpTopics {windowName} {
		variable _helpLink

		if {[controlEventGet ${windowName}.listhelptopics] != "dblclk"} {
			return
		}

		set helpTopic [controlSelectionGet ${windowName}.listhelptopics -string]

		windHelpDisplay $_helpLink($helpTopic)
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
		variable _helpLink

		::beginWaitCursor

		set prjName [::Object::evaluate \
			[::Object::evaluate $handle Project] Name]
		set hProj [::Workspace::projectHandleGet $prjName]
		set ::ComponentPropertySheet::_cDoc [::Workspace::cxrDocGet $prjName]
		set componentName [::Object::evaluate $handle Name]
		set wcc [::Object::evaluate $handle ComponentSet]
		set cSet [$::ComponentPropertySheet::_cDoc setCreate] 

		switch $sheet {
			"Definition" {
				# Set the label
				controlValuesSet ${windowName}.deflbl \
					"Attributes for component \
					[::Object::evaluate $handle DisplayName]"

				# Get all the attributes for the definition
				$cSet = $componentName
				set attributes [$cSet properties]
				foreach attribute $attributes {
					set val [$cSet get $attribute]
					if {$val != ""} {
						lappend attributeList $attribute
					}
				}
				controlValuesSet ${windowName}.deflist $attributeList
				controlSelectionSet ${windowName}.deflist 0
			}
			"Params" {
				catch {unset _localParamValues}
				variable _strNoValue

				# Get the params and values
				$cSet = $componentName
				set properties [lsort [$cSet get CFG_PARAMS]]

				# Fetch the initial values and cache them in an array
				foreach prop $properties {
					if {[::wccParamHasValue $hProj $wcc $prop]} {
						set val [::wccParamGet $hProj $wcc $prop]
					} else {
						# it doesn't have a value so fill with no value string
						set val $_strNoValue
					}
					set _localParamValues($prop) $val 
				}
				controlValuesSet ${windowName}.proplist $properties
				controlSelectionSet ${windowName}.proplist 0
			}
			"Errors" {
				set error [::ComponentViewServer::errorLookup \
					$hProj $componentName]
				set errorItem [lindex $error 0]
				$cSet = $errorItem
				set displayName \
					[lindex [::ComponentViewServer::idToName $cSet] 0]
				set errorMsg [lindex $error 1]
				
				controlTextSet ${windowName}.errorname $displayName
				controlTextSet ${windowName}.errortext $errorMsg
			}
			"General" {
				controlValuesSet ${windowName}.txtcompmacro $componentName

				$cSet = $componentName
				set displayName [$cSet get NAME] 
				controlValuesSet ${windowName}.txtcompname $displayName

				set synopsis [$cSet get SYNOPSIS]
				if {$synopsis == ""} {
					set synopsis "(info not available for this component)"
				}
				controlValuesSet ${windowName}.txtcompsynopsis $synopsis

				set componentList [::wccComponentListGet \
					$hProj [::wccCurrentGet $hProj]]
				$cSet = [$cSet & $componentList] 
				if {[$cSet instances Component] != ""} {
					set status "this component is included"
				} else {
					$cSet = $componentName
					set unavailable \
						[ComponentViewServer::unavailableListGet $hProj]
					$cSet = [$cSet & $unavailable] 
					if {[$cSet instances Component] != ""} {
						set status "this component is not installed"
					} else {
						set status "this component is not included"
					}
				}
				controlValuesSet ${windowName}.txtcompstatus $status

				# Fill the Topics list
				$cSet = $componentName
				set topicList ""

				# Add help attribute to the Topics list
				set helpAttr [$cSet get HELP]
				foreach attr $helpAttr {
					if {$attr != "" && \
						[lsearch $topicList $attr] == -1} {
						set helpLink [windHelpLinkGet -name $attr]
						if {$helpLink != ""} {
							lappend topicList $attr
							set _helpLink($attr) $helpLink
				}	}	}

				# Add library (got from module names) to the Topics list
				set modules [$cSet get MODULES]
				foreach module $modules {
					set topicName [file rootname $module]
					if {[lsearch $topicList $topicName] == -1} {
						set helpLink [windHelpLinkGet -name $topicName]
						if {$helpLink != ""} {
							lappend topicList $topicName
							set _helpLink($topicName) $helpLink
				}	}	}

				# Add initialization routine to the Topics list
				set intRtn [$cSet get INIT_RTN]
				if {$intRtn != ""} {
				    set topicName [lindex [split $intRtn] 0]
					if {[lsearch $topicList $topicName] == -1} {
						set helpLink [windHelpLinkGet -name $topicName]
						if {$helpLink != ""} {
							lappend topicList $topicName
							set _helpLink($topicName) $helpLink
				}	}	}

				controlValuesSet ${windowName}.listhelptopics $topicList
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
				variable _strNoValue
				foreach prop [array names _localParamValues] {
					if {[catch {
						set trimmedParamValue [string trim $_localParamValues($prop)]
						# clear the value if it's empty or has <no value> in it
						if { $trimmedParamValue == "" || \
								 [string tolower $trimmedParamValue] == $_strNoValue } {
							::wccParamClearValue $hProj $wcc $prop
						} else {
							# just set the value
							::wccParamSet $hProj $wcc $prop $trimmedParamValue
						}
					} error]} {
						controlSelectionSet ${windowName}.proplist -string $prop
						messageBox -ok $error
						set retcode 0
					}
				}
			}
			"Errors" {
				::ComponentViewServer::errorsUpdate $handle
			}
			"General" {
			}
		}
		
		::endWaitCursor
		return $retcode 
	}

	# This gets called as the final step to dataOut.
	proc dataCommit {handle} {
	}

	# This must be done last, to allow interface validation
	if {[catch {::Workspace::propPageRegister \
		::ComponentPropertySheet} \
		errorMsg]} {
		messageBox $errorMsg
	}
}
