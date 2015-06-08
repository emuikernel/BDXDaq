#
# database.tcl
#
# WindView 2.0 database support - reads event description files (.e files)
# and extracts event format information for use by WV parser engine.
#
# Copyright 1995 - 2001 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 02u,18oct01,tcr  add VxWorks events
# 02t,18sep01,cpd  Initial T2.2 Mod. Merge from T3 branch
#                  Add handling of -trigger and -helpid event fields
#             cpd  Add x-ref table for storage of kernel/OS revisions and 
#                  parser version information.
#             tcr  wvLoadUserEventFormats() should search for userEvents.tcl
#                  in $WIND_BASE/... before $HOME/...
#             cpd  Add stipple state for RUNNING+LOCKED+PENDED 
#             cpd  Add function to update the event search list.
#                  Add class enabled variables. Add offByDefault for net
#                  events.
#             spm  Fix SPR#23155: add classes for WindView networking events;
#                  optional -icon flag adds entry to bitmap table
#             cpd  Fix SPR#27330, #26934: Add TCL funtion to export to a file.
# 01s,18sep01,cpd  remove ctrl^ms
# 01r,21apr99,dbs  source user-event formating file
# 01q,22mar99,cjtc changed wvAnalysisPacksGet to fetch appropriate names of
#                  a shared resource file depending on release or debug
#                  run-time. e.g. *-wvap.dll or *-wvap-d.dll
#                  SPR #25688, SPR #25708
# 01p,08may98,c_s  port to UNIX (part 1)
# 01o,05may98,dbs  fix tick-event names
# 01n,01may98,dbs  fix states of non-running interrupts
# 01m,22apr98,dbs  re-implement analysis-packs
# 01l,17apr98,dbs  add RUNNING+PEND pseudo-state - this a slight
#                  kludge and handles the case where e.g. an
#                  EVENT_SEMTAKE appears some time before the
#                  corresponding DISPATCH. 
# 01k,15apr98,dbs  add searchListUpdate proc
# 01j,14apr98,nps  remove debug msgs.
# 01i,13apr98,nps  ensure slashes are consistent in WIND_BASE.
# 01h,30mar98,dbs  add wvFindUserDefinedIcons to this file
# 01g,18mar98,dbs  fix multiple appearances of events in search-list
# 01f,10feb98,dbs  add soft-state mapping
# 01e,20jan98,dbs  fix event names in output lists
# 01d,20jan98,dbs  Rename wvEventList to wvEventSearchList
# 01c,17dec97,c_s  add sourcing of analysis files.
# 01b,27nov97,c_s  add class names and color definitions
# 01a,12aug97,dbs  written.

# ensure slashes are consistent in WIND_BASE
regsub -all {\\} $env(WIND_BASE) / env(WIND_BASE)

global dataBaseDir

set dataBaseDir \
    "$env(WIND_BASE)/host/resource/tcl/app-config/WindView"

global prevName
set prevName ""

global wvAllEventsList
set wvAllEventsList ""

global wvEventSearchList
set wvEventSearchList "user"

global wvRangeBase
set wvRangeBase 0

global wvClass 
 
# Name of the parser library to load, without the extension

global wvParserBaseName
set wvParserBaseName ""

# Name of the directory containing os-specific event description files

global wvDatabaseName
set wvDatabaseName ""

# Version string associated with selected log revision
 
global wvLogRevisionString
set wvLogRevisionString ""

#########################################################################
## These values *MUST* match those declared in target/h/private/eventP.h
## otherwise a match for a valid log may not be found and WindView will
## be unable to load the selected log

set wvRevId(T2) 0xb0b0
set wvRevId(T3) 0xcdcd

set wvEvtProtoRev(REV_1_0_FCS) 1
set wvEvtProtoRev(REV_2_0_FCS) 2
set wvEvtProtoRev(REV_3_0_FCS) 3
set wvEvtProtoRev(REV_3_1_FCS) 4
set wvEvtProtoRev(REV_2_2_FCS) 5
#########################################################################

