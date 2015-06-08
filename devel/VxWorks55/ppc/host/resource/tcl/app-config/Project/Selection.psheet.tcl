# Selection.psheet.tcl - generic property sheet for components
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01x,18jun02,rbl  fix TCL error when there is an error in a selection
# 01w,15may02,rbl  fixing SPR 77280, "Project facility messes up
#                  dependencies on a selection  e.g. can't change WDB
#                  connection"
# 01v,17mar99,jmp  fixed on-line help for Selection properties tab (SPR# 25681).
# 01u,15jan99,jmp  fixed on-line help when the HELP macro of a component
#                  contains several keyWords.
# 01t,28oct98,cjs  Use NAME attribute if SYNOPSIS isn't defined 
# 01s,23oct98,cjs  Add static word 'Properties:' to title 
# 01r,30sep98,cjs  display component name without {}; make error desc.
#                  scrolling, multi-line 
# 01q,21sep98,cjs  display param synopses 
# 01p,21sep98,cjs  add vscroll bar to Synopsis; adjust width of help topics
# 01o,31aug98,cjs  make "General" sheet visible 
# 01n,11aug98,cjs  changed value displayed for param w/ no value to
#                  '<no value>'
# 01m,30jul98,cjs  add radio button behavior for cardinality 1-1;
#                  upgrade for new component server architecture
# 01l,17jul98,cjs  implement dataOut()
# 01k,14jul98,cjs  adapt to new dependency dialog
# 01j,13jul98,cjs  adapted to new dependency dialog and idToName()
# 01i,09jul98,cjs  call new atomic add/delete routine in workspace
#                  add support <n>- for cardinality
# 01i,07jul98,cjs  add dependency checking for component change 
# 01h,23jun98,cjs  updated to work with new, globally unique object IDs 
# 01g,18jun98,ms   updated for new wccParamGet syntax
# 01f,09jun98,cjs  implement dataOut() with return value
# 01e,04may98,cjs  display properties, not just attributes 
# 01d,23apr98,cjs  various improvements 
# 01c,09apr98,cjs  changed function signatures for better support
#                  of multiple sheets 
# 01b,31mar98,cjs  got rid of unnecessary format() statements.
# 01a,27mar98,cjs  written.
#
# DESCRIPTION
#
#############################################################

namespace eval ::SelectionPropertySheet {
	variable _ctrlList
	variable _currentPage
	variable _componentNameMap
	variable _componentList
	variable _localParamValues
	variable _semaphore 0
	variable _cDoc
	variable _helpLink()

	# Fetch the cardinality of selection in cSet and return its bounds
	# in the variables named by rc1 and rc2
	proc cardinalityGet {cSet rcardinality rc1 rc2} {
		upvar $rc1 c1
		upvar $rc2 c2
		upvar $rcardinality cardinality 
		set cardinality [$cSet get COUNT]
		set result 0
		if {$cardinality != ""} {
			set result [scan $cardinality "%d-%d" c1 c2]
		}
		return $result
	}

	proc titleGet {handle sheetId} {
		return "$sheetId"
	}

	proc dateStampGet {handle} {
		return 980409
	}

	proc pageMatch {handle} {
		# Keeps track of what was initially in the selection
		# at startup
		variable _componentList ""
		variable _cDoc

		if {[string match [::Object::evaluate $handle Type] \
			selectionObject]} {

			# Do some one-time only initialization
			set prjObj [::Object::evaluate $handle Project]
			set hProj [::Object::evaluate $prjObj Handle]
			set prjName [::Object::evaluate $prjObj Name]
			set selectionName [::Object::evaluate $handle Name]
			set _cDoc [::Workspace::cxrDocGet $prjName]
			set wcc [::Object::evaluate $handle ComponentSet]

			# Cache what was originally in the selection
			# Determine all the selections's children that exist in the config,
			# then lump all their params together.  Use this to determine
			# if we need a params sheet
			set cSet [$_cDoc setCreate]
			$cSet = $selectionName
			$cSet = [$cSet get CHILDREN]
			$cSet = [$cSet & [::wccComponentListGet $hProj $wcc]]
			set _componentList [$cSet instances]
			$cSet = [$cSet get CFG_PARAMS]
			set properties [$cSet contents]

			# If defPageShowRead is set in the registry, then show the
			# component definition page
			set defsShow [::PSViewer::defPageShowRead]

			if {$defsShow} {
				lappend sheets "Definition"
			}
			if {[llength $properties]} {
				lappend sheets "Params"
			}
			lappend sheets Components
			if {[::ComponentViewServer::bError $hProj $selectionName]} {
				lappend sheets "Errors"
			}

			lappend sheets "General"

			set retval [list 1 $sheets]
			$cSet delete
		} else {
			set retval 0
		}
		return $retval 
	}

