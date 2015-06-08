# ViewBrowser.tcl - gui portion of the project workspace portion of
# the Tornado project facility
#
# modification history
# --------------------
# 04a,08apr99,j_k  Remove call to controlLockUpdate. it is not needed. it
#                  causes desktop to flash and it does not have a
#                  corresponding controlUnlockUpdate.
# 03z,17mar99,ren  Garbage spews to stdout after closing a project on UNIX
# 03y,10mar99,ren  Puts internal tcl errors when menu match fails.
# 03x,03mar99,cjs  Adding workspace icon
# 03w,25feb99,rbl  Eliminate any double separators in ctxMenuShow
# 03v,08feb99,rbl  Changes to handle missing or read-only projects
# 03v,17feb99,cjs  Adding support for new icons
# 03u,11nov98,cjs  call OnSize() immediately after creating VB to get
#                  it laid-out propertly
# 03t,12sep98,cjs  Re-introduced a change that was somehow lost when we
#                  moved to the release branch 
# 03s,05sep98,cjs  Combined all controlItemPropertySet()s for efficiency;
#                  added logic to set null object as current selection
#                  in OnWatch()
# 03q,30sep98,ren  Now the project facil window will never open taller
#                  than client area of the parent window.
# 03p,30sep98,cjs  Fix pathShow() to work with multiple paths 
# 03o,30sep98,cjs  Fixed bug preventing conversion of '+' to '-'
#                  when folder node is empty.
# 03n,29sep98,ren  Fixed little left over problem with double clicking.
# 03m,28sep98,ren  The tree-control now correctly expands when you click
#                  On the plus sign.
# 03l,24sep98,ren  Use windowLockUpdate to avoid tree control flicker.
# 03k,24sep98,cjs  Handle "expanding" event -- still need more help from tree
#                  ctrl to get it right
# 03j,22sep98,ren  Fixed failure in OnSize when the window doesn't exist.
# 03i,21sep98,cjs  catching workspaceSave() errors 
# 03h,18sep98,cjs  fixed pathShow() to expand and select a node;
#                  added viewBrowserTitleSet()
# 03g,18sep98,cjs  use -noexpand flag for folder creation 
# 03f,15sep98,cjs  changed startup sequence of view browser; call
#                  preferredPageSort() to order VB panes
# 03e,10sep98,cjs  don't muck w/ tree control background color unless user
#                  has explicitly set it 
# 03d,09sep98,cjs  updated color logic for tree nodes
# 03c,04sep98,cjs  add treeNodeStateColorGet() function; altered
#                  startup sequence to fix bug in unix version
# 03b,19aug98,aam  Fix under unix 
# 03a,19aug98,cjs  View Browser and Workspace now work more like
#                  other addins 
# 02g,17aug98,cjs  OnWatch() now fakes a selection change event when
#                  a view has no contents; onTreeClick() now checks
#                  to see if there is no selection and sets the null
#                  object in this case
# 02f,13aug98,cjs  add window sizing coordination to support window
#                  resize; fixed bug in initial tree selection
# 02e,11aug98,cjs  sort and improve speed for addition of new nodes
# 02d,04aug98,cjs  make sure there is a current selection following
#                  OnWatch() 
# 02c,04aug98,cjs  added pathShow() 
# 02b,31jul98,cjs  added build combo box support
# 02a,29jul98,cjs  Got rid of iHierarchy, added demand drilling
#                  and so forth.
# 01f,07jul98,cjs  change all windowShow()s to windowHide().  Changed
#                  initial window size
# 01e,26jun98,cjs  hide vb window instead of closing it on viewBrowserClose 
# 01d,23jun98,cjs  use controlSelectionGet -string for tab control 
# 01g,07may98,cjs  check for support of object type before looking for
#			       event handlers 
# 01f,01may98,cjs  updated for use with new tree control
# 01d,21apr98,cjs  modified startup logic -- now use viewBrowserCreate()
#                  and viewBrowserInit()
# 01c,30mar98,cjs  reinitialize _tabsheets when browser is shut down.
# 01b,27mar98,cjs  changed references to className(), objectType()
#                  to classTitle(), className(), respectively
# 01a,27mar98,cjs  written.
#
# DESCRIPTION
# This file implements the visible portion of the workspace --
# a window that contains a tabbed pane for each server.  It
# manages context menus for each of the panes as well.
#
#############################################################
namespace eval ::ViewBrowser {

	variable _connections
	variable _tabsheets
	variable _currentTab "Files"
	variable _ctrlNameList

	# Initial window and widget geometries
	variable _windowName ::ViewBrowser::vbWindow
	variable _selfXPos 0
	variable _selfYPos 0
	variable _selfWidth 180
	variable _selfHeight 240
	
	variable _treeviewXPos 8
	variable _treeviewYPos 28
	variable _treeviewWidth 164
	variable _treeviewHeight 184

	variable _tabsheetXPos 4
	variable _tabsheetYPos 22
	variable _tabsheetWidth 172
	variable _tabsheetHeight 210

	# set hideOnClose to 0 to prevent the window from being
	# hidden when viewBrowserClose() is called 
	variable _hideOnClose 1

