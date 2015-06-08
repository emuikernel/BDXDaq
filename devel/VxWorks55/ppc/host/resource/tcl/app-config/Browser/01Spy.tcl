# 01Spy.tcl - task spy reporting
#
# Copyright 1994-2001 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01p,05dec01,tam  fix computation of spyLib.o path for repackaging
# 01o,02oct01,fmk  move hardcoding for taskTicks offset to sh-<arch>.tcl files
# 01n,09jun98,dbt  check if the target OS is initialized before proceeding.
# 01m,23apr98,elp  fix kernel and interrupt value inversion (SPR# 20304).
# 01l,07apr98,elp  get target info in 1 shot (fix SPR# 20735).
# 01k,18mar98,jmb  HPSIM merge of Mark Mason patch from 21nov96:
#                  Replaced hard-coded gnu with [targetTool] call.
# 01j,21oct96,elp  Made sure auxiliary clock is available before downloading
#                  spyLib.o.
# 01i,09nov95,pad  Fixed a bug that forced the update routine to be evaluated
#		   twice when spy started while the broser's auto update was
#		   on (SPR #4612).
#		   Cosmetic changes to follow WRS Tcl coding conventions.
# 01h,08nov95,pad  Reworked spyTaskSetup{} and spyStart{} for a better error
#		   handling.
# 01g,09oct95,pad  Added reference to Target Server in window title (SPR #5110).
# 01f,30jun95,pad  enhanced robustness so that if spyLib.o is removed from
#                  windsh, the spy componant reloads it when started.
#                  Takes care of agent mode before running the spy task.
# 01g,29jun95,pad  now really stop spy when spy window is closed from menu.
# 01f,15jun95,pad  spy window no longer pops up every time it is updated.
# 01e,07jun95,f_v  now don't unload spyLib.o in spyStop procedure
# 01d,06jun95,f_v  changed taskReport implementation
# 01c,23may95,pad  imported spy related stuff from Browser.tcl
# 01b,11may95,f_v  move few line from spyTaskSetup to spyStart
# 01a,06dec94,f_v  create GUI for spy,spy body code and add delta toolbar icon
#*/

# DESCRIPTION
# This module holds the code which manages the spy facility from the browser.
# It creates the spy chart window and adds a button in the browser toolbar.
# This button is a toggle button that starts or stops the spy utility.
# The spy chart window can be removed using the window manager.
# The default frequency and interval for the spy task can be changed with
# the browser configuration dialog.
# 
# RESOURCE FILES: N/A
#

# Globals

set spyRunning		0	;# spy is not running by default
set spyOnScreen		0	;# spy is not on screen by default
set spyFrequency	100	;# default spy frequency is 100 Hz
set spyInterval		5000	;# default spy interval is 5 seconds
set displayValue	1	;# default mode is cumulative

##############################################################################
#
# spyInit - miscellaneous tool initialization
# 
# This procedure prepares for the usage of spy.
#
# SYNOPSIS:
# spyInit
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc spyInit {} {

   # Create spy button in the browser.

    toolBarItemCreate	spy	button	{spyToggleProc}

    # Create spy chart. The chart is closed from the window manager level.

    chartCreate spyChart -destroy spyChartDestroyed -valuefmt %d%% -size 300 600

    # record the spyTaskReport so that the chart is updated regularly

    browserAuxProcAdd spyTaskReport
}

##############################################################################
#
# spyTaskSetup - prepare target side
#
# This routine loads, if necessary, the appropriate spyLib.o module for the
# target architecture
#
# SYNOPSIS:
#   spyTaskSetup  
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS:
# File <path>/spyLib.o is missing: spyLib.o couldn't be found.
# Auxiliary clock is not available.
# The target system is not correctly initialized yet ...
#

proc spyTaskSetup {} {
    global __wtxCpuType

    # Check that the target system has been correctely initialized before
    # proceeding

    if {! [targetInitCheck]} {
	noticePost error "The target system is not initialized yet.\
				Can't proceed."
	return -1
    }

    # Determine if it is necessary to load spyLib.o. This is done by
    # checking if the symbol "spyCommon" exists in the target symbol table.

    if [catch "wtxSymFind -name sysAuxClkEnable" status] {
    	if {[wtxErrorName $status] == "SYMTBL_SYMBOL_NOT_FOUND"} {
	    
	    # Aux clock is unavailable, spy can not run

	    noticePost error "Auxiliary clock is not available."
	    return -1
	} else {
	    error $status
	    return -1
	}
    }

    if [catch "wtxSymFind -name spyCommon" status] {
	if {[wtxErrorName $status] == "SYMTBL_SYMBOL_NOT_FOUND"} {

	    # Symbol "spyCommon" does not exist: the module must be loaded.
	    # Verify if spyLib.o exists in right directory, but first
	    # compute the path to spyLib.o

            set tool [targetTool]
            if {[string first gnu $tool] != -1} {
                set toolType gnu
            } elseif {[string first diab $tool] != -1} {
                set toolType diab
            } else {
                noticePost error "Unknown tool type $tool. Tool type must be \
                gnu or diab.\n Cannot construct path to load spyLib.o."
            }

            bindNamesToList {cpuDir cpuFamObjDir} [wtxCpuInfoGet -n -o \
                                                   $__wtxCpuType]
            regsub $toolType $tool common commonDir
            set spyLibModPath [wtxPath target lib $cpuFamObjDir $cpuDir \
                               $commonDir objos]

            set name ${spyLibModPath}spyLib.o

	    if { [file exists $name] == 0 } {
		noticePost error "File $name is missing" 
		return -1
	    }

	    # load spyLib.o file to get spy symbols

	    wtxObjModuleLoad LOAD_ALL_SYMBOLS $name

        } else {

	    # An other kind of error occured. Just resubmit it.

	    error $status
	    return -1
	}
    }
}

