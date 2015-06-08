# FileViewServer.addin.tcl - 
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 03p,30may02,cjs  isSimulatorTc moved to tc_cplus.tcl
# 03o,10dec01,rbl  fix problem where can only download to vxsim0
# 03n,12nov01,cjs  Fix problem when fileAddDirectory doesn't exist anymore
# 03m,24oct01,cjs  Fix logic to remove duplicates from dependency list
# 03l,03oct01,cjs  Fix spr 23704 -- add memory to add file dialog
# 03k,24mar99,cjs  Make all view servers catch WorkspaceUpdate event
# 03j,23mar99,ren  Fixing SPR 25784: Garbage spews to stdout when right click.
# 03i,18mar99,ren  Adding & short cut to Start context menu item.
# 03h,16mar99,cjs  Fixing spr 25030: Hands are tied...
# 03g,05mar99,cjs  Fix bug introduced by addition of workspace object to view
# 03f,03mar99,cjs  Add workspace icon to all view servers
# 03e,03mar99,ren  Adding project rename context menu item.
# 03d,02mar99,ren  Adding unload object to the context menu.
# 03c,02feb99,rbl  Changes to handle missing or read-only projects
# 03b,15jan99,cjs  Validate source files before adding
# 03a,13jan99,jak  conditionally enable Add Files from project
# 02z,08dec98,ren  Adding project name to file add dialog title.
# 02y,30nov98,cjs  make OK/Cancel dialog do Yes/No for sim launch warning
# 02x,25nov98,cjs  fix selection logic for FileAdd in OnEvent()
# 02w,17nov98,cjs  changed 'Add File...' menu text 
# 02v,11nov98,cjs  cleaned up simulator startup logic in  projectFilesDownload
# 02v,10nov98,cjs  Handle a "DerivedObjectsUpdate" event so that the
#                  object modules folder can be updated; change menu item
#                  from 'Download' to 'Start' for simulator executables;
#                  fixed a bug that allowed a binary to be opened,
#                  crashing Tornado
# 02u,29oct98,cjs  Add hProj param to call to filePathFix() 
# 02t,19oct98,cjs  Changed ctx menu text 
# 02s,15oct98,cjs  make ctx menu item text more consistent
# 02r,02oct98,cjs  protect gui/non-gui interface w/ viewBrowserIsVisible()
#                  checks 
# 02q,30sep98,cjs  unknown cpu type (ie, no project) passed
#                  to download() is now -1
# 02p,30sep98,cjs  show/select newly added files 
# 02o,29sep98,cjs  add logic to download to start simulator if appropriate 
# 02n,23sep98,cjs  delete output image when file is added or deleted 
# 02m,21sep98,cjs  override object's color attributes 
# 02l,18sep98,cjs  removed CustomBuildDlg namespace; this is dead code 
# 02k,15sep98,cjs  catch ProjectUpdate event; catch/display error on fileAdd()
# 02j,14sep98,cjs  fixed bug in menuMatch() for derivedObject 
# 02i,09sep98,cjs  change default op (dblclk) for derivedObject to Properties
# 02h,02sep98,cjs  fix download for partially linked object 
# 02g,02sep98,cjs  removed custom build menu item; now in prop sheet
# 02f,19aug98,cjs  make objectsEnumerate return _NULL_OBJECT when things
#                  go south 
# 02e,14aug98,cjs  Reworked download; download default object if selection is
#                  not a derived object
# 02d,13aug98,cjs  Removed project add context menu item; moved rest
#                  of VC code into 01VersionControl.addin.tcl
# 02c,04aug98,jmp  reworked Custom Build facility to automatically launch and
#                  initialize the build rule editor.
# 02b,31jul98,jak  changed Ver. Ctrl opt display
# 02a,28jul98,cjs  overhaul to reflect new objects, support demand
#                  drill, etc.
# 01o,07jul98,cjs  removed rename project ctx menu
#                  renamed derived obj folder
# 01n,30jun98,jmp  Add {Custom build...} to the file view floating menu.
# 01m,30jun98,cjs  fix: derived objects added multiple times
# 01l,24jun98,cjs  added icon support; enabled download
# 01k,23jun98,cjs  added Path() to FolderObject 
# 01j,23jun98,cjs  convert backslashes to forward slashes in download;
#                  add wait cursor
# 01i,22jun98,cjs  added Path() to ProjectObject; add download menu item;
#                  changed cwd for file add; assume path for DerivedObject
# 01h,04jun98,jak  added version control items to menu
# 01g,01may98,cjs  updated for use with new tree control;
#					 fixed file add/delete
# 01f,09apr98,cjs  fixed Title() 
# 01e,06apr98,cjs  fixed bug in OnProjectDelete() 
# 01d,04apr98,cjs  move all project management to workspace 
# 01c,31mar98,cjs  got rid of unnecessary format() statements,
#                  started real implementation 
# 01b,27mar98,cjs  changed references to className(), objectType()
#                  to classTitle(), className(), respectively
# 01a,27mar98,cjs  written.
#
# DESCRIPTION
# This file implements the File View Server.  View Servers
# populate a pane in the view browser by fetching data from
# a lower-level data engine.  The cookies returned are actually
# valid object handles, making objectBind() a no-op.
# 

namespace eval ::FileViewServer {

	# Private variable to track folder names
	variable _staticFolderNames
	set _staticFolderNames(Dependencies) "External Dependencies"
	set _staticFolderNames(DerivedObjects) "Object Modules"

