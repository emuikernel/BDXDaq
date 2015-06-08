# DepDlg.tcl - component dependency dialog 
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 02t,09may02,cjs  Fix SPRs 75901, 76221
# 02s,29mar99,jak  remove kbd shortcuts from OK and Cancel
# 02r,17mar99,cjs  Added, but didn't enable, support for symbol search.
#                  Changed labels in search dialog. 
# 02q,17mar99,cjs  Fix cancel button for Find dialog
# 02p,16mar99,cjs  Added support for finding component params, selections,
#                  folders
# 02o,16mar99,ren  Making OK button the default button.
# 02n,05mar99,cjs  Fixing path used in component find dialog color clearing
#                  logic
# 02m,18feb99,jak  add more -helpbutton s
# 02l,01feb99,ren  Adding help button.
# 02k,26oct98,cjs  change all references to "Add" and "Remove" for
#                  components and folders to "Include" and "Exclude" 
# 02j,19oct98,cjs  add "explain" panel to autoscale 
# 02i,14oct98,cjs  change misleading size info 
# 02h,09oct98,cjs  remove right-hand listbox for autoscale 
# 02g,30sep98,cjs  added dialog for clearing highlight after component
#                  find 
# 02f,18aug98,cjs  now call only workspaceOpen(), workspaceClose() 
# 02e,13aug98,cjs  bug in autoscale prevented green redraw
# 02d,04aug98,cjs  added component search support 
# 02c,04aug98,cjs  added autoscale support dialogs 
# 02b,17jul98,cjs  Changed language on dep dlg. 
# 02a,14jul98,cjs  substantial rewrite.  No more checkboxes, no
#                  more dep calculation logic.  Added FolderDlg.
# 01a,07jul98,cjs  extracted from ComponentAddDlg.tcl, extended to
#                  accomodate selections and autoscale
#
# DESCRIPTION
# This file is sourced by the component view server.  depDlgShow() is
# used to show dependencies for component add, component delete,
# and selection changes. folderDlgShow(), similarly, shows the user
# the default components in the folder that will be added
# or removed.  In this case, they have the chance to modify the list.

#############################################################

source [wtxPath host resource tcl app-config Project]AutoScale.tcl

namespace eval ::ComponentViewServer::gui::DepDlg {
	variable _retCode
	variable _mode

	variable _autoScaleInfo

	# The dialog's return value indicates the user's response:
	# 'ok' or 'cancel'
	proc depDlgShow {mode addList addDepList rmList rmDepList \
		deltaSize newSize} {
		variable _componentList
		variable _autoScaleInfo
		variable _mode $mode
		
		set dlgInfo [depDlgSizeControls_$mode $rmList]
		set title [lindex $dlgInfo 0]
		set w [lindex [lindex $dlgInfo 1] 0]
		set h [lindex [lindex $dlgInfo 1] 1]
		set ctrlList [lindex $dlgInfo 2]

		set ns ::ComponentViewServer::gui::DepDlg
		dialogCreate -name compDepDlg -w $w -h $h -title $title \
			-init [list ${ns}::depDlgInit $addList $addDepList \
				$rmList $rmDepList $deltaSize $newSize] \
			-controls $ctrlList 
	}

	proc depDlgSizeControls_add {{param ""}} {
		set ns ::ComponentViewServer::gui::DepDlg
		set title "Include Component(s)"
		set caption1 "The following components will be included"
		set caption2 "NOTE: The following components are required \
			by the components to the left and will also be included"
		set ctrlList [list \
			[list list -name list1 \
				-x 6 -y 41 -w 131 -h 71] \
			[list list -name list2 -x 145 -y 41 -w 131 -h 71] \
			[list button -name okbutton -title OK -x 51 -y 153 \
				-w 50 -h 14 -callback ${ns}::OnOk -defaultbutton] \
			[list button -name cancelbutton -title Cancel \
				-x 117 -y 153 -w 50 -h 14 \
				-callback ${ns}::depDlgOnCancel] \
			[list button -name helpbutton -title &Help \
				-x 183 -y 153 -w 50 -h 14 -helpbutton] \
			[list table -name sizetable \
				-border -columns 4 -columnheader \
				-x 12 -y 119 -w 125 -h 28] \
			[list table -border -name totalsizetable -columns 4 \
				-columnheader -x 146 -y 119 -w 125 -h 28] \
			[list group -name sizegroup \
				-title "Image size change (bytes):" \
				-x 6 -y 108 -w 136 -h 42] \
			[list label -name list1label \
				-title $caption1  -x 8 -y 14  -w 131 -h 24] \
			[list label -name list2label \
				-title $caption2 -x 145 -y 14 -w 131 -h 24] \
			[list group -name totalsizegroup \
				-title "Size of included components (bytes):" \
				-x 140 -y 108 -w 136 -h 42] \
		]
		return [list $title {286 175} $ctrlList]
	}

