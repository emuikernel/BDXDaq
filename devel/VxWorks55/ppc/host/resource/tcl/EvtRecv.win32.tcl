# EvtRecv.win32.tcl - Event Receive Tcl implementation file
#
# Copyright 1996 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01s,04dec01,fle  fixed Wind package use
# 01r,15oct01,fle  unified Tornado Version String
# 01q,21sep01,cpd  Initial Tornado 2.2 mods.
#                  Fix SPR#30563: Fix Thread exit/restart problem
#                  Fix SPR#27036, #27020, #34469 :  Fix Online help
# 01p,08apr99,nps  fix help base path was being generated incorrectly
#                  if WIND_BASE wasn't set (spr 26464).
# 01o,02mar99,nps  Change app title to Event Receive.
#                  Check the following sequence for default log path:
#                  env(TEMP), env(TMP), c:\temp, c:\windows\temp, c:\.
#                  Use 'modern' registry API and format - registry
#                  entries are now located under Tornado 2.0/EvtRecv.
#                  Changed exit message to be more meaningful.
#                  Improved error message when specified log path
#                  does not exist.
#                  Exit cleanly via Exit button or window frame X.
# 01n,01dec98,nps  fix setting of 'basePath'.
#                  fix Help buttons.
# 01m,19nov98,nps  Convert to UITcl II (fixing SPR #23377).
#                  Fixed erroneous file path truncation.
#                  Use DOS style backslash pathnames consistently.
# 01l,06mar97,j_k  added applicationTitleSet call
# 01k,19jun96,j_k  part of fix for SPR #6769.
# 01j,04jun96,jank part of fix for SPRs #6662 and #6664. add confirmation on exit 
#                  in case of running threads, -helpid on labels now supported
#                  which makes modification 01i obsolete
# 01i,06may96,jank removed -helpid's from dialog label calls, else not displayed 
# 01h,11apr96,j_k  fixed up some empty message boxes and made help work
#                  correctly.
# 01g,11apr96,jank globals saved in Tornado registry if successful finished
# 01f,04apr96,jank setup dialog starts if selected port number is busy; 
#                  directory browser added; 
# 01e,22mar96,j_k  removed path to evtrecv.dll.  Assuming that path will be
#                  setup correctly.
# 01d,15mar96,jank evtRecvDlgExit added and onTerm changed, corrected
#                  update of hostname and ipaddr
# 01c,11mar96,jank add: selection user defined/hostinfo as logfile base
# 01b,01mar96,jank bug fixes: enable/disable buttons, setup dialog
# 01a,28feb96,jank written (prototype)

# DESCRIPTION
# This module is the Tcl code for the graphical front end for eventReceive.
# It creates a dialog based win32 application and uses the evtrecv.dll for
# collecting event data from connecting targets. Current status informations
# are displayed.
#
# RESOURCE FILES
# evtrecv.dll

package require Wind

set tRegRoot [Wind::key]
set regRoot $tRegRoot\\EvtRecv

    # pre-populate registry
    catch {sysRegistryKeyCreate HKEY_CURRENT_USER "" Software}
    catch {sysRegistryKeyCreate HKEY_CURRENT_USER Software {Wind River Systems}}
    catch {sysRegistryKeyCreate HKEY_CURRENT_USER [Wind::key]}
    catch {sysRegistryKeyCreate HKEY_CURRENT_USER [Wind::key] EvtRecv}

if {[catch {sysRegistryValueRead HKEY_CURRENT_USER $regRoot logPath} result]} {
	sysRegistryValueWrite HKEY_CURRENT_USER $regRoot portNumber 6164
	sysRegistryValueWrite HKEY_CURRENT_USER $regRoot useHostBase 1
	sysRegistryValueWrite HKEY_CURRENT_USER $regRoot overWrite 1
	sysRegistryValueWrite HKEY_CURRENT_USER $regRoot logFbase "eventLog"
	sysRegistryValueWrite HKEY_CURRENT_USER $regRoot logPath ""
}

set pform $tcl_platform(os)

if {[string match {*Windows*} $pform]} {
    set dllName UITclControls.dll
    if {[uitclinfo debugmode]} {
        set dllName UITclControls-d.dll
    }
    dllLoad $dllName _UITclControls_Init
} elseif {[string match {*SunOS*} $pform]} {
    dllLoad libuitclcontrols.so _UITclControls_Init
}

