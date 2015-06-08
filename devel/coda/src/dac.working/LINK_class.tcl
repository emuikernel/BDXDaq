# class to handle data links into and out of
# CODA components..

if {"[info command LINK]"!="LINK"} {
global tcl_modules

lappend tcl_modules "LINK_class.tcl {} {} {\$Id: LINK_class.tcl,v 2.25 1998/07/27 19:17:45 heyes Exp $}"

class LINK {
    constructor {from to {host ""}} {}
    destructor  {}

    method constructor_C {from to {host ""}} @LINK_constructor_C
    method destructor_C  {} @LINK_destructor_C

}

body LINK::destructor {} {
	database query "DELETE FROM links WHERE name='$name'"
    destructor_C
}

body LINK::constructor {from to {host ""}} {
    constructor_C $from $to $host
}

 
}
