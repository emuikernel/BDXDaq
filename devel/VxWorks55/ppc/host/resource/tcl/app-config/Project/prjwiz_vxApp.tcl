# prjwiz_vxApp.tcl - project creation wizard for vxApp-based projects 
#
# modification history
# --------------------
# 02m,18jun02,rbl  fix problem where saved toolchain is wrong arch for this
#                  install
# 02l,23may02,cjs  Fix spr 77715: put make diab toolchain default if
#                  installed, and remember last choice
# 02k,21may02,rbl  fix SPR 77683 -Keyboard navigtion broken in
#                  project wizards.
# 02j,20may02,rbl  sort drop-down list of projects to create from
# 02j,17may02,cjs  Moved projectsEnumerate to Utils.tcl
# 02i,08may02,rbl  make project to copy from location writable
# 02h,31may01,t_m  if sim doesnt exists default to one of the existing 
# 02g,24mar99,rbl  removing finish.bmp icon, adding prjType argument to getControlList 
# 02f,18mar99,rbl  changing wizard text to explain which files are copied and
#                  which are references
# 02e,17mar99,rbl  all choice controls in group must be explicitly initialized (without -noevent flag)
# 02d,16mar99,rbl  adding keyboard shortcuts
# 02c,09mar99,rbl  adding option to add to current, or new/existing workspace
# 02b,08mar99,cjs  Adding iconGet() proc
# 02a,03mar99,rbl  unified code for Project Save As dialog and first sheet of 
#                  Project Creation wizards 
# 01z,25feb99,rbl  Changes to handle missing or read-only projects
# 01y,24feb99,ren    Display native file names on windows
# 01x,21jan99,cjs    Changed params to prjCopy(); now use handle of open project
# 01w,15jan99,cjs    More screening of bad chars in project names
# 01v,02dec98,cjs    fix spr 23341: downloadable app wizard doesn't remember
#                    basis project
# 01u,13nov98,ren    modified tab order in step 1
# 01t,10nov98,cjs    restore status log routine correctly 
# 01s,06nov98,cjs    fixing bug that passes null arg to filePathUnFix() 
# 01r,21oct98,cjs    added project description field; display file paths in
#                    host friendly manner
# 01q,16oct98,cjs    validate basis project type
# 01p,12oct98,cjs    make wizard save workspace following project creation
# 01o,21sep98,cjs    made call to ::WorkspaceDoc::workspaceSave compatible
#                    with changes made to support saveAs; removed call to
#                    workspaceSave(); call ::Workspace::projectAdd()
# 01n,18sep98,cjs    added name() 
# 01m,17sep98,cjs    initialize location variables in one-time only init rtn;
#                    cleanup status in one-time only exit rtn
# 01l,10sep98,cjs    fixed project browse dialog logic; fixed routines
#                    that fetch host-specific into; cleaned up layout 
# 01k,14aug98,cjs    truncate long basis project paths in combo box
# 01j,10aug98,cjs    more cleanup.  Projects listed in combo too.
# 01f,06aug98,cjs    listified controls so that bitmap path will work;
#                    remove current selection in path specification
# 01e,04aug98,jak    cleaned up dir browse, project name, simpc selection
# 01d,01jul98,cjs    increased hight of text controls to make underscores
#                    visible 
# 01c,30jun98,cjs    changed project name/path logic in first screen 
# 01b,18jun98,cjs    added progress reporting; improve name/dir entry logic 
# 01a,21may98,cjs    written.
#
# DESCRIPTION
# This wizard complements the prj_vxApp project creation code.
# It queries the user and creates a new partially-linked, relocatable
# application project in the current workspace (if one is open) or in
# a new workspace that it creates in the project directory.
#	 
#############################################################

namespace eval ::prjwiz_vxApp {
	variable _projectListCache
	variable _projectLocation
	variable _projectName

	variable _basisToolChain
	variable _basisProject
	variable _fullPath
	variable _newOrExistingWorkspaceFileName
	variable _currentOrNewOrExistingWorkspace
	variable _workspaceFileName
	variable _bUseToolChain 1
	variable _comboWidth 36
	variable _description

