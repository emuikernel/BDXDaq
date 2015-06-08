# Project.win32.tcl - entry point for the config tool portion of Tornado
#
# modification history
# --------------------
# 03g,08may02,cjs  Fix spr spr 74121: Shift+F6 shortcut for download doesn't
#                  work
# 03f,09jan02,rbl  fix erroneous error message about mismatched CPU's for
#                  download
# 03e,07dec01,rbl  set up diab environment if it is installed
# 03d,15oct01,rbl  read build environment variables (e.g. WIND_SOURCE_BASE)
#                  from preferences and set them at project facility startup
#                  minor syntactic change to guiWorkspaceSave
# 03c,04oct01,rbl  report errors in workspace save (was reporting that
#                  operation was cancelled).  
# 03b,22mar99,rbl  added guiProjectOpen, guiProjectOpen functions designed for 
#                  drag-and-drop and double-clicking on projects or workspaces
#                  to launch Tornado.
#                  don't show welcome dialog, do this in WrsIdeApp::InitInstance().
#                  remove bad file characters from path in guiPrjSave
# 03a,22mar99,cjs  Changing menu text for Object Search dialog
# 02z,19mar99,ren  Fixing SPR 25876: Build fails after renaming project.
# 02y,16mar99,cjs  Changing title of what was formerly 'Find Component'
# 02x,16mar99,rbl  adding -filemustexist flag to "Add Project(s) to workspace"
#                  dialog
# 02w,10mar99,rbl  Changing ::guiPrjSave::guiPrjSave to use
#                  ::ProjectSaveAs::get...
#                  functions to get data out of ProjectSaveAs dialog. 
#                  Handle case where user wants to save Project to a
#                  new workspace
#                  by closing old workspace and opening new one
# 02v,05mar99,cjs  Fix bug in save object related to tree path synthesis for
#                  project files
# 02u,02mar99,cjs  Add support for workspace object
# 02t,03mar99,ren  Adding rename project procedure.
# 02s,02feb99,rbl  Changes to handle missing or read-only projects
# 02r,21jan99,cjs  fix spr 24094: junk chars in Add Project to Workspace
#                  dialog's file filter
# 02q,15jan99,cjs  Validate project file before adding to workspace
# 02p,24nov98,j_k  Part of fix for SPR #23481.
# 02o,20nov98,cjs  Change Add/Delete menu items
# 02n,12nov98,cjs  source WorkspaceBackup.tcl 
# 02m,11nov98,cjs  use download default path from registry;
#                  get rid of 'Stop Build' on UNIX
# 02l,10nov98,cjs  make download toolbar item available on UNIX 
# 02k,10nov98,cjs  restored 'Close Workspace' menu item; only available on UNIX 
# 02j,28oct98,cjs  added an 'Add project to workspace' menu item; removed
#                  unnecessary 'Close Workspace'
# 02i,13oct98,cjs  altered startup sequence for unix so that
#                  welcome dlg is shown only after window is created 
# 02h,05oct98,cjs  changed references to 'Project' to 'Workspace' 
# 02g,03oct98,cjs  changed implementation of guiClose()
# 02f,01oct98,cjs  disable compile button and menu unless source
#                  file selected in project facility
# 02e,30sep98,cjs  removed '*.' from download file dialog filter 
# 02d,29sep98,cjs  added file download to build toolbar and project menu 
# 02c,22sep98,cjs  added build toolbar 
# 02b,21sep98,cjs  adding error handling to workspace save, saveas 
# 02a,17sep98,cjs  cleanup, reorganization, add items to project and build
#                  menus; unify pc/unix
# 01k,04sep98,cjs  removed explicit source of 01ToolsCustomize.win32.tcl 
# 01j,20aug98,jmp  added windHelpLib.tcl sourcing for the on-line help.
# 01i,18aug98,cjs  debug console only shown if reg value set
# 01h,06aug98,cjs  use prjDirDefault() for initial path 
# 01g,29jul98,cjs  call queryClose() instead of guiWorkspaceClose()
# 01f,07jul98,cjs  fix menu paths that didn't expand tabs 
# 01e,20may98,cjs  fix problem with File New 
# 01d,07may98,cjs  added workspace save, save as support 
# 01c,01may98,cjs  use fileDialogCreate to open workspace 
# 01b,16apr98,cjs  load wtxtcl-d.dll
# 01a,27mar98,cjs  written.
#
# DESCRIPTION
#
#############################################################

# Code for the debug command window
# Enable this by adding HKEY_CURRENT_USER/Software/Wind River Systems/
# 	Tornado(x.x)/Tornado/Workspace/ShowDebugConsole = 1
# in the registry
namespace eval ::ProjectConsoleWindow {

	proc execCmd {} {
		set txt [controlValuesGet _cmdWindow.cmdString]
		catch {eval $txt} result
		controlValuesSet _cmdWindow.outputString $result
	}

	proc init {} {
		controlCreate _cmdWindow \
			{text -name cmdString -x 5 -y 5 -width 300 -height 28 \
				-multiline -vscroll}
		controlCreate _cmdWindow \
			{text -name outputString -x 5 -y 34 -width 300 -height 28 \
				-multiline -vscroll}
		controlCreate _cmdWindow \
			{button -name okButton -title Execute -x 310 -y 5 \
			-width 50 -height 14 -callback ::ProjectConsoleWindow::execCmd}
	}

	proc create {} {
		windowCreate -name _cmdWindow \
			-title "Commands" \
			-init ::ProjectConsoleWindow::init \
			-nonewinterp \
			-x 0 -y 200 -width 300 -height 70 
	}
}

namespace eval ::ProjectFacility {

	variable _menuNames
	variable _tbBitmaps
	variable _toolTips

