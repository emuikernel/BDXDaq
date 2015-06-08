# ComponentViewServer.addin.tcl - the component view server implementation
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 04k,14may02,rbl  fixing SPR 77280, "Project facility messes up dependencies
#                  on a selection  e.g. can't change WDB connection"
# 04j,09may02,cjs  Fix SPRs 75901, 76221 (permamently green components)
# 04i,25apr02,cjs  Report build errors during autoscale (spr 73344)
# 04h,13mar02,rbl  Fix bug where unavailable list was not stored per-project
# 04g,31mar99,cjs  Fixed bug that prevents component error correction from
#                  updating component view
# 04f,30mar99,jak  catch error on find failure
# 04e,24mar99,cjs  Make all view servers catch WorkspaceUpdate event
# 04d,22mar99,cjs  Changing text for object find dialog
# 04c,16mar99,cjs  Adding param find support to find component dialog 
# 04b,16mar99,ren  Reformatting configuration error message
# 04a,05mar99,cjs  Fix bug introduced by addition of workspace object to view
# 03z,02mar99,cjs  Add workspace icon to all view servers
# 03y,23nov98,cjs  Remove extra brace causing obscure bug
# 03x,03nov98,cjs  add error handling for missing archive file or tool
#                  from toolchain 
# 03w,19oct98,cjs  Ask for, and pass along, extended autoscale data; fixed
#                  size info returned by autoscale
# 03v,15oct98,cjs  Changed default selection color to dark green 
# 03u,13oct98,cjs  Changed menu captions (Add -> Include, etc)
# 03t,09oct98,cjs  fixed autoscale 
# 03s,02oct98,cjs  protect gui/non-gui interface w/ viewBrowserIsVisible()
#                  checks 
# 03r,30sep98,cjs  add modeless dialog to restore found components'
#                  highlighting when user is done viewing
# 03q,30sep98,cjs  altered text in error introduction dialog 
# 03p,29sep98,ms   fixed INCLUDE_WHEN handling
# 03o,21sep98,ms   modified errorListUpdate to do all the time-consuming
#		   engine work at once.
# 03n,18sep98,ms   fixed dependency calculation on component and folder add
# 03m,18sep98,cjs  fixed bug that allows all objects to show up in green;
#                  component find now stains tree all the way up; and expands
#                  path for node and selects it
# 03l,15sep98,cjs  send a ProjectUpdate event when the error list changes 
# 03k,14sep98,cjs  call OnWatch() of root node to ensure that
#                  error status change for projects is reflected 
# 03j,09sep98,cjs  warn if config errors introduced
# 03i,04sep98,cjs  fetch colors for errors; store persistent info about
#                  project errors in the workspace
# 03h,31aug98,cjs  '&' dependencies w/ cset contents when displaying delete
#                  dependencies 
# 03g,18aug98,cjs  make objectsEnumerate return _NULL_OBJECT when things
#                  go south 
# 03f,18aug98,cjs  deleting folder deletes dependencies too;
#                  fixed size calculation for component add 
# 03e,13aug98,cjs  update errors on component add, delete, addDelete;
#                  don't show projects that don't have components
# 03d,04aug98,cjs  added component search facility 
# 03c,04aug98,cjs  call implemented autoscale 
# 03b,31jul98,ms   call wccValidate instead of cxrSetValidate
# 03a,30jul98,cjs  revised objects, implement demand drilling, got rid
#                  of component add dialog...
# 02g,15jul98,cjs  fixed bug in OnCSetDelete()
# 02f,13jul98,cjs  fixing folder add, folder delete, adapt to
#                  new dependency dialog
# 02e,09jul98,cjs  added first implementation of autoscale 
# 02d,06jul98,cjs  changed signature and behavior of depDlgShow() 
#                  renamed derived objects folder to build objects
# 02c,27jun98,cjs  added more config error flagging support 
# 02b,24jun98,cjs  added icon support 
# 02a,23jun98,cjs  center and revise active component set selection dialog;
#                  crunch virtually every object into globally unique ids;
#                  implement add and delete of component sets
# 01l,15jun98,cjs  added open/dblclk behavior for componentSet object;
#                  added current comp set selection dialog; misc cleanup 
# 01k,09jun98,cjs  remove size pane; add support for selection
#                  configuration; add logic to set tree selection
#                  after component add/remove
# 01k,06jun98,cjs  added size pane
# 01j,01may98,cjs  removed any reference to subsystems; replaced
#                  with selection
# 01i,01may98,cjs  updated for use with new tree control;
#                  fixed component add/delete
# 01h,23apr98,cjs  removed cdf code; moved to workspace 
# 01g,21apr98,cjs  added Handle() method to ProjectObject.
# 01f,14apr98,cjs  add folder hierarchy to display 
# 01e,09apr98,cjs  fixed Title() 
# 01d,31mar98,cjs  got rid of unnecessary format() statements,
#                  changed object syntax for objs returned from bind
# 01c,27mar98,cjs  changed references to className(), objectType()
#                  to classTitle(), className(), respectively
# 01b,27mar98,cjs  changed event logic; now respond to Project
#                  add/remove.
# 01a,27mar98,cjs  written.
#
# DESCRIPTION
# This file implements the Component View Server.  View Servers
# populate a pane in the view browser by fetching data from
# a lower-level data engine.
# 

#############################################################

source "[wtxPath host resource tcl app-config Project]ConfigAddDlg.tcl"
source "[wtxPath host resource tcl app-config Project]DepDlg.tcl"

namespace eval ::ComponentViewServer {
	# _errorMap maps component names to their error data
	# _errorList is a list of the components in error for
	# a particular project
	# _selectList maps contains components to be drawn
	# in special colors to their full paths.  Currently, this
	# supports autoscale only.

	variable _errorList
	variable _errorMap
	variable _selectList
	variable _nameMap

	# array of unavailable components per project
	variable _unavailableList

	# list of project handles for which we have unavailable lists
	variable _unavailableListPrjHandles ""

	array set _unavailableList {}

	#############################################################
	#
	# Addin implementation 
	#
	#############################################################

	proc Init {} {
		set pattern {(^Project.*$)|(^ComponentAdd.*$)|(^ComponentDelete.*$)|(^ComponentSet.*$)|(^ComponentAddDelete.*$)|(^CompErrorsUpdate.*$)|(^WorkspaceUpdate$)}
		::Workspace::eventRegister ::ComponentViewServer $pattern

		::Workspace::serverRegister ::ComponentViewServer

		# Register server's menu items
		::ViewBrowser::menuServerRegister ::ComponentViewServer::gui ""

		set ns ::ComponentViewServer::gui
		# Register to handle particular gui events for particular objects
		::ViewBrowser::guiEventHandlerRegister "dblclk" \
			"componentObject" ${ns}::OnComponentOpen
		::ViewBrowser::guiEventHandlerRegister "dblclk" \
			"selectionObject" ${ns}::OnComponentOpen
		::ViewBrowser::guiEventHandlerRegister "delete" \
			"componentObject" ${ns}::OnComponentDelete
		::ViewBrowser::guiEventHandlerRegister "delete" \
			"componentFolderObject" ${ns}::OnFolderDelete
		::ViewBrowser::guiEventHandlerRegister "dblclk" \
			"componentFolderObject" ${ns}::OnComponentOpen

		catch {unset ::ComponentViewServer::gui::_data}
		catch {unset ::ComponentViewServer::gui::_children}
	}

	###########################################################################
	#
	# parentPathGet - returns the fully qualified cookie path for this cxrDoc
	# item 
	# XXX - maybe unify this so that it works for projects too?
	proc parentPathGet {workspaceName prjName itemId} {
		set hProj [::Workspace::projectHandleGet $prjName]
		set cDoc [::Workspace::cxrDocGet $prjName]
		set cSet [$cDoc setCreate]
		set path "" 
		set parentId $itemId

		# Sanity check
		$cSet = $itemId
		if {[$cSet instances] == ""} {
			error "'$itemId' doesn't have a parent!"
		}

		while {![string match $parentId FOLDER_ROOT]} {
			set path [concat $parentId $path]
			$cSet = $parentId
			$cSet = [$cSet get _CHILDREN]
			set parentId [$cSet instances]
			if {$parentId == ""} {
				puts "WARNING: $itemId has no folder; placing in FOLDER_ROOT"
				set parentId FOLDER_ROOT
			} 
		}
		set path [concat $workspaceName $prjName $path]
		$cSet delete
		return $path
	}