	# Registered menus.  There are 4 types: Workspace, WorkspaceShared,
	# Server, and Object.  We track 3 of the 4 types: WorkspaceShared,
	# Server, and Object.
	variable _menuServers
	variable _projectWizardList
	variable _eventHandlers
	variable _currentMenu
	variable _bMenuAdded
	variable _iconNames { \
		IDB_FILE IDB_DEPEND IDB_DERIVED IDB_COMPONENT \
		IDB_SELECTION IDB_BUILD IDB_FOLDER \
		IDB_ROUTINE IDB_COMPONENTFOLDER IDB_COMPONENTSET \
		IDB_SELECTIONERROR IDB_COMPONENTERROR \
		IDB_BOOTABLE IDB_DOWNLOADABLE IDB_BOOTROM IDB_DEADPROJECT \
		IDB_WORKSPACE \
	}

	######################################################################
	#
	# Externally visible View Browser (workspace viewer) API
	#
	######################################################################


	###########################################################################
	#
	# viewBrowserCreate - creates a view browser window and prepares to catch
	# events.
	# NOTE: 1) ::Workspace::serversConnect must be called first
	#		2) viewBrowserInit should be called next	
	#
	proc viewBrowserCreate {} {

		# Create the view browser window 
		if {![viewBrowserIsVisible]} {
			variable _selfXPos
			variable _selfYPos
			variable _selfHeight
			variable _selfWidth

			set maxHeight [lindex [windowClientSizeGet mainwindow] 1]
			if { $_selfHeight > $maxHeight } {
				set _selfHeight $maxHeight
			}
			
			windowCreate -name $::ViewBrowser::_windowName \
				-title "(No open workspace)"	\
				-nonewinterp \
				-x $_selfXPos \
				-y $_selfYPos \
				-width $_selfWidth \
				-height $_selfHeight

			windowQueryCloseCallbackSet $::ViewBrowser::_windowName \
				::ViewBrowser::queryClose

			windowSizeCallbackSet $::ViewBrowser::_windowName \
				::ViewBrowser::OnSize

			set bWindowCreated 1
		} else {
			windowHide $::ViewBrowser::_windowName 0
		}

		# If we have any panes at this point, then we don't want to
		# muck with the current tab selection.  Obtain the current
		# selection if the tab exists and save for later use
		set currentSelection ""
		if {[controlExists ${::ViewBrowser::_windowName}.tabctrl]} {
			set currentSelection [controlSelectionGet \
				$::ViewBrowser::_windowName.tabctrl -string]
		} else {
			# Create the view browser tab window, if it doesn't exist 

			variable _tabsheetXPos
			variable _tabsheetYPos
			variable _tabsheetWidth
			variable _tabsheetHeight
		
			controlCreate $::ViewBrowser::_windowName \
				[list tab -name tabctrl \
				-callback ::ViewBrowser::OnTabClicked \
				-multiline -bottom \
				-xpos $_tabsheetXPos \
				-ypos $_tabsheetYPos \
				-width $_tabsheetWidth \
				-height $_tabsheetHeight]

			# Connect to and create sheets for view servers if we are not
			# currently connected.  Use a specific order for the panes
			set preferredList {Files VxWorks Builds}
			set panes [::preferredPageSort $preferredList \
				[array names ::ViewBrowser::_connections]]

			foreach pane $panes { 
				set server $::ViewBrowser::_connections($pane)

				# Create the tab sheet
				sheetCreate $::ViewBrowser::_windowName $pane
			}
		}

		# If all the tabs are new, then trigger the callback to hide and
		# show the appropriate controls
		if {$currentSelection == ""} {
			controlSelectionSet $::ViewBrowser::_windowName.tabctrl 0

			# Do the following to overcome a UITcl bug:
			# controlSelectionSet doesn't generate an event 
			set ::ViewBrowser::_currentTab \
				[controlSelectionGet $::ViewBrowser::_windowName.tabctrl \
				-string]
			OnTabClicked
		}

		# Call OnSize to get the controls laid out properly relative
		# to the window
		OnSize
	}

	# Called after workspace is open
	proc viewBrowserInit {} {

		# Write the currently open workspace to the
		# registry.  Put the most recent at the head of the list
		set mruEntries [::WelcomeDlg::mruEntriesRead]
		set idx [lsearch $mruEntries [::Workspace::openWorkspaceGet]]
		if {$idx != -1} {
			set mruEntries \
				[lreplace $mruEntries $idx $idx]
		}
		set mruEntries \
			[linsert $mruEntries 0 [::Workspace::openWorkspaceGet]]
		# Save only 5
		set mruEntries \
			[lrange $mruEntries 0 5] 
		::WelcomeDlg::mruEntriesWrite $mruEntries
	}

	# Used to indicate the open workspace and its dirty state
	proc viewBrowserTitleSet {caption} {
		windowTitleSet $::ViewBrowser::_windowName $caption
	}

	proc viewBrowserClose {} {

		# Clean up all the connections
		foreach tab [array names ::ViewBrowser::_connections] {
			# Destroy the controls
			catch {controlsDestroy $::ViewBrowser::_windowName $tab}
		}

		catch {controlDestroy $::ViewBrowser::_windowName.tabctrl}
		catch {unset ::ViewBrowser::_tabsheets}
		catch {windowHide $::ViewBrowser::_windowName 1}
	}

