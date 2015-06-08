# Workspace.tcl - Workspace implementation file
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 04h,28may02,cjs  Fix spr 77715: moved routines for getting default bsp and tc
# 04g,04oct01,rbl  send status message if there are errors in Workspace save
# 04f,14apr99,cjs  Fix deletion of custom rule for file when file is removed
# 04e,19mar99,rbl  fixing projectDelete which would fail if prjName had wrong
#                  case on Windows
# 04d,16mar99,cjs  Fixing spr 25030: Hands are tied....
#				   Added isToolChainInstalled().
# 04c,15mar99,rbl  adding location argument to newWorkspaceFileNameGet. 
#                  Both newProjectDirGet and newWorkspaceFileNameGet now
#                  use location of currently open workspace if there is 
#                  a workspace open.
# 04b,11mar99,rbl  adding a newWorkspaceFileNameGet function
# 04a,10mar99,cjs  Protect currentProjectNameGet() from workspaceObject
# 03z,05mar99,cjs  Fixing problems related to increasing tree path length via
#                  addition of workspace object
# 03y,02mar99,cjs  Add workspace icon to all view servers
# 03x,02mar99,ren  Changing title bar back to Workspace
# 03w,25feb99,rbl  Changes to handle missing or read-only projects
# 03v,17feb99,cjs  Adding support for new project icons
# 03u,13jan99,jak  added support for macros for tools customize
# 03t,09dec98,cjs  added projectBspPathGet()
# 03s,03dec98,cjs  deleting source file should cleanup custom build rule
# 03r,16nov98,cjs  added sourcePathListGet(), currentProjectNameGet() 
# 03q,13nov98,cjs  fixed bug that allowed partial open of workspace 
# 03p,03nov98,cjs  added projectDirGet() 
# 03o,03nov98,ren  Changed titlebar from Workspace to "Project Workspace"
# 03n,20oct98,cjs  report error on non-unique file adds
# 03m,16oct98,cjs  shortened error for workspaceOpen()
# 03l,15oct98,cjs  added 'Workspace' to view browser title 
# 03k,02oct98,cjs  cleanup workspaceClose() logic 
# 03j,30sep98,cjs  removed hack to adjust pc simulator image name 
# 03i,28sep98,cjs  remove code to remove built image on fileAdd/Delete;
#                  added support for simulator start/kill on download and
#                  build
# 03h,23sep98,cjs  added isSimulatorTc(), delete project image on file add,
#                  delete; added projectTSSet/Get()
# 03g,21sep98,cjs  improving workspace save/saveas logic; added projectAdd;
#                  removed stack behavior of statusMessagePush/Pop
# 03f,10sep98,cjs  changed host-specific wizard routines to be host-specific 
# 03e,09sep98,cjs  use colors from colors option dialog for ProjectObject
# 03c,04sep98,cjs  color project object if error 
# 03b,19aug98,aam  Fix for Unix 
# 03a,19aug98,cjs  Workspace/View Browser now use event framework 
#                  like other add-ins do
# 02l,17aug98,cjs  fixed bug in defaultProjectDirGet()
# 02k,14aug98,cjs  made defaultProjectDirGet() more robust 
# 02j,13aug98,cjs  fixed bug in projectDelete() so that last project closed
#                  completely deletes _projectHandles. 
# 02i,10aug98,cjs  added defaultProjectDirGet(), defaultBasisProjectGet(),
#                  defaultBasisBspGet(), defaultBasisToolChainGet() 
# 02h,31jul98,jak  added call to editors - integrated & external 
# 02g,30jul98,cjs  buildSpecActiveSet now posts an event
# 02f,24jul98,cjs  added ProjectObject; now all servers can share it;
#                  datadocs no longer cached at this layer, closed
#                  when done
# 02e,15jul98,cjs  added uiEventProcess() to statusMessagePush/Pop() 
# 02d,08jul98,cjs  added atomic op componentAddDelete(), got rid
#                  of selectionComponentAdd/Delete()
# 02c,26jun98,cjs  part of fix of file download.
# 02b,24jun98,cjs  added buildSpecCopy() 
# 02a,23jun98,cjs  added and revised a whole bunch of component,
#                  build, and component set related APIs 
# 01h,23jun98,cjs  added fileDownload() 
# 01g,09jun98,cjs  added selectionComponentAdd/Delete() 
# 01f,05jun98,cjs  added currentCSetSizeGet() 
# 01e,08may98,cjs  added support for multiple component add/delete 
# 01d,07may98,cjs  added workspaceSave() 
# 01c,01may98,cjs  fixed fileAdd() 
# 01d,23apr98,cjs  add cdf doc loading/fetching code 
# 01c,21apr98,cjs  add logic to invoke build 
# 01b,31mar98,cjs  got rid of unnecessary format() statements.
# 01a,27mar98,cjs  written.
#
# DESCRIPTION
# This file implements shared functionality for the config tool.
# It also implements the graphical representation of the project
# workspace, the View Browser, which manages a tabbed pane on
# behalf of each of the View Servers that register
#

source "[wtxPath host resource tcl app-config Project]WorkspaceDoc.tcl"

#############################################################
#
# Workspace declarations and definitions follow
#
#############################################################

catch {namespace delete ::Workspace}

namespace eval ::Workspace {

	# The one and only NULL object instance
	variable _NULL_OBJECT {::Null {}}

	variable _addins
	variable _servers

	variable _registrees
	variable _eventQ empty

	# The cached current selection handle
	variable _currentSelection $_NULL_OBJECT

	# Array of project handles corresponding to open projects
	# in the current workspace
	variable _projectHandles

	# Status message stack
	variable _statusMessageStack

	######################################################################
	#
	# The Workspace core API
	#
	######################################################################

	proc addinRegister {namespace} {
		set ifaceList {Init OnEvent}
		if {[::ifaceValidate $namespace $ifaceList]} {
			lappend ::Workspace::_addins $namespace
		} else {
			error "Addin '$namespace' must support ($ifaceList)"
		}
	}