	#############################################################
	#
	# Addin implementation 
	#
	#############################################################

	proc Init {} {
		set pattern {(^Project.*$)|(^File.*$)|(^Build.*$)|(^DerivedObjectsUpdate$)|(^WorkspaceUpdate$)}
		::Workspace::eventRegister ::FileViewServer $pattern 
		::Workspace::serverRegister ::FileViewServer
		::ViewBrowser::menuServerRegister ::FileViewServer::gui \
			{	::FileViewServer::FolderObject \
				::FileViewServer::ProjectObject \
				::FileViewServer::FileObject \
				::FileViewServer::DependObject \
				::FileViewServer::DerivedObject \
			}

		# Register to handle particular gui events for particular objects
		::ViewBrowser::guiEventHandlerRegister "dblclk" "sourceFileObject" \
			::FileViewServer::gui::OnFileOpen
		::ViewBrowser::guiEventHandlerRegister "delete" "sourceFileObject" \
			::FileViewServer::gui::OnFileDelete
		::ViewBrowser::guiEventHandlerRegister "dblclk" "dependObject" \
			::FileViewServer::gui::OnFileOpen
		::ViewBrowser::guiEventHandlerRegister "dblclk" "derivedObject" \
			::FileViewServer::gui::OnFileProperties
		::ViewBrowser::guiEventHandlerRegister "delete" "derivedObject" \
			::FileViewServer::gui::OnFileRemove
		::ViewBrowser::guiEventHandlerRegister "delete" "projectObject" \
			::FileViewServer::gui::OnProjectDelete
	}

	proc OnEvent {cookie eventInfo} {
		set eventData [lindex $eventInfo 1]
		set eventName [lindex $eventInfo 0]

		if {[regexp {Project.*} $eventName]} {
			if {[::ViewBrowser::viewBrowserIsVisible]} {

				set root [lindex [objectsEnumerate "" -containers] 0]
				set firstChild [lindex [objectsEnumerate \
					$root {-leaves -containers}] 0]
				set cookieLists [list ""]

				# If this is a project add, it may be the first one.
				# Be sure the project is visible
				if {[string match "ProjectAdd" $eventName]} {
					::ViewBrowser::OnWatch ::FileViewServer::gui $cookieLists 

					# Note dependency on view structure
					set cookieLists [list $root $firstChild]
					::ViewBrowser::OnWatch ::FileViewServer::gui $cookieLists
					::ViewBrowser::pathShow [namespace current]::gui \
						$cookieLists 
				} else {
					::ViewBrowser::OnWatch ::FileViewServer::gui $cookieLists
				}
			}
		}

		if {[regexp {FileAdd|FileDelete|DerivedObjectsUpdate} $eventName]} {

			if {[::ViewBrowser::viewBrowserIsVisible]} {

				# Look up the object handle for the project
				set prjName [lindex $eventData 0]
				set hProj [::Workspace::projectHandleGet $prjName]
				set root [lindex [objectsEnumerate "" -containers] 0]

				# Note dependency on view structure
				set cookieLists [list [list $root $prjName]]

				# Update everything under the project
				::ViewBrowser::OnWatch ::FileViewServer::gui $cookieLists

				# If this was a file add of a single file, show/select the file
				if {[regexp {FileAdd} $eventName]} {

					set fileList [lindex $eventData 1]
					if {[llength $fileList] == 1} {
						foreach file $fileList {
							lappend pathList [list $root $prjName $file]
						}
						if {[info exists pathList]} {
							::ViewBrowser::pathShow \
								::FileViewServer::gui $pathList
						}
					}
				}
			}
		}

		# A derived object has been removed from the file system
		if {[regexp {FileRemove} $eventName]} {
			if {[::ViewBrowser::viewBrowserIsVisible]} {
				set root [lindex [objectsEnumerate "" -containers] 0]
				set prjName [lindex $eventData 0]
				set hProj [::Workspace::projectHandleGet $prjName]

				# Update only the Derived Objects folder 
				set cookieLists [list [list $root $prjName Derived]]
				::ViewBrowser::OnWatch ::FileViewServer::gui $cookieLists
			}
		}

        if {[regexp {BuildSpecAdd|BuildSpecDelete|BuildSpecChange} \
				$eventName]} {
			if {[::ViewBrowser::viewBrowserIsVisible]} {
				set root [lindex [objectsEnumerate "" -containers] 0]
            	set prjName [lindex $eventData 0]

            	# Update only the Derived Objects folder
            	set cookieLists [list [list $root $prjName]]
            	::ViewBrowser::OnWatch ::FileViewServer::gui $cookieLists
        	}
		}

		if {[string match WorkspaceUpdate $eventName]} {
			if {[::ViewBrowser::viewBrowserIsVisible]} {
				::ViewBrowser::OnWatch ::FileViewServer::gui [list ""] 
			}
		}
	}

	#############################################################
	#
	# View Server Implementation 
	#
	#############################################################

	proc titleGet {} {
		return "Files"
	}

