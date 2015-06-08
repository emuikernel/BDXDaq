# File.psheet.tcl - generic property sheet for source files 
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 02u,03dec98,cjs  deleting source file should cleanup custom build rule
# 02t,24nov98,cjs  do two-way path conversion for 'Build' fields
# 02s,27oct98,cjs  Fixed bug in dependencies sheet 
# 02r,26oct98,cjs  Removed reference to obsolete DONT_BUILD file attribute
# 02q,23oct98,cjs  Add static word 'Properties:' to title 
# 02p,21oct98,cjs  display full path of selected dependency file; fix
#                  date/time display for files in prop sheets; display paths
#                  in host-friendly manner
# 02o,19oct98,cjs  quick fix to prevent boolBootFlag from
#                  appearing for files in non-VxWorks projects 
# 02n,19oct98,ren  protect logic working with boolBootFile
# 02m,19oct98,ren  re-did layout and hot-keys in dialogs
# 02l,16oct98,cjs  expose boot file flag 
# 02k,21sep98,cjs  added file size info 
# 02j,18sep98,ren  Modifed code so dataIn is readable by humans.
# 02i,14sep98,cjs  abbreviate depend file paths 
# 02h,09sep98,cjs  make build target and dependency fields in build
#                  pane readonly instead of disabled 
# 02g,02sep98,cjs  moved buildRuleSplit/Join to Utils.tcl
# 02f,02sep98,cjs  add custom build pane 
# 02e,30jul98,cjs  upgraded for new file view server architecture 
# 02d,27jul98,jak  changed label to text widget for long filenames
# 02c,24jun98,cjs  added missing handle param to controlsDestroy() 
# 02b,09jun98,cjs  added copyright, return value to dataOut()
# 02a,24apr98,cjs  ape MSVC file prop sheet; use some fakery to
#                  support sharing of controls in pages 
# 01c,09apr98,cjs  changed function signatures for better support
#                  of multiple sheets 
# 01b,31mar98,cjs  got rid of unnecessary format() statements.
# 01a,27mar98,cjs  written.
#
# DESCRIPTION
#
#############################################################

namespace eval ::FilePropertySheet {
	variable _ctrlList
	variable _fileNameMap
	variable _noDepString "<no user-defined dependencies>"

	proc titleGet {handle sheet} {
		return $sheet 
	}

	proc dateStampGet {handle} {
		return 980409
	}

	proc pageMatch {handle} {
		if {[string match [::Object::evaluate $handle Type] \
			sourceFileObject]} {
			return [list 1 {Dependencies General Build}] 
		} else {
			return 0
		}
	}

	proc OnShow {windowName sheet handle} {
		set fileName [::Object::evaluate $handle DisplayName]
		windowTitleSet $windowName "Properties: source file '$fileName'" 

		variable _ctrlList
		foreach ctrl $_ctrlList($sheet) {
			controlHide [format "%s.%s" $windowName \
				[::flagValueGet $ctrl -name]] 0
		}

		# XXX -- is is a quick and dirty fix that ought to be rethought
		if {[string match Build $sheet]} {

			# If this is a VxWorks app, show the boot file checkbox
			set hProj [::Object::evaluate \
				[::Object::evaluate $handle Project] Handle]
			if {[::prjHasComponents $hProj]} {
				controlHide ${windowName}.boolBootFile 0
			} else {
				controlHide ${windowName}.boolBootFile 1
			}
		}
	}

	proc OnHide {windowName sheet handle} {
		variable _ctrlList
		foreach ctrl $_ctrlList($sheet) {
			controlHide [format "%s.%s" $windowName \
				[::flagValueGet $ctrl -name]] 1
		}
	}

