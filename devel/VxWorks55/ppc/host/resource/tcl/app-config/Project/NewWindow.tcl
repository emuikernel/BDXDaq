# NewWindow.tcl - Window for creating new objects and placing them into 
# a Tornado Workspace or Project.
#
# modification history
# --------------------
# 01g,27jun02,cjs  Fix spr 79044
# 01f,12apr99,j_k  part of fix for SPR #26648.
# 01e,16mar99,rbl  adding keyboard shortcuts, fixing drawing problem in large
#                  fonts by increasing comboFileAddToProject_y by 2 dialog units. 
#                  converting txtFileLocation to native file format.
# 01d,09feb99,j_k  Part of fix for SPR #22908.
# 01c,09feb99,ren  New layout.
# 01a,01feb99,ren  Adding help button.
# 01b,23nov98,???  Remove extra brace causing obscure bug
# 01a,31aug98,aam  written.
#
# DESCRIPTION
#
#############################################################


#
# This namespace is a generic TCL API for implementing multi-tabbed
# dialogs.  It does not really belong in this file, but the NewWindow
# is currently the only window to use the API.
#
namespace eval TabControl {
    namespace export tabcontrolOnTabchange \
	tabcontrolTabCreate \
	tabcontrolQueryExit \
	tabcontrolSave \
	tabcontrolTabSelect \
	tabcontrolInit 

    # This array stores the tab names, control names and callbacks
    # for every tab that is placed onto a tab control
    #
    variable tabArray 

    # This proc is used to create a "tab" on a tabControl
    # Usage:
    #   tabcontrolName - control on which to create tab
    #   tabName - name of tab to create
    #   propertyArray - array of properties for the tab
    #     propertyArray(controls) list of control creations for the tab
    #     propertyArray(onInit) callback for handling the tab init
    #     propertyArray(onEnter) callback for handling the tab selection
    #     propertyArray(queryExit) called to decide if tab can be exited
    #     propertyArray(onSave) callback for handling the tab data save
    #
    proc tabcontrolTabCreate {tabcontrolName tabName propertyArray} {
	upvar $propertyArray tabProperties
	variable tabArray
	set parentName [string range $tabcontrolName \
			    0 [expr [string last . $tabcontrolName] -1]
			]

	controlTabsAdd $tabcontrolName $tabName
	if {! [info exists tabArray($tabcontrolName,currentTab)] } {
	    # This is the first tab placed on this TabControl.
	    # 
	    # Create an array to manage it's information.
	    set tabArray($tabcontrolName,currentTab) {}
	    set tabArray($tabcontrolName,tabs) {}
	}

	lappend tabArray($tabcontrolName,tabs) $tabName

	# Initialize the properties for the new Tab
	set tabArray($tabcontrolName,$tabName,controls) {}
	set tabArray($tabcontrolName,$tabName,onInit) {}
	set tabArray($tabcontrolName,$tabName,onEnter) {}
	set tabArray($tabcontrolName,$tabName,queryExit) {}
	set tabArray($tabcontrolName,$tabName,onSave) {}

	foreach property [array names tabProperties] {
	    set tabArray($tabcontrolName,$tabName,$property) \
		$tabProperties($property)
	}

	# Now create each control and hide it.
	# Also create an array of control names to use for showing controls,
	#
	set controls $tabArray($tabcontrolName,$tabName,controls)

	# blank out the list of control names
	#
	set tabArray($tabcontrolName,$tabName,controls) {}
	foreach control $controls {
	    controlCreate $parentName $control
	    # AAM - kludge, should use "flagValueGet $control -name"
	    set controlName "$parentName.[lindex $control 2]"
	    lappend tabArray($tabcontrolName,$tabName,controls) $controlName
	    controlHide $controlName 1
	}

	if {[llength $tabArray($tabcontrolName,$tabName,onInit)]} {
	    # invoke the "onInit" routine for the tab.
	    #
	    eval $tabArray($tabcontrolName,$tabName,onInit)
	}
    }

    proc tabcontrolQueryExit {tabcontrolName} {
	variable tabArray

	set currentTab [controlSelectionGet $tabcontrolName -string]
	if {[llength $tabArray($tabcontrolName,$currentTab,queryExit)]} {
	    # invoke the "queryExit" routine for the tab.
	    #
	    return [eval $tabArray($tabcontrolName,$currentTab,queryExit)]
	}
	return 1
    }