	# Returns 1 if the view browser window has been created,
	# 0 if not.
	proc viewBrowserIsVisible {} {
		return [windowExists $::ViewBrowser::_windowName]
	}

	# queryClose -- view browser window exit callback
	proc queryClose {} {
		set retval [querySave]
		if {$retval} {
			::Workspace::workspaceClose
		}
		return $retval
	}

	# querySave -- called to see if workspace is dirty when the user 
	# requests that we open another workspace
	proc querySave {} {
		if {[::Workspace::workspaceDirtyGet]} {
			set msg "Workspace has changed.  Save?"
			set status [messageBox -yesnocancel -questionicon $msg]
			switch $status {
				yes {
					::beginWaitCursor
					set retval 1
					if {[catch {::ProjectFacility::guiWorkspaceSave} error]} {
						messageBox -ok $error
						set retval 0
					# ::ProjectFacility::guiWorkspaceSave returns blank fileName on a cancel
					} elseif {$error == ""} {
						set retval 0
					}

					::endWaitCursor
				}
				no {
					::beginWaitCursor
					set retval 1
					::endWaitCursor
				}
				cancel {
					set retval 0
				}
			}
		} else {
			::beginWaitCursor
			set retval 1
			::endWaitCursor
		}
		return $retval 
	}

	proc OnSize {} {
		variable _selfWidth
		variable _selfHeight
		variable _tabsheetWidth
		variable _tabsheetHeight
		variable _treeviewWidth
		variable _treeviewHeight
		
		if {[viewBrowserIsVisible]} {
			set size [windowClientSizeGet $::ViewBrowser::_windowName]
			set _selfWidth [lindex $size 0]
			set _selfHeight [lindex $size 1]
			
			set _tabsheetWidth [expr $_selfWidth - 8]
			set _tabsheetHeight [expr $_selfHeight - 24]
			controlSizeSet $::ViewBrowser::_windowName.tabctrl \
			$_tabsheetWidth $_tabsheetHeight
			
			# Adjust the combo box
			set sizeLbl [controlSizeGet \
							 $::ViewBrowser::_windowName.lblactivebuild]
			set sizeCombo [controlSizeGet \
							   $::ViewBrowser::_windowName.comboactivebuild]
			controlSizeSet $::ViewBrowser::_windowName.comboactivebuild \
			[expr $_tabsheetWidth - [lindex $sizeLbl 0]] [lindex $sizeCombo 1]
			
			set _treeviewWidth [expr $_tabsheetWidth - 8]
			set _treeviewHeight [expr $_tabsheetHeight - 22] 
			foreach tab [array names ::ViewBrowser::_connections] {
				set control [format "%s.%sTree" \
								 $::ViewBrowser::_windowName $tab] 
				controlSizeSet $control \
				$_treeviewWidth $_treeviewHeight 
			}
		}
	}

	# Register namespace as a server supporting menuMatch
	# objHandleList is a list of objects to be registered
	# so that the top level menu system can enumerate object
	# types and organize and display menus for them
	proc menuServerRegister {namespace objHandleList} {
		set ifaceList {menuMatch}
		if {[::ifaceValidate $namespace $ifaceList]} {
			lappend ::ViewBrowser::_menuServers $namespace
		} else {
			error "Menu Server '$namespace' must support ($ifaceList)"
		}

		# Make objects this server knows about known to the system
		if {$objHandleList != ""} {
			foreach objHandle $objHandleList {
				lappend ::ViewBrowser::_objTypeHandleList $objHandle	
			}
		}
	}

	# Register a wizard so that we can map a project type to the
	# wizard namespace from which we should launch projectWizardRun() 
	proc projectWizardRegister {description namespace} {
		variable _projectWizardList
		set _projectWizardList($description) $namespace
	}

	proc projectWizardListGet {} {
		variable _projectWizardList
		if {[info exists _projectWizardList]} {
			set retval [array names _projectWizardList]
		} else {
			set retval ""
		}
		return $retval
	}

	proc projectWizardGet {description} {
		variable _projectWizardList
		if {[info exists _projectWizardList($description)]} {
			set retval $_projectWizardList($description)
		} else {
			set retval ""
		} 
		return $retval 
	}

	# Allows a view server to indicate that it can handle a
	# gui event (eg, dblclk) for a particular object type
	proc guiEventHandlerRegister {event objectType callback} {
		if {[llength [info args $callback]] == 2} {
			lappend ::ViewBrowser::_eventHandlers($objectType) \
				[list $event $callback]
		} else {
			set error "Error: event handler callback '$callback'"
			set error "$error has wrong signature"
			error $error
		}
	}

	# Called back by server client when hit with a menuMatch()
	proc menuAdd {text callback {bDisable 0}} {
		variable _bMenuAdded
		menuItemAppend $::ViewBrowser::_currentMenu -callback $callback $text
		menuItemEnable $::ViewBrowser::_currentMenu \
			-string $text [expr !$bDisable]
		set _bMenuAdded 1
	}

	proc menuServersGet {} {
		return $::ViewBrowser::_menuServers
	}

	proc iconNamesGet {} {
		variable _iconNames
		return $_iconNames
	}

	proc iconIndexLookup {iconName} {
		variable _iconNames
		set idx [lsearch $_iconNames $iconName]
		return $idx
	}

