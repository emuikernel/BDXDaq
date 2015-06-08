if {"[info command TS]"!="TS"} {
    global tcl_modules

    lappend tcl_modules "TS_class.tcl {} {} {\$Id: TS_class.tcl,v 2.8 1998/09/09 17:50:23 abbottd Exp $}"
    
    class TS {
	inherit ROC

    }    
}

