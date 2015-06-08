# Dependencies.addin.tcl - file dependencies dialogs
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01u,01jul02,rbl  disable file list in the case where user has no choice
#                  anyway
# 01t,13may02,cjs  Fix spr 75899: allow user to type in a read-only dependency
#                  path; defer terrible directory search dialog
# 01s,04oct01,cjs  Fixing spr 25520 -- add errorShow method() to display
#                  dependency errors
# 01r,29mar99,jak  remove kbd shortcuts for OK and Cancel, reordered
# 01q,17mar99,rbl  adding keyboard shortcuts, made advanced dialog a child of dependencies
#                  dialog so focus is returned to dependencies dialog when advanced dialog quits
# 01p,17mar99,ren  Fixing SPR 25745: external dependendies are not updated in
#                  tree control.
# 01o,16mar99,cjs  Fixing spr 25030: Hands are tied...
# 01n,12mar99,rbl  -initialdir argument to dirBrowseDialogCreate must be in
#                  native format
# 01m,10mar99,cjs  Fixing problems in menuMatch
# 01l,02feb99,rbl  Changes to handle missing or read-only projects
# 01k,18feb99,???  fix -helpbutton
# 01j,01feb99,ren  Adding help button.
# 01i,10nov98,cjs  restore status log routine correctly 
# 01h,26oct98,cjs  update view server file pane after recalcing dependencies 
# 01g,13oct98,cjs  changed color and size of progress meter 
# 01f,28sep98,cjs  fixed Advanced... button 
# 01e,14sep98,cjs  return value for OnDependencies(); added onCancel()
# 01d,03sep98,jmp  made the dependencies dialog select current file in the
#		   dialog checklist if the selected object is a source file.
# 01c,02sep98,cjs  disabled dependObject, sourceFileObject-specific
#                  menu items 
# 01b,01sep98,jmp  added a checklist box to select project files for
#                  dependencies regeneration.
# 01a,28aug98,jmp  written.

# DESCRIPTION
# Provide context menus regenerating files dependencies
#
# RESOURCE FILES
# N/A
#
namespace eval ::Dependencies {

    # The OnDependencies() return value
    variable _retval

	proc UseQuickScan? {} {
		appRegistryEntryRead -default 0 Dependencies UseQuickScan
	}
	proc SetQuickScan { bool} {
		appRegistryEntryWrite Dependencies UseQuickScan $bool
	}

    proc menuMatch {type server handle} {

	if {[string match $type Object]} {
	    set objectType [::Object::evaluate $handle Type]
	    if {$objectType != "deadProjectObject" && \
		$objectType != "workspaceObject"} {
		set hProj [::Object::evaluate \
		    [::Object::evaluate $handle Project] Handle]
		set build  [prjBuildCurrentGet $hProj]
		set tc [prjBuildTcGet $hProj $build]
		if {[::Workspace::isToolchainInstalled $tc]} {
		    ::ViewBrowser::menuAdd "Depe&ndencies..." \
			[list ::Dependencies::OnDependencies $hProj $handle]
		} else {
		    ::ViewBrowser::menuAdd "Depe&ndencies..." {} 1	
		}
	    }
	}
    }

    proc OnEvent {cookie eventInfo} {
    }

    proc Init {} {
	::ViewBrowser::menuServerRegister ::Dependencies {object}
    }

