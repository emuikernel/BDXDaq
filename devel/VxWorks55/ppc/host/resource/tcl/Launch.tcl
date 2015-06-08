# Launch.tcl - Launch application Tcl implementation file
#
# Copyright 1994-2002 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 04d,15apr02,fle  SPR#73266 : added note about this SPR in error messages
# 04c,08oct01,fle  unified Tornado version String
# 04b,22jun01,tpw  Utilize targetGdbSuffix to centralize exception cases for
#                  GDB naming.
# 04b,10may01,tpw  Tidy up some duplicate little thumb logic slightly different.
# 04a,18apr01,mem  update win32 registry key info for 2.1
# 03z,15dec99,jgn  update win32 registry key info for 2.0.1
# 03y,10nov99,jgn  change ARM7TDMI_T to ARMARCH4_T
# 03x,02nov99,tpw  Check specifically for thumb so we run the right gdb.
# 03x,27may99,dra  Removed conditional check for little-endian mips for
#                  bi-endian Tornado 2.
# 03x,12jun98,mem  added sparcv9 support.
# 03x,02nov99,tpw  Check specifically for thumb so we run the right gdb.
# 03w,19feb99,jmp  added startNetscapeFromUI variable to workaround (SPRs
#                  #24072, #24300).
# 03v,30nov98,jmp  removed Admin->IntallTape (SPR #23113).
# 03u,19nov98,jmp  no longer source windHelpLib.tcl on startup, source it on
#                  first windHelp invocation.
# 03t,13nov98,jmp  removed frustating error message (SPR# 23161).
# 03s,04nov98,jmp  added Manuals index entry to the help menu.
# 03r,30sep98,jmp  modified help menus to use windHelpLib instead of windman.
# 03q,21sep98,tpw  added wrsPreferredServerOrderGet, and customization hooks.
# 03p,11sep98,fle  made registry use the HOME/.wind directory for data base
#                    storage when WIND_BASE is not a writable directory
# 03o,10sep98,p_m  Updated version numbers for support reports
# 03n,02sep98,dbt  replaced simhp with simhppa.
# 03m,22jun98,fle  removed license manager related calls and procs
# 03l,15jun98,p_m  Adapted to new web pages addresses
# 03k,13may98,fle  removed windCFG from Launcher
#		   + made it wtxcore.tcl independant since wtxCpuInfoGet
# 03j,05may98,fle  made Launcher select target server when alone in list
# 03i,24mar98,fle  moved userlock directory from WIND_BASE/host/resource to
#                  WIND_BASE/.wind
#                  + history cleanup
#                  + made Launcher have a better behaviour in case of registry
#                    shutdown.
# 03h,06mar98,fle  made it able to detect SERVER_DOESNT_RESPOND error message
# 03g,19dec97,jdi  changed windman call to use -m option (-k has bug).
# 03f,03feb98,fle  made launcher refresh list when receiving
#                  REGISTRY_NAME_NOT_FOUND error
# 03e,21jan98,fle  updated for REGISTRY_SVR_IS_DEAD message receiving
# 03d,21jan97,elp  Reset build_label (unused in 1.0.1).
# 03c,14nov96,p_m  Removed Elan Web site access button
# 03b,21oct96,p_m  replaced FlexLm with Elan
# 03b,21nov96,mem  Added little endian MIPS support.
# 03a,11sep96,p_m  Added WindSurff access from support menu.
# 02z,30aug96,elp  Changed wpwrVersion{} into wtxTsVersionGet{}.
# 02y,12aug96,p_m  Added ease patch (kab) for HPSIM.
# 02x,08jul96,p_m  added agent version handling.
# 02w,17jun96,jco  fixed typo and enhanced an info message.
# 02v,25mar96,jco  generalized Ok/Cancel style for question, added support for
#		   multiple dead target server (spr# 5644).
# 02u,23may96,pad  now handles gracefully unknown CPU numbers (SPR #6591).
#		   Updated copyright line.
# 02t,20mar96,p_m  updated version for support requests and added build
#		   label.
# 02s,01feb96,jco  Changed the wtxreg return checking to make use of the new 
#	            wtxInfoQ return value (introduced by the fix of spr # 5750) 
#		    and catched the wtxInfoQ call. Changed Yes/No question to 
#		    Ok/Cancel in tsUnregister and tgtReboot routines, added a 
#		    confirmation on tgtsvr kill (spr #6073). Added globetrotter 
#		    www link in the info menu for FLEXlm product (spr #5900). 
#		    Separated Target menu entries involved with the target from 
#		    those involved with the target server, took care of Support
#		    menu entries style (trailing ...). 
# 01a,20jan95,c_s  written.
#*/

# DESCRIPTION
# This file holds the Tcl code required to set up the Launcher graphic
# interface.This code creates the main window of the Launcher and initializes
# the various objects in this window such the toolbar and application bar.
# This file also holds all the necessary procedures to get information from
# the target servers recorded in the Registry.
#
# RESOURCE FILES:
# wpwr/host/resource/tcl/wtxcore.tcl
# wpwr/host/resource/tcl/app-config/Launch/*.tcl
# wpwr/host/resource/tcl/app-config/all/host.tcl
# <user's home directory path>/.wind/launch.tcl
#

package require Wind

# globals