	proc objectsEnumerate {path flags} {

		if {[lsearch $flags "-leaves"] != -1} {
			set bLeaves 1
		} else {
			set bLeaves 0
		}
		if {[lsearch $flags "-containers"] != -1} {
			set bContainers 1
		} else {
			set bContainers 0
		}

		switch [llength $path] {
			0 -
			1 {
				if {$bContainers} {
					set retval [::Workspace::objectsEnumerate $path $flags]
				} else {
					set retval ""
				}
			}
			2 {
				set workspaceName [lindex $path 0]
				set prjName [lindex $path 1]
				set hProj [::Workspace::projectHandleGet $prjName]

				if {$bLeaves} {
					set fileList [::prjFileListGet $hProj]
					foreach file $fileList {
						lappend retval [list $workspaceName $prjName $file]
					}
				}
				if {$bContainers} {
					lappend retval \
						[list $workspaceName $prjName DerivedObjects]
					lappend retval \
						[list $workspaceName $prjName Dependencies]
				}
			}
			3 {
				set workspaceName [lindex $path 0]
				set prjName [lindex $path 1]
				set itemName [lindex $path 2]
				set hProj [::Workspace::projectHandleGet $prjName]
				if {$bLeaves} {
					switch $itemName {
						DerivedObjects {
							# Add the derived objects 
							set prjType [::prjTypeGet $hProj]
							set hSpec [::prjBuildCurrentGet $hProj]
							set buildRule \
								[::prjBuildRuleCurrentGet $hProj $hSpec]
							set derivedList \
								[${prjType}::derivedObjsGet $hProj $buildRule]
							if {[llength $derivedList]} {
								foreach derived [lsort $derivedList] {
									lappend retval \
										[list $workspaceName $prjName \
										DerivedObjects $derived]
								}
							}
						}
						Dependencies {
							# Make sure the dependencies have been updated
							set ns ::Workspace 
							set status ok
							if {[${ns}::obviouslyStaleDependenciesGet \
								$hProj] != ""} {
								set projObj [objectBind $prjName]
								if {![catch {::Dependencies::OnDependencies \
									$hProj $projObj 1} tmp]} {
									if {![string match $tmp "ok"]} {
										set status cancel
									}
								}
							}

							if {[string match $status "ok"]} {
								# Fetch the dep files
								set fileList [::prjFileListGet $hProj]
								set dependList ""
								foreach fileName $fileList { 
									set dependList [concat $dependList \
										[::prjFileInfoGet $hProj $fileName \
											dependencies]]
								}

								if {$dependList != ""} {

									# Eliminate dups
                                    removeDuplicatesFromList dependList 1

									# Sort and convert to objects 
									foreach depend [lsort $dependList] {
										lappend retval \
											[list $workspaceName $prjName \
											Dependencies $depend]
									}
                                }
							}
						}
						default {
						}
					}
				}
			}
		}
				
		if {![info exists retval]} {
			set retval ""
		}
		return $retval
	}

	proc objectBind {path} {
		set obj $::Workspace::_NULL_OBJECT 

		switch [llength $path] {
			0 {
				error "objectBind: can't bind to '$path'"	
			}
			1 -
			2 {
				# Bind to a project; punt to workspace
				set obj [::Workspace::objectBind $path]
			}
			3 {
				# It's a folder or source file
				set prjName [lindex $path 1]
				set hProj [::Workspace::projectHandleGet $prjName]
				if {$hProj != ""} {
					set name [lindex $path 2]
					switch $name {
						DerivedObjects {
							set obj [::Object::create \
								::FileViewServer::FolderObject \
								[list $hProj DerivedObjects]]
						}
						Dependencies {
							set obj [::Object::create \
								::FileViewServer::FolderObject \
								[list $hProj Dependencies]]
						}
						default {
							# Source file
							set obj [::Object::create \
								::FileViewServer::FileObject \
								[list $hProj $name]]
						}
					}
				} else {
					error "objectBind() path: no project handle!"
				}
			}
			4 {
				# It's a dependency or derived object file
				set prjName [lindex $path 1]
				set folderName [lindex $path 2]
				set fileName [lindex $path 3]
				set hProj [::Workspace::projectHandleGet $prjName]
				if {$hProj != ""} {
					switch $folderName { 
						DerivedObjects {
							set obj [::Object::create \
								::FileViewServer::DerivedObject \
								[list $hProj DerivedObjects $fileName]]
						}
						Dependencies {
							set obj [::Object::create \
								::FileViewServer::DependObject \
								[list $hProj Dependencies $fileName]]
						}
						default {
							error "::FileViewServer::objectBind: \
								bad path '$path'"
						}
					}
				}
			}
			default {
				error "::FileViewServer::objectBind() can't bind to '$path'"
			}
		}
		return $obj
	}

	# XXX - this shouldn't have to bind to the object, but no big deal
	proc displayNameGet {callerName path} {
		set displayName ""
		set obj [objectBind $path]
		set displayName [::Object::evaluate $obj DisplayName $callerName]
		return $displayName 
	}

	# XXX - this shouldn't have to bind to the object, but no big deal
	proc attributesGet {path flagList} {
		set retval 0
		set obj [objectBind $path]
		set type [::Object::evaluate $obj Type]

		# Only set the color attribute for the "dead project" object
		if {$type != "deadProjectObject"} {
			set idx [lsearch $flagList -color]
			if {$idx != -1} {
				set flagList [lreplace $flagList $idx $idx]
			}
		}

		set retval [::Object::evaluate $obj Attributes [list $flagList]]
		return $retval
	}

	proc bFolder {path} {
		set retval 0
		set obj [objectBind $path]
		set retval [::Object::evaluate $obj bFolder]
		return $retval
	}

	#############################################################
	#
	# Private helper routines
	#
	#############################################################

	# Determine if a file can be launched in the integrated editor
	proc bEditable {fileName} {
		set retval 0
		switch [file extension $fileName] {
			".o" -
			".a" -
			"" -
			".exe" -
			".obj" {}	
			default {
				if {[file exists $fileName]} {
					set retval 1
				}
			}
		}
		return $retval
	}