applicationTitleSet "Event Receive"
set basePath ""
if {[catch {set basePath [wtxPath]}]} {
    if {[catch {set basePath "$env(WIND_BASE)"}]} {
        set basePath [uitclinfo exepath]
        regsub -all / $basePath {\\} basePath
        regexp -nocase {.*/host/x86-win32/bin} $basePath basePath
        set basePath [string range $basePath 0 \
            [expr [string length $basePath] - 32]]
    }
} {
	regsub -all / $basePath {\\} basePath
}

if {[string match {*Windows*} $pform]} {
    catch {exec regsvr32 /s $basePath\\host\\x86-win32\\bin\\OnlineHelp.dll}
}

# globals  

set logs()	""		;# list of event data logs
set tids	""		;# list of thread ID's
set evtRecvDll	""		;# name of evtrecv.dll returned by dlload

set quickExit   0

set helpFile	$basePath\\host\\resource\\help\\evtrecv.hlp

# globals which read there default values from the Tornado registry

set oldPortNumber 0
set portNumber	[sysRegistryValueRead HKEY_CURRENT_USER $regRoot \
			portNumber]	;# default port number

set overWrite	[sysRegistryValueRead HKEY_CURRENT_USER $regRoot \
			overWrite]	;# overwrite existing files by default

set useHostBase	[sysRegistryValueRead HKEY_CURRENT_USER $regRoot \
			useHostBase]	;# use host info as logfile base

set logFbase	[sysRegistryValueRead HKEY_CURRENT_USER $regRoot \
			logFbase]	;# user defined logfile base

set logPath	[sysRegistryValueRead HKEY_CURRENT_USER $regRoot \
			logPath]	;# destination path for logfiles

# if nothing in registry fetch a nice default value if possible for logPath 
# Windows PC have often %TEMP% set to \windows\temp treat this as the
# second chance 

if {[string length $logPath] == 0} {
	if [info exists env(TMP)] {
		set logPath $env(TMP)
	} {
		if [info exists env(TEMP)] {
			set logPath $env(TEMP)
		} {
			if [file isdirectory "c:\\temp"] {
				set logPath "c:\\temp"
			} {
				if [file isdirectory "c:\\windows\\temp"] {
					set logPath "c:\\windows\\temp"
				} {
					set logPath "c:\\"
				}
			}
		}
	}
}


##############################################################################
#
# evtRecvDlg - main dialog, started right after successful initialization
#
# main evtRecv Dialog, displays all neccessary information of event data logs
# stop running receive threads bound to the "Stop" button, remove certain log 
# entries bound to the "Remove" button (this does not remove the belonging
# logfile from disk), provide setup dialog bound to the 
# "Setup" button
#
# SYNOPSIS:
#	evtRecvDlg
#
# PARAMETERS:	
#
# RETURNS: N/A
#
# ERRORS: N/A
#
proc evtRecvDlg {} {
    global tids
    global helpFile
    global basePath
    global portNumber

    set dlgTitle "Event Receive"

    dialogCreate \
        -name evtRecvDialog \
        -title  $dlgTitle \
        -icon "$basePath\\host\\resource\\bitmaps\\WindView\\evtRecv.ico" \
        -helpfile $helpFile \
        -w 305 -h 132 \
        -init {
            windowQueryCloseCallbackSet evtRecvDialog onQueryClose
            controlEnable evtRecvDialog.stopButton 0
            controlEnable evtRecvDialog.removeButton 0
            controlValuesSet evtRecvDialog.labelSocket $portNumber
        } \
        -exit { onExit
        } \
        -controls {
            { label -fixed -title \
            	"Name                     Size      Status" \
                -x 7 -y 7 -w 250 -h 7 }
            { list -fixed -name logInfoList -helpid 5001 \
                -x 7 -y 18 -w 231 -h 65 \
    	    	-callback {
    	    	    onListEvent
    	    	} \
            }
            { label -title "Target name:" -x 7 -y 85 -w 60 -h 8 }
            { label -name labelHostName -helpid 5002 -x 67 -y 85 -w 100 -h 7 }
            { label -title "IP-Address:" -x 7 -y 96 -w 60 -h 7 }
            { label -name labelIPAddr -helpid 5003 -x 67 -y 96 -w 100 -h 7 }
            { label -title "Current Socket:" -x 197 -y 85 -w 60 -h 7 }
            { label -name labelSocket -x 257 -y 85 -w 40 -h 7 }
            { button -name stopButton -title "S&top" -helpid 5006 \
                -x 247 -y 18 -w 50 -h 14 \
    	    	-callback {
                    set curTid [lindex $tids \
                        [controlSelectionGet evtRecvDialog.logInfoList]]
                    evtRecvStop $curTid
            	    onStop $curTid
                } \
            }
            { button -name removeButton \
                -title "&Remove" -helpid 5007 -x 247 -y 37 -w 50 -h 14 \
                -callback {
                    onRemove
                } \
            }
            { button -default -name exitButton -title "E&xit" -helpid 5004 \
              	-x 7 -y 111 -w 50 -h 14 \
                -callback  { onExit
                } \
            }
            { button -title "&Setup..." -helpid 5005 -x 62 -y 111 -w 50 -h 14 \
                -callback {
                    setupDialog 1
                } \
            }
            { button -title "&Help" -helpid 100 \
                -x 247 -y 111 -w 50 -h 14 -callback {evtRecvHelp 100}
            }
        }
}