    proc tabcontrolSave {tabcontrolName} {
	variable tabArray

	# invoke "onSave" callback for each tab
	foreach tabName $tabArray($tabcontrolName,tabs) {
	    # Check for an "onClose" callback
	    #
	    if {[llength $tabArray($tabcontrolName,$tabName,onSave)]} {
		# invoke the "onClose" callback for the tab.
		#
		eval $tabArray($tabcontrolName,$currentTab,onSave)
	    }  
	}

    }

    # callback for handling the selection of a new tab.
    #
    proc tabcontrolOnTabchange {tabcontrolName} {
	variable tabArray

	set tabName [controlSelectionGet $tabcontrolName -string] 
	set currentTab $tabArray($tabcontrolName,currentTab)

	# Do nothing when there is no real tab change
	#
	if [string match $tabName $currentTab] return

	# Check if a tab has ever been selected for this tabcontrol.
	# 
	if {[llength $currentTab]} {
	    # There was a previously selected tab previously selected.
	    # Look for a "queryExit" routine for the tab.
	    #
	    if {[llength $tabArray($tabcontrolName,$currentTab,queryExit)]} {
		# invoke the "queryExit" routine for the tab.
		#
		set retValue [eval $tabArray($tabcontrolName,$currentTab,queryExit)]
		if {! $retValue} {
		    # Tab does not want the user to exit it.
		    #
		    controlSelectionSet $tabcontrolName -string $currentTab
		    return
		}
	    }
	    
	    # If we have not exited yet then hide the controls of the
	    # previously selected tab.
	    #
	    foreach controlName $tabArray($tabcontrolName,$currentTab,controls) {
		controlHide $controlName 1
	    }
	}

	# Set the current tab to the new tab.
	#
	set tabArray($tabcontrolName,currentTab) $tabName

	# Unhide the new tab's controls
	#
	foreach controlName $tabArray($tabcontrolName,$tabName,controls) {
	    controlHide $controlName 0
	}

	# Check if the new tab has an "onEnter" callback
	#
	if {[llength $tabArray($tabcontrolName,$tabName,onEnter)]} {
	    # invoke the new tabs "onEnter" callback
	    eval $tabArray($tabcontrolName,$tabName,onEnter)
	}
    }

    proc tabcontrolInit {tabcontrolName} {
	variable tabArray
	if {[info exists tabArray($tabcontrolName,currentTab)]} {
	    unset tabArray($tabcontrolName,currentTab)
	}
    }

    proc tabcontrolTabSelect {tabcontrolName tabName} {

	set currentTab [controlSelectionGet $tabcontrolName -string]
	if {[string match $currentTab $tabName]} {
	    # If the selected tab is the one wanted then
	    # we must manually invoked the OnTabChange handler
	    #
	    tabcontrolOnTabchange $tabcontrolName
	} else {
	    controlSelectionSet $tabcontrolName -string $tabName
	}
	
    }

    namespace eval :: {
	namespace import TabControl::*
    }

}



namespace eval NewWindow {
    # Window Properties
    variable m_self {}
    variable m_name NewWindow
    variable m_title New
    variable m_width 300
    variable m_height 150

    namespace eval TabControl {
	variable m_self {}
	variable m_parent {}
	variable m_name TabControl
	variable m_currentTab {}
	variable m_width
	variable m_height
	variable m_xpos
	variable m_ypos
        variable m_fileTypeSelected {}

	namespace eval FilesTab {
	    variable m_parent {}
	    variable m_name Files
	    variable m_width
	    variable m_lst_x
	    variable m_lst_y
	    variable m_lst_h
	    variable m_lst_w

	    # Tab state machine variable
	    variable m_bFileTypesFetched 0

	    proc onOk {} {
		variable m_parent
                variable m_fileTypeSelected

		set fileType [controlSelectionGet \
				  $m_parent.lstFileTypes -string]

		set fileName [controlValuesGet \
				  $m_parent.txtFileName]

		set fileLocation [controlValuesGet \
				      $m_parent.txtFileLocation]

		set bAddToProject [controlChecked \
				      $m_parent.chFileAddToProject]
                    
                if {[needBlankFilename] == 1} {
                    set m_fileTypeSelected $fileType
                    return 1
                }

		if { [catch {documentTypeFileNew $fileType \
				 [file join $fileLocation $fileName]} result] } {
		    messageBox $result
		    return 0
		}

		if {$bAddToProject} {
		    set filePath $result
		    set projectPath [selectedProjectPath]

		    if { [catch {::Workspace::fileAdd junk \
				     $projectPath [list $filePath]} result] } {
			messageBox $result
			return 0
		    }
		}

		return 1
	    }