	# This array allows us to change menu text (fiddle with accelerators
	# and so forth) without having to do a global search-and-replace
	array set _menuNames [list \
		projectMenu "&Project" \
		projectAddMenu "&Add/Include" \
		projectAddFile "&File" \
		projectAddComponent "&Component(s)" \
		projectAddBuild "&Build" \
		projectDeleteMenu "&Remove/Exclude" \
		projectDeleteFile "&File" \
		projectDeleteComponent "&Component" \
		projectDeleteBuild "&Build" \
		projectDeleteProject "&Project" \
		projectAutoScale "Auto &Scale..." \
		projectFindComponent "&Find Object..." \
		projectDownload "Do&wnload...\tShift+F6" \
		buildMenu "&Build" \
		buildBuild "&Build" \
		buildRebuildAll "&Rebuild All" \
		buildDependencies "&Dependencies..." \
		buildCompile "&Compile" \
		buildStopBuild "&Stop Build" \
		viewToolbarPC "&View &Toolbar" \
		viewToolbarUNIX "&View" \
		viewBuildTB "&Build Toolbar" \
		fileNewWorkspace "N&ew Project..." \
		fileOpenWorkspace "Open &Workspace..." \
		fileSaveWorkspace "Sa&ve Workspace..." \
		fileSaveAsWorkspace "Save Workspace &as..." \
		fileProjectRename "&Rename project...." \
		fileSaveAsProject "Save &Project as..." \
		fileAddProjectToWorkspace "Add Pro&ject to Workspace..." \
		fileCloseWorkspace "Close Wor&kspace" \
	]

	set bmpPath [wtxPath host resource bitmaps Project]
	set bmpPath2 [wtxPath host resource bitmaps CrossWind controls]
	array set _tbBitmaps [list \
		buildBuild ${bmpPath}Build.bmp\
		buildRebuildAll ${bmpPath}RebuildAll.bmp \
		buildDependencies ${bmpPath}Dependencies.bmp \
		buildCompile ${bmpPath}Compile.bmp \
		buildStopBuild ${bmpPath}StopBuild.bmp \
		projectDownload ${bmpPath2}download.bmp \
	]

	array set _toolTips [list \
		buildBuild "Build Project" \
		buildRebuildAll "Re-Build All" \
		buildDependencies "Update dependencies" \
		buildCompile "Compile current source file" \
		buildStopBuild "Stop Build" \
		projectDownload "Download object file or boot image" \
	]

	# Add menu items to the file menu

	proc fileMenuAugment {insertionPoint} {
		variable _menuNames

		menuItemInsert -after \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				fileNewWorkspace] \
			-callback ::ProjectFacility::guiNew \
			-bypath $insertionPoint $_menuNames(fileNewWorkspace)

		# Hack: -after is broken; so can do it first
		menuItemInsert -bypath -separator \
			[list "&File" $_menuNames(fileNewWorkspace)]