proc evtRecvHelp {id} {
    global helpFile

    helpLaunch context $helpFile $id
}

proc onExit {} {
    global quickExit

    if {$quickExit == 0} {
        if {[checkRunningThreads] == 1} {
            set quickExit 1
            windowClose evtRecvDialog
        } else { return }
    }
}

proc onQueryClose {} {
    global quickExit

    set ans [messageBox -yesno "Are you sure you want to exit?"]

    if {$ans == "yes"} { return 1 }

    set quickExit 0
    return 0
}

##############################################################################
#
# onListEvent - application specific handler for list control
#
# detection for enable/disable buttons regarding current selection in list
#
# SYNOPSIS:
#	onListEvent
#
# PARAMETERS:	
#
# RETURNS: N/A
#
# ERRORS: N/A
#
proc onListEvent {} {
	global tids
	global logs

	set selection [controlSelectionGet evtRecvDialog.logInfoList]
	if {$selection == -1} {
		controlEnable evtRecvDialog.stopButton 0
		controlEnable evtRecvDialog.removeButton 0

		controlValuesSet evtRecvDialog.labelHostName ""
		controlValuesSet evtRecvDialog.labelIPAddr ""

	} {
		set status [lindex $logs([lindex $tids $selection]) 3]
		if {$status == "DONE"} {
			controlEnable evtRecvDialog.stopButton 0
			controlEnable evtRecvDialog.removeButton 1
		} {							
			controlEnable evtRecvDialog.stopButton 1
			controlEnable evtRecvDialog.removeButton 0
		}
		controlValuesSet evtRecvDialog.labelHostName \
			[lindex $logs([lindex $tids $selection]) 4]
		controlValuesSet evtRecvDialog.labelIPAddr \
			[lindex $logs([lindex $tids $selection]) 5]
	}
}


##############################################################################
#
# onConnect - target connection established 
#
# dll catched window message signaling successful connection from a target
#
# SYNOPSIS:
#	onConnect tid fileName hostName ipAddr
#
# PARAMETERS:	
#	tid: thread identifier of log-thread
#	fileName: file name for output of logged event data
#	hostName: name of connected host
#	ipaddr:	IP-address of connected host
#
# RETURNS: N/A
#
# ERRORS: N/A
#
proc onConnect {tid fileName hostName ipAddr} {
	global logs
	global tids

	lappend tids $tid
	set logs($tid) "$tid $fileName 0 WAIT $hostName $ipAddr"

	controlValuesSet evtRecvDialog.labelHostName $hostName
	controlValuesSet evtRecvDialog.labelIPAddr $ipAddr

	controlValuesSet evtRecvDialog.logInfoList -append \
		[list [format "%-23s %-9s %-6s" \
		[lindex $logs($tid) 1] [lindex $logs($tid) 2] \
		[lindex $logs($tid) 3] ] ] 

	controlSelectionSet evtRecvDialog.logInfoList \
				[lsearch -exact $tids $tid]
}