	    proc onAddToProject {} {
		variable m_parent

		set add2project [controlChecked \
			       $m_parent.chFileAddToProject]
		
		controlEnable $m_parent.comboFileAddToProject $add2project
	       
	
	    }

	    proc onProjectSelection {} {
		variable m_parent

		set eventInfo [controlEventGet \
				   $m_parent.comboFileAddToProject]
		if [string match {*selchange*} $eventInfo] {
		    populateFileLocation
		}
	    }
	
	    proc onFileNameChange {} {
		
		# Deterime if the OK button should be enabled
		okButtonStateSet
	    }

	    proc onFileLocationBrowse {} {
		variable m_parent
		
		# Browse for a directory
			set directoryPath [ dirBrowseDialogCreate \
								-initialdir [file nativename [controlValuesGet $m_parent.txtFileLocation]] \
							   ]

		if {$directoryPath != ""} {
		    controlValuesSet $m_parent.txtFileLocation  \
			[file nativename $directoryPath]
		}
		    
	    }

	    proc controlListGet {} {
		variable m_width
		variable m_lst_x
		variable m_lst_y
		variable m_lst_h
		variable m_lst_w

		
		set chFileAddToProject_x [expr $m_lst_x + $m_lst_w + 4]
		set chFileAddToProject_y $m_lst_y
		set chFileAddToProject_h 14
		set chFileAddToProject_w [expr $m_width - $chFileAddToProject_x - 4]

		set comboFileAddToProject_x $chFileAddToProject_x
		set comboFileAddToProject_y [expr $chFileAddToProject_y + 14]
		set comboFileAddToProject_h 96
		set comboFileAddToProject_w $chFileAddToProject_w

		set lblFileName_x $chFileAddToProject_x
		set lblFileName_y [expr $chFileAddToProject_y + 32]
		set lblFileName_h 9
		set lblFileName_w $chFileAddToProject_w

		set txtFileName_x $lblFileName_x
		set txtFileName_y [expr $lblFileName_y + 12]
		set txtFileName_h 14
		set txtFileName_w $lblFileName_w


		set lblFileLocation_x $lblFileName_x
			set lblFileLocation_y [expr $txtFileName_y + 16]
		set lblFileLocation_h 9
		set lblFileLocation_w [expr $txtFileName_w - 16]

		set txtFileLocation_x $lblFileLocation_x
		set txtFileLocation_y [expr $lblFileLocation_y + 12]
		set txtFileLocation_h 14
		set txtFileLocation_w $lblFileLocation_w

		set cbFileLocation_x [expr $txtFileLocation_x + $txtFileLocation_w + 2]
		set cbFileLocation_y [expr $txtFileLocation_y ]
		set cbFileLocation_h 14
		set cbFileLocation_w 16
		return	 [ list \
			       [list list -name lstFileTypes \
			          -callback ::NewWindow::TabControl::FilesTab::onDocTypeSelected \
				  -x $m_lst_x -y $m_lst_y \
				  -w $m_lst_w -h $m_lst_h] \
			       [list boolean -name chFileAddToProject \
				    -title "&Add to project:" \
				    -auto \
				    -callback \
				    ::NewWindow::TabControl::FilesTab::onAddToProject \
				    -x $chFileAddToProject_x -y $chFileAddToProject_y \
				    -w $chFileAddToProject_w -h $chFileAddToProject_h] \
			       [list combo -name comboFileAddToProject \
				    -callback \
				    ::NewWindow::TabControl::FilesTab::onProjectSelection \
				    -x $comboFileAddToProject_x -y $comboFileAddToProject_y \
				    -w $comboFileAddToProject_w -h $comboFileAddToProject_h]\
			       [list label -name lblFileName \
				    -title {&File name:} \
				    -x $lblFileName_x -y $lblFileName_y \
				    -w $lblFileName_w -h $lblFileName_h] \
			       [list text -name txtFileName \
				    -border \
				    -callback \
				    ::NewWindow::TabControl::FilesTab::onFileNameChange \
				    -x $txtFileName_x -y $txtFileName_y \
				    -w $txtFileName_w -h $txtFileName_h] \
			       [list label -name lblFileLocation \
				    -title {&Location:} \
				    -x $lblFileLocation_x -y $lblFileLocation_y \
				    -w $lblFileLocation_w -h $lblFileLocation_h] \
			       [list text -name txtFileLocation \
				    -border \
				    -x $txtFileLocation_x -y $txtFileLocation_y \
				    -w $txtFileLocation_w -h $txtFileLocation_h] \
			       [list button -name cbFileLocation \
				    -title "..." \
				    -callback \
				    ::NewWindow::TabControl::FilesTab::onFileLocationBrowse \
				    -x $cbFileLocation_x -y $cbFileLocation_y \
				    -w $cbFileLocation_w -h $cbFileLocation_h] 
			 ]
	    }

