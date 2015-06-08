#############################################################
#
# ProjectSaveAs.tcl -- Save As dialog for a project
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01i,08may02,rbl  better handling of bad file characters in project name and
#                  location
# 01h,23mar99,rbl  remove bad file characters from paths
#                  set wizard bitmap for different project types
# 01g,18mar99,rbl  adding text to Project Save As dialog to explain which files 
#                  are copied and which are references
# 01f,17mar99,rbl  all choice controls in group must be explicitly initialized
# 01e,12mar99,rbl  putting project name and location into one group box
#                  adding keyboard shortcuts
#                  adding type-ahead from project location to new/existing 
#                  workspace filename
#                  doing a file nativename in onProjectLocationBrowse so that
#                  the initial directory registers
#                  making the description text box -wantenter
#                  checking for .wsp extension in workspace file name 
# 01d,08mar99,rbl  rearranging dialog and adding option to add to current, or
#                  new/existing workspace
# 01c,05mar99,rbl  Removing some "puts" from exit {}
# 01b,03mar99,rbl  unified code for Project Save As dialog and first sheet of 
#                  Project Creation wizards 
# 01a,19feb99,rbl  created. 
#
# DESCRIPTION
# 
# Implements a "Save As" dialog for a project.
# 
# ENTRY POINT: getNewNameAndDescription, which returns a new name and description for a project.
#
#############################################################

namespace eval ::ProjectSaveAs {
	variable _dialogName "wizardDialog"
	variable _projectLocation ""
	variable _projectName ""
	variable _prjType ""
	variable _fullPath ""
	variable _newOrExistingWorkspaceFileName ""
	variable _workspaceFileName ""
	variable _comboWidth 36
	variable _initDescriptionText "<Enter description here>"
	variable _description ""
	variable _nextbuttonDims ""
    variable _projectSaveAsButtonRow 230
	variable _controlList ""

	#############################################################################
	#
	# getNewNameAndDescription returns a new name and description for a project.
	#
	# RETURNS: list whose first element is the name, second element the description
	#          The name and description are both "" if the user has cancelled.
	#

	proc getNewNameAndDescription {hProj} {
		variable _fullPath
		variable _initDescriptionText
		variable _description
		variable _controlList

		set prjType [prjTypeGet $hProj]
		set projectLocation [::Workspace::newProjectDirGet]
		set projectName [makeNameFromLocation $projectLocation]		
		set newWorkspaceFileName [::Workspace::newWorkspaceFileNameGet]

		set oldPrjName [file tail [prjInfoGet $hProj fileName]]
		# Create the Project Save As window 
		dialogCreate -name projectSaveAsDialog \
			-parent ::ViewBrowser \
			-title "Save Project $oldPrjName As" -noframe \
			-x 100 -y 0 -width 314 -height 250 \
			-init {::ProjectSaveAs::init 0 "$prjType" "$projectName" "$projectLocation" "$_initDescriptionText" "$newWorkspaceFileName" "current"} 
	}

	###########################################################################
	#
	# init initializes the control values from the values passed in. 
	#
	# ARGUMENTS: bWizard: 1 if this is the first sheet of the wizard, 0 if it 
	#            is the Project Save As dialog.
	#            
	#            prjType: ::prj_vxApp or ::prj_vxWorks
	#            
	#            projectName: a default name for the new project
	#
	#            projectLocation: a default location for the new project
	#
	#            description: a default description for the new project
	#
	# RETURNS:   The list of controls
	#
	proc init {bWizard prjType projectName projectLocation description workspaceFileName currentOrNewOrExistingWorkspace} {
		variable _controlList 
		variable _projectName
		variable _projectLocation 
		variable _description
		variable _dialogName
		variable _prjType
		variable _newOrExistingWorkspaceFileName 
		variable _currentOrNewOrExistingWorkspace

		set _prjType $prjType
		set _projectLocation $projectLocation
		set _projectName $projectName		
		set _description $description
		set _newOrExistingWorkspaceFileName $workspaceFileName
		set _currentOrNewOrExistingWorkspace $currentOrNewOrExistingWorkspace

		#
		# Handle differences between Project Save As dialog and the Project
		# Creation wizard.
		#
		# e.g. Create controls unless this is the project creation wizard, 
		# which has already created the controls...
		#
		if {$bWizard == 0} {
			set _dialogName "projectSaveAsDialog"
			getControlList 0 $_prjType
			foreach ctrl $_controlList {
				controlCreate $_dialogName $ctrl
			}
		} else {
			set _dialogName "wizardDialog"
			::WizardCore::buttonFocusSet 1
		}

		controlTextSet ${_dialogName}.textProjectLocation [file nativename $_projectLocation]
		controlTextSet ${_dialogName}.textname $_projectName
		controlTextSet ${_dialogName}.textdescription $_description
		controlTextSet ${_dialogName}.textWorkspaceFileName [file nativename $_newOrExistingWorkspaceFileName]

		#controlTextSet ${_dialogName}.textresult [file nativename [pathCombine $_projectLocation $_projectName]]

		if {[::Workspace::openWorkspaceGet] == ""} {
			controlEnable ${_dialogName}.workspaceChoiceCurrent 0
		}

		if {$_currentOrNewOrExistingWorkspace == "current" &&
			[::Workspace::openWorkspaceGet] != ""} {
			controlCheckSet ${_dialogName}.workspaceChoiceNewOrExisting 0
			controlCheckSet ${_dialogName}.workspaceChoiceCurrent 1
		} else {
			controlCheckSet ${_dialogName}.workspaceChoiceCurrent 0
			controlCheckSet ${_dialogName}.workspaceChoiceNewOrExisting 1
		}
	}		