##############################################################################
#
# onStatusChanged - progress report of particular thread 
#
# dll catched window message signaling progress in logging event data, updates
# status information 
#
# SYNOPSIS:
#	onStatusChanged tid fileSize statusType
#
# PARAMETERS:	
#	tid: thread identifier of log-thread
#	fileSize: current number of written bytes onto logfile
#	statusType: string of current status
#
# RETURNS: N/A
#
# ERRORS: N/A
#
proc onStatusChanged {tid fileSize statusType} {
	global logs
	global tids

	set logs($tid) [lreplace $logs($tid) 2 3 $fileSize $statusType]
	set currentSel [controlSelectionGet evtRecvDialog.logInfoList]

	set listToInsert ""
	foreach tid $tids {
		lappend listToInsert \
                        [format "%-23s %-9s %-6s" \
                        [lindex $logs($tid) 1] \
                        [lindex $logs($tid) 2] \
                        [lindex $logs($tid) 3] ] 
	}
	controlValuesSet evtRecvDialog.logInfoList $listToInsert
	controlSelectionSet evtRecvDialog.logInfoList $currentSel
}


##############################################################################
#
# onTerm - termination on error or oob message
#
# dll catched window message signaling termination or oob message
# decide what to do either restart or dllUnload and exit
#
# SYNOPSIS:
#	onTerm tid errorNumber 
#
# PARAMETERS:	
#	tid: thread identifier of log-thread
#	errorNumber:	-1 no empty entry for new thread
#					-2 could not create logfile
#					-3 event buffer overflow
#					-4 system error 
#
# RETURNS: N/A
#
# ERRORS: N/A
#
proc onTerm {tid errorNumber} {
	global logs
	global tids

	catch { evtRecvGetLastErr } errmsg
	if {$errmsg != ""} {
		messageBox $errmsg
	}

	set logs($tid) [lreplace $logs($tid) 3 3 DONE]
	set currentSel [controlSelectionGet evtRecvDialog.logInfoList]

	set listToInsert ""
	foreach tid $tids {
		lappend listToInsert \
                        [format "%-23s %-9s %-6s" \
                        [lindex $logs($tid) 1] \
                        [lindex $logs($tid) 2] \
                        [lindex $logs($tid) 3] ] 
	}
	controlValuesSet evtRecvDialog.logInfoList $listToInsert
	controlSelectionSet evtRecvDialog.logInfoList $currentSel
}


##############################################################################
#
# onStop - user stopped connection from the application
#
#
# SYNOPSIS:
#	onStop tid
#
# PARAMETERS:	
#	tid: thread identifier of log-thread
#
# RETURNS: N/A
#
# ERRORS: N/A
#
proc onStop {tid} {
	global logs
	global tids

	set logs($tid) [lreplace $logs($tid) 3 3 DONE]
	set currentSel [controlSelectionGet evtRecvDialog.logInfoList]

	set listToInsert ""
	foreach tid $tids {
		lappend listToInsert \
                        [format "%-23s %-9s %-6s" \
                        [lindex $logs($tid) 1] \
                        [lindex $logs($tid) 2] \
                        [lindex $logs($tid) 3] ] 
	}
	controlValuesSet evtRecvDialog.logInfoList $listToInsert
	controlSelectionSet evtRecvDialog.logInfoList $currentSel
}


##############################################################################
#
# onRemove - remove selected item from listbox
#
# dll catched window message signaling termination or the end of a particular 
# thread, updates status information an displays error message box in case
#
# SYNOPSIS:
#	onRemove
#
# PARAMETERS:	
#
# RETURNS: N/A
#
# ERRORS: N/A
#
proc onRemove {} {
	global logs
	global tids

	set listContents [controlValuesGet evtRecvDialog.logInfoList]
        set currentSel [controlSelectionGet evtRecvDialog.logInfoList]

	# delete selected entries in tids and logs

	unset logs([lindex $tids $currentSel])
	set tids [lreplace $tids $currentSel $currentSel]

	set listToInsert ""
	foreach tid $tids {
		lappend listToInsert \
                        [format "%-23s %-9s %-6s" \
                        [lindex $logs($tid) 1] \
                        [lindex $logs($tid) 2] \
                        [lindex $logs($tid) 3] ] 
	}
	controlValuesSet evtRecvDialog.logInfoList $listToInsert

	# no selection so empty hostname and ipaddr

	controlValuesSet evtRecvDialog.labelHostName ""
	controlValuesSet evtRecvDialog.labelIPAddr ""

	# in case of empty list disable buttons

    if {[controlSelectionGet evtRecvDialog.logInfoList] == ""} {
		controlEnable evtRecvDialog.stopButton 0
		controlEnable evtRecvDialog.removeButton 0
	}
}