	proc depDlgSizeControls_remove {{param ""}} {
		set ns ::ComponentViewServer::gui::DepDlg
		set title "Exclude Component(s)"
		set caption1 "The following components will be EXCLUDED"
		set caption2 "WARNING: The following components depend on the \
			components to the left and will also be EXCLUDED"
		set ctrlList [list \
			[list list -name list1 \
				-x 6 -y 41 -w 131 -h 71] \
			[list list -name list2 -x 145 -y 41 -w 131 -h 71] \
			[list button -name okbutton -title OK -x 51 -y 153 \
				-w 50 -h 14 -callback ${ns}::OnOk] \
			[list button -name cancelbutton -title Cancel \
				-x 117 -y 153 -w 50 -h 14 \
				-callback ${ns}::depDlgOnCancel] \
			[list button -name helpbutton -title &Help \
				-x 183 -y 153 -w 50 -h 14 -helpbutton] \
			[list table -name sizetable \
				-border -columns 4 -columnheader \
				-x 12 -y 119 -w 125 -h 28] \
			[list table -border -name totalsizetable -columns 4 \
				-columnheader -x 146 -y 119 -w 125 -h 28] \
			[list group -name sizegroup \
				-title "Image size change (bytes):" \
				-x 6 -y 108 -w 136 -h 42] \
			[list label -name list1label \
				-title $caption1  -x 8 -y 14  -w 131 -h 24] \
			[list label -name list2label \
				-title $caption2 -x 145 -y 14 -w 131 -h 24] \
			[list group -name totalsizegroup \
				-title "Size of included components (bytes):" \
				-x 140 -y 108 -w 136 -h 42] \
		]
		return [list $title {286 175} $ctrlList]

	}
	proc depDlgSizeControls_select {{param ""}} {
		set ns ::ComponentViewServer::gui::DepDlg
		set title "Selection Component(s) Changed"
		set caption1 "Include the following components to support \
			your selection change?"
		set caption2 "WARNING: The following components will be \
			EXCLUDED to support your selection change"	
		set ctrlList [list \
			[list list -name list1 \
				-x 6 -y 41 -w 131 -h 71] \
			[list list -name list2 -x 145 -y 41 -w 131 -h 71] \
			[list button -name okbutton -title OK -x 51 -y 153 \
				-w 50 -h 14 -callback ${ns}::OnOk] \
			[list button -name cancelbutton -title Cancel \
				-x 117 -y 153 -w 50 -h 14 \
				-callback ${ns}::depDlgOnCancel] \
			[list button -name helpbutton -title Help \
				-x 183 -y 153 -w 50 -h 14 -helpbutton] \
			[list table -name sizetable \
				-border -columns 4 -columnheader \
				-x 12 -y 119 -w 125 -h 28] \
			[list table -border -name totalsizetable -columns 4 \
				-columnheader -x 146 -y 119 -w 125 -h 28] \
			[list group -name sizegroup \
				-title "Image size change (bytes):" \
				-x 6 -y 108 -w 136 -h 42] \
			[list label -name list1label \
				-title $caption1  -x 8 -y 14  -w 131 -h 24] \
			[list label -name list2label \
				-title $caption2 -x 145 -y 14 -w 131 -h 24] \
			[list group -name totalsizegroup \
				-title "Size of included components (bytes):" \
				-x 140 -y 108 -w 136 -h 42] \
		]
		return [list $title {286 175} $ctrlList]
	}

