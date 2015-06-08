# 01Launch.win32.tcl - Tornado (Windows) Tool Launching
#
# Copyright (C) 1995-2002 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 02d,15apr02,fle  SPR#73266 : added note about this SPR in error messages
# 02c,22jun01,tpw  Utilize targetGdbSuffix to centralize exception cases for
#                  GDB naming.
# 02c,31may01,kab  Fixed bug in call to setSourceSearchPaths
# 02b,10mar99,jak  add option to unregister ts if not responding
#		   fixed bug in ext. editor launch for filenames starting with 0
# 02a,05mar99,cjs  fix bug in dbgSaveStateDirSet
# 01z,10feb99,l_k  onEditorContextMenu SPR 24576
# 01y,08feb99,dbt  fixed a syntax error in isTgtSvrSelectable() routine.
# 01x,02feb99,c_c  Fixed a target server thread leak (SPR #24847).
# 01w,25jan99,jak  enhanced error handling for tgtsvr combo
# 01v,15jan99,jak  fix checkTarget, validate tgt before selection,
#					non-gui tgtcheck proc isTgtSvrSelectable
# 01u,11jan99,jak  add local registry auto-launch at start-up if reqrd
# 01t,24nov98,j_k  Part of fix for SPR #23267.
# 01s,24nov98,jak  change text appearing when Tornado registry is invalid.
# 01r,13nov98,l_k  (for d_s) dbgSaveStateDirSet  defaultDir "\"\""
# 01q,05nov98,ren  delayed displaying all message boxes until after callback
#                  fixes SPR #22924
# 01p,03nov98,j_k  changed the tooltip for debugger button to say debugger
#                  rather than cw
# 01n,01oct98,ren  delayed displaying error message box until after callback
# 01m,25sep98,j_k  editor callback now sends the mouse pointer position.
# 01l,23sep98,cjs  moved download code to Download.tcl, where it can be
#                  shared 
# 01k,23sep98,j_k  implement showing errors while downloading.
# 01k,22sep98,j_k  added cpu as an argument so that it is validated 
#                    before downloading the object.
# 01j,14sep98,j_k  added download progress/cancel functionality.
# 01i,14aug98,cjs  modified onDownloadFiles
# 01h,31jul98,jak  changed onOpenFile to accept macros in any order.
#					added version control options
# 01g,17jul98,j_k  now uses cmdui callback to update toolbar buttons stop using
#                  launcherGlobals(targetChangeNotifiers) for enabling/disabling
#                  toolbar buttons
# 01f,29jun98,j_k  started work on moving debugger implementation into Tcl.
# 01e,24jun98,jak  added a select_checkTgtSvr as external hook 
# 01d,22jun98,j_k  added a common download routine.
# 01c,20jun98,j_k  added editor context menu customizations.
#                  removed licence manager related code.
# 01b,21jan98,fle  updated checkTarget{} since new error message SVR_IS_DEAD can
#                  happen.
#                  + made tgtServerListGet{} more simple
# 01a,24apr96,j_k  written. Broken this file into two logical pieces.
#

#
# globals
#
# currently selected target server in lauch toolbar

set launcherGlobals(tgtsvr)	""
set launcherGlobals(extEditorCmd)	""
set launcherGlobals(bExtEditorSetupOk) 0

set torGlobal(editorContextMenuCbs) ""
set torGlobal(currentEditorMenu) ""
set torGlobal(currentEditorFilename) ""

set extEditorGlobal(fromFileMenu) [appRegistryEntryRead -int -default 0 ExtEditor FromFileMenu]
set extEditorGlobal(fromProject) [appRegistryEntryRead -int -default 0 ExtEditor FromProject]
set extEditorGlobal(fromOutputWindow) [appRegistryEntryRead -int -default 0 ExtEditor FromOutputWindow]

set launcherGlobals(mainWinInitNotifiers) {}
set launcherGlobals(mainWinExitNotifiers) {}

# list of commands that are evaluated when the selection in the target list
# combo box change.

set launcherGlobals(targetChangeNotifiers) {}

set launcherGlobals(extEditorCmd) [appRegistryEntryRead -default "" ExtEditor Path]
if {$launcherGlobals(extEditorCmd) != ""} {
    set launcherGlobals(bExtEditorSetupOk) 1
}

proc getRegErrMessage { } {
	return "You need to have access to a Tornado Registry \
			to connect to any targets. For instructions on how to start \
	        the Tornado Registry, please refer to the documentation."
}

################################################################################
#
# MISC. TOOL LAUNCHING CALLBACK ROUTINES
#
# Callback routines (called from Tornado.Exe) to launch: tools, shells, and
# debuggers.
#