	# Determine if the file is of type object, and exists on
	# file system
	proc bDownloadableType {hProj fileName} {
		set retval 0
		switch [file extension $fileName] {
			".o" {
				set retval 1
			}
			default { 
				set prjType [::prjTypeGet $hProj]
				set hSpec [::prjBuildCurrentGet $hProj]
				set buildRule \
					[::prjBuildRuleCurrentGet $hProj $hSpec]
				set derivedList \
					[${prjType}::derivedObjsGet $hProj $buildRule]
				if {[lsearch $derivedList $fileName] != -1} {
					set retval 1
				}
			}
		}
		return $retval
	}

	#############################################################
	#
	# Define the server/view browser interface
	#
	#############################################################

	namespace eval gui {

		# Nothing happens right now
		proc Init {pane} {

		}

		#############################################################
		#
		# Menu Server Implementation 
		#
		#############################################################

		# Register server's menu items
		proc menuMatch {type server handle} {
			variable _staticFolderNames

			if {[string match $type Server]} {

				# Do these only for the file view server
				if {[string match $server ::FileViewServer::gui]} {

					# Server-specific menus	
					set objType [::Object::evaluate $handle Type]

					# Dead project and workspace objects don't support
					# file list operations.  The hackery here is required
					# in order to get the menu items to appear on projects
					# and all their children.  From a technical standpoint,
					# only the project object should support these operations 
					if {$objType != "deadProjectObject" && \
						$objType != "workspaceObject"} {
						::ViewBrowser::menuAdd "Add &Files..." \
							[list ::FileViewServer::gui::OnFileAdd \
							"" $handle] 

						# if there are more than one prj in wsp:
						set root \
							[lindex [::Workspace::objectsEnumerate \
								"" -containers] 0]
						set projectList \
							[lindex [::Workspace::objectsEnumerate \
								$root -containers] 0]
						if {[llength $projectList] > 1 } {
							set ns ::FileViewServer::gui::fileCopyDlg
							::ViewBrowser::menuAdd \
								"Add File&s from project..." \
								[list ${ns}::OnFilesFromProjectCopy "" $handle] 
						}
					}

				}
			} elseif {[string match $type Object]} {

				set objType [::Object::evaluate $handle Type]

				switch -exact $objType {
					sourceFileObject {

						# Source files
						set filePath [::Object::evaluate $handle Name]
						set fileName [file tail $filePath] 
						::ViewBrowser::menuAdd "Ope&n '$fileName'..." \
							[list ::FileViewServer::gui::OnFileOpen \
								"dblclk" $handle]
						::ViewBrowser::menuAdd "&Compile '$fileName'" \
							[list ::FileViewServer::gui::OnFileCompile \
								$handle]
						defaultImageDownloadMenuAdd $handle
						::ViewBrowser::menuAdd "Remo&ve '$fileName'" \
							[list ::FileViewServer::gui::OnFileDelete \
							delete $handle]
					}
					dependObject { 

						# Dependency file (non-binary)
						set filePath [::Object::evaluate $handle Name]
						set fileName [file tail $filePath] 
						::ViewBrowser::menuAdd "Ope&n '$fileName'..." \
							[list ::FileViewServer::gui::OnFileOpen \
								"dblclk" $handle]
						defaultImageDownloadMenuAdd $handle
					}
					derivedObject {

						# Binary, source, or text
						set filePath [::Object::evaluate $handle Name]
						set fileName [file tail $filePath] 
						set hProj [::Object::evaluate \
							[::Object::evaluate $handle Project] Handle]
						set hBuild [::prjBuildCurrentGet $hProj]
						set tc [::prjBuildTcGet $hProj $hBuild]
						set prjType [::prjTypeGet $hProj]

						if {[::FileViewServer::bEditable $filePath]} {
							::ViewBrowser::menuAdd "Ope&n '$fileName'..." \
								[list ::FileViewServer::gui::OnFileOpen \
									"dblclk" $handle]
						} else {
							::ViewBrowser::menuAdd "Ope&n '$fileName'..." \
								[list ::FileViewServer::gui::OnFileProperties \
									"dblclk" $handle]
						}

						if {[[namespace parent]::bDownloadableType \
							$hProj $filePath]} {
							set ns ::FileViewServer::gui

							# If this is a simulator image, set menu text
							# to 'Start', if it is a non-simulator image,
							# then don't add a menu item; otherwise,
							# set it to 'Download'
							set menuText ""
							if {[string match $prjType ::prj_vxWorks]} {
								set image [::Workspace::projectImageGet $hProj]
								set imageName [file tail $image]

								if {[isSimulatorTc $tc \
									[wtxHostType]]} {
									if {[string match $fileName $imageName]} {
										set menuText "&Start '$fileName'"
									} else {
										set menuText "Do&wnload '$fileName'"
									}
								} else {
									if {![string match $fileName $imageName]} {
										set menuText "Do&wnload '$fileName'"
									}
								}
							} else {
								set menuText "Do&wnload '$fileName'"
							}

							if {$menuText != ""} {
								if {[file exists $filePath]} {
									::ViewBrowser::menuAdd $menuText \
										[list ${ns}::OnObjectDownload \
										"" $handle]
								} else { 
									::ViewBrowser::menuAdd $menuText \
										[list ${ns}::OnObjectDownload \
										"" $handle] 1 
								}
							}
						}

						${ns}::AddUnloadMenuItem $fileName $handle

						if {[file exists $filePath]} {
							set ns ::FileViewServer::gui
							::ViewBrowser::menuAdd "&Delete '$fileName'" \
								[list ${ns}::OnFileRemove delete $handle]
						}
					}
					folderObject {
						set folderName [::Object::evaluate $handle Name]
						defaultImageDownloadMenuAdd $handle
						set ns ::FileViewServer
						set ns2 ::FileViewServer::gui
						if {[string match $folderName DerivedObjects]} {	
							::ViewBrowser::menuAdd \
								"Delete \
								'[set ${ns}::_staticFolderNames($folderName)]' \
								contents" \
								[list ${ns2}::OnFileRemove delete $handle]
						}
					}
					projectObject {
						# Projects
						set filePath [::Object::evaluate $handle Name]
						set projectName [file tail $filePath] 
						defaultImageDownloadMenuAdd $handle
						::ViewBrowser::menuAdd "Remo&ve '$projectName'..." \
							[list ::FileViewServer::gui::OnProjectDelete \
							delete $handle]
						::ViewBrowser::menuAdd "Save '$projectName' &As ..." \
							[list ::FileViewServer::gui::OnProjectSaveAs \
							delete $handle]
						::ViewBrowser::menuAdd "Rena&me '$projectName' ..." \
							[list ::FileViewServer::gui::OnProjectRename \
							$handle]

					}
					deadProjectObject {
						# Dead Projects
						set filePath [::Object::evaluate $handle Name]
						set projectName [file tail $filePath] 
						::ViewBrowser::menuAdd "Remo&ve '$projectName'..." \
							[list ::FileViewServer::gui::OnProjectDelete \
							delete $handle]
						::ViewBrowser::menuAdd "&Find '$projectName'..." \
							[list ::FileViewServer::gui::OnDeadProjectFind \
							$handle]
					}
					workspaceObject {
						# New project operation
						::ViewBrowser::menuAdd "&New Project..." \
							::ProjectFacility::guiNew

						# Add project to workspace operation
						::ViewBrowser::menuAdd "&Add Project..." \
							::ProjectFacility::staticProjectToWorkspaceAdd

						# Add the workspace backup menu item
						::ViewBrowser::menuAdd "&Backup..." \
							::WkspBackup::onBackup
					}
				}
			} elseif {[string match $type WorkspaceShared]} {
			}
		}