	proc depDlgSizeControls_autoscale {rmList} {
		set ns ::ComponentViewServer::gui::DepDlg
		set ns2 ::ComponentViewServer::gui::AutoScaleExplainDlg
		set title "Auto Scale -- Components to Include"
		set caption1 "The following components are required by \
			your BSP and application source code and will be included"
		set caption2 ""
		set ctrlList [list \
			[list list -name list1 \
				-x 6 -y 41 -w 131 -h 71] \
			[list button -name okbutton -title OK -x 7 -y 153 \
				-w 50 -h 14 -callback ${ns}::OnOk] \
			[list button -name cancelbutton -title Cancel \
				-x 75 -y 153 -w 50 -h 14 \
				-callback ${ns}::depDlgOnCancel] \
			[list button -name explainbutton -x 205 -y 153 \
				-w 50 -h 14 -title "More info..." \
				-callback [list ${ns2}::autoScaleExplain $rmList]] \
			[list button -name helpbutton -title Help \
				-x 140 -y 153 -w 50 -h 14 -helpbutton] \
			[list table -name sizetable \
				-border -columns 4 -columnheader \
				-x 12 -y 119 -w 125 -h 28] \
			[list table -border -name totalsizetable -columns 4 \
				-columnheader -x 146 -y 119 -w 125 -h 28] \
			[list group -name sizegroup \
				-title "Image size change (bytes):" \
				-x 6 -y 108 -w 136 -h 42] \
			[list label -name list1label \
				-title $caption1  -x 8 -y 14  -w 131 -h 24] \
			[list group -name totalsizegroup \
				-title "Size of included components (bytes):" \
				-x 140 -y 108 -w 136 -h 42] \
		]
		return [list $title {286 175} $ctrlList]
	}

	proc depDlgInit {addList addDepList rmList rmDepList deltaSize newSize} {
		variable _mode

		switch $_mode {
			add {
				if {$addDepList == ""} {
					set addDepList [list "<No dependent components>"]
				}
				controlValuesSet compDepDlg.list1 $addList	
				controlValuesSet compDepDlg.list2 $addDepList	
				controlFocusSet compDepDlg.okbutton
			}
			remove {
				if {$rmDepList == ""} {
					set rmDepList [list "<No dependent components>"]
				}
				controlValuesSet compDepDlg.list1 $rmList	
				controlValuesSet compDepDlg.list2 $rmDepList	
				controlFocusSet compDepDlg.cancelbutton
			}
			select {
				controlValuesSet compDepDlg.list1 \
					[concat $addList $addDepList]
				controlValuesSet compDepDlg.list2 \
					[concat $rmList $rmDepList]
				controlFocusSet compDepDlg.okbutton
			}
			autoscale {
				set ns2 ::ComponentViewServer::gui::DepDlg
				controlValuesSet compDepDlg.list1 \
					[concat $addList $addDepList]
				controlFocusSet compDepDlg.okbutton
			}
		}
		controlPropertySet compDepDlg.sizetable \
			-columnwidths [list 30 30 30 32]
		controlPropertySet compDepDlg.sizetable -columnheaders \
			[list "text" "data" "bss" "total"]
		controlValuesSet compDepDlg.sizetable [list $deltaSize]

		controlPropertySet compDepDlg.totalsizetable \
			-columnwidths [list 30 30 30 32]
		controlPropertySet compDepDlg.totalsizetable -columnheaders \
			[list "text" "data" "bss" "total"]
		controlValuesSet compDepDlg.totalsizetable [list $newSize]
	}

	proc OnOk {} {
		variable _retCode
		set _retCode ok 
		windowClose compDepDlg
	}

	proc depDlgOnCancel {} {
		variable _retCode
		set _retCode cancel 
		windowClose compDepDlg
	}
}

namespace eval ::ComponentViewServer::gui::FolderDlg {
	variable _retCode
	variable _mode
	variable _componentList