set tgtsvr_name		""
set tgtsvr_selected	""	;# name of target server currently in selection
set tgtsvr_cpu		""
set tgtsvr_cpuid	0
set tgtsvr_rtvers	""
set tgtsvr_host		""
set tgtsvr_bsp		""
set tgtsvr_core		""
set tgtsvr_user		""
set tgtsvr_lock		""
set deadTsSelectedSoFar ""      ;# dead target server List selected so far.

 # Target Server Poll interval: 10 seconds.  At this interval, we see if 
 # the list of active target servers has changed.

set tsPollInterval 10000	

# On HPUX, netscape cannot be started from the Launcher, Browser, Crosswind
# (See SPR# 24072, 24300). We defined startNetscapeFromUI to FALSE, so
# when the HTML help is invoked on HPUX, windHelpDisplay{} check if the
# variable startNetscapeFromUI is defined to FALSE, if yes, we display
# an error message.

set startNetscapeFromUI FALSE

################################################################################
#
# registryLaunch - Tornado Registry automatic launching
#
# If WIND_REGISTRY is the local host and no registry daemon is running, then
# we propose to exec one. If WIND_REGISTRY is a remote host and no registry is
# running, then we ask for one and display the procedure.
# If one is already running, all is right.
#
# SYNOPSIS:
# registryLaunch
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc registryLaunch {} {
    global env

    if ![file exists $env(WIND_BASE)/host/[wtxHostType]/bin/wtxreg] {
	 noticePost error "Can't find wtxreg to execute."
	 exit
    }

    # here-under catch is necessary to prevent from abortion in case
    # we get the ld warning (that raises an error under tcl)

    catch {wtxInfoQ} result
 
    if {[lsearch $result "API_REGISTRY_UNREACHABLE"] != -1 } {
	set noRegText "No Tornado Registry running on host $env(WIND_REGISTRY)"
	if {$env(WIND_REGISTRY) == [exec uname -n]} {

	    # we are running on the wind-registry host, thus 'wtxHostType' apply

	    if {[noticePost question "$noRegText.\nLaunch registry daemon on \
		$env(WIND_REGISTRY)?" "Ok" "Cancel"] == 0} {
	    	exit
	    }

	    if ![file exists $env(WIND_BASE)/host/[wtxHostType]/bin/wtxregd] {
		 noticePost error "Can't find wtxregd to execute."
		 exit

	    } else {

		# process in background: no catch to prevents from possibly 
		# ld warning.

		if { [file exists $env(WIND_BASE)] && \
		     [file writable $env(WIND_BASE)] } {

		    # the registry data base file is saved in the 
		    # WIND_BASE/.wind directory (.wind directory is created by
		    # the registry if not existing)

		    exec $env(WIND_BASE)/host/[wtxHostType]/bin/wtxregd &

		} else {

		    # the WIND_BASE directory is not writable or does not exist
		    # save the registry database file in the HOME/.wind
		    # directory (wtxregd will create it if does not exist)

		    set homePath "[file dirname ~]/[file tail ~]"
		    if { [noticePost question \
				     "The $env(WIND_BASE) directory is not\
				      writable or does not exist.\n\nThe\
				      registry Data Base File is being written\
				      in your\n$homePath/.wind \ndirectory" \
				      "OK" "Exit"] 
			 == 0} {

			 exit
		    }

		    exec $env(WIND_BASE)/host/[wtxHostType]/bin/wtxregd \
			 -d $homePath/.wind &
		}
	    }

	} else {

	    # wind-registry host is a remote one and daemon auto-start is
	    # more tricky, we leave this to the user (for now).

	    noticePost warning "$noRegText.\n\
			  Please, start the registry daemon on \
			  $env(WIND_REGISTRY) before\n\
			  using the Launcher:\n    1-make sure Tornado is \
			  available at your site\n    for the host type of \
			  $env(WIND_REGISTRY),\n    2-go down to the  proper \
			  Tornado bin directory,\n    3-type:\n\    wtxregd\
			  &\nthen start again the Launcher on your host."
	    exit
	}
    } 
}

