# WelcomeDlg.tcl - dialog that appears on Tornado startup
#
# modification history
# --------------------
# 01y,23mar99,rbl  adding showWelcomeDlgIfNoWorkspaceOpen function
#                  fix SPR 25961 by making column wider in recentList table control
# 01x,15mar99,ren  Made Welcome dialog 20 units wider
# 01w,10mar99,cjs  Restore existing workspace text control
# 01v,10mar99,cjs  Patching up previous checkin
# 01u,08mar99,cjs  Adding icon support
# 01t,02mar99,ren  Changing titles to clarify Workspace and Project
#                  abstractions
# 01s,24feb99,rbl  Fixing SPR 23034, "Create Workspace window on Startup checkbox is ignored"
# 01r,01feb99,ren  Adding help button.
# 01q,13jan99,jak  resize lists on dlg
# 01p,30oct98,ren  cancel button no longer appears to be default button.
# 01o,16oct98,cjs  update title of dialog based on current tab; fix
#                  error handling for project creation
# 01n,05oct98,cjs  change references to 'Project' to 'Workspace' 
# 01m,03oct98,cjs  modified to work with new workspace management code 
# 01l,21sep98,jak  simplify interaction with workspace (just call projectAdd)
# 01k,04sep98,jak  prj creation fail dlg 
# 01j,19aug98,cjs  fixed 'return' inadvertantly removed 2 checkins ago 
# 01i,19aug98,cjs  fixed one char typo 
# 01h,19aug98,cjs  call workspaceOpen(), workspaceClose() directly now 
# 01g,17aug98,cjs  Fix browse button on open existing; change dialog title
# 01f,29jul98,cjs  call queryClose() instead of guiWorkspaceClose() 
# 01f,01jul98,cjs  changed new project label to reflect usage scenario 
# 01e,26jun98,cjs  don't call anything folllowing windowClose()
# 01d,23jun98,cjs  use controlSelectionGet -string for tab control 
# 01c,23jun98,cjs  centered dialog(s) 
# 01b,27may98,cjs  modified logic so that we check for existing
#                  startup dialog; go to "New" pane when
#                  MRU is empty. 
# 01a,20may98,cjs  written. 
#
# DESCRIPTION
# This file implements the welcome dialog that helps the user locate
# an existing project workspace, or create a new one.
#
# The menuChoice option allows 
# Registry Params:
# <app registry section> Workspace WorkspaceList - list of recent workspaces
# <app registry section> Workspace ShowWelcome - bool indicating whether
#	welcome dialog should be shown 
#
#############################################################

