# prjwiz_vxWorks.tcl - project creation wizard for vxWorks-based projects 
#
# modification history
# --------------------
# 02n,18jun02,rbl  fix problem where saved bsp has wrong arch for current
#                  install
# 02m,23may02,cjs  Fix spr 77715: put make diab toolchain default if
#                  installed, and remember last choice
# 02l,21may02,rbl  fix SPR 77683 -Keyboard navigtion broken in
#                  project wizards.
# 02k,17may02,cjs  Move bspsEnumerate() from prjwiz_vxWorks.tcl to Utils.tcl
# 02j,08may02,rbl  make bsp location and project location fields writable so
#                  that browse button (very slow on UNIX) doesn't need to be
#                  used
# 02i,15oct01,rbl  allow user to override default TOOL when creating
#                  bootable project from bsp
# 02h,23mar99,rbl  shortening text on second wizard screen
#                  removing finish.bmp icon, adding prjType argument to getControlList 
# 02g,18mar99,rbl  changing wizard text to explain which files are copied and
#                  which are references
# 02f,17mar99,rbl  all choice controls in group must be explicitly initialized 
#                  (without -noevent flag) for tabbing to work correctly. 
#                  also needed to enable and disable bspnametext and projectnametext
# 02e,12mar99,rbl  -initialdir argument to dirBrowseDialogCreate must be
#                  in native format.
#                  added keyboard shortcuts, increased height of lbl1 in second
#                  wizard screen to fix SPR 25687
# 02d,09mar99,rbl  adding option to add to current, or new/existing workspace
# 02c,08mar99,cjs  Adding iconGet() proc
# 02b,03mar99,rbl  unified code for Project Save As dialog and first sheet of 
#                  Project Creation wizards 
# 02a,02mar99,ren  Adding more info text.
# 01z,01mar99,rbl  Fix projectChoiceCB to enable projectnamebrowsebutton in
#                  all cases
# 01y,25feb99,rbl    Changes to handle missing or read-only projects
# 01x,25feb99,cjs  Use new bootrom param to vxworks project create routine
#                  in the future
# 01w,24feb99,ren    native file names on Windows
# 01v,21jan99,cjs  changed params to prjCopy(); now use handle of open project
# 01u,18nov98,cjs    fix problems in bsp selection logic 
# 01t,12nov98,ren    adjusted layout to accomodate status bargraph
# 01s,10nov98,cjs    restore status log routine correctly 
# 01r,06nov98,cjs    fixing bug that passes null arg to filePathUnFix() 
# 01q,26oct98,cjs    fixed default BSP selection 
# 01p,21oct98,cjs    added project description field; display file paths in
#                    host-friendly manner
# 01o,16oct98,cjs    validate basis project type
# 01n,12oct98,cjs    make wizard save workspace following project creation
# 01s,21sep98,cjs    made call to ::WorkspaceDoc::workspaceSave compatible
#                    with changes made to support saveAs; removed call to
#                    workspaceSave(); call ::Workspace::projectAdd()
# 01r,18sep98,cjs    added name() 
# 01q,17sep98,cjs    initialize location variables in one-time only init rtn;
#                    cleanup status in one-time only exit rtn
# 01p,10sep98,cjs    fixed project browse dialog logic; fixed routines
#                    that fetch host-specific into; cleaned up layout 
# 01o,09sep98,cjs    fixed minor bug in projectsEnumerate() 
# 01n,20aug98,cjs    if truncated, full bsp name must be used
# 01m,19aug98,cjs    re-implemented sort for projects and bsps, lost in 01k 
# 01l,19aug98,cjs    fixed bug in last checkin 
# 01k,14aug98,cjs    truncate long basis project and bsp paths in combo box
# 01j,10aug98,cjs    more cleanup.  Projects listed in combo too.
# 01i,06aug98,cjs    listified controls so that bitmap path will work;
#                    get button disable logic working correctly; remove
#                    current selection in path specification
# 01h,04aug98,jak    cleaned up dir browse, project name, simpc selection
# 01g,01jul98,cjs    increased hight of text controls to make underscores
#                    visible 
# 01f,30jun98,cjs    changed project name/path logic in first screen 
# 01e,18jun98,cjs    added progress reporting; improve name/dir entry logic 
# 01d,08mar98,cjs    fixed ref to bspDirValid() 
# 01c,07mar98,cjs    more cleanup; fixed creation from existing project 
# 01b,08apr98,cjs    cleaned up and modified to work with new
#						wizard.
# 01a,27mar98,cjs    written.
#
# DESCRIPTION
# This wizard complements the prj_vxWorks project creation code.
# It queries the user and creates a new vxWorks project in the
# current workspace (if one is open) or in a new workspace that
# it creates in the project directory.
#	 
#############################################################