	proc serverRegister {namespace} {
		set ifaceList {Init}
		if {[::ifaceValidate $namespace $ifaceList]} {
			lappend ::Workspace::_servers $namespace
		} else {
			error "View Server '$namespace' must support ($ifaceList)"
		}
	}

	proc eventRegister {namespace regexp} {
		set ifaceList {OnEvent}
		if {[::ifaceValidate $namespace $ifaceList]} {
			set ::Workspace::_registrees($namespace) $regexp
		} else {
			error "eventRegistree '$namespace' must support ($ifaceList)"
		}
	}

	proc eventUnregister {namespace} {
		catch {unset ::Workspace::_registrees($namespace)}
	}

    proc eventSend {cookie event} {
		enqueueEvent $cookie $event

		# rename eventSend so that any recursive calls to 
		# eventSend by event handlers only enqueue the events
		rename ::Workspace::eventSend ::Workspace::oldEventSend
		rename ::Workspace::enqueueEvent ::Workspace::eventSend

		# catch just in case something goes wrong we don't permanantly
		# screwup renamed functions
		catch {
		    # we can't use a foreach here because the contents of the
		    # list may be modified within the while statement
		    while { [llength $::Workspace::_eventQ] > 0 } {
				set nextEvent [lindex $::Workspace::_eventQ 0]
				set ::Workspace::_eventQ [lrange $::Workspace::_eventQ 1 end]
				dispatchEvent [lindex $nextEvent 0] [lindex $nextEvent 1]
		    }
		}

		# now restore the old function names
		rename ::Workspace::eventSend ::Workspace::enqueueEvent
		rename ::Workspace::oldEventSend ::Workspace::eventSend
    }

    proc enqueueEvent { cookie event } {
		lappend ::Workspace::_eventQ [list $cookie $event]
    }


	proc dispatchEvent {cookie event} {
		foreach addin [array names ::Workspace::_registrees] {
			if {[regexp $::Workspace::_registrees($addin) \
				[lindex $event 0]]} {
			        catch { eval [list ${addin}::OnEvent $cookie $event] }
			}
		}
	}

	proc addinsGet {} {
		return $::Workspace::_addins
	}

	proc serversGet {} {
		if {[info exists ::Workspace::_servers]} {
			return $::Workspace::_servers
		} else {
			return "" 
		}
	}

	proc serversConnect {} {
		set paneName ""
		foreach server [::Workspace::serversGet] {
			set paneName [eval [format "%s::titleGet" $server]]

			if {![info exists ::ViewBrowser::_connections($paneName)]} {
				set paneName [eval [format "%s::titleGet" $server]]
				set ns [format "%s::gui" $server]
				set ::ViewBrowser::_connections($paneName) $ns
			}
		}
	}

	proc serversDisconnect {} {
		catch {unset ::ViewBrowser::_connections}
	}

	proc statusMessagePush {message} {
		if {[catch {statusMessageSet $message}]} {
			puts "** $message"
		} else {
			uiEventProcess -allowpaintonly
		}	
	}

	proc statusMessagePop {} {
	}

	
	######################################################################
	#
	# The following routines manage the current selection,
	# an object handle
	#
	######################################################################

	proc selectionSet {handle} {
		if {$handle != ""} {
			set ::Workspace::_currentSelection $handle
		} else {
			error "selectionSet: can't set selection to '$handle'"
		}
		::Workspace::eventSend ::Workspace [list SelectionChange $handle]
	}

	proc selectionGet {} {
		return $::Workspace::_currentSelection
	}

	#############################################################
	#
	# Add-in implementation for the workspace
	#
	#############################################################

	proc OnEvent {cookie eventInfo} {
		set eventData [lindex $eventInfo 1]
		set eventName [lindex $eventInfo 0]

		switch $eventName {
			WorkspaceOpen {
				::ViewBrowser::viewBrowserCreate

				# Tell the world that projects have been added
				set prjNames $eventData
				::Workspace::eventSend ::Workspace \
					[list ProjectAdd $prjNames]

				::ViewBrowser::viewBrowserInit
				::ViewBrowser::viewBrowserTitleSet \
					"Workspace: [file rootname [file tail \
					[openWorkspaceGet]]]"
			}
			WorkspaceClose {
				# Broadcast a ProjectDelete event
				set projNames [::Workspace::objectsEnumerate "" -containers]
				::Workspace::eventSend ::Workspace \
					[list ProjectDelete $projNames]

				# Set the title of the view browser to reflect
				# the fact that the workspace has closed 
				if {[::ViewBrowser::viewBrowserIsVisible]} {
					::ViewBrowser::viewBrowserTitleSet \
						"(No open workspace)" 
				}
			}
			WorkspaceUpdate {
				if {[::ViewBrowser::viewBrowserIsVisible]} {
					::ViewBrowser::viewBrowserTitleSet \
						"Workspace: [file rootname [file tail \
						[openWorkspaceGet]]]"
				}
			}
		}
	}	

	#############################################################
	#
	# Workspace extensions added by the workspace itself
	#
	#############################################################

	#############################################################
	#
	# Open new workspace if no workspace is open
	# Create the new workspace if necessary, save it, and open it
	#
	proc createOrOpenWorkspace {workspaceFile} {
		# If a workspace is open, insert the project there.
		# Otherwise, create a new workspace, save it, and open it
		if {[openWorkspaceGet] == ""} {
			if {![file exists $workspaceFile]} {
				::WorkspaceDoc::workspaceCreate $workspaceFile
				::WorkspaceDoc::workspaceSave "" 
			} else {
				::WorkspaceDoc::workspaceOpen $workspaceFile
			}
			workspaceOpen $workspaceFile
		}
	}