################################################################################
#
# createForms - create all menus and buttons
#
# This routine create all the required menus and buttons of the launcher.
#
# SYNOPSIS:
# createForms
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc createForms {} {

    # Describe main menu bar and items in menus.

    menuCreate 		File	F
    menuCreate 		Target	T
    menuCreate 		Support	S
    menuCreate 		Admin	A
    menuCreate 		Info	I
    menuCreate		About	b
    menuCreate -help	Help	H

    menuButtonCreate	File	Quit		Q	{launchQuit}
    menuButtonCreate	About	"Tornado"	T	{tornadoVersion}
    
    menuButtonCreate	Target	"Create..."	C	{tgtsvrPost none}
    menuButtonCreate	Target	Unregister	n	{tsUnregister}
    menuButtonCreate	Target	Reattach	a	{tsTargetAttach}
    menuButtonCreate	Target	Reserve		R	{tsLock}
    menuButtonCreate	Target	Unreserve	U	{tsUnlock}
#   menuButtonCreate	Target	Restart		s	{tsRestart}
    menuButtonCreate	Target	Kill		K	{tsKill}
    menuButtonCreate    Target  -
    menuButtonCreate	Target	Reboot		b	{tgtReboot}

    menuButtonCreate    Support WindSurf	W	{webPage WindSurf}
    menuButtonCreate    Support -
    menuButtonCreate	Support	"Tornado..."	T	{supportRequest Tornado}
    menuButtonCreate	Support	"VxWorks..."	V	{supportRequest VxWorks}
    menuButtonCreate	Support	"CrossWind..."	C     {supportRequest CrossWind}
    menuButtonCreate	Support	"GNU..."		G	{supportRequest GNU}
    
    menuButtonCreate	Admin	"Install CD"	C	{installCD}
    menuButtonCreate	Admin	"FTP WRS"	F	{ftpWrsConnect}
    menuButtonCreate	Admin	Authorize	A	{authorize}
    
    menuButtonCreate	Info	Products	P	{webPage product}
    menuButtonCreate	Info	Training	T	{webPage training}
    menuButtonCreate	Info	Announcements	A	{webPage announce}
    menuButtonCreate	Info	"User's Group"	U	{webPage usrgroup}
    menuButtonCreate	Info	www.wrs.com	w	{webPage wrs}
    menuButtonCreate	Info	comp.os.vxworks	c	{webPage news}


    menuButtonCreate	Help	"On launch"		O	{launchHelp}
    menuButtonCreate	Help	"Manuals contents"	M	{tornadoHelp}
    menuButtonCreate	Help	"Manuals index"		i	{windmanHelp}


    # Describe toolbar buttons

    toolBarItemCreate	tsCreate	button	{tgtsvrPost none}
    toolBarItemCreate	tsUnregister	button	{tsUnregister}
    toolBarItemCreate	tsReattach	button	{tsTargetAttach}
    toolBarItemCreate	tsReserve	button	{tsLock}
    toolBarItemCreate	tsUnreserve	button	{tsUnlock}
    toolBarItemCreate	tsKill		button	{tsKill}
    toolBarItemCreate	tsReboot	button	{tgtReboot}

    # Describe default application bar

    objectCreate app shell       WindSh 	{launchWindSh}
    objectCreate app crosswind   CrossWind	{launchCrossWind} 
    objectCreate app browser     Browser	{launchBrowser}
}

##############################################################################
#
# targetServerSelect - display info about selected target server
#
# This routine displays information about the target server the user selected,
# in the launcher's information panel.
#
# SYNOPSIS:
# targetServerSelect ts
#
# PARAMETERS:
# ts: the name of a target server
#
# RETURNS: N/A
#
# ERRORS: N/A
#
# INTERNAL
# XXX : fle : with new registry taking care of dead target server,
# wtxToolAttach can't fail on a TCP timeout. Implementation of list of
# "dead so far" target servers is now part of the registry. We keep "dead so
# far" list and all the messages that come with it for Tempest, but it has to be
# removed for Tornado V.3.0
#
# This mechanism is kept to take care of launcher V2.0 using registry V1.0
# that doesn't send a WTX_ERR_SVR_IS_DEAD or WTX_ERR_SVR_DOESNT_RESPOND
#

