# BuildServer.addin.tcl - Server module for the Build View 
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 04c,30may02,cjs  isSimulatorTc moved to tc_cplus.tcl
# 04b,24mar99,cjs  Make all view servers catch WorkspaceUpdate event
# 04a,19mar99,jak  fix check if sim is running before build
# 03z,16mar99,cjs  Fixing spr 25030: Hands are tied...
# 03y,10mar99,cjs  Fixing bug in menuMatch that caused it to spew
#                  errors on unix
# 03x,05mar99,cjs  Fix bug introduced by addition of workspace object to view
# 03w,02mar99,cjs  Add workspace icon to all view servers
# 03v,02feb99,rbl  Changes to handle missing or read-only projects
# 03u,12nov98,cjs  kill debugger on build if user so requests 
# 03t,12nov98,cjs  Don't show 'Stop Build' menu item for UNIX 
# 03s,02nov98,ren  Fixed problem with resize & build combo box
# 03r,28oct98,cjs  Added name of build target to 'Build' menu; changed title
#                  of build combo to 'Build Spec'
# 03q,15oct98,cjs  Changed 'Active Build' to 'Build'; quoted specific object
#                  names in ctx menu text
# 03p,13oct98,cjs  Fixed bug in check to see if project's simulator is
#                  running and needs to be killed for build 
# 03o,03oct98,cjs  Protect build server when view browser is not visible 
# 03n,29sep98,cjs  tell user about running simulator when trying to build
#                  that image 
# 02m,21sep98,cjs  override object's color attributes 
# 02l,18sep98,cjs  prevent user from deleting last buildspec 
# 02k,17sep98,cjs  added OnStopBuild to context menu
# 02j,15sep98,cjs  catch ProjectUpdate event 
# 02i,14sep98,cjs  fixed bug in ReBuild All; update dependencies before
#                  building
# 02h,09sep98,cjs  allow download of .s files; warn of build errors 
# 02g,31aug98,cjs  only create build combo once 
# 02f,28aug98,jmp  moved dependencies regeneration stuff to
#		   		   Dependencies.addin.tcl.
# 02e,19aug98,cjs  make objectsEnumerate return _NULL_OBJECT when things
#                  go south 
# 02d,13aug98,cjs  No tree update on SelectionChange event.
# 02c,04aug98,jmp  Made the dependencies dialog only be visable from the file
#                  view. replaced dependancies by dependencies.
# 02b,31jul98,cjs  added build combo box; added 'Rebuild All' menu item
# 02a,30jul98,cjs  major overhaul for new view server scheme 
# 01k,29jul98,ms   updated calls to prj[Rel|Full]Path
# 01j,07jul98,cjs  removed rename buildspec from ctx menu 
# 01i,02jul98,jmp  added a progress meter to dependencies Window.
# 01h,01jul98,jmp  removed {Build <image>} from floating menu.
#                  added some text to the dependencies dialog.
#                  added a checkbox to regenerate file dependencies.
# 01g,30jun98,jmp  used ::filePathFix() to convert path to UNIX style.
# 01f,29jun98,jmp  added {Build <image>} to floating menu for all images.
# 01e,24jun98,cjs  added icon support 
# 01d,23jun98,cjs  added code to create, delete, and manage
#                  build specs 
# 01c,05jun98,cjs  cleanup of old object fcns, made build
#                  more global 
# 01b,01may98,cjs  updated for use with new tree control
# 01a,08apr98,cjs  created.
#
# DESCRIPTION
# This file implements the Build View Server.  View Servers
# populate a pane in the view browser by fetching data from
# a lower-level data engine.  The cookies returned are actually
# valid object handles, making objectBind() a no-op.
#

namespace eval ::BuildViewServer {

	#############################################################
	#
	# Addin implementation 
	#
	#############################################################

	proc Init {} {
		set pattern {(^Project.*$)|(^BuildSpec.*$)|(^SelectionChange.*$)|(^WorkspaceUpdate$)}
		::Workspace::eventRegister ::BuildViewServer $pattern 
		::Workspace::serverRegister ::BuildViewServer
		::ViewBrowser::menuServerRegister ::BuildViewServer \
			{
				::BuildViewServer::BuildObject \
			}

		# Register to handle particular gui events for particular objects.
		::ViewBrowser::guiEventHandlerRegister "dblclk" "buildObject" \
			::BuildViewServer::gui::OnBuildOpen
		::ViewBrowser::guiEventHandlerRegister "delete" "buildObject" \
			::BuildViewServer::gui::OnBuildDelete

		catch {unset ::BuildViewServer::gui::_data}
		catch {unset ::BuildViewServer::gui::_children}
	}