    proc OnDependencies {hProj handle {bObviouslyStale 0}} {

	variable _retval
	set _retval "ok" 
    	set ns ::Dependencies
	set buildSpec [::prjBuildCurrentGet $hProj]

	dialogCreate -name  ${ns}::DependDlg \
	    -title "Dependencies" \
	    -width 300 -height 79 \
	    -init [list ${ns}::DependenciesDlg::onInit $hProj $handle \
		$bObviouslyStale] \
	    -controls [list \
		[list button -name okButton -title "OK" \
		    -x 243 -y 5 -w 50 -h 14 -callback \
		    [list ${ns}::DependenciesDlg::onOk $hProj $buildSpec]] \
		[list button -name cancelButton -title "Cancel" \
		    -x 243 -y 23 -w 50 -h 14 -callback \
		    [list ${ns}::DependenciesDlg::onCancel]] \
		[list button -name advancedButton -title "&Advanced..." \
		    -x 243 -y 41 -w 50 -h 14 -callback \
		    [list ${ns}::OnAdvDependencies $hProj]] \
	    	[list label -name dependLbl \
		    -title "Regenerate project file dependencies:" \
		    -x 12 -y 5 -w 220 -h 20] \
		[list choice -name choiceAll -title "A&ll Project files" \
		    -callback [list ${ns}::DependenciesDlg::onDependChoice] \
		    -auto -x 12 -y 25 -w 100 -h 10] \
		[list choice -name choiceSelect \
		    -title "&Selected Project files" \
		    -callback [list ${ns}::DependenciesDlg::onDependChoice] \
		    -auto -x 12 -y 40 -w 100 -h 10] \
		[list checklist -name fileChkLst -x 130 -y 20 -w 100 -h 53] \
		[list button -name helpButton -title "&Help" \
		    -x 243 -y 59 -w 50 -h 14 \
		    -helpbutton] \
		[list group -name depGenGroup -title \
		    "" -x 7 -y 20 -w 227 -h 40 ] \
		[list label -name lblprogress -title "" \
		    -x 16 -y 30 -w 70 -h 24] \
		[list label -name lblprogress2 -title "" \
		    -x 16 -y 30 -w 200 -h 12] \
		[list meter -name meterprogress -x 90 -y 30 -w 136 -h 22] \
	    ]

	return $_retval
    }


    proc OnAdvDependencies {hProj} {
        set ns ::Dependencies
        set buildSpec [::prjBuildCurrentGet $hProj]
        dialogCreate -name  ${ns}::DependenciesAdvDlg \
        -parent ${ns}::DependDlg \
        -title "Advanced options for dependencies generation" \
        -width 305 -height 205 \
        -init [list ${ns}::DependenciesDlg::onAdvInit $hProj $buildSpec] \
        -controls [list \
		[list group -name dependGroup -title \
		    "Read-only dependency path" -x 7 -y 2 -w 291 -h 100 ] \
		[list label -name dependLbl -title \
		    "Normally our build system time checks all file\
			dependencies (#include's) to see if the file needs to be\
			rebuilt.  This is inefficient if some of the dependencies are\
			known to be read-only.  The read-only dependency path\
			allows our build system to go faster by skipping dependencies\
			in the following directories:" \
		    -x 12 -y 15 -w 220 -h 60] \
		[list list -name dependPathLst -x 10 -y 65 -w 220 -h 40] \
		[list group -name grpQuickScan -title \
		    "Quick Scan" -x 7 -y 105 -w 291 -h 75 ] \
		[list label -name lblQuickScan -title \
			 "Quick Scan is a much faster way of analyzing dependencies,\
              but it returns a superset of the real dependencies.  If you\
              frequently regenerate dependencies, we recommend using\
              Quick Scan.  If you frequently modify the headers in\
              [file nativename [wtxPath target h]], we recommend not using\
              Quick Scan." \
			 -x 12 -y 116 -w 281 -h 46] \
		[list boolean -name chkboxQuickScan \
			 -title "&Enable Quick Scan" \
			 -callback [list ${ns}::DependenciesDlg::onQuickScan] \
			 -auto -x 12 -y 165 -w 220 -h 12 ] \
		[list button -name okButton -title "OK" \
		    -x 140 -y 187 -w 50 -h 14 -default -callback \
			 [list ${ns}::DependenciesDlg::onAdvOk $hProj $buildSpec]] \
		[list button -name cancelButton -title "Cancel" \
		    -x 194 -y 187 -w 50 -h 14 -callback \
		    [list windowClose ${ns}::DependenciesAdvDlg]] \
		[list button -name helpButton -title "&Help" \
			-x 248 -y 187 -w 50 -h 14 -helpbutton ] \
        [list button -name addButton -title "&Add..." \
            -x 235 -y 25 -w 60 -h 14 -callback \
            {::UpdateDialog::onUpdate \
                ${ns}::DependenciesAdvDlg \
                [eval ${ns}::DependenciesDlg::getInitialDir $hProj] \
                ${ns}::DependenciesDlg::onAdd \
                0 \
            } \
        ] \
		[list button -name removeButton -title "&Remove" \
		    -x 235 -y 45 -w 60 -h 14 -callback \
		    [list ${ns}::DependenciesDlg::onRemove $hProj $buildSpec]] \
		[list button -name defaultButton -title "&Default" \
		    -x 235 -y 65 -w 60 -h 14 -callback \
		    [list ${ns}::DependenciesDlg::onDefault $hProj $buildSpec]] \
	    ]
    }