proc targetServerSelect {args} {
    global tgtsvr_selected
    global tgtsvr_cpu
    global tgtsvr_cpuid
    global tgtsvr_rtvers
    global tgtsvr_bsp
    global tgtsvr_core
    global tgtsvr_user
    global tgtsvr_lock
    global deadTsSelectedSoFar

    set ts [lindex $args 0]
    set onUpdate [lindex $args 1]

    # highlight selected target server

    set tgtsvr_selected $ts
    listBoxSelect tsPick $tgtsvr_selected

    set deadIndex [lsearch $deadTsSelectedSoFar $ts]

    if {$onUpdate == 1 && $deadIndex != -1} {
	textWindowSet tsInfo "Target Server not responding.\n\nNOTE :\n    If\
			      the problem persists, please\n    see the\
			      Tornado Migration Guide,\n    section \"Tornado\
			      Registry\""
        return
    }

    if {$deadIndex != -1} {
        if ![noticePost question \
            "Last attachment to this Target Server has failed.\nTry again?" \
	    "Ok" "Cancel"] {
            return
        }
    }

    # update target server list

    if {[catch {wtxToolAttach $ts -launcher} attachRetVal]} {

	if { ([regexp {SVR_IS_DEAD} $attachRetVal]) || \
	     ([regexp {NAME_NOT_FOUND} $attachRetVal]) } {

	    # target server is dead (no rpc connection can be done on it).
	    # registry has unregistered it, so just update server list
	    # or it is not anymore in registry (name not found), so it is dead.

	    targetServerListUpdate 1
	    return
	} elseif { ([regexp {SVR_DOESNT_RESPOND} $attachRetVal]) } {
	    
	    # server is not responding and kept in list for the moment.

	    textWindowSet tsInfo "Target Server not responding.\n\nNOTE :\
				  \n    If the problem persists, please\
				  \n    see the Tornado Migration Guide,\
				  \n    section \"Tornado\ Registry\""

	    if {$deadIndex == -1} {
		lappend deadTsSelectedSoFar $ts
	    }
	    return
	} else {

	    # server is alive, but we can't connect. It is still registered,
	    # keep it in memory and wait for another connection try.

	    textWindowSet tsInfo "Attach failed.\n$attachRetVal"
	    if {$deadIndex == -1} {
		lappend deadTsSelectedSoFar $ts
	    }
	    return
	}
    }

    # if the selected Target Server was previously not responding 
    # is now responding it should no longer be in the dead list.

    if {$deadIndex != -1} {
        set deadTsSelectedSoFar \
            [lreplace $deadTsSelectedSoFar $deadIndex $deadIndex]
    }

    # Check that Target is attached 

    if [catch "wtxTsInfoGet" info] {
	if {[lindex $info 3] == "(SVR_TARGET_NOT_ATTACHED)"} {
	    # clear tgtsvr_lock  
	    set tgtsvr_lock	""

	    textWindowSet tsInfo "Target not Attached"
	    wtxToolDetach -all
	    return
	} else {
	    # resubmit unexpected error
	    error $info
	}
    }
    set link		[lindex $info 0]
    set typeId		[lindex [lindex $info 2] 0]

    # Check if we can bind the CPU number to a CPU name. If not,
    # use the default name: UNKNOWN

    set tgtsvr_cpu [wtxCpuInfoGet -n $typeId]
    if { $tgtsvr_cpu == "" } {
	set tgtsvr_cpu "UNKNOWN"
    }

    set tgtsvr_cpuid    $typeId
    set tgtsvr_rtvers	"VxWorks [lindex [lindex $info 3] 1]"
    set tgtsvr_bsp	[lindex $info 4]
    set tgtsvr_core	[lindex $info 5]

    set memSize		[lindex [lindex $info 6] 1]
    set tgtsvr_user	[lindex $info 8]
    set start		[lindex $info 9]
    set access		[lindex $info 10]
    set tgtsvr_lock	[lindex $info 11]
    set tgtsvr_ver      [lindex $info 13]
    set agentVersion	"WDB [lindex $info 14]"

    set infoString              "Name   : $ts"
    set infoString "$infoString\nVersion: $tgtsvr_ver"
    set infoString "$infoString\nStatus : $tgtsvr_lock"
    set infoString "$infoString\nRuntime: $tgtsvr_rtvers"
    set infoString "$infoString\nAgent  : $agentVersion"
    set infoString "$infoString\nCPU    : $tgtsvr_cpu"
    set infoString "$infoString\nBSP    : $tgtsvr_bsp"
    set infoString "$infoString\nMemory : [format "%#x" $memSize]"
    set infoString "$infoString\nLink   : $link"
    set infoString "$infoString\nUser   : $tgtsvr_user"
    set infoString "$infoString\nStart  : $start"
    set infoString "$infoString\nLast   : $access"

    set toolListString "Attached Tools :\n"
    set toolList [lrange $info 15 end]

    foreach tool $toolList {
	if {[lindex $tool 4] != ""} {
	    set toolUser "([lindex $tool 4])"
	} else {
	    set toolUser ""
	}

	set toolListString \
		"$toolListString    [lindex $tool 1] $toolUser\n"
    }

    set infoString "$infoString\n$toolListString"

    textWindowSet tsInfo $infoString
    wtxToolDetach -all
}

##############################################################################
#
# wrsPreferredServerOrderGet - default target server sorting algorithm
#
# This routine sorts the target servers names into the launcher's target
# servers list window. Target servers on the local host are listed first,
# followed by all others, in alphabetical order.
#
# SYNOPSIS:
#   wrsPreferredServerOrderGet serverList
#
# PARAMETERS:
#   serverList : list of target servers to sort
#
# RETURNS: sorted list of target servers
#
# ERRORS: N/A
#

proc wrsPreferredServerOrderGet { serverList } {

    set localList ""
    set sortedList ""
    set launcherHostName [info hostname]

    # separate server list into local servers and others

    foreach server $serverList {
	set tgtsvrHostName [lindex [split $server @] 1]
	if { $tgtsvrHostName == $launcherHostName } {
	    lappend localList $server
	} else {
	    lappend sortedList $server
	}
    }

    # sort each list alphabetically

    set localList [lsort $localList]
    set sortedList [lsort $sortedList]

    # append the others to the local list, and return the result.

    foreach server $sortedList {
	lappend localList $server
    }

    return $localList
}

##############################################################################
#
# targetServerListUpdate - update the target server list
#
# This routine updates the target server list in the launcher's targets panel.
# It registred itself in the update mechanism so that it is called at fixed
# intervals.
#
# SYNOPSIS:
# targetServerListUpdate once
#
# PARAMETERS:
# once: 0 if the routine must registred itself in the update mechanism, 1 else.
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc targetServerListUpdate {once} {
    global tsPollInterval
    global tgtsvr_selected

    # get all the target server names from registry

    if [catch {wtxInfoQ .* tgtsvr} lstSvr] {
	if {$lstSvr == "REGISTRY_NAME_NOT_FOUND"} {
	    set lstSvr ""
	} elseif { $lstSvr == "API_REGISTRY_UNREACHABLE" } {
	    noticePost info "Tornado registry is unreachable.\n\nPlease contact\
			     your Tornado administrator to restart it if needed"
	    return
	} else {
	    error $lstSvr
	}
    }

    # make target servers list

    set servList ""
    foreach server $lstSvr { 
	lappend servList [lindex $server 0]
    }

    # use user defined sorting proc if it exists, else default sorting proc

    if { [info procs userPreferredServerOrderGet] != "" } {
      set servList [userPreferredServerOrderGet $servList]
    } else {
      set servList [wrsPreferredServerOrderGet $servList]
    }

    # select alone target server if not selected

    if { ([llength $servList] == 1) && ($tgtsvr_selected == "") } {
	set tgtsvr_selected [lindex $servList 0]
	targetServerSelect $tgtsvr_selected 1
    }

    # refresh information window

    listBoxUpdateItems tsPick $servList

    if {$tsPollInterval > 0 && !$once} {
	delayedEval $tsPollInterval "targetServerListUpdate 0"
    }

    set tgtsvr_selected_new ""

    if {$tgtsvr_selected != ""} {
	foreach serverName $servList {
	    if {$serverName ==  $tgtsvr_selected} {
		set tgtsvr_selected_new $serverName
	    }
	}
	
	set tgtsvr_selected $tgtsvr_selected_new

	if {$tgtsvr_selected == ""} {
	    textWindowSet tsInfo ""
	} else {
	    targetServerSelect $tgtsvr_selected 1
	}
    }
}