		menuItemInsert -after \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				fileNewWorkspace] \
			-callback ::ProjectFacility::guiOpen \
			-bypath [list &File $_menuNames(fileNewWorkspace)] \
		 	$_menuNames(fileOpenWorkspace)
		menuItemInsert -after \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				fileSaveWorkspace] \
			-callback ::ProjectFacility::guiSave \
			-bypath [list &File $_menuNames(fileOpenWorkspace)] \
		 	$_menuNames(fileSaveWorkspace)	
		menuItemInsert -after \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				fileSaveAsWorkspace] \
			-callback ::ProjectFacility::guiSaveAs \
			-bypath [list &File $_menuNames(fileSaveWorkspace)] \
		 	$_menuNames(fileSaveAsWorkspace)	
		menuItemInsert -after \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				fileSaveAsProject] \
			-callback ::ProjectFacility::guiSaveProjectAs \
			-bypath [list &File $_menuNames(fileSaveAsWorkspace)] \
		 	$_menuNames(fileSaveAsProject)	
		menuItemInsert -after \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				fileSaveAsProject] \
			-callback ::ProjectFacility::guiRenameProject \
			-bypath [list &File $_menuNames(fileSaveAsProject)] \
		 	$_menuNames(fileProjectRename)	
		menuItemInsert -after \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				fileAddProjectToWorkspace] \
			-callback ::ProjectFacility::staticProjectToWorkspaceAdd \
			-bypath [list &File $_menuNames(fileSaveAsProject)] \
		 	$_menuNames(fileAddProjectToWorkspace)

		if {[string match "x86-win32" [wtxHostType]]} {
			menuItemInsert -bypath -separator [list "&File" "&Save\tCtrl+S"] 
		} else {
			menuItemInsert -after \
				-cmduicallback [list ::ProjectFacility::cmdUICallback \
					fileCloseWorkspace] \
				-callback ::ProjectFacility::guiClose \
				-bypath [list &File $_menuNames(fileAddProjectToWorkspace)] \
			 	$_menuNames(fileCloseWorkspace)
		}
	}

	proc projectMenuCreate {} {
		variable _menuNames

		menuItemInsert -name projectMenu \
			-popup -after -bypath {&View} $_menuNames(projectMenu) 
		menuItemAppend projectMenu -name projectAddMenu -popup \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				projectAddMenu] $_menuNames(projectAddMenu)
			menuItemAppend projectAddMenu \
				-callback [list ::ProjectFacility::staticOnFileAdd] \
				-cmduicallback [list ::ProjectFacility::cmdUICallback \
					projectAddFile] $_menuNames(projectAddFile)
			menuItemAppend projectAddMenu \
				-callback [list ::ProjectFacility::staticOnComponentAdd] \
				-cmduicallback [list ::ProjectFacility::cmdUICallback \
					projectAddComponent] $_menuNames(projectAddComponent)
			menuItemAppend projectAddMenu \
				-callback [list ::ProjectFacility::staticOnBuildAdd] \
				-cmduicallback [list ::ProjectFacility::cmdUICallback \
					projectAddBuild] $_menuNames(projectAddBuild)

		menuItemAppend projectMenu -name projectDeleteMenu -popup \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				projectDeleteMenu] $_menuNames(projectDeleteMenu)
			menuItemAppend projectDeleteMenu \
				-callback [list ::ProjectFacility::staticOnFileDelete] \
				-cmduicallback [list ::ProjectFacility::cmdUICallback \
					projectDeleteFile] $_menuNames(projectDeleteFile)
			menuItemAppend projectDeleteMenu \
				-callback [list ::ProjectFacility::staticOnComponentDelete] \
				-cmduicallback [list ::ProjectFacility::cmdUICallback \
					projectDeleteComponent] $_menuNames(projectDeleteComponent)
			menuItemAppend projectDeleteMenu \
				-callback [list ::ProjectFacility::staticOnBuildDelete] \
				-cmduicallback [list ::ProjectFacility::cmdUICallback \
					projectDeleteBuild] $_menuNames(projectDeleteBuild)
			menuItemAppend projectDeleteMenu \
				-callback [list ::ProjectFacility::staticOnProjectDelete] \
				-cmduicallback [list ::ProjectFacility::cmdUICallback \
					projectDeleteProject] $_menuNames(projectDeleteProject)

		menuItemAppend projectMenu \
      -accelerator {SHIFT F6} \
			-callback [list ::ProjectFacility::staticOnDownload] \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				projectDownload] $_menuNames(projectDownload)
		menuItemAppend projectMenu \
			-callback [list ::ProjectFacility::staticOnAutoScale] \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				projectAutoScale] $_menuNames(projectAutoScale)
		menuItemAppend projectMenu \
			-callback ::ProjectFacility::staticOnFindComponent \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				projectFindComponent] $_menuNames(projectFindComponent)
	}

	# Creates the build menu and toolbar
	proc buildMenuAndToolbarCreate {} {
		variable _menuNames
		variable _tbBitmaps
		variable _toolTips

		# Create the build menu
		menuItemInsert -name buildMenu \
			-popup -after -bypath $_menuNames(projectMenu) \
			$_menuNames(buildMenu)

		# Create the build toolbar
		toolbarCreate mainwindow -title "Build" -name buildTB -any

		# Add menu items and buttons in pairs

		# Build
		menuItemAppend buildMenu \
			-callback ::ProjectFacility::staticOnBuild \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				buildBuild] $_menuNames(buildBuild)
		controlCreate buildTB [list toolbarbutton -name buildButton \
			-callback ::ProjectFacility::staticOnBuild \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				buildBuild] \
			-tooltip $_toolTips(buildBuild) \
			-bitmap $_tbBitmaps(buildBuild)]

		# Rebuild all
		menuItemAppend buildMenu \
			-callback ::ProjectFacility::staticOnRebuildAll \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				buildRebuildAll] $_menuNames(buildRebuildAll)
		controlCreate buildTB [list toolbarbutton -name rebuildAllButton \
			-callback ::ProjectFacility::staticOnRebuildAll \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				buildBuild] \
			-tooltip $_toolTips(buildRebuildAll) \
			-bitmap $_tbBitmaps(buildRebuildAll)]

		# Compile current source file
		menuItemAppend buildMenu \
			-callback ::ProjectFacility::staticOnCompile \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				buildCompile] $_menuNames(buildCompile)
		controlCreate buildTB [list toolbarbutton \
			-name buildCompileButton \
			-callback ::ProjectFacility::staticOnCompile \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				buildCompile] \
			-tooltip $_toolTips(buildCompile) \
			-bitmap $_tbBitmaps(buildCompile)]

		# Update dependencies
		menuItemAppend buildMenu \
			-callback ::ProjectFacility::staticOnDependencies \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				buildDependencies] $_menuNames(buildDependencies)
		controlCreate buildTB [list toolbarbutton \
			-name buildDependenciesButton \
			-callback [list ::ProjectFacility::staticOnDependencies] \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				buildDependencies] \
			-tooltip $_toolTips(buildDependencies) \
			-bitmap $_tbBitmaps(buildDependencies)]

		# Stop build
		if {[string match [wtxHostType] "x86-win32"]} {
			menuItemAppend buildMenu \
				-callback ::ProjectFacility::staticOnStopBuild \
				-cmduicallback [list ::ProjectFacility::cmdUICallback \
					buildStopBuild] $_menuNames(buildStopBuild)
			controlCreate buildTB [list toolbarbutton \
				-name buildStopButton \
				-callback [list ::ProjectFacility::staticOnStopBuild] \
				-cmduicallback [list ::ProjectFacility::cmdUICallback \
					buildStopBuild] \
				-tooltip $_toolTips(buildStopBuild) \
				-bitmap $_tbBitmaps(buildStopBuild)]
		}

		controlCreate buildTB [list toolbarbutton \
			-name downloadButton \
			-separator \
			-callback [list ::ProjectFacility::staticOnDownload] \
			-cmduicallback [list ::ProjectFacility::cmdUICallback \
				projectDownload] \
			-tooltip $_toolTips(projectDownload) \
			-bitmap $_tbBitmaps(projectDownload)]

		# Menu item for the view menu
		if {[string match [wtxHostType] "x86-win32"]} {
			set vtb viewToolbarPC
		} else {
			set vtb viewToolbarUNIX
		}
		menuItemAppend \
			-callback ::ProjectFacility::onToggleBuildTB \
			-cmduicallback ::ProjectFacility::onCmdUIBuildTB \
			-bypath $_menuNames($vtb) \
			$_menuNames(viewBuildTB)
	}

        # set WIND_PROJ_BASE and WIND_SOURCE_BASE from the registry values
        proc environmentVariablesSet {} {
	    set windProjBase [appRegistryEntryRead \
                -default [file nativename [wtxPath target proj]]  \
	        Workspace WindProjBase]
	    set windSourceBase [appRegistryEntryRead \
                -default ""  \
	        Workspace WindSourceBase]
            filePathFix windProjBase
            filePathFix windSourceBase

            set ::env(WIND_PROJ_BASE) $windProjBase
            set ::env(WIND_SOURCE_BASE) $windSourceBase
        }

	# Show/hide the build toolbar
	proc onToggleBuildTB {} {
	    if {[windowVisible buildTB] == 1} {
	        windowShow buildTB 0
	    } else {
	        windowShow buildTB 1
	    }
	}

	# Determine whether the View/Toolbar/Build Toolbar menu item
	# is checked
	proc onCmdUIBuildTB {} {
	    global checkflag
	    set checkflag [windowVisible buildTB]
	}

	# Determine whether any number of the following toolbar buttons
	# and menu items are enabled
	proc cmdUICallback {itemName} {
    	global enableflag
    	global checkflag

		set enableflag 0
		set checkflag 0

		set handle [::Workspace::selectionGet]
		set type [::Object::evaluate $handle Type]
		set bWorkspaceObject 0
		set bNullObject 0
		if {[string match $type nullObject] || \
			[string match $type deadProjectObject]} {
			set bNullObject 1
		} elseif {[string match $type workspaceObject]} {
			set bWorkspaceObject 1
		}

		switch $itemName {
			projectMenu {
				# Not implemented
			}
			projectAddMenu {
				if {[expr !$bNullObject]} {
					set enableflag 1
				}
			}
			projectAddFile {
				if {[expr !$bNullObject] && \
					![string match workspaceObject $type]} {
					set enableflag 1
				}
			}
			projectAddComponent {
				switch $type {
					componentObject -
					componentFolderObject {
						set enableflag 1
					}
				}
			}
			projectAddBuild {
				if {[expr !$bNullObject] && \
					![string match workspaceObject $type]} {
					set enableflag 1
				}
			}
			projectDeleteMenu {
				if {[expr !$bNullObject]} {
					set enableflag 1
				}
			}
			projectDeleteFile {
				switch $type {
					derivedObject -
					sourceFileObject {
						set enableflag 1
					}
				}
			}
			projectDeleteComponent {
				switch $type {
					componentObject -
					componentFolderObject {
						set enableflag 1
					}
				}
			}
			projectDeleteBuild {
				if {[string match $type buildObject]} {
					set enableflag 1
				}
			}
			projectDeleteProject {
				if {[string match $type projectObject] || \
					[string match $type deadProjectObject]} {
					set enableflag 1
				}
			}
			projectAutoScale {
				if {!$bNullObject && !$bWorkspaceObject} {
					set hProj [::Object::evaluate \
						[::Object::evaluate $handle Project] Handle]
					if {[::prjHasComponents $hProj]} {
						set enableflag 1
					}
				}
			}
			projectFindComponent {
				if {!$bNullObject && !$bWorkspaceObject} {
					set hProj [::Object::evaluate \
						[::Object::evaluate $handle Project] Handle]
					if {[::prjHasComponents $hProj]} {
						set enableflag 1
					}
				}
			}
			projectDownload {
				if {![::isBuildActive] && \
				    ![string match $type deadProjectObject] && \
					!$bWorkspaceObject} {
					set enableflag 1
				}
			}
			buildMenu {
				# Not implemented
			}
			buildBuild {
				if {!$bNullObject && !$bWorkspaceObject} {
					set hProj [::Object::evaluate \
						[::Object::evaluate $handle Project] Handle]
					set build [::prjBuildCurrentGet $hProj]
					if {$build != ""} {
						if {![::isBuildActive]} {
							set enableflag 1
						}
					}
				}
			}
			buildRebuildAll {
				if {!$bNullObject && !$bWorkspaceObject} {
					set hProj [::Object::evaluate \
						[::Object::evaluate $handle Project] Handle]
					set build [::prjBuildCurrentGet $hProj]
					if {$build != ""} {
						if {![::isBuildActive]} {
							set enableflag 1
						}
					}
				}
			}
			buildDependencies {
				if {!$bNullObject && !$bWorkspaceObject} {
					if {![::isBuildActive]} {
						set enableflag 1
					}
				}
			}
			buildCompile {
				if {![::isBuildActive]} {
					if {[string match $type sourceFileObject]} {
						set enableflag 1
					}
				}
			}
			buildStopBuild {
				if {[::isBuildActive]} {
					set enableflag 1
				}
			}
			fileNewWorkspace {
				set enableflag 1
			}
			fileOpenWorkspace {
				set enableflag 1
			}
			fileCloseWorkspace {
				if {[::Workspace::openWorkspaceGet] != ""} {
					set enableflag 1
				}
			}
			fileSaveWorkspace {
				if {[::Workspace::workspaceDirtyGet]} {
					set enableflag 1
				}
			}
			fileSaveAsWorkspace {
				set enableflag 1
			}
			fileSaveAsProject {
				if {[::Workspace::currentProjectNameGet] != ""} {
					set enableflag 1
				}
			}
			fileAddProjectToWorkspace {
				if {[::Workspace::openWorkspaceGet] != ""} {
					set enableflag 1
				}
			}
		}
		return $enableflag
	}

	#############################################################
	#
	# Use the following API to open, close and save workspaces 
	#
	#############################################################

	proc guiNew {} {
		::WelcomeDlg::welcomeDlgShow "New"
	}

	proc guiOpen {} {
		::WelcomeDlg::welcomeDlgShow "Existing"
	}

	proc guiSave {} {
		guiWorkspaceSave
	}

	proc guiSaveAs {} {
		guiWorkspaceSave "" 1
	}

	proc guiSaveProjectAs {} {
		set prjName [::Workspace::currentProjectNameGet]
		set hOldProj [::Workspace::projectHandleGet $prjName]
		guiSaveProjectAsFromHandle $hOldProj
	}

	proc guiRenameProject { {strProjectFullPath ""} } {
		if { $strProjectFullPath == "" } {
			set strProjectFullPath [::Workspace::currentProjectNameGet]
		}
		set strProjectName [file rootname [file tail $strProjectFullPath]]
		set strNewName [DialogRename::DoModal project $strProjectName]
		
		if { $strNewName != "" } {
			set strNewProjectFullPath \
				[file join [file dirname $strProjectFullPath] $strNewName.wpj]
			if {[file exists $strNewProjectFullPath] && \
				[messageBox -yesno -exclamationicon \
					 "[file nativename $strNewProjectFullPath] exists.  \
					Overwrite?"] != "yes"} {
				return ;#abort
			}
			if {[catch {
				if {[file isdirectory $strNewProjectFullPath]} {
					error "[file nativename $strNewProjectFullPath] \
						already exists and is a directory."
				}
				# first save the old project to disk
				set hProj [Workspace::projectHandleGet $strProjectFullPath]
				guiPrjSave::guiPrjSave $hProj
				# now rename the files
			    file rename -force -- $strProjectFullPath $strNewProjectFullPath
				Workspace::projectAdd $strNewProjectFullPath
				Workspace::projectDelete $strProjectFullPath
				# set the modified flag for the new project
				set hProj [Workspace::projectHandleGet $strNewProjectFullPath]
				prjInfoSet $hProj changed [clock seconds]
			} strError]} {
				messageBox "Failed to rename $strProjectName:\n$strError"
			}
		}
		
	}		

	proc guiClose {} {
		if {[::ViewBrowser::queryClose]} {
			if {$::ViewBrowser::_hideOnClose} {
				::ViewBrowser::viewBrowserClose
			}
		}
	}

	proc guiSaveProjectAsFromHandle {hOldProj} {
		::guiPrjSave::guiPrjSave $hOldProj "" 1	
	}

	###########################################################################
	#
	# guiWorkspaceOpen is designed to be a "one-shot" call from C code to 
	# implement:
	# 
	# 1. drag-and-drop of a workspace onto Tornado
	# 2. double-clicking on a workspace to launch Tornado
	# 
	# open new workspace, creating if necessary. 
	# Prompt to save previous workspace (if any)
	#
	proc guiWorkspaceOpen {workspaceFile} {
		if [ catch {
			::filePathFix workspaceFile
			# Validate file name first
			if {![::isFileNameOk $workspaceFile]} {
				error "'[file nativename $workspaceFile]':\
					\ncontains invalid characters.  Rename file\
					before opening."
			}

			if {[::Workspace::openWorkspaceGet] == "" || \
				[::ViewBrowser::queryClose]} {
				::Workspace::createOrOpenWorkspace $workspaceFile
			}
		} error ] {
			messageBox -ok "Problem opening $workspaceFile:\n\n$error"
		}
	}

	###########################################################################
	#
	# guiProjectOpen is designed to be a "one-shot" call from C code to 
	# implement:
	# 
	# 1. drag-and-drop of a Project onto Tornado
	# 2. double-clicking on a Project to launch Tornado
	# 
	#
	# open or refresh project in current workspace, or in a new
	# workspace if there is no open workspace
	#
	proc guiProjectOpen {projectFile} {
		if [catch {
			if {[::Workspace::openWorkspaceGet] == ""} {
				set workspaceFile [::Workspace::newWorkspaceFileNameGet]
				::Workspace::workspaceCreate $workspaceFile
			}

			::filePathFix projectFile
			# Validate file name first
			if {![::isFileNameOk $projectFile]} {
				error "'[file nativename $projectFile]':\
					\ncontains invalid characters.  Rename file\
					before opening."
			}

			::Workspace::projectAddOrRefresh $projectFile
		} error ] {
			messageBox -ok "Problem opening $projectFile:\n\n$error"
		}
	}

	###########################################################################
	#
	# Save and SaveAs.  SaveAs writes the workspace document but leaves
	# the projects dirty; save saves projects and workspace
	#
	# Calls ::Workspace::workspaceSave, hooking in guiPrjSave to deal 
	# with project file being read-only.
	# If a project file is read-only, the user is prompted to save it to 
	# a different name. The new project is NOT added to the current workspace.
	#
	# RETURNS: the new workspace fileName, or "" if the operation is cancelled
	#
	proc guiWorkspaceSave {{fileName ""} {fForceSaveAs 0}} {
		if {$fForceSaveAs == "1"} {
			::guiPrjSave::guiPrjForceWorkspaceSaveAs 
		}
		# Note: must protect fileName argument with quotes since it may be blank
                set returnValue ""
		catch {::Workspace::workspaceSave "$fileName" ::guiPrjSave::guiPrjSave} returnValue 
                return $returnValue
	}

	#############################################################
	#
	# The following procs provide the same services a routines
	# in various view servers, but they are made to do late-binding
	# and error checking
	#
	#############################################################

	proc staticOnFileAdd {} {
		set handle [::Workspace::selectionGet]
		set type [::Object::evaluate $handle Type]
		switch $type {
			nullObject {
				error "current selection is nullObject; can't determine \
					project to which file should be added"
			}
			default {
				::FileViewServer::gui::OnFileAdd insert $handle
			}
		}
	}

	proc staticOnFileDelete {} {
		set handle [::Workspace::selectionGet]
		set type [::Object::evaluate $handle Type]
		switch $type {
			sourceFileObject {
				::FileViewServer::gui::OnFileDelete delete $handle
			}
			derivedObject {
				::FileViewServer::gui::OnFileRemove delete $handle
			}
		}
	}

	proc staticOnComponentAdd {} {
		set handle [::Workspace::selectionGet]
		set type [::Object::evaluate $handle Type]
		switch $type {
			componentFolderObject {	
				::ComponentViewServer::gui::OnFolderAdd insert $handle
			}
			componentObject {
				::ComponentViewServer::gui::OnComponentAdd insert $handle
			}
		}
	}

	proc staticOnComponentDelete {} {
		set handle [::Workspace::selectionGet]
		set type [::Object::evaluate $handle Type]
		switch $type {
			componentFolderObject {	
				::ComponentViewServer::gui::OnFolderDelete delete $handle
			}
			componentObject {
				::ComponentViewServer::gui::OnComponentDelete insert $handle
			}
		}
	}

	proc staticOnBuildAdd {} {
		set handle [::Workspace::selectionGet]
		set type [::Object::evaluate $handle Type]
		if {![string match $type nullObject]} {
			::BuildViewServer::gui::OnNewBuild insert $handle
		}
	}

	proc staticOnBuildDelete {} {
		set handle [::Workspace::selectionGet]
		set type [::Object::evaluate $handle Type]
		if {[string match $type buildObject]} {
			::BuildViewServer::gui::OnBuildDelete delete $handle
		}
	}

	proc staticOnProjectDelete {} {
		set handle [::Workspace::selectionGet]
		set type [::Object::evaluate $handle Type]
		if {[string match $type projectObject] || \
			[string match $type deadProjectObject] } {
			::FileViewServer::gui::OnProjectDelete delete $handle	
		}
	}

	proc staticOnAutoScale {} {
		set handle [::Workspace::selectionGet]
		set type [::Object::evaluate $handle Type]
		if {![string match $type nullObject]} {
			::ComponentViewServer::gui::OnAutoScale "" $handle
		}
	}

	proc staticOnDownload {} {
		set handle [::Workspace::selectionGet]

		# Get a default directory from which to present the download
		# dialog
		set defaultDir ""
		if {![string match $handle $::Workspace::_NULL_OBJECT]} {
			set hProj [::Object::evaluate \
				[::Object::evaluate $handle Project] Handle]
			set defaultDir [::Workspace::projectBuildDirGet $hProj]
		}

    	set prevDwnLdDir [appRegistryEntryRead \
			-default $defaultDir Download LastDir]
    	set saveCwd [pwd]
    	catch {cd $prevDwnLdDir}
    	set files [fileDialogCreate -filemustexist \
    	        -multiselect \
    	        -pathmustexist \
    	        -noreadonlyreturn \
    	        -filefilters \
    	        "Object Files (*.o;*.out;*.)|*.o;*.out|All Files (*.*)|*.*||" \
    	        -title "Download objects" \
    	        -okbuttontitle "&Download"]
    	catch {cd $saveCwd}

		if {$files != ""} {
			foreach file $files {
				regsub -all {\\} $file {/} file
				lappend fileList $file
			}
			::FileViewServer::gui::projectFilesDownload "" $fileList
		}
	}

	proc staticProjectToWorkspaceAdd {} {
		# Get project directory and use it as cwd
		set workspace [::Workspace::openWorkspaceGet]
		set workspaceName [file rootname \
			[file tail $workspace]]
		set newwd [file dirname [::Workspace::openWorkspaceGet]]
		set cwd [pwd]
		cd $newwd
		set filter "Project Files(*.wpj)|*.wpj||"
		set fileNames [fileDialogCreate \
			-filefilters $filter \
			-pathmustexist -filemustexist \
			-title "Add Project(s) to Workspace '$workspaceName'" \
			-okbuttontitle "Add" -multisel]
		::beginWaitCursor
		cd $cwd
		if {[llength $fileNames]} {

			# Fix the path, then check for duplicates.  If not a dup,
			# add project to workspace
			set projectsInWorkspace \
				[::Workspace::objectsEnumerate "" -containers]
			if {$projectsInWorkspace != ""} {
				set hProj [::Workspace::projectHandleGet \
					[lindex $projectsInWorkspace 0]]
			} else {
				set hProj ""
			}
			::filePathFix fileNames $hProj
			set projectFile [lindex $fileNames 0]

			# Validate file name first
			if {[::isFileNameOk $projectFile]} {
				if {[lsearch $projectsInWorkspace $projectFile] == -1} {
					if {[catch {::Workspace::projectAdd $projectFile} error]} {
						messageBox -ok "$error"
					}
				} else {
					messageBox -ok -exclamationicon "Project '$projectFile' is\
						already in your workspace!"
				}
			} else {
				messageBox -ok "'$projectFile':\
					\ncontains invalid characters.  Rename file\
					before adding."
			}
		}
		::endWaitCursor
		
	}

	proc staticOnFindComponent {} {
		set handle [::Workspace::selectionGet]
		set type [::Object::evaluate $handle Type]
		if {![string match $type nullObject]} {
			::ComponentViewServer::gui::OnComponentFind "" $handle
		}
	}

	proc staticOnBuild {} {
		set handle [::Workspace::selectionGet]
		set type [::Object::evaluate $handle Type]
		if {![string match $type nullObject]} {
			set projObj [::Object::evaluate $handle Project]
			::BuildViewServer::gui::OnProjectBuild $projObj
		}
	}

	proc staticOnRebuildAll {} {
		set handle [::Workspace::selectionGet]
		set type [::Object::evaluate $handle Type]
		if {![string match $type nullObject]} {
			set projObj [::Object::evaluate $handle Project]
			::BuildViewServer::gui::OnProjectReBuildAll $projObj
		}
	}

	proc staticOnDependencies {} {
		set handle [::Workspace::selectionGet]
		set type [::Object::evaluate $handle Type]
		if {![string match $type nullObject]} {
			set hProj [::Object::evaluate \
				[::Object::evaluate $handle Project] Handle]
			::Dependencies::OnDependencies $hProj $handle
		}
	}

	proc staticOnCompile {} {
		set handle [::Workspace::selectionGet]
		set type [::Object::evaluate $handle Type]

		# Deal with the cases of there being a file object, or not,
		# and an editor file, or not, in combination
		if {[string match $type sourceFileObject]} {
			::FileViewServer::gui::OnFileCompile $handle
		} else {
			messageBox -ok "Please add this file to a project."
		}
	}

	proc staticOnStopBuild {} {
		::buildStop
	}

	proc notImplemented {} {
		messageBox -ok "Sorry, not implemented yet. \
			\nTry context menu in Project Facility"
	}
}