# This array maps log revison and version to parser basename, the name 
# of the directory containing the .e files, and the string to be
# displayed with the log properties.

set wvVersionsTable($wvRevId(T2),$wvEvtProtoRev(REV_1_0_FCS)) \
{"vxworks" "vxworks" "VxWorks 5.x"}

set wvVersionsTable($wvRevId(T2),$wvEvtProtoRev(REV_2_0_FCS)) \
{"vxworks" "vxworks" "VxWorks 5.x"}

set wvVersionsTable($wvRevId(T3),$wvEvtProtoRev(REV_3_0_FCS)) \
{"vxworksAE1_0" "VxWorksAE1.0" "VxWorks AE 1.0"}

set wvVersionsTable($wvRevId(T3),$wvEvtProtoRev(REV_3_1_FCS)) \
{"vxworksAE1_0" "VxWorksAE1.0" "VxWorks AE 1.1"}

set wvVersionsTable($wvRevId(T2),$wvEvtProtoRev(REV_2_2_FCS)) \
{"vxworks" "vxworks" "VxWorks 5.5"}



#########################################################################
#
# wvDatabaseParserSelect - decode log revision number to strings
#
# The revision word from the WindView log is passed into this routine
# and is used to determine which parser to load, and what version
# string to indicate for this log.
#
#########################################################################
proc wvDatabaseParserSelect {wvRevId wvEvtProtoRev} {
    global wvParserBaseName wvDatabaseName wvLogRevisionString
    global wvVersionsTable

    set wvConfigInfo $wvVersionsTable($wvRevId,$wvEvtProtoRev)
        
    set wvParserBaseName [lindex $wvConfigInfo 0]
    set wvDatabaseName [lindex $wvConfigInfo 1]
    set wvLogRevisionString [lindex $wvConfigInfo 2]
}
 
#########################################################################
#
# eventFileRead - read a WindView 2.0 kernel-specific directory.
#
# USAGE: eventFileRead fileName
#
# All .tcl files in this directory ($kernelName) must be executable 
# without reference to code in this file (database.tcl). The event
# descriptions rely on the definition of wvEvent and so are kept in .e 
# files, which are read in *after* the .tcl files. Thus, the .tcl files
# are generally re-usable by other aspects of the project.
#
#########################################################################
proc eventFileRead {kernelName} {
    global dataBaseDir
    global wvAllEventsList
    global wvEventSearchList

    set wvAllEventsList ""
    set wvEventSearchList ""

    set files "$dataBaseDir/$kernelName/*.e"
    foreach f [glob $files] {
	   source $f
    }
}

#########################################################################
#
# trigEventFileRead - read a WindView kernel-specific directory.
#		      Include only the events that have 'trigger=true'
#                     as a parameter in the event list.
#
# USAGE: wvEventFileRead
#
# This function has the same requirements as eventFileRead.
#
#########################################################################
proc trigEventFileRead {kernel} {
    global wvClass
    global wvAllEventsList
    global wvEventSearchList
    global wvEventInfo

    eventFileRead $kernel

    set wvEventSearchList ""

    foreach ev $wvAllEventsList { 
        set evID $wvEventInfo(id,$ev)
    set evTrigger ""
        catch [set evTrigger $wvEventInfo($evID,trigger)] 
 
        if { $evTrigger == "true" } { 
            lappend wvEventSearchList $ev 
        } 
    } 
}

#########################################################################
#
# wvEventFileRead - read a WindView kernel-specific directory.
#
# USAGE: wvEventFileRead
#
# This function has the same requirements as eventFileRead but the user
# is not required to specifiy a directory in which the events are kept.
# The directory is taken from the global wvDatabaseName and this is 
# passed to eventFileRead The function sets the database name varable and then
# calls the proper eventFileRead function.
#
#########################################################################
proc wvEventFileRead {} {
    global wvDatabaseName
    eventFileRead $wvDatabaseName
}