namespace eval ::prjwiz_vxWorks {
	variable _projectLocation
	variable _projectName
	variable _basisBsp \
		[lindex [defaultBasisBspGet [wtxHostType]] 0]
	variable _basisProject
        variable _tool \
		[lindex [defaultBasisBspGet [wtxHostType]] 1]

	variable _bspListCache
	variable _projectListCache

	variable _fullPath
	variable _bootromProjectFile ;# for future use
	variable _newOrExistingWorkspaceFileName
	variable _currentOrNewOrExistingWorkspace
	variable _workspaceFileName
	variable _bUseBsp 0
	variable _description
	variable _comboWidth 36

	proc name {} {
		return [::prj_vxWorks::name]
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

# XXX the following control should be added for real bootrom support \
    	[list boolean -name bootrombool \
			-title "Also create bootrom project" \
			-x 120 -y 166 -w 100 -h 12 -auto]

set sheetOneControlList [::ProjectSaveAs::getControlList 1 ::prj_vxWorks]
set sheetStructList [list \
	[ list \
	$sheetOneControlList \
 	::prjwiz_vxWorks::initProc1 ::prjwiz_vxWorks::exitProc1 {return 0} \
	] \
	[ list \
	[ list \
    	[list label -name lbl1 -title "Specify the Board Support Package (BSP) which will provide\
			board-specific code needed by VxWorks. \
			Alternatively, you may base your Project on an existing\
			Bootable Project.\
			\
			\n\nSource Files will appear in your new Project.  Only those\
			which are dynamically generated will be duplicated in\
			the new Project's directory.\
                        \
			\n\nWould you like to base your project on:" \
			-x 105 -y 7 -w 203 -h 82 \
		] \
		[list choice -newgroup -name projectchoice -title "An &existing project" \
			-auto -x 16 -y 88 -w 90 -h 9 \
		] \
		[list choice -name bspchoice -title "A B&SP" \
			-auto -x 16 -y 123 -w 69 -h 9 \
		] \
    	[list combo -name projectnamecombo -x 120 -y 90 -w 186 -h 100 \
			-callback {
				set curSel [controlSelectionGet \
					wizardDialog.projectnamecombo -string]
				if {$curSel != ""} {
					filePathUnFix \
					    ::prjwiz_vxWorks::_projectListCache($curSel)
					textControlTextSetKeepingSelection \
                                            wizardDialog.projectnametext \
					    $::prjwiz_vxWorks::_projectListCache($curSel)
                                        controlFocusSet wizardDialog.projectnamecombo
				}
			} \
		] \
    	[list text -name projectnametext -x 120 -y 104 -w 170 -h 12 -callback {
	    set fileName [controlTextGet wizardDialog.projectnametext]
            ::prjwiz_vxWorks::updateProjectComboControl $fileName
        } \
        ] \
		[list button -name projectnamebrowsebutton -title "..." \
			-x 292 -y 104 -w 14 -h 12 -callback {
				set cwd [pwd]
				if [catch { cd [file dirname [controlTextGet wizardDialog.projectnametext]] }] {
					cd [::Workspace::defaultProjDirGet]
				}
				set fileName [join [fileDialogCreate -filefilters \
					"Project Files(*.wpj)|*.wpj|All Files (*.*)|*.*||" \
					-title "Select Project to Copy" -okbuttontitle "Ok"]]
				cd $cwd
                                if {$fileName != ""} {
		                    controlTextSet wizardDialog.projectnametext $fileName
                                }
			}
		] \
		[list combo -name bspnamecombo -x 120 -y 123 -w 186 -h 100 \
		-callback {
			set curSel [controlSelectionGet \
				wizardDialog.bspnamecombo -string]
			if {$curSel != ""} {
				filePathUnFix \
					::prjwiz_vxWorks::_bspListCache($curSel) 
				textControlTextSetKeepingSelection \
                                    wizardDialog.bspnametext \
				    $::prjwiz_vxWorks::_bspListCache($curSel)
                                controlFocusSet wizardDialog.bspnamecombo
                                ::prjwiz_vxWorks::fillToolComboControl $::prjwiz_vxWorks::_bspListCache($curSel)
			}
		} \
		] \
    	[list text -name bspnametext -x 120 -y 137 -w 170 -h 12 -callback {
	    set bspName [controlTextGet wizardDialog.bspnametext]
            ::prjwiz_vxWorks::updateBspComboControl $bspName
        } \
        ] \
	[list button -name bspnamebrowsebutton -title "..." \
	    -x 292 -y 137 -w 14 -h 12 -callback {
                set initialDir [controlTextGet wizardDialog.bspnametext]
		set bspName [dirBrowseDialogCreate \
			-initialdir [file nativename $initialDir] \
			-title "BSP Directory" -okbuttontitle "Ok"]
                if {$bspName != ""} {
		    controlTextSet wizardDialog.bspnametext $bspName
                }
	    } \
	] \
	[list label -name toollabel \
		-title "Tool" \
		-x 120 -y 153 -w 20 -h 8 \
	] \
	[list combo -name toolcombo -x 150 -y 151 -w 50 -h 100 \
        ] \
	] 	::prjwiz_vxWorks::initProc2 ::prjwiz_vxWorks::exitProc2 {return 0} \
	] \
	[list \
	[list \
    	[list label -name lbl1 -title "The Project Creation Wizard will now create your\
			Bootable Project." \
			-x 100 -y 4 -w 200 -h 56] \
    	[list frame -name finishframe -title "Summary" \
			-x 7 -y 93 -w 300 -h 100] \
    	[list label -name lblworkspace -title "Workspace" \
			-x 14 -y 106 -w 66 -h 12] \
    	[list label -name lblproject -title "Project" \
			-x 14 -y 120 -w 66 -h 12] \
    	[list label -name lblprojbsp -title "Basis Project or BSP" \
			-x 14 -y 135 -w 66 -h 12] \
    	[list text -name textworkspace \
		-border \
		-readonly -x 87 -y 103 -w 210 -h 12] \
    	[list text -name textproject \
		-border \
		-readonly -x 87 -y 118 -w 210 -h 12] \
    	[list text -name textprojbsp \
		-border \
		-readonly -x 87 -y 133 -w 210 -h 12] \
	] 	::prjwiz_vxWorks::initProc3 prjwiz_vxWorks::finishProc {return 0} \
	] \
]

proc initProc0 {} {
	variable _projectName
	variable _projectLocation 
	variable _projectListCache
	variable _bspListCache
	variable _basisProject
	variable _description
	variable _newOrExistingWorkspaceFileName
	variable _currentOrNewOrExistingWorkspace

	catch {unset _projectListCache}
	set _projectLocation [::Workspace::newProjectDirGet]
	set _projectName [lindex [file split $_projectLocation] \
		[expr [llength [file split $_projectLocation]] - 1]]
	catch {unset _bspListCache}
	set _basisProject \
		[::Workspace::defaultBasisProjectGet ::prj_vxWorks [wtxHostType]]

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

	::ProjectSaveAs::init 1 ::prj_vxWorks $_projectName $_projectLocation "$_description" $_newOrExistingWorkspaceFileName $_currentOrNewOrExistingWorkspace
}

proc bspChoiceCB {} {
	variable _basisBsp
	variable _bspListCache
	variable _comboWidth
        variable _tool

	if {![info exists _bspListCache]} {
		controlEnable wizardDialog.bspnamebrowsebutton 0
		controlEnable wizardDialog.bspchoice 0
		controlEnable wizardDialog.projectchoice 0
		set states [::WizardCore::wizardControlStatesGet]
		::WizardCore::wizardControlStatesSet {0 0 0 0 0}
    	set bsps [::bspsEnumerate]
		foreach bsp $bsps {
			set truncBsp [file tail $bsp]
			set _bspListCache($truncBsp) $bsp
		}
		controlEnable wizardDialog.bspchoice 1
		controlEnable wizardDialog.projectchoice 1
		controlEnable wizardDialog.bspnamebrowsebutton 1
		::WizardCore::wizardControlStatesSet $states 
	}

	controlValuesSet wizardDialog.bspnamecombo \
		[lsort [array names _bspListCache]]
	set truncBsp [file tail $_basisBsp]
	if {[lsearch [array names _bspListCache] $truncBsp] != -1} {
		controlSelectionSet wizardDialog.bspnamecombo -string $truncBsp 
	} else {

            # try ignoring the saved value - it might 
            # be the wrong arch for this install

	    set _basisBsp \
		[lindex [defaultBasisBspGet [wtxHostType] 1] 0]
            set _tool \
		[lindex [defaultBasisBspGet [wtxHostType] 1] 1]
	    set truncBsp [file tail $_basisBsp]
	    if {[lsearch [array names _bspListCache] $truncBsp] != -1} {
		    controlSelectionSet wizardDialog.bspnamecombo -string $truncBsp 
            } else {

                # finally just punt and pick the first item in the list

	        controlSelectionSet wizardDialog.bspnamecombo 0
            }
	}

	controlEnable wizardDialog.bspnamecombo 1
	controlEnable wizardDialog.bspnamebrowsebutton 1
	controlEnable wizardDialog.bspnametext 1
	controlEnable wizardDialog.toolcombo 1
	controlEnable wizardDialog.projectnamecombo 0
	controlEnable wizardDialog.projectnamebrowsebutton 0
	controlEnable wizardDialog.projectnametext 0
	controlFocusSet wizardDialog.bspnamecombo
# XXX -- enable for bootrom support
#	controlEnable wizardDialog.bootrombool 1
#	controlCheckSet wizardDialog.bootrombool 1
}

proc projectChoiceCB {} {
	variable _basisProject
	variable _projectListCache
	variable _comboWidth

	if {![info exists _projectListCache]} {
		controlEnable wizardDialog.projectnamebrowsebutton 0
		controlEnable wizardDialog.bspchoice 0
		controlEnable wizardDialog.projectchoice 0
		set states [::WizardCore::wizardControlStatesGet]
		::WizardCore::wizardControlStatesSet {0 0 0 0 0}
		set projects [::projectsEnumerate ::prj_vxWorks]
		foreach prj $projects {
			set truncName [file tail $prj]
			set _projectListCache($truncName) $prj
		}
		controlEnable wizardDialog.bspchoice 1
		controlEnable wizardDialog.projectchoice 1
		controlEnable wizardDialog.projectnamebrowsebutton 1
		::WizardCore::wizardControlStatesSet $states 
	}

	controlValuesSet wizardDialog.projectnamecombo \
		[lsort [array names _projectListCache]]
	controlEnable wizardDialog.projectnamecombo 1
	set truncName [file tail $_basisProject]
	if {[lsearch [array names _projectListCache] $truncName] != -1} {
		controlSelectionSet wizardDialog.projectnamecombo \
			-string $truncName 
	} else {
		controlSelectionSet wizardDialog.projectnamecombo 0
		set _basisProject [controlSelectionGet \
			wizardDialog.projectnamecombo -string]
		if {[info exists _projectListCache($_basisProject)]} {
			set _basisProject $_projectListCache($_basisProject)
		} else {
			set _basisProject ""
		}
	}
	controlEnable wizardDialog.bspnamecombo 0
	controlEnable wizardDialog.bspnamebrowsebutton 0
	controlEnable wizardDialog.bspnametext 0
	controlEnable wizardDialog.toolcombo 0
        controlHide wizardDialog.toollabel 0
	controlEnable wizardDialog.projectnamebrowsebutton 1
	controlEnable wizardDialog.projectnametext 1

# XXX enable for bootrom support
#	controlEnable wizardDialog.bootrombool 0
#	controlCheckSet wizardDialog.bootrombool 0
}


#########################################################################
#
# fillToolComboControl - fill the tool combo control
#
# PARAMETERS 
#   bspDir: bsp directory
#
# RETURNS: N / A
#
# ERRORS: N / A
#
proc fillToolComboControl {bspDir} {
    variable _basisBsp
    variable _tool

    set info [getToolChainInfoForBsp $bspDir]
    set toolList [lindex $info 0]

    set defaultTool [lindex $info 1]
    if {[string match $bspDir $_basisBsp]} {
        if {[lsearch $toolList $_tool] != -1} {
            set defaultTool $_tool
        }
    }

    controlValuesSet wizardDialog.toolcombo $toolList
    controlSelectionSet wizardDialog.toolcombo -string $defaultTool
}

#########################################################################
#
# updateBspComboControl - update the bsp combo control
#
# Also updates cached list of bsps
#
# PARAMETERS 
#   bspDir: bsp directory
#
# RETURNS: N / A
#
# ERRORS: N / A
#
proc updateBspComboControl {bspDir} {
    variable _bspListCache
    ::filePathFix bspDir
    if {$bspDir == "" || \
        ![::prj_vxWorks_hidden::bspDirValid $bspDir]} {
        return
    }
    set truncName [file tail $bspDir]
    set nBspsPrev [array size _bspListCache]
    set _bspListCache($truncName) $bspDir
    if {[array size _bspListCache] > $nBspsPrev} {
        controlValuesSet wizardDialog.bspnamecombo [lsort [array names \
	    _bspListCache]]
    }
    if {[controlSelectionGet wizardDialog.bspnamecombo -string] \
        != $truncName } {
        controlSelectionSet wizardDialog.bspnamecombo -string $truncName
        controlFocusSet wizardDialog.bspnametext
    } 
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
	variable _bUseBsp

	controlCallbackSet wizardDialog.bspchoice \
		::prjwiz_vxWorks::bspChoiceCB
	controlCallbackSet wizardDialog.projectchoice \
		::prjwiz_vxWorks::projectChoiceCB

	::WizardCore::buttonFocusSet 1

	if {$_bUseBsp == 1} {
		controlCheckSet wizardDialog.projectchoice 0
		controlCheckSet wizardDialog.bspchoice 1
	} else {
		controlCheckSet wizardDialog.bspchoice 0
		controlCheckSet wizardDialog.projectchoice 1
	}
}

proc initProc3 {} {
	variable _projectName
	variable _projectLocation
	variable _basisBsp
	variable _basisProject
	variable _workspaceFileName
	variable _fullPath
	variable _bUseBsp

	# Determine the workspace to use
	set _workspaceFile [::WorkspaceDoc::openWorkspaceGet]
	if {$_workspaceFile == ""} {
		set _workspaceFile [file rootname $_fullPath]
		set _workspaceFile [format "%s.wsp" $_workspaceFile] 
	}

	if {$_bUseBsp} {
		set basisLbl "Basis BSP"
		set basis $_basisBsp
	} else {
		set basisLbl "Basis Project"
		set basis $_basisProject
	}
	
	controlTextSet wizardDialog.lblprojbsp $basisLbl
	controlTextSet wizardDialog.textworkspace [file nativename $_workspaceFileName]
	controlTextSet wizardDialog.textproject [file nativename $_fullPath]
	controlTextSet wizardDialog.textprojbsp [file nativename $basis]

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
	variable _bspListCache
	variable _basisBsp
	variable _basisProject
	variable _bUseBsp
	variable _bootromProjectFile ""
    variable _tool

	if {[controlChecked wizardDialog.bspchoice]} {
		set _bUseBsp 1

		# Obtain and validate the bsp
		set _basisBsp [controlTextGet wizardDialog.bspnametext]
                set _tool [controlSelectionGet wizardDialog.toolcombo -string]
		::filePathFix _basisBsp
    		if {![::prj_vxWorks_hidden::bspDirValid $_basisBsp]} {
    		    messageBox "Directory '$_basisBsp' is not a BSP"
    		} else {
                    defaultBasisBspSet $_basisBsp $_tool
		    set retval 1
		}
# For future integration
if {0} {
		if {[controlChecked wizardDialog.bootrombool]} {
			set _bootromProjectFile [file dirname $_fullPath]/bootrom
			set _bootromProjectFile $_bootromProjectFile/[file rootname \
				[file tail $_fullPath]]-BootRom.wpj
		}
} else {
		set _bootromProjectFile "" 
} 

	} else {
		# Obtain and validate the project
		set _bUseBsp 0
		set _basisProject [controlTextGet \
			wizardDialog.projectnametext]
		::filePathFix _basisProject
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
        	    if {![string match "::prj_vxWorks" $prjType]} {
					set prjName [file rootname [file tail $_basisProject]]
        	        messageBox -ok -exclamationicon \
						"'$prjName' is not a VxWorks image. \
						Please select another project to replicate."
					set retval 0
        	    } else {
                        ::Workspace::defaultBasisProjectSet \
                            $prjType $_basisProject 
        	    }
        	}
		}
	}