##############################################################################
#
# launchWindSh - launch the WindSh application
#
# This routine launches the WindSh application (Tornado Core Tool) within a
# terminal so that the shell appears in its own independent window.
#
# SYNOPSIS:
# launchWindSh
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc launchWindSh {} {
    global tgtsvr_selected
    global tgtsvr_user
    global tgtsvr_lock

    if {$tgtsvr_selected == ""} {
	noticePost error "Select a target first."
	return
    }

    if {$tgtsvr_lock == "locked"} {
	noticePost error "Target Server locked by $tgtsvr_user."
	return
    }

    if {$tgtsvr_lock == "unauthorized"} {
	noticePost error "Access not authorized."
	return
    }

    runTerminal "windsh: $tgtsvr_selected" 1 windsh $tgtsvr_selected
}

##############################################################################
#
# launchCrossWind - launch the CrossWind application
#
# This routine launches the CrossWind application (Tornado Core Tool).
#
# SYNOPSIS:
# launchCrossWind
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc launchCrossWind {} {
    global tgtsvr_selected
    global tgtsvr_cpuid

    if {$tgtsvr_selected == "" || $tgtsvr_cpuid == 0} {
	noticePost error "Select an attached target first."
	return
    }

    set family [targetGdbSuffix $tgtsvr_cpuid]

    # Try to locate debugger for this architecture family.
    set binPath [wtxPath host %H bin]
    set exePath ${binPath}gdb$family

    if [file executable $exePath] {
	set startFileName /tmp/cwstartup.[pid]
	if [catch {open $startFileName w} file] {
	    # couldn't create a crosswind startup file.  Oh, well.
	    exec ${binPath}crosswind -X $exePath &
	} else {
	    # write out a little /tmp file that attaches to the 
	    # selected target server and then deletes itself.

	    puts $file "set wtx-tool-name crosswind"
	    puts $file "target wtx $tgtsvr_selected"
	    puts $file "tcl exec rm $startFileName"
	    close $file

	    exec ${binPath}crosswind -X $exePath -x $startFileName &
	}
    } else {
	noticePost error "Can't find gdb$family to execute."
	return
    }
}

##############################################################################
#
# launchBrowser - launch the Browser application
#
# This routine launches the Browser application (Tornado Core Tool).
#
# SYNOPSIS:
# launchBrowser
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc launchBrowser {} {
    global tgtsvr_selected
    global tgtsvr_user
    global tgtsvr_lock
    
    if {$tgtsvr_selected == ""} {
	noticePost error "Select a target first."
	return
    }

    if {$tgtsvr_lock == "locked"} {
	noticePost error "Target Server locked by $tgtsvr_user."
	return
    }

    if {$tgtsvr_lock == "unauthorized"} {
	noticePost error "Access not authorized."
	return
    }

    exec browser $tgtsvr_selected &
}

##############################################################################
#
# authorize - edit the target server access permission file
#
# This routine spawns an editor in a terminal window in order to let the user
# edit the access permission file (userlock) of the Target Servers.
#
# SYNOPSIS:
# authorize
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc authorize {} {

    editFile -title "Authorize Users" [wtxPath .wind]userlock
    }

##############################################################################
#
# tsLock - reserve the selected target server
#
# This routine reserves the access to the selected target server to the
# launcher's user. No tool will be allowed to attach this target server.
#
# SYNOPSIS:
# tsLock
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc tsLock {} {
    global tgtsvr_selected
    global tgtsvr_user
    global tgtsvr_lock

    if {$tgtsvr_selected == ""} {
	noticePost error "Select a target first."
	return
    }

    if {$tgtsvr_lock == "locked"} {
	noticePost error "Target Server locked by $tgtsvr_user."
	return
    }

    if {$tgtsvr_lock == "unauthorized"} {
	noticePost error "Access not authorized."
	return
    }

    if {[catch {wtxToolAttach $tgtsvr_selected -launcher} result]} {

	set errMsg "Can't communicate with target server \"$tgtsvr_selected\""

	# if it is a "SVR_DOESNT_RESPOND" error, talk about SPR#73266

	if {[regexp "SVR_DOESNT_RESPOND" $result]} {
	    append errMsg "Target Server not responding.\n\nNOTE :\n    If\
			   the problem persists, please\n    see the\
			   Tornado Migration Guide,\n    section \"Tornado\
			   Registry\""
	}

	noticePost error $errMsg
	return
    }

    wtxTsLock
    wtxToolDetach -all
    targetServerSelect $tgtsvr_selected 0
}