	proc controlsCreate {windowName sheet handle} {
		variable _ctrlList
		variable _fileNameMap
		if {[string match $sheet "General"]} {
			set leftMargin 11
			set indentedMargin 15
			set labelVspace 12
			set labelPos 46
			set _ctrlList($sheet) [list \
				[list label -name lblpath -x $leftMargin -y 26 -w 70 -h 10 \
				    -title "&Source file:"] \
        	    [list text -name txtpath -x $leftMargin -y 38 -width 210 -h 14 \
 				    -readonly ] \
				[list label -name lblattributes -x $indentedMargin \
				    -y [incr labelPos $labelVspace] -w 200 -h 10 -title ""] \
				[list label -name lblsize -x $indentedMargin \
					 -y [incr labelPos $labelVspace] -w 200 -h 10 -title ""] \
				[list label -name lblatime -x $indentedMargin \
					 -y [incr labelPos $labelVspace] -w 200 -h 10 -title ""] \
			]
		} elseif {[string match $sheet "Build"]} {
			set ns ::FilePropertySheet
			set hProj [::Object::evaluate \
				[::Object::evaluate $handle Project] Handle]
			set fileName [::Object::evaluate $handle Name] 

			set _ctrlList($sheet) [list \
				[list label -name tgtTxt -title "Build &target" \
					-x 14 -y 38 -w 68 -h 10] \
				[list text -name ruleName -x 14 -y 48 -w 68 -h 14 \
					-callback [list ${ns}::onRuleDing $windowName ruleName]] \
				[list label -name depTxt -title "Custom &dependencies" \
					-x 86 -y 38 -w 145 -h 10] \
				[list text -name ruleDepend -x 86 -y 48 -w 145 -h 14 \
					-callback [list ${ns}::onRuleDing \
						$windowName ruleDepend]] \
				[list label -name cmdTxt -title "&Commands" \
					-x 14 -y 69 -w 40 -h 10] \
				[list text -name ruleCommands -x 54 -y 64 -w 177 -h 36 \
					-callback [list ${ns}::onRuleDing \
						$windowName ruleCommands] \
					-vscroll -multiline] \
				[list boolean -name boolDefaultBuild \
					-title "&Use default build rule for this file" \
					-callback [list ::FilePropertySheet::onUseDefaultBuild \
						$windowName] \
					-auto -x 14 -y 108 -w 147 -h 10] \
				[list label -name lblRuleDesc \
					-title "Enter dependencies and commands for this file" \
 					-x 14 -y 20 -w 180 -h 12] \
				[list label -name ruleSeparator -title ":" \
					-x 83 -y 49 -w 2 -h 14] \
				[list boolean -name boolBootFile \
					-title "Build for ROM images only" \
					-callback [list ::FilePropertySheet::onBootFile \
						$windowName] \
					-auto -x 14 -y 120 -w 147 -h 10 \
				] \
			]
		} elseif {[string match $sheet "Dependencies"]} {
			catch {unset _fileNameMap}
			set _ctrlList($sheet) [ list \
				[list label -name lbltool -x 10 -y 26 -w 76 -height 14 \
					-title "&Tool:"] \
				[list text -name listtool -readonly -x 10 -y 36 \
					-width 56 -height 24] \
				[list label -name lblfiles -x 72 -y 26 -width 120 -height 14 \
					-title "Calculated &dependencies"] \
				[list list -name listfiles -x 72 -y 36 -width 156 -height 55 \
					-callback [list ::FilePropertySheet::onSel \
					$windowName $sheet]] \
				[list text -name textdepname -x 72 -y 90 -width 156 \
					-height 12 -readonly] \
				[list label -name lblfileatime -x 72 -y 104 -width 156 \
					-height 10 -title ""] \
			]
		}

		foreach ctrl $_ctrlList($sheet) {
			set ctrlName [::flagValueGet $ctrl -name]
			if {![controlExists ${windowName}.$ctrlName]} {
				controlCreate $windowName $ctrl
			}
		}
		OnHide $windowName $sheet $handle
	}