################################################################################
#
# wrapper_toolLaunch - expands variables in menuText toolCommandLine workingDir
#
# Given the six arguments, this command expands any Tcl variables and braced
# Tcl commands within 'menuText', 'toolCommandLine' and 'workingDir'.
#
# Once the expansion is complete and results in a non-zero length string, the
# built-in routine 'toolLaunch' is invoked with appropriate arguments.  Removes
# the last occurances of the '&' character in 'menuText'.
#
# SYNOPSIS:
#   wrapper_toolLaunch menuText toolCommandLine workingDir redirectStdInput
#                      redirectStdError redirectStdOutput redirectStdError
#                      promptForArgs closeWindowOnExit redirectToChildWindow
#
# PARAMETERS:
#   menuText : menu parameters
#   toolCommandLine : tool command line
#   workingDir : working directory
#   redirectStdInput
#   redirectStdError
#   redirectStdOutput
#   redirectStdError
#   promptForArgs : prompt for args ?
#   closeWindowOnExit : close window on exit ?
#   redirectToChildWindow
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc wrapper_toolLaunch {menuText toolCommandLine workingDir \
			 redirectStdInput redirectStdOutput redirectStdError \
			 promptForArgs closeWindowOnExit \
			 redirectToChildWindow} {

    # Remove the last occurance of '&' character.

    regsub {(.*)&(.*)} $menuText {\1\2} menuText

    set toolCommandLine [expandAndOrEvalutateList $toolCommandLine]
    set windowTitle [expandAndOrEvalutateList $menuText]
    set workingDir [expandAndOrEvalutateList $workingDir]

    if {[llength $toolCommandLine]} {

	# In 'toolCommandLine' and 'workingDir', if the last character is a
	# backslash, replace it with a double backslash to fix a bug with
	# 'RunCMD.Exe'.

	regsub {(.*)\\$} $toolCommandLine {\1\\\\} toolCommandLine
	regsub {(.*)\\$} $workingDir {\1\\\\} workingDir

	if {[catch {toolLaunch $windowTitle $toolCommandLine $workingDir \
			       $redirectStdInput $redirectStdOutput \
			       $redirectStdError \
			       $promptForArgs $closeWindowOnExit \
			       $redirectToChildWindow} result]} {

	    puts "\nError: Tool launch failed: '$result'\n"
	}
    }
}

################################################################################
#
# wrapper_buildLaunch - expands arguments to menuText buildTarget workingDir
#
# Given the six arguments, this command expands any Tcl variables
# and braced Tcl commands within 'menuText', 'buildTarget' and 'workingDir'.
#
# Once the expansion is complete, the built-in routine 'buildLaunch' is invoked
# with appropriate arguments.  Removes the last occurances of the '&' character
# in 'menuText'.
#
# SYNOPSIS:
#    wrapper_buildLaunch menuText buildTarget workingDir
#
# PARAMETERS:
#   menuText : menu parameters
#   buildTarget : targets to build
#   workingDir : working directory
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc wrapper_buildLaunch {menuText buildTarget workingDir} {

    # Remove the last occurance of '&' character.

    regsub {(.*)&(.*)} $menuText {\1\2} menuText

    set makeCommandLine [format "make.exe %s" \
				[expandAndOrEvalutateList $buildTarget]]

    set windowTitle [expandAndOrEvalutateList $menuText]
    set workingDir [expandAndOrEvalutateList $workingDir]

    # In 'makeCommandLine' and 'workingDir', if the last character is a
    # backslash, replace it with a double backslash to fix a bug with
    # 'RunCMD.Exe'.

    regsub {(.*)\\$} $makeCommandLine {\1\\\\} makeCommandLine
    regsub {(.*)\\$} $workingDir {\1\\\\} workingDir

    if {[catch {buildLaunch $windowTitle $makeCommandLine $workingDir} \
	 result]} {

	puts "\nError: Build launch failed: '$result'\n"
	error $result
    }
}

################################################################################
#
# wrapper_shellLaunch - expands targetName to windSh command line
#
# Given the target name in 'targetName', this command forms the
# WindSh command line and passes it back to the Tornado dev. env. for actual
# execution.  Currently it adds no special command line options, but can be
# customized to add any user required options (e.g. "-c[plus]" for C++
# demangling, "-T[cl mode]" to start WindSh in Tcl mode, "-p[oll]" change the
# WindSh poll interval, etc.).
#
# SYNOPSIS:
#   wrapper_shellLaunch targetName
#
# PARAMETERS:
#   targetName : name of the target
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc wrapper_shellLaunch {targetName} {
    if {[catch {shellLaunch "Shell $targetName" "windsh $targetName"} result]} {
	puts "\nError: Shell launch failed: '$result'\n"
    }
}

