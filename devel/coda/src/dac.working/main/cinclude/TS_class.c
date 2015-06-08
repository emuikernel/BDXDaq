/* This file has been generated from the following Tcl source file(s): TS_class.tcl
 * on Thu Jun 12 14:53:52 EDT 2008 by boiarino
 */
#include <tcl.h>
static char initCmdTS[] =
    "if {\"[info command TS]\"!=\"TS\"} {\n"
    "    global tcl_modules\n"
    "\n"
    "    lappend tcl_modules \"TS_class.tcl {} {} {\\$Id: TS_class.tcl,v 2.8 1998/09/09 17:50:23 abbottd Exp $}\"\n"
    "    \n"
    "    class TS {\n"
    "	inherit ROC\n"
    "\n"
    "    }    \n"
    "}\n"
    "\n"
    ;
/* End of Tcl code */
/* Init routine to drag this code into our program...*/
int TS_class_Init (Tcl_Interp *interp)
{
    if (Tcl_Eval(interp,initCmdTS) != 0) {
    char *val;
    fprintf (stderr, "ERROR:\n      %s\n", interp->result);
    val = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
    fprintf (stderr, "TclStack:\n      %s\n",val);
       return TCL_ERROR;
    }
    return TCL_OK;
}
/* End of C code */