	    proc onDocTypeSelected {} {
		variable m_parent

                set fileName [controlTextGet $m_parent.txtFileName]

                if {[needBlankFilename] == 1} {
                    controlEnable $m_parent.chFileAddToProject 0
                    controlEnable $m_parent.comboFileAddToProject 0
                    controlEnable $m_parent.lblFileName 0
                    controlEnable $m_parent.txtFileName 0
                    controlEnable $m_parent.lblFileLocation 0
                    controlEnable $m_parent.txtFileLocation 0
                    controlEnable $m_parent.cbFileLocation 0
		    controlEnable $m_parent.cbOK 1
                } else {
                    controlEnable $m_parent.chFileAddToProject 1
                    controlEnable $m_parent.comboFileAddToProject 1
                    controlEnable $m_parent.lblFileName 1
                    controlEnable $m_parent.txtFileName 1
                    controlEnable $m_parent.lblFileLocation 1
                    controlEnable $m_parent.txtFileLocation 1
                    controlEnable $m_parent.cbFileLocation 1

                    if {$fileName == ""} { 
		        controlEnable $m_parent.cbOK 0
                    } else { 
		        controlEnable $m_parent.cbOK 1
                    }
                }
            }

	    proc initialize {} {
		variable m_bFileTypesFetched
		variable m_projectHandles

		set m_bFileTypesFetched 0
	    }

	    proc selectedProjectPath {} {
		variable m_parent

		set selNo [controlSelectionGet $m_parent.comboFileAddToProject]
		set prjNo 0
		foreach projectFile [array names \
					 ::Workspace::_projectHandles] {
		    if {$prjNo == $selNo} {
			return $projectFile
		    }
		    incr prjNo
		}

		return ""
	    }

	    proc populateProjectList {} {
		variable m_parent
		set projectNames {}

		foreach projectFile [array names \
					     ::Workspace::_projectHandles] {
		    lappend projectNames [file tail $projectFile]
		}

		controlValuesSet $m_parent.comboFileAddToProject \
			$projectNames

		# Check if there are any projects
		#
		if {[llength $projectNames] > 0} {
		    set bProjectsExist 1

		    # select first project in list
		    #
		    controlSelectionSet $m_parent.comboFileAddToProject 0
		} else {
		    set bProjectsExist 0
		}

		# If projects exist then enable the controls for adding
		# files to a project, else disable those controls
		#
		controlCheckSet $m_parent.chFileAddToProject $bProjectsExist
		controlEnable $m_parent.chFileAddToProject $bProjectsExist
		controlEnable $m_parent.comboFileAddToProject $bProjectsExist
	    }

	    proc populateFileLocation {} {
		variable m_parent

		set add2project [controlChecked \
			       $m_parent.chFileAddToProject]
		if {$add2project} {
		    controlValuesSet $m_parent.txtFileLocation  \
			[file nativename [file dirname [selectedProjectPath]]]
		}
	    }

            proc needBlankFilename {} {
		variable m_parent

                set selDocType [controlSelectionGet $m_parent.lstFileTypes -string]
                if [catch {documentTypeGet $selDocType NeedBlankFilename} bFileNameMustBeBlank] {
                    set bFileNameMustBeBlank 0
                }
		return $bFileNameMustBeBlank
            }


	    proc okButtonStateSet {} {
		variable m_parent

		set fileName [controlValuesGet $m_parent.txtFileName]
		if {[needBlankFilename] == 1 || [llength $fileName] > 0} {
		    controlEnable $m_parent.cbOK 1
		} else {
		    controlEnable $m_parent.cbOK 0
		}
	    }

