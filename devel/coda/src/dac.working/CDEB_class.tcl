# ITCL class definition for the dummy event builder class

if {"[info command CDEB]"!="CDEB"} {
global tcl_modules

lappend tcl_modules "CDEB_class.tcl {} {} {\$Id: CDEB_class.tcl,v 2.34 1998/11/17 19:14:06 abbottd Exp $}"

class CDEB {

    inherit CODA

            method download        {conf}     {}
            method prestart        {}         {}
            method pause           {}         {}
            method exit            {}         {}

# see deb_component.c
            method c_prestart      {}         @deb_prestart
    private method c_constructor {sess}       @deb_constructor
    private method c_download {conf}          @deb_download
            method go              {}         @deb_go
            method end             {}         @deb_end 
            method shutdown_build  {}         @shutdown_build

    constructor {sess} {CODA::constructor $sess} {
	  c_constructor $sess
    }

    destructor @deb_destructor

}

#
# methods
#

body CDEB::download {conf} {
    c_download $conf
}

body CDEB::prestart {} {
    c_prestart
}

body CDEB::exit {} {
    shutdown_build
}

}











