####################################
#
# CLASS DEFINITIONS
#
####################################

class Host {
    constructor {hostname} {}
    destructor  {}

    private common hostlist	;# list of hosts w/ connected msql servers
    set hostlist {}

    private variable name
    private variable dblist

    public method find_dbs  {}
    public method get       {option}
    public proc   get_hostlist  {} {return $hostlist}
}

body Host::constructor { {hostname alcor} } {
    # check to see if an object with connection to host exists
    if {[lsearch -exact $hostlist $hostname] != -1} {
	tk_dialog .err ERROR "Connection to $name already exists" {error} 0 "DISMISS"
	return   -code error "Connection to $name already exists"
    }

    # The msql commands' names are the host names
    # If there's an error ...
    if {[catch {msql connect $hostname $hostname} msg] == 1} {
	tk_dialog .err ERROR "$msg" {error} 0 "DISMISS"
	return   -code error "$msg"
    }
   
    set name $hostname
    lappend hostlist $name
    return
}

body Host::get {option} {
    switch -- $option {
	-name     {return $name}
	-dblist   {return $dblist}
	-hostlist {return $hostlist}
    }
    error "Host::get bad option \"$option\""
}

body Host::destructor {} {
    $name disconnect
    set index [lsearch -exact $hostlist $name]
    if {$index != -1} {
	set hostlist [lreplace $hostlist $index $index]
    }
    return
}


body Host::find_dbs {} {
    # returns: error if error in talking to msql server
    #          number of dbs found if no error

    # Get databases for host
    if {[catch {$name get databases} msg] == 1} {
	tk_dialog .err ERROR "$msg" {error} 0 "DISMISS"
	return -code error $msg
    } else {
	set dblist [lsort $msg]
    }
    return [llength $msg]
}
####################

class Display {
    constructor {hostname} {}
    destructor  {}
    
    private variable host
    private variable db
    private variable table
    private variable id
    private variable edit
    private variable flash
    
    # Store the widget name, value, and indexname (unique name)
    # of field currently in focus before its value is changed.
    private variable focus_value
    private variable focus_widget
    private variable focus_indexname
    
    private common counter 0;	# keep counter to number new displays
    private common displaylist;	# keep array of lists of active displays
    set displaylist(init)  0
    public  proc   get_counter     {}         {return $counter}
    public  proc   get_displaylist {hostname} {return $displaylist($hostname)}

    public method get       {option}
    public method set_it    {option a}
    public method set_edit  {a}
    public method set_db    {dbname}
    public method bind_db   {dbname window}
    public method flash_on  {}
    public method flash_off {}

    public method deiconify  {} {}
    public method withdraw   {} {}
}

body Display::constructor {hostname} {
    # check to see if hostname exists
    set index [lsearch [Host::get_hostlist] $hostname]
    if {$index == -1} {
	tk_dialog .err ERROR "Connection to $hostname does NOT exist" {error} 0 "DISMISS"
	return   -code error "Connection to $hostname does NOT exist"
    }
    
    incr counter
    set host  $hostname
    set db    ""
    set table ""
    set id    $counter
    set edit  normal
    set flash off
    set focus_value ""
    set focus_widget ""
    set focus_indexname ""
    lappend displaylist($host) $counter
    dbWindow $host $counter

    return
}

body Display::get {option} {
    switch -- $option {
	-host    {return $host}
	-db      {return $db}
	-table   {return $table}
	-id      {return $id}
	-window  {return [.fright.n subwidget editors:dbedit:$host$id]}
	-edit    {return $edit}
	-flash   {return $flash}
	-fvalue  {return $focus_value}
	-fwidget {return $focus_widget}
	-fname   {return $focus_indexname}
	-counter {return $counter}
   }
    error "Display::get bad option \"$option\""
}

body Display::set_it {option a} {
    switch -- $option {
	-table   {set table $a; return}
	-fvalue  {set focus_value $a; return}
	-fwidget {set focus_widget $a; return}
	-fname   {set focus_indexname $a; return}
    }
    error "Display::set_it bad option \"$option\""
}

body Display::destructor {} {
    set index [lsearch -exact $displaylist($host) $id]
    if {$index != -1} {
	set displaylist [lreplace $displaylist($host) $index $index]
    }
    
    catch {delete object ${host}_${db}_$table.obj}
    
    destroy [get -window]
    return
}

body Display::set_db {dbname} {
    if {[catch {@scope ::Host $host set database $dbname} msg] == 1} {
	return -code error $msg
    }
    
    # binding to set current database when mouse enters window
    bind [.fright.n subwidget editors:dbedit:$host$id] <Enter> "$this bind_db $dbname %W"
    
    set db $dbname
}

body Display::bind_db {dbname window} {
    if {"$window" == "[get -window]"} {
	if {[catch {@scope ::Host $host set database $dbname} msg] == 1} {
	    tk_dialog .err ERROR "$msg" {error} 0 "DISMISS"
	    return
	}
    }
    return
}

body Display::set_edit {a} {
    if {("[string tolower $a]" == "on")   || \
        ("[string tolower $a]" == "true") || ($a == 1)} {
    	set edit normal
    } elseif {("[string tolower $a]" == "off")   || \
              ("[string tolower $a]" == "false") || ($a == 0)} {
    	set edit disabled
    }
    return
}

body Display::flash_on {} {
    flashOn [get -window].f2.f2.f1.label2
    set flash on
    return
}

body Display::flash_off {} {
    flashOff [get -window].f2.f2.f1.label2
    set flash off
    return
}

####################

class Table {
    constructor {hostname dbname tablename} {}
    destructor  {}
   
    private variable db
    private variable host
    private variable table
    private variable names	;# list of field names
    private variable types	;# list of field types
    private variable sizes	;# list of field sizes
    private variable nulls	;# list of field nulls
    private variable keys 	;# list of field keys
    
    private variable data	;# list of lists
    private variable datarows	;# no. entries/field
    private variable datacols	;# no. fields
    
    private variable index	;# index no. of unique fieldname in "names"
    private variable indexname	;# unique fieldname in "names"
    private variable indextype	;# type of unique fieldname in "names"
    private variable indexvals	;# values of unique fieldname
    
    private variable deletions	;# list of deleted rows
    private variable display	;# host's display number if actually
				 # displayed, else -1
    
    public method get           {options}
    public method set_it        {options a}
    public method find_fields   {}
    public method find_data     {}
}

body Table::constructor {hostname dbname tablename} {
    # Only become an object under certain conditions.
    # First, see if host object exists:
    if {[lsearch -exact [Host::get_hostlist] $hostname] == -1} {
	tk_dialog .err ERROR "No connection to \"$hostname\" msql server exists" {error} 0 "DISMISS"
	return   -code error "No connection to \"$hostname\" msql server exists"
    }
    
    # Next, see if host object has a database called $dbname:
    if {[lsearch -exact [$hostname.obj get -dblist] $dbname] == -1} {
	tk_dialog .err ERROR "No database \"$dbname\" exists" {error} 0 "DISMISS"
	return   -code error "No database \"$dbname\" exists"
    }
    
    # Finally, see if db object has a table called $tablename:
    if {[lsearch -exact [${hostname}_$dbname.obj get -tables] $tablename] == -1} {
	tk_dialog .err ERROR "No table \"$tablename\" exists" {error} 0 "DISMISS"
	return   -code error "No table \"$tablename\" exists"
    }

    set name  ${hostname}_${dbname}_$tablename
    set db    $dbname
    set host  $hostname
    set table $tablename
    set data      {}
    set datarows  {0}
    set datacols  {0}
    set index     {-1}
    set indexname {}
    set indextype {}
    set indexvals {}
    set deletions ""
    set display   {-1}
    
    # Note: although the defined fields in a table may not change, it is
    # possible to delete the table and recreate it with different fields.
    # Thus, this constructor will NOT go out and find the fields. It will
    # be done just before the user needs to use the field values.
    
    return
}

body Table::get {option} {
    switch -- $option {
	-host      {return $host}
	-db        {return $db}
	-table     {return $table}
	-names     {return $names}
	-types     {return $types}
	-sizes     {return $sizes}
	-nulls     {return $nulls}
	-keys      {return $keys}
	-data      {return $data}
	-datarows  {return $datarows}
	-datacols  {return $datacols}
	-index     {return $index}
	-indexname {return $indexname}
	-indextype {return $indextype}
	-indexvals {return $indexvals}
	-deletions {return $deletions}
	-display   {return $display}
    }
    error "Table::get bad option \"$option\""
}

body Table::set_it {option a} {
    switch -- $option {
	-indexvals {set indexvals $a; return}
	-deletions {set deletions $a; return}
	-display   {set display $a; return}
    }
    error "Table::set_it bad option \"$option\""
}

body Table::find_fields {} {
    # returns: error if error in talking to msql server
    #          number of tables found if no error

    # get field definition data
    if {[catch {@scope ::Host $host get fields $table} msg] == 1} {
	return -code error $msg
    } else {
	set fieldinfo $msg
    }
    
    if {$fieldinfo == ""} {return 0}
    
    set index {-1}
    set names {}
    set types {}
    set sizes {}
    set nulls {}
    set keys  {}
   
    # break up each field into 4 components
    for {set i 0} {$i < [llength $fieldinfo]} {incr i} {
	regexp {(^.+) +(CHAR|INT|REAL)\(*([0-9]+)*\)* +([0123])} \
		[lindex $fieldinfo $i] all nm type size nullorkey
	
	if {[expr 1 & $nullorkey] == 1} {
	    set null 1
	} else {
	    set null 0
	}
	if {[expr 2 & $nullorkey] == 2} {
	    set key 2
	} else {
	    set key 0
	}
		
	lappend names $nm
	lappend types $type
	lappend sizes $size
	lappend nulls $null
	lappend keys  $key
    }
    
    # search for "name" or "index" among field-names list & store its index,
    # name and type since they're useful for manipulating rows.
    # index = position that the unique field is in the row data
    #       = -1 if none
    set index     [lsearch -exact $names {name}]
    set indexname {name}
    set indextype [lindex $types $index]
    
    # do the same for "index" if "name" not present
    if {$index == -1} {
	set index     [lsearch -exact $names {index}]
	set indexname {index}
	set indextype [lindex $types $index]
    }
    
    if {$index == -1} {
	set indexname {}
	set indextype {}
    }

    return [llength $fieldinfo]
}