	# Create a workspace.  Closes the current one first, and leaves
	# the workspace open for the addition of projects.
	proc workspaceCreate {workspaceFile} {

		# Close any open workspace
		if {$workspaceFile != ""} {
			workspaceClose
		}

		# Create and open a workspace file
		if {![file exists $workspaceFile]} {
			::WorkspaceDoc::workspaceCreate $workspaceFile
			::WorkspaceDoc::workspaceSave "" 
			workspaceOpen $workspaceFile
		} else {
			error "Workspace '$workspaceFile' already exists!"
		}
	}

	# Returns 1 if successful, 0 if not
	proc workspaceOpen {fileName} {
		::filePathFix fileName

		set retval 1
		# Attempt to open the workspace file.  If it fails, it will 
		# restore the old workspace (if there was one)
		if {[catch {::WorkspaceDoc::workspaceOpen $fileName} error]} {
			error $error
		} else {
			# Close any currently open projects
			internalProjectsClose

			# Open each project and save its handle
			foreach prjFile [::WorkspaceDoc::projectListGet] { 
				if {[catch {::prjOpen $prjFile} handle]} {
					set handle [::prjOpenDead $prjFile]
				}
				lappend prjNames $prjFile
				set ::Workspace::_projectHandles($prjFile) $handle
			}

			# Send an event telling the add-in portion of the workspace 
			::Workspace::eventSend ::Workspace \
				[list WorkspaceOpen $fileName]
		}
		return $retval
	}

	proc workspaceClose {} {
		# Close the workspace document
		::WorkspaceDoc::workspaceClose
			
		# Close the projects
		internalProjectsClose
	}

	# A private routine to close the project files
	proc internalProjectsClose {} {
		if {[info exists ::Workspace::_projectHandles]} {

			# Set the selected object to NULL
			selectionSet $::Workspace::_NULL_OBJECT

			# Close all the projects
			foreach prjName [array names ::Workspace::_projectHandles] {
				set hProj $::Workspace::_projectHandles($prjName)
				::prjClose $hProj DONT_SAVE
				lappend hProjs $hProj 
			}

			# Close the data docs
			foreach hProj $hProjs {
				cxrDocDelete $hProj
				mxrDocDelete $hProj
			}
			catch {unset ::Workspace::_projectHandles}
			# Send an event telling the world about workspace closing 
			::Workspace::eventSend ::Workspace WorkspaceClose
		}
	}

	########################################################################
	#
	# workspaceSave implements Save and SaveAs functionality, depending 
	# on whether the fileName parameter is set.
	#
	# SaveAs writes the workspace document but leaves
	# the projects dirty; save saves projects and workspace
	#
	# The first parameter is the workspace fileName. This may be "".
	# The second parameter, prjSaveFunction defaults to the non-gui function ::prjSaveAs.
	# $prjSaveFunction must return the name of the saved project (possibly a new name). 
	# If $prjSaveFunction returns "" the save operation is cancelled. 
	#
	# You can hook in a gui function here to cope with for instance read-only 
	# project or workspace files
	#
	# RETURNS: The new workspace filename, or "" if the operation has been cancelled
	#
	proc workspaceSave {{fileName ""} {prjSaveFunction "::prjSaveAs"}} {
		# Save the underlying projects
		set errors ""
		if {$fileName == ""} {
			foreach prjName [array names ::Workspace::_projectHandles] {
				set hProj [projectHandleGet $prjName]
				set showProgress 0
				if {[::prjInfoGet $hProj changed] != ""} {
					set showProgress 1
				}
				if {$showProgress} {
					statusMessagePush "Saving project $prjName..."
				}
				if [catch {$prjSaveFunction $hProj} currentProjectSaveError] {
					set errors "$errors\n$prjSaveFunction $hProj failed:\n$currentProjectSaveError"
				# If prjSaveFunction returns "" user has hit "cancel"
				} elseif {$currentProjectSaveError == ""} {
					::Workspace::statusMessagePush \
						"Saving workspace '[openWorkspaceGet]'...cancelled"
					return ""
				}

				if {$showProgress} {
					statusMessagePush "Saving project $prjName...done"
				}
			}
		}

		set showProgress 0
		if {[workspaceDirtyGet]} {
			set showProgress 1
			set currentFileName [openWorkspaceGet]

		}

		if {$fileName == ""} {
			if {$showProgress} {
				::Workspace::statusMessagePush \
					"Saving workspace '$currentFileName'"
			}
		} else {
			::Workspace::statusMessagePush \
				"Saving workspace as '$fileName'"
		}

		# Protect fileName argument with quotes since it may be blank
		if {![catch {::WorkspaceDoc::workspaceSave "$fileName" "$prjSaveFunction"} newWorkspaceFileName]} {

			# Blank return from ::WorkspaceDoc::workspaceSave means operation was cancelled
			if {$newWorkspaceFileName == ""} {
				::Workspace::statusMessagePush \
					"Saving workspace '[openWorkspaceGet]'...cancelled"
				return ""
			}

			::Workspace::eventSend ::Workspace [list WorkspaceUpdate $fileName]
			if {$fileName == ""} {
				if {$showProgress} {
					::Workspace::statusMessagePush \
						"Saving workspace '$currentFileName'...done"
				}
			} else {
				::Workspace::statusMessagePush \
					"Saving workspace as '$fileName'...done"
			}
		} else {
			::Workspace::statusMessagePush \
				"Saving workspace as '$fileName'...FAILED"
			set errors "$errors\n$newWorkspaceFileName"
		}

		# Fire this error only after we have saved all the projects we can and the workspace document
		if {$errors != ""} {
			::Workspace::statusMessagePush \
				"Saving workspace completed with ERRORS"
			error $errors
		} else {
			return $newWorkspaceFileName
		}
	}

	proc workspaceDirtyGet {} {
		set bDirty 0

		if {[::WorkspaceDoc::workspaceDirtyGet]} {
			set bDirty 1
		} else {
			foreach prjName [array names ::Workspace::_projectHandles] {
				set hProj $::Workspace::_projectHandles($prjName) 
				if {[::prjInfoGet $hProj changed] != ""} {
					set bDirty 1
					break
				}
			}
		}
		return $bDirty
	}