		proc canUnloadObject { strObjectname } {
			expr { [::SelectedTarget::targetGet] != ""}
		}

		# Add the menu item for download if the default image has been
		# built
		proc defaultImageDownloadMenuAdd {handle} {

			# Assume that the last element in the list of derived
			# objects is the "image"; add menu for download it if it exists
			set prjObj [::Object::evaluate $handle Project]
			set hProj [::Object::evaluate $prjObj Handle]
			set prjType [::prjTypeGet $hProj]
			set hBuild [::prjBuildCurrentGet $hProj]
			set tc [::prjBuildTcGet $hProj $hBuild]
			if {[::Workspace::isToolchainInstalled $tc]} {
				set image [::Workspace::projectImageGet $hProj]
				set imageName [file tail $image]
			} else {
				set image ""
				set imageName ""
			}
			set ns ::FileViewServer::gui
			
			# If simulator, simulator can be rebooted with new image,
			# or partially linked image can be downloaded
			# If not simulator, bootable images can't be downloaded
			if {[isSimulatorTc $tc [wtxHostType]]} {

				# Menu text should be 'Start' for images,
				# 'Download' for objects
				if {[string match $prjType "::prj_vxWorks"]} {
					set menuText "&Start '$imageName'"
				} else {
					set menuText "Do&wnload '$imageName'"
				}

				if {[file exists $image]} {

					set obj [::Object::create \
						::FileViewServer::DerivedObject \
						[list $hProj DerivedObjects $image]]

					# If image exists on the FS, enable menu;
					# otherwise, disable.
					::ViewBrowser::menuAdd $menuText \
						[list ${ns}::OnObjectDownload "dblclk" $obj]
					${ns}::AddUnloadMenuItem $imageName $obj
				} else {
					::ViewBrowser::menuAdd $menuText {} 1 
					${ns}::AddUnloadMenuItem $imageName "" 1
				}
			} elseif {![string match $prjType "::prj_vxWorks"]} {

				if {[file exists $image]} {
					# If the image exists, then create an object handle
					# to be bound into the call to OnObjectDownload()
					set obj [::Object::create \
						::FileViewServer::DerivedObject \
						[list $hProj DerivedObjects $image]]
					::ViewBrowser::menuAdd "Do&wnload '$imageName'" \
						[list ${ns}::OnObjectDownload "dblclk" $obj]
					${ns}::AddUnloadMenuItem $imageName $obj
				} else {
					::ViewBrowser::menuAdd "Do&wnload '$imageName'" {} 1 
					${ns}::AddUnloadMenuItem $imageName {} 1
				}
			}
		}
	
		proc AddUnloadMenuItem { strFilename handle {bDisable 0}} {
			if {!$bDisable} {
				if { [canUnloadObject $strFilename] } {
					::ViewBrowser::menuAdd "&Unload '$strFilename'" \
						[list [namespace current]::OnObjectUnload {} $handle] \
						$bDisable
				}
			} else {
				::ViewBrowser::menuAdd "&Unload '$strFilename'" {} $bDisable
			}
		}

