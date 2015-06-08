/* This file has been generated from the following Tcl source file(s): MON_class.tcl
 * on Thu Jun 12 14:53:52 EDT 2008 by boiarino
 */
#include <tcl.h>
static char initCmdMON[] =
    "if {\"[info command MON]\"!=\"MON\"} {\n"
    "global tcl_modules\n"
    "\n"
    "lappend tcl_modules \"MON_class.tcl {} {} {\\$Id: MON_class.tcl,v 2.18 2000/01/14 21:50:40 abbottd Exp $}\"\n"
    "class MON {\n"
    "    inherit CODA\n"
    "\n"
    "\n"
    "	method   download        {conf} @MON_download\n"
    "	method   prestart        {}     @MON_prestart\n"
    "	method   go              {}     @MON_go\n"
    "	method   end             {}     @MON_end\n"
    "	method   pause           {}     @MON_pause\n"
    "	method   exit            {}     @MON_exit\n"
    "\n"
    "    constructor      {sess} {CODA::constructor $sess} @MON_constructor\n"
    "    \n"
    "    destructor                      {}\n"
    "}\n"
    "\n"
    "}\n"
    ;
/* End of Tcl code */
/* Init routine to drag this code into our program...*/
int MON_class_Init (Tcl_Interp *interp)
{
    if (Tcl_Eval(interp,initCmdMON) != 0) {
    char *val;
    fprintf (stderr, "ERROR:\n      %s\n", interp->result);
    val = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
    fprintf (stderr, "TclStack:\n      %s\n",val);
       return TCL_ERROR;
    }
    return TCL_OK;
}
/* End of C code */