	proc buildSpecsForProject {prjName} {
		set hProj [projectHandleGet $prjName]
		return [::prjBuildListGet $hProj]
	}

	proc componentSetsForProject {prjName} {
		set hProj [projectHandleGet $prjName]
		return [::wccListGet $hProj]
	}

	proc componentsForComponentSet {prjName wcc} {
		set hProj [projectHandleGet $prjName]
		return [::wccComponentListGet $hProj $wcc]
	}

	#######################################################################
	#
	# projectHandleGet {prjName} returns the handle corresponding to the 
	# full path prjName. prjName should have been "fixed" using ::filePathFix
	# before calling projectHandleGet.
	#
	# RETURNS: the the handle corresponding to the full path prjName, or 
	#          "" if there is no match
	#
	# EXCEPTIONS: None
	#
	proc projectHandleGet {prjName} {
		#
		# Try quick look up
		#
		if {[info exists ::Workspace::_projectHandles($prjName)]} {
			return $::Workspace::_projectHandles($prjName)
		#
		# for windows the above may have worked, but 
		# we must do a case-insensitive search if it failed
		#
		} elseif {[string match [wtxHostType] "x86-win32"]} {
			set lowerCasePrjName [string tolower $prjName]
			foreach openProjectName [array names ::Workspace::_projectHandles] {
				if {[string compare $lowerCasePrjName [string tolower $openProjectName]] == 0} {
					return $::Workspace::_projectHandles($openProjectName)
				}
			}
			return ""
		} else {
			return ""
		}
	}

	proc projectNameGet {hProj} {
		return [::prjInfoGet $hProj fileName]
	}

	proc projectListGet {} {
		return [::WorkspaceDoc::projectListGet]
	}

	proc openWorkspaceGet {} {
		return [::WorkspaceDoc::openWorkspaceGet]
	}

	proc workspaceHandleGet {} {
		return [::WorkspaceDoc::workspaceHandleGet]
	}

	proc currentProjectNameGet {} {
		set handle [selectionGet]
		if {![string match [::Object::evaluate $handle Type] nullObject] && \
			![string match [::Object::evaluate $handle Type] workspaceObject]} {
			set projectName [::Object::evaluate \
				[::Object::evaluate $handle Project] Name]
		} else {
			set projectName ""
		}
		return $projectName
	}

	proc projectRefresh {prjName} {
		projectDelete $prjName
		projectAdd $prjName			
	}

	proc projectAdd {prjName} {
		variable _projectHandles
		if {[catch {::prjOpen $prjName} hProj]} {
			error "projectAdd($prjName) failed: $hProj"
		} else {
			::WorkspaceDoc::projectAdd $prjName
			set ::Workspace::_projectHandles($prjName) $hProj
			::Workspace::eventSend ::Workspace [list ProjectAdd $prjName]
		}
	}

	proc projectAddOrRefresh {prjName} {
		# Check to see if project already open
		set hProj [projectHandleGet $prjName]
		if {$hProj != ""} {
			projectRefresh $prjName
		} else {
			projectAdd $prjName
		}
	}

	proc projectDelete {prjName} {
		variable _projectHandles
		set hProj [projectHandleGet $prjName]

		# fix prjName - may have different case for instance on Windows
		set prjName [projectNameGet $hProj]

		# Delete the project name from the workspace document 
		::WorkspaceDoc::projectDelete $prjName

		# Close the project from the workspace by removing it's entry
		# from _projectHandles; also close it from the DB
		::prjClose $hProj DONT_SAVE

		# Clean up the list of open projects
		unset ::Workspace::_projectHandles($prjName)
		if {[llength [array names ::Workspace::_projectHandles]] == 0} {
			catch {unset ::Workspace::_projectHandles}
		}

		::Workspace::eventSend ::Workspace [list ProjectDelete $prjName]
	}

	proc projectBuild {prjName buildTargets} {
		set hProj [::Workspace::projectHandleGet $prjName]
    	set prjType [prjTypeGet $hProj]

    	set buildInfo [::${prjType}::buildCmdGet $hProj $buildTargets]

		if {[catch {buildLaunch "Build $prjName $buildTargets" \
			[lindex $buildInfo 0] [lindex $buildInfo 1]} error]} {
			error $error
		}
	}
	
	proc fileBuild {prjName fileName} {
		set hProj [::Workspace::projectHandleGet $prjName] 
		set objects [::prjFileInfoGet $hProj $fileName objects]
    	set prjType [prjTypeGet $hProj]
    	set buildInfo [::${prjType}::buildCmdGet $hProj $objects]

		if {[catch {buildLaunch "Build $prjName $fileName" \
			[lindex $buildInfo 0] [lindex $buildInfo 1]} error]} {
			error $error
		}
	}	

	proc fileAdd {cookie prjName fileNameList} {
		set prjHandle [::Workspace::projectHandleGet $prjName]

		# Check to see that the file(s) aren't already 
		# in the project
		set fileList [::prjFileListGet $prjHandle]
		foreach file $fileList {
			set idx [lsearch $fileNameList $file]
			if {$idx != -1} {
				error "The following file is already in the project:\n \
					'$file'.  Operation aborted."
			}
		}
		foreach fileName $fileNameList {
			::prjFileAdd $prjHandle $fileName
		}

		# Notify the workspace framework
		::Workspace::eventSend $cookie [list FileAdd \
			[list $prjName $fileNameList]]
	}

	proc fileDelete {cookie prjName fileNameList} {
		set hProj [::Workspace::projectHandleGet $prjName]
		foreach fileName $fileNameList {
			# Remove the custom rule, if it exists
			customRuleDelete $hProj $fileName

			::prjFileRemove $hProj $fileName
		}

		# Notify the workspace framework
		::Workspace::eventSend $cookie [list FileDelete \
			[list $prjName $fileNameList]]
	}