namespace eval ::WelcomeDlg {
}

	proc ::WelcomeDlg::welcomeStateRead {} {
		set state [appRegistryEntryRead -default 1 Workspace ShowWelcome]
		return $state
	}
	proc ::WelcomeDlg::welcomeStateWrite {bState} {
		appRegistryEntryWrite Workspace ShowWelcome $bState 
	}
	proc ::WelcomeDlg::mruEntriesRead {{bValidate 0}} {
		set mruEntries [appRegistryEntryRead Workspace WorkspaceList]

		# If validation is requested, get rid of all entries that
		# don't exist on disk
		if {$bValidate} {
			foreach entry $mruEntries {
				if {![file exists $entry]} {
					set idx [lsearch $mruEntries $entry]
					if {$idx != -1} {
						set mruEntries [lreplace $mruEntries $idx $idx]
					}
				}
			}
			mruEntriesWrite $mruEntries
		}
		return $mruEntries
	}

	proc ::WelcomeDlg::mruEntriesWrite {entries} {
         appRegistryEntryWrite Workspace WorkspaceList $entries
	}

	###############################################################
	#
	# showWelcomeDlgIfNoWorkspaceOpen
	#
	# calls welcomeDlgShow only if there is not a workspace
	# open. This is used when Tornado is first coming up: we may 
	# have passed in a workspace and opened it, and then don't 
	# want the Welcome window...
	#
	proc ::WelcomeDlg::showWelcomeDlgIfNoWorkspaceOpen {} {
		if {[::Workspace::openWorkspaceGet] == ""} {
			welcomeDlgShow
		}
	}

	# We can enter this proc from one of three states:
	# (1) The dialog is already showing, and the user has picked a
	#	menu selection
	# (2) The dialog is not showing, and the user picks a menu item
	# (3) The dialog is not showing, and the app has just started 
	#
	proc ::WelcomeDlg::welcomeDlgShow {{menuChoice ""}} {
		variable _workspaceName
		variable _lastTab
		variable _ctrlList

		# Auto load last workspace if the user has disabled this dialog
		# Also, ensure that registry contains a value
		if {$menuChoice == ""} {
			set state [welcomeStateRead]
			if {$state == 0} {
				set mruEntries [mruEntriesRead 1]
				# load last workspace if it exists
				if {$mruEntries != ""} {
					::Workspace::workspaceClose
					if {[catch {::Workspace::workspaceOpen \
						[lindex $mruEntries 0]} error]} {
						messageBox -ok -exclamationicon $error
					}
					return
				} else {
					return
				}
			} else {
				welcomeStateWrite 1
			}
		}

		# If this dialog isn't showing, create it 
		# and initialize it

		if {![windowExists welcomeDlg]} {

			# Use menuChoice as the last pane selection to bootstrap
			# onTabClicked.
			# This works because all controls are created hidden.
			if {$menuChoice != ""} {
				set _lastTab $menuChoice
			} else {
				# If this is startup and there aren't any recent
				# projects, set new project as default pane
				set mruEntries [mruEntriesRead 1]
				if {$mruEntries == ""} {
					set _lastTab "New"
				} else {
					set _lastTab "Recent"
				}
			}

			set _ctrlList([list Recent]) {
				{label -name recentlbl -title "Select a workspace:" \
					-x 10 -y 20 -width 212 -height 10}
				{table -name recentlist \
					-columns 1 -border \
					-x 10 -y 30 -width 208 -height 68 \
					-callback {::WelcomeDlg::onEvent recentlist}}
			}
			set _ctrlList([list Existing]) {
				{label -name existinglbl -title "Enter a workspace \
					to open, or press 'Browse...' to search:" \
					-x 10 -y 20 -width 208 -height 20}
				{text -name existingtext \
					-x 10 -y 40 -width 208 -height 12}
				{button -name existingbutton \
					-title "Browse..." \
					-x 148 -y 70 \
					-width 50 -height 14 \
					-callback ::WelcomeDlg::onBrowse \
				}
			}
			set _ctrlList([list New]) {
				{label -name newlbl -title \
					"What would you like to do?" \
					-x 10 -y 20 -width 212 -height 10}
				{table -name newlist -columns 1 -border \
					-x 10 -y 30 -width 208 -height 68 \
					-callback {::WelcomeDlg::onEvent newlist}}
			}

			dialogCreate -name welcomeDlg -title "Open Workspace" -width \
				282 -height 120 -init ::WelcomeDlg::initProc -modeless
		} else {
			# The dialog has already been created.  Hide the last set
			# of controls, set _lastTab to the correct pane, then
			# call onTabClicked to get the correct pane shown	

			foreach ctrl $_ctrlList($_lastTab) {
				set ctrlName [::flagValueGet $ctrl -name]
				controlHide welcomeDlg.$ctrlName 1
			}
			set _lastTab $menuChoice
			controlSelectionSet welcomeDlg.tabctrl -string $_lastTab
			onTabClicked
		}
	}

	proc ::WelcomeDlg::onTabClicked {} {
		variable _lastTab
		variable _ctrlList
		set currentTab [controlSelectionGet welcomeDlg.tabctrl -string]

		# Hide the last set of controls
		foreach ctrl $_ctrlList($_lastTab) {
			set ctrlName [::flagValueGet $ctrl -name]
			controlHide welcomeDlg.$ctrlName 1
		}

		# Show the next set of controls
		foreach ctrl $_ctrlList($currentTab) {
			set ctrlName [::flagValueGet $ctrl -name]
			controlHide welcomeDlg.$ctrlName 0
		}
		set _lastTab $currentTab

		# Set the title to reflect the pane
		switch $currentTab {
			"New" {
				set workspace [::Workspace::openWorkspaceGet]
				if {$workspace == ""} {
					set title "Create Project in New/Existing Workspace"
				} else {
					set title "Create Project in Workspace \
						'[file rootname [file tail $workspace]]'"
				}
			}
			"Existing" - 
			"Recent" {
				set title "Open Workspace"
			}
		}
		windowTitleSet welcomeDlg $title 
	}

	proc ::WelcomeDlg::initProc {} {
		variable _lastTab
		variable _ctrlList

		# Create the welcome dialog's controls
		set ctrls {
			{tab -name tabctrl -x 4 -y 4 -width 220 -height 100 \
				-callback ::WelcomeDlg::onTabClicked}
			{button -name okbutton -title OK -x 228 -y 20 \
				-width 50 -height 14 -default \
				-callback ::WelcomeDlg::onOk}
			{button -name cancelbutton \
				-title Cancel \
				-x 228 -y 38 \
				-width 50 -height 14 \
				-callback {windowClose welcomeDlg} }
			{button -name helpbutton -title Help -x 228 -y 56 \
				-width 50 -height 14 -helpbutton} \
			{boolean -name showbool -title "Show this window on startup" \
				-auto -x 7 -y 105 -width 150 -height 14 \
				-callback {
					# Save the show/hide state
					set state [controlChecked welcomeDlg.showbool]
					::WelcomeDlg::welcomeStateWrite $state
				}
			}
		}
		foreach ctrl $ctrls {
			controlCreate welcomeDlg $ctrl
		}

		# Create all the tabs and their controls (in a hidden state)
		set tabList [array names _ctrlList]
		foreach tab $tabList {
			controlTabsAdd welcomeDlg.tabctrl [list $tab]

			foreach ctrl $_ctrlList($tab) {
				controlCreate welcomeDlg $ctrl
				set ctrlName [::flagValueGet $ctrl -name]
				controlHide welcomeDlg.$ctrlName 1
			}
		}

		recentlistInit

		newlistInit

		# Set the current tab to be Recent
		controlSelectionSet welcomeDlg.tabctrl -string $_lastTab

		# Enable/disable the show again checkbox
		set state [::WelcomeDlg::welcomeStateRead]
		controlCheckSet welcomeDlg.showbool $state

		# Make sure the correct tab is showing
		onTabClicked
	}

	proc ::WelcomeDlg::recentlistInit {} {

		# Initialize the Recent list
		set mruEntries [NativeFileNames [appRegistryEntryRead \
			Workspace WorkspaceList]]
		
		# The table control is very fussy and will misinterpret
		# backslashes, so we must protect them
		foreach mruEntry $mruEntries {
			lappend workspaceList [list $mruEntry]
		}

		# Make the workspace icon known
		controlPropertySet welcomeDlg.recentlist \
			-imagelist "Icons.dll IDI_WORKSPACE"

		# Make the column wide so that paths are hardly ever truncated with the table
		# control's "..."
		controlPropertySet welcomeDlg.recentlist \
			-columnwidths [list 500]

		if {[info exists workspaceList]} {
			controlValuesSet welcomeDlg.recentlist $workspaceList 
			controlSelectionSet welcomeDlg.recentlist 0 

			set i 0
			foreach workspace $workspaceList {
				controlItemPropertySet welcomeDlg.recentlist \
					-imageno 0 -row $i
				incr i
			}

			# Put the most recent item in the Existing text control
			# We do this here only to avoid duplicating code
			controlValuesSet welcomeDlg.existingtext \
				[lindex [lindex $workspaceList 0] 0]
		}
	}

	proc ::WelcomeDlg::newlistInit {} {
		# Initialize the new project dialog with available wizards
		controlPropertySet welcomeDlg.newlist -columnwidths [list 200]

		set i 0
		catch {unset iconsToUse}
		catch {unset iconOffsetMap}
		foreach wiz [lsort [::ViewBrowser::projectWizardListGet]] {
			lappend newlistList [list $wiz]

			# Also, enumerate icons and create an array to their
			# offsets
			set wizard [::ViewBrowser::projectWizardGet $wiz]
			set icon [${wizard}::iconGet]
			lappend iconsToUse $icon 
			set iconOffsetMap($icon) $i
			incr i
		}
		if {[info exists newlistList]} {
			controlValuesSet welcomeDlg.newlist $newlistList
		}
		if {[info exists iconsToUse]} {
			controlPropertySet welcomeDlg.newlist \
				-imagelist "Icons.dll $iconsToUse"
		}
		
		set tableIndex 0
		foreach wiz [lsort [::ViewBrowser::projectWizardListGet]] {
			set wizard [::ViewBrowser::projectWizardGet $wiz]
			set iconImage [${wizard}::iconGet]
			set iconOffset $iconOffsetMap($iconImage)
			controlItemPropertySet welcomeDlg.newlist \
				-imageno $iconOffset -row $tableIndex
			incr tableIndex
		}
		controlSelectionSet welcomeDlg.newlist 0
	}

	proc ::WelcomeDlg::onEvent {ctrl} {
		switch [controlEventGet welcomeDlg.$ctrl] {
			dblclk {
				onOk
			}
			default {
			}
		}
	}

	proc ::WelcomeDlg::onBrowse {} {
		set cwd [pwd]
		set handle [::Workspace::selectionGet]

		# If there is a current project, open this dialog
		# in its parent 
		set dir [file dirname [controlValuesGet \
			welcomeDlg.existingtext]]
		if { "$dir" != "" && ![catch { cd $dir }] } {
		} elseif {$handle != "" && \
			![string match [Object::evaluate \
			$handle Type] nullObject]} {
			set prjObj [::Object::evaluate $handle Project]
			set prjName [::Object::evaluate $prjObj Name]
			set split [file split $prjName]
			set split [lrange $split 0 \
				[expr [llength $split] - 2]]
			set path [eval file join $split]
			if {[catch {cd $path} error]} {
				catch {cd [::prjDirDefault]}
			}
		} else {
			catch {cd [::prjDirDefault]}
		}
		set fileName [fileDialogCreate \
			-filefilters "Workspace Files(*.wsp)|*.wsp||" \
			-title "Workspace Files" \
			-okbuttontitle "Open"]
		if {$fileName != ""} {
			controlValuesSet welcomeDlg.existingtext \
				[lindex $fileName 0]
		}
	}

	proc ::WelcomeDlg::onOk {} {
		set currentTab [controlSelectionGet welcomeDlg.tabctrl -string]
		switch $currentTab {
			"Recent" {
				# -string flag is broken for table control
				set workspaceFile [lindex [lindex \
					[controlValuesGet welcomeDlg.recentlist] \
					[controlSelectionGet welcomeDlg.recentlist]] 0]
				if {[::ViewBrowser::querySave]} {
					if {![catch {::Workspace::workspaceOpen $workspaceFile} \
						error]} {
						windowClose welcomeDlg
					} else {
						messageBox -ok -exclamationicon $error
					}
				}
			}
			"Existing" {
				set workspaceFile [controlTextGet welcomeDlg.existingtext]
				if {[::ViewBrowser::querySave]} {
					if {![catch {::Workspace::workspaceOpen $workspaceFile} \
						error]} {
						windowClose welcomeDlg
					} else {
						messageBox -ok -exclamationicon $error
					}
				}
			}
			"New" {
				# Get the wizard name, and map it to a namespace
				# -string flag is broken for table control
				set projType [lindex [lindex \
					[controlValuesGet welcomeDlg.newlist] \
					[controlSelectionGet welcomeDlg.newlist]] 0]
				set wizard [::ViewBrowser::projectWizardGet $projType]

				# Run the wizard.  It returns {retval workspace}
				# If it's successful, it has created a project for
				# us and returned the name of the workspace that
				# we should open
				set retval [${wizard}::projectWizardRun]
				set retcode [lindex $retval 0]
				if {$retcode == -1} {
					prjCreateErr [lindex $retval 1]
				} elseif {$retcode == 0} {
					set projectFile [lindex $retval 1]
					if {$projectFile != ""} {
						windowClose welcomeDlg
					}
				}
			}
		}
	}

	# prjCreateErr - indicates failure of project creation, with details
	proc ::WelcomeDlg::prjCreateErr {details} {

		proc ::WelcomeDlg::prjCreateErrInit {details} { 
			messageBeep -exclamation
			controlTextSet IDD_DIALOG_prjCreateErr.textDetails $details
		}

		# add carriage returns before inserting into the text box.
		regsub -all "\n" $details "\r\n" details
		dialogCreate \
    		-name IDD_DIALOG_prjCreateErr \
    		-title "Project Creation Error" \
    		-width 250 -height 90 \
    		-controls { \
        		{ button -title "Close" -name IDOK_tgtSvr \
            	-callback "windowClose IDD_DIALOG_prjCreateErr" \
            	-xpos 100 -ypos 72 -width 40 -height 14  \
        		} \
				{ label -title "The project creation failed. Details:" \
       			-xpos 5 -ypos 5 -width 150 -height 12  \
				} \
				{ text -name textDetails \
       			-xpos 5 -ypos 15 -width 240 -height 50 \
				-multiline -vscroll -readonly \
				} \
			} \
			-init [list ::WelcomeDlg::prjCreateErrInit $details] 
	}