		#############################################################
		#
		# Menu callbacks 
		#
		#############################################################

		proc OnFileAdd {event handle} {
			# Use last accessed directory as initial directory
       		# if none, use project directory
       		set cwd [pwd]
            set fileAddDirectory [appRegistryEntryRead -default "" \
                Workspace FileAddPath] 
			if {![string match ($fileAddDirectory ""]} {
                if {[catch {cd $fileAddDirectory}]} {
				    set fileAddDirectory [file dirname \
					    [::Object::evaluate [::Object::evaluate \
					    $handle Project] Name]]
                } else {
                }
			}
			set filter "Source Files(*.c;*.cpp;*.cxx;*.s)|*.c;"
			set filter [format "%s%s" $filter "*.cpp;*.cxx;*.s|Header Files"]
			set filter [format "%s%s" $filter " (*.h;*.inl)|*.h;*.inl|"]
			set filter [format "%s%s" $filter "All Files (*.*)|*.*||"]
			set projectName [file root [file tail [::Object::evaluate \
				[::Object::evaluate \
				$handle Project] Name]]]
			set fileNames [fileDialogCreate -filefilters $filter \
				-title "Add Source File to $projectName" \
                -okbuttontitle "Add" -multisel]
			cd $cwd
			::beginWaitCursor
			if {[llength $fileNames]} {
				set fileAddDirectory [file dirname [lindex $fileNames 0]]

				set projObj [::Object::evaluate $handle Project]
				set hProj [::Object::evaluate $projObj Handle]
				set projName [::prjInfoGet $hProj fileName]
				::filePathFix fileNames $hProj
				
				# Validate vis-a-vis bad chars
				catch {unset fileList}
				foreach file $fileNames {
					if {![::isFileNameOk $file]} {
						messageBox -ok "$file:\ncontains invalid characters.\
							Rename file before adding."
					} else {
						lappend fileList $file
					}
				}

				# Add the file(s) to the data layer
				if {[info exists fileList]} {
					if {[catch {::Workspace::fileAdd ::FileViewServer \
						$projName $fileList} error]} {
						messageBox -ok "$error"
					}
				}
			}

			# Save user's path choice
			appRegistryEntryWrite Workspace FileAddPath $fileAddDirectory
                
			::endWaitCursor
		}

		proc OnFileDelete {event fileObj} {
			::beginWaitCursor
			set fileName [::Object::evaluate $fileObj Name]
			set hProj [::Object::evaluate \
				[::Object::evaluate $fileObj Project] Handle]
			set projName [::prjInfoGet $hProj fileName]
			set status [messageBox -okcancel -questionicon \
				"Remove '[file tail $fileName]' from project\
				'[file tail $projName]'?"]
			if {[string match "ok" $status]} {
				::Workspace::fileDelete ::FileViewServer $projName $fileName
			}
			::endWaitCursor
		}

		proc OnFileRemove {event handle} {
			# Object can either be a file or the "Derived Objects" folder
			set type [::Object::evaluate $handle Type]
			set hProj [::Object::evaluate \
				[::Object::evaluate $handle Project] Handle]

			if {[string match $type folderObject]} {
				set prjType [::prjTypeGet $hProj]
				set fileNames [::prjFileListGet $hProj]
	
				# Add any special project-specific derived files
				set prjType [::prjTypeGet $hProj]
				set hSpec [::prjBuildCurrentGet $hProj]
				set buildRule [::prjBuildRuleCurrentGet $hProj $hSpec]
				set derivedList \
					[${prjType}::derivedObjsGet $hProj $buildRule]
				if {[llength $derivedList]} {
					set derivedList [lsort $derivedList]
				}
			} else {
				set derivedList [::Object::evaluate $handle Name]
			}

			set projName [::prjInfoGet $hProj fileName]
			::Workspace::fileRemove ::FileViewServer $projName $derivedList
		}

		proc OnFileCompile {fileObj} {
			set fileName [::Object::evaluate $fileObj Name]
			set projObj [::Object::evaluate $fileObj Project]
			set projName [::Object::evaluate $projObj Name]
			if {[catch {::Workspace::fileBuild \
				$projName $fileName} error]} {
				messageBox "Build failed: $error"
			}
		}

		proc OnFileOpen {event fileObj} {
			::beginWaitCursor
			set fileName [::Object::evaluate $fileObj Name]
			set hProj [::Object::evaluate \
				[::Object::evaluate $fileObj Project] Handle]
			set projName [::prjInfoGet $hProj fileName]
			::FileLaunch::fileOpen $fileName
			::endWaitCursor
		}

		proc OnFileProperties {event fileObj} {
			::Workspace::propSheetShow
		}

		proc OnProjectRename {projObj} {
			set prjFile [::Object::evaluate $projObj Name]
			ProjectFacility::guiRenameProject $prjFile
		}

		proc OnProjectDelete {event projObj} {
			set prjFile [::Object::evaluate $projObj Name]
			set projectName [file rootname [file tail $prjFile]]
			if {[string match [messageBox -okcancel -questionicon \
				"Remove project '$projectName'?\n($prjFile)"] ok]} {
				::Workspace::projectDelete $prjFile
			}
		}

		proc OnProjectSaveAs {event projObj} {
			set hOldProj [::Object::evaluate $projObj Handle]
			::ProjectFacility::guiSaveProjectAsFromHandle $hOldProj		
		}

		proc OnDeadProjectFind {projObj} {
			set oldPrjFile [::Object::evaluate $projObj Name]
			::filePathFix oldPrjFile
			set oldProjectName [file rootname [file tail $oldPrjFile]]
			set cwd [pwd]
			# Attempt to change to the directory where the project
			# file last was...
			catch { cd [file dirname $oldPrjFile] }
			set newPrjFile [join [fileDialogCreate -filefilters \
				"Project Files(*.wpj)|*.wpj||" \
				-title "Find Missing Project File: [file tail $oldPrjFile]" \
					-okbuttontitle "Ok"]]
			if {$newPrjFile != ""} {
				::filePathFix newPrjFile
				::Workspace::projectAdd $newPrjFile
				# Attempt to remove Makefile, since PRJ_FILE macro 
				# is now out of date. 
				# It will get regenerated on the next build
				if {$newPrjFile != $oldPrjFile} {
					set newProjDir [file dirname $newPrjFile]
					catch {file delete -force $newProjDir/Makefile}
				}
				# Remove the dead project
				::Workspace::projectDelete $oldPrjFile
				}
			cd $cwd
		}

		# translates { event handle } args into { hProj fileList }
		# args and calls operation with those args
		proc OnObjectOperation { event handle operation } {
			set hProj [::Object::evaluate \
				[::Object::evaluate $handle Project] Handle]
			set objectName [::Object::evaluate $handle Name]
			set fileList [list $objectName]

			eval $operation [list $hProj $fileList]
		}
		# The selected object is ok to download.  If project uses a simulator
		# TC: (1) If the object is a simulator image, boot it, killing the
		# previously running image if it exists; or (2) If the object is
		# simply a downloadable object, start a simulator if one is needed.
		#
		proc OnObjectDownload {event handle} {
			OnObjectOperation $event $handle projectFilesDownload
		}

		proc OnObjectUnload {event handle} {
			OnObjectOperation $event $handle projectFilesUnload
		}

		proc projectFilesUnload { hProj fileList} {
			if {[catch {
				set buildSpec [::prjBuildCurrentGet $hProj]
				set tc [::prjBuildTcGet $hProj $buildSpec]
				set cpu [${tc}::cpu]
				::SelectedTarget::unload $cpu [list $fileList]
				statusMessageSet -normal "[FileTails $fileList] successfully unloaded."
			} error]} {
				# handle the error case
				messageBox -ok -stopicon \
					"Error, could not unload '[FileTails $fileList]':\n$error"
			}
		}


		# FileList can either be a list of objects to download, or
		# a simulator image to boot.
		# If the current build is based on a simulator toolchain:
		# 	- 	If fileList corresponds to a bootable image, boot
		#		the image, halting the currently running simulator
		#		if necessary
		#	- 	If fileList contains one or more downloadable objects,
		#		download them, starting the simulator if necessary
		# The project handle passed in hProj is used to determine the
		# toolchain, whether this is a simulator image,	what the cpu
		# is, etc.
		proc projectFilesDownload {hProj fileList} {

			set simStartImage "" 
			set cpu -1 
			set hostType [wtxHostType]
			set status "ok" 

			if {$hProj != ""} {
				set defaultImage [::Workspace::projectImageGet $hProj]
				set buildSpec [::prjBuildCurrentGet $hProj]
				set tc [::prjBuildTcGet $hProj $buildSpec]
				set cpu [${tc}::cpu]

				if {[isSimulatorTc $tc $hostType]} {
					if {[string match [::prjTypeGet $hProj] ::prj_vxWorks]} {

						# This may be a reboot of the simulator
						if {[string match [file tail $defaultImage] \
							[file tail [lindex $fileList 0]]]} {

							# This is a reboot of the simulator
							set simStartImage [lindex $fileList 0] 
							set fileList [list ""] 
							set status [::SelectedTarget::simulatorStart \
								$simStartImage]

						} elseif {[IDESimInterface::simFirstActivePnoGet] == -1} {

							# We are simply downloading an object.  
                                                        # There is no simulator running, so tell user
							# simulator must be started
							set status [simStartWarn]

							if {[string match "yes" $status]} {
								set simStartImage \
									[::SelectedTarget::defaultSimPathGet \
									$hostType]
								set status [::SelectedTarget::simulatorStart \
									$simStartImage]
							}
						}
					} else {

						if {[IDESimInterface::simFirstActivePnoGet] == -1} {

							# We are simply downloading an object.  
                                                        # There is no simulator running, so tell user
							# simulator must be started
							set status [simStartWarn]
							if {[string match "yes" $status]} {
								set simStartImage \
									[::SelectedTarget::defaultSimPathGet \
									$hostType]
								set status [::SelectedTarget::simulatorStart \
									$simStartImage]
							}
						}
					}
				} else {
					set status "ok"
				}
			}

			if {[string match "ok" $status]} {
				set cmd "::SelectedTarget::download $cpu [list $fileList]"
				if {[catch $cmd error]} {
					messageBox -ok -stopicon \
						"Error downloading '$fileList':\n$error"
				}
			}
		}

		proc simStartWarn {} {
            set status [messageBox -yesno -questionicon \
            	"This project is built for a simulator\
				toolchain.\nNo simulator is currently running.\
				Start simulator?"]
			return $status
		}

	}; # End of namespace