	proc OnEvent {cookie eventInfo} {
		set eventData [lindex $eventInfo 1]
		set eventName [lindex $eventInfo 0]

		if {[::ViewBrowser::viewBrowserIsVisible]} {
			if {[regexp {SelectionChange} $eventName]} {
				set handle $eventData 
				::BuildViewServer::gui::buildComboInit $handle
			} elseif {[regexp {ProjectAdd} $eventName]} {
				# If this is a project add, it may be the first one.
				# Be sure the project is visible
				set root [lindex [objectsEnumerate "" -containers] 0]
				set firstChild [lindex [objectsEnumerate \
					$root {-leaves -containers}] 0]
				set cookieLists [list $firstChild]
				::ViewBrowser::OnWatch [namespace current]::gui [list ""] 
				::ViewBrowser::OnWatch [namespace current]::gui \
					$cookieLists
				::ViewBrowser::pathShow [namespace current]::gui \
					$cookieLists

				# Let the user know if the current project uses an
				# uninstalled toolchain
				::BuildViewServer::gui::WarnUserAboutToolchain
			} elseif {[regexp {Project.*} $eventName]} {
				set cookieLists [list ""]
				::ViewBrowser::OnWatch ::BuildViewServer::gui $cookieLists
			} elseif {[regexp \
				{BuildSpecAdd|BuildSpecDelete|BuildSpecChange} $eventName]} {
				set root [lindex [objectsEnumerate "" -containers] 0]
				set prjName [lindex $eventData 0]
				set handle [::Workspace::selectionGet]
				::BuildViewServer::gui::buildComboInit $handle
				set cookieLists [list $root $prjName]
				::ViewBrowser::OnWatch ::BuildViewServer::gui $cookieLists
			} elseif {[string match WorkspaceUpdate $eventName]} {
				::ViewBrowser::OnWatch ::BuildViewServer::gui [list ""] 
			}
		}
	}

	#############################################################
	#
	# Private API
	#
	#############################################################


	#############################################################
	#
	# View Server Implementation 
	#
	#############################################################

	namespace eval gui {

		proc Init {pane} {
			buildComboCreate
		}

		#############################################################
		#
		# Menu callbacks 
		#
		#############################################################

		proc OnProjectBuild {projObj} {
			set projectName [::Object::evaluate $projObj Name]
			set hProj [::Object::evaluate $projObj Handle]
			set status ok
			::beginWaitCursor

			if {[::Workspace::projectErrorStateGet $projectName]} {
				set status [messageBox -okcancel \
					"There are configuration errors.  Attempt to build anyway?"]
			}

			if {[string match $status ok]} {
				# If there are any files needing dependencies calculated,
				# calculate them now 
				# XXX -- note: this introduces inter-server dependence
				set ns ::Workspace 
				if {[${ns}::obviouslyStaleDependenciesGet $hProj] != ""} {
					if {![catch {::Dependencies::OnDependencies \
						$hProj $projObj 1} retval]} {
						if {![string match $retval "ok"]} {
							set status cancel
						}
					}
				}
			}

			# Check to ensure that the simulator is writable
			if {[string match $status "ok"]} {
				set status [simulatorStatusCheck $hProj]
			}

			# Check to see that the debugger is not running
			# XXX -- incomplete
			if {[string match "ok" $status]} {
				if {[::isDebuggerActive]} {
					set status [messageBox -okcancel \
						"You must end your debug session in order to build."]
					if {[string match "ok" $status]} {
						# kill the debugger before building
						::stopDebugger
					}
				}
			}

			if {[string match $status "ok"]} {

				set hSpec [::prjBuildCurrentGet $hProj]
				set rule [::prjBuildRuleCurrentGet $hProj $hSpec]
				if [::prjDirtyGet $hProj] {
					if [::prjProjectFileWritable $hProj] {
						::prjSave $hProj
					} else {
						set status [messageBox -okcancel "Could not save project file [::prjInfoGet $hProj fileName].\n\nIf you continue with the build the Makefile will not be synchronized with the project file.\n\nBuild anyway?"]
					}
				}
			}

			if {[string match $status "ok"]} {
				if {[catch {::Workspace::projectBuild \
					$projectName $rule} error]} {
					messageBox "Build failed: $error"
				}
			}
			::endWaitCursor
		}