    namespace eval DependenciesDlg {

        variable lastPath [wtxPath target]
	variable _prjFileTbl()

	proc onInit {hProj handle bObviouslyStale} {
	    variable _prjFileTbl
	    set ns ::Dependencies

	    set objType [::Object::evaluate $handle Type]
	    if {$objType == "sourceFileObject"} {
		set fileName [::Object::evaluate $handle Name]
	    } else {
	    	set fileName ""
	    }

	    controlHide ${ns}::DependDlg.lblprogress 1
	    controlHide ${ns}::DependDlg.lblprogress2 1
	    controlHide ${ns}::DependDlg.meterprogress 1
	    controlHide ${ns}::DependDlg.depGenGroup 1

	    controlPropertySet ${ns}::DependDlg.meterprogress \
		-background Blue -foreground Black

	    set fileList ""
	    foreach file [prjFileListGet $hProj] {
		lappend fileList [file tail $file]
		set _prjFileTbl([file tail $file]) $file
	    }

	    controlValuesSet ${ns}::DependDlg.fileChkLst [lsort $fileList]
	    set checklist [controlValuesGet ${ns}::DependDlg.fileChkLst]

	    if {$bObviouslyStale} {
		controlTextSet ${ns}::DependDlg.dependLbl \
		    "Dependencies for the following files have never been \
		    generated,\nand will be calculated now"
		controlCheckSet ${ns}::DependDlg.choiceAll	0
		controlCheckSet ${ns}::DependDlg.choiceSelect	1

		controlEnable ${ns}::DependDlg.choiceAll 0
		controlEnable ${ns}::DependDlg.choiceSelect 0
		controlEnable ${ns}::DependDlg.advancedButton 0
                controlEnable ${ns}::DependDlg.fileChkLst 0

	    	set idx 0
		set staleFiles \
		    [::Workspace::obviouslyStaleDependenciesGet $hProj]
		foreach staleFile $staleFiles {
		    lappend tmp [file tail $staleFile]
		}
		foreach item $checklist {
		    if {[lsearch $tmp [lindex $item 0]] != -1} {
		    	controlItemPropertySet ${ns}::DependDlg.fileChkLst \
			    -checkstate -index $idx 1
		    }
		    incr idx
		}
	    } elseif {$fileName == ""} {
		controlCheckSet ${ns}::DependDlg.choiceAll	1
		controlCheckSet ${ns}::DependDlg.choiceSelect	0
	    } else {
	    	set idx 0

		# only select $fileName
		controlCheckSet ${ns}::DependDlg.choiceAll	0
		controlCheckSet ${ns}::DependDlg.choiceSelect	1

		foreach item $checklist {
		    if {[lindex $item 0] == [file tail $fileName]} {
		    	controlItemPropertySet ${ns}::DependDlg.fileChkLst \
			    -checkstate -index $idx 1
		    }
		    incr idx
		}
	    }
	}

	######################################################################
	# Compares the directories in the list box agains the default directories
    # and returns Add if no default directories are in the list box, and
    # it returns Remove if one or more default directories are in the list box
	proc AddOrRemoveDefaults { } {
		variable _listReadOnlyDirectories
		# return Add iff the path already contains all defaults
		# otherwise return Remove
		set listDefaults [prjDepPathDefaults]
		# put defaults into an array to keep this whole operation O(n + m)
		foreach directory $listDefaults {
			set arDefaults($directory) {}
		}
		# now iterate through the directories in the list box, and see if
		# they are a default
		foreach directory $_listReadOnlyDirectories {
			if { [info exists arDefaults($directory)] } {
				return Remove
			}
		}
		# else
		return Add
	}

