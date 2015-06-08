# WorkspaceDoc.tcl - implementation of the workspace document
#
# modification history
# --------------------
# 03n,08oct01,fle  unified Tornado version String
# 03m,04mar99,cjs  Trim whitespace from workspace file name
# 03l,11feb99,rbl  Changes to handle missing or read-only projects
# 03k,02oct98,cjs  cleanup workspaceClose() logic 
# 02d,24sep98,cjs  removed gui code!
# 02c,21sep98,cjs  fixed workspace save as functionality 
# 02b,09sep98,cjs  removed error status code and put it into project engine
# 02a,04sep98,cjs  workspace is now a project document (prjLib API);
#                  added version and datestamp
# 01d,28aug98,ms   store projects using relative paths.
# 01c,22jul98,cjs  added workspaceDirtyGet() 
# 01b,07may98,cjs  added dirty flag support 
# 01a,27mar98,cjs  written.
#
# DESCRIPTION
# The workspace document is a file that contains, among other
# things, the list of Projects in the workspace.
# This module manages addition, removal, enumeration of projects.
# The workspace is different from other objects in that it is
# a singleton (no more than one instance of it will ever be
# created).  Consequently, all the instance data is lumped in
# as class data.
#
#############################################################

package require Wind

namespace eval ::WorkspaceDoc {

	variable _hWorkspace
	set _WORKSPACE_VERSION [Wind::version]

	# Since this is a singleton, our instance data
	# can be stored in the class
	variable _workspaceObj

	proc Name {args} {
		return [lindex $args 0]
	}

	proc Type {args} {
		return workspaceObject 
	}

	proc workspaceDirtyGet {} {
		variable _hWorkspace
		set retval 0
		if {[info exists _hWorkspace]} {
			set changed [::prjInfoGet $_hWorkspace changed]
			if {$changed != ""} {
				set retval 1
			}
		}
		return $retval
	}

	proc projectListGet {} {
		variable _hWorkspace
		set projectList [::prjTagDataGet $_hWorkspace projectList]
		return $projectList
	}

	proc versionGet {} {
		variable _hWorkspace
		return [::prjVersionGet $_hWorkspace]
	}

	proc workspaceOpen {fileName} {
		variable _hWorkspace

		# Check to see that the workspace file exists
		set fileName [string trim $fileName]
		if {![file exists $fileName]} {
			error "Workspace file '$fileName' not found"
		} elseif {![string match ".wsp" [file extension $fileName]]} {
			error "This file doesn't have a workspace (.wsp) extension"
		}
		
		# Backup and close the old workspace
		if {[info exists _hWorkspace]} {
			set oldWorkspace [::prjInfoGet $_hWorkspace fileName]
			::prjClose $_hWorkspace
		} else {
			set oldWorkspace ""
		}

		if {[catch {prjOpen $fileName} _hWorkspace]} {
			set errorMsg $_hWorkspace
			catch {unset _hWorkspace}
			if {$oldWorkspace != ""} {
				# Attempt to re-open old workspace file
				if {[catch {::prjOpen $oldWorkspace} _hWorkspace]} {
					set errorMsg $_hWorkspace
					catch {unset _hWorkspace}
					error "bad news: $errorMsg"
				}
			}
			error $errorMsg
		} else {
			# Open was successful.  Now check version info
			if {[versionGet] > $::WorkspaceDoc::_WORKSPACE_VERSION} {
				::prjClose $_hWorkspace 
				catch {unset _hWorkspace}
				
				if {$oldWorkspace != ""} {
					# Attempt to re-open old workspace file
					if {[catch {::prjOpen $oldWorkspace} _hWorkspace]} {
						set errorMsg $_hWorkspace
						catch {unset _hWorkspace}
						error "bad news: $errorMsg"
					}
				}
				error "Workspace open failed: This project\
					workspace is of a newer format"
			} elseif {[versionGet] < $::WorkspaceDoc::_WORKSPACE_VERSION} {
				::prjClose $_hWorkspace
				catch {unset _hWorkspace}
				if {$oldWorkspace != ""} {
					# Attempt to re-open old workspace file
					if {[catch {::prjOpen $oldWorkspace} _hWorkspace]} {
						set errorMsg $_hWorkspace
						catch {unset _hWorkspace}
						error "bad news: $errorMsg"
					}
				}
				error "Workspace open failed: This project\
					workspace is an older format" 
			}
		}
	}

	# Determine to determine the currently open workspace 
	proc openWorkspaceGet {} {
		variable _hWorkspace
		set retval ""

		if {[info exists _hWorkspace]} {
			set retval [::prjInfoGet $_hWorkspace fileName]
		}
		return $retval
	}

	# Get the project handle of the open workspace
	proc workspaceHandleGet {} {
		variable _hWorkspace
		return $_hWorkspace
	}