##########################################################################################
#
# Namespace ::guiPrjSave implements a gui save function for project files and workspace
# files.
#
# ENTRY POINT: ::guiPrjSave::guiPrjSave
#

namespace eval ::guiPrjSave {

	variable prjName
	variable objType
	variable fileFilters
	variable project
	variable extension
	variable readOnlyMessage
	variable currentPrjFileName
	variable nameOfObjectType
	variable SaveOrSaveAs
	variable newPrjFileName
	variable error
	variable saveStatus
	variable saveAsOrBackupString
	variable fForceWorkspaceSaveAs 0

	##########################################################################################
	#
	# ::guiPrjSave::guiPrjForceWorkspaceSaveAs {} forces the next workspace save through
	# ::guiPrjSave::guiPrjSave
	# to be a "save as", even if fileName is blank
	#
	# RETURNS: None
	#
	proc guiPrjForceWorkspaceSaveAs {} {
		variable fForceWorkspaceSaveAs
		set fForceWorkspaceSaveAs 1
	}

	##########################################################################################
	#
	# ::guiPrjSave::guiPrjSave {hProj {fileName ""} {fForceSaveAs 0}} saves a project file or workspace file. 
	# 
	# If hProj represents a workspace file only the workspace file itself is 
	# saved, not the projects therein.
	#
	# If the fileName argument is specified, or if fForceSaveAs is set to 1, we do a Save As. 
	#
	# If a project or workspace file is read-only, the user is prompted to save it to 
	# a different name. If it is a project, copy to a backup directory. If it is 
	# a workspace, copy the workspace document only.
	#
	# 
	#
	# RETURNS: The new filename, or "" if the operation is cancelled
	#
	proc guiPrjSave {hProj {fileName ""} {fForceSaveAs 0}} {

		variable prjName
		variable objType
		variable fileFilters
		variable project
		variable extension
		variable readOnlyMessage
		variable currentPrjFileName
		variable nameOfObjectType
		variable SaveOrSaveAs
		variable newPrjFileName
		variable newDescription
		variable newWorkspaceFileName
		variable error
		variable saveStatus
		variable fForceWorkspaceSaveAs
		
		set cwd [pwd]

		setUp
		handleObjectType
		decideWhetherSaveOrSaveAs

		if {$newPrjFileName == ""} {
			set saveStatus "getNewFileName"
		} else {
			set saveStatus "testForReadonly"
		}

		while {$saveStatus != "done"} {
			switch $saveStatus {
				testForReadonly {
					doTestForReadonly
				}
				readonlyNotify {
					doReadonlyNotify
				}
				getNewFileName {
					doGetNewFileName
				}
				checkFileName {
					doCheckFileName
				}
				trySave {
					doTrySave
				}
				trySaveAs {
					doTrySaveAs
				}
				cancel {
					break
				}
				error {
					break
				}
				default {
					doDefault
				}
			}
		}

		cd $cwd
		if {$saveStatus == "done"} {
			::Workspace::statusMessagePush "Saving $nameOfObjectType $prjName...done"
			return $newPrjFileName
		} elseif {$saveStatus == "cancel"} {
			::Workspace::statusMessagePush "Saving $nameOfObjectType $prjName...cancelled"
			return ""
		} else {
			::Workspace::statusMessagePush "Saving $nameOfObjectType $prjName...FAILED"
			error "$error"
		}
	}
	#####################################################################
	#
	# Define Helper functions...
	#
	proc setUp {} {
		upvar hProj hProj
		upvar fileName fileName
		variable prjName
		variable currentPrjFileName
		variable error

		set error ""
		set prjName [::prjNameGet $hProj]
		set currentPrjFileName [::prjInfoGet $hProj fileName]
		::filePathFix currentPrjFileName
	}
	proc handleObjectType {} {
		upvar hProj hProj
		variable objType
		variable fileFilters
		variable project
		variable nameOfObjectType
		variable extension
		variable readOnlyMessage
		variable currentPrjFileName
		variable saveAsOrBackupString

		# Workspace object
		if {$hProj == [::WorkspaceDoc::workspaceHandleGet]} {
			set objType [::WorkspaceDoc::Type]
		# Project object
		} else {
			set workspaceName [::Workspace::objectsEnumerate "" -containers]
			set prjObject [::Workspace::objectBind \
				[list $workspaceName $currentPrjFileName]]
			set objType [::Object::evaluate $prjObject Type]
		}

		if {$objType == "projectObject"} {
			set fileFilters "Project Files(*.wpj)|*.wpj||"
			set nameOfObjectType "project"
			set extension ".wpj"
			set readOnlyMessage "This project file is read-only. Save your changes to a different directory?"
			set saveAsOrBackupString "Backup your changes to"
		} elseif {$objType == "workspaceObject"} {
			set fileFilters "Workspace Files(*.wsp)|*.wsp||"
			set nameOfObjectType "workspace"
			set extension ".wsp"
			set readOnlyMessage "This workspace file is read-only. Save your changes to a different filename?"
			set saveAsOrBackupString "Save"
		} elseif {$objType == "deadProjectObject"} {
			# Nothing to do, so return
			# (return from guiPrjSave as well)
			return -code return $currentPrjFileName
		} else {
			error "::guiPrjSave::guiPrjSave: Cannot save object of type $objType"
		}
	}
	proc decideWhetherSaveOrSaveAs {} {
		upvar hProj hProj
		upvar fileName fileName
		upvar fForceSaveAs fForceSaveAs
		variable objType
		variable SaveOrSaveAs
		variable newPrjFileName
		variable nameOfObjectType
		variable currentPrjFileName
		variable fForceWorkspaceSaveAs
		variable prjName

		set fForceThisWorkspaceSaveAs 0
		if {$fForceWorkspaceSaveAs == "1" && \
			$objType == "workspaceObject"} {
			# clear global flag, it's one-shot only
			set fForceWorkspaceSaveAs 0
			# set local flag
			set fForceThisWorkspaceSaveAs 1
		}

		if {$fileName != "" \
			|| $fForceSaveAs == "1" \
			|| $fForceThisWorkspaceSaveAs == "1"} {
			set SaveOrSaveAs "SaveAs"
			set newPrjFileName $fileName
			::Workspace::statusMessagePush "Saving $nameOfObjectType $prjName As..."
		} else {
			set SaveOrSaveAs "Save"
			set newPrjFileName $currentPrjFileName
			# If this is just a save, check whether we have anything to do...
			if {[::prjInfoGet $hProj changed] == ""} {
				# return from guiPrjSave as well
				return -code return $currentPrjFileName
			}
			::Workspace::statusMessagePush "Saving $nameOfObjectType $prjName..."
		}
	}
	###################################################################
	#
	# "do..." procedures implement actions in the state machine
	# (switch statement) below. They must take an action and change the 
	# state of saveStatus
	#
	proc doTestForReadonly {} {
		variable saveStatus
		variable newPrjFileName

		if {[file exists $newPrjFileName] && ![file writable $newPrjFileName]} {
			set saveStatus "readonlyNotify"
		} else {
			set saveStatus "checkFileName"
		}
	}
	proc doReadonlyNotify {} {
		variable saveStatus
		variable SaveOrSaveAs
		variable newPrjFileName
		variable readOnlyMessage

		set reply [messageBox -okcancel "$newPrjFileName\n\n$readOnlyMessage"]
		if {$reply == "cancel"} {
			set saveStatus "cancel"
		} else {
			set SaveOrSaveAs "SaveAs"
			set saveStatus "getNewFileName"
		}
	}
	proc doGetNewFileName {} {
		upvar hProj hProj
		variable saveStatus
		variable SaveOrSaveAs
		variable newPrjFileName
		variable currentPrjFileName
		variable fileFilters
		variable objType
		variable saveAsOrBackupString
		variable newDescription
		variable newWorkspaceFileName

		# Attempt to change to the directory where we last were...
		if {$newPrjFileName != ""} {
			catch { cd [file dirname $newPrjFileName] }
		} else {
			catch { cd [file dirname $currentPrjFileName] }
		}
		set newDescription ""
		if {$objType == "workspaceObject"} {
			set fileDialogFlags "-savefile -noreadonlyreturn"
			if {$SaveOrSaveAs == "SaveAs"} {
				set fileDialogFlags "$fileDialogFlags -overwriteprompt"
			}
			set newPrjFileName [eval fileDialogCreate $fileDialogFlags \
				[list -filefilters $fileFilters \
				-title "$saveAsOrBackupString [file tail $currentPrjFileName] As" -okbuttontitle "Ok" ]\
				 ]
			# fileDialogCreate returns a list (with {}) and we only want one name
			set newPrjFileName [join $newPrjFileName]
			::filePathFix newPrjFileName
		} else {
			::ProjectSaveAs::getNewNameAndDescription $hProj
			# Get data back from ProjectSaveAs
			set newPrjFileName [::ProjectSaveAs::getFullPath]
			set newDescription [::ProjectSaveAs::getDescription]
			set newWorkspaceFileName [::ProjectSaveAs::getWorkspaceFileName]
		}
		set saveStatus "checkFileName"
	}
	proc doCheckFileName {} {
		variable saveStatus
		variable SaveOrSaveAs
		variable newPrjFileName
		variable extension
		if {[file tail $newPrjFileName] == ""} {
			set saveStatus "cancel"
			return
		}

		# Remove spaces from path 
		regsub -all [::badFileCharsGet] $newPrjFileName "_" newPrjFileName 

		if {[file extension $newPrjFileName] == $extension} {
			set saveStatus "try$SaveOrSaveAs"
		} elseif {[file extension $newPrjFileName] == ""} {
			set newPrjFileName "${newPrjFileName}${extension}"
			set saveStatus "try$SaveOrSaveAs"
		} else {
			set reply [messageBox -okcancel "[file tail $newPrjFileName]: New file name must have extension $extension"]
			if {$reply == "cancel"} {
				set saveStatus "cancel"
			} else {
				set saveStatus "getNewFileName"
			}
		}
	}
	proc doTrySave {} {
		variable saveStatus
		variable error
		upvar hProj hProj
		if [catch {::prjSave $hProj} error] {
			set reply [messageBox -okcancel "Problem saving project [prjNameGet $hProj]:\n\n$error"]
			if {$reply == "cancel"} {
				set saveStatus "cancel"
			} else {
				set saveStatus "error"
			}
		} else {
			set saveStatus "done"
		}
	}
	proc doTrySaveAs {} {
		variable saveStatus
		variable error
		variable newPrjFileName
		variable objType
		variable newDescription
		variable newWorkspaceFileName

		upvar hProj hProj
		if {$objType == "workspaceObject"} {
			if [catch {::prjSaveAs $hProj $newPrjFileName 0} error] {
				set saveStatus "cancel"
			} else {
				set saveStatus "done"
			}
		} elseif {$objType == "projectObject"} {
			if [catch {
				# Copy project and add it to the workspace
				::prjCopy $hProj $newPrjFileName 0 $newDescription

				# Close workspace if user has chosen to add to a different workspace
				if {[::Workspace::openWorkspaceGet] != "" && \
					[::Workspace::openWorkspaceGet] != "$newWorkspaceFileName"} {
					if {[::ViewBrowser::queryClose] == 0} {
						# if workspace is dirty and user cancels out of save, abort the operation
						# here (project has been created but not added to any workspace...)
						set saveStatus "cancel"
					}
				}
				
				if {$saveStatus != "cancel"} {
					::Workspace::createOrOpenWorkspace $newWorkspaceFileName
					::Workspace::projectAddOrRefresh $newPrjFileName
					set hNewProj [::Workspace::projectHandleGet $newPrjFileName]

					# Save the workspace document since we have added a new project
					::WorkspaceDoc::workspaceSave "" ::guiPrjSave::guiPrjSave
					set saveStatus "done"
				}
			} error] {
				set reply [messageBox -okcancel "Project Save As Failed with error:\n\n$error\n\nTry again?"]
				if {$reply == "cancel"} {
					set saveStatus "cancel"
				} else {
					set saveStatus "getNewFileName"
				}
			}
		}
	}
	proc doDefault {} {
		variable saveStatus
		set error "::guiPrjSave::guiPrjSave: exited loop when saveStatus = $saveStatus"
		set saveStatus "cancel"
	}
}