	# The dialog's return value indicates the user's response:
	# 'ok' or 'cancel'
	proc folderDlgShow {mode folderName compList compListDefault} {
		variable _componentList
		variable _mode $mode
		
		switch $mode {
			add {
				set title "Include Folder"
				set caption1 "Checked components from this folder \
					will be included."
			}
			remove {
				set title "Exclude Folder"
				set caption1 "Checked components from this folder \
					will be removed."
			}
			default {
				set title "Bad mode"
				set caption1 "caption1"
				set caption2 "caption2"
				error "Bad mode specified for folderDlgShow"
			}
		}

		# Create the dialog
		set ns ::ComponentViewServer::gui::FolderDlg
		dialogCreate -name folderDlg -title $title -w 234 -h 153 \
			-controls [list \
				[list button -name okbutton -title "OK" \
					-x 174 -y 42 -w 50 -h 14 -callback ${ns}::OnOk] \
				[list button -name cancelbutton -title "Cancel" \
					-x 174 -y 60 -w 50 -h 14 -callback ${ns}::OnCancel] \
				[list checklist -name checklist -x 6 -y 41 -w 160 -h 103] \
				[list label -name lbl1 -title $caption1 \
					-x 8 -y 20 -w 184 -h 19] \
				[list button -name helpbutton -title "&Help" \
					-x 174 -y 131 -w 50 -h 14 -helpbutton ] \
				[list label -name lbl2 -title Folder \
					-x 7 -y 7 -w 23 -h 10] \
				[list text -name foldernametext -readonly \
					-x 36 -y 7 -w 129 -h 12] \
			] \
			-init [list ${ns}::folderDlgInit $folderName \
				$compList $compListDefault]
	}				

	proc folderDlgInit {folderName compList compListDefault} {
		foreach default $compListDefault {
			lappend checklist [list $default 1]
		}
		foreach comp $compList {
			lappend checklist [list $comp 0]
		}
		if {![info exists checklist]} {
			set checklist ""
		}
		controlValuesSet folderDlg.checklist $checklist
		controlTextSet folderDlg.foldernametext $folderName
		controlFocusSet folderDlg.okbutton
	}

	proc OnOk {} {
		variable _retCode
		variable _componentList
		catch {unset _componentList}
		foreach item [controlValuesGet folderDlg.checklist] {
			if {[lindex $item 1] == 1} {
				lappend _componentList [lindex $item 0]
			}
		}
		if {![info exists _componentList]} {
			set _componentList ""
		}

		set _retCode ok 
		windowClose folderDlg
	}

	proc OnCancel {} {
		variable _retCode
		variable _componentList
		set _retCode cancel 
		set _componentList ""
		windowClose folderDlg
	}
}

namespace eval ::ComponentViewServer::gui::AutoScaleAddDlg {
	variable _retCode

	proc autoScaleAddDlgShow {} {
		variable _retCode
		set _retCode [messageBox -okcancel -informationicon \
			"Auto Scale will check to see if your code\n\
			calls any components that are not in your configuration.\n\
			This may take some time.  To perform this check, press OK."]
	}
}

namespace eval ::ComponentViewServer::gui::AutoScaleRmDlg {

	# The dialog's return value indicates the user's response:
	# 'ok' or 'cancel'
	proc autoScaleRmDlgShow {prjName notrequired} {

		set result [messageBox -okcancel -informationicon \
			"Auto Scale will check to see if there are any\n\
			components in your configuration that are not\n\
			directly called by your code.  To continue, press OK."]

		# Create the dialog
		if { $result == "ok" } {
			::ComponentViewServer::selectListSet $prjName $notrequired
			set ns ::ComponentViewServer::gui::AutoScaleRmDlg
			dialogCreate -name AutoScaleRmDlg \
				-w 258 -h 114 \
				-title "Auto Scale -- Component Removal" \
				-modeless \
				-controls [list \
					[list button -name okbutton -title OK \
						-callback ${ns}::OnOk \
						-x 61 -y 93 -w 50 -h 14] \
					[list button -name cancelbutton -title Cancel \
						-callback ${ns}::OnCancel \
						-x 143 -y 93 -w 50 -h 14] \
					[list label -name lbl1 -title xxx -x 7 -y 7 -w 242 -h 76] \
				]
			OnOk
		}
	}