	proc AddDefaults { } {
		variable _listReadOnlyDirectories
		# put the read only directories into an array so we keep the whole
		# operation O(n + m)
		foreach directory $_listReadOnlyDirectories {
			set arReadOnlyDirectories($directory) {}
		}
		# now iterate through the defaults, and add any that are not
		# already in the list box
		foreach directory [prjDepPathDefaults] {
			if { ![info exists arReadOnlyDirectories($directory)] } {
				lappend _listReadOnlyDirectories $directory
			}
		}
	}

	proc RemoveDefaults { } {
		variable _listReadOnlyDirectories
		set listResult [list]
		# put default directories into an array to keep everything O(n + m)
		foreach directory [prjDepPathDefaults] {
			set arDefaults($directory) {}
		}
		# now iterate through the read only directories, keeping the ones
		# which are not defaults
		foreach directory $_listReadOnlyDirectories {
			if { ![info exists arDefaults($directory)] } {
				lappend listResult $directory
			}
		}
		set _listReadOnlyDirectories $listResult
	}

	proc onAdvInit {hProj buildSpec} {
	    set ns ::Dependencies
		variable _listReadOnlyDirectories
		variable _bUseQuickScan 
		set _listReadOnlyDirectories [prjDepPathGet $hProj $buildSpec]
		set _bUseQuickScan [::Dependencies::UseQuickScan?]

	    set buildSpec [::prjBuildCurrentGet $hProj]
	    controlValuesSet ${ns}::DependenciesAdvDlg.dependPathLst \
		    [NativeFileNames $_listReadOnlyDirectories]
	    controlSelectionSet ${ns}::DependenciesAdvDlg.dependPathLst \
		-noevent 0
	    controlFocusSet ${ns}::DependenciesAdvDlg.okButton
		controlCheckSet ${ns}::DependenciesAdvDlg.chkboxQuickScan \
								$_bUseQuickScan
		controlTextSet ${ns}::DependenciesAdvDlg.defaultButton \
										  "[AddOrRemoveDefaults] &Default"
	}

	proc onDependChoice {} {
	    set ns ::Dependencies

	    if {[controlChecked ${ns}::DependDlg.choiceAll]} {
		controlEnable ${ns}::DependDlg.fileChkLst 0
		set state 1
	    } else {
		controlEnable ${ns}::DependDlg.fileChkLst 1
		set state 0
	    }

	    # mark all files to $state selected|unselected
	    set checklist [controlValuesGet ${ns}::DependDlg.fileChkLst]
	    set idx 0
	    foreach item $checklist {
		controlItemPropertySet ${ns}::DependDlg.fileChkLst \
		    -checkstate -index $idx $state
		incr idx
	    }
	}

    proc onQuickScan { } { 
		::Dependencies::SetQuickScan \
			[ controlChecked ::Dependencies::DependenciesAdvDlg.chkboxQuickScan]			
	}

	proc onAdd {dirName} {
	    variable lastPath
	    set ns ::Dependencies

	    if {$dirName != ""} {
			::filePathFix dirName
			set lastPath $dirName
			
			variable _listReadOnlyDirectories
            # The following is not needed, as this happens when we write
            # the project file anyway.  Otherwise, it looks funky for the user
			# lappend _listReadOnlyDirectories [prjRelPath $hProj $dirName]
			lappend _listReadOnlyDirectories $dirName

			# reinitialize Read-only dependency directories list
			controlValuesSet ${ns}::DependenciesAdvDlg.dependPathLst \
		        [NativeFileNames $_listReadOnlyDirectories]

            # next 2 statements are hack to fix redraw bug in checklist control
			controlSelectionSet ${ns}::DependenciesAdvDlg.dependPathLst 0 
            uiEventProcess
            # this is what we really want to achieve
			controlSelectionSet ${ns}::DependenciesAdvDlg.dependPathLst \
		        [expr [llength $_listReadOnlyDirectories] -1]
	    }
	}

        proc getInitialDir {hProj} {
            set ctrl ::Dependencies::DependenciesAdvDlg.dependPathLst
            set curSel [controlSelectionGet ${ctrl} -string]
            if {$curSel == ""} {
                set curSel [file dirname [prjInfoGet $hProj fileName]]
            }
            return $curSel 
        }

	proc onAdvOk {hProj buildSpec} {
		# save the values
		variable _listReadOnlyDirectories
		set ns ::Dependencies

		prjDepPathSet $hProj $buildSpec $_listReadOnlyDirectories
		::Dependencies::SetQuickScan \
			[controlChecked ${ns}::DependenciesAdvDlg.chkboxQuickScan]
		windowClose ${ns}::DependenciesAdvDlg
	}