################################################################################
#
# LaunchBrowser - forms the browser command
#
# Given the target name in 'targetName', this command forms the
# Browser command line and invokes the browser by passing it back to the Tornado
# dev. env. for actual execution.
#
# SYNOPSIS:
#   LaunchBrowser args
#
# PARAMETERS:
#   args : arguments list
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc LaunchBrowser {args} {
    global env

    set target [lindex $args 0]
    if {$target == ""} {
		set target [selectedTargetGet]
    }
	set browserFile "[wtxPath host resource tcl]Browser.win32.tcl"
	
	set execScrpt "source $browserFile"
	set initScrpt "browserInit $target"

    if {[catch {windowCreate -width 250 -height 220 -name Browser \
		-exec "$execScrpt" -init "$initScrpt" \
		-icon [wtxPath host resource bitmaps browser controls]Browser.ico } result]} {
		puts "\nError: Failed to launch the Browser: '$result'\n"
    }
}

################################################################################
#
# LaunchShell - determines shell command through args
#
# Given the target name in 'targetName', this
# command forms the Shell command line and invokes the
# Shell by passing it back to the Tornado dev. env. for actual execution.
#
# SYNOPSIS:
#   LaunchShell args
#
# PARAMETERS:
#   args : arguments list
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc LaunchShell {args} {

    set target [lindex $args 0]
    if {$target == ""} {
	set target [selectedTargetGet]
    }

    wrapper_shellLaunch "$target"
}

################################################################################
#
# LaunchDebugger - determines debugger command line through args
#
# Given the target name in 'targetName', this
# command forms the Debugger command line and invokes the
# Debugger by passing it back to the Tornado dev. env. for actual execution.
#
# SYNOPSIS:
#   LaunchDebugger args
#
# PARAMETERS:
#   args : arguments list for debugger
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc LaunchDebugger {args} {

    if {[isDebuggerActive] == 1} {
        return
    }

    set target [lindex $args 0]
    if {$target == ""} {
        set target [selectedTargetGet]
    }

    # if we still don't have a target name just give up
    # not point in struggling.
    if {$target == ""} {
        return
    }

    catch "::Workspace::sourcePathListGet [::Workspace::currentProjectNameGet]" paths

    dbgSaveStateDirSet
    crosswind eval onStartDebugging [targetServerGdbArchGet $target] $target
    crosswind eval setSourceSearchPaths [list [join $paths]]
}

proc dbgSaveStateDirSet {} {
    # Get a default directory from which to present the download
    # dialog

    if {![catch "::Workspace::selectionGet" handle]} {
        set defaultDir "\"\""
        if {![string match $handle $::Workspace::_NULL_OBJECT] && \
			![string match [::Object::evaluate $handle Type] \
				workspaceObject]} {
            set hProj [::Object::evaluate \
            [::Object::evaluate $handle Project] Handle]
            set defaultDir [::Workspace::projectBuildDirGet $hProj]
        }
        crosswind eval cwSaveStateDirSet $defaultDir
    }
}

proc onCmdUIDebuger {} {
    global enableflag

    set target [selectedTargetGet]
    if {$target == "" || [isDebuggerActive] == 1} {
        set enableflag 0
    } {
        set enableflag 1
    }
    
    return $enableflag
}

proc isDebuggerActive {} {
    set dbgNotActive [crosswind eval {canStartDebugger}]
    if {$dbgNotActive == 1} {
        return 0
    } else {
        return 1
    }
}

proc stopDebugger {} {
    crosswind eval {onStopDebugging}
}

proc onOpenFile {fromWhere bExtEditorOk fileName lineNo} {
    global launcherGlobals
    global extEditorGlobal

    if {$fileName == ""} {
        error "invalid filename!"
        return
    }

    set fileName [file nativename $fileName]
    if {$lineNo == ""} {
        set lineNo 0
    }

    # first save all the open files
    fileSaveAll

	# if it's not a Source/Tcl/Text file, let the internal doc. arch. handle it
	set listEditables [list .c .cpp .cxx .cc .h .hpp .hxx .s .tcl .txt]
	set extn [file extension $fileName]
	if {[lsearch -exact $listEditables $extn] < 0 } {
        fileOpen $fileName $lineNo
		return
	}

    # Check if the external editor must be used, reading prefs from registry
    set bExtEditor 0
    if {[string match {*fileMenu*} $fromWhere]} {
        set bExtEditor $extEditorGlobal(fromFileMenu)
    } elseif {[string match {*project*} $fromWhere]} {
        set bExtEditor $extEditorGlobal(fromProject)
    } elseif {[string match {*output*} $fromWhere]} {
        set bExtEditor $extEditorGlobal(fromOutputWindow)
    }


    # or using 'openFileListGet' and 'fileSave' or 'fileSaveAll'
    # make sure that the file is saved before invoking an external
    # editor

    if {$bExtEditor == 1 && $launcherGlobals(bExtEditorSetupOk) == 1} {
        set cmdLine $launcherGlobals(extEditorCmd)
        # substitute the macros:

        # double backslash because of regsub behaviour
        regsub -all {\\} $fileName {\\\\} fileName 

        regsub -all {\$filename} $cmdLine "$fileName" cmdLine
        regsub -all {\$lineno} $cmdLine "$lineNo" cmdLine
        puts stderr "Command: $cmdLine"
        set workingDir [workingDirectoryGet]
        if {[catch {toolLaunch "" $cmdLine $workingDir 0 0 0 0 0 0} result]} {
            puts "\nError: Editor launch failed: '$result'\n"
            error $result
        }
    } else {
        fileOpen $fileName $lineNo
    }
}