#############################################################
#
# The entry point
#
#############################################################

namespace eval :: {

	# XXX -- Maybe move this to Tornado.win32.tcl?
	if {[info exists bUseTclPro]} {
		source c:/tcl80/bin/tclpro1.0b5w/bin/prodebug.tcl 
		debugger_init
	}

	# Create the debug command window
	if {[appRegistryEntryRead -default 0 \
		Workspace ShowDebugConsole] == 1} {
		::ProjectConsoleWindow::create
	}

        #
        # set up diab environment if diab is installed
        #
        set diabHostDir "NONE"
        switch [wtxHostType] {
	    sun4-solaris2 { set diabHostDir "SUNS" }
	    parisc-hpux10 { set diabHostDir "HPUX" }
	    x86-win32     { set diabHostDir "WIN32" }
        }
        set pathSep ":"
        if {[wtxHostType] == "x86-win32"} {
            set pathSep ";"
        }

        # assume diab is installed if host/diab directory exists

        if {[file exists [wtxPath host]diab] && \
            [file isdirectory [wtxPath host]diab] } {
            set ::env(DIABLIB) [file nativename [wtxPath host]diab]
            set ::env(PATH) \
                [file nativename \
                    [wtxPath host diab $diabHostDir]bin]${pathSep}$::env(PATH)
        }