	proc onRemove {hProj buildSpec} {
	    set ns ::Dependencies

	    set iDir [controlSelectionGet \
		    ${ns}::DependenciesAdvDlg.dependPathLst]
		variable _listReadOnlyDirectories
		if { $iDir >= 0 && $iDir < [llength $_listReadOnlyDirectories] } {			
			set _listReadOnlyDirectories \
				[lreplace $_listReadOnlyDirectories $iDir $iDir]			

			controlValuesSet ${ns}::DependenciesAdvDlg.dependPathLst \
    			[NativeFileNames $_listReadOnlyDirectories]
			# if we delete the bottom element, move up one; otherwise 
			# high light the same one
			if { $iDir >= [llength $_listReadOnlyDirectories] } {
				incr iDir -1
			}
			controlSelectionSet ${ns}::DependenciesAdvDlg.dependPathLst 0 
            uiEventProcess -allowpaintonly
            ::endWaitCursor
			controlSelectionSet ${ns}::DependenciesAdvDlg.dependPathLst $iDir
		}
	}

	proc onDefault {hProj buildSpec} {
	    set ns ::Dependencies
		variable _listReadOnlyDirectories
		[AddOrRemoveDefaults]Defaults
		controlValuesSet ${ns}::DependenciesAdvDlg.dependPathLst \
			[NativeFileNames $_listReadOnlyDirectories]
		controlTextSet ${ns}::DependenciesAdvDlg.defaultButton \
										  "[AddOrRemoveDefaults] &Default"
	}

    namespace eval CallbackData {
        variable listFiles
        variable indexFile 0
        variable hProj
	variable dependErrors ""
    }

	proc onOk {hProj buildSpec} {
	    variable _prjFileTbl
	    set ns ::Dependencies

		controlEnable ${ns}::DependDlg.okButton 0
		controlEnable ${ns}::DependDlg.advancedButton 0

	    # Hide file list
	    controlHide ${ns}::DependDlg.fileChkLst 1
	    controlHide ${ns}::DependDlg.choiceAll 1
	    controlHide ${ns}::DependDlg.choiceSelect 1

	    # Display the progress meter
	    controlHide ${ns}::DependDlg.lblprogress 0
	    controlHide ${ns}::DependDlg.lblprogress2 0
	    controlHide ${ns}::DependDlg.meterprogress 0
	    controlHide ${ns}::DependDlg.depGenGroup 0

	    ::prjStatusLogFuncSet ${ns}::DependenciesDlg::progressShow

	    set checklist [controlValuesGet ${ns}::DependDlg.fileChkLst]

	    set files ""
	    foreach item $checklist {
		if {[lindex $item 1] == 1} {
		    lappend files $_prjFileTbl([lindex $item 0])
		}
	    }

	    # regenerate file dependencies
		catch { ::DependencyAnalyzer::ResetCache }

        set CallbackData::listFiles $files 
        set CallbackData::hProj $hProj 
        set CallbackData::indexFile 0
        set CallbackData::dependErrors "" 

		windowTimerCallbackSet ${ns}::DependDlg -milli 10 \
			[list [namespace current]::GenerateLoop \
                [namespace current]::CallbackData]
	}