################################################################################
#
# expandAndOrEvalutateList - expand or evaluate a list
#
# This routine performs the variable substituion and
# Tcl command expansion for 'lst'.  Care has been taken to prevent backslash
# substitution by the Tcl interpretter during evaluation.
#
# SYNOPSIS:
#   expandAndOrEvalutateList lst
#
# PARAMETERS:
#   lst : list of arguments to expand
#
# RETURNS: expanded list
#
# ERRORS: N/A
#

proc expandAndOrEvalutateList {lst} {

    global env

    if {[currentFileGet] != ""} {

	# If there is no active document, leave the variables undefined causing
	# subsequent substitution errors that are handled below.

        set filepath [currentFileGet]
	set filedir  [file dirname $filepath]
	set filename [file tail $filepath]
	set basename [file rootname $filename]
    }

    catch {set line [currentFileLineGet]}
    catch {set column [currentFileColumnGet]}
    catch {set textsel [currentFileTextSelectionGet]}
    catch {set targetName [currentTargetGet]}
    catch {set workingDir [workingDirectoryGet]}

    if {![catch {currentTargetGet}]} {
	catch {regexp {(.*)\@} [currentTargetGet] dummy target}
    }

    # get tornado environment variables

    set wind_base ""
    set wind_registry ""
    set wind_host_type 	[wtxHostType]
	set wind_base [WIND_Env::WIND_BASE_get] 
	set wind_registry [WIND_Env::WIND_REGISTRY_get]

    set newList ""

    # Does the list actually contain anything?

    if {[llength $lst]} {

	# Expand each list element.

	for {set i 0} {$i < [llength $lst]} {incr i} {

	    set error ""

	    # Extact the $i'th item back as a list.  'lrange' is used, because
	    # 'lindex' would cause one layer of backslashes to be removed;
	    # 'lrange' does not do this (possibly a Tcl v7.3 bug).

	    set listElem [lrange $lst $i $i]

	    # Protect single back-slashes ('\') from one level of 'eval'.

	    regsub -all {\\} $listElem {\\\\} listElem

	    # Does the element represent a "braced" Tcl script.

	    if {([string range $listElem 0 0] == "\{") &&
		([string range $listElem [expr [string length $listElem] - 1] \
			 end] == "\}")} {

		# Evaluate the Tcl script, while catching any evaluation errors.

		if {[catch {eval [lindex $listElem 0] [lrange $listElem 1 end]}\
		     listElem]} {

		    set error $listElem
		}

	    } else {

		# Expand the list element; causes substitute of all referenced
		# variables, while catching any substitution error.

		if {[catch {eval format "%s" $listElem} listElem]} {
		    set error $listElem
		}
	    }

	    # Did an error occur during script or substitution evaluation?
	    if {$error != ""} {
		if {[regexp {.*("filepath").*} $error dummy errVar] ||
		    [regexp {.*("filedir").*} $error dummy errVar] ||
		    [regexp {.*("filename").*} $error dummy errVar] ||
		    [regexp {.*("basename").*} $error dummy errVar]} {

		    set error "attempt to use '$errVar' with no active document"

		} elseif {[regexp {.*("line").*} $error dummy errVar]} {

		    catch {currentFileLineGet} errString
		    set error "attempt to use '$errVar' caused the error: \
			       $errString"

		} elseif {[regexp {.*("column").*} $error dummy errVar]} {

		    catch {currentFileColumnGet} errString
		    set error "attempt to use '$errVar' caused the error: \
			       $errString"

		} elseif {[regexp {.*(textsel).*} $error dummy errVar]} {

		    catch {currentFileTextSelectionGetz} errString
		    set error "attempt to use '$errVar' caused the error: \
			       $errString"

		} elseif {[regexp {.*("target.*").*} $error dummy errVar]} {

		    catch {currentTargetGet} errString
		    set error "attempt to use '$errVar' with no selected target"

		}

	    messageBox -excl "Unable to evaluate '$lst'.  Cause: $error."
	    error $error
	    }

	    # Append 'listElem' to '$newList'.

	    if {$i == 0} {
		set newList $listElem
	    } else {
		set newList [format "%s %s" $newList $listElem]
	    }
	}
    }
    # Return the expanded/evaluated list!

    return $newList
}