	###########################################################################
	#
	# sizeInfoGet - get size data for two lists of components, old and new 
	#
	proc sizeInfoGet {cSet mDoc oldSet neededComponents} { 
		$cSet = $oldSet
		set oldSize [::cxrSizeGet $cSet $mDoc]
		$cSet = $neededComponents
   		set newSize [::cxrSizeGet $cSet $mDoc]
		set deltaSize [list \
			[expr [lindex $newSize 0] - [lindex $oldSize 0]] \
			[expr [lindex $newSize 1] - [lindex $oldSize 1]] \
			[expr [lindex $newSize 2] - [lindex $oldSize 2]] \
		]
		lappend deltaSize [expr \
			[lindex $deltaSize 0] + \
			[lindex $deltaSize 1] + \
			[lindex $deltaSize 2] \
		]
		lappend newSize [expr \
			[lindex $newSize 0] + \
			[lindex $newSize 1] + \
			[lindex $newSize 2] \
		]
		return [list $deltaSize $newSize]
	}

	###########################################################################
	#
	# errorListGet - returns the list of components and selections that
	# have errors.
	# NOTE: will update error list if necessary
	#
	proc errorListGet {hProj} {
		variable _errorList
		if {![info exists _errorList($hProj)]} {
			errorListUpdate $hProj
		}
		return $_errorList($hProj)
	}

	###########################################################################
	#
	# errorListGet - returns the list of components and selections that
	# Returns the error message associated with item 'id'
	# in project 'hProj'
	# NOTE: will update error list if necessary
	proc errorLookup {hProj id} {
		variable _errorMap
		variable _errorList
		if {![info exists _errorList($hProj)]} {
			errorListUpdate $hProj
		}

		set retval "" 
		if {$_errorList($hProj) != ""} {
			set idx [lsearch $_errorList($hProj) $id]
			if {$idx != -1} {
				set retval [lindex $_errorMap($hProj) $idx]
			}
		}
		return $retval
	}

	###########################################################################
	#
	# errorListUpdate - (re)generate the list of components that have errors
	# associated with them, and establish the component to error struct map. 
	# NOTE: building this list is costly
	#
	# Builds the error references
	proc errorListUpdate {hProj} {
		set bError 0
		variable _errorList
		variable _errorMap
		set _errorList($hProj) ""
		set _errorMap($hProj) ""

		::beginWaitCursor
		::Workspace::statusMessagePush "Reading component descriptor files..."
		if {![catch {::cxrDocCreate $hProj} error]} {
			::Workspace::statusMessagePush "Getting module xref info..."
			if {![catch {::mxrDocCreate $hProj} error]} {

				# Check for uninstalled components
				::Workspace::statusMessagePush \
					"Checking component availability..."
				if {![catch {unavailableListUpdate $hProj} error]} {
					::Workspace::statusMessagePush "Validating configuration..."

					set prjName [::Workspace::projectNameGet $hProj]
					set wcc [::wccCurrentGet $hProj]
					set cDoc [::Workspace::cxrDocGet $prjName]
					set cSet [$cDoc setCreate]
					$cSet = [::wccComponentListGet $hProj $wcc]
					set errorList [::wccValidate $hProj $cSet]

					foreach error $errorList {
						set id [lindex [lindex $error 0] 0]
						$cSet = $id
						parentContainersGet $cSet
						foreach item [$cSet instances] {
							lappend _errorList($hProj) $item
							lappend _errorMap($hProj) $error
						}
					}

					$cSet delete
				} else {
					set bError 1
				}
			} else {
				set bError 1
			}
		} else {
			set bError 1
		}
		::endWaitCursor

		if {$bError} {
			::Workspace::statusMessagePush "Configuration validation failed"
			if {[info commands messageBox] != ""} {
				messageBox -ok -exclamationicon $error
			} else {
				error $error
			}
		} else {
			::Workspace::statusMessagePush "Validating configuration...Done"

			# Warn the user that errors exist
			if {$errorList != ""} {
				messageBox -ok "Your project contains configuration errors.\n\
					In the 'VxWorks' view, follow the highlighted folders\
					(colored red by default) until you reach the\
					component(s) or selection(s) annotated with the error\
					icon.  Double-click each of these to display its\
					property sheets.  The first property page will describe\
					the error."
			}
		}
	}

	proc bError {hProj id} {
		set retval 0
		if {[errorLookup $hProj $id] != ""} {
			set retval 1
		}
		return $retval
	}

	###########################################################################
	#
	# selectLookup - returns "", or the id of the item.  Can easily be modified 
	# to returns tree paths instead 
	#
	proc selectLookup {prjName id} {
		variable _selectList
		set retval "" 

		if {[info exists _selectList($prjName,$id)]} {
			set retval $_selectList($prjName,$id)
		}
		return $retval
	}

	proc selectListSet {prjName itemList} {
		variable _selectList
		selectListClear $prjName

		::beginWaitCursor
		::Workspace::statusMessagePush \
			"Updating list of selected components..."

		set cDoc [::Workspace::cxrDocGet $prjName]
		set cSet [$cDoc setCreate]

		# Get each item and its parents and put them in
		# the _selectList array
		if {$itemList != ""} {
			foreach comp $itemList {
				$cSet = $comp
				parentContainersGet $cSet
				foreach item [$cSet instances] {
					set _selectList($prjName,$item) $item 
				}
				set _selectList($prjName,$comp) $comp 
			}
		}

		$cSet delete

		::Workspace::statusMessagePush \
			"Updating list of selected components...Done"
		::endWaitCursor
	}

	proc selectListGet {prjName} {
		variable _selectList
		if {[info exists _selectList]} {
			set retval [array names _selectList ${prjName}*]
		} else {
			set retval ""
		}
		return $retval
	}
 
	proc selectListClear {prjName} {
		variable _selectList
		# Clear the map of selected items
		foreach item [array names _selectList ${prjName}*] {
		    unset _selectList($item)
        }
	}

	proc bSelect {prjName id} {
		variable _selectList
		set retval 0
		if {[info exists _selectList(${prjName},${id})]} {
			set retval 1
		}
		return $retval
	}

	###########################################################################
	#
	# unavailableListGet - returns the list of components that are currently
	# unavailable.
	#
	proc unavailableListGet {hProj} {
		variable _unavailableList

		# Update on demand
		if {![info exists _unavailableList($hProj)]} {
			unavailableListUpdate $hProj
		}

		if {![info exists _unavailableList($hProj)]} {
			set _unavailableList($hProj) ""
		} else {
			return $_unavailableList($hProj)
		}
	}

	proc unavailableListUpdate {hProj} {
		variable _unavailableList
		variable _unavailableListPrjHandles

		set bError 0
		::beginWaitCursor
		::Workspace::statusMessagePush "Checking installed components..."

		unavailableListCleanUp

		set prjName [::Workspace::projectNameGet $hProj]
		if {![catch {set cDoc [::Workspace::cxrDocGet $prjName]} error]} {
			if {![catch {set mDoc [::Workspace::mxrDocGet $prjName]} error]} {
				if {![catch {set _unavailableList($hProj) \
					[::cxrUnavailListGet $cDoc $mDoc]} error]} {
					if {[lsearch $_unavailableListPrjHandles $hProj] == -1} {
						lappend _unavailableListPrjHandles $hProj
					}
					::Workspace::statusMessagePush \
						"Checking installed components...Done"
				} else {
					set bError 1
				}
			} else {
				set bError 1
			}
		} else {
			set bError 1
		}
		::endWaitCursor

		if {$bError} {
			::Workspace::statusMessagePush \
				"Unable to check for installed components"
			if {[info commands messageBox] != ""} {
				messageBox -ok -exclamationicon $error
			} else {
				error $error
			}
		}
	}