#########################################################################
#
# wvEvent - creates an event-description entry in the wvEventInfo array.
#
# USAGE: wvEvent eventName eventId eventFlags eventParams
#
# Used by the .e files (which are just straight Tcl code but won't work 
# without this definition) to instantiate the event-descriptions when 
# an event-base is loaded.
#
# eventId is the numerical ID matching that in eventP.h
# eventFlags is list of optional -notimestamp / -nosearch / etc,
# eventParams is type/name pairs of event params
#
#########################################################################
proc wvEvent {eventName eventId eventFlags eventParams} {
    global wvEventInfo
    global wvAllEventsList
    global wvEventSearchList
    global prevName
    global wvRangeBase
    set timestamp 1
    set search 1

    # If eventId is like 100-200 then its a range, so recursively 
    # enter all items in the range into the array. If not then 
    # enter a single event.
    if {[regexp {([0-9]*)-([0-9]*)} $eventId x start end]} {
	set wvRangeBase $start
        for {set i $start} {$i < $end} {incr i} {
	    eval {wvEvent $eventName $i $eventFlags $eventParams}
	}
    } else {
	# Assume no niceName for now
	set name $eventName
	# Loop over all flags
	while {[llength $eventFlags] > 0} {
	    # Prepare to scan flags
	    set currFlag [lindex $eventFlags 0]
	    # Look for -notimestamp flag that indicates the
	    # event doesn't carry a 32-bit timestamp field
	    if {$currFlag == "-notimestamp"} {
		set timestamp 0
	    } elseif {$currFlag == "-nosearch"} {
		# The -nosearch flag indicates the event should
		# not show up in the GUI event-search dialog
		set search 0 
	    } elseif [regexp -- -name=(.*) "$currFlag" x matchname] {
		# The -name=xxx flag gives the event a nicer name in
		# the VxWorks format e.g. semTake not EVENT_SEMTAKE
		# NB:- note the use of the global wvRangeBase as the
		# offset for the event ID - this allows ranges of
		# event-IDs like intEnt to work...
		set name [format $matchname [expr $eventId - $wvRangeBase]]
	    } elseif [regexp -- -class=(.*) "$currFlag" x matchcl] {
		# The -class=xxx flag puts the event into a specific
		# class for display purposes
		set wvEventInfo($eventId,class) $matchcl
	    } elseif [regexp -- -icon=(.*) "$currFlag" x iconname] {
		# The -icon=xxx flag specifies the bitmap file to
		# display if the event occurs. It allow multiple events
                # to share the same icon. If this argument is not present,
                # WindView will search for a bitmap file matching the event
                # name.
                set wvEventInfo($eventId,icon) $iconname
	    } elseif [regexp -- -helpid=(.*) "$currFlag" x helpid] {
		# The -helpid=xxx flag specifies the help id of the event
                set wvEventInfo($eventId,helpid) $helpid
	    } elseif [regexp -- -trigger=(.*) "$currFlag" x trigger] {
		# The -trigger=xxx flag specifies whether the event can
		# fire a trigger or not
                set wvEventInfo($eventId,trigger) $trigger
	    }
	    set eventFlags [lreplace $eventFlags 0 0]
	}
	# $eventParams just holds param info
        set wvEventInfo($eventId,name) $name
        # establish a reverse lookup
        set wvEventInfo(id,$name) $eventId
	set wvEventInfo($eventId,cname) $eventName
	set wvEventInfo($eventId,timestamp) $timestamp
	set wvEventInfo($eventId,objArg) -1
        set wvEventInfo($eventId,search) $search
        set wvEventInfo($eventId,opcodes) ""
	# Update list of all event-names
	if {$name != $prevName } {
	    lappend wvAllEventsList $name
	}
	set prevName $name
	# Update list of searchable event-names
        if {$search} {
            lappend wvEventSearchList $name
        }

	# now we expect type/name argument pairs, 
	# possibly with a trailing * to indicate 
	# the distinguished "object id" parameter.

	set argIx 0
	
	while {[llength $eventParams] > 0} {
	    # Extract type/name pair
	    set argType [lindex $eventParams 0]
	    set argName [lindex $eventParams 1]
	    set eventParams [lreplace $eventParams 0 1]
	    # Store type and name
	    set wvEventInfo($eventId,[format "arg%dtype" $argIx]) $argType
	    set wvEventInfo($eventId,[format "arg%dname" $argIx]) $argName
	    # If next element is "*" then it is object-arg
	    if {[lindex $eventParams 0] == "*"} {
		set wvEventInfo($eventId,objArg) $argIx
		set eventParams [lreplace $eventParams 0 0]
	    }
	    # Must be next arg
	    incr argIx
	}
	# Count number of eventParams
	set wvEventInfo($eventId,nArgs) $argIx
    }
}