	proc OnOk {} {
		set buttonText [controlTextGet AutoScaleRmDlg.okbutton]

		switch $buttonText {
			"OK" {
				controlTextSet AutoScaleRmDlg.okbutton Close 
				controlHide AutoScaleRmDlg.cancelbutton 1
				set pos [controlPositionGet AutoScaleRmDlg.okbutton]
				controlPositionSet AutoScaleRmDlg.okbutton \
					[expr [lindex $pos 0] + 40] [lindex $pos 1]

				controlTextSet AutoScaleRmDlg.lbl1 "Folders shown in \
				GREEN contain components which have also been colored green. \
				These components are not directly called by your \
				configuration.  You MAY be able to remove them. \
				\n\nWARNING: these components may still be needed in order \
				for your application to work! \
				\n\nWhen you are done examining and/or deleting components, \
				press Close to remove the auto scale highlighting."

				# Convert the components to paths, make those paths visible,
				# and change the coloration of the nodes to white on black 
				::beginWaitCursor
				set obj [::Workspace::selectionGet]
				set prjObj [::Object::evaluate $obj Project]
				set prjName [::Object::evaluate $prjObj Name]
				set workspaceName [::Workspace::openWorkspaceGet]
				::ViewBrowser::OnWatch ::ComponentViewServer::gui \
					[list $workspaceName $prjName]
				::endWaitCursor
			}
			"Close" {
				cleanUp 0
				windowClose AutoScaleRmDlg
			}
		}
	}

	proc OnCancel {} {
		cleanUp 1
		windowClose AutoScaleRmDlg
	}
	
	proc cleanUp {bAbort} {
		set obj [::Workspace::selectionGet]
		set prjObj [::Object::evaluate $obj Project]
		set prjName [::Object::evaluate $prjObj Name]
		::ComponentViewServer::selectListClear $prjName

		# If user aborts before we redraw in green, no need to update
		if {!$bAbort} {
			if {$prjName != ""} {
				set workspaceName [::Workspace::openWorkspaceGet]
				set cookieLists [list $workspaceName $prjName]
				::ViewBrowser::OnWatch ::ComponentViewServer::gui $cookieLists
			}
		}
	}
}

namespace eval ::ComponentViewServer::gui::ComponentFindDlg {
	variable _macroId
	variable _objectType

	proc componentFindDlgShow {prjName} {
		set ns ::ComponentViewServer::gui::ComponentFindDlg 

		dialogCreate -name ComponentFindDlg -title "Find Object" \
			-w 224 -h 98 -init ${ns}::componentFindDlgInit \
			-controls [list \
				[list label -name lbl1 -title \
    				"Select the type of object to search\
					for, then enter a macro name and press\
					Find to locate the corresponding object." \
                    -x 7 -y 8 -w 203 -h 18] \
    			[list label -name lbl3 -right -title "&Type" \
					-x 8 -y 30 -w 32 -h 10] \
				[list combo -name combofilter -sort \
					-x 46 -y 29 -w 164 -h 56 -callback \
					[list ${ns}::onSelChangeFilterCombo $prjName]] \
    			[list label -name lbl2 -right -title "&Object" \
					-x 8 -y 44 -w 32 -h 10] \
				[list combo -name combofind -editable -sort \
					-x 46 -y 43 -w 164 -h 56] \
    			[list button -default -name okbutton -title Find \
					-x 60 -y 77 -w 50 -h 14 \
					-callback [list ${ns}::OnFind $prjName]] \
    			[list button -name cancelbutton -title Cancel \
					-x 114 -y 77 -w 50 -h 14 \
					-callback [list ${ns}::OnCancel $prjName]] \
    			[list button -name helpbutton -title &Help \
					-x 168 -y 77 -w 50 -h 14 \
				    -helpbutton ]
			]
	}

	proc componentFindDlgInit {} {
		::beginWaitCursor
# Enable the following at some future time
#		set objectTypes {Component Parameter Folder Selection Module Symbol}
		set objectTypes {Component Parameter Folder Selection}
		controlValuesSet ComponentFindDlg.combofilter \
			$objectTypes
		controlSelectionSet ComponentFindDlg.combofilter -string \
			[lindex $objectTypes 0]
		::endWaitCursor
	}