	    proc onTabSelect {} {
		variable m_parent
		variable m_bFileTypesFetched

		if {! $m_bFileTypesFetched } {
		    set m_bFileTypesFetched 1

		    controlValuesSet $m_parent.lstFileTypes \
			[lsort [documentTypes]]

		    # Select first document type in list
		    #
		    if {[llength [documentTypes]] > 0} {
			controlSelectionSet $m_parent.lstFileTypes 0
		    }

		    populateProjectList
		    populateFileLocation
		}
		okButtonStateSet
		if {[needBlankFilename] == 1} {
                    controlEnable $m_parent.chFileAddToProject 0
                }
	    }

	}

	namespace eval ProjectsTab {
	    variable m_parent {}
	    variable m_name Projects
	    variable m_width
	    variable m_lst_x
	    variable m_lst_y
	    variable m_lst_h
	    variable m_lst_w

	    # Window state machine variables
	    variable m_projectTypesFetched 0

	    proc onOk {} {
		return 1
	    }

	    proc controlListGet {} {
		variable m_width
		variable m_lst_x
		variable m_lst_y
		variable m_lst_h
		variable m_lst_w

		set lblPrjName_x [expr $m_lst_x + $m_lst_w + 4]
		set lblPrjName_y $m_lst_y
		set lblPrjName_h 14
		set lblPrjName_w [expr $m_width - $lblPrjName_x - 4]

		set txtPrjName_x $lblPrjName_x
		set txtPrjName_y [expr $lblPrjName_y + 14]
		set txtPrjName_h 14
		set txtPrjName_w $lblPrjName_w


		set lblPrjLocation_x $lblPrjName_x
		set lblPrjLocation_y [expr $txtPrjName_y + 14]
		set lblPrjLocation_h 14
		set lblPrjLocation_w [expr $txtPrjName_w - 16]

		set txtPrjLocation_x $lblPrjLocation_x
		set txtPrjLocation_y [expr $lblPrjLocation_y + 14]
		set txtPrjLocation_h 14
		set txtPrjLocation_w $lblPrjLocation_w

		set cbPrjLocation_x [expr $txtPrjLocation_x + $txtPrjLocation_w + 2]
		set cbPrjLocation_y [expr $txtPrjLocation_y - 2]
		set cbPrjLocation_h 14
		set cbPrjLocation_w 16

		set rbPrjCreateNewWksp_x $txtPrjLocation_x
		set rbPrjCreateNewWksp_y [expr $txtPrjLocation_y + 28]
		set rbPrjCreateNewWksp_h 14
		set rbPrjCreateNewWksp_w $txtPrjName_w

		set rbPrjAddToWksp_x $rbPrjCreateNewWksp_x
		set rbPrjAddToWksp_y [expr $rbPrjCreateNewWksp_y + 14]
		set rbPrjAddToWksp_h 14
		set rbPrjAddToWksp_w $rbPrjCreateNewWksp_w
		return	 [ list \
			       [list list -name lstPrjTypes \
				  -x $m_lst_x -y $m_lst_y \
				  -w $m_lst_w -h $m_lst_h] \
			       [list label -name lblPrjName \
				    -title {Project name:} \
				    -x $lblPrjName_x -y $lblPrjName_y \
				    -w $lblPrjName_w -h $lblPrjName_h] \
			       [list text -name txtPrjName \
				    -border \
				    -x $txtPrjName_x -y $txtPrjName_y \
				    -w $txtPrjName_w -h $txtPrjName_h] \
			       [list label -name lblPrjLocation \
				    -title {Location:} \
				    -x $lblPrjLocation_x -y $lblPrjLocation_y \
				    -w $lblPrjLocation_w -h $lblPrjLocation_h] \
			       [list text -name txtPrjLocation \
				    -border \
				    -x $txtPrjLocation_x -y $txtPrjLocation_y \
				    -w $txtPrjLocation_w -h $txtPrjLocation_h] \
			       [list button -name cbPrjLocation \
				    -title "..." \
				    -x $cbPrjLocation_x -y $cbPrjLocation_y \
				    -w $cbPrjLocation_w -h $cbPrjLocation_h] \
			       [list choice -name rbPrjCreateNewWksp \
				    -auto -newgroup \
				    -title "Create new workspace" \
				    -x $rbPrjCreateNewWksp_x -y $rbPrjCreateNewWksp_y \
				    -w $rbPrjCreateNewWksp_w -h $rbPrjCreateNewWksp_h] \
			       [list choice -name rbPrjAddToWksp \
				    -auto \
				    -title "Add to current workspace" \
				    -x $rbPrjAddToWksp_x -y $rbPrjAddToWksp_y \
				    -w $rbPrjAddToWksp_w -h $rbPrjAddToWksp_h]
			 ]
	    }

