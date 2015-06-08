/* This file has been generated from the following Tcl source file(s): ER_class.tcl
 * on Thu Jun 12 14:53:52 EDT 2008 by boiarino
 */
#include <tcl.h>
static char initCmdER[] =
    "if {\"[info command ER]\"!=\"ER\"} {\n"
    "global tcl_modules\n"
    "\n"
    "lappend tcl_modules \"ER_class.tcl {} {} {\\$Id: ER_class.tcl,v 2.18 2000/01/14 21:50:40 abbottd Exp $}\"\n"
    "class ER {\n"
    "    inherit CODA\n"
    "\n"
    "\n"
    "	method   download        {conf} @ER_download\n"
    "	method   prestart        {}     @ER_prestart\n"
    "	method   go              {}     @ER_go\n"
    "	method   end             {}     @ER_end\n"
    "	method   pause           {}     @ER_pause\n"
    "	method   exit            {}     @ER_exit\n"
    "\n"
    "    constructor      {sess} {CODA::constructor $sess} @ER_constructor\n"
    "    \n"
    "    destructor                      {}\n"
    "}\n"
    "\n"
    "}\n"
    ;
/* End of Tcl code */
/* Init routine to drag this code into our program...*/
int ER_class_Init (Tcl_Interp *interp)
{
    if (Tcl_Eval(interp,initCmdER) != 0) {
    char *val;
    fprintf (stderr, "ERROR:\n      %s\n", interp->result);
    val = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
    fprintf (stderr, "TclStack:\n      %s\n",val);
       return TCL_ERROR;
    }
    return TCL_OK;
}
/* End of C code */