	proc OnShow {windowName sheet handle} {
		set name [::Object::evaluate $handle DisplayName]
		windowTitleSet $windowName "Properties: selection '$name'"

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

	# XXX todo: a "set defaults" button
	proc controlsCreate {windowName sheet handle} {
		variable _ctrlList
		variable _currentPage

		set ns SelectionPropertySheet
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
							[list ${ns}::onSelParamValue $windowName] \
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
						-callback [list ::SelectionPropertySheet::onSelPropDef \
						$windowName]] \
					[list text -name defvaluelist \
						-readonly -multiline -x 100 -y 32 -w 126 -h 68] \
				]
			}
			"Components" {
				set _ctrlList($sheet) [list \
					[list label -name selsynopsislbl -x 10 -y 20 -w 200 -h 20 \
						-title "<Synopsis>"] \
					[list group -name selcomponentsgroup -x 10 -y 38 \
						-w 200 -h 62 -title "Instructions"] \
					[list checklist -name sellist \
						-callback [list ::SelectionPropertySheet::onSelComp \
						$windowName] -x 14 -y 50 -w 182 -h 50] \
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
						-title "Help Topics"] \
					[list list -name listhelptopics -x 58 -y 98 -w 150 -h 36] \
				]
			}
		}
		foreach ctrl $_ctrlList($sheet) {
			controlCreate $windowName $ctrl
		}
		OnHide $windowName $sheet $handle

                if {$sheet == "General"} {
		    controlCallbackSet $windowName.listhelptopics \
			"::SelectionPropertySheet::onHelpTopics $windowName"
                }
	}

	proc onSelComp {windowName} {
		variable _cDoc
		set handle [::Workspace::selectionGet]
		set event [controlEventGet ${windowName}.sellist]

		if {[string match $event "chkchange"]} {
			set prjObj [::Object::evaluate $handle Project]
			set hProj [::Object::evaluate $prjObj Handle]
			set prjName [::Object::evaluate $prjObj Name]
			set selectionName [::Object::evaluate $handle Name]
			set cSet [$_cDoc setCreate]
			set wcc [::Object::evaluate $handle ComponentSet]

			# Uncheck the last selection if cardinality is 1-1
			$cSet = $selectionName
			set result [cardinalityGet $cSet cardinality c1 c2]
			if {($result == 2) && ($c1 == $c2) && ($c1 == 1)} {

				# This is the one to be checked
				set id [controlSelectionGet ${windowName}.sellist]

				# Unset everything
				set checklist [controlValuesGet ${windowName}.sellist]
				set idx 0
				foreach item $checklist {
					if {$idx != $id} {
						controlItemPropertySet ${windowName}.sellist \
							-checkstate -index $idx 0
					}
					incr idx
				}
			}

		    ::PSViewer::dirtyFlagSet 1
			$cSet delete
		}
	}

	proc onSelPropDef {windowName} {
		variable _cDoc
		set handle [::Workspace::selectionGet]
		set prjObj [::Object::evaluate $handle Project]
		set prjName [::Object::evaluate $prjObj Name]
		set selectionName [::Object::evaluate $handle Name]
		set curSel [controlSelectionGet ${windowName}.deflist -string]
		if {$curSel != ""} {
			set sSet [$_cDoc setCreate] 
			$sSet = $selectionName
			set values [$sSet get $curSel]
			if {$values == ""} {
				set values "N/A"
			}
			$sSet delete
			controlValuesSet ${windowName}.defvaluelist $values
		}
	}

	proc onSelParamValue {windowName} {
		variable _localParamValues
		variable _semaphore
		variable _cDoc

		set handle [::Workspace::selectionGet]
		set hProj [::Object::evaluate \
			[::Object::evaluate $handle Project] Handle]
		set wcc [::Object::evaluate $handle ComponentSet]
		set selectionName [::Object::evaluate $handle Name]
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
			set cSet [$_cDoc setCreate] 
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

	proc onHelpTopics {windowName} {
		variable _helpLink

		if {[controlEventGet ${windowName}.listhelptopics] != "dblclk"} {
			return
		}

		set helpTopic [controlSelectionGet ${windowName}.listhelptopics -string]
		windHelpDisplay $_helpLink($helpTopic)
    }

	proc dataIn {windowName handle sheet} {
		variable _componentNameMap
		variable _localParamValues
		variable _cDoc
		variable _helpLink

		::beginWaitCursor

		set selectionName [::Object::evaluate $handle Name]
		set projObj [::Object::evaluate $handle Project]
		set prjName [::Object::evaluate $projObj Name] 
		set hProj [::Object::evaluate $projObj Handle] 
		set wcc [::Object::evaluate $handle ComponentSet]
		set mDoc [::Workspace::mxrDocGet $prjName]
		set cSet [$_cDoc setCreate]

		switch $sheet {
			"Params" {
				catch {unset _localParamValues}

				# Determine all the selections's children that exist
				# in the config, then lump all their params together.
				$cSet = $selectionName
				$cSet = [$cSet get CHILDREN]
				$cSet = [$cSet & [::wccComponentListGet $hProj $wcc]]
				$cSet = [$cSet get CFG_PARAMS]
				set properties [lsort [$cSet contents]]

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
			"Definition" {
				# Set the title
				controlValuesSet ${windowName}.deflbl \
					[format "Attributes for selection %s" \
					[::Object::evaluate $handle DisplayName]]

				# Get all the attributes for the definition
				$cSet = $selectionName
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
			"Components" {
				# Set the synopsis
				$cSet = $selectionName
				set displayName [::Object::evaluate $handle DisplayName]
				set synopsis "$displayName: [$cSet get SYNOPSIS]"
				controlValuesSet ${windowName}.selsynopsislbl $synopsis
 
				# Set the directions based on the cardinality
				set result [cardinalityGet $cSet cardinality c1 c2]
				set instr "(Cardinality '$cardinality' not valid)"
				if {$result == 2} {
					if {$c1 == $c2} {
						set instr "Select exactly $c1"
						set instr "$instr component(s) from the following:"
					} else {
						set instr "Select min $c1, max $c2"
						set instr "$instr components from the following:"
					}
				} elseif {$result == 1 && [info exists c1]} {
					set instr "Select at least $c1"
					set instr "$instr Component(s) from the following:"
				}
				controlValuesSet ${windowName}.selcomponentsgroup $instr

				# Add all the components in the selection
				# If any are in the component set, place a check
				# next to them 
				$cSet = [$cSet get CHILDREN]
				$cSet = [$cSet - [::cxrUnavailListGet $_cDoc $mDoc]]
				set children [$cSet instances]
				set cfgComps [::wccComponentListGet $hProj $wcc]
				catch {unset _componentNameMap}
				foreach child $children {
					$cSet = $cfgComps
					if {[llength [$cSet & $child]]} {
						$cSet = $child
						lappend compList [list [$cSet get NAME] 1]
					} else {
						$cSet = $child
						lappend compList [list [$cSet get NAME] 0]
					}
					# Create a map between friendly and component names
					set _componentNameMap([$cSet get NAME]) $child
				}
				if {![info exists compList]} {
					messageBox "Selection '$selectionName' has no children"
				} else {
					controlValuesSet ${windowName}.sellist $compList
				}
			}
			"Errors" {
				set error [::ComponentViewServer::errorLookup \
					$hProj $selectionName]
				set errorItem [lindex $error 0]
				$cSet = $errorItem
				set displayName \
					[lindex [::ComponentViewServer::idToName $cSet] 0]
				set errorMsg [lindex $error 1]
				
				controlTextSet ${windowName}.errorname $displayName
				controlTextSet ${windowName}.errortext $errorMsg
			}
			"General" {
				controlValuesSet ${windowName}.txtcompmacro $selectionName

				$cSet = $selectionName
				set displayName [$cSet get NAME] 
				controlValuesSet ${windowName}.txtcompname $displayName

				set synopsis [$cSet get SYNOPSIS]
				if {$synopsis == ""} {
					set synopsis "(info not available for this selection)"
				}
				controlValuesSet ${windowName}.txtcompsynopsis $synopsis

				set componentList [::wccComponentListGet \
					$hProj [::wccCurrentGet $hProj]]
				$cSet = [$cSet & $componentList] 
				if {[$cSet instances Component] != ""} {
					set status "this selection is included"
				} else {
					set status "this selection is not included"
				}
				controlValuesSet ${windowName}.txtcompstatus $status

				# Fill the Topics list
				$cSet = $selectionName
				set topicList ""

				# Add help attribute to the Topics list
				set helpAttr [$cSet get HELP]
				foreach attr $helpAttr {
				if {$attr != "" && [lsearch $topicList $attr] == -1} {
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
			default {
				error "default case shouldn't occur for '$sheet'"
			}
		}

		# Cleanup
		::endWaitCursor
		$cSet delete
	}

	proc dataOut {windowName handle sheet} {
		variable _componentNameMap
		variable _componentList
		variable _cDoc

		::beginWaitCursor
		set retval 1
		set projObj [::Object::evaluate $handle Project]
		set hProj [::Object::evaluate $projObj Handle]
		set wcc [::Object::evaluate $handle ComponentSet]
		set prjName [::Object::evaluate $projObj Name] 
		set mDoc [::Workspace::mxrDocGet $prjName]
		set selectionName [::Object::evaluate $handle Name]
		set cSet [$_cDoc setCreate]

		switch $sheet {
			"Components" {
				::beginWaitCursor

				$cSet = $selectionName
				set displayName [::Object::evaluate $handle DisplayName]

				# Count the user's selections
				set count 0
				set componentInList ""
				foreach item [controlValuesGet ${windowName}.sellist] {
					if {[lindex $item 1] == 1} {	
						incr count
						lappend componentInList \
							$_componentNameMap([lindex $item 0]) 
					}
				}

				# Determine the cardinality and see if it matches the count;
				# if not, inform the user
				set instr "Cardinality invalid"
				set bCardinalityMatch 0 
				set result [cardinalityGet $cSet cardinality c1 c2]
				if {$result == 2} {
					if {$c1 == $c2} {
						set instr "Select exactly $c1"
						set instr "$instr component(s) for $displayName"
						if {$c1 == $count} {
							set bCardinalityMatch 1 
						}
					} else {
						set instr "$displayName requires min $c1, max $c2\n"
						set instr "$instr component(s).  \
							Save changes anyway?"
						if {($count >= $c1) && ($count <= $c2)} {
							set bCardinalityMatch 1
						}
					}
				} elseif {$result == 1 && [info exists c1]} {
					set instr "$displayName requires at least $c1\n"
					set instr "$instr component(s).  Save changes anyway?"
					if {$count >= $c1} {
						set bCardinalityMatch 1
					}
				}
				if {$bCardinalityMatch} {
					set result "ok"
				} else {
					set result [messageBox -okcancel -informationico $instr]
				}

				# If the cardinality was ok or the user waived his rights,
				# Do dependency checking.
				# Add anything needing adding (as compared to the original
				# component set), and remove anything not needed
				set retval 0
				if {[string match $result "ok"]} {

					# Get the components currently in the configuration
					set wccList [::wccComponentListGet $hProj $wcc]

					# Determine what must be added
					$cSet = $componentInList
					$cSet = [$cSet - $_componentList]
					set addList [$cSet instances]

					# Determine the dependencies for the add list
					$cSet = [::cxrSubtree $cSet $mDoc]			
					$cSet = [$cSet - $addList]
					$cSet = [$cSet - $wccList]
					set addDepList [$cSet instances Component]

					# Determine what must be removed
					$cSet = $_componentList
					$cSet = [$cSet - $componentInList]
					set removeList [$cSet instances]

					# Determine the dependencies for the remove list
                                        set cmpsIncluded "$wccList $addList $addDepList" 
				        $cSet = [::cxrSupertree $cSet $mDoc $cmpsIncluded]
					$cSet = [$cSet - $removeList]
					$cSet = [$cSet & $wccList]
					set removeDepList [$cSet instances Component]

					set ns ::ComponentViewServer::iHierarchyView::DepDlg

					# Convert the IDs to friendly names for the
					# dependency dialog, but save all the IDs before
					# hand 
					set allAddList [concat $addList $addDepList]
					$cSet = $addList
					set addList [ComponentViewServer::idToName $cSet]
					$cSet = $addDepList
					set addDepList [ComponentViewServer::idToName $cSet]
					set allRemoveList [concat $removeList $removeDepList]
					$cSet = $removeList
					set removeList [ComponentViewServer::idToName $cSet]
					$cSet = $removeDepList
					set removeDepList [ComponentViewServer::idToName $cSet]

					# Calculate the delta and new image size
					set oldSet $wccList
					$cSet = $wccList
					$cSet = [$cSet + $allAddList]
					$cSet = [$cSet - $allRemoveList]
					set newSet [$cSet instances Component]
					set sizeData [ComponentViewServer::sizeInfoGet $cSet \
						$mDoc $oldSet $newSet]

					# Show the user the dependency dialog
					set ns ::ComponentViewServer::gui::DepDlg
					set tmp [concat $addList $addDepList]
					set tmp2 [concat $removeList $removeDepList]
					if {$tmp != "" && $tmp2 == ""} {
						${ns}::depDlgShow add $addList $addDepList \
							$removeList $removeDepList \
							[lindex $sizeData 0] [lindex $sizeData 1]
					} elseif {$tmp == "" && $tmp2 != ""} {
						${ns}::depDlgShow remove $addList $addDepList \
							$removeList $removeDepList \
							[lindex $sizeData 0] [lindex $sizeData 1]
					} elseif {$tmp != "" && $tmp2 != ""} {
						${ns}::depDlgShow select $addList $addDepList \
							$removeList $removeDepList \
							[lindex $sizeData 0] [lindex $sizeData 1]
					} else {
						# Fake a cancel
						set ${ns}::_retCode cancel
					}

					if {[string match [set ${ns}::_retCode] ok]} {
						# Call atomic add and delete operation 
						::Workspace::componentAddDelete ::ComponentViewServer \
							$prjName $wcc $allAddList $allRemoveList
						set retval 1
					}
				} else {
					set retval 0
				}
			}
			"Params" {
				variable _localParamValues
				set retval 1
				foreach prop [array names _localParamValues] {
					if {[catch {::wccParamSet $hProj $wcc \
						$prop $_localParamValues($prop)} error]} {
						controlSelectionSet ${windowName}.proplist -string $prop
						messageBox -ok $error
						set retval 0
					}
				}
			}
			"Errors" -
			default {
			}
		}

		$cSet delete
		::endWaitCursor

		return $retval
	}

	# This gets called as the final step to dataOut.
	proc dataCommit {handle} {
	}

	# This must be done last, to allow interface validation
	if {[catch {::Workspace::propPageRegister \
		::SelectionPropertySheet} \
		errorMsg]} {
		messageBox $errorMsg
	}
}