	proc makeNameFromLocation {location} {
		set name [lindex [file split $location] \
			[expr [llength [file split $location]] - 1]]
		return $name
	}

	proc onEditProjectText {nameOrLocation} {
		variable _dialogName
		variable _newOrExistingWorkspaceFileName

		set projectLocation [controlValuesGet ${_dialogName}.textProjectLocation]
		::filePathFix projectLocation
                set projectName [controlValuesGet ${_dialogName}.textname]
		::filePathFix projectName

                # fix any bad file name characters

                if {$nameOrLocation == "location" && \
                    [regsub -all [::badFileCharsGet] \
                    $projectLocation "_" projectLocation] > 0} {
		    messageBeep -exclamation
                    textControlTextSetKeepingSelection \
                        ${_dialogName}.textProjectLocation \
                        [file nativename $projectLocation]
                    return
                }
                if {$nameOrLocation == "name" && \
                    [regsub -all [::badFileCharsGet] \
                    $projectName "_" projectName] > 0} {
		    messageBeep -exclamation
                    textControlTextSetKeepingSelection \
                        ${_dialogName}.textname [file nativename $projectName]
                    return
                }

                # set the workspace location based on the project location 

		if {[llength [file split $projectLocation]] >= 2} {
			set workspaceLocation [lrange [file split $projectLocation] 0 \
								   [expr [llength [file split $projectLocation]] - 2]]
			set workspaceLocation [eval file join $workspaceLocation]
			if {[file exists $workspaceLocation] && \
				[file isdirectory $workspaceLocation] } {
				set _newOrExistingWorkspaceFileName [::Workspace::newWorkspaceFileNameGet $workspaceLocation]
				if {[controlChecked ${_dialogName}.workspaceChoiceNewOrExisting]} {
					controlTextSet ${_dialogName}.textWorkspaceFileName [file nativename $_newOrExistingWorkspaceFileName]
				}
			}
		}
		#controlValuesSet ${_dialogName}.textresult [getProjectFullPath]
	}

	proc getProjectFullPath {} {
		variable _dialogName

		# fetch text from both the name and location fields
		set location [controlValuesGet ${_dialogName}.textProjectLocation]
		::filePathFix location
		set name [controlValuesGet ${_dialogName}.textname]
		if {$name == ""} {
			set name [makeNameFromLocation $location]
		}
		set result [pathCombine $location $name]
		return $result
	}

	proc pathCombine {location name} {
		::filePathFix location

		# Remove spaces from both 
		regsub -all [::badFileCharsGet] $name "_" name
		regsub -all [::badFileCharsGet] $location "_" location 

		# Make sure project name doesn't end in .wpj
		set name [file rootname $name]

		if {$name != ""} {
			set name [format "%s.wpj" $name]
		}
		set result [file join $location $name]
		return $result
	}

	##########################################################
	#
	# onSaveButton is used only by the Project Save As dialog, 
	# not by the wizard
	#
	proc onSaveButton {} {
		variable _dialogName

		if {[exit 0] == 1} {
			windowClose $_dialogName
		}
	}