	# Remove the derived file from the file system
	proc fileRemove {cookie prjName fileNameList} {
		set prjHandle [::Workspace::projectHandleGet $prjName]
		foreach fileName $fileNameList {
			catch {file delete $fileName}
		}

		# Notify the workspace framework
		::Workspace::eventSend $cookie [list FileRemove \
			[list $prjName $fileNameList]]
	}

	# Get the list of paths for all the source files
	proc sourcePathListGet {projectName} {
		if {$projectName != ""} {
			set hProj [projectHandleGet $projectName]
			foreach file [::prjFileListGet $hProj] {

				# Use an array to hash for uniqueness
				set pathList([file dirname $file]) ""
			}
		}
		return [array names pathList]
	}

	# Return the derived object of the currently selected source file,
	# or "" if none is selected
	proc fileDerivedObjectGet { } {
		set obj [::Workspace::selectionGet]
        # Must have valid current selection to make this work
		if {[string match [::Object::evaluate $obj Type] sourceFileObject]} {
        	set sourceFile [::Object::evaluate $obj Name]
        	set hProj [::Object::evaluate \
						[::Object::evaluate $obj Project] Handle]
        	set derivedObj [::prjFileInfoGet $hProj $sourceFile objects]
		} else {
        	set derivedObj ""
		}
		return $derivedObj
	}

	# Change the current component set -- obsolete
	proc componentSetActiveSet {cookie prjName cSetName {noevent 0}} {
		set hProj [projectHandleGet $prjName]
		::wccCurrentSet $hProj $cSetName

		# Notify the workspace framework
		if {!$noevent} {
			::Workspace::eventSend $cookie [list ComponentSetChange \
				[list $prjName $cSetName]]
		}
	}

	# Add a new, empty component set 
	proc componentSetAdd {cookie prjName cSetName componentList} {
		set hProj [projectHandleGet $prjName]
		::wccCreate $hProj $cSetName

		# Add the components
		if {$componentList != ""} {
			::wccComponentAdd $hProj $cSetName $componentList
		}

		# Notify the workspace framework
		::Workspace::eventSend $cookie [list ComponentSetAdd \
			[list $prjName $cSetName]]
	}

	# Delete a component set from a project
	proc componentSetDelete {cookie prjName cSetName} {
		set hProj [projectHandleGet $prjName]
		::wccDelete $hProj $cSetName

		# Notify the workspace framework
		::Workspace::eventSend $cookie [list ComponentSetDelete \
			[list $prjName $cSetName]]
	}

	# Get the size of the current component set for the given project
	proc currentCSetSizeGet {prjName} {
		set hProj [::Workspace::projectHandleGet $prjName]
		set wcc [::wccCurrentGet $hProj]
		set cDoc [cxrDocGet $prjName]
		set cSet [$cDoc setCreate]
		$cSet = [::wccComponentListGet $hProj $wcc]
		set mDoc [::Workspace::mxrDocGet $prjName]
		set sizeData [::cxrSizeGet $cSet $mDoc]
		$cSet delete
		return $sizeData
	}

	# Add component(s) to a component set in a project
	proc componentAdd {cookie prjName cSetName componentNameList} {
		set hProj [projectHandleGet $prjName]
		foreach component $componentNameList {
			::wccComponentAdd $hProj $cSetName $component
		}

		# Notify the workspace framework
		::Workspace::eventSend $cookie [list ComponentAdd \
			[list $prjName $cSetName $componentNameList]]
	}

	# Remove a component from a component set in a project
	proc componentDelete {cookie prjName cSetName \
		componentNameList} {
		set hProj [projectHandleGet $prjName]
		foreach component $componentNameList {
			::wccComponentRemove $hProj $cSetName $component
		}

		# Notify the workspace framework
		::Workspace::eventSend $cookie [list ComponentDelete \
			[list $prjName $cSetName $componentNameList]]
	}

	proc componentAddDelete {cookie prjName cSetName addComps deleteComps} {
		set hProj [projectHandleGet $prjName]
		foreach component $addComps {
			::wccComponentAdd $hProj $cSetName $component
		}
		foreach component $deleteComps {
			::wccComponentRemove $hProj $cSetName $component
		}

		# Notify the workspace framework
		::Workspace::eventSend $cookie [list ComponentAddDelete \
			[list $prjName $cSetName [list $addComps $deleteComps]]]
	}

	proc buildSpecAdd {cookie prjName buildSpecName tc} {
		set hProj [projectHandleGet $prjName]
		::prjBuildCreate $hProj $buildSpecName $tc
		::Workspace::eventSend $cookie [list BuildSpecAdd \
			[list $prjName $buildSpecName]]
	}

	proc buildSpecCopy {cookie prjName buildSpecFrom buildSpecTo} {
		set hProj [projectHandleGet $prjName]
		::prjBuildCopy $hProj $buildSpecFrom $buildSpecTo
		::Workspace::eventSend $cookie [list BuildSpecAdd \
			[list $prjName $buildSpecTo]]
	}

	proc buildSpecActiveSet {cookie prjName buildSpecName} {
		set hProj [projectHandleGet $prjName]
		::prjBuildCurrentSet $hProj $buildSpecName
		::Workspace::eventSend $cookie [list BuildSpecChange \
			[list $prjName $buildSpecName]]
	}

	proc buildSpecDelete {cookie prjName buildSpecName} {
		set hProj [projectHandleGet $prjName]
		::prjBuildDelete $hProj $buildSpecName
		::Workspace::eventSend $cookie [list BuildSpecDelete \
			[list $prjName $buildSpecName]]
	}

