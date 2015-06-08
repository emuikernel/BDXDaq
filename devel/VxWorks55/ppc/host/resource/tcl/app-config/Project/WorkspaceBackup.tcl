# WorkspaceBackup.tcl - project workspace archival/restoration
#
#
# modification history
# --------------------
# 02q,29mar99,jak  fix zipfile path (2)
# 02p,16mar99,cjs  Use correct zip binary
# 02o,03mar99,cjs  Fixing to accomodate new workspace object
# 02n,01feb99,ren  Adding help button.
# 02m,02dec98,cjs  Fix problem archiving workspace on UNIX
# 02l,23nov98,cjs  Use correct path for binaries in installed tree
# 02k,13nov98,cjs  written 
#
# DESCRIPTION
# UI and logic to backup or restore a project workspace.
#
#############################################################
namespace eval ::WkspBackup {

	proc onBackup {} {
		set ctrlList [list \
			[list group -name zipfilegroup \
				-title "Create &zip file"\
				-x 7 -y 3 -w 252 -h 28\
			] \
			[list text -name backupnametext -x 14 -y 14 -w 238 -h 12] \
			[list group -name filesgroup \
				-title "The following &files will be backed up"\
				-x 7 -y 38 -w 252 -h 93\
			] \
			[list list -name fileslist -x 14 -y 49 -w 238 -h 61 \
				-callback ::WkspBackup::onSelFile -nointegralheight\
			]\
			[list text -name filetext -x 14 -y 112 -w 238 -h 12 \
				-readonly\
			] \
			[list button -name okbutton -title &OK -default \
				-x 270 -y 7 -w 50 -h 14 \
				-callback ::WkspBackup::onOk\
			] \
			[list button -name cancelbutton -title &Cancel \
				-x 270 -y 25 -w 50 -h 14 \
				-callback ::WkspBackup::onCancel\
			] \
			[list button -name helpbutton -title &Help \
				-x 270 -y 43 -w 50 -h 14 \
				 -helpbutton ] \
		]

		set workspaceFile [file rootname \
			[file tail [::Workspace::openWorkspaceGet]]]

		dialogCreate -name ::WkspBackup::BackupDlg \
			-title "Backup Project Workspace" \
			-w 326 -h 138 \
			-controls $ctrlList \
			-init ::WkspBackup::init
	}

	proc init {} {

		# Set the default workspace to be backed up
		set workspaceFile [::Workspace::openWorkspaceGet]
		controlValuesSet ::WkspBackup::BackupDlg.backupnametext \
			"[file rootname $workspaceFile].zip"

		# Collect all the source files for the various projects
		# and display them in the checklist

		# If a setup log exists, record it
		set setupLog [setupLogGet]

		# Now, archive the relevant files
		if {$setupLog != ""} {
			lappend fileList $setupLog
		}
		lappend fileList $workspaceFile
		set prjList [::Workspace::projectListGet]
		set fileList [concat $fileList $prjList]
		foreach proj $prjList {
			set hProj [::Workspace::projectHandleGet $proj]
			set newFiles [::prjFileListGet $hProj]

			# Eliminate duplicates and set checkstate
			foreach file $newFiles {
				if {[lsearch $fileList $file] == -1} {
					lappend fileList $file
				}
			}
		}
		controlValuesSet ::WkspBackup::BackupDlg.fileslist $fileList

		# Select the first file entry
		controlSelectionSet ::WkspBackup::BackupDlg.fileslist 0
	}