##############################################################################
#
# targetServerGdbArchGet - returns the GDB architecture of the specified target
#
# It depends on the fact that the globals in the file wtxcore.tcl are setup
# correctly.  It returns a blank string if cannot attach to the target or
# if the information is not available
#
# SYNOPSIS:
#   targetServerGdbArchGet
#
# PARAMETERS: N/A
#
# RETURNS: CPU family or empty string if unknown
#
# ERRORS: N/A
#

proc targetServerGdbArchGet {ts} {
    global cpuFamily

    if {! [catch {wtxToolAttach $ts -launcher} attachRetVal] &&
        ! [catch "wtxTsInfoGet" info]} {
        wtxToolDetach
        set typeId [lindex [lindex $info 2] 0]
        return [targetGdbSuffix $typeId]
    }

    return ""
}

##############################################################################
#
# selectedTargetGet - returns selected target in launch toolbar combo box.
#
# SYNOPSIS:
#   selectedTargetGet
#
# PARAMETERS: N/A
#
# RETURNS: selected target
#
# ERRORS: N/A
#

proc selectedTargetGet {} {
    return [controlSelectionGet launch.tgtCombo -string]
}

##############################################################################
#
# onTgtComboSelChange - this is a callback procedure
#
# this is the callback procedure for combo box that appears on the launch
# toolbar.  Whenever a selection changes, or user drops down the combo box,
# this procedure is called.
#
# SYNOPSIS:
#   onTgtComboSelChange
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS:
#

proc onTgtComboSelChange {} {
    global launcherGlobals
	
	# first find out the type of event that occured
	set eventInfo [controlEventGet launch.tgtCombo]
	
	# if it is dropdown, then update the target server list
	if [string match {*dropdown*} $eventInfo] {
		tgtServerListUpdate
		return
	}
	
	set curTgtSvr [controlSelectionGet launch.tgtCombo -string]
	if {$curTgtSvr == ""} {
		return 
	}

	if {[checkTarget $curTgtSvr] == 0} {
		set launcherGlobals(tgtsvr) $curTgtSvr
	} else {
		# tgtsvr isn't responding, checkTarget will display a messagebox
		set iCurTgtSvr [controlSelectionGet launch.tgtCombo]
		set curList [controlValuesGet launch.tgtCombo]  
		set curList [lreplace $curList $iCurTgtSvr $iCurTgtSvr]	
		controlValuesSet launch.tgtCombo $curList
		set launcherGlobals(tgtsvr) "" 
	}
}


##############################################################################
#
# toolSelect - callback by the buttons of the launch toolbar.
#
# the callback includes parameter specifying which button has been depressed.
# Depending on that value, the correct tool gets launched.  For example,
# browser toolbutton invokes with a arg value of "Browser".  If the selected
# target server is valid, the final call will be formatted as
# LaunchBrowser with the target server name.
#
# In order to add new tools to the toolbar button simply create the button and
# make the callback to be toolSelect <toolname> provide a tcl proc or 'C'
# callback for "Launch<Tool>".
#
# SYNOPSIS:
#   toolSelect callback
#
# PARAMETERS:
#   callback : information about caller
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc toolSelectCallback {callback} {
    global launcherGlobals

    set launcherGlobals(tgtsvr) [selectedTargetGet]
 	if {$callback != "" && $launcherGlobals(tgtsvr) != "" &&
	[checkTarget $launcherGlobals(tgtsvr)] == "0"} {
	    eval "$callback"
    }
}


##############################################################################
#
# messageBoxDisplay
#
# This should be called by a windows call back
# It displays the error message, then kills the timer

proc messageBoxDisplay { errMessageArgs } {
    windowTimerCallbackSet launch 0
	eval messageBox $errMessageArgs
}	

##############################################################################
#
# tgtServerListInitialize - retrieves list of registered target servers
#				when Tornado is first launched. If a local registry is to
#				be used and is not running, wtxregd is launched.
# RETURNS	- a list of target servers
#

proc tgtServerListInitialize {} {
    global launcherGlobals

    beginWaitCursor

	set regVal localhost
	catch {set regVal [WIND_Env::WIND_REGISTRY_get]} err
	set tgtSvrList {}

	# query registry:
    if {[catch {tgtServerListGet} tgtSvrList]} {
		set bSuccess 0
	} else {
		set bSuccess 1
	}

	# if it failed, and it was local host, start local reg
	if {(!$bSuccess) && ($regVal == "localhost")} {
		set bSuccess [localRegistryStart] 
		set tgtSvrList {} ;# you just started the registry, no tgts yet
	} 

	# if all failed, time to report the bad news	
	if {!$bSuccess} {
		if {$regVal == "localhost"} {
			set hostName "this host"
		} else {
			set hostName "host \"$regVal\"" 
		}
		set msg "Warning: Cannot find a valid Tornado Registry on $hostName.\n\n"
		append msg [getRegErrMessage]
	    delayedMessageBox $msg 
    } else { 
        controlValuesSet launch.tgtCombo $tgtSvrList
    }

    endWaitCursor

	return $tgtSvrList
}