	#
	# remove lists corresponding to stale handles
	# (closed projects)
	#
	proc unavailableListCleanUp {} {
	    variable _unavailableList
	    variable _unavailableListPrjHandles

	    for {set i 0} {$i < [llength $_unavailableListPrjHandles]} {incr i} {
                set hProj [lindex $_unavailableListPrjHandles $i]
	        if {[catch {prjHandleValidCheck $hProj}] && \
                    [info exists _unavailableList($hProj)]} {
                    unset _unavailableList($hProj)
                    set _unavailableListPrjHandles \
                        [lreplace $_unavailableListPrjHandles $i $i]
                    incr i -1
	        }
	    }
	}

	###########################################################################
	#
	# ID to friendly name mapping routines
	#
	proc nameReset {} {
		catch {unset ComponentViewServer::_nameMap}
	}

	proc idToName {cSet} {
		# If it doesn't have a display name, use it's id
		set ids [$cSet contents]
		foreach id $ids {
			$cSet = $id
			set retval [$cSet get NAME]
			if {$retval == ""} {
				set retval $id 
			}

			# If it's a folder, prefix it w/ <Folder>
			set type [$cSet types] 
			if {[string match $type Folder]} {
				set retval [format "<Folder> %s" $retval]
			}
			set ComponentViewServer::_nameMap($retval) $id
			lappend retvals $retval
		}

		if {![info exists retvals]} {
			set retvals ""
		}
		return $retvals
	}

	proc nameToId {items} {
		foreach item $items { 
			if {$item == ""} {
				error "nameToId($item): item cannot be ''"
			} elseif {[string compare $item \
				{<Back to Parent Folder>}] != 0} {

				# Match selection's display name with its id
				if {![info exists ComponentViewServer::_nameMap($item)]} {
					error "nameToId() couldn't map '$item'"
				} else {
					lappend retvals [set ComponentViewServer::_nameMap($item)]
				}
			} else {
				lappend retvals $item
			}
		}
		if {![info exists retvals]} {
			set retvals ""
		}
		return $retvals
	}

	###########################################################################
	#
	# OnEvent - tell the gui about a list of cookie paths that should
	# be refreshed.  The gui only wants the ancestor of nodes that have
	# changed.  It will take care of asking for the children of these
	# ancestor nodes, and merging these children into the existing tree
	# so as not to lose the current expansion state of the tree
	#
	proc OnEvent {cookie eventInfo} {
		set eventData [lindex $eventInfo 1]
		set eventName [lindex $eventInfo 0]

		if {(![string match $cookie ::ComponentViewServer::gui]) && \
			([::ViewBrowser::viewBrowserIsVisible])} {
			switch $eventName {
				ProjectAdd {
					# Expand tree one level to make sure projects are visible
					if {[string match "ProjectAdd" $eventName]} {
						set root [objectsEnumerate "" -containers]
						set firstChild [objectsEnumerate \
							$root {-leaves -containers}]
						set firstChild [lindex $firstChild 0]
						set cookieLists [list $firstChild]
						::ViewBrowser::OnWatch [namespace current]::gui \
							[list ""] 
						::ViewBrowser::OnWatch [namespace current]::gui \
							$cookieLists
						::ViewBrowser::pathShow [namespace current]::gui \
							$cookieLists
					} else {
						set cookieLists [list ""]
						::ViewBrowser::OnWatch ::FileViewServer::gui \
							$cookieLists
					}
				}
				ProjectDelete -
				ProjectUpdate {
					set cookieLists [list ""]
					::ViewBrowser::OnWatch ::ComponentViewServer::gui \
						$cookieLists
				}
				ComponentAdd -
				ComponentDelete {
					set prjName [lindex $eventData 0]
					set hProj [::Workspace::projectHandleGet $prjName]
					errorListUpdate $hProj
					set cookieLists [list ""]
					::ViewBrowser::OnWatch ::ComponentViewServer::gui \
						$cookieLists
					::Workspace::eventSend ::ComponentViewServer::gui \
						[list ProjectUpdate $prjName]
				}
				ComponentAddDelete {
					set prjName [lindex $eventData 0]
					set hProj [::Workspace::projectHandleGet $prjName]
					errorListUpdate $hProj
					set cookieLists [list ""]
					::ViewBrowser::OnWatch ::ComponentViewServer::gui \
						$cookieLists
					::Workspace::eventSend ::ComponentViewServer::gui \
						[list ProjectUpdate $prjName]
				}
				CompErrorsUpdate {
					set prjName [lindex $eventData 0]
					set root [objectsEnumerate "" -containers]
					set cookieLists $root 
					::ViewBrowser::OnWatch ::ComponentViewServer::gui \
						$cookieLists

					# Component server won't get this event
					::Workspace::eventSend ::ComponentViewServer::gui \
						[list ProjectUpdate $prjName]
				}
				WorkspaceUpdate {
					::ViewBrowser::OnWatch ::ComponentViewServer::gui [list ""] 
				}
			}
		}
	}

	proc titleGet {} {
		return "VxWorks"
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
		set retval [::Object::evaluate $obj Attributes [list $flagList]]
		return $retval
	}

	proc bFolder {path} {
		set retval 0
		set obj [objectBind $path]
		set retval [::Object::evaluate $obj bFolder]
		return $retval
	}

	proc objectBind {path} {
		set obj $::Workspace::_NULL_OBJECT 

		switch [llength $path] {
			0 {
				error "objectBind: bad path '$path'"
			}
			1 -
			2 {
				# Bind to a project; punt to workspace
				set obj [::Workspace::objectBind $path]
			}
			default {
				set workspaceName [lindex $path 0]
				set prjName [lindex $path 1]
				set hProj [::Workspace::projectHandleGet $prjName]
				if {$hProj != ""} {
					set wcc [::wccCurrentGet $hProj]
					set pathLen [llength $path]
					set id [lindex $path [expr $pathLen - 1]]
					set cDoc [::Workspace::cxrDocGet $prjName] 
					set cSet [$cDoc setCreate]
					$cSet = $id
					switch [$cSet types] {
						Folder {
							set obj [::Object::create \
								::ComponentViewServer::ComponentFolderObject \
								[list $hProj $wcc $id]]
						}
						Component {
							set obj [::Object::create \
								::ComponentViewServer::ComponentObject \
								[list $hProj $wcc $id]]
	
						}
						Selection {
							set obj [::Object::create \
								::ComponentViewServer::SelectionObject \
								[list $hProj $wcc $id]]
						}
						default {
							# XXX todo -- deal w/ routines
						}
					}
					$cSet delete
				}
			}
		}
		return $obj
	}

	# Given a particular path, enumerate cookies corresponding to items
	# at that level
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

