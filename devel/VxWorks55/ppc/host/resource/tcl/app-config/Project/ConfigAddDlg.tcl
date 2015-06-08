# ConfigAddDlg.tcl - add a build config or component set
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01s,07jan02,rbl  rollback last change - bootable projects not ready yet to
#                  be built with more than one compiler - e.g. LD_LINK_PATH is
#                  hard-coded to one or the other...
# 01r,29oct01,rbl  enable user to compile bootable project with more than one
#                  compiler (e.g. gnu and diab)
# 01q,08apr99,cjs  Make build spec combo taller for spr 26459
# 01p,01feb99,ren  Adding help button.
# 01o,25jan99,cjs  new layout for add new build spec dialog (spr 23663)
# 01n,21jan99,cjs  On error, don't close dialog
# 01m,21jan99,cjs  fixed typo in an error message that produced an error
# 01l,09dec98,cjs  protect user from adding a build w/ different tc to a
#                  vxWorks project; beefed up graying logic
# 01k,25nov98,cjs  lengthen width of text
# 01j,28oct98,cjs  convert project selection combo into read-only text;
#                  changed tab order; removed component set logic
# 01i,15oct98,cjs  changed title of dialog for builds
# 01h,12oct98,ren  made dialog box look professional.  things line up like
#                  they should.  Logical tab order.  Hot keys.
# 01g,06sep98,cjs  replace 'projectsFromWorkspace()' with
#                  objectsEnumerate(); set a default toolchain
# 01f,04sep98,cjs  fixed add default build for toolchain 
# 01e,01aug98,jak  added validation of buildnames - removal of spaces 
# 01d,31jul98,cjs  restored default option for builds (default for tc)
# 01c,30jul98,cjs  removed default option for builds; changed
#                  references to iHierarchyView to 'gui'
# 01b,24jun98,cjs  modified to use revised build and component
#                  view server objects 
# 01a,23jun98,cjs  written 
#
# DESCRIPTION
# This file is sourced by the component view server. It operates
# in one of two modes, and allows the user to add either a 
# component set or a build specification to a project.  The new
# configuration can be copied from an existing one, or it can
# be created blank.

#############################################################

namespace eval ::ComponentViewServer::gui::AddCSetDlg {
	proc AddComponentSetDlgShow {handle} {
		set ::AddConfigDlg::_handle $handle 
		::AddConfigDlg::AddConfigDlgShow "Add New Component Set"
	}
}

namespace eval ::BuildViewServer::gui::AddBuildDlg {
	proc AddBuildConfigDlgShow {handle} {
		set ::AddConfigDlg::_handle $handle 
		::AddConfigDlg::AddConfigDlgShow "Add New Build Specification"
	}
}

namespace eval ::AddConfigDlg {
	variable _handle
	variable _tcMap
	
	proc AddConfigDlgShow {title} {
		set controls [list \
			[list label -title "&Name" -x 13 -y 16 \
				-width 32 -height 10] \
			[list text -name textbuildspecname -x 49 -y 14 \
				-width 212 -height 12] \
			[list combo -name combotoolchain -x 111 -y 98 \
				-width 150 -height 40] \
			[list choice -name choicecopy -title "Build Spec in Cu&rrent Project" \
				-auto -x 14 -y 49 -width 93 -height 10 \
				-callback [list ::AddConfigDlg::OnChoice choicecopy]] \
			[list label -name lblproject -title &Project -x 43 -y 61 \
				-right -width 32 -height 10] \
			[list text -name textprojectname -x 83 -y 60 \
				-width 178 -height 12 -readonly] \
			[list label -name lblconfiguration -title "&Build" \
				-right -x 43 -y 77 -width 32 -height 10] \
			[list combo -name combobuild -x 83 -y 76 \
				-width 178 -height 48 \
					-callback ::AddConfigDlg::controlStateSet] \
			[list choice -name choicedefault \
				-title "&Default Build Spec for tool chain" \
				-auto \
				-x 14 -y 99 -width 80 -height 10 \
				-callback [list ::AddConfigDlg::OnChoice choicedefault]] \
			[list group -name groupcreate -title "Create from" -x 7 -y 35 \
				-width 262 -height 83] \
			[list button -default -name okbutton -title &OK -x 110 -y 124 \
				-width 50 -height 14 -callback ::AddConfigDlg::OnOk] \
			[list button -name cancelbutton -title &Cancel -x 164 -y 124 \
				-width 50 -height 14 -callback ::AddConfigDlg::OnCancel] \
			[list button -name helpbutton -title &Help -x 218 -y 124 \
  			    -width 50 -height 14 -helpbutton] \
		]
 
		dialogCreate -name ::AddConfigDlg::AddConfigDlg -title $title \
			-width 276 -height 145 -init ::AddConfigDlg::OnInit \
			-controls $controls
	}