body Table::find_data {} {
    # construct msql query command
    set Fields [join $names ,]
    set querycmd "SELECT $Fields FROM $table"

    # get all data
    if {[catch {@scope ::Host $host query $querycmd} msg] == 1} {
	tk_dialog .err ERROR "Table::find_data: $msg" {error} 0 "DISMISS"
	return
    }

    set data     [@scope ::Host $host get rest]; # list of lists
    set datarows [@scope ::Host $host get rows]; # number of entries for each field
    set datacols [@scope ::Host $host get cols]; # number of fields
    
    if {$data == ""} {
	tk_dialog .err ERROR "Table::find_data: No data in $table" {error} 0 "DISMISS"
	return
    }
    return
}

####################

class Database {
    constructor {dbase dbhost} {}
    destructor  {}
    
    private variable host
    private variable db
    private variable name
    private variable tablelist
    
    public method get {option}
    public method find_tables {}
    public method set_current {dis} {}
}


body Database::constructor {dbase dbhost} {
    # Only become an object under certain conditions.
    # First, see if host object exists:
    if {[lsearch -exact [Host::get_hostlist] $dbhost] == -1} {
	tk_dialog .err ERROR "No connection to \"$dbhost\" msql server exists" {error} 0 "DISMISS"
	return   -code error "No connection to \"$dbhost\" msql server exists"
    }
    
    # Next, see if host object has a database called $dbase:
    if {[lsearch -exact [$dbhost.obj get -dblist] $dbase] == -1} {
	tk_dialog .err ERROR "No database \"$dbase\" exists" {error} 0 "DISMISS"
	return   -code error "No database \"$dbase\" exists"
    }
    
    set name ${dbhost}_$dbase
    set host $dbhost
    set db   $dbase
    set tablelist {}
    
    # Note: we'll find all the tables only when this dbase is displayed
    # and the table widget is used to list the available tables since
    # this info could easily become outdated between this object's creation
    # and when the user wants to find the tables.
    
    return
}

body Database::get {option} {
    switch -- $option {
	-db     {return $db}
	-host   {return $host}
	-name   {return $name}
	-tables {return $tablelist}
    }
    error "Database::get bad option \"$option\""
}

body Database::set_current {dis} {
    # make this the current database for a host/display
    
    if {[catch {@scope ::Host $host set database $db} msg] == 1} {
	tk_dialog .err ERROR "$msg" {error} 0 "DISMISS"
	return
    }
    return
}

body Database::find_tables {} {
    # returns: error if error in talking to msql server
    #          number of tables found if no error

    if {("$host" == "") || ("$db" == "")} {return 0}

    # Get tables for current db in host
    if {[catch {@scope ::Host $host get tables} msg] == 1} {
	# instead of returning an error, return blank list of tables
	set tablelist ""
	return 0
    }
    
    set tablelist [lsort $msg]
    return [llength $msg]
}


##################################################################################
##################################################################################

# Get rid of dangerous behavior. Prevent entry widgets
# changing value without being entered in the database.
bind all <Tab> {}
bind Entry <ButtonRelease-2> {}

####################################
#wm withdraw .
####################################

# Top Level Window 

global env dbeditTop

set dbeditTop [.fright.n subwidget editors:dbedit]

image create photo codaImage \
	-file $env(CODA)/common/images/gif/RCLogo.gif
	
image create photo msqlImage \
	-file $env(CODA)/common/images/gif/msql-pwr.gif
		
frame $dbeditTop.f1 -relief ridge -borderwidth 2 -background white

label $dbeditTop.f1.label1 \
    -image codaImage

label $dbeditTop.f1.label2 \
    -image msqlImage
	
pack $dbeditTop.f1.label1 -pady 1
pack $dbeditTop.f1.label2 -pady 1

frame $dbeditTop.f2 -relief ridge -borderwidth 2

label $dbeditTop.f2.label \
	-background white \
	-foreground blue \
	-anchor center \
	-relief ridge \
	-borderwidth 2 \
	-text "Msql Browser" \
	-font "-*-helvetica-bold-r-normal--20-*-*-*-*-*-*-*"

tixComboBox $dbeditTop.f2.host \
	-command chooseHost \
	-editable true \
	-value localhost \
	-fancy true \
	-label "HOST: " \
	-history false \
	-prunehistory true
	
if {"[array names env MSQL_HOME]" != ""} {
    set dir $env(MSQL_HOME)
    if {[file readable $dir/codaDB.hosts] == 1} {
	set fid [open $dir/codaDB.hosts r]
	set hostdata [read -nonewline $fid]
	set hostlist [split $hostdata \n]
	$dbeditTop.f2.host insert end localhost
	foreach i $hostlist {
	    $dbeditTop.f2.host insert end $i
	}
    } else {
	$dbeditTop.f2.host insert end localhost
    }
} else {
    $dbeditTop.f2.host insert end localhost
}

pack $dbeditTop.f2.label  \
     $dbeditTop.f2.host   \
     -fill x -expand 1
		
pack $dbeditTop.f1 -side left -fill y -anchor nw
pack $dbeditTop.f2 -side left -anchor nw

####################################

# Non-blocking message window
toplevel .warn
wm withdraw .warn
wm geometry .warn 200x80
wm title .warn "MESSAGE"

message .warn.message -cursor watch
pack    .warn.message -fill both -expand 1

####################################

