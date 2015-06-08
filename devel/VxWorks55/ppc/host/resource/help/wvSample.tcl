#
# WRS WindView 2.0 API library - EAR Sample code
# (c) Wind River Systems, Inc 1997
#
# Author: Dave Sellars, WRS Uk Engineering, Swindon, UK
# email: dsellars@wrs.com

# MODIFICATION HISTORY
#
# 01b 15jan97,dbs  Removed hard-coded paths to apilib shared libs.
# 01a 23oct97,dbs  first version

#
# PROCEDURE: wvMinMaxAvg logfile.wvr ?regexp?
#
# This procedure loads the given file 'logfile.wvr' and performs some
# analysis of the contents. The optional 2nd param 'regexp' is a
# regular expression describing context-names, for example {INT.*}' would
# represent all the interrupt contexts. This can be omitted and the
# default will be all contexts. The results will be printed to stdout.
#

# UN-COMMENT ONE OF THE FOLLOWING load STATEMENTS FOR YOUR HOST-TYPE
# Load Solaris 2.x shared-lib Tcl extension
#load libwvapi.so
# Load Win32 DLL Tcl extension
load apilib-d.dll

proc wvMinMaxAvg args {

    set result ""

    #
    # Error check on args
    #
    if {[llength $args] == 0} {
	puts "usage: wvMinMaxAvg logFile ?regexp?"
	puts "where <regexp> is a Tcl regexp for context names"
	puts "for example \{INT.*\} for all interrupt contexts"
	return
    }

    #
    # Make environment vars available in this scope - this is required 
    # because the wvEventBaseLoad procedure needs access to the
    # WIND_BASE environment variable
    #
    upvar #0 env env
    #
    # Load event base, creating command 'eb' and one other command per 
    # context, like 'eb.tNetTask' etc...
    #
    set fileName [lindex $args 0]
    puts "Loading event-base $fileName"
    wvEventBaseLoad $fileName eb
    #
    # Find names of all contexts
    #
    set allCtxts [eb names]
    #
    # If we have 2nd arg, it is regexp of context-names, in Tcl regexp 
    # format, for example to get all interrupt tasks it would be {INT.*}
    #
    if {[llength $args] == 2} {
	set re [lindex $args 1]
	puts "Using regexp $re"
	#
	# Find all contexts that fit given regexp, save their names in 
	# the list called 'ctxts' so we can address them later
	#
	set ctxts {}
	foreach c $allCtxts {
	    set matchCtx ""
	    if {[regexp $re $c matchCtx]} {
		lappend ctxts $matchCtx
	    }
	}
    } else {
	# Just consider every context
	set ctxts $allCtxts
    }
    #
    # DEBUG: display list of context-names
    #
    puts "Using contexts $ctxts"
    #
    # Now, for each context in 'ctxts' find the min, max and average
    # time spent in one of the RUNNING states
    #

    set imax 0
    set imin [lindex [eb interval] 1]

    foreach ctx $ctxts {
	#
	# Initialise stats variables
	#
	set max $imax
	set min $imin
	set numEvents [eval "eb.$ctx events"]
	set wasRunning 0
	set startPeriod 0.0
	set endPeriod 0.0
	set duration 0.0
	set allTimes {}
	set mnmx $ctx
	#
	# Iterate over all events
	#
	for {set i 0} {$i < $numEvents} {incr i} {
	    set eventDescr [eval "eb.$ctx $i"]
	    #
	    # DEBUG: uncomment next command to see event-description
	    #puts $eventDescr
	    #
	    set state [lindex $eventDescr 3]
	    set timeStamp [lindex $eventDescr 1]
	    #
	    # Test 'Wind State Bits' for RUNNING bit - although other
	    # bits may be set, only the 0x40 bit means the task is
	    # actually executing. Then we do some logic to determine
	    # if the task was running at the previous event, and to
	    # detect the state changes between running and non-running
	    # states...
	    #
	    if {$state & 0x40} {
		# Is now RUNNING
		if {$wasRunning} {
		    # Was running at previous event, so extend end of
		    # running period
		    set endPeriod $timeStamp
		} else {
		    # Wasn't running previously so this is start of 
		    # running period
		    set startPeriod $timeStamp
		}
		set wasRunning 1
	    } else {
		# Is now NOT RUNNING
		if {$wasRunning} {
		    # Was running at previous event, now isn't
		    set endPeriod $timeStamp
		    # Calculate duration of running period
		    set duration [expr $endPeriod - $startPeriod]
		    #
		    # DEBUG: uncomment next command to see the
		    # duration of the running period
		    #puts "Ran for $duration secs"
		    #
		    # See if this duration is min or max so far...
		    #
		    if {$duration < $min} { set min $duration }
		    if {$duration > $max} { set max $duration }
		    #
		    # Add this duration to list of all times. Later
		    # this will be used to calculate the average
		    #
		    lappend allTimes $duration
		} else {
		    # Wasn't running at previous event, still isn't
		    set startPeriod $timeStamp
		}
		set wasRunning 0
	    }
	}
	#
	# Print min and max run-times for this context
	#
	if {$min == $imin} {
	    set min 0.0
	}
	if {$max == $imax} {
	    set max 0.0
	}
	puts "$ctx Minimum run time was $min secs."
	lappend mnmx $min
	puts "$ctx Maximum run time was $max secs."
	lappend mnmx $max
	#
	# Calculate and print average run-time
	#
	set average 0.0
	for {set i 0} {$i < [llength $allTimes]} {incr i} {
	    set average [expr $average + [lindex $allTimes $i]]
	}
	if {$average != 0.0} {
	    set average [expr $average / [llength $allTimes]]
	}
	puts "$ctx Average run time was $average secs."
	lappend mnmx $average

	lappend result $mnmx
    }
    #
    # Destroy the event base before we exit
    #
    eb destroy

    return $result
}

#
# PROCEDURE: wvDump logfile.wvr ?outfileHandle?
#
# Dumps the entire event-base to an output file, or stdout if no
# handle is given. Note that the 2nd argument must be a Tcl FILE
# HANDLE, i.e. the result of
#
# set f [open someFile w]
#
# Each event is a Tcl list, in the standard format:- 
#
# eventName timeStamp argList state
#
# where argList is a Tcl list of 'name=value' strings, like:-
#
# { semId=394588 owner="" }
#
# and an empty 'value' string means that no information was captured
# about that event-parameter, because the detail level of the event
# capture was too low.
#

proc wvDump {logFile {fhandle stdout}} {
    #
    # Open the event-base log (see wvMinMaxAvg for more details)
    #
    upvar #0 env env
    wvEventBaseLoad $logFile eb
    #
    # Iterate over all contexts, dumping each event to 'fhandle'
    # In order to separate contexts, the title of each context
    # is printed first.
    #
    foreach ctx [eb names] {
	puts $fhandle "CONTEXT:$ctx"
	for {set i 0} {$i < [eval "eb.$ctx events"]} {incr i} {
	    puts $fhandle [eval "eb.$ctx $i"]
	}
    }
}

	