	##########################################################
	#
	# onCancelButton is used only by the Project Save As dialog, 
	# not by the wizard
	#
	proc onCancelButton {} {
		variable _dialogName
		variable _fullPath
		variable _projectName

		set _fullPath ""
		set _projectName ""
		windowClose $_dialogName
	}	
	
	#############################################################################################
	#
	# exit saves the control values into _fullPath, _projectName, _projectLocation, _description.
	#
	# These values can then be retrieved using getFullPath, getProjectName, getProjectLocation and 
	# getDescription
	# 
	# RETURNS: 1 if no errors, 0 if there are any errors (e.g. directory not writable)
	#
	proc exit {{bWizard 1}} {
		variable _fullPath
		variable _projectName
		variable _projectLocation
		variable _description
		variable _newOrExistingWorkspaceFileName
		variable _currentOrNewOrExistingWorkspace
		variable _workspaceFileName
		variable _dialogName
		variable _prjType

		set bReturn 0

		# Save these values
		set _projectName [controlTextGet ${_dialogName}.textname]
		set _projectLocation [controlTextGet ${_dialogName}.textProjectLocation]
		::filePathFix _projectLocation

		set result [getProjectFullPath]
		::filePathFix result

		if {$result != ""} {

			set _fullPath $result

			# If this is not the project creation wizard, prompt to overwrite an exisiting project
			if {$bWizard == 0 && \
				[file exists $_fullPath]} {
				if {[catch {::prjOverwritable $_fullPath $_prjType} error]} {
					messageBox $error
				} else {
					set reply [messageBox -yesno "$_fullPath:\n\nProject already exists. Overwrite existing project?"]
					if {$reply == "yes"} {
						set bReturn 1
					}
				}

			# Make sure parent of location is a writable directory
			} elseif {[catch {::prjCreateable $_fullPath $_prjType} error]} {
				messageBox $error
			} else {
				set bReturn 1
			}
		} else {
			messageBox "Please enter a name for your project"
		}
		set _description [controlTextGet ${_dialogName}.textdescription]

		set _newOrExistingWorkspaceFileName [controlTextGet ${_dialogName}.textWorkspaceFileName]
		::filePathFix _newOrExistingWorkspaceFileName
		# Remove spaces from path 
		regsub -all [::badFileCharsGet] $_newOrExistingWorkspaceFileName "_" _newOrExistingWorkspaceFileName 

		if {[controlChecked ${_dialogName}.workspaceChoiceCurrent]} {
			set _currentOrNewOrExistingWorkspace "current"
			set _workspaceFileName [::Workspace::openWorkspaceGet]
		} else {
			set _currentOrNewOrExistingWorkspace "newOrExisting"
			if {[file extension $_newOrExistingWorkspaceFileName] == ""} {
				set _newOrExistingWorkspaceFileName "${_newOrExistingWorkspaceFileName}.wsp"
			} elseif {[file extension $_newOrExistingWorkspaceFileName] != ".wsp"} {
				messageBox "Workspace must have a .wsp extension"
				set bReturn 0
			} 
			set _workspaceFileName $_newOrExistingWorkspaceFileName
		}

		return $bReturn
	}

	proc getFullPath {} {
		variable _fullPath

		return $_fullPath
		}

	proc getProjectName {} {
		variable _projectName

		return $_projectName
		}

	proc getProjectLocation {} {
		variable _projectLocation

		return $_projectLocation
		}

	proc getDescription {} {
		variable _description

		return $_description
		}

	proc getNewOrExistingWorkspaceFileName {} {
		variable _newOrExistingWorkspaceFileName 

		return $_newOrExistingWorkspaceFileName
		}

	proc getCurrentOrNewOrExistingWorkspace {} {
		variable _currentOrNewOrExistingWorkspace

		return $_currentOrNewOrExistingWorkspace
		}

	proc getWorkspaceFileName {} {
		variable _workspaceFileName 

		return $_workspaceFileName
		}

	proc onProjectLocationBrowse {} {
		variable _dialogName

		set initialDir [file nativename [controlTextGet ${_dialogName}.textProjectLocation]]
		set dirName [dirBrowseDialogCreate \
			-initialdir $initialDir \
			-title "Project Directory"]
		if {$dirName != ""} {
			::filePathFix dirName
			# Remove spaces from dirName 
			regsub -all [::badFileCharsGet] $dirName "_" dirName 
			controlTextSet ${_dialogName}.textProjectLocation [file nativename $dirName]
		}
	}

