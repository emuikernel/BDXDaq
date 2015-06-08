#############################################################
#
# PSManager.addin.tcl -- Property Sheet Manager Add-in
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01z,05nov01,rbl  get "show advanced component properties" working on unix
# 01y,16mar99,cjs  Add optional param to propSheetShow() to display a
#                  particular page
# 01x,04mar99,ren  Now OK button is always enabled.
# 01w,01feb99,ren  Adding help button.
# 01v,11jan99,ren  Show advanced properties tab on UNIX
# 01u,04dec98,cjs  Put dataDiscard() back into a catch()
# 01t,01dec98,cjs  Needed an dataDiscard() function
# 01s,18sep98,ren  Reduced flicker on UNIX by delaying display of dialog
#                  until totally drawn.
# 01r,15sep98,cjs  add order to property sheets 
# 01q,20aug98,cjs  disable OK also when sheets are clean 
# 01p,29jul98,cjs  trigger SelectionChange event and call to
#                  prop sheet's optional dataCommit() routine
#                  in OnApply(); fix bug in page replacement
# 01o,27jul98,jak  added  OK, Cancel buttons instead of Close
# 01n,17jul98,cjs  added defPageShowRead/Write() 
# 01m,07jul98,cjs  change all windowShow()s to windowHide()
# 01l,23jun98,cjs  use controlSelectionGet -string for tab control 
# 01k,18jun98,jmp  increased size of Build Spec window to add rules and
#                  macros editors.
# 01j,09jun98,cjs  use return value for dataOut()/dirty logic
# 01i,07may98,cjs  unset dirty flag after dataOut() 
# 01h,23apr98,cjs  add windowQueryCloseCallback for sheet 
# 01g,21apr98,cjs  add a default tab for property sheets 
# 01f,10apr98,cjs  wrapped arg to controlTabsAdd in protective
#                  quotes.  Added logic to hide sheet when
#                  handle is null.
# 01e,09apr98,cjs  changed function signatures for better support
#                  of multiple sheets; fixed Title()
# 01d,08apr98,cjs  pass object handle with titleGet(). 
# 01c,06apr98,cjs  got rid of a debug puts statement. 
# 01b,31mar98,cjs  got rid of unnecessary format() statements.
# 01a,20mar98,cjs  created. 
#
# DESCRIPTION
# Each sheet has the opportunity to register to handle
# the same object multiple times (ie, it can display different
# attributes of the same object in as many sheets) by passing
# back a context "cookie" for each sheet it plans to display.
# This cookie should be the tab sheet's title.
#
#############################################################

namespace eval ::PSViewer {

	variable _connections
	variable _currentTab ""
	variable _propertySheets
	variable _dirtyFlag 0
	variable _currentSheetSet ""

	#############################################################
	#
	# Workspace extension 
	#
	#############################################################
	proc ::Workspace::propPageRegister {namespace} {

		set apiList { titleGet controlsCreate controlsDestroy OnShow OnHide \
			pageMatch dataIn dataOut dateStampGet }

		foreach api $apiList {
			if {[namespace eval $namespace [list info proc $api]] == ""} {
				set error \
					"Invalid property sheet '$namespace': $api not supplied"
				set error "$error \nProperty sheet must support: $apiList"
				error $error
			}
		}

		lappend ::PSViewer::_propertySheets $namespace
	}

	proc ::Workspace::propSheetShow {{page ""}} {
		# if it hasn't been created already
		::PSViewer::CreateThePropertyDialog

		# Show the property sheets
		::PSViewer::propertySheetsShow $page

		# Show the window
		::PSViewer::DisplayThePropertyDialog
	}

	proc ::Workspace::propSheetHide {} {
		# Hide the window
		::PSViewer::HideThePropertyDialog
	}

	proc dirtyFlagSet {{value 1}} {
		if [ThePropertyDialogIsVisible] {
			variable _dirtyFlag
			set _dirtyFlag $value 
			controlEnable ::PSViewer::psWindow.applybutton $value
			controlEnable ::PSViewer::psWindow.okbutton 1
		}
	}
	
