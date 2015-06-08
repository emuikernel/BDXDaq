# ITCL class definition for the dummy event builder class
if {"[info command ER]"!="ER"} {
global tcl_modules

lappend tcl_modules "ER_class.tcl {} {} {\$Id: ER_class.tcl,v 2.18 2000/01/14 21:50:40 abbottd Exp $}"
class ER {
    inherit CODA


	method   download        {conf} @ER_download
	method   prestart        {}     @ER_prestart
	method   go              {}     @ER_go
	method   end             {}     @ER_end
	method   pause           {}     @ER_pause
	method   exit            {}     @ER_exit

    constructor      {sess} {CODA::constructor $sess} @ER_constructor
    
    destructor                      {}
}

}
