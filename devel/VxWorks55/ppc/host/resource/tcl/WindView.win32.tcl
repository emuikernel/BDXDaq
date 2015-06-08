# WindView.tcl - WindView Tcl implementation file
#
# Copyright 1994 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01z,10may01,tpw  Add VX_ALTIVEC_TASK to taskOptionsFmt.
# 01y,15mar00,zl   merged SH support from T1
# 01x,12may98,nps  cumulative patch merge.
# 01w,20apr98,dbs  fix eventFormat2 to remove empty-string params
# 01v,08apr98,nps  present numeric event params on status line in hex.
# 01u,13feb98,dbs  fix event tooltips, add eventFormat2
# 01t,31dec97,c_s  WV2.0 development.
# 01s,17jun97,c_s  fix SPR #8772.
# 01s,17jun97,c_s  fix SPR #8772.
# 01r,19oct96,c_s  fix SPR #6871.
# 01q,08may96,c_s  fixes for SPR #6440.
# 01p,29apr96,c_s  fixed swapping for U.D. events
# 01o,05mar96,c_s  inspector formatting improvements/rework.
# 01n,07feb96,c_s  first update for Windows use.  Formats returned by 
#                    eventFormat have changed; other formatters added.
# 01m,06dec94,rdc  fixed spr 3447.
# 01l,08apr94,c_s  added formatters for buffer, and sigset; corrected that
#                    for msgPri.
# 01k,01apr94,c_s  fixed alignment of Address field; added formatter for
#                    msgOption arguments.
# 01j,30mar94,c_s  more cleanup to argument printing procedures.
# 01i,24mar94,c_s  updated event dictionary for new kernel event stream.
# 01h,07mar94,c_s  added more event parameter formatters to improve the look
#                    of events in the inspector.  Aligned them.
#                  moved file.
#                  added comments.
# 01g,31jan94,c_s  changed event dictionary entry for exception event
#                    (SPR #2986).
# 01f,26jan94,c_s  fixed a naming bug in eventDictSchema.
# 01e,14jan94,c_s  objectObjId now returns error if applied to an event
#                    that does not have an associated object ID.
# 01d,06jan94,c_s  in Event Inspector, contexts of dropped events now reveal
#	             more information: the name, VxWorks ID, and unique 
#		     context ID number.  Methods used to reveal event and 
#                    context IDs of dropped objects now signal errors if 
#                    they detect that the dropped object doesn't have the
#                    requested attribute, so this case can be distinguished
#                    from a drop of something that's not an object at all.
# 01c,02jan94,c_s  fixed swapTargetInt (declared int variable).  unpackstring
#                    now discards characters beyond any null it finds up
#                    to the declared length of the string, if that is given.
# 01b,16dec93,c_s  changed user event ID range to 40000-65535, as was done in 
#                    h/private/eventP.h.        
# 01a,23jul93,c_s  written.
#*/

# GLOBAL DATA DECLARATIONS

#
# this error will be thrown if an attribute is requested from an object
# that does not possess the attribute.
#

global objectLacksAttributeMsg "object does not possess that attribute."

#
# minimum and maximum numbers of user-defined events.
#

global minUsrEventNum
set minUsrEventNum 	40000
global maxUsrEventNum
set maxUsrEventNum	65535

#
# inspectorFieldWidth is the number of characters allotted for the 
# names of inspector arguments.  The formatting procedures will attempt
# to align the contents of the inspector based on this number.  If 
# a parameter's name is longer than this width, it will not be truncated;
# instead the alignment will "spill" into the value area of the inspector.
#

global inspectorFieldWidth
set inspectorFieldWidth 10

#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#
# OBJECTS
#
# An "object" is a string that WindView provides via the window system
# selection mechanism.  The string has a particular format, and represents
# something that is selected or dragged from windview.  The objects and
# their syntax is:
#
# an non-userdefined event icon from the view graph:
# 
#   #event <time> <context-id> {<event-id> {<p0> <p1> <p2> <p3> <p4> <p5>}}
#
# user defined events have this format:
#
#   #event <time> <context-id> {<event-id> {0 <ascii-hex-contents>}}
#
# a time instant:
#
#   #time <time>
#
# a time interval:
#
#   #timeinterval <time0> <time1>
#
# a context:
#
#   #context <id> <name>
#
# The following procedures work with these structures.
#
##############################################################################