	proc onWorkspaceLocationBrowse {} {
		variable _dialogName

		set workspaceFileName [controlTextGet ${_dialogName}.textWorkspaceFileName]
		# Attempt to change to the directory where we last were...
		set cwd [pwd]
		if {$workspaceFileName != ""} {
			catch { cd [file dirname $workspaceFileName] }
		} 
		set fileFilters "Workspace Files(*.wsp)|*.wsp|All Files (*.*)|*.*||"
		set fileDialogFlags "-noreadonlyreturn"
		set workspaceFileName [eval fileDialogCreate $fileDialogFlags \
			[list -filefilters $fileFilters \
			-title "Select New or Existing Workspace to Add Project to" -okbuttontitle "Ok" ]\
			 ]
		# fileDialogCreate returns a list (with {}) and we only want one name
		set workspaceFileName [join $workspaceFileName]
		::filePathFix workspaceFileName
		# Remove spaces from filename 
		regsub -all [::badFileCharsGet] $workspaceFileName "_" workspaceFileName 

		if {$workspaceFileName != ""} {
			# Make sure workspace file name ends in .wsp
			set workspaceFileName "[file rootname $workspaceFileName].wsp"
			controlTextSet ${_dialogName}.textWorkspaceFileName [file nativename $workspaceFileName]
		}
		# restore current working directory
		cd $cwd
	}

	proc onWorkspaceChoiceCurrent {} {
		variable _dialogName
		variable _newOrExistingWorkspaceFileName

		# save contents of textWorkspaceFileName for the next time the user
		# clicks the "NewOrExisting" radio button
		set _newOrExistingWorkspaceFileName [controlTextGet ${_dialogName}.textWorkspaceFileName]
		::filePathFix _newOrExistingWorkspaceFileName
		controlTextSet ${_dialogName}.textWorkspaceFileName [file nativename [::Workspace::openWorkspaceGet]]

		controlEnable ${_dialogName}.textWorkspaceFileName 0
		controlEnable ${_dialogName}.workspacelocationbrowsebutton 0
	}

	proc onWorkspaceChoiceNewOrExisting {} {
		variable _dialogName
		variable _newOrExistingWorkspaceFileName

		controlEnable ${_dialogName}.textWorkspaceFileName 1
		controlEnable ${_dialogName}.workspacelocationbrowsebutton 1
		controlTextSet ${_dialogName}.textWorkspaceFileName [file nativename $_newOrExistingWorkspaceFileName]
		controlSelectionSet ${_dialogName}.textWorkspaceFileName [list 0 [string length $_newOrExistingWorkspaceFileName]]
		controlFocusSet ${_dialogName}.textWorkspaceFileName
	}