    	    proc initialize {} {
		variable m_projectTypesFetched 0
	    }

	    proc okButtonStateSet {} {
	    }

	    proc onTabSelect {} {
		variable m_parent
		variable m_projectTypesFetched

		if {! $m_projectTypesFetched } {
		    controlValuesSet $m_parent.lstPrjTypes \
			[::ViewBrowser::projectWizardListGet]
		    controlSelectionSet $m_parent.lstPrjTypes 0
		    set m_projectTypesFetched 1
		}

		okButtonStateSet
	    } 
	}

	namespace eval WorkspacesTab {
	    variable m_parent {}
	    variable m_name Workspaces
	    variable m_width
	    variable m_lst_x
	    variable m_lst_y
	    variable m_lst_h
	    variable m_lst_w

	    # Tabs state machine variables
	    variable m_workspaceTypesFetched 0

	    proc onOk {} {
		return 1
	    }

	    proc controlListGet {} {
		variable m_width
		variable m_lst_x
		variable m_lst_y
		variable m_lst_h
		variable m_lst_w

		set lblWkspName_x [expr $m_lst_x + $m_lst_w + 4]
		set lblWkspName_y $m_lst_y
		set lblWkspName_h 14
		set lblWkspName_w [expr $m_width - $lblWkspName_x - 4]

		set txtWkspName_x $lblWkspName_x
		set txtWkspName_y [expr $lblWkspName_y + 14]
		set txtWkspName_h 14
		set txtWkspName_w $lblWkspName_w


		set lblWkspLocation_x $lblWkspName_x
		set lblWkspLocation_y [expr $txtWkspName_y + 14]
		set lblWkspLocation_h 14
		set lblWkspLocation_w [expr $txtWkspName_w - 16]

		set txtWkspLocation_x $lblWkspLocation_x
		set txtWkspLocation_y [expr $lblWkspLocation_y + 14]
		set txtWkspLocation_h 14
		set txtWkspLocation_w $lblWkspLocation_w

		set cbWkspLocation_x [expr $txtWkspLocation_x + $txtWkspLocation_w + 2]
		set cbWkspLocation_y [expr $txtWkspLocation_y - 2]
		set cbWkspLocation_h 14
		set cbWkspLocation_w 16
		return	 [ list \
			       [list list -name lstWkspTypes \
				  -x $m_lst_x -y $m_lst_y \
				  -w $m_lst_w -h $m_lst_h] \
			       [list label -name lblWkspName \
				    -title {Workspace name:} \
				    -x $lblWkspName_x -y $lblWkspName_y \
				    -w $lblWkspName_w -h $lblWkspName_h] \
			       [list text -name txtWkspName \
				    -border \
				    -x $txtWkspName_x -y $txtWkspName_y \
				    -w $txtWkspName_w -h $txtWkspName_h] \
			       [list label -name lblWkspLocation \
				    -title {Location:} \
				    -x $lblWkspLocation_x -y $lblWkspLocation_y \
				    -w $lblWkspLocation_w -h $lblWkspLocation_h] \
			       [list text -name txtWkspLocation \
				    -border \
				    -x $txtWkspLocation_x -y $txtWkspLocation_y \
				    -w $txtWkspLocation_w -h $txtWkspLocation_h] \
			       [list button -name cbWkspLocation \
				    -title "..." \
				    -x $cbWkspLocation_x -y $cbWkspLocation_y \
				    -w $cbWkspLocation_w -h $cbWkspLocation_h]
			 ]
	    }

	    proc initialize {} {
		variable m_bWorkspaceTypesFetched 

		set m_bWorkspaceTypesFetched 0
	    }

	    proc onTabSelect {} {
		variable m_parent
		variable m_bWorkspaceTypesFetched

		if {! $m_bWorkspaceTypesFetched } {
		    controlValuesSet $m_parent.lstWkspTypes \
			[list {Blank Workspace}]
		    controlSelectionSet $m_parent.lstWkspTypes 0
		    set m_bWorkspaceTypesFetched 1
		}

	    }
	}