#*****************************************************************************
#
# objectClass - map object to object class
#
# Given an object, return the string represening the type
# of object (one of #event, #time, #timeinterval, or #context.)
#
# RETURNS: the object class, or "#bad" if the argument is not recognized
# as an object
# 
# ERRORS: none

proc objectClass obj { 
    set cls [lindex $obj 0]
    case $cls { 
        \#event		{return #event}
	\#time		{return #time}
	\#timeinterval	{return #timeinterval}
	\#context	{return #context}
	*		{return #bad}
    }
}

#*****************************************************************************
#
# objectTime - map object to timestamp
#
# Given an object, return any timestamp that may be a part of it, or
# -1.  For events, this is the event timestamp; time instants are
# handled in the obvious way, and time intervals (arbitrarily) return
# the starting time of the interval.
#
# RETURNS: timestamp, or -1 if <obj> is not recognized as an object.
#
# ERRORS: $objectLacksAttributeMsg

proc objectTime obj {
    global objectLacksAttributeMsg
    set objCls [objectClass $obj]

    case $objCls { 
        \#event		{return [lindex $obj 1]}
	\#time		{return [lindex $obj 1]}
	\#timeinterval  {return [lindex $obj 1]}
	\#context       {error $objectLacksAttributeMsg}
	*		{return -1}
    }
}

#*****************************************************************************
#
# objectEvent - map object to event
#
# return the name of the event contained in the object, else error.
#
# RETURNS: event name
#
# ERRORS: $objectLacksAttributeMsg

proc objectEvent obj {
    global objectLacksAttributeMsg
    set objCls [objectClass $obj]

    case $objCls {
	\#event		{return [eventIdToName [lindex [lindex $obj 6] 0]]}
	\#time          {error $objectLacksAttributeMsg}
	\#timeinterval  {error $objectLacksAttributeMsg}
	\#context       {error $objectLacksAttributeMsg}
        *               {return ""}
    }
}

#*****************************************************************************
#
# objectCtxId - map object to unique context id
#
# return the context id contained in the object, else error.
#
# RETURNS: unique context id
#
# ERRORS: $objectLacksAttributeMsg

proc objectCtxId obj {
    global objectLacksAttributeMsg
    set objCls [objectClass $obj]

    case $objCls { 
        \#event		{return [lindex $obj 2]}
	\#time          {error $objectLacksAttributeMsg}
	\#timeinterval  {error $objectLacksAttributeMsg}
	\#context	{return [lindex $obj 1]}
        *		{return ""}
    }
}

#*****************************************************************************
#
# objectObjId - map object to VxWorks object ID acted upon
#
# return the VxWorks object id contained in the object,
# else error.  This is defined for events only, and represents the
# parameter of the event that represents the VxWorks object acted on 
# by the event.  The mapping between events and objects is maintained
# in the Tcl event dictionary.
#
# RETURNS: VxWorks object ID
#
# ERRORS: $objectLacksAttributeMsg

proc objectObjId obj {
    global eventDict
    global objectLacksAttributeMsg
    set objCls [objectClass $obj]

    if {"$objCls" == "#bad"} {
	return ""
    }

    if {"$objCls" != "#event"} {
        error $objectLacksAttributeMsg
    }

    set evtId [eventId $obj]
    set spec $eventDict($evtId)

    set argIx [specIdArgIx $spec]

    if {$argIx == -1} {return ""}

    return [eventArg $obj $argIx]
}


    
#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#
# EVENTS
#

#
# Given an event object, return the id.
#

proc eventId event { 
    return [lindex [lindex $event 6] 0]
}

#
# Given an event object, return argument <argNum>.
#

proc eventArg {event argNum} {
    return [lindex [lindex [lindex $event 6] 1] $argNum]
}

# 
# Given an event dictionary specification, return the argument list.
#

proc specArgs evtSpec { 
    return [lindex $evtSpec 1]
}

# 
# Given an event dictionary specification, return the event name.
#

proc specName evtSpec { 
    return [lindex $evtSpec 0] 
}

#
# Given an event dictionary specification, return the object-id argument index.
#

proc specIdArgIx evtSpec {
    return [lindex $evtSpec 2]
}

##############################################################################
#
# EVENT ARGUMENT FORMATTING PROCEDURES
#
#
# These are called by the event inspector formatting procedures.  If an 
# event containes a parameter called "delay", a procedure named "delayFmt"
# is searched for in the Tcl procedure list.  If one is found, it is called
# with the delay parameter as argument.  The procedure then has the opportunity
# to determine what is shown in the inspector for that event parameter.
#
# If any of these procedures return the empty string, the parameter of the 
# event will be suppressed entirely (that is, the name of the parameter
# won't even appear).
#

#*****************************************************************************
#
# standardArgFormat - utility argument formatter
#
# The event argument <name>, whose value is <val>, is printed with 
# printf format specification <format> (e.g., "%d").  If <val> is 
# numerically equal to -2, the value is printed as "?".
#
# RETURNS: formatted argument
#
# ERRORS: none

proc standardArgFormat {name format val} {
    if {$val == -2} {
	return [list $name "?"]
    } {
	return [list $name [format $format $val]]
    }
}

    
#
# Each of the following procedures has the form <parameter>Fmt, and 
# returns a string that is to be placed in an event inspector for that
# argument.
#

#*****************************************************************************
#
# delayFmt - format delay argument.
#
# The value is reported in decimal.
#
# RETURNS: formatted argument
#
# ERRORS: none

proc delayFmt val { 
    return [standardArgFormat delay %d $val]
}

#*****************************************************************************
#
# semOptionsFmt - format semOptions argument.
#
# The bitfield is examined and a list of the flags is reported.
#
# RETURNS: formatted argument
#
# ERRORS: none

proc semOptionsFmt val {
    set result ""

    case [expr {$val & 0x3}] {
	0		{set result SEM_Q_FIFO}
	1		{set result SEM_Q_PRIORITY}
	*		{set result {Uknown Queue Type}}
    }

    if {$val & 0x4} {set result [concat $result SEM_DELETE_SAFE]}
    if {$val & 0x8} {set reuslt [concat $result SEM_INVERSION_SAFE]}

    return [list "options" $result]
}

#*****************************************************************************
#
# msgOptionsFmt - format msgOptions argument.
#
# The bitfield is examined and a list of the flags is reported.
#
# RETURNS: formatted argument
#
# ERRORS: none

proc msgOptionsFmt val {
    set result ""

    case [expr {$val & 0x1}] {
	0		{set result MSG_Q_FIFO}
	1		{set result MSG_Q_PRIORITY}
    }

    return [list "msgOptions" $result]
}

#*****************************************************************************
#
# taskOptionsFmt - format taskOptions argument.
#
# The bitfield is examined and a list of the set flags is reported.
#
# RETURNS: formatted argument
#
# ERRORS: none

proc taskOptionsFmt val {

    if {$val == -2} {
	return [list "options" "?"]
    }

    set result ""

    if {$val & 0x001} {append result "VX_SUPERVISOR_MODE "}
    if {$val & 0x002} {append result "VX_UNBREAKABLE "}
    if {$val & 0x004} {append result "VX_DEALLOC_STACK "}
    if {$val & 0x008} {append result "VX_FP_TASK "}
    if {$val & 0x010} {append result "VX_STDIO "}
    if {$val & 0x020} {append result "VX_ADA_DEBUG "}
    if {$val & 0x040} {append result "VX_FORTRAN "}
    if {$val & 0x080} {append result "VX_PRIVATE_ENV "}
    if {$val & 0x100} {append result "VX_NO_STACK_FILL "}
    if {$val & 0x200} {append result "VX_DSP_TASK "}
    if {$val & 0x400} {append result "VX_ALTIVEC_TASK "}

    return [list "options" [string trim $result]]
}

#*****************************************************************************
#
# timeoutFmt - format timeout argument.
#
# The value is reported in decimal.  -1 and 0 are mapped to WAIT_FOREVER
# and NO_WAIT, respectively.
#
# RETURNS: formatted argument
#
# ERRORS: none

proc timeoutFmt val {
    case [expr $val] {
    0		{set s "NO_WAIT"}
    -1		{set s "WAIT_FOREVER"}
    -2          {set s "?"}
    *		{set s [expr $val]}}

    return [list "timeout" $s]
}

#*****************************************************************************
#
# recurseFmt - format recurse argument.
#
# The value is returned in decimal.
#
# RETURNS: formatted argument
#
# ERRORS: none

proc recurseFmt val {
    return [standardArgFormat recurse %d $val]
}

#*****************************************************************************
#
# entryFmt - format entry argument.
#
# An entry point of -2 means the information wasn't collected.
#
# RETURNS: formatted argument
#
# ERRORS: none

proc entryFmt val {
    return [standardArgFormat entry %#x $val]
}

#*****************************************************************************
#
# priorityFmt - format priority argument.
#
# A priority of -2 means the information wasn't collected.
#
# RETURNS: formatted argument
#
# ERRORS: none

proc priorityFmt val {
    return [standardArgFormat priority %d $val]
}

#*****************************************************************************
#
# safeCntFmt - format safeCnt argument.
#
# An safeCount of -2 means the information wasn't collected.
#
# RETURNS: formatted argument
#
# ERRORS: none

proc safeCntFmt val {
    return [standardArgFormat safeCnt %d $val]
}

#*****************************************************************************
#
# bufSizeFmt - format bufSize argument.
#
# The value is returned in decimal.  -2 is interpreted as "?."
#
# RETURNS: formatted argument
#
# ERRORS: none

proc bufSizeFmt val {
    return [standardArgFormat bufSize %d $val]
}

#*****************************************************************************
#
# semValueFmt - format semValueFmt argument.
#
# The value is returned in decimal.  -2 is interpreted as "?."
#
# RETURNS: formatted argument
#
# ERRORS: none

proc semValueFmt val {
    return [standardArgFormat semValue %#x $val]
}

#*****************************************************************************
#
# bufferFmt - format buffer argument.
#
# The value is returned in hexadecimal.  -2 is interpreted as "?."
#
# RETURNS: formatted argument
#
# ERRORS: none

proc bufferFmt val {
    return [standardArgFormat buffer %#x $val]
}

#*****************************************************************************
#
# sigsetFmt - format sigset argument.
#
# The value is returned in hexadecimal.  -2 is interpreted as "?."
#
# RETURNS: formatted argument
#
# ERRORS: none

proc sigsetFmt val {
    return [standardArgFormat sigset %#x $val]
}

#*****************************************************************************
#
# msgPriFmt - format msgPri argument.
#
# The value is converted to the symbol MSG_PRI_NORMAL or MSG_PRI_URGENT.
#
# RETURNS: formatted argument
#
# ERRORS: none

proc msgPriFmt val {
    case [expr $val] {
	0		{set valstring "MSG_PRI_NORMAL"}
	1		{set valstring "MSG_PRI_URGENT"}
	-2		{set valstring "?"}
	*		{set valstring $val}
    }

    return [list "msgPri" $valstring]
}

##############################################################################

#*****************************************************************************
#
# evtArgFmt - format an event argument, given name and value
#
# Given an argument name and value, searches the procedure list
# looking for a procedure that has been supplied to provide special
# formatting.  If one is present, it is called, and the string
# returned by that procedure is returned.  A list of two elements (the 
# proper display name of the parameter and the formatted value) are returned.
#
# RETURNS: {arg-name formatted-arg-val}
#
# ERRORS: none

proc evtArgFmt {fmt val} {
    set argFmtProcedure ${fmt}Fmt

    if {[info procs $argFmtProcedure] == ""} {
        return [list $fmt $val]
    } { 
	return [$argFmtProcedure $val]
    }
}

#*****************************************************************************
#
# eventFormat - format an event for the Event Inspector
#
# The main procedure for formatting events in the event inspector.  If
# the event is a user-defined event or eventpoint, usrEventFormat is
# called to do the work.  Otherwise a standard format is used.  For
# each event argument, evtArgFmt is called to produce the inspector
# entry.
#
# RETURNS: text suitable for placing in the Event Inspector
#
# ERRORS: "Can't understand that event!" if a malformed event is supplied.

proc eventFormat event {
    global eventDict
    global inspectorFieldWidth
    global minUsrEventNum
    global maxUsrEventNum
    global wvUsrEventFormat

    set result ""
    set evtId [eventId $event]
    set uniqueCtxId [objectCtxId $event]

    lappend result [format "%.15g" [objectTime $event]]
    set vxid [contextUniqueIdToId $uniqueCtxId]

    if {$vxid == 0xffffffff || ($vxid >= 0 && $vxid <= 512)} {
        lappend result [format "%s" \
                        [contextUniqueIdToName $uniqueCtxId]]
    } else {
        lappend result [format "%s (%#x)" \
                        [contextUniqueIdToName $uniqueCtxId] $vxid]
    }
        
    
    if {$evtId >= $minUsrEventNum && $evtId <= $maxUsrEventNum} {

        set usrEvtId [expr {$evtId-$minUsrEventNum}]
        set adrs [eventArg $event 0]
        if {$adrs == 0} {
            # User event.  See if the user has provided a structure
            # to unpack the parameters (if there are any).

            lappend result "user$usrEvtId"
            lappend result ""
            set evtData [eventArg $event 1]

            if {[string compare $evtData ""] != 0 &&
                [info exists wvUsrEventFormat($usrEvtId)]} {
                set params [usrUnpack $event $wvUsrEventFormat($usrEvtId)]
                lappend result $params
            } {
                if {[string compare $evtData ""] != 0} {
                    set params [format "%*s: %s" -$inspectorFieldWidth \
                        "Data" [eventArg $event 1]]
                    lappend result $params
                }
            }

        } else {
            # Evenpoint.  Include the address.

            lappend result "Eventpoint $usrEvtId"
            lappend result ""

            set param [format "%*s: %#x\n" -$inspectorFieldWidth \
                "Address" $adrs]
            lappend result $param
        }
    } else {
        # Non-user event.  Use the dictionary to retrieve the 
        # parameters.

        if {![info exists eventDict($evtId)]} {
            error "Unknown event id: $evtId"
        }

        set evtSpec $eventDict([eventId $event])

        if {$evtSpec == ""} {
	    error "Can't understand that event!"
        }

        lappend result [specName $evtSpec]
        lappend result [objectObjId $event]
	
        set ix 0
        set params ""

        foreach evtArgType [specArgs $evtSpec] { 
	    set formattedArg [evtArgFmt $evtArgType [eventArg $event $ix]]
	    if {$formattedArg != ""} {
	        append params [format "%*s: %s\n" -$inspectorFieldWidth \
	            [lindex $formattedArg 0] \
	            [lindex $formattedArg 1]]
	    }
            incr ix
        }
        lappend result $params
    }
    return $result
}
	
#*****************************************************************************
#
# eventSynopsis - format a one-line description of event for Status Bars
#
# RETURNS: text suitable for placing in a status bar, or other report
#
# ERRORS: "Can't understand that event!" if a malformed event is supplied.

set wvRawSynopsis 0

proc eventSynopsis event {
    global wvRawSynopsis

    if {$wvRawSynopsis} {return $event}

    set fmt [eventFormat $event]
    set result ""

    if {[llength $fmt] == 5} {
        
        set result [format "\[%s @ %s\] %s (" \
                        [lindex $fmt 1] \
                        [lindex $fmt 0] \
                        [lindex $fmt 2]]

        set needcomma 0
        set alist [split [lindex $fmt 4] "\n"]
        foreach arg $alist {
            if {$arg != ""} {
                if [regexp {.*: (.*)} $arg whole val] {
                    if {$needcomma} {append result ", "}
                    append result $val
                    set needcomma 1
                }
            }
        }
        if {! $needcomma} {
            append result " "
        }
        append result ")"
    }

    return $result
}

	
#*****************************************************************************
#
# usrEventFormat - format a user-defined event for the Event Inspector
#
# Formats a user-defined event for the event inspector.  If the user
# event is not an eventpoint, the Tcl procedure list is searched for a
# procedure named usrEvent<id>Format and if it exists, it is called to
# generate the text for the inspector window.  Otherwise a default
# format is provided here.  If the event is an eventpoint, a default
# format is produced that contains the address where the eventpoint
# was hit.
#
# RETURNS: text suitable for placing in the Event Inspector
# 
# ERRORS: none


proc usrEventFormat event {
    global eventDict
    global inspectorFieldWidth
    global minUsrEventNum
    global maxUsrEventNum
    set result ""
    set evtId [eventId $event]
    set usrEvtId [expr {$evtId-$minUsrEventNum}]
    set adrs [eventArg $event 0]

    set uniqueCtxId [objectCtxId $event]

    lappend result [format "%.15g" [objectTime $event]]
    lappend result [contextUniqueIdToName $uniqueCtxId]
    if {$adrs == 0} {
        lappend result [format "user%d" $usrEvtId]
        lappend result ""
        # XXX deal with user event formatting
        lappend result {}
    } {
        lappend result [format "Eventpoint %d" $usrEvtId]
        lappend result ""
        lappend result [format "%*s: %s\n" -$inspectorFieldWidth
                        "address" $addr]
    }


    return $result
}

#*****************************************************************************
#
# eventInspector - responder for Event Inspector window
#
# The procedure bound to the event inspector window.  Whenever
# something is dropped in the inspector window (or an action bound to
# an event is dispatched to the window), this procedure is called.
#
# If an event is dropped, eventFormat is called to do the processing.
# If something other than an event is dropped, a message is placed in
# the window inviting the user to drop an event.  The event inspector
# responds to the action "clear" by discarding the event contained
# here.
#
# RETURNS: a formatted event, or "Drop an event here!"
#
# ERRORS: none

proc eventInspector {} {
    global dropvalue
    global action

    if {$action == "clear"} {
        set dropvalue ""
    }

    if {[objectClass $dropvalue] != "#event"} {
	return "Drop an event here!"
    } {
	return [eventFormat $dropvalue]
    }
}

##############################################################################

#*****************************************************************************
#
# eventId - map event id to its name
#
# Given an event id, return the name of the event by consulting
# the event dictionary.  An empty string is returned if the event
# cannot be found.
#
# RETURNS: event name, or ""
#
# ERRORS: none

proc eventIdToName {evtId} {
    global eventDict
    global minUsrEventNum
    global maxUsrEventNum
    if {$evtId >= $minUsrEventNum && $evtId <= $maxUsrEventNum} {
	set usrEvtId [expr $evtId - $minUsrEventNum]
        return [format "user%d" $usrEvtId]
    }
    if [info exists eventDict($evtId)] {
        return [lindex $eventDict($evtId) 0]
    }
    return ""
}

#*****************************************************************************
#
# eventNameToId - map event name to its ID
#
# Given an event name, return the id of the event by consulting
# the event dictionary.  
#
# RETURNS: event Id, or -1 if the event name can't be found in the dictionary.
#
# ERRORS: none

proc eventNameToId {evtName} {
    global wvEventInfo
    global minUsrEventNum
    global maxUsrEventNum
    
    if [regexp {user([0-9]+)} $evtName matchall matchnum] {
	return [expr $matchnum+$minUsrEventNum]
    }

    if [catch {set id $wvEventInfo(id,$evtName)} result] {
        return -1
    } else {
        return $id
    }
}

#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#
# USER EVENT UNPACKING
#

#*****************************************************************************
#
# usrUnpack - produce a formatted list of members of user event buffer
#
# Unpack the user event "usrEvt", using the format "evtFormat."  The
# format is a list of format pairs.  The first element of a format
# pair represents the name of the user event member.  The second
# represents the type.  An example format for a structure that
# contains one integer named "count" and a 6-character string named
# "name" would be: {{count int} {name string<6>}}.
#
# If an unpack procedure for the supplied type is not present, a message
# saying so is substituted for the value.
#
# RETURNS: a formatted list of user-event buffer members.
#
# ERRORS: none

proc usrUnpack {usrEvt evtFormat} {
    global inspectorFieldWidth
    
    # get CPU id to see if we need to swap
    
    set cpuId [lindex $usrEvt 5]

    set result ""
    set eventData [eventArg $usrEvt 1]

    foreach fmt $evtFormat {
	set itemName [lindex $fmt 0]
	set typeString [lindex $fmt 1]

	# See if there's an argument to the type, e.g., "string<6>".

	if [regexp -indices {<.*>} $typeString argPart] { 
	    set itemType [string range $typeString 0\
		[expr {[lindex $argPart 0]-1}]]
	    set typeArg [string trim \
			    [string range $typeString \
			        [lindex $argPart 0] [lindex $argPart 1]] \
		        "<>"]
	} { 
	    set itemType $typeString
	    set typeArg ""
        }

	set typeUnpackRtn unpack$itemType

	# see if the user has provided a procedure to unpack this user event.

	if {[info procs $typeUnpackRtn] != ""} {
	    set formattedArg [format "%*s: %s\n" \
		-$inspectorFieldWidth $itemName [$typeUnpackRtn $typeArg $cpuId eventData]]
	} {
	    set formattedArg [format "%*s: (no unpack procedure for %s)\n" \
		-$inspectorFieldWidth $itemName $itemType]
	}

        append result $formattedArg
    }

    return $result
}

##############################################################################
#
# USER EVENT TYPE UNPACKING PROCEDURES
# 
# These procedures can be used in user event unpacking lists to gather and 
# print data from a user event's buffer.  Each procedure in this section is
# named unpack<type>, and is automatically dispatched whenever <type> is 
# given as the type of a member of a user-defined buffer structure.
#
# Each such routine should take three parameters: the argument to the type
# (e.g., 3 in the type string<3>); the CPU ID where the event was collected
# (this can be useful for byte-order or structure alignment issues), and the
# name of the variable containing the data buffer in ASCII hex.  The caller's
# buffer is modified to remove the bytes consumed by the type formatting.

#*****************************************************************************
#
# unpackstring - unpack string
#
# Unpacking procedure for a string.  The length argument 
# specifies the maximum length of the string.  If length == 0, the 
# string has no maximum length, and characters will be unpacked from 
# the event data until a null is reached.
# 
# RETURNS: unpacked value
#
# ERRORS: none

proc unpackstring {length cpuId data} {
    upvar $data d
    set result ""

    if {[string compare $d ""] == 0} {return}
    if {$length == ""} {set length 0}

    scan $d "%2x" byte
    set d [string range $d 2 end]

    set count 1

    while {$byte} {
	set result [format "%s%c" $result $byte]

	if {$length && $count >= $length} break

        set conv [scan $d "%2x" byte]

	if {$conv == -1} break

        set d [string range $d 2 end]
	incr count
    }

    # consume extra characters after the NUL, if any

    set extraNybbles [expr {2 * ($length - $count)}]
	
    if {$extraNybbles > 0} {
        set d [string range $d $extraNybbles end]
    }

    return $result
}

#*****************************************************************************
#
# unpackint - unpack int
#
# unpacking procedure for a 4-byte signed integer ("int").  The result
# is the decimal representation of the integer.
#
# RETURNS: unpacked value
#
# ERRORS: none

proc unpackint {arg cpuId data} {
    upvar $data d
    set int 0

    scan $d "%8x" int
    set d [string range $d 8 end]

    if [cpuLittleEndian $cpuId] {
        set int [swapTargetInt $int]
    }

    return $int
}

#*****************************************************************************
#
# unpackuint - unpack uint
#
# unpacking procedure for a 4-byte unsigned integer ("uint").
# The result is a hexadecimal string.
#
# RETURNS: unpacked value
#
# ERRORS: none

proc unpackuint {arg cpuId data} { 
    upvar $data d

    return [format "0x%x" [unpackint $arg $cpuId d]]
}

#*****************************************************************************
#
# unpackshort - unpack short
#
# unpacking procedure for a 2-byte signed integer ("short").
# The result is a decimal string.
#
# RETURNS: unpacked value
#
# ERRORS: none

proc unpackshort {arg cpuId data} {
    upvar $data d
    set short 0

    scan $d "%4x" short
    set d [string range $d 4 end]

  
    if [cpuLittleEndian $cpuId] {
        set short [swapTargetShort $short]
    }

    return $short
}

#*****************************************************************************
#
# unpackushort - unpack ushort
#
# unpacking procedure for a 2-byte unsigned integer ("ushort").
# The result is a hexadecimal string.
#
# RETURNS: unpacked value
#
# ERRORS: none

proc unpackushort {arg cpuId data} {
    upvar $data d

    return [format "0x%x" [unpackshort $arg $cpuId d]]
}

#*****************************************************************************
#
# unpackchar - unpack char
#
# unpacking procedure for a character ("char"). The result 
# is a string of the form "'c'", where c is the unpacked character.
#
# RETURNS: unpacked value
#
# ERRORS: none

proc unpackchar {arg cpuId data} { 
    upvar $data d
    set char 0

    scan $d "%2x" char
    set d [string range $d 2 end]

    return [format "'%c'" $char]
}

#*****************************************************************************
#
# unpackbyte - unpack byte
#
# unpacking procedure for an 8-bit signed integer ("byte"). 
# The result is a decimal string.
#
# RETURNS: unpacked value
#
# ERRORS: none

proc unpackbyte {arg cpuId data} {
    upvar $data d
    set byte 0

    scan $d "%2x" byte
    set d [string range $d 2 end]

    return $byte
}

#*****************************************************************************
#
# unpackubyte - unpack ubyte
#
# unpacking procedure for an 8-bit unsigned integer ("ubyte").
# The result is a hexadecimal string.
#
# RETURNS: unpacked value
#
# ERRORS: none

proc unpackubyte {arg cpuId data} { 
    upvar $data d

    return [format "0x%x" [unpackbyte $arg $cpuId d]]
}

proc cpuLittleEndian {cpuId} {
    # return endiannes of target based on cpuId.  This doesn't 
    # take ambi-endian architectures into account.  In this 
    # arrangement we have no direct connection to the target
    # server so we can't have authoritative data.
    return [expr $cpuId >= 20 && $cpuId <= 29 || $cpuId >= 80 && $cpuId <= 89]
}

proc swapTargetShort {short} {
    set lsb [format "%02x" [expr $short & 0xff]]
	set msb [format "%02x" [expr ($short >> 8) & 0xff]]
	return 0x$lsb$msb
}

proc swapTargetInt {int} {
    set lsb [format "%02x" [expr $int & 0xff]]
	set lmsb [format "%02x" [expr ($int >> 8) & 0xff]]
	set mlsb [format "%02x" [expr ($int >> 16) & 0xff]]
	set msb [format "%02x" [expr ($int >> 24) & 0xff]]
    return 0x$lsb$lmsb$mlsb$msb
}

proc wvAnalysisPackAdd {args} {}
    
#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#
# MISCELLANEOUS
#

#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#
# INITIALIZATION
#

# 
# The following command, which will be executed when WindView reads this 
# file, adds the "Event Inspector" item to the "Windows" menu of the main
# window and binds the procedure "eventInspector" to the window that is 
# popped up when that menu button is selected.
#
#windowAdd "Event Inspector" eventInspector

#messagePut "Site Tcl Initialization Complete.\n"

# Set a variable indicating that this file has been loaded.
set __INCWindView.tcl 1

# Source the user's customization files.
#

if {![catch {file exists $env(WIND_BASE)/.wind/eventbase.tcl} result] \
		&& $result} {
    uplevel #0 source $env(WIND_BASE)/.wind/eventbase.tcl
}

#
# eventFormat2 - format events for display in status-bar in nice
# func-call-like way. Input is std Tcl-format of event.
#

proc eventFormat2 {event taskName} {
    # split event up
    set name [lindex $event 0]
    set timeStamp [lindex $event 1]
    set params [lindex $event 2]
    set result "$taskName : $timeStamp : $name ("
    for {set n 0} {$n < [llength $params]} {incr n} {
	if {$n > 0} {
	    append result ", "
	}

        set param_str [lindex $params $n]
        regsub "=" [lindex $params $n] " " param_str

        set p_name [lindex $param_str 0]
        set p_val  [lindex $param_str 1]

        if {$p_val == ""} {
	    append result $p_name
	} else {
	    append result "$p_name=$p_val"
	}
    }
    append result ")"
    return $result
}