	return $retval 
}

proc finishProc {} {
	variable _basisBsp
	variable _basisProject
	variable _fullPath
	variable _workspaceFileName
	variable _bUseBsp
	variable _description
	variable _bootromProjectFile
        variable _tool

	set projectsToAdd $_fullPath ;# List of projects to add to workspace

	set retval ""

	if {$_bUseBsp} {

		# Build project from a bsp
		::beginWaitCursor
		if {$_bootromProjectFile != ""} {
			lappend projectsToAdd $_bootromProjectFile
		}

		if {[catch {::prj_vxWorks::create \
			$_fullPath $_basisBsp $_bootromProjectFile \
                        $_description "" $_tool} error]} {
			::prjStatusLog ""
			::endWaitCursor
    		::prjStatusLogFuncSet [::defaultPrjStatusLogFuncGet] 
			error $error
		} else {
			::prjStatusLog ""
			::endWaitCursor
		}
	} else {
		# Build project from another project; _basisProject
		# is actually that project
		::beginWaitCursor

		#
		# If basis project is open, decide whether to copy from memory or disk
		#
		set fCopyFromMemory 0
		set hProj [::Workspace::projectHandleGet $_basisProject]
		if {$hProj != ""} { 
			# project open and not dirty - use version in memory
			if {![::prjDirtyGet $hProj]} {
				set fCopyFromMemory 0
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

		# Add the project(s) created
		foreach project $projectsToAdd {
			::prjStatusLog "Saving project..."
			::Workspace::projectAdd $project
			::prjStatusLog "Saving project...Done"
		}

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
	return IDI_BOOTABLE
}

proc projectWizardRun {} {
	set retval [::WizardCore::WizardRun $::prjwiz_vxWorks::myFSM \
		$::prjwiz_vxWorks::sheetStructList ::prjwiz_vxWorks::initProc0 \
		::prjwiz_vxWorks::exitProcN]
	return $retval
}

# Register the wizard
::ViewBrowser::projectWizardRegister "[::prj_vxWorks::name]" ::prjwiz_vxWorks
}