	# Returns 1 if toolchain is installed, 0 if not
	proc isToolchainInstalled {toolchain} {
		regsub {(:*)tc_} $toolchain {} tc 
		
		if {![catch {namespace parent $toolchain}]} {
			statusMessagePush "Wind River Systems"
			return 1
		} else {
			statusMessagePush "Warning: Invalid toolchain '$tc'!"
			return 0
		}
	}

	# Routine to access the component library for a particular
	# project
	proc cxrDocGet {prjName} {
		set hProj [projectHandleGet $prjName]
		return [::cxrDocCreate $hProj]
	}

	# Routine to access the module xref doc for a particular
	# project
	proc mxrDocGet {prjName} {
		set hProj [projectHandleGet $prjName]
		return [::mxrDocCreate $hProj]
	}

	###############################################################
	#
	# Returns a unique directory into which to create a new project,  
	# in the same directory as the currently open workspace.
	# If no workspace is open, returns
	# a unique workspace file name in the [::prjDirDefault] directory
	#	
	proc newProjectDirGet {} {
		set projectCounter 0
		set openWorkspaceFile [openWorkspaceGet]
		if {$openWorkspaceFile != ""} {
			set location [file dirname $openWorkspaceFile]
		} else {
			set location [::prjDirDefault]
		}
		set existing [glob -nocomplain $location\/Project*]
		if {$existing != ""} {
			foreach proj $existing {
				if {[scan [file tail $proj] "Project%d" idx] != 0} {
					if {$idx >= $projectCounter} {
						set projectCounter [expr $idx + 1]
					}
				}
			}
		}
		set dirName [format "Project%d" $projectCounter]
		set retval [file join $location $dirName]
		incr projectCounter
		return $retval
	}
	
	###############################################################
	#
	# Returns a unique workspace file name in the directory 
	# specified in location. If no directory specified, returns 
	# a unique workspace file name in the same directory as the 
	# currently open workspace. If no workspace is open, returns
	# a unique workspace file name in the [::prjDirDefault] directory
	# 
	proc newWorkspaceFileNameGet {{location ""}} {
		set workspaceCounter 0
		if {$location == ""} {
			set openWorkspaceFile [openWorkspaceGet]
			if {$openWorkspaceFile != ""} {
				set location [file dirname $openWorkspaceFile]
			} else {
				set location [::prjDirDefault]
			}
		}
		set existing [glob -nocomplain $location\/Workspace*]
		if {$existing != ""} {
			foreach workspace $existing {
				if {[scan [file tail $workspace] "Workspace%d" idx] != 0} {
					if {$idx >= $workspaceCounter} {
						set workspaceCounter [expr $idx + 1]
					}
				}
			}
		}
		set fileName [format "Workspace%d.wsp" $workspaceCounter]
		set retval [file join $location $fileName]
		incr workspaceCounter
		return $retval
	}


    proc defaultBasisProjectGet {projectType hostType} {
        
        switch -glob $hostType {
            sun4-sunos* {
                set retval [file join [defaultProjDirGet] \
                    sunos_gnu sunos_gnu.wpj]
            }
            sun4-solaris* {
                set retval [file join [defaultProjDirGet] \
		            solaris_gnu solaris_gnu.wpj]
	        }
	        parisc-hpux* {
	            set retval [file join [defaultProjDirGet] \
		            hpux_gnu hpux_gnu.wpj]
	        }
	        x86-win32 -
	        default {
	            set retval [file join [defaultProjDirGet] \
	                simpc_gnu simpc_gnu.wpj]
	        }
        }
        set retval [appRegistryEntryRead -default \
            $retval Workspace DefaultProject${projectType}]
        return $retval
    }

    proc defaultBasisProjectSet {projectType projectFile} {
        appRegistryEntryWrite Workspace DefaultProject${projectType} \
		    $projectFile
    }
    

	proc defaultProjDirGet {} {
            return [wtxPath target proj] 
	}

	# Set the current target server associated with a project
	proc projectTSSet {target} {
		set handle [selectionGet]
		if {![string match [::Object::evaluate $handle Type] nullObject] && \
		    ![string match [::Object::evaluate $handle Type] workspaceObject]} {
			set hProj [::Object::evaluate \
				[::Object::evaluate $handle Project] Handle]
			::prjInfoSet $hProj currentTarget $target
		}
	}

	# Return the current target server associated with the current
	# project, if it is available 
	proc projectTSGet {} {
		set retval ""
		set handle [selectionGet]
		if {![string match [::Object::evaluate $handle Type] \
			nullObject] && \
		    ![string match [::Object::evaluate $handle Type] \
			workspaceObject]} {
			set hProj [::Object::evaluate \
				[::Object::evaluate $handle Project] Handle]
			if {[catch {::prjInfoGet $hProj currentTarget} retval]} { 
				set retval "" 
			}
		}
		return $retval 
	}

	# Fetch the project directory, or "" if there is no current project
	proc projectDirGet {} {
		set dir ""
		set handle [selectionGet]
		if {![string match [::Object::evaluate $handle Type] \
			nullObject] && \
		    ![string match [::Object::evaluate $handle Type] \
			workspaceObject]} {
			set hProj [::Object::evaluate \
				[::Object::evaluate $handle Project] Handle]
			set dir [file dirname [::prjInfoGet $hProj fileName]]
		}
		return $dir
	}

	# Get the fully qualified image name for a project 
	proc projectImageGet {hProj} {
		set hSpec [::prjBuildCurrentGet $hProj]
		set tc [::prjBuildTcGet $hProj $hSpec]
		set prjType [::prjTypeGet $hProj]
		set rule [::prjBuildRuleCurrentGet $hProj $hSpec]

		set derivedObjs [${prjType}::derivedObjsGet $hProj $rule]
		set image [lindex $derivedObjs \
			[expr [llength $derivedObjs] - 1]]
		return $image
	}