#########################################################################
# logFileNameExpand - given the base name of a WV event-log file, works
# out all the possible MP (multi-processor) filenames that can be
# associated with the base name. This involves extra suffixes .1 .2
# etc that indicate the extra processors - by default, processor 0 is
# in the unsuffixed file.
#########################################################################

proc logFileNameExpand { baseName } {
    if { [catch {set mpNames [glob "$baseName.*"]} ] } {set mpNames $baseName}
    return $mpNames
}

#
# colors for various icons when the background is black
#

set wvClass(task,bColor)	152:245:245		;# task icons
set wvClass(sem,bColor)		250:128:114		;# semaphore icons
set wvClass(wd,bColor)		255:165:0		;# watchdog icons
set wvClass(tick,bColor)	255:165:0		;# tick icons
set wvClass(msgq,bColor)	127:255:212		;# message queue icons
set wvClass(sig,bColor)		255:0:0			;# signal icons
set wvClass(int,bColor)		204:204:204		;# interrupt icons
set wvClass(ud,bColor)		218:112:214		;# user defined icons

set wvClass(mem,bColor)		218:112:214		;# memory icons
##set wvClass(io,bColor)	218:112:214		;# io icons
##set wvClass(obj,bColor)	218:112:214		;# obj icons
set wvClass(auxnet,bColor)      218:112:214             ;# auxiliary net icons
set wvClass(corenet,bColor)     218:112:214             ;# core net icons
set wvClass(event,bColor)       255:196:128             ;# VxWorks event icons

#
# colors for various icons when the background is white
#

set wvClass(task,wColor)	0:0:255			;# task icons
set wvClass(sem,wColor)		250:128:114		;# semaphore icons
set wvClass(wd,wColor)		255:165:0		;# watchdog icons
set wvClass(tick,wColor)	255:165:0		;# tick icons
set wvClass(msgq,wColor)	64:127:106		;# message queue icons
set wvClass(sig,wColor)		255:0:0			;# signal icons
set wvClass(int,wColor)		127:127:127		;# interrupt icons
set wvClass(ud,wColor)		200:94:196		;# user defined icons

set wvClass(mem,wColor)		200:94:196		;# memory icons
##set wvClass(io,wColor)	200:94:196		;# io icons
##set wvClass(obj,wColor)	200:94:196		;# obj icons
set wvClass(auxnet,wColor)      200:94:196              ;# auxiliary net icons
set wvClass(corenet,wColor)     200:94:196              ;# core net icons
set wvClass(event,wColor)       255:196:128             ;# VxWorks event icons

#
# descriptive strings for event classes
#

set wvClass(task,name)		"Task Events"
set wvClass(sem,name)		"Semaphore Events"
set wvClass(wd,name)		"Watchdog Events"
set wvClass(tick,name)		"Tick Events"
set wvClass(msgq,name)		"Message Queue Events"
set wvClass(sig,name)		"Signal Events"
set wvClass(int,name)		"Interrupt Events"
set wvClass(ud,name)		"User-Defined Events"