		proc OnProjectReBuildAll {projObj} {
			::beginWaitCursor
			set projectName [::Object::evaluate $projObj Name]
			set hProj [::Object::evaluate $projObj Handle]
			set hSpec [::prjBuildCurrentGet $hProj]
			set rule [::prjBuildRuleCurrentGet $hProj $hSpec]

			set status ok
			if {[::Workspace::projectErrorStateGet $projectName]} {
				set status [messageBox -okcancel \
					"There are configuration errors.  Attempt to build anyway?"]
			}

			if {[string match $status ok]} {
				# If there are any files needing dependencies calculated,
				# calculate them now 
				# XXX -- note: this introduces inter-server dependence
				set ns  ::Workspace
				if {[${ns}::obviouslyStaleDependenciesGet $hProj] != ""} {
					if {![catch {::Dependencies::OnDependencies \
						$hProj $projObj 1} retval]} {
						if {![string match $retval "ok"]} {
							set status cancel
						}
					}
				}
			}

			# Check to ensure that the simulator is writable
			if {[string match $status "ok"]} {
				set status [simulatorStatusCheck $hProj]
			}

			if {[string match $status "ok"]} {
				if [::prjDirtyGet $hProj] {
					if [::prjProjectFileWritable $hProj] {
						::prjSave $hProj
					} else {
						set status [messageBox -okcancel "Could not save project file [::prjInfoGet $hProj fileName].\n\nIf you continue with the build the Makefile will not be synchronized with the project file.\n\nBuild anyway?"]
					}
				}
			}

			if {[string match $status "ok"]} {
				if {[catch {::Workspace::projectBuild \
					$projectName [list clean $rule]} error]} {
					messageBox "Build failed: $error"
				}
			}
			::endWaitCursor
		}

		proc OnBuildStop {} {
			::buildStop
		}

		proc OnBuildDelete {event handle} {
			set buildSpec [::Object::evaluate $handle Name]
			set projectName [::Object::evaluate \
				[::Object::evaluate $handle Project] Name]
			set hProj [::Object::evaluate \
				[::Object::evaluate $handle Project] Handle]

			if {[llength [::prjBuildListGet $hProj]] > 1} {
				if {[string match [messageBox -okcancel \
					"Delete build spec '$buildSpec'?"] ok]} {
					::Workspace::buildSpecDelete \
						::BuildViewServer $projectName $buildSpec
				}
			} else {
				messageBox -ok "Sorry, can't delete the last \
					build in a project."
			}
		}

		proc OnBuildOpen {event handle} {
			::Workspace::propSheetShow
		}

		proc OnNewBuild {event handle} {
			set ns ::BuildViewServer::gui::AddBuildDlg
			${ns}::AddBuildConfigDlgShow $handle
		}

		# The following three procs co-opt a combo box on the view browser
		# so we can set the current build spec

		proc buildComboCreate {} {
			# These controls are created in a specially designed space
			# for the purpose in the viewbrowser
			if {![controlExists \
				${::ViewBrowser::_windowName}.lblactivebuild]} {
				controlCreate $::ViewBrowser::_windowName \
					[list label -name lblactivebuild \
					-title "Bui&ld Spec" \
					-xpos 4 -ypos 6 -width 40 -height 12]
				controlCreate $::ViewBrowser::_windowName \
					[list combo -name comboactivebuild \
					-xpos 44 -ypos 4 -width 118 -height 48 \
					-callback \
						::BuildViewServer::gui::OnActiveBuildComboClicked \
					-nointegralheight]
			}
		}

		proc buildComboInit {handle} {
			if {![catch {::Object::evaluate \
					[::Object::evaluate $handle Project] Handle} hProj]} {
				set items [::prjBuildListGet $hProj]
				set current [::prjBuildCurrentGet $hProj]
				controlValuesSet $::ViewBrowser::_windowName.comboactivebuild \
					$items
				controlSelectionSet \
					$::ViewBrowser::_windowName.comboactivebuild \
					-noevent -string $current
			} else {
				controlValuesSet \
					$::ViewBrowser::_windowName.comboactivebuild "" 
			}
		}