##############################################################################
#
# spyTaskReport - gather information collected by the spy task and display it
#
# This procedure gets the data recorded by the spy task (tSpyTask) form the
# target memory, does the necessary computation, then updates the spy chart.
#
# SYNOPSIS:
#   spyUpdate
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc spyTaskReport {} {
    global spyInterval
    global displayValue
    global spyRunning
    global spyOnScreen

    # This routine is recorded in the auxiliary procedure list, then will be
    # called each time the browser is updated. If spy is not running, we just
    # return immediately.

    if {! $spyRunning} return

    # get active tasks and stock them in a list by Id and by Name

    set aqHead [shSymAddr activeQHead]

    # get ticks value by order :
    # spyTotalTicks,spyIncTicks,spyInterruptTicks,spyInterruptIncTicks,
    # spyKernelTicks,spyKernelIncTicks,spyIdleTicks,spyIdleIncTicks 

    set tot     [lindex [wtxSymFind -name spyTotalTicks] 1]
    set inc     [lindex [wtxSymFind -name spyIncTicks] 1]
    set int     [lindex [wtxSymFind -name spyInterruptTicks] 1]
    set intInc  [lindex [wtxSymFind -name spyInterruptIncTicks] 1]
    set ker     [lindex [wtxSymFind -name spyKernelTicks] 1]
    set kerInc  [lindex [wtxSymFind -name spyKernelIncTicks] 1]
    set idle    [lindex [wtxSymFind -name spyIdleTicks] 1]
    set idleInc [lindex [wtxSymFind -name spyIdleIncTicks] 1]

    global offset

    #
    # Get every target values in 1 shot to get coherent values.
    # As we are using the gopher preemption lock, there is still a
    # chance that an interrupt occurs during gopher evaluation so that
    # the interrupt tick count becomes wrong. 
    # More getting everything in one shot might limit the number of tasks that
    # spy can manage (however it seems enough : 400 tasks with 20 char long
    # names)
    #

    set targetList [wtxGopherEval "$tot @ $inc @ $int @ $intInc @ $ker @
				   $kerInc @ $idle @ $idleInc @
				   $aqHead *
				   {<-$offset(WIND_TCB,activeNode) !
				     <+$offset(WIND_TCB,name) *$>
				     <+$offset(WIND_TCB,taskTicks) @@>> *}"]
 
    set listTicks [lrange $targetList 0 7]
    set targetList [lrange $targetList 8 end]
    set taskIdList {}
    set taskNameList {}
    for {set ix 0} {$ix < [llength $targetList]} {incr ix} {
    	if {$ix % 4 == 0} {
	    lappend taskIdList [lindex $targetList $ix]
	} elseif {$ix % 4 == 1} {
	    lappend taskNameList [lindex $targetList $ix]
	} elseif {$ix % 4 >= 2} {
	    lappend listTicks [lindex $targetList $ix]
	}
    }

    set totalTicks {}
    set deltaTicks {}

    # create one list with total ticks and one other with delta ticks

    for {set i 0} {$i < [llength $listTicks]} {incr i} {
        if {[expr $i%2] == 0} {
	    lappend totalTicks [lindex $listTicks $i]
	} else {
	    lappend deltaTicks [lindex $listTicks $i]
	}
    }

    # format a list with task name for chartSetQuantities routine

    set taskNameList2 {} 
    foreach i $taskNameList {
	lappend taskNameList2 [list $i 0 100]
    }

    # add KERNEL INTERRUPT IDLE TOTAL in taskNameList 

    lappend taskNameList2 {INTERRUPT 0 100} {KERNEL 0 100} {IDLE 0 100} \
    			  {TOTAL 0 100} 

    set spyTotTicks [lindex $totalTicks 0]
    set totalTicks  [lrange $totalTicks 1 end]

    set tmp 0
    foreach i $totalTicks {
       set tmp [expr $tmp+$i]
    }

    if {$spyTotTicks == 0} {
	set k 0
	foreach i $totalTicks {
	  set totalTicks [lreplace $totalTicks $k $k 0] 
	  incr k
	}
    } else {

	set k 0
	foreach i $totalTicks {
	  set totalTicks [lreplace $totalTicks $k $k [expr ($i*100)/$spyTotTicks]] 
	  incr k
	}
	set spyTotTicks [expr ($tmp*100)/$spyTotTicks]
    }

    set spyIncTicks [lindex $deltaTicks 0]
    set deltaTicks  [lrange $deltaTicks 1 end]

    set tmp 0
    foreach i $deltaTicks {
       set tmp [expr $tmp+$i]
    }

    if {$spyIncTicks == 0} {
	set k 0
	foreach i $deltaTicks {
	  set deltaTicks [lreplace $deltaTicks $k $k 0] 
	  incr k
	}
    } else {
	set k 0
	foreach i $deltaTicks {
	  set deltaTicks [lreplace $deltaTicks $k $k [expr ($i*100)/$spyIncTicks]] 
	  incr k
	}
	set spyIncTicks  [expr ($tmp*100)/$spyIncTicks]
    }

    # create a list of percentage

    set taskTotalTicks [lrange $totalTicks 3 end]
    set sysTotalTicks  [lrange $totalTicks 0 2]
    set TotalTicks     [concat $taskTotalTicks $sysTotalTicks $spyTotTicks]

    set taskDeltaTicks [lrange $deltaTicks 3 end]
    set sysDeltaTicks  [lrange $deltaTicks 0 2]
    set DeltaTicks     [concat $taskDeltaTicks $sysDeltaTicks $spyIncTicks]

    # display name and body in spyChart

    chartSetQuantities spyChart $taskNameList2

    # set value in spyChart

    if { $displayValue == 1 } {
        chartSetValues spyChart $TotalTicks
    } else {
        chartSetValues spyChart $DeltaTicks
    }

    # post the spy window

    if {! $spyOnScreen} {
	set spyOnScreen 1
	chartPost spyChart
    }
}

##############################################################################
#
# spyTaskDisplaySet - set the spy displayValue
#
# This routine sets the spy chart title.
#
# if displayValue = 1 chart display total value (cumulative mode)
# if displayValue = 0 chart display delta value (differential mode)
#
# SYNOPSIS:
#   spyTaskDisplaySet
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc spyTaskDisplaySet {} {
    global displayValue

    if {$displayValue == 1} {
	chartTitleSet spyChart "[lindex [wtxHandle] 0]: spy (total time)"
    } else {
	chartTitleSet spyChart "[lindex [wtxHandle] 0]: spy (delta time)"
    }
}

##############################################################################
#
# spyChartDestroyed - Remove the spy chart
#
# This procedure stops the spy task and undisplays the spy chart. It is called
# when the window manager requests the window to be closed.
#
# SYNOPSIS:
# spyChartDestroyed name
#
# PARAMETERS:
# name - unused
#
# RETURNS: N/A
# 
# ERRORS: N/A
#

proc spyChartDestroyed {name} {

    spyStop
    chartUnpost spyChart
}

##############################################################################
#
# spyStop - stop spy polling
#
# This is attached as a callback to the spy dialog
#
#
# SYNOPSIS:
#   spyStop
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc spyStop {} {
    global spyRunning
    global spyOnScreen

    # find symbol spyStopCommon

    set sym spyStopCommon
    set tAddrs [lindex [wtxSymFind -name "$sym"] 1]

    # stop spy task

    wtxFuncCall $tAddrs
    set spyRunning 0
    set spyOnScreen 0
}

##############################################################################
#
# spyStart - start spy polling
#
# This is attached as a callback to the spy dialog
#
# SYNOPSIS:
#   spyStart
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS:
# Can't start spy in system mode: the spy utility can be run only with the
#				  target agent in task mode.
#

proc spyStart {} {
    global spyInterval
    global spyRunning
    global spyFrequency
    global browserUpdate

    # load spyLib.o if necessary

    if {[spyTaskSetup] == -1} {
	return
    }

    # create spy task

    set tAddrs [lindex [wtxSymFind -name spyCommon] 1]

    # spawn spy task
    
    set intvl [expr $spyInterval / 1000]
    if [catch {wtxFuncCall $tAddrs $intvl $spyFrequency} status] {
	if {[wtxErrorName $status] == "AGENT_AGENT_MODE_ERROR"} {
	    error "Can't start spy in system mode" 
	} else {
	    error $status
	}
    }

    set spyRunning 1

    # Turn on the update mechanism. Note that we don't want to interfere with
    # the browser's auto update which already turned on the update mechanism.

    if {! $browserUpdate} {
	delayedEval $spyInterval {update 1}
    }
}

##############################################################################
#
# spyToggleProc - run/stop the spy utility
#
# This procedure starts, or stops, the spy utility when the spy button is
# pressed.
#
# SYNOPSIS:
# spyToggleProc
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc spyToggleProc {} {
    global spyRunning

    if {$spyRunning} {

	# Spy is running: stop it.

        spyStop

    } else {

	# set chart's title. This must be done after the Target Server is 
	# connected, so we do it here.

	spyTaskDisplaySet

	# Spy is not running: start it

        spyStart
    }
}

##############################################################################
#
# 01Spy.tcl - initialization
#
# This part of the file does all the necessary initialization and sourcing to
# get a runable spy utility.
#

spyInit