set wvClass(mem,name)		"Memory Events"
set wvClass(auxnet,name)	"Aux Net Events"
set wvClass(corenet,name)	"Core Net Events"
##set wvClass(io,name)		"I/O Events"
##set wvClass(obj,name)		"Object Events"
set wvClass(event,name)        "VxWorks Events"

set wvClass(int,offByDefault)	0
set wvClass(tick,offByDefault)	1
set wvClass(auxnet,offByDefault)    1 
set wvClass(corenet,offByDefault)   1 

set wvClass(task,enabled)       1 
set wvClass(sem,enabled)        1 
set wvClass(wd,enabled)         1 
set wvClass(tick,enabled)       0 
set wvClass(msgq,enabled)       1 
set wvClass(sig,enabled)        1 
set wvClass(int,enabled)        1 
set wvClass(ud,enabled)         1 
 
set wvClass(mem,enabled)        1 
set wvClass(auxnet,enabled)     0 
set wvClass(corenet,enabled)    0 
##set wvClass(io,enabled)         1 
##set wvClass(obj,enabled)        1 
set wvClass(event,enabled)    1

#
# colours for state-stipples - each array-entry is a 3-element list 
# containing (1) a triple of R:G:B colour indication for a black
# background, (2) a triple of R:G:B colour indication for a white
# background, , and (3) the name of a bitmap-file that corresponds 
# to the image (8x8) that gets rendered in that colour, for that state.
#
# By indexing the array wvStippleInfo with the state-bits for
# any given state combination, the stipple-info can be retrieved.
# Any combination not present implies that state is not rendered.
#

global wvStippleInfo

#
# wvStippleSet - establishes the mapping between state-bits and
# the stipple-drawing info described above.
#

proc wvStippleSet { stateBits stippleInfo } {
    global wvStippleInfo
    
    set stateBits [string tolower $stateBits]
    set wvStippleInfo($stateBits) $stippleInfo
}

#
# Stipple-setting commands - note that the array-index MUST be
# a 2-digit hex number, with leading 0x and exactly 2 digits.
# When digits are alpha characters, they should be in lower case.
#

wvStippleSet 0x00 { 0:128:0   0:128:0   smm_read.bmp } ;#ready

# DEAD state is explicitly NOT drawn

wvStippleSet 0x01 { 128:0:0   128:0:0   smm_susp.bmp } ;#suspended
wvStippleSet 0x02 { 255:0:255 255:0:255 smm_pend.bmp } ;#pend
wvStippleSet 0x03 { 128:0:0   128:0:0   smm_susp.bmp } ;#suspended+pend
wvStippleSet 0x04 { 0:255:255 0:255:255 smm_dela.bmp } ;#delayed
wvStippleSet 0x05 { 128:0:0   128:0:0   smm_susp.bmp } ;#susp+delay
wvStippleSet 0x06 { 255:0:255 255:0:255 smm_pend.bmp } ;#delay+pend
wvStippleSet 0x07 { 128:0:0   128:0:0   smm_susp.bmp } ;#susp+pend+delay


##wvStippleSet 0x80 { 128:0:0   128:0:0   smm_brak.bmp } ;#break
##wvStippleSet 0x82 { 128:0:0   128:0:0   smm_brak.bmp } ;#break+pend
##wvStippleSet 0x84 { 128:0:0   128:0:0   smm_brak.bmp } ;#break+delay
##wvStippleSet 0x86 { 128:0:0   128:0:0   smm_brak.bmp } ;#break+delay+pend
##wvStippleSet 0x81 { 128:0:0   128:0:0   smm_susp.bmp } ;#susp+break
##wvStippleSet 0x85 { 128:0:0   128:0:0   smm_susp.bmp } ;#susp+break+delay
##wvStippleSet 0x83 { 128:0:0   128:0:0   smm_susp.bmp } ;#susp+break+pend
##wvStippleSet 0x87 { 128:0:0   128:0:0   smm_susp.bmp } ;#susp+break+pend+delay
	    