		set workspaceName [lindex $path 0]
		if {[llength $path] == 0} {
			if {$bContainers} {
				set retval [::Workspace::objectsEnumerate $path $flags]
			} else {
				set retval ""
			}
		} elseif {[llength $path] == 1} {
			if {$bContainers} {
				# Normally, we'd just call ::Workspace::objectsEnumerate(),
				# but here we need to know if the projects have components.
				foreach prjPath [::Workspace::objectsEnumerate $path $flags] {
					set prjName [lindex $prjPath 1]
					set hProj [::Workspace::projectHandleGet $prjName]
					if {$hProj != ""} {
						if {[::prjHasComponents $hProj]} {
							lappend retval [list $workspaceName $prjName]
						}
					} else {
						set error "invalid project handle for '$prjName' in \
							::ComponentViewServer::objectsEnumerate"
						error $error
					}
				}
			} else {
				set retval ""
			}
		} else {
			set prjName [lindex $path 1]
			set hProj [Workspace::projectHandleGet $prjName]
			if {[::prjHasComponents $hProj]} {
				set wcc [::wccCurrentGet $hProj]
				set cDoc [::Workspace::cxrDocGet $prjName] 
				set cSet [$cDoc setCreate]

				if {[llength $path] == 2} {
					$cSet = FOLDER_ROOT
				} else {	
					set pathLen [llength $path]
					set id [lindex $path [expr $pathLen - 1]]
					$cSet = $id
				}
				$cSet = [$cSet get CHILDREN]

				set items ""
				if {$bLeaves} {
					set items [::ViewBrowser::flatJoin $items \
						[$cSet instances Component]]
					set items [::ViewBrowser::flatJoin $items \
						[$cSet instances Selection]]
				}
				if {$bContainers} {
					set items [::ViewBrowser::flatJoin $items \
						[$cSet instances Folder]]
				}

				foreach item $items {
					lappend retval [parentPathGet \
						$workspaceName $prjName $item]
				}
				$cSet delete
			}
		}
		if {![info exists retval]} {
			set retval ""
		}