	proc GenerateLoop {dataNS} {
            set ns ::Dependencies
            set thisWindow ${ns}::DependDlg
            set file [lindex [set ${dataNS}::listFiles] \
                [set ${dataNS}::indexFile]]
            set nFiles [llength [set ${dataNS}::listFiles]]
            set fileTail [file tail $file]
            prjStatusLog "generating dependencies ($fileTail)" \
                [set ${dataNS}::indexFile] $nFiles

            # Record preprocessor error and resume 
	    if {[catch {prjFileBuildInfoGen [set ${dataNS}::hProj] $file} \
	            msg]} {
                lappend ${dataNS}::dependErrors $msg
            }

            incr ${dataNS}::indexFile
            prjStatusLog "generating dependencies ($fileTail)" \
                [set ${dataNS}::indexFile] $nFiles

            if { [set ${dataNS}::indexFile] >= $nFiles } {

                # we've iterated through all the files so close up
                windowTimerCallbackSet $thisWindow 0
                catch { ::DependencyAnalyzer::ResetCache }
                ::prjStatusLogFuncSet [::defaultPrjStatusLogFuncGet] 

                set ${ns}::_retval "ok" 
                windowClose ${ns}::DependDlg

                # Tell user about any errors
                if {[llength [set ${dataNS}::dependErrors]] > 0} {
                   errorDialogShow $dataNS
                }

                # Propagate an update to the dependency folder in the
                # file view by masquerading as the file view server
                set root [lindex [::FileViewServer::objectsEnumerate "" \
                    -containers] 0]
                set prjName [::Workspace::projectNameGet \
                    [set ${dataNS}::hProj]]
                ::ViewBrowser::OnWatch ::FileViewServer::gui \
                    [list $root $prjName \
                        $::FileViewServer::_staticFolderNames(Dependencies)]
            }
	}

        proc errorDlgInit {dataNS} {
            set controls {
                {button -title OK -name ID_OK \
                    -callback \
                    {windowClose DependencyDlg::errorDlg} \
	            -xpos 100 -ypos 110 -width 50 -h 14}
                {text -name ID_ERROR_TXT -multiline -vscroll -xpos 5 -ypos 5
                    -width 240 -height 100}
            }
            foreach control $controls {
                controlCreate DependencyDlg::errorDlg $control
            }

            # Convert newlines to windows-style newlines
            # and convert all paths
            if {[wtxHostType] == "x86-win32"} {
                regsub -all "\n" [set ${dataNS}::dependErrors] \
                    "\r\n" ${dataNS}::dependErrors 
                regsub -all \/ [set ${dataNS}::dependErrors] \
                    \\ ${dataNS}::dependErrors 
            }

	    controlTextSet DependencyDlg::errorDlg.ID_ERROR_TXT \
                [lindex [set ${dataNS}::dependErrors] 0]
            messageBeep
        }

        #############################################################
        #
        # errorDialogShow 
        # Show a dialog listing all preprocessor errors 
        #
        #############################################################
        proc errorDialogShow {dataNS} {
	    set ns ::Dependencies
	    set theWindow ${ns}::DependDlg
            dialogCreate -name DependencyDlg::errorDlg -modeless \
                -title "Warning: C/C++ Preprocessor Errors" \
                -width 250 -height 130 \
                -init [list [namespace parent $dataNS]::errorDlgInit $dataNS] 
        }

	proc onCancel {} {
    	    set ns ::Dependencies
	    set ${ns}::_retval "cancel" 
	    windowClose ${ns}::DependDlg
	}

	proc progressShow {args} {

	    variable _abortCallback
	    set args [lindex $args 0]
	    set dlg ::Dependencies::DependDlg

	    set caption [lindex $args 0]
	    if {[llength $args] == 4} {
		set abortCallback [lindex $args 3]
	    } 
	    if {[llength $args] >= 3} {
		set current [lindex $args 1]
		set max [lindex $args 2]
	    } elseif {[llength $args] == 1} {
		set current 0 
		set max 0 
	    } else {
		# If the caption is "", the meter should be hidden
		controlHide $dlg.lblprogress 1
		controlHide $dlg.lblprogress2 1
		controlHide $dlg.meterprogress 1
		return
	    }

	    # If we have numbers, show the meter and the smaller label
	    if {$max} {
		controlHide $dlg.lblprogress2 1
		controlShow $dlg.lblprogress 1
		controlShow $dlg.meterprogress 1 
		controlValuesSet $dlg.meterprogress \
		    [list 0 $max $current $current] 
		uiEventProcess -allowpaintonly
	    } else {
		# Otherwise, no meter, larger label
		controlHide $dlg.lblprogress 1 
		controlHide $dlg.meterprogress 1 
		controlShow $dlg.lblprogress2 1 
	    }

	    # Set the text, if it has changed
	    set oldText [controlValuesGet $dlg.lblprogress]
	    if {![string match $oldText $caption]} {
		controlTextSet $dlg.lblprogress $caption
		controlTextSet $dlg.lblprogress2 $caption
	    }
	}
    }
}

::Workspace::addinRegister ::Dependencies