	proc onSelChangeFilterCombo {prjName} {
		set objectType [controlTextGet ComponentFindDlg.combofilter]
		::Workspace::statusMessagePush "Enumerating ${objectType}s..."
		::beginWaitCursor
		switch $objectType {
			"Symbol" {
				set hProj [::Workspace::projectHandleGet $prjName]
				set buildSpec [::prjBuildCurrentGet $hProj]
				set toolchain [prjBuildTcGet $hProj $buildSpec]
				set doc [::Workspace::mxrDocGet $prjName]
				set objects [$doc instances $objectType]
				set objects [cSymsDemangle $objects $toolchain]
			}
			default {
				set doc [::Workspace::cxrDocGet $prjName]
				set objects [$doc instances $objectType]
			}
		}
		controlValuesSet ComponentFindDlg.combofind $objects 
		controlSelectionSet ComponentFindDlg.combofind 0 
		::endWaitCursor
		::Workspace::statusMessagePush "Enumerating ${objectType}s...Done"
	}

	proc OnFind {prjName} {
		variable _macroId
		variable _objectType
		set _macroId \
			[controlTextGet ComponentFindDlg.combofind]
		set objectType [controlSelectionGet \
			ComponentFindDlg.combofilter -string]
		set _objectType $objectType
		set originalMacro $_macroId

		# Validate that we have a component, folder, selection, or param
		switch $objectType {
			"Symbol" {
				set hProj [::Workspace::projectHandleGet $prjName]
				set buildSpec [::prjBuildCurrentGet $hProj]
				set toolchain [prjBuildTcGet $hProj $buildSpec]
				set cDoc [::Workspace::cxrDocGet $prjName]
				set mDoc [::Workspace::mxrDocGet $prjName]
				set mSet [$mDoc setCreate]
				set cSet [$cDoc setCreate]
				set symbol $_macroId
				set _macroId [cSymsMangle $_macroId $toolchain]
				$mSet = $_macroId
				$mSet = [$mSet get exportedBy]
				set module [$mSet instances]
				$cSet = $module 
				$cSet = [$cSet get _MODULES]
				set _macroId [$cSet instances]
				if {$_macroId == ""} {
					messageBox -ok "Sorry, symbol '$symbol' maps to\
					\nmodule '$module', which doesn't map\
					\nto a component."
				}
				$cSet delete
				$mSet delete
			}
			"Parameter" {
				set _macroId [string toupper $_macroId]
				set cDoc [::Workspace::cxrDocGet $prjName]
				set cSet [$cDoc setCreate]
				$cSet = $_macroId
				$cSet = [$cSet get _CFG_PARAMS]
				set _macroId [$cSet instances]
				$cSet delete
			}
			default {
				set _macroId [string toupper $_macroId]
			}
		}
		if {$_macroId != ""} {
			if {[llength $_macroId] > 1} {
				messageBox -ok "The following items were found:\
				\n${_macroId}.\nThe first will be shown."
				set _macroId [lindex $_macroId 0]
			}
			windowClose ComponentFindDlg
		} else {
			messageBox -ok "Item '$originalMacro' not found"
		}
	} 

	proc OnCancel {prjName} {
		variable _macroId
		variable _objectType
		set _macroId ""
		set _objectType ""
		windowClose ComponentFindDlg
	}

	proc findShow {} {

		# Create the dialog
		set ns ::ComponentViewServer::gui::ComponentFindDlg
		dialogCreate -name componentFindDlgDone \
			-w 120 -h 44 \
			-title "Component(s) Found" \
			-modeless \
			-controls [list \
				[list button -name okbutton -title OK \
					-callback ${ns}::onDone \
					-x 35 -y 29 -w 50 -h 14] \
				[list label -name lbl1 -title \
					"Click OK to restore highlighted \
					\nitems to their normal state" \
					-x 7 -y 7 -w 140 -h 20] \
			]
	}

	proc onDone {} {
		::beginWaitCursor
		set obj [::Workspace::selectionGet]
		set prjObj [::Object::evaluate $obj Project]
		set prjName [::Object::evaluate $prjObj Name]
		::ComponentViewServer::selectListClear $prjName
		set workspace [::Workspace::openWorkspaceGet]
		::ViewBrowser::OnWatch ::ComponentViewServer::gui \
			[list $workspace $prjName]
		::endWaitCursor
		windowClose componentFindDlgDone
	}
}