	##########################################################################
	#
	# onSel - callback for dependencies file list
	#
	proc onSel {windowName sheet} {
		variable _fileNameMap
	
		set file [controlSelectionGet ${windowName}.listfiles -string]
		if {[info exists _fileNameMap($file)]} {
			set file $_fileNameMap($file)
			set setVal ""
			if {$file != ""} {
				if {[file exists $file]} {
					set setVal [mTimeGet $file]
				}
			} else {
				set setVal "(file not found)"
			}
			controlValuesSet ${windowName}.lblfileatime $setVal 

			::filePathUnFix file
			controlTextSet ${windowName}.textdepname $file 
		} else {
			controlTextSet ${windowName}.textdepname "<no current selection>"
		}
	}
			
	proc controlsDestroy {windowName sheet handle} {
		variable _ctrlList
		foreach ctrl $_ctrlList($sheet) {
			set name [::flagValueGet $ctrl -name]
			if {[controlExists ${windowName}.$name]} {
				controlDestroy ${windowName}.$name
			}
		}
	}

	##########################################################################
	#
	# Build rules normally associate with files via the 'objects' attribute
	# of a source file.  Those objects are built using default rules.
	# For source files with custom builds, the 'customRule' attribute is
	# set on the source file.  A
	# custom build rule and commands is then added.  This rule is written
	# to the makefile, and added to the LDDEPS macro to insure that the
	# new rule (build target) is a dependency of the link.
	#
	##########################################################################


	##########################################################################
	#
	# defaultBuildRuleSet - set the default build rule for a source file
	# Removes previous custom rule and reference in LDDEPS, if any
	#
	proc defaultBuildRuleSet {hProj fileName} {

		# Remove any old custom rule and reference in the LDDEP macro
		::Workspace::customRuleDelete $hProj $fileName
	}

	##########################################################################
	#
	# customBuildRuleSet - set a custom build rule for a source file
	#
	proc customBuildRuleSet {hProj fileName ruleName cmds} {
		set buildSpec [::prjBuildCurrentGet $hProj]
		::Workspace::customRuleAdd $hProj $buildSpec $fileName $ruleName $cmds
	}

	##########################################################################
	#
	# customBuildCommandsGet - get the build commands in the following format
	# 	see buildRuleSplit for form of return value
	#
	proc customBuildCommandsGet {hProj rule} { 
		set buildSpec [::prjBuildCurrentGet $hProj]
		set ruleCommands [::prjBuildRuleGet $hProj $buildSpec $rule]
		return $ruleCommands
	}

	##########################################################################
	#
	# defaultDependenciesGet -
	#
	proc defaultDependenciesGet {hProj fileName} {
		set dependencies [::prjFileInfoGet $hProj $fileName dependencies]
		return $dependencies
	}

	##########################################################################
	#
	# ruleDisplayUpdate - populates the build target, dependencies
	# and commands fields
	#
	proc ruleDisplayUpdate {windowName hProj fileName bUseDefaultRule} {
		set buildSpec [::prjBuildCurrentGet $hProj]

		if {$bUseDefaultRule} {
			set targetName [::prjFileInfoGet $hProj $fileName objects] 
			set ruleCommands [::prjFileInfoGet $hProj $fileName tool] 
			regsub " " $ruleCommands "_" ruleCommands
			set tool [::prjFileInfoGet $hProj $fileName tool]
			if {$tool == ""} {
				set rules "(no default tool available)"
				controlTextSet ${windowName}.lblRuleDesc \
					"Warning: don't know how to build this file"
			} else {
				set rules "($tool will be used)" 
				controlTextSet ${windowName}.lblRuleDesc \
					"This file will be built using the default rule"
			}
			::filePathUnFix targetName
			set dependencies \
				[::prjFileInfoGet $hProj $fileName dependencies] 
			::filePathUnFix dependencies
		} else {
			set targetName [::prjFileInfoGet $hProj $fileName customRule]
			if {$targetName == ""} {
				set targetName [::prjFileInfoGet $hProj $fileName objects]
			}
			set ruleCommands \
				[::prjBuildRuleGet $hProj $buildSpec $targetName]
			set split [::buildRuleSplit $ruleCommands]
			::filePathUnFix targetName 
			set rules [lindex $split 2]
			::filePathUnFix rules
			set dependencies [lindex $split 1]
			::filePathUnFix dependencies 
			controlTextSet ${windowName}.lblRuleDesc \
				"Enter dependencies and commands for this file"
		}

		# Get rid of enclosing braces
		if {$dependencies != ""} {
			regsub -all {\{} $dependencies {} dependencies
			regsub -all {\}} $dependencies {} dependencies
		} else {
			set depList "<no user-defined dependencies>"
		}
		controlValuesSet ${windowName}.ruleDepend $dependencies
   		controlValuesSet ${windowName}.ruleName $targetName
		controlValuesSet ${windowName}.ruleCommands $rules
	}


