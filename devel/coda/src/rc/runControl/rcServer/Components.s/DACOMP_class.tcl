class DACOMP { 
    public variable state "unknown"; 
    public variable name "unknown";
    public variable sock ""

    constructor {} {
    }
    destructor {
	dp_atclose $sock delete "puts \"$name has gone away\";$name status dormant"
    }

    method configure {} {
	set name [info namespace tail $this]
	puts "configuring $this"
	if {[catch "DP_ask -timeout 10000 $name status" res]} {
	    set state "dormant"
	} else {
	    set state $res
	    if {![catch "NS_FindServiceByName $name" res]} {
		set sock $res
		dp_atclose $sock append "puts \"$name has gone away\";$name status dormant"
	    }
	}
    }

    method status {{arg ""}} {
	puts "DACOMP_class.tcl method status reached"
	if {$arg!=""} {
	    set state $arg
	} else {
	    if {[catch "DP_ask -timeout 10000 $name status" res]} {
		set state "dormant"
	    } else {
		set state $res
		if {![catch "NS_FindServiceByName $name" res]} {
		    set sock $res
		    dp_atclose $sock append "puts \"$name has gone away\";$name status dormant"
		}
	    }
	}
        return $state 
    } 
}