	#############################################################
	#
	# Workspace extensions
	#
	#############################################################

	namespace eval ::Workspace {
	}

	#############################################################
	#
	# Object wrappers for files, projects, and folders w/in the
	# FileViewServer 
	#
	#############################################################

	# STRUCTURE -- {hProj name}
	namespace eval ::FileViewServer::FileObject {
		proc Type {args} { return sourceFileObject }
		proc bFolder {args} { return 0 }
		proc Name {args} {
			set args [lindex $args 0]
			set retval [lindex $args 1]
			return $retval
		}
		proc DisplayName {args} {
			set args [lindex $args 0]
			set name [lindex $args 1]
			return [file tail $name]
		} 
		proc Project {args} {
			set args [lindex $args 0]
			set hProj [lindex $args 0]
			set prjObj [::Object::create \
				::Workspace::ProjectObject $hProj]
			return $prjObj
		}
		proc Attributes {args} {
			set flags [lindex $args 1]
			set args [lindex $args 0]
			set hProj [lindex $args 0]
			set id [lindex $args 1]
			set nsp [namespace parent]
			set prjName [::Workspace::projectNameGet $hProj]

			foreach flag $flags {
				set flag [string range $flag 1 end]
				switch -exact $flag {
					icon {
						lappend retval {-icon IDB_FILE}
					}
					bold {
					}
					italic {
					}
					color {
					}
					default {
					}
				}
			}

			if {![info exists retval]} {
				set retval ""
			}
			return $retval 
		}
	}