##############################################################################
#
# tgtServerListUpdate - retrieves list of registered target servers
#
# SYNOPSIS:
#   tgtServerListGet
#
# PARAMETERS: N/A
#
# RETURNS: registered target servers list or blank string on error
#
# ERRORS: N/A
#

proc tgtServerListUpdate {} {
    global launcherGlobals
	set fSuccess 1

    beginWaitCursor
    if {[catch {tgtServerListGet} tgtSvrList]} {
	    delayedMessageBox \
			"Warning: unable to retrieve Target Server list - $tgtSvrList"
            set fSuccess 0
    } {
        controlValuesSet launch.tgtCombo $tgtSvrList
    }
    endWaitCursor
	set fSuccess
}

##############################################################################
#
# tgtServerListGet - retrieves list of registered target servers
#
# SYNOPSIS:
#   tgtServerListGet
#
# PARAMETERS: N/A
#
# RETURNS: the regsitered target servers list or blank string on error
#
# ERRORS: N/A
#

proc tgtServerListGet {} {

    set servList [list ]

    if {! [catch {wtxInfoQ .* tgtsvr} tmpServList]} {
        foreach targetServer $tmpServList {
            lappend servList [lindex $targetServer 0]
        }
    } else {
        if {$tmpServList == "API_REGISTRY_UNREACHABLE"} {
			set errMsg "Cannot find a valid Tornado Registry.\n\n"
			append errMsg [getRegErrMessage]
            error $errMsg 
        }
    }
    set servList
}


################################################################################
# delayedMessageBox - calls message box in windows callback
#
# This allows us to display a message box during a UITcl callback
# 
# SYNOPSIS:
#    delayedMessageBox args
#
# PARAMETERS:
#    args: all the standard args to the messageBox procedure, as documented in
#          the Tornado API guide
# RETURNS: nothing
#
# ERRORS: N/A
proc delayedMessageBox { args } {    
    windowTimerCallbackSet launch -milli 5 \
       [list messageBoxDisplay $args]
}

################################################################################
#
# checkTarget - checks if the target server is alive and responsive.
#
# Checks if the target server is alive, displays a message if it is not, and
# gives the option to the user to unregister the target server.
#
# SYNOPSIS:
#   checkTarget ts
#
# PARAMETERS:
#   ts : Target Server name
#
# RETURNS: 0 if ok, 1 if not, and "" if no target server has been given
#
# ERRORS: N/A
#

proc checkTarget {ts} {

    if {$ts == ""} return ""
    set retValue 0

    beginWaitCursor

    if {[catch {wtxToolAttach $ts -launcher} attachRetVal]} {

	if { [regexp {SVR_IS_DEAD} $attachRetVal] } {
	    delayedMessageBox -ok "Target server $ts is dead and unregistered."
	    tgtServerListUpdate
	} elseif { [regexp {SVR_DOESNT_RESPOND} $attachRetVal] } {
	    displayNoRespondDlg $ts
	} elseif { [regexp {REGISTRY_NAME_NOT_FOUND} $attachRetVal] } {
	    delayedMessageBox -ok "Target Server $ts is no longer registered."
	    tgtServerListUpdate
	} elseif { [regexp {API_REGISTRY_UNREACHABLE} $attachRetVal] } {
	    set msg "Cannot find a valid Tornado Registry.\n\n"
	    append msg [getRegErrMessage]
	    delayedMessageBox $msg 
        } else {  ;# unexpected error, record it on console 
	    puts "$attachRetVal"
	}
        set retValue 1
    }

    catch { wtxToolDetach } err
    endWaitCursor
    return $retValue
}


################################################################################
#
# isTgtSvrSelectable - checks if the target server is alive and responsive.
#	
#	same as checkTarget except:
#	- it does not display a message if target does not respond
#	- it returns 1 if selectable, 0 if not
#
proc isTgtSvrSelectable {tgtSvrName} {
    if {[string trim $tgtSvrName] ==""} {
	return 0
    }

    beginWaitCursor

    if {[catch {wtxToolAttach $tgtSvrName -launcher} attachRetVal]} {
	set retValue 0
	if {(![regexp {SVR_IS_DEAD} $attachRetVal]) && \
	    (![regexp {SVR_DOESNT_RESPOND} $attachRetVal]) && \
	    (![regexp {REGISTRY_NAME_NOT_FOUND} $attachRetVal]) && \
	    (![regexp {API_REGISTRY_UNREACHABLE} $attachRetVal])} {
	    # an unexpected error, record it on console
	    puts "$attachRetVal"
	}
    } else {
        set retValue 1
    }

    catch { wtxToolDetach } err
    endWaitCursor
    return $retValue
}


