if {"[info command ROC]"!="ROC"} {
    global tcl_modules
    
    lappend tcl_modules "ROC_class.tcl {} {} {\$Id: ROC_class.tcl,v 1.37 1998/11/13 20:10:09 timmer Exp $}"
    
    class ROC {
	inherit CODA

# reload CODA_class methods	
	method   download        {configname} @roc_download
	method   prestart        {}           @roc_prestart
	method   go              {}           @roc_go
	method   end             {}           @roc_end
	method   pause           {}           @roc_pause
	method   exit            {}           @roc_exit

	method   roc_constructor {args}       @roc_constructor

	constructor {sess} {CODA::constructor $sess} {
	    global env
	    roc_constructor $sess
	}

	destructor @roc_destructor

    }

}