		# Returns 1 if toolchain exists, 0 if not 
		proc WarnUserAboutToolchain { } {
			set handle [::Workspace::selectionGet]
			if {![catch {::Object::evaluate $handle Project} projObj]} {
				set hProj [::Object::evaluate $projObj Handle]
				set currentBuildName [controlSelectionGet \
					$::ViewBrowser::_windowName.comboactivebuild -string]
				set build  [prjBuildCurrentGet $hProj]
				set tc [prjBuildTcGet $hProj $build]
				if {![::Workspace::isToolchainInstalled $tc]} {
					regsub {(:*)tc_} $tc {} toolchain
						messageBox "The build $currentBuildName uses\
							the toolchain $toolchain\nwhich is not part\
							of your Tornado installation.\
							\nNot all operations are available."
				}
			}
		}			

		proc OnActiveBuildComboClicked {} {
			set event [controlEventGet \
				$::ViewBrowser::_windowName.comboactivebuild]
			if {$event == "selchange"} {
				set handle [::Workspace::selectionGet]
				if {![catch {::Object::evaluate $handle Project} projObj]} {
    	    		set prjName [::Object::evaluate \
    	                $projObj Name]
    	    		set current [controlSelectionGet \
    	                $::ViewBrowser::_windowName.comboactivebuild -string]
    	    		if {$current != ""} {
						::Workspace::buildSpecActiveSet \
							::BuildViewServer::iHierarchyView $prjName $current
    	    		}
				} else {
					controlValuesSet \
						$::ViewBrowser::_windowName.comboactivebuild ""
				}
			}
		}

		# Warn user if the currently running simulator matches what
		# s/he is trying to build.  Will choke if hProj is not a project
		# with a valid toolchain
		proc simulatorStatusCheck {hProj} {
			set hSpec [::prjBuildCurrentGet $hProj]
			set tc [::prjBuildTcGet $hProj $hSpec]
			set prjType [::prjTypeGet $hProj]
			set retval ok

			# XXX
			# This is only relevant for PC; bail if unix
			if {![string match [wtxHostType] "x86-win32"]} {
				return $retval 
			}

			# This only applies to vxWorks images for the simulator
			if {([isSimulatorTc $tc [wtxHostType]]) && \
				([string match $prjType "::prj_vxWorks"])} {

				# XXX -- hack!
				set simulator [::Workspace::projectImageGet $hProj]

				# We only have a problem if we have a running custom
				# simulator
				set simPath [::SelectedTarget::defaultSimPathGet [wtxPath]]

				# XXX -- hack to get case agreement on drive letters
				set simPath [format "%s%s" \
					[string tolower [string index $simPath 0]] \
					[string range $simPath 1 end]]
				set simulator [format "%s%s" \
					[string tolower [string index $simulator 0]] \
					[string range $simulator 1 end]]

				# makesure slashes are right
				set simPath [file nativename $simPath]
				set simulator [file nativename $simulator]

				if {([::SelectedTarget::isSimulatorActive]) && \
					($simPath == $simulator)} {

					# Warn user, offer to kill simulator
					set retval [messageBox -okcancel -questionicon \
						"The simulator you are about to \
						build is currently running.\nKill it?"]
					if {[string match "ok" $retval]} {
						set retval [::SelectedTarget::simulatorKill]
					}
				}
			}
			return $retval
		}
	}

	#############################################################
	#
	# Menu Server Implementation 
	#
	#############################################################