	proc onOk {} {
		::beginWaitCursor
		::Workspace::statusMessagePush "Fetching workspace files..."
		::uiEventProcess

		set zipFile [controlValuesGet \
			::WkspBackup::BackupDlg.backupnametext]

		set openWorkspace [::Workspace::openWorkspaceGet]
		set projects [::Workspace::objectsEnumerate "" -containers]	

		# If a setup log exists, record it
		set setupLog [setupLogGet]

		# Now, archive the relevant files
		if {$setupLog != ""} {
			lappend fileList $setupLog
		}

		# Append the workspace file
		lappend fileList [::Workspace::openWorkspaceGet]

		# Fetch the list of files from the checklist 
		foreach file [controlValuesGet ::WkspBackup::BackupDlg.fileslist] {
			lappend fileList $file
		}

		# Fetch the name of the backup and zip binary
		set zip [zipExeGet ZIP]

		set bError 0

		if {$zip != ""} {

			# Archive it all in the zip file 
			set oldwd [pwd]
			if {[catch {cd [file dirname $zipFile]} error]} {
				messageBox -ok "Couldn't create backup file: \n  $error"
				set bError 1
			}

			::Workspace::statusMessagePush "Fetching workspace files...done"
			set homePath "[file dirname ~]/[file tail ~]"
			foreach var {zip zipFile} {
				regsub -all "\~" [set $var] $homePath $var 
			}

			foreach file $fileList {
				::Workspace::statusMessagePush "Zipping file '$file'..."
				::uiEventProcess

				# Perform tilde substitution
				regsub -all "\~" $file $homePath file 
				if {[catch {exec $zip $zipFile $file} error]} {
					messageBox -ok -exclamationicon \
						"Couldn't backup file '$file': $error"
					set bError 1
				}
				::Workspace::statusMessagePush "Zipping file '$file'...done"
				::uiEventProcess
			}
			cd $oldwd
		} else {
			set bError 1
		}

		::endWaitCursor
		::uiEventProcess

		if {$bError} {
			::Workspace::statusMessagePush "Workspace back up failed"
			messageBox -ok "Workspace backup failed"
		} else {
			::Workspace::statusMessagePush "Workspace backed up"
			messageBox -ok "Workspace backup complete"
			windowClose ::WkspBackup::BackupDlg
		}
	}

	# Obtain the path to the setup log file, or "" if it doesn't exist
	proc setupLogGet {} {
		if {$::env(HOME) != ""} {
			set setupLog $::env(HOME)/.wind/setup.log
			regsub -all {\\} $setupLog {/} setupLog 
			if {![file exists $setupLog]} {
				set setupLog ""
			}
		} else {
			set setupLog ""
		}
		return $setupLog
	}

	proc driveStrip {path} {
		if {[string match [string index $path 1] ":"]} {
			return [string range $path 2 end]
		} else {
			return $path
		}
	}

	proc onCancel {} {
		windowClose ::WkspBackup::BackupDlg
	}

	proc onSelFile {} {
		# Set the path display text
		controlValuesSet ::WkspBackup::BackupDlg.filetext \
			[controlSelectionGet ::WkspBackup::BackupDlg.fileslist -string]
	}

	proc onCmdUI {} {
    	global enableflag
    	global checkflag

		set enableflag 0
		set checkflag 0
		if {[::Workspace::openWorkspaceGet] != ""} {
			set enableflag 1
		}
	}

	proc zipExeGet {binaryName} {
		if {[string match "x86-win32" [wtxHostType]]} {
			set binary ${binaryName}.EXE
		} else {
			set binary [string toupper $binaryName]
		}
		set saveBinary $binary

		switch [wtxHostType] {
			x86-win32 {
				set binary [wtxPath SETUP X86 WIN32]$binary
			}
			sun4-solaris2 {
				set binary [wtxPath SETUP SUN4 SOLARIS2]$binary
			}
			parisc-hpux10 {
				set binary [wtxPath SETUP PARISC HPUX10]$binary
			}
			default {
				set binary ""
			}
		}
		if {![file exists $binary]} {
			set binary [wtxPath]../etc/tools/[wtxHostType]/bin/$saveBinary
			if {![file exists $binary]} {
				messageBox -ok "unable to locate zip binary"
				set binary ""
			}
		}
		return $binary
	}

	# Add a menu item to the Project menu
	menuItemAppend projectMenu \
		-callback ::WkspBackup::onBackup\
		-cmduicallback ::WkspBackup::onCmdUI\
		{&Backup Workspace}
}