	proc flatJoin {list1 list2} {
		if {[lindex $list1 0] == {}} {
			set retval $list2
		} elseif {[lindex $list2 0] == {}} {
			set retval $list1
		} else {
			set retval $list1
			foreach elem $list2 {
				lappend retval $elem
			}
		}

		return $retval
	}
	
	proc listify {cSet} {
		if {[$cSet size] == 1} {
			lappend retval [$cSet contents]
		} else {
			set retval [$cSet contents]
		}
		return $retval
	}

	######################################################################
	#
	# OnWatch - based on path of cookies passed to us, query server
	# and re-populate child entries below each path from tree control.
	# 
	proc OnWatch {guiServer cookieLists} {
		::beginWaitCursor

		set pane [serverToPane $guiServer]
		if {$pane == ""} {
			error "Server '$guiServer' not connected to view browser"
		}

		set control $::ViewBrowser::_windowName.[set pane]Tree

		# Update/Populate the tree
		catch { recursiveOnWatch $guiServer $cookieLists }

		# If there is no current selection in the tree, make one
		set control [format "%s.%sTree" $::ViewBrowser::_windowName $pane] 
		if {[controlSelectionGet $control] == ""} {
			set rootItems [controlValuesGet $control -childrenonly ""]
			set rootItems [lindex $rootItems 0]
			if {[llength $rootItems] > 0} {
				controlSelectionSet $control [lindex $rootItems 0]
			} else {
				# In some cases, onTreeClick will do this for us
				::Workspace::selectionSet $::Workspace::_NULL_OBJECT
			}
		}
		::endWaitCursor
	}

	proc recursiveOnWatch {guiServer cookieLists} {
		variable _connections
	
		# Map namespace to tab
		set pane [serverToPane $guiServer]

		set control [format "%s.%sTree" $::ViewBrowser::_windowName $pane] 

		# We only need to fetch the children of a node and merge them
		# into the tree if the tree is currently expanded deep enough
		# to make that node's children visible

		# For each path in the list of paths, walk the segments of the
		# path, bailing if we find that a node has not yet been created.
		# If we make it all the way through the path, then we know we
		# need to get the the tree's current contents for the node,
		# fetch the server's version, and merge the two
		set server [namespace parent $guiServer]

		# Create and cache a set that we can use for
		# generic set operations
		set cDoc [::docCreate CxrDoc] 
		set cSet [$cDoc setCreate] 

		foreach cookieList $cookieLists {

			# Determine if the segment is already in the tree
			set treeChildren \
				[controlValuesGet $control -childrenonly $cookieList]

			# Remove extra outer parens
			set treeChildren [lindex $treeChildren 0]

			# The tree control doesn't throw exceptions, so we must
			# match against return value containing "invalid" 
			if {[string match "invalid*" $treeChildren]} {
				# It wasn't found, bail 
				set bExpanded 0
			} else {
				set bExpanded 1
			}

			# If the path is expanded, then do the merge
			if {$bExpanded} {

				# The tree children we get from the control don't have
				# full paths; fix that
				set tmp ""	
				foreach t $treeChildren {
					if {$t != ""} {
						lappend tmp [concat $cookieList $t]
					}
				}
				if {[info exists tmp]} {
					set treeChildren $tmp
				} else {
					set treeChildren ""
				}

				# Fetch the server's version of the node's children
				set server [namespace parent $guiServer]
				set serverChildren \
					[${server}::objectsEnumerate \
					$cookieList {-leaves -containers}]

				# Use set operations to obtain those nodes that must
				# be added, deleted, and updated.  Order by leaves,
				# then containers
				$cSet = $serverChildren
				$cSet = [$cSet - $treeChildren]
				set adds [listify $cSet]

				$cSet = $treeChildren
				$cSet = [$cSet - $serverChildren]
				set deletes [listify $cSet]

				$cSet = $serverChildren
				$cSet = [$cSet & $treeChildren]
				set updates [listify $cSet]

				# Add nodes to the tree
				treeMerge $server $control $adds $deletes $updates

				# Recalculate children in the tree
			 	set tmp ""	
				set treeChildren \
					[controlValuesGet $control -childrenonly $cookieList]
				set treeChildren [lindex $treeChildren 0]
                foreach t $treeChildren {
                    if {$t != ""} {
                        lappend tmp [concat $cookieList $t]
                    }
                }
                if {[info exists tmp]} {
                    set treeChildren $tmp
                } else {
                    set treeChildren ""
                }

				# Recurse down and update containers previously in tree
				$cSet = $serverChildren
				$cSet = [$cSet & $treeChildren]
				catch {unset tmp}
				foreach container [listify $cSet] {
					set children \
						[controlValuesGet $control -childrenonly $container]
					set children [lindex $children 0]
					if {$children != ""} {
						lappend tmp $container
					}
				}
				if {[info exists tmp]} {
					OnWatch $guiServer $tmp
				}
			}
		}

		# Cleanup
		if {$cSet != ""} {
			$cSet delete
		}
		if {$cDoc != ""} {
			$cDoc delete
		}
	}