	proc OnInit {} {
		variable _handle
		variable _tcMap

		# Set the label describing what it is that can be copied,
		# and give a default name for the config
		set projName [::Object::evaluate \
			[::Object::evaluate $_handle Project] Name]
		controlTextSet ::AddConfigDlg::AddConfigDlg.lblconfiguration "&Build"
		controlHide ::AddConfigDlg::AddConfigDlg.groupcreate 0
		controlHide ::AddConfigDlg::AddConfigDlg.combotoolchain 0

		# Set the radio buttons and control states
		controlCheckSet ::AddConfigDlg::AddConfigDlg.choicecopy 1
		controlCheckSet ::AddConfigDlg::AddConfigDlg.choicedefault 0

		# List projects in this workspace in the project combo
		if {![string match [::Object::evaluate $_handle Type] nullObject]} {
			set currentProject [::Object::evaluate \
				[::Object::evaluate $_handle Project] Name]
			controlTextSet ::AddConfigDlg::AddConfigDlg.textprojectname \
				$currentProject

			buildNameFix
			set hProj [::Workspace::projectHandleGet $currentProject]

			# Populate the toolchain combo and set it current selection
			set type [::prjTypeGet $hProj]
                        set build [::prjBuildCurrentGet $hProj]
                        set currentTc [::prjBuildTcGet $hProj $build]
                        set currentCpu [::${currentTc}::cpu]
			set tcList [${type}::toolChainListGet]
			foreach tc $tcList {
				set name [${tc}::name]
				set _tcMap($name) $tc

                                # for vxWorks projects, 
                                # only allow toolchains with matching cpu
                                # for now

                                if {$type != "::prj_vxWorks" || \
                                    [${tc}::cpu] == $currentCpu} {
				    lappend names $name
                                }
			}
			set names [lsort $names]
			controlValuesSet ::AddConfigDlg::AddConfigDlg.combotoolchain $names

			# Populate the list w/ the builds for this project;
			# also set the current selection
			set values [::Workspace::buildSpecsForProject $currentProject]
			set current [::prjBuildCurrentGet $hProj]

			controlValuesSet ::AddConfigDlg::AddConfigDlg.combobuild $values 
			controlSelectionSet ::AddConfigDlg::AddConfigDlg.combobuild \
				-string $current; # This implicitly calls controlStateSet()
		}
	}

	proc OnChoice {control} {
		buildNameFix
		controlStateSet
	}

	proc controlStateSet {} {
		variable _handle
		set currentProject [::Object::evaluate \
			[::Object::evaluate $_handle Project] Name]
		set hProj [::Workspace::projectHandleGet $currentProject]
		set prjType [::prjTypeGet $hProj]

		# Select the toolchain corresponding to the active build
		# This implicitly calls controlStateSet()
		set build [controlSelectionGet \
			::AddConfigDlg::AddConfigDlg.combobuild -string]
		if {$build == ""} {
			set build [::prjBuildCurrentGet $hProj]
		}
		set tc [::prjBuildTcGet $hProj $build]
		set tcName [${tc}::name]
		set names [controlValuesGet \
			::AddConfigDlg::AddConfigDlg.combotoolchain]
		set index [lsearch $names $tcName] 
		if {$index == -1} {
			set index 0
		}
		controlSelectionSet \
			::AddConfigDlg::AddConfigDlg.combotoolchain $index 

		# Tool chain combo should only be enabled if project type
		# is downloadable
		if {[string match ::prj_vxApp $prjType]} {
			set tcEnableState 1
		} else {
			set tcEnableState 0
		}

		# Project name and build combo should only be enabled if "Copy From"
		# is checked
		if {[controlChecked ::AddConfigDlg::AddConfigDlg.choicedefault]} {
			set buildEnable 0
		} else {
			set buildEnable 1

			# If we are copying a build from a project, the toolchain selection
			# should be disabled
			set tcEnableState 0
		}
		controlEnable ::AddConfigDlg::AddConfigDlg.textprojectname $buildEnable
		controlEnable ::AddConfigDlg::AddConfigDlg.combobuild $buildEnable
		controlEnable ::AddConfigDlg::AddConfigDlg.combotoolchain \
			$tcEnableState 
	}

	proc OnOk {} {
		variable _handle
		variable _tcMap

		buildNameFix
		set projName [::Object::evaluate \
			[::Object::evaluate $_handle Project] Name]
		set hProj [::Workspace::projectHandleGet $projName]
		set configName [controlTextGet \
			::AddConfigDlg::AddConfigDlg.textbuildspecname]

		# Bail if name is default
		if {$configName == ""} {
			messageBox -ok "Please choose a name"
			return
		}

		set configList [::Workspace::buildSpecsForProject $projName]
		if {[lsearch $configList $configName] != -1} {
			messageBox -ok \
				"'$configName' already exists; please choose a new name"
		} else {
			set tc [controlSelectionGet \
				::AddConfigDlg::AddConfigDlg.combotoolchain -string]
			set tc $_tcMap($tc)
			set currentProject [::Object::evaluate \
				[::Object::evaluate $_handle Project] Name]

			set ctrl ::AddConfigDlg::AddConfigDlg.choicedefault
			if {![controlChecked $ctrl]} { 
				set configFrom [controlSelectionGet \
					::AddConfigDlg::AddConfigDlg.combobuild -string]
				if {$configFrom != ""} {
					::Workspace::buildSpecCopy \
						::BuildViewServer::gui \
						$projName $configFrom $configName
				}
			} else {
				::Workspace::buildSpecAdd ::AddConfigDlg \
					$currentProject $configName $tc
			}
			windowClose ::AddConfigDlg::AddConfigDlg
		}
	}

	proc OnCancel {} {
		windowClose ::AddConfigDlg::AddConfigDlg
	}

	proc buildNameFix {} {
		set name [controlTextGet ::AddConfigDlg::AddConfigDlg.textbuildspecname]
		set name [string trimright $name]
	    regsub -all " " $name "_" name
		controlTextSet ::AddConfigDlg::AddConfigDlg.textbuildspecname $name
	}
}
