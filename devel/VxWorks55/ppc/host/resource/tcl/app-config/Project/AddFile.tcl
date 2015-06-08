# AddFile.tcl - Code for the 'Add File from Project' menu item from the
# FileViewServer 
#
# modification history
# --------------------
# 01e,24mar99,cjs  Fixing spr 25688
# 01d,16mar99,cjs  Fixing 'Add Files From Project' to accommodate new
#                  top-level project object
# 01c,01feb99,ren  Adding help button.
# 01b,10nov98,cjs  restore status log routine correctly 
# 01a,20oct98,cjs  Written.
# 
#############################################################

set ns ::FileViewServer::gui::fileCopyDlg
namespace eval $ns { 
	proc projectsGet {} {
		set workspace [::Workspace::objectsEnumerate "" -containers]
		if {$workspace != ""} {
			set projectList [::Workspace::objectsEnumerate \
				$workspace -containers]

			# objectsEnumerate() returns full paths, so we must collect just
			# the leaves
			foreach project $projectList {
				lappend projectLeaves [lindex $project \
					[expr [llength $project] - 1]]
			}
		}
		if {![info exists projectLeaves]} {
			set projectLeaves ""
		}
		return $projectLeaves
	}
	
	proc OnFilesFromProjectCopy {event handle} {
		# Figure out if we have more than one project in the workspace.
		# If not, abort 
		if {[llength [projectsGet]] > 1} {
			dialogCreate -name fileCopyDlg -title \
				"Add Files from Another Project in Workspace" \
				-w 286 -h 138 -init [list [namespace current]::onInit $handle] \
				-controls [list \
					[list combo -name srcprjcombo -x 14 -y 18 -w 200 -h 100 \
						-callback [namespace current]::onSelChange] \
					[list combo -name destprjcombo -x 14 -y 108 -w 200 -h 100] \
					[list checklist -name filelist -x 14 -y 46 -w 200 -h 52] \
					[list button -default -name okbutton \
						-title &OK -x 229 -y 7 \
						-w 50 -h 14 -callback [namespace current]::onOk] \
					[list button -name cancelbutton -title "&Cancel" \
						-x 229 -y 24 \
						-w 50 -h 14 -callback {windowClose fileCopyDlg}] \
					[list button -name helpbutton -helpbutton -x 229 -y 41 \
						 -w 50 -h 14 -title "&Help"] \
					[list group -name grp1 \
						-title "Source Project" \
						-x 7 -y 7 -w 213 -h 85] \
					[list label -name lbl1 \
						-title "Check files to be added" \
						-x 14 -y 36 -w 100 -h 8] \
					[list group -name grp2 \
						-title "Destination Project" \
						-x 7 -y 97 -w 213 -h 33] \
				]
		} else {
			messageBox -ok -exclamationicon \
				"You can only add files from\
				another project in your workspace.\
				\nYour workspace contains only one project."
		}
	}
	
	proc onInit {handle} {
		::prjStatusLogFuncSet [::defaultPrjStatusLogFuncGet] 
		set workspace [::Workspace::objectsEnumerate "" -containers]
		set projectList [::Workspace::objectsEnumerate \
			$workspace -containers]

		# objectsEnumerate() returns full paths, so we must collect just
		# the leaves
		foreach project $projectList {
			lappend projectLeaves [lindex $project \
				[expr [llength $project] - 1]]
		}
		if {[info exists projectLeaves]} {
			set projectList $projectLeaves
		} else {
			set projectList ""
		}

		# Identify a current project for the destination project list
		if {![catch {::Object::evaluate $handle Project} projObj]} {
			set destinationProject [::Object::evaluate $projObj Name]
		} else {
			set destinationProject [lindex $projectList 0]
		}

		# Identify a current project for the source project list.
		# Obviously, we don't want the same one as the destination.
		set idx [lsearch $projectList $destinationProject]
		if {$idx == [expr [llength $projectList] - 1]} {
			set idx 0
		} else {
			incr idx
		}
		set sourceProject [lindex $projectList $idx]
		controlValuesSet fileCopyDlg.srcprjcombo $projectList 
		controlSelectionSet fileCopyDlg.srcprjcombo -string $sourceProject 
		controlValuesSet fileCopyDlg.destprjcombo $projectList
		controlSelectionSet fileCopyDlg.destprjcombo \
			-string $destinationProject
		::prjStatusLogFuncSet [::defaultPrjStatusLogFuncGet] 
	}

	proc onSelChange {} {
		set srcProjName [controlSelectionGet \
			fileCopyDlg.srcprjcombo -string]
		if {$srcProjName != ""} {

			set hSrcPrj [::Workspace::projectHandleGet $srcProjName]
			set fileList [::prjFileListGet $hSrcPrj]
			catch {unset valueList}
			foreach file $fileList {
				lappend valueList [list $file 1]
			}
			controlValuesSet fileCopyDlg.filelist "" 
			if {[info exists valueList]} {
				controlValuesSet fileCopyDlg.filelist $valueList
				controlEnable fileCopyDlg.okbutton 1
			} else {
				controlEnable fileCopyDlg.okbutton 0
			}
		}
	}

	proc onOk {} {
		set destProjName [controlSelectionGet \
			fileCopyDlg.destprjcombo -string]
		set fileStructs [controlValuesGet fileCopyDlg.filelist]
		if {$fileStructs != ""} {
			catch {unset fileList}
			foreach struct $fileStructs {
				if {[lindex $struct 1]} {
					lappend fileList [lindex $struct 0]
				}
			}

			# Add file references to project
			foreach file $fileList {
				if {[catch {::Workspace::fileAdd ::FileViewServer \
					$destProjName $file} error]} {
					messageBox -ok $error
				}
			}
		}
		
		windowClose fileCopyDlg
	}
}