	# Determine if component definition pane should be shown for
	# components, selections, etc.
	proc defPageShowRead {} {
	    return [appRegistryEntryRead -default 0 Workspace ShowCompDefSheet]
	}

	proc defPageShowWrite {bState} {
		appRegistryEntryWrite Workspace ShowCompDefSheet $bState
	}

	#############################################################
	#
	# Add-in API implementation 
	#
	#############################################################

	proc Init {} {

		# Register for events
		::Workspace::eventRegister ::PSViewer {SelectionChange}

		# Source known property sheets and allow them to register
		# themselves

		# Source each property sheet 
		set path "[wtxPath host resource tcl app-config Project]*.psheet.tcl"
		if {![catch {glob $path} sheets]} {
			foreach sheet [glob $path] {
				source $sheet
			}
		}
			
	}

	proc ThePropertyDialogIsVisible {} {
		expr { [windowExists ::PSViewer::psWindow] && [windowVisible ::PSViewer::psWindow] }
	}

	proc HideThePropertyDialog {} {
		if [ThePropertyDialogIsVisible] {
			windowHide ::PSViewer::psWindow 1
		}
	}

	proc CreateThePropertyDialog {} {
		if {! [windowExists ::PSViewer::psWindow] } {
			
			# Create the property window 
			dialogCreate -name ::PSViewer::psWindow \
			-parent ::ViewBrowser \
			-title "Properties"	-modeless -noframe \
			-x 100 -y 0 -width 294 -height 140 \
			-init \
			{
				# Create OK, Cancel and Apply buttons
				controlCreate ::PSViewer::psWindow \
				[list button -name cancelbutton \
				 -title Cancel \
				 -callback ::PSViewer::OnCancel \
				 -xpos 240 -ypos 36 -width 50 -height 14]
				controlCreate ::PSViewer::psWindow \
				[list button -name okbutton \
				 -title OK \
				 -callback ::PSViewer::OnOK \
				 -xpos 240 -ypos 18 -width 50 -height 14]
				controlCreate ::PSViewer::psWindow \
				[list button -name applybutton \
				 -title Apply \
				 -callback ::PSViewer::OnApply \
				 -xpos 240 -ypos 54 -width 50 -height 14]
				controlCreate ::PSViewer::psWindow \
				[list button -name helpbutton \
				 -title Help \
				 -helpbutton \
				 -xpos 240 -ypos 72 -width 50 -height 14]
			}
			windowQueryCloseCallbackSet ::PSViewer::psWindow \
			[list ::PSViewer::OnQueryClose]
		}
	}

	proc DisplayThePropertyDialog {} {
		if {! [windowExists ::PSViewer::psWindow] } {
			CreateThePropertyDialog
		} elseif { ![windowVisible ::PSViewer::psWindow] } {
			windowHide ::PSViewer::psWindow 0
		}
	}


	proc OnEvent {cookie event} {
		# If the property window isn't visible, don't bother
		# with the rest
		if [ThePropertyDialogIsVisible] {
			propertySheetsShow ""
		}
	}

	proc OnQueryClose {} {
		variable _dirtyFlag

		# This dialog never really closes, it just gets hidden
		if {$_dirtyFlag} {
			set status [messageBox -okcancel \
				"Values not saved.  Close Property sheets anyway?"]
			if {[string match $status "ok"]} {
				::Workspace::propSheetHide	
			}
		} else {
			::Workspace::propSheetHide	
		}
		return 0
	}

	#############################################################
	#
	# Private helper functions 
	#
	#############################################################