	# Take the list of nodes, determine what's in the tree, and return
	# a list of structure containing {node insertion_cmd}, where insertion_cmd
	# describes what needs to be eval'd to get the node into the proper place
	# in the tree
	#
	proc insertionCmdsGet {server control nodes} {

		set title [${server}::titleGet]

		# Determine the path for items in the tree by extracting it
		# from any of the nodes passed in
		set path [lindex $nodes 0]
		if {[llength $path] >= 2} {
			set path [lrange $path 0 [expr [llength $path] - 2]]
		} else {
			set path ""
		}

		# Get the tree's immediate children and convert to full paths
		set children \
			[controlValuesGet $control -childrenonly $path]
		set children [lindex $children 0]
		foreach child $children {
			lappend tmp [concat $path $child]
		}
		if {[info exists tmp]} {
			set children $tmp
		} else {
			set children ""
		}

		set folders ""
		set leaves ""
		foreach node $nodes {
			set friendlyName [${server}::displayNameGet $title $node]
			if {[${server}::bFolder $node]} {
				set friendlyName [uniqueAppend folders $friendlyName]
			} else {
				set friendlyName [uniqueAppend leaves $friendlyName]
			}
			set map($friendlyName) $node
		}

		foreach node $children {
			set friendlyName [${server}::displayNameGet $title $node]
			if {[${server}::bFolder $node]} {
				set friendlyName [uniqueAppend folders $friendlyName]
			} else {
				set friendlyName [uniqueAppend leaves $friendlyName]
			}
			set map($friendlyName) $node
		}

		if {$folders != ""} {
			set folders [lsort $folders]
		}

		if {$leaves != ""} {
			set leaves [lsort $leaves]
		}

		# Create the sorted lists of nodes
		catch {unset tmp}
		foreach item $folders {
			lappend tmp $map($item)
		}
		if {[info exists tmp]} {
			set folders $tmp
		} else {
			set folders ""
		}

		catch {unset tmp}
		foreach item $leaves {
			lappend tmp $map($item)
		}
		if {[info exists tmp]} {
			set leaves $tmp
		} else {
			set leaves ""
		}

		# Combine the two lists; now we get everything that should
		# be in the tree, and in its correct order
		set combinedList [flatJoin $leaves $folders]

		# Create commands for each 
		set peer [lindex $children 0]
		set mode ""
		foreach node $combinedList {
			set idx [lsearch $children $node]
			if {$idx != -1} {
				# It already exists in the tree, so the next peer we
				# might insert before is the next child
				incr idx
				set peer [lindex $children $idx] 
			} else {
				set nodeToInsert $node
				if {$peer != ""} {
					set mode "-before"
					if {[llength $node] > 1} {
						set nodeToInsert [lindex $node \
							[expr [llength $node] - 1]]
					}
				}
				set cmd [list controlItemInsert $control]
				if {[${server}::bFolder $node]} {
					lappend cmd -makesubtree
					lappend cmd -noexpand
				}
				if {$mode != ""} {
					lappend cmd  $mode
				}
				if {$peer != ""} {
					lappend cmd $peer
				}
				lappend cmd $nodeToInsert
				lappend retval [list $node $cmd]
			}
		}

		if {![info exists retval]} {
			set retval ""
		}
		return $retval
	}

	# A private routine that will mangle a name into unique form,
	# if necessary, before appending it in a list 
	variable index 0
	proc uniqueAppend {list name} {
		variable index
		upvar $list l 
		if {[lsearch -exact $l $name] != -1} {
			set name [format "%s%d" $name $index]
			incr index
		}
		lappend l $name
		return $name
	}