	########################################################################
	#
	# workspaceSave {{fileName ""} {prjSaveFunction "::prjSaveAs"}} 
	# 
	# implements Save and SaveAs functionality, depending 
	# on whether the fileName parameter is set.
	#
	# The first argument is the fileName parameter (may be "").
	# The second argument is prjSaveFunction parameter. 
	# prjSaveFunction must have signature prjSaveFunction {handle, fileName}, 
	# It must save the actual workspace document, and return the new fileName.
	#
	# If not set, this
	# defaults to the non-gui function ::prjSaveAs.
	#
	# You can hook in a gui function here to cope with for instance read-only 
	# project or workspace files
	#
	# RETURNS: the new filename, or "" if the operation was cancelled
	#
	proc workspaceSave {{fileName ""} {prjSaveFunction "::prjSaveAs"}} {
		variable _hWorkspace
		set currentFileName [::prjInfoGet $_hWorkspace fileName]

		if {[catch {$prjSaveFunction $_hWorkspace $fileName} newFileName]} {
			set error "Unable to save workspace '$fileName':\n$newFileName"
			error $error
		# blank fileName means save was cancelled
		} elseif {$newFileName == ""} {
			return ""
		} 

		# On a "saveas", or if a "save" resulted in a "saveas" because the workspace file
		# was read only, we need to do the following:
		#
		# Close the old file, open the new one, remove and add
		# the projects, save.  
		#
		# This ensures that the paths for the
		# projects get written relative to the correct PRJ_DIR. 
		#
		if {$newFileName != "" && \
			$newFileName != $currentFileName} {
			set prjFiles [::prjTagDataGet $_hWorkspace projectList]
			::prjClose $_hWorkspace
			set _hWorkspace [::prjOpen $newFileName]
			::prjTagRemove $_hWorkspace projectList
			::prjTagDataSet $_hWorkspace projectList $prjFiles
			::prjSave $_hWorkspace
			return $newFileName
		} else {
			return $currentFileName
		}
	}

	proc workspaceCreate {fileName} {
		variable _hWorkspace
		variable _workspaceObject
		set _hWorkspace [::prjCoreCreate $fileName Workspace \
			$::WorkspaceDoc::_WORKSPACE_VERSION 0]
		set _workspaceObject [::Object::create ::WorkspaceDoc \
			[list $fileName workspaceObject]]
	}

	proc workspaceClose {} {
		variable _hWorkspace
		variable _workspaceObject
		catch {unset _workspaceObj}
		if {[info exists _hWorkspace]} {
			if {[catch {::prjClose $_hWorkspace} errorMsg]} {
				error "workspaceClose($_hWorkspace) failed: $errorMsg"
			}
			unset _hWorkspace
		}
	}

	proc projectAdd {projectName} {
		variable _hWorkspace
		set projectList [::prjTagDataGet $_hWorkspace projectList]
		lappend projectList $projectName
		::prjTagDataSet $_hWorkspace projectList $projectList
	}

	proc projectDelete {projectName} {
		variable _hWorkspace
		set projectList [::prjTagDataGet $_hWorkspace projectList]
		set idx [lsearch $projectList $projectName]
		if {$idx == -1} {
			error "Error: project '$projectName' not found in workspace"
		} else {
			set projectList [lreplace $projectList $idx $idx]
		}
		::prjTagDataSet $_hWorkspace projectList $projectList
		::prjTagRemove $_hWorkspace PROJECT_${projectName}_componentErrorState
	}

	proc test {} {
		catch {file delete workspace1.wsp}
		catch {file delete workspace2.wsp}
		puts "creating workspace1"
		set wspName t:/wpwr/host/resource/tcl/app-config/project/workspace1.wsp
		workspaceCreate $wspName 

		puts "add projects foo1, project1, project2:"
		projectAdd t:/wpwr/host/resource/tcl/app-config/project/foo1.wpj
		projectAdd t:/wpwr/host/resource/tcl/app-config/project/project1.wpj
		projectAdd t:/wpwr/host/resource/tcl/app-config/project/project2.wpj

		puts "save workspace1"
		workspaceSave $wspName 

		puts "close workspace1"
		workspaceClose

		puts "open workspace1"
		workspaceOpen $wspName 
		foreach name [projectListGet] {
			puts "$name" 
		}

		puts "delete project foo1.wpj:"
		projectDelete t:/wpwr/host/resource/tcl/app-config/project/foo1.wpj

		foreach name [projectListGet] {
			puts "$name" 
		}

		puts "save as workspace1.wsp"
		workspaceSave $wspName 

		puts "close workspace1"
		workspaceClose

		puts "open workspace1.wsp"
		workspaceOpen $wspName
 
		puts "contents of workspace1.wsp:"
		foreach name [projectListGet] {
			puts "$name" 
		}
		workspaceClose
	}
}