##############################################################################
#
# tsUnlock - unreserve the selected target server
#
# This routine unreserves the access to the selected target server. Any user
# will be allowed to attach a tool to this target server.
#
# SYNOPSIS:
# tsUnlock
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc tsUnlock {} {
    global tgtsvr_selected
    global tgtsvr_user
    global tgtsvr_lock

    if {$tgtsvr_selected == ""} {
	noticePost error "Select a target first."
	return
    }

    if {$tgtsvr_lock == "locked"} {
	noticePost error "Target Server locked by $tgtsvr_user."
	return
    }

    if {$tgtsvr_lock == "unauthorized"} {
	noticePost error "Access not authorized."
	return
    }

    if {[catch {wtxToolAttach $tgtsvr_selected -launcher} result]} {

	set errMsg "Can't communicate with target server \"$tgtsvr_selected\""

	# if it is a "SVR_DOESNT_RESPOND" error, talk about SPR#73266

	if {[regexp "SVR_DOESNT_RESPOND" $result]} {
	    append errMsg "Target Server not responding.\n\nNOTE :\n    If\
			   the problem persists, please\n    see the\
			   Tornado Migration Guide,\n    section \"Tornado\
			   Registry\""
	}

	noticePost error $errMsg
	return
    }

    wtxTsUnlock
    wtxToolDetach -all
    targetServerSelect $tgtsvr_selected 0
}

##############################################################################
#
# tsTargetAttach - attach the selected target server to its target
#
# This routine attaches the selected target server to its target. This is
# useful when the connection between the target server and the target agent
# has been lost but is up again.
#
# SYNOPSIS:
# tsTargetAttach
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc tsTargetAttach {} {
    global tgtsvr_selected
    global tgtsvr_user
    global tgtsvr_lock

    if {$tgtsvr_selected == ""} {
	noticePost error "Select a target first."
	return
    }

    if {$tgtsvr_lock == "locked"} {
	noticePost error "Target Server locked by $tgtsvr_user."
	return
    }

    if {$tgtsvr_lock == "unauthorized"} {
	noticePost error "Access not authorized."
	return
    }

    if {[catch {wtxToolAttach $tgtsvr_selected -launcher} result]} {

	set errMsg "Can't communicate with target server \"$tgtsvr_selected\""

	# if it is a "SVR_DOESNT_RESPOND" error, talk about SPR#73266

	if {[regexp "SVR_DOESNT_RESPOND" $result]} {
	    append errMsg "Target Server not responding.\n\nNOTE :\n    If\
			   the problem persists, please\n    see the\
			   Tornado Migration Guide,\n    section \"Tornado\
			   Registry\""
	}

	noticePost error $errMsg
	return
    }

    # attempt to reattach to the Target 

    catch "wtxTargetAttach" attachResult

    if {[lindex $attachResult 3] != "(SVR_TARGET_NOT_ATTACHED)"} {
	targetServerSelect $tgtsvr_selected 0
    }
}

##############################################################################
#
# tsUnregister - unregister the selected target server
#
# This routine unregisters the selected target server from the registry.
# The target server must be dead.
#
# SYNOPSIS:
# tsUnregister
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc tsUnregister {} {
    global tgtsvr_selected
    global tgtsvr_user
    global tgtsvr_lock
    global deadTsSelectedSoFar

    set deadListLength [llength $deadTsSelectedSoFar]

    if {$deadListLength == 0} {
        noticePost info "Selected Target Server is running and thus can't \
	    be Unregistered!"
        return
    }

    # update target server list

    set dontClean 0
    if {[lsearch $deadTsSelectedSoFar $tgtsvr_selected] == -1} {
	set varWordsList {candidate_s it_them}
	if {$deadListLength == 1} {
	    bindNamesToList $varWordsList {candidate it}
	} else {
	    bindNamesToList $varWordsList {candidates them}
	}
        if ![noticePost question \
             "Selected Target Server is running and thus can't
be Unregistered but there exists $deadListLength $candidate_s
for unregistration.\n\nDo you want to unregister $it_them?" \
	"Ok" "Cancel"] {
        return
        }
        set dontClean 1
    }

    # Ask for confirmation.

    set varWordsList {is_are it_them It_They the_these server_s}
    if {$deadListLength == 1} {
	bindNamesToList $varWordsList {is it It the Server}
    } else {
	bindNamesToList $varWordsList {are them They these Servers}
    }

    if ![noticePost question \
        "Target $server_s $deadTsSelectedSoFar $is_are not responding.
$It_They may simply be too busy to answer.  If $the_these Target $server_s
$is_are known to have exited, we suggest you remove $it_them 
from the Launcher.\n\nWould you like to remove $it_them now?" \
	"Ok" "Cancel"] {
        return
    }

    # unregister the Target Server from the WTX registry

   foreach dts $deadTsSelectedSoFar {
        if {[catch {wtxUnregister $dts} result]} {
            noticePost error "Can't communicate with the WTX registry."
            return
        }
        set ix [lsearch deadTsSelectedSoFar $dts]
        set deadTsSelectedSoFar [lreplace $deadTsSelectedSoFar $ix $ix]
    }

    set deadTsSelectedSoFar ""

    # Clean up info window.

    if {$dontClean == 0} {
        textWindowSet tsInfo ""
    }

    # update Target Server list

    targetServerListUpdate 1
}