wvStippleSet 0x40 { 0:255:0   0:255:0   smm_exec.bmp } ;#running
wvStippleSet 0x50 { 255:0:0   255:0:0   smm_lock.bmp } ;#run+locked
wvStippleSet 0x60 { 255:255:0 255:255:0 smm_inhe.bmp } ;#run+inherited
wvStippleSet 0x70 { 255:255:0 255:255:0 smm_lock.bmp } ;#run+lock+inh

## Although not strictly speaking possible, these states should not appear
## but do (especially with the PPC tick-announce without int-ent)
wvStippleSet 0x42 { 0:255:0   0:255:0   smm_exec.bmp } ;#running+pend
wvStippleSet 0x44 { 255:0:0   255:0:0   smm_exec.bmp } ;#run+delay
wvStippleSet 0x52 { 255:0:0   255:0:0   smm_lock.bmp } ;#run+locked+pend

proc wvState {state isIntr} {
    if {$isIntr != 0} {
	if {$state & 0x40} {return RUNNING} {return INACTIVE}
    }
    if {$state == 0x0} {return READY}
    set stList ""
    if {$state & 0x40} {lappend stList RUNNING}
    if {$state & 0x10} {lappend stList LOCKED}
    if {$state & 0x20} {lappend stList INHERITED}
    if {$state & 0x01} {lappend stList SUSPEND}
    if {$state & 0x02} {lappend stList PEND}
    if {$state & 0x04} {lappend stList DELAY}
    if {$state & 0x08} {lappend stList DEAD}
##    if {$state & 0x80} {lappend stList BREAK}
    if {$state & 0xff00} {lappend stList [format "0x%x" $state]}

    set first 1
    set stDescrip ""
    foreach stName $stList {
        if {$first != 1} {append stDescrip +}
        append stDescrip $stName
        set first 0 
    }
        
    return $stDescrip
}

proc wvStateMapFormat {statemap} {
}