	proc treeMerge {server control adds deletes updates} {

		# Delete the nodes not needed
		foreach del $deletes {
			controlItemDelete $control $del
		}

		set title [${server}::titleGet]

		# Get default colors from the registry
		set color [appRegistryEntryRead -default \
			"0 0 0 255 255 255" Colors ConfigNormal]
		set defaultforeground [lrange $color 0 2]
		set defaultbackground [lrange $color 3 end] 

		# Take the list of adds, sort them, and create a list of structures
		# containing the item and the command needed to add it appropriately 
		set structs [insertionCmdsGet $server $control $adds]

		set parent [lindex $adds 0]
		if {[llength $parent] >= 2} {
			set parent [lrange $parent 0 [expr [llength $parent] - 2]]
		} else {
			set parent ""
		}

		foreach struct $structs {
			set add [lindex $struct 0]
			set cmd [lindex $struct 1]

			set friendlyName [${server}::displayNameGet \
				$title $add]
			set attributeTuples \
				[${server}::attributesGet \
				$add {-bold -italic -icon -color}]
			set icon 0 
			set bBold 0 
			set bItalic 0 
			set foreground $defaultforeground
			set background $defaultbackground
			foreach tuple $attributeTuples {
				set value [lindex $tuple 1]
				switch [string range [lindex $tuple 0] 1 end] {
					icon {
						set icon $value
					}
					bold {
						set bBold $value
					} 
					italic {
						set bItalic $value
					}
					color {
						set foreground [lrange $value 0 2]
						set background [lrange $value 3 end]
					}
				}
			}

			# Create the control 
			eval $cmd

			# Set item's properties in the tree
			if {$icon != 0} {
				set iconIdx [iconIndexLookup $icon]
			} else {
				set iconIdx -1
			}

			# Determine the colors
			set foreground [::colorFromRGBGet $foreground]
			set background [::colorFromRGBGet $background]

			if {$iconIdx >= 0} {
				# XXX -- hack needed because certain color schemes may cause
				# problems for certain window managers on unix hosts
				if {[string match [wtxHostType] x86-win32]} {
					controlItemPropertySet $control \
						-displaytext $friendlyName \
						-bold $bBold \
						-italic $bItalic \
						-foreground $foreground \
						-background $background \
						-imageno $iconIdx $add
				} else {
					controlItemPropertySet $control \
						-displaytext $friendlyName \
						-bold $bBold \
						-italic $bItalic \
						-foreground $foreground \
						-imageno $iconIdx $add
				}
			} else {
				# XXX -- hack needed because certain color schemes may cause
				# problems for certain window managers on unix hosts
				puts "icon '$icon' not found"
				if {[string match [wtxHostType] x86-win32]} {
					controlItemPropertySet $control \
						-displaytext $friendlyName \
						-bold $bBold \
						-italic $bItalic \
						-foreground $foreground \
						-background $background \
						$add
				} else {
					controlItemPropertySet $control \
						-displaytext $friendlyName \
						-bold $bBold \
						-italic $bItalic \
						-foreground $foreground \
						$add
				}
			}
		}

		# Now set properties for the updates by querying server about
		# each item
		foreach item $updates {
			set friendlyName [${server}::displayNameGet \
				$title $item]
			set attributeTuples \
				[${server}::attributesGet \
				$item {-bold -italic -icon -color}]
			set icon 0 
			set bBold 0 
			set bItalic 0
			set foreground $defaultforeground
			set background $defaultbackground
			foreach tuple $attributeTuples {
				set value [lindex $tuple 1]
				switch [string range [lindex $tuple 0] 1 end] {
					icon {
						set icon $value
					}
					bold {
						set bBold $value
					} 
					italic {
						set bItalic $value
					} 
					color {
						set foreground [lrange $value 0 2]
						set background [lrange $value 3 end]
					}
				}
			}

			# Set item's properties in the tree
			if {$icon != 0} {
				set iconIdx [iconIndexLookup $icon]
			} else {
				set iconIdx -1
			}

			# Determine the colors
			set foreground [::colorFromRGBGet $foreground]
			set background [::colorFromRGBGet $background]

			# Update item's properties in the tree
			if {$iconIdx >= 0} {
				# XXX -- hack needed because certain color schemes may cause
				# problems for certain window managers on unix hosts
				if {[string match [wtxHostType] x86-win32]} {
					controlItemPropertySet $control \
						-displaytext $friendlyName \
						-bold $bBold \
						-italic $bItalic \
						-foreground $foreground \
						-background $background \
						-imageno $iconIdx $item
				} else {
					controlItemPropertySet $control \
						-displaytext $friendlyName \
						-bold $bBold \
						-italic $bItalic \
						-foreground $foreground \
						-imageno $iconIdx $item
				}
			} else {
				# XXX -- hack needed because certain color schemes may cause
				# problems for certain window managers on unix hosts
				puts "icon '$icon' not found"
				if {[string match [wtxHostType] x86-win32]} {
					controlItemPropertySet $control \
						-displaytext $friendlyName \
						-bold $bBold \
						-italic $bItalic \
						-foreground $foreground \
						-background $background \
						$item
				} else {
					controlItemPropertySet $control \
						-displaytext $friendlyName \
						-bold $bBold \
						-italic $bItalic \
						-foreground $foreground \
						$item
				}
			}
		}
	}

	######################################################################
	#
	# pathShow - make sure each fully qualified path in the list is visible
	# and up-to-date (expand nodes along the way if necessary)
	#
	proc pathShow {guiServer pathList} {
		set server [namespace parent $guiServer]
		set sheetName [${server}::titleGet]
		set control [format "$::ViewBrowser::_windowName.%sTree" $sheetName]
		set pathListOrig $pathList

		# Drop the last node from each path and merge the paths
		foreach path $pathList {
			set idx [llength $path]
			if {$idx > 1} {
				set idx [expr $idx - 2]
				set parent [lrange $path 0 $idx]
			} else {
				set parent ""
			}
			set mergedPaths($parent) ""
		}
		set pathList [array names mergedPaths]

		# Walk each path, segment by segment, and create what is needed
		foreach path $pathList {
			set bExists 1
			foreach segment $path {
				# Expand sub path
				set subPath [lrange $path 0 [lsearch $path $segment]]
				set pane [[namespace parent $guiServer]::titleGet]
				nodeExpand $::ViewBrowser::_windowName $pane $subPath
			}
		}

		# Select each of the items in turn
		foreach path $pathListOrig {
			controlSelectionSet $control $path
		}
	}

	######################################################################
	#
	# The Workspace's view server implementation 
	#
	######################################################################

	proc menuMatch {type server handle} {
		if {[string compare $type WorkspaceShared] == 0} {
			::ViewBrowser::menuAdd "&Properties..." {::Workspace::propSheetShow}
		}
	}

	# Add workspace's menu contributions
	::ViewBrowser::menuServerRegister ::ViewBrowser {}

	######################################################################
	#
	# Implementation 
	#
	######################################################################