	proc show {} {
	    variable m_self
	    variable m_parent
	    variable m_xpos
	    variable m_ypos
	    variable m_height
	    variable m_width

	    tabcontrolInit $m_self
#
# Temporarily disable the multitab appearance of this window.
#    foreach tabName - Files Projects Workspaces -
# only show the Files tab.

	    foreach tabName {Files} {
		set ${tabName}Tab::m_parent $m_parent
		set ${tabName}Tab::m_width $m_width
		set ${tabName}Tab::m_lst_x [expr $m_xpos + 7]
		set ${tabName}Tab::m_lst_y [expr $m_ypos + 19]
		set ${tabName}Tab::m_lst_h [expr $m_height - 20]
		set ${tabName}Tab::m_lst_w [expr $m_width * 2 / 5]
		${tabName}Tab::initialize

		set tabProperties(onEnter) \
		    NewWindow::TabControl::${tabName}Tab::onTabSelect
		set tabProperties(controls) \
		    [${tabName}Tab::controlListGet]
		tabcontrolTabCreate $m_self $tabName \
		     tabProperties
	    }

	    tabcontrolTabSelect $m_self Files
	}

    }

    proc show {} {
	variable m_self
	variable m_name
	variable m_title
	variable m_height
	variable m_width


	set m_self $m_name
	dialogCreate -name $m_name \
	    -title $m_title \
	    -init ::NewWindow::onInit \
	    -width $m_width -height $m_height \
	    -controls [controlsListGet]
     
    }

    proc onInit {} {
	variable m_self

	TabControl::show
    }

    proc controlsListGet {} {
	variable m_self
	variable m_width
	variable m_height

	set cbHelp_h 14
    set cbHelp_w 50
	set cbHelp_x [expr $m_width - $cbHelp_w - 6]
	set cbHelp_y [expr $m_height - $cbHelp_h - 6]

	set cbCancel_h 14
	set cbCancel_w 50
	set cbCancel_x [expr {$cbHelp_x - $cbCancel_w - 4}]
	set cbCancel_y $cbHelp_y

	set cbOK_h 14
	set cbOK_w 50
	set cbOK_x [expr {$cbCancel_x - $cbOK_w - 4}]
	set cbOK_y $cbHelp_y

	set TabControl::m_parent $m_self
	set TabControl::m_self $m_self.$TabControl::m_name
	set TabControl::m_height [expr $m_height - $cbOK_h - 16]
	set TabControl::m_width [expr $m_width - 8]
	set TabControl::m_xpos 4
	set TabControl::m_ypos 4

	set controls [list \
			  [list button -name cbOK \
			       -title "&OK" \
			       -default \
			       -callback ::NewWindow::onOk \
			       -x $cbOK_x -y $cbOK_y \
			       -w $cbOK_w -h $cbOK_h] \
			  [list button -name cbCancel \
			       -title "&Cancel" \
			       -callback ::NewWindow::onCancel \
			       -x $cbCancel_x -y $cbCancel_y \
			       -w $cbCancel_w -h $cbCancel_h] \
			  [list button -name cbHelp \
			       -title &Help \
			       -helpbutton \
			       -x $cbHelp_x -y $cbHelp_y \
			       -w $cbHelp_w -h $cbHelp_h] \
			  [list tab -name $TabControl::m_name \
			       -multiline  \
			       -callback [list \
					      tabcontrolOnTabchange $m_self.$TabControl::m_name] \
			       -x $TabControl::m_xpos -y $TabControl::m_ypos \
			       -w $TabControl::m_width -h $TabControl::m_height] \
		      ]

	return $controls
    }

    proc onOk {} {
	variable m_self

	set tabName [controlSelectionGet $TabControl::m_self -string]
	if {[TabControl::${tabName}Tab::onOk]} {
	    windowClose $m_self
	}
    }

    proc onCancel {} {
	variable m_self

	windowClose $m_self
    }

    proc menusCreate {} {
	menuItemAppend -top -bypath -callback \
	    { NewWindow::show } \
	    {&File} {&New...}
    }
}


# The following is a temporary if clause, eventually the menus
# will be created on all platforms.
#
# On windows the FILE->NEW menu cannot yet be added until removed
# from the default IDE.
# Until that happens only create the FILE->NEW menu on UNIX platforms.
#
if {![ string match *win32* [wtxHostType] ]} {
    NewWindow::menusCreate
}