#
# Proc to locate all the analysis-packs. All files are named 
# with the suffix -wvap.dll (e.g. memlib-wvap.dll) meaning
# WindView Analysis Pack.
# Note: for debug builds (on win32-x86 only) , the analysis pack
# dll name is of the form *-wvap-d.dll
#
proc wvAnalysisPacksGet {} {
    global env
    global tcl_platform

    set files ""
    set ext [info sharedlibextension]
    if {$tcl_platform(platform) == "windows"} {
    	if {[uitclinfo debugmode] == 1 } {
            set apackPath $env(WIND_BASE)/host/x86-win32/bin/*-wvap-d$ext
    	} else {
            set apackPath $env(WIND_BASE)/host/x86-win32/bin/*-wvap$ext
    	}
    } else {
    	set apackPath $env(WIND_BASE)/host/$env(WIND_HOST_TYPE)/lib/*-wvap$ext
    }
    foreach f [glob -nocomplain $apackPath] {
	lappend files $f
    }
    return $files
}

#####################################################################
#
# userEventFormat - extract the structured data from a user-event.
#
# Uses a formatting list, where each item is a letter-number pair,
# and the letter indicates the basic type (n=number, s=string) and
# the number indicates the size (in bytes) of the field.
#
# For example, the C struct { int xyz; char sss[20]; } could be
# extracted into a 2-element Tcl list using the formatting-list
# { n4 s20 } as the 2nd arg to this proc.
#
# A further example:-
#
# set x [userEventFormat "12345678313233414243" {n4 s6}]
#
# would set variable x to a 2-element list { 0x12345678 123ABC }
#

proc userEventFormat { rawData fmt } {
    set result {}

    set dataIndex 0
    set fmtIndex 0

    foreach f $fmt {
	set type [string index $f 0]
	set nbytes [string range $f 1 end]
	switch $type {
	    "n" {
		set x [string range $rawData \
			   $dataIndex \
			   [expr $dataIndex + [expr $nbytes * 2 - 1]]] 
		lappend result 0x$x
	    }
	    "s" {
		set x "" 
		for {set n 0} {$n < $nbytes} {incr n} {
		    set nn [expr $n * 2]
		    set pair [string range $rawData \
				  [expr $dataIndex + $nn] \
				  [expr $dataIndex + $nn + 1]]
		    if {$pair == "00"} {
			# NULL terminator - break loop
			break
		    }
		    set pair "0x$pair"
		    set ch [format "%c" $pair]
		    append x $ch
		}
		lappend result $x
	    }
	    default {
		error "unknown type-char $type"
	    }
	}
	incr dataIndex [expr $nbytes * 2]
    }
    return $result
}

proc wvFindUserDefinedIcons {} {
    global env

    # search least-specific to most-specific directories.

    set result ""
    set baseDirs ""
    set iconDir ".wind/WindView"

    # NB:- hard-coded user-event IDs
    set maxNUser [expr 65535 - 40000 + 1]
    
    if {[info exists env(WIND_BASE)] && 
        [file readable $env(WIND_BASE)/.wind/WindView]} {
        lappend baseDirs $env(WIND_BASE)/$iconDir
    }

    if {[info exists env(HOMEDRIVE)] &&
        [info exists env(HOMEPATH)] &&
        [file readable $env(HOMEDRIVE)/$env(HOMEPATH)/.wind/WindView]} {
        lappend baseDirs $env(HOMEDRIVE)/$env(HOMEPATH)/$iconDir
    }

    if {[info exists env(HOME)] &&
        [file readable $env(HOME)/.wind/WindView]} {
        lappend baseDirs $env(HOME)/$iconDir
    }

    foreach baseDir $baseDirs {
        regsub -all {\\} $baseDir / baseDir
        set flist [glob -nocomplain $baseDir/*]

        foreach bmp $flist {
            # extract the bitmap number and make sure it 
            # is in range.  If all goes well store it in 
            # the array.
            
            if [regexp -nocase {.*user([0-9]+)\.bmp$} $bmp all idx] {
                scan $idx "%d" n

                if {$n <= $maxNUser && $n >= 0} {
                    # it's a hit!
                    lappend result [list $n $bmp]
                }
            }
        }
    }

    return $result
}


#
# searchListUpdate - adds intEnt events to the wvEventSearchList
# for all interrupt-contexts in current event base
#
proc searchListUpdate {eb} {
    global wvEventSearchList
 
##    puts "searchlistUpdate" 
 
    if {[catch {
	foreach ctx [$eb names] {
	    set intLevel [$eb.$ctx taskId]
	    if {$intLevel < 512 && $intLevel >= 0} {
		# must be an interrupt
		scan $intLevel %x i
		lappend wvEventSearchList "intEnt-$i"
	    }
	}
	set wvEventSearchList [lsort $wvEventSearchList]
    } errMsg]} {
	puts "$errMsg -- $wvEventSearchList"
    }
}

#
# proc wvExportExcel - exports the data from the array 
#   wvExportData (where each array-name is a cell-id and the
#   contents of the array-location are the data) to MS Excel
#   using COMTCL as the OLE Automation link to Excel...
#

proc wvExportExcel {dbg} {
    global wvExportData

    puts "Exporting wvExportData to Excel"

    if {$dbg} {load comtcl-d} {load comtcl}

    puts "Starting Excel..."

    # create Excel, make it visible
    set xl [cocreate excel.application]
    $xl set visible true

    # get collection of workbooks
    set books [$xl get Workbooks]
    # add a new workbook
    set book [$books Add]

    # find the active sheet
    set sheet [$xl get ActiveSheet]

    # make a 'Range' object
    set range [$sheet get Range a1 d10]

    # output data values
    foreach cell [array names wvExportData] {

	set data $wvExportData($cell)

##	puts "$cell = $data"

	# write the Excel cell
	set range [$sheet get Range $cell]
	$range set Value $data
    }

    # release all our COM handles
    codestroyall
}

proc wvExportFile {dbg} {
    global wvExportData

    puts "Exporting wvExportData to file"

    set fname [fileDialogCreate -savefile -overwriteprompt -pathmustexist -filefilters "Analysis pack Files (*.csv)|*.csv||" -title "Save Analysis Pack Data"]

    beginWaitCursor

    set fname [string trim $fname "{}"]

    # if no name returned, user selected cancel, so do nothing more
    if { $fname == ""} {
        return
    }

    if { [string match *.csv $fname] == 0} {
        set fname [format "%s.csv" $fname]
    }

    set fd [open $fname w]

    foreach cell [array names wvExportData] {

	set data $wvExportData($cell)

	if { [string match a* $cell] != 0} {
	    set postn [format "%05d" [string trim $cell "a"] ]
	    lappend wvExportDataLista "$postn,$data"
	}

	if { [string match b* $cell] != 0} {
	    set postn [format "%05d" [string trim $cell "b"] ]
	    lappend wvExportDataListb "$postn:$data"
	}


    }

    set wvSortedLista [lsort $wvExportDataLista]
    set wvSortedListb [lsort $wvExportDataListb]

    for {set loop 0} {$loop < [llength $wvSortedLista]} {incr loop +1} {

	set dataA [lindex $wvSortedLista $loop]
	set dataB [lindex $wvSortedListb $loop]

	set p1 [string range $dataA 6 end ]
	set p2 [string range $dataB 6 end ]

	puts $fd "$p1,$p2"
    }  

    close $fd

    endWaitCursor

}

proc wvLoadUserEventFormats {} {
    global env

    set dirList {}

    if {[info exists env(WIND_BASE)] &&
	[file readable $env(WIND_BASE)/.wind/WindView]} {
	lappend dirList "$env(WIND_BASE)/.wind/WindView"
    }
    
    if {[info exists env(HOMEDRIVE)] &&
        [info exists env(HOMEPATH)] &&
        [file readable $env(HOMEDRIVE)/$env(HOMEPATH)/.wind/WindView]} {
        lappend dirList $env(HOMEDRIVE)/$env(HOMEPATH)/.wind/WindView
    }

    if {[info exists env(HOME)] &&
	[file readable $env(HOME)/.wind/WindView]} {
	lappend dirList "$env(HOME)/.wind/WindView"
    }

    foreach d $dirList {
        regsub -all {\\} $d / d
	if {[file readable $d/userEvents.tcl]} {
	    if {[catch {source "$d/userEvents.tcl"} err]} {
		#puts "Error reading $d/userEvents.tcl"
	    }
	}
    }
}

wvLoadUserEventFormats
 
proc wvSearchListUpdate {eb} { 
    global wvAllEventsList 
    global wvEventSearchList 
    global wvEventInfo 
    global wvClass 
 
##    puts "updating search events list" 
 
##    puts $wvAllEventsList 
 
    set newSearchList "" 
 
    foreach ev $wvAllEventsList { 
        set evID $wvEventInfo(id,$ev) 
        set evSearch $wvEventInfo($evID,search) 
        set evClass "    " 
        set evClassEnabled "" 
        catch [set evClass $wvEventInfo($evID,class)] 
        catch [set evClassEnabled $wvClass($evClass,enabled)] 
 
##      puts "$evClass $ev $evID $evClassEnabled $evSearch" 
 
        if { $evClassEnabled == "1" && $evSearch == "1"} { 
            lappend newSearchList $ev 
        } 
    } 
##    puts $newSearchList 
 
    if { $wvClass(ud,enabled) == 1 } { 
        lappend newSearchList "user" 
    } 
 
    set wvEventSearchList $newSearchList

    if { $wvClass(int,enabled) == 1 } { 
        searchListUpdate $eb 
    } 


} 
 