	proc sheetCreate {windowName pane} {
		controlTabsAdd [format "%s.tabctrl" $windowName] $pane

		set serverIface $::ViewBrowser::_connections($pane)

		# Call the server's gui initialization
		if {[catch {eval [format "%s::Init $pane" $serverIface]} \
			errorMsg]} {
			error "View Server Init() failed: $errorMsg"
		}

		# Create the pane's controls by asking the server to do so
		if {[catch {eval controlsCreate $windowName $pane} errorMsg]} {
			puts "error creating controls for interface '$serverIface':"
			puts "    $errorMsg"
		} else {
			lappend ::ViewBrowser::_tabsheets $pane
		}
	}

	proc OnTabClicked {} {

		# Hide the last one
		set tab $::ViewBrowser::_currentTab
		OnHide $::ViewBrowser::_windowName $tab

		# Show the new current selection
		set tab \
			[controlSelectionGet $::ViewBrowser::_windowName.tabctrl -string]
		OnShow $::ViewBrowser::_windowName $tab
		set ::ViewBrowser::_currentTab $tab
	}

	# Called by the ViewBrowser to display a context menu
	proc ctxMenuShow {ctrlPath} {
		variable _bMenuAdded

		set server \
			$::ViewBrowser::_connections($::ViewBrowser::_currentTab)
		set handle [::Workspace::selectionGet]

		# Create or cleanup after our menu
		if {[menuExists ctxMenu]} {
			menuDelete $::ViewBrowser::_currentMenu
		}
		menuCreate -name ctxMenu -context
		set ::ViewBrowser::_currentMenu ctxMenu

		# Ask each server to register menus of each type 
		# Create the object-specific menus first, then
		# then server-specific ones, then the workspace ones
		# Add a separator between each

		# Object-specific menus only good for value object handle
		if {$handle != ""} {
			foreach menuServer [::ViewBrowser::menuServersGet] {
				set cmd [list [format "%s::menuMatch" $menuServer] \
					Object $server $handle]
				# this could fail if the current build is not installed
				# so just pocket any error messages
				if { [catch { [format "%s::menuMatch" $menuServer] \
								  Object $server $handle }] } {
					global errorInfo
					puts $errorInfo
				}
			}
		}
		
		foreach menuType {Server WorkspaceShared} {
			# Place a separator between each menu type
			menuItemAppend $::ViewBrowser::_currentMenu -separator

			foreach menuServer [::ViewBrowser::menuServersGet] {
				catch { [format "%s::menuMatch" $menuServer] \
							$menuType $server $handle }
			}
		}

		# Eliminate any double separators
		menuCleanSeparators $::ViewBrowser::_currentMenu

		# Show the menu
		controlContextMenuShow $ctrlPath $::ViewBrowser::_currentMenu
	}

	proc guiEventHandle {event} {
		set server \
			$::ViewBrowser::_connections($::ViewBrowser::_currentTab)
		set handle [::Workspace::selectionGet]
		set objType [::Object::evaluate $handle Type] 
		set supportedObjs [array names ::ViewBrowser::_eventHandlers]
		if {[lsearch $supportedObjs $objType] != -1} {
			foreach handler $::ViewBrowser::_eventHandlers($objType) {
				if {[string match [lindex $handler 0] $event]} {
					set callback [lindex $handler 1]
					set cmd [list $callback $event $handle]
					if {[catch {eval $cmd} errorMsg]} {
						set error "Error: gui event handler"
						set error "$error callback bombed '$cmd': $errorMsg"
						error $error
					}
					break
				}
			}
		}
	}

	proc controlsCreate {windowName sheetName} {
		variable _treeviewXPos
		variable _treeviewYPos
		variable _treeviewHeight
		variable _treeviewWidth

		set ctrlNamesList [format "%s::_ctrlNameList($sheetName)" \
			[namespace current]]
		set treeCtrlName [format "%sTree" $sheetName]
		set ctrlList [list \
			[list tree -name $treeCtrlName \
				-border \
				-callback [list [namespace current]::onTreeClick \
					$windowName $treeCtrlName $sheetName] \
				-x $_treeviewXPos \
				-y $_treeviewYPos \
				-width [expr $_treeviewWidth - 5] \
				-height [expr $_treeviewHeight - 5] \
			] \
		]
		foreach ctrl $ctrlList {
			set ctrlName [::flagValueGet $ctrl -name]	
			if {![catch {controlCreate $windowName $ctrl} errorMsg]} {

				# Hide the control
				controlHide ${windowName}.$ctrlName 1
				lappend $ctrlNamesList $ctrlName
			}
		}
		if {[catch {controlPropertySet ${windowName}.$treeCtrlName \
			-imagelist "icons.dll [iconNamesGet]"} error]} {
			messageBox -ok -exclamationicon \
				"ImageList for explorer didn't load: $error"
		}
	}

	proc controlsDestroy {windowName sheetName} {
		set ctrlNamesList [format "%s::_ctrlNameList($sheetName)" \
			[namespace current]]
		set treeCtrlName [format "%sTree" $sheetName]
		foreach ctrl [set $ctrlNamesList] {
			controlDestroy ${windowName}.$ctrl
		}
		unset $ctrlNamesList
	}