	# Fetch the project's current build directory, 
	# "" if there is no current project
	proc projectBuildDirGet {args} {
		if {[llength $args] > 0} {
			set hProj [lindex $args 0]
		} else {
			set handle [selectionGet]
			if {![string match [::Object::evaluate $handle Type] \
					nullObject] && \
				![string match [::Object::evaluate $handle Type] \
					workspaceObject]} {
				set hProj [::Object::evaluate \
					[::Object::evaluate $handle Project] Handle]
			} else {
				return ""
			}
		}
		set buildSpec [::prjBuildCurrentGet $hProj]
		set dir [file dirname [::prjInfoGet $hProj fileName]]/$buildSpec
		return $dir
	}


	# Returns the fully qualified bsp for bootable projects
	proc projectBspPathGet {hProj} {
		return [::prjTagDataGet $hProj BSP_DIR]
	}

	###########################################################################
	#
	# customRuleDelete -- Removes a custom rule for a source file
	#

	proc customRuleDelete {hProj fileName} {
		set buildSpec [::prjBuildCurrentGet $hProj]
		set objects [::prjFileInfoGet $hProj $fileName objects]	

		# Delete any associated custom build rule name from the file info
		set customRule [::prjFileInfoGet $hProj $fileName customRule]
		if {$customRule != ""} {
			::prjFileInfoDelete $hProj $fileName customRule
		} else {
			set customRule $objects 
		}

		# Delete any associated custom build rule from the rule base
		if {[::prjBuildRuleGet $hProj $buildSpec $customRule] != ""} {
			::prjBuildRuleDelete $hProj $buildSpec $customRule
		}

		# Remove this rule from LDDEPS macro
		set rulesList [prjBuildMacroGet $hProj $buildSpec LDDEPS]
		set idx [lsearch -exact $rulesList $customRule]
		if {$idx != -1} {
			set rulesList [lreplace $rulesList $idx $idx]
		}
		if {[llength $rulesList]} {
			::prjBuildMacroSet $hProj $buildSpec LDDEPS $rulesList
		} else {
			::prjBuildMacroRemove $hProj $buildSpec LDDEPS
		}
	}

	###########################################################################
	#
	# customRuleAdd -- Adds a custom rule for a source file
	#
	proc customRuleAdd {hProj buildSpec fileName ruleName cmds} {

		# Remove any old custom rule
		::Workspace::customRuleDelete $hProj $fileName

		# Indicate that this file has a custom rule
		::prjFileInfoSet $hProj $fileName customRule $ruleName

		# Set the new custom build
		::prjBuildRuleSet $hProj $buildSpec $ruleName $cmds

		# add this rule to LDDEPS macro
		set rulesList [prjBuildMacroGet $hProj $buildSpec LDDEPS]
		if {[lsearch $rulesList $ruleName] == -1} {
			set rulesList [concat $rulesList $ruleName]
		}
		::prjBuildMacroSet $hProj $buildSpec LDDEPS $rulesList
	}

	###########################################################################
	#
	# obviouslyStaleDependenciesGet - return those files that have
	# never had dependencies calculated
	#

	proc obviouslyStaleDependenciesGet {hProj} {
		foreach file [::prjFileListGet $hProj] {
			if {![string match \
				[::prjFileInfoGet $hProj $file dependDone] TRUE]} {
				lappend retval $file
			}
		}
		if {![info exists retval]} {
			set retval ""
		}	
		return $retval
	}

	###########################################################################
	#
	# projectErrorStateGet - returns cached info about whether the
	# project has component configuration errors
	#
	# RETURNS:
	#	 0 - no component errors 
	#	 1 - component errors 
	#

	proc projectErrorStateGet {projectName} {
		set hProj [projectHandleGet $projectName]
		set state 0
		if {[::prjHasComponents $hProj]} {
			set state [::wccErrorStatusGet $hProj]
			if {$state == -1} {
				::ComponentViewServer::errorListUpdate $hProj
				set state [::wccErrorStatusGet $hProj]
			}
		}
		return $state
	}

	#############################################################
	#
	# View server functionality
	#
	#############################################################

