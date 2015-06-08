# ITCL class definition for the dummy monitor process class
if {"[info command MON]"!="MON"} {
global tcl_modules

lappend tcl_modules "MON_class.tcl {} {} {\$Id: MON_class.tcl,v 2.18 2000/01/14 21:50:40 abbottd Exp $}"
class MON {
    inherit CODA


	method   download        {conf} @MON_download
	method   prestart        {}     @MON_prestart
	method   go              {}     @MON_go
	method   end             {}     @MON_end
	method   pause           {}     @MON_pause
	method   exit            {}     @MON_exit

    constructor      {sess} {CODA::constructor $sess} @MON_constructor
    
    destructor                      {}
}

}