	proc OnShow {windowName pane} {
		set ctrlNames [format "%s::_ctrlNameList($pane)" [namespace current]]
		foreach ctrl [set $ctrlNames] {
			controlHide ${windowName}.$ctrl 0
		}

		# Make sure that the current selection is set
		set treeCtrlName [format "%sTree" $pane]
		if {[controlSelectionGet ${windowName}.$treeCtrlName] == ""} {
			# Get the first entry in the tree
			set iface [namespace current]
			set leaves [controlValuesGet ${windowName}.$treeCtrlName] 
			set firstNode [lindex $leaves 0] 
			controlSelectionSet ${windowName}.$treeCtrlName \
				-noevent $firstNode
		}
	}

	proc OnHide {windowName pane} {
		set ctrlNames [format "%s::_ctrlNameList($pane)" [namespace current]]
		foreach ctrl [set $ctrlNames] {
			controlHide ${windowName}.$ctrl 1
		}
	}

	#############################################################
	#
	# Expand this node one level if it hasn't been populated yet.
	# Since the View Browser is actually a view for the workspace
	# and projects, it has the smarts to ask the workspace questions
	# about objects that the view servers know nothing about, like
	# projects.
	#
	variable _expandSemaphore 0
	proc nodeExpand {windowName pane path} {
		variable _expandSemaphore	
		::beginWaitCursor
		set control [format "%s.%sTree" $windowName $pane] 
		set guiServer $::ViewBrowser::_connections($pane)
		set server [namespace parent $guiServer] 

		set currentValues [controlValuesGet $control -childrenonly $path]
		set currentValues [lindex $currentValues 0]

		# If this node has no children, expand
		if {$currentValues == "" } {
			OnWatch $guiServer [list $path]
			set currentValues [controlValuesGet $control -childrenonly $path]
			set currentValues [lindex $currentValues 0]
		}
		::endWaitCursor
		
		if {$currentValues == ""} {
			# Asking an empty node to expand itself will get rid of the '+'
			set _expandSemaphore 1
			controlItemPropertySet $control -expand 1 $path
			set _expandSemaphore 0
		}
	}

	proc expandAndFillIfNecessary { handle parentWindow \
			theControl node event pane} {

		# _expandSemaphore keeps us from becoming reentrant.  See
		# nodeExpand()
        variable _expandSemaphore

		if {[expr !$_expandSemaphore]} {
			if {[::Object::evaluate $handle bFolder]} {
				
				# If the node has already been populated, call 
				# the gui event handler	
				set nodeContents [controlValuesGet \
								  $theControl \
								  -childrenonly $node]
				set nodeContents [lindex $nodeContents 0]
				if {$nodeContents != ""} {
					::ViewBrowser::guiEventHandle $event
				} else {
					# Fetch the nodes.  If none, remove '+' 
					nodeExpand $parentWindow $pane $node
				}
			} else {
				::ViewBrowser::guiEventHandle $event
			}
		}
	}

	proc onTreeClick {parentWindow treeCtrl pane} {
		set event [controlEventGet ${parentWindow}.$treeCtrl]

		# Obtain the current tree selection and use it to set
		# the workspace's current selection handle
		set ns [namespace parent [paneToServerNs $pane]]
		set theControl $parentWindow.$treeCtrl
		if { $event == "expanding" } {
			set targetNode [controlExpandingItemGet $theControl]
		} else {
			set targetNode [controlSelectionGet $theControl]
		}
		if {$targetNode != "" && \
				![catch {set handle [${ns}::objectBind $targetNode] }] } {
			set iface [paneToServerNs $pane]
			::Workspace::selectionSet $handle

			switch $event {
				contextmenu {
					# If this is a mouse right-click, show the context menu
					::ViewBrowser::ctxMenuShow $theControl
					return
				}
				selchange {
				}
				expanding { 
					expandAndFillIfNecessary $handle $parentWindow \
						$theControl $targetNode $event $pane
				}
				dblclk {
					expandAndFillIfNecessary $handle $parentWindow $theControl \
							$targetNode $event $pane
				}
				default {
					::ViewBrowser::guiEventHandle $event
				}
			}
		}

		# Check to see if we have a selection now; if not, set null object 
		# This is also covered in onWatch(), but you never know...
		set curSel [controlSelectionGet $theControl]
		if {$curSel == ""} {
			::Workspace::selectionSet $::Workspace::_NULL_OBJECT
		}
	}

	proc serverToPane {namespace} {
		variable _connections
		set tab ""
		foreach t [array names _connections] {
			if {[string match $namespace $_connections($t)]} {
				set tab $t
				break
			}
		}
		return $tab
	}

	proc paneToServerNs {pane} {
		variable _connections
		set ns ""
		if {[info exists _connections($pane)]} {
			set ns $_connections($pane)
		}
		return $ns
	}	

	#############################################################
	#
	# serverNodeStateColorGet - return the user-customizable color
	# for a particular tree node state.  Whether this state has any
	# relevance to a node is entirely up to the corresponding
	# view server 
	#
	proc serverNodeStateColorGet {type} {
		set retval ""
		switch $type {
			error {
				set retval {White Red}
			}
			select {
				set retval {White Green}
			}
			treeDefault -
			default {
				set retval {White Black}
			}
		}
	}
}