	proc dataIn {windowName handle sheet} {

		::beginWaitCursor
		set fileName [::Object::evaluate $handle Name]
		
		fillSheet$sheet $windowName $handle $sheet $fileName

		::endWaitCursor
	}

	proc fillSheetGeneral { windowName handle sheet fileName } {
		# Set the full path of the file
		set tmp $fileName
		filePathUnFix tmp
		controlTextSet ${windowName}.txtpath $tmp

		# Set the file's attributes
		controlValuesSet ${windowName}.lblattributes \
			"Attributes: [ ::FileLaunch::fileDescribe $fileName]"

		# Set the file's size
		controlValuesSet ${windowName}.lblsize \
			[::FileLaunch::fileSizeGet $fileName]

		# Set the file's timestamp
		controlValuesSet ${windowName}.lblatime [mTimeGet $fileName]
	}


	proc fillSheetBuild { windowName handle sheet fileName } {
		set hProj [::Object::evaluate \
				   [::Object::evaluate $handle Project] Handle]
		set fileName [::Object::evaluate $handle Name]
		if {"[prjFileInfoGet $hProj $fileName customRule]" != ""} {
			set checkState 0
		} else {
			set checkState 1
		}
		controlCheckSet ${windowName}.boolDefaultBuild $checkState
		
		set state [::prjFileInfoGet $hProj $fileName BOOT_FILE]
		if {$state != ""} {
			set checkState 1
		} else {
			set checkState 0
		}
		controlCheckSet ${windowName}.boolBootFile $checkState

		# Override the callback's desire to set the dirty flag
		::PSViewer::dirtyFlagSet 0
	}

	proc fillSheetDependencies { windowName handle sheet fileName } {
		variable _fileNameMap
		catch {unset _fileNameMap}
		
		# Indicate whether this file is built at all, and if so
		# how
		set projObj [::Object::evaluate $handle Project]
		set hProj [::Object::evaluate $projObj Handle]
		set buildSpec [::prjBuildCurrentGet $hProj]
		set tool [::prjFileInfoGet $hProj $fileName tool] 
		regsub " " $tool "_" tool
		
		set rule [::prjFileInfoGet $hProj $fileName customRule]
			
		# If this file is not built by default using the default tool...
		if {$rule != ""} {
				
			# It has a custom rule
			set tool "Custom build"
			set commands [::prjBuildRuleGet $hProj $buildSpec $rule]
			set split [buildRuleSplit $commands]
		} elseif {$tool != ""} {
			# It's slated to be built by default
			# Use the default tool
		} else {
			# There is no default tool; this will cause a build error
			set tool "None!"
			messageBox -ok "Warning: system doesn't know how to build \
					\n'[file tail $fileName]'!"
		}
		set fileList [::prjFileInfoGet $hProj $fileName dependencies]
		
		# Abbreviate the file names and store the originals
		foreach file $fileList {
			set _fileNameMap([file tail $file]) $file
		}
		if {[info exists _fileNameMap]} {
			set fileList [array names _fileNameMap]
			set fileList [lsort $fileList]
		} else {
			set fileList {{<no calculated dependencies>}}
		}
		
		# Set a current selection in the dep list and populate
		# the file stat label
		controlValuesSet ${windowName}.listfiles $fileList
		controlValuesSet ${windowName}.listtool $tool
		controlSelectionSet ${windowName}.listfiles 0
	}