##############################################################################
#
# setupDialog - shows setup dialog and alters globals on demand
#
# set port number, basename for logfile and select opening mode for output
# files, on port number
# change demand confirmation to stop all running log-threads and closing the
# service socket if there is at least one log-thread still running
#
# SYNOPSIS:
#	setupDialog <init>
#
# PARAMETERS:	
#	<init>	if 1 evtRecvInit/evtRecvStartup are called otherwise not
#
# RETURNS: 0 - on cancel; 1 - on ok
#
# ERRORS: N/A
#
proc setupDialog {init} {
	global portNumber
	global oldPortNumber
	global overWrite
	global logFbase
	global logPath
	global useHostBase
	global helpFile

	set selPath ""
	set cancelState 0

	dialogCreate -name setupDialog \
		-title "Setup" \
		-helpfile $helpFile \
		-w 191 -h 100 \
		-init {
			set oldPortNumber $portNumber
			controlValuesSet setupDialog.portNumberEdit $portNumber
			controlValuesSet setupDialog.logFbaseEdit $logFbase
			controlValuesSet setupDialog.logPathEdit $logPath
			controlCheckSet setupDialog.overwriteBoolean $overWrite
			controlCheckSet setupDialog.usehostBoolean $useHostBase
			if {$useHostBase} {
				controlEnable setupDialog.logFbaseEdit 0
			} {
				controlEnable setupDialog.logFbaseEdit 1
			}
		} \
		-controls [list \
		    [list boolean -x 7 -y 7 -w 110 -h 14 -name overwriteBoolean \
			    -auto -title "Overwrite Existing Files" -helpid 5009 ] \
		    [list boolean -x 7 -y 25 -w 125 -h 14 -name usehostBoolean \
			    -title "Use Target name as Log-file Base" -auto -helpid 5010 \
			    -callback {
    				if {[controlChecked setupDialog.usehostBoolean]} {
	    				controlEnable setupDialog.logFbaseEdit 0
		    		} {
			    		controlEnable setupDialog.logFbaseEdit 1
				    }
			    }
		    ] \
		    [list label -title "Log-file Base" -helpid 5011 -x 7 -y 45 -h 14 -w 55] \
		    [list text -name logFbaseEdit -helpid 5011 -x 56 -y 44 -w 67 -h 14] \
		    [list label -title "Port Number" -helpid 5012 -x 7 -y 64 -w 55 -h 14] \
		    [list text -name portNumberEdit -helpid 5012 -x 56 -y 63 -w 67 -h 14] \
		    [list label -title "Log Path" -helpid 5013 -x 7 -y 83 -w 55 -h 14] \
		    [list text -name logPathEdit -helpid 5013 -x 56 -y 82 -w 67 -h 14] \
		    [list button -title "..." -helpid 5014 -x 126 -y 82 -w 14 -h 14 \
				-callback {
					set selPath [dirBrowseDialogCreate -title "Select log path"]
					if {[string length $selPath] > 0} {
						controlValuesSet setupDialog.logPathEdit $selPath

					} 
				} \
			] \
			[list button -title "OK" -defaultbutton -x 134 -y 7 -w 50 -h 14 \
				-callback {


					set portNumber [controlValuesGet setupDialog.portNumberEdit]

					# make sure only path name gets into it	 
					set oldPath $logPath
					set logPath [controlValuesGet setupDialog.logPathEdit]

					# in case UNIX style convert to backslash
					regsub -all (/+) $logPath {\\} logPath

					# make sure last char of logPath == \\	for "file dirname"
					if {[string last \\ $logPath ] != \
						[expr [string length $logPath] - 1]} {
 						set logPath $logPath\\
					}

					# check if path exists
					#set logPath [file dirname $logPath]

				    # file isdirectory works obviously only with slash
					regsub -all (/+) $logPath {\\} dir
					set outLogPath [string trimright $logPath \\]
					if {![file isdirectory $dir]} {
						messageBox "Path \"$outLogPath\" is invalid"

						set logPath $oldPath
						return
					}

					# make sure only file name gets into it
					
					set logFbase [controlValuesGet setupDialog.logFbaseEdit]
					
					# in case UNIX style convert to backslash; essential for
					# using "file tail"

					regsub -all (/+) $logFbase {\\} logFbase
					set logFbase [file tail $logFbase]

					set useHostBase [controlChecked \
										setupDialog.usehostBoolean]
					set overWrite [controlChecked \
										setupDialog.overwriteBoolean]
					
					## Here we attempt to open the socket
    				
					## if the variable is still 0 we failed so we need to send a message
					if {$portNumber != $oldPortNumber || $init == 0} {
						if {[catch {set testSock [socket -server {} $portNumber]} errmsg]} {
							messageBox "Couldn't open socket $portNumber - Port is already in use"
							set portNumber $oldPortNumber
							return
						} else {
							close $testSock
						}
					}

					# only if init is true do the necassary evtRecvStartup/Init
					if {$init == 1} {
						if {$portNumber != $oldPortNumber} {
							onPortChanged
						}
						if {$useHostBase} {
							evtRecvInit $overWrite $logPath
						} {
							evtRecvInit $logFbase $overWrite $logPath
						}
					}

					if {$portNumber != $oldPortNumber} {
						if {[catch {evtRecvStartup $portNumber} errmsg]} {
							if {[regexp "couldn't bind socket" $errmsg] == 1} {
								messageBox "Port $portNumber is busy - Please select a different one"
								set portNumber $oldPortNumber
								return
							}  {
								if {$errmsg != ""} {
									messageBox $errmsg
								}
								return
							}
						}
					}

					windowClose setupDialog
					set cancelState 1
				} \
			] \
			[list button -title "&Cancel" -x 134 -y 25 -w 50 -h 14 \
				-callback {
					windowClose setupDialog

				} \
			] \
			[list button -title "&Help" -helpid 101 \
				-x 134 -y 43 -w 50 -h 14 -callback {evtRecvHelp 101} \
			] \
		]
	
	catch [controlValuesSet evtRecvDialog.labelSocket $portNumber]
	return $cancelState
}