	###########################################################################
	#
	# getControlList returns a list of controls for the Project Save As dialog
	# or the first sheet of the project creation wizard. Buttons (Save and 
	# Cancel) are added only in the case of the Project Save As dialog: the 
	# wizard core creates its own buttons. Other than that the controls are 
	# identical except that in the wizard case we need to make some space for 
	# the wizard "wand" icon in the upper left corner
	#
	# ARGUMENTS: bWizard = 1 if this is the first sheet of the wizard, 0 if it 
	#            is the Project Save As dialog.
	#            prjType is used for the Project Save As dialog (bWizard = 0) since the explanatory
	#            text for the Project Save As dialog is different for different
	#            project types
	#            prjType is also used for the wizard (bWizard = 1) since the bitmaps
	#            in the upper left corner are different for different
	#            project types
	#
	# RETURNS:   The list of controls
	#
	proc getControlList {bWizard {prjType ::prj_vxWorks}} {
		variable _controlList
		variable _dialogName
	    variable _projectSaveAsButtonRow

		#
		# In the wizard we need to shunt the first couple of controls over to the 
		# right to make room for the "wand" icon in the top left corner
		#
		if {$bWizard == 1} {
			set wizardIconOffset 100
		} else {
			set wizardIconOffset 0
		}

		#
		# In the project save as dialog we need to all controls down 
		# to make room for explanatory text at the top
		#
		if {$bWizard == 1} {
			set explainTextOffset 0
		} else {
			if {$prjType == "::prj_vxWorks"} {
				set explainText "Your Project will be saved in the new Project directory.\
					Only those files which are dynamically generated (linkSyms.c,\
					prjComps.h, prjConfig.c, prjParams.h, and usrAppInit.c)\
					will be copied. All other Source Files will simply be references\
					back to their original location."
				set explainTextOffset 30
			} else {
				set explainText "Your Project will be saved in the new Project directory, but all\
					Source Files will continue to reference their original location."
				set explainTextOffset 20
			}
		}

		set _controlList [ list \
			[list group -name grpname -title "Project" \
				-x [expr $wizardIconOffset + 4] -y [expr $explainTextOffset + 4] -w [expr 306 - $wizardIconOffset] -h 78] \
				[list label -name labelProjectName \
					-x [expr $wizardIconOffset + 9] -y [expr $explainTextOffset + 17] -w 35 -h 9 \
					-title "N&ame:"] \
				[list text -name textname \
					-border \
					-x [expr $wizardIconOffset + 9] -y [expr $explainTextOffset + 29] -w [expr 291 - $wizardIconOffset] -h 12 \
					-callback {::ProjectSaveAs::onEditProjectText name}] \
				[list label -name labelProjectLocation \
					-x [expr $wizardIconOffset + 9] -y [expr $explainTextOffset + 47] -w 35 -h 12 \
					-title "&Location:"] \
				[list text -name textProjectLocation \
					-border \
					-x [expr $wizardIconOffset + 9] -y [expr $explainTextOffset + 62] -w [expr 274 - $wizardIconOffset] -h 12 \
					-callback {::ProjectSaveAs::onEditProjectText location}] \
				[list button -name projectlocationbrowsebutton -title "..." \
					-x 286 -y [expr $explainTextOffset + 62] -w 14 -h 12 -callback ::ProjectSaveAs::onProjectLocationBrowse ] \
			[list group -name grpdescription \
				-title "Project &description (optional)" \
				-x 4 -y [expr $explainTextOffset + 86] -w 306 -h 43] \
				[list text -name textdescription \
					-border \
					-x 9 -y [expr $explainTextOffset + 100] -w 294 -h 24 -multiline -vscroll -wantenter] \
			[list group -name grpworkspace -title "Workspace" \
				-x 4 -y [expr $explainTextOffset + 133] -w 306 -h 57] \
				[list choice -title "Add &to current Workspace" -name workspaceChoiceCurrent \
					-auto -newgroup \
					-callback ::ProjectSaveAs::onWorkspaceChoiceCurrent \
					-xpos 12 -y [expr $explainTextOffset + 145] -width 130 -height 10  ] \
				[list choice -title "Add to a New or &Existing Workspace" -name workspaceChoiceNewOrExisting \
					-callback ::ProjectSaveAs::onWorkspaceChoiceNewOrExisting \
					-auto -xpos 12 -y [expr $explainTextOffset + 159] -width 130 -height 10 ] \
				[list text -name textWorkspaceFileName \
					-border \
					-x 49 -y [expr $explainTextOffset + 173] -w 234 -h 12 ] \
				[list button -name workspacelocationbrowsebutton -title "..." \
					-x 286 -y [expr $explainTextOffset + 173] -w 14 -h 12 -callback ::ProjectSaveAs::onWorkspaceLocationBrowse ] \
		]

		# 
		# If this is the "Project Save As" dialog rather than the wizard, we need save and 
		# cancel buttons and some extra explanatory text.
		# The wizard inserts its own "Help Cancel Back Next Finish" buttons
		#
		if {$bWizard == 0} {
			lappend _controlList [list label -name labelExplainText \
					-x 4 -y 4 -w 306 -h [expr $explainTextOffset - 2] \
					-title "$explainText"] 
			lappend _controlList [list button -name _nextbutton -title "&Save" \
						 -x 204 -y $_projectSaveAsButtonRow -w 50 -h 14 \
						-defaultbutton -callback ::ProjectSaveAs::onSaveButton]
			lappend _controlList [list button -name cancelbutton -title "&Cancel" \
					-x 260 -y $_projectSaveAsButtonRow -w 50 -h 14 \
					-callback ::ProjectSaveAs::onCancelButton]
		#
		# If this is the wizard, need to set the wizard bitmaps for the upper left corner...
		#
		} else {
			if {$prjType == "::prj_vxWorks"} {
				lappend _controlList [list bitmap -name bmpX \
					-title "[wtxPath host resource bitmaps Project]/BootableWizard.bmp"]
			} else {
				lappend _controlList [list bitmap -name bmpX \
					-title "[wtxPath host resource bitmaps Project]/DownloadableWizard.bmp"]
			}
		}

		return $_controlList
	}
}