	# source the data layer and some primitives
	source "[wtxPath host resource tcl app-config Project]Utils.tcl"
	source "[wtxPath host resource tcl app-config Project]prjLib.tcl"
	source "[wtxPath host resource tcl app-config Project]Workspace.tcl"
	source "[wtxPath host resource tcl app-config Project]Wizard.tcl"

	# Source the 'Project Save As' functionality
	source "[wtxPath host resource tcl app-config Project]ProjectSaveAs.tcl"

	# XXX -- This is the help facility, and it should be moved
	source "[wtxPath host resource doctools]windHelpLib.tcl"

	# Source the wizards
	set path "[wtxPath host resource tcl app-config Project]prjwiz_*.tcl"
	foreach file [glob $path] {
		source $file
	} 

	# Create and populate the Project and Build menus, and augment the
	# File menu
	if {[string match [wtxHostType] "x86-win32"]} {
		::ProjectFacility::fileMenuAugment "&File &Close"
	} else {
		::ProjectFacility::fileMenuAugment $::MainWindow::m_menuFilePath
		menuItemInsert -after -bypath -separator $::MainWindow::m_menuFilePath
	}
	::ProjectFacility::projectMenuCreate
	::ProjectFacility::buildMenuAndToolbarCreate

	# Connect the view servers
	::Workspace::serversConnect

	# Show the welcome dialog, if it hasn't been disabled
	source "[wtxPath host resource tcl app-config Project]WelcomeDlg.tcl"
    source "[wtxPath host resource tcl app-config Project]NewWindow.tcl"
    source "[wtxPath host resource tcl app-config Project]WorkspaceBackup.tcl"

    # set build environment variables

    ::ProjectFacility::environmentVariablesSet
}