##############################################################################
#
# loadTargetName - loads the last selected target name, if available 
#
# loads the last selected target server name from the Windows Registry
# as part of initialization of Tornado
#
# SYNOPSIS:
#   loadTargetName
#
# PARAMETERS: currentList 
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc loadTargetName {currentList} {
    set previousTgt [appRegistryEntryRead Target Target]
	# select the tgt only if it's on the current list and is sane
	if { [lsearch $currentList $previousTgt] !=-1 } {
		if {[isTgtSvrSelectable $previousTgt]} {	
    		controlSelectionSet launch.tgtCombo -noevent -string $previousTgt
		}
	}
}

##############################################################################
#
# saveTargetName - saves the currently selected target name.
#
# writes the currently selected target server name to the Windows Registry
# before exiting Tornado to restore the name for the next Tornado session.
# Name of the target server has to be provided as the first and only argument.
#
# SYNOPSIS:
#   saveTargetName 
#
# PARAMETERS:
#   tgtName : Name to be used for writing.
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc saveTargetName {} {
    set tgtName [selectedTargetGet]

    appRegistryEntryWrite Target Target $tgtName
}

proc addEditorCb {newCb} {
    global torGlobal

    lappend torGlobal(editorContextMenuCbs) $newCb
}

proc removeEditorCb {newCb} {
    global torGlobal

    # lappend torGlobal(editorContextMenuCbs) $newCb
}

proc onEditorContextMenuOpenDoc {fileName} {
	set lineNo [string trim [currentFileLineGet]]
	if {$lineNo==""} { set lineNo 0 }
    onOpenFile fileMenu 1 $fileName $lineNo 
}

proc onEditorContextMenu {args} {
    global launcherGlobals
    global torGlobal

    set fileName [lindex $args 0]
    set menuName [lindex $args 1]
    set mousePt [list [lindex $args 2]]

    # this menu will be delete using menuDelete
    # from the source window after finishing
    # processing menu

    menuCreate -name $menuName -context

    menuItemAppend $menuName -menuidentifier 0xE123 "Cu&t"
    menuItemAppend $menuName -menuidentifier 0xE122 "&Copy"
    menuItemAppend $menuName -menuidentifier 0xE125 "&Paste"
    menuItemAppend $menuName -separator
    menuItemAppend $menuName -menuidentifier 0xE120 "&Delete"
    menuItemAppend $menuName -menuidentifier 0xE12B "&Undo"

    menuItemAppend $menuName -separator
    menuItemAppend $menuName -callback "onEditorContextMenuOpenDoc $fileName" \
					        "Open Document"
    menuItemAppend $menuName -callback "vcUIShow checkout $fileName" \
					        "Checkout Document"
    menuItemAppend $menuName -callback "vcUIShow checkin $fileName" \
					        "Checkin Document"

    if {$launcherGlobals(bExtEditorSetupOk) != 1} {
        menuItemEnable $menuName -string "Open Document" 0
    }

    # If the VC script hasn't been sourced, or if VC hasn't been configured:
    if {([info commands vcIsConfigured] != "vcIsConfigured") || \
        ![vcIsConfigured]} {
        menuItemEnable $menuName -string "Checkout Document" 0
        menuItemEnable $menuName -string "Checkin Document" 0
    }

    set torGlobal(currentEditorMenu) $menuName
    set torGlobal(currentEditorFilename) $fileName

    foreach command $torGlobal(editorContextMenuCbs) {
        catch {eval "$command $fileName $menuName $mousePt"} errMsg
        puts $errMsg
    }

    set torGlobal(currentEditorMenu) ""
    set torGlobal(currentEditorFilename) ""
}

proc select_checkTgtSvr {name} {
    global launcherGlobals
    tgtServerListUpdate
    set launcherGlobals(tgtsvr) $name
    controlSelectionSet launch.tgtCombo -noevent -string "$name"

    foreach command $launcherGlobals(targetChangeNotifiers) {
        $command
    }
    checkTarget $name
}

proc addMainWinInitCb {newCb} {
    global launcherGlobals

    lappend launcherGlobals(mainWinInitNotifiers) $newCb
}

proc addMainWinExitCb {newCb} {
    global launcherGlobals

    lappend launcherGlobals(mainWinExitNotifiers) $newCb
}

proc mainWindowInitCallback {} {
    global launcherGlobals

    foreach command $launcherGlobals(mainWinInitNotifiers) {
        eval "$command"
    }
}