	# Register server's menu items
	proc menuMatch {type server handle} {
		set objType [::Object::evaluate $handle Type]


		if {[string match $type Server]} {

			# Server-specific menus.  We cheat, because we
			# do this on behalf of all servers.
			# add build menu that must operate on a projectObject
			set ns ::BuildViewServer::gui
			if {![catch {::Object::evaluate $handle Project} projObj]} {
				set hProj [::Object::evaluate $projObj Handle]
				set build  [::prjBuildCurrentGet $hProj]
				set tc [::prjBuildTcGet $hProj $build]
				set bValidToolchain \
					[::Workspace::isToolchainInstalled $tc]
				if {$bValidToolchain} {
					set imageName [file tail \
						[::Workspace::projectImageGet $hProj]]
					::ViewBrowser::menuAdd "Bui&ld '$imageName'" \
						[list ${ns}::OnProjectBuild $projObj]
					::ViewBrowser::menuAdd "&ReBuild All ($imageName)" \
						[list ${ns}::OnProjectReBuildAll $projObj]
				} else {
					::ViewBrowser::menuAdd "Bui&ld" {} 1
					::ViewBrowser::menuAdd "&ReBuild All" {} 1 
				}
			}

			if {[string match "x86-win32" [wtxHostType]]} {
				if {[::isBuildActive]} {
					::ViewBrowser::menuAdd "&Stop Build" \
						${ns}::OnBuildStop 0
				} else {
					::ViewBrowser::menuAdd "Stop Build" ${ns}::OnBuildStop 1
				}
			}

			# Make 'add new build' available on both project objects and their
			# children 
			if {![catch {::Object::evaluate $handle Project}]} {
				if {[string match ::BuildViewServer::gui $server]} {
					::ViewBrowser::menuAdd "Ne&w Build..." \
						[list ${ns}::OnNewBuild "none" $handle]
				}
			}

		} elseif {[string match $type Object]} {

			if {[string match $objType buildObject]} {
			
				# Build specs 
				set projObj [::Object::evaluate $handle Project]
				set hProj [::Object::evaluate $projObj Handle]
				set build [::Object::evaluate $handle Name]
				set tc [prjBuildTcGet $hProj $build]
				set bValidToolchain \
					[::Workspace::isToolchainInstalled $tc]

				::ViewBrowser::menuAdd "&Open '$build'" \
					[list ::BuildViewServer::gui::OnBuildOpen \
					"dblclk" $handle]
				::ViewBrowser::menuAdd "&Delete '$build'" \
					[list ::BuildViewServer::gui::OnBuildDelete \
					"delete" $handle]

				if {$bValidToolchain} {
			    	::ViewBrowser::menuAdd "Se&t '$build'\
						as Active Build" \
				    	[list controlSelectionSet \
				    		${::ViewBrowser::_windowName}.comboactivebuild \
				    		-string $build]
				} else {
			    	::ViewBrowser::menuAdd "Se&t '$build' as Active Build" {} 1 
				}
			} elseif {[string match $objType projectObject]} {

				# Projects
			}
		} elseif {[string match $type WorkspaceShared]} {

			# Globals
		}
	}

	#############################################################
	#
	# View Server/View Browser Interface API implementation 
	#
	#############################################################

	proc titleGet {} {
		return "Builds"
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
					set buildList [::prjBuildListGet $hProj]
					foreach build $buildList {
						lappend retval [list $workspaceName $prjName $build]
					}
				} else {
					set retval ""
				}
			}
			default {
				error "::BuildViewServer::objectsEnumerate: path '$path' \
					not valid"
			}
		}

		if {![info exists retval]} {
			set retval ""
		}
		return $retval
	}

	proc bFolder {path} {
		set retval 0
		set obj [objectBind $path]
		set retval [::Object::evaluate $obj bFolder]
		return $retval
	}

	proc displayNameGet {callerName path} {
		set displayName ""
		set obj [objectBind $path]
		if {$obj != ""} {
			set displayName [::Object::evaluate $obj DisplayName $callerName]
		}
		return $displayName 
	}

	# XXX -- todo: finish
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
				# It's a build 
				set workspaceName [lindex $path 0]
				set prjName [lindex $path 1]
				set buildName [lindex $path 2]
				set hProj [::Workspace::projectHandleGet $prjName]
				if {$hProj != ""} {
					set name $buildName 
					set obj [::Object::create \
						::BuildViewServer::BuildObject \
						[list $hProj $buildName]]
				}
			}
		}
		return $obj
	}

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
	# BuildViewServer 
	#
	#############################################################

	# STRUCTURE -- {hProj name}
	namespace eval ::BuildViewServer::BuildObject {
		proc Type {args} { return buildObject }
		proc Name {args} {
			set args [lindex $args 0]
			return [lindex $args 1]
		}
		proc bFolder {args} {
			return 0
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
						lappend retval {-icon IDB_BUILD}
					}
					bold {
						if {[string match $id [::prjBuildCurrentGet $hProj]]} {
							lappend retval {-bold 1}
						}
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
		proc DisplayName {args} {
			set args [lindex $args 0]
			return [lindex $args 1] 
		} 
		proc Project {args} {
			set args [lindex $args 0]
			set hProj [lindex $args 0]
			set obj [::Object::create ::Workspace::ProjectObject $hProj] 
			return $obj
		}
	}

	#############################################################
	#
	# Register ourselves as an add-in.  This must occur last.
	#
	#############################################################

	::Workspace::addinRegister ::BuildViewServer
}