##############################################################################
#
# tsRestart - restart the selected target server
#
# This routine restarts the selected target server. the previous target server
# state (symbol table, loaded module, allocated memory for agent pool) is lost.
# This may bring inconsistencies between the Target Server and teh attached
# target.
#
# SYNOPSIS:
# tsRestart
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc tsRestart {} {
    global tgtsvr_selected
    global tgtsvr_user
    global tgtsvr_lock

    if {$tgtsvr_selected == ""} {
	noticePost error "Select a target first."
	return
    }

    if {$tgtsvr_lock == "locked"} {
	noticePost error "Target Server locked by $tgtsvr_user."
	return
    }

    if {$tgtsvr_lock == "unauthorized"} {
	noticePost error "Access not authorized."
	return
    }

    if {[catch {wtxToolAttach $tgtsvr_selected -launcher} result]} {

	set errMsg "Can't communicate with target server \"$tgtsvr_selected\""

	# if it is a "SVR_DOESNT_RESPOND" error, talk about SPR#73266

	if {[regexp "SVR_DOESNT_RESPOND" $result]} {
	    append errMsg "Target Server not responding.\n\nNOTE :\n    If\
			   the problem persists, please\n    see the\
			   Tornado Migration Guide,\n    section \"Tornado\
			   Registry\""
	}

	noticePost error $errMsg
	return
    }

    wtxTsKill "restart"
    targetServerSelect $tgtsvr_selected 0
}

##############################################################################
#
# tsKill - kill the selected target server
#
# This routine kills the selected target server and removes it from the
# launcher's panels.
#
# SYNOPSIS:
# tsKill
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc tsKill {} {
    global tgtsvr_selected
    global tgtsvr_user
    global tgtsvr_lock
    global tgtsvr_name
    global tgtsvr_rtvers
    global tgtsvr_cpu
    global tgtsvr_cpuid
    global tgtsvr_bsp

    if {$tgtsvr_selected == ""} {
	noticePost error "Select a target first."
	return
    }

    if {$tgtsvr_lock == "locked"} {
	noticePost error "Target Server locked by $tgtsvr_user."
	return
    }

    if {$tgtsvr_lock == "unauthorized"} {
	noticePost error "Access not authorized."
	return
    }

    if ![noticePost question \
	"Really kill target server $tgtsvr_selected?" \
	"Ok" "Cancel"] {
	return
    }

    if {[catch {wtxToolAttach $tgtsvr_selected -launcher} result]} {

	set errMsg "Can't communicate with target server \"$tgtsvr_selected\""

	# if it is a "SVR_DOESNT_RESPOND" error, talk about SPR#73266

	if {[regexp "SVR_DOESNT_RESPOND" $result]} {
	    append errMsg "Target Server not responding.\n\nNOTE :\n    If\
			   the problem persists, please\n    see the\
			   Tornado Migration Guide,\n    section \"Tornado\
			   Registry\""
	}

	noticePost error $errMsg
	return
    }

    wtxTsKill "destroy"
    catch {wtxToolDetach -all}
    set tgtsvr_selected 	""
    set tgtsvr_name		""
    set tgtsvr_rtvers		""
    set tgtsvr_cpu		""
    set tgtsvr_cpuid		0
    set tgtsvr_core		""
    set tgtsvr_bsp		""
    textWindowSet tsInfo	""
    exec sleep 1
    targetServerListUpdate 1
}

##############################################################################
#
# webPage - connect WRS Web page
#
# This routine spawns a Web browser connected to the appropriate WRS page,
# depending on the keyword given as parameter.
#
# SYNOPSIS:
# webPage page
#
# PARAMETERS:
# page: one of the following keywords: product, announce, wrs, training,
#       usrgroup, news
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc webPage {page} {
    global docDir

    case $page {
	product		{set webpage "http://www.wrs.com/products/index.html"}
	announce	{set webpage "http://www.wrs.com/html/whatsnewindex.html"}
	wrs		{set webpage "http://www.wrs.com/"}
	training	{set webpage "http://www.wrs.com/training/index.html"}
	usrgroup	{set webpage "http://www.wrs.com/corporate/html/usrgroup.html"}
    	news		{set webpage "news:comp.os.vxworks"}
	WindSurf	{set webpage "http://www.wrs.com/corporate/html/tsmain.html"}
    }

    if {[info proc windHelpDisplay] == ""} {
	# Source windHelp library
	uplevel #0 source [wtxPath host resource doctools]windHelpLib.tcl
    }

    windHelpDisplay $webpage
}