	# STRUCTURE -- {hProj name}
	namespace eval ::FileViewServer::FolderObject {
		proc bFolder {args} { return 1 }
		proc Type {args} { return folderObject } 
		proc Name {args} {
			set args [lindex $args 0]
			set retval [lindex $args 1] 
			return $retval
		}
		proc DisplayName {args} {
			set args [lindex $args 0]
			set name [lindex $args 1]
			set displayName $::FileViewServer::_staticFolderNames($name)
			switch $name {
				DerivedObjects {
					set hProj [lindex $args 0]
					set build [::prjBuildCurrentGet $hProj] 
					set displayName [format "%s \[%s\]" \
						$displayName $build]
				}
				Dependencies {
				}
				default {
					error "::FileViewServer::FolderObject: '$name' not \
						known as a folder"
				}
			}
			return $displayName
		}
		proc Project {args} {
			set args [lindex $args 0]
			set hProj [lindex $args 0]
			set prjObj [::Object::create ::Workspace::ProjectObject $hProj]
			return $prjObj
		}
		proc Attributes {args} {
			set flags [lindex $args 1]
			set args [lindex $args 0]
			set hProj [lindex $args 0]
			set id [lindex $args 1]
			set nsp [namespace parent]
			set prjName [::Workspace::projectNameGet $hProj]

			foreach flag $flags {
				set flag [string range $flag 1 end]
				switch -exact $flag {
					icon {
						lappend retval {-icon IDB_FOLDER}
					}
					bold {
					}
					italic {
					}
					color {
					}
					default {
					}
				}
			}

			if {![info exists retval]} {
				set retval ""
			}
			return $retval 
		}
	}

	# STRUCTURE -- {hProj "Dependencies" name}
	namespace eval ::FileViewServer::DependObject {
		proc Type {args} { return dependObject }
		proc bFolder {args} { return 0 }
		proc Name {args} {
			set args [lindex $args 0]
			set retval [lindex $args 2] 
			return $retval
		}
		proc DisplayName {args} {
			set args [lindex $args 0]
			set name [lindex $args 2]
			set retval [file tail $name] 
			return $retval
		}
		proc Project {args} {
			set args [lindex $args 0]
			set hProj [lindex $args 0]
			set projObj [::Object::create \
				::Workspace::ProjectObject $hProj]
			return $projObj 
		}
		proc Attributes {args} {
			set flags [lindex $args 1]
			set args [lindex $args 0]
			set hProj [lindex $args 0]

			foreach flag $flags {
				set flag [string range $flag 1 end]
				switch -exact $flag {
					icon {
						lappend retval {-icon IDB_DEPEND}
					}
					bold {
					}
					italic {
					}
					color {
					}
					default {
					}
				}
			}

			if {![info exists retval]} {
				set retval ""
			}
			return $retval 
		}
	}

	# STRUCTURE -- {hProj "DerivedObjects" name}
	namespace eval ::FileViewServer::DerivedObject {
		proc Type {args} { return derivedObject }
		proc bFolder {args} { return 0 }
		proc Name {args} {
			set args [lindex $args 0]
			set retval [lindex $args 2] 
			return $retval
		}
		proc DisplayName {args} {
			set args [lindex $args 0]
			set name [lindex $args 2]
			set retval [file tail $name] 
			return $retval
		}
		proc Project {args} {
			set args [lindex $args 0]
			set hProj [lindex $args 0]
			set projObj [::Object::create \
				::Workspace::ProjectObject $hProj]
			return $projObj 
		}
		proc Attributes {args} {
			set flags [lindex $args 1]
			set args [lindex $args 0]
			set hProj [lindex $args 0]

			foreach flag $flags {
				set flag [string range $flag 1 end]
				switch -exact $flag {
					icon {
						lappend retval {-icon IDB_DERIVED}
					}
					bold {
					}
					italic {
					}
					color {
					}
					default {
					}
				}
			}

			if {![info exists retval]} {
				set retval ""
			}
			return $retval 
		}
	}

	#############################################################
	#
	# Register ourselves as an add-in.  This must occur last.
	#
	#############################################################

	::Workspace::addinRegister ::FileViewServer

	# Source the 'Add file from project' functionality
	source [wtxPath host resource tcl app-config Project]AddFile.tcl

}