# DATABASE window-generating procedure
proc dbWindow {host dis} {

    #.fright.n subwidget hlist add editors:dbedit:$host$dis -itemtype text \
    #-text "msql $host" -under 0
    .fright.n add editors:dbedit:$host$dis -label "$host"

    
######

    # Text input window for long, character database entries.
    # Associated with a particular display
    toplevel .text_$host$dis
    wm withdraw .text_$host$dis
    wm geometry .text_$host$dis 350x100
    wm title    .text_$host$dis "Text Input for $host #$dis"
    wm protocol .text_$host$dis WM_DELETE_WINDOW "wm withdraw .text_$host$dis"

    tixScrolledText .text_$host$dis.tixst -scrollbar y
    .text_$host$dis.tixst subwidget text config -wrap word

    frame  .text_$host$dis.f -borderwidth 2 -relief groove
    button .text_$host$dis.f.button1 \
	-width 7 \
	-text "APPLY"

    button .text_$host$dis.f.button2 \
	-width 7 \
	-text "DISMISS" \
	-command "wm withdraw .text_$host$dis"

    pack .text_$host$dis.f.button1 .text_$host$dis.f.button2 \
	-side left -padx 20 -fill x -expand 1
    pack .text_$host$dis.f -fill x -side bottom
    pack .text_$host$dis.tixst -fill both -expand 1
	
###### .f2

    frame [.fright.n subwidget editors:dbedit:$host$dis].f2
    
###### .f2.f1

    frame [.fright.n subwidget editors:dbedit:$host$dis].f2.f1
    
    # When focus not in table entry widgets, disable text input window
    bind [.fright.n subwidget editors:dbedit:$host$dis].f2.f1 <FocusIn> "textWinDisable $host $dis"
	   
###### .f2.f1.f1

    frame [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1 -relief groove -borderwidth 2

    label [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.label1 \
	-text "Server $host" \
	-anchor center \
	-relief groove \
	-borderwidth 2 \
	-background gray95
	
    label [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.label2 \
	-text "DATABASE" \
	-anchor center 
    
    # Note: combo box selection is automatically
    # given as the last argument to command
    tixComboBox [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.db \
	-editable false \
	-fancy true \
	-label Database \
	-listcmd "postDatabases  $host $dis" \
	-command "chooseDatabase $host $dis" \
	-options {entry.width 14}

    frame [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.f1 -height 10

    pack [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.label1 \
	    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.db -side left

#    pack [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.label2 \
#	 [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.db \
#	 [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.f1 -fill x

###### .f2.f1.f2

    frame [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2 -relief groove -borderwidth 2
    frame [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.f1 -height 15
    frame [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.f2 -height 15
    frame [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.f3 -height 10

    label [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.label1 -text "TABLE" -height 2

    tixComboBox [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.table \
	-editable false \
	-label Table \
	-fancy true \
	-state disabled \
	-listcmd "dbWindowFindTables  $host $dis [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.table" \
	-command "dbWindowChooseTable $host $dis" \
	-options { entry.width 14 }

    button [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.create \
	-width 6 \
	-text "Create" \
	-command "wm deiconify   .table
		  pack propagate .table.f1.f2.f1.f2.host 0
		  pack propagate .table.f1.f2.f1.f2.db   0
		  pack propagate .table.f1.f2.f2.f2.copy 0"

    tixSelect [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.function \
	-orientation horizontal \
	-radio true \
	-allowzero true

    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.function add copy   -text "Copy" -width 6
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.function add delete -text Delete     -width 6

    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.function subwidget delete config \
	-command "deleteTable $host $dis"
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.function subwidget copy   config \
	-command "copyTable $host $dis"

    tixComboBox [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.tbl \
	    -editable true \
	    -fancy true \
	    -state disabled \
	    -options { entry.width 14 } \
	    -listcmd "dbWindowFindTables  $host $dis [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.tbl"

   [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.tbl subwidget entry config -width 17

    pack [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.table  \
	 [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.create \
	 [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.function \
	 [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.tbl -side left
	 
    pack propagate [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.f1 0

###### .f2.f1.f3

    frame [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3 -relief groove -borderwidth 2
    
    label [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.label1 -text "EDITING" -height 2

    tixSelect [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.editview \
	-orientation horizontal \
	-radio true \
	-allowzero false \
	-command "dbWindowEditView $host $dis"
    
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.editview add edit -text Edit -width 4
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.editview add view -text View -width 4
    
    label [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.label2 -text "ROWS" -height 2

    tixSelect [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.rowedit \
	-orientation horizontal \
	-radio true \
	-allowzero true

    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.rowedit add add      -text +      -width 1
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.rowedit add delete   -text "-"   -width 1
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.rowedit add undelete -text Undo -width 4
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.rowedit subwidget delete   config \
	-command "deleteRow $host $dis"
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.rowedit subwidget add      config \
	-command "addRow $host $dis"
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.rowedit subwidget undelete config \
	-command "undeleteRow $host $dis"
	
    tixControl [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.row \
	    -allowempty false \
	    -integer true \
	    -min 1 \
	    -label "Row" \
	    -options {
	label.width 4
	entry.width 3
    }
    
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.row config -value 1
    
    frame [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.f1 -height 3

    pack [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.label1 \
	 [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.editview \
	 [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.label2 \
	 [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.rowedit \
	 [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.row \
	 [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.f1 -side left
	
    puts " widget [.fright.n subwidget editors:dbedit:$host$dis].f2.f1"

    pack [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1 -anchor nw
    pack [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2 -anchor nw
    pack [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3 -anchor nw


	     
###### .f2.f2

    frame [.fright.n subwidget editors:dbedit:$host$dis].f2.f2 -borderwidth 2
    frame [.fright.n subwidget editors:dbedit:$host$dis].f2.f2.f1
    
    label [.fright.n subwidget editors:dbedit:$host$dis].f2.f2.f1.label1 \
	-text "   FIELD VALUES   " \
	-background gray95 \
	-relief groove \
	-borderwidth 2

    label [.fright.n subwidget editors:dbedit:$host$dis].f2.f2.f1.label2 \
 	-text "   Hit Enter To Save Changes   " \
 	-foreground lightgray \
	-relief groove \
	-borderwidth 2  
    
    pack [.fright.n subwidget editors:dbedit:$host$dis].f2.f2.f1.label1 -anchor nw -side left
    pack [.fright.n subwidget editors:dbedit:$host$dis].f2.f2.f1.label2 -anchor nw -side left -fill x

    tixScrolledWindow [.fright.n subwidget editors:dbedit:$host$dis].f2.f2.data  -scrollbar auto
    
    pack [.fright.n subwidget editors:dbedit:$host$dis].f2.f2.f1   -anchor nw -fill x
    pack [.fright.n subwidget editors:dbedit:$host$dis].f2.f2.data -anchor nw -fill both -expand 1

###### .f2.f3

    frame [.fright.n subwidget editors:dbedit:$host$dis].f2.f3 -width 20

    pack [.fright.n subwidget editors:dbedit:$host$dis].f2.f1 \
	    -side top -anchor nw -fill x
#    pack [.fright.n subwidget editors:dbedit:$host$dis].f2.f3 \
#	    -side bottom -fill y \
#	    -anchor nw
    pack [.fright.n subwidget editors:dbedit:$host$dis].f2.f2 -side bottom -fill both -anchor nw -expand 1
    
###### .f3

    frame [.fright.n subwidget editors:dbedit:$host$dis].f3 -height 20

    pack [.fright.n subwidget editors:dbedit:$host$dis].f3 -fill x
    pack [.fright.n subwidget editors:dbedit:$host$dis].f2 -fill both -expand 1 -anchor nw

    return [.fright.n subwidget editors:dbedit:$host$dis]
}
####################################

proc dbWindowDisplay {arg} {    
    foreach i [Host::get_hostlist] {
	if {("$arg" == "$i") || ("$arg" == "all")} {
	    set displaylist [$i.obj get_dislist]
	    foreach j $displaylist {
		wm deiconify .$i$j
	    }
	}
    }
}
 
proc dbWindowChooseTable {host dis table} {
    chooseTable $host $dis [${host}_$dis.obj get -db] $table
    return
}

proc dbWindowFindTables {host dis widget} {
    postTables $host [${host}_$dis.obj get -db] $widget
    return
}

proc dbWindowExpand {host dis} {
}

proc dbWindowShrink {host dis} {
}

proc dbWindowEditView {host dis button on} {
    # only use 1 of 2 button changes
    if {"$button" == "edit"} {return}
	
    if {$on == 0} {
	viewToEdit $host $dis
    } else {
	wm withdraw .text_$host$dis
	editToView $host $dis
    }
}

####################################

proc chooseHost { {host alcor} } {       
    global dbeditTop
#puts "chooseHost: got in proc"

    # Reject null
    if {$host == ""} {return}

    # See if windows w/ connection to host exists, deiconify, return
    # (Check for display objects)
    set display_objs [info objects -class Display ${host}_*.obj]
    if {"$display_objs" != ""} {
	foreach i $display_objs {
	    $i deiconify
	}
	return
    }
        
    # Wait message    
    .warn.message config -text "Please wait. Attempting to connect to $host"
    wm deiconify .warn
    
    changeCursor watch
    
    # create a host object with a connection to $host
    if {[catch {Host $host.obj $host} msg] == 1} {
	wm withdraw .warn
	changeCursor arrow
	tk_dialog .err ERROR "chooseHost: $msg" {error} 0 "DISMISS"
	# clear host widget
	$dbeditTop.f2.host config -disablecallback 1
	$dbeditTop.f2.host config -value ""
	$dbeditTop.f2.host config -disablecallback 0
	return
    }

    # If there's a connection to the msql server ...
    changeCursor arrow
    wm withdraw .warn

    # add host to HOST: widget's list (prunehistory option is on)
    $dbeditTop.f2.host config -history true
    $dbeditTop.f2.host addhistory $host
    $dbeditTop.f2.host config -history false
   
    # create display object
    set dis [expr 1 + [Display::get_counter]]
    Display ${host}_$dis.obj $host
    
    # add option to table definition window's host menu
    .table.f1.f2.f1.f2.host config -disablecallback 1
    .table.f1.f2.f1.f2.host add command $host
    .table.f1.f2.f1.f2.host config -disablecallback 0
    .table.f1.f2.f1.f2.host subwidget menubutton config -text ""

    return
}

####################################

proc changeCursor {cursor} {
    global dbeditTop
    set hostlist [Host::get_hostlist]
    
    $dbeditTop   config -cursor $cursor
    .table config -cursor $cursor
    
    set display_objs [info objects -class Display]
    foreach i $display_objs {
	set win [$i get -window]
	$win config -cursor $cursor
    }
    update
    return
}

####################################

proc postDatabases {host dis} {
#puts "proc postDatabases"
    if {[catch {$host.obj find_dbs} msg] == 1} {
	[.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.db subwidget listbox delete 0 end
	after 200 tk_dialog .err ERROR "postDatabases: $msg" {error} 0 "DISMISS"
	return
    }
    
    set dblist [$host.obj get -dblist]
    
    if {[llength $dblist] == 0} {
	[.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.db subwidget listbox delete 0 end
	after 200 tk_dialog .err ERROR "postDatabases: No databases found in \"$host\"" {error} 0 "DISMISS"
	return
    }
        
    # Stick databases into widget's listbox
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.db subwidget listbox delete 0 end
    foreach i $dblist {
	[.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.db insert end $i
    }
    return
}

####################################

proc chooseDatabase {host dis db} {
    # find tables associated with the database
    
    if {($host == "") || ($dis == "") || ($db == "")} {return}
    
    set old_db    [${host}_$dis.obj get -db]
    set old_table [${host}_$dis.obj get -table]

    # create a db object if necessary
    if {"[info objects -class Database ${host}_$db.obj]" != "${host}_$db.obj"} {
	if {[catch {Database ${host}_$db.obj $db $host} msg] == 1} {
	    tk_dialog .err ERROR "chooseDatabase: $msg" {error} 0 "DISMISS"
	    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.db config -disablecallback 1
	    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.db config -value "$old_db"
	    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.db config -disablecallback 0
	    return
	}
    }
    
    # set current database value in display object
    if {[catch {${host}_$dis.obj set_db $db} msg] == 1} {
	tk_dialog .err ERROR "chooseDatabase: $msg" {error} 0 "DISMISS"
	[.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.db config -disablecallback 1
	[.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.db config -value "$old_db"
	[.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f1.db config -disablecallback 0
	return
    }
    
    # update object's list of tables
    if {[catch {${host}_$db.obj find_tables} msg] == 1} {
	tk_dialog .err ERROR "chooseDatabase: $msg" {error} 0 "DISMISS"
	return
    } 
    
    # set current table name
    ${host}_$dis.obj set_it -table ""
    
    # get rid of text input window
    wm withdraw .text_$host$dis
   
    # clear table-related data
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.table subwidget listbox delete 0 end
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.table config -disablecallback 1
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.table config -value ""
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.table config -disablecallback 0
    catch {destroy [[.fright.n subwidget editors:dbedit:$host$dis].f2.f2.data subwidget window].f}
    
    # delete old table object if existing
#puts "chooseDatabase: delete ${host}_${old_db}_${old_table}.obj"
    catch {delete object ${host}_${old_db}_${old_table}.obj}

    # enable widgets to select table
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.table config -state normal
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.tbl   config -state normal

    return
}

####################################

proc deleteTable {host dis} {      
    # Get table listed in widget
    set table [[.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.tbl cget -selection]
    if {"$table" == "" } {return}
    
    set button [tk_dialog .err WARNING "The table \"$table\" will be gone forever" \
		{warning} 1 "OK" "CANCEL"]
    if {$button == 1} {return}
    
    set querycmd "DROP TABLE $table"
    
    if {[catch {@scope ::Host $host query $querycmd} msg] == 1} {
	tk_dialog .err ERROR "deleteTable: $msg" {error} 0 "DISMISS"
	return
    }
    
    # clear delete table widget stuff
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.tbl config -value ""
    
    # Clear other table widget stuff if necessary.
    # See if there's a window showing this table -- if so, clear.    
    set tableobjs [info objects -class Table]
    foreach i $tableobjs {
	set display [$i get -display]
	if {("$i" == "${host}_[${host}_$dis.obj get -db]_${table}.obj")} {
	    [.fright.n subwidget editors:dbedit:$host$dis]play.f2.f1.f2.table config -disablecallback 1
	    [.fright.n subwidget editors:dbedit:$host$dis]play.f2.f1.f2.table config -value ""
	    [.fright.n subwidget editors:dbedit:$host$dis]play.f2.f1.f2.table config -disablecallback 0
	    catch {destroy [[.fright.n subwidget editors:dbedit:$host$dis]play.f2.f2.data subwidget window].f}
	    return
	}
    }
    
   return
}

####################################

proc postTables {host db widget} {
    # Get tables for current db in host
    # Can't put up error message windows here or things will crash ?!
    # (bug in the TIX widgets)
    #if {[catch {${host}_$db.obj find_tables} msg] == 1} {
	#$widget subwidget listbox delete 0 end
	#after 200 tk_dialog .err ERROR "postTables: $msg" {error} 0 "DISMISS"
	#return
    #}
    catch {${host}_$db.obj find_tables}
    # Stick tables into widget's listbox
    $widget subwidget listbox delete 0 end
    foreach i [${host}_$db.obj get -tables] {
	$widget insert end $i
    }
    
    return
}

####################################

proc copyTable {host dis} {
    if {($host == "") || ($dis == "")} {return}
    
    # get widget data
    set db       [${host}_$dis.obj get -db]; # current db
    set table    [[.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.table cget -value]
    set newtable [[.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.tbl   cget -selection]
    # Note that "selection" represents what's in the combo box's
    # entry widget, while "value" is the last value selected by
    # the user from the listbox.
        
    if {("$table" == "") || ("$newtable" == "") } {return}
    
    set obj   ${host}_${db}_$table.obj
    set names [$obj get -names]
    set types [$obj get -types]
    set sizes [$obj get -sizes]
    set nulls [$obj get -nulls]
    set keys  [$obj get -keys]
    
    $obj find_data
    set data  [$obj get -data]
    set nrows [$obj get -datarows]; # number of entries for each field
    set ncols [$obj get -datacols]; # number of fields
   
    # Construct command to create "newtable" with same fields as "table"
    set cmd "CREATE TABLE $newtable ("
    for {set i 0} {$i < $ncols} {incr i} {
 	set name [lindex $names $i]
 	set type [lindex $types $i]
 	set size [lindex $sizes $i]
 	set null [lindex $nulls $i]
 	set key  [lindex $keys  $i]
   
	if {"[string toupper $type]" == "CHAR"} {
	    set cmd "$cmd$name CHAR($size)"
	} else {
	    set cmd "$cmd$name $type"
	}	
	
	if {$key == 2} {
	    set cmd "$cmd primary key"
	} elseif {$null == 1} {
	    set cmd "$cmd not null"
	}

	# if last entry skip final comma
	if {[expr $ncols - 1] != $i} {
	    set cmd "$cmd,"
	}
    }
    set cmd "$cmd)"
    
    # create table
    if {[catch {@scope ::Host $host query $cmd} msg] == 1} {
	tk_dialog .err ERROR "copyTable: $msg" {error} 0 "DISMISS"
	return
    }
    
    # write data into new table
    set Fields [join $names ,]
    
    for {set i 0}  {$i < $nrows} {incr i} {
	set rowdata [lindex $data $i]
	set values  {}
	
	for {set j 0} {$j < $ncols}  {incr j} {
	    if {"[string toupper [lindex $types  $j]]" == "CHAR"} {
		lappend values "'[lindex $rowdata $j]'"
	    } else {
		lappend values "[lindex $rowdata $j]"
	    }
	}
	set Values [join $values ,]
    
	set querycmd "INSERT INTO $newtable ($Fields) VALUES ($Values)"
	if {[catch {@scope ::Host $host query $querycmd} msg] == 1} {
	    tk_dialog .err ERROR "$copyTable: msg" {error} 0 "DISMISS"
	    return
	}
    }
    return
}

####################################

proc chooseTable {host dis db table} {
    if {($host == "") || ($dis == "") || ($db == "") || ($table == "")} {return}
    
    # get rid of text input window
    wm withdraw .text_$host$dis
    
    # table before new selection
    set old_table [${host}_$dis.obj get -table]

    # See if there's already a window for this table -- if so, deiconify.
    # Scan through all display objects.
    set display_objs [info objects -class Display ${host}_*.obj]
    foreach obj $display_objs {
	if {"[$obj get -table]" == "$table" && "[$obj get -db]" == "$db"} {
	    # update table w/ new data
	    packData $host $dis $db $table
	   
	    set win [$obj get -window]
	    $obj withdraw
	    $obj deiconify
	
	    # restore table widget on original display
	    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.table config -disablecallback 1
	    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.table config -value "$old_table"
	    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.table config -disablecallback 0
	
	    return
	}
    }


    set obj ${host}_${db}_$table.obj
#puts "chooseTable: create object $obj"
    
    # if object doesn't exist, create
    if {"[info objects -class Table $obj]" != "$obj"} {
      if {[catch {Table ${host}_${db}_$table.obj $host $db $table} msg] == 1} {
	tk_dialog .err ERROR "chooseTable: $msg" {error} 0 "DISMISS"
	[.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.table config -disablecallback 1
	[.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.table config -value "$old_table"
	[.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f2.table config -disablecallback 0
	return
      }
    }	    
    
    # delete old object if necessary
#puts "chooseTable: delete ${host}_${db}_${old_table}.obj"
    catch {delete object ${host}_${db}_${old_table}.obj}
    
    # take care of table object display numbers
    $obj set_it -display $dis
    
    # take care of display object's current table
    ${host}_$dis.obj set_it -table $table
 
    # object now exists so update its list of fields
    set numfields [$obj find_fields]
    if {$numfields == 0} {
	tk_dialog .err ERROR "chooseTable: No fields defined for $table" {error} 0 "DISMISS"
	return
    }
    			
    packData $host $dis $db $table
    return
}

###################################

proc packData {host dis db table} {    
    # find data for this table
    set obj ${host}_${db}_$table.obj
    $obj find_data
    set data  [$obj get -data]
    set nrows [$obj get -datarows]
    set ncols [$obj get -datacols]
    set index [$obj get -index]
    $obj set_it -indexvals {}

    set names [$obj get -names]
    set types [$obj get -types]
    set sizes [$obj get -sizes]
    set nulls [$obj get -nulls]
    set keys  [$obj get -keys]

    set win [[.fright.n subwidget editors:dbedit:$host$dis].f2.f2.data subwidget window].f
    catch {destroy $win}
    frame $win

    frame $win.field
    label $win.field.label -width 3 -text "Row"
    pack  $win.field.label -side left
    
    for {set i 0} {$i < [llength $names]} {incr i} {
	set i1 [expr $i+1]
	label $win.field.label$i1 \
		-width 10 \
		-text "[lindex $names $i]" \
		-padx 5
	pack $win.field.label$i1 -side left
    }    
    pack  $win.field -anchor nw

    # set edit/view state of entry widgets created
    set editstate [${host}_$dis.obj get -edit]

    for {set i 0}  {$i < $nrows} {incr i} {
	set i1 [expr $i+1]
	
	frame $win.row$i1 -background gray[expr 10 + $i*80/$nrows]
	label $win.row$i1.label -width 6 -text "  $i1" -pady 3
	pack  $win.row$i1.label -side left

	set rowdata [lindex $data $i]

	# store list of values of field "name" or "index" if existing
	if {$index > -1} {
	        $obj set_it -indexvals [linsert [$obj get -indexvals] end [lindex $rowdata $index]]
# $obj get -indexvals
	}

	# place data into entry widgets & pack
	for {set j 0} {$j < $ncols} {incr j} {
	    set field [lindex $names $j]
	    set type  [lindex $types $j]
	    set size  [lindex $sizes $j]
	    set j1 [expr $j+1]
	    set widget "$win.row$i1.entry$j1"
	    entry $widget \
	    	-width 10 \
	    	-highlightbackground gray[expr 10 + $i*80/$nrows] \
	    	-highlightcolor red
	    $widget insert end "[lindex $rowdata $j]"
	    # turn off AFTER data placed in widget
	    $widget config -state $editstate
	    bind $widget <ButtonRelease-1> "+  grabValue $widget $host $dis $index $i1 $j1 $db $table $field $type $win.row$i1 $size"
	    bind $widget <ButtonRelease-3> "+    textWin $widget $host $dis $index $i1 $j1 $db $table $field $type $size"
	    bind $widget <Return>          "+ insertData $widget $host $dis $index $i1 $j1 $db $table $field $type"
	    bind $widget <FocusOut> "+ resetData  $widget $host $dis"
	    bind $widget <FocusIn>  "+ updateDelRow $host $dis $i1"
	    bind $widget <KeyPress> "+ ${host}_$dis.obj flash_on"
	    pack $widget -side left -anchor nw
	}
	pack $win.row$i1 -anchor nw
    }
    pack $win -anchor nw
    return
}

####################################

proc grabValue {widget host dis index i1 j1 db table field type widgetRow {size 0}} {
    # Return if Edit/View select button value = "view"
    set editView [[.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.editview cget -value]
    if {"$editView" == "view"} {return}
    
    set obj ${host}_$dis.obj
    
    # save a widget's value only once at focus change
    if {"[$obj get -fwidget]" == "$widget"} {
	return
    }
    
    # get entry widget data before it changes (upon mouse entry)
    $obj set_it -fvalue [$widget get]

    # index = position that field "name" or "index" is in the raw row.    
    # data. Now add 1 cause the label widget comes first in row
    set index [expr $index + 1]
    
    # get unique identifier of same row if any
    if {$index <= 0} {
	$obj set_it -fname {}
    } else {
	$obj set_it -fname [$widgetRow.entry$index get]
    }

    # stick data in global list
    $obj set_it -fwidget $widget
  
    # withdraw text input window if ...
    # change   text input window if ...
    if {("$type" != "CHAR") || ($size < 10)} {
	wm withdraw .text_$host$dis
	return
    } elseif {[winfo ismapped .text_$host$dis] == 1} {
	# update text in window
	textUpLoad $widget $host $dis
	
	wm withdraw  .text_$host$dis
	wm deiconify .text_$host$dis
	
	.text_$host$dis.f.button1 config \
	    -command "textDownLoad $widget $host $dis;
		      insertData   $widget $host $dis $index $i1 $j1 $db $table $field $type"
    }
    
    return
}

####################################

proc textWin {widget host dis index i1 j1 db table field type {size 0}} {
    # Put up an additional window for text entry
    # This routine is called by ButtonPress-3.
    
    set obj ${host}_${db}_$table.obj

    # If the focus is not in widget, set params for focus widget
    set focuswidget [focus -lastfor $widget]
    if {"$focuswidget" != "$widget"} {
    
	# Find name, type & size of column entry under ButtonPress-3
	# to see if we should deiconify text input window.
	if {[regexp {row([0-9]+)\.entry([0-9]+)$} $focuswidget all i j] == 0} {
		return
	}
	set _i1    [expr $i - 1]
	set _j1    [expr $j - 1]
	set fname  [lindex [$obj get -names] $_j1]
	set ftype  [lindex [$obj get -types] $_j1]
	set fsize  [lindex [$obj get -sizes] $_j1]
	if {"$fsize" == ""} {set fsize 0}
		
	if {("$ftype" != "CHAR") || ($fsize < 10)} {return}

	if {[winfo ismapped .text_$host$dis] != 1} {
	    wm geometry  .text_$host$dis +[winfo rootx $widget]+[expr 30+ [winfo rooty $widget]]
	    wm deiconify .text_$host$dis
	} else {
	    # pop window to forefront
	    wm withdraw  .text_$host$dis
	    wm deiconify .text_$host$dis
	}
	
	# when window comes up, make sure its data is up-to-date
	.text_$host$dis.f.button1 config \
	    -command "	textDownLoad $focuswidget $host $dis;
			insertData   $focuswidget $host $dis $index $_i1 $_j1 $db $table $fname $ftype"
		  
	textUpLoad $focuswidget $host $dis

	return
    }
    
    if {("$type" != "CHAR") || ($size < 10)} {return}

    if {[winfo ismapped .text_$host$dis] != 1} {
	wm geometry  .text_$host$dis +[winfo rootx $widget]+[expr 30+ [winfo rooty $widget]]
	wm deiconify .text_$host$dis
    } else {
	# pop window to forefront
	wm withdraw  .text_$host$dis
	wm deiconify .text_$host$dis
    }
    
    # when window comes up, make sure its data is up-to-date
    .text_$host$dis.f.button1 config \
	-command "textDownLoad $widget $host $dis;
		  insertData   $widget $host $dis $index $i1 $j1 $db $table $field $type"
		  
    textUpLoad $widget $host $dis
    
    return
}

####################################

proc textWinDisable {host dis} {
    # disable the text input window    
    .text_$host$dis.tixst subwidget text delete 1.0 end
    .text_$host$dis.f.button1 config -command {}
}

####################################

proc resetData {widget host dis} {    
    if {[string match "[focus -displayof $widget]" ".text_$host$dis*"] == 1} {
	# focus has shifted to text input window
	return
    }

    $widget delete 0 end
    $widget insert end [${host}_$dis.obj get -fvalue]
    ${host}_$dis.obj flash_off
    
    return
}

####################################

proc textDownLoad {widget host dis} {
    # send text data from special text window to entry widget
    
    $widget delete 0 end
    $widget insert end [.text_$host$dis.tixst subwidget text get 1.0 "end -1 chars"]

    return
}

####################################

proc textUpLoad {widget host dis} {
    # send text data from entry widget to special text window 

    .text_$host$dis.tixst subwidget text delete 1.0 end
    .text_$host$dis.tixst subwidget text insert end [$widget get]
        
    return
}

####################################

proc updateDelRow {host dis row} {
    # Update widget showing which row will be deleted.
    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.row config -value $row
    return
    
}

####################################

proc flashOn {widget {color1 red} {color2 lightgray}} {
    global flash
    
    if {[info exists flash($widget)] == 1} {
	if {$flash($widget) > 0} {
	    return
	}
    }
    set flash($widget) 1

    # find fore/background colors & start flashing
    set flash($widget,fcolor) [$widget cget -foreground]
    set flash($widget,bcolor) [$widget cget -background]
    
    flashing $widget "$color1" "$color2"
    return
}
    
proc flashOff {widget} {
    global flash
    
    if {[info exists flash($widget)] == 0} {return}
    
    set flash($widget) 0
    after cancel $flash($widget,id)
    set flash($widget) 0
    $widget config -foreground "$flash($widget,fcolor)"
    $widget config -background "$flash($widget,bcolor)"
    
    return
}

proc flashing {widget color1 color2} {
    global flash
    
    if {[expr $flash($widget)%2] != 0} {
	$widget config -foreground "$color1"
    } else {
	$widget config -foreground "$color2"
    }
    
    if {$flash($widget) < 65000} {
	incr flash($widget)
    } else {
	set flash($widget) 1
    }
    
    set flash($widget,id) [after 800 "flashing $widget \"$color1\" \"$color2\""]
    return
}

####################################

proc tk_focusExplicit {} {
# procedure to undo the tk_focusFollowsMouse bindings
    bind all <Enter> {}
    set old [bind all <Button-1>]
    set script {
	if {("%d" == "NotifyAncestor") || ("%d" == "NotifyNonlinear")
		|| ("%d" == "NotifyInferior")} {
	    if [tkFocusOK %W] {
		focus %W
	    }
	}
    }
    if {$old != ""} {
	bind all <Button-1> "$old; $script"
    } else {
	bind all <Button-1> $script
    }
}

####################################

proc insertData {widget host dis uniqueindex rowindex colindex db table field type} {
    set oldvalue [${host}_$dis.obj get -fvalue]
    set name     [${host}_$dis.obj get -fname]
    set value    [$widget get]
    set obj      ${host}_${db}_$table.obj
    
    if {"$oldvalue" == "$value"} {
	${host}_$dis.obj flash_off
	return
    }

    # if no unique field, can't do anything
    if {$uniqueindex < 0} {
	$widget delete 0 end
	$widget insert end $oldvalue
	return
    }
    
    # if changing the "name" or "index" field, make sure it's unique
    if {$colindex == [expr 1 + $uniqueindex]} {
	set index [lsearch -exact [$obj get -indexvals] $value]
	if {$index != -1} {
	    tk_dialog .err ERROR "insertData: Entry needs to be unique" {error} 0 "DISMISS"
	    $widget delete 0 end
	    $widget insert end $oldvalue
	    return
	}
    }

    # check if input in proper formats
    if {"$type" == "INT"} {
	if {[regexp {^[+-]*[0-9]+$} $value] == 0} {
	    tk_dialog .err ERROR "insertData: Expecting an integer but got \"$value\"" {error} 0 "DISMISS"
	    $widget delete 0 end
	    $widget insert end $oldvalue
	    return
	}
    } elseif {"$type" == "REAL"} {
	if {[regexp {^[+-]*([0-9]+\.*[0-9]*)|(\.[0-9]+)([eE][+-]*[0-9]+)*$} $value] == 0} {
	    tk_dialog .err ERROR "insertData: Expecting a number but got \"$value\"" {error} 0 "DISMISS"
	    $widget delete 0 end
	    $widget insert end $oldvalue
	    return
	}
    } else {
	# put a slash in front of single quotes
	regsub -all ' $value \\' Value
    }

    set uniquefield [$obj get -indexname]
    set uniquetype  [$obj get -indextype]
    if {("$type" == "CHAR") && ("$uniquetype" == "CHAR")} {
	set querycmd "UPDATE $table SET $field='${Value}' WHERE $uniquefield = '$name'"
    } elseif {("$type" == "CHAR") && ("$uniquetype" != "CHAR")} {
	set querycmd "UPDATE $table SET $field='${Value}' WHERE $uniquefield = $name"
    } elseif {("$type" != "CHAR") && ("$uniquetype" == "CHAR")} {
	set querycmd "UPDATE $table SET $field=$value WHERE $uniquefield = '$name'"
   } else {
	set querycmd "UPDATE $table SET $field=$value WHERE $uniquefield = $name"
    }
    
    if {[catch {@scope ::Host $host query $querycmd} msg] == 1} {
	tk_dialog .err ERROR "insertData: $msg" {error} 0 "DISMISS"
	$widget delete 0 end
	$widget insert end $oldvalue
    }
    
    # if changing the "name" or "index" field,
    # update the indexvals list

    if {$colindex == [expr 1 + $uniqueindex]} {
	set rowindex [expr $rowindex - 1]
	$obj set_it -indexvals [lreplace [$obj get -indexvals] $rowindex $rowindex $value]
    }

    # change the stored old value of widget so it doesn't get reset
    ${host}_$dis.obj set_it -fvalue $value
    ${host}_$dis.obj set_it -fname  $name

    # reflect changes in text window
    textUpLoad $widget $host $dis

    ${host}_$dis.obj flash_off
   
    return
}

####################################

proc editToView {host dis} {    
    set win [[.fright.n subwidget editors:dbedit:$host$dis].f2.f2.data subwidget window].f
    ${host}_$dis.obj set_edit off
    
    if {[info commands $win] == ""} {return}
    
    # skip first row of labels
    set slaves($win) [lrange [pack slaves $win] 1 end]
    
    foreach i $slaves($win) {
	# skip first, label widget
	set slaves($i) [lrange [pack slaves $i] 1 end]
	foreach j $slaves($i) {
	    $j config -state disabled
	}
    }
    
    # turn off flashing
    ${host}_$dis.obj flash_off

    return
}

####################################

proc viewToEdit {host dis} {    
    set win [[.fright.n subwidget editors:dbedit:$host$dis].f2.f2.data subwidget window].f
    ${host}_$dis.obj set_edit on

    if {[info commands $win] == ""} {return}
    
    # skip first row of labels
    set slaves($win) [lrange [pack slaves $win] 1 end]
    
    foreach i $slaves($win) {
	# skip first, label widget
	set slaves($i) [lrange [pack slaves $i] 1 end]
	foreach j $slaves($i) {
	    $j config -state normal
	}
    }
    return
}

####################################

proc deleteRow {host dis} {    
    set win [[.fright.n subwidget editors:dbedit:$host$dis].f2.f2.data subwidget window].f
    if {[info commands $win] == ""} {return}

    set db      [${host}_$dis.obj get -db]
    set table   [${host}_$dis.obj get -table]
    set obj     ${host}_${db}_$table.obj
    set names   [$obj get -names]
    set nfields [llength $names]

    [.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.row update
    set row [[.fright.n subwidget editors:dbedit:$host$dis].f2.f1.f3.row cget -value]
    set win [[.fright.n subwidget editors:dbedit:$host$dis].f2.f2.data subwidget window].f
    set slaves($win) [pack slaves $win]
    
    if {$row > [expr [llength $slaves($win)]-1]} {return}
    
    # uniqueindex = position that field "name" or "index" is in the raw row data
    # (add 1 cause the label widget comes first)
    set uniqueindex [$obj get -index]
    set index       [expr 1 + $uniqueindex]
    
    if {$index < 1} {return}
    set widgetrow   [lindex $slaves($win) $row]
    set field       [$widgetrow.entry$index get]
    
    set uniquefield [$obj get -indexname]
    set uniquetype  [$obj get -indextype]
    if {"$uniquetype" == "CHAR"} {
	set querycmd "DELETE FROM $table WHERE $uniquefield = '${field}'"
    } else {
	set querycmd "DELETE FROM $table WHERE $uniquefield = ${field}"
    }
    
    if {[catch {@scope ::Host $host query $querycmd} msg] == 1} {
	tk_dialog .err ERROR "deleteRow: $msg" {error} 0 "DISMISS"
	return
    }
        
    # Save all the values being deleted for future undelete.
    # It's saved in a list of lists
    for {set i 1} {$i <= $nfields} {incr i} {
	lappend valuelist [$widgetrow.entry$i get]
    }
    $obj set_it -deletions [linsert [$obj get -deletions] end $valuelist]
    
    # reset data in global list since widget names are changed
    ${host}_$dis.obj set_it -fvalue  {}
    ${host}_$dis.obj set_it -fname   {}
    ${host}_$dis.obj set_it -fwidget {}
    wm withdraw .text_$host$dis

    packData $host $dis $db $table    
    return
}

####################################

proc undeleteRow {host dis} {    
    set win [[.fright.n subwidget editors:dbedit:$host$dis].f2.f2.data subwidget window].f
    if {[info commands $win] == ""} {return}

    set db    [${host}_$dis.obj get -db]
    set table [${host}_$dis.obj get -table]
    set obj   ${host}_${db}_$table.obj

    # return if nothing there
    if {"[$obj get -deletions]" == ""} {return}
    
    set names   [$obj get -names]
    set types   [$obj get -types]
    set nfields [llength $types]
    
    # construct msql query command; need quotes around strings;
    # find last entry in delete list of lists
    set deleteitem [lindex [$obj get -deletions] end]
    set values {}
    for {set i 0} {$i < $nfields}  {incr i} {
	if {"[lindex $types $i]" == "CHAR"} {
	    lappend values "'[lindex $deleteitem $i]'"
	} else {
	    set v [lindex $deleteitem $i]
	    if {$v == ""} {
	        lappend values 0
	    } else {
	        lappend values $v
	    }
	}
    }
    
    set Values [join $values ,]
    set Fields [join $names ,]    
    set querycmd "INSERT INTO $table ($Fields) VALUES ($Values)"
    
    if {[catch {@scope ::Host $host query $querycmd} msg] == 1} {
	tk_dialog .err ERROR "undeleteRow: $msg" {error} 0 "DISMISS"
	return
    } else {
	# remove item off delete list since it's restored
	$obj set_it -deletions [lreplace [$obj get -deletions] end end]
    }

    # reset data in global list since widget names are changed
    ${host}_$dis.obj set_it -fvalue  {}
    ${host}_$dis.obj set_it -fname   {}
    ${host}_$dis.obj set_it -fwidget {}
    wm withdraw .text_$host$dis

    packData $host $dis $db $table
    return
}

####################################

proc addRow {host dis} {
    set db    [${host}_$dis.obj get -db]
    set table [${host}_$dis.obj get -table]
    set obj   ${host}_${db}_$table.obj
        
    # Construct msql query command. Only need values for not-null fields
    set types   [$obj get -types]
    set names   [$obj get -names]
    set nfields [llength $names]
    set values  {}
    
    # construct msql query command
    set values {}
    for {set i 0} {$i < $nfields}  {incr i} {
	if {"[lindex $types $i]" == "CHAR"} {
	    lappend values "''"
	} else {
	    lappend values "-999"
	}
    }
    set Values [join $values ,]
    set Fields [join $names  ,]
    
    set querycmd "INSERT INTO $table ($Fields) VALUES ($Values)"
    if {[catch {@scope ::Host $host query $querycmd} msg] == 1} {
	tk_dialog .err ERROR "addRow: $msg" {error} 0 "DISMISS"
	return
    }
    
    # reset data in global list since widget names are changed
    ${host}_$dis.obj set_it -fvalue  {}
    ${host}_$dis.obj set_it -fname   {}
    ${host}_$dis.obj set_it -fwidget {}
    wm withdraw .text_$host$dis

    packData $host $dis $db $table
    return
}

#####################################
#
# Row Class
#	This class is used to define all fields in a table.
#	Each Row object also creates a frame with all the data
#	of a single field and the widgets to modify it.
#
#####################################

class Row {
    constructor {} {}
    destructor  {}
    
    private variable number
    private variable widget
    
    public method readRow    {}
    public method writeRow   {datalist}
    
    private common total     0   ;# total number of displayed rows
    private common counter   0   ;# keep counter to number new rows
    private common rowlist   {}  ;# list of displayed-rows' numbers
    private common deletions {}  ;# list of deletions
    
    public proc get         {options}    
    public proc deleteRow   {}
    public proc undeleteRow {}
    public proc storeRowNo  {window}
    public proc moveRows    {rownumber window}
}

###############
body Row::destructor {} {
    set index   [lsearch -exact $rowlist $number]
    set rowlist [lreplace $rowlist $index $index]
    incr total  -1
    pack forget $widget
    destroy  $widget
    return
}

###############
body Row::constructor {} {
    set win [.table.f1.f4.fields subwidget window]
    
    # keep track of rows created
    incr counter		; # number in name of the row's widgets
    incr total			; # total no. of displayed rows
    lappend rowlist $counter	; # list of displayed-rows' numbers
    
    set number $counter
    set widget $win.row$number

    # create a row's widgets
    frame $widget
    
    entry $widget.entry0 \
	-width 4 \
     	-highlightcolor red
     	
    bind $widget.entry0 <Enter>  "+ Row::storeRowNo $widget.entry0"
    bind $widget.entry0 <Leave>  "+ Row::moveRows $number $widget.entry0"
    bind $widget.entry0 <Return> "+ Row::moveRows $number $widget.entry0"
    
    entry $widget.entry1 \
	-width 14 \
	-highlightcolor red

    tixOptionMenu $widget.entry2
    $widget.entry2 add command CHAR
    $widget.entry2 add command REAL
    $widget.entry2 add command INT
    $widget.entry2 config \
	-command "TableDef::setType $widget.entry3"
    $widget.entry2 subwidget menubutton config \
	-highlightcolor red
   
    tixControl $widget.entry3 \
	-integer 1 -max 999 -min 1
    $widget.entry3 subwidget entry config \
	-width 3 -highlightcolor red
    $widget.entry3 config -value 1
   
    tixOptionMenu $widget.entry4
    $widget.entry4 add command 0
    $widget.entry4 add command 1
    $widget.entry4 subwidget menubutton config \
	-highlightcolor red
    $widget.entry4 config \
	-command "TableDef::setNotNull $widget.entry5"

    tixOptionMenu $widget.entry5
    $widget.entry5 add command 0
    $widget.entry5 add command 1
    $widget.entry5 subwidget menubutton config \
	-highlightcolor red
    $widget.entry5 config \
	-command "TableDef::setPrimaryKey $widget.entry5 $widget.entry4"
    
    pack $widget.entry0 \
	 $widget.entry1 \
	 $widget.entry2 \
	 $widget.entry3 \
	 -side left -anchor nw -fill y -expand 1

    pack $widget.entry4 \
	 -side left -anchor nw -fill y -expand 1 -padx 7

    pack $widget.entry5 \
	 -side left -anchor nw -fill y -expand 1

    $widget.entry0 insert end "$total"
    pack $widget -anchor nw -pady 1
    return

}

###############
body Row::get {option} {
    switch -- $option {
	-total     {return $total}
	-counter   {return $counter}
	-rowlist   {return $rowlist}
	-deletions {return $deletions}
   }
    error "Row::get bad option \"$option\""
}

###############
body Row::readRow {} {
    # Read table definition parameters for 1 row and return in a list.
    # returnlist = { name type size null key }
        
    # name
    lappend returnlist [$widget.entry1 get]
    # type
    set widgetVal [$widget.entry2 cget -value]
    lappend returnlist $widgetVal
    # size
    $widget.entry3 update
    if {"$widgetVal" == "CHAR"} {
	lappend returnlist [$widget.entry3 cget -value]
    } else {
	lappend returnlist ""
    }
    # not null
    set widgetVal [$widget.entry4 cget -value]
    if {"$widgetVal" == "1"} {
	lappend returnlist 1
    } else {
	lappend returnlist 0
    }
    # primary key
    set widgetVal [$widget.entry5 cget -value]
    if {"$widgetVal" == "1"} {
	lappend returnlist 2
    } else {
	lappend returnlist 0
    }

    return $returnlist
}

###############

body Row::writeRow {datalist} {
    # datalist is in the form: { name type size null key }
    
    # name
    $widget.entry1 delete 0 end
    $widget.entry1 insert end [lindex $datalist 0]
    # type
    $widget.entry2 config -value [lindex $datalist 1]
    # size
    if {"[string toupper [lindex $datalist 1]]" == "CHAR"} {
	$widget.entry3 config -value [lindex $datalist 2]
    } else {
	$widget.entry3 config -state disabled
    }
    # not null
    $widget.entry4 config -value [lindex $datalist 3]
    # primary key
    if {[lindex $datalist 4] == 2} {
	$widget.entry5 config -value 1
    } else {
	$widget.entry5 config -value 0
    }
    return
}

###############

body Row::deleteRow {} {
    if {$total <= 0} {return}
    
    set win [.table.f1.f4.fields subwidget window]
    
    # row # to be deleted, from widget next to Delete button
    # (actually index to displayed rows' namelist)
    .table.f1.f2.f2.f3.row update
    set rowindex [.table.f1.f2.f2.f3.row cget -value]
    if {$rowindex > $total} {return}
    set rowindex [expr $rowindex - 1]
    
    # row number in name of to-be-deleted-row's frame
    set rownumber [lindex $rowlist $rowindex]
    
    # Save values being deleted in a list for possible undelete.
    set valuelist [r$rownumber.obj readRow]
    lappend deletions $valuelist
    
    # delete object
    delete object r$rownumber.obj
    
    # update all Field# widgets
    set j 0
    foreach i $rowlist {
	incr j
	$win.row$i.entry0 delete 0 end
	$win.row$i.entry0 insert end $j
    }

    return
}

###############

body Row::undeleteRow {} {
    if {[llength $deletions] < 1} {return}
    
    # anticipate its row number and use to name a new row
    set rownum [expr $counter + 1]
    Row ::r$rownum.obj

    # get last row of deleted data
    set data [lindex $deletions end]
    
    # write data to new widgets
    r$rownum.obj writeRow $data
    
    # get rid of last entry to deletions list
    set deletions [lreplace $deletions end end]

    return
}

###############

body Row::storeRowNo {window} {
    global place
    set place [$window get]
    return
}

###############

body Row::moveRows {rownumber window} {
    # rownumber = number in name of a row's widgets
    # row_cur   = current placement of row (#rownumber) in display
    # row_new   = desired placement of row (#rownumber) in display
    global place
    
    set row_cur $place
    set row_new [$window get]
    
    # check if input is acceptable integer
    if {[regexp {^[+]*[1-9][0-9]*$} $row_new] == 0} {
	$window delete 0 end
	$window insert end $row_cur
	return
    }
    
    if {$row_new >  $total} {
	set row_new $total
 	$window delete 0 end
	$window insert end $row_new
    }
    
    if {$row_cur == $row_new} {return}
    
    set win [.table.f1.f4.fields subwidget window]
    
    # find row# of row occupying place this row wants to go
    set index   [expr $row_new - 1]
    set rowToGo [lindex $rowlist $index]
    
    # move rownumber in master list
    set indx    [expr $row_cur - 1]
    set rowlist [lreplace $rowlist $indx $indx]
    set rowlist [linsert $rowlist $index $rownumber]
       
    pack forget $win.row$rownumber
    if {$row_new > $row_cur} {
	pack $win.row$rownumber -after  $win.row$rowToGo -anchor nw -pady 1
    } else {
	pack $win.row$rownumber -before $win.row$rowToGo -anchor nw -pady 1
    }	
	
    # update all Field# widgets
    set j 0
    foreach i $rowlist {
	incr j
	$win.row$i.entry0 delete 0 end
	$win.row$i.entry0 insert end $j
    }
    
    set place [$window get]
    return
}

#########################################
#
#  TableDef Class
#	This class handles all the toplevel window details of the
#	"CODA 2.0 Table Definition" window. There is one such
#	window and it creates a table by having the user define
#	all its parameters. These parameters include host, db, its
#	name, and possibly names of other tables from which to use
#	field definitions.
#
#########################################

#####  Some Globals #####
global tableDef
set    tableDef(host) ""
set    tableDef(db)   ""
#########################

class TableDef {
    constructor {} {}
    destructor  {destroy .table}
    
    private common host        {}
    private common old_host    {}
    private common db          {}
    private common old_db      {}
    private common deletions   {}

    public proc setPrimaryKey  {keyWidget nullWidget key}
    public proc setNotNull     {keyWidget null}
    public proc setType        {sizeWidget type}
    public proc createTable    {}
    public proc addTables      {table}
    public proc postAddTables  {}
    public proc chooseHost     {dummy}
    public proc chooseDatabase {dummy}
    public proc postDatabases  {}
    public proc bindDb         {window}
    public proc get            {option}
}

#########################################
body TableDef::get {option} {
    switch -- $option {
	-db        {return $db}
	-host      {return $host}
	-oldDb     {return $old_db}
	-oldHost   {return $old_host}
	-deletions {return $deletions}
   }
    error "Row::get bad option \"$option\""
}

#########################################
body TableDef::setPrimaryKey {keyWidget nullWidget key} {
    set win [.table.f1.f4.fields subwidget window]
    set rowlist [Row::get -rowlist]

    if {"$key" == "1"} {
	# turn off all other primary key buttons
	foreach i $rowlist {
	    if {"$keyWidget" != "$win.row$i.entry5"} {
		if {"[$win.row$i.entry5 cget -value]" == "1"} {
		    $win.row$i.entry5 config -disablecallback 1
		    $win.row$i.entry5 config -value 0
		    $win.row$i.entry5 config -disablecallback 0
		    break
		}
	    }
	}

	# If primary key selected, select not-null also.
	$nullWidget config -disablecallback 1
	$nullWidget config -value 1
	$nullWidget config -disablecallback 0
    }
    return
}

#########################################
body TableDef::setNotNull {keyWidget null} {
    # If null unselected, set primary key zero also.
    if {"$null" == "0"} {
	$keyWidget config -value 0
    }
}

#########################################
body TableDef::setType {sizeWidget type} {
    if {"$type" == "CHAR"} {
	$sizeWidget config -state normal
    } else {
	$sizeWidget config -state disabled
    }
}

#########################################
body TableDef::createTable {} {
    set table [.table.f1.f2.f1.f2.entry get]
    if {"$table" == ""} {
	tk_dialog .err ERROR "Give the table a name please" {error} 0 "DISMISS"
	return
    }
    
    # construct msql query command
    set rowlist [Row::get -rowlist]
    set length  [llength $rowlist]
    set j 1
    set cmd "CREATE TABLE $table ("
    
    foreach i $rowlist {
	set datalist [r$i.obj readRow]
	set name [lindex $datalist 0]
	set type [lindex $datalist 1]
	set size [lindex $datalist 2]
	set null [lindex $datalist 3]
	set key  [lindex $datalist 4]
	
	# check for valid name
	if {"$name" == ""} {
	    tk_dialog .err ERROR "Missing a name entry" {error} 0 "DISMISS"
	    return
	}

	if {"$type" == "CHAR"} {
	    set cmd "$cmd$name CHAR($size)"
	} else {
	    set cmd "$cmd$name $type"
	}
	
	if {$key == 2} {
	    set cmd "$cmd primary key"
	} elseif {$null == 1} {
	    set cmd "$cmd not null"
	}

	# if last entry skip final comma
	if {$j != $length} {
	    set cmd "$cmd,"
	}
	incr j
    }
    set cmd "$cmd)"

    # create table
    if {[catch {@scope ::Host $host query $cmd} msg] == 1} {
	tk_dialog .err ERROR "$msg: Check name syntax" {error} 0 "DISMISS"
	return
    }
    return
}

##########################################
body TableDef::addTables {table} {        
    if {("$host" == "") || ("$db" == "") || ("$table" == "")} {return}
   
    set obj  ${host}_${db}_$table.obj
    if {"[info objects -class Table $obj]" != "$obj"} {
	# object doesn't exist, so create it
	if {[catch {Table $obj $host $db $table} msg] == 1} {
	    tk_dialog .err ERROR "$msg" {error} 0 "DISMISS"
	    return
	}
    }
    
    # find/update fields
    if {[catch {$obj find_fields} msg] == 1} {
	tk_dialog .err ERROR "$msg" {error} 0 "DISMISS"
	return
    } elseif {$msg == 0} {
	tk_dialog .err ERROR "No Fields in \"$table\"" {error} 0 "DISMISS"
	return
    }
    
    set names [$obj get -names]
    set nfields [llength $names]
    set win     [.table.f1.f4.fields subwidget window]
    
    # keep list of names so as not to duplicate row definitions
    set Names {}
    foreach i [Row::get -rowlist] {
	lappend Names [$win.row$i.entry1 get]
    }

    for {set i 0} {$i < $nfields} {incr i} {
	if {[lsearch -exact $Names [lindex $names $i]] != -1} {
	    continue
	}
	set rownum [expr 1 + [Row::get -counter]]
	Row ::r$rownum.obj
	set data {}
	lappend data [lindex $names $i]
	lappend data [lindex [$obj get -types] $i]
	lappend data [lindex [$obj get -sizes] $i]
	lappend data [lindex [$obj get -nulls] $i]
	lappend data [lindex [$obj get -keys]  $i]
	r$rownum.obj writeRow $data
    }   

    return
}

##########################################
body TableDef::postAddTables {} {
    global tableDef
    set db   $tableDef(db)
    set host $tableDef(host)
   
    
    set obj  ${host}_$db.obj
    if {"[@scope :: info objects -class Database $obj]" != "$obj"} {
	# object doesn't exist, so create it
	if {[catch {@scope :: Database $obj $db $host} msg] == 1} {
	    tk_dialog .err ERROR "$msg" {error} 0 "DISMISS"
	    return 1
	}
    }
#puts "postAddTables: created object"
    # find/update tables
    if {[catch {$obj find_tables} msg] == 1} {
	tk_dialog .err ERROR "$msg" {error} 0 "DISMISS"
	return 1
    } elseif {$msg == 0} {
	    tk_dialog .err ERROR "No tables in \"$db\"" {error} 0 "DISMISS"
	return 1
    }
    
    set Tables [$obj get -tables]
#puts "postAddTables: found tables"
    
    # erase widget's menu
    foreach i [.table.f1.f2.f2.f2.copy entries] {
	.table.f1.f2.f2.f2.copy delete $i
    }
    
    # Stick tables into widget's menu
    .table.f1.f2.f2.f2.copy config -disablecallback 1
    foreach i $Tables {
	.table.f1.f2.f2.f2.copy add command $i
    }
    if {[llength $Tables] == 0} {
	.table.f1.f2.f2.f2.copy add command ""
    }
    .table.f1.f2.f2.f2.copy config -disablecallback 0
    
    return 0
}

##########################################
body TableDef::chooseHost {_host} {
    # if host changes, be sure to update database choices
#puts "chooseHost: new"
    
    # turn on database choice widget
    .table.f1.f2.f1.f2.db config -state normal
    
    if {[postDatabases] == 1} {
	# there's been an error of some sort so reset host
#puts "chooseHost: error in postDatabases, host = $host, oldhost = $old_host"
	set host $old_host
	postDatabases
	return
    }
    
    set old_host $host
#puts "chooseHost: ok, host = $host, oldhost = $old_host"
    return
}

#########################################
body TableDef::chooseDatabase {_db} {
    global tableDef
    
    set host $tableDef(host)
    set db   $_db
    
    if {("$host" == "") || ("$db" == "")} {return}
    
    # set current db for the .table window
    if {[catch {@scope ::Host $host set database $db} msg] == 1} {
	tk_dialog .err ERROR "$msg" {error} 0 "DISMISS"
	set db $old_db
	return
    }
    
    # bind .table entry to set current db
    bind .table <Enter> {TableDef::bindDb %W}
    
    # list associated fields
    if {[postAddTables] == 1} {
	# there's been an error of some sort so reset db
	set db $old_db
	bind .table <Enter> {TableDef::bindDb %W}
	postAddTables
    }    

    set old_db $db	
    return
}

#########################################
body TableDef::bindDb {window} {
    global tableDef
        
    if {("$window" == ".table") && ("$tableDef(db)" != "")} {
	@scope ::Host $host set database $tableDef(db)
    }
}

#########################################
body TableDef::postDatabases {} {
    # We can't return error because this routine is called on
    # selection of a tixOptionMenu widget which can't catch an
    # error. So return 1 as a simulated error for routines
    # like chooseHost which call this rountine.
    
    global tableDef
    set db   $tableDef(db)
    set host $tableDef(host)
#puts "postDatabases: db = $db, host = $host"
   
    # grab current entry value
    set val $db
    foreach i [.table.f1.f2.f1.f2.db entries] {
	.table.f1.f2.f1.f2.db delete $i
    }
	
    if {"[@scope :: info objects -class Host $host.obj]" != "$host.obj"} {
	tk_dialog .err ERROR "No connection to \"$host\" exists" {error} 0 "DISMISS"
	return 1
	
    }
    
    if {[catch {$host.obj find_dbs} msg] == 1} {
	    tk_dialog .err ERROR "$msg" {error} 0 "DISMISS"
 	    return 1
    } elseif {$msg == 0} {
	    #tk_dialog .err ERROR "No databases found in \"$host\"" {error} 0 "DISMISS"
 	    return 1
    }
    
    set dblist [$host.obj get -dblist]
	
    .table.f1.f2.f1.f2.db config -disablecallback 1
    foreach i $dblist {
	.table.f1.f2.f1.f2.db add command $i
    }
    .table.f1.f2.f1.f2.db config -disablecallback 0
    
    # As a default, pick first database as the current database.
    # This is necessary before the next step of getting tables.
    if {"$db" == ""} {
	set firstdb [lindex [.table.f1.f2.f1.f2.db entries] 0]
	catch {@scope ::Host $host set database $firstdb}
    }
    
    # turn on "add tables from" menu
    .table.f1.f2.f2.f2.copy config -state normal

    # update the "add tables from" menu as well
    if {[postAddTables] == 1} {
	return 1
    }    

    # try reseting the db to what it was
    catch {set db $val}
    
    return 0
}
    
########################################################################
body TableDef::constructor {} {
    # Table editing window
    # If it already exists, don't create another
    if {[winfo exists .table] == 1} {error}
    
    # Table editing window

    toplevel .table
    wm withdraw .table
    wm geometry .table 480x550
    wm title .table {CODA 2.0 Table Definition}
    wm protocol .table WM_DELETE_WINDOW {wm withdraw .table}
   
    ###### .f1

    frame .table.f1

    ###### .f1.f1

    frame .table.f1.f1 -relief groove -borderwidth 3
    
    label .table.f1.f1.label \
	-background white \
	-foreground blue \
	-text "CODA 2.0 Table Definition" \
	-anchor center \
	-height 2 \
	-font "-*-helvetica-bold-r-normal--20-*-*-*-*-*-*-*"
	
    pack .table.f1.f1.label -fill x

    ###### .f1.f2

    frame .table.f1.f2 -relief groove -borderwidth 2
    
    ###### .f1.f2.f1

    frame .table.f1.f2.f1 -borderwidth 2 -relief groove

    frame .table.f1.f2.f1.f1 -width 10
    frame .table.f1.f2.f1.f2
    frame .table.f1.f2.f1.f3 -width 10
    
    frame .table.f1.f2.f1.f2.f1 -height 10
    frame .table.f1.f2.f1.f2.f2 -height 10
    frame .table.f1.f2.f1.f2.f3 -height 10
    frame .table.f1.f2.f1.f2.f4 -height 10
    
    label .table.f1.f2.f1.f2.label1 \
	-background gray95 \
	-borderwidth 2 \
	-relief groove \
	-text "HOST" \
	-anchor center
	
    label .table.f1.f2.f1.f2.label2 \
	-background gray95 \
	-borderwidth 2 \
	-relief groove \
	-text "DATABASE" \
	-anchor center
	
    tixOptionMenu .table.f1.f2.f1.f2.host \
	-variable tableDef(host) \
	-command  {TableDef::chooseHost}

    tixOptionMenu .table.f1.f2.f1.f2.db \
	-variable tableDef(db) \
	-state    disabled \
	-command  {TableDef::chooseDatabase}

    .table.f1.f2.f1.f2.db subwidget menu config \
	-postcommand {TableDef::postDatabases}

    label .table.f1.f2.f1.f2.label3 \
	-background gray95 \
	-borderwidth 2 \
	-relief groove \
	-text "TABLE NAME" \
	-anchor center
	
    entry .table.f1.f2.f1.f2.entry -width 15
    
    pack .table.f1.f2.f1.f2.f1 \
	 .table.f1.f2.f1.f2.label1 \
	 .table.f1.f2.f1.f2.host \
	 .table.f1.f2.f1.f2.f2 \
	 .table.f1.f2.f1.f2.label2 \
	 .table.f1.f2.f1.f2.db \
	 .table.f1.f2.f1.f2.f3 \
	 .table.f1.f2.f1.f2.label3 \
	 .table.f1.f2.f1.f2.entry \
	 .table.f1.f2.f1.f2.f4 \
	 -fill x
	 
    pack .table.f1.f2.f1.f1  .table.f1.f2.f1.f3 -side left
    pack .table.f1.f2.f1.f2 \
	-after .table.f1.f2.f1.f1 \
	-side left -fill both -expand 1
	 	 
    ###### .f1.f2.f2

    frame .table.f1.f2.f2 -borderwidth 2 -relief groove

    frame .table.f1.f2.f2.f1 -width 10
    frame .table.f1.f2.f2.f2
    frame .table.f1.f2.f2.f3
    frame .table.f1.f2.f2.f4 -width 10
    
    frame .table.f1.f2.f2.f2.f1 -height 10
    frame .table.f1.f2.f2.f2.f2 -height 10
    frame .table.f1.f2.f2.f2.f3 -height 10

    frame .table.f1.f2.f2.f3.f1 -height 90
    
    label .table.f1.f2.f2.f2.label1 \
	-background gray95 \
	-borderwidth 2 \
	-relief groove \
	-text "FIELDS" \
	-anchor center

    tixSelect .table.f1.f2.f2.f2.rowedit \
	-orientation vertical \
	-radio true \
	-allowzero true

    .table.f1.f2.f2.f2.rowedit add add      -text Add      -width 10
    .table.f1.f2.f2.f2.rowedit add undelete -text Undelete -width 10
    .table.f1.f2.f2.f2.rowedit add delete   -text Delete   -width 10
    
    .table.f1.f2.f2.f2.rowedit subwidget delete   config \
	-command {Row::deleteRow}
    .table.f1.f2.f2.f2.rowedit subwidget add      config \
	-command {Row ::r[expr 1 + [Row::get -counter]].obj}
    .table.f1.f2.f2.f2.rowedit subwidget undelete config \
	-command {Row::undeleteRow}
    
    label .table.f1.f2.f2.f2.label2 \
	-text "add fields from:" \
	-anchor center
	
    tixOptionMenu .table.f1.f2.f2.f2.copy \
	-state   disabled \
	-command {TableDef::addTables}
	
    .table.f1.f2.f2.f2.copy subwidget menu config \
	-postcommand {TableDef::postAddTables}

    pack .table.f1.f2.f2.f2.f1 \
	 .table.f1.f2.f2.f2.label1 \
	 .table.f1.f2.f2.f2.rowedit \
	 -fill x
    pack .table.f1.f2.f2.f2.f2 -fill both -expand 1
    pack .table.f1.f2.f2.f2.label2 \
	 .table.f1.f2.f2.f2.copy \
	 .table.f1.f2.f2.f2.f3 \
	 -fill x

   tixControl .table.f1.f2.f2.f3.row \
	-allowempty false \
	-integer true \
	-min 1
	
    .table.f1.f2.f2.f3.row config -value 1
    .table.f1.f2.f2.f3.row subwidget entry config -width 3

    pack .table.f1.f2.f2.f3.f1 \
	 .table.f1.f2.f2.f3.row

    pack .table.f1.f2.f2.f1 -side left
    pack .table.f1.f2.f2.f3 -side left -fill y
    pack .table.f1.f2.f2.f4 -side left
    pack .table.f1.f2.f2.f2 \
	 -after .table.f1.f2.f2.f1 \
	 -side left -fill both -expand 1
   
    pack .table.f1.f2.f1 .table.f1.f2.f2 -side left -fill both -expand 1
    

    ###### .f1.f4

    frame .table.f1.f4 -relief groove -borderwidth 2
    
    frame .table.f1.f4.f1
    label .table.f1.f4.f1.label1 -text ""    -width 39
    label .table.f1.f4.f1.label2 -text "Not" -width 7
    label .table.f1.f4.f1.label3 -text "Pri" -width 6
    
    
    frame .table.f1.f4.f2 
    label .table.f1.f4.f2.label1 -text "   No." -width 6
    label .table.f1.f4.f2.label2 -text "  Name" -width 16
    label .table.f1.f4.f2.label3 -text "Type"   -width 10
    label .table.f1.f4.f2.label4 -text "Size"   -width 8
    label .table.f1.f4.f2.label5 -text "Null"   -width 7
    label .table.f1.f4.f2.label6 -text "Key"    -width 6

    tixScrolledWindow .table.f1.f4.fields \
	-scrollbar "auto -x" \
	-height 10

    set win   [.table.f1.f4.fields subwidget window]
    bind $win <Enter> "tk_focusFollowsMouse"
    bind $win <Leave> "tk_focusExplicit"
      
    # create the first row object
    Row ::r1.obj
       
    pack .table.f1.f4.f1.label1 \
	 .table.f1.f4.f1.label2 \
	 .table.f1.f4.f1.label3 \
	  -side left
    pack .table.f1.f4.f2.label1 \
	 .table.f1.f4.f2.label2 \
	 .table.f1.f4.f2.label3 \
	 .table.f1.f4.f2.label4 \
	 .table.f1.f4.f2.label5 \
	 .table.f1.f4.f2.label6 \
	  -side left
    pack .table.f1.f4.f1 -anchor nw
    pack .table.f1.f4.f2 -anchor nw
    pack .table.f1.f4.fields -fill both -expand 1 -anchor nw
    
    ###### .f1.f5
    
    frame  .table.f1.f5

    tixButtonBox .table.f1.f5.buttons

    .table.f1.f5.buttons add apply -text "APPLY"
    .table.f1.f5.buttons add quit  -text "QUIT"
    
    .table.f1.f5.buttons subwidget apply config \
	-command {TableDef::createTable} \
	-width 10
    .table.f1.f5.buttons subwidget quit  config \
	-command {wm withdraw .table} \
	-width 10
    
    pack .table.f1.f5.buttons -fill x
        
    ######

    pack .table.f1.f1 \
	 .table.f1.f2 \
	 -fill x
    pack .table.f1.f4 -fill both -expand 1
    pack .table.f1.f5 -fill x
    
    pack .table.f1 -fill both -expand 1

    return
}

TableDef tableDef.obj