proc mainWindowExitCallback {} {
    global launcherGlobals

    foreach command $launcherGlobals(mainWinExitNotifiers) {
        eval "$command"
    }
}

# setup mainwindow init and exit callbacks
windowInitCallbackSet mainwindow mainWindowInitCallback
windowExitCallbackSet mainwindow mainWindowExitCallback

################################################################################
#
# Set up the main window's callback such that the current selection is saved
# before Tornado is exited.
#

addMainWinExitCb saveTargetName

#
# Source wtx libraries
#
source [wtxPath host resource tcl]/wtxcore.tcl

#
# Toolbar and button creation
#
proc onToggleLaunchTB {} {

    if {[windowVisible launch] == 1} {
        windowShow launch 0
    } {
        windowShow launch 1
    }
}

proc onCmdUILaunchTB {} {
    global checkflag

    set checkflag [windowVisible launch]
}

proc onCmdUITool {tool} {
    global enableflag

    set target [selectedTargetGet]
    if {$target == ""} {
        set enableflag 0
    } {
        set enableflag 1
    }
}

proc localRegistryStart {} {
    if {[uitclinfo debugmode]} {
        set cmdLine [wtxPath  host x86-win32 bin]wtxregd-d.exe
    } else {
        set cmdLine [wtxPath  host x86-win32 bin]wtxregd.exe
    }

    if {[info exists env(TEMP)]} {
        set dbPath $env(TEMP)
    }  else {
		set dbPath "C:\\TEMP"
    } 
    append cmdLine " -V -d $dbPath"
    puts stderr "Command: $cmdLine"
    set workingDir [wtxPath host x86-win32]bin\\

    # launch external process with these options:
    # capture stdin, capture stdout, capture stderr,
    # prompt for args, close window on exit,
    # run as mdi child
    if {[catch {::toolLaunch "wtxregd:" $cmdLine $workingDir \
                           0 0 0 \
                           0 0 \
                           0} err]} {
        puts "\nerror: '$err'\n"
        return 0
    } else {
		return 1
	}
}

proc displayNoRespondDlg {tsName} {

    proc noRespond_onUnregister {tsName} {
	if {[catch {wtxUnregister $tsName} errMsg]} {
	    messageBox "Error: unregister of $tsName failed!"
	}
	windowClose IDD_DIALOG_tsNotResp
    }

    dialogCreate -name IDD_DIALOG_tsNotResp -title "Tornado Warning" -width 230\
		 -height 100 -init {messageBeep -asterisk} \
		 -controls [list \
		     {button -title "OK" -name IDOK -xpos 116 -ypos 79 \
			     -width 50 -height 14  \
			     -callback {windowClose IDD_DIALOG_tsNotResp}} \
		     [list button -title "Unregister" -name IDC_BUTTON1 \
			   -xpos 173 -ypos 79 -width 50 -height 14 \
			   -callback "noRespond_onUnregister $tsName"] \
		     [list label -title "Target Server \"$tsName\" is not\
					 responding.\nThis may be due to slow\
					 network response.\n\nNOTE : If the\
					 problem persists, please see the\
					 Tornado Migration\
					 \n             Guide, section\
					 \"Tornado Registry\"" \
				 -xpos 4 -ypos 4 -width 219 -height 68] \
		 ]
}

toolbarCreate mainwindow -name launch -title "Tornado Launch" -top -bottom

menuItemInsert -bypath -after -callback onToggleLaunchTB \
	-cmduicallback onCmdUILaunchTB \
	{&View &Toolbar "S&tandard Toolbar"} "&Launch Toolbar"

controlCreate launch \
	[list combo -callback onTgtComboSelChange -name tgtCombo \
		-sort -tooltip "Target Server List" -xpos 5 -width 100 -height 70]

controlCreate launch \
	[list toolbarbutton -name browser -separator -callback "toolSelectCallback LaunchBrowser" \
		-cmduicallback "onCmdUITool Browser" -tooltip "Launch Browser" \
		-bitmap "[wtxPath host resource bitmaps Launch controls]Browser.bmp"]

controlCreate launch \
	[list toolbarbutton -name shell -tooltip "Launch Shell" \
		-cmduicallback "onCmdUITool Shell" \
		-callback "toolSelectCallback \"wrapper_shellLaunch \[selectedTargetGet\]\"" \
		-bitmap "[wtxPath host resource bitmaps Launch controls]Shell.bmp"]

controlCreate launch \
	[list toolbarbutton -name debugger -callback {toolSelectCallback LaunchDebugger} \
		-cmduicallback onCmdUIDebuger -tooltip "Launch Debugger" \
		-bitmap "[wtxPath host resource bitmaps Launch controls]Debugger.bmp"]

# Finally, initialize the target server list in the Tornado Launch toolbar.
set tsl [tgtServerListInitialize]
loadTargetName $tsl