	proc propertySheetsShow {preferredPage} {

		# Ask the workspace for the list of registered property sheets
		# Check each to see which are applicable, and collect those
		# that are.
		set handle [::Workspace::selectionGet]
		if {$handle != ""} {
			if {[info exists ::PSViewer::_propertySheets]} {
				foreach psheet $::PSViewer::_propertySheets {

					# Page match will return one of two things:
					# 0, or a list consisting of 1 and a number of sheet
					# cookies (which can be passed back to the property
					# sheet to reference a unique sheet)
					set cmd [list ${psheet}::pageMatch $handle]
					set retval [eval $cmd]
					if {[lindex $retval 0] != 0} {
						# Save the sheet (namespace) and the list of cookies
						lappend sheetSetList [list $psheet [lindex $retval 1]]
					}
				}
			}

			# Prune the list down to one sheet.
			# with any others registered for this object, and
			# We need to compare the datestamp for the sheet set
			# against that of anyone else registering for this object.
			if {[info exists sheetSetList]} {
				# Save the last sheetSet for a later optimization
				variable _currentSheetSet
				set oldSheetSet $_currentSheetSet
				set _currentSheetSet ""

				foreach page $sheetSetList {
					set newns [lindex $page 0]
					set newDateStamp [eval [list ${newns}::dateStampGet \
						$handle]]
					if {$_currentSheetSet == ""} {
						set _currentSheetSet $page
					} else {
						set oldns [lindex $_currentSheetSet 0]
						set oldDateStamp [eval [list ${oldns}::dateStampGet \
							$handle]]
						if {$newDateStamp > $oldDateStamp} {
							set _currentSheetSet $page
						}
					}
				}
	
				if {$_currentSheetSet != ""} {

					# If this is the same _currentSheetSet as last time,
					# and it has the same number of sheets,
					# don't bother recreating the sheets, just refresh the
					# data

					if {(![string match [lindex $oldSheetSet 0] \
							[lindex $_currentSheetSet 0]]) || \
						(![string match [lindex $_currentSheetSet 1] \
							[lindex $oldSheetSet 1]])} {

						# Destroy previous sheets, if they exist
						foreach sheetTitle [array names \
							::PSViewer::_connections] {
							set ns $::PSViewer::_connections($sheetTitle)
							eval [format "%s::controlsDestroy \
								::PSViewer::psWindow \
								[list $sheetTitle $handle]" $ns]
						}
						catch {unset ::PSViewer::_connections}
	
						# Destroy the whole tab control, because
						# controlTabDestroy() doesn't exist
						if {[controlExists ::PSViewer::psWindow.tabctrl]} {
							controlDestroy ::PSViewer::psWindow.tabctrl
						}

						# Create a fresh, empty tab control
						controlCreate ::PSViewer::psWindow \
							[list tab -name tabctrl \
							-callback ::PSViewer::OnTabClicked \
							-multiline -xpos 4 -ypos 4 \
							-width 230 -height 130]

						# Create each sheet 
						set ns [lindex $_currentSheetSet 0]

						# Give the sheets a standard order
						set preferredList {Errors General \
							Components Dependencies Build Size Definition \
							Rules Macros}
						set sheetList [::preferredPageSort \
							$preferredList [lindex $_currentSheetSet 1]]

						# If we've been passed a default sheet,
						# attempt to use that
						if {$preferredPage != ""} {
							set idx [lsearch $sheetList $preferredPage]
							if {$idx != -1} {
								set ::PSViewer::_currentTab $idx 
							} else {
								set ::PSViewer::_currentTab 0
							}
						}

						foreach sheet $sheetList { 
							# Connect the tab to the sheet
							set title [eval [list ${ns}::titleGet \
								$handle $sheet]]
							set ::PSViewer::_connections($title) $ns
		
							# Create the sheet
							sheetCreate ::PSViewer::psWindow $title
						}
					}

					set ns [lindex $_currentSheetSet 0]
					foreach sheet [lindex $_currentSheetSet 1] {
						# If we haven't already, connect the tabs to the sheets
						if {![string match [lindex $oldSheetSet 0] \
							[lindex $_currentSheetSet 0]]} {
							set title [eval [list ${ns}::titleGet \
								$handle $sheet]]
							set ::PSViewer::_connections($title) $ns
						}
		
						# Ask the sheets to update themselves
						set cmd [list ${ns}::dataIn ::PSViewer::psWindow \
							$handle $sheet]
						if {[catch {eval $cmd} errorMsg]} {
							messageBox \
								"call to \"${ns}::dataIn\" failed: $errorMsg"
						}
					}

					# Make the window and the correct tab visible
					controlSelectionSet ::PSViewer::psWindow.tabctrl \
						$::PSViewer::_currentTab
					set ::PSViewer::_currentTab [controlSelectionGet \
						::PSViewer::psWindow.tabctrl -string]
					OnTabClicked
					DisplayThePropertyDialog
				} else {
					set error "Internal error in property"
					set error "$error sheetSet selection process"
					error $error
				}
			} else {
				messageBox "No property sheets found in current installation!"
			}

			# Reset the dirty flag
			dirtyFlagSet 0

		} else {
			# No handle -> no prop sheets.  Hide page.
			HideThePropertyDialog
		}
	}