##############################################################################
#
# supportRequest - give access to support dialogs
#
# This routine posts the appropriate support dialog, depending on the keyword
# given as parameter.
#
# SYNOPSIS:
# supportRequest product
#
# PARAMETERS:
# product: a string which format is: "<product name> <product version>"
#          Some predetermined keywords exist: Tornado, VxWorks, CrossWind, GNU
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc supportRequest {product} {
    global env
    global tgtsvr_rtvers
    global tgtsvr_cpu
    global tgtsvr_bsp

    # Post support request dialog with appropriate product/version string
    # for the core products.  Optional products should call supportRequest
    # with $product set to the product name and version, and also add a 
    # pulldown menu item to "Support".

    if {$tgtsvr_rtvers == "" } { 
	set rtvers "VxWorks ??"
    } else {
	set rtvers $tgtsvr_rtvers
    }
    if {$tgtsvr_cpu == "" } { 
	if [catch {set cpu $env(VX_CPU_FAMILY)}] {
	    set cpu ""
	}
    } else {
	set cpu $tgtsvr_cpu
    }
    if {$tgtsvr_bsp == "" } { 
	set bsp "n/a"
    } else {
	set bsp $tgtsvr_bsp
    }

    set build_label	""
    set host		[exec uname -a]

    case $product {
	Tornado		{
			set torver [wtxTsVersionGet]
			supportPost "Tornado $torver $build_label"	"$cpu" "$host" "$bsp" 
			}
	VxWorks		{supportPost "$rtvers"		"$cpu" "$host" "$bsp" }
	CrossWind	{supportPost "CrossWind for Tornado [Wind::version] $build_label"	"$cpu" "$host" "$bsp" }
	GNU		{supportPost "GNU 2.7 $build_label"	"$cpu" "$host" "$bsp" }
	*		{supportPost "$product"		"$cpu" "$host" "$bsp" }
    }
}

##############################################################################
#
# launchQuit - quit the launcher.
#
# This routine posts a question dialog. The user may answer "Quit" to quit
# the launcher.
#
# SYNOPSIS:
# launchQuit
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc launchQuit {} {
	if { [noticePost question \
	    "Quit now?" "Ok" "Cancel"] == 1} {
	    exit
	} else {
	    return
	}
}


##############################################################################
#
# tgtReboot - reboot the attached target 
#
# This procedure sends a reboot request to the target attached to the specified
# target server. This makes the target server and all tools restarting.
#
# SYNOPSIS:
# tgtReboot
#
# PARAMETER: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc tgtReboot {} {
    global tgtsvr_selected
    global tgtsvr_user
    global tgtsvr_lock

    if {$tgtsvr_selected == ""} {
	noticePost error "Select a target first."
	return
    }

    if {$tgtsvr_lock == "locked"} {
	noticePost error "Target Server locked by $tgtsvr_user."
	return
    }

    if {$tgtsvr_lock == "unauthorized"} {
	noticePost error "Access not authorized."
	return
    }

    set question "Really reboot target connected to $tgtsvr_selected ?"
    if ![noticePost question \
	"Really reboot target connected to $tgtsvr_selected?" \
	"Ok" "Cancel"] {
	return
    }

    if {[catch {wtxToolAttach $tgtsvr_selected -launcher} result]} {

	set errMsg "Can't communicate with target server \"$tgtsvr_selected\""

	# if it is a "SVR_DOESNT_RESPOND" error, talk about SPR#73266

	if {[regexp "SVR_DOESNT_RESPOND" $result]} {
	    append errMsg "Target Server not responding.\n\nNOTE :\n    If\
			   the problem persists, please\n    see the\
			   Tornado Migration Guide,\n    section \"Tornado\
			   Registry\""
	}

	noticePost error $errMsg
	return
    }

    if [catch {wtxTargetReset} result] {
	if {[wtxErrorName $result] != "AGENT_COMMUNICATION_ERROR"} {
	    error $result
	}
    }
}

##############################################################################
#
# launchHelp - get help about the launcher
#
# This procedure runs the windHelp tool, set on the launch command.
#
# SYNOPSIS:
# launchHelp
#
# PARAMETER: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc launchHelp {} {

    if {[info proc windHelpDisplay] == ""} {
	# Source windHelp library
	uplevel #0 source [wtxPath host resource doctools]windHelpLib.tcl
    }

    set htmlAddr [windHelpLinkGet -name launch]

    if {$htmlAddr != ""} {
	windHelpDisplay $htmlAddr
    } else {
    	error "Tornado help files not installed"
    }
}

##############################################################################
#
# tornadoHelp - get help about Tornado.
#
# This procedure runs the windHelp tool.
#
# SYNOPSIS:
# tornadoHelp
#
# PARAMETER: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc tornadoHelp {} {
    global docDir

    if {[info proc windHelpDisplay] == ""} {
	# Source windHelp library
	uplevel #0 source [wtxPath host resource doctools]windHelpLib.tcl
    }

    windHelpDisplay $docDir/books.html
}


################################################################################
#
# windmanHelp - launch windman
#
# This procedure runs the windman tool
#
# SYNOPSIS:
#	windmanHelp
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc windmanHelp {} {
    exec windman &
}

################################################################################
#
# Launch.tcl - Initialization
#
# This part of the file does all the necessary initialization and sourcing to
# get a runable launch application.
# The user's resource file, if any, is also sourced here.
#

# Check if the registry daemon exists somewhere and launch one if required

registryLaunch

# Create all the buttons and menus

createForms

# Source core procedures

source [wtxPath host resource tcl]wtxcore.tcl

# Read in optional Launch features

set cfgdir [wtxPath host resource tcl app-config Launch]*.tcl
foreach file [lsort [glob -nocomplain $cfgdir]] {source $file}
source [wtxPath host resource tcl app-config all]host.tcl

targetServerListUpdate 0
 
# reads user definitions to overide WRS defaults.

if [file exists $env(HOME)/.wind/launch.tcl] {
    source $env(HOME)/.wind/launch.tcl
}