		return $retval
	}

	proc errorsUpdate {handle} {
		set hProj [::Object::evaluate \
			[::Object::evaluate $handle Project] Handle]
		set prjName [::Object::evaluate \
			[::Object::evaluate $handle Project] Name]

		errorListUpdate $hProj

		# Send this anonymously, so we won't filter the message out
		::Workspace::eventSend anonymous \
			[list CompErrorsUpdate $prjName]
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

	proc menuMatch {type server handle} {
		set ns ::ComponentViewServer::gui
		set objType [::Object::evaluate $handle Type]
		if {([string match "Server" $type]) && \
			([string match "::ComponentViewServer::gui" $server])} {

			# Server-specific menus	go here
			if {![string match "workspaceObject" $objType]} {
				::ViewBrowser::menuAdd "&Auto\ Scale..." \
					[list ${ns}::OnAutoScale "" $handle]
				::ViewBrowser::menuAdd "&Find Object..." \
					[list ${ns}::OnComponentFind "" $handle]
			}

		} elseif {[string compare $type Object] == 0} {
			# Object-specific menus

			set name [::Object::evaluate $handle DisplayName]
			set type [::Object::evaluate $handle Type]
			switch $type {
				componentObject {

					set included [::Object::evaluate $handle bIncluded]
					set available [::Object::evaluate $handle bAvailable]
					# If this component is in the configuration,
					# it can be opened
					if {$included} {
						::ViewBrowser::menuAdd "&Properties of\ '$name'" \
							[list ${ns}::OnComponentOpen "dblclk" $handle]
					} else {
						::ViewBrowser::menuAdd "&Properties of\ '$name'" \
							[list messageBox -ok "Component '$name' \
							is not in your project"] 
					}

					# If this component is not unavailable and is not
					# in the configuration, it can be added 
					if {!$included} { 
						if {$available} {
							# It can be added
							::ViewBrowser::menuAdd "&Include\ '$name'..." \
								[list ${ns}::OnComponentAdd "insert" $handle]
						} else {
							# It isn't installed
							::ViewBrowser::menuAdd "&Include\ '$name'..." \
								[list messageBox -ok "Component '$name' \
								is not installed.\nYou may need to \
								purchase and/or install it."]
						}
					} else {
						# It's already in
						::ViewBrowser::menuAdd "&Include\ '$name'..." \
							[list ${ns}::OnComponentAdd "insert" $handle] 1
					}

					# If this component is in the configuration,
					# it can be deleted
					if {$included} {
						::ViewBrowser::menuAdd "E&xclude\ '$name'" \
							[list ${ns}::OnComponentDelete "delete" $handle]
					} else {
						::ViewBrowser::menuAdd "E&xclude\ '$name'" \
							[list ${ns}::OnComponentDelete "delete" $handle] 1
					}
				}
				selectionObject {
					set included [::Object::evaluate $handle bIncluded]
					if {$included} {
						::ViewBrowser::menuAdd "&Configure\ '$name'" \
							[list ${ns}::OnComponentOpen "dblclk" $handle]
					} else {
						::ViewBrowser::menuAdd "&Configure\ '$name'" \
							[list ${ns}::OnComponentOpen "dblclk" $handle] 1
					}
				}
				componentFolderObject {
					set included [::Object::evaluate $handle bIncluded]
					set allIncluded [::Object::evaluate $handle bAllIncluded]
					if {$included} {
						::ViewBrowser::menuAdd "Para&ms for\ '$name'" \
							[list ${ns}::OnComponentOpen "dblclk" $handle]
						::ViewBrowser::menuAdd "E&xclude\ '$name'" \
							[list ${ns}::OnFolderDelete "delete" $handle]
					} else {
						::ViewBrowser::menuAdd "Para&ms for\ '$name'" \
							[list ${ns}::OnComponentOpen "dblclk" $handle] 1
						::ViewBrowser::menuAdd "E&xclude\ '$name'" \
							[list ${ns}::OnFolderDelete "delete" $handle] 1
					}
					if {!$allIncluded} {
						::ViewBrowser::menuAdd "&Include\ '$name'..." \
							[list ${ns}::OnFolderAdd "insert" $handle]
					} else {
						::ViewBrowser::menuAdd "&Include\ '$name'..." \
							[list ${ns}::OnFolderAdd "insert" $handle] 1
					}
				}
				default {}
			}
		}
	}

	#############################################################
	#
	# Menu callbacks 
	#
	#############################################################

	proc OnComponentOpen {event handle} {
		::Workspace::propSheetShow
	}

	proc OnComponentFind {event handle} {
		if {![catch {set prjObj [::Object::evaluate $handle Project]}]} {
			set workspaceName [::Workspace::openWorkspaceGet]
			set prjName [::Object::evaluate $prjObj Name]
			set ns ::ComponentViewServer::gui::ComponentFindDlg 
			${ns}::componentFindDlgShow $prjName
			set item [set ${ns}::_macroId]
			set objectType [set ${ns}::_objectType]

			# if it was a param and it wasn't found, a message would have 
			# appeared and item would be "". If it was any other type and 
			# not found, the parent location would cause an error. 
			if {[string length [string trim $item]] == 0} {
				return
			}
			if {[catch { \
				set path [[namespace parent]::parentPathGet \
					$workspaceName $prjName $item] \
						} err] } {
				messageBox "Unable to find $objectType \"$item\""
				return
			}

			# Set the select list, trigger a tree update, then call pathShow()
			# to expand and select the found item
			if {$item != ""} {
				[namespace parent]::selectListSet $prjName [list $item]
				set cookieLists [lindex $path 0]
				::ViewBrowser::OnWatch ::ComponentViewServer::gui $cookieLists
				::ViewBrowser::pathShow ::ComponentViewServer::gui [list $path]

				# If the object type is a param, the show the property sheet
				# for the found component
				if {[string match "Parameter" $objectType]} {
					::Workspace::propSheetShow "Params"
				}
				::ComponentViewServer::gui::ComponentFindDlg::findShow	
			}
		}
	}

	#############################################################
	#
	# AutoScale -
	# (1) tell user what must be added.  If approved, add
	# (2) post modeless dialog telling user that components 
	# now drawn in green can possibly be deleted; closing dialog
	# ends this mode
	#
	proc OnAutoScale {event handle} {
		::ComponentViewServer::gui::AutoScaleAddDlg::autoScaleAddDlgShow
		if {[string match \
			$::ComponentViewServer::gui::AutoScaleAddDlg::_retCode \
			ok]} {

			set bError 0
			set ns ::ComponentViewServer::gui::DepDlg
			set projObj [::Object::evaluate $handle Project]
			set prjName [::Object::evaluate $projObj Name]
			set hProj [::Object::evaluate $projObj Handle]
			set cSetName [::wccCurrentGet $hProj] 

			# Fetch the components that are required for the current
			# configuration.  These must be added.
			::beginWaitCursor	
			::Workspace::statusMessagePush "Calculating components\
				required by your code..."
			if {![catch {set cDoc [::Workspace::cxrDocGet $prjName]} error]} {
				if {![catch {set mDoc \
					[::Workspace::mxrDocGet $prjName]} error]} {
					set cSet [$cDoc setCreate]

					if {[catch {set struct [::prj_vxWorks_hidden::autoscale \
						$hProj verbose throwErrors]} error]} {
						$cSet delete
						set bError 1
					} else {
						set neededComponents [lindex $struct 0]
						set extraData [lrange $struct 1 end]

						set oldSet [::wccComponentListGet $hProj $cSetName]
						$cSet = $neededComponents
						$cSet = [$cSet - $oldSet]
						set required [$cSet instances]
						$cSet = $oldSet
						$cSet = [$cSet - $neededComponents]
						set notrequired [$cSet instances] 
	
						# Convert the components to friendly names 
						$cSet = $required
						set friendlyRequired \
							[[namespace parent]::idToName $cSet]
						$cSet = $oldSet
						$cSet = [$cSet + $neededComponents]	
						set neededComponents [$cSet instances Component]

						# Calculate the image's size if these components
						# are added
						set sizeData [[namespace parent]::sizeInfoGet \
							$cSet $mDoc $oldSet $neededComponents]
						$cSet delete
		
						# Show the friendly names and dependencies to the user
						if {$required != ""} {
							${ns}::depDlgShow autoscale \
								$friendlyRequired "" $extraData "" \
								[lindex $sizeData 0] [lindex $sizeData 1]
							if {[string match [set ${ns}::_retCode] ok]} {
								# Add the component(s)
								::Workspace::componentAdd \
									::ComponentViewServer \
									$prjName $cSetName $required 
							}
						} else {
							messageBox -ok "No new components needed"
						}
		
						# Now tell the user about the deletion options
						if {$notrequired != ""} {
							set ns [namespace current]::AutoScaleRmDlg
							${ns}::autoScaleRmDlgShow $prjName $notrequired
						} else {
							messageBox -ok "No components can be\
								removed at this time"
						}
					}
				} else {
					set bError 1
				}
			} else {
				set bError 1
			}
			::endWaitCursor

			if {$bError} {
				::Workspace::statusMessagePush "Auto scale failed"
				messageBox -ok -exclamationicon "Auto Scale failed: $error"
			}
		}
	}

	proc OnComponentAdd {event handle} {
		::beginWaitCursor

		set ns ::ComponentViewServer::gui::DepDlg
		set component [::Object::evaluate $handle Name]

		# We need the name of the component set and
		# a handle to the project 
		set prjObj [::Object::evaluate $handle Project]
		set hProj [::Object::evaluate $prjObj Handle]
		set prjName [::Object::evaluate $prjObj Name]
		set cSetName [::Object::evaluate $handle ComponentSet]

		set bError 0
		if {![catch {set cDoc [::Workspace::cxrDocGet $prjName]} error]} {
			if {![catch {set mDoc [::Workspace::mxrDocGet  $prjName]} error]} {
				set cSet [$cDoc setCreate]
				set wccList [::wccComponentListGet $hProj $cSetName]

				# Check dependencies on the component; show dependencies 
				# to user

				if {$component != ""} {
					$cSet = $component
					set friendlyComponents \
						[[namespace parent]::idToName $cSet]

					# Get dependencies
					set cSet2 [$cSet setCreate]
					$cSet = $component
					$cSet2 = $wccList
					set depList [lsort [cxrDependsGet $cSet $cSet2 $mDoc]]
					$cSet2 delete

					# Convert the dependencies back to friendly names
					if {$depList != ""} {
						$cSet = $depList
						set friendlyDeps [[namespace parent]::idToName $cSet]
					} else {
						set friendlyDeps ""
					}
				
					# Calculate the delta and new image size
					set oldSet $wccList
					$cSet = [$cSet + $wccList]
					$cSet = [$cSet + $component]
					$cSet = [$cSet + $depList]
					set neededComponents [$cSet instances]
					set sizeData [[namespace parent]::sizeInfoGet \
						$cSet $mDoc $oldSet $neededComponents]

					# Show the friendly names and dependencies to the user
					${ns}::depDlgShow add $friendlyComponents $friendlyDeps \
						"" "" [lindex $sizeData 0] [lindex $sizeData 1]

					if {[string match [set ${ns}::_retCode] ok]} {
						# Merge the components and dependencies into one list
						set componentList [concat $component $depList]

						# Add the component(s)
						if {$componentList != ""} {
							::Workspace::componentAdd ::ComponentViewServer \
								$prjName $cSetName $componentList
						}
					}
				}

				$cSet delete
			} else {
				set bError 1
			}
		} else {
			set bError 1
		}
		::endWaitCursor

		if {$bError} {
			messageBox -ok -exclamationicon $error
		}
	}

	proc OnFolderAdd {event handle} {
		::beginWaitCursor

		set ns ::ComponentViewServer::gui::DepDlg
		set ns2 ::ComponentViewServer::gui::FolderDlg

		# We need the name of the component set and
		# a handle to the project 

		set prjObj [::Object::evaluate $handle Project]
		set hProj [::Object::evaluate $prjObj Handle]
		set prjName [::Object::evaluate $prjObj Name]
		set cSetName [::Object::evaluate $handle ComponentSet]

		set bError 0
		if {![catch {set cDoc [::Workspace::cxrDocGet $prjName]} error]} {
			if {![catch {set mDoc [::Workspace::mxrDocGet  $prjName]} error]} {
				set cSet [$cDoc setCreate]
				set wccList [::wccComponentListGet $hProj $cSetName]

				# Get the list of default components for this folder that are
				# not currently in the config and allow the user to play
				# with them 
				set folder [::Object::evaluate $handle Name]
				$cSet = $folder

				# Get the default components for the folder
				::defaultFolderSubTree $cSet

				set bCancel 0

				# Remove from the folder defaults those that
				# are already in our configuration,
				# and those that are unavailable
				$cSet = [$cSet - $wccList]
				set unavailable [::cxrUnavailListGet $cDoc $mDoc]
				$cSet = [$cSet - $unavailable]
				set folderDefaults [$cSet instances Component]

				# Get the folder contents
				$cSet = $folder
				set folderName [$cSet get NAME]
				$cSet = [::cxrSubfolders $cSet]

				# subtract the defaults calculated above
				$cSet = [$cSet instances Component]
				$cSet = [$cSet - $folderDefaults]

				# subtract off those currently in the configuration,
				# and those that are unavailable
				$cSet = [$cSet - $wccList]
				$cSet = [$cSet - $unavailable]
				set folderNoDefaults [$cSet instances]

				# Convert all our ids to friendly names
				$cSet = $folderNoDefaults
				set folderNoDefaults [[namespace parent]::idToName $cSet]
				$cSet = $folderDefaults
				set folderDefaults [[namespace parent]::idToName $cSet]

				if {($folderNoDefaults == "") && ($folderDefaults == "")} {
					messageBox -ok "No installed components in this folder"
					set bCancel 1
				} else {
					# Show the folder dialog to the user and
					# allow overrides
					${ns2}::folderDlgShow add $folderName \
						$folderNoDefaults $folderDefaults

					set componentList ""
					if {[string match [set ${ns2}::_retCode] ok]} {
						set componentList [set ${ns2}::_componentList]
					} else {
						set bCancel 1 
					}
				}

				# Check dependencies on user's component(s); show dependencies 
				# to user

				if {!$bCancel} {
					set friendlyComponents $componentList

					# Convert from friendly names back to IDs
					set componentList [[namespace parent]::nameToId \
						$componentList]

					# Get dependencies
					set cSet2 [$cSet setCreate]
					$cSet = $componentList
					$cSet2 = $wccList
					set depList [lsort [cxrDependsGet $cSet $cSet2 $mDoc]]
					$cSet2 delete

					# Convert the dependencies back to friendly names
					if {$depList != ""} {
						$cSet = $depList
						set friendlyDeps [[namespace parent]::idToName $cSet]
					} else {
						set friendlyDeps ""
					}
				
					# Calculate the delta and new image size
					set oldSet $wccList
					$cSet = $wccList
					$cSet = [$cSet + $componentList]
					$cSet = [$cSet + $depList]
					set neededComponents [$cSet instances]
					set sizeData [[namespace parent]::sizeInfoGet \
						$cSet $mDoc $oldSet $neededComponents]

					# Show the friendly names and dependencies to the user
					${ns}::depDlgShow add $friendlyComponents $friendlyDeps \
						"" "" [lindex $sizeData 0] [lindex $sizeData 1]

					if {[string match [set ${ns}::_retCode] ok]} {
						# Merge the components and dependencies into one list
						set componentList [concat $componentList $depList]

						# Add the component(s)
						if {$componentList != ""} {
							::Workspace::componentAdd ::ComponentViewServer \
								$prjName $cSetName $componentList
						}
					}
				}

				$cSet delete
			} else {
				set bError 1
			}
		} else {
			set bError 1
		}
		::endWaitCursor

		if {$bError} {
			messageBox -ok -exclamationicon $error
		}
	}

	proc OnComponentDelete {event handle} {
		::beginWaitCursor

		set componentName [::Object::evaluate $handle Name]
		set cSetName [::Object::evaluate $handle ComponentSet]
		set projObj [::Object::evaluate $handle Project]
		set prjName [::Object::evaluate $projObj Name]
		set hProj [::Object::evaluate $projObj Handle]
		set wccList [::wccComponentListGet $hProj $cSetName] 

		set ns ::ComponentViewServer::gui::AddCmpDlg
		set ns2 ::ComponentViewServer::gui::DepDlg

		# Determine components using component
		set bError 0
		if {![catch {set cDoc [::Workspace::cxrDocGet $prjName]} error]} {
			if {![catch {set mDoc [::Workspace::mxrDocGet  $prjName]} error]} {
				set cSet [$cDoc setCreate]
				$cSet = $componentName
				$cSet = [::cxrSupertree $cSet $mDoc $wccList]

				# Remove componentName from the dependency list
				$cSet = [$cSet - $componentName]
				$cSet = [$cSet & $wccList] 
				set depList [$cSet instances Component]

				# Convert to friendly names
				$cSet = $depList
				set friendlyDepList [[namespace parent]::idToName $cSet]
				$cSet = $componentName
				set friendlyComponentName [[namespace parent]::idToName $cSet]

				# Calculate the delta and new image size
				set oldSet $wccList
				$cSet = $wccList
				$cSet = [$cSet - $componentName]
				$cSet = [$cSet - $depList]
				set neededComponents [$cSet instances]
				set sizeData [[namespace parent]::sizeInfoGet \
					$cSet $mDoc $oldSet $neededComponents]

				# Show the friendly names and dependencies to the user
				${ns2}::depDlgShow remove "" "" \
					$friendlyComponentName $friendlyDepList \
					[lindex $sizeData 0] [lindex $sizeData 1]


				# Delete the component(s)
				if {[string match [set ${ns2}::_retCode] ok]} {
					# Convert back to IDs
					set componentList [concat $depList $componentName]
					::Workspace::componentDelete ::ComponentViewServer \
						$prjName $cSetName $componentList
				}
				$cSet delete
			} else {
				set bError 1
			}
		} else {
			set bError 1
		}
		::endWaitCursor

		if {$bError} {
			messageBox -ok -exclamationicon $error
		}
	}

	proc OnFolderDelete {event handle} {
		::beginWaitCursor

		set ns ::ComponentViewServer::gui::FolderDlg
		set folderName [::Object::evaluate $handle Name]
		set cSetName [::Object::evaluate $handle ComponentSet]
		set projObj [::Object::evaluate $handle Project]
		set prjName [::Object::evaluate $projObj Name]

		# Determine the folder contents
		set bError 0
		if {![catch {set cDoc [::Workspace::cxrDocGet $prjName]} error]} {
			if {![catch {set mDoc [::Workspace::mxrDocGet  $prjName]} error]} {
				set cSet [$cDoc setCreate]
				set hProj [::Object::evaluate $projObj Handle]
				set wccList [::wccComponentListGet $hProj $cSetName]

				$cSet = $folderName
				set folderContents [::foldersDeletableComponentsGet \
					$cSet $wccList]
				set safeComponents [lindex $folderContents 0] 
				set unsafeComponents [lindex $folderContents 1] 

				# Convert ids to friendly names
				set ns3 ::ComponentViewServer::gui::DepDlg
				$cSet = $safeComponents
				set friendlySafeComponentList \
					[[namespace parent]::idToName $cSet]
				$cSet = $unsafeComponents
				set friendlyUnsafeComponentList \
					[[namespace parent]::idToName $cSet]

				# Show user what the folder contains
				set ${ns}::_currentObject $handle
				set folderName [::Object::evaluate $handle DisplayName]
				${ns}::folderDlgShow remove $folderName \
					$friendlyUnsafeComponentList $friendlySafeComponentList

				# Delete the component(s)
				set friendlyComponentList [set ${ns}::_componentList]
				if {$friendlyComponentList != ""} {

					# Convert the names back to IDs
					set componentList [[namespace parent]::nameToId \
						$friendlyComponentList]

					# Calculate the dependencies
					$cSet = $componentList
					set depList [::cxrSupertree $cSet $mDoc $wccList]
					$cSet = $depList
					$cSet = [$cSet - $componentList]
					$cSet = [$cSet & $wccList]
					set depList [$cSet instances Component]

					# Map the IDs to friendly names
					set depNames [[namespace parent]::idToName $cSet]

					# Calculate the delta and new image size
					set oldSet $wccList
					$cSet = $wccList 
					$cSet = [$cSet - $depList]
					$cSet = [$cSet - $componentList]
					set neededComponents [$cSet instances Component]
					set sizeData [[namespace parent]::sizeInfoGet \
						$cSet $mDoc $oldSet $neededComponents]

					# Show the user the dependency dialog
					set ns ::ComponentViewServer::gui::DepDlg
					$cSet = $componentList
					set componentNames [[namespace parent]::idToName $cSet]
					${ns}::depDlgShow remove "" "" $componentNames $depNames \
						[lindex $sizeData 0] [lindex $sizeData 1]

					$cSet = $componentList
					$cSet = [$cSet + $depList]
					set compPlusDep [$cSet instances Component]
					if {[string match [set ${ns}::_retCode] ok]} {
						::Workspace::componentDelete ::ComponentViewServer \
							$prjName $cSetName $compPlusDep
					}
				}

				# Cleanup
				$cSet delete

			} else {
				set bError 1
			}
		} else {
			set bError 1
		}
		::endWaitCursor

		if {$bError} {
			messageBox -ok -exclamationicon $error
		}
	}

	}

	# End of gui namespace

	#############################################################
	#
	# Workspace extensions -- none.  As a general rule, this
	# code should be added directly to the workspace implementation.
	#
	#############################################################

	namespace eval ::Workspace {
	}

	#############################################################
	#
	# Object wrappers for components, projects, and folders w/in the
	# ComponentViewServer
	# NOTE: the Attributes() proc must be passed a list of flags
	# of interest.  Those that apply will be returned back in
	# a list of {attribute value} pairs,
	# 	eg, {{-bold 1} {-italic 0}}
	#
	#############################################################

	# Returns the original components and all their ancestors
	# in cSet
	proc parentContainersGet {cSet} {
		set retval ""
		set size [$cSet size]
		set parents [$cSet get _CHILDREN]
		$cSet = [$cSet + $parents]
		set newSize [$cSet size]
		while {$size != $newSize} {
			set size $newSize
			set parents [$cSet get _CHILDREN]
			$cSet = [$cSet + $parents]
			set newSize [$cSet size]
		}
		$cSet = [$cSet - FOLDER_ROOT]
	}

	# STRUCTURE -- {hProj cSetName name}
	namespace eval ::ComponentViewServer::ComponentObject {
		proc Type {args} { return componentObject }
		proc bFolder {args} { return 0 }
		proc Name {args} {
			set args [lindex $args 0]
			set retval [lindex $args 2]
			return $retval
		}
		# XXX -- this can be sped up: it references
		# the component library every time now
		proc DisplayName {args} {
			set args [lindex $args 0]
			set prjName [::Workspace::projectNameGet [lindex $args 0]]
			set cDoc [::Workspace::cxrDocGet $prjName]
			set cSet [$cDoc setCreate]
			$cSet = [lindex $args 2] 
			set displayName [$cSet get NAME]
			$cSet delete
			return $displayName
		}
		proc Project {args} {
			set args [lindex $args 0]
			set projObj [::Object::create \
				::Workspace::ProjectObject [lindex $args 0]]
			return $projObj 
		}
		proc ProjectHandle {args} {
			set args [lindex $args 0]
			return [lindex $args 0]
		} 
		proc ComponentSet {args} {
			set args [lindex $args 0]
			set cSetName [lindex $args 1]
			return $cSetName
		}
		proc ComponentSetName {args} {
			set args [lindex $args 0]
			return [lindex $args 1]
		}
		proc FolderName {args} {
			set args [lindex $args 0]
			set prjName [::Workspace::projectNameGet [lindex $args 0]]
			set cDoc [::Workspace::cxrDocGet $prjName]
			set cSet [$cDoc setCreate]
			$cSet = [lindex $args 2]
			set folderName [$cSet get _CHILDREN]
			$cSet delete
			return $folderName
		}
		proc Attributes {args} {
			set flags [lindex $args 1]
			set args [lindex $args 0]
			set hProj [lindex $args 0]
			set id [lindex $args 2]
			set nsp [namespace parent]
			set prjName [::Workspace::projectNameGet $hProj]
			set cDoc [::Workspace::cxrDocGet $prjName]
			set cSet [$cDoc setCreate]

			# Present in config
			set bIn 0
			$cSet = $id
			set componentList [::wccComponentListGet \
			$hProj [::wccCurrentGet $hProj]]
			$cSet = [$cSet & $componentList] 
			if {[$cSet instances Component] != ""} {
				set bIn 1
			}

			# Not available (not purchased)
			set bNA 0
			$cSet = $id
			set unavailable \
				[[namespace parent]::unavailableListGet $hProj]
			$cSet = [$cSet & $unavailable] 
			if {[$cSet instances Component] != ""} {
				set bNA 1
			}

			foreach flag $flags {
				set flag [string range $flag 1 end]
				switch -exact $flag {
					icon {
						if {[${nsp}::bError $hProj $id]} {
							lappend retval {-icon IDB_COMPONENTERROR}
						} else {
							lappend retval {-icon IDB_COMPONENT}
						}
					}
					bold {
						# Present in config
						if {$bIn} {
							lappend retval {-bold 1}
						} else {
							lappend retval {-bold 0}
						}
					}
					italic {
						# Not available (not purchased)
						if {$bNA} {
							lappend retval {-italic 1}
						} else {
							lappend retval {-italic 0}
						}
					}
					color {
						# Red for error; Green for removable;
						# Green trumps Red
						set nsp [namespace parent]
						set ns ::ViewBrowser
						if {[${nsp}::bSelect $prjName $id]} {
							lappend retval [list -color \
								[appRegistryEntryRead -default \
								"0 128 0 255 255 255" Colors ConfigHighlight]]
						} elseif {[${nsp}::bError $hProj $id]} {
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

			$cSet delete

			if {![info exists retval]} {
				set retval ""
			}
			return $retval 
		}

		#
		# Private APIs
		#

		# Is component in current configuration?
		proc bIncluded {args} {
			set args [lindex $args 0] 

			set retval 0
			set hProj [lindex $args 0]
			set id [lindex $args 2]
			set nsp [namespace parent]
			set prjName [::Workspace::projectNameGet $hProj]
			set cDoc [::Workspace::cxrDocGet $prjName]
			set cSet [$cDoc setCreate]
			$cSet = $id 
			set componentList [::wccComponentListGet \
				$hProj [::wccCurrentGet $hProj]]
			$cSet = [$cSet & $componentList] 
			if {[$cSet instances Component] != ""} {
				set retval 1
			}
			$cSet delete
			return $retval
		}

		# Is component available (is it, or does it depend on,
		# a product that has yet to be purchased?)
		proc bAvailable {args} {
			set args [lindex $args 0] 
			set retval 1
			set hProj [lindex $args 0]
			set id [lindex $args 2]
			set nsp [namespace parent]
			set prjName [::Workspace::projectNameGet $hProj]
			set cDoc [::Workspace::cxrDocGet $prjName]
			set cSet [$cDoc setCreate]
			$cSet = $id
			set unavailable \
				[[namespace parent]::unavailableListGet $hProj]
			$cSet = [$cSet & $unavailable] 
			if {[$cSet instances Component] != ""} {
				set retval 0
			}
			$cSet delete
			return $retval 
		}
	}

	# STRUCTURE -- {hProj cSetName name}
	namespace eval ::ComponentViewServer::RoutineObject {
		proc Type {args} { return routineObject }
		proc bFolder {args} { return 0 }
		proc Name {args} {
			set args [lindex $args 0]
			set retval [lindex $args 2]
			return $retval
		}
		# XXX -- this can be sped up: it references
		# the component library every time now
		proc DisplayName {args} {
			set args [lindex $args 0]
			set displayName [lindex $args 2]
			return $displayName 
		}
		proc Project {args} {
			set args [lindex $args 0]
			set projObj [::Object::create \
				::Workspace::ProjectObject [lindex $args 0]]
			return $projObj 
		}
		proc ProjectHandle {args} {
			set args [lindex $args 0]
			return [lindex $args 0]
		} 
		proc ComponentSet {args} {
			set args [lindex $args 0]
			set cSetName [lindex $args 1]
			return $cSetName
		}
		proc FolderName {args} {
			set args [lindex $args 0]
			set prjName [::Workspace::projectNameGet [lindex $args 0]]
			set cDoc [::Workspace::cxrDocGet $prjName]
			set cSet [$cDoc setCreate]
			$cSet = [lindex $args 2]
			set folderName [$cSet get _CHILDREN]
			$cSet delete
			return $folderName
		}
		proc Attributes {args} {
			set flags [lindex $args 1]
			set args [lindex $args 0]
			set hProj [lindex $args 0]
			set id [lindex $args 2]
			set nsp [namespace parent]

			foreach flag $flags {
				set flag [string range $flag 1 end]
				switch -exact $flag {
					icon {
						if {[${nsp}::bError $hProj $id]} {
							lappend retval {-icon IDB_ROUTINE}
						} else {
							lappend retval {-icon IDB_ROUTINE}
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

	# STRUCTURE -- {hProj cSetName name}
	namespace eval ::ComponentViewServer::ComponentFolderObject {
		proc Type {args} { return componentFolderObject } 
		proc bFolder {args} { return 1 }
		proc Name {args} {
			set args [lindex $args 0]
			set retval [lindex $args 2]
			return $retval
		}
		# XXX -- this can be sped up: it references
		# the component library every time now
		proc DisplayName {args} {
			set args [lindex $args 0]
			set prjName [::Workspace::projectNameGet [lindex $args 0]]
			set cDoc [::Workspace::cxrDocGet $prjName]
			set cSet [$cDoc setCreate]
			$cSet = [lindex $args 2] 
			set displayName [$cSet get NAME]
			$cSet delete
			return $displayName
		}
		proc Project {args} {
			set args [lindex $args 0]
			set projObj [::Object::create \
				::Workspace::ProjectObject [lindex $args 0]]
			return $projObj 
		}
		proc ProjectHandle {args} {
			set args [lindex $args 0]
			return [lindex $args 0]
		} 
		proc ComponentSet {args} {
			set args [lindex $args 0]
			set cSetName [lindex $args 1]
			return $cSetName
		}
		proc ComponentSetName {args} {
			set args [lindex $args 0]
			return [lindex $args 1]
		}
		proc FolderName {args} {
			set args [lindex $args 0]
			set prjName [::Workspace::projectNameGet [lindex $args 0]]
			set cDoc [::Workspace::cxrDocGet $prjName]
			set cSet [$cDoc setCreate]
			$cSet = [lindex $args 2]
			set folderName [$cSet get _CHILDREN]
			$cSet delete
			return $folderName
		}
		proc Attributes {args} {
			set flags [lindex $args 1]
			set args [lindex $args 0]
			set prjName [::Workspace::projectNameGet [lindex $args 0]]
			set cDoc [::Workspace::cxrDocGet $prjName]
			set cSet [$cDoc setCreate]
			set hProj [lindex $args 0]
			set id [lindex $args 2]
			set nsp [namespace parent]

			# Present in config
			set bIn 0
			$cSet = [::wccComponentListGet \
				$hProj [::wccCurrentGet $hProj]]
			[namespace parent]::parentContainersGet $cSet
			$cSet = [$cSet instances Folder]
			$cSet = [$cSet & $id] 
			if {[$cSet instances] != ""} {
				set bIn 1
			}

			foreach flag $flags {
				set flag [string range $flag 1 end]
				switch -exact $flag {
					icon {
						if {[${nsp}::bError $hProj $id]} {
							lappend retval {-icon IDB_COMPONENTFOLDER}
						} else {
							lappend retval {-icon IDB_COMPONENTFOLDER}
						}
					}
					color {
						# Red for error; Green for removable
						# Green trumps Red
						set nsp [namespace parent]
						if {[${nsp}::bSelect $prjName $id]} {
							lappend retval [list -color \
								[appRegistryEntryRead -default \
								"0 128 0 255 255 255" Colors ConfigHighlight]]
						} elseif {[${nsp}::bError $hProj $id]} {
							lappend retval [list -color \
								[appRegistryEntryRead -default \
								"255 0 0 255 255 255" Colors ConfigError]]
						} else {
							lappend retval [list -color \
								[appRegistryEntryRead -default \
								"0 0 0 255 255 255" Colors ConfigNormal]]
						}
					}
					bold {
						# Present in config
						if {$bIn} {
							lappend retval {-bold 1}
						} else {
							lappend retval {-bold 0}
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

		#
		# Private APIs
		#

		# Does folder contain any children in the current configuration?
		proc bIncluded {args} {
			set args [lindex $args 0] 

			set retval 0
			set hProj [lindex $args 0]
			set id [lindex $args 2]
			set nsp [namespace parent]
			set prjName [::Workspace::projectNameGet $hProj] 
			set cDoc [::Workspace::cxrDocGet $prjName]
			set cSet [$cDoc setCreate]
			$cSet = $id 
			$cSet = [::cxrSubfolders $cSet]
			set componentList [::wccComponentListGet \
				$hProj [::wccCurrentGet $hProj]]
			$cSet = [$cSet & $componentList] 
			if {[$cSet instances Component] != ""} {
				set retval 1
			}
			$cSet delete
			return $retval
		}

		# Does folder contain *all* children in the current configuration?
		proc bAllIncluded {args} {
			set args [lindex $args 0] 

			set retval 1
			set hProj [lindex $args 0]
			set id [lindex $args 2]
			set nsp [namespace parent]
			set prjName [::Workspace::projectNameGet $hProj]
			set cDoc [::Workspace::cxrDocGet $prjName]
			set cSet [$cDoc setCreate]
			$cSet = $id 
			$cSet = [::cxrSubfolders $cSet]
			set componentList [::wccComponentListGet \
				$hProj [::wccCurrentGet $hProj]]
			$cSet = [$cSet - $componentList] 
			if {[$cSet instances Component] != ""} {
				set retval 0
			}
			$cSet delete
			return $retval
		}
	}

	# STRUCTURE -- {hProj cSetName name}
	namespace eval ::ComponentViewServer::SelectionObject {
		proc Type {args} { return selectionObject }
		proc bFolder {args} { return 1 }
		proc Name {args} {
			set args [lindex $args 0]
			set retval [lindex $args 2]
			return $retval
		}
		# XXX -- this can be sped up: it references
		# the component library every time now
		proc DisplayName {args} {
			set args [lindex $args 0]
			set prjName [::Workspace::projectNameGet [lindex $args 0]]
			set cDoc [::Workspace::cxrDocGet $prjName]
			set cSet [$cDoc setCreate]
			$cSet = [lindex $args 2] 
			set displayName [$cSet get NAME]
			$cSet delete
			return $displayName
		}
		proc Project {args} {
			set args [lindex $args 0]
			set projObj [::Object::create \
				::Workspace::ProjectObject [lindex $args 0]]
			return $projObj 
		}
		proc ProjectHandle {args} {
			set args [lindex $args 0]
			return [lindex $args 0]
		} 
		proc ComponentSet {args} {
			set args [lindex $args 0]
			set cSetName [lindex $args 1]
			return $cSetName
		}
		proc ComponentSetName {args} {
			set args [lindex $args 0]
			return [lindex $args 1]
		}
		proc FolderName {args} {
			set args [lindex $args 0]
			set prjName [::Workspace::projectNameGet [lindex $args 0]]
			set cDoc [::Workspace::cxrDocGet $prjName]
			set cSet [$cDoc setCreate]
			$cSet = [lindex $args 2]
			set folderName [$cSet get _CHILDREN]
			$cSet delete
			return $folderName
		}
		proc Attributes {args} {
			set flags [lindex $args 1]
			set args [lindex $args 0]
			set prjName [::Workspace::projectNameGet [lindex $args 0]]
			set cDoc [::Workspace::cxrDocGet $prjName]
			set cSet [$cDoc setCreate]
			set hProj [lindex $args 0]
			set id [lindex $args 2]
			set nsp [namespace parent]

			foreach flag $flags {
				set flag [string range $flag 1 end]
				switch -exact $flag {
					icon {
						if {[${nsp}::bError $hProj $id]} {
							lappend retval {-icon IDB_SELECTIONERROR}
						} else {
							lappend retval {-icon IDB_SELECTION}
						}
					}
					bold {
						# Present in config
						if {[bIncluded $args]} {
							lappend retval {-bold 1}
						} else {
							lappend retval {-bold 0}
						}
					}
					color {
						# Red for error, green for selection
						set nsp [namespace parent]
						if {[${nsp}::bSelect $prjName $id]} {
							lappend retval [list -color \
								[appRegistryEntryRead -default \
								"0 128 0 255 255 255" Colors ConfigHighlight]]
						} elseif {[${nsp}::bError $hProj $id]} {
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

			$cSet delete
			if {![info exists retval]} {
				set retval ""
			}
			return $retval 
		}

		#
		# Private APIs
		#

		# If any component in the configuration belongs to a selection, 
		# or if any component requires our selection itself, then the
		# selection is in 
		proc bIncluded {args} {
			set args [lindex $args 0] 

			set retval 0
			set hProj [lindex $args 0]
			set id [lindex $args 2]
			set nsp [namespace parent]
			set prjName [::Workspace::projectNameGet $hProj]
			set cDoc [::Workspace::cxrDocGet $prjName]
			set cSet [$cDoc setCreate]

			$cSet = [::wccComponentListGet $hProj [::wccCurrentGet $hProj]]
			set requires [$cSet get REQUIRES]
			set parents [$cSet get _CHILDREN]
			$cSet = $requires
			$cSet = [$cSet + $parents]
			$cSet = [$cSet & $id] 
			if {[$cSet instances Selection] != ""} {
				set retval 1
			}
			$cSet delete
			return $retval
		}
	}

	#############################################################
	#
	# Register ourselves as an add-in.  This must occur last.
	#
	#############################################################

	::Workspace::addinRegister ::ComponentViewServer
}