	proc sheetCreate {windowName pane} {

		catch {controlTabsAdd ${windowName}.tabctrl \"$pane\"}

		# Create the necessary controls by getting the server's
		# gui interface and asking the interface to create controls
		set handle [::Workspace::selectionGet]
		set psheetIface $::PSViewer::_connections($pane)
		if {[catch {eval [list ${psheetIface}::controlsCreate \
			$windowName $pane $handle]} \
			errorMsg]} {
			puts "error creating controls for interface '$psheetIface':"
			puts "    $errorMsg"
		}

		# Only show the tab control if there is more than one tab
		#if {[llength [array names ::PSViewer::_connections]] > 1} {
		#	controlHide ::PSViewer::psWindow.tabctrl 0
		#} else {
		#	controlHide ::PSViewer::psWindow.tabctrl 1
		#}
	}

	proc OnTabClicked {} {

		# Hide the last one, show the new one 
		set tab $::PSViewer::_currentTab
		set handle [::Workspace::selectionGet]
		
		set iface $::PSViewer::_connections($tab)
		eval {${iface}::OnHide ::PSViewer::psWindow $tab $handle}

		# Show the new current selection
		set tab [controlSelectionGet ::PSViewer::psWindow.tabctrl -string]
		set iface $::PSViewer::_connections($tab)
		eval {${iface}::OnShow ::PSViewer::psWindow $tab $handle}
		set ::PSViewer::_currentTab $tab
	}

	proc OnOK {} {
		variable _dirtyFlag
		if { $_dirtyFlag } {
			OnApply
		}
		::Workspace::propSheetHide
	}

	proc OnCancel {} {
		# Any one-time only cleanup
		set tab $::PSViewer::_currentTab
		set ns $::PSViewer::_connections($tab)

		catch {${ns}::dataDiscard [::Workspace::selectionGet]}
		::Workspace::propSheetHide
	}

	proc OnApply {} {

		# Ask each property sheet to update its data source
		set dirtyState 0
		foreach sheet [array names ::PSViewer::_connections] {
			set ns $::PSViewer::_connections($sheet) 
			set handle [::Workspace::selectionGet]
			set cmd [list ${ns}::dataOut ::PSViewer::psWindow \
				$handle $sheet]
			if {[catch {eval $cmd} errorMsg]} {
				messageBox "call to \"${ns}::dataOut\" failed: $errorMsg"
				# dataOut failed ungracefully; don't clear dirty flag
				set dirtyState 1
			} else {
				# dataOut failed gracefully and returned 0;
				# don't clear dirty flag
				if {$errorMsg == 0} {
					set dirtyState 1
				}
			}
		}

		# Reset the dirty flag
		dirtyFlagSet $dirtyState 

		if {!$dirtyState} {
			# Any one-time only commitment 
			catch {${ns}::dataCommit $handle}

			# Have the sheet refresh itself
			::Workspace::selectionSet [::Workspace::selectionGet]
		}
	}

	::Workspace::addinRegister ::PSViewer

	# Add a menu for showing the properties window
	menuItemAppend -bypath -separator {&View}

	menuItemAppend -bypath -callback \
		{ ::Workspace::propSheetShow } \
		{&View} {&Properties}
}