	proc objectsEnumerate {path flags} {
		variable _projectHandles

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
			0 {
				# Special case: "" means just the workspace.
				if {[llength $path] == 0} {
					if {$bContainers} {
						set retval [list [::Workspace::openWorkspaceGet]]
					} else {
						set retval ""
					}
				}
			}
			1 {
				if {$bContainers} {
					if {[info exists ::Workspace::_projectHandles]} {
						foreach prjName [array names \
							::Workspace::_projectHandles] {
							lappend retval [list \
								[::Workspace::openWorkspaceGet] $prjName]
						}
						if {![info exists retval]} {
							set retval ""
						}
					} else {
						set retval ""
					}
				} else {
					set retval ""
				}
			}
			default {
				error "::Workspace::objectsEnumerate() can't return data \
					for '$path'"
			}
		}
		return $retval
	}

	proc objectBind {path} {
        set retval $::Workspace::_NULL_OBJECT

        switch [llength $path] {
			0 {
				error "::Workspace::objectBind() can't bind to '$path'"
			}
			1 {
				set hWorkspace [::Workspace::workspaceHandleGet]
				set retval [::Object::create \
					::Workspace::WorkspaceObject $hWorkspace] 
			}
			2 {
				set prjName [lindex $path 1]
				set hProj [::Workspace::projectHandleGet $prjName]
				if {$hProj != ""} {
					if {[::prjProjectFileStatusGet $hProj] == \
						"CANNOT_FIND_PROJECT_FILE"} {
						set retval [::Object::create \
							::Workspace::DeadProjectObject $hProj]
					} else {
						set retval [::Object::create \
							::Workspace::ProjectObject $hProj]
					}
				}
			}
			default {
				error "::Workspace::objectBind() can't bind to '$path'"
			}
		}
		return $retval
	}

	#############################################################
	#
	# View Server objects
	#
	# STRUCTURE -- {handle}

	# ProjectObject
	namespace eval ::Workspace::ProjectObject {
		proc Type {args} { return projectObject }
		proc bFolder {args} { return 1 }
		proc Name {args} {
			set args [lindex $args 0]
			set hProj [lindex $args 0] 
			set name [::prjInfoGet $hProj fileName]			
			return $name
		} 
		proc Handle {args} {
			set args [lindex $args 0] 
			set hProj [lindex $args 0] 
			return $hProj 
		}
		proc DisplayName {args} {
			set suffix [lindex $args 1]
			set args [lindex $args 0]
			set hProj [lindex $args 0]
			set name [::prjInfoGet $hProj fileName]
			set name [file rootname [file tail $name]]
			return [format "%s\ %s" $name $suffix]
		}
		proc Project {args} {
			set args [lindex $args 0]
			return [list ::Workspace::ProjectObject $args] 
		}
		proc Attributes {args} {
			set flags [lindex $args 1]
			set args [lindex $args 0]
			set hProj [lindex $args 0]
			set prjName [::Workspace::projectNameGet $hProj]
			set id [lindex $args 2]
			set nsp [namespace parent]

			foreach flag $flags {
				set flag [string range $flag 1 end]
				switch -exact $flag {
					icon {
						set prjType [prjTypeGet $hProj]
						switch -exact $prjType {
							::prj_vxWorks {
								if {[string match "bootrom" \
									[prjTagDataGet $hProj \
									CORE_INFO_SUBTYPE]]} {
						    	    lappend retval {-icon IDB_BOOTROM}
								} else {
							    	lappend retval {-icon IDB_BOOTABLE}
								}
							}
							::prj_vxApp {
							    lappend retval {-icon IDB_DOWNLOADABLE}
							}
							default {
								puts "unrecognized type '$prjType' \
									for project '$prjName'"
							}
						}
					}
					bold {
						lappend retval {-bold 1}
					}
					color {
						set ns ::ViewBrowser
						# red for configuration error
						if {[::Workspace::projectErrorStateGet \
							$prjName] == 1} {
							lappend retval [list -color \
								[appRegistryEntryRead -default \
								"255 0 0 255 255 255" Colors ConfigError]]
						} else {
							lappend retval [list -color \
								[appRegistryEntryRead -default \
								"0 0 0 255 255 255" Colors ConfigNormal]]
						}
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
	
	# DeadProjectObject
	namespace eval ::Workspace::DeadProjectObject {
		proc Type {args} { return deadProjectObject }
		proc bFolder {args} { return 0 }
		proc Name {args} {
			set args [lindex $args 0]
			set hProj [lindex $args 0] 
			set name [::prjInfoGet $hProj fileName]			
			return $name
		} 
		proc Handle {args} {
			set args [lindex $args 0] 
			set hProj [lindex $args 0] 
			return $hProj 
		}
		proc DisplayName {args} {
			set args [lindex $args 0]
			set hProj [lindex $args 0]
			set name [::prjInfoGet $hProj fileName]
			set name [file rootname [file tail $name]]
			set suffix "project file load failed"
			return [format "%s\ %s" $name $suffix]
		}
		proc Project {args} {
			set args [lindex $args 0]
			return [list ::Workspace::DeadProjectObject $args] 
		}
		proc Attributes {args} {
			set flags [lindex $args 1]
			set args [lindex $args 0]
			set hProj [lindex $args 0]
			set prjName [::Workspace::projectNameGet $hProj]
			set id [lindex $args 2]
			set nsp [namespace parent]

			foreach flag $flags {
				set flag [string range $flag 1 end]
				switch -exact $flag {
					icon {
						lappend retval {-icon IDB_DEADPROJECT}
					}
					bold {
						lappend retval {-bold 0}
					}
					color {
						set ns ::ViewBrowser
						# grey for dead project
							lappend retval [list -color \
								[appRegistryEntryRead -default \
								"128 128 128 255 255 255" Colors DeadProject]]
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

	# DeadProjectObject
	namespace eval ::Workspace::WorkspaceObject {
		proc Type {args} { return workspaceObject }
		proc bFolder {args} { return 1 }
		proc Name {args} {
			set args [lindex $args 0]
			set hProj [lindex $args 0] 
			set name [::prjInfoGet $hProj fileName]			
			return $name
		} 
		proc Handle {args} {
			set args [lindex $args 0] 
			set hProj [lindex $args 0]
			return $hProj 
		}
		proc DisplayName {args} {
			set args [lindex $args 0]
			set hProj [lindex $args 0]
			set name [::prjInfoGet $hProj fileName]
			return [file rootname [file tail $name]]
		}
		proc Project {args} {
			return nullObject
		}
		proc Attributes {args} {
			set flags [lindex $args 1]
			set args [lindex $args 0]
			set hProj [lindex $args 0]
			set name [::prjInfoGet $hProj fileName]
			set nsp [namespace parent]

			foreach flag $flags {
				set flag [string range $flag 1 end]
				switch -exact $flag {
					icon {
						lappend retval {-icon IDB_WORKSPACE}
					}
					bold {
						lappend retval {-bold 1}
					}
					color {
						lappend retval [list -color \
							[appRegistryEntryRead -default \
							"0 0 0 255 255 255" Colors ConfigNormal]]
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
	# The Workspace main entry point
	#
	#############################################################

	# Source each addin
	set path "[wtxPath host resource tcl app-config Project]*.addin.tcl"
	foreach addin [lsort [glob $path]] {
		if {[catch {source $addin} errorMsg]} {
			puts "error in source of addin '$addin': $errorMsg"
		}
	}
	# Add-in Init() code for the workspace --
	# register to receive WorkspaceChange events
	::Workspace::eventRegister ::Workspace {^Workspace.*$}

	# View Server Init() code for the workspace --
	# start up the view browser 
	namespace eval :: {
		source "[wtxPath host resource tcl app-config Project]ViewBrowser.tcl"
	}

	# Call each addin's Init()
	foreach addin [addinsGet] {
		eval ${addin}::Init
	}
}