proc checkRunningThreads {} {
	global logs
	global tids
	global portNumber

	set runningThreads ""
	foreach tid $tids {
		set status [lindex $logs($tid) 3]
		if { $status != "DONE" } {
			lappend runningThreads $tid
		}
	}

	if {[llength $runningThreads] > 0} {
		if {[messageBox -yesno \
			"Do you want to stop all running logs"] != "yes"} {
			return 0
		}			
	}

    return 1
}


##############################################################################
#
# onPortChanged - if required cleanup and start on new port number
#
# in case that there are currently running log-threads demand decision from 
# user if he wants to stop these log-threads 
#
# SYNOPSIS:
#	onPortChanged  
#
# PARAMETERS:	
#	oldPortNumber	-	port# value before executing dialog
#
# RETURNS: 
#	1 - evtRecv is restarted with effect of the new settings
#	0 - the user wants to dismiss the new settings
#
# ERRORS: N/A
#
proc onPortChanged {} {
	global logs
	global tids
	global portNumber
	
	if {[checkRunningThreads] == 0} {return 0}

	evtRecvCleanup

	return 1
}


##############################################################################
#
# main program flow
#
# loads evtrecv.dll and makes the essential initialisation,
# enables target connections
#
# SYNOPSIS:
#
# PARAMETERS:	
#
# RETURNS: N/A
#
# ERRORS: N/A
#
# if [catch {dllLoad [wtxPath host [wtxHostType] bin]evtrecv.dll} evtRecvDll]

set dllName "evtRecv.dll"
if {[uitclinfo debugmode] == 1} {
	set dllName "evtRecv-d.dll"
}
if [catch {dllLoad $dllName _Evtrecv_Init} evtRecvDll] {
	puts "Unable to open 'evtrecv.dll'. Cause: $evtRecvDll"
	return
}

if {[catch {evtRecvStartup $portNumber} errmsg]} {
	if {[regexp "couldn't bind socket" $errmsg] == 1} {
		messageBox "Port $portNumber is busy - Please select a different one"
		if {[setupDialog 0] == 0} {
			dllUnload $evtRecvDll
			return ""
		}
	} {
		messageBox "evtRecvStartup: $errmsg"
		dllUnload $evtRecvDll
		return ""
	}
}
	
if {$useHostBase} {
	evtRecvInit $overWrite $logPath
} {
	evtRecvInit $logFbase $overWrite $logPath
}

evtRecvDlg
dllUnload $evtRecvDll

# only after successful evtRecvDlg save current parameters to registry

sysRegistryValueWrite HKEY_CURRENT_USER $regRoot portNumber $portNumber
sysRegistryValueWrite HKEY_CURRENT_USER $regRoot useHostBase $useHostBase
sysRegistryValueWrite HKEY_CURRENT_USER $regRoot overWrite $overWrite
sysRegistryValueWrite HKEY_CURRENT_USER $regRoot logFbase $logFbase
sysRegistryValueWrite HKEY_CURRENT_USER $regRoot logPath $logPath

return