	proc dataOut {windowName handle sheet} {

		if {[string match $sheet "Build"]} {
			set hProj [::Object::evaluate \
				[::Object::evaluate $handle Project] Handle]
			set fileName [::Object::evaluate $handle Name]

			# Utilize 'build for ROM images only'
			set buildSpec [::prjBuildCurrentGet $hProj]
			set tc [::prjBuildTcGet $hProj $buildSpec]
			set objFile [::prjFileInfoGet $hProj $fileName objects]

			if {![controlHidden ${windowName}.boolBootFile]} {
				set state [controlChecked ${windowName}.boolBootFile]

				if {$state} {
					# Set boot file flag
					::prjFileInfoSet $hProj $fileName BOOT_FILE TRUE
				} else {
					# Remove boot file flag
					::prjFileInfoDelete $hProj $fileName BOOT_FILE
				}
			}
			if {[controlChecked ${windowName}.boolDefaultBuild]} {

				# Delete custom build data
				set fileName [::Object::evaluate $handle Name]
				defaultBuildRuleSet $hProj $fileName

			} else {
	
				# Write custom build data
				set ruleName [controlValuesGet ${windowName}.ruleName]
				::filePathFix ruleName
				set dependencies [controlValuesGet ${windowName}.ruleDepend]
				if {[string match $::FilePropertySheet::_noDepString \
					$dependencies]} {
					set dependencies ""
				}
				::filePathFix dependencies
				set commands [controlValuesGet ${windowName}.ruleCommands]
				::filePathFix commands 

				# Add rule to project
				set rule [::buildRuleJoin $ruleName $dependencies $commands]
				customBuildRuleSet $hProj $fileName $ruleName $rule
			}
		}
		return 1
	}

	##########################################################################
	#
	# sourceFileRule - return the source file this rule corresponds to, or "" 
	#
	proc sourceFileRule {hProj ruleName} {
		set files [::prjFileListGet $hProj]
		set retval ""
		foreach file $files {
			set customRule [::prjFileInfoGet $hProj $file customRule]
			if {$ruleName != ""} {
				if {[string match $customRule $ruleName]} {
					set retval $file
					break
				}
			}
		}
	    return $retval 
	}

	##########################################################################
	#
	# onUseDefaultBuild - callback for 'use default build' checkbox
	#
	proc onUseDefaultBuild {windowName} {
		set state [controlChecked ${windowName}.boolDefaultBuild]
		foreach ctrl {ruleName ruleDepend ruleCommands} {
			if {$state == 1} {
				controlPropertySet ${windowName}.$ctrl -readonly 1 
			} else {
				controlPropertySet ${windowName}.$ctrl -readonly 0
			}
		}

		set handle [::Workspace::selectionGet]
		set hProj [::Object::evaluate \
			[::Object::evaluate $handle Project] Handle]
		set fileName [::Object::evaluate $handle Name]
		ruleDisplayUpdate $windowName $hProj $fileName $state

		::PSViewer::dirtyFlagSet 1 
	}

	##########################################################################
	#
	# onBootFile - callback for 'build for ROM images only' checkbox
	#
	proc onBootFile {windowName} {
		::PSViewer::dirtyFlagSet 1 
	}

	##########################################################################
	#
	# onRuleDing - callback for edit of any of the rule fields so that
	# result can be saved
	#
	proc onRuleDing {windowName ctrl} {
		if {![controlPropertyGet ${windowName}.$ctrl -readonly] == 1} {
			::PSViewer::dirtyFlagSet 1
		}
	}

	# This must be done last, to allow interface validation
	if {[catch {::Workspace::propPageRegister \
		::FilePropertySheet} \
		errorMsg]} {
		messageBox $errorMsg
	}
}
