#!/bin/sh
#\
exec $CODA_BIN/dpsh -f "$0" ${1+"$@"}
#

proc fh { m fd} {

    if {[string compare $m e] == 0} {
	dp_filehandler $fd
	close $fd;
	return; 
    }

    set status [gets $fd result]
    if { $status == -1 } {
	dp_filehandler $fd
	if {[catch "close $fd" err]} {
	    puts "ERROR : $err"
	}
	return;
    }
    puts "$result"
}


proc rem_exec {where what} {

    set fd [open "|rsh -n $where $what" r+]
    
    dp_filehandler $fd re fh
}


proc coda_tcl_activate {args} {
    global env
    puts "coda_tcl_activate $args"
    set node       [dp_hostname]
    set logfile  "/dev/null"
    
    set dbasenode     [lindex $args 0]
    set host          [lindex $args 1]
    set experiment    [lindex $args 2]
    set session       [lindex $args 3]
    set class         [lindex $args 4]
    set name          [lindex $args 5]
    
    set options [lrange $args 6 end]
    
    if {"$class" == "RCS"} {
	set program rcServer
	
	if {"$host" == "$node" || "$host" == "localhost"} {
	    puts "starting $program $options" 
	    dp_filehandler [open "|$env(CODA_BIN)/$program -m $dbasenode -d $experiment -s $session $options" r+] re fh
	} else {
	    puts "starting $program $options on $host" 
	    rem_exec $host "$env(CODA_BIN)/$program -m $dbasenode -d $experiment -s $session $options"
	}
    } else {
	switch "$class" {
	    "ANA" { 
		set program coda_gen
		set obj_ty CODA
	    }
	    "EB" {
		set program coda_eb
		set obj_ty CDEB
	    }
	    "CDEB" {
		set program coda_eb
		set obj_ty CDEB
	    }
	    "ER" {
		set program coda_er
		set obj_ty ER
	    }
	    "LOG" {
		set program cmsglog
		set obj_ty LOG
	    }
	    "ROC" {
		set program coda_roc
		set obj_ty ROC
	    }
	    
	    default {
		puts "$class illegal - request ignored"
		error "$class illegal - request ignored"
	    }
	}
	if {"$host" == "$node" || "$host" == "localhost"} {
	    puts "starting $program $options" 
	    exec $env(CODA_BIN)/$program -sess $session -msqld_host $dbasenode -name $name -type $obj_ty $options </dev/null &
	} else {
	    puts "starting $program $options on $host" 
	    rem_exec $host "setenv EXPID $experiment;setenv MYSQL_HOST $dbasenode;$env(CODA_BIN)/$program -ses $session -msqld_host $dbasenode -name $name -type $obj_ty $options"
	}
    }
}

eval "coda_tcl_activate $argv"
exit