	proc name {} {
		return [::prj_vxApp::name]
	}

# screen x button x radio button
set myFSM {
	{
		{	-1		}
		{	1		}
		{	-1		}
	}
	{
		{	0		}
		{	2		}
		{	-1		}
	}
	{
		{	0		}
		{	-1		}
		{	0		}
	}
}

set sheetOneControlList [::ProjectSaveAs::getControlList 1 ::prj_vxApp]
set sheetStructList [list \
	[ list \
	$sheetOneControlList \
 	::prjwiz_vxApp::initProc1 ::prjwiz_vxApp::exitProc1 {return 0} \
	] \
	[list \
	[list \
    	[list label -name lbl1 \
			-title "Specify a toolchain for building this Project.\
				This toolchain can be set explicitly, or can be\
				copied from an existing Downloadable Project.\
				\n\n\
				Builds using additional toolchains can be added\
				at a later time." \
			-x 105 -y 22 -w 200 -h 27 \
		] \
		[list label -name lbl3 \
			-title "Would you like to base your project on:" \
			-x 120 -y 80 -w 171 -h 8 ] \
		[list group -name group1 -x 14 -y 86 -w 297 -h 40] \
		[list choice -name projectchoice -title "An &existing project" \
            -auto -x 16 -y 94 -w 90 -h 9 \
		] \
		[list choice -name toolchainchoice -title "A &toolchain" \
            -auto -x 16 -y 138 -w 69 -h 9 \
		] \
		[list group -name group2 -x 14 -y 130 -w 297 -h 24] \
    	[list combo -name projectnamecombo -x 120 -y 94 -w 187 -h 100 \
			-callback {
				set curSel [controlSelectionGet \
					wizardDialog.projectnamecombo -string]
				if {$curSel != ""} {
					filePathUnFix \
                                            ::prjwiz_vxApp::_projectListCache($curSel)
					textControlTextSetKeepingSelection \
                                            wizardDialog.projectnametext \
					    $::prjwiz_vxApp::_projectListCache($curSel)
                                        controlFocusSet wizardDialog.projectnamecombo
				}
			} \
		] \
    	[list text -name projectnametext -x 120 -y 111 -w 170 -h 12 -callback {
	    set fileName [controlTextGet wizardDialog.projectnametext]
            ::prjwiz_vxApp::updateProjectComboControl $fileName
            } \
        ] \
		[list combo -name toolchaincombo -x 120 -y 138 -w 187 -h 100] \
		[list button -name browsebutton -title "..." \
			-x 292 -y 111 -w 14 -h 12 -callback {
				set cwd [pwd]
				catch { cd [file dirname [controlTextGet wizardDialog.projectnametext]] }
				set fileName [join [fileDialogCreate -filefilters \
					"Project Files(*.wpj)|*.wpj|All Files (*.*)|*.*||" \
					-title "Select Project to Copy" -okbuttontitle "Ok"]]
				if {$fileName != ""} {
		                    controlTextSet wizardDialog.projectnametext $fileName
				}
			}
		] \
	] 	::prjwiz_vxApp::initProc2 ::prjwiz_vxApp::exitProc2 {return 0} \
	] \
	[list \
	[list \
    	[list label -name lbl1 \
			-title "The Project Creation Wizard will now create your\
				Downloadable Project." \
			-x 100 -y 4 -w 200 -h 76] \
    	[list frame -name finishframe -title "Summary" \
			-x 7 -y 93 -w 300 -h 88] \
    	[list label -name lblworkspace -title "Workspace" \
			-x 14 -y 106 -w 66 -h 12] \
    	[list label -name lblproject -title "Project" \
			-x 14 -y 120 -w 66 -h 12] \
    	[list label -name lbltoolchain -title "Basis Project or BSP" \
			-x 14 -y 135 -w 66 -h 12] \
    	[list text -name textworkspace \
		-border \
		-readonly -x 87 -y 103 -w 210 -h 12] \
    	[list text -name textproject \
		-border \
		-readonly -x 87 -y 118 -w 210 -h 12] \
    	[list text -name texttoolchain \
		-border \
		-readonly -x 87 -y 133 -w 210 -h 12] \
	] 	::prjwiz_vxApp::initProc3 prjwiz_vxApp::finishProc {return 0} \
	] \
]

proc initProc0 {} {
	variable _projectName
	variable _projectLocation 
	variable _projectListCache
	variable _basisToolChain
	variable _basisProject
	variable _bUseToolChain
	variable _description
	variable _newOrExistingWorkspaceFileName
	variable _currentOrNewOrExistingWorkspace

	set _basisToolChain \
		[defaultBasisToolChainGet [wtxHostType]]
	set _basisToolChain \
		[::prjwiz_vxApp::toolChainGet $_basisToolChain]

        # if we get nothing, try ignoring the saved value - it might 
        # be the wrong arch for this install

        if {$_basisToolChain == ""} {
            set _basisToolChain \
                [defaultBasisToolChainGet [wtxHostType] 1]           
	    set _basisToolChain \
		    [::prjwiz_vxApp::toolChainGet $_basisToolChain]
        }

	set _basisProject \
		[::Workspace::defaultBasisProjectGet \
                ::prj_vxApp [wtxHostType]]
	catch {unset _projectListCache}
	set _projectLocation [::Workspace::newProjectDirGet]
	set _projectName [lindex [file split $_projectLocation] \
		[expr [llength [file split $_projectLocation]] - 1]]
	set _description $::ProjectSaveAs::_initDescriptionText
	set _newOrExistingWorkspaceFileName [::Workspace::newWorkspaceFileNameGet]
	set _currentOrNewOrExistingWorkspace "current"

	# Catch the progress
	::prjStatusLogFuncSet ::WizardCore::progressShow
}

proc initProc1 {} {
	variable _projectName 
	variable _projectLocation 
	variable _description
	variable _newOrExistingWorkspaceFileName
	variable _currentOrNewOrExistingWorkspace

	::ProjectSaveAs::init 1 ::prj_vxApp $_projectName $_projectLocation "$_description" $_newOrExistingWorkspaceFileName $_currentOrNewOrExistingWorkspace

}

proc toolchainChoiceCB {} {
	variable _basisToolChain

	# Populate the combo with the list of toolchains
	set toolChains [::prj_vxApp::toolChainListGet]
	foreach toolchain $toolChains {
		lappend names [${toolchain}::name]
	}
	set names [lsort $names]
	controlValuesSet wizardDialog.toolchaincombo $names
	
	set tcName [${_basisToolChain}::name]
	if {[lsearch $names $tcName] != -1} {
		controlSelectionSet wizardDialog.toolchaincombo \
			-string $tcName
	} else {
		controlSelectionSet wizardDialog.toolchaincombo 0 
	}

	controlEnable wizardDialog.toolchaincombo 1
	controlEnable wizardDialog.projectnamecombo 0
	controlEnable wizardDialog.browsebutton 0
	controlEnable wizardDialog.projectnametext 0
	controlFocusSet wizardDialog.toolchaincombo
}

proc projectChoiceCB {} {
	variable _basisProject
	variable _projectListCache
	variable _comboWidth

	if {![info exists _projectListCache]} {
		set projects [::projectsEnumerate ::prjwiz_vxApp]
		foreach prj $projects {
			set truncName [file tail $prj]
			set _projectListCache($truncName) $prj
		}
	}
	controlValuesSet wizardDialog.projectnamecombo \
	    [lsort [array names _projectListCache]]

	set truncName [file tail $_basisProject]
	if {[lsearch [array names _projectListCache] $truncName] != -1} {
		controlSelectionSet wizardDialog.projectnamecombo -string $truncName 
	} else {
		controlSelectionSet wizardDialog.projectnamecombo 0
	}

	controlEnable wizardDialog.toolchaincombo 0
	controlEnable wizardDialog.projectnamecombo 1
	controlEnable wizardDialog.projectnametext 1
	controlEnable wizardDialog.browsebutton 1
	controlFocusSet wizardDialog.projectnamecombo
}

#########################################################################
#
# updateProjectComboControl - update the project combo control
#
# Takes no action if projectFile does not have extension .wpj or does
# not exist. 
# Also updates cached list of projects.
#
# PARAMETERS 
#   projectFile: full path to project (.wpj) file
#
# RETURNS: N / A
#
# ERRORS: N / A
#
proc updateProjectComboControl {projectFile} {
    variable _projectListCache
    ::filePathFix projectFile
    if {[file extension $projectFile] != ".wpj" || \
        ![file exists $projectFile]} {
        return
    }
    set truncName [file tail $projectFile]
    set nProjectsPrev [array size _projectListCache]
    set _projectListCache($truncName) $projectFile
    if {[array size _projectListCache] > $nProjectsPrev} {
        controlValuesSet wizardDialog.projectnamecombo [lsort [array names \
	    _projectListCache]]
    }
    if {[controlSelectionGet wizardDialog.projectnamecombo -string] \
        != $truncName } {
        controlSelectionSet wizardDialog.projectnamecombo -string $truncName
        controlFocusSet wizardDialog.projectnametext
    } 
}

proc initProc2 {} {
	variable _bUseToolChain
	controlCallbackSet wizardDialog.toolchainchoice \
		::prjwiz_vxApp::toolchainChoiceCB 
	controlCallbackSet wizardDialog.projectchoice \
		::prjwiz_vxApp::projectChoiceCB 

	# Set the focus to the Next button
	::WizardCore::buttonFocusSet 1

	if {$_bUseToolChain == 1} {
		controlCheckSet wizardDialog.projectchoice 0
		controlCheckSet wizardDialog.toolchainchoice 1
	} else {
		controlCheckSet wizardDialog.toolchainchoice 0
		controlCheckSet wizardDialog.projectchoice 1
	}
}

proc initProc3 {} {
	variable _projectName
	variable _projectLocation
	variable _bspDir
	variable _workspaceFileName
	variable _fullPath
	variable _basisToolChain
	variable _basisProject
	variable _bUseToolChain

	if {$_bUseToolChain} {
		set basis "Tool Chain"
		set tcName [${_basisToolChain}::name]
	} else {
		set basis "Basis Project"
		set tcName $_basisProject
	}
	
	controlTextSet wizardDialog.lbltoolchain $basis
	controlTextSet wizardDialog.textworkspace [file nativename $_workspaceFileName]
	controlTextSet wizardDialog.textproject [file nativename $_fullPath]

	# tcName contains either the friendly name of a toolchain, or
	# a project whose toolchain we'll use
	controlTextSet wizardDialog.texttoolchain [file nativename $tcName]

	# Set the focus to the finish button
	::WizardCore::buttonFocusSet 2
}

proc exitProc1 {} {
	variable _fullPath
	variable _projectName
	variable _projectLocation
	variable _description
	variable _newOrExistingWorkspaceFileName
	variable _currentOrNewOrExistingWorkspace
	variable _workspaceFileName

	set retval [::ProjectSaveAs::exit]

	# Get data back from ProjectSaveAs and save these values for next visit to dialog
	set _fullPath [::ProjectSaveAs::getFullPath]
	set _projectName [::ProjectSaveAs::getProjectName]
	set _projectLocation [::ProjectSaveAs::getProjectLocation]
	set _description [::ProjectSaveAs::getDescription]
	set _newOrExistingWorkspaceFileName [::ProjectSaveAs::getNewOrExistingWorkspaceFileName]
	set _currentOrNewOrExistingWorkspace [::ProjectSaveAs::getCurrentOrNewOrExistingWorkspace]
	set _workspaceFileName [::ProjectSaveAs::getWorkspaceFileName]

	return $retval
}

proc exitProc2 {} {
	set retval 0
	variable _projectListCache
	variable _basisToolChain
	variable _basisProject
	variable _bUseToolChain

	if {[controlChecked wizardDialog.toolchainchoice]} {
		set _bUseToolChain 1

		# Map the toolchain's friendly name back to a namespace
		set toolChainName \
			[controlSelectionGet wizardDialog.toolchaincombo -string]
		set _basisToolChain [::prjwiz_vxApp::toolChainGet $toolChainName]
        defaultBasisToolChainSet $toolChainName
		set retval 1
	} else {
		# Obtain and validate the project
		set _bUseToolChain 0
		set _basisProject [controlSelectionGet \
			wizardDialog.projectnamecombo -string]
		set _basisProject $_projectListCache($_basisProject)
		filePathFix _basisProject
		if {![file exists $_basisProject]} {
			messageBox "'$_basisProject' is not a valid project file"
		} else {
			set retval 1
		}

		if {$retval} {

        	# Verify that it is of the correct type
        	if {[catch {::prjOpen $_basisProject "r"} hProj]} {
        	    messageBox "'$_basisProject' is not a valid project file"
        	} else {
        	    set prjType [::prjTypeGet $hProj]
        	    prjClose $hProj
        	    if {![string match "::prj_vxApp" $prjType]} {
					set prjName [file rootname [file tail $_basisProject]]
        	        messageBox -ok -exclamationicon \
						"'$prjName' is not a downloadable application. \
						Please select another project to replicate."
					set retval 0
        	    } else {
                        ::Workspace::defaultBasisProjectSet \
                            $prjType $_basisProject
        	    }
        	}
		}
	}

	# We can do this using a toolchain or another project.
	# Return value indicates which
	return $retval 
}

proc toolChainGet {tcName} {
	set retval ""
	set toolChains [::prj_vxApp::toolChainListGet]
	foreach toolchain $toolChains {
		set name [${toolchain}::name]
		if {[string match $name $tcName]} {
			set retval $toolchain
			break
		} 
	}
	return $retval
}

proc finishProc {} {
	variable _basisToolChain
	variable _basisProject
	variable _fullPath
	variable _workspaceFileName
	variable _bUseToolChain
	variable _description
	set retval ""

	::beginWaitCursor

	if {$_bUseToolChain} {
		# Build project from a bsp
		if {[catch {::prj_vxApp_hidden::projCreate \
			"$_fullPath" "$_basisToolChain" "$_description"} error]} {
			::prjStatusLog ""
	    		::prjStatusLogFuncSet [::defaultPrjStatusLogFuncGet] 
			::endWaitCursor
			error $error
		}
	} else {
		# Build project from _basisProject
		#
		# If basis project is open, decide whether to copy from memory or disk
		#
		set fCopyFromMemory 0
		set hProj [::Workspace::projectHandleGet $_basisProject]
		if {$hProj != ""} { 
			# project open and not dirty - use version in memory
			if {![::prjDirtyGet $hProj]} {
				set fCopyFromMemory 1
			# project open and dirty - user decides which version to use
			} else {
				set reply [messageBox -yesnocancel "$_basisProject:\n\nThis project is open and has been modified. Use the version in memory?"]
				if {$reply == "yes"} {
					set fCopyFromMemory 1
				} elseif {$reply == "cancel"} {
					::WizardCore::onCancelButton				
					return ""
				}
			}
		}
		if {$fCopyFromMemory == 0} {
			set hProj [::prjOpen $_basisProject]
			prjCopy $hProj $_fullPath 1 $_description
			prjClose $hProj
		} else {
			prjCopy $hProj $_fullPath 1 $_description
		}
		::endWaitCursor
	}

	::beginWaitCursor
	# Close workspace if user has chosen to add to a different workspace
	if {[::Workspace::openWorkspaceGet] != "" && \
		[::Workspace::openWorkspaceGet] != "$_workspaceFileName"} {
		if {[::ViewBrowser::queryClose] == 0} {
			return ""
		}
	}

	if {![catch {::Workspace::createOrOpenWorkspace $_workspaceFileName} \
		error]} {
		::prjStatusLog "Saving project..."
		::Workspace::projectAdd $_fullPath

		::prjStatusLog "Saving workspace..."
		::ProjectFacility::guiWorkspaceSave
		::prjStatusLog "Saving workspace...Done"
		::prjStatusLog ""
		set retval $_fullPath
		::endWaitCursor
	} else {
		::endWaitCursor
		error $error
	}
	return $retval
}

proc exitProcN {} {
	::prjStatusLogFuncSet [::defaultPrjStatusLogFuncGet] 
}

proc iconGet {} {
	return IDI_DOWNLOADABLE
}

proc projectWizardRun {} {
	set retval [::WizardCore::WizardRun $::prjwiz_vxApp::myFSM \
		$::prjwiz_vxApp::sheetStructList ::prjwiz_vxApp::initProc0 \
		::prjwiz_vxApp::exitProcN]
	return $retval
}

# Register the wizard
::ViewBrowser::projectWizardRegister "[::prj_vxApp::name]" ::prjwiz_vxApp
}
