# Class definition for CODA class.
#
# The CODA class is common to all CODA components and
# contains all of the basic functionality of the CODA
# state transition diagram.
#
# CODA inherits RPC so that each component is an RPC
# server...

class CODA {
    public  variable local_input ""

    public  variable state "unknown"
    public  variable name  "unknown"
    public  variable debug
    public  variable nlongs 
    public  variable nevents 

    public  variable event_rate 0
    public  variable data_rate 0

    public  variable codaid  
    public  variable targetOK 
    public  variable session

    method   status         {{arg ""}}   {}
    method   statusstate    {{arg ""}}   {}
    method   download       {}       {}
    method   prestart       {}       {}
    method   go             {}       {}
    method   end            {}       {}
    method   pause          {}       {}

    method   exit           {}       {}

    method   version        {}       {
	global tcl_modules coda_version
	puts "CODA version $coda_version"
	puts "=========================="
	foreach module $tcl_modules  {
	    puts "module : [lindex $module 0]"
	    if {"[lindex $module 2]" != ""} {
		puts "         Compiled by [lindex $module 2] on [lindex $module 1]"
	    }
	    puts "         CVS info :"
	    set cvs [lindex $module 3]
	    puts "                   File version: [lindex $cvs 2]"
	    puts "                   Commit by   : [lindex $cvs 5]"
	    puts "                   Date        : [lindex $cvs 3]"
	    puts "                   Time        : [lindex $cvs 4]"
	    puts "                   File status : [lindex $cvs 6]"
	}
    }




    method   statistics {} {
	return [list $nevents $event_rate $nlongs $data_rate]
    }

    method   stat_nevents {} {
	return $nevents
    }
    method   stat_event_rate {} {
	return $event_rate
    }
    method   stat_nlongs {} {
	return $nlongs
    }
    method   stat_data_rate {} {
	return $data_rate
    }

    constructor             {sess} { 
	global os_name
	set session $sess

    } @coda_constructor

    destructor              @coda_destructor
}


#
# NOTE: sometimes stop to work, for example typing 'dc6 status'
# have following:
#     Sergey: CODA::status reached
#     Connect: S_errno_ECONNREFUSED
#     database query: MSQL server has gone away
# but ROC works fine and DB updated; do we need 'status' command ???
#

# NOTE: that method will be overloaded by rcServer in the very
#       beginning of 'Download' (see codaCompClnt.cc)

body CODA::status {{arg ""}} { 
    puts "Sergey: CODA::status reached"
    if { "$arg" != "" } {
#set 'state' variable
      $this configure -state $arg
#update 'state' field in database
	  database query "UPDATE process SET state='$arg' WHERE name='$name'"
    }
#Sergey
	database query "SELECT state FROM process WHERE name='$name'"
    set res [database get next]
    set state [lindex $res 0]
    puts "Sergey: state=$state"
#Sergey
    return $state
}


# sergey: statusstate: update variable 'state' only, does not touch database
body CODA::statusstate {{arg ""}} { 
    puts "Sergey: CODA::status reached"
    if { "$arg" != "" } {
#set 'state' variable
      $this configure -state $arg
    }
    return $state
}

######################################################################
# following methods are reloaded in ROC_class.tcl and CDEB_class.tcl #
######################################################################

body CODA::download {} {
puts "download 111"
    status downloaded
puts "download 222"
    return $state
}

body CODA::prestart {} {
    status paused
    return $state
}

body CODA::go {} {
    status active
    return $state
}

body CODA::end {} {
    status downloaded
    return $state
}

body CODA::pause {} {
    status paused
    return $state
}

body CODA::exit {} {
    global os_name

    end
    set state configured
}



