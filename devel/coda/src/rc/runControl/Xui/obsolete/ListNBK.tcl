# Tix Demostration Program
#
# This sample program is structured in such a way so that it can be
# executed from the Tix demo program "widget": it must have a
# procedure called "RunSample". It should also have the "if" statment
# at the end of this file so that it can be run as a standalone
# program using tixwish.

# This program demonstrates the ListBoteBook widget, which is very similar
# to a NoteBook widget but uses an HList instead of page tabs to list the
# pages.
global env
#source $env(CODA)/common/lib/xmon/Dialog.tcl
#source $env(CODA)/common/lib/xmon/cmonitor.tcl
#source $env(CODA)/common/lib/xmon/procview.tcl

proc NoteBook {w} {
    global env
    set top $w
    #----------------------------------------------------------------------
    # Create the ListNoteBook with nice icons
    #----------------------------------------------------------------------
    
    tixNoteBook ${top}.n -ipadx 6 -ipady 6
#    set img0 [tix getimage about]
#    set img1 [tix getimage network]
#    set img2 [tix getimage about]
    ${top}.n add about -label hints
    ${top}.n add editors:dbedit -label dbedit

    # We use these options to set the sizes of the subwidgets inside the
    # notebook, so that they are well-aligned on the screen.
    #
    set name [tixOptionName ${top}.n]
    option add *$name*TixControl*entry.width 10
    option add *$name*TixControl*label.width 18
    option add *$name*TixControl*label.anchor e
    
    set f [${top}.n subwidget about]
    
    tixScrolledText $f.a
    
    $f.a subwidget text insert end {	
About Run Control 
This is a configurable GUI for RunControl. The pane at the left 
is the Motif RunControl interface, this pane is availible from Tcl. 

To place Tk widgets in either of this user configurable 
areas use the following procedure...
	
Generate a file my_name.tcl, make my_name something unique.

This is a "Tix" notebook widget. The widget name is .fright.n,
to add a page to the notebook use the following procedure.

    .fright.n subwidget hlist add my_page -itemtype text \
	    -text "My Page" -under 0
    .fright.n add my_page


You can use tk to add widgets to the new page, the name of the widget
representing the page can by found by...

    set page_widget [.fright.n subwidget my_page]



Once you have your file of Tcl it must be converted into a loadable
modue, run the following commands (assumes Solaris) ...

	tcl2c my_name.tcl > my_name.c
		
	cc -K pic -c -I$CODA/common/include
				
	ld -G -z text -o my_name.so my_name.o
				
	setenv RC_MODULES `pwd`/my_name
				
Now run "runcontrol" again..
	
Someday there will be more help availible at the following URL 
	
	http://alcor.jlab.org/
				
}

    pack $f.a -expand yes -fill both -side left
    pack .fright.n -expand yes -fill both
}

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
	    puts "$env(CODA_BIN)/$program -m $dbasenode -d $experiment -s $session $options"
	    dp_filehandler [open "|$env(CODA_BIN)/$program -m $dbasenode -d $experiment -s $session $options" r+] re fh" 
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
		error
	    }
	}
	if {"$host" == "$node" || "$host" == "localhost"} {
	    puts "starting $program $options" 
	    dp_filehandler [open "|$env(CODA_BIN)/$program -sess $session -msqld_host $dbasenode -name $name -type $obj_ty $options" r+] re fh
	} else {
	    puts "starting $program $options on $host" 
	    rem_exec $host "setenv EXPID $experiment;setenv MYSQL_HOST $dbasenode;$env(CODA_BIN)/$program -ses $session -msqld_host $dbasenode -name $name -type $obj_ty $options"
	}
    }
}



